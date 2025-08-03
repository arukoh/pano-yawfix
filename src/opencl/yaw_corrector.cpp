#include "opencl/yaw_corrector.h"

namespace opencl {

std::unique_ptr<IYawCorrector> YawCorrector::Create(y4m::YUV420ChromaFormat fmt) {
  switch (fmt) {
    case y4m::YUV420ChromaFormat::JPEG:
      return std::make_unique<YawCorrectorJPEG>();
    case y4m::YUV420ChromaFormat::MPEG2:
      return std::make_unique<YawCorrectorMPEG2>();
    default:
      return nullptr;
  }
}

}  // namespace opencl
