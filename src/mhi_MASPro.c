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

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/cia_protos.h>
#include <clib/misc_protos.h>
#include <clib/dos_protos.h>

#include <exec/libraries.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include <resources/cia.h>
#include <resources/misc.h>
#include <clib/debug_protos.h>

#include <libraries/mhi.h>
#include "mhilib.h"
#include "version.h"

// Comment out to enable debug output:
#define KPrintF(...)


#pragma libbase MHIBase

extern void dcr_pro_int();
extern void SetupParPort();
extern void SetVolume(ULONG l asm("d0"), ULONG r asm("d1"));
extern void SetPrefactor(UWORD prefac asm("d0"));
extern void SetBass(UWORD bass asm("d0"));
extern void SetTreble(UWORD treb asm("d0"));

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

BOOL UserLibInit(struct MHI_LibBase *MhiLibBase) {
	return TRUE;
}

void UserLibCleanup(struct MHI_LibBase *MhiLibBase) {
}

/* --------------------------------------------------------------------- */
/*                             Start of code                             */
/* --------------------------------------------------------------------- */

APTR i_MHIAllocDecoder(struct Task *task asm("a0"), ULONG mhisignal asm("d0"))
/* Allocate MHI. Open all needed resources etc, and be prepared
	to output.

	*task is the task that wants signals
	mhisignal is the signal mask to use
*/
{
	struct MPHandle *handle;
	KPrintF("MHIAllocDecoder...\n");
	if( !( GFXBase = (struct LibBase *) OpenLibrary("graphics.library",0)) )
		return 0L;		/* ech! */
	KPrintF("MHIAllocDecoder: gfx opened.\n");	
	if(!mhiallocated) {
		if( handle = AllocMem(sizeof(struct MPHandle), MEMF_CLEAR) )
		{
			handle->task = task;
			handle->mhisignal = mhisignal;
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
					KPrintF("MHIAllocDecoder: SetupParPort()\n");	
					SetupParPort();	// set par port bits

					KPrintF("MHIAllocDecoder: SetVolume()\n");	
					SetVolume(voltab[0], voltab[0]);
					KPrintF("MHIAllocDecoder: SetPrefactor()\n");	
					SetPrefactor(preftab[0]);
					KPrintF("MHIAllocDecoder: SetBass()\n");	
					SetBass(basstab[15]);
					KPrintF("MHIAllocDecoder: SetTreble()\n");	
					SetTreble(trebtab[15]);

					ft.timerint.is_Node.ln_Type 	= NT_INTERRUPT;
					ft.timerint.is_Node.ln_Pri 	= 0;
					ft.timerint.is_Node.ln_Name 	= "MHI_MAS_Player_Pro";
					ft.timerint.is_Data 				= handle;
					ft.timerint.is_Code 				= dcr_pro_int;

					KPrintF("MHIAllocDecoder: FindFreeTimer()\n");	
					if(FindFreeTimer(&ft,TRUE))
					{
						KPrintF("MHIAllocDecoder: StartTimer()\n");	
						StartTimer(&ft);

						/* setup main list */
						BufList.mlh_Head		= (struct MinNode *) &BufList.mlh_Tail;
						BufList.mlh_Tail		= 0;
						BufList.mlh_TailPred	= (struct MinNode *) &BufList.mlh_Head;
						mhiallocated = TRUE;
						KPrintF("MHIAllocDecoder: return handle\n");
						return handle;
					}
				}
			}
		}
	}

	KPrintF("MHIAllocDecoder: return NULL\n");
	return 0L;
}

VOID i_MHIFreeDecoder(APTR handle asm("a3"))
/* Free MHI and all resources
*/
{
	APTR killednode;

	if(handle)
	{
		RemICRVector(ft.ciabase,ft.timerbit,&ft.timerint);
		
		FreeMem(handle, sizeof(struct MPHandle));
		
		FreeMiscResource(MR_PARALLELPORT);
		FreeMiscResource(MR_PARALLELBITS);
		
		/* free buffer list */
		while ( killednode = RemHead((struct List *)&BufList) )
			FreeMem(killednode, sizeof(struct MPBufferNode));
			
		mhiallocated = FALSE;

		CloseLibrary((struct Library *) GFXBase);
	}
}

BOOL i_MHIQueueBuffer(struct MPHandle *handle asm("a3"), APTR buffer asm("a0"), ULONG size asm("d0")) {
	/* Add this buffer to the queue
	*/
	struct MPBufferNode *newnode;

	KPrintF("MHIQueueBuffer(size=%ld)\n", size);

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

APTR i_MHIGetEmpty(struct MPHandle *handle asm("a3")) {
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

UBYTE i_MHIGetStatus(struct MPHandle *handle asm("a3")) {
	/* Get the MHI players status flags
	*/
	return handle->status;
}

VOID i_MHIPlay(struct MPHandle *handle asm("a3")) {
	/* Set the player to play mode
	*/
	handle->status = MHIF_PLAYING;
}

VOID i_MHIStop(struct MPHandle *handle asm("a3")) {
	/* Stop the player and free all buffers
	*/
	APTR killednode;

	handle->status = MHIF_STOPPED;
	handle->data = 0;
	/* free buffer list */
	while ( killednode = RemHead((struct List *)&BufList) )
		FreeMem(killednode, sizeof(struct MPBufferNode));	
}

VOID i_MHIPause(struct MPHandle *handle asm("a3")) {
	/* Pause the player
	*/
	if(handle->status == MHIF_PAUSED)			/* Unpause */
		handle->status = MHIF_PLAYING;
	else
		if(handle->status == MHIF_PLAYING)		/* Pause */
			handle->status = MHIF_PAUSED;
}

ULONG i_MHIQuery(ULONG query asm("d1")) {
	/* Respond to a feature query from an application
	*/
	KPrintF("MHIQuery(%ld)\n", query);
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
			return (ULONG)IDSTRING;
			break;
			
		case MHIQ_AUTHOR:
			return (ULONG)"Paul Qureshi, Thomas Wenzel";
			break;
			
		default:
			return MHIF_UNSUPPORTED;
			break;			
	}
}

static void setVolPan(struct MPHandle *handle) {
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
	SetVolume(MasL, MasR);
	handle->status = handle->oldstatus;
}

VOID i_MHISetParam(struct MPHandle *handle asm("a3"), UWORD param asm("d0"), ULONG value asm("d1")) {
	/* Set decoder parameter.
	*/
	LONG calc;
	UBYTE r;
	UBYTE l;

	KPrintF("MHISetParam(handle=0x%08lX, %ld=%ld)\n", handle, param, value);

	switch (param) {
		case MHIP_VOLUME:
			if(value != handle->volume) {
				handle->volume = value;
				#if 1
				setVolPan(handle);
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
				setVolPan(handle);
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
				SetPrefactor(preftab[calc]);
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
				SetBass(basstab[calc]);
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
				SetTreble(trebtab[calc]);
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
		KPrintF("MHIAllocDecoder: FindFreeTimer() -> CIA-A\n");	
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
		KPrintF("MHIAllocDecoder: FindFreeTimer() -> CIA-B\n");	
		return TRUE ;
	}

	KPrintF("MHIAllocDecoder: FindFreeTimer() -> FAIL\n"); 
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
