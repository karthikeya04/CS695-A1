#!/bin/bash

## Building the applications
make 

## Build your ioctl driver and load it here
insmod chng_par_ioctl.ko

###############################################

# Launching the control station
./control_station &
c_pid=$!
echo "[Dr. Doom] Control station PID: $c_pid"

# sleep to make sure that the custom sighandler in 
# control station is registered
sleep 3

# Launching the soldier
./soldier $c_pid &
echo "[Dr. Doom] Soldier PID: $!"

sleep 3
kill -9 $c_pid

## Remove the driver here
rmmod chng_par_ioctl
make clean

