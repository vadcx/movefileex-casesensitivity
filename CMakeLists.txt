cmake_minimum_required(VERSION 3.20)

# set the project name
project(movefileex-test)

# add the executable
add_executable(Win32 main.cpp)

# Use W versions of API
#add_definitions(-DUNICODE -D_UNICODE)
