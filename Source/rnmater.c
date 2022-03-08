/**************************************************************************
 * Name         : rnmater.c
 * Title        : SGL Render traversal
 * Author       : Stel Michael
 * Created      : 13th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Routines that handle material nodes during display list traversal.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnmater.c,v $
 * Revision 1.21  1997/06/24  18:27:48  gdc
 * got non-textrured translucency working better
 *
 * Revision 1.20  1997/06/24  17:34:04  gdc
 * enabled non-textured translucency - i.e. not masking it off anymore
 * tested with meshes on PCX2 - but not convexs, or PCX1
 *
 * Revision 1.19  1997/04/30  19:24:10  mjg
 * Replaced TEXTURE_MAP with HTEXTURE.
 *
 * Revision 1.18  1997/04/07  16:23:12  gdc
 * WrappingTransform now set via a function call rather than directly
 * tweaking the global
 *
 * Revision 1.17  1996/07/11  16:40:35  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.16  1996/06/26  10:36:00  sjf
 * Hmmm Should now correctly stop attempts to do non textured translucency.
 *
 * Revision 1.15  1996/06/20  20:35:02  jop
 * 1) (by SJF) Set up flag to correctly indicate if texture wrapping is on
 * 2) (by JOP) I think he has put in better translucency testing.
 *
 * Revision 1.14  1996/06/10  12:37:39  sjf
 * Made sure translucency was turned off when texturing was disabled.
 *
 * Revision 1.13  1996/06/07  15:58:15  sjf
 * Fixed yet another bug with texture caching and translucency.
 *
 * Revision 1.12  1996/06/06  23:04:47  sjf
 * 1) Fixed bug with translucent textures and texture caching
 * 2) Added UNTESTED material instancing.
 *
 * Revision 1.11  1996/05/30  20:32:32  sjf
 *  Added texture caching.
 *
 * Revision 1.10  1995/09/28  10:31:46  pdl
 *  RnProcessMaterialNode has changed.
 * removed the smap axis stuff.
 *
 * Revision 1.9  1995/08/27  15:27:09  sjf
 * Fixed bug which randomly decided to cause translucency to be switched on
 * 2) Also simplified the code, as the optimisations didn't seem justified
 * given the size of the code.
 *
 * Revision 1.8  1995/08/23  14:20:53  pdl
 * Altered the text_effect copying.
 *
 * Revision 1.7  1995/08/22  17:11:25  sm
 * fixed some translucency probs.
 *
 * Revision 1.6  1995/08/21  17:38:54  pdl
 * changed the wrapping processing.
 *
 * Revision 1.5  1995/08/21  11:40:43  sm
 * added stuff for translucency + fixed bugette in setting
 * of precomp structure.
 *
 * Revision 1.4  1995/08/09  17:43:32  pdl
 * added some opacity processing.
 *
 * Revision 1.3  1995/07/30  18:50:53  sjf
 * Had a quiet clash of identifiers - (with is_local) - renamed things to fix it
 *
 * Revision 1.2  1995/07/25  10:34:19  pdl
 * Modified how text_effect is processed.
 * Added Remove_texture.
 *
 * Revision 1.1  1995/06/13  16:20:26  sm
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

#include "rnmater.h"
#include "dlglobal.h"
#include "txmops.h"
#include "texapi.h"

/**************************************************************************
 * Function Name  : CopyTexWrapData
 * Inputs         : matNode
					  
 * Outputs        :  
 * Input/Output	  : stateMaterial
						  
 * Returns        : 
 * Global Used    : WrappingTransform
 * Description    : copy texture wrap data to current material state.
 *					
 *				   
 **************************************************************************/

static void	CopyTexWrapData(const MATERIAL_NODE_STRUCT 	* matNode,
						 		  MATERIAL_STATE_STRUCT	* stateMaterial,
						 		  TRANSFORM_STRUCT      * pTransformState)

{
	if ((matNode->data_flags) & mnf_has_smap)
	{
   	   	/* an s-map has been defined. Copy over all smap 
	   	   data and set smap part in texture flags */

		/* clear the smap area of the texture_flags*/
	   	stateMaterial->texture_flags&=~SMAP_BITS;
	   	stateMaterial->texture_flags|=matNode->pwrap_data->smap;
							
   		stateMaterial->su=matNode->pwrap_data->su;
   		stateMaterial->sv=matNode->pwrap_data->sv;
	 	stateMaterial->ou=matNode->pwrap_data->ou;
   		stateMaterial->ov=matNode->pwrap_data->ov;

   		stateMaterial->radius=matNode->pwrap_data->radius;

		/* make a copy of the current transformation */

		RnGlobalSetWrappingTransform (pTransformState);

   	}

   	if ((matNode->data_flags) & mnf_has_omap)
   	{
   		/* copy omap data and set data omap part in texture flags */

		/* clear the Omap area of the texture_flags*/
	   	stateMaterial->texture_flags&=~OMAP_BITS;
  		stateMaterial->texture_flags|=matNode->pwrap_data->omap;
   		stateMaterial->refrac_index=matNode->pwrap_data->refrac_index;
	}


	/*
	// Decide if texture wrapping is enabled - basically both SMap and Omap
	// Must be defined
	*/
	if((stateMaterial->texture_flags & SMAP_BITS) && 
			(stateMaterial->texture_flags & OMAP_BITS))
	{
		stateMaterial->texture_flags |= wrapping_on;		
	}
	else
	{
		stateMaterial->texture_flags &= ~wrapping_on;		
	}
}

/**************************************************************************
 * Function Name  : RnCTPreprocessMaterialNode
 * Inputs         : matNode
					pTransformState
					  
 * Outputs        :  
 * Input/Output	  : stateMaterial
						  
 * Returns        : 
 * Global Used    : 
 * Description    : This processes a material node, and returns a pointer
 *					to a cached material, if there is one, OR sets it to
 *					resets it to if texturing is turned off or a normal
 *					texture is used.
 *				   
 **************************************************************************/
void RnCTPreprocessMaterialNode(const MATERIAL_NODE_STRUCT *matNode,
								MASTER_STATE_STRUCT	*pState,
								void ** pCachedTexture)
{
	int MatFlags;
	INSTANCE_SUBS_STATE_STRUCT *pInstanceSubState= pState->pInstanceSubState;

	/*
	// get the nodes flags so we know what is being changed
	*/
	MatFlags = matNode->data_flags;

	/*
	// If this is an instance, then get the material node we should
	// be instancing, or get out.
	//
	// Note we can't instance a material node that itself is referencing
	// an instance.
	*/
	if(MatFlags & mnf_instance_ref)
	{
		int i;
		int itemType;
		int InstanceName;

		InstanceName = matNode->instanced_material_name;
		
		for(i=0; i < pInstanceSubState->num_subs; i++)
		{
			if(pInstanceSubState->nSubs[i][0] == InstanceName)
			{
				InstanceName= pInstanceSubState->nSubs[i][1];
				break;
			}
		}/*end for*/


		/*
		// Ok, check this instance.
		*/
		if(InstanceName == SGL_NULL_MATERIAL) 
		{
			/*
			// It's null, do nothing
			*/
			return;
		}
		
		/*
		// Check if we have been given a valid material name
		*/
		matNode = GetNamedItemAndType(dlUserGlobals.pNamtab, InstanceName,
															 &itemType);
		if((matNode == NULL) || (itemType != nt_material))
		{
			/*
			// No good, get out
			*/
			return;
		}


		/*
		// Ok, re-get the flags
		*/
		MatFlags = matNode->data_flags;
	}/*end if instanced*/


#if DEBUG
   	if ((MatFlags & mnf_remove_texture) && (MatFlags & mnf_has_text_map))
	{
		DPF((DBG_FATAL, "WHAT THE XXXX! Remove AND define texture ???"));
	}
#endif

   	if (MatFlags & mnf_remove_texture)
   	{
		/*
		// Ok, turn off texturing
		*/
		*pCachedTexture = NULL;
	}


	else if (MatFlags & mnf_has_text_map)
	{

		/*
		//Return whether we have a cacheable texture
		*/
		*pCachedTexture = matNode->pcached_texture;

	}/*if has texture map*/


}

/**************************************************************************
 * Function Name  : RnProcessMaterialNode
 * Inputs         : matNode
					pTransformState
					  
 * Outputs        :  
 * Input/Output	  : stateMaterial
						  
 * Returns        : 
 * Global Used    : 
 * Description    : updates the current material state
 *					
 *				   
 **************************************************************************/


void	RnProcessMaterialNode(const MATERIAL_NODE_STRUCT  * matNode,
									MATERIAL_STATE_STRUCT * stateMaterial,
									MASTER_STATE_STRUCT *pState)
{
	int MatFlags;
	int InstanceName;

	TRANSFORM_STRUCT      * pTransformState = pState->pTransformState;
	INSTANCE_SUBS_STATE_STRUCT *pInstanceSubState= pState->pInstanceSubState;

	/*
	// get the nodes flags so we know what is being changed
	*/
	MatFlags = matNode->data_flags;

	/* check each bit individually */

	/*
	// If this is an instance, then get the material node we should
	// be instancing, or get out.
	//
	// Note we can't instance a material node that itself is referencing
	// an instance.
	*/
	if(MatFlags & mnf_instance_ref)
	{
		int i;
		int itemType;

		InstanceName = matNode->instanced_material_name;
		
		for(i=0; i < pInstanceSubState->num_subs; i++)
		{
			if(pInstanceSubState->nSubs[i][0] == InstanceName)
			{
				InstanceName= pInstanceSubState->nSubs[i][1];
				break;
			}
		}/*end for*/


		/*
		// Ok, check this instance.
		*/
		if(InstanceName == SGL_NULL_MATERIAL) 
		{
			/*
			// It's null, do nothing
			*/
			return;
		}
		
		/*
		// Check if we have been given a valid material name
		*/
		matNode = GetNamedItemAndType(dlUserGlobals.pNamtab, InstanceName,
															 &itemType);
		if((matNode == NULL) || (itemType != nt_material))
		{
			/*
			// No good, get out
			*/
			return;
		}


		/*
		// Ok, re-get the flags
		*/
		MatFlags = matNode->data_flags;
	}/*end if instanced*/


	if (MatFlags & mnf_has_diffuse)
	{
		VecCopy(matNode->diffuse_colour, stateMaterial->diffuse);
	}


   	if (MatFlags & mnf_has_ambient)
	{	
		VecCopy(matNode->ambient_colour, stateMaterial->ambient);
	}


	if (MatFlags & mnf_has_specular)
   	{	
		VecCopy(matNode->specular_colour, stateMaterial->specular);

		stateMaterial->specular_shininess=matNode->specular_shininess;
		stateMaterial->specular_shininess_float=matNode->specular_shininess_float;
	}

	if (MatFlags & mnf_has_glow)
	{	
		VecCopy(matNode->glow_colour, stateMaterial->glow);
	}

#if DEBUG
   	if ((MatFlags & mnf_remove_texture) && (MatFlags & mnf_has_text_map))
	{
		DPF((DBG_FATAL, "WHAT THE XXXX! Remove AND define texture ???"));
	}
#endif

   	if (MatFlags & mnf_remove_texture)
   	{
		/*
		// remove the texture flag
		*/
		stateMaterial->texture_flags&=~(MASK_FLIP_UV | MASK_TEXTURE);
	}


	if (MatFlags & mnf_has_text_map)
	{
		/*
		// if this is a normal (i.e. non cached) texture
		*/
		if(matNode->pcached_texture == NULL)
		{

			/* copy over the texture address + control bits */

			stateMaterial->texas_precomp.TexAddress=matNode->texture_control;

			/* add the relevant parts of texture flags */
			stateMaterial->texture_flags&=~(MASK_FLIP_UV | MASK_TEXTURE);
			stateMaterial->texture_flags|=
				(matNode->text_effect)& (MASK_FLIP_UV | MASK_TEXTURE);

			/* keep the precomputed LowWord up to date */
			stateMaterial->texas_precomp.LowWord &= ~(MASK_FLIP_UV | MASK_TEXTURE);

			stateMaterial->texas_precomp.LowWord|=
				(matNode->text_effect)& (MASK_FLIP_UV | MASK_TEXTURE);


			/*
			// Check for possible translucency
			// (ie either the texture is translucent or opacity!=1)
			*/
			if ((matNode->texture_control & MASK_4444_555) ||
				(stateMaterial->opacity!=1.0f))
			{
				stateMaterial->texas_precomp.LowWord |= MASK_TRANS;
			}
			else
			{  
				stateMaterial->texas_precomp.LowWord &= ~MASK_TRANS;
			}
		}
		/*
		// Ok, we have a cached texture. Get the real texture info for it
		// (if any)
		*/
		else
		{
			HTEXTURE hTexture;
			
			hTexture = GetRealTexture(matNode->pcached_texture);

			/*
			// If texture is actually loaded (else leave as is)
			*/
			if(hTexture != NULL)
			{
				/*
				// Copy over the texture address and control bits
				*/
				stateMaterial->texas_precomp.TexAddress =
										hTexture->TSPTextureControlWord;

				/*
				// add the relevant parts of texture flags 
				*/
				stateMaterial->texture_flags&=~(MASK_FLIP_UV | MASK_TEXTURE);
				stateMaterial->texture_flags|=
					(matNode->text_effect)& (MASK_FLIP_UV | MASK_TEXTURE);

				/*
				// keep the precomputed LowWord up to date 
				*/
				stateMaterial->texas_precomp.LowWord &= 
										~(MASK_FLIP_UV | MASK_TEXTURE);

				stateMaterial->texas_precomp.LowWord|=
				  (matNode->text_effect)& (MASK_FLIP_UV | MASK_TEXTURE);


				/*
				// Check for possible translucency
				// (ie either the texture is translucent or opacity!=1)
				*/
				if ((hTexture->TSPTextureControlWord & MASK_4444_555) ||
					(stateMaterial->opacity!=1.0f))
				{
					stateMaterial->texas_precomp.LowWord |= MASK_TRANS;
				}
				else
				{
					stateMaterial->texas_precomp.LowWord &= ~MASK_TRANS;
				}

			}/*end if textured*/
			
		}/*end else is a cached texture*/
	}/*if has texture map*/


	if ((matNode->pwrap_data)!=NULL)
	{
		/* a texture wrap block has been defined */
		CopyTexWrapData(matNode,stateMaterial,pTransformState);	
	}


	if (MatFlags & mnf_has_opacity)
	{
		stateMaterial->opacity=matNode->opacity;
		stateMaterial->translucent_int=matNode->translucent_int;

		 /* clear current value */
		stateMaterial->texas_precomp.LowWord &= ~MASK_GLOBAL_TRANS;
		stateMaterial->texas_precomp.LowWord |= 
		 (((matNode->translucent_int) << SHIFT_GLOBAL_TRANS) & MASK_GLOBAL_TRANS);

		stateMaterial->texas_precomp.LowWord |= MASK_TRANS;

		if( (stateMaterial->texas_precomp.LowWord & MASK_TEXTURE) &&
			((matNode->opacity!=1.0f) || 
			 (stateMaterial->texas_precomp.TexAddress	& MASK_4444_555)))
 		{
 			stateMaterial->texas_precomp.LowWord |= MASK_TRANS;
 		}
 		else
 		{
			if(matNode->opacity==1.0f)
			{
				stateMaterial->texas_precomp.LowWord &= ~MASK_TRANS;
			}

		}

	}
	


   	if (MatFlags & mnf_has_text_effect)
	{
		/*copy over the text_effects */
   		stateMaterial->texture_flags&= ~( affect_ambient | 
										  affect_diffuse |
										  affect_specular|
										  affect_glow);

   		stateMaterial->texture_flags|=(matNode->text_effect)& 
										( affect_ambient | 
										  affect_diffuse |
										  affect_specular|
										  affect_glow);

	}


}
