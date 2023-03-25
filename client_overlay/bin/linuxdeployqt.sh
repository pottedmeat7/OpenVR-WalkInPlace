
#~/appimagetool-715-x86_64.AppImage ./x64/AppImageDir

cd client_overlay/bin/
python linuxdeployqt.py ./x64/OpenVR-WalkInPlaceOverlay --qt-base-dir /usr/lib/x86_64-linux-gnu/qt5/ --qml-scan-dir ./x64/res/qml/ --appimage ../../package/overlay/ --appdir ../../package/overlay/
cd ../../
