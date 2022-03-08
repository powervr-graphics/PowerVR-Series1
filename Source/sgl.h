 /******************************************************************************
 * Name : sgl.h
 * Title : Prototypes for SGL library calls
 * Author : VideoLogic Limited
 * Created : 18/04/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : Prototypes for SGL library calls
 * Full descpitions of the following SGL functions can
 * be found in the document trdd025a
 *
 * Platform : ANSI compatible
 *
 * (internal only version $Revision: 1.228 $ )
 *
 *****************************************************************************/
/*
// If being compiled in C++, inform it that SGL is C
*/
#ifdef __cplusplus
extern "C" {
#endif 

/*############################################################################*/
/*############################################################################*/
/*############################################################################*/
/*  Radical departure time! Ok kids, for version 4.0 onwards we are getting   */
/*  a bit cleverer about controlling our interface definitions, so we are     */
/*  going to do things a little differently. This file is now divided into    */
/*  two halves: the top is types, and the bottom is for function declarations */
/*  by means of	macros. Please do not reorder or add to the function list     */
/*  unless you are sure you wish to commit yourself to invalidating people's  */
/*  binaries without warning.                                                 */
/*############################################################################*/
/*############################################################################*/
/*############################################################################*/

#ifndef __SGL_H__
#define __SGL_H__

/*
// The following builds are legal on the PC:
//
// Windows95: Microsoft C, the intel reference compiler
//			  and watcom are the only supported compilers
//			  though others may work. The WIN32 macro must
//			  be defined to be 1.
//
// DOS4GW:    Watcom must be used. there are no special macros 
//            necessary.
*/

#ifdef WIN32
 #define CALL_CONV __cdecl
#else
 #ifdef __WATCOMC__
  #define CALL_CONV __cdecl
 #else
  #define CALL_CONV
 #endif
#endif



#ifdef MIDAS_ARCADE
 /*
 // Don't want byte packing for MIDAS Arcade
 */
 #else
 /*
 // Byte structure packing for this file
 */
 #ifdef __WATCOMC__
  #ifndef __SW_EI
   #error SGL requires the '-ei' Watcom compiler switch to be set.
  #endif
 #endif
 #pragma pack(1)
#endif


#define SGL_HEADER_VERSION  "1.33"


#define SGL_MAX_PATH		256

/* Maximum number of planes for a single convex object */
#define SGL_MAX_PLANES 		100 

/* Max limits for polygon meshes */

#define SGL_MAX_VERTS 		10000
#define SGL_MAX_FACES 		10000
#define SGL_MAX_FACE_VERTS 	4


/* Max limits for lights */

#define SGL_MAX_ACTIVE_LIGHTS 	8
#define SGL_MAX_SHADOW_LIGHTS 	1
#define SGL_MAX_SPECULAR_LIGHTS 1


/* Max limits for Collision points */
#define SGL_MAX_ACTIVE_POINTS	50


/*
// SGL_FIRST_ERROR is the most negative number an error code can be.  Codes
// more negative than this are warnings (which start at SGL_FIRST_WARNING).
*/
#define SGL_FIRST_ERROR  -255
#define SGL_FIRST_WARNING  -100

/* 
// Error/warning return codes 
*/
enum
{	
	/*
	// No error/warning (This CANT BE NEGATIVE)
	*/
	sgl_no_err = 0, 

	/*
	// ALL errors and warnings MUST be negative
	*/
	sgl_err_no_mem = SGL_FIRST_ERROR,
	sgl_err_no_name,
	sgl_err_bad_name,
	sgl_err_bad_parameter,
	sgl_err_cyclic_reference,
	sgl_err_list_too_deep,
	sgl_err_too_many_planes,
	sgl_err_no_convex,
    sgl_err_no_mesh,
	sgl_err_bad_index,
	sgl_err_failed_init,
	sgl_err_bad_device,
	sgl_err_texture_memory_full,
	sgl_err_colinear_plane_points,
	sgl_err_no_board_found,
	sgl_err_no_library_found,

	sgl_warn_colinear_face_points = SGL_FIRST_WARNING,
	sgl_warn_colinear_uvs,
	sgl_warn_large_uvs,
	sgl_warn_noncoplanar_quad
};


/*
// Define the Special List Names. As these have to be checked specially,
// they can be negative values
*/
#define SGL_DEFAULT_LIST (-1)
#define SGL_NULL_LIST	 (-2)

/*
// Define the texture name to clear the texture.
*/
#define SGL_NULL_TEXTURE (-1)

/*
// The value that represents an unnamed object.
*/
#define SGL_ANON_OBJECT (-1)

/*
// A null material
*/
#define SGL_NULL_MATERIAL SGL_NULL_LIST

/*
// Define boolean values, if not already defined
*/
#ifndef TRUE
	#define TRUE (1)
#endif

#ifndef FALSE
	#define FALSE (0)
#endif
	


/*
// Define standard SGL types
*/
typedef int		sgl_bool;
typedef float	sgl_vector[3];
typedef float	sgl_2d_vec[2];
typedef float	sgl_colour[3];
typedef int		sgl_name;

/*
// Define special integer types
*/ 
typedef signed char 	sgl_int8;
typedef unsigned char	sgl_uint8;
typedef signed short	sgl_int16;
typedef unsigned short	sgl_uint16;
typedef signed long		sgl_int32;
typedef unsigned long	sgl_uint32;
typedef	unsigned int	sgl_uint;

typedef enum
{
	sgl_device_16bit,
	sgl_device_24bit

} sgl_device_colour_types;

typedef enum
{
	SGL_STRING,
	SGL_INT32,
} SGLINIDATATYPE;

#ifdef WIN32
static sgl_vector sgl_x_axis = {1.0f, 0.0f, 0.0f};
static sgl_vector sgl_y_axis = {0.0f, 1.0f, 0.0f};
static sgl_vector sgl_z_axis = {0.0f, 0.0f, 1.0f};
#else
extern sgl_vector sgl_x_axis;
extern sgl_vector sgl_y_axis;
extern sgl_vector sgl_z_axis;
#endif

/*
// Pixel structure for the user-accessible texture map
*/
typedef struct tag_map_pixel
{
	unsigned char	red;
	unsigned char	green;
	unsigned char	blue;
	unsigned char	alpha; /* 0=Fully Opaque, 255=fully transparent*/

} sgl_map_pixel;

/*
// Structure for the intermediate map
*/
typedef struct
{
	/* 
	// ID for type of map. User accessible version must be set to "IMAP".
	// SGL uses other IDs for the pre-processed maps.
	*/
	char id[4];

	/*
	// X and Y dimensions of texture map
	*/
	int	x_dim;
	int	y_dim;

	/*
	// Array of pixels.
	*/
	sgl_map_pixel	* pixels;

} sgl_intermediate_map;


typedef struct
{
	/*
	// Size of this structure in bytes
	*/
	sgl_uint32 u32StructSize;

	/*
	// Number of free blocks for specific texture sizes
	*/
	sgl_uint16 free32_8bit;
	sgl_uint16 free64_8bit;
	sgl_uint16 free128_8bit;
	sgl_uint16 free256_8bit;

	sgl_uint16 free32_16bit;
	sgl_uint16 free64_16bit;
	sgl_uint16 free128_16bit;
	sgl_uint16 free256_16bit;

	sgl_uint16 free32_16bit_mipmap;
	sgl_uint16 free64_16bit_mipmap;
	sgl_uint16 free128_16bit_mipmap;
	sgl_uint16 free256_16bit_mipmap;

	/*
	// Free memory that has not been assigned to specific texture sizes
	*/
	sgl_uint32 u32OtherFreeMem;
	sgl_uint32 u32LargestOtherFreeMem; /* largest contiguous block */

} sgl_texture_mem_info;


typedef enum
{
	sgl_map_16bit,
	sgl_map_16bit_mm,
	sgl_map_8bit,
	sgl_map_trans16,
	sgl_map_trans16_mm

} sgl_map_types;

/* Map Base Sizes */

typedef enum
{
	sgl_map_32x32,
	sgl_map_64x64,
	sgl_map_128x128,
	sgl_map_256x256

} sgl_map_sizes;


/*
// For the Texture Map conversion, define the method used to make MIP map levels.
// There is a choice of not doing it, or two different filters.
//
// Earlier versions used a boolean value with 0=> no automatic MIP map generation
// and...									  1=> a 4x4 filter kernel was used
*/
typedef enum
{
	sgl_mipmap_generate_none,
	sgl_mipmap_generate_2x2,
	sgl_mipmap_generate_4x4
} sgl_mipmap_generation_options;


/*
// Texture Wrap Type Definitions 
//
// For reasons described in the SGL design Document, the map type
// values are desgined to occupy unique sets of bits.
//
//	The Smap will use bits 0-1, and the omap 2-4. (SEE TRDD027x.doc)
*/
 
typedef enum
{
	sgl_smap_plane = 1,
	sgl_smap_cylinder = 2,
	sgl_smap_sphere = 3

} sgl_smap_types;

typedef enum
{
	sgl_omap_obj_normal   = (1 << 2),
	sgl_omap_inter_normal =	(2 << 2),
	sgl_omap_reflection	  =	(3 << 2),
	sgl_omap_transmission = (4 << 2)

} sgl_omap_types;

#define SGL_MAX_INSTANCE_PARAMS 10
#define SGL_MAX_INSTANCE_SUBS   30

typedef enum 
{
	sgl_no_cull, 		/*keeps all faces (default for each mesh)*/
	sgl_cull_anticlock, /*removes anticlockwise faces*/
	sgl_cull_clockwise  /*removes clockwise faces*/
}sgl_cull_mode;

/*
// Needed for sgl_dump_list
*/
#include <stdio.h>

typedef enum
{
	sgl_dumplist_instances = 0x1
} sgl_dumplist_types;

/*
// The following macro calculates the pixel data size of a user accessible 
// intermediate map. This can be used in a malloc type call.
*/
#define	SGL_INTERMEDIATE_MAP_SIZE(x_dim, y_dim) (((long) x_dim * (long) y_dim) * (long) sizeof (sgl_map_pixel))

typedef struct tag_collision_data
{
	sgl_bool   collision;
	int		   object_name;
	int		   object_plane;
	sgl_vector normal;
	float	   d;
	int		   path_length;
	int		   path[SGL_MAX_PATH];

} sgl_collision_data;

/* Use an 'enum' to define the bilinear setting for the bilinear API.
 */
typedef enum
{
	sgl_tf_point_sample		= 0x0,
	sgl_tf_adapt_bilin		= 0x1,
	sgl_tf_bilinear			= 0x2
} sgl_texture_filter_type;

typedef struct
{
	char *library;
	char *required_header;

} sgl_versions;


/*============================================================================
// PowerSGL Direct
*/

typedef struct
{
	int BytesPerPixel; /*2 or 4*/
	sgl_uint32 RedMask;
	sgl_uint32 GreenMask;
	sgl_uint32 BlueMask;
	sgl_uint32 AlphaMask; 
	
/* to keep the size of the struture the same with 4.0.0., added four reserved fields */	
	sgl_uint32 RESERVED1;
	sgl_uint32 RESERVED2;
	sgl_uint32 RESERVED3;
	sgl_uint32 RESERVED4; 

	sgl_bool Alpha0IsOpaque;

}sgl_direct_pixformat_struct;

typedef enum
{
	sgl_direct_src_use_pixformat,     /* Arbitrary texture format, all Masks must been set. */
	sgl_direct_src_4444,              /* 4444 texture format.                               */
	sgl_direct_src_555,               /* 555 texture format.                                */
	sgl_direct_src_1555,              /* 1555 for SGLDirect only. HW supported in PCX2_003. */
	sgl_direct_src_555CK,             /* 555CK texture format, for SGLDirect only.          */
	sgl_direct_src_565,               /* 565 texture format.                                */
	sgl_direct_src_565CK,             /* 565CK texture format, for SGLDirect only.          */
	sgl_direct_src_fast_palette_4444, /* fast palette in 4444 format                        */
	sgl_direct_src_fast_palette_555,  /* fast palette in 555 format                         */

} sgl_direct_srctype;

/*
// ---------------
// SGLTRIANGLETYPE
// ---------------
// These values are different bits that may be ORed together to form the
// material for the face.
*/

typedef enum
{
	SGLTT_GOURAUD 			= 0x00000001, /* leave unset for flat shading */
	SGLTT_TEXTURE 			= 0x00000002,
	SGLTT_HIGHLIGHT 		= 0x00000004,

	/*** 0x00000008 and 0x00000010 are reserved flags: ensure they are set to zero! ***/

	SGLTT_GLOBALTRANS 		= 0x00000020,
	SGLTT_WRAPU 			= 0x00000040, /* for D3D: ignored if         */
	SGLTT_WRAPV 			= 0x00000080, /*   bDoUVTimesInvW is not set */

	/*** 0x00000100 is a reserved flag: ensure it is set to zero! ***/

	SGLTT_VERTEXTRANS 		= 0x00000200, /* use vertex alpha */
	SGLTT_MIPMAPOFFSET		= 0x00000400, /* see n32MipmapOffset */
	SGLTT_FACESIND3DFORMAT	= 0x00000800,
	SGLTT_USED3DSTRIPFLAGS	= 0x00001000, /* ignored if the above is not set */
	SGLTT_DISABLEZBUFFER	= 0x00002000,
	SGLTT_AVERAGECOLOUR		= 0x00004000, /* average colour for flat shading */
	SGLTT_VERTEXFOG			= 0x00008000, /* vertex fog (not as fast as global fog) */
	SGLTT_TRANSBACKGROUND	= 0x00010000, /*** Reserved flag: set to zero! ***/
	SGLTT_NEWPASSPERTRI		= 0x00020000, /* use a separate pass for each translucent triangle */
	SGLTT_OPAQUE			= 0x00040000, /* put trans objects in single pseudo-opaque pass */
	SGLTT_BILINEAR			= 0x00080000, /* enable bilinear for current set of polygons */

	/*** 0x00100000 is a reserved flag: ensure it is set to zero! ***/

	SGLTT_NONPERSPTEX		= 0x00200000, /* non perspective correction option */
	SGLTT_BOGUSINVZPROVIDED	= 0x00400000, /* Bogus Z value is provided by the user */
	SGLTT_DISABLEDITHERING	= 0x00800000, /* Used to disable dithering, set to 1 to disable */

	/*** 0x01000000..0x04000000 are reserved flags: ensure they are set to zero! ***/

	SGLTT_SELECTTRANSSORT   = 0x08000000, /* Used to enable selection of a trans sort method via the SGLTRANSSORT enum in the context */
	SGLTT_DEPTHBIAS         = 0x10000000, /* Used to enable depth biasing */

	/*** 0x20000000 is a reserved flag: ensure it is set to zero! ***/

	SGLTT_DIRECT_EXTENSIONS = 0x40000000, /* Reserved as flag to indicate future extensions */
	SGLTT_HAL_RESERVED	    = 0x80000000  /* Reserved as flag to indicate future extensions */

} SGLTRIANGLETYPE;

typedef enum
{
	NO_SHADOWS_OR_LIGHTVOLS,
	ENABLE_SHADOWS,
	ENABLE_LIGHTVOLS

} SGLSHADOWTYPE;

typedef enum
{
	ALWAYS_RENDER_ALL_REGIONS,
	DONT_RENDER_EMPTY_REGIONS

} SGLRENDERREGIONS;

typedef enum
{
	NO_SORT = 1,   /* Assuming SGLTT_NEWPASSPERTRI = 0, then each call
				   ** to add a set of translucent primitives generates a translucent
				   ** pass and these passes are emitted in the order they are 
				   ** recieved
				   */

	MINZ_SORT = 3, /* Assuming SGLTT_NEWPASSPERTRI = 0, a minZ for each 
				   ** call is calculated and this value is used to sort passes
				   ** into output Z order
				   */

	/* the enabling of SGLTT_NEWPASSPERTRI means above statements apply to 
	** individual primitives within a call, and consequently many more translucent
	** passes are generated
	*/

} SGLTRANSSORT;


/*
// ----------
// SGLCONTEXT
// ----------
*/

typedef struct tagSGLCONTEXT
{

	/*
	// Enable fog:
	*/
	sgl_bool  bFogOn;


	/*
	// Fog colour:
	*/
	float  fFogR,fFogG,fFogB;  /* 0 <= colour component <= 1 */


	/*
	// Fog density
	// 
	// values range from 0 (hardly any fog) to 31 (very foggy)
	// This only affects triangles that have been added with bFogOn set to
	// TRUE
	*/
	sgl_uint32	u32FogDensity;

	sgl_bool  bCullBackfacing; /* non-zero to cull backfacing triangles */


	/*
	// Material type for the triangles, using ORed members of SGLTRIANGLETYPE.
	// These flags are specific to the set of triangles currently being added
	// to the frame's scene.
	*/
	sgl_uint32  u32Flags;


	/*
	// Texture name for texturing triangles:
	*/
	int  nTextureName;

	/*
	// Reserved : set to 0.0f
	*/
	float	RESERVED;

	/*
	// Enable/Disable Clipping.  If zero then all vertices must be on screen.
	// Note; This currently only refers to XY clipping and not Z.
	*/
	sgl_bool bDoClipping;

	/*
	// Scene background colour
	*/
	sgl_colour	cBackgroundColour;

	/*
	// Shadow or Light volume mode
	//
	// Only one mode can be used per scene; a single mode must be selected
	// on a per scene basis and must be correct before calling 
	// sgltri_startofframe()
	// 
	*/
	SGLSHADOWTYPE	eShadowLightVolMode;

	union
	{
		/*
		// Shadow brightness
		//
		// Can take values between 0.0 and 1.0. A value of 0 makes shadows
		// completely dark, 0.5 half the brightness of surrounding objects.
		// 1.0 gives no shadowing at all, but adds the shadow parameter 
		// processing. Only enabled if eShadowType is ENABLE_SHADOWS. Different
		// values may be selected between primitive routine calls.
		// 
		*/
		float fShadowBrightness;
	
		/*
		// light volume colour (D3DCOLOR format)
		//
		// This is the colour ADDED to the environment whenever a light
		// volume intersects with it. Only enabled if eShadowType is 
		// ENABLE_LIGHTVOLS. Different values may be selected between 
		// primitive routine calls.
		//
		*/
		sgl_uint32  u32LightVolColour;
	} u;

	/*
	// Flip U and V : Equivalent to D3D Mirror
	*/
	sgl_bool	bFlipU, bFlipV;

	/*
	// If vertices are in d3d format multiply them by fInvW
	// Please set this TRUE for the best performance with future hardware
	// revisions.
	*/
	sgl_bool	bDoUVTimesInvW;


	int		FirstXRegion, LastXRegion, FirstYRegion, LastYRegion;
	float	invRegX, invRegY;

	/* 
	// 0 (Transparent) - 255 (Opaque), though there are only 16 different
	// levels of translucency.  This value is ignored if the SGLTT_GLOBALTRANS
	// flag is not set.
	*/
	sgl_uint32	u32GlobalTrans; 

	/*
	// Number of mipmap level shifts to apply to the current set of triangles.
	// SGLTT_MIPMAPOFFSET must be set in u32Flags for this value to take
	// effect.
	// The best range for compatibility with future hardware revisions is
	// -2..2
	*/
	sgl_int32  n32MipmapOffset;

	/*
	// See enum above
	*/
	SGLRENDERREGIONS  RenderRegions;

	/* Define the bilinear type for the current context.
	 */
	sgl_texture_filter_type	eFilterType;

	/* Width of line or size of point in pixels for line/point support
	 */
	sgl_uint32 uLineWidth;
	
	/* The (confusingly named) bogus inverse W value 
	 * which can be specified by the user.
	 * Used ONLY when SGLTT_BOGUSINVZPROVIDED is set.
	 */
	float	fBogusInvZ;

	/*
	// Reserved: set to 0
	*/
	sgl_uint32 u32Reserve1;

	/* this field selects a translucent pass emission method if its use
	** is enabled via the u32Flags SGLTT_SELECTTRANSSORT
	** This field is only read in sgltri_startofframe() and the sort 
	** applies to the whole frame
	*/ 
	SGLTRANSSORT  eTransSortMethod;

	sgl_uint32   uDepthBias;

	/* Reserved fields used to pad the context structure out to a standard size.
	 * These must be set to 0 as they may be used in future revisions.
	 */
	sgl_uint32 u32Reserve3[3];

} SGLCONTEXT, *PSGLCONTEXT;


/*
// ---------
// SGLVERTEX
// ---------
// This structure is currently physically equivalent to Direct3D's D3DTLVERTEX
// (so one can be cast to the other), with the exception that Direct3D's u and
// v values must be multiplied by the 1/w value.  RAVE vertices already contain
// u/w and v/w.
*/

typedef struct tagSGLVERTEX
{

	/*
	// Projected screen coordinates:
	*/
	float fX; /* 0 <= fX < screen width, unless SGLCONTEXT.bDoClipping!=0 */
	float fY; /* 0 <= fY < screen height, unless SGLCONTEXT.bDoClipping!=0 */
	float fZ; /* ignored */
	/*
	// 1/w for the perspective projection (should be 1.0
	// at minimum distance from viewer, and 0.0 at maximum):
	*/
	float fInvW;

	/*
	// Material colour (D3DCOLOR format):
	*/
	sgl_uint32  u32Colour;


	/*
	// Specular colour (D3DCOLOR format):
	*/
	sgl_uint32  u32Specular;


	/*
	// Texture coordinates (actually just u and v):
	*/
	float  fUOverW, fVOverW;

} SGLVERTEX, *PSGLVERTEX;


/* Define for the bogus inverse W increment. Used when Z buffering
 * is disabled. i.e. the driver controls the 1/w value to specify
 * depth.
 * If using the BOGUSINVZ_INCREMENT value in specifing the bogus
 * inverse Z value one needs to divide it by the INVZ_SCALE_VALUE.
 *
 * Apologies for confusing name malarkey - for Z read W 
 */
#define BOGUSINVZ_INCREMENT			(1.0f / ((float) 0x80000000))
#define INVZ_DEFAULT_SCALE_VALUE	0.01f


/*
// ----------
// SGLD3DFACE
// ----------
*/

/*
// This face is not colinear with the next one.
*/
#define SGLD3DFACEFLAG_START  0x00000000L

/*
// Number of faces after the first one that are colinear with it.  For example
// this allows up to 32-sided convex polygons that use a single face plane.
*/
#define SGLD3DFACEFLAG_STARTFLAT(len)  (len)  /* 1 <= len <= 29 */

/*
// How this face is attached to the preceding one.
*/
#define SGLD3DFACEFLAG_ODD   0x0000001eL
#define SGLD3DFACEFLAG_EVEN  0x0000001fL

typedef struct
{
    sgl_uint16  v1;      /* Vertex indices */
    sgl_uint16  v2;      /* Vertex indices */
    sgl_uint16  v3;      /* Vertex indices */
    sgl_uint16  wFlags;  /* strip/fan flags */

} SGLD3DFACE, *PSGLD3DFACE;


typedef enum
{
	/* Note: Use of these values is not currently reliable. */

	IRC_RENDER_COMPLETE,
	IRC_RENDER_NOT_COMPLETE,
	IRC_RENDER_TIMEOUT

} IRC_RESULT;


/*
// Define the structure used for communicating the cached texture requirements
// between sgl and application
*/
typedef struct
{

	/*
	// SGL and User name for texture
	*/
	int  texture_name;
	long user_id;

	/*
	// Whether it is currently loaded,
	// needed for this render,
	// still in use by a previous render, 
	// (and, if needed) the approximate size in pixels, of the largest
	//  object that does use it. (Note this is the whole object, even if
	// only a small surface of that object uses the texture).
	*/
	sgl_bool loaded;
	sgl_bool needed;
	sgl_bool used_previous_render;
	int		 approx_object_size;

}sgl_tex_callback_struct;

/*
//Define the texture cache callback routine type
*/
typedef void (CALL_CONV *sgl_tex_callback_func)
						(sgl_tex_callback_struct tex_data_array[],
											int	 num_used,
											long free_space);

/*
// driver apis only ... internal use only
*/


#if WIN32

#define SGL_WIN_HEADER_VERSION SGL_HEADER_VERSION

#define SGL_STATUS_RENDER_BUSY		1

/************************************/
/* SGL/Win32 extensions : type defs */
/************************************/

/***********************/
/* PowerVR Device ID's */
/***********************/

#define NEC_VENDOR_ID					0x1033  /* PCI Vendor ID */
#define MIDAS3_PCI_BRIDGE_DEVICE_ID		0x001F  /* PCI Bridge ID */
#define PCX1_DEVICE_ID					0x002A  /* PCX1 device ID */
#define PCX2_DEVICE_ID					0x0046  /* PCX2 device ID */

/*********************/
/* SGL/Win32 status  */
/*********************/

typedef struct
{
	char *required_sglwin32_header;
	char *sgl_vxd_rev;
	char *pci_bridge_vendor_id;
	char *pci_bridge_device_id;
	char *pci_bridge_rev;
	char *pci_bridge_irq;
	char *pci_bridge_io_base;
	char *tsp_rev;
	char *tsp_mem_size;
	char *isp_rev;
	char *mode;
	char *status;
	char *build_info;

} sgl_win_versions;

/**************************************/
/* SGL/Win32 extensions : error codes */
/**************************************/

enum
{	
	sgl_err_ddraw_not_available  = -1024,
	sgl_err_mode_already_defined,
	sgl_err_insufficient_surface_memory,
	sgl_err_graphics_not_capable,
	sgl_err_display_mode_not_supported,
	sgl_err_illegal_address,
	sgl_err_bad_hwnd,
	sgl_err_bad_ddraw_vblank,
	sgl_err_bad_ddraw_flip_status,
	sgl_err_bad_ddraw_flip,
	sgl_err_bad_ddraw_vblank_status,
	sgl_err_bad_ddraw_vblank_wait,
	sgl_err_bad_ddraw_restore,
	sgl_err_bad_ddraw_lock,
	sgl_err_bad_ddraw_unlock,
	sgl_err_bad_ddraw_bltfill,
	sgl_err_bad_ddraw_close,
	sgl_err_bad_ddraw_getcaps,
	sgl_err_bad_ddraw_cooperate,
	sgl_err_bad_ddraw_modechange,
	sgl_err_in_address_callback,
	sgl_err_graphics_locked_out,
	sgl_err_pixel_format_not_supported
};

/************************************/
/* SGL/Win32 extensions : type defs */
/************************************/

typedef struct
{
	/* 
		only use these ddraw types if ddraw.h has already been
		included. Otherwise just void * them to speed compilation
	*/
	#ifdef __DDRAW_INCLUDED__
		LPDIRECTDRAW			pDDObject;
		LPDIRECTDRAWSURFACE		p3DSurfaceObject;
	#else
		#define SGL_DDRAW_STRUCTS_NOT_CONFIGURED
		void					*pDDObject;
		void 					*p3DSurfaceObject;
	#endif
	void					*p3DSurfaceMemory;
	sgl_uint16				wBitsPerPixel;
	sgl_uint32				dwStride;
	sgl_uint16				wWidth;
	sgl_uint16				wHeight;

}CALLBACK_SURFACE_PARAMS, *P_CALLBACK_SURFACE_PARAMS;

typedef struct
{
	void					*pMem;
	sgl_uint16				wStride;
	sgl_uint8				bBitsPerPixel;

}CALLBACK_ADDRESS_PARAMS, *P_CALLBACK_ADDRESS_PARAMS;

typedef int (CALL_CONV *PROC_2D_CALLBACK) (P_CALLBACK_SURFACE_PARAMS);
typedef int (CALL_CONV *PROC_ADDRESS_CALLBACK) (P_CALLBACK_ADDRESS_PARAMS);
typedef int (CALL_CONV *PROC_END_OF_RENDER_CALLBACK) ();

#ifdef _BUILDING_SGL_
typedef enum
{
	SGL_DeviceID,
	SGL_DllHandle,	
	SGL_DeviceType,
	SGL_LogicalDev,
	SGL_TextureIF,
	SGL_APIDev

} SGL_Globals;
#endif

#endif /* WIN32 */


/*
// Revert to previous structure packing mode
*/
#pragma pack()


#endif    /* __SGL_H__ */

/* 
	By default, sgl.h will provide a set of extern prototypes as we know
	and love.
*/

#ifdef _BUILDING_SGL_
#include "pvrif.h"
#else

	#define API_START(x)
	#define API_END(x)
	#define API_FN(ret,name,parms)		ret CALL_CONV name parms;

#endif
 
API_START(SGL)

/******************************
* Windows Routines 
******************************/

#if WIN32
API_FN(sgl_win_versions*, sgl_get_win_versions, (void))
API_FN(int, sgl_use_ddraw_mode,		(void *hWnd, PROC_2D_CALLBACK Proc2d))
API_FN(int, sgl_use_address_mode,	(PROC_ADDRESS_CALLBACK ProcNextAddress, sgl_uint32 **pStatus))
API_FN(int, sgl_use_eor_callback,	(PROC_END_OF_RENDER_CALLBACK ProcEOR))
#endif

/******************************
* Device and Viewport Routines 
******************************/

API_FN(void,	sgl_get_errors, ( int *earliest_error,
									     int *most_recent_error ))

API_FN(int,		sgl_create_screen_device, (  int	device_number,
												int	x_dimension,
												int	y_dimension,
												sgl_device_colour_types device_mode,
												sgl_bool	double_buffer))

API_FN(int,		sgl_get_device, ( int device_name,
									 int * device_number,
									 int * x_dimension,
									 int * y_dimension,
									 sgl_device_colour_types * device_mode,
									 sgl_bool * double_buffer))

API_FN(void,	sgl_delete_device, (int device_name))




API_FN(int ,	sgl_create_viewport, ( int parent_device, 
										 int left, int top, 
										 int right, int bottom, 
										 float cam_rect_left,
										 float cam_rect_top,
										 float cam_rect_right, 
										 float cam_rect_bottom))

API_FN(int ,	sgl_get_viewport, ( int viewport, 
									  int * left, int * top, 
									  int *right, int *bottom, 
									  float *cam_rect_left,
							   		  float *cam_rect_top, 
									  float *cam_rect_right, 
									  float *cam_rect_bottom))

API_FN(int,		sgl_set_viewport, ( int viewport, 
									  int left, int top, 
									  int right, int bottom, 
									  float cam_rect_left,
									  float cam_rect_top,
									  float cam_rect_right,		  
									  float cam_rect_bottom))

API_FN(void,	sgl_subtract_viewport, ( int viewport,
										int removed_viewport))

API_FN(void,	sgl_delete_viewport, (int viewport))

/******************************
* List and Instance Routines 
*
* Also define the Limits associated with 
******************************/

API_FN(int,		sgl_create_list, (sgl_bool generate_name, 
										sgl_bool preserve_state, 
										sgl_bool separate_list))

API_FN(void,	sgl_to_parent, (void))

API_FN(void,	sgl_modify_list, (int list_name, sgl_bool clear_list))

API_FN(void,	sgl_delete_list, (int list_name))

API_FN(void,	sgl_detach_list, (int list_name))

API_FN(void, 	sgl_attach_list, (int list_name))

API_FN(void, 	sgl_set_ignore_list, (int list_name, sgl_bool ignore))

API_FN(void,	sgl_use_instance, (int list_to_instance))

API_FN(void,	sgl_instance_substitutions, (int num_subs, 
										int *instance_params))

API_FN(void,	sgl_dump_list, (int listname, FILE * outfile, 
									 sgl_uint32 traverse_flags))

/****************************/
/* Transformation Rountines */
/****************************/

API_FN(int,		sgl_create_transform, (sgl_bool generate_name))

API_FN(void,	sgl_modify_transform, (int name, sgl_bool clear_transform))

API_FN(void,	sgl_translate, (float x, float y, float z))

API_FN(void,	sgl_rotate, (sgl_vector axis, float angle))

API_FN(void,	sgl_scale, (float x, float y, float z))

/*****************************/
/* Objects creation routines */
/*****************************/

API_FN(int,		sgl_create_convex, (sgl_bool generate_name))

API_FN(int,		sgl_create_light_volume, (const sgl_bool bGenerateName,
											const sgl_bool bLightVol,
											const int LightName))

API_FN(int,		sgl_create_hidden_convex, ( sgl_bool bGenerateName))


API_FN(void, 	sgl_modify_convex, (int name, sgl_bool clear))

API_FN(void,	sgl_add_simple_plane, (sgl_vector surface_point,
										  sgl_vector normal, 
										  sgl_bool invisible))

API_FN(void,	sgl_add_shadow_limit_plane, (sgl_vector surface_point,
										  sgl_vector normal))

API_FN(void,	sgl_add_plane, ( sgl_vector	surface_point,
									sgl_vector	point2,
									sgl_vector	point3,
									sgl_bool	invisible,
									sgl_vector	normal1,
									sgl_vector	normal2,
									sgl_vector	normal3,
									sgl_2d_vec	uv1,
									sgl_2d_vec	uv2,
									sgl_2d_vec	uv3))

API_FN(void,	sgl_set_simple_plane, (  int			plane_index,
											sgl_vector  surface_point,
											sgl_vector  normal,
											sgl_bool 	invisible))

API_FN(void,	sgl_set_plane, ( int 		plane_index,
									sgl_vector	surface_point,
									sgl_vector	point2,
									sgl_vector	point3,
									sgl_bool 	invisible,
									sgl_vector	normal1,
									sgl_vector	normal2,
									sgl_vector	normal3,
									sgl_2d_vec	uv1,
									sgl_2d_vec	uv2,
									sgl_2d_vec	uv3))

API_FN(void,	sgl_delete_plane, (int plane_index))

API_FN(int,		sgl_plane_count, ())


/*****************/
/* Mesh Routines */
/*****************/


API_FN(int,		sgl_create_mesh, (sgl_bool generate_name))

API_FN(void,	sgl_delete_mesh, (int mesh_name))

API_FN(void,	sgl_modify_mesh, (int mesh_name, sgl_bool clear_mesh))

API_FN(void,	sgl_add_vertices, (  int			num_to_add,
										sgl_vector  *vertices,
										sgl_vector  *vertex_normals,
										sgl_2d_vec  *vertex_uvs))

API_FN(void,	sgl_add_face, (int num_face_points, int *vertex_ids))

API_FN(void,	sgl_set_cull_mode, (sgl_cull_mode  cull_mode))

API_FN(int,		sgl_num_vertices, (void))

API_FN(int,		sgl_num_faces, (void))

API_FN(int,		sgl_num_face_vertices, (int face))

API_FN(int,		sgl_get_vertex, (int vertex, 
									   sgl_vector position, 
									   sgl_vector normal, 
									   sgl_2d_vec uv))

API_FN(void,	sgl_set_vertex, (int vertex, 
								sgl_vector position, 
								sgl_vector normal, 
								sgl_2d_vec uv))

API_FN(int,		sgl_get_face, (int face, int *vertex_indices))

API_FN(void,	sgl_set_face, (int face, 
									 int num_vertices, 
									 int *vertex_indices))

API_FN(void,	sgl_delete_vertex, (int vertex))

API_FN(void,	sgl_delete_face, (int face))


/*********************/
/* Material Routines */
/*********************/

API_FN(int ,	sgl_create_material, (sgl_bool generate_name, sgl_bool param_is_local))

API_FN(void,	sgl_use_material_instance, (int instanced_material, 
											sgl_bool param_is_local))

API_FN(void,	sgl_modify_material, (int name, sgl_bool clear_material))

API_FN(void,	sgl_set_ambient, (sgl_colour colour))

API_FN(void,	sgl_set_diffuse, (sgl_colour colour))

API_FN(void,	sgl_set_specular, (sgl_colour colour, int shininess))

API_FN(void,	sgl_set_glow, (sgl_colour colour))

API_FN(void,	sgl_set_opacity, (float opacity))

API_FN(void,	sgl_set_texture_map, (int texture_name, 
											sgl_bool flip_u, 
											sgl_bool flip_v))

API_FN(void,	sgl_set_texture_effect, (sgl_bool affect_ambient,
											sgl_bool affect_diffuse,
											sgl_bool affect_specular,
											sgl_bool affect_glow))

API_FN(int,		sgl_create_texture, (sgl_map_types 		map_type,
										sgl_map_sizes 		map_size, 
						sgl_mipmap_generation_options 	  	generate_mipmap,
										sgl_bool 			dither,
								  const sgl_intermediate_map *pixel_data,
								  const	sgl_intermediate_map *filtered_maps[]))

API_FN(int,		sgl_create_cached_texture, (long user_id))
						
API_FN(void,	sgl_load_cached_texture, (int 		name, 
										sgl_map_types 		map_type,
										sgl_map_sizes 		map_size, 
						sgl_mipmap_generation_options 	  	generate_mipmap,
										sgl_bool 			dither,
								const	sgl_intermediate_map *pixel_data,
								const 	sgl_intermediate_map *filtered_maps[],
										sgl_bool	overide_prepocessed_type))


API_FN(int,		sgl_preprocess_texture, (sgl_map_types 		map_type,
									sgl_map_sizes 		map_size, 
   					sgl_mipmap_generation_options 	  	generate_mipmap,
									sgl_bool 			dither,
							const	sgl_intermediate_map *src_pixel_data,
							const	sgl_intermediate_map *filtered_maps[],
									sgl_intermediate_map *processed_map))


API_FN(void ,	sgl_free_pixels, (sgl_intermediate_map *imap))

API_FN(void,	sgl_unload_cached_texture, (int 		name))

API_FN(void,	sgl_register_texture_callback, (sgl_tex_callback_func func,
									sgl_tex_callback_struct tex_data_array[],
														int	array_size))





/*
// This routine computes the size of the pixel data for ANY type of intermediate
// map. It does this by reading the values in the intermediate map header.
*/
API_FN(long,	sgl_texture_size, (sgl_intermediate_map *texture_map))


API_FN(void,   	sgl_set_texture, (int				texture_name,
   					sgl_mipmap_generation_options 	  	generate_mipmap,
									 sgl_bool 				dither,
							   const sgl_intermediate_map 	*pixel_data,
							   const sgl_intermediate_map 	*filtered_maps[]))
							   
API_FN(void,   	sgl_direct_set_texture, ( int TextureName,
										int MapLevel,
									  sgl_map_types NewMapType,
									  sgl_direct_srctype EnumSrcType,
   									  int SrcX,
									  int SrcY,
									  const void *pPixelData,
									  sgl_uint32 StrideInUINT32,
									  const sgl_direct_pixformat_struct *pPixFormat,
									  sgl_bool UseChromaKey,
									  sgl_uint32   ChromaColourOrIndex,
									  const void *Palette ))
									  
API_FN(int, 	sgl_set_texture_extended, (int			texture_name,
									 sgl_map_types			new_map_type,
									 sgl_map_sizes			new_map_size, 
						 sgl_mipmap_generation_options		generate_mipmap,
									 sgl_bool 				dither,
							   const sgl_intermediate_map 	*pixel_data,
							   const sgl_intermediate_map 	*filtered_maps[]))

API_FN(void,	sgl_delete_texture, (int texture_name))

API_FN(void,	sgl_delete_all_textures, ())

API_FN(unsigned long, sgl_get_free_texture_mem, ())


API_FN(void,	sgl_get_free_texture_mem_info, (sgl_texture_mem_info *info))


API_FN(void,	sgl_set_smap, (  sgl_smap_types 	smap_type,
									float			su,
									float			sv,
									float			ou,
									float			ov,
									float			r))

API_FN(void,	sgl_set_omap, (sgl_omap_types omap_type, 
									 float refractive_index))




/******************/
/* Light Routines */
/******************/

API_FN(int,		sgl_create_ambient_light, (  sgl_bool 	generate_name,
									   sgl_colour 	colour,
									   sgl_bool 	relative))

API_FN(int,		sgl_create_parallel_light, (	sgl_bool 	generate_name,
									sgl_colour 	colour,
									sgl_vector 	direction,
									sgl_bool	casts_shadows,
									sgl_bool	smooth_highlights))


API_FN(int,		sgl_create_point_light, (	sgl_bool 	generate_name,
									sgl_colour 	colour,
									sgl_vector 	direction,
									sgl_vector 	position,
									int 		concentration,
									sgl_bool	casts_shadows,
									sgl_bool	smooth_highlights))


API_FN(void,	sgl_position_light, (int name))


API_FN(void,	sgl_switch_light, ( int name, 
									  sgl_bool on, 
									  sgl_bool casts_shadows,
									  sgl_bool smooth_highlights))


API_FN(void,	sgl_set_ambient_light, ( int name, 
										   sgl_colour colour, 
										   sgl_bool relative))


API_FN(void,	sgl_set_parallel_light, ( int 	name,
									sgl_colour 	colour,
									sgl_vector 	direction,
									sgl_bool 	casts_shadows,
									sgl_bool 	smooth_highlights))

API_FN(void,	sgl_set_point_light, ( int 	name,
									sgl_colour 	colour,
									sgl_vector 	direction,
									sgl_vector 	position,
									int			concentration,
									sgl_bool 	casts_shadows,
									sgl_bool 	smooth_highlights))

API_FN(void,	sgl_pseudo_multishadows, ( sgl_bool enable,
										   sgl_colour colour))

/*******************/
/* Camera Routines */
/*******************/

API_FN(int,		sgl_create_camera, ( float zoom_factor,
								float foreground,
								float inv_background))


API_FN(int,		sgl_get_camera, (int			name,
									float 		*zoom_factor,
									float 		*foreground,
									float 		*inv_background))

API_FN(void,	sgl_set_camera, (int			name,
									float 		zoom_factor,
									float 		foreground,
									float 		inv_background))

API_FN(void,	sgl_render, (const int viewport_or_device, 
								const int camera_or_list, 
								const sgl_bool swap_buffers))


/*
// NOT YET IMPLEMENTED
//
API_FN(int,	sgl_render_time, (void))
*/

/****************************/
/* Level of Detail Routines */
/****************************/

API_FN(int,		sgl_create_detail_levels, ( sgl_bool   generate_name,
											   sgl_vector box_corner1,
											   sgl_vector box_corner2,
											   int	      models[3],
											   int		  change_sizes[2] ))

API_FN(int,		sgl_get_detail_levels, ( int        name,
										    sgl_vector box_corner1,
											sgl_vector box_corner2,
											int	       models[3],
											int		   change_sizes[2] ))

API_FN(void,	sgl_set_detail_levels, ( int        name,
										    sgl_vector box_corner1,
											sgl_vector box_corner2,
											int	       models[3],
											int		   change_sizes[2] ))

/********************************/
/* Collision Detection Routines */
/********************************/

API_FN(int,		sgl_create_point, ( sgl_vector offset,
								   sgl_bool   collision_check ))

API_FN(void,	sgl_set_point	, ( int		  point_name,
								   sgl_vector offset,
								   sgl_bool   collision_check ))

API_FN(void,	sgl_switch_point, ( int      name,
								   sgl_bool enable_check ))

API_FN(void,	sgl_position_point, (int name))

API_FN(int,		sgl_query_point, ( int				 name,
								  sgl_vector		 point_pos,
								  sgl_collision_data *collision_data ))


/********************************
* Fog & Background Routines 
********************************/
API_FN(void,	sgl_set_fog, ( int  		camera_name,	
							 sgl_colour	fog_colour,
							 float		fog_density))


API_FN(void,	sgl_set_background_colour, ( int  		camera_name,	
										   sgl_colour	colour))

/********************************
* Primitive Object Routines 
********************************/
API_FN(int,		sgl_create_rectprism, ( 	float x, 
									float y, 
									float z, 
									sgl_bool bName,
									sgl_bool bTexture))

API_FN(int,		sgl_create_prism, (	float height, 
								float radius, 
								int num_sides, 
								sgl_bool bName,
								sgl_bool bSmoothShade,
								sgl_bool bTexture))

API_FN(int,		sgl_create_sphere, (	int sph_type, 
								float radius, 
								int num_sides, 
								sgl_bool bName, 
								sgl_bool bSmoothShade, 
								sgl_bool bTexture))

API_FN(int ,	sgl_create_extruded_poly, (	int numpoints, 
						      		float point_list[][2], 
					       			float depth, 
				       				sgl_bool bName,
								sgl_bool bSmoothShade, 
								sgl_bool bTexture))

API_FN(int ,	sgl_revolve_polygon, (	int nNoPoints, 
								float pt[][2], 
								int nNoSects,
								sgl_bool bClosed,
  								sgl_bool nYAxis,
  								int bSmoothShade,
  								sgl_bool bTexture))

/********************************
* Translucency Routine 
********************************/

API_FN(void,	sgl_new_translucent, (void))

/********************************
* Quality Settings 
********************************/
API_FN(void,	sgl_qual_smooth_shading, (const sgl_bool enable))

API_FN(void,	sgl_qual_texturing, (const sgl_bool enable))

API_FN(void,	sgl_qual_collision_detection, (const sgl_bool enable, 
										 const sgl_bool enable_offscreen))

API_FN(void,	sgl_qual_generate_shadows, (const sgl_bool enable))

API_FN(void,	sgl_qual_fog, (const sgl_bool enable))

API_FN(void,	sgl_qual_texture_filter, (const sgl_texture_filter_type type))

API_FN(void,	sgl_qual_dither, (const sgl_bool enable))

/********************************
* random number generator. 
********************************/

API_FN(long, 	sgl_rand, (void))

API_FN(void,	sgl_srand, (unsigned long Seed))


/********************************
* Version Information
********************************/

API_FN(sgl_versions *, sgl_get_versions, (void))


/********************************
* This is only for the simulator
********************************/

API_FN(void, TexasWriteBMP, (char * filename))




/********************************
* BMP to Texture laoding code.
*
* Given a filename, load the texture and create an SGL intermediate texture map.
********************************/
API_FN(sgl_intermediate_map, ConvertBMPtoSGL, (char * 	filename,
									 sgl_bool  	Translucent))


/********************************
*  Load a bmp file, work out the biggest texture it will fit into and return
*  a texture handle. NB textures only loaded once! 
********************************/

API_FN(int,	LoadBMPTexture, (char *pszFilename, sgl_bool bTranslucent, 
					   sgl_mipmap_generation_options  generate_mipmap, sgl_bool dither))

/* free a texture loaded with the above function */
API_FN(void,	FreeBMPTexture, (int nTextureName))

/* free all textures loaded with the above function */
API_FN(void,	FreeAllBMPTextures, (void))



/*
// Load SGLB file
*/
API_FN(int, LoadSglBin, (char *pszFileName,	
					 int *noMeshes,		int **meshIDs, 	
					 int *noMaterials,	int **materialIDs,
					 int *noBMPTextures, int **BMPTextureIDs))

API_FN(void, FreeSglBinTextures, (int noBMPTextures, int *BMPTextureIDs))



/*
// -------------------
// sgltri_startofframe
// -------------------
// see sgltri_triangles
*/

API_FN(void,  sgltri_startofframe, (PSGLCONTEXT pContext))


/*
// ----------------
// sgltri_triangles
// ----------------
// Sends a set of triangles, which may have been output by the transformation
// and lighting stages of Direct3D or RAVE, to the PowerVR hardware.
//
// sgltri_startofframe should be called before the first call of this function
// for a frame.
//
// This function can then be called multiple times to add subsequent sets of
// triangles before the sgltri_render function is called to render the complete
// scene.
//
// pContext: Pointer to a context structure for this set of triangles.
// nNumFaces: Number of faces.
// pFaces: Pointer to the array of faces.
// pVertices: Pointer to the first vertex in the array of vertices, some or all
//   of which are used by the faces pFaces[0..nNumFaces].
//
// Error values set by this function (read using sgl_get_errors):
//   sgl_no_error: success
//   sgl_bad_parameter: one or more of the pointer parameters was null, or
//     nNumFaces was negative.
//   sgl_err_failed_init: initialisation failed (probably out of memory)
//	 sgl_err_no_mem: out of memory
//
// It is assumed that the face arrays refer to valid vertices.
*/

API_FN(void,  sgltri_triangles, ( PSGLCONTEXT  pContext,
										  int  nNumFaces,
										  int  pFaces[][3],
										  PSGLVERTEX  pVertices ))

API_FN(void,  sgltri_quads, ( PSGLCONTEXT  pContext,
									  int  nNumFaces,
									  int  pFaces[][4],
							   PSGLVERTEX  pVertices ))

API_FN(void,  sgltri_points, ( PSGLCONTEXT  pContext,
									   int  nNumPoints,
							    PSGLVERTEX  pVertices ))

API_FN(void,  sgltri_lines, ( PSGLCONTEXT  pContext,
							          int  nLines,
							   sgl_uint16  pLines[][2],
							   PSGLVERTEX  pVertices ))

/*
// NEW FUNCTION - will draw rectangular polys in const Z-depth
// defined with two vertices which are the opposite corners
// First vertex defines all stuff i.e. colour, translucency,
// depth, etc.
// second vertex used for x,y and U,V only
// Try using for text etc - triangle setup is quicker than 
// using quads
// To avoid problems with previous versions not having this
// function please link dynamically and do a GetProcAddress to 
// check its there :)
// P.S. if it ain't there call sgltri_quads :( 
//
*/
API_FN(void,  sgltri_sprites, ( PSGLCONTEXT  pContext,
							          int  nSprites,
							   sgl_uint16  pSprites[][2],
							   PSGLVERTEX  pVertices ))

API_FN(void,  sgltri_shadow, ( PSGLCONTEXT  pContext,
								       int  nNumFaces,
								       int  pFaces[][3],
								PSGLVERTEX  pVertices,
								     float  fBoundingBox[2][2]))

/*
// -------------
// sgltri_render
// -------------
// see sgltri_triangles
//
// Render the entire scene of triangles.
*/

API_FN(void,  sgltri_render, (PSGLCONTEXT  pContext))

/* variant for VideoLogic use only */

API_FN(sgl_bool, sgltri_renderstrip, (PSGLCONTEXT pContext, int nStrip, int nXExtent[2]))

/*
// sgltri_rerender
// This function is no longer implemented.
*/
API_FN(void,  sgltri_rerender, (PSGLCONTEXT  pContext))

/*
// -----------------------
// sgltri_isrendercomplete
// -----------------------
//
// If the timeout value is non-zero, the function will loop 
// round until the hardware either finishes or times out,
// otherwise the state is sampled and returned.
//
// Note: Use of this function is not currently reliable.
*/

API_FN(IRC_RESULT, sgltri_isrendercomplete, (PSGLCONTEXT  pContext, 
											 sgl_uint32 u32Timeout))

/* These two functions are now the recommend way to get settings
** which used to be in the sglhw.ini file e.g StrictLocks
** They will look in the registry first, but will be overridden
** if an INI file exists with the requested section/entry pair
** TRUE returned on success - SglError set in fails
**/

API_FN(sgl_bool, sgl_get_ini_string, (char *ReturnData, int ReturnSize, 
									  char *DefaultDataValue, 
									  char *Section, char *Entry))

API_FN(sgl_bool, sgl_get_ini_int, (sgl_int32 *ReturnData,
								   sgl_int32 DefaultDataValue, 
								   char *Section, char *Entry))

#ifdef _BUILDING_SGL_

/* PRIVATE FUNCTION entry point to allow sgl to understand
** its process context and coexist with other sgl/d3d processes
*/
API_FN(void, SglSetGlobal, (SGL_Globals eGlobal, void* pValue))

#endif

API_END(SGL)

#ifdef __cplusplus
}
#endif 

/*------------------------------ End of File --------------------------------*/
