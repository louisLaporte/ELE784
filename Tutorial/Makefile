KERNEL_DIR:=/usr/src/linux-headers-3.16.0
PWD:= $(shell pwd)
DRIVER_NAME:=HelloDev

obj-m:=$(DRIVER_NAME).o
	
all: ele784_driver

ele784_driver:
	@echo "Building the ELE784 Tutorial: Hello driver"
	@make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules

load:
	@echo "[ Root level ] insert module $(DRIVER_NAME)"
	@sudo insmod $(DRIVER_NAME).ko
	@$(MAKE) --silent info
	@$(MAKE) --silent clean

unload:
	@echo "[ Root level ] remove module $(DRIVER_NAME)"
	@sudo rmmod $(DRIVER_NAME)

info:
	@lsmod | grep $(DRIVER_NAME) || \
	(echo "No module $(DRIVER_NAME)"; exit 1)
			
clean:
	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c Module.symvers modules.order
	-rm -rf .tmp_versions
