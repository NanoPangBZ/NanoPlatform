#pragma once

#include <stdint.h>

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

typedef enum{
    SYS_OK = 0,
    SYS_NO_INIT = -1,
    SYS_BUSY = -2,
    SYS_TIMEOUT = -3,
    SYS_NO_MEM = -4,
    SYS_ERR = -5,
    SYS_UNKOWN_ERR = -6,
}system_err_t;

typedef enum{
    ELF_BOOTLOADER,
    ELF_APPLICATION,
    ELF_FACTORY,
    ELF_TEST,
}elf_type_t;

typedef struct{
    uint8_t index;
    char name[32];
    char desc_str[64];
    elf_type_t  type;
    uint32_t    version;        //版本号
    uint64_t    magic;          //魔术字
    uint8_t     md5_value[16];  //md5校验

    uint32_t addr;              //固件地址
    size_t size;                //固件大小
    uint32_t nvic_offset;       //中断向量表相对固件地址的偏移
}elf_info_t;

typedef enum{
    DRIVER_THREAD,
    SYSTEM_THREAD,
    FUNCTION_THREAD,
}thread_type_t;

typedef struct{
    char name[32];
    thread_type_t   type;
    uint32_t        stack_size;

    void* ctx;
    system_err_t (*thread)(void* ctx);
    system_err_t (*thread_befor_kill_cb)(void* ctx);
}thread_desc_t;

typedef struct{
    char name[32];
    thread_type_t   type;
    uint32_t        stack_size;
    uint32_t        cycle_ms;
}polling_thread_pool_desc_t;

typedef struct{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week;
    uint8_t hours;
    uint8_t minute;
    uint8_t second;
}rtc_time_t;

struct thread_t;
typedef struct thread_t* thread_handle_t;

struct polling_thread_pool_t;
typedef struct polling_thread_pool_t* thread_pool_handle_t;

system_err_t system_init(void);
system_err_t system_reset(void);

uint32_t get_system_time(void);
system_err_t get_system_rtc_time(rtc_time_t* rtc_time);

uint8_t get_elf_num(void);
system_err_t get_running_elf_info(elf_info_t* info);
system_err_t get_elf_info_by_index(elf_info_t* info);
system_err_t get_elf_info_by_name(char* name,elf_info_t* info);
system_err_t run_elf(elf_info_t* info,uint32_t param);

system_err_t create_thread(thread_handle_t* handle,thread_desc_t* desc);
system_err_t kill_thread(thread_handle_t handle);
thread_handle_t find_thread(char* name);
system_err_t suspend_thread(thread_handle_t handle);
system_err_t resume_thread(thread_handle_t handle);
thread_handle_t get_now_thread_handle(thread_handle_t handle);

system_err_t create_polling_thread_pool();
system_err_t add_task_to_polling_thread_pool();
system_err_t remove_task_form_polling_thread_pool();

#ifdef __cplusplus
}
#endif  //__cplusplus
