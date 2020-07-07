cmake_minimum_required (VERSION 3.0.0)

project(DriverWalkInPlace)

# The version number.
set (DriverWalkInPlace_VERSION_MAJOR 1)
set (DriverWalkInPlace_VERSION_MINOR 0)
 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "./lib")
 
include_directories ("src/com/shm/")
include_directories ("src/virtualdevice/")
include_directories ("src/driver/")
include_directories ("../openvr/headers/")
include_directories ("../third-party/boost_1_65_1/")
include_directories ("../third-party/easylogging++/")
include_directories ("../lib_vrwalkinplace/include/")

set(SOURCES
	src/com/shm/driver_ipc_shm.cpp 
	src/virtualdevice/VirtualController.cpp 
	src/driver/ServerDriver.cpp 
	src/driver/WatchdogProvider.cpp
	)
	
add_library(DriverWalkInPlace SHARED ${SOURCES})


