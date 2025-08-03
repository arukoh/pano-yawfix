#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "y4m/y4m_types.h"

namespace y4m {

class FrameData {
 public:
  int width;
  int height;
  int frame_number;
  std::vector<uint8_t> yuv_data;
  YUV420ChromaFormat chroma_format = YUV420ChromaFormat::Unknown;

  FrameData(int w, int h, int fn) : width(w), height(h), frame_number(fn) {}
  static std::unique_ptr<FrameData> FromY4MFrame(const Y4MFrame& y4m_frame, const Y4MHeader& header);
  Y4MFrame ToY4MFrame() const;
};

}  // namespace y4m
