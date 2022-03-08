/******************************************************************************
 * Name         : pktsp.h
 * Title        : Performs the TEXAS parameter packing.
 * Author       : Peter Leaback
 * Created      : 21/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: pktsp.h,v $
 * Revision 1.29  1997/07/14  19:30:46  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.28  1997/06/18  14:40:50  mjg
 * Fixed PCX2 slow down with a mask-plane fix - made it PCX2 only.
 *
 * Revision 1.27  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.26  1997/04/21  15:27:40  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.25  1997/02/11  14:56:23  erf
 * Add prototype definition of PackTexasMask() routine.
 *
 * Revision 1.24  1996/12/13  17:26:14  gdc
 * added prototypes for individual non-textured shading functions
 *
 * Revision 1.23  1996/12/09  13:53:40  gdc
 * removed references to Triangle API - assmuse sgltri stuff is now all
 * done through D3DOPTIM stuff
 *
 * Revision 1.22  1996/11/28  17:14:03  jrc
 * Added hardware specular highlights for faces enabled for shadows or
 * light volumes.
 *
 * Revision 1.21  1996/11/18  18:34:13  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.20  1996/10/23  16:13:07  jop
 *  Added debug stuff (ifdefed out) for sm/shad/trans bug
 *
 * Revision 1.19  1996/10/18  13:05:01  jop
 * Added late background colour support
 *
 * Revision 1.18  1996/09/27  14:27:42  jop
 * Added IsHighlight param to tri code
 *
 * Revision 1.17  1996/09/07  22:59:06  jop
 * Added light volumes to triangle api
 *
 * Revision 1.16  1996/09/04  00:00:39  jop
 * Added shadow support for alpha blend triangles
 *
 * Revision 1.15  1996/09/02  16:59:35  mjg
 * Updated parameters for alpha channel translucency
 *
 * Revision 1.14  1996/07/19  19:32:59  jop
 * Added white packer and NTT support
 *
 * Revision 1.13  1996/07/15  17:08:57  jop
 * Added error detection in triangle packing
 *
 * Revision 1.12  1996/06/06  23:06:34  jop
 * Added shadow param to packtexasflat
 *
 * Revision 1.11  1996/06/06  14:22:37  jop
 * Added shoadowed version of triangle render code
 *
 * Revision 1.10  1996/05/09  17:32:26  jop
 * Added triangle packing support
 *
 * Revision 1.9  1996/05/03  16:20:31  sjf
 * Got rid of uneeded parameter
 *
 * Revision 1.8  1996/04/22  16:10:13  sjf
 *  Added non textured pack routine
 *
 * Revision 1.7  1996/04/20  16:45:53  sjf
 *  Added a smooth textured packing routine.
 *
 * Revision 1.6  1996/04/19  19:53:47  sjf
 * Added a prototype for the flat textured pack routine.
 *
 * Revision 1.5  1995/10/04  08:33:04  sjf
 * Added extern keywords, and added a routine to get how much of
 * the texas parameter store is in use.
 *
 * Revision 1.4  1995/08/21  09:49:07  sjf
 * Added fogging param to PackTexasFlat.
 *
 * Revision 1.3  1995/07/27  11:17:22  pdl
 * added PackTexasFlat.
 *
 * Revision 1.2  1995/07/26  15:02:46  jop
 * added single parameter
 * doodah
 *
 * Revision 1.1  1995/07/23  15:26:15  pdl
 * Initial revision
 *
 *
 *
 *****************************************************************************/

typedef struct tagTEXPARAMBLK
{
	TEXTURING_RESULT_STRUCT 	TextR;
	SHADING_RESULT_STRUCT		ShadeR;
	sgl_bool					IsTextured;
	sgl_bool					IsSmoothShaded;
	sgl_bool					IsShadowed;
	sgl_bool					IsHighlighted;
	sgl_bool					FogOn;
	MATERIAL_STATE_STRUCT		*MState;
	sgl_uint32						u32TexasTag;
	sgl_2d_vec					projRepPoint;
} TEXPARAMBLK, *PTEXPARAMBLK;

void ClearParameterStore();

extern void PackTexasParamsNonTextured(int		NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *ShadeR,
					 sgl_bool					IsSmoothShaded,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[]);

extern void PackTexasParamsFlatNonTexturedTrans(int		NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *ShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 sgl_uint32                 uAlpha,
					 TRANSFORMED_PLANE_STRUCT  *Planes[]);

extern void PackTexasParamsFlatNonTextured(int		NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *ShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[]);

extern void PackTexasParamsSmoothNonTextured(int		NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *ShadeR,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[]);

extern void PackTexasParamsSmoothNonTexturedShad(int		NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *ShadeR,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[]);


extern void PackTexasParamsFlatTextured(int	   	NumberOfPlanes,
					 TEXTURING_RESULT_STRUCT   *pTextR,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[],
				const MATERIAL_STATE_STRUCT	   *MState);

extern void PackTexasParamsSmoothTextured(int	NumberOfPlanes,
					 TEXTURING_RESULT_STRUCT   *pTextR,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[],
				const MATERIAL_STATE_STRUCT	   *MState);


extern void PackTexasParamIndirect (PTEXPARAMBLK pTP);

extern sgl_uint32 PackTexasTransparent(sgl_bool ShadowsOn);
extern sgl_uint32 PackTexasFlat(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn);
extern void PackTexasFlatAtAddr(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn, sgl_uint32 Addr);

#if PCX2 || PCX2_003
extern sgl_uint32 PackTexasMask(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn);
#endif

/*
// End of file
*/

