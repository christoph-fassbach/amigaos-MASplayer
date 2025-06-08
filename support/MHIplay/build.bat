REM Licenced under modified BSD licence.
REM
REM Copyright © Paul Qureshi, Thomas Whenzel and Dirk Conrad. All rights
REM reserved.
REM 
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted provided that the following conditions are
REM met:
REM
REM 1. Redistributions of source code must retain the above copyright notice,
REM    this list of conditions and the following disclaimer.
REM 2. Redistributions in binary form must reproduce the above copyright
REM    notice, this list of conditions and the following disclaimer in the
REM    documentation and/or other materials provided with the distribution.
REM 3. Neither the name of the authors nor the names of its contributors
REM    may be used to endorse or promote products derived from this software
REM    without specific prior written permission.
REM
REM
REM Or, in English:
REM   - You're free to derive any work you like from this, just don't change
REM     the original source.
REM   - Give credit where credit is due
REM   - Don't fob it off as your own work
REM
REM THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND ANY
REM EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
REM WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
REM DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
REM DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
REM (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
REM SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
REM CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
REM LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
REM OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
REM DAMAGE.

@echo off
m68k-amigaos-gcc -o MHIplay MHIplay.c -O3 -noixemul -D__USE_NEW_TIMEVAL__
dir MHIplay
