cd win64

C:\Qt\Qt5.9.0\5.9\msvc2017_64\bin\windeployqt.exe --dir qtdata --libdir . --plugindir qtdata/plugins --debug  --qmldir res/qml OpenVR-WalkInPlaceOverlay.exe

@REM Debug:
@REM windeployqt.exe --dir qtdata --libdir . --plugindir qtdata/plugins --no-system-d3d-compiler --compiler-runtime --no-opengl-sw --debug  --qmldir res/qml OpenVR-WalkInPlaceOverlay.exe
