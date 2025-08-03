#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace y4m {

// YUV420 chroma subsampling format
enum class YUV420ChromaFormat { JPEG, MPEG2, Unknown };

// Y4M frame data
struct Y4MFrame {
  int frame_number = 0;
  std::vector<uint8_t> yuv_data;
};

// Y4M header information
struct Y4MHeader {
  int width = 0;
  int height = 0;
  int framerate_num = 0;
  int framerate_den = 0;
  std::string chroma;
  YUV420ChromaFormat chroma_format = YUV420ChromaFormat::Unknown;
};

}  // namespace y4m
