cmake_minimum_required(VERSION 3.19)
project(HW3 C)

set(CMAKE_C_STANDARD 11)

add_executable(HW3 message_slot.c message_slot.h message_reader.c message_sender.c)