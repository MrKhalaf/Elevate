cmake_minimum_required(VERSION 3.17)
project(Elevate C)

set(CMAKE_C_STANDARD 99)

add_executable(elevator_part_2 elevator_skeleton.c elevator_part_2.c fields.c finesleep.c jval.c jrb.c dllist.c dllist.h elevator.h fields.h finesleep.h jval.h jrb.h names.h)
#add_executable(elevator_part_1 elevator_skeleton.c elevator_part_1.c fields.c finesleep.c jval.c jrb.c dllist.c dllist.h elevator.h fields.h finesleep.h jval.h jrb.h names.h)