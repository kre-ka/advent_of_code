from typing import Iterator, Iterable

def input_reader(path="input.txt") -> list[str]:
    with open(path) as f:
        return f.readlines()

def strip(input: list[str]) -> list[str]:
    for i in range(len(input)):
        input[i] = input[i].strip()
    return input

def input_reader_generator(path="input.txt") -> Iterator[str]:
    with open(path) as f:
        yield f.readline()

def strip_generator(input: Iterable[str]) -> Iterator[str]:
    for line in input:
        yield line.strip()