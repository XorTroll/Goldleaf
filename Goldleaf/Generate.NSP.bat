@echo off

rem IMPORTANT! Place keys.dat at "BuildTools/keys.dat"!

del Output\050032a5cf12e000.nsp
BuildTools\hacbrewpack.exe -k BuildTools\keys.dat --exefsdir=ExeFs --romfsdir=RomFs --nologo --controldir=Control --nspdir=Output
ren Output\050032a5cf12e000.nsp Goldleaf.nsp
pause