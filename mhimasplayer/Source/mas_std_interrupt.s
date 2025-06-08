#a2
#struct MPHandle
#{
#   APTR data;             /* current buffer node           */   4   0
#   APTR endnode;          /* node at the end of the list   */   4   4
#   struct Task *task;     /* task that wants signals       */   4   8
#   ULONG mhisignal;       /* signal mask to use            */   4  12
#   UBYTE status;                                                1  16
#};

#a3
#struct MPBufferNode
#{
#   struct MinNode  my_MinNode;     8    0
#   APTR    buffer;                 4    8
#   APTR    pos;                    4   12
#   ULONG   bytesleft;              4   16
#};

#****************************************************************

	xdef	_dcr_pro_int
_dcr_pro_int:					/* this is the routine which gets executed each time the Timer fires. */
	cmp.b	#0,16(a1)			/* are we in play mode? */
	bne		quick_exit			/* nope, so exit */

	btst.b	#0,0xbfd000			/* check if mas3507 is demanding data */
	beq		quick_exit			/* nope, so exit */

	movem.l	d0-d7/a0-a3,-(a7)   /* save registers */

node_loop:
								/* a pointer to our interupt data is in a1 */
	move.l	(a1),a2				/* interupt data node address */
	beq		quick_exit2			/* no data, so exit */

	move.l	12(a2),a0			/* data position */
	move.l	16(a2),d7			/* bytes left to process */
	beq		next_node			/* if no more data skip to next node */
#	beq		quick_exit

#	move.w	#63,d6				/* byte counter */
	cmp.l	#200,d7
	bge		max
#	bset.b	#1,0xbfe001
	move.l	d7,d6
	move.l	d7,d4
	sub.w	#1,d6
	jmp		byteloop
max:
	move.w	#199,d6
	move.l	#200,d4

byteloop:

	move.b	(a0)+,d0			/* get data */

	move.l	#7,d5

bitloop:

	move.b	d0,d1
	and.b	#0b10000000,d1
	ror.b	#7,d1
	bset.b	#1,d1
	move.b	d1,0xbfe101
	bclr.b	#1,d1
	move.b	d1,0xbfe101
	rol.b	#1,d0

	dbra	d5,bitloop

	dbra	d6,byteloop

#	eori.B	#0x02,0xbfe001

	move.l	a0,12(a2)			/* save address */
#	sub.l	#64,d7				/* subtract bytes from bytesleft */
	sub.l	d4,d7
	bge		no_zero
	move.l	#0,d7
no_zero:
	move.l	d7,16(a2)			/* save it back out */

	move.b	#0,16(a1)

	movem.l	(a7)+,d0-d7/a0-a3	/* restore registers */
#	bclr.b  #1,0xbfe001
	rts

quick_exit:

#	movem.l	(a7)+,d0-d7/a0-a3	/* restore registers */
#	bset.b	#1,0xbfe001			/* led off         */
#	moveq	#0,d0
	rts

quick_exit2:

	movem.l	(a7)+,d0-d7/a0-a3	/* restore registers */
#	bclr.b	#1,0xbfe001			/* led off */
#	moveq	#0,d0
	RTS

next_node:
#	bclr.b	#1,0xbfe001

	move.l	a1,d7				/* save a1 */
	move.l	12(a1),d0			/* get signal mask */
	move.l	8(a1),a1			/* get task address */
	jsr		_LVOSignal(a6)		/* signal task */
	move.l	d7,a1				/* restore a1 */

	move.l	4(a1),d0
	cmp.l	a2,d0				/* is this the last node? */
	beq		end_play			/* it is so exit */

#	bset.b	#1,0xbfe001

	move.l	(a2),(a1)
	beq		end_play
	move.l	(a2),a2				/* get next buffer data */
	move.l	a2,(a1)				/* update data address */
	jmp		node_loop

end_play:

	move.l	#0,(a1)				/* clear data address */
	move.b	#2,16(a1)			/* out of data flag in status */
	bclr.b	#1,0xbfe001
	movem.l	(a7)+,d0-d7/a0-a3
	rts
	
#	end
