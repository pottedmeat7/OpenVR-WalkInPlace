# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

TARGET = OpenVR-WalkInPlaceOverlay

DESTDIR = $$PWD/client_overlay/bin/x64
OBJECTS_DIR=$$PWD/build/
MOC_DIR=$$PWD/build/

QT = \
    core \
    gui \
    widgets \
    quick \
    multimedia \
    #core5compat \

LIBS = \
    -L"$$PWD/lib_vrwalkinplace/lib" -lVRWalkInPlace \
    -L"$$PWD/openvr/lib/linux64" -lopenvr_api \
    -larmadillo \
    -lboost_system \
    -lboost_filesystem \

HEADERS = \
   $$PWD/client_overlay/src/tabcontrollers/WalkInPlaceTabController.h \
   $$PWD/client_overlay/src/logging.h \
   $$PWD/client_overlay/src/overlaycontroller.h \

SOURCES = \
   $$PWD/client_overlay/src/tabcontrollers/WalkInPlaceTabController.cpp \
   $$PWD/client_overlay/src/main.cpp \
   $$PWD/client_overlay/src/overlaycontroller.cpp \

INCLUDEPATH = \
    $$PWD/lib_vrwalkinplace/include \
    $$PWD/openvr/headers \

#DEFINES = 

