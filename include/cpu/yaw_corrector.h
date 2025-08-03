#pragma once

#include <cstdint>
#include <memory>
#include "y4m/frame.h"
#include "y4m/y4m_types.h"

namespace cpu {

using y4m::FrameData;
using y4m::YUV420ChromaFormat;

class IYawCorrector {
 public:
  virtual ~IYawCorrector() = default;
  virtual std::unique_ptr<FrameData> Correct(const FrameData& frame, double yaw_radians) = 0;
};

class YawCorrectorJPEG : public IYawCorrector {
 public:
  std::unique_ptr<FrameData> Correct(const FrameData& frame, double yaw_radians) override;
};

class YawCorrectorMPEG2 : public IYawCorrector {
 public:
  std::unique_ptr<FrameData> Correct(const FrameData& frame, double yaw_radians) override;
};

class YawCorrector {
 public:
  static std::unique_ptr<IYawCorrector> Create(YUV420ChromaFormat fmt);
};

}  // namespace cpu
