#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "computer.h"

#define REGS_COUNT	6

typedef struct computer computer_t;

typedef enum cpu_state
{
	CPU_STATE_RUNNING,
	CPU_STATE_PAUSED,
	CPU_STATE_ERROR,
	CPU_STATE_SHUTDOWN
} cpu_state_t;

typedef enum cpu_error
{
	CPU_ERROR_NO,
	CPU_ERROR_INVALID_INSTRUCTION,
	CPU_ERROR_INVALID_OPERAND
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
	OPERAND_TYPE_REGISTER_VALUE,
	OPERAND_TYPE_REGISTER_POINTER,
	OPERAND_TYPE_VALUE,
	OPERAND_TYPE_POINTER
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
