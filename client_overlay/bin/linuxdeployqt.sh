
#~/appimagetool-715-x86_64.AppImage ./x64/AppImageDir

cd client_overlay/bin/
python linuxdeployqt.py ./x64/OpenVR-WalkInPlaceOverlay --qt-base-dir ~/Qt/5.15.2/gcc_64/ --qml-scan-dir ./x64/res/qml/ --appimage ../../package/overlay/ --appdir ../../package/overlay/ --verbose=3
cd ../../
