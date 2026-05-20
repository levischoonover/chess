cmake_minimum_required(VERSION 4.3.2)

project(random_move_bot LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(random_move_bot
	src/main.cpp
	src/util.cpp
)