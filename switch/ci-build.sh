#!/bin/bash -x

source /etc/profile.d/devkit-env.sh
cd switch
make -j8 || exit 1

# Bundle and prepare for release
mkdir -p switch/TriplaneTurmoil
mv TriplaneTurmoil.nro switch/TriplaneTurmoil/TriplaneTurmoil.nro
cp ../fokker.dks switch/TriplaneTurmoil/
zip -r TriplaneTurmoil-switch.zip switch
