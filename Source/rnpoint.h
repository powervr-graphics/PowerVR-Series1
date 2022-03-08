/******************************************************************************
 * Name         : rnpoint.h
 * Title        : SGL Render traversal for collision/position points
 * Author       : John Catchpole
 * Created      : 06/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Prototypes for the routines that handle collision/position
 *				  point nodes during display list traversal.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnpoint.h,v $
 * Revision 1.7  1996/09/10  10:15:55  jrc
 * Added collision state to list of parameters for RnProcessPointPosNode (to
 * enable rnpoint.c to compile).
 *
 * Revision 1.6  1995/11/03  14:37:49  jrc
 * Tidied up file.  Should not change code.
 *
 * Revision 1.5  1995/09/27  13:33:41  jrc
 * The point position node rendering function now takes the current transform
 * state instead of the collision state.
 *
 * Revision 1.4  1995/09/15  18:17:08  jrc
 * Added prototypes for RnTestPointsWithTransformed|LocalPlanes.
 *
 * Revision 1.3  1995/07/10  12:14:11  jrc
 * Made RnCleanupCollisionState's parameters consistent with rntrav.c
 *
 * Revision 1.2  1995/07/10  11:55:38  jrc
 * Added end of file marker and multiple #include sentry.
 *
 * Revision 1.1  1995/07/06  17:02:10  jrc
 * Initial revision
 *
 *****************************************************************************/
#ifndef __RNPOINT_H
#define __RNPOINT_H


#include "rnconvst.h"


void RnProcessPointNode( POINT_NODE_STRUCT		*pPointNode,
						 COLLISION_STATE_STRUCT *pCollisionState,
						 TRANSFORM_STRUCT		*pTransform);

void RnProcessPointPosNode( POINT_POSITION_NODE_STRUCT *pPosNode,
							TRANSFORM_STRUCT		   *pTransformState,
							COLLISION_STATE_STRUCT	   *pCollisionState);

void RnProcessPointSwitchNode( POINT_SWITCH_NODE_STRUCT *pSwitchNode,
							   COLLISION_STATE_STRUCT	*pCollisionState);

void RnTestPointsWithTransformedPlanes(
  const CONVEX_NODE_STRUCT *pConvexNode,
  TRANS_PLANE_ARRAY_TYPE   pTransformedPlanes,
  COLLISION_STATE_STRUCT   *pCollisionState,
  sgl_bool				   *pbParentUpdatePoints);

void RnTestPointsWithLocalPlanes(
  const CONVEX_NODE_STRUCT *pConvexNode,
  TRANSFORM_STRUCT		   *pTransformState,
  COLLISION_STATE_STRUCT   *pCollisionState,
  sgl_bool				   *pbParentUpdatePoints);

void RnCleanupCollisionState( COLLISION_STATE_STRUCT *pCollisionState,
							  sgl_bool				 *pbParentUpdatePoints);


#endif
/*------------------------------- End of File -------------------------------*/
