/**************************************************************************
 * Name         : rncamera.c
 * Title        : Sets up projection matrix global vars stuff from Camera
 * Author       : Simon Fenney
 * Created      : 11 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Sets up projeciton matrix according to the maths
 *				  in TRDD027x.doc
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rncamera.c,v $
 * Revision 1.42  1997/07/14  19:31:18  mjg
 * Added the PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.41  1997/06/20  14:18:25  jop
 * Set up default [3~filter mode to off in projection matrix
 *
 * Revision 1.40  1997/06/04  15:30:50  gdc
 * stopped compiler warnings
 *
 * Revision 1.39  1997/06/04  11:16:00  gdc
 * removed some compiler warnings
 *
 * Revision 1.38  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.37  1997/04/08  15:21:10  gdc
 * and projection_mat is now accessed thru functions not as a global
 *
 * Revision 1.36  1997/04/08  14:38:14  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.35  1997/04/07  11:49:19  gdc
 * put DefaultCamera parameter back as externs are now BAD
 *
 * Revision 1.34  1997/02/04  11:16:48  gdc
 * removed CAMERA parameter form call to SetupDefaultCamera,
 * function now picks up the global variable directly
 *
 * Revision 1.33  1997/02/03  10:26:58  gdc
 * removed the only D3DOPTIM_SGL conditional form this module - as this
 * is now the only build variant
 *
 * Revision 1.32  1996/12/10  23:53:13  erf
 * Changed PVR3 to PCX2.
 *
 * Revision 1.31  1996/12/02  13:00:35  erf
 * D3D optimisation enabled for SGL using D3DOPTIM_SGL compile switch.
 *
 * Revision 1.30  1996/11/29  11:53:26  erf
 * Using D3DOPTIM compile switch for optimisation do:
 * 1. Initialise SyToRegionsExact, OyToRegionsExact, FirstYRegionExact and
 *    LastYRegionExact values. Used to calculate absolute Y co-ordinate values.
 *
 * Revision 1.29  1996/11/07  13:50:11  erf
 * PVR3 compile switch used for PCX2 to implement IEEE floating point support.
 * Changes made:
 * 1. Changed n32FixedProjBackDist & n32FixedClipDist to f32FixedProjBackDist
 *    & f32FixedClipDist.
 * 2. Removed FLOAT_TO_FIXED requirement.
 *
 * Revision 1.28  1995/11/28  11:49:03  pdl
 * altered the CFR calculation.
 *
 * Revision 1.27  1995/11/10  16:14:52  sjf
 * Added computation of planes that form edges of viewing pyramid, for moving
 * the rep point..
 *
 * Revision 1.26  1995/10/17  12:17:29  jrc
 * Region area values are copied into the new struct for a faster way of
 * passing them as function parameters.
 *
 * Revision 1.25  1995/10/07  17:48:04  sjf
 * For the Test Bounding box routine, put in an optimisation for
 * the most common case - where the viewing pyramid lies around the Z axis.
 *
 * Revision 1.24  1995/10/07  15:08:16  sjf
 * Added extras for bounding box rejection to reduce the number
 * of multiplies performed.
 *
 * Revision 1.23  1995/10/04  08:34:44  sjf
 * Added code to support texture space that runs from 0 to 1 rather thab
 * depend on the size of the actual texture.
 *
 * Revision 1.22  1995/09/22  12:25:43  jrc
 * Changed the definition of fCosAnglePerCentralPixel (level of detail).
 *
 * Revision 1.21  1995/09/15  18:09:55  jrc
 * Added fCosAnglePerCentralPixel for level of detail code.
 *
 * Revision 1.20  1995/09/12  14:30:49  sjf
 * float.h was incorrectly #included.
 *
 * Revision 1.19  1995/08/31  13:43:31  jrc
 * Added calculations for:
 * - fProjectedBackground
 * - VpHalfWidth
 * - fRegion00CentreX|Y (renamed from fRegionZeroX|YOffset)
 * - fRegionHalfWidth|Height (renamed from fHalfRegionX|YWidth)
 *
 * Revision 1.18  1995/08/21  16:50:38  sjf
 * Added more values for rejection of infinite convex objects.
 *
 * Revision 1.17  1995/08/21  09:49:56  sjf
 * Added test for forcing fogging to be off.
 *
 * Revision 1.16  1995/08/18  17:05:10  sjf
 * Added correct fogging calculation
 *
 * Revision 1.15  1995/08/16  08:38:31  sjf
 * Fixed incorrect set up of foreground and background clipping plane distances.
 *
 * Revision 1.14  1995/07/30  12:58:40  sjf
 * Added values to support region testing of bounding boxes etc.
 *
 * Revision 1.13  1995/07/29  15:35:49  sjf
 * Fixed the texture CFR value -- I don't know where my brain was when I wrote
 * the original code.....
 *
 * Revision 1.12  1995/07/29  13:34:47  sjf
 * Fixed A&B overflow prevention... wasnt scaling enough.
 *
 * Revision 1.11  1995/07/29  11:25:29  sjf
 * Fixed plane projection offset problem.
 *
 * Revision 1.10  1995/07/28  11:28:28  sjf
 * Added routine to update the local projection matrix.
 *
 * Revision 1.9  1995/07/28  10:59:11  sjf
 * Fixed rescale - was making it larger not smaller!!!.
 *
 * Revision 1.8  1995/07/27  18:34:08  sjf
 * Added assert.
 *
 * Revision 1.7  1995/07/27  11:53:56  sjf
 * Added fixed point background distance, and made default background grey.
 *
 * Revision 1.6  1995/07/27  10:39:44  sjf
 * Fixed calculation of projecte clip distance.
 *
 * Revision 1.5  1995/07/26  16:24:42  sjf
 * Put region calculation into projection matrix.
 *
 * Revision 1.4  1995/07/25  11:34:56  sjf
 * Added device size set up in the projection matrix.
 *
 * Revision 1.3  1995/07/24  14:38:42  sjf
 * Added support for Texturing scaling (CFR Value)
 *
 * Revision 1.2  1995/07/23  16:19:18  sjf
 * Added code to set up all the projection "matrix" values EXCEPT the
 * plane rejection test/ region test stuff.
 *
 * Revision 1.1  1995/07/20  17:59:48  sjf
 * Initial revision
 *
 *
 *
 *
 **************************************************************************/

#define MODULE_ID MODID_RNCAMERA

#include <math.h>
#include "sgl_defs.h"
#include "dlnodes.h"
#include "rnglobal.h"
#include "hwinterf.h"
#include "dvdevice.h"
#include <float.h>
#include "sgl_math.h"


/**************************************************************************
 * Function Name  : RnSetupProjectionMatrix
 * Inputs         : pCamera, pViewport
					  
 * Outputs        :  None
 * Returns        :  None
 * Global Used    : Global Projection "matrix": projection_mat
 * Description    : Initialises the projection values - should be called
 *					at the beginning of a render.
 *					
 **************************************************************************/

#define CD_SCALE_FACTOR (1.0f / 10.0f)

void	RnSetupProjectionMatrix(const CAMERA_NODE_STRUCT *pCamera, 
							 const VIEWPORT_NODE_STRUCT *pViewport)
{
	float Cd;
	int	  CdPower;
	int   FogShift;

	float CamXSize, CamYSize;
	float CamXCentre, CamYCentre;

	float DevXMid, DevYMid;
	float AScale, BScale, AOffset, BOffset;

	int MinPixels;
	int nNumRescaleShifts;

	PROJECTION_MATRIX_STRUCT proj_mat;

	/*////////////////////////
	// Do some safety checks
 	//////////////////////// */
	ASSERT(pCamera->zoom_factor > 0.0001f);
	ASSERT(pCamera->foreground_dist > 1.0E-9f); /*Better not be too small*/


	/* get a copy of the current local proj mat
	** we will stuff all the values in when were finished
	*/
	RnGlobalCopyProjMat(&proj_mat);

	/*////////////////////////////////
	// Because the A&B parameters of the hardware are restricted on their
	// magnitude (compared to C) we have to scale the projection values
	// when we are rendering too few pixels. (NOTE: This is based on the
	// PSEUDO rendered rectangle..)
	//
	// Basically,  PARAM_PRECISION_DIFF says how many bits smaller 
	// (in magnitude) the A&B parameters can be, which governs the
	// the rate of change of the planes. If there are too few pixels,
	// we may overflow, so we scale the range of depths by a power of
	// two accordingly...
	//
	// I.E. C ranges from -1<C<1, so can change by 2, while A&B are at most
	//					-PARAM_PREC..
	//	(in Magnitude) 2			 so make sure we dont exceed this
	// limit. I.E get the min dimensions, and produce a scaling value
	// so we dont' overflow...
	//
	// So that the fogging is also correctly modified, we need to count
	// how many bits down we effectively shift numbers.
	//////////////////////////////// */
	MinPixels = MIN( pViewport->pParentDevice->xDim,
					 pViewport->pParentDevice->yDim);

	proj_mat.fOverflowRescale = 1.0f;
	nNumRescaleShifts = 0;
	while(MinPixels < (1<<(PARAM_PRECISION_DIFF +1)))
	{
		proj_mat.fOverflowRescale *= 0.5f;
		MinPixels *= 2;
		nNumRescaleShifts ++;
	}/*end while*/


	/* ////////////////////////
	// Calculate the Camera D value, which controls "Z scaling".
	// and the gradual changes in fogging. First let it be a fraction of
	// the foreground clipping distance.
	//////////////////////// */
	Cd = pCamera->foreground_dist * CD_SCALE_FACTOR;

	/*
	// Extract the power of 2
	*/
	frexp(Cd, &CdPower);

	/*
	// Adjust the Cd value based on the fog, and compute the fog
	// shift value based on the Cd and the fog denisty
	*/
	ASSERT(pCamera->invlogfogFraction >= 1.0f);
	ASSERT(pCamera->invlogfogFraction <= 2.0f);

	/*
	// Modify Cd by the fogging fraction
	*/
	Cd *= pCamera->invlogfogFraction;


	/*
	// Define how much we should shift up by so that the depth
	// range of the fog function is sort of in the right range.
	// 
	// The fog function works on the lower 10 bits of the depth
	// value (which I believe in Sabre will be 32 bits at that point)
	// so calculate how much to shift down so we get 0-1 in a 10 bit
	// number.
	*/
	#define FOG_OFFSET (32 - 10)

	/*
	// Calculate how much shifting we have to do, but clamp to the
	// legal range.
	//
	// Need to add the Cd (binary) order of magnitude to the cameras fog,
	// add the overall offset for where the fog function is computed from
	// but take off the rescaling that gets done during projection.
	*/
	FogShift = CdPower + pCamera->logfogPower + FOG_OFFSET - nNumRescaleShifts;

	/*
	// If there is no (or virtually no) fogging, turn it off completely
	*/
	if(FogShift < 0)
	{
		proj_mat.FogShift = 0;
		proj_mat.FogOn = FALSE;
	}
	else
	{
		proj_mat.FogShift = MIN(FogShift, 31);
		proj_mat.FogOn = TRUE;
	}

	DPF((DBG_MESSAGE, "Cd Power:%d  FogPower:%d",CdPower, pCamera->logfogPower));
	DPF((DBG_MESSAGE, "Fog Fraction:%f", pCamera->invlogfogFraction));
	DPF((DBG_MESSAGE, "Shift Value %d", proj_mat.FogShift));
	DPF((DBG_MESSAGE, "FOG ON?: %d", proj_mat.FogOn));
	
	/*
	// Also, don't allow the camera rectangle to be too small..
	// dont let the dimensions get less than a 1/10th of a pixel (say)
	// (note negative direction on y)
	*/
	CamXSize =  MAX(0.1f, pViewport->fCamRight - pViewport->fCamLeft);
	CamYSize =  MAX(0.1f, pViewport->fCamBottom - pViewport->fCamTop);
	CamXCentre =(pViewport->fCamRight + pViewport->fCamLeft) *0.5f;
	CamYCentre =(pViewport->fCamBottom + pViewport->fCamTop) *0.5f;
	 
	/*
	// Compute the global projection matrix values...
	*/
	proj_mat.SxDash=  pCamera->zoom_factor * 0.5f * CamXSize;
	proj_mat.SyDash= -pCamera->zoom_factor * 0.5f * CamYSize; 
	proj_mat.OxDash= CamXCentre;
	proj_mat.OyDash= CamYCentre;


	/*
	// get device dimensions
	*/
	proj_mat.fDevXMax = ((float)pViewport->pParentDevice->xDim) - 1.0f;
	proj_mat.fDevYMax = ((float)pViewport->pParentDevice->yDim) - 1.0f;


	/*
	// Compute the values needed to project the infinite planes...
	//
	// get the "mid point" of the device (Note the "odd" pixel
	// coordinate system)
	*/
	DevXMid = proj_mat.fDevXMax * 0.5f;
	DevYMid = proj_mat.fDevYMax * 0.5f;

	AScale =  (2.0f * Cd) /(CamXSize * pCamera->zoom_factor);
	AOffset= -(2.0f * CamXCentre * Cd) / (CamXSize * pCamera->zoom_factor);

	BScale = -(2.0f * Cd) /(CamYSize * pCamera->zoom_factor);
	BOffset=  (2.0f * CamYCentre * Cd) / (CamYSize * pCamera->zoom_factor);
		
	/*
	// Store the distances between pixels...
	*/
	proj_mat.xPerPixel =AScale; 
	proj_mat.yPerPixel =BScale;
	
	/*
	// Compute the distance from the centre to the corners
	*/
	proj_mat.xToCorner = AScale * DevXMid;
	proj_mat.yToCorner = BScale * DevYMid;

	/*
	// get the vector from the origin to the centre point (in World Coords)
	*/
	proj_mat.RCentre[0] = DevXMid * AScale + AOffset;
	proj_mat.RCentre[1] = DevYMid * BScale + BOffset;
	proj_mat.RCentre[2] = Cd;



	/*////////////////////////
	//
	// Compute "Normals" for the planes which form the edges of the
	// viewing Pyramid,
	//
	// These are computed by taking cross prods of vectors in the planes.
	// Since the origin is in the planes, then the vectors to the corners
	// of the screen are suitable.
	//
	// This could be much more efficient, but who cares...
	//////////////////////// */
	{
		sgl_vector Corners[4];
		int i;

		for(i=0; i < 4; i++)
		{
			Corners[i][0] = proj_mat.RCentre[0];
			Corners[i][1] = proj_mat.RCentre[1];
			Corners[i][2] = proj_mat.RCentre[2];
		}

		/*
		// Do the X offsets
		*/
		Corners[0][0] -= proj_mat.xToCorner;
		Corners[1][0] += proj_mat.xToCorner;
		Corners[2][0] += proj_mat.xToCorner;
		Corners[3][0] -= proj_mat.xToCorner;

		/*
		// Do the Y offsets
		*/
		Corners[0][1] += proj_mat.yToCorner;
		Corners[1][1] += proj_mat.yToCorner;
		Corners[2][1] -= proj_mat.yToCorner;
		Corners[3][1] -= proj_mat.yToCorner;
		
		/*
		// Compute the normals, and normalise, so that all are
		// "fair" when we compare which is most suitable
		*/
		CrossProd(Corners[0], Corners[1], proj_mat.EdgeNormals[0]);
		CrossProd(Corners[1], Corners[2], proj_mat.EdgeNormals[1]);
		CrossProd(Corners[2], Corners[3], proj_mat.EdgeNormals[2]);
		CrossProd(Corners[3], Corners[0], proj_mat.EdgeNormals[3]);

		VecNormalise(proj_mat.EdgeNormals[0]);
		VecNormalise(proj_mat.EdgeNormals[1]);
		VecNormalise(proj_mat.EdgeNormals[2]);
		VecNormalise(proj_mat.EdgeNormals[3]);
	}


	/*////////////////////////
	//
	//  Compute specialised foreground and background distances.
	//
	//////////////////////// */
	/*
	// Store the foreground and background distances, so that we don't
	// have to go looking for them again.
	// Check if it is safe to take the inverse of the background distance,
	// else choose a V Large number instead.
	*/
	ASSERT(pCamera->foreground_dist >= 0.0f)
	ASSERT(pCamera->inv_background_dist >= 0.0f)

	proj_mat.foregroundDistance	 = pCamera->foreground_dist;
	if(proj_mat.foregroundDistance < 1.0E-10f)
	{
		proj_mat.invForegroundDistance= 1.0E10f;
	}
	else
	{
		proj_mat.invForegroundDistance= 
				1.0f /proj_mat.foregroundDistance;
	}


	proj_mat.invBackgroundDistance = pCamera->inv_background_dist;

	if(proj_mat.invBackgroundDistance != 0.0f)
	{
		proj_mat.BackgroundDistance = 
			1.0f / proj_mat.invBackgroundDistance;
	}
	else
	{
		proj_mat.BackgroundDistance = FLT_MAX;
	}

	/* The projected background distance for rnreject.c: */
	proj_mat.fProjectedBackground =
	  proj_mat.RCentre[2] * proj_mat.invBackgroundDistance;


	/*
	// Compute the distance to the foreground clipping plane in projected
	// distance, and also store this in fixed point format.
	*/
	ASSERT(pCamera->foreground_dist > Cd);
	proj_mat.fProjectedClipDist = Cd / pCamera->foreground_dist *
									  proj_mat.fOverflowRescale;

	ASSERT(proj_mat.fProjectedClipDist > 0.0f);

#if PCX2 || PCX2_003
	/* Use floating point numbers for PCX2 family */
	proj_mat.f32FixedClipDist = proj_mat.fProjectedClipDist;

	DPF((DBG_MESSAGE, "Fixed clip Dist:%f", proj_mat.f32FixedClipDist));

	proj_mat.f32FixedProjBackDist = 
				(Cd * pCamera->inv_background_dist * proj_mat.fOverflowRescale);
#else
	proj_mat.n32FixedClipDist = 
				(sgl_int32)(proj_mat.fProjectedClipDist * FLOAT_TO_FIXED);

	DPF((DBG_MESSAGE, "Fixed clip Dist:%lx", proj_mat.n32FixedClipDist));

	proj_mat.n32FixedProjBackDist = 
				(sgl_int32)(Cd * pCamera->inv_background_dist 
				 * proj_mat.fOverflowRescale* FLOAT_TO_FIXED);
#endif

	/*////////////////////////
	// Work out where the viewport maps to on the z=1 plane
	// in world space. This is used by the bounding box rejection
	// code.
	//
	// This is can computed from the inverse of the 
	// descaled projection matrix... as given in TRDD027x.doc
	// (This code could be more efficient, but its only run
	// once per scene...) (Simon Fenney)
	//
	//////////////////////// */
	proj_mat.fViewportMinX = 
					(((float)pViewport->Left) - proj_mat.OxDash)
					/ proj_mat.SxDash;

	proj_mat.fViewportMaxX = 
					(((float)pViewport->Right) - proj_mat.OxDash)
					/ proj_mat.SxDash;

	proj_mat.fViewportMinY = 
					(((float)pViewport->Bottom) - proj_mat.OyDash)
					/ proj_mat.SyDash;

	proj_mat.fViewportMaxY = 
					(((float)pViewport->Top) - proj_mat.OyDash)
					/ proj_mat.SyDash;
	/*
	// Make sure the Y's are around the right way (note change
	// of direction
	*/
	ASSERT(proj_mat.fViewportMinY < proj_mat.fViewportMaxY)

	/*
	// For efficiency in the bounding box rejection code, calculate an index
	// into an array of 2 Z values (Min and Max). This depends on the sign
	// of the values
	//
	// In most cases, the view pyramid goes round the Z axis, so
	// note that as a special case, and optimise it.
	*/
	proj_mat.VPAroundCentre = TRUE;
	if(proj_mat.fViewportMinX <= 0.0f)
		proj_mat.VPMinXZI = TB_USE_ZMAX;
	else
	{
		proj_mat.VPMinXZI = TB_USE_ZMIN;
		proj_mat.VPAroundCentre = FALSE;
	}

	if(proj_mat.fViewportMaxX <= 0.0f)
	{
		proj_mat.VPMaxXZI = TB_USE_ZMIN;
		proj_mat.VPAroundCentre = FALSE;
	}
	else
		proj_mat.VPMaxXZI = TB_USE_ZMAX;


	if(proj_mat.fViewportMinY <= 0.0f)
		proj_mat.VPMinYZI = TB_USE_ZMAX;
	else
	{
		proj_mat.VPMinYZI = TB_USE_ZMIN;
		proj_mat.VPAroundCentre = FALSE;
	}

	if(proj_mat.fViewportMaxY <= 0.0f)
	{
		proj_mat.VPMaxYZI = TB_USE_ZMIN;
		proj_mat.VPAroundCentre = FALSE;
	}
	else
		proj_mat.VPMaxYZI = TB_USE_ZMAX;




	proj_mat.VpCentre[0] = 
		(proj_mat.fViewportMinX + proj_mat.fViewportMaxX) * 0.5f;

	proj_mat.VpCentre[1] = 
		(proj_mat.fViewportMinY + proj_mat.fViewportMaxY) * 0.5f;

	proj_mat.VpOffset[0] = 
	  	(proj_mat.fViewportMaxX - proj_mat.fViewportMinX) * 0.5f;

	proj_mat.VpOffset[1] = 
	  	(proj_mat.fViewportMaxY - proj_mat.fViewportMinY) * 0.5f;

	proj_mat.VpHalfWidth[0] =
	  proj_mat.VpCentre[0] - proj_mat.fViewportMinX;

	proj_mat.VpHalfWidth[1] =
	  proj_mat.VpCentre[1] - proj_mat.fViewportMinY;

	/* ////////////////////////
	// Grab the region info for this device
	//////////////////////// */
	HWGetRegionInfo(pViewport->pParentDevice->PhDeviceID,
					&proj_mat.RegionInfo);

	/*
	// Using this data, compute the regions for the viewport
	*/
	proj_mat.FirstXRegion= pViewport->Left / proj_mat.RegionInfo.XSize;
	proj_mat.LastXRegion = pViewport->Right/ proj_mat.RegionInfo.XSize;

	proj_mat.FirstYRegion= pViewport->Top / proj_mat.RegionInfo.YSize;
	proj_mat.LastYRegion = pViewport->Bottom/ proj_mat.RegionInfo.YSize;
	proj_mat.RegionsRect.FirstXRegion = proj_mat.FirstXRegion;
	proj_mat.RegionsRect.LastXRegion  = proj_mat.LastXRegion;
	proj_mat.RegionsRect.FirstYRegion = proj_mat.FirstYRegion;
	proj_mat.RegionsRect.LastYRegion  = proj_mat.LastYRegion;

	/*
	// Also work out projection from world space to region space...
	// Just divide the SxDash etc values by the respective size of regions
	// in pixels..
	*/
	proj_mat.SxToRegions =  proj_mat.SxDash / 
							(float) proj_mat.RegionInfo.XSize;
	proj_mat.OxToRegions = proj_mat.OxDash / 
							(float) proj_mat.RegionInfo.XSize;

	proj_mat.SyToRegions =  proj_mat.SyDash /
							(float) proj_mat.RegionInfo.YSize;
	proj_mat.OyToRegions = proj_mat.OyDash / 
							(float) proj_mat.RegionInfo.YSize;

	/* For optimisation new variables are required. These are
	 * in absolute co-ordinates and are used to calculate the Y
	 * position in absolute co-ordinates. This is required by the
	 * the optimised AddRdegionX..() routines.
	 */
	proj_mat.SyToRegionsExact = proj_mat.SyDash;
	proj_mat.OyToRegionsExact = proj_mat.OyDash;
	
	proj_mat.FirstYRegionExact = 	proj_mat.FirstYRegion *
										proj_mat.RegionInfo.YSize;
	proj_mat.LastYRegionExact	= ((proj_mat.LastYRegion + 1) *
										proj_mat.RegionInfo.YSize) - 1;

	/*
	// Converting region space into projected world space:
	*/
	proj_mat.fRegionHalfWidth = 0.5f / proj_mat.SxToRegions;
	proj_mat.fRegionHalfHeight = 0.5f / proj_mat.SyToRegions;
	proj_mat.fRegion00CentreX =
	  (0.5f - proj_mat.OxToRegions) / proj_mat.SxToRegions;
	proj_mat.fRegion00CentreY =
	  (0.5f - proj_mat.OyToRegions) / proj_mat.SyToRegions;

	/**************************
	// Compute values needed by the texturing..
	//
	// It effectively wants how far from the "screen" the viewpoint is, 
	// measured in pixels, to use as a scaling factor on the parameters so
	// that they are approximately in the right range.
	// I THINK that I should get the average dimension of the camera rectangle,
	// and multiply that by the zoom factor to get the best approx distance.
	// PLUS got to clip to the allowed range.
	**************************/
	proj_mat.n32CFRValue = 
			CLAMP( (sgl_int32)( ((CamXSize + CamYSize) / 4.0f) * pCamera->zoom_factor),
					 1L, MAX_CFR_VALUE);
							   
	proj_mat.CFRreciprocal = 1.0f /((float)proj_mat.n32CFRValue);

	/**************************
	// Approximate single pixel value for level of detail selection.  See
	// comments in rnlod.c
	//
	// Assumes that the pixel width is much shorter than RCentre[2] because its
	// angle to the camera tends to PI/2 as the pixel size tends to infinity
	// instead of tending to PI.  (For 'normal' small pixels the angle will be
	// accurate.)
	//
	// Since the value we require is very close to 1.0 we store the difference
	// from 1.0 (which must be calculated using double precision) instead of
	// the actual value to enable sufficient precision in the float.  Values
	// close to zero can benefit from the exponent whereas values close to 1.0
	// cannot.
	//
	// The corresponding values for other pixel sizes are found using the fact
	// that for small angles the shape of the cosine curve is close to an
	// inverted square function which is much faster to calculate.  The offset
	// from 1.0 for the cosine of the angle is approximated by the value
	// calculated below for one pixel multiplied by the square of the number of
	// pixels.
	**************************/
	proj_mat.fCosAnglePerCentralPixel = (float)(1.0 - cos(atan2(
	  (double)proj_mat.xPerPixel, (double)proj_mat.RCentre[2] )));

	/*
	// set filter type to a default value
	*/
	proj_mat.eFilterType = sgl_tf_point_sample;

	RnGlobalSetProjMat(&proj_mat);

	DPF((DBG_MESSAGE, "Done Projection Matrix set up"));
}

/**************************************************************************
 * Function Name  : RnSetupDefaultCamera
 * Inputs         : pDefaultCamera
 * Outputs        : None
 * Returns        : None
 * Global Used    : None
 * Description    :	Creates a default camera for time when the user has not
 *					specified a camera.
 *					This probably should be called either at system 
 *					initialisation, or (less efficiently) at the start of
 *					some renders...
 *					
 **************************************************************************/

void RnSetupDefaultCamera(CAMERA_NODE_STRUCT *pDefaultCamera)
{
	/*
	// Set up header info. We don't want this accidently appearing
	// in any display lists, so put illegal values in...
	*/
	#if DEBUG
		pDefaultCamera->node_hdr.n16_node_type = -1;
		pDefaultCamera->node_hdr.n16_name = -1;
		pDefaultCamera->node_hdr.next_node = NULL;
	#endif
	

	/*
	// no parent pointer so set this to null also
	*/
	#if DEBUG
		pDefaultCamera->pparent = NULL;
	#endif
	

	/*
	// Define the fields needed for a basic camera
	*/
	pDefaultCamera->zoom_factor = 4.0f; /* as specified by SGL */
	
	pDefaultCamera->foreground_dist = 1.0f;
	pDefaultCamera->inv_background_dist = 0.0f;

	/*
	// Background colour
	*/
	pDefaultCamera->backgroundColour[0] = 0.5f;
	pDefaultCamera->backgroundColour[1] = 0.5f;
	pDefaultCamera->backgroundColour[2] = 0.5f;


	/*
	// Fog parameters:
	*/
	pDefaultCamera->FogCol.red    =  0;
	pDefaultCamera->FogCol.green  =  0;
	pDefaultCamera->FogCol.blue   =  0;

	/* the following should indicate no fog */
	pDefaultCamera->invlogfogFraction = 1.0f;                
	pDefaultCamera->logfogPower = -1000;


	DPF((DBG_MESSAGE, "Set up Default Camera"));
}




/**************************************************************************
 * Function Name  : RnUpdateLocalProjectionMatrix
 * Inputs         : pTransformState
 * Outputs        :  None
 * Returns        :  None
 * Global Used    : Local Projection "matrix": and projection_mat
 * Description    : Re-evaluates the local proj mat from the
 *					projection matrix and the current transform state.
 *					(SEE TRDD027?.doc)
 *
 *					Also updates the "texture" projection matrix which is
 *					like the above but also includes scaling information
 *					to map 0..1 textures into the correct size for each
 *					resolution texture.
 **************************************************************************/
void RnUpdateLocalProjectionMatrix(const TRANSFORM_STRUCT *pTransform)
{
	
	float Scale, Offset;
	int i;
	LOCAL_PROJECTION_STRUCT  LocalProjMat,*pLocalProjMat;
	PROJECTION_MATRIX_STRUCT *pProjMat;

	pLocalProjMat = RnGlobalGetLocalProjMat();
	pProjMat = RnGlobalGetProjMat();

	LocalProjMat.LastTextureSize = pLocalProjMat->LastTextureSize;

	/*
	// Do the first row... 
	*/
	Scale =  pProjMat->SxDash;
	Offset = pProjMat->OxDash;

	/*
	// Leave it up to the compiler to unroll this loop...
	*/
	for(i = 0; i < 4; i ++)
	{
		LocalProjMat.sr[0][i] = 
			Scale  * pTransform->mat[0][i] +
			Offset * pTransform->mat[2][i];
	}

	/*
	// Do row 2
	*/
	Scale =  pProjMat->SyDash;
	Offset = pProjMat->OyDash;

	/*
	// Leave it up to the compiler to unroll this loop as well
	*/
	for(i = 0; i < 4; i ++)
	{
		LocalProjMat.sr[1][i] = 
			Scale  * pTransform->mat[1][i] +
			Offset * pTransform->mat[2][i];
	}

	/*
	// mark the local proj matrix as valid.
	*/	
	LocalProjMat.valid = TRUE;


	/*
	// Update the special texture matrix as well
	*/
	Scale = 1.0f / (float) LocalProjMat.LastTextureSize;

	for(i = 0; i < 3; i++)
	{
		LocalProjMat.textureSR[0][i] = Scale * LocalProjMat.sr[0][i];
		LocalProjMat.textureSR[1][i] = Scale * LocalProjMat.sr[1][i];
		LocalProjMat.textureSR[2][i] = Scale * pTransform->mat[2][i];
	}

	RnGlobalSetLocalProjMat(&LocalProjMat);
}

/**************************************************************************
 * Function Name  : RnRescaleLocalProjectionTexture
 * Inputs         : pTransformState
 * Outputs        :  None
 * Returns        :  None
 * Global Used    : Local Projection "matrix": and projection_mat
 * Description    : Given a new texture size, this rescales the texture
 *					projecting bit of the local projection matrix.
 **************************************************************************/
void RnRescaleLocalProjectionTexture(const int TextureSize,
						const TRANSFORM_STRUCT *pTransform)
{
	
	float Scale;
	int i;

	/*
	// Update the special texture matrix
	*/
	LOCAL_PROJECTION_STRUCT  *pLocalProjMat;

	pLocalProjMat = RnGlobalGetLocalProjMat();

	pLocalProjMat->LastTextureSize = TextureSize;

	Scale = 1.0f / (float) TextureSize;

	for(i = 0; i < 3; i++)
	{
		pLocalProjMat->textureSR[0][i] = 
						Scale * pLocalProjMat->sr[0][i];

		pLocalProjMat->textureSR[1][i] = 
						Scale * pLocalProjMat->sr[1][i];

		pLocalProjMat->textureSR[2][i] = 
						Scale * pTransform->mat[2][i];
	}
}


/*--------------------------- End of File --------------------------------*/

