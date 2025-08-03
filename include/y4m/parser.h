#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "y4m/y4m_types.h"

namespace y4m {

class Parser {
 public:
  Parser();
  bool ParseHeader(const std::string& header_line);
  bool WriteHeader(std::ostream& os) const;
  bool ReadFrame(std::istream& is, Y4MFrame& frame);
  bool WriteFrame(std::ostream& os, const Y4MFrame& frame) const;
  const Y4MHeader& GetHeader() const;

 private:
  Y4MHeader header_;
  int frame_count_ = 0;
};

}  // namespace y4m
