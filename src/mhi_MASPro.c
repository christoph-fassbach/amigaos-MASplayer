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
/* MHI DCR MAS-Player library */

// Yes, we want to use the pragmas/cia_pragmas.h for SAS/C.
#define __USE_CIA_STUBS

#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/cia.h>
#include <proto/misc.h>
#include <proto/dos.h>

#include <exec/libraries.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include <resources/cia.h>
#include <resources/misc.h>

#include "libraries/mhi.h"

#include "debug.h"
#include "mas_mhi.h"
#include "mas3507.h"
#include "mas_interrupts.h"


/* volume
 * first is loudest
 * 21 total
 */
const ULONG voltab[21] = 
	{0x80000,0x8DEB8,0x9A537,0xA5621,0xAF3CD,0xB8053,0xBFD92,0xC6D31,
	 0xCD0AD,0xD2958,0xD785E,0xDBECC,0xDFD91,0xE3583,0xE675F,0xE93CF,
	 0xEBB6A,0xEDEB6,0xEFE2C,0xF1A36,0xFFFFA};

/* prefactor
 * first is quietest
 * 16 total
 */

const UWORD preftab[16] = 
	{0x8000,0x8E00,0x9A40,0xA580,0xAF40,0xB800,0xBFC0,0xC6C0,
	 0xCD00,0xD25C,0xD780,0xDC00,0xDFC0,0xE340,0xE680,0xE940};

/* bass
 * 0x0000 (15) is normal
 * 31 total
 */
 
const UWORD basstab[31] = 
	{0x9E40,0xA280,0xA740,0xAC40,0xB180,0xB740,0xBD40,0xC3C0,
	 0xCA40,0xD180,0xD8C0,0xE040,0xE800,0xEFC0,0xF7C0,0x0000,
	 0x0800,0x1000,0x17C0,0x1F80,0x2700,0x2E40,0x3580,0x3C00,
	 0x4280,0x4880,0x4E40,0x5380,0x5880,0x5D40,0x6180};

/* treble
 * 0x0000 (15) is normal
 * 31 total
 */

const UWORD trebtab[31] = 
	{0xB2C0,0xBB40,0xC180,0xC6C0,0xCBC0,0xD040,0xD500,0xD980,
	 0xDE00,0xE280,0xE7E0,0xEC00,0xF0C0,0xF5C0,0xFAC0,0x0000,
	 0x0540,0x0AC0,0x1040,0x1600,0x1C00,0x2200,0x2840,0x2EC0,
	 0x3540,0x3C00,0x42C0,0x49C0,0x5180,0x5840,0x5F80};

/* Prototypes */
VOID StartTimer(struct freetimer *ft);
BOOL FindFreeTimer(struct freetimer *ft, int preferA);
BOOL TryTimer(struct freetimer *ft);

struct MinList BufList;

BOOL mhiallocated;

UBYTE *owner = NULL;
struct Library *MiscBase = NULL;

/*
 * CIA stuff
 *
 */

#define HICOUNT 0x0c
#define LOCOUNT 0x00

#define STOPA_AND  CIACRAF_TODIN | CIACRAF_PBON | CIACRAF_OUTMODE | CIACRAF_SPMODE
#define STOPB_AND  CIACRBF_ALARM | CIACRBF_PBON | CIACRBF_OUTMODE

#define STARTA_OR  CIACRAF_START
#define STARTB_OR  CIACRBF_START


struct CIA *ciaa = (struct CIA *)0xbfe001;
struct CIA *ciab = (struct CIA *)0xbfd000;

struct freetimer ft;

/* for mas3507 timing */

struct LibBase *GFXBase;

/* --------------------------------------------------------------------- */
/*                             Start of code                             */
/* --------------------------------------------------------------------- */

ASM( APTR ) SAVEDS MHIAllocDecoder(
  REG( a0, struct Task * task ),
  REG( d0, ULONG signal ),
  REG( a6, struct MASplayer_MHI * base ))
/* Allocate MHI. Open all needed resources etc, and be prepared
	to output.

	*task is the task that wants signals
	signal is the signal mask to use
*/
{
	struct MASplayer_MHI_Handle *handle;
	LOG_D(("MHIAllocDecoder...\n"));
	if( !( GFXBase = (struct LibBase *) OpenLibrary("graphics.library", 0)) )
		return 0L;		/* ech! */
	LOG_D(("MHIAllocDecoder: gfx opened at 0x%08lx.\n", GFXBase));
	if(!mhiallocated) {
		if( handle = AllocMem(sizeof(struct MASplayer_MHI_Handle), MEMF_CLEAR) )
		{
			handle->task = task;
			handle->mhisignal = signal;
			handle->status = MHIF_STOPPED;

			handle->volume		= 100;
			handle->panning	= 50;
			handle->mixing		= 50;
			handle->prefactor	= 50;
			handle->bass		= 50;
			handle->treble		= 50;
			handle->mid			= 50;

			if( MiscBase = (struct Library *)OpenResource(MISCNAME) )
			{
				if( ((owner = AllocMiscResource(MR_PARALLELPORT, "MHI")) == NULL)
				&&	  ((owner = AllocMiscResource(MR_PARALLELBITS, "MHI")) == NULL) )
				{
					LOG_D(("MHIAllocDecoder: SetupParPort()\n"));
					SetupParPort(base);	// set par port bits

					LOG_D(("MHIAllocDecoder: SetVolume()\n"));
					SetVolume(voltab[0], voltab[0], base);
					LOG_D(("MHIAllocDecoder: SetPrefactor()\n"));
					SetPrefactor(preftab[0], base);
					LOG_D(("MHIAllocDecoder: SetBass()\n"));
					SetBass(basstab[15], base);
					LOG_D(("MHIAllocDecoder: SetTreble()\n"));
					SetTreble(trebtab[15], base);

					ft.timerint.is_Node.ln_Type 	= NT_INTERRUPT;
					ft.timerint.is_Node.ln_Pri 	= 0;
					ft.timerint.is_Node.ln_Name 	= "MHI_MAS_Player_Pro";
					ft.timerint.is_Data 				= handle;
					ft.timerint.is_Code 				= (VOID (*)()) dcr_pro_int;

					LOG_D(("MHIAllocDecoder: FindFreeTimer()\n"));
					if(FindFreeTimer(&ft,TRUE))
					{
						LOG_D(("MHIAllocDecoder: StartTimer()\n"));
						StartTimer(&ft);

						/* setup main list */
						BufList.mlh_Head		= (struct MinNode *) &BufList.mlh_Tail;
						BufList.mlh_Tail		= 0;
						BufList.mlh_TailPred	= (struct MinNode *) &BufList.mlh_Head;
						mhiallocated = TRUE;
						LOG_D(("MHIAllocDecoder: return handle\n"));
						return handle;
					}
				}
			}
		}
	}

	LOG_D(("MHIAllocDecoder: return NULL\n"));
	return 0L;
}

ASM( VOID ) SAVEDS MHIFreeDecoder(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a6, struct MASplayer_MHI * base ))
/* Free MHI and all resources
*/
{
	APTR killednode;

	if(handle)
	{
		RemICRVector(ft.ciabase,ft.timerbit,&ft.timerint);
		
		FreeMem(handle, sizeof(struct MASplayer_MHI_Handle));
		
		FreeMiscResource(MR_PARALLELPORT);
		FreeMiscResource(MR_PARALLELBITS);
		
		/* free buffer list */
		while ( killednode = RemHead((struct List *)&BufList) )
			FreeMem(killednode, sizeof(struct MPBufferNode));
			
		mhiallocated = FALSE;

		CloseLibrary((struct Library *) GFXBase);
	}
}

ASM( BOOL ) SAVEDS MHIQueueBuffer(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a0, APTR buffer ),
  REG( d0, ULONG size),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Add this buffer to the queue
	*/
	struct MPBufferNode *newnode;

	LOG_D(("MHIQueueBuffer(size=%ld)\n", size));

	if(!( newnode = AllocMem(sizeof(struct MPBufferNode), MEMF_CLEAR))) {
		return FALSE;
	}
	else {
		newnode->buffer 		= buffer;
		newnode->pos			= buffer;
		newnode->bytesleft 	= size;
		AddTail((struct List *)&BufList,(struct Node *)newnode);
		handle->endnode = newnode;
		
		if(handle->data == 0) handle->data = newnode;
			
		return TRUE;
	}
}

ASM( APTR ) SAVEDS MHIGetEmpty(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Find the next empty buffer node and return the buffers address
	*/
	struct MPBufferNode *mynode;
	APTR temp = 0;
	
	mynode = (struct MPBufferNode *)BufList.mlh_Head;
	
	for (mynode = (struct MPBufferNode *)BufList.mlh_Head ; mynode->my_MinNode.mln_Succ ; mynode = (struct MPBufferNode *)mynode->my_MinNode.mln_Succ)
		if(mynode->bytesleft == 0)
			temp = mynode;
			
	mynode = temp;
	
	if((APTR)mynode != &BufList && mynode != 0) {
		Remove((struct Node *)mynode);
		temp = mynode->buffer;
		FreeMem(mynode, sizeof(struct MPBufferNode));
		return temp;
	}
	else {
		return NULL;
	}
}

ASM( UBYTE ) SAVEDS MHIGetStatus(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Get the MHI players status flags
	*/
	return handle->status;
}

ASM( VOID ) SAVEDS MHIPlay(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Set the player to play mode
	*/
	handle->status = MHIF_PLAYING;
}

ASM( VOID ) SAVEDS MHIStop(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Stop the player and free all buffers
	*/
	APTR killednode;

	handle->status = MHIF_STOPPED;
	handle->data = 0;
	/* free buffer list */
	while ( killednode = RemHead((struct List *)&BufList) )
		FreeMem(killednode, sizeof(struct MPBufferNode));	
}

ASM( VOID ) SAVEDS MHIPause(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Pause the player
	*/
	if(handle->status == MHIF_PAUSED)			/* Unpause */
		handle->status = MHIF_PLAYING;
	else
		if(handle->status == MHIF_PLAYING)		/* Pause */
			handle->status = MHIF_PAUSED;
}

ASM( ULONG ) SAVEDS MHIQuery(
  REG( d1, ULONG query ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Respond to a feature query from an application
	*/
	LOG_D(("MHIQuery(%ld)\n", query));
	switch (query) {
		case MHIQ_MPEG1: case MHIQ_MPEG2: case MHIQ_MPEG25:
			return MHIF_SUPPORTED;
			break;
		
		case MHIQ_MPEG4:
			return MHIF_UNSUPPORTED;
			break;
			
		case MHIQ_LAYER1:
			return MHIF_UNSUPPORTED;
			break;
			
		case MHIQ_LAYER2: case MHIQ_LAYER3:
			return MHIF_SUPPORTED;
			break;
			
		case MHIQ_VARIABLE_BITRATE:
			return MHIF_SUPPORTED;
			break;
		
		//case MHIQ_JOINT_STERIO:
		//	return MHIF_SUPPORTED;
		//	break;
			
		case MHIQ_BASS_CONTROL: case MHIQ_TREBLE_CONTROL: case MHIQ_PREFACTOR_CONTROL:
			return MHIF_SUPPORTED;
			break;
			
		case MHIQ_MID_CONTROL:
			return MHIF_UNSUPPORTED;
			
		case MHIQ_VOLUME_CONTROL: case MHIQ_PANNING_CONTROL:
			return MHIF_SUPPORTED;
			break;

		case MHIQ_CROSSMIXING:
			return MHIF_UNSUPPORTED;
			
		case MHIQ_IS_HARDWARE:
			return MHIF_TRUE;
			break;

		case MHIQ_IS_68K: case MHIQ_IS_PPC:
			return MHIF_FALSE;
			break;
			
		case MHIQ_DECODER_NAME:
			return (ULONG)"MAS Player Pro";
			break;

		case MHIQ_DECODER_VERSION:
			return (ULONG)LIBRARY_IDSTRING;
			break;
			
		case MHIQ_AUTHOR:
			return (ULONG)"Paul Qureshi, Thomas Wenzel, Christoph Fassbach";
			break;
			
		default:
			return MHIF_UNSUPPORTED;
			break;			
	}
}

static void setVolPan(struct MASplayer_MHI_Handle *handle, struct MASplayer_MHI * base) {
	LONG VolL, VolR;
	ULONG MasL, MasR;
	
	VolL = handle->volume;
	if(handle->panning > 50) VolL -= 2*(handle->panning-50);
	VolR = handle->volume;
	if(handle->panning < 50) VolR -= 2*(50-handle->panning);
	
	if(VolL > 100) VolL = 100;
	if(VolL <	0) VolL =	0;
	if(VolR > 100) VolR = 100;
	if(VolR <	0) VolR =	0;
	
	// Volume = 0 .. 100% = 0xFFFFF .. 0x80000
	
	if(VolL == 0) {
		MasL = 0xFFFFF;
	}
	else {
		MasL = 0x80000 + 5000*(100-VolL);
	}
	if(VolR == 0) {
		MasR = 0xFFFFF;
	}
	else {
		MasR = 0x80000 + 5000*(100-VolR);
	}
	
	handle->oldstatus = handle->status;
	handle->status = MHIF_PAUSED;
	SetVolume(MasL, MasR, base);
	handle->status = handle->oldstatus;
}

ASM( VOID ) SAVEDS MHISetParam(
  REG( a3, struct MASplayer_MHI_Handle * handle ),
  REG( d0, UWORD param ),
  REG( d1, ULONG value ),
  REG( a6, struct MASplayer_MHI * base )) {
	/* Set decoder parameter.
	*/
	WORD calc;
	UBYTE r;
	UBYTE l;

	LOG_D(("MHISetParam(handle=0x%08lX, %ld=%ld)\n", handle, param, value));

	switch (param) {
		case MHIP_VOLUME:
			if(value != handle->volume) {
				handle->volume = value;
				#if 1
				setVolPan(handle, base);
				#else
				value = handle->panning;

				if(value > 50) {		// pan to right
					r = (float)(100 - ( handle->volume * ((100-value)*0.02) ) * 0.2);
					l = (float)(100 - handle->volume) * 0.2;
				}
				else {
					if(value < 50) {
						r = (float)(100 - handle->volume) * 0.2;
						l = (float)(100 - (handle->volume * (value*0.02)) * 0.2);
					}
					else {
						l = r = (float)(100 - handle->volume) * 0.2;
					}
				}

				if(r > 20) r = 20;
				if(l > 20) l = 20;
				if(r <  0) r = 0;
				if(l <  0) l = 0;

				handle->oldstatus = handle->status;
				handle->status = MHIF_PAUSED;
				SetVolume(voltab[l], voltab[r]);
				handle->status = handle->oldstatus;
				#endif
			}

			break;
		
		case MHIP_PANNING:
			if(value != handle->panning) {
				handle->panning = value;
				#if 1
				setVolPan(handle, base);
				#else
				if(value > 50) {		// pan to right
					r = (float)(100 - ( handle->volume * ((100-value)*0.02) )) * 0.2;
					l = (float)(100 - handle->volume) * 0.2;
				}
				else {
					if(value < 50) {		// pan to right
						r = (float)(100 - handle->volume) * 0.2;
						l = (float)(100 - (handle->volume * (value*0.02))) * 0.2;
					}
					else {
						l = r = (float)(100 - handle->volume) * 0.2;
					}
				}

				if(r > 20) r = 20;
				if(l > 20) l = 20;
				if(r <  0) r = 0;
				if(l <  0) l = 0;

				handle->oldstatus = handle->status;
				handle->status = MHIF_PAUSED;
				SetVolume(voltab[l], voltab[r]);
				handle->status = handle->oldstatus;
				#endif
			}
			break;

		case MHIP_CROSSMIXING:
			if(value != handle->mixing) {
				handle->mixing = value;
			}
			break;
			
		case MHIP_PREFACTOR:
			if(value != handle->prefactor) {
				handle->prefactor = value;
				calc = value;
				if(calc > 50) {
					calc -= 50;            // only prefactors above 50
					if(calc < 0) calc = 0; // are used by the MAS3507
					calc *= 30;
					calc /= 100;
					if(calc > 15) calc = 15;
					if(calc <  0) calc =  0;
				}
				else {
					calc=0;
				}
				SetPrefactor(preftab[calc], base);
			}
			break;

		case MHIP_BASS:
			if(value != handle->bass) {
				handle->bass = value;
				calc = value;
				calc *= 30;
				calc /= 100;
				if(calc > 30) calc = 30;
				if(calc <  0) calc =  0;
				SetBass(basstab[calc], base);
			}
			break;

		case MHIP_TREBLE:
			if(value != handle->treble) {
				handle->treble = value;
				calc = value;
				calc *= 30;
				calc /= 100;
				if(calc > 30) calc = 30;
				if(calc <  0) calc = 0;
				SetTreble(trebtab[calc], base);
			}
			break;
	}
}

void StartTimer(struct freetimer *ft) {
	register struct CIA *cia;

	cia = ft->cia;

	/* Note that there are differences between control register A,
	 * and B on each CIA (e.g., the TOD alarm bit, and INMODE bits.
	 */

	if(ft->timerbit == CIAICRB_TA) {
		ft->ciacr = &cia->ciacra;       /* control register A   */
		ft->cialo = &cia->ciatalo;      /* low byte counter     */
		ft->ciahi = &cia->ciatahi;      /* high byte counter    */

		ft->stopmask = STOPA_AND;       /* set-up mask values   */
		ft->startmask = STARTA_OR;
	}
	else {
		ft->ciacr = &cia->ciacrb;       /* control register B   */
		ft->cialo = &cia->ciatblo;      /* low byte counter     */
		ft->ciahi = &cia->ciatbhi;      /* high byte counter    */

		ft->stopmask = STOPB_AND;       /* set-up mask values   */
		ft->startmask = STARTB_OR;
	}

	Disable();
	*ft->ciacr &= ft->stopmask;
	Enable();

	/* Clear signal bit - interrupt will signal us later */
	/* SetSignal(0L,1L<<ed->signal);
	*/

	*ft->cialo = LOCOUNT;
	*ft->ciahi = HICOUNT;

	/* Turn on start bit - same bit for both A, and B control regs  */

	Disable();
	*ft->ciacr |= ft->startmask;
	Enable();
}



BOOL FindFreeTimer(struct freetimer *ft, int preferA) {
	//struct CIABase *ciaabase, *ciabbase;
	struct Library *ciaabase, *ciabbase;

	ciaabase = OpenResource(CIAANAME);
	ciabbase = OpenResource(CIABNAME);

	if(preferA) {
		ft->ciabase = ciaabase; /* library address  */
		ft->cia     = ciaa;     /* hardware address */
	}
	else {
		ft->ciabase = ciabbase; /* library address  */
		ft->cia     = ciab;     /* hardware address */
	}

	if(TryTimer(ft)) {
		LOG_D(("MHIAllocDecoder: FindFreeTimer() -> CIA-A\n"));
		return TRUE;
	}

	if(!(preferA)) {
		ft->ciabase = ciaabase; /* library address  */
		ft->cia     = ciaa;     /* hardware address */
	}
	else {
		ft->ciabase = ciabbase; /* library address  */
		ft->cia     = ciab;     /* hardware address */
	}

	if(TryTimer(ft)) {
		LOG_D(("MHIAllocDecoder: FindFreeTimer() -> CIA-B\n"));
		return TRUE ;
	}

	LOG_D(("MHIAllocDecoder: FindFreeTimer() -> FAIL\n"));
	return FALSE ;

}

BOOL TryTimer(struct freetimer *ft)
{

if(!(AddICRVector(ft->ciabase,CIAICRB_TA,&ft->timerint)))
    {
    ft->timerbit = CIAICRB_TA;
    return(TRUE);
    }

if(!(AddICRVector(ft->ciabase,CIAICRB_TB,&ft->timerint)))
    {
    ft->timerbit = CIAICRB_TB;
    return(TRUE);
    }

return(FALSE);
}
