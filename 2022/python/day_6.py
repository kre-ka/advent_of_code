from collections import deque
from utils import input_reader


class Day6Task:
    mark_len = 14
    input = []
    sign_queue = deque()

    def __init__(self) -> None:
        self.input = input_reader('input')[0]
        self.input = [sign for sign in self.input]
        self.input.reverse()
        
        # load first signs to queue
        for _ in range(self.mark_len):
            self.sign_queue.append(self.input.pop())
        
    # check all signs for repetition and return position of first sign of furthest pair
    # return -1 if no pair found
    def check_for_repeated_sign(self, num_to_check) -> int:
        pair_pos = -1
        for i in range(-1, -num_to_check-1, -1):
            for j in range(i-1,-self.mark_len-1, -1):
                if self.sign_queue[i] == self.sign_queue[j]:
                    new_pair_pos = self.mark_len+j
                    if new_pair_pos > pair_pos:
                        pair_pos = new_pair_pos
        return pair_pos

    def move_queue(self, amount):
        for _ in range(amount):
            self.sign_queue.popleft()
            self.sign_queue.append(self.input.pop())

    # return position of first marker
    def part_one(self) -> int:
        # first repetition check - needs to be full
        # get number of signs to skip to allow quick check
        pair_pos = self.check_for_repeated_sign(self.mark_len-1)
        # check if mark didn't happen to be here
        if pair_pos == -1:
            return 0
        
        # skip some signs to allow quick repetition check
        self.move_queue(pair_pos+1)
        
        i = pair_pos+1
        while True:
            pair_pos = self.check_for_repeated_sign(pair_pos+1)
            if pair_pos == -1:
                return i+self.mark_len
            self.move_queue(pair_pos+1)
            i += pair_pos+1


day_6_task = Day6Task()
print(f"first marker position: {day_6_task.part_one()}")