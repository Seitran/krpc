TARGET = krpc

ifneq ($(KERNELRELEASE), )
	obj-m := $(TARGET).o
	$(TARGET)-objs := krpc_main.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:
	cp *.ko $(KRPC_BASE)/output

clean:
	rm -rf *.ko *.o *.mod.* .H* .tm* .*cmd Module.symvers *.cmd *.order  $(KRPC_BASE)/output/*
endif


