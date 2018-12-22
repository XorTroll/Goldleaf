
# IMPORTANT! Place keys.dat at "BuildTools/keys.dat"!

rm -r ExeFs/main &> /dev/null
make
cp Output/Goldleaf.nso ExeFs/main
rm Output/Goldleaf.nso &> /dev/null
rm Output/Goldleaf.nsp &> /dev/null
./BuildTools/hacbrewpack -k BuildTools/keys.dat --exefsdir=ExeFs --romfsdir=RomFs --nologo --controldir=Control --nspdir=Output
mv Output/050032a5cf12e000.nsp Output/Goldleaf.nsp
read -n1 -r -p "Press any key to continue..." key