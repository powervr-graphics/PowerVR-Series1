;*****************************************************************************
;*++
;*  Name        :   $RCSfile: texapiml.asm,v $
;*  Title       :   texapi assembler routines
;*  ASM Author  :   Sheila Yang
;*  Created     :   17/4/97
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
;*  $Date: 1997/09/24 10:37:31 $
;*  $Revision: 1.8 $
;*  $Locker:  $
;*  $Log: texapiml.asm,v $
;; Revision 1.8  1997/09/24  10:37:31  sxy
;; Made conversion of 565, 1555, 555-ck and 565-ck available.
;;
;; Revision 1.7  1997/08/08  16:31:59  sxy
;; Moved the branch 1.6.0.3 to 1.7.
;;
;; Revision 1.6.0.3  1997/08/05  17:14:11  sxy
;; Fixed a bug of missing textures in release build.
;;
;; Revision 1.6.0.2  1997/08/01  16:40:50  sxy
;; For PCX_003 turn off the alpha bit of 555 textures.
;;
;; Revision 1.6.0.1  1997/07/21  16:38:11  sxy
;; PCX2_003 changeds, made the Alpha Mask as a parameter so it can be used both
;; in 4444 or 1555 reversed alpha.
;;
;; Revision 1.6  1997/07/08  16:02:14  sxy
;; added functions for palettised textures. (not in use at the moment)
;;
;; Revision 1.5  1997/05/14  14:51:49  sxy
;; added new functions for packing 8888 (ABGR) format textures.
;;
;; Revision 1.4  1997/04/30  16:13:10  sxy
;; Put reversed alpha into separate functions.
;;
;; Revision 1.3  1997/04/24  11:14:50  sxy
;; Added functions for writing texture into memory and converting reversed alpha.
;;
;; Revision 1.2  1997/04/21  18:56:52  sxy
;; Added a new function FourPairs.
;;
;; Revision 1.1  1997/04/17  16:55:56  sxy
;; Initial revision
;;
;*
;*****************************************************************************

  .386
  .386p


PUBLIC   _Pack565To4444WithColourKey
PUBLIC   _Pack555To4444WithColourKey
PUBLIC   _Pack565To555
PUBLIC   _Pack1555To4444
PUBLIC   _Pack888To555
PUBLIC   _Pack8888To4444
PUBLIC   _Pack888BGRTo555
PUBLIC   _Pack8888BGRTo4444
PUBLIC   _Write8x8PixelsFor256
PUBLIC   _Write8x8PixelsFor128
PUBLIC   _Write8x8PixelsFor64
PUBLIC   _Write8x8PixelsFor32
PUBLIC   _Write8x8PixelsFor16
PUBLIC   _Write8x8Pixels
PUBLIC   _Write8x8For256ReversedAlpha
PUBLIC   _Write8x8For128ReversedAlpha
PUBLIC   _Write8x8For64ReversedAlpha
PUBLIC   _Write8x8For32ReversedAlpha
PUBLIC   _Write8x8For16ReversedAlpha
PUBLIC   _Write8x8ReversedAlpha
;PUBLIC   _Write8x8PixelsPalettised

_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'

;*pPixels =  (((*pSrcPixel) >> 3) & 0x0F00) |  /*R*/
;			(((*pSrcPixel) >> 2) & 0x00F0) |  /*G*/
;			(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
;			if((*pSrcPixel) & 0x8000)	*pPixels |= 0xF000;

ALIGN     4
_Pack1555To4444:

    push	esi
	push	edi
	push	ecx
    push    ebx
	
	mov		esi, [esp+4+16]		 ;pSrc
	mov		edi, [esp+8+16]		 ;pDest
	mov		ecx, [esp+12+16]	 ;nMapsize
	
	shr    ecx, 1
  
loop1555to4444:

    mov		eax, [esi+ecx*4-4]
    mov		ebx, [esi+ecx*4-4]

    shr     eax, 1
	shr     ebx, 2
	and     eax, 0000F000Fh   ;'B'
	and     ebx, 000F000F0h   ;'G'
	or      ebx, eax
	mov		eax, [esi+ecx*4-4]
	shr     eax, 3
	and     eax, 00F000F00h   ;'R'
	or      ebx, eax          ;'R', 'G', 'B'
	
	mov		eax, [esi+ecx*4-4]
	and     eax, 080000000h
	jz      lable_1
	or      ebx, 0F0000000h 
	
lable_1:
    mov		eax, [esi+ecx*4-4]
    and     eax, 000008000h
	jz      lable_2
	or      ebx, 00000F000h 
	
lable_2:

	dec     ecx
	mov		[edi+ecx*4], ebx
	jnz		loop1555to4444
	
	pop     ebx
	pop		ecx
	pop		edi
	pop		esi

ret

;	*pPixels =  (((*pSrcPixel) >> 1) & 0x7FE0) |  /*R* and *G*/
;				((*pSrcPixel) & 0x001F); 		  /*B*/

ALIGN     4
  
_Pack565To555:

	push	esi
	push	edi
	push	ecx
	push	ebx

	mov		esi, [esp+4+16]
	mov		edi, [esp+8+16]
	mov		ecx, [esp+12+16]
	
	shr    ecx, 1
  
pack_loop565:
			
	mov		eax, [esi+ecx*4-4]
    mov		ebx, [esi+ecx*4-4]

    shr     ax, 1
    shr     eax, 1
	shl     ax, 1 
	and     ebx, 0001F001Fh  ;'B'
	and     eax, 07FE07FE0h  ;'R' and 'G'
	or      ebx, eax         ;'R', 'G', 'B'
	
	dec     ecx
	mov		[edi+ecx*4], ebx
	jnz		pack_loop565
				
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret

;*pPixels =  (((*pSrcPixel) >> 4) & 0x0F00) |  /*R*/
;			(((*pSrcPixel) >> 3) & 0x00F0) |  /*G*/
;			(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
;			if(*pSrcPixel == KeyColour)		*pPixels |= 0xF000;
;			Pack565To4444WithColourKey (pSrcPixel, pPixels, nMapSize, KeyColour);

ALIGN     4
  
_Pack565To4444WithColourKey:

	push	esi
	push	edi
	push	ecx
	push	ebx
	push	edx

	mov		esi, [esp+4+20]
	mov		edi, [esp+8+20]
	mov		ecx, [esp+12+20]
	mov		edx, [esp+16+20]
	
	shr    ecx, 1
  
pack_loop565ck:
	mov		ebx, [esi+ecx*4-4]
	mov		eax, [esi+ecx*4-4]
    
    shr     ebx, 3
	shr     eax, 1
	and     ebx, 000F000F0h   ;'G'
	and     eax, 0000F000Fh   ;'B'
	or      ebx, eax
	mov		eax, [esi+ecx*4-4]
	shr     eax, 4
	and     eax, 00F000F00h   ;'R'
	or      ebx, eax          ;'R', 'G', 'B'
	
	mov		eax, [esi+ecx*4-4]
	cmp     ax, dx 
	jne     lable_ck1 
	or      ebx, 00000F000h   ;set the pixel translucent

lable_ck1:
    shr     eax, 16
	cmp     ax, dx 
	jne     lable_ck2 
	or      ebx, 0F0000000h   ;set the pixel translucent

lable_ck2:
	dec     ecx
	mov		[edi+ecx*4], ebx
	jnz		pack_loop565ck
				
    pop		edx
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret	
;*pPixels =  (((*pSrcPixel) >> 3) & 0x0F00) |  /*R*/
;			(((*pSrcPixel) >> 2) & 0x00F0) |  /*G*/
;			(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
;			if(*pSrcPixel == KeyColour)	*pPixels |= 0xF000;
;Pack555To4444WithColourKey (pSrcPixel, pPixels, nMapSize, KeyColour);

ALIGN     4
  
_Pack555To4444WithColourKey:

	push	esi
	push	edi
	push	ecx
	push	ebx
	push	edx

	mov		esi, [esp+4+20]
	mov		edi, [esp+8+20]
	mov		ecx, [esp+12+20]
	mov		edx, [esp+16+20]
	
	shr    ecx, 1
  
pack_loop555ck:
			
	mov		eax, [esi+ecx*4-4]
    mov		ebx, [esi+ecx*4-4]

    shr     eax, 1
	shr     ebx, 2
	and     eax, 0000F000Fh   ;'B'
	and     ebx, 000F000F0h   ;'G'
	or      ebx, eax
	mov		eax, [esi+ecx*4-4]
	shr     eax, 3
	and     eax, 00F000F00h   ;'R'
	or      ebx, eax          ;'R', 'G', 'B'

	mov		eax, [esi+ecx*4-4]
	cmp     ax, dx 
	jne     lable_555ck1 
	or      ebx, 00000F000h   ;set the pixel translucent

lable_555ck1:
    shr     eax, 16
	cmp     ax, dx 
	jne     lable_555ck2 
	or      ebx, 0F0000000h   ;set the pixel translucent

lable_555ck2:
	dec     ecx
	mov		[edi+ecx*4], ebx
	jnz		pack_loop555ck
				
    pop		edx
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret	


ALIGN     4
_Pack888BGRTo555:

	push	esi
	push	edi
	push	ecx
	push	ebx

	mov		esi, [esp+4+16]
	mov		edi, [esp+8+16]
	mov		ecx, [esp+12+16]
	
	shr    ecx, 1
  
pack_loopBGR:
			
	mov		eax, [esi+ecx*8-8]
    mov		ebx, [esi+ecx*8-4]

    ror     ax, 8
	ror     bx, 8
	shr     ax, 3
	shr     bx, 3
	shl     al, 3
	shl     bl, 3
	shr     ax, 3
	shr     bx, 3
	ror     eax, 16
	ror     ebx, 16
	ror     ax, 8
	ror     bx, 8
	shr     eax, 11
	shl     ebx, 5

	dec     ecx

	mov     bx, ax
	mov		[edi+ecx*4], ebx

	jnz		pack_loopBGR
				
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret
	
ALIGN     4

_Pack8888BGRTo4444:

	push	esi
	push	edi
	push	ecx
	push	ebx

	mov		esi, [esp+4+16]
	mov		edi, [esp+8+16]
	mov		ecx, [esp+12+16]
	
	shr    ecx, 1
  
pack_loop4444BGR:
			
	mov		eax, [esi+ecx*8-8]
    mov		ebx, [esi+ecx*8-4]

    rol     eax, 8
	rol     ebx, 8
    ror     ax, 8
	ror     bx, 8
	shr     ax, 4
	shr     bx, 4
	shl     al, 4
	shl     bl, 4
	shr     ax, 4
	shr     bx, 4
	ror     eax, 16
	ror     ebx, 16
	ror     ax, 8
	ror     bx, 8
	shr     ax, 4
	shr     bx, 4
	shl     al, 4
	shl     bl, 4
	shl     ax, 4
	shl     bx, 4
	shr     eax, 8
	shl     ebx, 8

	dec     ecx

	mov     bx, ax
	mov		[edi+ecx*4], ebx

	jnz		pack_loop4444BGR
				
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret	

ALIGN     4
_Pack888To555:

	push	esi
	push	edi
	push	ecx
	push	ebx

	mov		esi, [esp+4+16]
	mov		edi, [esp+8+16]
	mov		ecx, [esp+12+16]
	
	shr    ecx, 1
  
pack_loop:
			
	mov		eax, [esi+ecx*8-8]
    mov		ebx, [esi+ecx*8-4]

    shr     eax, 3
    shr     ebx, 3
	shl     al, 3
	shl     bl, 3
	shl     ax, 3
	shl     bx, 3
	shr     eax, 6
	shl     ebx, 10

	dec     ecx

	mov     bx, ax
	mov		[edi+ecx*4], ebx

	jnz		pack_loop
				
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret
	
ALIGN     4

_Pack8888To4444:

	push	esi
	push	edi
	push	ecx
	push	ebx

	mov		esi, [esp+4+16]
	mov		edi, [esp+8+16]
	mov		ecx, [esp+12+16]
	
	shr    ecx, 1
  
pack_loop4444:
			
	mov		eax, [esi+ecx*8-8]
    mov		ebx, [esi+ecx*8-4]

    shr     eax, 4
    shr     ebx, 4
	shl     al, 4
	shl     bl, 4
	shl     ax, 4
	shl     bx, 4
	shr     eax, 8
	shr     ebx, 8
	shl     ax, 4
	shl     bx, 4
	shr     eax, 4
	shl     ebx, 12

	dec     ecx

	mov     bx, ax
	mov		[edi+ecx*4], ebx

	jnz		pack_loop4444
				
	pop		ebx
	pop		ecx
	pop		edi
	pop		esi

ret	
	

;#define PixelPair(pDst, pSrc, DstIndex, TexWidth) \
;    IW (pDst, DstIndex, (((*(pSrc))<<16) | (*(pSrc+TexWidth))))	
  
mPixelPair  macro pDst, pSrc, DstIndex, TexWidth, MaskRGB

	push	ecx

	mov		ecx, [pSrc]
	mov		eax, [pSrc+TexWidth*2]
	shl     ecx, 16
	mov     cx, ax 
  	and     ecx, MaskRGB		 ;07FFF7FFFh for 555 otherwise 0FFFFFFFFh 
	mov		[pDst+DstIndex*4], ecx
	
	pop		ecx		

endm
	
;#define FourPairs(pDst, pSrc, TexWidth) \
;	PixelPair(pDst, pSrc,              0, TexWidth);\
;	PixelPair(pDst, pSrc+1,            1, TexWidth);\
;	PixelPair(pDst, pSrc+2*TexWidth,   2, TexWidth);\
;	PixelPair(pDst, pSrc+2*TexWidth+1, 3, TexWidth)
	
mFourPairs  macro pDst, pSrc, TexWidth, MaskRGB 

    mPixelPair pDst, pSrc, 0, TexWidth, MaskRGB
    mPixelPair pDst, pSrc+2, 1, TexWidth, MaskRGB
    mPixelPair pDst, pSrc+4*TexWidth, 2, TexWidth, MaskRGB
    mPixelPair pDst, pSrc+4*TexWidth+2, 3, TexWidth, MaskRGB
    
endm

;Write four pairs pixels for
;    pPixels,              pPixels+2, 
;    pPixels+4*MapWidth,   pPixels+4*MapWidth+2, 
;    pPixels+4,            pPixels+6, 
;    pPixels+4*MapWidth+4, pPixels+4*MapWidth+6.

ALIGN     4
_Write8x8PixelsFor256:
    
	push	edi
	push	esi
    push    edx

	mov		edi, [esp+8+12]      ;pDst
	mov		esi, [esp+4+12]      ;pSrc
	mov		edx, [esp+12+12]     ;MaskRGB
	
	mov		edi, [edi]
	
    mFourPairs  edi, esi, 256, edx 
	mFourPairs  edi+16, esi+4, 256, edx  
	mFourPairs  edi+32, esi+2048, 256, edx  
	mFourPairs  edi+48, esi+2052, 256, edx  
	mFourPairs  edi+64, esi+8, 256, edx   
	mFourPairs  edi+80, esi+12, 256, edx   
	mFourPairs  edi+96, esi+2056, 256, edx  
	mFourPairs  edi+112, esi+2060, 256, edx      
    add     edi, 128
	
	mov		eax, [esp+8+12]      ;pDst
	mov     [eax], edi

	pop     edx
	pop 	esi
	pop		edi

    ret
	
ALIGN     4
_Write8x8PixelsFor128:
    
	push	edi
	push	esi
    push    edx

	mov		edi, [esp+8+12]      ;pDst
	mov		esi, [esp+4+12]      ;pSrc
	mov		edx, [esp+12+12]     ;MaskRGB
	
	mov		edi, [edi]
	
    mFourPairs  edi, esi, 128, edx 
	mFourPairs  edi+16, esi+4, 128, edx        
	mFourPairs  edi+32, esi+1024, 128, edx        
	mFourPairs  edi+48, esi+1028, 128, edx        
	mFourPairs  edi+64, esi+8, 128, edx        
	mFourPairs  edi+80, esi+12, 128, edx        
	mFourPairs  edi+96, esi+1032, 128, edx        
	mFourPairs  edi+112, esi+1036, 128, edx  
    add     edi, 128
	
	mov		eax, [esp+8+12]      ;pDst
	mov     [eax], edi

	pop     edx
	pop 	esi
	pop		edi

    ret

;Write8x8PixelsFor64(void *pSrc, int Pitch, void **pDst)
	ALIGN     4
_Write8x8PixelsFor64:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;MaskRGB
	
	mov		edi, [edi]
	shl     ecx, 1   
	
    mFourPairs  edi, esi, 64, edx 
	mFourPairs  edi+16, esi+4, 64, edx        
	mFourPairs  edi+32, esi+512, 64, edx        
	mFourPairs  edi+48, esi+516, 64, edx        
	mFourPairs  edi+64, esi+8, 64, edx        
	mFourPairs  edi+80, esi+12, 64, edx        
	mFourPairs  edi+96, esi+520, 64, edx        
	mFourPairs  edi+112, esi+524, 64, edx  
    add     edi, ecx
	
	mov		eax, [esp+12+16]     ;pDst
	mov     [eax], edi

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
		
		ALIGN     4
_Write8x8PixelsFor32:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;MaskRGB
	
	shl     ecx, 1              
    mov		edi, [edi]
	
    mFourPairs  edi, esi, 32, edx  
	mFourPairs  edi+16, esi+4, 32, edx         
	mFourPairs  edi+32, esi+256, 32, edx         
	mFourPairs  edi+48, esi+260, 32, edx         
    add     edi, ecx
	mFourPairs  edi, esi+8, 32, edx         
	mFourPairs  edi+16, esi+12, 32, edx         
	mFourPairs  edi+32, esi+264, 32, edx         
	mFourPairs  edi+48, esi+268, 32, edx   
    add     edi, ecx
	
	mov		eax, [esp+12+16]     ;pDst
	mov     [eax], edi

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
	
	ALIGN     4
_Write8x8PixelsFor16:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;MaskRGB
	
	shl     ecx, 1               
    mov		edi, [edi]
	
    mFourPairs  edi, esi, 16, edx  
	mFourPairs  edi+16, esi+4, 16, edx         
    add     edi, ecx 
	mFourPairs  edi, esi+128, 16, edx         
	mFourPairs  edi+16, esi+132, 16, edx         
    add     edi, ecx
	mFourPairs  edi, esi+8, 16, edx         
	mFourPairs  edi+16, esi+12, 16, edx         
	add     edi, ecx
	mFourPairs  edi, esi+136, 16, edx         
	mFourPairs  edi+16, esi+140, 16, edx   
    add     edi, ecx
	
	mov		eax, [esp+12+16]     ;pDst
	mov     [eax], edi

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
	
	ALIGN     4
_Write8x8Pixels:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;MaskRGB
	
	shl     ecx, 1               ;4*(Pitch >> 1) = 2*Pitch
	
    mFourPairs  edi, esi, 8, edx  
    add     edi, ecx 
	mFourPairs  edi, esi+4, 8, edx         
    add     edi, ecx 
	mFourPairs  edi, esi+64, 8, edx         
    add     edi, ecx
	mFourPairs  edi, esi+68, 8, edx        
    add     edi, ecx
	mFourPairs  edi, esi+8, 8, edx        
    add     edi, ecx
	mFourPairs  edi, esi+12, 8, edx        
	add     edi, ecx
	mFourPairs  edi, esi+72, 8, edx        
    add     edi, ecx
	mFourPairs  edi, esi+76, 8, edx  
    add     edi, ecx

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
	
; functions for reversed alpha 
mPixelPairReversedAlpha  macro pDst, pSrc, DstIndex, TexWidth, AlphaMasks

	push	ecx

	mov		ecx, [pSrc]
	mov		eax, [pSrc+TexWidth*2]
	shl     ecx, 16
	mov     cx, ax 
	xor     ecx, AlphaMasks     ;0F000F000h or 080008000h
	mov		[pDst+DstIndex*4], ecx
	
	pop		ecx		

endm
	
mFourPairsReversedAlpha  macro pDst, pSrc, TexWidth, AlphaMasks 

    mPixelPairReversedAlpha pDst, pSrc, 0, TexWidth, AlphaMasks
    mPixelPairReversedAlpha pDst, pSrc+2, 1, TexWidth, AlphaMasks
    mPixelPairReversedAlpha pDst, pSrc+4*TexWidth, 2, TexWidth, AlphaMasks
    mPixelPairReversedAlpha pDst, pSrc+4*TexWidth+2, 3, TexWidth, AlphaMasks
    
endm

ALIGN     4
_Write8x8For256ReversedAlpha:
    
	push	edi
	push	esi
    push    edx

	mov		edi, [esp+8+12]      ;pDst
	mov		esi, [esp+4+12]      ;pSrc
	mov		edx, [esp+12+12]     ;AlphaMasks
	
	mov		edi, [edi]
	
    mFourPairsReversedAlpha  edi, esi, 256, edx 
	mFourPairsReversedAlpha  edi+16, esi+4, 256, edx 
	mFourPairsReversedAlpha  edi+32, esi+2048, 256, edx 
	mFourPairsReversedAlpha  edi+48, esi+2052, 256, edx 
	mFourPairsReversedAlpha  edi+64, esi+8, 256, edx  
	mFourPairsReversedAlpha  edi+80, esi+12, 256, edx  
	mFourPairsReversedAlpha  edi+96, esi+2056, 256, edx 
	mFourPairsReversedAlpha  edi+112, esi+2060, 256, edx     
    add     edi, 128
	
	mov		eax, [esp+8+12]      ;pDst
	mov     [eax], edi

	pop     edx
	pop 	esi
	pop		edi

    ret
	
ALIGN     4
_Write8x8For128ReversedAlpha:
    
	push	edi
	push	esi
    push    edx

	mov		edi, [esp+8+12]      ;pDst
	mov		esi, [esp+4+12]      ;pSrc
	mov		edx, [esp+12+12]     ;AlphaMasks
	
	mov		edi, [edi]
	
    mFourPairsReversedAlpha  edi, esi, 128, edx 
	mFourPairsReversedAlpha  edi+16, esi+4, 128, edx        
	mFourPairsReversedAlpha  edi+32, esi+1024, 128, edx        
	mFourPairsReversedAlpha  edi+48, esi+1028, 128, edx        
	mFourPairsReversedAlpha  edi+64, esi+8, 128, edx        
	mFourPairsReversedAlpha  edi+80, esi+12, 128, edx        
	mFourPairsReversedAlpha  edi+96, esi+1032, 128, edx        
	mFourPairsReversedAlpha  edi+112, esi+1036, 128, edx  
    add     edi, 128
	
	mov		eax, [esp+8+12]      ;pDst
	mov     [eax], edi

	pop     edx
	pop 	esi
	pop		edi

    ret

	ALIGN     4
_Write8x8For64ReversedAlpha:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;AlphaMasks
	
	mov		edi, [edi]
	shl     ecx, 1   
	
    mFourPairsReversedAlpha  edi, esi, 64, edx 
	mFourPairsReversedAlpha  edi+16, esi+4, 64, edx        
	mFourPairsReversedAlpha  edi+32, esi+512, 64, edx        
	mFourPairsReversedAlpha  edi+48, esi+516, 64, edx        
	mFourPairsReversedAlpha  edi+64, esi+8, 64, edx        
	mFourPairsReversedAlpha  edi+80, esi+12, 64, edx        
	mFourPairsReversedAlpha  edi+96, esi+520, 64, edx        
	mFourPairsReversedAlpha  edi+112, esi+524, 64, edx  
    add     edi, ecx
	
	mov		eax, [esp+12+16]     ;pDst
	mov     [eax], edi

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
		
		ALIGN     4
_Write8x8For32ReversedAlpha:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;AlphaMasks
	
	shl     ecx, 1              
    mov		edi, [edi]
	
    mFourPairsReversedAlpha  edi, esi, 32, edx  
	mFourPairsReversedAlpha  edi+16, esi+4, 32, edx         
	mFourPairsReversedAlpha  edi+32, esi+256, 32, edx         
	mFourPairsReversedAlpha  edi+48, esi+260, 32, edx         
    add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+8, 32, edx         
	mFourPairsReversedAlpha  edi+16, esi+12, 32, edx         
	mFourPairsReversedAlpha  edi+32, esi+264, 32, edx         
	mFourPairsReversedAlpha  edi+48, esi+268, 32, edx   
    add     edi, ecx
	
	mov		eax, [esp+12+16]     ;pDst
	mov     [eax], edi

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
	
	ALIGN     4
_Write8x8For16ReversedAlpha:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;AlphaMasks
	
	shl     ecx, 1               
    mov		edi, [edi]
	
    mFourPairsReversedAlpha  edi, esi, 16, edx  
	mFourPairsReversedAlpha  edi+16, esi+4, 16, edx         
    add     edi, ecx 
	mFourPairsReversedAlpha  edi, esi+128, 16, edx         
	mFourPairsReversedAlpha  edi+16, esi+132, 16, edx         
    add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+8, 16, edx         
	mFourPairsReversedAlpha  edi+16, esi+12, 16, edx         
	add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+136, 16, edx         
	mFourPairsReversedAlpha  edi+16, esi+140, 16, edx   
    add     edi, ecx
	
	mov		eax, [esp+12+16]     ;pDst
	mov     [eax], edi

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret
	
	ALIGN     4
_Write8x8ReversedAlpha:
    
	push	edi
	push	esi
	push    ecx
	push    edx

	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	mov		edx, [esp+16+16]     ;AlphaMasks

	shl     ecx, 1               ;4*(Pitch >> 1) = 2*Pitch
	
    mFourPairsReversedAlpha  edi, esi, 8, edx  
    add     edi, ecx 
	mFourPairsReversedAlpha  edi, esi+4, 8, edx         
    add     edi, ecx 
	mFourPairsReversedAlpha  edi, esi+64, 8, edx         
    add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+68, 8, edx        
    add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+8, 8, edx        
    add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+12, 8, edx        
	add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+72, 8, edx        
    add     edi, ecx
	mFourPairsReversedAlpha  edi, esi+76, 8, edx  
    add     edi, ecx

	pop     edx
	pop     ecx
	pop 	esi
	pop		edi

    ret


;functions for palettised textures
mPixelPairPalettised  macro pDst, pSrc, DstIndex, pPalette, pSrc2 
	
	push	edx
	push    ecx
	push    eax

	mov		ecx, pPalette
	push	ecx
	push	eax
	movzx	eax, byte ptr[pSrc]
	;mov		eax, [pSrc]
	;and		eax, 0000000FFh
   	shl		eax, 1
	add		ecx, eax
	mov		edx, [ecx]
	sub		ecx, eax

	pop 	eax
	movzx	eax, byte ptr[pSrc2]
	;mov		eax, [pSrc2]
	;and		eax, 0000000FFh
  	shl		eax, 1
	add		ecx, eax
	mov		ecx, [ecx]

	shl     edx, 16
	mov     dx, cx 
	mov		[pDst+DstIndex*4], edx		
	pop		ecx
	mov		pPalette, ecx

	pop     eax
	pop     ecx
	pop 	edx

endm
		
mFourPairsPalettised  macro pDst, pSrc, TexWidth, pPalette, pSrc2 

    mPixelPairPalettised pDst, pSrc,              0, pPalette, pSrc2
    mPixelPairPalettised pDst, pSrc+1,            1, pPalette, pSrc2+1
    mPixelPairPalettised pDst, pSrc+2*TexWidth,   2, pPalette, pSrc2+2*TexWidth
    mPixelPairPalettised pDst, pSrc+2*TexWidth+1, 3, pPalette, pSrc2+2*TexWidth+1
    
endm


ALIGN     4
_Write8x8PixelsPalettised:
    
	push	edi
	push	esi
    push    edx
	push	ecx

	mov		ebx, [esp+20+16]     ;pSrc+Pitch
	mov		edx, [esp+16+16]     ;pPalette
	mov		edi, [esp+12+16]     ;pDst
	mov		ecx, [esp+8+16]      ;Pitch
	mov		esi, [esp+4+16]      ;pSrc
	
	shl		ecx, 2				 ;4*Pitch
	mov		eax, [esp+8+16]      ;Pitch
	mov		edi, [edi]

    mFourPairsPalettised  edi,     esi,   eax, edx, ebx 
	mFourPairsPalettised  edi+16,  esi+2, eax, edx, ebx+2 
   	mFourPairsPalettised  edi+64,  esi+4, eax, edx, ebx+4  
	mFourPairsPalettised  edi+80,  esi+6, eax, edx, ebx+6
   
    add		esi, ecx
	add		ebx, ecx
   	mFourPairsPalettised  edi+32,  esi,   eax, edx, ebx 
   	mFourPairsPalettised  edi+48,  esi+2, eax, edx, ebx+2 
   	mFourPairsPalettised  edi+96,  esi+4, eax, edx, ebx+4 
   	mFourPairsPalettised  edi+112, esi+6, eax, edx, ebx+6
   	sub		esi, ecx
	sub		ebx, ecx     

    add     edi, 128
	mov		eax, [esp+12+16]      ;pDst
	mov     [eax], edi
	
	pop		ecx
	pop     edx
	pop 	esi
	pop		edi

    ret


_TEXT   ENDS

    END

;* end of $RCSfile: texapiml.asm,v $
