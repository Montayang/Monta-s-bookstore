cmake_minimum_required(VERSION 3.17)
project(bookstore)

set(CMAKE_CXX_STANDARD 14)

add_executable(bookstore main.cpp
        user.cpp user.h Bookstore.h Bookstore.cpp command.cpp command.h blocklist.cpp blocklist.h)
