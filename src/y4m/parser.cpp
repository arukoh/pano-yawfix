#include "y4m/parser.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace y4m {

YUV420ChromaFormat ParseChromaFormat(const std::string& chroma) {
  if (chroma == "420jpeg") return YUV420ChromaFormat::JPEG;
  if (chroma == "420mpeg2") return YUV420ChromaFormat::MPEG2;
  // ffmpeg output is C420mpeg2
  if (chroma == "C420jpeg") return YUV420ChromaFormat::JPEG;
  if (chroma == "C420mpeg2") return YUV420ChromaFormat::MPEG2;
  return YUV420ChromaFormat::Unknown;
}

Parser::Parser() {}

bool Parser::ParseHeader(const std::string& header_line) {
  std::istringstream iss(header_line);
  std::string token;
  if (!(iss >> token) || token != "YUV4MPEG2") return false;
  while (iss >> token) {
    if (token[0] == 'W')
      header_.width = std::stoi(token.substr(1));
    else if (token[0] == 'H')
      header_.height = std::stoi(token.substr(1));
    else if (token[0] == 'F') {
      auto pos = token.find(":");
      if (pos != std::string::npos) {
        header_.framerate_num = std::stoi(token.substr(1, pos - 1));
        header_.framerate_den = std::stoi(token.substr(pos + 1));
      }
    } else if (token[0] == 'C') {
      header_.chroma = token.substr(1);
      header_.chroma_format = ParseChromaFormat(header_.chroma);
    } else if (token[0] == 'X') {
      // X extension fields are completely ignored
      continue;
    }
  }
  return header_.width > 0 && header_.height > 0;
}

bool Parser::WriteHeader(std::ostream& os) const {
  if (header_.width <= 0 || header_.height <= 0) return false;
  os << "YUV4MPEG2 " << "W" << header_.width << " " << "H" << header_.height << " " << "F" << header_.framerate_num
     << ":" << header_.framerate_den << " " << "C" << header_.chroma << std::endl;
  return true;
}

bool Parser::ReadFrame(std::istream& is, Y4MFrame& frame) {
  std::string line;
  if (!std::getline(is, line) || line != "FRAME") return false;
  size_t y_size = header_.width * header_.height;
  size_t uv_size = y_size / 2;  // YUV420P
  frame.yuv_data.resize(y_size + uv_size);
  is.read(reinterpret_cast<char*>(frame.yuv_data.data()), y_size + uv_size);
  if (!is) return false;
  frame.frame_number++;
  return true;
}

bool Parser::WriteFrame(std::ostream& os, const Y4MFrame& frame) const {
  os << "FRAME" << std::endl;
  os.write(reinterpret_cast<const char*>(frame.yuv_data.data()), frame.yuv_data.size());
  return os.good();
}

const Y4MHeader& Parser::GetHeader() const {
  return header_;
}

}  // namespace y4m
