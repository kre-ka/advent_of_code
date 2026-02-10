#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>

std::vector<std::string> read_file(std::string name) {
  std::ifstream file(name);
  std::string line;
  std::vector<std::string> data;

  while (std::getline(file, line)) {
    data.push_back(line);
  }
  file.close();

  return data;
}

int char_digit_to_int(char in) { return in - '0'; }

char int_digit_to_char(char in) { return in + '0'; }

ExecutionTimer::ExecutionTimer() : result_ref_(result_) { Start(); }

ExecutionTimer::ExecutionTimer(int64_t& result_ref) : result_ref_(result_ref) { Start(); }

ExecutionTimer::~ExecutionTimer() { result_ref_ = GetTimeUs(); }

void ExecutionTimer::Start() {
  time_start_ = std::chrono::high_resolution_clock::now();
}

int64_t ExecutionTimer::GetTimeUs() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::high_resolution_clock::now() - time_start_)
      .count();
}

void sanitize_split(std::vector<std::string>& parsed) {
  if ((*parsed.begin()).empty()) {
    parsed.erase(parsed.begin());
  }
  if ((*(parsed.end() - 1)).empty()) {
    parsed.pop_back();  // last element is empty
  }
}
