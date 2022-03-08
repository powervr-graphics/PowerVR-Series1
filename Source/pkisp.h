/**************************************************************************
 * Name         : pkisp.h
 * Title        : Plane data packing for ISP chip
 * Author       : Stel Michael
 * Created      : 19th July 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: pkisp.h,v $
 * Revision 1.33  1997/07/14  19:30:28  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.32  1997/05/09  11:24:36  jrc
 * Changed some static functions to extern.
 *
 * Revision 1.31  1997/05/09  09:21:37  erf
 * Added prototype for AddDummyPlanesL(). Removed some prototype definitions.
 *
 * Revision 1.30  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.29  1997/02/19  14:39:05  erf
 * Prototype for PackOpaqueDummyLarge().
 *
 * Revision 1.28  1997/02/12  12:33:35  erf
 * New prototype definition for PackTranslucentBeginTranslucent().
 *
 * Revision 1.27  1996/12/10  23:34:13  erf
 * Changed PVR3 to PCX2.
 *
 * Revision 1.26  1996/12/09  14:04:59  gdc
 * removed SGLTri stuff - all sgl tri is now done thru' D3DOPTIM modules
 *
 * Revision 1.25  1996/11/18  18:37:12  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.24  1996/11/07  17:02:37  erf
 * Fixed #ifdef PVR3 to #if PVR3.
 *
 * Revision 1.23  1996/11/07  11:19:10  erf
 * Added PVR3 compile switch to:
 * 1.Change BackgroundDistance parameter of PackBackgroundPlane()
 *   INT32 to float.
 *
 * Revision 1.22  1996/10/16  21:13:27  jop
 * Added a new pack routine
 *
 * Revision 1.21  1996/07/15  17:07:42  jop
 * Added quad packing routine
 *
 * Revision 1.20  1996/05/10  14:44:59  jop
 * Added some structures
 *
 * Revision 1.19  1996/05/03  14:42:48  jop
 * Added triangle api pack call
 *
 * Revision 1.18  1996/04/02  12:33:18  sjf
 * Added 20bit Zero value.
 *
 * Revision 1.17  1995/11/05  15:23:28  sjf
 * Changed structure of plane order to use Ints not int16s.
 *
 * Revision 1.16  1995/10/04  10:37:56  jop
 * Added PackMeshParamsOrdered
 *
 * Revision 1.15  1995/09/19  09:02:57  sjf
 * Deleted PackLightVols and PackShadows... and replaced with a unified
 * routine.
 *
 * Revision 1.14  1995/09/11  15:56:23  sm
 * removed PackTranslucentParams
 *
 * Revision 1.13  1995/09/08  15:55:37  sm
 * added dummy object pack routines
 *
 * Revision 1.12  1995/09/06  09:34:36  sjf
 * Optimised pack opaque ... the same could/should be done for the
 * other pack routines.
 *
 * Revision 1.11  1995/09/04  15:29:43  sm
 * added pack dummy trans
 *
 * Revision 1.10  1995/08/27  15:23:13  sjf
 * Deleted unneeded param from pack shadows.
 *
 * Revision 1.9  1995/08/14  11:37:19  sjf
 * Made packing routine external, and fixed a parameter to mesh packing which should
 * not have been a const.
 *
 * Revision 1.8  1995/08/08  12:17:49  jop
 * Changed prototype to PackMeshParams
 *
 * Revision 1.7  1995/07/27  18:34:29  jop
 * Added parameter to packmeshparams
 *
 * Revision 1.6  1995/07/27  11:53:42  sjf
 *  Added a Pack background plane.
 *
 * Revision 1.5  1995/07/26  12:17:03  sm
 * changed to return num planes added
 *
 * Revision 1.4  1995/07/25  17:01:12  sjf
 * Changed "see inside" to "repeat reverse".
 *
 * Revision 1.3  1995/07/25  12:53:25  sjf
 * 1) Added polygon packing routine.
 * 2) changed parameters to const.
 *
 * Revision 1.2  1995/07/24  15:14:19  sm
 * split up into four pack routines
 *
 * Revision 1.1  1995/07/24  10:28:21  sm
 * Initial revision
 *
 *
 *************************************************************************/


#define SFLOAT_20BIT_ZERO (0)


extern sgl_int32 PackTo20Bit(const float Value);


extern int PackOpaqueParams( const PLANE_CATEGORIES_STRUCT * pPlaneLists,
							 const sgl_bool AddClipPlane, 
							 const sgl_bool RepeatReverse);

extern int	PackLightShadVolParams( const PLANE_CATEGORIES_STRUCT * pPlaneLists,
							const sgl_bool IsShadowVol);

#ifndef __RNMESH_H__
#include "rnmesh.h"
#endif

extern int PackMeshParams (	const TRANSFORMED_PLANE_STRUCT Planes[],
								  XMESHEXTRA Extras[],
						 	const int nNumPlanes);

extern int PackMeshParamsOrdered (	const TRANSFORMED_PLANE_STRUCT Planes[],
								  XMESHEXTRA Extras[],
								  sgl_int32 *Order,
						 	const int nNumPlanes);

#if PCX2 || PCX2_003
extern void PackBackgroundPlane ( const sgl_uint32 u32TexasTag,
							 const float  BackgroundDistance);
#else
extern void PackBackgroundPlane ( const sgl_uint32 u32TexasTag,
							 const sgl_int32  BackgroundDistance);
#endif

void PackOpaqueDummy();
void PackOpaqueDummyLarge();
void PackCompleteShadow();
void PackTranslucentBeginDummy(sgl_uint32 u32TSPTag);
void PackTranslucentDummy(sgl_uint32 u32TSPTag);

void AddDummyPlanesL(sgl_bool fShadowsOn);


/*------------------------------ End of File --------------------------------*/
