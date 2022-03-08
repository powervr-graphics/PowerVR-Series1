/**************************************************************************
 * Name         : rnconvst.h
 * Title        : Rendering of Convex Primitives - Data structures.
 * Author       : Simon Fenney
 * Created      : 22 June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Description of the data structures used to hold convex
 *				  primitive data. This is needed by a couple of different
 *				  "C" files but not by all that need the prototype of the
 *				  convex processing routine.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnconvst.h,v $
 * Revision 1.19  1997/11/07  15:30:32  mjg
 * Removed the FORCE_NO_FPU pragma message.
 *
 * Revision 1.18  1997/07/14  19:31:41  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.17  1997/06/17  10:24:44  mjg
 *  Added FORCE_NO_FPU option.
 *
 * Revision 1.16  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.15  1996/12/10  23:32:13  erf
 * Changed PVR3 to PCX2.
 *
 * Revision 1.14  1996/11/07  13:01:58  erf
 * Added PVR3 compile switch for PCX2 to:
 * 1. Change n32A, B & C to f32A, B & C.
 *
 * Revision 1.13  1996/11/04  11:41:10  erf
 * Added semi-colon (;).
 *
 * Revision 1.12  1996/10/15  15:16:15  gdc
 * added a structure to carry shadow limiting planes and
 * removed some ^M's
 *
 * Revision 1.11  1996/09/19  14:17:13  mal
 * Added vNewProjrepPoint and bNewRepPoint to the TRANSFORMED_PLANES_STRUCT
 * so Smooth Shading can use the projected rep point NOT clipped to the
 * H/W limits. Re: NECHE 094.
 *
 * Revision 1.10  1996/09/12  13:14:55  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES
 * as shadow objects may now have more than SGL_MAX_PLANES.
 * (Actually twice as many) Re: NHE044a, NHE049A
 *
 * Revision 1.9  1996/09/09  17:05:22  mal
 * Created an internal shadow plane count twice that of the
 * external plane count. SGL_MAX_PLANES
 *
 * Revision 1.8  1996/04/02  12:34:00  sjf
 * Added const defs to pointers into original data.
 *
 * Revision 1.7  1996/01/31  14:25:10  jop
 * changed shading/points struct
 *
 * Revision 1.6  1995/11/10  16:15:59  sjf
 * deleted unused field in projected planes.
 *
 * Revision 1.5  1995/09/15  18:12:13  jrc
 * Added sentry against multiple #includes of this header file.
 *
 * Revision 1.4  1995/09/06  14:55:11  sjf
 * Added a plane categories/lists structure.
 *
 * Revision 1.3  1995/08/14  11:39:30  sjf
 * changed A & B values to be in packed format, not float.
 *
 * Revision 1.2  1995/07/20  15:41:34  sjf
 * Added more of the required transformed plane structure fields.
 *
 * Revision 1.1  1995/07/06  16:21:57  sjf
 * Initial revision
 *
 **************************************************************************/
#ifndef __RNCONVST_H
#define __RNCONVST_H

#ifndef FORCE_NO_FPU
#include "pvrlims.h"
#endif

/*
// Define the transformed plane structure. This holds plane data after 
// transformation from local to world coordiantes (plus other stuff)
*/
typedef struct
{
	/* This is just a copy of the flags from the node structure	*/
	int flags;

	sgl_vector normal;	/*Plane normal*/
	sgl_vector repPnt;	/*Representative Point*/
	float	   d;		/* plane "d " value */

	/*
	// Projected position of plane, and texas Tag (where required)
	*/
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	float	f32A;
	float	f32B;
	float	f32C;
#else
	sgl_int32	n32A;
	sgl_int32	n32B;
	sgl_int32	n32C;
#endif
	sgl_uint32	u32TexasTag;

	/*
	// Projected Representative point. This is an on-screen (or a
	// near on-screen) value used mainly for determining the mip
	// mapping level in texturing.
	//
	// Note that it may have moved from the original repPnt value.
	*/
	sgl_vector projRepPoint;

	/*
	// pointers to various parts of the plane data
	// The original untransformed data, shading, and texture data.
	//
	// NOTE we will not modify the values these pointers reference.
	*/
	const CONV_PLANE_STRUCT 	* pOriginalData;
	const CONV_SHADING_STRUCT	* pShadingData;
	const CONV_POINTS_STRUCT	* pPointsData;
	const CONV_TEXTURE_UNION	* pTextureData;
	sgl_vector vNewProjRepPoint;
	sgl_bool   bNewProjRepPoint;

} TRANSFORMED_PLANE_STRUCT;


/*
// Definition of structure which carries transformed shadow limiting planes
// this is used like a state in D-L traversal
*/

#define SGL_MAX_SHAD_LIM_PLANES 16
typedef struct
{
    int nNumShadLimPlanes;
	TRANSFORMED_PLANE_STRUCT TransShadLimPlanes[SGL_MAX_SHAD_LIM_PLANES];
} SHADOW_LIM_STRUCT;

/*
// Define an array type to contain all planes for a primitive
*/
#define SGL_MAX_INTERNAL_PLANES (2*SGL_MAX_PLANES)

typedef TRANSFORMED_PLANE_STRUCT  TRANS_PLANE_ARRAY_TYPE[SGL_MAX_INTERNAL_PLANES];
typedef TRANSFORMED_PLANE_STRUCT  SHADOWS_TRANS_PLANE_ARRAY_TYPE[SGL_MAX_INTERNAL_PLANES];


/*
// Define types for the forward Visible,forward insvisible ETC, visible etc
// These are simply arrays of pointers into the transformed plane array.
*/
typedef TRANSFORMED_PLANE_STRUCT * PLANE_LISTS_TYPE[SGL_MAX_INTERNAL_PLANES];


/*
// Define a COMBINED set of lists, so that we can pass all plane
// category lists as one parameter. Doing them all as separate parameters
// appears to be a reasonable overhead.
//
// There are five categories - Forward Visible, Forward Invis, Reverse Vis,
//							   Rev Invis, and Perpendicular
*/
typedef struct
{
	int NumFV;
	int NumFI;
	int NumRV;
	int NumRI;
	int NumPE;

	PLANE_LISTS_TYPE FVPlanes;
	PLANE_LISTS_TYPE FIPlanes;
	PLANE_LISTS_TYPE RVPlanes;
	PLANE_LISTS_TYPE RIPlanes;
	PLANE_LISTS_TYPE PEPlanes;

} PLANE_CATEGORIES_STRUCT;

#endif
/*
// END OF FILE
*/
