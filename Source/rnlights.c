/**************************************************************************
 * Name         : rnlights.c
 * Title        : SGL Render traversal for lights
 * Author       : Stel Michael
 * Created      : 15th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Routines that handle light nodes during display list traversal.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnlights.c,v $
 * Revision 1.36  1997/06/04  15:33:02  gdc
 * stopped compiler warnings
 *
 * Revision 1.35  1997/06/04  10:27:55  gdc
 * fixed some compiler warnings
 *
 * Revision 1.34  1997/04/07  16:40:47  gdc
 * made current_light_slot a local variable which is got and set thru
 * functions now
 *
 * Revision 1.33  1997/04/03  09:57:25  rgi
 * Ported ZEUS_ARCADE support to top version
 *
 * Revision 1.32  1997/03/31  23:40:44  mlj
 * Fixes to get DSO32 driver to build
 *
 * Revision 1.31  1996/10/02  16:37:29  msh
 * CalcAverageCol forced to use single precision floating
 * point for MIDAS Arcade
 *
 * Revision 1.30  1996/07/11  16:40:28  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.29  1996/05/24  11:03:13  sm
 * nothing much!
 *
 * Revision 1.28  1996/05/23  18:03:03  sm
 * disabled a bug fix that had made things worse!!!
 *
 * Revision 1.27  1996/05/16  11:59:42  sjf
 * Fixed a few things in Process Switch Mode
 * (err Node)
 * 1) Put in test to check if any other lights cast shadows
 * 2) Ditto for all grey lights
 * 3) Hopefully fixed setting up of shadow flag in light state.
 *
 * Revision 1.26  1996/05/14  18:04:46  sm
 * made change to process_switch
 *
 * Revision 1.25  1996/04/22  18:01:38  sjf
 * Set the grey flag appropriately in the multi shadow light node
 * processing.
 *
 * Revision 1.24  1996/04/17  17:02:28  sjf
 * My favourite .... removed the ^Ms from the file.
 *
 * Revision 1.23  1996/03/11  16:13:03  jop
 * Fixed smooth shading light summing
 *
 * Revision 1.22  1996/02/02  14:48:45  jop
 * Fixed multi-colour mixing pinkness
 *
 * Revision 1.21  1995/10/27  15:43:57  jrc
 * Fixed cut'n'paste bug (point and parallel asserts wrong way round for
 * switch light nodes).
 *
 * Revision 1.20  1995/10/06  20:08:21  sjf
 * Changed the way lights are stored in the light state. They are kept with
 * all the On Parallel lights first, followed by the On Point lights, and then
 * the Off lights. This is to (hopefully) speed up the shading a little.
 *
 * Revision 1.19  1995/09/25  18:10:07  sjf
 * Added multi pseudo shadow handling.
 *
 * Revision 1.18  1995/09/19  09:46:12  jop
 * 1.16 and 1.17 merged - proper light colour averaging now enabled for all
 * (both?) light slots
 *
 * Revision 1.17  1995/09/18  10:48:55  jop
 * Does not contain changes made to 1.16 - will try to merge into 1.18
 *
 * Revision 1.16  1995/09/15  19:50:23  sjf
 * 1) Fixed bug when we ran out of shadow light slots - code was throwing away
 * the light instead of just ignoring the shadow setting
 * 2) Tidied up code to use a local pointer to the light entry.. this
 *   should also speed it up (as well as making it look neater).
 *
 * Revision 1.15  1995/09/12  10:09:20  sjf
 * Cleaned up the positioning of lights code, as it done
 * in rntrav, and this code doesn't have to worry about it.
 *
 * Revision 1.14  1995/08/28  16:55:44  sjf
 * 1) Added logConcentration to point lights
 * 2) Made some internal functions static.
 *
 * Revision 1.13  1995/08/21  21:59:06  jop
 * Wrote a new algorithm for calculating average light colours
 *
 * Revision 1.12  1995/08/21  11:51:49  jop
 * Corrected average light scaling factor calculation
 *
 * Revision 1.11  1995/08/11  16:23:21  sm
 * fixed bugette in CalcAveCol for copy over ambient col
 *
 * Revision 1.10  1995/08/04  10:36:48  sm
 * negated direction for point lights as well
 *
 * Revision 1.9  1995/08/01  15:40:30  sm
 * negated given light direction required for lighting
 *
 * Revision 1.8  1995/07/06  11:58:26  sjf
 * Put prefix on enumerated name type as a couple were VERY likely
 * to clash with variable names.
 *
 * Revision 1.7  1995/07/06  11:30:09  sjf
 * Changed quality state structure.
 *
 * Revision 1.6  1995/06/27  13:12:04  sm
 * added shadow flag
 *
 * Revision 1.5  1995/06/27  11:47:40  sm
 * changed to transform_node struct
 *
 * Revision 1.4  1995/06/26  10:33:41  sm
 * *** empty log message ***
 *
 * Revision 1.3  1995/06/20  17:12:21  sm
 * added routine to calculate average light colour
 *
 * Revision 1.2  1995/06/16  17:32:01  sm
 * added processing for light switch nodes
 *
 * Revision 1.1  1995/06/15  16:00:03  sm
 * Initial revision
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
#include "rnglobal.h"
#include "rnlights.h"



/**************************************************************************
 * Function Name  : SetParLightData
 * Inputs         : lightNode,tranNode
					  
 * Outputs        :  
 * Input/Output	  : lightState
						  
 * Returns        : a pointer to the light entry
 * Global Used    : 
 * Description    : updates the current light entry with a parallel light 
 *					IT ASSUMES THERE IS ROOM IN THE TABLE.
 *
 *					It also shuffles up all the "off" lights and point
 *					source lights to make space.
 *				   
 **************************************************************************/

static LIGHT_ENTRY_STRUCT * SetParLightData( const LIGHT_NODE_STRUCT * plightNode,
							  const TRANSFORM_STRUCT * ptransform,
							  LIGHTS_STATE_STRUCT	* plightState)
{
	LIGHT_ENTRY_STRUCT * pCurrEntry;
	int first, last;

	ASSERT((plightState->numOnParLights + plightState->numOnPntLights +
		   plightState->numOffLights) == plightState->num_lights);

	ASSERT(plightState->numOnParLights >= 0);
	ASSERT(plightState->numOnPntLights >= 0);
	ASSERT(plightState->numOffLights >= 0);

	/*
	// Make space for this new entry. We have to move the point source
	// lights and the "off" lights up one place. Since order is not
	// important, move the first off light to the position after the
	// last off light
	*/
	if(plightState->numOffLights != 0)
	{
		first = plightState->numOnParLights + plightState->numOnPntLights;
		last  = first + plightState->numOffLights;

		/*
		// Move the the first to the new last spot
		*/
		plightState->light_entries[last] = plightState->light_entries[first];
	}

	/*
	// Move the point lights up
	*/
	if(plightState->numOnPntLights != 0)
	{
		first = plightState->numOnParLights;
		last  = first +  plightState->numOnPntLights;

		/*
		// Move the the first to the new last spot
		*/
		plightState->light_entries[last] = plightState->light_entries[first];
	}

	/*
	// get a pointer to the spare spot
	*/
	pCurrEntry = & plightState->light_entries[plightState->numOnParLights];

	/*
	// Increment the number of On parallel lights
	*/
	plightState->numOnParLights ++;
	

	/* 
	// set light flags, both light and highlights are being set. 
	// Note: highlights_on is only relevent if smooth_highlights bit is set 
	// also, shadow part of flags has become redundant 
	*/
	pCurrEntry->light_flags = plightNode->flags | light_on | highlights_on; 
	pCurrEntry->light_name	= plightNode->node_hdr.n16_name;



	/* have to transform direction vector */

	TransformDirVector( ptransform, plightNode->direction, 
						pCurrEntry->direction);

	/* direction vector must be normalised */
					
	if(ptransform->scale_flag==uniform_scale)
	{
   		pCurrEntry->direction[0]*=ptransform->rescale;
	   	pCurrEntry->direction[1]*=ptransform->rescale;
	   	pCurrEntry->direction[2]*=ptransform->rescale;
	}
	else if(ptransform->scale_flag==arbitrary_scale)
	{
		VecNormalise(pCurrEntry->direction);
	} /* else if no_scale we dont have to do anything */


   	if (plightNode->flags & coloured)
   	{
		/* I am assuming that the earl grey flag is set as default */

		plightState->flags &= ~lsf_earl_grey; /* clear earl_grey flag */ 

	   	pCurrEntry->colour[0]=plightNode->colour[0];
	  	pCurrEntry->colour[1]=plightNode->colour[1];
	   	pCurrEntry->colour[2]=plightNode->colour[2];
   	}
	else
		pCurrEntry->colour[0]=plightNode->colour[0];


	/* 
	// must negate dir vector for the lighting calc 
	// We do this as it makes the shading slightly cheaper
	*/
	VecNegate(pCurrEntry->direction);

	/*
	// Return the pointer to the entry
	*/
	return (pCurrEntry);
}


/**************************************************************************
 * Function Name  : SetPointLightData
 * Inputs         : lightNode,tranNode
					  
 * Outputs        :  
 * Input/Output	  : lightState
						  
 * Returns        : Pointer to the added light entry
 * Global Used    : 
 * Description    : updates the current light entry with a point light 
 *					NOTE: it must shift the off lights up one position
 *					to make room for the new light.			
 *				   
 **************************************************************************/

static LIGHT_ENTRY_STRUCT * SetPointLightData(const LIGHT_NODE_STRUCT	*plightNode,
							   const TRANSFORM_STRUCT * ptransform,
							   LIGHTS_STATE_STRUCT	* plightState)
{
	LIGHT_ENTRY_STRUCT * pCurrEntry;
	int first, last;

	ASSERT((plightState->numOnParLights + plightState->numOnPntLights +
		   plightState->numOffLights) == plightState->num_lights);

	ASSERT(plightState->numOnParLights >= 0);
	ASSERT(plightState->numOnPntLights >= 0);
	ASSERT(plightState->numOffLights >= 0);

	/*
	// Make space for this new entry. We have to move the off
	// lights up one place. Since order is not
	// important, move the first off light to the position after the
	// last off light
	*/
	if(plightState->numOffLights != 0)
	{
		first = plightState->numOnParLights + plightState->numOnPntLights;
		last  = first + plightState->numOffLights;

		/*
		// Move the the first to the new last spot
		*/
		plightState->light_entries[last] = plightState->light_entries[first];
	}

	/*
	// get a pointer to the spare spot
	*/
	last = plightState->numOnParLights + plightState->numOnPntLights;
	pCurrEntry = & plightState->light_entries[last];

	/*
	// increment the number of point lights
	*/
	plightState->numOnPntLights ++;


	/* 
	// set light flags, both light and highlights are being set. 
	// Note: highlights_on is only relevent if smooth_highlights bit is set 
	// also, shadow part of flags has become redundant
	*/

	pCurrEntry->light_flags = plightNode->flags | light_on | highlights_on; 
	pCurrEntry->light_name	= plightNode->node_hdr.n16_name;

	plightState->flags|=lsf_has_point_light;


	/* have to transform both position and direction vector */

	TransformDirVector( ptransform, plightNode->direction, 
						pCurrEntry->direction);


	/* direction vector must be normalised */

	if(ptransform->scale_flag==uniform_scale)
	{
	   	pCurrEntry->direction[0]*= ptransform->rescale;
		pCurrEntry->direction[1]*= ptransform->rescale;
	  	pCurrEntry->direction[2]*= ptransform->rescale;
	}
	else if(ptransform->scale_flag==arbitrary_scale)
	{
	   	VecNormalise(pCurrEntry->direction);

	} /* else if no_scale we dont have to do anything */


 	TransformVector( ptransform, plightNode->position,
					 pCurrEntry->position);

   	if (plightNode->flags & coloured)
   	{
		/* I am assuming that the earl grey flag is set as default */

		plightState->flags &= ~lsf_earl_grey; /* clear earl_grey flag */ 

	   	pCurrEntry->colour[0]= plightNode->colour[0];
	  	pCurrEntry->colour[1]= plightNode->colour[1];
	   	pCurrEntry->colour[2]= plightNode->colour[2];
   	}
   	else
		pCurrEntry->colour[0]=plightNode->colour[0];


	pCurrEntry->concentration= plightNode->concentration;
   	pCurrEntry->log2concentration=plightNode->log2concentration;

	/* 
	// must negate dir vector for the lighting calc 
	// We do this as it makes the shading slightly cheaper
	*/
	VecNegate(pCurrEntry->direction);

	/*
	// Return the pointer to the entry
	*/
	return (pCurrEntry);
}





/**************************************************************************
 * Function Name  : CalcAverageCol
 * Inputs         : 
					  
 * Outputs        :  
 * Input/Output	  : lightState
						  
 * Returns        : 
 * Global Used    : 
 * Description    : Calculates average light colour for light slot 
					zero, plus the smooth shading intensity values 
 *					
 *				   
 **************************************************************************/


void	CalcAverageCol(LIGHTS_STATE_STRUCT	* lightState)
{
	int	curEntry;
	int numOnLights[NUM_LIGHT_SLOTS];
	sgl_vector	SummedCol[NUM_LIGHT_SLOTS];
	float fRescale[NUM_LIGHT_SLOTS];
	float SummedBrightness[NUM_LIGHT_SLOTS];
	int nSlot;
	float i;

	/* init the average light colours */

	numOnLights[0] = 1;

	if (lightState->flags & lsf_ambient_grey)
	{
		i = lightState->ambient_colour[0];
	
		SummedCol[0][0] = i;
		SummedCol[0][1] = i;
		SummedCol[0][2] = i;

	  #if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)
		SummedBrightness[0] = ssqrt (3.0f * (i * i));
	  #else
		SummedBrightness[0] = (float)sqrt (3.0f * (i * i));
	  #endif
	}
	else if ((lightState->ambient_colour[0] > 0.0f) ||
			 (lightState->ambient_colour[1] > 0.0f) ||
			 (lightState->ambient_colour[2] > 0.0f))
	{
		SummedCol[0][0] = lightState->ambient_colour[0];
		SummedCol[0][1] = lightState->ambient_colour[1];
		SummedCol[0][2] = lightState->ambient_colour[2];

		SummedBrightness[0] = VecLength (lightState->ambient_colour);
	}
	else
	{
		numOnLights[0] = 0;

		SummedCol[0][0] = 0.0f;
		SummedCol[0][1] = 0.0f;
		SummedCol[0][2] = 0.0f;

		SummedBrightness[0] = 0.0f;
	}

	for (nSlot = 1; nSlot < NUM_LIGHT_SLOTS; ++nSlot)
	{
		SummedCol[nSlot][0] = 0.0f;
		SummedCol[nSlot][1] = 0.0f;
		SummedCol[nSlot][2] = 0.0f;
		
		SummedBrightness[nSlot] = 0.0f;

		numOnLights[nSlot] = 0;
	}

	/* step through all the lights currently in the light entry table */

	for (curEntry = 0; curEntry < (lightState->num_lights); curEntry++)
	{
		/* only add components of lights that are switched on */

		if (lightState->light_entries[curEntry].light_flags & light_on)
		{
			nSlot = lightState->light_entries[curEntry].light_colour_slot;

			ASSERT (nSlot >= 0);			
			ASSERT (nSlot <= NUM_LIGHT_SLOTS);			

			if (lightState->light_entries[curEntry].light_flags & coloured)
			{
				SummedCol[nSlot][0] += lightState->light_entries[curEntry].colour[0];
				SummedCol[nSlot][1] += lightState->light_entries[curEntry].colour[1];
				SummedCol[nSlot][2] += lightState->light_entries[curEntry].colour[2];

				SummedBrightness[nSlot] += VecLength (lightState->light_entries[curEntry].colour);
			}
			else
			{
				i = lightState->light_entries[curEntry].colour[0];

				SummedCol[nSlot][0] += i;
				SummedCol[nSlot][1] += i;
				SummedCol[nSlot][2] += i;

			  #if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)
				SummedBrightness[nSlot] += ssqrt (3.0f * (i * i));
			  #else
				SummedBrightness[nSlot] += (float)sqrt (3.0f * (i * i));
			  #endif
			}

			numOnLights[nSlot]++;
		}
	}

	/* calculate the average colours */
	
	for (nSlot = 0; nSlot < NUM_LIGHT_SLOTS; ++nSlot)
	{
		if (numOnLights[nSlot])
		{
			SummedBrightness[nSlot] = 1.0f / SummedBrightness[nSlot];
			
			#if 1

			if (SummedCol[nSlot][0] > 1.0f || SummedCol[nSlot][1] > 1.0f || SummedCol[nSlot][2] > 1.0f)
			{
				float fMax = SummedCol[nSlot][0];
				float f1overMax;

				CHOOSE_MAX (fMax, SummedCol[nSlot][1]);
				CHOOSE_MAX (fMax, SummedCol[nSlot][2]);

				fRescale[nSlot] = fMax;
				f1overMax = 1.0f / fMax;

				SummedCol[nSlot][0] *= f1overMax; 
				SummedCol[nSlot][1] *= f1overMax; 
				SummedCol[nSlot][2] *= f1overMax; 
			}
			else
			{
				fRescale[nSlot] = 1.0f;
			}

			#else

			fRescale[nSlot] = 1.0f;

			#endif

			lightState->light_slots[nSlot].colour[0] = SummedCol[nSlot][0];
			lightState->light_slots[nSlot].colour[1] = SummedCol[nSlot][1];
			lightState->light_slots[nSlot].colour[2] = SummedCol[nSlot][2];

			if (nSlot == 0)	
			{
				float fAmbientVecLength;

				if (lightState->flags & lsf_ambient_grey)
				{
				  #if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)
					fAmbientVecLength = ssqrt (3.0f * (lightState->ambient_colour[0] * lightState->ambient_colour[0]));
				  #else
					fAmbientVecLength = (float)sqrt (3.0f * (lightState->ambient_colour[0] * lightState->ambient_colour[0]));
				  #endif
				}
				else
				{
					fAmbientVecLength = VecLength (lightState->ambient_colour);
				}

				lightState->ambient_smooth_intensity = fAmbientVecLength * SummedBrightness[nSlot] * fRescale[nSlot];
			}
		}
	}

	/* find largest dot product */

	for (curEntry = 0; curEntry < lightState->num_lights; curEntry++)
	{
		/* only interested in lights that are switched on */

		if (lightState->light_entries[curEntry].light_flags & light_on)
		{
			float fVecLength;

			nSlot = lightState->light_entries[curEntry].light_colour_slot;
	
			if (lightState->light_entries[curEntry].light_flags & coloured)
			{
				fVecLength = VecLength (lightState->light_entries[curEntry].colour);
			}
			else
			{
				fVecLength = (float)sqrt (3.0f * (lightState->light_entries[curEntry].colour[0] * 
										lightState->light_entries[curEntry].colour[0]));
			}

			lightState->light_entries[curEntry].smooth_intensity = fVecLength * SummedBrightness[nSlot] * fRescale[nSlot];   
		}
	}

	/* clear dirty flag bit. Is now nice and clean */
   	lightState->flags &= ~lsf_dirty_smooth;
}	



/**************************************************************************
 * Function Name  : MoveToONSection
 * Inputs         : 
					  
 * Outputs        :  
 * Input/Output	  : lightState
 *
 * Returns        : pointer to the new position of the light entry
 * Global Used    : 
 * Description    : Moves a light that was switched from off to ON into the
 *				   "on" section which is either for parallel or point 
 *				   source lights.
 *					
 **************************************************************************/
static LIGHT_ENTRY_STRUCT * MoveToONSection(LIGHT_ENTRY_STRUCT *pOriginalPos,	
											LIGHTS_STATE_STRUCT	 * plightState)
{


	LIGHT_ENTRY_STRUCT TempEnt;

	LIGHT_ENTRY_STRUCT *pFirstOff, *pFirstPoint;
	sgl_bool IsPoint;

	ASSERT((plightState->numOnParLights + plightState->numOnPntLights +
		   plightState->numOffLights) == plightState->num_lights);

	ASSERT(plightState->numOnParLights >= 0);
	ASSERT(plightState->numOnPntLights >= 0);
	ASSERT(plightState->numOffLights >= 1);

	/*
	// Make a copy of the data we are moving, because we will
	// write over this spot.
	*/
	TempEnt = *pOriginalPos;

	IsPoint =(TempEnt.light_flags & mask_light_types )== point_light_type;
	/*
	// If there is more than one off light, then we need to make
	// as space at the start of the off lights .. move the first one
	// to the position occupied by the one we are moving out.
	// (if there is only one, then we destroying it anyway)
	*/
	pFirstOff = &plightState->light_entries[plightState->numOnParLights + 
											plightState->numOnPntLights];
	if(plightState->numOffLights > 1)
	{
		*pOriginalPos = *pFirstOff;
	}
	

	/*
	// If the light being switched is a point source light, move it
	// to the spot that WAS occupied by the first OFF light.
	*/
	if(IsPoint)
	{
		*pFirstOff = TempEnt;

		/*
		//Save where we put the thing
		*/
		pOriginalPos = pFirstOff;

		/*
		// Increment the number of On point lights. We'll decrement the
		// number of off lights shortly
		*/
		plightState->numOnPntLights ++;
	}
	/*
	// else it IS a parallel light. We need to move the FIRST point light
	// into the spot that was occupied by the First OFF light.
	// The FIRST off light spot is where we then put the switched ON light
	*/
	else
	{
		pFirstPoint = &plightState->light_entries[plightState->numOnParLights];
		
		/*
		// Move the first Point light into the spot vacated by the first
		// off light
		*/
		*pFirstOff = *pFirstPoint;

		/*
		// Move the switched on Parallel light into the spot vacated by
		// the first Point light
		*/
		*pFirstPoint = TempEnt;

		/*
		//Save where we put the thing
		*/
		pOriginalPos = pFirstPoint;

		/*
		// Increment the number of On parallel lights. We'll decrement the
		// number of off lights shortly
		*/
		plightState->numOnParLights ++;
	}

	/*
	// decrement the number of Off lights
	*/
	plightState->numOffLights --;


	/*
	// return the pointer to where the light ended up
	*/
	return(pOriginalPos);


}

/**************************************************************************
 * Function Name  : MoveToOFFSection
 * Inputs         : 
					  
 * Outputs        :  
 * Input/Output	  : lightState
 *
 * Returns        : pointer to the new position of the light entry
 * Global Used    : 
 * Description    : Moves a light that was switched from on to off into the 
 *					"OFF" section
 *					for either parallel or point source lights.
 *					
 **************************************************************************/
static LIGHT_ENTRY_STRUCT * MoveToOFFSection(LIGHT_ENTRY_STRUCT *pOriginalPos,	
											LIGHTS_STATE_STRUCT	 * plightState)
{


	LIGHT_ENTRY_STRUCT TempEnt;

	LIGHT_ENTRY_STRUCT *pLastPar, *pLastPoint;
	sgl_bool IsPoint;



	ASSERT((plightState->numOnParLights + plightState->numOnPntLights +
		   plightState->numOffLights) == plightState->num_lights);

	/*
	// Make a copy of the data we are moving, because we will
	// write over this spot
	*/
	TempEnt = *pOriginalPos;

	IsPoint =(TempEnt.light_flags & mask_light_types )== point_light_type;

#if DEBUG
	if(IsPoint)
	{
		ASSERT(plightState->numOnPntLights >= 1);
		ASSERT(plightState->numOnParLights >= 0);
	}
	else
	{
		ASSERT(plightState->numOnPntLights >= 0);
		ASSERT(plightState->numOnParLights >= 1);
	}
#endif

	ASSERT(plightState->numOffLights >= 0);





	/*
	// get a pointer to where the data will end up. This is currently
	// occupied by the last point source light
	*/
	pLastPoint = &plightState->light_entries[plightState->numOnParLights + 
											plightState->numOnPntLights - 1];


	/*
	// If this is a point source light, then move the last point source light
	// to the vacated position.
	*/
	if(IsPoint)
	{
		*pOriginalPos = *pLastPoint;

		plightState->numOnPntLights --;

	}
	/*
	// Else it is a parallel light. We need to move the last parallel light
	// into this place, then move the last point light into that.
	*/
	else
	{
		pLastPar = &plightState->light_entries[plightState->numOnParLights -1];

		*pOriginalPos = *pLastPar;

		*pLastPar = *pLastPoint;

		plightState->numOnParLights --;
	}

	/*
	// Finally save the switched off light in its new resting place
	*/
	*pLastPoint = TempEnt;

	plightState->numOffLights++;

	/*
	// return the pointer to where the light ended up
	*/
	return(pLastPoint);

}


/**************************************************************************
 * Function Name  : RnProcessLightSwitchNode
 * Inputs         : lightNode
					  
 * Outputs        :  
 * Input/Output	  : lightState
 *
 * Returns        : 
 * Global Used    : 
 * Description    : updates the current light state
 *					NOTE : if a shadow light is turned off or has its shadowing 
 *					capabilities disabled shadow flag should be recalculated
 *					- THIS IS NOT IMPLEMENTED !!!!
 *						   also for point light flags etc.. 
 *					
 **************************************************************************/


void RnProcessLightSwitchNode( const LIGHT_SWITCH_NODE_STRUCT	* switchNode,	
							   LIGHTS_STATE_STRUCT	 * plightState)
{
	int curIndex, i;
	int switchOn,shadowOn;
	int light_switched;
	LIGHT_ENTRY_STRUCT * pCurrEntry, *pLight2;

	/* first have to locate light in entries list */
	pCurrEntry = &plightState->light_entries[0];
	for(curIndex=0;  curIndex < plightState->num_lights; curIndex++,pCurrEntry++)
	{
		if (switchNode->light_name == pCurrEntry->light_name)
		{
			break;
		}
	}

	/* 
	// if named light has not been found in list we cant do anything. 
	// This would happen for example if somebody tried to switch a light
	// that wasn't in scope !! 
	*/
    if(curIndex != plightState->num_lights)
	{
		/* 
		// we are interested in a change of state
		//
		// Note using the ! because the masked nums are different 
		*/
		switchOn	   = switchNode->light_switches & switch_light_on;
		light_switched = (!switchOn) != !(pCurrEntry->light_flags & light_on);

		if (light_switched)
		{
			int some_coloured;

			if (switchOn)
			{
				pCurrEntry =MoveToONSection(pCurrEntry, plightState);
				pCurrEntry->light_flags |= light_on;
			}
			else
			{
				pCurrEntry =MoveToOFFSection(pCurrEntry, plightState);
				pCurrEntry->light_flags &= ~light_on;
			}


			/*
			// Check if all the On lights are all grey
			*/
			some_coloured = 0;
			pLight2	 = &plightState->light_entries[0];

			i = (plightState->numOnParLights + plightState->numOnParLights);

			for(/*Nil*/;   i != 0; i--, pLight2++)
			{
				some_coloured |= pLight2->light_flags;
			}/*end for*/


			if(some_coloured & coloured)
			{
				plightState->flags &= ~ lsf_earl_grey;
			}
			else
			{
				plightState->flags |= lsf_earl_grey;
			}


			/* 
			// the lights state has changed therefore IF it belongs to light 
			// slot 0, the average colour  has to be recalculated 
			*/  
			if (pCurrEntry->light_colour_slot==0)
				plightState->flags|=lsf_dirty_smooth;
		}

		

		/*
		// Decide if shadows are on.
		// Note, if the light is turned off, then the shadows are also effectively
		// turned off.
		*/
		shadowOn=(switchNode->light_switches & switch_shadows_on) &&
				 (pCurrEntry->light_flags & light_on);
		/* 
		// if requested to turn on shadows and shadows are off, or vice versa 
		*/
		if ( (!shadowOn) != ( pCurrEntry->shad_volume==0  ) )
		{
			if (shadowOn)
			{
				/*	
				// note that if the assigned shadow is zero (not a shadow light)
				// shadowing stays disabled 
				*/
				pCurrEntry->shad_volume=
						pCurrEntry->assigned_shad_volume;
				
				if (pCurrEntry->shad_volume!=0)
				{
	  				plightState->flags |= lsf_shadows;
				}
			}

			/*
			// Else we are turning off the shadows
			*/
			else
			{
				int casts_shadows;


				/*
				// turn off shadows for this light
				*/
				pCurrEntry->shad_volume=0;


				/*
				// Check if any of the on lights are casting shadows
				*/
				casts_shadows = 0;
				pLight2	 = &plightState->light_entries[0];

				i = (plightState->numOnParLights + plightState->numOnParLights);

				for(/*Nil*/;   i != 0; i--, pLight2++)
				{
					casts_shadows |= pLight2->shad_volume;
				}/*end for*/

				if(casts_shadows)
				{
					plightState->flags |= lsf_shadows;
				}
				
/* This bug fix seem to screw things up!!!!!! */		
				
#if 0
				else
				{
					plightState->flags &= ~lsf_shadows;
				}
#endif
			}

		} /*end if shadow on*/

		/* 
		// The switch_smooth_highlights flag is being ignored here since there 
		// is no support in texas !!!!!!!!! 
		*/

	}/*end if light found */
}







/**************************************************************************
 * Function Name  : RnProcessLightNode
 * Inputs         : lightNode, tranNode
					  
 * Outputs        :  
 * Input/Output	  : lightState
						  
 * Returns        : 
 * Global Used    : 
 * Description    : updates the current light state
 *					
 *				   
 **************************************************************************/


void RnProcessLightNode( const LIGHT_NODE_STRUCT	* lightNode, 
						 const TRANSFORM_STRUCT * tranNode, 
						 LIGHTS_STATE_STRUCT	* lightState)
{
	LIGHT_ENTRY_STRUCT * pCurrEntry;
	int LightType;
	int current_light_slot;

	current_light_slot = RnGlobalGetCurrentLightSlot();

	LightType = lightNode->flags & mask_light_types;
	switch(LightType)
	{
		/* ///////////////
		// AMBIENT LIGHT TYPE
		/////////////// */
		case ambient_light_type:
				
			if (lightNode->flags & ambient_relative)
			{
				if (lightNode->flags & coloured)
				{
					/* 
					// Assume that as default, ambient is grey and 
					// any spec of ambient that is colour will set ambient
					// grey flag to false  
					//
					// clear grey flag and  earl_grey flag
					*/
					lightState->flags &= ~lsf_ambient_grey;
					lightState->flags &= ~lsf_earl_grey;

					lightState->ambient_colour[0]*=lightNode->colour[0];
					lightState->ambient_colour[1]*=lightNode->colour[1];
					lightState->ambient_colour[2]*=lightNode->colour[2];
				}
				else
					lightState->ambient_colour[0]*=lightNode->colour[0];
			}	
			else
			{
				if (lightNode->flags & coloured)
				{
					/*
					// clear grey flag and  earl_grey flag
					*/
					lightState->flags &= ~lsf_ambient_grey;
					lightState->flags &= ~lsf_earl_grey;

					lightState->ambient_colour[0]=lightNode->colour[0];
					lightState->ambient_colour[1]=lightNode->colour[1];
					lightState->ambient_colour[2]=lightNode->colour[2];
				}
				else
				{
					lightState->flags |= lsf_ambient_grey; /* set flag bit */
					lightState->ambient_colour[0]=lightNode->colour[0];
				}
			}				

			/* 
			// ambient colour has changed therefore light slot 0 colour
			// has to be recalculated 
			*/  
			lightState->flags|=lsf_dirty_smooth;
			break;

		/* ///////////////
		// Parallel and Point LIGHT TYPEs
		/////////////// */
		case parallel_light_type:
		case point_light_type:
		{
			/* check to see if there is any space available in light entries */
			if (lightState->num_lights < SGL_MAX_ACTIVE_LIGHTS)
				{
					sgl_bool castShadows;

					/*
					// Set up the parallel/point data, and get a pointer to
					// the added light entry.
					*/
					if(LightType == parallel_light_type)
					{
						pCurrEntry = 
						      SetParLightData(lightNode,tranNode,lightState);
					}
   					else
					{
						ASSERT(LightType == point_light_type);
   						pCurrEntry = 
							  SetPointLightData(lightNode,tranNode,lightState);
					}

					/*
					// Decide if we should treat this light as casting shadows
					// or not
					// The cast shadows flag must be set - we can cast shadows
					// if in multi shadow mode, OR (if in single shadow mode) 
					// there are available light slots
					//
					// If we can't cast shadows, treat it as an ordinary light.
					*/
					castShadows = (lightNode->flags & casts_shadows) && 
					   ((lightNode->flags & lsf_multi_shadows) ||
						(current_light_slot < (NUM_LIGHT_SLOTS-1)));

					/* 
					// is it a shadowing light 
					*/
					if (castShadows)
					{
						/*
						// If MULTI SHADOW mode
						*/
						if((lightState->flags & lsf_multi_shadows))
						{
							/* add light to current slot */
							
	   					   	pCurrEntry->shad_volume=current_light_slot;       
	   					   	pCurrEntry->assigned_shad_volume=current_light_slot;

							/* 
							// for multi shadow lights light slot 0 is used 
							// to store shading info 
							*/
   					   		pCurrEntry->light_colour_slot=0;  

							ASSERT(current_light_slot <= SGL_MAX_SHADOW_LIGHTS);
							lightState->light_slots[current_light_slot].multi_light
											=TRUE;

							/*
							// Since this is a multi shadow light, we need to mark
							// that the smooth shading colour is out of date as
							// we mix multi shadow light colours into slot 0.
							*/
							lightState->flags|=lsf_dirty_smooth;

				   			lightState->num_lights++;
							lightState->flags |= lsf_shadows;

						}
						/*
						// ELSE single shadow mode.
						*/
						else
						{
							/* need a new light slot */
   			   				ASSERT(current_light_slot < (NUM_LIGHT_SLOTS-1) );

						 	current_light_slot++;

						   	pCurrEntry->shad_volume=current_light_slot;       
						   	pCurrEntry->assigned_shad_volume=current_light_slot;

							/* for the current  system this will be 1 */ 
				   			pCurrEntry->light_colour_slot=current_light_slot;

							/* 
							// since this is only a single shadow light, multi
							// light flag for this slot is set to FALSE 
							// NOTE: we dont need to store anything is the slots
							// colour 
							*/
							lightState->light_slots[current_light_slot].multi_light
								=FALSE;

				   			lightState->num_lights++;
							lightState->flags |= lsf_shadows;
  		   				    lightState->flags |= lsf_dirty_smooth;

						} /* end else single light shadow */

					}
					/*
					// ELSE NOT casting shadows
					*/
					else
					{
						/* 
						// it is a non shadowing light, and is assigned to slot 0
						*/
						/* 
						// I assume these are probably redundant as they could
						//  be the default !!!!!!!!   (Stel)
						*/
						pCurrEntry->shad_volume=0;       
						pCurrEntry->assigned_shad_volume=0;

			   			pCurrEntry->light_colour_slot=0;  

	  		   			lightState->num_lights++;

	   				    lightState->flags |= lsf_dirty_smooth;

					} /*end if else casting shadows*/

				}/*END IF not at max acive lights*/

				break;
		}

   		default:
			ASSERT(FALSE);
			break;

	} /*end switch LightType*/

	/* update the current light slot */
	RnGlobalSetCurrentLightSlot(current_light_slot);
}






/**************************************************************************
 * Function Name  : RnProcessMultiShadowNode
 * Inputs         : pNode
					  
 * Outputs        :  
 * Input/Output	  : lightState
						  
 * Returns        : 
 * Global Used    : the light slot count
 * Description    : updates the current light state withe the multi shadow
 *					flag
 *					
 *				   
 **************************************************************************/
void	RnProcessMultiShadowNode(const MULTI_SHADOW_NODE_STRUCT * pNode,  
									LIGHTS_STATE_STRUCT * plightState)
{

	int current_light_slot;

	current_light_slot = RnGlobalGetCurrentLightSlot();

	/*
	// See if we are multi switching shadows on or off
	*/
	if(pNode->Enable)
	{
		/*
		// If there is an available shadow slot
		*/
		if(current_light_slot < (NUM_LIGHT_SLOTS-1))
		{
			/*
			// Increment the light slot
			*/
		 	current_light_slot++;
			
			/*
			// Store the colour for the multi shadows
			// and the fact that this lot is being used for multi
			// shadows.
			*/
			VecCopy(pNode->colour,
				plightState->light_slots[current_light_slot].colour);
			plightState->light_slots[current_light_slot].multi_light=TRUE;

			/*
			//If this colour is not grey, then clear the all grey flag in
			//the light state
			*/
			if(!(pNode->colour[0] == pNode->colour[1] == pNode->colour[2]))
			{
				plightState->flags &=  ~lsf_earl_grey;
			}

			/*
			// Set the flag wo we know that lights (with shadow) will
			// be assigned to the multi shadow group
			*/
			plightState->flags |= lsf_multi_shadows | lsf_shadows;
			
		}
		/*
		// Else multi shadowing is OUT (as is any new shadow lights)
		// ... make sure it is turned off. (Note that if it was on,
		// then we are effectively
		// asking for a new independant group of lights
		*/
		else
		{
			plightState->flags &= ~lsf_multi_shadows;
		}/*end if/else available light slot*/
	}
	/*
	// Else we are switching off multi shadows. If it wasn't
	// on then do nothing.
	*/
	else if(plightState->flags & lsf_multi_shadows)
	{
		/*
		// We have to disable the multi shadow flag
		*/
		plightState->flags &= ~lsf_multi_shadows;
		
	}/*end if*/

	/* update the current light slot */
	RnGlobalSetCurrentLightSlot(current_light_slot);
}


/*
// *************  END OF FILE ********************
*/


