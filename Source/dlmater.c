/*****************************************************************************
 *;++
 * Name           : $RCSfile: dlmater.c,v $
 * Title          : DLMATER.C
 * C Author       : Jim Page
 * Created        : 01/06/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :	SGL material functions
 *                   
 * Program Type   :	C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1997/10/16 11:06:30 $
 * $Revision: 1.40 $
 * $Locker:  $
 * $Log: dlmater.c,v $
 * Revision 1.40  1997/10/16  11:06:30  sxy
 * deliberatly over wrote previous version, as it could not be outdated
 * as it had already gone into a build - this is why there is a missing version
 * number
 *
 * Revision 1.38  1997/09/25  14:56:15  sxy
 * changed some DPFDEVs.
 *
 * Revision 1.37  1997/09/24  15:02:35  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.36  1997/07/18  13:57:31  gdc
 * PC'ified
 *
 * Revision 1.35  1997/05/13  17:52:00  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.34  1997/05/01  02:40:39  jop
 * Undid Ed's fixes
 *
 * Revision 1.33  1997/04/30  19:19:32  mjg
 * Replaced TEXTURE_MAP with HTEXTURE.
 *
 * Revision 1.32  1997/04/29  14:44:42  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.31  1997/04/24  20:53:10  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.30  1997/04/02  21:27:07  mjg
 * Updated calling conventions.
 *
 * Revision 1.29  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.28  1996/07/17  15:18:10  sjf
 * 1) Was missing include of sgl_init.h
 * 2) Fixed incorrect printout of shininess in the "dump" routine.
 * 3) got rid of unused variable.
 *
 * Revision 1.27  1996/07/11  16:39:14  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 * ,
 *
 * Revision 1.26  1996/07/08  04:22:36  sjf
 * 1) Fixed bug with deletion of material instances (uninitialised
 * value)
 * 2) Added routine to support debug output of display lists.
 *
 * Revision 1.25  1996/06/07  16:35:15  sjf
 * Put the material instancing parameters in a more consistent order.
 *
 * Revision 1.24  1996/06/06  23:03:00  sjf
 * Added (untested) material instancing
 *
 * Revision 1.23  1996/05/30  20:30:59  sjf
 * Added texture caching.
 *
 * Revision 1.22  1996/03/25  13:56:45  sjf
 * Fixed BUG (ID 3432) ; Setting texture to SGL_NULL_TEXTURE now turns off
 * texturing (as it is supposed to do).
 *
 * Revision 1.21  1996/03/25  12:21:57  jop
 * Fixed end-of-convex bug
 *
 * Revision 1.20  1995/09/28  10:34:42  pdl
 * removed the smap axis stuff.
 *
 * Revision 1.19  1995/09/01  16:05:17  pdl
 * changed the refractive index processing.
 *
 * Revision 1.18  1995/08/23  15:00:22  pdl
 * initialised the text_effect.
 *
 * Revision 1.17  1995/08/09  17:24:40  pdl
 * altered the opacity processing.
 *
 * Revision 1.16  1995/08/09  16:24:44  sm
 * *** empty log message ***
 *
 * Revision 1.15  1995/08/01  15:43:18  sm
 * changed spec value again
 *
 * Revision 1.15  1995/08/01  15:43:18  sm
 * changed spec value again
 *
 * Revision 1.14  1995/07/30  21:40:33  sm
 * fixed bug in set_texture effect
 *
 * Revision 1.13  1995/07/30  18:48:55  sjf
 * Had a quiet clash of identifiers - (with is_local) . Did a major
 * renaming tofix this.
 *
 * Revision 1.12  1995/07/30  16:00:41  sm
 * changed specular stuff for power of four
 *
 * Revision 1.11  1995/07/28  18:25:54  sm
 * made some changes to highlight calc
 *
 * Revision 1.10  1995/07/28  16:39:51  pdl
 * added texture_on flag.
 *
 * Revision 1.9  1995/07/24  17:07:37  pdl
 * added the texture removal stuff.
 *
 * Revision 1.8  1995/07/23  17:56:49  pdl
 * replaced the texture name with the control word.
 * fixed a flip UV bug.
 *
 * Revision 1.7  1995/07/23  17:26:41  jop
 * commented out TexasMapOpacityFloatToInt function - it's nor there yet
 *
 * Revision 1.6  1995/07/06  10:19:57  sjf
 * Removed unused variable.
 *
 * Revision 1.5  1995/06/05  15:56:35  jop
 * Added support for current mesh as well as current convex
 *
 * Revision 1.3  1995/06/02  13:24:25  jop
 * Added smap and omap functions and added local polyhedron creation
 *
 * Revision 1.2  1995/06/01  19:35:58  jop
 * Loads more stuff
 *
 *
 *;--
 *****************************************************************************/
/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                				Includes		                       	 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

#include <float.h>

#define MODULE_ID MODID_DLMATER

#include "sgl_defs.h"
#include "sgl.h"
#include "sgl_init.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "texapi.h"
#include "dlnodes.h"	/* material structure definition */
#include "nm_intf.h"
#include "dlglobal.h"	/* current material ptr, etc */
/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                				Macros			                       	 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

#define CLAMP(x,min,max)  (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

#define NOT_CREATING_NEW_LOCAL	FALSE
/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                		static function prototypes	                     ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

static void 	TidyUpCurrentState (sgl_bool);
static int 		CheckCurrentMaterial (void);

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                			static functions		                     ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	
/*===========================================
 * Function:	TidyUpCurrentState
 *===========================================
 *
 * Scope:		static to this module
 *
 * Purpose:		Completes necessary 'current' definitions; only completes current
 *				convex object if the current material is NOT local.
 *
 * Params:		BOOL bCreatingNewLocal: TRUE if about to create a new local material, FALSE
 *										if not.
 *
 * Return:		void
 *
 * Globals accessed:    dlUserGlobals.pCurrentMaterial
 *						dlUserGlobals.pCurrentConvex
 *========================================================================================*/
static void TidyUpCurrentState (sgl_bool bCreatingNewLocal)
{
	/* complete the convex/mesh if the current material is NOT local, */

	if (dlUserGlobals.pCurrentConvex)
	{
		ASSERT (dlUserGlobals.pCurrentMesh == NULL);

		/* 
		// if we are NOT creating a new local material OR the current
		// material is NOT local, complete the current convex poly 
		*/

		if (!bCreatingNewLocal && 
		    !(dlUserGlobals.pCurrentMaterial && (dlUserGlobals.pCurrentMaterial->data_flags & mnf_is_local)))
		{
			DlCompleteCurrentConvex ();
		}
	}
	else if (dlUserGlobals.pCurrentMesh)
	{
		ASSERT (dlUserGlobals.pCurrentConvex == NULL);

		/* if we are NOT creating a new local material OR the current material is NOT
		// local, complete the current mesh 
		*/

		if (!bCreatingNewLocal && 
		    !(dlUserGlobals.pCurrentMaterial && (dlUserGlobals.pCurrentMaterial->data_flags & mnf_is_local)))
		{
			DlCompleteCurrentMesh ();
		}
	}

	/* always complete the current transform */
	
	DlCompleteCurrentTransform ();
}

/*===========================================
 * Function:	CheckCurrentMaterial
 *===========================================
 *
 * Scope:		static to this module
 *
 * Purpose:		Checks if there is a current material. If not, creates an anonymous
 *				one (non-local). If there is an error, it is returned raw.
 *
 * Params:		void
 *
 * Return:		int sgl_no_err if a current material is in place, sgl_err_xxx if there
 *				was an error creating a new one.
 *
 * Globals accessed:   dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
static int CheckCurrentMaterial (void)
{
	int nError;

	if (dlUserGlobals.pCurrentMaterial)
	{
		/* we are already in a material definition */

		nError = sgl_no_err;
	}
	else
	{
		/* no current material, so create an anonymous one */

		nError = sgl_create_material (FALSE, FALSE);

		/* it might return a name (despite being anonymous), so be safe ... */
		
		if (nError > 0)
		{
			nError = sgl_no_err;
		}
	}

	return (nError);
}

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                		SGL internal entry points	                     ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	
/*===========================================
 * Function:	DlCompleteCurrentMaterial
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		Completes a material definition; sets the current material ptr to NULL.
 *
 * Params:		void
 *
 * Return:		void
 *
 * Globals accessed:   dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void DlCompleteCurrentMaterial (void)
{
	if (dlUserGlobals.pCurrentMaterial)
	{
		/* finished with current material - set ptr to NULL */

		dlUserGlobals.pCurrentMaterial = NULL;
	}
}

/*===========================================
 * Function:	DlDeleteMaterialNodeRefs
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		'destructor' for material nodes; deletes texture wrap data if it exists
 *
 * Params:		DL_NODE_STRUCT *pNode: material node to clean up
 *
 * Return:		void
 *
 * Globals accessed:   none
 *========================================================================================*/
void DlDeleteMaterialNodeRefs (DL_NODE_STRUCT *pNode)
{
	MATERIAL_NODE_STRUCT *pMaterial = (MATERIAL_NODE_STRUCT *) pNode;
	
	/* if either of these ASSERTs fail, something is badly wrong in the list department */

	ASSERT (pMaterial);
	ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

	DPF ((DBG_VERBOSE, "deleting material structure (name = %d)", pMaterial->node_hdr.n16_name));

	if (pMaterial->pwrap_data)
	{
		/* if extra texture wrapping structure exists, delete it */

		SGLFree (pMaterial->pwrap_data);
		pMaterial->pwrap_data = NULL;
	}
}

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                			SGL API entry points	                     ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	
/*===========================================
 * Function:	sgl_create_material
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Creates and initialises a material node, adds it to the current display
 *				list and if necessary adds it to the name table. It is either 'local' to
 *				a polyhedron or mesh, allowing parts of these to have different material
 *				definitions in the middle, or 'non-local', in which case any non-material
 *				API function causes the termination of the material definition
 *
 * Params:		sgl_bool generate_name: TRUE if material is to be added to name table
 *				sgl_bool is_local: TRUE if the material is part of a poly/mesh definition
 *
 * Return:		int -ve error, or sgl_no_err/+ve name according to 'generate_name' param
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *						dlUserGlobals.pNamtab
 *						dlUserGlobals.pCurrentList
 *========================================================================================*/
int CALL_CONV sgl_create_material ( sgl_bool generate_name, 
									sgl_bool param_is_local )
{
	int nError;
	MATERIAL_NODE_STRUCT *pMaterial;
	sgl_bool bCreateLocalConvex = (param_is_local && (dlUserGlobals.pCurrentConvex != NULL));
	sgl_bool bCreateLocalMesh = (param_is_local && (dlUserGlobals.pCurrentMesh != NULL));

/* #if !WIN32 */
/* 	if (SglInitialise()) */
/* 	{ */
/* 		return SglError(sgl_err_failed_init); */
/* 	} */
/* #endif */

	TidyUpCurrentState (bCreateLocalConvex || bCreateLocalMesh);

	/* starting a new material, so complete the previous one if it exists */

	DlCompleteCurrentMaterial ();

	/* allocate a new material structure */
	
	pMaterial = SGLMalloc (sizeof (MATERIAL_NODE_STRUCT));

	/* did it work? */
	
	if (!pMaterial)
	{
		DPF ((DBG_ERROR, "sgl_create_material: SGLMalloc failed"));
		nError = sgl_err_no_mem;
	}
	else
	{
		pMaterial->node_hdr.n16_node_type = nt_material;

		if (generate_name)
		{
			int nName;

			/* add name to name table */
			
			ASSERT (dlUserGlobals.pNamtab);

			nName = AddNamedItem (dlUserGlobals.pNamtab, pMaterial, nt_material);

			if (nName < 0)
			{
				SGLFree (pMaterial);
			
				DPF ((DBG_ERROR, "sgl_create_material: couldn't add material block to name table"));
				nError = sgl_err_no_name;
			}
			else
			{
				/* the user wants the name returned */

				pMaterial->node_hdr.n16_name = nName;
				nError = nName;
			}
		}
		else
		{
			/* no name, no error */

			pMaterial->node_hdr.n16_name = NM_INVALID_NAME;
			nError = sgl_no_err;
		}

		if ((nError == sgl_no_err) || (nError > 0))
		{
			ASSERT (dlUserGlobals.pCurrentList);
			
			/* 
			// See if we are adding a local material to a poly/mesh, or just 
			// appending to the current list.
			// treat is_local with NO current poly/mesh as material ordinaire ... 
			 */

			if (bCreateLocalConvex || bCreateLocalMesh)
			{
				/* 
				// local convex polyhedron or mesh material - appropriate 
				// module handles it - it may fail! 
				*/

				int nLocalError;
				
				if (bCreateLocalConvex)
				{
					nLocalError = DlConvexAddLocalMaterial (pMaterial);
				}
				else
				{
					nLocalError = DlMeshAddLocalMaterial (pMaterial);
				}

				if (nLocalError != sgl_no_err)
				{
					/* romove from name table if necessary */

					if (generate_name)
					{
						DeleteNamedItem (dlUserGlobals.pNamtab, nError);
					}

					/* free the block */

					SGLFree (pMaterial);

					/* transmit error info back to caller */
					
					DPF ((DBG_ERROR, "sgl_create_material: error adding local material to convex/mesh"));
					nError = nLocalError;
				}
				else
				{
					/* set local material flag in struct */
					
					pMaterial->data_flags = mnf_is_local;
				}
			}
			else
			{
				/* not local - just append to current list */
				
				AppendNodeToList (dlUserGlobals.pCurrentList, pMaterial);
				pMaterial->data_flags = 0; /* not local */
			}

			/* chack that adding local material didn't fail */

			if ((nError == sgl_no_err) || (nError > 0))
			{
				/* 
					Everything is now cool, so initialise the structure. Flags are now set
					so all we need to do is ensure that the extra texture data ptr is NULL
				 */

				pMaterial->pwrap_data = NULL;

				/*zeroing text_effect is required*/

				pMaterial->text_effect=0;

				/* set the current material global to point to new one */

				dlUserGlobals.pCurrentMaterial = pMaterial;
			}
		}
	}

	return (SglError (nError));
}

/*===========================================
 * Function:	sgl_use_material_instance
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		References another named material as if it were defined at this
 *				point in the display list.
 *
 * Return:		Nothing.
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *						dlUserGlobals.pNamtab
 *						dlUserGlobals.pCurrentList
 *========================================================================================*/
void CALL_CONV sgl_use_material_instance ( int instanced_material,
										   sgl_bool param_is_local )
{
	int nError;
	MATERIAL_NODE_STRUCT *pMaterial;

	void * pNamedItem;
	int	   namedItemType;
	
	sgl_bool bCreateLocalConvex = (param_is_local && (dlUserGlobals.pCurrentConvex != NULL));
	sgl_bool bCreateLocalMesh = (param_is_local && (dlUserGlobals.pCurrentMesh != NULL));

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Check if we have been given a valid material name
	*/
	pNamedItem = GetNamedItemAndType(dlUserGlobals.pNamtab, instanced_material,
									&namedItemType);

	if((pNamedItem == NULL) || (namedItemType != nt_material))
	{
		SglError(sgl_err_bad_name);
		return; 
	}


	TidyUpCurrentState (bCreateLocalConvex || bCreateLocalMesh);

	/* starting a new material, so complete the previous one if it exists */

	DlCompleteCurrentMaterial ();

	/* allocate a new material structure */
	
	pMaterial = SGLMalloc (sizeof (MATERIAL_NODE_STRUCT));

	/* did it work? */
	
	if (!pMaterial)
	{
		DPF ((DBG_ERROR, "sgl_use_material_instance: SGLMalloc failed"));
		nError = sgl_err_no_mem;
	}
	else
	{
		pMaterial->node_hdr.n16_node_type = nt_material;
		pMaterial->node_hdr.n16_name = NM_INVALID_NAME;

		ASSERT (dlUserGlobals.pCurrentList);

		nError= sgl_no_err;

		/* 
		// See if we are adding a local material to a poly/mesh, or just 
		// appending to the current list.
		// treat is_local with NO current poly/mesh as material ordinaire ... 
		*/

		if (bCreateLocalConvex || bCreateLocalMesh)
		{
			/* 
			// local convex polyhedron or mesh material - appropriate 
			// module handles it - it may fail! 
			*/

			int nLocalError;
				
			if (bCreateLocalConvex)
			{
				nLocalError = DlConvexAddLocalMaterial (pMaterial);
			}
			else
			{
				nLocalError = DlMeshAddLocalMaterial (pMaterial);
			}

			if (nLocalError != sgl_no_err)
			{
				/* free the block */

				SGLFree (pMaterial);
				/* transmit error info back to caller */
				DPF ((DBG_ERROR,
					"sgl_use_material_instace: error adding local material"));
					nError = nLocalError;
			}
			else
			{
				/* set local material flag in struct */
					
				pMaterial->data_flags = mnf_is_local;
			}
		}
		else
		{
			/* not local - just append to current list */
				
			AppendNodeToList (dlUserGlobals.pCurrentList, pMaterial);
			pMaterial->data_flags = 0; /* not local */
		}

		/* check that adding local material didn't fail */

		if ((nError == sgl_no_err) || (nError > 0))
		{
			/*
			// Its all ok, so save the instance information
			*/
			pMaterial->data_flags |= mnf_instance_ref;
			pMaterial->instanced_material_name = instanced_material;

			/*
			// To make it safe when deleting material nodes, make sure
			// we have a NULL pointer to the texture wrapping data.
			*/
			pMaterial->pwrap_data = NULL;

		}
	
	}/*end if/else node allocation succeeded*/

	SglError (nError);
}

/*===========================================
 * Function:	sgl_modify_material
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Sets a named material up for modification if it exists. The material
 *				node is optionally cleared. If the material does not exist, the global
 *				error variables are set.
 *
 * Params:		int nName: material to modify
 *				sgl_bool bClearMaterial: TRUE if node is to be cleared
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *						dlUserGlobals.pNamtab
 *========================================================================================*/
void CALL_CONV sgl_modify_material ( int nName, sgl_bool bClearMaterial )
{
	int nError;
	MATERIAL_NODE_STRUCT *pMaterial;


	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	/* finish with current material if there is one */

	DlCompleteCurrentMaterial ();

	ASSERT (dlUserGlobals.pNamtab);
	
	/* get pointer to named item */

	if (GetNamedItemType (dlUserGlobals.pNamtab, nName) != nt_material)
	{
		DPFDEV ((DBG_WARNING, "sgl_modify_material: bad name %d - not a material, or doesn't exist", nName));
		nError = sgl_err_bad_name;
	}
	else
	{
		pMaterial = GetNamedItem (dlUserGlobals.pNamtab, nName);

		/* if it's in the name table, the pointer should be non-zero! */

		ASSERT (pMaterial);
		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);
		
		/* set up the global current material pointer */

		dlUserGlobals.pCurrentMaterial = pMaterial;

		if (bClearMaterial)
		{
			/* clear material flags, and dispose of texture wrap info */
			
			pMaterial->data_flags &= mnf_is_local;

			if (pMaterial->pwrap_data)
			{
				/* if extra texture wrapping structure exists, delete it */

				SGLFree (pMaterial->pwrap_data);
				pMaterial->pwrap_data = NULL;
			}
		}		

		nError = sgl_no_err;
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_ambient
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set the current material's ambient refectivity. Colour components
 *				are clamped between 0 and 1;
 *
 * Params:		sgl_colour cColour: new ambient reflectivity colour.
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_ambient ( sgl_colour cColour )
{
	int nError;

	ASSERT (cColour);

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_ambient: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

		/* set the 'ambient' flag */

		pMaterial->data_flags |= mnf_has_ambient;

		/* copy the colour */

		pMaterial->ambient_colour[0] = CLAMP (cColour[0], 0, 1);
		pMaterial->ambient_colour[1] = CLAMP (cColour[1], 0, 1);
		pMaterial->ambient_colour[2] = CLAMP (cColour[2], 0, 1);
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_diffuse
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set the current material's diffuse refectivity. Colour components
 *				are clamped between 0 and 1;
 *
 * Params:		sgl_colour cColour: new diffuse reflectivity colour.
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_diffuse ( sgl_colour cColour )
{
	int nError;

	ASSERT (cColour);

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_diffuse: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

		/* set the 'diffuse' flag */

		pMaterial->data_flags |= mnf_has_diffuse;

		/* copy the colour */

		pMaterial->diffuse_colour[0] = CLAMP (cColour[0], 0, 1);
		pMaterial->diffuse_colour[1] = CLAMP (cColour[1], 0, 1);
		pMaterial->diffuse_colour[2] = CLAMP (cColour[2], 0, 1);
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_specular
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set the current material's specular refectivity. Colour components
 *				are clamped between 0 and 1. This function pre-calculates some components
 *				for the specular highlight calculation. 
 *
 * Params:		sgl_colour cColour: new specular highlight colour
 *				int nShininess: 1 = piprick highlight - 1000 = very diffuse highlight
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_specular ( sgl_colour cColour, int nShininess )
{
	int nError;

	ASSERT (cColour);

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_specular: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

		/* set the 'specular' flag */

		pMaterial->data_flags |= mnf_has_specular;

		/* clamp and copy the colour */

		pMaterial->specular_colour[0] = CLAMP (cColour[0], 0, 1);
		pMaterial->specular_colour[1] = CLAMP (cColour[1], 0, 1);
		pMaterial->specular_colour[2] = CLAMP (cColour[2], 0, 1);

		/* zero or negative nShininess value indicates 'off' */

		if (nShininess > 0)
		{
			int k;

			/* clamp the shininess parameter */

			nShininess = CLAMP (nShininess, 1, 1000);

			/* calculate 'short cut' Shininess values; see TRDD0290.DOC */

			/* find integer part of log2 n by searching for MSB in nShininess value */

			for (k = 0; k < 9; ++k)
			{
				if (nShininess & (1 << k))
					pMaterial->specular_shininess = k;
			}

			/* specular_shininess = (log2 n) - 1 */
			/* specular_shininess_float = 2^((log2 n) - 1) */

			pMaterial->specular_shininess -= 1;

			if (pMaterial->specular_shininess<0) 
					pMaterial->specular_shininess=0;

   			pMaterial->specular_shininess_float = (float) (1 << pMaterial->specular_shininess) ;
		}
		else
		{
			/* a shininess value of 0 indicates no specular component */

			pMaterial->specular_shininess = 0;
			pMaterial->specular_shininess_float = 0.0f;
		}
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_glow
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set the current material's 'glow' colour. Colour components
 *				are clamped between 0 and 1. 
 *
 * Params:		sgl_colour cColour: new glow colour
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_glow ( sgl_colour cColour )
{
	int nError;

	ASSERT (cColour);

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_glow: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

		/* set the 'glow' flag */

		pMaterial->data_flags |= mnf_has_glow;

		/* copy the colour */

		pMaterial->glow_colour[0] = CLAMP (cColour[0], 0, 1);
		pMaterial->glow_colour[1] = CLAMP (cColour[1], 0, 1);
		pMaterial->glow_colour[2] = CLAMP (cColour[2], 0, 1);
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_opacity
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set the current material's opacity. Values are clamped between 0.0 and 1.0
 *
 * Params:		float fOpacity: 1.0 = opaque (off), 0.0 = transparent
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_opacity ( float fOpacity )
{
	int nError;

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_opacity: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

		/* set the 'opacity' flag */

		pMaterial->data_flags |= mnf_has_opacity;

		/* copy the opacity value */

		pMaterial->opacity = CLAMP (fOpacity, 0, 1);

		/* use hardware dependant mapping function for 'short cut' parameter */

		pMaterial->translucent_int = TexasMapOpacityFloatToInt (pMaterial->opacity);
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_texture_map
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Give a material a texture. The texture name is validated, and flip U and V
 *				bits are set here.
 *
 * Params:		int nTextureName: name of texture to set
 *				sgl_bool bFlipU: TRUE / FALSE - mirror in U texture direction
 *				sgl_bool bFlipV: TRUE / FALSE - mirror in V texture direction
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_texture_map ( int nTextureName,
									 sgl_bool bFlipU, sgl_bool bFlipV )
{
	int nError;
	int itemType;
	HTEXTURE hTexture;

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_texture_map: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);


		if(nTextureName==SGL_NULL_TEXTURE)
		{
			/* set the 'remove texture' flag, and clear the has texture one. */

			pMaterial->data_flags |= mnf_remove_texture;
			pMaterial->data_flags &= ~mnf_has_text_map;
			
		}
		else
		{

			/* check that the user has requested a valid texture */

			ASSERT (dlUserGlobals.pNamtab);

			hTexture=GetNamedItemAndType(dlUserGlobals.pNamtab,
										 nTextureName, 
										 &itemType);
			if ((itemType == nt_texture)||(itemType == nt_cached_texture))
			{
				
				/* set the 'texture map' flag */

				pMaterial->data_flags |= mnf_has_text_map;

				/* clear the 'remove texture' flag */

				pMaterial->data_flags &=~ mnf_remove_texture;

				/*THIS is the same as MASK_TEXTURE */
  				pMaterial->text_effect |= texture_on; 

				/* 
				// convert boolean parameters to hardware register bits 
				*/
				if (bFlipU)
				{
					pMaterial->text_effect |= mirror_u;
				}
				else
				{
					pMaterial->text_effect &=~mirror_u;
				}

				if (bFlipV)
				{
					pMaterial->text_effect |= mirror_v;
				}
				else
				{
					pMaterial->text_effect &=~mirror_v;
				}

				/*
				// if normal texture
				*/
				if(itemType == nt_texture)
				{
					/*
					// Set that it is NOT a cached texture
					*/
					pMaterial->pcached_texture = NULL;
					pMaterial->texture_control = hTexture->TSPTextureControlWord;
				}
				else
				{
					/*
					// Ok just save the pointer to the cached texture
					*/
					pMaterial->pcached_texture = hTexture;
				}
			
			}

			/*
			// Else we have been passed nonsense
			*/
			else
			{
				DPF ((DBG_WARNING, "sgl_set_texture_map: bad texture name %d", nTextureName));
				nError = sgl_err_bad_name;
			}
		}

	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_texture_effect
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set which reflectivity/light components of the material are affected
 *				by the texture map.
 *
 * Params:		sgl_bool fAffectAmbient: allow the texture map to vary the ambient reflectivity
 *				sgl_bool fAffectDiffuse: allow the texture map to vary the diffuse reflectivity
 *				sgl_bool fAffectSpecular: allow the texture map to vary the specular highlight
 *				sgl_bool fAffectGlow: allow the texture map to vary the glow colour
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_texture_effect (sgl_bool fAffectAmbient,
							 		   sgl_bool fAffectDiffuse,
							 		   sgl_bool fAffectSpecular,
							 		   sgl_bool fAffectGlow )
{
	int nError;

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_texture_effect: no current material"));
	}
	else
	{
		MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

		/* check that current material is to some extent valid */

		ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

		/* set the 'texture map' flag */

		pMaterial->data_flags |= mnf_has_text_effect;

		/* convert boolean parameters to material flags */
		
		if (fAffectAmbient)
		{
			pMaterial->text_effect |= affect_ambient;
		}
		else
		{
			pMaterial->text_effect &= ~affect_ambient;
		}

		if (fAffectDiffuse)
		{
			pMaterial->text_effect |= affect_diffuse;
		}
		else
		{
			pMaterial->text_effect &= ~affect_diffuse;
		}

		if (fAffectSpecular)
		{
			pMaterial->text_effect |= affect_specular;
		}
		else
		{
			pMaterial->text_effect &= ~affect_specular;
		}

		if (fAffectGlow)
		{
			pMaterial->text_effect |= affect_glow;
		}
		else
		{
			pMaterial->text_effect &= ~affect_glow;
		}
	}

	SglError (nError);
}

/*===========================================
 * Function:	sgl_set_smap
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set a material's texture S map parameters
 *
 * Params:		sgl_smap_types 	SMapType: planar, cylindrical, spherical
 *				float			fSU, fSV: scaling values for texture
 *				float			fOU, fOV: origin offset values for texture
 *				float			fR: radius for cylindrical/spherical maps
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_smap ( sgl_smap_types SMapType,
							  float fSU, float fSV,
							  float	fOU, float fOV,
							  float fR )
{
	int nError;


	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_smap: no current material"));
	}
	else
	{
		switch (SMapType)
		{
			case sgl_smap_plane:
			case sgl_smap_cylinder:
			case sgl_smap_sphere:
			{
				MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

				/* check that current material is to some extent valid */

				ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

				if (!pMaterial->pwrap_data)
				{
					ASSERT ((pMaterial->data_flags & (mnf_has_smap | mnf_has_omap)) == 0);

					pMaterial->pwrap_data = SGLMalloc (sizeof (TEXT_WRAP_STRUCT));
				}

				if (!pMaterial->pwrap_data)
				{
					DPF ((DBG_ERROR, "sgl_set_smap: SGLMalloc failed on text wrap structure"));
					nError = sgl_err_no_mem;
				}
				else
				{
					/* set the 'smap' flag */

					pMaterial->data_flags |= mnf_has_smap;

				 	/* copy the data to the texture wrap struct */

					pMaterial->pwrap_data->smap = SMapType;
					pMaterial->pwrap_data->su = CLAMP (fSU, (float) 0.00001, (float) FLT_MAX);
					pMaterial->pwrap_data->sv = CLAMP (fSV, (float) 0.00001, (float) FLT_MAX);
					pMaterial->pwrap_data->ou = fOU;
					pMaterial->pwrap_data->ov = fOV;
					pMaterial->pwrap_data->radius = fR;

				}
				break;
			}

			default:
			{
				DPFDEV ((DBG_WARNING, "sgl_set_smap: bad smap type 0x%x", SMapType));
				nError = sgl_err_bad_parameter;
				break;
			}
		}
	}

	SglError (nError);
}

/*===========================================
 * Function:	OMapType
 *===========================================
 *
 * Scope:		SGL API
 *
 * Purpose:		Set a material's texture O map parameters
 *
 * Params:		sgl_omap_types OMapType: object normal, inter. surf normal, reflected, transmitted
 *				float fRefractiveIndex: limited to >= 1.0
 *
 * Return:		void
 *
 * Globals accessed:	dlUserGlobals.pCurrentMaterial
 *========================================================================================*/
void CALL_CONV sgl_set_omap ( sgl_omap_types OMapType, 
							  float fRefractiveIndex )
{
	int nError;

	/*
	// Initialise the system if necessary
	*/
#if !WIN32
	if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return; 
	}
#endif
	TidyUpCurrentState (NOT_CREATING_NEW_LOCAL);

	nError = CheckCurrentMaterial ();

	if (nError != sgl_no_err)
	{
		DPF ((DBG_WARNING, "sgl_set_omap: no current material"));
	}
	else
	{
		switch (OMapType)
		{
			case sgl_omap_obj_normal:
			case sgl_omap_inter_normal:
			case sgl_omap_reflection:
			case sgl_omap_transmission:
			{
				MATERIAL_NODE_STRUCT *pMaterial = dlUserGlobals.pCurrentMaterial;

				/* check that current material is to some extent valid */

				ASSERT (pMaterial->node_hdr.n16_node_type == nt_material);

				if (!pMaterial->pwrap_data)
				{
					ASSERT ((pMaterial->data_flags & (mnf_has_smap | mnf_has_omap)) == 0);

					pMaterial->pwrap_data = SGLMalloc (sizeof (TEXT_WRAP_STRUCT));
				}

				if (!pMaterial->pwrap_data)
				{
					DPF ((DBG_ERROR, "sgl_set_omap: SGLMalloc failed on text wrap structure"));
					nError = sgl_err_no_mem;
				}
				else
				{
					/* set the 'smap' flag */

					pMaterial->data_flags |= mnf_has_omap;

				 	/* copy the data to the texture wrap struct */

					pMaterial->pwrap_data->omap = OMapType;
					pMaterial->pwrap_data->refrac_index = CLAMP (fRefractiveIndex, (float) 1.0, (float) FLT_MAX);
					/* the texture wrapping would like the reciprocal of the refractive index */

					pMaterial->pwrap_data->refrac_index = 1.0f/pMaterial->pwrap_data->refrac_index;
				}
				break;
			}

			default:
			{
				DPFDEV ((DBG_WARNING, "sgl_set_omap: bad omap type 0x%x", OMapType));
				nError = sgl_err_bad_parameter;
				break;
			}
		}
	}

	SglError (nError);
}


/**************************************************************************
 * Function Name  : DumpMaterial
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : 
 *	
 **************************************************************************/
static void DumpColour(FILE * outfile, sgl_vector col)
{
	fprintf(outfile, "R:%1.3f G:%1.3f B:%1.3f ", 
					col[0], col[1], col[2]);
}

void DumpMaterial(	MATERIAL_NODE_STRUCT* pNode, 
					FILE * outfile,
					char * pPaddingString)
{
	int MatFlags = pNode->data_flags;


	/*
	// If is a material instance
	*/
	if(MatFlags & mnf_instance_ref)
	{
		fprintf(outfile, "%sInstance of Material %d\n",
				 pPaddingString, pNode->instanced_material_name);
	}
	else
	{
		fprintf(outfile, "%sMaterial: Name:%d\n",
			 pPaddingString, pNode->node_hdr.n16_name);

		if (MatFlags & mnf_has_diffuse)
		{
			fprintf(outfile, "%s\tDiffuse:", pPaddingString);
			DumpColour(outfile, pNode->diffuse_colour);
			fprintf(outfile, "\n");
		}


   		if (MatFlags & mnf_has_ambient)
		{
			fprintf(outfile, "%s\tAmbient:", pPaddingString);
			DumpColour(outfile, pNode->ambient_colour);
			fprintf(outfile, "\n");
		}

		if (MatFlags & mnf_has_specular)
   		{	
			fprintf(outfile, "%s\tSpecular:", pPaddingString);
			DumpColour(outfile, pNode->specular_colour);
			fprintf(outfile, " Shininess:%d",pNode->specular_shininess );
			fprintf(outfile, "\n");
		}

		if (MatFlags & mnf_has_glow)
		{
			fprintf(outfile, "%s\tGlow:", pPaddingString);
			DumpColour(outfile, pNode->glow_colour);
			fprintf(outfile, "\n");
		}


   		if (MatFlags & mnf_remove_texture)
   		{
			fprintf(outfile, "%s\tTexture: Turned Off\n", pPaddingString);
		}


		if (MatFlags & mnf_has_text_map)
		{
			if(pNode->pcached_texture == NULL)
			{
				fprintf(outfile, "%s\tTexture: Set\n", pPaddingString);
			}
			else
			{
				fprintf(outfile, "%s\tTexture: Cached\n", pPaddingString);
			}
			
		}/*if has texture map*/

#if 0
		if ((pNode->pwrap_data)!=NULL)
		{
			/* a texture wrap block has been defined */
			CopyTexWrapData(pNode,stateMaterial,pTransformState);	
		}


		if (MatFlags & mnf_has_opacity)
		{
			stateMaterial->opacity=pNode->opacity;
		stateMaterial->translucent_int=pNode->translucent_int;

		 /* clear current value */
		stateMaterial->texas_precomp.LowWord &= ~MASK_GLOBAL_TRANS;
		stateMaterial->texas_precomp.LowWord |= 
		 (((pNode->translucent_int) << SHIFT_GLOBAL_TRANS) & MASK_GLOBAL_TRANS);

		/*
		// OK, only enable translucency if we have a texture AND either it
		// is a translucent texture OR the opacity is partial.
		*/
   		if( (stateMaterial->texas_precomp.LowWord & MASK_TEXTURE) &&
			 ( (pNode->opacity!=1.0f) || 
			   (stateMaterial->texas_precomp.TexAddress	& MASK_4444_555)))
		{
			stateMaterial->texas_precomp.LowWord |= MASK_TRANS;
		}
		else
		{
			stateMaterial->texas_precomp.LowWord &= ~MASK_TRANS;
		}

	}
#endif	


   		if (MatFlags & mnf_has_text_effect)
		{
			fprintf(outfile, "%s\tTexture Effect:", pPaddingString);
			if(pNode->text_effect & affect_ambient)
			{
				 fprintf(outfile," Ambi");
			}
			if(pNode->text_effect & affect_diffuse)
			{
				 fprintf(outfile," Diff");
			}
			if(pNode->text_effect & affect_specular)
			{
				 fprintf(outfile," Spec");
			}
			if(pNode->text_effect & affect_glow)
			{
				 fprintf(outfile, " Glow");
			}

			fprintf(outfile, "\n");
		}

	}/*end if real material or an instance*/

}

/* end of $RCSfile: dlmater.c,v $ */
