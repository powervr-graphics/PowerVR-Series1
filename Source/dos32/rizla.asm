; nasty stdlib hacks

.386P
.387

; this seems to be needed by the intel compiler
; but nothing seems to access it ... better get your
; tin hats on lads.

PUBLIC __fltused

_DATA   SEGMENT PARA PUBLIC USE32 'DATA'

        __fltused       db 0

_DATA   ENDS

; chkstk function (used by txmops.c)

PUBLIC __chkstk
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
__chkstk:

        ret

_TEXT   ENDS

; Macros for producing the wrappers

; 1 to 4 arguments

CDeclToReg1     MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        mov             eax, 4[esp]
        call    fn&_
        ret             

_TEXT   ENDS
ENDM

CDeclToReg2     MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        push    edx
        mov             eax, 4[esp+4]
        mov             edx, 8[esp+4]
        call    fn&_
        pop             edx
        ret             

_TEXT   ENDS
ENDM

CDeclToReg3     MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        sub             esp, 8
        mov             0[esp], edx
        mov             4[esp], ebx
        mov             eax, 4[esp+8]
        mov             edx, 8[esp+8]
        mov             ebx, 12[esp+8]
        call    fn&_
        mov             edx, 0[esp]
        mov             ebx, 4[esp]
        add             esp, 8
        ret             

_TEXT   ENDS
ENDM

CDeclToReg4     MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        sub             esp, 12
        mov             0[esp], edx
        mov             4[esp], ebx
        mov             8[esp], ecx
        mov             eax, 4[esp+12]
        mov             edx, 8[esp+12]
        mov             ebx, 12[esp+12]
        mov             ecx, 16[esp+12]
        call    fn&_
        mov             edx, 0[esp]
        mov             ebx, 4[esp]
        mov             ecx, 8[esp]
        add             esp, 12
        ret

_TEXT   ENDS
ENDM

CDeclToRegFP8     MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        sub             esp, 8
        mov             eax, 12[esp]
        mov             [esp], eax
        mov             eax, 16[esp]
        mov             4[esp], eax

        call    fn&_

        ret

_TEXT   ENDS
ENDM

CDeclToRegFP12     MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        sub             esp, 12
        mov             eax, 16[esp]
        mov             [esp], eax
        mov             eax, 20[esp]
        mov             4[esp], eax
        mov             eax, 24[esp]
        mov             8[esp], eax

        call    fn&_

        ret

_TEXT   ENDS
ENDM

; variable argument list

CDeclToRegOnStack       MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

        jmp             fn&_

_TEXT   ENDS
ENDM

CDeclToRegOnStackI3       MACRO fn
PUBLIC _&fn
EXTERN fn&_:NEAR
_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
_&fn&:

                int                             3
        jmp             fn&_

_TEXT   ENDS
ENDM

CDeclToReg2                     calloc
CDeclToReg1                     malloc
CDeclToReg2                     realloc
CDeclToReg1                     free

CDeclToRegOnStack       printf
CDeclToRegOnStack       sprintf
CDeclToRegOnStack       fprintf
CDeclToReg3                     vsprintf

CDeclToRegOnStack       exit

CDeclToReg2                     strstr
CDeclToReg3                     strncpy
CDeclToReg3                     strncmp
CDeclToReg2                     strchr
CDeclToReg2                     strrchr
CDeclToReg3                     strtol
CDeclToReg1                     tolower

CDeclToReg1                     fclose
CDeclToReg2                     fopen
CDeclToReg4                     fwrite
CDeclToReg4                     fread
CDeclToReg2                     fputs
CDeclToReg1                     fflush
CDeclToReg1                     ftell
CDeclToReg3                     fseek
CDeclToReg2                     fgetc
CDeclToReg2                     fputc

CDeclToReg3                     memmove
CDeclToReg3                     memcpy
CDeclToReg3                     memset

CDeclToReg2                     rename
CDeclToReg1                     remove

CDeclToRegOnStackI3       cos
CDeclToRegOnStackI3       sin
CDeclToRegFP8                   floor
CDeclToRegFP12                  frexp

END
