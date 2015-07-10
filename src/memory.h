#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#include "computer.h"

#define MEM_SIZE 128

typedef struct computer computer_t;

typedef enum memory_error
{
	out_of_memory,
	invalid_address
} memory_error_t;

typedef enum memory_state
{
	memory_ready,
	memory_error,
	memory_reading,
	memory_writing
} memory_state_t;

typedef struct memory
{
	uint8_t        content[MEM_SIZE];
	uint32_t	   size;
	memory_state_t state;
	memory_error_t error;
} memory_t;

memory_t *memory_create  (computer_t *computer);

uint8_t   memory_read8   (computer_t *computer, uint32_t addr);
uint32_t  *memory_read32 (computer_t *computer, uint32_t addr);

void	  memory_write8  (computer_t *computer, uint32_t addr, uint8_t  value);
void	  memory_write32 (computer_t *computer, uint32_t addr, uint32_t value);

#endif
