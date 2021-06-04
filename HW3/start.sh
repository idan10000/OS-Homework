#! /bin/sh

make > /dev/null
sudo insmod message_slot.ko
sudo mknod /dev/slot0 c 240 5
sudo mknod /dev/slot1 c 240 6
sudo chmod 777 /dev/slot0
sudo chmod 777 /dev/slot1
