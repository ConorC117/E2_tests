obj-m += proc_kdlp.o

.PHONY: build clean load unload

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(shell pwd) && chmod 0444 proc_kdlp.ko
	
clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(shell pwd)

load:
	sudo insmod proc_kdlp.ko
	
unload:
	-sudo rmmod proc_kdlp