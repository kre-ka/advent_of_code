#include <iostream>
#include <vector>

#include "lib/utils.hpp"

// places "true" where rolls are
std::vector<std::vector<bool>> parse(const std::vector<std::string>& input) {
  std::vector<std::vector<bool>> rolls_diagram(
      input.size(), std::vector<bool>(input[0].size(), false));
  int row = 0;
  int column = 0;
  for (auto& line : input) {
    for (auto character : line) {
      if (character == '@') {
        rolls_diagram[row][column] = true;
      }
      ++column;
    }
    column = 0;
    ++row;
  }
  return rolls_diagram;
}

// adds frame of "false" values to input
void add_frame(std::vector<std::vector<bool>>& input) {
  // add left and right frame
  for (auto& row : input) {
    row.insert(row.begin(), false);
    row.insert(row.end(), false);
  }
  // add top and bottom frame
  size_t cols = input[0].size();
  input.insert(input.begin(), std::vector<bool>(cols, false));
  input.insert(input.end(), std::vector<bool>(cols, false));
}

namespace part_one {
int solution(const std::vector<std::vector<bool>>& input) {
  const int num_adjacent_max = 4;

  int num_accessible_rolls = 0;

  // iterate over elements without frame
  for (size_t row = 1; row < input.size() - 1; ++row) {
    for (size_t col = 1; col < input[0].size() - 1; ++col) {
      // count all rolls around the position, including position
      int num_adjacent = 0;
      for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
          num_adjacent += input[row + i][col + j];
        }
      }

      // mask with input - there will be zeroes where there are no rolls
      num_adjacent *= input[row][col];

      // check for number of adjacent rolls + the roll itself
      if (num_adjacent < num_adjacent_max + 1 && num_adjacent != 0) {
        ++num_accessible_rolls;
      }
    }
  }

  return num_accessible_rolls;
}
}  // namespace part_one

namespace part_two_naive {
int solution(std::vector<std::vector<bool>> input) {
  const int num_adjacent_max = 4;

  int num_removed = 0;

  // find and remove rolls until no new rolls are removed
  while (true) {
    std::vector<std::pair<int, int>> to_remove = {};

    // find rolls to remove
    // iterate over elements without frame
    for (size_t row = 1; row < input.size() - 1; ++row) {
      for (size_t col = 1; col < input[0].size() - 1; ++col) {
        // count all rolls around the position, including position
        int num_adjacent = 0;
        for (int i = -1; i <= 1; ++i) {
          for (int j = -1; j <= 1; ++j) {
            num_adjacent += input[row + i][col + j];
          }
        }

        // mask with input - there will be zeroes where there are no rolls
        num_adjacent *= input[row][col];

        // check for number of adjacent rolls + the roll itself
        if (num_adjacent < num_adjacent_max + 1 && num_adjacent != 0) {
          to_remove.push_back({row, col});
          ++num_removed;
        }
      }
    }

    // remove rolls
    for (auto [row, col] : to_remove) {
      input[row][col] = false;
    }

    // finish if nothing was removed
    if (to_remove.empty()) {
      break;
    }
  }
  return num_removed;
}
}  // namespace part_two

int main() {
  int64_t execution_time_us;
  long result;
  // execution time measurement scope
  {
    auto execution_timer = ExecutionTimer(execution_time_us);

    auto input = parse(read_file());

    add_frame(input);

    // result = part_one::solution(input);

    result = part_two_naive::solution(input);
  }

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}