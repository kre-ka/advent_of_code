from enum import Enum
from utils import input_reader


class Move(Enum):
    ROCK = 0
    PAPER = 1
    SCISSORS = 2

class Result(Enum):
    LOSS = 0
    TIE = 1
    WIN = 2

# my move      : rock, paper, scissors
# opponent move:
#           rock
#          paper
#       scissors
result_table = [[Result.TIE, Result.WIN, Result.LOSS],
                [Result.LOSS, Result.TIE, Result.WIN],
                [Result.WIN, Result.LOSS, Result.TIE]]

# moves required to get specific result
# opponent move: rock, paper, scissors
# result:
#           loss
#           tie
#           win
move_table = [[Move.SCISSORS, Move.ROCK, Move.PAPER],
              [Move.ROCK, Move.PAPER, Move.SCISSORS],
              [Move.PAPER, Move.SCISSORS, Move.ROCK]]

result_scoring = {Result.LOSS: 0, Result.TIE: 3, Result.WIN: 6}
move_scoring = {Move.ROCK: 1, Move.PAPER: 2, Move.SCISSORS: 3}

move_opponent_decoder = {'A': Move.ROCK, 'B': Move.PAPER, 'C': Move.SCISSORS}
move_myself_decoder = {'X': Move.ROCK, 'Y': Move.PAPER, 'Z': Move.SCISSORS}
result_decoder = {'X': Result.LOSS, 'Y': Result.TIE, 'Z': Result.WIN}

input = input_reader('input')



def check_result(move_myself, move_opponent):
    return result_table[move_opponent.value][move_myself.value]

def part_one():
    score = 0
    for line in input:
        move_opponent, move_myself = line.split()
        move_opponent = move_opponent_decoder[move_opponent]
        move_myself = move_myself_decoder[move_myself]
        
        score += result_scoring[check_result(move_myself, move_opponent)]
        score += move_scoring[move_myself]
    return score

def check_move(result, move_opponent):
    return move_table[result.value][move_opponent.value]

def part_two():
    score = 0
    for line in input:
        move_opponent, result = line.split()
        move_opponent = move_opponent_decoder[move_opponent]
        result = result_decoder[result]
        
        score += result_scoring[result]
        move_myself = check_move(result, move_opponent)
        score += move_scoring[move_myself]
    return score



# score = part_one()
score = part_two()
print(score)