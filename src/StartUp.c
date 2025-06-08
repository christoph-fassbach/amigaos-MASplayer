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

/*
** Library startup-code and function table definition
**
** Based on CLib37x by Andreas R. Kleinert
*/

#define __USE_SYSBASE

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>

#include <proto/exec.h>

#include "mhilib.h"
#include "mhi_MASPro.h"

extern ULONG __stdargs L_OpenLibs(struct MHI_LibBase *MhiBase);
extern void  __stdargs L_CloseLibs(struct MHI_LibBase *MhiBase);

struct MHI_LibBase *InitLib(struct ExecBase *sysbase __asm("a6"), BPTR seglist __asm("a0"), struct MHI_LibBase *mpb __asm("d0"));
struct MHI_LibBase *OpenLib(struct MHI_LibBase *MhiBase __asm("a6"));
BPTR CloseLib(struct MHI_LibBase *MhiBase __asm("a6"));
BPTR ExpungeLib(struct MHI_LibBase *mpb __asm("a6"));
ULONG ExtFuncLib(void);


/* ----------------------------------------------------------------------------------------
   ! LibStart:
   !
   ! If someone tries to start a library as an executable, it must return (LONG) -1
   ! as result. That's what we are doing here.
   ---------------------------------------------------------------------------------------- */

LONG LibStart(void) {
	return(-1);
}


/* ----------------------------------------------------------------------------------------
   ! Function and Data Tables:
   !
   ! The function and data tables have been placed here for traditional reasons.
   ! Placing the RomTag structure before (-> LibInit.c) would also be a good idea,
   ! but it depends on whether you would like to keep the "version" stuff separately.
   ---------------------------------------------------------------------------------------- */

extern APTR FuncTab [];
extern struct MyDataInit DataTab;
// extern DataTab; /* DICE fix */
                                  /* Instead you may place ROMTag + Datatab directly, here */
                                  /* (see LibInit.c). This may fix "Installer" version     */
                                  /* checking problems, too - try it.                      */

struct InitTable {                       /* do not change */
	ULONG              LibBaseSize;
	APTR              *FunctionTable;
	struct MyDataInit *DataTable;
	APTR               InitLibTable;
} InitTab = {
	(ULONG)               sizeof(struct MHI_LibBase),
	(APTR              *) &FuncTab[0],
	(struct MyDataInit *) &DataTab,
	(APTR)                InitLib
};

APTR FuncTab [] = {
	OpenLib,
	CloseLib,
	ExpungeLib,
	ExtFuncLib,

	i_MHIAllocDecoder,
	i_MHIFreeDecoder,
	i_MHIQueueBuffer,
	i_MHIGetEmpty,
	i_MHIGetStatus,
	i_MHIPlay,
	i_MHIStop,
	i_MHIPause,
	i_MHIQuery,
	i_MHISetParam,

	(APTR) ((LONG)-1)
};


extern struct MHI_LibBase *MhiBase;

/* ----------------------------------------------------------------------------------------
   ! InitLib:
   !
   ! This one is single-threaded by the Ramlib process. Theoretically you can do, what
   ! you like here, since you have full exclusive control over all the library code and data.
   ! But due to some bugs in Ramlib V37-40, you can easily cause a deadlock when opening
   ! certain libraries here (which open other libraries, that open other libraries, that...)
   !
   ---------------------------------------------------------------------------------------- */

struct MHI_LibBase * InitLib(struct ExecBase *sysbase __asm("a6"), BPTR seglist __asm("a0"), struct MHI_LibBase *mpb __asm("d0")) {
	MhiBase = mpb;
	ULONG negsize, possize, fullsize;
	UBYTE *negptr = (UBYTE *) MhiBase;

	MhiBase->mhi_SysBase = sysbase;
	MhiBase->mhi_SegList = seglist;

	if(L_OpenLibs(MhiBase)) return(MhiBase);

	L_CloseLibs(MhiBase);


	negsize  = MhiBase->mhi_Library.lib_NegSize;
	possize  = MhiBase->mhi_Library.lib_PosSize;
	fullsize = negsize + possize;
	negptr  -= negsize;

	FreeMem(negptr, fullsize);
	return NULL;
}

/* ----------------------------------------------------------------------------------------
   ! OpenLib:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only can bypass this restriction by supplying your own semaphore
   ! mechanism.
   ---------------------------------------------------------------------------------------- */

struct MHI_LibBase * OpenLib(struct MHI_LibBase *MhiBase __asm("a6")) {
	MhiBase->mhi_Library.lib_OpenCnt++;

	MhiBase->mhi_Library.lib_Flags &= ~LIBF_DELEXP;

	return MhiBase;
}

/* ----------------------------------------------------------------------------------------
   ! CloseLib:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only can bypass this restriction by supplying your own semaphore
   ! mechanism.
   ---------------------------------------------------------------------------------------- */

BPTR CloseLib(struct MHI_LibBase *MhiBase __asm("a6")) {
	MhiBase->mhi_Library.lib_OpenCnt--;

	if(!MhiBase->mhi_Library.lib_OpenCnt) {
		if(MhiBase->mhi_Library.lib_Flags & LIBF_DELEXP) {
			return ExpungeLib(MhiBase);
		}
	}

	return NULL;
}

/* ----------------------------------------------------------------------------------------
   ! ExpungeLib:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only could bypass this restriction by supplying your own semaphore
   ! mechanism - but since expunging can't be done twice, one should avoid it here.
   ---------------------------------------------------------------------------------------- */

BPTR ExpungeLib(struct MHI_LibBase *mpb __asm("a6")) {
	struct MHI_LibBase *MhiBase = mpb;
	BPTR seglist;

	if(!MhiBase->mhi_Library.lib_OpenCnt) {
		ULONG negsize, possize, fullsize;
		UBYTE *negptr = (UBYTE *) MhiBase;

		seglist = MhiBase->mhi_SegList;

		Remove((struct Node *)MhiBase);

		L_CloseLibs(MhiBase);

		negsize  = MhiBase->mhi_Library.lib_NegSize;
		possize  = MhiBase->mhi_Library.lib_PosSize;
		fullsize = negsize + possize;
		negptr  -= negsize;

		FreeMem(negptr, fullsize);

		return seglist;
	}

	MhiBase->mhi_Library.lib_Flags |= LIBF_DELEXP;

	return NULL;
}

/* ----------------------------------------------------------------------------------------
   ! ExtFunct:
   !
   ! This one is enclosed within a Forbid/Permit pair by Exec V37-40. Since a Wait() call
   ! would break this Forbid/Permit(), you are not allowed to start any operations that
   ! may cause a Wait() during their processing. It's possible, that future OS versions
   ! won't turn the multi-tasking off, but instead use semaphore protection for this
   ! function.
   !
   ! Currently you only can bypass this restriction by supplying your own semaphore
   ! mechanism - but since this function currently is unused, you should not touch
   ! it, either.
   ---------------------------------------------------------------------------------------- */

ULONG ExtFuncLib(void) {
	return NULL;
}

struct MHI_LibBase *MhiBase = NULL;

