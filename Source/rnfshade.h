/**************************************************************************
 * Name         : rnfshade.h
 * Title        : Flat Shading 
 * Author       : Stel Michael
 * Created      : 20th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Routines to handle the flat shading of planes
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnfshade.h,v $
 * Revision 1.19  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.18  1996/09/30  14:17:14  jop
 * new colour format for triangles
 *
 * Revision 1.17  1996/07/19  19:33:20  jop
 * Added alpha for upcoming NTT support
 *
 * Revision 1.16  1996/05/03  16:27:17  sjf
 * Changed INT16 param to int, to make conversion from float to int quicker.
 *
 * Revision 1.15  1996/03/22  16:51:05  sjf
 * Added include guards.
 *
 * Revision 1.14  1995/10/06  14:19:06  jop
 * Added ShadingRepPt
 *
 * Revision 1.13  1995/09/15  19:49:53  sjf
 * removed dud param.
 *
 * Revision 1.12  1995/08/28  16:55:26  sjf
 * Added shadow support to textired flat shading.
 *
 * Revision 1.11  1995/08/27  15:26:50  sjf
 * Made changes to support shadows.
 *
 * Revision 1.10  1995/08/02  15:09:39  sm
 * split into two routines
 *
 * Revision 1.9  1995/07/25  16:54:17  sm
 * added bool for reverse planes
 *
 * Revision 1.8  1995/07/25  10:59:48  sm
 * changed 't' shading params to INT16
 *
 * Revision 1.7  1995/07/24  17:08:42  jop
 * Changed BOOL to sgl_bool
 *
 * Revision 1.6  1995/07/24  15:54:57  sm
 * added a ,
 *
 * Revision 1.5  1995/07/24  15:47:18  sm
 * took an s out
 *
 * Revision 1.4  1995/07/24  12:40:34  sm
 * added function prototype for DoFlatShading
 *
 * Revision 1.3  1995/07/23  12:42:21  pdl
 * changed rglColour to rgbColour.
 *
 * Revision 1.2  1995/07/23  12:13:57  sm
 * added typedef
 *
 * Revision 1.1  1995/07/21  15:52:40  sm
 * Initial revision
 *
 *
 *
 **************************************************************************/

#ifndef __RNFSHADE_H__
#define __RNFSHADE_H__

#include "rnstate.h"

typedef union
{
	struct 
	{
		sgl_vector 	rgbColour;
		int			t0,t1,t2;
		sgl_2d_vec 	ShadingRepPt;
	} smooth;

	struct
	{
		/*
		// NOTE: The current texture flat shading ASSUMES that the entries
		// are multiples of floats, so that we can do pointer maths to get
		// to either the base colour or highlight colour.
		*/
		sgl_vector baseColour;
		sgl_vector highlightColour;
		sgl_uint32	   nAlpha;
	} flat;

	struct
	{
		sgl_uint32 uBaseColour;		/* RGB 888 */
		sgl_uint32 uHighColour;		/* RGB 565 */
		sgl_uint32 nAlpha;
	} flat_int;

} SHADING_SLOT_UNION;

typedef struct 
{
	/* 
	// Array of flat or smooth shading vals. Each entry
	// corresponds to a light slot 
	*/
	SHADING_SLOT_UNION	 slot[NUM_LIGHT_SLOTS];

} SHADING_RESULT_STRUCT;



extern	void  DoFlatShading( sgl_bool ReversePlanes, 
							 int numPlanes, 
								   TRANSFORMED_PLANE_STRUCT * transPlanes[],
							 const TRANSFORM_STRUCT		 * pTransform,
							 const MATERIAL_STATE_STRUCT * stateMaterial, 
							 const LIGHTS_STATE_STRUCT	 * lightState,
								   SHADING_RESULT_STRUCT * ShadingResults);


extern	void  DoFlatTextureShading( sgl_bool ReversePlanes, 
							 int numPlanes, 
								   TRANSFORMED_PLANE_STRUCT * transPlanes[],
							 const TRANSFORM_STRUCT		 * pTransform,
							 const MATERIAL_STATE_STRUCT * stateMaterial, 
							 const LIGHTS_STATE_STRUCT	 * lightState,
								   SHADING_RESULT_STRUCT * ShadingResults);

#endif /*__RNFSHADE_H__ */

/*------------------------------ End of File --------------------------------*/


