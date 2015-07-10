#include "computer.h"
#include "cpu.h"
#include "graphic-card.h"

#include <stdlib.h>

computer_t *computer_create ()
{
	computer_t *computer = (computer_t*) malloc (sizeof (computer_t));

	computer->cpu     = cpu_create    		(computer);
	computer->memory  = memory_create 		(computer);
	computer->devices = device_list_create 	();

	graphic_card_register (computer);

	return computer;
}

void computer_start (computer_t *computer)
{
	cpu_run (computer->cpu);
}
