#include <algorithm>
#include <cmath>
#include "cpu/yaw_corrector.h"

namespace cpu {

std::unique_ptr<FrameData> YawCorrectorMPEG2::Correct(const FrameData& frame, double yaw_radians) {
  const int width = frame.width;
  const int height = frame.height;
  const int y_size = width * height;
  const int uv_size = y_size / 4;
  auto result = std::make_unique<FrameData>(width, height, frame.frame_number);
  result->yuv_data.resize(frame.yuv_data.size());
  // Y
  const double PI = 3.14159265358979323846;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double longitude = (2.0 * x / width - 1.0) * PI;
      double latitude = (1.0 - 2.0 * y / height) * (PI / 2.0);
      double corrected_longitude = longitude + yaw_radians;
      while (corrected_longitude > PI) corrected_longitude -= 2.0 * PI;
      while (corrected_longitude < -PI) corrected_longitude += 2.0 * PI;
      int src_x = static_cast<int>((corrected_longitude / PI + 1.0) * width / 2.0);
      int src_y = static_cast<int>((1.0 - 2.0 * latitude / PI) * height / 2.0);
      src_x = std::max(0, std::min(width - 1, src_x));
      src_y = std::max(0, std::min(height - 1, src_y));
      result->yuv_data[y * width + x] = frame.yuv_data[src_y * width + src_x];
    }
  }
  // UV (MPEG2 sampling)
  for (int c = 0; c < 2; ++c) {
    const uint8_t* in = frame.yuv_data.data() + y_size + c * uv_size;
    uint8_t* out = result->yuv_data.data() + y_size + c * uv_size;
    for (int y = 0; y < height / 2; y++) {
      for (int x = 0; x < width / 2; x++) {
        double longitude = (2.0 * x / (width / 2) - 1.0) * PI;
        double latitude = (1.0 - 2.0 * y / (height / 2)) * (PI / 2.0);
        double corrected_longitude = longitude + yaw_radians;
        while (corrected_longitude > PI) corrected_longitude -= 2.0 * PI;
        while (corrected_longitude < -PI) corrected_longitude += 2.0 * PI;
        int src_x = static_cast<int>((corrected_longitude / PI + 1.0) * (width / 2) / 2.0);
        int src_y = static_cast<int>((1.0 - 2.0 * latitude / PI) * (height / 2) / 2.0);
        src_x = std::max(0, std::min((width / 2) - 1, src_x));
        src_y = std::max(0, std::min((height / 2) - 1, src_y));
        out[y * (width / 2) + x] = in[src_y * (width / 2) + src_x];
      }
    }
  }
  return result;
}

}  // namespace cpu
