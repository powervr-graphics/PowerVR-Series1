/******************************************************************************
 * Name : dlconvex.h
 * Title : Display List convex node types
 * Author : Simon Fenney
 * Created : 1/05/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : Definition of convex primitive node for SGL.
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dlconvex.h,v $
 * Revision 1.29  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.28  1996/10/15  11:49:31  gdc
 * added a new node type for shadow limit planes
 *
 * Revision 1.27  1996/10/14  13:23:07  gdc
 * added new plane type, which indicates a plane which can truncate
 * shadows
 *
 * Revision 1.26  1996/07/08  17:31:03  jop
 * Added new smooth shading mode
 *
 * Revision 1.25  1996/07/05  07:52:28  sjf
 * Removed param from map extern etc.
 *
 * Revision 1.24  1996/06/27  11:38:49  jrc
 * Added warning handling to MapExternalToInternal.
 *
 * Revision 1.23  1996/05/01  13:59:42  sjf
 * Removed sample texel flag.
 *
 * Revision 1.22  1996/04/24  18:00:18  sjf
 * Made plane structure a multiple of 4 bytes in size.
 *
 * Revision 1.21  1996/04/22  08:45:29  sjf
 * Removed incorrect comment
 *
 * Revision 1.20  1996/01/31  14:23:47  jop
 * Added new points/shading structs
 *
 * Revision 1.19  1995/11/07  18:30:42  jop
 * Removed a couple of redundant parameters from s/s struct and
 * added one to get proper mobile rep points in smooth sh.
 *
 * Revision 1.18  1995/09/19  08:59:57  sjf
 * changed if of "invisible" convex to hidden, as this was less
 * confusing wrt invisible planes.
 *
 * Revision 1.17  1995/08/23  16:24:23  jop
 * Added pf_reversed flag
 *
 * Revision 1.16  1995/08/22  17:22:27  jop
 * Added texel sampling stuff
 *
 * Revision 1.15  1995/08/14  12:05:44  jop
 * Added some members to the shading structure
 *
 * Revision 1.14  1995/08/01  16:59:59  jop
 * Made MapExternalToInternal accessable to outside functions
 *
 * Revision 1.13  1995/07/30  11:20:48  sjf
 * Changed bounding box definition as there are now two flavours of
 * bounding box.
 *
 * Revision 1.12  1995/07/06  16:17:16  sjf
 * Added shadow volume convex type, and "see inside" flag to the
 * primitive.
 *
 * Revision 1.11  1995/07/05  17:15:02  sjf
 * Added a "sub type" of convex primitive.
 *
 * Revision 1.10  1995/06/28  14:20:24  jrc
 * Added sentry against multiple inclusion.
 *
 * Revision 1.9  1995/06/23  15:22:41  sjf
 * Used special bounding box type.
 *
 * Revision 1.8  1995/06/23  15:15:30  jrc
 * Added comment for DlConvexAddLocalMaterial.
 *
 * Revision 1.7  1995/06/22  12:04:00  jrc
 * Added prototype for DlDeleteConvexNodeRefs.
 *
 * Revision 1.6  1995/06/12  15:00:17  jrc
 * Changed typo double underscore to single underscore.
 *
 * Revision 1.5  1995/06/02  13:23:20  jop
 * Added prototype for DLConvexAddLocalMaterial, called from
 * dlmater.c
 *
 * Revision 1.4  1995/06/01  10:52:26  jrc
 * Removed space from $Log $ to enable log output.
 *
 *****************************************************************************/
#ifndef __DLCONVEX_H
#define __DLCONVEX_H


/*////////////////////////////////////////////////////////////////////
// Define the edge list structure. This is a "variable" size structure -
// which has a variable length array of "edges" as the final field. Each
// edge element will be defined, then the total edge list will be defined
// to contain ONE of these elements. Its up to the code to allocate things
// correctly....
//////////////////////////////////////////////////////////////////////
*/
typedef struct
{
	sgl_uint16 u16_plane1, u16_plane2;

	sgl_vector mid_point;

} EDGE_DATA_STRUCT;


typedef struct
{
	int num_edges;

	EDGE_DATA_STRUCT edges[1];
} EDGE_INFO_STRUCT;





/*////////////////////////////////////////////////////////////////////
// Define the Plane data (i.e. not including texture or shading)
//
// Define the legal flag values. To keep them grouped, use an enum with
// specific bit patterns. This is a little neater than #defines.
//
// Note that these are combined using approriate ORing etc.
//////////////////////////////////////////////////////////////////////
*/
typedef enum
{
	pf_visible			= 0x01,
	pf_textured			= 0x02,
	pf_pre_mapped		= 0x04,
	pf_smooth_shad		= 0x08,
	/*pf_sample_texel		= 0x10,  Eliminating this option*/
	pf_reversed			= 0x20,
	pf_shadow_plane     = 0x40
}plane_flags_enum;


typedef struct
{
	/*
	// define flags for the plane. These are
	// built from combinations of plane_flags_enum values.
	*/
	sgl_uint32 flags;

	/*
	// Surface Normal - this MUST be a unit normal
	*/
	sgl_vector normal;

	/*
	// Representative point
	*/
	sgl_vector rep_point;

} CONV_PLANE_STRUCT;




/*////////////////////////////////////////////////////////////////////
// Define the texture data.
//
// This defines one element of an array of texture data for the planes.
// There is a choice of two possible versions, which is controlled by
// the flags on the plane - hence the union.
//
//////////////////////////////////////////////////////////////////////
*/
typedef union
{
	struct
	{
		sgl_vector u_vect; 	/*vector giving unit change in u*/
		sgl_vector v_vect; 	/*unit change in v*/
		sgl_vector o_vect; 	/*Origin of texture map on plane*/

	} pre_mapped;

	struct
	{
		sgl_2d_vec	RepUV;	/* for sampled texture pixel workaround*/
		sgl_vector	TexelColour;
	} sample_texel;
	
} CONV_TEXTURE_UNION;

/*////////////////////////////////////////////////////////////////////
// Define points structure - used by texture wrapping & smooth shading
//////////////////////////////////////////////////////////////////////
*/
typedef struct tagCONV_POINTS_STRUCT
{
	sgl_vector	pt1;
	sgl_vector	pt2_delta;
	sgl_vector	pt3_delta;

} CONV_POINTS_STRUCT;


/*////////////////////////////////////////////////////////////////////
// Define the smooth shading data
//////////////////////////////////////////////////////////////////////
*/
typedef struct
{
	sgl_vector	norm1;
	sgl_vector	norm2;
	sgl_vector	norm3;

	float		f1OverQpaRpbMinusQpbRpa;
	sgl_int8		nQ, nR;

	float		fAdjoint[2][3];
	float		f1OverDet;
	float		fRepDeltaX, fRepDeltaY;
	
} CONV_SHADING_STRUCT;



/*
// Define the convex node flags:
// These include the "type" of the convex primitive, and
// also single bit flags
*/
typedef enum
{
	/*
	// define the "types" of convex primitive
	// and a mask value to get the type easily
	//
	// CURRENTLY USES 2 BITS,
	*/
	cf_standard_convex	= 0x00,
	cf_hidden_convex	= 0x01,
	cf_light_volume		= 0x02,
	cf_shadow_volume	= 0x03,

	/*
	// Mask for the above
	*/
	cf_mask_type		= 0x03,


	/*
	// define the single bit flags, in the bits
	// above the type.
	*/
	cf_has_bbox 	 = (1 << 2),
	cf_all_premapped = (1 << 3),
	cf_all_text_wrap = (1 << 4),
	cf_all_smooth	 = (1 << 5),
	cf_all_visible	 = (1 << 6),
	cf_see_inside	 = (1 << 7)

} CONVEX_FLAGS_ENUM;


/*////////////////////////////////////////////////////////////////////
// Define the local materials structures
//
// This is one element of an array of local materials.
//
// NOTE: the material node structure definition must have already
//		 been made.
//////////////////////////////////////////////////////////////////////
*/
typedef struct
{
	int planes_affected;
	int material_name;
	MATERIAL_NODE_STRUCT * p_material;
} LOCAL_MATERIALS_STRUCT;



/*////////////////////////////////////////////////////////////////////
//
// Define the convex node type struct
//
//////////////////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// number and max planes space allocated in struct
	*/
	sgl_uint16  u16_num_planes;
	sgl_uint16	u16_max_planes;

	/*
	// Flags - see CONVEX_FLAGS_ENUM for legal bit
	// positions.
	*/
	sgl_uint16 u16_flags;

	/*
	// If this is a Light or Shadow Volume, this stores the name
	// of the light associated with the volume
	*/
	sgl_uint16 u16_volumes_light;

	/*
	// Pointer to the edge information - if any
	*/
	EDGE_INFO_STRUCT * edge_info;

	/*
	// optional bounding box (see flag information)
	*/
	BBOX_CENT_STRUCT bbox;

	/*
	// pointer to the list of plane data. Note this
	// points to the first element of an array of planes
	*/
	CONV_PLANE_STRUCT * plane_data;

	/*
	// pointer to the texturing info. Note that this points
	// to the first element of an array of texture data.
	*/
	CONV_TEXTURE_UNION * texture_data;
	
	/*
	// pointer to the shading information. Again, this
	// points to the first element of an array of
	// smooth shading data
	*/
	CONV_SHADING_STRUCT * shading_data;

	/*
	// 3 points used by texture wrapping/smooth shading
	*/
	CONV_POINTS_STRUCT * points_data;

	/*
	// Local materials, house-keeping fields
	*/
	sgl_uint16 u16_num_materials;
	sgl_uint16 u16_max_materials;

	/*
	// Pointer to the local materials array structure. This is a
	// pointer to the first element (if any)
	*/
	LOCAL_MATERIALS_STRUCT * local_materials;

} CONVEX_NODE_STRUCT;


/*//////////////////////////////////////////////////
// define a shadow limit plane (1 plane per) node
///////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// basic node header
	*/

	DL_NODE_STRUCT node_hdr;

    /*
	// pointer to a single convex plane
	// which is the shadow limit plane
	*/
	CONV_PLANE_STRUCT plane_data;

} SHAD_LIM_NODE_STRUCT;


/*
// -----------------------
// DlCompleteCurrentConvex
// -----------------------
*/
void DlCompleteCurrentConvex(void);


/*
// ------------------------
// DlConvexAddLocalMaterial
// ------------------------
// Returns sgl_no_err or sgl_err_no_mem.
*/
int DlConvexAddLocalMaterial(MATERIAL_NODE_STRUCT *pMaterialNode);


/*
// ----------------------
// DlDeleteConvexNodeRefs
// ----------------------
*/
void DlDeleteConvexNodeRefs(DL_NODE_STRUCT *pNodeHdr);

/* map uv coords to U,V vectors and texture map origin O - needed by dlmesh.c */

void MapExternalToInternal(sgl_vector P0, sgl_vector P1, sgl_vector P2,
  sgl_2d_vec UV0, sgl_2d_vec UV1, sgl_2d_vec UV2,
  sgl_vector U, sgl_vector V, sgl_vector O);

#endif
/*------------------------------- End of File -------------------------------*/
