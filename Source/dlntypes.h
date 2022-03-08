/*************************************************************************
* Name : dlntypes.h
* Title : Display List node types
* Author : Simon Fenney
* Created : 1/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of SGL display list node types. It also includes
*			  the other "nameable" entity types (eg viewports) which aren't
*			  actually part of the display list, but must be stored in the
*			  name table.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dlntypes.h,v $
 * Revision 1.12  1997/05/01  15:38:29  mjg
 * Replaced INT16 with sgl_int16.
 *
 * Revision 1.11  1996/10/15  11:56:35  gdc
 * added enum type for shadow limit node
 *
 * Revision 1.10  1996/05/30  20:31:31  sjf
 *  Added texture caching.
 *
 * Revision 1.9  1996/05/21  14:00:59  sjf
 * Added new node type that is simply skipped over (makes deleting easy!)
 *
 * Revision 1.8  1995/09/25  18:07:22  sjf
 * Added multi shadow node type.
 *
 * Revision 1.7  1995/09/04  15:27:52  sm
 * added type for newtran
 *
 * Revision 1.6  1995/06/12  16:18:17  sjf
 * Added point switch ndoe.
 *
 * Revision 1.5  1995/06/07  10:50:44  sjf
 * Removed fog node as the values will be stored in the camera...
 *
 * Revision 1.4  1995/06/06  15:41:33  sjf
 * Added a "debug" value to node type enumeration to allow easy
 * range check on possible node types.
 *
 * Revision 1.3  1995/05/25  14:18:43  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.2  1995/05/05  16:12:20  sjf
 * Added the basic display list node header to the file.
 *
 * Revision 1.1  1995/05/01  15:50:23  sjf
 * Initial revision
 *
*
*
**************************************************************************/

#ifndef __DLNTYPES_H__
#define __DLNTYPES_H__



/*
// Define the types of node in the display list tree
//
// At the end, also include the other entity types which can
// be refered to by name.
//
// IT IS ASSUMED THAT THESE WILL FIT IN 16 bits. Because we cannot
// make an "enum" use a specific number of bits (it automatically uses
// int) we will use another type when refering to the stored data.
//
*/
typedef enum
{
	nt_list_node,		/* List node type id  */
	nt_instance,		/* Instance reference */
	nt_inst_subs,		/* Substitutions for instancing */
	
	nt_transform,		/* Transformation */
	
	nt_convex,			/* convex polyhedron primitive */
	nt_mesh,			/* mesh primitive */

	nt_material,		/* Material defs */
	
	nt_light,			/* Light type */
	nt_light_pos,		/* Light positioning node */
	nt_light_switch,	/* Light switch node */
	nt_multi_shadow,	/* Multi shadow switch node */

	nt_camera,			/* Camera node */

	nt_lod,				/* Level of detail node */

	nt_point,			/* collision / feedback point*/
	nt_point_pos,		/* point position */
	nt_point_switch,	/* collision point On/Off */

	nt_quality,			/* Quality setting node */

	nt_newtran,			/* new translucent set node */

	nt_shadow_limit,    /* shadow limiting node */

	nt_dummy,			/* A dummy node so we can "delete" things without
						// having to disconnect them from their parent list
						*/

	/*
	// Define a value that comes immediately after the last valid
	// node type so that we can easily do debug testing for 
	// out-of-range values
	*/
	nt_node_limit,


	/*
	// Define the types that DONT reside in the display list
	// BUT still must be refered to by name.
	//
	// Start counting from further up, so that it is obvious
	// these are separate from the display list nodes
	*/
	nt_device = 100,	
	nt_viewport,
	nt_cached_texture,
	nt_texture

} NODE_TYPES_ENUM;






/*
// Define the basic Node header type
*/
typedef struct _DL_NODE_STRUCT
{
	/* 
	// Define the type of the node. This is really of 
	// type NODE_TYPES_ENUM but we only want 16 bits
	*/
	sgl_int16	n16_node_type; 

	/*
	// Optional name.
	*/
	sgl_int16 	n16_name;


	struct _DL_NODE_STRUCT * next_node;


} DL_NODE_STRUCT;



#endif  /* __DLNTYPES_H__ */

/*---------------------------- End of File -------------------------------*/


