#include "nano_net_ringbuf.h"
#include <stddef.h>
#include <string.h>

#include "../ll_depend/nano_net_ll_heap.h"
#define MALLOC(__size)  nano_net_ll_heap_malloc(__size)
#define FREE(__ptr)     nano_net_ll_heap_free(__ptr)

typedef struct nano_net_ringbuf_t {
    uint8_t* buffer;        // 环形缓冲区的实际数据存储
    uint32_t size;          // 缓冲区大小
    uint32_t head;          // 写指针
    uint32_t tail;          // 读指针
} nano_net_ringbuf_t;

nano_net_ringbuf_handle_t nano_net_ringbuf_create(uint32_t size) {
    if (size == 0) {
        return NULL;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)MALLOC(sizeof(nano_net_ringbuf_t));
    if (!ringbuf) {
        return NULL;
    }
    
    ringbuf->buffer = (uint8_t*)MALLOC(size);
    if (!ringbuf->buffer) {
        FREE(ringbuf);
        return NULL;
    }
    
    ringbuf->size = size;
    ringbuf->head = 0;
    ringbuf->tail = 0;
    
    return (nano_net_ringbuf_handle_t)ringbuf;
}

void nano_net_ringbuf_destroy(nano_net_ringbuf_handle_t handle) {
    if (!handle) {
        return;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    if (ringbuf->buffer) {
        FREE(ringbuf->buffer);
    }
    FREE(ringbuf);
}

int nano_net_ringbuf_write(nano_net_ringbuf_handle_t handle, const uint8_t* data, uint32_t len) {
    if (!handle || !data || len == 0) {
        return -1;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    uint16_t free_space = (ringbuf->tail + ringbuf->size - ringbuf->head - 1) % ringbuf->size;
    uint32_t write_len = (len > free_space) ? free_space : len;

    if (write_len == 0) {
        return 0;
    }

    // Handle wrap-around case
    if (ringbuf->head + write_len > ringbuf->size) {
        // Split into two parts
        uint32_t first_part = ringbuf->size - ringbuf->head;
        uint32_t second_part = write_len - first_part;
        
        memcpy(&ringbuf->buffer[ringbuf->head], data, first_part);
        memcpy(&ringbuf->buffer[0], data + first_part, second_part);
    } else {
        // Single copy
        memcpy(&ringbuf->buffer[ringbuf->head], data, write_len);
    }

    ringbuf->head = (ringbuf->head + write_len) % ringbuf->size;

    return write_len;
}

int nano_net_ringbuf_read(nano_net_ringbuf_handle_t handle, uint8_t* data, uint32_t len) {
    if (!handle || !data || len == 0) {
        return 0;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    uint32_t available_data = (ringbuf->head + ringbuf->size - ringbuf->tail) % ringbuf->size;
    uint32_t read_len = (len > available_data) ? available_data : len;

    if (read_len == 0) {
        return 0;
    }

    // Handle wrap-around case
    if (ringbuf->tail + read_len > ringbuf->size) {
        // Split into two parts
        uint32_t first_part = ringbuf->size - ringbuf->tail;
        uint32_t second_part = read_len - first_part;
        
        memcpy(data, &ringbuf->buffer[ringbuf->tail], first_part);
        memcpy(data + first_part, &ringbuf->buffer[0], second_part);
    } else {
        // Single copy
        memcpy(data, &ringbuf->buffer[ringbuf->tail], read_len);
    }

    ringbuf->tail = (ringbuf->tail + read_len) % ringbuf->size;

    return read_len;
}

uint8_t nano_net_ringbuf_is_empty(nano_net_ringbuf_handle_t handle) {
    if (!handle) {
        return 1;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    return ringbuf->head == ringbuf->tail ? 1 : 0;
}

uint8_t nano_net_ringbuf_is_full(nano_net_ringbuf_handle_t handle) {
    if (!handle) {
        return 0;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    return ((ringbuf->head + 1) % ringbuf->size) == ringbuf->tail ? 1 : 0;
}

uint32_t nano_net_ringbuf_get_size(nano_net_ringbuf_handle_t handle) {
    if (!handle) {
        return 0;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    return ringbuf->size;
}

void nano_net_ringbuf_clear(nano_net_ringbuf_handle_t handle) {
    if (!handle) {
        return;
    }
    
    nano_net_ringbuf_t* ringbuf = (nano_net_ringbuf_t*)handle;
    ringbuf->head = 0;
    ringbuf->tail = 0;
}


