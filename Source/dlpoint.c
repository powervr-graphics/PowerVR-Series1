/******************************************************************************
 * Name         : dlpoint.c
 * Title        : Collision/position point display list editing SGL functions.
 * Author       : John Catchpole
 * Created      : 01/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dlpoint.c,v $
 * Revision 1.22  1997/05/13  17:53:02  mjg
 * Replaced PVROS memory management call with an SGL one.
 *
 * Revision 1.21  1997/04/30  19:18:54  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.20  1997/04/29  14:45:53  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.19  1997/04/24  20:54:55  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.18  1997/04/08  12:04:47  gdc
 * ensured n16pathlength is initialised on point creation as this bizarely
 * started causing a page fault
 *
 * Revision 1.17  1997/04/02  21:27:38  mjg
 * Updated calling conventions.
 *
 * Revision 1.16  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.15  1996/10/04  15:07:53  sjf
 * Fixed silliness with a ";" following an if statement that really shouldn't
 * have been there!
 *
 * Revision 1.14  1996/01/09  12:10:23  jrc
 * The seven collision information values returned by sgl_query_point are now
 * passed back in a single struct.
 *
 * Revision 1.13  1995/09/27  13:11:39  jrc
 * 1. Added missing AppendNodeToList for sgl_switch_point.
 * 2. Added missing setup of parent list pointer for sgl_position_point.
 *
 * Revision 1.12  1995/09/15  18:03:48  jrc
 * Finished sgl_query_point.
 *
 * Revision 1.11  1995/06/26  09:20:10  jrc
 * Removed sgl.h from #includes because it is now included by sgl_defs.h
 *
 * Revision 1.10  1995/06/13  12:21:55  jrc
 * No longer assumes that NULL is zero.
 *
 * Revision 1.9  1995/06/13  11:21:37  jrc
 * Changed 'light' references to 'point'.
 *
 * Revision 1.8  1995/06/12  16:18:38  sjf
 * Added point switch node processing, and fixed undefined name problem
 * with position nodes.
 *
 * Revision 1.7  1995/06/08  16:10:50  jrc
 * Added node deletion cleanup routines.
 *
 * Revision 1.6  1995/06/05  15:02:06  jrc
 * Added calls to DlCompleteCurrentMesh.
 *
 * Revision 1.5  1995/06/05  12:29:38  jrc
 * Added some { }s to 'if' statements.
 *
 * Revision 1.4  1995/06/02  16:53:52  jrc
 * Added MODULE_ID definition.
 *
 * Revision 1.3  1995/06/01  12:37:53  jrc
 * No longer checks sglSystemInitialised value.
 *
 * Revision 1.2  1995/06/01  12:00:37  jrc
 * Simplified #includes by using dlnodes.h.
 *
 * Revision 1.1  1995/06/01  11:45:34  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_DLPOINT

#include "sgl_defs.h"
#include "sgl_init.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "sgl_math.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"

/*
// ============================================================================
// 							  SGL INTERNAL ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: DlDeletePointNodeRefs
 *
 * Inputs       : pNodeHdr
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Cleanup processing before deletion of a point display list
 *				  node.
 *****************************************************************************/
void DlDeletePointNodeRefs(DL_NODE_STRUCT *pNodeHdr)
{
	POINT_NODE_STRUCT *pPointNode;

	/*
	// ==============
	// GET POINT NODE
	// ==============
	*/
	ASSERT(pNodeHdr != NULL);
	pPointNode = (POINT_NODE_STRUCT*)pNodeHdr;

	/*
	// =============================
	// REMOVE REFERENCE TO THIS NODE
	// =============================
	*/
	if (pPointNode->ppoint_position != NULL)
	{
		/*
		// --------------------------
		// Check reference is correct
		// --------------------------
		*/
		ASSERT(pPointNode->ppoint_position->point_name ==
		  pPointNode->node_hdr.n16_name);

		/*
		// ------------------------------
		// Unregister and clear reference
		// ------------------------------
		*/
		DecNamedItemUsage(dlUserGlobals.pNamtab,pPointNode->node_hdr.n16_name);
		pPointNode->ppoint_position->point_name = NM_INVALID_NAME;
	}
}


/******************************************************************************
 * Function Name: DlDeletePointPosNodeRefs
 *
 * Inputs       : pNodeHdr
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Cleanup processing before deletion of a point position
 *				  display list node.
 *****************************************************************************/
void DlDeletePointPosNodeRefs(DL_NODE_STRUCT *pNodeHdr)
{
	POINT_POSITION_NODE_STRUCT *pPosNode;

	ASSERT(pNodeHdr != NULL);
	pPosNode = (POINT_POSITION_NODE_STRUCT*)pNodeHdr;

	/*
	// If there is a corresponding point node for this position node then clear
	// the point node's pointer to this position node:
	*/
	if (pPosNode->point_name != NM_INVALID_NAME)
	{
		/*
		// Set the point node's pointer to this position node to NULL:
		*/
		ASSERT( GetNamedItemType(dlUserGlobals.pNamtab, pPosNode->point_name)
		  == nt_point );

		( (POINT_NODE_STRUCT*)
		  GetNamedItem(dlUserGlobals.pNamtab, pPosNode->point_name) )
		  ->ppoint_position = NULL;

		/*
		// Unregister this use of the point node's name:
		*/
		DecNamedItemUsage(dlUserGlobals.pNamtab, pPosNode->point_name);
	}
}


/******************************************************************************
 * Function Name: DlDeletePointSwitchNodeRefs
 *
 * Inputs       : pNodeHdr
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Cleanup processing before deletion of a point switch display
 *				  list node.
 *****************************************************************************/
void DlDeletePointSwitchNodeRefs(DL_NODE_STRUCT *pNodeHdr)
{
	POINT_SWITCH_NODE_STRUCT *pSwitchNode;

	/*
	// ---------------------
	// Get point switch node
	// ---------------------
	*/
	ASSERT(pNodeHdr != NULL);
	pSwitchNode = (POINT_SWITCH_NODE_STRUCT*)pNodeHdr;

	/*
	// ----------------------------------
	// Unregister reference to point node
	// ----------------------------------
	*/
	ASSERT(pSwitchNode->n16_point_name != NM_INVALID_NAME);
	DecNamedItemUsage(dlUserGlobals.pNamtab, pSwitchNode->n16_point_name);
}


/*
// ============================================================================
// 								SGL API ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: sgl_create_point
 *
 * Inputs       : offset,bCollisionCheck
 * Outputs      : -
 * Returns      : name or error status
 * Globals Used : dlUserGlobals.pNameTab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_create_point( sgl_vector offset, 
								sgl_bool bCollisionCheck )
{
    /*
	// ----------
	// Initialise
	// ----------
	*/
    POINT_NODE_STRUCT *pPointNode;

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	/*
	// ---------
	// Make node
	// ---------
	*/
    pPointNode = NEW(POINT_NODE_STRUCT);
	if (pPointNode == NULL)
	{
        return SglError(sgl_err_no_mem);
	}

    pPointNode->node_hdr.n16_node_type = nt_point;
    pPointNode->node_hdr.n16_name =
      AddNamedItem(dlUserGlobals.pNamtab, pPointNode, nt_point);
    if (pPointNode->node_hdr.n16_name == sgl_err_no_name)
    {
	    SGLFree(pPointNode);
		return SglError(sgl_err_no_name);
    }

    VecCopy(offset, pPointNode->offset);
	pPointNode->collision_check = bCollisionCheck;
    pPointNode->ppoint_position = NULL;
	pPointNode->n16PathLength = 0;
	/*
	// -------------------
	// Add to display list
	// -------------------
	*/
    AppendNodeToList(dlUserGlobals.pCurrentList, pPointNode);

    /*
	// ------
	// Return
	// ------
	*/
    return SglError(pPointNode->node_hdr.n16_name);
}


/******************************************************************************
 * Function Name: sgl_set_point
 *
 * Inputs       : nName, offset, bCollisionCheck
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNameTab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_point( int nName, sgl_vector offset, 
							  sgl_bool bCollisionCheck )
{
    POINT_NODE_STRUCT *pPointNode;

    /*
	// ----------
	// Initialise
	// ----------
	*/
#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	/*
	// ---------
	// Edit node
	// ---------
	*/
    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_point)
    {
		SglError(sgl_err_bad_name);
		return;
	}
    pPointNode = GetNamedItem(dlUserGlobals.pNamtab, nName);
	ASSERT(pPointNode != NULL);

    VecCopy(offset, pPointNode->offset);
	pPointNode->collision_check = bCollisionCheck;

    /*
	// ------
	// Return
	// ------
	*/
    SglError(sgl_no_err);
	return;
}


/******************************************************************************
 * Function Name: sgl_switch_point
 *
 * Inputs       : nName,bEnableCheck
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_switch_point( int nName, sgl_bool bEnableCheck )
{
	POINT_SWITCH_NODE_STRUCT *pSwitchNode;
#if DEBUG
    POINT_NODE_STRUCT *pPointNode;
#endif

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_point)
    {
		SglError(sgl_err_bad_name);
		return;
	}

	/*
	// Create a node to do the switching
	*/
	pSwitchNode = NEW(POINT_SWITCH_NODE_STRUCT);
	if (pSwitchNode == NULL)
	{
		SglError(sgl_err_no_mem);
		return;
	}

#if DEBUG
    pPointNode = GetNamedItem(dlUserGlobals.pNamtab, nName);
	ASSERT(pPointNode->node_hdr.n16_node_type == nt_point);
	ASSERT(pPointNode->node_hdr.n16_name == nName);
#endif

	/*
	// Set the stored values in the node appropriately
	*/
	pSwitchNode->node_hdr.n16_node_type = nt_point_switch;
	pSwitchNode->node_hdr.n16_name		= NM_INVALID_NAME;

	pSwitchNode->n16_point_name	= nName;

	/* The following guarantees it fits in 16 bits) */
	pSwitchNode->n16_enable_check = (bEnableCheck != 0);

	/*
	// Increment the usage count on the name
	*/
	IncNamedItemUsage(dlUserGlobals.pNamtab, nName);

	/*
	// -------------------
	// Add to display list
	// -------------------
	*/
    AppendNodeToList(dlUserGlobals.pCurrentList, pSwitchNode);

    /*
	// ------
	// Return
	// ------
	*/
    SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_position_point
 *
 * Inputs       : nName
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab, dlUserGlobals.pCurrentList
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_position_point( int nName )
{
    POINT_POSITION_NODE_STRUCT *pPosNode;
    POINT_NODE_STRUCT          *pPointNode;

    /*
	// ----------
	// Initialise
	// ----------
	*/
#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_point)
    {
		SglError(sgl_err_bad_name);
		return;
	}

	/*
	// ------------------
	// Make position node
	// ------------------
	*/
    pPosNode = NEW(POINT_POSITION_NODE_STRUCT);
	if (pPosNode == NULL)
    {
		SglError(sgl_err_no_mem);
		return;
	}

	/*
	// Fill in the header info (type and "unused" name):
	*/
    pPosNode->node_hdr.n16_node_type = nt_point_pos;
    pPosNode->node_hdr.n16_name		 = NM_INVALID_NAME;

    pPosNode->point_name = nName;

	/*
	// Set reference to the current list so that we can backtrack up the list
	// during traversal
	*/
	pPosNode->pparent_list = dlUserGlobals.pCurrentList; 

	/*
	// -------------------------------
	// Update references in point node
	// -------------------------------
	*/
    pPointNode = GetNamedItem(dlUserGlobals.pNamtab, nName);
	ASSERT(pPointNode != NULL);

	/*
	// If this point already has a position node, then stop its effect first:
	*/
	if (pPointNode->ppoint_position != NULL)
	{
		/*
		// Reset the old position node's name field to be invalid:
		*/
	    pPointNode->ppoint_position->point_name = NM_INVALID_NAME;
	}
	else
	{
		/* Increment the usage count of the point node's name: */
		IncNamedItemUsage(dlUserGlobals.pNamtab, nName);
	}

	/*
	// Store the pointer to the position node in the point.
	*/
	pPointNode->ppoint_position = pPosNode;

	/*
	// ---------------------------------
	// Add position node to display list
	// ---------------------------------
	*/
    AppendNodeToList(dlUserGlobals.pCurrentList, pPosNode);

    /*
	// ------
	// Return
	// ------
	*/
    SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_query_point
 *
 * Inputs       : nName
 * Outputs      : position, pCollisionData
 * Returns      : error status
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_query_point( int nName, sgl_vector position,
							   sgl_collision_data *pCollisionData )
{
    POINT_NODE_STRUCT *pPointNode;

	/*
	// ----------
	// Initialise
	// ----------
	*/
#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif
	/*
	// --------------
	// Get point node
	// --------------
	*/
    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_point)
	{
        return SglError(sgl_err_bad_name);
	}
    pPointNode = GetNamedItem(dlUserGlobals.pNamtab, nName);
	ASSERT(pPointNode != NULL);

	/*
	// ---------------------
	// Read point's position
	// ---------------------
	// We are not spending time checking for 'position' being NULL because if
	// the pointer is wrong it is unlikely to be NULL.
	*/
	VecCopy(pPointNode->position, position);

	/*
	// --------------------------
	// Read collision information
	// --------------------------
	// It would be quicker to copy this information as a struct copy.
	*/
    if (pCollisionData != NULL)
    {
		if ((pCollisionData->collision = (sgl_bool)pPointNode->n16Collision))
		{
			int nItem;

			pCollisionData->object_name = (int)pPointNode->n16ObjectName;

			ASSERT(pPointNode->n16ObjectPlane >= 0);
			pCollisionData->object_plane = pPointNode->n16ObjectPlane;

			VecCopy(pPointNode->normal, pCollisionData->normal);

			pCollisionData->d = pPointNode->fD;

			ASSERT(pPointNode->n16PathLength >= 0); /* >= 1? */
			ASSERT(pPointNode->n16PathLength <= SGL_MAX_PATH);
			for (nItem = 0; nItem < pPointNode->n16PathLength; nItem++)
			{
				pCollisionData->path[nItem] = (int)pPointNode->pn16Path[nItem];
			}
			pCollisionData->path_length = pPointNode->n16PathLength;
		}
	}

	/*
	// ------
	// Return
	// ------
	*/
    return SglError(sgl_no_err);
}

/*------------------------------- End of File -------------------------------*/
