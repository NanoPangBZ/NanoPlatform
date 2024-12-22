#include "nano_tp_private.h"

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

                if( task->status.is_active )
                {
                    continue;
                }

                //判断是否到了执行时间
                if( nano_tp_impl_get_sys_time() - task->last_run_time > task->desc.cycle_ms )
                {
                    task->status.is_active = 1;
                }


                //执行任务
                if( task->status.is_active )
                {
                    task->desc.task_func( task->desc.user_ctx );
                    task->last_run_time = nano_tp_impl_get_sys_time();
                    task->status.is_active = 0;
                }
            }
        }

        nano_tp_impl_thread_sleep(20);
    }
}

