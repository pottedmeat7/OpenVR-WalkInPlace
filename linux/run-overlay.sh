#!/bin/sh
HERE="$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="$HERE/lib:$LD_LIBRARY_PATH"
export QT_FONT_DPI=96
cd "$HERE"
exec "$HERE/OpenVR-WalkInPlaceOverlay" "$@"
