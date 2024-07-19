#include "nano_plantform.h"
#include "nano_time.h"
#include "nano_io_device.h"

static void test_demo(void)
{
    nano_plantform_init();

    nano_io_dev_handle_t handle = nano_io_device_open( "debug led" , NANO_IO_READ_WRITE , NANO_BIO );

    uint32_t last_time = nano_sys_time_ms();
    uint8_t data = 0;
    while(1)
    {
        if( nano_sys_time_ms() - last_time > 500 )
        {
            nano_io_device_write( handle , &data , 1 , NULL );
            data = !data;
            last_time = nano_sys_time_ms();
        }
    }
}

int main(void)
{
    test_demo();
    return -1;
}
