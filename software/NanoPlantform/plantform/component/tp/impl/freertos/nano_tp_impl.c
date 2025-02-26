#include "nano_tp_cfg.h"

#ifdef NANO_TP_USE_FREERTOS

#include "nano_tp_impl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void* nano_tp_impl_malloc(uint32_t size)
{
    return pvPortMalloc( size );
}

void nano_tp_impl_free(void* ptr)
{
    vPortFree(ptr);
}

uint32_t nano_tp_impl_get_sys_time(void)
{
    return pdTICKS_TO_MS(xTaskGetTickCount());
}

void nano_tp_impl_thread_create(nano_tp_impl_thread_handle_t* thread_handle, void (*thread_func)(void*), void* args, nano_tp_thread_attr_t attr)
{
    size_t stack_size = 0;
    if( attr & NANO_TP_THREAD_ATTR_BIG_STACK_SIZE )
    {
        stack_size = 8 * 1024;
    }
    else if( attr & NANO_TP_THREAD_ATTR_LARG_STACK_SIZE )
    {
        stack_size = 12 * 1024;
    }
    else
    {
        stack_size = 4 * 1024;
    }

    UBaseType_t priority = 0;
    if( attr & NANO_TP_THREAD_ATTR_IMPORTANT )
    {
        priority = configMAX_PRIORITIES - 1;
    }
    else if( attr & NANO_TP_THREAD_ATTR_REALTIME )
    {
        priority = configMAX_PRIORITIES * 3 / 4;
    }
    else
    {
        priority = configMAX_PRIORITIES / 2;
    }

    TaskHandle_t rtos_handle;
    xTaskCreate( thread_func ,
                 "tp" ,
                 stack_size,
                 args,
                 priority,
                 &rtos_handle);

    *thread_handle = rtos_handle;
}

void nano_tp_impl_thread_destroy(nano_tp_impl_thread_handle_t thread_handle)
{
    TaskHandle_t rtos_handle = (TaskHandle_t)thread_handle;
    vTaskDelete( rtos_handle );
}

void nano_tp_impl_thread_sleep(uint32_t ms)
{
    vTaskDelay( pdMS_TO_TICKS(ms) );
}

void nano_tp_impl_lock_create(nano_tp_impl_lock_handle_t* lock_handle)
{
    SemaphoreHandle_t rtos_handle = xSemaphoreCreateMutex();
    *lock_handle = rtos_handle;
}

void nano_tp_impl_lock_destroy(nano_tp_impl_lock_handle_t lock_handle)
{
    vSemaphoreDelete( (SemaphoreHandle_t)lock_handle );
}

tp_err_t nano_tp_impl_lock_lock(nano_tp_impl_lock_handle_t lock_handle)
{
    SemaphoreHandle_t rtos_handle = (SemaphoreHandle_t)lock_handle;
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(rtos_handle, &xHigherPriorityTaskWoken);
        if(  xHigherPriorityTaskWoken == pdTRUE )
        {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
    else
    {
        xSemaphoreGive(rtos_handle);
    }
    return ERR_CODE_OK;
}

void nano_tp_impl_lock_unlock(nano_tp_impl_lock_handle_t lock_handle)
{
    SemaphoreHandle_t rtos_handle = (SemaphoreHandle_t)lock_handle;
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(rtos_handle, &xHigherPriorityTaskWoken);
        if(  xHigherPriorityTaskWoken == pdTRUE )
        {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
    else
    {
        xSemaphoreGive(rtos_handle);
    }
}

tp_err_t nano_tp_impl_lock_try_lock(nano_tp_impl_lock_handle_t lock_handle, uint32_t timeout_ms)
{
    SemaphoreHandle_t rtos_handle = (SemaphoreHandle_t)lock_handle;
    if( xPortIsInsideInterrupt() )
    {
        return ERR_CODE_FAIL;
    }

    if( xSemaphoreTake(rtos_handle, timeout_ms / portTICK_PERIOD_MS) == pdTRUE )
    {
        return ERR_CODE_OK;
    }
    else
    {
        return ERR_CODE_FAIL;
    }
}

#endif  //NANO_TP_USE_FREERTOS
