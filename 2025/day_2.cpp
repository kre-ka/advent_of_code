#include <boost/algorithm/string.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

// #define DEBUG

#include "lib/tree_node.hpp"
#include "lib/utils.hpp"

std::vector<std::tuple<std::string, std::string>> parse(
    std::vector<std::string> in) {
  std::vector<std::tuple<std::string, std::string>> out;

  if (in.size() == 0) {
    return out;
  }

  std::string line = in[0];
  std::vector<std::string> ranges;

  boost::split(ranges, line, boost::is_any_of(","));

  for (std::string range : ranges) {
    std::vector<std::string> items;
    boost::split(items, range, boost::is_any_of("-"));

    out.push_back({items[0], items[1]});
  }
  return out;
}

std::string generateLargest(size_t num_digits) {
  std::string out = "";
  for (size_t i = 0; i < num_digits; ++i) {
    out.append("9");
  }
  return out;
};

std::string generateSmallest(size_t num_digits) {
  std::string out = "1";
  for (size_t i = 1; i < num_digits; i++) {
    out.append("0");
  }
  return out;
};

std::vector<std::tuple<std::string, std::string>> splitToSubRanges(
    std::tuple<std::string, std::string> in) {
  std::vector<std::tuple<std::string, std::string>> out;

  size_t size_diff = std::get<1>(in).size() - std::get<0>(in).size();
  // just one subrange
  if (size_diff == 0) {
    out.push_back(in);
  }
  // multiple
  else {
    // first subrange
    out.push_back({std::get<0>(in), generateLargest(std::get<0>(in).size())});

    // middle ones
    for (size_t i = std::get<0>(in).size() + 1; i < std::get<1>(in).size();
         ++i) {
      out.push_back({generateSmallest(i), generateLargest(i)});
    }

    // last one
    out.push_back({generateSmallest(std::get<1>(in).size()), std::get<1>(in)});
  }

  return out;
}

// efficient comparison of numbers in string format - digit by digit
bool isSmaller(const std::string& first, const std::string& second,
               size_t omit_first = 0) {
  if (first.size() != second.size()) {
    throw std::invalid_argument("first and second must be the same size");
  }
  // if (omit_first >= first.size()) {
  //   omit_first = first.size() - 1;
  // }
  // omit_first = 0;

  for (size_t place = omit_first; place < first.size(); ++place) {
    size_t digit_first = char_digit_to_int(first[place]);
    size_t digit_second = char_digit_to_int(second[place]);

    if (digit_first < digit_second) {
      return true;
    } else if (digit_first > digit_second) {
      return false;
    }
    // equal and last digit
    if (place == first.size() - 1) {
      return false;
    }
    // equal - check next digit
  }
}

// efficient comparison of numbers in string format - digit by digit
bool isGreater(const std::string& first, const std::string& second,
               size_t omit_first = 0) {
  if (first.size() != second.size()) {
    throw std::invalid_argument("first and second must be the same size");
  }

  for (size_t place = omit_first; place < first.size(); ++place) {
    size_t digit_first = char_digit_to_int(first[place]);
    size_t digit_second = char_digit_to_int(second[place]);

    if (digit_first > digit_second) {
      return true;
    } else if (digit_first < digit_second) {
      return false;
    }
    // equal and last digit
    if (place == first.size() - 1) {
      return false;
    }
    // equal - check next digit
  }
}

namespace part_one {
long solution(std::vector<std::tuple<std::string, std::string>>& input) {
#ifdef DEBUG
  std::ofstream f;
  f.open("output_smart.txt");
#endif  // DEBUG

  long sum = 0;
  for (auto& range : input) {
    // split to ranges of the same number of digits
    auto sub_ranges = splitToSubRanges(range);

    for (auto& [lower, upper] : sub_ranges) {
      size_t num_digits = lower.size();
      // skip this range if number of digits is uneven
      if ((num_digits % 2) != 0) {
        continue;
      }
      // make a tree structure of consecutive possible digits (i.e. in range)
      // it will only contain the greater half the number (the other half is to
      // be repeated)
      auto root = std::make_shared<TreeNode<char>>(0);
      auto current_node = root;
      // move over digits of first part of the number
      for (size_t place = 0; place < (num_digits / 2); ++place) {
        while (true) {
          // set range to iterate over
          // if current_node has no siblings, move through digits in range
          size_t num_start = char_digit_to_int(lower[place]);
          size_t num_end = char_digit_to_int(upper[place]);
          // if there is a smaller digit in this or higher position -
          // start from 0
          auto temp_node = current_node;
          while (temp_node) {
            if (!temp_node->older_sibling.expired()) {
              num_start = 0;
              break;
            }
            temp_node = temp_node->parent.lock();
          }
          // if there is a greater digit in this or higher position -
          // end at 9
          temp_node = current_node;
          while (temp_node) {
            if (!temp_node->younger_sibling.expired()) {
              num_end = 9;
              break;
            }
            temp_node = temp_node->parent.lock();
          }

          // add all digits between lower and upper range
          for (size_t num = num_start; num <= num_end; ++num) {
            current_node->addChild(
                std::make_shared<TreeNode<char>>(int_digit_to_char(num)));
          }

          // try moving to next node on the same level
          // (i.e. in the same position in the number)
          temp_node = current_node->getNextLeaf();
          if (temp_node) {
            current_node = temp_node;
            continue;
          }
          // if there isn't any -
          // move to smallest number in the tree and iterate to next digit
          current_node = root->getOldestLeaf();
          break;
        }
      }

      // make a vector of possible ids from tree
      // it will be sorted from smallest to largest
      current_node = root;
      auto ids_vec = current_node->flatten();
      std::vector<std::string> ids_str;
      for (auto& id_vec : ids_vec) {
        std::string id_str = "";
        // iterate digits in reverse order, without last (which is always 0)
        for (auto it = id_vec.rbegin() + 1; it != id_vec.rend(); ++it) {
          id_str += *it;
        }
        // add second id part
        id_str += id_str;
        ids_str.push_back(id_str);
      }

      // check

      // only first and last id need to be checked if they're in range - middle
      // ones are guaranteed (!)

      // start checking from the middle of id - first half is guaranteed to be
      // in range
      size_t omit_first = num_digits / 2;

      // if there's only one id - check it for both lower and upper range
      if (ids_str.size() == 1) {
        std::string& id_str = ids_str[0];
        if (!isSmaller(id_str, lower, omit_first) &&
            !isGreater(id_str, upper, omit_first)) {
          sum += std::stol(id_str);
#ifdef DEBUG
          f << id_str << "\n";
#endif  // DEBUG
        }
        continue;
      }
      // multiple ids case - check first for lower range, check last for upper
      // range, don't check middle ones
      auto ids_str_it = ids_str.begin();
      // first
      if (!isSmaller(*ids_str_it, lower, omit_first)) {
        sum += std::stol(*ids_str_it);
#ifdef DEBUG
        f << *ids_str_it << "\n";
#endif  // DEBUG
      }
      ++ids_str_it;
      // middle - guaranteed
      for (ids_str_it; ids_str_it < ids_str.end() - 1; ++ids_str_it) {
        sum += std::stol(*ids_str_it);
#ifdef DEBUG
        f << *ids_str_it << "\n";
#endif  // DEBUG
      }
      // last
      if (!isGreater(*ids_str_it, upper, omit_first)) {
        sum += std::stol(*ids_str_it);
#ifdef DEBUG
        f << *ids_str_it << "\n";
#endif  // DEBUG
      }
    }
  }
#ifdef DEBUG
  f.close();
#endif  // DEBUG
  return sum;
}

long solution_naive(std::vector<std::tuple<std::string, std::string>>& input) {
#ifdef DEBUG
  std::ofstream f;
  f.open("output_naive.txt");
#endif  // DEBUG

  long sum = 0;
  for (auto& [lower, upper] : input) {
    long id = std::stol(lower);
    while (id <= std::stol(upper)) {
      std::string id_str = std::to_string(id);
      size_t num_digits = id_str.size();

      // omit uneven number of digits
      if ((num_digits % 2) != 0) {
        // move to lowest number with more digits
        id = (long)powl(10, num_digits);
        continue;
      }

      std::string id_str_u = id_str.substr(0, num_digits / 2);
      std::string id_str_l = id_str.substr(num_digits / 2, num_digits / 2);

      if (id_str_u == id_str_l) {
        sum += id;
#ifdef DEBUG
        f << id << "\n";
#endif  // DEBUG
      }

      ++id;
    }
  }
#ifdef DEBUG
  f.close();
#endif  // DEBUG

  return sum;
}

}  // namespace part_one

namespace part_two_naive {
bool hasPattern(std::string& in) {
  size_t in_len = in.size();
  if (in_len == 1) {
    return false;
  }
  // check for pattern of increasing length
  bool has_pattern;
  for (size_t pattern_len = 1; pattern_len <= (in_len / 2); ++pattern_len) {
    // skip if in_len is not a multiple of pattern_len
    if ((in_len % pattern_len) != 0) {
      has_pattern = false;
      continue;
    }
    std::string pattern = in.substr(0, pattern_len);
    // search for non-pattern instance
    has_pattern = true;
    for (size_t place = 1; place < (in_len / pattern_len); ++place) {
      std::string checked = in.substr(pattern_len * place, pattern_len);
      if (checked != pattern) {
        has_pattern = false;
        break;
      }
    }
    // no need to keep searching if pattern has been found
    if (has_pattern) {
      break;
    }
  }
  return has_pattern;
}

long solution(std::vector<std::tuple<std::string, std::string>>& input) {
#ifdef DEBUG
  std::ofstream f;
  f.open("output_smart.txt");
#endif  // DEBUG

  long sum = 0;
  for (auto& range : input) {
    // split to ranges of the same number of digits
    auto sub_ranges = splitToSubRanges(range);

    for (auto& [lower, upper] : sub_ranges) {
      size_t id_len = lower.size();

      // check for repeating pattern of pattern_len length
      for (size_t pattern_len = 1; pattern_len <= (id_len / 2); ++pattern_len) {
        // skip if id length is not a multiple of pattern length
        if ((id_len % pattern_len) != 0) {
          continue;
        }

        // make a tree structure of consecutive digits that could make a pattern
        // - they need to be in range
        auto root = std::make_shared<TreeNode<char>>(0);
        auto current_node = root;
        // move over pattern digits
        for (size_t place = 0; place < pattern_len; ++place) {
          while (true) {
            // set range to iterate over
            // if current_node has no siblings, move through digits in range
            size_t num_start = char_digit_to_int(lower[place]);
            size_t num_end = char_digit_to_int(upper[place]);
            // if there is a smaller digit in this or higher position -
            // start from 0
            auto temp_node = current_node;
            while (temp_node) {
              if (!temp_node->older_sibling.expired()) {
                num_start = 0;
                break;
              }
              temp_node = temp_node->parent.lock();
            }
            // if there is a greater digit in this or higher position -
            // end at 9
            temp_node = current_node;
            while (temp_node) {
              if (!temp_node->younger_sibling.expired()) {
                num_end = 9;
                break;
              }
              temp_node = temp_node->parent.lock();
            }
            // add all digits between lower and upper range
            for (size_t num = num_start; num <= num_end; ++num) {
              current_node->addChild(
                  std::make_shared<TreeNode<char>>(int_digit_to_char(num)));
            }

            // try moving to next node on the same level
            // (i.e. in the same position in the number)
            temp_node = current_node->getNextLeaf();
            if (temp_node) {
              current_node = temp_node;
              continue;
            }
            // if there isn't any -
            // move to smallest number in the tree and iterate to next digit
            current_node = root->getOldestLeaf();
            break;
          }
        }

        // make a vector of possible ids from tree
        // it will be sorted from smallest to largest
        current_node = root;
        auto ids_vec = current_node->flatten();
        std::vector<std::string> ids_str;
        for (auto& id_vec : ids_vec) {
          std::string id_pattern = "";
          std::string id_str = "";
          // iterate digits in reverse order, without last (which is always 0)
          for (auto it = id_vec.rbegin() + 1; it != id_vec.rend(); ++it) {
            id_pattern += *it;
          }

          // ensure there is no subpattern inside the pattern - which would
          // mean that the id was already considered
          // e.g. "1212" contains "12", which must have been considered before
          if (hasPattern(id_pattern)) {
            continue;
          }

          // repeat pattern for full id
          for (size_t i = 0; i < (id_len / pattern_len); ++i) {
            id_str += id_pattern;
          }
          ids_str.push_back(id_str);
        }

        // check

        if (ids_str.empty()) {
          continue;
        }

        // only one id case
        if (ids_str.size() == 1) {
          if (!isSmaller(ids_str[0], lower) && !isGreater(ids_str[0], upper)) {
            sum += std::stol(ids_str[0]);
#ifdef DEBUG
            f << id_smallest << "\n";
#endif  // DEBUG
          }
          continue;
        }

        // multiple ids case - check first for lower range,
        // check last for upper range, don't check middle ones
        auto ids_str_it = ids_str.begin();
        // first
        if (!isSmaller(*ids_str_it, lower)) {
          sum += std::stol(*ids_str_it);

#ifdef DEBUG
          f << *ids_str_it << "\n";
#endif  // DEBUG
        }
        ++ids_str_it;
        // middle - guaranteed
        for (ids_str_it; ids_str_it < ids_str.end() - 1; ++ids_str_it) {
          sum += std::stol(*ids_str_it);

#ifdef DEBUG
          f << *ids_str_it << "\n";
#endif  // DEBUG
        }
        // last
        if (!isGreater(*ids_str_it, upper)) {
          sum += std::stol(*ids_str_it);

#ifdef DEBUG
          f << *ids_str_it << "\n";
#endif  // DEBUG
        }
      }
    }
  }
#ifdef DEBUG
  f.close();
#endif  // DEBUG

  return sum;
}

long solution_naive(std::vector<std::tuple<std::string, std::string>>& input) {
#ifdef DEBUG
  std::ofstream f;
  f.open("output_naive.txt");
#endif  // DEBUG

  long sum = 0;
  for (auto& range : input) {
    // split to ranges of the same number of digits
    auto sub_ranges = splitToSubRanges(range);

    for (auto& [lower, upper] : sub_ranges) {
      for (long id = std::stol(lower); id <= std::stol(upper); ++id) {
        std::string id_str = std::to_string(id);
        if (hasPattern(id_str)) {
          sum += id;

#ifdef DEBUG
          f << id_str << "\n";
#endif  // DEBUG
        }
      }
    }
  }

#ifdef DEBUG
  f.close();
#endif  // DEBUG

  return sum;
}
}  // namespace part_two_naive

int main() {
  int64_t execution_time_us;
  long result;
  // execution time measurement scope
  {
    auto execution_timer = ExecutionTimer(execution_time_us);

    // auto input = parse(read_file("input_test.txt"));
    // auto input = parse(read_file("input_example.txt"));
    auto input = parse(read_file());

    result = part_one::solution(input);
    // result = part_one::solution_naive(input);

    // result = part_two_naive::solution(input);
    // result = part_two_naive::solution_naive(input);
  }

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}