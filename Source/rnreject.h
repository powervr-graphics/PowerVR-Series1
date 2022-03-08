/******************************************************************************
 * Name         : rnreject.h
 * Title        : Rejection testing for convex primitives.
 * Author       : John Catchpole
 * Created      : 10/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Internal routines for rejection testing of convex primitives:
 *				  bounded or unbounded respectively.  The primitives are
 *				  checked for probable presence in the view pyramid.  Called by
 *				  rnconvex.c
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnreject.h,v $
 * Revision 1.11  1995/10/26  14:07:52  jop
 * GetRegionsFromBBox now returns TRUE for onscreen FALSE for offscreen
 *
 * Revision 1.10  1995/10/23  15:47:05  jrc
 * Added boolean return value to RnDetermineRegionsFromPlanes.
 *
 * Revision 1.9  1995/10/17  12:43:03  jrc
 * For speed (less use of the stack) the rectangles of regions are now passed
 * as a struct instead of four separate ints.
 *
 * Revision 1.8  1995/10/09  15:29:29  jrc
 * It is now ok to pass no forward planes to RnDetermineRegionsFromPlanes
 * (just removed a comment).  This is for the case where we are inside the
 * infinite object.
 *
 * Revision 1.7  1995/10/07  17:50:52  sjf
 *  Moved Determine regions from BBox from rnconvpr to more common
 *  location so that it can be used by meshes.
 *
 * Revision 1.6  1995/10/07  15:11:12  sjf
 * Updated test box with camera to classify the box into one of 3 sets...
 * Offscreen, partly O, Completely ON.
 *
 * Revision 1.5  1995/09/07  12:13:48  jrc
 * Changed RnDetermineRegionsFromPlanes to pass planes in a single struct.
 *
 * Revision 1.4  1995/08/31  13:48:43  jrc
 * Added prototype for RnDetermineRegionsFromPlanes.
 *
 * Revision 1.3  1995/07/30  10:56:06  sjf
 * Changed typedef name of bounding box.
 *
 * Revision 1.2  1995/07/26  12:45:38  jrc
 * Fixed bad parameter pointer.
 *
 * Revision 1.1  1995/07/26  12:43:53  jrc
 * Initial revision
 *
 *****************************************************************************/

#ifndef __RNREJECT_H
#define __RNREJECT_H


#include "pmsabre.h"  /* for REGIONS_RECT_STRUCT */


/*
// --------------------------
// RnDetermineRegionsFromBBox
// --------------------------
// From a Bounding box, get the screen regions covered.
*/

/* returns TRUE if BBox is on-screen, FALSE if completely off */
sgl_bool RnDetermineRegionsFromBBox(const BBOX_MINMAX_STRUCT *pBBox,
								 	REGIONS_RECT_STRUCT	  *pRegions);


/*
// -------------------
// RnTextBoxWithCamera
// -------------------
// The bounding box is assumed to be in world space (camera axis-aligned).
//
// IF Classify Partial is TRUE then it returns...
//
//  	TB_BOX_OFFSCREEN iff the bounding box is entirely outside of the
//							view pyramid.
//		TB_BOX_PART_ONSCREEN if the box partly intersects the view pyramid
//
//		TB_BOX_ALL_ONSCREEN if the box is entirely in the view pyramid
//
// IF Classify Partial is FALSE
//	  then for both TB_BOX_PART_ONSCREEN and TB_BOX_ALL_ONSCREEN it returns
//	  TB_BOX_PART_ONSCREEN.  This takes less time than checking if its all
//    in the viewport.
// 
*/

typedef enum
{
	TB_BOX_OFFSCREEN,
	TB_BOX_PART_ONSCREEN,
	TB_BOX_ALL_ONSCREEN
} TEST_BOX_ENUM;

TEST_BOX_ENUM RnTestBoxWithCamera( const BBOX_MINMAX_STRUCT *pBox, 
								   const sgl_bool ClassifyPartial,
								   sgl_bool *pbClipFront);


/*
// ----------------------
// RnTestPlanesWithCamera
// ----------------------
// Returns TRUE iff the primitive is entirely outside of the view pyramid.
//
// pPlanes: List of planes transformed into world space.
//
// The return value is undefined if the camera is inside the object.
//
// It is assumed that there is at least one forward plane.  There may be zero
// reverse planes.
*/
sgl_bool RnTestPlanesWithCamera( const TRANS_PLANE_ARRAY_TYPE pPlanes,
								 const int					  nNumPlanes);


/*
// ----------------------------
// RnDetermineRegionsFromPlanes
// ----------------------------
// Finds the smallest rectangle of regions in the image that contains the
// object.
//
// The input planes are assumed to have been transformed into world
// coordinates.
//
// Invisible and perpendicular planes are treated as regular forward or reverse
// planes.
//
// Returns FALSE iff the object is not in any regions.
*/
sgl_bool RnDetermineRegionsFromPlanes( PLANE_CATEGORIES_STRUCT *pPlaneLists,
									   REGIONS_RECT_STRUCT	   *pRegions);


#endif

/*------------------------------- End of File -------------------------------*/
