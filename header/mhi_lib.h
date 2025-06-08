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

#ifndef _INCLUDE_PRAGMA_MHI_LIB_H
#define _INCLUDE_PRAGMA_MHI_LIB_H

#ifndef CLIB_MHI_PROTOS_H
#include <clib/mhi_protos.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma amicall(MHIBase,0x01E,MHIAllocDecoder(a0,d0))
#pragma amicall(MHIBase,0x024,MHIFreeDecoder(a3))
#pragma amicall(MHIBase,0x02A,MHIQueueBuffer(a3,a0,d0))
#pragma amicall(MHIBase,0x030,MHIGetEmpty(a3))
#pragma amicall(MHIBase,0x036,MHIGetStatus(a3))
#pragma amicall(MHIBase,0x03C,MHIPlay(a3))
#pragma amicall(MHIBase,0x042,MHIStop(a3))
#pragma amicall(MHIBase,0x048,MHIPause(a3))
#pragma amicall(MHIBase,0x04E,MHIQuery(d1))
#pragma amicall(MHIBase,0x054,MHISetParam(a3,d0,d1))

#ifdef __cplusplus
}
#endif

#endif	/*  _INCLUDE_PRAGMA_MHI_LIB_H  */
