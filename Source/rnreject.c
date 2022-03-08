/******************************************************************************
 * Name         : rnreject.c
 * Title        : Rejection testing for convex primitives.
 * Author       : John Catchpole
 * Created      : 10/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : See rnreject.h.  This code has a lot of loose ends because it
 *				  has yet to be determined which of the three methods is
 *				  optimal for RnDetermineRegionsFromPlanes, and they have not
 *				  all been fully implemented.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnreject.c,v $
 * Revision 1.39  1997/06/04  15:32:13  gdc
 * stopped compiler warnings
 *
 * Revision 1.38  1997/06/04  11:15:05  gdc
 * removed some compiler warnings
 *
 * Revision 1.37  1997/04/08  16:16:03  gdc
 * all accesses to projection_mat are now done via pointers obtained
 * from a function rather than directly to the global
 *
 * Revision 1.36  1997/04/08  15:38:13  gdc
 * removed methods 0,1,2 from file leaving the only actually used
 * method 3
 *
 * Revision 1.35  1997/02/03  11:02:18  gdc
 * removed D3DOPTIM_SGL conditionals as this is now the only build option
 *
 * Revision 1.34  1997/01/09  17:49:03  gdc
 * modified constant used for SLOP for new dregion stuff which works
 * with exact Y-coords rather than regions
 *
 * Revision 1.33  1996/12/10  23:51:11  erf
 * Changed #ifdef D3DOPTIM_SGL to #if D3DOPTIM_SGL.
 *
 * Revision 1.32  1996/12/03  16:52:14  erf
 * Fixed bug to provide absolute Y co-ordinates with single reversed plane.
 * D3DOPTIM_SGL compile switch in RnDetermineRegionsFromPlanes().
 *
 * Revision 1.31  1996/12/02  13:02:06  erf
 * D3D optimisation enabled for SGL using D3DOPTIM_SGL compile switch.
 *
 * Revision 1.30  1996/11/29  11:47:56  erf
 * Using D3DOPTIM compile switch for optimisation do:
 * 1. Use .SyToRegionsExact to calculate absolute Y co-ordinates.
 * 2. Use .OyToRegionsExact to calculate absolute Y co-ordinates.
 * 3. Also use FirstYRegionExact and LastYRegionExact to set absolute Y values.
 * 4. In RnDetermineRegionsFromPLanes() convert from Y tiles to absolute Y co-ordinates.
 *
 * Revision 1.29  1996/10/09  15:34:40  mal
 * Changed some metric macros
 *
 * Revision 1.28  1996/09/12  13:22:07  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES.
 * Shadows objects may now have more than SGL_MAX_PLANES.
 * Actually twice as many. re: NHE 044A, NHE 049A.
 *
 * Revision 1.27  1996/06/17  19:10:43  jop
 * GetRegionsFromBBox now does z clipping too
 *
 * Revision 1.26  1996/05/29  17:29:28  jrc
 * 1. Added DERIVE_STRIP stuff for separate calculation of horizontal and
 *    vertical strips, but not actually using it yet.
 * 2. Added function name comments at the end of all functions for readibility
 *    of the large source file.
 *
 * Revision 1.25  1996/05/22  12:00:48  jrc
 * Added externally visible debug variables for shadow region results.
 *
 * Revision 1.24  1996/04/22  12:12:18  sjf
 * Changed memory.h to string.h
 *
 * Revision 1.23  1996/04/17  15:48:51  sjf
 * Moved "front clip" to a more useful spot.
 *
 * Revision 1.22  1996/03/22  18:19:15  jrc
 * Removed more references to projection_mat.RCentre[Z] where the value
 * required is 1.  This is an extension of the bug fix of revision 1.9.
 *
 * Revision 1.21  1995/12/05  14:49:28  jrc
 * Removed unused return value variable from RnDetermineRegionsFromBBox.
 *
 * Revision 1.20  1995/10/27  14:25:45  jop
 * Fixed cut & paste bug in GetRegions from BBox
 *
 * Revision 1.19  1995/10/26  14:07:52  jop
 * GetRegionsFromBBox now returns TRUE for onscreen FALSE for offscreen
 *
 * Revision 1.18  1995/10/26  13:22:49  jrc
 * Just updated a comment.
 *
 * Revision 1.17  1995/10/23  15:46:20  jrc
 * Added code to handle the case where the object was thought to be probably in
 * the viewport but was subsequently found to be in none of the regions.
 *
 * Revision 1.16  1995/10/23  11:49:35  jrc
 * Added check for being inside the infinite object to RnTestPlanesWithCamera.
 *
 * Revision 1.15  1995/10/17  12:42:20  jrc
 * Now passing rectangle of regions as a struct instead of four separate ints.
 * This should be faster.
 *
 * Revision 1.14  1995/10/10  15:05:31  sjf
 * Reduced the slop factor when calculating a boxes regions.
 *
 * Revision 1.13  1995/10/09  15:22:22  jrc
 * Added handling for case in RnDetermineRegionsFromPlanes where we are inside
 * the infinite object (there are no forward planes) whereby all regions should
 * be checked for that object.
 *
 * Revision 1.12  1995/10/07  17:50:20  sjf
 * Moved Determine regions from BBox from rnconvpr to more common
 * location so that it can be used by meshes.
 *
 * Revision 1.11  1995/10/07  17:08:07  sjf
 * OOPS fixed bug with changes to test box...
 *
 * Revision 1.10  1995/10/07  15:10:03  sjf
 * 1) Made test box with camera classify the box into 3 groups...
 *    Offscreen, partly on, completely on
 * 2) Optimised it a bit by reducing the number of multiplies performed by
 *    half.
 *
 * Revision 1.9  1995/10/03  17:59:19  jrc
 * Removed incorrect use of projection_mat.RCentre[2] when the value 1.0 was
 * required instead.
 *
 * Revision 1.8  1995/09/29  19:32:19  jrc
 * 1. Now pass correct parameter order to FindEdge.
 *
 * Revision 1.7  1995/09/07  12:13:48  jrc
 * Changed RnDetermineRegionsFromPlanes to pass planes in a single struct.
 *
 * Revision 1.6  1995/08/31  15:03:00  jrc
 * Changed a pFirstXRegion to a pLastXRegion.
 *
 * Revision 1.5  1995/08/31  13:49:10  jrc
 * Added a huge amount of code.
 *
 * Revision 1.4  1995/07/31  12:48:35  jrc
 * More progress on converting RnTestPlanesWithCamera to SGL.
 *
 * Revision 1.3  1995/07/30  11:17:34  sjf
 * Changed the bounding box type used by reject code from the centre/offset
 * to the min/max variety as this made a lot more sence. (Apologies to John C
 * for asking him to do it the other way first).
 *
 * Revision 1.2  1995/07/26  15:53:17  jrc
 * Fixed DPFOO command.
 *
 * Revision 1.1  1995/07/26  15:50:35  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_RN


#include <math.h>
#include <string.h>  /* for memset() */

#include "sgl_defs.h"
#include "dlnodes.h"
#include "rnstate.h"
#include "rnglobal.h"
#include "rnconvst.h"
#include "pmsabre.h"
#include "rnreject.h"
#include "singmath.h"

#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/*
// ------
// METHOD
// ------
// Should be 0, 1, 2 or 3.  See RnDetermineRegionsFromPlanes header.
*/
#define METHOD 3

/*
// ----------
// PLANE_STEP
// ----------
// Check alternate planes in MarkRepPointsRegion and RnTestPlanesWithCamera.
*/
/* OPTIMISATION ? HAVE SEPARATE STEP SIZES FOR EACH USE OF PLANE_STEP */
#define PLANE_STEP 2

/*
// --------------------------------------------------
// REJECT_X_RECURSION_LIMIT, REJECT_Y_RECURSION_LIMIT
// --------------------------------------------------
// Used only for Methods 1 and 2.
//
// The maximum recursion depths (including the initial pass) in
// RnDetermineRegionsFromPlanes with respect to each region grid dimension.
// They should be set to the maximums required for MAX_X_REGIONS and
// MAX_Y_REGIONS, as defined in pmsabre.h, respectively.
//
// E.g. a value of 4 for each allows for the case where we have child blocks
// with dimensions of the following numbers of regions: 8, 4, 2 and 1.  For
// this case it is assumed that MAX_X_REGIONS and MAX_Y_REGIONS do not exceed
// 16.
//
// The #defines for these are in rnglobal.h because they are required for
// precalculated region block centres and sizes in the projection matrix
// structure.
*/

/*
// -----
// X,Y,Z
// -----
*/
#define X 0
#define Y 1
#define Z 2

/*
// --------------------------------
// VERY_SMALL, ACCURACY_SLOP_FACTOR
// --------------------------------
// Constants for PlaneRangesInViewport, PlaneRangesInRegions and
// PlaneRangesInBlocks.
*/
#define VERY_SMALL (1.0E-10f)
#define ACCURACY_SLOP_FACTOR (1.0625f)




/*
// ============================================================================
// 								GLOBAL VARIABLES:
// ============================================================================
*/

/*
// These are used for storing the rectangle of regions of the last multi-plane
// shadow found in the scene.  This is to help debugging the rectangle of
// regions calculation.
*/
#if WIN32 && DEBUG
__declspec(dllexport) int  gnShadowRgnXMin, gnShadowRgnXMax, gnShadowRgnYMin,
  gnShadowRgnYMax;
#endif


/*
// ============================================================================
// 								STATIC FUNCTIONS:
// ============================================================================
*/


#if METHOD == 3
/******************************************************************************
 * Function Name: MarkPointsOnStrips
 *
 * Inputs       : pPlanes, nNumPlanes
 * Outputs      : pbInRows, pbInColumns
 * Returns      : -
 * Globals Used : -
 *
 * Description  : This function is used to determine which rows and columns of
 *				  regions an object is definitely in.
 *
 *				  For each plane it examines the rep point, projects it into
 *				  world coordinates, and then records the region row and
 *				  column, if any, of the display the point belongs to.  If the
 *				  point is outside the display then the region map is not
 *				  updated.  NOTE: it isn't too exact, but we only require a
 *				  guess as to where the object is.
 *****************************************************************************/
static void  MarkPointsOnStrips (
  const TRANSFORMED_PLANE_STRUCT  *ppPlanes[SGL_MAX_INTERNAL_PLANES],
  int  nNumPlanes,
  sgl_bool  pbInRows[MAX_Y_REGIONS],
  sgl_bool  pbInColumns[MAX_X_REGIONS] )
{
	int   		nRegionX,nRegionY, nPlane;
	float		fInvZ;
	int         tmpFx, tmpFy, tmpLx, tmpLy;
	float       tmpOx, tmpOy, tmpSx, tmpSy;
	const float *repPoint;  /* Treat this as a sgl_vector. */
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();

	tmpOx = pProjMat->OxToRegions;
	tmpOy = pProjMat->OyToRegions;
	tmpSx = pProjMat->SxToRegions;
	tmpSy = pProjMat->SyToRegions;

	tmpFx = pProjMat->FirstXRegion;
	tmpFy = pProjMat->FirstYRegion;
	tmpLx = pProjMat->LastXRegion;
	tmpLy = pProjMat->LastYRegion;

	for (nPlane=0; nPlane < nNumPlanes; nPlane += PLANE_STEP)
	{
		/*
		// Get access to the rep point of this plane
		*/
		repPoint = ppPlanes[nPlane]->repPnt;

		/*
		// First see if the point is not too close to, or behind, the camera.
		// If it is OK, then proceed with the projection.
		*/
		if (repPoint[Z] > 1e-10f)
		{
			/*
			// Project the point
			*/
			fInvZ = 1.0f/repPoint[Z];

			/*
			// Determine the region to which it belongs.
			*/
			nRegionX = (int)( repPoint[X] * fInvZ * tmpSx + tmpOx );
			nRegionY = (int)( repPoint[Y] * fInvZ * tmpSy + tmpOy );

			/*
			// If these are in range, mark the column and row:
			// OPTIMISATION ? JUST CHECK THAT nRegionX|Y >= 0 and <=
			// MAX_X|Y_REGIONS
			*/
			if ( nRegionX >= tmpFx && nRegionX <= tmpLx &&
				 nRegionY >= tmpFy && nRegionY <= tmpLy )
			{
				pbInRows[nRegionY] = TRUE;
				pbInColumns[nRegionX] = TRUE;
				/*DPF((DBG_MESSAGE,"Found rep point in strip (%d,%d)",
				  nRegionX,nRegionY));
				DPF((DBG_MESSAGE,"rep point in column 1: %d",pbInColumns[1]));
				*/
			}
		}
	}

} /* MarkPointsOnStrips */

#endif  /* METHOD == 3 */


/******************************************************************************
 * Function Name: PlaneRangesInViewport
 *
 * Inputs       : ppPlanes, nNumPlanes, bReturnClosest
 * Outputs      : pfExtremes
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Calculates pfExtremes to be the appropriate extreme depth
 *				  values for each plane within the scope of the viewport.
 *
 *				  Note that the depths are given as (1/depth) values.  Depth
 *				  values will be checked to make sure we don't overflow values.
 *
 *				  Called by RnTestPlanesWithCamera.  This function is derived
 *				  from the RGL planes_depth_ranges().
 *****************************************************************************/
static void PlaneRangesInViewport (
  const TRANSFORMED_PLANE_STRUCT  *ppPlanes[SGL_MAX_INTERNAL_PLANES],
  int  nNumPlanes,
  sgl_bool  bReturnClosest,
  float  pfExtremes[SGL_MAX_INTERNAL_PLANES] )
{
	int	  nPlane;
	float fCentreValue, fHalfChangeX,fHalfChangeY, fInvD,
		  fCFMult;  /* closest/furthest multiplier */
	const TRANSFORMED_PLANE_STRUCT *pThisPlane;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();

	ASSERT(ppPlanes != NULL);
	ASSERT(nNumPlanes >= 0);
	ASSERT(pfExtremes != NULL);

	/*
	// Determine if we want the closest or furthest values.  We do this as a
	// multiplier, because another multiply will probably be cheaper than the
	// delay associated with branches.
	//
	// The accuracy slop factor is to prevent numerical accuracy problems
	// causing objects to be missed out of regions.
	*/
	if (bReturnClosest)
	{
		fCFMult =  1.0f * ACCURACY_SLOP_FACTOR;
	}
	else
	{
		fCFMult = -1.0f * ACCURACY_SLOP_FACTOR;
	}

	for (nPlane=0; nPlane < nNumPlanes; nPlane++)
	{
	 	/*
		// Get access to this plane
		*/
		pThisPlane = ppPlanes[nPlane];

		/*
		// Unless d is ludicrously tiny, get its inverse.  Othersise give it a
		// reasonable magnitude, and get the inverse of that.
		*/
		if (fabs(pThisPlane->d) > VERY_SMALL)
		{
			fInvD = 1.0f / pThisPlane->d;
		}
		else if (pThisPlane->d > 0.0f)
		{
			fInvD = 1.0f / VERY_SMALL;
		}
		else
		{
			fInvD = -1.0f / VERY_SMALL;
		}

		/*
		// ===================================
		// CALCULATE EXTREMES FOR THE VIEWPORT
		// ===================================
		*/
		/* The depth value at the centre of the viewport: */
		fCentreValue =
		  ( pProjMat->VpCentre[X] * pThisPlane->normal[X] +
			pProjMat->VpCentre[Y] * pThisPlane->normal[Y] +
			pThisPlane->normal[Z]
		  ) * fInvD;

		/* Calculate how much the plane distance varies in the viewport: */
		fHalfChangeX =
		  pProjMat->VpHalfWidth[X] * pThisPlane->normal[X] * fInvD;
		fHalfChangeY =
		  pProjMat->VpHalfWidth[Y] * pThisPlane->normal[Y] * fInvD;

		/*
		// ======================
		// STORE REQUIRED EXTREME
		// ======================
		// Get the extreme we want based on the boolean flag by using the
		// multiplier (ie either add or subtract half the change).  Note that
		// closer values are larger.
		*/
		pfExtremes[nPlane] = fCentreValue +
		  fCFMult * ( sfabs(fHalfChangeX) + sfabs(fHalfChangeY) );
	}

} /* PlaneRangesInViewport */



#if METHOD == 3
/*
// Set DERIVE_STRIP TRUE for old method where extremes in first strip are
// derived from those in region 00 (fails to correctly reject some strips).
*/
#define DERIVE_STRIP  TRUE
/******************************************************************************
 * Function Name: PlaneRangesInStrips
 *
 * Inputs       : ppPlanes, nNumPlanes, bReturnClosest
 * Outputs      : pfHorizStrip0Extremes, pfVertStrip0Extremes,
 *				  pfChangesPerHorizStrip, pfChangesPerVertStrip
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Assists in pruning out objects that are not within the scope
 *				  of regions of the display.
 *
 *				  The strips are a single region wide, and the number of
 *				  regions in the appropriate viewport dimension long.  Their
 *				  positions are lengthwise the same as the viewport but for
 *				  simplicity when calculating the offsets for successive strips
 *				  their widthwise position is zero.  The calling function will
 *				  use the change values (see below) to work on subsequent
 *				  strips.
 *
 *				  pfHorizStrip0Extremes and pfVertStrip0Extremes are calculated
 *				  to be the appropriate extreme depth values for each plane
#if DERIVE_STRIP
 *				  within the area of the strip.  These values are both derived
 *				  from internal values calculated for region (0,0).
#else
 *				  within the area of the strip.
#endif
 *
 *				  pfChangesPerHorizStrip and pfChangesPerVertStrip are the
 *				  depth change delta values for each plane from one strip to
 *				  the next in the viewport.  Since the strips are one region
 *				  wide these values also correspond to the changes per single
 *				  region in the vertical and horizontal directions
 *				  respectively.
 *
 *				  Note that the depths are given as (1/depth) values.  Depth
 *				  values will be checked to make sure we don't overflow values.
 *
 *				  Called by RnDetermineRegionsFromPlanes.  This function is
 *				  derived from the RGL planes_depth_ranges().
 *****************************************************************************/
static void  PlaneRangesInStrips (
  const TRANSFORMED_PLANE_STRUCT  *ppPlanes[SGL_MAX_INTERNAL_PLANES],
  int  nNumPlanes,
  sgl_bool  bReturnClosest,
  float  pfHorizStrip0Extremes [SGL_MAX_INTERNAL_PLANES],
  float	 pfVertStrip0Extremes  [SGL_MAX_INTERNAL_PLANES],
  float	 pfChangesPerHorizStrip [SGL_MAX_INTERNAL_PLANES],
  float	 pfChangesPerVertStrip  [SGL_MAX_INTERNAL_PLANES] )
{
	int  nPlane;
	float  fInvD,
		fCFMult, /* closest/furthest multiplier */
#if DERIVE_STRIP
		   fCentreValue, fHalfChangeX,fHalfChangeY,
		   fRegion00Extreme,  /* extreme for region (0,0) */
		   /* extremes for first/last regions in strip 0: */
		   fFirstRegionExtreme, fLastRegionExtreme;
#else
		   fHorizStrip0CentreValue, fVertStrip0CentreValue,
		   fHorizStripChangeToEnd, fVertStripChangeToEnd,
		   fHorizStrip0Extreme, fVertStrip0Extreme; /* for the current plane */
#endif
	const TRANSFORMED_PLANE_STRUCT  *pThisPlane;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();

	ASSERT(ppPlanes != NULL);
	ASSERT(nNumPlanes >= 0);
	ASSERT(pfHorizStrip0Extremes != NULL);
	ASSERT(pfVertStrip0Extremes != NULL);
	ASSERT(pfChangesPerHorizStrip != NULL);
	ASSERT(pfChangesPerVertStrip != NULL);

	/* See PlaneRangesInViewport for comments on this common code: */

	if (bReturnClosest)
	{
		fCFMult =  1.0f * ACCURACY_SLOP_FACTOR;
	}
	else
	{
		fCFMult = -1.0f * ACCURACY_SLOP_FACTOR;
	}

	for (nPlane=0; nPlane < nNumPlanes; nPlane++)
	{
		pThisPlane = ppPlanes[nPlane];

		/*
		// ===============================
		// CALCULATE RECIPROCAL OF D VALUE
		// ===============================
		*/
		if (fabs(pThisPlane->d) > VERY_SMALL)
		{
			fInvD = 1.0f / pThisPlane->d;
		}
		else if (pThisPlane->d > 0.0f)
		{
			fInvD = 1.0f / VERY_SMALL;
		}
		else
		{
			fInvD = -1.0f / VERY_SMALL;
		}


#if DERIVE_STRIP
		/*
		// ================================================================
		// CENTRE DEPTH AND CHANGES TO EDGES FOR THIS PLANE IN REGION (0,0)
		// ================================================================
		*/
		/* The plane's depth value at the centre of the region: */
		fCentreValue =
		  ( pProjMat->fRegion00CentreX * pThisPlane->normal[X] +
			pProjMat->fRegion00CentreY * pThisPlane->normal[Y] +
		    pThisPlane->normal[Z]
		  ) * fInvD;

		/* Calculate how much the depth varies to the edges of the region: */
		fHalfChangeX =
		  pProjMat->fRegionHalfWidth * pThisPlane->normal[X] * fInvD;
		fHalfChangeY =
		  pProjMat->fRegionHalfHeight * pThisPlane->normal[Y] * fInvD;
#else
		/*
		// =========================================================
		// THE PLANE'S DEPTH VALUE AT THE CENTRE OF THE FIRST STRIPS
		// =========================================================
		*/
		fHorizStrip0CentreValue =
		  ( pProjMat->fVPCentreX * pThisPlane->normal[X] +
		    pProjMat->fRegion00CentreY * pThisPlane->normal[Y] +
			pThisPlane->normal[Z]
		  ) * fInvD;

		fVertStrip0CentreValue =
		  ( pProjMat->fRegion00CentreX * pThisPlane->normal[X] +
		    pProjMat->fVPCentreY * pThisPlane->normal[Y] +
			pThisPlane->normal[Z]
		  ) * fInvD;

		/*
		// ==================================================
		// CHANGE IN DEPTH VALUE FROM CENTRE TO END OF STRIPS
		// ==================================================
		*/
		fHorizStripChangeToEnd =
		  pProjMat->fVPHalfWidth * pThisPlane->normal[X] * fInvD;
		fVertStripChangeToEnd =
		  pProjMat->fVPHalfHeight * pThisPlane->normal[Y] * fInvD;

		/*
		// ===========================================
		// CHANGE IN DEPTH VALUE FOR SUBSEQUENT STRIPS
		// ===========================================
		*/
		pfChangesPerHorizStrip[nPlane] =
		  pProjMat->fRegionHeight * pThisPlane->normal[Y] * fInvD;
		pfChangesPerVertStrip[nPlane] =
		  pProjMat->fRegionWidth * pThisPlane->normal[X] * fInvD;
#endif


#if DERIVE_STRIP
		/*
		// =================================
		// REQUIRED EXTREME FOR FIRST REGION
		// =================================
		// Get the extreme we want based on the boolean flag by using the
		// multiplier (ie either add or subtract half the change).  Note
		// that closer values are larger.
		*/
		fRegion00Extreme =
		  fCentreValue + fCFMult * (sfabs(fHalfChangeX) + sfabs(fHalfChangeY));
#else
		/*
		// =====================================
		// REQUIRED EXTREME FOR THE FIRST STRIPS
		// =====================================
		// Get the extremes we want based on the boolean flag by using the
		// multiplier (ie either add or subtract half the change).  Note
		// that closer values are larger.
		*/
		fHorizStrip0Extremes[nPlane] = fCentreValue + fCFMult *
		 (fabs(fHorizStripChangeToEnd) + fabs(pfChangesPerHorizStrip[nPlane]));

		fVertStrip0Extremes[nPlane] = fCentreValue + fCFMult *
		 (fabs(pfChangesPerVertStrip[nPlane]) + fabs(fVertStripChangeToEnd));
#endif


#if DERIVE_STRIP
		/*
		// =================
		// CHANGES PER STRIP
		// =================
		// Successive vertical strips are in the horizontal direction and vice-
		// versa.
		*/
		pfChangesPerVertStrip[nPlane] = 2.0f * fHalfChangeX;
		pfChangesPerHorizStrip[nPlane] = 2.0f * fHalfChangeY;


		/*
		// ============================
		// REQUIRED EXTREMES FOR STRIPS
		// ============================
		*/
		/* horizontal strip */
		fFirstRegionExtreme = fRegion00Extreme +
		  pProjMat->FirstXRegion * pfChangesPerVertStrip[nPlane];
		fLastRegionExtreme = fRegion00Extreme +
		  pProjMat->LastXRegion * pfChangesPerVertStrip[nPlane];

		pfHorizStrip0Extremes[nPlane] = bReturnClosest ?
		  MAX(fFirstRegionExtreme, fLastRegionExtreme) :
		  MIN(fFirstRegionExtreme, fLastRegionExtreme);

		/* vertical strip */
		fFirstRegionExtreme = fRegion00Extreme +
		  pProjMat->FirstYRegion * pfChangesPerHorizStrip[nPlane];
		fLastRegionExtreme = fRegion00Extreme +
		  pProjMat->LastYRegion * pfChangesPerHorizStrip[nPlane];

		pfVertStrip0Extremes[nPlane] = bReturnClosest ?
		  MAX(fFirstRegionExtreme, fLastRegionExtreme) :
		  MIN(fFirstRegionExtreme, fLastRegionExtreme);
#endif

	} /* for (nPlane...) */

} /* PlaneRangesInStrips */


/******************************************************************************
 * Function Name: FindEdge
 *
 * Inputs       : pfStrip0ClosestForwards, pfStrip0FurthestReverses,
 *				  pfForwardChangesPerStrip, pfReverseChangesPerStrip,
 *				  nNumForward, nNumReverse, pbInStrips, nFirstStrip,
 *				  nFirstInvalidStrip, bIncreasing
 * Outputs      : pnPosition
 * Returns      : FALSE iff the object is established to be in none of the
 *				  strips.
 * Globals Used : -
 *
 * Description  : Finds the position of one of the four edges of the minimum
 *				  enclosing rectangle of regions.
 *
 *				  pbInStrips should be a list of flags for each possible strip
 *				  of regions indicating whether a rep point has been projected
 *				  into that strip.
 *
 *				  bIncreasing indicates whether successive strips are in a
 *				  positive or negative direction.
 *
 *				  The return value is via a pointer for efficiency since the
 *				  calling function does the same.
 *
 *				  Called by RnDetermineRegionsFromPlanes.
 *****************************************************************************/
static sgl_bool  FindEdge ( int  *pnPosition,
						    float  pfStrip0ClosestForwards [SGL_MAX_INTERNAL_PLANES],
						    float  pfStrip0FurthestReverses [SGL_MAX_INTERNAL_PLANES],
						    float  pfForwardChangesPerStrip [SGL_MAX_INTERNAL_PLANES],
						    float  pfReverseChangesPerStrip [SGL_MAX_INTERNAL_PLANES],
						    int  nNumForward,
						    int  nNumReverse,
						    sgl_bool  *pbInStrips,
							float fProjectedBackground,
						    int  nFirstStrip,
						    int  nFirstInvalidStrip,
						    sgl_bool  bIncreasing)
{
	int nPreviousExtremeForward, nPreviousExtremeReverse, nPlane,nStrip, nStep;
	/*
	// Extremes of the extremes, i.e. the closest of all the furthest points of
	// the reverse, and the furthest of the closest points of the forward
	// surfaces.
	*/
	float fClosestFurthestReverse, fFurthestClosestForward,
		  fThisExtreme,fBestExtremeSoFar;

	/*
	// Get one of the furthest of the closest forwards and vice-versa for
	// reverse surfaces.  Again, don't look at all of the planes just yet; just
	// do one in every PLANE_STEP planes.
	*/
	/* forward */
	fBestExtremeSoFar = pfStrip0ClosestForwards[0] +
	  (float)nFirstStrip * pfForwardChangesPerStrip[0];

	nPreviousExtremeForward = 0;

	for (nPlane=1; nPlane < nNumForward; nPlane += PLANE_STEP)
	{
		fThisExtreme = pfStrip0ClosestForwards[nPlane] +
		  (float)nFirstStrip * pfForwardChangesPerStrip[nPlane];
		if (fBestExtremeSoFar > fThisExtreme)
		{
		 	fBestExtremeSoFar = fThisExtreme;
			nPreviousExtremeForward = nPlane;
		}
	}

	/* reverse */
	fBestExtremeSoFar = fProjectedBackground;
	nPreviousExtremeReverse = 0;
	for (nPlane=0; nPlane < nNumReverse; nPlane += PLANE_STEP)
	{
		fThisExtreme = pfStrip0FurthestReverses[nPlane] +
		  (float)nFirstStrip * pfReverseChangesPerStrip[nPlane];
		if (fBestExtremeSoFar < fThisExtreme)
		{
		 	fBestExtremeSoFar = fThisExtreme;
			nPreviousExtremeReverse = nPlane;
		}
	}

	nStep = bIncreasing ? 1 : -1;
	/*
	// Step through the region strips, starting with the outermost one, until
	// the object cannot be rejected from the current strip.
	// OPTIMISATION ? THIS MAY BE INEFFICIENT FOR THE FIRST STRIP SINCE WE MAY
	// BE CALCULATING THE RANGE OF DEPTH VALUES TWICE.
	// Convert this into a while loop sometime...
	*/
	for (nStrip = nFirstStrip; nStrip != nFirstInvalidStrip; nStrip += nStep)
	{
		/* Ensure loop is finite in debugging mode at least: */
		ASSERT( nStrip >= 0 && nStrip < MAX(MAX_X_REGIONS,MAX_Y_REGIONS) );

		if (pbInStrips[nStrip])
		{
			/*
			// The rep point was projected into this strip so we have found the
			// position of this edge of the rectangle of regions.
			*/
			/*DPF((DBG_MESSAGE,"hit rep point in strip %d",nStrip));*/
			break;
		}
		/*DPF((DBG_MESSAGE,"checking strip without rep point %d",nStrip));*/

		/*
		// Calculate the depth range, for this strip, of the extreme planes of
		// the previous strip.  Hopefully, this should mean we catch strips of
		// regions where the object is not visible quite early.  Note that
		// closer values are LARGER.
		*/
		/* forward */
		fFurthestClosestForward =
		  pfStrip0ClosestForwards[nPreviousExtremeForward] +
		  (float)nStrip * pfForwardChangesPerStrip [nPreviousExtremeForward];

		/* reverse */
		fThisExtreme = pfStrip0FurthestReverses[nPreviousExtremeReverse] +
		  (float)nStrip * pfReverseChangesPerStrip [nPreviousExtremeReverse];
		fClosestFurthestReverse =
		  MAX(fProjectedBackground, fThisExtreme);

		/*
		// If we cannot immediately class this object as not being within the
		// scope of the strip, then do the depth calculations on the forward
		// planes.
		*/
		if (fClosestFurthestReverse <= fFurthestClosestForward)
		{
			for (nPlane=0; nPlane < nNumForward; nPlane++)
			{
				/* Calculate the depth of this plane: */
				fThisExtreme = pfStrip0ClosestForwards[nPlane] +
				  (float)nStrip * pfForwardChangesPerStrip[nPlane];

				/* Determine whether this plane is the most extreme so far:	*/
				if (fFurthestClosestForward > fThisExtreme)
				{
			 		fFurthestClosestForward = fThisExtreme;
					nPreviousExtremeForward = nPlane;
					/*
					// Since the extreme has been updated it may be possible to
					// reject the object.  If so exit the loop.
					*/
					if (fClosestFurthestReverse > fFurthestClosestForward)
					{
						break;
					}
				}
			}
		}

		/*
		// Do the remaining reverse planes' depth calculations (if we do not
		// already know that the object is outside the strip)
		*/
		if (fClosestFurthestReverse <= fFurthestClosestForward)
		{
			for (nPlane=0; nPlane < nNumReverse; nPlane++)
			{
				fThisExtreme = pfStrip0FurthestReverses[nPlane] +
				  (float)nStrip * pfReverseChangesPerStrip[nPlane];

				if (fClosestFurthestReverse < fThisExtreme)
				{
					fClosestFurthestReverse = fThisExtreme;
					nPreviousExtremeReverse = nPlane;

					if (fClosestFurthestReverse > fFurthestClosestForward)
					{
						break;
					}
				}
			}
		}

		/*
		// If the object has not been rejected from this strip then we have
		// found the required edge.
		*/
		if (fClosestFurthestReverse <= fFurthestClosestForward)
		{
			break;
		}
	}/* for each strip */

	if (nStrip == nFirstInvalidStrip)
	{
		DPF((DBG_MESSAGE,"Infinite object not in any regions"));
		return FALSE;
	}
	else
	{
		*pnPosition = nStrip;
		return TRUE;
	}

} /* FindEdge */

#endif  /* METHOD == 3 */


/*
// ============================================================================
// 							  SGL INTERNAL ROUTINES:
// ============================================================================
*/

/**************************************************************************
* Function Name  : RnDetermineRegionsFromBBox
* Inputs         : Bounding Box
* Outputs        : The regions the object falls in, expressed as a rectangular
*				   block of regions
*
* Input/Output	  :None
* Returns        : None
* Global Used    : Projection matrix values...
*
* Description    :Basically projects the bounding box extremes, using the
*				 values that put them straight into region space.
*
*				Note that if the min values are < 0 and the max values are
*				> 0, then we only need to divide by the closest Z value, otherwise
*				we have to use the two z extremes.
*
**************************************************************************/
sgl_bool  RnDetermineRegionsFromBBox ( const BBOX_MINMAX_STRUCT  *pBBox,
									   REGIONS_RECT_STRUCT  *pRegions )
{
	float ProjXmin, ProjYmin, ProjXmax, ProjYmax;

	float invZmin, invZmax;
	sgl_bool invZmaxValid;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();

	SGL_TIME_START(DETERMINE_BBOX_REGION_TIME)

	/*
	// return false if we are zclipped
	*/
	if (pBBox->boxMax[2] < pProjMat->foregroundDistance)
	{
	    SGL_TIME_STOP(DETERMINE_BBOX_REGION_TIME)
		return (FALSE);
	}

	/*
	// Clip the bounding box to the foreground distance, and get the
	// inverse value
	*/
	if(pBBox->boxMin[2] < pProjMat->foregroundDistance)
	{
		invZmin = pProjMat->invForegroundDistance;
	}
	else
	{
		invZmin = 1.0f / pBBox->boxMin[2];
	}

	/*
	// To save having to decide which is Max and Mins of the project values,
	// I make assumptions about the Sign of the Scaling values. I assume X is
	// positive, but Y is negative. Unless someone changes the coordinate
	// systems this should be ok .. check it to be safe..
	*/
	ASSERT(pProjMat->SxToRegions > 0.0f);
	ProjXmin = pProjMat->SxToRegions * pBBox->boxMin[0];
	ProjXmax = pProjMat->SxToRegions * pBBox->boxMax[0];

	ASSERT( ProjXmin <= ProjXmax);

	/*
	// note change of direction (swap of max and min)
	*/

	/* For optimisation calculate exact Y co-ordinate positions.
	 */
	ASSERT(pProjMat->SyToRegionsExact < 0.0f);
	ProjYmin = pProjMat->SyToRegionsExact * pBBox->boxMax[1];
	ProjYmax = pProjMat->SyToRegionsExact * pBBox->boxMin[1];

	ASSERT( ProjYmin <= ProjYmax);

	/*
	// Are we forced to do two divisions?
	// Check the Y first.
	*/
	if( ProjYmin > 0.0f )
	{

		DPF((DBG_VERBOSE, "Box Regions: Y min > 0"));
		invZmaxValid = TRUE;
		invZmax = 1.0f / pBBox->boxMax[2];

		/* For optimisation calculate exact Y co-ordinate positions.
		 */
		/*
		// NOTE Y Max MUST be > 0, so use closer of the two
		*/
		ProjYmax = ProjYmax * invZmin + pProjMat->OyToRegionsExact;

		/*
		// but use z max for the min.
		*/
		ProjYmin = ProjYmin * invZmax + pProjMat->OyToRegionsExact;
	}
	/*
	// Still forced to do two divisions?
	*/
	else if(ProjYmax < 0.0f)
	{
		DPF((DBG_VERBOSE, "Box Regions: Y Max < 0"));
		
		invZmaxValid = TRUE;
		invZmax = 1.0f / pBBox->boxMax[2];

		/* For optimisation calculate exact Y co-ordinate positions.
		 */
		/*
		// Y Min must be < 0.0
		*/
		ProjYmin = ProjYmin * invZmin + pProjMat->OyToRegionsExact;

		/*
		// but use z max for the max.
		*/
		ProjYmax = ProjYmax * invZmax + pProjMat->OyToRegionsExact;
	}
	/*
	// Else we dont need the second division (yet)
	*/
	else
	{
		DPF((DBG_VERBOSE, "Box Regions: Y centred"));
		
		invZmaxValid = FALSE;
		
		/* For optimisation calculate exact Y co-ordinate positions.
		 */
		ProjYmin = ProjYmin * invZmin + pProjMat->OyToRegionsExact;
		ProjYmax = ProjYmax * invZmin + pProjMat->OyToRegionsExact;
		/*
		// This is here to prevent warnings about possible unitialised
		// value of invZmax, when in fact it is not. The overhead is
		// miniscule
		*/
		invZmax = 0.0f;

	}/*end if else of Y cases*/

	
	/*
	// NOW DO X.
	// Are we forced to do two divisions?
	*/
	if( ProjXmin > 0.0f)
	{
		DPF((DBG_VERBOSE, "Box Regions: X min > 0"));
		if(!invZmaxValid)
		{
			invZmax = 1.0f / pBBox->boxMax[2];
		}
		
		/*
		// NOTE X Max MUST be > 0, so use closer of the two
		*/
		ProjXmax = ProjXmax * invZmin + pProjMat->OxToRegions;
		
		/*
		// but use z max for the min.
		*/
		ProjXmin = ProjXmin * invZmax + pProjMat->OxToRegions;
	}
	/*
	// Else do we have to divide the max by the max distance
	*/
	else if(ProjXmax < 0.0f)
	{
		DPF((DBG_VERBOSE, "Box Regions: X Max < 0"));
		if(!invZmaxValid)
		{
			invZmax = 1.0f / pBBox->boxMax[2];
		}
		
		/*
		// NOTE X Min MUST be < 0, so use closer of the two z's
		*/
		ProjXmin = ProjXmin * invZmin + pProjMat->OxToRegions;
		
		/*
		// but use z max for the max.
		*/
		ProjXmax = ProjXmax * invZmax + pProjMat->OxToRegions;
	}
	/*
	// Else on either side of the middle, so divide both by the min
	*/
	else
	{
		DPF((DBG_VERBOSE, "Box Regions: X centred"));
		ProjXmin = ProjXmin * invZmin + pProjMat->OxToRegions;
		ProjXmax = ProjXmax * invZmin + pProjMat->OxToRegions;
	} /*end X cases*/


	/*
	// Clip these to the legal limits .. add a modicum of slop to
	// these values, say, a very small power of 2, to the regions
	*/
	#define SLOP (1.220703125E-4f)
	pRegions->FirstXRegion =
	  MAX(((int)(ProjXmin - SLOP)), pProjMat->FirstXRegion);
	pRegions->LastXRegion  =
	  MIN(((int)(ProjXmax + SLOP)), pProjMat->LastXRegion);

	/* For optimisation calculate exact Y co-ordinate positions.
	 */
	pRegions->FirstYRegion =
	  MAX(((int)(ProjYmin) - 1), pProjMat->FirstYRegionExact);
	pRegions->LastYRegion  =
	  MIN(((int)(ProjYmax) + 1), pProjMat->LastYRegionExact);
	#undef SLOP	

	SGL_TIME_STOP(DETERMINE_BBOX_REGION_TIME)

	return ((pRegions->FirstXRegion <= pRegions->LastXRegion) && (pRegions->FirstYRegion <= pRegions->LastYRegion));

} /* RnDetermineRegionsFromBBox */



/******************************************************************************
 * Function Name: RnTestBoxWithCamera
 *
 * Inputs       : pBox
 * Outputs      : pbClipFront
 * Returns      : See header file
 * Globals Used : 
 *
 * Description  : See header file
 *****************************************************************************/
TEST_BOX_ENUM  RnTestBoxWithCamera ( const BBOX_MINMAX_STRUCT  *pBox,
								     const sgl_bool  ClassifyPartial,
									 sgl_bool  *pbClipFront )
{
	float fBoxMinZ, fBoxMaxZ;
	float fEdgeLimit;

	/*
	// Put the clipped Z Min and Max values into this array.
	// We then index into it using predetermined values based
	// on whether the viewing plane values are negative or
	// positive.
	//
	// The index constants are defined by the typedef 
	// TB_USE_WHICH_Z_ENUM
	*/
	float ZVals[2];
	PROJECTION_MATRIX_STRUCT *pProjMat;

	pProjMat = RnGlobalGetProjMat();

	/*
	// Check that the constant OFFSET values make sence
	*/
	ASSERT((TB_USE_ZMIN ^ TB_USE_OTHER_Z) == TB_USE_ZMAX);

	/*
	// ==================================================
	// IS BBOX COMPLETELY BEHIND FORWARD CLIPPING PLANE ?
	// ==================================================
	// If so then the object is not visible.
	*/
	fBoxMaxZ = pBox->boxMax[Z];
	if (fBoxMaxZ < pProjMat->foregroundDistance)
	{
		return TB_BOX_OFFSCREEN;
	}

	/*
	// =====================================================
	// IS BBOX COMPLETELY BEYOND BACKGROUND CLIPPING PLANE ?
	// =====================================================
	*/
	fBoxMinZ = pBox->boxMin[Z];
	if (fBoxMinZ > pProjMat->BackgroundDistance)
	{
		return TB_BOX_OFFSCREEN;
	}


	/*
	// Decide if we would have to clip against the front plane, and
	// in fact, do so localy.
	//
	// Report the result to the outside world (through pbClipFront)
	*/
	if (fBoxMinZ < pProjMat->foregroundDistance)
	{
		fBoxMinZ = pProjMat->foregroundDistance;
		*pbClipFront = TRUE;
	}
	else
	{
		*pbClipFront = FALSE;
	}


	/*
	// Optimise for the most common case, where the view pyramid
	// goes round the z axis.
	*/
	if(pProjMat->VPAroundCentre)
	{
		DPF((DBG_MESSAGE, "Doing BBox test with Centred VP"));
		/*
		// ==================================================
		// DOES THE BOUNDING BOX INTERSECT THE VIEW PYRAMID ?
		// ==================================================
		*/

		/*
		// Look at Box X Max VS the left edge of the viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinX * fBoxMaxZ;

		if (pBox->boxMax[X] < fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}

		/*
		// Look at Box X Min   VS  Right edge of Vp
		*/
		fEdgeLimit = pProjMat->fViewportMaxX * fBoxMaxZ;

		if (pBox->boxMin[X] > fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}

		/*
		// box Y Max and Bottom of Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinY * fBoxMaxZ;
	
		if( pBox->boxMax[Y] < fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}


		/*
		// box Y Min and TOP of Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMaxY * fBoxMaxZ;
	
		if(pBox->boxMin[Y] > fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}

		/*
		// ==================================================
		// OK, so its on screen. If we aren't interested in How much is on
		// screen, get out now.
		// ==================================================
		*/
		if(!ClassifyPartial)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// If the front clipping plane is intersected, or the background, then
		// we are part on screen
		*/
		if(*pbClipFront || (fBoxMaxZ > pProjMat->BackgroundDistance))
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// Look at the left edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinX * fBoxMinZ;

		if (pBox->boxMin[X] < fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// Look at the right edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMaxX * fBoxMinZ;

		if (pBox->boxMax[X] > fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// Look at the bottom edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinY * fBoxMinZ;

		if (pBox->boxMin[Y] < fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// Look at the top edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMaxY * fBoxMinZ;

		if (pBox->boxMax[Y] > fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

	}
	/*
	// Else we handle it depending on the values for the pyramid edges
	*/
	else
	{
		DPF((DBG_MESSAGE, "Doing BBox test with NON Centred VP"));
		
		/*
		// Store the Min and max values in an array so we can choose which
		// one to multiply by in a predetermined fashion
		*/
		ZVals[TB_USE_ZMAX] = fBoxMaxZ;
		ZVals[TB_USE_ZMIN] = fBoxMinZ;

		/*
		// ==================================================
		// DOES THE BOUNDING BOX INTERSECT THE VIEW PYRAMID ?
		// ==================================================
		*/

		/*
		// Look at Box X Max VS the left edge of the viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinX * ZVals[pProjMat->VPMinXZI];

		if (pBox->boxMax[X] < fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}

		/*
		// Look at Box X Min   VS  Right edge of Vp
		*/
		fEdgeLimit = pProjMat->fViewportMaxX * ZVals[pProjMat->VPMaxXZI];

		if (pBox->boxMin[X] > fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}


		/*
		// box Y Max and Bottom of Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinY * ZVals[pProjMat->VPMinYZI];
	
		if( pBox->boxMax[Y] < fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}


		/*
		// box Y Min and TOP of Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMaxY * ZVals[pProjMat->VPMaxYZI];
	
		if(pBox->boxMin[Y] > fEdgeLimit)
		{
			return TB_BOX_OFFSCREEN;
		}


		/*
		// ==================================================
		// OK, so its on screen. If we aren't interested in How much is on
		// screen, get out now.
		// ==================================================
		*/
		if(!ClassifyPartial)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// If the front clipping plane is intersected, or the background, then
		// we are part on screen
		*/
		if(*pbClipFront || (fBoxMaxZ > pProjMat->BackgroundDistance))
		{
			return TB_BOX_PART_ONSCREEN;
		}



		/*
		// Look at the left edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinX * 
			   ZVals[pProjMat->VPMinXZI ^ TB_USE_OTHER_Z];

		if (pBox->boxMin[X] < fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// Look at the right edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMaxX * 
			   ZVals[pProjMat->VPMaxXZI ^ TB_USE_OTHER_Z];

		if (pBox->boxMax[X] > fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}



		/*
		// Look at the bottom edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMinY * 
			   ZVals[pProjMat->VPMinYZI ^ TB_USE_OTHER_Z];

		if (pBox->boxMin[Y] < fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

		/*
		// Look at the right edges of the Bounding box and Viewport
		*/
		fEdgeLimit = pProjMat->fViewportMaxY * 
			   ZVals[pProjMat->VPMaxYZI ^ TB_USE_OTHER_Z];

		if (pBox->boxMax[Y] > fEdgeLimit)
		{
			return TB_BOX_PART_ONSCREEN;
		}

	}/*end if/else values for the pyramid edges*/
	

	/*
	// ============
	// Else its completely in the viewing pyramid.
	// ============
	*/
	return TB_BOX_ALL_ONSCREEN;

} /* RnTestBoxWithCamera */


/******************************************************************************
 * Function Name: RnTestPlanesWithCamera
 *
 * Inputs       : pPlanes, nNumPlanes
 * Outputs      : -
 * Returns      : See header file
 * Globals Used : -
 *
 * Description  : See header file
 *****************************************************************************/
sgl_bool  RnTestPlanesWithCamera ( const TRANS_PLANE_ARRAY_TYPE  pPlanes,
								   const int  nNumPlanes )
{
	/*
	// As we process the object, maintain a record of the furthest and closest
	// points of the surfaces within the scope of the viewport.  This
	// information is used to weed out unnecessary objects from the display.
	//
	// fClosestForward and fFurthestReverse are the extreme values for each
	// plane within the view pyramid.  Note that we only require the closest
	// values for the forward planes, and the furthest for the reverse planes.
	*/
	static float pfClosestForward[SGL_MAX_INTERNAL_PLANES],
				 pfFurthestReverse[SGL_MAX_INTERNAL_PLANES];

	/*
	// Extreme values obtained from the above: IE: The closest of all the
	// furthest points of the reverse, and the furthest of the closest points
	// of the forward surfaces.
	*/
	float fClosestFurthestReverse, fFurthestClosestForward,
		  fRepX,fRepY,fRepZ, fProjectedBackground;

	int nPlane, nNumForward=0, nNumReverse=0;

	const TRANSFORMED_PLANE_STRUCT *ppForwardPlanes[SGL_MAX_INTERNAL_PLANES],
								   *ppReversePlanes[SGL_MAX_INTERNAL_PLANES];

	PROJECTION_MATRIX_STRUCT *pProjMat;

	pProjMat = RnGlobalGetProjMat();

	/* OPTIMISATION ? PRECALCULATE THIS VALUE IN THE PROJECTION MATRIX */
	fProjectedBackground =
		    pProjMat->RCentre[Z] * pProjMat->invBackgroundDistance;

	ASSERT(pPlanes != NULL);
	ASSERT(nNumPlanes > 0 && nNumPlanes <= SGL_MAX_INTERNAL_PLANES);

	/*
	// ===================================
	// CHECK FOR REP POINTS IN VIEW VOLUME
	// ===================================
	// If one or more rep points are on or within the view volume then at least
	// part of the object is within the view volume and we can exit straight
	// away.
	*/
	/* OPTIMISATION ??? IS THE OVERHEAD OF THIS GREATER THAN ITS SAVING ? */
	/* OPTIMISATION ? INCREASE THE STEP SIZE SO WE DON'T CHECK ALL PLANES */
	for (nPlane = 0; nPlane < nNumPlanes; nPlane += PLANE_STEP)
	{
		/* OPTIMISATION ? DO pPlanes[nPlane].repPnt ONCE ONLY */
		fRepX = pPlanes[nPlane].repPnt[X];
		fRepY = pPlanes[nPlane].repPnt[Y];
		fRepZ = pPlanes[nPlane].repPnt[Z];
		if (fRepZ >= pProjMat->foregroundDistance &&
		    fRepZ <= pProjMat->BackgroundDistance &&

			fRepX >= pProjMat->fViewportMinX * fRepZ &&
			fRepX <= pProjMat->fViewportMaxX * fRepZ &&

			fRepY >= pProjMat->fViewportMinY * fRepZ &&
			fRepY <= pProjMat->fViewportMaxY * fRepZ)
		{
			return FALSE;
		}
	}

	/*
	// ==================================
	// CREATE FORWARD/REVERSE PLANE LISTS
	// ==================================
	// Build lists of pointers to forward and reverse planes respectively.
	*/
	for (nPlane=0; nPlane < nNumPlanes; nPlane++)
		if (pPlanes[nPlane].d <= 0.0f)
		{
			ppForwardPlanes[nNumForward++] = pPlanes + nPlane;
		}
		else
		{
			ppReversePlanes[nNumReverse++] = pPlanes + nPlane;
		}

	/*
	// ======================================
	// CHECK WHETHER WE ARE INSIDE THE OBJECT
	// ======================================
	// If so then we assume that the object is within the view volume.
	*/
	if (nNumForward == 0)
	{
		return FALSE;
	}
	ASSERT(nNumForward >= 1);

	/*
	// ==================================
	// FIND EXTREME VALUES FOR EACH PLANE
	// ==================================
	*/
	PlaneRangesInViewport(
	  ppForwardPlanes,nNumForward, TRUE /*closest values*/, pfClosestForward );

	/*
	// To simplify matters when we have no reverse planes, set up a 'dummy'
	// plane.
	*/
	if (nNumReverse == 0)
	{
		pfFurthestReverse[0] = fProjectedBackground;
	}
	else
	{
		PlaneRangesInViewport(
		  ppReversePlanes,nNumReverse, FALSE /*furthest*/, pfFurthestReverse );
	}

	/*
	// ============================================
	// DETERMINE WHETHER THE OBJECT CAN BE REJECTED
	// ============================================
	// Just check every PLANE_STEP planes for optimal performance.
	// Note that closer values are larger.
	//
	// Find the most extreme values, ie get the furthest of the closest
	// forwards and vice-versa for the reverse surfaces.
	*/
	fFurthestClosestForward = pfClosestForward[0];
	for (nPlane=1; nPlane < nNumForward; nPlane += PLANE_STEP)
		if (fFurthestClosestForward > pfClosestForward[nPlane])
		{
		 	fFurthestClosestForward = pfClosestForward[nPlane];
		}

	fClosestFurthestReverse = fProjectedBackground;
	for (nPlane=0; nPlane < nNumReverse;  nPlane += PLANE_STEP)
		if (fClosestFurthestReverse < pfFurthestReverse[nPlane])
		{
			fClosestFurthestReverse = pfFurthestReverse[nPlane];
		}

	/* return TRUE if object is definitely outside the viewport */
	return fClosestFurthestReverse > fFurthestClosestForward;

} /* RnTestPlanesWithCamera */


/******************************************************************************
 * Function Name: RnDetermineRegionsFromPlanes
 *
 * Inputs       : pPlaneLists
 * Outputs      : pRegions
 * Returns      : see header file
 * Globals Used : -
 *
 * Description  : see header file and comment for RECURSION_LIMIT
 *
 *				  There are several methods that this function may use
 *				  (determined by the value of METHOD).  It has yet to be
 *				  determined which method is better in general use.  Currently
 *				  only method 3 has been implemented fully.
 *
 *				  Method 0 is the same as that used in RGL, where every region
 *				  is individually tested using the previous horizontal and
 *				  vertical regions' extreme planes in the first instance.
 *
 *				  Method 1 varies in its execution time depending on the shadow
 *				  size and position.  It performs recursive binary subdivision
 *				  of the rectangle of regions (initially the device's regions)
 *				  currently suspected to contain the object.  The subdivision
 *				  is performed on the axis that currently has the larger number
 *				  of regions, or (arbitrarily) the horizontal axis if the
 *				  numbers are equal.  The initial region block suspected to
 *				  contain the object is split into two child region blocks; if
 *				  one of the children is found not to contain the object then
 *				  it can immediately be assumed that the other probably does,
 *				  and so its rejection test need not be carried out before its
 *				  subdivision.
 *
 *				  The best case for method 1, where the object is in just one
 *				  region and all region blocks explicitly tested are found not
 *				  to contain the object is log(xy) time.  If the object is in
 *				  just one region but both child blocks have to be checked at
 *				  each stage then we get 2 log(xy) time.  When 2 or more
 *				  adjacent blocks contain the object all such blocks have to be
 *				  recursively investigated.  The worst case for this is an
 *				  object that completely fills the display: all regions and
 *				  region blocks have to be tested.  Situations where the object
 *				  completely fills a given block of regions can be tested for,
 *				  but this slows down the better cases: to be investigated.
 *
 *				  Method 2 has constant execution time: twice method 1's best
 *				  case, and a small fraction of its worst case.  It separately
 *				  finds the four edges of the smallest enclosing rectangle of
 *				  regions.  Each edge is found by binary subdivision in
 *				  log(number of regions in the dimension) time, resulting in a
 *				  total of 2 log(xy) time.
 *
 *				  Methods 1 and 2 work on the entire device area rather than
 *				  just the viewport (though they may often be the same anyway).
 *				  Things may be speeded up slightly by only considering the
 *				  viewport's regions.  Since we are operating in O(log N) time
 *				  this is not likely to make much difference.
 *
 *				  Having the number of horizontal and vertical regions in the
 *				  device/viewport exactly a power of two would also help.
 *				  Currently the sizes are rounded up to a power of two for
 *				  simplicity.
 *
 *				  Method 3 is a non-recursive version of method 2.  The loss of
 *				  recursion benefit is thought to be outweighed by the fact
 *				  that shadow region rectangles are expected to be large and
 *				  therefore requiring only a limited number of iterations to
 *				  find the edges when starting from the boundary of the regions
 *				  in the viewport.  It also has the benefit of requiring only
 *				  two region rectangle shapes to be calculated for each plane's
 *				  extremes (PlaneRangesInStrips).
 *****************************************************************************/
sgl_bool  RnDetermineRegionsFromPlanes( PLANE_CATEGORIES_STRUCT  *pPlaneLists,
										REGIONS_RECT_STRUCT  *pRegions )
{
	/*
	// The local variables are declared in sections that relate to the code
	// they are first used in.
	*/
	int nPlane, nNumForward=0, nNumReverse=0;
	const TRANSFORMED_PLANE_STRUCT *ppForwardPlanes[SGL_MAX_INTERNAL_PLANES],
								   *ppReversePlanes[SGL_MAX_INTERNAL_PLANES];

#if METHOD == 3
	static sgl_bool pbInColumns[MAX_X_REGIONS], pbInRows[MAX_Y_REGIONS];
	/*
	// Create a record of the furthest and closest points of the forward and
	// reverse planes respectively - on strip of regions by strip of regions
	// basis.  This is used to determine whether the object is likely to be
	// present in a given area of the viewport.  We only require the closest
	// values for the forward planes, and the furthest for the reverse planes.
	//
	// pfHoriz|VertStrip0ClosestForwards and pfHoriz|VertStrip0FurthestReverses
	// are the extreme values for each plane in strips starting from region
	// (0,0) of the display.  The delta values that update the extremes for
	// successive strips are also stored.
	*/
	static float pfHorizStrip0ClosestForwards [SGL_MAX_INTERNAL_PLANES],
				 pfHorizStrip0FurthestReverses [SGL_MAX_INTERNAL_PLANES],
				 pfForwardChangesPerHorizStrip [SGL_MAX_INTERNAL_PLANES],
				 pfReverseChangesPerHorizStrip [SGL_MAX_INTERNAL_PLANES],

				 pfVertStrip0ClosestForwards [SGL_MAX_INTERNAL_PLANES],
				 pfVertStrip0FurthestReverses [SGL_MAX_INTERNAL_PLANES],
				 pfForwardChangesPerVertStrip [SGL_MAX_INTERNAL_PLANES],
				 pfReverseChangesPerVertStrip [SGL_MAX_INTERNAL_PLANES];
#endif
	PROJECTION_MATRIX_STRUCT *pProjMat;


	ASSERT(pPlaneLists->FVPlanes != NULL);
	ASSERT(pPlaneLists->FIPlanes != NULL);
	ASSERT(pPlaneLists->RVPlanes != NULL);
	ASSERT(pPlaneLists->RIPlanes != NULL);
	ASSERT(pPlaneLists->PEPlanes != NULL);
	ASSERT(pPlaneLists->NumFV >= 0 && pPlaneLists->NumFV <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pPlaneLists->NumFI >= 0 && pPlaneLists->NumFI <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pPlaneLists->NumRV >= 0 && pPlaneLists->NumRV <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pPlaneLists->NumRI >= 0 && pPlaneLists->NumRI <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pPlaneLists->NumPE >= 0 && pPlaneLists->NumPE <= SGL_MAX_INTERNAL_PLANES);
	ASSERT(pRegions != NULL);
	/* Only method 3 has been implemented fully at present: */
	ASSERT(METHOD == 3);

	SGL_TIME_START(DETERMINE_PLANES_REGION_TIME)

	pProjMat = RnGlobalGetProjMat();

	/*
	// =================================================
	// 1. CREATE LISTS OF ALL FORWARD AND REVERSE PLANES
	// =================================================
	// --------------
	// Forward planes
	// --------------
	*/
	for (nPlane = 0; nPlane < pPlaneLists->NumFV; nPlane++)
		ppForwardPlanes[nNumForward++] = pPlaneLists->FVPlanes[nPlane];

	for (nPlane = 0; nPlane < pPlaneLists->NumFI; nPlane++)
		ppForwardPlanes[nNumForward++] = pPlaneLists->FIPlanes[nPlane];

	/*
	// --------------
	// Reverse planes
	// --------------
	*/
	for (nPlane = 0; nPlane < pPlaneLists->NumRV; nPlane++)
		ppReversePlanes[nNumReverse++] = pPlaneLists->RVPlanes[nPlane];

	for (nPlane = 0; nPlane < pPlaneLists->NumRI; nPlane++)
		ppReversePlanes[nNumReverse++] = pPlaneLists->RIPlanes[nPlane];

	/*
	// -------------------------
	// Sort perpendicular planes
	// -------------------------
	*/
	for (nPlane = 0; nPlane < pPlaneLists->NumPE; nPlane++)
		if (pPlaneLists->PEPlanes[nPlane]->d <= 0.0f)
		{
			ppForwardPlanes[nNumForward++] = pPlaneLists->PEPlanes[nPlane];
		}
		else
		{
			ppReversePlanes[nNumReverse++] = pPlaneLists->PEPlanes[nPlane];
		}

	/*
	// --------------------------------------
	// Check whether we are inside the object
	// --------------------------------------
	// If so then all regions contain the object.
	*/
	if (nNumForward == 0)
	{
		pRegions->FirstXRegion = pProjMat->FirstXRegion;
		pRegions->LastXRegion  = pProjMat->LastXRegion;

	    /* Calculate absolute Y co-ordinates for D3DOPTIM.
		 */
		pRegions->FirstYRegion = pProjMat->FirstYRegionExact;
		pRegions->LastYRegion  = pProjMat->LastYRegionExact;

		SGL_TIME_STOP(DETERMINE_PLANES_REGION_TIME)

		return TRUE;
	}
	ASSERT(nNumForward >= 1);

	/*
	// ===============================================================
	// 2. FIND REGIONS AND THEIR REGION BLOCKS THAT CONTAIN REP POINTS
	// ===============================================================
	*/
#if METHOD == 3
	/*
	// Clear the structure (to the value of FALSE) to indicate that no rep
	// points have been found yet in any of the region strips.
	*/
	ASSERT(FALSE == 0);
	/* OPTIMISATION ? IS sizeof CALCULATED AT COMPILE TIME? */
	memset(pbInColumns,0,sizeof(pbInColumns));
	memset(pbInRows,0,sizeof(pbInRows));

	MarkPointsOnStrips(ppForwardPlanes, nNumForward, pbInRows,pbInColumns);
	MarkPointsOnStrips(ppReversePlanes, nNumReverse, pbInRows,pbInColumns);
#endif

	/*
	// ============================================================
	// 3. INITIAL DEPTH RANGES AND CHANGES PER REGION OF THE PLANES
	// ============================================================
	*/
#if METHOD == 3
	/*
	// --------------
	// Forward planes
	// --------------
	*/
	PlaneRangesInStrips(ppForwardPlanes, nNumForward,
	  TRUE /*we want closest values*/,
	  pfHorizStrip0ClosestForwards, pfVertStrip0ClosestForwards,
	  pfForwardChangesPerHorizStrip, pfForwardChangesPerVertStrip);

	/*
	// --------------
	// Reverse planes
	// --------------
	// To simplify matters when we have no reverse planes, set up a 'dummy'
	// plane.
	*/
	if (nNumReverse == 0)
	{
		pfHorizStrip0FurthestReverses[0] = pProjMat->fProjectedBackground;
		pfVertStrip0FurthestReverses[0] = pProjMat->fProjectedBackground;

		pfReverseChangesPerHorizStrip[0] = 0.0f;
		pfReverseChangesPerVertStrip[0] = 0.0f;
	}
	else 
	{
		PlaneRangesInStrips(ppReversePlanes, nNumReverse,
		  FALSE /*we want furthest*/,
		  pfHorizStrip0FurthestReverses, pfVertStrip0FurthestReverses,
		  pfReverseChangesPerHorizStrip, pfReverseChangesPerVertStrip);
	}
#endif

	/*
	// =================================================
	// 4. FIND EACH EDGE OF MINIMUM RECTANGLE OF REGIONS
	// =================================================
	// OPTIMISATION (FOR METHOD 3) ? PERHAPS 2 OR ALL 4 INITIAL STRIPS COULD
	// SHARE A COMMON FIRST GUESS FOR EXTREME PLANES
	*/
#if METHOD == 3
	/* OPTIMISATION ? USE A STRUCT FOR PASSING ALL THESE PARAMETERS */
	if (!FindEdge( & pRegions->FirstYRegion,  /* top edge */
				   pfHorizStrip0ClosestForwards,
				   pfHorizStrip0FurthestReverses,
				   pfForwardChangesPerHorizStrip,
				   pfReverseChangesPerHorizStrip,
				   nNumForward,
				   nNumReverse,
				   pbInRows,
				   pProjMat->fProjectedBackground,
				   pProjMat->FirstYRegion,
				   pProjMat->LastYRegion + 1,  /* first invalid strip */
				   TRUE  /* incrementing strips */))
	{
		SGL_TIME_STOP(DETERMINE_PLANES_REGION_TIME)
		return FALSE;
	}

	if (!FindEdge( & pRegions->LastYRegion,  /* bottom edge */
				   pfHorizStrip0ClosestForwards,
				   pfHorizStrip0FurthestReverses,
				   pfForwardChangesPerHorizStrip,
				   pfReverseChangesPerHorizStrip,
				   nNumForward,
				   nNumReverse,
				   pbInRows,
				   pProjMat->fProjectedBackground,
				   pProjMat->LastYRegion,
				   pProjMat->FirstYRegion - 1,
				   FALSE  /* decrementing strips */))
	{
		SGL_TIME_STOP(DETERMINE_PLANES_REGION_TIME)
		return FALSE;
	}

	if (!FindEdge( & pRegions->FirstXRegion,  /* left edge */
				   pfVertStrip0ClosestForwards,
				   pfVertStrip0FurthestReverses,
				   pfForwardChangesPerVertStrip,
				   pfReverseChangesPerVertStrip,
				   nNumForward,
				   nNumReverse,
				   pbInColumns,
				   pProjMat->fProjectedBackground,
				   pProjMat->FirstXRegion,
				   pProjMat->LastXRegion + 1,
				   TRUE))
	{
		SGL_TIME_STOP(DETERMINE_PLANES_REGION_TIME)
		return FALSE;
	}

	if (!FindEdge( & pRegions->LastXRegion,  /* right edge */
				   pfVertStrip0ClosestForwards,
				   pfVertStrip0FurthestReverses,
				   pfForwardChangesPerVertStrip,
				   pfReverseChangesPerVertStrip,
				   nNumForward,
				   nNumReverse,
				   pbInColumns,
				   pProjMat->fProjectedBackground,
				   pProjMat->LastXRegion,
				   pProjMat->FirstXRegion - 1,
				   FALSE))
	{
		SGL_TIME_STOP(DETERMINE_PLANES_REGION_TIME)
		return FALSE;
	}
#endif

#if WIN32 && DEBUG
	/*
	// See comment where these variables are defined at the top of this file.
	*/
	if (nNumForward + nNumReverse > 1)
	{
		gnShadowRgnXMin = pRegions->FirstXRegion;
		gnShadowRgnXMax = pRegions->LastXRegion;
		gnShadowRgnYMin = pRegions->FirstYRegion;
		gnShadowRgnYMax = pRegions->LastYRegion;
	}
#endif

	/* Required for optimisation. Need to scale Y to absolute corrodinates.
	 */
	pRegions->FirstYRegion = pRegions->FirstYRegion * pProjMat->RegionInfo.YSize;
	pRegions->LastYRegion = ((pRegions->LastYRegion + 1) * pProjMat->RegionInfo.YSize)-1;

	SGL_TIME_STOP(DETERMINE_PLANES_REGION_TIME)

	return TRUE;

} /* RnDetermineRegionsFromPlanes */


/*------------------------------- End of File -------------------------------*/
