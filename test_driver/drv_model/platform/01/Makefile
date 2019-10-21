
obj-m	:= dev.o drv.o

KERNEL	:= /linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 

