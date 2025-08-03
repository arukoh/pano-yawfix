#include <CL/cl.h>
#include <iostream>

int main() {
  cl_uint num_platforms;
  clGetPlatformIDs(0, nullptr, &num_platforms);
  std::cout << "OpenCL platforms: " << num_platforms << std::endl;
  if (num_platforms > 0) {
    std::cout << "OpenCL is available" << std::endl;
    return 0;
  } else {
    std::cout << "OpenCL is not available" << std::endl;
    return 1;
  }
}
