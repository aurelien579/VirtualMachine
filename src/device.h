#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

typedef struct device device_t;
typedef void	 	(*out_t) (device_t*, uint8_t, uint32_t);
typedef uint32_t	(*in_t)  (device_t*, uint8_t);

typedef struct computer computer_t;
typedef struct device
{
    uint32_t    id;
    computer_t *computer;
    out_t       out;
    in_t        in;
    uint32_t   *custom_ptr;
} device_t;

typedef struct device_list
{
    device_t **devices;
    uint32_t   size;
} device_list_t;

void            device_list_add     (device_list_t *list, device_t *device);
device_list_t  *device_list_create  ();
device_t       *device_list_id      (device_list_t *list, uint8_t id);

#endif
