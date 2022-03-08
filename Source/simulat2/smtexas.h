/******************************************************************************
 * Name         : smtexas.h
 * Title        : This is the texas software simulator.
 * Author       : Peter Leaback
 * Created      : 9/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : This a convert from RGL.
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: smtexas.h,v $
 * Revision 1.4  1996/06/27  12:04:11  sjf
 * Added write N pixel pairs.
 *
 * Revision 1.3  1996/04/22  15:48:47  pdl
 * made the texture memory 32 bits wide.
 * added TEX_PARAM_SIZE
 *
 * Revision 1.2  1996/02/19  14:19:57  sjf
 * Moved Twiddle functions to txmops
 *
 * Revision 1.1  1995/12/18  14:22:30  sjf
 * Initial revision
 *
 * Revision 1.11  1995/12/05  10:31:23  pdl
 * *** empty log message ***
 *
 * Revision 1.10  1995/12/05  10:20:21  pdl
 * coded the PCX1 textures.
 *
 * Revision 1.9  1995/11/28  11:58:34  pdl
 * enabled more parameter file writing.
 *
 * Revision 1.8  1995/11/24  17:30:33  pdl
 * added two pixel access.
 *
 * Revision 1.7  1995/08/22  11:08:35  pdl
 * added FetchPixel.
 *
 * Revision 1.6  1995/08/19  09:45:45  sjf
 * Added fog bit.
 *
 * Revision 1.5  1995/08/03  12:53:37  pdl
 * nothing!
 *
 * Revision 1.4  1995/07/30  20:09:04  pdl
 * removed the BMP writing prototype.
 *
 * Revision 1.3  1995/07/26  17:22:27  pdl
 * removed the parameter store..
 *
 * Revision 1.2  1995/07/24  15:41:35  pdl
 * added MASK_FLIP_U and MASK_FLIP_V.
 *
 * Revision 1.1  1995/07/17  16:08:16  pdl
 * Initial revision
 *
 *
 *****************************************************************************/


#define DUMP_PARAM_FILES	1

#define TEXTURE_MEMORY_SIZE	2097152

		/* how much to allocate in the texture memory for params*/

#define TEX_PARAM_SIZE (32*1024)

#ifdef PCX1

#define BIG_BANK 0x100000    /*this is where the mip-maps have to be separated over etc*/

#else

#define BIG_BANK 0x400000    /*this is where the mip-maps have to be separated over etc*/

#endif

#define MAX_X_DIM 1024	  /* temp define*/
#define MAX_Y_DIM 768	  /* temp define*/

#define CACHE_ENTRIES 6
#define CACHE_BLOCK_SIZE 3


/*
** control word
*/

#define MASK_TEXTURE					0x80000000
#define MASK_SMOOTH_SHADE				0x40000000
#define MASK_DISABLE_FOG				0x20000000
#define MASK_SHADOW_FLAG				0x10000000
#define MASK_FLAT_HIGHLIGHT             0x04000000

/* up to here */

/*
** texture control word
*/

#define MASK_EXPONENT					0x003C0000
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


extern unsigned long textureMemory[TEXTURE_MEMORY_SIZE>>1]; /* changed to 32 bit words*/



/*=========================================================================
name	|Texas
function|This is the pixel interface with Sabre. Sabre communucates x,y
		|,instruction address etc and the routine colours that pixel.
in		|x, the x coordinate
		|y, the y coordinate
		|address, the address of the shading instruction
		|shadow, the shadow bit 1==in shadow
		|fog, the fogging interpolation factor
out		|-
rd		|parameterStore
wr		|frameBuffer
pre		|0<x<2048
		|0<y<2048
post	|-		 
=========================================================================*/
void Texas(int x,int y,unsigned long address,unsigned char shadow,unsigned char fog);

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
void WritePixel(unsigned long address,unsigned short pixel);

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
void WriteTwoPixels(unsigned long address,unsigned long pixels);

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
void WriteNPixelPairs( unsigned long address,
					   UINT16 *pSrc,
					   long  numPixelPairs);


/*=========================================================================
name	|DumpTextureMemory
function|Dumps the 1Mb texture memory to a file called "texturemem.bin"
in		|-
out		|-
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void DumpTextureMemory();

/*=========================================================================
name	|DumpParmeterMemory
function|Dumps 1Mb of the parameter store to a file called "parameters.bin"
in		|-
out		|-
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void DumpParameterMemory();

/*=========================================================================
name	|FlushCache
function|makes each cache slot empty
in		|-
out		|-
rd		|-
wr		|tags
	   	|missCount
pre		|-
post	|-		 
=========================================================================*/
void FlushCache();

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
void TexasSetDim(int x,int y);

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
void TexasSetFogColour(sgl_map_pixel Colour);

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
void TexasSetCFRScale(UINT16 Scale);

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

/*=========================================================================
name	|InitTexasSimulator
function|opens the param files for writing
in		|
out		|-
rd		|
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void InitTexasSimulator();

/*=========================================================================
name	|FinishTexasSimulator
function|closes the param files.
in		|
out		|-
rd		|
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void FinishTexasSimulator();

#include <stdio.h>

extern FILE *FshadePreCalc;
extern FILE *FtexPreCalc;
extern FILE *FrawTexPixels;
extern FILE *FtexasInput;
extern FILE *Fpixels;
extern FILE *FtexasInputSabOutputFormat; /*I'm having a laugh*/
