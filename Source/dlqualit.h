/*************************************************************************
* Name : dlqualit.h
* Title : Display List quality node
* Author : Simon Fenney
* Created : 12/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of display list quality node type for SGL
*
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dlqualit.h,v $
 * Revision 1.6  1997/09/11  10:23:10  erf
 * Added QFE_DITHERING enum to allow control of dithering.
 *
 * Revision 1.5  1997/03/25  10:21:37  erf
 * Added texture filtering control in the form of defines and variables
 * to indicate which filtering setting has been selected.
 *
 * Revision 1.4  1995/11/02  19:48:58  sjf
 * Added fog quality setting.
 *
 * Revision 1.3  1995/11/02  16:55:31  jop
 * Added quality default flags init from ini file
 *
 * Revision 1.2  1995/09/25  18:07:46  sjf
 * Defined quality structure properly.
 *
 * Revision 1.1  1995/06/12  18:48:37  sjf
 * Initial revision
 *
*
*
*
**************************************************************************/

typedef enum 
{
	QFE_SMOOTH_SHAD,
	QFE_TEXTURE,
	QFE_COLLISIONS,
	QFE_SHADOWS,
	QFE_FOG,
	QFE_TEXTURE_FILTERING,
	QFE_DITHERING
} QUAL_ENUM;

typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// Unlike materials, this only defines ONE change to the quality
	// settings. The following field defines what is being changed
	*/
	QUAL_ENUM WhatsSet;

	/*
	// Generic On/Off
	*/
	sgl_bool Enable;

	/*
	// For collision detection, need a second flag
	*/
	sgl_bool Enable2;

	/*
	// For bilinear filtering, need a variable.
	*/
	sgl_texture_filter_type eFilterType;

} QUALITY_NODE_STRUCT;


int GetDefaultQualityFlags (void);
void InitDefaultQualityFlags (void);



/*
// END OF FILE
*/
