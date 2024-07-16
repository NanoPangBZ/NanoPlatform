#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

typedef enum{
    NANO_OK = 0,            //成功
    NANO_NO_INIT = -1,      //未初始化
    NANO_BUSY = -2,         //忙
    NANO_TIMEOUT = -3,      //超时
    NANO_NO_MEM = -4,       //内存不足
    NANO_NO_IMPL = -5,      //未实现
    NANO_ILLEG_OBJ = -6,    //非法的对象
    NANO_ILLEG_PARAM = -7,  //非法的参数
    NANO_BUFF_FULL = -8,    //缓存区满
    NANO_NO_SUPPORT = -9,   //不支持的操作
    NANO_ERR = -9,          //错误
    NANO_UNKOWN_ERR = -10,  //未知错误
}nano_err_e;
typedef int32_t nano_err_t;

//io操作属性
typedef enum{
    NANO_BIO,       //阻塞式IO - 阻塞cpu，调用read/write时不拷贝读/写数据，等待底层读/写完成后才返回
    NANO_NIO,       //非阻塞式IO - 不阻塞cpu，调用read/write时读/写内容拷贝到用户/底层缓冲区后立刻返回，底层读/写结果不通知用户
    NANO_AIO,       //异步IO - 不阻塞cpu，调用read/write时不拷贝读/写数据到用户/底层缓冲区，调用后立刻返回，读/写完成后通过回调函数通知用户
    NANO_IO_MAP,    //内存映射 - 不阻塞cpu，通过对指定的read/write内存段进行操作完成读写，调用read/write后的行为不确定，由实现模块自由决定
}nano_io_mode_e;
typedef uint8_t nano_io_mode_t;

//io操作类型
typedef enum{
    NANO_IO_READ = 0x01 << 0,
    NANO_IO_WRITE = 0x01 << 1,
    NANO_IO_READ_WRITE = NANO_IO_READ | NANO_IO_WRITE,
}nano_io_opt_type_e;
typedef uint8_t nano_io_opt_type_t;

//CPU的运行模式
typedef enum{
    NANO_CPU_USER_MODE,     //用户模式
    NANO_CPU_ISR_MODE,      //管理员模式
}nano_cpu_mode_e;
typedef uint8_t nano_cpu_mode_t;

/**
 * @brief 异步io回调
 * @param ctx 用户上下文
 * @param opt_ret io操作结果
 * @param cpu_mode 当前的cpu运行状态
*/
typedef void (*nano_aio_callback_t)(void* ctx,nano_err_t opt_ret,nano_cpu_mode_t cpu_mode);

nano_err_t nano_plantform_init(void);
uint32_t nano_sys_time_ms(void);

#ifdef __cplusplus
}
#endif  //__cplusplus
