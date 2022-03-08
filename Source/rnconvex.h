/**************************************************************************
 * Name         : rnconvex.h
 * Title        : Rendering of Convex Primitives - Interface
 * Author       : Simon Fenney
 * Created      : 22 June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Interface to Code to process convex primitives as they are
 *				  encountered in the display list.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnconvex.h,v $
 * Revision 1.10  1997/04/07  14:51:57  gdc
 * added shadow limit struct to function parameters for RnProcessConvexNode
 *
 * Revision 1.9  1997/04/07  13:30:24  gdc
 * added a trans_set_id parameter to the RnProcessConvexNode function
 *
 * Revision 1.8  1996/10/17  14:10:27  gdc
 * reverted back accidently changed RnProcessConvexNode prototype
 *
 * Revision 1.7  1996/10/16  14:35:02  gdc
 * made RnTransformBasicPlanes non static so it can be called
 * from rntrav.c
 *
 * Revision 1.6  1996/10/15  15:25:43  gdc
 * forgot a semicolon
 *
 * Revision 1.5  1996/10/15  15:14:23  gdc
 * made RnTransformBasicPlanes available outside rnconvex.c
 * for shadow limit purposes
 *
 * Revision 1.4  1996/05/30  20:32:21  sjf
 *  Added texture caching.
 *
 * Revision 1.3  1995/09/15  18:11:15  jrc
 * Added parentUpdatePoints parameter to prototype for RnProcessConvexNode.
 *
 * Revision 1.2  1995/07/26  16:26:24  sjf
 * added const.
 *
 * Revision 1.1  1995/07/06  16:21:19  sjf
 * Initial revision
 *
 *
 *
 *
 **************************************************************************/


/*
// Routine to process Convex Node
*/
extern void RnTransformBasicPlanes( const CONV_PLANE_STRUCT * pPlanes,
									const int numPlanes,
									const TRANSFORM_STRUCT * pTransform,
									TRANS_PLANE_ARRAY_TYPE TransformedPlanes);

extern sgl_bool RnProcessConvexNode( const CONVEX_NODE_STRUCT *pConvex,
									 const MASTER_STATE_STRUCT *pState,
									 const SHADOW_LIM_STRUCT *pShadowLimitPlanes,
									 sgl_bool *parentUpdatePoints,
									 int nCurrTransSetID);
				
extern void RnCTPreProcessConvexNode(const CONVEX_NODE_STRUCT  *pConvex,
							 const MASTER_STATE_STRUCT *pState,
							 void *pCachedTexture);

/*
// END OF FILE
*/
