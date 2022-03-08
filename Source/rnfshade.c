/**************************************************************************
 * Name         : rnfshade.c
 * Title        : Flat Shading 
 * Author       : Stel Michael
 * Created      : 20th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Routines to handle the flat shading of planes
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnfshade.c,v $
 * Revision 1.34  1997/06/04  11:15:25  gdc
 * removed compiler warnings
 *
 * Revision 1.33  1997/04/07  16:38:35  gdc
 * removed references to current_light_slot global
 *
 * Revision 1.32  1996/10/10  11:10:01  mal
 * Modified some of the metrics macros.
 *
 * Revision 1.31  1996/10/04  14:32:45  mal
 * Added some metric macros for profiling the code.
 *
 * Revision 1.30  1996/08/14  14:41:57  pdl
 * changed some #ifdefs to #if
 *
 * Revision 1.29  1996/08/06  10:28:14  mal
 * Some < and > were the wrong way round. opps.
 *
 * Revision 1.28  1996/08/06  08:43:04  mal
 * Changed some fp comparisons to long ones for performance optimizations.
 *
 * Revision 1.27  1996/08/05  09:05:16  gdc
 * altered logic of FCMP's (see prev) to avoid potential colour change due to
 * clipping twice
 *
 * Revision 1.26  1996/08/01  16:10:13  gdc
 * modified bounds checks on colour values to use integer compares for pentiums
 * via CHOOSE_FLOAT_MAX/MIN macros
 *
 * Revision 1.25  1996/07/11  16:40:22  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.24  1996/06/13  14:47:05  sjf
 * Further eased up assert checking on some floating point calcs.
 *
 * Revision 1.23  1996/05/21  14:01:21  sjf
 * Made checking for normalised vector less strict in ASSERT
 *
 * Revision 1.22  1996/04/22  18:01:01  sjf
 * Added a routine specifically for when all lights are grey and we
 * are texturing. It might save a little bit.
 *
 * Revision 1.21  1995/10/07  15:09:04  sjf
 * removed dud local variables.
 *
 * Revision 1.20  1995/10/06  20:06:19  sjf
 * Rewrote the flat shading routines to take advantage of the fact that I
 * ordered the lights into 3 consecutive groups - On Parallel, On Point, and
 * Off lights. This means less ifs in per plane, and that the parallel
 * shading now does less copying of data.
 * It, of course, makes the code a lot longer.
 *
 * Revision 1.19  1995/09/25  18:09:13  sjf
 * Added (untested as yet) multi pseudo shadows.
 *
 * Revision 1.18  1995/09/16  19:45:22  sjf
 * Changed ASSERTS to warnings as they weren't fatal, just something
 * to clean up later.
 *
 * Revision 1.17  1995/09/15  19:49:39  sjf
 * Removed dud param from shading routines.
 *
 * Revision 1.16  1995/09/12  09:15:21  sjf
 * 1) Fixed grey lights - was treating as if red!
 * 2) "Factorised" the material colours out of the lights loop - this
 *     should speed things up when there is more than one light, but
 *     probably will slow down with a single light.
 *
 * Revision 1.15  1995/08/28  16:54:22  sjf
 * 1) Made some optimisations for the non textured shading
 * 2) Redid the textured shading (based on the non textured) so that
 *    shadows are supported.
 *
 * Revision 1.14  1995/08/27  15:25:13  sjf
 * 1) Cleaned up Flat NON TEXTURED shading and added shadow handling...
 * NOTE: Need to do similar to the Textured version
 * NOTE2: View Vector calculation could be taken out of the lights loop!!
 *
 * Revision 1.13  1995/08/04  19:03:53  sm
 * added spots to texture shading
 *
 * Revision 1.12  1995/08/04  14:36:19  sm
 * added spot lighting
 *
 * Revision 1.11  1995/08/04  10:34:45  sm
 * added point lights (no spot lights yet!)
 *
 * Revision 1.10  1995/08/02  15:03:44  sm
 * split up in two routines
 *
 * Revision 1.9  1995/08/01  15:39:42  sm
 * made use specified parallel lights
 *
 * Revision 1.7  1995/07/30  21:41:26  sm
 * *** empty log message ***
 *
 * Revision 1.6  1995/07/30  16:00:10  sm
 * changed specular stuff a bit
 *
 * Revision 1.5  1995/07/28  18:26:28  sm
 * commented power function for highlights back in
 *
 * Revision 1.4  1995/07/28  17:31:49  sm
 * fixed smooth shading (ish)
 *
 * Revision 1.3  1995/07/27  09:54:46  sm
 * moved dot prod calc into plane loop
 *
 * Revision 1.2  1995/07/25  16:52:46  sm
 * implemented version with a fixed parallel light source
 *
 * Revision 1.1  1995/07/21  15:53:06  sm
 * Initial revision
 *
 *
 *
 **************************************************************************/

#include "sgl_defs.h"
#include "sgl.h"
#include "sgl_math.h"
#include "nm_intf.h"
#include "dlnodes.h" 

#include "rnstate.h"
#include "rntrav.h"
#include "rnconvst.h"
#include "rnfshade.h"

#include "rnglobal.h"

#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

static const sgl_vector ZERO_VEC={0.0f, 0.0f, 0.0f};

/* a compare and assign macro for pentiums */
#if SLOW_FCMP
#define CHOOSE_FLOAT_MIN(a,b,c) ((b)<FLOAT_TO_LONG(a)?(a=c):(a))
#define CHOOSE_FLOAT_MAX(a,b,c) ((b)>FLOAT_TO_LONG(a)?(a=c):(a))
#endif

/*
// Determine if we use pow or the quicker approximation
*/
#define USE_POW 0

/*
// Define a small value to decide whether shading is worthwhile. Note
// make this a power of two so that it can be loaded with a 16bit
// number if the architecture allows it
*/
#define TWO_TO_MINUS_10 	 (9.765625E-4f)
#define TWO_TO_MINUS_10_LONG (0x2F5BE6FFl)

/**************************************************************************
 * Function Name  : DoFlatShading
 * Inputs         : ReversePlanes - whether batch of given planes are reverse visible
   					numPlanes     - number of transformed planes
					transPlanes[] - array of pointers to transform plane struct
					stateMaterial - current material	
					lightState   - current light , not use in this version !!

 * Outputs        :	ShadingResults - shading colour for batch of given planes
  
 * Input/Output	  :
						  
 * Returns        : 
 * Global Used    : 
 * Description    :
 *
 *					NOTE: Lights have been grouped into all the ON parallel
 *					lights followed by the ON Point lights followed by the
 *					off lights.
 *					
 *				   
 **************************************************************************/

void	DoFlatShading( sgl_bool ReversePlanes, 
					   int numPlanes, 
							 TRANSFORMED_PLANE_STRUCT * ptransPlanes[],
					   const TRANSFORM_STRUCT		  * pTransform,
					   const MATERIAL_STATE_STRUCT	  * pMaterial, 
					   const LIGHTS_STATE_STRUCT	  * lightState,
							 SHADING_RESULT_STRUCT	  * pShadingResults)
{
	const TRANSFORMED_PLANE_STRUCT *pPlane;

	int	lightC; /*to count DOWN through the lights*/
	const LIGHT_ENTRY_STRUCT * pLight;

#if !SLOW_FCMP
	int i;
#endif
	sgl_vector	ReflVec;
	sgl_vector UnitNormalVec;
	float	LightNormalDP;


	sgl_bool 	DoSpecular;
	sgl_bool 	IsShadowed;

	sgl_vector AmbientGlowCol;

	/*
	// The following is initialised to 0 if there are
	// no "multi shadow" lights, else it is set to the
	// intensity of the multi shadow light
	*/
	sgl_vector InitialSlot1Diffuse;

	/*
	// Local results of specular and diffuse shading
	// for a particular plane. This is an array, one entry
	// per light slot, with diffuse and specular components.
	*/
	struct
	{
		sgl_vector Diffuse;
		sgl_vector Specular;

	} LocalResults[2], *pThisSlot;

	SGL_TIME_START(FLAT_PARAM_TIME)

	/*
	// Decide if we need to process shadows
	*/
	IsShadowed = (lightState->flags & lsf_shadows);

	/*
	// Check that the light flag is set up correctly in the lights state
	*/
#if DEBUG
	{
		sgl_bool CheckShadowed;

		CheckShadowed = FALSE;
		pLight = lightState->light_entries;
		for (lightC=lightState->num_lights; lightC!=0; lightC --, pLight++)
		{
			/*
			// if this light is on AND casts shadows
			*/
			if ( (pLight->light_flags & light_on) &&
			   (pLight->light_colour_slot != 0) )
			{
				CheckShadowed = TRUE;
				break;
			}/*end if*/
		}/* end for*/

		if(IsShadowed)
		{
			ASSERT(CheckShadowed);
		}
		else
		{
			ASSERT( ! CheckShadowed);
		}
	}
#endif

	/*
	// Do we need to do specular highlights
	*/
	DoSpecular = (pMaterial->specular_shininess_float != 0.0f);

	/*
	// Compute the ambient and glow colours. These will stay constant for
	// this set of planes
	*/
  	if (lightState->flags & lsf_ambient_grey)
   	{
		float greyLev;
   		/* grey value is stored in red component */
		greyLev = lightState->ambient_colour[0];

   		AmbientGlowCol[0] =	pMaterial->glow[0] + 
			(pMaterial->ambient[0]*	greyLev);

   		AmbientGlowCol[1] =	pMaterial->glow[1] + 
			(pMaterial->ambient[1]*	greyLev);

   		AmbientGlowCol[2] =	pMaterial->glow[2] + 
			(pMaterial->ambient[2]* greyLev);
   	}
   	else
   	{
   		AmbientGlowCol[0] = pMaterial->glow[0] + 
			(pMaterial->ambient[0]* lightState->ambient_colour[0]);
   		AmbientGlowCol[1] =	pMaterial->glow[1] +
			(pMaterial->ambient[1]*	lightState->ambient_colour[1]);
   		AmbientGlowCol[2] = pMaterial->glow[2] + 
			(pMaterial->ambient[2]*	lightState->ambient_colour[2]);
   	}

	/*
	// Determine if we have to add in any multi shadow lights
	//
	// THIS code currently assumes 2 light slots
	*/
	ASSERT(	RnGlobalGetCurrentLightSlot() < NUM_LIGHT_SLOTS);
	ASSERT(NUM_LIGHT_SLOTS == 2);

	if(IsShadowed && lightState->light_slots[1].multi_light)
	{
		/*
		// We have multi shadow lights.. put the colour into the
		// diffuse value
		*/
		VecCopy(lightState->light_slots[1].colour, InitialSlot1Diffuse);
	}
	else
	{
		VecCopy(ZERO_VEC , InitialSlot1Diffuse);
	}

	/*
	// Step through the planes. Count down with the counter,
	// and increment a pointer to the shading results as well
	// (we don't use the plane count for anything but setting the number
	// of iterations)
	*/
	for(/*Nil*/; numPlanes != 0 ; numPlanes--, pShadingResults++)
	{
		/*
		// get convenient access to this planes pointer.
		*/
		pPlane = *ptransPlanes;

		/*
		// Move on to the next position in the array of plane pointers
		*/
		ptransPlanes ++;

		/*
		// Initialise the Diffuse and Specular Multipliers
		*/
		VecCopy(ZERO_VEC, LocalResults[0].Diffuse);
		if(DoSpecular)
		{
			VecCopy(ZERO_VEC, LocalResults[0].Specular);
		}

		if(IsShadowed)
		{
			VecCopy(InitialSlot1Diffuse, LocalResults[1].Diffuse);
			if(DoSpecular)
			{
				VecCopy(ZERO_VEC, LocalResults[1].Specular);
			}
		}

		/* 
		// The normal in the transformed planes has to be normalised.
		// The way this is done depends on the type of the 
		// transformation matrix.
		*/
		switch(pTransform->scale_flag)
		{
			case no_scale:
				 VecCopy(pPlane->normal, UnitNormalVec);
				 break;

			case uniform_scale:
				 UnitNormalVec[0] = pPlane->normal[0] * pTransform->rescale;
				 UnitNormalVec[1] = pPlane->normal[1] *	pTransform->rescale;
				 UnitNormalVec[2] = pPlane->normal[2] *	pTransform->rescale;

				 break;

			default:
				ASSERT(pTransform->scale_flag == arbitrary_scale);
				VecCopy(pPlane->normal, UnitNormalVec);
				VecNormalise(UnitNormalVec);
				break;

		}/*end switch normalising normal*/

		/*
		// Flip the normal for reverse planes
		// This doesn't happen all that often, so this there probably is no
		// point in combining it with the renormalising.
		*/
		if (ReversePlanes)
		{		
			VecNegate(UnitNormalVec);
		}

		/*
		// just check vector IS normalised
		*/
		ASSERT(sfabs(VecLength(UnitNormalVec) - 1.0f) < 1.0E-3f);

		/*/////////////////////////////////
		// If we have some specular component, get a unit "reflection
		// direction" vector
		///////////////////////////////// */
		if (DoSpecular)
		{
			float DPt2;
			float InvLen;

			/* 
			// calculate reflection vector.. get length of "view vector"
			// so we can "normalise" it later.
			*/
			InvLen = 1.0f / VecLength(pPlane->repPnt);

			DPt2 = 2.0f * DotProd(pPlane->repPnt,UnitNormalVec);

			ReflVec[0] = (pPlane->repPnt[0] - (DPt2* UnitNormalVec[0]))*InvLen;
			ReflVec[1] = (pPlane->repPnt[1] - (DPt2* UnitNormalVec[1]))*InvLen;
			ReflVec[2] = (pPlane->repPnt[2] - (DPt2* UnitNormalVec[2]))*InvLen;
		}



		/* //////////////////////////////////
		// get convenient access to the light entries
		////////////////////////////////// */
		pLight = lightState->light_entries;


	   	/* ///////////////////////////////////////////////////////////////////
		// step through the Parallel lights
		/////////////////////////////////////////////////////////////////// */
		for (lightC=lightState->numOnParLights; lightC != 0 ; lightC--, pLight++)
		{
			ASSERT(pLight->light_flags & light_on);
			ASSERT((pLight->light_flags & mask_light_types )== parallel_light_type);

			/*/////////////////////////////////
			// Determine which slot to use for the lights results.
			// Also remember if we use a shadow slot (i.e. a non zero)
			///////////////////////////////// */
			ASSERT(pLight->light_colour_slot==0||pLight->light_colour_slot==1);

			pThisSlot  = & LocalResults[pLight->light_colour_slot];

			/*/////////////////////////////////
			// Do the specular component (if any)
			///////////////////////////////// */
			if (DoSpecular)
			{
				float SpecularFactor;

				SpecularFactor = DotProd(ReflVec,pLight->direction);
				ASSERT(SpecularFactor < 1.01f); /*less than 1, plus slop*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if ( SpecularFactor > TWO_TO_MINUS_10 )
			#endif
				{
					#if ! USE_POW
						/*
						// clip Specular factor to make sure it doesnt 
						// go over 1 
						*/	
						SpecularFactor = MIN(1.0f, SpecularFactor); 	
						SpecularFactor = (1.0f - SpecularFactor) * pMaterial->specular_shininess_float;
						SpecularFactor = 1.0f - MIN(1.0f, SpecularFactor );
						SpecularFactor*=SpecularFactor;
					#else
						SpecularFactor=(float) pow( SpecularFactor, pMaterial->specular_shininess_float);  
					#endif 

					/*
					// After raising to the power, is the lighting
					// significant?
					*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) < TWO_TO_MINUS_10_LONG )
			#else
				if ( SpecularFactor < TWO_TO_MINUS_10 )
			#endif
					{
						/* No it isn't. Do nothing*/
					}
					/*
					//  ELSE add the specular component to the
					//  shading result, depending on whether we have
					// a grey or coloured light
					*/
					else if(pLight->light_flags & coloured)
					{
						pThisSlot->Specular[0]+=SpecularFactor*pLight->colour[0];
						pThisSlot->Specular[1]+=SpecularFactor*pLight->colour[1];
						pThisSlot->Specular[2]+=SpecularFactor*pLight->colour[2];
					}
					/*
					// Else is a grey light, get the intensity from the 
					// red component
					*/
					else
					{
						float Factor = SpecularFactor*pLight->colour[0];

						pThisSlot->Specular[0] += Factor;
						pThisSlot->Specular[1] += Factor;
						pThisSlot->Specular[2] += Factor;
					}/*end light type etc*/

				}/*end if specular factor reasonable */
			}/*end if some specular component */

			/*/////////////////////////////////
			// Get Dot prod for diffuse shading.
			// Note that the light direction has already been normalised,
			// and that it goes TOWARD the light not away from it.
			///////////////////////////////// */
	 		LightNormalDP = DotProd(UnitNormalVec, pLight->direction);

			/*
			// Is it worthwhile doing anything?
			*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(LightNormalDP) < TWO_TO_MINUS_10_LONG )
			#else
				if ( LightNormalDP < TWO_TO_MINUS_10 )
			#endif
			{
				/* NO! do nothing */	
			}
			/*
			// else if a coloured light
			*/
			else  if(pLight->light_flags & coloured)
			{
				pThisSlot->Diffuse[0] += LightNormalDP*pLight->colour[0];
				pThisSlot->Diffuse[1] += LightNormalDP*pLight->colour[1];
				pThisSlot->Diffuse[2] += LightNormalDP*pLight->colour[2];
			}
			/*
			// Else is a grey light
			*/
			else
			{
				float Factor = LightNormalDP*pLight->colour[0];

				pThisSlot->Diffuse[0] += Factor;
				pThisSlot->Diffuse[1] += Factor;
				pThisSlot->Diffuse[2] += Factor;
			}/*if diffuse component non zero*/
		   
		}/*end for parallel lights*/



	   	/* ///////////////////////////////////////////////////////////////////
		// step through the Point lights
		/////////////////////////////////////////////////////////////////// */
		for (lightC=lightState->numOnPntLights; lightC != 0 ; lightC--, pLight++)
		{
			sgl_vector	LightDir;	
			sgl_vector	LightCol;	

			ASSERT(pLight->light_flags & light_on);
			ASSERT((pLight->light_flags & mask_light_types )== point_light_type);

			/*
			// Get the direction of light from the point to the
			// planes rep point
			*/
			VecSub(pLight->position, pPlane->repPnt, LightDir);
			VecNormalise(LightDir);

			/*
			// Is this a spot light?
			//
			// No? then leave the light colour as is.
			*/
			if(pLight->concentration == 0)
			{
				VecCopy(pLight->colour, LightCol);
			}
			/*
			// ELSE modify light intensity by scaling colour factor,
			// depending on concentration
			// NOTE pLight->direction goes TOWARD the light
			*/
			else
			{
				float   SpotDP;

				SpotDP = DotProd(pLight->direction,LightDir);
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpotDP) > TWO_TO_MINUS_10_LONG )
			#else
				if ( SpotDP > TWO_TO_MINUS_10 )
			#endif
				{
					#if USE_POW
						SpotDP = (float) pow(SpotDP,pLight->concentration);
					#else

					if(pLight->concentration == 1)
					{
						/*leave it as is*/
					}
					else
					{
						/* 
						// clip Specular factor to make sure it
						// doesnt go over 1
						*/	
						SpotDP = MIN(1.0f, SpotDP); 	
						SpotDP = (1.0f - SpotDP) * pLight->log2concentration;
						SpotDP = 1.0f - MIN(1.0f, SpotDP);
						SpotDP*=SpotDP;
					}
					#endif

					/*
					// if the light is going to be VERY Dim
					// then skip it
					*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpotDP) < TWO_TO_MINUS_10_LONG )
			#else
				if ( SpotDP < TWO_TO_MINUS_10 )
			#endif
					{
						continue;
					}
						
					LightCol[0] = pLight->colour[0] * SpotDP;
					LightCol[1] = pLight->colour[1] * SpotDP;
					LightCol[2] = pLight->colour[2] * SpotDP;
				}
				/*
				// Else the light can't be seen, so skip to the next one
				*/
				else
				{
					continue;
				}
			}/*end if spot light or not*/

			/*/////////////////////////////////
			// Determine which slot to use for the lights results.
			// Also remember if we use a shadow slot (i.e. a non zero)
			///////////////////////////////// */
			ASSERT(pLight->light_colour_slot==0||pLight->light_colour_slot==1);

			pThisSlot  = & LocalResults[pLight->light_colour_slot];


			/*/////////////////////////////////
			// Do the specular component (if any)
			///////////////////////////////// */
			if (DoSpecular)
			{
				float SpecularFactor;

				SpecularFactor = DotProd(ReflVec,LightDir);
				ASSERT(SpecularFactor < 1.01f); /*less than 1, plus slop*/

			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if ( SpecularFactor > TWO_TO_MINUS_10 )
			#endif
				{
					#if ! USE_POW
						/*
						// clip Specular factor to make sure it doesnt 
						// go over 1 
						*/	
						SpecularFactor = MIN(1.0f, SpecularFactor); 	
						SpecularFactor = (1.0f - SpecularFactor) *  pMaterial->specular_shininess_float;
						SpecularFactor = 1.0f - MIN(1.0f, SpecularFactor );
						SpecularFactor*=SpecularFactor;
					#else
						SpecularFactor=(float) pow( SpecularFactor, pMaterial->specular_shininess_float);  
					#endif 

					/*
					// After raising to the power, is the lighting
					// significant?
					*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) < TWO_TO_MINUS_10_LONG )
			#else
				if ( SpecularFactor < TWO_TO_MINUS_10 )
			#endif
					{
						/* No it isn't. Do nothing*/
					}
					/*
					//  ELSE add the specular component to the
					//  shading result, depending on whether we have
					// a grey or coloured light
					*/
					else if(pLight->light_flags & coloured)
					{
						pThisSlot->Specular[0]+= SpecularFactor*LightCol[0];
						pThisSlot->Specular[1]+= SpecularFactor*LightCol[1];
						pThisSlot->Specular[2]+= SpecularFactor*LightCol[2];
					}
					/*
					// Else is a grey light, get the intensity from the 
					// red component
					*/
					else
					{
						float Factor = SpecularFactor*LightCol[0];
						
						pThisSlot->Specular[0] += Factor;
						pThisSlot->Specular[1] += Factor;
						pThisSlot->Specular[2] += Factor;
					}/*end light type etc*/

				}/*end if specular factor reasonable */
			}/*end if some specular component */

			/*/////////////////////////////////
			// Get Dot prod for diffuse shading.
			// Note that the light direction has already been normalised,
			// and that it goes TOWARD the light not away from it.
			///////////////////////////////// */
 		   	LightNormalDP = DotProd(UnitNormalVec,LightDir);

			/*
			// Is it worthwhile doing anything?
			*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(LightNormalDP) < TWO_TO_MINUS_10_LONG )
			#else
				if ( LightNormalDP < TWO_TO_MINUS_10 )
			#endif
			{
				/* NO! do nothing */	
			}
			/*
			// else if a coloured light
			*/
			else  if(pLight->light_flags & coloured)
			{
				pThisSlot->Diffuse[0] += LightNormalDP*LightCol[0];
				pThisSlot->Diffuse[1] += LightNormalDP*LightCol[1];
				pThisSlot->Diffuse[2] += LightNormalDP*LightCol[2];
			}
			/*
			// Else is a grey light
			*/
			else
			{
				float Factor = LightNormalDP*LightCol[0];

				pThisSlot->Diffuse[0] += Factor;
				pThisSlot->Diffuse[1] += Factor;
				pThisSlot->Diffuse[2] += Factor;
			}/*if diffuse component non zero*/
		   

		}/*end for point lights*/


	   	/* ///////////////////////////////////////////////////////////////////
		// Multiply the diffuse and specular components and add them
		// to the ambient and glow, for slot 0.
		// The compiler should be able to unroll these loops trivially
		//
		// Clip the colours to lie in legal ranges
		/////////////////////////////////////////////////////////////////// */
		if(DoSpecular)
		{
#if SLOW_FCMP
				float *Dest;
				Dest = pShadingResults->slot[0].flat.baseColour;
				Dest[0] = LocalResults[0].Diffuse[0] * pMaterial->diffuse[0] +
						LocalResults[0].Specular[0]* pMaterial->specular[0] +
						AmbientGlowCol[0];
				CHOOSE_FLOAT_MIN(Dest[0], 0x3f800000L, 1.0f);

				Dest[1] = LocalResults[0].Diffuse[1] * pMaterial->diffuse[1] +
						LocalResults[0].Specular[1]* pMaterial->specular[1] +
						AmbientGlowCol[1];
				CHOOSE_FLOAT_MIN(Dest[1], 0x3f800000L, 1.0f);

				Dest[2] = LocalResults[0].Diffuse[2] * pMaterial->diffuse[2] +
						LocalResults[0].Specular[2]* pMaterial->specular[2] +
						AmbientGlowCol[2];
				CHOOSE_FLOAT_MIN(Dest[2], 0x3f800000L, 1.0f);
#else
			for(i = 0; i < 3; i++)
			{
				float temp;
				temp = 	LocalResults[0].Diffuse[i] * pMaterial->diffuse[i] +
						LocalResults[0].Specular[i]* pMaterial->specular[i] +
						AmbientGlowCol[i];
				pShadingResults->slot[0].flat.baseColour[i] = MIN(temp, 1.0f);
			}
#endif
		}
		else
		{
#if SLOW_FCMP
				float *Dest;
				Dest = pShadingResults->slot[0].flat.baseColour;
				Dest[0] = LocalResults[0].Diffuse[0] * pMaterial->diffuse[0] +
						AmbientGlowCol[0];
				CHOOSE_FLOAT_MIN(Dest[0], 0x3f800000L, 1.0f);

				Dest[1] = LocalResults[0].Diffuse[1] * pMaterial->diffuse[1] +
						AmbientGlowCol[1];
				CHOOSE_FLOAT_MIN(Dest[1], 0x3f800000L, 1.0f);

				Dest[2] = LocalResults[0].Diffuse[2] * pMaterial->diffuse[2] +
						AmbientGlowCol[2];
				CHOOSE_FLOAT_MIN(Dest[2], 0x3f800000L, 1.0f);
#else
			for(i = 0; i < 3; i++)
			{
				float temp;
				temp = 	LocalResults[0].Diffuse[i] * pMaterial->diffuse[i] +
						AmbientGlowCol[i];
				pShadingResults->slot[0].flat.baseColour[i] = MIN(temp, 1.0f);
			}
#endif
		}/*end if/else specular */


		/*
		// Do we do slot 1 then?
		*/
		if(IsShadowed)
		{
			if(DoSpecular)
			{
#if SLOW_FCMP
				float *Dest;
				Dest = pShadingResults->slot[1].flat.baseColour;
				Dest[0] = LocalResults[1].Diffuse[0] * pMaterial->diffuse[0] +
						LocalResults[1].Specular[0]* pMaterial->specular[0];
				CHOOSE_FLOAT_MIN(Dest[0], 0x3f800000L, 1.0f);

				Dest[1] = LocalResults[1].Diffuse[1] * pMaterial->diffuse[1] +
						LocalResults[1].Specular[1]* pMaterial->specular[1];
				CHOOSE_FLOAT_MIN(Dest[1], 0x3f800000L, 1.0f);

				Dest[2] = LocalResults[1].Diffuse[2] * pMaterial->diffuse[2] +
						LocalResults[1].Specular[2]* pMaterial->specular[2];
				CHOOSE_FLOAT_MIN(Dest[2], 0x3f800000L, 1.0f);
#else
				for(i = 0; i < 3; i++)
				{
					float temp;
					temp = 	LocalResults[1].Diffuse[i] * pMaterial->diffuse[i] +
							LocalResults[1].Specular[i]* pMaterial->specular[i];
					pShadingResults->slot[1].flat.baseColour[i] =MIN(temp, 1.0f);
				}
#endif
			}
			else
			{
#if SLOW_FCMP
				float *Dest;
				Dest = pShadingResults->slot[1].flat.baseColour;
				Dest[0] = LocalResults[1].Diffuse[0] * pMaterial->diffuse[0];
				CHOOSE_FLOAT_MIN(Dest[0], 0x3f800000L, 1.0f);

				Dest[1] = LocalResults[1].Diffuse[1] * pMaterial->diffuse[1];
				CHOOSE_FLOAT_MIN(Dest[1], 0x3f800000L, 1.0f);

				Dest[2] = LocalResults[1].Diffuse[2] * pMaterial->diffuse[2];
				CHOOSE_FLOAT_MIN(Dest[2], 0x3f800000L, 1.0f);
#else
				for(i = 0; i < 3; i++)
				{
					float temp;
					temp = 	LocalResults[1].Diffuse[i] * pMaterial->diffuse[i];
					pShadingResults->slot[1].flat.baseColour[i]=MIN(temp, 1.0f);
				}
#endif
			}/*end if/else specular */

		}/*end if shadowed*/

	}/*end for plane*/

	SGL_TIME_STOP(FLAT_PARAM_TIME)

}

/**************************************************************************
 * Function Name  : DoFlatTextureShadingAllGrey
 * Inputs         : Textured	  - whether batch of planes are textured
					ReversePlanes - whether batch of given planes are
									 reverse visible
   					numPlanes     - number of transformed planes
					transPlanes[] - array of pointers to transform plane struct
					pMaterial - current material	
					lightState   - current light , not use in this version !!

 * Outputs        :	ShadingResults - shading colour for batch of given planes
  
 * Input/Output	  :
						  
 * Returns        : 
 * Global Used    : 
 * Description    : Called by DoFlatTextureShading when it knows all the
 *					lights are grey.
 *					
 *				   
 **************************************************************************/
void INLINE	DoFlatTextureShadingAllGrey( sgl_bool ReversePlanes, 
						   int numPlanes, 
							 TRANSFORMED_PLANE_STRUCT * ptransPlanes[],
					   const TRANSFORM_STRUCT		  * pTransform,
					   const MATERIAL_STATE_STRUCT	  * pMaterial, 
					   const LIGHTS_STATE_STRUCT	  * lightState,
							 SHADING_RESULT_STRUCT	  * pShadingResults)
{
	const TRANSFORMED_PLANE_STRUCT *pPlane;

	int	lightC; /*to count DOWN through the lights*/
	const LIGHT_ENTRY_STRUCT * pLight;

	sgl_vector	ReflVec;
	sgl_vector UnitNormalVec;
	float	LightNormalDP;


	sgl_bool 	IsShadowed;
	sgl_bool 	DoSpecular;

	sgl_vector AmbientCol;
	/*
	// The following is initialised to 0 if there are
	// no "multi shadow" lights, else it is set to the
	// intensity of the multi shadow light
	*/
	float InitialSlot1Diffuse;


	float *pColour;
	int	  DiffuseOffset, SpecularOffset;

	SHADING_SLOT_UNION InitialSlot0;

	float greyLev;

	/*
	// Local results of specular and diffuse shading
	// for a particular plane. This is an array, one entry
	// per light slot, with diffuse and specular components.
	//
	// Note these are just intensities as we know all the lights
	// are grey.
	*/
	struct
	{
		float Diffuse;
		float Specular;
	} LocalResults[2], *pThisSlot;

	/*
	// Decide if we need to process shadows
	*/
	IsShadowed = (lightState->flags & lsf_shadows);

	/*
	// Do we need to do specular highlights at all?
	*/
	DoSpecular = pMaterial->specular_shininess_float!=0.0f;

	/*
	// Compute the ambient colour. This will stay constant for
	// this set of planes. 
	*/
  	ASSERT(lightState->flags & lsf_ambient_grey)

	/* grey value is stored in red component */
	greyLev = lightState->ambient_colour[0];

	AmbientCol[0] =	pMaterial->ambient[0] *	greyLev;
	AmbientCol[1] =	pMaterial->ambient[1] *	greyLev;
	AmbientCol[2] = pMaterial->ambient[2] * greyLev;


	/*
	// Determine if we have to add in any multi shadow lights
	//
	// THIS code currently assumes 2 light slots
	*/
	ASSERT(	RnGlobalGetCurrentLightSlot() < NUM_LIGHT_SLOTS);
	ASSERT(NUM_LIGHT_SLOTS == 2);

	if(IsShadowed && lightState->light_slots[1].multi_light)
	{
		/*
		// We have multi shadow lights.. put the colour into the
		// diffuse value
		*/
		InitialSlot1Diffuse = lightState->light_slots[1].colour[0];
	}
	else
	{
		InitialSlot1Diffuse = 0.0f;
	}


	/*
	// Set up initial slot0 colours depending on where we put the ambient and
	// glow colours
	*/
	if(pMaterial->texture_flags & affect_glow)
	{
		VecCopy(pMaterial->glow, InitialSlot0.flat.baseColour );
		VecCopy(ZERO_VEC,			 InitialSlot0.flat.highlightColour);
	}
	else
	{
		VecCopy(ZERO_VEC, 			 InitialSlot0.flat.baseColour);
		VecCopy(pMaterial->glow, InitialSlot0.flat.highlightColour);
	}

	if(pMaterial->texture_flags & affect_ambient)
	{
		VecAdd(AmbientCol, InitialSlot0.flat.baseColour, 
						   InitialSlot0.flat.baseColour);
	}
	else
	{
		VecAdd(AmbientCol, InitialSlot0.flat.highlightColour, 
						   InitialSlot0.flat.highlightColour);
	}

	/* normalise the ambient and glow colours here - this might result in
	** change of appearance compared to doing all the normalising at end
	*/
	InitialSlot0.flat.baseColour[0] = MIN(InitialSlot0.flat.baseColour[0], 1.0f );
	InitialSlot0.flat.baseColour[1] = MIN(InitialSlot0.flat.baseColour[1], 1.0f );
	InitialSlot0.flat.baseColour[2] = MIN(InitialSlot0.flat.baseColour[2], 1.0f );
	InitialSlot0.flat.highlightColour[0] = MIN(InitialSlot0.flat.highlightColour[0], 1.0f );
	InitialSlot0.flat.highlightColour[1] = MIN(InitialSlot0.flat.highlightColour[1], 1.0f );
	InitialSlot0.flat.highlightColour[2] = MIN(InitialSlot0.flat.highlightColour[2], 1.0f );

	/*
	// Decide (in advance) where to put the diffuse and specular results.
	// Each can go either in the base or highlight colour depending on the
	// texture flag settings. To save redeciding this each time, compute an
	// offset to either the base colour or the texture colour
	*/
	if(pMaterial->texture_flags & affect_diffuse)
	{
		/*
		// offset from base colour to base colour
		*/
		DiffuseOffset = 0;
	}
	else
	{
		DiffuseOffset = 3;

		/*
		// Just check that this offset is  correct
		*/
		ASSERT((pShadingResults->slot[0].flat.highlightColour -
			    pShadingResults->slot[0].flat.baseColour ) == DiffuseOffset);
	}

	if(pMaterial->texture_flags & affect_specular)
	{
		/*
		// offset from base colour to base colour
		*/
		SpecularOffset = 0;
	}
	else
	{
		/*
		// offset from base colour to highlight colour
		*/
		SpecularOffset = 3;

		/*
		// Just check that this offset is  correct
		*/
		ASSERT((pShadingResults->slot[0].flat.highlightColour -
			    pShadingResults->slot[0].flat.baseColour ) == SpecularOffset);
	}


	/*
	// Step through the planes. Increment the counter, and a pointer to the
	// shading results as well
	*/
	for(/*Nil*/; numPlanes != 0 ; numPlanes--, pShadingResults++)
	{
		/*////////////
		// get convenient access to this planes pointer.
		//////////// */
		pPlane = *ptransPlanes;

		/*
		// Move on to the next position in the array of plane pointers
		*/
		ptransPlanes ++;

		/*////////////
		// Initialise the diffuse & Specular shading results to be all zero
		// (except if we have a multi light shadow)
		//////////// */
		LocalResults[0].Diffuse =0.0f;
		LocalResults[0].Specular=0.0f;
		if(IsShadowed)
		{
			LocalResults[1].Diffuse  = InitialSlot1Diffuse;
			LocalResults[1].Specular = 0.0f;
		}


		/* ////////////
		// The normal in the transformed planes has to be normalised.
		// The way this is done depends on the type of the 
		// transformation matrix.
		//////////// */
		switch(pTransform->scale_flag)
		{
			case no_scale:
				 VecCopy(pPlane->normal, UnitNormalVec);
				 break;

			case uniform_scale:
				 UnitNormalVec[0] = pPlane->normal[0] * pTransform->rescale;
				 UnitNormalVec[1] = pPlane->normal[1] *	pTransform->rescale;
				 UnitNormalVec[2] = pPlane->normal[2] *	pTransform->rescale;

				 break;

			default:
				ASSERT(pTransform->scale_flag == arbitrary_scale)
				VecCopy(pPlane->normal, UnitNormalVec);
				VecNormalise(UnitNormalVec);
				break;

		}/*end switch normalising normal*/

		/*
		// just check vector IS normalised
		*/
		ASSERT(sfabs(VecLength(UnitNormalVec) - 1.0f) < 1.0E-3f);

		/*
		// Flip the normal for reverse planes
		*/
		if (ReversePlanes)
		{		
			VecNegate(UnitNormalVec);
		}

		/*/////////////////////////////////
		// If we have some specular component, get a unit "reflection
		// direction" vector
		///////////////////////////////// */
		if (DoSpecular)
		{
			float DPt2;
			float InvLen;

			/* 
			// calculate reflection vector.. get length of "view vector"
			// so we can "normalise" it later.
			*/
			InvLen = 1.0f / VecLength(pPlane->repPnt);

			DPt2 = 2.0f * DotProd(pPlane->repPnt,UnitNormalVec);

			ReflVec[0] = (pPlane->repPnt[0] - (DPt2* UnitNormalVec[0]))*InvLen;
			ReflVec[1] = (pPlane->repPnt[1] - (DPt2* UnitNormalVec[1]))*InvLen;
			ReflVec[2] = (pPlane->repPnt[2] - (DPt2* UnitNormalVec[2]))*InvLen;
		}

		/* //////////////////////////////////
		// get convenient access to the light entries
		////////////////////////////////// */
		pLight = lightState->light_entries;


	   	/* ///////////////////////////////////////////////////////////////////
		// step through the Parallel lights
		/////////////////////////////////////////////////////////////////// */
		for (lightC=lightState->numOnParLights; lightC != 0 ; lightC--, pLight++)
		{
			ASSERT(pLight->light_flags & light_on);
			ASSERT((pLight->light_flags & mask_light_types )== parallel_light_type);

			/*/////////////////////////////////
			// Determine which slot to use for the lights results.
			///////////////////////////////// */
			ASSERT(pLight->light_colour_slot==0||pLight->light_colour_slot==1);

			pThisSlot =   & LocalResults[pLight->light_colour_slot];

			/*/////////////////////////////////
			// Do the specular component (if any)
			///////////////////////////////// */
			if (DoSpecular)
			{
				float SpecularFactor;

				SpecularFactor = DotProd(ReflVec,pLight->direction);

				ASSERT(SpecularFactor < 1.01f); /*less than 1, plus slop*/

				/*
				// If specular is worthwhile raising to a power, do so
				// (compare against a small power of 2)
				*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpecularFactor> TWO_TO_MINUS_10)
			#endif
				{
					#if ! USE_POW
					    /* clip Specular factor to make sure it doesnt go over 1 */	
						SpecularFactor = MIN(1.0f, SpecularFactor); 	
						SpecularFactor = (1.0f - SpecularFactor) *  pMaterial->specular_shininess_float;
						SpecularFactor = 1.0f - MIN(1.0f, SpecularFactor );
						SpecularFactor*=SpecularFactor;
					#else
						SpecularFactor=(float) pow( SpecularFactor, pMaterial->specular_shininess_float);  
					#endif 

					/*
					// After raising to a power, is the value still large
					// enough to be worth considering?
					*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpecularFactor> TWO_TO_MINUS_10)
			#endif
					{
						pThisSlot->Specular += SpecularFactor * pLight->colour[0];
					}
				}/*end if specular factor reasonable */
			}/*end if some specular component */

			/*/////////////////////////////////
			// Get Dot prod for diffuse shading.
			// Note that the light direction has already been normalised,
			// and that it goes TOWARD the light not away from it.
			///////////////////////////////// */
	 		LightNormalDP = DotProd(UnitNormalVec,pLight->direction);

			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(LightNormalDP) < TWO_TO_MINUS_10_LONG )
			#else
				if (LightNormalDP< TWO_TO_MINUS_10)
			#endif
			{
				/*Light is too dim to be worth doing*/
			}
			/*
			// Else is grey, save a couple of multiplies
			*/
			else
			{	
				float result = LightNormalDP * pLight->colour[0];

				pThisSlot->Diffuse += result;

			}/*if diffuse component non zero etc*/

		}/*end for parallel lights*/



	   	/* ///////////////////////////////////////////////////////////////////
		// step through the Point lights
		/////////////////////////////////////////////////////////////////// */
		for (lightC=lightState->numOnPntLights; lightC != 0 ; lightC--, pLight++)
		{
			sgl_vector	LightDir;	
			float	LightCol;

			ASSERT(pLight->light_flags & light_on);
			ASSERT((pLight->light_flags & mask_light_types )== point_light_type);

			/*
			// Get the direction of light from the point to the
			// planes rep point
			*/
			VecSub(pLight->position, pPlane->repPnt, LightDir);
			VecNormalise(LightDir);

			/*
			// Is this a spot light?
			//
			// No? then leave the light colour as is.
			*/
			if(pLight->concentration == 0)
			{
				LightCol = pLight->colour[0];
			}
			/*
			// ELSE modify light intensity by scaling colour factor,
			// depending on concentration
			// NOTE pLight->direction goes TOWARD the light
			*/
			else
			{
				float   SpotDP;
				SpotDP = DotProd(pLight->direction,LightDir);
				/*
				// is the spot bright enough to be worth doing?
				*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpotDP) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpotDP > TWO_TO_MINUS_10)
			#endif
				{
				#if USE_POW
					SpotDP = (float) pow(SpotDP,pLight->concentration);
				#else
					if(pLight->concentration == 1)
					{
						/*leave it as is*/
					}
					else
					{
						/* 
						// clip Specular factor to make sure it
						// doesnt go over 1
						*/	
						SpotDP = MIN(1.0f, SpotDP); 	
						SpotDP = (1.0f - SpotDP)  * pLight->log2concentration;
						SpotDP = 1.0f - MIN(1.0f, SpotDP); 
						SpotDP*=SpotDP;
					}
				#endif

					/*
					// if the light is going to be VERY Dim
					// then skip it
					*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpotDP) < TWO_TO_MINUS_10_LONG )
			#else
				if (SpotDP < TWO_TO_MINUS_10)
			#endif
					{
						continue;
					}
				
					LightCol = pLight->colour[0] * SpotDP;
				}
				/*
				// Else the light can't be seen, so skip to the next one
				*/
				else
				{
					continue;
				}

			}/*end if spot light or not*/
	
			/*/////////////////////////////////
			// Determine which slot to use for the lights results.
			///////////////////////////////// */
			ASSERT(pLight->light_colour_slot==0||pLight->light_colour_slot==1);

			pThisSlot =   & LocalResults[pLight->light_colour_slot];

			/*/////////////////////////////////
			// Do the specular component (if any)
			///////////////////////////////// */
			if (DoSpecular)
			{
				float SpecularFactor;

				SpecularFactor = DotProd(ReflVec,LightDir);

				ASSERT(SpecularFactor < 1.01f); /*less than 1, plus slop*/

				/*
				// If specular is worthwhile raising to a power, do so
				// (compare against a small power of 2)
				*/
			#if SLOW_FCMP
				if (FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpecularFactor> TWO_TO_MINUS_10)
			#endif
				{
					#if ! USE_POW
					    /* clip Specular factor to make sure it doesnt go over 1 */	
						SpecularFactor = MIN(1.0f, SpecularFactor); 	
						SpecularFactor = (1.0f - SpecularFactor) * pMaterial->specular_shininess_float;
						SpecularFactor = 1.0f - MIN(1.0f, SpecularFactor );
						SpecularFactor*=SpecularFactor;
					#else
						SpecularFactor=(float) pow( SpecularFactor, pMaterial->specular_shininess_float);  
					#endif 

					/*
					// After raising to a power, is the value still large
					// enough to be worth considering?
					*/
			#if SLOW_FCMP
				if (FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpecularFactor> TWO_TO_MINUS_10)
			#endif
					{
						pThisSlot->Specular += SpecularFactor * LightCol;
					}
				}/*end if specular factor reasonable */
			}/*end if some specular component */

			/*/////////////////////////////////
			// Get Dot prod for diffuse shading.
			// Note that the light direction has already been normalised,
			// and that it goes TOWARD the light not away from it.
			///////////////////////////////// */
	 		LightNormalDP = DotProd(UnitNormalVec,LightDir);

		#if SLOW_FCMP
			if (FLOAT_TO_LONG(LightNormalDP) < TWO_TO_MINUS_10_LONG )
		#else
			if (LightNormalDP< TWO_TO_MINUS_10)
		#endif
			{
				/*Light is too dim to be worth doing*/
			}
			/*
			// Else is grey, save a couple of multiplies
			*/
			else
			{	
				pThisSlot->Diffuse += LightNormalDP * LightCol;

			}/*if diffuse component non zero etc*/

		}/*end for point lights*/

	

	   	/* ///////////////////////////////////////////////////////////////////
		// Put in each of the necessary components. Slot 0 diffuse is always
		// needed. First put in ambient and glow.
		/////////////////////////////////////////////////////////////////// */
		pShadingResults->slot[0] = InitialSlot0;
		
		/*
		// Add the diffuse component - to wherever its supposed to go!
		*/
		pColour = pShadingResults->slot[0].flat.baseColour + DiffuseOffset;

		pColour[0] += LocalResults[0].Diffuse * pMaterial->diffuse[0];
		pColour[1] += LocalResults[0].Diffuse * pMaterial->diffuse[1];
		pColour[2] += LocalResults[0].Diffuse * pMaterial->diffuse[2];

#if SLOW_FCMP
		/* decide whether we are about to change same data set in specular calculation
		** if not clip now
		*/
		if( (DiffuseOffset != SpecularOffset) || !DoSpecular )
		{
			CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
		}
#endif
		/*
		// Add the specular (if any)
		*/
		if(DoSpecular)
		{
			pColour = pShadingResults->slot[0].flat.baseColour + SpecularOffset;

			pColour[0] += LocalResults[0].Specular * pMaterial->specular[0];
			pColour[1] += LocalResults[0].Specular * pMaterial->specular[1];
			pColour[2] += LocalResults[0].Specular * pMaterial->specular[2];
#if SLOW_FCMP
			CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
#endif
		}

		/* 
		// clip the shading result colours.Dont bother doing the other
		// shadow slots unless there was a chance of some contribution
		// NOTE: Probably could do some optimisation if the highlight
		// slot isn't used, but this may not be very common.
		*/

#if ! SLOW_FCMP
	/* not sure you have to do all six tests - only test the values that have been modified */ 
	/* that is the logic applied to the SLOW_FCMP anyway */
		pShadingResults->slot[0].flat.baseColour[0] =
					MIN(pShadingResults->slot[0].flat.baseColour[0], 1.0f);
		pShadingResults->slot[0].flat.baseColour[1] =
					MIN(pShadingResults->slot[0].flat.baseColour[1], 1.0f);
		pShadingResults->slot[0].flat.baseColour[2] =
					MIN(pShadingResults->slot[0].flat.baseColour[2], 1.0f);

		pShadingResults->slot[0].flat.highlightColour[0] =
					MIN(pShadingResults->slot[0].flat.highlightColour[0], 1.0f);
		pShadingResults->slot[0].flat.highlightColour[1] =
					MIN(pShadingResults->slot[0].flat.highlightColour[1], 1.0f);
		pShadingResults->slot[0].flat.highlightColour[2] =
					MIN(pShadingResults->slot[0].flat.highlightColour[2], 1.0f);
#endif
		/*
		// Do the shadow slot (if shadows are in fashion)
		*/
		if(IsShadowed)
		{
			VecCopy(ZERO_VEC, pShadingResults->slot[1].flat.baseColour);
			VecCopy(ZERO_VEC, pShadingResults->slot[1].flat.highlightColour);

			/*
			// Add the diffuse component - to wherever its supposed to go!
			*/
			pColour = pShadingResults->slot[1].flat.baseColour + DiffuseOffset;

			pColour[0] += LocalResults[1].Diffuse * pMaterial->diffuse[0];
			pColour[1] += LocalResults[1].Diffuse * pMaterial->diffuse[1];
			pColour[2] += LocalResults[1].Diffuse * pMaterial->diffuse[2];

#if SLOW_FCMP
			/* decide whether we are about to change same data set in specular calculation
			** if not clip now
			*/
			if( (DiffuseOffset != SpecularOffset) || !DoSpecular )
			{
				CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
			}
#endif
			/*
			// Add the specular (if any)
			*/
			if(DoSpecular)
			{
				pColour = pShadingResults->slot[1].flat.baseColour +
							SpecularOffset;
				pColour[0]+= LocalResults[1].Specular * pMaterial->specular[0];
				pColour[1]+= LocalResults[1].Specular * pMaterial->specular[1];
				pColour[2]+= LocalResults[1].Specular * pMaterial->specular[2];
#if SLOW_FCMP
				CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
#endif
			}

#if ! SLOW_FCMP
			pShadingResults->slot[1].flat.baseColour[0] =
				MIN(pShadingResults->slot[1].flat.baseColour[0], 1.0f);
			pShadingResults->slot[1].flat.baseColour[1] =
				MIN(pShadingResults->slot[1].flat.baseColour[1], 1.0f);
			pShadingResults->slot[1].flat.baseColour[2] =
				MIN(pShadingResults->slot[1].flat.baseColour[2], 1.0f);

			pShadingResults->slot[1].flat.highlightColour[0] =
				MIN(pShadingResults->slot[1].flat.highlightColour[0], 1.0f);
			pShadingResults->slot[1].flat.highlightColour[1] =
				MIN(pShadingResults->slot[1].flat.highlightColour[1], 1.0f);
			pShadingResults->slot[1].flat.highlightColour[2] =
				MIN(pShadingResults->slot[1].flat.highlightColour[2], 1.0f);
#endif
		}/*end if shadowed*/
	
	}/*end for plane*/

}

/**************************************************************************
 * Function Name  : DoFlatTextureShading
 * Inputs         : Textured	  - whether batch of planes are textured
					ReversePlanes - whether batch of given planes are
									 reverse visible
   					numPlanes     - number of transformed planes
					transPlanes[] - array of pointers to transform plane struct
					pMaterial - current material	
					lightState   - current light , not use in this version !!

 * Outputs        :	ShadingResults - shading colour for batch of given planes
  
 * Input/Output	  :
						  
 * Returns        : 
 * Global Used    : 
 * Description    :
 *					
 *				   
 **************************************************************************/
void	DoFlatTextureShading( sgl_bool ReversePlanes, 
					   int numPlanes, 
							 TRANSFORMED_PLANE_STRUCT * ptransPlanes[],
					   const TRANSFORM_STRUCT		  * pTransform,
					   const MATERIAL_STATE_STRUCT	  * pMaterial, 
					   const LIGHTS_STATE_STRUCT	  * lightState,
							 SHADING_RESULT_STRUCT	  * pShadingResults)
{
	const TRANSFORMED_PLANE_STRUCT *pPlane;

	int	lightC; /*to count DOWN through the lights*/
	const LIGHT_ENTRY_STRUCT * pLight;

	sgl_vector	ReflVec;
	sgl_vector UnitNormalVec;
	float	LightNormalDP;


	sgl_bool 	IsShadowed;
	sgl_bool 	DoSpecular;

	sgl_vector AmbientCol;
	/*
	// The following is initialised to 0 if there are
	// no "multi shadow" lights, else it is set to the
	// intensity of the multi shadow light
	*/
	sgl_vector InitialSlot1Diffuse;


	float *pColour;
	int	  DiffuseOffset, SpecularOffset;

	SHADING_SLOT_UNION InitialSlot0;

	/*
	// Local results of specular and diffuse shading
	// for a particular plane. This is an array, one entry
	// per light slot, with diffuse and specular components.
	*/
	struct
	{
		sgl_vector Diffuse;
		sgl_vector Specular;

	} LocalResults[2], *pThisSlot;

	SGL_TIME_START(FLATTEXTURE_PARAM_TIME)

	/*
	// If it appears that the user has defined all her lights
	// to be grey ... take advantage of it.
	*/
	if(lightState->flags &  lsf_earl_grey)
	{
		DoFlatTextureShadingAllGrey(ReversePlanes, numPlanes, 
							 ptransPlanes, pTransform, pMaterial, 
							 lightState, pShadingResults);
	    SGL_TIME_STOP(FLATTEXTURE_PARAM_TIME)
		return;
	}


	/*
	// Decide if we need to process shadows
	*/
	IsShadowed = (lightState->flags & lsf_shadows);

	/*
	// Check that the light flag is set up correctly in the lights state
	*/
#if DEBUG
	{
		sgl_bool CheckShadowed;

		CheckShadowed = FALSE;
		pLight = lightState->light_entries;
		for (lightC=lightState->num_lights; lightC!=0; lightC --, pLight++)
		{
			/*
			// if this light is on AND casts shadows
			*/
			if ( (pLight->light_flags & light_on) &&
			   (pLight->light_colour_slot != 0) )
			{
				CheckShadowed = TRUE;
				break;
			}/*end if*/
		}/* end for*/

		if((IsShadowed) && (!CheckShadowed))
		{
			DPF((DBG_WARNING, 
				"Light State Shadow Flag and actual contents dont match up!"));
		}
		else if ((!IsShadowed) && (CheckShadowed))
		{
			DPF((DBG_WARNING, 
				"Light State Shadow Flag and actual contents dont match up!"));
		}
	}
#endif

	/*
	// Do we need to do specular highlights at all?
	*/
	DoSpecular = pMaterial->specular_shininess_float!=0.0f;

	/*
	// Compute the ambient colour. This will stay constant for
	// this set of planes. 
	*/
  	if (lightState->flags & lsf_ambient_grey)
   	{
		float greyLev;
   		/* grey value is stored in red component */
		greyLev = lightState->ambient_colour[0];

   		AmbientCol[0] =	pMaterial->ambient[0] *	greyLev;
   		AmbientCol[1] =	pMaterial->ambient[1] *	greyLev;
   		AmbientCol[2] = pMaterial->ambient[2] * greyLev;
   	}
   	else
   	{
   		AmbientCol[0] = pMaterial->ambient[0]* lightState->ambient_colour[0];
   		AmbientCol[1] = pMaterial->ambient[1]* lightState->ambient_colour[1];
   		AmbientCol[2] = pMaterial->ambient[2]* lightState->ambient_colour[2];
   	}


	/*
	// Determine if we have to add in any multi shadow lights
	//
	// THIS code currently assumes 2 light slots
	*/

	ASSERT(	RnGlobalGetCurrentLightSlot() < NUM_LIGHT_SLOTS);
	ASSERT(NUM_LIGHT_SLOTS == 2);

	if(IsShadowed && lightState->light_slots[1].multi_light)
	{
		/*
		// We have multi shadow lights.. put the colour into the
		// diffuse value
		*/
		VecCopy(lightState->light_slots[1].colour, InitialSlot1Diffuse);
	}
	else
	{
		VecCopy(ZERO_VEC , InitialSlot1Diffuse);
	}


	/*
	// Set up initial slot0 colours depending on where we put the ambient and
	// glow colours
	*/
	if(pMaterial->texture_flags & affect_glow)
	{
		VecCopy(pMaterial->glow, InitialSlot0.flat.baseColour);
		VecCopy(ZERO_VEC,			 InitialSlot0.flat.highlightColour);
	}
	else
	{
		VecCopy(ZERO_VEC, 			 InitialSlot0.flat.baseColour);
		VecCopy(pMaterial->glow, InitialSlot0.flat.highlightColour);
	}
 
		
	if(pMaterial->texture_flags & affect_ambient)
	{
		VecAdd(AmbientCol, InitialSlot0.flat.baseColour, 
						   InitialSlot0.flat.baseColour);
	}
	else
	{
		VecAdd(AmbientCol, InitialSlot0.flat.highlightColour, 
						   InitialSlot0.flat.highlightColour);
	}
	
	InitialSlot0.flat.baseColour[0] = MIN(InitialSlot0.flat.baseColour[0], 1.0f );
	InitialSlot0.flat.baseColour[1] = MIN(InitialSlot0.flat.baseColour[1], 1.0f );
	InitialSlot0.flat.baseColour[2] = MIN(InitialSlot0.flat.baseColour[2], 1.0f );
	InitialSlot0.flat.highlightColour[0] = MIN(InitialSlot0.flat.highlightColour[0], 1.0f );
	InitialSlot0.flat.highlightColour[1] = MIN(InitialSlot0.flat.highlightColour[1], 1.0f );
	InitialSlot0.flat.highlightColour[2] = MIN(InitialSlot0.flat.highlightColour[2], 1.0f );

	/*
	// Decide (in advance) where to put the diffuse and specular results.
	// Each can go either in the base or highlight colour depending on the
	// texture flag settings. To save redeciding this each time, compute an
	// offset to either the base colour or the texture colour
	*/
	if(pMaterial->texture_flags & affect_diffuse)
	{
		/*
		// offset from base colour to base colour
		*/
		DiffuseOffset = 0;
	}
	else
	{
		DiffuseOffset = 3;

		/*
		// Just check that this offset is  correct
		*/
		ASSERT((pShadingResults->slot[0].flat.highlightColour -
			    pShadingResults->slot[0].flat.baseColour ) == DiffuseOffset);
	}

	if(pMaterial->texture_flags & affect_specular)
	{
		/*
		// offset from base colour to base colour
		*/
		SpecularOffset = 0;
	}
	else
	{
		/*
		// offset from base colour to highlight colour
		*/
		SpecularOffset = 3;

		/*
		// Just check that this offset is  correct
		*/
		ASSERT((pShadingResults->slot[0].flat.highlightColour -
			    pShadingResults->slot[0].flat.baseColour ) == SpecularOffset);
	}


	/*
	// Step through the planes. Increment the counter, and a pointer to the
	// shading results as well
	*/
	for(/*Nil*/; numPlanes != 0 ; numPlanes--, pShadingResults++)
	{
		/*////////////
		// get convenient access to this planes pointer.
		//////////// */
		pPlane = *ptransPlanes;

		/*
		// Move on to the next position in the array of plane pointers
		*/
		ptransPlanes ++;

		/*////////////
		// Initialise the diffuse & Specular shading results to be all zero
		// (except if we have a multi light shadow)
		//////////// */
		VecCopy(ZERO_VEC, LocalResults[0].Diffuse);
		VecCopy(ZERO_VEC, LocalResults[0].Specular);
		if(IsShadowed)
		{
			VecCopy(InitialSlot1Diffuse, LocalResults[1].Diffuse);
			VecCopy(ZERO_VEC, LocalResults[1].Specular);
		}


		/* ////////////
		// The normal in the transformed planes has to be normalised.
		// The way this is done depends on the type of the 
		// transformation matrix.
		//////////// */
		switch(pTransform->scale_flag)
		{
			case no_scale:
				 VecCopy(pPlane->normal, UnitNormalVec);
				 break;

			case uniform_scale:
				 UnitNormalVec[0] = pPlane->normal[0] * pTransform->rescale;
				 UnitNormalVec[1] = pPlane->normal[1] *	pTransform->rescale;
				 UnitNormalVec[2] = pPlane->normal[2] *	pTransform->rescale;

				 break;

			default:
				ASSERT(pTransform->scale_flag == arbitrary_scale)
				VecCopy(pPlane->normal, UnitNormalVec);
				VecNormalise(UnitNormalVec);
				break;

		}/*end switch normalising normal*/

		/*
		// just check vector IS normalised
		*/
		ASSERT(sfabs(VecLength(UnitNormalVec) - 1.0f) < 1.0E-3f);

		/*
		// Flip the normal for reverse planes
		*/
		if (ReversePlanes)
		{		
			VecNegate(UnitNormalVec);
		}

		/*/////////////////////////////////
		// If we have some specular component, get a unit "reflection
		// direction" vector
		///////////////////////////////// */
		if (DoSpecular)
		{
			float DPt2;
			float InvLen;

			/* 
			// calculate reflection vector.. get length of "view vector"
			// so we can "normalise" it later.
			*/
			InvLen = 1.0f / VecLength(pPlane->repPnt);

			DPt2 = 2.0f * DotProd(pPlane->repPnt,UnitNormalVec);

			ReflVec[0] = (pPlane->repPnt[0] - (DPt2* UnitNormalVec[0]))*InvLen;
			ReflVec[1] = (pPlane->repPnt[1] - (DPt2* UnitNormalVec[1]))*InvLen;
			ReflVec[2] = (pPlane->repPnt[2] - (DPt2* UnitNormalVec[2]))*InvLen;
		}

		/* //////////////////////////////////
		// get convenient access to the light entries
		////////////////////////////////// */
		pLight = lightState->light_entries;


	   	/* ///////////////////////////////////////////////////////////////////
		// step through the Parallel lights
		/////////////////////////////////////////////////////////////////// */
		for (lightC=lightState->numOnParLights; lightC != 0 ; lightC--, pLight++)
		{
			ASSERT(pLight->light_flags & light_on);
			ASSERT((pLight->light_flags & mask_light_types )== parallel_light_type);

			/*/////////////////////////////////
			// Determine which slot to use for the lights results.
			///////////////////////////////// */
			ASSERT(pLight->light_colour_slot==0||pLight->light_colour_slot==1);

			pThisSlot =   & LocalResults[pLight->light_colour_slot];

			/*/////////////////////////////////
			// Do the specular component (if any)
			///////////////////////////////// */
			if (DoSpecular)
			{
				float SpecularFactor;

				SpecularFactor = DotProd(ReflVec,pLight->direction);

				ASSERT(SpecularFactor < 1.01f); /*less than 1, plus slop*/

				/*
				// If specular is worthwhile raising to a power, do so
				// (compare against a small power of 2)
				*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpecularFactor> TWO_TO_MINUS_10)
			#endif
				{
					#if ! USE_POW
					    /* clip Specular factor to make sure it doesnt go over 1 */	
						SpecularFactor = MIN(1.0f, SpecularFactor); 	
						SpecularFactor = (1.0f - SpecularFactor) * pMaterial->specular_shininess_float;
						SpecularFactor = 1.0f - MIN(1.0f, SpecularFactor );
						SpecularFactor*=SpecularFactor;
					#else
						SpecularFactor=(float) pow( SpecularFactor, pMaterial->specular_shininess_float);  
					#endif 

					/*
					// After raising to a power, is the value still large
					// enough to be worth considering?
					*/
				#if SLOW_FCMP
					if( FLOAT_TO_LONG(SpecularFactor) < TWO_TO_MINUS_10_LONG )
				#else
					if(SpecularFactor < TWO_TO_MINUS_10)
				#endif
					{
						/*too small, forget it */
					}
					/* 
					// add the specular component to the shading result 
					*/
					else if(pLight->light_flags & coloured)
					{
						pThisSlot->Specular[0]+=SpecularFactor*pLight->colour[0];
						pThisSlot->Specular[1]+=SpecularFactor*pLight->colour[1];
						pThisSlot->Specular[2]+=SpecularFactor*pLight->colour[2];
					}
					/*
					// else is grey light - the intensity is in red
					*/
					else
					{
						float result = SpecularFactor * pLight->colour[0];

						pThisSlot->Specular[0] += result;
						pThisSlot->Specular[1] += result;
						pThisSlot->Specular[2] += result;
					}
				}/*end if specular factor reasonable */
			}/*end if some specular component */

			/*/////////////////////////////////
			// Get Dot prod for diffuse shading.
			// Note that the light direction has already been normalised,
			// and that it goes TOWARD the light not away from it.
			///////////////////////////////// */
	 		LightNormalDP = DotProd(UnitNormalVec,pLight->direction);
		#if SLOW_FCMP
			if ( FLOAT_TO_LONG(LightNormalDP) < TWO_TO_MINUS_10_LONG)
		#else
			if (LightNormalDP < TWO_TO_MINUS_10)
		#endif
			{
				/*Light is too dim to be worth doing*/
			}
			/*
			// is this a coloured light
			*/
			else if(pLight->light_flags & coloured)
			{
				pThisSlot->Diffuse[0] += LightNormalDP*pLight->colour[0];
				pThisSlot->Diffuse[1] += LightNormalDP*pLight->colour[1];
				pThisSlot->Diffuse[2] += LightNormalDP*pLight->colour[2];
			}
			/*
			// Else is grey, save a couple of multiplies
			*/
			else
			{	
				float result = LightNormalDP * pLight->colour[0];

				pThisSlot->Diffuse[0] += result;
				pThisSlot->Diffuse[1] += result;
				pThisSlot->Diffuse[2] += result;

			}/*if diffuse component non zero etc*/

		}/*end for parallel lights*/



	   	/* ///////////////////////////////////////////////////////////////////
		// step through the Point lights
		/////////////////////////////////////////////////////////////////// */
		for (lightC=lightState->numOnPntLights; lightC != 0 ; lightC--, pLight++)
		{
			sgl_vector	LightDir;	
			sgl_vector	LightCol;

			ASSERT(pLight->light_flags & light_on);
			ASSERT((pLight->light_flags & mask_light_types )== point_light_type);

			/*
			// Get the direction of light from the point to the
			// planes rep point
			*/
			VecSub(pLight->position, pPlane->repPnt, LightDir);
			VecNormalise(LightDir);

			/*
			// Is this a spot light?
			//
			// No? then leave the light colour as is.
			*/
			if(pLight->concentration == 0)
			{
				VecCopy(pLight->colour, LightCol);
			}
			/*
			// ELSE modify light intensity by scaling colour factor,
			// depending on concentration
			// NOTE pLight->direction goes TOWARD the light
			*/
			else
			{
				float   SpotDP;
				SpotDP = DotProd(pLight->direction,LightDir);
				/*
				// is the spot bright enough to be worth doing?
				*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpotDP) > TWO_TO_MINUS_10_LONG )
			#else
				if (SpotDP > TWO_TO_MINUS_10)
			#endif
				{
				#if USE_POW
					SpotDP = (float) pow(SpotDP,pLight->concentration);
				#else
					if(pLight->concentration == 1)
					{
						/*leave it as is*/
					}
					else
					{
						/* 
						// clip Specular factor to make sure it
						// doesnt go over 1
						*/	
						SpotDP = MIN(1.0f, SpotDP); 	
						SpotDP = (1.0f - SpotDP)  * pLight->log2concentration;
						SpotDP = 1.0f - MIN(1.0f, SpotDP); 
						SpotDP*=SpotDP;
					}
				#endif

					/*
					// if the light is going to be VERY Dim
					// then skip it
					*/
				#if SLOW_FCMP
					if( FLOAT_TO_LONG(SpotDP) < TWO_TO_MINUS_10_LONG )
				#else
					if( SpotDP < TWO_TO_MINUS_10 )
				#endif
					{
						continue;
					}
				
					LightCol[0] = pLight->colour[0] * SpotDP;
					LightCol[1] = pLight->colour[1] * SpotDP;
					LightCol[2] = pLight->colour[2] * SpotDP;
				}
				/*
				// Else the light can't be seen, so skip to the next one
				*/
				else
				{
					continue;
				}

			}/*end if spot light or not*/
	
			/*/////////////////////////////////
			// Determine which slot to use for the lights results.
			///////////////////////////////// */
			ASSERT(pLight->light_colour_slot==0||pLight->light_colour_slot==1);

			pThisSlot =   & LocalResults[pLight->light_colour_slot];

			/*/////////////////////////////////
			// Do the specular component (if any)
			///////////////////////////////// */
			if (DoSpecular)
			{
				float SpecularFactor;

				SpecularFactor = DotProd(ReflVec,LightDir);

				ASSERT(SpecularFactor < 1.01f); /*less than 1, plus slop*/

				/*
				// If specular is worthwhile raising to a power, do so
				// (compare against a small power of 2)
				*/
			#if SLOW_FCMP
				if ( FLOAT_TO_LONG(SpecularFactor) > TWO_TO_MINUS_10_LONG)
			#else
				if (SpecularFactor> TWO_TO_MINUS_10)
			#endif
				{
					#if ! USE_POW
					/* clip Specular factor to make sure it doesnt go over 1 */	

						SpecularFactor = MIN(1.0f, SpecularFactor); 	
						SpecularFactor = (1.0f - SpecularFactor) *  pMaterial->specular_shininess_float;
						SpecularFactor = 1.0f - MIN(1.0f, SpecularFactor );
						SpecularFactor*=SpecularFactor;
					#else

						SpecularFactor=(float) pow( SpecularFactor, pMaterial->specular_shininess_float);  
					#endif 

					/*
					// After raising to a power, is the value still large
					// enough to be worth considering?
					*/
				#if SLOW_FCMP
					if( FLOAT_TO_LONG(SpecularFactor) < TWO_TO_MINUS_10_LONG )
				#else
					if(SpecularFactor < TWO_TO_MINUS_10)
				#endif
					{
						/*too small, forget it */
					}
					/* 
					// add the specular component to the shading result 
					*/
					else if(pLight->light_flags & coloured)
					{
						pThisSlot->Specular[0]+= SpecularFactor*LightCol[0];
						pThisSlot->Specular[1]+= SpecularFactor*LightCol[1];
						pThisSlot->Specular[2]+= SpecularFactor*LightCol[2];
					}
					/*
					// else is grey light - the intensity is in red
					*/
					else
					{
						float result = SpecularFactor * LightCol[0];

						pThisSlot->Specular[0] += result;
						pThisSlot->Specular[1] += result;
						pThisSlot->Specular[2] += result;
					}
				}/*end if specular factor reasonable */
			}/*end if some specular component */

			/*/////////////////////////////////
			// Get Dot prod for diffuse shading.
			// Note that the light direction has already been normalised,
			// and that it goes TOWARD the light not away from it.
			///////////////////////////////// */
	 		LightNormalDP = DotProd(UnitNormalVec,LightDir);

		#if SLOW_FCMP
			if ( FLOAT_TO_LONG(LightNormalDP) < TWO_TO_MINUS_10_LONG )
		#else
			if (LightNormalDP < TWO_TO_MINUS_10 )
		#endif
			{
				/*Light is too dim to be worth doing*/
			}
			/*
			// is this a coloured light
			*/
			else if(pLight->light_flags & coloured)
			{
				pThisSlot->Diffuse[0] += LightNormalDP*LightCol[0];
				pThisSlot->Diffuse[1] += LightNormalDP*LightCol[1];
				pThisSlot->Diffuse[2] += LightNormalDP*LightCol[2];
			}
			/*
			// Else is grey, save a couple of multiplies
			*/
			else
			{	
				float result = LightNormalDP * LightCol[0];

				pThisSlot->Diffuse[0] += result;
				pThisSlot->Diffuse[1] += result;
				pThisSlot->Diffuse[2] += result;

			}/*if diffuse component non zero etc*/

		}/*end for point lights*/



	   	/* ///////////////////////////////////////////////////////////////////
		// Put in each of the necessary components. Slot 0 diffuse is always
		// needed. First put in ambient and glow.
		/////////////////////////////////////////////////////////////////// */
		pShadingResults->slot[0] = InitialSlot0;
		
		/*
		// Add the diffuse component - to wherever its supposed to go!
		*/
		pColour = pShadingResults->slot[0].flat.baseColour + DiffuseOffset;

		pColour[0] += LocalResults[0].Diffuse[0] * pMaterial->diffuse[0];
		pColour[1] += LocalResults[0].Diffuse[1] * pMaterial->diffuse[1];
		pColour[2] += LocalResults[0].Diffuse[2] * pMaterial->diffuse[2];

#if SLOW_FCMP
			/* decide whether we are about to change same data set in specular calculation
			** if not clip now
			*/
		if( (DiffuseOffset != SpecularOffset) || !DoSpecular )
		{
			CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
		}
#endif
		/*
		// Add the specular (if any)
		*/
		if(DoSpecular)
		{
			pColour = pShadingResults->slot[0].flat.baseColour + SpecularOffset;

			pColour[0] += LocalResults[0].Specular[0] * pMaterial->specular[0];
			pColour[1] += LocalResults[0].Specular[1] * pMaterial->specular[1];
			pColour[2] += LocalResults[0].Specular[2] * pMaterial->specular[2];
#if SLOW_FCMP
			CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
			CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
#endif
		}

		/* 
		// clip the shading result colours.Dont bother doing the other
		// shadow slots unless there was a chance of some contribution
		// NOTE: Probably could do some optimisation if the highlight
		// slot isn't used, but this may not be very common.
		*/
#if ! SLOW_FCMP
		/* not sure you have to do all six tests - only test the values that have been modified */ 
		/* that is the logic applied to the SLOW_FCMP anyway */
		pShadingResults->slot[0].flat.baseColour[0] =
					MIN(pShadingResults->slot[0].flat.baseColour[0], 1.0f);
		pShadingResults->slot[0].flat.baseColour[1] =
					MIN(pShadingResults->slot[0].flat.baseColour[1], 1.0f);
		pShadingResults->slot[0].flat.baseColour[2] =
					MIN(pShadingResults->slot[0].flat.baseColour[2], 1.0f);

		pShadingResults->slot[0].flat.highlightColour[0] =
					MIN(pShadingResults->slot[0].flat.highlightColour[0], 1.0f);
		pShadingResults->slot[0].flat.highlightColour[1] =
					MIN(pShadingResults->slot[0].flat.highlightColour[1], 1.0f);
		pShadingResults->slot[0].flat.highlightColour[2] =
					MIN(pShadingResults->slot[0].flat.highlightColour[2], 1.0f);
#endif
					
		/*
		// Do the shadow slot (if shadows are in fashion)
		*/
		if(IsShadowed)
		{
			VecCopy(ZERO_VEC, pShadingResults->slot[1].flat.baseColour);
			VecCopy(ZERO_VEC, pShadingResults->slot[1].flat.highlightColour);

			/*
			// Add the diffuse component - to wherever its supposed to go!
			*/
			pColour = pShadingResults->slot[1].flat.baseColour + DiffuseOffset;

			pColour[0] += LocalResults[1].Diffuse[0] * pMaterial->diffuse[0];
			pColour[1] += LocalResults[1].Diffuse[1] * pMaterial->diffuse[1];
			pColour[2] += LocalResults[1].Diffuse[2] * pMaterial->diffuse[2];
#if SLOW_FCMP
			/* decide whether we are about to change asme data set in specular calculation
			** if not clip now
			*/
			if( (DiffuseOffset != SpecularOffset) || !DoSpecular )
			{
				CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
			}
#endif
			/*
			// Add the specular (if any)
			*/
			if(DoSpecular)
			{
				pColour = pShadingResults->slot[1].flat.baseColour
						 + SpecularOffset;

				pColour[0]+= LocalResults[1].Specular[0]*pMaterial->specular[0];
				pColour[1]+= LocalResults[1].Specular[1]*pMaterial->specular[1];
				pColour[2]+= LocalResults[1].Specular[2]*pMaterial->specular[2];
#if SLOW_FCMP
				CHOOSE_FLOAT_MIN(pColour[0], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[1], 0x3F800000L, 1.0f);
				CHOOSE_FLOAT_MIN(pColour[2], 0x3F800000L, 1.0f);
#endif
			}

#if ! SLOW_FCMP
			pShadingResults->slot[1].flat.baseColour[0] =
				MIN(pShadingResults->slot[1].flat.baseColour[0], 1.0f);
			pShadingResults->slot[1].flat.baseColour[1] =
				MIN(pShadingResults->slot[1].flat.baseColour[1], 1.0f);
			pShadingResults->slot[1].flat.baseColour[2] =
				MIN(pShadingResults->slot[1].flat.baseColour[2], 1.0f);

			pShadingResults->slot[1].flat.highlightColour[0] =
				MIN(pShadingResults->slot[1].flat.highlightColour[0], 1.0f);
			pShadingResults->slot[1].flat.highlightColour[1] =
				MIN(pShadingResults->slot[1].flat.highlightColour[1], 1.0f);
			pShadingResults->slot[1].flat.highlightColour[2] =
				MIN(pShadingResults->slot[1].flat.highlightColour[2], 1.0f);
#endif

		}/*end if shadowed*/

	}/*end for plane*/

	SGL_TIME_STOP(FLATTEXTURE_PARAM_TIME)

}


/*------------------------------ End of File --------------------------------*/
