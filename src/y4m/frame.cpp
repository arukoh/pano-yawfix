#include "y4m/frame.h"
#include <memory>

namespace y4m {

std::unique_ptr<FrameData> FrameData::FromY4MFrame(const Y4MFrame& y4m_frame, const Y4MHeader& header) {
  if (y4m_frame.yuv_data.empty()) return nullptr;
  auto frame = std::make_unique<FrameData>(header.width, header.height, y4m_frame.frame_number);
  frame->yuv_data = y4m_frame.yuv_data;
  frame->chroma_format = header.chroma_format;
  return frame;
}

Y4MFrame FrameData::ToY4MFrame() const {
  Y4MFrame y4m_frame;
  y4m_frame.frame_number = frame_number;
  y4m_frame.yuv_data = yuv_data;
  return y4m_frame;
}

}  // namespace y4m
