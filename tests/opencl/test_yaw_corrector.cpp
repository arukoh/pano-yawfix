#include <gtest/gtest.h>
#include <vector>
#include "opencl/yaw_corrector.h"
#include "y4m/frame.h"

using namespace opencl;
using y4m::FrameData;

TEST(OpenCLYawCorrectorFactoryTest, CreateReturnsCorrectType) {
  // JPEG
  auto jpeg = YawCorrector::Create(y4m::YUV420ChromaFormat::JPEG);
  ASSERT_TRUE(jpeg != nullptr);
  EXPECT_TRUE(dynamic_cast<YawCorrectorJPEG*>(jpeg.get()) != nullptr);

  // MPEG2
  auto mpeg2 = YawCorrector::Create(y4m::YUV420ChromaFormat::MPEG2);
  ASSERT_TRUE(mpeg2 != nullptr);
  EXPECT_TRUE(dynamic_cast<YawCorrectorMPEG2*>(mpeg2.get()) != nullptr);

  // Unknown
  auto unknown = YawCorrector::Create(y4m::YUV420ChromaFormat::Unknown);
  EXPECT_TRUE(unknown == nullptr);
}
