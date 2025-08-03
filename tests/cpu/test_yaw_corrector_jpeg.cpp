#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include "cpu/yaw_corrector.h"
#include "y4m/frame.h"

using namespace cpu;
using y4m::FrameData;

namespace {

// Generate dummy frame for testing
FrameData CreateTestFrame(int width, int height, int frame_number) {
  FrameData frame(width, height, frame_number);
  frame.yuv_data.resize(width * height * 3 / 2);
  // Y: Gradient
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      frame.yuv_data[y * width + x] = static_cast<uint8_t>((x + y) % 256);
    }
  }
  // UV: Fixed value
  int y_size = width * height;
  int uv_size = y_size / 4;
  for (int i = 0; i < uv_size; ++i) {
    frame.yuv_data[y_size + i] = 128;
    frame.yuv_data[y_size + uv_size + i] = 64;
  }
  return frame;
}

TEST(YawCorrectorJPEGTest, NoCorrectionIdentity) {
  int width = 8, height = 4;
  FrameData frame = CreateTestFrame(width, height, 0);
  YawCorrectorJPEG corrector;
  auto result = corrector.Correct(frame, 0.0);
  ASSERT_EQ(result->width, width);
  ASSERT_EQ(result->height, height);
  ASSERT_EQ(result->yuv_data.size(), frame.yuv_data.size());
  // Y: Exact match
  for (int i = 0; i < width * height; ++i) {
    EXPECT_EQ(result->yuv_data[i], frame.yuv_data[i]);
  }
  // UV: Exact match
  for (int i = width * height; i < static_cast<int>(frame.yuv_data.size()); ++i) {
    EXPECT_EQ(result->yuv_data[i], frame.yuv_data[i]);
  }
}

TEST(YawCorrectorJPEGTest, YawShiftedFrame) {
  int width = 8, height = 4;
  FrameData frame = CreateTestFrame(width, height, 0);
  YawCorrectorJPEG corrector;
  double yaw = 3.14159265358979323846 / 2;  // 90度
  auto result = corrector.Correct(frame, yaw);
  ASSERT_EQ(result->width, width);
  ASSERT_EQ(result->height, height);
  // Y: Should have some changes
  int diff_count = 0;
  for (int i = 0; i < width * height; ++i) {
    if (result->yuv_data[i] != frame.yuv_data[i]) ++diff_count;
  }
  EXPECT_GT(diff_count, 0);
}

TEST(YawCorrectorJPEGTest, BoundaryYawWrap) {
  int width = 8, height = 4;
  FrameData frame = CreateTestFrame(width, height, 0);
  YawCorrectorJPEG corrector;
  double yaw = 3.14159265358979323846;  // 180度
  auto result = corrector.Correct(frame, yaw);
  ASSERT_EQ(result->width, width);
  ASSERT_EQ(result->height, height);
  // Y: Should have some changes
  int diff_count = 0;
  for (int i = 0; i < width * height; ++i) {
    if (result->yuv_data[i] != frame.yuv_data[i]) ++diff_count;
  }
  EXPECT_GT(diff_count, 0);
}

}  // namespace
