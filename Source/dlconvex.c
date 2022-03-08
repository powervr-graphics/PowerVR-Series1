/******************************************************************************
 * Name			: dlconvex.c
 * Title		: Convex Primitive display list editing SGL functions
 * Author		: John Catchpole
 * Created		: 31/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	: SGL Routines for editing convex primitive nodes +
 *				  utility routines.
 *
 *				  Most of this is covered in TRDD027x.doc
 *
 * Platform		: ANSI
 *
 * Modifications:
 * $Log: dlconvex.c,v $
 * Revision 1.53  1997/10/16  13:02:27  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.52  1997/10/15  11:25:21  sxy
 * added some DPFDEVF messages.
 *
 * Revision 1.51  1997/09/25  14:08:37  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.50  1997/09/24  14:35:34  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.49  1997/05/13  17:50:56  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.48  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.47  1997/04/30  19:18:01  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.46  1997/04/29  14:41:47  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.45  1997/04/24  20:46:29  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.44  1997/04/02  21:26:32  mjg
 * Updated calling conventions.
 *
 * Revision 1.43  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.42  1997/01/09  16:59:11  sxy
 * moved free(pNode->plane_data); from StripNode() to DlDeleteConvexNodeRefs().
 *
 * Revision 1.41  1997/01/08  11:23:11  sxy
 * non-functional change to demonstrate RCS system to Sheila
 * Please ingore (gdc)
 *
 * Revision 1.40  1996/10/16  14:37:32  gdc
 * fiddled with shadow limiting planes again so that each
 * plane is now a display list node
 *
 * Revision 1.39  1996/10/14  13:20:49  gdc
 * added new API func 'sgl_add_shadow_limit_plane' to adds a plane
 * which truncates shadow volumes
 *
 * Revision 1.38  1996/08/29  20:31:40  jrc
 * Made sure at last that we weren't changing stored UV values (to avoid
 * affecting neighbouring faces), and shifted all UV values to be as close to
 * the origin as possible without changing the visible texture mapping (reduces
 * texture wobbling).
 * /
 *
 * Revision 1.37  1996/07/10  17:03:20  sjf
 * Hey, here we go again. Did yet another fix to the funny UVs,
 * basically modifying the UVs further. There was a slight logic
 * problem with the original that caused it to recurse when this
 * wasn't really necessary - we just needed to add on slightly
 * more to the movement amount.
 * PROBLEMS: Currently (and this has been the case for a few
 * versions) the actual UV data gets modified. This is probably
 * NOT a good idea. We need to work with local variables only.
 *
 * Revision 1.36  1996/07/10  13:43:00  jop
 * enlarged SHORT_DISTANCE to improve mech recursion
 *
 * Revision 1.35  1996/07/05  07:50:35  sjf
 * 1) Renamed error message
 * 2) Now warns for large UVs
 * 3) Removed param from map extern to intern
 * 4) Uses sgl warning
 * 5) Fixes the fix for funny UVs and generates a warning into the bargain.
 *
 * Revision 1.34  1996/06/27  11:38:26  jrc
 * Added warning handling to MapExternalToInternal.
 *
 * Revision 1.33  1996/06/26  16:18:08  sm
 * enabled determinant clipping
 *
 * Revision 1.32  1996/04/02  14:42:27  sjf
 * compiler cant handle comments at the end of #else (and possibly other)
 * lines.. Fixed it in this file.
 *
 * Revision 1.31  1996/03/14  16:11:56  jrc
 * Changed loop that deletes local materials to go to u16_num_materials instead
 * of u16_max_materials.
 *
 * Revision 1.30  1996/02/26  15:36:37  sjf
 * Changed back to correct malloc etc routines
 *
 * Revision 1.29  1996/02/23  16:25:47  sjf
 * Jim made some changes for the addition of separate points, but
 * unfortunately put ^Ms and changed all the tabs. I obsoleted that
 * file and have (hopefully) redone the edits to the previous
 * version to give this one.
 *
 *
 * Revision 1.28  1996/01/31  14:20:33  jop
 * New shading/points sytructures
 *
 * Revision 1.27  1996/01/09  11:56:53  jrc
 * 1. Removed supply_normals and supply_uv flags from sgl_add_plane and
 *    sgl_set_plane.
 * 2. Invalid NULL pointers passed to sgl_add_simple_plane, sgl_add_plane,
 *    sgl_set_simple_plane and sgl_set_plane now generate sgl_err_bad_parameter
 *    instead of failing ASSERTS.
 * 3. Changed sgl_err_no_poly to sgl_err_no_convex.
 *
 * Revision 1.26  1995/11/07  18:30:16  jop
 * More effiecient plane data copy in delete
 *
 * Revision 1.25  1995/11/02  12:42:44  pdl
 * reverted to old MapExtern...
 *
 * Revision 1.24  1995/10/26  17:23:46  pdl
 * added a temp MapExtern...
 *
 * Revision 1.23  1995/10/24  17:24:12  jrc
 * sgl_delete_plane now returns any error generated by GenerateAdjacencyInfo.
 *
 * Revision 1.22  1995/10/20  12:25:43  jrc
 * Yet another wrapping around unsigned value.
 *
 * Revision 1.21  1995/10/20  12:21:59  jrc
 * Fixed another wrap-around unsigned int.
 *
 * Revision 1.20  1995/10/20  12:09:55  jrc
 * 1. Removed redundant duplicate 'if'.
 * 2. Fixed an expression to remain in unsigned arithmetic.
 *
 * Revision 1.19  1995/10/09  16:13:06  jrc
 * For points being colinear, they are now displayed in the debug message.
 *
 * Revision 1.18  1995/10/09  13:05:44  jrc
 * 1. Changed colinearity test strictness to the same as for smooth shading
 *    precalc.
 * 2. Changed bTextureOk and bShadingOk variables in DefinePlane to
 *    bTextureMemOk and bShadingMemOk for clarity.
 * 3. Added a debug message for when DefinePlane decides that the points are
 *    colinear.
 * 4. Added sgl_err_colinear_points error.
 * 5. Fixed bugs that did not always clear the 'all planes are smooth shaded'
 *    flag when it should have been.
 *
 * Revision 1.17  1995/10/04  17:38:59  jop
 * Fixed no-uv no-texture bug
 *
 * Revision 1.16  1995/10/03  16:45:03  jop
 * Changed to 3 shading normal format in add_plane
 *
 * Revision 1.15  1995/09/19  08:59:14  sjf
 * Added light/shadow volumes and hidden convex primitives.
 *
 * Revision 1.14  1995/09/14  12:03:53  jop
 * Place shading precalculation into rnshade.c
 *
 * Revision 1.13  1995/09/13  18:21:21  jop
 * Implemented smooth shading
 *
 * Revision 1.12  1995/08/04  17:16:01  jop
 * Added warning to warn us if the texturing params look dodgy
 *
 * Revision 1.11  1995/08/03  14:56:15  jrc
 * Missed one CreatePolyhedron call that needed changing to CreateConvex.
 *
 * Revision 1.10  1995/08/03  12:57:46  jrc
 * 1) Renamed CreatePolyhedron to CreateConvex for consistency with API change.
 * 2) Fixed a bug in realloc call that happens to be cancelled out by a bug in
 *    Sun's realloc !
 *
 * Revision 1.9  1995/08/02  09:33:44  pdl
 * removed a double divide.
 *
 * Revision 1.8  1995/08/01  16:59:59  jop
 * Made MapExternalToInternal accessable to outside functions
 *
 * Revision 1.7  1995/07/30  17:46:22  sjf
 * In addition of local materials, the planes affected by the material
 * prior to the primitives definition wasnt being handled.
 *
 * Revision 1.6  1995/07/29  15:33:58  sjf
 * In a fit of depravity, rename *_polyhedron to *_convex
 *
 * Revision 1.5  1995/07/28  12:39:54  jrc
 * Fixed bug: fabs required in comparisons with minium acceptable value in
 * DefinePlane.
 *
 * Revision 1.4  1995/07/27  21:03:17  sjf
 * By default, primitives now have "insides".
 *
 * Revision 1.3  1995/07/10  11:43:45  jrc
 * Used realloc for CommonAddPlane for faster and simpler memory extension.
 *
 * Revision 1.2  1995/07/06  10:12:29  jrc
 * The full on convex primitive display list editing technology.
 *
 * Revision 1.1  1995/05/31  17:55:17  sjf
 * Initial revision
 *

 *****************************************************************************/


#define MODULE_ID MODID_DLCONVEX

#include <math.h>  /* fabs() */
#include "sgl_defs.h"
#include "sgl_init.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "dldelete.h"
#include "adjacenc.h"
#include "sgl_math.h"

#include "pvrosapi.h"
#include "sglmem.h"
#include "dlshade.h"	/* SmoothPlaneDataPrecalc() */

/*
// --------------
// MIN_CROSS_PROD
// --------------
// The minimum acceptable value for the cross product in sgl_add_plane. If this
// value is too small then it may cause incorrect behaviour in routines that
// assume the three provided points are not close to colinear.  A value that is
// too large restricts the available modelling scale range.
//
// The value below is for consistency with smooth shading code
// (SmoothPlaneDataPrecalc in rnshade.c), but 1e-30 is probably ok.
*/
#define MIN_CROSS_PROD 0.00001

/*
// ----------
// CHUNK_SIZE
// ----------
// The step size for allocation of new plane/texture/shading data items.
// u16_max_planes in the convex node should always be a non-zero multiple of
// this number.
*/
#define CHUNK_SIZE 10


/*
// ===========================================================================
// 								STATIC FUNCTIONS:
// ============================================================================
*/

/******************************************************************************
 * Function Name: CreateConvex
 *
 * Inputs       : - The type of the convex (standard etc)
 * Outputs      : -
 * Returns      : error status
 * Globals Used : dlUserGlobals.pCurrentList, dlUserGlobals.pCurrentConvex
 *
 * Description  : Creates an unnamed standard convex node.  Called by
 *				  sgl_create_polyhedron (which names the node itself if
 *				  required), and CommonAddPlane (for sgl_add_simple_plane and
 *				  sgl_add_plane).
 *****************************************************************************/
static int CreateConvex(const CONVEX_FLAGS_ENUM ConvexType)
{

    CONVEX_NODE_STRUCT *pNode;

	/*
	// Check that the parameter is valid (note the (int) is just to
	// stop a warning about unsigned >= 0)
	*/
	ASSERT( (((int)ConvexType) >= cf_standard_convex) &&
			(ConvexType <= cf_shadow_volume));

	/*
	// =========
	// MAKE NODE
	// =========
	*/
	pNode = NEW(CONVEX_NODE_STRUCT);
	if (pNode == NULL)
	{
		return sgl_err_no_mem;
	}

	pNode->node_hdr.n16_node_type = nt_convex;
	pNode->node_hdr.n16_name	  = NM_INVALID_NAME;

	pNode->u16_num_planes = 0;
	pNode->u16_max_planes = CHUNK_SIZE;

	pNode->u16_flags = ConvexType | cf_all_premapped |
	  cf_all_text_wrap | cf_all_smooth | cf_all_visible | cf_see_inside;

	pNode->edge_info = NULL;

	pNode->plane_data =
	  SGLMalloc(pNode->u16_max_planes * sizeof(CONV_PLANE_STRUCT));
	if (pNode->plane_data == NULL)
	{
		SGLFree(pNode);
		return sgl_err_no_mem;
	}

	pNode->texture_data = NULL;
	pNode->shading_data = NULL;
	pNode->points_data = NULL;
				
	pNode->u16_num_materials = 0;
				
	/*** NEEDS CHUNK OPTIMISATION: ***/
	pNode->u16_max_materials = SGL_MAX_PLANES;
				
	pNode->local_materials = NULL;
				
	/*
	// ============
	// INSTALL NODE
	// ============
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);
	dlUserGlobals.pCurrentConvex = pNode;
	return sgl_no_err;
}


/******************************************************************************
 * Function Name: CopyPlaneInfo
 *
 * Inputs       : pSrcPlane,pSrcTexture,pSrcShading,
 *				  bCopyTextures,bCopyShadings
 * Outputs      : pDestPlane,pDestTexture,pDestShading
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Copies plane related information. Called by sgl_delete_plane.
 *****************************************************************************/
static void CopyPlaneInfo(
  CONV_PLANE_STRUCT   *pDestPlane,
  CONV_TEXTURE_UNION  *pDestTexture,
  CONV_SHADING_STRUCT *pDestShading,
  CONV_POINTS_STRUCT *pDestPoints,
  CONV_PLANE_STRUCT	  *pSrcPlane,
  CONV_TEXTURE_UNION  *pSrcTexture,
  CONV_SHADING_STRUCT *pSrcShading,
  CONV_POINTS_STRUCT *pSrcPoints,
  sgl_bool			  bCopyTextures,
  sgl_bool            bCopyShadings,
  sgl_bool            bCopyPoints)
{
	ASSERT(pDestPlane	!= NULL);
	ASSERT(pSrcPlane	!= NULL);

	*pDestPlane = *pSrcPlane;

	if (bCopyTextures && (pDestPlane->flags & pf_textured))
	{
		ASSERT(pDestTexture != NULL);
		ASSERT(pSrcTexture != NULL);

		*pDestTexture = *pSrcTexture;
	}

	if (bCopyShadings && (pDestPlane->flags & pf_smooth_shad))
	{
		ASSERT(pDestShading != NULL);
		ASSERT(pSrcShading != NULL);

		*pDestShading = *pSrcShading;
	}

	if (bCopyPoints)
	{
		ASSERT(pDestPoints != NULL);
		ASSERT(pSrcPoints != NULL);

		*pDestPoints = *pSrcPoints;
	}
}


/******************************************************************************
 * Function Name: CommonAddPlane
 *
 * Inputs       : -
 * Outputs      : ppNode,ppPlane
 * Returns      : error status
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : Common setup code for sgl_add_simple_plane and sgl_add_plane.
 *****************************************************************************/
static int CommonAddPlane(CONVEX_NODE_STRUCT **ppNode,
  CONV_PLANE_STRUCT **ppPlane)
{
	CONVEX_NODE_STRUCT *pNode;

	/*
	// ==========
	// INITIALISE
	// ==========
	*/
	ASSERT(ppNode != NULL);
	ASSERT(ppPlane != NULL);

#if !WIN32
    if (SglInitialise())
	{
		return sgl_err_failed_init;
	}
#endif

	DlCompleteCurrentTransform();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	/*
	// ===============
	// GET CONVEX NODE
	// ===============
	// Create a new one if required.
	*/
	if (dlUserGlobals.pCurrentConvex == NULL)
	{
		int nError;

		nError = CreateConvex(cf_standard_convex);
		if (nError != sgl_no_err)
		{
			return nError;
		}
	}
	/*
	// dlUserGlobals.pCurrentConvex may have been changed by CreateConvex.
	*/
	*ppNode = dlUserGlobals.pCurrentConvex;  /* Caller's pointer to node. */
	pNode = *ppNode;  /* Local pointer to the node. */
	ASSERT(pNode != NULL);

	/*
	// ================
	// EXTEND PRIMITIVE
	// ================
	*/
	ASSERT(pNode->u16_num_planes <= pNode->u16_max_planes);
	if (pNode->u16_num_planes == SGL_MAX_PLANES)
	{
		return sgl_err_too_many_planes;
	}
	if (pNode->u16_num_planes == pNode->u16_max_planes)
	{
		/*
		// --------------------------
		// Extend capacity by a chunk
		// --------------------------
		*/
		sgl_uint16 u16NewMaxPlanes = pNode->u16_max_planes + CHUNK_SIZE;

		CONV_PLANE_STRUCT	*pNewPlaneData;
		CONV_TEXTURE_UNION	*pNewTextureData;
		CONV_SHADING_STRUCT *pNewShadingData;
		CONV_POINTS_STRUCT *pNewPointsData;


		pNewPlaneData = SGLRealloc( pNode->plane_data,
		  u16NewMaxPlanes * sizeof(CONV_PLANE_STRUCT) );
		if (pNewPlaneData == NULL)
		{
			return sgl_err_no_mem;  /* The plane data is unchanged. */
		}

		if (pNode->texture_data != NULL)
		{
			pNewTextureData = SGLRealloc( pNode->texture_data,
			  u16NewMaxPlanes * sizeof(CONV_TEXTURE_UNION) );
			if (pNewTextureData == NULL)
			{
				/* Restore plane data to the unexpanded size: */
				pNode->plane_data =
				  SGLRealloc(pNewPlaneData, pNode->u16_max_planes);
				ASSERT(pNode->plane_data != NULL);

				return sgl_err_no_mem;
			}
		}
		else
		{
			pNewTextureData = NULL;
		}

		if (pNode->points_data != NULL)
		{
			pNewPointsData = SGLRealloc( pNode->points_data,
						   u16NewMaxPlanes * sizeof(CONV_POINTS_STRUCT) );
			if (pNewPointsData == NULL)
			{
				/* Restore plane data to the unexpanded size: */
				pNode->plane_data =
						SGLRealloc(pNewPlaneData, pNode->u16_max_planes);
				ASSERT(pNode->plane_data != NULL);

				return sgl_err_no_mem;
			}
		}
		else
		{
			pNewPointsData = NULL;
		}

		if (pNode->shading_data != NULL)
		{
			pNewShadingData = SGLRealloc( pNode->shading_data,
			  u16NewMaxPlanes * sizeof(CONV_SHADING_STRUCT) );
			if (pNewShadingData == NULL)
			{
				/* Restore plane and texture data to the unexpanded size: */

				ASSERT(pNode->u16_max_planes > 0);  /* Previous max_planes. */

				pNode->plane_data =
				  SGLRealloc(pNewPlaneData, pNode->u16_max_planes);
				ASSERT(pNode->plane_data != NULL);

				if (pNewTextureData != NULL)
				{
					pNode->texture_data =
					  SGLRealloc(pNewTextureData, pNode->u16_max_planes);
					ASSERT(pNode->texture_data != NULL);
				}

				return sgl_err_no_mem;
			}
		}
		else
		{
			pNewShadingData = NULL;
		}

		/*
		// --------------------
		// Update node contents
		// --------------------
		// It safe to do this now since we are past all possible errors.
		*/
		pNode->u16_max_planes = u16NewMaxPlanes;

		pNode->plane_data   = pNewPlaneData;
		pNode->texture_data = pNewTextureData;
		pNode->shading_data = pNewShadingData;
		pNode->points_data = pNewPointsData;
	}
	pNode->u16_num_planes++;

	ASSERT(pNode->plane_data != NULL);
	*ppPlane = pNode->plane_data + pNode->u16_num_planes -1;

	return sgl_no_err;
}


/******************************************************************************
 * Function Name: CommonSetPlane
 *
 * Inputs       : nPlaneIndex
 * Outputs      : ppNode,ppPlane
 * Returns      : error status
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : Common setup code for sgl_set_simple_plane, sgl_set_plane and
 *				  sgl_delete_plane.
 *****************************************************************************/
static int CommonSetPlane(CONVEX_NODE_STRUCT **ppNode,
  CONV_PLANE_STRUCT **ppPlane, int nPlaneIndex)
{
	ASSERT(ppNode != NULL);
	/* ppPlane is NULL when called from sgl_delete_plane. */


#if !WIN32
    if (SglInitialise())
	{
		return sgl_err_failed_init;
	}
#endif

	DlCompleteCurrentTransform();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	if (dlUserGlobals.pCurrentConvex == NULL)
	{
		return sgl_err_no_convex;
	}

	*ppNode = dlUserGlobals.pCurrentConvex;
	ASSERT(*ppNode != NULL);

	if (nPlaneIndex < 0 || nPlaneIndex >= (*ppNode)->u16_num_planes)
	{
		return sgl_err_bad_index;
	}

	if (ppPlane != NULL)
	{
		*ppPlane = (*ppNode)->plane_data + nPlaneIndex;
		ASSERT(*ppPlane != NULL);
	}

	return sgl_no_err;
}


/******************************************************************************
 * Function Name: DefineSimplePlane
 *
 * Inputs       : pNode,pPlane,surfacePoint,normal,bInvisible
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Defines a simple plane.  Called by sgl_add_simple_plane and
 *				  sgl_set_simple_plane.
 *****************************************************************************/
static void DefineSimplePlane( CONVEX_NODE_STRUCT *pNode,
							   CONV_PLANE_STRUCT *pPlane, 
							   sgl_vector surfacePoint, 
							   sgl_vector normal,
							   sgl_bool bInvisible)
{

	sgl_bool StandardConvex;

	ASSERT(pNode != NULL);
	ASSERT(pPlane != NULL);

	/*
	// is this a standard convex primitive, or one of the special types?
	// Special types ignore invisibility, texture and smooth shading info
	*/
	StandardConvex =((pNode->u16_flags & cf_mask_type) == cf_standard_convex);

	/*
	// ignore invisible if not a standard convex
	*/
	if(!StandardConvex)
	{
		bInvisible = FALSE;
	}
	else if (bInvisible)
	{
		pNode->u16_flags &= ~cf_all_visible;
	}

	pNode->u16_flags &= ~(cf_all_premapped | cf_all_text_wrap | cf_all_smooth);

	pPlane->flags = bInvisible ? 0 : pf_visible;
	pPlane->normal[0] = normal[0];
	pPlane->normal[1] = normal[1];
	pPlane->normal[2] = normal[2];
	VecNormalise(pPlane->normal);
	pPlane->rep_point[0] = surfacePoint[0];
	pPlane->rep_point[1] = surfacePoint[1];
	pPlane->rep_point[2] = surfacePoint[2];
}


/******************************************************************************
 * Function Name: DefinePlane
 *
 * Inputs       : pNode, pPlane, surfacePoint, point2, point3, bInvisible,
 *				  normal1, normal2, normal3, uv1, uv2, uv3
 * Outputs      : -
 * Returns      : error status
 * Globals Used : -
 *
 * Description  : Defines a non-simple plane.  Called by sgl_add_plane and
 *				  sgl_set_plane.
 *****************************************************************************/
static int DefinePlane(	CONVEX_NODE_STRUCT *pNode, 
						CONV_PLANE_STRUCT *pPlane,
						sgl_vector surfacePoint, 
						sgl_vector point2, 
						sgl_vector point3,
						sgl_bool   bInvisible, 
						sgl_vector normal1,
						sgl_vector normal2,
						sgl_vector normal3, 
						sgl_2d_vec uv1, 
						sgl_2d_vec uv2,
						sgl_2d_vec uv3 )
{
	int		   nErr = sgl_no_err;
	sgl_bool   bSupplyNormals, bSupplyUV,
			     bTextureMemOk=TRUE, bShadingMemOk=TRUE;
	sgl_vector diff1,diff2, crossProd, newSurfacePoint,newPoint2,newPoint3;

	CONV_TEXTURE_UNION  *pTexture;
	CONV_SHADING_STRUCT *pShading;
	CONV_POINTS_STRUCT  *pPoints;

	ASSERT(pNode != NULL);
	ASSERT(pPlane != NULL);

	bSupplyNormals = (normal1 != NULL);
	bSupplyUV = (uv1 != NULL);

	/*
	// check whether normal information is only partly present
	*/
	if (bSupplyNormals)
	{
		if (normal2 == NULL || normal3 == NULL)
		{
			nErr = sgl_err_bad_parameter;
			bSupplyNormals = FALSE;
		}
	}
	else
	{
		if (normal2 != NULL || normal3 != NULL)
		{
			nErr = sgl_err_bad_parameter;
		}
	}

	/*
	// check whether UV information is only partly present
	*/
	if (bSupplyUV)
	{
		if (uv2 == NULL || uv3 == NULL)
		{
			nErr = sgl_err_bad_parameter;
			bSupplyUV = FALSE;
		}
	}
	else
	{
		if (uv2 != NULL || uv3 != NULL)
		{
			nErr = sgl_err_bad_parameter;
		}
	}

	/*
	// is this a standard convex primitive, or one of the special types?
	// Special types ignore invisibility, texture and smooth shading info
	*/
	if ((pNode->u16_flags & cf_mask_type) != cf_standard_convex)
	{
		bInvisible = FALSE;
		bSupplyNormals = FALSE;
		bSupplyUV = FALSE;
	}

	/*
	// ==================
	// ENSURE COLINEARITY
	// ==================
	*/
	diff1[0] = surfacePoint[0] - point2[0];
	diff1[1] = surfacePoint[1] - point2[1];
	diff1[2] = surfacePoint[2] - point2[2];

	diff2[0] = surfacePoint[0] - point3[0];
	diff2[1] = surfacePoint[1] - point3[1];
	diff2[2] = surfacePoint[2] - point3[2];

	CrossProd(diff1,diff2,crossProd);
	if (fabs(crossProd[0]) < MIN_CROSS_PROD &&
	    fabs(crossProd[1]) < MIN_CROSS_PROD &&
	    fabs(crossProd[2]) < MIN_CROSS_PROD)
	{
		DPFDEV((DBG_MESSAGE,"Colinear pts:\n(%e,%e,%e)\n(%e,%e,%e)\n(%e,%e,%e)",			 
		  surfacePoint[0], surfacePoint[1], surfacePoint[2],
		  point2[0], point2[1], point2[2],
		  point3[0], point3[1], point3[2]));

		newSurfacePoint[0] = 0.0f;
		newSurfacePoint[1] = 0.0f;
		newSurfacePoint[2] = 0.0f;

		newPoint2[0] = 1.0f;
		newPoint2[1] = 0.0f;
		newPoint2[2] = 0.0f;

		newPoint3[0] = 0.0f;
		newPoint3[1] = 1.0f;
		newPoint3[2] = 0.0f;

		/*
		// We are changing the pointers (which are now local to this function)
		// rather than the values in order that the caller's vectors are not
		// changed.
		*/
		surfacePoint = newSurfacePoint;
		point2 = newPoint2;
		point3 = newPoint3;

		crossProd[0] = 0.0f;
		crossProd[1] = 0.0f;
		crossProd[2] = 1.0f; /* Normal to the plane. */

		nErr = sgl_err_colinear_plane_points;
	}

	/*
	// ==================
	// CONTINUE NODE FILL
	// ==================
	*/
	if (bInvisible)
	{
		pNode->u16_flags &= ~cf_all_visible;
	}
	pPlane->flags = bInvisible ? 0 : pf_visible;

	VecNormalise(crossProd);
	pPlane->normal[0] = crossProd[0];
	pPlane->normal[1] = crossProd[1];
	pPlane->normal[2] = crossProd[2];

	pPlane->rep_point[0] = surfacePoint[0];
	pPlane->rep_point[1] = surfacePoint[1];
	pPlane->rep_point[2] = surfacePoint[2];

	/* if we have points, hang on to 'em */
	
	if (pNode->points_data == NULL)
	{
		pNode->points_data = SGLMalloc (pNode->u16_max_planes * 
									sizeof(CONV_POINTS_STRUCT));
 
		if (pNode->points_data == NULL)
		{
			DPF ((DBG_ERROR, "DefinePlane: no memory for points data"));
			nErr = sgl_err_no_mem;
		}
	}

	pPoints = pNode->points_data + pNode->u16_num_planes -1;

	VecCopy (surfacePoint, pPoints->pt1);
	VecSub (point2, surfacePoint, pPoints->pt2_delta);
	VecSub (point3, surfacePoint, pPoints->pt3_delta);

	/*
	// -------
	// Texture
	// -------
	*/

	if (bSupplyUV)
	{
		/***** Ensure texture information memory is set up: *****/
	
		if (pNode->texture_data == NULL)
		{
			pNode->texture_data = SGLMalloc (pNode->u16_max_planes * sizeof(CONV_TEXTURE_UNION));
			if (pNode->texture_data == NULL)
			{
				bTextureMemOk = FALSE;
				nErr = sgl_err_no_mem;
			}
		}
	
		if (bTextureMemOk)
		{
			/***** Texture flags: *****/
	
			pPlane->flags |= pf_textured;			   /* This is textured. */
			/*OPTIMISATION: USE PREMAPPING ALSO FOR UNMODIFYABLE NODE & MATERIAL*/
			pPlane->flags |= pf_pre_mapped;		   /* This is premapped. */
			pNode->u16_flags &= ~cf_all_text_wrap; /* Not all texture wrapped*/
	
			/***** Calculate texture information: *****/
	
			pTexture = pNode->texture_data + pNode->u16_num_planes -1;
			/*OPTIMISATION: USE PREMAPPING ALSO FOR UNMODIFYABLE NODE & MATERIAL*/
			MapExternalToInternal(surfacePoint,point2,point3, uv1,uv2,uv3,
			  pTexture->pre_mapped.u_vect, pTexture->pre_mapped.v_vect,
			  pTexture->pre_mapped.o_vect);
		}
		else
		{
			/* Not all texture wrapped and not all premapped: */
			pNode->u16_flags &= ~(cf_all_text_wrap | cf_all_premapped);
		}
	}
	else
	{
		/* Not all premapped. */
		pNode->u16_flags &= ~cf_all_premapped; 
	}

	/*
	// -------
	// Shading
	// -------
	*/
	if (bSupplyNormals)
	{
		/***** Ensure shading information memory is set up: *****/

		if (pNode->shading_data == NULL)
		{
			pNode->shading_data = 
			  SGLMalloc (pNode->u16_max_planes * sizeof(CONV_SHADING_STRUCT));
			
			if (pNode->shading_data == NULL)
			{
				DPF ((DBG_ERROR, "DefinePlane: no memory for shading data"));
				bShadingMemOk = FALSE;
				nErr = sgl_err_no_mem;
			}
		}

		if (bShadingMemOk)
		{
			/***** Calculate address of shading information: *****/

			pShading = pNode->shading_data + pNode->u16_num_planes -1;

			if (SmoothPlaneDataPrecalc( pShading, surfacePoint, point2, point3, 
										normal1, normal2, normal3))
			{
				/***** Shading flag: *****/

				pPlane->flags |= pf_smooth_shad;  /* This is smooth shaded. */
			}
			else
			{
				DPF ((DBG_WARNING, "DefinePlane: invalid shading data"));

				/* 
				// non fatal - plane will be flat shaded, but return an 
				// error in case anyone is interested 
				*/
				pNode->u16_flags &= ~cf_all_smooth; /* not all smooth shaded */

				/*
				// This is assuming that colinear plane points were the
				// problem.
				*/
				nErr = sgl_err_colinear_plane_points;
			}
		}
		else
		{
			pNode->u16_flags &= ~cf_all_smooth; /* not all smooth shaded */
		}
	}
	else
	{
		pNode->u16_flags &= ~cf_all_smooth; /* not all smooth shaded */
	}

	if (!bShadingMemOk)
	{
		pNode->u16_flags &= ~cf_all_smooth;  /* Not all smooth shaded. */
	}

	/*
	// ======
	// RETURN
	// ======
	*/
	return nErr;
}


/******************************************************************************
 * Function Name: StripNode
 *
 * Inputs       : pNode
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Strips all data belonging to the node.  The node is not
 *				  updated itself.  (It still says the data is present.)
 *				  Called by DlDeleteConvexNodeRefs and sgl_modify_polyhedron.
 *****************************************************************************/
static void StripNode(CONVEX_NODE_STRUCT *pNode)
{
	CONVEX_FLAGS_ENUM ConvexType;
	ASSERT(pNode != NULL);

	SGLFree(pNode->edge_info);
       
	SGLFree(pNode->texture_data);
	SGLFree(pNode->shading_data);
	SGLFree(pNode->points_data);

	/*
	// if this was a light or shadow volume node, then decrement usage
	// count on the associated light
	*/
	ConvexType = (CONVEX_FLAGS_ENUM) (pNode->u16_flags & cf_mask_type);

	if(ConvexType == cf_light_volume || ConvexType == cf_shadow_volume)
	{
		DecNamedItemUsage(dlUserGlobals.pNamtab, pNode->u16_volumes_light);
	}


	if (pNode->local_materials != NULL)
	{
		unsigned uMat;
		LOCAL_MATERIALS_STRUCT *pMatData;

		for (uMat=0; uMat < pNode->u16_num_materials; uMat++)
		{
			pMatData = pNode->local_materials + uMat;
			/* If we have a valid material name: */
			if (pMatData->material_name != NM_INVALID_NAME)
			{
				/*
				// We cannot have both a material name and a material node
				// pointer:
				*/
				ASSERT(pMatData->p_material == NULL);

				DecNamedItemUsage(dlUserGlobals.pNamtab,
				  pMatData->material_name);
			}
			else
			{
				/* If we have a valid material node pointer: */
				if (pMatData->p_material != NULL)
				{
					DlDeleteNode((DL_NODE_STRUCT *)pMatData->p_material);
				}
				else
				{
					/*
					// We can have a blank local material only for the first
					// local material in the list.
					*/
					ASSERT(uMat == 0);
				}
			}
		}
		SGLFree(pNode->local_materials);
	}
}


/*
// ============================================================================
// 							  SGL INTERNAL ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: MapExternalToInternal
 *
 * Inputs       : P0,P1,P2: The three 3D coordinates.
 *				  UV0,UV1,UV2: The u,v coordinates at the above positions.
 * Outputs      : U,V,O, pnWarning
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Maps from 3 object coordinates and 3 u,v's to the 9 texture
 *				  parameters ( U,V and O vectors).  Called by DefinePlane.
 *				  _        _    _   _   _    _    _  _   _    _    _
 *				  P0= O+ u0U+ v0V   P1= O+ u1U+ v1V  P2= O+ u2U+ v2V
 *				  _   _    _   _   _    _
 *				  Q0= P0 - P1  Q1= P0 - P2
 *				  _    _   _   _    _   _
 *				  Q0= aU+ bV   Q1= cU+ dV
 *
 *						-1        1
 *				  |a  b|      --------- | d -b|
 *				  |c  d|    =  ad - bc  |-c  a|
 *
 *						-1  _     _
 *				  |a  b|   |Q0|  |U|
 *				  |    | * |_ | =|_|
 *				  |c  d|   |Q1|  |V|
 *
 *				  If pnWarning is non-zero then any warning that arises is
 *				  stored therein.
 *****************************************************************************/


#if 0  


/*
// This version does not check for large UV values and does not modify
// colinear UV combinations.
*/
void MapExternalToInternal(sgl_vector P0, sgl_vector P1, sgl_vector P2,
  sgl_2d_vec UV0, sgl_2d_vec UV1, sgl_2d_vec UV2, sgl_vector U, sgl_vector V,
  sgl_vector O)
{
	sgl_vector Q0,Q1;
	float a,b,c,d;        /*2d matrix to invert */
	float ai,bi,ci,di;    /*inverted 2d matrix  */
	float det;

	DPF(( DBG_VERBOSE, "P0 =%f %f %f", P0[0],P0[1],P0[2] ));
	DPF(( DBG_VERBOSE, "P1 =%f %f %f", P1[0],P1[1],P1[2] ));
	DPF(( DBG_VERBOSE, "P2 =%f %f %f", P2[0],P2[1],P2[2] ));

	DPF(( DBG_VERBOSE, "UV0 =%f %f", UV0[0],UV0[1] ));
	DPF(( DBG_VERBOSE, "UV1 =%f %f", UV1[0],UV1[1] ));
	DPF(( DBG_VERBOSE, "UV2 =%f %f", UV2[0],UV2[1] ));

	Q0[0]=P0[0]-P1[0];
	Q0[1]=P0[1]-P1[1];
	Q0[2]=P0[2]-P1[2];

	Q1[0]=P0[0]-P2[0];
	Q1[1]=P0[1]-P2[1];
	Q1[2]=P0[2]-P2[2];

	a=UV0[0]-UV1[0];
	b=UV0[1]-UV1[1];
	c=UV0[0]-UV2[0];
	d=UV0[1]-UV2[1];

	ai=d;    /*transpose matrix*/
	bi=-b;
	ci=-c;
	di=a;

	det=a*d - b*c; /*calculate determinant*/

	DPF(( DBG_VERBOSE, "det=%e", det ));

	if(fabs(det)<1.0e-4)    /*clip determinant*/
	{
		DPF ((DBG_WARNING, "MapExternalToInternal: determinant is very small (%f); clipping", det));
	    det=1.0e-4;

		SglWarning(sgl_warn_colinear_uvs);
	}

	det=1.0f/det;

	DPF(( DBG_VERBOSE, "det=%e", det ));

	ai*=det;
	bi*=det;
	ci*=det;
	di*=det;

	/*
	** multiply the inverse by Q0 and Q1
	*/

	U[0]=ai*Q0[0];  /*calculate U*/
	U[1]=ai*Q0[1]; 
	U[2]=ai*Q0[2]; 

	U[0]+=bi*Q1[0];
	U[1]+=bi*Q1[1]; 
	U[2]+=bi*Q1[2]; 

	V[0]=ci*Q0[0];  /*calculate V*/
	V[1]=ci*Q0[1]; 
	V[2]=ci*Q0[2]; 

	V[0]+=di*Q1[0];
	V[1]+=di*Q1[1]; 
	V[2]+=di*Q1[2]; 

	/*
	** now calculate the origin using the first set of coeff's. 
	*/

	O[0]=UV0[0]*U[0] + UV0[1]*V[0];
	O[1]=UV0[0]*U[1] + UV0[1]*V[1];
	O[2]=UV0[0]*U[2] + UV0[1]*V[2];

	O[0]=P0[0]-O[0];
	O[1]=P0[1]-O[1];
	O[2]=P0[2]-O[2];


#if DEBUG
	/* now output the coefficients */

	DPF(( DBG_VERBOSE, "U[]=%f %f %f", U[0],U[1],U[2] ));
	DPF(( DBG_VERBOSE, "V[]=%f %f %f", V[0],V[1],V[2] ));
	DPF(( DBG_VERBOSE, "O[]=%f %f %f", O[0],O[1],O[2] ));

	/*test that my code is working*/

	if(fabs((O[0]+UV0[0]*U[0]+UV0[1]*V[0])-P0[0])>1e-4 ||
	   fabs((O[1]+UV0[0]*U[1]+UV0[1]*V[1])-P0[1])>1e-4 ||
	   fabs((O[2]+UV0[0]*U[2]+UV0[1]*V[2])-P0[2])>1e-4 )
	   DPF(( DBG_VERBOSE, "P0 does not match!!" ));
	   
	if(fabs((O[0]+UV1[0]*U[0]+UV1[1]*V[0])-P1[0])>1e-4 ||
	   fabs((O[1]+UV1[0]*U[1]+UV1[1]*V[1])-P1[1])>1e-4 ||
	   fabs((O[2]+UV1[0]*U[2]+UV1[1]*V[2])-P1[2])>1e-4 )
	   DPF(( DBG_VERBOSE, "P1 does not match!!" ));

	if(fabs((O[0]+UV2[0]*U[0]+UV2[1]*V[0])-P2[0])>1e-4 ||
	   fabs((O[1]+UV2[0]*U[1]+UV2[1]*V[1])-P2[1])>1e-4 ||
	   fabs((O[2]+UV2[0]*U[2]+UV2[1]*V[2])-P2[2])>1e-4 )
	   DPF(( DBG_VERBOSE, "P2 does not match!!" ));

	DPF(( DBG_VERBOSE, "P0= %f %f %f", O[0]+UV0[0]*U[0]+UV0[1]*V[0],
								  	   O[1]+UV0[0]*U[1]+UV0[1]*V[1],
									   O[2]+UV0[0]*U[2]+UV0[1]*V[2] ));

	DPF(( DBG_VERBOSE, "P1= %f %f %f", O[0]+UV1[0]*U[0]+UV1[1]*V[0],
								  	   O[1]+UV1[0]*U[1]+UV1[1]*V[1],
									   O[2]+UV1[0]*U[2]+UV1[1]*V[2] ));

	DPF(( DBG_VERBOSE, "P2= %f %f %f", O[0]+UV2[0]*U[0]+UV2[1]*V[0],
								  	   O[1]+UV2[0]*U[1]+UV2[1]*V[1],
									   O[2]+UV2[0]*U[2]+UV2[1]*V[2] ));
#endif /* #if DEBUG */
}


#else


/*
// This version warns about large UV values and slightly modifies colinear UV
// values so that the determinant does not get too close to zero.  It also
// shifts UVs to be as close to the origin as possible, without changing the
// visible texture mapping (reduces wobbling).
*/
#define SMALL_DETERMINANT 		 (1.0E-6f)
#define SHORT_DISTANCE_TWEEN_UVS (1.0e-3f)

void MapExternalToInternal(sgl_vector P0, sgl_vector P1, sgl_vector P2,
  sgl_2d_vec UV0, sgl_2d_vec UV1, sgl_2d_vec UV2, sgl_vector U, sgl_vector V,
  sgl_vector O)
{
	sgl_vector Q0,Q1;
	float a,b,c,d;        /*2d matrix to invert */
	float ai,bi,ci,di;    /*inverted 2d matrix  */
	float det;

	sgl_2d_vec localUV0, localUV1, localUV2;

	#if DEBUG
	static sgl_bool  bRecursing = FALSE;
	#endif

	DPF(( DBG_VERBOSE, "P0 =%f %f %f", P0[0],P0[1],P0[2] ));
	DPF(( DBG_VERBOSE, "P1 =%f %f %f", P1[0],P1[1],P1[2] ));
	DPF(( DBG_VERBOSE, "P2 =%f %f %f", P2[0],P2[1],P2[2] ));

	DPF(( DBG_VERBOSE, "UV0 =%f %f", UV0[0],UV0[1] ));
	DPF(( DBG_VERBOSE, "UV1 =%f %f", UV1[0],UV1[1] ));
	DPF(( DBG_VERBOSE, "UV2 =%f %f", UV2[0],UV2[1] ));

	/*
	// Warn about UV values that are greater in magnitude than LARGE_UV:
	*/
	#define LARGE_UV  100.0f

	if ((fabs(UV0[0]) >= LARGE_UV || fabs(UV0[1]) >= LARGE_UV ||
	     fabs(UV1[0]) >= LARGE_UV || fabs(UV1[1]) >= LARGE_UV ||
	     fabs(UV2[0]) >= LARGE_UV || fabs(UV2[1]) >= LARGE_UV))
	{
		SglWarning(sgl_warn_large_uvs);
	}
	
	#undef LARGE_UV

	/*
	// Make local copies of the UV values so that original ones are not
	// changed, which would affect neighbouring faces.
	*/
	localUV0[0] = UV0[0];
	localUV1[0] = UV1[0];
	localUV2[0] = UV2[0];

	localUV0[1] = UV0[1];
	localUV1[1] = UV1[1];
	localUV2[1] = UV2[1];

	/*
	// Improves modelling where UVs are shifted a long way from zero, causing
	// texture wobbling.  Should probably do a warning message for this.
	*/
	{
		/*
		// Average distance of Us and Vs from the origin.
		*/
		float fAverageU = (localUV0[0]+localUV1[0]+localUV2[0]) / 3.0f,
			  fAverageV = (localUV0[1]+localUV1[1]+localUV2[1]) / 3.0f;

		/*
		// The shift must be a multiple of 2 to allow for UV flipping.
		*/
		int nShiftU = ((int)fAverageU / 2) *2,
			nShiftV = ((int)fAverageV / 2) *2;

	 	/*
		// The 'if' is a slight speedup perhaps, and useful for setting a
		// breakpoint when a shift occurs.
		*/
		if (nShiftU || nShiftV)
		{
			localUV0[0] -= (float)nShiftU;
			localUV1[0] -= (float)nShiftU;
			localUV2[0] -= (float)nShiftU;

			localUV0[1] -= (float)nShiftV;
			localUV1[1] -= (float)nShiftV;
			localUV2[1] -= (float)nShiftV;
		}

	} /* UV shifting local variables */

	Q0[0]=P0[0]-P1[0];
	Q0[1]=P0[1]-P1[1];
	Q0[2]=P0[2]-P1[2];

	Q1[0]=P0[0]-P2[0];
	Q1[1]=P0[1]-P2[1];
	Q1[2]=P0[2]-P2[2];

	a=localUV0[0]-localUV1[0];
	b=localUV0[1]-localUV1[1];
	c=localUV0[0]-localUV2[0];
	d=localUV0[1]-localUV2[1];

	ai=d;    /*transpose matrix*/
	bi=-b;
	ci=-c;
	di=a;

	det=a*d - b*c; /*calculate determinant*/

	DPF(( DBG_VERBOSE, "det=%e", det ));

	/*
	// We need to ensure that the determinant does not get too small.  This
	// happens if the UV points are colinear or almost colinear in UV space.
	// In these cases one or more of the UVs are modified slightly.
	*/
	if (fabs(det) < SMALL_DETERMINANT)
	{
		/*
		// fShort is the minimum distance allowed between UV points and the
		// distance UV values are moved by to keep them apart.
		//
		// If the UV components are far away from each other then fShort is
		// scaled accordingly.
		*/
		float  fShort;

		DPFDEV((DBG_WARNING,
		  "MapExternalToInternal: UV colinear, determinant is very small (%f); clipping",
		  det));

		SglWarning(sgl_warn_colinear_uvs);

		/*
		// Add the absolute distances between UV points, and then use
		// a scaling factor. We don't allow there to be too huge a 
		// difference between the UV coordinates.
		*/
		fShort = SHORT_DISTANCE_TWEEN_UVS * (
		  sfabs(a) + sfabs(b) +
		  sfabs(c) + sfabs(d) );

		/*
		// If they are all too close together, then just set the "min".
		*/
		if (fShort < SHORT_DISTANCE_TWEEN_UVS)
		{
			fShort = SHORT_DISTANCE_TWEEN_UVS;
		}


		/*
		// If the first and second points are too close:
		*/
		if (sfabs(a) + sfabs(b) < fShort)
		{
			/*
			// If the second and third points are too close (and so all three
			// points are too close):
			*/
			if (sfabs(localUV1[0] - localUV2[0]) +
			  sfabs(localUV1[1] - localUV2[1]) < fShort)
			{
				/*
				// Just move the second and third points away from the first
				// point, ensuring they are not colinear:
				// Note: we add 2x the fshort value to guarantee the points
				// are at least fShort apart.
				*/
				localUV1[0] += 2.0f * fShort;
				localUV2[1] += 2.0f * fShort;
			}
			else
			{
				/*
				// The last two UV points are different, so move the first
				// point slightly to one side of the line joining the other
				// two.
				//
				// This movement is less than 1/100 of a texel, and is parallel
				// to either the U or V axis, whichever is more perpendicular
				// to the line joining the original points.
				*/
				if (sfabs(localUV1[0] - localUV2[0]) <
				  sfabs(localUV1[1] - localUV2[1]))
				{
					localUV0[0] += 2.0f * fShort;
				}
				else
				{
					localUV0[1] += 2.0f * fShort;
				}
			}

		} /* if the first two points are too close */

		else

		{
			/*
			// The first two UVs are different, so move the the last one
			// slightly, as above.
			*/
			if (sfabs(a) < sfabs(b))
			{
				localUV2[0] += 2.0f * fShort;
			}
			else
			{
				localUV2[1] += 2.0f * fShort;
			}
		}

		/*
		// Check everything again to be absolutely sure the UVs are not too
		// colinear.
		//
		// In debug mode ensure we do not recurse multiple times: that would be
		// inefficient and mean that we are not moving the UVs sufficiently.
		*/
		#if DEBUG
		ASSERT(bRecursing == FALSE);
		bRecursing = TRUE;
		#endif
		MapExternalToInternal(P0,P1,P2, localUV0,localUV1,localUV2, U,V,O);

		return;

	} /*if (fabs(det) < SMALL_DETERMINANT) */

	/*
	// We have finished any recursing, so reset the flag for the next external
	// call to this function.
	*/
	#if DEBUG
	bRecursing = FALSE;
	#endif

	det=1.0f/det;

	DPF(( DBG_VERBOSE, "det=%e", det ));

	ai*=det;
	bi*=det;
	ci*=det;
	di*=det;

	/*
	** multiply the inverse by Q0 and Q1
	*/

	U[0]=ai*Q0[0];  /*calculate U*/
	U[1]=ai*Q0[1]; 
	U[2]=ai*Q0[2]; 

	U[0]+=bi*Q1[0];
	U[1]+=bi*Q1[1]; 
	U[2]+=bi*Q1[2]; 

	V[0]=ci*Q0[0];  /*calculate V*/
	V[1]=ci*Q0[1]; 
	V[2]=ci*Q0[2]; 

	V[0]+=di*Q1[0];
	V[1]+=di*Q1[1]; 
	V[2]+=di*Q1[2]; 

	/*
	** now calculate the origin using the first set of coeff's. 
	*/

	O[0]=localUV0[0]*U[0] + localUV0[1]*V[0];
	O[1]=localUV0[0]*U[1] + localUV0[1]*V[1];
	O[2]=localUV0[0]*U[2] + localUV0[1]*V[2];

	O[0]=P0[0]-O[0];
	O[1]=P0[1]-O[1];
	O[2]=P0[2]-O[2];


#if DEBUG
	/* now output the coefficients */

	DPF(( DBG_VERBOSE, "U[]=%f %f %f", U[0],U[1],U[2] ));
	DPF(( DBG_VERBOSE, "V[]=%f %f %f", V[0],V[1],V[2] ));
	DPF(( DBG_VERBOSE, "O[]=%f %f %f", O[0],O[1],O[2] ));

	/*test that my code is working*/

	if(fabs((O[0]+localUV0[0]*U[0]+localUV0[1]*V[0])-P0[0])>1e-4 ||
	   fabs((O[1]+localUV0[0]*U[1]+localUV0[1]*V[1])-P0[1])>1e-4 ||
	   fabs((O[2]+localUV0[0]*U[2]+localUV0[1]*V[2])-P0[2])>1e-4 )
	   DPF(( DBG_VERBOSE, "P0 does not match!!" ));
	   
	if(fabs((O[0]+localUV1[0]*U[0]+localUV1[1]*V[0])-P1[0])>1e-4 ||
	   fabs((O[1]+localUV1[0]*U[1]+localUV1[1]*V[1])-P1[1])>1e-4 ||
	   fabs((O[2]+localUV1[0]*U[2]+localUV1[1]*V[2])-P1[2])>1e-4 )
	   DPF(( DBG_VERBOSE, "P1 does not match!!" ));

	if(fabs((O[0]+localUV2[0]*U[0]+localUV2[1]*V[0])-P2[0])>1e-4 ||
	   fabs((O[1]+localUV2[0]*U[1]+localUV2[1]*V[1])-P2[1])>1e-4 ||
	   fabs((O[2]+localUV2[0]*U[2]+localUV2[1]*V[2])-P2[2])>1e-4 )
	   DPF(( DBG_VERBOSE, "P2 does not match!!" ));

	DPF(( DBG_VERBOSE, "P0= %f %f %f", O[0]+localUV0[0]*U[0]+localUV0[1]*V[0],
								  	   O[1]+localUV0[0]*U[1]+localUV0[1]*V[1],
									   O[2]+localUV0[0]*U[2]+localUV0[1]*V[2] ));

	DPF(( DBG_VERBOSE, "P1= %f %f %f", O[0]+localUV1[0]*U[0]+localUV1[1]*V[0],
								  	   O[1]+localUV1[0]*U[1]+localUV1[1]*V[1],
									   O[2]+localUV1[0]*U[2]+localUV1[1]*V[2] ));

	DPF(( DBG_VERBOSE, "P2= %f %f %f", O[0]+localUV2[0]*U[0]+localUV2[1]*V[0],
								  	   O[1]+localUV2[0]*U[1]+localUV2[1]*V[1],
									   O[2]+localUV2[0]*U[2]+localUV2[1]*V[2] ));
#endif /* #if DEBUG */
}


#endif


/******************************************************************************
 * Function Name: DlCompleteCurrentConvex
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : Does any tidying up to complete the convex node.
 *				  This requires creating adjacency information, bounding box
 *				  etc for the primitive.
 *****************************************************************************/
void DlCompleteCurrentConvex(void)
{
	CONVEX_NODE_STRUCT *pNode = dlUserGlobals.pCurrentConvex;

	if (pNode != NULL)
	{
		/*
		// ==========================
		// CALCULATE EDGE INFORMATION
		// ==========================
		// NOTE: THIS MAY FAIL (returning sgl_err_no_mem).  THIS IS A SITUATION
		// THAT IS NOT CORRECTLY HANDLED, AND IS A DREADED BUG!
		*/
		GenerateAdjacencyInfo(pNode);

		/*
		// ========================
		// COMPLETE LOCAL MATERIALS
		// ========================
		*/
		if (pNode->u16_num_materials)
		{
			int nMat, nPlaneCount=0;
			LOCAL_MATERIALS_STRUCT *pFinalMaterialData =
			  pNode->local_materials + pNode->u16_num_materials -1;

			ASSERT(pNode->local_materials != NULL);

			/*
			// -------------------------------------------------
			// Calculate planes affected by final local material
			// -------------------------------------------------
			*/
			for (nMat=0; nMat < pNode->u16_num_materials-1; nMat++)
			{
				nPlaneCount +=
				  pNode->local_materials[nMat].planes_affected;
			}

			pFinalMaterialData->planes_affected =
			  pNode->u16_num_planes - nPlaneCount;
			ASSERT(pFinalMaterialData->planes_affected >= 0);
		}
		else
		{
			ASSERT(pNode->local_materials == NULL);
		}

		dlUserGlobals.pCurrentConvex = NULL;
	}
}


/******************************************************************************
 * Function Name: DlConvexAddLocalMaterial
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : error status
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : Adds a local material to the current convex primitive node.
 *****************************************************************************/
int DlConvexAddLocalMaterial(MATERIAL_NODE_STRUCT *pMaterialNode)
{
	CONVEX_NODE_STRUCT     *pConvexNode = dlUserGlobals.pCurrentConvex;
	LOCAL_MATERIALS_STRUCT *pMaterialData;

	ASSERT(pMaterialNode != NULL);
	ASSERT(pConvexNode != NULL);


	/*
	// NOTE that there is no point in adding a local material to anything
	// but a standard convex, but I don't really see what options are open
	// to us... I guess we could simply delete the local material passed
	// to this routine PROVIDED it doesn't have a name... Perhaps we 
	// could implement it later. At the moment just let it occupy space
	// (besides its the users fault 8-) )
	*/
	
	/*
	// ==========================================
	// ALLOCATE ANY NEW LOCAL DATA SPACE REQUIRED
	// ==========================================
	*/
	/* OPTIMISATION: DO ALLOCATION IN CHUNKS RATHER THAN ALL OR NOTHING. */
	ASSERT(pConvexNode->u16_max_materials == SGL_MAX_PLANES)
	if (pConvexNode->u16_num_materials != 0)
	{
		ASSERT(pConvexNode->local_materials != NULL);
	}
	else
	{
		ASSERT(pConvexNode->local_materials == NULL);
		pConvexNode->local_materials = SGLMalloc(sizeof(LOCAL_MATERIALS_STRUCT) *
		  pConvexNode->u16_max_materials);
		if (pConvexNode->local_materials == NULL)
		{
			return sgl_err_no_mem;
		}

		/*
		// Test to see if any planes have already been added. If so
		// then they are affected by the previous material state. This
		// is indicated by storing a NULL ptr in the first material
		// entry
		*/
		if(pConvexNode->u16_num_planes != 0)
		{
			
			/*
			// add one local material, and set its planes affected, and pointer
			*/
			pMaterialData = pConvexNode->local_materials;

			pMaterialData->planes_affected = pConvexNode->u16_num_planes;
			pMaterialData->material_name   = NM_INVALID_NAME;
			pMaterialData->p_material      = NULL;
			
			pConvexNode->u16_num_materials=1;
		}
	}

	/*
	// =========================================
	// ADD NEW LOCAL MATERIAL TO THE CONVEX NODE
	// =========================================
	*/
	ASSERT(pConvexNode->u16_num_materials < pConvexNode->u16_max_materials);
	pConvexNode->u16_num_materials++;

	pMaterialData =
	  pConvexNode->local_materials + (pConvexNode->u16_num_materials -1);

	pMaterialData->planes_affected = 0;
	pMaterialData->material_name   = NM_INVALID_NAME;
	pMaterialData->p_material      = pMaterialNode;

	/*
	// ====================================================
	// CALCULATE PLANES AFFECTED BY PREVIOUS LOCAL MATERIAL
	// (Note last one will be done when primitive is completed)
	// ====================================================
	*/
	if (pConvexNode->u16_num_materials >= 2)
	{
		/*
		// OPTIMISATION: REMOVE THE PREVIOUS LOCAL MATERIAL (SEE STRIPNODE
		// ABOVE) IF IT IS NOT USED BY ANY PLANES.
		*/
		int nMat, nPlaneCount=0;
		LOCAL_MATERIALS_STRUCT *pPrevMaterialData = pMaterialData-1;

		for (nMat=0; nMat < pConvexNode->u16_num_materials-2; nMat++)
			nPlaneCount += pConvexNode->local_materials[nMat].planes_affected;

		pPrevMaterialData->planes_affected =
		  pConvexNode->u16_num_planes - nPlaneCount;
		ASSERT(pPrevMaterialData->planes_affected >= 0);
	}

	return sgl_no_err;
}


/******************************************************************************
 * Function Name: DlDeleteConvexNodeRefs
 *
 * Inputs       : pNodeHdr
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Cleanup processing before deletion of a convex primitive
 *				  display list node.
 *****************************************************************************/
void DlDeleteConvexNodeRefs(DL_NODE_STRUCT *pNodeHdr)
{
        CONVEX_NODE_STRUCT *pNode;

	ASSERT(pNodeHdr != NULL);
	ASSERT(pNodeHdr->n16_node_type == nt_convex);

        pNode = (CONVEX_NODE_STRUCT *)pNodeHdr;
	
        StripNode(pNode);

        SGLFree(pNode->plane_data);
}


/*
// ============================================================================
// 								SGL API ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: sgl_create_convex
 *
 * Inputs       : bGenerateName
 * Outputs      : -
 * Returns      : name or error
 * Globals Used : dlUserGlobals.pNamtab, dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_create_convex( sgl_bool bGenerateName )
{

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
	else
#endif
	{
		int nError;

		DlCompleteCurrentTransform();
		DlCompleteCurrentConvex();
		DlCompleteCurrentMaterial();
		DlCompleteCurrentMesh();

		nError = CreateConvex(cf_standard_convex);

		if (bGenerateName && nError == sgl_no_err)
		{
			int nName;

			ASSERT(dlUserGlobals.pCurrentConvex != NULL);

			nName = AddNamedItem(dlUserGlobals.pNamtab,
			  dlUserGlobals.pCurrentConvex, nt_convex);

			dlUserGlobals.pCurrentConvex->node_hdr.n16_name =
			  nName == sgl_err_no_name ? NM_INVALID_NAME : (sgl_int16)nName;
			return SglError(nName);
		}
		else
		{
			return SglError(nError);
		}
	}
}


/******************************************************************************
 * Function Name: sgl_modify_convex
 *
 * Inputs       : nName,bClearPolyhedron
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab, dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_modify_convex( int nName, sgl_bool bClearPolyhedron )
{
	/*
	// ==========
	// INITIALISE
	// ==========
	*/
    CONVEX_NODE_STRUCT *pNode;

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_convex)
	{
		DPFDEV((DBG_WARNING, "sgl_modify_convex: calling with bad name"));
		SglError(sgl_err_bad_name);
		return;
	}

	/*
	// =================
	// SELECT POLYHEDRON
	// =================
	*/
	pNode = GetNamedItem(dlUserGlobals.pNamtab, nName);
	ASSERT(pNode != NULL);

	dlUserGlobals.pCurrentConvex = pNode;

	/*
	// ================
	// CLEAR POLYHEDRON
	// ================
	*/
	if (bClearPolyhedron)
	{
		StripNode(pNode);

		pNode->u16_num_planes = 0;
		/*
		// We are not bothering to reduce u16_max_planes and the corresponding
		// data structure sizes.
		*/

		pNode->u16_flags = cf_all_premapped | cf_all_text_wrap |
		  cf_all_smooth | cf_all_visible;

		pNode->edge_info = NULL;
		pNode->texture_data = NULL;
		pNode->shading_data = NULL;
		pNode->points_data = NULL;

		if (pNode->u16_num_materials)
		{
			ASSERT(pNode->local_materials != NULL);
			pNode->u16_num_materials = 0;

			/*** NEEDS CHUNK OPTIMISATION: ***/
			/* When we do optimisation u16_max_materials may need reducing.*/

			pNode->local_materials = NULL;
		}
		else
		{
			ASSERT(pNode->local_materials == NULL);
		}
	}

	/*
	// ======
	// RETURN
	// ======
	*/
	SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_add_simple_plane
 *
 * Inputs       : surfacePoint,normal,bInvisible
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_add_simple_plane( sgl_vector surfacePoint, 
									 sgl_vector normal,
									 sgl_bool bInvisible)
{
	int nError;
	CONVEX_NODE_STRUCT *pNode;
	CONV_PLANE_STRUCT  *pPlane;

	if (surfacePoint == NULL || normal == NULL)
	{
		DPFDEV((DBG_WARNING, "sgl_add_simple_plane: calling with bad parameters"));	
		SglError(sgl_err_bad_parameter);
		return;
	}

	nError = CommonAddPlane(&pNode,&pPlane);
	if (nError != sgl_no_err)
	{
		SglError(nError);
		return;
	}

	DefineSimplePlane(pNode,pPlane, surfacePoint,normal,bInvisible);
	SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_add_shadow_limit_plane
 *
 * Inputs       : surfacePoint,normal
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/

void CALL_CONV sgl_add_shadow_limit_plane( sgl_vector surfacePoint, 
										   sgl_vector normal )
{
    SHAD_LIM_NODE_STRUCT *pNode;
	CONV_PLANE_STRUCT  *pPlane;

#if !WIN32
    if (SglInitialise())
	{
	  SglError(sgl_err_failed_init);
	  return;
	}
#endif

	/*
	// =========
	// MAKE NODE
	// =========
	*/
	pNode = NEW(SHAD_LIM_NODE_STRUCT);
	if (pNode == NULL)
	  {
		SglError(sgl_err_no_mem);
		return;

	  }

	pNode->node_hdr.n16_node_type = nt_shadow_limit;
	pNode->node_hdr.n16_name	  = NM_INVALID_NAME;
	pNode->node_hdr.next_node	  = NULL;

	/*
	// ============
	// INSTALL NODE
	// ============
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);


	/* add data into plane space in node struct */
	pPlane = &(pNode->plane_data);

	if (surfacePoint == NULL || normal == NULL)
	{
		DPFDEV((DBG_WARNING, "sgl_add_shadow_limit_plane: calling with bad parameters"));	
		SglError(sgl_err_bad_parameter);
		return;
	}

	pPlane->normal[0] = normal[0];
	pPlane->normal[1] = normal[1];
	pPlane->normal[2] = normal[2];
	VecNormalise(pPlane->normal);
	pPlane->rep_point[0] = surfacePoint[0];
	pPlane->rep_point[1] = surfacePoint[1];
	pPlane->rep_point[2] = surfacePoint[2];

	pPlane->flags = pf_shadow_plane;

	SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_add_plane
 *
 * Inputs       : surfacePoint, point2, point3, bInvisible, normal2, normal3,
 *				  uv1, uv2, uv3
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_add_plane( sgl_vector surfacePoint,
							  sgl_vector point2, sgl_vector point3,
		 					  sgl_bool   bInvisible,
							  sgl_vector normal1,
							  sgl_vector normal2,
							  sgl_vector normal3,
							  sgl_2d_vec uv1, sgl_2d_vec uv2, sgl_2d_vec uv3 )
{
	int nError;

	CONVEX_NODE_STRUCT *pNode;
	CONV_PLANE_STRUCT  *pPlane;

	/*
	// check whether point information is present
	*/
	if (surfacePoint == NULL || point2 == NULL || point3 == NULL)
	{
		DPFDEV((DBG_WARNING, "sgl_add_plane: calling with bad parameters"));	
		SglError(sgl_err_bad_parameter);
		return;
	}

	/*
	// check whether normal information is only partly present
	*/
	if (normal1 == NULL)
	{
		if (normal2 != NULL || normal3 != NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_add_plane: calling with bad normals"));		
			SglError(sgl_err_bad_parameter);
			return;
		}
	}
	else
	{
		if (normal2 == NULL || normal3 == NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_add_plane: calling with bad normals"));				
			SglError(sgl_err_bad_parameter);
			return;
		}
	}

	/*
	// check whether UV information is only partly present
	*/
	if (uv1 == NULL)
	{
		if (uv2 != NULL || uv3 != NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_add_plane: calling with bad UVs"));		
			SglError(sgl_err_bad_parameter);
			return;
		}
	}
	else
	{
		if (uv2 == NULL || uv3 == NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_add_plane: calling with bad UVs"));			
			SglError(sgl_err_bad_parameter);
			return;
		}
	}

	nError = CommonAddPlane(&pNode,&pPlane);
	if (nError != sgl_no_err)
	{
		SglError(nError);
		return;
	}

	SglError(DefinePlane( pNode,pPlane, surfacePoint,point2,point3, bInvisible,
	  normal1,normal2,normal3, uv1,uv2,uv3 ));
}


/******************************************************************************
 * Function Name: sgl_set_simple_plane
 *
 * Inputs       : nPlaneIndex,surfacePoint,normal,bInvisible
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_simple_plane( int nPlaneIndex, sgl_vector surfacePoint,
									 sgl_vector normal, sgl_bool bInvisible )
{
	int nError;

	CONVEX_NODE_STRUCT *pNode;
	CONV_PLANE_STRUCT  *pPlane;

	if (surfacePoint == NULL || normal == NULL)
	{
		DPFDEV((DBG_WARNING, "sgl_set_simple_plane: calling with bad parameters"));	
		SglError(sgl_err_bad_parameter);
		return;
	}

	nError = CommonSetPlane(&pNode,&pPlane, nPlaneIndex);
	if (nError != sgl_no_err)
	{
		SglError(nError);
		return;
	}

	DefineSimplePlane(pNode,pPlane, surfacePoint,normal,bInvisible);
	SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_set_plane
 *
 * Inputs       : nPlaneIndex, surfacePoint, point2, point3, bInvisible,
 *				  normal1, normal2, normal3, uv1, uv2, uv3
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_plane( int nPlaneIndex,
							  sgl_vector surfacePoint,
							  sgl_vector point2, sgl_vector point3,
							  sgl_bool   bInvisible,
							  sgl_vector normal1,
 							  sgl_vector normal2, sgl_vector normal3,
							  sgl_2d_vec uv1, sgl_2d_vec uv2, sgl_2d_vec uv3 )
{
	int nError;

	CONVEX_NODE_STRUCT *pNode;
	CONV_PLANE_STRUCT  *pPlane;

	/*
	// check whether point information is present
	*/
	if (surfacePoint == NULL || point2 == NULL || point3 == NULL)
	{
		DPFDEV((DBG_WARNING, "sgl_set_plane: calling with bad parameters"));		
		SglError(sgl_err_bad_parameter);
		return;
	}

	/*
	// check whether normal information is only partly present
	*/
	if (normal1 == NULL)
	{
		if (normal2 != NULL || normal3 != NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_set_plane: calling with bad normals"));			
			SglError(sgl_err_bad_parameter);
			return;
		}
	}
	else
	{
		if (normal2 == NULL || normal3 == NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_set_plane: calling with bad normals"));		
			SglError(sgl_err_bad_parameter);
			return;
		}
	}

	/*
	// check whether UV information is only partly present
	*/
	if (uv1 == NULL)
	{
		if (uv2 != NULL || uv3 != NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_set_plane: calling with bad UVs"));			
			SglError(sgl_err_bad_parameter);
			return;
		}
	}
	else
	{
		if (uv2 == NULL || uv3 == NULL)
		{
			DPFDEV((DBG_WARNING, "sgl_set_plane: calling with bad UVs"));			
			SglError(sgl_err_bad_parameter);
			return;
		}
	}

	nError = CommonSetPlane(&pNode,&pPlane, nPlaneIndex);
	if (nError != sgl_no_err)
	{
		SglError(nError);
		return;
	}

	SglError(DefinePlane( pNode,pPlane, surfacePoint,point2,point3, bInvisible,
	  normal1,normal2,normal3, uv1,uv2,uv3 ));
}


/******************************************************************************
 * Function Name: sgl_delete_plane
 *
 * Inputs       : nPlaneIndex
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_delete_plane( int nPlaneIndex )
{
	int					nError,nDestPlane;
	CONVEX_NODE_STRUCT	*pNode;
	CONV_PLANE_STRUCT	*pDestPlaneData,*pSrcPlaneData;
	CONV_TEXTURE_UNION	*pDestTexture=NULL,*pSrcTexture=NULL;
	CONV_SHADING_STRUCT	*pDestShading=NULL,*pSrcShading=NULL;
	CONV_POINTS_STRUCT *pDestPoints=NULL,*pSrcPoints=NULL;

	nError = CommonSetPlane(&pNode,NULL, nPlaneIndex);
	if (nError != sgl_no_err)
	{
		SglError(nError);
		return;
	}

	for (nDestPlane = nPlaneIndex; nDestPlane <= (int)pNode->u16_num_planes -2;
	  nDestPlane++)
	{
		pDestPlaneData = pNode->plane_data + nDestPlane;
		pSrcPlaneData = pDestPlaneData+1;

		if (pNode->texture_data != NULL)
		{
			pDestTexture = pNode->texture_data + nDestPlane;
			pSrcTexture = pDestTexture+1;
		}

		if (pNode->shading_data != NULL)
		{
			pDestShading = pNode->shading_data + nDestPlane;
			pSrcShading = pDestShading+1;
		}

		if (pNode->points_data != NULL)
		{
			pDestPoints = pNode->points_data + nDestPlane;
			pSrcPoints = pDestPoints+1;
		}

		/*
		// OPTIMISATION: PUT CopyPlaneInfo CODE HERE INSTEAD OF IN SEPARATE
		// FUNCTION.
		*/
		CopyPlaneInfo(pDestPlaneData,pDestTexture,pDestShading,pDestPoints,
			pSrcPlaneData,pSrcTexture,pSrcShading,pSrcPoints,
			pNode->texture_data != NULL, 
			pNode->shading_data != NULL,
			pNode->points_data != NULL);
	}

	if (pNode->local_materials != NULL)
	{
		/*
		// Decrement the plane count of the corresponding local material (if
		// there is one):
		*/
		int nPlaneCount=0, nMat;

		for (nMat=0; nMat < pNode->u16_num_materials; nMat++)
		{
			nPlaneCount += pNode->local_materials[nMat].planes_affected;
			if (nPlaneCount > nPlaneIndex)
			{
				pNode->local_materials[nMat].planes_affected--;
				break;
			}
			/*
			// OPTIMISATION: MERGE THIS LOCAL MATERIAL WITH SUBSEQUENT ONES IF
			// IT IS NO LONGER USED FOR ANY PLANES.  REDUCE MAX_MATERIALS BY A
			// CHUNK IF APPLICABLE.
			// We are not bothering to reduce u16_max_materials and the
			// corresponding data structure size.
			*/
		}
	}

	pNode->u16_num_planes--;

	SglError(GenerateAdjacencyInfo(pNode));

	/* OPTIMISATION: CHECK WHETHER EACH CONVEX 'ALL' FLAG CAN NOW BE SET. */
}


/******************************************************************************
 * Function Name: sgl_plane_count
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : plane count or error
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_plane_count()
{
#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif

	DlCompleteCurrentTransform();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	if (dlUserGlobals.pCurrentConvex == NULL)
	{
		return SglError(sgl_err_no_convex);
	}

	return SglError(dlUserGlobals.pCurrentConvex->u16_num_planes);
}




/******************************************************************************
 * Function Name: sgl_create_light_volume()
 *
 * Inputs       : - bGenerateName -generate a name for this convex
 *				  - bLightVol - if TRUE create a light volume, else a shadow vol
 * Outputs      : -
 * Returns      : OPTIONALLY returns Name or error
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_create_light_volume( const sgl_bool bGenerateName,
							 		   const sgl_bool bLightVol,
									   const int LightName)
{

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
	else
#endif
	{
		int nError;

		DlCompleteCurrentTransform();
		DlCompleteCurrentConvex();
		DlCompleteCurrentMaterial();
		DlCompleteCurrentMesh();

		/*
		// Check that LightName is a valid light
		*/
		if(GetNamedItemType(dlUserGlobals.pNamtab, LightName) != nt_light)
		{
			DPFDEV((DBG_WARNING, "sgl_create_light_volume: calling with bad name"));			
			return SglError(sgl_err_bad_name);
		}
		
		/*
		// Check that it is a parallel or point light
		*/
		/*
		// NOT Implemented, it'll get skipped if its an ambient light anyway
		//							  SJF
		*/

		if(bLightVol)
		{
			nError = CreateConvex(cf_light_volume);
		}
		else
		{
			nError = CreateConvex(cf_shadow_volume);
		}

		/*
		// Store the associated light, and increment the usage count
		// on the name. (it should get decremented in the deletion of the
		// convex)
		*/
		if(nError == sgl_no_err)
		{
			dlUserGlobals.pCurrentConvex->u16_volumes_light = LightName;

			IncNamedItemUsage(dlUserGlobals.pNamtab, LightName);
		}

		if (bGenerateName && nError == sgl_no_err)
		{
			int nName;

			ASSERT(dlUserGlobals.pCurrentConvex != NULL);

			nName = AddNamedItem(dlUserGlobals.pNamtab,
			  dlUserGlobals.pCurrentConvex, nt_convex);

			dlUserGlobals.pCurrentConvex->node_hdr.n16_name =
			  nName == sgl_err_no_name ? NM_INVALID_NAME : (sgl_int16)nName;
			return SglError(nName);
		}
		else
		{
			return SglError(nError);
		}
	}
}

/******************************************************************************
 * Function Name: sgl_create_hidden_convex()
 *
 * Inputs       : - bGenerateName -generate a name for this convex
 * Outputs      : -
 * Returns      : OPTIONALLY returns Name or error
 * Globals Used : dlUserGlobals.pCurrentConvex
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_create_hidden_convex( sgl_bool bGenerateName )
{

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
	else
#endif
	{
		int nError;

		DlCompleteCurrentTransform();
		DlCompleteCurrentConvex();
		DlCompleteCurrentMaterial();
		DlCompleteCurrentMesh();

		nError = CreateConvex(cf_hidden_convex);

		if (bGenerateName && nError == sgl_no_err)
		{
			int nName;

			ASSERT(dlUserGlobals.pCurrentConvex != NULL);

			nName = AddNamedItem(dlUserGlobals.pNamtab,
			  dlUserGlobals.pCurrentConvex, nt_convex);

			dlUserGlobals.pCurrentConvex->node_hdr.n16_name =
			  nName == sgl_err_no_name ? NM_INVALID_NAME : (sgl_int16)nName;
			return SglError(nName);
		}
		else
		{
			return SglError(nError);
		}
	}
}


/*------------------------------- End of File -------------------------------*/
