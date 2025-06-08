	xdef	_SetupParPort
	xdef	_SetVolume
	xdef	_SetPrefactor
	xdef	_SetBass
	xdef	_SetTreble

	.globl	_GFXBase

_SetupParPort:
#Set the parallel port hardware up

	movel	a6,-(sp)

	andi.b  #0b11111000,0xbfd200 | ciab ddra: Set SEL,POUT and BUSY to input.
	move.b  #0b00000011,0xbfe101 | ciaa prb  (Parallel port)
	move.b  #0b00010011,0xbfe301 | ciaa ddrb (Data direction for Parallel port)
	move.b  #0b00000011,0xbfe301 | ciaa ddrb (Data direction for Parallel port)
	move.b  #0b00000011,0xbfe101 | ciaa prb  (Parallel port)

	lea		0xbfe301,a1
	bsr		Write_IIC_S
	move.b	#0x3A,d0
	bsr		Write_IIC_D0
	move.b	#0x68,d0
	bsr		Write_IIC_D0
	move.b	#0x93,d0
	bsr		Write_IIC_D0
	move.b	#0xb0,d0
	bsr		Write_IIC_D0
	move.b	#0x00,d0
	bsr		Write_IIC_D0
	move.b	#0x02,d0
	bsr		Write_IIC_D0
	bsr		Write_IIC_P
	bsr		Write_IIC_S
	move.b	#0x3A,d0
	bsr		Write_IIC_D0
	move.b	#0x68,d0
	bsr		Write_IIC_D0
	move.b	#0x00,d0
	bsr		Write_IIC_D0
	move.b	#0x01,d0
	bsr		Write_IIC_D0
	bsr		Write_IIC_P

	movel	(sp)+,a6

	rts

_SetVolume:
#Set the overall volume level
#d0 - volume left
#d1 - volume right

	movem.l	d2-d5/a6,-(sp)

	move.l	d1,d5

	move.l  d0,d2
	move.w  d2,d3
	lsr.w   #8,d3
	move.l  d2,d4
	swap    d4

	lea     0xBFE301,a1
VolumeS_LL:
	bsr     Write_IIC_S
	move.b  #0x3A,D0
	bsr     Write_IIC_D0
	move.b  #0x68,D0
	bsr     Write_IIC_D0
	move.b  #0xB0,D0
	bsr     Write_IIC_D0
	move.b  #0x00,D0
	bsr     Write_IIC_D0
	move.b  #0x00,D0
	bsr     Write_IIC_D0
	move.b  #0x01,D0
	bsr     Write_IIC_D0
	move.b  #0x07,D0
	bsr     Write_IIC_D0
	move.b  #0xF8,D0
	bsr     Write_IIC_D0
	move.b  D3,D0
	bsr     Write_IIC_D0
	move.b  D2,D0
	bsr     Write_IIC_D0
	move.b  #0x00,D0
	bsr     Write_IIC_D0
	move.b  D4,D0
	bsr     Write_IIC_D0
	bsr     Write_IIC_P
	bsr     Wait_IIC
	bsr     Wait_IIC
	bsr     Wait_IIC

	move.l  d5,d2
	move.w  d2,d3
	lsr.w   #8,d3
	move.l  d2,d4
	swap    d4

VolumeS_RR:
	bsr     Write_IIC_S
	move.b  #0x3A,D0
	bsr     Write_IIC_D0
	move.b  #0x68,D0
	bsr     Write_IIC_D0
	move.b  #0xB0,D0
	bsr     Write_IIC_D0
	move.b  #0x00,D0
	bsr     Write_IIC_D0
	move.b  #0x00,D0
	bsr     Write_IIC_D0
	move.b  #0x01,D0
	bsr     Write_IIC_D0
	move.b  #0x07,D0
	bsr     Write_IIC_D0
	move.b  #0xFB,D0
	bsr     Write_IIC_D0
	move.b  D3,D0
	bsr     Write_IIC_D0
	move.b  D2,D0
	bsr     Write_IIC_D0
	move.b  #0x00,D0
	bsr     Write_IIC_D0
	move.b  D4,D0
	bsr     Write_IIC_D0
	bsr     Write_IIC_P
	bsr     Wait_IIC
	bsr     Wait_IIC
	bsr     Wait_IIC

	movem.l	(sp)+,d2-d5/a6

	rts

_SetPrefactor:
#d0 - prefactor level

	movem.l	d2-d4/a6,-(sp)

	move.w  d0,d2
	move.w  d2,d3
	lsr.w   #8,d3

	lea     0xBFE301,A1
	bsr     Write_IIC_S
	move.b  #0x3a,d0
	bsr     Write_IIC_D0
	move.b  #0x68,d0
	bsr     Write_IIC_D0
	move.b  #0x9E,d0
	bsr     Write_IIC_D0
	move.b  #0x70,d0
	bsr     Write_IIC_D0
	move.b  d3,d0
	bsr     Write_IIC_D0
	move.b  d2,d0
	bsr     Write_IIC_D0
	bsr     Write_IIC_P

	movem.l	(sp)+,d2-d4/a6

	rts

_SetBass:
#d0 - bass

	movem.l	d2-d4/a6,-(sp)

	move.b	#0xb0,d4
	jmp		go

_SetTreble:
#d0 - treble

	movem.l	d2-d4/a6,-(sp)

	move.b	#0xf0,d4

go:
	move.w  d0,d2
	move.w  d2,d3
	lsr.w   #8,d3

	lea     0xBFE301,A1
	bsr     Write_IIC_S
	move.b  #0x3a,d0
	bsr     Write_IIC_D0
	move.b  #0x68,d0
	bsr     Write_IIC_D0
	move.b  #0x96,d0
	bsr     Write_IIC_D0
	move.b  d4,d0
	bsr     Write_IIC_D0
	move.b  d3,d0
	bsr     Write_IIC_D0
	move.b  d2,d0
	bsr     Write_IIC_D0
	bsr     Write_IIC_P

	movem.l	(sp)+,d2-d4/a6

	rts



Write_IIC_S:
	BSR     Wait_IIC
	MOVE.B  #0b00000011,(a1)
#	BSR     Wait_IIC
	MOVE.B  #0b00001011,(a1)
# 	BSR     Wait_IIC
	MOVE.B  #0b00001111,(a1)
	RTS

Write_IIC_P:
	BSR     Wait_IIC
	MOVE.B  #0b00001111,(a1)
#	BSR     Wait_IIC
 	MOVE.B  #0b00001011,(a1)
#	BSR     Wait_IIC
	MOVE.B  #0b00000011,(a1)
	RTS

Write_IIC_1:
#	BSR     Wait_IIC
#	MOVE.B  #0b00001111,(a1)
	BSR     Wait_IIC
	MOVE.B  #0b00000111,(a1)
	BSR     Wait_IIC
	MOVE.B  #0b00000011,(a1)
	BSR     Wait_IIC
	MOVE.B  #0b00000111,(a1)
#	BSR     Wait_IIC
#	MOVE.B  #0b00001111,(a1)
	RTS

Write_IIC_0:
#	BSR     Wait_IIC
#	MOVE.B  #0b00001111,(a1)
	BSR     Wait_IIC
	MOVE.B  #0b00001111,(a1)
	BSR     Wait_IIC
	MOVE.B  #0b00001011,(a1)
	BSR     Wait_IIC
	MOVE.B  #0b00001111,(a1)
#	BSR     Wait_IIC
#	MOVE.B  #0b00001111,(a1)
	RTS

Write_IIC_D0:
	Moveq.L	#9,D1
Write_Loop:
	Subq.B	#1,D1
	Beq.b	Write_IIC_0
	Rol.B	#1,D0
	Btst	#0,D0
	Beq.b	Write_0
	Bsr.b	Write_IIC_1
	Bra.b	Write_Loop
Write_0:
	Bsr.b	Write_IIC_0
	Bra.b	Write_Loop
 
Wait_IIC:
	MOVEM.L	D0-D1/A1,-(A7)
#	MOVEA.L	(A1),A6
	MOVE.L	_GFXBase,A6
	JSR		-384(A6)
	MOVE.L	D0,D7
LAB_0198:
#	MOVEA.L	(A1),A6
	MOVE.L	_GFXBase,A6
	JSR		-384(A6)
	CMP.L	D0,D7
	BEQ.S	LAB_0198
	MOVEM.L	(A7)+,D0-D1/A1
	RTS

