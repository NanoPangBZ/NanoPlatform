#pragma once

#include <stdint.h>

//轮询任务句柄
typedef struct nano_polling_task_t* nano_polling_task_handle_t;

//轮询任务属性
typedef enum nano_polling_task_attr_e{
    NANO_POLLING_TASK_ATTR_NONE = 0x00,
    NANO_POLLING_TASK_ATTR_REALTIME = 0x01 << 0,    //实时任务
    NANO_POLLING_TASK_ATTR_DEEP_STACK = 0x01 << 1,  //深栈任务

    //其他扩展属性
    //...

    NANO_POLLING_TASK_ATTR_DEFAULT = NANO_POLLING_TASK_ATTR_NONE,
}nano_polling_task_attr_e;
typedef uint32_t nano_polling_task_attr_t;

//轮询任务描述
typedef struct nano_polling_task_desc_t{
    const char* name;
    void* user_ctx;
    void (*polling_func)(void* user_ctx);
    uint32_t freq_hz;                       //轮询频率(Hz)，0表示不定时轮询
    uint8_t start_before_create;            //创建前是否启动
    nano_polling_task_attr_t attr;
}nano_polling_task_desc_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建轮询任务
 * @param desc 轮询任务描述
 * @return 轮询任务句柄
*/
nano_polling_task_handle_t nano_polling_task_create( nano_polling_task_desc_t* desc );

/**
 * @brief 启动轮询任务
 * @param handle 轮询任务句柄
 * @return 0成功，非0失败
*/
void nano_polling_task_destroy( nano_polling_task_handle_t handle );

/**
 * @brief 启动轮询任务
 * @param handle 轮询任务句柄
*/
void nano_polling_task_start( nano_polling_task_handle_t handle );

/**
 * @brief 停止轮询任务
 * @param handle 轮询任务句柄
 * @note 当前任务正在执行时，停止操作会等待当前任务执行完毕后再停止
*/
void nano_polling_task_stop( nano_polling_task_handle_t handle );

/**
 * @brief 设置轮询任务的频率
 * @param handle 轮询任务句柄
 * @param freq_hz 轮询频率(Hz)
*/
void nano_polling_task_set_freq( nano_polling_task_handle_t handle, uint32_t freq_hz );

#ifdef __cplusplus
}
#endif

