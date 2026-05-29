# Makefile for lkm_debug
#
# CFLAGS_lkm_debug.o:
#   -pg          — инструментирует каждую функцию для ftrace (mcount/fentry).
#              Без этого флага функции модуля не появляются
#              в available_filter_functions и недоступны для ftrace/kprobe.
#   -mfentry     — использовать __fentry__ вместо mcount (x86-64, ядро >= 3.19).
#              С CONFIG_DYNAMIC_FTRACE это даёт нулевой overhead в нормальном режиме.
TGT=mod_123
obj-m += ${TGT}.o
#CFLAGS_lkm_debug.o := -pg -mfentry

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod ${TGT}.ko

unload:
	sudo rmmod ${TGT}

info:
	modinfo ${TGT}.ko

dmesg:
	sudo dmesg | tail -20
