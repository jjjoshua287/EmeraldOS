SRCS += kernel/kernel.c kernel/printk.c kernel/panic.c kernel/tests.c

# version info printed in kernel.c, cflag add here rather than root makefile
# Notice the escaped quotes: '\"'
CFLAGS += -DKERNEL_VERSION='"$(VERSION)-$(GIT_HASH)"'
CFLAGS += -DCFG_TEST
CFLAGS += -DCFG_PANIC