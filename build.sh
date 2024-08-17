#!/bin/sh

if [ $1 = "install" ]; then

	cd package

	mkdir -p ~/.steam/steam/pottedmeat7/OpenVRWalkInPlace/

	rm -rf ~/.steam/steam/vrwalkinplace
	cp -rT ./overlay ~/.steam/steam/vrwalkinplace

	rm -rf ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
	cp -rT ./driver ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace

	~/.steam/steam/steamapps/common/SteamVR/bin/vrpathreg.sh adddriver ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace

	echo "install complete"
elif [ $1 = 'uninstall' ]; then
	~/.steam/steam/steamapps/common/SteamVR/bin/vrpathreg.sh removedriver ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
elif [ $1 = "clean" ]; then

	cd lib_vrwalkinplace 
	make clean

	cd ../driver_vrwalkinplace 
	make clean

	cd ../
	make clean

	echo "clean complete"

else 

	cd lib_vrwalkinplace 
    cmake -DCMAKE_BUILD_TYPE=Release .
	make

	cd ../driver_vrwalkinplace 
    cmake -DCMAKE_BUILD_TYPE=Release .
	make

	cd ../

	QT_PATH=/usr/lib/x86_64-linux-gnu/qt5/
	# QT_PATH=~/Qt/5.15.2/gcc_64/bin/

	PATH=$QT_PATH:$PATH
	export PATH
	qmake -config release
	make

	rm -rf ./package

	python3 ./client_overlay/bin/linuxdeployqt.py ./client_overlay/bin/x64/OpenVR-WalkInPlaceOverlay --qt-base-dir $QT_PATH --qml-scan-dir ./client_overlay/bin/x64/res/qml/ --appdir ./package/overlay/ --verbose=1
	cp $QT_PATH/plugins/imageformats/libqsvg.so ./package/overlay/plugins/imageformats

	cd package

	cp ../openvr/lib/linux64/libopenvr_api.so ./overlay/lib
	cp -r ../client_overlay/bin/x64/res ./overlay
	cp ../client_overlay/bin/x64/manifest.vrmanifest ./overlay
	cp ../client_overlay/bin/x64/qt.conf ./overlay
	cp ../client_overlay/bin/x64/default.desktop ./overlay
	cp ../client_overlay/bin/x64/LICENSE ./overlay
	cp ../client_overlay/bin/x64/default.png ./overlay
	chmod +x ./overlay/OpenVR-WalkInPlaceOverlay

	echo "packaging driver files..."

	mkdir -p ./driver/bin/linux32
	mkdir -p ./driver/bin/linux64
	cp -r ../driver_vrwalkinplace/resources ./driver
	cp ../driver_vrwalkinplace/driver.vrdrivermanifest ./driver
	cp ../driver_vrwalkinplace/lib/libDriverWalkInPlace.so ./driver/bin/linux32/driver_00vrwalkinplace.so
	cp ../driver_vrwalkinplace/lib/libDriverWalkInPlace.so ./driver/bin/linux64/driver_00vrwalkinplace.so

	echo "build complete"
fi
