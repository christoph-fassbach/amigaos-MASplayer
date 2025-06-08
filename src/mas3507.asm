*
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
*
	xdef	_SetupParPort
	xdef	_SetVolume
	xdef	_SetPrefactor
	xdef	_SetBass
	xdef	_SetTreble

	xref	_GFXBase

	section text,code

_SetupParPort:
* Set the parallel port hardware up

	move.l	a6,-(sp)
	move.l	a1,-(sp)

	andi.b  #%11111000,$00bfd200 ; ciab ddra: Set SEL,POUT and BUSY to input.
	move.b  #%00000011,$00bfe101 ; ciaa prb  (Parallel port)
	move.b  #%00010011,$00bfe301 ; ciaa ddrb (Data direction for Parallel port)
	move.b  #%00000011,$00bfe301 ; ciaa ddrb (Data direction for Parallel port)
	move.b  #%00000011,$00bfe101 ; ciaa prb  (Parallel port)

	lea		$00bfe301,a1         ; set up target for Write_IIC_S
	bsr		Write_IIC_S
	move.b	#$3A,d0
	bsr		Write_IIC_D0
	move.b	#$68,d0
	bsr		Write_IIC_D0
	move.b	#$93,d0
	bsr		Write_IIC_D0
	move.b	#$b0,d0
	bsr		Write_IIC_D0
	move.b	#$00,d0
	bsr		Write_IIC_D0
	move.b	#$02,d0
	bsr		Write_IIC_D0
	bsr		Write_IIC_P
	bsr		Write_IIC_S
	move.b	#$3A,d0
	bsr		Write_IIC_D0
	move.b	#$68,d0
	bsr		Write_IIC_D0
	move.b	#$00,d0
	bsr		Write_IIC_D0
	move.b	#$01,d0
	bsr		Write_IIC_D0
	bsr		Write_IIC_P

	move.l	(sp)+,a1
	move.l	(sp)+,a6

	rts

_SetVolume:
* Set the overall volume level
* d0 - volume left
* d1 - volume right

	movem.l	d2-d5/a6,-(sp)

	move.l	d1,d5

	move.l  d0,d2
	move.w  d2,d3
	lsr.w   #8,d3
	move.l  d2,d4
	swap    d4

	lea     $00BFE301,a1
VolumeS_LL:
	bsr     Write_IIC_S
	move.b  #$3A,D0
	bsr     Write_IIC_D0
	move.b  #$68,D0
	bsr     Write_IIC_D0
	move.b  #$B0,D0
	bsr     Write_IIC_D0
	move.b  #$00,D0
	bsr     Write_IIC_D0
	move.b  #$00,D0
	bsr     Write_IIC_D0
	move.b  #$01,D0
	bsr     Write_IIC_D0
	move.b  #$07,D0
	bsr     Write_IIC_D0
	move.b  #$F8,D0
	bsr     Write_IIC_D0
	move.b  D3,D0
	bsr     Write_IIC_D0
	move.b  D2,D0
	bsr     Write_IIC_D0
	move.b  #$00,D0
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
	move.b  #$3A,D0
	bsr     Write_IIC_D0
	move.b  #$68,D0
	bsr     Write_IIC_D0
	move.b  #$B0,D0
	bsr     Write_IIC_D0
	move.b  #$00,D0
	bsr     Write_IIC_D0
	move.b  #$00,D0
	bsr     Write_IIC_D0
	move.b  #$01,D0
	bsr     Write_IIC_D0
	move.b  #$07,D0
	bsr     Write_IIC_D0
	move.b  #$FB,D0
	bsr     Write_IIC_D0
	move.b  D3,D0
	bsr     Write_IIC_D0
	move.b  D2,D0
	bsr     Write_IIC_D0
	move.b  #$00,D0
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
* d0 - prefactor level

	movem.l	d2-d4/a6,-(sp)

	move.w  d0,d2
	move.w  d2,d3
	lsr.w   #8,d3

	lea     $BFE301,A1
	bsr     Write_IIC_S
	move.b  #$3a,d0
	bsr     Write_IIC_D0
	move.b  #$68,d0
	bsr     Write_IIC_D0
	move.b  #$9E,d0
	bsr     Write_IIC_D0
	move.b  #$70,d0
	bsr     Write_IIC_D0
	move.b  d3,d0
	bsr     Write_IIC_D0
	move.b  d2,d0
	bsr     Write_IIC_D0
	bsr     Write_IIC_P

	movem.l	(sp)+,d2-d4/a6

	rts

_SetBass:
* d0 - bass

	movem.l	d2-d4/a6,-(sp)

	move.b	#$b0,d4
	jmp		go

_SetTreble:
* d0 - treble

	movem.l	d2-d4/a6,-(sp)

	move.b	#$f0,d4

go:
	move.w  d0,d2
	move.w  d2,d3
	lsr.w   #8,d3

	lea     $00BFE301,A1
	bsr     Write_IIC_S
	move.b  #$3a,d0
	bsr     Write_IIC_D0
	move.b  #$68,d0
	bsr     Write_IIC_D0
	move.b  #$96,d0
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
	MOVE.B  #%00000011,(a1)
*	BSR     Wait_IIC
	MOVE.B  #%00001011,(a1)
* 	BSR     Wait_IIC
	MOVE.B  #%00001111,(a1)
	RTS

Write_IIC_P:
	BSR     Wait_IIC
	MOVE.B  #%00001111,(a1)
*	BSR     Wait_IIC
 	MOVE.B  #%00001011,(a1)
*	BSR     Wait_IIC
	MOVE.B  #%00000011,(a1)
	RTS

Write_IIC_1:
*	BSR     Wait_IIC
*	MOVE.B  #%00001111,(a1)
	BSR     Wait_IIC
	MOVE.B  #%00000111,(a1)
	BSR     Wait_IIC
	MOVE.B  #%00000011,(a1)
	BSR     Wait_IIC
	MOVE.B  #%00000111,(a1)
*	BSR     Wait_IIC
*	MOVE.B  #%00001111,(a1)
	RTS

Write_IIC_0:
*	BSR     Wait_IIC
*	MOVE.B  #%00001111,(a1)
	BSR     Wait_IIC
	MOVE.B  #%00001111,(a1)
	BSR     Wait_IIC
	MOVE.B  #%00001011,(a1)
	BSR     Wait_IIC
	MOVE.B  #%00001111,(a1)
*	BSR     Wait_IIC
*	MOVE.B  #%00001111,(a1)
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
*	MOVEA.L	(A1),A6
	MOVE.L	_GFXBase,A6
	JSR		-384(A6)            ; VBEAMPOS
	MOVE.L	D0,D1
LAB_0198:
*	MOVEA.L	(A1),A6
	MOVE.L	_GFXBase,A6
	JSR		-384(A6)            ; VBEAMPOS
	CMP.L	D0,D1
	BEQ.S	LAB_0198
	MOVEM.L	(A7)+,D0-D1/A1
	RTS

	end

