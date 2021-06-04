#! /bin/sh

sudo rm /dev/slot0
sudo rm /dev/slot1
sudo rmmod message_slot
make clean > /dev/null
