#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>

// #define DEBUG

#include "lib/utils.hpp"

struct Data {
  short target;                           // bitmask of target indicator lights
  std::vector<decltype(target)> buttons;  // bitmasks of button wirings
  std::vector<short> joltage;
};

/*
Encodes fired indicator light as bitmask, starting from left

Params:
n - number of lights
i - which light to fire
*/
short light_num_to_bit(int n, int i) { return 1 << (n - 1 - i); }

std::vector<Data> parse(const std::vector<std::string>& input) {
  std::vector<Data> out;
  out.reserve(input.size());
  for (const auto& line : input) {
    std::vector<std::string> parsed;
    boost::split(parsed, line, boost::is_any_of(" "));

    // target
    decltype(Data::target) target = 0;
    // remove brackets
    std::string target_str = parsed[0].substr(1, parsed[0].size() - 2);
    int n = target_str.size();  // number of lights
    int i = 0;
    for (char c : target_str) {
      if (c == '#') {
        // stack lights masks with OR gate
        target |= light_num_to_bit(n, i);
      }
      ++i;
    }

    // buttons
    decltype(Data::buttons) buttons;
    buttons.reserve((parsed.end() - 1) - (parsed.begin() + 1));
    for (auto it = parsed.begin() + 1; it != parsed.end() - 1; ++it) {
      short wiring = 0;
      // remove brackets
      std::string wiring_str = (*it).substr(1, (*it).size() - 2);
      std::vector<std::string> wiring_parsed;
      boost::split(wiring_parsed, wiring_str, boost::is_any_of(","));
      for (auto i : wiring_parsed) {
        wiring |= light_num_to_bit(n, std::stoi(i));
      }
      buttons.push_back(wiring);
    }

    // joltage
    decltype(Data::joltage) joltage;
    // remove brackets
    std::string joltage_str =
        (*(parsed.end() - 1)).substr(1, (*(parsed.end() - 1)).size() - 2);
    std::vector<std::string> joltage_parsed;
    boost::split(joltage_parsed, joltage_str, boost::is_any_of(","));
    joltage.reserve(joltage_parsed.size());
    for (auto i : joltage_parsed) {
      joltage.push_back(std::stoi(i));
    }

    out.push_back({target, buttons, joltage});
  }
  return out;
}

namespace part_one {

struct Combination {
  // bitmask of combined buttons
  decltype(Data::target) result;
  // iterator pointing to last button in combination
  decltype(Data::buttons)::iterator last_it;

  void add_button(decltype(Data::buttons)::iterator button_it) {
    // combine button bitmasks with XOR gate
    result ^= *button_it;
    last_it = button_it;
  }
};

int solution(const std::vector<Data>& input) {
  int sol = 0;
  for (auto [target, buttons, _] : input) {
    // check every combination of buttons until result matches the target.
    // no multiple presses for any button (they cancel each other).
    // button pressing order doesn't matter.
    // growing number of buttons pressed, starting from 1

    // emplace button with empty wiring at first place of buttons vector -
    // it will be needed for button combinations vector initialization
    buttons.emplace(buttons.begin(), 0);

    // initialize vector of combinations with first (empty) button
    std::vector<Combination> combinations_current{{0, buttons.begin()}};

    bool found = false;
    for (size_t num_buttons = 1; num_buttons != buttons.size() + 1;
         ++num_buttons) {
      // make all combinations with current num_buttons
      std::vector<Combination> combinations_new;
      for (const auto& combination : combinations_current) {
        // add next buttons to current combination (one at the time)
        for (auto it = combination.last_it + 1; it != buttons.end(); ++it) {
          auto combination_new = combination;
          combination_new.add_button(it);
          combinations_new.push_back(combination_new);
        }
      }
      combinations_current = combinations_new;

      // check
      for (const auto& combination : combinations_current) {
        if (combination.result == target) {
          sol += num_buttons;
          found = true;
          break;
        }
      }
      if (found) {
        break;
      }
    }
#ifdef DEBUG
    if (!found) {
      throw std::runtime_error("Button combination not found");
    }
#endif
  }
  return sol;
}
}  // namespace part_one

int main() {
  const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  // const std::string input_filename = "input_test.txt";

  auto execution_timer = ExecutionTimer();

  auto input = parse(read_file(input_filename));
  auto result = part_one::solution(input);
  // auto result = part_two::solution(input);

  auto execution_time_us = execution_timer.GetTimeUs();

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}