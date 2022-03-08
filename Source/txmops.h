/******************************************************************************
 * Name         : txmops.h
 * Title        : Performs all the operations on texture pixels.
 * Author       : Peter Leaback
 * Created      : 23/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : sgl.h defines most of the prototypes.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: txmops.h,v $
 * Revision 1.10  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.9  1996/07/17  14:02:28  sjf
 * 1) Got rid of the stuff to read back texels - it wasn't being used anywhere
 * 2) Made the Twiddle stuff conditional on it being a Simulator build
 *    as I threatened to do last check in.
 *
 * Revision 1.8  1996/06/25  16:38:18  sjf
 * Added Twid definition back in as it was actually being used elsewhere.
 *
 * Revision 1.7  1996/05/30  20:34:17  sjf
 *  Added texture caching.
 * 2) Got rid of dead functions.
 *
 * Revision 1.6  1996/04/19  17:23:09  pdl
 * removed a return.
 *
 * Revision 1.5  1996/04/15  11:42:17  pdl
 *  added some PCX1 support
 *
 * Revision 1.4  1996/02/19  11:55:33  sjf
 * Moved common functions out of hardware texas.c files to txmops.
 *
 * Revision 1.3  1995/08/22  15:12:16  pdl
 * changed the interface to InternalGetTexel.
 *
 * Revision 1.2  1995/08/22  11:40:50  pdl
 * added InternalGetTexel.
 *
 * Revision 1.1  1995/08/09  12:27:58  pdl
 * Initial revision
 *
 *
 *****************************************************************************/
#ifndef _TXMOPS_H_
#define _TXMOPS_H_


/*
// The following is used by the display list traversal to decide if
// it needs to do a first pass or not. If there arent any, it doesn't
// need to.
//
*/
extern int nCachedTextures;
extern void InitCachedTextures();


/*
// Routines used internally to handle the cached textures
*/
extern void ResetCachedTextureUsage();
extern void MarkCachedTextureUsed(void *CachedTexturePointer, 
												int		 size);
extern void ReportCachedTexturesToUser();
extern void* GetRealTexture(void *CachedTexturePointer);


sgl_uint32 SetupTransPixel();



/*
// The following two routines are only needed for the Simulators
*/
#if SIMULATOR
/*=========================================================================
name	|InitTwiddle
function|Initialises the bit twiddling table (used for texture indexing)
		|11111111 becomes 101010101010101.Also it initialised the inverse
		|bit twiddling table,giving a y3y2y1y0x3x2x1x0 from an 8 bit twid.
in		|-
out		|-
rd		|-
wr		|twiddle
		|INVtwiddle
pre		|-
post	|-		 
=========================================================================*/
void InitTwiddle();


/*=========================================================================
name	|Twid
function|given an x,y coordinate, generate the memory offset using 
		|twiddling
in		|x, the x coordinate
		|y, the y coordinate
out		|address, the memory offset
rd		|twiddle
wr		|-
pre		|0<x<256
		|0<y<256
		|InitTwiddle has been called.
post	|-		 
=========================================================================*/
unsigned long Twid(int x,int y);

#endif


/******************************************************************************
 * Function Name: SetupOverflowArea
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : This is for PCX1 to overflow sabre parameters into the 
 *				  texture memory. 'Size' bytes are allocated in BOTH banks
 *				  The MNODE_BLOCK gets lost, so this memory can't be 
 *				  deallocated. This probably isn't a problem.
 * Pre-Conditon : This routine is only called once, before any texture is
 *				  allocated, and before SetupTransPixel.
 *				  InitTextureMemory has been called.
 *				  Size IS DIVISIBLE BY 4
 *****************************************************************************/
void SetupOverflowArea(sgl_uint32 Size);


#endif /* _TXMOPS_H_ */
/*
// End of file
*/
