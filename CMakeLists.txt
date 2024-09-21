cmake_minimum_required(VERSION 3.29)
project(InformationSecurityLabs)

set(CMAKE_CXX_STANDARD 26)

add_executable(InformationSecurityLabs main.cpp
        lab1_1/cardano.cpp
        lab1_1/cardano.h)
