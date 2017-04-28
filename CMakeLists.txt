cmake_minimum_required(VERSION 3.1)
set (PROJECT_NAME "libsane")
set (PROJECT_TYPE "CXX")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

include(sane_config.cmake)


set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_custom_command(
	OUTPUT src/saneparser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory src
	COMMAND ragel -p -G2 -o src/saneparser.cpp "${CMAKE_CURRENT_SOURCE_DIR}/src/saneparser.rl"
	MAIN_DEPENDENCY src/saneparser.rl 
)


add_library(sane
	src/saneparser.cpp
	src/sane.cpp
	src/floating_point.cpp
)

target_include_directories(sane PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)


add_executable(sane_test src/sane_test.cpp)
target_link_libraries(sane_test sane)

enable_testing()
add_test(NAME sane_test COMMAND sane_test)
