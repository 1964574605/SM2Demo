cmake_minimum_required(VERSION 3.19)
project(SM2Demo)

set(CMAKE_CXX_STANDARD 14)

#add_executable(SM2Demo basedata.h tool.h coordinate.h sm3.h)
#add_executable(SM2Demo main.cpp basedata.h tool.h coordinate.h sm3.h)
add_executable(SM2Demo main.cpp basedata.h tool.h coordinate.h ellipse.h)