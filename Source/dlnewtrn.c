/******************************************************************************
 * Name : dltransf.c
 * Title : Transformation Display List Routine,  List nodes
 * Author : Stel Michael
 * Created : 1/05/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : SGL Routines for adding a new translucent node
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dlnewtrn.c,v $
 * Revision 1.6  1997/05/13  17:52:30  mjg
 * Added sglmem include.
 *
 * Revision 1.5  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.4  1997/04/24  20:53:57  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.3  1997/04/04  13:02:57  jop
 * Added explicit #include "pvrosapi.h"
 *
 * Revision 1.2  1997/04/03  13:12:34  mjg
 * Updated the calling convention.
 *
 * Revision 1.1  1995/09/04  15:36:10  sm
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl_defs.h"
#include "sgl_init.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"

void CALL_CONV sgl_new_translucent()
{
	NTRAN_NODE_STRUCT * pNode;

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
	/* Create a new translucent node */

	pNode = NEW(NTRAN_NODE_STRUCT);

   	if(pNode == NULL)
	{
   		/*
   			Abort	   
   		*/
   		SglError(sgl_err_no_mem);
   		return;
   	} 

	pNode->node_hdr.n16_node_type = (sgl_int16) nt_newtran;
	pNode->node_hdr.n16_name	  = (sgl_int16) NM_INVALID_NAME;
	pNode->node_hdr.next_node	  = NULL;

	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);

	SglError(sgl_no_err);
}

/*--------------------------- End of File --------------------------------*/
