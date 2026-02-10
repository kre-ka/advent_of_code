from typing import List
from utils import input_reader


correction_lower = ord('a') - 1
correction_upper = ord('A') - 27
def unicode_to_priority(set: str) -> List[int]:
    set_new = []
    for letter in set.strip():
        correction = correction_lower if letter.islower() else correction_upper
        set_new.append(ord(letter) - correction)
        # print(f'{letter} -> {ord(letter)} -> {ord(letter) - correction}')
    return set_new

num_steps = 0
def comparator_basic(set_0: List[int], set_1: List[int]):
    global num_steps
    for item_0 in set_0:
        for item_1 in set_1:
            num_steps += 1
            if item_0 == item_1:
                return item_0

def comparator_cut_in_half(set_0: List[int], set_1: List[int], find_all=False):
    def search(item, set):
        global num_steps
        num_steps += 1
        cut_idx = int(len(set)/2)
        # check at cut index
        if item == set[cut_idx]:
            return True
        # stop if all checked (first half)
        if len(set) == 1:
            return False
        # search in proper half of the set
        # second half
        if item > set[cut_idx]:
            # stop if all checked (second half)
            if cut_idx+1 >= len(set):
                return False
            return search(item, set[cut_idx+1:])
        # first half
        else:
            return search(item, set[:cut_idx])

    # remove duplicates
    set_0 = list(set(set_0))
    set_1 = list(set(set_1))

    set_0.sort()
    set_1.sort()

    if find_all:
        common_items = []
    
    for item in set_0:
        if search(item, set_1):
            if not find_all:
                return item
            else:
                common_items.append(item)
    return common_items

def part_one():
    input = input_reader('python/input')
    sum = 0
    for rucksack in input:
        rucksack = unicode_to_priority(rucksack)
        compartment_0, compartment_1 = rucksack[:len(rucksack)//2], rucksack[len(rucksack)//2:]
        # common_item = comparator_basic(compartment_0, compartment_1)
        common_item = comparator_cut_in_half(compartment_0, compartment_1)
        sum += common_item
    print('sum: ', sum)
    print('num_steps: ', num_steps)

def part_two():
    input = input_reader('input')
    sum = 0
    rucksack_groups = [input[i:i+3] for i in range(0, len(input), 3)]
    for group in rucksack_groups:
        group = [unicode_to_priority(rucksack) for rucksack in group]
        common_items_0_1 = comparator_cut_in_half(group[0], group[1], True)
        common_item = comparator_cut_in_half(common_items_0_1, group[2])
        sum += common_item
    print('sum: ', sum)
    print('num_steps: ', num_steps)
        

part_one()
# part_two()
