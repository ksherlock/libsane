cmake_minimum_required(VERSION 3.1)
set (PROJECT_TYPE "CXX")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SANE_SRC
	saneparser.cpp
	sane.cpp
)


add_custom_command(
	OUTPUT saneparser.cpp
	COMMAND ragel -p -G2 -o saneparser.cpp "${CMAKE_CURRENT_SOURCE_DIR}/saneparser.rl"
	MAIN_DEPENDENCY saneparser.rl 
)


add_library(SANE_LIB ${SANE_SRC})

add_library(Catch INTERFACE)


add_executable(sane_test sane_test.cpp)
target_link_libraries(sane_test Catch SANE_LIB)

enable_testing()
add_test(NAME Sanity COMMAND sane_test)