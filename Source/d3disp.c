/******************************************************************************
 * Name         : d3disp.c
 * Title        : ISP plane packing routines for PowerSGL Direct
 * Author       : Michael Green
 * Created      : 30th August 1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : Supplement to disp.c for D3D translucent triangles
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: d3disp.c,v $
 * Revision 1.6  1997/10/16  12:57:01  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.5  1997/10/15  10:50:51  sxy
 * added some DPFDEVF messages.
 *
 * Revision 1.4  1997/09/24  14:13:30  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.3  1997/09/15  16:32:33  mjg
 * Added DAG_TRANS_SORTING compile switches.
 *
 * Revision 1.2  1997/09/03  11:30:58  dm
 * removed some ^M's and protected one func from pcx1 - but more
 * needs done
 *
 * Revision 1.1  1997/09/01  12:16:02  mjg
 * Initial revision
 * 
 *
 *****************************************************************************/
#define MODULE_ID MODID_D3DISP

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "metrics.h"
#include "pvrlims.h"
#include "hwinterf.h"
#include "parmbuff.h"
#include "sgl_math.h"
#include "dregion.h"	/* JWF added for missing prototypes */

#include "pmsabre.h"

#if DAG_TRANS_SORTING

#include "dtri.h"
#include "disp.h"
#include "d3dtsort.h" /* for ITRIPLUS */

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

static TRANS_REGION_DEPTHS_STRUCT gDepthInfo[IBUFFERSIZE];

/* this array indicates whether polys have a vfog or gouraud specular plane */
static FOGHIGHLIGHT eExtraStatus[IBUFFERSIZE];

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

void PackISPCoreClipTestedExtraD3DTrans(sgl_uint32 *pPlaneMem, 
										sgl_uint32  nPolysInChunk, 
										sgl_uint32 *rTSPAddr, 
										PITRI      *rpTri,
										int         nIncrement, 
								        PITRIPLUS *rpTriPlus,
								        int nPlusIncrement, 						   							
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
	PITRIPLUS	pTriPlus = *rpTriPlus;
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
		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2;

		pTriPlus->fA = fA; 
		pTriPlus->fB = fB; 
		pTriPlus->fC = fC;

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
		*((sgl_uint32 *) &pTriPlus)	+= nPlusIncrement;
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

void PackISPCoreExtraD3DTrans(sgl_uint32 *pPlaneMem, 
							  sgl_uint32  nPolysInChunk, 
							  sgl_uint32 *rTSPAddr, 
							  PITRI      *rpTri,
							  int         nIncrement, 
							  PITRIPLUS *rpTriPlus, 
							  int nPlusIncrement, 						   		
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
	PITRIPLUS	pTriPlus = *rpTriPlus;
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
		fC = fZ0*pTri->fAdjoint[2][0] +  fZ1*pTri->fAdjoint[2][1] + fZ2;

		pTriPlus->fA = fA; 
		pTriPlus->fB = fB; 
		pTriPlus->fC = fC;

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
		*((sgl_uint32 *) &pTriPlus)	+= nPlusIncrement;
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

void PackISPCoreD3DTrans(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, 
						 PITRI *rpTri, int nIncrement, 
				         PITRIPLUS *rpTriPlus, int nPlusIncrement, 						   						 
						 sgl_uint32 *rTSPAddr, sgl_uint32 TSPIncrement)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	PITRIPLUS	pTriPlus = *rpTriPlus;
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

		pTriPlus->fA = fA; 
		pTriPlus->fB = fB; 
		pTriPlus->fC = fC;

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
		*((sgl_uint32 *) &pTriPlus)	+= nPlusIncrement;
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

void PackISPCoreClipTestedD3DTrans(sgl_uint32 *pPlaneMem, sgl_uint32 nPolysInChunk, 								    
								   PITRI *rpTri, int nIncrement, 
							       PITRIPLUS *rpTriPlus, int nPlusIncrement, 						   								   
								   sgl_uint32 *rTSPAddr, sgl_uint32 TSPIncrement)
{
	float 	fA, fB, fC;
	float 	fZ0, fZ1, fZ2;
	float 	f1OverDet;
	float 	fMaxVal;
	PITRI	pTri = *rpTri;
	PITRIPLUS	pTriPlus = *rpTriPlus;
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

		pTriPlus->fA = fA; 
		pTriPlus->fB = fB; 
		pTriPlus->fC = fC;

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
		*((sgl_uint32 *) &pTriPlus)	+= nPlusIncrement;
		TSPAddr += TSPIncrement;
	}

	*rpTri = pTri;
	*rTSPAddr = TSPAddr;
}


/**********************************************************************
**
** Hardware: PCX2, PCX2_003
** Calls the core packing function, adds the objects to regions,
** and does the extra stuff needed for vertex fog and gouraud specular
**
**********************************************************************/

int PackISPD3DTriTrans(PITRI rpTri, PITRIPLUS rpTriPlus, int nTriangles, 
					   sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nPolysInChunk;
	PITRI pTri = rpTri;
	PITRIPLUS pTriPlus = rpTriPlus;
	int 	nPolys;
	
	SGL_TIME_START(PACK_ISPTRI_TIME)

	nPolys = nTriangles;

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
			
			pTri = rpTri;
			pTriPlus = rpTriPlus;

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			nPolys -= nPolysInChunk;
			
			SGL_TIME_START(PACK_ISPCORE_TIME)

			/*
			// If we aren't guaranteed that vertices are on screen, then
			// use the safer pack routine
			*/
			if(gPDC.Context.bDoClipping)
			{
				PackISPCoreClipTestedD3DTrans( &(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
												nPolysInChunk, 
												&rpTri, sizeof (ITRI),
												&rpTriPlus, sizeof (ITRIPLUS),
												&TSPAddr, TSPIncrement );
			}
			else
			{
				PackISPCoreD3DTrans( &(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
									nPolysInChunk, 
									&rpTri, sizeof (ITRI),
									&rpTriPlus, sizeof (ITRIPLUS),
									&TSPAddr, TSPIncrement );
			}

			AddRegionD3DTransTris(&pTri, sizeof (ITRI), 
								  &pTriPlus, 4,
								  nPolysInChunk, CurrentPos );			

			SGL_TIME_STOP(PACK_ISPCORE_TIME)
		}
	}
	
	if (nPolys)
	{
		DPFDEV ((DBG_WARNING, "PackISPD3DTriTrans: Out of ISP buffer space"));
	}

	SGL_TIME_STOP(PACK_ISPTRI_TIME)

	return (nTriangles - nPolys);
}

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	
int PackISPD3DTriTransExtra (PITRI rpTri, PITRIPLUS rpTriPlus,
							 PIMATERIAL rpMat, int nTriangles, 
							 sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nPolysInChunk;
	const sgl_uint32  TSPIncrementExtra = 8>>1; /* both the VFog and specular are flat textured */
	int 	nPolys, i, nObjPlanesInWords;
	int TSPSize = 0;
	sgl_uint32 TSPOffsetAddr;
	PIMATERIAL pMat = rpMat;
	PITRIPLUS pTriPlus = rpTriPlus;
	PITRI pTri = rpTri;
	sgl_uint32 DataInc;

	SGL_TIME_START(PACK_ISPTRI_TIME)

	nPolys = nTriangles;
				
	TSPOffsetAddr = TSPAddr + TSPIncrement*nPolys;

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
			pTriPlus = rpTriPlus;
			/*
			// If we aren't guaranteed that vertices are on screen, then
			// use the safer pack routine
			*/
			if(gPDC.Context.bDoClipping)
			{
				PackISPCoreClipTestedExtraD3DTrans(&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
												   nPolysInChunk, &TSPAddr, 
												   &rpTri, sizeof (ITRI), 
												   &rpTriPlus, sizeof (ITRIPLUS),
												   &rpMat, sizeof (IMATERIAL), 
												   TSPIncrement, 
												   &TSPOffsetAddr, TSPIncrementExtra );
			}
			else
			{
				PackISPCoreExtraD3DTrans(&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
										 nPolysInChunk, &TSPAddr, 
										 &rpTri, sizeof (ITRI), 
										 &rpTriPlus, sizeof (ITRIPLUS),		
										 &rpMat, sizeof (IMATERIAL), 
										 TSPIncrement, 
										 &TSPOffsetAddr, TSPIncrementExtra );
			}

			/* add the objects after the extra status has been calculated */
			AddRegionD3DTransTrisExtra(&pTri, sizeof (ITRI), 
									   &pTriPlus, 4,
									   nPolysInChunk, CurrentPos, eExtraStatus);			

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			SGL_TIME_STOP(PACK_ISPCORE_TIME);			
		}
	}
	
	if (nPolys)
	{
		DPFDEV ((DBG_WARNING, "PackISPD3DTriTransExtra: Out of ISP buffer space"));
	}

	SGL_TIME_STOP(PACK_ISPTRI_TIME)

	return (TSPSize);
}

#endif /* (PCX2 || PCX2_003) && !FORCE_NO_FPU */
#endif /*DAG_TRANS_SORTING*/