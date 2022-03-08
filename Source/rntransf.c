/**************************************************************************
 * Name         : rntransf.c
 * Title        : Rendering tranformation
 * Author       : Stel Michael
 * Created      : 12th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rntransf.c,v $
 * Revision 1.8  1997/04/08  14:38:24  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.7  1995/06/30  11:01:34  sjf
 * Removed redundant sgl.h include.
 *
 * Revision 1.6  1995/06/23  18:45:50  sjf
 * Moved the actual matrix multiply to a generic routine in sgl_math.c
 *
 * Revision 1.5  1995/06/15  11:09:06  sm
 * changed to take transform node struct
 *
 * Revision 1.4  1995/06/13  18:29:15  sm
 * modified scale_flag stuff
 *
 * Revision 1.3  1995/06/12  17:53:29  sm
 * had forgotten rnglobal.h
 *
 * Revision 1.2  1995/06/12  16:59:44  sm
 * fixed a couple of typos
 *
 * Revision 1.1  1995/06/12  16:03:22  sm
 * Initial revision
 *
 *
 **************************************************************************/

#include "sgl_defs.h"

#include "dlnodes.h" 

#include "rnglobal.h"
#include "rntransf.h"
#include "rnstate.h"
#include "rntrav.h"
#include "sgl_math.h"


/**************************************************************************
 * Function Name  : RnProcessTransformNode
 * Inputs         : tranNode
					  
 * Outputs        :  
 * Input/Output	  : stateTransform
						  
 * Returns        : 
 * Global Used    : 
 * Description    : updates the current transform state by multiplying
					a given transform node with it 
 *					
 *				   
 **************************************************************************/


void	RnProcessTransformNode(TRANSFORM_NODE_STRUCT * tranNode,  /* transform node */
						 TRANSFORM_STRUCT  * stateTransform) 	/* Updated state transform node */
{


	/*  we are modifying the current transform state which is used
		as the local to world matrix therefore the local projection
		matrix will become invalid
	*/
	LOCAL_PROJECTION_STRUCT *pLocalProjectionMat = NULL;

	pLocalProjectionMat = RnGlobalGetLocalProjMat();
	ASSERT(pLocalProjectionMat);
	pLocalProjectionMat->valid = FALSE;

	/*
	// Multiply the matrices, and put the result back into
	// the Transform state
	*/
	TransformMultiply(stateTransform, &tranNode->transform, stateTransform);

}
