#!/bin/sh

if [ $1 = "install" ]; then

	cd package

	#rm -rf ~/.local/share/vrwalkinplace

	mkdir -p ~/.local/share/vrwalkinplace
	mkdir -p ~/.local/share/pottedmeat7/OpenVRWalkInPlace/
	mkdir -p ~/.local/.config/pottedmeat7/

	cp ./libopenvr_api.so ~/.local/share/vrwalkinplace

	cp -r ./overlay/lib ~/.local/share/vrwalkinplace/lib
	cp -r ./overlay/qml ~/.local/share/vrwalkinplace/qml
	cp -r ./overlay/res ~/.local/share/vrwalkinplace/res
	cp -r ./overlay/plugins ~/.local/share/vrwalkinplace/plugins

	cp ./overlay/manifest.vrmanifest ~/.local/share/vrwalkinplace/manifest.vrmanifest
	cp ./overlay/qt.conf ~/.local/share/vrwalkinplace/qt.conf
	cp ./overlay/LICENSE ~/.local/share/vrwalkinplace/LICENSE
	cp ./overlay/ovrwip.png ~/.local/share/vrwalkinplace/ovrwip.png
	cp ./overlay/OpenVR-WalkInPlaceOverlay ~/.local/share/vrwalkinplace/OpenVR-WalkInPlaceOverlay

	#rm -rf ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace

	mkdir -p ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
	mkdir -p ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin
	mkdir -p ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux32
	mkdir -p ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux64

	cp ./driver/driver.vrdrivermanifest ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/driver.vrdrivermanifest

	cp -r ./driver/resources ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/resources

	cp ./driver/lib/driver_00vrwalkinplace.so ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux32/driver_00vrwalkinplace.so
	cp ./driver/lib/driver_00vrwalkinplace.so ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux64/driver_00vrwalkinplace.so

	cd ~/.local/share/vrwalkinplace/

	~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ~/.local/share/Steam/steamapps/common/SteamVR/bin/linux64/vrpathreg adddriver ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace

	~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./OpenVR-WalkInPlaceOverlay -installmanifest
	~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./OpenVR-WalkInPlaceOverlay -postinstallationstep

	echo "install complete"
elif [ $1 = 'uninstall' ]; then
	#LD_LIBRARY_PATH=~/.steam/steam/steamapps/common/SteamVR/bin/linux64 ~/.steam/steam/steamapps/common/SteamVR/bin/linux64/vrpathreg removedriver  ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
	LD_LIBRARY_PATH=~/.steam/steam/steamapps/common/SteamVR/bin/linux64 ~/.steam/steam/steamapps/common/SteamVR/bin/linux64/vrpathreg removedriver  ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
elif [ $1 = "clean" ]; then

	cd lib_vrwalkinplace 
	make clean

	cd ../driver_vrwalkinplace 
	make clean

	cd ../
	qmake clean
	make clean

	echo "clean complete"

else 

	cd lib_vrwalkinplace 
        cmake .
	make

	cd ../driver_vrwalkinplace 
        cmake .
	make

	export LD_LIBRARY_PATH="/lib/x86_64-linux-gnu/"

	cd ../
	qmake
	make

        ./client_overlay/bin/linuxdeployqt.sh

	mkdir -p package

	cd package

	mkdir -p overlay
	mkdir -p driver

	cp ../openvr/lib/linux64/libopenvr_api.so ./

	cp -r ../client_overlay/bin/x64/lib ./overlay/lib
	cp -r ../client_overlay/bin/x64/qml ./overlay/qml
	cp -r ../client_overlay/bin/x64/res ./overlay/res
	cp -r ../client_overlay/bin/x64/plugins ./overlay/plugins

	cp ../client_overlay/bin/x64/manifest.vrmanifest ./overlay/manifest.vrmanifest
	cp ../client_overlay/bin/x64/qt.conf ./overlay/qt.conf
	cp ../client_overlay/bin/x64/LICENSE ./overlay/LICENSE
	cp ../client_overlay/bin/x64/default.png ./overlay/ovrwip.png
	cp ../client_overlay/bin/x64/OpenVR-WalkInPlaceOverlay ./overlay/OpenVR-WalkInPlaceOverlay

	mkdir -p ./driver/lib

	cp -r ../driver_vrwalkinplace/resources ./driver/resources
	cp ../driver_vrwalkinplace/driver.vrdrivermanifest ./driver/driver.vrdrivermanifest
	cp ../driver_vrwalkinplace/lib/driver_00vrwalkinplace.so ./driver/lib/

	echo "build complete"
fi
