#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

std::vector<std::string> read_file(std::string name = "input.txt");

int char_digit_to_int(char in);

char int_digit_to_char(char in);

class ExecutionTimer {
 public:
  ExecutionTimer();
  ExecutionTimer(int64_t& result);
  ~ExecutionTimer();
  void Start();
  int64_t GetTimeUs();

 private:
  std::chrono::_V2::system_clock::time_point time_start_;
  int64_t result_;
  int64_t& result_ref_;
};

/*
Clean output of boost::split() of possible empty elements at the beginning and
end
*/
void sanitize_split(std::vector<std::string>& parsed);

template <typename Container, typename T>
std::vector<typename Container::iterator> find_all(Container& cont,
                                                   const T& val) {
  std::vector<typename Container::iterator> out;
  auto it = cont.begin();
  while ((it = std::find(it, cont.end(), val)) != cont.end()) {
    out.push_back(it);
    ++it;
  }
  return out;
}

/*
Makes 2d vector of references to data in input 2d vector.

You must trust me on not modifying input here. (Making input constant would force the output to use
std::reference_wrapper<const T>, which I don't want)
*/
template <typename T>
std::vector<std::vector<typename std::reference_wrapper<T>>> make_ref_container(
    std::vector<std::vector<T>>& input) {
  std::vector<std::vector<std::reference_wrapper<T>>> output(input.size());
  size_t i = 0;
  for (auto& row : input) {
    output[i].reserve(row.size());
    for (auto& item : row) {
      output[i].push_back(item);
    }
    ++i;
  }
  return output;
}

/*
Input must be rectangular
*/
template <typename T>
void transpose(std::vector<std::vector<T>>& input) {
  std::vector<std::vector<T>> transposed((*input.begin()).size());
  for (auto& row : transposed) {
    row.reserve(input.size());
  }

  for (auto& row : input) {
    size_t col = 0;
    for (auto& item : row) {
      transposed[col].push_back(item);
      ++col;
    }
  }
  input = std::move(transposed);
}
#endif