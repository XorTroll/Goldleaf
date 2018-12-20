@echo off

del ExeFs\main
make
copy Output\Goldleaf.nso ExeFs\main
del Output\Goldleaf.nso
pause