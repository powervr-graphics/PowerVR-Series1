;*****************************************************************************
;*++
;*  Name        :   $RCSfile: fast.asm,v $
;*  Title       :   
;*  ASM Author  :   Jim Page
;*  Created     :   20/04/94
;*
;* Copyright	: 1995-2022 Imagination Technologies (c)
;* License	: MIT
;*
;*  Description :   Pentium optimised shading routines
;*
;*  Program Type:   ASM module (.dll)
;* 
;*  RCS info:
;*
;*  $Date: 1997/02/05 13:48:59 $
;*  $Revision: 1.3 $
;*  $Locker:  $
;*  $Log: fast.asm,v $
;; Revision 1.3  1997/02/05  13:48:59  ncj
;; Added CPUID instruction
;;
;; Revision 1.2  1996/08/02  18:03:09  jop
;; Added FPU restore
;;
;; Revision 1.1  1996/06/10  11:51:12  jop
;; Initial revision
;;
;; Revision 1.1  1996/06/10  11:39:13  jop
;; Initial revision
;;
;; Revision 1.2  1996/02/07  15:17:09  ncj
;; Added int3() utility
;;
;; Revision 1.1  1995/10/20  10:32:51  jop
;; Initial revision
;;
;*
;*--
;*****************************************************************************


        .386

_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'

	wFPUFlags			dw ?
	wFPUSaveFlags		dw ?
	
_DATA	ENDS

_TEXT   SEGMENT DWORD USE32 PUBLIC 'CODE'

        ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING
        
PUBLIC  _SetupFPU
PUBLIC  _RestoreFPU
PUBLIC  _GetCS
PUBLIC  _GetDS
PUBLIC  _int3
PUBLIC  _cpuid

_GetDS PROC

		mov		ax, ds
        ret

_GetDS ENDP

_GetCS PROC

		mov		ax, cs
        ret

_GetCS ENDP

_SetupFPU PROC

        fstcw   [wFPUSaveFlags]
		mov		ax, [wFPUSaveFlags]
        and     ax, 0FCFFh
        or      ax, 00C00h
		mov		[wFPUFlags], ax
        fldcw   [wFPUFlags]
        ret

_SetupFPU ENDP

_RestoreFPU PROC

        fldcw   [wFPUSaveFlags]
        ret

_RestoreFPU ENDP


_Int3 PROC
		int		3
		ret
_Int3 ENDP


_cpuid PROC
		db		0Fh		; CPUID instruction
		db		0A2h
		ret
_cpuid ENDP


_TEXT   ENDS

END

; end of file $RCSfile: fast.asm,v $

