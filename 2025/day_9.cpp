#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#define DEBUG

#include "lib/utils.hpp"
#include "lib/vector2i.hpp"

long get_area(const Vector2i<int>& first, const Vector2i<int>& second) {
  return (labs(first.x - second.x) + 1) * (labs(first.y - second.y) + 1);
}

std::vector<Vector2i<int>> parse(const std::vector<std::string>& input) {
  std::vector<Vector2i<int>> out;
  for (auto& line : input) {
    std::vector<std::string> parsed;
    boost::split(parsed, line, boost::is_any_of(","));
    out.push_back({std::stoi(parsed[0]), std::stoi(parsed[1])});
  }
  return out;
}

namespace part_one {
long solution(const std::vector<Vector2i<int>>& input) {
  long area_max = 0;

  for (auto first_it = input.begin(); first_it != input.end() - 1; ++first_it) {
    for (auto second_it = first_it + 1; second_it != input.end(); ++second_it) {
      area_max = std::max(area_max, get_area(*first_it, *second_it));
    }
  }

  return area_max;
}
}  // namespace part_one

namespace part_two_naive {
/*
  Didn't notice the red tiles are sorted, but I'll leave it as it is
  ASSUMPTIONS:
  - no closed holes inside the shape
*/

enum Tile { UNKNOWN, RED, GREEN, EMPTY };

std::ostream& operator<<(std::ostream& os, const Tile& tile) {
  char c = '?';
  switch (tile) {
    case Tile::UNKNOWN:
      c = '?';
      break;
    case Tile::RED:
      c = '#';
      break;
    case Tile::GREEN:
      c = 'X';
      break;
    case Tile::EMPTY:
      c = '.';
      break;
  }
  os << c;
  return os;
}

/*
Make 2d vector of tiles that can fit all tiles given in input + 1 tile frame,
filled with Tile::UNKNOWN.
Also remaps positions in input to correspond to tile vector indexes
*/
std::vector<std::vector<Tile>> make_tiles(std::vector<Vector2i<int>>& input) {
  // make 2d vector of tiles
  // first, find tile array size from red tile positions
  Vector2i<int> p_min{std::numeric_limits<int>::max(),
                 std::numeric_limits<int>::max()};
  Vector2i<int> p_max{0, 0};
  for (const auto& position : input) {
    p_min.x = std::min(p_min.x, position.x);
    p_min.y = std::min(p_min.y, position.y);
    p_max.x = std::max(p_max.x, position.x);
    p_max.y = std::max(p_max.y, position.y);
  }
  // add 1-tile frame
  p_min += {-1, -1};
  p_max += {1, 1};
  // remap tile positions so that [x_min, y_min] is [0, 0]
  Vector2i<int> p_correction{-p_min.x, -p_min.y};
  p_min += p_correction;
  p_max += p_correction;
  for (auto& position : input) {
    position += p_correction;
  }
  // make tiles vector and fill it with Tile::UNKNOWN
  std::vector<std::vector<Tile>> tiles(
      p_max.y + 1, std::vector<Tile>(p_max.x + 1, Tile::UNKNOWN));
  return tiles;
}

void put_green_walls_horizontal(
    std::vector<std::vector<std::reference_wrapper<Tile>>>& tiles) {
  for (auto& row : tiles) {
    auto red_its = find_all(row, Tile::RED);
    if (red_its.size() <= 1) {
      continue;
    } else {
      // iterate from first to next-to last red tile iterators
      for (auto red_its_it = red_its.begin(); red_its_it != red_its.end() - 1;
           ++red_its_it) {
        // iterate all tiles between red_its_it and next red tile
        // (exclusively)
        for (auto it = *red_its_it + 1; it != *(red_its_it + 1); ++it) {
          (*it).get() = Tile::GREEN;
        }
      }
    }
  }
}

void print_tiles(const std::vector<std::vector<Tile>>& tiles) {
  for (const auto& row : tiles) {
    for (const Tile tile : row) {
      std::cout << tile;
    }
    std::cout << std::endl;
  }
}

long solution(std::vector<Vector2i<int>> input) {
  auto tiles = make_tiles(input);

  //  put red tiles in vector
  for (const auto& position : input) {
    tiles[position.y][position.x] = Tile::RED;
  }

  // put green tiles between red tiles
  // horizontally
  //   this is kinda nasty - I need to use ref_container later, but make it here
  //   to avoid making 2 versions of put_green_walls_horizontal with duplicated
  //   code. Could also use a nice override that creates ref_container
  //   internally, but I'll be needing this ref_container later, so why make it
  //   2 times
  auto tiles_refed = make_ref_container(tiles);
  put_green_walls_horizontal(tiles_refed);
  // vertically - same algorithm but on transposed tiles
  // don't want to transpose tiles themselves, hence ref_container
  auto tiles_refed_transposed = tiles_refed;
  transpose(tiles_refed_transposed);
  put_green_walls_horizontal(tiles_refed_transposed);

  print_tiles(tiles);

  // fill outside area
  // I use flood fill algorithm with queue
  std::vector<Vector2i<int>> fill_queue;
  fill_queue.push_back({0, 0});
  while (!fill_queue.empty()) {
    Vector2i pos = *fill_queue.end();
    fill_queue.pop_back();
    if ((pos.x >= 0 && pos.x < tiles[0].size()) &&
        (pos.y >= 0 && pos.y < tiles.size())) {
      Tile& tile = tiles[pos.y][pos.x];
      if (tile == Tile::UNKNOWN) {
        tile = Tile::EMPTY;

        fill_queue.push_back(pos + Vector2i<int>{-1, 0});
        fill_queue.push_back(pos + Vector2i<int>{1, 0});
        fill_queue.push_back(pos + Vector2i<int>{0, -1});
        fill_queue.push_back(pos + Vector2i<int>{0, 1});
      }
    }
  }

  print_tiles(tiles);

  // set remaining UNKNOWN tiles to GREEN
  //   here I assume that there are no holes inside the shape
  for (auto& row : tiles) {
    for (auto& tile : row) {
      if (tile == Tile::UNKNOWN) {
        tile = Tile::GREEN;
      }
    }
  }

  print_tiles(tiles);

  // find area of largest rectangle
  // like in part_one, but with checking tile colour
  long area_max = 0;

  for (auto first_it = input.begin(); first_it != input.end() - 1; ++first_it) {
    for (auto second_it = first_it + 1; second_it != input.end(); ++second_it) {
      // find top-left and bottom-right corners
      Vector2i<int> top_left{std::min((*first_it).x, (*second_it).x),
                        std::min((*first_it).y, (*second_it).y)};
      Vector2i<int> bottom_right{std::max((*first_it).x, (*second_it).x),
                            std::max((*first_it).y, (*second_it).y)};
      // check if there aren't any EMPTY tiles in rectangle
      // need to check only sides, not inside (assuming there are no holes)
      bool is_inside = true;
      // upper side
      if (is_inside) {
        for (size_t i = top_left.x; i != bottom_right.x; ++i) {
          if (tiles[top_left.y][i] == Tile::EMPTY) {
            is_inside = false;
            break;
          }
        }
      }
      // lower side
      if (is_inside) {
        for (size_t i = top_left.x; i != bottom_right.x; ++i) {
          if (tiles[bottom_right.y][i] == Tile::EMPTY) {
            is_inside = false;
            break;
          }
        }
      }
      // left side
      if (is_inside) {
        for (size_t i = top_left.y; i != bottom_right.y; ++i) {
          if (tiles[i][top_left.x] == Tile::EMPTY) {
            is_inside = false;
            break;
          }
        }
      }
      // right side
      if (is_inside) {
        for (size_t i = top_left.y; i != bottom_right.y; ++i) {
          if (tiles[i][bottom_right.x] == Tile::EMPTY) {
            is_inside = false;
            break;
          }
        }
      }

      if (is_inside) {
        area_max = std::max(area_max, get_area(*first_it, *second_it));
      }
    }
  }

  return area_max;
}
}  // namespace part_two_naive

namespace part_two {

enum Turn {
  LEFT,
  RIGHT,
#ifdef DEBUG
  // there shouldn't be any of these
  FRONT,
  BACK
#endif
};

struct Corner {
  Vector2i<int> pos;
  // need to be normalized
  Vector2i<int> direction_from;
  Turn turn;
};

struct Wall {
  // x for vertical, y for horizontal
  int common;
  // needs to be sorted in ascending order
  std::array<int, 2> range;
};

struct Rectangle {
  long area;
  std::array<size_t, 2> corner_idxs;
};

/*
`from` and `to` need to be normalized
*/
Turn identify_turn(const Vector2i<int>& from, const Vector2i<int>& to) {
#ifdef DEBUG
  // full check
  // these cryptic conditions are based on rotation matrices
  // also, left and right are swapped, as this is a clockwise coordination
  // system (x points right, y points down; normally it's counter-clockwise)
  if (to.x == -from.y && to.y == from.x) {
    return Turn::RIGHT;
  }
  if (to.x == from.y && to.y == -from.x) {
    return Turn::LEFT;
  }
  if (to.x == from.x && to.y == from.y) {
    return Turn::FRONT;
  }
  if (to.x == -from.x && to.y == -from.y) {
    return Turn::BACK;
  }
#endif
  // it can only turn left or right - it's enough to check this condition
  if (to.x == -from.y && to.y == from.x) {
    return Turn::RIGHT;
  } else {
    return Turn::LEFT;
  }
}

Vector2i<int> rotate(const Vector2i<int>& v, Turn turn) {
  // check `identify_turn` if you don't know what happens here
  if (turn == Turn::LEFT) {
    return {v.y, -v.x};
  } else {  // right
    return {-v.y, v.x};
  }
}

long solution(std::vector<Vector2i<int>> input) {
  // check if rectangle crosses the outer perimeter of shape made by red tiles

  // each red tile is a corner of the shape -
  // add information about shape behavior in that corner
  std::vector<Corner> corners;
  corners.reserve(input.size());

  // first corner - made by last, first and second red tiles
  Vector2i<int> direction_from = (*input.begin() - *(input.end() - 1)).normalize();
  Vector2i<int> direction_to = (*(input.begin() + 1) - *input.begin()).normalize();
  corners.push_back({*input.begin(), direction_from,
                     identify_turn(direction_from, direction_to)});

  // remaining corners
  // this is needed for direction from last to first point
  input.reserve(input.size() + 1);
  input.push_back(input[0]);
  for (auto it = input.begin() + 1; it != input.end() - 1; ++it) {
    direction_from = direction_to;
    direction_to = (*(it + 1) - *it).normalize();
    corners.push_back(
        {*it, direction_from, identify_turn(direction_from, direction_to)});
  }

  // cleanup
  input.pop_back();

  // find the outside of shape
  // by counting left and right turns
  int left_counter = 0;
  int right_counter = 0;
  for (const auto& corner : corners) {
#ifdef DEBUG
    if (corner.turn == Turn::FRONT) {
      throw std::runtime_error("Non-corner red tile detected");
    }
    if (corner.turn == Turn::BACK) {
      throw std::runtime_error("Invalid 180 deg turn detected");
    }
#endif
    if (corner.turn == Turn::LEFT) {
      ++left_counter;
    } else {
      ++right_counter;
    }
  }
  // if there are more right turns, the outside is on the left
  // (from the perspective of progessing through red tiles)
  Turn outside_direction =
      (right_counter > left_counter) ? Turn::LEFT : Turn::RIGHT;

  // make corners of 1-tile-out offset of shape
  // rectangles will be checked for not crossing this offset
  std::vector<Vector2i<int>> offset;
  offset.reserve(corners.size());
  for (const auto& corner : corners) {
    Vector2i<int> pos = corner.pos;
    // move one tile out
    pos += rotate(corner.direction_from, outside_direction);
    // corner is right angle (90 deg)
    if (corner.turn != outside_direction) {
      // move one tile front
      pos += corner.direction_from;
    }
    // corner is reflex angle (270 deg)
    else {
      // move one tile back
      pos -= corner.direction_from;
    }
    offset.push_back(pos);
  }

  // make walls between subsequent offset points
  std::vector<Wall> walls_horizontal;
  walls_horizontal.reserve(offset.size() / 2 + 1);
  std::vector<Wall> walls_vertical;
  walls_vertical.reserve(offset.size() / 2 + 1);

  auto point_current_it = offset.begin();
  auto point_previous_it = offset.end() - 1;
  // detect if first wall is horizontal or vertical
  bool is_current_wall_horizontal =
      (*point_current_it).y == (*point_previous_it).y;
  for (; point_current_it != offset.end(); ++point_current_it) {
    if (is_current_wall_horizontal) {
      int common = (*point_current_it).y;
      std::array<int, 2> range{(*point_current_it).x, (*point_previous_it).x};
      // range needs to be sorted
      if (range[0] > range[1]) {
        std::swap(range[0], range[1]);
      }
      walls_horizontal.push_back({common, range});
    } else {  // vertical
      int common = (*point_current_it).x;
      std::array<int, 2> range{(*point_current_it).y, (*point_previous_it).y};
      if (range[0] > range[1]) {
        std::swap(range[0], range[1]);
      }
      walls_vertical.push_back({common, range});
    }
    // subsequent walls are alternating between horizontal and vertical
    is_current_wall_horizontal = !is_current_wall_horizontal;
    point_previous_it = point_current_it;
  }

  // sort vectors of walls by ascending `common`
  // i.e. from top to bottom for horizontal and from left to right for vertical
  auto sort_fun = [](const Wall& a, const Wall& b) {
    return b.common > a.common;
  };
  std::sort(walls_horizontal.begin(), walls_horizontal.end(), sort_fun);
  std::sort(walls_vertical.begin(), walls_vertical.end(), sort_fun);

  // make all rectangles
  std::vector<Rectangle> rectangles;
  rectangles.reserve((input.size() - 1) * input.size() / 2);
  size_t first_idx = 0;
  for (auto first_it = input.begin(); first_it != input.end() - 1; ++first_it) {
    size_t second_idx = first_idx + 1;
    for (auto second_it = first_it + 1; second_it != input.end(); ++second_it) {
      rectangles.push_back(
          {get_area(*first_it, *second_it), {first_idx, second_idx}});

      ++second_idx;
    }
    ++first_idx;
  }
  // sort rectangles from greatest area
  std::sort(
      rectangles.begin(), rectangles.end(),
      [](const Rectangle& a, const Rectangle& b) { return b.area < a.area; });

  // find first (greatest) rectangle that doesn't cross offset walls
  long max_area = 0;
  for (const auto& rectangle : rectangles) {
    std::array<int, 2> sides_x{input[rectangle.corner_idxs[0]].x,
                               input[rectangle.corner_idxs[1]].x};
    std::sort(sides_x.begin(), sides_x.end());
    std::array<int, 2> sides_y{input[rectangle.corner_idxs[0]].y,
                               input[rectangle.corner_idxs[1]].y};
    std::sort(sides_y.begin(), sides_y.end());

    // check if any side of the rectangle crosses any offset wall - it means
    // that part of the rectangle is outside the shape
    bool is_valid = true;
    // check if horizontal walls cross vertical rectangle sides
    for (const auto& wall_horizontal : walls_horizontal) {
      // only check walls that may cross the rectangle side
      // (it works because walls and sides are sorted)
      if (wall_horizontal.common < sides_y[0]) {
        continue;
      }
      if (wall_horizontal.common > sides_y[1]) {
        break;
      }
      for (const auto& side_x : sides_x) {
        // check if side crosses the wall
        if (wall_horizontal.range[0] <= side_x &&
            side_x <= wall_horizontal.range[1]) {
          is_valid = false;
          break;
        }
      }
      if (!is_valid) {
        break;
      }
    }
    if (!is_valid) {
      continue;
    }
    // check if vertical walls cross horizontal rectangle sides
    // same algorithm as before
    for (const auto& wall_vertical : walls_vertical) {
      if (wall_vertical.common < sides_x[0]) {
        continue;
      }
      if (wall_vertical.common > sides_x[1]) {
        break;
      }
      for (const auto& side_y : sides_y) {
        // check if side crosses the wall
        if (wall_vertical.range[0] <= side_y &&
            side_y <= wall_vertical.range[1]) {
          is_valid = false;
          break;
        }
      }
    }
    if (!is_valid) {
      continue;
    }
    max_area = rectangle.area;
    break;
  }

  return max_area;
}
}  // namespace part_two

int main() {
  const std::string input_filename = "input.txt";
  // const std::string input_filename = "input_example.txt";
  // const std::string input_filename = "input_test.txt";

  auto execution_timer = ExecutionTimer();

  auto input = parse(read_file(input_filename));
  // auto result = part_one::solution(input);
  // auto result = part_two_naive::solution(input);
  auto result = part_two::solution(input);

  auto execution_time_us = execution_timer.GetTimeUs();

  std::cout << "Result:\n" << result << std::endl;
  std::cout << "Execution time [us]: " << execution_time_us << std::endl;

  return 0;
}