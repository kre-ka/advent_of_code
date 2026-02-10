from typing import List

def input_reader(path: str) -> List[str]:
    with open(path) as f:
        return f.readlines()