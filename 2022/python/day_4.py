from typing import List
from utils import input_reader


def parse_input(input: List[str]) -> List[List[List[int]]]:
    input = [[[int(bound) for bound in range.split('-')] for range in line.split(',')] for line in input]
    return input
        

def part_one():
    input = input_reader('input')
    input = parse_input(input)

    count = 0
    for pair in input:
        if (pair[0][0] >= pair[1][0] and pair[0][1] <= pair[1][1]) or \
           (pair[0][0] <= pair[1][0] and pair[0][1] >= pair[1][1]):
            # print(pair)
            count += 1
    print(f"number of fully contained pairs: {count}")
    
def part_two():
    def find_overlap(pair: List[List[int]]):
        for i in range(2):
            for j in range(2):
                if (pair[i][j] >= pair[not i][j] and pair[i][j] <= pair[not i][not j]):
                    return 1
        return 0

    input = input_reader('input')
    input = parse_input(input)

    count = 0
    for pair in input:
        count += find_overlap(pair)
    print(f"number of overlapping pairs: {count}")

# part_one()
part_two()

# (pair[1][0] >= pair[0][0] and pair[1][0] <= pair[0][1]) or \
# (pair[1][1] >= pair[0][0] and pair[1][1] <= pair[0][1])
# (pair[0][0] >= pair[1][0] and pair[0][0] <= pair[1][1]) or \
# (pair[0][1] >= pair[1][0] and pair[0][1] <= pair[1][1])