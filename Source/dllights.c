/******************************************************************************
 * Name : dllights.c
 * Title : 
 * Author : Stel Michael
 * Created : 6/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : SGL Routines for creating and editing light nodes + utility routines
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dllights.c,v $
 * Revision 1.24  1997/06/04  15:32:02  gdc
 * stopped compiler warnings
 *
 * Revision 1.23  1997/05/13  17:51:29  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.22  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.21  1997/04/30  19:18:17  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.20  1997/04/29  14:42:31  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.19  1997/04/24  20:48:22  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.18  1997/04/02  21:26:47  mjg
 * Updated calling conventions.
 *
 * Revision 1.17  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.16  1997/02/21  19:17:59  mjg
 * Put the Green and the Blue values in the colour clamping.
 *
 * Revision 1.15  1997/02/05  13:09:06  sjf
 * 1) Fixed compiler warning .. might have been a bug with invalid
 * operator precedence
 * 2) Fixed incorrect clamping of light colours .. we weren't checking
 * for illegal negative values.
 *
 * Revision 1.14  1996/12/03  10:39:13  gdc
 * added a check for a NULL pointer in sgl_switch_light which was causing
 * problem in A5(ArtDink)
 *
 * Revision 1.13  1996/11/14  15:41:18  erf
 * Changed DL_NODE_STRUCT to LIGHT_SWITCH_NODE_STRUCT in sgl_switch_light().
 *
 * Revision 1.12  1996/11/11  14:13:39  gdc
 * added a check in sgl_switch_light to avoid duplication of nodes
 *
 * Revision 1.11  1996/07/11  16:39:07  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.10  1996/01/09  12:01:11  jrc
 * Removed 'coloured' flag from all functions and replaced with auto-detection
 * of grey lights.
 *
 * Revision 1.9  1995/09/25  18:20:01  sjf
 * Added multi shadow stuff.
 *
 * Revision 1.8  1995/08/28  16:51:57  sjf
 * 1) Made a few internal functions static
 * 2) Changed light concentration calc.
 *
 * Revision 1.7  1995/08/04  14:33:25  sm
 * subtracted one from concentration value
 *
 * Revision 1.6  1995/08/02  15:15:21  sm
 * fixed switch light prototype
 *
 * Revision 1.5  1995/06/14  12:32:58  sm
 * was normalising pos for some reason
 *
 * Revision 1.4  1995/06/08  13:03:58  sm
 * Added module id
 *
 * Revision 1.3  1995/06/08  12:45:13  sm
 * Added delete routines
 *
 * Revision 1.2  1995/06/07  17:19:58  sm
 * added sgl_switch_light and sgl_position_light routines
 * also added bit to normalise pos and dir vectors and to
 * clamp light colour
 *
 * Revision 1.1  1995/06/07  12:03:05  sm
 * Initial revision
 *
 *
 *****************************************************************************/


#define MODULE_ID MODID_DLLIGHTS


#include <math.h>
#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "sgl_init.h"
#include "sgl_math.h"

/*
// Define the Max colour component we are likely to have
*/
#define COLOUR_CLAMP    10.0f


/**************************************************************************
 * Function Name  : DlDeleteLightPosNodeRefs
 * Inputs Only    : pNode	-	pointer to light position node
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The name table.
 * Description    : If a vailid position node is given, the light node
					it refers to is made to point to NULL
 *					
 *
 **************************************************************************/



void 	DlDeleteLightPosNodeRefs(DL_NODE_STRUCT	 * pNode)
{
	LIGHT_NODE_STRUCT 		* litNode;
	LIGHT_POS_NODE_STRUCT 	* posNode;
	int	name;


	/*
		get easier access to the list pos node...
	*/
	posNode = (LIGHT_POS_NODE_STRUCT *) pNode;

	name = posNode->light_name;


	/* an invalid name means this position node has been overridden
	   by a later defined position node */

	if (name!=NM_INVALID_NAME)
	{
		/* a valid name means that the light node contains a reference to this node
		   and it must be set to null */
			

		/* Make sure that the name in light pos node is the name of a light */

    	ASSERT ( GetNamedItemType(dlUserGlobals.pNamtab, name) == nt_light )
	 
	

		/*	get reference to named light from the name table
			and set light position reference to NULL */

    	litNode = GetNamedItem(dlUserGlobals.pNamtab, name);

		litNode->plight_position=NULL;

	}
}








/**************************************************************************
 * Function Name  : DlDeleteLightSwitchNodeRefs
 * Inputs Only    : pointer to light switch node
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The name table.
 * Description    : Decrements the reference count on the light mentioned in the
 *					the name table.
 *
 **************************************************************************/


void	DlDeleteLightSwitchNodeRefs(DL_NODE_STRUCT	 * pNode)
{
	LIGHT_SWITCH_NODE_STRUCT	* lsNode;

	/*
		get easier access to the light switch node...
	*/
	lsNode = (LIGHT_SWITCH_NODE_STRUCT *) pNode;

	/*
		decrement the usage count for this light
	*/
	DecNamedItemUsage(dlUserGlobals.pNamtab,
					lsNode->light_name);
	
}



/**************************************************************************
 * Function Name  : DlDeleteLightNodeRefs
 * Inputs Only    : pointer to light node
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The name table.
 * Description    : Cleans up last reference to light node from position node
					if node is defined.
 *					
 *
 **************************************************************************/


void	DlDeleteLightNodeRefs(DL_NODE_STRUCT	* pNode)
{
	LIGHT_NODE_STRUCT 		* litNode;


	/*
		get easier access to the light switch node...
	*/
	litNode = (LIGHT_NODE_STRUCT *) pNode;


	if (litNode->plight_position!=NULL)
	{
		/* must nullify the currently defined position node
		   so that it can be deleted cleanly */

		litNode->plight_position->light_name=NM_INVALID_NAME;
	}

	/* NOTE : a much cleaner way would be to actually delete the position node
	   but what the heck! wait for version 1.1 !!! */
	
}






/**************************************************************************
 * Function Name  : SetAmbientLight   LOCAL FUNCTION ONLY
 * Inputs         : name  			- light name 
				  : colour			- colour of ambient light
				  : relative		- Specifies whether ambient colour is 
									  relative to the last ambient defined
 * Outputs        : 
 * Input/Output	  : pTranNode		- light node being initialised
 * Returns        : None
 * Global Used    : None
 * Description    : Sets the node header fields for ambient light
 *				   
 **************************************************************************/

static void SetAmbientLight( LIGHT_NODE_STRUCT * pLightNode,
							 int name, 
							 sgl_colour colour, 
							 sgl_bool Relative,
							 sgl_bool StoreHeader)
{
	int Rel,Col;


	/* store header information  */

	if (StoreHeader)
	{
		pLightNode->node_hdr.n16_node_type = (sgl_int16) nt_light;
		pLightNode->node_hdr.n16_name	  = (sgl_int16) name;
		pLightNode->node_hdr.next_node	  = NULL;
	}

	/* store light info */

	Rel = (Relative) ? ambient_relative : 0 ;
	Col = (colour[0] == colour[1] && colour[1] == colour[2]) ? 0 : coloured;

	pLightNode->flags = (ambient_light_type | Rel ) | Col;

	pLightNode->colour[0]= CLAMP(colour[0], 0.0f, COLOUR_CLAMP);
	pLightNode->colour[1]= CLAMP(colour[1], 0.0f, COLOUR_CLAMP);
	pLightNode->colour[2]= CLAMP(colour[2], 0.0f, COLOUR_CLAMP);

}




/**************************************************************************
 * Function Name  : SetParallelLight  LOCAL FUNCTION ONLY
 * Inputs         : name  			- light name 
				  : colour			- colour of ambient light
					direction		- direction of light source
					Shadows			- specifies whether or not
									  the light casts shadows
					Highlights	   	- specifies whether or not
									  light can produce smooth
									  highlights.
 * Outputs        : 
 * Input/Output	  : pTranNode		- light node being initialised
 * Returns        : None
 * Global Used    : None
 * Description    : Sets the node header fields and the light struct fields
					for a parallel light
 *				   
 **************************************************************************/

static void SetParallelLight( LIGHT_NODE_STRUCT * pLightNode,
							  int name, 
							  sgl_colour colour, 
							  sgl_vector direction,
							  sgl_bool Shadows, 
							  sgl_bool Highlights, 
							  sgl_bool StoreHeader)
{
	int Col;
	int Shd;
	int Hlt;

	/* store header information  */

	if (StoreHeader)
	{
		pLightNode->node_hdr.n16_node_type = (sgl_int16) nt_light;
		pLightNode->node_hdr.n16_name	  = (sgl_int16) name;
		pLightNode->node_hdr.next_node	  = NULL;
	}

	/* store light info */

	Shd = (Shadows) ? casts_shadows : 0;
	Col = (colour[0] == colour[1] && colour[1] == colour[2]) ? 0 : coloured;
	Hlt = (Highlights) ? smooth_highlights : 0;

	pLightNode->flags = ( ( ( parallel_light_type | Shd ) | Col ) | Hlt);

	pLightNode->colour[0]= CLAMP(colour[0], 0.0f, COLOUR_CLAMP);
	pLightNode->colour[1]= CLAMP(colour[1], 0.0f, COLOUR_CLAMP);
	pLightNode->colour[2]= CLAMP(colour[2], 0.0f, COLOUR_CLAMP);

	VecNormalise(direction);

	pLightNode->direction[0]=direction[0];
	pLightNode->direction[1]=direction[1];
	pLightNode->direction[2]=direction[2];

}





/**************************************************************************
 * Function Name  : SetPointLight
 * Inputs         : name  			- light name 
				  : colour			- colour of ambient light
					direction		- direction of light source
					position		- position of light source
					concentration   - spot light concentration
					Shadows			- specifies whether or not
									  the light casts shadows
					Highlights	   	- specifies whether or not
									  light can produce smooth
									  highlights.
 * Outputs        : 
 * Input/Output	  : pTranNode		- light node being initialised
 * Returns        : None
 * Global Used    : None
 * Description    : Sets the node header fields and the light struct fields
					for a point light
 *				   
 **************************************************************************/

static void SetPointLight( LIGHT_NODE_STRUCT * pLightNode,
						   int name, 
						   sgl_colour colour, 
						   sgl_vector direction,
						   sgl_vector position, 
						   int concentration, 
						   sgl_bool Shadows, 
						   sgl_bool Highlights, 
						   sgl_bool StoreHeader)
{
	int Col;
	int Shd;
	int Hlt;

	/* store header information  */

   	if (StoreHeader)
	{
		pLightNode->node_hdr.n16_node_type = (sgl_int16) nt_light;
		pLightNode->node_hdr.n16_name	  = (sgl_int16) name;
		pLightNode->node_hdr.next_node	  = NULL;
	}

	/* store light info */

	Shd = (Shadows) ? casts_shadows : 0;
	Col = (colour[0] == colour[1] && colour[1] == colour[2]) ? 0 : coloured;
	Hlt = (Highlights) ? smooth_highlights : 0;

	pLightNode->flags =  point_light_type | Shd  | Col | Hlt;

	pLightNode->colour[0]= CLAMP(colour[0], 0.0f, COLOUR_CLAMP);
	pLightNode->colour[1]= CLAMP(colour[1], 0.0f, COLOUR_CLAMP);
	pLightNode->colour[2]= CLAMP(colour[2], 0.0f, COLOUR_CLAMP);

	VecNormalise(direction);

	pLightNode->direction[0]=direction[0];
	pLightNode->direction[1]=direction[1];
	pLightNode->direction[2]=direction[2];

	pLightNode->position[0]=position[0];
	pLightNode->position[1]=position[1];
	pLightNode->position[2]=position[2];

	/*
	// If the light shines everywhere
	*/
	if(concentration<=0)
	{
		pLightNode->concentration=0;
	}
	/*
	// else if the concentration is 1, then we dont have to raise the
	// anything to power function.. in fact our approximation doesn't
	// work, so handle as a special case.
	*/
	else if(concentration==1)
	{
		pLightNode->concentration=1;
	}
	/*
	// Else there is a reasnable concentration factor. Compute an
	// "equivalent" multiplier for our approximation of the 
	// power function
	*/
	else
	{
		pLightNode->concentration=concentration;

		pLightNode->log2concentration = (float)log(concentration) / (float)log(2.0f);

	}/*end if concentration*/
		

}



/**************************************************************************
 * Function Name  : sgl_create_ambient_light
 * Inputs         : generate_name 
				  : colour			- colour of ambient light
				  : relative		- Specifies whether ambient colour is 
									  relative to the last ambient defined
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : if generate_name is TRUE and function is successful
					light name is returned, otherwise err is returned
					if generate_name is FALSE return value has no meaning
 * Global Used    : dlUserGlobals structure
 * Description    : adds an ambient light node to the display list
 *
 **************************************************************************/


int	CALL_CONV sgl_create_ambient_light( sgl_bool generate_name,
							  			sgl_colour colour,
										sgl_bool relative )
{
	int error = sgl_no_err;
	int name  = NM_INVALID_NAME;

	LIGHT_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/

		error = sgl_err_failed_init;
		SglError(error);

		return error;
	}
#endif
	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();

	/* 
		Create light node 
	*/


	pNode = NEW(LIGHT_NODE_STRUCT);

	if(pNode == NULL)
	{
		/*
			Abort	   
		*/
		error = sgl_err_no_mem;
		SglError(error);

		return error;
	} 


	/*
 		If we need a name, generate one, adding the item to
		the name table at the same time.
	*/

	if (generate_name)
	{
		name = AddNamedItem(dlUserGlobals.pNamtab,
			   				pNode,
							nt_light);
		/*
		 	If there were no free spaces, then the name will
		 	contain an error value (i.e.negative)
		
			In this situation TIDY UP and ABORT.
		*/
		if(name < 0)
		{
			error = name;
			name = NM_INVALID_NAME;

			SGLFree(pNode);
			SglError(error);
			return error;
		}
	} /*end if generate name */


	SetAmbientLight(pNode, name, colour, relative, TRUE);

	pNode->plight_position=NULL;

	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);


	SglError(error);

	return name;
}






/**************************************************************************
 * Function Name  : sgl_create_parallel_light
 * Inputs         : generate_name 
				  : colour			- colour of ambient light
					direction		- direction of light source
					Shadows			- specifies whether or not
									  the light casts shadows
					Highlights	   	- specifies whether or not
									  light can produce smooth
									  highlights.
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : if generate_name is TRUE and function is successful
					light name is returned, otherwise err is returned
					if generate_name is FALSE return value has no meaning
 * Global Used    : dlUserGlobals structure
 * Description    : adds an ambient light node to the display list
 *
 **************************************************************************/

int	CALL_CONV sgl_create_parallel_light( sgl_bool generate_name,
										 sgl_colour colour,
										 sgl_vector direction,
										 sgl_bool casts_shadows,
										 sgl_bool smooth_highlights )
{
	int error = sgl_no_err;
	int name  = NM_INVALID_NAME;

	LIGHT_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/

		error = sgl_err_failed_init;
		SglError(error);

		return error;
	}
#endif
	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();

	/* 
		Create light node 
	*/


	pNode = NEW(LIGHT_NODE_STRUCT);

	if(pNode == NULL)
	{
		/*
			Abort	   
		*/
		error = sgl_err_no_mem;
		SglError(error);

		return error;
	} 


	/*
 		If we need a name, generate one, adding the item to
		the name table at the same time.
	*/

	if (generate_name)
	{
		name = AddNamedItem(dlUserGlobals.pNamtab,
			   				pNode,
							nt_light);
		/*
		 	If there were no free spaces, then the name will
		 	contain an error value (i.e.negative)
		
			In this situation TIDY UP and ABORT.
		*/
		if(name < 0)
		{
			error = name;
			name = NM_INVALID_NAME;

			SGLFree(pNode);
			SglError(error);
			return error;
		}
	} /*end if generate name */

	SetParallelLight(pNode, name, colour, direction, casts_shadows, smooth_highlights, TRUE);


	pNode->plight_position=NULL;


	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);

	SglError(error);

	return name;
}



/**************************************************************************
 * Function Name  : sgl_create_point_light
 * Inputs         : generate_name 
				  : colour			- colour of ambient light
					direction		- direction of light source
					position		- position of light source
					concentration   - spot light concentration
					Shadows			- specifies whether or not
									  the light casts shadows
					Highlights	   	- specifies whether or not
									  light can produce smooth
									  highlights.
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : if generate_name is TRUE and function is successful
					light name is returned, otherwise err is returned
					if generate_name is FALSE return value has no meaning
 * Global Used    : dlUserGlobals structure
 * Description    : adds an ambient light node to the display list
 *
 **************************************************************************/


int	CALL_CONV sgl_create_point_light( sgl_bool generate_name,
									  sgl_colour colour,
									  sgl_vector direction,
									  sgl_vector position,
									  int concentration,
									  sgl_bool casts_shadows,
									  sgl_bool smooth_highlights )
{
	int error = sgl_no_err;
	int name  = NM_INVALID_NAME;

	LIGHT_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/

		error = sgl_err_failed_init;
		SglError(error);

		return error;
	}
#endif
	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();

	/* 
		Create light node 
	*/


	pNode = NEW(LIGHT_NODE_STRUCT);

	if(pNode == NULL)
	{
		/*
			Abort	   
		*/
		error = sgl_err_no_mem;
		SglError(error);

		return error;
	} 


	/*
 		If we need a name, generate one, adding the item to
		the name table at the same time.
	*/

	if (generate_name)
	{
		name = AddNamedItem(dlUserGlobals.pNamtab,
			   				pNode,
							nt_light);
		/*
		 	If there were no free spaces, then the name will
		 	contain an error value (i.e.negative)
		
			In this situation TIDY UP and ABORT.
		*/
		if(name < 0)
		{
			error = name;
			name = NM_INVALID_NAME;

			SGLFree(pNode);
			SglError(error);
			return error;
		}
	} /*end if generate name */

	SetPointLight(pNode, name, colour, direction, position, concentration, casts_shadows, smooth_highlights, TRUE);

	pNode->plight_position=NULL;

	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);

	SglError(error);

	return name;
}




/**************************************************************************
 * Function Name  : sgl_set_ambient_light
 * Inputs         : name			- light name 
				  : colour			- colour of ambient light
				  : relative		- Specifies whether ambient colour is 
									  relative to the last ambient defined
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : dlUserGlobals structure
 * Description    : sets ambient light node data
 *
 **************************************************************************/


void CALL_CONV sgl_set_ambient_light( int name, sgl_colour colour,
									  sgl_bool relative )
{
	LIGHT_NODE_STRUCT * pNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif


	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();



	/* Make sure that given name is the name of a light */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, name) != nt_light )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}


	/*	get reference to light from the name table */

    pNode = GetNamedItem(dlUserGlobals.pNamtab, name);

	SetAmbientLight(pNode, name, colour, relative, FALSE);

	SglError(sgl_no_err);

}



/**************************************************************************
 * Function Name  : sgl_set_parallel_light
 * Inputs         : name			- light name 
				  : colour			- colour of point light
					direction		- direction of light source
					Shadows			- specifies whether or not
									  the light casts shadows
					Highlights	   	- specifies whether or not
									  light can produce smooth
									  highlights.
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : dlUserGlobals structure
 * Description    : sets parallel light node data
 *
 **************************************************************************/


void CALL_CONV sgl_set_parallel_light( int name, sgl_colour colour,
									   sgl_vector direction,
							  		   sgl_bool casts_shadows,
									   sgl_bool smooth_highlights )
{
	LIGHT_NODE_STRUCT * pNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();



	/* Make sure that given name is the name of a light */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, name) != nt_light )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}


	/*	get reference to light from the name table */

    pNode = GetNamedItem(dlUserGlobals.pNamtab, name);

	SetParallelLight(pNode, name, colour, direction, casts_shadows, smooth_highlights,FALSE);

	SglError(sgl_no_err);

}


/**************************************************************************
 * Function Name  : sgl_set_point_light
 * Inputs         : name			- light name 
				  : colour			- colour of point light
					direction		- direction of light source
					position		- position of light source
					concentration   - spot light concentration
					Shadows			- specifies whether or not
									  the light casts shadows
					Highlights	   	- specifies whether or not
									  light can produce smooth
									  highlights.
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : dlUserGlobals structure
 * Description    : sets point light node data
 *
 **************************************************************************/

void CALL_CONV sgl_set_point_light( int name, sgl_colour colour,
									sgl_vector direction,
					  				sgl_vector position,									int concentration,
									sgl_bool casts_shadows,
									sgl_bool smooth_highlights )
{
	LIGHT_NODE_STRUCT * pNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();

	/* Make sure that given name is the name of a light */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, name) != nt_light )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}


	/*	get reference to light from the name table */

    pNode = GetNamedItem(dlUserGlobals.pNamtab, name);

	SetPointLight(pNode, name, colour, direction, position, concentration, casts_shadows, smooth_highlights, FALSE);

	SglError(sgl_no_err);
}



/**************************************************************************
 * Function Name  : sgl_position_light
 * Inputs         : name			- name of light being positioned 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : dlUserGlobals structure
 * Description    : positions a specified light
 *
 **************************************************************************/


void CALL_CONV sgl_position_light( int name )
{
	LIGHT_NODE_STRUCT 		* litNode;
	LIGHT_POS_NODE_STRUCT 	* posNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();



	/* Make sure that given name is the name of a light */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, name) != nt_light )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}


	/* 
		Create a light pos node 
	*/

	posNode = NEW(LIGHT_POS_NODE_STRUCT);

	if(posNode == NULL)
	{
		/*
			Abort	   
		*/
		SglError(sgl_err_no_mem);

		return;
	} 

	/* store header information  */

	posNode->node_hdr.n16_node_type = (sgl_int16) nt_light_pos;
	posNode->node_hdr.n16_name	  = (sgl_int16) NM_INVALID_NAME;
	posNode->node_hdr.next_node	  = NULL;

	posNode->light_name=name;


	/* set reference to point to the current list so
	   that we can backtrack up the list during traversal */

	posNode->pparent_list=dlUserGlobals.pCurrentList; 


	AppendNodeToList(dlUserGlobals.pCurrentList, posNode);


	/*	get reference to named light from the name table
		and set light position reference to point
		to newly creating position node  */

    litNode = GetNamedItem(dlUserGlobals.pNamtab, name);


	if (litNode->plight_position!=NULL)
	{
		/* must nullify the previously defined position node
		   so that it can be deleted cleanly */

		litNode->plight_position->light_name=NM_INVALID_NAME;
	}


   	litNode->plight_position=posNode;


	SglError(sgl_no_err);
}



void CALL_CONV sgl_switch_light( int name,
								 sgl_bool on,
								 sgl_bool casts_shadows,
								 sgl_bool smooth_highlights )
{
	int LiOn,ShOn,HiOn;

	LIGHT_SWITCH_NODE_STRUCT * pNode;
	LIGHT_SWITCH_NODE_STRUCT *pLast;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	/*
	   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
 
	/* Make sure that given name is the name of a light */
    if ( GetNamedItemType(dlUserGlobals.pNamtab, name) != nt_light )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}

	/* if the previous node in this list is a light switch node 
	** and if it refers to the same light rather than creating 
	** a new node just modify the previous one
	*/
	pLast = NULL;
	if(dlUserGlobals.pCurrentList)
	{
		if(dlUserGlobals.pCurrentList->plast)
		{
			pLast = (LIGHT_SWITCH_NODE_STRUCT *)dlUserGlobals.pCurrentList->plast;

		}
	}

	if( (pLast!=NULL) && 
		(pLast->node_hdr.n16_node_type == (sgl_int16) nt_light_switch) &&
	    (( (LIGHT_SWITCH_NODE_STRUCT *)pLast)->light_name==name ))
	{
		LiOn = (on) ? switch_light_on : 0 ;
		ShOn = (casts_shadows) ? switch_shadows_on : 0 ;
		HiOn = (smooth_highlights) ? switch_highlights_on : 0;

		pLast->light_switches = LiOn | ShOn | HiOn;
	}
	else
	{
		/*
		  increment the usage count for the named light
		  */
		IncNamedItemUsage(dlUserGlobals.pNamtab, name);

		pNode = NEW(LIGHT_SWITCH_NODE_STRUCT);
		
		/* store header information  */

		pNode->node_hdr.n16_node_type = (sgl_int16) nt_light_switch;
		pNode->node_hdr.n16_name	  = (sgl_int16) NM_INVALID_NAME;
		pNode->node_hdr.next_node	  = NULL;

		pNode->light_name=name;


		LiOn = (on) ? switch_light_on : 0 ;
		ShOn = (casts_shadows) ? switch_shadows_on : 0 ;
		HiOn = (smooth_highlights) ? switch_highlights_on : 0;

		pNode->light_switches = LiOn | ShOn | HiOn;


		AppendNodeToList(dlUserGlobals.pCurrentList, pNode);

	}
	SglError(sgl_no_err);

}


/**************************************************************************
 * Function Name  : sgl_pseudo_multishadows
 * Inputs         : name			- name of light being positioned 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : dlUserGlobals structure
 * Description    : Sets up/ turns off multi shadow light grouping.
 *
 **************************************************************************/

void CALL_CONV sgl_pseudo_multishadows ( sgl_bool enable,
							   			 sgl_colour colour )
{
	MULTI_SHADOW_NODE_STRUCT * pNode;

  	/*	
	//	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		//	We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	//   Tidy up current transforms etc...
	*/

	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	
	/*
	// Allocate the node
	*/
	pNode = NEW(MULTI_SHADOW_NODE_STRUCT);


	if(pNode == NULL)
	{
		/*
		//Abort	   
		*/
		SglError(sgl_err_no_mem);
	} 

	/*
	// Fill in the header details
	*/
	pNode->node_hdr.n16_node_type = (sgl_int16) nt_multi_shadow;
	pNode->node_hdr.n16_name	  = NM_INVALID_NAME;
	
	/*
	// Store the supplied values
	*/
	pNode->Enable = enable;
	VecCopy(colour, pNode->colour);

	/*
	// Add it to the end of the current list
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);
	

	/*
	// All OK
	*/
	SglError(sgl_no_err);
}

/*---------------------------- End of File -------------------------------*/

