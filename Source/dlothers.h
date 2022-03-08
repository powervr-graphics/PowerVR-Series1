/*************************************************************************
* Name : dlothers.h
* Title : Display List nodes (left overs...)
* Author : Simon Fenney
* Created : 1/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of display list node types for SGL, which
*				aren't very large, and so may as well be lumped into one file.
*
*				The node types in this file are...
*					transforms
*					cameras
*
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dlothers.h,v $
 * Revision 1.9  1997/06/04  15:30:33  gdc
 * stopped multiple inclusions
 *
 * Revision 1.8  1995/09/04  15:29:07  sm
 * changed for new translucent instr
 *
 * Revision 1.7  1995/08/18  17:04:35  sjf
 * Made some fogging values precalculated.
 *
 * Revision 1.6  1995/07/21  09:40:34  sjf
 * Removed aspect factor as no longer part of camera spec.
 *
 * Revision 1.5  1995/06/07  12:05:05  jrc
 * Added background colour and fog parameters to the camera node structure.
 *
 * Revision 1.4  1995/05/31  17:56:51  sjf
 * Added prototype.
 *
 * Revision 1.3  1995/05/25  15:06:22  sjf
 * Fixed space in log command???
 *
**************************************************************************/
#ifndef __DLOTHERS_H__
#define __DLOTHERS_H__

/*///////////////////////////////////////////////////
// define the transform node type
/////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// Contains the common transform type...
	*/
	TRANSFORM_STRUCT transform;

} TRANSFORM_NODE_STRUCT;





/*///////////////////////////////////////////////////
// define the camera node type
/////////////////////////////////////////////////////
*/

typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;
	
	/*
	// Get a pointer to our parent list
	*/
	LIST_NODE_STRUCT * pparent;

	/*
	// Define the fields needed for a basic camera
	*/
	float zoom_factor;
	
	float foreground_dist;
	float inv_background_dist;

	/*
	// Background colour:
	*/
	sgl_colour backgroundColour;


	/*
	// Fog parameters: 
	// These are converted from the raw user format..
	// The RGB float colour is converted to the packed INT format
	// while the fog density is converted into a fraction and 
	// pwer value.
	*/
	sgl_map_pixel FogCol;

	float invlogfogFraction;
	int	  logfogPower;

} CAMERA_NODE_STRUCT;


/*//////////////////////////////////////////////////
// define new transucent layer node
///////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// basic node header
	*/

	DL_NODE_STRUCT node_hdr;

} NTRAN_NODE_STRUCT;



/*//////////////////////////////////////////////////////////
// Node Utility Routines
////////////////////////////////////////////////////////////
*/
void DlCompleteCurrentTransform(void);

#endif

/*---------------------------- End of File -------------------------------*/
