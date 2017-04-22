cmake_minimum_required(VERSION 3.6)
set (PROJECT_NAME "libsane")
set (PROJECT_TYPE "CXX")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

include(ExternalProject)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_custom_command(
	OUTPUT src/saneparser.cpp
	COMMAND mkdir -p src
	COMMAND ragel -p -G2 -o src/saneparser.cpp "${CMAKE_CURRENT_SOURCE_DIR}/src/saneparser.rl"
	MAIN_DEPENDENCY src/saneparser.rl 
)


add_library(sane
	src/saneparser.cpp
	src/sane.cpp
	src/fpinfo.cpp
)

target_include_directories(sane PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)


ExternalProject_Add(Catch-External
	DOWNLOAD_NO_EXTRACT 1
	URL https://github.com/philsquared/Catch/releases/download/v1.9.1/catch.hpp
	URL_HASH MD5=e392ca7010ec9caeb7a9899659cfdd31
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <DOWNLOADED_FILE> ${CMAKE_BINARY_DIR}/catch.hpp
)

add_library(Catch INTERFACE)
add_dependencies(Catch Catch-External)

add_executable(sane_test src/sane_test.cpp)
target_link_libraries(sane_test Catch sane)

enable_testing()
add_test(NAME Sanity COMMAND sane_test)