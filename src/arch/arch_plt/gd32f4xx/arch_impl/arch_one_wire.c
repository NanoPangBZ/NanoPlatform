#include "arch/arch_one_wire.h"
#include "arch_cfg.h"

#ifdef ARCH_ONE_WIRE_MAP_TABLE

#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_dma.h"
#include "gd32f4xx_timer.h"
#include <string.h>

typedef struct arch_pin_map_t{
    uint32_t gpio_periph;
    uint32_t gpio_pin;
    uint32_t gpio_af;
    uint32_t gpio_rcu;
}arch_pin_map_t;

typedef struct arch_one_wire_map_t{
    uint32_t timer_rcu;
    uint32_t timer_periph;
    uint16_t timer_channel;
    arch_pin_map_t timer_channel_pin_map;
    uint32_t timer_clk_prescaler;
    uint16_t timer_prescaler;
    uint16_t timer_period;
    uint16_t timer_repetition_counter;
    uint16_t bit0_pulse;
    uint16_t bit1_pulse;
    uint16_t stop_pulse_count;
    uint32_t dma_periph;
    uint32_t dma_rcu;
    dma_channel_enum dma_channel;
    dma_subperipheral_enum dma_subperiph;
    uint32_t dma_priority;
}arch_one_wire_map_t;

typedef struct arch_one_wire_ins_t{
    const arch_one_wire_map_t* map;
    uint16_t bits_buffer[256];
    arch_one_wire_write_callback_t write_cb;
    void* write_cb_ctx;
    uint8_t is_sending;
}arch_one_wire_ins_t;

#define ARCH_ONE_WIRE_PWM_BUF_SIZE   (uint32_t)(sizeof(((arch_one_wire_ins_t*)0)->bits_buffer) / sizeof(uint16_t))

static const arch_one_wire_map_t one_wire_map_table[] = ARCH_ONE_WIRE_MAP_TABLE;

static arch_one_wire_ins_t one_wire_ins_table[ sizeof(one_wire_map_table) / sizeof(one_wire_map_table[0]) ];

static uint32_t arch_one_wire_timer_chcv_addr( uint32_t timer_periph , uint16_t timer_channel )
{
    switch( timer_channel )
    {
        case TIMER_CH_0:
            return (uint32_t)TIMER_CH0CV( timer_periph );
        case TIMER_CH_1:
            return (uint32_t)TIMER_CH1CV( timer_periph );
        case TIMER_CH_2:
            return (uint32_t)TIMER_CH2CV( timer_periph );
        case TIMER_CH_3:
            return (uint32_t)TIMER_CH3CV( timer_periph );
        default:
            return (uint32_t)TIMER_CH0CV( timer_periph );
    }
}

static void arch_pwm_gpio_init( arch_one_wire_ins_t* ins )
{
    const arch_pin_map_t* pin = &ins->map->timer_channel_pin_map;

    rcu_periph_clock_enable( pin->gpio_rcu );
    gpio_mode_set( pin->gpio_periph , GPIO_MODE_AF , GPIO_PUPD_NONE , pin->gpio_pin );
    gpio_output_options_set( pin->gpio_periph , GPIO_OTYPE_PP , GPIO_OSPEED_50MHZ , pin->gpio_pin );
    gpio_af_set( pin->gpio_periph , pin->gpio_af , pin->gpio_pin );
}

static void arch_pwm_timer_init( arch_one_wire_ins_t* ins )
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable( ins->map->timer_rcu );
    rcu_timer_clock_prescaler_config( ins->map->timer_clk_prescaler );
    timer_deinit( ins->map->timer_periph );

    timer_struct_para_init( &timer_initpara );
    timer_initpara.prescaler = ins->map->timer_prescaler;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = ins->map->timer_period;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = ins->map->timer_repetition_counter;
    timer_init( ins->map->timer_periph , &timer_initpara );

    timer_channel_output_struct_para_init( &timer_ocintpara );
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config( ins->map->timer_periph , ins->map->timer_channel , &timer_ocintpara );

    timer_channel_output_pulse_value_config( ins->map->timer_periph , ins->map->timer_channel , 0U );
    timer_channel_output_mode_config( ins->map->timer_periph , ins->map->timer_channel , TIMER_OC_MODE_PWM0 );
    timer_channel_output_shadow_config( ins->map->timer_periph , ins->map->timer_channel , TIMER_OC_SHADOW_DISABLE );

    if( ( ins->map->timer_periph == TIMER0 ) || ( ins->map->timer_periph == TIMER7 ) )
    {
        timer_primary_output_config( ins->map->timer_periph , ENABLE );
    }

    timer_dma_enable( ins->map->timer_periph , TIMER_DMA_UPD );
    timer_auto_reload_shadow_enable( ins->map->timer_periph );
    timer_disable( ins->map->timer_periph );
}

static void arch_pwm_timer_dma_init( arch_one_wire_ins_t* ins )
{
    dma_single_data_parameter_struct dma_init_struct;

    rcu_periph_clock_enable( ins->map->dma_rcu );
    dma_deinit( ins->map->dma_periph , ins->map->dma_channel );

    dma_single_data_para_struct_init( &dma_init_struct );
    dma_init_struct.periph_addr = arch_one_wire_timer_chcv_addr( ins->map->timer_periph , ins->map->timer_channel );
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory0_addr = (uint32_t)ins->bits_buffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.number = 0U;
    dma_init_struct.priority = ins->map->dma_priority;
    dma_single_data_mode_init( ins->map->dma_periph , ins->map->dma_channel , &dma_init_struct );
    dma_channel_subperipheral_select( ins->map->dma_periph , ins->map->dma_channel , ins->map->dma_subperiph );
}

static uint32_t arch_one_wire_fill_pwm_buffer( arch_one_wire_ins_t* ins , const uint8_t* buf , uint32_t bit_count )
{
    const arch_one_wire_map_t* map = ins->map;
    uint32_t pulse_count = bit_count + map->stop_pulse_count;

    if( ( buf == NULL ) || ( bit_count == 0U ) || ( pulse_count > ARCH_ONE_WIRE_PWM_BUF_SIZE ) )
    {
        return 0U;
    }

    for( uint32_t i = 0U; i < bit_count; i++ )
    {
        uint32_t byte_idx = i / 8U;
        uint32_t bit_idx = 7U - ( i % 8U );
        ins->bits_buffer[i] = ( ( buf[byte_idx] & ( 1U << bit_idx ) ) != 0U ) ? map->bit1_pulse : map->bit0_pulse;
    }

    for( uint32_t i = 0U; i < map->stop_pulse_count; i++ )
    {
        ins->bits_buffer[bit_count + i] = 0U;
    }

    return pulse_count;
}

static void arch_one_wire_wait_idle( arch_one_wire_ins_t* ins )
{
    while( ins->is_sending )
    {
    }
}

static void arch_one_wire_dma_start( arch_one_wire_ins_t* ins , uint32_t pulse_count )
{
    const arch_one_wire_map_t* map = ins->map;

    dma_channel_disable( map->dma_periph , map->dma_channel );
    dma_flag_clear( map->dma_periph , map->dma_channel , DMA_FLAG_FTF );

    timer_counter_value_config( map->timer_periph , 0U );
    timer_channel_output_pulse_value_config( map->timer_periph , map->timer_channel , ins->bits_buffer[0] );

    if( pulse_count > 1U )
    {
        dma_memory_address_config( map->dma_periph , map->dma_channel , DMA_MEMORY_0 , (uint32_t)&ins->bits_buffer[1] );
        dma_transfer_number_config( map->dma_periph , map->dma_channel , pulse_count - 1U );
        dma_channel_enable( map->dma_periph , map->dma_channel );
    }

    timer_enable( map->timer_periph );
}

static void arch_one_wire_dma_wait_done( arch_one_wire_ins_t* ins , uint32_t pulse_count )
{
    const arch_one_wire_map_t* map = ins->map;

    if( pulse_count > 1U )
    {
        while( dma_flag_get( map->dma_periph , map->dma_channel , DMA_FLAG_FTF ) == RESET )
        {
        }
        dma_flag_clear( map->dma_periph , map->dma_channel , DMA_FLAG_FTF );
        dma_channel_disable( map->dma_periph , map->dma_channel );
    }

    while( timer_flag_get( map->timer_periph , TIMER_FLAG_UP ) == RESET )
    {
    }
    timer_flag_clear( map->timer_periph , TIMER_FLAG_UP );

    timer_disable( map->timer_periph );
    timer_channel_output_pulse_value_config( map->timer_periph , map->timer_channel , 0U );
}

void arch_one_wire_init( arch_one_wire_port_t port )
{
    if( port >= sizeof(one_wire_map_table) / sizeof(one_wire_map_table[0]) )
    {
        return;
    }

    arch_one_wire_ins_t* ins = &one_wire_ins_table[port];
    memset( ins , 0 , sizeof(arch_one_wire_ins_t) );
    ins->map = &one_wire_map_table[port];

    arch_pwm_gpio_init( ins );
    arch_pwm_timer_init( ins );
    arch_pwm_timer_dma_init( ins );
}

void arch_one_wire_deinit( arch_one_wire_port_t port )
{
    //@todo
    (void)port;
}

void arch_one_wire_reset( arch_one_wire_port_t port )
{
    //@todo
    (void)port;
}

uint32_t arch_one_wire_send( arch_one_wire_port_t port , const uint8_t* buf , uint32_t bit_count )
{
    if( port >= sizeof(one_wire_map_table) / sizeof(one_wire_map_table[0]) )
    {
        return 0U;
    }

    arch_one_wire_ins_t* ins = &one_wire_ins_table[port];
    arch_one_wire_wait_idle( ins );

    uint32_t pulse_count = arch_one_wire_fill_pwm_buffer( ins , buf , bit_count );
    if( pulse_count == 0U )
    {
        return 0U;
    }

    ins->is_sending = 1U;
    arch_one_wire_dma_start( ins , pulse_count );
    arch_one_wire_dma_wait_done( ins , pulse_count );
    ins->is_sending = 0U;

    if( ins->write_cb != NULL )
    {
        ins->write_cb( port , ins->write_cb_ctx );
    }

    return bit_count;
}

uint32_t arch_one_wire_read( arch_one_wire_port_t port , uint8_t* buf , uint32_t bit_count )
{
    //@todo
    (void)port;
    (void)buf;
    (void)bit_count;
    return 0U;
}

uint32_t arch_one_write_set_wire_finish_cb( arch_one_wire_port_t port , arch_one_wire_write_callback_t write_cb , void* ctx )
{
    if( port >= sizeof(one_wire_map_table) / sizeof(one_wire_map_table[0]) )
    {
        return 1U;
    }

    arch_one_wire_ins_t* ins = &one_wire_ins_table[port];
    ins->write_cb = write_cb;
    ins->write_cb_ctx = ctx;
    return 0U;
}

#endif
