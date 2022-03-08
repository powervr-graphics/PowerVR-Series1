;*****************************************************************************
;*++
;*  Name        :   $RCSfile: dispml.asm,v $
;*  Title       :   
;*  ASM Author  :   Rob Isherwood
;*  Created     :   19/11/96
;*
;*  Copyright	: 1995-2022 Imagination Technologies (c)
;*  License		: MIT
;*
;*  Description :   Pentium optimised PackISPCore and PackTo20Bit
;*
;*  Program Type:   ASM module (.dll)
;* 
;*  RCS info:
;*
;*  $Date: 1997/08/22 10:47:00 $
;*  $Revision: 1.18 $
;*  $Locker:  $
;*  $Log: dispml.asm,v $
;; Revision 1.18  1997/08/22  10:47:00  sks
;; Increased PIMAT_SIZE by 4 for nSmoothSpecularColour.
;;
;; Revision 1.17  1997/08/18  16:28:20  gdc
;; chnaged PIMAT_SIZE again
;;
;; Revision 1.16  1997/08/13  16:18:04  gdc
;; chnaged size of material struct - and made asm use a symbol
;; not 64 buried in the code
;;
;; Revision 1.15  1997/03/25  10:17:36  jop
;; Changed c/asm fn names so it would link without too
;; many errors
;;
;; Revision 1.14  1997/01/27  13:50:14  rgi
;; Corrected the offsets in the gPDC structure to match the
;; changes made to dtri.h
;;
;; Revision 1.13  1997/01/16  13:04:08  rgi
;; Moved ProcessD3DTriCore to dtriml.asm module
;;
;; Revision 1.12  1997/01/15  13:22:52  rgi
;; Added fields used by dtexml.asm to the know section of
;; the gPDC structure.
;;
;; Revision 1.11  1996/12/20  09:58:23  gdc
;; removed a lot of ^M's
;;
;; Revision 1.10  1996/12/19  06:14:16  jop
;; Some more stuff on polys
;;
;; Revision 1.9  1996/12/19  00:14:08  jop
;; Started coding the new polygon stuff
;;
;; Revision 1.8  1996/12/11  03:43:21  jop
;; Fixed a bug in the bogus Z code
;;
;; Revision 1.7  1996/12/10  23:58:12  jop
;; Added BogusZ mode to ProcessD3DTriCore. I couldn't detect any
;; loss in performance. In Bogus Z mode there are 2 more branches, 
;; and 1 more in 'normal' mode. These should be predicted correctly
;;
;; Revision 1.6  1996/12/05  20:56:43  jop
;; Fixed perp bug
;;
;; Revision 1.5  1996/12/03  15:55:48  rgi
;; Added ProcessD3DStripClippingCore to the growing list of
;; assembler functions. Not enough time/energy to do the full
;; port of ProcessD3DTriCore technology into had to just move the
;; fDiv a bit for better performance.
;;
;; Revision 1.4  1996/12/03  10:58:57  rgi
;; Added PackISPPolygonCore to the assembler support for Midas4
;;
;; Revision 1.3  1996/12/02  09:36:28  rgi
;; Updated to use the 'new' maths in C version of PackISPCore
;;
;; Revision 1.2  1996/11/22  17:16:12  rgi
;; Added optimized version of ProcessD3DCore in dtri.c
;;
;; Revision 1.1  1996/11/19  11:37:38  rgi
;; Initial revision
;;
;* 
;* 	This was all produced from compiler output of disp.c's version with
;*        the following options.
;* 
;*  -- Machine type P			    
;*  mark_description "Intel Reference	Release 5 Version 2.3.2 Beta B96061";
;*  mark_description "-DWIN32 -QWl, -force:multiple -QWl, -incremental:no -S -Fae:\\sabre\\sgl\\SGLMID4\\release.tmp\\pkisp.asm -DFU";
;*  mark_description "LL_ON -DPVR2 -DPCX1 -DDDRAW -DWIN32 -DICL -DVER_LIB_MAX=3 -DVER_LIB_MA=0 -DVER_LIB_MI=1 -DVER_LIB_MIN=0x0606";
;*  mark_description " -DVER_LIB_TXT=\"3.0.1b6\" -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\. -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\p";
;*  mark_description "cx -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\win32 -Ie:\\sabre\\SGL\\..\\vsgl -Gy -Zp4 -G5 -QIfdiv- -W0 -nologo";
;*  mark_description " -Qpc32 -Qrct -Op- -O2 -Oy -Qip -Qmem -Qscope_alloc -QW0, -mo_alternate_loops, -mo_block_loops, -mo_strip_mine ";
;*  mark_description "-Oi";
;* ident "Intel Reference	Release 5 Version 2.3.2 Beta B96061"
;* ident "-DWIN32 -QWl, -force:multiple -QWl, -incremental:no -S -Fae:\sabre\sgl\SGLMID4\release.tmp\pkisp.asm -DFULL_ON -DPVR2 -DPCX1 -DD"
;*
;*--
;*****************************************************************************
	.386P
	.387
_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'
_TEXT	ENDS
_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
	ALIGN 010H
_DATA	ENDS
_BSS	SEGMENT PARA PUBLIC USE32 'BSS'
	ALIGN 010H
_BSS	ENDS
_DATA1	SEGMENT PARA PUBLIC USE32 'DATA'
	ALIGN 010H
_DATA1	ENDS
_TLS	SEGMENT PARA PUBLIC USE32 'TLS'
_TLS	ENDS
	ASSUME	CS:FLAT, DS:FLAT, SS:FLAT
_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'

;Constants
WORDS_PER_PLANE		EQU	(3*4)
IEEE_MANTISSA_MASK	EQU	07FFFFFh
IEEE_ASSUMED_BIT	EQU	0800000h
IBUFFERSIZE		EQU	32
SGLTT_DISABLEZBUFFER EQU 02000h
D3DTRIFLAG_ODD    equ 0000001eh
D3DTRIFLAG_EVEN   equ 0000001fh

;; Size of Material struct
PIMAT_SIZE	EQU	76

;PITRI structure
Adj0		EQU	0
Adj1		EQU	12
Adj2		EQU	24
f1OverDet	EQU	36
fZ		EQU	40
PITRI_SIZE	EQU	64

;gPDC structure, we only know about stuff upto Context sub-struct
nPolys		EQU	0
pVertices	EQU	4
TSPControlWord	EQU	8
pFace		EQU	12
pV0		EQU	16
pV1		EQU	20
pV2		EQU	24
ShiftRegX	EQU	28
TexAddress	EQU	32
n32MipmapOffset EQU	36
invTexSize	EQU	40
Context		EQU	44

;SGLCONTEXT structure
bFogOn		EQU	0
fFogR		EQU	4
fFogG		EQU	8
fFogB		EQU	12
u32FogDensity	EQU	16
bCullBackfacing	EQU	20 ; (+Context = 64)
u32Flags	EQU	24
nTextureName	EQU	28
fTransPassDepth	EQU	32
bDoClipping	EQU	36
cBackColour	EQU	40 ; 3 floats
eShadowLVolMode EQU	52
fShadowBright	EQU	56
u32LVolColour	EQU	56 ; union with previous
bFlipU		EQU	60
bFlipV		EQU	64
bDoUVTimesInvW	EQU	68
FirstXRegion	EQU	72 ; (+Context = 116)
LastXRegion	EQU	76 ; (+Context = 120)
FirstYRegion	EQU	80 ; (+Context = 124)
LastYRegion	EQU	84 ; (+Context = 128)

;Vertex structure
fX		EQU	0
fY		EQU	4

if 0
; Not quite the biscuit
NPackTo20Bit1	macro
	add	eax, eax			; eax = IE8, IM23, 0 CF=S1
	 mov	ecx, (-102)*001000000h	; ecx = -102, 24*0
endm
NPackTo20Bit2CXc macro Normal, Signed, Negit
	local Zero
	adc	ecx, eax		; ecx = PE=(IE8-102), IM23, S1 CF = (PE >= 0)
	 jc	Normal
	cmp	eax, (87)*001000000h; ecx = (IE8-102), IM23, S1 Flags = (IE8-87)
	 jc	Zero
	ror	ecx, 24		; ecx = IM23, S1, (IE8-102)
	 and	eax, 000fffc00h	; eax = 8*0, PM_13-0, 10*0
	neg	ecx		; ecx = ~IM23, ~S1, neg = -(IE8-102)
	or	eax, 001000000h	; eax = 7*0, PM15, 10*0
	shr	eax, cl		; eax = 7*0, PM15, 10*X
	add	eax, 000000200h	; Round up contents of eax
	and	ecx, 100h	; Flags = (~S1 & 1)
	 jnz	Signed
Negit:
ifdef PVR1
	xor	eax, 003fffc00h	; S1, PM15 = ~(0, 1, IM22-9)
	add	eax, 000000400h  ; S1, PM15 = -(0, 1, IM22-9) 
endif
ifdef PVR2
	add	eax, 002000000h	; eax = 2*0, PE4, 1, PM15, 7*0, S1, 2*0
endif
	jmp	Signed
Zero:	xor	eax, eax
	jmp	Signed
endm

NPackTo20Bit3CX macro Normal
Normal:	and	ecx, 00f000001h	; ecx = 4*0, PE4, 23*0, S1
	 and	eax, 000fffc00h	; eax = 8*0, PM_13-0, 10*0
endm	
NPackTo20Bit4UCX macro
	lea	eax, [eax][ecx*4][001000000h]
				; eax = 2*0, PE4, 0, PM15, 7*0, S1, 2*0
endm

NPackTo20Bit5 macro Negit
	test	eax, 4h
	 jne	Negit
endm

NPackTo20Bit6U macro Signed
Signed:	shr	eax, 10		; eax = 12*0, PE4, S1, PM15 = Pack20Value
endm
endif

;****************************************************************************;
;									     ;
;	PackTo20Bit1-6[U|C] converts eax into Packed version		     ;
;									     ;
;	All macros execute best at a U boundary and macros with U at the end ;
;       are pairable with an additional V instruction placed after the macro.;
;									     ;
;	If C is added at the end of the macro then beware that the Carry     ;
;       flag carries data and must be preserved till the next step.          ;
;									     ;
;	Notation:							     ;
;		S1 = Sign Bit, IE8 = IEEE Exponent			     ;
;		IM23 = IEEE Mantissa, PE4 = Pack 20			     ;
;		exponent, PM15 = Pack 20 Mantissa			     ;
;		IM_n is bit n of IM, n-m is bit slice			     ;
;		CF is carry flag					     ;
;		Normaly PM15 = 1, IM_22-9 and PE4 = IE8-102		     ;
;									     ;
;****************************************************************************;
PackTo20Bit1U	macro
	rol	eax, 9		; eax = IEEE, IM23, S1, IE8
endm
PackTo20Bit2UC	macro
	local LNormal, LFixed, LZero
	add	al, -102		; If (IE8>=102) eax = IM23, S1, XXXX, PE4 CF==1
	jc	LNormal		; If (CF==0) then handle smaller numbers
	push	ecx		; Preserve ecx
	mov	ecx, eax		; ecx = IM23, S1, IE8-102 -> 154 to 255
	add	cl, 16		; cl = 16-neg, Flags = ((IE8-102) - 240)
	jle	LZero		; if ( (256+IE8-102) <= 240 ) too small
	shr	eax, 17		; eax = 17*0, IM_22-8
	add	eax, 8000h	; eax = 16*0, 1, IM_22-8
	shl	eax, cl		; eax = neg*0, 1, IM_22-8, (16-neg)*0
	add	eax, 10000h	; eax = PM15, 17*X
	xor	al, al		; eax = PM15, 9*X, 8*0
	shr	ecx, 9		; CF = S1
	rcr	al, 1		; eax = PM15, 9*X, S1, 7*0 CF=0
	pop	ecx		; Restore ecx
	jmp	LFixed
LZero:	xor	eax, eax		; eax = 0, CF=0
	pop	ecx
	jmp	LFixed
LNormal:
	rcr	eax, 1		; eax = PM15, IM_8-0, S1, XXXX, PE_3-1 CF=PE_0
LFixed:				; LSpecial comes back here
endm
PackTo20Bit3UC	macro
	adc	al, al		; eax = PM15, IM_8-0, XXXX, PE4 CF=S1
endm
PackTo20Bit4U	macro
	adc	al, al		; eax = PM15, IM_8-0, XXX, PE4, S1 CF=X
;ifnb <LVeryBig>
;	jc	LVeryBig      	; If carry then ((IE8-102) >= 128) Big!!
;	cmp	al, (14*2)+1	; Flags = XXX, PE4, S1 - 000, MaxPE4, 1
;	jbe	@F		; If ((IE8-102) <= MaxPE4) Okay!
;LVeryBig:
;	and	al, 1dh		; eax = PM15, IM8_0, 000, XXX0, S1 CF=0
;	or	eax, 07ffe001ch	; eax = 15*1, IM8-0, 000, 1110, S1 CF=0
;@@:
;endif
ifdef PVR1
	test	al, 1		; Flags = Sign Bit - 1
	jnz	@F		; If not signed then ready to go
	xor	eax, 0FFFE0000h	; eax = (~PM15), IM_8-0, XXX, PE4, S CF=0
	add	eax, 000020000h	; eax = (-PM15), IM_8-0, XXX, PE4, S CF=?
@@:
endif
endm
PackTo20Bit5U	macro
	ror	eax, 5		; eax = PE4, S, PM15, IM_8-0, XXX
endm
PackTo20Bit6U	macro
	shr	eax, 12		; eax = Pack20 = 12*0, PE4, S, PM15
endm

;;;;;;;;;;;;;;;;;;;;;;

MinMaxSorted	macro  min, max, new

	local	Finished

	cmp		new, max
	jle		@F
	mov		max, new
	jmp		Finished
@@:	cmp		new, min
	jge		Finished
	mov		min, new

Finished:
endm

MinMaxUnsorted	macro  min, max, new

	local	Finished

	cmp		min, max
	jle		@F
	xchg	min, max
@@:	cmp		new, max
	je		Finished
	jl		@F
	mov		max, new
	jmp		Finished
@@:	cmp		new, min
	jge		Finished
	mov		min, new

Finished:
endm

Constrain macro min, max, lolim, hilim, bugout

	cmp		min, lolim
	jge		@F
	mov		min, lolim
	cmp		max, min
	jl		bugout
@@:	cmp		max, hilim
	jle		@F
	mov		max, hilim
	cmp		min, max
	jg		bugout
@@:

endm

ConstrainB macro min, max, lolim, hilim, bugout

	cmp		min, BYTE PTR lolim
	jge		@F
	mov		min, BYTE PTR lolim
	cmp		max, min
	jl		bugout
@@:	cmp		max, BYTE PTR hilim
	jle		@F
	mov		max, BYTE PTR hilim
	cmp		min, max
	jg		bugout
@@:

endm

ifdef D3DOPTIM
;	COMDAT _PackTo20BitTest
; -- Begin	_PackTo20BitTest
; mark_begin;
	ALIGN	4
	PUBLIC	_PackTo20BitTest
_PackTo20BitTest	PROC NEAR
	mov	eax, [esp][4]
	PackTo20Bit1U
;	xor	edx, edx
	PackTo20Bit2UC
;	mov	edx, ebx
	PackTo20Bit3UC
;	mov	edx, ebx
	PackTo20Bit4U
;	xor	edx, edx
	PackTo20Bit5U
;	xor	edx, edx
	PackTo20Bit6U
;	xor	edx, edx
	ret
	ALIGN	4
; mark_end;
_PackTo20BitTest ENDP
;_PackTo20BitTest	ENDS
endif

;	COMDAT _PackISPCore
; -- Begin	_PackISPCore
; mark_begin;
	LInvMaxV	TEXTEQU <esp+  0>
	LPack20PB	TEXTEQU <esp+  4>
	LPack20PA	TEXTEQU <esp+  8>
	Adjust		TEXTEQU	<12>
	SaveEBX		TEXTEQU <esp+  0+Adjust>
	SaveEBP		TEXTEQU <esp+  4+Adjust>
	SaveESI		TEXTEQU <esp+  8+Adjust>
	SaveEDI		TEXTEQU <esp+ 12+Adjust>
	RetAddr		TEXTEQU <esp+ 16+Adjust>
	pPlaneMem	TEXTEQU	<esp+ 20+Adjust>
	nPolysInChunk	TEXTEQU <esp+ 24+Adjust>
	rTSPAddr	TEXTEQU <esp+ 28+Adjust>
	rpTri		TEXTEQU <esp+ 32+Adjust>
	nIncrement	TEXTEQU <esp+ 36+Adjust>
	TSPIncrement	TEXTEQU <esp+ 40+Adjust>

	ALIGN	4
	PUBLIC	__PackISPCore
__PackISPCore	PROC NEAR
	mov	eax, [esp][4]		; eax = pPlaneMem
	mov	edx, [esp][8]		; edx = nPolysInChunk
	mov	ecx, [esp][12]		; ecx = rTSPAddr
	push	esi			; Preserve additional registers used
	push	edi
	push	ebp
	push	ebx
	mov	esi, [esp][32]		; esi = rpTri
	mov	ebp, edx			; EBP = edx = nPolysInChunk
	mov	edi, eax		; EDI = pPlaneMem
	sub	esp, Adjust		; Allocate space for locals
	mov	ebx, [ecx]		; EBX = TSPAddr = [ecx]
	mov	esi, [esi]		; ESI = pTri = *rpTri
	dec	ebp			; EBP = nPolysInChunk - 1
	js	L_EndWhileLoop		; if ( EDX < 0 ) L_EndWhileLoop
L_MainLoop:
if 0
	fld	DWORD PTR [esi][fZ][0]	; F1A = pTri->fZ[0] *
	fmul	DWORD PTR [esi][Adj0][0];       pTri->fAdjoint[0][0]
	fld	DWORD PTR [esi][fZ][4]	; F1B = pTri->fZ[1] * 
	fmul	DWORD PTR [esi][Adj0][4];       pTri->fAdjoint[0][1]
	fld	DWORD PTR [esi][fZ][8]	; F1C = pTri->fZ[2] * 
	fmul	DWORD PTR [esi][Adj0][8];       pTri->fAdjoint[0][2]
	fxch	st(1)			; F1B, F1C, F1A
	faddp	st(2), st		; F1C, F1B+F1A
	fld	DWORD PTR [esi][fZ][0]	; F1D = pTri->fZ[0] *
	fmul	DWORD PTR [esi][Adj1][0];       pTri->fAdjoint[1][0]
	fxch	st(1)			; F1C, F1D, F1B+F1A
	faddp	st(2), st		; F1D, F1C+F1B+F1A
	fld	DWORD PTR [esi][fZ][4]	; F1E = pTri->fZ[1] *
	fmul	DWORD PTR [esi][Adj1][4];       pTri->fAdjoint[1][1]
	fld	DWORD PTR [esi][fZ][8]	; F1F = pTri->fZ[2] *
	fmul	DWORD PTR [esi][Adj1][8];       pTri->fAdjoint[1][2]
	fxch	st(1)			; F1E, F1F, F1D, F1C+F1B+F1A
	faddp	st(2), st		; F1F, F1E+F1D, F1C+F1B+F1A
	fxch	st(2)			; F1C+F1B+F1A, F1E+F1D, F1F
	fmul	DWORD PTR [esi][f1OverDet]; _fA=(F1C+F1B+F1A)*f1OverDet
	fld	DWORD PTR [esi][fZ][0]	; _fZ0=pTri->fZ[0]
	fxch	st(3)			; F1F, _fA, F1E+F1D, _fZ0
	faddp	st(2), st		; _fA, F1F+F1E+F1D, _fZ0
	fld	DWORD PTR [esi][Adj2][0]; F1J=pTri->fAdjoint[2][0]
	fmul	st, st(3)		; F1K=F1J*F1I, _fA, F1F+F1E+F1D, _fZ0\/
	fld	DWORD PTR [esi][fZ][4]	; F1L = pTri->fZ[1] *
	fmul	DWORD PTR [esi][Adj2][4];       pTri->fAdjoint[2][1]
	fld	DWORD PTR [esi][fZ][8]	; F1M = pTri->fZ[2] *
	fmul	DWORD PTR [esi][Adj2][8];       pTri->fAdjoint[2][2]
	fxch	st(1)			; F1L, F1M, F1K, _fA, F1F+F1E+F1D
	faddp	st(2), st		; F1M, F1L+F1K, _fA, F1F+F1E+F1D
	fxch	st(3)			; F1F+F1E+F1D, F1L+F1K, _fA, F1M
	fmul	DWORD PTR [esi][f1OverDet]; _fB=(F1F+F1E+F1D)*f1OverDet
	fld	DWORD PTR fINVMAGIC	; C1=INVMAGIC
	fxch	st(4)			; F1M, _fB, F1L+F1K, _fA, C1
	faddp	st(2), st		; _fB, F1M+F1L+F1K, _fA, C1
	fld	st(2)			; _fA, _fB, F1M+F1L+F1K, _fA, C1
	fadd	st, st(1)		; _fA+_fB, _fB, F1M+F1L+F1K, _fA, C1
	fxch	st(2)			; F1M+F1L+F1K, _fB, _fA+_fB, _fA, C1
	fmul	DWORD PTR [esi][f1OverDet]; _fC=(F1M+F1L+F1K)*f1OverDet
	fld	st(3)			; _fA, _fC, _fB, _fA+_fB, _fA, C1
	fabs				; fAbs(_fA), _fC, _fB, _fA+_fB, _fA, C1
	fxch	st(1)			; _fC, fAbs(_fA), _fB, _fA+_fB, _fA, C1
	fmul	st(5), st		; _fC, fAbs(_fA), _fB, _fA+_fB, _fA, _fC*C1
	fld	st(2)			; _fB, _fC, fAbs(_fA), _fB, _fA+_fB, _fA, _fC*C1
	fabs				; fAbs(_fB), _fC, fAbs(_fA), _fB, _fA+_fB, _fA, _fC*C1
	fxch	st(6)			; _fC*C1, _fC, fAbs(_fA), _fB, _fA+_fB, _fA, fAbs(_fB)
	faddp	st(4), st		; _fC, fAbs(_fA), _fB, _fA+_fB+(_fC*C1), _fA, fAbs(_fB)
	fxch	st(5)			; fAbs(_fB), fAbs(_fA), _fB, _fA+_fB+(_fC*C1), _fA, _fC
	faddp	st(1), st		; fAbs(_fB)+fAbs(_fA), _fB, _fA+_fB+(_fC*C1), _fA, _fC
	fxch	st(2)			; _fA+_fB+(_fC*C1), _fB, fAbs(_fB)+fAbs(_fA), _fA, _fC
	mov	eax, 03a800000h
	fabs				; fAbs(_fA+_fB+(_fC*C1)), _fB, fAbs(_fB)+fAbs(_fA), _fA, _fC
	faddp	st(2), st		; _fB, fMaxVal=fAbs(_fA+_fB+(_fC*C1))+fAbs(_fB)+fAbs(_fA), _fA, _fC
	fxch	st(1)			; fMaxVal, _fB, _fA, _fC, _fZ0/\
else
        fld       DWORD PTR [esi+40]	; fZ0
        fsub      DWORD PTR [esi+48]	; fZ[0]-fZ2
        fld       DWORD PTR [esi+24]	; fAdj2[0], fZ[0]-fZ2
        fld       DWORD PTR [esi+48]	; fZ2, fAdj2[0], fZ[0]-fZ2
        fsubr     DWORD PTR [esi+44]	; fZ2-fZ[1], fAdj2[0], fZ[0]-fZ2
        fxch      st(2)			; fZ[0]-fZ2, fAdj2[0], fZ2-fZ[1]
        fmul      DWORD PTR [esi+36]	; fZ0, fAdj2[0], fZ2-fZ[1]
        fld       DWORD PTR [esi+28]	; fAdj2[1], fZ0, fAdj2[0], fZ2-fZ[1]
        fxch      st(3)			; fZ2-fZ[1], fZ0, fAdj2[0], fAdj2[1]
        fmul      DWORD PTR [esi+36]	; fZ1, fZ0, fAdj2[0], fAdj2[1]
        fld       DWORD PTR fINVMAGIC	; fINV, fZ1, fZ0, fAdj2[0], fAdj2[1]
        fxch      st(2)			; fZ0, fZ1, fINV, fAdj2[0], fAdj2[1]
        fmul      st(3), st		; fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fld       DWORD PTR [esi]	; fAdj0[0], fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st, st(1)		; fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fld       DWORD PTR [esi+4]	; fAdj0[1], fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st, st(3)		; fAdj0[1]*fZ1, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fld       DWORD PTR [esi+12]	; fAdj1[0], fAdj0[1]*fZ1, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st, st(3)		; fAdj1[0]*fZ0, fAdj0[1]*fZ1, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fxch      st(1)			; fAdj0[1]*fZ1, fAdj1[0]*fZ0, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        faddp     st(2), st		; fAdj1[0]*fZ0, fA, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fxch      st(3)			; fZ1, fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st(6), st		; fZ1, fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fAdj2[1]*fZ1
        fmul      DWORD PTR [esi+16]	; fZ1*fAdj1[1], fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fAdj2[1]*fZ1
        fxch      st(6)			; fAdj2[1]*fZ1, fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fZ1*fAdj1[1]
        faddp     st(5), st		; fA, fZ0, fAdj1[0]*fZ0, fINV, fC-fZ2, fZ1*fAdj1[1]
        fld       st(0)			; fA, fA, fZ0, fAdj1[0]*fZ0, fINV, fC-fZ2, fZ1*fAdj1[1]
        fabs				; fabs(fA), fA, fZ0, fAdj1[0]*fZ0, fINV, fC-fZ2, fZ1*fAdj1[1]
        fxch      st(5)			; fC-fZ2, fA, fZ0, fAdj1[0]*fZ0, fINV, fabs(fA), fZ1*fAdj1[1]
        fadd      DWORD PTR [esi+48]	; fC, fA, fZ0, fAdj1[0]*fZ0, fINV, fabs(fA), fZ1*fAdj1[1]
        fxch      st(6)			; fZ1*fAdj1[1], fA, fZ0, fAdj1[0]*fZ0, fINV, fabs(fA), fC
	faddp     st(3), st		; fA, fZ0, fB, fINV, fabs(fA), fC
        fxch      st(5)			; fC, fZ0, fB, fINV, fabs(fA), fA
        fmul      st(3), st		; fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fld       st(5)			; fA, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fadd      st, st(3)		; fA+fB, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fld       st(3)			; fB, fA+fB, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fabs				; fabs(fB), fA+fB, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fxch      st(5)			; fINV*fC, fA+fB, fC, fZ0, fB, fabs(fB), fabs(fA), fA
        faddp     st(1), st		; (fINV*fC)+fA+fB, fC, fZ0, fB, fabs(fB), fabs(fA), fA
        fxch      st(4)			; fabs(fB), fC, fZ0, fB, (fINV*fC)+fA+fB, fabs(fA), fA
        faddp     st(5), st		; fC, fZ0, fB, (fINV*fC)+fA+fB, fabs(fA)+fabs(fB), fA
        fxch      st(3)			; (fINV*fC)+fA+fB, fZ0, fB, fC, fabs(fA)+fabs(fB), fA
	mov	eax, 03a800000h		; eax = Value in stall
        fabs				; fabs((fINV*fC)+fA+fB), fZ0, fB, fC, fabs(fA)+fabs(fB), fA
        faddp     st(4), st		; fZ0, fB, fC, fMaxVal, fA
        fxch      st(3)			; fMaxVal, fB, fC, fZ0, fA
endif
	fstp	DWORD PTR [LPack20PA]	; fB, fC, fZ0, fA
	sub	eax, [LPack20PA]
	jg	NormalPack20		; if ( <= 0 ) NormalPack20
	fstp	st(0)			; fC, fZ0, fA
	fldz				; 0, fC, fZ0, fA
	fxch	st(3)			; fA, fC, fZ0, 0
	fstp	st(0)			; fC, fZ0, 0
	fxch	st(1)			; fZ0, Junk, 0
;++ jimp
; fix to get around error in nearly perpendicular case after conversion
; to simon's new maths
	fstp	st(0)
	fld		DWORD PTR [esi+40]	; load original fZ
;-- jimp
	fldz				; fB->0, fC->fZ0, Junk, fA->0
NormalPack20:
	fstp	DWORD PTR [LPack20PB]	; LPack20PB = fB, fC, Junk, fA
	fmul	DWORD PTR fFLOAT2FIXED	; fC * FloatToFixed (+2), Junk, fA
	fxch	st(2)			; fA, Junk, fC*FloatToFixed
	fstp	DWORD PTR [LPack20PA]	; LPack20PA = fA, Junk, fC*FloatToFixed
	fstp	st(0)			; fC*FloatToFixed
	fistp	DWORD PTR [edi][8]	; pPlaneMem[2+0] = INT32 fC*FloatToFixed
      mov	eax, [LPack20PB]		; eax = LPack20PB = INT32 fB
      PackTo20Bit1U
      PackTo20Bit2UC
       mov	ecx, ebx		; ecx = TSPAddr
	fld	DWORD PTR [esi][Adj0][8]; = pTri->fAdjoint[0][2]
	fld	DWORD PTR [esi][Adj1][8]; = pTri->fAdjoint[1][2]
	fabs				; 2856
	fld	st(1)			; 2852
	fabs				; 2856
	fld	DWORD PTR [esi][Adj2][8]; = pTri->fAdjoint[2][2]
	fld	st(0)			; 2854
	fabs				; 2856
	fxch	st(3)			; 2856
	faddp	st(2), st		; 2856 (+2)
      PackTo20Bit3UC
      PackTo20Bit4U
       shl	ecx, 20			; ecx = TSPAddr << 20
	fld	DWORD PTR [esi][Adj1][8]; = pTri->fAdjoint[1][2]
	fxch	st(2)			; 2853
	fmul	DWORD PTR f2xMAGIC	; 2856 (2)
      PackTo20Bit5U
      PackTo20Bit6U
		faddp	st(3), st		; 2856 (2)
	fxch	st(2)			; 2856  ^ (stall 1)
      or	ecx, eax		; 2844
       mov	eax, [LPack20PA]		; eax = LPack20PA
      PackTo20Bit1U
       mov	DWORD PTR [edi][4], ecx	; pPlaneMem[1+0] = ecx
	fstp	DWORD PTR [LInvMaxV]	; 742 (-1)
      PackTo20Bit2UC
       mov	ecx, 03F000h		; ecx = ( UPPER_6_OF_TAG &
      PackTo20Bit3UC
       mov	edx, [LInvMaxV]		; Do ApproxRecip integer part on edx
      PackTo20Bit4U
       rol	edx, 1			; edx = IE8, IM23, S1
      PackTo20Bit5U
       xor	edx, 0fffffffeh		; edx = (-IE8)-1, ~IM23, S1
      PackTo20Bit6U
       add	edx, 0fe000000h		; edx = 253-IE8, ~IM23, S1
      and	ecx, ebx		;	  TSPAddr          )
       ror	edx, 1			; edx = 253-IE8, ~IM23, S1
      shl	ecx, 8			; ecx <<= (20 - 12)
       mov	DWORD PTR [LInvMaxV], edx	; 756
	fmul	DWORD PTR [LInvMaxV]	; 2858
	fld	DWORD PTR [LInvMaxV]	; 2856
	fxch	st(3)			; 2856
	fmul	DWORD PTR [LInvMaxV]	; 2857 (2)
      or	ecx, 020000000h		; ecx |= forw_visiblefp
      or	ecx, eax		; ecx |= Pack 20 fA
      mov	DWORD PTR [edi], ecx	; pPlaneMem[0][0] = ecx
	fstp	DWORD PTR [LPack20PA]	; 530  (-1)
	fstp	DWORD PTR [LPack20PB]	; 530
	fmulp	st(1), st		; 2870 (2)
      mov	eax, [LPack20PA]		; 531
      PackTo20Bit1U
      PackTo20Bit2UC
	fmul	DWORD PTR fFLOAT2FIXED	; 2870 (-1 stall, 2)
      PackTo20Bit3UC
      PackTo20Bit4U
      PackTo20Bit5U
	fistp	DWORD PTR [edi][20]	; pPlaneMem[2+3] = Fpu (-1)
	fld	DWORD PTR [esi][Adj0][0]; 2874
	fld	DWORD PTR [esi][Adj1][0]; 2875
	fabs				; 2878
	fld	st(1)			; 2874
	fabs				; 2878
	fld	DWORD PTR [esi][Adj2][0]; 2876
	fld	st(0)			; 2876
	fabs				; 2878
	fxch	st(3)			; 2878
	faddp	st(2), st		; 2878 (2)
      PackTo20Bit6U
      or	eax, 008000000h		; 2865
	fld	DWORD PTR [esi][Adj1][0]; 2875
	fxch	st(2)			; 2875
	fmul	DWORD PTR f2xMAGIC	; 2878 (2)
      mov	DWORD PTR [edi][12], eax; pPlaneMem[0+3] = eax
       mov	eax, [LPack20PB]		; 531
      PackTo20Bit1U
	faddp	st(3), st		; 2878 (2)
	fxch	st(2)			; 2878  ^ (stall 1)
      PackTo20Bit2UC
      PackTo20Bit3UC
	fstp	DWORD PTR [LInvMaxV]	; 742 (-1)
      PackTo20Bit4U
       mov	edx, [LInvMaxV]		; Do ApproxRecip integer part on edx
      rol	edx, 1			; edx = IE8, IM23, S1
      PackTo20Bit5U
       xor	edx, 0fffffffeh		; edx = (-IE8)-1, ~IM23, S1
      PackTo20Bit6U
       add	edx, 0fe000000h		; edx = 253-IE8, ~IM23, S1
      ror	edx, 1			; edx = 253-IE8, ~IM23, S1
       mov	DWORD PTR [edi][16], eax	; pPlaneMem[1+3] = eax
      mov	DWORD PTR [LInvMaxV], edx; 756
	fmul	DWORD PTR [LInvMaxV]	; 2880
	fld	DWORD PTR [LInvMaxV]	; 2878
	fxch	st(3)			; 2878
	fmul	DWORD PTR [LInvMaxV]	; 2879 (2)
		fstp	DWORD PTR [LPack20PA]	; 530 (-1)
	fstp	DWORD PTR [LPack20PB]	; 530
	fmulp	st(1), st		; 2892 (2)
      mov	eax, [LPack20PA]		; 531
      PackTo20Bit1U
      PackTo20Bit2UC
	fmul	DWORD PTR fFLOAT2FIXED	; 2892 (-1 stall, 2)
      PackTo20Bit3UC
      PackTo20Bit4U
      PackTo20Bit5U
       mov	ecx, 008000000h		; 2887
	fistp	DWORD PTR [edi][32]	; pPlaneMem[2+6] = Fpu (-1)
	fld	DWORD PTR [esi][Adj0][4]; 2896
	fld	DWORD PTR [esi][Adj1][4]; 2897
	fabs				; 2900
	fld	st(1)			; 2896
	fabs				; 2900
	fld	DWORD PTR [esi][Adj2][4]; 2898
	fld	st(0)			; 2898
	fabs				; 2900
	fxch	st(3)			; 2900
	faddp	st(2), st		; 2900 (2)
      PackTo20Bit6U
      or	ecx, eax
	fld	DWORD PTR [esi][Adj1][4]; 2897
	fxch	st(2)			; 2897
	fmul	DWORD PTR f2xMAGIC	; 2900 (2)
      mov	eax, [LPack20PB]		; 531
      PackTo20Bit1U
       mov	DWORD PTR [edi][24], ecx	; pPlaneMem[0+6] = ecx
	faddp	st(3), st		; 2900 (2)
	fxch	st(2)			; 2900		^ (-1 stall)
	fstp	DWORD PTR [LInvMaxV]	; 742 (-1)
      PackTo20Bit2UC
       mov	edx, [LInvMaxV]		; Do ApproxRecip integer part on edx
      PackTo20Bit3UC
      PackTo20Bit4U
      rol	edx, 1			; edx = IE8, IM23, S1
      PackTo20Bit5U
       xor	edx, 0fffffffeh		; edx = (-IE8)-1, ~IM23, S1
      PackTo20Bit6U
       add	edx, 0fe000000h		; edx = 253-IE8, ~IM23, S1
      ror	edx, 1			; edx = 253-IE8, ~IM23, S1
       mov	DWORD PTR [edi][28], eax	; pPlaneMem[1+6] = Fpu
      mov	DWORD PTR [LInvMaxV], edx	; 756
	fmul	DWORD PTR [LInvMaxV]	; 2902
	fld	DWORD PTR [LInvMaxV]	; 2900
	fxch	st(3)			; 2900
	fmul	DWORD PTR [LInvMaxV]	; 2901 (2)
	fstp	DWORD PTR [LPack20PA]	; 530  (-1)
	fstp	DWORD PTR [LPack20PB]	; 530
      mov	eax, [LPack20PA]		; 531
      PackTo20Bit1U
      PackTo20Bit2UC
      PackTo20Bit3UC
      PackTo20Bit4U
      PackTo20Bit5U
      PackTo20Bit6U
       mov	edx, 008000000h		; 2910
      or	edx, eax			; 2910
       mov	eax, [LPack20PB]		; 531
      PackTo20Bit1U
       mov	DWORD PTR [edi][36], edx	; pPlaneMem[0+9] = edx
	fmulp	st(1), st		; 2915 (2)
      PackTo20Bit2UC
	fmul	DWORD PTR fFLOAT2FIXED	; 2915 (1 stall, 2)
      PackTo20Bit3UC
      PackTo20Bit4U
       add	esi, [nIncrement]	; ESI = ( pTri += nIncrement )
      PackTo20Bit5U
	fistp	DWORD PTR [edi][44]	; pPlaneMem[2+9] = Fpu (-1)
      PackTo20Bit6U
       add	ebx, [TSPIncrement]	; EBX = TSPAddr += TSPIncrement
      mov	DWORD PTR [edi][40], eax	; pPlanemem[1+9] = eax
       add	edi, WORDS_PER_PLANE*4	; pPlaneMem++
	dec	ebp			; EBP = nPolysInChunk - 1
	jns	L_MainLoop		; if ( EDX >= 0 ) L_MainLoop
L_EndWhileLoop:
	mov	edi, [rTSPAddr]		; edi = rTSPAddr
	mov	eax, [rpTri]		; eax = rpTri
	add	esp, Adjust		; Lose locals
	mov	ecx, ebx			; ecx = EBX = TSPAddr
	pop	ebx			; Recover registers
	pop	ebp
	mov	DWORD PTR [edi], ecx	; *rTSPAddr = ecx = TSPAddr
	pop	edi
	mov	DWORD PTR [eax], esi	; *rpTri = ESI = pTri
	pop	esi
	ret
	ALIGN	4
; mark_end;
__PackISPCore ENDP
;_PackISPCore	ENDS

;	COMDAT _PackISPPolygonCore
; -- Begin	_PackISPPolygonCore
; mark_begin;
	LInvMaxV	TEXTEQU <esp+  0>
	LPack20PB	TEXTEQU <esp+  4>
	LPack20PA	TEXTEQU <esp+  8>
	Adjust		TEXTEQU	<12>
	SaveEBX		TEXTEQU <esp+  0+Adjust>
	SaveEBP		TEXTEQU <esp+  4+Adjust>
	SaveESI		TEXTEQU <esp+  8+Adjust>
	SaveEDI		TEXTEQU <esp+ 12+Adjust>
	RetAddr		TEXTEQU <esp+ 16+Adjust>
	pPlaneMem	TEXTEQU	<esp+ 20+Adjust>
	nPolysInChunk	TEXTEQU <esp+ 24+Adjust>
	rTSPAddr	TEXTEQU <esp+ 28+Adjust>
	rpTri		TEXTEQU <esp+ 32+Adjust>
	nIncrement	TEXTEQU <esp+ 36+Adjust>
	TSPIncrement	TEXTEQU <esp+ 40+Adjust>
	rpEdge		TEXTEQU <esp+ 44+Adjust>
	nExtraVertices	TEXTEQU <esp+ 48+Adjust>

	ALIGN	4
	PUBLIC	__PackISPPolygonCore
__PackISPPolygonCore	PROC NEAR
	mov	eax, [esp][4]		; eax = pPlaneMem
	mov	edx, [esp][8]		; edx = nPolysInChunk
	mov	ecx, [esp][12]		; ecx = rTSPAddr
	push	esi			; Preserve additional registers used
	push	edi
	push	ebp
	push	ebx
	mov	esi, [esp][32]		; esi = rpTri
	mov	ebp, edx			; EBP = edx = nPolysInChunk
	mov	edi, eax		; EDI = pPlaneMem
	sub	esp, Adjust		; Allocate space for locals
	mov	ebx, [ecx]		; EBX = TSPAddr = [ecx]
	mov	esi, [esi]		; ESI = pTri = *rpTri
	dec	ebp			; EBP = nPolysInChunk - 1
	js	M_EndWhileLoop		; if ( EDX < 0 ) M_EndWhileLoop
M_MainLoop:
        fld       DWORD PTR [esi+40]	; fZ0
        fsub      DWORD PTR [esi+48]	; fZ[0]-fZ2
        fld       DWORD PTR [esi+24]	; fAdj2[0], fZ[0]-fZ2
        fld       DWORD PTR [esi+48]	; fZ2, fAdj2[0], fZ[0]-fZ2
        fsubr     DWORD PTR [esi+44]	; fZ2-fZ[1], fAdj2[0], fZ[0]-fZ2
        fxch      st(2)			; fZ[0]-fZ2, fAdj2[0], fZ2-fZ[1]
        fmul      DWORD PTR [esi+36]	; fZ0, fAdj2[0], fZ2-fZ[1]
        fld       DWORD PTR [esi+28]	; fAdj2[1], fZ0, fAdj2[0], fZ2-fZ[1]
        fxch      st(3)			; fZ2-fZ[1], fZ0, fAdj2[0], fAdj2[1]
        fmul      DWORD PTR [esi+36]	; fZ1, fZ0, fAdj2[0], fAdj2[1]
        fld       DWORD PTR fINVMAGIC	; fINV, fZ1, fZ0, fAdj2[0], fAdj2[1]
        fxch      st(2)			; fZ0, fZ1, fINV, fAdj2[0], fAdj2[1]
        fmul      st(3), st		; fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fld       DWORD PTR [esi]	; fAdj0[0], fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st, st(1)		; fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fld       DWORD PTR [esi+4]	; fAdj0[1], fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st, st(3)		; fAdj0[1]*fZ1, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fld       DWORD PTR [esi+12]	; fAdj1[0], fAdj0[1]*fZ1, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st, st(3)		; fAdj1[0]*fZ0, fAdj0[1]*fZ1, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fxch      st(1)			; fAdj0[1]*fZ1, fAdj1[0]*fZ0, fAdj0[0]*fZ0, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        faddp     st(2), st		; fAdj1[0]*fZ0, fA, fZ0, fZ1, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fxch      st(3)			; fZ1, fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fAdj2[1]
        fmul      st(6), st		; fZ1, fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fAdj2[1]*fZ1
        fmul      DWORD PTR [esi+16]	; fZ1*fAdj1[1], fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fAdj2[1]*fZ1
        fxch      st(6)			; fAdj2[1]*fZ1, fA, fZ0, fAdj1[0]*fZ0, fINV, fAdj2[0]*fZ0, fZ1*fAdj1[1]
        faddp     st(5), st		; fA, fZ0, fAdj1[0]*fZ0, fINV, fC-fZ2, fZ1*fAdj1[1]
        fld       st(0)			; fA, fA, fZ0, fAdj1[0]*fZ0, fINV, fC-fZ2, fZ1*fAdj1[1]
        fabs				; fabs(fA), fA, fZ0, fAdj1[0]*fZ0, fINV, fC-fZ2, fZ1*fAdj1[1]
        fxch      st(5)			; fC-fZ2, fA, fZ0, fAdj1[0]*fZ0, fINV, fabs(fA), fZ1*fAdj1[1]
        fadd      DWORD PTR [esi+48]	; fC, fA, fZ0, fAdj1[0]*fZ0, fINV, fabs(fA), fZ1*fAdj1[1]
        fxch      st(6)			; fZ1*fAdj1[1], fA, fZ0, fAdj1[0]*fZ0, fINV, fabs(fA), fC
	faddp     st(3), st		; fA, fZ0, fB, fINV, fabs(fA), fC
        fxch      st(5)			; fC, fZ0, fB, fINV, fabs(fA), fA
        fmul      st(3), st		; fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fld       st(5)			; fA, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fadd      st, st(3)		; fA+fB, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fld       st(3)			; fB, fA+fB, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fabs				; fabs(fB), fA+fB, fC, fZ0, fB, fINV*fC, fabs(fA), fA
        fxch      st(5)			; fINV*fC, fA+fB, fC, fZ0, fB, fabs(fB), fabs(fA), fA
        faddp     st(1), st		; (fINV*fC)+fA+fB, fC, fZ0, fB, fabs(fB), fabs(fA), fA
        fxch      st(4)			; fabs(fB), fC, fZ0, fB, (fINV*fC)+fA+fB, fabs(fA), fA
        faddp     st(5), st		; fC, fZ0, fB, (fINV*fC)+fA+fB, fabs(fA)+fabs(fB), fA
        fxch      st(3)			; (fINV*fC)+fA+fB, fZ0, fB, fC, fabs(fA)+fabs(fB), fA
	mov	eax, 03a800000h		; eax = Value in stall
        fabs				; fabs((fINV*fC)+fA+fB), fZ0, fB, fC, fabs(fA)+fabs(fB), fA
        faddp     st(4), st		; fZ0, fB, fC, fMaxVal, fA
        fxch      st(3)			; fMaxVal, fB, fC, fZ0, fA
	fstp	DWORD PTR [LPack20PA]	; fB, fC, fZ0, fA
	sub	eax, [LPack20PA]
	jg	M_NormalPack20		; if ( <= 0 ) NormalPack20
	fstp	st(0)			; fC, fZ0, fA
	fldz				; 0, fC, fZ0, fA
	fxch	st(3)			; fA, fC, fZ0, 0
	fstp	st(0)			; fC, fZ0, 0
	fxch	st(1)			; fZ0, Junk, 0
;++ jimp
; fix to get around error in nearly perpendicular case after conversion
; to simon's new maths
	fstp	st(0)
	fld		DWORD PTR [esi+40]	; load original fZ
;-- jimp
	fldz				; fB->0, fC->fZ0, Junk, fA->0
M_NormalPack20:
	fstp	DWORD PTR [LPack20PB]	; LPack20PB = fB, fC, Junk, fA
	fmul	DWORD PTR fFLOAT2FIXED	; fC * FloatToFixed (+2), Junk, fA
	fxch	st(2)			; fA, Junk, fC*FloatToFixed
	fstp	DWORD PTR [LPack20PA]	; LPack20PA = fA, Junk, fC*FloatToFixed
	fstp	st(0)			; fC*FloatToFixed
	fistp	DWORD PTR [edi][8]	; pPlaneMem[2+0] = INT32 fC*FloatToFixed
      mov	eax, [LPack20PB]		; eax = LPack20PB = INT32 fB
      PackTo20Bit1U
      PackTo20Bit2UC
       mov	ecx, ebx		; ecx = TSPAddr
	fld	DWORD PTR [esi][Adj0][8]; = pTri->fAdjoint[0][2]
	fld	DWORD PTR [esi][Adj1][8]; = pTri->fAdjoint[1][2]
	fabs				; 2856
	fld	st(1)			; 2852
	fabs				; 2856
	fld	DWORD PTR [esi][Adj2][8]; = pTri->fAdjoint[2][2]
	fld	st(0)			; 2854
	fabs				; 2856
	fxch	st(3)			; 2856
	faddp	st(2), st		; 2856 (+2)
      PackTo20Bit3UC
      PackTo20Bit4U
       shl	ecx, 20			; ecx = TSPAddr << 20
	fld	DWORD PTR [esi][Adj1][8]; = pTri->fAdjoint[1][2]
	fxch	st(2)			; 2853
	fmul	DWORD PTR f2xMAGIC	; 2856 (2)
      PackTo20Bit5U
      PackTo20Bit6U
		faddp	st(3), st		; 2856 (2)
	fxch	st(2)			; 2856  ^ (stall 1)
      or	ecx, eax		; 2844
       mov	eax, [LPack20PA]		; eax = LPack20PA
      PackTo20Bit1U
       mov	DWORD PTR [edi][4], ecx	; pPlaneMem[1+0] = ecx
	fstp	DWORD PTR [LInvMaxV]	; 742 (-1)
      PackTo20Bit2UC
       mov	ecx, 03F000h		; ecx = ( UPPER_6_OF_TAG &
      PackTo20Bit3UC
       mov	edx, [LInvMaxV]		; Do ApproxRecip integer part on edx
      PackTo20Bit4U
       rol	edx, 1			; edx = IE8, IM23, S1
      PackTo20Bit5U
       xor	edx, 0fffffffeh		; edx = (-IE8)-1, ~IM23, S1
      PackTo20Bit6U
       add	edx, 0fe000000h		; edx = 253-IE8, ~IM23, S1
      and	ecx, ebx		;	  TSPAddr          )
       ror	edx, 1			; edx = 253-IE8, ~IM23, S1
      shl	ecx, 8			; ecx <<= (20 - 12)
       mov	DWORD PTR [LInvMaxV], edx	; 756
	fmul	DWORD PTR [LInvMaxV]	; 2858
	fld	DWORD PTR [LInvMaxV]	; 2856
	fxch	st(3)			; 2856
	fmul	DWORD PTR [LInvMaxV]	; 2857 (2)
      or	ecx, 020000000h		; ecx |= forw_visiblefp
      or	ecx, eax		; ecx |= Pack 20 fA
      mov	DWORD PTR [edi], ecx	; pPlaneMem[0][0] = ecx
	fstp	DWORD PTR [LPack20PA]	; 530  (-1)
	fstp	DWORD PTR [LPack20PB]	; 530
	fmulp	st(1), st		; 2870 (2)
      mov	eax, [LPack20PA]		; 531
      PackTo20Bit1U
      PackTo20Bit2UC
	fmul	DWORD PTR fFLOAT2FIXED	; 2870 (-1 stall, 2)
      PackTo20Bit3UC
      PackTo20Bit4U
      PackTo20Bit5U
	fistp	DWORD PTR [edi][20]	; pPlaneMem[2+3] = Fpu (-1)
	fld	DWORD PTR [esi][Adj0][0]; 2874
	fld	DWORD PTR [esi][Adj1][0]; 2875
	fabs				; 2878
	fld	st(1)			; 2874
	fabs				; 2878
	fld	DWORD PTR [esi][Adj2][0]; 2876
	fld	st(0)			; 2876
	fabs				; 2878
	fxch	st(3)			; 2878
	faddp	st(2), st		; 2878 (2)
      PackTo20Bit6U
      or	eax, 008000000h		; 2865
	fld	DWORD PTR [esi][Adj1][0]; 2875
	fxch	st(2)			; 2875
	fmul	DWORD PTR f2xMAGIC	; 2878 (2)
      mov	DWORD PTR [edi][12], eax; pPlaneMem[0+3] = eax
       mov	eax, [LPack20PB]		; 531
      PackTo20Bit1U
	faddp	st(3), st		; 2878 (2)
	fxch	st(2)			; 2878  ^ (stall 1)
      PackTo20Bit2UC
      PackTo20Bit3UC
	fstp	DWORD PTR [LInvMaxV]	; 742 (-1)
      PackTo20Bit4U
       mov	edx, [LInvMaxV]		; Do ApproxRecip integer part on edx
      rol	edx, 1			; edx = IE8, IM23, S1
      PackTo20Bit5U
       xor	edx, 0fffffffeh		; edx = (-IE8)-1, ~IM23, S1
      PackTo20Bit6U
       add	edx, 0fe000000h		; edx = 253-IE8, ~IM23, S1
      ror	edx, 1			; edx = 253-IE8, ~IM23, S1
       mov	DWORD PTR [edi][16], eax	; pPlaneMem[1+3] = eax
      mov	DWORD PTR [LInvMaxV], edx; 756
	fmul	DWORD PTR [LInvMaxV]	; 2880
	fld	DWORD PTR [LInvMaxV]	; 2878
	fxch	st(3)			; 2878
	fmul	DWORD PTR [LInvMaxV]	; 2879 (2)
	fstp	DWORD PTR [LPack20PA]	; 530 (-1)
	fstp	DWORD PTR [LPack20PB]	; 530
	fmulp	st(1), st		; 2892 (2)
      mov	eax, [LPack20PA]		; 531
      PackTo20Bit1U
      PackTo20Bit2UC
	fmul	DWORD PTR fFLOAT2FIXED	; 2892 (-1 stall, 2)
      PackTo20Bit3UC
       mov	edx, DWORD PTR [rpEdge]	; edx = rpEdge
      PackTo20Bit4U
       mov	ecx, 008000000h		; 2887
      PackTo20Bit5U
       mov	[nPolysInChunk], ebp	; Save nPolysInChunk = EBP
	fistp	DWORD PTR [edi][32]	; pPlaneMem[2+6] = Fpu (-1)
      PackTo20Bit6U
       mov	edx, DWORD PTR [edx]	; EDX = *rpEdge
      or	ecx, eax
       mov	eax, [LPack20PB]		; 531
;
; pPlaneMem = PackISPPoly( pPlaneMem + (WORDS_PER_PLANE*3), 
;					ppEdge, nExtraVertices );
        fld       DWORD PTR [edx] 	; fA
        fabs                            ; fAbs(fA)
        fld       DWORD PTR [edx+4] 	; fB, fAbs(fA)
        fabs                            ; fAbs(fB)
        fld       DWORD PTR [edx+8] 	; fC, fAbs(fB), fAbs(fA)
        fabs                            ; fAbs(fC)
        fxch      st(1)                 ; fAbs(fB), fAbs(fC), fAbs(fA)
	faddp     st(2), st             ;2:fAbs(fC), fAbs(fA)+fAbs(fB) (2)
      PackTo20Bit1U
       mov	DWORD PTR [edi][24], ecx	; pPlaneMem[0+6] = ecx
      PackTo20Bit2UC
        fld       DWORD PTR [edx] 	; fA
        fxch      st(2)                 ; fAbs(fA)+fAbs(fB), fAbs(fC), fA
        fmul      DWORD PTR f2xMAGIC    ;2:2*MAGIC*(fAbs(fA)+fAbs(fB)), fAbs(fC), fA
      PackTo20Bit3UC
       mov	ebp, [nExtraVertices]	; EBP = nExtraVertices 
      PackTo20Bit4U
       add	esi, [nIncrement]	; ESI = ( pTri += nIncrement )
        faddp     st(1), st             ;2:2*MAGIC*(fAbs(fA)+fAbs(fB))+fAbs(fC), fA
      PackTo20Bit5U
       mov	[pPlaneMem], esi		; Save pTri in pPlaneMem
      PackTo20Bit6U
       add	ebx, [TSPIncrement]	; EBX = TSPAddr += TSPIncrement
      mov	DWORD PTR [edi][28], eax	; pPlaneMem[1+6] = Fpu
       mov	esi, 2139095040                                       ; 746
        fstp      DWORD PTR [LInvMaxV] 	;-1:742
      mov	ecx, DWORD PTR [LInvMaxV] ; 746
       mov	eax, 2122317824                                       ; 747
      and       esi, ecx                                              ; 746
       and	ecx, -2139095041                                      ; 751
      sub	eax, esi                                              ; 747
       xor       ecx, 8388607                                          ; 752
      and       eax, 2139095040                                       ; 747
      or        eax, ecx                                              ; 756
      mov       DWORD PTR [LInvMaxV], eax ; 756
        fmul      DWORD PTR [LInvMaxV] ; 543
        fld       DWORD PTR [LInvMaxV] ; 534
        fxch      st(1)                                                 ; 534
        fstp      DWORD PTR [LPack20PA] ; 169
        fld       DWORD PTR [edx+4] ; 546
        fmul      st, st(1)             ;2:546
      mov       eax, DWORD PTR [LPack20PA] ; 170
      PackTo20Bit1U
        fstp      DWORD PTR [LPack20PB] ; 169
        fmul      DWORD PTR [edx+8] 	 ;2: 548
      PackTo20Bit2UC
      PackTo20Bit3UC
        fmul      DWORD PTR fFLOAT2FIXED ;2: 548
      PackTo20Bit4U
       lea       edx, DWORD PTR [edx+12] ; 558
      PackTo20Bit5U
       mov	 ecx, 134217728                                        ; 543
      PackTo20Bit6U
        fistp     DWORD PTR [edi][36][8] ;-1: 548
      or        ecx, eax
       mov       eax, DWORD PTR [LPack20PB] ; 170
      PackTo20Bit1U
       mov       DWORD PTR [edi][36], ecx ; 543
      PackTo20Bit2UC
      PackTo20Bit3UC
      PackTo20Bit4U
      PackTo20Bit5U
      PackTo20Bit6U
      mov	DWORD PTR [edi][36][4], eax ; 546
       lea       edi, DWORD PTR [edi][36][12] ; 557
M_EdgeLoop:
        fld       DWORD PTR [edx] 	; fA
        fabs                            ; fAbs(fA)
        fld       DWORD PTR [edx+4] 	; fB, fAbs(fA)
        fabs                            ; fAbs(fB)
        fld       DWORD PTR [edx+8] 	; fC, fAbs(fB), fAbs(fA)
        fabs                            ; fAbs(fC)
        fxch      st(1)                 ; fAbs(fB), fAbs(fC), fAbs(fA)
	faddp     st(2), st             ; fAbs(fC), fAbs(fA)+fAbs(fB)
        fld       DWORD PTR [edx] 	; fA
        fxch      st(2)                 ; fAbs(fA)+fAbs(fB), fAbs(fC), fA
        fmul      DWORD PTR f2xMAGIC    ; 2*MAGIC*(fAbs(fA)+fAbs(fB)), fAbs(fC), fA
        faddp     st(1), st             ; 2*MAGIC*(fAbs(fA)+fAbs(fB))+fAbs(fC), fA
      mov	eax, 2122317824                                       ; 747
       mov	esi, 2139095040                                       ; 746
        fstp      DWORD PTR [LInvMaxV] 	; 742
      mov	ecx, DWORD PTR [LInvMaxV] ; 746
      and       esi, ecx                                              ; 746
      and	ecx, -2139095041                                      ; 751
       sub	eax, esi                                              ; 747
      xor       ecx, 8388607                                          ; 752
       and       eax, 2139095040                                       ; 747
      or        eax, ecx                                              ; 756
      mov       DWORD PTR [LInvMaxV], eax ; 756
        fmul      DWORD PTR [LInvMaxV] ; 543
        fld       DWORD PTR [LInvMaxV] ; 534
        fxch      st(1)                                                 ; 534
        fstp      DWORD PTR [LPack20PA] ; 169
        fld       DWORD PTR [edx+4] ; 546
        fmul      st, st(1)         ;2: 546
      mov       eax, DWORD PTR [LPack20PA] ; 170
      PackTo20Bit1U
      PackTo20Bit2UC
        fstp      DWORD PTR [LPack20PB] ;-1:546
        fmul      DWORD PTR [edx+8] ;2: 548
      PackTo20Bit3UC
      PackTo20Bit4U
        fmul      DWORD PTR fFLOAT2FIXED ;2: 548
      PackTo20Bit5U
      	mov	ecx, 134217728                                        ; 543
      PackTo20Bit6U
        fistp     DWORD PTR [edi+8] ;-1: 548
      or        ecx, eax
       mov       eax, DWORD PTR [LPack20PB] ; 170
      PackTo20Bit1U
       mov       DWORD PTR [edi], ecx ; 543
      PackTo20Bit2UC
      PackTo20Bit3UC
      PackTo20Bit4U
      PackTo20Bit5U
      PackTo20Bit6U
      mov	DWORD PTR [edi+4], eax ; 546
      dec       ebp                                                   ; 559
       lea       edx, DWORD PTR [edx+12] ; 558
      lea       edi, DWORD PTR [edi+12] ; 557
       jnz       M_EdgeLoop		; Main loop till all done
      mov	ecx, DWORD PTR [rpEdge]	; Update *rpEdge
       mov	esi, [pPlaneMem]                                       ; 526
      mov	ebp, [nPolysInChunk]                                   ; 526
       dec	ebp			; EBP = nPolysInChunk - 1
;
; End of INLINE _PackISPPoly
;
      mov	DWORD PTR [ecx], edx
       jns	M_MainLoop		; if ( EDX >= 0 ) M_MainLoop
M_EndWhileLoop:
	mov	edi, [rTSPAddr]		; edi = rTSPAddr
	mov	eax, [rpTri]		; eax = rpTri
	add	esp, Adjust		; Lose locals
	mov	ecx, ebx			; ecx = EBX = TSPAddr
	pop	ebx			; Recover registers
	pop	ebp
	mov	DWORD PTR [edi], ecx	; *rTSPAddr = ecx = TSPAddr
	pop	edi
	mov	DWORD PTR [eax], esi	; *rpTri = ESI = pTri
	pop	esi
	ret
	ALIGN	4
; mark_end;
__PackISPPolygonCore ENDP
;_PackISPPolygonCore	ENDS
_TEXT	ENDS

_DATA1	SEGMENT PARA PUBLIC USE32 'DATA'
f2xMAGIC	DD 045000000H	; 1024.0f = 1x2^10
fFLOAT2FIXED	DD 04f000000H	; 1x2^20
fINVMAGIC		DD 03a800000H	; 1/1024.0f = 1x2^-10
_DATA1	ENDS

; -- End	_PackISPCores

if 0
; The code below is out of date, new versions of the Process routines are
; now in dtriml.asm anyway.

	PUBLIC _ProcessD3DStripCore

_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'

    ALIGN     4                                                    ; 0

_ProcessD3DStripCore	PROC NEAR

	nParam		TEXTEQU <esp+  0>
	LfTemp1		TEXTEQU <esp+  8>
	LnTemp2     TEXTEQU <esp+ 16>
	LpAddFn		TEXTEQU <esp+ 36>
	LShiftRegX	TEXTEQU <esp+ 40>
	LfTemp3		TEXTEQU <esp+ 48>
;	LfTemp4		TEXTEQU <esp+ 56>
	LfTemp5		TEXTEQU <esp+ 64>
	LMyMagic1	TEXTEQU <esp+ 68>
	LfTemp6		TEXTEQU <esp+ 72>
	NExtraV		TEXTEQU <esp+ 84>
	u2NextFace	TEXTEQU <esp+ 88>
	pEdge		TEXTEQU <esp+ 92>
	pStartEdge	TEXTEQU <esp+ 96>
	pMat		TEXTEQU <esp+ 100>
	Adjust		TEXTEQU	<104>
	SaveEBX		TEXTEQU <esp+  0+Adjust>
	SaveEBP		TEXTEQU <esp+  4+Adjust>
	SaveESI		TEXTEQU <esp+  8+Adjust>
	SaveEDI		TEXTEQU <esp+ 12+Adjust>
	RetAddr		TEXTEQU <esp+ 16+Adjust>

ifdef DEBUG
	mov	eax, [esp][4]		; presumably if debug we dont optimize
endif

	push	edi			; Save registers
	push	esi
	push	ebp
	push	ebx

	sub		esp, Adjust
	mov		edx, DWORD PTR _gPDC[ShiftRegX]
	mov		ecx, OFFSET FLAT: _gpEdge		; ecx = pEdge = _gpEdge
	mov		ebx, OFFSET FLAT: _gpTri		; EBX = pTri = _gpTri
	mov		DWORD PTR [LpAddFn], eax
	mov		eax, OFFSET FLAT: _gpMat
	mov		DWORD PTR [LShiftRegX], edx
	mov		DWORD PTR [pEdge], ecx			; save edge struct ptrs
	mov		DWORD PTR [pStartEdge], ecx
	mov		DWORD PTR [pMat], eax

	jmp		P_MainLoop_Strip

	; culling stuff - called when continuing after a culled poly

P_MainLoop_Culled_Strip:

	mov		ecx, DWORD PTR [NExtraV]
	mov		eax, DWORD PTR _gPDC[nPolys]	; eax = nPolys
	mov		edx, ecx
	inc		ecx
	shl		edx, 3
	sub		eax, ecx						; eax = nPolys - nExtras - 1
	js		P_End_Strip
	mov		DWORD PTR _gPDC[nPolys], eax	; nPolys = eax
	add		edx, DWORD PTR _gPDC[pFace]		; edx = pFace
	jmp		@F

P_MainLoop_Strip:
	
	mov		eax, -1
	mov		edx, DWORD PTR _gPDC[pFace]		; edx = pFace
	add		eax, DWORD PTR _gPDC[nPolys]	; eax = nPolys - 1
	js		P_End_Strip						; If ( ( nPolys - 1 ) < 0 ) End
	mov		DWORD PTR _gPDC[nPolys], eax	; nPolys = eax

@@:	; start analysing triangle

	mov		ebp, DWORD PTR [pStartEdge]		; initialise pEdge
	mov		ecx, DWORD PTR [edx][0]			; ecx = u1 = pFace[0]
	xor		eax, eax
	mov		DWORD PTR [pEdge], ebp			; save for now
	mov		edi, DWORD PTR _gPDC[pVertices]	; edi = pVertices
	shld	eax, ecx, 16					; eax = u1>>16
	mov		esi, edi
	shl		eax, 5							; eax *= sizeof (VERTEX)
	and		ecx, 0FFFFh						; ecx = u1 & FFFF
	add		esi, eax						; ESI = v1 : pVertices+((u1>>16)*sizeof(VERTEX))
	shl		ecx, 5							; ecx *= (u1&FFFF)*sizeof (VERTEX)
	mov		ebp, edi
	mov		eax, DWORD PTR [edx][4]			; eax = u2 = pFace[1]
	add		ebp, ecx						; EBP = v0 : pVertices+((u1&FFFF)*sizeof(VERTEX))
	shld	ecx, eax, 16					; ecx = u2>>16 (face flags)
	and		eax, 0FFFFh						; eax = u2 & FFFF
	add		edx, 8							; edx += sizeof(D3DFACE)
	shl		eax, 5							; eax *= sizeof (VERTEX)
	and		ecx, 0001Fh						; ecx = face flags, correctly masked
	add		edi, eax						; EDI = v2 : pVertices+((u2&FFFF)*sizeof(VERTEX))

	; see if we have a valid number of extras

	or		ecx, ecx						; zero?
	mov		DWORD PTR _gPDC[pFace], edx		; _gPDC.pFace = edx
	jz		ValidNumberOfTris				; yes, no extra tris
	jmp		@F

	; this is unlikely so lets take a penalty for it to make the
	; rest go faster

InvalidNumberOfTris:

	xor		ecx, ecx
	jmp		ValidNumberOfTris

	; next two branches test unusual conditions 
	; so start loading next face flags now, we
	; probably won't need to jump

@@:

	mov		eax, DWORD PTR [edx][4]			; Next face's u2
	cmp		ecx, DWORD PTR _gPDC[nPolys]	; extra tris > nPolys?
	jg		InvalidNumberOfTris				; yes, cancel extra tris
	and		eax, 01f0000h					; mask in flag bits
	cmp		ecx, D3DTRIFLAG_ODD				; valid number of extras?
	jge		InvalidNumberOfTris

	; we have a valid number of extras
	
	cmp		eax, 0001e0000h					; odd?
	jne		ValidNumberOfTris				; no, jump

	; ODD face: the vertices need to be rotated
	; and the flag changed to even

	xchg	ebp, edi						; edi=v0,esi=v1,ebp=v2
	and		eax, 0FFFFh
	xchg	esi, edi						; esi=v0,edi=v1,ebp=v2
	or		eax, 0001f0000h					; or in an even flag for first extra

ValidNumberOfTris:

	; save number of tris and next face info

	mov		DWORD PTR [NExtraV], ecx
	mov		DWORD PTR [u2NextFace], eax

	; start doing the adjoint

	fld		DWORD PTR [edi][fY]				; fY2
	fmul	DWORD PTR [esi][fX]				; fX1*fY2
	fld		DWORD PTR [edi][fX]				; fX2, fX1*fY2
	fmul	DWORD PTR [esi][fY]				; fX2*fY1, fX1*fY2
	fld		DWORD PTR [edi][fX]				; fX2, fX2*fY1, fX1*fY2
	mov		DWORD PTR _gPDC[pV2], edi		; _gPDC.pV2 = EDI
	fld		DWORD PTR [ebp][fY]				; fY0, fX2, fX2*fY1, fX1*fY2
	fmul	st, st(1)						; fY0*fX2, fX2, fX2*fY1, fX1*fY2
	fxch	st(2)							; fX2*fY1, fX2, fY0*fX2, fX1*fY2
	fsubp	st(3), st						; fX2, fY0*fX2, Adj20
	mov		DWORD PTR _gPDC[pV1], esi		; _gPDC.pV1 = ESI
	fld		DWORD PTR [edi][fY]				; fY2, fX2, fY0*fX2, Adj20
	fmul	DWORD PTR [ebp][fX]				; fY2*fX0, fX2, fY0*fX2, Adj20
	fld		DWORD PTR [ebp][fX]				; fX0, fY2*fX0, fX2, fY0*fX2, Adj20
	fmul	DWORD PTR [esi][fY]				; fY1*fX0, fY2*fX0, fX2, fY0*fX2, Adj20
	fxch	st(1)							; fY2*fX0, fY1*fX0, fX2, fY0*fX2, Adj20
	fsubp	st(3), st						; fY1*fX0, fX2, Adj21, Adj20
	fld		DWORD PTR [ebp][fY]				; fY0, fY1*fX0, fX2, Adj21, Adj20
	fmul	DWORD PTR [esi][fX]				; fX1*fY0, fY1*fX0, fX2, Adj21, Adj20
	fld		DWORD PTR _2_3_2il2floatpacket_7; C1=?, fX1*fY0, fY1*fX0, fX2, Adj21, Adj20
	mov		DWORD PTR _gPDC[pV0], ebp		; _gPDC.pV0 = EBP
	fld		st(5)							; Adj20, C1=?, fX1*fY0, fY1*fX0, fX2, Adj21, Adj20
	fadd	st, st(5)						; Adj20+Adj21, C1=?, fX1*fY0, fY1*fX0, fX2, Adj21, Adj20
	fxch	st(2)							; fX1*fY0, C1=?, Adj20+Adj21, fY1*fX0, fX2, Adj21, Adj20
	fsubp	st(3), st						; C1=?, Adj20+Adj21, Adj22, fX2, Adj21, Adj20
	fld		DWORD PTR [esi][fX]				; fX1, C1=?, Adj20+Adj21, Adj22, fX2, Adj21, Adj20
	fxch	st(3)							; Adj22, C1=?, Adj20+Adj21, fX1, fX2, Adj21, Adj20
	fadd	st(2), st						; Adj22, C1=?, fDet, fX1, fX2, Adj21, Adj20
	fxch	st(3)							; fX1, C1=?, fDet, Adj22, fX2, Adj21, Adj20
	fxch	st(1)							; C1=?, fX1, fDet, Adj22, fX2, Adj21, Adj20
	fcomp	st(2)							; fX1, fDet, Adj22, fX2, Adj21, Adj20
	fnstsw	ax								; Flags in ax
	fxch	st(2)							; Adj22, fDet, fX1, fX2, Adj21, Adj20
	fstp	DWORD PTR [ebx+32]				; Store Adj22
	fxch	st(3)							; Adj21, fX1, fX2, fDet, Adj20
	fstp	DWORD PTR [ebx+28]				; Store Adj21
	fxch	st(3)							; Adj20, fX2, fDet, fX1
	fstp	DWORD PTR [ebx+24]				; Store Adj20
	fxch	st(1)							; fDet, fX2, fX1
	fstp	DWORD PTR [LfTemp1]				; LfTemp1 = fDet
	and		eax, 16640						; 
	jne		P_DeterNorm_Strip				;

	fstp	st(0)							; Discard fX2 and fX1
	fstp	st(0)							; 
	mov		eax, _gPDC[Context][bCullBackfacing]
	test	eax, eax						; 464
	jne		P_MainLoop_Culled_Strip			; if bCullBackfacing continue

	fld		DWORD PTR [LfTemp1]				; fDet
	fcomp	DWORD PTR _2_3_2il2floatpacket_10; if fDet > -1e-20 continue
	fnstsw	ax								; 464
	and		eax, 16640						; 464
	je		P_MainLoop_Culled_Strip			; if perpendicular continue

	mov		eax, edi				; Swap esi and edi
	mov		DWORD PTR _gPDC[pV1], edi	; _gPDC.pV1 = old _gPDC.pV2
	mov		edi, esi
	mov		DWORD PTR _gPDC[pV2], esi	; _gPDC.pV2 = old _gPDC.pV1
	mov		esi, eax
	mov		ecx, DWORD PTR [ebx+32]		; 485
	mov		eax, DWORD PTR [ebx+28]		; 486
	mov		edx, 080000000h
	fld		DWORD PTR [esi][fX]			; fX1
	fld		DWORD PTR [edi][fX]			; fX2, fX1
	mov		DWORD PTR [ebx+32], eax		; 486
	mov		eax, DWORD PTR [ebx+32]		; 493
	xor		eax, edx					; 493
	mov		DWORD PTR [ebx+28], ecx		; 487
	mov		DWORD PTR [ebx+32], eax		; 493
	mov		eax, DWORD PTR [ebx+28]		; 492
	xor		eax, edx					; 492
	xor		edx, DWORD PTR [ebx+24]		; 491
	mov		DWORD PTR [ebx+28], eax		; 492
	mov		eax, DWORD PTR [LfTemp1]	; 495
	mov		DWORD PTR [ebx+24], edx		; 491
	xor		eax, -2147483648			; 495
	mov		DWORD PTR [LfTemp1], eax	; 495

P_DeterNorm_Strip:
	
	fld		DWORD PTR [ebp][fX]		; fX0, fX2, fX1
	fsubr	st, st(2)			; Adj12, fX2, fX1
	fld		DWORD PTR [ebp][fX]		; fX0, Adj12, fX2, fX1
	fsub	st, st(2)			; Adj11, Adj12, fX2, fX1
	fld		DWORD PTR [ebp][fY]		; fY0, Adj11, Adj12, fX2, fX1
	fsub	DWORD PTR [esi][fY]		; Adj02, Adj11, Adj12, fX2, fX1
	fxch	st(2)				; Adj12, Adj11, Adj02, fX2, fX1
	fstp	DWORD PTR [ebx+20]		; Store Adj12 
	fstp	DWORD PTR [ebx+16]		; Store Adj11
	fstp	DWORD PTR [ebx+8]		; Store Adj02
	fld		DWORD PTR [ebp][fY]		; fY0, fX2, fX1
	fsubr	DWORD PTR [edi][fY]		; Adj01, fX2, fX1
	fld		DWORD PTR [edi][fY]		; fY2, Adj01, fX2, fX1
	fsubr	DWORD PTR [esi][fY]		; Adj00, Adj01, fX2, fX1
	fxch	st(1)				; Adj01, Adj00, fX2, fX1
	fstp	DWORD PTR [ebx+4]		; Store Adj01
	fstp	DWORD PTR [ebx]			; Store Adj00
	fld		DWORD PTR _2_3_2il2floatpacket_13; C2 =?, fX2, fX1
	fld		st(1)				; fX2, C2, fX2, fX1
	fsub	st, st(3)			; Adj10, C2, fX2, fX1

	test	DWORD PTR _gPDC[Context][u32Flags], SGLTT_DISABLEZBUFFER
	fstp	DWORD PTR [ebx+12]		; Store Adj10
	; fp stack now	C2, fX2, fX1
	jz		not_bogus_z_Strip

	fld		DWORD PTR _gfBogusZ
	fld		DWORD PTR _BogusZIncrementValue ; BZD, BZ, C2, fX2, fX1
	fxch	st(1)							; BZ, BZD, C2, fX2, fX1
	fst		DWORD PTR [ebx+40]				
	fadd	st(1), st						
	fst		DWORD PTR [ebx+44]				; BZ, BZD+BZ, C2, fX2, fX1
	fstp	DWORD PTR [ebx+48]				; BZD+BZ, C2, fX2, fX1
	fstp	DWORD PTR _gfBogusZ				; C2, fX2, fX1
	fxch	st(2)							; fX1, fX2, C2
	fistp	DWORD PTR [LnTemp2]				; lnTemp2 = INT32 fX1
	fld		DWORD PTR [ebp][fX]				; fX0, fX2, C2
	mov		eax, DWORD PTR [LnTemp2]			; eax = rX1 = lnTemp2
	fistp	DWORD PTR [LnTemp2]				; lnTemp2 = INT32 fX0
	jmp		z_complete_Strip					

not_bogus_z_Strip:
			
	fld		DWORD PTR _fMinZ		; fMinZ, Adj10, C2, fX2, fX1
	fld		DWORD PTR _fMinZ		; fMinZ, fMinZ, Adj10, C2, fX2, fX1
	fld		DWORD PTR _fMinZ		; fMinZ, fMinZ, fMinZ, Adj10, C2, fX2, fX1

	fxch	st(3)				; C2, fMinZ, fMinZ, fMinZ, fX2, fX1
	fld		DWORD PTR [ebp][fX]		; fX0, C2, fMinZ, fMinZ, fMinZ, fX2, fX1
	fxch	st(6)				; fX1, C2, fMinZ, fMinZ, fMinZ, fX2, fX0
	fistp	DWORD PTR [LnTemp2]		; lnTemp2 = INT32 fX1
	mov		eax, DWORD PTR [LnTemp2]		; eax = rX1 = lnTemp2
	mov		ecx, DWORD PTR _gPDC[pV0]	; ecx = pV0
	fxch	st(5)				; fX0, fMinZ, fMinZ, fMinZ, fX2, C2
	fistp	DWORD PTR [LnTemp2]		; lnTemp2 = INT32 fX0
	fld		DWORD PTR [ecx+12]		; pV0->fInvW, fMinZ, fMinZ, fMinZ, fX2, C2
	mov		ecx, DWORD PTR _gPDC[pV1]	; ecx = pV1
	fmulp	st(1), st			; fZ0, fMinZ, fMinZ, fX2, C2
	fstp	DWORD PTR [ebx+40]		; Store fZ0
	fmul	DWORD PTR [ecx+12]		; fZ1, fMinZ, fX2, C2
	mov		ecx, DWORD PTR _gPDC[pV2]	; ecx = pV2
	fstp	DWORD PTR [ebx+44]		; Store fZ1
	fld		DWORD PTR [ecx+12]		; pV2->fInvW, fMinZ, fX2, C2
	fmulp	st(1), st			; fZ2, fX2, C2
	fstp	DWORD PTR [ebx+48]		; Store fZ2

z_complete_Strip:

	fld		DWORD PTR [ebp][fY]		; fY0, fX2, C2
	fld		DWORD PTR [esi][fY]		; fY1, fY0, fX2, C2
	fxch	st(1)				; fY0, fY1, fX2, C2
	fistp	DWORD PTR [LfTemp3]		; LfTemp3 = INT32 fY0
	fistp	DWORD PTR [LfTemp6]		; LfTemp6 = INT32 fY1
	mov		edx, DWORD PTR [LnTemp2]		; edx = rX0 = lnTemp2
	fistp	DWORD PTR [LnTemp2]		; LnTemp2 = INT32 fX2
	fld		DWORD PTR [edi][fY]		; fY2
	mov		esi, _gPDC[Context][FirstXRegion]; esi = FirstXRegion
	fistp	DWORD PTR [LfTemp5]		; LfTemp5 = INT32 fY2 (dep1)
	fdiv	DWORD PTR [LfTemp1]		; C2/fDet

;
; FPU is now clear and not used any further (except for result of C2/fDet)
;	edx = INT32 fX1, eax = INT32 fX0, ebx = pTri
;	esi = FirstXRegion and edi, ebp, ecx are free
;
;	result =  ((Y0>>1)<<19) + ((Y1>>1)<<10) + (X0<<5) + X1
;
;	Y0 is min( INT32 fY0, INT32 fY1, INT32 fY2 )
;	Y1 is max( INT32 fY0, INT32 fY1, INT32 fY2 )
;	X0 is min( INT32 fX0, INT32 fX1, INT32 fX2 )
;	X1 is max( INT32 fX0, INT32 fX1, INT32 fX2 )
;
;	FirstX-LastX, FirstY-LastY need to clip X0-X1, Y0-Y1 but this is not
;       likely to happen often.
;
;	int 3


	mov		ecx, DWORD PTR [LShiftRegX]		; ecx = XShift
	mov		edi, DWORD PTR [LnTemp2]		; edi = fX2
	sar		edx, cl
	sar		eax, cl
	sar		edi, cl

	MinMaxUnsorted	edx, eax, edi

	mov		ah, dl

	; al: X1
	; ah: X0
	;
	; do Ys now

	mov		edi, DWORD PTR [LfTemp6]		; ecx = fY1
	mov		ebp, DWORD PTR [LfTemp3]		; edx = fY0
	shr		edi, 1
	mov		esi, DWORD PTR [LfTemp5]		; edi = fY2
	shr		ebp, 1
	shr		esi, 1

	MinMaxUnsorted	edi, ebp, esi

	; concatentate the triangles here
	; 
	; regs: 
	;		eax = X0:X1 (ah:al)
	;		ebx = pTri
	;		ecx	= 
	;		edx =
	;		esi = 
	;		edi = Y0
	;		ebp = Y1

	mov		edx, [pMat]
	mov		ecx, [NExtraV]
	mov		esi, [u2NextFace]
	mov		DWORD PTR [edx+60], 0; edx		; save number of extras
	or		ecx, ecx
	jz		NoPolys

	; we have some polys
	; advance pointers

	and		esi, 0FFFFh
	shl		esi, 5
	add		DWORD PTR _gPDC[pFace], 8		; edx = pFace
	add		esi, DWORD PTR _gPDC[pVertices]
	dec		DWORD PTR _gPDC[nPolys]			; eax = nPolys - 1

	; first face is always even

	fld		DWORD PTR [esi][fX]	
	fld		DWORD PTR [esi][fY]	
	fxch	st(1)
	fistp	DWORD PTR [LfTemp3]				; int fX
	mov		ecx, DWORD PTR [LShiftRegX]		; ecx = XShift
	fistp	DWORD PTR [LfTemp6]				; int fY

	mov		edx, DWORD PTR [LfTemp3]
	shr		edx, cl

	MinMaxSorted	ah, al, dl

	mov		edx, DWORD PTR [LfTemp6]
	shr		edx, 1

	MinMaxSorted	edi, ebp, edx
	
NoPolys:

	; regs: 
	;		eax = X0:X1 (al:ah)
	;		ebx = pTri
	;		ecx	= 
	;		edx =
	;		esi = 
	;		edi = Y0
	;		ebp = Y1

	test	DWORD PTR _gPDC[Context][bDoClipping], 0FFh
	jz		PackItAllUp

	; do a load of clipping
	
	ConstrainB ah, al, _gPDC[Context][FirstXRegion], _gPDC[Context][LastXRegion], P_MainLoop_Strip

	Constrain edi, ebp, _gPDC[Context][FirstYRegion], _gPDC[Context][LastYRegion], P_MainLoop_Strip

PackItAllUp:

	shrd	ecx, eax, 5
	shr		eax, 8
	shrd	ecx, eax, 5
	shrd	ecx, ebp, 9
	shr		eax, 16
	shrd	ecx, edi, 13

	mov		edi, DWORD PTR _gPDC[pV0]	; edi = pV0
	mov		esi, _gPDC[TSPControlWord]	; esi = TSPControlWord
	fstp	DWORD PTR [ebx+36]			; Store f1OverDet (every time)
	mov		edx, DWORD PTR [LpAddFn]	; edx = lpAddFn
	mov		DWORD PTR [ebx+52], ecx		; dwTypeRgn = ecx
	mov		DWORD PTR [ebx+56], esi		; pTri->TSPControlWord = TSP
	mov		eax, DWORD PTR [edi+16]		; eax = pV0->u32Color
	mov		DWORD PTR [ebx+60], eax		; pTri->BaseColor = eax
	test	edx, edx					; Flags = LpAddFn & lpAddFn
	je		@F
	mov		DWORD PTR [esp], ebx		; 620
	call	[LpAddFn]					; 0
@@:	add		ebx, PITRI_SIZE				; pTri += sizeof(ITRI)
	add		DWORD PTR [pMat], PIMAT_SIZE	; pMat += sizeof(IMATERIAL)
	mov		eax, [OFFSET FLAT: _gpTri][PITRI_SIZE*IBUFFERSIZE]
	mov		ecx, DWORD PTR [pEdge]		; save pEdge
	cmp		eax, ebx					; 623
	mov		DWORD PTR [pStartEdge], ecx
	jne		P_MainLoop_Strip			; PROB 95% MISP 0%	; 623

P_End_Strip:	

	add		esp, Adjust					; Discard Locals
	mov		eax, ebx					; eax = pTri
	sub		eax, OFFSET FLAT: _gpTri	; All over, eax = Tris stored
	sar		eax, 6						; eax /= sizeof(ITRI)
	
	pop		ebx							; Recover registers
	pop		ebp
	pop		esi
	pop		edi
	ret
	
	ALIGN	4
; mark_end;
_ProcessD3DStripCore ENDP

_TEXT	ENDS

; -- End  _ProcessD3DStripCore
endif

END
