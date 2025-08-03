#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include "temp_utils.h"
#include "y4m/frame.h"
#include "yawfix.h"

using y4m::FrameData;

TEST(YawFixTest, ProcessFrameBasic) {
  const std::string corr_path = create_temp_file_in_unique_subdir("YawFixTest_ProcessFrameBasic.txt");
  {
    std::ofstream ofs(corr_path);
    ofs << "# Test correction value file\n";
    ofs << "0.0\n0.01\n0.02\n0.03\n0.04\n0.05\n";
  }

  YawCorrectionSet corr_set(corr_path);
  YawFix filter(corr_set);
  FrameData frame(8, 4, 0);
  frame.yuv_data.resize(8 * 4 * 3 / 2);
  auto result = filter.ProcessFrame(frame);
  EXPECT_EQ(result->width, 8);
  EXPECT_EQ(result->height, 4);

  // Cleanup
  std::remove(corr_path.c_str());
}
