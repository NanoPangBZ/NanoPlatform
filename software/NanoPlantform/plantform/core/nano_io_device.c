#include "nano_io_device.h"
#include "nano_heap.h"
#include <string.h>

typedef struct{
    const char* name;
    void* desc;
    void* instance;
    const nano_io_device_opt_t* opt;
}nano_io_device_t;

struct nano_io_device_node_t{
    nano_io_device_t* device;
    struct nano_io_device_node_t* next_node;
};

static struct nano_io_device_node_t* nano_io_device_head_node = NULL;

/**
 * @brief 注册一个io设备
 * @param name 名字
 * @param desc 设备初始化描述指针
 * @param opt 设备操作函数
 * @return NANO_OK:成功 其他:失败
 * @note 需要用户自行维护desc和opt的内存。
*/
nano_err_t nano_register_io_device(const char* name,const void* desc,const nano_io_device_opt_t* opt)
{
    nano_io_device_t* device = (nano_io_device_t*)nano_heap_malloc(sizeof(nano_io_device_t));

    if( device == NULL )
    {
        return NANO_NO_MEM;
    }

    device->desc = desc;
    device->instance = NULL;
    device->name = name;
    device->opt = opt;

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

/**
 * @brief 移除一个io设备
 * @param name 名称
 * @return NANO_OK:成功 其他:失败
 * @note 未实现
*/
nano_err_t nano_remove_io_device(const char* name)
{
    return NANO_NO_IMPL;
}

/**
 * @brief 打开一个io设备
 * @param name 设备名 或 设备路径
 * @param opt_type io操作类型，见nano_io_opt_type_e枚举
 * @param io_mode io操作模式，见nano_io_mode_e枚举
 * @param handle handle指针，设备句柄
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_io_device_open(const char* name,nano_io_opt_type_t opt_type,nano_io_mode_t io_mode,nano_io_dev_handle_t* handle)
{
    if( nano_io_device_head_node == NULL )
        return NANO_NO_INSTANCE;

    struct nano_io_device_node_t* curr_node = nano_io_device_head_node;
    do{
        nano_io_device_t* device = curr_node->device;
        if( !strcmp( device->name , name ) )
        {
            *handle = device;
            return device->opt->open( device->desc , opt_type , io_mode , &device->instance );
        }
        curr_node = curr_node->next_node == NULL ? curr_node : curr_node->next_node;
    }while( curr_node->next_node != NULL );

    return NANO_NO_INSTANCE;
}

/**
 * @brief 关闭一个io设备
 * @param handle 设备句柄
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_io_device_close(nano_io_dev_handle_t handle)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt->close ) return NANO_NO_IMPL;
    
    return device->opt->close( device->instance );
}

/**
 * @brief 向一个io设备写入数据
 * @param handle 设备句柄
 * @param data 数据指针
 * @param len 数据长度
 * @param writed_len 写入成功的长度
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_io_device_write(nano_io_dev_handle_t handle,uint8_t* data,uint16_t len,uint16_t* writed_len)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt->write ) return NANO_NO_IMPL;
    
    return device->opt->write( device->instance , data , len , writed_len );
}

/**
 * @brief 从一个io设备中读取数据
 * @param handle 设备句柄
 * @param data 数据缓存
 * @param len 要读取的大小
 * @param read_len 读取到的大小
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_io_device_read(nano_io_dev_handle_t handle,uint8_t* data,uint16_t len,uint16_t* read_len)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt->read ) return NANO_NO_IMPL;
    
    return device->opt->write( device->instance , data , len , read_len );
}

/**
 * @brief 立刻清空io设备的缓存区数据
 * @param handle io设备句柄
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_io_device_flush(nano_io_dev_handle_t handle)
{
    nano_io_device_t* device = (nano_io_device_t*)handle;

    if( !device->opt->flush ) return NANO_NO_IMPL;
    
    return device->opt->flush( device->instance );
}
