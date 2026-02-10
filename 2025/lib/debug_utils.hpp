#include <fstream>
#include <iostream>

void CompareOutputFiles(std::string filename_0, std::string filename_1) {
  std::ifstream f_0(filename_0), f_1(filename_1);

  std::string line_0, line_1;
  int idx_line = 1;
  while (std::getline(f_0, line_0) && std::getline(f_1, line_1)) {
    if (line_0 != line_1) {
      std::cout << "v--HERE--v\n";
    }
    std::cout << idx_line << "\n";
    std::cout << line_0 << "\n" << line_1 << "\n";
    std::cout << std::endl;

    ++idx_line;
  }
}