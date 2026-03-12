#include <array>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

// #define DEBUG

#include "lib/utils.hpp"

using DeviceId = std::array<char, 3>;

std::map<DeviceId, std::vector<DeviceId>> parse(
    const std::vector<std::string>& input) {
  std::map<DeviceId, std::vector<DeviceId>> out;

  for (auto line : input) {
    std::vector<std::string> parsed;
    boost::split(parsed, line, boost::is_any_of(":"));
    DeviceId device;
    // put string into array
    std::copy(parsed[0].begin(), parsed[0].end(), device.data());

    line = parsed[1];
    boost::split(parsed, line, boost::is_any_of(" "));
    sanitize_split(parsed);

    std::vector<DeviceId> output_devices;
    output_devices.reserve(parsed.size());
    for (const auto& item : parsed) {
      DeviceId output_device;
      std::copy(item.begin(), item.end(), output_device.data());
      output_devices.push_back(output_device);
    }
    out.emplace(device, output_devices);
  }
  return out;
}

namespace part_one {
int solution(const std::map<DeviceId, std::vector<DeviceId>>& input) {
  // kinda ugly - in C++20 I would use std::to_array
  const DeviceId device_from{'y', 'o', 'u'};
  const DeviceId device_to{'o', 'u', 't'};

  // make a queue of devices and count their outputs
  std::queue<DeviceId> device_queue;
  device_queue.push(device_from);

  // output - number of all paths
  int n_paths = 1;
  while (!device_queue.empty()) {
    auto& devices = input.at(device_queue.front());
    device_queue.pop();
    n_paths += devices.size() - 1;
    for (const auto& item : devices) {
      // don't add `device_to` to queue
      if (item != device_to) {
        device_queue.push(item);
      }
    }
  }

  return n_paths;
}
}  // namespace part_one

int main() {
  const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  // const std::string input_filename = "input_test.txt";

  auto execution_timer = ExecutionTimer();

  auto input = parse(read_file(input_filename));
  auto result = part_one::solution(input);

  auto execution_time_us = execution_timer.GetTimeUs();

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}