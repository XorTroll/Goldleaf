@echo off

rem IMPORTANT! Place keys.dat at "BuildTools/keys.dat"!

del ExeFs\main >nul 2>nul
make
copy Output\Goldleaf.nso ExeFs\main
del Output\Goldleaf.nso >nul 2>nul
del Output\Goldleaf.nsp >nul 2>nul
BuildTools\hacbrewpack.exe -k BuildTools\keys.dat --exefsdir=ExeFs --romfsdir=RomFs --nologo --controldir=Control --nspdir=Output
ren Output\050032a5cf12e000.nsp Goldleaf.nsp
pause