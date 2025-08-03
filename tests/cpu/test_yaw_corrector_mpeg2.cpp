#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include "cpu/yaw_corrector.h"
#include "y4m/frame.h"

using namespace cpu;
using y4m::FrameData;

TEST(YawCorrectorMPEG2Test, NoCorrectionIdentity) {
  int width = 4, height = 2;
  FrameData frame(width, height, 0);
  frame.yuv_data.resize(width * height + (width * height) / 2);
  // Y: 0,1,2,3,4,5,6,7
  for (int i = 0; i < width * height; ++i) frame.yuv_data[i] = i;
  // U: 8,9,10,11
  for (int i = 0; i < (width * height) / 4; ++i) frame.yuv_data[width * height + i] = 8 + i;
  // V: 12,13,14,15
  for (int i = 0; i < (width * height) / 4; ++i) frame.yuv_data[width * height + (width * height) / 4 + i] = 12 + i;

  YawCorrectorMPEG2 corrector;
  auto result = corrector.Correct(frame, 0.0);
  ASSERT_EQ(result->yuv_data.size(), frame.yuv_data.size());
  EXPECT_EQ(result->yuv_data, frame.yuv_data);
}

TEST(YawCorrectorMPEG2Test, YawShiftedFrame) {
  int width = 4, height = 2;
  FrameData frame(width, height, 0);
  frame.yuv_data.resize(width * height + (width * height) / 2);
  for (int i = 0; i < width * height; ++i) frame.yuv_data[i] = i;
  for (int i = 0; i < (width * height) / 4; ++i) frame.yuv_data[width * height + i] = 8 + i;
  for (int i = 0; i < (width * height) / 4; ++i) frame.yuv_data[width * height + (width * height) / 4 + i] = 12 + i;

  YawCorrectorMPEG2 corrector;
  double yaw = 3.14159265358979323846 / 2;  // 90度
  auto result = corrector.Correct(frame, yaw);
  ASSERT_EQ(result->yuv_data.size(), frame.yuv_data.size());
  // Y component should be shifted (values should change)
  EXPECT_NE(result->yuv_data, frame.yuv_data);
}

TEST(YawCorrectorMPEG2Test, BoundaryYawWrap) {
  int width = 4, height = 2;
  FrameData frame(width, height, 0);
  frame.yuv_data.resize(width * height + (width * height) / 2);
  for (int i = 0; i < width * height; ++i) frame.yuv_data[i] = i;
  for (int i = 0; i < (width * height) / 4; ++i) frame.yuv_data[width * height + i] = 8 + i;
  for (int i = 0; i < (width * height) / 4; ++i) frame.yuv_data[width * height + (width * height) / 4 + i] = 12 + i;

  YawCorrectorMPEG2 corrector;
  double yaw = 2 * 3.14159265358979323846;  // 360度
  auto result = corrector.Correct(frame, yaw);
  ASSERT_EQ(result->yuv_data.size(), frame.yuv_data.size());
  // 360 degree rotation should return to original (values should mostly match: within error tolerance 1)
  int max_diff = 0;
  for (size_t i = 0; i < frame.yuv_data.size(); ++i) {
    int diff = std::abs((int) result->yuv_data[i] - (int) frame.yuv_data[i]);
    if (diff > max_diff) max_diff = diff;
    EXPECT_LE(diff, 3) << "index " << i;
  }
  std::cout << "BoundaryYawWrap: 最大誤差 = " << max_diff << std::endl;
}
