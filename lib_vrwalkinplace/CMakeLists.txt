cmake_minimum_required (VERSION 3.0.0)

project(VRWalkInPlace)

# The version number.
set (VRWalkInPlace_VERSION_MAJOR 1)
set (VRWalkInPlace_VERSION_MINOR 0)
 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "./lib")


include_directories ("../openvr/headers/")
## include_directories ("../third-party/boost_1_65_1/")

include_directories ("include")

add_library(VRWalkInPlace src/vrwalkinplace.cpp)
