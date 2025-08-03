#include <gtest/gtest.h>
#include <string>
#include "opencl/yaw_corrector.h"

using namespace opencl;

TEST(YawCorrectorMPEG2Test, KernelSourceContainsMainLogic) {
  YawCorrectorMPEG2 corrector;
  const char* src = corrector.GetKernelSource();
  ASSERT_NE(src, nullptr);
  std::string kernel(src);
  // Check if main function names and coordinate transformation logic are included
  EXPECT_NE(kernel.find("__kernel void yaw_correction"), std::string::npos);
  EXPECT_NE(kernel.find("get_global_id(0)"), std::string::npos);
  EXPECT_NE(kernel.find("longitude = (2.0f * x / (float)width - 1.0f) * PI;"), std::string::npos);
  EXPECT_NE(kernel.find("output[idx] = input[src_idx];"), std::string::npos);
  EXPECT_NE(kernel.find("UV: MPEG2/top-left sampling"), std::string::npos);
}

TEST(YawCorrectorMPEG2Test, KernelSourceIsValidCL) {
  YawCorrectorMPEG2 corrector;
  const char* src = corrector.GetKernelSource();
  ASSERT_NE(src, nullptr);
  std::string kernel(src);
  // Check if minimum OpenCL syntax is included
  EXPECT_NE(kernel.find("__global"), std::string::npos);
  EXPECT_NE(kernel.find("float yaw_radians"), std::string::npos);
  EXPECT_NE(kernel.find("max(0, min(uv_width - 1, uv_src_x))"), std::string::npos);
}
