#include "cpu.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSTRUCTIONS_COUNT	7

typedef void(*instruction_t)(cpu_t*);

static void              exec_next_instruction (cpu_t *cpu);
static void    			 exec_halt			   (cpu_t *cpu);
static void              exec_mov              (cpu_t *cpu);
static void              exec_jmp              (cpu_t *cpu);
static void				 exec_add			   (cpu_t *cpu);
static void   			 exec_shutdown		   (cpu_t *cpu);
static void    			 exec_device_outb      (cpu_t *cpu);
static void    			 exec_device_inb       (cpu_t *cpu);

static inline void       cpu_set_error         (cpu_t *cpu,
												cpu_error_t cpu_error);

static inline operand_t* get_next_operand      (cpu_t *cpu);
static uint32_t          read_operand          (cpu_t *cpu, operand_t *operand);
static void              write_operand         (cpu_t *cpu, operand_t *operand,
												uint32_t val);

static void              cpu_dump              (cpu_t *cpu);

static instruction_t instructions[INSTRUCTIONS_COUNT] =
{
	exec_halt,
	exec_mov,
	exec_jmp,
	exec_add,
	exec_shutdown,
	exec_device_outb,
	exec_device_inb
};

static char* error_messages[] =
{
	"No error",
	"Invalid instruction",
	"Invalid operand"
};

cpu_t *cpu_create (computer_t *computer)
{
	cpu_t *cpu = (cpu_t*) malloc (sizeof(cpu_t));
	memset (cpu, 0, sizeof (cpu_t));

	cpu->state = paused;
	cpu->error = no_error;
	cpu->computer = computer;

	return cpu;
}

void cpu_run (cpu_t *cpu)
{
	cpu->state = running;
	while (cpu->state == running)
	{
		exec_next_instruction (cpu);
		if (cpu->computer->memory->state == memory_error)
		{
			cpu_dump (cpu);
			return;
		}
	}
}

static void exec_next_instruction (cpu_t *cpu)
{
	uint8_t instruction_code = memory_read8 (cpu->computer, cpu->ip);
	cpu->ip++;

	//printf("instruction: %d at %d\n", instruction_code, cpu->ip);

	if (instruction_code <= INSTRUCTIONS_COUNT)
		instructions[instruction_code] (cpu);
	else
		cpu_set_error (cpu, invalid_instruction);
}

static void exec_halt (cpu_t *cpu)
{
	cpu->state = paused;
	printf ("Cpu halted\n");
	while (cpu->state == paused) { }
	printf ("Cpu halt ended\n");
}

static void exec_mov (cpu_t *cpu)
{
	operand_t* dest = get_next_operand (cpu);
	operand_t* src  = get_next_operand (cpu);
	write_operand (cpu, dest, read_operand (cpu, src));
}

static void exec_jmp(cpu_t *cpu)
{
	cpu->ip = read_operand(cpu, get_next_operand(cpu));
}

static void exec_add (cpu_t *cpu)
{
	operand_t *o1  = get_next_operand (cpu);
	operand_t *o2  = get_next_operand (cpu);
	uint32_t data1 = read_operand (cpu, o1);
	uint32_t data2 = read_operand (cpu, o2);
	write_operand (cpu, o1, data1 + data2);
}

static void exec_shutdown (cpu_t *cpu)
{
	cpu->state = shutdown;
}

static void exec_device_outb (cpu_t *cpu)
{
	uint8_t port      = (uint8_t) get_next_operand (cpu);
	uint8_t device_id = (uint8_t) get_next_operand (cpu);

	device_t *device = device_list_id (cpu->computer->devices, device_id);
	device->outb (device, port);
}

static void exec_device_inb (cpu_t *cpu)
{

}

static inline operand_t* get_next_operand(cpu_t *cpu)
{
	operand_t* operand = (operand_t*) memory_read32 (cpu->computer, cpu->ip);
	cpu->ip += 5;
	return operand;
}

static uint32_t read_operand (cpu_t *cpu, operand_t *operand)
{
	switch (operand->type)
	{
	case register_value:
		return cpu->registers[operand->value];
	case register_pointer:
	{
		uint32_t addr = cpu->registers[operand->value];
		return (uint32_t) memory_read32 (cpu->computer, addr);
	}
	case value:
		return operand->value;
	case pointer:
		return (uint32_t) memory_read32 (cpu->computer, operand->value);
	default:
		cpu_set_error (cpu, invalid_operand);
	}

	return 0;
}

static void write_operand (cpu_t *cpu, operand_t *operand, uint32_t val)
{
	switch (operand->type)
	{
	case register_value:
		cpu->registers[operand->value] = val;
		break;
	case register_pointer:
		memory_write32 (cpu->computer, cpu->registers[operand->value], val);
		break;
	case value:
		memory_write32 (cpu->computer, operand->value, val);
		break;
	case pointer:
	{
		uint32_t addr = (uint32_t) memory_read32 (cpu->computer,
												  operand->value);
		memory_write32 (cpu->computer, addr , val);
		break;
	}
	default:
		cpu_set_error (cpu, invalid_operand);
	}
}

static inline void cpu_set_error (cpu_t *cpu, cpu_error_t cpu_error)
{
	printf ("[ERROR] %s\n", error_messages[cpu_error]);
	cpu_dump (cpu);
	cpu->error = cpu_error;
	cpu->state = error;
}

static void cpu_dump (cpu_t *cpu)
{
	printf("==== Cpu dump ====\n");
	printf("Registers:\n");
	for (int i = 0; i < REGS_COUNT; i++)
	{
		printf("reg%d = %d   ", i, cpu->registers[i]);
	}

	printf("ip = %d\n", cpu->ip);
}
