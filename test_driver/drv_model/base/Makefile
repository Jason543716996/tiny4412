
obj-m	:= bus.o dev1.o dev2.o dev3.o drv1.o drv2.o

KERNEL	:= /linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 

