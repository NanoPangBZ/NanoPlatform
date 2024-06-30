#pragma once

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
    NANO_ERR = -8,          //错误
    NANO_UNKOWN_ERR = -9,   //未知错误
}nano_err_t;

nano_err_t nano_plantform_init(void);
uint32_t nano_sys_time_ms(void);

#ifdef __cplusplus
}
#endif  //__cplusplus
