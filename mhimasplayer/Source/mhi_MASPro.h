/*
 * Licenced under modified BSD licence.
 *
 * Copyright © Paul Qureshi, Thomas Whenzel and Dirk Conrad. All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *
 * Or, in English:
 *   - You're free to derive any work you like from this, just don't change
 *     the original source.
 *   - Give credit where credit is due
 *   - Don't fob it off as your own work
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef MHI_MASPRO_H
#define MHI_MASPRO_H
extern APTR    i_MHIAllocDecoder (struct Task *task asm("a0"), ULONG mhisignal asm("d0"));
extern VOID    StartTimer        (struct freetimer *ft);
extern BOOL    FindFreeTimer     (struct freetimer *ft, int preferA);
extern BOOL    TryTimer          (struct freetimer *ft);
extern VOID    i_MHIFreeDecoder  (APTR handle asm("a3"));
extern BOOL    i_MHIQueueBuffer  (struct MPHandle *handle asm("a3"), APTR buffer asm("a0"), ULONG size asm("d0"));
extern APTR    i_MHIGetEmpty     (struct MPHandle *handle asm("a3"));
extern UBYTE   i_MHIGetStatus    (struct MPHandle *handle asm("a3"));
extern VOID    i_MHIPlay         (struct MPHandle *handle asm("a3"));
extern VOID    i_MHIStop         (struct MPHandle *handle asm("a3"));
extern VOID    i_MHIPause        (struct MPHandle *handle asm("a3"));
extern ULONG   i_MHIQuery        (ULONG query asm("d1"));
extern VOID    i_MHISetParam     (struct MPHandle *handle asm("a3"), UWORD param asm("d0"), ULONG value asm("d1"));
#endif
