;*****************************************************************************
;*++
;*  Name        :   $RCSfile: d3dtriml.asm,v $
;*  Title       :   
;*  ASM Author  :   Edward Forde
;*  Created     :   15/1/97
;*
;* Copyright	: 1995-2022 Imagination Technologies (c)
;* License		: MIT
;*
;*  Description :   Pentium optimised Process loops from d3dtri.c
;*
;*  Program Type:   ASM module (.dll)
;* 
;*  RCS info:
;*
;*  $Date: 1998/01/26 18:02:26 $
;*  $Revision: 1.16 $
;*  $Locker:  $
;*  $Log: d3dtriml.asm,v $
;; Revision 1.16  1998/01/26  18:02:26  erf
;; Reverted changes done to implement new Z mode.
;; Now done by setting fMinInvZ to negative value
;; and gfInvertZ to 0 in d3dtri.c if the flag is
;; set
;;
;; Revision 1.15  1998/01/16  15:39:12  jrc
;; Assembler needs numeric values not constants, so '>=' mode should now work.
;;
;; Revision 1.14  1998/01/16  14:47:09  jrc
;; Put '>=' Z sort mode into assembler.
;;
;; Revision 1.13  1997/12/10  16:00:15  erf
;; Tweaked code to make it tidier.
;;
;; Revision 1.12  1997/12/10  12:23:08  erf
;; Added code to simulate Z buffer system more effectly.
;; The bogus inv Z value is now set to the maximum 1/w
;; value when the Z buffer is enabled. Not perfect but heh...
;;
;; Revision 1.11  1997/11/04  17:42:03  sxy
;; when have no sort, put all trans faces into the front list.
;;
;; Revision 1.10  1997/10/27  10:28:09  erf
;; Now use Z for D3D as default. ie set USERHW to 0.
;;
;; Revision 1.9  1997/10/24  09:19:51  erf
;; 1. Included routines that used to be in dcore.h .
;; 2. Added code to use Z instead of 1/w (compiled out at the moment).
;; 3. Added clockwise culling.
;;
;; Revision 1.8  1997/10/14  18:08:20  erf
;; 1. Spring cleaning. ie bit of tidying.
;; 2. Added assembler versions for DrawPrimitive process triangles routines
;;    for DAG sorting.
;;
;; Revision 1.7  1997/10/13  11:04:20  erf
;; Added define for reading the nNextTriInc from the passed parameters even
;; though it may not be used.
;;
;; Revision 1.6  1997/09/09  21:31:53  mjg
;; Fixed the problem with the assembly version of TransTriCoreLite -
;; the structure of 'extra' information (X,Y,A,B and C) was being
;; incremented as it was being used, rather than at the end. Which meant
;; that if any of the later tests jumped to the next iteration of the
;; loop, the structure wouldn't remain unincremented as it would in the
;; C version. I've also removed some unecessary code for dtermining the
;; 'CurrentType' of the triangles, as this is known, zero is used.
;;
;; Revision 1.5  1997/09/08  12:32:05  mjg
;; Added assembler code for new ProcessD3DTransTriCoreLite() routine.
;;
;; Revision 1.4  1997/09/08  10:42:15  erf
;; 1. This file now contains the assembler version of ProcessTriCoreLite.
;;    Copied from dtriml.asm and made D3D specific.
;; 2. Removed texture processing code not required by D3D.
;; 3. Added extra small triangle culling code to 3 routines.
;;
;; Revision 1.3  1997/09/02  16:23:25  erf
;; Fixed draw primitive assembler code.
;;
;; Revision 1.2  1997/09/01  15:12:41  erf
;; Checked in assembler versions of process tri primitive code to do
;; lists, strips and fans.
;;
;; Revision 1.1  1997/08/21  11:15:28  erf
;; Initial revision
;;
;*
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
USERHW						EQU	0
MINZ_SORT  					EQU 3
Z_BUFFER_SIM				EQU 1	; Modify bogus Z value based on greatest 1/w.

; Defines for culling modes supported.
SGL_NO_CULL					EQU 0
SGL_CULL_ANTICLOCK			EQU 1
SGL_CULL_CLOCKWISE			EQU 2

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

;PITRIPLUS structure 
fX						EQU	0	
fY						EQU	12
fA						EQU	24
fB						EQU	28
fC						EQU	32
PITRIPLUS_SIZE					EQU	36

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
EXTRN	_bStrip:DWORD
EXTRN	_nFaces:DWORD
EXTRN	_gfBogusInvZ:DWORD
EXTRN	_fMinInvZ:DWORD
EXTRN	_gpTri:DWORD
EXTRN	_gpTriPlus:DWORD ; For Transorting
EXTRN	_ProcessFlatTexFn:DWORD
EXTRN	_gpMatCurrent:DWORD
EXTRN	_gpPDC:DWORD
EXTRN	_gPDC:DWORD

;
; Needed to implement Z instead of 1/w surface removal.
;
EXTRN	_gfInvertZ:DWORD

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
TriAddr			DD 1 DUP (0)	; Address of gpTriPlus

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
	PFaceIndex	TEXTEQU <esp+  28+Adjust> ; 160

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
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
	
TCL_D3DFace:
       mov	ebx,[eax][0]		; ebx = (Index1<<16) + Index0
	mov	edi,[eax][4]		; edi = (Flags<<16) + Index2
       mov	edx,ebx			; edx = ebx
        add	eax,8			; eax = pFace + sizeof(SGLD3DFACE)
       shr	ebx,16			; ebx = Index1
        and	edi,0ffffh		; edi = Index2
       and	edx,0ffffh		; edx = Index0
	jmp	TCL_FloatIt		; Carry on

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
;
; Read culling mode here.
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
;
; Check for clockwise culling if det. greater than MAGIC_NUM_LARGE and positive.
;
	jge	TCL_CheckCW						; if fDet < MAGIC_NUM_LARGE	check for clockwise culling
;
       mov	edx,LESS_THAN_EPSILON		; edx = MAGIC_NUM
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
	xor	eax,080000000h		; eax = - fDet
;
; Check for anticlock culling
;
	cmp	ecx, SGL_CULL_ANTICLOCK
	je	TCL_ShortCut2		; If culling then TCL_ShortCut2
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
;
	jmp TCL_NormDet
;
; New checking for clockwise culling
;
TCL_CheckCW:
	cmp	ecx, SGL_CULL_CLOCKWISE
	je	TCL_ShortCut2		; If culling then TCL_ShortCut2
;
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
;
       mov	ecx,OFFSET FLAT: _ProcessD3DFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
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
;
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
       or	eax,eax
	jz	TCL_DoneTex
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
;
; For D3D we have been requested to use Z v's 1/w for ISP surface removal.
;
if USERHW
;
; Provide 1/w for the ISP
;
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
else
;
; Provide Z (in face 1-Z) for the ISP
;
	fld		DWORD PTR _gfInvertZ		; 1
	fsub	DWORD PTR rpV0[V_fZ]		; 1-fZ0
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ0)
	fsub	DWORD PTR rpV2[V_fZ]		; 1-fZ2,MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ2),MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fsub	DWORD PTR rpV1[V_fZ]		; 1-fZ1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ1),MinZ*(1-fZ2),MinZ*(1-fZ0)
	fxch	st(2)						; MinZ*(1-fZ0),MinZ*(1-fZ2),MinZ*(1-fZ1)
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
;
; To prevent Z of > 1 (which incidenetly are illegal in D3D) from appearing behind
; the viewer clamp the calculated 1/w (1-z) value to 0. 
; The 3 calculated fZ's are checked.
;
	xor		eax, eax
	test	DWORD PTR [esi][fZ][0], 080000000h
	setnz	al
		xor		ecx, ecx
		test	DWORD PTR [esi][fZ][8], 080000000h
		setnz	cl
	sub		eax, 1
		sub		ecx, 1
	and		DWORD PTR [esi][fZ][0], eax
			and		DWORD PTR [esi][fZ][8], ecx
			xor		edx, edx
			test	DWORD PTR [esi][fZ][4], 080000000h
			setnz	dl
			sub		edx, 1
			and		DWORD PTR [esi][fZ][4], edx
endif
;
if Z_BUFFER_SIM
;
; Set the Bogus Z value to the greatest 1/w value. Helps apps which switch
; the Z buffer ON/OFF. It isn't a perfect Z buffer system but heh...
;
	mov		ecx, DWORD PTR [esi][fZ][0]
	mov		eax, DWORD PTR [esi][fZ][8]
	cmp		ecx, DWORD PTR _gfBogusInvZ
	mov		edx, DWORD PTR [esi][fZ][4]
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, ecx
@@:	cmp		eax, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, eax
@@:	cmp		edx, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, edx
@@:
endif
;
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
;
; D3D we always do clipping.
;	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
;	or	eax,eax			; Flags = eax | eax
;	je	TCL_NoLimits		; if ( !bDoClipping ) TCL_NoLimits
;
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



;	COMDAT _ProcessD3DTransTriCoreLite
; -- Begin	_ProcessD3DTransTriCoreLite
; mark_begin;
	ALIGN	4			; 0
	PUBLIC	__ProcessD3DTransTriCoreLite
__ProcessD3DTransTriCoreLite	PROC NEAR

	LCallPara	TEXTEQU <esp+	0>
	LfDet		TEXTEQU <esp+	4>
	LrX1		TEXTEQU <esp+   8>
	LrX0		TEXTEQU <esp+  12>
	LrY1		TEXTEQU <esp+  16>
	LrY0		TEXTEQU <esp+  20>
	Adjust		TEXTEQU <24>
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
	PFaceIndex	TEXTEQU <esp+  28+Adjust> ; 160

       push	edi			; Save registers
	push	esi
       push	ebp
	push	ebx
       sub	esp,Adjust		; Preserve local space
	mov	esi,OFFSET FLAT: _gpTri	; ESI = gpTri
       mov     edx,_gPDC[TSPControlWord]; edx = TSPControlWord
;
	mov	ebp,OFFSET FLAT: _gpTriPlus	; Transorting, Ebp = gpTriPlus
	mov	TriAddr,ebp			; Transorting
	
D3D_TCL_Loop:
;
; Main loop: esi = pTri
;  	     eax, ebx, ecx, edx, ebp, edi = undefined
;
       mov	eax,_gPDC[pFace]	; eax = pFace
	mov	ebx,_gPDC[nPolys]	; ebx = nPolys
       dec	ebx			; ebx = nPolys - 1
	js	D3D_TCL_Done		; If -ive nPolys then D3D_TCL_Done
       mov	_gPDC[nPolys],ebx	; nPolys = nPolys - 1 = ebx
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
	
D3D_TCL_D3DFace:
       mov	ebx,[eax][0]		; ebx = (Index1<<16) + Index0
	mov	edi,[eax][4]		; edi = (Flags<<16) + Index2
       mov	edx,ebx			; edx = ebx
        add	eax,8			; eax = pFace + sizeof(SGLD3DFACE)
       shr	ebx,16			; ebx = Index1
        and	edi,0ffffh		; edi = Index2
       and	edx,0ffffh		; edx = Index0

D3D_TCL_FloatIt:
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
;
; Read culling mode here.
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
;
; Check for clockwise culling if det. greater than MAGIC_NUM_LARGE and positive.
;
	jge	D3D_TCL_CheckCW						; if fDet < MAGIC_NUM_LARGE	check for clockwise culling
;
       mov	edx,LESS_THAN_EPSILON				; edx = MAGIC_NUM
       cmp	eax,edx						; Flags = eax - MAGIC_NUM
	jl	D3D_TCL_NormalPath					; if fDet >= MAGIC_NUM then TCLLS_NormalPath
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
	jz	D3D_TCL_ShortCut2		; continue if result zero.
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
	jz	D3D_TCL_ShortCut2		; continue if result zero.
;
; Restore edx
	mov	edx,_gPDC[pV0]			; eax = edx = pV0
	jmp	D3D_TCL_NormDet				; go normal route
;
D3D_TCL_NormalPath:
;
; Determinate is very small or negative
;
	xor	eax,080000000h		; eax = - fDet
;
; Check for anticlockwise culling
	cmp	ecx, SGL_CULL_ANTICLOCK
	je	D3D_TCL_ShortCut2		; If culling then TCL_ShortCut2
;
; Invert the Determinate
;
;	Reload the values for the time being.
;
   mov	ebp,LESS_THAN_LARGE_EPSILON	; ebp = MAGIC_NUM_LARGE
   mov	edx,LESS_THAN_EPSILON				; edx = MAGIC_NUM
;
	cmp	eax,ebp
	 jge	D3D_TCL_NormalPath2	; if fDet < MAGIC_NUM_LARGE continue
;
	cmp	eax,edx
	 jl	D3D_TCL_ShortCut2		; if fDet < MAGIC_NUM D3D_TCL_ShortCut2
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
	jz	D3D_TCL_ShortCut2		; continue if result zero.
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
	jz	D3D_TCL_ShortCut2		; continue if result zero.
;
D3D_TCL_NormalPath2:
;
; Determinate is significant and was negative, invert/swap related values
;
	fchs				; -AdjX0,AdjX2
	fxch	st(1)			; AdjX2,-AdjX0
	fchs				; -AdjX2,-AdjX0
       mov	[LfDet],eax		; fDet = eax = -fDet
       mov	eax,[esi][Adj2][4]	; eax = AdjX1 = pTri->Adj2[1]
       xor	eax,080000000h		; eax = -AdjX1
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
;
	jmp D3D_TCL_NormDet
;
; New checking for clockwise culling
;
D3D_TCL_CheckCW:
	cmp	ecx, SGL_CULL_CLOCKWISE
	je	D3D_TCL_ShortCut2		; If culling then D3D_TCL_ShortCut2
;
D3D_TCL_NormDet:
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
;
       mov	ecx,OFFSET FLAT: _ProcessD3DFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
	jne	D3D_TCL_CheckNonPer
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
	jmp	D3D_TCL_DoWTex

D3D_TCL_ShortCut2:							; Discard 2 fpu values
	fstp	st(0)
	fstp	st(0)
	jmp	D3D_TCL_Loop						; Next poly please

D3D_TCL_ByPtr:
       push	edx							; Save edx and pass pTri as
	push	esi							; only para.
	call	eax							; Call _ProcessFlatTexFn
       pop	esi							; Recover state; edx is the
	pop	edx								; only reg is use not saved
	jmp	D3D_TCL_DoneTex						; by the Intel compiler.
;
D3D_TCL_DoWTex:
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
	jmp		D3D_TCL_DoneTex			; Finished perspective texturing case.
;
; Start of non-perspective cases. Both SGLDirect & D3D
;
D3D_TCL_CheckNonPer:
       or	eax,eax
	jz	D3D_TCL_DoneTex
    cmp		eax,OFFSET FLAT: _ProcessFlatTexNonPer
	jne		D3D_TCL_ByPtr
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
D3D_TCL_DoWTexNonPer:
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
D3D_TCL_DoneTex:
	mov		eax,_gPDC[Ctx][u32Flags]	; eax = gPDC.Context.u32Flags
	mov		ecx,8192					; ecx = ZBUFFER_MASK
	test	eax,ecx						; Flags = u32Flags & ZBUFFER_MASK
	jne		D3D_TCL_NoZBuff					; If u32Flags & 8192 then D3D_TCL_NoZBuff
;
; For D3D we have been requested to use Z v's 1/w for ISP surface removal.
;
if USERHW
;
; Provide 1/w for the ISP
;
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
else
;
; Provide Z (in face 1-Z) for the ISP
;
	fld		DWORD PTR _gfInvertZ		; 1
	fsub	DWORD PTR rpV0[V_fZ]		; 1-fZ0
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ0)
	fsub	DWORD PTR rpV2[V_fZ]		; 1-fZ2,MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ2),MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fsub	DWORD PTR rpV1[V_fZ]		; 1-fZ1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ1),MinZ*(1-fZ2),MinZ*(1-fZ0)
	fxch	st(2)						; MinZ*(1-fZ0),MinZ*(1-fZ2),MinZ*(1-fZ1)
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
;
; To prevent Z of > 1 (which incidenetly are illegal in D3D) from appearing behind
; the viewer clamp the calculated 1/w (1-z) value to 0. 
; The 3 calculated fZ's are checked.
;
	xor		eax, eax
	test	DWORD PTR [esi][fZ][0], 080000000h
	setnz	al
		xor		ecx, ecx
		test	DWORD PTR [esi][fZ][8], 080000000h
		setnz	cl
	sub		eax, 1
		sub		ecx, 1
	and		DWORD PTR [esi][fZ][0], eax
			and		DWORD PTR [esi][fZ][8], ecx
			xor		edx, edx
			test	DWORD PTR [esi][fZ][4], 080000000h
			setnz	dl
			sub		edx, 1
			and		DWORD PTR [esi][fZ][4], edx
endif
;
if Z_BUFFER_SIM
;
; Set the Bogus Z value to the greatest 1/w value. Helps apps which switch
; the Z buffer ON/OFF. It isn't a perfect Z buffer system but heh...
;
	mov		ecx, DWORD PTR [esi][fZ][0]
	mov		eax, DWORD PTR [esi][fZ][8]
	cmp		ecx, DWORD PTR _gfBogusInvZ
	mov		edx, DWORD PTR [esi][fZ][4]
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, ecx
@@:	cmp		eax, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, eax
@@:	cmp		edx, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, edx
@@:
endif
;
	jmp	D3D_TCL_Cont1						; goto D3D_TCL_Cont1

D3D_TCL_ShortCut1:							; Dicard 1 fpu value
	fstp	st(0)
D3D_TCL_ShortCut0:
	jmp	D3D_TCL_Loop						; Next poly please

D3D_TCL_NoZBuff:
	fld	QWORD PTR _2_1_float_13			; c1=BOGUSZ_INCREMENT
	fadd	DWORD PTR _gfBogusInvZ		; c1+gfBogusInvZ
	mov	eax,_gfBogusInvZ				; eax=gfBogusInvZ
	mov	[esi][fZ][0],eax				; pTri->fZ[0]=eax
	mov	[esi][fZ][4],eax				; pTri->fZ[1]=eax
	mov	[esi][fZ][8],eax				; pTri->fZ[2]=eax
	fstp	DWORD PTR _gfBogusInvZ		; gfBogusInvZ=
D3D_TCL_Cont1:
;
; Using integer comparisons set ecx to min X and eax to max X
;

	mov	ebp, TriAddr			; Transorting, load pointer to array

	mov	ecx,DWORD PTR fX2		; Transorting
	mov	DWORD PTR [ebp][fX][8], ecx 	; Transorting, copy X2

	mov	eax,DWORD PTR fX0	; eax = pV0->fX
	mov	ecx,DWORD PTR fX1	; ecx = pV1->fX

	mov	DWORD PTR [ebp][fX][0], eax 	; Transorting, copy X0
	mov	DWORD PTR [ebp][fX][4], ecx 	; Transorting, copy X1

	cmp	eax,ecx			; Flags = pV0->fX - pV1->fX
	jge	@F			; If eax >= ecx carry on
	mov	eax,ecx			; eax = Max value
	mov	ecx,DWORD PTR fX0	; ecx = Min value
@@:	cmp	eax,DWORD PTR fX2	; Flags = Max value - pV2->fX
	jge	@F			; If Max value > pV2->fX carry on
	mov	eax,DWORD PTR fX2	; Max value = pV2->fX
	jmp	D3D_TCL_XDone		; goto D3D_TCL_XDone (cannot be min too)
@@:	cmp	ecx,DWORD PTR fX2	; Flags = Min value - pV2->fX
	jle	D3D_TCL_XDone		; If Min value < pV2->fX carry on
	mov	ecx,DWORD PTR fX2	; Min value = pV2->fX
;
; Now eax = X1 (Max X) and ecx = X0 (Min X) as floating point, we check here
; if X1 (Max X) is negative as this indicates an off-screen poly that can
; be skipped. If X1 is +ive but X0 is -ive then we set X0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
D3D_TCL_XDone:
	or	eax,eax			; Flags = eax & eax (Max X)
	js	D3D_TCL_ShortCut0		; if ( Max X < 0 ) next poly please
	or	ecx,ecx			; Flags = ecx & ecx (Min X)
	jns	@F			; if ( Min X >= 0 ) carry on
	xor	ecx,ecx			; Make MinX = 0
;
; Using integer comparisons set ecx to min Y and eax to max Y
;
@@:	mov	DWORD PTR [LrX0],ecx	; Store minX as rX0
	mov	ecx,DWORD PTR fY1	; ecx = pV1->fY

	mov	DWORD PTR [ebp][fY][4], ecx 	; Transorting, copy Y1

	mov	DWORD PTR [LrX1],eax	; Store maxX as rX1
	mov	eax,DWORD PTR fY0	; eax = pV0->fY
	mov	edi,DWORD PTR fY2	; edi = pV2->fY

	mov	DWORD PTR [ebp][fY][0], eax 	; Transorting, copy Y0
	mov	DWORD PTR [ebp][fY][8], edi 	; Transorting, copy Y2
;
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
	jmp	D3D_TCL_YDone		; goto D3D_TCL_YDone
@@:	cmp	ecx,edi			; Flags = Min value - pV2->fY
	jle	D3D_TCL_YDone		; If Min value < pV2->fY D3D_TCL_YDone
	mov	ecx,edi			; Store Min value = pV2->fY
D3D_TCL_YDone:
;
; Now eax = Y1 (Max Y) and ecx = Y0 (Min Y) as floating point, we check here
; if Y1 (Max Y) is negative as this indicates an off-screen poly that can
; be skipped. If Y1 is +ive but Y0 is -ive then we set Y0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
	or	eax,eax			; Flags = eax & eax (Max Y)
	js	D3D_TCL_ShortCut0		; if ( Max Y < 0 ) next poly please
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
;
; D3D we always do clipping.
;	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
;	or	eax,eax			; Flags = eax | eax
;	je	D3D_TCL_NoLimits		; if ( !bDoClipping ) D3D_TCL_NoLimits
;
;
; Apply [First/Last][X/Y]Region if required, likely case is for no clipping
; to occur.
;
	mov	eax,_gPDC[Ctx][FirstXRegion]	; eax = FirstXRegion
	mov	ebp,_gPDC[Ctx][LastXRegion]	; ebp = LastXRegion
	cmp	ebx,eax			; Flags = rX0 - FirstXReg
	jge	@F			; If rX0 >= FirstXReg skip clip
	cmp	edx,eax			; Flags = rX1 - FirstXReg
	jl	D3D_TCL_ShortCut1		; If rX1 < FirstXReg skip poly
	mov	ebx,eax			; rX0 = FirstXReg (very rare)
@@:	cmp	edx,ebp			; Flags = rX1 - LastXReg
	jle	@F			; If rX1 <= LastXReg skip clip
	cmp	ebx,ebp			; Flags = rX0 - LastXReg
	jg	D3D_TCL_ShortCut1		; If rX1 > LastXReg skip poly
	mov	edx,ebp			; rX1 = LastXReg (very rare)
@@:	mov	eax,_gPDC[Ctx][FirstYRegion]	; eax = FirstYRegion
	mov	ebp,_gPDC[Ctx][LastYRegion]	; ebp = LastYRegion
	cmp	edi,eax			; Flags = rY0 - FirstYReg
	jge	@F			; If rY0 >= FirstYReg skip clip
	cmp	ecx,eax			; Flags = rY1 - FirstYReg
	jl	D3D_TCL_ShortCut1		; If rY1 < FirstYReg skip poly
	mov	edi,eax			; rY0 = FirstYReg (very rare)
	and	edi,0fffffffeh		; ensure LSB of Y value stays zero
@@:	cmp	ecx,ebp			; Flags = rY1 - LastYReg
	jle	@F			; If rY1 <= LastYReg skip clip
	cmp	edi,ebp			; Flags = rY0 - LastYReg
	jg	D3D_TCL_ShortCut1		; If rY1 > LastYReg skip poly
	mov	ecx,ebp			; rY1 = LastYReg (very rare)
	and	ecx,0fffffffeh		; ensure LSB of Y value stays zero
@@:
;
; Region bounding box now established in ebx-edx,edi-ecx
;
D3D_TCL_NoLimits:
	shl	ebx,5			; ebx <<= 5  (X0)
	add	ebx,edx			; ebx = (X0<<5)+X1
;
; Back to the main track, ebp = uCurrentType, bounding box = ebx-edx,edi-ecx
;	(LSBs of Y values have already been masked off earlier).
	mov	eax,DWORD PTR [PpAddfn]	; eax = pAddfn
	shl	edi,18			; edi <<= (19-1) (Y0)
	mov	ebp,edi			; ebp = edi (Y0)
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
@@:	mov	ecx, TriAddr			; Transorting	
	add	ebp,PIMAT_SIZE		; gpMatCurrent++
	add	ecx, PITRIPLUS_SIZE		; Transorting
	add	esi,PITRI_SIZE		; pTri++
	mov	eax,[OFFSET FLAT: _gpTri][PITRI_SIZE*IBUFFERSIZE]
	mov	DWORD PTR _gpMatCurrent, ebp
	mov	TriAddr, ecx  			; Transorting, gpTriPlus++

	cmp	eax,esi			; Flags = gpTri[IBUFFERSIZE]-pTri
	jne	D3D_TCL_Loop		; If not at end of buffer D3D_TCL_Loop
D3D_TCL_Done:
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
__ProcessD3DTransTriCoreLite ENDP
;_ProcessD3DTransTriCoreLite	ENDS
;**********************************************************************************


;**********************************************************************************
	ALIGN	4			; 0
	PUBLIC	__ProcessD3DTransCorePrimitive
__ProcessD3DTransCorePrimitive	PROC NEAR

	LCallPara	TEXTEQU <esp+	0>
	LfDet		TEXTEQU <esp+	4>
	LrX1		TEXTEQU <esp+   8>
	LrX0		TEXTEQU <esp+  12>
	LrY1		TEXTEQU <esp+  16>
	LrY0		TEXTEQU <esp+  20>
	Adjust		TEXTEQU <24>
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
	PFaceIndex	TEXTEQU <esp+  28+Adjust> ; 160

       push	edi			; Save registers
	push	esi
       push	ebp
	push	ebx
       sub	esp,Adjust		; Preserve local space
	mov	esi,OFFSET FLAT: _gpTri	; ESI = gpTri
       mov     edx,_gPDC[TSPControlWord]; edx = TSPControlWord

	mov	ebp,OFFSET FLAT: _gpTriPlus	; Transorting, Ebp = gpTriPlus
	mov	TriAddr,ebp			; Transorting

TCLTLS_Loop:
;
; Main loop: esi = pTri
;  	     eax, ebx, ecx, edx, ebp, edi = undefined
;
		mov	ebx,_gPDC[nPolys]	; ebx = nPolys
       	mov edx,_bStrip				;avoid stalling because of dependencies in bStrip & nFace & 1
       	dec	ebx			; ebx = nPolys - 1
		js	TCLTLS_Done		; If -ive nPolys then TCLTLS_Done
       	
       	mov	eax,_nFaces
       	mov	_gPDC[nPolys],ebx	; nPolys = nPolys - 1 = ebx
        mov	edi,_gPDC[pFace]
		mov ebp,65535;				;mask
		test edi,edi				;FIX: MOV does NOT affect the flags
		jz TCLTLS_NonIndexed			; Indexed or NonIndexed ???
        and edx,1					;edx=bStrip & 1
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
;
TCLTLS_Indexed:
		and edx,eax					;edx=nFaces & 1 & bStrip
									;TEST edx,eax should work as well
		jz TCLTLS_Indexed_else

TCLTLS_Indexed_if:		
		mov ebx,[edi+eax*2]		; load 2 vertices at a time	   
	   	
	   	inc eax					;next vertex: "false" increment
		mov edx,ebx				;copy 1st and second vertex
		and	ebx,ebp					;get first vertex by masking out upper 16bits
		shr	edx,16					;edx=2nd vertex
		
		inc eax					;next vertex
		mov di,[edi+eax*2]		;FIX:changed edi to di because it might cause page faults
								; 1 cc penalty + 1 byte	
		
		add	eax,[PFaceIndex] 
		and edi,ebp				;remove upper 16bits 
		jmp	TCLTLS_FloatIt					;done
		
TCLTLS_Indexed_else:
	   	mov edx,[edi+eax*2]		   ;edi= *(pFaces+nFaces)
		
		inc eax					;next vertex
		mov ebx,edx
		and edx,ebp				    ;fetched DWORD, remove upper 16bits
		shr ebx,16
				
		inc eax					;next vertex
		mov di,[edi+eax*2]			
	  	
	  	add	eax,[PFaceIndex] ;POS:common to all if-else blocks above
		and edi,ebp					;remove upper 16bits
	  	jmp TCLTLS_FloatIt


TCLTLS_NonIndexed:
		and edx,1					;avoid stalling because of dependencies in bStrip & nFace & 1
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
		
		and edx,eax					;;edx = nFaces & 1 & bStrip
		jz  TCLTLS_NonIndexed_else
TCLTLS_NonIndexed_if:
		mov	ebx,eax
		inc eax
		mov edx,eax
		inc eax
		mov edi,eax
		
		add	eax,[PFaceIndex] ;POS:common to all if-else blocks above
		jmp TCLTLS_FloatIt
TCLTLS_NonIndexed_else:
       mov	edx,eax
	   inc	eax
	   mov	ebx,eax
	   inc	eax
       mov	edi,eax
	   add	eax,[PFaceIndex] ;POS:common to all if-else blocks above

TCLTLS_FloatIt:
    shl	edi,5			; edi *= sizeof(VERTEX)
	mov	_nFaces,eax	; pFace++ = eax
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
;
; Read culling mode here.
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
;
; Check for clockwise culling if det. greater than MAGIC_NUM_LARGE and positive.
;
	jge	TCLTLS_CheckCW						; if fDet < MAGIC_NUM_LARGE	check for clockwise culling
;
       mov	edx,LESS_THAN_EPSILON		; edx = MAGIC_NUM
       cmp	eax,edx						; Flags = eax - MAGIC_NUM
	jl	TCLTLS_NormalPath				; if fDet >= MAGIC_NUM then TCLTLS_NormalPath
;
; Check to see if integer X the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fX0				; int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX0)
	fadd	DWORD PTR fX1				; int(fX1), int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fX0)
	fadd	DWORD PTR fX2				; int(fX2), int(fX1), int(fX0)
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
	jz	TCLTLS_ShortCut2					; continue if result zero.
;
; Check to see if integer Y's the same. fY0, fY1, fY2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fY0				; int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fY0)
	fadd	DWORD PTR fY1				; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2				; int(fY2), int(fY1), int(fY0)
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
	jz	TCLTLS_ShortCut2					; continue if result zero.
;
; Restore edx
	mov	edx,_gPDC[pV0]					; eax = edx = pV0
	jmp	TCLTLS_NormDet					; go normal route
;
TCLTLS_NormalPath:
;
; Determinate is very small or negative
;
	xor	eax,080000000h					; eax = - fDet
;
; Check for anticlockwise culling
	cmp	ecx, SGL_CULL_ANTICLOCK
	je	TCLTLS_ShortCut2					; If culling then TCLTLS_ShortCut2
;
; Invert the Determinate
;
;	Reload the values for the time being.
;
   mov	ebp,LESS_THAN_LARGE_EPSILON		; ebp = MAGIC_NUM_LARGE
   mov	edx,LESS_THAN_EPSILON			; edx = MAGIC_NUM
;
	cmp	eax,ebp
	 jge	TCLTLS_NormalPath2			; if fDet < MAGIC_NUM_LARGE continue
;
	cmp	eax,edx
	 jl	TCLTLS_ShortCut2					; if fDet < MAGIC_NUM TCLTLS_ShortCut2
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
	jz	TCLTLS_ShortCut2				; continue if result zero.
;
; Check to see if integer Y's the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]		; MAGIC
	fadd	DWORD PTR fY0			; int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fY0)
	fadd	DWORD PTR fY1			; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2			; int(fY2), int(fY1), int(fY0)
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
	jz	TCLTLS_ShortCut2				; continue if result zero.
;
TCLTLS_NormalPath2:
;
; Determinate is significant and was negative, invert/swap related values
;
	fchs				; -AdjX0,AdjX2
	fxch	st(1)			; AdjX2,-AdjX0
	fchs				; -AdjX2,-AdjX0
       mov	[LfDet],eax		; fDet = eax = -fDet
       mov	eax,[esi][Adj2][4]	; eax = AdjX1 = pTri->Adj2[1]
       xor	eax,080000000h		; eax = -AdjX1
       mov	[esi][Adj2][4],eax	; pTri->Adj2[1] = -AdjX1
;
; To free up a register read from memory !!!!!
;	mov	eax,edx			; eax = edx = pV0
	mov	eax,_gPDC[pV0]			; eax = pV0
;
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
;
	jmp TCLTLS_NormDet
;
; New checking for clockwise culling
;
TCLTLS_CheckCW:
	cmp	ecx, SGL_CULL_CLOCKWISE
	je	TCLTLS_ShortCut2		; If culling then TCLTLS_ShortCut2
;
TCLTLS_NormDet:
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
;
       mov	ecx,OFFSET FLAT: _ProcessD3DFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
	jne	TCLTLS_CheckNonPer
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
	jmp	TCLTLS_DoWTex
;
TCLTLS_ShortCut2:							; Discard 2 fpu values
	fstp	st(0)
	fstp	st(0)
	jmp	TCLTLS_Loop						; Next poly please

TCLTLS_ByPtr:
       push	edx							; Save edx and pass pTri as
	push	esi							; only para.
	call	eax							; Call _ProcessFlatTexFn
       pop	esi							; Recover state; edx is the
	pop	edx								; only reg is use not saved
	jmp	TCLTLS_DoneTex						; by the Intel compiler.
;
TCLTLS_DoWTex:
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
	jmp		TCLTLS_DoneTex			; Finished perspective texturing case.
;
; Start of non-perspective cases. Both SGLDirect & D3D
;
TCLTLS_CheckNonPer:
       or	eax,eax
	jz	TCLTLS_DoneTex
    cmp		eax,OFFSET FLAT: _ProcessFlatTexNonPer
	jne		TCLTLS_ByPtr
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
TCLTLS_DoWTexNonPer:
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
TCLTLS_DoneTex:
	mov		eax,_gPDC[Ctx][u32Flags]	; eax = gPDC.Context.u32Flags
	mov		ecx,8192					; ecx = ZBUFFER_MASK
	test	eax,ecx						; Flags = u32Flags & ZBUFFER_MASK
	jne		TCLTLS_NoZBuff					; If u32Flags & 8192 then TCLTLS_NoZBuff
;
; For D3D we have been requested to use Z v's 1/w for ISP surface removal.
;
if USERHW
;
; Provide 1/w for the ISP
;
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
else
	fld		DWORD PTR _gfInvertZ		; 1
	fsub	DWORD PTR rpV0[V_fZ]		; 1-fZ0
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ0)
	fsub	DWORD PTR rpV2[V_fZ]		; 1-fZ2,MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ2),MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fsub	DWORD PTR rpV1[V_fZ]		; 1-fZ1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ1),MinZ*(1-fZ2),MinZ*(1-fZ0)
	fxch	st(2)						; MinZ*(1-fZ0),MinZ*(1-fZ2),MinZ*(1-fZ1)
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
;
; To prevent Z of > 1 (which incidenetly are illegal in D3D) from appearing behind
; the viewer clamp the calculated 1/w (1-z) value to 0. 
; The 3 calculated fZ's are checked.
;
	xor		eax, eax
	test	DWORD PTR [esi][fZ][0], 080000000h
	setnz	al
		xor		ecx, ecx
		test	DWORD PTR [esi][fZ][8], 080000000h
		setnz	cl
	sub		eax, 1
		sub		ecx, 1
	and		DWORD PTR [esi][fZ][0], eax
			and		DWORD PTR [esi][fZ][8], ecx
			xor		edx, edx
			test	DWORD PTR [esi][fZ][4], 080000000h
			setnz	dl
			sub		edx, 1
			and		DWORD PTR [esi][fZ][4], edx
endif
;
if Z_BUFFER_SIM
;
; Set the Bogus Z value to the greatest 1/w value. Helps apps which switch
; the Z buffer ON/OFF. It isn't a perfect Z buffer system but heh...
;
	mov		ecx, DWORD PTR [esi][fZ][0]
	mov		eax, DWORD PTR [esi][fZ][8]
	cmp		ecx, DWORD PTR _gfBogusInvZ
	mov		edx, DWORD PTR [esi][fZ][4]
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, ecx
@@:	cmp		eax, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, eax
@@:	cmp		edx, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, edx
@@:
endif
;
	jmp	TCLTLS_Cont1						; goto TCLTLS_Cont1

TCLTLS_ShortCut1:							; Dicard 1 fpu value
	fstp	st(0)
TCLTLS_ShortCut0:
	jmp	TCLTLS_Loop						; Next poly please

TCLTLS_NoZBuff:
	fld	QWORD PTR _2_1_float_13			; c1=BOGUSZ_INCREMENT
	fadd	DWORD PTR _gfBogusInvZ		; c1+gfBogusInvZ
	mov	eax,_gfBogusInvZ				; eax=gfBogusInvZ
	mov	[esi][fZ][0],eax				; pTri->fZ[0]=eax
	mov	[esi][fZ][4],eax				; pTri->fZ[1]=eax
	mov	[esi][fZ][8],eax				; pTri->fZ[2]=eax
	fstp	DWORD PTR _gfBogusInvZ		; gfBogusInvZ=
TCLTLS_Cont1:
;
; Interleave DAG sorting stuff.
;
	mov	ebp, TriAddr			; Transorting, load pointer to array

	mov	ecx,DWORD PTR fX2		; Transorting
	mov	DWORD PTR [ebp][fX][8], ecx 	; Transorting, copy X2

	mov	eax,DWORD PTR fX0	; eax = pV0->fX
	mov	ecx,DWORD PTR fX1	; ecx = pV1->fX

	mov	DWORD PTR [ebp][fX][0], eax 	; Transorting, copy X0
	mov	DWORD PTR [ebp][fX][4], ecx 	; Transorting, copy X1
;
; Using integer comparisons set ecx to min X and eax to max X
;
	cmp	eax,ecx			; Flags = pV0->fX - pV1->fX
	jge	@F			; If eax >= ecx carry on
	mov	eax,ecx			; eax = Max value
	mov	ecx,DWORD PTR fX0	; ecx = Min value
@@:	cmp	eax,DWORD PTR fX2	; Flags = Max value - pV2->fX
	jge	@F			; If Max value > pV2->fX carry on
	mov	eax,DWORD PTR fX2	; Max value = pV2->fX
	jmp	TCLTLS_XDone		; goto TCLTLS_XDone (cannot be min too)
@@:	cmp	ecx,DWORD PTR fX2	; Flags = Min value - pV2->fX
	jle	TCLTLS_XDone		; If Min value < pV2->fX carry on
	mov	ecx,DWORD PTR fX2	; Min value = pV2->fX
;
; Now eax = X1 (Max X) and ecx = X0 (Min X) as floating point, we check here
; if X1 (Max X) is negative as this indicates an off-screen poly that can
; be skipped. If X1 is +ive but X0 is -ive then we set X0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
TCLTLS_XDone:
	or	eax,eax			; Flags = eax & eax (Max X)
	js	TCLTLS_ShortCut0		; if ( Max X < 0 ) next poly please
	or	ecx,ecx			; Flags = ecx & ecx (Min X)
	jns	@F			; if ( Min X >= 0 ) carry on
	xor	ecx,ecx			; Make MinX = 0
;
; Interleave DAG sorting stuff.
;
@@:	mov	DWORD PTR [LrX0],ecx	; Store minX as rX0
	mov	ecx,DWORD PTR fY1	; ecx = pV1->fY

	mov	DWORD PTR [ebp][fY][4], ecx 	; Transorting, copy Y1

	mov	DWORD PTR [LrX1],eax	; Store maxX as rX1
	mov	eax,DWORD PTR fY0	; eax = pV0->fY
	mov	edi,DWORD PTR fY2	; edi = pV2->fY

	mov	DWORD PTR [ebp][fY][0], eax 	; Transorting, copy Y0
	mov	DWORD PTR [ebp][fY][8], edi 	; Transorting, copy Y2
;
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
	jmp	TCLTLS_YDone		; goto TCLTLS_YDone
@@:	cmp	ecx,edi			; Flags = Min value - pV2->fY
	jle	TCLTLS_YDone		; If Min value < pV2->fY TCLTLS_YDone
	mov	ecx,edi			; Store Min value = pV2->fY
TCLTLS_YDone:
;
; Now eax = Y1 (Max Y) and ecx = Y0 (Min Y) as floating point, we check here
; if Y1 (Max Y) is negative as this indicates an off-screen poly that can
; be skipped. If Y1 is +ive but Y0 is -ive then we set Y0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
	or	eax,eax			; Flags = eax & eax (Max Y)
	js	TCLTLS_ShortCut0		; if ( Max Y < 0 ) next poly please
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
;
; D3D we always do clipping.
;	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
;	or	eax,eax			; Flags = eax | eax
;	je	TCLTLS_NoLimits		; if ( !bDoClipping ) TCLTLS_NoLimits
;
;
; Apply [First/Last][X/Y]Region if required, likely case is for no clipping
; to occur.
;
	mov	eax,_gPDC[Ctx][FirstXRegion]	; eax = FirstXRegion
	mov	ebp,_gPDC[Ctx][LastXRegion]	; ebp = LastXRegion
	cmp	ebx,eax			; Flags = rX0 - FirstXReg
	jge	@F			; If rX0 >= FirstXReg skip clip
	cmp	edx,eax			; Flags = rX1 - FirstXReg
	jl	TCLTLS_ShortCut1		; If rX1 < FirstXReg skip poly
	mov	ebx,eax			; rX0 = FirstXReg (very rare)
@@:	cmp	edx,ebp			; Flags = rX1 - LastXReg
	jle	@F			; If rX1 <= LastXReg skip clip
	cmp	ebx,ebp			; Flags = rX0 - LastXReg
	jg	TCLTLS_ShortCut1		; If rX1 > LastXReg skip poly
	mov	edx,ebp			; rX1 = LastXReg (very rare)
@@:	mov	eax,_gPDC[Ctx][FirstYRegion]	; eax = FirstYRegion
	mov	ebp,_gPDC[Ctx][LastYRegion]	; ebp = LastYRegion
	cmp	edi,eax			; Flags = rY0 - FirstYReg
	jge	@F			; If rY0 >= FirstYReg skip clip
	cmp	ecx,eax			; Flags = rY1 - FirstYReg
	jl	TCLTLS_ShortCut1		; If rY1 < FirstYReg skip poly
	mov	edi,eax			; rY0 = FirstYReg (very rare)
	and	edi,0fffffffeh		; ensure LSB of Y value stays zero
@@:	cmp	ecx,ebp			; Flags = rY1 - LastYReg
	jle	@F			; If rY1 <= LastYReg skip clip
	cmp	edi,ebp			; Flags = rY0 - LastYReg
	jg	TCLTLS_ShortCut1		; If rY1 > LastYReg skip poly
	mov	ecx,ebp			; rY1 = LastYReg (very rare)
	and	ecx,0fffffffeh		; ensure LSB of Y value stays zero
@@:
;
; Region bounding box now established in ebx-edx,edi-ecx
;
TCLTLS_NoLimits:
	shl	ebx,5			; ebx <<= 5  (X0)
	add	ebx,edx			; ebx = (X0<<5)+X1
;
	shl	edi,18			; edi <<= (19-1) (Y0)
	mov	eax,DWORD PTR [PpAddfn]	; eax = pAddfn
;	add	ebp,edi			; ebp += edi (Y0)
	mov	ebp,edi			; ebp += edi (Y0)
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
@@:	mov	ecx, TriAddr			; Transorting	
	add	ebp,PIMAT_SIZE		; gpMatCurrent++
	add	ecx, PITRIPLUS_SIZE		; Transorting
	add	esi,PITRI_SIZE		; pTri++
	mov	eax,[OFFSET FLAT: _gpTri][PITRI_SIZE*IBUFFERSIZE]
	mov	DWORD PTR _gpMatCurrent, ebp
	mov	TriAddr, ecx  			; Transorting, gpTriPlus++
;
	cmp	eax,esi			; Flags = gpTri[IBUFFERSIZE]-pTri
	jne	TCLTLS_Loop		; If not at end of buffer TCLTLS_Loop
TCLTLS_Done:
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
__ProcessD3DTransCorePrimitive ENDP
;_ProcessD3DTransCorePrimitive	ENDS


;**********************************************************************************
	ALIGN	4			; 0
	PUBLIC	__ProcessD3DTransFanCorePrimitive
__ProcessD3DTransFanCorePrimitive	PROC NEAR

	LCallPara	TEXTEQU <esp+	0>
	LfDet		TEXTEQU <esp+	4>
	LrX1		TEXTEQU <esp+   8>
	LrX0		TEXTEQU <esp+  12>
	LrY1		TEXTEQU <esp+  16>
	LrY0		TEXTEQU <esp+  20>
	Adjust		TEXTEQU <24>
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
	PFaceIndex	TEXTEQU <esp+  28+Adjust> ; 160

       push	edi			; Save registers
	push	esi
       push	ebp
	push	ebx
       sub	esp,Adjust		; Preserve local space
	mov	esi,OFFSET FLAT: _gpTri	; ESI = gpTri

	mov	ebp,OFFSET FLAT: _gpTriPlus	; Transorting, Ebp = gpTriPlus
	mov	TriAddr,ebp			; Transorting
	
TCTLF_Loop:
;
; Main loop: esi = pTri
;  	     eax, ebx, ecx, edx, ebp, edi = undefined
;
		mov	ebx,_gPDC[nPolys]	; ebx = nPolys
       	mov	eax,_nFaces
       	dec	ebx			; ebx = nPolys - 1
		js	TCTLF_Done		; If -ive nPolys then TCTLF_Done
       	
       	mov	_gPDC[nPolys],ebx	; nPolys = nPolys - 1 = ebx
        mov	edi,_gPDC[pFace]
		mov ebp,65535;				;mask
		test edi,edi				
		jz TCTLF_NonIndexed			; Indexed or NonIndexed ???
TCTLF_Indexed:
								
	    mov ebx,[edi+eax*2]		;load 2nd and 3rd vertices			
		
		inc eax											
		
		mov edx,[edi]			;load first vertex 	   
	   	mov edi,ebx				;copy 3rd vertex
		and	edx,ebp				;mask out upper 16bits(get rid of junk)
								;first vertex ready
		and ebx,ebp				;mask out upper 16bits(get rid of 3rd vertex)
								;second vertex ready
		shr	edi,16				;third vertex ready	
		
		
		jmp	TCTLF_FloatIt					;done
		

TCTLF_NonIndexed:
		
		xor edx,edx			  ; 0	  
		mov ebx,eax			  ; 2nd vertex
		inc eax
		mov edi,eax			  ; 3rd vertex
		
;
;
; End of vertex determination.
;
TCTLF_FloatIt:
    mov	ecx,_gPDC[pVertices]	; ecx = pVertices
    shl	edi,5			; edi *= sizeof(VERTEX)
	mov	_nFaces,eax			; pFace++ = eax
    shl	ebx,5			; ebx *= sizeof(VERTEX)
    add	edi,ecx			; EDI = pVertices + (edi*Size)
    shl	edx,5			; edx *= sizeof(VERTEX)
	add	ebx,ecx			; EBX = pVertices + (ebx*Size)
    add	edx,ecx			; EDX = pVertices + (edx*Size)
	mov eax,_gPDC[TSPControlWord]; eax = gPDC.TSPControlWord
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
;
; Read culling mode here.
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
;
; Check for clockwise culling if det. greater than MAGIC_NUM_LARGE and positive.
;
	jge	TCTLF_CheckCW						; if fDet < MAGIC_NUM_LARGE	check for clockwise culling
;
       mov	edx,LESS_THAN_EPSILON		; edx = MAGIC_NUM
       cmp	eax,edx						; Flags = eax - MAGIC_NUM
	jl	TCTLF_NormalPath					; if fDet >= MAGIC_NUM then TCTLF_NormalPath
;
; Check to see if integer X the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fX0				; int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX0)
	fadd	DWORD PTR fX1				; int(fX1), int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fX0)
	fadd	DWORD PTR fX2				; int(fX2), int(fX1), int(fX0)
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
	jz	TCTLF_ShortCut2					; continue if result zero.
;
; Check to see if integer Y's the same. fY0, fY1, fY2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fY0				; int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fY0)
	fadd	DWORD PTR fY1				; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2				; int(fY2), int(fY1), int(fY0)
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
	jz	TCTLF_ShortCut2					; continue if result zero.
;
; Restore edx
	mov	edx,_gPDC[pV0]					; eax = edx = pV0
	jmp	TCTLF_NormDet					; go normal route
;
TCTLF_NormalPath:
;
; Determinate is very small or negative
;
	xor	eax,080000000h					; eax = - fDet
;
; Check for anticlockwise culling
	cmp	ecx, SGL_CULL_ANTICLOCK
	je	TCTLF_ShortCut2					; If culling then TCTLF_ShortCut2
;
; Invert the Determinate
;
;	Reload the values for the time being.
;
   mov	ebp,LESS_THAN_LARGE_EPSILON		; ebp = MAGIC_NUM_LARGE
   mov	edx,LESS_THAN_EPSILON			; edx = MAGIC_NUM
;
	cmp	eax,ebp
	 jge	TCTLF_NormalPath2			; if fDet < MAGIC_NUM_LARGE continue
;
	cmp	eax,edx
	 jl	TCTLF_ShortCut2					; if fDet < MAGIC_NUM TCTLF_ShortCut2
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
	jz	TCTLF_ShortCut2				; continue if result zero.
;
; Check to see if integer Y's the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]		; MAGIC
	fadd	DWORD PTR fY0			; int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fY0)
	fadd	DWORD PTR fY1			; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2			; int(fY2), int(fY1), int(fY0)
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
	jz	TCTLF_ShortCut2				; continue if result zero.
;
TCTLF_NormalPath2:
;
; Determinate is significant and was negative, invert/swap related values
;
	fchs				; -AdjX0,AdjX2
	fxch	st(1)			; AdjX2,-AdjX0
	fchs				; -AdjX2,-AdjX0
       mov	[LfDet],eax		; fDet = eax = -fDet
       mov	eax,[esi][Adj2][4]	; eax = AdjX1 = pTri->Adj2[1]
       xor	eax,080000000h		; eax = -AdjX1
       mov	[esi][Adj2][4],eax	; pTri->Adj2[1] = -AdjX1
;
; To free up a register read from memory !!!!!
;	mov	eax,edx			; eax = edx = pV0
	mov	eax,_gPDC[pV0]			; eax = pV0
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
;
	jmp TCTLF_NormDet
;
; New checking for clockwise culling
;
TCTLF_CheckCW:
	cmp	ecx, SGL_CULL_CLOCKWISE
	je	TCTLF_ShortCut2		; If culling then TCTLF_ShortCut2
;
TCTLF_NormDet:
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
;
       mov	ecx,OFFSET FLAT: _ProcessD3DFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
	jne	TCTLF_CheckNonPer
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
	jmp	TCTLF_DoWTex
;
TCTLF_ShortCut2:							; Discard 2 fpu values
	fstp	st(0)
	fstp	st(0)
	jmp	TCTLF_Loop						; Next poly please

TCTLF_ByPtr:
       push	edx							; Save edx and pass pTri as
	push	esi							; only para.
	call	eax							; Call _ProcessFlatTexFn
       pop	esi							; Recover state; edx is the
	pop	edx								; only reg is use not saved
	jmp	TCTLF_DoneTex						; by the Intel compiler.
;
TCTLF_DoWTex:
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
	jmp		TCTLF_DoneTex			; Finished perspective texturing case.
;
; Start of non-perspective cases. Both SGLDirect & D3D
;
TCTLF_CheckNonPer:
       or	eax,eax
	jz	TCTLF_DoneTex
    cmp		eax,OFFSET FLAT: _ProcessFlatTexNonPer
	jne		TCTLF_ByPtr
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
TCTLF_DoWTexNonPer:
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
TCTLF_DoneTex:
	mov		eax,_gPDC[Ctx][u32Flags]	; eax = gPDC.Context.u32Flags
	mov		ecx,8192					; ecx = ZBUFFER_MASK
	test	eax,ecx						; Flags = u32Flags & ZBUFFER_MASK
	jne		TCTLF_NoZBuff					; If u32Flags & 8192 then TCTLF_NoZBuff
;
; For D3D we have been requested to use Z v's 1/w for ISP surface removal.
;
if USERHW
;
; Provide 1/w for the ISP
;
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
else
;
; Provide Z for the ISP
;
	fld		DWORD PTR _gfInvertZ		; 1
	fsub	DWORD PTR rpV0[V_fZ]		; 1-fZ0
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ0)
	fsub	DWORD PTR rpV2[V_fZ]		; 1-fZ2,MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ2),MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fsub	DWORD PTR rpV1[V_fZ]		; 1-fZ1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ1),MinZ*(1-fZ2),MinZ*(1-fZ0)
	fxch	st(2)						; MinZ*(1-fZ0),MinZ*(1-fZ2),MinZ*(1-fZ1)
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
;
; To prevent Z of > 1 (which incidenetly are illegal in D3D) from appearing behind
; the viewer clamp the calculated 1/w (1-z) value to 0. 
; The 3 calculated fZ's are checked.
;
	xor		eax, eax
	test	DWORD PTR [esi][fZ][0], 080000000h
	setnz	al
		xor		ecx, ecx
		test	DWORD PTR [esi][fZ][8], 080000000h
		setnz	cl
	sub		eax, 1
		sub		ecx, 1
	and		DWORD PTR [esi][fZ][0], eax
			and		DWORD PTR [esi][fZ][8], ecx
			xor		edx, edx
			test	DWORD PTR [esi][fZ][4], 080000000h
			setnz	dl
			sub		edx, 1
			and		DWORD PTR [esi][fZ][4], edx
;
endif
;
if Z_BUFFER_SIM
;
; Set the Bogus Z value to the greatest 1/w value. Helps apps which switch
; the Z buffer ON/OFF. It isn't a perfect Z buffer system but heh...
;
	mov		ecx, DWORD PTR [esi][fZ][0]
	mov		eax, DWORD PTR [esi][fZ][8]
	cmp		ecx, DWORD PTR _gfBogusInvZ
	mov		edx, DWORD PTR [esi][fZ][4]
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, ecx
@@:	cmp		eax, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, eax
@@:	cmp		edx, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, edx
@@:
endif
;
	jmp	TCTLF_Cont1						; goto TCTLF_Cont1

TCTLF_ShortCut1:							; Dicard 1 fpu value
	fstp	st(0)
TCTLF_ShortCut0:
	jmp	TCTLF_Loop						; Next poly please

TCTLF_NoZBuff:
	fld	QWORD PTR _2_1_float_13			; c1=BOGUSZ_INCREMENT
	fadd	DWORD PTR _gfBogusInvZ		; c1+gfBogusInvZ
	mov	eax,_gfBogusInvZ				; eax=gfBogusInvZ
	mov	[esi][fZ][0],eax				; pTri->fZ[0]=eax
	mov	[esi][fZ][4],eax				; pTri->fZ[1]=eax
	mov	[esi][fZ][8],eax				; pTri->fZ[2]=eax
	fstp	DWORD PTR _gfBogusInvZ		; gfBogusInvZ=
TCTLF_Cont1:
;
; Interleave DAG sorting stuff.
;
	mov	ebp, TriAddr			; Transorting, load pointer to array

	mov	ecx,DWORD PTR fX2		; Transorting
	mov	DWORD PTR [ebp][fX][8], ecx 	; Transorting, copy X2

	mov	eax,DWORD PTR fX0	; eax = pV0->fX
	mov	ecx,DWORD PTR fX1	; ecx = pV1->fX

	mov	DWORD PTR [ebp][fX][0], eax 	; Transorting, copy X0
	mov	DWORD PTR [ebp][fX][4], ecx 	; Transorting, copy X1
;
; Using integer comparisons set ecx to min X and eax to max X
;
	cmp	eax,ecx			; Flags = pV0->fX - pV1->fX
	jge	@F			; If eax >= ecx carry on
	mov	eax,ecx			; eax = Max value
	mov	ecx,DWORD PTR fX0	; ecx = Min value
@@:	cmp	eax,DWORD PTR fX2	; Flags = Max value - pV2->fX
	jge	@F			; If Max value > pV2->fX carry on
	mov	eax,DWORD PTR fX2	; Max value = pV2->fX
	jmp	TCTLF_XDone		; goto TCTLF_XDone (cannot be min too)
@@:	cmp	ecx,DWORD PTR fX2	; Flags = Min value - pV2->fX
	jle	TCTLF_XDone		; If Min value < pV2->fX carry on
	mov	ecx,DWORD PTR fX2	; Min value = pV2->fX
;
; Now eax = X1 (Max X) and ecx = X0 (Min X) as floating point, we check here
; if X1 (Max X) is negative as this indicates an off-screen poly that can
; be skipped. If X1 is +ive but X0 is -ive then we set X0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
TCTLF_XDone:
	or	eax,eax			; Flags = eax & eax (Max X)
	js	TCTLF_ShortCut0		; if ( Max X < 0 ) next poly please
	or	ecx,ecx			; Flags = ecx & ecx (Min X)
	jns	@F			; if ( Min X >= 0 ) carry on
	xor	ecx,ecx			; Make MinX = 0
;
; Interleave DAG sorting stuff.
;
@@:	mov	DWORD PTR [LrX0],ecx	; Store minX as rX0
	mov	ecx,DWORD PTR fY1	; ecx = pV1->fY

	mov	DWORD PTR [ebp][fY][4], ecx 	; Transorting, copy Y1

	mov	DWORD PTR [LrX1],eax	; Store maxX as rX1
	mov	eax,DWORD PTR fY0	; eax = pV0->fY
	mov	edi,DWORD PTR fY2	; edi = pV2->fY

	mov	DWORD PTR [ebp][fY][0], eax 	; Transorting, copy Y0
	mov	DWORD PTR [ebp][fY][8], edi 	; Transorting, copy Y2
;
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
	jmp	TCTLF_YDone		; goto TCTLF_YDone
@@:	cmp	ecx,edi			; Flags = Min value - pV2->fY
	jle	TCTLF_YDone		; If Min value < pV2->fY TCTLF_YDone
	mov	ecx,edi			; Store Min value = pV2->fY
TCTLF_YDone:
;
; Now eax = Y1 (Max Y) and ecx = Y0 (Min Y) as floating point, we check here
; if Y1 (Max Y) is negative as this indicates an off-screen poly that can
; be skipped. If Y1 is +ive but Y0 is -ive then we set Y0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
	or	eax,eax			; Flags = eax & eax (Max Y)
	js	TCTLF_ShortCut0		; if ( Max Y < 0 ) next poly please
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
;
; D3D we always do clipping.
;	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
;	or	eax,eax			; Flags = eax | eax
;	je	TCTLF_NoLimits		; if ( !bDoClipping ) TCTLF_NoLimits
;
;
; Apply [First/Last][X/Y]Region if required, likely case is for no clipping
; to occur.
;
	mov	eax,_gPDC[Ctx][FirstXRegion]	; eax = FirstXRegion
	mov	ebp,_gPDC[Ctx][LastXRegion]	; ebp = LastXRegion
	cmp	ebx,eax			; Flags = rX0 - FirstXReg
	jge	@F			; If rX0 >= FirstXReg skip clip
	cmp	edx,eax			; Flags = rX1 - FirstXReg
	jl	TCTLF_ShortCut1		; If rX1 < FirstXReg skip poly
	mov	ebx,eax			; rX0 = FirstXReg (very rare)
@@:	cmp	edx,ebp			; Flags = rX1 - LastXReg
	jle	@F			; If rX1 <= LastXReg skip clip
	cmp	ebx,ebp			; Flags = rX0 - LastXReg
	jg	TCTLF_ShortCut1		; If rX1 > LastXReg skip poly
	mov	edx,ebp			; rX1 = LastXReg (very rare)
@@:	mov	eax,_gPDC[Ctx][FirstYRegion]	; eax = FirstYRegion
	mov	ebp,_gPDC[Ctx][LastYRegion]	; ebp = LastYRegion
	cmp	edi,eax			; Flags = rY0 - FirstYReg
	jge	@F			; If rY0 >= FirstYReg skip clip
	cmp	ecx,eax			; Flags = rY1 - FirstYReg
	jl	TCTLF_ShortCut1		; If rY1 < FirstYReg skip poly
	mov	edi,eax			; rY0 = FirstYReg (very rare)
	and	edi,0fffffffeh		; ensure LSB of Y value stays zero
@@:	cmp	ecx,ebp			; Flags = rY1 - LastYReg
	jle	@F			; If rY1 <= LastYReg skip clip
	cmp	edi,ebp			; Flags = rY0 - LastYReg
	jg	TCTLF_ShortCut1		; If rY1 > LastYReg skip poly
	mov	ecx,ebp			; rY1 = LastYReg (very rare)
	and	ecx,0fffffffeh		; ensure LSB of Y value stays zero
@@:
;
; Region bounding box now established in ebx-edx,edi-ecx
;
TCTLF_NoLimits:
	shl	ebx,5			; ebx <<= 5  (X0)
	add	ebx,edx			; ebx = (X0<<5)+X1
;
	shl	edi,18			; edi <<= (19-1) (Y0)
	mov	eax,DWORD PTR [PpAddfn]	; eax = pAddfn
;	add	ebp,edi			; ebp += edi (Y0)
	mov	ebp,edi			; ebp += edi (Y0)
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
@@:	mov	ecx, TriAddr			; Transorting	
	add	ebp,PIMAT_SIZE		; gpMatCurrent++
	add	ecx, PITRIPLUS_SIZE		; Transorting
	add	esi,PITRI_SIZE		; pTri++
	mov	eax,[OFFSET FLAT: _gpTri][PITRI_SIZE*IBUFFERSIZE]
	mov	DWORD PTR _gpMatCurrent, ebp
	mov	TriAddr, ecx  			; Transorting, gpTriPlus++
;
	cmp	eax,esi			; Flags = gpTri[IBUFFERSIZE]-pTri
	jne	TCTLF_Loop		; If not at end of buffer TCTLF_Loop
TCTLF_Done:
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
__ProcessD3DTransFanCorePrimitive ENDP
;__ProcessD3DTransFanCorePrimitive	ENDS


;**********************************************************************************
	ALIGN	4			; 0
	PUBLIC	__ProcessD3DCorePrimitive
__ProcessD3DCorePrimitive	PROC NEAR

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
	PFaceIndex	TEXTEQU <esp+  28+Adjust> ; 160

       push	edi			; Save registers
	push	esi
       push	ebp
	push	ebx
       sub	esp,Adjust		; Preserve local space
	mov	esi,OFFSET FLAT: _gpTri	; ESI = gpTri
       mov     edx,_gPDC[TSPControlWord]; edx = TSPControlWord
	xor	eax,eax			; eax = 0
       and	edx,MASK_TRANS		; edx = MASK_TRANS bit or 0
	je	TCLLS_TSetup 		; if 0 TCLLS_TSetup with eax,edx = 0
;
; Check for Opaque trans. Done by reading the Context flag to check for
; opaque setting.
;
	mov	ebx,_gPDC[Ctx][u32Flags]
    and	ebx,MASK_OPAQUE_TRANS			; edx = MASK_OPAQUE_TRANS bit or 0
	je	TCLLS_TNewPass					; If !OpaqueTrans skip to new pass per tri
	mov	eax,PACKED_TYPE_OPAQUETRANS		; eax = PACKED_TYPE_OPAQUETRANS for u*Type
	mov	edx,PACKED_TYPE_OPAQUETRANS		; edx = PACKED_TYPE_OPAQUETRANS for u*Type
	jmp	TCLLS_TSetup 						; continue...
;
; Else check for new pass per triangle setup
;
TCLLS_TNewPass:
	mov	ebx,_gPDC[Ctx][u32Flags]
    and	ebx,MASK_NEWPASSPERTRI			; ebx = MASK_NEWPASSPERTRI bit or 0
	je	TCLLS_TTrans						; If !NewPassPerTri skip tp normal trans
;
; PACKED_TRANS_AND_SETMARK = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK
;
		mov	eax,PACKED_TRANS_AND_SETMARK; eax = 6 for u*Type
		xor	edx,edx						; edx = 0 for uOrBackType
	jmp	TCLLS_TSetup 						; continue...
;
; Else translucent setup (rare for performance case)
;
TCLLS_TTrans:
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
		je	TCLLS_TSetup 				   	; ebx =  MINZ_SORT, continue...
		xor	edx,edx						; edx = 0 for uOrBackType	
;
; Carry on with the main line
;
TCLLS_TSetup:
       mov	[LuDefaultType],eax	; uDefaultType = eax
	mov	[LuCurrentType],eax	; uCurrentType = eax
	mov	[LuOrBackType],edx	; uOrBackType = edx
	
TCLLS_Loop:
;
; Main loop: esi = pTri
;  	     eax, ebx, ecx, edx, ebp, edi = undefined
;
		mov	ebx,_gPDC[nPolys]	; ebx = nPolys
       	mov edx,_bStrip				;avoid stalling because of dependencies in bStrip & nFace & 1
       	dec	ebx			; ebx = nPolys - 1
		js	TCLLS_Done		; If -ive nPolys then TCLLS_Done
       	
       	mov	eax,_nFaces
       	mov	_gPDC[nPolys],ebx	; nPolys = nPolys - 1 = ebx
        mov	edi,_gPDC[pFace]
		mov ebp,65535;				;mask
		test edi,edi				;FIX: MOV does NOT affect the flags
		jz TCLLS_NonIndexed			; Indexed or NonIndexed ???
        and edx,1					;edx=bStrip & 1
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
;
TCLLS_Indexed:
		and edx,eax					;edx=nFaces & 1 & bStrip
									;TEST edx,eax should work as well
		jz TCLLS_Indexed_else

TCLLS_Indexed_if:		
		mov ebx,[edi+eax*2]		; load 2 vertices at a time	   
	   	
	   	inc eax					;next vertex: "false" increment
		mov edx,ebx				;copy 1st and second vertex
		and	ebx,ebp					;get first vertex by masking out upper 16bits
		shr	edx,16					;edx=2nd vertex
		
		inc eax					;next vertex
		mov di,[edi+eax*2]		;FIX:changed edi to di because it might cause page faults
								; 1 cc penalty + 1 byte	
		
		add	eax,[PFaceIndex] 
		and edi,ebp				;remove upper 16bits 
		jmp	TCLLS_FloatIt					;done
		
TCLLS_Indexed_else:
	   	mov edx,[edi+eax*2]		   ;edi= *(pFaces+nFaces)
		
		inc eax					;next vertex
		mov ebx,edx
		and edx,ebp				    ;fetched DWORD, remove upper 16bits
		shr ebx,16
				
		inc eax					;next vertex
		mov di,[edi+eax*2]			
	  	
	  	add	eax,[PFaceIndex] ;POS:common to all if-else blocks above
		and edi,ebp					;remove upper 16bits
	  	jmp TCLLS_FloatIt


TCLLS_NonIndexed:
		and edx,1					;avoid stalling because of dependencies in bStrip & nFace & 1
        mov	ecx,_gPDC[pVertices]	; ecx = pVertices
		
		and edx,eax					;;edx = nFaces & 1 & bStrip
		jz  TCLLS_NonIndexed_else
TCLLS_NonIndexed_if:
		mov	ebx,eax
		inc eax
		mov edx,eax
		inc eax
		mov edi,eax
		
		add	eax,[PFaceIndex] ;POS:common to all if-else blocks above
		jmp TCLLS_FloatIt
TCLLS_NonIndexed_else:
       mov	edx,eax
	   inc	eax
	   mov	ebx,eax
	   inc	eax
       mov	edi,eax
	   add	eax,[PFaceIndex] ;POS:common to all if-else blocks above

TCLLS_FloatIt:
    shl	edi,5			; edi *= sizeof(VERTEX)
	mov	_nFaces,eax	; pFace++ = eax
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
;
; Read culling mode here.
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
;
; Check for clockwise culling if det. greater than MAGIC_NUM_LARGE and positive.
;
	jge	TCLLS_CheckCW					; if fDet < MAGIC_NUM_LARGE	check for clockwise culling
;
       mov	edx,LESS_THAN_EPSILON		; edx = MAGIC_NUM
       cmp	eax,edx						; Flags = eax - MAGIC_NUM
	jl	TCLLS_NormalPath				; if fDet >= MAGIC_NUM then TCLLS_NormalPath
;
; Check to see if integer X the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fX0				; int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX0)
	fadd	DWORD PTR fX1				; int(fX1), int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fX0)
	fadd	DWORD PTR fX2				; int(fX2), int(fX1), int(fX0)
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
	jz	TCLLS_ShortCut2					; continue if result zero.
;
; Check to see if integer Y's the same. fY0, fY1, fY2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fY0				; int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fY0)
	fadd	DWORD PTR fY1				; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2				; int(fY2), int(fY1), int(fY0)
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
	jz	TCLLS_ShortCut2					; continue if result zero.
;
; Restore edx
	mov	edx,_gPDC[pV0]					; eax = edx = pV0
	jmp	TCLLS_NormDet					; go normal route
;
TCLLS_NormalPath:
;
; Determinate is very small or negative
;
	xor	eax,080000000h					; eax = - fDet
;
; Check for anticlockwise culling
	cmp	ecx, SGL_CULL_ANTICLOCK
	je	TCLLS_ShortCut2					; If culling then TCLLS_ShortCut2
;
; Invert the Determinate
;
;	Reload the values for the time being.
;
   mov	ebp,LESS_THAN_LARGE_EPSILON		; ebp = MAGIC_NUM_LARGE
   mov	edx,LESS_THAN_EPSILON			; edx = MAGIC_NUM
;
	cmp	eax,ebp
	 jge	TCLLS_NormalPath2			; if fDet < MAGIC_NUM_LARGE continue
;
	cmp	eax,edx
	 jl	TCLLS_ShortCut2					; if fDet < MAGIC_NUM TCLLS_ShortCut2
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
	jz	TCLLS_ShortCut2				; continue if result zero.
;
; Check to see if integer Y's the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]		; MAGIC
	fadd	DWORD PTR fY0			; int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fY0)
	fadd	DWORD PTR fY1			; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2			; int(fY2), int(fY1), int(fY0)
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
	jz	TCLLS_ShortCut2				; continue if result zero.
;
TCLLS_NormalPath2:
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
;
; To free up a register read from memory !!!!!
;	mov	eax,edx			; eax = edx = pV0
	mov	eax,_gPDC[pV0]			; eax = pV0
;
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
;
	jmp TCLLS_NormDet
;
; New checking for clockwise culling
;
TCLLS_CheckCW:
	cmp	ecx, SGL_CULL_CLOCKWISE
	je	TCLLS_ShortCut2		; If culling then TCLLS_ShortCut2
;
TCLLS_NormDet:
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
;
       mov	ecx,OFFSET FLAT: _ProcessD3DFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
	jne	TCLLS_CheckNonPer
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
	jmp	TCLLS_DoWTex
;
TCLLS_ShortCut2:							; Discard 2 fpu values
	fstp	st(0)
	fstp	st(0)
	jmp	TCLLS_Loop						; Next poly please

TCLLS_ByPtr:
       push	edx							; Save edx and pass pTri as
	push	esi							; only para.
	call	eax							; Call _ProcessFlatTexFn
       pop	esi							; Recover state; edx is the
	pop	edx								; only reg is use not saved
	jmp	TCLLS_DoneTex						; by the Intel compiler.
;
TCLLS_DoWTex:
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
	jmp		TCLLS_DoneTex			; Finished perspective texturing case.
;
; Start of non-perspective cases. Both SGLDirect & D3D
;
TCLLS_CheckNonPer:
       or	eax,eax
	jz	TCLLS_DoneTex
    cmp		eax,OFFSET FLAT: _ProcessFlatTexNonPer
	jne		TCLLS_ByPtr
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
TCLLS_DoWTexNonPer:
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
TCLLS_DoneTex:
	mov		eax,_gPDC[Ctx][u32Flags]	; eax = gPDC.Context.u32Flags
	mov		ecx,8192					; ecx = ZBUFFER_MASK
	test	eax,ecx						; Flags = u32Flags & ZBUFFER_MASK
	jne		TCLLS_NoZBuff					; If u32Flags & 8192 then TCLLS_NoZBuff
;
; For D3D we have been requested to use Z v's 1/w for ISP surface removal.
;
if USERHW
;
; Provide 1/w for the ISP
;
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
else
;
; Provide Z for the ISP
;
	fld		DWORD PTR _gfInvertZ		; 1
	fsub	DWORD PTR rpV0[V_fZ]		; 1-fZ0
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ0)
	fsub	DWORD PTR rpV2[V_fZ]		; 1-fZ2,MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ2),MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fsub	DWORD PTR rpV1[V_fZ]		; 1-fZ1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ1),MinZ*(1-fZ2),MinZ*(1-fZ0)
	fxch	st(2)						; MinZ*(1-fZ0),MinZ*(1-fZ2),MinZ*(1-fZ1)
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
;
; To prevent Z of > 1 (which incidenetly are illegal in D3D) from appearing behind
; the viewer clamp the calculated 1/w (1-z) value to 0. 
; The 3 calculated fZ's are checked.
;
	xor		eax, eax
	test	DWORD PTR [esi][fZ][0], 080000000h
	setnz	al
		xor		ecx, ecx
		test	DWORD PTR [esi][fZ][8], 080000000h
		setnz	cl
	sub		eax, 1
		sub		ecx, 1
	and		DWORD PTR [esi][fZ][0], eax
			and		DWORD PTR [esi][fZ][8], ecx
			xor		edx, edx
			test	DWORD PTR [esi][fZ][4], 080000000h
			setnz	dl
			sub		edx, 1
			and		DWORD PTR [esi][fZ][4], edx
;
endif
;
if Z_BUFFER_SIM
;
; Set the Bogus Z value to the greatest 1/w value. Helps apps which switch
; the Z buffer ON/OFF. It isn't a perfect Z buffer system but heh...
;
	mov		ecx, DWORD PTR [esi][fZ][0]
	mov		eax, DWORD PTR [esi][fZ][8]
	cmp		ecx, DWORD PTR _gfBogusInvZ
	mov		edx, DWORD PTR [esi][fZ][4]
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, ecx
@@:	cmp		eax, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, eax
@@:	cmp		edx, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, edx
@@:
endif
;
	jmp	TCLLS_Cont1						; goto TCLLS_Cont1

TCLLS_ShortCut1:							; Dicard 1 fpu value
	fstp	st(0)
TCLLS_ShortCut0:
	jmp	TCLLS_Loop						; Next poly please

TCLLS_NoZBuff:
	fld	QWORD PTR _2_1_float_13			; c1=BOGUSZ_INCREMENT
	fadd	DWORD PTR _gfBogusInvZ		; c1+gfBogusInvZ
	mov	eax,_gfBogusInvZ				; eax=gfBogusInvZ
	mov	[esi][fZ][0],eax				; pTri->fZ[0]=eax
	mov	[esi][fZ][4],eax				; pTri->fZ[1]=eax
	mov	[esi][fZ][8],eax				; pTri->fZ[2]=eax
	fstp	DWORD PTR _gfBogusInvZ		; gfBogusInvZ=
TCLLS_Cont1:
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
	jmp	TCLLS_XDone		; goto TCLLS_XDone (cannot be min too)
@@:	cmp	ecx,DWORD PTR fX2	; Flags = Min value - pV2->fX
	jle	TCLLS_XDone		; If Min value < pV2->fX carry on
	mov	ecx,DWORD PTR fX2	; Min value = pV2->fX
;
; Now eax = X1 (Max X) and ecx = X0 (Min X) as floating point, we check here
; if X1 (Max X) is negative as this indicates an off-screen poly that can
; be skipped. If X1 is +ive but X0 is -ive then we set X0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
TCLLS_XDone:
	or	eax,eax			; Flags = eax & eax (Max X)
	js	TCLLS_ShortCut0		; if ( Max X < 0 ) next poly please
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
	jmp	TCLLS_YDone		; goto TCLLS_YDone
@@:	cmp	ecx,edi			; Flags = Min value - pV2->fY
	jle	TCLLS_YDone		; If Min value < pV2->fY TCLLS_YDone
	mov	ecx,edi			; Store Min value = pV2->fY
TCLLS_YDone:
;
; Now eax = Y1 (Max Y) and ecx = Y0 (Min Y) as floating point, we check here
; if Y1 (Max Y) is negative as this indicates an off-screen poly that can
; be skipped. If Y1 is +ive but Y0 is -ive then we set Y0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
	or	eax,eax			; Flags = eax & eax (Max Y)
	js	TCLLS_ShortCut0		; if ( Max Y < 0 ) next poly please
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
;
; D3D we always do clipping.
;	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
;	or	eax,eax			; Flags = eax | eax
;	je	TCLLS_NoLimits		; if ( !bDoClipping ) TCLLS_NoLimits
;
;
; Apply [First/Last][X/Y]Region if required, likely case is for no clipping
; to occur.
;
	mov	eax,_gPDC[Ctx][FirstXRegion]	; eax = FirstXRegion
	mov	ebp,_gPDC[Ctx][LastXRegion]	; ebp = LastXRegion
	cmp	ebx,eax			; Flags = rX0 - FirstXReg
	jge	@F			; If rX0 >= FirstXReg skip clip
	cmp	edx,eax			; Flags = rX1 - FirstXReg
	jl	TCLLS_ShortCut1		; If rX1 < FirstXReg skip poly
	mov	ebx,eax			; rX0 = FirstXReg (very rare)
@@:	cmp	edx,ebp			; Flags = rX1 - LastXReg
	jle	@F			; If rX1 <= LastXReg skip clip
	cmp	ebx,ebp			; Flags = rX0 - LastXReg
	jg	TCLLS_ShortCut1		; If rX1 > LastXReg skip poly
	mov	edx,ebp			; rX1 = LastXReg (very rare)
@@:	mov	eax,_gPDC[Ctx][FirstYRegion]	; eax = FirstYRegion
	mov	ebp,_gPDC[Ctx][LastYRegion]	; ebp = LastYRegion
	cmp	edi,eax			; Flags = rY0 - FirstYReg
	jge	@F			; If rY0 >= FirstYReg skip clip
	cmp	ecx,eax			; Flags = rY1 - FirstYReg
	jl	TCLLS_ShortCut1		; If rY1 < FirstYReg skip poly
	mov	edi,eax			; rY0 = FirstYReg (very rare)
	and	edi,0fffffffeh		; ensure LSB of Y value stays zero
@@:	cmp	ecx,ebp			; Flags = rY1 - LastYReg
	jle	@F			; If rY1 <= LastYReg skip clip
	cmp	edi,ebp			; Flags = rY0 - LastYReg
	jg	TCLLS_ShortCut1		; If rY1 > LastYReg skip poly
	mov	ecx,ebp			; rY1 = LastYReg (very rare)
	and	ecx,0fffffffeh		; ensure LSB of Y value stays zero
@@:
;
; Region bounding box now established in ebx-edx,edi-ecx
;
TCLLS_NoLimits:
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
	jne	TCLLS_Loop		; If not at end of buffer TCLLS_Loop
TCLLS_Done:
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
__ProcessD3DCorePrimitive ENDP
;_ProcessD3DCorePrimitive	ENDS


;**********************************************************************************
	ALIGN	4			; 0
	PUBLIC	__ProcessD3DFanCorePrimitive
__ProcessD3DFanCorePrimitive	PROC NEAR

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
	PFaceIndex	TEXTEQU <esp+  28+Adjust> ; 160

       push	edi			; Save registers
	push	esi
       push	ebp
	push	ebx
       sub	esp,Adjust		; Preserve local space
	mov	esi,OFFSET FLAT: _gpTri	; ESI = gpTri
       mov     edx,_gPDC[TSPControlWord]; edx = TSPControlWord
	xor	eax,eax			; eax = 0
       and	edx,MASK_TRANS		; edx = MASK_TRANS bit or 0
	je	TCLF_TSetup 		; if 0 TCLF_TSetup with eax,edx = 0
;
; Check for Opaque trans. Done by reading the Context flag to check for
; opaque setting.
;
	mov	ebx,_gPDC[Ctx][u32Flags]
    and	ebx,MASK_OPAQUE_TRANS			; edx = MASK_OPAQUE_TRANS bit or 0
	je	TCLF_TNewPass					; If !OpaqueTrans skip to new pass per tri
	mov	eax,PACKED_TYPE_OPAQUETRANS		; eax = PACKED_TYPE_OPAQUETRANS for u*Type
	mov	edx,PACKED_TYPE_OPAQUETRANS		; edx = PACKED_TYPE_OPAQUETRANS for u*Type
	jmp	TCLF_TSetup 						; continue...
;
; Else check for new pass per triangle setup
;
TCLF_TNewPass:
	mov	ebx,_gPDC[Ctx][u32Flags]
    and	ebx,MASK_NEWPASSPERTRI			; ebx = MASK_NEWPASSPERTRI bit or 0
	je	TCLF_TTrans						; If !NewPassPerTri skip tp normal trans
;
; PACKED_TRANS_AND_SETMARK = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK
;
		mov	eax,PACKED_TRANS_AND_SETMARK; eax = 6 for u*Type
		xor	edx,edx						; edx = 0 for uOrBackType
	jmp	TCLF_TSetup 						; continue...
;
; Else translucent setup (rare for performance case)
;
TCLF_TTrans:
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
		je	TCLF_TSetup 				   	; ebx =  MINZ_SORT, continue...
		xor	edx,edx						; edx = 0 for uOrBackType	
;
; Carry on with the main line
;
TCLF_TSetup:
       mov	[LuDefaultType],eax	; uDefaultType = eax
	mov	[LuCurrentType],eax	; uCurrentType = eax
	mov	[LuOrBackType],edx	; uOrBackType = edx
	
TCLF_Loop:
;
; Main loop: esi = pTri
;  	     eax, ebx, ecx, edx, ebp, edi = undefined
;
		mov	ebx,_gPDC[nPolys]	; ebx = nPolys
       	mov	eax,_nFaces
       	dec	ebx			; ebx = nPolys - 1
		js	TCLF_Done		; If -ive nPolys then TCLF_Done
       	
       	mov	_gPDC[nPolys],ebx	; nPolys = nPolys - 1 = ebx
        mov	edi,_gPDC[pFace]
		mov ebp,65535;				;mask
		test edi,edi				
		jz TCLF_NonIndexed			; Indexed or NonIndexed ???
TCLF_Indexed:
								
	    mov ebx,[edi+eax*2]		;load 2nd and 3rd vertices			
		
		inc eax											
		
		mov edx,[edi]			;load first vertex 	   
	   	mov edi,ebx				;copy 3rd vertex
		and	edx,ebp				;mask out upper 16bits(get rid of junk)
								;first vertex ready
		and ebx,ebp				;mask out upper 16bits(get rid of 3rd vertex)
								;second vertex ready
		shr	edi,16				;third vertex ready	
		
		
		jmp	TCLF_FloatIt					;done
		

TCLF_NonIndexed:
		
		xor edx,edx			  ; 0	  
		mov ebx,eax			  ; 2nd vertex
		inc eax
		mov edi,eax			  ; 3rd vertex
		
;
;
; End of vertex determination.
;
TCLF_FloatIt:
    mov	ecx,_gPDC[pVertices]	; ecx = pVertices
    shl	edi,5			; edi *= sizeof(VERTEX)
	mov	_nFaces,eax			; pFace++ = eax
    shl	ebx,5			; ebx *= sizeof(VERTEX)
    add	edi,ecx			; EDI = pVertices + (edi*Size)
    shl	edx,5			; edx *= sizeof(VERTEX)
	add	ebx,ecx			; EBX = pVertices + (ebx*Size)
    add	edx,ecx			; EDX = pVertices + (edx*Size)
	mov eax,_gPDC[TSPControlWord]; eax = gPDC.TSPControlWord
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
;
; Read culling mode here.
       mov ecx,_gPDC[Ctx][bCullBackfacing]; ecx = bCullBackfacing
       cmp	eax,ebp						; Flags = eax - MAGIC_NUM
;
; Check for clockwise culling if det. greater than MAGIC_NUM_LARGE and positive.
;
	jge	TCLF_CheckCW					; if fDet < MAGIC_NUM_LARGE	check for clockwise culling
;
       mov	edx,LESS_THAN_EPSILON		; edx = MAGIC_NUM
       cmp	eax,edx						; Flags = eax - MAGIC_NUM
	jl	TCLF_NormalPath					; if fDet >= MAGIC_NUM then TCLF_NormalPath
;
; Check to see if integer X the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fX0				; int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX0)
	fadd	DWORD PTR fX1				; int(fX1), int(fX0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fX0)
	fadd	DWORD PTR fX2				; int(fX2), int(fX1), int(fX0)
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
	jz	TCLF_ShortCut2					; continue if result zero.
;
; Check to see if integer Y's the same. fY0, fY1, fY2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]			; MAGIC
	fadd	DWORD PTR fY0				; int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fY0)
	fadd	DWORD PTR fY1				; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]			; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2				; int(fY2), int(fY1), int(fY0)
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
	jz	TCLF_ShortCut2					; continue if result zero.
;
; Restore edx
	mov	edx,_gPDC[pV0]					; eax = edx = pV0
	jmp	TCLF_NormDet					; go normal route
;
TCLF_NormalPath:
;
; Determinate is very small or negative
;
	xor	eax,080000000h					; eax = - fDet
;
; Check for anticlockwise culling
	cmp	ecx, SGL_CULL_ANTICLOCK
	je	TCLF_ShortCut2					; If culling then TCLF_ShortCut2
;
; Invert the Determinate
;
;	Reload the values for the time being.
;
   mov	ebp,LESS_THAN_LARGE_EPSILON		; ebp = MAGIC_NUM_LARGE
   mov	edx,LESS_THAN_EPSILON			; edx = MAGIC_NUM
;
	cmp	eax,ebp
	 jge	TCLF_NormalPath2			; if fDet < MAGIC_NUM_LARGE continue
;
	cmp	eax,edx
	 jl	TCLF_ShortCut2					; if fDet < MAGIC_NUM TCLF_ShortCut2
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
	jz	TCLF_ShortCut2				; continue if result zero.
;
; Check to see if integer Y's the same. fX0, fX1, fX2.
; Use Simon's method which saves time over using the fispt instruction.
;
	mov		Temp[0], MAGIC_INT_CONV
	fld		DWORD PTR Temp[0]		; MAGIC
	fadd	DWORD PTR fY0			; int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fY0)
	fadd	DWORD PTR fY1			; int(fY1), int(fY0)
	fld		DWORD PTR Temp[0]		; MAGIC, int(fX1), int(fY0)
	fadd	DWORD PTR fY2			; int(fY2), int(fY1), int(fY0)
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
	jz	TCLF_ShortCut2				; continue if result zero.
;
TCLF_NormalPath2:
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
;
; To free up a register read from memory !!!!!
;	mov	eax,edx			; eax = edx = pV0
	mov	eax,_gPDC[pV0]			; eax = pV0
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
;
	jmp TCLF_NormDet
;
; New checking for clockwise culling
;
TCLF_CheckCW:
	cmp	ecx, SGL_CULL_CLOCKWISE
	je	TCLF_ShortCut2		; If culling then TCLF_ShortCut2
;
TCLF_NormDet:
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
;
       mov	ecx,OFFSET FLAT: _ProcessD3DFlatTex
	mov	eax,DWORD PTR _ProcessFlatTexFn
       cmp	eax,ecx
	jne	TCLF_CheckNonPer
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
	jmp	TCLF_DoWTex
;
TCLF_ShortCut2:							; Discard 2 fpu values
	fstp	st(0)
	fstp	st(0)
	jmp	TCLF_Loop						; Next poly please

TCLF_ByPtr:
       push	edx							; Save edx and pass pTri as
	push	esi							; only para.
	call	eax							; Call _ProcessFlatTexFn
       pop	esi							; Recover state; edx is the
	pop	edx								; only reg is use not saved
	jmp	TCLF_DoneTex						; by the Intel compiler.
;
TCLF_DoWTex:
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
	jmp		TCLF_DoneTex			; Finished perspective texturing case.
;
; Start of non-perspective cases. Both SGLDirect & D3D
;
TCLF_CheckNonPer:
       or	eax,eax
	jz	TCLF_DoneTex
    cmp		eax,OFFSET FLAT: _ProcessFlatTexNonPer
	jne		TCLF_ByPtr
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
TCLF_DoWTexNonPer:
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
TCLF_DoneTex:
	mov		eax,_gPDC[Ctx][u32Flags]	; eax = gPDC.Context.u32Flags
	mov		ecx,8192					; ecx = ZBUFFER_MASK
	test	eax,ecx						; Flags = u32Flags & ZBUFFER_MASK
	jne		TCLF_NoZBuff					; If u32Flags & 8192 then TCLF_NoZBuff
;
; For D3D we have been requested to use Z v's 1/w for ISP surface removal.
;
if USERHW
;
; Provide 1/w for the ISP
;
	fld		DWORD PTR _fMinInvZ			; MinZ
	fmul	DWORD PTR rpV0[V_fInvW]		; MinZ*pV0->fInvW
	fld		DWORD PTR rpV2[V_fInvW]		; pV2->fInvW,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fld		DWORD PTR rpV1[V_fInvW]		; pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fmul	DWORD PTR _fMinInvZ			; MinZ*pV1->fInvW,pV2->fInvW*MinZ,MinZ*pV0->fInvW
	fxch	st(2)						; MinZ*pV0->fInvW,pV2->fInvW*MinZ,MinZ*pV1->fInvW
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
else
;
; Provide Z for the ISP
;
	fld		DWORD PTR _gfInvertZ		; 1
	fsub	DWORD PTR rpV0[V_fZ]		; 1-fZ0
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ0)
	fsub	DWORD PTR rpV2[V_fZ]		; 1-fZ2,MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ2),MinZ*(1-fZ0)
	fld		DWORD PTR _gfInvertZ		; 1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fsub	DWORD PTR rpV1[V_fZ]		; 1-fZ1,MinZ*(1-fZ2),MinZ*(1-fZ0)
	fmul	DWORD PTR _fMinInvZ			; MinZ*(1-fZ1),MinZ*(1-fZ2),MinZ*(1-fZ0)
	fxch	st(2)						; MinZ*(1-fZ0),MinZ*(1-fZ2),MinZ*(1-fZ1)
;
	fstp	DWORD PTR [esi][fZ][0]		; pTri->fZ[0]=,pV2->fInvW*MinZ,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][8]		; pTri->fZ[2]=,MinZ*pV1->fInvW
	fstp	DWORD PTR [esi][fZ][4]		; pTri->fZ[1]=
;
; To prevent Z of > 1 (which incidenetly are illegal in D3D) from appearing behind
; the viewer clamp the calculated 1/w (1-z) value to 0. 
; The 3 calculated fZ's are checked.
;
	xor		eax, eax
	test	DWORD PTR [esi][fZ][0], 080000000h
	setnz	al
		xor		ecx, ecx
		test	DWORD PTR [esi][fZ][8], 080000000h
		setnz	cl
	sub		eax, 1
		sub		ecx, 1
	and		DWORD PTR [esi][fZ][0], eax
			and		DWORD PTR [esi][fZ][8], ecx
			xor		edx, edx
			test	DWORD PTR [esi][fZ][4], 080000000h
			setnz	dl
			sub		edx, 1
			and		DWORD PTR [esi][fZ][4], edx
;
endif
;
if Z_BUFFER_SIM
;
; Set the Bogus Z value to the greatest 1/w value. Helps apps which switch
; the Z buffer ON/OFF. It isn't a perfect Z buffer system but heh...
;
	mov		ecx, DWORD PTR [esi][fZ][0]
	mov		eax, DWORD PTR [esi][fZ][8]
	cmp		ecx, DWORD PTR _gfBogusInvZ
	mov		edx, DWORD PTR [esi][fZ][4]
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, ecx
@@:	cmp		eax, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, eax
@@:	cmp		edx, DWORD PTR _gfBogusInvZ
	jl		@F
	mov		DWORD PTR _gfBogusInvZ, edx
@@:
endif
;
	jmp	TCLF_Cont1						; goto TCLF_Cont1

TCLF_ShortCut1:							; Dicard 1 fpu value
	fstp	st(0)
TCLF_ShortCut0:
	jmp	TCLF_Loop						; Next poly please

TCLF_NoZBuff:
	fld	QWORD PTR _2_1_float_13			; c1=BOGUSZ_INCREMENT
	fadd	DWORD PTR _gfBogusInvZ		; c1+gfBogusInvZ
	mov	eax,_gfBogusInvZ				; eax=gfBogusInvZ
	mov	[esi][fZ][0],eax				; pTri->fZ[0]=eax
	mov	[esi][fZ][4],eax				; pTri->fZ[1]=eax
	mov	[esi][fZ][8],eax				; pTri->fZ[2]=eax
	fstp	DWORD PTR _gfBogusInvZ		; gfBogusInvZ=
TCLF_Cont1:
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
	jmp	TCLF_XDone		; goto TCLF_XDone (cannot be min too)
@@:	cmp	ecx,DWORD PTR fX2	; Flags = Min value - pV2->fX
	jle	TCLF_XDone		; If Min value < pV2->fX carry on
	mov	ecx,DWORD PTR fX2	; Min value = pV2->fX
;
; Now eax = X1 (Max X) and ecx = X0 (Min X) as floating point, we check here
; if X1 (Max X) is negative as this indicates an off-screen poly that can
; be skipped. If X1 is +ive but X0 is -ive then we set X0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
TCLF_XDone:
	or	eax,eax			; Flags = eax & eax (Max X)
	js	TCLF_ShortCut0		; if ( Max X < 0 ) next poly please
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
	jmp	TCLF_YDone		; goto TCLF_YDone
@@:	cmp	ecx,edi			; Flags = Min value - pV2->fY
	jle	TCLF_YDone		; If Min value < pV2->fY TCLF_YDone
	mov	ecx,edi			; Store Min value = pV2->fY
TCLF_YDone:
;
; Now eax = Y1 (Max Y) and ecx = Y0 (Min Y) as floating point, we check here
; if Y1 (Max Y) is negative as this indicates an off-screen poly that can
; be skipped. If Y1 is +ive but Y0 is -ive then we set Y0 to 0.
; This saves us coping with negative values during the quicky float to
; integer conversions later.
;
	or	eax,eax			; Flags = eax & eax (Max Y)
	js	TCLF_ShortCut0		; if ( Max Y < 0 ) next poly please
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
;
; D3D we always do clipping.
;	mov	eax,_gPDC[Ctx][bDoClipping]; eax = bDoClipping
;	or	eax,eax			; Flags = eax | eax
;	je	TCLF_NoLimits		; if ( !bDoClipping ) TCLF_NoLimits
;
;
; Apply [First/Last][X/Y]Region if required, likely case is for no clipping
; to occur.
;
	mov	eax,_gPDC[Ctx][FirstXRegion]	; eax = FirstXRegion
	mov	ebp,_gPDC[Ctx][LastXRegion]	; ebp = LastXRegion
	cmp	ebx,eax			; Flags = rX0 - FirstXReg
	jge	@F			; If rX0 >= FirstXReg skip clip
	cmp	edx,eax			; Flags = rX1 - FirstXReg
	jl	TCLF_ShortCut1		; If rX1 < FirstXReg skip poly
	mov	ebx,eax			; rX0 = FirstXReg (very rare)
@@:	cmp	edx,ebp			; Flags = rX1 - LastXReg
	jle	@F			; If rX1 <= LastXReg skip clip
	cmp	ebx,ebp			; Flags = rX0 - LastXReg
	jg	TCLF_ShortCut1		; If rX1 > LastXReg skip poly
	mov	edx,ebp			; rX1 = LastXReg (very rare)
@@:	mov	eax,_gPDC[Ctx][FirstYRegion]	; eax = FirstYRegion
	mov	ebp,_gPDC[Ctx][LastYRegion]	; ebp = LastYRegion
	cmp	edi,eax			; Flags = rY0 - FirstYReg
	jge	@F			; If rY0 >= FirstYReg skip clip
	cmp	ecx,eax			; Flags = rY1 - FirstYReg
	jl	TCLF_ShortCut1		; If rY1 < FirstYReg skip poly
	mov	edi,eax			; rY0 = FirstYReg (very rare)
	and	edi,0fffffffeh		; ensure LSB of Y value stays zero
@@:	cmp	ecx,ebp			; Flags = rY1 - LastYReg
	jle	@F			; If rY1 <= LastYReg skip clip
	cmp	edi,ebp			; Flags = rY0 - LastYReg
	jg	TCLF_ShortCut1		; If rY1 > LastYReg skip poly
	mov	ecx,ebp			; rY1 = LastYReg (very rare)
	and	ecx,0fffffffeh		; ensure LSB of Y value stays zero
@@:
;
; Region bounding box now established in ebx-edx,edi-ecx
;
TCLF_NoLimits:
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
	jne	TCLF_Loop		; If not at end of buffer TCLF_Loop
TCLF_Done:
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
__ProcessD3DFanCorePrimitive ENDP
;__ProcessD3DFanCorePrimitive	ENDS



_TEXT	ENDS
_DATA1	SEGMENT PARA PUBLIC USE32 'DATA'
_2_1_float_13	DD 000000000H,03e000000H	; xf64
_2_1_float_18	DD 03f800000H	; xf32
_2_1_float_18_d	DD 000000000H,03ff00000H	; xf32
_float_to_int22	DD 04B400000H			; MAGIC_FLOAT TO INT22 VALUE
_DATA1	ENDS
_DATA	SEGMENT PARA PUBLIC USE32 'DATA'
_DATA	ENDS
; -- End	_ProcessD3DCorePrimitive

EXTRN	__fltused:BYTE

END
