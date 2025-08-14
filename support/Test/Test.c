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

#include <stdio.h>

#include <dos/dos.h>

#include <exec/libraries.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/mhi.h>

#include <libraries/mhi.h>

extern struct ExecBase *SysBase;
extern struct DOSLibrary *DOSBase;

#define NUMBUFS	7
#define BUFSIZE	10000
						//12800
						//204800

struct Library *MHIBase;

int main(int argc, char *argv[])
{
	char temp;
	char *string;
	struct Task *mytask;
	BYTE	mysignal;
	ULONG	sigmask, tempsigs;
	APTR 	handle, handle2;
	FILE 	*fp;
	APTR	bufmem[NUMBUFS+1];
	ULONG	loaded[NUMBUFS+1];
	int	i, bufsleft;
	BOOL	flag;
	APTR	usedbuf;
	int	usedbufnum;
	
	mytask = FindTask(0);
	printf("My task is at: %ld\n",mytask);

	if (-1 == (mysignal = AllocSignal(-1)))
	{
		printf("No signals available! Crazy man!\n");
		return 20;
	}

	sigmask = 1L << mysignal;
	printf("My signal mask: %ld\n",sigmask);

	if (MHIBase = (struct Library *) OpenLibrary("mhi/mhimaspro.library",0))
	{
		printf("Allocating MHI\n");
		handle = MHIAllocDecoder(mytask, sigmask);
		if (handle)
			printf("Handle allocated at: %ld\n",handle);
		else
			printf("Allocation failed, returned %ld\n",handle);

		printf("Allocating MHI second time\n");
		handle2 = MHIAllocDecoder(mytask, 0L);
		if (handle2)
			printf("Handle allocated at: %ld\n",handle2);
		else
			printf("Allocation failed, returned %ld\n",handle2);
			
		printf("\nDecoder details:\n");
		printf("Name:    %s\n", (char*)MHIQuery(MHIQ_DECODER_NAME));
		printf("Version: %s\n", (char*)MHIQuery(MHIQ_DECODER_VERSION));
		printf("Author:  %s\n", (char*)MHIQuery(MHIQ_AUTHOR));

		printf("\nQuerying decoder abilities:\n");
		printf("MIME types: 			  ");
		if((string = (char*)MHIQuery(MHIQ_CAPABILITIES))) {
			printf("%s\n", string);
		}
		else {
			printf("not specified\n");
		}

		printf("Supported MPEG versions:  ");
		if ( MHIQuery(MHIQ_MPEG1) )
			printf("MPEG-1 ");
		if ( MHIQuery(MHIQ_MPEG2) )
			printf("MPEG-2 ");
		if ( MHIQuery(MHIQ_MPEG25) )
			printf("MPEG-2.5 ");
		if ( MHIQuery(MHIQ_MPEG4) )
			printf("MPEG-4 ");
		printf("\n");

		printf("Supported audio layers:   ");
		if (MHIQuery(MHIQ_LAYER1) )
			printf("Layer-1 ");
		if (MHIQuery(MHIQ_LAYER2) )
			printf("Layer-2 ");		
		if (MHIQuery(MHIQ_LAYER3) )
			printf("Layer-3 ");
		printf("\n");
		
		printf("Variable bitrate streams: ");
		if ( MHIQuery(MHIQ_VARIABLE_BITRATE) )
			printf("Yes\n");
		else
			printf("No\n");
			
		printf("Joint stereo encoding:    ");
		if ( MHIQuery(MHIQ_JOINT_STEREO) )
			printf("Yes\n");
		else
			printf("No\n");
		
		printf("Bass control:             ");
		if ( MHIQuery(MHIQ_BASS_CONTROL) )
			printf("Yes\n");
		else
			printf("No\n");		

		printf("Treble control:           ");
		if ( MHIQuery(MHIQ_TREBLE_CONTROL) )
			printf("Yes\n");
		else
			printf("No\n");

		printf("Mid control:              ");
		if ( MHIQuery(MHIQ_MID_CONTROL) )
			printf("Yes\n");
		else
			printf("No\n");

		printf("5-band equalizer:         ");
		if ( MHIQuery(MHIQ_5_BAND_EQ) )
			printf("Yes\n");
		else
			printf("No\n");

		printf("10-band equalizer:        ");
		if ( MHIQuery(MHIQ_10_BAND_EQ) )
			printf("Yes\n");
		else
			printf("No\n");

		if (argc != 0)
		{
			printf("\nMHI Status: %ld", MHIGetStatus(handle));

			printf("\nBuffering MPEG audio stream..\n");
		
			flag = FALSE;
			for (i=0; i<=NUMBUFS; i++)
			{
				bufmem[i] = AllocMem(BUFSIZE, MEMF_CLEAR);
				if (bufmem[i] == 0)
					flag = TRUE;
			}

			if (flag == TRUE)
				printf("Could not allocate memory!\n");
			else
			{
				if ( (fp = fopen(argv[1], "r")) == 0)
					printf("Could not lock file!\n");
				else
				{
					for (i=0; i<=NUMBUFS; i++)
						loaded[i] = fread(bufmem[i], 1, BUFSIZE, fp);

/*
					for (i=0; i<=NUMBUFS; i++)
					{
						printf ("Buffer %ld = %ld\n", i, bufmem[i]);
						printf ("Loaded %ld = %ld\n", i, loaded[i]);
					}
*/
				
					for (i=0; i<=NUMBUFS; i++)
						if (loaded[i] != 0)
							MHIQueueBuffer(handle, bufmem[i], loaded[i]);
				}
			}

			printf("Playing MPEG stream..\n");
			MHIPlay(handle);


//			printf("Waiting 1 sec..\n");
			Delay(50);
			
			MHISetParam(handle, MHIP_PANNING, 70);
			
//			printf("Pausing for 1 sec..\n");
//			MHIPause(handle);
//			printf("Setting Bass..\n");
//			MHISetBass(handle, 50);
//			MHISetTreble(handle, 50);
//			MHISetPrefactor(handle, 50);
//			Delay(25);
//			MHISetVolume(handle, 50);
//			Delay(50);
//			printf("Playing to end..\n");
//			MHIPlay(handle);

		

			flag = FALSE;

			while (flag == FALSE)
			{
				tempsigs = Wait(sigmask | SIGBREAKF_CTRL_C);
//				printf("Got back MHI signal!\n");
				if (tempsigs == SIGBREAKF_CTRL_C)
				{
					printf("Got CTRL-C!\n");
					flag = TRUE;
				}
				else
				{
					while (usedbuf = MHIGetEmpty(handle))
					{
//						printf("MHIGetEmpty = %ld\n", usedbuf);
						usedbufnum = -1;
						for (i=0; i<=NUMBUFS; i++)
						{
							if (usedbuf == bufmem[i])
							{
								usedbufnum = i;
//								printf("Got back empty buf %ld\n", usedbufnum);
							}
						}
						if (usedbufnum != -1)
						{
							loaded[usedbufnum] = fread(bufmem[usedbufnum], 1, BUFSIZE, fp);
//							printf ("Buffer %ld = %ld\n", usedbufnum, bufmem[usedbufnum]);
//							printf ("Loaded %ld = %ld\n", usedbufnum, loaded[usedbufnum]);
							if (loaded[usedbufnum] != 0)
							{
								MHIQueueBuffer(handle, bufmem[usedbufnum], loaded[usedbufnum]);
//								printf("Loaded buffer!\n");
							}
							else
							{
								flag = TRUE;
//								printf("No buffer loaded\n");
							}
						}
					}
				}
			}
		}

		printf("End of file reached, waiting for audio to end.\n");
		
		while((MHIGetStatus(handle) == MHIF_PLAYING) && (tempsigs != SIGBREAKF_CTRL_C))
		{
			printf("Status: %ld\n", MHIGetStatus(handle));
			tempsigs = Wait(sigmask | SIGBREAKF_CTRL_C);
		}
					
		printf("Stream finished. Last buffer decoded.\n");


//		printf("\nEnter to quit");
//		temp = getchar();

		printf("Freeing first handle\n");
		MHIFreeDecoder(handle);

		for (i=0; i<=NUMBUFS; i++)
			if (bufmem[i] != 0)
				FreeMem(bufmem[i], BUFSIZE);
			
		CloseLibrary((struct Library *) MHIBase);
		
		FreeSignal(mysignal);
	}
	else
		printf("Unable to open \"mhimaspro.library\".");
}

