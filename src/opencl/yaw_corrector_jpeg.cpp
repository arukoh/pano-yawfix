#include <iostream>
#include "opencl/yaw_corrector.h"

namespace opencl {

const char* YawCorrectorJPEG::GetKernelSource() const {
  return R"(
__kernel void yaw_correction(__global const uchar* input,
                           __global uchar* output,
                           int width,
                           int height,
                           float yaw_radians) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    if (x >= width || y >= height) return;
    const float PI = 3.14159265358979323846f;
    float longitude = (2.0f * x / (float)width - 1.0f) * PI;
    float latitude = (1.0f - 2.0f * y / (float)height) * (PI / 2.0f);
    float corrected_longitude = longitude + yaw_radians;
    while (corrected_longitude > PI) corrected_longitude -= 2.0f * PI;
    while (corrected_longitude < -PI) corrected_longitude += 2.0f * PI;
    int src_x = (int)((corrected_longitude / PI + 1.0f) * (float)width / 2.0f);
    int src_y = (int)((1.0f - 2.0f * latitude / PI) * (float)height / 2.0f);
    src_x = max(0, min(width - 1, src_x));
    src_y = max(0, min(height - 1, src_y));
    int idx = y * width + x;
    int src_idx = src_y * width + src_x;
    output[idx] = input[src_idx];
    // UV: JPEG/center sampling
    if (x < width/2 && y < height/2) {
        int uv_width = width / 2;
        int uv_height = height / 2;
        int y_size = width * height;
        float uv_longitude = (2.0f * x / (float)uv_width - 1.0f) * PI;
        float uv_latitude = (1.0f - 2.0f * y / (float)uv_height) * (PI / 2.0f);
        float uv_corrected_longitude = uv_longitude + yaw_radians;
        while (uv_corrected_longitude > PI) uv_corrected_longitude -= 2.0f * PI;
        while (uv_corrected_longitude < -PI) uv_corrected_longitude += 2.0f * PI;
        int uv_src_x = (int)((uv_corrected_longitude / PI + 1.0f) * (float)uv_width / 2.0f);
        int uv_src_y = (int)((1.0f - 2.0f * uv_latitude / PI) * (float)uv_height / 2.0f);
        uv_src_x = max(0, min(uv_width - 1, uv_src_x));
        uv_src_y = max(0, min(uv_height - 1, uv_src_y));
        int uv_idx = y * uv_width + x;
        int uv_src_idx = uv_src_y * uv_width + uv_src_x;
        output[y_size + uv_idx] = input[y_size + uv_src_idx];
        output[y_size + y_size/4 + uv_idx] = input[y_size + y_size/4 + uv_src_idx];
    }
}
)";
};

}  // namespace opencl
