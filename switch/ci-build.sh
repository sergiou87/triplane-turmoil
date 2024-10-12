#!/bin/bash -x

source /etc/profile.d/devkit-env.sh
cd switch
make -j8 || exit 1

# Bundle and prepare for release
mkdir -p switch/triplane-turmoil
mv triplane-turmoil.nro switch/triplane-turmoil/triplane-turmoil.nro
cp ../data/fokker.dks -d switch/triplane-turmoil/
zip -r triplane-turmoil-switch.zip switch
