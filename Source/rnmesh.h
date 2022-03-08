/*****************************************************************************
 *;++
 * Name           : $RCSfile: rnmesh.h,v $
 * Title          : RNMESH.H
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    : Mesh node rendering control - header file
 *                   
 * Program Type   : header file
 *
 * RCS info:
 *
 * $Date: 1997/11/07 15:31:29 $
 * $Revision: 1.21 $
 * $Locker:  $
 * $Log: rnmesh.h,v $
 * Revision 1.21  1997/11/07  15:31:29  mjg
 * Removed the FORCE_NO_FPU pragma message.
 *
 * Revision 1.20  1997/07/14  19:32:58  mjg
 * Added PCX2_003 compile switch.
 *
 * Revision 1.19  1997/06/17  09:27:19  mjg
 * Added the FORCE_NO_FPU option.
 *
 * Revision 1.18  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.17  1997/04/07  13:33:20  gdc
 * added in nCurrTransSetID as a function parameter
 *
 * Revision 1.16  1996/12/10  23:30:38  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR3 to PCX2.
 *
 * Revision 1.15  1996/11/07  13:02:18  erf
 * Added PVR3 compile switch for PCX2 to:
 * 1. Change n32A, B & C to f32A, B & C.
 *
 * Revision 1.14  1996/11/05  14:51:26  gdc
 * moved TRANSEDGE and TRANSVERTEX struct defs from dlmesh.h to rnmesh.h
 *
 * Revision 1.13  1996/09/04  14:37:14  sjf
 * "improved" sorting of translucent passes with large meshes by
 * basing ordering on individual faces, rather than the overall
 *  bounding box.
 *
 * Revision 1.12  1996/08/30  15:49:19  sjf
 * Added in small object rejection for PVR1.
 *
 * Revision 1.11  1996/05/30  20:32:57  sjf
 *  Added texture caching.
 *
 * Revision 1.10  1996/04/02  12:35:09  sjf
 * Changed structure def for ooptimisation.
 *
 * Revision 1.9  1996/03/25  19:37:10  sjf
 * removed dud routine.
 *
 * Revision 1.8  1996/03/25  16:39:31  jop
 * Optimisations
 *
 * Revision 1.7  1995/11/15  14:32:49  jop
 * Added proto for analyser
 *
 * Revision 1.6  1995/11/09  18:22:32  jop
 * xplane struct now uses TRANSEDGE instead of EDGE
 *
 * Revision 1.5  1995/11/02  20:55:25  jop
 * Fixed wrapping bug - data sizes enlarged
 *
 * Revision 1.4  1995/08/08  12:27:29  jop
 * Added XMESHEXTRA structure
 *
 * Revision 1.2  1995/07/27  12:56:57  jop
 * Added proto for rendering routine
 *
 * Revision 1.1  1995/07/25  16:41:19  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#ifndef	__RNMESH_H__
#define __RNMESH_H__

#ifndef FORCE_NO_FPU
#include "pvrlims.h"
#endif

typedef struct tagTRANSEDGE
{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	float	 f32A;			/* Sabre parameters for edge */
	float	 f32B;			     
	float	 f32C;
#else
	sgl_int32	 n32A;			/* Sabre parameters for edge */
	sgl_int32	 n32B;			     
	sgl_int32	 n32C;
#endif
	
} TRANSEDGE, *PTRANSEDGE;

typedef struct tagTRANSVERTEX
{
	float		    *pvVertex;
	sgl_vector      vWorldSpace;
	sgl_2d_vec      vScreenSpace;
	sgl_int32	    	nRegion[2];
	sgl_int32	    	Flags;
	
} TRANSVERTEX, *PTRANSVERTEX;

typedef struct tagXMESHEXTRA
{
	/*
	// pointer back to the original face data. Note we swear on our mothers
	// grave not to change the contents.
	*/
	const FACE	*pOrigFace;

	/*
	// screen regions the face occupies
	*/
	REGIONS_RECT_STRUCT RegionsRect;


	/*
	// Because of the limit on the number of planes with MIDAS3,
	// we need some sort of heuristic rejection method. This will be
	// based on the screen size of the faces
	*/
	#if ISPTSP
	int ApproxSize;
	#endif

	/*
	// pointers to the transformed edges
	*/
	PTRANSEDGE	pE[4];

	/*
	// The number of edges in the face - either 3 or 4
	*/
	int nEdges;

	/*
	// To improve the sorting of large translucent meshes, store a "Z" value
	// for each face.
	*/
	float NearZ;

	/*
	// If the face is Z clipped
	*/
	sgl_bool	ZClipped;

	sgl_uint32		u32SabreIndex;
	sgl_uint32		u32EdgeFlags;

	int			nSabrePlanes;
	
} XMESHEXTRA, *PXMESHEXTRA;


void RnProcessMeshNode (const MESH_NODE_STRUCT *pMesh, 
						MASTER_STATE_STRUCT *pState,
						int nCurrTransSetID);
void RnCTPreProcessMeshNode(const MESH_NODE_STRUCT * pMesh, 
								  MASTER_STATE_STRUCT *pState,
								  void *ppCachedTexture);

void RnMeshOnSglInitialise (void);
void RnMeshGetStats (float Ticks);

#endif

/* end of $RCSfile: rnmesh.h,v $ */

