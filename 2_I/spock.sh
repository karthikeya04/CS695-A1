#!/bin/bash

make
gcc test_prog.c -o test_prog

insmod mem_ops_ioctl.ko

printf "\n\n=====================  APPLICATION OUTPUT  =====================\n\n"
./test_prog
printf "\n=================================================================\n\n"

rmmod mem_ops_ioctl.ko

rm test_prog
make clean



