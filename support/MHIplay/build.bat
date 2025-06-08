@echo off
m68k-amigaos-gcc -o MHIplay MHIplay.c -O3 -noixemul -D__USE_NEW_TIMEVAL__
dir MHIplay
