#include "cpu.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSTRUCTIONS_COUNT	7

typedef void (*instruction_t) (cpu_t*);

static void 				cpu_exec_next_instruction	(cpu_t *cpu);
static void 				cpu_exec_halt				(cpu_t *cpu);
static void 				cpu_exec_mov				(cpu_t *cpu);
static void 				cpu_exec_jmp				(cpu_t *cpu);
static void 				cpu_exec_add				(cpu_t *cpu);
static void 				cpu_exec_shutdown			(cpu_t *cpu);
static void 				cpu_exec_device_out 		(cpu_t *cpu);
static void 				cpu_exec_device_in 			(cpu_t *cpu);

static inline void 			cpu_set_error			(cpu_t *cpu,
													 cpu_error_t cpu_error);

static inline operand_t* 	cpu_get_next_operand		(cpu_t *cpu);
static uint32_t 			cpu_read_operand			(cpu_t *cpu, operand_t *operand);
static void 				cpu_write_operand			(cpu_t *cpu, operand_t *operand,
													 uint32_t val);

static void 				cpu_dump				(cpu_t *cpu);

static instruction_t instructions[INSTRUCTIONS_COUNT] =
{
	cpu_exec_halt,
	cpu_exec_mov,
	cpu_exec_jmp,
	cpu_exec_add,
	cpu_exec_shutdown,
	cpu_exec_device_out,
	cpu_exec_device_in
};

static char* cpu_error_messages[] =
{
	"No error",
	"Invalid instruction",
	"Invalid operand"
};



cpu_t *cpu_create (computer_t *computer)
{
	cpu_t *cpu = (cpu_t*) malloc (sizeof(cpu_t));
	memset (cpu, 0, sizeof (cpu_t));

	cpu->state = CPU_STATE_PAUSED;
	cpu->error = CPU_ERROR_NO;
	cpu->computer = computer;

	return cpu;
}

void cpu_run (cpu_t *cpu)
{
	cpu->state = CPU_STATE_RUNNING;
	while (cpu->state == CPU_STATE_RUNNING)
	{
		cpu_exec_next_instruction (cpu);
		if (cpu->computer->memory->state == memory_error)
		{
			cpu_dump (cpu);
			return;
		}
	}
	cpu_dump (cpu);
}

static void cpu_exec_next_instruction (cpu_t *cpu)
{
	uint8_t instruction_code = memory_read8 (cpu->computer, cpu->ip);
	cpu->ip++;

	//printf("instruction: %d at %d\n", instruction_code, cpu->ip);

	if (instruction_code <= INSTRUCTIONS_COUNT)
		instructions[instruction_code] (cpu);
	else
		cpu_set_error (cpu, CPU_ERROR_INVALID_INSTRUCTION);
}

static void cpu_exec_halt (cpu_t *cpu)
{
	cpu->state = CPU_STATE_PAUSED;
	printf ("Cpu halted\n");
	while (cpu->state == CPU_STATE_PAUSED) { }
	printf ("Cpu halt ended\n");
}

static void cpu_exec_mov (cpu_t *cpu)
{
	operand_t* dest = cpu_get_next_operand (cpu);
	operand_t* src  = cpu_get_next_operand (cpu);
	cpu_write_operand (cpu, dest, cpu_read_operand (cpu, src));
}

static void cpu_exec_jmp(cpu_t *cpu)
{
	cpu->ip = cpu_read_operand (cpu, cpu_get_next_operand(cpu));
}

static void cpu_exec_add (cpu_t *cpu)
{
	operand_t *o1  = cpu_get_next_operand (cpu);
	operand_t *o2  = cpu_get_next_operand (cpu);
	uint32_t data1 = cpu_read_operand (cpu, o1);
	uint32_t data2 = cpu_read_operand (cpu, o2);
	cpu_write_operand (cpu, o1, data1 + data2);
}

static void cpu_exec_shutdown (cpu_t *cpu)
{
	cpu->state = CPU_STATE_SHUTDOWN;
}

static void cpu_exec_device_out (cpu_t *cpu)
{
	uint8_t device_id = (uint8_t) cpu_read_operand (cpu, cpu_get_next_operand (cpu));
	uint8_t port      = (uint8_t) cpu_read_operand (cpu, cpu_get_next_operand (cpu));
	device_t *device = device_list_id (cpu->computer->devices, device_id);
	if (device != NULL)			
		device->out (device, port, cpu->registers[0]);
	else
		printf ("ERROR unknown device: %d\n", device_id);
}

static void cpu_exec_device_in (cpu_t *cpu)
{
	uint8_t device_id = (uint8_t) cpu_read_operand (cpu, cpu_get_next_operand (cpu));
	uint8_t port      = (uint8_t) cpu_read_operand (cpu, cpu_get_next_operand (cpu));

	device_t *device = device_list_id (cpu->computer->devices, device_id);
	
	if (device != NULL)			
		cpu->registers[0] = device->in (device, port);
	else
		printf ("ERROR unknown device: %d\n", device_id);
}

static inline operand_t* cpu_get_next_operand (cpu_t *cpu)
{
	operand_t* operand = (operand_t*) memory_read32 (cpu->computer, cpu->ip);
	cpu->ip += 5;
	return operand;
}

static uint32_t cpu_read_operand (cpu_t *cpu, operand_t *operand)
{
	switch (operand->type)
	{
	case OPERAND_TYPE_REGISTER_VALUE:
		return cpu->registers[operand->value];
	case OPERAND_TYPE_REGISTER_POINTER:
	{
		uint32_t addr = cpu->registers[operand->value];
		return (uint32_t) memory_read32 (cpu->computer, addr);
	}
	case OPERAND_TYPE_VALUE:
		return operand->value;
	case OPERAND_TYPE_POINTER:
		return (uint32_t) memory_read32 (cpu->computer, operand->value);
	default:
		cpu_set_error (cpu, CPU_ERROR_INVALID_OPERAND);
	}

	return 0;
}

static void cpu_write_operand (cpu_t *cpu, operand_t *operand, uint32_t val)
{
	switch (operand->type)
	{
	case OPERAND_TYPE_REGISTER_VALUE:
		cpu->registers[operand->value] = val;
		break;
	case OPERAND_TYPE_REGISTER_POINTER:
		memory_write32 (cpu->computer, cpu->registers[operand->value], val);
		break;
	case OPERAND_TYPE_VALUE:
		memory_write32 (cpu->computer, operand->value, val);
		break;
	case OPERAND_TYPE_POINTER:
	{
		uint32_t addr = (uint32_t) memory_read32 (cpu->computer,
												  operand->value);
		memory_write32 (cpu->computer, addr , val);
		break;
	}
	default:
		cpu_set_error (cpu, CPU_ERROR_INVALID_OPERAND);
	}
}

static inline void cpu_set_error (cpu_t *cpu, cpu_error_t error)
{
	printf ("[ERROR] %s\n", cpu_error_messages[error]);
	cpu_dump (cpu);
	cpu->error = error;
	cpu->state = CPU_STATE_ERROR;
}

static void cpu_dump (cpu_t *cpu)
{
	printf ("==== Cpu dump ====\n");
	printf ("Registers:\n");
	for (int i = 0; i < REGS_COUNT; i++)
	{
		printf ("reg%d = %d   ", i, cpu->registers[i]);
	}

	printf ("ip = %d\n", cpu->ip);
}
