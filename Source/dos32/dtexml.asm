DEBUG_ASSEMBLER EQU 0

;*****************************************************************************
;*++
;*  Name        :   $RCSfile: dtexml.asm,v $
;*  Title       :   
;*  ASM Author  :   Rob Isherwood
;*  Created     :   9/1/97
;*
;*  Copyright	: 1995-2022 Imagination Technologies (c)
;*  License		: MIT
;*
;*  Description :   Pentium optimised texture processing routines
;*
;*  Program Type:   ASM module (.dll)
;* 
;*  RCS info:
;*
;*  $Date: 1997/04/04 16:30:34 $
;*  $Revision: 1.2 $
;*  $Locker:  $
;*  $Log: dtexml.asm,v $
;; Revision 1.2  1997/04/04  16:30:34  jop
;; fixed textequ bug
;;
;; Revision 1.5  1997/01/30  19:24:20  rgi
;; Added the non MipMapped texture case
;;
;; Revision 1.4  1997/01/30  17:30:40  rgi
;; New MipMaped in order function, the non MipMapped is on
;; the way.
;;
;; Revision 1.3  1997/01/27  13:50:47  rgi
;; Removed the Context structure offset from gPDC description
;; this module never actually used it.
;;
;; Revision 1.2  1997/01/15  17:58:04  rgi
;; Fixed bug in non-mip mapped code, shows you what
;; happens if you stick your or in at the wrong moment.
;;
;; Revision 1.1  1997/01/15  13:23:49  rgi
;; Initial revision
;;
;* 
;* 	This was all produced from compiler output of dtex.c's version with
;*        the following options.
;* 
; -- Machine type P
; mark_description "Intel Reference  Release 5 Version 2.3.2 Beta B96061";
; mark_description "-DWIN32 -QWl,-force:multiple -QWl,-incremental:no -S -Fae:\\sabre\\sgl\\SGLMID4\\release.tmp\\dtex.asm -DFUL";
; mark_description "L_ON -DPCX1=1 -DD3DOPTIM=1 -DDDRAW -DWIN32=1 -DICL=1 -DVER_LIB_MAX=3 -DVER_LIB_MA=1 -DVER_LIB_MI=0 -DVER_LIB";
; mark_description "_MIN=0x0402 -DVER_LIB_TXT=\"3.1.0b2\" -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\. -Ie:\\sabre\\sgl\\SGLMID4\\so";
; mark_description "urce.tmp\\pcx -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\win32 -Ie:\\sabre\\SGL\\..\\vsgl -Gy -Zp4 -QIfdiv- -W0 ";
; mark_description "-nologo -Qpc32 -Qrct -Op- -DDLL_METRIC=1 -G5 -O2 -Oy -Qip -Qmem -Qscope_alloc -QW0,-mo_alternate_loops,-mo_b";
; mark_description "lock_loops,-mo_strip_mine -Oi";
;ident "Intel Reference  Release 5 Version 2.3.2 Beta B96061"
;ident "-DWIN32 -QWl,-force:multiple -QWl,-incremental:no -S -Fae:\sabre\sgl\SGLMID4\release.tmp\dtex.asm -DFULL_ON -DPCX1=1 -DD3DOPTI"
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

;Constants
WORDS_PER_PLANE		EQU	(3*4)
IEEE_MANTISSA_MASK	EQU	07FFFFFh
IEEE_ASSUMED_BIT	EQU	0800000h

;PITRI structure
Adj0		EQU	0
Adj1		EQU	12
Adj2		EQU	24
f1OverDet	EQU	36
fZ		EQU	40
PITRI_SIZE	EQU	64

;gPDC structure, we only know about stuff upto n32MipmapOffset
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

; Material structure, essentially the TEXDATA structure defined in dtri.h
Tex		EQU	0		; We know that Tex is first
u0		EQU	(Tex+ 0)
v0		EQU	(Tex+ 4)
w0		EQU	(Tex+ 8)
u1		EQU	(Tex+12)
v1		EQU	(Tex+16)
w1		EQU	(Tex+20)
u2		EQU	(Tex+24)
v2		EQU	(Tex+28)
w2		EQU	(Tex+32)
WBck		EQU	(Tex+36)	; And WriteBack follows on
Highlight	EQU	(WBck+0)
ModControl	EQU	(WBck+4)
ModBaseCol	EQU	(WBck+8)
Smooth1		EQU	(WBck+12)
Smooth2		EQU	(WBck+16)
LightVolCol	EQU	(WBck+20)

; TypeFlag bit masks passed as parameters
SMOOTHTEX	EQU	1
HIGHTEX		EQU	2
SMOOTHSHADTEX	EQU	4

_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
EXTRN	_gPDC:BYTE
_DATA	ENDS

_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'
       ALIGN     4
if DEBUG_ASSEMBLER
	PUBLIC   _PackTSPMipMapTexInAsm
_PackTSPMipMapTexInAsm	PROC NEAR
else
	PUBLIC   _PackTSPMipMapTex
_PackTSPMipMapTex	PROC NEAR
endif
	Lcompress	TEXTEQU <esp+  0>
	Lintconvt0	TEXTEQU <esp+  4>
		Lint_v0		TEXTEQU <esp+  4>
	Lintconvt1	TEXTEQU <esp+  8>
		Lint_w0		TEXTEQU <esp+  8>
	Lsw2		TEXTEQU <esp+ 12>
	LintconvPR	TEXTEQU <esp+ 16>
		Lint_w1		TEXTEQU <esp+ 16>
	LlargestT	TEXTEQU <esp+ 20>
		Lint_v2		TEXTEQU <esp+ 20>
	LTopExp		TEXTEQU <esp+ 24>
	Lsv2		TEXTEQU <esp+ 28>
	LintconvAF	TEXTEQU <esp+ 32>
		Lint_v1		TEXTEQU <esp+ 32>
	Lsu2		TEXTEQU <esp+ 36>
		Lint_w2		TEXTEQU <esp+ 36>
	LlargestB	TEXTEQU <esp+ 40>
		Lint_u2		TEXTEQU <esp+ 40>
	Ldist0		TEXTEQU <esp+ 44>
		Lint_u0 	TEXTEQU <esp+ 44>
	Ldist1		TEXTEQU <esp+ 48>
		Lint_u1		TEXTEQU <esp+ 48>
	LMipExp		TEXTEQU <esp+ 60>
	Adjust		TEXTEQU	<68>
	SaveEBX		TEXTEQU <esp+  0+Adjust>
	SaveEBP		TEXTEQU <esp+  4+Adjust>
	SaveESI		TEXTEQU <esp+  8+Adjust>
	SaveEDI		TEXTEQU <esp+ 12+Adjust>
	RetAddr		TEXTEQU <esp+ 16+Adjust>
	pMat		TEXTEQU <esp+ 20+Adjust>
	PnPolys		TEXTEQU <esp+ 24+Adjust>
	TypeFlag	TEXTEQU <esp+ 28+Adjust>
	TSPIncrement	TEXTEQU <esp+ 32+Adjust>
	pTSP		TEXTEQU <esp+ 36+Adjust>
	nMatInc		TEXTEQU <esp+ 40+Adjust>

	push	edi				; Save Registers
	push	esi
	push	ebp
	push	ebx
	mov	eax, DWORD PTR [esp+24]		; ebp = PnPolys
	mov	esi, DWORD PTR [esp+20]		; ESI = pMat
	mov	edi, DWORD PTR [esp+36]		; EDI = pTSP
	sub	esp, Adjust
	or	eax,eax				; Flags = eax | eax
	jz	MMT_Done			; If (PnPolys == 0) MMT_Done
MMT_Loop:
;
; Calculate the input for the greatest pythagorian distance calculation
;
	fld	DWORD PTR [esi][w2]		; w2
	fmul	DWORD PTR [esi][v0]		; w2*v0
	fld	DWORD PTR [esi][w0]		; w0,w2*v0
	fmul	DWORD PTR [esi][v2]		; w0*v2,w2*v0
	fld	DWORD PTR [esi][w2]		; w2,w0*v2,w2*v0
	fmul	DWORD PTR [esi][u0]		; w2*u0,w0*v2,w2*v0
	fxch	st(1)				; w0*v2,w2*u0,w2*v0
	fsubp	st(2),st			; w2*u0,w2*v0-w0*v2
	fld	DWORD PTR [esi][w0]		; w0,w2*u0,w2*v0-w0*v2
	fmul	DWORD PTR [esi][u2]		; w0*u2,w2*u0,w2*v0-w0*v2
	fld	DWORD PTR [esi][w2]		; w2,w0*u2,w2*u0,w2*v0-w0*v2
	fmul	DWORD PTR [esi][v1]		; w2*v1,w0*u2,w2*u0,w2*v0-w0*v2
	fxch	st(3)				; w2*v0-w0*v2,w0*u2,w2*u0,w2*v1
	fstp	DWORD PTR [Lintconvt1]		; intconvt1=w2*v0-w0*v2,w0*u2,w2*u0,w2*v1
	fsubp	st(1),st			; 1: w2*u0-w0*u2,w2*v1
	fld	DWORD PTR [esi][w1]		; w1,w2*u0-w0*u2,w2*v1
	fmul	DWORD PTR [esi][v2]		; w1*v2,w2*u0-w0*u2,w2*v1
	fld	DWORD PTR [esi][w2]		; w2,w1*v2,w2*u0-w0*u2,w2*v1
	fmul	DWORD PTR [esi][u1]		; w2*u1,w1*v2,w2*u0-w0*u2,w2*v1
	fxch	st(2)				; w2*u0-w0*u2,w1*v2,w2*u1,w2*v1
	fstp	DWORD PTR [Lintconvt0]		; intconvt0=w2*u0-w0*u2,w1*v2,w2*u1,w2*v1
	fsubp	st(2),st			; w2*u1,w2*v1-w1*v2
	fld	DWORD PTR [esi][w1]		; w1,w2*u1,w2*v1-w1*v2
	fmul	DWORD PTR [esi][u2]		; w1*u2,w2*u1,w2*v1-w1*v2
	fld	DWORD PTR [esi][u2]		; u2,w1*u2,w2*u1,w2*v1-w1*v2
	fmul	DWORD PTR _2_3_2il2floater_9	; su2,w1*u2,w2*u1,w2*v1-w1*v2
	fxch	st(3)				; w2*v1-w1*v2,w1*u2,w2*u1,su2
	fstp	DWORD PTR [Ldist1]		; dist1=,w1*u2,w2*u1,su2
	fsubp	st(1),st			; w2*u1-w1*u2,su2
	fld	DWORD PTR [esi][v2]		; v2,w2*u1-w1*u2,su2
	fmul	DWORD PTR _2_3_2il2floater_9	; sv2,w2*u1-w1*u2,su2
	fld	DWORD PTR [esi][w2]		; w2,sv2,w2*u1-w1*u2,su2
	fmul	DWORD PTR _2_3_2il2floater_9	; sw2,sv2,w2*u1-w1*u2,su2
	fxch	st(2)				; w2*u1-w1*u2,sv2,sw2,su2
	fstp	DWORD PTR [Ldist0]		; dist0=w2*u1-w1*u2,sv2,sw2,su2
	fstp	DWORD PTR [Lsv2]		; Lsv2=,sw2,su2
	fstp	DWORD PTR [Lsw2]		; Lsw2=,su2
	fstp	DWORD PTR [Lsu2]		; Lsu2=
;
; Calculate largestT = max(fabs(u0),fabs(u1),fabs(su2),
;				fabs(v0),fabs(v1),fabs(sv2)).l
;
       mov	edx,07fffffffh			; edx = fabs mask
	mov	eax,DWORD PTR [esi][u0]		; eax = u0.l
       mov	ecx,edx				; ecx = fabs mask
	mov	 ebx,edx			; ebx = fabs mask
       and	ecx,DWORD PTR [esi][u1]		; ecx = fabs(u1).l
	and	eax,edx				; eax = fabs(u0).l
       cmp	eax,ecx				; compare with current max
	jnc	@F				; keep current if no change
       mov	eax,ecx
@@:    and	ebx,DWORD PTR [Lsu2]		; ebx = fabs(su2).l
	mov	ecx,edx				; ecx = fabs mask
       cmp	eax,ebx				; keep current if no change
	jnc	@F
       mov	eax,ebx
@@:    and	ecx,DWORD PTR [esi][v0]		; ecx = fabs(v0).l
	mov	ebx,edx				; ebx = fabs mask
       cmp	eax,ecx				; keep current if no change
	jnc	@F
       mov	eax,ecx
@@:    and	ebx,DWORD PTR [esi][v1]		; ebx = fabs(v1).l
	mov	ecx,edx				; ecx = fabs mask
       cmp	eax,ebx				; keep current if no change
	jnc	@F
       mov	eax,ebx
@@:    and	ecx,DWORD PTR [Lsv2]		; ecx = fabs(sv2).l
; V slot free here if you can find something to put in it
       cmp	eax,ecx				; keep current if no change
	jnc	@F				
       mov	eax,ecx
@@:    mov	DWORD PTR [LlargestT],eax	; 954
	mov	ebp,65535			; ebp = 65535 for later
	fld	DWORD PTR _2_3_2il2floater_24	; 1.0f
	fdiv	DWORD PTR [LlargestT]		; 1.0f/largestT in 36 cycles
;
; Data is setup to do as much pure integer code at this point as possible,
; theory says we are getting free cycles if we do integer stuff while the FPU
; is busy.
;
; Ensure intconvt0=min(fabs(intconvt0),fabs(intconvt1)) and intconvt1 is max
;
       mov	eax,DWORD PTR [Lintconvt1]	; eax = (w2*v0-w0*v2).l
	mov	ebx,edx				; ebx = fabs mask
       and	eax,edx				; eax = fabs(w2*v0-w0*v2).l
	and	ebx,DWORD PTR [Lintconvt0]	; ebx = fabs(w2*u0-w0*u2).l
       cmp	ebx,eax				; Flags = ebx - eax
	jbe	@F				; if intconvt0<=intconvt1 @@
       mov	DWORD PTR [Lintconvt0],eax	; swap fabs intconvt0 and 
	mov	DWORD PTR [Lintconvt1],ebx	; fabs intconvt1 for later
       jmp	MMT_Cont1	
@@:    mov	DWORD PTR [Lintconvt0],ebx	; just fabs intconvt0 and
	mov	DWORD PTR [Lintconvt1],eax	; fabs intconvt1 for later
MMT_Cont1:
;
; Ensure dist0=min(fabs(dist0),fabs(dist1)) and dist1 is max
;
       mov	eax,DWORD PTR [Ldist1]		; eax = (w2*v0-w0*v2).l
	mov	ebx,edx				; ebx = fabs mask
       and	eax,edx				; eax = fabs(w2*v0-w0*v2).l
	and	ebx,DWORD PTR [Ldist0]		; edx = fabs(w2*u0-w0*u2).l
       cmp	ebx,eax				; Flags = ebx - eax
	jbe	@F				; If intconvt0 < intconvt1 @@
       mov	DWORD PTR [Ldist0],eax		; swap fabs dist0 and
	mov	DWORD PTR [Ldist1],ebx		; fabs dist1 for later
       jmp	MMT_Cont2	
@@:    mov	DWORD PTR [Ldist0],ebx		; just fabs dist0 and
	mov	DWORD PTR [Ldist1],eax		; fabs dist1 for later
MMT_Cont2:
;
; Calculate largestB = max(fabs(w0),fabs(w1),fabs(sw2))
;
       mov	ebx,DWORD PTR [esi][w1]		; ebx = w1.l
	mov	eax,edx				; eax = fabs mask
       and	eax,DWORD PTR [esi][w0]		; eax = fabs(w0).l
	and	ebx,edx				; ebx = fabs(w1).l
       cmp	ebx,eax				; Flags = ebx - eax
       	jbe	@F				; If ebx <= eax keep eax else
       mov	eax,ebx				; eax = fabs(w1).l
@@:    and	edx,DWORD PTR [Lsw2]		; edx = fabs(sw2).l
        xor	ebx,ebx				; ebx = 0 (for later)
       cmp	edx,eax				; Flags = fabs(sw2).l - eax
        jbe	@F				; If edx <= eax keep eax else
       mov	eax,edx				; eax = fabs(sw2).l
@@:    mov	DWORD PTR [LlargestB],eax	; largestB = eax
;
; Do initialisation of intconvAF, intconvPR, and MipExp
;
	mov	eax, 04B400000h			; eax = MAGIC_FLOAT TO INT VALUE
       mov	DWORD PTR [LintconvAF], eax	; intconvAF.l = eax
	mov	DWORD PTR [LintconvPR], eax	; intconvPR.l = eax
;
; Do the remaining FPU stuff to calculate the two pythagoran distances
;	
	fld	DWORD PTR [Lintconvt0]		; intconvt0, 1.0f/largestT
	fmul	DWORD PTR _2_3_2il2floater_3	; intcontt0 * (c1=(1.0f-0.585786f))
	fxch	st(1)
	fstp	DWORD PTR [LlargestT]		; Store result of divide
	fadd	DWORD PTR [Lintconvt1]		; intcontt0 * c1 + intconvt1
	fld	DWORD PTR [Ldist0]		; dist0
	fmul	DWORD PTR _2_3_2il2floater_3	; dist0 * c1
;
; Floating point calculates TopExp and largestT values (in parallel)
;
	fld	DWORD PTR [LlargestB]		; largestB,dist0*c1,ndist0
	fmul	DWORD PTR [LlargestT]		; largestT*largestB,dist0*c1,ndist0
	fxch	st(1)				; dist0*c1,lT*lB,ndist0
	fadd	DWORD PTR [Ldist1]		; dist0*c1+dist1,lT*lB,ndist0
	fxch	st(2)				; ndist0,lT*lB,dist0*c1+dist1
	fstp	DWORD PTR [Ldist0]		; dist0=,lT*lB,dist0*c1+dist1
	fld	DWORD PTR [LlargestT]		; lT,lT*lB,dist0*c1+dist1
	fmul	DWORD PTR _2_3_2il2floater_31	; lT*c2,lT*lB,dist0*c1+dist1
	fxch	st(2)				; dist0*cl+dist1,lT*lB,lT*c2
	fstp	DWORD PTR [Ldist1]		; dist1=,lT*lB,lT*c2
	fstp	DWORD PTR [LTopExp]		; TopExp = lT*lB
	fstp	DWORD PTR [LlargestT]		; largestT = lT*c2
;
; Integer code ensures that dist0 is the biggest pythagoran distance
;
       mov	DWORD PTR [LMipExp],ebx		; MipExp = 0
	mov	ebx, DWORD PTR [Ldist1]		; ebx = dist1
       cmp	ebx, DWORD PTR [Ldist0]		; Flags = dist1 - dist0
	jle	@F				; If dist1 < dist0 keep dist0
       mov	DWORD PTR [Ldist0],ebx		; dist0 = max(dist1,dist0)
@@:
;
; Setup exp (ebx) as 126 - ( (TopExp>>23) & 0ffh ) and then
;       TopExp as (exp+127)<<23
;
	mov	ebx, 126			; ebx = 126
	 mov	ecx, DWORD PTR [LTopExp]	; ecx = TopExp.l (always +ive)
	shr	ecx, 23				; ecx = exp = (TopExp.l>>23)
	 mov	eax, 127+126			; eax = 127+126
	sub	ebx, ecx			; ebx = exp = 126-exp
	 sub	eax, ecx			; eax = 127+126-exp
	shl	eax, 23				; eax = (127+(126-exp))<<23
	 cmp	ebx,15				; Flags = ebx - 15
	mov	DWORD PTR [LTopExp], eax	; TopExp.l = eax
	 jbe	MMT_Store			; If 0 <= exp.u <= 15 MMT_Store
;
; The code below does not happen very often
;
	jg	MMT_Big_Exp			; If exp.l > 15 MMT_Big_Exp
MMT_Neg_Exp:
       shl	ebx,23				; ebx = exp<<23
	mov	eax,DWORD PTR [LintconvAF]	; eax = intconvAF
       sub	eax,ebx				; eax = intconvAF - (exp<<23)
	xor	ebx, ebx			; ebx = exp = 0
       mov	DWORD PTR [LintconvAF], eax	; intconvAF = eax
	jmp	MMT_Store			; goto MMT_Store
MMT_Big_Exp:
       shl	ebx,23				; ebx = exp<<23
	mov	eax,4B400000h+((-15)*00800000h)	; eax = MV+(-15)<<23
       add	eax,ebx				; eax = MV+(exp-15)<<23
	mov	ebx,15				; ebx = 15
       mov	DWORD PTR [LintconvPR], eax	; intconvPR = eax
        sub	eax,4B400000h			; eax = ((exp-15)<<23)
       shr	eax,22				; eax = ((exp-15)<<1)
       neg	eax				; eax = -((exp-15)<<1)
       mov	DWORD PTR [LMipExp],eax		; MipExp = -((exp-15)<<1)
MMT_Store:
;
; Here we go for the big calc of the output data
;
	fld	DWORD PTR [LlargestT]	; lT
	fld	st(0)			; lT,lT
	fld	st(0)			; lT,lT,lT
	fmul	DWORD PTR [Lsu2]	; su2*lT,lT,lT
	fld	st(1)			; lT,su2*lT,lT,lT
	fmul	DWORD PTR [Lsv2]	; sv2*lT,su2*lT,lT,lT
	fxch	st(1)			; su2*lT,sv2*lT,lT,lT
	fadd	DWORD PTR [LintconvAF]	; I_su2*lT,sv2*lT,lT,lT
; Add 1 integer cycle here (see below)
; The real crux is how we do up to TSP[2] which is a complex expression
; based on lots of recently calculated values-
;
;      pTSP[0] = (exp<<18) | pMat->WriteBack.ModControl;
;      pTSP[1] = pMat->WriteBack.ModBaseCol;
;		MipM =((compress>>16)&0x7f)+128;
;		MipE = (compress>>23) + gPDC.n32MipmapOffset + MipExp - 126
;      pTSP[2] = (MipM << (SHIFT_PMIP_M=24)) |
;		 (MipE << (SHIFT_PMIP_E=18)) | (w2 & 0xffffUL);
;
; We re-arrange code to require w2 and compress as late as possible. So we
; do all possible preparation first and try to reduce the shifts required as
; they only can execute in U pipe-
;
;      pTSP[0] = (exp<<18) | pMat->WriteBack.ModControl;
;      pTSP[1] = pMat->WriteBack.ModBaseCol;
;      pTSP[2]= ( (gPDC.n32MipMapOffset + MipExp)<<18 ) +
;		( (128<<24)-(126<<18) )                 +
;		( (compress & (127<<16))<<(24-16) )     +
;               ( (compress & (255<<23))>>(23-18)       +
;		( w2 & (ebp=ffffUL) )                     ;
;

       shl	ebx,18				; ebx <<= 18
        mov	ecx,DWORD PTR [esi][ModControl] ; ecx = pMat->WriteBack.ModControl
	fld	st(2)			; lT,I_su2*lT,sv2*lT,lT,lT
	fxch	st(2)			; sv2*lT,I_su2*lT,lT,lT,lT
	fadd	DWORD PTR [LintconvAF]	; I_sv2*lT,I_su2*lT,lT,lT,lT
	fxch	st(1)			; I_su2*lT,I_sv2*lT,lT,lT,lT
; Stall of 1 unless we use integer after the fadd above
	fstp	DWORD PTR [Lint_u2]	; int_u2=,I_sv2*lT,lT,lT,lT
	fld	st(1)			; lT,I_sv2*lT,lT,lT,lT,lT
	fld	st(2)			; lT,lT,I_sv2*lT,lT,lT,lT
	fmul	DWORD PTR [LTopExp]	; lBT=lT*TopExp,lT,I_sv2*lT,lT,lT,lT
	fld	DWORD PTR [Ldist0]	; dist0,lBT,lT,I_sv2*lT,lT,lT,lT
; st(0) stall 1 (but can use integer)
       mov	edx, DWORD PTR [LMipExp]	; edx = MipExp
        or	ebx,ecx				; ebx |= ecx (ModControl)
  	fmul	st, st(1)		; dist0*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
	fld	DWORD PTR [Lsw2]	; sw2,dist0*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(2)			; lBT,dist0*lBT,sw2,lT,I_sv2*lT,lT,lT,lT
	fmul	st(2), st		; lBT,dist0*lBT,sw2*lBT,lT,I_sv2*lT,lT,lT,lT
; mul stall 1 (but can use integer)
       mov	ecx,0007f0000h			; ecx = Preloaded (127<<16) val
        mov	eax, DWORD PTR _gPDC[n32MipmapOffset]; eax = gPDC.n32MipmapOffset
	fmul	st(1), st		; lBT,dist0*lBT*lBT,sw2*lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(2)			; sw2*1BT,dist0*lBT*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
	fadd	DWORD PTR [LintconvPR]	; I_sw2*lBT,dist0*lBT*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
; fstp stall 3 (but can use integer)
       add	edx,eax				; edx += MipMapOffset
        mov	DWORD PTR [edi], ebx		; pTSP[0] = ebx
       shl	edx, 18
        mov	ebx,DWORD PTR [esi][ModBaseCol] ; eax = pMat->WriteBack.ModBaseCol
       add	edx, 080000000h-(126*040000h)	; edx += (128<<24)-(126<<18)
        mov	DWORD PTR [edi+4], ebx		; pTSP[1] = ebx
	fstp	DWORD PTR [Lint_w2]	; int_w2=,dist0*lBT*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
	fld	DWORD PTR [esi][w0]	; w0,dist0*lBT*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(2)			; lBT,dist0*lBT*lBT,w0,lT,I_sv2*lT,lT,lT,lT
	fmul	st(2),st		; lBT,dist0*lBT*lBT,w0*lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(1)			; dist0*lBT*lBT,lBT,w0*lBT,lT,I_sv2*lT,lT,lT,lT
	fstp	DWORD PTR [Lcompress]	; compress=,1BT,w0*lBT,lT,I_sv2*lT,lT,lT,lT
	fmul	DWORD PTR [esi][w1]	; w1*lBT,w0*lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(1)			; w0*lBT,w1*lBT,lT,I_sv2*lT,lT,lT,lT
	fadd	DWORD PTR [LintconvPR]	; I_w0*lBT,w1*lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(6)			; lT,w1*lBT,lT,I_sv2*lT,lT,lT,I_w0*lBT
	fmul	DWORD PTR [esi][u0]	; u0*lT,w1*lBT,lT,I_sv2*lT,lT,lT,I_w0*lBT
	fxch	st(3)			; I_sv2*lT,w1*lBT,lT,u0*lT,lT,lT,I_w0*lBT
	fstp	DWORD PTR [Lint_v2]	; int_v2=,w1*lBT,lT,u0*lT,lT,lT,I_w0*lBT
	fadd	DWORD PTR [LintconvPR]	; I_w1*1BT,lT,u0*lT,lT,lT,I_w0*lBT
	fxch	st(5)			; I_w0*lBT,lT,u0*lT,lT,lT,I_w1*lBT
	fstp	DWORD PTR [Lint_w0]	; int_w1=,lT,u0*lT,lT,lT,I_w1*lBT
	fmul	DWORD PTR [esi][u1]	; u1*lT,u0*lT,lT,lT,I_w1*lBT
	fxch	st(4)			; I_w1*lBT,u0*lT,lT,lT,u1*lT
	fstp	DWORD PTR [Lint_w1]	; int_w1=,u0*lT,lT,lT,u1*lT
	fadd	DWORD PTR [LintconvAF]	; I_u0*lT,lT,lT,u1*lT
; Stall of 3 but can use integer
       mov	eax, DWORD PTR [Lint_w2]	; eax = int_w2
        mov	ebx, DWORD PTR [Lcompress]	; ebx = compress
       and	eax,ebp				; eax &= 65535
        and	ecx,ebx				; ecx = compress & (127<<16)
       shl	ecx,24-16			; ecx <<= SHIFT_PMIP_M-16
        and	ebx,255*0800000h		; ebx &= 255<<23
	fstp	DWORD PTR [Lint_u0]	; int_u0=,lT,lT,u1*lT
	fmul	DWORD PTR [esi][v1]	; v1*lT,lT,u1*lT
	fxch	st(2)			; u1*lT,lT,v1*lT
	fadd	DWORD PTR [LintconvAF]	; I_u1*lT,lT,v1*lT
	fxch	st(1)			; lT,I_u1*lT,v1*lT
	fmul	DWORD PTR [esi][v0]	; v0*lT,I_u1*lT,v1*lT
; Stall of 2 but can do integer	
       shr	ebx,23-18			; ebx = (compress>>23)<<18
        add	edx,eax
       mov	eax,DWORD PTR [Lint_w1]		; eax = int_w1
	add	edx,ecx				; edx += ecx
	fadd	DWORD PTR [LintconvAF]	; I_v0*lT,I_u1*lT,v1*lT
	fxch	st(1)			; I_u1*lT,I_v0*lT,v1*lT
	fstp	DWORD PTR [Lint_u1]	; int_u1=,I_v0*lT,v1*lT
; Stall of 1 but can do integer
       shl	eax,16				; eax <<= 16
        add	ebx,edx				; ebx += edx
	fstp	DWORD PTR [Lint_v0]	; int_v0=,v1*lT
	fadd	DWORD PTR [LintconvAF]	; I_v1*lT
; stall of 3 but can use integer
       mov	DWORD PTR [edi+8], ebx		; pTSP[2] = ebx
        mov	ebx,DWORD PTR [Lint_w0]		; ebp = w0
       and	ebx,ebp				; ebx &= w0&0ffffh
	mov	edx,DWORD PTR _gPDC[TexAddress]	; edx = gPDC.TexAddress
       shl	edx,16				; edx = gPDC.TexAddress << 16
        or	eax,ebx				; eax = (w1<<16) | (w0&0ffffh)
	fstp	DWORD PTR [Lint_v1]	; int_v1=

       mov	ebx,DWORD PTR [Lint_u2]		; eax = INT32 su2*largT
        mov	DWORD PTR [edi+12], eax		; pTSP[3] = eax
       and	ebx,ebp				; eax &= 65535
        mov	eax,DWORD PTR [Lint_u0]		; edx = int_u0
       or	edx,ebx				; edx = (gPDC.TexAddress << 16) | eax
        mov	ebx, DWORD PTR [Lint_u1]	; ebx = int_u1
       shl	ebx,16				; ebx <<= 16
        and	eax,ebp				; edx &= 65535
       mov	DWORD PTR [edi+16], edx		; pTSP[4] = edx
        or	ebx,eax				; ebx = (u1<<16) | (u0&0ffffh)
       mov	eax,DWORD PTR _gPDC[TexAddress]	; eax = gPDC.TexAddress
        mov	DWORD PTR [edi+20], ebx		; pTSP[5] = ebx
       mov	ebx,DWORD PTR [Lint_v2]		; ebx = int_v2
        and	eax,-65536			; eax &= 0ffff0000h
       and	ebx,ebp				; ebx &= 65535
        mov	edx, DWORD PTR [Lint_v1]	; edx = int_v1
       shl	edx, 16				; edx <<= 16
        or	eax,ebx				; eax |= (v2&0ffffh)
       mov	DWORD PTR [edi+24], eax		; pTSP[6] = eax
        mov	ebx, DWORD PTR [Lint_v0]	; ebx = v0
       mov	eax, DWORD PTR [TypeFlag]	; eax = TypeFlag
        and	ebp,ebx				; ebp = v0 & 65535
       mov	ebx, DWORD PTR [PnPolys]	; ecx = PnPolys
        or	edx,ebp				; edx = (v1<<16) | (v0&0ffffh)
       mov	DWORD PTR [edi+28], edx		; pTSP[7] = edx
        mov	edx, DWORD PTR [TSPIncrement]	; edx = TSPIncrement
       shr	eax,1
        jc	MMT_FlatShade
       jz	MMT_NoExtras
       shr	eax,1
        jnc	MMT_SmoothSTex
       mov	eax,DWORD PTR [esi][Highlight]	; eax = pMat->WriteBack.Highlight
	mov	DWORD PTR [edi+32],eax		; pTSP[8] = eax
        jmp	MMT_NoExtras
       
MMT_FlatShade:
       mov	eax,DWORD PTR [esi][Smooth1]	; eax = pMat->WriteBack.Smooth1
        mov	ecx,DWORD PTR [esi][Smooth2]	; ecx = pMat->WriteBack.Smooth2
       mov	DWORD PTR [edi+32],eax		; pTSP[8] = eax
        mov	DWORD PTR [edi+36],ecx		; pTSP[9] = ecx
	jmp	MMT_NoExtras
	
MMT_SmoothSTex:					; SMOOTHSHADTEX
       mov	eax,DWORD PTR [esi][Smooth1]	; eax |= pMat->WriteBack.Smooth1
        mov	ecx,DWORD PTR [esi][Smooth2]	; ecx |= pMat->WriteBack.Smooth2
       mov	DWORD PTR [edi+32],eax		; pTSP[8] = eax
        mov	eax,DWORD PTR [esi][LightVolCol]; eax |= pMat->v.LightVolCol
       mov	DWORD PTR [edi+36],ecx		; pTSP[9] = ecx
        mov	ecx,DWORD PTR [esi][Highlight]	; ecx = pMat->WriteBack.Highlight
       mov	DWORD PTR [edi+40],eax		; pTSP[10] = eax
        mov	DWORD PTR [edi+44],ecx		; pTSP[11] = ecx
MMT_NoExtras:
       mov	eax, DWORD PTR [nMatInc]	; eax = nMatInc
        lea	edi, DWORD PTR [edi+edx*4]	; edi += TSPIncrement
       add	esi,eax				; ESI += eax (nMatInc)
        dec	ebx				; ebx = PnPolys - 1
       mov	DWORD PTR [PnPolys], ebx	; PnPolys = ebx
        jnz	MMT_Loop			; If PnPolys == 0 then Loop
MMT_Done:
	add	esp, Adjust			; Lose locals
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret
	ALIGN	4
if DEBUG_ASSEMBLER
_PackTSPMipMapTexInAsm ENDP
else
_PackTSPMipMapTex ENDP
endif
_TEXT	ENDS

_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'
       ALIGN     4
if DEBUG_ASSEMBLER
	PUBLIC   _PackTSPTexInAsm
_PackTSPTexInAsm	PROC NEAR
else
	PUBLIC   _PackTSPTex
_PackTSPTex	PROC NEAR
endif
	push	edi				; Save Registers
	push	esi
	push	ebp
	push	ebx
	mov	eax, DWORD PTR [esp+24]		; ebp = PnPolys
	mov	esi, DWORD PTR [esp+20]		; ESI = pMat
	mov	edi, DWORD PTR [esp+36]		; EDI = pTSP
	sub	esp, Adjust
	or	eax,eax				; Flags = eax | eax
	jz	TEX_Done			; If (PnPolys == 0) TEX_Done
TEX_Loop:
;
; Scale Z values only
;
	fld	DWORD PTR [esi][u2]		; u2
	fmul	DWORD PTR _2_3_2il2floater_9	; su2
	fld	DWORD PTR [esi][v2]		; v2,su2
	fmul	DWORD PTR _2_3_2il2floater_9	; sv2,su2
	fld	DWORD PTR [esi][w2]		; w2,sv2,su2
	fmul	DWORD PTR _2_3_2il2floater_9	; sw2,sv2,su2
	fxch	st(2)				; su2,sv2,sw2
	fstp	DWORD PTR [Lsu2]		; Lsu2=,sv2,sw2
	fstp	DWORD PTR [Lsv2]		; Lsv2=,sw2
	fstp	DWORD PTR [Lsw2]		; Lsw2=,
;
; Calculate largestT = max(fabs(u0),fabs(u1),fabs(su2),
;				fabs(v0),fabs(v1),fabs(sv2)).l
;
       mov	edx,07fffffffh			; edx = fabs mask
	mov	eax,DWORD PTR [esi][u0]		; eax = u0.l
       mov	ecx,edx				; ecx = fabs mask
	mov	 ebx,edx			; ebx = fabs mask
       and	ecx,DWORD PTR [esi][u1]		; ecx = fabs(u1).l
	and	eax,edx				; eax = fabs(u0).l
       cmp	eax,ecx				; compare with current max
	jnc	@F				; keep current if no change
       mov	eax,ecx
@@:    and	ebx,DWORD PTR [Lsu2]		; ebx = fabs(su2).l
	mov	ecx,edx				; ecx = fabs mask
       cmp	eax,ebx				; keep current if no change
	jnc	@F
       mov	eax,ebx
@@:    and	ecx,DWORD PTR [esi][v0]		; ecx = fabs(v0).l
	mov	ebx,edx				; ebx = fabs mask
       cmp	eax,ecx				; keep current if no change
	jnc	@F
       mov	eax,ecx
@@:    and	ebx,DWORD PTR [esi][v1]		; ebx = fabs(v1).l
	mov	ecx,edx				; ecx = fabs mask
       cmp	eax,ebx				; keep current if no change
	jnc	@F
       mov	eax,ebx
@@:    and	ecx,DWORD PTR [Lsv2]		; ecx = fabs(sv2).l
; V slot free here if you can find something to put in it
       cmp	eax,ecx				; keep current if no change
	jnc	@F				
       mov	eax,ecx
@@:    mov	DWORD PTR [LlargestT],eax	; 954
	mov	ebp,65535			; ebp = 65535 for later
	fld	DWORD PTR _2_3_2il2floater_24	; 1.0f
	fdiv	DWORD PTR [LlargestT]		; 1.0f/largestT in 36 cycles
;
; Data is setup to do as much pure integer code at this point as possible,
; theory says we are getting free cycles if we do integer stuff while the FPU
; is busy.
;
; Calculate largestB = max(fabs(w0),fabs(w1),fabs(sw2))
;
       mov	ebx,DWORD PTR [esi][w1]		; ebx = w1.l
	mov	eax,edx				; eax = fabs mask
       and	eax,DWORD PTR [esi][w0]		; eax = fabs(w0).l
	and	ebx,edx				; ebx = fabs(w1).l
       cmp	ebx,eax				; Flags = ebx - eax
       	jbe	@F				; If ebx <= eax keep eax else
       mov	eax,ebx				; eax = fabs(w1).l
@@:    and	edx,DWORD PTR [Lsw2]		; edx = fabs(sw2).l
	mov	ebx, 04B400000h			; ebx = MAGIC_FLOAT TO INT VALUE
       cmp	edx,eax				; Flags = fabs(sw2).l - eax
        jbe	@F				; If edx <= eax keep eax else
       mov	eax,edx				; eax = fabs(sw2).l
@@:    mov	DWORD PTR [LlargestB],eax	; largestB = eax
;
; Do initialisation of intconvAF, intconvPR, and MipExp
;
       mov	DWORD PTR [LintconvAF], ebx	; intconvAF.l = ebx
;
; Floating point calculates TopExp and largestT values
;
	fld	st(0)				; largestT,largestT
	fmul	DWORD PTR _2_3_2il2floater_31	; largestT*c2,largestT
	fld	DWORD PTR [LlargestB]		; largestB,largestT*c2,largestT
	fmulp	st(2), st			; largestT*c2,largestT*largestB
	mov	DWORD PTR [LintconvPR], ebx	; intconvPR.l = ebx
	fstp	DWORD PTR [LlargestT]		; largestT = lT*c2
	fstp	DWORD PTR [LTopExp]		; TopExp = lT*lB
;
; Setup exp (ebx) as 126 - ( (TopExp>>23) & 0ffh ) and then
;       TopExp as (exp+127)<<23
;
	mov	ebx, 126			; ebx = 126
	 mov	ecx, DWORD PTR [LTopExp]	; ecx = TopExp.l (always +ive)
	shr	ecx, 23				; ecx = exp = (TopExp.l>>23)
	 mov	eax, 127+126			; eax = 127+126
	sub	ebx, ecx			; ebx = exp = 126-exp
	 sub	eax, ecx			; eax = 127+126-exp
	shl	eax, 23				; eax = (127+(126-exp))<<23
	 cmp	ebx,15				; Flags = ebx - 15
	mov	DWORD PTR [LTopExp], eax	; TopExp.l = eax
	 jbe	TEX_Store			; If 0 <= exp.u <= 15 TEX_Store
;
; The code below does not happen very often
;
	jg	TEX_Big_Exp			; If exp.l > 15 TEX_Big_Exp
TEX_Neg_Exp:
       shl	ebx,23				; ebx = exp<<23
	mov	eax,DWORD PTR [LintconvAF]	; eax = intconvAF
       sub	eax,ebx				; eax = intconvAF - (exp<<23)
	xor	ebx, ebx			; ebx = exp = 0
       mov	DWORD PTR [LintconvAF], eax	; intconvAF = eax
	jmp	TEX_Store			; goto TEX_Store
TEX_Big_Exp:
       shl	ebx,23				; ebx = exp<<23
	mov	eax,4B400000h+((-15)*00800000h)	; eax = MV+(-15)<<23
       add	eax,ebx				; eax = MV+(exp-15)<<23
	mov	ebx,15				; ebx = 15
       mov	DWORD PTR [LintconvPR], eax	; intconvPR = eax
TEX_Store:
;
; Here we go for the big calc of the output data
;
	fld	DWORD PTR [LlargestT]	; lT
	fld	st(0)			; lT,lT
	fld	st(0)			; lT,lT,lT
	fmul	DWORD PTR [Lsu2]	; su2*lT,lT,lT
	fld	st(1)			; lT,su2*lT,lT,lT
	fmul	DWORD PTR [Lsv2]	; sv2*lT,su2*lT,lT,lT
	fxch	st(1)			; su2*lT,sv2*lT,lT,lT
	fadd	DWORD PTR [LintconvAF]	; I_su2*lT,sv2*lT,lT,lT
; Add 1 integer cycle here (see below)
; The real crux is how we do up to TSP[2]-
;
;      pTSP[0] = (exp<<18) | pMat->WriteBack.ModControl;
;      pTSP[1] = pMat->WriteBack.ModBaseCol;
;      pTSP[2] = (w2 & 0xffffUL);
;
       shl	ebx,18				; ebx <<= 18
        mov	ecx,DWORD PTR [esi][ModControl] ; ecx = pMat->WriteBack.ModControl
	fld	st(2)			; lT,I_su2*lT,sv2*lT,lT,lT
	fxch	st(2)			; sv2*lT,I_su2*lT,lT,lT,lT
	fadd	DWORD PTR [LintconvAF]	; I_sv2*lT,I_su2*lT,lT,lT,lT
	fxch	st(1)			; I_su2*lT,I_sv2*lT,lT,lT,lT
; Stall of 1 unless we use integer after the fadd above
	fstp	DWORD PTR [Lint_u2]	; int_u2=,I_sv2*lT,lT,lT,lT
	fld	st(1)			; lT,I_sv2*lT,lT,lT,lT,lT
	fld	st(2)			; lT,lT,I_sv2*lT,lT,lT,lT
	fmul	DWORD PTR [LTopExp]	; lBT=lT*TopExp,lT,I_sv2*lT,lT,lT,lT
	fld	DWORD PTR [Lsw2]	; sw2,lBT,lT,I_sv2*lT,lT,lT,lT
; stall 1 (but can use integer)
       or	ecx,ebx				; ecx |= ebx (exp<<18)
        mov	ebx,DWORD PTR [esi][ModBaseCol] ; ebx = pMat->WriteBack.ModBaseCol
	fmul	st, st(1)		; sw2*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
; stall 2 (but can use integer)
       mov	DWORD PTR [edi], ecx		; pTSP[0] = ecx
        mov	edx,DWORD PTR _gPDC[TexAddress]	; edx = gPDC.TexAddress
       shl	edx,16				; edx = gPDC.TexAddress << 16
        mov	DWORD PTR [edi+4], ebx		; pTSP[1] = ebx
	fadd	DWORD PTR [LintconvPR]	; I_sw2*lBT,lBT,lT,I_sv2*lT,lT,lT,lT
; fstp stall 3 (but can use integer)
	fstp	DWORD PTR [Lint_w2]	; int_w2=,lBT,lT,I_sv2*lT,lT,lT,lT
	fld	DWORD PTR [esi][w0]	; w0,lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(1)			; lBT,w0,lT,I_sv2*lT,lT,lT,lT
	fmul	st(1),st		; lBT,w0*lBT,lT,I_sv2*lT,lT,lT,lT
; fmul stall 1 (but can use integer)
	fmul	DWORD PTR [esi][w1]	; w1*lBT,w0*lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(1)			; w0*lBT,w1*lBT,lT,I_sv2*lT,lT,lT,lT
	fadd	DWORD PTR [LintconvPR]	; I_w0*lBT,w1*lBT,lT,I_sv2*lT,lT,lT,lT
	fxch	st(6)			; lT,w1*lBT,lT,I_sv2*lT,lT,lT,I_w0*lBT
	fmul	DWORD PTR [esi][u0]	; u0*lT,w1*lBT,lT,I_sv2*lT,lT,lT,I_w0*lBT
	fxch	st(3)			; I_sv2*lT,w1*lBT,lT,u0*lT,lT,lT,I_w0*lBT
	fstp	DWORD PTR [Lint_v2]	; int_v2=,w1*lBT,lT,u0*lT,lT,lT,I_w0*lBT
	fadd	DWORD PTR [LintconvPR]	; I_w1*1BT,lT,u0*lT,lT,lT,I_w0*lBT
	fxch	st(5)			; I_w0*lBT,lT,u0*lT,lT,lT,I_w1*lBT
	fstp	DWORD PTR [Lint_w0]	; int_w1=,lT,u0*lT,lT,lT,I_w1*lBT
	fmul	DWORD PTR [esi][u1]	; u1*lT,u0*lT,lT,lT,I_w1*lBT
	fxch	st(4)			; I_w1*lBT,u0*lT,lT,lT,u1*lT
	fstp	DWORD PTR [Lint_w1]	; int_w1=,u0*lT,lT,lT,u1*lT
	fadd	DWORD PTR [LintconvAF]	; I_u0*lT,lT,lT,u1*lT
; Stall of 3 but can use integer
       mov	ecx, DWORD PTR [Lint_w2]	; ecx = int_w2
        mov	eax,DWORD PTR [Lint_w1]		; eax = int_w1
       shl	eax,16				; eax <<= 16
        and	ecx,ebp				; ecx &= 65535
       mov	DWORD PTR [edi+8], ecx		; pTSP[2] = ecx
        mov	ebx,DWORD PTR [Lint_w0]		; ebp = w0
	fstp	DWORD PTR [Lint_u0]	; int_u0=,lT,lT,u1*lT
	fmul	DWORD PTR [esi][v1]	; v1*lT,lT,u1*lT
	fxch	st(2)			; u1*lT,lT,v1*lT
	fadd	DWORD PTR [LintconvAF]	; I_u1*lT,lT,v1*lT
	fxch	st(1)			; lT,I_u1*lT,v1*lT
	fmul	DWORD PTR [esi][v0]	; v0*lT,I_u1*lT,v1*lT
; Stall of 2 but can do integer	
       and	ebx,ebp				; ebx &= w0&0ffffh
        or	eax,ebx				; eax = (w1<<16) | (w0&0ffffh)
       mov	ebx,DWORD PTR [Lint_u2]		; eax = INT32 su2*largT
        mov	DWORD PTR [edi+12], eax		; pTSP[3] = eax
	fadd	DWORD PTR [LintconvAF]	; I_v0*lT,I_u1*lT,v1*lT
	fxch	st(1)			; I_u1*lT,I_v0*lT,v1*lT
	fstp	DWORD PTR [Lint_u1]	; int_u1=,I_v0*lT,v1*lT
; Stall of 1 but can do integer
       and	ebx,ebp				; eax &= 65535
        mov	eax,DWORD PTR [Lint_u0]		; edx = int_u0
	fstp	DWORD PTR [Lint_v0]	; int_v0=,v1*lT
	fadd	DWORD PTR [LintconvAF]	; I_v1*lT
; stall of 3 but can use integer
       or	edx,ebx				; edx = (gPDC.TexAddress << 16) = ebx
        mov	ebx, DWORD PTR [Lint_u1]	; ebx = int_u1
       shl	ebx,16				; ebx <<= 16
        and	eax,ebp				; edx &= 65535
       mov	DWORD PTR [edi+16], edx		; pTSP[4] = edx
        or	ebx,eax				; ebx = (u1<<16) | (u0&0ffffh)
	fstp	DWORD PTR [Lint_v1]	; int_v1=

       mov	eax,DWORD PTR _gPDC[TexAddress]	; eax = gPDC.TexAddress
        mov	DWORD PTR [edi+20], ebx		; pTSP[5] = ebx
       mov	ebx,DWORD PTR [Lint_v2]		; ebx = int_v2
        and	eax,-65536			; eax &= 0ffff0000h
       and	ebx,ebp				; ebx &= 65535
        mov	edx, DWORD PTR [Lint_v1]	; edx = int_v1
       shl	edx, 16				; edx <<= 16
        or	eax,ebx				; eax |= (v2&0ffffh)
       mov	DWORD PTR [edi+24], eax		; pTSP[6] = eax
        mov	ebx, DWORD PTR [Lint_v0]	; ebx = v0
       mov	eax, DWORD PTR [TypeFlag]	; eax = TypeFlag
        and	ebp,ebx				; ebp = v0 & 65535
       mov	ebx, DWORD PTR [PnPolys]	; ecx = PnPolys
        or	edx,ebp				; edx = (v1<<16) | (v0&0ffffh)
       mov	DWORD PTR [edi+28], edx		; pTSP[7] = edx
        mov	edx, DWORD PTR [TSPIncrement]	; edx = TSPIncrement
       shr	eax,1
        jc	TEX_FlatShade
       jz	TEX_NoExtras
       shr	eax,1
        jnc	TEX_SmoothSTex
       mov	eax,DWORD PTR [esi][Highlight]	; eax = pMat->WriteBack.Highlight
	mov	DWORD PTR [edi+32],eax		; pTSP[8] = eax
        jmp	TEX_NoExtras
       
TEX_FlatShade:
       mov	eax,DWORD PTR [esi][Smooth1]	; eax = pMat->WriteBack.Smooth1
        mov	ecx,DWORD PTR [esi][Smooth2]	; ecx = pMat->WriteBack.Smooth2
       mov	DWORD PTR [edi+32],eax		; pTSP[8] = eax
        mov	DWORD PTR [edi+36],ecx		; pTSP[9] = ecx
	jmp	TEX_NoExtras
	
TEX_SmoothSTex:					; SMOOTHSHADTEX
       mov	eax,DWORD PTR [esi][Smooth1]	; eax |= pMat->WriteBack.Smooth1
        mov	ecx,DWORD PTR [esi][Smooth2]	; ecx |= pMat->WriteBack.Smooth2
       mov	DWORD PTR [edi+32],eax		; pTSP[8] = eax
        mov	eax,DWORD PTR [esi][LightVolCol]; eax |= pMat->v.LightVolCol
       mov	DWORD PTR [edi+36],ecx		; pTSP[9] = ecx
        mov	ecx,DWORD PTR [esi][Highlight]	; ecx = pMat->WriteBack.Highlight
       mov	DWORD PTR [edi+40],eax		; pTSP[10] = eax
        mov	DWORD PTR [edi+44],ecx		; pTSP[11] = ecx
TEX_NoExtras:
       mov	eax, DWORD PTR [nMatInc]	; eax = nMatInc
        lea	edi, DWORD PTR [edi+edx*4]	; edi += TSPIncrement
       add	esi,eax				; ESI += eax (nMatInc)
        dec	ebx				; ebx = PnPolys - 1
       mov	DWORD PTR [PnPolys], ebx	; PnPolys = ebx
        jnz	TEX_Loop			; If PnPolys == 0 then Loop
TEX_Done:
	add	esp, Adjust			; Lose locals
	pop	ebx
	pop	ebp
	pop	esi
	pop	edi
	ret
	ALIGN	4
if DEBUG_ASSEMBLER
_PackTSPTexInAsm ENDP
else
_PackTSPTex ENDP
endif
_TEXT	ENDS

_DATA1	SEGMENT PARA PUBLIC USE32 'DATA'
_2_3_2il2floater_31	DD 046fffe00H	; xf32
_2_3_2il2floater_24	DD 03f800000H	; 1.0f
_2_3_2il2floater_9	DD 03a802008H	; xf32
_2_3_2il2floater_3	DD 03ed413dcH	; 1.0f/1023.0f
_DATA1	ENDS
_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
_DATA	ENDS
; -- End  _PackTSPTex

END
