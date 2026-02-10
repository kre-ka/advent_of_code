#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

// #define DEBUG

#include "lib/utils.hpp"

/*
  ASSUMPTIONS:
*/

struct Position {
  int x;
  int y;
  int z;
};
std::ostream& operator<<(std::ostream& os, const Position& obj) {
  os << "[" << obj.x << ", " << obj.y << ", " << obj.z << "]";
  return os;
}

std::vector<Position> parse(const std::vector<std::string>& input) {
  std::vector<Position> out;
  for (auto& line : input) {
    std::vector<std::string> parsed;
    boost::split(parsed, line, boost::is_any_of(","));
    out.push_back(
        {std::stoi(parsed[0]), std::stoi(parsed[1]), std::stoi(parsed[2])});
  }
  return out;
}

double get_distance(const Position& first, const Position& second) {
  return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2) +
              pow(first.z - second.z, 2));
}

struct Box;
struct Circuit;

struct Box {
  Position position;
  size_t circuit_id;

  Box(Position p) : position(p), circuit_id(0) {};
};

struct Distance {
  double distance;
  // these need to be stored in a containter - hence reference wrapper instead
  // of regular reference
  std::pair<std::reference_wrapper<Box>, std::reference_wrapper<Box>> boxes;
};

struct Circuit {
  size_t id;
  std::vector<std::shared_ptr<Box>> boxes;

  Circuit(size_t id, std::shared_ptr<Box> box) : id(id) {
    add_box(std::move(box));
  };

  void add_box(std::shared_ptr<Box> box) {
    box->circuit_id = id;
    boxes.push_back(std::move(box));
  }

  void merge(Circuit& other) {
    for (auto& box : other.boxes) {
      add_box(std::move(box));
    }
  }
};

class CircuitsManager {
 public:
  CircuitsManager() = default;
  CircuitsManager(size_t num_circuits) { circuits.reserve(num_circuits); }

  Circuit& generate_new_circuit(std::shared_ptr<Box> box) {
    circuits.push_back(Circuit(free_id, std::move(box)));
    ++free_id;
    return *(circuits.end() - 1);
  }

  const std::vector<Circuit>& get_circuits() { return circuits; }

  std::vector<Circuit>::iterator find(size_t id) {
    return std::find_if(circuits.begin(), circuits.end(),
                        [id](const Circuit& c) { return c.id == id; });
  }

  /*
  Returns false when trying to merge circuit with itself
  */
  bool merge_circuits(size_t id_first, size_t id_second) {
    // if trying to merge circuit with itself
    if (id_first == id_second) {
      return false;
    }

    auto& first = *find(id_first);
    auto& second = *find(id_second);

    first.merge(second);
    remove_circuit(second.id);

    return true;
  }

  /*
  Sort circuits from greatest number of boxes
  */
  void sort() {
    std::sort(circuits.begin(), circuits.end(),
              [](const Circuit& a, const Circuit& b) {
                return b.boxes.size() < a.boxes.size();
              });
  }

 private:
  void remove_circuit(size_t id) {
    auto removed_it = find(id);

    // no circuit with given id
    if (removed_it == circuits.end()) {
      return;
    }

    circuits.erase(removed_it);
  }
  std::vector<Circuit> circuits;
  // id = 0 is reserved for boxes without a circuit (uninitialized)
  size_t free_id = 1;
};

namespace part_one {
size_t solution(std::vector<Position> input) {
  size_t sol = 1;
  // make boxes from input positions
  std::vector<std::shared_ptr<Box>> boxes;
  boxes.reserve(input.size());
  for (const auto& position : input) {
    boxes.push_back(std::make_shared<Box>(position));
  }
  // put each box in individual circuit
  CircuitsManager circuits_manager(boxes.size());
  for (auto& box : boxes) {
    circuits_manager.generate_new_circuit(box);
  }

  // make a vector of all distances between boxes, sorted from smallest distance
  //   using std::set was tempting (to keep the list sorted), but it would
  //   remove elements with same distance but different boxes
  std::vector<Distance> distances;
  for (auto first_it = boxes.begin(); first_it != boxes.end(); ++first_it) {
    for (auto second_it = first_it + 1; second_it != boxes.end(); ++second_it) {
      distances.push_back(
          {get_distance((*first_it)->position, (*second_it)->position),
           {**first_it, **second_it}});
    }
  }
  // sort by smallest distance
  std::sort(distances.begin(), distances.end(),
            [](const Distance& a, const Distance& b) {
              return a.distance < b.distance;
            });

  // join circuits
  int pair_count = 0;
  const int pair_count_max = 1000;
  // const int pair_count_max = 10;
  for (auto& [_, box_pair] : distances) {
    auto& [box_first, box_second] = box_pair;
    circuits_manager.merge_circuits(box_first.get().circuit_id,
                                    box_second.get().circuit_id);
    ++pair_count;
    if (pair_count == pair_count_max) {
      break;
    }
  }

  circuits_manager.sort();

  // calculate solution from 3 largest circuits
  size_t circuits_count_sol = 0;
  const size_t circuits_count_sol_max = 3;
  for (const auto& circuit : circuits_manager.get_circuits()) {
    sol *= circuit.boxes.size();
    ++circuits_count_sol;
    if (circuits_count_sol == circuits_count_sol_max) {
      break;
    }
  }

  return sol;
}
}  // namespace part_one

namespace part_two_naive {
int solution(std::vector<Position> input) {
  int sol = 1;
  // make boxes from input positions
  std::vector<std::shared_ptr<Box>> boxes;
  boxes.reserve(input.size());
  for (const auto& position : input) {
    boxes.push_back(std::make_shared<Box>(position));
  }
  // put each box in individual circuit
  CircuitsManager circuits_manager(boxes.size());
  for (auto& box : boxes) {
    circuits_manager.generate_new_circuit(box);
  }

  std::vector<Distance> distances;
  for (auto first_it = boxes.begin(); first_it != boxes.end(); ++first_it) {
    for (auto second_it = first_it + 1; second_it != boxes.end(); ++second_it) {
      distances.push_back(
          {get_distance((*first_it)->position, (*second_it)->position),
           {**first_it, **second_it}});
    }
  }
  // sort by smallest distance
  std::sort(distances.begin(), distances.end(),
            [](const Distance& a, const Distance& b) {
              return a.distance < b.distance;
            });

  // join circuits
  for (auto& [_, box_pair] : distances) {
    auto& [box_first, box_second] = box_pair;
    circuits_manager.merge_circuits(box_first.get().circuit_id,
                                    box_second.get().circuit_id);
    // end when there's only one circuit
    if (circuits_manager.get_circuits().size() == 1) {
      sol = box_first.get().position.x * box_second.get().position.x;
      break;
    }
  }

  return sol;
}
}  // namespace part_two_naive

int main() {
  const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  // const std::string input_filename = "input_test.txt";

  auto execution_timer = ExecutionTimer();

  auto input = parse(read_file(input_filename));
  // auto result = part_one::solution(input);
  auto result = part_two_naive::solution(input);

  auto execution_time_us = execution_timer.GetTimeUs();

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}