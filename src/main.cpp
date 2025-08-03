#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#endif

#include "progress_reporter.h"
#include "version.h"
#include "y4m/frame.h"
#include "y4m/parser.h"
#include "yawfix.h"

void print_usage(std::ostream& os, const char* progname) {
  os << "\nUsage: " << progname << " --corrections <file> [options]\n"
     << "\n[Required Parameters]\n"
     << "  --corrections, -c <file>    Correction values file (CSV/Text)\n"
     << "\n[Main Options]\n"
     << "  --opencl-mode <mode>        OpenCL usage mode (auto|force|disable)\n"
     << "  --report, -r <target>       Target name for detailed log output\n"
     << "  --verbose, -v               Detailed log output (progress/error/info in JSON to stderr)\n"
     << "\n[Other Options]\n"
     << "  --help, -h                  Show this help\n"
     << "  --version, -V               Show version information\n"
     << "\n[Examples]\n"
     << "  # Standard processing\n"
     << "  ffmpeg -i input.mp4 -f yuv4mpegpipe - | " << progname << " --corrections corrections.txt | \\\n"
     << "    ffmpeg -f yuv4mpegpipe -i - -c:v libx264 -crf 23 output.mp4\n"
     << "\n[Correction File Format Example]\n"
     << "  # 1 line per frame (in radians)\n"
     << "  0.0\n 0.0174533\n 0.0349066\n ...\n"
     << "\n[Error Output Example (stderr)]\n"
     << "  {\"type\":\"error\",\"code\":1001,\"message\":\"Correction file not found\"}\n"
     << "\n[Progress Output Example (stderr)]\n"
     << "  {\"type\":\"progress\",\"target\":\"stream\",\"frame\":120,\"percent\":12}\n"
     << std::endl;
}

void print_version(const char* progname) {
  std::cout << progname << " version " << PANO_YAWFIX_VERSION_STRING << "\n"
            << "Copyright (c) 2025 arukoh\n"
            << "License: MIT\n"
            << std::endl;
}

int main(int argc, char* argv[]) {
// Set stdin / stdout to binary mode on Windows
#ifdef _WIN32
  setmode(_fileno(stdin), O_BINARY);
  setmode(_fileno(stdout), O_BINARY);
#endif

  const char* progname = argv[0];
  std::string corrections_file;
  bool use_opencl = true;
  std::string report = "stream";
  bool verbose = false;
  bool show_help = false;
  bool show_version = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-c" || arg == "--corrections") && i + 1 < argc) {
      corrections_file = argv[++i];
    } else if (arg == "--opencl-mode" && i + 1 < argc) {
      std::string mode = argv[++i];
      if (mode == "disable") use_opencl = false;
    } else if (arg == "-r" || arg == "--report") {
      report = argv[++i];
    } else if (arg == "-v" || arg == "--verbose") {
      verbose = true;
    } else if (arg == "-h" || arg == "--help") {
      show_help = true;
    } else if (arg == "-V" || arg == "--version") {
      show_version = true;
    }
  }

  if (show_version) {
    print_version(progname);
    return 0;
  }

  if (show_help) {
    print_usage(std::cout, progname);
    return 0;
  }
  if (corrections_file.empty()) {
    std::cerr << "[stream] Correction file (--corrections) is required\n";
    print_usage(std::cerr, progname);
    return 1;
  }

  try {
    YawCorrectionSet corrections(corrections_file);
    size_t frame_count = corrections.size();
    YawFix filter(corrections);
    ProgressReporter reporter(report, frame_count, verbose);
    if (use_opencl) {
      if (!filter.InitializeOpenCL()) {
        reporter.Info("Failed to initialize OpenCL. Falling back to CPU processing.");
        use_opencl = false;
      }
    }

    y4m::Parser parser;
    std::string header_line;
    if (!std::getline(std::cin, header_line)) {
      reporter.Error(1002, "Failed to read Y4M header (stdin)");
      return 1;
    }
    if (!parser.ParseHeader(header_line)) {
      reporter.Error(1003, std::string("Failed to parse Y4M header: ") + header_line);
      return 1;
    }
    if (!parser.WriteHeader(std::cout)) {
      reporter.Error(1004, "Failed to write Y4M header (stdout)");
      return 1;
    }

    y4m::Y4MFrame input_frame;
    int processed = 0, failed = 0;
    while (parser.ReadFrame(std::cin, input_frame)) {
      auto frame_data = y4m::FrameData::FromY4MFrame(input_frame, parser.GetHeader());
      if (!frame_data) {
        ++failed;
        reporter.Error(2001, "Failed to generate frame data");
        continue;
      }
      try {
        auto corrected = filter.ProcessFrame(*frame_data);
        if (!corrected) {
          ++failed;
          reporter.Error(2002, "Failed to process frame correction");
          continue;
        }
        auto y4m_out = corrected->ToY4MFrame();
        if (!parser.WriteFrame(std::cout, y4m_out)) {
          ++failed;
          reporter.Error(2003, "Failed to write Y4M frame");
          continue;
        }
        ++processed;
        reporter.Report(processed);
      } catch (const YawFixException& ex) {
        ++failed;
        reporter.Error(ex.code, ex.what());
        continue;
      }
    }
    reporter.Info(std::string("Done: Success ") + std::to_string(processed) + " Failed " + std::to_string(failed));
    return (processed > 0) ? 0 : 1;
  } catch (const std::exception& e) {
    ProgressReporter reporter(report, 0, verbose);
    reporter.Error(9999, std::string("Exception: ") + e.what());
    return 1;
  }
  return 1;
}
