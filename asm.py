import sys

instruction_codes = [
	('mov', 0x1),
	('jmp', 0x2),
	('add', 0x3),
	('sht', 0x4),
	('out', 0x5),
	('in' , 0x6)
]

def main():
	input_path = sys.argv[1];
	output_path = sys.argv[2];

	in_f = open(input_path, 'r')
	ou_f = open(output_path, 'wb')

	for line in in_f:
		ou_f.write(asm_line(line))

def asm_line(line):
	binary = bytearray()
	elements = line.split(' ')
	elements = [e.replace('\n', '') for e in elements]
	instruction_code = get_instruction_code(elements[0])
	operands = asm_operands(elements[1:])

	binary.append(instruction_code)
	binary += operands
	return binary

def get_instruction_code(word):
	for instruction in instruction_codes:
		if instruction[0] == word:
			return instruction[1]
	print('[ERROR] Unknown instruction: {0}'.format(word))

def asm_operands(operands):
	array = bytearray()
	for operand in operands:
		array += asm_operand(operand.replace(',', ''))
	return array

def asm_operand(operand_str):
	array = bytearray()
	if operand_str[0] == 'r':
		array.append(0)
		array += bytearray(int32_to_int8(int(operand_str[1:])))
	elif operand_str.startswith('0x'):
		operand_str = operand_str.replace('0x', '')
		array.append(2)
		array += bytearray(int32_to_int8(int(operand_str, 16)))
	else:
		print('[ERROR] Bad operand: {0}'.format(operand_str))
	return array

def int32_to_int8(int32):
	mask = (1 << 8) - 1
	return [(int32 >> k) & mask for k in range(0, 32, 8)]

if __name__ == '__main__':
	main()
