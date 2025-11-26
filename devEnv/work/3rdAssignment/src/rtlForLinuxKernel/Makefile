# Makefile for assignment_sensor kernel module
# Follows the same structure shown in presentation slides:
#   obj-m += module.o
#   make -C /lib/modules/$(uname -r)/build M=$(PWD) modules

obj-m += assignment_sensor.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
		