#!/bin/bash

if [ $# -ne 1 ] || ([ $1 != "x86_64" ] && [ $1 != "i686" ]); then
    echo "Missing architecture parameter (x86_64 or i686). Usages:"
    echo "$0 x86_64 # for 64bit build"
    echo "$0 i686 # for 32bit build"
fi

ARCH_SUFFIX=$1
MINGW_PREFFIX="mingw32"

if [ ${ARCH_SUFFIX} == "x86_64" ]; then
    MINGW_PREFFIX="mingw64"
fi

echo "Arch: ${ARCH_SUFFIX}"
echo "Mingw: ${MINGW_PREFFIX}"

make -j8 triplane || exit

cd windows
mkdir -p triplane-turmoil
cp ../triplane.exe ./triplane-turmoil/triplane.exe
cp ../fokker.dks ./triplane-turmoil/fokker.dks
cp /usr/local/sdl-dlls/{SDL2.dll,SDL2_mixer.dll,optional/libxmp.dll} ./triplane-turmoil/

zip -r triplane-turmoil-windows-${ARCH_SUFFIX}.zip ./triplane-turmoil
