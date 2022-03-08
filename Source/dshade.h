/******************************************************************************
 * Name         : dshade.h
 * Title        : Smooth shading routines for PowerSGL Direct
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
 * $Log: dshade.h,v $
 * Revision 1.8  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.7  1997/04/22  08:59:14  rgi
 * Added Arcade/R5000 cache flushing code (new style)
 *
 * Revision 1.6  1997/03/13  16:19:53  ncj
 * Remove const
 *
 * Revision 1.5  1997/01/24  19:52:22  gdc
 * Added new version of Pack functions which do not write to TSP memory
 * but require the associated texture function to do it - instead they write
 * back to intermediate structures
 *
 * Revision 1.4  1997/01/14  17:08:16  sjf
 * Made input only parameters consts.
 *
 * Revision 1.3  1996/11/20  10:04:44  gdc
 * added shadow and light volume equivalent prototypes
 *
 * Revision 1.2  1996/11/19  00:55:53  jop
 * Updated i/f, allowing texture parameters to be inserted into
 * the middle
 *
 * Revision 1.1  1996/11/18  19:08:03  jop
 * Initial revision
 *
 * Revision 1.1  1996/11/18  18:29:36  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __DSHADE_H__
#define __DSHADE_H__

void PackSmoothAndStore ( const ITRI *pTri, IMATERIAL *pMat, int nPolys);
void PackSmoothShadAndStore ( const ITRI *pTri, IMATERIAL *pMat, int nPolys);
void PackSmoothLiVolAndStore ( const ITRI *pTri, IMATERIAL *pMat, int nPolys);

#if CACHE_HOST_TSP_PARAM_BUFFER
void PackTSPSmooth ( const ITRI *pTri, const IMATERIAL *pMat, int nPolys, 
					sgl_uint32 TSPIncrement, sgl_uint32 TSPGap, volatile sgl_uint32 *pTSP);

void PackTSPSmoothShad (const ITRI *pTri, const IMATERIAL *pMat, int nPolys, 
					sgl_uint32 TSPIncrement, sgl_uint32 TSPGap, volatile sgl_uint32 *pTSP);

void PackTSPSmoothLiVol (const ITRI *pTri, const IMATERIAL *pMat, int nPolys, 
					sgl_uint32 TSPIncrement, sgl_uint32 TSPGap, volatile sgl_uint32 *pTSP);
#else
void PackTSPSmooth ( const ITRI *pTri, const IMATERIAL *pMat, int nPolys, 
					sgl_uint32 TSPIncrement, sgl_uint32 TSPGap, sgl_uint32 *pTSP);

void PackTSPSmoothShad (const ITRI *pTri, const IMATERIAL *pMat, int nPolys, 
					sgl_uint32 TSPIncrement, sgl_uint32 TSPGap, sgl_uint32 *pTSP);

void PackTSPSmoothLiVol (const ITRI *pTri, const IMATERIAL *pMat, int nPolys, 
					sgl_uint32 TSPIncrement, sgl_uint32 TSPGap, sgl_uint32 *pTSP);
#endif

#endif
