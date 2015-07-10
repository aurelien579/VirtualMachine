#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "computer.h"

#define REGS_COUNT	6

typedef struct computer computer_t;

typedef enum cpu_state
{
	running,
	paused,
	error,
	shutdown
} cpu_state_t;

typedef enum cpu_error
{
	no_error,
	invalid_instruction,
	invalid_operand
} cpu_error_t;

typedef struct cpu
{
	computer_t *computer;
	uint32_t    ip;
	uint32_t	registers[REGS_COUNT];
	cpu_state_t state;
	cpu_error_t error;
} cpu_t;

enum operand_type
{
	register_value,
	register_pointer,
	value,
	pointer
};

typedef uint8_t operand_type_t;

typedef struct operand
{
	operand_type_t type;
	uint32_t       value;
} __attribute__((packed)) operand_t;

cpu_t *cpu_create (computer_t *computer);
void   cpu_run    (cpu_t *cpu);

#endif
