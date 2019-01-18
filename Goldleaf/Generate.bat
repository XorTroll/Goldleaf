@echo off

rem IMPORTANT! Place keys.dat at "BuildTools/keys.dat"!

del ExeFs\main >nul 2>nul
make
copy Output\Goldleaf.nso ExeFs\main
del Output\Goldleaf.nso >nul 2>nul
del Output\Goldleaf.nsp >nul 2>nul
rmdir /s /q Build.program
rmdir /s /q Build.control
rmdir /s /q Build.html
rmdir /s /q Build.meta
rmdir /s /q Build.nsp
BuildTools\hacpack.exe -k BuildTools\keys.dat --type nca --ncatype program --titleid 050032A5CF12E000 --exefsdir=ExeFs --romfsdir=RomFs --logodir=Logo --outdir=Build.program
for %%f in (Build.program\*.nca) do (
set pnca=%%f
set spnca=%%~nxf
goto afterp
)
:afterp
BuildTools\hacpack.exe -k BuildTools\keys.dat --type nca --ncatype control --titleid 050032A5CF12E000 --romfsdir=Control --outdir=Build.control
for %%f in (Build.control\*.nca) do (
set cnca=%%f
set scnca=%%~nxf
goto afterc
)
:afterc
BuildTools\hacpack.exe -k BuildTools\keys.dat --type nca --ncatype manual --titleid 050032A5CF12E000 --romfsdir=OfflineHtml --outdir=Build.html
for %%f in (Build.html\*.nca) do (
set hnca=%%f
set shnca=%%~nxf
goto afterh
)
:afterh
BuildTools\hacpack.exe -k BuildTools\keys.dat --type nca --ncatype meta --titleid 050032A5CF12E000 --titletype application --programnca %pnca% --controlnca %cnca% --htmldocnca %hnca% --outdir=Build.meta
for %%f in (Build.meta\*.nca) do (
set mnca=%%f
set smnca=%%~nxf
goto afterm
)
:afterm
mkdir Build.nsp
copy %pnca% Build.nsp\%spnca%
copy %cnca% Build.nsp\%scnca%
copy %hnca% Build.nsp\%shnca%
copy %mnca% Build.nsp\%smnca%
BuildTools\hacpack.exe -k BuildTools\keys.dat --type nsp --titleid 050032A5CF12E000 --ncadir=Build.nsp --outdir=Output
ren Output\050032a5cf12e000.nsp Goldleaf.nsp
pause