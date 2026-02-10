from utils import input_reader


def parse() -> tuple[list[int], list[int]]:
    input = input_reader()
    left_list = []
    right_list = []
    for line in input:
        left, right = [int(item) for item in line.split()]
        left_list.append(left)
        right_list.append(right)
    return left_list, right_list


def first():
    left_list, right_list = parse()
    left_list.sort()
    right_list.sort()

    result = 0
    for left_item, right_item in zip(left_list, right_list):
        result += abs(left_item - right_item)

    return result


def second():
    left_list, right_list = parse()

    result = 0
    for left_item in left_list:
        count = 0
        for right_item in right_list:
            if right_item == left_item:
                count += 1
        result += left_item * count
    return result


if __name__ == "__main__":
    # print(first())
    print(second())
