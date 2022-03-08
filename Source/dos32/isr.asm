;;****************************************************************************
;;++
;;  Name        :   $RCSfile: isr.asm,v $
;;  Title       :   
;;  ASM Author  :   Jim Page
;;  Created     :   19/11/96
;;
;;  Copyright	: 1995-2022 Imagination Technologies (c)
;;  License		: MIT
;;
;;  Description :   Bi-modal ISRs for DOS PVR driver (DOS4GW)
;;
;;  Program Type:   ASM module 
;; 
;;  RCS info:
;;
;;  $Date: 1997/04/15 17:30:26 $
;;  $Revision: 1.4 $
;;  $Locker:  $
;;  $Log: isr.asm,v $
;; Revision 1.4  1997/04/15  17:30:26  jop
;; sorted out RM ISR to acknowlege interrupt
;;
;; Revision 1.3  1997/04/15  10:42:52  jop
;; Added call to DPMI fn 303 to handle RM ins better
;;
;; Revision 1.2  1997/04/09  12:56:51  jop
;; Fixed ds:/es: bug in protoected mode ISR
;;
;; Revision 1.1  1997/04/09  02:53:30  jop
;; Initial revision
;;
;;
;;--
;;****************************************************************************

PCX_INTSTATUS   equ  	003h*4
PCX_INTMASK     equ		004h*4
PVR_STATUS_EOR	equ		2
PVR_STATUS_RETRIG	equ		080000000h

SetupPIC MACRO Val
	
	LOCAL SP_001

	cmp		al, 7
	jle		SP_001
	mov		dx, 0A0h
	mov		al, 20h
	out		dx, al
SP_001:
	mov		dx, 20h
	mov		al, 20h
	out		dx, al

ENDM

;; real mode info structure passed in to us

RMDATA	STRUCT
		RMStatusFlags	DWORD	?
		RMBusDevFnc		DWORD	?
		RMIRQ			DWORD	?
RMDATA	ENDS

PMDATA	STRUCT
		PMStatusFlags	DWORD	?
		PMRegs			DWORD	?
		PMIRQ			DWORD	?
PMDATA	ENDS

;**
;** Assembler code for real-mode and protected-mode
;** INT 0xC interrupt handlers to support the INT 0xC
;** interrupt in both modes
;**
	.386
;**
;** The real-mode interrupt handler is in a 16-bit code
;** segment so that the assembler will generate the right
;** code.  We will copy this code down to a 16-bit segment
;** in low memory rather than executing it in place.
;**

PUBLIC  ISR_pcx_realmode_
PUBLIC  ISR_pcx_protmode_

PUBLIC	_PMData
PUBLIC	_RMData

READ_CONFIGURATION_DWORD_16		equ 0B10Ah
WRITE_CONFIGURATION_DWORD_16	equ 0B10Dh

SavePCIRegs MACRO

	mov		dx, 0CF8h
	in		eax, dx
	push	eax

ENDM

RestorePCIRegs MACRO

	pop		eax
	mov		dx, 0CF8h
	out		dx, eax

ENDM

;
; uses eax, dx
;
WritePCIReg32 MACRO BusDevFnc, Reg, Data

	if 0
	mov		ax, WRITE_CONFIGURATION_DWORD_16
	mov		bx, WORD PTR BusDevFnc	; bDevFncID
	mov		di, Reg		; wReg
	mov		ecx, Data	; Data
	int		1Ah

	; carry set -> error
	; 	"	clr -> ok
	else

		mov		eax, DWORD PTR BusDevFnc
		shl		eax, 8
		mov		al, Reg
		or		eax, 080000000h
		mov		dx, 0CF8h
		out		dx, eax
		mov		eax, Data
		mov		dx, 0CFCh
		out		dx, eax

	endif

ENDM

;
; uses eax, dx
;
ReadPCIReg32 MACRO BusDevFnc, Reg

	if 0
	mov		ax, READ_CONFIGURATION_DWORD_16
	mov		bx, WORD PTR BusDevFnc	; bDevFncID
	mov		di, Reg		; wReg
	int		1Ah

	; carry set -> error
	; 	"	clr -> ok
	else

		mov		eax, DWORD PTR BusDevFnc
		shl		eax, 8
		mov		al, Reg
		or		eax, 080000000h
		mov		dx, 0CF8h
		out		dx, eax
		mov		dx, 0CFCh
		in		eax, dx

	endif
	; ecx 		-> data

ENDM

_TEXT16 SEGMENT BYTE PUBLIC USE16 'CODE'

	ASSUME  cs:_TEXT16

ISR_pcx_realmode_:

;	cli	; already interrupts off?

if 1
	
	; stick an @ sign at the top left of a mono screen

	push	es
	push	bx
	
	mov		bx, 0B000h
	mov		es, bx
	mov		bx, 0
	mov		BYTE PTR es:[bx], 64
	
	pop		bx
	pop		es

endif

	push	es
	push	si
	push	di
	push    ecx
	push	eax

	SavePCIRegs

	; get data offset from start of ISR. cs will point
	; to start of ISR, so this offset is the offset
	; to the start of the local data

	mov		si, _RMData - ISR_pcx_realmode_

	; WORKAROUND TIME! move the PCX register base from FF000000 (ish)
	; to b8000 (VGA reg space) to acknowlege the interrupt, then
	; move it back again and hope noone notices

	ReadPCIReg32	cs:[si].RMBusDevFnc, 010h
	push			eax
	WritePCIReg32	cs:[si].RMBusDevFnc, 010h, 000b8000h

	; get seg value of base of our register file

	mov				ax, 0B800h
	mov				es, ax
	xor				di, di
	mov				eax, DWORD PTR es:[di].PCX_INTSTATUS
	and				eax, DWORD PTR es:[di].PCX_INTMASK
	jz				not_us_rm

	push    		dx
	mov				eax, DWORD PTR cs:[si].RMIRQ
	SetupPIC
	pop     		dx

	or				DWORD PTR cs:[si].RMStatusFlags, PVR_STATUS_EOR

not_us_rm:

	pop				ecx
	WritePCIReg32	cs:[si].RMBusDevFnc, 010h, ecx

	RestorePCIRegs

	pop		eax
	pop     ecx
	pop     di
	pop     si
	pop		es

	sti

	iret

_RMData		RMDATA <>

_TEXT16 ENDS

;**
;** The protected-mode interrupt handler is in a 32-bit code
;** segment.  Even so, we have to be sure to force an IRETD
;** at the end of the handler, because MASM doesn't generate
;** one.  This handler will be called on a 32-bit stack by
;** DOS/4GW.
;**
;** _DATA is the flat model data segment, which we load into
;** ES so we can write to absolute address 0xB8000.  (In the
;** flat model, DS is based at 0.)
;**

_DATA   SEGMENT BYTE PUBLIC USE32 'DATA'

	_PMData	PMDATA	<>

_DATA   ENDS

	DGROUP GROUP _DATA

; Protected mode interrupt handler (the straightforward one!)

_TEXT   SEGMENT BYTE PUBLIC USE32 'CODE'
	
	ASSUME  cs:_TEXT

ISR_pcx_protmode_:

	cli

	push    es
	push	eax
	push    ebx

	mov     bx,DGROUP
	mov     es, bx

	mov		eax, es:[_PMData].PMRegs
	mov		ebx, es:[eax+PCX_INTSTATUS]
	and		ebx, es:[eax+PCX_INTMASK]
	jz		not_us_pm

	mov		ebx, es:[_PMData].PMStatusFlags
	or		es:[ebx], PVR_STATUS_EOR

	push    dx
	mov		eax, es:[_PMData].PMIRQ
	SetupPIC
	pop     dx

not_us_pm:

	pop     ebx
	pop     eax
	pop     es

	sti

	iretd

_TEXT   ENDS

	END

; end of $RCSfile: isr.asm,v $
