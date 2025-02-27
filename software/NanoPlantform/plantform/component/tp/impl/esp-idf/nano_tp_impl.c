#include "nano_tp_cfg.h"

#ifdef NANO_TP_USE_ESP_IDF

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
    UBaseType_t priority = 0;
    nano_tp_thread_attr_t prio_attr = NANO_TP_THREAD_ATTR_GET_PRIO_ATTR( attr );
    nano_tp_thread_attr_t stack_size_attr = NANO_TO_THREAD_ATTR_GET_STACK_ATTR( attr );

    switch( stack_size_attr )
    {
        case NANO_TP_THREAD_ATTR_DEFAULT_STACK_SIZE:
            stack_size = 6 * 1024;
            break;
        case NANO_TP_THREAD_ATTR_MIN_STACK_SIZE:
            stack_size = 4 * 1024;
            break;
        case NANO_TP_THREAD_ATTR_MID_STACK_SIZE:
            stack_size = 8 * 1024;
            break;
        case NANO_TP_THREAD_ATTR_BIG_STACK_SIZE:
            stack_size = 12 * 1024;
            break;
        default:
            stack_size = 6 * 1024;
    }

    switch( prio_attr )
    {
        case NANO_TP_THREAD_ATTR_DEFAULT_PRIO:
            priority = configMAX_PRIORITIES  / 2;
            break;
        case NANO_TP_THREAD_ATTR_LOW_PRIO:
            priority = configMAX_PRIORITIES / 4;
            break;
        case NANO_TP_THREAD_ATTR_MID_PRIO:
            priority = configMAX_PRIORITIES / 2 + 1;
            break;
        case NANO_TP_THREAD_ATTR_HIGHT_PRIO:
            priority = configMAX_PRIORITIES * 4 / 5;
            break;
        default:
            priority = configMAX_PRIORITIES / 2;
            break;
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
