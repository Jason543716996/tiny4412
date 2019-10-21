
obj-m	:= dev1.o dev2.o drv.o

KERNEL	:= /linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 

