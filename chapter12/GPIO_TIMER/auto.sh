#!/bin/sh
sudo insmod gpiosignal_module.ko
#sudo insmod gpioirq_module.ko
#sudo insmod gpiofunction_module.ko
dmesg | tail -3
sudo mknod /dev/gpioled c 200 0
sudo chmod 666 /dev/gpioled
./catch_signal 1
sleep 1
./catch_signal 0
