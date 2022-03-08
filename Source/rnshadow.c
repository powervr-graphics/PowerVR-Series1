/******************************************************************************
 * Name         : rnshadow.c
 * Title        : Shadow volume generation.
 * Author       : Simon Fenney; converted to SGL by John Catchpole.
 * Created      : RGL: 11/06/1993, SGL: 11/08/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Internal routine for shadow volume generation.
 *
 * Platform     : ANSI
 *
 * Modifications:
 *
 * SGL version:
 * $Log: rnshadow.c,v $
 * Revision 1.12  1997/04/07  14:47:44  gdc
 * shadow limit planes now come in as a function parameter rather
 * than a global stucture - also so old if-zero code removed
 *
 * Revision 1.11  1996/10/16  14:40:58  gdc
 * added shadow limiting planes to shadow volumes where required
 *
 * Revision 1.10  1996/10/11  12:05:14  mal
 * Removed the ; from the metric macros.
 *
 * Revision 1.9  1996/10/04  15:26:42  mal
 * oops a mistype
 *
 * Revision 1.8  1996/10/04  14:54:54  mal
 * Added some metric macros.
 *
 * Revision 1.7  1996/09/12  13:23:39  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES.
 * Shadow objects may now have more than SGL_MAX_PLANES.
 * Actually twice as many. Re: NHE 044A, 049A
 *
 * Revision 1.6  1996/09/09  17:07:10  mal
 * Fixed 2 Bugs.
 * 1 - There was a missing parameter in the argument list here.
 * 2 - NAB. Changed the check for the bounds of the array on
 * the number of shadow planes generated in this routine.
 *
 * Revision 1.5  1995/11/08  12:02:49  sjf
 * good old gcc picked it up tho'.
 *
 * Revision 1.4  1995/11/06  15:17:49  sjf
 * Made more effective use of pointer incrementing.
 *
 * Revision 1.3  1995/08/27  15:29:19  sjf
 * Coped with light parallel light direction being opposite to the expected direction.
 *
 * Revision 1.2  1995/08/22  09:44:46  jrc
 * Added a couple more ASSERTs.
 *
 * Revision 1.1  1995/08/14  15:47:55  jrc
 * Initial revision
 *
 * RGL version:
 * Revision 1.3  93/11/11  09:12:05  SimonF
 * Fixed bug of unfinished comment !
 * 
 * Revision 1.2  93/09/16  15:24:27  SimonF
 * Code now handles case when light is inside the object.
 * 
 * Revision 1.1  93/09/03  11:01:50  SimonF
 * Initial revision
 *
 *****************************************************************************/

#define MODULE_ID MODID_RN

#include "sgl_defs.h"
#include "debug.h"
#include "sgl_math.h"
#include "dlnodes.h"
#include "rnconvst.h"
#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/******************************************************************************
 * Function Name: RnGenerateShadowVolume
 *
 * Inputs       : pNode,pPlanes,nNumPlanes, pCurrentTransform,
 *				  bParallelLight,directionPosition
 * Outputs      : pShadowPlanes,pnNumShadowPlanes,pbLightInside
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Given an object's planes, and its adjacency information, this
 *				  generates a set of shadow planes for the shadow volume.
 *				  NOTE: It won't generate more than SGL_MAX_PLANES planes. This
 *				  can happen if an object has a large number of planes.
 *****************************************************************************/
void RnGenerateShadowVolume(CONVEX_NODE_STRUCT			 *pConvex,
							const TRANS_PLANE_ARRAY_TYPE  Planes,
							const TRANSFORM_STRUCT *pCurrentTransform,
							const SHADOW_LIM_STRUCT *pShadowLimitPlanes,
							const sgl_bool   bParallelLight,
							const sgl_vector directionPosition,
							TRANS_PLANE_ARRAY_TYPE 	ShadowPlanes,
							int					   *pnNumShadowPlanes,
							sgl_bool			   *pbLightInside,
							int					   *pNumOrigObjPlanes)

{
	int nPlane,nEdge,nAxis, i;


	/*
	// Array to store which planes are forward and which are reverse (wrt the
	// light).  Also store the dot product values.
	*/
	sgl_bool bForward[SGL_MAX_PLANES];
	float	fDotProd[SGL_MAX_PLANES];
	float *pfDotProd;
	sgl_bool *pbForward;


	EDGE_INFO_STRUCT *pEdgeInfo;
	EDGE_DATA_STRUCT *pThisEdge;

	/*
	// Pointer to a particular shadow plane
	*/
	TRANSFORMED_PLANE_STRUCT *pThisShadowPlane;

	/*
	// Pointers to the the forward and reverse planes that make up a silhoutte
	// edge.
	*/
	const TRANSFORMED_PLANE_STRUCT *pForwardPlane,*pReversePlane;
	const TRANSFORMED_PLANE_STRUCT *pPlane;

	/*
	// Dot product values for the forward and reverse planes
	*/
	float fForwardDot,fReverseDot;

	int nNumPlanes;


	ASSERT(pConvex != NULL);
	ASSERT(Planes != NULL);

	SGL_TIME_START(SHADOW_VOL_TIME)

	/* There was an extra parameter being passed in - to be used later ? */
	*pNumOrigObjPlanes = *pNumOrigObjPlanes;

	nNumPlanes = pConvex->u16_num_planes;

	ASSERT(pCurrentTransform != NULL);
	ASSERT(directionPosition != NULL);
	ASSERT(ShadowPlanes != NULL);
	ASSERT(pnNumShadowPlanes != NULL);
	ASSERT(pbLightInside != NULL);

	/*
	// Step through the array of object planes, getting the dot product of the
	// light direction/position and the normal to the plane, and classifying the
	// planes into forward and reverse.
	//
	// NOTE if the light is a point source light, then the direction/position
	// gives its position, BUT If it is a parallel light, then its the direction
	// TO the light (this is the opposite to what is stated in the real-time
	// shadow generation document).
	//
	// Deciding which planes are forward and which are reverse ALSO depends on 
	// the type of the light.
	*/
	pPlane = Planes;


	/*
	// Initialise the list of shadow volume surfaces.
	*/
	*pnNumShadowPlanes = 0;

	/*
	// Add all the forward planes to the list.
	//
	// If ALL the objects planes are 'reverse' planes wrt the light, then the
	// light is inside the object. The shadow volume is then constructed
	// slightly differently.
	//
	// Note that we don't have to check about overflowing the shadow array, as
	// there can't be more forward planes than the max number of object planes.
	*/

	*pbLightInside = TRUE;

	/*
	// If a parallel light
	*/
	if (bParallelLight)
	{
		for ( nPlane = nNumPlanes, pfDotProd=fDotProd, pbForward = bForward;
			  nPlane!=0 ; nPlane--, pPlane ++, pfDotProd++, pbForward++)
		{
			/*
			// Negate the dot prod as direction is opposite to original doc
			*/
			*pfDotProd =  -DotProd(directionPosition, pPlane->normal);

			/*
			// Determine if the plane is forward WRT the light
			*/
		 	
			if (*pbForward = (*pfDotProd < 0.0f))
			  {
				*pbLightInside = FALSE; /*Light isn't inside object*/
				ShadowPlanes[*pnNumShadowPlanes] = *pPlane; 
				(*pnNumShadowPlanes)++;
			  }

		}/*end for*/
	}
	/*
	// Otherwise it is a point source light.
	*/
	else
	{
		for ( nPlane = nNumPlanes, pfDotProd=fDotProd, pbForward = bForward;
			  nPlane!=0 ; nPlane--, pPlane ++, pfDotProd++, pbForward++)
		{
			*pfDotProd = DotProd(directionPosition, pPlane->normal);

			/*
			// Determine if the plane is forward WRT the light
			*/

			if (*pbForward = (*pfDotProd > pPlane->d))
			  {
				*pbLightInside = FALSE; /*Light isn't inside object*/
				ShadowPlanes[*pnNumShadowPlanes] = *pPlane; 
				(*pnNumShadowPlanes)++;
			  }

		}/*end for*/
	}/*end if else light type*/

    /*
	// If there are shadow limit planes copy them to the shadow plane list
	*/
	if ( pShadowLimitPlanes->nNumShadLimPlanes > 0)
	{
	  for(i=0; i<pShadowLimitPlanes->nNumShadLimPlanes; i++)
	  {
		ShadowPlanes[*pnNumShadowPlanes] = 
		  (pShadowLimitPlanes->TransShadLimPlanes[i]); 
		(*pnNumShadowPlanes)++;
	  }
	}

	/*
	// If the light is inside the object, then copy ALL the planes to the
	// output.
	*/
	if (*pbLightInside)
	{
		pPlane = Planes;
	 	for (nPlane=0; nPlane < nNumPlanes; nPlane++)
		{
			  ShadowPlanes[*pnNumShadowPlanes] = *pPlane; 
			  (*pnNumShadowPlanes)++;
		}

	}
	/*
	// Else process the silhoutte edges - if there are any. Note that
	// a single plane does not generate edges, and no structure is stored
	// in that case.
	*/
	else if(pConvex->edge_info != NULL)
	{
		/*
		// Get access to the edge information
		*/
		pEdgeInfo = pConvex->edge_info;

		/*
		// Now examine the edge information. Step through the edges, and create
		// a shadow plane for each silhoutte edge. That is, where a forward and
		// a reverse surface meet. Note that there are slightly different
		// methods for parallel and point source lights.
		//
		// NOTE: Check that we don't overflow the max number of shadow planes.
		*/
		pThisEdge = pEdgeInfo->edges;
		for (nEdge=0; nEdge < pEdgeInfo->num_edges; nEdge++, pThisEdge++)
		{
			/*
			// Is this made from a forward and a reverse surface?
			*/
	  		if (bForward[pThisEdge->u16_plane1] != bForward[pThisEdge->u16_plane2])
			{
				pThisShadowPlane = & ShadowPlanes[*pnNumShadowPlanes];
				(*pnNumShadowPlanes)++;

				/*
				// Set up the shadow plane's representative point, by getting
				// the midpoint of the edge, and transforming it.
				*/
				TransformVector(pCurrentTransform, pThisEdge->mid_point, pThisShadowPlane->repPnt);

				/*
				// Get which planes are the forward and reverse, and the dot product values
				*/
				if (bForward[pThisEdge->u16_plane1])
				{
					pForwardPlane = Planes + pThisEdge->u16_plane1;
					fForwardDot   = fDotProd[pThisEdge->u16_plane1];

					pReversePlane = Planes + pThisEdge->u16_plane2;
					fReverseDot   = fDotProd[pThisEdge->u16_plane2];
				}
				else
				{
					pForwardPlane = Planes + pThisEdge->u16_plane2;
					fForwardDot   = fDotProd[pThisEdge->u16_plane2];

					pReversePlane = Planes + pThisEdge->u16_plane1;
					fReverseDot   = fDotProd[pThisEdge->u16_plane1];
				}

				/*
				// Handle point source and parallel lights differently
				*/
				if (bParallelLight)
				{
					/*
					// Calculate the d value
					*/
					pThisShadowPlane->d = fReverseDot * pForwardPlane->d - fForwardDot * pReversePlane->d;

					/*
					// Calculate the surface normal
					*/
					for (nAxis=0; nAxis < 3; nAxis++)
						pThisShadowPlane->normal[nAxis] = fReverseDot * pForwardPlane->normal[nAxis] - 
														  fForwardDot * pReversePlane->normal[nAxis];
				}
				/*
				// else is a point source light
				*/
				else
				{
					/*
					// Calculate the d value
					*/
					pThisShadowPlane->d = fForwardDot * pReversePlane->d - fReverseDot * pForwardPlane->d;

					/*
					// Calculate the surface normal. For efficiency, redefine
					// fReverseDot and fForwardDot (as we don't need them any
					// more).
					*/
					fReverseDot = (pReversePlane->d - fReverseDot);
					fForwardDot = (pForwardPlane->d - fForwardDot);

					for (nAxis=0; nAxis < 3; nAxis++)
						pThisShadowPlane->normal[nAxis] = fReverseDot * pForwardPlane->normal[nAxis] - 
						                                  fForwardDot * pReversePlane->normal[nAxis];

				}/*end if light type */


				/*
				// Break out of the loop if we have the max number of shadow
				// planes.
				*/
				if (*pnNumShadowPlanes == SGL_MAX_INTERNAL_PLANES)
				{
					break;
				}

			}/*end if silhoutte edge*/

		}/*end for i*/

	}/*end else*/

	SGL_TIME_STOP(SHADOW_VOL_TIME)

}/*end function*/


/*------------------------------- End of File -------------------------------*/
