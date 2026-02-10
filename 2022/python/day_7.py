from __future__ import annotations
from typing import List
from utils import input_reader


# a tree structure, basically
class Directory:
    def __init__(self, parent_dir=None) -> None:
        if parent_dir is not None:
            self.parent_dir = parent_dir
        self.dirs = {}
        self.files = {}
        self.size = None
    
    def add_dir(self, name: str):
        self.dirs[name] = Directory(self)
    
    def add_file(self, name: str, size: str):
        self.files[name] = int(size)
    
    def cd(self, dir: str):
        if dir == '..':
            return self.parent_dir
        else:
            return self.dirs[dir]
    
    def calc_size(self):
        if self.size is None:
            total_size = 0
            for size in self.files.values():
                total_size += size
            for dir in self.dirs.values():
                dir.calc_size()
                total_size += dir.size
            self.size = total_size



def find_command(line: List[str]) -> bool:
    return line[0] == '$'

def populate_tree(input: List[List[str]]) -> Directory:
    root_dir = Directory()
    current_dir = root_dir
    # discard first line: '$ cd /'
    input.pop()

    while len(input) != 0:
        line = input.pop()
        if find_command(line):
            if line[1] == 'cd':
                current_dir = current_dir.cd(line[2])
            elif line[1] == 'ls':
                continue
        else:
            if line[0] == 'dir':
                current_dir.add_dir(line[1])
            else:
                current_dir.add_file(line[1], line[0])
    return root_dir

def common_part() -> Directory:
    input = input_reader('input')
    # reverse so you can pop lines from the top
    input.reverse()
    input = [line.split() for line in input]

    root_dir = populate_tree(input)
    # to caluculate root dir size you actually need to calculate all directory sizes
    root_dir.calc_size()
    return root_dir

def part_one():
    max_size = 100000
    sum_sizes_part_one = 0
    def calc_size_sum(dir: Directory):
        if dir.size <= max_size:
            nonlocal sum_sizes_part_one
            sum_sizes_part_one += dir.size
        for child_dir in dir.dirs.values():
            calc_size_sum(child_dir)
    root_dir = common_part()
    calc_size_sum(root_dir)
    print(sum_sizes_part_one)

def part_two():
    root_dir = common_part()
    total_space = 70000000
    needed_free_space = 30000000
    free_space = total_space - root_dir.size
    space_to_clear = needed_free_space - free_space
    closest_dir_size = root_dir.size
    def find_dir_to_clear(dir: Directory):
        if dir.size >= space_to_clear:
            nonlocal closest_dir_size
            if dir.size < closest_dir_size:
                closest_dir_size = dir.size
            for child_dir in dir.dirs.values():
                find_dir_to_clear(child_dir)
    find_dir_to_clear(root_dir)
    print(closest_dir_size)


# part_one()
part_two()