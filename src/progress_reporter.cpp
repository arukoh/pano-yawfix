#include "progress_reporter.h"
#include <iomanip>

ProgressReporter::ProgressReporter(const std::string& target, int total_frames, bool verbose)
    : target_(target), total_frames_(total_frames), last_percent_(-1), verbose_(verbose) {}

void ProgressReporter::Reset(const std::string& target, int total_frames, bool verbose) {
  std::lock_guard<std::mutex> lock(mutex_);
  target_ = target;
  total_frames_ = total_frames;
  last_percent_ = -1;
  verbose_ = verbose;
}

void ProgressReporter::Report(int current_frame) {
  if (!verbose_) return;
  if (total_frames_ <= 0) return;
  int percent = static_cast<int>(100.0 * current_frame / total_frames_);
  std::lock_guard<std::mutex> lock(mutex_);
  if (percent > last_percent_) {
    last_percent_ = percent;
    std::cerr << "{\"type\":\"progress\",\"target\":\"" << target_ << "\",\"frame\":" << current_frame
              << ",\"percent\":" << percent << "}" << std::endl;
  }
}

void ProgressReporter::Info(const std::string& message) {
  if (!verbose_) return;
  std::lock_guard<std::mutex> lock(mutex_);
  std::cerr << "{\"type\":\"info\",\"target\":\"" << target_ << "\",\"message\":\"" << message << "\"}" << std::endl;
}

void ProgressReporter::Error(int code, const std::string& message) {
  if (!verbose_) return;
  std::lock_guard<std::mutex> lock(mutex_);
  std::cerr << "{\"type\":\"error\",\"target\":\"" << target_ << "\",\"code\":" << code << ",\"message\":\"" << message
            << "\"}" << std::endl;
}
