/******************************************************************************
 * Name         : rnlod.c
 * Title        : Level of detail rendering SGL function.
 * Author       : John Catchpole
 * Created      : 04/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnlod.c,v $
 * Revision 1.6  1997/04/09  16:58:37  gdc
 * removed reference to projection_mat global
 *
 * Revision 1.5  1995/10/16  11:26:26  jrc
 * Fixed incorrect model order bug.
 *
 * Revision 1.4  1995/09/22  12:31:13  jrc
 * Improved the accuracy of the level of detail calculation without slowing it
 * down.
 *
 * Revision 1.3  1995/09/15  18:14:21  jrc
 * Changed to use bounding box instead of sphere.
 *
 * Revision 1.2  1995/07/06  15:35:19  jrc
 * Added MODULE_ID
 *
 * Revision 1.1  1995/07/06  15:00:08  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_RN

#include "sgl_defs.h"
#include "dlnodes.h"
#include "rnglobal.h"
#include "sgl_math.h"


/*
// ============================================================================
// 							  SGL INTERNAL ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: RnProcessLodNode
 *
 * Inputs       : pNode, pTransformState
 * Outputs      : -
 * Returns      : list to use for the required model
 * Globals Used : projection_mat.fCosAnglePerCentralPixel
 *
 * Description  : Selects the required level of detail object list to render.
 *				  Assumes that pNode points to a valid level of detail display
 *				  list node.
 *
 *				  The method used ignores the effect of perspective projection
 *				  on the difference between the sizes of the front and back of
 *				  the projected bounding box.  This is only likely to be a
 *				  problem in some unusual cases when there is a large ratio
 *				  between the back and front distances to the camera, say when
 *				  the back is twice or more as far away from the camera as the
 *				  front, and the angular size of the back is less than the
 *				  largest level of detail threshold.  If this proves to be a
 *				  problem then more box vertices should be tested.
 *
 *				  Currently only two vertices, either end of one of the longest
 *				  edges of the box are tested.  This is done by transforming
 *				  the positions of these two vertices and the box centre into
 *				  world coordinates (relative to the camera) and normalising
 *				  the resulting vectors.  The offset from 1 of the smallest dot
 *				  product (corresponding to the largest angle) of the
 *				  normalised box centre and vertex vectors is compared with the
 *				  threshold offset cosine of angle values (from the display
 *				  list node pixel thresholds and
 *				  projection_mat.fCosAnglePerCentralPixel - see comment in
 *				  rncamera.c) to determine the level of detail list to be used.
 *
 *				  If the transformation state at the level of detail node
 *				  contains uniform absolute scaling (reflections, i.e. negative
 *				  scalings can be ignored) such that angles are preserved then
 *				  the calculations can be done in local coordinates.
 *
 *				  Since we are comparing the angles at the camera between the
 *				  box centre and vertex directions the stored thresholds
 *				  represent half the pixel sizes specified by the user.
 *
 * OPTIMISATION ? USE PREVIOUS VALUES OF TRANSFORMED CAMERA / BOX CORNERS AND
 *				  OTHER RESULTS IF TRANSFORMATION MATRIX AND/OR BOX DEFINITION
 *				  HAVE NOT CHANGED FROM THE PREVIOUS FRAME ?
 *****************************************************************************/
int RnProcessLodNode(LOD_NODE_STRUCT *pNode, TRANSFORM_STRUCT *pTransformState)
{
    float	   fDotProd1,fDotProd2, fLargestDotProdOffset;
	sgl_vector cameraToBoxCentre, cameraToBoxVertex1, cameraToBoxVertex2,
			   cameraLC;  /* camera position in local coordinates */

	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	float fCosAnglePerCentralPixel = pProjMat->fCosAnglePerCentralPixel;

	ASSERT(pNode != NULL);
	ASSERT(pTransformState != NULL);

	if (pTransformState->scale_flag == arbitrary_scale)
	{
		/*
		// -----------------------------------------
		// Calculations are now in world coordinates
		// -----------------------------------------
		// This is slower than the local coordinate method (see description
		// above).
		*/
		TransformVector(pTransformState, pNode->boxCentre, cameraToBoxCentre);
		TransformVector(pTransformState, pNode->boxVertex1,cameraToBoxVertex1);
		TransformVector(pTransformState, pNode->boxVertex2,cameraToBoxVertex2);
	}
	else
	{
		/*
		// -----------------------------------------
		// Calculations are now in local coordinates
		// -----------------------------------------
		*/
		cameraLC[0] = pTransformState->inv[0][3];
		cameraLC[1] = pTransformState->inv[1][3];
		cameraLC[2] = pTransformState->inv[2][3];

		VecSub(pNode->boxCentre,cameraLC, cameraToBoxCentre);
		VecSub(pNode->boxVertex1,cameraLC, cameraToBoxVertex1);
		VecSub(pNode->boxVertex2,cameraLC, cameraToBoxVertex2);
	}

	/*
	// If we cannot normalise cameraToBoxCentre because it is too short (use
	// sum of squares of ordinates - Pythagoras without the square root)...
	*/
	/* OPTIMISATION ? MAKE THIS TEST PART OF THE
	   VecNormalise(cameraToBoxCentre) BELOW */
	if ( DotProd(cameraToBoxCentre, cameraToBoxCentre) < 1.0e-10f )
	{
		/*
		// Return the largest level of detail since the box centre is very
		// close to the camera.  This will not happen very often, so it is not
		// important that cameraToBoxVertex1 & 2 may have been calculated
		// unnecessarily.
		*/
		DPF((DBG_MESSAGE,"camera close to box centre"));
		return pNode->pn16Models[0];
	}

	VecNormalise(cameraToBoxCentre);
	VecNormalise(cameraToBoxVertex1);
	VecNormalise(cameraToBoxVertex2);

	fDotProd1 = DotProd(cameraToBoxCentre, cameraToBoxVertex1);
	fDotProd2 = DotProd(cameraToBoxCentre, cameraToBoxVertex2);
	/*
	// fLargestDotProdOffset represents the largest angle (see description
	// above).  The dot products will usually be just less than 1.0 since the
	// angles are small so we use the offset from 1.0 to compare with
	// fCosAnglePerCentralPixel to retain sufficient accuracy.  Floats close to
	// zero benefit from the exponent whereas values close to 1.0 do not.
	*/
	fLargestDotProdOffset = 1.0f - MIN(fDotProd1,fDotProd2);

	/*
	// An approximation here is to assume that for a range of small angles
	// the cosine of these angles is approximated by a 1 - x*x curve.
	*/
	DPF((DBG_MESSAGE,"value: %e",fLargestDotProdOffset));
	DPF((DBG_MESSAGE,"thresholds: %e, %e",
	  pNode->pfHalfThresholds[0] * fCosAnglePerCentralPixel,
	  pNode->pfHalfThresholds[1] * fCosAnglePerCentralPixel));

	if ( fLargestDotProdOffset <
	  pNode->pfHalfThresholds[0] * fCosAnglePerCentralPixel )
	{
		return pNode->pn16Models[2];  /* least detailed */
	}

	if ( fLargestDotProdOffset >
	  pNode->pfHalfThresholds[1] * fCosAnglePerCentralPixel )
	{
		return pNode->pn16Models[0];  /* most detailed */
	}

	return pNode->pn16Models[1];  /* middle level of detail */
}


/*------------------------------- End of File -------------------------------*/
