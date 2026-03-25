#include <stdint.h>
#include "arch/arch_init.h"
#include "framework/nano_framework_core.h"

/**
 * @brief The application entry point.
*/
int main(void)
{
    arch_init();
    enter_nano_framework();
    return 0;
}

