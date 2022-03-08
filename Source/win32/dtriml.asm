;*****************************************************************************
;*++
;*  Name        :   $RCSfile: dtriml.asm,v $
;*  Title       :   
;*  ASM Author  :   Rob Isherwood
;*  Created     :   15/1/97
;*
;* Copyright	: 1995-2022 Imagination Technologies (c)
;* License		: MIT
;*
;*  Description :   Pentium optimised Process loops from dtri.c
;*
;*  Program Type:   ASM module (.dll)
;* 
;*  RCS info:
;*
;*  $Date: 1997/10/30 17:05:10 $
;*  $Revision: 1.24 $
;*  $Locker:  $
;*  $Log: dtriml.asm,v $
;; Revision 1.24  1997/10/30  17:05:10  sxy
;; when have no sort, put all trans faces into the front list.
;;
;; Revision 1.23  1997/09/08  16:19:30  erf
;; Fixed code with new culling stuff.
;;
;; Revision 1.22  1997/09/08  09:31:17  erf
;; Added new culling of smal triangles to assembler.
;;
;; Revision 1.21  1997/08/22  10:50:46  sks
;; Increased PIMAT_SIZE by 4 for nSmoothSpecularColour
;;
;; Revision 1.20  1997/08/18  16:27:51  gdc
;; increased PIMAT_SIZE again
;;
;; Revision 1.19  1997/08/13  16:22:02  gdc
;; used a symbol for material struct size - would be nice to
;; feed it in on the stack
;;
;; Revision 1.18  1997/06/26  17:37:59  erf
;; Fixed new pass per tri code. Sorry ho...
;;
;; Revision 1.17  1997/06/26  10:53:02  erf
;; Fixed problem with non-perspective code.
;;
;; Revision 1.16  1997/06/24  17:46:04  erf
;; Added new pass per translucent triangle code into triangle processing.
;;
;; Revision 1.15  1997/06/20  14:46:45  erf
;; Added assembler for processing the non-perspective case.
;;
;; Revision 1.14  1997/05/16  17:20:35  erf
;; Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
;;
;; Revision 1.13  1997/04/21  13:27:17  erf
;; Added opaque tanslucent code to the ProcessTriCoreLite routine.
;; Hopefully the assembler code is now re-sync'd with the C code.
;;
;; Revision 1.12  1997/04/18  16:30:00  erf
;; Implemented texel offset in the assembler code.
;;
;; Revision 1.11  1997/04/17  08:38:52  erf
;; Fixed the assembler code to call the texture C code so as to implement
;; the texel offset. The offset code should be added into the assembler.
;;
;; Revision 1.10  1997/03/20  18:25:54  jrc
;; Added underscore to function name so it'd build.
;;
;; Revision 1.9  1997/03/20  18:11:51  jop
;; Changed name of asm function to get rid of link errors
;;
;; Revision 1.8  1997/03/04  20:35:52  erf
;; XY offset fix. Enabled if MODIFYXY = 1 at start of file.
;; Adds fAddToXY to each of the vertices. Also region calculation
;; has changed. Region now calculated for X0, Y0, X1-1, Y1-1. This
;; prevents regions from having invalid plane info.
;;
;; Revision 1.7  1997/02/26  14:21:17  gdc
;; chnaged determinate based poly rejection criteria to match
;; that of sglmacro.h - i.e. 0.0625f. Need for monalisa->AK47
;;
;; Revision 1.6  1997/02/03  16:11:08  rgi
;; Added handling of D3D faces to ProcessTriCoreLite to get
;; the D3D applications back up to scratch.
;;
;; Revision 1.5  1997/01/30  17:20:45  rgi
;; Revised IBUFFERSIZE ala dtri.c change
;;
;; Revision 1.4  1997/01/27  13:51:35  rgi
;; Big change, removed D3DTriangle optimised version and added
;; newer ProcessTriCoreLite with inlined texture handling code.
;;
;; Revision 1.3  1997/01/24  09:17:59  rgi
;; Prepared for in-lining of ProcessFlatTex and ProcessD3DFlatTex
;; into dtriml.asm. Sadly this breaks ProcessD3DTriCore so it's
;; currently conditioned out.
;;
;; Revision 1.2  1997/01/20  17:34:46  rgi
;; First tru assembler version, appears to work with R4 and
;; Ultimate race.
;;
;; Revision 1.1  1997/01/16  13:05:09  rgi
;; Initial revision
;;
;* 
;* 	This was all produced from compiler output of dtri.c's version with
;*        the following options.
;* 
;*	The initial ProcessD3DTriCore was taken from dispml.asm
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
MAGIC_INT_CONV				EQU	4b400000h
LESS_THAN_LARGE_EPSILON		EQU	03ef00000h
LESS_THAN_EPSILON			EQU	03d800000h
WORDS_PER_PLANE				EQU	(3*4)
IEEE_MANTISSA_MASK			EQU	07FFFFFh
IEEE_ASSUMED_BIT			EQU	0800000h
IBUFFERSIZE					EQU	16
SGLTT_DISABLEZBUFFER		EQU	02000h
SGLTT_FACESIND3DFORMAT		EQU	00800h
D3DTRIFLAG_ODD      		equ	0000001eh
D3DTRIFLAG_EVEN     		equ	0000001fh
MASK_TRANS					EQU	1024
MASK_OPAQUE_TRANS			EQU	00040000h
MASK_NEWPASSPERTRI			EQU 00020000h
MODIFYXY    	    		EQU	1   ; Enable/disable modify XY.
TEXEL_OFFSET				EQU 1	; Enable/disable texel offset.
MINZ_SORT  					EQU 3

; Object type definitions
PACKED_TYPE_OPAQUE			EQU 0
PACKED_TYPE_SHADOW			EQU 1
PACKED_TYPE_LIGHTVOL		EQU 2
PACKED_TYPE_OPAQUETRANS     EQU 3
PACKED_TYPE_TRANSLUCENT		EQU 4
PACKED_TRANSTYPE_SETMARK  	EQU 2
PACKED_TRANSTYPE_BACKBIT  	EQU 1 
PACKED_TRANS_AND_SETMARK	EQU 6	; PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK

;PITRI structure
Adj0						EQU	0
Adj1						EQU	12
Adj2						EQU	24
f1OverDet					EQU	36
fZ							EQU	40
ITreg						EQU	52
ITTSPControlWord 			EQU	56
ITBaseColour				EQU	60
PITRI_SIZE					EQU	64

;gPDC structure, we only know about stuff upto Context sub-struct
nPolys						EQU	0
pVertices					EQU	4
TSPControlWord				EQU	8
pFace						EQU	12
pV0							EQU	16
pV1							EQU	20
pV2							EQU	24
ShiftRegX					EQU	28
TexAddress					EQU	32
n32MipmapOffset 			EQU	36
invTexSize					EQU	40
fHalfTexel					EQU	44
Ctx							EQU	48

;SGLCONTEXT structure. Size of context structure is 144 bytes.
bFogOn		EQU	0
fFogR		EQU	4
fFogG		EQU	8
fFogB		EQU	12
u32FogDensity	EQU	16
bCullBackfacing	EQU	20 ; (+Ctx = 64)
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
FirstXRegion	EQU	72 ; (+Ctx = 116)
LastXRegion	EQU	76 ; (+Ctx = 120)
FirstYRegion	EQU	80 ; (+Ctx = 124)
LastYRegion	EQU	84 ; (+Ctx = 128)

;Vertex structure
V_fX		EQU	0
V_fY		EQU	4
V_fZ		EQU	8
V_fInvW		EQU	12
V_fu32Color	EQU	16
V_fu32Specular	EQU	20
V_fUOvW		EQU	24
V_fVOvW		EQU	28

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
PIMAT_SIZE	EQU	76

if 0
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
endif

; mark_description "source.tmp\\pcx -Ie:\\sabre\\sgl\\D3DOPTIM\\source.tmp\\win32 -Ie:\\sabre\\SGL\\..\\vsgl -Gy -Zp4 -G5 -QIfdi";
; mark_description "v- -W0 -nologo -Qpc32 -Qrct -Op- -DDLL_METRIC=1 -O2 -Oy -Qip -Qmem -Qscope_alloc -QW0, -mo_alternate_loops, -m";
; mark_description "o_block_loops, -mo_strip_mine -Oi";
;ident "Intel Reference  Release 5 Version 2.3.2 Beta B96061"
;ident "-DWIN32 -QWl, -force:multiple -QWl, -incremental:no -S -Fae:\sabre\sgl\D3DOPTIM\release.tmp\dtri.asm -DFULL_ON -DPVR2 -DPCX1 -DD"

	.386P
	.387

_DATA1	SEGMENT PARA PUBLIC USE32 'DATA'

	_2_3_2il2floatpacket_13	DD 03f800000H	; xf32
	_2_3_2il2floatpacket_10	DD 09e3ce508H	; xf32
	_2_3_2il2floatpacket_7	DD 01e3ce508H	; xf32
	_BogusZIncrementValue	DD 030000000H

_DATA1	ENDS

	ASSUME	CS:FLAT, DS:FLAT, SS:FLAT

_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
EXTRN	_gfBogusInvZ:DWORD
EXTRN	_fMinInvZ:DWORD
EXTRN	_gpTri:DWORD
EXTRN	_ProcessFlatTexFn:DWORD
EXTRN	_gpMatCurrent:DWORD
EXTRN	_gpPDC:DWORD
EXTRN	_gPDC:DWORD
if MODIFYXY
; Offset value to add to vertices.
EXTRN	_fAddToXY:DWORD
endif
EXTRN	_gNoSortTransFaces:DWORD
_DATA	ENDS

_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'
	EXTRN	_ProcessD3DFlatTex:PROC
	EXTRN	_ProcessFlatTex:PROC
	EXTRN	_ProcessFlatTexNonPer:PROC
_TEXT	ENDS

; -- Machine type P
; mark_description "Intel Reference  Release 5 Version 2.3.2 Beta B96061";
; mark_description "-DWIN32 -QWl,-force:multiple -QWl,-incremental:no -S -Fae:\\sabre\\sgl\\SGLMID4\\release.tmp\\dtmp.asm -DFUL";
; mark_description "L_ON -DPCX1=1 -DD3DOPTIM=1 -DDDRAW -DWIN32=1 -DICL=1 -DVER_LIB_MAX=3 -DVER_LIB_MA=1 -DVER_LIB_MI=0 -DVER_LIB";
; mark_description "_MIN=0x0402 -DVER_LIB_TXT=\"3.1.0b2\" -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\. -Ie:\\sabre\\sgl\\SGLMID4\\so";
; mark_description "urce.tmp\\pcx -Ie:\\sabre\\sgl\\SGLMID4\\source.tmp\\win32 -Ie:\\sabre\\SGL\\..\\vsgl -Gy -Zp4 -QIfdiv- -W0 ";
; mark_description "-nologo -Qpc32 -Qrct -Op- -G5 -O2 -Oy -Qip -Qmem -Qscope_alloc -QW0,-mo_alternate_loops,-mo_block_loops,-mo_";
; mark_description "strip_mine -Oi";
;ident "Intel Reference  Release 5 Version 2.3.2 Beta B96061"
;ident "-DWIN32 -QWl,-force:multiple -QWl,-incremental:no -S -Fae:\sabre\sgl\SGLMID4\release.tmp\dtmp.asm -DFULL_ON -DPCX1=1 -DD3DOPTI"
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
	ASSUME	CS:FLAT,DS:FLAT,SS:FLAT
_BSS	SEGMENT PARA PUBLIC USE32 'BSS'
uTCLDoneFirst	DD 2 DUP (0)	; pad
if MODIFYXY
; Tempory variables to store XY coordinates
	XYs			DD 6 DUP (0)
endif
HalfTexTimesW	DD 3 DUP (0)
Temp			DD 3 DUP (0)	; Temporory memory. Used to see if coordinates are equal for extra culling

_BSS	ENDS
_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
_DATA	ENDS
_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'
;	COMDAT _ProcessTriCoreLite
; -- Begin	_ProcessTriCoreLite
; mark_begin;
	ALIGN	4			; 0
	PUBLIC	__ProcessTriCoreLite
__ProcessTriCoreLite	PROC NEAR

	LCallPara	TEXTEQU <esp+	0>
	LfDet		TEXTEQU <esp+	4>
	LuOrBackType	TEXTEQU <esp+   8>
	LuDefaultType	TEXTEQU <esp+  12>
	LuCurrentType	TEXTEQU <esp+  16>
	LrX1		TEXTEQU <esp+  20>
	LrX0		TEXTEQU <esp+  24>
	LrY1		TEXTEQU <esp+  28>
	LrY0		TEXTEQU <esp+  32>
	Adjust		TEXTEQU <36>
if MODIFYXY
; Local storage of XY coordinates
	fX0			TEXTEQU <XYs+0>
	fX1			TEXTEQU <XYs+4>
	fX2			TEXTEQU <XYs+8>
	fY0			TEXTEQU <XYs+12>
	fY1			TEXTEQU <XYs+16>
	fY2			TEXTEQU <XYs+20>
else
; Local storage of XY coordinates
	fX0			TEXTEQU <rpV0[V_fX]>
	fX1			TEXTEQU <rpV1[V_fX]>
	fX2			TEXTEQU <rpV2[V_fX]>
	fY0			TEXTEQU <rpV0[V_fY]>
	fY1			TEXTEQU <rpV1[V_fY]>
	fY2			TEXTEQU <rpV2[V_fY]>
endif
	RSaveEBX	TEXTEQU <esp+	0+Adjust>
	RSaveEBP	TEXTEQU <esp+	4+Adjust>
	RSaveESI	TEXTEQU <esp+	8+Adjust>
	RSaveEDI	TEXTEQU <esp+  12+Adjust>
	RRetAddr	TEXTEQU <esp+  16+Adjust>
	PpAddfn		TEXTEQU <esp+  20+Adjust> ; 152
	PNewObject	TEXTEQU <esp+  24+Adjust> ; 156

       push	edi			; Save registers
	push	esi
       push	ebp
	push	ebx
       sub	esp,Adjust		; Preserve local space
	mov	esi,OFFSET FLAT: _gpTri	; ESI = gpTri
       mov     edx,_gPDC[TSPControlWord]; edx = TSPControlWord
	xor	eax,eax			; eax = 0
       and	edx,MASK_TRANS		; edx = MASK_TRANS bit or 0
	je	TCL_TSetup 		; if 0 TCL_TSetup with eax,edx = 0
;
; Check for Opaque trans. Done by reading the Context flag to check for
; opaque setting.
;
	mov	ebx,_gPDC[Ctx][u32Flags]
    and	ebx,MASK_OPAQUE_TRANS			; edx = MASK_OPAQUE_TRANS bit or 0
	je	TCL_TNewPass					; If !OpaqueTrans skip to new pass per tri
	mov	eax,PACKED_TYPE_OPAQUETRANS		; eax = PACKED_TYPE_OPAQUETRANS for u*Type
	mov	edx,PACKED_TYPE_OPAQUETRANS		; edx = PACKED_TYPE_OPAQUETRANS for u*Type
	jmp	TCL_TSetup 						; continue...
;
; Else check for new pass per triangle setup
;
TCL_TNewPass:
	mov	ebx,_gPDC[Ctx][u32Flags]
    and	ebx,MASK_NEWPASSPERTRI			; ebx = MASK_NEWPASSPERTRI bit or 0
	je	TCL_TTrans						; If !NewPassPerTri skip tp normal trans
;
; PACKED_TRANS_AND_SETMARK = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK
;
		mov	eax,PACKED_TRANS_AND_SETMARK; eax = 6 for u*Type
		xor	edx,edx						; edx = 0 for uOrBackType
	jmp	TCL_TSetup 						; continue...
;
; Else translucent setup (rare for performance case)
;
TCL_TTrans:
    mov	ebx,[PNewObject]				; ebx = NewObject
	mov	edx,PACKED_TRANSTYPE_BACKBIT	; edx = 1 for uOrBackType
	test	ebx,ebx						; Flags = ebx & ebx
	je	@F								; If !NewObject skip reset
       mov	uTCLDoneFirst[0],eax		; uTCLDoneFirst[0] = 0
        mov	uTCLDoneFirst[4],eax		; uTCLDoneFirst[1] = 0
@@:     mov	eax,PACKED_TYPE_TRANSLUCENT	; eax = 4 for u*Type
; Check for no sort		
		mov	ebx,_gNoSortTransFaces		; ebx = _gNoSortTransFaces
		cmp ebx, MINZ_SORT
		je	TCL_TSetup 				   	; ebx =  MINZ_SORT, continue...
		xor	edx,edx						; edx = 0 for uOrBackType	
;
; Carry on with the main line
;
TCL_TSetup:
       mov	[LuDefaultType],eax	; uDefaultType = eax
	mov	[LuCurrentType],eax	; uCurrentType = eax
	mov	[LuOrBackType],edx	; uOrBackType = edx
	
TCL_Loop:
;
; Main loop: esi = pTri
;  	     eax, ebx, ecx, edx, ebp, edi = undefined
;
       mov	eax,_gPDC[pFace]	; eax = pFace
	mov	ebx,_gPDC[nPolys]	; ebx = nPolys
       dec	ebx			; ebx = nPolys - 1
	js	TCL_Done		; If -ive nPolys then TCL_Done
       mov	_gPDC[nPolys],ebx	; nPolys = nPolys - 1 = ebx
        mov	ebx,_gPDC[Ctx][u32Flags]; ebx = u32Flags
       and	ebx,SGLTT_FACESIND3DFORMAT; Is this D3D case?
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
	jz	TCL_SGLFace		; No so use SGL Face format
	
TCL_D3DFace:
       mov	ebx,[eax][0]		; ebx = (Index1<<16) + Index0
	mov	edi,[eax][4]		; edi = (Flags<<16) + Index2
       mov	edx,ebx			; edx = ebx
        add	eax,8			; eax = pFace + sizeof(SGLD3DFACE)
       shr	ebx,16			; ebx = Index1
        and	edi,0ffffh		; edi = Index2
       and	edx,0ffffh		; edx = Index0
	jmp	TCL_FloatIt		; Carry on

TCL_SGLFace:
       mov	edx,[eax][0]		; edx = pFace[0]
	mov	ebx,[eax][4]		; ebx = pFace[1]
       mov	edi,[eax][8]		; edi = pFace[2]
        add	eax,12			; eax = pFace + 3

TCL_FloatIt:
       shl	edi,5			; edi *= sizeof(VERTEX)
	mov	_gPDC[pFace],eax	; pFace++ = eax
       shl	ebx,5			; ebx *= sizeof(VERTEX)
        add	edi,ecx			; EDI = pVertices + (edi*Size)
       shl	edx,5			; edx *= sizeof(VERTEX)
	add	ebx,ecx			; EBX = pVertices + (ebx*Size)
       add	edx,ecx			; EDX = pVertices + (edx*Size)
	mov     eax,_gPDC[TSPControlWord]; eax = gPDC.TSPControlWord
rpV0	TEXTEQU		<[edx]>		; Vertex pointers available
rpV1	TEXTEQU		<[ebx]>
rpV2	TEXTEQU		<[edi]>
if MODIFYXY
; Add offset value to XY coordinates
	fld		DWORD PTR rpV0[V_fX]	; X0
	fadd	DWORD PTR _fAddToXY
	fld		DWORD PTR rpV1[V_fX]	; X1
	fadd	DWORD PTR _fAddToXY
	fxch	ST(1)
	fstp	DWORD PTR fX0
	fld		DWORD PTR rpV2[V_fX]	; X2
	fadd	DWORD PTR _fAddToXY
	fxch	ST(1)
	fstp	DWORD PTR fX1
	fld		DWORD PTR rpV0[V_fY]	; Y0
	fadd	DWORD PTR _fAddToXY
	fxch	ST(1)
	fstp	DWORD PTR fX2
	fld		DWORD PTR rpV1[V_fY]	; Y1
	fadd	DWORD PTR _fAddToXY
	fxch	ST(1)
	fstp	DWORD PTR fY0
	fld		DWORD PTR rpV2[V_fY]	; Y2
	fadd	DWORD PTR _fAddToXY
	fxch	ST(1)
	fstp	DWORD PTR fY1
	fstp	DWORD PTR fY2
endif
	fld	DWORD PTR fY1	; Y1
	fmul	DWORD PTR fX2	; X2*Y1
	fld	DWORD PTR fY2	; Y2,X2*Y1
	fmul	DWORD PTR fX1	; X1*Y2,X2*Y1
	fld	DWORD PTR fX2	; X2,X1*Y2,X2*Y1
	fmul	DWORD PTR fY0	; X2*Y0,X1*Y2,X2*Y1
	fld	DWORD PTR fX0	; X0,X2*Y0,X1*Y2,X2*Y1
	fmul	DWORD PTR fY2	; X0*Y2,X2*Y0,X1*Y2,X2*Y1
	fld	DWORD PTR fX0	; X0,X0*Y2,X2*Y0,X1*Y2,X2*Y1
	fmul	DWORD PTR fY1	; X0*Y1,X0*Y2,X2*Y0,X1*Y2,X2*Y1
	fld	DWORD PTR fX1	; X1,X0*Y1,X0*Y2,X2*Y0,X1*Y2,X2*Y1
	fmul	DWORD PTR fY0	; X1*Y0,X0*Y1,X0*Y2,X2*Y0,X1*Y2,X2*Y1
	fxch	st(5)			; X2*Y1,X0*Y1,X0*Y2,X2*Y0,X1*Y2,X1*Y0
	fsubp	st(4),st		; X0*Y1,X0*Y2,X2*Y0,AdjX0,X1*Y0
	fxch	st(1)			; X0*Y2,X0*Y1,X2*Y0,AdjX0,X1*Y0
	fsubp	st(2),st		; X0*Y1,AdjX1,AdjX0,X1*Y0
	fsubrp	st(3),st		; AdjX1,AdjX0,AdjX2
	fld	st(1)			; AdjX0,AdjX1,AdjX0,AdjX2
	fadd	st,st(1)		; AdjX0+AdjX1,AdjX1,AdjX0,AdjX2
	fxch	st(1)			; AdjX1,AdjX0+AdjX1,AdjX0,AdjX2
	fstp	DWORD PTR [esi][Adj2][4]; pTri->Adj2[1]=,AdjX0+AdjX1,AdjX0,AdjX2
	fadd	st,st(2)		; AdjX0+AdjX1+AdjX2,AdjX0,AdjX2
; Stalls 3 but can do integer
       mov   [esi][ITTSPControlWord],eax; pTri->TSPControlWord = eax
	mov	eax,[edx+16]					; eax = pV0->u32Colour
       mov	_gPDC[pV2],edi				; gPDC.pV2 = edi
        mov	_gPDC[pV0],edx				; gPDC.pV0 = edx
;
       mov	ebp,LESS_THAN_LARGE_EPSILON	; ebp = MAGIC_NUM_LARGE
        mov	_gPDC[pV1],ebx				; gPDC.pV1 = ebx
	fstp	DWORD PTR [LfDet]			; LfDet=,AdjX0,AdjX2
       mov	[esi][ITBaseColour],eax		; pTri->BaseColour = eax
        mov	eax,[LfDet]					; eax = fDet
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
	jge	TCL_NormDet						; if fDet < MAGIC_NUM_LARGE	no extra checking
;
;
       mov	edx,LESS_THAN_EPSILON				; edx = MAGIC_NUM
       cmp	eax,edx						; Flags = eax - MAGIC_NUM
	jl	TCL_NormalPath					; if fDet >= MAGIC_NUM then TCLLS_NormalPath
;
; Check to see if integer X the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]		; MAGIC
	fadd	DWORD PTR fX0			; int(fX0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fX0)
	fadd	DWORD PTR fX1			; int(fX1), int(fX0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fX1), int(fX0)
	fadd	DWORD PTR fX2			; int(fX2), int(fX1), int(fX0)
	fstp	DWORD PTR Temp[0]
	fstp	DWORD PTR Temp[4]
	fstp	DWORD PTR Temp[8]
;
	mov		edx, Temp[0]
	mov		ebp, Temp[4]
	xor		edx, Temp[8]
	xor		ebp, Temp[8]
	or		edx, ebp
	and		edx, 0FFFFFh
;
	jz	TCL_ShortCut2		; continue if result zero.
;
; Check to see if integer Y's the same. fY0, fY1, fY2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]	; MAGIC
	fadd	DWORD PTR fY0	; int(fY0)
	fld		DWORD PTR Temp[0]	; MAGIC, int(fY0)
	fadd	DWORD PTR fY1	; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]	; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2	; int(fY2), int(fY1), int(fY0)
	fstp	DWORD PTR Temp[0]
	fstp	DWORD PTR Temp[4]
	fstp	DWORD PTR Temp[8]
;
	mov		edx, Temp[0]
	mov		ebp, Temp[4]
	xor		edx, Temp[8]
	xor		ebp, Temp[8]
	or		edx, ebp
	and		edx, 0FFFFFh
;
	jz	TCL_ShortCut2		; continue if result zero.
;
; Restore edx
	mov	edx,_gPDC[pV0]			; eax = edx = pV0
	jmp	TCL_NormDet				; go normal route
;
TCL_NormalPath:
;
; Determinate is very small or negative
;
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
	xor	eax,080000000h		; eax = - fDet
       test	ecx,ecx			; Flags = bCullBackFacing
	jne	TCL_ShortCut2		; If culling then TCL_ShortCut2
;
; Invert the Determinate
;
;	Reload the values for the time being.
;
   mov	ebp,LESS_THAN_LARGE_EPSILON	; ebp = MAGIC_NUM_LARGE
   mov	edx,LESS_THAN_EPSILON				; edx = MAGIC_NUM
;
	cmp	eax,ebp
	 jge	TCL_NormalPath2	; if fDet < MAGIC_NUM_LARGE continue
;
	cmp	eax,edx
	 jl	TCL_ShortCut2		; if fDet < MAGIC_NUM TCL_ShortCut2
;
; Check to see if integer X's the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]	; MAGIC
	fadd	DWORD PTR fX0		; int(fX0)
	fld		DWORD PTR Temp[0]	; MAGIC, int(fX0)
	fadd	DWORD PTR fX1		; int(fX1), int(fX0)
	fld		DWORD PTR Temp[0]	; MAGIC, int(fX1), int(fX0)
	fadd	DWORD PTR fX2		; int(fX2), int(fX1), int(fX0)
	fstp	DWORD PTR Temp[0]
	fstp	DWORD PTR Temp[4]
	fstp	DWORD PTR Temp[8]
;
	mov		edx, Temp[0]
	mov		ebp, Temp[4]
	xor		edx, Temp[8]
	xor		ebp, Temp[8]
	or		edx, ebp
	and		edx, 0FFFFFh
;
	jz	TCL_ShortCut2		; continue if result zero.
;
; Check to see if integer Y's the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]	; MAGIC
	fadd	DWORD PTR fY0	; int(fY0)
	fld		DWORD PTR Temp[0]	; MAGIC, int(fY0)
	fadd	DWORD PTR fY1	; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]	; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2	; int(fY2), int(fY1), int(fY0)
	fstp	DWORD PTR Temp[0]
	fstp	DWORD PTR Temp[4]
	fstp	DWORD PTR Temp[8]

	mov		edx, Temp[0]
	mov		ebp, Temp[4]
	xor		edx, Temp[8]
	xor		ebp, Temp[8]
	or		edx, ebp
	and		edx, 0FFFFFh
;
	jz	TCL_ShortCut2		; continue if result zero.
;
TCL_NormalPath2:
;
; Determinate is significant and was negative, invert/swap related values
;
	fchs				; -AdjX0,AdjX2
	fxch	st(1)			; AdjX2,-AdjX0
	fchs				; -AdjX2,-AdjX0
       mov	[LfDet],eax		; fDet = eax = -fDet
	mov	ecx,[LuCurrentType]	; ecx = uCurrentType
       mov	eax,[esi][Adj2][4]	; eax = AdjX1 = pTri->Adj2[1]
        or	ecx,[LuOrBackType]	; ecx |= uOrBackType
       xor	eax,080000000h		; eax = -AdjX1
        mov	[LuCurrentType],ecx	; uCurrentType |= uOrBackType
       mov	[esi][Adj2][4],eax	; pTri->Adj2[1] = -AdjX1
; need edx
;	mov	eax,edx			; eax = edx = pV0
	mov	eax,_gPDC[pV0]			; eax = edx = pV0
       mov	edx,edi			; edx = edi = pV2
	mov	edi,eax			; edi = eax = pV0
       mov	_gPDC[pV2],edi		; gPDC.pV2 = edi
        mov	_gPDC[pV0],edx		; gPDC.pV0 = edx
if MODIFYXY
; Swap X0, X2 and Y0, Y2.
	mov	eax, fX0
	mov	ecx, fX2
	mov	fX2, eax
	mov	fX0, ecx
	mov	eax, fY0
	mov	ecx, fY2
	mov	fY2, eax
	mov	fY0, ecx
endif
TCL_NormDet:
;
; Registers: esi = pTri, edx = pV0, ebx = pV1, edi = pV2
;
	fstp	DWORD PTR [esi][Adj2][0]; pTri->Adj2[0]=AdjX0
	fstp	DWORD PTR [esi][Adj2][8]; pTri->Adj2[2]=AdjX2
	fld	DWORD PTR fY0	; pV0->fY
	fsub	DWORD PTR fY1	; pV0->fY-pV1->fY
	fld	DWORD PTR fY1	; pV1->fY,pV0->fY-pV1->fY
	fsub	DWORD PTR fY2	; pV1->fY-pV2->fY,pV0->fY-pV1->fY
	fld	DWORD PTR fY2	; pV2->fY,pV1->fY-pV2->fY,pV0->fY-pV1->fY
	fsub	DWORD PTR fY0	; pV2->fY-pV0->fY,pV1->fY-pV2->fY,pV0->fY-pV1->fY
	fxch	st(2)			; pV0->fY-pV1->fY,pV1->fY-pV2->fY,pV2->fY-pV0->fY
	fstp	DWORD PTR [esi][Adj0][8]; pTri->Adj0[2]=,pV1->fY-pV2->fY,pV2->fY-pV0->fY
	fstp	DWORD PTR [esi][Adj0][0]; pTri->Adj0[0]=,pV2->fY-pV0->fY
	fstp	DWORD PTR [esi][Adj0][4]; pTri->Adj0[1]=
	fld	DWORD PTR fX1	; pV1->fX
	fsub	DWORD PTR fX0	; pV1->fX-pV0->fX
	fld	DWORD PTR fX0	; pV0->fX,pV1->fX-pV0->fX
	fsub	DWORD PTR fX2	; pV0->fX-pV2->fX,pV1->fX-pV0->fX
	fld	DWORD PTR fX2	; pV2->fX,pV0->fX-pV2->fX,pV1->fX-pV0->fX
	fsub	DWORD PTR fX1	; pV2->fX-pV1->fX,pV0->fX-pV2->fX,pV1->fX-pV0->fX
	fxch	st(2)			; pV1->fX-pV0->fX,pV0->fX-pV2->fX,pV2->fX-pV1->fX
	fstp	DWORD PTR [esi][Adj1][8]; pTri->Adj1[2]=,pV0->fX-pV2->fX,pV2->fX-pV1->fX
	fstp	DWORD PTR [esi][Adj1][4]; pTri->Adj1[1]=,pV2->fX-pV1->fX
	fstp	DWORD PTR [esi][Adj1][0]; pTri->Adj1[0]=,
       mov	ecx,OFFSET FLAT: _ProcessFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
	jne	TCL_CheckD3D
;
; Process Flat Texture code (same as D3D without the *InvWs)
;
;
;	if(fHalfTexel)
;	{
; 		/* Bilinear case with texel offset fix.
;		 */
;		U0 = pV0->fUOverW - (fHalfTexel * W0);
;		U1 = pV1->fUOverW - (fHalfTexel * W1);
;		U2 = pV2->fUOverW - (fHalfTexel * W2);
;		/*
;		** Do V.   (V don't)
;		*/
;		V0 = pV0->fVOverW - (fHalfTexel * W0);
;		V1 = pV1->fVOverW - (fHalfTexel * W1);
;		V2 = pV2->fVOverW - (fHalfTexel * W2);
;	}
;	else
;	{
; 		/* NonBilinear case.
;		 */
;		U0 = pV0->fUOverW;
;		U1 = pV1->fUOverW;
;		U2 = pV2->fUOverW;
;		/*
;		** Do V.   (V don't)
;		*/
;		V0 = pV0->fVOverW;
;		V1 = pV1->fVOverW;
;		V2 = pV2->fVOverW;
;	}
;
; Need to decide whether the texel offset is required or not.
;
if TEXEL_OFFSET
	cmp		DWORD PTR _gPDC[fHalfTexel], 0
	jz		TCL_NonBilinear
;
	fld		DWORD PTR rpV0[V_fInvW]			; 0
	fmul	DWORD PTR _gPDC[fHalfTexel]
	fld		DWORD PTR rpV1[V_fInvW]			; 1, 0
	fmul	DWORD PTR _gPDC[fHalfTexel]
	fxch	st(1)							; 0, 1
	fstp	DWORD PTR HalfTexTimesW[0]		; 1
	fld		DWORD PTR rpV2[V_fInvW]			; 2, 1
	fmul	DWORD PTR _gPDC[fHalfTexel]
	fxch	st(1)							; 1, 2
	fstp	DWORD PTR HalfTexTimesW[4]		; 2
	fstp	DWORD PTR HalfTexTimesW[8]		; 
	jmp		TCL_Bilinear
;
TCL_NonBilinear:
; Set everything to zero.
	xor		eax, eax
	mov		DWORD PTR HalfTexTimesW[0], eax
	mov		DWORD PTR HalfTexTimesW[4], eax
	mov		DWORD PTR HalfTexTimesW[8], eax
;
TCL_Bilinear:
endif
	mov	ebp, DWORD PTR _gpMatCurrent	; EBP = gpMatCurrent
	fld	DWORD PTR rpV0[V_fUOvW]			; U0
if TEXEL_OFFSET
	fsub 	DWORD PTR HalfTexTimesW[0]	; U0
endif
	fld	DWORD PTR rpV1[V_fUOvW]			; U1,U0
if TEXEL_OFFSET
	fsub 	DWORD PTR HalfTexTimesW[4]	; U1,U0
endif
	fld	DWORD PTR [esi][Adj2][0]		; A20,U1,U0
	fmul	st, st(2)					; A20*U0(2),U1,U0
	fld	DWORD PTR rpV2[V_fUOvW]			; U2,A20*U0(1),U1,U0
if TEXEL_OFFSET
	fsub 	DWORD PTR HalfTexTimesW[8]
endif
	fld	DWORD PTR [esi][Adj2][4]		; A21,U2,A20*U0,U1,U0
	fmul	st, st(3)					; A21*U1(2),U2,A20*U0,U1,U0
	fxch	st(1)						; U2,A21*U1(2),A20*U0,U1,U0
	fsub	st(4),st					; U2,A21*U1(1),A20*U0,U1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]		; A22,U2,A21*U1,A20*U0,U1,o0(1)
	fmul	st, st(1)					; A22*U2(2),U2,A21*U1,A20*U0,U1,o0
	fxch	st(1)						; U2,A22*U2(2),A21*U1,A20*U0,U1,o0
	fsubp	st(4), st					; A22*U2(1),A21*U1,A20*U0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]		; A00,A22*U2,A21*U1,A20*U0,o1(1),o0
	fmul	st, st(5)					; A00*o0(2),A22*U2,A21*U1,A20*U0,o1,o0
	fxch	st(2)						; A21*U1,A22*U2,A00*o0(2),A20*U0,o1,o0
	faddp	st(3), st					; A22*U2,A00*o0(1),A20*U0+A21*U1(2),o1,o0
	fxch	st(4)						; o0,A00*o0(1),A20*U0+A21*U1(2),o1,A22*U2
	fmul	DWORD PTR [esi][Adj1][0]	; A10*o0(2),A00*o0,A20*U0+A21*U1(1),o1,A22*U2
	fld	DWORD PTR [esi][Adj0][4]		; A01,A10*o0(1),A00*o0,A20*U0+A21*U1,o1,A22*U2
	fmul	st, st(4)					; A01*o1(2),A10*o0,A00*o0,A20*U0+A21*U1,o1,A22*U2
	fxch	st(3)						; A20*U0+A21*U1,A10*o0,A00*o0,A01*o1(2),o1,A22*U2
	faddp	st(5), st					; A10*o0,A00*o0,A01*o1(1),o1,ut2(2)
	fxch	st(3)						; o1,A00*o0,A01*o1(1),A10*o0,ut2(2)
	fmul	DWORD PTR [esi][Adj1][4]	; A11*o1(2),A00*o0,A01*o1,A10*o0,ut2(1)
	fxch	st(2)						; A01*o1,A00*o0,A11*o1(2),A10*o0,ut2(1)
	faddp	st(1), st					; ut0(2),A11*o1(1),A10*o0,ut2
	fxch	st(3)						; ut2,A11*o1(1),A10*o0,ut0(2)
	fld	DWORD PTR rpV0[V_fVOvW]			; V0,ut2(-1),A11*o1,A10*o0,ut0(1)
if TEXEL_OFFSET
	fsub 	DWORD PTR HalfTexTimesW[0]
endif
	fxch	st(2)						; A11*o1,ut2(-1),V0,A10*o0,ut0(1)
	faddp	st(3), st					; ut2(-2),V0,ut1(2),ut0
	fstp	DWORD PTR [ebp][u2]			; Tex.u2=,V0,ut1,ut0(-2)
	fxch	st(2)						; ut0(-2),ut1,V0
	fstp	DWORD PTR [ebp][u0]			; Tex.u0=,ut1(-2),V0
	fstp	DWORD PTR [ebp][u1]			; Tex.u1=,V0
; Now for V
	fld	DWORD PTR rpV1[V_fVOvW]			; V1,V0
if TEXEL_OFFSET
	fsub 	DWORD PTR HalfTexTimesW[4]
endif
	fld	DWORD PTR [esi][Adj2][0]		; A20,V1,V0
	fmul	st, st(2)					; A20*V0(2),V1,V0
	fld	DWORD PTR rpV2[V_fVOvW]			; V2,A20*V0(1),V1,V0
if TEXEL_OFFSET
	fsub 	DWORD PTR HalfTexTimesW[8]
endif
	fld	DWORD PTR [esi][Adj2][4]		; A21,V2,A20*V0,V1,V0
	fmul	st, st(3)					; A21*V1(2),V2,A20*V0,V1,V0
	fxch	st(1)						; V2,A21*V1(2),A20*V0,V1,V0
	fsub	st(4),st					; V2,A21*V1(1),A20*V0,V1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]		; A22,V2,A21*V1,A20*V0,V1,o0(1)
	fmul	st, st(1)					; A22*V2(2),V2,A21*V1,A20*V0,V1,o0
	fxch	st(1)						; V2,A22*V2(2),A21*V1,A20*V0,V1,o0
	fsubp	st(4), st					; A22*V2(1),A21*V1,A20*V0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]		; A00,A22*V2,A21*V1,A20*V0,o1(1),o0
	fmul	st, st(5)					; A00*o0(2),A22*V2,A21*V1,A20*V0,o1,o0
	fxch	st(2)						; A21*V1,A22*V2,A00*o0(2),A20*V0,o1,o0
	faddp	st(3), st					; A22*V2,A00*o0(1),A20*V0+A21*V1(2),o1,o0
	fxch	st(4)						; o0,A00*o0(1),A20*V0+A21*V1(2),o1,A22*V2
	fmul	DWORD PTR [esi][Adj1][0]	; A10*o0(2),A00*o0,A20*V0+A21*V1(1),o1,A22*V2
	fld	DWORD PTR [esi][Adj0][4]		; A01,A10*o0(1),A00*o0,A20*V0+A21*V1,o1,A22*V2
	fmul	st, st(4)					; A01*o1(2),A10*o0,A00*o0,A20*V0+A21*V1,o1,A22*V2
	fxch	st(3)						; A20*V0+A21*V1,A10*o0,A00*o0,A01*o1(2),o1,A22*V2
	faddp	st(5), st					; A10*o0,A00*o0,A01*o1(1),o1,vt2(2)
	fxch	st(3)						; o1,A00*o0,A01*o1(1),A10*o0,vt2(2)
	fmul	DWORD PTR [esi][Adj1][4]	; A11*o1(2),A00*o0,A01*o1,A10*o0,vt2(1)
	fxch	st(2)						; A01*o1,A00*o0,A11*o1(2),A10*o0,vt2(1)
	faddp	st(1), st					; vt0(2),A11*o1(1),A10*o0,vt2
	fxch	st(3)						; vt2,A11*o1(1),A10*o0,vt0(2)
	fld	DWORD PTR rpV0[V_fInvW]			; W0,vt2(-1),A11*o1,A10*o0,vt0(1)
	fxch	st(2)						; A11*o1,vt2(-1),W0,A10*o0,vt0(1)
	faddp	st(3), st					; vt2(-2),W0,vt1(2),vt0
	fstp	DWORD PTR [ebp][v2]			; Tex.v2=,W0,vt1,vt0(-2)
	fxch	st(2)						; vt0(-2),vt1,W0
	fstp	DWORD PTR [ebp][v0]			; Tex.v0=,vt1(-2),W0
	fstp	DWORD PTR [ebp][v1]			; Tex.v1=,W0
	jmp	TCL_DoWTex

TCL_ShortCut2:							; Discard 2 fpu values
	fstp	st(0)
	fstp	st(0)
	jmp	TCL_Loop						; Next poly please

TCL_ByPtr:
       push	edx							; Save edx and pass pTri as
	push	esi							; only para.
	call	eax							; Call _ProcessFlatTexFn
       pop	esi							; Recover state; edx is the
	pop	edx								; only reg is use not saved
	jmp	TCL_DoneTex						; by the Intel compiler.

TCL_CheckD3D:
       or	eax,eax
	jz	TCL_DoneTex
       cmp	eax,OFFSET FLAT: _ProcessD3DFlatTex
	jne	TCL_CheckNonPer
;
; Process D3D Texture code, must do one mul for every add/sub/ld or we will
; run out of non multiply operations like the compiler does.
;
;	/*
;	// Do U. Note: it has to be scaled by W
;	*/
;	if(fHalfTexel)
;	{
; 		/* Bilinear case with texel offset fix.
;		 */
;		/*
;		** Do U. Note: it has to be scaled by W
;		*/
;		U0 = (gPDC.pV0->fUOverW - fHalfTexel )*W0;
;		U1 = (gPDC.pV1->fUOverW - fHalfTexel )*W1;
;		U2 = (gPDC.pV2->fUOverW - fHalfTexel )*W2;
;		/*
;		** Do V. Note: it has to be scaled by W
;		*/
;		V0 = (gPDC.pV0->fVOverW - fHalfTexel) *W0;
;		V1 = (gPDC.pV1->fVOverW - fHalfTexel) *W1;
;		V2 = (gPDC.pV2->fVOverW - fHalfTexel) *W2;
;	}
;	else
;	{
; 		/* NonBilinear case.
;		 */
;		U0 = gPDC.pV0->fUOverW*W0;
;		U1 = gPDC.pV1->fUOverW*W1;
;		U2 = gPDC.pV2->fUOverW*W2;
;		/*
;		** Do V. Note: it has to be scaled by W
;		*/
;		V0 = gPDC.pV0->fVOverW *W0;
;		V1 = gPDC.pV1->fVOverW *W1;
;		V2 = gPDC.pV2->fVOverW *W2;
;	}
;
	mov	ebp, DWORD PTR _gpMatCurrent		; EBP = gpMatCurrent
	fld	DWORD PTR rpV0[V_fUOvW]				; U0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]		; (U0-fHalfTexel)
endif
	fmul	DWORD PTR rpV0[V_fInvW]			; U0*W0(2)
	fld	DWORD PTR rpV1[V_fUOvW]				; U1,U0(1)
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]		; (U1-fHalfTexel)
endif
	fmul	DWORD PTR rpV1[V_fInvW]			; U1*W1(2),U0
	fld	DWORD PTR [esi][Adj2][0]			; A20,U1(1),U0
	fmul	st, st(2)						; A20*U0(2),U1,U0
	fld	DWORD PTR rpV2[V_fUOvW]				; U2,A20*U0(1),U1,U0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]		; (U2-fHalfTexel)
endif
	fmul	DWORD PTR rpV2[V_fInvW]			; U2*W2(2),A20*U0,U1,U0
	fld	DWORD PTR [esi][Adj2][4]			; A21,U2(1),A20*U0,U1,U0
	fmul	st, st(3)						; A21*U1(2),U2,A20*U0,U1,U0
	fxch	st(1)							; U2,A21*U1(2),A20*U0,U1,U0
	fsub	st(4),st						; U2,A21*U1(1),A20*U0,U1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]			; A22,U2,A21*U1,A20*U0,U1,o0(1)
	fmul	st, st(1)						; A22*U2(2),U2,A21*U1,A20*U0,U1,o0
	fxch	st(1)							; U2,A22*U2(2),A21*U1,A20*U0,U1,o0
	fsubp	st(4), st						; A22*U2(1),A21*U1,A20*U0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]			; A00,A22*U2,A21*U1,A20*U0,o1(1),o0
	fmul	st, st(5)						; A00*o0(2),A22*U2,A21*U1,A20*U0,o1,o0
	fxch	st(2)							; A21*U1,A22*U2,A00*o0(2),A20*U0,o1,o0
	faddp	st(3), st						; A22*U2,A00*o0(1),A20*U0+A21*U1(2),o1,o0
	fxch	st(4)							; o0,A00*o0(1),A20*U0+A21*U1(2),o1,A22*U2
	fmul	DWORD PTR [esi][Adj1][0]		; A10*o0(2),A00*o0,A20*U0+A21*U1(1),o1,A22*U2
	fld	DWORD PTR [esi][Adj0][4]			; A01,A10*o0(1),A00*o0,A20*U0+A21*U1,o1,A22*U2
	fmul	st, st(4)						; A01*o1(2),A10*o0,A00*o0,A20*U0+A21*U1,o1,A22*U2
	fxch	st(3)							; A20*U0+A21*U1,A10*o0,A00*o0,A01*o1(2),o1,A22*U2
	faddp	st(5), st						; A10*o0,A00*o0,A01*o1(1),o1,ut2(2)
	fxch	st(3)							; o1,A00*o0,A01*o1(1),A10*o0,ut2(2)
	fmul	DWORD PTR [esi][Adj1][4]		; A11*o1(2),A00*o0,A01*o1,A10*o0,ut2(1)
	fxch	st(2)							; A01*o1,A00*o0,A11*o1(2),A10*o0,ut2(1)
	faddp	st(1), st						; ut0(2),A11*o1(1),A10*o0,ut2
	fxch	st(3)							; ut2,A11*o1(1),A10*o0,ut0(2)
	fld	DWORD PTR rpV0[V_fVOvW]				; V0,ut2(-1),A11*o1,A10*o0,ut0(1)
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]		; (V0-fHalfTexel)
endif
	fxch	st(2)							; A11*o1,ut2(-1),V0,A10*o0,ut0(1)
	faddp	st(3), st						; ut2(-2),V0,ut1(2),ut0
	fstp	DWORD PTR [ebp][u2]				; Tex.u2=,V0,ut1,ut0(-2)
	fxch	st(2)							; ut0(-2),ut1,V0
	fstp	DWORD PTR [ebp][u0]				; Tex.u0=,ut1(-2),V0
	fstp	DWORD PTR [ebp][u1]				; Tex.u1=,V0
; Now for V
	fmul	DWORD PTR rpV0[V_fInvW]			; V0*W0(2)
	fld	DWORD PTR rpV1[V_fVOvW]				; V1,V0(1)
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]		; (V1-fHalfTexel)
endif
	fmul	DWORD PTR rpV1[V_fInvW]			; V1*W1(2),V0
	fld	DWORD PTR [esi][Adj2][0]			; A20,V1(1),V0
	fmul	st, st(2)						; A20*V0(2),V1,V0
	fld	DWORD PTR rpV2[V_fVOvW]				; V2,A20*V0(1),V1,V0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]		; (V2-fHalfTexel)
endif
	fmul	DWORD PTR rpV2[V_fInvW]			; V2*W2(2),A20*V0,V1,V0
	fld	DWORD PTR [esi][Adj2][4]			; A21,V2(1),A20*V0,V1,V0
	fmul	st, st(3)						; A21*V1(2),V2,A20*V0,V1,V0
	fxch	st(1)							; V2,A21*V1(2),A20*V0,V1,V0
	fsub	st(4),st						; V2,A21*V1(1),A20*V0,V1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]			; A22,V2,A21*V1,A20*V0,V1,o0(1)
	fmul	st, st(1)						; A22*V2(2),V2,A21*V1,A20*V0,V1,o0
	fxch	st(1)							; V2,A22*V2(2),A21*V1,A20*V0,V1,o0
	fsubp	st(4), st						; A22*V2(1),A21*V1,A20*V0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]			; A00,A22*V2,A21*V1,A20*V0,o1(1),o0
	fmul	st, st(5)						; A00*o0(2),A22*V2,A21*V1,A20*V0,o1,o0
	fxch	st(2)							; A21*V1,A22*V2,A00*o0(2),A20*V0,o1,o0
	faddp	st(3), st						; A22*V2,A00*o0(1),A20*V0+A21*V1(2),o1,o0
	fxch	st(4)							; o0,A00*o0(1),A20*V0+A21*V1(2),o1,A22*V2
	fmul	DWORD PTR [esi][Adj1][0]		; A10*o0(2),A00*o0,A20*V0+A21*V1(1),o1,A22*V2
	fld	DWORD PTR [esi][Adj0][4]			; A01,A10*o0(1),A00*o0,A20*V0+A21*V1,o1,A22*V2
	fmul	st, st(4)						; A01*o1(2),A10*o0,A00*o0,A20*V0+A21*V1,o1,A22*V2
	fxch	st(3)							; A20*V0+A21*V1,A10*o0,A00*o0,A01*o1(2),o1,A22*V2
	faddp	st(5), st						; A10*o0,A00*o0,A01*o1(1),o1,vt2(2)
	fxch	st(3)							; o1,A00*o0,A01*o1(1),A10*o0,vt2(2)
	fmul	DWORD PTR [esi][Adj1][4]		; A11*o1(2),A00*o0,A01*o1,A10*o0,vt2(1)
	fxch	st(2)							; A01*o1,A00*o0,A11*o1(2),A10*o0,vt2(1)
	faddp	st(1), st						; vt0(2),A11*o1(1),A10*o0,vt2
	fxch	st(3)							; vt2,A11*o1(1),A10*o0,vt0(2)
	fld	DWORD PTR rpV0[V_fInvW]				; W0,vt2(-1),A11*o1,A10*o0,vt0(1)
	fxch	st(2)		   					; A11*o1,vt2(-1),W0,A10*o0,vt0(1)
	faddp	st(3), st						; vt2(-2),W0,vt1(2),vt0
	fstp	DWORD PTR [ebp][v2]				; Tex.v2=,W0,vt1,vt0(-2)
	fxch	st(2)							; vt0(-2),vt1,W0
	fstp	DWORD PTR [ebp][v0]				; Tex.v0=,vt1(-2),W0
	fstp	DWORD PTR [ebp][v1]				; Tex.v1=,W0
TCL_DoWTex:
;
; The code for W is common between D3D and true SGL Lite texture formats
;
	fld	DWORD PTR rpV1[V_fInvW]	; W1,W0
	fld	DWORD PTR [esi][Adj2][0]; A20,W1,W0
	fmul	st, st(2)		; A20*W0(2),W1,W0
	fld	DWORD PTR rpV2[V_fInvW]	; W2,A20*W0(1),W1,W0
	fld	DWORD PTR [esi][Adj2][4]; A21,W2,A20*W0,W1,W0
	fmul	st, st(3)		; A21*W1(2),W2,A20*W0,W1,W0
	fxch	st(1)			; W2,A21*W1(2),A20*W0,W1,W0
	fsub	st(4),st		; W2,A21*W1(1),A20*W0,W1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]; A22,W2,A21*W1,A20*W0,W1,o0(1)
	fmul	st, st(1)		; A22*W2(2),W2,A21*W1,A20*W0,W1,o0
	fxch	st(1)			; W2,A22*W2(2),A21*W1,A20*W0,W1,o0
	fsubp	st(4), st		; A22*W2(1),A21*W1,A20*W0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]; A00,A22*W2,A21*W1,A20*W0,o1(1),o0
	fmul	st, st(5)		; A00*o0(2),A22*W2,A21*W1,A20*W0,o1,o0
	fxch	st(2)			; A21*W1,A22*W2,A00*o0(2),A20*W0,o1,o0
	faddp	st(3), st		; A22*W2,A00*o0(1),A20*W0+A21*W1(2),o1,o0
	fxch	st(4)			; o0,A00*o0(1),A20*W0+A21*W1(2),o1,A22*W2
	fmul	DWORD PTR [esi][Adj1][0]; A10*o0(2),A00*o0,A20*W0+A21*W1(1),o1,A22*W2
	fld	DWORD PTR [esi][Adj0][4]; A01,A10*o0(1),A00*o0,A20*W0+A21*W1,o1,A22*W2
	fmul	st, st(4)		; A01*o1(2),A10*o0,A00*o0,A20*W0+A21*W1,o1,A22*W2
	fxch	st(3)			; A20*W0+A21*W1,A10*o0,A00*o0,A01*o1(2),o1,A22*W2
	faddp	st(5), st		; A10*o0,A00*o0,A01*o1(1),o1,nwt2(2)
	fxch	st(3)			; o1,A00*o0,A01*o1(1),A10*o0,nwt2(2)
	fmul	DWORD PTR [esi][Adj1][4]; A11*o1(2),A00*o0,A01*o1,A10*o0,nwt2(1)
	fxch	st(2)			; A01*o1,A00*o0,A11*o1(2),A10*o0,nwt2(1)
	faddp	st(1), st		; nwt0(2),A11*o1(1),A10*o0,nwt2
	fxch	st(3)			; nwt2,A11*o1(1),A10*o0,nwt0(2)
	fmul	DWORD PTR _gPDC[invTexSize];wt2(2),A11*o1,A10*o0,nwt0(1)
	fxch	st(1)			; A11*o1,wt2(2),A10*o0,nwt0(1)
	faddp	st(2),st		; wt2(1),nwt1(2),nwt0
	fxch	st(2)			; nwt0,nwt1(2),wt2(1)
	fmul	DWORD PTR _gPDC[invTexSize];wt0(2),nwt1(1),wt2
	fxch	st(2)			; wt2,nwt1(1),wt0(2)
	fstp	DWORD PTR [ebp][w2]	; Tex.w2=,nwt1,wt0
	fmul	DWORD PTR _gPDC[invTexSize];wt1(2),wt0
	fxch	st(1)			; wt0,wt1(2)
	fstp	DWORD PTR [ebp][w0]	; Tex.w0=,wt1
; Stall of 1
	fstp	DWORD PTR [ebp][w1]	; Tex.w1=,
	jmp		TCL_DoneTex			; Finished perspective texturing case.
;
; Start of non-perspective cases. Both SGLDirect & D3D
;
TCL_CheckNonPer:
    cmp		eax,OFFSET FLAT: _ProcessFlatTexNonPer
	jne		TCL_ByPtr
;
; Process Flat Texture code for SGLDirect and D3D
;
;
;	if(fHalfTexel)
;	{
; 		/* Bilinear case with texel offset fix.
;		 */
;		U0 = pV0->fUOverW - fHalfTexel;
;		U1 = pV1->fUOverW - fHalfTexel;
;		U2 = pV2->fUOverW - fHalfTexel;
;		/*
;		** Do V.   (V don't)
;		*/
;		V0 = pV0->fVOverW - fHalfTexel;
;		V1 = pV1->fVOverW - fHalfTexel;
;		V2 = pV2->fVOverW - fHalfTexel;
;	}
;	else
;	{
; 		/* NonBilinear case.
;		 */
;		U0 = pV0->fUOverW;
;		U1 = pV1->fUOverW;
;		U2 = pV2->fUOverW;
;		/*
;		** Do V.   (V don't)
;		*/
;		V0 = pV0->fVOverW;
;		V1 = pV1->fVOverW;
;		V2 = pV2->fVOverW;
;	}
;
; Need to decide whether the texel offset is required or not.
;
	mov	ebp, DWORD PTR _gpMatCurrent	; EBP = gpMatCurrent
	fld	DWORD PTR rpV0[V_fUOvW]			; U0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]	; U0
endif
	fld	DWORD PTR rpV1[V_fUOvW]			; U1,U0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]	; U1,U0
endif
	fld	DWORD PTR [esi][Adj2][0]		; A20,U1,U0
	fmul	st, st(2)					; A20*U0(2),U1,U0
	fld	DWORD PTR rpV2[V_fUOvW]			; U2,A20*U0(1),U1,U0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]
endif
	fld	DWORD PTR [esi][Adj2][4]		; A21,U2,A20*U0,U1,U0
	fmul	st, st(3)					; A21*U1(2),U2,A20*U0,U1,U0
	fxch	st(1)						; U2,A21*U1(2),A20*U0,U1,U0
	fsub	st(4),st					; U2,A21*U1(1),A20*U0,U1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]		; A22,U2,A21*U1,A20*U0,U1,o0(1)
	fmul	st, st(1)					; A22*U2(2),U2,A21*U1,A20*U0,U1,o0
	fxch	st(1)						; U2,A22*U2(2),A21*U1,A20*U0,U1,o0
	fsubp	st(4), st					; A22*U2(1),A21*U1,A20*U0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]		; A00,A22*U2,A21*U1,A20*U0,o1(1),o0
	fmul	st, st(5)					; A00*o0(2),A22*U2,A21*U1,A20*U0,o1,o0
	fxch	st(2)						; A21*U1,A22*U2,A00*o0(2),A20*U0,o1,o0
	faddp	st(3), st					; A22*U2,A00*o0(1),A20*U0+A21*U1(2),o1,o0
	fxch	st(4)						; o0,A00*o0(1),A20*U0+A21*U1(2),o1,A22*U2
	fmul	DWORD PTR [esi][Adj1][0]	; A10*o0(2),A00*o0,A20*U0+A21*U1(1),o1,A22*U2
	fld	DWORD PTR [esi][Adj0][4]		; A01,A10*o0(1),A00*o0,A20*U0+A21*U1,o1,A22*U2
	fmul	st, st(4)					; A01*o1(2),A10*o0,A00*o0,A20*U0+A21*U1,o1,A22*U2
	fxch	st(3)						; A20*U0+A21*U1,A10*o0,A00*o0,A01*o1(2),o1,A22*U2
	faddp	st(5), st					; A10*o0,A00*o0,A01*o1(1),o1,ut2(2)
	fxch	st(3)						; o1,A00*o0,A01*o1(1),A10*o0,ut2(2)
	fmul	DWORD PTR [esi][Adj1][4]	; A11*o1(2),A00*o0,A01*o1,A10*o0,ut2(1)
	fxch	st(2)						; A01*o1,A00*o0,A11*o1(2),A10*o0,ut2(1)
	faddp	st(1), st					; ut0(2),A11*o1(1),A10*o0,ut2
	fxch	st(3)						; ut2,A11*o1(1),A10*o0,ut0(2)
	fld	DWORD PTR rpV0[V_fVOvW]			; V0,ut2(-1),A11*o1,A10*o0,ut0(1)
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]
endif
	fxch	st(2)						; A11*o1,ut2(-1),V0,A10*o0,ut0(1)
	faddp	st(3), st					; ut2(-2),V0,ut1(2),ut0
	fstp	DWORD PTR [ebp][u2]			; Tex.u2=,V0,ut1,ut0(-2)
	fxch	st(2)						; ut0(-2),ut1,V0
	fstp	DWORD PTR [ebp][u0]			; Tex.u0=,ut1(-2),V0
	fstp	DWORD PTR [ebp][u1]			; Tex.u1=,V0
; Now for V
	fld	DWORD PTR rpV1[V_fVOvW]			; V1,V0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]
endif
	fld	DWORD PTR [esi][Adj2][0]		; A20,V1,V0
	fmul	st, st(2)					; A20*V0(2),V1,V0
	fld	DWORD PTR rpV2[V_fVOvW]			; V2,A20*V0(1),V1,V0
if TEXEL_OFFSET
	fsub 	DWORD PTR _gPDC[fHalfTexel]
endif
	fld	DWORD PTR [esi][Adj2][4]		; A21,V2,A20*V0,V1,V0
	fmul	st, st(3)					; A21*V1(2),V2,A20*V0,V1,V0
	fxch	st(1)						; V2,A21*V1(2),A20*V0,V1,V0
	fsub	st(4),st					; V2,A21*V1(1),A20*V0,V1,o0(2)
	fld	DWORD PTR [esi][Adj2][8]		; A22,V2,A21*V1,A20*V0,V1,o0(1)
	fmul	st, st(1)					; A22*V2(2),V2,A21*V1,A20*V0,V1,o0
	fxch	st(1)						; V2,A22*V2(2),A21*V1,A20*V0,V1,o0
	fsubp	st(4), st					; A22*V2(1),A21*V1,A20*V0,o1(2),o0
	fld	DWORD PTR [esi][Adj0][0]		; A00,A22*V2,A21*V1,A20*V0,o1(1),o0
	fmul	st, st(5)					; A00*o0(2),A22*V2,A21*V1,A20*V0,o1,o0
	fxch	st(2)						; A21*V1,A22*V2,A00*o0(2),A20*V0,o1,o0
	faddp	st(3), st					; A22*V2,A00*o0(1),A20*V0+A21*V1(2),o1,o0
	fxch	st(4)						; o0,A00*o0(1),A20*V0+A21*V1(2),o1,A22*V2
	fmul	DWORD PTR [esi][Adj1][0]	; A10*o0(2),A00*o0,A20*V0+A21*V1(1),o1,A22*V2
	fld	DWORD PTR [esi][Adj0][4]		; A01,A10*o0(1),A00*o0,A20*V0+A21*V1,o1,A22*V2
	fmul	st, st(4)					; A01*o1(2),A10*o0,A00*o0,A20*V0+A21*V1,o1,A22*V2
	fxch	st(3)						; A20*V0+A21*V1,A10*o0,A00*o0,A01*o1(2),o1,A22*V2
	faddp	st(5), st					; A10*o0,A00*o0,A01*o1(1),o1,vt2(2)
	fxch	st(3)						; o1,A00*o0,A01*o1(1),A10*o0,vt2(2)
	fmul	DWORD PTR [esi][Adj1][4]	; A11*o1(2),A00*o0,A01*o1,A10*o0,vt2(1)
	fxch	st(2)						; A01*o1,A00*o0,A11*o1(2),A10*o0,vt2(1)
	faddp	st(1), st					; vt0(2),A11*o1(1),A10*o0,vt2
	fxch	st(3)						; vt2,A11*o1(1),A10*o0,vt0(2)
	fxch	st(1)						; A11*o1,vt2(-1),A10*o0,vt0(1)
	faddp	st(2), st					; vt2(-2),vt1(2),vt0
	fstp	DWORD PTR [ebp][v2]			; Tex.v2=,vt1,vt0(-2)
	fxch	st(1)						; vt0(-2),vt1
	fstp	DWORD PTR [ebp][v0]			; Tex.v0=,vt1(-2)
	fstp	DWORD PTR [ebp][v1]			; Tex.v1=
;
; This is the non perspective correction texture case.
TCL_DoWTexNonPer:
;
; The code for W is common between D3D and true SGL Lite texture formats
; Need to check the state of the stack.
;
	fld		DWORD PTR [esi][Adj2][0]	; A20
	fadd	DWORD PTR [esi][Adj2][4]	; A21+A20
	fadd	DWORD PTR [esi][Adj2][8]	; A22+A21+A20
	fmul	DWORD PTR _gPDC[invTexSize]	; (A22+A21+A20)*invTexSize
	fstp	DWORD PTR [ebp][w2]			; Store result
	mov		DWORD PTR [ebp][w0],0		; Tex.w0 = 0
	mov		DWORD PTR [ebp][w1],0		; Tex.w1= 0
;
; End of non-perspective case.
;
TCL_DoneTex:
	mov		eax,_gPDC[Ctx][u32Flags]	; eax = gPDC.Context.u32Flags
	mov		ecx,8192					; ecx = ZBUFFER_MASK
	test	eax,ecx						; Flags = u32Flags & ZBUFFER_MASK
	jne		TCL_NoZBuff					; If u32Flags & 8192 then TCL_NoZBuff
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
	jmp	TCL_Cont1						; goto TCL_Cont1

TCL_ShortCut1:							; Dicard 1 fpu value
	fstp	st(0)
TCL_ShortCut0:
	jmp	TCL_Loop						; Next poly please

TCL_NoZBuff:
	fld	QWORD PTR _2_1_float_13			; c1=BOGUSZ_INCREMENT
	fadd	DWORD PTR _gfBogusInvZ		; c1+gfBogusInvZ
	mov	eax,_gfBogusInvZ				; eax=gfBogusInvZ
	mov	[esi][fZ][0],eax				; pTri->fZ[0]=eax
	mov	[esi][fZ][4],eax				; pTri->fZ[1]=eax
	mov	[esi][fZ][8],eax				; pTri->fZ[2]=eax
	fstp	DWORD PTR _gfBogusInvZ		; gfBogusInvZ=
TCL_Cont1:
;
; Using integer comparisons set ecx to min X and eax to max X
;
	mov	eax,DWORD PTR fX0	; eax = pV0->fX
	mov	ecx,DWORD PTR fX1	; ecx = pV1->fX
	cmp	eax,ecx			; Flags = pV0->fX - pV1->fX
	jge	@F			; If eax >= ecx carry on
	mov	eax,ecx			; eax = Max value
	mov	ecx,DWORD PTR fX0	; ecx = Min value
@@:	cmp	eax,DWORD PTR fX2	; Flags = Max value - pV2->fX
	jge	@F			; If Max value > pV2->fX carry on
	mov	eax,DWORD PTR fX2	; Max value = pV2->fX
	jmp	TCL_XDone		; goto TCL_XDone (cannot be min too)
@@:	cmp	ecx,DWORD PTR fX2	; Flags = Min value - pV2->fX
	jle	TCL_XDone		; If Min value < pV2->fX carry on
	mov	ecx,DWORD PTR fX2	; Min value = pV2->fX
;
; Now eax = X1 (Max X) and ecx = X0 (Min X) as floating point, we check here
; if X1 (Max X) is negative as this indicates an off-screen poly that can
; be skipped. If X1 is +ive but X0 is -ive then we set X0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
TCL_XDone:
	or	eax,eax			; Flags = eax & eax (Max X)
	js	TCL_ShortCut0		; if ( Max X < 0 ) next poly please
	or	ecx,ecx			; Flags = ecx & ecx (Min X)
	jns	@F			; if ( Min X >= 0 ) carry on
	xor	ecx,ecx			; Make MinX = 0
;
; Using integer comparisons set ecx to min Y and eax to max Y
;
@@:	mov	DWORD PTR [LrX0],ecx	; Store minX as rX0
	mov	ecx,DWORD PTR fY1	; ecx = pV1->fY
	mov	DWORD PTR [LrX1],eax	; Store maxX as rX1
	mov	eax,DWORD PTR fY0	; eax = pV0->fY
	mov	edi,DWORD PTR fY2	; edi = pV2->fY
	mov	edx,eax			; edx = copy of pV0->fY
rpV2 TEXTEQU <ErrorpV2>			; We have lost at least one of the
rpV0 TEXTEQU <ErrorpV0>			;  vertex pointers!
rpV1 TEXTEQU <ErrorpV1>
	cmp	eax,ecx			; Flags = pV0->fY - pV1->fY
	jge	@F			; If eax >= ecx carry on
	mov	eax,ecx			; eax = Max value
	mov	ecx,edx			; ecx = Min value
@@:	cmp	eax,edi			; Flags = Max value - pV2->fY
	jge	@F			; If Max value > pV2->fY carry on
	mov	eax,edi			; Max value = pV2->fY (cannot be min)
	jmp	TCL_YDone		; goto TCL_YDone
@@:	cmp	ecx,edi			; Flags = Min value - pV2->fY
	jle	TCL_YDone		; If Min value < pV2->fY TCL_YDone
	mov	ecx,edi			; Store Min value = pV2->fY
TCL_YDone:
;
; Now eax = Y1 (Max Y) and ecx = Y0 (Min Y) as floating point, we check here
; if Y1 (Max Y) is negative as this indicates an off-screen poly that can
; be skipped. If Y1 is +ive but Y0 is -ive then we set Y0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
	or	eax,eax			; Flags = eax & eax (Max Y)
	js	TCL_ShortCut0		; if ( Max Y < 0 ) next poly please
	or	ecx,ecx			; Flags = ecx & ecx (Min Y)
	jns	@F			; if ( Min Y >= 0 ) carry on
	xor	ecx,ecx			; Make MinY = 0
@@:	mov	DWORD PTR [LrY1],eax	; Store Max value = eax
	mov	DWORD PTR [LrY0],ecx	; Store Min value = ecx
;
; Now we need to convert rX0,rX1,rY0,rY1 to integers
;	use the registers ebx-edx,edi-ecx to store the bounding box.
;
	fld	DWORD PTR _float_to_int22; Integer conversion value
	fld	DWORD PTR [LrX0]	 ; fMinX,icv
	fadd	st,st(1)		 ; I_MinX,icv
	fld	DWORD PTR [LrX1]	 ; fMaxX,I_MinX,icv
	fadd	st,st(2)		 ; I_MaxX,I_MinX,icv
	fld	DWORD PTR [LrY0]	 ; fMinY,I_MaxX,I_MinX,icv
	fadd	st,st(3)		 ; I_MinY,I_MaxX,I_MinX,icv
	fxch	st(3)			 ; icv,I_MaxX,I_MinX,I_MinY
	fadd	DWORD PTR [LrY1]	 ; I_MaxY,I_MaxX,I_MinX,I_MinY
	fxch	st(2)			 ; I_MinX,I_MaxX,I_MaxY,I_MinY
	fstp	DWORD PTR [LrX0]	 ; rX0 = INT32 fMinX
	fstp	DWORD PTR [LrX1]	 ; rX1 = INT32 fMaxX
	fstp	DWORD PTR [LrY1]	 ; rY1 = INT32 fMaxY
	fstp	DWORD PTR [LrY0]	 ; rY0 = INT32 fMinY
	fld	DWORD PTR _2_1_float_18	 ; 1.0f
	fdiv	DWORD PTR [LfDet]	 ; 1.0f/fDet about 30 odd cycles later
;
; X0-X1, Y0-Y1 are now 22 bit +ive integer values with 10 bits of exponent
; etc in the MSBs. We have to load them up and and off the 'fpu_junk'.
;
; By rather deviously leaving off the LSB of the mask we avoid the need
; to apply the Y_SHIFT down by 1 to the Y values before encoding them
; later on. The Y limit values should not care about the LSB anyway and the X
; values are going to be sar'd by atleast 5 after masking so we are okay.
;
	mov	ebx,[LrX0]		; ebx = rX0 + fpu_junk
	mov	edx,[LrX1]		; edx = rX1 + fpu_junk
;
; Need to generate regions for X0+1, Y0+1, X1, Y1
; Always enabled since it is required for offset correction and
; no offset correct
    	cmp 	ebx, DWORD PTR [LrX1]
; Need to add 1 to LrX0 if carry set.
    	adc 	ebx,0
;
	mov	edi,0003ffffeh		; edi = significant bits mask
	mov	eax,5			; eax = likely shift factor
	and	ebx,edi			; ebx = rX0
	and	edx,edi			; edx = rX1
	sar	ebx,5			; ebx >>= 5 (ShiftRegX is 5 or 6)
	sub	eax,_gPDC[ShiftRegX]	; Flags = eax = 5 - gPDC.ShiftRegX
	mov	ecx,[LrY1]		; ecx = rY1 + fpu_junk
	mov	eax,[LrY0]		; eax = rY0
	je	@F			; If ShiftRegX == 5 carry on
	sar	ebx,1			; Extra shift of 1 for ebx
	sar	edx,1			; Extra shift of 1 for edx
@@:	sar	edx,5			; edx >>= 5 (ShiftRegX is 5 or 6)
;
; Need to generate regions for X0+1, Y0+1, X1, Y1
; Always enabled since it is required for offset correction and
; no offset correct
    	cmp 	eax, ecx
; Need to add 1 to LrY0 if carry set.
    	adc 	eax,0	    	    	; eax++ if carry set.
;
	and	ecx,edi			; ecx = rY1
	and	edi,eax		    	; edi = rY0
	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
	or	eax,eax			; Flags = eax | eax
	je	TCL_NoLimits		; if ( !bDoClipping ) TCL_NoLimits
;
; Apply [First/Last][X/Y]Region if required, likely case is for no clipping
; to occur.
;
	mov	eax,_gPDC[Ctx][FirstXRegion]	; eax = FirstXRegion
	mov	ebp,_gPDC[Ctx][LastXRegion]	; ebp = LastXRegion
	cmp	ebx,eax			; Flags = rX0 - FirstXReg
	jge	@F			; If rX0 >= FirstXReg skip clip
	cmp	edx,eax			; Flags = rX1 - FirstXReg
	jl	TCL_ShortCut1		; If rX1 < FirstXReg skip poly
	mov	ebx,eax			; rX0 = FirstXReg (very rare)
@@:	cmp	edx,ebp			; Flags = rX1 - LastXReg
	jle	@F			; If rX1 <= LastXReg skip clip
	cmp	ebx,ebp			; Flags = rX0 - LastXReg
	jg	TCL_ShortCut1		; If rX1 > LastXReg skip poly
	mov	edx,ebp			; rX1 = LastXReg (very rare)
@@:	mov	eax,_gPDC[Ctx][FirstYRegion]	; eax = FirstYRegion
	mov	ebp,_gPDC[Ctx][LastYRegion]	; ebp = LastYRegion
	cmp	edi,eax			; Flags = rY0 - FirstYReg
	jge	@F			; If rY0 >= FirstYReg skip clip
	cmp	ecx,eax			; Flags = rY1 - FirstYReg
	jl	TCL_ShortCut1		; If rY1 < FirstYReg skip poly
	mov	edi,eax			; rY0 = FirstYReg (very rare)
	and	edi,0fffffffeh		; ensure LSB of Y value stays zero
@@:	cmp	ecx,ebp			; Flags = rY1 - LastYReg
	jle	@F			; If rY1 <= LastYReg skip clip
	cmp	edi,ebp			; Flags = rY0 - LastYReg
	jg	TCL_ShortCut1		; If rY1 > LastYReg skip poly
	mov	ecx,ebp			; rY1 = LastYReg (very rare)
	and	ecx,0fffffffeh		; ensure LSB of Y value stays zero
@@:
;
; Region bounding box now established in ebx-edx,edi-ecx
;
TCL_NoLimits:
	shl	ebx,5			; ebx <<= 5  (X0)
	mov	eax,MASK_TRANS		; eax = MASK_TRANS
	mov	ebp,[LuCurrentType]	; ebp = uCurrentType
	add	ebx,edx			; ebx = (X0<<5)+X1
	and	eax,_gPDC[TSPControlWord]; eax = gPDC.TSPControlWord & MASK_TRANS
	je	@F			; If not transparent @@
;
; Transparent case is not ideal, but not a performance issue as such
;
	mov	eax,ebp			; eax = uCurrentType
	and	eax,1			; eax &= 1
	mov	edx,uTCLDoneFirst[eax*4]; AGI 1: edx = uTCLDoneFirst[eax]
	test	edx,edx			; Flags = edx & edx
	jne	@F			; If already set skip below
	or	ebp,2			; ebp |= PACKED_TRANSTYPE_SETMASK
	mov	DWORD PTR uTCLDoneFirst[eax*4],1; uTCLDoneFirst[eax] = 1
;
; Back to the main track, ebp = uCurrentType, bounding box = ebx-edx,edi-ecx
;	(LSBs of Y values have already been masked off earlier).
@@:	
	shl	ebp,29			; ebp = uCurrentType<<29
	mov	eax,[LuDefaultType]	; eax = uDefaultType
	shl	edi,18			; edi <<= (19-1) (Y0)
	mov	[LuCurrentType],eax	; uCurrentType = eax
	mov	eax,DWORD PTR [PpAddfn]	; eax = pAddfn
	add	ebp,edi			; ebp += edi (Y0)
	shl	ecx,9			; ecx <<= (9-1) (Y1)
	add	ebp,ebx			; ebp += ebx = (X0<<5) + X1
	add	ebp,ecx			; ebp += ecx (Y1)
	or	eax,eax			; Flags = pAddFn | pAddFn
	mov	[esi][ITreg],ebp	; pTri->reg = ebp
	mov	ebp, DWORD PTR _gpMatCurrent
	fstp	DWORD PTR [esi][f1OverDet]; pTri->f1OverDet= (late as pos.)
	mov	DWORD PTR [esp],esi	; Pass pTri as only para
	je	@F			; If no pAddFn skip call
	call	[PpAddFn]		; Call pAddFn
@@:	add	ebp,PIMAT_SIZE		; gpMatCurrent++
	add	esi,PITRI_SIZE		; pTri++
	mov	eax,[OFFSET FLAT: _gpTri][PITRI_SIZE*IBUFFERSIZE]
	mov	DWORD PTR _gpMatCurrent, ebp
	cmp	eax,esi			; Flags = gpTri[IBUFFERSIZE]-pTri
	jne	TCL_Loop		; If not at end of buffer TCL_Loop
TCL_Done:
	add	esp,Adjust		; Lose locals
	mov	eax,esi			; eax = pTri
	sub	eax,OFFSET FLAT: _gpTri	; eax = pTri - gpTri
	pop	ebx			; AGI 1 cycle with stack adjust
	shr	eax,6			; eax /= PITRI_SIZE
	pop	ebp			; Recover remaining registers
	pop	esi
	pop	edi
	ret
	ALIGN	4
__ProcessTriCoreLite ENDP
;_ProcessTriCoreLite	ENDS
_TEXT	ENDS
_DATA1	SEGMENT PARA PUBLIC USE32 'DATA'
_2_1_float_13	DD 000000000H,03e000000H	; xf64
_2_1_float_18	DD 03f800000H	; xf32
_2_1_float_18_d	DD 000000000H,03ff00000H	; xf32
_float_to_int22	DD 04B400000H			; MAGIC_FLOAT TO INT22 VALUE
_DATA1	ENDS
_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
_DATA	ENDS
; -- End	_ProcessTriCoreLite

EXTRN	__fltused:BYTE

END
