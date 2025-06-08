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

 /************************************************/
/*                                              */
/* MHIplay: A simple example how to play        */
/*          multibuffered data via MHI          */
/*                                              */
/*          Revision 1.0 by Thomas Wenzel       */
/*                                              */
/************************************************/

#include <stdio.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>

#include <proto/mhi.h>
#include <libraries/mhi.h>


#define NUMBUFS	8
#define BUFSIZE	128*128

static char Version[]="\0$VER: MHIplay 1.0 (30.03.2001)\0";

struct Library *MHIBase;

int main(int argc, char *argv[]) {
	char	PC[5] = "-\\|/";
	ULONG Progress;
	BYTE	MHISignal;
	ULONG	MHIMask, Signals;
	APTR 	MHIHandle;
	BPTR	InFile;
	APTR	Buffer[NUMBUFS];
	ULONG	CurrentLen;
	APTR	CurrentBuffer;
	BOOL	MemOk;
	BOOL	Done;
	ULONG ReturnCode;
	long i;

	ReturnCode=0;
	
	if(argc != 3) {
		printf("Usage: MHIplay <driver> <file>\n");
		return(5);
	}

	if(MHIBase = OpenLibrary(argv[1], 0)) {
		MHISignal = AllocSignal(-1);
		if(MHISignal != -1) {
			MHIMask = 1L << MHISignal;
			if(MHIHandle = MHIAllocDecoder(FindTask(NULL), MHIMask)) {
				printf("\nDriver details:\n");
				printf("Name:    %s\n", MHIQuery(MHIQ_DECODER_NAME));
				printf("Version: %s\n", MHIQuery(MHIQ_DECODER_VERSION));
				printf("Author:  %s\n", MHIQuery(MHIQ_AUTHOR));
				printf("\n");

				/************************/
				/* Allocate all buffers */
				/************************/
				MemOk=TRUE;
				for(i=0; i<NUMBUFS; i++) {
					Buffer[i] = AllocVec(BUFSIZE, MEMF_CLEAR);
					if(!Buffer[i]) MemOk=FALSE;
				}
				if(MemOk) {
					if(InFile=Open(argv[2], MODE_OLDFILE)) {

						Done=FALSE;

						/**************/
						/* Preloading */
						/**************/
						printf("Preloading.\n");
						for(i=0; i<NUMBUFS; i++) {
							if(CurrentLen = Read(InFile, Buffer[i], BUFSIZE)) {
								MHIQueueBuffer(MHIHandle, Buffer[i], CurrentLen);
								if(CurrentLen != BUFSIZE) Done=TRUE;
							}
						}

						/******************/
						/* Playback start */
						/******************/
						printf("Starting playback.\n");
						MHIPlay(MHIHandle);
						Progress=0;

						/*************/
						/* Main loop */
						/*************/
						while(!Done) {
							printf("Playing [%c]\r", PC[Progress]);
							flushall();
							Progress ++;
							if(Progress > 3) Progress=0;

							Signals=Wait(MHIMask | SIGBREAKF_CTRL_C);
							if(Signals & SIGBREAKF_CTRL_C) break;

							if(Signals & MHIMask) {
								/* Reload and queue all empty buffers */
								for(i=0; i<NUMBUFS; i++) {
									if(CurrentBuffer = MHIGetEmpty(MHIHandle)) {
										if(CurrentLen = Read(InFile, CurrentBuffer, BUFSIZE)) {
											MHIQueueBuffer(MHIHandle, CurrentBuffer, CurrentLen);
										}
										if(CurrentLen != BUFSIZE) Done=TRUE;
									}
								}

								/* Restart if needed */
								if(MHIGetStatus(MHIHandle) == MHIF_OUT_OF_DATA) MHIPlay(MHIHandle);
							}
						}

						/*************************************/
						/* Wait for all buffers to run empty */
						/*************************************/
						if(!(Signals & SIGBREAKF_CTRL_C)) {
							printf("EOF reached. Waiting for end of stream.\n");
							while((MHIGetStatus(MHIHandle) == MHIF_PLAYING)) {
								Signals=Wait(MHIMask | SIGBREAKF_CTRL_C);
								if(Signals & SIGBREAKF_CTRL_C) break;
							}
						}

						/*****************/
						/* Playback stop */
						/*****************/
						printf("Stopping playback.\n");
						MHIStop(MHIHandle);
						Close(InFile);
					}
					else {
						printf("Can't open file!\n");
					}
				}
				else {
					printf("Out of memory!\n");
					ReturnCode=5;
				}
				/********************/
				/* Free all buffers */
				/********************/
				for(i=0; i<NUMBUFS; i++) {
					if(Buffer[i]) FreeVec(Buffer[i]);
				}
				MHIFreeDecoder(MHIHandle);
			}
			else {
				printf("Can't allocate decoder!\n");
				ReturnCode=5;
			}
			FreeSignal(MHISignal);
		}
		else {
			printf("No signals available! Crazy man!\n");
			ReturnCode=5;
		}
		CloseLibrary(MHIBase);
	}
	else {
		printf("Can't open MHI driver \"%s\"\n", argv[1]);
	}
	return(ReturnCode);
}
