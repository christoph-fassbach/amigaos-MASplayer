#!/bin/sh
clear
m68k-amigaos-gcc -m68020 -o ../Libs/MHI/mhimaspro.library LibInit.c StartUp.c mhi_MASPro.c mas3507.s mas_interrupt.s     -nostartfiles -Wno-pointer-to-int-cast -ldebug -DMAS_PRO
m68k-amigaos-gcc -m68020 -o ../Libs/MHI/mhimasstd.library LibInit.c StartUp.c mhi_MASPro.c mas3507.s mas_std_interrupt.s -nostartfiles -Wno-pointer-to-int-cast -ldebug -DMAS_STD
m68k-amigaos-gcc -m68020 -O3 -o test test.c -noixemul -D__USE_NEW_TIMEVAL__
ls -l ../Libs/MHI/mhiMAS*
