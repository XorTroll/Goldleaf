@echo off
del ExeFs\main
make
copy "Output\Goldleaf.nso" "ExeFs\main"
pause