#!/bin/sh

if [ $1 = "install" ]; then

	cd package

    install_path=$(~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./overlay/OpenVR-WalkInPlaceOverlay -openvrpath)

	(echo "$install_path" | grep "VRVR") && install_path="${install_path%VRVR*}VR"

	echo "installing to: $install_path"

	#rm -rf ~/.local/share/vrwalkinplace
	#rm -rf ~/.steam/steam/vrwalkinplace

	mkdir -p ~/.steam/steam/vrwalkinplace
	#mkdir -p ~/.steam/steam/pottedmeat7/OpenVRWalkInPlace/
	mkdir -p ~/.local/.config/pottedmeat7/

	cp ./libopenvr_api.so ~/.steam/steam/vrwalkinplace
	cp ./libopenvr_api.so /usr/local/lib/

	cp -r ./overlay/lib ~/.steam/steam/vrwalkinplace/lib
	cp -r ./overlay/qml ~/.steam/steam/vrwalkinplace/qml
	cp -r ./overlay/res ~/.steam/steam/vrwalkinplace/res
	cp -r ./overlay/plugins ~/.steam/steam/vrwalkinplace/plugins

	cp ./overlay/manifest.vrmanifest ~/.steam/steam/vrwalkinplace/manifest.vrmanifest
	cp ./overlay/qt.conf ~/.steam/steam/vrwalkinplace/qt.conf
	cp ./overlay/LICENSE ~/.steam/steam/vrwalkinplace/LICENSE
	cp ./overlay/ovrwip.png ~/.steam/steam/vrwalkinplace/ovrwip.png
	cp ./overlay/OpenVR-WalkInPlaceOverlay ~/.steam/steam/vrwalkinplace/OpenVR-WalkInPlaceOverlay

	#rm -rf "$install_path"/drivers/00vrwalkinplace

	mkdir -p "$install_path"/drivers/00vrwalkinplace
	mkdir -p "$install_path"/drivers/00vrwalkinplace/bin
	mkdir -p "$install_path"/drivers/00vrwalkinplace/bin/linux32
	mkdir -p "$install_path"/drivers/00vrwalkinplace/bin/linux64

	cp ./driver/driver.vrdrivermanifest "$install_path"/drivers/00vrwalkinplace/driver.vrdrivermanifest

	cp -r ./driver/resources "$install_path"/drivers/00vrwalkinplace/resources

	cp ./driver/lib/driver_00vrwalkinplace.so "$install_path"/drivers/00vrwalkinplace/bin/linux32/driver_00vrwalkinplace.so
	cp ./driver/lib/driver_00vrwalkinplace.so "$install_path"/drivers/00vrwalkinplace/bin/linux64/driver_00vrwalkinplace.so

	cd ~/.steam/steam/vrwalkinplace/

	~/.steam/steam/ubuntu12_32/steam-runtime/run.sh "$install_path"/bin/linux64/vrpathreg adddriver "$install_path"/drivers/00vrwalkinplace

	~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./OpenVR-WalkInPlaceOverlay -installmanifest
	~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./OpenVR-WalkInPlaceOverlay -postinstallationstep

	echo "install complete"

elif [ $1 = 'test' ]; then

	echo "test"

elif [ $1 = 'uninstall' ]; then

    install_path=$(~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./package/overlay/OpenVR-WalkInPlaceOverlay -openvrpath)

	(echo "$install_path" | grep "VRVR") && install_path="${install_path%VRVR*}VR"

	echo "uninstalling from: $install_path"

	#LD_LIBRARY_PATH="$install_path"/bin/linux64 "$install_path"/bin/linux64/vrpathreg removedriver  ~/.local/share/Steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
	LD_LIBRARY_PATH="$install_path"/bin/linux64 
	"$install_path"/bin/linux64/vrpathreg removedriver  "$install_path"/drivers/00vrwalkinplace

	rm -rf "$install_path"/drivers/00vrwalkinplace

	rm -rf ~/.steam/steam/vrwalkinplace/

elif [ $1 = "clean" ]; then

	cd lib_vrwalkinplace 
	make clean

	cd ../driver_vrwalkinplace 
	make clean

	cd ../
	qmake clean
	make clean

	rm -rf package

	echo "clean complete"

else 

	cd lib_vrwalkinplace 
        cmake .
	make

	cd ../driver_vrwalkinplace 
        cmake .
	make

	#export LD_LIBRARY_PATH="/lib/x86_64-linux-gnu/"

	cd ../

	PATH=~/Qt/5.15.2/gcc_64/bin/:$PATH
	export PATH
	qmake -config release
	make

	
    ./client_overlay/bin/linuxdeployqt.sh

	mkdir -p package

	cd package

	mkdir -p overlay
	mkdir -p driver

	cp ../openvr/lib/linux64/libopenvr_api.so ./
	cp ../openvr/lib/linux64/libopenvr_api.so ./overlay/

	cp -r ../client_overlay/bin/x64/res ./overlay/res
	#cp -r ../client_overlay/bin/x64/lib ./overlay/lib
	#cp -r ../client_overlay/bin/x64/qml ./overlay/qml
	#cp -r ../client_overlay/bin/x64/plugins ./overlay/plugins

	cp ../client_overlay/bin/x64/manifest.vrmanifest ./overlay/manifest.vrmanifest
	cp ../client_overlay/bin/x64/qt.conf ./overlay/qt.conf
	cp ../client_overlay/bin/x64/default.desktop ./overlay/default.desktop
	cp ../client_overlay/bin/x64/LICENSE ./overlay/LICENSE
	cp ../client_overlay/bin/x64/default.png ./overlay/ovrwip.png
	mv package/overlay/AppRun package/overlay/OpenVR-WalkInPlaceOverlay
	#cp ../client_overlay/bin/x64/OpenVR-WalkInPlaceOverlay ./overlay/OpenVR-WalkInPlaceOverlay

	mkdir -p ./driver/lib

	echo "packaging driver files..."

	cp -r ../driver_vrwalkinplace/resources ./driver/resources
	cp ../driver_vrwalkinplace/driver.vrdrivermanifest ./driver/driver.vrdrivermanifest
	cp ../driver_vrwalkinplace/lib/libDriverWalkInPlace.so ./driver/lib/driver_00vrwalkinplace.so

	echo "build complete"
fi
