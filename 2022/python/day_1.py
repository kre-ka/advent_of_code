from utils import input_reader


input = input_reader('input')

max_calories = [0, 0, 0]
current_calories = 0
for line in input:
    if line == '\n':
        if current_calories > max_calories[2]:
            max_calories[2] = current_calories
            max_calories.sort(reverse=True)
        current_calories = 0
    else:
        current_calories += int(line)

print(f'{max_calories}\n\n{sum(max_calories)}')