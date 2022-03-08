/*****************************************************************************
 *;++
 * Name            :    $Source: /user/rcs/revfiles/sabre/sgl/RCS/pvrd.h,v $
 * Title           :    PVRDirect function entry points.
 * C Author        :    Edward Forde
 * Created         :    22/04/97
 *
 * Copyright	   : 1995-2022 Imagination Technologies (c)
 * License		   : MIT
 *
 * Description     :	Include file for entry routines to PVRDirect.
 *                    
 * Program Type    :	C module (ANSI)
 *
 * RCS info:
 * $Log: pvrd.h,v $
 * Revision 1.10  1997/08/20  16:56:19  erf
 * Added D3D DrawPrimitive support. New routine PVRDTrianglesPrimitive added
 * and change to PVRDPoints so that a point list is passed in. Needed
 * for DrawIndexedPrimitive.
 *
 * Revision 1.9  1997/07/18  08:45:31  gdc
 * added a Y Region size field so that the HAL can control region height
 *
 * Revision 1.8  1997/07/03  17:56:36  erf
 * Added PVRDShadows() routine to enable shadow and light volumes
 * in D3D.
 *
 * Revision 1.7  1997/05/12  15:16:40  erf
 * New field in the PVRDLines().
 *
 * Revision 1.6  1997/05/08  11:29:38  erf
 * PVRDirect now a seperate library. Yahoo !!!
 *
 * Revision 1.5  1997/05/01  08:31:31  erf
 * Added PVRDirect interface.
 *
 * Revision 1.4  1997/04/26  18:50:55  erf
 * Included texapi.h
 *
 * Revision 1.3  1997/04/23  16:28:03  erf
 * Put in more typedef's for John H.
 *
 * Revision 1.2  1997/04/23  15:03:18  erf
 * Stream lined the PVRDirect context structure.
 * Added prototype definitions for the functions.
 *
 *;--
 *****************************************************************************/


#ifndef __PVRD_H__
#define __PVRD_H__

#ifndef __PVROSAPI_H__
#include "pvrosapi.h"
#endif

#ifndef __TEXTAPI__
#include "texapi.h"
#endif

/* PVRDirect context structure definition for use with private functions as defined in
 * pvrd.c
 */
typedef struct _PVR_RENDERCONTEXT_
{
	HDEVICE			hDevice;
	HLDEVICE		hLogicalDevice;
	HTEXHEAP		hTextureHeap;

	/* Size of the device required.
	 */
	sgl_uint32		dwWidth;
	sgl_uint32		dwHeight;
	sgl_uint32 		lStride;

	/* Start of render address.
	 */
	sgl_uint32		dwPhysRenderAddress;

	/* Pointer to end of render flags.
	 */
	volatile		sgl_uint32		*pEndOfRender;

	SGLCONTEXT		SGLContext;

	/* Texture information.
	 */
	TEXTUREFORMAT	RenderSurfFormat;

	sgl_uint32      RegionYHeight;
}PVR_RENDERCONTEXT, *PPVR_RENDERCONTEXT;

/* These are defined the same as D3D.
 */
typedef enum _PRIMITIVETYPE {
        POINTLIST = 1,
        LINELIST = 2,
        LINESTRIP = 3,
        TRIANGLELIST = 4,
        TRIANGLESTRIP = 5,
        TRIANGLEFAN = 6
}PRIMITIVETYPE;

#endif	/* __PVRD_H__	*/

#include "pvrif.h"

/* Prototype definitions for pvrif.h file to generate the complete interface to
 * PVRDirect.
 */
API_START (PVRDIRECT)

	API_FN (PVROSERR, PVRDCreateRenderContext,	(PPVR_RENDERCONTEXT pRenderContext))
	API_FN (PVROSERR, PVRDDestroyRenderContext,	(PPVR_RENDERCONTEXT pRenderContext))
	API_FN (PVROSERR, PVRDStartFrame,			(PPVR_RENDERCONTEXT pRenderContext))
	API_FN (PVROSERR, PVRDTriangles,			(PPVR_RENDERCONTEXT pRenderContext,
												sgl_uint32 dwNumFaces,
												int pFaces[][3],
												PSGLVERTEX pVertices))
	API_FN (PVROSERR, PVRDTrianglesPrimitive,	(PPVR_RENDERCONTEXT pRenderContext,
												sgl_uint32 dwNumFaces,
												sgl_uint16 *pFaces,
												PSGLVERTEX pVertices,
												PRIMITIVETYPE PrimitiveType))
	API_FN (PVROSERR, PVRDLines,				(PPVR_RENDERCONTEXT	pRenderContext,
												sgl_uint32 dwNumLines,
												sgl_uint16 pLines[][2],
												PSGLVERTEX pVertices,
												PRIMITIVETYPE PrimitiveType))
	API_FN (PVROSERR, PVRDPoints,				(PPVR_RENDERCONTEXT	pRenderContext,
					 							sgl_uint32 dwNumPoints,
												sgl_uint16 *pPoints,
					 							PSGLVERTEX pVertices))
	API_FN (PVROSERR, PVRDShadows,				(PPVR_RENDERCONTEXT	pRenderContext,
											   	int					nNumFaces,
											   	int					pFaces[][3],
						 	   					PSGLVERTEX			pVertices,
											   	float				fBoundingBox[2][2]))
	API_FN (PVROSERR, PVRDPreRender,			(PPVR_RENDERCONTEXT pRenderContext))
	API_FN (PVROSERR, PVRDRender,				(PPVR_RENDERCONTEXT	pRenderContext)) 
	API_FN (PVROSERR, PVRDRenderStrip,			(PPVR_RENDERCONTEXT pRenderContext,
												int nStrip,
												int nXExtent[2]))
API_END (PVRDIRECT)

/* end of $RCSfile: pvrd.h,v $ */
