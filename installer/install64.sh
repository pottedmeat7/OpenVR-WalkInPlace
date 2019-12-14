
mkdir /opt/OpenVR-WalkInPlace/

cd ../client_overlay/bin/x64/
cp AppRun /opt/OpenVR-WalkInPlace/
cp OpenVR-WalkInPlaceOverlay /opt/OpenVR-WalkInPlace/
cp manifest.vrmanifest /opt/OpenVR-WalkInPlace/
cp qt.conf /opt/OpenVR-WalkInPlace/

mkdir /opt/OpenVR-WalkInPlace/res/
cp -r ./res/* /opt/OpenVR-WalkInPlace/res/

cp ../../../openvr/bin/linux64/libopenvr_api.so /opt/OpenVR-WalkInPlace

cd /opt/OpenVR-WalkInPlace/

~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./AppRun -installmanifest
~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./AppRun -postinstallationstep
