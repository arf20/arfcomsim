cmake_minimum_required(VERSION 3.20)

project(arfcomsim)

include_directories("libecon/")

file(GLOB SRC_LIBECON "libecon/*.c")
file(GLOB SRC_ARFCOMSIM "arfcomsim/*.c")

add_library(econ STATIC ${SRC_LIBECON})

add_executable(arfcomsim ${SRC_ARFCOMSIM})
target_link_libraries(arfcomsim econ m)

