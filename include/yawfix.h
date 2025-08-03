#pragma once

#include <memory>
#include <string>
#include <vector>
#include "y4m/frame.h"
#include "yaw_correction_set.h"

using y4m::FrameData;
using y4m::Y4MFrame;

class YawFixException : public std::runtime_error {
 public:
  int code;
  explicit YawFixException(int code, const std::string& msg) : std::runtime_error(msg), code(code) {}
};

/**
 * @brief Main class for Yaw correction filter
 */
class YawFix {
 public:
  YawFix(const YawCorrectionSet& corrections);
  ~YawFix();

  /**
   * @brief Apply Yaw correction to a frame
   * @param frame Input frame data
   * @return Corrected frame data
   */
  std::unique_ptr<FrameData> ProcessFrame(const FrameData& frame);

  /**
   * @brief Initialize OpenCL environment
   * @return true if successful, false otherwise
   */
  bool InitializeOpenCL();

  /**
   * @brief Get current OpenCL state
   * @return true if OpenCL is available, false otherwise
   */
  bool IsOpenCLAvailable() const;

 private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};
