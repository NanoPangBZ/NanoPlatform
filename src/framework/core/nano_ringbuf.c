#include "../nano_ringbuf.h"
#include "../nano_heap.h"
#include <stddef.h>
#include <string.h>

#define MALLOC(_size) nano_heap_malloc((_size), NANO_HEAP_ATTR_DEFAULT)
#define FREE(_ptr)    nano_heap_free((_ptr))

typedef struct nano_ringbuf_t {
    uint8_t* buffer;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    uint32_t used;
} nano_ringbuf_t;

nano_ringbuf_handle_t nano_ringbuf_create(uint32_t size)
{
    if (size == 0) {
        return NULL;
    }

    nano_ringbuf_t* ringbuf = (nano_ringbuf_t*)MALLOC(sizeof(nano_ringbuf_t));
    if (ringbuf == NULL) {
        return NULL;
    }

    ringbuf->buffer = (uint8_t*)MALLOC(size);
    if (ringbuf->buffer == NULL) {
        FREE(ringbuf);
        return NULL;
    }

    ringbuf->size = size;
    ringbuf->head = 0;
    ringbuf->tail = 0;
    ringbuf->used = 0;
    return ringbuf;
}

void nano_ringbuf_destroy(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return;
    }

    if (ringbuf->buffer != NULL) {
        FREE(ringbuf->buffer);
    }
    FREE(ringbuf);
}

int nano_ringbuf_write(nano_ringbuf_handle_t ringbuf, const uint8_t* data, uint32_t size)
{
    if (ringbuf == NULL || data == NULL || size == 0) {
        return -1;
    }

    uint32_t free_size = ringbuf->size - ringbuf->used;
    uint32_t write_size = (size > free_size) ? free_size : size;
    if (write_size == 0) {
        return 0;
    }

    uint32_t first_size = ringbuf->size - ringbuf->head;
    if (first_size > write_size) {
        first_size = write_size;
    }
    memcpy(ringbuf->buffer + ringbuf->head, data, first_size);

    uint32_t second_size = write_size - first_size;
    if (second_size > 0) {
        memcpy(ringbuf->buffer, data + first_size, second_size);
    }

    ringbuf->head = (ringbuf->head + write_size) % ringbuf->size;
    ringbuf->used += write_size;
    return (int)write_size;
}

int nano_ringbuf_read(nano_ringbuf_handle_t ringbuf, uint8_t* data, uint32_t size)
{
    if (ringbuf == NULL || data == NULL || size == 0) {
        return -1;
    }

    uint32_t read_size = (size > ringbuf->used) ? ringbuf->used : size;
    if (read_size == 0) {
        return 0;
    }

    uint32_t first_size = ringbuf->size - ringbuf->tail;
    if (first_size > read_size) {
        first_size = read_size;
    }
    memcpy(data, ringbuf->buffer + ringbuf->tail, first_size);

    uint32_t second_size = read_size - first_size;
    if (second_size > 0) {
        memcpy(data + first_size, ringbuf->buffer, second_size);
    }

    ringbuf->tail = (ringbuf->tail + read_size) % ringbuf->size;
    ringbuf->used -= read_size;
    return (int)read_size;
}

uint8_t nano_ringbuf_is_empty(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return 1;
    }
    return (ringbuf->used == 0) ? 1 : 0;
}

uint8_t nano_ringbuf_is_full(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return 0;
    }
    return (ringbuf->used == ringbuf->size) ? 1 : 0;
}

uint32_t nano_ringbuf_get_size(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return 0;
    }
    return ringbuf->size;
}

uint32_t nano_ringbuf_get_used_size(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return 0;
    }
    return ringbuf->used;
}

uint32_t nano_ringbuf_get_free_size(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return 0;
    }
    return ringbuf->size - ringbuf->used;
}

void nano_ringbuf_clear(nano_ringbuf_handle_t ringbuf)
{
    if (ringbuf == NULL) {
        return;
    }
    ringbuf->head = 0;
    ringbuf->tail = 0;
    ringbuf->used = 0;
}
