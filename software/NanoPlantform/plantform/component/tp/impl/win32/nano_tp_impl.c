#include "nano_tp_cfg.h"

#ifdef NANO_TP_USE_WIN32_IMPL

#include "nano_tp_impl.h"
#include <Windows.h>

void* nano_tp_impl_malloc(uint32_t size)
{
    return malloc(size);
}

void nano_tp_impl_free(void* ptr)
{
    free(ptr);
}

uint32_t nano_tp_impl_get_sys_time(void)
{
    return GetTickCount();
}

typedef struct{
    nano_tp_thread_attr_t attr;
    void (*thread_func)(void*);
    void* args;
}win_thread_desc_t;

typedef struct{
    win_thread_desc_t desc;
    HANDLE thread_handle;
}win_thread_ctx_t;

static DWORD WINAPI win32_thread_func(LPVOID lpParam)
{
    win_thread_ctx_t* ctx = (win_thread_ctx_t*)lpParam;
    ctx->desc.thread_func(ctx->desc.args);
    return 0;
}

void nano_tp_impl_thread_create(nano_tp_impl_thread_handle_t* thread_handle, void (*thread_func)(void*), void* args, nano_tp_thread_attr_t attr)
{
    win_thread_ctx_t* ctx = (win_thread_ctx_t*)malloc(sizeof(win_thread_ctx_t));

    if(ctx == NULL)
    {
        *thread_handle = NULL;
        return;
    }

    ctx->desc.attr = attr;
    ctx->desc.thread_func = thread_func;
	ctx->desc.args = args;
    ctx->thread_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)win32_thread_func,ctx,0,NULL);

    if(ctx->thread_handle == NULL)
    {
        free(ctx);
        *thread_handle = NULL;
        return;
    }

    *thread_handle = ctx;
}

void nano_tp_impl_thread_destroy(nano_tp_impl_thread_handle_t thread_handle)
{
    win_thread_ctx_t* ctx = (win_thread_ctx_t*)thread_handle;
    WaitForSingleObject(ctx->thread_handle,INFINITE);
    CloseHandle(ctx->thread_handle);
    free(ctx);
}

void nano_tp_impl_thread_sleep(uint32_t ms)
{
    Sleep(ms);
}

void nano_tp_impl_lock_create(nano_tp_impl_lock_handle_t* lock_handle)
{
    *lock_handle = CreateMutex(NULL,FALSE,NULL);
}

void nano_tp_impl_lock_destroy(nano_tp_impl_lock_handle_t lock_handle)
{
    CloseHandle(lock_handle);
}

tp_err_t nano_tp_impl_lock_lock(nano_tp_impl_lock_handle_t lock_handle)
{
    DWORD ret = WaitForSingleObject(lock_handle,INFINITE);
    if(ret == WAIT_OBJECT_0)
    {
        return ERR_CODE_OK;
    }
    else
    {
        return ERR_CODE_FAIL;
    }
}

void nano_tp_impl_lock_unlock(nano_tp_impl_lock_handle_t lock_handle)
{
    ReleaseMutex(lock_handle);
}

tp_err_t nano_tp_impl_lock_try_lock(nano_tp_impl_lock_handle_t lock_handle, uint32_t timeout_ms)
{
    DWORD ret = WaitForSingleObject(lock_handle,timeout_ms);
    if(ret == WAIT_OBJECT_0)
    {
        return ERR_CODE_OK;
    }
    else if(ret == WAIT_TIMEOUT)
    {
        return ERR_CODE_FAIL;
    }
    else
    {
        return ERR_CODE_FAIL;
    }
}

#endif  // NANO_TP_USE_WIN32_IMPL