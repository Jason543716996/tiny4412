
obj-m   := led.o

KERNEL  := /home/jason/Desktop/board/tiny4412/linux-4.4

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
