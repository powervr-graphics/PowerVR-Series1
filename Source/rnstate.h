/**************************************************************************
 * Name         : rnstate.h
 * Title        : Definition of the state variable types, and sizes 
 *				  of structures etc.
 * Author       : Simon Fenney
 * Created      : 09 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Defines the SGL state variables as suggested by TRDD027x.doc
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnstate.h,v $
 * Revision 1.25  1997/09/11  10:29:19  erf
 * Added qf_dithering to enum of available quality states.
 *
 * Revision 1.24  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.23  1997/03/25  10:29:13  erf
 * Added texture filtering quality support.
 *
 * Revision 1.22  1996/05/08  14:57:25  sjf
 * 1)Moved in the #defines for the stack sizes
 * 2)made the overall stack size the Max of the individual state stacks
 * 3)Changed INT16s to int in the Instnce subsitution struct.
 *
 * Revision 1.21  1996/03/22  16:52:07  sjf
 * Added #include guards.
 *
 * Revision 1.20  1995/11/02  19:50:50  sjf
 * Added fog quality.
 *
 * Revision 1.19  1995/10/06  20:09:29  sjf
 * Added support to order the lights into 3 categories for shading efficiency.
 *
 * Revision 1.18  1995/09/28  10:28:54  pdl
 * removed the smap axis stuff.
 *
 * Revision 1.17  1995/09/25  18:12:51  sjf
 * moved multi shadow out of quality.
 *
 * Revision 1.16  1995/09/15  18:19:02  jrc
 * Changed 'pos' to 'pointWC'.
 *
 * Revision 1.15  1995/08/28  16:56:29  sjf
 * Added log2Concentration.
 *
 * Revision 1.14  1995/08/09  17:43:57  pdl
 * added some opacity variables.
 *
 * Revision 1.13  1995/08/07  15:34:49  sm
 * added local direction in light state
 *
 * Revision 1.12  1995/07/24  15:28:37  pdl
 * changed the texture_flags to an UINT32.
 *
 * Revision 1.11  1995/07/20  18:01:50  sjf
 * Added extra flag values (dirty position) and sort of added
 * fields for when lights are stored in coordinates local to the
 * primitives.
 *
 * Revision 1.10  1995/07/06  12:01:03  sjf
 * Changed the identifiers of the light state flag enumerated
 * type to include a prefix so there was far less chance of them
 * clashing with other identifiers.
 *
 * Revision 1.9  1995/07/06  11:31:21  sjf
 * Changed quality state structure to use bit flags.
 *
 * Revision 1.8  1995/06/27  13:12:32  sm
 * added shadow flag
 *
 * Revision 1.7  1995/06/22  17:05:00  sm
 * added dirty flag for lights structure
 *
 * Revision 1.6  1995/06/15  09:43:43  sjf
 * Added scalar intensity values to lights so that smooth
 * shading can be done.
 *
 * Revision 1.5  1995/06/08  17:15:15  sjf
 * Added the Master State struct definition, and fixed a naming problem
 * with one of the variables.
 *
 * Revision 1.4  1995/05/25  14:22:00  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.3  1995/05/16  09:38:57  sjf
 * Changed name of initialisation routine to conform closer to standard.
 *
 * Revision 1.2  1995/05/11  14:11:21  sjf
 * Added the extern defs for the state variable stacks.
 *
 * Revision 1.1  1995/05/09  16:17:38  sjf
 * Initial revision
 *
 *
 **************************************************************************/

#ifndef __RNSTATE_H__
#define __RNSTATE_H__

/* ///////////////////////////////////////
// Declare the #defines for the maximum depth of
// the stacks for the various state variables
//////////////////////////////////////////
*/
#define MAX_MATERIAL_STATE_STACK 		30
#define MAX_TRANSFORM_STATE_STACK		40
#define MAX_LIGHTS_STATE_STACK			15
#define MAX_QUALITY_STATE_STACK			15
#define MAX_COLLISION_STATE_STACK		15
#define	MAX_INSTANCE_SUBS_STATE_STACK	10

/*
// This should be the MAX of the above.
*/
#define MAX_DEPTH_OF_TRAVERSAL 40





/* ///////////////////////////////////////
// Define the material state structure
//////////////////////////////////////////
*/
typedef struct
{
	sgl_colour	diffuse;
	sgl_colour	ambient;

	sgl_colour	specular;
	int			specular_shininess;
	float		specular_shininess_float;

	sgl_colour	glow;

	float opacity;
	int	  translucent_int;

	/*
	// The following is a combination of texture effect, etc
	// as specified in TRDD027x.doc
	*/
	sgl_uint32 	   texture_flags;

	/*
	// Precomputed values for Texas parameters.
	// The structure is defined as part of the Texas header files
	// and is meant to be "hidden" from outsiders.
	// This is "located" in texas.h
	*/
	TEXAS_PRECOMP_STRUCT	texas_precomp;

	/*
	// Wrap Data.. NOTE THIS IS STILL EFFECTIVELY TBA
	*/
	float su, sv, ou, ov;
	float radius;

	float	   refrac_index;



}MATERIAL_STATE_STRUCT; 



/*////////////////////////////////////////////////////////
// No need to Define the transformation state structure
// as this is identical to TRANSFORM_STRUCT.
//////////////////////////////////////////////////////////
*/



		     
/*////////////////////////////////////////////////////////
// Define the Lights state structure(s)
//
// First define the light slot struct.
//////////////////////////////////////////////////////////
*/
typedef struct
{
	int	multi_light;
	sgl_colour colour;

}LIGHT_SLOT_STRUCT;


/*
// Define the light entry structure
*/

typedef struct
{
	/*
	// The flags for the light. NOTE these are built from
	// combinations of "LIGHT_FLAGS_ENUM" as defined in 
	// "dllights.h" and TRDD027x.doc
	*/
	int light_flags;
	
	/* The name of the light */
	int light_name;

	/* 
	// The shadow slot being affected currently, and the one
	// the light is assigned to when it is casting shadows.
	// The light colour slot indicates where shading results should
	// be put.
	*/
	int	shad_volume;
	int	assigned_shad_volume;
	int light_colour_slot;

	sgl_vector	position;
	sgl_colour	colour;
	float		smooth_intensity;
	sgl_vector	direction;
	int			concentration;
	float		log2concentration;

	/*
	// Light position ETC in the current local
	// coordinate system.  TO BE DEFINED
	// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	*/
	sgl_vector local_position;
	sgl_vector local_direction;
 
} LIGHT_ENTRY_STRUCT;




/* //////////////
// Define the entire light state structure. Start with the possible
// flag values - NOTE these are ORed together.
/////////////////
*/
typedef enum
{
	lsf_ambient_grey 	= (1 << 0),
	lsf_has_point_light	= (1 << 1),
	lsf_earl_grey		= (1 << 2),
	lsf_dirty_smooth	= (1 << 3),
	lsf_dirty_position	= (1 << 4),
	lsf_shadows			= (1 << 5),
	lsf_multi_shadows	= (1 << 6)

}LIGHTS_STATE_FLAG_ENUM;


typedef struct
{
	/*
	// number of actual active lights in the structure
	*/
	int num_lights;

	/*
	// To reduce overheads when actually shading, the lights will be sorted
	// into 3 groups - the parallel lights, the point source lights, and
	// the "off" lights.
	// They will be placed in the light entries array in that order with
	// no gaps between them. This means that it takes a little more time
	// to add and/or swith lights on and off, but should mean the shading
	// is faster. That is more important.
	//
	// The three following ints MUST equal num_lights.
	*/
	int numOnParLights, numOnPntLights, numOffLights;
	
	/*
	// overall set of flags for the lights. This is a
	// combination of values from "LIGHTS_STATE_FLAG_ENUM"
	*/
	int flags;

	/*
	// Ambient light colour or grey level
	*/
	sgl_colour	ambient_colour;

	/*
	// Ambient smooth shading intensity - this is used
	// to represent the FRACTION of the average colour
	// for the ambient when smooth shading.
	*/
	float ambient_smooth_intensity;

	/*
	// declare the light slots
	*/
	LIGHT_SLOT_STRUCT light_slots[NUM_LIGHT_SLOTS];

	/*
	// declare the light entries
	*/
	LIGHT_ENTRY_STRUCT light_entries[SGL_MAX_ACTIVE_LIGHTS];

} LIGHTS_STATE_STRUCT;





/*////////////////////////////////////////////////////////
// Quality State Structure
//
// This is still pretty much TBA.
//////////////////////////////////////////////////////////
*/

/*
// "Enumerated type" for the quality state flags. These are
// bit field values.
*/
typedef enum
{
	qf_cast_shadows 		= (1 << 0),
	qf_smooth_shading		= (1 << 1),
	qf_textures				= (1 << 2),
	qf_full_collision		= (1 << 3),
	qf_onscreen_collision	= (1 << 4),
	qf_fog					= (1 << 5),
	qf_texture_filtering	= (1 << 6),
	qf_dithering			= (1 << 7)

} QUALITY_FLAGS_ENUM;


typedef struct
{
	/*
	// The flags field is built from ORing the 
	// QUALITY FLAGS ENUM values
	*/
	int flags;

	/*
	// For bilinear filtering, need a variable.
	*/
	sgl_texture_filter_type eFilterType;

} QUALITY_STATE_STRUCT;





/*////////////////////////////////////////////////////////
// Collision Points State Structure
//
// This is still pretty much TBA.
//
// 1st Define the data needed for each collision point.
//////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// The position of the point in world (camera-relative) coordinates:
	*/
	sgl_vector pointWC;

	/*
	// a pointer back to the point node structure so that we can store the
	// point's absolute position and mark it as "Hit"
	*/
	POINT_NODE_STRUCT * p_its_node;

} COLLISION_PNT_ENTRY_STRUCT;


/*
// Define the state structure to contain an array of these
// collision point entries
*/
typedef struct
{
	/*
	// Keep track of how many active points we have
	*/
	int num_pnts;

	/*
	// the array of active collision points
	*/
	COLLISION_PNT_ENTRY_STRUCT pnts[SGL_MAX_ACTIVE_POINTS];

} COLLISION_STATE_STRUCT;




/*////////////////////////////////////////////////////////
// Instance Substitutions State Structure
//
// This is simply an array of up to a maximum number
// of subsititution pairs, (i.e. a name and a replacement)
// along with the number of such replacements..
//////////////////////////////////////////////////////////
*/
typedef struct
{
	int num_subs;

	int	nSubs[SGL_MAX_INSTANCE_SUBS][2];

}INSTANCE_SUBS_STATE_STRUCT;




/*////////////////////////////////////////////////////////
// Define the "Master" state stack variables
//
//	This is used during display list traversal.
//
// It consists of pointers into the other state stacks, PLUS
// flags to indicate if certain of the states need to be preserved.
//
//	See TRDD027x.doc for details...
//
// First define the various save flags...
//////////////////////////////////////////////////////////
*/
typedef enum
{
	state_save_material  	= 0x01,
	state_save_transform	= 0x02,
	state_save_lights		= 0x04,
	state_save_quality		= 0x08,
	state_save_collision	= 0x10,
	state_save_instance_subs= 0x20

}STATE_SAVE_FLAGS_ENUM;

/*
// Define a value which sets ALL of the flags
*/
#define ALL_STATE_SAVE_FLAGS (0x3F)

/*
// Define the master stack frame type
*/
typedef struct
{
	/*
	// Save flags:Effectively this records which ones have
	//  not been "pushed" and so must be preserved before
	//  modifying
	*/
	int	saveFlags;

	/*
	// Pointer to the list node which currently defines the scope
	*/
	LIST_NODE_STRUCT * pScopingList;

	/*
	// list pointers to the various state struct variables
	*/
	MATERIAL_STATE_STRUCT        *pMaterialState;
	TRANSFORM_STRUCT             *pTransformState;
	LIGHTS_STATE_STRUCT          *pLightsState;
	QUALITY_STATE_STRUCT         *pQualityState;
	COLLISION_STATE_STRUCT       *pCollisionState;
	INSTANCE_SUBS_STATE_STRUCT   *pInstanceSubState;

} MASTER_STATE_STRUCT;


/* ///////////////////////////////////////////////////
// Declare external pointers to the base and LAST elements of the stacks,
// and a routine to initialise them. The last element is used to prevent us
// overflowing the stack.
//
// CURRENTLY,during a render display list traversal, it is assumed that
// these pointers remain constant.
// 
/////////////////////////////////////////////////// */

extern int InitStateStacks(void);


extern MATERIAL_STATE_STRUCT		*pMaterialStackBase;
extern MATERIAL_STATE_STRUCT		*pMaterialStackLast;

extern TRANSFORM_STRUCT				*pTransformStackBase;
extern TRANSFORM_STRUCT				*pTransformStackLast;

extern LIGHTS_STATE_STRUCT			*pLightsStackBase;
extern LIGHTS_STATE_STRUCT			*pLightsStackLast;

extern QUALITY_STATE_STRUCT			*pQualityStackBase;
extern QUALITY_STATE_STRUCT			*pQualityStackLast;

extern COLLISION_STATE_STRUCT		*pCollisionStackBase;
extern COLLISION_STATE_STRUCT		*pCollisionStackLast;

extern INSTANCE_SUBS_STATE_STRUCT	*pInstanceSubStackBase;
extern INSTANCE_SUBS_STATE_STRUCT	*pInstanceSubStackLast;



#endif /* __RNSTATE_H__  */

/*--------------------------- End of File --------------------------------*/

