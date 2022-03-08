/******************************************************************************
 * Name : dllists.c
 * Title : Display List,  List nodes
 * Author : Simon Fenney
 * Created : 1/05/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : SGL Routines for editing List nodes + utility routines
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dllists.c,v $
 * Revision 1.25  1997/05/13  17:51:36  mjg
 * Replaced PVROS memory management call with SGL.
 *
 * Revision 1.24  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.23  1997/04/30  19:18:26  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.22  1997/04/29  14:44:17  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.21  1997/04/24  20:50:21  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.20  1997/04/23  09:15:17  mjg
 * Removed compiler warnings.
 *
 * Revision 1.19  1997/04/02  21:26:59  mjg
 * Updated calling conventions.
 *
 * Revision 1.18  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.17  1996/11/15  12:08:07  sjf
 * Changed the Sgl Dump List debug function to take a set of flags
 * (well, only one at the mo') instead of a boolean, so that we
 * can expans its functionality later.
 *
 * Revision 1.16  1996/07/18  14:34:57  sjf
 * At Virtualities request, added extra debug output to the dump routine to
 * give some idea of the number of named lists etc.
 *
 * Revision 1.15  1996/07/10  09:29:03  pdl
 * i THINK i corrected some structures.
 *
 * Revision 1.14  1996/07/08  04:21:54  sjf
 * Added user debugging routine to output a display list.
 *
 * Revision 1.13  1996/06/06  23:02:27  sjf
 * Added UNTESTED material instancing
 *
 * Revision 1.12  1996/05/09  13:42:38  sjf
 * Added facility to ignore display lists.
 *
 * Revision 1.11  1996/03/18  14:49:41  jrc
 * sgl_create_list now returns a name if the preserve_state flag is not set
 * (as specified in the documentation).
 *
 * Revision 1.10  1995/10/25  18:38:15  sjf
 * OOPs wasn't clearing list's pointers when emptying the list, so it
 * still thought there was garbage in it.
 *
 * Revision 1.9  1995/09/21  14:43:49  jrc
 * Fixed ClearDisplayList (used by sgl_modify_list with flag set TRUE) to
 * correctly skip to the next node for deletion without skipping nodes.
 *
 * Revision 1.8  1995/09/16  19:43:25  sjf
 * Fixed 3 problems
 * 1) back the from ASSERT logic
 * 2) Ignoring preserve state in lists set up.
 * 3) Ignoring SGL_DEFAULT list in modify list.
 *
 * Revision 1.7  1995/07/14  15:07:28  sjf
 * Fixed syntax errors revealed when ASSERT actually wsa included in code.
 *
 * Revision 1.6  1995/07/14  14:53:32  sjf
 * Moved module id to before sgl_defs.h
 *
 * Revision 1.5  1995/06/08  09:30:09  sm
 * removed duplicate calls to DlCompleCur...
 *
 * Revision 1.4  1995/06/06  15:40:46  sjf
 * Added more list routines.
 *
 * Revision 1.3  1995/05/30  12:26:56  sjf
 * Added sgl_create_list and append to list routines.
 *
 * Revision 1.2  1995/05/26  14:33:42  jrc
 * Removed space from RCS Log macro.
 *
 *
 *****************************************************************************/

/*
// define Debug Module IDs (needed before sgl_defs.h)
*/
#define MODULE_ID	MODID_DL


#include "sgl_defs.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "sgl_init.h"
#include "dldelete.h"


/**************************************************************************
*
*  						List Node routines  
*
***************************************************************************/

/**************************************************************************
 * Function Name  : DisconnectNodeFromList    LOCAL ROUTINE ONLY
 * Inputs Only	  : None
 * Outputs Only   : None
 * Inputs/Outputs : pList - pointer to the list node that contains the node
 *					pNode - pointer to a node
 *					
 * Returns        : None
 * Global Used    : None
 * Description    : Removes the given node from the list - it first finds the
 *					supplied node in the list, then disconnects it.
 *
 *					DEBUG VERSION: Causes an ASSERT if the node is not found in
 *					the list.
 **************************************************************************/
static void DisconnectNodeFromList(LIST_NODE_STRUCT *pList,
									DL_NODE_STRUCT	*pNode)
{
	/*
	// pointers to access the nodes of the display list
	*/
	DL_NODE_STRUCT * pPrevNode, *pThisNode;


	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);
	ASSERT(pNode != NULL)

	/*
	// See if the node to disconnect is the first one in the list
	*/
	if(pList->pfirst == pNode)
	{
		/*
		// then make the first one the next one in the list
		*/
		pList->pfirst = pNode->next_node;

		/*
		// check to see if there WAS only one item in the list,
		// and update the last node pointer as well.
		*/
		if(pList->plast == pNode)
		{
			pList->plast = NULL;

			ASSERT(pNode->next_node == NULL)
		}


		/*
		// tidy up the pointers in the node (for neatness)
		// probably could make this a debug only option...
		*/
		pNode->next_node = NULL;
		
	}
	/*
	// Else it is somewhere further down the list...
	// (Well, it better be - can't have the lil' varmint sneakin' off...)
	*/
	else
	{
		/*
		// Get pointer to the first item, and set it up as the previous.
		// Get the next pointer.
		*/
		pPrevNode = pList->pfirst;
		pThisNode = pPrevNode->next_node;

		/*
		// Loop through looking for the node
		*/
		while((pThisNode != pNode) && (pThisNode != NULL))
		{
			/*
			// Skip to the loo my darling  (err.. next node)
			*/
			pPrevNode= pThisNode;
			pThisNode = pThisNode->next_node;

		} /*end while*/
		
		/*
		// Check that we did find it
		*/
		if(pThisNode == pNode)
		{
			/*
			// Connect the previous node to the one after ours
			*/
			pPrevNode->next_node = pNode->next_node;

			/*
			// Check if the located node was the last in the chain,
			// and set the list pointers accordingly
			*/
			if(pList->plast == pNode)
			{
				pList->plast = pPrevNode;
				ASSERT(pNode->next_node == NULL)
			}

		}
		/*
		// Else it ain't in this list... what sort of rubbish
		// are you trying to peddle here...
		*/
		else
		{
			ASSERT(FALSE)
		}/*end if found node*/


	}/* end if/else*/

}


/**************************************************************************
 * Function Name  : ClearDisplayList    LOCAL ROUTINE ONLY
 * Inputs         : pList - pointer to a VALID list node
 * Outputs        : 
 * Returns        : None
 * Global Used    : display list nodes.. + current list
 * Description    : Flushes out the contents of a list like a dose of salts.
 *
 *					Note: This routine calls the delete node routine to remove
 *						  each element in the list, and as a side-effect of
 *						  this call, the current list etc pointers may be 
 *						  set to NULL.
 **************************************************************************/
static void ClearDisplayList(LIST_NODE_STRUCT *  pList)
{
	/*
	// pointers to access the nodes of the display list
	// 
	*/
	DL_NODE_STRUCT * pNode, * pNextNode;

	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);


	/*
	// Start at the first node and work our way through the list
	*/
	pNode = pList->pfirst;
	while(pNode != NULL)
	{
		/*
		// Get a pointer to the next node, so we can cleanly delete
		// things
		*/
		pNextNode = pNode->next_node;
	
		/*
		// Call the routine to delete this node
		*/
		DlDeleteNode(pNode);
		 
		/*
		// Move to the next node
		*/
		pNode = pNextNode;
	}

	/*
	// Set the pointers to Null
	*/
	pList->pfirst  = NULL;
	pList->plast   = NULL;
}


/**************************************************************************
 * Function Name  : DlDeleteListNodeRefs
 * Inputs         : 
 * Outputs        : 
 * Input/Output	  : pNode
 * Returns        : None
 * Global Used    : None
 * Description    : Clears up any extransous references/memory used by the
 *					display list node - Basically it calls the clear list 
 *					routine. This turns out to be rather recursive....
 *
 **************************************************************************/
void  DlDeleteListNodeRefs(DL_NODE_STRUCT *pNode)
{
	LIST_NODE_STRUCT *  pList;

	/*
	// For neatness, retype the pointer to be a list node
	*/
	pList = (LIST_NODE_STRUCT * )pNode;

	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);

	ClearDisplayList(pList);
}




/**************************************************************************
 * Function Name  : InitListNode
 * Inputs         : "name"  		- a name for the list, 
 *					"pParentList"	- a pointer to the lists parent
 * Outputs        : 
 * Input/Output	  : pListNode		- List Node to initalise
 * Returns        : None
 * Global Used    : None
 * Description    : Sets up all the fields in a list node struct. NOTE. It
 *					DOES NOT add the list node to the "parent list"
 *
 *					By default, it initialises the Preserve State to TRUE
 **************************************************************************/


void InitListNode(LIST_NODE_STRUCT * pListNode,
				int name,
				LIST_NODE_STRUCT * pParentList)
{
	/*
	// Fill in the relevant standard header stuff...
	//
	// Setting the type and name are compulsary, but the
	// next node is just for cleanliness/error catching.
	*/
	pListNode->node_hdr.n16_node_type = (sgl_int16) nt_list_node;
	pListNode->node_hdr.n16_name	  = (sgl_int16) name;
	pListNode->node_hdr.next_node	  = NULL;

	/*
	// Set up the lists parent pointer, and first and last pointers
	*/

	pListNode->pparent = pParentList;
	pListNode->pfirst  = NULL;
	pListNode->plast   = NULL;


	/*
	// Set the flags so that, by default, we process the list, 
	// and preserve the state
	*/
	pListNode->flags   = (list_flags_enum) (lf_process_list | lf_preserve_state);

}


/**************************************************************************
 * Function Name  : AppendNodeToList
 * Inputs         : pParentList 		- pointer to the parent list
 *					pNode				- pointer to node being added.
 * Outputs        : None
 * Input/Output	  : 
 * Returns        : None
 * Global Used    : None
 * Description    : Appends a new node to a list. It does NO ERROR CHECKING
 *
 **************************************************************************/

void AppendNodeToList(LIST_NODE_STRUCT *  pParentList,
							void * pParamNode)
{

  	DL_NODE_STRUCT * pTheNode;

	/*
	// get easier access to the node
	*/
	pTheNode = (DL_NODE_STRUCT *) pParamNode;

	/*
	// DEBUG asserts. Check we have been passed a valid
	// list node.
	*/
	ASSERT(pParentList->node_hdr.n16_node_type == nt_list_node);
	

	/*
	// Do the case if the list is empty
	*/
	if(pParentList->pfirst == NULL)
	{
		ASSERT(pParentList->plast == NULL);

		/*
		// make this the first and last element in the list
		*/
		pParentList->pfirst = pTheNode;
		pParentList->plast  = pTheNode;

	}
	/*
	// Else add the node onto the last node in the list
	*/
	else
	{
		/*
		// Check that the last node of the list has a
		// NULL pointer
		*/
		ASSERT(pParentList->plast->next_node == NULL);

		/*
		// Set the last node's next pointer to point
		// to the new addition
		*/
		pParentList->plast->next_node = pTheNode;
		
		/*
		// Set the last node pointer to point to the
		// new addition
		*/
		pParentList->plast = pTheNode;
	}
	
	/*
	// In both cases, set up the node's "next" pointer to be NULL
	*/
	pTheNode->next_node = NULL;

}






/**************************************************************************
 * Function Name  : sgl_create_list
 * Inputs Only    : generate_name, perserve_state, separate_list
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : optionally either a name or error, and only if generate name
 *					is set.
 * Global Used    : Alters the display list data structures....
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					* Allocate the list node.
 *					* Add it to the name table if necessary
 *					* Attach it to a parent if need be.
 *					* Make this list the current one.
 *
 *
 *		NOTE: The returned value is not defined if generate_name is FALSE.
 **************************************************************************/

int	CALL_CONV sgl_create_list( sgl_bool generate_name,
							   sgl_bool preserve_state,
							   sgl_bool separate_list )
{
	int error = sgl_no_err;
	int name  = NM_INVALID_NAME;

	LIST_NODE_STRUCT * pList;

	/*
	// Set generate name flag in the cases where it is pointless NOT to do so
	*/
	if(separate_list || !preserve_state)
		generate_name = TRUE;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		error = sgl_err_failed_init;
		SglError(error);

		return error;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();

	/*
	// Allocate memory for the display list node,
	// and check if it's ok.
	*/
	pList = NEW(LIST_NODE_STRUCT);
	if(pList == NULL)
	{
		/*
		// ABORT ROUTINE
		*/
		error = sgl_err_no_mem;
		SglError(error);

		return error;
	} /*end if*/


	/*
	// If we need a name, generate one, adding the item to
	// the name table at the same time.
	*/
	if(generate_name)
	{
		name = AddNamedItem(dlUserGlobals.pNamtab,
			   				pList,
							nt_list_node);
		/*
		// If there were no free spaces, then the name will
		// contain an error value (i.e.negative)
		//
		// In this situation TIDY UP and ABORT.
		*/
		if(name < 0)
		{
			error = name;
			name = NM_INVALID_NAME;

			SGLFree(pList);
			SglError(error);
			return error;
		}
	} /*end if generate name */



	/*
	// Fill in the details, attaching either as a separate list
	// or as a child of the current list
	*/
	if(separate_list)
	{
		InitListNode(pList, name, NULL);
	}
	else
	{
		InitListNode(pList, name, dlUserGlobals.pCurrentList);

		AppendNodeToList(dlUserGlobals.pCurrentList, pList);
	}


	/*
	// Set the preserve state flag appropriately
	// (Note by default it has already been turned on)
	*/	
	if(!preserve_state)
	{
		pList->flags &= ~ lf_preserve_state;
	}


	/*
	// set the new current list to be this list
	*/
	dlUserGlobals.pCurrentList = pList;


	/*
	// record the error for posterity.
	*/
	SglError(error);

	/*
	// according to the definition, the function only returns
	// a valid value if the generate name flag is set.
	*/
	return name;

}

/**************************************************************************
 * Function Name  : sgl_to_parent
 * Inputs Only    : NONE
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 * Global Used    : Alters the global current display list
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					* Basically, it moves the current list pointer to its
 *					parent - if it has one.
 *
 **************************************************************************/

void CALL_CONV sgl_to_parent ( void )
{
	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise. ABORT ABORT ABORT
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();

	/*
	// do paranoia checking
	*/
	ASSERT(dlUserGlobals.pCurrentList->node_hdr.n16_node_type == nt_list_node)

	/*
	// if there IS a parent, go to it (otherwise "do nowt")
	*/
	if(dlUserGlobals.pCurrentList->pparent!=NULL)
	{
		dlUserGlobals.pCurrentList = dlUserGlobals.pCurrentList->pparent;
	}

	/*
	//  More paranoia checking
	*/
	ASSERT(dlUserGlobals.pCurrentList->node_hdr.n16_node_type == nt_list_node)

	SglError(sgl_no_err);
	

}



/**************************************************************************
 * Function Name  : sgl_modify_list
 * Inputs Only    : list_name, clear_list
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 * Global Used    : Changes the current list, and optionally clears it
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					* Basically, it moves the current list pointer to the one
 *					named in the parameter - if it exists, and optionally clears
 *					out its contents.
 *
 **************************************************************************/
void CALL_CONV sgl_modify_list ( int list_name, 
								 sgl_bool clear_list )
{
	/*
	// Pointer to the list to modify
	*/
	LIST_NODE_STRUCT * pList;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();


	/*
	// Examine the name of the list.
	//
	// If it is the special default list, then go to it
	*/
	if(list_name == SGL_DEFAULT_LIST)
	{
		pList = dlUserGlobals.pDefaultList;
	}
	/*
	// ELSE fetch a pointer to this list, by using name table
	*/
	else
	{
		pList = GetNamedItem(dlUserGlobals.pNamtab, list_name);
		/*
		// if this is NOT a valid list, then get the out of this
		// routine, and record that the user has supplied us with
		// a load of nonsense
		*/
		if( (pList == NULL) || 
			(GetNamedItemType(dlUserGlobals.pNamtab, list_name)!= nt_list_node))
		{
			SglError(sgl_err_bad_name);
			/*
			// ABORT  - this is the cleanest way to handle this...
			*/
			return;
		}

	}
	

	/*
	// For paranoia just check that node type field in the actual
	// structure ALSO ties up
	*/
	ASSERT(pList->node_hdr.n16_node_type == nt_list_node)

	/*
	// set this list to be the current list
	*/
	dlUserGlobals.pCurrentList = pList;

	/*
	// Clear out the display list if requested
	*/
	if(clear_list)
	{
		ClearDisplayList(pList);

		/*
		// After the clear list, we should still have a valid 
		// current list since it shouldnt have been deleted, as
		// we just moved it!!
		*/
		ASSERT(dlUserGlobals.pCurrentList != NULL)
	}


	SglError(sgl_no_err);

}


/**************************************************************************
 * Function Name  : sgl_delete_list
 * Inputs Only    : list_name
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The list referenced by the supplied name, and parent if any.
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					Deletes the named list, removing it from its parent if it has
 *					one.
 *
 **************************************************************************/
void CALL_CONV sgl_delete_list ( int list_name )
{
	/*
	// Pointer to the list to delete, and its parent, if any
	*/
	LIST_NODE_STRUCT * pList, *pParent;
	
	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	/*
	// fetch a pointer to this list, by using name table
	*/
	pList = GetNamedItem(dlUserGlobals.pNamtab, list_name);
	
	/*
	// if this is NOT a valid list, then get the out of this
	// routine, and record that the user has supplied us with
	// a load of nonsense
	*/
	if((pList == NULL) || 
		(GetNamedItemType(dlUserGlobals.pNamtab, list_name)!= nt_list_node))
	{
		SglError(sgl_err_bad_name);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();



	/*
	// If this list has a parent, remove it from that list
	*/
	pParent = pList->pparent;
	if(pParent != NULL)
	{
		DisconnectNodeFromList(pParent,(DL_NODE_STRUCT*)pList);
		pList->pparent =  NULL;
	}


	/*
	// Delete the node, using the generic delete node function.
	// (Of course that calls the clear list function , but we end up
	// not having to repeat code eg. cleaning up the name table etc..)
	*/
	DlDeleteNode((DL_NODE_STRUCT*)pList);


	/*
	// Is the current list still in existence after this...
	*/
	if(dlUserGlobals.pCurrentList != NULL)
	{
		/* NO, then do nothing */
	}
	/*
	// Else we have lost it, then if the deleted list had a parent,
	// use that as the current list
	*/
	else if(pParent != NULL)
	{
		dlUserGlobals.pCurrentList = pParent;
	}
	/*
	// else we have to use the default list
	*/
	else
	{
		dlUserGlobals.pCurrentList = dlUserGlobals.pDefaultList;
	}
	
	SglError(sgl_no_err);
}



/**************************************************************************
 * Function Name  : sgl_detach_list
 * Inputs Only    : list_name
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The list referenced by the supplied name, and parent if any.
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					Disconnects the list from its parent, if it had a parent.
 *
 **************************************************************************/
void CALL_CONV sgl_detach_list ( int list_name )
{
	/*
	// Pointer to the list to detach
	*/
	LIST_NODE_STRUCT * pList;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();

	
	/*
	// fetch a pointer to this list, by using name table
	*/
	pList = GetNamedItem(dlUserGlobals.pNamtab, list_name);
	
	/*
	// if this is NOT a valid list, then get the out of this
	// routine, and record that the user has supplied us with
	// a load of nonsense
	*/
	if((pList == NULL) || 
		(GetNamedItemType(dlUserGlobals.pNamtab, list_name)!= nt_list_node))
	{
		SglError(sgl_err_bad_name);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}


	/*
	// If the list has a parent, then detach it, otherwise, 
	// leave everything as is
	*/
	if(pList->pparent != NULL)
	{
		DisconnectNodeFromList(pList->pparent,(DL_NODE_STRUCT*)pList);
		pList->pparent = NULL;
	}

	SglError(sgl_no_err);

}


/**************************************************************************
 * Function Name  : ListContains  LOCAL ROUTINE ONLY
 * Inputs         : pList - pointer to a VALID list node
 *					pNodeToFind - pointer to a node
 * Outputs        : 
 * Returns        : None
 * Global Used    : 
 * Description    : Traverses the supplied list (recursively) and returns
 *					TRUE if the supplied node is contained within the list,
 *					or any child list etc.
 **************************************************************************/

static sgl_bool ListContains( LIST_NODE_STRUCT * pList, 
							  DL_NODE_STRUCT* pNodeToFind)
{
	sgl_bool found;
	DL_NODE_STRUCT* pNode;

	found= FALSE;

	/*
	// go to the start of the list, and loop through its contents.
	*/
	pNode = pList->pfirst;
	while(pNode != NULL)
	{
		/*
		// Do we have a match?, If so record the result, and stop
		// searching by escaping the loop
		*/
		if(pNode == pNodeToFind)
		{
			found = TRUE;
			break;
		}
		
		/*
		// If this is a list node, then search inside of it.
		// Again, exit the loop if we find the node.
		*/
		if( (pNode->n16_node_type == nt_list_node) &&
			ListContains((LIST_NODE_STRUCT *)pNode, pNodeToFind))
		{
			found = TRUE;
			break;
		}
		
		/*
		// Else just move onto the next node
		*/
		pNode = pNode->next_node;
		
	}/*end while*/




	return found;

}


/**************************************************************************
 * Function Name  : sgl_attach_list
 * Inputs Only    : list_name
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The list referenced by the supplied name, parent if any, and
 *					the current list.
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					Disconnects the list from its parent, if it had a parent, and
 *					reconnects to the end of the current list - PROVIDED the current
 *					list actually isn't contained within the list to attach. That
 *					would cause a recursive definition which would be a nightmare.
 *
 **************************************************************************/
void CALL_CONV sgl_attach_list ( int list_name )
{
	/*
	// Pointer to the list to detach
	*/
	LIST_NODE_STRUCT * pList;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();

	/*
	// fetch a pointer to this list, by using name table
	*/
	pList = GetNamedItem(dlUserGlobals.pNamtab, list_name);
	
	/*
	// if this is NOT a valid list, then get the out of this
	// routine, and record that the user has supplied us with
	// a load of nonsense
	*/
	if((pList == NULL) || 
		(GetNamedItemType(dlUserGlobals.pNamtab, list_name)!= nt_list_node))
	{
		SglError(sgl_err_bad_name);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}



	/*
	// check that the current list is NOT a child of this list.
	// (or if it IS this list!!)
	//
	// If this is the case, then we'd any end up with a nasty
	// self referencing list, und das ist verbotten.
	*/
	if( (pList == dlUserGlobals.pCurrentList) ||
		ListContains(pList, (DL_NODE_STRUCT*)dlUserGlobals.pCurrentList))
	{
		SglError(sgl_err_cyclic_reference);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}


	/*
	// Detach the list - THIS SHOULD NOT CAUSE AN ERROR
	//
	// NOTE: calling this routine will automatically set
	// up the error status ... ie. it performs a
	//
	//	  	SglError(sgl_no_err);
	//
	// for us automatically.
	*/
	sgl_detach_list(list_name);


	/*
	// append to the end of the current list, and set the lists parent
	// pointer to point back to its new parent
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pList);
	pList->pparent = dlUserGlobals.pCurrentList;

}


/**************************************************************************
 * Function Name  : sgl_set_ignore_list
 * Inputs Only    : list_name, ignore
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The list referenced by the supplied name, parent if any, and
 *					the current list.
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					Sets a flag in the list which allows the traversal to optionally
 *					skip over it.
 **************************************************************************/

void CALL_CONV sgl_set_ignore_list ( int list_name, sgl_bool ignore )
{
	/*
	// Pointer to the named list
	*/
	LIST_NODE_STRUCT * pList;
	int	NodeType;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();
	

	/*
	// fetch a pointer to the instanced list, by using name table
	*/
	pList = GetNamedItemAndType(dlUserGlobals.pNamtab, list_name, &NodeType);

	/*
	// if this is NOT a valid list, then get the out of this
	// routine, and record that the user has supplied us with
	// a load of nonsense
	*/
	if((pList == NULL) || NodeType != nt_list_node)
	{
		SglError(sgl_err_bad_name);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}

	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);


	/*
	// Set the ignore flag accordingly
	*/
	if(ignore)
	{
		pList->flags &= ~lf_process_list;
	}
	else
	{
		pList->flags |= lf_process_list;
	}


	/*
	// All went OK
	*/
	SglError(sgl_no_err);



}


/**************************************************************************
*
*  			List Instance and Substitution Routines  
*
***************************************************************************/


/**************************************************************************
 * Function Name  : sgl_use_instance
 * Inputs Only    : list_to_instance
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The current list, and name table.
 * Description    : The External behaviour of this function is described in the SGL
 *					interface document.
 *
 *					Adds the instance on to the current list - also updates
 *					the usageage count for the list.
 *
 **************************************************************************/

void CALL_CONV sgl_use_instance( int list_to_instance )
{
	/*
	// Pointer to the instanced list
	*/
	LIST_NODE_STRUCT * pInstancedList;
	INSTANCE_NODE_STRUCT *pInstance;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();
	


	/*
	// fetch a pointer to the instanced list, by using name table
	*/
	pInstancedList = GetNamedItem(dlUserGlobals.pNamtab, list_to_instance);

	/*
	// if this is NOT a valid list, then get the out of this
	// routine, and record that the user has supplied us with
	// a load of nonsense
	*/
	if((pInstancedList == NULL) || 
		(GetNamedItemType(dlUserGlobals.pNamtab, list_to_instance)!= nt_list_node))
	{
		SglError(sgl_err_bad_name);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}

	ASSERT(pInstancedList->node_hdr.n16_node_type == nt_list_node);


	/*
	// Create a new list instance node, if
	// this fails report the error and abort.
	*/
	pInstance = NEW(INSTANCE_NODE_STRUCT);
	if(pInstance == NULL)
	{
		SglError(sgl_err_no_mem);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}

	/*
	// Fill in the fields in the header of the instance node
	// i.e. type and name
	//
	// and the list that is instanced.
	*/
	pInstance->node_hdr.n16_node_type =  nt_instance;
	pInstance->node_hdr.n16_name	  =  NM_INVALID_NAME;
	pInstance->referenced_list = list_to_instance;


	/*
	// increment the usage count on this item in the name table
	*/
	IncNamedItemUsage(dlUserGlobals.pNamtab,
					list_to_instance);

	/*
	// Add the instance to the current list
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pInstance);

	/*
	// All went OK
	*/
	SglError(sgl_no_err);
	
}


/**************************************************************************
 * Function Name  : DlDeleteInstanceNodeRefs
 * Inputs Only    : pointer to instance node
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The name table.
 * Description    : Decrements the reference count on the list mentioned in the
 *					the name table.
 *
 **************************************************************************/

void  DlDeleteInstanceNodeRefs(DL_NODE_STRUCT *pNode)
{
	INSTANCE_NODE_STRUCT *pInstance;

	/*
	// get easier access to the instance node...
	*/
	pInstance = (INSTANCE_NODE_STRUCT *) pNode;

	ASSERT(pInstance->node_hdr.n16_node_type == nt_instance);

	/*
	// decrement the usage count for this node
	*/
	DecNamedItemUsage(dlUserGlobals.pNamtab,
					pInstance->referenced_list);
	
}



/**************************************************************************
 * Function Name  : sgl_instance_substitutions
 * Inputs Only    : Number of substitutions in the list, and a list
 *				  : of those substitutions
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The current display list.
 * Description    : Decrements the reference count on the list mentioned in the
 *					the name table.
 *
 **************************************************************************/
void CALL_CONV sgl_instance_substitutions( int num_subs_param,
										   int *instance_params )
{

	int numSubs;
	int numSubsAdded;
	int i;
	int error;	
	INSTANCE_SUBS_NODE_STRUCT * pSubNode;

	void *itemPtr;
	int	 replaceeType, replacementType;

	int replacementName, replaceeName;

	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();


	/*
	// create the substitution node - abort the routine
	// if we have no memory
	*/
	pSubNode = NEW(INSTANCE_SUBS_NODE_STRUCT);
	if(pSubNode == NULL)
	{
		SglError(sgl_err_no_mem);
		return;
	}

	/*
	// Fill in the header parameters.
	// (NOTE No name)
	*/
	pSubNode->node_hdr.n16_node_type = nt_inst_subs;
	pSubNode->node_hdr.n16_name		 = NM_INVALID_NAME;

	/*
	// Add it to the current display list
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pSubNode);
		

	/*
	// Add the parameters, ignoring substitutions which aren't valid.
	//
	// note:Limit the number of substitutions to be in a legal range
	//
	// Assume no error at this point, until proven guilty. Note that
	// bad name errors are ignored (according to the spec)
	*/
	numSubs = CLAMP(num_subs_param, 0, SGL_MAX_INSTANCE_PARAMS);
	numSubsAdded = 0;

	error = sgl_no_err;

	for(i = 0; i < numSubs; i++)
	{
		/*
		// Get the names of the two lists involved in this
		// substitution
		*/
		replaceeName 	= instance_params[i*2];
		replacementName = instance_params[i*2 + 1];


		/*
		// check the credentials of the replacee. If this name doesn't
		// exist, OR if it isn't a list or material, then SKIP this substitution, 
		// and and remember the error status.
		*/
		itemPtr = GetNamedItemAndType(dlUserGlobals.pNamtab, replaceeName,
									&replaceeType);
		if((itemPtr == NULL) || 
					((replaceeType!=nt_list_node)&&	(replaceeType!=nt_material)))
		{
			/*
			// bad name, remember the error, and go onto the next pair
			*/
			error = sgl_err_bad_name;
			continue;
		}
		
		/*
		// Paranoia ... check the type and name of the referenced node
		*/
		ASSERT(((DL_NODE_STRUCT*)itemPtr)->n16_node_type == replaceeType)
		ASSERT(((DL_NODE_STRUCT*)itemPtr)->n16_name		 == replaceeName)
		
		/*
		// Now check the replacement - it is also valid to replace it with NULL
		*/
		if(replacementName == SGL_NULL_LIST)
		{
			/*
			// It's Ok, so do nothing
			*/
		}
		/*
		// else do the full check
		*/
		else
		{
			itemPtr = GetNamedItemAndType(dlUserGlobals.pNamtab, replacementName,
									&replacementType);
			

			if((itemPtr == NULL) || (replacementType != replaceeType))
			{
				/*
				// bad name, remember the error, and go onto the next pair
				*/
				error = sgl_err_bad_name;
				continue;
			}

			/*
			// Paranoia ... check the type and name of the referenced node
			*/
			ASSERT(((DL_NODE_STRUCT*)itemPtr)->n16_node_type == replacementType)
			ASSERT(((DL_NODE_STRUCT*)itemPtr)->n16_name		 == replacementName)

		}/*end if name valid*/

		/*
		// Increment the usage counts on these two names (unless the second 
		// is the NULL list
		*/
		IncNamedItemUsage(dlUserGlobals.pNamtab, replaceeName);
		if(replacementName != SGL_NULL_LIST)
		{
				IncNamedItemUsage(dlUserGlobals.pNamtab, replacementName);
		}
		
		/*
		// Add them both to the list of substitutions in the node.
		*/
		pSubNode->param_list[numSubsAdded][0] = replaceeName;
		pSubNode->param_list[numSubsAdded][1] = replacementName;

		numSubsAdded ++;

	}/*End For*/


	/*
	// Save the count of the number of substitutions
	*/
	pSubNode->num_subs = numSubsAdded;
	/*
	// save the error status
	*/
	SglError(error);
	

}



/**************************************************************************
 * Function Name  : DlDeleteSubstitutionNodeRefs
 * Inputs Only    : Pointer to an instance substitution node 
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : The current display list.
 * Description    : Decrements the reference count on the instance node
 *					mentioned in the name table.
 *
 **************************************************************************/
void  DlDeleteSubstitutionNodeRefs(DL_NODE_STRUCT *pNode)
{

	INSTANCE_SUBS_NODE_STRUCT * pSubNode;
	int i;
	/*
	// Get more convenient access to the node
	*/
	pSubNode = (INSTANCE_SUBS_NODE_STRUCT *) pNode;

	/*
	// Test we haven't been given nonsense
	*/
	ASSERT(pSubNode->node_hdr.n16_node_type == nt_inst_subs)
	ASSERT(pSubNode->node_hdr.n16_name	== NM_INVALID_NAME)
	
	ASSERT(pSubNode->num_subs >= 0)
	ASSERT(pSubNode->num_subs <= SGL_MAX_INSTANCE_PARAMS)
	
	/*
	// Step through the entries in the list, decrementing the reference
	// counts
	*/
	for(i = 0; i < pSubNode->num_subs; i++)
	{
		/*
		// decrement the count on the first of the pair
		*/
		DecNamedItemUsage(dlUserGlobals.pNamtab, pSubNode->param_list[i][0]);

		/*
		// If its replacement isn't the Null List, then decrement its count too
		*/
		if(pSubNode->param_list[i][1] != SGL_NULL_LIST)
		{
				DecNamedItemUsage(dlUserGlobals.pNamtab, pSubNode->param_list[i][1]);
		}
	}/*end for*/

	
}


 /**************************************************************************
 * Function Name  : DumpAndRecurseList	 INTERNAL ONLY!
 * Inputs Only    : listname	Name of list to traverse and output structure
 *					outfile 	File to put the printout in.
 *					traverseFlags
 *					current depth;
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Nil
 * Description    : Traverse the supplied display list and outputs 
 *
 **************************************************************************/

#define MAX_DEPTH 60
static char blankpadding[MAX_DEPTH*2 +1];

static char NonPres[]="NonPreserving";
static char Ignored[]="Ignored";

static char blank[]=" ";

static void DumpAndRecurseList(LIST_NODE_STRUCT *pList, 
						FILE * outfile, 
						sgl_bool traverseFlags,
						int currentdepth)
{

	LIST_NODE_STRUCT * pChildList;
	DL_NODE_STRUCT 	 *pNode;
	char	* pPaddingString;

	char *pres, *igno;

	if(currentdepth > MAX_DEPTH)
	{
		return; /*this is actually an error!*/
	}
	
	pPaddingString = blankpadding+(MAX_DEPTH - currentdepth)*2;

	if(currentdepth == MAX_DEPTH)
	{
		fprintf(outfile, "%sList: MAX DUMP RECURSION DEPTH REACHED!\n",
							pPaddingString);
		return;
	}

	if(pList->flags & lf_process_list)
	{
		igno = blank;
	}
	else
	{
		igno = Ignored;
	}

	if(pList->flags & lf_preserve_state)
	{
		pres = blank;
	}
	else
	{
		pres = NonPres;
	}

	/*
	// Print out this list's details
	*/
	fprintf(outfile, "%sList.  Name:%d  %s %s\n", pPaddingString,
					   (int)pList->node_hdr.n16_name,
						igno, pres);
					   	
	/*
	// now traverse through the list
	*/
	pNode = pList->pfirst;

	currentdepth++;
	pPaddingString = blankpadding+(MAX_DEPTH - currentdepth)*2;

	while(pNode != NULL)
	{
		switch(pNode->n16_node_type)
		{

			case nt_list_node:
			{
				DumpAndRecurseList((LIST_NODE_STRUCT*) pNode, 
					outfile, traverseFlags, currentdepth);

				break;
			}

			case nt_convex:
			{
				fprintf(outfile, "%sConvex: Name%d\n",
						pPaddingString, pNode->n16_name);
				break;
			}

			case nt_mesh:
			{
				fprintf(outfile, "%sMesh: Name%d\n",
						pPaddingString, pNode->n16_name);
				break;
			}

	
			case nt_material:
			{
				DumpMaterial((MATERIAL_NODE_STRUCT*)pNode, outfile,
							pPaddingString);
				break;
			}
				

			case nt_lod:				/* Level of detail node */
			{
				fprintf(outfile, "%sLevel Of Detail\n",
						pPaddingString);
				break;
			}

			case nt_instance: /*list instance node*/
							
			{
				INSTANCE_NODE_STRUCT *pInst = (INSTANCE_NODE_STRUCT *)pNode;
				fprintf(outfile, "%sInstance Node: Uses List:%d\n",
						pPaddingString,pInst->referenced_list);

				pChildList = GetNamedItem(dlUserGlobals.pNamtab, 
											pInst->referenced_list);
				if((traverseFlags & sgl_dumplist_instances)
								   && (pChildList != NULL))
				{
					fprintf(outfile, "%s  Traveresing Instance Node\n",
									pPaddingString);

					DumpAndRecurseList(pChildList, 
						outfile, traverseFlags, currentdepth);
					
				}
				break;
			}


			/*
			// The ones I'm not handling yet
			*/
			case nt_inst_subs:
			{
				fprintf(outfile,"%sInstance Subs \n", pPaddingString);
				break;
			}
			case nt_transform:
			{
				fprintf(outfile,"%sTransform \n", pPaddingString);
				break;
			}

			case nt_light:
			{
				fprintf(outfile,"%sLight \n", pPaddingString);
				break;
			}
			case nt_light_pos:
			{
				fprintf(outfile,"%sLight Pos \n", pPaddingString);
				break;
			}
			case nt_light_switch:
			{
				fprintf(outfile,"%sLight Switch \n", pPaddingString);
				break;
			}
			case nt_multi_shadow:
			{
				fprintf(outfile,"%sMulti Shadow \n", pPaddingString);
				break;
			}
			case nt_camera:
			{
				fprintf(outfile,"%sCamera \n", pPaddingString);
				break;
			}
			case nt_point:
			{
				fprintf(outfile,"%sPoint \n", pPaddingString);
				break;
			}
			case nt_point_pos:
			{
				fprintf(outfile,"%sPoint Position \n", pPaddingString);
				break;
			}
			case nt_point_switch:
			{
				fprintf(outfile,"%sPoint Switch \n", pPaddingString);
				break;
			}
			case nt_quality:
			{
				fprintf(outfile,"%sQuality \n", pPaddingString);
				break;
			}
			case nt_newtran:
			{
				fprintf(outfile,"%sNew Translucent \n", pPaddingString);
				break;
			}
			case nt_dummy:
			{
				fprintf(outfile,"%sPartly Deleted Node \n",
							pPaddingString);
				break;
			}
			

			default:
				fprintf(outfile, "%sUnknown/Uncoded node type reached!\n",
							pPaddingString);
				
				break;
		}/*end switch*/

		/*
		// Move along
		*/
		pNode=pNode->next_node;

		fprintf(outfile, "\n");

	}/*end while*/


}



/**************************************************************************
 * Function Name  : sgl_dump_list
 * Inputs Only    : listname	Name of list to traverse and output structure
 *					outfile 	File to put the printout in.
 *					traverse_instances
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Nil
 * Description    : Traverse the supplied display list and outputs 
 *
 **************************************************************************/
void CALL_CONV sgl_dump_list( int listname, 
							  FILE * outfile, 
							  sgl_uint32 traverse_flags )
{
	/*
	// Pointer to the named list
	*/
	LIST_NODE_STRUCT * pList;
	int	NodeType;
	int i;

	NAME_STATS_STRUCT NameStats;


	/*
	// Do initialise if necesary. ABORT if error.
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		// We failed to initialise
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// Do standard "tidy ups" of any display list editing
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();
	DlCompleteCurrentTransform();
	
	/*
	// Initalise the blank padding string
	*/
	for(i = 0; i < MAX_DEPTH*2; i++)
	{
		blankpadding[i]=' ';
	}
	blankpadding[i]='\0';

	fprintf(outfile, "==============================\n");
	fprintf(outfile, "System Name State:\n");
	fprintf(outfile, "==============================\n");

	GetNameStats(dlUserGlobals.pNamtab,	&NameStats);

	fprintf(outfile, "# Named Lists:	%d\n", NameStats.numNamedLists);
	fprintf(outfile, "# Named Materials:%d\n", NameStats.numNamedMaterials);
	fprintf(outfile, "# Named Meshes:	%d\n", NameStats.numNamedMeshes);
	fprintf(outfile, "# Named Convexes:	%d\n", NameStats.numNamedConvex);
	fprintf(outfile, "# Named Textures:	%d\n", NameStats.numNamedTextures);
	fprintf(outfile, "# Named Cached Texts:%d\n", NameStats.numNamedCachedTextures);
	
	/*
	// fetch a pointer to the instanced list, by using name table
	*/
	if(listname == SGL_DEFAULT_LIST)
	{
		pList = dlUserGlobals.pDefaultList;
		NodeType = nt_list_node;
	}
	else if(listname == SGL_NULL_LIST)
	{
		/*
		// This list is always empty, so don't do anymore
		*/
		fprintf(outfile, "==============================\n");
		return;
	}
	else
	{
		pList = GetNamedItemAndType(dlUserGlobals.pNamtab, listname, &NodeType);
	}
	/*
	// if this is NOT a valid list, then get the out of this
	// routine, and record that the user has supplied us with
	// a load of nonsense
	*/
	if((pList == NULL) || NodeType != nt_list_node)
	{
		SglError(sgl_err_bad_name);
		/*
		// ABORT  - this is the cleanest way to handle this...
		*/
		return;
	}

	ASSERT(pList->node_hdr.n16_node_type == nt_list_node);


	fprintf(outfile, "==============================\n");
	fprintf(outfile, "Recursive Dump of List:%d\n", listname);
	fprintf(outfile, "==============================\n");

	/*
	// Recursively traverse the list
	*/
	DumpAndRecurseList(pList, outfile, traverse_flags, 0);


	fprintf(outfile, "==============================\n");
	fflush(outfile);
	/*
	// All went OK
	*/
	SglError(sgl_no_err);
}
/*
// ------- END OF FILE ---------
*/
