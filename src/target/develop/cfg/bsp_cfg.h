#pragma once

/* RGB灯条映射表 */
#define BSP_RGB_LIGHT_BAR_MAP_TABLE {\
        {\
            .name = "default",\
            .arch_one_wire_port_idx = 0,\
            .rgb_cnt = 30\
        }\
    }

/* led映射表 */
#define BSP_LED_MAP_TABLE {\
        {\
            .name = "tick",\
            .pin = 0,\
            .active_level = 0,\
        },\
    }
