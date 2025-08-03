#include "cpu/yaw_corrector.h"
#include <algorithm>
#include <cmath>

namespace cpu {

std::unique_ptr<IYawCorrector> YawCorrector::Create(YUV420ChromaFormat fmt) {
  switch (fmt) {
    case YUV420ChromaFormat::JPEG:
      return std::make_unique<YawCorrectorJPEG>();
    case YUV420ChromaFormat::MPEG2:
      return std::make_unique<YawCorrectorMPEG2>();
    default:
      return nullptr;
  }
}

}  // namespace cpu
