/******************************************************************************
 * Name         : dlpoint.h
 * Title        : Definition of the "point" related display list nodes
 *				  of structures etc.
 * Author       : Simon Fenney
 * Created      : 09 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Defines the display lists nodes needed to store points
 *				  and position them.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dlpoint.h,v $
 * Revision 1.12  1997/06/18  14:38:45  mjg
 * Replaced windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.11  1996/09/09  19:05:33  jrc
 * Added nCollisionStateIndex to POINT_NODE_STRUCT (collision detection
 * position point bug fix).
 *
 * Revision 1.10  1995/09/27  13:16:43  jrc
 * Changed the parent list name in the point position node type to a pointer
 * for consistency with the equivalent for lights.
 *
 * Revision 1.9  1995/09/15  18:09:22  jrc
 * Added values for reading by sgl_query_point.
 *
 * Revision 1.8  1995/06/13  11:21:37  jrc
 * Changed 'light' references to 'point'.
 *
 * Revision 1.7  1995/06/12  16:19:29  sjf
 * Added point switch node structure.
 *
 * Revision 1.6  1995/06/08  12:34:37  jrc
 * Added declarations for node deletion cleanup routines.
 *
 * Revision 1.5  1995/06/05  16:37:30  jrc
 * Changed int in display list structures to INT16.
 *
 * Revision 1.4  1995/05/25  14:19:05  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.3  1995/05/22  12:06:29  jrc
 * Added '*' for ppoint_position pointer.
 *
 * Revision 1.2  1995/05/22  10:17:05  jrc
 * 1. Changed 'light' to 'point'.
 * 2. Added collision_check.
 *
 * Revision 1.1  1995/05/09  16:17:23  sjf
 * Initial revision
 *
 *****************************************************************************/


typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// The name of the positioned point.  This is needed in case we delete
	// the position information and we have to refer back to the point.
	*/
	sgl_int16 point_name;

	/*
	// Pointer to this nodes parent list, so that we can backtrack up the
	// display list efficiently
	*/
	LIST_NODE_STRUCT *pparent_list;

} POINT_POSITION_NODE_STRUCT;


typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// Define the offset from the local coords
	*/
	sgl_vector offset;

	/*
	// Whether collision detection is required at all.  This is not changed
	// during a render, and must be non-zero for bEnableCollision to have any
	// effect.
	*/
	sgl_bool collision_check;

	/*
	// Whether collision detection is switched on.  During render this is set
	// TRUE at the beginning of the scope of the point, and may subsequently be
	// changed by a point switch node.
	*/
	sgl_bool bEnableCollision;

	/*
	// Index into the collision state array so that point position nodes can
	// update the world coordinate position.
	//
	// This value is set to -1 if collision detection is not enabled for the
	// point.
	*/
	int nCollisionStateIndex;

	/*
	// A pointer to the position node for this point (NULL if no positioning
	// node)
	*/
	POINT_POSITION_NODE_STRUCT *ppoint_position;

	/*
	// Items provided by the render code to be read by sgl_query_point.
	*/
	sgl_vector	position;  /* 3D position relative to origin (not camera) */
	sgl_int16	n16Collision;  /* boolean: Whether or not hit in this render */
	sgl_int16	n16ObjectName;
	sgl_int16	n16ObjectPlane;
	sgl_vector	normal;
	float		fD;
	sgl_int16	pn16Path[SGL_MAX_PATH]; /* high SGL_MAX_PATH will consume RAM */
	sgl_int16	n16PathLength;

} POINT_NODE_STRUCT;


typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// Name of the point being switched.
	// NOTE: If the corresponding point node is deleted this value will not be
	// changed, though the name will not be reused until this node is deleted.
	*/
	sgl_int16 n16_point_name;

	/*
	// Set whether we are enabling or disabling collision detection.
	// This is effectively a BOOL, but we'll pack it into 16 bits so hopefully
	// it will abut the point name above.
	*/
	sgl_int16 n16_enable_check;

} POINT_SWITCH_NODE_STRUCT;


extern void DlDeletePointNodeRefs(DL_NODE_STRUCT *pNodeHdr);
extern void DlDeletePointPosNodeRefs(DL_NODE_STRUCT *pNodeHdr);
extern void DlDeletePointSwitchNodeRefs(DL_NODE_STRUCT *pNodeHdr);


/*------------------------------- End of File -------------------------------*/
