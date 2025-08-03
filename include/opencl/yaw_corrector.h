#pragma once

#include <CL/cl.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>
#include "opencl/context.h"
#include "y4m/y4m_types.h"

namespace opencl {

class IYawCorrector {
 public:
  virtual ~IYawCorrector() = default;
  virtual bool Initialize() = 0;
  virtual bool Run(const uint8_t* input, uint8_t* output, int width, int height, double yaw_radians) = 0;
};

class YawCorrector : public IYawCorrector {
 public:
  static std::unique_ptr<IYawCorrector> Create(y4m::YUV420ChromaFormat fmt);

 protected:
  std::unique_ptr<Context> context_;
  cl_program program_ = nullptr;
  cl_kernel kernel_ = nullptr;

  YawCorrector() : context_(std::make_unique<Context>()) {}
  virtual ~YawCorrector() {
    if (kernel_) clReleaseKernel(kernel_);
    if (program_) clReleaseProgram(program_);
  }
  virtual const char* GetKernelSource() const = 0;
  virtual bool Initialize() {
    if (!context_->Initialize()) return false;
    const char* kernel_source = GetKernelSource();
    size_t source_size = strlen(kernel_source);
    cl_int err;
    cl_context ctx = context_->cl_ctx();
    cl_device_id dev = context_->device();
    program_ = clCreateProgramWithSource(ctx, 1, &kernel_source, &source_size, &err);
    if (err != CL_SUCCESS) return false;
    err = clBuildProgram(program_, 1, &dev, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
      size_t log_size;
      clGetProgramBuildInfo(program_, dev, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
      std::vector<char> log(log_size);
      clGetProgramBuildInfo(program_, dev, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
      std::cerr << "Failed to build OpenCL program:\n" << log.data() << std::endl;
      return false;
    }
    kernel_ = clCreateKernel(program_, "yaw_correction", &err);
    if (err != CL_SUCCESS) return false;
    return true;
  }
  virtual bool Run(const uint8_t* input, uint8_t* output, int width, int height, double yaw_radians) {
    if (!context_ || !context_->cl_ctx() || !context_->device() || !context_->queue()) return false;
    size_t y_size = width * height;
    size_t uv_size = y_size / 4;
    size_t total_size = y_size + 2 * uv_size;
    cl_int err;
    cl_context ctx = context_->cl_ctx();
    cl_command_queue queue = context_->queue();
    cl_kernel kernel = kernel_;
    cl_mem input_buffer = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, total_size, (void*) input, &err);
    if (err != CL_SUCCESS) return false;
    cl_mem output_buffer = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, total_size, nullptr, &err);
    if (err != CL_SUCCESS) {
      clReleaseMemObject(input_buffer);
      return false;
    }
    float yaw_float = static_cast<float>(yaw_radians);
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &width);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &height);
    err |= clSetKernelArg(kernel, 4, sizeof(float), &yaw_float);
    if (err != CL_SUCCESS) {
      clReleaseMemObject(input_buffer);
      clReleaseMemObject(output_buffer);
      return false;
    }
    size_t global_work_size[2] = {static_cast<size_t>(width), static_cast<size_t>(height)};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, global_work_size, nullptr, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
      clReleaseMemObject(input_buffer);
      clReleaseMemObject(output_buffer);
      return false;
    }
    err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, total_size, output, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
      clReleaseMemObject(input_buffer);
      clReleaseMemObject(output_buffer);
      return false;
    }
    clReleaseMemObject(input_buffer);
    clReleaseMemObject(output_buffer);
    return true;
  }
};

class YawCorrectorJPEG : public YawCorrector {
 public:
  YawCorrectorJPEG() = default;
  ~YawCorrectorJPEG() override = default;
  const char* GetKernelSource() const override;
};

class YawCorrectorMPEG2 : public YawCorrector {
 public:
  YawCorrectorMPEG2() = default;
  ~YawCorrectorMPEG2() override = default;
  const char* GetKernelSource() const override;
};

}  // namespace opencl
