/**************************************************************************
 * Name         : rnconvpr.c
 * Title        : Convex primitives Projection/Shading etc code
 * Author       : Simon Fenney
 * Created      : 22 June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Contains projection, shading etc code for convex primitives
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnconvpr.c,v $
 * Revision 1.118  1997/07/14  19:31:33  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.117  1997/06/17  10:24:10  mjg
 * Added FORCE_NO_FPU option.
 *
 * Revision 1.116  1997/06/04  11:15:46  gdc
 * removed some compiler warnings
 *
 * Revision 1.115  1997/05/15  13:48:42  gdc
 * and another
 *
 * Revision 1.114  1997/05/15  13:42:18  gdc
 * ...and another
 *
 * Revision 1.113  1997/05/15  13:38:56  gdc
 * downgraded a couple of warnings to messages
 *
 * Revision 1.112  1997/05/09  19:17:07  mjg
 * Downgraded a couple of ASSERTs to warnings.
 *
 * Revision 1.111  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.110  1997/04/30  19:20:27  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.109  1997/04/29  15:06:26  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.108  1997/04/24  12:49:53  mjg
 * Removed some compiler warnings.
 *
 * Revision 1.107  1997/04/21  20:01:00  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.106  1997/04/21  15:28:01  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.105  1997/04/08  17:45:11  gdc
 * removed all direct references to projection_mat global form this
 * module
 *
 * Revision 1.104  1997/04/08  14:39:43  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.103  1997/04/07  14:51:04  gdc
 * added a shadow limit plane pointer to RnProcessConvexShadows parameters
 *
 * Revision 1.102  1997/04/07  13:31:47  gdc
 * trans set ID is now received as a parameter not a global
 *
 * Revision 1.101  1997/03/05  17:37:08  erf
 * Remove changes made by SXY in 1.99 to check C parameter.
 * This is already done in pkisp.c.
 *
 * Revision 1.100  1997/03/05  12:21:23  jop
 * Fixed a typo
 *
 * Revision 1.99  1997/02/27  17:26:03  sxy
 * Added check fC values for PCX2.
 *
 * Revision 1.98  1997/02/03  10:46:20  gdc
 * removed D3DOPTIM_SGL conditionals from this module as this is the default
 * and only option
 *
 * Revision 1.97  1996/12/18  18:30:18  erf
 * PCX2 needs to scale A, B & C parameters to prevent overflow.
 * ie 32 bit to 20 bit conversion overflow to prevention.
 * Forget what I said in Rev 1.89.
 *
 * Revision 1.96  1996/12/13  18:06:49  gdc
 * added a parenthesis on smooth shading qual flag check as == has
 * higher precedence than &
 *
 * Revision 1.95  1996/12/13  17:17:15  gdc
 * added calls to individual non-branching non-textured texas param packing
 * routines, firstly to fix a bug when texuring is disabled via a quality
 * setting and translucency is on, and secondly non-branching pktsp routines
 * should be a little faster
 *
 * Revision 1.94  1996/12/10  23:42:20  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR3 to PCX2.
 *
 * Revision 1.93  1996/12/03  16:58:59  erf
 * Using D3DOPTIM_SGL compile switch for D3D optimisation with SGL do:
 * 1. No need to correct current_trans_set_id with translucent planes.
 *
 * Revision 1.92  1996/12/02  13:02:44  erf
 * D3D optimisation enabled for SGL using D3DOPTIM_SGL compile switch.
 *
 * Revision 1.91  1996/11/30  00:10:47  erf
 * In D3DOPTIM for translucency do:
 * 1. Ensure current_trans_set_id multiple of 2 and > 1.
 * 2. Depths are passed as positive. ie negate removed.
 *
 * Revision 1.90  1996/11/29  11:57:55  erf
 * Using D3DOPTIM compile switch for optimisation do:
 * 1. Change ASSERTs to check absolute Y co-ordinates.
 * 2. Use new routines AddRegionSolid(), AddRegionAtmos() and AddRegionSeeThru()
 *    to add objects.
 *
 * Revision 1.89  1996/11/14  15:55:13  erf
 * Use PVR3 compile switch for PCX2 to:
 * 1. Don't scale perpendicular edges in RnProjectAndClassifyPlanes().
 * 2. No need to scale to prevent overflow.
 *
 * Revision 1.88  1996/11/07  12:06:48  erf
 * Added PVR3 compile switch for PCX2 to:
 * 1. Change n32A, B & C to f32A, B & C.
 * 2. Remove need for FLOAT_TO_FIXED.
 *
 * Revision 1.87  1996/11/07  09:55:25  erf
 * Added PVR3 compile switch to:
 * 1.Remove calls to PackTo20Bit() since not needed in PCX2.
 *
 * Revision 1.86  1996/10/30  10:43:30  gdc
 * added an sfabs() in perp plane classification to improve the artdink
 * railway line problem.
 *
 * Revision 1.85  1996/10/10  11:12:46  mal
 * Modified some of the metric macros.
 *
 * Revision 1.84  1996/10/04  17:14:34  mal
 * Added some metric macros.
 *
 * Revision 1.83  1996/09/25  12:14:54  mal
 * Corrected the smooth shading bug when the RepPt goes infront
 * of the front clipping plane for smooth shading. Re: NEC HE.
 *
 * Revision 1.82  1996/09/20  10:17:48  sjf
 * Hopefully hacked a fix for the hardware limitation of No smooth shading
 * with translucency. It should now resort to flat shading if there is
 * any translucency in a convex.
 *
 * Revision 1.81  1996/09/19  15:37:15  mal
 * I have added the vNewProjRepPoint code to fix the Smooth Shading
 * problems when the rep point foes off screen. re: NECHE 094.
 *
 * Revision 1.80  1996/09/19  15:16:59  sjf
 * Hacked in a partial fix for a mixture of opaque and translucent faces
 * in a convex object.  PROBLEMS: Won't work if any of the (visible) faces
 * are NOT textured. PLUS it might also fail if there is some smooth
 * shading.
 *
 * Revision 1.79  1996/09/13  10:22:04  gdc
 * removed a sign change on the y value of proj rep point in the
 * difficult off-screen case. This seems to fix NECHE bug no.89
 * where textures swam when the rep point went off-screen
 *
 * Revision 1.78  1996/09/12  13:28:49  mal
 * Modified the previous comment :)
 *
 * Revision 1.77  1996/09/12  13:12:46  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES.
 * Shadow objects may now have more than SGL_MAX_PLANES.
 * (Twice as many actually) Re: NHE044A, NHE049A.
 * Also fixed an accuracy problem in RnProjectAndClassifyPlanes().
 * Mesh and Convex planes close together would swap depth order.
 * Re: NHE 064
 *
 * Revision 1.76  1996/09/10  08:25:32  mal
 * Modified a comment.
 *
 * Revision 1.75  1996/09/09  17:08:50  mal
 * Modified the definition of the number of shadow planes
 * in the shadow plane array. this has been bumped up
 * to twice the SGL_MAX_PLANES value.
 *
 * Revision 1.74  1996/09/09  14:03:29  mal
 * Fixed NEC HE Shadows Bug
 * Initialised the forward and reverse plane counts before
 * calling RnProjectAndClassifyPlanes() just after
 * RnGenerateShadowVolume(). Re: NHE 044-A
 *
 * Revision 1.73  1996/09/04  10:41:19  sjf
 * Typo bug in tyhe infinite translucency.
 *
 * Revision 1.72  1996/08/30  17:11:15  sjf
 * Finally fixed the infinite plane translucency sorting: I was only
 * handling the case where you are "outside" the convex. Now should
 * also work when you are inside as well.
 *
 * Revision 1.71  1996/08/30  15:48:55  sjf
 *  Hacked in small object rejection for PVR1.
 *
 * Revision 1.70  1996/08/30  11:51:33  sjf
 * I think I've now coded a suitable method of coping with large translucent objects.
 * It works quite well with my test program, but seems to fail with Mech Warrior, even
 * though my test program seems similar. Sigh.
 *
 * Revision 1.69  1996/08/29  13:11:52  sjf
 * Added new param to AddRegionTranslucent
 *
 * Revision 1.68  1996/08/13  10:14:03  sjf
 * Replaced an if-then with some bitwise boolean logic.
 *
 * Revision 1.67  1996/07/31  09:21:33  sjf
 * Hmm still getting problems with the mip mapping changing levels
 * abruptly when the plane rep-points go off-screen. I've just made it
 * a bit more stringent. I'm not sure this will really help. What we
 * probably need is a means of choosing the "closest" on screen point.
 * At the moment we are just choosing a corner of the screen.
 *
 * Revision 1.66  1996/07/11  16:40:16  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.65  1996/07/10  17:47:57  sjf
 * Hopefully fixed the funny flipping of MIP levels by not allowing the
 * rep point to move too far off screen. I was allowing it to get quite
 * negative - not the case anymore.
 *
 * Revision 1.64  1996/07/10  14:34:23  pdl
 * added the accurate/fast texturing.
 *
 * Revision 1.63  1996/06/20  20:32:02  jop
 * 1) By Jim: added a debugging option to turn off texturing in
 *     convex
 * 2) Texturing wrapping should now only be switched on when the
 *    SMAP and OMAPS are defined. (sjf)
 *
 * Revision 1.62  1996/06/06  23:04:09  sjf
 * Added material instancing.
 *
 * Revision 1.61  1996/05/31  10:37:32  jrc
 * Corrected the previous bug fix to reject the whole object if the close
 * perpendicular plane is facing forwards, or to reject just the plane if it is
 * facing backwards.
 *
 * Revision 1.60  1996/05/30  14:33:39  jrc
 * Now only rejects *reverse* planes that are completely behind the forward
 * clipping plane.  Forward planes that are behind should cause the entire
 * convex to be rejected, but that hasn't been implemented yet.
 *
 * Revision 1.59  1996/05/14  17:40:24  jrc
 * Updated a couple of out of date function calls in the 'show shadow volumes'
 * code (though for some reason using this code switches off most texturing!).
 *
 * Revision 1.58  1996/05/03  16:20:49  sjf
 * Got rid of unused parameter.
 *
 * Revision 1.57  1996/04/22  16:10:22  sjf
 * added call to non textured pack routine
 *
 * Revision 1.56  1996/04/20  16:46:06  sjf
 *  Added a smooth textured packing routine.
 *
 * Revision 1.55  1996/04/19  19:54:05  sjf
 * Added a call to the specialised flat textured pack routine.
 *
 * Revision 1.54  1996/03/22  16:50:42  sjf
 * Moved shading and texturing result structures to global.c
 *
 * Revision 1.53  1996/03/12  11:52:59  sm
 * fixed bug with convex tex wrapping caused by data struct changes
 *
 * Revision 1.52  1996/02/22  11:49:09  sjf
 * JIM made changes for the new data structures (ie separation of points
 * into their own structure) BUT he used a stuffed editor which changed almost
 * all the TABS to 4 spaces and added ^M's to the end of the lines. I obsoleted
 * that file, and remade the changes to the earlier version... and this is
 * THAT FILE !!!!
 *
 * Revision 1.51  1995/11/27  15:22:12  pdl
 * changed an error to a message.
 *
 * Revision 1.50  1995/11/10  16:15:38  sjf
 * Added working code for getting an on-screen rep point.
 *
 * Revision 1.49  1995/11/02  19:49:47  sjf
 * Made fog a quality setting.
 *
 * Revision 1.48  1995/10/26  12:37:19  jrc
 * Forgot to add checking for FALSE returns from other calls to
 * RnDetermineRegionsFromPlanes.
 *
 * Revision 1.47  1995/10/23  15:43:34  jrc
 * 1. Infinite translucent objects should now work (previously the BBox struct
 *    was accessed when it could be NULL.
 * 2. Added code to handle the region rejection case where the object was
 *    thought to be probably in the viewport but subsequently found to be in no
 *    regions.
 *
 * Revision 1.46  1995/10/17  12:31:40  jrc
 * Regions rectangle now dealt with by a struct instead of four separate ints
 * for faster parameter passing.
 *
 * Revision 1.45  1995/10/09  15:29:17  sjf
 * fixed incorrect precedence in boolean expression when testing
 * if flat or smooth shading in invidual batch shade routine.
 *
 * Revision 1.44  1995/10/09  10:39:59  jrc
 * Removed RnDetermineRegionsFromBBox as it is now in rnreject.c
 *
 * Revision 1.43  1995/10/04  14:49:55  sjf
 * Added more debug
 *
 * Revision 1.42  1995/10/04  08:36:05  sjf
 *  Added code to support texture space which is independant of the
 *  size of the texture.
 *
 * Revision 1.41  1995/09/29  19:31:03  jrc
 * Changed debug messages for infinite objects to be consistent for easier
 * reading of debug output.
 *
 * Revision 1.40  1995/09/29  10:41:54  jrc
 * Added Simon's changes to the SHOW_SHAD_VOLS section.
 *
 * Revision 1.39  1995/09/28  10:33:37  pdl
 * RnProcessMaterialNode has changed.
 *
 * Revision 1.38  1995/09/23  15:08:12  sjf
 * Removed redundant set up of local materials (when there wasn't
 * any) and changed 2 divisions to use approximate reciprocal
 * because we only needed to prevent overflow conditions.
 *
 * Revision 1.37  1995/09/19  09:10:38  sjf
 * Added Light/Shadow volume processing.
 *
 * Revision 1.36  1995/09/16  19:44:41  sjf
 * Fixed bug with my last change - wasnt initialising plane counts
 * in plane categories.
 *
 * Revision 1.35  1995/09/15  20:19:33  sjf
 * Got rid of overhead of calling Classify Material Planes when there
 * was only one material, and now just call the classify planes routine.
 *
 * Revision 1.34  1995/09/15  19:49:01  sjf
 * Removed uneeded param from shading routines.
 *
 * Revision 1.33  1995/09/13  18:24:07  jop
 * Implemented smooth shading
 *
 * Revision 1.32  1995/09/12  14:31:08  sjf
 * OOPS fixed bug in ignoring visibility flags.. wasn't incrementing a pointer
 * when storing results.
 *
 * Revision 1.31  1995/09/08  15:41:23  sjf
 * In project and classify, moved the ignore invisibility test out of the loop
 * as this is used less often.
 *
 * Revision 1.30  1995/09/07  12:13:48  jrc
 * Changed RnDetermineRegionsFromPlanes to pass planes in a single struct.
 *
 * Revision 1.29  1995/09/07  11:10:38  sjf
 * Added call to determine regions for shadow volumes.
 *
 * Revision 1.28  1995/09/06  14:59:47  sjf
 * Fixed typo in an assert.
 *
 * Revision 1.27  1995/09/06  14:54:42  sjf
 * Changed parameters for lists of planes to use a struct which lists the
 * lot.
 *
 * Revision 1.26  1995/09/05  09:41:39  sm
 * removed a commented bit !
 *
 * Revision 1.25  1995/09/04  15:26:01  sm
 * changed for multi trans
 *
 * Revision 1.24  1995/08/31  16:50:37  jrc
 * Added some messages to indicate what regions are being used.
 *
 * Revision 1.23  1995/08/31  14:14:24  jrc
 * Added call to RnDetermineRegionsFromPlanes.
 *
 * Revision 1.22  1995/08/28  16:53:42  sjf
 * Made chnages for new flat texture shading routine.
 *
 * Revision 1.21  1995/08/27  15:24:20  sjf
 * 1) Re-arranged code
 * 2) Added shadow generation.
 *
 * Revision 1.20  1995/08/21  16:09:49  sm
 * added stuf for single layer trans.
 *
 * Revision 1.19  1995/08/21  10:01:57  sjf
 * Set fog flag on planes correctly.
 *
 * Revision 1.18  1995/08/18  15:42:01  pdl
 * enabled texture wrapping.
 * corrected the mixed texture processing.
 *
 * Revision 1.17  1995/08/14  11:38:24  sjf
 * Adding packing for floats to 20 bit floats.
 *
 * Revision 1.16  1995/08/02  15:14:03  sm
 * changed to call the two shading functions
 *
 * Revision 1.15  1995/08/02  11:46:42  pdl
 * added sfabs()
 *
 * Revision 1.14  1995/07/31  13:48:23  sjf
 * Let the projection of bounding box routine do its own clipping
 * against the foreground clipping plane, as this is the best place for it.
 *
 * Revision 1.13  1995/07/30  17:48:15  sjf
 * Added a "slop factor" jsut for safety in the region calc, and
 * reduced some debugging output.
 *
 * Revision 1.12  1995/07/30  13:14:59  sjf
 * OOPs, cut and paste bug with last edition - had min instead of 
 * max in a few divisions.
 *
 * Revision 1.11  1995/07/30  13:04:23  sjf
 * Added regions from bounding box routine.
 *
 * Revision 1.10  1995/07/30  10:49:57  sjf
 * fixed incorrect projection of (on screen) rep point.
 *
 * Revision 1.9  1995/07/29  13:35:54  sjf
 * Removed copy of original data, as this is now in transform planes.
 *
 * Revision 1.8  1995/07/28  11:57:37  sjf
 * 1) Wasn't storing texture and shading data in transformed planes struct.
 * 2) In Perp planes, wasnt diving by the ABS max value - values could have
 *    gone out of range.
 *
 * Revision 1.7  1995/07/28  11:29:13  sjf
 * Added update of local proj matrix.
 *
 * Revision 1.6  1995/07/27  20:33:23  sjf
 * Fixed bug in deciding if insides are visible...
 *
 * Revision 1.5  1995/07/27  18:34:34  sjf
 * Fixed shading call (reverse flag was inverted) and added more debug.
 *
 * Revision 1.4  1995/07/27  15:17:07  sjf
 * Fixed perp plane directions.
 *
 * Revision 1.3  1995/07/27  10:57:09  sjf
 * Added reams of debug.
 *
 * Revision 1.2  1995/07/26  16:26:43  sjf
 * More code basically.
 *
 * Revision 1.1  1995/07/23  16:21:28  sjf
 * Initial revision
 *
 *
 *
 **************************************************************************/

/*
// define Debug Module IDs
*/
#define MODULE_ID	MODID_RNCONVPR

#include "sgl_defs.h"
#include "sgl.h"
#include "sgl_math.h"

#include "nm_intf.h"
#include "dlnodes.h" 

#include "rnglobal.h"
#include "rnstate.h"

#include "rnconvst.h"
#include "rnconvex.h"
#include "dvdevice.h"
#include "rncamera.h"

#include "rnconvpr.h"
#include "rnshadrj.h"
#include "rnshadow.h"
#include "rnreject.h"

#include "rnmater.h"

#include "rnfshade.h"
#include "rnshade.h"
#include "rntex.h"
#include "pktsp.h"
#include "pkisp.h"

#include "pmsabre.h"
#include "profile.h"
#include "metrics.h"
#include "parmbuff.h"

/* Needed for to use dregion.c optimisation routines. */
#include "dregion.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/**********************************
// Enumerated types for the modes of shading and texturing.
**********************************/
typedef enum
{
	NO_TEXTURING,
	PREMAPPED_TEXTURING,
	WRAP_TEXTURING,
	MIXED_TEXTURING

} BATCH_TEXTURE_MODES_ENUM;

typedef enum
{
	FLAT_SHADING,
	SMOOTH_SHADING,
	MIXED_SHADING

} BATCH_SHADING_MODES_ENUM;


/**********************************
// Define a "used local materials table" type. Note: this is different to
// the local materials table in the convex node, because it only is
// interested in those planes which are potentially shadeable - i.e.
// forward and reverse visible ones.
//
// Note there can't (shouldn't??) be more (used) materials than planes
**********************************/
typedef struct
{
	MATERIAL_NODE_STRUCT *pMaterialNode;
	int FVPlanesAffected;
	int RVPlanesAffected;

} USED_MATERIAL_ENTRY_STRUCT ;

typedef USED_MATERIAL_ENTRY_STRUCT 	USED_MATERIAL_TAB[SGL_MAX_PLANES];

static  USED_MATERIAL_TAB   UsedMaterials;

/* bit of a hack really ... */
static SMOOTHPARAMS gSP;


/**************************************************************************
* Function Name  : DetermineRegionDistances (LOCAL FUNCTION)
* Inputs         : Regions:  The regions the objects been put in.
*				   PlaneLists: List of plane categories etc.
*
*					
* Outputs        : NearZs: Values to determine the  approximate Z depth of the 
*						   the object in each region.
*
* Returns        : Nothing
*
* Description    : This routine is used as a hack to improve the translucency
*				   passes sorting for infinite objects. We basically get an
*				   estimate of the distance of the object at for each region.
*
*				   It really only estimates the (inverse) distaces:Hackety Hack.
*				   This code is NOT efficient.
*
**************************************************************************/

static void DetermineRegionDistances( REGIONS_RECT_STRUCT * Regions,
									  PLANE_CATEGORIES_STRUCT *PlaneLists,
									  TRANS_REGION_DEPTHS_STRUCT *NearZs)
{
	float TopLeft, TopRight, BotLeft;
	TRANSFORMED_PLANE_STRUCT * 	pPlane;
	TRANSFORMED_PLANE_STRUCT ** ppPlane;
	int i;
	sgl_bool Outside;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();
	float invForegroundDist, xToCorner, yToCorner, xPerPixel, yPerPixel;
	sgl_vector RCentre;
	int RegionXSize, RegionYSize;

	SGL_TIME_START(PROJECT_REP_TIME)

	invForegroundDist = pProjMat->invForegroundDistance;
	RCentre[0] = pProjMat->RCentre[0];
	RCentre[1] = pProjMat->RCentre[1];
	RCentre[2] = pProjMat->RCentre[2];
	xToCorner = pProjMat->xToCorner;
	yToCorner = pProjMat->yToCorner;
	yPerPixel = pProjMat->yPerPixel;
	xPerPixel = pProjMat->xPerPixel;
	RegionXSize = pProjMat->RegionInfo.XSize;
	RegionYSize = pProjMat->RegionInfo.YSize;
	/*
	// There are 2 possible cases to consider: We are either outside or
	// inside the convex object.
	//
	// If outside ....
	*/
	Outside = PlaneLists->NumFV;
	if(Outside)
	{
		i = PlaneLists->NumFV;
		ppPlane = PlaneLists->FVPlanes;
		/*
		// Set our initial distance guesses so they are as close as we can get.
		*/
		TopLeft = invForegroundDist;
		TopRight= invForegroundDist;
		BotLeft = invForegroundDist;
	}
	else
	{
		i = PlaneLists->NumRV;
		ppPlane = PlaneLists->RVPlanes;

		/*
		// Set our initial distance guesses so they are as FAR as we can get.
		*/
		TopLeft = 0.0f;
		TopRight= 0.0f;
		BotLeft = 0.0f;
	}
	/*
	// Step through the planes, and compute the depths. If they
	// are further/closer away, then update our dept values.
	*/
	for(/*Nothing*/; i != 0; i--, ppPlane++)
	{
		float Dist00, invD, tempD;
		float PerX, PerY;

		pPlane = *ppPlane;


		/*
		// Invert the plane D value
		*/
		if(sfabs(pPlane->d)> 1.0E-10f)
		{
			invD = 1.0f/ pPlane->d;
		}
		else if(pPlane->d >= 0.0f)
		{
			invD = 1.0E10f;
		}
		else
		{
			invD = -1.0E10f;
		}

		/*
		// get the 0,0 Distance
		*/
		Dist00 = (DotProd(pPlane->normal, RCentre) 
			   - pPlane->normal[0] * xToCorner
			   - pPlane->normal[1] * yToCorner) * invD;

		PerX = pPlane->normal[0] * xPerPixel * invD *
			   RegionXSize;
		PerY = pPlane->normal[1] * yPerPixel * invD *
			   RegionYSize;

		/*
		// URK! this is really awful. Now divide by the camera distance
		// so that we are pretty much in 1/world coords.  The stupid thing
		// is that the dot product could be much simpler! Oh what the hell.
		// who really cares anyway
		*/
		Dist00 /= RCentre[2];
		PerX   /= RCentre[2];
		PerY   /= RCentre[2];

		/*
		// Set  Dist00 to be the centre of the TOP LEFT region depth
		*/
		Dist00 = Dist00 + (Regions->FirstXRegion + 0.5f) * PerX
						+ (Regions->FirstYRegion + 0.5f) * PerY;


		if(Outside)
		{
			/*
			// if this value is further than all previous values, keep it
			*/
			if(Dist00 < TopLeft )
			{
				TopLeft = Dist00;
			}

			tempD = Dist00 + (Regions->LastXRegion - Regions->FirstXRegion) * PerX;

			if(tempD < TopRight)
			{
				TopRight = tempD;
			}

			tempD = Dist00 + (Regions->LastYRegion - Regions->FirstYRegion) * PerY;

			if(tempD < BotLeft)
			{
				BotLeft = tempD;
			}
		}
		else
		{
			/*
			// if this value is CLOSER than all previous values, keep it
			*/
			if(Dist00 > TopLeft )
			{
				TopLeft = Dist00;
			}

			tempD = Dist00 + (Regions->LastXRegion - Regions->FirstXRegion) * PerX;

			if(tempD > TopRight)
			{
				TopRight = tempD;
			}

			tempD = Dist00 + (Regions->LastYRegion - Regions->FirstYRegion) * PerY;

			if(tempD > BotLeft)
			{
				BotLeft = tempD;
			}
		}
	}/*end for*/


	/*
	// Convert the overall values
	*/
	if(Regions->LastXRegion == Regions->FirstXRegion)
	{
		NearZs->DepthDx = 0.0f;
	}
	else
	{
		NearZs->DepthDx = (TopRight- TopLeft)/ 
						(Regions->LastXRegion - Regions->FirstXRegion);
	}

	if(Regions->LastYRegion == Regions->FirstYRegion)
	{
		NearZs->DepthDy = 0.0f;
	}
	else
	{
		NearZs->DepthDy = (BotLeft - TopLeft)/ 
						(Regions->LastYRegion - Regions->FirstYRegion);
	}

	NearZs->BaseDepth = TopLeft - Regions->FirstXRegion * NearZs->DepthDx
								- Regions->FirstYRegion * NearZs->DepthDy;

}	


/**************************************************************************
* Function Name  : RnProjectAndClassifyPlanes (LOCAL FUNCTION)
* Inputs         : pointer to convex node
*				   Starting plane &  num planes to do
*
*				NOTE: If ignore invisible is TRUE then all invisible planes
*					  will be put on the respective visible list. THIS INCLUDES
*					  ANY PASSED IN FROM THE CALLER!!
*					
* Outputs        : Number of forward visible planes affected
*					If the object is offscreen.
*
* Input/Output	  : Transformed plane structure,
*					Lists of the plane categories and the number of planes
*						  in each list.
*
* Returns        : TRUE if the object is in fact, off screen.
*
* Global Used    : Projection "matrix" values
*
* Description    : This projects the transformed planes, and at the same time
*					classifies them into forward, reverse, perpendicular, etc
*
*
**************************************************************************/
static sgl_bool RnProjectAndClassifyPlanes(const int NumPlanesToDo,
					/*
					// Pointer to the first plane of the batch
					// to process, with pointers to its shading
					// and texturing entries
					*/
					TRANSFORMED_PLANE_STRUCT * 	pPlane,
					CONV_SHADING_STRUCT * pShadingData,
					CONV_TEXTURE_UNION  * pTextureData,
                    CONV_POINTS_STRUCT  * pPointsData,

						const sgl_bool ignore_visibility,

					PLANE_CATEGORIES_STRUCT * PlaneLists,
										int * pFVAffected, 
										int * pRVAffected)
{

	int i;

	int LocalFVAffected;
	int LocalRVAffected;

	/*
	// The vast majority of planes will be either forward visible
	// or reverse visible, so get local copies of pointers to them.
	*/
	TRANSFORMED_PLANE_STRUCT ** ppFVplane;
	TRANSFORMED_PLANE_STRUCT ** ppRVplane;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();
	sgl_vector RCentre;
	float xToCorner, yToCorner, xPerPixel, yPerPixel;
	float fOverflowRescale;

	RCentre[0] = pProjMat->RCentre[0];
	RCentre[1] = pProjMat->RCentre[1];
	RCentre[2] = pProjMat->RCentre[2];
	xToCorner = pProjMat->xToCorner;
	yToCorner = pProjMat->yToCorner;
	yPerPixel = pProjMat->yPerPixel;
	xPerPixel = pProjMat->xPerPixel;
	fOverflowRescale = pProjMat->fOverflowRescale;
	/*
	// Check the number of planes in each category list
	*/
	ASSERT(PlaneLists->NumFV >=0); ASSERT(PlaneLists->NumFV < SGL_MAX_INTERNAL_PLANES);
	ASSERT(PlaneLists->NumFI >=0); ASSERT(PlaneLists->NumFI < SGL_MAX_INTERNAL_PLANES);
	ASSERT(PlaneLists->NumRV >=0); ASSERT(PlaneLists->NumRV < SGL_MAX_INTERNAL_PLANES);
	ASSERT(PlaneLists->NumRI >=0); ASSERT(PlaneLists->NumRI < SGL_MAX_INTERNAL_PLANES);
	ASSERT(PlaneLists->NumPE >=0); ASSERT(PlaneLists->NumPE < SGL_MAX_INTERNAL_PLANES);

	SGL_TIME_START(PROJECTION_TIME)

	/*
	// reset the numbers of visible affected planes to zero.
	// Set up pointers to the most common plane lists as well.
	*/
	LocalFVAffected = 0;
	LocalRVAffected = 0;

	ppFVplane = &PlaneLists->FVPlanes[PlaneLists->NumFV];
	ppRVplane = &PlaneLists->RVPlanes[PlaneLists->NumRV];
	/*
	// Step through the planes in this batch
	*/
	for(i = 0; i < NumPlanesToDo; i++, pPlane++, pTextureData++, 
									   pShadingData++, pPointsData++)
	{
		float centreDotNorm;
		float XBorder, YBorder, MaxBorder;

		/*
		// Do checking for perpendicular plane cases
		//
		// Dot product of planes normal and the point at the centre
		// of the "pseudo" rendered region, and the offsets to the
		// corners
		*/
		centreDotNorm = DotProd(pPlane->normal, RCentre);
		XBorder = pPlane->normal[0] * xToCorner;
		YBorder = pPlane->normal[1] * yToCorner;
		MaxBorder = sfabs(XBorder) + sfabs(YBorder);

		/****************************
		// Check that the plane doesn't come too close to the camera.
		// This tests that ALL corners of pseudo rendered region intersect
		// the plane safely far enough away.
		****************************/
		DPFOO((DBG_MESSAGE, "Only Using simple Projection!"));

		if(sfabs(pPlane->d) > (sfabs(centreDotNorm) + MaxBorder))
		{
			float invD;

			/*
			// The plane is quite safe to project.
			// If A and B could span the same range as C, the following
			// would just be a division by Z. However, A and B are "n" bits
			// less (in max value) so to prevent overflow (which is more
			// likely to occur as the size of the device goes down) 
			// we rescale it.
			*/
			invD = fOverflowRescale/ pPlane->d;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			pPlane->f32A = pPlane->normal[0] * xPerPixel * invD;
			pPlane->f32B = pPlane->normal[1] * yPerPixel * invD;
			pPlane->f32C = ((centreDotNorm - (XBorder + YBorder))* invD);
#else
		    SGL_TIME_SUSPEND(PROJECTION_TIME)
			pPlane->n32A =
			  PackTo20Bit(pPlane->normal[0] * xPerPixel * invD);
			pPlane->n32B =
			  PackTo20Bit(pPlane->normal[1] * yPerPixel * invD);
		    SGL_TIME_RESUME(PROJECTION_TIME)

			/* 
			// NOTE: Following calculation may not be accurate enough....
			// (C is the value at pixel (0,0))
			//
			// First rescale by FLOAT TO FIXED.
			*/
			invD = invD * FLOAT_TO_FIXED;
			pPlane->n32C= (sgl_int32) ((centreDotNorm - (XBorder + YBorder))* invD);
#endif

			/*
			// Add the plane to the correct category
			// If a forward plane (i.e. negative d)
			*/
			if(pPlane->d <= 0.0f)
			{
				/*
				// Decide if visible or invisible, and add to
				// the required list.
				*/
				if(pPlane->flags & pf_visible)
				{
					/*
					// Save as forward visible
					*/
					*ppFVplane = pPlane;
					ppFVplane ++;
					LocalFVAffected++;

					/*
					// In case we need it, store the
					// shading/ texturing data for the plane
					*/
					pPlane->pTextureData  =	pTextureData;
					pPlane->pShadingData  = pShadingData;
					pPlane->pPointsData  = pPointsData;
						
				}
				/*
				// Save as forward INvisible
				*/
				else
				{
					PlaneLists->FIPlanes[PlaneLists->NumFI] = pPlane;
					PlaneLists->NumFI++;
				}
			}
			/*
			// Else a reverse plane
			*/
			else
			{
				/*
				// Decide if visible or invisible, and add to
				// the required list.
				*/
				if(pPlane->flags & pf_visible)
				{
					/*
					// Save as Reverse visible
					*/
					*ppRVplane = pPlane;
					ppRVplane ++;
					LocalRVAffected++;

					/*
					// In case we need it, store the
					// shading/ texturing data for the plane
					*/
					pPlane->pTextureData  =	 pTextureData;
					pPlane->pShadingData  =  pShadingData;
					pPlane->pPointsData  = pPointsData;
				}
				else
				{
					/*
					// Save as Reverse INvisible
					*/
					PlaneLists->RIPlanes[PlaneLists->NumRI] = pPlane;
					PlaneLists->NumRI++;
				}

			}/*end if (foward plane) else*/

		}
		/*****************************
		// ELSE we have a tricky plane which MIGHT be perpendicular
		//
		// Determine whether it is just too close to the eye, or always behind.
		//
		// If so then:
		//   If the plane is facing in the direction of the camera then the
		//   whole object can be rejected, else just the plane can be rejected.
		// 
		// This tests if ALL of the corners are IN FRONT of the clipping
		// plane.
		*****************************/
		else if ( (sfabs(pPlane->d) * pProjMat->fProjectedClipDist) <=
			      (sfabs(centreDotNorm) - MaxBorder) )
		{
			DPF((DBG_MESSAGE,
				"Got Plane too close everywhere: d:%f  N:[%f,%f,%f]", 
				pPlane->d, pPlane->normal[0], 
				pPlane->normal[1], pPlane->normal[2] ));

			if (pPlane->normal[2] > 0.0f)
			{
				/* Reject the whole object */
				SGL_TIME_STOP(PROJECTION_TIME)
				return TRUE; /* object is offscreen */
			}
			else
			{
				/* Reject (ignore) the plane */
			}
		}
		/*****************************
		// Else it needs the perpendicular treatment
		*****************************/
		else
		{
			float middle;
			float offset;
			float maxValue;

			DPF((DBG_MESSAGE, 
				"Got Perp Plane: d:%f  N:[%f,%f,%f]Using Min (not clip) dist)", 
				 pPlane->d, pPlane->normal[0], 
				 pPlane->normal[1], pPlane->normal[2] ));
			
			/*
			// if the rep point is in front of the clipping plane,
			// use the clipping plane as the plane that we intersect
			// the per plane with.
			*/
			if(sfabs(pPlane->repPnt[2]) < pProjMat->foregroundDistance)
			{
				offset = pPlane->d * pProjMat->invForegroundDistance;
			}
			/*
			// Else use the z=repPnt[2] plane (approximately)
			*/
			else
			{
				offset = pPlane->d * pProjMat->RCentre[2] *
									ApproxRecip(pPlane->repPnt[2]);
			}/*end else*/

			/*
			// The plane is intersected with either the foreground clip
			// distance, or the z= repZ plane.
			// Work out how large a naive function that specifies the line
			// equation (in the form Ax+By+C=0) will be for the extreme
			// X and Y values.
			// (also work out how small it will get... We may be able to
			// reject the whole plane, or if we are lucky, the entire
			// object).
			*/
			middle = (centreDotNorm - offset);
				
			/*
			// Is this object is completely off the PSEUDO 
			// RENDERED RECT / DEVICE?
			//
			// Effectively, get the minimum value, i.e.
			//     minValue = middle - MaxBorder;
			// and see if this is positive.
			// i.e.
			//     minValue = middle - MaxBorder >= 0;
			// i.e.
			*/
			if(middle >= MaxBorder)
			{
				DPF((DBG_MESSAGE, 
					"Perpendicular plane rejects object: d:%f  N:[%f,%f,%f]", 
					pPlane->d, pPlane->normal[0], 
					pPlane->normal[1], pPlane->normal[2] ));

			    SGL_TIME_STOP(PROJECTION_TIME)
				return TRUE;
			}
			/*
			// Is this plane off the pseudo rendered rect,
			// (but the object probably still on)
			//
			// This occurs when the maximum possible value is negative i.e.
			//		maxValue = middle + MaxBorder <= 0.0;
			// i.e.
			// 		middle <= - MaxBorder
			*/
			else if(middle <= -MaxBorder)
			{
				DPF((DBG_MESSAGE, 
					"Perp plane off device - ignoring it: d:%f  N:[%f,%f,%f]", 
					pPlane->d, pPlane->normal[0], 
					pPlane->normal[1], pPlane->normal[2] ));

				/*
				// ignore this plane, but continue with the others
				*/
				continue;
			}/*end special rejection cases for perp planes*/
			
			
			/*
			// Note that we negate here, because the hardware states
			// and object is inside when the function > 0, while we
			// are inside the object when N.X < 0...
			*/
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			maxValue = -fOverflowRescale;

			/* PCX2 hardware performs scaling when planes are perpendicular.
			 */
			pPlane->f32A = (pPlane->normal[0] * xPerPixel * maxValue);
			pPlane->f32B = (pPlane->normal[1] * yPerPixel * maxValue);
			pPlane->f32C = ((middle - (XBorder + YBorder)) * maxValue);
#else
			/*
			// Generate a scaling value based on the max Value. Effectively
			// we want to divide through by the ABSOLUTE  max value so that
			//  we don't exceed the limits of the fixed point hardware. 
			*/
			maxValue = sfabs(middle) + MaxBorder;
			maxValue = -fOverflowRescale * ApproxRecip(maxValue);

		    SGL_TIME_SUSPEND(PROJECTION_TIME)

			pPlane->n32A = PackTo20Bit(pPlane->normal[0] * xPerPixel * maxValue);
			pPlane->n32B = PackTo20Bit(pPlane->normal[1] * yPerPixel * maxValue);

		    SGL_TIME_RESUME(PROJECTION_TIME)

			pPlane->n32C = (sgl_int32)((middle - (XBorder + YBorder))*maxValue*FLOAT_TO_FIXED);
#endif


	 		/*
			// add it to the perpendicular plane list
			*/
			PlaneLists->PEPlanes[PlaneLists->NumPE] = pPlane;
			PlaneLists->NumPE ++;

		} /*if then else*/

	}/* END FOR stepping through planes*/


	/*
	// If we were to ignore the visibility flags,
	// then merge the invisible with the visible. This is
	// only used for shadows etc, so is less common.
	*/
	if(ignore_visibility)
	{
		/*
		// Copy over the foward invisible
		*/
		for(i = 0; i < PlaneLists->NumFI; i++)
		{
			*ppFVplane = PlaneLists->FIPlanes[i];
			ppFVplane ++;
		}
		LocalFVAffected += PlaneLists->NumFI;
		PlaneLists->NumFI = 0;

		/*
		// Copy over the reverse invisible
		*/
		for(i = 0; i < PlaneLists->NumRI; i++)
		{
			*ppRVplane = PlaneLists->RIPlanes[i];
			ppRVplane ++;
		}
		LocalRVAffected += PlaneLists->NumRI;
		PlaneLists->NumRI = 0;

	}/*end if ignore visiblility*/


	/*
	// Return the number of actual visible forward and reverse planes.
	// This is needed for the local material lists
	*/
	*pFVAffected= LocalFVAffected;
	*pRVAffected= LocalRVAffected;

	/*
	// Add on the number of forward and reverse visibles as well
	*/
	PlaneLists->NumFV+= LocalFVAffected;
	PlaneLists->NumRV+= LocalRVAffected;
	
	/*
	// Object is on screen, so return FALSE
	*/
	SGL_TIME_STOP(PROJECTION_TIME)
	return FALSE;
		
} /* RnProjectAndClassifyPlanes */


/**************************************************************************
* Function Name  : RnProjectAndClassifyMaterialsPlanes (LOCAL FUNCTION)
* Inputs         : Convex object, 
*					
* Outputs        : Lists of pointers to the transformed planes, and number
*					in each list
*
* Input/Output	  : Transformed plane structure
* Returns        : TRUE if the object is actually OFFSCREEN
*
* Global Used    : Projection "matrix" values
*
* Description    : This projects the transformed planes, and at the same time
*					classifies them into forward, reverse, perpendicular, etc
*
*				For the materials, it keeps track of the numbers of FV and
*				RV planes are affected by that material.	
*
*
**************************************************************************/

static sgl_bool RnProjectAndClassifyMaterialsPlanes(
							const CONVEX_NODE_STRUCT *pConvex,
							TRANS_PLANE_ARRAY_TYPE transformedPlanes,

							const LOCAL_MATERIALS_STRUCT *pLocalMaterials,
							const int  NumLocalMats,

							PLANE_CATEGORIES_STRUCT * PlaneLists,
							USED_MATERIAL_TAB UsedMatTab)
{
	int matCount;
	int NumPlanesAffected;

	TRANSFORMED_PLANE_STRUCT * 	pPlane;
	CONV_SHADING_STRUCT * pShadingData;
	CONV_TEXTURE_UNION  * pTextureData;
	CONV_POINTS_STRUCT  * pPointsData;

	/*
	// A pointer so we can step through the used material table easily
	*/
	USED_MATERIAL_ENTRY_STRUCT *pUsedMat;

	/*
	// Initialise the number of planes in each category list
	*/
	PlaneLists->NumFV = 0;
	PlaneLists->NumFI = 0;
	PlaneLists->NumRV = 0;
	PlaneLists->NumRI = 0;
	PlaneLists->NumPE = 0;
	
	/*
	// Initialise pointer to first used material
	*/
	pUsedMat = UsedMatTab;

	/*
	// Set up a pointer to the first plane of the list, and to the
	// texture and shading data associated with it. Note that these
	// could be NULL... it is up to the other routines that read the
	// the transformedPlanes struct NOT to use those pointers.
	*/
	pPlane = transformedPlanes;
	pShadingData = pConvex->shading_data;
	pTextureData = pConvex->texture_data;
	pPointsData = pConvex->points_data;

	/*
	// Step through all the local materials
	*/
	for(matCount = 0; matCount < NumLocalMats; matCount ++)
	{
		/*
		// get the number of planes to step through for this material
		// Also save the pointer to the material node, while we're at it.
		*/
		NumPlanesAffected = pLocalMaterials->planes_affected;
		pUsedMat->pMaterialNode = pLocalMaterials->p_material;

		/*
		//Check material validity
		*/
		ASSERT(pUsedMat->pMaterialNode == NULL || 
			   pUsedMat->pMaterialNode->node_hdr.n16_node_type == nt_material);

		/*
		// Project and classify this batch of planes
		*/
		if(RnProjectAndClassifyPlanes(NumPlanesAffected,
									pPlane, pShadingData, pTextureData, pPointsData,
									FALSE,
									PlaneLists,
									&pUsedMat->FVPlanesAffected, 
									&pUsedMat->RVPlanesAffected))
		{
			/*
			// If the function returns TRUE, then we are off screen
			*/
			return TRUE;
		} /*end if actually offscreen*/

		/* set pointer to points data */
		pPlane->pPointsData  =  pPointsData;

		/*
		// Move on to the next set of planes
		*/
		pPlane += NumPlanesAffected;
		pShadingData += NumPlanesAffected;
		pTextureData += NumPlanesAffected;
		pPointsData += NumPlanesAffected;

		/*
		// And the next material
		*/
		pUsedMat ++;
		pLocalMaterials ++;

	}/*end for step through local materials*/


	/*
	// Not off screen so return FALSE
	*/
	return FALSE;
}


/**************************************************************************
* Function Name  : RnProjectDifficultRepPoint (LOCAL FUNCTION)
* Inputs         : 
* Outputs        : 
* Input/Output	 : A difficult plane
* Returns        : None
* Global Used    : Projection matrix (read only)
*
* Description    : When RnProjectRepPoints has a "bad" rep point, this
*				   routine is called to choose an alternative and project
*				   that instead.
*
*					As most objects will be on screen, this routine won't
*					be called too often (hopefully!).
*
*		There are two cases to consider	
*
*		1) Where the plane doesn't get cut by the front clipping plane..
*		   In this situation we can fire rays through the corners of the
*		   screen/viewport and determine which one is best to use as the
*		   the rep point.
*
*		2) It does cut the front clipping plane. In this case the corners
*		   probably won't give sensible results so we need to do something else
*
*			Basically this bit computes the intersection of 3 planes..
*			the plane, a plane with fixed Z, and with each of the planes
*			forming	the viewing pyramid in turn.
*
*			This is done by solving the following equation
*
*
*
*			[	0		0		1	]	[X]		[CZ]
*			[	EX		EY		EZ	]	[Y]	 =	[0]
*			[	PX		PY		PZ	]	[Z]		[d]
*
*
*	where CZ is the Z value of the Constant Z Plane (with d of 1),
*	EX,EY,EZ is the normal of an edge (view pyramid) plane (which has a d=0)
*	and the Plane P, with Normal PX,PY,PZ and d of, well, d.
*	
*
*	Inverting the Matrix bit above we get....
*
*				1				[ EY.PZ-EZ.PY	 PY		-EY	]
*		--------------------	[-EX.PZ+EZ.PX	-PX		 EX	]
*		  (EX.PY  - EY.PX)		[ EX.PY-EY.PX	  0		  0	]
*
*
*		So we know we dont have an intersection if 
		(EX.PY  - EY.PX) IS 0 (or V. Small)
*
*		Calling the 1/det "G", we then have to perform
*
*		[ EY.PZ-EZ.PY    PY  -EY  ]  [CZ]
*	G	[-EX.PZ+EZ.PX   -PX   EX  ]  [0]  
*		[ EX.PY-EY.PX     0   0   ]  [d]
*
*
*		which gives
*			  [CZ( EY.PZ-EZ.PY)  - d EY]
*		   G  [CZ(-EX.PZ+EZ.PX)	 + d EX]
*			  [CZ( EX.PY-EY.PX)	 	  ]
*
*		which is
*			  [ G (CZ ( EY.PZ-EZ.PY)  - d EY) ]
*			  [ G (CZ (-EX.PZ+EZ.PX)  + d EX) ]
*			  [	  CZ						  ]
*
*
*		Now it seems to me we can either choose the closest new 3D rep point,
*		to the original rep point, or perhaps the one which has the "smallest"
*		G value, as that probably will be close to the origin, and be well
*		behaved.   That method will be chosen.
*
*
*		Now to project the point, we have to divide through by Z, so we get...
*
*		Proj X = G ( EY.PZ-EZ.PY  - d EY/CZ) * Sx + 0x
*		Proj Y = G (-EX.PZ+EZ.PX  + d EX/CZ) * Sy + 0y
*
* 			***     A work of Art, No?  (Simon)	***
*		
**************************************************************************/
static void	RnProjectDifficultRepPoint( TRANSFORMED_PLANE_STRUCT * pPlane)
{
	float dets[4];
	int i;

	/* 
	// The best projected values so far 
	*/
	float prX = 0.0f;
	float prY = 0.0f;

	/*
	// Pointer to the normal vector for the edges of the viewing matrix
	*/
	float * pEN;

	/*
	// The "best" determinant so far
	*/
	float LargestDetSoFar = 1.0E-10f;
	int	  BestEdgeOrCorner = -1;


	/*
	// Inverse of the plane D value.
	*/
	float invD;
	float centreDotNorm, XBorder, YBorder;
	float OneOnT, tmp;

	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();

	SGL_TIME_START(PROJECT_REP_TIME)

	/*
	// First try intersecting rays through the corners with the plane
	// Get inverse of D : This will NOT be zero because that
	// would be a perpendicular plane which will have been rejected.
	*/
	ASSERT(pPlane->d != 0.0f);
	invD = 1.0f / pPlane->d;

	/*
	// Prepare the intersection
	*/
	centreDotNorm = DotProd(pPlane->normal, pProjMat->RCentre) * invD;
	XBorder = pPlane->normal[0] * pProjMat->xToCorner * invD;
	YBorder = pPlane->normal[1] * pProjMat->yToCorner * invD;

	/*
	// Try each of the corners. First do Top Left.
	//
	// Note, these calculations give the "hw" inverse depth value.
	// So if it is positive and < 1/20th (say) it should be in front of
	// the clipping plane.
	//
	// Look at the top left corner
	*/
	OneOnT = (centreDotNorm - XBorder - YBorder);
	if((OneOnT > 0.0f))
	{
		BestEdgeOrCorner = 0;
	}
	else
	{
		OneOnT = 0.0f;
	}

	/*
	// Look at the top right.
	*/
	tmp = (centreDotNorm + XBorder - YBorder);
	if(tmp > OneOnT) 
	{
		OneOnT = tmp;
		BestEdgeOrCorner = 1;
	}
	
	/*
	// Look at the bottom left.
	*/
	tmp = (centreDotNorm - XBorder + YBorder);
	if(tmp > OneOnT) 
	{
		OneOnT = tmp;
		BestEdgeOrCorner = 2;
	}
	
	/*
	// Look at the bottom right.
	*/
	tmp = (centreDotNorm + XBorder + YBorder);
	if(tmp > OneOnT) 
	{
		OneOnT = tmp;
		BestEdgeOrCorner = 3;
	}
	


	/*
	// If Best edge or corner is sensible, and we haven't
	// gone through the front clipping plane...
	//
	// I should really KNOW where the corners of the screen
	// are but for the moment I don't know what that variable is, so
	// I'll project it !!!
	*/
	if((BestEdgeOrCorner != -1) && (OneOnT < 0.05f))
	{
		DPF((DBG_MESSAGE, "Have Chosen Corner %d with 1/t of %f",
						BestEdgeOrCorner, OneOnT));
		switch(BestEdgeOrCorner)
		{

			/* top Left */
			case 0:
			{
				prX = pProjMat->fViewportMinX;
				prY = pProjMat->fViewportMaxY;
				break;
			}

			/* top Right */
			case 1:
			{
				prX = pProjMat->fViewportMaxX;
				prY = pProjMat->fViewportMaxY;
				break;
			}

			/* Bot Left */
			case 2:
			{
				prX = pProjMat->fViewportMinX;
				prY = pProjMat->fViewportMinY;
				break;
			}

			/* Bot Right */
			default:
			{
				ASSERT(BestEdgeOrCorner == 3);
				prX = pProjMat->fViewportMaxX;
				prY = pProjMat->fViewportMinY;
				break;
			}
		}/*end switch*/

		/*
		// Project these: Really I should KNOW what the pixel coords of
		// the corners are but, I can't rememebr where to find that at the
		// moment and I'm in a big hurry for Comdex (SJF)
		*/
		prX = prX * pProjMat->SxDash  + pProjMat->OxDash;
		prY = prY * pProjMat->SyDash  + pProjMat->OyDash;
	}
	/*
	// Else we are probably going through the clipping plane,
	// so use the alternative method
	*/
	else
	{
		DPF((DBG_MESSAGE, "Probably Clipped (1/t was %f)", OneOnT));
		/*
		// Set the pointer to the first view pyramids side's normals
		*/
		pEN = pProjMat->EdgeNormals[0];
		BestEdgeOrCorner = -1;

		/*
		// Step through the edges of the viewing pyramid
		*/
		for(i=0; i < 4; i++, pEN+=3)
		{
			/*
			// Calculate the determinant  (EX.PY  - EY.PX)
			*/
			dets[i] = pEN[0] * pPlane->normal[1]  -
					  pEN[1] * pPlane->normal[0];


			/*
			// is this larger than our best so far?
			*/
			if(sfabs(dets[i]) > LargestDetSoFar)
			{
				LargestDetSoFar = sfabs(dets[i]);
				BestEdgeOrCorner = i;
			}
 
		}/*end for*/


		/*
		// OK, if we have a decent edge to intersect with, then use it
		*/
		if(BestEdgeOrCorner >= 0)
		{
			float InvDet;
			float dOn4Cz;
			/*
			// get pointer to the edge
			*/
			pEN = pProjMat->EdgeNormals[BestEdgeOrCorner];


			DPF((DBG_VERBOSE,"Choosing edge %d (Det was %f)", 
						 BestEdgeOrCorner, dets[BestEdgeOrCorner]));

			/*
			// get the inverse Determinant
			*/
			InvDet = 1.0f / dets[BestEdgeOrCorner];

			dOn4Cz = pPlane->d * pProjMat->invForegroundDistance;/* * 0.01f;*/

		#if DEBUG
		{
			sgl_vector New;

			/* Do some sanity checks */

			New[0]= pProjMat->foregroundDistance * 
				  (pEN[1] * pPlane->normal[2] -  pEN[2] * pPlane->normal[1]) - 
				  pPlane->d * pEN[1];
			New[0] = New[0] * InvDet;

			New[1]= pProjMat->foregroundDistance * 
				  (-pEN[0] * pPlane->normal[2] +  pEN[2] * pPlane->normal[0]) + 
				  pPlane->d * pEN[0];

			New[1] = New[1] * InvDet;

			/*
			// And calculate Z to see if it's all worked
			*/
			New[2] = InvDet * pProjMat->foregroundDistance *
				 (pEN[0] * pPlane->normal[1] - pEN[1] * pPlane->normal[0]);

			/*
			// Dummy shite so I cansee the results
			*/
			New[0] = New[0] * 1.0f;
		}
		#endif



			/*
			// Project this point
			//
			// Proj X = G ( EY.PZ-EZ.PY  - d EY/CZ) * Sx + 0x
			// Proj Y = G (-EX.PZ+EZ.PX  + d EX/CZ) * Sy + 0y
			//
			// Now we want a point which will be nearly on screen.
			// 
			// Since we think we are cutting the clipping plane, choose a
			// Z value of say ???? 10 * the clipping plane distance.
			//
			*/
			prX = InvDet * ( pEN[1] * pPlane->normal[2] - 
							 pEN[2] * pPlane->normal[1] -
							 dOn4Cz * pEN[1]);
			prX = prX * pProjMat->SxDash  + pProjMat->OxDash;

			/* 
			// note -EX.PZ+EZ.PX => EZ.PX-EX.PZ 
			*/
			prY = InvDet * ( pEN[2] * pPlane->normal[0] - 
						 pEN[0] * pPlane->normal[2] +
						 dOn4Cz * pEN[0]);

			prY = prY * pProjMat->SyDash  + pProjMat->OyDash;

		}/*end is sensible edge*/
		#if DEBUG
		else
		{
			DPF((DBG_ERROR, "No suitable screen edge found for mapping on screen"));
		}
		#endif	

	}/*end else corners weren't suitable so use alternative method*/

	/*
	// Store the result (note, store a "projected" Z -- don't know why really)
	*/
	pPlane->projRepPoint[0] =  prX;
	pPlane->projRepPoint[1] =  prY;
	pPlane->projRepPoint[2] =  1.0f; /*NOT CORRECT!!!!!*/
	
}

/**************************************************************************
* Function Name  : RnProjectRepPoints (LOCAL FUNCTION)
* Inputs         : Number of planes to process
* Outputs        : 
* Input/Output	  :list of pointers to transformed planes
* Returns        : None
* Global Used    : Projection matrix (read only)
*
* Description    : This projects the representative points of the
*				   the planes, clipping them to the device coordinates.
*				   The "z" value is set RELATIVE TO THE a camera plane at
*				   distance 1.0 (not Cd).
**************************************************************************/
static void	RnProjectRepPoints( TRANSFORMED_PLANE_STRUCT * Planes[],
								const int NumPlanesToDo)
{
	int i;
	TRANSFORMED_PLANE_STRUCT * pPlane;
	TRANSFORMED_PLANE_STRUCT ** ppPlane;
	/*
	// grab the projection matrix values from the global matrix
	// (This could be made processor dependant based on number of
	// FP registers)
	*/
	float SxDash, SyDash, OxDash, OyDash;


	float  DevXMax, DevYMax;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();

	SGL_TIME_START(PROJECT_REP_TIME)

	SxDash = pProjMat->SxDash;
	SyDash = pProjMat->SyDash;
	OxDash = pProjMat->OxDash;
	OyDash = pProjMat->OyDash;


	/*
	// its Ok if the projected rep point is not too far off screen
	// as it is used mainly for determining the mip mapping level
	*/
	DevXMax= pProjMat->fDevXMax * 3.0f;
	DevYMax= pProjMat->fDevYMax * 3.0f;

	ppPlane = Planes;

	for(i = 0; i < NumPlanesToDo; i ++, ppPlane++)
	{
		float x,y,z;

		/*
		// get access to the plane
		*/
		pPlane = *ppPlane;

		/* Initialise the bNewRepPoint element */
		pPlane->bNewProjRepPoint = FALSE;

		z = pPlane->repPnt[2];
		/*
		// Project the rotter
		//
		// Do the easy case, where z is in front of the clipping plane
		*/
		if( z >= pProjMat->foregroundDistance)
		{
			/*
			// get the inverse of z
			*/
			z = 1.0f / z;

			/*
			// project x and y, and store z
			*/
			x = SxDash * pPlane->repPnt[0] * z + OxDash;
			y = SyDash * pPlane->repPnt[1] * z + OyDash;

			pPlane->projRepPoint[2] = z;

			/*
			// If the rep point is close enough to being on screen,
			// stick with it, else choose another one
			//
			// Note: Use bit wise OR to reduce branching costs.
			*/
			if( (x < 0) | (x > DevXMax) | 
				(y < 0) | (y > DevYMax) )
			{
				DPF((DBG_MESSAGE, "RnPRPnt TOO Far off screen"));
				RnProjectDifficultRepPoint(pPlane);

				/* When the ProjRepPt is off screen and not Z Clipped 
				** Set Smooth Shading RepPt flag and store the true
				** projRepPt required for the Smooth Shading algorithm.
				*/ 
				pPlane->bNewProjRepPoint = TRUE;

				pPlane->vNewProjRepPoint[0] = x;
				pPlane->vNewProjRepPoint[1] = y;
				pPlane->vNewProjRepPoint[2] = z;

			}
			/*
			// Else the projected rep point is tolerable, save it
			*/
			else
			{
				pPlane->projRepPoint[0] =  x;
				pPlane->projRepPoint[1] =  y;

			}
		}
		/*
		// Else the rep point is in front of the clipping plane, 
		// use the tricky case routine.
		*/
		else
		{
			/* When the ProjRepPt is Z Clipped ( i.e. in front of CP ) 
			** Set Smooth Shading RepPt flag and store the true
			** projRepPt required for the Smooth Shading algorithm.
			*/ 
			pPlane->bNewProjRepPoint = TRUE;

			z = 1.0f / z;
			x = SxDash * pPlane->repPnt[0] * z + OxDash;
			y = SyDash * pPlane->repPnt[1] * z + OyDash;

			DPF((DBG_MESSAGE, "RnPRPnt in front of the Clip Plane"));
			RnProjectDifficultRepPoint(pPlane);

			pPlane->vNewProjRepPoint[0] = x;
			pPlane->vNewProjRepPoint[1] = y;
			pPlane->vNewProjRepPoint[2] = z;

		} /*end if else z > 0 */
		
	}/*end for*/

	SGL_TIME_STOP(PROJECT_REP_TIME)
}


/**************************************************************************
* Function Name  : RnBatchShadeAndTexture (LOCAL FUNCTION)
* Inputs         : Number of planes to texture/shade,
*				   Lights, material, and transform states
* Outputs        : 
*
* Input/Output	  :list of pointers to transformed planes
* Returns        : None
* Global Used    : world space 
*
* Description    :This shades and textures the supplied planes, and
*				 packs and combines the texture and shading results,
*				 putting it into the hardware parameter space. Note that
*				 the location of each planes texture/shading info is put
*				 in its TEXAS TAG field.
*
*
*
**************************************************************************/

static void RnBatchShadeAndTexture(TRANSFORMED_PLANE_STRUCT * Planes[],
		const int NumPlanesToDo,
		const sgl_bool ReversePlanes,
		
		/*
		// The material state may be a local material,so must
		// be supplied separately to the rest of the state
		*/
		/*const*/ MATERIAL_STATE_STRUCT * pMaterialState,
		
		/*const*/ MASTER_STATE_STRUCT	* pState,
			  BATCH_TEXTURE_MODES_ENUM OverallTextureMode, 
			  BATCH_SHADING_MODES_ENUM OverallShadingMode,
			  sgl_bool AccurateTex)

{
	#if DEBUG
	static int TextQualityInit = 0;
	static int TextQuality = 1;
	#endif


	#if DEBUG
	if (!TextQualityInit)
	{
		TextQualityInit = 1;
		TextQuality = SglReadPrivateProfileInt ("Quality", "ConvexTexturing", TextQuality, "sgl.ini");
	}

	if (!TextQuality)
	{
		OverallTextureMode = NO_TEXTURING;
	}
	#endif

	/*
	// Do we need to project rep points?
	*/
	if((OverallTextureMode != NO_TEXTURING) || 
	   (OverallShadingMode == SMOOTH_SHADING))
	{
		RnProjectRepPoints(Planes, NumPlanesToDo);

		/*
		// Do we need to update the local projection matrix
		*/
		if(!RnGlobalLocalProjMatIsValid())
		{
			DPF((DBG_MESSAGE, "Update LT mat"));
			RnUpdateLocalProjectionMatrix(pState->pTransformState);
		}

	} /*end if*/


	/*
	// Texture it accordingly
	*/
	if(OverallTextureMode == NO_TEXTURING)
	{
		DPF((DBG_MESSAGE, " BatchShadeA..: No Texturing on Obj"));
		
		/*
		// We just need to check if translucency is on, because
		// the hardware can only do this by using a predefined
		// translucent texture! If translucent, then do a routine
		// to generate translucency values as textures, and then
		// pretend it hsa been textured.
		*/
		DPFOO((DBG_MESSAGE, "BatchShadeA..: Not Testing for Transluceny"));
		/* if(pMaterialState->  etc*/
	}
	/*
	// Else it is some sort of texturing
	*/
	else
	{
		int ThisTextureSize;

		DPF((DBG_MESSAGE, " BatchShadeA..: TEXTURING on Obj"));


		ThisTextureSize = TexasGetTextureDimension(&pMaterialState->texas_precomp);
		/*
		// Do we need change the texture projection bit of the
		// local projection matrix for a different sized texture?
		*/
		if(!RnGlobalLocalProjMatTextureIsValid(ThisTextureSize))
		{
			RnRescaleLocalProjectionTexture(ThisTextureSize, 
											pState->pTransformState);
		}

		/*
		// Do we do premapped texture wrapping?
		*/
		if(OverallTextureMode == PREMAPPED_TEXTURING)
		{
			DPF((DBG_MESSAGE, " BatchShadeA..: PREMAPPED on Obj"));

			if(AccurateTex)
			{
				DoTextureMappingAccurate( NumPlanesToDo, 
					(const TRANSFORMED_PLANE_STRUCT *const*) Planes, 
					pState->pTransformState, pMaterialState, 
					GlobalTextureResults );

			}
			else
			{
				DoTextureMappingFast( NumPlanesToDo, 
					(const TRANSFORMED_PLANE_STRUCT *const*) Planes, 
					pState->pTransformState, pMaterialState,
					GlobalTextureResults );
			}
		}
		/*
		// Else, is it texture wrapping
		*/
		else
		{
			DPF((DBG_MESSAGE, " BatchShadeA..: WRAP on Obj"));
			ASSERT(OverallTextureMode == WRAP_TEXTURING);

			DoTextureWrapping(NumPlanesToDo, Planes, pState->pTransformState,
						 pMaterialState, GlobalTextureResults);
		}/*end if else*/
		
	}/*end if else*/

	/*
	// Shade it appropriately
	*/
	if(OverallShadingMode == FLAT_SHADING)
	{
		if (OverallTextureMode != NO_TEXTURING)
		{
			DoFlatTextureShading(ReversePlanes,	
					   		 NumPlanesToDo, Planes, 
							 pState->pTransformState,
					   		 pMaterialState, 
							 pState->pLightsState, 
					   		 GlobalShadingResults);
   		}
		else
		{
			DoFlatShading(ReversePlanes,	
					   		 NumPlanesToDo, Planes, 
							 pState->pTransformState,
					   		 pMaterialState, 
							 pState->pLightsState, 
					   		 GlobalShadingResults);
		}
	}
	else
	{
		ASSERT(OverallShadingMode == SMOOTH_SHADING);
		
		GetSmoothShadingParameters (NumPlanesToDo,
								 	Planes,
									pState->pTransformState,
					   		 		pMaterialState,
									pState->pLightsState, 
								 	GlobalShadingResults,
								 	&gSP);
	}
	

	/*
	// Pack your parameters in your old kit bag and smile, smile, smile
	//
	// If there is texturing...
	*/
	if( (OverallTextureMode != NO_TEXTURING) )
	{
		/*
		// If just flat shading
		*/
		if(OverallShadingMode != SMOOTH_SHADING) 
		{
			PackTexasParamsFlatTextured(NumPlanesToDo, GlobalTextureResults, 
												GlobalShadingResults,
				/* Is shadowed*/
				(pState->pLightsState->flags & lsf_shadows),


				pState->pQualityState->flags & qf_fog, /*Set fog flag correctly*/
				Planes, 	pMaterialState);
		}
		/*
		// Else smooooooooth shade away
		*/
		else
		{
			PackTexasParamsSmoothTextured(NumPlanesToDo, GlobalTextureResults, 
												GlobalShadingResults,
				/* Is shadowed*/
				(pState->pLightsState->flags & lsf_shadows),


				pState->pQualityState->flags & qf_fog, /*Set fog flag correctly*/
				Planes, 	pMaterialState);

		}

	}
	/*
	// Else do the non textured routine
	*/
	else
	{
		sgl_bool DoFog = pState->pQualityState->flags & qf_fog;
		sgl_bool DoShadows = pState->pLightsState->flags & lsf_shadows;
		
		if(OverallShadingMode == SMOOTH_SHADING)
		{
			if(DoShadows)
			{
 				PackTexasParamsSmoothNonTexturedShad(NumPlanesToDo, 
													 GlobalShadingResults,
													 DoFog,
													 Planes);
			}
			else
			{
 				PackTexasParamsSmoothNonTextured(NumPlanesToDo, 
												 GlobalShadingResults,
												 DoFog,
												 Planes);
			}
		}
		else
		{
			if(pMaterialState->texas_precomp.LowWord & MASK_TRANS)
			{
				PackTexasParamsFlatNonTexturedTrans(NumPlanesToDo, 
											   GlobalShadingResults,
											   DoShadows,
											   DoFog,
											   pMaterialState->translucent_int,
											   Planes);
			}
			else
			{
				PackTexasParamsFlatNonTextured(NumPlanesToDo, 
											   GlobalShadingResults,
											   DoShadows,
											   DoFog,
											   Planes);
			}
		}
	}
}


/**************************************************************************
* Function Name  : RnIndivShadeAndTexture (LOCAL FUNCTION)
* Inputs         : Number of planes to texture/shade,
*				   Lights, material, and transform states
* Outputs        : 
*
* Input/Output	  :list of pointers to transformed planes
* Returns        : None
* Global Used    : world space 
*
* Description    :This shades and textures the supplied planes, and
*				 packs and combines the texture and shading results,
*				 putting it into the hardware parameter space. Note that
*				 the location of each planes texture/shading info is put
*				 in its TEXAS TAG field.
**************************************************************************/

static void RnIndivShadeAndTexture(TRANSFORMED_PLANE_STRUCT * Planes[],
		const int NumPlanesToDo,
		const sgl_bool ReversePlanes,
		/*const*/ MATERIAL_STATE_STRUCT * pMaterialState,
		/*const*/ MASTER_STATE_STRUCT	* pState,

		const BATCH_TEXTURE_MODES_ENUM OverallTextureMode, 
		const BATCH_SHADING_MODES_ENUM OverallShadingMode,
		sgl_bool AccurateTex)

{
	int i;
	BATCH_TEXTURE_MODES_ENUM IndividualTextureMode; 
	BATCH_SHADING_MODES_ENUM IndividualShadingMode;

	/*
	// step through the planes
	*/
	for(i=0; i < NumPlanesToDo; i++)
	{
		/*
		// determine the characteristics of this plane
		*/
		if((OverallShadingMode == FLAT_SHADING) ||
			((Planes[i]->flags & pf_smooth_shad) == 0))
		{
			IndividualShadingMode = FLAT_SHADING;
		}
		else
		{
			IndividualShadingMode = SMOOTH_SHADING;
		}

		/*
		// determine the characteristics of the texturing
		*/
		if(OverallTextureMode != MIXED_TEXTURING)
		{
			 IndividualTextureMode = OverallTextureMode;
		}
		else if(Planes[i]->flags & pf_pre_mapped)
		{
			IndividualTextureMode = PREMAPPED_TEXTURING;
		}
		else if((Planes[i]->flags & pf_textured) && 
				(pMaterialState->texture_flags & wrapping_on))
		{
			IndividualTextureMode = WRAP_TEXTURING;
		}
		else
		{
			IndividualTextureMode = NO_TEXTURING;
			
		} /*end else*/

		RnBatchShadeAndTexture(& Planes[i], 1,
						ReversePlanes,
						pMaterialState,
						pState,
						IndividualTextureMode, 
						IndividualShadingMode,
						AccurateTex);

	}/*End for planes*/

} /*end function*/




/**************************************************************************
//
// Categories of planes in Transformed Planes
//
// These are used by the Process Visible Convex and Process Shadow routines
//
**************************************************************************/
static PLANE_CATEGORIES_STRUCT PlaneCategories;


/*
// The following is used to work around a limitation in the first few
// versions of PVR. For some inexplicable reason, ANY visible plane that
// goes in the  translucent pass MUST have the translucent bit set EVEN if
// it is completely opaque. Oh Well. Never mind
*/
#define FIX_OPAQUE_TRANSLUCENCY_BUG 1

/*
// The following should also work around the "No smooth shading when
// translucent" limitation in the hardware. I don't know which version of
// the hardware this will be fixed in, but it certainly doesn't work for
// Midas3 & PCX1 (old PVR1 & PVR2).
//
// NOTE this also ONLY works if FIX_OPAQUE_TRANSLUCENCY_BUG is set
*/
#define FIX_NO_SMOOTH_TRANSLUCENT_BUG 1

/**************************************************************************
* Function Name  : RnProcessVisibleConvex
* Inputs         : Number of planes in convex, 
*				 : bounding box (if any)
*				   clipping flag
*					
* Outputs        : pOffscreen - returns TRUE if the object gets rejected.
*
* Input/Output	 : Transformed plane structure
*
* Returns        : TRUE if plane "processed" Ok (False if we ran out of
*				   parameter space etc). If we get FALSE then it is time to
*				   stop rendering because we are out of space. (Although
*				   I suppose we could still do collision detection.
*
* Global Used    : Projection "matrix" values, and the parameter stores
*
* Description    : This does the processing for a normal visible object, 
*				   projecting, shading and texturing it as necessary.
*
*					THIS ROUTINE IS NOT RE_ENTRANT as it has static variables
*					(Not that it would make	any sence to do that anyway...)
**************************************************************************/
sgl_bool RnProcessVisibleConvex( const CONVEX_NODE_STRUCT  *pConvex,
								 const MASTER_STATE_STRUCT *pState,
								 TRANS_PLANE_ARRAY_TYPE transformedPlanes, 
								 const sgl_bool  NeedsClipping,
								 const BBOX_MINMAX_STRUCT *pBBox,
								 sgl_bool * pOffscreen,
								 int nCurrTransSetID)
{
	/*
	// Keep count of the number of planes in each category
	*/
	int numPlanes;
	int NumFV, NumFI, NumRV;
	int i;

	/*
	// Pointers to the	where we are up to in the texturing/shading of
	// the forward and reverse visible planes. Note that these point
	// into an array of pointers.
	*/
	TRANSFORMED_PLANE_STRUCT ** pForwardsDone, **pReversesDone;

	/*
	// To uniformly handle the cases where a convex plane has
	// no local material, set up a an equivalent dummy local
	// material.
	// Also, local materials are not allowed to affect the 
	// global material state. Therefore, we either use the real
	// material state, or a local copy.
	*/
	LOCAL_MATERIALS_STRUCT * pLocalMaterials;
	int NumLocalMats;
	
	MATERIAL_STATE_STRUCT TempMaterialState;
	MATERIAL_STATE_STRUCT *pMaterialState;

	BATCH_SHADING_MODES_ENUM OverallShadingMode;
	BATCH_TEXTURE_MODES_ENUM OverallTextureMode,
							 BatchTextureMode;

	/*
	// Where in Sabre param space does this start?, and how many planes
	// are ACTUALLY put in the hardware parameter memory.
	*/

	sgl_uint32 PrimStart;
	int NumPlanesAddedToHW;
	
	/*
	// Which regions does this object cover - for simplicty do
	// a rectangle of them.
	*/
	REGIONS_RECT_STRUCT Regions;

	/*
	// various booleans
	*/
	sgl_bool IgnoreReverseVis;
	sgl_bool HasTranslucent = FALSE;

	/* a pointer to the projection_matrix */
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();


	/*****************
	// How many planes are we dealing with then...
	*****************/
	numPlanes = pConvex->u16_num_planes;

	SGL_TIME_START(TRANSFORM_VISIBLE_CONVEX_TIME)

	/*****************
	// take special action if there are no local materials.
	// Pretend there is one, and that it is applied to all
	// the planes.
	// Note that a NULL material pointer implies use of the current
	// material state.
	*****************/
	if(pConvex->u16_num_materials == 0)
	{
		DPF((DBG_VERBOSE, "No local Materials"));
		NumLocalMats = 1;

		/*
		// we can use the real material state
		*/
		pMaterialState = pState->pMaterialState;

		UsedMaterials[0].pMaterialNode = NULL;

		/*****************
		// Project and classify the planes, assuming a single material
		*****************/
		PlaneCategories.NumFV = 0;
		PlaneCategories.NumFI = 0;
		PlaneCategories.NumRV = 0;
		PlaneCategories.NumRI = 0;
		PlaneCategories.NumPE = 0;

	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
		if(RnProjectAndClassifyPlanes(numPlanes, transformedPlanes,
						pConvex->shading_data,
						pConvex->texture_data,
						pConvex->points_data,

						FALSE, /*use invisibility flags*/
						&PlaneCategories,
						&UsedMaterials[0].FVPlanesAffected,
						&UsedMaterials[0].RVPlanesAffected))
		{
			DPF((DBG_MESSAGE, "RnProcessVisibleConvex ignoring offscreen convex"));

			*pOffscreen = TRUE;
	        SGL_TIME_STOP(TRANSFORM_VISIBLE_CONVEX_TIME)
			return TRUE;
		}
	    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)


		#if FIX_OPAQUE_TRANSLUCENCY_BUG
			HasTranslucent = (pMaterialState->texas_precomp.LowWord & MASK_TRANS);
		#endif
	}
	/*
	// Else set our local material pointer to our local copy
	*/
	else
	{
		DPF((DBG_MESSAGE, "Using local Materials"));
	
		pLocalMaterials = pConvex->local_materials;
		NumLocalMats    = pConvex->u16_num_materials;

		ASSERT(NumLocalMats > 0)
		ASSERT(pLocalMaterials != NULL)

		/*
		// we AREN'T allowed to use the real material state,
		// so make a copy of the way it currently is, and use
		// that.
		*/
		TempMaterialState = *(pState->pMaterialState);
		pMaterialState = &TempMaterialState;

		/*****************
		// First Project and classify the planes, sorting out the materials
		// Note that if this returns TRUE then the primitive is offscreen 
		// and can be discarded
		*****************/
	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
		if(RnProjectAndClassifyMaterialsPlanes( pConvex, transformedPlanes,
								pLocalMaterials, NumLocalMats,
								&PlaneCategories,
								UsedMaterials))
		{
			DPF((DBG_MESSAGE, "RnProcessVisibleConvex ignoring offscreen convex"));

			*pOffscreen = TRUE;
	        SGL_TIME_STOP(TRANSFORM_VISIBLE_CONVEX_TIME)
			return TRUE;
		}
	    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)

		/*
		// Unfortunately, for some reason, we MUST have the translucent
		// bit set on ALL faces if the object goes into the translucent
		// pass: Even if a face is completely opaque!
		// The only way I could see to do this was to scan ahead.
		//
		// Note: It still won't work with smooth shading or non-textured
		//		 stuff.
		*/
		HasTranslucent = FALSE;
		#if FIX_OPAQUE_TRANSLUCENCY_BUG
		for(i = 0; i < NumLocalMats; i ++)
		{
			/*****************************
			// If the material state needs updating.
			// (Note: a NULL pointer effectively means 
			// to leave the state as it is.)
			*****************************/
			if(UsedMaterials[i].pMaterialNode != NULL)
			{
	            SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
				RnProcessMaterialNode( UsedMaterials[i].pMaterialNode,
								   pMaterialState,
								   (MASTER_STATE_STRUCT *) pState);
	            SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
			}

			/*
			// If any of the local materials are translucent, then put whole object
			// in translucent phase.
			*/
			HasTranslucent |= (pMaterialState->texas_precomp.LowWord & MASK_TRANS);
		}

		/*
		// reset the local material back to its starting value
		*/
		TempMaterialState = *(pState->pMaterialState);
			
		#endif FIX_OPAQUE_TRANSLUCENCY_BUG

		
	}/*end if else*/


	*pOffscreen = FALSE;

	DPF((DBG_MESSAGE, "Classified planes:FV:%d  FI:%d  RV:%d  RI:%d  PE:%d",
					  PlaneCategories.NumFV, PlaneCategories.NumFI, 
					  PlaneCategories.NumRV, PlaneCategories.NumRI, 
					  PlaneCategories.NumPE ));

	/*
	// For convenience (and speed) grab the number of Forward vis and reverse
	// visible planes
	*/
	NumFV = PlaneCategories.NumFV;
	NumRV = PlaneCategories.NumRV;
	NumFI = PlaneCategories.NumFI;

	/*
	// Can we only see inside the object but are not interested in seeing
	// the inside?
	// 
	// OR can we see inside the object, but there is nothing to see (i.e.
	// no visible reverse planes)
	*/
	if((NumFV == 0) && 
		 (((pConvex->u16_flags & cf_see_inside) == 0) || (NumRV == 0)))
	{
		DPF((DBG_MESSAGE,
			 "RnProcessVisibleConvex No visible forwards (and no insides)"));
		return TRUE;
	} /*end if ... quick rejection*/


	/*****************
	// Are we actually interested in shading the reverse visible planes 
	// at all?
	// We can ignore them if we can't see them (i.e. all forward planes
	// are visible, (and there are some) and we aren't clipping...) 
	// or if we just couldn't care about the inside, OR there aren't
	// any reverse visible planes anyway.
	*****************/
	IgnoreReverseVis = ((NumFV != 0) && (NumFI == 0) && (!NeedsClipping) ) ||
					((pConvex->u16_flags & cf_see_inside) == 0) || 
					(NumRV == 0);

	DPF((DBG_MESSAGE,
			 "RnProcessVisibleConvex Ignore Rev Value:%d",IgnoreReverseVis ));
	/*****************
	// Determine Likely Overall texturing modes for the primitive
	// If the quality  says no texturing, or there is no texturing data
	// at all in the primitive, then texturing is off like prawns in 
	// the sun.
	*****************/
	if( ((pState->pQualityState->flags & qf_textures) == 0) || 
      ((pConvex->texture_data == NULL) && (pConvex->points_data == NULL)) )
	{
		OverallTextureMode = NO_TEXTURING;
		DPF((DBG_VERBOSE,
		  "RnProcessVisibleConvex: No texturing: %d %d",
			(pState->pQualityState->flags & qf_textures == 0),
			(pConvex->texture_data == NULL) ));
	}
	/*
	// Else if all texturing is predefined...
	*/
	else if(pConvex->u16_flags & cf_all_premapped)
	{
		OverallTextureMode = PREMAPPED_TEXTURING;
		DPF((DBG_MESSAGE,
		 "RnProcessVisibleConvex: All premapped texturing" ));
	}
	/*
	// Else is all texture wrapping...
	*/
	else if(pConvex->u16_flags & cf_all_text_wrap)
	{
		OverallTextureMode = WRAP_TEXTURING;
		DPF((DBG_MESSAGE,
			 "RnProcessVisibleConvex: All texture wrapping" ));

	}
	/*
	// Else it looks like a mixed bag of texturing
	*/
	else
	{
		OverallTextureMode = MIXED_TEXTURING;
		DPF((DBG_MESSAGE,
		 "RnProcessVisibleConvex: MIXED texture wrapping" ));

	} /*end if else on texturing type*/



	/*****************
	// Determine Likely Overall Shading modes for the primitive
	// Is it definitely Flat Shaded?
	*****************/
	if( (pState->pQualityState->flags & qf_smooth_shading) == 0 || 
		(pConvex->shading_data == NULL))
	{
		OverallShadingMode = FLAT_SHADING;
		DPF((DBG_VERBOSE,
		 "RnProcessVisibleConvex: Flat Shading=>: %d %d",
			((pState->pQualityState->flags & qf_smooth_shading) == 0),
			(pConvex->shading_data == NULL) ));
	}
	/*
	// Else is it all smooth shaded?
	*/
	else if(pConvex->u16_flags & cf_all_smooth)
	{
		OverallShadingMode = SMOOTH_SHADING;
		DPF((DBG_MESSAGE,
		 "RnProcessVisibleConvex: Smooth Shading"));

	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
		SmoothObjectPrecalc (pState, &gSP);
	    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
	}
	/*
	// Else the shading is a bit of a mixed bag
	*/
	else
	{
		OverallShadingMode = MIXED_SHADING;
		DPF((DBG_MESSAGE,
		 "RnProcessVisibleConvex: Mixed Shading"));

	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
		SmoothObjectPrecalc (pState, &gSP);
	    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
	}/* end if else shading mode */



	#if FIX_NO_SMOOTH_TRANSLUCENT_BUG && FIX_OPAQUE_TRANSLUCENCY_BUG
		if(HasTranslucent)
		{
			OverallShadingMode = FLAT_SHADING;
		}
	#endif


	/*
	// Initialise the processed planes pointers
	*/
	pForwardsDone = PlaneCategories.FVPlanes;
	pReversesDone = PlaneCategories.RVPlanes;

	/*****************
	// Step through the materials, and shade the planes
	// affected appropriately
	*****************/
	for(i = 0; i < NumLocalMats; i ++)
	{
		/*****************************
		// If the material state needs updating.
		// (Note: a NULL pointer effectively means 
		// to leave the state as it is.)
		*****************************/
		if(UsedMaterials[i].pMaterialNode != NULL)
		{
	        SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
			RnProcessMaterialNode( UsedMaterials[i].pMaterialNode,
								   pMaterialState,
								   (MASTER_STATE_STRUCT *) pState);
	        SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
		}


		#if FIX_OPAQUE_TRANSLUCENCY_BUG
			/*
			// Ok if ANY of the materials in this object were translucent,
			// then mark them ALL as translucent. We also put the whole
			// object into the translucent pass.
			*/
			if(HasTranslucent)
			{
				pMaterialState->texas_precomp.LowWord |= MASK_TRANS;
			}
		#else
			/*
			// If any of the local materials are translucent, then put whole object
			// in translucent phase.
			*/
			HasTranslucent |= (pMaterialState->texas_precomp.LowWord & MASK_TRANS);
		#endif


		/*****************************
		// Determine the texturing requirements for this batch
		// of planes. If the material has no texturing, then we
		// can't possibly texture
		*****************************/
		if((pMaterialState->texture_flags & (sgl_uint32) texture_on) == 0)
		{
			 BatchTextureMode = NO_TEXTURING;
		}
		/*
		// Else if we are supposed to be doing texture wrappping, and
		// it is not set up in the material, then turn it off
		*/
		else if((OverallTextureMode == WRAP_TEXTURING) && 
				!(pMaterialState->texture_flags & wrapping_on))
		{
			 BatchTextureMode = NO_TEXTURING;
		}
		/*
		// Else just use the overall setting.
		*/
		else
		{
			 BatchTextureMode = OverallTextureMode;
		}/*end if else on texturing mode*/


		/*****************************
		// Determine if we can do all the planes affected
		// by this material in one fell swoop...
		*****************************/
		if((BatchTextureMode != MIXED_TEXTURING) &&
		   (OverallShadingMode != MIXED_SHADING))
		{
			/*
			// Do the shading of the forward visible (if any)
			*/
			if(UsedMaterials[i].FVPlanesAffected > 0)
			{
	            SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
				RnBatchShadeAndTexture(pForwardsDone, 
								UsedMaterials[i].FVPlanesAffected,
								FALSE,
								pMaterialState,
								(MASTER_STATE_STRUCT *) pState,
								BatchTextureMode, 
								OverallShadingMode,
								pBBox == NULL);
	        	SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)

				pForwardsDone += UsedMaterials[i].FVPlanesAffected;
			}
			/*
			// Do the shading of the reverse planes (if necessary)
			*/
			if((!IgnoreReverseVis) && (UsedMaterials[i].RVPlanesAffected > 0))
			{
	            SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
				RnBatchShadeAndTexture(pReversesDone, 
								UsedMaterials[i].RVPlanesAffected,
								TRUE,
								pMaterialState,
								(MASTER_STATE_STRUCT *) pState,
								BatchTextureMode, 
								OverallShadingMode,
								pBBox == NULL);
	            SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)

				pReversesDone += UsedMaterials[i].RVPlanesAffected;
			}

		}
		/*****************************
		// Do each plane on an individual basis.
		*****************************/
		else
		{
			if(UsedMaterials[i].FVPlanesAffected > 0)
			{
	            SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
				RnIndivShadeAndTexture(pForwardsDone, 
								UsedMaterials[i].FVPlanesAffected,
								FALSE,
								pMaterialState,
								(MASTER_STATE_STRUCT *) pState,
								OverallTextureMode, 
								OverallShadingMode,
								pBBox == NULL);
	            SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)

				pForwardsDone += UsedMaterials[i].FVPlanesAffected;
			}

			/*
			// Do the shading of the reverse planes (if necessary)
			*/
			if((!IgnoreReverseVis) && UsedMaterials[i].RVPlanesAffected > 0)
			{
	            SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
				RnIndivShadeAndTexture(pReversesDone, 
								UsedMaterials[i].RVPlanesAffected,
								TRUE,
								pMaterialState,
								(MASTER_STATE_STRUCT *) pState,
								OverallTextureMode, 
								OverallShadingMode,
								pBBox == NULL);
			    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)

				pReversesDone += UsedMaterials[i].RVPlanesAffected;
			}

		}/*end if do all planes at once or individually*/
		
		/*
		// Check we haven crossed the bounds of decency
		*/
		ASSERT(pForwardsDone - PlaneCategories.FVPlanes <= NumFV);
		ASSERT(pReversesDone - PlaneCategories.RVPlanes <= NumRV);

	}/*end for materials*/

	/*
	// Pack the ISP parameters - first remember where we are currently,
	// in the parameter memory.
	*/
	PrimStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	
	SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
	NumPlanesAddedToHW = PackOpaqueParams( &PlaneCategories,
							 NeedsClipping,
							 !IgnoreReverseVis);
	SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)

	if(NumPlanesAddedToHW == 0)
	{
		DPF((DBG_MESSAGE, "Out of space after pack Opaq"));
		return FALSE;
	}
							 

	/*
	// Now determine which regions the convex belongs in...
	//
	// If we have a bounding box, use that to determine the regions
	*/
	if(pBBox != NULL)
	{
	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
		RnDetermineRegionsFromBBox(pBBox, &Regions);
		DPF((DBG_MESSAGE, "Object in regions X:%d to %d   Y:%d to %d", 
		  Regions.FirstXRegion, Regions.LastXRegion,
		  Regions.FirstYRegion, Regions.LastYRegion));
	    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
	}
	/*
	// Else use the "sampling method" for the infinite objects
	*/
	else
	{
		ASSERT(RnGlobalProjMatAllRegionsPositive());

		ASSERT(RnGlobalProjMatRegLastGtRegFirst());

	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
		if (RnDetermineRegionsFromPlanes(&PlaneCategories, &Regions))
		{
			DPF((DBG_MESSAGE,  "Region Box Offscreen = %d,\nInfObj in regions X:%d to %d   Y:%d to %d",
			  RnGlobalProjMatRegionBoxOnScreen(&Regions),	
			  Regions.FirstXRegion, Regions.LastXRegion,
			  Regions.FirstYRegion, Regions.LastYRegion));
		}
		else
		{
			/* OPTIMISATION ? WORK THIS OUT EARLIER SO WE HAVEN'T WASTED TIME
			   ON THE LOCAL MATERIALS ? */
			*pOffscreen = TRUE;
	        SGL_TIME_STOP(TRANSFORM_VISIBLE_CONVEX_TIME)
			return TRUE;
		}
	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
	}/*end if else */

	/*
	// Add this convex to all its regions. Note if it has translucent
	// properties, then treat it differently.
	*/

   	if (HasTranslucent)
   	{
		/* With optimization translucent variables are
		 * required for AddRegionSeeThru() routine.
		 */
		sgl_uint32						XYData;
		TRANS_REGION_DEPTHS_STRUCT	Trans;
		const TRANS_REGION_DEPTHS_STRUCT *pTrans;
		float invZMin;  /* nearest z value for objects bounding box */

		/*
		// get z depth used to sort translucent objects
		// NOTE use invZmin to get Zmin first.
		*/
		if (pBBox != NULL)
		{
			invZMin = pBBox->boxMin[2];

			if (invZMin < pProjMat->foregroundDistance)
			{
				invZMin = pProjMat->invForegroundDistance;
			}
			else
			{
				invZMin = 1.0f / invZMin;
			}

	        SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
			
			/* Optimisation uses AddRegionSeeThru() routine.
			 * Pack description for routine.
			 */
			XYData = ENCODE_OBJXYDATA(	PACKED_TYPE_TRANSLUCENT,
										Regions.FirstXRegion,
										Regions.FirstYRegion,
										Regions.LastXRegion,
										Regions.LastYRegion);

			/* Use the NearZ direct, allow for inversion
			 */
			Trans.BaseDepth = invZMin;
			Trans.DepthDx	= 0.0f;
			Trans.DepthDy	= 0.0f;
			pTrans = (const TRANS_REGION_DEPTHS_STRUCT *) &Trans;

			/* Use the add single translucent object function
			 */
			AddRegionSeeThru( XYData, nCurrTransSetID, NumPlanesAddedToHW, 
							  PrimStart, pTrans);

	        SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
		}
		/*
		// Else the object is infinite. Work out the distance for EACH of the 
		// the regions we are putting it in.
		*/
		else
		{
			TRANS_REGION_DEPTHS_STRUCT NearZs;

	        SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)
			DetermineRegionDistances(&Regions, &PlaneCategories, &NearZs);
			
			/* Optimisation uses AddRegionSeeThru() routine.
			 * Pack description for routine.
			 */
			XYData = ENCODE_OBJXYDATA(	PACKED_TYPE_TRANSLUCENT,
										Regions.FirstXRegion,
										Regions.FirstYRegion,
										Regions.LastXRegion,
										Regions.LastYRegion);

			/* Allow for inversion
			 */
			Trans.BaseDepth = NearZs.BaseDepth;
			Trans.DepthDx	= NearZs.DepthDx;
			Trans.DepthDy	= NearZs.DepthDy;
			pTrans = (const TRANS_REGION_DEPTHS_STRUCT *) &Trans;

			/* Use the add single shadow object function
			 */
			AddRegionSeeThru( XYData, nCurrTransSetID, NumPlanesAddedToHW, 
							  PrimStart, pTrans);

	        SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
		}
   	}
   	else
	{
		/* Optimisation uses AddRegionSolid() routine.
		 */
		sgl_uint32 XYData;

	    SGL_TIME_SUSPEND(TRANSFORM_VISIBLE_CONVEX_TIME)

		/* Pack description for routine.
		 */
		XYData = ENCODE_OBJXYDATA( 	PACKED_TYPE_OPAQUE,
		                           	Regions.FirstXRegion,
		                   			Regions.FirstYRegion,
	    	                       	Regions.LastXRegion,
	        	 					Regions.LastYRegion);

		/* Use the add single opaque object function
		 */
		AddRegionSolid( XYData, NumPlanesAddedToHW, PrimStart);
	    SGL_TIME_RESUME(TRANSFORM_VISIBLE_CONVEX_TIME)
	}
	/*
	// Succeeded
	*/
	SGL_TIME_STOP(TRANSFORM_VISIBLE_CONVEX_TIME)
	return TRUE;
	
						
}/*end function*/



/**************************************************************************
* Function Name  : RnProcessConvexShadows
* Inputs         : Convex Object pointer
*				 : State info
*				   transformed planes
*				   bounding box
*				   BOOl use A&B values already calculated
*
* Outputs        : None
* Input/Output	 : None
*
* Returns        : TRUE if shadows "processed" Ok (False if we ran out of
*				   parameter space etc). If we get FALSE then it is time to
*				   stop rendering because we are out of space. (Although
*				   I suppose we could still do collision detection.
*
* Global Used    : Projection "matrix" values, and the parameter stores
*
* Description    : This generates shadows from the supplied convex object
*				   and adds the shadow planes to the hardware parameter space.
*				   NOTE: It ignores invisibility settings on planes etc.
**************************************************************************/

/*
// Define the arrays of transformed planes, and lists of categories
*/

#define SHOW_SHAD_VOLS 0

sgl_bool RnProcessConvexShadows( const CONVEX_NODE_STRUCT  *pConvex,
								 const MASTER_STATE_STRUCT *pState,
								 const SHADOW_LIM_STRUCT *pShadowLimitPlanes,
								 const TRANS_PLANE_ARRAY_TYPE transformedPlanes, 
								 const BBOX_MINMAX_STRUCT *pBBox,
								 const sgl_bool objectOffscreen,
								 const sgl_bool ReUseABCs)
{
	int lightIndex;
	const LIGHT_ENTRY_STRUCT * pThisLight;
	int NumLights;

	sgl_bool IsParallel;
	const float * LightVect;

	static SHADOWS_TRANS_PLANE_ARRAY_TYPE ShadowPlanes;
	PROJECTION_MATRIX_STRUCT * const pProjMat  = RnGlobalGetProjMat();
	int	NumShadPlanes;
	int NumOriginalObjectPlanes;
	sgl_bool LightInside;
#if  SHOW_SHAD_VOLS
	int         tmpFx, tmpFy, tmpLx, tmpLy;
#endif

	/*
	// Where in Sabre param space does this start?, and how many planes
	// are ACTUALLY put in the hardware parameter memory.
	*/
	sgl_uint32 PrimStart;
	int NumPlanesAddedToHW;
	
	/*
	// Which regions does this object cover - for simplicty do
	// a rectangle of them.
	*/
	REGIONS_RECT_STRUCT Regions;

	/*
	// Step through the lights, deciding which ones cast shadows
	*/
	pThisLight= pState->pLightsState->light_entries;
	NumLights = pState->pLightsState->num_lights;

#if SHOW_SHAD_VOLS
	tmpFx = pProjMat->FirstXRegion;
	tmpFy = pProjMat->FirstYRegion;
	tmpLx = pProjMat->LastXRegion;
	tmpLy = pProjMat->LastYRegion;
#endif

	for(lightIndex = 0; lightIndex < NumLights;  lightIndex++, pThisLight ++)
	{
		/*
		// If this light casts a shadow
		*/
		if(pThisLight->shad_volume != 0)
		{
			/*////////
			//determine the light type, and get the light "vector"
			//////// */
			if((pThisLight->light_flags & mask_light_types) == parallel_light_type)
			{
				IsParallel = TRUE;
				LightVect = pThisLight->direction;
				DPF((DBG_MESSAGE, 
					"Generating shadow volume from parallel light:index %d",
					 lightIndex));
			}
			else
			{
				ASSERT((pThisLight->light_flags & mask_light_types) 
											   == point_light_type);

				IsParallel = FALSE;
				LightVect = pThisLight->position;

				DPF((DBG_MESSAGE, 
					"Generating shadow volume from Point light: index %d",
					 lightIndex));
			}/*determine light type*/


			/*////////
			// If there is a bounding box, see if we can reject the shadow
			// before we bother building it.
			// Note, if the object is onscreen, then so is its shadow volume.
			//////// */
			if(objectOffscreen && (pBBox != NULL))
			{
			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				SGL_TIME_RESUME(TRIVIAL_REJECTION_TIME)
				if(RnRejectBoxShadow(pBBox, LightVect, IsParallel))
				{
					/*
					// Skip this light and go on to the next
					*/
					continue;
				}
				SGL_TIME_SUSPEND(TRIVIAL_REJECTION_TIME)
			    SGL_TIME_RESUME(PROCESS_SHADOW_TIME)

			}/*can we easily reject the shadow*/


			/*////////
			// Generate shadow volume.
			//////// */
			RnGenerateShadowVolume( pConvex, 
									transformedPlanes,
									pState->pTransformState,
									pShadowLimitPlanes,
									IsParallel,
									LightVect,
									ShadowPlanes,
									&NumShadPlanes,
									&LightInside,
									&NumOriginalObjectPlanes);
			
			/*
			// There are two cases to handle. The simpler is when the light is
			// outside of the object...
			*/
			if(!LightInside)
			{
				int DummyInt=0;

				if(ReUseABCs && (NumOriginalObjectPlanes != 0))
				{
					DPF((DBG_MESSAGE, 
					"Future optimisation - Currently not re-using ABC in shads"));
				}

				/* ///////////// 
				// Classify the existing planes into forward, reverse etc
				// Initialise the Classification Counts to Zero - All of them !!
				//////////////// */

				PlaneCategories.NumFV = 0;
				PlaneCategories.NumFI = 0;
				PlaneCategories.NumRV = 0;
				PlaneCategories.NumRI = 0;
				PlaneCategories.NumPE = 0;

			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				if(RnProjectAndClassifyPlanes(NumShadPlanes,
										ShadowPlanes, NULL, NULL, NULL,
										TRUE,
										&PlaneCategories,
										&DummyInt,
										&DummyInt)  )
				{
					/*
					// The shadow is off screen... ignore it
					*/
					continue;
				}
			    SGL_TIME_RESUME(PROCESS_SHADOW_TIME)

#if SHOW_SHAD_VOLS
				{
					int i;

					for(i= 0; i < PlaneCategories.NumFV; i++)
					{
						float len;

						len = VecLength(PlaneCategories.FVPlanes[i]->normal);

						VecNormalise(PlaneCategories.FVPlanes[i]->normal);

						PlaneCategories.FVPlanes[i]->d /= len;

						if(pState->pTransformState->scale_flag==uniform_scale)
						{
							PlaneCategories.FVPlanes[i]->normal[0] /=
							  pState->pTransformState->rescale;
							PlaneCategories.FVPlanes[i]->normal[1] /=
							  pState->pTransformState->rescale;
							PlaneCategories.FVPlanes[i]->normal[2] /=
							  pState->pTransformState->rescale;
							PlaneCategories.FVPlanes[i]->d /=
							  pState->pTransformState->rescale;
						}

					}

				}

				/*
				// Do some fake shading of the primitive
				*/
			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				RnBatchShadeAndTexture(PlaneCategories.FVPlanes, 
								PlaneCategories.NumFV,
								FALSE,
								pState->pMaterialState,
								pState,
								NO_TEXTURING, 
								FLAT_SHADING,
								pBBox == NULL);
				
			    SGL_TIME_RESUME(PROCESS_SHADOW_TIME)

				/*
				// Put the shadow planes into the hardware AS VISIBLE OBJECTS
				*/
				PrimStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
				
			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				NumPlanesAddedToHW =
				  PackOpaqueParams(&PlaneCategories,TRUE,FALSE);
			    SGL_TIME_RESUME(PROCESS_SHADOW_TIME)

				if(NumPlanesAddedToHW == 0)
				{
					DPF((DBG_MESSAGE, "Out of space after pack VISIBLE Shadow"));
					return FALSE;
				}

				/*
				// Put it into the regions
				*/
				
				DPFOO((DBG_WARNING, 
					"Region class Not yet implemented for shadows: Assuming all"));
				Regions.FirstXRegion = tmpFx;
				Regions.LastXRegion  = tmpLx;
				Regions.FirstYRegion = tmpFy;
				Regions.LastYRegion  = tmpLy;

				/*
				// Add this convex to all its regions, It would be better to use
				// a routine that does all the block in one go...
				*/
			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				/* Optimisation uses AddRegionSolid() routine.
				 */
				{
					sgl_uint32 XYData;

					/* Pack description for routine.
					 */
					XYData = ENCODE_OBJXYDATA( 	PACKED_TYPE_OPAQUE,
		        		        	           	Regions.FirstXRegion,
		                			   			Regions.FirstYRegion,
	    	            	        		   	Regions.LastXRegion,
	        	 								Regions.LastYRegion);

					/* Use the add single opaque object function.
					 */
					AddRegionSolid( XYData, NumPlanesAddedToHW, PrimStart);
				}
			    SGL_TIME_RESUME(PROCESS_SHADOW_TIME)

#else
				/*
				// Put the shadow planes into the hadware
				*/
				PrimStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				NumPlanesAddedToHW = PackLightShadVolParams( &PlaneCategories,
															 TRUE );
			    SGL_TIME_RESUME(PROCESS_SHADOW_TIME)

				if(NumPlanesAddedToHW == 0)
				{
					DPF((DBG_MESSAGE, "Out of space after pack Shadow"));
					return FALSE;
				}

				/*
				// Put it into the regions
				//  use the "sampling method" for the infinite objects
				*/

			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
				if (RnDetermineRegionsFromPlanes(&PlaneCategories, &Regions))
				{
					sgl_uint32 XYData;
					
					/* Optimisation uses absolute Y co-ordinates so ASSERT
					 * against these.
					 */
					DPF((DBG_MESSAGE,"Region Box Offscreen = %d.",
						RnGlobalProjMatRegionBoxOnScreen(&Regions)));
					ASSERT(RnGlobalProjMatAllRegionsPositive(&Regions));

					ASSERT(Regions.LastXRegion >= Regions.FirstXRegion);
					ASSERT(Regions.LastYRegion >= Regions.FirstYRegion);

					DPF((DBG_MESSAGE,
					  "Shadow in regions X:%d to %d   Y:%d to %d",
					  Regions.FirstXRegion, Regions.LastXRegion,
					  Regions.FirstYRegion, Regions.LastYRegion));

					/*
					// Add this convex to all its regions
					*/
					/* Optimisation uses AddRegionAtmos() routine.
					 * Pack description for routine.
					 */
					XYData = ENCODE_OBJXYDATA(	PACKED_TYPE_SHADOW,
												Regions.FirstXRegion,
												Regions.FirstYRegion,
												Regions.LastXRegion,
												Regions.LastYRegion);

					/* Use the add single shadow object function
					 */
					AddRegionAtmos( XYData, NumPlanesAddedToHW, PrimStart);
				}
			    SGL_TIME_SUSPEND(PROCESS_SHADOW_TIME)
#endif
			}
			/*
			// Else the light is inside the object. Each plane has to be
			// "flipped" and treated as a separate object.
			*/
			else
			{
				DPF((DBG_MESSAGE, "Shadows when Light INSIDE object not yet done"));
			}

		} /*if light casts a shadow*/

	}/*end for light*/
	
	/*
	// If weve got this far, then the shadow volume was added ok
	*/
	return TRUE;
}


/**************************************************************************
* Function Name  : RnProcessLightOrShadVolume
* Inputs         : 
*					
* Outputs        : pOffscreen - returns TRUE if the object gets rejected.
*
* Input/Output	 : Transformed plane structure
*
* Returns        : TRUE if plane "processed" Ok (False if we ran out of
*				   parameter space etc). If we get FALSE then it is time to
*				   stop rendering because we are out of space. (Although
*				   I suppose we could still do collision detection.
*
* Global Used    : Projection "matrix" values, and the parameter stores
*
* Description    : This does the processing for a normal visible object, 
*				   projecting, shading and texturing it as necessary.
*
*					THIS ROUTINE IS NOT RE_ENTRANT as it has static variables
*					(Not that it would make	any sence to do that anyway...)
**************************************************************************/

sgl_bool RnProcessLightOrShadVolume(const CONVEX_NODE_STRUCT  *pConvex,
									const MASTER_STATE_STRUCT *pState,
									TRANS_PLANE_ARRAY_TYPE transformedPlanes, 
									const BBOX_MINMAX_STRUCT *pBBox)
{
	/*
	// Keep count of the number of planes in each category
	*/
	int numPlanes;
    int DummyInt;

	/*
	// Where in Sabre param space does this start?, and how many planes
	// are ACTUALLY put in the hardware parameter memory.
	*/

	sgl_uint32 PrimStart;
	int NumPlanesAddedToHW;

	sgl_bool ShadVol;
	
	/*
	// Which regions does this object cover - for simplicty do
	// a rectangle of them.
	*/
	REGIONS_RECT_STRUCT Regions;

	/*****************
	// How many planes are we dealing with then...
	*****************/
	numPlanes = pConvex->u16_num_planes;

	/*****************
	// What are we dealing with
	*****************/
	ShadVol =  ((pConvex->u16_flags & cf_mask_type) == cf_shadow_volume);
	
	ASSERT(ShadVol || ((pConvex->u16_flags & cf_mask_type) == cf_light_volume));
	/*****************
	// First Project and classify the planes
	// NOTE, no invisible planes to handle
	*****************/
	PlaneCategories.NumFV = 0;
	PlaneCategories.NumRV = 0;
	PlaneCategories.NumPE = 0;

	if(RnProjectAndClassifyPlanes( numPlanes, 
								   transformedPlanes,
								   NULL, NULL, NULL,
								   TRUE, /*IGNORE invisibility flags*/
								   &PlaneCategories,
								   &DummyInt,
								   &DummyInt))
	{
		DPF((DBG_MESSAGE, "Ignoring offscreen shad/light vol"));
		return TRUE;
	}

	DPF((DBG_MESSAGE, "Classified planes:FV:%d  RV:%d  PE:%d",
					  PlaneCategories.NumFV,  
					  PlaneCategories.NumRV, 
					  PlaneCategories.NumPE ));


	/*
	// Pack the ISP parameters - first remember where we are currently,
	// in the parameter memory.
	// 
	// This depends on the type of the object
	*/
	PrimStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	
	NumPlanesAddedToHW =  PackLightShadVolParams( &PlaneCategories, ShadVol);


	if(NumPlanesAddedToHW == 0)
	{
		DPF((DBG_MESSAGE, "Out of space after pack Shadow/LightVol"));
		return FALSE;
	}
							 
	/*
	// Now determine which regions the convex belongs in...
	//
	// If we have a bounding box, use that to determine the regions
	*/
	if(pBBox != NULL)
	{
		RnDetermineRegionsFromBBox(pBBox, &Regions);
		DPF((DBG_MESSAGE, "Object in regions X:%d to %d   Y:%d to %d", 
		  Regions.FirstXRegion, Regions.LastXRegion,
		  Regions.FirstYRegion, Regions.LastYRegion));

		/*
		// Add this convex to all its regions.This depends on its type
		*/
		if(ShadVol)
		{
			/* Optimisation uses AddRegionAtmos() routine.
			 */
			sgl_uint32 XYData;

			/* Pack description for routine
			 */
			XYData = ENCODE_OBJXYDATA( 	PACKED_TYPE_SHADOW,
	           			               	Regions.FirstXRegion,
                   						Regions.FirstYRegion,
   	                       				Regions.LastXRegion,
		        	 					Regions.LastYRegion);

			/* Use the add single shadow object routine
			 */
			AddRegionAtmos( XYData, NumPlanesAddedToHW, PrimStart);
		}
		else
		{
			sgl_uint32 XYData;

			/* Pack description for routine
			 */
			XYData = ENCODE_OBJXYDATA( 	PACKED_TYPE_LIGHTVOL,
		       			               	Regions.FirstXRegion,
		           						Regions.FirstYRegion,
	    	                			Regions.LastXRegion,
				        	 			Regions.LastYRegion);

			/* Use the add single light volume object routine
			 */
			AddRegionAtmos( XYData, NumPlanesAddedToHW, PrimStart);
		}
	}
	/*
	// Else use the "sampling method" for the infinite objects
	*/
	else
	{
		ASSERT(RnGlobalProjMatAllRegionsPositive());
		ASSERT(RnGlobalProjMatRegLastGtRegFirst());

		if (RnDetermineRegionsFromPlanes(&PlaneCategories, &Regions))
		{
			sgl_uint32 XYData;

			/* Optimisation uses absolute Y co-ordinates.
			 */
			ASSERT(RnGlobalProjMatRegionBoxOnScreen(&Regions));

			ASSERT(Regions.LastXRegion >= Regions.FirstXRegion);
			ASSERT(Regions.LastYRegion >= Regions.FirstYRegion);

			DPF((DBG_MESSAGE,   "Infinite object  X: %d-%d  Y: %d-%d\n",
			  Regions.FirstXRegion, Regions.LastXRegion,
			  Regions.FirstYRegion, Regions.LastYRegion));

			/*
			// Add this convex to all its regions.This depends on its type
			*/
			if(ShadVol)
			{
				/* Pack description for routine
				 */
				XYData = ENCODE_OBJXYDATA( 	PACKED_TYPE_SHADOW,
	            			               	Regions.FirstXRegion,
	                   						Regions.FirstYRegion,
	   	                       				Regions.LastXRegion,
			        	 					Regions.LastYRegion);

				/* Use the add single shadow object function
				 */
				AddRegionAtmos( XYData, NumPlanesAddedToHW, PrimStart);
			}
			else
			{
				/* Pack description for routine
				 */
				XYData = ENCODE_OBJXYDATA( 	PACKED_TYPE_LIGHTVOL,
		       				               	Regions.FirstXRegion,
		    	       						Regions.FirstYRegion,
	    		                			Regions.LastXRegion,
					        	 			Regions.LastYRegion);

				/* Use the add single light volume object function
				 */
				AddRegionAtmos( XYData, NumPlanesAddedToHW, PrimStart);
			}
		}
	}/*end if else */

	/*
	// Succeeded
	*/
	return TRUE;
	
						
}/*end function*/

/*
// End of file
*/
