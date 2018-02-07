#machine=powerpc
machine=x86_64
#PWD=$(shell pwd)
#LINUX_KERNEL=/home/qing/kernel/linux-2.6.35-r2
LINUX_KERNEL=/lib/modules/2.6.32-431.el6.x86_64/source/
#CROSS_COMPILE=powerpc-linux-gnu-
CC=$(CROSS_COMPILE)gcc

obj-m :=shm.o

modules:
	make -C $(LINUX_KERNEL) M=$(PWD) modules ARCH=$(machine)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers
	rm -rf *.ko.unsigned

.PHONY:modules clean
