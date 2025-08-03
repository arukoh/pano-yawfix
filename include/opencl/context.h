#pragma once

#include <CL/cl.h>
#include <memory>
#include <string>
#include <vector>

namespace opencl {

class Context {
 public:
  Context();
  ~Context();
  bool Initialize();
  cl_context cl_ctx() const;
  cl_device_id device() const;
  cl_command_queue queue() const;
  std::string GetDeviceName() const;

 private:
  cl_context context_ = nullptr;
  cl_device_id device_ = nullptr;
  cl_command_queue queue_ = nullptr;
};

}  // namespace opencl
