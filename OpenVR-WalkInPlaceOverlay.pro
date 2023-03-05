# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

TARGET = OpenVR-WalkInPlaceOverlay

DESTDIR = $$PWD/client_overlay/bin/x64
OBJECTS_DIR=$$PWD/build/
MOC_DIR=$$PWD/build/

#CONFIG   += c++11

# suppress the default RPATH if you wish
#QMAKE_LFLAGS_RPATH=
# add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
QMAKE_CXXFLAGS += "-fopenmp"

QT = \
    core \
    gui \
    widgets \
    quick \
    multimedia \
    #core5compat \

LIBS = \
    -lrt \
    -L"$$PWD/lib_vrwalkinplace/lib" -lVRWalkInPlace \
    -L"$$PWD/openvr/lib/linux64" -lopenvr_api \
    -larmadillo \
    -lgomp \
    -lmlpack \
    -lboost_system \
    -lboost_filesystem \
    #"/lib/x86_64-linux-gnu/libomp.so.5" \
    #"/usr/lib/x86_64-linux-gnu/libmlpack.so.3" \
    #-L"$$PWD/third-party/boost_1_65_1/stage/lib" -lboost_system \
    #-L"$$PWD/third-party/boost_1_65_1/stage/lib" -lboost_filesystem

HEADERS = \
   $$PWD/client_overlay/src/tabcontrollers/WalkInPlaceTabController.h \
   $$PWD/client_overlay/src/logging.h \
   $$PWD/client_overlay/src/overlaycontroller.h \
   $$PWD/third-party/easylogging++/easylogging++.h

SOURCES = \
   $$PWD/client_overlay/src/tabcontrollers/WalkInPlaceTabController.cpp \
   $$PWD/client_overlay/src/main.cpp \
   $$PWD/client_overlay/src/overlaycontroller.cpp \

INCLUDEPATH = \
    $$PWD/lib_vrwalkinplace/include \
    $$PWD/openvr/headers \
    $$PWD/third-party/easylogging++ \
    #$$PWD/third-party/boost_1_65_1 \


#DEFINES = 

