set path=C:\cygwin\bin;C:\devkitPro\devkitARM\bin;..\tools\mcpad\
echo off
cls

make
pause

::make disasm
make clean

..\tools\bin2mcs\bin2mcs BESNESP00000GAMETEST test.bin test.mcs
