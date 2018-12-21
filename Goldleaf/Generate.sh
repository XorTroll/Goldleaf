echo IMPORTANT! Place keys.dat at "BuildTools/keys.dat"!

rm -r ExeFs/main
make
cp Output/Goldleaf.nso ExeFs/main
rm Output/Goldleaf.nso
rm Output/Goldleaf.nsp
cd BuildTools
./hacbrewpack -k BuildTools/keys.dat --exefsdir=ExeFs --romfsdir=RomFs --nologo --controldir=Control --nspdir=Output
mv Output/050032a5cf12e000.nsp Output/Goldleaf.nsp