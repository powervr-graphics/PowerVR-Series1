/******************************************************************************
 * Name         : dtsp.h
 * Title        : TSP parameter packing routines for PowerSGL Direct
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
 * $Log: dtsp.h,v $
 * Revision 1.10  1997/10/23  09:58:31  erf
 * Added prototype definitions for PackTSPFlatDecal() and
 * PackFlatDecalAndStore().
 *
 * Revision 1.9  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.8  1997/04/22  09:01:45  rgi
 * Added Arcade/R5000 cache flushing code (new style)
 *
 * Revision 1.7  1997/02/11  12:17:19  ncj
 * Prototypes did not match the functions.
 *
 * Revision 1.6  1997/01/24  19:52:54  gdc
 * Added new version of Pack functions which do not write to TSP memory
 * but require the associated texture function to do it - instead they write
 * back to intermediate structures
 *
 * Revision 1.5  1997/01/23  15:29:27  jrc
 * Added decal mode.
 *
 * Revision 1.4  1996/11/21  12:29:45  gdc
 * added prototypes for translucent function versions
 *
 * Revision 1.3  1996/11/19  18:14:00  gdc
 * added prototypes for shad and light vol options, removed ^M's
 *
 * Revision 1.2  1996/11/19  00:59:17  jop
 * Cleaned up i/f
 *
 * Revision 1.1  1996/11/18  18:30:20  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __DTSP_H__
#define __DTSP_H__

void PackFlatAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles);
void PackFlatDecalAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles);
void PackFlatShadAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles);
void PackFlatLiVolAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles);

#if CACHE_HOST_TSP_PARAM_BUFFER
void PackTSPFlat (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPFlatDecal (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPHigh (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);

void PackTSPFlatShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPHighShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);

void PackTSPFlatLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPHighLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);

void PackTSPFlatTrans (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPHighTrans (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);

void PackTSPFlatTransShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPHighTransShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);

void PackTSPFlatTransLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
void PackTSPHighTransLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, volatile sgl_uint32 *pTSP);
#else
void PackTSPFlat (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPFlatDecal (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPHigh (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);

void PackTSPFlatShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPHighShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);

void PackTSPFlatLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPHighLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);

void PackTSPFlatTrans (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPHighTrans (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);

void PackTSPFlatTransShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPHighTransShad (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);

void PackTSPFlatTransLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
void PackTSPHighTransLiVol (PITRI pTri, PIMATERIAL pMat, int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP);
#endif

#endif

/* end of $RCSfile: dtsp.h,v $ */

