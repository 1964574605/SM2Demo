cmake_minimum_required(VERSION 3.19)
project(SM2Demo)

set(CMAKE_CXX_STANDARD 14)


# 修改clion 内存
MATH(EXPR stack_size "4*1024*1024")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")

#add_executable(SM2Demo basedata.h tool.h coordinate.h sm3.h)
#add_executable(SM2Demo main.cpp basedata.h tool.h coordinate.h sm3.h)
add_executable(SM2Demo main.cpp basedata.h tool.h coordinate.h ellipse.h key.h ciphertext.h sm3.h)

