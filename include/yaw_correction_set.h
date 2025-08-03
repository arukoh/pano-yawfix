#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class YawCorrectionSet {
 public:
  // Constructor: Load correction values from file path
  explicit YawCorrectionSet(const std::string& filepath);

  // Static factory
  static YawCorrectionSet Load(const std::string& filepath);

  // Array size (number of frames)
  size_t size() const;

  // Get array (const reference)
  const std::vector<double>& values() const;

  // Index access
  double at(size_t idx) const;

 private:
  std::vector<double> corrections_;
  void load_from_file(const std::string& filepath);
};

class YawCorrectionException : public std::runtime_error {
 public:
  int code;
  explicit YawCorrectionException(int code, const std::string& msg) : std::runtime_error(msg), code(code) {}
};
