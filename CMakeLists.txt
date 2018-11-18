cmake_minimum_required (VERSION 3.0.0)

project(OpenVRWalkInPlace)

# The version number.
set (OpenVRWalkInPlace_VERSION_MAJOR 4)
set (OpenVRWalkInPlace_VERSION_MINOR 0)

add_subdirectory(lib_vrwalkinplace)
add_subdirectory(client_overlay)
add_subdirectory(driver_vrwalkinplace)


