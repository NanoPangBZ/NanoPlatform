#include "nano_io_device.h"
#include "nano_heap.h"
#include <string.h>

typedef struct{
    const char* name;
    void* desc;
    void* instance;
    nano_io_device_opt_t opt;
}nano_io_device_t;

struct nano_io_device_node_t{
    nano_io_device_t* device;
    struct nano_io_device_node_t* next_node;
};

static struct nano_io_device_node_t* nano_io_device_head_node = NULL;

nano_err_t nano_register_io_device(const char* name,void* desc,nano_io_device_opt_t* opt)
{
    nano_io_device_t* device = (nano_io_device_t*)nano_heap_malloc(sizeof(nano_io_device_t));

    if( device == NULL )
    {
        return NANO_NO_MEM;
    }

    device->desc = desc;
    device->instance = NULL;
    device->name = name;
    device->opt = *opt;

    struct nano_io_device_node_t* node = (struct nano_io_device_node_t*)nano_heap_malloc(sizeof(nano_io_device_head_node));

    if( node == NULL )
    {
        nano_heap_free(device);
        return NANO_NO_MEM;
    }

    node->device = device;

    if( nano_io_device_head_node == NULL )
    {
        nano_io_device_head_node = node;
    }
    else
    {
        struct nano_io_device_node_t* curr_node = nano_io_device_head_node;
        while( curr_node->next_node != NULL )
        {
            curr_node = curr_node->next_node;
        }
        curr_node->next_node = node;
    }

    return NANO_OK;
}

nano_err_t nano_remove_io_device(const char* name)
{
    return NANO_NO_IMPL;
}

nano_io_dev_handle_t nano_io_device_open(const char* name,nano_io_opt_type_t opt_type,nano_io_mode_t io_mode)
{
    if( nano_io_device_head_node == NULL )
        return NULL;

    struct nano_io_device_node_t* curr_node = nano_io_device_head_node;
    do{
        nano_io_device_t* device = curr_node->device;
        if( !strcmp( device->name , name ) )
        {
            device->instance = device->opt.open( device->desc , opt_type , io_mode );
            return (void*)curr_node->device;
        }
        curr_node = curr_node->next_node == NULL ? curr_node : curr_node->next_node;
    }while( curr_node->next_node != NULL );

    return NULL;
}

nano_err_t nano_io_device_close(nano_io_dev_handle_t handle)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt.close ) return NANO_NO_IMPL;
    
    return device->opt.close( device->instance );
}

nano_err_t nano_io_device_write(nano_io_dev_handle_t handle,uint8_t* data,uint16_t len,uint16_t* writed_len)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt.write ) return NANO_NO_IMPL;
    
    return device->opt.write( device->instance , data , len , writed_len );
}

nano_err_t nano_io_device_read(nano_io_dev_handle_t handle,uint8_t* data,uint16_t len,uint16_t* read_len)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt.read ) return NANO_NO_IMPL;
    
    return device->opt.write( device->instance , data , len , read_len );
}

nano_err_t nano_io_device_flush(nano_io_dev_handle_t handle)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt.flush ) return NANO_NO_IMPL;
    
    return device->opt.flush( device->instance );
}
