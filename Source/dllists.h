/*************************************************************************
* Name : dllists.h
* Title : Display List,  List nodes
* Author : Simon Fenney
* Created : 1/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of display list list node types for SGL
*				Also includes the instance and instance substitution nodes.
*
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dllists.h,v $
 * Revision 1.9  1997/06/18  14:38:21  mjg
 * Replaced windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.8  1996/05/09  13:43:53  sjf
 * Added facility to ignore display lists.
 *
 * Revision 1.7  1995/06/26  12:01:27  sm
 * added protos for del light nodes
 *
 * Revision 1.6  1995/06/06  15:41:04  sjf
 * Added the specialised delete routines to delete cross references.
 *
 * Revision 1.5  1995/05/30  12:27:22  sjf
 * Added append to list utility routine.
 *
 * Revision 1.4  1995/05/26  12:48:30  jrc
 * Removed space from $Log $.
 *
*
*
*
**************************************************************************/

/*////////////////////////////////////////////////////////
// Define the list node type
//////////////////////////////////////////////////////////
*/

/*
// List Flags
//
// Process List: This flag indicates whether we should process this
//		   list, or skip over it during display traversal.
//
// Preserve State:  indicates if we must preserve the state
//			variables before entering the list, and then
//			restore them afterwards.
*/
typedef enum
{
	lf_process_list   = (1<<0),
	lf_preserve_state = (1<<1)

}list_flags_enum;

typedef struct _LIST_NODE_STRUCT
{


	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// Pointer to the lists parent (if any)
	*/
	struct _LIST_NODE_STRUCT * pparent;

	/*
	// Flags (see above)
	*/
	list_flags_enum flags;

	/*
	// Pointers to the first and last node of the list
	*/
	DL_NODE_STRUCT * pfirst;
	DL_NODE_STRUCT * plast;

} LIST_NODE_STRUCT;




/*////////////////////////////////////////////////////////
// Define the instance node type
//////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// The  name of the list being referenced
	*/
	int	referenced_list;

} INSTANCE_NODE_STRUCT;

/*////////////////////////////////////////////////////////
// Define the instance substitution node
//////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// The number of substitions in the following array...
	*/
	int 	num_subs;

	/*
	// List of the substitutions to make
	*/
	sgl_int16	param_list[SGL_MAX_INSTANCE_PARAMS][2];

} INSTANCE_SUBS_NODE_STRUCT;



/*////////////////////////////////////////////////////////
// Define Utility routines for Lists type nodes
//////////////////////////////////////////////////////////
*/




extern void	InitListNode(LIST_NODE_STRUCT * pListNode,
						int name,
						LIST_NODE_STRUCT * pParentList);

extern void  DlDeleteListNodeRefs(DL_NODE_STRUCT *pNode);

extern void AppendNodeToList(LIST_NODE_STRUCT *  pParentList,
							void * pNode);




extern void  DlDeleteInstanceNodeRefs(DL_NODE_STRUCT *pNode);
extern void  DlDeleteSubstitutionNodeRefs(DL_NODE_STRUCT *pNode);


extern void 	DlDeleteLightPosNodeRefs(DL_NODE_STRUCT	 * pNode);
extern void		DlDeleteLightSwitchNodeRefs(DL_NODE_STRUCT	 * pNode);
extern void		DlDeleteLightNodeRefs(DL_NODE_STRUCT	* pNode);




/*---------------------------- End of File -------------------------------*/
