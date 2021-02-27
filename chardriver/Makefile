
obj-m += chardevdriver.o

KDIR = /usr/src/linux-headers-5.4.0-1039-azure

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) chardevdriver

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *.mod .*.ko.cmd .*.mod.* .*.o.cmd
