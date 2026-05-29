# Makefile for lkm_debug
#
# CFLAGS_lkm_debug.o:
#   -pg          — инструментирует каждую функцию для ftrace (mcount/fentry).
#              Без этого флага функции модуля не появляются
#              в available_filter_functions и недоступны для ftrace/kprobe.
#   -mfentry     — использовать __fentry__ вместо mcount (x86-64, ядро >= 3.19).
#              С CONFIG_DYNAMIC_FTRACE это даёт нулевой overhead в нормальном режиме.
obj-m += lkm_debug.o
CFLAGS_lkm_debug.o := -pg -mfentry

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod lkm_debug.ko

reload:
	-sudo rmmod lkm_debug
	sudo insmod lkm_debug.ko

unload:
	sudo rmmod lkm_debug

info:
	modinfo lkm_debug.ko

dmesg:
	sudo dmesg | tail -20
