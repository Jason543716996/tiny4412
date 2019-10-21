
obj-m	:= demo.o demo2.o demo3.o demo4.o demo5.o demo6.o demo7.o

KERNEL	:= ~/linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 

