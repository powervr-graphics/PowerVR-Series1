/******************************************************************************
 * Name : dldelete.c
 * Title : Display List, "delete" routine
 * Author : Simon Fenney
 * Created : 1/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : various SGL Routines for deleting a node ..
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dldelete.c,v $
 * Revision 1.14  1997/11/07  15:30:07  mjg
 * Added a cast to remove a compiler warning.
 *
 * Revision 1.13  1997/05/13  17:51:03  mjg
 * Replaced PVROS memory management call with an SGL one.
 *
 * Revision 1.12  1997/04/30  19:18:09  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.11  1997/04/29  14:42:08  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.10  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.9  1996/10/15  13:56:02  gdc
 * added shadow limit node to cases handed for deletion
 *
 * Revision 1.8  1996/06/13  14:26:58  sjf
 * Fixed problem with debug versions - the case of nt_newtran wasn't handled
 * by the deletion.
 *
 * Revision 1.7  1996/05/21  13:59:30  sjf
 * Added new node type that is simply skipped over (makes deleting easy!)
 *
 * Revision 1.6  1995/07/14  15:31:02  sjf
 * Fixed syntax error in ASSERT statement, now that it works!!
 *
 * Revision 1.5  1995/07/05  16:54:07  jrc
 * Removed old dummy case for mesh node.
 *
 * Revision 1.4  1995/07/05  14:37:03  jrc
 * Added mesh node deletion.
 *
 * Revision 1.3  1995/06/12  16:17:55  sjf
 * Added point switch node.
 *
 * Revision 1.2  1995/06/07  10:54:42  sjf
 * Removed obsolete "fog" node processing.
 *
 * Revision 1.1  1995/06/06  15:39:20  sjf
 * Initial revision
 *
 *
 *
 *****************************************************************************/

/*
// define Debug Module IDs
*/
#define MODULE_ID	MODID_DL

#include "sgl_defs.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "sgl_init.h"
#include "dldelete.h"
#include "pvrosapi.h"
#include "sglmem.h"


/**************************************************************************
 * Function Name  : DlDeleteNode
 * Inputs         : pNode - pointer to a VALID node
 * Outputs        : None
 * Returns        : None
 * Global Used    : Name table, other referenced nodes...
 * Description    : Deletes the node. This requires the routine to delete any
 *					extra referenced memory, cleanup names, references to and from
 *					other nodes, and name reference counts wherever.
 *
 *					NOTE: It does NOT remove the node from its display list.
 *					This is left up to calling routines whenever necessary
 *
 *					The order of the operations is
 *						1) Clean up extra memory allocations hanging off the
 *						the node, any cross references to other nodes, and
 *						decrement reference counts.
 *
 *						2) Delete its name (for types which can be named)
 *
 *						3) Delete/Free the memory of the main node struct
 *
 *					If the deleted node is one of the "current" items, then that
 *					global pointer is set to NULL. IE this applies to  
 *
 *						   current list
 *						   current transform
 *						   current convex
 *						   current mesh
 *						   current material
 *					(Actually, I think the only one of concern is the current list
 *					because any routine that calls this one will have finished the
 *					current items anyway - In fact, I think the DEBUG version 
 *					should check that this is the case)	
 *
 **************************************************************************/

void DlDeleteNode(DL_NODE_STRUCT * pNode)
{
	/*
	// Name and type of the node
	*/
	int name, type;


	name = pNode->n16_name;
	type = pNode->n16_node_type;

	/*
	// Assertion check to see if this is a valid node...
	*/
	ASSERT((type >= 0) && (type < nt_node_limit))



	/*
	// Do cross reference and extra memory cleaning up by calling
	// specialised routines, wherever necessary.
	//
	// Also, if the deleted item was a current thing, set the current
	// pointer to NULL.
	*/
	switch(type)
	{
		/*
		// List node
		*/
		case nt_list_node:
			 DlDeleteListNodeRefs(pNode);
			 if((LIST_NODE_STRUCT*) pNode == dlUserGlobals.pCurrentList)
			 {
			 	dlUserGlobals.pCurrentList = NULL;
			 }
			 
			 break;
		/*
		// Instance refs
		*/	 
		case nt_instance:
			 DlDeleteInstanceNodeRefs(pNode);
			 break;

		/*
		// Instance Substitution references
		*/
		case nt_inst_subs:
			 DlDeleteSubstitutionNodeRefs(pNode);
			 break;
		
		/*
		// Convex primitives
		*/
		case nt_convex:
			 DlDeleteConvexNodeRefs(pNode);
			 if((CONVEX_NODE_STRUCT*)pNode == dlUserGlobals.pCurrentConvex)
			 {
			 	ASSERT(FALSE)
			 	dlUserGlobals.pCurrentConvex = NULL;
			 }
			 break;

		/*
		// Mesh node
		*/
		case nt_mesh:
			 DlDeleteMeshNodeRefs(pNode);
			 if((MESH_NODE_STRUCT*)pNode == dlUserGlobals.pCurrentMesh)
			 {
			 	ASSERT(FALSE)
			 	dlUserGlobals.pCurrentMesh = NULL;
			 }
			 break;

			
		/*
		// Material defs
		*/
		case nt_material:
			 DlDeleteMaterialNodeRefs(pNode);
			 if((MATERIAL_NODE_STRUCT *)pNode == dlUserGlobals.pCurrentMaterial)
			 {
			 	ASSERT(FALSE)
			 	dlUserGlobals.pCurrentMaterial = NULL;
			 }
			 break;

		/*
		// Light defs
		*/
		case nt_light:
			 DlDeleteLightNodeRefs(pNode);
			 break;

		/*
		// Light position node
		*/			 
		case nt_light_pos:
			 DlDeleteLightPosNodeRefs(pNode);
			 break;

		/*
		// Light switch node
		*/			 
		case nt_light_switch:
			 DlDeleteLightSwitchNodeRefs(pNode);
			 break;

		/*
		//  Level of detail node
		*/
		case nt_lod:
			 DlDeleteLodNodeRefs(pNode);
			 break;

		/*
		//  point node
		*/
		case nt_point:
			 DlDeletePointNodeRefs(pNode);
			 break;
		/*
		//  point position node
		*/
		case nt_point_pos:
			 DlDeletePointPosNodeRefs(pNode);
			 break;

		/*
		//  point switch node
		*/
		case nt_point_switch:
			 DlDeletePointSwitchNodeRefs(pNode);
			 break;

		/*
		// The transform is a simple structure, but
		// has a global current pointer
		*/
		case nt_transform:	
			 if((TRANSFORM_NODE_STRUCT*)pNode == dlUserGlobals.pCurrentTransform)
			 {
			 	ASSERT(FALSE)
			 	dlUserGlobals.pCurrentTransform = NULL;
			 }
			 break;


		/*
		// SIMPLE Node types - i.e. NO cross references etc.
		*/	
		case nt_camera:
		case nt_quality:
		case nt_dummy:
	    case nt_newtran:
	    case nt_shadow_limit:
		{
			break;
		}

		/*////////////////////////
		// Any LEFT OVER CASES are errors
		/////////////////////// */
		default:
			ASSERT(FALSE);
			break;

	} /*end switch*/
	
	/*
	// Does this node have a name. NOTE. It is ASSUMED that this
	// field is initialised either to a valid name or the constant
	// NM_INVALID_NAME
	*/
	if(name != NM_INVALID_NAME)
	{
		/*
		// DEBUG check that it IS in fact there...
		// and that it is of the correct type
		*/
		ASSERT((void*)pNode == GetNamedItem(dlUserGlobals.pNamtab,	name))
		ASSERT(type == 	GetNamedItemType(dlUserGlobals.pNamtab,	name))

		/*
		// Remove it from the name table
		*/
		DeleteNamedItem(dlUserGlobals.pNamtab,pNode->n16_name);
		
	}

	
	/*
	// Free the memory
	*/
	SGLFree(pNode);
	

}

/*
// ----------------------  END OF FILE ----------------------------
*/

