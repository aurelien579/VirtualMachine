#include "graphic-card.h"

#include <stdlib.h>
#include <stdio.h>

#define GC_ID 				1
#define OUT_HANDLERS_COUNT	1

static enum graphic_mode
{
	graphic_mode_default
};

typedef void (*out_handler_t) (device_t *gc, uint32_t);

static void		  gc_out (device_t *gc, uint8_t port, uint32_t data);
static uint32_t   gc_in  (device_t *gc, uint8_t port);

static void out_draw (device_t *gc, uint32_t data);

static out_handler_t out_handlers[OUT_HANDLERS_COUNT] =
{
	out_draw
};

void graphic_card_register (computer_t *computer)
{
    device_t *gc = (device_t*) malloc (sizeof (device_t));

    gc->computer    = computer;
    gc->id          = GC_ID;
    gc->out         = gc_out;
    gc->in          = gc_in;

    gc_t *custom = (gc_t*) malloc (sizeof (gc_t));
    custom->graphic_mode = graphic_mode_default;

    gc->custom_ptr  = custom;

    device_list_add (computer->devices, gc);
}

static void gc_out (device_t *gc, uint8_t port, uint32_t data)
{
    printf ("gc_out\n");
    
    if (port < OUT_HANDLERS_COUNT)
    	out_handlers[port] (gc, data);
    else
    	printf ("[ERROR] Unknown port: %d for device: %d\n", port, gc->id);
}

static uint32_t gc_in (device_t *gc, uint8_t port)
{
	return 74;
}

static void out_draw (device_t *gc, uint32_t data)
{
	switch ( ((gc_t*)gc->custom_ptr)->graphic_mode )
	{
	case graphic_mode_default:
		printf ("%c", (char) data);
		break;
	}
}
