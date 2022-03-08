/**************************************************************************
 * Name         : dregion.h
 * Title        : parameter management for sabre
 * Author       : Rob Isherwood
 * Created      : 23rd October 1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: dregion.h,v $
 * Revision 1.26  1997/09/02  15:33:53  erf
 * 1. Moved some defines to dregion.h since they are common between
 *    dregion.c, d3dreg.c and d3dtsort.c .
 *
 * Revision 1.25  1997/08/27  08:45:35  gdc
 * more changes for gouraud highlight and vertex fog
 *
 * Revision 1.24  1997/08/18  16:23:25  gdc
 * re-order pass storage to avoid a page fault
 *
 * Revision 1.23  1997/08/18  15:36:56  gdc
 * removed ^M's
 *
 * Revision 1.22  1997/08/18  10:49:46  sxy
 * Added enum type for vertex fog and GOURAUD highlight.
 *
 * Revision 1.21  1997/08/14  11:23:45  sxy
 * removed ISPTSP's.
 *
 * Revision 1.20  1997/07/10  08:30:52  erf
 * Added routine AddRegionObjectsLightMaps() which is #if'd out
 * incase we implement light maps.
 *
 * Revision 1.19  1997/06/05  09:06:19  mjg
 * Added a missing prototype.
 *
 * Revision 1.18  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.17  1997/04/04  10:46:42  gdc
 * removed reference to D3DOptimFrame which didn't seem to be used anymore
 *
 * Revision 1.16  1997/03/18  10:26:07  gdc
 * moved OPAQUETRANS pass to number 3, gets rid of vingettting
 *
 * Revision 1.15  1997/03/18  09:27:41  mjg
 * But ISPTSP changed back into this main branch (previously in 1.8.1.3).
 *
 * Revision 1.14  1997/03/14  13:04:19  jop
 * Fixed some // comments fletch put in
 *
 * Revision 1.13  1997/03/14  11:59:24  jwf
 * Changes for Mac
 *
 * Revision 1.11  1997/03/05  12:20:52  jop
 * Added opaqtrans stuff
 *
 * Revision 1.10  1997/02/20  23:38:45  mjg
 * Forgot to include SGL light volume addition from ages ago.
 *
 * Revision 1.9  1997/02/12  12:28:57  erf
 * New routine AddRegionBeginTransDummyL() routine.
 *
 * Revision 1.8  1997/02/06  14:02:07  mjg
 * Altered parameter for ResetRegionDataL for SGL in-app mode changes.
 *
 * Revision 1.7  1997/01/10  15:17:46  gdc
 * added prototype for GenerateObjectPtr - which is for SGL
 *
 * Revision 1.6  1997/01/08  12:17:51  jrc
 * Possible optimisation of ENCODE_OBJXYDATA.
 *
 * Revision 1.5  1996/12/10  15:28:42  jop
 * Added 2d composoiting support
 *
 * Revision 1.4  1996/12/03  02:13:21  erf
 * Added extern definitions for routines that are used globally in D3DOPTIM.
 *
 * Revision 1.3  1996/11/29  16:34:40  rgi
 * Added MaxRegionWidth/Height API to allow hwdevice to get
 * the info it needs to predict what dregion may do with
 * tile sizes.
 *
 * Revision 1.2  1996/11/26  16:16:49  rgi
 * Removed the private API between pmsabre and dregion (extern
 * defs are now in pmsabre itself).
 * Added new single poly functions for SGL to call, the translucent
 * stuff should now be easy to port just negate SGL's version of
 * the plane equation.
 * Sadly LightVol, Shadow, and Translucent code not yet tested.
 *
 * Revision 1.1  1996/11/19  00:55:35  jop
 * Initial revision
 *
 *
 *************************************************************************/

#ifndef __DREGION_H__
#define __DREGION_H__

#include "pmsabre.h"	/* JWF needed for def of TRANS_REGION_DEPTHS_STRUCT */

/*
// If this value is changed ENCODE_OBJXYDATA should be updated accordingly.
*/
#define Y_SHIFT	1				/* Divide all Y's by 2 */

/* We pack five items into the sgl_uint32 XYData describing each object,
   pass pXYData as a pointer to the first with nXYDataInc as sizeof the
   bigger structure it is no doubt part of.
   
   X Values passed in here should be shifted down already into fixed
   size Region indexes. Y Values however should be in pixels.        */

#if 0
  /* original version */
  #define ENCODE_OBJXYDATA( Type, X0, Y0, X1, Y1 ) ( \
       ( ((sgl_uint32) (Type))          <<29) +\
       ((((sgl_uint32)   (Y0))>>Y_SHIFT)<<19) +\
       ((((sgl_uint32)   (Y1))>>Y_SHIFT)<<10) +\
       ( ((sgl_uint32)   (X0))          << 5) + ((sgl_uint32) X1) )
#else
  /*
  // version that might be faster because two shifts cannot be done in
  // parallel, but a shift and a mask can.
  */
  #define ENCODE_OBJXYDATA( Type, X0, Y0, X1, Y1 ) ( \
       (   ((sgl_uint32)(Type))        <<29) +\
       ( ( ((sgl_uint32)(Y0)) & 0x3fe) <<18) +\
       ( ( ((sgl_uint32)(Y1)) & 0x3fe) <<9) +\
       (   ((sgl_uint32)(X0))          <<5) + ((sgl_uint32)(X1)) )
#endif

/* The 3 bit Type value describes the type of each object concerned */
#define PACKED_TYPE_OPAQUE			0
#define PACKED_TYPE_SHADOW			1
#define PACKED_TYPE_LIGHTVOL		2
#define PACKED_TYPE_OPAQUETRANS     3
#define PACKED_TYPE_TRANSLUCENT		4   /* Base of translucent types      */

#define PACKED_TYPE_GOURAUDHIGHLIGHT  0	        /* GOURAUD Highlight			  */
#define PACKED_TYPE_VERTEXFOG	      1	        /* Vertex Fog 					  */
#define PACKED_TYPE_TRANS_GOURAUDHIGHLIGHT  2	/* GOURAUD Highlight on a translucent object */
#define PACKED_TYPE_TRANS_VERTEXFOG	        3   /* Vertex Fog on a translucent object */

#define		PACKED_TRANSTYPE_SETMARK  2	/* Flag for start of a new set    */
#define     PACKED_TRANSTYPE_BACKBIT  1	/* Use LSB to indicate FRONT/BACK */


/* enum types for GOURAUD fog and highlight  */
typedef enum
{
	NO_FOG_AND_HIGHLIGHT,				 /* No Vertex fog and GOURAUD highlight   */
	GOURAUD_HIGHLIGHT,					 /* GOURAUD highlight					  */
	VERTEX_FOG,							 /* Vertex fog							  */
	VERTEX_FOG_AND_GOURAUD_HIGHLIGHT	 /* Vertex fog and GOURAUD highlight 	  */
} FOGHIGHLIGHT, *PFOGHIGHLIGHT;

typedef struct tagREGION_STRIP_DATA
{
	int		nXExtents[2];
	sgl_uint32	FirstObjectOffset;
	sgl_uint32	*pLastObject;
	sgl_bool fObjectsPresent;
	
} REGION_STRIP_DATA, *PREGION_STRIP_DATA;


/* For each translucent object provide a TRANS_REGION_DEPTHS structure,
   as for AddRegionTranslucent in pmsabre.h. Pass pZData as a pointer to
   the first with nZDataInc as sizeof the list elements it is part of.   */

extern void 	AddRegionObjects (  sgl_uint32 *pXYData, 
									int nXYDataInc,
									sgl_uint32 PlanesPerPoly, 
									int nPolys, 
									sgl_uint32 ISPAddr,
									const TRANS_REGION_DEPTHS_STRUCT *ZData, 
									int nZDataInc);

/* the routine takes an extra parameter for vertex fog and GOURAUD highlight */
extern void 	AddRegionObjectsExtra( 	sgl_uint32 *pXYData, 
										int nXYDataInc,
                     	  				sgl_uint32 PlanesPerPoly, 
                     	  				int nPolys, 
                     	  				sgl_uint32 ISPAddr,
					   					const TRANS_REGION_DEPTHS_STRUCT *pZData, 
					   					int nZDataInc, 
					   					FOGHIGHLIGHT *pFogHighlight);

#if 0
/* Required for implementation of light maps.
 */
extern void AddRegionObjectsLightMap( sgl_uint32 *pXYData, int nXYDataInc,
                       sgl_uint32 PlanesPerPoly, int nPolys, sgl_uint32 ISPAddr,
					   const TRANS_REGION_DEPTHS_STRUCT *pZData, int nZDataInc);
#endif



extern int		GenerateObjectPtr (const REGIONS_RECT_STRUCT *const pRegionsRect,
								   sgl_uint32 *RegionMask);

extern int		GenerateObjectPtrLite (const REGIONS_RECT_STRUCT *const pRegionsRect,
								 		sgl_bool  bRenderAllRegions);

extern void 	GenerateObjectPtrLiteStrip( const REGIONS_RECT_STRUCT *const pRegionsRect,
									    sgl_bool  bRenderAllRegions );

int SetupStripLite (int nStrip,
					const REGIONS_RECT_STRUCT *const pRegionsRect,
					PREGION_STRIP_DATA pRegionStripData);

extern void		ResetRegionDataL(sgl_bool bForceReset);

extern void AddRegionSolid( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr);

extern void AddRegionSeeThru( sgl_uint32 XYData, sgl_uint32 TransSetId, sgl_uint32 Planes,
					  sgl_uint32 ISPAddr, const TRANS_REGION_DEPTHS_STRUCT *pZData);

extern void AddRegionAtmos( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr );

/* Use these to decide what the real maximum tile size is, MaxY happens
   to correspond to RegionInfo.YSize at the moment but whoe knows?
   RegionInfo.XSize is currently the minimum tile width however so you
   do need this API to tell whats going on.                            */
extern int MaxRegionHeight( void );
extern int MaxRegionWidth( void );


/* The following parts of pmsabrel module are called solely
   from their pmsabre equivalents here to let the old API live on for
   a while. Please remove these when from dregion when this module
   becomes disused */

extern void		AddRegionTransDummyL(const sgl_uint32  ObjectAddr);

extern void		AddRegionBeginTransDummyL(const sgl_uint32  ObjectAddr);

extern void		AddRegionOpaqueDummyL(const sgl_uint32  ObjectAddr);
 
extern void		AddFlushingPlaneL(const sgl_uint32  ObjectAddr);

extern void		AddTransFlushingPlaneL(const sgl_uint32  ObjectAddr);

extern void		AddCompleteShadowDummyL(const sgl_uint32  ObjectAddr);

extern void		InitRegionDataL (void);

extern void		AddRegionOpaqueL(const REGIONS_RECT_STRUCT *const pRegionsRect,
                	             const sgl_uint32  ObjectAddr, const int NumPlanes);

extern void		AddRegionShadowL(const REGIONS_RECT_STRUCT *const pRegionsRect,
                	             const sgl_uint32  ObjectAddr, const int NumPlanes);


extern void		AddRegionLightVolL(const REGIONS_RECT_STRUCT *const pRegionsRect,
                	               const sgl_uint32  ObjectAddr, const int NumPlanes);

extern void		AddRegionTranslucentL(const REGIONS_RECT_STRUCT *const pRegionsRect,
	                                  const sgl_uint32  ObjectAddr, const int NumPlanes,
	                                  float NearZ, 
	                                  const TRANS_REGION_DEPTHS_STRUCT *NearZs);

extern void		AllowLightVolAdditionL(const REGIONS_RECT_STRUCT *const pRegionsRect);
extern void		AllowLightVolAdditionSGL(const REGIONS_RECT_STRUCT *const pRegionsRect);

#if VIGNETTE_FIX
extern void		AddRegionOpaqueDummyLargeL(const sgl_uint32  ObjectAddr);
#endif


/**************************************************************************
 **************************************************************************

	Hardware limits and dependancies

 **************************************************************************
 **************************************************************************/

/* Compute the max amount of planes we may throw into a pass to allow
   for min. This is the MIN we must have + the flushing plane + the size
   of the lumps we add.

   To make things easier, make sure that even if we add loads of translucent
   objects, we always have space for the minimum opaque ones.               */

#define FLUSH_PLANE (1)

	/* keep a reserve of object points so that we dont need to check all
	** the indivdual curAddr increments - cause we know the in advance
	** total
	*/ 
#define OBJECT_POINTER_SAFETY_MARGIN (16)

#define SAFETY_MARGIN_OPAQ  (REGION_PLANE_MIN + FLUSH_PLANE + NUM_DUMMY_OBJECT_PLANES)
#define MIN_TRANS_PLANES  (NUM_DUMMY_OBJECT_PLANES * (REGION_PLANE_MIN/NUM_DUMMY_OBJECT_PLANES + 1))

#if PCX1
	#define SAFETY_MARGIN_TRANS (REGION_PLANE_LIM - NUM_DUMMY_OBJECT_PLANES - \
			                     NUM_TRANS_PASS_START_PLANES - FLUSH_PLANE - FLUSH_PLANE - \
							     (NUM_DUMMY_OBJECT_PLANES * (REGION_PLANE_MIN/NUM_DUMMY_OBJECT_PLANES + 1)))
	#define SAFETY_MARGIN_TRANS_SGL (REGION_PLANE_MIN + FLUSH_PLANE + NUM_DUMMY_OBJECT_PLANES)
#else 
	#define SAFETY_MARGIN_TRANS (REGION_PLANE_LIM - FLUSH_PLANE - NUM_DUMMY_OBJECT_PLANES)	
#endif


#endif
/* end of $RCSfile: dregion.h,v $ */
