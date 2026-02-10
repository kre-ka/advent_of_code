from typing import List, Tuple, Dict
from utils import input_reader


def split_input(input: List[str]) -> Tuple[List[str], List[str]]:
    empty_idx = 0
    for item in input:
        if item == '\n':
            break
        empty_idx += 1
    crate_arrangement = input[:empty_idx-1]
    move_list = input[empty_idx+1:]
    return crate_arrangement, move_list

def get_crate_stacks(crate_arrangement: List[str]) -> List[List[str]]:
    num_stacks = int(len(crate_arrangement[0]) / 4)
    crate_stacks = [[] for _ in range(num_stacks)]
    for line in crate_arrangement:
        for i in range(num_stacks):
            sign = line[1 + i*4]
            if sign != ' ':
                crate_stacks[i].append(sign)
    for stack in crate_stacks:
        stack.reverse()
    return crate_stacks

# already fixing for crate stacks list starting at 0
def get_moves(move_list_str: List[str]) -> List[Dict]:
    move_list = []
    for line in move_list_str:
        line = line.split()
        move_list.append({'amount': int(line[1]), 'from': int(line[3])-1, 'to': int(line[5])-1})
    move_list.reverse()
    return move_list

def make_move_9000(crate_stacks: List[List[str]], move: Dict):
    for _ in range(move['amount']):
        crate_stacks[move['to']].append(crate_stacks[move['from']].pop())

def make_move_9001(crate_stacks: List[List[str]], move: Dict):
    moved_crates = crate_stacks[move['from']][-move['amount']:]
    crate_stacks[move['from']] = [crate_stacks[move['from']][i] for i in range(len(crate_stacks[move['from']])-move['amount'])]
    crate_stacks[move['to']].extend(moved_crates)

def get_top_crates(crate_stacks: List[List[str]]) -> str:
    output = ''
    for stack in crate_stacks:
        output += stack[-1]
    return output

def part_one():
    input = input_reader('input')
    crate_stacks, move_list = split_input(input)
    crate_stacks = get_crate_stacks(crate_stacks)
    move_list = get_moves(move_list)
    for _ in range(len(move_list)):
        make_move_9000(crate_stacks, move_list.pop())
    print(get_top_crates(crate_stacks))

def part_two():
    input = input_reader('input')
    crate_stacks, move_list = split_input(input)
    crate_stacks = get_crate_stacks(crate_stacks)
    move_list = get_moves(move_list)
    for _ in range(len(move_list)):
        make_move_9001(crate_stacks, move_list.pop())
    print(get_top_crates(crate_stacks))


# part_one()
part_two()