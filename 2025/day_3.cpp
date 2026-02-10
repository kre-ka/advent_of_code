#include <fstream>
#include <iostream>

#include "lib/utils.hpp"

namespace part_one {
int solution(std::vector<std::string>& input) {
  int sum = 0;
  for (auto& line : input) {
    // find first digit
    auto position_first = line.begin();
    int val_first = char_digit_to_int(*position_first);
    // check all digits but last
    for (auto position = position_first + 1; position != (line.end() - 1);
         ++position) {
      int val = char_digit_to_int(*position);
      if (val > val_first) {
        position_first = position;
        val_first = val;
      }
    }
    // find second digit
    auto position_second = position_first + 1;
    int val_second = char_digit_to_int(*position_second);
    for (auto position = position_second + 1; position != line.end();
         ++position) {
      int val = char_digit_to_int(*position);
      if (val > val_second) {
        position_second = position;
        val_second = val;
      }
    }

    int val = val_first * 10 + val_second;
    sum += val;
  }
  return sum;
}
}  // namespace part_one

namespace part_two_naive {
std::string::iterator getBiggest(std::string::iterator start,
                                 std::string::iterator stop) {
  auto position_biggest = start;
  for (auto position = start; position != stop; ++position) {
    if (*position > *position_biggest) {
      position_biggest = position;
    }
  }
  return position_biggest;
}

long solution(std::vector<std::string>& input) {
  long sum = 0;
  for (auto& line : input) {
    const int num_digits = 12;
    std::string val = "";
    auto position = line.begin();
    for (int i = num_digits - 1; i >= 0; --i) {
      position = getBiggest(position, line.end() - i);
      val += *position;
      ++position;
    }
    sum += std::stol(val);
  }
  return sum;
}
}  // namespace part_two

int main() {
  int64_t execution_time_us;
  long result;
  // execution time measurement scope
  {
    auto execution_timer = ExecutionTimer(execution_time_us);

    // auto input = read_file("input_test.txt");
    // auto input = read_file("input_example.txt");
    auto input = read_file();

    // result = part_one::solution(input);

    result = part_two_naive::solution(input);
  }

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}