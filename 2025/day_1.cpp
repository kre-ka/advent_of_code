#include <iostream>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "lib/utils.hpp"

std::vector<std::tuple<char, int>> parse(const std::vector<std::string>& in) {
  std::vector<std::tuple<char, int>> out;

  for (std::string line : in) {
    char direction = line[0];
    int value = std::stoi(line.substr(1));

    out.push_back({direction, value});
  }
  return out;
}

int part_one() {
  int current_val = 50;
  const int overflow_val = 100;

  std::vector<std::tuple<char, int>> input = parse(read_file());

  int sign;
  int count = 0;
  for (auto& [direction, value] : input) {
    if (direction == 'L') {
      sign = -1;
    } else if (direction == 'R') {
      sign = 1;
    } else {
      throw std::invalid_argument("invalid direction value");
    }
    value *= sign;

    current_val += value;
    current_val %= overflow_val;
    if (current_val < 0) {
      current_val += overflow_val;
    }

    if (current_val == 0) {
      ++count;
    }
  }
  return count;
}

int part_two() {
  int current_val = 50;
  const int overflow_val = 100;

  std::vector<std::tuple<char, int>> input = parse(read_file());

  int sign = 1;
  int count = 0;
  bool counted = false;
  for (auto& [direction, value] : input) {
    if (direction == 'L') {
      sign = -1;
    } else if (direction == 'R') {
      sign = 1;
    } else {
      throw std::invalid_argument("invalid direction value");
    }
    value *= sign;

    count += abs(value / overflow_val);
    value %= 100;

    current_val += value;

    if (current_val < 0 || current_val > overflow_val) {
      ++count;
      // correct if this was already counted when stopped at 0
      if (counted) {
        --count;
      }
    }
    counted = false;
    if ((current_val % overflow_val) == 0) {
      ++count;
      // information for next loop
      counted = true;
    }

    current_val %= overflow_val;
    if (current_val < 0) {
      current_val += overflow_val;
    }
  }
  return count;
}

int main() {
  // int result = part_one();
  int result = part_two();

  std::cout << result << std::endl;

  return 0;
}