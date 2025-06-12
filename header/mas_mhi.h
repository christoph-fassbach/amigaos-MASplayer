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

#ifndef MAS_MHI_H
#define MAS_MHI_H

/* Activate / De-activate this define to toggle lib base mode! */
#define BASE_GLOBAL /**/

#ifndef BASE_GLOBAL 
#ifndef NO_BASE_REDEFINE
/* 
 * either this is active for everything except libinit.c
 * or BASE_GLOBAL is active everywhere
 */
#define BASE_REDEFINE
#endif
#endif

#include "library.h"

struct MASplayer_MHI {

  struct BaseLibrary  mpb_Library;
  struct ExecBase   * mpb_SysBase;
  struct DosLibrary * mpb_DOSBase;
};

struct MASplayer_MHI_Handle {

  APTR data;            /* current buffer node         */
  APTR endnode;         /* node at the end of the list */
  struct Task *task;    /* task that wants signals     */
  ULONG mhisignal;      /* signal mask to use          */
  UBYTE status;         /* player status flags         */
  UBYTE  oldstatus;     /* last status                 */

  UBYTE volume;
  UBYTE panning;
  UBYTE mixing;
  UBYTE prefactor;
  UBYTE bass;
  UBYTE treble;
  UBYTE mid;
};

struct MPBufferNode {

  struct MinNode  my_MinNode;
  APTR  buffer;
  APTR  pos;
  ULONG  bytesleft;
};

/*
 * Structure which will be used to hold all relevant information about
 * the cia timer we manage to allocate.
 */
struct freetimer {

  struct Library *ciabase;        /* CIA Library Base             */
  ULONG  timerbit;                /* timer bit allocated          */
  struct CIA *cia;                /* ptr to hardware              */
  UBYTE *ciacr;                   /* ptr to control register      */
  UBYTE *cialo;                   /* ptr to low byte of timer     */
  UBYTE *ciahi;                   /* ptr to high byte of timer    */
  struct Interrupt timerint;      /* Interrupt structure          */
  UBYTE  stopmask;                /* Stop/set-up timer            */
  UBYTE  startmask;               /* Start timer                  */
};

/*
 * All libraries' base pointers used by the MHI driver library.
 * Also used to switch between relying on globals or not.
 */
#if defined(BASE_GLOBAL)
  extern struct MASplayer_MHI     * MASplayer_MHI_Base;
  extern struct DosLibrary        * DOSBase;
  extern struct ExecBase          * SysBase;
#elif defined(BASE_REDEFINE)
  #define MASplayer_MHI_Base        (base)
  #define DOSBase                   base->mpb_DOSBase
  #define SysBase                   base->mpb_SysBase
#endif

#endif /* MAS_MHI_H */
