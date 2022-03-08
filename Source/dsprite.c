/******************************************************************************
 * Name         : dsprite.c
 * Author       : Graham Connor
 * Created      : 30/07/1997
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
 * $Log: dsprite.c,v $
 * Revision 1.23  1997/11/26  15:33:52  sxy
 * added DPFDEV messages.
 *
 * Revision 1.22  1997/11/24  15:14:02  gdc
 * removed reference to SGLTT_TEXAPI
 *
 * Revision 1.21  1997/11/20  15:33:27  sxy
 * added a DPFDEV message for bad parameters.
 *
 * Revision 1.20  1997/11/20  12:48:48  sxy
 * added timing code.
 *
 * Revision 1.19  1997/11/11  10:57:42  gdc
 * took range limiting of z-bias value
 *
 * Revision 1.18  1997/11/10  14:31:25  gdc
 * added depth bias
 *
 * Revision 1.17  1997/11/06  19:25:53  mjg
 * Removed a compiler warning.
 *
 * Revision 1.16  1997/10/28  10:35:47  erf
 * Removed ExtendTSPBuffer() routine and changed calls to
 * PVROSExtendTSPBuffer().
 *
 * Revision 1.15  1997/10/16  13:11:51  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.14  1997/10/15  11:56:55  sxy
 * added DPFDEVF messages.
 *
 * Revision 1.13  1997/09/24  15:58:51  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.12  1997/09/01  14:09:35  sks
 * Finished off the vertex fog stuff.  Seems to all work
 * but not completely tested/optimised etc.
 *
 * Revision 1.11  1997/08/29  16:37:51  sks
 * half-completed v-fog checkin.
 *
 * Revision 1.10  1997/08/29  10:43:49  sks
 * Finished off the V&G trans stuff.
 *
 * Revision 1.9  1997/08/28  15:36:02  sks
 * Started adding stuff for vertex and global translucency.
 *
 * Revision 1.8  1997/08/28  13:51:14  gdc
 * added stuff to follow which context flags are used
 *
 * Revision 1.7  1997/08/12  10:46:13  gdc
 * few performance tweaks for texturing and reduce size of base amount
 * processed to 32
 *
 * Revision 1.6  1997/08/11  14:25:50  sks
 * Bit of tidying and some possible optimisations
 *
 * Revision 1.3  1997/08/07  15:50:55  sks
 * Nearly all working(ish)
 *
 * Revision 1.2  1997/08/04  10:51:38  gdc
 * simple changed to get edge winding right without too much thought
 *
 * Revision 1.1  1997/08/01  09:28:05  gdc
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_DSPRITE

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
#include "vfogtab.h"
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

#endif

#define IBUFFERSIZE	 32

static float gfDepthBias;

extern sgl_uint32 gu32UsedFlags;
extern HLDEVICE gHLogicalDev;


extern float gfBogusInvZ;
extern sgl_bool FogUsed;
extern float fMinInvZ;
extern DEVICE_REGION_INFO_STRUCT RegionInfo;
static TRANS_REGION_DEPTHS_STRUCT gDepthInfo[IBUFFERSIZE];
extern sgl_uint32 TranslucentControlWord;
extern sgl_uint32 VertexFogControlWord;

#ifdef DLL_METRIC
extern sgl_uint32 nTotalPolygonsInFrame, nTransPolygonsInFrame, nOpaquePolygonsInFrame;
#endif

/*
  // We need to "retype" the floating sprite value as an Integer
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
	sgl_uint32	b;
	sgl_uint32	c;
	sgl_uint32	d;
	sgl_uint32	e;
	sgl_uint32	f;
	sgl_uint32	r;
	sgl_uint32  exp;
	sgl_uint32	mipmapd;
	
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

	sgl_uint32 nFogIndex;

} IMATERIAL, *PIMATERIAL;

typedef struct tagISPRITE
{
	float	fX;
	float	fY;
	float	fZ;

	float   fUOverW[2];
	float   fVOverW[2];

	int     nDelX;
	int     nDelY;
	float	fDelU;
	float	fDelV;

	union	
	{
		sgl_int8	b[4];
		sgl_uint32	u;
	}		reg;
	
	sgl_uint32	TSPControlWord;
	sgl_uint32	BaseColour;

} ISPRITE, *PISPRITE;


typedef struct tagPROCESSDATACONTEXTSPRITES
{
	int				nInputSprites;
	sgl_uint16      *pSprites;
	PSGLVERTEX		pVertices; 
	PSGLVERTEX		pCurVer1; 
	PSGLVERTEX		pCurVer2; 
	sgl_uint32			TSPControlWord;

	int				ShiftRegX;
	
	sgl_uint32			TexAddress;
	int				n32MipmapOffset;
	float 			invTexSize;
	sgl_uint32	    VFogTSPControlWord[2]; /* TSP control word for per vertex fog */

	sgl_uint32		u32GlobalTransBy85div128;
	
	SGLCONTEXT		Context;
	
} PROCESSDATACONTEXTSPRITES;

/* per polygon intermediate routines */
typedef void (* PPIR)(PISPRITE pSprite);

/* per buffer pack routines */
typedef void (* PBPR)(PISPRITE pSprite, PIMATERIAL pMat, sgl_uint32 nPolys,sgl_uint32 *pTSP);


static FOGHIGHLIGHT eExtraStatus[IBUFFERSIZE];

static ISPRITE		gpSprites[IBUFFERSIZE];
static IMATERIAL 	gpMat[IBUFFERSIZE];
static PIMATERIAL	gpMatCurrent = gpMat;
const PROCESSDATACONTEXTSPRITES gSpriteDC;
PROCESSDATACONTEXTSPRITES * const gpSpriteDC = (PROCESSDATACONTEXTSPRITES *) &gSpriteDC;

/**********************************************************************/

static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16);

static void ProcessHigh(PISPRITE pSprite);
static void ProcessHighTex(PISPRITE pSprite);
static void ProcessFlatShadOrLiVol(PISPRITE pSprite);
static void ProcessHighShadOrLiVol(PISPRITE pSprite);
static void ProcessFlatTexShadOrLiVol(PISPRITE pSprite);
static void ProcessHighTexShadOrLiVol(PISPRITE pSprite);

static void ProcessTrans(PISPRITE pSprite);
static void ProcessTexTrans(PISPRITE pSprite);

static void ProcessGTrans(PISPRITE pSprite);
static void ProcessTexGTrans(PISPRITE pSprite);

static void ProcessHighTrans(PISPRITE pSprite);
static void ProcessHighTexTrans(PISPRITE pSprite);

static void ProcessHighGTrans(PISPRITE pSprite);
static void ProcessHighTexGTrans(PISPRITE pSprite);

static void ProcessFlatTransShadOrLiVol(PISPRITE pSprite);
static void ProcessFlatTexTransShadOrLiVol(PISPRITE pSprite);

static void ProcessHighTransShadOrLiVol(PISPRITE pSprite);
static void ProcessHighTexTransShadOrLiVol(PISPRITE pSprite);

static void ProcessFlatGTransShadOrLiVol(PISPRITE pSprite);
static void ProcessFlatTexGTransShadOrLiVol(PISPRITE pSprite);

static void ProcessHighGTransShadOrLiVol(PISPRITE pSprite);
static void ProcessHighTexGTransShadOrLiVol(PISPRITE pSprite);

static void PackFlat(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTex(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHigh(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTex(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatShad(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighShad(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTexShad(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTexShad(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatLiVol(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighLiVol(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTexLiVol(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTexLiVol(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);

static void PackFlatTrans(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTrans(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);

static void PackFlatTransShad(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTransShad(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);

static void PackFlatTransLiVol(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTransLiVol(PISPRITE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);

static void ProcessFlatVFog(PISPRITE pSprite);
static void ProcessFlatTexVFog(PISPRITE pSprite);
static void ProcessHighVFog(PISPRITE pSprite);
static void ProcessHighTexVFog(PISPRITE pSprite);
static void ProcessFlatTransVFog(PISPRITE pSprite);
static void ProcessFlatTexTransVFog(PISPRITE pSprite);
static void ProcessHighTransVFog(PISPRITE pSprite);
static void ProcessHighTexTransVFog(PISPRITE pSprite);
static void ProcessFlatGTransVFog(PISPRITE pSprite);
static void ProcessFlatTexGTransVFog(PISPRITE pSprite);
static void ProcessHighGTransVFog(PISPRITE pSprite);
static void ProcessHighTexGTransVFog(PISPRITE pSprite);


/**********************************************************************/

static void (*ProcessFlatTexFn)( PISPRITE pSprite );

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

static IFUNCBLOCK NoVolFuncs[16] =
{
	#define BASE_FLAGS  0

	/*
	// No translucency
	*/
	{NULL, 							PackFlat,				2,	FLAGS(0, 0) }, 
	{NULL,		 					PackFlatTex, 			8,	FLAGS(2, 0) }, 
	{ProcessHigh, 					PackHigh, 				4,	FLAGS(4, 0) }, 
	{ProcessHighTex, 				PackHighTex, 			10,	FLAGS(6, 0) }, 

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE

	/*
	// Global alpha
	*/
	{NULL, 							PackFlatTrans,			8,	FLAGS(0, 0) }, 
	{NULL,		 					PackFlatTex, 			8,	FLAGS(2, 0) }, 
	{ProcessHigh, 					PackHighTrans, 			10,	FLAGS(4, 0) }, 
	{ProcessHighTex, 				PackHighTex, 			10,	FLAGS(6, 0) }, 

	/*
	// Vertex alpha
	*/
	{ProcessTrans, 					PackFlatTrans,			8,	FLAGS(0, 0) }, 
	{ProcessTexTrans, 				PackFlatTex,			8,	FLAGS(2, 0) },
	{ProcessHighTrans, 				PackHighTrans, 			10,	FLAGS(4, 0) },
	{ProcessHighTexTrans, 			PackHighTex,			10,	FLAGS(6, 0) },

	/*
	// Global and vertex alpha
	*/
	{ProcessGTrans, 				PackFlatTrans,			8,	FLAGS(0, 0) }, 
	{ProcessTexGTrans, 				PackFlatTex,			8,	FLAGS(2, 0) },
	{ProcessHighGTrans, 			PackHighTrans, 			10,	FLAGS(4, 0) },
	{ProcessHighTexGTrans, 			PackHighTex,			10,	FLAGS(6, 0) },
	#undef BASE_FLAGS
};


/*
// SHADOWS
*/

static IFUNCBLOCK ShadowFuncs[16] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency */
	{ProcessFlatShadOrLiVol,			PackFlatShad,		2,	FLAGS(0, 0) }, 
	{ProcessFlatTexShadOrLiVol, 		PackFlatTexShad,	8,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighShad, 		4,	FLAGS(4, 0) }, 
	{ProcessHighTexShadOrLiVol,			PackHighTexShad, 	10,	FLAGS(6, 0) }, 

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE

	/* global alpha */
	{ProcessFlatShadOrLiVol,			PackFlatTransShad,	8,	FLAGS(0, 0) }, 
	{ProcessFlatTexShadOrLiVol, 		PackFlatTexShad,	8,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighTransShad, 	10,	FLAGS(4, 0) }, 
	{ProcessHighTexShadOrLiVol,			PackHighTexShad, 	10,	FLAGS(6, 0) }, 

	/* vertex alpha */
	{ProcessFlatTransShadOrLiVol, 		PackFlatTransShad,	8,	FLAGS(0, 0) }, 
	{ProcessFlatTexTransShadOrLiVol,	PackFlatTexShad,	8,	FLAGS(2, 0) },
	{ProcessHighTransShadOrLiVol, 		PackHighTransShad,	10,	FLAGS(4, 0) },
	{ProcessHighTexTransShadOrLiVol,	PackHighTexShad,	10,	FLAGS(6, 0) },

	/* vertex + global alpha */
	{ProcessFlatGTransShadOrLiVol, 	    PackFlatTransShad,	8,	FLAGS(0, 0) }, 
	{ProcessFlatTexGTransShadOrLiVol, 	PackFlatTexShad,	8,	FLAGS(2, 0) },
	{ProcessHighGTransShadOrLiVol, 	    PackHighTransShad,	10,	FLAGS(4, 0) },
	{ProcessHighTexGTransShadOrLiVol,	PackHighTexShad,	10,	FLAGS(6, 0) },
	#undef BASE_FLAGS
};


/*
// LIGHT VOLUMES
*/

static IFUNCBLOCK LightVolFuncs[16] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency */
	{ProcessFlatShadOrLiVol,			PackFlatLiVol,			2,	FLAGS(0, 0) }, 
	{ProcessFlatTexShadOrLiVol, 		PackFlatTexLiVol,		8,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighLiVol, 			4,	FLAGS(4, 0) }, 
	{ProcessHighTexShadOrLiVol,			PackHighTexLiVol, 		10,	FLAGS(6, 0) }, 

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE

	/* global alpha */
	{ProcessFlatShadOrLiVol,			PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTexShadOrLiVol, 		PackFlatTexLiVol,		8,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighTransLiVol, 	10,	FLAGS(4, 0) }, 
	{ProcessHighTexShadOrLiVol,			PackHighTexLiVol, 		10,	FLAGS(6, 0) }, 

	/* vertex alpha */
	{ProcessFlatTransShadOrLiVol, 		PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTexTransShadOrLiVol,	PackFlatTexLiVol, 		8,	FLAGS(2, 0) },
	{ProcessHighTransShadOrLiVol, 		PackHighTransLiVol,		10,	FLAGS(4, 0) },
	{ProcessHighTexTransShadOrLiVol,	PackHighTexLiVol, 		10,	FLAGS(6, 0) },

	/* vertex + global alpha */
	{ProcessFlatGTransShadOrLiVol, 	    PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTexGTransShadOrLiVol, 	PackFlatTexLiVol, 		8,	FLAGS(2, 0) },
	{ProcessHighGTransShadOrLiVol, 	    PackHighTransLiVol,		10,	FLAGS(4, 0) },
	{ProcessHighTexGTransShadOrLiVol, 	PackHighTexLiVol, 		10,	FLAGS(6, 0) },
	#undef BASE_FLAGS
};


/*
// Per vertex fogging
*/

static IFUNCBLOCK NoVolVFogFuncs[16] =
{
	#define BASE_FLAGS  0

	/*
	// No translucency
	*/
	{ProcessFlatVFog, 				NULL, 			    2,	FLAGS(0, 0) }, 
	{ProcessFlatTexVFog, 			PackFlatTex, 		8,	FLAGS(2, 0) }, 
	{ProcessHighVFog, 				PackHigh, 			4,	FLAGS(4, 0) }, 
	{ProcessHighTexVFog, 			PackHighTex, 		10,	FLAGS(6, 0) }, 

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE

	/*
	// Global alpha
	*/ 	
	{ProcessFlatVFog, 				PackFlatTrans,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTexVFog, 			PackFlatTex, 		8,	FLAGS(2, 0) }, 
	{ProcessHighVFog, 				PackHighTrans, 		10,	FLAGS(4, 0) }, 
	{ProcessHighTexVFog, 			PackHighTex, 		10,	FLAGS(6, 0) }, 

	/*
	// Vertex alpha
	*/
	{ProcessFlatTransVFog, 			PackFlatTrans,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTexTransVFog, 		PackFlatTex,		8,	FLAGS(2, 0) },
	{ProcessHighTransVFog, 			PackHighTrans, 		10,	FLAGS(4, 0) },
	{ProcessHighTexTransVFog, 		PackHighTex,		10,	FLAGS(6, 0) },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlatGTransVFog, 		PackFlatTrans,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTexGTransVFog, 		PackFlatTex,		8,	FLAGS(2, 0) },
	{ProcessHighGTransVFog, 		PackHighTrans, 		10,	FLAGS(4, 0) },
	{ProcessHighTexGTransVFog, 		PackHighTex,		10,	FLAGS(6, 0) },
	#undef BASE_FLAGS
};


/**********************************************************************/

/* 
// Not sure if this is correct
*/ 
#define IMPLEMENT_VERTEX_TRANS									\
{																\
	sgl_uint32 Alpha;											\
	PSGLVERTEX pVert1,pVert2;									\
	pVert1 = gpSpriteDC->pCurVer1;								\
	pVert2 = gpSpriteDC->pCurVer2;								\
	Alpha = (pVert1->u32Colour >> 24) +							\
			(pVert2->u32Colour >> 24);							\
	Alpha <<= 8;												\
	Alpha &= (0xF << 13);										\
	Alpha  = (0xF << 13) - Alpha;								\
	pSprite->TSPControlWord |= Alpha;							\
}

/*********************************/

#define IMPLEMENT_VERTEX_GLOBAL_TRANS							\
{																\
	sgl_uint32 Alpha;											\
	PSGLVERTEX pVert1,pVert2;									\
	pVert1 = gpSpriteDC->pCurVer1;								\
	pVert2 = gpSpriteDC->pCurVer2;								\
	Alpha = (pVert1->u32Colour >> 24) +							\
			(pVert2->u32Colour >> 24) +							\
			(pVert1->u32Colour >> 24);							\
	Alpha *= gSpriteDC.u32GlobalTransBy85div128;				\
	Alpha &= (0xF << 13);										\
	Alpha  = (0xF << 13) - Alpha;								\
	pSprite->TSPControlWord |= Alpha;							\
}

/*********************************/

#define IMPLEMENT_VERTEX_FOG									\
{																\
	sgl_uint32 nFogIndex=0;										\
	PSGLVERTEX pVert1;											\
	pVert1 = gpSpriteDC->pCurVer1;								\
	nFogIndex = ((pVert1->u32Specular & 0xF0000000) >> 28); 	\
	pMat->nFogIndex = (~nFogIndex)&0xfffUL;                 	\
}

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

static int PackISPSpriteCore(sgl_uint32 *pPlaneMem, sgl_uint32 nSpritesInChunk, 
							sgl_uint32 *rTSPAddr, PISPRITE *rpSprites,
							int nIncrement, sgl_uint32 TSPIncrement)
{
	PISPRITE	pSprite = *rpSprites;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while(nSpritesInChunk--)
	{
		float fX, fY, fX1, fY1,	fZ;

#if PCX2 || PCX2_003
		fX = -pSprite->fX;
		fY = -pSprite->fY;
		fX1 = (pSprite->nDelX - fX);
		fY1 = (pSprite->nDelY - fY);
		fZ = pSprite->fZ + gfDepthBias;

		IW(pPlaneMem, 0+0, 0);
		IW(pPlaneMem, 1+0, 0);
		FW(pPlaneMem, 2+0, fZ);
		/* Pack instruction and TSP address.
		 */
		IW(pPlaneMem, 3+0, ( forw_visib_fp | (TSPAddr << 4)));

		/* do the edges of the sprite */
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
		fX = -pSprite->fX;
		fY = -pSprite->fY;
		fX1 = (pSprite->nDelX - fX);
		fY1 = (pSprite->nDelY - fY);
		fZ = pSprite->fZ;

		pPlaneMem[0+0] = forw_visib_fp | 
			((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |	SFLOAT_20BIT_ZERO;
		pPlaneMem[1+0] = (TSPAddr << 20) | SFLOAT_20BIT_ZERO;
		pPlaneMem[2+0] = (sgl_int32) (pSprite->fZ * FLOAT_TO_FIXED);

		/* do the edges of the sprite */
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
#pragma message("ISPTSP doesn't have sprites support")
#endif
		pPlaneMem += WORDS_PER_PLANE * 5;
		
		*((sgl_uint32 *) &pSprite) += nIncrement;
		TSPAddr += TSPIncrement;

	}

	return (0);
}

/**********************************************************************/

static int PackISPSprites(PISPRITE pSprite, PIMATERIAL pMat, 
						 int numSprites, sgl_uint32 TSPAddr, 
						 sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nSpritesInChunk;
	int 	nSprites;
	
	nSprites = numSprites;
				
	while (nSprites)
	{
		CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, 
									   WORDS_PER_PLANE * 5);

		if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
		{
			break;
		}
		else
		{
			DataSize = WORDS_PER_PLANE * 5 * nSprites;

			ChunkLimit = GetSabreLimit (CurrentPos);
			ChunkSize = ChunkLimit - CurrentPos;

			if (DataSize < ChunkSize)
			{
				nSpritesInChunk = nSprites;
			}
			else
			{
				nSpritesInChunk = ChunkSize / (WORDS_PER_PLANE * 5);
				
				if (!nSpritesInChunk)
				{
					break;
				}

				/* We need to re-calculate the datasize based on the number of sprites
				 * from the bunch we are processing.
				 */
				DataSize = WORDS_PER_PLANE * 5 * nSpritesInChunk;
			}
			
			AddRegionObjects((sgl_uint32 *) &(pSprite->reg), sizeof (ISPRITE),
							 5, nSpritesInChunk, CurrentPos, 
							 gDepthInfo, sizeof(TRANS_REGION_DEPTHS_STRUCT));

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			nSprites -= nSpritesInChunk;
			
			PackISPSpriteCore( &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos], 
							  nSpritesInChunk, 
							  &TSPAddr, &pSprite,
							  sizeof (ISPRITE), TSPIncrement );
		}
	}
	
	if (nSprites)
	{
		DPFDEV ((DBG_WARNING, "PackISPSprite: Out of ISP buffer space"));
	}

	return (numSprites - nSprites);
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

/*#define PACKTEX(w,x,y,z) \
w[2] = (x->Tex.r) & 0xffffUL; \
w[3] = 0x0L; \
w[4] = (z) | (( x->Tex.c) & 0xffffUL); \
w[5] = (x->Tex.b << 16) | (( x->Tex.a) & 0xffffUL); \
w[6] = (y) | (( x->Tex.f) & 0xffffUL); \
w[7] = (x->Tex.e << 16);
*/

/* New one which includes d and mipmapd (D) variables */
#define PACKTEX(w,x,y,z) \
w[2] = (x->Tex.mipmapd) | ((x->Tex.r) & 0xffffUL); \
w[3] = 0x0L; \
w[4] = (z) | (( x->Tex.c) & 0xffffUL); \
w[5] = (x->Tex.b << 16) | (( x->Tex.a) & 0xffffUL); \
w[6] = (y) | (( x->Tex.f) & 0xffffUL); \
w[7] = (x->Tex.e << 16) | (( x->Tex.d) & 0xffffUL);


#define PACKFLATSHADLV(a, b, c, d) \
a[0] = (b->TSPControlWord | ((c >> 16) & 0x000000FF)); \
a[1] = (c << 16) | d;

#define PACKFLATSHADLVTEX(a, b, c, d, e) \
a[0] = (b->TSPControlWord | ((c >> 16) & 0x000000FF))|(e->Tex.exp << SHIFT_EXPONENT); \
a[1] = (c << 16) | d;

/**********************************************************************/

static void PackFlat( PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		
		PACKFLAT(pTSP , pSprite, uBaseColour);
		
		pSprite++;
		pTSP += 2;
		
	}/*end while*/
}


/**********************************************************************/

static void PackFlatTrans( PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	sgl_uint32 TexAddr = TranslucentControlWord;
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;

		IW( pTSP, 0, (pSprite->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16));

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */
		
		pSprite++;
		pTSP += 8;
		
	}/*end while*/
}


/**********************************************************************/

static void PackHigh(PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		
		PACKFLAT(pTSP, pSprite, uBaseColour);
		PACKHIGH(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 4;
		
	}/*end while*/
}

/**********************************************************************/
static void PackHighTrans(PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{

	sgl_uint32 Highlight, TexAddr;
	int HighlightOffset;
	
	TexAddr = TranslucentControlWord;

	HighlightOffset = 8;
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		Highlight = (pMat->Highlight & 0xFFFF0000);
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;

		IW( pTSP, 0, (pSprite->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16));

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	    /* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		IW( pTSP, HighlightOffset, Highlight);
		
		pSprite++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatTex(PISPRITE pSprite, PIMATERIAL pMat, 
						sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/

	sgl_uint32 AddrHi, AddrLo;

	AddrHi = gSpriteDC.TexAddress & 0xffff0000;
	AddrLo = gSpriteDC.TexAddress << 16;

	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		
		PACKFLATTEX(pTSP, pSprite, uBaseColour, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pSprite++;
		pMat++;
		pTSP += 8;
		
	}/*end while*/
	
}

/**********************************************************************/

static void PackHighTex(PISPRITE pSprite, PIMATERIAL pMat, 
						sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/

	sgl_uint32 AddrHi, AddrLo;

	AddrHi = gSpriteDC.TexAddress & 0xffff0000;
	AddrLo = gSpriteDC.TexAddress << 16;

	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		
		PACKFLATTEX(pTSP, pSprite, uBaseColour, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatShad(PISPRITE pSprite, PIMATERIAL pMat, 
						 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pSprite->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pSprite, uFlat0Col24, uFlat1Col16);
		
		pSprite++;
		pTSP += 2;
		
	}/*end for*/
}

/**********************************************************************/

static void PackFlatTransShad( PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	sgl_uint32 TexAddr = TranslucentControlWord;
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;

 		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pSprite, uFlat0Col24, uFlat1Col16);

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */
		
		pSprite++;
		pTSP += 8;
		
	}/*end while*/
}


/**********************************************************************/

static void PackHighShad(PISPRITE pSprite, PIMATERIAL pMat, 
						 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pSprite->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pSprite, uFlat0Col24, uFlat1Col16);
		PACKHIGH(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 4;
		
	}/*end for*/
}

/**********************************************************************/
static void PackHighTransShad(PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	sgl_uint32  TexAddr;
	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	
	TexAddr = TranslucentControlWord;

	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;

		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pSprite, uFlat0Col24, uFlat1Col16);

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	    /* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		PACKHIGH(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}


/**********************************************************************/
static void PackFlatTexShad(PISPRITE pSprite, PIMATERIAL pMat, 
							sgl_uint32 numSprites, sgl_uint32 *pTSP)
{


	sgl_uint32 AddrHi, AddrLo;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

	AddrHi = gSpriteDC.TexAddress & 0xffff0000;
	AddrLo = gSpriteDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pSprite->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLVTEX(pTSP, pSprite, uFlat0Col24, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pSprite++;
		pMat++;
		pTSP += 8;
		
	}/*end for*/
}

/**********************************************************************/

static void PackHighTexShad(PISPRITE pSprite, PIMATERIAL pMat, 
							sgl_uint32 numSprites, sgl_uint32 *pTSP)
{


	sgl_uint32 AddrHi, AddrLo;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

	AddrHi = gSpriteDC.TexAddress & 0xffff0000;
	AddrLo = gSpriteDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pSprite->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLVTEX(pTSP, pSprite, uFlat0Col24, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 10;
		
	}/*end for*/
}

/**********************************************************************/

static void PackFlatLiVol(PISPRITE pSprite, PIMATERIAL pMat, 
						  sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		PACKFLATSHADLV(pTSP, pSprite, uBaseColour, uFlat1Col16);
		
		pSprite++;
		pTSP += 2;
		
	}/*end for*/

}

/**********************************************************************/

static void PackFlatTransLiVol( PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	sgl_uint32 TexAddr = TranslucentControlWord;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);
	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;

		PACKFLATSHADLV(pTSP, pSprite, uBaseColour, uFlat1Col16);

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */
		
		pSprite++;
		pTSP += 8;
		
	}/*end while*/
}


/**********************************************************************/

static void PackHighLiVol(PISPRITE pSprite, PIMATERIAL pMat, 
						  sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		PACKFLATSHADLV(pTSP, pSprite, uBaseColour, uFlat1Col16);
		PACKHIGH(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 4;
		
	}/*end for*/

}

/**********************************************************************/
static void PackHighTransLiVol(PISPRITE pSprite, PIMATERIAL pMat, 
					 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{

	sgl_uint32	TexAddr;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	TexAddr = TranslucentControlWord;

	/*
	// Go through the sprites. Move on to the next shading results and
	// and next sprite pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;

		PACKFLATSHADLV(pTSP, pSprite, uBaseColour, uFlat1Col16);
		PACKHIGH(pTSP, pMat);

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	    /* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		pSprite++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatTexLiVol(PISPRITE pSprite, PIMATERIAL pMat, 
							 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{
	sgl_uint32 AddrHi, AddrLo;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	
	AddrHi = gSpriteDC.TexAddress & 0xffff0000;
	AddrLo = gSpriteDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pSprite->BaseColour;
		PACKFLATSHADLVTEX(pTSP, pSprite, uBaseColour, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pSprite++;
		pMat++;
		pTSP += 8;
		
	}/*end for*/

}

/**********************************************************************/

static void PackHighTexLiVol(PISPRITE pSprite, PIMATERIAL pMat, 
							 sgl_uint32 numSprites, sgl_uint32 *pTSP)
{

	sgl_uint32 AddrHi, AddrLo;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	AddrHi = gSpriteDC.TexAddress & 0xffff0000;
	AddrLo = gSpriteDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numSprites--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pSprite->BaseColour;
		
		PACKFLATSHADLVTEX(pTSP, pSprite, uBaseColour, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pSprite++;
		pMat++;
		pTSP += 10;
		
	}/*end for*/
}

/**********************************************************************/
/**********************************************************************/

#define IMPLEMENT_HIGHLIGHT						 \
{												 \
	sgl_uint32 H = gSpriteDC.pCurVer1->u32Specular;	 \
	pMat->Highlight = ((H << 07) & 0x7C000000) | \
					  ((H << 10) & 0x03E00000) | \
					  ((H << 13) & 0x001F0000);  \
}

#define IMPLEMENT_SHADOW_LIGHTVOL \
	pMat->v.LightVolCol = gSpriteDC.Context.u.u32LightVolColour;

/**********************************************************************/
/**********************************************************************/

#define RESCALE 1
static void ProcessD3DSpriteTexMipMap(PISPRITE pSprite)
{

	PIMATERIAL pMat = gpMatCurrent;
	float adj_a, adj_b, adj_c, adj_d, adj_e, adj_f, adj_r, mipmapd;
	float largestT,largestB;
	flong topExponent;
	long exp;
	flong 	MipMap;
	long	MipMant,MipExp;
	float	compression,approx1;
#if !SLOW_FCMP || MULTI_FP_REG
	float 	FabsT1,FabsT2;
#endif
 	sgl_int32 flag;
	sgl_uint32 nDelXY = pSprite->nDelY * pSprite->nDelX;

	union {
		sgl_int32 i;
		float f;
	} intdU,intdV;

	intdU.f = pSprite->fDelU;
	intdV.f = pSprite->fDelV;

#if 0
	if ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))
	{
 		adj_a = pSprite->fDelU * pSprite->nDelY;
		adj_b = 0.0f;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fX * adj_a);

		adj_d = 0.0f;
		adj_e = pSprite->fDelV * pSprite->nDelX;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fY * adj_e);
	}
	else
	{
		adj_a = 0.0f;
		adj_b = pSprite->fDelU * pSprite->nDelX;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fY * adj_b);

		adj_d = pSprite->fDelV * pSprite->nDelY;
		adj_e = 0.0f;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fX * adj_d);
	}
#else

  /* The following was an attempt to speed up the adj calculations by removing the branch - but it's probably
 // slower than it was before as there are now always 16 fp mults compared to 8 per branch.  Making flag =
 // 0xFFFFFFFFL instead of 1 for the condition when the deltas (U and V) are different would allow you to replace
 // the first fp mults for a,b,c and d with ANDs and NOTs which reduces the number to 12.  Might be better.
 // Alternatively scrap it and use the stuff above. 
 */

	flag = ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))==0x80000000L;

	adj_a =  flag * intdU.f * pSprite->nDelY;
	adj_b = !flag * intdU.f * pSprite->nDelX;
	adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY
					- (gSpriteDC.pCurVer1->fY * adj_b)
					- (gSpriteDC.pCurVer1->fX * adj_a);

	adj_d = !flag * intdV.f * pSprite->nDelY;
	adj_e =  flag * intdV.f * pSprite->nDelX;
	adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY
					- (gSpriteDC.pCurVer1->fX * adj_d)
				 	- (gSpriteDC.pCurVer1->fY * adj_e);
 
#endif
	adj_r = gSpriteDC.invTexSize * nDelXY;

	/*
	**	calculate the MIP-map coefficient.
	*/
	
	/*
	** As either adj_a or adj_d will be zero we can short cut
	** Like wise adj_b or adj_e will be zero ...
	** And adj_r is always positive
	*/
	/* calculate the gradient in U and V with respect to X... sort of */

	approx1 = sfabs(adj_a + adj_d);

	/* calculate the gradient in U and V with respect to y... sort of */

	compression= sfabs(adj_b + adj_e);

	/* 
	// pick the larger of the distances and store in
	// compression.
	*/
	ASSERT(approx1>= 0.0f);
	ASSERT(compression>= 0.0f);
	ASSERT(adj_r >= 0.0f);

#if SLOW_FCMP && !MULTI_FP_REG
	if (FLOAT_TO_LONG(approx1) > FLOAT_TO_LONG(compression))
#else
	if (approx1>compression)
#endif
	{
		compression=approx1;
	}
	/* multiply thru by the adj_r term eliminated earlier */
	compression *= adj_r;

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
		LargestIntEquiv = FLOAT_TO_LONG(adj_a) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
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

	largestT= sfabs(adj_a);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT1=sfabs(adj_b);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_d);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_e);
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
	
	adj_a=adj_a * largestT;
	adj_b=adj_b * largestT;
	adj_c=adj_c * largestT;
	adj_d=adj_d * largestT;
	adj_e=adj_e * largestT;
	adj_f=adj_f * largestT;
	
	/* calculate a fast pow(2.0,floor())*largestT */
	
	topExponent.l=(exp+127)<<23;
	
	largestT=largestT * topExponent.f;
	
	adj_r=adj_r * largestT;


		/* solve the compression for 'n'.Compression is
		   pre-multiplied by pqr*pqr, so we only have to multiply by the
		   square of the rescale value */

		MipMap.f=compression*largestT*largestT;

		/* convert from IEEE to the TEXAS floating point format*/

		MipMant =(MipMap.l>>16)&0x7f;
		MipMant+=128; /*add in the implied 0.5*/
		/*126 because of the different decimal point*/
		MipExp = (MipMap.l>>23)-126 + gSpriteDC.n32MipmapOffset;


	
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
		adj_a=(float)((sgl_int32)adj_a >>(0-exp));
		adj_b=(float)((sgl_int32)adj_b >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_d=(float)((sgl_int32)adj_d >>(0-exp));
		adj_e=(float)((sgl_int32)adj_e >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a;
	pMat->Tex.b = (sgl_int32)adj_b;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.d = (sgl_int32)adj_d;
	pMat->Tex.e = (sgl_int32)adj_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.exp = exp;

	pMat->Tex.mipmapd = (sgl_int32)(((sgl_uint32) MipMant << SHIFT_PMIP_M) | ((sgl_uint32) MipExp  << SHIFT_PMIP_E));

}

/**********************************************************************/

static void ProcessLiteSpriteTexMipMap(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_a, adj_b, adj_c, adj_d, adj_e, adj_f, adj_r,mipmapd;
	float largestT,largestB;
	flong topExponent;
	long exp;
	flong 	MipMap;
	long	MipMant,MipExp;
	float	compression,approx1;
#if !SLOW_FCMP || MULTI_FP_REG
	float 	FabsT1,FabsT2;
#endif
	sgl_uint32 nDelXY;
				 
 	sgl_int32 flag;
	union {
		sgl_int32 i;
		float f;
	} intdU,intdV;

	intdU.f = pSprite->fDelU;
	intdV.f = pSprite->fDelV;

	nDelXY = pSprite->nDelY * pSprite->nDelX;

#if 0
	if ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))
	{
 		adj_a = pSprite->fDelU * pSprite->nDelY;
		adj_b = 0.0f;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fX * adj_a);

		adj_d = 0.0f;
		adj_e = pSprite->fDelV * pSprite->nDelX;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fY * adj_e);
	}
	else
	{
		adj_a = 0.0f;
		adj_b = pSprite->fDelU * pSprite->nDelX;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fY * adj_b);

		adj_d = pSprite->fDelV * pSprite->nDelY;
		adj_e = 0.0f;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fX * adj_d);
	}
#else

 /* The following was an attempt to speed up the adj calculations by removing the branch - but it's probably
 // slower than it was before as there are now always 16 fp mults compared to 8 per branch.  Making flag =
 // 0xFFFFFFFFL instead of 1 for the condition when the deltas (U and V) are different would allow you to replace
 // the first fp mults for a,b,c and d with ANDs and NOTs which reduces the number to 12.  Might be better.
 // Alternatively scrap it and use the stuff above. 
 */
	flag = ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))==0x80000000L;

	adj_a =  flag * intdU.f * pSprite->nDelY;
	adj_b = !flag * intdU.f * pSprite->nDelX;
	adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY
					- (gSpriteDC.pCurVer1->fY * adj_b)
					- (gSpriteDC.pCurVer1->fX * adj_a);

	adj_d = !flag * intdV.f * pSprite->nDelY;
	adj_e =  flag * intdV.f * pSprite->nDelX;
	adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY
					- (gSpriteDC.pCurVer1->fX * adj_d)
				 	- (gSpriteDC.pCurVer1->fY * adj_e);
#endif
	adj_r = gSpriteDC.invTexSize * gSpriteDC.pCurVer1->fInvW * nDelXY;

	/*
	**	calculate the MIP-map coefficient.
	*/

	/* 
	//approximate the pythagorean distance 
	//
	// IE dist(a,b) = |a| + |b| - 0.585786f*min(|a|,|b|)
	// I think this is supposed to be about 7% out worst case...
	//
	// One of either dudx or dvdx will be zero here ...
	*/

	approx1 = sfabs(adj_a + adj_d);
		
	/* calculate the gradient in U and V with respect to y. */

	compression = sfabs(adj_b + adj_e);
		
	ASSERT(approx1>= 0.0f);
	ASSERT(compression>= 0.0f);
	ASSERT(adj_r >= 0.0f);

#if SLOW_FCMP && !MULTI_FP_REG
	if (FLOAT_TO_LONG(approx1) > FLOAT_TO_LONG(compression))
#else
	if (approx1>compression)
#endif
	{
		compression=approx1;
	}
	compression *= sfabs(adj_r);

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
		LargestIntEquiv = FLOAT_TO_LONG(adj_a) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
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

	largestT= sfabs(adj_a);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT1=sfabs(adj_b);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_d);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_e);
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
	
	adj_a=adj_a * largestT;
	adj_b=adj_b * largestT;
	adj_c=adj_c * largestT;
	adj_d=adj_d * largestT;
	adj_e=adj_e * largestT;
	adj_f=adj_f * largestT;
	
	/* calculate a fast pow(2.0,floor())*largestT */
	
	topExponent.l=(exp+127)<<23;
	
	largestT=largestT * topExponent.f;
	
	adj_r=adj_r * largestT;


		/* solve the compression for 'n'.Compression is
		   pre-multiplied by pqr*pqr, so we only have to multiply by the
		   square of the rescale value */

		MipMap.f=compression*largestT*largestT;

		/* convert from IEEE to the TEXAS floating point format*/

		MipMant =(MipMap.l>>16)&0x7f;
		MipMant+=128; /*add in the implied 0.5*/
		/*126 because of the different decimal point*/
		MipExp = (MipMap.l>>23)-126 + gSpriteDC.n32MipmapOffset;




	
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
		adj_a=(float)((sgl_int32)adj_a >>(0-exp));
		adj_b=(float)((sgl_int32)adj_b >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_d=(float)((sgl_int32)adj_d >>(0-exp));
		adj_e=(float)((sgl_int32)adj_e >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a;
	pMat->Tex.b = (sgl_int32)adj_b;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.d = (sgl_int32)adj_d;
	pMat->Tex.e = (sgl_int32)adj_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.exp = exp;

	pMat->Tex.mipmapd = (sgl_int32)(((sgl_uint32) MipMant << SHIFT_PMIP_M) | ((sgl_uint32) MipExp  << SHIFT_PMIP_E));

}

/**********************************************************************/

static void ProcessD3DSpriteTex(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_c, adj_f, adj_p, adj_q, adj_r;
	float adj_a,adj_b,adj_d,adj_e,delU,delV;
	float largestT,largestB;
	flong topExponent;
	long exp;
 	sgl_int32 flag;
	sgl_uint32 nDelXY;

	union {
		sgl_int32 i;
		float f;
	} intdU,intdV;

	intdU.f = pSprite->fDelU;
	intdV.f = pSprite->fDelV;

	nDelXY = pSprite->nDelY * pSprite->nDelX;

#if 0
	if ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))
	{
 		adj_a = pSprite->fDelU * pSprite->nDelY;
		adj_b = 0.0f;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fX * adj_a);

		adj_d = 0.0f;
		adj_e = pSprite->fDelV * pSprite->nDelX;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY -
			(gSpriteDC.pCurVer1->fY * adj_e);
	}
	else
	{
		adj_a = 0.0f;
		adj_b = pSprite->fDelU * pSprite->nDelX;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fY * adj_b);

		adj_d = pSprite->fDelV * pSprite->nDelY;
		adj_e = 0.0f;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - 
			(gSpriteDC.pCurVer1->fX * adj_d);
	}

#else 
 /* The following was an attempt to speed up the adj calculations by removing the branch - but it's probably
 // slower than it was before as there are now always 16 fp mults compared to 8 per branch.  Making flag =
 // 0xFFFFFFFFL instead of 1 for the condition when the deltas (U and V) are different would allow you to replace
 // the first fp mults for a,b,c and d with ANDs and NOTs which reduces the number to 12.  Might be better.
 // Alternatively scrap it and use the stuff above. */

	flag = ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))==0x80000000L;

	adj_a =  flag * intdU.f * pSprite->nDelY;
	adj_b = !flag * intdU.f * pSprite->nDelX;
	adj_c = gSpriteDC.pCurVer1->fUOverW * pSprite->nDelY * pSprite->nDelX
					- (gSpriteDC.pCurVer1->fY * adj_b)
					- (gSpriteDC.pCurVer1->fX * adj_a);

	adj_d = !flag * intdV.f * pSprite->nDelY;
	adj_e =  flag * intdV.f * pSprite->nDelX;
	adj_f = gSpriteDC.pCurVer1->fVOverW * pSprite->nDelY * pSprite->nDelX
					- (gSpriteDC.pCurVer1->fX * adj_d)
				 	- (gSpriteDC.pCurVer1->fY * adj_e);

#endif
	adj_r = gSpriteDC.invTexSize * nDelXY;

#if RESCALE
	adj_c *= (1.0f/1023.0f);
	adj_f *= (1.0f/1023.0f);
	adj_r *= (1.0f/1023.0f);
#endif

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
		LargestIntEquiv = FLOAT_TO_LONG(adj_a) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}

		temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
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

	largestT= sfabs(adj_a);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT1=sfabs(adj_b);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_d);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_e);
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
	
	adj_a=adj_a * largestT;
	adj_b=adj_b * largestT;
	adj_c=adj_c * largestT;
	adj_d=adj_d * largestT;
	adj_e=adj_e * largestT;
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
		adj_a=(float)((sgl_int32)adj_a >>(0-exp));
		adj_b=(float)((sgl_int32)adj_b >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_d=(float)((sgl_int32)adj_d >>(0-exp));
		adj_e=(float)((sgl_int32)adj_e >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a;
	pMat->Tex.b = (sgl_int32)adj_b;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.d = (sgl_int32)adj_d;
	pMat->Tex.e = (sgl_int32)adj_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.exp = exp;


}					   

/**********************************************************************/

static void ProcessLiteSpriteTex(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;
	float adj_a, adj_b, adj_c, adj_d, adj_e, adj_f, adj_r;
	float largestT,largestB;
	flong topExponent;
	long exp;
 	sgl_int32 flag;
	sgl_uint32 nDelXY;

	union {	  
		sgl_int32 i;
		float f;
	} intdU,intdV;

	intdU.f = pSprite->fDelU;
	intdV.f = pSprite->fDelV;

	nDelXY = pSprite->nDelY * pSprite->nDelX;

#if 0
	if ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))
	{
 		adj_a = pSprite->fDelU * pSprite->nDelY;
		adj_b = 0.0f;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - (gSpriteDC.pCurVer1->fX * adj_a);

		adj_d = 0.0f;
		adj_e = pSprite->fDelV * pSprite->nDelX;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - (gSpriteDC.pCurVer1->fY * adj_e);
	}
	else
	{
		adj_a = 0.0f;
		adj_b = pSprite->fDelU * pSprite->nDelX;
		adj_c = gSpriteDC.pCurVer1->fUOverW * nDelXY - (gSpriteDC.pCurVer1->fY * adj_b);

		adj_d = pSprite->fDelV * pSprite->nDelY;
		adj_e = 0.0f;
		adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY - (gSpriteDC.pCurVer1->fX * adj_d);
	}
 
#else 
 /* The following was an attempt to speed up the adj calculations by removing the branch - but it's probably
 // slower than it was before as there are now always 16 fp mults compared to 8 per branch.  Making flag =
 // 0xFFFFFFFFL instead of 1 for the condition when the deltas (U and V) are different would allow you to replace
 // the first fp mults for a,b,c and d with ANDs and NOTs which reduces the number to 12.  Might be better.
 // Alternatively scrap it and use the stuff above. 
 */
	flag = ((intdU.i & 0x80000000L) ^ (intdV.i & 0x80000000L))==0x80000000L;

	adj_a =  flag * intdU.f * pSprite->nDelY;
	adj_b = !flag * intdU.f * pSprite->nDelX;
	adj_c =	gSpriteDC.pCurVer1->fUOverW * nDelXY 
		          - (gSpriteDC.pCurVer1->fY * adj_b)
		          - (gSpriteDC.pCurVer1->fX * adj_a);

	adj_d = !flag * intdV.f * pSprite->nDelY;
	adj_e =  flag * intdV.f * pSprite->nDelX;
	adj_f = gSpriteDC.pCurVer1->fVOverW * nDelXY
					- (gSpriteDC.pCurVer1->fX * adj_d)
				 	- (gSpriteDC.pCurVer1->fY * adj_e);

#endif
	adj_r = gSpriteDC.invTexSize * gSpriteDC.pCurVer1->fInvW * nDelXY;
 
 
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
		LargestIntEquiv = FLOAT_TO_LONG(adj_a) & FABS_MASK;
		
		
		/*
		// Get "fabs" of b and compare it with that of a
		// etc etc etc
		*/
		temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}

		temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
		if(temp > LargestIntEquiv)
		{
			LargestIntEquiv = temp;
		}
		
		temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
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

	largestT= sfabs(adj_a);
	
	/* 
	// make a temp copy of the result of the sfabs() because
	// this may be faster on compilers that call an sfabs
	// function
	*/
	FabsT1=sfabs(adj_b);
	if(FabsT1>largestT)
		largestT=FabsT1;
	
	FabsT2=sfabs(adj_c);
	if(FabsT2>largestT)
		largestT=FabsT2;

	FabsT2=sfabs(adj_d);
	if(FabsT2>largestT)
		largestT=FabsT2;
	
	FabsT2=sfabs(adj_e);
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
	
	adj_a=adj_a * largestT;
	adj_b=adj_b * largestT;
	adj_c=adj_c * largestT;
	adj_d=adj_d * largestT;
	adj_e=adj_e * largestT;
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
		adj_a=(float)((sgl_int32)adj_a >>(0-exp));
		adj_b=(float)((sgl_int32)adj_b >>(0-exp));
		adj_c=(float)((sgl_int32)adj_c >>(0-exp));
		adj_d=(float)((sgl_int32)adj_d >>(0-exp));
		adj_e=(float)((sgl_int32)adj_e >>(0-exp));
		adj_f=(float)((sgl_int32)adj_f >>(0-exp));
		
		exp=0;
	}
	

	pMat->Tex.a = (sgl_int32)adj_a;
	pMat->Tex.b = (sgl_int32)adj_b;
	pMat->Tex.c = (sgl_int32)adj_c;
	pMat->Tex.d = (sgl_int32)adj_d;
	pMat->Tex.e = (sgl_int32)adj_e;
	pMat->Tex.f = (sgl_int32)adj_f;
	pMat->Tex.r = (sgl_int32)adj_r;

	pMat->Tex.exp = exp;
}
/**********************************************************************/

static void ProcessTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

 	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessTexTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

 	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessGTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

 	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessTexGTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

 	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessHigh(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessHighTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessHighTex(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessHighTexTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessHighGTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessHighTexGTrans(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessFlatShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessFlatTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessFlatGTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessHighShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessHighGTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessFlatTexShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessFlatTexTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessFlatTexGTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessHighTexShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighTexTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessHighTexGTransShadOrLiVol(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessFlatVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessFlatTexVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessHighVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessHighTexVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessFlatTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessFlatTexTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessHighTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessHighTexTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessFlatGTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessFlatTexGTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessHighGTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static void ProcessHighTexGTransVFog(PISPRITE pSprite)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_VERTEX_FOG;
}

/**********************************************************************/

static int ProcessSpritesCore( PPIR pPerSpritefn, sgl_bool NewObject)
{
	sgl_int32 rX0, rY0, rY1, rX1;
	sgl_uint32 uDefaultType, uCurrentType/*, uOrBackType*/;
	static sgl_uint32 uDoneFirst[2];
	sgl_uint16 nVer1, nVer2;
	PSGLVERTEX pVert1, pVert2;
	PISPRITE	pSprite = gpSprites;
	int	ShiftRegX = gpSpriteDC->ShiftRegX;
	TRANS_REGION_DEPTHS_STRUCT* pGDepth;

	int k=0;

/*	pGDepth = &gDepthInfo[0];	*/

	if (gpSpriteDC->TSPControlWord & MASK_TRANS)
	{
		if (gSpriteDC.Context.u32Flags & SGLTT_OPAQUE)
		{
			uDefaultType = PACKED_TYPE_OPAQUETRANS;
			uCurrentType = PACKED_TYPE_OPAQUETRANS;
/*			uOrBackType = PACKED_TYPE_OPAQUETRANS; */
		}
		else if (gSpriteDC.Context.u32Flags & SGLTT_NEWPASSPERTRI)
		{
			uDefaultType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
			uCurrentType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
/*			uOrBackType = 0; */
		}
		else
		{
			if (NewObject)
			{
				uDoneFirst[0] = FALSE;
				uDoneFirst[1] = FALSE;
			}
			
			uDefaultType = PACKED_TYPE_TRANSLUCENT;
			uCurrentType = PACKED_TYPE_TRANSLUCENT;
/*			uOrBackType = PACKED_TRANSTYPE_BACKBIT; */
		}
	}
	else
	{
		uDefaultType = PACKED_TYPE_OPAQUE;
		uCurrentType = PACKED_TYPE_OPAQUE;
/*		uOrBackType = 0; */
	}



	while ( gpSpriteDC->nInputSprites )
	{

		gpSpriteDC->nInputSprites--;

		pGDepth = &gDepthInfo[k++];	

		nVer1 = gpSpriteDC->pSprites[gpSpriteDC->nInputSprites*2];
		nVer2 = gpSpriteDC->pSprites[gpSpriteDC->nInputSprites*2+1];

		gpSpriteDC->pCurVer1 = &(gpSpriteDC->pVertices[nVer1]);
		gpSpriteDC->pCurVer2 = &(gpSpriteDC->pVertices[nVer2]);
		pVert1 = gpSpriteDC->pCurVer1;
		pVert2 = gpSpriteDC->pCurVer2;

		pSprite->nDelX = (int)(pVert2->fX - pVert1->fX); /* X length of line */
		pSprite->nDelY = (int)(pVert2->fY - pVert1->fY); /* Y length of line */

		pSprite->fDelU = pVert2->fUOverW - pVert1->fUOverW;
		pSprite->fDelV = pVert2->fVOverW - pVert1->fVOverW;

		/* needs both sides non zero */
		if(pSprite->nDelX == 0 || pSprite->nDelY==0)
		{
			continue;
		}

		if(pSprite->nDelX > 0)
		{
			pSprite->fX = pVert1->fX;
			rX0 = (int)(pVert1->fX);
			rX1 = (int)(pVert2->fX);

			rX0 >>= ShiftRegX;
			rX1 >>= ShiftRegX;

		} 
		else
		{
			pSprite->fX = pVert2->fX;
			
			rX0 = (int)(pVert2->fX);
			rX1 = (int)(pVert1->fX);
			pSprite->nDelX *= -1;
			pSprite->fDelU *= -1;
			rX0 >>= ShiftRegX;
			rX1 >>= ShiftRegX;

		}

		if(pSprite->nDelY > 0)
		{
			pSprite->fY = pVert1->fY;
			rY0 = (int)(pVert1->fY);
			rY1 = (int)(pVert2->fY);
		}
		else
		{
			pSprite->fY = pVert2->fY;
			pSprite->nDelY *= -1;
			pSprite->fDelV *= -1;
			rY0 = (int)(pVert2->fY);
			rY1 = (int)(pVert1->fY);
		}

#if DEBUGDEV || DEBUG
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpSpriteDC->Context.bDoClipping))
		{
			if( (rX0 < gpSpriteDC->Context.FirstXRegion) ||
				(rY0 < gpSpriteDC->Context.FirstYRegion) ||
				(rX1 > gpSpriteDC->Context.LastXRegion)  ||
				(rY1 > gpSpriteDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessSpritesCore: need clipping but bDoClipping is FALSE"));			
			}
		}
#endif		

		/*
		// Check for Region clipping
		*/
		if (gpSpriteDC->Context.bDoClipping)
		{
			if (rX0 < gpSpriteDC->Context.FirstXRegion)
			{
				if ( rX1 < gpSpriteDC->Context.FirstXRegion )
				{
					/* Very Unlikely */
					continue;
				}
				
				rX0 = gpSpriteDC->Context.FirstXRegion;
			}
		
			if (rY0 < gpSpriteDC->Context.FirstYRegion)
			{
				if ( rY1 < gpSpriteDC->Context.FirstYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY0 = gpSpriteDC->Context.FirstYRegion;
			}
		
			if (rX1 > gpSpriteDC->Context.LastXRegion)
			{
				if ( rX0 > gpSpriteDC->Context.LastXRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rX1 = gpSpriteDC->Context.LastXRegion;
			}
		
			if (rY1 > gpSpriteDC->Context.LastYRegion)
			{
				if ( rY0 > gpSpriteDC->Context.LastYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY1 = gpSpriteDC->Context.LastYRegion;
			}
		}

		if (gpSpriteDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
			pSprite->fZ = gfBogusInvZ;

			gfBogusInvZ += BOGUSINVZ_INCREMENT;
		}
		else
		{
			pSprite->fZ = pVert1->fInvW * fMinInvZ;
		}
#if DEBUG || DEBUGDEV

		if (pVert1->fInvW > 10.0f || pVert2->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessSpritesCore: fInvW out of range"));
		}
#endif
		/* might be taking the wrong depth here - it should be the first one */
		pGDepth->BaseDepth = pSprite->fZ;


		
		if (uDefaultType == PACKED_TYPE_TRANSLUCENT)
		{
			sgl_uint32 uDFIndex = (uCurrentType & 1);

			if (!uDoneFirst[uDFIndex])
			{
				uCurrentType |= PACKED_TRANSTYPE_SETMARK;
				uDoneFirst[uDFIndex] = TRUE;
			}
		}


		/* determine region in which sprite falls - max of 1 regions */


		pSprite->reg.u = ENCODE_OBJXYDATA( uCurrentType, rX0, rY0, rX1, rY1 );
		pSprite->TSPControlWord = 	gSpriteDC.TSPControlWord;
		pSprite->BaseColour = 		pVert1->u32Colour;

		uCurrentType = uDefaultType;


		/* call the per sprite func here */
		if(pPerSpritefn)
		{
			pPerSpritefn(pSprite);
		}

		if(ProcessFlatTexFn)
		{
			ProcessFlatTexFn(pSprite);
		}
			
		if ( ++pSprite == &gpSprites[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}
	gpMatCurrent++;
	}/*end while*/

	/* Return number in intermediate buffer this time */
	return( (int) ( pSprite - gpSprites ) );

}

/**********************************************************************/

static void ProcessSprites( PPIR pPerSpritefn, PBPR pPerBuffn, sgl_uint32 TSPWords)
{
	/* function to process the context data for the sprites */
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;

	while ( gpSpriteDC->nInputSprites != 0 )
	{
		int nBurst;

		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */

		nBurst = ProcessSpritesCore( pPerSpritefn, NewObject );
		NewObject = FALSE;

		SGL_TIME_STOP(SGLTRI_PROCESS_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)

		/* Process the content of the buffer */
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PACKTRI_TIME)

		/*
			Work out how many triangle's worth of data we can
			actually put in there ...
		*/
		TSPSpaceAvailable = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit -
							PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
														  		
#if PCX2 || PCX2_003
		if ((gpSpriteDC->Context.u32Flags & SGLTT_VERTEXFOG))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
				{
					/* buffer full so break after this pass */
					gpSpriteDC->nInputSprites = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 8);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else
#endif
		{
			if (TSPSpaceAvailable < (nBurst * TSPWords))
			{
				/* buffer full so break after this pass */
				gpSpriteDC->nInputSprites = 0;
			
				/*
				   This division should only take place if the buffer
				   is nearly full
				*/
				TSPSpaceAvailable /= TSPWords;
				nBurst = TSPSpaceAvailable;
			}
		}

		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpSpriteDC->Context.u32Flags & SGLTT_VERTEXFOG))  
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculated for the planes STP pointer
			*/

			TSPInc = PackISPSpriteExtra (gpSprites, gpMat, nBurst,
									 TSPAddr >> 1, TSPWords >> 1);

			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			/* Call additonal type specific handler */
			pPerBuffn(gpSprites, gpMat, nBurst, pTSP);

			/* I'm not sure if the TSP offset is correct */
			PackSpriteExtra(gpSprites, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
			PackISPSprites(gpSprites, gpMat, nBurst, TSPAddr >> 1, TSPWords >> 1);

			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;

			/* Call TSP packer */
			pPerBuffn(gpSprites, gpMat, nBurst, pTSP);
		}

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)	
	}
}

/**********************************************************************/

void DirectSprites(PSGLCONTEXT  pContext,
				  int  nSprites,
				  sgl_uint16  pSprites[][2],
				  PSGLVERTEX  pVertices )
{
	TEXAS_PRECOMP_STRUCT 	TPS;
	PIFUNCBLOCK				pFuncBlock;
	PPIR					fnPerPoly;
	sgl_uint32					uFuncBlockIndex;
	sgl_int32 				ISPSpaceAvailable;
	sgl_uint32            	nExtraPlanes = 0;  

	ISPSpaceAvailable = ( (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit) -
						  (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos) );

	if(pContext->u32Flags & SGLTT_DEPTHBIAS)
	{
		gfDepthBias = BOGUSINVZ_INCREMENT * (float)(pContext->uDepthBias);
	}
	else
	{
		gfDepthBias = 0.0f;
	}

	if(pContext->u32Flags & SGLTT_VERTEXFOG)
	{
		nExtraPlanes++;
	}
	if ( ISPSpaceAvailable < (nSprites * WORDS_PER_PLANE * 
							  (5 + nExtraPlanes)) )
	{
		if (ISPSpaceAvailable > 0)
		{
			/* only once per frame! */

			if (!nExtraPlanes)
			{
				nSprites = ISPSpaceAvailable / (WORDS_PER_PLANE * 5);
			}
			else
			{
				nSprites = ISPSpaceAvailable / (WORDS_PER_PLANE * (5+nExtraPlanes));
			}
		}
		else
		{
			DPFDEV ((DBG_ERROR, "DirectSprites: ISP space overflowing"));				
			return;
		}
	}

	/* ini PDC */
	gpSpriteDC->Context = *pContext; 
	gpSpriteDC->nInputSprites = nSprites;
	gpSpriteDC->pSprites = (sgl_uint16*) pSprites; 
	gpSpriteDC->pVertices = pVertices; 

	/*
	// init TSP control word to 0 or not fogged
	*/

	if (!gpSpriteDC->Context.bFogOn)
	{
		gpSpriteDC->TSPControlWord = MASK_DISABLE_FOG;
	}
	else
	{
		gpSpriteDC->TSPControlWord = 0;
		FogUsed = 1;
	}

	/* Need to set the Boguz Inv Z value if it is provided by the user.
	 * This is to be used for the ISP in the depth calculation.
	 */
	if (gpSpriteDC->Context.u32Flags & SGLTT_BOGUSINVZPROVIDED)
	{
		/* Read the bogus inverse Z provided.
		 * Scale the value correctly.
		 */
		gfBogusInvZ = (gpSpriteDC->Context.fBogusInvZ * fMinInvZ);
	}

	/*
	// work out which function block we are using
	*/
	uFuncBlockIndex = 
		gpSpriteDC->Context.u32Flags & (SGLTT_TEXTURE | SGLTT_HIGHLIGHT);

	/* we ignore gouraud which is bit 1, 
	** so the function blocks can be half the size
	*/
	uFuncBlockIndex >>= 1; 

	if (gpSpriteDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
	{
		pFuncBlock = NoVolFuncs;

		if(gpSpriteDC->Context.u32Flags & SGLTT_VERTEXFOG)
		{
			sgl_uint32 uFogColour;

			uFogColour =((sgl_uint32)(gpSpriteDC->Context.fFogR*255)<<16) |
						((sgl_uint32)(gpSpriteDC->Context.fFogG*255)<<8) |
						((sgl_uint32)(gpSpriteDC->Context.fFogB*255));

			gpSpriteDC->VFogTSPControlWord[0] = ((uFogColour >> 16) & 0x000000FF);
			gpSpriteDC->VFogTSPControlWord[0] |= (MASK_TEXTURE | MASK_TRANS | MASK_DISABLE_FOG);
			gpSpriteDC->VFogTSPControlWord[1] = (uFogColour << 16);
			pFuncBlock = NoVolVFogFuncs;
		}
	}
	else
	{
		if (gpSpriteDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
		{
			pFuncBlock = ShadowFuncs;
		}
		else
		{
			pFuncBlock = LightVolFuncs;
		}
	}
	
	/* set up for global translucency if necessary */

	if ((gpSpriteDC->Context.u32Flags & SGLTT_GLOBALTRANS) 
		&& (gpSpriteDC->Context.u32GlobalTrans < 0xF0))
	{
		uFuncBlockIndex |= 0x04;		

		if (!(gpSpriteDC->Context.u32Flags & SGLTT_VERTEXTRANS))
		{
			sgl_uint32 Alpha = 0xF - (gpSpriteDC->Context.u32GlobalTrans >> 4);
			gpSpriteDC->TSPControlWord |= Alpha << 13;
		}
	}
	
	/* 
	// set up for vertex trans 
	// To speed up the Vertex translucency averaging calc,
	// pre-compute 85 * the global trans value / 128.
	*/
	if (gpSpriteDC->Context.u32Flags & SGLTT_VERTEXTRANS)
	{
		uFuncBlockIndex |= 0x8;
/*		gpSpriteDC->u32GlobalTransBy85div128 = (85 * gpSpriteDC->Context.u32GlobalTrans) >> 7; */
		gpSpriteDC->u32GlobalTransBy85div128 = gpSpriteDC->Context.u32GlobalTrans;
	}
	
	/* Y coordinate is in LINES NOT normal REGIONS */
	gpSpriteDC->Context.FirstYRegion *= RegionInfo.YSize;
	
	if ( ( (gpSpriteDC->Context.LastYRegion+1) <= RegionInfo.NumYRegions ) ||
		 ( !RegionInfo.HasLeftOver ) )
	{
		/* Calculate accurate end of the Y Region */
		gpSpriteDC->Context.LastYRegion = ( ( gpSpriteDC->Context.LastYRegion + 1 ) *
									 RegionInfo.YSize ) - 1;
	}
	else
	{
		/* Last region was not full size, go to last line on screen */
		gpSpriteDC->Context.LastYRegion = ( ( RegionInfo.NumYRegions - 1 ) *
									 RegionInfo.YSize ) + RegionInfo.LeftOverY;
	}
	
	/* XRegion scaling can be achieved with a shift */
	gpSpriteDC->ShiftRegX = 5;		/* Start at 32 */
	
	if ( RegionInfo.XSize > (1<<gpSpriteDC->ShiftRegX) )
	{
		do
		{
			/* Larger shifts, for 64 perhaps */
			gpSpriteDC->ShiftRegX++;
		}
		while ( RegionInfo.XSize > (1<<gpSpriteDC->ShiftRegX) );
	}
	else
	{
		while ( RegionInfo.XSize < (1<<gpSpriteDC->ShiftRegX) )
		{
			/* Smaller Shifts ??? lets be future proof */
			gpSpriteDC->ShiftRegX--;
		}
	}
	
	ASSERT( ( RegionInfo.XSize == (1<<gpSpriteDC->ShiftRegX) ) );
	
	/* validate the texture if it's there */
	ProcessFlatTexFn = NULL;	

	if(gpSpriteDC->Context.u32Flags & SGLTT_GLOBALTRANS || 
	   gpSpriteDC->Context.u32Flags & SGLTT_VERTEXTRANS)
	{

#if 0
		uFuncBlockIndex |= 4; /* flat shading only just now */
#endif
	}


	if (gpSpriteDC->Context.u32Flags & SGLTT_TEXTURE)
	{
		sgl_bool fTexIsOK = TRUE;
		HTEXTURE hTexture;
		
		if (hTexture = GetTextureHandle (gpSpriteDC->Context.nTextureName))
		{			
			TPS.TexAddress = hTexture->TSPTextureControlWord;
			gpSpriteDC->TexAddress = hTexture->TSPTextureControlWord;
			gpSpriteDC->invTexSize = TexasGetInvTextureDimension (&TPS);
		}
		else
		{
			fTexIsOK = FALSE;
		}
		
		if (fTexIsOK)
		{
			gpSpriteDC->TexAddress = TPS.TexAddress;
			TPS.LowWord = MASK_TEXTURE;
			
			if (TPS.TexAddress & MASK_4444_555)
			{
				gpSpriteDC->TSPControlWord |= MASK_TRANS;
				/* if it is a translucent texture we should make sure and
				** flatten off smooth shading if there are shadows or L-Vs
				** i.e. treat it as if global translucency is set 
				*/
				#define SMFLATTENMASK	(SGLTT_GOURAUD | SGLTT_GLOBALTRANS | SGLTT_VERTEXTRANS)
				#define SMFLATTENRESULT	(SGLTT_GOURAUD)
				
				if (((gpSpriteDC->Context.u32Flags & SMFLATTENMASK) == SMFLATTENRESULT)
					&& (gpSpriteDC->Context.eShadowLightVolMode != NO_SHADOWS_OR_LIGHTVOLS))
				{
					/* 
					   no need to worry about the actual global trans
					   value in TSPControlWord, it's 0 by default
					*/
#if 0
					uFuncBlockIndex |= 0x08;
#endif
				}
			}

			if (gpSpriteDC->Context.bDoUVTimesInvW)
			{
				ProcessFlatTexFn = ProcessD3DSpriteTex;
				if (gpSpriteDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
				{
					gpSpriteDC->n32MipmapOffset = gpSpriteDC->Context.n32MipmapOffset;
					ProcessFlatTexFn = ProcessD3DSpriteTexMipMap;	
				}
			
			}
			else
			{
				ProcessFlatTexFn = ProcessLiteSpriteTex;
				if (gpSpriteDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
				{
					gpSpriteDC->n32MipmapOffset = gpSpriteDC->Context.n32MipmapOffset;
					ProcessFlatTexFn = ProcessLiteSpriteTexMipMap;	
				}
			
			}
		}
		else
		{
			/* not a valid texture handle */
			
			uFuncBlockIndex &= ~MASK_TEXTURE;
		}
		
	}

	#if DO_FPU_PRECISION

		SetupFPU ();
		
	#endif

	ASSERT ((uFuncBlockIndex & 0xFFFFFFF0) == 0);

#ifdef DLL_METRIC
	if (gpSpriteDC->TSPControlWord & MASK_TRANS)
	{
		nTransPolygonsInFrame += nSprites;
	}
	else
	{
		nOpaquePolygonsInFrame += nSprites;
	}
#endif
	
	pFuncBlock += uFuncBlockIndex;
	
	gpSpriteDC->TSPControlWord |= pFuncBlock->TSPControlWord;

	fnPerPoly = pFuncBlock->fnPerPoly;
	
	ProcessSprites (fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
	
	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif
}

/**********************************************************************/

void sgltri_sprites ( PSGLCONTEXT  pContext,
					int  nSprites,
					sgl_uint16  pSprites[][2],
					PSGLVERTEX  pVertices )
{
#ifdef DLL_METRIC   	
   	nTotalPolygonsInFrame += nSprites;
#endif
		
	SGL_TIME_START(SGLTRI_TRIANGLES_TIME);

	/*
	// ----------------------
	// Check input parameters
	// ----------------------
	*/
	gu32UsedFlags |= pContext->u32Flags;

	if (nSprites == 0)
	{
		SglError(sgl_no_err);
	}
	else if (pContext == NULL || pVertices == NULL || pSprites==NULL || nSprites< 0)
	{
		DPFDEV ((DBG_ERROR, "sgltri_sprites: calling with bad parameters"));	
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
		DirectSprites (pContext, nSprites, pSprites, pVertices);
		SglError(sgl_no_err);
	}
	
	SGL_TIME_STOP(SGLTRI_TRIANGLES_TIME)
}


static int PackSpriteExtra(PISPRITE pSprite, PIMATERIAL pMat, sgl_uint32 nSprites, sgl_uint32 TSPInc, sgl_uint32 *pTSP)
{
	sgl_uint32 FlatTexAddr;
	sgl_uint32 TSP0;
	sgl_uint32 TSP1;
	
	FlatTexAddr = TranslucentControlWord;
	TSP0 = gSpriteDC.VFogTSPControlWord[0];
	TSP1 = gSpriteDC.VFogTSPControlWord[1];

	while (nSprites--)
	{
		if (pMat->nFogIndex)
		{
			sgl_uint32 nFogIndex = pMat->nFogIndex;

			/* if the fog is constant over a poly shortcut it and
			** use the translucent texture
			*/
				sgl_uint32 Alpha;

				Alpha = (nFogIndex & 0x00fUL);
				Alpha = ~Alpha &0xfUL;

				IW( pTSP, 0, (TSP0 | Alpha << 13));
				IW( pTSP, 1, TSP1);
				IW( pTSP, 2, 1);						/* r */
				IW( pTSP, 3, 0);						/* q<<16 | p */
				IW( pTSP, 4, (FlatTexAddr << 16));			/* addr<16 | c */
				IW( pTSP, 5, 0);						/* b<<16 | a */
				IW( pTSP, 6, (FlatTexAddr & 0xFFFF0000));   /* addr&FFFF0000 | f */
				IW( pTSP, 7, 0);						/* e<<16 | d */

			pTSP += TSPInc;
		}

		/* overwrite the indices to stop them being accidently picked up
		** if two lots of tris end up in one buffers worth
		*/
		pMat->nFogIndex=0;
			
		pSprite++;
		pMat++;
		
	}/*end for*/

	return (nSprites);
}

/**********************************************************************/

static int PackISPSpriteCoreExtra(sgl_uint32 *pPlaneMem,
							sgl_uint32 nSpritesInChunk, 
							sgl_uint32 *rTSPAddr,
							PISPRITE *rpSprites,
							int nIncrement,
							PIMATERIAL  *rpMat,
					  		int         nMatInc,
							sgl_uint32 TSPIncrement,
							sgl_uint32  *rTSPOffsetAddr,
							sgl_uint32  TSPIncrementExtra)
{
	PISPRITE	pSprite = *rpSprites;
	sgl_uint32	TSPAddr = *rTSPAddr;
	PIMATERIAL	pMat = *rpMat;
	sgl_uint32	TSPOffsetAddr = *rTSPOffsetAddr;
	PFOGHIGHLIGHT epExtras = eExtraStatus;

	while(nSpritesInChunk--)
	{
		float fX, fY, fX1, fY1,	fZ;

#if PCX2 || PCX2_003
		fX = -pSprite->fX;
		fY = -pSprite->fY;
		fX1 = (pSprite->nDelX - fX);
		fY1 = (pSprite->nDelY - fY);
		fZ = pSprite->fZ;

		IW(pPlaneMem, 0+0, 0);
		IW(pPlaneMem, 1+0, 0);
		FW(pPlaneMem, 2+0, fZ);
		/* Pack instruction and TSP address.
		 */
		IW(pPlaneMem, 3+0, ( forw_visib_fp | (TSPAddr << 4)));

		/* do the edges of the sprite */
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
		fX = -pSprite->fX;
		fY = -pSprite->fY;
		fX1 = (pSprite->nDelX - fX);
		fY1 = (pSprite->nDelY - fY);
		fZ = pSprite->fZ;

		pPlaneMem[0+0] = forw_visib_fp | 
			((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |	SFLOAT_20BIT_ZERO;
		pPlaneMem[1+0] = (TSPAddr << 20) | SFLOAT_20BIT_ZERO;
		pPlaneMem[2+0] = (sgl_int32) (pSprite->fZ * FLOAT_TO_FIXED);

		/* do the edges of the sprite */
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
#pragma message("ISPTSP doesn't have sprites support")
#endif
		pPlaneMem += WORDS_PER_PLANE * 5;
	
		TSPAddr += TSPIncrement;

		
		/* if the fog plane is added ... */
		if(pMat->nFogIndex)
		{

#if PCX2 || PCX2_003
			FW( pPlaneMem, 0+0, 0);
			FW( pPlaneMem, 1+0, 0);
			FW( pPlaneMem, 2+0, fZ);
			IW( pPlaneMem, 3+0, (forw_visib | (TSPOffsetAddr << 4)));

#elif PCX1
			pPlaneMem[0+0] = forw_visib_fp | 
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |	SFLOAT_20BIT_ZERO;
			pPlaneMem[1+0] = (TSPOffsetAddr << 20) | SFLOAT_20BIT_ZERO;
			pPlaneMem[2+0] = (sgl_int32) (pSprite->fZ * FLOAT_TO_FIXED);
#endif

			*epExtras |= VERTEX_FOG;
			pPlaneMem += WORDS_PER_PLANE;
			TSPOffsetAddr += TSPIncrementExtra;
		}

		*((sgl_uint32 *) &pSprite) += nIncrement;
		*((sgl_uint32 *) &pMat) += nMatInc;
		epExtras++;
	}
	
	*rpSprites = pSprite;
	*rpMat = pMat;
	*rTSPAddr = TSPAddr;
	*rTSPOffsetAddr = TSPOffsetAddr;


	return (0);

 }

 /**********************************************************************/


static int PackISPSpriteExtra (PISPRITE rpSprite, PIMATERIAL rpMat, int nSprites,
					sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	DataSize, CurrentPos, ChunkSize, ChunkLimit, nPolysInChunk;
	int 	nPolys, nExtraVertices, nBurst, i, nObjPlanesInWords;
	const sgl_uint32  TSPIncrementExtra = 8>>1; /* both the VFog and specular are flat textured */
	int TSPSize = 0;
	sgl_uint32 TSPOffsetAddr;
	PIMATERIAL pMat = rpMat;
	PISPRITE pSprite = rpSprite;
	sgl_uint32 DataInc;

	nPolys = nSprites;

	TSPOffsetAddr = TSPAddr + TSPIncrement*nPolys;

	nBurst = nPolys;

	while (nBurst)
	{
		nObjPlanesInWords = WORDS_PER_PLANE * (5) + 
			((rpMat->nFogIndex)?WORDS_PER_PLANE:0);

		CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, nObjPlanesInWords);

		if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
		{
			return (nSprites - nPolys);
		}
		else
		{
			DataInc = 0;
			DataSize = nBurst * (5) * WORDS_PER_PLANE;
			TSPSize += nBurst * TSPIncrement;
			pMat = rpMat; /* points to unprocessed materials */
			for(i=0; i<nBurst; i++, pMat++)
			{
				DataInc += ((pMat->nFogIndex)?1:0);
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
				pMat = rpMat; /* points to unprocessed materials */
				for(nPolysInChunk=0; ChunkSize>(WORDS_PER_PLANE * (5)); 
					nPolysInChunk++, DataSize+=DataInc, pMat++)
				{
					DataInc = WORDS_PER_PLANE * (5); 
					DataInc += ((pMat->nFogIndex)?WORDS_PER_PLANE:0);
					ChunkSize -= DataInc;

				}
				
				if (!nPolysInChunk)
				{

					return (nSprites - nPolys);
				}
			}				
			
			nBurst -= nPolysInChunk;


			/* require a safe copy of the pointer to the current triangle
			** for adding regions, as isp packing updates the pTri pointer
			*/
			pSprite = rpSprite;
				
			/*
			// If we aren't guaranteed that vertices are on screen, then
			// use the safer pack routine
			*/
		  	PackISPSpriteCoreExtra (&(PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos]), 
									nPolysInChunk, &TSPAddr,
									&rpSprite, sizeof(ISPRITE),
									&rpMat, sizeof (IMATERIAL),  
									TSPIncrement,
									&TSPOffsetAddr, TSPIncrementExtra );

			/* add the objects after the extra status has been calculated */
			AddRegionObjectsExtra((sgl_uint32 *) &(pSprite->reg), sizeof (ISPRITE),
								  5, nPolysInChunk, CurrentPos, 
								  gDepthInfo + (nSprites - nPolys), 
								  sizeof(TRANS_REGION_DEPTHS_STRUCT),
								  eExtraStatus);
			
		  	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;			
						
		}
		
	}
		
/*	if (nPolys)
	{
		DPF ((DBG_WARNING, "PackSprite: Out of ISP buffer space"));
	}
*/
	return (TSPSize);

} /* PackISPSpriteExtra */



/**********************************************************************/

