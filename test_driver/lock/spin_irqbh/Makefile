obj-m		:= linyige.o
linyige-objs	:= demo.o tasklet.o
KERNEL	:= /linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules;
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
