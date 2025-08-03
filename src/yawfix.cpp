#include "yawfix.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "cpu/yaw_corrector.h"
#include "opencl/yaw_corrector.h"

class YawFix::Impl {
 public:
  Impl(const YawCorrectionSet& corrections) : corrections_(corrections) {}

  const YawCorrectionSet& corrections_;
  bool opencl_available_ = false;

  std::unique_ptr<FrameData> ProcessFrame(const FrameData& frame);
  double GetYawCorrectionForFrame(int frame_number) const;
  std::unique_ptr<FrameData> ProcessFrameOpenCL(const FrameData& frame, double yaw_radians);
};

YawFix::YawFix(const YawCorrectionSet& corrections) : pimpl_(std::make_unique<Impl>(corrections)) {}

YawFix::~YawFix() = default;

std::unique_ptr<FrameData> YawFix::ProcessFrame(const FrameData& frame) {
  return pimpl_->ProcessFrame(frame);
}

bool YawFix::InitializeOpenCL() {
  // chroma_format is unknown at initialization, so initialize as JPEG temporarily
  auto corrector = opencl::YawCorrector::Create(y4m::YUV420ChromaFormat::JPEG);
  if (!corrector) return false;
  pimpl_->opencl_available_ = corrector->Initialize();
  return pimpl_->opencl_available_;
}

bool YawFix::IsOpenCLAvailable() const {
  return pimpl_->opencl_available_;
}

std::unique_ptr<FrameData> YawFix::Impl::ProcessFrame(const FrameData& frame) {
  double yaw_radians = GetYawCorrectionForFrame(frame.frame_number);

  // If Yaw correction is not needed
  if (std::abs(yaw_radians) < 1e-6) {
    auto result = std::make_unique<FrameData>(frame.width, frame.height, frame.frame_number);
    result->yuv_data = frame.yuv_data;
    return result;
  }

  // Try OpenCL processing
  if (opencl_available_) {
    auto result = ProcessFrameOpenCL(frame, yaw_radians);
    if (result) {
      return result;
    }
    // If OpenCL processing fails, fall back to CPU processing
    throw YawFixException(2001, "OpenCL processing failed frame=" + std::to_string(frame.frame_number) +
                                    " falling back to CPU processing");
  }

  // CPU processing (responsibility separated by chroma_format)
  auto corrector = cpu::YawCorrector::Create(frame.chroma_format);
  if (!corrector) {
    throw YawFixException(2002, "Unsupported chroma_format: " + std::to_string(static_cast<int>(frame.chroma_format)));
  }
  auto cpu_result = corrector->Correct(frame, yaw_radians);
  return cpu_result;
}

double YawFix::Impl::GetYawCorrectionForFrame(int frame_number) const {
  if (corrections_.size() == 0 || frame_number < 0) {
    return 0.0;
  }
  if (frame_number < static_cast<int>(corrections_.size())) {
    return corrections_.at(frame_number);
  }
  return corrections_.values().back();
}

std::unique_ptr<FrameData> YawFix::Impl::ProcessFrameOpenCL(const FrameData& frame, double yaw_radians) {
  auto result = std::make_unique<FrameData>(frame.width, frame.height, frame.frame_number);
  result->yuv_data.resize(frame.yuv_data.size());

  auto corrector = opencl::YawCorrector::Create(frame.chroma_format);
  if (!corrector) return nullptr;
  if (!corrector->Initialize()) return nullptr;
  if (corrector->Run(frame.yuv_data.data(), result->yuv_data.data(), frame.width, frame.height, yaw_radians)) {
    return result;
  }
  return nullptr;
}
