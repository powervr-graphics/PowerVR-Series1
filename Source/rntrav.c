/**************************************************************************
 * Name         : rntrav.c
 * Title        : SGL Render traversal
 * Author       : Simon Fenney
 * Created      : 7th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Routine(s) that recursively traverse(s) a display
 *				  list during rendering.
 *
 *				  ALSO, because they are very similar, it also processes
 *				  instance and instance substitution nodes.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rntrav.c,v $
 * Revision 1.67  1997/07/02  13:47:41  mjg
 * Removed sweary words!
 *
 * Revision 1.66  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.65  1997/04/30  19:21:18  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.64  1997/04/29  15:07:23  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.63  1997/04/21  15:29:39  mjg
 * Removed a compiler warning.
 *
 * Revision 1.62  1997/04/09  17:19:07  gdc
 * removed a referenec to global projection_mat
 *
 * Revision 1.61  1997/04/08  14:36:07  gdc
 * local proj mat is not accessed thru functions no a global
 *
 * Revision 1.60  1997/04/08  08:54:07  gdc
 * made current light slot and abslute coord transform non-global
 *
 * Revision 1.59  1997/04/07  14:44:27  gdc
 * shadow limit planes are now static to this module
 *
 * Revision 1.58  1997/04/07  13:23:21  gdc
 * made current_trans_set_id static to this module and passed it into the
 * mesh and convex routines as a parameter
 *
 * Revision 1.57  1997/02/03  11:03:43  gdc
 * removed single D3DOPTIM_SGL conditional form this file as this
 * is now the only build option
 *
 * Revision 1.56  1996/12/10  23:50:13  erf
 * Changed #ifdef D3DOPTIM to #if D3DOPTIM.
 *
 * Revision 1.55  1996/12/03  16:55:57  erf
 * Using D3DOPTIM_SGL compile switch for D3D optimisation do:
 * 1. Set current_trans_set_id = 1. Fixed translucent pass bug.
 *
 * Revision 1.54  1996/11/11  16:03:49  gdc
 * fixed uninitialised local variable in shadow limit plane handling
 *
 * Revision 1.53  1996/10/16  14:41:44  gdc
 * added support for processing shadow limiting nodes
 *
 * Revision 1.52  1996/10/09  15:35:19  mal
 * Changed some more metric macros.
 *
 * Revision 1.51  1996/10/03  12:08:38  mal
 * Added the metric macros for doing code profiles.
 *
 * Revision 1.50  1996/09/24  18:26:15  sjf
 * Fixed bug with light positions : Was not concatenating with the
 * transform up to that point.
 * NOTE: There is probably a big error with the points/collision
 * detection. This needs to be sorted out. I don't think there will
 * be any consistency between local and "world/camera" points.
 *
 * Revision 1.49  1996/09/24  15:27:15  sjf
 * Noticed minor bug with point/camera/light positioning with non-state
 * preserving lists. The transforms might not have been included under some
 * conditions.
 *
 * Revision 1.48  1996/09/10  10:16:16  jrc
 * Added collision state to list of parameters for RnProcessPointPosNode (to
 * enable rnpoint.c to compile).
 *
 * Revision 1.47  1996/07/11  16:41:01  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.46  1996/07/05  07:59:04  sjf
 * Fixed bug with the state preservation of lights - the "position dirty"
 * flag might not be set correctly when a list  was exited.
 *
 * Revision 1.45  1996/06/20  20:38:31  jop
 * Added facility in debug to "binary chop" out meshes to find one that
 * is mis-behaving.
 *
 * Revision 1.44  1996/06/06  23:06:46  sjf
 * Added material instancing.
 *
 * Revision 1.43  1996/05/31  11:44:31  sjf
 * Added an extra little fix to the node position code (ie lights
 * and camera positioning)  - We weren't checking if a list was
 * ignored.
 *
 * Revision 1.42  1996/05/30  20:37:59  sjf
 * 1) Added texture caching.
 * 2) Fixed bug in camera and light positioning where it was
 *   ignoring non state preserving lists.
 *
 * Revision 1.41  1996/05/21  14:02:00  sjf
 * Added new node type that is simply skipped over (makes deleting easy!)
 *
 * Revision 1.40  1996/05/09  13:44:31  sjf
 * Added facility to ignore display lists.
 *
 * Revision 1.39  1996/05/08  14:59:34  sjf
 * "Optimised" the instance subsitition a little bit. This is in anticipation
 * of putting in material instancing.
 *
 * Revision 1.38  1995/11/02  19:51:00  sjf
 * Added fog quality.
 *
 * Revision 1.37  1995/11/02  16:55:31  jop
 * Added quality default flags init from ini file
 *
 * Revision 1.36  1995/10/06  20:10:06  sjf
 * Initialised extra light state values.
 *
 * Revision 1.35  1995/10/05  15:23:18  jrc
 * Added 'Found Material node' message.
 *
 * Revision 1.34  1995/10/04  08:37:57  sjf
 * Initialised the projection map invalid flag and had a guess at the
 * size of a texture.
 *
 * Revision 1.33  1995/09/28  10:26:08  pdl
 * RnProcessMaterialNode has changed.
 *
 * Revision 1.32  1995/09/27  14:59:54  jrc
 * 1. Added RnGetPointTransform for obtaining the position transform of the
 *    point via the position node (similar to RnGetLightTransform).
 * 2. Moved determination of whether to use the position node when we encounter
 *    the point node from rnpoint.c to this file to enable use of static
 *    functions.
 * 3. Now passing transform state instead of collision state to
 *    RnProcessPointPosNode.
 *
 * Revision 1.31  1995/09/25  16:35:21  sjf
 * Added Multi Light pseudo shadow handling
 *
 * Revision 1.30  1995/09/18  20:04:07  sjf
 * Added a flag to indicate if weve processed a light volume yet
 *
 * Revision 1.29  1995/09/16  19:46:02  sjf
 * Implemented instancing and instance substitution.
 *
 * Revision 1.28  1995/09/15  18:28:26  jrc
 * Changed spacing of a comment because I forgot to say that last revision
 * added setting the identity matrix for absoluteCoordTransform for default
 * camera situations.
 *
 * Revision 1.27  1995/09/15  18:21:56  jrc
 * 1. Added the parentUpdatePoints parameter to call to RnProcessConvexNode.
 * 2. Added pState->pTransformState parameter to call to RnProcessPointNode.
 *
 * Revision 1.26  1995/09/15  12:13:57  jop
 * Now sets dirty position flag when it processes transforms
 *
 * Revision 1.25  1995/09/12  09:18:12  sjf
 * Added light positioning.
 *
 * Revision 1.24  1995/09/04  15:26:19  sm
 * changed for multi trans
 *
 * Revision 1.23  1995/08/28  11:36:30  pdl
 * texas_precomp was not initialised.
 *
 * Revision 1.22  1995/08/27  15:31:22  sjf
 * FIxed unitialiased "current light slot"
 *
 * Revision 1.21  1995/08/21  16:02:18  sm
 * reset opacity to 1.0
 *
 * Revision 1.20  1995/08/04  14:41:29  sjf
 * Added code to position the scene relative to the camera.
 *
 * Revision 1.19  1995/07/30  16:08:11  sjf
 * Set texture affect ambient to be initially on.
 *
 * Revision 1.18  1995/07/30  15:15:20  sjf
 * Set up a default material ambient component.
 *
 * Revision 1.17  1995/07/28  13:25:59  sjf
 * Initialised texture effect to diffuse.
 *
 * Revision 1.16  1995/07/27  12:54:18  sjf
 * Changed the way traverse determines whether to start with a camera
 * or a list.
 *
 * Revision 1.15  1995/07/27  12:51:36  jop
 * Added mesh rendering routine
 *
 * Revision 1.14  1995/07/27  11:00:27  sjf
 * Added more debug.
 *
 * Revision 1.13  1995/07/26  16:27:51  sjf
 * y
 * (I didnt want to type the y above, honest).. 
 * Anyway, Put the main traversal call in,
 * that sets up the initials states etc.
 *
 * Revision 1.12  1995/07/20  11:53:53  jop
 * removed #include mod_id.h
 *
 * Revision 1.11  1995/07/14  14:51:09  sjf
 * Moved mod id to earlier pos in file.
 *
 * Revision 1.10  1995/07/10  12:29:18  jrc
 * Added several necessary #includes.
 *
 * Revision 1.9  1995/07/06  12:02:10  sjf
 * added prefix to light state enumerated type.
 *
 * Revision 1.8  1995/07/06  11:31:51  sjf
 * 1) Changed quality state structure to use bit flags.
 * 2) Removed saving of material state for convex primitives with local
 *    materials - definition of SGL's behaviour has changed.
 *
 * Revision 1.7  1995/06/27  10:04:02  sm
 * added initialisation
 *
 * Revision 1.6  1995/06/16  17:48:59  sm
 * modified light node function calls
 *
 * Revision 1.5  1995/06/15  11:15:20  sm
 * Included header files.
 *
 * Revision 1.4  1995/06/15  10:08:59  sjf
 * Mostly written all list/lod/instance list handling code in the
 * display list traversal. Substitutions etc are yet to be done.
 *
 * Revision 1.3  1995/06/13  10:47:46  sjf
 * 1) Added dummy instance substitution routines.
 * 2) Added handling for Level of Detail and instances.
 *
 * Revision 1.2  1995/06/12  18:49:05  sjf
 * Tided list based node processing and added other node
 * type processing.
 *
 * Revision 1.1  1995/06/12  16:19:58  sjf
 * Initial revision
 *
 *
 **************************************************************************/

/*
// define Debug Module IDs
*/
#define MODULE_ID	MODID_RN


#include "sgl_defs.h"
#include "sgl.h"
#include "sgl_math.h"

#include "profile.h"

#include "nm_intf.h"
#include "dlnodes.h" 

#include "dlglobal.h"
#include "dltransf.h"

#include "rnglobal.h"
#include "rnstate.h"
#include "rntrav.h"
#include "rnmesh.h"


/*
// Include the Render processing header files...
*/

#include "rntransf.h"
#include "rnconvex.h"
#include "rnmater.h"
#include "rnlights.h"
#include "rnlod.h"
#include "rnpoint.h"
#include "rnqualit.h"

#include "txmops.h"

#include "list.h"
#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code !!! */ 

/* structure which contains shadow limiting planes */
static SHADOW_LIM_STRUCT ShadowLimitPlanes;

static int current_trans_set_id;

/**************************************************************************
 * Function Name  : InitMasterState
 * Inputs         : 
					  
 * Outputs        :  
 * Input/Output	  : pState
						  
 * Returns        : 
 * Global Used    : 
 * Description    : initialises master state - used at beginning of render. 
 *					
 **************************************************************************/


void InitMasterState( MASTER_STATE_STRUCT *pState)
{
	int i;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	/* initialise light state info to just a single grey ambient light */
	
	pState->pLightsState->num_lights = 0;
	pState->pLightsState->numOnParLights = 0;
	pState->pLightsState->numOnPntLights= 0;
	pState->pLightsState->numOffLights= 0;

	pState->pLightsState->flags = lsf_ambient_grey | 
								  lsf_earl_grey | 
								  lsf_dirty_smooth;
	pState->pLightsState->ambient_colour[0] = 0.1f;

	/*
	// Set all the (relevant) light slots to be non shadowed
	*/
	for(i = 1; i < NUM_LIGHT_SLOTS; i++)
	{
		pState->pLightsState->light_slots[i].multi_light = FALSE;
	}

	/* 
	// initialise material state to diffuse grey with no texture
	// BUT set texture effect to affect the diffuse
	*/

	pState->pMaterialState->diffuse[0] = 0.5f;
	pState->pMaterialState->diffuse[1] = 0.5f;
	pState->pMaterialState->diffuse[2] = 0.5f;

	pState->pMaterialState->specular[0] = 0.0f;
	pState->pMaterialState->specular[1] = 0.0f;
	pState->pMaterialState->specular[2] = 0.0f;

	pState->pMaterialState->specular_shininess = 0;
	pState->pMaterialState->specular_shininess_float = 0.0f;

	pState->pMaterialState->glow[0] = 0.0f;
	pState->pMaterialState->glow[1] = 0.0f;
	pState->pMaterialState->glow[2] = 0.0f;

	pState->pMaterialState->ambient[0] = 0.5f;
	pState->pMaterialState->ambient[1] = 0.5f;
	pState->pMaterialState->ambient[2] = 0.5f;

	pState->pMaterialState->opacity = 1.0f;

	pState->pMaterialState->texture_flags = affect_diffuse | affect_ambient;

	pState->pMaterialState->texas_precomp.TexAddress=0;
	pState->pMaterialState->texas_precomp.LowWord=0;

	/* transform state will either be an identity matrix
	   or the inverse camera transform in the case when
	   a camera in given in the render command */ 
	    
  	SetIdentityMatrix(pState->pTransformState);


	/* 
	//set quality state flags 
	// get flags read in from sgl.ini
	*/
	pState->pQualityState->flags = GetDefaultQualityFlags ();

	/*
	// Set the quality fog value - this is determined from the camera's
	// setting
	*/
	if(pProjMat->FogOn)
	{
		/*
		// By default turn on fog
		*/
		pState->pQualityState->flags |= qf_fog;
	}
	else
	{
		/*
		// Else turn it off
		*/
		pState->pQualityState->flags &= ~qf_fog;
	}

	/* no collision points */

	pState->pCollisionState->num_pnts = 0;


	/* initialise instance subs to zero */

	pState->pInstanceSubState->num_subs=0;

}




/**************************************************************************
//
// Define a set of internal only routines that can preserve the state of
// variables where required.
//
 **************************************************************************/

/**************************************************************************
 * Function Name  : PreserveTransformState   INTERNAL ONLY
 * Inputs         : pState- pointer to a master state stack "frame"
 * Outputs        : None
 * Input/Output	  : an sgl error value.
 * Returns        : None
 * Global Used    : State variables and state stack limit.
 *
 * Description    : If NECESSARY, this saves the transform state variable
 **************************************************************************/
static INLINE void PreserveTransformState( MASTER_STATE_STRUCT *pState,
									  int *error)
{
	/*
	// Check if we need to save the transform state.
	*/
	if(pState->saveFlags & state_save_transform)
	{
		/*
		// If there is room on the stack, copy the top of
		// the stack, and increment the stack pointer.
		*/
		if(pState->pTransformState != pTransformStackLast)
		{
			*(pState->pTransformState + 1) = *pState->pTransformState;
			pState->pTransformState ++;
		}
		/*
		// else no room, just flag an error
		*/
		else if(*error == sgl_no_err)
		{
			*error = sgl_err_list_too_deep;
		}
		/*
		// Clear the save flag regardless
		*/
		pState->saveFlags &= ~state_save_transform;

	}/*end if*/
}


/**************************************************************************
 * Function Name  : PreserveMaterialState INTERNAL ONLY
 * Inputs         : pState- pointer to a master state stack "frame"
 * Outputs        : 
 * Input/Output	  : an sgl error value.
 * Returns        : 
 * Global Used    : State variables and state stack limit.
 *
 * Description    : If NECESSARY, this saves the material state variable
 **************************************************************************/
static INLINE void PreserveMaterialState( MASTER_STATE_STRUCT *pState,
									  int *error)
{
	/*
	// Check if we need to save the state.
	*/
	if(pState->saveFlags & state_save_material)
	{
		/*
		// If there is room on the stack, copy the top of
		// the stack, and increment the stack pointer.
		*/
		if(pState->pMaterialState != pMaterialStackLast)
		{
			*(pState->pMaterialState + 1) = *pState->pMaterialState;
			pState->pMaterialState ++;
		}
		/*
		// else no room, just flag an error
		*/
		else if(*error == sgl_no_err)
		{
			*error = sgl_err_list_too_deep;
		}
		/*
		// Clear the save flag regardless
		*/
		pState->saveFlags &= ~state_save_material;

	}/*end if*/
}

/**************************************************************************
 * Function Name  : PreserveLightsState  INTERNAL ONLY
 * Inputs         : pState- pointer to a master state stack "frame"
 * Outputs        : 
 * Input/Output	  : an sgl error value.
 * Returns        : 
 * Global Used    : State variables and state stack limit.
 *
 * Description    : If NECESSARY, this saves the lights state variable
 **************************************************************************/
static INLINE void PreserveLightsState( MASTER_STATE_STRUCT *pState,
									  int * error)
{
	/*
	// Check if we need to save the state.
	*/
	if(pState->saveFlags & state_save_lights)
	{
		/*
		// If there is room on the stack, copy the top of
		// the stack, and increment the stack pointer.
		*/
		if(pState->pLightsState != pLightsStackLast)
		{
			*(pState->pLightsState + 1) = *pState->pLightsState;
			pState->pLightsState ++;
		}
		/*
		// else no room, just flag an error
		*/
		else if(*error == sgl_no_err)
		{
			*error = sgl_err_list_too_deep;
		}
		/*
		// Clear the save flag regardless
		*/
		pState->saveFlags &= ~state_save_lights;

	}/*end if*/
}


/**************************************************************************
 * Function Name  : PreserveQualityState  INTERNAL ONLY
 * Inputs         : pState- pointer to a master state stack "frame"
 * Outputs        : 
 * Input/Output	  : an sgl error value.
 * Returns        : 
 * Global Used    : State variables and state stack limit.
 *
 * Description    : If NECESSARY, this saves the Quality state variable
 **************************************************************************/
static INLINE void PreserveQualityState( MASTER_STATE_STRUCT *pState,
									  int * error)
{
	/*
	// Check if we need to save the state.
	*/
	if(pState->saveFlags & state_save_quality)
	{
		/*
		// If there is room on the stack, copy the top of
		// the stack, and increment the stack pointer.
		*/
		if(pState->pQualityState != pQualityStackLast)
		{
			*(pState->pQualityState + 1) = *pState->pQualityState;
			pState->pQualityState ++;
		}
		/*
		// else no room, just flag an error
		*/
		else if(*error == sgl_no_err)
		{
			*error = sgl_err_list_too_deep;
		}
		/*
		// Clear the save flag regardless
		*/
		pState->saveFlags &= ~state_save_quality;

	}/*end if*/
}



/**************************************************************************
 * Function Name  : PreserveCollisionState  INTERNAL ONLY
 * Inputs         : pState- pointer to a master state stack "frame"
 * Outputs        : 
 * Input/Output	  : an sgl error value.
 * Returns        : 
 * Global Used    : State variables and state stack limit.
 *
 * Description    : If NECESSARY, this saves the Collision state variable
 **************************************************************************/
static INLINE void PreserveCollisionState( MASTER_STATE_STRUCT *pState,
									  int * error)
{
	/*
	// Check if we need to save the state.
	*/
	if(pState->saveFlags & state_save_collision)
	{
		/*
		// If there is room on the stack, copy the top of
		// the stack, and increment the stack pointer.
		*/
		if(pState->pCollisionState != pCollisionStackLast)
		{
			*(pState->pCollisionState + 1) = *pState->pCollisionState;
			pState->pCollisionState ++;
		}
		/*
		// else no room, just flag an error
		*/
		else if(*error == sgl_no_err)
		{
			*error = sgl_err_list_too_deep;
		}
		/*
		// Clear the save flag regardless
		*/
		pState->saveFlags &= ~state_save_collision;

	}/*end if*/
}

/**************************************************************************
 * Function Name  : PreserveInstanceSubState  INTERNAL ONLY
 * Inputs         : pState- pointer to a master state stack "frame"
 * Outputs        : 
 * Input/Output	  : an sgl error value.
 * Returns        : 
 * Global Used    : State variables and state stack limit.
 *
 * Description    : If NECESSARY, this saves the Instancing  state variable
 **************************************************************************/
static INLINE void PreserveInstanceSubState( MASTER_STATE_STRUCT *pState,
									  int * error)
{
	/*
	// Check if we need to save the state.
	*/
	if(pState->saveFlags & state_save_instance_subs)
	{
		/*
		// If there is room on the stack, copy the top of
		// the stack, and increment the stack pointer.
		*/
		if(pState->pInstanceSubState != pInstanceSubStackLast)
		{
			*(pState->pInstanceSubState + 1) = *pState->pInstanceSubState;
			pState->pInstanceSubState ++;
		}
		/*
		// else no room, just flag an error
		*/
		else if(*error == sgl_no_err)
		{
			*error = sgl_err_list_too_deep;
		}
		/*
		// Clear the save flag regardless
		*/
		pState->saveFlags &= ~state_save_instance_subs;

	}/*end if*/
}



/**************************************************************************
 * Function Name  : InstanceSubstitute   --LOCAL ONLY ---
 * Inputs         : listName - name of a list
 *					Instance substitution state
 * Outputs        : None
 * Returns        : If no replacement is specified, it returns a pointer to the
 *					named list, or a pointer to its replacement if one IS specified.
 *					If the named list (or its replacement) is either the null list, 
 *					or isn't in the name table (because of deletion?) then NULL is
 *					returned.
 *
 * Global Used    : Name table.
 *
 * Description    : Scans the substitution state to see if a replacement has been 
 *					specified for the supplied list name.
 **************************************************************************/

static LIST_NODE_STRUCT *  InstanceSubstitute(int listName, 
				INSTANCE_SUBS_STATE_STRUCT *pInstanceSubState)
{
	LIST_NODE_STRUCT * pList;
	int				 * pSubs;
	int i;
	int nType;

	/*
	// handle the case if the list specified is the special NULL list.
	// This can't be substituted.
	*/
	if(listName == SGL_NULL_LIST)
	{
		pList = NULL;
	}
	else
	{
		ASSERT(pInstanceSubState->num_subs <= SGL_MAX_INSTANCE_SUBS);
		/*
		// Search for a substitution
		*/
		pSubs = pInstanceSubState->nSubs[0];

		for(i = pInstanceSubState->num_subs; i !=0 ; i--, pSubs+=2)
		{
			/*
			// If the list ha a replacement, make that replacement
			// and exit the loop
			*/
			if(listName == pSubs[0])
			{
				DPF(( DBG_MESSAGE, "	Replacing List %d with %d ", 
					   listName, pSubs[1]));

				listName = pSubs[1];
				break;
			}
		} /*end for*/

		/*
		// IF the found substitution (if any) is the NULL_LIST, use a NULL pointer.
		*/
		if(listName == SGL_NULL_LIST)
		{
			pList = NULL;
		}
		/*
		// Else look it up in the name table. If it has been deleted, then
		// it will return a null pointer, which is quite safe...
		// Also check the type of the item
		*/
		else
		{
			pList= GetNamedItemAndType(dlUserGlobals.pNamtab, listName,	&nType);

			/*
			// if this item is NOT a list, then set the pointer to NULL
			*/
			if(nType != nt_list_node)
			{
				DPF(( DBG_MESSAGE, "The substitution is NOT a list. Skipping "));

				pList = NULL;
			}
		}
		
	}

	return pList;
}




/**************************************************************************
 * Function Name  : RnProcessInstanceSubsNode   --LOCAL ONLY ---
 * Inputs         : listName - name of a list
 *					Instance substitution state
 * Outputs        : None
 * Returns        : The replacement list name, or the original if no subsitution is
 *					specified.
 * Global Used    : None
 *
 * Description    : Scans the substitution state to see if a replacement has been 
 *					specified for the supplied list name.
 **************************************************************************/
static void	RnProcessInstanceSubsNode( INSTANCE_SUBS_NODE_STRUCT* pNode,
								INSTANCE_SUBS_STATE_STRUCT *pInstanceSubState)

{
	int newCount, currCount;
	sgl_bool DidReplace;

	/*
	// Pointer to each of the new substitions, and a pointer to the current
	// subs
	*/
	sgl_int16 * pNew;
	int * pCurr;

	int newOrig, newReplacement;

	ASSERT(pNode->node_hdr.n16_node_type == nt_inst_subs);
	ASSERT(pNode->num_subs <= SGL_MAX_INSTANCE_PARAMS);

	ASSERT(pInstanceSubState->num_subs <= SGL_MAX_INSTANCE_SUBS);
	
	/*
	// Step through each of the new substitutions, applying each
	// to all the current substitutions
	*/
	pNew = pNode->param_list[0];
	for(newCount = pNode->num_subs; newCount != 0 ; newCount--, pNew+=2)
	{
		/*
		// remember if we have "added" this substitution to the 
		// the replacement state
		*/
		DidReplace = FALSE;

		/*
		// Get the list and its replacement
		*/
		newOrig 	   = pNew[0];
		newReplacement = pNew[1];

		/*
		// Step through the replacement state
		*/
		pCurr = pInstanceSubState->nSubs[0];
		for(currCount = pInstanceSubState->num_subs; currCount != 0 ; 
													 currCount--, pCurr+=2)
		{
			/*
			// LET  A->B be the current one in the state, and our new one
			// is C->D...    if C==A, then change A->B to A->D
			*/
			if(newOrig == pCurr[0])
			{
				pCurr[1] = newReplacement;

				/*
				// OK we are replacing A->B with A->C, so mark that we dont need
				// to actually add A->C
				*/
				DidReplace = TRUE;
			}
			/*
			// ELSE is B==C, then change A->B to A->D
			*/
			else if(newOrig == pCurr[1])
			{
				pCurr[1] = newReplacement;
			}

		}/*end for currCount*/

		/*
		// Have we effectively added this substitution to the state list?
		// and if not do we have room?
		*/
		if( !DidReplace && (pInstanceSubState->num_subs <SGL_MAX_INSTANCE_SUBS))
		{
			/*
			// Add it
			*/
			pCurr[0] = newOrig;
			pCurr[1] = newReplacement;

			pInstanceSubState->num_subs ++;

		}/*if add new sub to sub state */

	}/*end for newsubs newCount*/

}

/**************************************************************************
 * Function Name  : RnConcatNPListTransforms   --LOCAL ONLY ---
 * Inputs         : pList - pointer to the list that we want to get all the
 *						  transforms from, and combine with the passed in
 *						  transformation.
 * Input/Outputs  : The transform with which we combine the transforms inside
 *					this list
 * Returns        : None
 * Global Used    : None
 *
 * Description    : Steps through the list, combining any transforms, and
 *					recursively descending any contianed non preserving lists.
 **************************************************************************/

static void	RnConcatNPListTransforms(const LIST_NODE_STRUCT *pList,
									TRANSFORM_STRUCT  *pTransform)
{
	const DL_NODE_STRUCT * pNode;

	/*
	// Get the first element in the list
	*/
	pNode = pList->pfirst;

	while(pNode != NULL)
	{
		/*
		// If this is a transformation, then contatenate it with the
		// one we've already got
		*/
		if(pNode->n16_node_type == nt_transform)
		{
			TransformMultiply(pTransform, 
					&((TRANSFORM_NODE_STRUCT*)pNode)->transform,
					pTransform);

		}
		/*
		// NOTE! if this node is a list  which does not preserve 
		// state, then we should then process it  (and any of NP 
		// decendants as well)
		//
		// Ignore the list if we arent to process it.
		*/
		else if(pNode->n16_node_type == nt_list_node)
		{
			LIST_NODE_STRUCT * pList2 = (LIST_NODE_STRUCT *)pNode;

			if(!(pList2->flags & lf_preserve_state) &&
				(pList2->flags & lf_process_list))
			{
				RnConcatNPListTransforms(pList2, pTransform);
			}
		}/*end if*/

		/*
		// Move to the next one
		*/
		pNode = pNode->next_node;
		
	}/*end while*/

}


/**************************************************************************
 * Function Name  : RnGetTransformToNode   --LOCAL ONLY ---
 * Inputs         : pNode - pointer to the node that we want to get the
 *							transformation for.
 *					pParentList - pointer to its parent list.
 * Outputs        : The transform of this node relative to the root of the
 *					list
 * Returns        : None
 * Global Used    : None
 *
 * Description    : Basically backtracks up the through the parents of the lists
 *					concatentating the positioning transformations.
 *
 *					This is a little tricky, as we only have a forward linked
 *					list.
 *
 *					The routine is used as part of the positioning of lights
 *					and cameras.
 **************************************************************************/
static void	RnGetTransformToNode(const DL_NODE_STRUCT * pNode,
								 const LIST_NODE_STRUCT * pList,
					   TRANSFORM_STRUCT *pTransform)

{
	TRANSFORM_STRUCT LocalTransform; /*transform for current list*/

	/*
	// Pointer to the node we stop at in the current list
	*/
	const DL_NODE_STRUCT * pLastToDo;

	/*
	// As a minor optimisation, remember if a list contains no
	// transforms. This is also useful to keep a record of whether
	// we have to re-initialise the local transform structure. If
	// it didn't get used in the last pass, then there is no
	// point in re-initialising it again!
	*/
	sgl_bool TransformInThisList;

	ASSERT(pNode!=NULL);
	ASSERT(pList!=NULL);
	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);

	/*
	// Initialise the positioning transform, and
	*/
	SetIdentityMatrix(pTransform);

	/*
	// Force us to initialise the local transform
	*/
	TransformInThisList = TRUE;

	/*
	// set the last to do to be the passed in node.
	*/
	pLastToDo = pNode;


	/*
	// While we still have lists left
	*/
	while(pList != NULL)
	{
		/*
		// Initialise the transformations for the local list, but
		// if it wasn't used in the last list, then it is already
		// intialised.
		*/
		if(TransformInThisList)
		{
			SetIdentityMatrix(&LocalTransform);
			TransformInThisList = FALSE;
		}
		
		/*
		// Use pNode to step through the contents of the current list
		*/
		pNode = pList->pfirst;
		while(pNode != pLastToDo)
		{
			/*
			// Whe should NOT be able to reach the end of the list
			*/
			ASSERT(pNode != NULL);

			/*
			// examine the type of this node..
			// If its a transformation, combine it with the 
			// the current local. 
			// Note that the order is very important.
			*/
			if(pNode->n16_node_type == nt_transform)
			{
				TransformMultiply(&LocalTransform, 
					&((TRANSFORM_NODE_STRUCT*)pNode)->transform,
					&LocalTransform);

				TransformInThisList = TRUE;
					
			}
			/*
			// NOTE! if this node is a list  which does not preserve 
			// state, then we should then process it  (and any of NP 
			// decendants as well)
			// NOTE we must ignore lists that aren't to be processed.
			*/
			else if(pNode->n16_node_type == nt_list_node)
			{
				LIST_NODE_STRUCT * pList = (LIST_NODE_STRUCT *)pNode;

				if(!(pList->flags & lf_preserve_state) &&
					(pList->flags & lf_process_list))
				{
					RnConcatNPListTransforms(pList, &LocalTransform);
					
					/*
					// Assume there was a transform in this list... it
					// won't hurt it there wasn't
					*/
					TransformInThisList = TRUE;
				}
			}/*end if*/
			
			/*
			// Move to the next one
			*/
			pNode = pNode->next_node;

		}/*end while processing current list*/

		/*
		// combine the local transform with the overall one.
		//
		// Again note that the order is very important
		*/
		if(TransformInThisList)
		{
			TransformMultiply(&LocalTransform, pTransform, pTransform);
		}

		/*
		// Go up to this lists parent, and mark that the list we were on,
		// is the one to stop on the next pass.
		*/
		pLastToDo = (DL_NODE_STRUCT *)pList;
		pList = pList->pparent;

	}/*end while lists left*/

}/*end function*/



/**************************************************************************
 * Function Name  : RnGetCameraTransform   --LOCAL ONLY ---
 * Inputs         : pCamera - pointer to the camera node
 * Outputs        : The transform use to reposition objects relative to the
 *					camera.
 * Returns        : None
 * Global Used    : None
 *
 * Description    : gets the camera's transform, descales and inverts it, so
 *					that the scene can be rendered relative to the camera's
 *					viewpoint.
 **************************************************************************/
static void	RnGetCameraTransform(const CAMERA_NODE_STRUCT * pCamera,
								   TRANSFORM_STRUCT *pTransform)
{
	/*
	// pointer to the cameras parent list
	*/
	LIST_NODE_STRUCT * pList;

	ASSERT(pCamera->node_hdr.n16_node_type == nt_camera);
	
	/*
	//Get the cameras parent
	*/	
	pList = pCamera->pparent;

	ASSERT(pList!=NULL);
	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);

	/*
	// Get the cameras position etc
	*/
	RnGetTransformToNode( (DL_NODE_STRUCT *) pCamera,  pList,  pTransform);
	RnGlobalSetAbsoluteCoordTransform (pTransform);

	/*
	// Descale and invert the matrix
	*/
	TransformDescale(pTransform);
	TransformInvert(pTransform);


} /* end for */


	   
/**************************************************************************
 * Function Name  : RnGetLightTransform   --LOCAL ONLY ---
 * Inputs         : pLightPosNode  - pointer to the lights position node.
 *
 * Outputs        : pLightTransform - The position for the light.
 *
 * Returns        : None
 * Global Used    : None
 *
 * Description    : Computes the positioning info for a light based on
 *					its position node.
 **************************************************************************/
static void	RnGetLightTransform(const LIGHT_POS_NODE_STRUCT * pLightPosNode,
									  TRANSFORM_STRUCT *pLightTransform)
{
	/*
	// pointer to the  parent list
	*/
	LIST_NODE_STRUCT * pList;

	ASSERT(pLightPosNode->node_hdr.n16_node_type == nt_light_pos);
	
	/*
	//Get the position nodes parent
	*/	
	pList = pLightPosNode->pparent_list;

	ASSERT(pList!=NULL);
	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);

	/*
	// Get the position etc
	*/
	RnGetTransformToNode( (DL_NODE_STRUCT *) pLightPosNode,  pList,  
						  pLightTransform);

} /* end for */
	   

/**************************************************************************
 * Function Name  : RnGetPointTransform   --LOCAL ONLY ---
 *
 * Inputs         : pPointPosNode  - pointer to the point's position node.
 * Outputs        : pLightTransform - The position for the point.
 * Returns        : None
 * Global Used    : None
 *
 * Description    : Computes the positioning info for a point based on
 *					its position node.
 *
 *					This is identical to RnGetLightTransform, but for position/
 *					collision points instead of lights.
 **************************************************************************/
static void	RnGetPointTransform(
  const POINT_POSITION_NODE_STRUCT *pPointPosNode,
  TRANSFORM_STRUCT				   *pPointTransform)
{
	/*
	// pointer to the parent list
	*/
	LIST_NODE_STRUCT *pList;

	ASSERT(pPointPosNode->node_hdr.n16_node_type == nt_point_pos);
	
	/*
	//Get the position nodes parent
	*/	
	pList = pPointPosNode->pparent_list;

	ASSERT(pList != NULL);
	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);

	/*
	// Get the position etc
	*/
	RnGetTransformToNode( (DL_NODE_STRUCT *) pPointPosNode, pList,
						  pPointTransform);
}


/**************************************************************************
 * Function Name  : RnTextureCacheTraverse
 * Inputs         : pList - pointer to a display list
 *					pCachedTexture - pointer to cached texture currently
 *									 in use.
 *					pState- pointer to a master state stack "frame"
 *					depthRemaining - recursion is prevented if this hits zero.
 * Outputs        : None
 * Returns        : None
 * Global Used    : Display list, name table, hardware parameter managers etc.
 *
 * Description    : Similar to the main traverser (see below), except this only 
 *					works out what cached textures are required.
 * 
 **************************************************************************/
static void RnTextureCacheTraverse(const  LIST_NODE_STRUCT * pList, 
									void ** ppCachedTexture,
									MASTER_STATE_STRUCT *pState,
									 int depthRemaining)
{
	/*
	// node pointer used to traverse the list
	*/
	DL_NODE_STRUCT * pNode;

	/*
	// So that we can preserve the state when entering
	// child lists, we need to be able to copy the state
	// variable pointers
	*/
	MASTER_STATE_STRUCT localState;

	/*
	// The current cached texture being used
	*/
	void * pLocalCachedTexture;

	/*
	// check recursion depth param
	*/
	ASSERT((depthRemaining >= 0)&&(depthRemaining < 1000))

	/*
	// step through the contents of the list
	*/
	pNode = pList->pfirst;
	while(pNode!= NULL)
	{
		ASSERT(pNode->n16_node_type >= 0)
		ASSERT(pNode->n16_node_type < nt_node_limit)

		/*
		// determine what type of node this is, and
		// process accordingly
		*/
		switch(pNode->n16_node_type)
		{

			/* /////////////
			// Handle List, Instance, LOD Nodes...
			///////////// */
			case nt_list_node:
			case nt_instance:
			case nt_lod:
			{
				LIST_NODE_STRUCT *pChildList= NULL;
				int	childName;
				
				DPF((DBG_MESSAGE,"Found list, instance, or LOD node"));

				/*
				// get the pointer to the list node we want, depending
				// on the type of this node
				*/
				switch(pNode->n16_node_type)
				{
					/*////////
					// If a list node, simply get the pointer to the
					// child list
					//////// */
					case nt_list_node:
					{
						pChildList = (LIST_NODE_STRUCT *)pNode;
						break;
					}

					/*////////
					// For the instance node, get the name of the instance,
					// and perform a substitution.
					//////// */
					case nt_instance:
					{
						childName = ((INSTANCE_NODE_STRUCT*)pNode)->referenced_list;

						/*
						// Perform an instance substitution
						*/
						pChildList = InstanceSubstitute(childName, 
											pState->pInstanceSubState);

						break;
					}

					/*////
					//Level Of Detail Node
					//// */
					case nt_lod:
					{
						/*
						// Get the name of the Level of Detail List to
						// to use
						*/
						childName = RnProcessLodNode((LOD_NODE_STRUCT*)pNode,
													pState->pTransformState);

						/*
						// Perform an instance substitution
						*/
						pChildList = InstanceSubstitute(childName, 
											pState->pInstanceSubState);

						break;
					}

					/* ////
					//Something to remove warnings of unitialised pChildList
					//// */
					default:
					{
						ASSERT(FALSE);
						break;
					}
				}/*end switch*/


				/*
				// If we have been given the "Null list" (eg in the level of
				// detail etc) OR, we are to skip this list, then do nothing.
				*/
				if((pChildList == NULL)|| !(pChildList->flags & lf_process_list))
				{
					/* DO NOTHING*/
				}
				/*
				// else flag a "too deep" error if we are at the max depth allowed
				*/
				else if(depthRemaining == 0)
				{
					/* DO NOTHING*/
				}
				/*
				// Else allow the recursion - choose either the
				// state preserving path, or not...
				//
				// if Save State, copy the existing state var, and update
				// flags that all must be saved.
				*/
				else if(pChildList->flags & lf_preserve_state)
				{
					localState = *pState;
					localState.saveFlags = ALL_STATE_SAVE_FLAGS;

					/*
					// Make a copy of the current cached texture
					// pointer. It MAY get modified
					*/
					pLocalCachedTexture  = *ppCachedTexture;

					/*
					// We dont need to set the scoping
					//
					//localState.pScopingList = pChildList;
					*/

					/*
					// recurse into this list
					*/
					RnTextureCacheTraverse(pChildList,
										&pLocalCachedTexture,
										&localState,
										depthRemaining - 1);

				}
				/*
				// Else DONT save the state
				*/
				else
				{
					RnTextureCacheTraverse(pChildList,
										   ppCachedTexture,
										   pState,
										   depthRemaining - 1);

				} /*end else*/

				
				break;
			}




			/* /////////////
			// Handle the Instance Substitution Node.
			///////////// */
			case nt_inst_subs:
			{
				int error;
				PreserveInstanceSubState(pState, &error);

				RnProcessInstanceSubsNode((INSTANCE_SUBS_NODE_STRUCT*)pNode,
								pState->pInstanceSubState);
				break;
			}


			/* /////////////
			// Handle Transformations
			///////////// */
			case nt_transform:
			{
				int error;
				DPF((DBG_MESSAGE,"Found Transform node"));
				
				/*
				// First preserve the transform state if necessary
				*/
				PreserveTransformState(pState, &error);

				/*
				// Update the transformation (it is assumed this
				// clears the local projection matrix valid flag)
				*/
				RnProcessTransformNode((TRANSFORM_NODE_STRUCT *)pNode,
									pState->pTransformState);
					
				/*
				// No need for the following
				*/
				/* we need to set the lights' dirty position flag */
				/*pState->pLightsState->flags |= lsf_dirty_position;*/
				
				break;
			}/*end transform case*/


			/* /////////////
			// Handle the primitive types.
			///////////// */
			case nt_convex:
			{
				CONVEX_NODE_STRUCT * pConvex = (CONVEX_NODE_STRUCT *)pNode;

				DPF((DBG_MESSAGE,"Found Convex  node"));
				/*
				// Pass the convex node pointer and entire state pointers in
				// cause we need virtually the lot anyway.
				//
				// The value of parentUpdatePoints is set TRUE if a collision
				// occurs with this object.  This value is passed back to the
				// parent list which may have to remove the hit point from its
				// list of active points.
				*/
				RnCTPreProcessConvexNode(pConvex, pState, *ppCachedTexture);
				
				break;
			}

			case nt_mesh:
			{
				DPF ((DBG_VERBOSE,"Found mesh node"));
				RnCTPreProcessMeshNode ((const MESH_NODE_STRUCT *) pNode, pState,
											   *ppCachedTexture);
				break;
			}


			/* /////////////
			// Material defs...
			///////////// */
			case nt_material:
			{
				DPF((DBG_MESSAGE,"Found Material node"));


				/*
				// Update our cached texture
				*/
				RnCTPreprocessMaterialNode((MATERIAL_NODE_STRUCT*)pNode,
							pState,
							ppCachedTexture);

				break;
			}


			/* /////////////
			// the Light Nodes
			//
			//  We do nothing in this pass
			///////////// */
			case nt_light:
			case nt_light_switch:
			case nt_light_pos:
			case nt_multi_shadow:
			{
				break;
			}


			/* /////////////
			// Camera Node
			///////////// */
			case nt_camera:
			{
				DPF((DBG_MESSAGE,"Found camera node"));
				/*
				// Nothing needs to be done when we encounter a camera
				// node during traversal
				*/
				break;
			}

			/* /////////////
			// (Collision) Point Nodes. - We do nothing
			///////////// */
			case nt_point:
			case nt_point_pos:
			case nt_point_switch:
			{
				break;
			}

			/* /////////////
			// Quality Point Nodes.
			///////////// */
			case nt_quality:
			{
				int error;

				DPF((DBG_MESSAGE,"Found quality node"));

				PreserveQualityState(pState, &error);
				RnProcessQualityNode((QUALITY_NODE_STRUCT*) pNode,
											 pState->pQualityState);
				break;
			}
			

			/*
			// Do nothing
			*/
			case nt_newtran:
			{
				break;
			}
	
			/*
			// Do nothing
			*/
			case nt_shadow_limit:
			{
				break;
			}
	
			/*
			// Was this a deleted item? If so skip it
			*/
			case nt_dummy:
			{
				DPF((DBG_MESSAGE,"Skipping dummy node"));
				break;
			}


			/*
			// If an invalid case, then there's been a programming
			// error !!!!
			*/
			default:
				ASSERT(FALSE)
				break;

		}/*end switch*/
		

		/*
		// move to the next list node
		*/
		pNode = pNode->next_node;
	
	} /*end while*/	

}
/**************************************************************************
 * Function Name  : RnRecursiveTraverse
 * Inputs         : pList - pointer to a display list
 *					pState- pointer to a master state stack "frame"
 *					depthRemaining - recursion is prevented if this hits zero.
 *					parentUpdatePoints - indicates if the parent routine needs
 *					to clean up the list collision points because a collision
 *					was detected.
 * Outputs        : None
 * Returns        : an sgl error value.
 * Global Used    : Display list, name table, hardware parameter managers etc.
 *
 * Description    : Recursively traverses the display list using the supplied
 *					state information, updating the state information and producing
 *					the "hardware parameters" for rendering.
 *
 *					It is assumed that the transformation for the camera etc
 *					has already been set up.
 *
 *					This routine handles the preserving of state variables - i.e.
 *					stacking variables before calling specialised node handling
 *					routines where necesary.
 *
 *					It returns the first "error /warning" encountered
 **************************************************************************/
#if DEBUG
static sgl_bool CompareFunction (void *item, sgl_uint32 s)
{
	return (*((sgl_uint32 *) item) == s);
}
#endif

static int RnRecursiveTraverse(const  LIST_NODE_STRUCT  *pList, 
							   MASTER_STATE_STRUCT  *pState,
							   int depthRemaining,
							   sgl_bool *parentUpdatePoints)
{
	/*
	// error values
	*/
	int error, localErr;
	int nLocalLimPlanes;	
	/*
	// node pointer used to traverse the list
	*/
	DL_NODE_STRUCT * pNode;
	LOCAL_PROJECTION_STRUCT *pLocalProjMat = NULL;

	/*
	// So that we can preserve the state when entering
	// child lists, we need to be able to copy the state
	// variable pointers
	*/
	MASTER_STATE_STRUCT localState;

	/*
	// The following is used in recursive calls to determine if we need to
	// clean up the collision point data.
	*/
	sgl_bool localUpdatePoints;

	/*
	// initialise the error state
	*/
	error = sgl_no_err;

	/*
	// check recursion depth param
	*/
	ASSERT((depthRemaining >= 0)&&(depthRemaining < 1000))


	pLocalProjMat = RnGlobalGetLocalProjMat();
	ASSERT(pLocalProjMat);
	/*
	// step through the contents of the list
	*/
	pNode = pList->pfirst;
	while(pNode!= NULL)
	{
		ASSERT(pNode->n16_node_type >= 0)
		ASSERT(pNode->n16_node_type < nt_node_limit)

		/*
		// determine what type of node this is, and
		// process accordingly
		*/
		switch(pNode->n16_node_type)
		{

			/* /////////////
			// Handle List, Instance, LOD Nodes...
			///////////// */
			case nt_list_node:
			case nt_instance:
			case nt_lod:
			{
				LIST_NODE_STRUCT *pChildList= NULL;
				int	childName;
				
				DPF((DBG_MESSAGE,"Found list, instance, or LOD node"));

				nLocalLimPlanes = ShadowLimitPlanes.nNumShadLimPlanes;

				/*
				// get the pointer to the list node we want, depending
				// on the type of this node
				*/
				switch(pNode->n16_node_type)
				{
					/*////////
					// If a list node, simply get the pointer to the
					// child list
					//////// */
					case nt_list_node:
					{
						pChildList = (LIST_NODE_STRUCT *)pNode;
						break;
					}

					/*////////
					// For the instance node, get the name of the instance,
					// and perform a substitution.
					//////// */
					case nt_instance:
					{
						childName = ((INSTANCE_NODE_STRUCT*)pNode)->referenced_list;

						/*
						// Perform an instance substitution
						*/
						pChildList = InstanceSubstitute(childName, 
											pState->pInstanceSubState);

						break;
					}

					/*////
					//Level Of Detail Node
					//// */
					case nt_lod:
					{
						/*
						// Get the name of the Level of Detail List to
						// to use
						*/
						childName = RnProcessLodNode((LOD_NODE_STRUCT*)pNode,
													pState->pTransformState);

						/*
						// Perform an instance substitution
						*/
						pChildList = InstanceSubstitute(childName, 
											pState->pInstanceSubState);

						break;
					}

					/* ////
					//Something to remove warnings of unitialised pChildList
					//// */
					default:
					{
						ASSERT(FALSE);
						break;
					}
				}/*end switch*/


				localErr = sgl_no_err;

				/*
				// If we have been given the "Null list" (eg in the level of
				// detail etc) OR, we are to skip this list, then do nothing.
				*/
				if((pChildList == NULL)|| !(pChildList->flags & lf_process_list))
				{
					/* DO NOTHING*/
				}
				/*
				// else flag a "too deep" error if we are at the max depth allowed
				*/
				else if(depthRemaining == 0)
				{
					localErr = sgl_err_list_too_deep;
				}
				/*
				// Else allow the recursion - choose either the
				// state preserving path, or not...
				//
				// if Save State, copy the existing state var, and update
				// flags that all must be saved.
				*/
				else if(pChildList->flags & lf_preserve_state)
				{
					localState = *pState;
					localState.saveFlags = ALL_STATE_SAVE_FLAGS;
					localState.pScopingList = pChildList;

					localUpdatePoints = FALSE;

					localErr = RnRecursiveTraverse(pChildList,
										& localState,
									 depthRemaining - 1,
									 & localUpdatePoints);
					
					/*
					// Do we need to clean up the collision detection
					// points because a collision WAS detected?
					*/
					if(localUpdatePoints)
					{
						RnCleanupCollisionState(pState->pCollisionState,
											parentUpdatePoints);
					}

					/*
					// Check if the transform state was altered during the child
					//  list because this PROBABLY means the local to projection 
					// matrix is out of date.
					//
					// Similarly, the light state may be  invalid for the same reason
					// (Note: I was originally going to only set the "dirty position"
					// flag if the light state hadn't been preserved, (for efficiency
					// reasons) but this won't work. If in a preserving child list the
					// transformation changes, a smooth shaded object is encountered
					// and THEN the light state changes, we won't correctly realise the
					// the lights have moved in local space
					*/
					if(localState.pTransformState != pState->pTransformState)
					{
						pLocalProjMat->valid = FALSE;

						pState->pLightsState->flags |= lsf_dirty_position;
					}

				}
				/*
				// Else DONT save the state
				*/
				else
				{
					RnRecursiveTraverse(pChildList, pState,
										 depthRemaining - 1,
									     parentUpdatePoints);
				} /*end else*/

				
				/*
				// Update any error
				*/
				if(error != sgl_no_err)
				{
					error = localErr;
				}

				/* reinstate the number of shadow limit planes */
				ShadowLimitPlanes.nNumShadLimPlanes =  nLocalLimPlanes;
				break;
			}




			/* /////////////
			// Handle the Instance Substitution Node.
			///////////// */
			case nt_inst_subs:
			{
				PreserveInstanceSubState(pState, &error);

				RnProcessInstanceSubsNode((INSTANCE_SUBS_NODE_STRUCT*)pNode,
								pState->pInstanceSubState);
				break;
			}


			/* /////////////
			// Handle Transformations
			///////////// */
			case nt_transform:
			{
				DPF((DBG_MESSAGE,"Found Transform node"));
				
				/*
				// First preserve the transform state if necessary
				*/
				PreserveTransformState(pState, &error);

				/*
				// Update the transformation (it is assumed this
				// clears the local projection matrix valid flag)
				*/
				RnProcessTransformNode((TRANSFORM_NODE_STRUCT *)pNode,
									pState->pTransformState);
					
				/*
				// we need to set the lights' dirty position flag.
				// NOTE: Since we are changing the lights state, we
				// should really preserve it. However, the number of
				// times we change the transformations is fairly high
				// compared to the normal light state changes.  What we
				// will do instead  is reset the flag when we exit state
				// preserving lists.
				*/
				pState->pLightsState->flags |= lsf_dirty_position;
				
				break;
			}/*end transform case*/


			/* /////////////
			// Handle the primitive types.
			///////////// */
			case nt_convex:
			{
				CONVEX_NODE_STRUCT * pConvex = (CONVEX_NODE_STRUCT *)pNode;

				DPF((DBG_MESSAGE,"Found Convex  node"));
				/*
				// Pass the convex node pointer and entire state pointers in
				// cause we need virtually the lot anyway.
				//
				// The value of parentUpdatePoints is set TRUE if a collision
				// occurs with this object.  This value is passed back to the
				// parent list which may have to remove the hit point from its
				// list of active points.
				*/
	SGL_TIME_SUSPEND(DATABASE_TRAVERSAL_TIME)

				RnProcessConvexNode(pConvex,
									pState,
									&ShadowLimitPlanes,
									parentUpdatePoints, 
									current_trans_set_id);
	SGL_TIME_RESUME(DATABASE_TRAVERSAL_TIME)
				
				break;
			}

			case nt_mesh:
			{
#if DEBUG
				static PLIST L;
				static int init = 1, pick = 0;
				sgl_uint32 *puListItem;
				int ID;
				static int Lo, Hi;

				if (init)
				{
					init = 0;
					ListInitialiseList (&L, sizeof (sgl_uint32), 32, NULL);
					Lo = SglReadPrivateProfileInt ("Mesh", "LoLimitID", 0, "sgl.ini");
					Hi = SglReadPrivateProfileInt ("Mesh", "HiLimitID", 0x7FFFFFFF, "sgl.ini");
					pick = SglReadPrivateProfileInt ("Mesh", "Pick", 0, "sgl.ini");
				}
				else if (pick)
				{
					Lo = SglReadPrivateProfileInt ("Mesh", "LoLimitID", 0, "sgl.ini");
					Hi = SglReadPrivateProfileInt ("Mesh", "HiLimitID", 0x7FFFFFFF, "sgl.ini");
				}
				
				puListItem = ListFindItem (L, CompareFunction, (sgl_uint32) pNode);
				
				if (!puListItem)
				{
					puListItem = ListAddItem (L);
					
					if (puListItem)
					{
						*puListItem = (sgl_uint32) pNode;
					}
				}
				
				ID = ListGetItemID (L, puListItem);
				
				if (ID < Lo || ID > Hi)
				{
					break;
				}
#endif
					
				
				DPF ((DBG_VERBOSE,"Found mesh node"));
				SGL_TIME_SUSPEND(DATABASE_TRAVERSAL_TIME)
				RnProcessMeshNode ((const MESH_NODE_STRUCT *) pNode, 
								   pState,current_trans_set_id);
				SGL_TIME_RESUME(DATABASE_TRAVERSAL_TIME)
				break;
			}


			/* /////////////
			// Material defs...
			///////////// */
			case nt_material:
			{
				DPF((DBG_MESSAGE,"Found Material node"));

				/*
				// First preserve the material state if necessary
				*/
				PreserveMaterialState(pState, &error);
				RnProcessMaterialNode((MATERIAL_NODE_STRUCT*)pNode,  
											pState->pMaterialState,
											pState);


				break;
			}

			/* /////////////
			// the Light Nodes
			///////////// */
			case nt_light:
			{
				TRANSFORM_STRUCT *pLightTransform;
				TRANSFORM_STRUCT LightTrans;
				LIGHT_NODE_STRUCT * pLightNode;

				DPF((DBG_MESSAGE,"Found Light node"));
				
				pLightNode = (LIGHT_NODE_STRUCT*) pNode;

				/*
				// First preserve the lights state if necessary
				*/
				PreserveLightsState(pState, &error);

				/*
				// Decide if this light has position node, if so
				// use that to position the light, otherwise use the
				// local transform state.
				*/
				if(pLightNode->plight_position != NULL)
				{
					pLightTransform = &LightTrans;
					RnGetLightTransform(pLightNode->plight_position,
									  	pLightTransform);
					/*
					// Combine the transformation from this position to
					// its positioning node with the current transform state
					*/
					TransformMultiply(pState->pTransformState,
										pLightTransform,
										pLightTransform);
				}
				else
				{
					pLightTransform = pState->pTransformState;
				}

				RnProcessLightNode(pLightNode,
								pLightTransform,  
								pState->pLightsState);

				break;
			}

			case nt_light_switch:
			{
				DPF((DBG_MESSAGE,"Found light switch node"));
				/*
				// First preserve the lights state if necessary
				*/
				PreserveLightsState(pState, &error);
				RnProcessLightSwitchNode((LIGHT_SWITCH_NODE_STRUCT*)pNode,  
										pState->pLightsState);
				break;
			}

			case nt_light_pos:
			{
				DPF((DBG_MESSAGE,"Found light pos node"));
				/*
				// Nothing needs to be done when we encounter a light pos
				// node during traversal
				*/
				break;
			}

			case nt_multi_shadow:
			{
				DPF((DBG_MESSAGE,"Found Multi Shadow node"));
				RnProcessMultiShadowNode((MULTI_SHADOW_NODE_STRUCT*)pNode,  
										pState->pLightsState);
				
				break;
			}

			/* /////////////
			// Camera Node
			///////////// */
			case nt_camera:
			{
				DPF((DBG_MESSAGE,"Found camera node"));
				/*
				// Nothing needs to be done when we encounter a camera
				// node during traversal
				*/
				break;
			}

			/* /////////////
			// (Collision) Point Nodes.
			///////////// */
			case nt_point:
			{
				static TRANSFORM_STRUCT pointPosTransform;

				DPF((DBG_MESSAGE,"Found point node"));
				
				/*
				// ------------------------
				// Preserve collision state
				// ------------------------
				// Only bother to save the state if this is a collision
				// detection point. Points used just for position feedback are
				// quite benign, so we may as well save a little bit of time.
				*/
				if ( ((POINT_NODE_STRUCT*)pNode)->collision_check )
				{
					PreserveCollisionState(pState, &error);
				}

				/*
				// --------------------------------
				// Find correct transform for point
				// --------------------------------
				*/

				/* If we have a position node... */
				if ( ((POINT_NODE_STRUCT*)pNode)->ppoint_position != NULL )
				{
					if ( ((POINT_NODE_STRUCT*)pNode)->collision_check )
					{
						/* calculate transform from position node */

						RnGetPointTransform(
						  ((POINT_NODE_STRUCT*)pNode)->ppoint_position,
						  &pointPosTransform );

						RnProcessPointNode((POINT_NODE_STRUCT*)pNode,
						  pState->pCollisionState, &pointPosTransform);

					}
					/*
					// else do nothing (wait until we reach the position node
					// to find the position of the position-only point).
					*/
				}
				else
				{
					/* use the current transform */

					RnProcessPointNode((POINT_NODE_STRUCT*)pNode,
					  pState->pCollisionState, pState->pTransformState);
				}

				break;
			}


			case nt_point_pos:
			{
				/*
				// get more convenient access to the node
				*/
				POINT_POSITION_NODE_STRUCT * pPosNode;
				pPosNode = (POINT_POSITION_NODE_STRUCT *)pNode;

				/*
				// Check if this position node is actually in use..
				//
				// Note that this routine only needs to handle the positioning
				// of points which AREN'T involved in collision detection. This 
				// test COULD be moved here later as an optimisation.....
				*/
				if( pPosNode->point_name != NM_INVALID_NAME )
				{
					RnProcessPointPosNode(pPosNode, pState->pTransformState,
					  pState->pCollisionState);
				}
					
				break;
			}


			case nt_point_switch:
			{
				/*
				// get more convenient access to the node
				*/
				POINT_SWITCH_NODE_STRUCT * pSwitchNode;
				pSwitchNode = (POINT_SWITCH_NODE_STRUCT *)pNode;

				DPF((DBG_MESSAGE,"Found point switch node"));

				RnProcessPointSwitchNode(pSwitchNode, pState->pCollisionState);

				break;
			}


			/* /////////////
			// Quality Point Nodes.
			///////////// */
			case nt_quality:
			{
				DPF((DBG_MESSAGE,"Found quality node"));

				PreserveQualityState(pState, &error);
				RnProcessQualityNode((QUALITY_NODE_STRUCT*) pNode,
											 pState->pQualityState);
				break;
			}


			case nt_newtran:
			{
				DPF((DBG_MESSAGE,"Found new translucent node"));

				current_trans_set_id++;				

				break;
			}
	
		    case nt_shadow_limit:
		    {
			    SHAD_LIM_NODE_STRUCT *pShadNode;
				DPF((DBG_MESSAGE,"Found shadow limit plane node"));
				/* if the max shadow limit planes reached - silently skip */
				if(	ShadowLimitPlanes.nNumShadLimPlanes < SGL_MAX_SHAD_LIM_PLANES)
				{
				  pShadNode = (SHAD_LIM_NODE_STRUCT *)pNode;
				  /* transform plane and store in array */
				  RnTransformBasicPlanes(&(pShadNode->plane_data),
					 1,
					 pState->pTransformState, 
					 &(ShadowLimitPlanes.TransShadLimPlanes[ShadowLimitPlanes.nNumShadLimPlanes]));

				  /* increment count */
				  ShadowLimitPlanes.nNumShadLimPlanes++;
				}

				break;
		    }


			/*
			// Was this a deleted item? If so skip it
			*/
			case nt_dummy:
			{
				DPF((DBG_MESSAGE,"Skipping dummy node"));
				break;
			}


			/*
			// If an invalid case, then there's been a programming
			// error !!!!
			*/
			default:
				ASSERT(FALSE)
				break;

		}/*end switch*/
		
		

		/*
		// move to the next list node
		*/
		pNode = pNode->next_node;
	
	} /*end while*/	


	return error;
}


/**************************************************************************
 * Function Name  : RnTraverseDisplayList
 * Inputs         : pList - pointer to a display list
 *
 * Outputs        : None
 * Returns        : an sgl error value.
 * Global Used    : Display list, name table, hardware parameter managers etc.
 *
 * Description    : Recursively traverses the display list using the supplied
 *					state information, updating the state information and producing
 *					the "hardware parameters" for rendering.
 *
 *					It is assumed that the transformation for the camera etc
 *					has already been set up.
 *
 *					This routine handles the preserving of state variables - i.e.
 *					stacking variables before calling specialised node handling
 *					routines where necesary.
 *
 *					It returns the first "error /warning" encountered
 **************************************************************************/

int RnTraverseDisplayList( const LIST_NODE_STRUCT   *pList, 
						   const CAMERA_NODE_STRUCT *pCamera)
{
	/*
	// Declare the first stack frame
	*/
	MASTER_STATE_STRUCT	FirstState;
	LOCAL_PROJECTION_STRUCT *pLocalProjMat;	
	/*
	// Error result
	*/
	int error;

	
	sgl_bool DummyBool;

	SGL_TIME_START(DATABASE_TRAVERSAL_TIME)
	/*
	// Initialise the states
	// Note: Initialise the save flags so that we dont bother saving the
	// state on the first pass.
	*/
	DPF((DBG_MESSAGE,"Initialising State Stacks"));
	FirstState.pMaterialState  	= pMaterialStackBase;
	FirstState.pTransformState	= pTransformStackBase;
	FirstState.pLightsState		= pLightsStackBase;
	FirstState.pQualityState	= pQualityStackBase;
	FirstState.pCollisionState	= pCollisionStackBase;
	FirstState.pInstanceSubState= pInstanceSubStackBase;

	FirstState.saveFlags		= 0;
	InitMasterState(&FirstState);

	/*
	// Initialise other rendering globals
	*/
	RnGlobalSetCurrentLightSlot(0);
	/* set to 1 to differentiate from 0.
	 */
	current_trans_set_id = 1;

	/* initialise shadow limiting planes - this is a global */
	ShadowLimitPlanes.nNumShadLimPlanes = 0;

	/*
	// The current projection matrix is invalid
	// Also take a guess at what size texture to set up for.
	*/
	pLocalProjMat = RnGlobalGetLocalProjMat();
	pLocalProjMat->valid = FALSE;
	pLocalProjMat->LastTextureSize = 128;

	/*
	// We aint seen a light volume yet...
	*/
	doneALightVol = FALSE;

	/*
	// If there isn't a LIST then we are rendering with a camera.
	*/
	if(pList == NULL)
	{
		DPF((DBG_MESSAGE, "USING Position of Camera..."));
		ASSERT(pCamera->node_hdr.n16_node_type == nt_camera);

		/*
		// First determine the initial transform from the camera's
		// viewpoint
		*/
		RnGetCameraTransform(pCamera, FirstState.pTransformState);

		/*
		// Now go to the top of the display list
		*/
		pList = pCamera->pparent;
		while(pList->pparent !=NULL )
		{
			DPF((DBG_MESSAGE, "Going up to parent...."));
			pList = pList->pparent;
		}/*end while*/
	}/*end if*/
	else
	{
		SetIdentityMatrix(RnGlobalGetAbsoluteCoordTransform());
	}



	/*
	// If we have any cached textures, then we need to make a preliminary pass
	// of the display list
	*/
	if(nCachedTextures)
	{
		void * pCachedTexture;


		/*
		// Because we need to return the global state to the way it was, we'll
		// set the save flags to preserve everything
		*/
		FirstState.saveFlags = ALL_STATE_SAVE_FLAGS;

		/*
		// Reset the usage on the cached textures
		*/
		ResetCachedTextureUsage();

		/*
		// traverse the display list and work out what is needed
		//
		// Pass NULL in as the pointer to the active 
		*/
		pCachedTexture = NULL;
		RnTextureCacheTraverse( pList,
								&pCachedTexture,
								&FirstState,
								MAX_DEPTH_OF_TRAVERSAL);

		/*
		// Report which ones are used by the display list to the user, and
		// let them sort out what to do
		*/
		ReportCachedTexturesToUser();


		/*
		// reset the save flags to zero again
		*/
		FirstState.saveFlags = 0;
		
	}




	/*
	// Traverse the database
	*/
	error = RnRecursiveTraverse( pList, &FirstState, MAX_DEPTH_OF_TRAVERSAL,
									&DummyBool);

	SGL_TIME_STOP(DATABASE_TRAVERSAL_TIME)
	return error;

}


/*
// End of file
*/
