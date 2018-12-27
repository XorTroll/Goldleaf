#!/bin/bash

if [ -e BuildTools/keys.dat ]
then
	case $OSTYPE in
	darwin*)
		run="hacbrewpack";;
	linux*)
		run="hacbrewpack-linux";;
	msys*)
		read -s -n1 -p "Detected Windows, please use Generate.bat"$'\n'"Press any key to exit"$'\n'
		exit;;
	esac
else
	read -s -n1 -r -p "Please place keys.dat at "BuildTools/keys.dat"!"$'\n'
	exit
fi
	rm -r ExeFs/main &> /dev/null
	make
	cp Output/Goldleaf.nso ExeFs/main
	rm Output/Goldleaf.nso Output/Goldleaf.nsp
	./BuildTools/$run -k BuildTools/keys.dat --exefsdir=ExeFs --romfsdir=RomFs --nologo --controldir=Control --nspdir=Output
	mv Output/050032a5cf12e000.nsp Output/Goldleaf.nsp
	read -s -n1 -r -p "Press any key to continue..."$'\n'
