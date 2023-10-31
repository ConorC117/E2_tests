obj-m += hello.o

.PHONY: build clean load unload

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(shell pwd)
	
clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(shell pwd)

load:
	sudo insmod hello.ko
	
unload:
	-sudo rmmod hello