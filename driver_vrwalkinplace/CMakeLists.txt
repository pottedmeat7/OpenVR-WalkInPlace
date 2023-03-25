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
## include_directories ("../third-party/boost_1_65_1/")
## include_directories ("../third-party/easylogging++/")
include_directories ("../lib_vrwalkinplace/include/")

set(SOURCES
	src/driver_vrwalkinplace.cpp
	src/com/shm/driver_ipc_shm.cpp 
	src/virtualdevice/VirtualController.cpp 
	src/driver/ServerDriver.cpp 
	src/driver/WatchdogProvider.cpp
	)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)


find_package(Boost 1.73 REQUIRED ALL)

##find_package(Boost 1.74 ) ##COMPONENTS date_time interprocess variant regex string message_queue posix_time_types REQUIRED)

include_directories(DriverWalkInPlace ${Boost_INCLUDE_DIRS})

include_directories ("include")


add_library(DriverWalkInPlace SHARED ${SOURCES})


target_link_libraries(DriverWalkInPlace ${Boost_LIBRARIES})
	

