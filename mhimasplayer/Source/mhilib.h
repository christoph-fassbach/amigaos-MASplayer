#ifndef MHILIB_H
#define MHILIB_H

struct MHI_LibBase {
	struct Library mhi_Library;
	BPTR mhi_SegList;
	struct ExecBase *mhi_SysBase;
	struct DosLibrary *mhi_DOSBase;
};

struct __attribute__((__packed__)) MPHandle
{
	APTR data;				/* current buffer node 				*/
	APTR endnode;			/* node at the end of the list 	*/
	struct Task *task;	/* task that wants signals 		*/
	ULONG mhisignal;		/* signal mask to use 				*/
	UBYTE status;			/* player status flags				*/
	UBYTE	oldstatus;		/* last status							*/

	UBYTE volume;
	UBYTE panning;
	UBYTE mixing;
	UBYTE prefactor;
	UBYTE bass;
	UBYTE treble;
	UBYTE mid;
};

struct __attribute__((__packed__)) MPBufferNode
{
	struct MinNode	my_MinNode;
	APTR	buffer;
	APTR	pos;
	ULONG	bytesleft;
};

/*
 * Structure which will be used to hold all relevant information about
 * the cia timer we manage to allocate.
 *
 */

struct __attribute__((__packed__)) freetimer
{
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


#endif

