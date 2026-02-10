#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>

#include "lib/utils.hpp"

// places "true" where rolls are
std::pair<std::vector<std::pair<long, long>>, std::vector<long>> parse(
    const std::vector<std::string>& input) {
  enum DataType { RANGE, ID };
  std::vector<std::pair<long, long>> ranges;
  std::vector<long> ids;
  enum DataType data_type = RANGE;
  for (auto& line : input) {
    if (line.size() == 0) {
      data_type = ID;
      continue;
    }
    if (data_type == RANGE) {
      std::vector<std::string> range;
      boost::split(range, line, boost::is_any_of("-"));
      ranges.push_back({std::stol(range[0]), std::stol(range[1])});
    } else if (data_type == ID) {
      ids.push_back(std::stol(line));
    }
  }
  return {ranges, ids};
}

namespace part_one {
int solution(const std::pair<std::vector<std::pair<long, long>>,
                             std::vector<long>>& input) {
  int num_fresh = 0;
  auto& [ranges, ids] = input;
  for (auto id : ids) {
    bool is_fresh = false;
    for (auto [lower, upper] : ranges) {
      if (id >= lower && id <= upper) {
        is_fresh = true;
        break;
      }
    }
    if (is_fresh) {
      ++num_fresh;
    }
  }
  return num_fresh;
}
}  // namespace part_one

namespace part_two_naive {
long solution(std::vector<std::pair<long, long>> ranges) {
  long num_fresh_ids = 0;
  // sort by first range element in ascending order
  std::sort(ranges.begin(), ranges.end(),
            [](const std::pair<long, long>& a, const std::pair<long, long>& b) {
              return b.first > a.first;
            });
  // merge overlapping ranges
  for (auto it = ranges.begin(); it != ranges.end() - 1;) {
    auto& [lower, upper] = *it;
    auto& [next_lower, next_upper] = *(it + 1);

    // if lower end of next range is within current range - merge ranges
    if (next_lower >= lower && next_lower <= upper) {
      // expand if upper end of next range is greater, don't shrink if it's
      // smaller
      *it = {lower, std::max(upper, next_upper)};

      ranges.erase(it + 1);
    }
    // move iterator only if there are no merges
    else {
      ++it;
    }
  }

  // count all fresh ids
  for (auto& [lower, upper] : ranges) {
    num_fresh_ids += upper - lower + 1;
  }

  return num_fresh_ids;
}
}  // namespace part_two

int main() {
  int64_t execution_time_us;
  long result;
  // execution time measurement scope
  {
    auto execution_timer = ExecutionTimer(execution_time_us);

    auto input = parse(read_file());
    // auto input = parse(read_file("input_example.txt"));

    // result = part_one::solution(input);

    result = part_two_naive::solution(input.first);
  }

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}