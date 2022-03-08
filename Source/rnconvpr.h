/**************************************************************************
 * Name         : rnconvpr.h
 * Title        : Convex primitives Projection/Shading etc code header
 * Author       : Simon Fenney
 * Created      : 22 June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Interface to projection, shading etc code for convex 
 *				  primitives.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnconvpr.h,v $
 * Revision 1.7  1997/04/07  14:49:59  gdc
 * added shadow limit plane to RnProcessConvexShadows parameter list
 *
 * Revision 1.6  1997/04/07  13:24:35  gdc
 * added trans set ID as a call parameter to processvisibleconvex
 *
 * Revision 1.5  1995/09/19  09:10:50  sjf
 *  Added Light/Shadow volume processing routine.
 *
 * Revision 1.4  1995/08/27  15:24:46  sjf
 * Added shadow generation.
 *
 * Revision 1.3  1995/07/30  13:00:08  sjf
 * Added bounding box pointer param.
 *
 * Revision 1.2  1995/07/26  16:26:57  sjf
 * Added prototype def.
 *
 * Revision 1.1  1995/07/23  16:21:55  sjf
 * Initial revision
 *
 *
 *
 *
 **************************************************************************/

extern sgl_bool RnProcessVisibleConvex(const CONVEX_NODE_STRUCT  *pConvex,
									   const MASTER_STATE_STRUCT *pState,
									   TRANS_PLANE_ARRAY_TYPE transformedPlanes, 
									   const sgl_bool  NeedsClipping,
									   const BBOX_MINMAX_STRUCT * pBBox,
									   sgl_bool * pOffscreen,
									   int nCurrTransSetID);

extern sgl_bool RnProcessConvexShadows(const CONVEX_NODE_STRUCT  *pConvex,
									   const MASTER_STATE_STRUCT *pState,
									   const SHADOW_LIM_STRUCT *pShadowLimitPlanes,
									   const TRANS_PLANE_ARRAY_TYPE transformedPlanes, 
									   const BBOX_MINMAX_STRUCT *pBBox,
									   const sgl_bool objectOffscreen,
									   const sgl_bool ReUseABCs);

extern sgl_bool RnProcessLightOrShadVolume(const CONVEX_NODE_STRUCT  *pConvex,
								const MASTER_STATE_STRUCT *pState,
								TRANS_PLANE_ARRAY_TYPE transformedPlanes, 
								const BBOX_MINMAX_STRUCT *pBBox);



/*
// End of file
*/
