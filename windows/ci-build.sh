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

pushd ..
make -j8 || exit
popd

mv ../triplane.exe ../triplane-tmp.exe
mkdir -p triplane-turmoil
cp ../triplane-tmp.exe ./triplane-turmoil/triplane.exe
cp ../fokker.dks ./triplane-turmoil/fokker.dks

for F in `ldd ../triplane-tmp.exe | grep ${MINGW_PREFFIX} | cut -d' ' -f3`
do
    cp $F ./triplane-turmoil/
done

zip -r triplane-turmoil-windows-${ARCH_SUFFIX}.zip ./triplane-turmoil
