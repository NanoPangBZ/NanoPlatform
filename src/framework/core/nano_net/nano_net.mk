NANO_NET_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# nano_net模块的源文件
NANO_NET_SRCS += $(NANO_NET_DIR)nano_net.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/nano_net_packager.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/nano_net_port.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/lib/nano_net_crc16.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/lib/nano_net_list.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/lib/nano_net_ringbuf.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/ll_depend/nano_net_ll_heap.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/ll_depend/nano_net_ll_systime.c
NANO_NET_SRCS += $(NANO_NET_DIR)private/package/nano_net_default_packager.c
