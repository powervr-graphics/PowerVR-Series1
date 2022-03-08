/******************************************************************************
 * Name  	    : rnpoint.c
 * Title        : Collision/position point rendering SGL functions.
 * Author  	    : John Catchpole
 * Created  	: 04/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Collision/position point rendering SGL functions.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnpoint.c,v $
 * Revision 1.10  1997/04/07  16:28:31  gdc
 * removed global reference to the absolute coord transform, and now
 * use a pointer got thru a function call
 *
 * Revision 1.9  1997/01/20  17:23:59  sxy
 * Added extra condition in collision detection for the point switch on (enable),
 * fixed the bug in neche\m044
 *
 * Revision 1.8  1996/09/12  13:20:30  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES.
 * Shadows may now have more than SGL_MAX_PLANES.
 * Actually twice as many. Re: NHE 044A, 049A.
 *
 * Revision 1.7  1996/09/10  10:14:40  jrc
 * Added collision state to list of parameters for RnProcessPointPosNode (to
 * enable this file to compile).
 *
 * Revision 1.6  1996/09/09  19:09:08  jrc
 * Hopefully fixed collision detection position point bug (position node
 * was not updating the world coordinates position of the point).
 *
 * Revision 1.5  1995/11/03  14:47:15  jrc
 * Tidied up code a bit - should be no functional change.
 *
 * Revision 1.4  1995/09/27  13:33:12  jrc
 * Should now correctly calculate the point's position from the position node.
 *
 * Revision 1.3  1995/09/15  18:15:43  jrc
 * The position/collision detection code for rendered points.
 *
 * Revision 1.2  1995/07/10  12:14:11  jrc
 * Made RnCleanupCollisionState's parameters consistent with rntrav.c
 *
 * Revision 1.1  1995/07/06  16:11:12  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_RN

#include <float.h>
#include "sgl_defs.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "rnstate.h"
#include "rnconvst.h"
#include "sgl_math.h"
#include "dlglobal.h"
#include "rnglobal.h"


/******************************************************************************
 * Function Name: RnProcessPointNode
 *
 * Inputs       : pPointNode, pCollisionState, pTransformState
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Process a position/collision point at render time.  Assumes
 *                that pNode points to a valid position/collision point display
 *                list node.
 *****************************************************************************/
void RnProcessPointNode(POINT_NODE_STRUCT *pPointNode,
  COLLISION_STATE_STRUCT *pCollisionState, TRANSFORM_STRUCT *pTransform)
{
	/* position of the point in world (camera-relative) coordinates: */
	sgl_vector pointWC;

	ASSERT(pPointNode != NULL);
	ASSERT(pCollisionState != NULL);
	ASSERT(pTransform != NULL);

	/*
	// ==============
	// POINT POSITION
	// ==============
	*/

	/* position in world coordinates */
	TransformVector(pTransform, pPointNode->offset, pointWC);

	/* position in absolute coordinates */
	TransformVector(RnGlobalGetAbsoluteCoordTransform(), 
					pointWC, pPointNode->position);

	/*
	// =========================
	// ADD TO ACTIVE POINTS LIST
	// =========================
	*/
	if ( pPointNode->collision_check &&
	  pCollisionState->num_pnts < SGL_MAX_ACTIVE_POINTS )
	{
		pPointNode->bEnableCollision = TRUE;
		pPointNode->n16Collision = FALSE;  /* not hit yet */

		pPointNode->nCollisionStateIndex = pCollisionState->num_pnts;

		VecCopy(pointWC,
		  pCollisionState->pnts[pCollisionState->num_pnts].pointWC);
		pCollisionState->pnts[pCollisionState->num_pnts].p_its_node =
		  pPointNode;

		pCollisionState->num_pnts++;
	}
	else
	{
		pPointNode->nCollisionStateIndex = -1;
	}

} /* RnProcessPointNode */


/******************************************************************************
 * Function Name: RnProcessPointPosNode
 *
 * Inputs       : pNode, pTransformState, pCollisionState
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Process a position point's position node at render time.
 *				  Assumes that pNode points to a valid position point display
 *				  list node.
 *
 *				  This needs only to be called for points not used for
 *				  collision detection (though calculating the position twice
 *				  will do no harm apart from a possibly significant efficiency
 *				  loss).
 *
 *				  Position only points do not require their positions to be
 *				  calculated until the point node is encountered during display
 *				  list traversal, when the node's camera-relative
 *				  transformation can be combined with the camera's own
 *				  transformation to determine the position in absolute
 *				  coordinates.
 *****************************************************************************/
void RnProcessPointPosNode( POINT_POSITION_NODE_STRUCT  *pPosNode,
							TRANSFORM_STRUCT  *pTransformState,
							COLLISION_STATE_STRUCT  *pCollisionState )
{
	sgl_vector  pointWC;
	POINT_NODE_STRUCT  *pPointNode;

	ASSERT(pPosNode != NULL);
	ASSERT(pTransformState != NULL);

	ASSERT( GetNamedItemType(dlUserGlobals.pNamtab, pPosNode->point_name) ==
	  nt_point );
	pPointNode = GetNamedItem(dlUserGlobals.pNamtab, pPosNode->point_name);
	ASSERT(pPointNode != NULL);

	/*
	// Calculate the point's position in world coordinates.
	*/
	TransformVector(pTransformState, pPointNode->offset, pointWC);

	if (pPointNode->nCollisionStateIndex != -1)
	{
		ASSERT(pPointNode->nCollisionStateIndex >= 0);
		ASSERT(pPointNode->nCollisionStateIndex < SGL_MAX_ACTIVE_POINTS);

		VecCopy(pointWC,
		  pCollisionState->pnts[pPointNode->nCollisionStateIndex].pointWC);
	}

	/*
	// Calculate the point's position in absolute coordinates.
	*/
	TransformVector(RnGlobalGetAbsoluteCoordTransform(), 
					pointWC, pPointNode->position);

} /* RnProcessPointPosNode */


/******************************************************************************
 * Function Name: RnProcessPointSwitchNode
 *
 * Inputs       : pSwitchNode, pCollisionState
 * Outputs      : pCollisionState
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Process a point switch node at render time.  Assumes that
 *				  pSwitchNode points to a valid point switch display list node.
 *****************************************************************************/
void RnProcessPointSwitchNode(POINT_SWITCH_NODE_STRUCT *pSwitchNode,
  COLLISION_STATE_STRUCT *pCollisionState)
{
	int nEntry=0;

	ASSERT(pSwitchNode != NULL);
	ASSERT(pCollisionState != NULL);

	/*
	// ----------------------------------------------------
	// Look for the switch's point in the active point list
	// ----------------------------------------------------
	// The point will not be found if the switch is out of its scope, the point
	// was deleted leaving the switch in place, or the point was hit during
	// traversal of a child list that preserved the state.
	// OPTIMISATION ? STORE THE POINT'S NAME DIRECTLY IN THE ENTRY STRUCTURE
	*/
	ASSERT(pSwitchNode->n16_point_name >= 0);
	ASSERT(pSwitchNode->n16_point_name != NM_INVALID_NAME);

	while (nEntry < pCollisionState->num_pnts &&
	  pCollisionState->pnts[nEntry].p_its_node->node_hdr.n16_name !=
	  pSwitchNode->n16_point_name)
	    nEntry++;

	/*
	// ------------------------------
	// If found then switch the point
	// ------------------------------
	*/
	ASSERT(nEntry <= pCollisionState->num_pnts);

	if (nEntry < pCollisionState->num_pnts)
	{
		pCollisionState->pnts[nEntry].p_its_node->bEnableCollision =
		  pSwitchNode->n16_enable_check;
	}

} /* RnProcessPointSwitchNode */


/******************************************************************************
 * Function Name: RnTestPointsWithTransformedPlanes
 *
 * Inputs       : pConvexNode, pTransformedPlanes, pCollisionState
 * Outputs      : pCollisionState, pbParentUpdatePoints
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Determines points that hit the transformed object, and sets
 *				  the value of pbParentUpdatePoints to TRUE if a hit occurs.
 *
 *				  Called by RnProcessConvexNode in rnconvex.c after
 *				  transforming the planes of each object at render time.
 *
 *				  OPTIMISATION ? TEST ON OBJECTS' BOUNDING BOXES (IN ADJACENCY
 *				  INFO) BEFORE TESTING WITH THE ACTUAL PLANES
 *****************************************************************************/
void RnTestPointsWithTransformedPlanes(
  const CONVEX_NODE_STRUCT *pConvexNode,
  TRANS_PLANE_ARRAY_TYPE   pTransformedPlanes,
  COLLISION_STATE_STRUCT   *pCollisionState,
  sgl_bool				   *pbParentUpdatePoints)
{
	int	  nPoint, nPlane,nClosestPlane,
		  nNumPlanes = pConvexNode->u16_num_planes;
	float fDistance,fClosestDistance;

	COLLISION_PNT_ENTRY_STRUCT *pEntry;
	POINT_NODE_STRUCT		   *pPointNode;

	ASSERT(pTransformedPlanes != NULL);
	ASSERT(nNumPlanes > 0);
	ASSERT(nNumPlanes <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pCollisionState != NULL);
	ASSERT(pConvexNode != NULL);
	ASSERT(pbParentUpdatePoints != NULL);

	ASSERT(pCollisionState->num_pnts >= 0);
	ASSERT(pCollisionState->num_pnts <= SGL_MAX_ACTIVE_POINTS);

	/*
	// ===============
	// TEST EACH POINT
	// ===============
	*/
	for (nPoint=0; nPoint < pCollisionState->num_pnts; nPoint++)
	{
		pEntry = pCollisionState->pnts + nPoint;
		
		/*
		// If this point is enabled for check collision
		*/
		if ( pEntry->p_its_node->bEnableCollision)
		{
			/* OPTIMISATION FOR LONG DISPLAY LISTS ? REMOVE ENTRY FROM ACTIVE
		   	   POINTS LIST ON HIT SO WE DO NOT NEED THIS TEST */
			/*
			// If this point has not already hit...
			*/
			if (! pEntry->p_its_node->n16Collision)
			{
				nClosestPlane = -1;
				fClosestDistance = -FLT_MAX;

				/*
				// ---------------
				// Test each plane
				// ---------------
				*/
				for (nPlane=0; nPlane < nNumPlanes; nPlane++)
				{
					fDistance = DotProd(pEntry->pointWC,
					  pTransformedPlanes[nPlane].normal)
					  - pTransformedPlanes[nPlane].d;

					if (fDistance < 0.0f)
					{
						/* inside object */
						if (fDistance > fClosestDistance)
						{
							fClosestDistance = fDistance;
							nClosestPlane = nPlane;
						}
					}
					else
					{
						/* outside object */
						nClosestPlane = -1;
						break;
					}
				}

				/*
				// ------
				// If hit
				// ------
				*/
				if (nClosestPlane != -1)
				{
					ASSERT(nClosestPlane >= 0);
	
					pPointNode = pEntry->p_its_node;

					pPointNode->n16Collision = TRUE;

					/* OPTIMISATION: MAKE NM_INVALID_NAME AND SGL_ANON_OBJECT ONE
					   AND THE SAME TO AVOID THE COMPARISON */
	                pPointNode->n16ObjectName =
					  pConvexNode->node_hdr.n16_name == NM_INVALID_NAME ?
					  SGL_ANON_OBJECT : pConvexNode->node_hdr.n16_name;
	
					pPointNode->n16ObjectPlane = nClosestPlane;

					VecCopy(pTransformedPlanes[nClosestPlane].normal,
					  pPointNode->normal);

					pPointNode->fD = pTransformedPlanes[nClosestPlane].d;

					/*
					// UNFINISHED: Set pPointNode->pn16Path and
					// pPointNode->n16PathLength properly.
					*/
					pPointNode->n16PathLength = 0;

					*pbParentUpdatePoints = TRUE;
				}
				
			}
		
		}

	} /* for each point */

} /* RnTestPointsWithTransformedPlanes */


/******************************************************************************
 * Function Name: RnTestPointsWithLocalPlanes
 *
 * Inputs       : pConvexNode, pCollisionState, pTransformState
 * Outputs      : pCollisionState, pbParentUpdatePoints
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Determines points that hit the object (in local coordinates),
 *				  and sets the value of pbParentUpdatePoints to TRUE if a hit
 *				  occurs.
 *
 *				  This is similar to RnTestPointsWithTransformedPlanes, but
 *				  with the collision point first transformed into the object's
 *				  local coordinates.  Other differences are due to the d value
 *				  for each plane not being available.
 *				  OPTIMISATION ? STORE EACH PLANE'S LOCAL D VALUE IN THE CONVEX
 *				  NODE.
 *
 *				  Called by RnProcessConvexNode in rnconvex.c for objects that
 *				  are off screen and do not have shadows
 *
 *				  OPTIMISATION ? TEST ON OBJECTS' BOUNDING BOXES (IN ADJACENCY
 *				  INFO) BEFORE TESTING WITH THE ACTUAL PLANES
 *****************************************************************************/
void RnTestPointsWithLocalPlanes(
  const CONVEX_NODE_STRUCT *pConvexNode,
  TRANSFORM_STRUCT		   *pTransformState,
  COLLISION_STATE_STRUCT   *pCollisionState,
  sgl_bool				   *pbParentUpdatePoints)
{
	int		   nPoint, nPlane,nClosestPlane;
	float	   fDistance,fClosestDistance;
	sgl_vector pointLC,  /* current point in local coordinates */
			   repToPoint;

	CONV_PLANE_STRUCT *pPlaneData = pConvexNode->plane_data;
	int				  nNumPlanes = pConvexNode->u16_num_planes;

	COLLISION_PNT_ENTRY_STRUCT *pEntry;
	POINT_NODE_STRUCT		   *pPointNode;


	ASSERT(pConvexNode != NULL);
	ASSERT(pPlaneData != NULL);
	ASSERT(nNumPlanes > 0);
	ASSERT(nNumPlanes <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pCollisionState != NULL);
	ASSERT(pbParentUpdatePoints != NULL);

	ASSERT(pCollisionState->num_pnts >= 0);
	ASSERT(pCollisionState->num_pnts <= SGL_MAX_ACTIVE_POINTS);

	/*
	// ===============
	// TEST EACH POINT
	// ===============
	*/
	for (nPoint=0; nPoint < pCollisionState->num_pnts; nPoint++)
	{
		pEntry = pCollisionState->pnts + nPoint;
		
		/*
		// If this point is enabled for check collision
		*/
		if ( pEntry->p_its_node->bEnableCollision)
		{

			/* OPTIMISATION FOR LONG DISPLAY LISTS ? REMOVE ENTRY FROM ACTIVE
		  	   POINTS LIST ON HIT SO WE DO NOT NEED THIS TEST */
			/*
			// If this point has not already hit...
			*/
			if (! pEntry->p_its_node->n16Collision)
			{
				InvTransformVector(pTransformState, pEntry->pointWC,pointLC);

				nClosestPlane = -1;
				fClosestDistance = -FLT_MAX;

				/*
				// ---------------
				// Test each plane
				// ---------------
				*/
				for (nPlane=0; nPlane < nNumPlanes; nPlane++)
				{
					VecSub(pointLC, pPlaneData[nPlane].rep_point, repToPoint);
					fDistance = DotProd(repToPoint, pPlaneData[nPlane].normal);
					if (fDistance < 0.0f)
					{
						/* inside object */
						if (fDistance > fClosestDistance)
						{
							fClosestDistance = fDistance;
							nClosestPlane = nPlane;
						}
					}
					else
					{
						/* outside object */
						nClosestPlane = -1;
						break;
					}
				}

				/*
				// ------
				// If hit
				// ------
				*/
				if (nClosestPlane != -1)
				{
					ASSERT(nClosestPlane >= 0);
	
					pPointNode = pEntry->p_its_node;

					pPointNode->n16Collision = TRUE;

					/* OPTIMISATION: MAKE NM_INVALID_NAME AND SGL_ANON_OBJECT ONE
					   AND THE SAME TO AVOID THE COMPARISON */
					pPointNode->n16ObjectName =
					  pConvexNode->node_hdr.n16_name == NM_INVALID_NAME ?
					  SGL_ANON_OBJECT : pConvexNode->node_hdr.n16_name;

					pPointNode->n16ObjectPlane = nClosestPlane;

					VecCopy(pPlaneData[nClosestPlane].normal, pPointNode->normal);

					pPointNode->fD = DotProd(pPlaneData[nClosestPlane].rep_point,
					  pPlaneData[nClosestPlane].normal);

					/*
					// UNFINISHED: Set pPointNode->pn16Path and
					// pPointNode->n16PathLength properly.
					*/
					pPointNode->n16PathLength = 0;
	
					*pbParentUpdatePoints = TRUE;
				}
				
			}
			
		}

	} /* for each point */

} /* RnTestPointsWithLocalPlanes */


/******************************************************************************
 * Function Name: RnCleanupCollisionState
 *
 * Inputs       : pCollisionState
 * Outputs      : pCollisionState, pbParentUpdatePoints
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Called when we have finished processing a child list that
 *				  preserved the state and had at least one point hit during its
 *				  processing.
 *
 *				  Removes any hit collision points from the list of active
 *				  points, compacting the list as necessary, and sets the
 *				  contents of pbParentUpdatePoints if we have found any such
 *				  points (else leave it unchanged).
 *****************************************************************************/
void RnCleanupCollisionState(COLLISION_STATE_STRUCT *pCollisionState,
  sgl_bool *pbParentUpdatePoints)
{
	int nSrc,nDest=0;

	ASSERT(pCollisionState != NULL);
	ASSERT(pbParentUpdatePoints != NULL);

	ASSERT(pCollisionState->num_pnts >= 0);
	ASSERT(pCollisionState->num_pnts <= SGL_MAX_ACTIVE_POINTS);

	for (nSrc=0; nSrc < pCollisionState->num_pnts; nSrc++)
	{
	  	if (pCollisionState->pnts[nSrc].p_its_node->n16Collision)
		{
			*pbParentUpdatePoints = TRUE;
		}
		else
		{
			/* point not hit */

			/* OPTIMISATION ? CHECK THAT nDest != nSrc FIRST ? */
			pCollisionState->pnts[nDest] = pCollisionState->pnts[nSrc];

			nDest++;
		}
	}

	ASSERT(nDest >= 0 && nDest <= pCollisionState->num_pnts);

	pCollisionState->num_pnts = nDest;  /* the number of unhit points left */

} /* RnCleanupCollisionState */


/*------------------------------- End of File -------------------------------*/
