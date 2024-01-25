#!/bin/bash

# compile ioctl driver and the test program
make

# load the device
insmod mem_ops_ioctl.ko

printf "\n\n=====================  APPLICATION OUTPUT  =====================\n\n"

# execute user space application
./test_prog

printf "\n=================================================================\n\n"

# remove the device
rmmod mem_ops_ioctl

# clean
make clean



