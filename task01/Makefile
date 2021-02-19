# SPDX-License-Identifier: GPL-2.0

ifneq ($(KERNELRELEASE),)
obj-m := hello.o
CFLAGS_hello.o := -DDEBUG

else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

endif
