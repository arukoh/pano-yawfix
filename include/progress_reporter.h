#pragma once
#include <iostream>
#include <mutex>
#include <string>

class ProgressReporter {
 public:
  ProgressReporter(const std::string& target = "process", int total_frames = 1, bool verbose = false);
  void Reset(const std::string& target, int total_frames, bool verbose = false);
  void Report(int current_frame);
  void Info(const std::string& message);
  void Error(int code, const std::string& message);

 private:
  std::string target_;
  int total_frames_;
  int last_percent_;
  bool verbose_;
  std::mutex mutex_;
};
