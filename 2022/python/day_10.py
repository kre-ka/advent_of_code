from utils import input_reader


def part_one():
    input = input_reader('input')
    input = [command.split() for command in input]
    input.reverse()

    cycles_to_check = [20, 60, 100, 140, 180, 220]

    cycle_num = 1
    register_value = 1
    value_to_add = 0
    is_processing = False
    processing_time = 0
    signal_strength_sum = 0
    while len(input) != 0:
        # process command
        if is_processing:
            processing_time += -1
            if processing_time == 0:
                is_processing = False
                register_value += value_to_add
        
        # read new command
        if not is_processing:
            command = input.pop()
            if command[0] == 'addx':
                processing_time = 2
                value_to_add = int(command[1])
                is_processing = True
        
        # check signal strength at given cycles
        if cycle_num == cycles_to_check[0]:
            cycles_to_check.pop(0)
            signal_strength = register_value * cycle_num
            signal_strength_sum += signal_strength
            if len(cycles_to_check) == 0:
                break

        cycle_num += 1
    print(signal_strength_sum)

def part_two():
    input = input_reader('input')
    input = [command.split() for command in input]
    input.reverse()

    cycle_num = 0
    sprite_pos = 1
    value_to_add = 0
    is_processing = False
    processing_time = 0
    image = ""
    pixel_pos = 0

    while len(input) != 0:
        cycle_num += 1

        # read new command
        if not is_processing:
            command = input.pop()
            if command[0] == 'addx':
                processing_time = 2
                value_to_add = int(command[1])
                is_processing = True
        
        # render pixel
        image += '#' if (cycle_num-1)%40 in (sprite_pos-1, sprite_pos, sprite_pos+1) else '.'
        
        # process command
        if is_processing:
            processing_time += -1
            if processing_time == 0:
                is_processing = False
                sprite_pos += value_to_add

    for i in range(6):
        print(image[40*i:40*(i+1)])
    

# part_one()
part_two()