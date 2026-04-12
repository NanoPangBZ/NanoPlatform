TEST_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#测试源文件
# TEST_SRCS += $(TEST_DIR)nano_polling_pool_test.c
# TEST_SRCS += $(TEST_DIR)gd32f4xx_gpio_write_test.c
# TEST_SRCS += $(TEST_DIR)arch_gpio_test.c
# TEST_SRCS += $(TEST_DIR)nano_net_host_test.c
TEST_SRCS += $(TEST_DIR)arch_uart_test.c
# TEST_SRCS += $(TEST_DIR)nano_msg_test.c
