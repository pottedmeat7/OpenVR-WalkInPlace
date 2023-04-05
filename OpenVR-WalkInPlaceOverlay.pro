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
QMAKE_CXXFLAGS += "-fopenmp "
# QMAKE_CXXFLAGS += "-fno-sized-deallocation"

QTPLUGIN += qsvg

QT = \
    core \
    gui \
    widgets \
    quick \
    multimedia \
    svg \
    xml \
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

DISTFILES += \
    client_overlay/bin/linuxdeployqt.py \
    client_overlay/bin/linuxdeployqt.sh \
    client_overlay/bin/logging.conf \
    client_overlay/bin/logging.conf.installer \
    client_overlay/bin/logging.conf.standalone \
    client_overlay/bin/windeployqt.bat \
    client_overlay/bin/x64/LICENSE \
    client_overlay/bin/x64/OpenVR-WalkInPlaceOverlay \
    client_overlay/bin/x64/backup.desktop.txt \
    client_overlay/bin/x64/default.desktop \
    client_overlay/bin/x64/default.png \
    client_overlay/bin/x64/error.log \
    client_overlay/bin/x64/manifest.vrmanifest \
    client_overlay/bin/x64/qt.conf \
    client_overlay/bin/x64/qt_win.conf \
    client_overlay/bin/x64/res/OVRWIP.ico \
    client_overlay/bin/x64/res/qml/DataModelPage.qml \
    client_overlay/bin/x64/res/qml/DisableDevicePage.qml \
    client_overlay/bin/x64/res/qml/GraphPage.qml \
    client_overlay/bin/x64/res/qml/MainConfPage.qml \
    client_overlay/bin/x64/res/qml/MyComboBox.qml \
    client_overlay/bin/x64/res/qml/MyDialogOkCancelPopup.qml \
    client_overlay/bin/x64/res/qml/MyDialogOkPopup.qml \
    client_overlay/bin/x64/res/qml/MyInfoPopup.qml \
    client_overlay/bin/x64/res/qml/MyMainViewPage.qml \
    client_overlay/bin/x64/res/qml/MyPushButton.qml \
    client_overlay/bin/x64/res/qml/MyStackGraphPage.qml \
    client_overlay/bin/x64/res/qml/MyStackViewPage.qml \
    client_overlay/bin/x64/res/qml/MyTab.qml \
    client_overlay/bin/x64/res/qml/MyText.qml \
    client_overlay/bin/x64/res/qml/MyTextField.qml \
    client_overlay/bin/x64/res/qml/MyTimerPopup.qml \
    client_overlay/bin/x64/res/qml/MyToggleButton.qml \
    client_overlay/bin/x64/res/qml/PaceControlPage.qml \
    client_overlay/bin/x64/res/qml/backarrow.svg \
    client_overlay/bin/x64/res/qml/mainwidget.qml \
    client_overlay/bin/x64/res/thumbicon.png \
    client_overlay/bin/x64/res/transparent.png \
    client_overlay/bin/x64/startdesktopmode.bat

