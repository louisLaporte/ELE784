## Module variables
MODULE_DIR:=$(PWD)
MODULE_NAME:=scull
MODULE_OBJECT:=$(MODULE_DIR)/$(MODULE_NAME).ko
## Binary
BIN_DIR:="./bin"
## Use bash as shell
SHELL=/usr/bin/env bash
# Color log variables
INFO    = echo -e "[ \e[32mInfo\e[0m ]"
WARNING = echo -e "[ \e[33mWarning\e[0m ]"
ERROR   = echo -e "[ \e[31mError\e[0m ]"
	

SRC     :=  src/module/$(MODULE_NAME).c
$(MODULE_NAME)-objs = $(SRC:.c=.o)
obj-m := $(MODULE_NAME).o
KERNEL_DIR:=/usr/src/linux-headers-3.16.0
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
load: all
	sudo insmod $(MODULE_OBJECT)
	@make --silent info  
	@make --silent clean
#sudo mknod /dev/$(MODULE_NAME) c 250 0
	sudo chmod 666 /dev/$(MODULE_NAME)_Node
################################################################################
#                                                                              #
#      Unload the module:                                                      #
#                        - if module exist it will be remove else exit         #
#                                                                              #
################################################################################
unload:
	@sudo rmmod $(MODULE_NAME) 2>/dev/null || \
	    ($(ERROR) "No module" $(MODULE_NAME); exit 1)
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
	-rm -f src/application/*.o src/module/*.o *.o *.ko .*.cmd .*.flags *.mod.c Module.symvers modules.order
	-rm -rf .tmp_versions
################################################################################
#                                                                              #
#      compile the user interface from src/application to usr/bin              #
#                                                                              #
################################################################################
app:
	@[ -d "$(BIN_DIR)" ] || mkdir $(BIN_DIR)
	gcc -o $(BIN_DIR)/ui src/application/main.c
################################################################################
#                                                                              #
#      run the user interface binary                                           #
#                                                                              #
################################################################################
run: app clean
	$(BIN_DIR)/ui

root: app clean
	sudo $(BIN_DIR)/ui
