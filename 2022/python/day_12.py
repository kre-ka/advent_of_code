from dataclasses import dataclass
from collections import deque

from utils import input_reader

@dataclass
class Square:
    x: int
    y: int
    height: int
    visited: bool = False
    steps: int = -1
    queued: bool = False

class Map:
    def __init__(self) -> None:
        input = input_reader('input')
        self.squares = [[square for square in line[:-1]] for line in input]
        start_coords = self.find_sign('S')
        self.squares[start_coords[1]][start_coords[0]] = 'a'
        end_coords = self.find_sign('E')
        self.squares[end_coords[1]][end_coords[0]] = 'z'
        self.squares = [[ord(square) - ord('a') for square in row] for row in self.squares]
        self.squares = [[Square(x, y, height) for x, height in enumerate(row)] for y, row in enumerate(self.squares)]
        self.start = self.squares[start_coords[1]][start_coords[0]]
        self.end = self.squares[end_coords[1]][end_coords[0]]
    
    def find_sign(self, sign: str) -> tuple:
        for y, row in enumerate(self.squares):
            if sign in row:
                x = row.index(sign)
                return (x, y)

    def test_pass(self, pos_height: int, target_height:int) -> bool:
        return target_height - pos_height <= 1
    
    def flood_fill(self, start: Square):
        square_queue = deque()
        square_queue.append((start, 0))
        while len(square_queue) > 0:
            square, steps = square_queue.popleft()
            # print(f"visiting square {square.x} {square.y}")
            square.visited = True
            square.steps = steps
            # left
            if square.x > 0:
                target_square = self.squares[square.y][square.x-1]
                if not target_square.visited and \
                        self.test_pass(target_square.height, square.height) and \
                        not target_square.queued:
                    target_square.queued = True
                    square_queue.append((target_square, steps+1))
            # right
            if square.x < len(self.squares[0]) - 1:
                target_square = self.squares[square.y][square.x+1]
                if not target_square.visited and \
                        self.test_pass(target_square.height, square.height) and \
                        not target_square.queued:
                    target_square.queued = True
                    square_queue.append((target_square, steps+1))
            # top
            if square.y > 0:
                target_square = self.squares[square.y-1][square.x]
                if not target_square.visited and \
                        self.test_pass(target_square.height, square.height) and \
                        not target_square.queued:
                    target_square.queued = True
                    square_queue.append((target_square, steps+1))
            # bottom
            if square.y < len(self.squares) - 1:
                target_square = self.squares[square.y+1][square.x]
                if not target_square.visited and \
                        self.test_pass(target_square.height, square.height) and \
                        not target_square.queued:
                    target_square.queued = True
                    square_queue.append((target_square, steps+1))


def part_one():
    map = Map()
    map.flood_fill(map.end)
    print(f"number of steps from start: {map.start.steps}")

def part_two():
    map = Map()
    map.flood_fill(map.end)
    min_steps = map.start.steps
    for row in map.squares:
        for square in row:
            if square.height == 0 and square.visited:
                min_steps = min(min_steps, square.steps)
    print(f"number of steps from elevation a: {min_steps}")


# part_one()
part_two()