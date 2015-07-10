#include "device.h"

#include <stdlib.h>

device_list_t *device_list_create ()
{
    device_list_t *list = malloc (sizeof (device_list_t));
    return list;
}

void device_list_add (device_list_t *list, device_t *device)
{
    list->size++;
    list->devices = realloc (list->devices, sizeof (device_t*) * list->size);
    list->devices[list->size - 1] = device;
}

device_t *device_list_id (device_list_t *list, uint8_t id)
{
    for (int i = 0; i < list->size; i++)
        if (list->devices[i]->id == id)
            return list->devices[i];

    return NULL;
}
