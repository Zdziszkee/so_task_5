cmake_minimum_required(VERSION 3.24)
project(so_task_5 C)

set(CMAKE_C_STANDARD 23)

add_executable(so_task_5 producer.c consumer.c so_task_5.c)
