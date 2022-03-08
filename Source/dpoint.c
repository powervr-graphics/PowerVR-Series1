/******************************************************************************
 * Name         : dpoint.c
 * Author       : Graham Connor
 * Created      : 10/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Low level API top level code for PowerVR Support for 
 *                D3D Point rendering mode
 *                
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dpoint.c,v $
 * Revision 1.48  1997/11/26  15:32:43  sxy
 * added DPFDEV messages.
 *
 * Revision 1.47  1997/11/24  15:18:08  gdc
 * removed reference to SGLTT_TEXAPI
 *
 * Revision 1.46  1997/11/20  15:56:59  sxy
 * added timing code.
 *
 * Revision 1.45  1997/11/11  10:56:55  gdc
 * took range limiting of z-bias value
 *
 * Revision 1.44  1997/11/10  14:31:12  gdc
 * added depth bias
 *
 * Revision 1.43  1997/10/16  11:15:41  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.42  1997/10/15  15:58:48  sxy
 * added DPFDEVF messages.
 *
 * Revision 1.41  1997/09/25  15:40:28  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.40  1997/09/24  15:46:18  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.39  1997/09/08  16:43:48  gdc
 * swapped point texturing so that projected/non-proj text coords
 * has right sense (hopefully)
 *
 * Revision 1.38  1997/08/28  13:51:39  gdc
 * added stuff to follow which context flags are used
 *
 * Revision 1.37  1997/08/26  17:08:49  erf
 * Added TSP flat shaded highlight fix for points.
 *
 * Revision 1.36  1997/08/20  16:29:52  erf
 * Changed to add D3D DrawPrimitive support. Need to specify a point list
 * to index the vertices. Required for DrawIndexedPrimitive.
 *
 * Revision 1.35  1997/08/07  10:09:06  gdc
 * tweak
 *
 * Revision 1.34  1997/08/07  09:57:02  gdc
 * added Mip Map texture path
 *
 * Revision 1.33  1997/08/06  16:48:56  gdc
 * fixed bug in textured points
 *
 * Revision 1.32  1997/07/21  17:22:35  gdc
 * fixe for vignetting when light vols on
 *
 * Revision 1.31  1997/07/14  19:27:45  mjg
 * Added the PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.30  1997/07/09  07:22:07  erf
 * When packing the lines and chunking in required we need to
 * recalculate the datasize so that we start at the page break
 * and not waste ISP plane space.
 *
 * Revision 1.29  1997/06/27  15:47:14  gdc
 * added a point size which reads the linewidth field in the context
 *
 * Revision 1.28  1997/06/09  08:59:28  gdc
 * got rid of compiler warning
 *
 * Revision 1.27  1997/06/06  10:50:42  erf
 * Moved BOGUSZ_INCREMENT definition to sgl.h and renamed to
 * BOGUSINVZ_INCREMENT.
 *
 * Revision 1.26  1997/06/04  13:51:08  erf
 * Added facility for user to set the bogus inverse Z value. New flag
 * and variable (SGLTT_BOGUSINVZPROVIDED and fBogusInvZ) defined in
 * the context structure.
 *
 * Revision 1.25  1997/06/03  15:38:02  gdc
 * added region clipping
 *
 * Revision 1.24  1997/06/02  02:45:10  jop
 * Improve asm intermediates
 *
 * Revision 1.23  1997/05/27  14:32:05  jop
 *  Added a check for available ISP space.
 *
 * Revision 1.22  1997/05/22  14:23:43  erf
 * Added bogus Z stuff when Z buffering is disabled
 *
 * Revision 1.21  1997/05/19  09:01:47  gdc
 * moved GetSabreLimit to parambuff.h
 *
 * Revision 1.20  1997/05/16  18:35:35  erf
 * Changed struct definition of PROCESSDATACONTEXT to PROCESSDATACONTEXTPOINTS
 * to avoid confusion in winice.
 *
 * Revision 1.19  1997/05/16  17:19:27  erf
 * Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
 *
 * Revision 1.18  1997/05/16  15:37:50  gdc
 * tried to fixe textures but it doen't work - left it in as I might
 * come back to this
 *
 * Revision 1.17  1997/05/08  10:07:05  erf
 * 1. Removed static from DirectPoints() so that is can be used by the
 *    PVRDirect driver.
 * 2. Added GetTextureHandle() call instead of GetNamedItemType() and
 *    GetNamedItem() etc, to reduce the files required by the
 *    PVRDirect driver.
 *
 * Revision 1.16  1997/05/06  16:04:03  jop
 * Replaced support for both texture handle types
 *
 * Revision 1.15  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.14  1997/04/30  19:21:25  mjg
 * Updated TEXTURE_MAP to HTEXTURE.
 *
 * Revision 1.13  1997/04/24  20:57:46  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.12  1997/04/22  18:01:19  mjg
 * Removed brdsetup.h and altred the SabreMem functions.
 *
 * Revision 1.11  1997/04/21  15:25:09  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.10  1997/04/08  14:22:48  jop
 * Tried to fix warning but failed dismally
 *
 * Revision 1.9  1997/04/08  13:53:12  jop
 * Cleaned up includes a bit
 * and fixed a syntax error
 *
 * Revision 1.8  1997/04/03  13:21:49  mjg
 * Made changes to allow a Midas3 build - WILL CRASH IF USED!!!
 *
 * Revision 1.7  1997/04/02  21:28:04  mjg
 * Updated calling conventions.
 *
 * Revision 1.6  1997/04/02  15:43:05  gdc
 * bit of a tidy up - textures, still not working well
 *
 * Revision 1.5  1997/04/02  14:27:55  gdc
 * engrained a one pixel shift to make pcx2 work in terms of correct
 * appearance (which is not the same as correct)
 *
 * Revision 1.4  1997/03/31  22:06:11  jop
 * rationalised WIN32s, #include "windows.h", and stuck in some DOS32s
 *
 * Revision 1.3  1997/03/26  19:39:44  gdc
 * almost got points working for PCX2, and textures still not mapping
 * properly
 *
 * Revision 1.2  1997/03/26  18:19:24  gdc
 * slight change to includes for new 4.0 stuff
 *
 * Revision 1.1  1997/03/26  17:35:03  gdc
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_DPOINT

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "sgl_init.h"
#include "hwinterf.h"
#include "parmbuff.h"
#include "sgl_math.h"
#include "pvrlims.h"

#include "nm_intf.h"
#include "getnamtb.h"
#include "dlntypes.h"

#include "pmsabre.h"
#include "dregion.h"
#include "texapi.h"
#include "pvrosio.h"

#include "hwtexas.h"
#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

#if PCX1

#define UPPER_6_OF_TAG		0x3F000UL
#define SFLOAT_20BIT_ZERO	(0)
#define FLOAT_TO_FIXED 		((float) (((sgl_uint32)1) <<31))
#define MAGIC  				(1024.0f)
/*
** Include the real C Packto20bit code
*/
#define INLINE_P20 static INLINE
#include "pto20b.h"
#define InPackTo20Bit PackTo20Bit				/* Try C version      */
#undef  INLINE_P20
#endif

#define INV_MAGIC           (0.00048828125f)


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

#if WIN32 || DOS32

	#define DO_FPU_PRECISION TRUE

	void SetupFPU (void);
	void RestoreFPU (void);

#else

	#define DO_FPU_PRECISION FALSE

#endif

#if  PCX2 || PCX2_003
/* values for the edges */

#define EDGE_0_A  1.0f
#define EDGE_1_A  0.0f
#define EDGE_2_A  -1.0f
#define EDGE_3_A  0.0f

#define EDGE_0_B  0.0f
#define EDGE_1_B  -1.0f
#define EDGE_2_B  0.0f
#define EDGE_3_B  1.0f

#elif PCX1
/* pre 20 bit packed values for the edges */

#define EDGE_0_A  0x000E4000UL
#define EDGE_1_A  0x00000000UL
#define EDGE_2_A  0x000EC000UL
#define EDGE_3_A  0X00000000UL

#define EDGE_0_B  0X00000000UL
#define EDGE_1_B  0x000EC000UL
#define EDGE_2_B  0X00000000UL
#define EDGE_3_B  0x000E4000UL

#elif ISPTSP
#define SabreLimit	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit
#endif

#define IBUFFERSIZE	 48

static float gfDepthBias;

extern sgl_uint32 gu32UsedFlags;

extern float gfBogusInvZ;
extern sgl_bool FogUsed;
extern float fMinInvZ;
extern DEVICE_REGION_INFO_STRUCT RegionInfo;
static TRANS_REGION_DEPTHS_STRUCT gDepthInfo[IBUFFERSIZE];

#ifdef DLL_METRIC
extern sgl_uint32 nTotalPolygonsInFrame, nTransPolygonsInFrame, nOpaquePolygonsInFrame;
#endif

/*
  // We need to "retype" the floating point value as an Integer
  // so we can muck around with the bits. The fastest method seems
  // to depend on the compiler/CPU.
  */
typedef union
{
	sgl_uint32 l;
	float f;
}flong;

typedef struct tagTEXDATA
{
	sgl_uint32	a;
	sgl_uint32	c;
	sgl_uint32	e;
	sgl_uint32	f;
	sgl_uint32	r;
	sgl_uint32  MipMant;
	sgl_uint32  MipExp;
	sgl_uint32  exp;
	
} TEXDATA, *PTEXDATA;

typedef struct tagIMATERIAL
{
	TEXDATA		Tex;	
	sgl_uint32	Highlight;	/* 0xFFFF0000->555 colour, 0x000000FF->alpha */

	union
	{
		sgl_uint32	LightVolCol;
		float	ShadowBrightness;
	} v;

} IMATERIAL, *PIMATERIAL;

typedef struct tagIPOINT
{
	float	fX;
	float	fY;
	float	fZ;

	union	
	{
		sgl_int8	b[4];
		sgl_uint32	u;
	}		reg;
	
	sgl_uint32	TSPControlWord;
	sgl_uint32	BaseColour;

} IPOINT, *PIPOINT;


typedef struct tagPROCESSDATACONTEXTPOINTS
{
	int				nInputPoints;
	sgl_uint16      *pPoints;
	PSGLVERTEX		pVertices; 
	PSGLVERTEX		pCurVer; 
	sgl_uint32		TSPControlWord;

	int				ShiftRegX;
	
	sgl_uint32		TexAddress;
	int				n32MipmapOffset;
	sgl_uint32      uPixelWidth;
	float 			invTexSize;
	float 			fWidthInvTex;
	float   		fWidth;
	float   		fWidth2;
	
	SGLCONTEXT		Context;
	
} PROCESSDATACONTEXTPOINTS;

/* per polygon intermediate routines */
typedef void (* PPIR)(PIPOINT pPoint);

/* per buffer pack routines */
typedef void (* PBPR)(PIPOINT pPoint, PIMATERIAL pMat, sgl_uint32 nPolys,sgl_uint32 *pTSP);


static IPOINT		gpPoints[IBUFFERSIZE];
static IMATERIAL 	gpMat[IBUFFERSIZE];
static PIMATERIAL	gpMatCurrent = gpMat;
const PROCESSDATACONTEXTPOINTS gPointDC;
PROCESSDATACONTEXTPOINTS * const gpPointDC = (PROCESSDATACONTEXTPOINTS *) &gPointDC;

/**********************************************************************/

static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16);

static void ProcessHigh(PIPOINT pPoint);
static void ProcessHighTex(PIPOINT pPoint);
static void ProcessFlatShadOrLiVol(PIPOINT pPoint);
static void ProcessHighShadOrLiVol(PIPOINT pPoint);
static void ProcessFlatTexShadOrLiVol(PIPOINT pPoint);
static void ProcessHighTexShadOrLiVol(PIPOINT pPoint);

static void PackFlat(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTex(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHigh(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTex(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatShad(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighShad(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTexShad(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTexShad(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatLiVol(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighLiVol(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTexLiVol(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTexLiVol(PIPOINT , PIMATERIAL , sgl_uint32 , sgl_uint32 *);

/**********************************************************************/

static void (*ProcessFlatTexFn)( PIPOINT pPoint );

/**********************************************************************/

typedef struct tagIFUNCBLOCK
{
	PPIR	fnPerPoly;
	PBPR	fnPerBuffer;
	sgl_uint32	uSize;
	sgl_uint32	TSPControlWord;

} IFUNCBLOCK, *PIFUNCBLOCK;

#define FLAGS(x,y)		((BASE_FLAGS | (FLAGS_ ## x))&(~(y)))

#define FLAGS_0			0
#define FLAGS_2			MASK_TEXTURE
#define FLAGS_4			MASK_FLAT_HIGHLIGHT
#define FLAGS_6			MASK_FLAT_HIGHLIGHT | MASK_TEXTURE


/*
// Traditional/D3D shading
*/

static IFUNCBLOCK NoVolFuncs[4] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{NULL, 					PackFlat,	    	2,	FLAGS(0, 0) }, 
	{NULL,		            PackFlatTex, 		8,	FLAGS(2, 0) }, 
	{ProcessHigh, 			PackHigh, 			4,	FLAGS(4, 0) }, 
	{ProcessHighTex, 		PackHighTex,		10,	FLAGS(6, 0) }, 
	#undef BASE_FLAGS
};

/*
// SHADOWS
*/

static IFUNCBLOCK ShadowFuncs[4] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency, but with shadows, no lightvols */

	{ProcessFlatShadOrLiVol,	PackFlatShad,		2,	FLAGS(0, 0) }, 
	{ProcessFlatTexShadOrLiVol, PackFlatTexShad,	8,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 	PackHighShad, 		4,	FLAGS(4, 0) }, 
	{ProcessHighTexShadOrLiVol, PackHighTexShad, 	10,	FLAGS(6, 0) }, 

	#undef BASE_FLAGS
};


/*
// LIGHT VOLUMES
*/

static IFUNCBLOCK LightVolFuncs[4] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency, no shadows , but lightvols */

	{ProcessFlatShadOrLiVol,	PackFlatLiVol,		2,	FLAGS(0, 0) }, 
	{ProcessFlatTexShadOrLiVol, PackFlatTexLiVol,	8,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 	PackHighLiVol, 		4,	FLAGS(4, 0) }, 
	{ProcessHighTexShadOrLiVol, PackHighTexLiVol, 	10,	FLAGS(6, 0) }, 

	#undef BASE_FLAGS
};

/**********************************************************************/

static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16)
{

	float Red =  (float)((Colour & 0x00FF0000) >> 16);
	float Green = (float)((Colour & 0x0000FF00) >> 8);
	float Blue =   (float)(Colour & 0x000000FF);
	
	f16 *= (31.0f / 255.0f);

	/*
	// NOTE: Convert first to SIGNED int as this is MUCH quicker
	// (generally) than conversion straight to UNSIGNED
	*/
	*p24= (sgl_uint32) ( ((sgl_int32)(Blue*f24)) | 
					(((sgl_int32)(Green*f24))<<8) |
					(((sgl_int32)(Red*f24))<<16)
				   );

	*p16= (sgl_uint32) ( ((sgl_int32)(Blue*f16)) | 
					(((sgl_int32)(Green*f16))<<5) |
					(((sgl_int32)(Red*f16))<<10)
				   );
}

/**********************************************************************/

static int PackISPPointCore(sgl_uint32 *pPlaneMem, sgl_uint32 nPointsInChunk, 
							sgl_uint32 *rTSPAddr, PIPOINT *rpPoints,
							int nIncrement, sgl_uint32 TSPIncrement)
{
	PIPOINT	pPoint = *rpPoints;
	sgl_uint32	TSPAddr = *rTSPAddr;
    float width = gpPointDC->fWidth;

	while(nPointsInChunk--)
	{
		float fX, fY, fX1, fY1,	fZ;

#if PCX2 || PCX2_003
		fX = -pPoint->fX;
		fY = -pPoint->fY;
		fX1 = (width - fX);
		fY1 = (width - fY);
		fZ = pPoint->fZ + gfDepthBias;

		IW(pPlaneMem, 0+0, 0);
		IW(pPlaneMem, 1+0, 0);
		FW(pPlaneMem, 2+0, fZ);
		/* Pack instruction and TSP address.
		 */
		IW(pPlaneMem, 3+0, ( forw_visib_fp | (TSPAddr << 4)));

		/* do the edges of the point */
		FW( pPlaneMem, 0+4, EDGE_0_A);
		FW( pPlaneMem, 1+4, EDGE_0_B);
		FW( pPlaneMem, 2+4, fX);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		FW( pPlaneMem, 0+8, EDGE_1_A);
		FW( pPlaneMem, 1+8, EDGE_1_B);
		FW( pPlaneMem, 2+8, fY1);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		FW( pPlaneMem, 0+12, EDGE_2_A);
		FW( pPlaneMem, 1+12, EDGE_2_B);
		FW( pPlaneMem, 2+12, fX1);
		IW( pPlaneMem, 3+12, (forw_perp | (DEBUG_PERP_TAG << 4)));

		FW( pPlaneMem, 0+16, EDGE_3_A);
		FW( pPlaneMem, 1+16, EDGE_3_B);
		FW( pPlaneMem, 2+16, fY);
		IW( pPlaneMem, 3+16, (forw_perp | (DEBUG_PERP_TAG << 4)));
		
#elif PCX1
		fX = -pPoint->fX;
		fY = -pPoint->fY;
		fX1 = width - fX;
		fY1 = width - fY;

		pPlaneMem[0+0] = forw_visib_fp | 
			((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |	SFLOAT_20BIT_ZERO;
		pPlaneMem[1+0] = (TSPAddr << 20) | SFLOAT_20BIT_ZERO;
		pPlaneMem[2+0] = (sgl_int32) (pPoint->fZ * FLOAT_TO_FIXED);

		/* do the edges of the point */
		pPlaneMem[0+3] = forw_perp | 
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | EDGE_0_A ;
		pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20) | EDGE_0_B ;
		pPlaneMem[2+3] = (sgl_int32) ( FLOAT_TO_FIXED * fX * INV_MAGIC);


		pPlaneMem[0+6] = forw_perp | 
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | EDGE_1_A ;
		pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20) | EDGE_1_B ;
		pPlaneMem[2+6] = (sgl_int32) ( FLOAT_TO_FIXED * fY1 * INV_MAGIC);


		pPlaneMem[0+9] = forw_perp | 
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | EDGE_2_A ;
		pPlaneMem[1+9] = (DEBUG_PERP_TAG << 20) | EDGE_2_B ;
		pPlaneMem[2+9] = (sgl_int32) ( FLOAT_TO_FIXED * fX1 * INV_MAGIC);


		pPlaneMem[0+12] = forw_perp | 
			((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | EDGE_3_A ;
		pPlaneMem[1+12] = (DEBUG_PERP_TAG << 20) | EDGE_3_B ;
		pPlaneMem[2+12] = (sgl_int32) ( FLOAT_TO_FIXED * fY * INV_MAGIC);
		
#elif ISPTSP
#pragma message("ISPTSP doesn't have points support")
#endif
		pPlaneMem += WORDS_PER_PLANE * 5;
		
		*((sgl_uint32 *) &pPoint) += nIncrement;
		TSPAddr += TSPIncrement;

	}

	return (0);
}

/**********************************************************************/

static int PackISPPoints(PIPOINT pPoint, PIMATERIAL pMat, 
						 int numPoints, sgl_uint32 TSPAddr, 
						 sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nPointsInChunk;
	int 	nPoints;
	
	nPoints = numPoints;
				
	while (nPoints)
	{
		CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, 
									   WORDS_PER_PLANE * 5);

		if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
		{
			break;
		}
		else
		{
			DataSize = WORDS_PER_PLANE * 5 * nPoints;

			ChunkLimit = GetSabreLimit (CurrentPos);
			ChunkSize = ChunkLimit - CurrentPos;

			if (DataSize < ChunkSize)
			{
				nPointsInChunk = nPoints;
			}
			else
			{
				nPointsInChunk = ChunkSize / (WORDS_PER_PLANE * 5);
				
				if (!nPointsInChunk)
				{
					break;
				}

				/* We need to re-calculate the datasize based on the number of points
				 * from the bunch we are processing.
				 */
				DataSize = WORDS_PER_PLANE * 5 * nPointsInChunk;
			}
			
			AddRegionObjects((sgl_uint32 *) &(pPoint->reg), sizeof (IPOINT),
							 5, nPointsInChunk, CurrentPos, 
							 gDepthInfo, sizeof(TRANS_REGION_DEPTHS_STRUCT));

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			nPoints -= nPointsInChunk;
			
			PackISPPointCore( &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos], 
							  nPointsInChunk, 
							  &TSPAddr, &pPoint,
							  sizeof (IPOINT), TSPIncrement );
		}
	}
	
	if (nPoints)
	{
		DPFDEV ((DBG_WARNING, "PackISPPoint: Out of ISP buffer space"));
	}

	return (numPoints - nPoints);
}

/**********************************************************************/

#define PACKFLAT(a,b,c) \
a[0] = b->TSPControlWord | ((c >> 16) & 0x000000FF); \
a[1] = c << 16; 

#define PACKFLATTEX(a,b,c,d) \
a[0] = b->TSPControlWord |((c >> 16) & 0x000000FF)|(d->Tex.exp << SHIFT_EXPONENT); \
a[1] = c << 16; 

#define PACKHIGH(a,b) \
a[2] = b->Highlight; \
a[3] = 0x0L;

#define PACKHIGHTEX(a,b) \
a[8] = b->Highlight; \
a[9] = 0x0L;

#define PACKTEX(w,x,y,z) \
w[2] = ((x->Tex.MipMant) << SHIFT_PMIP_M) | \
	   ((x->Tex.MipExp)  << SHIFT_PMIP_E) |((x->Tex.r) & 0xffffUL); \
w[3] = 0x0L; \
w[4] = (z) | (( x->Tex.c) & 0xffffUL); \
w[5] = (( x->Tex.a) & 0xffffUL); \
w[6] = (y) | (( x->Tex.f) & 0xffffUL); \
w[7] = (x->Tex.e << 16);

#define PACKFLATSHADLV(a, b, c, d) \
a[0] = (b->TSPControlWord | ((c >> 16) & 0x000000FF)); \
a[1] = (c << 16) | d;

#define PACKFLATSHADLVTEX(a, b, c, d, e) \
a[0] = (b->TSPControlWord | ((c >> 16) & 0x000000FF))|(e->Tex.exp << SHIFT_EXPONENT); \
a[1] = (c << 16) | d;

/**********************************************************************/

static void PackFlat( PIPOINT pPoint, PIMATERIAL pMat, 
					 sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	/*
	// Go through the points. Move on to the next shading results and
	// and next point pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		
		PACKFLAT(pTSP , pPoint, uBaseColour);
		
		pPoint++;
		pTSP += 2;
		
	}/*end while*/
}


/**********************************************************************/

static void PackHigh(PIPOINT pPoint, PIMATERIAL pMat, 
					 sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	/*
	// Go through the points. Move on to the next shading results and
	// and next point pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		
		PACKFLAT(pTSP, pPoint, uBaseColour);
		PACKHIGH(pTSP, pMat);
		
		pPoint++;
		pMat++;
		pTSP += 4;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatTex(PIPOINT pPoint, PIMATERIAL pMat, 
						sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	/*
	// Go through the points. Move on to the next shading results and
	// and next point pointer (NOTE Planes is array of pointers)
	*/

	sgl_uint32 AddrHi, AddrLo;

	AddrHi = gPointDC.TexAddress & 0xffff0000;
	AddrLo = gPointDC.TexAddress << 16;

	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		
		PACKFLATTEX(pTSP, pPoint, uBaseColour, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pPoint++;
		pMat++;
		pTSP += 8;
		
	}/*end while*/
	
}

/**********************************************************************/

static void PackHighTex(PIPOINT pPoint, PIMATERIAL pMat, 
						sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	/*
	// Go through the points. Move on to the next shading results and
	// and next point pointer (NOTE Planes is array of pointers)
	*/

	sgl_uint32 AddrHi, AddrLo;

	AddrHi = gPointDC.TexAddress & 0xffff0000;
	AddrLo = gPointDC.TexAddress << 16;

	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		
		PACKFLATTEX(pTSP, pPoint, uBaseColour, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pPoint++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatShad(PIPOINT pPoint, PIMATERIAL pMat, 
						 sgl_uint32 numPoints, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pPoint->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pPoint, uFlat0Col24, uFlat1Col16);
		
		pPoint++;
		pTSP += 2;
		
	}/*end for*/
}

/**********************************************************************/

static void PackHighShad(PIPOINT pPoint, PIMATERIAL pMat, 
						 sgl_uint32 numPoints, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pPoint->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pPoint, uFlat0Col24, uFlat1Col16);
		PACKHIGH(pTSP, pMat);
		
		pPoint++;
		pMat++;
		pTSP += 4;
		
	}/*end for*/
}

/**********************************************************************/

static void PackFlatTexShad(PIPOINT pPoint, PIMATERIAL pMat, 
							sgl_uint32 numPoints, sgl_uint32 *pTSP)
{


	sgl_uint32 AddrHi, AddrLo;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

	AddrHi = gPointDC.TexAddress & 0xffff0000;
	AddrLo = gPointDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pPoint->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLVTEX(pTSP, pPoint, uFlat0Col24, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pPoint++;
		pMat++;
		pTSP += 8;
		
	}/*end for*/
}

/**********************************************************************/

static void PackHighTexShad(PIPOINT pPoint, PIMATERIAL pMat, 
							sgl_uint32 numPoints, sgl_uint32 *pTSP)
{


	sgl_uint32 AddrHi, AddrLo;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

	AddrHi = gPointDC.TexAddress & 0xffff0000;
	AddrLo = gPointDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pPoint->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLVTEX(pTSP, pPoint, uFlat0Col24, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pPoint++;
		pMat++;
		pTSP += 10;
		
	}/*end for*/
}

/**********************************************************************/

static void PackFlatLiVol(PIPOINT pPoint, PIMATERIAL pMat, 
						  sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		PACKFLATSHADLV(pTSP, pPoint, uBaseColour, uFlat1Col16);
		
		pPoint++;
		pTSP += 2;
		
	}/*end for*/

}

/**********************************************************************/

static void PackHighLiVol(PIPOINT pPoint, PIMATERIAL pMat, 
						  sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		PACKFLATSHADLV(pTSP, pPoint, uBaseColour, uFlat1Col16);
		PACKHIGH(pTSP, pMat);
		
		pPoint++;
		pMat++;
		pTSP += 4;
		
	}/*end for*/

}

/**********************************************************************/

static void PackFlatTexLiVol(PIPOINT pPoint, PIMATERIAL pMat, 
							 sgl_uint32 numPoints, sgl_uint32 *pTSP)
{
	sgl_uint32 AddrHi, AddrLo;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	
	AddrHi = gPointDC.TexAddress & 0xffff0000;
	AddrLo = gPointDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pPoint->BaseColour;
		PACKFLATSHADLVTEX(pTSP, pPoint, uBaseColour, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pPoint++;
		pMat++;
		pTSP += 8;
		
	}/*end for*/

}

/**********************************************************************/

static void PackHighTexLiVol(PIPOINT pPoint, PIMATERIAL pMat, 
							 sgl_uint32 numPoints, sgl_uint32 *pTSP)
{

	sgl_uint32 AddrHi, AddrLo;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	AddrHi = gPointDC.TexAddress & 0xffff0000;
	AddrLo = gPointDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numPoints--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pPoint->BaseColour;
		
		PACKFLATSHADLVTEX(pTSP, pPoint, uBaseColour, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pPoint++;
		pMat++;
		pTSP += 10;
		
	}/*end for*/
}

/**********************************************************************/
/**********************************************************************/

#define IMPLEMENT_HIGHLIGHT						 \
{												 \
	sgl_uint32 H = gpPointDC->pCurVer->u32Specular;	 \
	pMat->Highlight = ((H << 07) & 0x7C000000) | \
					  ((H << 10) & 0x03E00000) | \
					  ((H << 13) & 0x001F0000);  \
}

#define IMPLEMENT_SHADOW_LIGHTVOL \
	pMat->v.LightVolCol = gpPointDC->Context.u.u32LightVolColour;

/**********************************************************************/
/**********************************************************************/

static void ProcessLitePointTexMipMap(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_a_e, adj_c, adj_f, adj_r, compression;
	float temp_1,largestT,largestB;
	float width=gpPointDC->fWidth;
	float width2=gpPointDC->fWidth2;
	float fWidthInvTex=gpPointDC->fWidthInvTex;
	float fInvTexSize = gpPointDC->invTexSize;
	flong topExponent, MipMap;
	long exp, MipMant, MipExp;

	temp_1 = width2 * gpPointDC->pCurVer->fInvW;

	adj_a_e = fWidthInvTex * gpPointDC->pCurVer->fInvW;
	adj_c = temp_1 * gpPointDC->pCurVer->fUOverW - (gPointDC.pCurVer->fX * adj_a_e);

	adj_f = temp_1 * gpPointDC->pCurVer->fVOverW - (gPointDC.pCurVer->fY * adj_a_e);

	adj_r = width2 * fInvTexSize;

	/*
	**	calculate the MIP-map coefficient.
	*/
	
	compression = adj_a_e*adj_r;
	
	ASSERT(compression>= 0.0f);
	

	/* Rescale c,f,r */
	adj_c *= (1.0f/1023.0f);
	adj_f *= (1.0f/1023.0f);
	adj_r *= (1.0f/1023.0f);

	/*
	**	find the largest (magnitude) of a,b,c,d,e,f 
	** 
	** On some slower processors floating point compare
	** etc is very slow. Since these values are all positive, we
	** can just as validly "assume" they are integer as the IEEE
	** fp format will work.
	** Note also that fabs can be replaced with a clearing of the
	** TOP bit.
	** This bit of code also assumes that there are B'all FP registers
	** and therefore most FP values are sitting out in memory (not
	** registers)
	*/
#if SLOW_FCMP && !MULTI_FP_REG

	{
		long  LargestIntEquiv;
		long  temp;
		
		/*
		// Get the abs of a, and "convert" it to int.
		*/
		LargestIntEquiv = FLOAT_TO_LONG(adj_a_e) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		largestT = LONG_TO_FLOAT(LargestIntEquiv);
	}
	/*
    // Use FP compares to get the largest
	*/
	
#else

	largestT= sfabs(adj_a_e);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;

	FabsT1=sfabs(adj_f);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
#endif

	/*
	** find the largest of p,q,r 
	**
	** Again use a suitable method...
	*/
	largestB = adj_r;
		
	/*
	** calculate a fast floor(log2(largest/largest_bot))
	*/
	
	largestT=1.0f/largestT;
	
	topExponent.f=largestB*largestT;

	/* extract the offset exponent from the IEEE fp number */
	
	exp=(topExponent.l>>23) & 0xff; 	
	
	/* calculate the reciprocal (ignore post normalising) */
	
	exp=126-exp;
	
	/*
	** rescale the texturing coefficients to 16 bits.
	*/
	
	largestT=largestT * 32767.0f;
	
	adj_a_e=adj_a_e * largestT;
	adj_c=adj_c * largestT;
	adj_f=adj_f * largestT;
	
	/* calculate a fast pow(2.0,floor())*largestT */
	
	topExponent.l=(exp+127)<<23;
	
	largestT=largestT * topExponent.f;
	
	adj_r=adj_r * largestT;
	

	MipMap.f=compression*largestT*largestT;

	/* convert from IEEE to the TEXAS floating point format*/
	
	MipMant =(MipMap.l>>16)&0x7f;
	MipMant+=128; /*add in the implied 0.5*/
	/*126 because of the different decimal point*/
	MipExp = (MipMap.l>>23)-126 + gPointDC.n32MipmapOffset;
	
	/*
	** Texas can't handle an exponent greater than 15,so we will
	** reduce the resolution of 'p', 'q', and 'r'.
	** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
	** should have removed the texture by now.
	*/
	
	if(exp>15)
	{
		adj_r=(float)((sgl_int32)adj_r >>(exp-15));
		MipExp-=(exp-15)<<1;
		
		exp=15;
	}
	
	/*
	** Texas can't handle a negative exponent,so we will
	** reduce the resolution of a,b,c,d,e and f.
	** This condition only happens if the texture is VERY zoomed.
	** It may not be worth the expense of testing for this.
	*/
	else if(exp<0)
	{
		adj_a_e=(float)((sgl_int32)adj_a_e >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a_e;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.e = (sgl_int32)adj_a_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;
	
	pMat->Tex.MipMant = MipMant;
	pMat->Tex.MipExp = MipExp;

	pMat->Tex.exp = exp;

}

/**********************************************************************/

static void ProcessD3DPointTexMipMap(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_a_e, adj_c, adj_f, adj_r, compression;
	float largestT,largestB;
	float width=gpPointDC->fWidth;
	float width2=gpPointDC->fWidth2;
	float fWidthInvTex=gpPointDC->fWidthInvTex;
	float fInvTexSize = gpPointDC->invTexSize;
	flong topExponent, MipMap;
	long exp, MipMant, MipExp;

	adj_a_e = fWidthInvTex * gpPointDC->pCurVer->fInvW;

	adj_c = width2 * gpPointDC->pCurVer->fUOverW - (gpPointDC->pCurVer->fX * adj_a_e);

	adj_f = width2 * gpPointDC->pCurVer->fVOverW - (gpPointDC->pCurVer->fY * adj_a_e);

	adj_r = width2 * fInvTexSize;

	/*
	**	calculate the MIP-map coefficient.
	*/
	compression = adj_a_e*adj_r;
	
	ASSERT(compression>= 0.0f);
	

	adj_c *= (1.0f/1023.0f);
	adj_f *= (1.0f/1023.0f);
	adj_r *= (1.0f/1023.0f);
	/*
	**	find the largest (magnitude) of a,b,c,d,e,f 
	** 
	** On some slower processors floating point compare
	** etc is very slow. Since these values are all positive, we
	** can just as validly "assume" they are integer as the IEEE
	** fp format will work.
	** Note also that fabs can be replaced with a clearing of the
	** TOP bit.
	** This bit of code also assumes that there are B'all FP registers
	** and therefore most FP values are sitting out in memory (not
	** registers)
	*/
#if SLOW_FCMP && !MULTI_FP_REG

	{
		long  LargestIntEquiv;
		long  temp;
		
		/*
		// Get the abs of a, and "convert" it to int.
		*/
		LargestIntEquiv = FLOAT_TO_LONG(adj_a_e) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}

		temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		largestT = LONG_TO_FLOAT(LargestIntEquiv);
	}
	/*
    // Use FP compares to get the largest
	*/
	
#else

	largestT= sfabs(adj_a_e);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT1=sfabs(adj_f);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
#endif

	/*
	** find the largest of p,q,r 
	**
	** Again use a suitable method...
	*/
	largestB = adj_r;
		
	/*
	** calculate a fast floor(log2(largest/largest_bot))
	*/
	
	largestT=1.0f/largestT;
	
	topExponent.f=largestB*largestT;

	/* extract the offset exponent from the IEEE fp number */
	
	exp=(topExponent.l>>23) & 0xff; 	
	
	/* calculate the reciprocal (ignore post normalising) */
	
	exp=126-exp;
	
	/*
	** rescale the texturing coefficients to 16 bits.
	*/
	
	largestT=largestT * 32767.0f;
	
	adj_a_e=adj_a_e * largestT;
	adj_c=adj_c * largestT;
	adj_f=adj_f * largestT;
	
	/* calculate a fast pow(2.0,floor())*largestT */
	
	topExponent.l=(exp+127)<<23;
	
	largestT=largestT * topExponent.f;
	
	adj_r=adj_r * largestT;
	
	MipMap.f=compression*largestT*largestT;

	/* convert from IEEE to the TEXAS floating point format*/
	
	MipMant =(MipMap.l>>16)&0x7f;
	MipMant+=128; /*add in the implied 0.5*/
	/*126 because of the different decimal point*/
	MipExp = (MipMap.l>>23)-126 + gPointDC.n32MipmapOffset;
	
	/*
	** Texas can't handle an exponent greater than 15,so we will
	** reduce the resolution of 'p', 'q', and 'r'.
	** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
	** should have removed the texture by now.
	*/
	
	if(exp>15)
	{
		adj_r=(float)((sgl_int32)adj_r >>(exp-15));
		MipExp-=(exp-15)<<1;
		
		exp=15;
	}
	
	/*
	** Texas can't handle a negative exponent,so we will
	** reduce the resolution of a,b,c,d,e and f.
	** This condition only happens if the texture is VERY zoomed.
	** It may not be worth the expense of testing for this.
	*/
	else if(exp<0)
	{
		adj_a_e=(float)((sgl_int32)adj_a_e >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a_e;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.e = (sgl_int32)adj_a_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.MipMant = MipMant;
	pMat->Tex.MipExp = MipExp;

	pMat->Tex.exp = exp;

}

/**********************************************************************/


static void ProcessLitePointTex(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_a_e, adj_c, adj_f, adj_r;
	float temp_1,largestT,largestB;
	float width=gpPointDC->fWidth;
	float width2=gpPointDC->fWidth2;
	float fWidthInvTex=gpPointDC->fWidthInvTex;
	float fInvTexSize = gpPointDC->invTexSize;
	flong topExponent;
	long exp;

	temp_1 = width2 * gpPointDC->pCurVer->fInvW;

	adj_a_e = fWidthInvTex * gpPointDC->pCurVer->fInvW;
	adj_c = temp_1 * gpPointDC->pCurVer->fUOverW - (gPointDC.pCurVer->fX * adj_a_e);

	adj_f = temp_1 * gpPointDC->pCurVer->fVOverW - (gPointDC.pCurVer->fY * adj_a_e);

	adj_r = width2 * fInvTexSize;

	adj_c *= (1.0f/1023.0f);
	adj_f *= (1.0f/1023.0f);
	adj_r *= (1.0f/1023.0f);
		/*
		**	find the largest (magnitude) of a,b,c,d,e,f 
		** 
		** On some slower processors floating point compare
		** etc is very slow. Since these values are all positive, we
		** can just as validly "assume" they are integer as the IEEE
		** fp format will work.
		** Note also that fabs can be replaced with a clearing of the
		** TOP bit.
		** This bit of code also assumes that there are B'all FP registers
		** and therefore most FP values are sitting out in memory (not
		** registers)
		*/
#if SLOW_FCMP && !MULTI_FP_REG

	{
		long  LargestIntEquiv;
		long  temp;
		
		/*
		// Get the abs of a, and "convert" it to int.
		*/
		LargestIntEquiv = FLOAT_TO_LONG(adj_a_e) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		largestT = LONG_TO_FLOAT(LargestIntEquiv);
	}
	/*
    // Use FP compares to get the largest
	*/
	
#else

	largestT= sfabs(adj_a_e);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;

	FabsT1=sfabs(adj_f);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
#endif

	/*
	** find the largest of p,q,r 
	**
	** Again use a suitable method...
	*/
	largestB = adj_r;
		
	/*
	** calculate a fast floor(log2(largest/largest_bot))
	*/
	
	largestT=1.0f/largestT;
	
	topExponent.f=largestB*largestT;

	/* extract the offset exponent from the IEEE fp number */
	
	exp=(topExponent.l>>23) & 0xff; 	
	
	/* calculate the reciprocal (ignore post normalising) */
	
	exp=126-exp;
	
	/*
	** rescale the texturing coefficients to 16 bits.
	*/
	
	largestT=largestT * 32767.0f;
	
	adj_a_e=adj_a_e * largestT;
	adj_c=adj_c * largestT;
	adj_f=adj_f * largestT;
	
	/* calculate a fast pow(2.0,floor())*largestT */
	
	topExponent.l=(exp+127)<<23;
	
	largestT=largestT * topExponent.f;
	
	adj_r=adj_r * largestT;
	
	/*
	** Texas can't handle an exponent greater than 15,so we will
	** reduce the resolution of 'p', 'q', and 'r'.
	** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
	** should have removed the texture by now.
	*/
	
	if(exp>15)
	{
		adj_r=(float)((sgl_int32)adj_r >>(exp-15));
		
		exp=15;
	}
	
	/*
	** Texas can't handle a negative exponent,so we will
	** reduce the resolution of a,b,c,d,e and f.
	** This condition only happens if the texture is VERY zoomed.
	** It may not be worth the expense of testing for this.
	*/
	else if(exp<0)
	{
		adj_a_e=(float)((sgl_int32)adj_a_e >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a_e;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.e = (sgl_int32)adj_a_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.MipMant = 0x0UL;
	pMat->Tex.MipExp = 0x0UL;

	pMat->Tex.exp = exp;

}

/**********************************************************************/

static void ProcessD3DPointTex(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_a_e, adj_c, adj_f, adj_r;
	float largestT,largestB;
	float width=gpPointDC->fWidth;
	float width2=gpPointDC->fWidth2;
	float fWidthInvTex=gpPointDC->fWidthInvTex;
	float fInvTexSize = gpPointDC->invTexSize;
	flong topExponent;
	long exp;

	adj_a_e = fWidthInvTex * gpPointDC->pCurVer->fInvW;

	adj_c = width2 * gpPointDC->pCurVer->fUOverW - (gpPointDC->pCurVer->fX * adj_a_e);

	adj_f = width2 * gpPointDC->pCurVer->fVOverW - (gpPointDC->pCurVer->fY * adj_a_e);

	adj_r = width2 * fInvTexSize;

	adj_c *= (1.0f/1023.0f);
	adj_f *= (1.0f/1023.0f);
	adj_r *= (1.0f/1023.0f);
		/*
		**	find the largest (magnitude) of a,b,c,d,e,f 
		** 
		** On some slower processors floating point compare
		** etc is very slow. Since these values are all positive, we
		** can just as validly "assume" they are integer as the IEEE
		** fp format will work.
		** Note also that fabs can be replaced with a clearing of the
		** TOP bit.
		** This bit of code also assumes that there are B'all FP registers
		** and therefore most FP values are sitting out in memory (not
		** registers)
		*/
#if SLOW_FCMP && !MULTI_FP_REG

	{
		long  LargestIntEquiv;
		long  temp;
		
		/*
		// Get the abs of a, and "convert" it to int.
		*/
		LargestIntEquiv = FLOAT_TO_LONG(adj_a_e) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}

		temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		largestT = LONG_TO_FLOAT(LargestIntEquiv);
	}
	/*
    // Use FP compares to get the largest
	*/
	
#else

	largestT= sfabs(adj_a_e);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT1=sfabs(adj_f);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
#endif

	/*
	** find the largest of p,q,r 
	**
	** Again use a suitable method...
	*/
	largestB = adj_r;
		
	/*
	** calculate a fast floor(log2(largest/largest_bot))
	*/
	
	largestT=1.0f/largestT;
	
	topExponent.f=largestB*largestT;

	/* extract the offset exponent from the IEEE fp number */
	
	exp=(topExponent.l>>23) & 0xff; 	
	
	/* calculate the reciprocal (ignore post normalising) */
	
	exp=126-exp;
	
	/*
	** rescale the texturing coefficients to 16 bits.
	*/
	
	largestT=largestT * 32767.0f;
	
	adj_a_e=adj_a_e * largestT;
	adj_c=adj_c * largestT;
	adj_f=adj_f * largestT;
	
	/* calculate a fast pow(2.0,floor())*largestT */
	
	topExponent.l=(exp+127)<<23;
	
	largestT=largestT * topExponent.f;
	
	adj_r=adj_r * largestT;
	
	/*
	** Texas can't handle an exponent greater than 15,so we will
	** reduce the resolution of 'p', 'q', and 'r'.
	** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
	** should have removed the texture by now.
	*/
	
	if(exp>15)
	{
		adj_r=(float)((sgl_int32)adj_r >>(exp-15));
		
		exp=15;
	}
	
	/*
	** Texas can't handle a negative exponent,so we will
	** reduce the resolution of a,b,c,d,e and f.
	** This condition only happens if the texture is VERY zoomed.
	** It may not be worth the expense of testing for this.
	*/
	else if(exp<0)
	{
		adj_a_e=(float)((sgl_int32)adj_a_e >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a_e;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.e = (sgl_int32)adj_a_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.MipMant = 0x0UL;
	pMat->Tex.MipExp = 0x0UL;

	pMat->Tex.exp = exp;

}

/**********************************************************************/

static void ProcessHigh(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessHighTex(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessFlatShadOrLiVol(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighShadOrLiVol(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessFlatTexShadOrLiVol(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighTexShadOrLiVol(PIPOINT pPoint)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

/* Used to index the face list if called be DrawPrimitive.
 */
static	sgl_uint32	nPointsIndex = 0;


static int ProcessPointsCore( PPIR pPerPointfn)
{
	sgl_int32 rX0, rY0, rY1, rX1;
	PSGLVERTEX pVert;
	sgl_uint32 uCurrentType;
	PIPOINT	pPoint = gpPoints;
	int	ShiftRegX = gpPointDC->ShiftRegX;
	sgl_uint32 PixelWidth = gpPointDC->uPixelWidth;

	while ( gpPointDC->nInputPoints )
	{

		gpPointDC->nInputPoints--;

		/* Have we specified a list of points as in DrawPrimitive.
		 */
		if (gpPointDC->pPoints == NULL)
		{
			/* Normal case.
			 */
			gpPointDC->pCurVer = &(gpPointDC->pVertices[gpPointDC->nInputPoints]);
		}
		else
		{
			sgl_uint16 *pnPoints;
		
			pnPoints = (sgl_uint16 *) gpPointDC->pPoints;

			/* This is the DrawIndexPrimitive.
			 */
			gpPointDC->pCurVer = &(gpPointDC->pVertices[(sgl_uint16) *(pnPoints + nPointsIndex)]);
			nPointsIndex++;
		}

		pVert = gpPointDC->pCurVer;

		/* if we round the point coords to always fall in the pixel center
		** each point only goes in one region - guarenteed
		*/
		/*
		** For some reason PCX2 coordinates must be shifted
		** towards the origin to appear in their correct region
		** but whether this means the pixels are in the correct place 
		** on the screen remains to be seen 
		*/
#if PCX2 || PCX2_003
		rX0 = (int)(pVert->fX);
		pPoint->fX = (float)(rX0-1);
		rX1 = rX0 + PixelWidth;
		rX0 >>= ShiftRegX;
		rX1 >>= ShiftRegX;

		rY0 = (int)(pVert->fY);
		pPoint->fY = (float)(rY0-1);
		rY1 = rY0 + PixelWidth;
#else
		rX0 = (int)(pVert->fX);
		pPoint->fX = (float)(rX0);
		rX1 = rX0 + PixelWidth;
		rX0 >>= ShiftRegX;
		rX1 >>= ShiftRegX;

		rY0 = (int)(pVert->fY);
		pPoint->fY = (float)(rY0);
		rY1 = rY0 + PixelWidth;
#endif

#if DEBUGDEV || DEBUG
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpPointDC->Context.bDoClipping))
		{
			if( (rX0 < gpPointDC->Context.FirstXRegion) ||
				(rY0 < gpPointDC->Context.FirstYRegion) ||
				(rX1 > gpPointDC->Context.LastXRegion)  ||
				(rY1 > gpPointDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessPointsCore: need clipping but bDoClipping is FALSE"));	
        	}
		}
#endif

		/*
		// Check for Region clipping
		*/
		if (gpPointDC->Context.bDoClipping)
		{
			if (rX0 < gpPointDC->Context.FirstXRegion)
			{
				if (rX1 < gpPointDC->Context.FirstXRegion)
				{
					continue;
				}
				else
				{
					rX0 = gpPointDC->Context.FirstXRegion;
				}
			}
		
			if (rY0 < gpPointDC->Context.FirstYRegion)
			{
		
				if (rY1 < gpPointDC->Context.FirstYRegion)
				{
					continue;
				}
				else
				{
					rY0 = gpPointDC->Context.FirstYRegion;
				}

			}
		
			if (rX1 > gpPointDC->Context.LastXRegion)
			{
				if (rX0 > gpPointDC->Context.LastXRegion)
				{
					continue;
				}
				else
				{
					rX1 = gpPointDC->Context.LastXRegion;
				}

			}
		
			if (rY1 > gpPointDC->Context.LastYRegion)
			{
				if (rY0 > gpPointDC->Context.LastYRegion)
				{
					continue;
				}
				else
				{
					rY1 = gpPointDC->Context.LastYRegion;
				}
			}
		}

		if (gpPointDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
			pPoint->fZ = gfBogusInvZ;

			gfBogusInvZ += BOGUSINVZ_INCREMENT;
		}
		else
		{
			pPoint->fZ = pVert->fInvW * fMinInvZ;
		}
#if DEBUG || DEBUGDEV

		if (pVert->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessPointsCore: fInvW out of range"));
		}
#endif
		/* for the moment only flat shaded opaque */
		uCurrentType = PACKED_TYPE_OPAQUE;

		/* determine region in which point falls - max of 1 regions */
		pPoint->reg.u = ENCODE_OBJXYDATA( uCurrentType, rX0, rY0, rX1, rY1 );
		pPoint->TSPControlWord = 	gpPointDC->TSPControlWord;
		pPoint->BaseColour = 		pVert->u32Colour;


		/* call the per point func here */
		if(pPerPointfn)
		{
			pPerPointfn(pPoint);
		}

		if(ProcessFlatTexFn)
		{
			ProcessFlatTexFn(pPoint);
		}
			
		if ( ++pPoint == &gpPoints[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}
		gpMatCurrent++;

	}/*end while*/

	/* Return number in intermediate buffer this time */
	return( (int) ( pPoint - gpPoints ) );

}

/**********************************************************************/

static void ProcessPoints( PPIR pPerPointfn, PBPR pPerBuffn, sgl_uint32 TSPWords)
{
	/* function to process the context data for the points */
	sgl_uint32	TSPAddr;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;

	while ( gpPointDC->nInputPoints != 0 )
	{
		int nBurst;

		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */

		nBurst = ProcessPointsCore( pPerPointfn);

		SGL_TIME_STOP(SGLTRI_PROCESS_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)

		/* Process the content of the buffer */
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PACKTRI_TIME)

		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;
		
		/*
			Work out how many triangle's worth of data we can
			actually put in there ...
		*/
		TSPSpaceAvailable = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit -
							PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
														  		
		if (TSPSpaceAvailable < (nBurst * TSPWords))
		{
			/* buffer full so break after this pass */
			gpPointDC->nInputPoints = 0;
			
			/*
			   This division should only take place if the buffer
			   is nearly full
			*/
			TSPSpaceAvailable /= TSPWords;
			nBurst = TSPSpaceAvailable;
		}

		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;

		PackISPPoints(gpPoints, gpMat, nBurst, TSPAddr >> 1, TSPWords >> 1);

		/* Call TSP packer */
		pPerBuffn(gpPoints, gpMat, nBurst, pTSP);

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

/**********************************************************************/

void DirectPoints(	PSGLCONTEXT pContext,
				  	int			nPoints,
					sgl_uint16	*pPoints,
				  	PSGLVERTEX	pVertices )
{
	TEXAS_PRECOMP_STRUCT 	TPS;
	PIFUNCBLOCK				pFuncBlock;
	PPIR					fnPerPoly;
	sgl_uint32					uFuncBlockIndex;
	sgl_int32 				ISPSpaceAvailable;

	ISPSpaceAvailable = ( (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit) -
						  (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos) );

	if ( ISPSpaceAvailable < (nPoints * WORDS_PER_PLANE * 5) )
	{
		if (ISPSpaceAvailable > 0)
		{
			/* only once per frame! */
			nPoints = ISPSpaceAvailable / (WORDS_PER_PLANE * 5);
		}
		else
		{
			DPFDEV ((DBG_ERROR, "DirectPoints: ISP space overflowing"));
			return;
		}
	}

	if(pContext->u32Flags & SGLTT_DEPTHBIAS)
	{
		gfDepthBias = BOGUSINVZ_INCREMENT * (float)(pContext->uDepthBias);
	}
	else
	{
		gfDepthBias = 0.0f;
	}

	/* ini PDC
	 */
	gpPointDC->Context = *pContext; 
	gpPointDC->nInputPoints = nPoints;
	gpPointDC->pVertices = pVertices; 
	gpPointDC->pPoints		= pPoints;

#if PCX2 || PCX2_003
	if(gpPointDC->Context.uLineWidth < 1)
	{
		gpPointDC->uPixelWidth = 0;
		gpPointDC->fWidth = 1.0f;
		gpPointDC->fWidth2 = 1.0f;
	}
	else if(gpPointDC->Context.uLineWidth > 64 )
	{
		gpPointDC->uPixelWidth = 63;
		gpPointDC->fWidth = 64.0f;
		gpPointDC->fWidth2 = 64.0f * 64.0f;
	}
	else
	{
		gpPointDC->uPixelWidth = gpPointDC->Context.uLineWidth - 1;
		gpPointDC->fWidth = (float)gpPointDC->Context.uLineWidth;
		gpPointDC->fWidth2 = (float)(gpPointDC->Context.uLineWidth * 
									 gpPointDC->Context.uLineWidth);
	}
#else /* PCX1 */
	if(gpPointDC->Context.uLineWidth < 1)
	{
		gpPointDC->uPixelWidth = 0;
		gpPointDC->fWidth = 0.51f;
		gpPointDC->fWidth2 = 1.0f;
	}
	else if(gpPointDC->Context.uLineWidth > 64 )
	{
		gpPointDC->uPixelWidth = 63;
		gpPointDC->fWidth = 63.51f;
		gpPointDC->fWidth2 = 64.0f * 64.0f;
	}
	else
	{
		gpPointDC->uPixelWidth = gpPointDC->Context.uLineWidth - 1;
		gpPointDC->fWidth = (float)gpPointDC->Context.uLineWidth - 0.49f;
		gpPointDC->fWidth2 = (float)(gpPointDC->Context.uLineWidth * 
									 gpPointDC->Context.uLineWidth);
	}
#endif

	/*
	// init TSP control word to 0 or not fogged
	*/

	if (!gpPointDC->Context.bFogOn)
	{
		gpPointDC->TSPControlWord = MASK_DISABLE_FOG;
	}
	else
	{
		gpPointDC->TSPControlWord = 0;
		FogUsed = 1;
	}

	/* Need to set the Boguz Inv Z value if it is provided by the user.
	 * This is to be used for the ISP in the depth calculation.
	 */
	if (gpPointDC->Context.u32Flags & SGLTT_BOGUSINVZPROVIDED)
	{
		/* Read the bogus inverse Z provided.
		 * Scale the value correctly.
		 */
		gfBogusInvZ = (gpPointDC->Context.fBogusInvZ * fMinInvZ);
	}

	/*
	// work out which function block we are using
	*/
	uFuncBlockIndex = 
		gpPointDC->Context.u32Flags & (SGLTT_TEXTURE | SGLTT_HIGHLIGHT);

	/* we ignore gouraud which is bit 1, 
	** so the function blocks can be half the size
	*/
	uFuncBlockIndex >>= 1; 

	if (gpPointDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
	{
		pFuncBlock = NoVolFuncs;
	}
	else
	{
		if (gpPointDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
		{
			pFuncBlock = ShadowFuncs;
		}
		else
		{
			pFuncBlock = LightVolFuncs;
		}
	}

	
	/* Y coordinate is in LINES NOT normal REGIONS */
	gpPointDC->Context.FirstYRegion *= RegionInfo.YSize;
	
	if ( ( (gpPointDC->Context.LastYRegion+1) <= RegionInfo.NumYRegions ) ||
		 ( !RegionInfo.HasLeftOver ) )
	{
		/* Calculate accurate end of the Y Region */
		gpPointDC->Context.LastYRegion = ( ( gpPointDC->Context.LastYRegion + 1 ) *
									 RegionInfo.YSize ) - 1;
	}
	else
	{
		/* Last region was not full size, go to last line on screen */
		gpPointDC->Context.LastYRegion = ( ( RegionInfo.NumYRegions - 1 ) *
									 RegionInfo.YSize ) + RegionInfo.LeftOverY;
	}
	
	/* XRegion scaling can be achieved with a shift */
	gpPointDC->ShiftRegX = 5;		/* Start at 32 */
	
	if ( RegionInfo.XSize > (1<<gpPointDC->ShiftRegX) )
	{
		do
		{
			/* Larger shifts, for 64 perhaps */
			gpPointDC->ShiftRegX++;
		}
		while ( RegionInfo.XSize > (1<<gpPointDC->ShiftRegX) );
	}
	else
	{
		while ( RegionInfo.XSize < (1<<gpPointDC->ShiftRegX) )
		{
			/* Smaller Shifts ??? lets be future proof */
			gpPointDC->ShiftRegX--;
		}
	}
	
	ASSERT( ( RegionInfo.XSize == (1<<gpPointDC->ShiftRegX) ) );
	
	/* validate the texture if it's there */
	ProcessFlatTexFn = NULL;	

	if (gpPointDC->Context.u32Flags & SGLTT_TEXTURE)
	{
		sgl_bool fTexIsOK = TRUE;
		HTEXTURE hTexture;

		if (hTexture = GetTextureHandle (gpPointDC->Context.nTextureName))
		{			
			TPS.TexAddress = hTexture->TSPTextureControlWord;
			gpPointDC->TexAddress = hTexture->TSPTextureControlWord;
			gpPointDC->invTexSize = TexasGetInvTextureDimension (&TPS);
			gpPointDC->fWidthInvTex = gpPointDC->Context.uLineWidth * 
				gpPointDC->invTexSize;
		}
		else
		{
			fTexIsOK = FALSE;
		}
		
		if (fTexIsOK)
		{
			gpPointDC->TexAddress = TPS.TexAddress;
			TPS.LowWord = MASK_TEXTURE;
			
			if (gpPointDC->Context.bDoUVTimesInvW)
			{
				ProcessFlatTexFn = ProcessD3DPointTex;
				if (gpPointDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
				{
					gpPointDC->n32MipmapOffset = gpPointDC->Context.n32MipmapOffset;
					ProcessFlatTexFn = ProcessD3DPointTexMipMap;	
				}
			
			}
			else
			{
				ProcessFlatTexFn = ProcessLitePointTex;
				if (gpPointDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
				{
					gpPointDC->n32MipmapOffset = gpPointDC->Context.n32MipmapOffset;
					ProcessFlatTexFn = ProcessLitePointTexMipMap;	
				}
			
			}
		}
		else
		{
			/* not a valid texture handle */
			
			uFuncBlockIndex &= ~MASK_TEXTURE;
		}
		
	}
	else
	{
		/* Check for the allignment of the TSP parameters.
		 * No problem with textured triangles.
		 * Potential wastage but only when different triangles passed in single
		 * calls.
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += 0x2;
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos &= 0xFFFFFFFC;
	}


	#if DO_FPU_PRECISION

		SetupFPU ();
		
	#endif

	ASSERT ((uFuncBlockIndex & 0xFFFFFFF0) == 0);

#ifdef DLL_METRIC
	if (gpPointDC->TSPControlWord & MASK_TRANS)
	{
		nTransPolygonsInFrame += nPoints;
	}
	else
	{
		nOpaquePolygonsInFrame += nPoints;
	}
#endif
	
	pFuncBlock += uFuncBlockIndex;
	
	gpPointDC->TSPControlWord |= pFuncBlock->TSPControlWord;

	fnPerPoly = pFuncBlock->fnPerPoly;
	
	/* Reset the point index just incase it is used.
	 */
	nPointsIndex = 0;
	
	ProcessPoints (fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
	
	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif
}

/**********************************************************************/

void CALL_CONV sgltri_points ( PSGLCONTEXT  pContext,
							   int  nPoints,
							   PSGLVERTEX  pVertices )
{
#ifdef DLL_METRIC   	
   	nTotalPolygonsInFrame += nPoints;
#endif
		
	SGL_TIME_START(SGLTRI_TRIANGLES_TIME);
	
	/*
	// ----------------------
	// Check input parameters
	// ----------------------
	*/
	gu32UsedFlags |= pContext->u32Flags;

	if (nPoints == 0)
	{
		SglError(sgl_no_err);
	}
	else if (pContext == NULL || pVertices == NULL || nPoints < 0)
	{
		DPFDEV ((DBG_ERROR, "sgltri_points: calling with bad parameters"));
		SglError(sgl_err_bad_parameter);
	}
#if !WIN32
    else if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
	}
#endif
	/* all parameters ok */

	else
	{
		/* Need to call DirectPoints with a NULL face list.
		 * Only needed in D3D DrawPrimitive.
		 */
		DirectPoints (pContext, nPoints, NULL, pVertices);

		SglError(sgl_no_err);
	}
	SGL_TIME_STOP(SGLTRI_TRIANGLES_TIME)
}

/**********************************************************************/

