# SPDX-License-Identifier: GPL-2.0

ifneq ($(KERNELRELEASE),)
obj-m := cp210x.o
CFLAGS_cp210x.o := -DDEBUG

else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

endif
