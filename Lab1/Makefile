## Module variables
MODULE_DIR:=$(PWD)
MODULE_NAME:=scull
MODULE_OBJECT:=$(MODULE_DIR)/$(MODULE_NAME).ko
## Use bash as shell
SHELL=/usr/bin/env bash
# Color log variables
INFO   =echo -e "[ \e[32mInfo\e[0m ]"
WARNING=echo -e "[ \e[33mWarning\e[0m ]"
ERROR  =echo -e "[ \e[31mError\e[0m ]"
	

SRC     :=  src/module/$(MODULE_NAME).c
$(MODULE_NAME)-objs = $(SRC:.c=.o)
obj-m := $(MODULE_NAME).o
KERNEL_DIR?=/usr/src/linux-headers-3.16.0
PWD:= $(shell pwd)
EXTRA_CFLAGS := -I$(PWD)/src/module
################################################################################
#                                                                              #
#      Build the module char_dev from directory src/module                     #
#                                                                              #
################################################################################
all: 
	@echo "Building the ELE784 Lab1: Ring buffer driver"
	@$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(MODULE_DIR) modules
################################################################################
#                                                                              #
#      Load the module:                                                        #
#                      - check if *.ko file exists else exit                   #
#                      - insert module                                         #
#                      - display module's info                                 #
#                      - clean garbage                                         #
#                      - create a node for the new device                      #
#                      - change permission for the device rw-rw-rw             #
#                                                                              #
################################################################################
load:
	$(call FileExist, $(MODULE_OBJECT))
#	$(call ModuleExist, $(MODULE_NAME))
	sudo insmod $(MODULE_OBJECT)
	@make --silent info  
	@make --silent clean
	sudo mknod /dev/$(MODULE_NAME) c 250 0
	sudo chmod 666 /dev/$(MODULE_NAME)_Node
################################################################################
#                                                                              #
#      Unload the module:                                                      #
#                        - if module exist it will be remove else exit         #
#                        - remove device                                       #
#                                                                              #
################################################################################
unload:
	@sudo rmmod $(MODULE_NAME) 2>/dev/null || \
	    ($(ERROR) "No module" $(MODULE_NAME); exit 1)
	sudo rm /dev/$(MODULE_NAME)_Node 
################################################################################
#                                                                              #
#      give info about module else print error                                 #
#                                                                              #
################################################################################
info:
	@lsmod | grep $(MODULE_NAME) || $(ERROR) "No module" $(MODULE_NAME) 
################################################################################
#                                                                              #
#      clean garbage                                                           #
#                                                                              #
################################################################################
clean:
	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c Module.symvers modules.order
	-rm -rf .tmp_versions
################################################################################
#                                                                              #
#      compile the user interface from src/application to usr/bin              #
#                                                                              #
################################################################################
app:
	gcc -o bin/ui src/application/main.c
################################################################################
#                                                                              #
#      run the user interface binary                                           #
#                                                                              #
################################################################################
run:
	./bin/ui
################################################################################
#                                                                              #
#      @function: FileExist                                                    #
#      @param   : $1 = file name                                               #
#      @description: check if file *.ko exist else exit                        #
#                                                                              #
#                                                                              #
################################################################################
define FileExist
@[ -f $(MODULE_OBJECT) ] || ($(ERROR) "File $(1) not found" ; \
   $(INFO) "You must run make before" ; exit 1)
endef

define ModuleExist
@lsmod  | grep $(1) && ($(INFO) "Module $(1) already loaded" ; exit 1)
endef

