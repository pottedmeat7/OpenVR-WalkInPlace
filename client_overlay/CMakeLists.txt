cmake_minimum_required (VERSION 3.0.0)

project(OpenVRWalkInPlaceOverlay)

# The version number.
set (OpenVRWalkInPlaceOverlay_VERSION_MAJOR 1)
set (OpenVRWalkInPlaceOverlay_VERSION_MINOR 0)
 
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
 
include_directories ("src/")
include_directories ("src/tabcontrollers/")
include_directories ("../openvr/headers/")
include_directories ("../third-party/boost_1_65_1/")
include_directories ("../third-party/easylogging++/")
include_directories ("../lib_vrwalkinplace/include/")

add_library(openvr_api SHARED IMPORTED)

set_target_properties(openvr_api PROPERTIES
  IMPORTED_LOCATION "../openvr/lib/linux64/openvr_api.so"
)

set(SOURCES
	src/main.cpp 
	src/overlaycontroller.cpp 
	src/tabcontrollers/WalkInPlaceTabController.cpp 
   )

add_executable(OpenVRWalkInPlaceOverlay ${SOURCES})

qt5_use_modules(OpenVRWalkInPlaceOverlay Core Gui Widgets Quick) # This macro depends from Qt version

target_link_libraries(OpenVRWalkInPlaceOverlay openvr_api VRWalkInPlace -lpthread -lm -lrt)





