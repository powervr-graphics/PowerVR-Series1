/*************************************************************************
* Name : dllights.h
* Title : Display List Light nodes
* Author : Simon Fenney
* Created : 1/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of display list light node types for SGL
*
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dllights.h,v $
 * Revision 1.6  1995/09/25  18:07:03  sjf
 * Added multi shadow structure.
 *
 * Revision 1.5  1995/08/28  16:52:27  sjf
 * Added extra light concentration value.
 *
 * Revision 1.4  1995/06/12  18:40:36  sjf
 * Fixed log keyword.
 *
*
*
*
**************************************************************************/

/*////////////////////////////////////////////////////////
// Define the light Position node (so it can be referenced
// within the light node struct).
//
// Note that it, in turn, references the list structure type.
//////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/* The name of the positioned light */
	int light_name; 
	
	/*
	// Pointer to this nodes parent list, so that we can
	// backtrack up the display list efficiently
	*/
	LIST_NODE_STRUCT	* pparent_list;

}LIGHT_POS_NODE_STRUCT;


/*////////////////////////////////////////////////////////
//
// Define the light switch node - first define the flags
//
//////////////////////////////////////////////////////////
*/
typedef enum
{
	switch_light_on		 = 0x1,
	switch_shadows_on	 = 0x2,
	switch_highlights_on = 0x4

}LIGHT_SWITCH_FLAGS;

typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// Light name
	*/
	int light_name;

	/*
	// Switches - these are a combination of
	//			LIGHT_SWITCH_FLAGS
	*/
	int light_switches;

} LIGHT_SWITCH_NODE_STRUCT;


/*////////////////////////////////////////////////////////
// Basic Light Node
//
// First define the light flags...
// NOTE: This also includes definitions for the Light state flags
//		 as described in TRDD027x.doc
// These flags are done as an ENUM but they occupy various unique
// bit positions (where necessary)
//////////////////////////////////////////////////////////
*/
typedef enum
{
	/*
	// Assign bits 0-2 to the type of the light...
	// Also define a Light type mask, which is just the bits
	// used by the light types...
	*/
	ambient_light_type 	= 1,
	parallel_light_type	= 2,
	point_light_type	= 3,
	mask_light_types	= 3,

	

	/*
	// define the other single bit flags,
	// which are part of light node....
	*/
	ambient_relative 	= 0x04,
	casts_shadows		= 0x08,
	smooth_highlights	= 0x10,
	coloured			= 0x20,


	/*
	// define the flags which are part of the 
	// light state light entry flags, and aren't
	// already defined for the light node.
	*/
	light_on			= 0x40,
	highlights_on		= 0x80


} LIGHT_FLAGS_ENUM;




/*
// define the light node structure
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// The following flags value is made up of combinations
	// of LIGHT_FLAGS_ENUM  values.
	*/
	int flags;

	/*
	// define the lights colour or grey level...
	*/
	sgl_colour colour;

	/*
	// define direction and position vectors (remember 
	// that the direction vector is assumed to be a
	// unit vector.
	*/
	sgl_vector direction,
			   position;

	/*
	// Define concentration for a spot(point) light, and
	// an equivalent "power" multiplier.
	*/
	int   concentration;
	float log2concentration;

	/*
	// Optional reference to a light position node
	*/
	LIGHT_POS_NODE_STRUCT * plight_position;

} LIGHT_NODE_STRUCT;

/*////////////////////////////////////////////////////////
// Multi Shadow Node
//
// This just changes the shadow mode - allowing or disabling
// the pseudo multi shadow mode
//////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// boolean to either enable or disable pesudo multi shadows
	*/
	sgl_bool Enable;

	/*
	// If enabling, the colour for the "shadow" when non shadowed
	// (Confused? well I am)
	*/
	sgl_colour colour;
} MULTI_SHADOW_NODE_STRUCT;


/*---------------------------- End of File -------------------------------*/



