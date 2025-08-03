#include <gtest/gtest.h>
#include <vector>
#include "y4m/frame.h"

using namespace y4m;

TEST(FrameDataTest, BasicConstruction) {
  int width = 16, height = 8, frame_number = 42;
  FrameData frame(width, height, frame_number);
  EXPECT_EQ(frame.width, width);
  EXPECT_EQ(frame.height, height);
  EXPECT_EQ(frame.frame_number, frame_number);
  EXPECT_EQ(frame.chroma_format, YUV420ChromaFormat::Unknown);
}

TEST(FrameDataTest, YUVDataResize) {
  FrameData frame(4, 2, 0);
  frame.yuv_data.resize(12);
  EXPECT_EQ(frame.yuv_data.size(), 12);
  for (size_t i = 0; i < frame.yuv_data.size(); ++i) {
    frame.yuv_data[i] = static_cast<uint8_t>(i);
  }
  for (size_t i = 0; i < frame.yuv_data.size(); ++i) {
    EXPECT_EQ(frame.yuv_data[i], i);
  }
}
