#!/usr/bin/env bash

which indent
if [ $? -neq 0 ]
then
    sudo apt-get install indent
fi
if [ $# -lt 1 ]
then
    echo "You must specify a file to indent"
    echo "usgae: ./indent.sh my_file"
    exit 1
fi
indent -kr -i8 -ts8 -sob -l80 -ss -bs -psl $1
