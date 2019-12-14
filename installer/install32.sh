
mkdir /opt/OpenVR-WalkInPlace/

cd ../client_overlay/bin/x64/
cp AppRun /opt/OpenVR-WalkInPlace/
cp OpenVR-WalkInPlaceOverlay /opt/OpenVR-WalkInPlace/
cp manifest.vrmanifest /opt/OpenVR-WalkInPlace/
cp qt.conf /opt/OpenVR-WalkInPlace/

cp ../../../openvr/bin/linux32/libopenvr_api.so.dbg /opt/OpenVR-WalkInPlace

cd /opt/OpenVR-WalkInPlace/

~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./AppRun -installmanifest
~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./AppRun -postinstallationstep
