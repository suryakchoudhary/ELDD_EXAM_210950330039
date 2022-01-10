obj-m := rdwr_ioctl.o

KERN = /lib/modules/$(shell uname -r)/build/

PWD:=$(shell pwd)

default:
	make -C $(KERN) M=$(PWD) modules
	gcc -o writer writer.c
	gcc -o reader reader.c

clean:
	make -C $(KERN) M=$(PWD) clean
	rm writer reader 

