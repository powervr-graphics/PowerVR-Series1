/******************************************************************************
 * Name         : rnqualit.c
 * Title        : SGL render traversal for quality nodes.
 * Author       : John Catchpole
 * Created      : 10/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnqualit.c,v $
 * Revision 1.7  1997/09/11  10:35:27  erf
 * Added dithering case condition as a quality setting.
 *
 * Revision 1.6  1997/04/09  17:00:29  gdc
 * removed reference to global projection_mat
 *
 * Revision 1.5  1997/03/25  10:28:30  erf
 * Added texture filtering quality case.
 *
 * Revision 1.4  1995/11/02  19:50:33  sjf
 * Added fog as a quality setting
 *
 * Revision 1.3  1995/09/25  18:12:16  sjf
 * Added quality node processing.
 *
 * Revision 1.2  1995/07/10  14:11:36  jrc
 * Fixed function name.
 *
 * Revision 1.1  1995/07/10  14:04:23  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_RN

#include "sgl_defs.h"
#include "dlnodes.h"
#include "rnstate.h"
#include "rnglobal.h"


#define SET_OR_CLEAR_FLAGS(VAL, SET, FLAGS) \
		 {if(SET) VAL |=  (FLAGS); else VAL &= ~(FLAGS);}


/******************************************************************************
 * Function Name: RnProcessQualityNode
 *
 * Inputs       : pNode,pQualityState
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Select the required level of detail object list to render.
 *				  Assumes that pNode points to a valid level of detail display
 *				  list node.
 *****************************************************************************/
void RnProcessQualityNode(	const QUALITY_NODE_STRUCT *pNode,
							QUALITY_STATE_STRUCT *pQualityState)
{

	/*
	// Decide what is being changed
	*/
	switch(pNode->WhatsSet)
	{
		/*//////////////
		// SMOOTH SHADING
		////////////// */
		case QFE_SMOOTH_SHAD:
		{
			SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   pNode->Enable, 
							   qf_smooth_shading);
			break;
		}

		/*//////////////
		// TEXTURING
		////////////// */
		case QFE_TEXTURE:
		{
			SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   pNode->Enable, 
							   qf_textures);

			break;
		}

		/*//////////////
		// Fogging
		////////////// */
		case QFE_FOG:
		{
			PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

			/*
			// If fogging IS in action, then allow the user to
			// change the value, else ignore it
			*/
			if(pProjMat->FogOn)
			{
				SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   pNode->Enable, 
							   qf_fog);
			}

			break;
		}

		/*//////////////
		// COLLISION DETECTION
		////////////// */
		case QFE_COLLISIONS:
		{
			/*
			// If disabled, disable all collision detection
			*/
			if(!pNode->Enable)
			{
				SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   FALSE, 
							   qf_full_collision | qf_onscreen_collision);

			}
			/*
			// Else if only onscreen collision is allowed
			*/
			else if(!pNode->Enable2)
			{
				SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   TRUE, 
							   qf_onscreen_collision);

				SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   FALSE, 
							   qf_full_collision);

			}
			/*
			// Else all collisions allowed
			*/
			else
			{
				SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   TRUE, 
							   qf_full_collision);

			}
			
			break;
		}

		/*////////////////////////////
		// Texture filtering setting
		/////////////////////////// */
		case QFE_TEXTURE_FILTERING:
		{
			/* Always set to TRUE since we want the settings
			 * to become active.
			 */
			SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   TRUE,
							   qf_texture_filtering);

			/* Set the bilinear filtering option.
			 */
			pQualityState->eFilterType = pNode->eFilterType;
			break;
		}

		/*//////////////
		// DITHERING
		////////////// */
		case QFE_DITHERING:
		{
			SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   pNode->Enable, 
							   qf_dithering);
			break;
		}

		/*//////////////
		// SHADOWS
		////////////// */
		default:
		{
			ASSERT(pNode->WhatsSet == QFE_SHADOWS);

			SET_OR_CLEAR_FLAGS(pQualityState->flags,
							   pNode->Enable, 
							   qf_cast_shadows);
			break;
		}

	}/*end switch*/
}


/*------------------------------- End of File -------------------------------*/
