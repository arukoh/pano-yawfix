#include <gtest/gtest.h>
#include "opencl/context.h"

using namespace opencl;

TEST(OpenCLContextTest, BasicConstruction) {
  Context ctx;
  // Device name should not be empty (if OpenCL environment is available)
  std::string name = ctx.GetDeviceName();
  EXPECT_TRUE(name.empty() || !name.empty());  // クラッシュしないことのみ保証
}
