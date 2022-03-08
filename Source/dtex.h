/******************************************************************************
 * Name         : dtex.h
 * Title        : Texture routines for PowerSGL Direct
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dtex.h,v $
 * Revision 1.15  1997/06/20  14:45:36  erf
 * Removed prototype for ProcessD3DFlatTexNonPer() since the routine
 * is the same as ProcessFlatTexNonPer() as for SGLDirect.
 *
 * Revision 1.14  1997/05/13  17:42:48  erf
 * 1. Removed bilinear function prototypes since they don't exist anymore.
 * 2. Added new non perspective correction function prototypes.
 *
 * Revision 1.13  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.12  1997/04/22  09:00:17  rgi
 * Added Arcade/R5000 cache flushing code (new style)
 *
 * Revision 1.11  1997/04/15  17:23:26  rgi
 * Made TSP parameter pointer volatile on arcade system to sustain
 * ordering WRT cache operations.
 *
 * Revision 1.10  1997/03/20  18:02:54  jrc
 * Added bilinear versions of processing functions for OpenGL half texel offset
 *
 * Revision 1.9  1997/01/30  11:14:59  gdc
 * changed the texture packing prototype again!! (sorry).
 * Removed completely redundant Tri pointer and extra TSP pointer as
 * d-buff write will be done outside texture loop to speed up assemblerisation
 *
 * Revision 1.8  1997/01/28  14:24:42  gdc
 * added Material structure size to prototypes
 *
 * Revision 1.7  1997/01/27  18:29:26  gdc
 * altered prototypes for texture packing func to model for writing all
 * TSP params
 *
 * Revision 1.6  1997/01/15  13:19:53  rgi
 * Changed API to pass size of IMATERIAL structure
 * explicitly. This allows the assembler versions to be more
 * isolated from the structure definitions.
 *
 * Revision 1.5  1997/01/09  17:06:47  rgi
 * Added the API's for TexMat(Mul)(WrapUV) to be called
 * directly by dtri.c as part of the first pass. Also split
 * PackTSPTex functionality into two varients.
 *
 * Revision 1.4  1997/01/06  14:18:09  jrc
 * Added declaration for PackTSPTransTex which was previously being
 * implicitly defined.
 *
 * Revision 1.3  1996/11/23  02:33:15  jop
 * Moved matrix mul stuff back to pack end of things
 *
 * Revision 1.2  1996/11/19  00:57:05  jop
 * Cleaned up i/f, pTSP now passed in from dtri.c
 *
 * Revision 1.1  1996/11/18  18:30:01  jop
 * Initial revision
 *
 *
 *****************************************************************************/


#ifndef __DTEX_H__
#define __DTEX_H__

void ProcessFlatTex( PITRI pTri );
void ProcessD3DFlatTex( PITRI pTri );
void ProcessD3DFlatTexWrapU( PITRI pTri );
void ProcessD3DFlatTexWrapV( PITRI pTri );
void ProcessD3DFlatTexWrapUV( PITRI pTri );

void ProcessFlatTexNonPer( PITRI pTri );
void ProcessD3DFlatTexWrapUNonPer( PITRI pTri );
void ProcessD3DFlatTexWrapVNonPer( PITRI pTri );
void ProcessD3DFlatTexWrapUVNonPer( PITRI pTri );

#if CACHE_HOST_TSP_PARAM_BUFFER
/* We have to make sure the R5000 cache manipulation happens in sync */
int PackTSPTex (PIMATERIAL pMat, int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, volatile sgl_uint32 *pTSP, int MatSize);

int PackTSPMipMapTex (PIMATERIAL pMat, int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, volatile sgl_uint32 *pTSP, int MatSize);

int PackTSPTexNonPer (PIMATERIAL pMat, int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, volatile sgl_uint32 *pTSP, int MatSize);

int PackTSPMipMapTexNonPer (PIMATERIAL pMat, int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, volatile sgl_uint32 *pTSP, int MatSize);

int PackTSPTransTex (PITRI pTri, PIMATERIAL pMat, int nPolys, int nCWOffset, 
					 sgl_uint32 nTSPIncrement, volatile sgl_uint32 *pTSP);
#else
int PackTSPTex (PIMATERIAL pMat, int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, int MatSize);

int PackTSPMipMapTex (PIMATERIAL pMat, int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, int MatSize);

int PackTSPTexNonPer (PIMATERIAL pMat, int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, int MatSize);

int PackTSPMipMapTexNonPer (PIMATERIAL pMat, int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, int MatSize);

int PackTSPTransTex (PITRI pTri, PIMATERIAL pMat, int nPolys, int nCWOffset, 
					 sgl_uint32 nTSPIncrement, sgl_uint32 *pTSP);
#endif

#endif

