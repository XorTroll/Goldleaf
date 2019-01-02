#!/usr/bin/env bash

# IMPORTANT! Place keys.dat at "BuildTools/keys.dat"!

rm -r ExeFs/main &> /dev/null
make
cp Output/Goldleaf.nso ExeFs/main
rm Output/Goldleaf.nso Output/Goldleaf.nsp
./BuildTools/hacbrewpack-linux -k BuildTools/keys.dat --exefsdir=ExeFs --romfsdir=RomFs --logodir=Logo --controldir=Control --nspdir=Output
mv Output/050032a5cf12e000.nsp Output/Goldleaf.nsp
read -n 1 -s -r -p "Press any key to continue..."$'\n'
