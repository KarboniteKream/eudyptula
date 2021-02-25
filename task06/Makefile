# SPDX-License-Identifier: GPL-2.0

ifneq ($(KERNELRELEASE),)
obj-m := eudyptula.o
CFLAGS_eudyptula.o := -DDEBUG

else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

endif
