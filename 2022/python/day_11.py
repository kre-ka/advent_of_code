from __future__ import annotations
from typing import List
from collections import deque
from math import floor

from utils import input_reader

monkeys = []
optimization_factor = 1

class Monkey:
    def __init__(self, description: List[str]) -> None:
        self.number = int(description[0].split()[1][:-1])
        self.items = deque()
        for item in description[1].split(':')[1].split(','):
            self.items.append(int(item))
        self.operation = description[2].split(':')[1].split('=')[1][:-1]
        self.test_value = int(description[3].split()[-1])
        self.target_test_true = int(description[4].split()[-1])
        self.target_test_false = int(description[5].split()[-1])
        self.inspection_counter = 0
    
    def test(self, item: int):
        return item % self.test_value == 0
    
    def make_turn(self):
        while len(self.items) != 0:
            self.inspection_counter += 1
            old = self.items.popleft()
            new = eval(self.operation)
            # new = floor(new / 3)
            new = optimize_item(new)
            target = self.target_test_true if self.test(new) else self.target_test_false
            monkeys[target].items.append(new)
            
def set_optimization_factor():
    global optimization_factor
    optimization_factor = 1
    for monkey in monkeys:
        optimization_factor *= monkey.test_value

def optimize_item(item: int) -> int:
    if item >= optimization_factor:
        item = item % optimization_factor
    return item

def slice_monkey_descriptions() -> List[List[str]]:
    input = input_reader('input')
    monkey_descriptions = []
    description = []
    for line in input:
        if line == '\n':
            monkey_descriptions.append(description)
            description = []
        else:
            description.append(line)
    return monkey_descriptions

def part_one():
    monkey_descriptions = slice_monkey_descriptions()
    for description in monkey_descriptions:
        monkeys.append(Monkey(description))
    
    set_optimization_factor()
    
    num_of_rounds = 10000
    for i in range(num_of_rounds):
        print(i)
        for monkey in monkeys:
            monkey.make_turn()
    
    inspection_list = []
    for monkey in monkeys:
        inspection_list.append(monkey.inspection_counter)
        print(f"monkey {monkey.number} inspection count: {monkey.inspection_counter}")
    
    inspection_list.sort(reverse=True)
    print(f"monkey business = {inspection_list[0]} * {inspection_list[1]} = {inspection_list[0]*inspection_list[1]}")
    
    


part_one()