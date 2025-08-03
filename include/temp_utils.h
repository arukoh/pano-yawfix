#pragma once

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

// Common utility function to get the system temporary directory path
inline std::string get_system_temp_directory() {
#ifdef _WIN32
  const char* tmp = std::getenv("TEMP");
  if (tmp) return std::string(tmp);
  tmp = std::getenv("TMP");
  if (tmp) return std::string(tmp);
  return "C:/Windows/Temp";
#else
  const char* tmp = std::getenv("TMPDIR");
  if (tmp && tmp[0] != '\0') return std::string(tmp);
  return "/tmp";
#endif
}

// Generate a random subdirectory under the temporary directory and return its path
inline std::string create_unique_temp_subdirectory(const std::string& prefix = "pano-yawfix_") {
  std::string base_tmp = get_system_temp_directory();
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> dis;
  std::string rand_dir = prefix + std::to_string(dis(gen));
  std::filesystem::path tmp_path = std::filesystem::path(base_tmp) / rand_dir;
  std::filesystem::create_directories(tmp_path);
  return tmp_path.string();
}

// Create a temporary file in a random subdirectory and return its path
inline std::string create_temp_file_in_unique_subdir(const std::string& filename = "tmp.txt",
                                                     const std::string& prefix = "pano-yawfix_") {
  std::string dir = create_unique_temp_subdirectory(prefix);
  std::filesystem::path file_path = std::filesystem::path(dir) / filename;
  std::ofstream ofs(file_path);
  ofs.close();
  return file_path.string();
}
