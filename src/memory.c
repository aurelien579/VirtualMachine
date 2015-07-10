#include "memory.h"
#include <stdlib.h>
#include <string.h>

static void    memory_set_error (memory_t *memory, memory_error_t error);
static uint8_t check_address    (memory_t *memory, uint32_t addr);

memory_t *memory_create(computer_t *computer)
{
	memory_t *memory = (memory_t*)malloc(sizeof(memory_t));

	memset(memory->content, 0, MEM_SIZE);

	memory->size = MEM_SIZE;

	return memory;
}

uint8_t memory_read8(computer_t *computer, uint32_t addr)
{
	if (check_address(computer->memory, addr))
		return computer->memory->content[addr];
	else
		return 0;
}

uint32_t *memory_read32(computer_t *computer, uint32_t addr)
{
	if (check_address(computer->memory, addr))
		return (uint32_t*)&computer->memory->content[addr];
	else
		return 0;
}

void memory_write8(computer_t *computer, uint32_t addr, uint8_t  value)
{
	if (check_address(computer->memory, addr))
		computer->memory->content[addr] = value;
}

void memory_write32(computer_t *computer, uint32_t addr, uint32_t value)
{
	if (check_address(computer->memory, addr))
		*(uint32_t*)&computer->memory->content[addr] = value;
}

static uint8_t check_address(memory_t *memory, uint32_t addr)
{
	if (addr > memory->size)
	{
		memory_set_error(memory, out_of_memory);
		return 0;
	}
	else
		return 1;
}

static void memory_set_error(memory_t *memory, memory_error_t memory_error_value)
{
	memory->state = memory_error;
	memory->error = memory_error_value;
}
