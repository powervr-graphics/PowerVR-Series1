/*************************************************************************
* Name : dlmater.h
* Title : Display List Material node structures
* Author : Simon Fenney
* Created : 1/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of all the material node stuctures
*				for SGL Display lists
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dlmater.h,v $
 * Revision 1.18  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.17  1996/07/08  04:23:42  sjf
 * Added routine to support debug output of display lists.
 *
 * Revision 1.16  1996/06/20  20:29:31  jop
 * Added a flag that makes it easier to tell if texture wrapping is on.
 * In both convex and meshes texture wrapping was being attempted when
 * the UVs weren't defined, and were completely ignoring the fact that
 * smaps and omaps weren't defined.
 *
 * Revision 1.15  1996/06/06  23:03:20  sjf
 * Added material instancing.
 *
 * Revision 1.14  1996/05/30  20:31:20  sjf
 *  Added texture caching.
 * ,
 *
 * Revision 1.13  1995/09/28  10:34:14  pdl
 * removed the smap axis stuff.
 *
 * Revision 1.12  1995/08/09  17:25:11  pdl
 * altered the opacity variables.
 *
 * Revision 1.11  1995/08/02  13:21:28  sm
 * remove a comma
 *
 * Revision 1.10  1995/07/30  18:50:05  sjf
 * Had a quiet clash of identifiers - (with is_local) - renamed things to fix it
 *
 * Revision 1.9  1995/07/24  17:06:25  pdl
 * added the texture removal stuff.
 *
 * Revision 1.8  1995/07/23  17:33:39  pdl
 * replaced the texture name with the control word.
 *
 * Revision 1.7  1995/06/29  14:54:35  jrc
 * Added prototype for DlDeleteMaterialNodeRefs.
 *
 * Revision 1.6  1995/06/05  15:55:57  jop
 * Corrected spelling of DlCompleteCurrentMaterial
 *
 * Revision 1.5  1995/06/01  19:45:07  jop
 * Added is_local flag and sorted out affect_xxx bitfield
 *
 * Revision 1.4  1995/06/01  11:16:59  jrc
 * Removed space from $Log $ to enable log message output in header.
 *
**************************************************************************/


/*
// Define the optional texture wrap structures.
*/
typedef struct
{
	sgl_smap_types	smap;
	float su, sv, ou, ov;
	float radius;
	
	sgl_omap_types omap;
	float		   refrac_index;

} TEXT_WRAP_STRUCT;




/*
// Define the data flags for material node. These MUST be bit positions
// but are grouped together in an enum for neatness....
*/
typedef enum
{
	mnf_has_diffuse		= 0x001,
	mnf_has_ambient		= 0x002,
	mnf_has_specular	= 0x004,
	mnf_has_glow		= 0x008,
	mnf_has_opacity		= 0x010,

	mnf_has_text_map	= 0x020,
	mnf_has_text_effect	= 0x040,

	mnf_has_smap		= 0x080,
	mnf_has_omap		= 0x100,

	mnf_is_local		= 0x200,

	mnf_remove_texture  = 0x400,

	mnf_instance_ref	= 0x800

} MATERIAL_NODE_FLAGS_ENUM;


/*
// Define the texture effect fields in the same manner, except note that
// the flip_U and flip_V texture bits are set to exactly align with the
// required field in the hardware. This is done to reduce work in the 
// rendering. (See TRDD026x.doc and TRDD027x.doc)
//
// texture_on is also set to exactly align with the required field in the
// hardware.
*/
typedef enum
{
	affect_ambient	= (1 << 5),
	affect_diffuse	= (1 << 6),
	affect_specular	= (1 << 7),
	affect_glow		= (1 << 8),

	/*
	// A flag which indicates if texture wrapping is defined
	// (A valid Smap and Omap must be defined)
	*/
	wrapping_on 	= (1 << 9),

	/*
	// Do the flip U and flip V flags. THESE ARE DEFINED BY THE HARDWARE.
	*/
	mirror_u		= MASK_FLIP_U,
	mirror_v		= MASK_FLIP_V,

	texture_on		= MASK_TEXTURE
	
} MATERIAL_NODE_TEXT_EFFECT_ENUM;

/*
// For convenience, define masks for the Smaps and Omaps
*/
#define SMAP_BITS (sgl_smap_plane | sgl_smap_cylinder | sgl_smap_sphere)

#define OMAP_BITS ( sgl_omap_obj_normal | sgl_omap_inter_normal | \
					sgl_omap_reflection | sgl_omap_transmission)



/*
// Define the material node structure
*/
typedef struct
{
	/*
	// Put in the basic node header
	*/
	DL_NODE_STRUCT node_hdr;
	
	/*
	// The data flags (we need < 16 bits)
	// These are built from ORS of
	//  "MATERIAL_NODE_FLAGS_ENUM" values
	*/
	int	data_flags;

	/*
	// The various "colour" fields
	*/
	sgl_colour diffuse_colour;
	sgl_colour ambient_colour;

	sgl_colour specular_colour;
	int		   specular_shininess;
	float	   specular_shininess_float;

	
	sgl_colour glow_colour;

	/*
	// Opacity. 1.0=> opaque, 0.0 transparent.
	//			Also store the harware INTEGER equivalent.
	//			This can be packed into the correct bits, I guess
	//			by a extern hardware routine...
	*/
	float opacity;
	int	  translucent_int;

	/*
	// text_map "name", and texture effect. The texture effect
	// is defined by a combination of 
	// "MATERIAL_NODE_TEXT_EFFECT_ENUM" values.
	//
	// If the texture is cached, store a pointer to it.
	*/
	void * 	pcached_texture;
	sgl_uint32	texture_control;  /*contains the address and sizes etc in TEXAS format*/
	int	text_effect;

	/*
	// Pointer to the optional wrap data
	*/
	TEXT_WRAP_STRUCT * pwrap_data;

	/*
	// If the "instance ref" flag is set, then all of the above should
	// be ignored. The only thing that is relevant is the name below.
	*/
	int instanced_material_name;

} MATERIAL_NODE_STRUCT;









/*//////////////////////////////////////////////////////////
// Material Utility Routines
////////////////////////////////////////////////////////////
*/
void DlCompleteCurrentMaterial (void);
void DlDeleteMaterialNodeRefs(DL_NODE_STRUCT *pNode);

void DumpMaterial(MATERIAL_NODE_STRUCT* pNode, FILE * outfile,
							char * pPaddingString);



/*---------------------------- End of File -------------------------------*/



