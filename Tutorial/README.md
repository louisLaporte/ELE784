###Hello dev
##Driver creation
#Compile source
```
$ make 
```
#Insert module to kernel
```
$ sudo insmod ./HelloDev.ko
```
#List module and get our module
```
$ lsmod | grep HelloDev
```
#Get module's info
```
$ modinfo ./HelloDev.ko
```
#Remove module
```
$ sudo rmmod HelloDev.ko
```
#Print kernel ring buffer
```
$ dmesg
```
or
```
$ journalctl -k --since "10 minute ago" 
```
