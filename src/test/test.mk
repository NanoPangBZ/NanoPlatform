TEST_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#测试源文件
# TEST_SRCS += $(TEST_DIR)nano_polling_pool_test.c
# TEST_SRCS += $(TEST_DIR)gd32f4xx_gpio_write_test.c
# TEST_SRCS += $(TEST_DIR)arch_gpio_test.c
# TEST_SRCS += $(TEST_DIR)nano_net_host_test.c
# UART测试一次只能启用一个，避免多个用例同时占用同一端口
# TEST_SRCS += $(TEST_DIR)arch_uart/arch_uart_send_test.c
# TEST_SRCS += $(TEST_DIR)arch_uart/arch_uart_recieve_test.c
TEST_SRCS += $(TEST_DIR)arch_uart/arch_uart_echo.c
# TEST_SRCS += $(TEST_DIR)arch_uart/arch_uart_non_block_send_test.c
# TEST_SRCS += $(TEST_DIR)arch_uart/arch_uart_non_block_recieve_test.c
# TEST_SRCS += $(TEST_DIR)nano_msg_test.c
# TEST_SRCS += $(TEST_DIR)arch_one_wire_test.c
