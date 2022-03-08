/******************************************************************************
 * Name         : hwtexas.h
 * Title        : This is the texas hardware support routines.
 * Author       : Peter Leaback
 * Created      : 20/10/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: hwtexas.h,v $
 * Revision 1.10  1997/08/08  19:09:18  mjg
 * Added new features, relevent only for PCX2_003 - namely masks for
 * bilinear per surface and alpha blending.
 *
 * Revision 1.9  1997/06/04  13:52:08  gdc
 * stopped compiler warnings
 *
 * Revision 1.8  1997/05/28  08:22:15  gdc
 * chnaged default TEX_PARAM_SIZE to match current ini file TSPParamSize
 *
 * Revision 1.7  1997/05/14  00:24:04  mjg
 * Made file able to be used for PCX1 and PCX2.
 *
 * Revision 1.6  1997/04/03  18:01:49  mjg
 * Added more CALL_CONVs
 *
 * Revision 1.5  1996/06/27  12:03:30  sjf
 *  Fixed Signed/Unsigned bug in WriteNPixelPairs.
 *
 * Revision 1.4  1996/06/25  16:27:07  sjf
 * Added WriteNPixelpairs  to speed up texture caching.
 *
 * Revision 1.3  1996/05/07  14:19:33  jop
 * Increased TEX_PARAM_SIZE to 128 k
 *
 * Revision 1.2  1996/04/26  16:23:37  pdl
 * added the texas param size.
 *
 * Revision 1.1  1996/04/17  17:46:51  pdl
 * Initial revision
 *
 *
 *
 *****************************************************************************/


#ifndef __HWTEXAS_H__
#define __HWTEXAS_H__
   	/* how much to allocate in the texture memory for params*/

#define TEX_PARAM_SIZE (256*1024)


#define BIG_BANK 0x100000    /*this is where the mip-maps have to be separated over etc*/


#define MAX_X_DIM 1024	  /* temp define*/
#define MAX_Y_DIM 768	  /* temp define*/


/*
** control word
*/

#define MASK_TEXTURE					0x80000000
#define MASK_SMOOTH_SHADE				0x40000000
#define MASK_DISABLE_FOG				0x20000000
#define MASK_SHADOW_FLAG				0x10000000
#define MASK_BLEND_ONE_OVER_ALPHA 		0x08000000 /* PCX2_003 source blending */
#define MASK_FLAT_HIGHLIGHT             0x04000000
#define MASK_BLEND_ALPHA				0x02000000 /* PCX2_003 destination blending */
/*
** texture control word
*/

#define MASK_EXPONENT					0x003C0000
#define	MASK_BILINEAR					0x00020000 /* PCX2_003 bilinear per surface */
#define MASK_GLOBAL_TRANS               0x0001E000
#define MASK_FLIP_UV					0x00001800
#define MASK_FLIP_U						0x00001000
#define MASK_FLIP_V						0x00000800
#define MASK_TRANS    					0x00000400

#define SHIFT_EXPONENT					18
#define SHIFT_GLOBAL_TRANS		   		13
#define SHIFT_FLIP_UV   		   		11

/*
** texture type byte
*/

#define MASK_MIP_MAPPED					0x80000000
#define MASK_8_16_MAPS					0x40000000
#define MASK_MAP_SIZE					0x30000000
#define MASK_4444_555					0x08000000

#define SHIFT_MAP_SIZE					28

/*
** pmip coefficient 
*/

#define MASK_PMIP_M						0xFF000000
#define MASK_PMIP_E						0x00FC0000

#define SHIFT_PMIP_M					24
#define SHIFT_PMIP_E					18




/*=========================================================================
name	|WritePixel
function|writes a pixel into the texture memory
in		|address,
		|pixel,
out		|-
rd		|-
wr		|textureMemory
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV WritePixel(unsigned long address,unsigned short pixel);

/*=========================================================================
name	|WriteTwoPixels
function|writes a pair of adjacent pixels into the texture memory
in		|address,
		|pixels,
out		|-
rd		|-
wr		|textureMemory
pre		|address is two word aligned.
post	|-		 
=========================================================================*/
void CALL_CONV WriteTwoPixels(unsigned long address,unsigned long pixels);

/*=========================================================================
name	|WriteNPixelPairs
function|writes an even number of pixels to the texture memory, starting from
		|the given addresses.
		|
out		|-
rd		|-
wr		|textureMemory
pre		|address is two word aligned.
post	|-		 
=========================================================================*/
void CALL_CONV WriteNPixelPairs( unsigned long address,
					  	 		 sgl_uint16 *pixels,
						   		 long  numPixelPairs );


/*=========================================================================
name	|TexasSetDim
function|temp function for bringing texas simulator up with sgl.
in		|x,
		|y,
out		|-
rd		|-
wr		|Bitmap_ydim,Bitmap_xdim
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasSetDim(int x, int y);

/*=========================================================================
name	|TexasSetFogColour
function|.
in		|Colour,
out		|-
rd		|-
wr		|fogColour
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasSetFogColour(sgl_map_pixel Colour);

/*=========================================================================
name	|TexasSetCFRScale
function|This can be the distance between the center of projection and the
		|the projection plane.This is a function,because the HW may need it.
in		|Scale,
out		|-
rd		|-
wr		|fogColour
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasSetCFRScale(sgl_uint32 Scale);

/*=========================================================================
name	|FetchPixel
function|returns a raw 16 bit pixel given an address
		|this may perform page break counting some time.
in		|address,
out		|pixel,
rd		|textureMemory
wr		|-
pre		|-
post	|-		 
=========================================================================*/
unsigned short FetchPixel(unsigned long address);

#endif
