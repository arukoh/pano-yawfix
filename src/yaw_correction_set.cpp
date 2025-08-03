#include "yaw_correction_set.h"
#include <fstream>
#include <sstream>

YawCorrectionSet::YawCorrectionSet(const std::string& filepath) {
  load_from_file(filepath);
}

YawCorrectionSet YawCorrectionSet::Load(const std::string& filepath) {
  return YawCorrectionSet(filepath);
}

size_t YawCorrectionSet::size() const {
  return corrections_.size();
}

const std::vector<double>& YawCorrectionSet::values() const {
  return corrections_;
}

double YawCorrectionSet::at(size_t idx) const {
  if (idx >= corrections_.size()) {
    throw YawCorrectionException(2001, "Yaw correction index out of range: " + std::to_string(idx));
  }
  return corrections_[idx];
}

void YawCorrectionSet::load_from_file(const std::string& filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw YawCorrectionException(1001, "Failed to open correction file: " + filepath);
  }
  corrections_.clear();
  std::string line;
  int line_number = 0;
  while (std::getline(file, line)) {
    line_number++;
    // Skip comment lines and empty lines
    if (line.empty() || line[0] == '#') {
      continue;
    }
    try {
      double yaw = std::stod(line);
      corrections_.emplace_back(yaw);
    } catch (const std::exception&) {
      throw YawCorrectionException(1002, "行 " + std::to_string(line_number) + " の解析に失敗: " + line);
    }
  }
}
