#!/usr/bin/env bash

declare RED=$(printf '\033[31m')
declare GRN=$(printf '\033[32m')
declare YEL=$(printf '\033[33m')
declare BLU=$(printf '\033[34m')
declare MAG=$(printf '\033[35m')
declare CYA=$(printf '\033[36m')
declare RST=$(printf '\033[0m')

sudo dmesg -C
watch --color "dmesg | tail -n 50 | sed \"
                        s/scull_read/${MAG}&${RST}/
                        s/scull_write/${YEL}m&${RST}/
                        s/scull_release/${RED}m&${RST}/
                        s/scull_init/${CYA}m&${RST}/
                        s/scull_open/${GRN}m&${RST}/
                        \""
