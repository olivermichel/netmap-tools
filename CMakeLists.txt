cmake_minimum_required(VERSION 3.5)
project(libom2)

set(CMAKE_CXX_STANDARD 11)

find_path(NETMAP_INC NAMES net/netmap_user.h HINTS /usr/ /usr/local/)
find_file(NETMAP_HEADER NAMES net/netmap_user.h HINTS ${NETMAP_INC})

if(NOT NETMAP_HEADER)
	message(FATAL_ERROR "Detecting netmap: not found - disable support")
else()
	message(STATUS "Detecting netmap: NETMAP_HEADER:${NETMAP_HEADER} - done")
	set(CMAKE_CXX_FLAGS "-fpermissive")
endif()

set(CATCH_VERSION 2.2.2)

if (NOT EXISTS ${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
	file(DOWNLOAD
		https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp
		${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
endif ()

enable_testing()

add_executable(test_runner test/test_runner.cc test/iface_test.cc  include/netmap_tools.h)
target_include_directories(test_runner PUBLIC include)
target_include_directories(test_runner PUBLIC test/include)

add_test(NAME netmap_iface WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND test_runner netmap_iface)

