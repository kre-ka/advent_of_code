from utils import input_reader, strip
from utils import input_reader_generator, strip_generator


def findall(input: list[str], substring="XMAS") -> int:
    count = 0
    for line in input:
        found_idx = 0
        while True:
            found_idx = line.find(substring, found_idx)
            if found_idx == -1:
                break
            found_idx += len(substring)
            count += 1
    return count


def reverse(input: list[str]) -> list[str]:
    return [line[::-1] for line in input]


def first():
    input = strip(input_reader())
    # input is square

    count = 0

    # close modified versions of input in local scopes
    def horizontal():
        nonlocal count

        # left_to_right
        count += findall(input)
        # right_to_left
        count += findall(reverse(input))

    horizontal()

    def vertical():
        nonlocal count

        # transform to columns
        input_alt = [""] * len(input[0])  # assumes each row is the same length
        for line in input:
            for i in range(len(line)):
                input_alt[i] += line[i]

        count += findall(input_alt)
        count += findall(reverse(input_alt))

    vertical()

    def diagonal():
        nonlocal count

        # transform to diagonals bottom_left_to_top_right
        input_alt = [""] * (len(input) + len(input[0]) - 1)
        for i in range(len(input[0])):
            for j in range(len(input)):
                input_alt[i + j] += input[j][i]

        count += findall(input_alt)
        count += findall(reverse(input_alt))

        # transform to diagonals bottom_right_to_top_left
        input_alt = [""] * (len(input) + len(input[0]) - 1)
        for i in range(len(input[0])):
            for j in range(len(input)):
                input_alt[i - j - (len(input) - 1)] += input[j][i]

        count += findall(input_alt)
        count += findall(reverse(input_alt))

    diagonal()

    return count


def second():
    input = strip(input_reader())
    # input = strip(input_reader("input_example.txt"))

    count = 0
    for i in range(1, len(input) - 1):
        for j in range(1, len(input[0]) - 1):
            if input[i][j] == "A":
                if (
                    (input[i - 1][j - 1] == "M" and input[i + 1][j + 1] == "S")
                    or (input[i - 1][j - 1] == "S" and input[i + 1][j + 1] == "M")
                ) and (
                    (input[i - 1][j + 1] == "M" and input[i + 1][j - 1] == "S")
                    or (input[i - 1][j + 1] == "S" and input[i + 1][j - 1] == "M")
                ):
                    count += 1
    return count

def second_with_generators():
    input = strip_generator(input_reader_generator())

if __name__ == "__main__":
    # print(first())
    print(second())

    # x = ''
    # for letter in "dupa":
    #     x += letter
    # print(x)
