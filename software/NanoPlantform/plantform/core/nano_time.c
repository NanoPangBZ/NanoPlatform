#include "nano_time.h"
#include "nano_bsp_cpu.h"
#include "nano_io_device.h"

static uint32_t sys_tick = 0;
static nano_io_dev_handle_t led_dev = NULL;

static nano_err_t nano_tick_led_init(void* args)
{
    (void)args;
    if( NANO_OK != nano_io_device_open( "debug led" , NANO_IO_READ_WRITE , NANO_BIO , &led_dev ) )
    {
        led_dev = NULL;
    }
    return led_dev ? NANO_OK : NANO_NO_INSTANCE;
}

static void nano_tick_led_run(void)
{
    if( led_dev != NULL )
    {
        static uint16_t led_tick = 0;
        static uint8_t led_on = 0;
        uint16_t temp;
        if( (led_on && led_tick == 100) ||
            (!led_on && led_tick == 500))
        {
            led_tick = 0;
            led_on = !led_on;
            nano_io_device_write(led_dev,&led_on,1,&temp);
        }
        led_tick++;
    }
}

uint32_t nano_sys_time_ms(void)
{
    return sys_tick;
}

static void nano_plantform_systick_handler(void)
{
    nano_tick_led_run();
    sys_tick++;
}

static nano_err_t nano_plantform_systick_init(void* args)
{
    (void)args;

    nano_bsp_interrupt_disable();

    nano_bsp_set_systick_fre( 1000 );
    nano_bsp_register_systick_cb(nano_plantform_systick_handler);
    nano_bsp_systick_start();

    nano_bsp_interrupt_enable();

    return NANO_OK;
}

#include "nano_func_manager.h"

LOAD_FUNC_TO_FUNC_MANAGER( nano_plantform_systick_init , NANO_PLTFM_PRE_INIT_FUNC_GROUP );
LOAD_FUNC_TO_FUNC_MANAGER( nano_tick_led_init , NANO_PLTFM_SVC_ENTER_FUNC_GRUOP );

