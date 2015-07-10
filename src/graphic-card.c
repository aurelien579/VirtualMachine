#include "graphic-card.h"

#include <stdlib.h>
#include <stdio.h>

#define GC_ID 1

static uint8_t  gc_outb (device_t *gc, uint8_t port);
static void     gc_inb  (device_t *gc, uint8_t port, uint8_t data);

void graphic_card_register (computer_t *computer)
{
    device_t *gc = (device_t*) malloc (sizeof (device_t));

    gc->computer    = computer;
    gc->id          = GC_ID;
    gc->outb        = gc_outb;
    gc->inb         = gc_inb;

    device_list_add (computer->devices, gc);
}

static uint8_t gc_outb (device_t *gc, uint8_t port)
{
    printf("gc_outb\n");
    return 45;
}

static void gc_inb (device_t *gc, uint8_t port, uint8_t data)
{

}
