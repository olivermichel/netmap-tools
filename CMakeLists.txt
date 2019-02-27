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

set(CXXOPTS_VERSION 2.0.0)

if(NOT EXISTS ${CMAKE_HOME_DIRECTORY}/ext/include/cxxopts/cxxopts.h)
	file(DOWNLOAD
		https://raw.githubusercontent.com/jarro2783/cxxopts/v${CXXOPTS_VERSION}/include/cxxopts.hpp
		${CMAKE_HOME_DIRECTORY}/ext/include/cxxopts/cxxopts.h)
endif()

set(CATCH_VERSION 2.2.2)

if (NOT EXISTS ${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
	file(DOWNLOAD
		https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp
		${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
endif ()

enable_testing()

add_executable(unit test/unit.cc test/iface_test.cc  include/netmap_tools.h)
target_include_directories(unit PUBLIC include)
target_include_directories(unit PUBLIC test/include)

add_test(NAME iface WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND test_runner iface)

add_executable(pkt_sender examples/pkt_sender.cc examples/pkt_sender_config.h include/netmap_tools.h)
target_include_directories(pkt_sender PUBLIC include)
target_include_directories(pkt_sender PUBLIC ext/include)

add_executable(pkt_receiver examples/pkt_receiver.cc examples/pkt_receiver_config.h include/netmap_tools.h)
target_include_directories(pkt_receiver PUBLIC include)
target_include_directories(pkt_receiver PUBLIC ext/include)
