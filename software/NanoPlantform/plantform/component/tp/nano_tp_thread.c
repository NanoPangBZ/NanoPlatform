#include "nano_tp_private.h"

#define TAKE_LOCK(lock) nano_tp_impl_lock_lock(lock)
#define GIVE_LOCK(lock) nano_tp_impl_lock_unlock(lock)

void nano_tp_thread_func(void* args)
{
    nano_tp_thread_handle_t thread = (nano_tp_thread_handle_t)args;

    while(1)
    {
        //遍历线程池
        FOREACH_NODE_IN_LIST(thread->bind_pool_list,pool_node)
        {
            nano_tp_pool_handle_t pool = (nano_tp_pool_handle_t)pool_node->obj;

            //判断线程池是否暂停
            if( pool->status.is_pause )
            {
                continue;
            }

            //遍历任务
            FOREACH_NODE_IN_LIST(pool->task_list,task_node)
            {
                nano_tp_task_handle_t task = (nano_tp_task_handle_t)task_node->obj;

                //当前任务正在活跃(被其他线程执行中)
                if( task->status.is_active )
                {
                    continue;
                }

                TAKE_LOCK(pool->lock);

                //判断是否到了执行时间
                if( nano_tp_impl_get_sys_time() - task->last_run_time > task->desc.cycle_ms &&
                    !task->status.is_active )
                {
                    task->status.is_active = 1;
                }
                else
                {
                    GIVE_LOCK(pool->lock);
                    continue;
                }

                GIVE_LOCK(pool->lock);

                //执行任务
                task->desc.task_func( task->desc.user_ctx );
                task->last_run_time = nano_tp_impl_get_sys_time();
                task->status.is_active = 0;
            }
        }

        uint32_t sleep_time = 0;
        if( thread->desc.thread_attr & NANO_TP_THREAD_ATTR_REALTIME )
        {
            //500Hz轮询
            sleep_time = 2;
        }
        else if( thread->desc.thread_attr & NANO_TP_THREAD_ATTR_LONG_CYCLE )
        {
            //1Hz轮询
            sleep_time = 1000;
        }
        else if( thread->desc.thread_attr & NANO_TP_THREAD_ATTR_LOW_ACCURACY )
        {
            //20Hz轮询
            sleep_time = 50;
        }
        else
        {
            //100Hz轮询
            sleep_time = 10;
        }
        nano_tp_impl_thread_sleep(sleep_time);
    }
}

