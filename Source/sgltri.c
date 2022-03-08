/******************************************************************************
 * Name         : sgltri.c
 * Title        : SGL Lite
 * Author       : John Catchpole
 * Created      : 30/04/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : SGL Lite
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: sgltri.c,v $
 * Revision 1.114  1997/07/14  19:33:27  mjg
 * Added PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.113  1997/07/04  07:31:53  erf
 * Removed SglError(sgl_no_error) calls from DirectShadows()
 * since they are not needed as they are handled in sgltri_shadow().
 *
 * Revision 1.112  1997/07/03  17:57:15  erf
 * Restructured sgltri_shadow() to call DirectShadows() so that a
 * direct interface from pvrd.c can be made.
 *
 * Revision 1.111  1997/06/17  10:25:19  mjg
 *  Added FORCE_NO_FPU option.
 *
 * Revision 1.110  1997/06/11  17:25:33  mjg
 * Added a few lines for PCX1 in ProcessConvexPlane.
 *
 * Revision 1.109  1997/06/11  13:46:22  gdc
 * tried to improve shadow plane behaviour when shadows become
 * near perpendicular with very distant defining points
 *
 * Revision 1.108  1997/06/09  09:07:44  gdc
 * got rid of compiler warnings
 *
 * Revision 1.107  1997/05/16  17:19:27  erf
 * Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
 *
 * Revision 1.106  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.105  1997/04/21  20:02:05  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.104  1997/04/21  15:30:35  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.103  1997/04/02  21:30:00  mjg
 * Updated calling conventions.
 *
 * Revision 1.102  1997/03/31  23:41:32  mlj
 * Fixes to get DSO32 driver to build
 *
 * Revision 1.101  1997/03/31  22:10:32  jop
 * Sorted out WIN32/DOS32 issues
 * y
 *
 * Revision 1.100  1997/01/08  12:39:52  jrc
 * Added a workaround for bug in Ultimate Race that was ok in the old
 * drivers.  This should not affect performance.
 *
 * Revision 1.99  1996/12/11  16:39:44  erf
 * Changed #if defined(WIN32) to #if WIN32.
 *
 * Revision 1.98  1996/12/10  23:15:23  erf
 * Changed compile switches:
 * 1. PVR3 to PCX2.
 *
 * Revision 1.97  1996/12/09  18:02:03  gdc
 * removed un-needed dependancy on dtri.h
 *
 * Revision 1.96  1996/12/09  14:17:16  gdc
 * removed stuff now handled by default by D3DOPTIM stuff - all thats really left
 * here is the code to handle shadow volumes in sgltri API
 *
 * Revision 1.95  1996/12/05  21:26:35  jrc
 * Added code for freezing the rendered image and dumping parameters after a
 * certain number of frames.
 *
 * Revision 1.94  1996/12/02  19:06:55  erf
 * In sgltri_shadow() calls to AddRegionXXXX for D3DOPTIM are made to
 * AddRegionXXXXL(). This is part of the seperation between D3DOPTIM
 * and not D3DOPTIM.
 *
 * Revision 1.93  1996/12/02  16:48:21  jop
 * Added quad support in revised code
 *
 * Revision 1.92  1996/11/28  17:14:03  jrc
 * Added hardware specular highlights for faces enabled for shadows or
 * light volumes.
 *
 * Revision 1.91  1996/11/22  19:11:56  jop
 * Set fMinZ to 0.01
 *
 * Revision 1.90  1996/11/19  01:01:00  jop
 * Fixed a dodgy assert and added conditional code to use
 * function in dtri.c if desired
 *
 * Revision 1.89  1996/11/18  21:47:35  jrc
 * Fixed a bug in the comvex polygon testing (though the testing is commented
 * out for the moment anyway).
 *
 * Revision 1.88  1996/11/15  17:15:18  erf
 * Use PVR3 compile switch for PCX2 to:
 * 1. Remove scaling of A, B & C parameters for perpendicular edges.
 *
 * Revision 1.87  1996/11/15  00:51:59  jop
 * Added offset for X and Y to app hints
 *
 * Revision 1.86  1996/11/14  19:30:12  jrc
 * 1. Removed debug SCALE/OFFSET system for scaling and offsetting input
 *    coordinates.
 * 2. Added convex testing code (currently disabled) for polygons.
 * 3. Removed quads from fan processing code (4-sided n-polygons are just as
 *    fast.
 * 4. Only doing COMBINE method for textured faces.
 *
 * Revision 1.85  1996/11/12  22:49:25  jrc
 * Now inlining ProcessPolygonFacePlane because it seems to help after all.
 *
 * Revision 1.84  1996/11/12  21:53:32  jrc
 * Reverted back to inlining ProcessPolygonFacePlane because we now use it
 * in the D3DTest.  Speeds things up a few %.
 *
 * Revision 1.83  1996/11/12  11:23:01  jrc
 * Performance optimisations that combine polygons are now switched off when
 * specular highlights are used.
 *
 * Revision 1.82  1996/11/12  10:26:01  gdc
 * removed a conflict between MAX as a constant redefining MAX as a macro
 *
 * Revision 1.81  1996/11/11  21:36:20  jrc
 * Removed debug version of D3D face processing code.
 *
 * Revision 1.80  1996/11/11  20:06:45  jrc
 * Good version for fast Direct3D test results.
 *
 * Revision 1.79  1996/11/11  17:47:18  jrc
 * Most bugs fixed hopefully...
 *
 * Revision 1.78  1996/11/09  06:17:13  jrc
 * Optimised the fans, but intersection test seems to be broken.
 *
 * Revision 1.77  1996/11/09  04:26:35  jrc
 * At last managed to get 'COMBINE' hack to work for joining D3D quads to
 * form larger polygons.  Now it needs to be optimised...
 *
 * Revision 1.76  1996/11/07  15:15:59  erf
 * PVR3 compile switch used for PCX2 to implement IEEE floating point support.
 * Changes made:
 * 1. Changed n32A, B & C to f32A, B & C.
 * 2. Removed FLOAT_TO_FIXED multiplication factor.
 * 3. Removed calls to PackTo20Bit() to generate plane parameters.
 *
 * Revision 1.75  1996/11/06  19:43:55  jrc
 * Got polygons working now, and a 'COMBINE' hack for making games and
 * certain performance tests go quicker... (but COMBINE is turned off for
 * now until it can be finished).
 *
 * Revision 1.74  1996/11/06  12:36:32  jop
 * Added 0.5 to all xs and ys (sigh)
 *
 * Revision 1.73  1996/11/05  16:42:50  jrc
 * Done some more work on n-polygons (mosly commented out for now).
 *
 * Revision 1.72  1996/11/04  16:09:03  jrc
 * Added (mostly commented out) start of n-polygon code.
 *
 * Revision 1.71  1996/10/30  14:12:54  jop
 * removed NETRACE stuff
 *
 * Revision 1.70  1996/10/29  10:39:40  jop
 * Added flat vertex fogging
 *
 * Revision 1.69  1996/10/25  00:08:29  jop
 * Translucency/smooth shading now falls back to Average colour
 * flat shading
 *
 * Revision 1.68  1996/10/24  21:57:46  jop
 * Fixed average colour bug and highlight colour channel masks
 *
 * Revision 1.67  1996/10/24  16:47:32  jrc
 * Disabled shading-some-of-the-triangles hack.
 *
 * Revision 1.66  1996/10/24  16:08:04  jrc
 * An effort at working around smooth shading/translucency/shadows problem.
 *
 * Revision 1.65  1996/10/23  16:14:20  jop
 * Fix for bogus Z stuff (rockem)
 *
 * Revision 1.64  1996/10/16  21:17:29  jop
 * Tried to optimise translucent passes
 *
 * Revision 1.63  1996/10/15  21:20:14  jrc
 * Changed the order of red,green,blue for specular highlights in translucent
 * faces (same done for non-translucent faces in the previous revision).
 *
 * Revision 1.62  1996/10/15  21:08:02  jrc
 * Added average colour flat shading.
 *
 * Revision 1.61  1996/10/11  20:44:04  jrc
 * Backfacing quads now use the first three vertices to determine the texture
 * mapping.  This has been the case previously only for forward facing quads.
 *
 * Revision 1.60  1996/10/11  12:09:36  msh
 * "Small number" checks modified for Midas Arcade
 *
 * Revision 1.59  1996/10/09  18:10:56  jop
 * Trivia
 *
 * Revision 1.58  1996/10/03  14:22:02  jop
 * Removed mesh comping
 *
 * Revision 1.57  1996/10/02  14:51:11  jop
 * Fixed smooth shading bug
 *
 * Revision 1.56  1996/09/30  18:45:29  jop
 * Fixed int colour packing
 *
 * Revision 1.55  1996/09/30  14:20:46  jop
 * New triangle colour format
 *
 * Revision 1.54  1996/09/28  15:27:35  jop
 * Fixed bug in quads
 *
 * Revision 1.53  1996/09/27  16:21:12  jop
 * Fixed bogus Z
 *
 * Revision 1.52  1996/09/27  14:28:46  jop
 * Lots of hacking for d3d compat + optimisations
 *
 * Revision 1.51  1996/09/25  18:22:55  jop
 * Edge and face now processed in same function
 *
 * Revision 1.50  1996/09/18  19:07:34  jrc
 * Reverted to older shadow/lightvol perp plane handling.
 *
 * Revision 1.49  1996/09/13  11:37:48  jop
 * Fixed shadow bbox bug
 *
 * Revision 1.48  1996/09/09  17:15:00  mjg
 * Fixed union stuff
 *
 * Revision 1.47  1996/09/08  02:35:19  jop
 * added app hints
 *
 * Revision 1.46  1996/09/07  23:00:55  jop
 * Added light volume support
 *
 * Revision 1.45  1996/09/05  17:02:49  jop
 * Went back to adjoint method (sparklies fix)
 *
 * Revision 1.44  1996/09/04  00:02:15  jop
 * Added comments, got rid of adjoint matrix from precalc struct,
 * added shadow support to alpha blend triangles
 *
 * Revision 1.43  1996/09/02  17:00:50  mjg
 * Created new version of DoFlatShadingTri, to deal with alpha channel and
 * incorporating the Global Translucency value.
 *
 * Revision 1.42  1996/09/02  12:44:05  jop
 * Fixed mesh bits
 *
 * Revision 1.41  1996/08/30  19:53:13  jop
 * Temp fix for Z calculation
 *
 * Revision 1.40  1996/08/30  15:50:00  sjf
 *  Hacked in small object rejection for PVR1 - dummy
 * param added to addRegionOpaque calls.
 *
 * Revision 1.39  1996/08/30  13:20:14  sjf
 * Jop messed up my previous change. Put it back in again!
 *
 * Revision 1.38  1996/08/30  11:47:42  jop
 * Messed around with z accuracy and fixed some bad params
 *
 * Revision 1.37  1996/08/29  13:13:12  sjf
 * Added new param to AddRegionTranslucent.
 *
 * Revision 1.36  1996/08/29  11:16:52  jop
 * Added stuff for defensive translucent passes
 *
 * Revision 1.35  1996/08/14  14:40:51  pdl
 * changed a c++ comment.
 *
 * Revision 1.34  1996/08/13  15:20:15  jop
 * Fixed possible bug in texturing
 *
 * Revision 1.33  1996/08/02  18:02:22  jop
 * Added FPU fiddle around triangle/quad calls
 *
 * Revision 1.32  1996/07/31  21:48:53  jop
 * Added inverse to precalc structure - may make some savings
 *
 * Revision 1.31  1996/07/19  19:35:02  jop
 * Lots of fixes and optimisations
 *
 * Revision 1.30  1996/07/16  10:51:02  jop
 * Saved a division and reordered some code
 *
 * Revision 1.29  1996/07/15  17:10:18  jop
 * Added quads
 *
 * Revision 1.28  1996/07/11  16:41:42  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.27  1996/07/10  13:45:45  jop
 * Fixed flip_u/flip_v and added translucency
 *
 * Revision 1.26  1996/07/05  15:13:06  jop
 * Fixed UV and started some transparency stuff
 *
 * Revision 1.25  1996/06/27  11:46:38  jrc
 * Removed initialisations of most recent warnings.
 *
 * Revision 1.24  1996/06/26  17:15:53  jrc
 * sgltri_triangles sets the most recent warning status to zero.
 *
 * Revision 1.23  1996/06/20  20:42:34  jop
 * Added flipU and V.
 *
 * Revision 1.22  1996/06/06  23:07:44  jop
 * Fixed shadow bugs
 *
 * Revision 1.21  1996/06/06  17:38:09  jop
 * Fixed perpendicular handling
 *
 * Revision 1.20  1996/06/06  14:23:41  jop
 * added shadows to triangle api - doesn't work yet
 *
 * Revision 1.19  1996/06/05  15:54:19  jop
 * Optimised and enabled fogging and clipping
 *
 * Revision 1.18  1996/06/03  15:32:17  jop
 * Added region clipping
 *
 * Revision 1.17  1996/05/22  10:40:33  jop
 * 1. Added BASE and HIGH macros.
 * 2. Now averaging all three vertices for flat shading colour.
 * 3. Removed pointer to face parameter from calls to DoTextureMappingTri.
 *
 * Revision 1.16  1996/05/20  12:15:49  jrc
 * Fixed specular to use specified specular components instead of reusing the
 * flat shading colour.
 *
 * Revision 1.15  1996/05/16  15:52:12  jop
 * Fixed reverse plane crap
 *
 * Revision 1.14  1996/05/16  10:37:01  jop
 * Fixed MaxVal stuff
 *
 * Revision 1.13  1996/05/15  17:58:17  jrc
 * 1. Changed MAX_CENTRE and HALF_OFFSET in ProcessFacePlane from 2048 to
 *    1024.
 * 2. Moved scaling by 1/determinant above fMaxVal calculation.
 * 3. Started to remove fMaxVal altogether because this should have been a
 *    constant for all faces.
 *
 * Revision 1.12  1996/05/15  10:43:47  jop
 * Fixed bugs and moved to new triprecalc structure
 *
 * Revision 1.11  1996/05/10  20:53:04  jop
 * Some trivia
 *
 * Revision 1.10  1996/05/10  16:50:44  jrc
 * Changed pContext->wFlags to pContext->u32Flags
 *
 * Revision 1.9  1996/05/10  16:06:21  jrc
 * Changed wXXX and dwXXX variables to u16XXX and u32XXX names.
 *
 * Revision 1.8  1996/05/10  14:47:15  jop
 * Fixed a lot of things and unified structures
 *
 * Revision 1.7  1996/05/09  20:31:20  jrc
 * Changed loads of things for Direct3D efficiency and added a buffer concept
 * for shading and texturing bits that Jim'll do next...
 *
 * Revision 1.6  1996/05/08  17:44:17  jrc
 * Changed specular highlight components variable names.
 *
 * Revision 1.5  1996/05/08  17:37:04  jrc
 * Removed unused file scope static variables and did a few other tidyups.
 *
 * Revision 1.4  1996/05/07  14:16:32  jop
 * Fixed /= *= bug
 *
 * Revision 1.3  1996/05/03  17:59:49  jop
 * Some changes
 *
 * Revision 1.2  1996/05/03  15:58:14  jrc
 * Finished matrix code in face processing function.
 *
 * Revision 1.1  1996/05/03  15:26:12  jrc
 * Initial revision
 *
 *****************************************************************************/

#define MODULE_ID MODID_SGLTRI

#include <stdio.h>

#include "nm_intf.h"

#include "sgl_init.h"
#include "sgl_defs.h"
#include "sgl.h"
#include "sgl_math.h"

#include "dlntypes.h"
#include "dlmater.h"
#include "dlconvex.h"
#include "dlmesh.h"
#include "dllists.h"
#include "dlpoint.h"
#include "dlothers.h"
#include "dlglobal.h"

#include "rnglobal.h"
#include "pkisp.h"

#include "dregion.h"
#include "pvrosapi.h"
#include "parmbuff.h"

#if WIN32 || DOS32

	#define DO_FPU_PRECISION TRUE

	void __cdecl SetupFPU (void);
	void __cdecl RestoreFPU (void);

#else

	#define DO_FPU_PRECISION FALSE

#endif

/*
// ============================================================================
// 								STATIC VARIABLES:
// ============================================================================
*/

#define MAGIC  (1024.0f)
#define INVMAGIC  (1.0f/MAGIC)

#define FindZMinMax 0
#if FindZMinMax
extern float fMaxZ, fMinInvZ;
#endif

/* declared in rnrender beside GetApplicationHint func */
extern float 	fMinInvZ;
extern float 	fAddToXY;

/*
// ============================================================================
// 								STATIC FUNCTIONS:
// ============================================================================
*/

static INLINE void ProcessConvexPlanePerp (PSGLVERTEX	pV0,
										   PSGLVERTEX	pV1,
										   PSGLVERTEX	pV2,
										   TRANSFORMED_PLANE_STRUCT *pPlane,
										   sgl_bool bIsReversed)
{
	float fA, fB, fC;
#if !((PCX2 || PCX2_003) && !FORCE_NO_FPU)
	float fMaxVal;
#endif
	float delX1,delX2,delX3;
	float delY1,delY2,delY3;
	float del1,del2,del3;

	/* use the RMS deltas to figure which two vertices have greatest
	** screen separation - these will give the most accurate result
	*/ 
	delX1 =  pV1->fX - pV0->fX;
	delX2 =  pV2->fX - pV0->fX;
	delX3 =  pV2->fX - pV1->fX;

	delY1 =  pV1->fY - pV0->fY;
	delY2 =  pV2->fY - pV0->fY;
	delY3 =  pV2->fY - pV1->fY;

	del1=delX1*delX1 + delY1*delY1;
	del2=delX2*delX2 + delY2*delY2;
	del3=delX3*delX3 + delY3*delY3;

	if(del1>del2)
	{
		if(del1>del3)
		{
			/*1 and 0*/
			fA = -delY1;
			fB = delX1;
			fC = ((pV0->fX * pV1->fY) - (pV1->fX * pV0->fY));
		}
		else
		{
			/*2 and 1*/
			fA = -delY3;
			fB = delX3;
			fC = ((pV1->fX * pV2->fY) - (pV2->fX * pV1->fY));
		}
	}
	else 
	{
		if(del2>del3)
		{
			/*2 and 0*/
			fA = -delY2;
			fB = delX2;
			fC = ((pV0->fX * pV2->fY) - (pV2->fX * pV0->fY));
		}
		else
		{
			/*2 and 1*/
			fA = -delY3;
			fB = delX3;
			fC = ((pV1->fX * pV2->fY) - (pV2->fX * pV1->fY));
		}
	}

	if(bIsReversed==FALSE)
	{
		fA *= -1.0f;
		fB *= -1.0f;
		fC *= -1.0f;
	}

	/*
	// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
	// larger than I originally thought necessary)
	*/


#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		/* PCX2 supports IEEE floating point numbers.
		 */	
		pPlane->f32A = fA;
		pPlane->f32B = fB;
		pPlane->f32C = fC;
#else

#define MAX_CENTRE  (1024.0f)
#define HALF_OFFSET (1024.0f)

	fMaxVal = sfabs(MAX_CENTRE * (fA+fB) + fC) +
		HALF_OFFSET * (sfabs(fA) + sfabs(fB));

#undef MAX_CENTRE
#undef HALF_OFFSET
	

	fMaxVal = ApproxRecip (fMaxVal);

	pPlane->n32A = PackTo20Bit (fA * fMaxVal);
	pPlane->n32B = PackTo20Bit (fB * fMaxVal);
	pPlane->n32C = (sgl_int32) (fC * fMaxVal * FLOAT_TO_FIXED);
#endif
}

static INLINE TRANSFORMED_PLANE_STRUCT *ProcessConvexPlane (PLANE_CATEGORIES_STRUCT *PlaneCats, 
											TRANSFORMED_PLANE_STRUCT *pPlane,
											PSGLVERTEX	pV0,
										    PSGLVERTEX	pV1,
										    PSGLVERTEX	pV2,
										    sgl_bool bIsReversed)
{
	float  fX0,fX1,fX2, fY0,fY1,fY2, fZ0,fZ1,fZ2, fA,fB,fC, fMaxVal;
	float  fDet; /* matrix determinant */

	ASSERT(pV0 != pV1 && pV1 != pV2);

	fX0 = pV0->fX;
	fX1 = pV1->fX;
	fX2 = pV2->fX;

	fY0 = pV0->fY;
	fY1 = pV1->fY;
	fY2 = pV2->fY;

	/* calculate determinant and see if we can cull this plane */

	fDet = fX0*(fY1 - fY2) + fX1*(fY2 - fY0) + fX2*(fY0 - fY1);

	/*
	// Calculate 1/determinant:
	*/

  #if defined (MIDAS_ARCADE)

	/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
	if (sfabs (fDet) < 1.0e-3f)
	{
		/* perpendicular plane */
		
		ProcessConvexPlanePerp (pV0, pV1, pV2, pPlane, bIsReversed);
		PlaneCats->PEPlanes[PlaneCats->NumPE++] = pPlane;
	}
 
  #else

	if (sfabs (fDet) < 2.0f)
	{
		/* perpendicular plane */
		
		ProcessConvexPlanePerp (pV0, pV1, pV2, pPlane, bIsReversed);
		PlaneCats->PEPlanes[PlaneCats->NumPE++] = pPlane;
	}

  #endif

	else
	{
		float	fAdjoint[3][3];
		float	f1OverDet;

		f1OverDet = 1.0f / fDet;

		/*
		// Reduce Z range from 32 to 24 bits (taking a sign bit into account), and
		// invert so 0 is at infinity.
		*/
		fZ0 = pV0->fInvW * fMinInvZ;
		fZ1 = pV1->fInvW * fMinInvZ;
		fZ2 = pV2->fInvW * fMinInvZ;

		/*
		// fAdjoint is a constant zero offset
		*/
		fAdjoint[0][0] = fY1-fY2;
		fAdjoint[0][1] = fY2-fY0;
		fAdjoint[0][2] = fY0-fY1;

		fAdjoint[1][0] = fX2-fX1;
		fAdjoint[1][1] = fX0-fX2;
		fAdjoint[1][2] = fX1-fX0;

		fAdjoint[2][0] = fX1*fY2 - fX2*fY1;
		fAdjoint[2][1] = fX2*fY0 - fX0*fY2;
		fAdjoint[2][2] = fX0*fY1 - fX1*fY0;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		*/
		fA = fZ0*fAdjoint[0][0] + fZ1*fAdjoint[0][1] + fZ2*fAdjoint[0][2];
		fB = fZ0*fAdjoint[1][0] + fZ1*fAdjoint[1][1] + fZ2*fAdjoint[1][2];
		fC = fZ0*fAdjoint[2][0] + fZ1*fAdjoint[2][1] + fZ2*fAdjoint[2][2];

		/*
		// Scale A,B,C values:
		*/
		fA *= f1OverDet;
		fB *= f1OverDet;
		fC *= f1OverDet;

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		#define MAX_CENTRE  (1024.0f)
		#define HALF_OFFSET (1024.0f)

		fMaxVal = sfabs(MAX_CENTRE * (fA+fB) + fC) +
		  HALF_OFFSET * (sfabs(fA) + sfabs(fB));

		#undef MAX_CENTRE
		#undef HALF_OFFSET

		/*fMaxVal = (2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC);*/

		if (fMaxVal > 1.0f)
		{
			float fInvMaxVal = ApproxRecip(fMaxVal);

			/* out of hardware range - treat as perp */
			
		#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			pPlane->f32A = fA * fInvMaxVal;
			pPlane->f32B = fB * fInvMaxVal;
			pPlane->f32C = fC * fInvMaxVal;
		#else
			pPlane->n32A = PackTo20Bit (fA * fInvMaxVal);
			pPlane->n32B = PackTo20Bit (fB * fInvMaxVal);
			pPlane->n32C = (sgl_int32) (fC * FLOAT_TO_FIXED * fInvMaxVal);		
		#endif	

			if (bIsReversed)
			{
				PlaneCats->RVPlanes[PlaneCats->NumRV++] = pPlane;
			}
			else
			{
				PlaneCats->FVPlanes[PlaneCats->NumFV++] = pPlane;
				pPlane->u32TexasTag = 2;
			}
			/*			ProcessConvexPlanePerp (pV0, pV1, pV2, pPlane);
			**PlaneCats->PEPlanes[PlaneCats->NumPE++] = pPlane;
			*/
		}
		else
		{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			/* PCX2 supports IEEE floating point numbers.
			 */	
			pPlane->f32A = fA;
			pPlane->f32B = fB;
			pPlane->f32C = fC;
#else
			pPlane->n32A = PackTo20Bit (fA);
			pPlane->n32B = PackTo20Bit (fB);
			pPlane->n32C = (sgl_int32) (fC * FLOAT_TO_FIXED);
#endif

			if (bIsReversed)
			{
				PlaneCats->RVPlanes[PlaneCats->NumRV++] = pPlane;
			}
			else
			{
				PlaneCats->FVPlanes[PlaneCats->NumFV++] = pPlane;
				pPlane->u32TexasTag = 2;
			}
		}
	}

	return (pPlane + 1);
}


void DirectShadows(PSGLCONTEXT	pContext,
				   int			nNumFaces,
				   int			pFaces[][3],
			 	   PSGLVERTEX	pVertices,
				   float		fBoundingBox[2][2] )
{
	static PLANE_CATEGORIES_STRUCT	PlaneCats;
	static TRANSFORMED_PLANE_STRUCT	Planes[SGL_MAX_PLANES];

	sgl_uint32 					k;
	REGIONS_RECT_STRUCT 		RegionsRect;
	int							nNumPlanes;
	sgl_uint32					u32StartOfSection;
	TRANSFORMED_PLANE_STRUCT	*pXPlane;
	int							*pFace = (int *) pFaces;

	/* ignore this call if shadows are disabled
	 */
	if (pContext->eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
	{
		return;
	}

	pXPlane = Planes;

	PlaneCats.NumPE = 0;
	PlaneCats.NumFV = 0;
	PlaneCats.NumRV = 0;
	PlaneCats.NumFI = 0;
	PlaneCats.NumRI = 0;

	#if DO_FPU_PRECISION

		SetupFPU ();

	#endif

	if (fBoundingBox)
	{
		/* calculate plane data and max vertex (hack)
		 */
		for (k = 0; k < nNumFaces; ++k, pFace += 3)
		{
			PSGLVERTEX  pV0,pV1,pV2;
			sgl_vector	Vec1, Vec2;
			float		fNormZ;
			
			pV0 = pVertices + pFace[0];
			pV1 = pVertices + pFace[1];
			pV2 = pVertices + pFace[2];

			Vec1[0] = pV1->fX - pV0->fX;
			Vec1[1] = pV1->fY - pV0->fY;

			Vec2[0] = pV2->fX - pV0->fX;
			Vec2[1] = pV2->fY - pV0->fY;

			fNormZ = (Vec1[0] * Vec2[1]) - (Vec1[1] * Vec2[0]);

			pXPlane = ProcessConvexPlane (&PlaneCats, pXPlane, pV0, pV1, pV2, (fNormZ > 0.0f));
		}

		RegionsRect.FirstXRegion = (int)(fBoundingBox[0][0] * pContext->invRegX);
		RegionsRect.FirstYRegion = (int)(fBoundingBox[0][1] * pContext->invRegY);
		RegionsRect.LastXRegion =  (int)(fBoundingBox[1][0] * pContext->invRegX);
		RegionsRect.LastYRegion =  (int)(fBoundingBox[1][1] * pContext->invRegY);

		/*
		// Workaround for bug in Ultimate Race.
		*/
		if (RegionsRect.FirstXRegion > RegionsRect.LastXRegion)
		{
			RegionsRect.FirstXRegion = RegionsRect.LastXRegion;
		}

		if (RegionsRect.FirstYRegion > RegionsRect.LastYRegion)
		{
			RegionsRect.FirstYRegion = RegionsRect.LastYRegion;
		}
	}
	else
	{
		int nMaxVertexID = 0;

		/* calculate plane data and max vertex (hack)
		 */
		for (k = 0; k < nNumFaces; ++k, pFace += 3)
		{
			PSGLVERTEX  pV0,pV1,pV2;
			sgl_vector	Vec1, Vec2;
			float		fNormZ;
			
			pV0 = pVertices + pFace[0];
			pV1 = pVertices + pFace[1];
			pV2 = pVertices + pFace[2];

			if (pFace[0] > nMaxVertexID)
			{
				nMaxVertexID = pFace[0];
			}

			if (pFace[1] > nMaxVertexID)
			{
				nMaxVertexID = pFace[1];
			}

			if (pFace[2] > nMaxVertexID)
			{
				nMaxVertexID = pFace[2];
			}

			Vec1[0] = pV1->fX - pV0->fX;
			Vec1[1] = pV1->fY - pV0->fY;

			Vec2[0] = pV2->fX - pV0->fX;
			Vec2[1] = pV2->fY - pV0->fY;

			fNormZ = (Vec1[0] * Vec2[1]) - (Vec1[1] * Vec2[0]);

			pXPlane = ProcessConvexPlane (&PlaneCats, pXPlane, pV0, pV1, pV2, (fNormZ > 0.0f));
		}

		/* calculate bounding box
		 */
		RegionsRect.FirstXRegion = pContext->LastXRegion;
		RegionsRect.FirstYRegion = pContext->LastYRegion;
		RegionsRect.LastXRegion = pContext->FirstXRegion;
		RegionsRect.LastYRegion = pContext->FirstYRegion;

		for (k = 0; k <= nMaxVertexID; ++k, ++pVertices)
		{
			int 	nXRgn, nYRgn;

			nXRgn = (int)((pVertices->fX + fAddToXY) * pContext->invRegX);

			if (nXRgn < RegionsRect.FirstXRegion)
			{
				RegionsRect.FirstXRegion = nXRgn;
			}
			else if (nXRgn > RegionsRect.LastXRegion)
			{
				RegionsRect.LastXRegion = nXRgn;
			}

			nYRgn = (int)((pVertices->fY + fAddToXY) * pContext->invRegY);

			if (nYRgn < RegionsRect.FirstYRegion)
			{
				RegionsRect.FirstYRegion = nYRgn;
			}
			else if (nYRgn > RegionsRect.LastYRegion)
			{
				RegionsRect.LastYRegion = nYRgn;
			}
		}
	}

	if (pContext->bDoClipping)
	{
		if (RegionsRect.FirstXRegion > pContext->LastXRegion ||
		    RegionsRect.FirstYRegion > pContext->LastYRegion ||
			RegionsRect.LastXRegion < pContext->FirstXRegion ||
			RegionsRect.LastYRegion < pContext->FirstYRegion)
		{
		#if DO_FPU_PRECISION
			RestoreFPU();
		#endif
			return;
		}

		if (RegionsRect.FirstXRegion < pContext->FirstXRegion)
		{
			RegionsRect.FirstXRegion = pContext->FirstXRegion;
		}

		if (RegionsRect.FirstYRegion < pContext->FirstYRegion)
		{
			RegionsRect.FirstYRegion = pContext->FirstYRegion;
		}

		if (RegionsRect.LastXRegion > pContext->LastXRegion)
		{
			RegionsRect.LastXRegion = pContext->LastXRegion;
		}

		if (RegionsRect.LastYRegion > pContext->LastYRegion)
		{
			RegionsRect.LastYRegion = pContext->LastYRegion;
		}
	}

	/* add whole shadow volume */
	
	u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

	#if DEBUG_SHADOWS && FALSE

		RegionsRect.FirstXRegion = pContext->FirstXRegion;
		RegionsRect.FirstYRegion = pContext->FirstYRegion;
		RegionsRect.LastXRegion = pContext->LastXRegion;
		RegionsRect.LastYRegion = pContext->LastYRegion;

	#endif

	#if DEBUG_SHADOWS || FALSE

		nNumPlanes = PackOpaqueParams (&PlaneCats, FALSE, FALSE);
	
	#else
	
		nNumPlanes = PackLightShadVolParams (&PlaneCats, 
						pContext->eShadowLightVolMode == ENABLE_SHADOWS);

	#endif

	if (nNumPlanes)
	{
		if (pContext->eShadowLightVolMode == ENABLE_SHADOWS)
		{
			/* using this works just as well for visible convexes */
			AddRegionShadowL(&RegionsRect, u32StartOfSection, nNumPlanes);
		}
		else
		{
			/* using this works just as well for visible convexes */
			AddRegionLightVolL(&RegionsRect, u32StartOfSection, nNumPlanes);
		}
	}

	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif

} /* DirectShadows */


/*
// ============================================================================
// 							  SGL EXTERNAL ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name  : sgltri_shadow
 *
 * Inputs         : pContext, nNumFaces, pFaces, pVertices
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : -
 *
 * Description    : see header file
 *****************************************************************************/

#define DEBUG_SHADOWS	0

void CALL_CONV sgltri_shadow ( PSGLCONTEXT	pContext,
							   int			nNumFaces,
							   int			pFaces[][3],
						 	   PSGLVERTEX	pVertices,
							   float		fBoundingBox[2][2] )
{
	#if FIX_RENDER
		if (nFrameNum > FIX_FRAME_NUM)
		{
			return;
		}
	#endif

	/*
	// ----------------------
	// Check input parameters
	// ----------------------
	*/

	if (nNumFaces == 0)
	{
		SglError(sgl_no_err);
		return;
	}

	if (nNumFaces > SGL_MAX_PLANES)
	{
		SglError(sgl_err_bad_parameter);
		return;
	}

	if (pContext == NULL || pFaces == NULL || pVertices == NULL || nNumFaces < 0)
	{
		SglError(sgl_err_bad_parameter);
		return;
	}

    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}

	/* Call the DirectShadows() routine.
	 */
	DirectShadows(pContext, nNumFaces, pFaces, pVertices, fBoundingBox);
	
	/*
	// ------------------------------
	// Set ok error status and return
	// ------------------------------
	*/

	SglError(sgl_no_err);

} /* sgltri_shadow */

/*------------------------------- End of File -------------------------------*/
