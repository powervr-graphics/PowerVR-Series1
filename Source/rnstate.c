/**************************************************************************
 * Name         : rnstate.c
 * Title        : Declaration of state variable stacks and initialisation
 *				  routines
 *				  of structures etc.
 * Author       : Simon Fenney
 * Created      : 09 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Defines the SGL state variables as suggested by TRDD027x.doc
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnstate.c,v $
 * Revision 1.13  1997/05/13  17:56:03  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.12  1997/04/30  19:21:12  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.11  1997/04/29  14:50:39  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.10  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.9  1996/07/11  16:40:54  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.8  1996/06/20  20:37:41  jop
 * For defensive purposes, initialised the state stacks to NULL.
 *
 * Revision 1.7  1996/05/08  14:56:46  sjf
 * Moved the #defines for the stack sizes out
 *
 * Revision 1.6  1995/07/14  13:05:01  sjf
 * Fixed spelling error wrt its .h file.
 *
 * Revision 1.5  1995/06/06  16:56:17  sjf
 * Fixed "spelling" errors in variable names after conversion to
 * VL C style standard.
 *
 * Revision 1.4  1995/05/26  12:38:37  sjf
 *  moved ANSI .h's to sgl_defs.h, + other common includes.
 *
 * Revision 1.3  1995/05/25  14:21:47  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.2  1995/05/16  09:37:35  sjf
 * I don't know where I left my brain when I wrote this, but I had completely
 * messed up the setting of errors in the initialisation routine.
 *
 * Revision 1.1  1995/05/11  14:11:03  sjf
 * Initial revision
 *
 * Revision 1.1  1995/05/09  16:17:38  sjf
 * Initial revision
 *
 *
 **************************************************************************/

#include <string.h>
#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h" 
#include "pvrosapi.h"
#include "sglmem.h"
#include "rnstate.h"

/*
// Declare types for the arrays of state values that will form a stack
//
// Currently these have hardwired sizes, but I guess they could be resized
// if the stack depth needed to grow. HOWEVER it would be difficult to do this
// during a render traversal. Possibly it would be best to abort the render
// and restart it...
*/
typedef MATERIAL_STATE_STRUCT	material_stack_type[MAX_MATERIAL_STATE_STACK];
typedef TRANSFORM_STRUCT		transform_stack_type[MAX_TRANSFORM_STATE_STACK];
typedef LIGHTS_STATE_STRUCT		lights_stack_type[MAX_LIGHTS_STATE_STACK];
typedef QUALITY_STATE_STRUCT	quality_stack_type[MAX_QUALITY_STATE_STACK];
typedef COLLISION_STATE_STRUCT	collision_stack_type[MAX_COLLISION_STATE_STACK];
typedef INSTANCE_SUBS_STATE_STRUCT	instance_subs_stack_type[MAX_INSTANCE_SUBS_STATE_STACK];



/*
// Declare the pointers to the limits of the stacks. Note that the actual management
// of the (current) top of stack is done within the display traversal.
*/

MATERIAL_STATE_STRUCT	*pMaterialStackBase = NULL;
MATERIAL_STATE_STRUCT	*pMaterialStackLast = NULL;

TRANSFORM_STRUCT		*pTransformStackBase = NULL;
TRANSFORM_STRUCT		*pTransformStackLast = NULL;

LIGHTS_STATE_STRUCT		*pLightsStackBase = NULL;
LIGHTS_STATE_STRUCT		*pLightsStackLast = NULL;

QUALITY_STATE_STRUCT	*pQualityStackBase = NULL;
QUALITY_STATE_STRUCT	*pQualityStackLast = NULL;

COLLISION_STATE_STRUCT	*pCollisionStackBase = NULL;
COLLISION_STATE_STRUCT	*pCollisionStackLast = NULL;

INSTANCE_SUBS_STATE_STRUCT	*pInstanceSubStackBase = NULL;
INSTANCE_SUBS_STATE_STRUCT	*pInstanceSubStackLast = NULL;




/**************************************************************************
 * Function Name  : InitStateStacks
 * Inputs         : NONE
 * Outputs        : NONE
 * Returns        : 0 if success, Non ZERO if error
 * Global Used    : All the state stack base and last pointers
 * Description    : Allocates space for the various state variable stacks.
 *					ands sets up the pointers to their extremes.
 **************************************************************************/
int InitStateStacks(void)
{
	int err = 0;

	pMaterialStackBase = (MATERIAL_STATE_STRUCT*) NEW(material_stack_type);
	if(pMaterialStackBase!=NULL)
	{
		memset (pMaterialStackBase, 0, sizeof (material_stack_type));
		pMaterialStackLast = pMaterialStackBase + (MAX_MATERIAL_STATE_STACK -1);
	}
	else
	{
		err = 1;
	}


	pTransformStackBase = (TRANSFORM_STRUCT *) NEW(transform_stack_type);
	if(pTransformStackBase !=NULL)
	{
		memset (pTransformStackBase, 0, sizeof (transform_stack_type));
		pTransformStackLast = pTransformStackBase + (MAX_TRANSFORM_STATE_STACK -1);
	}
	else
	{
		err = 1;
	}


	pLightsStackBase = (LIGHTS_STATE_STRUCT *) NEW(lights_stack_type);
	if(pLightsStackBase != NULL)
	{
		memset (pLightsStackBase, 0, sizeof (lights_stack_type));
		pLightsStackLast = pLightsStackBase + (MAX_LIGHTS_STATE_STACK -1);
	}
	else
	{
		err = 1;
	}


	pQualityStackBase = (QUALITY_STATE_STRUCT *) NEW(quality_stack_type);
	if(pQualityStackBase != NULL)
	{
		memset (pQualityStackBase, 0, sizeof (quality_stack_type));
		pQualityStackLast = pQualityStackBase + (MAX_QUALITY_STATE_STACK -1);
	}
	else
	{
		err = 1;
	}

	pCollisionStackBase = (COLLISION_STATE_STRUCT *) NEW(collision_stack_type);
	if(pCollisionStackBase != NULL)
	{
		memset (pCollisionStackBase, 0, sizeof (collision_stack_type));
		pCollisionStackLast = pCollisionStackBase + (MAX_COLLISION_STATE_STACK -1);
	}
	else
	{
		err = 1;
	}


	pInstanceSubStackBase = (INSTANCE_SUBS_STATE_STRUCT *) NEW(instance_subs_stack_type);
	if(pInstanceSubStackBase!= NULL)
	{
		memset (pInstanceSubStackBase, 0, sizeof (instance_subs_stack_type));
		pInstanceSubStackLast = pInstanceSubStackBase + (MAX_INSTANCE_SUBS_STATE_STACK -1);
	}
	else
	{
		err = 1;
	}



	/*
	// Check for an error, and clean up if something went wrong
	// Note ANSI states that free(NULL) does nothing.
	*/
	if(err)
	{
		SGLFree(pMaterialStackBase);
		SGLFree(pTransformStackBase);
		SGLFree(pLightsStackBase);
		SGLFree(pQualityStackBase);
		SGLFree(pCollisionStackBase);
		SGLFree(pInstanceSubStackBase);

		pMaterialStackBase = NULL;        
		pMaterialStackLast = NULL;        
		pTransformStackBase = NULL;       
		pTransformStackLast = NULL;       
		pLightsStackBase = NULL;          
		pLightsStackLast = NULL;          
		pQualityStackBase = NULL;         
		pQualityStackLast = NULL;         
		pCollisionStackBase = NULL;       
		pCollisionStackLast = NULL;       
		pInstanceSubStackBase = NULL;
		pInstanceSubStackLast = NULL;
	} /* end if */



	return(err);
}






/*--------------------------- End of File --------------------------------*/
