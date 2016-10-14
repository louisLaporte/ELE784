#Char Driver
##Compilation
```
$ make
```
###Load module
```
$ make load
```
###Unload module
```
$ make unload
```
##Driver usage

open 3 terminal
###dmesg output (terminal 1)
```
$ ln -s script/dmesg.sh dmesg.sh
$ ./dmesg.sh
```
###writer (terminal 2)
```
$ echo "abcde" >> /dev/scull_Node
```
###reader (terminal 3)
```
$ tail -f /dev/scull_Node
```
