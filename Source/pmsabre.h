/**************************************************************************
 * Name         : pmsabre.h
 * Title        : parameter management for sabre
 * Author       : Stel Michael
 * Created      : 5th July 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: pmsabre.h,v $
 * Revision 1.47  1997/08/08  09:09:47  gdc
 * upped maximum number of Y-regions allowed
 *
 * Revision 1.46  1997/08/02  16:41:32  mjg
 * Removed the 003 references as it doesn't need to be special cased.
 *
 * Revision 1.45  1997/07/14  19:53:07  mjg
 * Added PCX2_003 compile switch and a temporary define for testing.
 *
 * Revision 1.44  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.43  1997/01/24  09:33:46  erf
 * Double MAGIC_FLOAT_FIX value
 *
 * Revision 1.42  1997/01/22  16:17:30  erf
 * Added define for PCX2 floating point hardware fix.
 *
 * Revision 1.41  1996/12/11  02:21:38  jop
 * Changed PVR1 to ISPTSP for sim builds
 *
 * Revision 1.40  1996/12/05  21:26:35  jrc
 * Added code for freezing the rendered image and dumping parameters after a
 * certain number of frames.
 *
 * Revision 1.39  1996/12/05  17:01:36  jrc
 * Renamed DUMP_PARAM_FILES to DUMP_PARAMS so as not to clash with simulator
 * version.
 *
 * Revision 1.38  1996/11/29  12:53:34  gdc
 * put wrapper on DUMP_PARAM_FILES definition to stop pre-processor
 * complaints
 *
 * Revision 1.37  1996/11/25  21:14:25  jrc
 * Added Rowland's new sabre and texas parameter dumping.
 *
 * Revision 1.36  1996/11/19  01:00:09  jop
 * Fixed so that dregion.c funcs could coexist comfortably with
 * pmsabre.c
 *
 * Revision 1.35  1996/11/15  09:30:51  rgi
 * Added D3DOPTIM version of pmsabre module (pmsabrel) the
 * two co-exist at the moment even if the D3DOPTIM flag is
 * set but the pmsabrel module only gets used by SGLLite and
 * hence D3D HAL.
 *
 * Revision 1.34  1996/10/16  21:15:33  jop
 * Added GenerateObj..Strip and did something else I can't remember
 *
 * Revision 1.33  1996/10/11  17:08:55  jop
 * Added a structure member
 *
 * Revision 1.32  1996/10/09  18:09:52  jop
 * Added strip functionality
 *
 * Revision 1.31  1996/10/04  16:25:18  sjf
 * Killed off PMSabreOnRender or whatevers it's called.
 *
 * Revision 1.30  1996/10/03  14:21:07  jop
 * added PMSAbreOnRender
 *
 * Revision 1.29  1996/09/04  11:52:28  jop
 * New param in lite
 *
 * Revision 1.28  1996/09/02  17:44:48  jrc
 * Added a more complete version of region skipping for SGL Lite/Direct3D.
 *
 * Revision 1.27  1996/08/30  15:48:42  sjf
 * Added in small object rejection for PVR1
 *
 * Revision 1.26  1996/08/30  11:49:54  sjf
 * Deleted dead function prototypes, and added stuff for better sorting of
 * large translucent objects.
 *
 * Revision 1.25  1996/08/29  13:11:32  sjf
 * We need to do better sorting of large translucent objects... just
 * added new parameter to addregiontranslucent in antcipation.
 *
 * Revision 1.24  1996/07/11  11:03:17  sjf
 * Reduced MAX_X_REGIONS so that it was possible to implement the region masking.
 *
 * Revision 1.23  1996/04/26  14:37:04  sm
 * increase region limit
 *
 * Revision 1.22  1996/03/25  19:35:51  sjf
 * Added extern defs.
 *
 * Revision 1.21  1996/03/25  18:56:19  sm
 * added proto for a function i dont even use !
 *
 * Revision 1.20  1996/02/20  15:43:35  sm
 * added trans single func proto
 *
 * Revision 1.19  1995/10/27  14:22:23  jop
 * Increased max X and Y regions to 20x16 (32x32pix)
 *
 * Revision 1.18  1995/10/19  14:20:15  sjf
 * Change interface for  Generate Object Ptr mem.
 *
 * Revision 1.17  1995/10/17  12:16:26  jrc
 * 1. The functions now take a pointer to a single struct instead of the four
 *    region area edges separately.
 * 2. Added sentry against multiple #includes of this file.
 *
 * Revision 1.16  1995/10/04  08:34:27  sjf
 *  Made GenerateObjectPtrMemory return how much space it used.
 *
 * Revision 1.15  1995/10/03  19:40:48  jop
 * Added single region version of PackOpaqueParams
 *
 * Revision 1.14  1995/09/22  14:16:31  sjf
 * Removed redundant declarations.
 *
 * Revision 1.13  1995/09/19  09:05:15  sjf
 * Added two routines to support light volumes - AddCompleteShadowDummy and
 * AllowLightVolAddition.
 *
 * Revision 1.12  1995/09/12  10:17:44  sm
 * add region opaque dummy proto added
 *
 * Revision 1.11  1995/09/05  14:35:21  sm
 * *** empty log message ***
 *
 * Revision 1.10  1995/09/05  12:41:51  sm
 * *** empty log message ***
 *
 * Revision 1.10  1995/09/05  12:41:51  sm
 * *** empty log message ***
 *
 * Revision 1.9  1995/09/05  12:37:21  sm
 * *** empty log message ***
 *
 * Revision 1.8  1995/09/04  15:28:43  sm
 * changed for multi translucency
 *
 * Revision 1.7  1995/08/24  12:22:50  pdl
 * added some const.
 *
 * Revision 1.6  1995/08/21  15:44:02  sm
 * same!
 *
 * Revision 1.5  1995/07/26  14:36:06  sm
 * added MAX for x and y region sizes
 *
 * Revision 1.4  1995/07/26  08:53:28  sjf
 * 1) Moved internal typedefs from pmsabre.h
 * 2) changed ENUM consts so less likely to clash with other identifiers
 * 3) added limits to which regions get put into sabre param space so that
 * we won't render everything (I hope)
 * 4) Made some parameters const, and changed object "ptr" to be sgl_uint32 not a
 * actual pointer.
 *
 * Revision 1.3  1995/07/24  11:12:14  sm
 * cant remember !
 *
 * Revision 1.2  1995/07/07  14:37:42  sm
 * changed data structs
 *
 * Revision 1.1  1995/07/06  16:52:46  sm
 * Initial revision
 *
 *
 **************************************************************************/

#ifndef __PMSABRE_H
#define __PMSABRE_H


/*
// FIX_RENDER
//
// Renders as normal until the FIX_FRAME_NUM frame, after which that frame is
// repeated for all subsequent sgltri_render calls.
//
// If DUMP_WHEN_FIXED is set the hardware parameters are dumped to disk when
// the fixing of the image occurs.
*/
#define FIX_RENDER 0
#if FIX_RENDER
	extern int nFrameNum;
	#define FIX_FRAME_NUM 601 /* odd number for dumping from first buffer */
	#define DUMP_WHEN_FIXED 1
#endif


/*
// DUMP_PARAMS
//
// Dumps hardware parameters to disk.
*/
#define DUMP_PARAMS (0 | DUMP_WHEN_FIXED)


#define	MAX_X_REGIONS	32	/* WARNING To implement the viewport masking, I've
							// assumed that the max_x_regions is <= 32. IT
							// WILL BREAK if this is larger. However, as the
							// harware can only do a min of 32 pixels in X per
							// region, and the largest width is 1024, this is
							// not a problem
							*/
#define	MAX_Y_REGIONS	128 /* this will allow a 1024x768 device to
							** run with a Y-region height of 8
							** Previous 32 value wouldn't let a 800x600 run with
							** a height of 16
							*/


typedef struct
{
	int FirstXRegion, LastXRegion,
		FirstYRegion, LastYRegion;

} REGIONS_RECT_STRUCT;

typedef struct tagREGION_STRIP_STRUCT
{
	int		nXExtents[2];
	sgl_uint32	FirstObjectOffset;
	sgl_uint32	*pLastObject;
	sgl_bool fObjectsPresent;
	
} REGION_STRIP_STRUCT;


extern void		AddRegionTransDummy(const sgl_uint32  ObjectAddr);

extern void		AddRegionOpaqueDummy(const sgl_uint32  ObjectAddr);
 
extern	void	AddFlushingPlane(const sgl_uint32  ObjectAddr);

extern	void	AddTransFlushingPlane(const sgl_uint32  ObjectAddr);

extern	void	AddCompleteShadowDummy(const sgl_uint32  ObjectAddr);

extern void		AllowLightVolAddition(
  const REGIONS_RECT_STRUCT *const pRegionsRect);


extern	void	InitRegionData();

extern	void GenerateObjectPtrMemory(const REGIONS_RECT_STRUCT *const pRegionsRect,
									const sgl_uint32 * pRegionMask);

extern	int  GenerateObjectPtrMemoryLite(const REGIONS_RECT_STRUCT *const pRegionsRect,
									sgl_bool  bRenderAllRegions,
									int *nMaxPlanesInARegion);

extern	int  GenerateObjectPtrMemoryStrip(const REGIONS_RECT_STRUCT *const pRegionsRect,
									sgl_bool  bRenderAllRegions,
									int *nMaxPlanesInARegion);

REGION_STRIP_STRUCT *SetupStrip (int nStrip);

extern 	void	ResetRegionData();



extern void	AddRegionOpaque(const REGIONS_RECT_STRUCT *const pRegionsRect,
						const sgl_uint32  ObjectAddr, const int NumPlanes
						#if ISPTSP
						, int ApproxPixelSize
						#endif
						);

extern void	AddRegionShadow(const REGIONS_RECT_STRUCT *const pRegionsRect,
						const sgl_uint32  ObjectAddr, const int NumPlanes);


extern void	AddRegionLightVol(const REGIONS_RECT_STRUCT *const pRegionsRect,
						const sgl_uint32  ObjectAddr, const int NumPlanes);



typedef struct
{
	float BaseDepth;
	float DepthDx;
	float DepthDy;

}TRANS_REGION_DEPTHS_STRUCT;

extern void	AddRegionTranslucent(const REGIONS_RECT_STRUCT *const pRegionsRect,
						const sgl_uint32  ObjectAddr, const int NumPlanes, 
						float NearZ, 
						const TRANS_REGION_DEPTHS_STRUCT *NearZs);

void PackRegionOpaque (void *pData, int nIncrement, sgl_uint32 PlanesPerPoly, 
					   int nPolys, sgl_uint32 ISPAddr);

#ifdef D3DOPTIM
#include "dregion.h"
#endif

#endif

#if PCX2
	/* This is defined for PCX2 to fix the float to 20 bit conversion bug.
	 */
	#define MAGIC_FLOAT_FIX	(1.862645149231e-09)
#endif

/*------------------------------ End of File --------------------------------*/
