/**************************************************************************
 * Name         : rnconvex.c
 * Title        : Rendering of Convex Primitives
 * Author       : Simon Fenney
 * Created      : 22 June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Code to process convex primitives as they are encountered in
 *				  the display list.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnconvex.c,v $
 * Revision 1.41  1997/09/11  10:23:53  erf
 * Read dithering quality setting.
 *
 * Revision 1.40  1997/06/04  11:16:14  gdc
 * removed some compiler warnings
 *
 * Revision 1.39  1997/04/23  09:31:47  mjg
 * Removed compiler warnings.
 *
 * Revision 1.38  1997/04/09  17:05:16  gdc
 * removed reference to projection_mat global
 *
 * Revision 1.37  1997/04/07  14:52:33  gdc
 * added shadow limit planes pointer to RnProcessConvexNode parameters
 *
 * Revision 1.36  1997/04/07  13:31:03  gdc
 * now get trans set ID as a function parameter not a global
 *
 * Revision 1.35  1997/03/25  10:23:32  erf
 * When processing convexes read texture filtering quality setting to
 * set the bilinear register on PCX2.
 *
 * Revision 1.34  1997/02/03  10:32:28  gdc
 * removed D3DOPTIM_SGL compile conditional from this module as this
 * is now default and only option
 *
 * Revision 1.33  1997/01/31  11:30:10  mjg
 * Altered call for Light Volume initialisation, to use the SGL specific
 * code in dregion.c
 *
 * Revision 1.32  1996/12/10  17:28:11  gdc
 * added D3DOPTIM_SGL setting for AllowLightVolumeAdditionL
 *
 * Revision 1.31  1996/10/29  10:26:23  mal
 * Modified the timing code.
 *
 * Revision 1.30  1996/10/22  15:00:05  mal
 * Modified TransformBasicPlanes so that the arbitrary scale
 * loops(x2) have now been combined into one. Also changed the
 * switch to an if-then-else.
 *
 * Revision 1.29  1996/10/16  14:34:15  gdc
 * made RnTransformBasicPlanes non static so it can
 * be called from rntrav.c
 *
 * Revision 1.28  1996/10/10  11:11:40  mal
 * Modified some of the metric macros.
 *
 * Revision 1.27  1996/10/04  17:12:34  mal
 * modified some of the metric macros.
 *
 * Revision 1.26  1996/10/03  12:35:42  mal
 * Added some metric macros to do code profiling.
 *
 * Revision 1.25  1996/07/05  07:56:36  sjf
 * Fixed possible problem with texture pre-process and null local
 * material pointers.
 *
 * Revision 1.24  1996/06/06  23:03:56  sjf
 * Added material instancing.
 *
 * Revision 1.23  1996/06/04  16:12:00  sjf
 * In texture caching, forgot to check the quality flags. Fixed now.
 *
 * Revision 1.22  1996/05/30  20:32:16  sjf
 *  Added texture caching.
 *
 * Revision 1.21  1995/12/05  15:14:12  jrc
 * Now passing parameters in calls to RnTestPointsWithLocalPlanes in correct
 * order.
 *
 * Revision 1.20  1995/11/02  15:28:57  sjf
 * ARGGHHH, changed the ASSERT to a DPF so that the program keeps running.
 *
 * Revision 1.19  1995/11/02  14:51:31  sjf
 * OOPS, missed acouple of other places where the accuracy could be a
 * bit more lenient.
 *
 * Revision 1.18  1995/11/02  14:40:21  sjf
 * Reduced the accuracy of the checking of the object transformation.
 *
 * Revision 1.17  1995/10/17  12:18:42  jrc
 * Now passing regions rectangle's edges as a single structure to
 * AllowLightVolAddition.
 *
 * Revision 1.16  1995/10/07  15:08:46  sjf
 * Changed Test Box with camera.
 *
 * Revision 1.15  1995/09/19  09:06:01  sjf
 * Added light volume support (and hidden object support - but that is untested).
 * NOTE: Added a routine here which should really go in rnlights, but the formatting has
 * gone haywire, and I'd really like to revert that version when jim P gets in.
 *
 * Revision 1.14  1995/09/15  18:10:33  jrc
 * Added prototypes for RnTestPointsWithTransformed|LocalPlanes.
 *
 * Revision 1.13  1995/09/15  16:44:27  jop
 * Made vector length ASSERT a bit easier
 *
 * Revision 1.12  1995/08/27  15:23:51  sjf
 * 1) Cleaned up code
 * 2) Added shadow code.
 *
 * Revision 1.11  1995/08/07  12:40:45  sm
 * *** empty log message ***
 * (NO CHNAGES WERE ACTUALLY MADE - Simon Fenney)
 *
 * Revision 1.10  1995/08/04  14:34:41  sm
 * fixed mistake in calc of d value
 *
 * Revision 1.10  1995/08/04  14:34:41  sm
 * fixed mistake in calc of d value
 *
 * Revision 1.9  1995/07/31  13:48:09  sjf
 * cosmetic changes to comments.
 *
 * Revision 1.8  1995/07/30  17:48:00  sjf
 * Reduced debug levels.
 *
 * Revision 1.7  1995/07/30  12:59:06  sjf
 * Now have to pass a bounding box to the opaque primitive handling code.
 *
 * Revision 1.6  1995/07/30  12:14:30  sjf
 * Changed the Bounding box types.
 *
 * Revision 1.5  1995/07/29  13:35:28  sjf
 * MOved copy of original data into transform and classify planes.
 *
 * Revision 1.4  1995/07/27  10:58:55  sjf
 * OOPS forgot to copy the plane flags in the other situation.
 *
 * Revision 1.3  1995/07/27  10:56:04  sjf
 * Fixed bug in setting flags in transformed planes, and added more
 * debug.
 *
 * Revision 1.2  1995/07/26  16:25:23  sjf
 * Just more code basically (except I lumped the uniform scale and no scale
 * transformation routines together).
 *
 * Revision 1.1  1995/07/06  16:21:05  sjf
 * Initial revision
 *
 *
 *
 *
 **************************************************************************/

/*
// define Debug Module IDs
*/
#define MODULE_ID	MODID_RN


#include "sgl_defs.h"
#include "sgl_math.h"

#include "nm_intf.h"
#include "dlnodes.h" 

#include "rnglobal.h"
#include "rnstate.h"

#include "rnconvst.h"
#include "rnconvex.h"

#include "rnreject.h"
#include "rnconvpr.h"

#include "rnpoint.h"
#include "pmsabre.h"
#include "dregion.h"

#include "rnmater.h"
#include "txmops.h"

#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/*
// Define the arrays of transformed planes, and lists of categories
*/
static TRANS_PLANE_ARRAY_TYPE transformedPlanes;


/*************************************************************************
// I have made this letter longer than usual because I lack the time to
// make it shorter.
//                -- Blaise Pascal
*************************************************************************/


/**************************************************************************
 * Function Name  : RnTransformBasicPlanes (can be called fron rntrav.c)
 * Inputs         : pPlanes    - pointer to the convex plane data
 *					numPlanes  - the number of planes
 *					pTransform - pointer to the transformation
 *					
 * Outputs        : TransformedPlanes - the planes after transformation
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : This transforms the BASIC plane data in the convex
 *					from local to "world" coordinates. It does not process the
 *					texturing or shading data as this only has to be done for
 *					visible planes and that has yet to be determined.
 *
 *					It stores the flag, and calculates the rep point, normal
 *					and "d" value for the plane. 
 *					It also stores the pointer to original data, since we
 *					already have that.
 *
 *					NOTE: The normal does NOT remain a unit vector. This is
 *					because we only need unit vectors for shading, and 
 *					that will be done in object space (by inverse transforms
 *					of the lights)
 *					
 *					NOTE: There are effectively two versions of this code. One
 *					which assumes that we have a decent number of floating 
 *					point registers (e.g. MIPS, Sparc, PowerPC) and the
 *					other for older type architectures. eg ix86.
 *
 *					Basically it makes creative use of macros VS variables,
 *					with the MACRO MULTI_FP_REG determining which version to
 *					use.
 **************************************************************************/
 void RnTransformBasicPlanes( const CONV_PLANE_STRUCT * pPlanes,
									const int numPlanes,
									const TRANSFORM_STRUCT * pTransform,
									TRANS_PLANE_ARRAY_TYPE TransformedPlanes)
{

	int i;

	const CONV_PLANE_STRUCT * pInputPlane;
	TRANSFORMED_PLANE_STRUCT *pOutputPlane;

	/*
	// Code the routine depending on the number of available FP registers
	*/
	#if MULTI_FP_REG
		/*
		// A copy of an entire matrix sucked into registers (where available).
		// The matrix stays constant for a lot of operations so it makes sense
		// to save it locally.
		*/
		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;

		float inv00, inv01, inv02, inv03;
		float inv10, inv11, inv12, inv13;
		float inv20, inv21, inv22, inv23;
	#else
		/*
		// For the case where we DONT have many FP registers,
		// define aliases for the matrix entries. (NOTE this
		// gets redefined in one of the cases
		*/
		#define m00 pTransform->mat[0][0]
		#define m01 pTransform->mat[0][1]
		#define m02 pTransform->mat[0][2]
		#define m03 pTransform->mat[0][3]

		#define m10 pTransform->mat[1][0]
		#define m11 pTransform->mat[1][1]
		#define m12 pTransform->mat[1][2]
		#define m13 pTransform->mat[1][3]

		#define m20 pTransform->mat[2][0]
		#define m21 pTransform->mat[2][1]
		#define m22 pTransform->mat[2][2]
		#define m23 pTransform->mat[2][3]

		/*
		// for arbitrary scale we have to use 
		// the inverse matrix.
		*/
		#define inv00  pTransform->inv[0][0]
		#define inv01  pTransform->inv[1][0]
		#define inv02  pTransform->inv[2][0]

		#define inv10  pTransform->inv[0][1]
		#define inv11  pTransform->inv[1][1]
		#define inv12  pTransform->inv[2][1]

		#define inv20  pTransform->inv[0][2]
		#define inv21  pTransform->inv[1][2]
		#define inv22  pTransform->inv[2][2]

	#endif

	/*
	// Local copies of the components of the normal and the rep point.
	*/
	float normX, normY, normZ;
	float repX,	 repY,	repZ;

	/*
	// Variables used to compute the plane "d" value. 
	// lastNorm and lastRep save the temporary results to 
	// speed calculation of the d value
	*/
	float tmpD, lastNorm, lastRep;

	/*
	// Set up pointers to the first input plane, and where the transformed
	// results will go...
	*/
	pInputPlane = pPlanes;
	pOutputPlane = TransformedPlanes;


	#if MULTI_FP_REG
		/*
		// Suck in the matrix bit... this is needed in all 3 cases below
		*/
		m00 = pTransform->mat[0][0];
		m01 = pTransform->mat[0][1];
		m02 = pTransform->mat[0][2];
		m03 = pTransform->mat[0][3];

		m10 = pTransform->mat[1][0];
		m11 = pTransform->mat[1][1];
		m12 = pTransform->mat[1][2];
		m13 = pTransform->mat[1][3];

	    m20 = pTransform->mat[2][0];
	    m21 = pTransform->mat[2][1];
	    m22 = pTransform->mat[2][2];
	    m23 = pTransform->mat[2][3];

	    /*
		// Suck in the TRANSPOSE of the Inverse matrix. 
		// (Note that we don't need the translation bits for direction
		// vectors)
		*/
		inv00 = pTransform->inv[0][0];
		inv01 = pTransform->inv[1][0];
		inv02 = pTransform->inv[2][0];
	
		inv10 = pTransform->inv[0][1];
		inv11 = pTransform->inv[1][1];
		inv12 = pTransform->inv[2][1];

		inv20 = pTransform->inv[0][2];
		inv21 = pTransform->inv[1][2];
		inv22 = pTransform->inv[2][2];

	#endif

	/*
	// Determine the type of the matrix, so we can choose the
	// optimal method of transforming the plane data.
	*/

	/* /////////////////////////////////
	// No scaling or Uniform Scaling..
	// then we can transform the normal the same as the rep point.
	// Note that we arent renormalising the normal
	///////////////////////////////// */	
	if ( (pTransform->scale_flag == no_scale) || (pTransform->scale_flag == uniform_scale) )
	{
		
		/*
		// Loop through the planes
		*/
		for(i = 0; i < numPlanes; i++)
		{
			/*
			// Copy the flag, and original data pointer
			// Note the cast below JUST removes const..  we really need to
			// propogate const ...
			*/
			pOutputPlane->flags= pInputPlane->flags;
			DPFOO((DBG_VERBOSE, 
					"Ideally like to put consts everywhere below.."));
			pOutputPlane->pOriginalData = (CONV_PLANE_STRUCT *)pInputPlane;

			/*
			// get the normal in local storage
			*/
			normX = pInputPlane->normal[0];
			normY = pInputPlane->normal[1];
			normZ = pInputPlane->normal[2];

			/*
			// get the rep point in local storage
			*/
			repX = pInputPlane->rep_point[0];
			repY = pInputPlane->rep_point[1];
			repZ = pInputPlane->rep_point[2];

			/*
			// compute X components of Normal and Rep point. 
			//
			// Note: save results in temp  variables, so we can compute
			// "d" efficiently. THIS GOES AGAINST VL PROGRAMMING STYLE BUT
			// TIME IS CRITICAL HERE.
			*/
			pOutputPlane->normal[0] = lastNorm =  m00 * normX +  m01 * normY  +  m02 * normZ;
			pOutputPlane->repPnt[0] = lastRep =  m00 * repX +  m01 * repY  +  m02 * repZ + m03;
			tmpD = lastNorm * lastRep;


			/*
			// compute the Y components
			*/
			pOutputPlane->normal[1] = lastNorm =  m10 * normX +  m11 * normY  +  m12 * normZ;
			pOutputPlane->repPnt[1] = lastRep =  m10 * repX +  m11 * repY  +  m12 * repZ + m13;
			tmpD += lastNorm * lastRep;

			/*
			// compute the Z components
			*/
			pOutputPlane->normal[2] = lastNorm =  m20 * normX +  m21 * normY  +  m22 * normZ;
			pOutputPlane->repPnt[2] = lastRep =  m20 * repX +  m21 * repY  +  m22 * repZ + m23;

			/*
			// compute and save the plane d value.
			*/
			pOutputPlane->d = tmpD + lastNorm * lastRep;

			pInputPlane++;
			pOutputPlane++; 

		}/*end for*/
	} /*end if no scale*/
	else if (pTransform->scale_flag == arbitrary_scale) 
	{
		
		/* /////////////////////////////////
		// Arbitrary scaling.
		// This is the general case. To maintain a normal,
		// we must use the transpose of the inverse matrix.
		// (see the appendix of Foley and Van Dam and the old RGL code)
		//
		// We therefore do the transformation in two passes - the first of the
		// rep point by the usual matrix, and the second of the
		// normal by the transpose etc. Calc D value in the 2nd pass.
		///////////////////////////////// */
			/*
			// Loop through the planes 
			*/
		    for(i = 0; i < numPlanes; i++) 
			{
				/*
				// Copy the flag, and original data pointer
				// Note the cast below JUST removes const..  we really need to
				// propogate const ...
				*/
				pOutputPlane->flags= pInputPlane->flags;

				DPFOO((DBG_VERBOSE, 
						"Ideally like to put consts everywhere below.."));
				pOutputPlane->pOriginalData = (CONV_PLANE_STRUCT *)pInputPlane;

				/*
				// get the rep point in local storage
				*/
				repX = pInputPlane->rep_point[0];
				repY = pInputPlane->rep_point[1];
				repZ = pInputPlane->rep_point[2];

				/*
				// compute X component
				*/
				pOutputPlane->repPnt[0] =  m00 * repX +  m01 * repY  +  m02 * repZ + m03;

				/*
				// compute the Y component
				*/
				pOutputPlane->repPnt[1] = m10 * repX +  m11 * repY  +  m12 * repZ + m13;

				/*
				// compute the Z component
				*/
				pOutputPlane->repPnt[2] =  m20 * repX +  m21 * repY  +  m22 * repZ + m23;


				/*
				// get the normal in local storage.
				*/
				normX = pInputPlane->normal[0];
				normY = pInputPlane->normal[1];
				normZ = pInputPlane->normal[2];

				/*
				// compute X component of Normal
				// NOTE save results in temp  variables, so we can compute
				// "d" efficiently. THIS GOES AGAINST VL PROGRAMMING STYLE BUT
				// TIME IS CRITICAL HERE.
				//
				*/
				pOutputPlane->normal[0] = lastNorm =  inv00 * normX +  inv01 * normY  +  inv02 * normZ;
				tmpD = lastNorm * pOutputPlane->repPnt[0];

				/*
				// compute the Y component
				*/
				pOutputPlane->normal[1] = lastNorm =  inv10 * normX +  inv11 * normY  +  inv12 * normZ;
				tmpD += lastNorm * pOutputPlane->repPnt[1];

				/*
				// compute the Z component
				*/
				pOutputPlane->normal[2] = lastNorm =  inv20 * normX +  inv21 * normY  +  inv22 * normZ;

				/*
				// compute and save the plane d value.
				*/
				pOutputPlane->d = tmpD + lastNorm * pOutputPlane->repPnt[2];

				pInputPlane++;
				pOutputPlane++; 

			}/*end for*/

	}/*end if Arbitrary scale */
	else
	{
		/* /////////////////////////////////
		// UNKNOWN CASE ??? The system is messing up....
		///////////////////////////////// */
		ASSERT(FALSE);
	}/*end if*/

	#if !MULTI_FP_REG
		/*
		//	undefine all the macros
		*/
		#undef m00
		#undef m01
		#undef m02
		#undef m03

		#undef m10
		#undef m11
		#undef m12
		#undef m13

		#undef m20
		#undef m21
		#undef m22
		#undef m23

	    #undef inv00
		#undef inv01
		#undef inv02
		#undef inv03

		#undef inv10
		#undef inv11
		#undef inv12
		#undef inv13

		#undef inv20
		#undef inv21
		#undef inv22
		#undef inv23

	#endif



	/* //////////////////////////////////////
	// DEBUG Error testing......
	////////////////////////////////////// */
	#if DEBUG
		/*
		// Check that the plane normals were unit vectors
		// This really has nothing to do with the routine, but
		// it can't hurt!!!
		*/
		for(i = 0; i < numPlanes; i++)
		{
			ASSERT(fabs(SqrVecLength((pPlanes+i)->normal) - 1.0f) < 0.01);
		}
		/*
		// check the computed "d" values, and transformed vectors
		*/
		for(i = 0; i < numPlanes; i++)
		{
			sgl_vector temp;

			/*
			// check rep point.
			*/
			TransformVector(pTransform, (pPlanes+i)->rep_point , temp);
			VecSub(temp, TransformedPlanes[i].repPnt, temp);
			ASSERT(fabs(SqrVecLength(temp)) < 0.01f);

			/*
			// check d matches an alternate computation method
			*/
			if(fabs(DotProd(TransformedPlanes[i].normal,
							 TransformedPlanes[i].repPnt)
				 - TransformedPlanes[i].d) > 0.01)
			{
				DPF((DBG_MESSAGE, "Inaccuracy in Transform Convex???"));
			}
		}
	#endif
	/* ////////////////////////////////////// */
	
}




/*
// ###################################################
//
// I WANT TO PUT THIS FUNCTION IN RNLIGHTS BUT THE FILE IS SCREWED AT 
// PRESENT AND I WANT TO GO BACK TO AN EARLIER VERSION
//
//
*/
static int RnGetLightsSlot(const int LightName, const MASTER_STATE_STRUCT *pState)
{
	LIGHT_ENTRY_STRUCT * pCurrEntry;

	int i;

	pCurrEntry = pState->pLightsState->light_entries;

	/*
	// Look through the light state for this light, and
	// get its light slot
	*/
	for(i = 0; i < pState->pLightsState->num_lights; i++)
	{
		/*
		// Is this the light we want?
		*/
		if(pCurrEntry->light_name == LightName)
		{
			/*
			// then return the slot
			*/
			return pCurrEntry->assigned_shad_volume;
		}

		/*
		// try the next one
		*/
		pCurrEntry ++;

	}/*end for */

	/*
	// Not found then return 0
	*/
	return 0;
}






/**************************************************************************
 * Function Name  : RnProcessConvexNode
 * Inputs         : pConvex- pointer to the convex primitive node
 *					
 * Outputs        : parentUpdatePoints- set TRUE if a collision takes place.
 *					This value is passed to the parent list so that it can
 *					remove the hit collision point from its list of active
 *					points.
 * Input/Output	  : pState- pointer to a master state stack "frame"
 * Returns        : TRUE if the convex was processed OK, else we are
 *				    out of parameter space, so may as well give up on the
 *					rest of the render
 * Global Used    : See description
 *
 * Description    : This does (or at least organises) all the processing
 *					of a convex node  - from rejecting/accepting, transformation
 *					projection, shading, and giving to the hardware management
 *					side of things.
 *
 *
 *				Note: There is possibly some redundant processing i.e.
 *					  transforming bounding boxes etc with...
 *					  1) invisible objects when collision detection and shadows
 *						 are off could be rejected very early
 *
 *					  2) light volume def when the matching light is not
 *						 in scope.
 *
 *					These are probably such rare cases that they are not worth
 *					testing for.
 *
 **************************************************************************/

sgl_bool RnProcessConvexNode(const CONVEX_NODE_STRUCT  *pConvex,
							 const MASTER_STATE_STRUCT *pState,
							 const SHADOW_LIM_STRUCT *pShadowLimitPlanes,
							 sgl_bool *parentUpdatePoints,
							 int nCurrTransSetID)
{
	/*
	// Transformed Bounding box (if any) in world Coordinates
	*/
	BBOX_MINMAX_STRUCT bboxInWC;

	/*
	// Boolean if primitive is DEFINITELY off screen.
	// (Note if this is FALSE then we can only say if it
	// is PROBABLY on screen.)
	*/
	sgl_bool offScreen;

	/*
	// Type of convex we are dealing with
	*/
	CONVEX_FLAGS_ENUM ConvexType;

	/*
	// Flags indicating what work will need to be done to each
	// primitive.
	*/
	sgl_bool generateShadowVolumes;
	sgl_bool NeedsClipping;

	/*
	// Remember if the primitive has been transformed
	*/
	sgl_bool beenTransformed;


	/*
	// A pointer to the bounding box OR NULL if there isn't one.
	*/
	const BBOX_MINMAX_STRUCT *pBBox;

	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	ASSERT(parentUpdatePoints != NULL);
	/*
	// Paranoia
	*/
	ASSERT(pConvex->u16_num_planes <= pConvex->u16_max_planes)
	ASSERT(pConvex->u16_num_planes <= SGL_MAX_PLANES)

	SGL_TIME_START(TRIVIAL_REJECTION_TIME)

	/*
	// Exit Early if there aren't any planes!!!
	*/
	if(pConvex->u16_num_planes == 0)
	{
		DPF((DBG_MESSAGE, "Skipping Empty Convex"));
		return TRUE; /*still ok though*/
	}

	/* Set bilinear filtering mode.
	 * This is the best place to do this. The relevent bilinear
	 * filtering setting will be set for all subsequent mesh objects.
	 */
  	if (pState->pQualityState->flags & qf_texture_filtering)
	{
		pProjMat->eFilterType = pState->pQualityState->eFilterType;
	}

	/* Set dithering mode.
	 */
  	if (pState->pQualityState->flags & qf_dithering)
	{
		pProjMat->bDithering = TRUE;
	}
	else
	{
		pProjMat->bDithering = FALSE;
	}

	/*
	// See if the primitive has a bounding box,
	// and transform the box.
	*/
	if(pConvex->u16_flags & cf_has_bbox)
	{
		
		DPF((DBG_VERBOSE, 
		 "Convex with BBOX: \n\tInitial Pos:[%f, %f, %f] +-[%f, %f, %f]",
						 pConvex->bbox.boxCentre[0],
						 pConvex->bbox.boxCentre[1],
						 pConvex->bbox.boxCentre[2],
						 pConvex->bbox.boxOffsets[0],
						 pConvex->bbox.boxOffsets[1],
						 pConvex->bbox.boxOffsets[2]));


		TransformBBox(pState->pTransformState, 
					 & (pConvex->bbox),	 &bboxInWC);

		DPF((DBG_VERBOSE, "After Transform: Min:[%f, %f, %f] Max [%f, %f, %f]",
						 bboxInWC.boxMin[0],
						 bboxInWC.boxMin[1],
						 bboxInWC.boxMin[2],
						 bboxInWC.boxMax[0],
						 bboxInWC.boxMax[1],
						 bboxInWC.boxMax[2]));
		/*
		// test to see if it is DEFINITELY off screen. (It could
		// still not be visible even when it is FALSE).
		*/
		offScreen =  (RnTestBoxWithCamera(&bboxInWC, FALSE, &NeedsClipping)
					   == TB_BOX_OFFSCREEN);
		
		DPF((DBG_VERBOSE, "BBox Offscreen?:%d", offScreen));

		/*
		// Record that the primitive itself hasn't been transformed
		*/
		beenTransformed = FALSE;

		/*
		// Save a pointer to the bounidng box
		*/
		pBBox = &bboxInWC;
	}
	/*
	// Else the object is infinite, so the only way to determine if
	// it is on screen is to transform the planes first
	*/
	else
	{
		SGL_TIME_SUSPEND(TRIVIAL_REJECTION_TIME)
		SGL_TIME_START(TRANSFORM_PLANES_TIME)
		RnTransformBasicPlanes( pConvex->plane_data,
								pConvex->u16_num_planes,
								pState->pTransformState, 
								transformedPlanes);
		SGL_TIME_STOP(TRANSFORM_PLANES_TIME)
		SGL_TIME_RESUME(TRIVIAL_REJECTION_TIME)
		
		offScreen = RnTestPlanesWithCamera( transformedPlanes, 
											pConvex->u16_num_planes);

		beenTransformed = TRUE;

		/*
		// There isn't a bounding box so save as NULL
		*/
		pBBox = NULL;
	}/*end if/else bounding box*/

	SGL_TIME_STOP(TRIVIAL_REJECTION_TIME)

	SGL_TIME_START(PROCESS_CONVEX_NODE_TIME)
	/*//////////////////////////////////////
	// Decide what processing needs to be done, based on the type of
	// convex primitive, setting the flags which enable the various
	// bits of processing to be done.
	////////////////////////////////////// */
	ConvexType = (CONVEX_FLAGS_ENUM) (pConvex->u16_flags & cf_mask_type);
	switch(ConvexType)
	{
		/*///////////////////////////////////////////////////////
		// CASE Standard convex primitive
		/////////////////////////////////////////////////////// */
		case cf_standard_convex:
		{
			/*
			// generate shadows? Must have a light that casts shadows, and 
			// have shadows switched on in the quality.
			*/
			generateShadowVolumes=(pState->pLightsState->flags & lsf_shadows) &&
						 (pState->pQualityState->flags & qf_cast_shadows);

			/*
			// Do we bother transforming the convex?
			*/
			if((!beenTransformed) && (!offScreen || generateShadowVolumes))
			{
	    	    SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
			    SGL_TIME_START(TRANSFORM_PLANES_TIME)
				RnTransformBasicPlanes( pConvex->plane_data,
								pConvex->u16_num_planes,
								pState->pTransformState, 
								transformedPlanes);
			    SGL_TIME_STOP(TRANSFORM_PLANES_TIME)
	    	    SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)

				beenTransformed = TRUE;

			}/*end if transformPrim*/

			/*
			// Do we have to project/classify/shade the primitive
			*/
			if(!offScreen)
			{
				ASSERT(beenTransformed);

	    	    SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
				if(! RnProcessVisibleConvex( pConvex,  pState,
											 transformedPlanes,  
											 NeedsClipping, 
											 pBBox,
											 &offScreen,
											 nCurrTransSetID))
				{
	    	        SGL_TIME_STOP(PROCESS_CONVEX_NODE_TIME)
					return FALSE; /*out of parameter space*/
				}
	    	    SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)
			}/*end if projectEtcPrim */


			/*
			// Generate Shadows?
			*/
			if(generateShadowVolumes)
			{
				DPF((DBG_MESSAGE, "**************************"));
	    	    SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
	    	    SGL_TIME_START(PROCESS_SHADOW_TIME)
				if(! RnProcessConvexShadows( pConvex,  pState, 
											 pShadowLimitPlanes,
											 transformedPlanes,  
											 pBBox, offScreen,
											 !offScreen))
				{
	    	        SGL_TIME_STOP(PROCESS_SHADOW_TIME)
	    	        SGL_TIME_STOP(PROCESS_CONVEX_NODE_TIME)
					return FALSE;  /*out of parameter space*/
				}
	    	    SGL_TIME_STOP(PROCESS_SHADOW_TIME)
	    	    SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)

			}/*end if shadows wanted*/


			/*
			// perform collision detection?
			// Must be collision points, then either full collision detection,
			//  or onscreen and onscreen collision detection.
			*/
			if( (pState->pCollisionState->num_pnts != 0) &&
				((pState->pQualityState->flags & qf_full_collision) ||
				 (!offScreen &&	
				   (pState->pQualityState->flags & qf_onscreen_collision))))
			{
				/*
				// which space do we do the detection in? If the primitive has
				// been transformed, we may as well do it in world coordinates,
				*/
				if(beenTransformed)
				{
					RnTestPointsWithTransformedPlanes(
					  pConvex,
					  transformedPlanes,
					  pState->pCollisionState,
					  parentUpdatePoints);
				}
				else
				{
					RnTestPointsWithLocalPlanes(
					  pConvex,
					  pState->pTransformState,
					  pState->pCollisionState,
					  parentUpdatePoints);
				}/*end if beenTransformed*/
			}/*end if do collision detection*/


			break;

		}

		/*///////////////////////////////////////////////////////
		// CASE "hidden" primitive, for shadows and/or collision testing
		/////////////////////////////////////////////////////// */
		case cf_hidden_convex:
		{

			/*
			// generate shadows? Must have a light that casts shadows, and 
			// have shadows switched on in the quality.
			*/
			if( (pState->pLightsState->flags & lsf_shadows) &&
				(pState->pQualityState->flags & qf_cast_shadows))
			{
				/*
				// Do we bother transforming the convex?
				*/
				if(!beenTransformed)
				{
	    	        SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
					SGL_TIME_START(TRANSFORM_PLANES_TIME)
					RnTransformBasicPlanes( pConvex->plane_data,
								pConvex->u16_num_planes,
								pState->pTransformState, 
								transformedPlanes);
					SGL_TIME_STOP(TRANSFORM_PLANES_TIME)
	    	        SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)

					beenTransformed = TRUE;

				}/*end if transformPrim*/

				/*
				// Generate Shadows
				*/
				DPF((DBG_MESSAGE, "**************************"));
	    	    SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
	    	    SGL_TIME_START(PROCESS_SHADOW_TIME)
				if(! RnProcessConvexShadows( pConvex,  pState, 
											 pShadowLimitPlanes,
											 transformedPlanes,  
											 pBBox, offScreen,
											 !offScreen))
				{
	    	        SGL_TIME_STOP(PROCESS_SHADOW_TIME)
	    	        SGL_TIME_STOP(PROCESS_CONVEX_NODE_TIME)
					return FALSE;  /*out of parameter space*/
				}
	    	    SGL_TIME_STOP(PROCESS_SHADOW_TIME)
	    	    SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)
			}

			/*
			// perform collision detection?
			// Must be collision points, then either full collision detection,
			//  or onscreen and onscreen collision detection.
			*/
			if( (pState->pCollisionState->num_pnts != 0) &&
				((pState->pQualityState->flags & qf_full_collision) ||
				 (!offScreen &&	
				   (pState->pQualityState->flags & qf_onscreen_collision))))
			{
				/*
				// which space do we do the detection in? If the primitive has
				// been transformed, we may as well do it in world coordinates,
				*/
				if(beenTransformed)
				{
					RnTestPointsWithTransformedPlanes(
					  pConvex,
					  transformedPlanes,
					  pState->pCollisionState,
					  parentUpdatePoints);
				}
				else
				{
					RnTestPointsWithLocalPlanes(
					  pConvex,
					  pState->pTransformState,
					  pState->pCollisionState,
					  parentUpdatePoints);
				}/*end if beenTransformed*/
			}/*end if do collision detection*/

			break;
		}


		/*///////////////////////////////////////////////////////
		// CASE Light Volume / Forced Shadow Volume primitive
		// Use default case as no other (legal) cases are left
		/////////////////////////////////////////////////////// */
		default:
		{
			int volumesLightSlot;

			/*
			// Check that we have been given something sensible
			*/
			ASSERT(ConvexType==cf_shadow_volume || ConvexType==cf_light_volume);

			/*
			// Determine if the light associated with the light volume
			// CAN cast shadows, and what shadow slot number it uses.
			*/
			volumesLightSlot = RnGetLightsSlot(pConvex->u16_volumes_light, pState);

			/*
			// if the associated light can cast shadows
			*/
			if(volumesLightSlot != 0)
			{
				/*
				// If this is a light volume, then we must check
				// if we've already processed a light volume before. We have
				// to "initialise" registers in the hardware in order
				// to handle light vols, on a region by region basis.
				*/
				if(ConvexType==cf_light_volume && !doneALightVol)
				{
					DPF((DBG_MESSAGE, "***Initialising for light volume***"));
					AllowLightVolAdditionSGL(& pProjMat->RegionsRect);
					doneALightVol = TRUE;
				}


				/*
				// Only interested in processing this primitive if it
				// in onscreen
				*/
				if(!offScreen)
				{
					/*
					// Do we need to transform the planes ? 
					*/
					if(!beenTransformed)
					{
	    	            SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
					    SGL_TIME_START(TRANSFORM_PLANES_TIME)
						RnTransformBasicPlanes( pConvex->plane_data,
								pConvex->u16_num_planes,
								pState->pTransformState, 
								transformedPlanes);
					    SGL_TIME_STOP(TRANSFORM_PLANES_TIME)
	    	            SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)

					}/*end if transformPrim*/

					/*
					// Project and classify etc the primitive
					*/
	    	        SGL_TIME_SUSPEND(PROCESS_CONVEX_NODE_TIME)
					if(!RnProcessLightOrShadVolume(pConvex, pState,
												transformedPlanes, 
												pBBox) )
					{
	    	            SGL_TIME_STOP(PROCESS_CONVEX_NODE_TIME)
						return FALSE;  /*out of parameter space*/
					}
	    	        SGL_TIME_RESUME(PROCESS_CONVEX_NODE_TIME)

				}/*end if not offscreen*/

			}/*end if valid shadow light */

			break;
		}/*default (ie. shadow/light volume cases) */

	} /*End Switch*/

	/*
	// Got this far so should be ok!
	*/
	SGL_TIME_STOP(PROCESS_CONVEX_NODE_TIME)
	return TRUE;
}


/**************************************************************************
 * Function Name  : RnCTPreProcessConvexNode
 * Inputs         : pConvex- pointer to the convex primitive node
 *					pState,
 *
 * Outputs        : 
 * Input/Output	  : pState- pointer to a master state stack "frame"
 * Returns        : TRUE if the convex was processed OK, else we are
 *				    out of parameter space, so may as well give up on the
 *					rest of the render
 * Global Used    : See description
 *
 * Description    : This function is used when deciding what cached textures
 *					need to be loaded. 
 *
 *
 *					It checks if the bounding box is on screen, then
 *					estimates its size in pixels. 
 *					All cacheable textures that are on the object are then
 *					marked as needed with this size.
 *
 *					Optimisations: If the object has no local materials, and
 *					the global material does no use cached textures, then
 *					we can get out early. 
 **************************************************************************/

void RnCTPreProcessConvexNode(const CONVEX_NODE_STRUCT  *pConvex,
							 const MASTER_STATE_STRUCT *pState,
							 void *pCachedTexture)
{
	/*
	// pointer to local material
	*/
	LOCAL_MATERIALS_STRUCT * pLocalMaterial;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	int size,YSize, i;
	
	/*
	// Transformed Bounding box (if any) in world Coordinates
	*/
	BBOX_MINMAX_STRUCT bboxInWC;

	/*
	// The regions the box spans
	*/	
	REGIONS_RECT_STRUCT Regions;

	pLocalMaterial = pConvex->local_materials;
	/*
	// If there definately arent any cached textures, get out now
	*/
	if((pCachedTexture== NULL) && (pLocalMaterial == NULL))
	{
		return;
	}

	/*
	// If the quality flags say there is no texture - get out as well
	*/
	if(! pState->pQualityState->flags & qf_textures)
	{
		return;
	}

	/*
	// Decide if the object is on screen
	//
	// See if the primitive has a bounding box,
	// and transform the box.
	*/
	if(pConvex->u16_flags & cf_has_bbox)
	{
		
		TransformBBox(pState->pTransformState, 
					 & (pConvex->bbox),	 &bboxInWC);


		/*
		// Determine the size by looking at the bounding box regions
		*/
		if(! RnDetermineRegionsFromBBox(&bboxInWC, &Regions))
		{
			/*
			// Get out of here
			*/
			DPF((DBG_VERBOSE, "BBox Offscreen:"));
			return;
		}

		/*
		// Ok map these into sizes
		*/
		size= (Regions.LastXRegion - Regions.FirstXRegion +1) * 
									 pProjMat->RegionInfo.XSize;
		YSize= (Regions.LastYRegion - Regions.FirstYRegion +1) * 
									 pProjMat->RegionInfo.YSize;

		if(YSize > size)
		{
			size = YSize;
		}

	}
	/*
	// Else the object is infinite: To make life easy just assume it is on screen
	// and that it is large.
	*/
	else
	{
		size = 1024;
	}/*end if/else bounding box*/




	/*
	// Now step through the materials used, (including the first) and
	// set up any cacheable textures. NOTE this doesnt check to see if
	// the texture IS actually used, but who cares - it's close enough
	*/
	if(pCachedTexture != NULL)
	{
		MarkCachedTextureUsed(pCachedTexture, size);
	}

	for(i = pConvex->u16_num_materials; i != 0;   i--, pLocalMaterial++)
	{
		void * pPreviousCText;

		pPreviousCText = pCachedTexture;
		/*
		// Update using this material
		*/
		if(pLocalMaterial->p_material != NULL)
		{
			RnCTPreprocessMaterialNode(pLocalMaterial->p_material, 
					(MASTER_STATE_STRUCT *) pState, &pCachedTexture);
		}

		/*
		// if the texture has changed, and we still have a cached texture,
		// mark it as used
		*/
		if((pPreviousCText != pCachedTexture) &&(pCachedTexture != NULL))
		{
			MarkCachedTextureUsed(pCachedTexture, size);
		}
		
	}
}





/*
// END OF FILE
*/

