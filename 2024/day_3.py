import re

from utils import input_reader


def process_mul(input: str) -> int:
    items = re.findall("mul\(\d{1,3},\d{1,3}\)", input)

    result = 0
    for item in items:
        left, right = [int(number) for number in re.findall("\d{1,3}", item)]
        result += left * right
    return result

def first():
    input = input_reader()

    result = 0
    for line in input:
        result += process_mul(line)
    return result


def second():
    input = input_reader()

    result = 0
    for line in input:
        by_dont = re.split("don't\(\)", line)
        for item in by_dont:
            re.split("do\(\)", item)
        # first split by 'don't()', then by 'do()'
        parsed = [re.split("do\(\)", item) for item in re.split("don't\(\)", line)]
        result += process_mul(parsed[0][0])  # first is always 'do'
        for by_dont in parsed:
            for by_do in by_dont[1:]:
                result += process_mul(by_do)
    return result


if __name__ == "__main__":
    # print(first())
    print(second())
