/******************************************************************************
 * Name         : disp.h
 * Title        : ISP plane packing routines for PowerSGL Direct
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
 * $Log: disp.h,v $
 * Revision 1.9  1997/08/18  16:24:47  gdc
 * more messing with prototypes
 *
 * Revision 1.8  1997/08/15  11:14:33  gdc
 * chnaged prototype name
 *
 * Revision 1.7  1997/08/14  09:13:15  gdc
 * removed ISPTSP stuff and added a prototype for a fog to
 * output vertex fog triangles
 *
 * Revision 1.6  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.5  1997/02/20  21:51:28  mjg
 * 3.1: Smooth/Trans/Shad-LightVol fix for ISPTSP passes shading info about.
 *
 * Revision 1.4  1996/11/30  04:54:20  jop
 * Changed interface to polygon code.
 *
 * Revision 1.3  1996/11/23  04:15:00  jop
 * Structure and i/f changes
 *
 * Revision 1.2  1996/11/22  02:25:26  jrc
 * Added polygon packing function, and dummy parameter to triangle function
 * for type compatibility with the polygon one.
 *
 * Revision 1.1  1996/11/18  18:29:40  jop
 * Initial revision
 *
 *****************************************************************************/

#ifndef __DISP_H__
#define __DISP_H__

int PackISPTri (PITRI pTri, int nTriangles, sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement);

int PackISPTriExtra (PITRI pTri, PIMATERIAL pMat, int nTriangles, 
					 sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement);

int PackISPPolygon (PITRI pTri, PIMATERIAL pMat, PIEDGE pEdge, int nTriangles,
					sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement);

#endif

/* end of $RCSfile: disp.h,v $ */
