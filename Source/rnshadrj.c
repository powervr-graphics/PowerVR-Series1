/**************************************************************************
 * Name         : rnshadrj.c
 * Title        : Render: Shadow Reject
 * Author       : Simon Fenney
 * Created      : 19th August 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Code for testing if we can reject shadows etc.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnshadrj.c,v $
 * Revision 1.5  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.4  1997/04/09  16:55:56  gdc
 * removed reference to projection_mat global
 *
 * Revision 1.3  1996/04/01  17:10:44  jrc
 * Negated the references to LightVec in the initial simple rejection testing.
 *
 * Revision 1.2  1996/04/01  14:35:30  jrc
 * 1. Bug fix: Now expects the light vector to have been reversed.
 * 2. Optimisation: Moved setting of new values for FurthestClosestForward and
 *    ClosestFurthestReverse to before where they are used for rejection tests.
 * 3. Bug fix: Added some minus signs for forward plane comparisons.
 *
 * Revision 1.1  1995/08/27  15:31:07  sjf
 * Initial revision
 *
 **************************************************************************/

/*
// define Debug Module IDs
*/
#define MODULE_ID	MODID_RN


#include "sgl_defs.h"
#include "sgl_math.h"
#include "dlnodes.h"
#include "rnglobal.h"


/**************************************************************************
 * Function Name  : RnRejectBoxShadow
 * Inputs         : pBBox   	Pointer to bounding box
 *					LightVec	Either the direction of a parallel light or
 *								the position of a point source light
 *					IsParallel	TRUE if light is a parallel source.
 *
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : TRUE if the shadow DEFINITELY doesn't come into the field
 *					of view
 *
 * Global Used    : Camera/projection matrix values
 *
 * Description    : This uses the basic algorithm for shadow generation, but
 *					applies it to the bounding box. The "shadow planes" generated
 *					are then tested against the viewing fustrum to see if the
 *					shadow is probaly visible or not.
 **************************************************************************/
sgl_bool RnRejectBoxShadow( const BBOX_MINMAX_STRUCT * pBBox,
						  const sgl_vector LightVec,
						  const	sgl_bool   IsParallel)
{
	#define CUBE_SIDES 6
	#define CUBE_EDGES 12
	#define TOO_SMALL_TO_DIVIDE  (9.536743164E-7f)  /* 2^-20 */

	#define REJECT_IT TRUE
	#define KEEP_IT	  FALSE
	/*
	// Define an internal enumerated type to define the planes of the
	// bounding box (for readibility)
	*/
	typedef enum {LEFT, RIGHT, BOTTOM, TOP, FRONT, BACK} SIDES;
	typedef enum {X, Y, Z} COORDS;

	int i;

	/*
	// define the normals of the box
	*/
	static const sgl_vector	BoxNorms[CUBE_SIDES] =
		   {{-1.0f, 0.0f, 0.0f},
			{ 1.0f, 0.0f, 0.0f},
			{ 0.0f,-1.0f, 0.0f},
			{ 0.0f, 1.0f, 0.0f},
			{ 0.0f, 0.0f,-1.0f},
			{ 0.0f, 0.0f, 1.0f}};

	/*
	// Define the Edge information  - this is a list of pairs of
	// planes that meet to form an edge
	*/
	static const sgl_uint8 Edges[CUBE_EDGES][2] =
	   { {TOP, LEFT},
		 {TOP, RIGHT},
		 {TOP, BACK},
		 {TOP, FRONT},
		 {BOTTOM, LEFT},
		 {BOTTOM, RIGHT},
		 {BOTTOM, BACK},
		 {BOTTOM, FRONT},
		 {LEFT, FRONT},
		 {FRONT, RIGHT},
		 {RIGHT, BACK},
		 {BACK, LEFT}};

	/*
	// An array of the "d" values for the box planes
	*/
	float BoxDs[CUBE_SIDES];

	/*
	// Array of dot prods with light direction
	*/
	float DotProds[CUBE_SIDES];

	/*
	// Array of which planes are forward with respect to the LIGHT
	*/
	sgl_bool Forward[CUBE_SIDES];

	/*
	// The normal and D value of each shadow plane as we encounter it 
	*/
	float SNx, SNy, SNz, Sd;

	/*
	// To check if the light is inside, count up how many forwards we get
	*/
	int  NumForwards;

	/*
	// pointers to the normals of two cube faces, and the
	// associated d values
	*/
	float ForDot;
	float RevDot;

	int ForwardFace, ReverseFace;

	/*
	// Values used to reject the shadow volume from the viewing fustrum
	*/
	float FurthestClosestForward, ClosestFurthestReverse;

	/*
	// Copies of values from the projection matrix, for convenience
	*/
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	float VpXc,	VpYc, VpXoff, VpYoff;


	/*///////////////////
	// A simple rejection test to see if the bounding box is entirely
	// in front of the forward plane, and that the light is behind the
	// the object.
	/////////////////// */
	if(pBBox->boxMax[Z] < pProjMat->foregroundDistance)
	{
		/*
		// Need to look at the type of the light.
		// If the light is parallel, and the light is shining
		// forward, then reject it.
		//
		// If a point source, and the point is behind the object, then
		// we can also reject it.
		*/
		if( (IsParallel && (LightVec[Z] > 0.0f)) ||
			(!IsParallel && (LightVec[Z] < pBBox->boxMax[Z])))
		{
	   		DPF((DBG_MESSAGE, "Light BEHIND too close BBOX"));
			return REJECT_IT;
		}
		
	}
	/*
	// Else there is a similar test with the background plane
	*/
	else if(pBBox->boxMin[Z] > pProjMat->BackgroundDistance)
	{
		if( (IsParallel && (LightVec[Z] < 0.0f)) ||
			(!IsParallel && (LightVec[Z] > pBBox->boxMin[Z])))
		{
	   		DPF((DBG_MESSAGE, "Light IN FRONT of too far BBOX"));
			return REJECT_IT;
		}
		
	}/*end simple rejection tests*/


	/*/////////////////////////////////////////
	// Now construct the shadow volume from the bounding box
	//////////////////////////////////////// */


	/*///////////////////
	// Get the box "D" values
	/////////////////// */
	BoxDs[LEFT]	  = -pBBox->boxMin[X];
	BoxDs[RIGHT]  =  pBBox->boxMax[X];
	BoxDs[BOTTOM] = -pBBox->boxMin[Y];
	BoxDs[TOP]	  =  pBBox->boxMax[Y];
	BoxDs[FRONT]  = -pBBox->boxMin[Z];
	BoxDs[BACK]	  =  pBBox->boxMax[Z];

	/*
	// Get the "dot" prods of the light vector and the plane normals.
	// This is trivial because of the 0's in the normals
	*/
	DotProds[LEFT]	=   LightVec[X];
	DotProds[RIGHT] = - LightVec[X];
	DotProds[BOTTOM]=   LightVec[Y];
	DotProds[TOP]	= - LightVec[Y];
	DotProds[FRONT]	=   LightVec[Z];
	DotProds[BACK]	= - LightVec[Z];

	/*
	// Depending on the light type, classify planes into forward and reverse
	// WRT the light
	*/
	if(IsParallel)
	{
		for(i = 0; i < CUBE_SIDES; i++)
		{
			Forward[i] = DotProds[i] < 0.0f;
		}
	}
	else
	{
		for(i = 0; i < CUBE_SIDES; i++)
		{
			Forward[i] = DotProds[i] > BoxDs[i];
		}
	}/*end if else*/

	/*
	// As part of the rejection testing..
	// Set intial limits on the range of (inverse) depths the
	// shadow volume spans in the viewport. Assume that it occupies
	// all of the viewport space, until proven otherwise
	*/
	FurthestClosestForward = pProjMat->invForegroundDistance;
	ClosestFurthestReverse = pProjMat->invBackgroundDistance;

	/*
	// For convenience, get access to the viewport "centre" and corner
	// offset values
	*/
	VpXc = pProjMat->VpCentre[0];
	VpYc = pProjMat->VpCentre[1];

	VpXoff = pProjMat->VpOffset[0];
	VpYoff = pProjMat->VpOffset[1];

	/*
	// Examine all the forward planes (wrt the light) as they are
	// part of the shadow volume.
	*/
	NumForwards = 0;

	for(i=0; i< CUBE_SIDES; i++)
	{
		if(Forward[i])
		{
			float Centre, Offset;

			NumForwards ++;
			/*
			// get the normal and D value
			*/
			SNx = BoxNorms[i][0];
			SNy = BoxNorms[i][1];
			SNz = BoxNorms[i][2];
			Sd	= BoxDs[i];

			/*
			// Can we reject the shadow
			//
			// If D is too small to handle, then just ignore the plane
			// This is quite safe since we are only trying to reject the
			// shadow, and it doesn't matter if we get it wrong.
			*/
			if(sfabs(Sd) < TOO_SMALL_TO_DIVIDE)
			{
				continue;  /*try the next plane*/
			}

			/*
			// Compute the centre and offset values
			*/
			Centre = SNx * VpXc + SNy * VpYc + SNz;
			Offset = sfabs(SNx * VpXoff) + sfabs(SNy * VpYoff);

			/*
			// Is this a forward plane?
			*/
			if(Sd < 0.0f)
			{
				float closest;
				/*
				// compute the closest (inverse) distance the plane
				// has in the viewport. We need the largest value possible
				*/
				closest = (-Centre + Offset) / -Sd;
			
				/*
				// If this is further than our best so far, update it
				*/
				if(closest < FurthestClosestForward)
				{
					FurthestClosestForward = closest;

					/*
					// Can we reject the shadow?
					*/
					if(FurthestClosestForward < ClosestFurthestReverse)
					{
						DPF((DBG_MESSAGE, "Rejecting BBOX Shadow"));
						return REJECT_IT;
					}

				}/*end if plane more limiting*/
			}
			/*
			// Else it is a reverse plane
			*/
			else
			{
				float furthest;
				/*
				// compute the furthest (inverse) distance the plane
				*/
				furthest = (Centre - Offset) / Sd;
			
				/*
				// If this is closer than our best so far, update it
				*/
				if(furthest > ClosestFurthestReverse)
				{
					ClosestFurthestReverse = furthest;

					/*
					// Can we reject the shadow?
					*/
					if(FurthestClosestForward < ClosestFurthestReverse)
					{
						DPF((DBG_MESSAGE, "Rejecting BBOX Shadow"));
						return REJECT_IT;
					}

				}/*end if plane more limiting*/

			}/*end plane classification*/
					
		}/*end if forward*/
	}/*end for*/

	/*
	// If the light is inside the object, then the shadow goes everywhere,
	// so we can't really reject it!
	*/
	if(NumForwards == 0)
	{
		/* (This can only happen with point source lights)*/
		ASSERT(!IsParallel); 

		DPF((DBG_MESSAGE, "Light inside BBOX"));
		return KEEP_IT;
	}


	/* ///////////////////////////////////////////////////
	// Now process the silhoutte edges, creating shadow planes from them
	// and testing each for rejection.
	/////////////////////////////////////////////////// */
	for(i=0; i < CUBE_EDGES; i++)
	{
		float Centre, Offset;

		/*
		// Note Forweard and reverse may have to be swapped
		*/
		ForwardFace = Edges[i][0];
		ReverseFace = Edges[i][1];

		ASSERT(ForwardFace != ReverseFace);

		/*
		// Is this a silhoutte edge?
		*/
		if(Forward[ForwardFace] != Forward[ReverseFace])
		{
			/*
			// If we haven't forward and reverse right, swap them
			*/
			if(!Forward[ForwardFace])
			{
				ForwardFace = ReverseFace;
				ReverseFace = Edges[i][0];
			}

			/*
			// Get local access to the dot products (with respect to the light)
			*/
			ForDot = DotProds[ForwardFace];
			RevDot = DotProds[ReverseFace];
		
			/*
			// Compute the equation for the shadow plane passing through the
			// edge.  This depends on the light type.
			*/
			if(IsParallel)
			{
				/*
				// Get the plane D
				*/
				Sd= RevDot * BoxDs[ForwardFace] - ForDot * BoxDs[ReverseFace];
				/*
				// Calculate the normal.
				// Given the number of zeros in the Norms, it might
				// be possible to optimise this by detecting which faces
				// are used, but I doubt the effort is worth it.
				*/
				SNx= RevDot * BoxNorms[ForwardFace][0] -
					 ForDot * BoxNorms[ReverseFace][0];
				SNy= RevDot * BoxNorms[ForwardFace][1] -
					 ForDot * BoxNorms[ReverseFace][1];
				SNz= RevDot * BoxNorms[ForwardFace][2] -
					 ForDot * BoxNorms[ReverseFace][2];
												
			}
			/*
			// Else is a point source light
			*/
			else
			{
				/*
				// Get plane D
				*/
				Sd = ForDot * BoxDs[ReverseFace] - 	RevDot * BoxDs[ForwardFace];

				/*
				// calculate the normal - for efficiency reuse the
				// dot prod values.
				*/
				ForDot = BoxDs[ForwardFace] - ForDot;
				RevDot = BoxDs[ReverseFace] - RevDot;
				SNx= RevDot * BoxNorms[ForwardFace][0] -
					 ForDot * BoxNorms[ReverseFace][0];
				SNy= RevDot * BoxNorms[ForwardFace][1] -
					 ForDot * BoxNorms[ReverseFace][1];
				SNz= RevDot * BoxNorms[ForwardFace][2] -
					 ForDot * BoxNorms[ReverseFace][2];
			}/*end if light type*/

			/*
			// Can we reject the shadow
			//
			// If D is too small to handle, then just ignore the plane
			// This is quite safe since we are only trying to reject the
			// shadow, and it doesn't matter if we get it wrong.
			*/
			if(sfabs(Sd) < TOO_SMALL_TO_DIVIDE)
			{
				continue;  /*try the next plane*/
			}

			/*
			// Compute the centre and offset values
			*/
			Centre = SNx * VpXc + SNy * VpYc + SNz;
			Offset = sfabs(SNx * VpXoff) + sfabs(SNy * VpYoff);

			/*
			// Is this a forward plane?
			*/
			if(Sd < 0.0f)
			{
				float closest;
				/*
				// compute the closest (inverse) distance the plane
				// has in the viewport. We need the largest value possible
				*/
				closest = (-Centre + Offset) / -Sd;
			
				/*
				// If this is further than our best so far, update it
				*/
				if(closest < FurthestClosestForward)
				{
					FurthestClosestForward = closest;

					/*
					// Can we reject the shadow?
					*/
					if(FurthestClosestForward < ClosestFurthestReverse)
					{
						DPF((DBG_MESSAGE, "Rejecting BBOX Shadow"));
						return REJECT_IT;
					}

				}/*end if plane more limiting*/
			}
			/*
			// Else it is a reverse plane
			*/
			else
			{
				float furthest;
				/*
				// compute the furthest (inverse) distance the plane
				*/
				furthest = (Centre - Offset) / Sd;
			
				/*
				// If this is closer than our best so far, update it
				*/
				if(furthest > ClosestFurthestReverse)
				{
					ClosestFurthestReverse = furthest;

					/*
					// Can we reject the shadow?
					*/
					if(FurthestClosestForward < ClosestFurthestReverse)
					{
						DPF((DBG_MESSAGE, "Rejecting BBOX Shadow"));
						return REJECT_IT;
					}

				}/*end if plane more limiting*/

			}/*end plane classification*/
					

		}/*end if silhoutte edge*/
	}/*end for processing edges*/



	/*
	// Well, if we've got this far, then we haven't managed to reject it
	*/
	return KEEP_IT;

} /*end function*/


/*
// END OF FILE
*/
