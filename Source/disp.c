/******************************************************************************
 * Name         : disp.c
 * Title        : ISP plane packing routines for PowerSGL Direct
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: disp.c,v $
 * Revision 1.65  1998/08/13  09:50:34  ls
 * NewmanHaas was dropping triangles in the release build but not in
 * the debug.  (Fine in the release without the -Qip option of the CFLAGS).
 * Tracked this down to the (FLOAT_TO_LONG)fMaxVal < LONG_INVMAGIC - a speed
 * up for the pentium.  fMaxVal was typically 0.00000 so I presume the FLOAT_TO
 * LONG has a problem with this.  Forcing it to do the float compare once agai
 * fixes the game.
 *
 * Revision 1.64  1997/11/10  14:18:19  gdc
 * made depth bias work
 *
 * Revision 1.63  1997/11/07  15:59:45  gdc
 * added depth bias - but not tested yet
 *
 * Revision 1.62  1997/10/16  11:36:50  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.61  1997/10/15  11:05:34  sxy
 * added some DPFDEVF messages.
 *
 * Revision 1.60  1997/09/24  14:29:57  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.59  1997/08/28  13:58:25  gdc
 * set number back from 5 to 4
 *
 * Revision 1.58  1997/08/27  16:48:06  gdc
 * shelias fix for vingetting
 *
 * Revision 1.57  1997/08/26  08:46:01  gdc
 * fixed a compiler warning and removed uneeded code
 *
 * Revision 1.56  1997/08/22  16:35:40  sxy
 * Fixed a bug in counting TSPSize in PackISPPolygonExtra.
 *
 * Revision 1.55  1997/08/21  16:32:55  sxy
 * added vertex fog in quads routines.
 *
 * Revision 1.54  1997/08/18  16:24:18  gdc
 * started adding stuff for vertex for and smooth highlights,
 * PCX2 and triangles only at the moment
 *
 * Revision 1.53  1997/08/11  14:00:49  gdc
 * removed ISPTSP compatibility
 *
 * Revision 1.52  1997/08/08  19:41:28  mjg
 * Added PCX2_003 compatibility.
 *
 * Revision 1.51  1997/07/16  10:44:33  mjg
 * Recalculated the DataSize variable and added the MODULE_ID.
 *
 * Revision 1.50  1997/06/17  09:28:34  mjg
 * Added the FORCE_NO_FPU option.
 *
 * Revision 1.49  1997/06/09  15:46:59  mjg
 * Hopefully fixed some compiler warnings and broken nothing:
 * 1) Removed a rather unpleasant l-value cast.
 * 2) Added a cast to resolve a signed/unsigned equality.
 * 3) For ISPTSP and PCX1 removed long/float mismatch by pre-calculating
 *    the FLOAT_TO_FIXED * (+/-) 0.5f value.
 * 4) Removed some token ^Ms.
 *
 * Revision 1.48  1997/06/06  10:00:28  erf
 * Fixed clipped tested PCX1 & ISPTSP packing routines, when special
 * case required.
 *
 * Revision 1.47  1997/06/04  08:57:12  erf
 * Fixed clipping code for PCX1 and ISPTSP. The PackISP clip tested
 * routines are in C. These will have to be converted to assembler
 * at some stage.
 *
 * Revision 1.46  1997/06/03  15:55:15  erf
 * Added Simon's clipping fix to PCX1 & ISPTSP and for N sided polygons.
 *
 * Revision 1.45  1997/06/02  14:15:51  sjf
 * WARNING CONTAINS UNTESTED CODE!
 * ===============================
 * Added the "unclipped" version of the pack routines which will
 * test the ranges of the Edge parameters before packing them.
 * This has been tested fairly well (via tombraider) with PCX2 but
 * not with PCX1.
 *
 * Please also note: It has not been done for the polygon core (only
 * the triangles), and it should also be added to SGL.
 *
 * Revision 1.44  1997/06/02  02:44:06  jop
 * Small changed to improve asm intermediate builds
 *
 * Revision 1.43  1997/05/19  09:02:08  gdc
 * moved get sabre limit to parabuff.h
 *
 * Revision 1.42  1997/05/19  07:35:17  jop
 * Fixed sabre buffer limit checks
 *
 * Revision 1.41  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.40  1997/04/22  18:00:44  mjg
 * Removed brdsetup.h and altered the SabreMem functions.
 *
 * Revision 1.39  1997/04/21  15:24:16  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.38  1997/04/08  13:52:59  jop
 * Cleaned up includes a bit
 *
 * Revision 1.37  1997/04/02  13:43:17  gdc
 * a couple of defines with prepended _'s so dispml.asm funcs link
 *
 * Revision 1.36  1997/03/31  22:05:55  jop
 * rationalised WIN32s, #include "windows.h", and stuck in some DOS32s
 *
 * Revision 1.35  1997/03/14  18:08:08  jop
 * Added pvrosapi.h include!
 *
 * Revision 1.34  1997/03/14  15:56:32  jwf
 * Added HWIO macros for macintosh compatability
 *
 * Revision 1.33  1997/03/14  13:04:19  jop
 * Fixed some // comments fletch put in
 *
 * Revision 1.32  1997/03/14  11:05:01  jwf
 * Made some changes to make it compile on the macintosh
 *
 * Revision 1.31  1997/02/24  10:40:06  gdc
 * removed a PCX2 conditional bit of code from within a !PCX2 bit of code
 * and neatened up a bit
 *
 * Revision 1.30  1997/01/30  09:46:53  erf
 * Revised PCX2 optimization mentioned in 1.27.
 *
 * Revision 1.29  1997/01/22  16:19:06  erf
 * Fixed PCX2 floating point hardware bug in software.
 * Tidied up PCX2 packing code. ie seperated for PCX1 and ISPTSP.
 *
 * Revision 1.28  1997/01/21  11:46:06  mjg
 * Removed changes made in version 1.26.
 *
 * Revision 1.27  1997/01/21  11:03:14  erf
 * Optimized PCX2 path.
 *
 * Revision 1.26  1997/01/17  14:42:09  mjg
 * Test to stop very complex scenes on midas4 from hanging - effective?
 *
 * Revision 1.25  1997/01/16  21:26:29  erf
 * Fixed PCX2 problem where meshs on edge of screen were not displayed.
 *
 * Revision 1.24  1997/01/03  14:31:52  rgi
 * Replaced the SGLTRI_SPARE_TIME metric with PACK_ISPCORE_TIME
 * so that it is a bit more intuitive.
 *
 * Revision 1.23  1996/12/11  19:37:39  mjg
 * Put in a check to ensure the sabre limit for non-PCX builds.
 *
 * Revision 1.22  1996/12/10  23:21:54  erf
 * Changed compile switches:
 * 1. PCX1 to PCX1 || PCX2.
 * 2. PVR1 to ISPTSP.
 * 3. PVR2 to PCX1.
 * 4. PVR3 to PCX2.
 *
 * Revision 1.21  1996/12/06  15:38:56  jop
 * Fixed netrace crashing bug (null trans depth struct ptr)
 *
 * Revision 1.20  1996/12/05  20:55:14  jop
 * Removed asm compile flags to dtri.h
 *
 * Revision 1.19  1996/12/05  18:03:45  jop
 * Fixed near-perp bug
 *
 * Revision 1.18  1996/12/05  13:32:16  erf
 * Add #elif to use assembler verions if WIN32 && PVR3.
 *
 * Revision 1.17  1996/12/03  10:56:42  rgi
 * Added assembler PackISPPolygonCore after making the
 * C more efficient and easier to rip off.
 *
 * Revision 1.16  1996/12/02  13:29:28  mjg
 * Sorted out the FLOAT_TO_LONG(INVMAGIC) problem.
 *
 * Revision 1.15  1996/12/02  13:13:32  jop
 * Fixed processing of extra edge in poly pack routine
 *
 * Revision 1.14  1996/12/02  11:20:40  mjg
 * Changed a nested #define that wouldn't compile for Midas3.
 *
 * Revision 1.13  1996/11/30  04:52:48  jop
 * Fixed a comment bug and went through poly code to hopefully
 * accelerate, and bring the architecture into line with the
 * rest of the new regime. Initial results are disappointing.
 * Ah well, there's always the Severn bridge.
 *
 * Revision 1.12  1996/11/30  00:39:34  sjf
 * Small amount of optimisation (mainly doing INT compares instead of float)
 * Well OK the optimisation is only for pentium.
 *
 * Revision 1.11  1996/11/29  19:26:58  jop
 * Things
 *
 * Revision 1.10  1996/11/28  17:15:14  gdc
 * put define for InPackTo20Bit to PackTo20Bit when pto20b.h is included
 * to stop link undef'd extern error
 *
 * Revision 1.9  1996/11/28  15:53:51  sjf
 * Moved pack to 20 bits into its own file so that we are no longer duplicating
 * it.
 *
 * Revision 1.8  1996/11/28  12:11:15  sjf
 * 1) Got rid of those ^Ms
 * 2) Redid the maths to use the new basis vectors which makes a lot
 *    of the maths disappear up its own sphincter.
 * 3) Ummm Haven't actually compiled it yet SO it may have some
 *   syntax errors.
 *
 * Revision 1.7  1996/11/27  19:00:01  mjg
 * Started messin' about with translucency sorting.
 *
 * Revision 1.6  1996/11/27  00:08:27  jop
 * Improved pcx buffer efficiency
 *
 * Revision 1.5  1996/11/23  04:14:50  jop
 * Structure changes
 *
 * Revision 1.4  1996/11/22  02:24:30  jrc
 * First attempt at polygons.
 *
 * Revision 1.3  1996/11/19  11:27:49  rgi
 * Modified to work with METRICS and the new assembler
 * version done for performance dispml.asm in win32.
 *
 * Revision 1.2  1996/11/19  00:53:42  jop
 * Got it to build
 *
 * Revision 1.1  1996/11/18  18:29:36  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_DISP

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "metrics.h"
#include "pvrlims.h"
#include "hwinterf.h"
#include "parmbuff.h"
#include "sgl_math.h"
#include "dregion.h"	/* JWF added for missing prototypes */

#include "pmsabre.h"

#include "dtri.h"
#include "disp.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

#define UPPER_6_OF_TAG		0x3F000UL
#define SFLOAT_20BIT_ZERO	(0)
#define FLOAT_TO_FIXED 		((float) (((sgl_uint32)1) <<31))
#define MAGIC  				(1024.0f)
#define INVMAGIC  			(1.0f/MAGIC)

#if SLOW_FCMP && !MULTI_FP_REG
/* A bit messy, but the pre-processor was unable to deal with the
 * value "FLOAT_TO_LONG (INVMAGIC)".
 */
#define LONG_INVMAGIC		0x3A800000
#endif

/*
// Define some dummy Tags for the planes which don't use them, but
// Would be useful for debugging. Note in the non debug, use small
// 16 bit constants as these should be loaded faster.
*/
#if DEBUG

#define DEBUG_CLIP_PLANE_TAG 0xAAAAAAUL
#define DEBUG_INVIS_FP_TAG	 0xBBBBBBUL
#define DEBUG_INVIS_RP_TAG	 0xCCCCCCUL
#define DEBUG_PERP_TAG		 0xDDDDDDUL
#define DEBUG_DUMMY_F_TAG	 0xEEEEEEUL
#define DEBUG_DUMMY_R_TAG	 0xFFFFFFUL

#else

#define DEBUG_CLIP_PLANE_TAG 0x000000UL
#define DEBUG_INVIS_FP_TAG	 0x000000UL
#define DEBUG_INVIS_RP_TAG	 0x000000UL
#define DEBUG_PERP_TAG		 0x000000UL
#define DEBUG_DUMMY_F_TAG	 0x000000UL
#define DEBUG_DUMMY_R_TAG	 0x000000UL

#endif

extern float gfDepthBias;

static TRANS_REGION_DEPTHS_STRUCT gDepthInfo[IBUFFERSIZE];

/* this array indicates whether polys have a vfog or gouraud specular plane */
static FOGHIGHLIGHT eExtraStatus[IBUFFERSIZE];

/* ////////////////////////////////////////////////////
// Include the correct pack to 20 Bit
//////////////////////////////////////////////////// */

#if PACKISPCORE || PACKISPPOLYGONCORE || PACKISPCORECLIPTESTED || PACKISPPOLYGONCORECLIPTESTED

	#if 1								/* Change if verifying assembler stuff */

		/*
		// Include the real C Packto20bit code
		*/
		#define INLINE_P20 static INLINE
		#include "pto20b.h"
		#define InPackTo20Bit PackTo20Bit				/* Try C version      */
		#undef  INLINE_P20

	#else

		/* For debuggin assembler equivalents */
		extern sgl_int32 PackTo20Bit(const float Value);		/* pkisp.c version    */
		extern sgl_int32 PackTo20BitTest(const float Value);	/* dispml.asm version */

		#if 0
			#define InPackTo20Bit PackTo20BitTest			/* Try ASM version    */
		#elif 1
			#define InPackTo20Bit PackTo20Bit				/* Try C version      */
		#else
			#define InPackTo20Bit PackTo20BitTrial		/* Try both?          */

			static INLINE sgl_int32 PackTo20BitTrial( const float Value )
			{
				sgl_int32 Res1 = PackTo20Bit(Value);
				sgl_int32 Res2 = PackTo20BitTest(Value);

				if ( Res1 != Res2 ) __asm int 3;

				return (Res1);
			}

		#endif

	#endif

#endif /* #if PACKISPCORE || PACKISPPOLYGONCORE */


/**********************************************************************
**
** Function to generate the pittance of depth information used
** by the list sorting of translucent objects in dregion.c
** At one point some tried to put in dz/dx and dz/dy parameters
** to improve sorting, but I don't think it has been fully implemented
**
**********************************************************************/

void GenerateDepthInfo(PITRI pTri, sgl_uint32 nPolys)
{
	TRANS_REGION_DEPTHS_STRUCT* pGDepth;

	pGDepth = &gDepthInfo[0];	

	for ( ; nPolys; nPolys--, pGDepth++)
	{
		/* static variable should init these to zero */
		#if 0
		pGDepth->DepthDx = 0.0f;
		pGDepth->DepthDy = 0.0f;
		#endif

		/*
		// Find and save the miniumum of the "Z" (i.e. 1/w) values. 
		// Now, to my knowledge, these Z values are all positive, so
		// fcloate compare is nad, use Integer instead.
		*/
		
		ASSERT(pTri->fZ[0] >= 0);
		ASSERT(pTri->fZ[1] >= 0);
		ASSERT(pTri->fZ[2] >= 0);

		#if (SLOW_FCMP && !MULTI_FP_REG)
		{		
			long  lTemp;
		
			lTemp = MIN( FLOAT_TO_LONG(pTri->fZ[2]), 
					 MIN( FLOAT_TO_LONG(pTri->fZ[0]), 
						  FLOAT_TO_LONG(pTri->fZ[1])));

			pGDepth->BaseDepth = LONG_TO_FLOAT(lTemp);
		}
		#else

			pGDepth->BaseDepth = MIN(pTri->fZ[2], MIN(pTri->fZ[0], pTri->fZ[1]));

		#endif

		pTri ++;
	}	
}

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU


/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges
** This routine is similar to the below except it also checks the Edges to make
** sure they don't go out of range
** It also add the extra planes needed for smooth specular and vertex fog
** 
**********************************************************************/

void PackISPCoreClipTestedExtra(sgl_uint32 *pPlaneMem, 
								sgl_uint32  nPolysInChunk, 
								sgl_uint32 *rTSPAddr, 
								PITRI      *rpTri,
								int         nIncrement, 
								PIMATERIAL *rpMat,
								int         nMatInc, 
								sgl_uint32  TSPIncrement,
								sgl_uint32  *rTSPOffsetAddr,
								sgl_uint32  TSPIncrementExtra)
{
	float 	fA, fB, fC;
	float 	fAe, fBe, fCe;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;
	sgl_uint32	TSPOffsetAddr = *rTSPOffsetAddr;
	PIMATERIAL	pMat = *rpMat;
	PFOGHIGHLIGHT epExtras = eExtraStatus;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		FW( pPlaneMem, 0+0, fA);
		FW( pPlaneMem, 1+0, fB);

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
		{
			/* If problem set C to 0.
		     */
			fC = 0.0f;
			IW( pPlaneMem, 2+0, 0);
		}
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/

        /* Doing it the INT way fails when fMaxVal is 0.0 - so have to go back to the
		   float way. This problem was highlighted by the release build with the -Qip
		   option of the CFLAGS */

		if (fMaxVal > INVMAGIC)
		{
			fA = 0.0f;
			fB = 0.0f;
			fC = fZ2;
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fAe = pTri->fAdjoint[0][2];
		fBe = pTri->fAdjoint[1][2];
		fCe = pTri->fAdjoint[2][2];
		/* Check the range*/
		if(sfabs((2*INVMAGIC) * fCe + fAe + fBe) > (sfabs(fAe) + sfabs(fBe)))
		{
			fAe = 0.0f;
			fBe = 0.0f;
		}
		FW( pPlaneMem, 0+4, fAe);
		FW( pPlaneMem, 1+4, fBe);
		FW( pPlaneMem, 2+4, fCe);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane 
		 */
		fAe = pTri->fAdjoint[0][0];
		fBe = pTri->fAdjoint[1][0];
		fCe = pTri->fAdjoint[2][0];
		/* Check the range*/
		if(sfabs((2*INVMAGIC) * fCe + fAe + fBe) > (sfabs(fAe) + sfabs(fBe)))
		{
			fAe = 0.0f;
			fBe = 0.0f;
		}
		FW( pPlaneMem, 0+8, fAe);
		FW( pPlaneMem, 1+8, fBe);
		FW( pPlaneMem, 2+8, fCe);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do third edge plane
		 */
		fAe = pTri->fAdjoint[0][1];
		fBe = pTri->fAdjoint[1][1];
		fCe = pTri->fAdjoint[2][1];
		/* Check the range*/
		if(sfabs((2*INVMAGIC) * fCe + fAe + fBe) > (sfabs(fAe) + sfabs(fBe)))
		{
			fAe = 0.0f;
			fBe = 0.0f;
		}
		FW( pPlaneMem, 0+12, fAe);
		FW( pPlaneMem, 1+12, fBe);
		FW( pPlaneMem, 2+12, fCe);
		IW( pPlaneMem, 3+12, (forw_perp | (DEBUG_PERP_TAG << 4)));
		
		TSPAddr += TSPIncrement;
		pPlaneMem += WORDS_PER_PLANE * 4;
		
		/* if the smooth specular plane is added ... */
		if(pMat->nSmoothSpecularIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras = GOURAUD_HIGHLIGHT;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}
		else
		{
			/* no extra plane */
			*epExtras = NO_FOG_AND_HIGHLIGHT;
		}

		/* if the fog plane is added ... */
		if(pMat->nFogIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras |= VERTEX_FOG;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}

		*((sgl_uint32 *) &pTri) += nIncrement;
		*((sgl_uint32 *) &pMat) += nMatInc;

		epExtras++;
	}

	*rpTri = pTri;
	*rpMat = pMat;
	*rTSPAddr = TSPAddr;
	*rTSPOffsetAddr = TSPOffsetAddr;
}

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges - technically a Triangle
** Also conditionally adds an extra face if there is vertex fog
** on the triangle
**
**********************************************************************/

void PackISPCoreExtra(sgl_uint32 *pPlaneMem, 
					  sgl_uint32  nPolysInChunk, 
					  sgl_uint32 *rTSPAddr, 
					  PITRI      *rpTri,
					  int         nIncrement, 
					  PIMATERIAL  *rpMat,
					  int         nMatInc, 
					  sgl_uint32  TSPIncrement,
					  sgl_uint32  *rTSPOffsetAddr,
					  sgl_uint32  TSPIncrementExtra)
{
	float 	fA, fB, fC;
	float 	fAe, fBe, fCe;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	PIMATERIAL	pMat = *rpMat;
	sgl_uint32	TSPAddr = *rTSPAddr;
	sgl_uint32	TSPOffsetAddr = *rTSPOffsetAddr;
	PFOGHIGHLIGHT epExtras = eExtraStatus;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		FW( pPlaneMem, 0+0, fA);
		FW( pPlaneMem, 1+0, fB);

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
		{
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
			fC = 0.0f;
		}
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
#else
		if (fMaxVal > INVMAGIC)
#endif
		{
			fA = 0.0f;
			fB = 0.0f;
			fC = fZ2;
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fAe = pTri->fAdjoint[0][2];
		fBe = pTri->fAdjoint[1][2];
		fCe = pTri->fAdjoint[2][2];
		FW( pPlaneMem, 0+4, fAe);
		FW( pPlaneMem, 1+4, fBe);
		FW( pPlaneMem, 2+4, fCe);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane 
		 */
		fAe = pTri->fAdjoint[0][0];
		fBe = pTri->fAdjoint[1][0];
		fCe = pTri->fAdjoint[2][0];
		FW( pPlaneMem, 0+8, fAe);
		FW( pPlaneMem, 1+8, fBe);
		FW( pPlaneMem, 2+8, fCe);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do third edge plane
		 */
		fAe = pTri->fAdjoint[0][1];
		fBe = pTri->fAdjoint[1][1];
		fCe = pTri->fAdjoint[2][1];
		FW( pPlaneMem, 0+12, fAe);
		FW( pPlaneMem, 1+12, fBe);
		FW( pPlaneMem, 2+12, fCe);
		IW( pPlaneMem, 3+12, (forw_perp | (DEBUG_PERP_TAG << 4)));

		TSPAddr += TSPIncrement;

		pPlaneMem += WORDS_PER_PLANE * 4;

		/* if the smooth specular plane is added ... */
		if(pMat->nSmoothSpecularIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras = GOURAUD_HIGHLIGHT;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}
		else
		{
			/* no extra plane */
			*epExtras = NO_FOG_AND_HIGHLIGHT;
		}

		/* if the fog plane is added ... */
		if(pMat->nFogIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras |= VERTEX_FOG;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}

		*((sgl_uint32 *) &pTri) += nIncrement;
		*((sgl_uint32 *) &pMat) += nMatInc;
		epExtras++;
	}

	*rpTri = pTri;
	*rpMat = pMat;
	*rTSPAddr = TSPAddr;
	*rTSPOffsetAddr = TSPOffsetAddr;
}

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges - technically a Triangle
**
**********************************************************************/

void PackISPCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, sgl_uint32 *rTSPAddr, PITRI *rpTri,
															int nIncrement, sgl_uint32 TSPIncrement)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		FW( pPlaneMem, 0+0, fA);
		FW( pPlaneMem, 1+0, fB);

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
				IW( pPlaneMem, 0+0, 0); 
				IW( pPlaneMem, 1+0, 0);
				FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];
		FW( pPlaneMem, 0+4, fA);
		FW( pPlaneMem, 1+4, fB);
		FW( pPlaneMem, 2+4, fC);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane 
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		FW( pPlaneMem, 0+8, fA);
		FW( pPlaneMem, 1+8, fB);
		FW( pPlaneMem, 2+8, fC);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do third edge plane
		 */
		fA = pTri->fAdjoint[0][1];
		fB = pTri->fAdjoint[1][1];
		fC = pTri->fAdjoint[2][1];
		FW( pPlaneMem, 0+12, fA);
		FW( pPlaneMem, 1+12, fB);
		FW( pPlaneMem, 2+12, fC);
		IW( pPlaneMem, 3+12, (forw_perp | (DEBUG_PERP_TAG << 4)));
		
		pPlaneMem += WORDS_PER_PLANE * 4;
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
}

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges
** This routine is similar to the above except it also checks the Edges to make
** sure they don't go out of range
**
**********************************************************************/

void PackISPCoreClipTested(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, sgl_uint32 *rTSPAddr, PITRI *rpTri,
															int nIncrement, sgl_uint32 TSPIncrement)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		FW( pPlaneMem, 0+0, fA);
		FW( pPlaneMem, 1+0, fB);

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
				IW( pPlaneMem, 0+0, 0); 
				IW( pPlaneMem, 1+0, 0);
				FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];
		/* Check the range*/
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			fA = 0.0f;
			fB = 0.0f;
		}
		FW( pPlaneMem, 0+4, fA);
		FW( pPlaneMem, 1+4, fB);
		FW( pPlaneMem, 2+4, fC);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane 
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		/* Check the range*/
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			fA = 0.0f;
			fB = 0.0f;
		}
		FW( pPlaneMem, 0+8, fA);
		FW( pPlaneMem, 1+8, fB);
		FW( pPlaneMem, 2+8, fC);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do third edge plane
		 */
		fA = pTri->fAdjoint[0][1];
		fB = pTri->fAdjoint[1][1];
		fC = pTri->fAdjoint[2][1];
		/* Check the range*/
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			fA = 0.0f;
			fB = 0.0f;
		}
		FW( pPlaneMem, 0+12, fA);
		FW( pPlaneMem, 1+12, fB);
		FW( pPlaneMem, 2+12, fC);
		IW( pPlaneMem, 3+12, (forw_perp | (DEBUG_PERP_TAG << 4)));
		
		pPlaneMem += WORDS_PER_PLANE * 4;
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
}

 
/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the extra edges of a polygon
**
**********************************************************************/

INLINE sgl_uint32 *PackISPPoly(sgl_uint32 *pPlaneMem, PIEDGE *ppEdge, int nExtraVertices )
{
	int 	nEdge = nExtraVertices + 1;
	PIEDGE	pEdge = *ppEdge;

	do
	{
		FW( pPlaneMem, 0, pEdge->fA);
		FW( pPlaneMem, 1, pEdge->fB);
		FW( pPlaneMem, 2, pEdge->fC);
		IW( pPlaneMem, 3, (forw_perp | (DEBUG_PERP_TAG << 4)));
	
		/* Next Edge if you please */
		pPlaneMem += WORDS_PER_PLANE;
		pEdge++;
	}
	while ( --nEdge != 0 );
	
	/* Update pEdge for next time */
	*ppEdge = pEdge;
	
	return (pPlaneMem);
}

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges and call PackISPPoly to pack
** extra edges
** Also conditionally adds an extra face if there is vertex fog
** on the triangle
**
**********************************************************************/

void PackISPPolygonCoreExtra(sgl_uint32 *pPlaneMem, 
							 sgl_uint32 nPolysInChunk,
							 sgl_uint32 *rTSPAddr, 
							 PITRI *rpTri, 
							 int nIncrement,
							 PIMATERIAL  *rpMat,
					  		 int         nMatInc,
							 sgl_uint32 TSPIncrement,
							 PIEDGE *ppEdge, 
							 int nExtraVertices,
							 sgl_uint32  *rTSPOffsetAddr,
							 sgl_uint32  TSPIncrementExtra)
{
	float 	fA, fB, fC;
	float 	fAe, fBe, fCe;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	PIMATERIAL	pMat = *rpMat;
	sgl_uint32	TSPAddr = *rTSPAddr;
	sgl_uint32	TSPOffsetAddr = *rTSPOffsetAddr;
	PFOGHIGHLIGHT epExtras = eExtraStatus;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		FW( pPlaneMem, 0+0, fA);

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		FW( pPlaneMem, 1+0, fB);

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
		{
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
			fC = 0.0f;
		}
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			fA = 0.0f;
			fB = 0.0f;
			fC = fZ2;
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fAe = pTri->fAdjoint[0][2];
		FW( pPlaneMem, 0+4, fAe);
		fBe = pTri->fAdjoint[1][2];
		FW( pPlaneMem, 1+4, fBe);
		fCe = pTri->fAdjoint[2][2];
		FW( pPlaneMem, 2+4, fCe);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane
		 */
		fAe = pTri->fAdjoint[0][0];
		FW( pPlaneMem, 0+8, fAe);
		fBe = pTri->fAdjoint[1][0];
		FW( pPlaneMem, 1+8, fBe);
		fCe = pTri->fAdjoint[2][0];
		FW( pPlaneMem, 2+8, fCe);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* Do the third and any remaining edge planes from *ppEdge */
		pPlaneMem = PackISPPoly( pPlaneMem + (WORDS_PER_PLANE*3),
												ppEdge, nExtraVertices );
		
		TSPAddr += TSPIncrement;

		/* if the smooth specular plane is added ... */
		if(pMat->nSmoothSpecularIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras = GOURAUD_HIGHLIGHT;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}
		else
		{
			/* no extra plane */
			*epExtras = NO_FOG_AND_HIGHLIGHT;
		}

		/* if the fog plane is added ... */
		if(pMat->nFogIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras |= VERTEX_FOG;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}

		*((sgl_uint32 *) &pTri) += nIncrement;
		*((sgl_uint32 *) &pMat) += nMatInc;
		epExtras++;
	}

	*rpTri = pTri;
	*rpMat = pMat;
	*rTSPAddr = TSPAddr;
	*rTSPOffsetAddr = TSPOffsetAddr;
} /* PackISPPolygonCoreExtra */

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the extra edges of a polygon
** This routine is similar to the above except it also checks the Edges to make
** sure they don't go out of range
**
**********************************************************************/

INLINE sgl_uint32 *PackISPPolyClipTested(sgl_uint32 *pPlaneMem, PIEDGE *ppEdge, int nExtraVertices )
{
	int 	nEdge = nExtraVertices + 1;
	PIEDGE	pEdge = *ppEdge;

	do
	{
		/* Check the range
		 */
		if(	sfabs((2*INVMAGIC) * pEdge->fC + pEdge->fA + pEdge->fB) >
			(sfabs(pEdge->fA) + sfabs(pEdge->fB)))
		{
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
		}
		else
		{
			FW( pPlaneMem, 0, pEdge->fA);
			FW( pPlaneMem, 1, pEdge->fB);
		}
		FW( pPlaneMem, 2, pEdge->fC);
		IW( pPlaneMem, 3, (forw_perp | (DEBUG_PERP_TAG << 4)));
	
		/* Next Edge if you please */
		pPlaneMem += WORDS_PER_PLANE;
		pEdge++;
	}
	while ( --nEdge != 0 );
	
	/* Update pEdge for next time */
	*ppEdge = pEdge;
	
	return (pPlaneMem);
}

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges and call PackISPPoly to pack
** extra edges
** This routine is similar to the above except it also checks the Edges to make
** sure they don't go out of range
** Also conditionally adds an extra face if there is vertex fog
** on the triangle
**
**********************************************************************/

void PackISPPolygonCoreClipTestedExtra(sgl_uint32 *pPlaneMem,
				 					   sgl_uint32 nPolysInChunk,
									   sgl_uint32 *rTSPAddr, 
									   PITRI *rpTri, 
									   int nIncrement,
									   PIMATERIAL  *rpMat,
					  				   int         nMatInc, 
									   sgl_uint32 TSPIncrement,
									   PIEDGE *ppEdge, 
									   int nExtraVertices,
									   sgl_uint32  *rTSPOffsetAddr,
					  				   sgl_uint32  TSPIncrementExtra)
{
	float 	fA, fB, fC;
	float 	fAe, fBe, fCe;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	PIMATERIAL	pMat = *rpMat;
	sgl_uint32	TSPAddr = *rTSPAddr;
	sgl_uint32	TSPOffsetAddr = *rTSPOffsetAddr;
	PFOGHIGHLIGHT epExtras = eExtraStatus;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		FW( pPlaneMem, 0+0, fA);

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		FW( pPlaneMem, 1+0, fB);

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
		{
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
			fC = 0.0f;
		}
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			fA = 0.0f;
			fB = 0.0f;
			fC = fZ2;
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fAe = pTri->fAdjoint[0][2];
		fBe = pTri->fAdjoint[1][2];
		fCe = pTri->fAdjoint[2][2];
		/* Check the range
		 */
		if(sfabs((2*INVMAGIC) * fCe + fAe + fBe) > (sfabs(fAe) + sfabs(fBe)))
		{
			fAe = 0.0f;
			fBe = 0.0f;
		}
		FW( pPlaneMem, 0+4, fAe);
		FW( pPlaneMem, 1+4, fBe);
		FW( pPlaneMem, 2+4, fCe);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane
		 */
		fAe = pTri->fAdjoint[0][0];
		fBe = pTri->fAdjoint[1][0];
		fCe = pTri->fAdjoint[2][0];
		/* Check the range
		 */
		if(sfabs((2*INVMAGIC) * fCe + fAe + fBe) > (sfabs(fAe) + sfabs(fBe)))
		{
			fAe = 0.0f;
			fBe = 0.0f;
		}
		FW( pPlaneMem, 0+8, fAe);
		FW( pPlaneMem, 1+8, fBe);
		FW( pPlaneMem, 2+8, fCe);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* Do the third and any remaining edge planes from *ppEdge */
		pPlaneMem = PackISPPolyClipTested( pPlaneMem + (WORDS_PER_PLANE*3),
												ppEdge, nExtraVertices );
		
		TSPAddr += TSPIncrement;

		/* if the smooth specular plane is added ... */
		if(pMat->nSmoothSpecularIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras = GOURAUD_HIGHLIGHT;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}
		else
		{
			/* no extra plane */
			*epExtras = NO_FOG_AND_HIGHLIGHT;
		}

		/* if the fog plane is added ... */
		if(pMat->nFogIndex)
		{
			/* we saved the values that were used for the front plane */
			FW( pPlaneMem, 0+0, fA);
			FW( pPlaneMem, 1+0, fB);
			FW( pPlaneMem, 2+0, fC);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

			*epExtras |= VERTEX_FOG;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}

		*((sgl_uint32 *) &pTri) += nIncrement;
		*((sgl_uint32 *) &pMat) += nMatInc;

		epExtras++;
	}

	*rpTri = pTri;
	*rpMat = pMat;
	*rTSPAddr = TSPAddr;
	*rTSPOffsetAddr = TSPOffsetAddr;
} /* PackISPPolygonCoreClipTestedExtra */


/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges and call PackISPPoly to pack
** extra edges
**
**********************************************************************/

void PackISPPolygonCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk,
	sgl_uint32 *rTSPAddr, PITRI *rpTri, int nIncrement, sgl_uint32 TSPIncrement,
										PIEDGE *ppEdge, int nExtraVertices)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		FW( pPlaneMem, 0+0, fA);

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		FW( pPlaneMem, 1+0, fB);

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		FW( pPlaneMem, 0+4, fA);
		fB = pTri->fAdjoint[1][2];
		FW( pPlaneMem, 1+4, fB);
		fC = pTri->fAdjoint[2][2];
		FW( pPlaneMem, 2+4, fC);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane
		 */
		fA = pTri->fAdjoint[0][0];
		FW( pPlaneMem, 0+8, fA);
		fB = pTri->fAdjoint[1][0];
		FW( pPlaneMem, 1+8, fB);
		fC = pTri->fAdjoint[2][0];
		FW( pPlaneMem, 2+8, fC);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* Do the third and any remaining edge planes from *ppEdge */
		pPlaneMem = PackISPPoly( pPlaneMem + (WORDS_PER_PLANE*3),
												ppEdge, nExtraVertices );
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
} /* PackISPPolygonCore */

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** No assembler equivalent
** Packs the front face and three edges and call PackISPPoly to pack
** extra edges
** This routine is similar to the above except it also checks the Edges to make
** sure they don't go out of range
**
**********************************************************************/

void PackISPPolygonCoreClipTested(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk,
	sgl_uint32 *rTSPAddr, PITRI *rpTri, int nIncrement, sgl_uint32 TSPIncrement,
										PIEDGE *ppEdge, int nExtraVertices)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];
		FW( pPlaneMem, 0+0, fA);

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];
		FW( pPlaneMem, 1+0, fB);

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2 + gfDepthBias;

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
#if PCX2
		if (sfabs(fC) <= MAGIC_FLOAT_FIX)
			/* If problem set C to 0.
		     */
			IW( pPlaneMem, 2+0, 0);
		else
#endif
			/* Continue as normal.
			 */
			FW( pPlaneMem, 2+0, fC);

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			IW( pPlaneMem, 0+0, 0); 
			IW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ2);
		}

		/* Pack instruction and TSP address.
		 */
		IW( pPlaneMem, 3+0, (forw_visib_fp | (TSPAddr << 4)));

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];
		/* Check the range
		 */
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			fA = 0.0f;
			fB = 0.0f;
		}
		FW( pPlaneMem, 0+4, fA);
		FW( pPlaneMem, 1+4, fB);
		FW( pPlaneMem, 2+4, fC);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* do second edge plane
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		/* Check the range
		 */
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			fA = 0.0f;
			fB = 0.0f;
		}
		FW( pPlaneMem, 0+8, fA);
		FW( pPlaneMem, 1+8, fB);
		FW( pPlaneMem, 2+8, fC);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* Do the third and any remaining edge planes from *ppEdge */
		pPlaneMem = PackISPPolyClipTested( pPlaneMem + (WORDS_PER_PLANE*3),
												ppEdge, nExtraVertices );
	   
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
} /* PackISPPolygonCoreClipTested */

#else	/* #if PCX2	*/

#if PACKISPCORECLIPTESTED


/**********************************************************************
**
** Hardware: PCX1
** No Assembler Equivalent in dispml.asm
** Packs the front face and three edges
** This routine also checks the Edges to make
** sure they don't go out of range
**
**********************************************************************/

void PackISPCoreClipTested(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, sgl_uint32 *rTSPAddr, PITRI *rpTri,
															int nIncrement, sgl_uint32 TSPIncrement)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	float 	fInvMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2;

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[1+0] = (TSPAddr << 20) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[2+0] = (sgl_int32) (fZ2 * FLOAT_TO_FIXED);
		}
		else
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				InPackTo20Bit (fA);
			pPlaneMem[1+0] = (TSPAddr << 20) |
				InPackTo20Bit (fB);
			pPlaneMem[2+0] = (sgl_int32) (fC * FLOAT_TO_FIXED);
		}

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];
		/* 
		// HMM the following IS NOT a very efficient arrangement!!!!
		//
		// Check range does not go out of bounds
		*/
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			pPlaneMem[0+3] = forw_perp |
							 ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12));
			pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20);


			/* Not used fInvMaxVal = 1.0f;	*/

			if(fC < 0.0f)
			{
				pPlaneMem[2+3] = 0xCE800000; /* (-0.5f * FLOAT_TO_FIXED) */
			}
			else
			{
				pPlaneMem[2+3] = 0x4E800000; /* (0.5f * FLOAT_TO_FIXED) */
			}
		}
		else
		{
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+3] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
				InPackTo20Bit (fA * fInvMaxVal);
			pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20) |
				InPackTo20Bit (fB * fInvMaxVal);
			pPlaneMem[2+3] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		}
		
		/* do second edge plane 
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		/* 
		// HMM the following IS NOT a very efficient arrangement!!!!
		//
		// Check range does not go out of bounds
		*/
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			pPlaneMem[0+6] = forw_perp |
							 ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12));
			pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20);

			/* Not used fInvMaxVal = 1.0f;	*/

			if(fC < 0.0f)
			{
				pPlaneMem[2+6] = 0xCE800000; /* (-0.5f * FLOAT_TO_FIXED) */
			}
			else
			{
				pPlaneMem[2+6] = 0x4E800000; /* (0.5f * FLOAT_TO_FIXED) */
			}
		}
		else
		{
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+6] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
				InPackTo20Bit (fA * fInvMaxVal);
			pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20) |
				InPackTo20Bit (fB * fInvMaxVal);
			pPlaneMem[2+6] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		}

		/* do third edge plane
		 */
		fA = pTri->fAdjoint[0][1];
		fB = pTri->fAdjoint[1][1];
		fC = pTri->fAdjoint[2][1];
		/* 
		// HMM the following IS NOT a very efficient arrangement!!!!
		//
		// Check range does not go out of bounds
		*/
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			pPlaneMem[0+9] = forw_perp |
							 ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12));
			pPlaneMem[1+9] = (DEBUG_PERP_TAG << 20);

			/* Not used fInvMaxVal = 1.0f;	*/

			if(fC < 0.0f)
			{
				pPlaneMem[2+9] = 0xCE800000; /* (-0.5f * FLOAT_TO_FIXED) */
			}
			else
			{
				pPlaneMem[2+9] = 0x4E800000; /* (0.5f * FLOAT_TO_FIXED) */
			}
		}
		else
		{
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+9] = forw_perp |
							 ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
							 InPackTo20Bit (fA * fInvMaxVal);
			pPlaneMem[1+9] = (DEBUG_PERP_TAG << 20) |
							 InPackTo20Bit (fB * fInvMaxVal);
			pPlaneMem[2+9] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);

		}
		
		pPlaneMem += WORDS_PER_PLANE * 4;
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
}

#else
#define PackISPCoreClipTested _PackISPCoreClipTested


/**********************************************************************
**
** Would reference the assembler varaint of above if it existed,
** but it doesn't
**
**********************************************************************/

extern void PackISPCoreClipTested(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, sgl_uint32 *rTSPAddr, PITRI *rpTri,
															int nIncrement, sgl_uint32 TSPIncrement);
#endif

#if PACKISPCORE


/**********************************************************************
**
** Hardware: PCX1
** Has Assembler Equivalent in dispml.asm
** Packs the front face and three edges
**
**********************************************************************/

void PackISPCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, sgl_uint32 *rTSPAddr, PITRI *rpTri,
															int nIncrement, sgl_uint32 TSPIncrement)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	float 	fInvMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2;

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[1+0] = (TSPAddr << 20) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[2+0] = (sgl_int32) (fZ2 * FLOAT_TO_FIXED);
		}
		else
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				InPackTo20Bit (fA);
			pPlaneMem[1+0] = (TSPAddr << 20) |
				InPackTo20Bit (fB);
			pPlaneMem[2+0] = (sgl_int32) (fC * FLOAT_TO_FIXED);
		}


		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];
		fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

		pPlaneMem[0+3] = forw_perp |
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
			InPackTo20Bit (fA * fInvMaxVal);
		pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20) |
			InPackTo20Bit (fB * fInvMaxVal);
		pPlaneMem[2+3] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		
		/* do second edge plane 
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));
		
		pPlaneMem[0+6] = forw_perp |
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
			InPackTo20Bit (fA * fInvMaxVal);
		pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20) |
			InPackTo20Bit (fB * fInvMaxVal);
		pPlaneMem[2+6] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);

		/* do third edge plane
		 */
		fA = pTri->fAdjoint[0][1];
		fB = pTri->fAdjoint[1][1];
		fC = pTri->fAdjoint[2][1];
		fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

		pPlaneMem[0+9] = forw_perp |
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
			InPackTo20Bit (fA * fInvMaxVal);
		pPlaneMem[1+9] = (DEBUG_PERP_TAG << 20) |
			InPackTo20Bit (fB * fInvMaxVal);
		pPlaneMem[2+9] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		
		pPlaneMem += WORDS_PER_PLANE * 4;
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
}
#else
#define PackISPCore _PackISPCore


/**********************************************************************
**
** Hardware: PCX1
** Function Prototype for Assembler Equivalent in dispml.asm
**
**********************************************************************/

extern void PackISPCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, sgl_uint32 *rTSPAddr, PITRI *rpTri,
															int nIncrement, sgl_uint32 TSPIncrement);
#endif

#if PACKISPPOLYGONCORE

/**********************************************************************
**
** Hardware: PCX1
** Assembler equivalent is included in PackISPPolygonCore in dispml.asm
** Packs extra edges for polygons
**
**********************************************************************/

INLINE sgl_uint32 *PackISPPoly(sgl_uint32 *pPlaneMem, PIEDGE *ppEdge, int nExtraVertices )
{
	int 	nEdge = nExtraVertices + 1;
	PIEDGE	pEdge = *ppEdge;
	float 	fInvMaxVal;

	do
	{
			
		/* Do the third and any extra edges
		 */
		fInvMaxVal =
		  ApproxRecip ((2 * MAGIC * (sfabs (pEdge->fA) + sfabs (pEdge->fB))) + sfabs (pEdge->fC));

		pPlaneMem[0] = forw_perp |
								((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
								InPackTo20Bit (pEdge->fA * fInvMaxVal);
		pPlaneMem[1] = (DEBUG_PERP_TAG << 20) |
								InPackTo20Bit (pEdge->fB * fInvMaxVal);
		pPlaneMem[2] = (sgl_int32) (pEdge->fC * fInvMaxVal * FLOAT_TO_FIXED);
		pPlaneMem[3] = forw_perp | (DEBUG_PERP_TAG << 4);
	
		/* Next Edge if you please */
		pPlaneMem += WORDS_PER_PLANE;
		pEdge++;
	}
	while ( --nEdge != 0 );
	
	/* Update pEdge for next time */
	*ppEdge = pEdge;
	
	return (pPlaneMem);
}


/**********************************************************************
**
** Hardware: PCX1
** Has Assembler equivalent in dispml.asm
** Packs the front face and three edges and call PackISPPoly to pack
** extra edges
**
**********************************************************************/


void PackISPPolygonCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk,
	sgl_uint32 *rTSPAddr, PITRI *rpTri, int nIncrement, sgl_uint32 TSPIncrement,
										PIEDGE *ppEdge, int nExtraVertices)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	float 	fInvMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2;

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[1+0] = (TSPAddr << 20) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[2+0] = (sgl_int32) (fZ2 * FLOAT_TO_FIXED);
		}
		else
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				InPackTo20Bit (fA);
			pPlaneMem[1+0] = (TSPAddr << 20) |
				InPackTo20Bit (fB);
			pPlaneMem[2+0] = (sgl_int32) (fC * FLOAT_TO_FIXED);
		}

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];

		fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));
		
		pPlaneMem[0+3] = forw_perp |
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
			InPackTo20Bit (fA * fInvMaxVal);
		pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20) |
			InPackTo20Bit (fB * fInvMaxVal);
		pPlaneMem[2+3] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		
		/* do second edge plane
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		
		fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));
		
		pPlaneMem[0+6] = forw_perp |
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
			InPackTo20Bit (fA * fInvMaxVal);
		pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20) |
			InPackTo20Bit (fB * fInvMaxVal);
		pPlaneMem[2+6] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		
		/* Do the third and any remaining edge planes from *ppEdge */
		pPlaneMem = PackISPPoly( pPlaneMem + (WORDS_PER_PLANE*3),
												ppEdge, nExtraVertices );
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
} /* PackISPPolygonCore */

#else
#define PackISPPolygonCore _PackISPPolygonCore


/**********************************************************************
**
** Hardware: PCX1
** Prototype for Assembler equivalent in dispml.asm
**
**********************************************************************/

extern void PackISPPolygonCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk,
		sgl_uint32 *rTSPAddr, PITRI *rpTri, int nIncrement, sgl_uint32 TSPIncrement,
											PIEDGE *ppEdge, int nExtraVertices);
#endif /* #if PACKISPPOLYGONCORE */

#if PACKISPPOLYGONCORECLIPTESTED

/**********************************************************************
**
**
**
**
**
**
**********************************************************************/

INLINE sgl_uint32 *PackISPPolyClipTested(sgl_uint32 *pPlaneMem, PIEDGE *ppEdge, int nExtraVertices )
{
	int 	nEdge = nExtraVertices + 1;
	PIEDGE	pEdge = *ppEdge;
	float 	fInvMaxVal;

	do
	{
		/* Do the third and any extra edges
		 */
			
		/* Change the range.
		 */
		if(	sfabs((2*INVMAGIC) * pEdge->fC + pEdge->fA + pEdge->fB) >
			(sfabs(pEdge->fA) + sfabs(pEdge->fB)))
		{
			pPlaneMem[0] = forw_perp | ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12));
			pPlaneMem[1] = (DEBUG_PERP_TAG << 20);
			pPlaneMem[3] = forw_perp | (DEBUG_PERP_TAG << 4);

			/* Not used fInvMaxVal = 1.0f;	*/

			if(pEdge->fC < 0.0f)
			{
				pPlaneMem[2] = 0xCE800000; /* (-0.5f * FLOAT_TO_FIXED) */
			}
			else
			{
				pPlaneMem[2] = 0x4E800000; /* (0.5f * FLOAT_TO_FIXED) */
			}
		}
		else
		{
			fInvMaxVal =
			  ApproxRecip ((2 * MAGIC * (sfabs (pEdge->fA) + sfabs (pEdge->fB))) + sfabs (pEdge->fC));

			pPlaneMem[0] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
				InPackTo20Bit (pEdge->fA * fInvMaxVal);
			pPlaneMem[1] = (DEBUG_PERP_TAG << 20) |
				InPackTo20Bit (pEdge->fB * fInvMaxVal);
			pPlaneMem[2] = (sgl_int32) (pEdge->fC * fInvMaxVal * FLOAT_TO_FIXED);
			pPlaneMem[3] = forw_perp | (DEBUG_PERP_TAG << 4);
		}

		/* Next Edge if you please */
		pPlaneMem += WORDS_PER_PLANE;
		pEdge++;
	}
	while ( --nEdge != 0 );
	
	/* Update pEdge for next time */
	*ppEdge = pEdge;
	
	return (pPlaneMem);
}


/**********************************************************************
**
**
**
**
**
**
**********************************************************************/


void PackISPPolygonCoreClipTested(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk,
	sgl_uint32 *rTSPAddr, PITRI *rpTri, int nIncrement, sgl_uint32 TSPIncrement,
										PIEDGE *ppEdge, int nExtraVertices)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	float 	fInvMaxVal;
	PITRI	pTri = *rpTri;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while (nPolysInChunk--)
	{
		fZ2 = pTri->fZ[2];
				
		f1OverDet = pTri->f1OverDet;

		fZ0 = (pTri->fZ[0] - fZ2) * f1OverDet;
		fZ1 = (pTri->fZ[1] - fZ2) * f1OverDet;

		/*
		// Compute initial A, B and C values before scaling, using the projected
		// vertex positions:
		//
		// NOTE: I have changed the calculation to use a different basis system
		// which means that the new "adjoint" which actually re-uses values from
		// the adjoint we pass in except for the last row which is 0,0, Det.
		//									    Simon.
		*/
		fA = fZ0*pTri->fAdjoint[0][0] +  fZ1*pTri->fAdjoint[0][1];

		fB = fZ0*pTri->fAdjoint[1][0] +  fZ1*pTri->fAdjoint[1][1];

		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2;

		/*
		// to prevent the 20Bit SFLOATS from overflowing (currently they are 2x
		// larger than I originally thought necessary)
		*/
		fMaxVal = sfabs (fA + fB + (fC * INVMAGIC)) + (sfabs (fA) + sfabs (fB));

		/*
		// Once again, this float value is always positive. On Pentium, the
		// float compare is awful, so do it in INT instead.
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(fMaxVal) > LONG_INVMAGIC )
	#else
		if (fMaxVal > INVMAGIC)
	#endif
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[1+0] = (TSPAddr << 20) |
				SFLOAT_20BIT_ZERO;
			pPlaneMem[2+0] = (sgl_int32) (fZ2 * FLOAT_TO_FIXED);
			
		}
		else
		{
			pPlaneMem[0+0] = forw_visib_fp |
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |
				InPackTo20Bit (fA);
			pPlaneMem[1+0] = (TSPAddr << 20) |
				InPackTo20Bit (fB);
			pPlaneMem[2+0] = (sgl_int32) (fC * FLOAT_TO_FIXED);
		}

		/* do first edge plane
		 */
		fA = pTri->fAdjoint[0][2];
		fB = pTri->fAdjoint[1][2];
		fC = pTri->fAdjoint[2][2];

		/* Change the range.
		 */
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			pPlaneMem[0+3] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12));
			pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20);

			/* Not used fInvMaxVal = 1.0f;	*/

			if(fC < 0.0f)
			{
				pPlaneMem[2+3] = 0xCE800000; /* (-0.5f * FLOAT_TO_FIXED) */
			}
			else
			{
				pPlaneMem[2+3] = 0x4E800000; /* (0.5f * FLOAT_TO_FIXED) */
			}
		}
		else
		{
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+3] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
				InPackTo20Bit (fA * fInvMaxVal);
			pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20) |
				InPackTo20Bit (fB * fInvMaxVal);
			pPlaneMem[2+3] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);
		}

		/* do second edge plane
		 */
		fA = pTri->fAdjoint[0][0];
		fB = pTri->fAdjoint[1][0];
		fC = pTri->fAdjoint[2][0];
		
		/* Change the range.
		 */
		if(sfabs((2*INVMAGIC) * fC + fA + fB) > (sfabs(fA) + sfabs(fB)))
		{
			pPlaneMem[0+6] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12));
			pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20);

			/* Not used fInvMaxVal = 1.0f;	*/

			if(fC < 0.0f)
			{
				pPlaneMem[2+6] = 0xCE800000; /* (-0.5f * FLOAT_TO_FIXED) */
			}
			else
			{
				pPlaneMem[2+6] = 0x4E800000; /* (0.5f * FLOAT_TO_FIXED) */
			}
		}
		else
		{
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+6] = forw_perp |
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
				InPackTo20Bit (fA * fInvMaxVal);
			pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20) |
				InPackTo20Bit (fB * fInvMaxVal);
			pPlaneMem[2+6] = (sgl_int32) (fC * fInvMaxVal * FLOAT_TO_FIXED);

		}

		/* Do the third and any remaining edge planes from *ppEdge */
		pPlaneMem = PackISPPolyClipTested( pPlaneMem + (WORDS_PER_PLANE*3),
												ppEdge, nExtraVertices );
		
		*((sgl_uint32 *) &pTri) += nIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
} /* PackISPPolygonCoreClipTested */

#else
#define PackISPPolygonCoreClipTested _PackISPPolygonCoreClipTested

extern void PackISPPolygonCoreClipTested(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk,
		sgl_uint32 *rTSPAddr, PITRI *rpTri, int nIncrement, sgl_uint32 TSPIncrement,
											PIEDGE *ppEdge, int nExtraVertices);
#endif /* #if PACKISPPOLYGONCORECLIPTESTED */

#endif	/* #if PCX2	*/


/**********************************************************************
**
** Hardware: PCX1, PCX2, PCX2_003
**
**
**
**
**********************************************************************/

int PackISPTri (PITRI pTri, int nTriangles, sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nPolysInChunk;
	int 	nPolys;
	
	SGL_TIME_START(PACK_ISPTRI_TIME)

	nPolys = nTriangles;
				
	if (gPDC.TSPControlWord & MASK_TRANS)
	{
		GenerateDepthInfo (pTri, nPolys);
	}

	while (nPolys)
	{
		CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, WORDS_PER_PLANE * 4);

		if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
		{
			break;
		}
		else
		{
			DataSize = WORDS_PER_PLANE * 4 * nPolys;

			ChunkLimit = GetSabreLimit (CurrentPos);
			ChunkSize = ChunkLimit - CurrentPos;

			if (DataSize < ChunkSize)
			{
				nPolysInChunk = nPolys;
			}
			else
			{
				nPolysInChunk = ChunkSize / (WORDS_PER_PLANE * 4);
				
				if (!nPolysInChunk)
				{
					break;
				}
				/* Recalculate the DataSize for the new number of polys */
				DataSize = WORDS_PER_PLANE * 4 * nPolysInChunk;
			}
			
			AddRegionObjects((sgl_uint32 *) &(pTri->reg), sizeof (ITRI),
								  4, nPolysInChunk, CurrentPos, gDepthInfo, sizeof(TRANS_REGION_DEPTHS_STRUCT));

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			nPolys -= nPolysInChunk;
			
			SGL_TIME_START(PACK_ISPCORE_TIME)

			/*
			// If we aren't guaranteed that vertices are on screen, then
			// use the safer pack routine
			*/
			if(gPDC.Context.bDoClipping)
			{
				PackISPCoreClipTested( &(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
							nPolysInChunk, &TSPAddr, &pTri,
							sizeof (ITRI), TSPIncrement );
			}
			else
			{
				PackISPCore( &(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
							nPolysInChunk, &TSPAddr, &pTri,
							sizeof (ITRI), TSPIncrement );
			}
			SGL_TIME_STOP(PACK_ISPCORE_TIME)
		}
	}
	
	if (nPolys)
	{
		DPFDEV ((DBG_WARNING, "PackISPTri: Out of ISP buffer space"));
	}

	SGL_TIME_STOP(PACK_ISPTRI_TIME)

	return (nTriangles - nPolys);
}

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** Calls the core packing function, adds the objects to regions,
** and does the extra stuff needed for vertex fog and gouraud specular
**
**********************************************************************/

int PackISPTriExtra (PITRI rpTri, PIMATERIAL rpMat, 
					 int nTriangles, 
					 sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nPolysInChunk;
	const sgl_uint32  TSPIncrementExtra = 8>>1; /* both the VFog and specular are flat textured */
	int 	nPolys, i, nObjPlanesInWords;
	int TSPSize = 0;
	sgl_uint32 TSPOffsetAddr;
	PIMATERIAL pMat = rpMat;
	PITRI pTri = rpTri;
	sgl_uint32 DataInc;

	SGL_TIME_START(PACK_ISPTRI_TIME)

	nPolys = nTriangles;
				
	TSPOffsetAddr = TSPAddr + TSPIncrement*nPolys;

	if (gPDC.TSPControlWord & MASK_TRANS)
	{
		GenerateDepthInfo (pTri, nPolys);
	}

	while (nPolys)
	{
		nObjPlanesInWords = WORDS_PER_PLANE * 4 + 
			((rpMat->nFogIndex)?WORDS_PER_PLANE:0) +
			((rpMat->nSmoothSpecularIndex)?WORDS_PER_PLANE:0);

		CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, nObjPlanesInWords);

		if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
		{
			break;
		}
		else
		{
			DataInc = 0;
			DataSize = WORDS_PER_PLANE * 4 * nPolys;
			TSPSize += nPolys * TSPIncrement;
			pMat = rpMat; /* points to unprocessed triangle materials */
			for(i=0; i<nPolys; i++, pMat++)
			{
				DataInc += ((pMat->nFogIndex)?1:0);
				DataInc += ((pMat->nSmoothSpecularIndex)?1:0);
			}
			TSPSize += (TSPIncrementExtra * DataInc);
			DataSize += (WORDS_PER_PLANE * DataInc);

			ChunkLimit = GetSabreLimit (CurrentPos);
			ChunkSize = ChunkLimit - CurrentPos;

			/* see if we can fit all the polys in this chunk... */
			if (DataSize < ChunkSize)
			{
				nPolysInChunk = nPolys;
			}
			else
			{
				/* ...we cant, so see how many will go in */ 
				DataSize=0;
				DataInc=0;
				pMat = rpMat; /* points to unprocessed triangle materials */
				for(nPolysInChunk=0; ChunkSize>(WORDS_PER_PLANE * 4); 
					nPolysInChunk++, DataSize+=DataInc, pMat++)
				{
					DataInc = WORDS_PER_PLANE * 4; 
					DataInc += ((pMat->nFogIndex)?WORDS_PER_PLANE:0);
					DataInc += ((pMat->nSmoothSpecularIndex)?WORDS_PER_PLANE:0);
					ChunkSize -= DataInc;
					/*nPolysInChunk = ChunkSize / (WORDS_PER_PLANE * 4);*/

				}
				
				if (!nPolysInChunk)
				{
					break;
				}

			}
			

			nPolys -= nPolysInChunk;
			
			SGL_TIME_START(PACK_ISPCORE_TIME);

			/* require a safe copy of the pointer to the current triangle
			** for adding regions, as isp packing updates the pTri pointer
			*/
			pTri = rpTri;

			/*
			// If we aren't guaranteed that vertices are on screen, then
			// use the safer pack routine
			*/
			if(gPDC.Context.bDoClipping)
			{
				PackISPCoreClipTestedExtra(&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
										   nPolysInChunk, &TSPAddr, 
										   &rpTri, sizeof (ITRI), 
										   &rpMat, sizeof (IMATERIAL), 
										   TSPIncrement, 
										   &TSPOffsetAddr, TSPIncrementExtra );
			}
			else
			{
				PackISPCoreExtra(&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
								 nPolysInChunk, &TSPAddr, 
								 &rpTri, sizeof (ITRI), 
								 &rpMat, sizeof (IMATERIAL), 
								 TSPIncrement, 
								 &TSPOffsetAddr, TSPIncrementExtra );
			}

			/* add the objects after the extra status has been calculated */
			AddRegionObjectsExtra((sgl_uint32 *) &(pTri->reg), sizeof (ITRI),
								  4, nPolysInChunk, CurrentPos, 
								  gDepthInfo, sizeof(TRANS_REGION_DEPTHS_STRUCT),
								  eExtraStatus);

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			SGL_TIME_STOP(PACK_ISPCORE_TIME);
				

		}
	}
	
	if (nPolys)
	{
		DPFDEV ((DBG_WARNING, "PackISPTriExtra: Out of ISP buffer space"));
	}

	SGL_TIME_STOP(PACK_ISPTRI_TIME)

	return (TSPSize);
}

/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** Calls the core packing function, adds the objects to regions,
** and does the extra stuff needed for vertex fog and gouraud specular
**
**********************************************************************/
int PackISPPolygonExtra (PITRI rpTri, PIMATERIAL rpMat, PIEDGE pEdge, int nPolygons,
					sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	DataSize, CurrentPos, ChunkSize, ChunkLimit, nPolysInChunk;
	int 	nPolys, nExtraVertices, nPlanePolys, nBurst, i, nObjPlanesInWords;
	const sgl_uint32  TSPIncrementExtra = 8>>1; /* both the VFog and specular are flat textured */
	int TSPSize = 0;
	sgl_uint32 TSPOffsetAddr;
	PIMATERIAL pMat = rpMat;
	PITRI pTri = rpTri;
	sgl_uint32 DataInc;


	SGL_TIME_START(PACK_ISPTRI_TIME)

	nPolys = nPolygons;

	TSPOffsetAddr = TSPAddr + TSPIncrement*nPolys;

	if (gPDC.TSPControlWord & MASK_TRANS)
	{
		GenerateDepthInfo (pTri, nPolys);
	}

	nPlanePolys = nPolys;

	while (nPolys)
	{
		nExtraVertices = pMat->NumExtraVertices;

		/* count the burst of polys of same size */
		
		do
		{
			--nPlanePolys;
			++pMat;
		}
		while ((pMat->NumExtraVertices == (sgl_uint32) nExtraVertices) && nPlanePolys);

		nBurst = nPolys - nPlanePolys;
		
		/* iterate through the burst */
		
		while (nBurst)
		{
			nObjPlanesInWords = WORDS_PER_PLANE * (4 + nExtraVertices) + 
				((rpMat->nFogIndex)?WORDS_PER_PLANE:0) +
				((rpMat->nSmoothSpecularIndex)?WORDS_PER_PLANE:0);

			CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, nObjPlanesInWords);

			if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
			{
				SGL_TIME_STOP(PACK_ISPTRI_TIME)

				return (nPolygons - nPolys);
			}
			else
			{
				DataInc = 0;
				DataSize = nBurst * (4 + nExtraVertices) * WORDS_PER_PLANE;
				TSPSize += nBurst * TSPIncrement;
				pMat = rpMat; /* points to unprocessed triangle materials */
				for(i=0; i<nBurst; i++, pMat++)
				{
					DataInc += ((pMat->nFogIndex)?1:0);
					DataInc += ((pMat->nSmoothSpecularIndex)?1:0);
				}
				TSPSize += (TSPIncrementExtra * DataInc);
				DataSize += (WORDS_PER_PLANE * DataInc);

				ChunkLimit = GetSabreLimit (CurrentPos);
				ChunkSize = ChunkLimit - CurrentPos;

				if (DataSize < ChunkSize)
				{
					nPolysInChunk = nBurst;
				}
				else
				{
					DataSize=0;
					DataInc=0;
					pMat = rpMat; /* points to unprocessed triangle materials */
					for(nPolysInChunk=0; ChunkSize>(WORDS_PER_PLANE * (4 + nExtraVertices)); 
						nPolysInChunk++, DataSize+=DataInc, pMat++)
					{
						DataInc = WORDS_PER_PLANE * (4 + nExtraVertices); 
						DataInc += ((pMat->nFogIndex)?WORDS_PER_PLANE:0);
						DataInc += ((pMat->nSmoothSpecularIndex)?WORDS_PER_PLANE:0);
						ChunkSize -= DataInc;
						/*nPolysInChunk = ChunkSize / (WORDS_PER_PLANE * (4 + nExtraVertices));*/

					}
					
					if (!nPolysInChunk)
					{
						SGL_TIME_STOP(PACK_ISPTRI_TIME)

						return (nPolygons - nPolys);
					}
				}				
				
				nBurst -= nPolysInChunk;

				SGL_TIME_START(PACK_ISPCORE_TIME);

				/* require a safe copy of the pointer to the current triangle
				** for adding regions, as isp packing updates the pTri pointer
				*/
				pTri = rpTri;
					
				/*
				// If we aren't guaranteed that vertices are on screen, then
				// use the safer pack routine
				*/
				if(gPDC.Context.bDoClipping)
				{
					if (nExtraVertices == 0)
					{
						PackISPCoreClipTestedExtra(&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
										   nPolysInChunk, &TSPAddr, 
										   &rpTri, sizeof (ITRI), 
										   &rpMat, sizeof (IMATERIAL), 
										   TSPIncrement, 
										   &TSPOffsetAddr, TSPIncrementExtra );
					}
					else
					{
					  	PackISPPolygonCoreClipTestedExtra (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
											nPolysInChunk, &TSPAddr, 
											&rpTri,	sizeof(ITRI), 
											&rpMat, sizeof (IMATERIAL),
											TSPIncrement,
											&pEdge, nExtraVertices,
											&TSPOffsetAddr, TSPIncrementExtra);	
					}
				}
				else
				{
					if (nExtraVertices == 0)
					{
						PackISPCoreExtra(&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
							   			 nPolysInChunk, &TSPAddr, 
										 &rpTri, sizeof (ITRI), 
										 &rpMat, sizeof (IMATERIAL), 
									 	 TSPIncrement, 
										 &TSPOffsetAddr, TSPIncrementExtra );
					}
					else
					{
					  	PackISPPolygonCoreExtra (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
										nPolysInChunk, &TSPAddr,
										&rpTri, sizeof(ITRI),
										&rpMat, sizeof (IMATERIAL),  
										TSPIncrement,
										&pEdge, nExtraVertices,
										&TSPOffsetAddr, TSPIncrementExtra );
					}
				}

				/* add the objects after the extra status has been calculated */
				AddRegionObjectsExtra((sgl_uint32 *) &(pTri->reg.u), sizeof (ITRI),
									  4 + nExtraVertices, nPolysInChunk, CurrentPos, 
									  gDepthInfo + (nPolygons - nPolys), 
									  sizeof(TRANS_REGION_DEPTHS_STRUCT),
									  eExtraStatus);
				
			  	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;			
							
				SGL_TIME_STOP(PACK_ISPCORE_TIME)
			}
			
		}

		nPolys = nPlanePolys;
	}
		
	if (nPolys)
	{
		DPFDEV ((DBG_WARNING, "PackISPPolygonExtra: Out of ISP buffer space"));		
	}

	SGL_TIME_STOP(PACK_ISPTRI_TIME)

	return (TSPSize);

} /* PackISPPolygonExtra */

#endif

/**********************************************************************
**
** Hardware: PCX1, PCX2, PCX2_003
**
**
**
**
**
**********************************************************************/

int PackISPPolygon (PITRI pTri, PIMATERIAL pMat, PIEDGE pEdge, int nPolygons,
					sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	DataSize, CurrentPos, ChunkSize, ChunkLimit;
	int 	nPolys, nPolysInChunk, nExtraVertices, nPlanePolys, nBurst;
	PIMATERIAL pThisMat = pMat;
	
	SGL_TIME_START(PACK_ISPTRI_TIME)

	nPolys = nPolygons;

	if (gPDC.TSPControlWord & MASK_TRANS)
	{
		GenerateDepthInfo (pTri, nPolys);
	}

	nPlanePolys = nPolys;

	while (nPolys)
	{
		nExtraVertices = pMat->NumExtraVertices;

		/* count the burst of polys of same size */
		
		do
		{
			--nPlanePolys;
			++pMat;
		}
		while ((pMat->NumExtraVertices == (sgl_uint32) nExtraVertices) && nPlanePolys);

		nBurst = nPolys - nPlanePolys;
		
		/* iterate through the burst */
		
		while (nBurst)
		{
			CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, WORDS_PER_PLANE * (4 + nExtraVertices));

			if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
			{
				SGL_TIME_STOP(PACK_ISPTRI_TIME)

				return (nPolygons - nPolys);
			}
			else
			{
				DataSize = nBurst * (4 + nExtraVertices) * WORDS_PER_PLANE;

				ChunkLimit = GetSabreLimit (CurrentPos);
				ChunkSize = ChunkLimit - CurrentPos;

				if (DataSize < ChunkSize)
				{
					nPolysInChunk = nBurst;
				}
				else
				{
					nPolysInChunk = ChunkSize / (WORDS_PER_PLANE * (4 + nExtraVertices));
					
					if (!nPolysInChunk)
					{
						SGL_TIME_STOP(PACK_ISPTRI_TIME)

						return (nPolygons - nPolys);
					}
					/* Recalculate the DataSize for the new number of polys */
					DataSize = (WORDS_PER_PLANE * (4 + nExtraVertices)) * nPolysInChunk;
				}

				PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + (nPolysInChunk * (4 + nExtraVertices) * WORDS_PER_PLANE);
							
				AddRegionObjects (&(pTri->reg.u), 
								  sizeof (ITRI),
				  				  4 + nExtraVertices, 
								  nPolysInChunk, 
								  CurrentPos, 
								  gDepthInfo + (nPolygons - nPolys), 
								  sizeof(TRANS_REGION_DEPTHS_STRUCT));

				SGL_TIME_START(PACK_ISPCORE_TIME)
					
				/*
				// If we aren't guaranteed that vertices are on screen, then
				// use the safer pack routine
				*/
				if(gPDC.Context.bDoClipping)
				{
					if (nExtraVertices == 0)
					{
						PackISPCoreClipTested (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
									 nPolysInChunk, 
									 &TSPAddr, &pTri,
									 sizeof (ITRI), TSPIncrement );
					}
					else
					{
						PackISPPolygonCoreClipTested (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
										nPolysInChunk,
										&TSPAddr, &pTri,
										sizeof(ITRI), TSPIncrement,
										&pEdge, nExtraVertices);
					}
				}
				else
				{
					if (nExtraVertices == 0)
					{
						PackISPCore (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
									 nPolysInChunk, 
									 &TSPAddr, &pTri,
									 sizeof (ITRI), TSPIncrement );
					}
					else
					{
						PackISPPolygonCore (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
										nPolysInChunk,
										&TSPAddr, &pTri,
										sizeof(ITRI), TSPIncrement,
										&pEdge, nExtraVertices);
					}
				}
							
				SGL_TIME_STOP(PACK_ISPCORE_TIME)
			}
			
			nBurst -= nPolysInChunk;
		}

		nPolys = nPlanePolys;
	}
		
	if (nPolys)
	{
		DPFDEV ((DBG_WARNING, "PackISPPolygon: Out of ISP buffer space"));
	}

	SGL_TIME_STOP(PACK_ISPTRI_TIME)

	return (nPolygons - nPolys);

} /* PackISPPolygon */
/* end of $RCSfile: disp.c,v $ */





							   
