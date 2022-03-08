/**************************************************************************
 * Name         : rnglobal.h
 * Title        : Extern definitions of the rendering global vars (other than
 *				  state variables).
 * Author       : Simon Fenney
 * Created      : 11 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Defines the SGL render globals as defined in TRDD027x.doc
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnglobal.h,v $
 * Revision 1.62  1997/09/24  14:21:12  gdc
 * added new global for API developers
 *
 * Revision 1.61  1997/09/11  10:25:03  erf
 * Added bDithering field for SGL. Could be used for SGLDirect but
 * isn't because the setting is defined in the context flags.
 *
 * Revision 1.60  1997/07/14  19:39:29  mjg
 * Added PCX2_003 compile switch.
 *
 * Revision 1.59  1997/05/09  09:23:00  erf
 * Moved AddDummyPlanesL() function prototype to pkisp.h.
 *
 * Revision 1.58  1997/05/08  08:28:07  erf
 * Added function prototype for function to get fixed clipping distance.
 *
 * Revision 1.57  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.56  1997/04/21  20:01:20  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.55  1997/04/21  15:28:34  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.54  1997/04/15  13:10:57  mjg
 * Removed prototype for RnInitialise.
 *
 * Revision 1.53  1997/04/09  17:12:52  gdc
 * extern projection_mat removed
 *
 * Revision 1.52  1997/04/09  10:58:29  gdc
 * added another function to aid de-globalistaion of projection_mat
 *
 * Revision 1.51  1997/04/08  16:48:33  gdc
 * more chnages to eliminate projection_mat as a global
 *
 * Revision 1.50  1997/04/08  14:39:15  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.49  1997/04/08  13:55:23  jop
 * Cleaned up includes a bit
 *
 * Revision 1.48  1997/04/08  08:52:48  gdc
 * removed some more variable from global scope - not got and set via
 * rnglobal functions
 *
 * Revision 1.47  1997/04/07  14:56:17  gdc
 * added prototype for GetDefaultCamera and removed extern definition
 * of DefaultCamera
 *
 * Revision 1.46  1997/04/07  13:21:12  gdc
 * removed current_trans_set_id as a global
 *
 * Revision 1.45  1997/04/04  17:40:41  gdc
 * added fRegionScale entries to replace separate globals
 *
 * Revision 1.44  1997/03/25  10:26:36  erf
 * Added texture filtering variable to projection matrix.
 *
 * Revision 1.43  1997/03/10  08:16:03  erf
 * Removed definition of PARAM_BUF_MANAGEMENT_STRUCT structure since
 * already defined in parmbuff.h.
 *
 * Revision 1.42  1997/02/04  12:30:54  gdc
 * put in prototypes for the common rendering funcs from rnrender.c
 * and sgllite.c
 *
 * Revision 1.41  1997/02/03  11:51:47  gdc
 * removed D3DOPTIM_SGL compile flags from module - made code the default
 *
 * Revision 1.40  1996/12/10  23:08:17  erf
 * Changed PVR3 to PCX2.
 *
 * Revision 1.39  1996/12/02  12:58:59  erf
 * D3D optimisation enabled using D3DOPTIM_SGL compile switch for SGL.
 *
 * Revision 1.38  1996/11/29  12:22:53  erf
 * Using D3DOPTIM compile switch for optimisation do:
 * 1. Define the variables SyToRegionsExact, OyToRegionsExact, FirstYRegionExact
 *    and LastYRegionExact in the structure definition PROJECTION_MATRIX_STRUCT.
 *
 * Revision 1.37  1996/11/07  13:53:46  erf
 * PVR3 compile switch used for PCX2 to implement IEEE floating point support.
 * Changes made:
 * 1. Changed n32FixedProjBackDist & n32FixedClipDist definitions to
 *    f32FixedProjBackDist & f32FixedClipDist.
 *
 * Revision 1.36  1996/09/12  13:16:40  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES
 * as shadow objects may now have more than SGL_MAX_PLANES.
 * (Actually twice as many) Re: NHE 044A, 049A.
 *
 * Revision 1.35  1996/05/15  12:14:12  sjf
 * Removed a duplicate external definition.
 *
 * Revision 1.34  1996/03/22  16:51:53  sjf
 * Made shading, texturing, and transformed plane structures global.
 *
 * Revision 1.33  1995/11/10  16:16:37  sjf
 * Added plane equations for the edges of the viewing pyramid.
 *
 * Revision 1.32  1995/10/19  14:22:00  sjf
 * Chnaged storage of buffer management to allow independant
 * double/single buffering of ISP and TSP params.
 *
 * Revision 1.31  1995/10/17  12:37:44  jrc
 * Added RegionsRect structure for faster parameter passing of the regions
 * rectangle's edges.  The separate ints are kept (see comment).
 *
 * Revision 1.30  1995/10/07  17:49:02  sjf
 * Added flag for most common bounding box test.
 *
 * Revision 1.29  1995/10/07  15:09:19  sjf
 * Added new type and extra fields in projection matrix to support
 * new test box with camera in rnreject.c
 *
 * Revision 1.28  1995/10/04  08:36:41  sjf
 * Added a "texture scaling" projection matrix so that we can handle
 * UV texture space which is independant of the texture map size.
 *
 * Revision 1.27  1995/09/28  10:32:50  pdl
 * added the WrappingTransform.
 *
 * Revision 1.26  1995/09/18  20:04:07  sjf
 * Added a flag to indicate if weve processed a light volume yet
 *
 * Revision 1.25  1995/09/15  18:13:38  jrc
 * Added prototype for absoluteCoordTransform for position point calculations.
 *
 * Revision 1.24  1995/09/05  14:32:44  sm
 * changed cur tran to an INT32
 *
 * Revision 1.23  1995/09/04  15:35:20  sm
 * added new trans global
 *
 * Revision 1.22  1995/08/31  13:19:32  jrc
 * Added VpHalfWidth.
 *
 * Revision 1.21  1995/08/31  11:26:56  jrc
 * Commented out region block values that are not currently needed.
 *
 * Revision 1.20  1995/08/31  10:36:52  jrc
 * 1. Changed fFirstRegionX|YCentre to fRegion00CentreX|Y.
 * 2. Added fProjectedBackground.
 *
 * Revision 1.19  1995/08/25  17:55:14  jrc
 * Added some more items for rnreject.c.
 *
 * Revision 1.18  1995/08/21  16:51:07  sjf
 *  Added more values for rejection of infinite convex objects.
 *
 * Revision 1.17  1995/08/21  09:50:34  sjf
 * Added a boolean to enable/disable fogging.
 *
 * Revision 1.16  1995/08/18  17:05:26  sjf
 * Added fogging.
 *
 * Revision 1.15  1995/08/09  12:25:30  pdl
 * added TranslucentControlWord.
 *
 * Revision 1.14  1995/07/30  13:00:34  sjf
 * Added values to support region testing of points and Bounding boxes.
 *
 * Revision 1.13  1995/07/27  18:35:00  sjf
 * Fixed FLOAT_TO_FIXED as it was shifting into sign bit.
 *
 * Revision 1.12  1995/07/27  11:54:29  sjf
 * Added fixed backgroun distance.
 *
 * Revision 1.11  1995/07/26  15:48:36  sjf
 * Added region info to projection mega structure.
 *
 * Revision 1.10  1995/07/25  16:22:17  sjf
 * Added structure to manage the hardware parameter buffers.
 *
 * Revision 1.9  1995/07/25  11:30:03  sjf
 * Added device size to projection matrix.
 *
 * Revision 1.8  1995/07/24  14:38:42  sjf
 * Added support for Texturing scaling (CFR Value)
 *
 * Revision 1.7  1995/07/23  15:50:54  sjf
 * Added Extra values for the projection of infinte planes (to prevent
 * overflow of A & B values) and fixed defs for bounding box checking.
 *
 * Revision 1.6  1995/07/21  12:38:45  jrc
 * Added viewport information to PROJECTION_MATRIX_STRUCT.
 *
 * Revision 1.5  1995/07/21  11:51:45  sjf
 * Re-fixed the size of the local proj matrix (due to misunderstanding
 * between Pete and me) and added a default camera struct.
 *
 * Revision 1.4  1995/07/21  09:42:01  sjf
 * Added foreground and background distances.
 *
 * Revision 1.3  1995/07/20  18:01:09  sjf
 * redefined most of fields to reflect correct desgin of TRDD027x.doc
 *
 * Revision 1.2  1995/05/25  14:21:08  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.1  1995/05/11  17:32:39  sjf
 * Initial revision
 *
 *
 *
 **************************************************************************/


/*
// We need hwinterf to define the parameter buffer names
*/
#include "hwinterf.h"
/*
// For REGIONS_RECT_STRUCT
*/
#include "pmsabre.h"

#include "pvrosapi.h" /* For PVR_PARAM_BUFF */

/*
// For the Global transformed planes, and shading and texturing results
*/
#include "rnconvst.h"
#include "rnfshade.h"

/*
// Define the arrays of transformed planes, and the shading
// and texturing results of a convex or mesh.
*/
extern TRANS_PLANE_ARRAY_TYPE 	GlobalTransformedPlanes;
extern SHADING_RESULT_STRUCT   GlobalShadingResults[SGL_MAX_INTERNAL_PLANES];
extern TEXTURING_RESULT_STRUCT GlobalTextureResults[SGL_MAX_INTERNAL_PLANES];



/*
// ENUM used ONLY within the Test box with camera and the projection matrix
// setup.
// SEE rnreject.c
*/
typedef enum
{
	TB_USE_ZMIN = 0,
	TB_USE_ZMAX = 1,

	TB_USE_OTHER_Z = 1    /* Exclusive OR this to get the other one! */
}TB_USE_WHICH_Z_ENUM;



/*
// Value used to convert from floating point to "fixed" point
// 32bit value in the range (-1, 1) (not inclusive)
*/
#define FLOAT_TO_FIXED ((float) (((sgl_uint32)1) <<31))


/*
// Explained in their comment near the top of rnreject.c.
// The values should be set to ceiling(log2(MAX_X_REGIONS and MAX_Y_REGIONS
// respectively)).
*/
#define REJECT_X_RECURSION_LIMIT 4  /* log2(16) */
#define REJECT_Y_RECURSION_LIMIT 3  /* log2(8) */


/*/////////////////////////////////////////////////////
// Define the two Projection matrix types - these aren't complete
// matrices : See TRDD027x.doc for details
///////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// For projection of infinite planes, we have the centre of 
	// "pseudo rendered rectangle"
	*/
	sgl_vector RCentre;

	/*
	// Offsets to the corners of the pseudo rendered rectangle from
	// its centre
	*/
	float xToCorner, yToCorner;


	/*
	// "Normals" for the planes which form the edges of the
	// viewing Pyramid
	*/
	sgl_vector EdgeNormals[4];


	/*
	// The distances between pixels in x and y directions 
	//(in world space) on the camera plane at Z distance: RCentre[2];
	*/
	float xPerPixel, yPerPixel;

	/*
	// Non Zero projection matrix values (as described in
	// TRDD027x.doc)
	*/
	float SxDash, SyDash; 
	float OxDash, OyDash;

	/*
	// The same as above, but scaled so they project to "regions",
	// so we can work out which regions need to include particular
	// objects.
	*/
	float SxToRegions, SyToRegions; 
	float OxToRegions, OyToRegions;
	
	/* Variables to calculate the exact Y position as required by
	 * optimised AddRegioX...() routines.
	 */
	float OyToRegionsExact, SyToRegionsExact; 

	/*
	// Max pixel dim of device (in floats)
	*/
	float fDevXMax, fDevYMax;

	/*
	// The viewport in world space on a plane of distance 1.0 from the origin
	// This is used by the bounding box rejection code.
	*/
	float fViewportMinX,fViewportMaxX, fViewportMinY,fViewportMaxY;

	/*
	// The following are determined by the sign of the above, and are
	// used to access the correct Z value when rejecting a bounding box
	*/
	sgl_bool VPAroundCentre;
	TB_USE_WHICH_Z_ENUM VPMinXZI, VPMaxXZI, VPMinYZI, VPMaxYZI;

	/*
	// Also give the centre, offsets and half widths - these are used for
	// infinite plane rejection testing.
	*/
	sgl_2d_vec VpCentre, VpOffset, VpHalfWidth;

	/*
	// Centres and sizes in world coordinates of the blocks of regions for
	// different levels of recursion in infinite object rejection testing
	// (rnreject.c).  The blocks all start with region (0,0).
	// THESE AREN'T COMPUTED YET!!!
	*/
	float /* fBlockXCentres [REJECT_X_RECURSION_LIMIT],
		  fBlockYCentres [REJECT_Y_RECURSION_LIMIT],
		  fBlockHalfWidths [REJECT_X_RECURSION_LIMIT],
		  fBlockHalfHeights [REJECT_Y_RECURSION_LIMIT], */

		  /*
		  // Versions for Methods 0 and 3 in rnreject.c that replace the above
		  // block ones.  fRegion00X|YCentre refer to region (0,0).
		  */
		  fRegion00CentreX, fRegion00CentreY,
		  fRegionHalfWidth, fRegionHalfHeight;

	/*
	// Foreground and inverse background and Background distances
	// (in world coordinate space).
	*/
	float foregroundDistance, invForegroundDistance;
	float invBackgroundDistance, BackgroundDistance;
	/*
	// fProjectedBackground is used by rnreject.c
	// Its value is RCentre[2] * invBackgroundDistance.
	*/
	float fProjectedBackground;

	/*
	// Because the A&B parameters of the hardware are restricted on their
	// magnitude (compared to C) we have to scale the projection values
	// accordingly using the following...
	*/
	float fOverflowRescale;

	/*
	// Store the foreground clipping distance, relative to "Cd", 
	// and the fixed point equivalent (for when we put in clipping 
	// planes in the hardware)
	*/
	float fProjectedClipDist;

#if PCX2 || PCX2_003
	/* PCX2 family uses IEEE floating point numbers. */
	float	f32FixedClipDist;
	float	f32FixedProjBackDist;
#else
	sgl_int32	n32FixedClipDist;
	sgl_int32	n32FixedProjBackDist;
#endif

	/*
	// Values used by the texturing and Texas
	// The CFR value is the Zoom factor expressed in pixels!
	*/
	sgl_int32 n32CFRValue;
	float CFRreciprocal;

	/*
	// Information on the devices regions
	*/
	DEVICE_REGION_INFO_STRUCT RegionInfo;

	/*
	// Which regions does the viewport fall in
	*/
	int FirstXRegion, LastXRegion;
	int FirstYRegion, LastYRegion;
	
	/* Remove need for extra calculation when calculating the bounding box
	 * for an object.
	 */
	int FirstYRegionExact, LastYRegionExact;

	/*
	// The same values are now also stored in a struct for faster function
	// parameter passing.
	//
	// Presumably keep the above values to save an indirection when they are
	// used separately.  Even if this is not the case, it's a hassle to change
	// the code further for no speed improvement.
	*/
	REGIONS_RECT_STRUCT RegionsRect;

	/*
	// fog shift, because it seemed as good a place as any to put it
	*/
	int FogShift;
	sgl_bool FogOn;

	/*
	// For level of detail selection, this value is multiplied by a pixel size
	// to get the approximate cosine of the angle that the pixels make in the
	// centre of the viewport from the camera.
	*/
	float fCosAnglePerCentralPixel;
	
	/* Define the bilinear type for the current context.
	 */
	sgl_texture_filter_type	eFilterType;

	/* Dithering enabled or disabled.
	 */
	sgl_bool bDithering;

	/* scales used to get correct conversion from regions to pixels */
	float fRegionXScale;
	float fRegionYScale;

} PROJECTION_MATRIX_STRUCT;


/*/////////////////////////////////////////////////////
// And the local projection matrix
/////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// indicates if the contents are up to date.. this must be reset
	// at the commencement of rendering  (after the camera is setup)
	// and whenever the transformation state changes.
	*/
	int	valid;

	/*
	// The bits of the local to projection matrix which aren't constant
	// or aren't just copies from the transformation state matrix. 
	// Again, look at TRDD027x.doc
	*/
	float sr[2][4];


	/*
	// The following is used to scale the textures from 0-1 to the size needed
	// by the textures AND also do the projection itself. This value depends on 
	// the size of the texture. We thus check to see if the texture size is the
	// same as the last one used, and if so, we can resuse this "matrix".
	// Otherwise we re-generate it from the projection matrix.
	//
	// Note that this is used on the U,V vectors of the texture and but not
	// the Origin.
	*/
	int   LastTextureSize;
	float textureSR[3][3];

} LOCAL_PROJECTION_STRUCT;

/*/////////////////////////////////////////////////////
// Define references to these...
/////////////////////////////////////////////////////
*/

extern sgl_bool bExternalAPI;

/*/////////////////////////////////////////////////////
// Declare flag indicating if weve seen a light volume in
// the current render.
// Initialised (per render) in rntrav...
///////////////////////////////////////////////////////
*/
extern sgl_bool doneALightVol;

/*////////////////////////////////////////////////////
// the translucent control word is the TEXAS control word that
// 'points' to a single white pixel.This is used for non-textured
// translucency.
//////////////////////////////////////////////////////
*/

extern sgl_uint32 TranslucentControlWord;

/*/////////////////////////////////////////////////////
// Routine Prototypes
///////////////////////////////////////////////////////
*/

#if PCX2 || PCX2_003
float RnGlobalGetFixedClipDist ();
#else
int RnGlobalGetFixedClipDist ();
#endif
void RnGlobalSetProjMat (PROJECTION_MATRIX_STRUCT  *pProjMat);
void RnGlobalCopyProjMat (PROJECTION_MATRIX_STRUCT  *pProjMat);
PROJECTION_MATRIX_STRUCT  *RnGlobalGetProjMat ();
sgl_bool RnGlobalProjMatRegionOnScreen(int XRegion, int YRegion);
sgl_bool RnGlobalProjMatRegLastGtRegFirst();
sgl_bool RnGlobalProjMatAllRegionsPositive();
sgl_bool RnGlobalProjMatRegionBoxOnScreen(REGIONS_RECT_STRUCT *Regions);

void RnGlobalSetLocalProjMat (LOCAL_PROJECTION_STRUCT  *pLocalProjMat);
sgl_bool RnGlobalLocalProjMatIsValid (void);
sgl_bool RnGlobalLocalProjMatTextureIsValid (int TexSize);
LOCAL_PROJECTION_STRUCT	*RnGlobalGetLocalProjMat (void);
void RnGlobalCopyLocalProjMat (LOCAL_PROJECTION_STRUCT  *pLocalProjMat);

TRANSFORM_STRUCT *RnGlobalGetAbsoluteCoordTransform ();
void RnGlobalSetAbsoluteCoordTransform (TRANSFORM_STRUCT *pTransformState);

void RnGlobalSetWrappingTransform (TRANSFORM_STRUCT *pTransformState);
void RnGlobalCopyWrappingTransform (TRANSFORM_STRUCT *pTransformState);
TRANSFORM_STRUCT *RnGlobalGetWrappingTransform ();

int RnGlobalGetCurrentLightSlot();
void RnGlobalSetCurrentLightSlot(int slot);

CAMERA_NODE_STRUCT *GetDefaultCamera(void);

void GetParameterSpace (PVR_PARAM_BUFF pBuff[3]);
void CopyParamsIfRequired(PVR_PARAM_BUFF pBuff[3]);
void WaitForHardware ();

/*--------------------------- End of File --------------------------------*/

