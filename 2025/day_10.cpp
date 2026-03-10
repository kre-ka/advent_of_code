#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>

#include "lp_solve/lp_lib.h"

// #define DEBUG

#include "lib/utils.hpp"

namespace part_one {

struct Data {
  short target;                           // bitmask of target indicator lights
  std::vector<decltype(target)> buttons;  // bitmasks of button wirings
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
      std::string wiring_str = it->substr(1, it->size() - 2);
      std::vector<std::string> wiring_parsed;
      boost::split(wiring_parsed, wiring_str, boost::is_any_of(","));
      for (auto i : wiring_parsed) {
        wiring |= light_num_to_bit(n, std::stoi(i));
      }
      buttons.push_back(wiring);
    }
    out.push_back({target, buttons});
  }
  return out;
}

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
  for (auto [target, buttons] : input) {
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

namespace part_two_cheating {

struct Button {
  std::vector<int> wiring_idxs;
};

struct Data {
  std::vector<int> target;
  std::vector<Button> buttons;
};

std::vector<Data> parse(const std::vector<std::string>& input) {
  std::vector<Data> out;
  out.reserve(input.size());
  for (const auto& line : input) {
    std::vector<std::string> parsed;
    boost::split(parsed, line, boost::is_any_of(" "));

    // target
    decltype(Data::target) target;
    // remove brackets
    std::string target_str =
        (parsed.end() - 1)->substr(1, (parsed.end() - 1)->size() - 2);
    std::vector<std::string> target_parsed;
    boost::split(target_parsed, target_str, boost::is_any_of(","));
    target.reserve(target_parsed.size());
    for (auto i : target_parsed) {
      target.push_back(std::stoi(i));
    }

    // buttons
    decltype(Data::buttons) buttons;
    buttons.reserve((parsed.end() - 1) - (parsed.begin() + 1));
    for (auto it = parsed.begin() + 1; it != parsed.end() - 1; ++it) {
      Button button;
      // remove brackets
      std::string wiring_str = it->substr(1, it->size() - 2);
      std::vector<std::string> wiring_parsed;
      boost::split(wiring_parsed, wiring_str, boost::is_any_of(","));
      button.wiring_idxs.reserve(wiring_parsed.size());
      for (auto i : wiring_parsed) {
        button.wiring_idxs.push_back(std::stoi(i));
      }
      buttons.push_back(button);
    }

    out.push_back({target, buttons});
  }
  return out;
}

int solution(const std::vector<Data>& input) {
  /*
    Solve minimum number of button presses with linear program solver
    (lp_solver).

    - variables are numbers of each button presses
    - constraints are target joltage values

    e.g. (for first input):
    [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}

    There are 6 buttons - 6 variables

    With button wirings and targets they make a linear problem:

      minimize x1 + x2 + x3 + x4 + x5 + x6

      with constraints:
        (columns of
        this matrix
        are button
        wirings)
      | 0 0 0 0 1 1 |   | x1 |   | 3 |
      | 0 1 0 0 0 1 | * | x2 | = | 5 |
      | 0 0 1 1 1 0 |   | x3 |   | 4 |
      | 1 1 0 1 0 0 |   | x4 |   | 7 |
                        | x5 |
                        | x6 |
      and
      (this goes by default in lp_solver)
      | x1 |    | 0 |
      | x2 |    | 0 |
      | x3 | <= | 0 |
      | x4 |    | 0 |
      | x5 |    | 0 |
      | x6 |    | 0 |

    In lp_solver objective function (the minimization) and constraints
    coefficients take the form of a single matrix:

      | 0 1 1 1 1 1 1 |  <- objective
      | 0 0 0 0 0 1 1 |  <- constraints
      | 0 0 1 0 0 0 1 |
      | 0 0 0 1 1 1 0 |
      | 0 1 1 0 1 0 0 |
        ^
        additional column with 0 values (that's because lp_solver starts
        indexing from 1)

  */

  int all_presses_num = 0;
#ifdef DEBUG
  int dbg_counter = 0;
#endif
  for (auto [target, buttons] : input) {
#ifdef DEBUG
    // this just prints data
    std::cout << "#" << dbg_counter << ":" << std::endl;
    for (const auto& button : buttons) {
      std::cout << "(";
      for (const auto& wiring : button.wiring_idxs) {
        std::cout << wiring << ",";
      }
      std::cout << ") ";
    }
    std::cout << "{";
    for (const auto& val : target) {
      std::cout << val << ",";
    }
    std::cout << "}" << std::endl;
#endif

    // set up problem
    lprec* problem;
    int n_constraints = target.size();
    int n_vars = buttons.size();
    // rows are constraints
    // columns correspond to variables
    problem = make_lp(n_constraints, n_vars);
    // this is a minimization problem
    set_minim(problem);
    // reduce solver logs
    set_verbose(problem, IMPORTANT);

    // set problem variables to integers
    for (int i = 1; i <= n_vars; ++i) {
      set_int(problem, i, true);
    }

    // set problem matrix by columns
    // first column is all zeroes (for indexing from 1; they're set by default)
    // next columns are objective function coefficient (always 1)
    // and constraint functions coefficients (from button wirings)
    int i = 1;
    for (auto& button : buttons) {
      // use sparse arrays - with array of non-zero coefficients and array
      // of their indexes

      // coefficients - array of ones with size equal to number of button
      // wirings + 1 (for objective function)
      std::vector<double> column(button.wiring_idxs.size() + 1, 1);

      // indexes - objective function and button wirings
      std::vector<int> rowno;
      rowno.reserve(button.wiring_idxs.size() + 1);
      // objective
      rowno.push_back(0);
      // button wirings - indexes increased by 1 (to account for objective)
      for (const auto& idx : button.wiring_idxs) {
        rowno.push_back(idx + 1);
      }

      set_columnex(problem, i, column.size(), column.data(), rowno.data());

      ++i;
    }
    // set constraints type and right-hand side
    for (i = 1; i <= n_constraints; ++i) {
      set_constr_type(problem, i, EQ);
      set_rh(problem, i, target[i - 1]);
    }

    // // use this to see model
    // print_lp(problem);
    
    solve(problem);

    // number of presses is the problem objective
    all_presses_num += get_objective(problem);

#ifdef DEBUG
    std::cout << "objective: " << get_objective(problem) << std::endl;
    // get solution variable values
    double* sol;
    get_ptr_variables(problem, &sol);
    std::cout << "variables: ";
    for (int i = 0; i < n_vars; ++i) {
      std::cout << sol[i] << ", ";
    }
    std::cout << std::endl << std::endl;
    ++dbg_counter;
#endif
  }

  return all_presses_num;
}
}  // namespace part_two_cheating

int main() {
  const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  // const std::string input_filename = "input_test.txt";

  auto execution_timer = ExecutionTimer();

  // auto input = part_one::parse(read_file(input_filename));
  // auto result = part_one::solution(input);

  auto input = part_two_cheating::parse(read_file(input_filename));
  auto result = part_two_cheating::solution(input);

  auto execution_time_us = execution_timer.GetTimeUs();

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}