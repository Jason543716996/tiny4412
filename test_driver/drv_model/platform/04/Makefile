
obj-m	:= dev1.o drv.o

KERNEL	:= /home/zhangjie/study/Linux_drv/linux-3.5

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 

