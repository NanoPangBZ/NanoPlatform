#include "../nano_function_gruop.h"

extern const nano_function_item_t __nano_function_regedit_start;
extern const nano_function_item_t __nano_function_regedit_end;

#define NANO_FUNCTION_ITEM_REGEDIT_START_ADDR ((uintptr_t)&__nano_function_regedit_start)
#define NANO_FUNCTION_ITEM_REGEDIT_END_ADDR ((uintptr_t)&__nano_function_regedit_end)

int nano_function_gruop_call(nano_function_group_t group)
{
    int ret = 0;

    for( uint8_t curr_pri = 0; curr_pri < 4; curr_pri++)
    {
        const nano_function_item_t* item = (const nano_function_item_t*)NANO_FUNCTION_ITEM_REGEDIT_START_ADDR;
        while ((uintptr_t)item < NANO_FUNCTION_ITEM_REGEDIT_END_ADDR)
        {
            if (item->group == group &&  item->pri == curr_pri && item->function )
            {
                ret = item->function();
            }
            item++;
        }

        if( ret )
        {
            return ret;
        }
    }

    return 0;
}

