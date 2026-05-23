cmake_minimum_required(VERSION 4.3.2)

project(random_move_bot LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(random_move_bot_executable
	src/main.cpp
	src/game_state.cpp
	src/search.cpp
    src/util.cpp
)

set_target_properties(random_move_bot_executable PROPERTIES OUTPUT_NAME "Random Move Bot")