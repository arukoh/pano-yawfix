#include <gtest/gtest.h>
#include <vector>
#include "cpu/yaw_corrector.h"

using namespace cpu;

TEST(YawCorrectorFactoryTest, CreateReturnsCorrectType) {
  auto mpeg2 = YawCorrector::Create(y4m::YUV420ChromaFormat::MPEG2);
  ASSERT_TRUE(mpeg2 != nullptr);
  EXPECT_TRUE(dynamic_cast<YawCorrectorMPEG2*>(mpeg2.get()) != nullptr);

  auto jpeg = YawCorrector::Create(y4m::YUV420ChromaFormat::JPEG);
  ASSERT_TRUE(jpeg != nullptr);
  EXPECT_TRUE(dynamic_cast<YawCorrectorJPEG*>(jpeg.get()) != nullptr);

  auto unknown = YawCorrector::Create(y4m::YUV420ChromaFormat::Unknown);
  EXPECT_TRUE(unknown == nullptr);
}
