@echo off
cls
m68k-amigaos-gcc -m68020 -O2 -o ..\Libs\MHI\mhimaspro.library LibInit.c StartUp.c mhi_MASPro.c mas3507.s mas_interrupt.s     -nostartfiles -Wno-pointer-to-int-cast -ldebug -DMAS_PRO
m68k-amigaos-gcc -m68020 -O2 -o ..\Libs\MHI\mhimasstd.library LibInit.c StartUp.c mhi_MASPro.c mas3507.s mas_std_interrupt.s -nostartfiles -Wno-pointer-to-int-cast -ldebug -DMAS_STD
dir ..\Libs\MHI\mhimaspro.library ..\Libs\MHI\mhimasstd.library