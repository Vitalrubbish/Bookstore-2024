cmake_minimum_required(VERSION 3.22)

project(code)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(code
        src/book.cpp
        src/book.h
        src/diary.cpp
        src/diary.h
        src/log.cpp
        src/log.h
        main.cpp
        src/finance.h
        src/author.h
        src/keyword.h
)