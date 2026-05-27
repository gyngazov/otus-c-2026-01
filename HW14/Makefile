
TGT=charmod
obj-m += ${TGT}.o

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