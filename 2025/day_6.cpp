#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#define DEBUG

#include "lib/utils.hpp"

struct Problem {
  std::vector<int> numbers;
  char operation;

  Problem() {};
  Problem(size_t n) : numbers(n, 0) {};
};

long long solution(const std::vector<Problem>& input) {
#ifdef DEBUG
  std::ofstream file_out_debug("output_dbg_mine.txt");
#endif  // DEBUG
  long long sum = 0;
  for (auto& problem : input) {
    long solution = problem.numbers[0];
    // *
    if (problem.operation == '*') {
      for (auto number_it = problem.numbers.begin() + 1;
           number_it != problem.numbers.end(); ++number_it) {
        solution *= *number_it;
      }
    }
    // +
    else {
      for (auto number_it = problem.numbers.begin() + 1;
           number_it != problem.numbers.end(); ++number_it) {
        solution += *number_it;
      }
    }
#ifdef DEBUG
    file_out_debug << solution << std::endl;
#endif  // DEBUG
    sum += solution;
  }

#ifdef DEBUG
  file_out_debug.close();
#endif  // DEBUG

  return sum;
}

namespace part_one {
std::vector<Problem> parse(const std::vector<std::string>& input) {
  std::vector<Problem> out;

  // initialize problems and append first line
  auto line_it = input.begin();
  std::vector<std::string> parsed;
  boost::split(parsed, *line_it, boost::is_any_of(" "),
               boost::algorithm::token_compress_on);
  sanitize_split(parsed);
  for (auto& item : parsed) {
    Problem problem;
    problem.numbers.push_back(std::stoi(item));
    out.push_back(problem);
  }
  // append remaining number lines
  ++line_it;
  for (; line_it != input.end() - 1; ++line_it) {
    boost::split(parsed, *line_it, boost::is_any_of(" "),
                 boost::algorithm::token_compress_on);
    sanitize_split(parsed);
    int problem_idx = 0;
    for (auto& item : parsed) {
      out[problem_idx].numbers.push_back(std::stoi(item));
      ++problem_idx;
    }
  }
  // last line - set operation
  boost::split(parsed, *line_it, boost::is_any_of(" "),
               boost::algorithm::token_compress_on);
  sanitize_split(parsed);
  if ((*parsed.begin()).empty()) {
    parsed.erase(parsed.begin());
  }
  if ((*(parsed.end() - 1)).empty()) {
    parsed.pop_back();  // last element is empty
  }
  int problem_idx = 0;
  for (auto& item : parsed) {
    out[problem_idx].operation = item[0];
    ++problem_idx;
  }
  return out;
}

}  // namespace part_one

namespace part_two_naive {
// line: line with operation characters
std::vector<size_t> get_number_count(const std::string& line) {
  std::vector<size_t> out;
  std::vector<std::string> parsed;
  boost::split(parsed, line, boost::is_any_of("+*"));
  sanitize_split(parsed);
  for (auto spaces = parsed.begin(); spaces != parsed.end(); ++spaces) {
    // number of spaces - 1 (seperator) + 1 (operation character)
    out.push_back((*spaces).size());
  }
  // no seperator after last element
  *(out.end() - 1) += 1;
  return out;
}

std::vector<Problem> parse(const std::vector<std::string>& input) {
  std::vector<Problem> out;

  auto number_counts = get_number_count(*(input.end() - 1));

  // initialize Problems with correct number count
  out.reserve(number_counts.size());
  for (const auto& number_count : number_counts) {
    out.push_back(Problem(number_count));
  }

  // make vector of horizontal (intermediate) numbers for each problem
  std::vector<std::vector<std::string>> numbers_horizontal_vec(
      number_counts.size());
  for (auto line = input.begin(); line != (input.end() - 1); ++line) {
    size_t problem_idx = 0;
    size_t pos = 0;
    for (const auto& number_count : number_counts) {
      numbers_horizontal_vec[problem_idx].push_back(
          (*line).substr(pos, number_count));

      pos += number_count + 1;
      ++problem_idx;
    }
  }

  // make output numbers from horizontal ones
  // order is reversed, but there's no need to change order for + and *
  // operations
  size_t problem_idx = 0;
  for (const auto& numbers_horizontal : numbers_horizontal_vec) {
    // make number strings
    std::vector<std::string> numbers(numbers_horizontal[0].size());
    for (const auto& number_horizontal : numbers_horizontal) {
      size_t pos = 0;
      for (char c : number_horizontal) {
        numbers[pos] += c;

        ++pos;
      }
    }
    // convert number strings to int and assign
    size_t number_idx = 0;
    for (const auto& number : numbers) {
      out[problem_idx].numbers[number_idx] = std::stoi(number);
      ++number_idx;
    }
    ++problem_idx;
  }

  // last line - set operation
  std::vector<std::string> parsed;
  boost::split(parsed, *(input.end() - 1), boost::is_any_of(" "),
               boost::algorithm::token_compress_on);
  sanitize_split(parsed);
  problem_idx = 0;
  for (auto& item : parsed) {
    out[problem_idx].operation = item[0];
    ++problem_idx;
  }

  return out;
}
}  // namespace part_two_naive

int main() {
  // const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  const std::string input_filename = "input_test.txt";
  int64_t execution_time_us;
  long long result;
  // execution time measurement scope
  {
    auto execution_timer = ExecutionTimer(execution_time_us);

    // auto input = part_one::parse(read_file(input_filename));
    // result = solution(input);

    auto input = part_two_naive::parse(read_file(input_filename));
    result = solution(input);
  }

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}