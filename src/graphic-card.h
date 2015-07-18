#ifndef GRAPHIC_CARD_H
#define GRAPHIC_CARD_H

#include "device.h"
#include "computer.h"

#include <stdint.h>

typedef struct
{
	uint8_t graphic_mode;
} gc_t;

void graphic_card_register (computer_t *computer);

#endif
