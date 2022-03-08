;*****************************************************************************
;*++
;*  Name        :   $RCSfile: pci.asm,v $
;*  Title       :   PCI.ASM
;*  ASM Author  :   Jim Page
;*  Created     :   20/04/94
;*
;* Copyright	: 1995-2022 Imagination Technologies (c)
;* License	: MIT
;*
;*  Description :   Basic PCI i/o layer
;*
;*  Program Type:   ASM module (.dll)
;* 
;*  RCS info:
;*
;*  $Date: 1996/11/04 08:33:30 $
;*  $Revision: 1.3 $
;*  $Locker:  $
;*  $Log: pci.asm,v $
;; Revision 1.3  1996/11/04  08:33:30  mal
;; Modified PCIFindBoard as new compiler was using ebx
;; and PCIFindBoard calls int 1A which does not preserve
;; ebx. Hence it now push's and pop's ebx to prevent a page
;; fault occurring.
;;
;; Revision 1.2  1996/07/19  19:42:00  jop
;; Added pc98 support
;;
;; Revision 1.1  1996/06/10  11:51:12  jop
;; Initial revision
;;
;; Revision 1.1  1996/06/10  11:39:13  jop
;; Initial revision
;;
;; Revision 1.5  1996/01/05  16:51:57  jop
;; Routes PCI config 32 bit read/write through BIOS
;;
;; Revision 1.4  1995/11/07  18:28:37  jop
;; Bridge-proofed the code (in theory)
;;
;; Revision 1.3  1995/11/02  13:39:36  jop
;; now pushes correct regs onto stack
;;
;; Revision 1.2  1995/10/20  15:53:48  jop
;; Added word and dword io functions
;;
;; Revision 1.1  1995/10/19  18:37:50  jop
;; Initial revision
;;
;*
;*--
;*****************************************************************************


        .386

_DATA   SEGMENT DWORD USE32 PUBLIC 'DATA'

	_wIsPC98	dw 0

PUBLIC  _wIsPC98

_DATA	ENDS

; 16 bit
 
INSTALLATION_CHECK_16	  		equ 0B101h
FIND_PCI_DEVICE_16				equ 0B102h
FIND_PCI_CLASS_CODE_16			equ 0B103h
PCI_BUS_SPECIFIC_OPERATIONS_16  equ 0B106h
READ_CONFIGURATION_BYTE_16		equ 0B108h
READ_CONFIGURATION_WORD_16		equ 0B109h
READ_CONFIGURATION_DWORD_16		equ 0B10Ah
WRITE_CONFIGURATION_BYTE_16		equ 0B10Bh
WRITE_CONFIGURATION_WORD_16		equ 0B10Ch
WRITE_CONFIGURATION_DWORD_16	equ 0B10Dh
GET_IRQ_ROUTING_OPTIONS_16		equ 0B10Eh

; 32 bit 

INSTALLATION_CHECK_32	  		equ 0B181h
FIND_PCI_DEVICE_32				equ 0B182h
FIND_PCI_CLASS_CODE_32			equ 0B183h
PCI_BUS_SPECIFIC_OPERATIONS_32  equ 0B186h
READ_CONFIGURATION_BYTE_32		equ 0B188h
READ_CONFIGURATION_WORD_32		equ 0B189h
READ_CONFIGURATION_DWORD_32		equ 0B18Ah
WRITE_CONFIGURATION_BYTE_32		equ 0B18Bh
WRITE_CONFIGURATION_WORD_32		equ 0B18Ch
WRITE_CONFIGURATION_DWORD_32	equ 0B18Dh 
GET_IRQ_ROUTING_OPTIONS_32		equ 0B18Eh


_TEXT   SEGMENT DWORD USE32 PUBLIC 'CODE'

        ASSUME  CS:FLAT, DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING
        
PUBLIC  _PCIFindBoard
PUBLIC  _PCIRead8
PUBLIC  _PCIRead16
PUBLIC  _PCIRead32
PUBLIC  _PCIRead32Hack
PUBLIC  _PCIWrite8
PUBLIC  _PCIWrite16
PUBLIC  _PCIWrite32
PUBLIC  _PCIWrite32Hack
PUBLIC	_OutByte
PUBLIC	_OutWord
PUBLIC	_OutDword
PUBLIC	_InByte
PUBLIC	_InWord
PUBLIC	_InDword

; The following registers are always preserved!!
;
; esp
; ebp
; ebx
; esi
; edi

_OutByte PROC

	; stack:
	;
	; esp+0:  return ptr
	; esp+4:  port (WORD)
	; esp+8:  value (BYTE)

	mov		dx, WORD PTR [esp+4]
	mov		al, BYTE PTR [esp+8]
	out		dx, al
	ret
		
_OutByte ENDP

_OutWord PROC

	; stack:
	;
	; esp+0:  return ptr
	; esp+4:  port (WORD)
	; esp+8:  value (WORD)

	mov		dx, WORD PTR [esp+4]
	mov		ax, WORD PTR [esp+8]
	out		dx, ax
	ret
		
_OutWord ENDP

_OutDword PROC

	; stack:
	;
	; esp+0:  return ptr
	; esp+4:  port (WORD)
	; esp+8:  value (DWORD)

	mov		dx, WORD PTR [esp+4]
	mov		eax, DWORD PTR [esp+8]
	out		dx, eax
	ret
		
_OutDword ENDP

_InByte PROC

	; stack:
	;
	; esp+0:  return ptr
	; esp+4:  port (WORD)

	mov		dx, WORD PTR [esp+4]
	in		al, dx
	ret
		
_InByte ENDP

_InWord PROC

	; stack:
	;
	; esp+0:  return ptr
	; esp+4:  port (WORD)

	mov		dx, WORD PTR [esp+4]
	in		ax, dx
	ret
		
_InWord ENDP

_InDword PROC

	; stack:
	;
	; esp+0:  return ptr
	; esp+4:  port (WORD)

	mov		dx, WORD PTR [esp+4]
	in		eax, dx
	ret
		
_InDword ENDP

_PCIFindBoard PROC

	; stack:
	;
	; ebp+4:  return ptr
	; ebp+8:  bBusIDNum (BYTE *)
	; ebp+12: bDevFncID (BYTE *)
	; ebp+16: vendor (WORD)
	; ebp+20: device (WORD)

	; Setup A Stack Frame
	push ebp
	mov	 ebp, esp 	

	mov		ah, 086h
	
	mov		ax, FIND_PCI_DEVICE_16
	mov		cx, WORD PTR [ebp+20]		; device
	mov		dx, WORD PTR [ebp+16]		; vendor

	; ICL NOW passes args using ebx
	; Int 1A (On Some Compaq Machines ) uses EBX and does not preserve it.
	push	ebx							; ebx must be preserved
	push	esi							; esi must be preserved
	mov		si, 0

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		FB_Is98

	; no - normal machine

	int		1Ah
	pop		esi
	jc		FindBoardError
	jmp		FindBoardOk

FB_Is98:

	mov		ah, 0CCh
	int		1Fh
	pop		esi
	jc		FindBoardError

FindBoardOk:

	mov		eax, [ebp+8]				; bBusIDNum
	mov		BYTE PTR [eax], bh
	mov		eax, [ebp+12]				; bDevFncID
	mov		BYTE PTR [eax], bl
	xor		eax, eax
	pop		ebx							; ebx must be preserved
	pop     ebp
	ret	

FindBoardError:

	movzx	eax, ah					; error in ah
	push	eax

	; try the oblique approach
	
	mov		edx, 0

FindLoop:

	mov		eax, 80000000h
	mov		ecx, edx				; get device ID
	and		ecx, 00FF0000h			
	shr		ecx, 5
	or		eax, ecx
	mov		dx, 0CF8h
	out		dx, eax
	mov		dx, 0CFCh
	xor		eax, eax
	out		dx, eax						; read only reg - get around bridge bug!
	in		eax, dx

	mov		cx, WORD PTR [ebp+20]		; device
	shl		ecx, 16
	mov		cx, WORD PTR [ebp+16]		; vendor
	cmp		eax, ecx
	jne		@F

	; success!

	pop		eax
	pop		ebx							; ebx must be preserved

	mov		eax, [ebp+8]				; bBusIDNum
	mov		BYTE PTR [eax], 0
	mov		eax, [ebp+12]				; bDevFncID
	and		edx, 00FF0000h
	shr		edx, 13
	mov		BYTE PTR [eax], dl
	xor		eax, eax
	pop     ebp
	ret

@@:	shr		edx, 16
	inc		edx
	cmp		edx, 32
	shl		edx, 16
	jnz		FindLoop
	
	; return error from BIOS call

	pop		eax
	pop		ebx							; ebx must be preserved
	pop     ebp
	ret
		
_PCIFindBoard ENDP

_PCIRead8 PROC

	; stack: + 8 pushed!!
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (BYTE *)

	push	ebx
	push	edi
	mov		ax, READ_CONFIGURATION_BYTE_16
	mov		bh, BYTE PTR [esp+4+8]	; bBusIDNum
	mov		bl, BYTE PTR [esp+8+8]	; bDevFncID
	mov		di, WORD PTR [esp+12+8]	; wReg

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		R8_Is98

	; no - normal machine

	int		1Ah
	pop		edi
	pop		ebx
	jc		Read8Error
	jmp		Read8Ok
	
R8_Is98:
	
	mov		ah, 0CCh
	int		1Fh
	pop		edi
	pop		ebx
	jc		Read8Error

Read8Ok:

	mov		eax, [esp+16]			; Data
	mov		BYTE PTR [eax], cl
	xor		eax, eax
	ret

Read8Error:

	movzx	eax, ah					; error in ah
	ret

_PCIRead8 ENDP

_PCIRead16 PROC

	; stack: + 8 pushed!!
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (WORD *)

	push	ebx
	push	edi
	mov		ax, READ_CONFIGURATION_WORD_16
	mov		bh, BYTE PTR [esp+4+8]	; bBusIDNum
	mov		bl, BYTE PTR [esp+8+8]	; bDevFncID
	mov		di, WORD PTR [esp+12+8]	; wReg

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		R16_Is98

	; no - normal machine

	int		1Ah
	pop		edi
	pop		ebx
	jc		Read16Error
	jmp		Read16Ok

R16_Is98:

	mov		ah, 0cch
	int		1fh
	pop		edi
	pop		ebx
	jc		Read16Error

Read16Ok:

	mov		eax, [esp+16]			; Data
	mov		WORD PTR [eax], cx
	xor		eax, eax
	ret

Read16Error:

	movzx	eax, ah					; error in ah
	ret

_PCIRead16 ENDP

_PCIRead32 PROC

	; stack: + 8 pushed
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (DWORD *)

	push	ebx
	push	edi
	mov		ax, READ_CONFIGURATION_DWORD_16
	mov		bh, BYTE PTR [esp+4+8]	; bBusIDNum
	mov		bl, BYTE PTR [esp+8+8]	; bDevFncID
	mov		di, WORD PTR [esp+12+8]	; wReg

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		R32_Is98

	; no - normal machine

	int		1Ah
	pop		edi
	pop		ebx
	jc		Read32Error
	jmp		Read32Ok

R32_Is98:

	mov		ah, 0cch
	int		1fh
	pop		edi
	pop		ebx
	jc		Read32Error

Read32Ok:

	mov		eax, [esp+16]			; Data
	mov		DWORD PTR [eax+0], ecx
	xor		eax, eax
	ret

Read32Error:

	movzx	eax, ah					; error in ah
	ret

_PCIRead32 ENDP

_PCIRead32Hack PROC

	; stack: + 8 pushed
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (DWORD *)

	mov		eax, 80000000h
	movzx	ecx, BYTE PTR [esp+4]	; bBusIDNum
	shl		ecx, 16
	or		eax, ecx
	movzx	ecx, BYTE PTR [esp+8]	; bDevFncID
	shl		ecx, 8
	or		eax, ecx
	movzx	ecx, BYTE PTR [esp+12]	; bDevFncID
	and		ecx, 0FCh
	or		eax, ecx
	mov		dx, 0CF8h
	out		dx, eax
	mov		dx, 0CFCh
	in		eax, dx
	mov		edx, [esp+16]
	mov		[edx], eax
	xor		eax, eax
	ret

_PCIRead32Hack ENDP

_PCIWrite8 PROC

	; stack + 8 pushed:
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (BYTE)

	push	ebx
	push	edi
	mov		ax, WRITE_CONFIGURATION_BYTE_16
	mov		bh, BYTE PTR [esp+12]	; bBusIDNum
	mov		bl, BYTE PTR [esp+16]	; bDevFncID
	mov		di, WORD PTR [esp+20]	; wReg
	mov		cl, BYTE PTR [esp+24]	; Data

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		W8_Is98

	; no - normal machine

	int		1Ah
	pop		edi
	pop		ebx
	jc		Write8Error
	jmp		Write8Ok

W8_Is98:

	mov		ah, 0cch
	int		1fh
	pop		edi
	pop		ebx
	jc		Write8Error

Write8Ok:

	xor		eax, eax
	ret

Write8Error:

	movzx	eax, ah					; error in ah
	ret

_PCIWrite8 ENDP

_PCIWrite16 PROC

	; stack + 8 pushed:
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+14: Data (WORD)

	push	ebx
	push	edi
	mov		ax, WRITE_CONFIGURATION_WORD_16
	mov		bh, BYTE PTR [esp+12]	; bBusIDNum
	mov		bl, BYTE PTR [esp+16]	; bDevFncID
	mov		di, WORD PTR [esp+20]	; wReg
	mov		cx, WORD PTR [esp+24]	; Data

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		W16_Is98

	; no - normal machine

	int		1Ah
	pop		edi
	pop		ebx
	jc		Write16Error
	jmp		Write16Ok

W16_Is98:

	mov		ah, 0cch
	int		1fh
	pop		edi
	pop		ebx
	jc		Write16Error

Write16Ok:

	xor		eax, eax
	ret

Write16Error:

	movzx	eax, ah					; error in ah
	ret

_PCIWrite16 ENDP

_PCIWrite32 PROC

	; stack + 8 pushed:
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (DWORD)

	push	ebx
	push	edi
	mov		ax, WRITE_CONFIGURATION_DWORD_16
	mov		bh, BYTE PTR [esp+12]	; bBusIDNum
	mov		bl, BYTE PTR [esp+16]	; bDevFncID
	mov		di, WORD PTR [esp+20]	; wReg
	mov		ecx, DWORD PTR [esp+24]	; Data

	; PC98 machine?

	test	WORD PTR [_wIsPC98], 0FFFFh
	jnz		W32_Is98

	; no - normal machine

	int		1Ah
	pop		edi
	pop		ebx
	jc		Write32Error
	jmp		Write32Ok

W32_Is98:

	mov		ah, 0cch
	int		1fh
	pop		edi
	pop		ebx
	jc		Write32Error

Write32Ok:

	xor		eax, eax				; eax is zero
	ret

Write32Error:

	movzx	eax, ah					; error in eax
	ret

_PCIWrite32 ENDP

_PCIWrite32Hack PROC

	; stack + 8 pushed:
	;
	; esp+0:  return ptr
	; esp+4:  bBusIDNum (BYTE)
	; esp+8:  bDevFncID (BYTE)
	; esp+12: wReg (WORD)
	; esp+16: Data (DWORD)

	mov		eax, 80000000h
	movzx	ecx, BYTE PTR [esp+4]	; bBusIDNum
	shl		ecx, 16
	or		eax, ecx
	movzx	ecx, BYTE PTR [esp+8]	; bDevFncID
	shl		ecx, 8
	or		eax, ecx
	movzx	ecx, BYTE PTR [esp+12]	; bDevFncID
	and		ecx, 0FCh
	or		eax, ecx
	mov		dx, 0CF8h
	out		dx, eax
	mov		dx, 0CFCh
	mov		eax, DWORD PTR [esp+16]	; data
	out		dx, eax
	xor		eax, eax
	ret

_PCIWrite32Hack ENDP

_TEXT   ENDS
		END

; end of $RCSfile: pci.asm,v $ 

