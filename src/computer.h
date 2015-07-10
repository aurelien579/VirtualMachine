#ifndef COMPUTER_H
#define COMPUTER_H

#include <stdint.h>

#include "cpu.h"
#include "memory.h"
#include "device.h"

typedef struct cpu cpu_t;
typedef struct memory memory_t;

typedef struct computer
{
	cpu_t         *cpu;
	memory_t      *memory;
	device_list_t *devices;
} computer_t;

computer_t *computer_create	();
void		computer_start	(computer_t *computer);

#endif
