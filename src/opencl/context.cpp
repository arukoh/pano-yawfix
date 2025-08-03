#include "opencl/context.h"
#include <CL/cl.h>
#include <iostream>

namespace opencl {

Context::Context() {}
Context::~Context() {
  if (queue_) clReleaseCommandQueue(queue_);
  if (context_) clReleaseContext(context_);
}

bool Context::Initialize() {
  cl_uint num_platforms = 0;
  clGetPlatformIDs(0, nullptr, &num_platforms);
  if (num_platforms == 0) return false;
  std::vector<cl_platform_id> platforms(num_platforms);
  clGetPlatformIDs(num_platforms, platforms.data(), nullptr);
  cl_platform_id platform = platforms[0];
  cl_uint num_devices = 0;
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &num_devices);
  if (num_devices == 0) return false;
  std::vector<cl_device_id> devices(num_devices);
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, num_devices, devices.data(), nullptr);
  device_ = devices[0];
  context_ = clCreateContext(nullptr, 1, &device_, nullptr, nullptr, nullptr);
  cl_int err = CL_SUCCESS;
  const cl_queue_properties props[] = {0};
  queue_ = clCreateCommandQueueWithProperties(context_, device_, props, &err);
  return context_ && device_ && queue_ && err == CL_SUCCESS;
}

cl_context Context::cl_ctx() const {
  return context_;
}
cl_device_id Context::device() const {
  return device_;
}
cl_command_queue Context::queue() const {
  return queue_;
}
std::string Context::GetDeviceName() const {
  char name[256] = {};
  clGetDeviceInfo(device_, CL_DEVICE_NAME, sizeof(name), name, nullptr);
  return std::string(name);
}

}  // namespace opencl
