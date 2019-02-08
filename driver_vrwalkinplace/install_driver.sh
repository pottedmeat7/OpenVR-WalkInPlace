#!/bin/sh

rm -rf ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux32
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux64
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/win32
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/win64
mkdir ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/osx32

cp ../driver_vrwalkinplace/driver.vrdrivermanifest ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/driver.vrdrivermanifest

cp -r ../driver_vrwalkinplace/resources ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/resources

cp ../driver_vrwalkinplace/lib/libDriverWalkInPlace.so ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux32/
cp ../driver_vrwalkinplace/lib/libDriverWalkInPlace.so ~/.steam/steam/steamapps/common/SteamVR/drivers/00vrwalkinplace/bin/linux64/




