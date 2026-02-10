import numpy as np

from utils import input_reader


def parse() -> list[np.ndarray[int]]:
    input = input_reader()
    out = []
    for line in input:
        out.append(np.array([int(item) for item in line.split()]))

    return out


def first():
    input = parse()

    count = 0
    for item in input:
        diff = np.diff(item)
        diff_abs = np.abs(diff)
        if (np.all(diff > 0) or np.all(diff < 0)) and (
            np.all(diff_abs >= 1) and np.all(diff_abs <= 3)
        ):
            count += 1

    return count


def second():
    input = parse()

    count = 0
    for item in input:
        diff = np.diff(item)
        diff_abs = np.abs(diff)
        if (np.all(diff > 0) or np.all(diff < 0)) and (
            np.all(diff_abs >= 1) and np.all(diff_abs <= 3)
        ):
            count += 1
            continue

        # with corrections
        for i in range(item.size):
            item_corrected = np.delete(item, i)
            diff = np.diff(item_corrected)
            diff_abs = np.abs(diff)
            if (np.all(diff > 0) or np.all(diff < 0)) and (
                np.all(diff_abs >= 1) and np.all(diff_abs <= 3)
            ):
                count += 1
                break

    return count


if __name__ == "__main__":
    # print(first())
    print(second())
