#include <algorithm>
#include <iostream>
#include <vector>

// #define DEBUG

#include "lib/tree_node.hpp"
#include "lib/utils.hpp"

/*
  ASSUMPTIONS:
  - one source
  - empty space below source
  - last line is empty space
  - empty space next to every splitter
  */

namespace part_one {
int solution(std::vector<std::string> input) {
  int split_count = 0;
  // first line
  // propagate beam below source
  input[1][input[0].find('S')] = '|';

  // the rest
  // go from second to next to last line
  for (auto line_it = input.begin() + 1; line_it != input.end() - 1;
       ++line_it) {
    // propagate every beam
    auto beam_iters = find_all(*line_it, '|');
    std::vector<size_t> beam_idxs(beam_iters.size());
    for (const auto& beam_iter : beam_iters) {
      beam_idxs.push_back(beam_iter - (*line_it).begin());
    }
    for (size_t idx : beam_idxs) {
      // check character below beam
      char& c_below = (*(line_it + 1))[idx];
      // splitter below beam
      if (c_below == '^') {
        ++split_count;

        // propagate beam left and right to splitter
        *(&c_below - 1) = '|';
        *(&c_below + 1) = '|';
      }
      // empty below beam
      else {
        // propagate beam below
        c_below = '|';
      }
    }
  }
#ifdef DEBUG
  for (auto& line : input) {
    std::cout << line << std::endl;
  }
#endif  // DEBUG

  return split_count;
}
}  // namespace part_one

namespace part_two_naive {
int solution_naive(std::vector<std::string> input) {
  int timeline_count = 0;

  // make a tree structure of all possible timelines

  // first line
  // initialize beam timelines tree
  // data is beam position idx
  auto timeline_root = std::make_shared<TreeNode<size_t>>(input[0].find('S'));

// the rest
// go from second to next to last line
#ifdef DEBUG
  size_t line_idx = 1;
#endif  // DEBUG
  for (auto line_it = input.begin() + 1; line_it != input.end() - 1;
       ++line_it) {
#ifdef DEBUG
    std::cout << line_idx << std::endl;
    ++line_idx;
#endif  // DEBUG
    // iterate over all timelines
    for (auto timeline = timeline_root->getOldestLeaf(); timeline != nullptr;
         timeline = timeline->getNextLeaf()) {
      // propagate every beam
      // check character below beam
      char& c_below = (*(line_it + 1))[timeline->data];
      // splitter below beam
      if (c_below == '^') {
        // propagate beam left and right to splitter
        timeline->addChild(
            std::make_shared<TreeNode<size_t>>(timeline->data - 1));
        timeline->addChild(
            std::make_shared<TreeNode<size_t>>(timeline->data + 1));
      }
      // empty below beam
      else {
        // propagate beam below
        // timeline->addChild(std::make_shared<TreeNode<size_t>>(timeline->data));
      }
    }
  }

  // count timelines
  for (auto timeline = timeline_root->getOldestLeaf(); timeline != nullptr;
       timeline = timeline->getNextLeaf()) {
    ++timeline_count;
  }

  return timeline_count;
}

struct Beam {
  size_t position;  // horizontal position
  long num;         // number of beams/timelines
};

struct BeamsVec {
  std::vector<Beam> beams;

  void add_beam(Beam new_beam) {
    // check if there already is a beam in new_beam's position
    // there can't be more than 1
    if (auto found = std::find_if(beams.begin(), beams.end(),
                                  [&new_beam](const Beam& beam) -> bool {
                                    return new_beam.position == beam.position;
                                  });
        found != beams.end()) {
      // add new beams to existing beams
      (*found).num += new_beam.num;
    }
    // no beam in new_beam's position
    else {
      beams.push_back(new_beam);
    }
  }
};

long solution(std::vector<std::string> input) {
  long timeline_count = 0;

  // make a tree structure of all possible timelines

  // first line
  // initialize beams vector
  BeamsVec beams;
  beams.add_beam({
      input[0].find('S'),  // position
      1                    // num
  });

// go from first to next to last line
#ifdef DEBUG
  size_t line_idx = 1;
#endif  // DEBUG
  for (auto line_it = input.begin(); line_it != input.end() - 1; ++line_it) {
#ifdef DEBUG
    std::cout << line_idx << std::endl;
    ++line_idx;
#endif  // DEBUG
    BeamsVec beams_new;
    // iterate over all beams on current level
    for (auto& beam : beams.beams) {
      // propagate every beam
      // check character below beam
      char& c_below = (*(line_it + 1))[beam.position];
      // splitter below beam
      if (c_below == '^') {
        // propagate beam left and right to splitter
        beams_new.add_beam({beam.position - 1, beam.num});
        beams_new.add_beam({beam.position + 1, beam.num});
      }
      // empty below beam
      else {
        // propagate beam below
        beams_new.add_beam(beam);
      }
    }
    beams = beams_new;
  }

  // count all beams on lowest level
  for (auto& beam : beams.beams) {
    timeline_count += beam.num;
  }

  return timeline_count;
}
}  // namespace part_two

int main() {
  const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  // const std::string input_filename = "input_test.txt";

  auto execution_timer = ExecutionTimer();

  auto input = read_file(input_filename);
  auto result = part_one::solution(input);
  // auto result = part_two::solution(input);

  auto execution_time_us = execution_timer.GetTimeUs();

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}