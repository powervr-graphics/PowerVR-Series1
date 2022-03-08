/******************************************************************************
 * Name         : dlines.c
 * Author       : Graham Connor
 * Created      : 10/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Low level API top level code for PowerVR Support for 
 *                D3D Line rendering mode
 *                
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dlines.c,v $
 * Revision 1.46  1997/11/24  15:17:24  gdc
 * removed reference to SGLTT_TEXAPI
 *
 * Revision 1.45  1997/11/20  15:34:38  sxy
 * sdded timing code.
 *
 * Revision 1.44  1997/11/17  12:49:10  erf
 * Changed TSP increment of PackHighShad in ShadowFuncs from 6 to 4.
 *
 * Revision 1.43  1997/11/11  10:57:21  gdc
 * took range limiting of z-bias value
 *
 * Revision 1.42  1997/11/10  14:31:01  gdc
 * added depth bias
 *
 * Revision 1.41  1997/10/16  11:49:14  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.40  1997/10/15  13:45:57  sxy
 * added DPFDEVF messages.
 *
 * Revision 1.39  1997/09/25  14:43:15  sxy
 * added DPFDEV messages.
 *
 * Revision 1.38  1997/09/24  14:48:21  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.37  1997/09/17  15:06:16  sxy
 * Removed warnings.
 *
 * Revision 1.36  1997/09/03  16:18:19  sks
 * Added vertex & global translucency.
 * Put in:  Sheila's uDefaultType in ProcessLinesCore.
 *          Graham's negated DelX,DelY fix in ProcessLinesCore.
 *
 * Revision 1.35  1997/09/02  14:15:19  sks
 * Added debug warning for out of range (>10.0) fInvW.
 *
 * Revision 1.34  1997/08/28  13:52:01  gdc
 * added stuff to follow which context flags are used
 *
 * Revision 1.33  1997/08/26  17:09:33  erf
 * Added TSP flat shaded highlight fix for lines.
 *
 * Revision 1.32  1997/08/20  16:28:05  erf
 * Change so include D3D DrawPrimitive support. Basically a increment
 * is specified which moves the vertex index along for the next line.
 *
 * Revision 1.31  1997/08/08  19:41:52  mjg
 * Added PCX2_003 compatibility (blend modes)
 *
 * Revision 1.30  1997/07/21  17:16:59  gdc
 * fix for vingetting when shadow or LV's on
 *
 * Revision 1.29  1997/07/14  19:27:20  mjg
 * Added the PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.28  1997/07/09  07:23:46  erf
 * When packing the points and chunking in required we need to
 * recalculate the datasize so that we start at the page break
 * and not waste ISP plane space.
 *
 * Revision 1.27  1997/06/18  14:37:45  mjg
 * Replaced windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.26  1997/06/06  10:51:48  erf
 * Moved BOGUSZ_INCREMENT definition to sgl.h and renamed to
 * BOGUSINVZ_INCREMENT.
 *
 * Revision 1.25  1997/06/04  15:29:16  gdc
 * removed compiler warnings
 *
 * Revision 1.24  1997/06/04  13:51:08  erf
 * Added facility for user to set the bogus inverse Z value. New flag
 * and variable (SGLTT_BOGUSINVZPROVIDED and fBogusInvZ) defined in
 * the context structure.
 *
 * Revision 1.23  1997/06/03  15:37:50  gdc
 * added region clipping
 *
 * Revision 1.22  1997/06/03  10:43:49  gdc
 * added gouruad line - even dirtier than translucent
 *
 * Revision 1.20  1997/05/29  14:54:10  gdc
 * put in translucent lines as a workaround - this should not go
 * into builds yet
 *
 * Revision 1.19  1997/05/27  14:31:16  jop
 * Added a check for available ISP space.
 *
 * Revision 1.18  1997/05/22  14:23:10  erf
 * Added bogus Z stuff when Z buffering is disabled.
 *
 * Revision 1.17  1997/05/19  17:29:54  gdc
 * masked of all features but flat, and put in a default min and max line width
 *
 * Revision 1.16  1997/05/19  09:01:30  gdc
 * moved GetSabreLimit to parambuff.h
 *
 * Revision 1.15  1997/05/16  18:36:53  erf
 * Changed struct definition of PROCESSDATACONTEXT to PROCESSDATACONTEXTPOINTS
 * to avoid confusion in winice.
 *
 * Revision 1.14  1997/05/16  17:19:27  erf
 * Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
 *
 * Revision 1.13  1997/05/13  17:03:57  mjg
 * Replaced UINT32's with sgl_uint32's and the like/
 *
 * Revision 1.12  1997/05/12  18:45:48  mjg
 * Got PCX1 working and fixed PCX2.
 *
 * Revision 1.11  1997/05/12  15:59:52  gdc
 * added pcx1 packing - compiles ok, but not tested
 *
 * Revision 1.10  1997/05/12  15:29:00  gdc
 * unmasked DirectLines so it can be exported from pvrd
 *
 * Revision 1.9  1997/05/12  15:26:52  gdc
 * depth still wierd but I'm at a loss to fix it - otherwise shadows, LVs now
 * tested
 *
 * Revision 1.8  1997/05/12  11:16:12  gdc
 * added width knowledge to region calculation
 *
 * Revision 1.7  1997/05/12  09:42:01  gdc
 * checkpoint - better math, but depth still wrong
 *
 * Revision 1.6  1997/05/10  15:20:48  gdc
 * added horz and vert short cuts, but depth of lines not correct
 *
 * Revision 1.5  1997/05/09  18:58:56  gdc
 * used a faster sqrt - but the math needs more improvement
 *
 * Revision 1.4  1997/05/09  18:55:46  gdc
 * first working one - flat shaded only tested
 *
 * Revision 1.3  1997/05/09  18:05:36  gdc
 * checkpoint
 *
 * Revision 1.2  1997/05/09  17:20:41  gdc
 * checkpoint
 *
 * Revision 1.1  1997/05/09  15:57:02  gdc
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_DLINES

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
#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

#if PCX1

#define UPPER_6_OF_TAG		0x3F000UL
#define SFLOAT_20BIT_ZERO	(0)
#define FLOAT_TO_FIXED 		((float) (((sgl_uint32)1) <<31))
#define MAGIC  				(1024.0f)
#define INV_MAGIC           (0.00048828125f)
/*
** Include the real C Packto20bit code
*/
#define INLINE_P20 static INLINE
#include "pto20b.h"
#define InPackTo20Bit PackTo20Bit				/* Try C version      */
#undef  INLINE_P20
#endif

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

#define IBUFFERSIZE	 48

static float gfDepthBias;

extern sgl_uint32 gu32UsedFlags;

extern sgl_uint32 TranslucentControlWord;
extern sgl_bool FogUsed;
extern float fMinInvZ;
extern DEVICE_REGION_INFO_STRUCT RegionInfo;
static TRANS_REGION_DEPTHS_STRUCT gDepthInfo[IBUFFERSIZE];

extern float gfBogusInvZ;

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

typedef struct tagSHADINGDATA
{
	union
	{
		sgl_uint32	Highlight;	/* 0xFFFF0000->555 colour, 0x000000FF->alpha */

		/* this part is re-read by Texture code to achive in-order TSP writes */ 
		/* chnaging this may break texture assembler if it is in build */
		struct
		{
			sgl_uint32	Highlight;
			sgl_uint32	ModControl;
			sgl_uint32	ModBaseCol;
			sgl_uint32	Smooth1;
			sgl_uint32	Smooth2;
		} WriteBack;

		struct
		{
			sgl_uint32	Col1;
			sgl_uint32	Col2;
			sgl_uint32	Col3;
			sgl_int32	nX;
			sgl_int32	nY;
		} Smooth;

		struct
		{
			float fIntensity[3];
			sgl_int32 nX;
			sgl_int32 nY;
		} NativeSmooth;
	} u;

} SHADINGDATA, *PSHADINGDATA;

typedef struct tagTEXDATA
{
	sgl_uint32	c;
	sgl_uint32	f;
	sgl_uint32	r;
	sgl_uint32  exp;
	
} TEXDATA, *PTEXDATA;

typedef struct tagIMATERIAL
{
	TEXDATA		Tex;	
	SHADINGDATA Shading;

	union
	{
		sgl_uint32	LightVolCol;
		float		ShadowBrightness;
	} v;

	sgl_uint32  NumExtraVertices;  /* 0 for triangle, 1 for quad, etc. */
			
} IMATERIAL, *PIMATERIAL;

typedef struct tagILINE
{
	float	fEdge[4][3];
	float   f1OverDet;
	float   fA;
	float   fB;
	float   fC;
	float   fZ1;
	float   fZ2;

	union	
	{
		sgl_int8	b[4];
		sgl_uint32	u;
	}		reg;
	
	sgl_uint32	TSPControlWord;
	sgl_uint32	BaseColour;

} ILINE, *PILINE;


typedef struct tagPROCESSDATACONTEXTLINES
{
	int				nInputLines;
	sgl_uint16      *pLines;
	PSGLVERTEX		pVertices; 
	PSGLVERTEX		pCurVer1; 
	PSGLVERTEX		pCurVer2; 
	sgl_uint32		TSPControlWord;

	int				ShiftRegX;
	
	sgl_uint32		TexAddress;
	int				n32MipmapOffset;
	float 			invTexSize;
	float           TexSize; /* used in d3d texture func */
	sgl_uint32		u32GlobalTransBy85div128;
	SGLCONTEXT		Context;
	
} PROCESSDATACONTEXTLINES;

/* per polygon intermediate routines */
typedef void (* PPIR)(PILINE pLine);

/* per buffer pack routines */
typedef void (* PBPR)(PILINE pLine, PIMATERIAL pMat, sgl_uint32 nLines,sgl_uint32 *pTSP);


static ILINE		gpLines[IBUFFERSIZE];
static IMATERIAL 	gpMat[IBUFFERSIZE];
static PIMATERIAL	gpMatCurrent = gpMat;
const PROCESSDATACONTEXTLINES gLineDC;
PROCESSDATACONTEXTLINES * gpLineDC = (PROCESSDATACONTEXTLINES *) &gLineDC;

/**********************************************************************/

static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16);

static void ProcessSmooth(PILINE pLine);
static void ProcessSmoothTrans(PILINE pLine);
static void ProcessSmoothGTrans(PILINE pLine);
static void ProcessHigh(PILINE pLine);
static void ProcessHighTrans(PILINE pLine);
static void ProcessFlatTrans(PILINE pLine);
static void ProcessHighGTrans(PILINE pLine);
static void ProcessFlatGTrans(PILINE pLine);
static void ProcessHighTexShadOrLiVol(PILINE pLine);
static void ProcessSmoothShadOrLiVol(PILINE pLine);
static void ProcessFlatShadOrLiVol(PILINE pLine);
static void ProcessFlatTransShadOrLiVol(PILINE pLine);
static void ProcessFlatGTransShadOrLiVol(PILINE pLine);
static void ProcessHighShadOrLiVol(PILINE pLine);
static void ProcessHighTransShadOrLiVol(PILINE pLine);
static void ProcessHighGTransShadOrLiVol(PILINE pLine);

#if 0
/* not in use at the moment */
static void ProcessHighTex(PILINE pLine);
static void ProcessSmoothTransShadOrLiVol(PILINE pLine);
static void ProcessSmoothGTransShadOrLiVol(PILINE pLine);
static void ProcessFlatTexShadOrLiVol(PILINE pLine);
#endif

static void PackFlat(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTrans(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHigh(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTrans(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackSmooth(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackSmoothShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackSmoothTrans(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTransShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTransShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTransLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTransLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackSmoothLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);

#if 0
/* not in use at the moment */
static void PackFlatTex(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTex(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTexShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTexShad(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackFlatTexLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
static void PackHighTexLiVol(PILINE , PIMATERIAL , sgl_uint32 , sgl_uint32 *);
#endif

/**********************************************************************/

static void (*ProcessFlatTexFn)( PILINE pLine );

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
#define FLAGS_1			MASK_SMOOTH_SHADE
#define FLAGS_2			MASK_FLAT_HIGHLIGHT
#define FLAGS_3			MASK_FLAT_HIGHLIGHT | MASK_TEXTURE

#define FLAGS_4			0
#define FLAGS_5			MASK_TEXTURE
#define FLAGS_6			MASK_FLAT_HIGHLIGHT
#define FLAGS_7			MASK_FLAT_HIGHLIGHT | MASK_TEXTURE


/*
// Traditional/D3D shading
*/

static IFUNCBLOCK NoVolFuncs[16] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{NULL, 					PackFlat,	    	2,	FLAGS(0, 0) }, 
	{ProcessSmooth,         PackSmooth, 		4,	FLAGS(1, 0) }, 
	{ProcessHigh, 			PackHigh, 			4,	FLAGS(2, 0) }, 
	{ProcessHigh, 			PackHigh,			4,	FLAGS(2, 0) }, 
	#undef BASE_FLAGS

	/*
	// Global Translucency
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS  MASK_TRANS | MASK_TEXTURE
	{NULL, 					PackFlatTrans,    	8,	FLAGS(0, 0) }, 
	{ProcessSmooth,     	PackSmoothTrans,	10,	FLAGS(1, 0) }, 
	{ProcessHigh, 			PackHighTrans,		10,	FLAGS(2, 0) }, 
	{ProcessHigh, 			PackHighTrans,		10,	FLAGS(2, 0) }, 
	 
	/*
	// Vertex Translucency
	*/
	{ProcessFlatTrans,		PackFlatTrans,    	8,	FLAGS(0, 0) }, 
	{ProcessSmoothTrans,    PackSmoothTrans,	10,	FLAGS(1, 0) }, 
	{ProcessHighTrans, 		PackHighTrans,		10,	FLAGS(2, 0) }, 
	{ProcessHighTrans, 		PackHighTrans,		10,	FLAGS(2, 0) }, 

	/*
	// Vertex & Global Translucency
	*/
	{ProcessFlatGTrans,		PackFlatTrans,    	8,	FLAGS(0, 0) }, 
	{ProcessSmoothGTrans,   PackSmoothTrans,	10,	FLAGS(1, 0) }, 
	{ProcessHighGTrans, 	PackHighTrans,		10,	FLAGS(2, 0) }, 
	{ProcessHighGTrans, 	PackHighTrans,		10,	FLAGS(2, 0) }, 
	#undef BASE_FLAGS
};

/*
// SHADOWS
*/

static IFUNCBLOCK ShadowFuncs[16] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* No translucency  */

	{ProcessFlatShadOrLiVol,			PackFlatShad,			2,	FLAGS(0, 0) }, 
	{ProcessSmoothShadOrLiVol, 			PackSmoothShad,			6,	FLAGS(1, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighShad, 			4,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighShad,	 		4,	FLAGS(2, 0) }, 

	/*
	// Global Translucency
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS  MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE
	{ProcessFlatShadOrLiVol,			PackFlatTransShad,		8,	FLAGS(0, 0) }, 
	{ProcessFlatShadOrLiVol, 			PackFlatTransShad,		8,	FLAGS(0, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighTransShad, 		10,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighTransShad,	 	10,	FLAGS(2, 0) }, 

	/*
	// Vertex Translucency
	*/
	{ProcessFlatTransShadOrLiVol,		PackFlatTransShad,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTransShadOrLiVol,	 	PackFlatTransShad,		8,	FLAGS(0, 0) }, 
	{ProcessHighTransShadOrLiVol, 		PackHighTransShad, 		10,	FLAGS(2, 0) }, 
	{ProcessHighTransShadOrLiVol,		PackHighTransShad, 		10,	FLAGS(2, 0) }, 

	/*
	// Vertex & Global Translucency
	*/
	{ProcessFlatGTransShadOrLiVol,		PackFlatTransShad,		8,	FLAGS(0, 0) }, 
	{ProcessFlatGTransShadOrLiVol,		PackFlatTransShad,		8,	FLAGS(0, 0) }, 
	{ProcessHighGTransShadOrLiVol, 		PackHighTransShad, 		10,	FLAGS(2, 0) }, 
	{ProcessHighGTransShadOrLiVol, 		PackHighTransShad, 		10,	FLAGS(2, 0) }, 
	#undef BASE_FLAGS
};


/*
// LIGHT VOLUMES
*/

static IFUNCBLOCK LightVolFuncs[16] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* No translucency  */
	{ProcessFlatShadOrLiVol,			PackFlatLiVol,			2,	FLAGS(0, 0) }, 
	{ProcessSmoothShadOrLiVol, 			PackSmoothLiVol,		6,	FLAGS(1, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighLiVol, 			4,	FLAGS(2, 0) }, 
	{ProcessHighTexShadOrLiVol, 		PackHighLiVol,	 		4,	FLAGS(2, 0) }, 

	/*
	// Global Translucency
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS  MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE
	{ProcessFlatShadOrLiVol,			PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessFlatShadOrLiVol, 			PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighTransLiVol, 	10,	FLAGS(2, 0) }, 
	{ProcessHighShadOrLiVol, 			PackHighTransLiVol,	 	10,	FLAGS(2, 0) }, 

	/*
	// Vertex Translucency
	*/
	{ProcessFlatTransShadOrLiVol,		PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessFlatTransShadOrLiVol,	 	PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessHighTransShadOrLiVol, 		PackHighTransLiVol, 	10,	FLAGS(2, 0) }, 
	{ProcessHighTransShadOrLiVol,		PackHighTransLiVol, 	10,	FLAGS(2, 0) }, 

	/*
	// Vertex & Global Translucency
	*/
	{ProcessFlatGTransShadOrLiVol,		PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessFlatGTransShadOrLiVol,		PackFlatTransLiVol,		8,	FLAGS(0, 0) }, 
	{ProcessHighGTransShadOrLiVol, 		PackHighTransLiVol, 	10,	FLAGS(2, 0) }, 
	{ProcessHighGTransShadOrLiVol, 		PackHighTransLiVol, 	10,	FLAGS(2, 0) }, 
	#undef BASE_FLAGS
};

/**********************************************************************/

static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16)
{

	float Red =   (float)((Colour & 0x00FF0000) >> 16);
	float Green = (float)((Colour & 0x0000FF00) >> 8);
	float Blue =  (float)(Colour & 0x000000FF);
	
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

void PackISPLinesCore(sgl_uint32 *pPlaneMem, sgl_uint32 nLinesInChunk, 
					  sgl_uint32 *rTSPAddr, PILINE *rpLines,
					  int nIncrement, sgl_uint32 TSPIncrement)
{
	PILINE	pLine = *rpLines;
	sgl_uint32	TSPAddr = *rTSPAddr;

	while(nLinesInChunk--)
	{
#if PCX2 || PCX2_003
		/* front face here */
		FW(pPlaneMem, 0+0, pLine->fA);
		FW(pPlaneMem, 1+0, pLine->fB);
		FW(pPlaneMem, 2+0, (pLine->fC + gfDepthBias));
		/* Pack instruction and TSP address.
		 */
		IW(pPlaneMem, 3+0, ( forw_visib_fp | (TSPAddr << 4)));

		/* do the edges of the line */
		/* line edge */
		FW( pPlaneMem, 0+4, pLine->fEdge[0][0]);
		FW( pPlaneMem, 1+4, pLine->fEdge[0][1]);
		FW( pPlaneMem, 2+4, pLine->fEdge[0][2]);
		IW( pPlaneMem, 3+4, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* edge here */
		FW( pPlaneMem, 0+8, pLine->fEdge[1][0]);
		FW( pPlaneMem, 1+8, pLine->fEdge[1][1]);
		FW( pPlaneMem, 2+8, pLine->fEdge[1][2]);
		IW( pPlaneMem, 3+8, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* line edge */
		FW( pPlaneMem, 0+12, pLine->fEdge[2][0]);
		FW( pPlaneMem, 1+12, pLine->fEdge[2][1]);
		FW( pPlaneMem, 2+12, pLine->fEdge[2][2]);
		IW( pPlaneMem, 3+12, (forw_perp | (DEBUG_PERP_TAG << 4)));

		/* edge here */
		FW( pPlaneMem, 0+16, pLine->fEdge[3][0]);
		FW( pPlaneMem, 1+16, pLine->fEdge[3][1]);
		FW( pPlaneMem, 2+16, pLine->fEdge[3][2]);
		IW( pPlaneMem, 3+16, (forw_perp | (DEBUG_PERP_TAG << 4)));

#elif PCX1
		{
			float fInvMaxVal, fA, fB, fC;
		
			pPlaneMem[0+0] = forw_visib_fp | 
				((UPPER_6_OF_TAG & TSPAddr) << (20 - 12)) |	InPackTo20Bit(pLine->fA);
			pPlaneMem[1+0] = (TSPAddr << 20) | InPackTo20Bit(pLine->fB);
			pPlaneMem[2+0] = (sgl_int32) (pLine->fC * FLOAT_TO_FIXED);
			
			/* do the edges of the line */
			fA = pLine->fEdge[0][0];
			fB = pLine->fEdge[0][1];
			fC = pLine->fEdge[0][2];
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+3] = forw_perp | 
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | 
				InPackTo20Bit(fA * fInvMaxVal) ;
			pPlaneMem[1+3] = (DEBUG_PERP_TAG << 20) | InPackTo20Bit(fB * fInvMaxVal);
			pPlaneMem[2+3] = (sgl_int32) ( FLOAT_TO_FIXED * fC * fInvMaxVal );

			fA = pLine->fEdge[1][0];
			fB = pLine->fEdge[1][1];
			fC = pLine->fEdge[1][2];
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+6] = forw_perp | 
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | 
				InPackTo20Bit(fA * fInvMaxVal) ;
			pPlaneMem[1+6] = (DEBUG_PERP_TAG << 20) | InPackTo20Bit(fB * fInvMaxVal);
			pPlaneMem[2+6] = (sgl_int32) ( FLOAT_TO_FIXED * fC * fInvMaxVal );

			fA = pLine->fEdge[2][0];
			fB = pLine->fEdge[2][1];
			fC = pLine->fEdge[2][2];
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+9] = forw_perp | 
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | 
				InPackTo20Bit(fA * fInvMaxVal) ;
			pPlaneMem[1+9] = (DEBUG_PERP_TAG << 20) | InPackTo20Bit(fB * fInvMaxVal);
			pPlaneMem[2+9] = (sgl_int32) ( FLOAT_TO_FIXED * fC * fInvMaxVal );

			fA = pLine->fEdge[3][0];
			fB = pLine->fEdge[3][1];
			fC = pLine->fEdge[3][2];
			fInvMaxVal = ApproxRecip ((2 * MAGIC * (sfabs (fA) + sfabs (fB))) + sfabs (fC));

			pPlaneMem[0+12] = forw_perp | 
				((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) | 
				InPackTo20Bit(fA * fInvMaxVal) ;
			pPlaneMem[1+12] = (DEBUG_PERP_TAG << 20) | InPackTo20Bit(fB * fInvMaxVal);
			pPlaneMem[2+12] = (sgl_int32) ( FLOAT_TO_FIXED * fC * fInvMaxVal );
		
		}
#else
#pragma message ("Current Hardware selection doesn't support lines")
#endif
		pPlaneMem += WORDS_PER_PLANE * 5;

		*((sgl_uint32 *) &pLine) += nIncrement;
		TSPAddr += TSPIncrement;

	}
}

/**********************************************************************/

static int PackISPLines(PILINE pLine, PIMATERIAL pMat, 
						 int numLines, sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement)
{
	sgl_uint32	ChunkLimit, CurrentPos, DataSize, ChunkSize, nLinesInChunk;
	int 	nLines;
	
	nLines = numLines;
				
	while (nLines)
	{
		CurrentPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, 
									   WORDS_PER_PLANE * 5);

		if (CurrentPos == PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
		{
			break;
		}
		else
		{
			DataSize = WORDS_PER_PLANE * 5 * nLines;

			ChunkLimit = GetSabreLimit (CurrentPos);
			ChunkSize = ChunkLimit - CurrentPos;

			if (DataSize < ChunkSize)
			{
				nLinesInChunk = nLines;
			}
			else
			{
				nLinesInChunk = ChunkSize / (WORDS_PER_PLANE * 5);
				
				if (!nLinesInChunk)
				{
					break;
				}

				/* We need to re-calculate the datasize based on the number of lines
				 * from the bunch we are processing.
				 */
				DataSize = WORDS_PER_PLANE * 5 * nLinesInChunk;

			}
			
			AddRegionObjects((sgl_uint32 *) &(pLine->reg), sizeof (ILINE),
							 5, nLinesInChunk, CurrentPos, 
							 gDepthInfo, sizeof(TRANS_REGION_DEPTHS_STRUCT));

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = CurrentPos + DataSize;

			nLines -= nLinesInChunk;
			
			PackISPLinesCore( &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[CurrentPos], 
							  nLinesInChunk, 
							  &TSPAddr, &pLine,
							  sizeof (ILINE), TSPIncrement );
		}
	}
	
	if (nLines)
	{
		DPFDEV ((DBG_WARNING, "PackISPLine: Out of ISP buffer space"));

	}

	return (numLines - nLines);
}

/**********************************************************************/

#define PACKFLAT(a,b,c) \
a[0] = b->TSPControlWord | ((c >> 16) & 0x000000FF); \
a[1] = c << 16; 

#define PACKSMOOTH(a,b,c) \
a[0] = b->TSPControlWord ; \
a[1] = c->Shading.u.Smooth.nX << 16 | (c->Shading.u.Smooth.nY & 0x0000ffff); \
a[2] = c->Shading.u.Smooth.Col3 << 16 | (c->Shading.u.Smooth.Col1 & 0x0000ffff); \
a[3] = c->Shading.u.Smooth.Col2;

#define PACKFLATTEX(a,b,c,d) \
a[0] = b->TSPControlWord |((c >> 16) & 0x000000FF)|(d->Tex.exp << SHIFT_EXPONENT); \
a[1] = c << 16; 

#define PACKHIGH(a,b) \
a[2] = b->Shading.u.Highlight; \
a[3] = 0x0L;

#define PACKHIGHTEX(a,b) \
a[8] = b->Shading.u.Highlight; \
a[9] = 0x0L;

#define PACKTEX(w,x,y,z) \
w[2] = (x->Tex.r) & 0xffffUL; \
w[3] = 0x0L; \
w[4] = (z) | (( x->Tex.c) & 0xffffUL); \
w[5] = 0x0L; \
w[6] = (y) | (( x->Tex.f) & 0xffffUL); \
w[7] = 0x0L;

#define PACKFLATTRANS(w, x, y) \
w[0] = x->TSPControlWord | ((y >> 16) & 0x000000FF); \
w[1] = y << 16; \
w[2] = 0x1L; \
w[3] = 0x0L; \
w[4] = TranslucentControlWord << 16; \
w[5] = 0x0L; \
w[6] = TranslucentControlWord & 0xFFFF0000; \
w[7] = 0x0L;

#define PACKHIGHTRANS(w, x, y, z) \
w[0] = x->TSPControlWord | ((y >> 16) & 0x000000FF); \
w[1] = y << 16; \
w[2] = 0x1L; \
w[3] = 0x0L; \
w[4] = TranslucentControlWord << 16; \
w[5] = 0x0L; \
w[6] = TranslucentControlWord & 0xFFFF0000; \
w[7] = 0x0L; \
w[8] = z->Shading.u.Highlight; \
w[9] = 0x0L;

#define PACKSMOOTHTRANS(w,x,z) \
w[0] = x->TSPControlWord; \
w[1] = z->Shading.u.Smooth.nX << 16 | (z->Shading.u.Smooth.nY & 0x0000ffff); \
w[2] = 0x1L; \
w[3] = 0x0L; \
w[4] = TranslucentControlWord << 16; \
w[5] = 0x0L; \
w[6] = TranslucentControlWord & 0xFFFF0000; \
w[7] = 0x0L; \
w[8] = z->Shading.u.Smooth.Col3 << 16 | (z->Shading.u.Smooth.Col1 & 0x0000ffff); \
w[9] = z->Shading.u.Smooth.Col2; 

#define PACKFLATSHADLV(a, b, c, d) \
a[0] = (b->TSPControlWord | (c >> 16) & 0x000000FF); \
a[1] = (c << 16) | d;

#define PACKSMOOTHSHADLV(a, b, c, d, e) \
a[0] = b->TSPControlWord; \
a[1] = e->Shading.u.Smooth.nX << 16 | (e->Shading.u.Smooth.nY & 0x0000ffff); \
a[2] = e->Shading.u.Smooth.Col3 << 16 | (e->Shading.u.Smooth.Col1 & 0x0000ffff); \
a[3] = e->Shading.u.Smooth.Col2; \
a[4] = (d << 16) | (e->Shading.u.Smooth.Col1 & 0x0000ffff); \
a[5] = e->Shading.u.Smooth.Col2;

#define PACKFLATTRANSSHADLV(a, b, c, d) \
a[0] = (b->TSPControlWord | (c >> 16) & 0x000000FF); \
a[1] = (c << 16) | d; \
a[2] = 0x1L; \
a[3] = 0x0L; \
a[4] = TranslucentControlWord << 16; \
a[5] = 0x0L; \
a[6] = TranslucentControlWord & 0xFFFF0000; \
a[7] = 0x0L; 

#define PACKFLATSHADLVTEX(a, b, c, d, e) \
a[0] = (b->TSPControlWord | (c >> 16) & 0x000000FF)|(e->Tex.exp << SHIFT_EXPONENT); \
a[1] = (c << 16) | d;

/**********************************************************************/

static void PackFlat( PILINE pLine, PIMATERIAL pMat, 
					 sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		
		PACKFLAT(pTSP , pLine, uBaseColour);
		
		pLine++;
		pTSP += 2;
		
	}/*end while*/
}


/**********************************************************************/

static void PackSmooth( PILINE pLine, PIMATERIAL pMat, 
						sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		/* sgl_uint32 uBaseColour;*/
		
		/*
		** Put the index in the projected plane structure.
		*/
		/* uBaseColour = pLine->BaseColour; */
		
		PACKSMOOTH(pTSP , pLine, pMat);

		pLine++;
		pMat++;
		pTSP += 4;
		
	}/*end while*/
}


static void PackSmoothTrans( PILINE pLine, PIMATERIAL pMat, 
						sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		/* sgl_uint32 uBaseColour;*/
		
		/*
		** Put the index in the projected plane structure.
		*/

		PACKSMOOTHTRANS(pTSP , pLine, pMat);

		pLine++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}


/**********************************************************************/

static void PackFlatTrans( PILINE pLine, PIMATERIAL pMat, 
					 sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		
		PACKFLATTRANS(pTSP , pLine, uBaseColour);
		
		pLine++;
		pTSP += 8;
		
	}/*end while*/
}


/**********************************************************************/

static void PackHigh(PILINE pLine, PIMATERIAL pMat, 
					 sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		
		PACKFLAT(pTSP, pLine, uBaseColour);
		PACKHIGH(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 4;
		
	}/*end while*/
}

/**********************************************************************/

static void PackHighTrans(PILINE pLine, PIMATERIAL pMat, 
					 sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		
		PACKHIGHTRANS(pTSP,  pLine, uBaseColour, pMat);
		
		pLine++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatShad(PILINE pLine, PIMATERIAL pMat, 
						 sgl_uint32 numLines, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pLine, uFlat0Col24, uFlat1Col16);
		
		pLine++;
		pTSP += 2;
		
	}/*end for*/
}

/**********************************************************************/

static void PackFlatTransShad(PILINE pLine, PIMATERIAL pMat, 
						 sgl_uint32 numLines, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATTRANSSHADLV(pTSP, pLine, uFlat0Col24, uFlat1Col16);
		
		pLine++;
		pTSP += 8;
		
	}/*end for*/
}

/**********************************************************************/

static void PackHighShad(PILINE pLine, PIMATERIAL pMat, 
						 sgl_uint32 numLines, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLV(pTSP, pLine, uFlat0Col24, uFlat1Col16);
		PACKHIGH(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 4;
		
	}/*end for*/
}

/**********************************************************************/

static void PackHighTransShad(PILINE pLine, PIMATERIAL pMat, 
						 sgl_uint32 numLines, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATTRANSSHADLV(pTSP, pLine, uFlat0Col24, uFlat1Col16);
		pTSP += 8;
		PACKHIGH(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 2;
		
	}/*end for*/
}

/**********************************************************************/

static void PackSmoothShad(PILINE pLine, PIMATERIAL pMat, 
						 sgl_uint32 numLines, sgl_uint32 *pTSP)
{

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKSMOOTHSHADLV(pTSP, pLine, uFlat0Col24, uFlat1Col16, pMat);
		
		pLine++;
		pTSP += 6;
		pMat ++;	
	}/*end for*/
}

/**********************************************************************/

static void PackFlatLiVol(PILINE pLine, PIMATERIAL pMat, 
						  sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		PACKFLATSHADLV(pTSP, pLine, uBaseColour, uFlat1Col16);
		
		pLine++;
		pTSP += 2;
		
	}/*end for*/

}

/**********************************************************************/

static void PackFlatTransLiVol(PILINE pLine, PIMATERIAL pMat, 
						  sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		PACKFLATTRANSSHADLV(pTSP, pLine, uBaseColour, uFlat1Col16);
		
		pLine++;
		pTSP += 8;
		
	}/*end for*/

}

/**********************************************************************/

static void PackHighLiVol(PILINE pLine, PIMATERIAL pMat, 
						  sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		PACKFLATSHADLV(pTSP, pLine, uBaseColour, uFlat1Col16);
		PACKHIGH(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 4;
		
	}/*end for*/

}

/**********************************************************************/

static void PackHighTransLiVol(PILINE pLine, PIMATERIAL pMat, 
						  sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		PACKFLATTRANSSHADLV(pTSP, pLine, uBaseColour, uFlat1Col16);
		pTSP += 8;
		PACKHIGH(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 2;
		
	}/*end for*/

}

/**********************************************************************/

static void PackSmoothLiVol(PILINE pLine, PIMATERIAL pMat, 
						  sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	sgl_uint32	uFlat1Col16;
   	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{	
		/*
		** Put the index in the projected plane structure.
		*/
		PACKSMOOTHSHADLV(pTSP, pLine, pLine->BaseColour, uFlat1Col16, pMat);
		
		pLine++;
		pMat++;
		pTSP += 6;
		
	}/*end for*/

}

#if 0
/* not in use at the moment */
/**********************************************************************/

static void PackFlatTex(PILINE pLine, PIMATERIAL pMat, 
						sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/

	sgl_uint32 AddrHi, AddrLo;

	AddrHi = gLineDC.TexAddress & 0xffff0000;
	AddrLo = gLineDC.TexAddress << 16;

	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		
		PACKFLATTEX(pTSP, pLine, uBaseColour, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pLine++;
		pMat++;
		pTSP += 8;
		
	}/*end while*/
	
}

/**********************************************************************/

static void PackHighTex(PILINE pLine, PIMATERIAL pMat, 
						sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	/*
	// Go through the lines. Move on to the next shading results and
	// and next line pointer (NOTE Planes is array of pointers)
	*/

	sgl_uint32 AddrHi, AddrLo;

	AddrHi = gLineDC.TexAddress & 0xffff0000;
	AddrLo = gLineDC.TexAddress << 16;

	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		
		PACKFLATTEX(pTSP, pLine, uBaseColour, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 10;
		
	}/*end while*/
}

/**********************************************************************/

static void PackFlatTexShad(PILINE pLine, PIMATERIAL pMat, 
							sgl_uint32 numLines, sgl_uint32 *pTSP)
{


	sgl_uint32 AddrHi, AddrLo;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

	AddrHi = gLineDC.TexAddress & 0xffff0000;
	AddrLo = gLineDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane lineer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLVTEX(pTSP, pLine, uFlat0Col24, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pLine++;
		pMat++;
		pTSP += 8;
		
	}/*end for*/
}

/**********************************************************************/

static void PackHighTexShad(PILINE pLine, PIMATERIAL pMat, 
							sgl_uint32 numLines, sgl_uint32 *pTSP)
{


	sgl_uint32 AddrHi, AddrLo;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

	AddrHi = gLineDC.TexAddress & 0xffff0000;
	AddrLo = gLineDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		PACKFLATSHADLVTEX(pTSP, pLine, uFlat0Col24, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 10;
		
	}/*end for*/
}

/**********************************************************************/

static void PackFlatTexLiVol(PILINE pLine, PIMATERIAL pMat, 
							 sgl_uint32 numLines, sgl_uint32 *pTSP)
{
	sgl_uint32 AddrHi, AddrLo;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	
	AddrHi = gLineDC.TexAddress & 0xffff0000;
	AddrLo = gLineDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pLine->BaseColour;
		PACKFLATSHADLVTEX(pTSP, pLine, uBaseColour, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		
		pLine++;
		pMat++;
		pTSP += 8;
		
	}/*end for*/

}

/**********************************************************************/

static void PackHighTexLiVol(PILINE pLine, PIMATERIAL pMat, 
							 sgl_uint32 numLines, sgl_uint32 *pTSP)
{

	sgl_uint32 AddrHi, AddrLo;
	sgl_uint32	uFlat1Col16;
	
	uFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	AddrHi = gLineDC.TexAddress & 0xffff0000;
	AddrLo = gLineDC.TexAddress << 16;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numLines--)
	{
		sgl_uint32 uBaseColour;
		
		uBaseColour = pLine->BaseColour;
		
		PACKFLATSHADLVTEX(pTSP, pLine, uBaseColour, uFlat1Col16, pMat);
		PACKTEX(pTSP, pMat, AddrHi, AddrLo)
		PACKHIGHTEX(pTSP, pMat);
		
		pLine++;
		pMat++;
		pTSP += 10;
		
	}/*end for*/
}

#endif

/**********************************************************************/
#define IMPLEMENT_VERTEX_TRANS							\
{												\
	sgl_uint32 Alpha;							\
	Alpha = (gLineDC.pCurVer1->u32Colour >> 24) +	\
			(gLineDC.pCurVer1->u32Colour >> 24);	\
	Alpha *= (2*85);							\
	Alpha &= (0xF << 13);						\
	Alpha  = (0xF << 13) - Alpha;				\
	pLine->TSPControlWord |= Alpha;				\
}

#define IMPLEMENT_VERTEX_GLOBAL_TRANS						\
{																\
	sgl_uint32 Alpha;											\
	Alpha = (gLineDC.pCurVer1->u32Colour >> 24) +						\
			(gLineDC.pCurVer1->u32Colour >> 24) +						\
			(gLineDC.pCurVer1->u32Colour >> 24);						\
	Alpha *= gLineDC.u32GlobalTransBy85div128;						\
	Alpha &= (0xF << 13);										\
	Alpha  = (0xF << 13) - Alpha;								\
	pLine->TSPControlWord |= Alpha;								\
}

#define IMPLEMENT_HIGHLIGHT						 \
{												 \
	sgl_uint32 H = gLineDC.pCurVer1->u32Specular;	 \
	pMat->Shading.u.Highlight = ((H << 07) & 0x7C000000) | \
					  ((H << 10) & 0x03E00000) | \
					  ((H << 13) & 0x001F0000);  \
}

#define IMPLEMENT_SMOOTH						 \
{												 \
	pMat->Shading.u.Smooth.Col1 = gLineDC.pCurVer1->u32Colour;		\
	pMat->Shading.u.Smooth.Col2 = gLineDC.pCurVer2->u32Colour;		\
	pMat->Shading.u.Smooth.nX = (int) gLineDC.pCurVer1->fX;			\
	pMat->Shading.u.Smooth.nY = (int) gLineDC.pCurVer1->fY;			\
}

#define IMPLEMENT_SHADOW_LIGHTVOL \
	pMat->v.LightVolCol = gLineDC.Context.u.u32LightVolColour;

/**********************************************************************/
/**********************************************************************/

#define RESCALE 1
static void ProcessD3DLineTexMipMap(PILINE pLine)
{
}

/**********************************************************************/

static void ProcessLiteLineTexMipMap(PILINE pLine)
{
}

/**********************************************************************/

static void ProcessD3DLineTex(PILINE pLine)
{
}

/**********************************************************************/

static void ProcessLiteLineTex(PILINE pLine)
{
}

/**********************************************************************/

#define GET_R(x)				(((x) >> 16) & 0x000000FF)
#define GET_G(x)				(((x) >> 8) & 0x000000FF)
#define GET_B(x)				((x) & 0x000000FF)

/**********************************************************************/

static sgl_uint32 /*INLINE*/ PackTSPSmoothGetI ( const IMATERIAL *pMat, 
											 const float	 InverseDet,
												   float	 fIntensity[3])
{
	int RTotal, GTotal, BTotal;
	int V0Int,	V1Int;
	int IntensityTotals;
	int  nTmp;
	float fLargest;
	float fIntensityScale, IScaleByIDet;
	float fColourScale;
	int	  nColourScale;

	int FinalColour;
    /*
	// Extract each of the red green and blue components from each of the
	// three vertex colours, and sum up the totals of the R,G,Bs PLUS the
	// total of each component for the three verts, which will be used to
	// compute an "intensity"
	*/

	int Colour;

	/* Vertex 0 */
	Colour = pMat->Shading.u.Smooth.Col1;
	RTotal = GET_R(Colour);
	GTotal = GET_G(Colour);
	BTotal = GET_B(Colour);
	V0Int  = RTotal + GTotal + BTotal;

	/* Vertex 1 */
	Colour = pMat->Shading.u.Smooth.Col2;
	V1Int  = GET_R(Colour);
	RTotal+= V1Int;

	nTmp   = GET_G(Colour);
	GTotal+= nTmp;
	V1Int += nTmp;

	nTmp   = GET_B(Colour);
	BTotal+= nTmp;
	V1Int += nTmp;


	/*
	// We now want to find the largest overal colour component, so that
	// we can scale up the colours. Our interpolation of intensity can't make
	// things brighter, only darker, so to allow us to interpolate outside
	// of the triangle we base the calc on, we need to make the colours as
	// bright as possible, and at the same time reduce the intensities by
	// the same factor.
	*/
	if(RTotal > GTotal)
	{
		if(RTotal > BTotal)
		{
			fLargest = (float) RTotal;
		}
		else
		{
			fLargest = (float) BTotal;
		}
	}
	else if(GTotal > BTotal)
	{
		fLargest = (float) GTotal;
	}
	else if(BTotal > 0)
	{
		fLargest = (float) BTotal;
	}
	/*
	// Else the whole thing is black. Exit out now
	*/
	else
	{
		fIntensity[0] = 0.0f;
		fIntensity[1] = 0.0f;
		fIntensity[2] = 0.0f;

		return 0;
	}

	/*
	// At this point we could check to see if the largest is so close to its
	// maximum value, that scaling it up futher would be a complete waste of
	// time. We need to do some investigation to see if this would be
	// worthwhile for various benchmarks ;)
	*/


	/*
	// Compute scaling factors: One to scale the colours up, the other to scale
	// the intensity values down (as soon as we compute them!).
	//
	// Note we do the colour scale as a "fixed point" value which is equiv to
	// (255/Largest)*256. It's difficult to explain. This, of course, contains a
	// disgusting division followed by a float to int conversion. URK!
	*/
	fColourScale = (256.0f * 255.0f)/ fLargest;

	/*
	// Add all the "intensity" values to get the "average" intensity.
	// Note that we can use the RTotal etc values just as well. These are
	// more likely to be sitting in registers than V0Int etc since they
	// were used more recently.
	//
	// Hopefully it should also give us some integer stuff to do while
	// the divide occurs?
	*/
	IntensityTotals	= RTotal + GTotal + BTotal;

	/*
	// Convert the colour scaling to our integer
	*/
	nColourScale = (int) (fColourScale);

	/*
	// Compute the scaling factor for the Intensities.  Now the overall colours
	// have been sort of scaled up by fColourScale, so the intensities should
	// come down by 1/fColour scale. With a bit of luck the floating point in
	// this should be overlapable with the previous code, since they are
	// independant.
	//
	// Being more precise...
	//
	// If we ignore the colour rescaling for the moment, then the scaling factor
	// for the intensities should be 3/(IntensityTotals). 
	//
	// Since each of the colours have been made brighter by (3*255)/fLargest
	// (note we have cunningly hidden the 3x in the integer maths), we should
	// therefore scale down our intensity by fLargest/(3*255). This then gives
	// a combined scaling of...
	//
	//  scaling = fLargest * (1.0f/255)) /(IntensityTotals).
	*/
	fIntensityScale = fLargest * (1.0f/255.0f) / ((float) IntensityTotals);

	/* 
	// Scale the RGB components so that they only occupy 5 bits. Int Muls are
	// slow on a Pentium, but should be 2x faster on MIPS 5000. However, with
	// a bit of luck, these will go on while the previous division is chugging
	// away.
	*/
	RTotal = (nColourScale * RTotal) >> (11 - 10);
	GTotal = (nColourScale * GTotal) >> (11 -  5);
	BTotal = (nColourScale * BTotal) >>  11;
	FinalColour	= (RTotal & (0x1f << 10)) | (GTotal & (0x1f << 5)) | BTotal;

	/*
	// lastly, calc the scaled intensity (delta) values...
	*/
	IScaleByIDet = fIntensityScale * InverseDet;

	fIntensity[0] = ((float) (V1Int - V0Int)) * IScaleByIDet;
	fIntensity[1] = 0.0f;
	fIntensity[2] = ((float) (V0Int)) * fIntensityScale;

	return (FinalColour);
}

/**********************************************************************/

static void INLINE ConvertIntensitiesToFixedPoint( const float	 fBase,
												   const float	 fT1,
												   const float	 fT2,
													sgl_uint32	 nIntensity[2])
{
#if 0 /* !(SLOW_FTOI) */

	ASSERT(fBase >= 0.0f);

	nIntensity[0] = (sgl_int32)(fBase * (float) 0x4000);
	nIntensity[1] = ( ((sgl_int32)(fT2  * (float) 0x10000)) & 0xFFFF) |
					( ((sgl_int32)(fT1  * (float) 0x10000)) << 16);
#else
/*
// To convert a float to an int, we need to add 1.5 * 2^23. To allow for the
// extra "fraction" bits in the fixed point number, reduce the number of
// shifts that occur in the FP add by the appropriate amount
*/
#define F_TO_INT_AND_SCALE_BY_0x4000  (1.5f * ((float) (1 << 23)) / ((float) 0x4000))
#define F_TO_INT_AND_SCALE_BY_0x10000 (1.5f * ((float) (1 << 23)) / ((float) 0x10000))
	union
	{
		float		f;
		sgl_uint32	i;
	} flongFBase, flongFT1, flongFT2;

	flongFBase.f = fBase + F_TO_INT_AND_SCALE_BY_0x4000;
	flongFT1.f	 = fT1	 + F_TO_INT_AND_SCALE_BY_0x10000;
	flongFT2.f	 = fT2	 + F_TO_INT_AND_SCALE_BY_0x10000;

	nIntensity[0] = flongFBase.i & 0xFFFF;
	nIntensity[1] = (flongFT2.i	 & 0xFFFF) | (flongFT1.i << 16);

#endif
}

/**********************************************************************/

static void ProcessSmooth(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;
	sgl_uint32 u32Intensity[2];
	sgl_uint32 Col16;
	float 		fIntensity[3],  fT1, fT2, fBase;

	IMPLEMENT_SMOOTH;
	Col16 = PackTSPSmoothGetI(pMat, pLine->f1OverDet, fIntensity);

	fBase = fIntensity[2];
	fT1 = fIntensity[0] * pLine->fEdge[1][1];
	fT2 = fIntensity[0] * pLine->fEdge[1][0];

	ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);
	pMat->Shading.u.Smooth.Col1 = u32Intensity[0];
	pMat->Shading.u.Smooth.Col2 = u32Intensity[1];
	pMat->Shading.u.Smooth.Col3 = Col16;

	gpMatCurrent = ++pMat;
}

/**********************************************************************/
static void ProcessSmoothTrans(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;
	sgl_uint32 u32Intensity[2];
	sgl_uint32 Col16;
	float 		fIntensity[3],  fT1, fT2, fBase;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_SMOOTH;
	Col16 = PackTSPSmoothGetI(pMat, pLine->f1OverDet, fIntensity);

	fBase = fIntensity[2];
	fT1 = fIntensity[0] * pLine->fEdge[1][1];
	fT2 = fIntensity[0] * pLine->fEdge[1][0];

	ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);
	pMat->Shading.u.Smooth.Col1 = u32Intensity[0];
	pMat->Shading.u.Smooth.Col2 = u32Intensity[1];
	pMat->Shading.u.Smooth.Col3 = Col16;

	gpMatCurrent = ++pMat;
}

/**********************************************************************/
static void ProcessSmoothGTrans(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;
	sgl_uint32 u32Intensity[2];
	sgl_uint32 Col16;
	float 		fIntensity[3],  fT1, fT2, fBase;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_SMOOTH;
	Col16 = PackTSPSmoothGetI(pMat, pLine->f1OverDet, fIntensity);

	fBase = fIntensity[2];
	fT1 = fIntensity[0] * pLine->fEdge[1][1];
	fT2 = fIntensity[0] * pLine->fEdge[1][0];

	ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);
	pMat->Shading.u.Smooth.Col1 = u32Intensity[0];
	pMat->Shading.u.Smooth.Col2 = u32Intensity[1];
	pMat->Shading.u.Smooth.Col3 = Col16;

	gpMatCurrent = ++pMat;
}

/**********************************************************************/

static void ProcessFlatTrans(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS;
}

/**********************************************************************/

static void ProcessFlatGTrans(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
}

/**********************************************************************/

static void ProcessHighTrans(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessHighGTrans(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessHigh(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
}

/**********************************************************************/

static void ProcessFlatShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;


	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/
static void ProcessFlatTransShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessFlatGTransShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighTransShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighGTransShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessHighTexShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/

static void ProcessSmoothShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;
	sgl_uint32 u32Intensity[2];
	sgl_uint32 Col16;
	float 		fIntensity[3],  fT1, fT2, fBase;

	IMPLEMENT_SMOOTH;
	IMPLEMENT_SHADOW_LIGHTVOL;
	Col16 = PackTSPSmoothGetI(pMat, pLine->f1OverDet, fIntensity);

	fBase = fIntensity[2];
	fT1 = fIntensity[0] * pLine->fEdge[1][1];
	fT2 = fIntensity[0] * pLine->fEdge[1][0];

	ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);
	pMat->Shading.u.Smooth.Col1 = u32Intensity[0];
	pMat->Shading.u.Smooth.Col2 = u32Intensity[1];
	pMat->Shading.u.Smooth.Col3 = Col16;

	gpMatCurrent = ++pMat;
}

#if 0
/* not in use at the moment */
/**********************************************************************/

static void ProcessSmoothTransShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;
	sgl_uint32 u32Intensity[2];
	sgl_uint32 Col16;
	float 		fIntensity[3],  fT1, fT2, fBase;

	IMPLEMENT_VERTEX_TRANS;
	IMPLEMENT_SMOOTH;
	IMPLEMENT_SHADOW_LIGHTVOL;
	Col16 = PackTSPSmoothGetI(pMat, pLine->f1OverDet, fIntensity);

	fBase = fIntensity[2];
	fT1 = fIntensity[0] * pLine->fEdge[1][1];
	fT2 = fIntensity[0] * pLine->fEdge[1][0];

	ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);
	pMat->Shading.u.Smooth.Col1 = u32Intensity[0];
	pMat->Shading.u.Smooth.Col2 = u32Intensity[1];
	pMat->Shading.u.Smooth.Col3 = Col16;

	gpMatCurrent = ++pMat;
}

/**********************************************************************/

static void ProcessSmoothGTransShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;
	sgl_uint32 u32Intensity[2];
	sgl_uint32 Col16;
	float 		fIntensity[3],  fT1, fT2, fBase;

	IMPLEMENT_VERTEX_GLOBAL_TRANS;
	IMPLEMENT_SMOOTH;
	IMPLEMENT_SHADOW_LIGHTVOL;
	Col16 = PackTSPSmoothGetI(pMat, pLine->f1OverDet, fIntensity);

	fBase = fIntensity[2];
	fT1 = fIntensity[0] * pLine->fEdge[1][1];
	fT2 = fIntensity[0] * pLine->fEdge[1][0];

	ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);
	pMat->Shading.u.Smooth.Col1 = u32Intensity[0];
	pMat->Shading.u.Smooth.Col2 = u32Intensity[1];
	pMat->Shading.u.Smooth.Col3 = Col16;

	gpMatCurrent = ++pMat;
}

/**********************************************************************/

static void ProcessFlatTexShadOrLiVol(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL;
}

/**********************************************************************/
static void ProcessHighTex(PILINE pLine)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT;
}

#endif

/**********************************************************************/

/* Used as an Index into the vertex list.
 */
static sgl_uint32	nLineVertexIndex = 0;

static int ProcessLinesCore( PPIR pPerLinefn, sgl_uint32 NewObject, sgl_uint32 nNextLineInc)
{
	sgl_int32 rX0, rY0, rY1, rX1, tmp;
	PSGLVERTEX pVert1,pVert2;
	sgl_uint32 uDefaultType, uCurrentType/*, uOrBackType*/;
	static sgl_uint32 uDoneFirst[2];
	PILINE	pLine = gpLines;
	int	ShiftRegX = gpLineDC->ShiftRegX;
	float length, invLength, shiftX, shiftY, delX, delY, delZ, fDet, fZInvDet;
	float ftmp, fwtmp, width;
	TRANS_REGION_DEPTHS_STRUCT* pGDepth;

	pGDepth = &gDepthInfo[0];	


	if(gpLineDC->Context.uLineWidth < 1)
	{
		width = 0.5f;
	}
	else if(gpLineDC->Context.uLineWidth > 64 )
	{
		width = 32.0f;
	}
	else
	{
		width = gpLineDC->Context.uLineWidth * 0.5f;
	}

	/* for the moment only flat shaded opaque */
	if(gpLineDC->TSPControlWord & MASK_TRANS) 
	{
		if (NewObject)
		{
			uDoneFirst[0] = FALSE;
			uDoneFirst[1] = FALSE;
		}
			
		uDefaultType = PACKED_TYPE_TRANSLUCENT;
		uCurrentType = PACKED_TYPE_TRANSLUCENT;
/*		uOrBackType = PACKED_TRANSTYPE_BACKBIT; */
	}
	else
	{
		uDefaultType = PACKED_TYPE_OPAQUE;
		uCurrentType = PACKED_TYPE_OPAQUE;
	}
	

	while ( gpLineDC->nInputLines )
	{
		gpLineDC->nInputLines--;

		if (gpLineDC->pLines == NULL)
		{
			/* DrawPrimitve case. ie no face list is defined.
			 */
			gpLineDC->pCurVer1 = &(gpLineDC->pVertices[nLineVertexIndex]);
			nLineVertexIndex++;
			gpLineDC->pCurVer2 = &(gpLineDC->pVertices[nLineVertexIndex]);

			/* Index differs between SGL, D3D and DrawPrimitive.
			 */
			nLineVertexIndex += nNextLineInc;
		}
		else
		{
			sgl_uint16 *pnLineVertices;
		
			pnLineVertices = (sgl_uint16 *) gpLineDC->pLines;

			/* This is the DrawIndexPrimitive and SGLDirect case
			 * since we have defined faces.
			 */
			gpLineDC->pCurVer1 = &(gpLineDC->pVertices[(sgl_uint16) *(pnLineVertices + nLineVertexIndex)]);
			nLineVertexIndex++;
			gpLineDC->pCurVer2 = &(gpLineDC->pVertices[(sgl_uint16) *(pnLineVertices + nLineVertexIndex)]);

			/* Index differs between SGL, D3D and DrawPrimitive.
			 */
			nLineVertexIndex += nNextLineInc;
		}

		pVert1 = gpLineDC->pCurVer1;
		pVert2 = gpLineDC->pCurVer2;

		pGDepth->BaseDepth = MIN (gpLineDC->pCurVer1->fZ , gpLineDC->pCurVer2->fZ);

		if(pVert1->fX < pVert2->fX)
		{
			rX0 = (int)(pVert1->fX - width);
			rX1 = (int)(pVert2->fX + width);

			rX0 >>= ShiftRegX;
			rX1 >>= ShiftRegX;

		} 
		else
		{
			rX0 = (int)(pVert2->fX - width);
			rX1 = (int)(pVert1->fX + width);

			rX0 >>= ShiftRegX;
			rX1 >>= ShiftRegX;

		}

		if(pVert1->fY < pVert2->fY)
		{
			rY0 = (int)(pVert1->fY - width);
			rY1 = (int)(pVert2->fY + width);
		}
		else
		{
			rY0 = (int)(pVert2->fY - width);
			rY1 = (int)(pVert1->fY + width);
		}

		if(rY0 > rY1)
		{
			tmp = rY1;
			rY1 = rY0;
			rY0 = tmp;
		}

		if(rX0 > rX1)
		{
			tmp = rX1;
			rX1 = rX0;
			rX0 = tmp;
		}

#if DEBUGDEV || DEBUG
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpLineDC->Context.bDoClipping))
		{
			if( (rX0 < gpLineDC->Context.FirstXRegion) ||
				(rY0 < gpLineDC->Context.FirstYRegion) ||
				(rX1 > gpLineDC->Context.LastXRegion)  ||
				(rY1 > gpLineDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessLinesCore: need clipping but bDoClipping is FALSE"));			
			}
		}
#endif
		
		/*
		// Check for Region clipping
		*/
		if (gpLineDC->Context.bDoClipping)
		{
			if (rX0 < gpLineDC->Context.FirstXRegion)
			{
				if ( rX1 < gpLineDC->Context.FirstXRegion )
				{
					/* Very Unlikely */
					continue;
				}
				
				rX0 = gpLineDC->Context.FirstXRegion;
			}
		
			if (rY0 < gpLineDC->Context.FirstYRegion)
			{
				if ( rY1 < gpLineDC->Context.FirstYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY0 = gpLineDC->Context.FirstYRegion;
			}
		
			if (rX1 > gpLineDC->Context.LastXRegion)
			{
				if ( rX0 > gpLineDC->Context.LastXRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rX1 = gpLineDC->Context.LastXRegion;
			}
		
			if (rY1 > gpLineDC->Context.LastYRegion)
			{
				if ( rY0 > gpLineDC->Context.LastYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY1 = gpLineDC->Context.LastYRegion;
			}
		}

		/* Shifts are either looked up or calculated depending on slope */ 
		delX = pVert2->fX - pVert1->fX; /* X length of line */
		delY = pVert2->fY - pVert1->fY; /* Y length of line */

		if (gpLineDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
			pLine->fZ1 = pLine->fZ2 = gfBogusInvZ;

			gfBogusInvZ += BOGUSINVZ_INCREMENT;
		}
		else
		{
			pLine->fZ1 = pVert1->fInvW * fMinInvZ;
			pLine->fZ2 = pVert2->fInvW * fMinInvZ;
		}

#if DEBUG || DEBUGDEV

		if (pVert1->fInvW > 10.0f || pVert2->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessLinesCore: fInvW out of range"));
		}
#endif
		
		delZ = pLine->fZ2 - pLine->fZ1;
		
		if(delX == 0.0f)
		{

			/* Vertical line short cut */
			if(delY < 0.5f)
			{
				delY = -delY;
				if(delY < 0.5f)
				{
					continue;
				}
				/* determinant */
				fDet = delY;
				pLine->f1OverDet = 1.0f/fDet;
				fZInvDet = delZ * pLine->f1OverDet;
				
				ftmp = delY * pVert2->fX;
				fwtmp = fDet * width;
				
				pLine->fEdge[0][0] = -delY; 
				pLine->fEdge[0][1] = 0.0f; 
				pLine->fEdge[0][2] = ftmp + fwtmp ;
				
				pLine->fEdge[1][0] = 0.0f;
				pLine->fEdge[1][1] = 1.0f;
				pLine->fEdge[1][2] = -pVert2->fY;
				
				pLine->fEdge[2][0] = delY;
				pLine->fEdge[2][1] = 0.0f;
				pLine->fEdge[2][2] = -ftmp + fwtmp;
				
				pLine->fEdge[3][0] = 0.0f;
				pLine->fEdge[3][1] = -1.0f;
				pLine->fEdge[3][2] = pVert1->fY;
			
			}
			else
			{
				/* determinant */
				fDet = delY;
				pLine->f1OverDet = 1.0f/fDet;
				fZInvDet = delZ * pLine->f1OverDet;
				
				ftmp = delY * pVert1->fX;
				fwtmp = fDet * width;
				
				pLine->fEdge[0][0] = -delY; 
				pLine->fEdge[0][1] = 0.0f; 
				pLine->fEdge[0][2] = ftmp + fwtmp ;
				
				pLine->fEdge[1][0] = 0.0f;
				pLine->fEdge[1][1] = 1.0f;
				pLine->fEdge[1][2] = -pVert1->fY;
				
				pLine->fEdge[2][0] = delY;
				pLine->fEdge[2][1] = 0.0f;
				pLine->fEdge[2][2] = -ftmp + fwtmp;
				
				pLine->fEdge[3][0] = 0.0f;
				pLine->fEdge[3][1] = -1.0f;
				pLine->fEdge[3][2] = pVert2->fY;
			}

			/* Figure out the front face */
			pLine->fA = 0.0f;
			pLine->fB = fZInvDet;
			pLine->fC = -fZInvDet*pVert1->fY + pLine->fZ1;
			
		}
		else if( delY == 0.0f)
		{
			/* Horizontal line - short cut */
			if(delX < 0.5f)
			{
				delX = -delX;
				if(delX < 0.5f)
				{
					continue;
				}
				/* determinant */
				fDet = delX;
				pLine->f1OverDet = 1.0f/fDet;
				fZInvDet = delZ * pLine->f1OverDet;
				
				ftmp = -delX * pVert2->fY;
				fwtmp = fDet * width;
				
				pLine->fEdge[0][0] = 0.0f; 
				pLine->fEdge[0][1] = delX; 
				pLine->fEdge[0][2] = ftmp + fwtmp;
				
				pLine->fEdge[1][0] = 1.0f;
				pLine->fEdge[1][1] = 0.0f;
				pLine->fEdge[1][2] = -pVert2->fX;

				pLine->fEdge[2][0] = 0.0f;
				pLine->fEdge[2][1] = -delX;
				pLine->fEdge[2][2] = -ftmp + fwtmp;
				
				pLine->fEdge[3][0] = -1.0f;
				pLine->fEdge[3][1] = 0.0f;
				pLine->fEdge[3][2] = pVert1->fX;
			
			}
			else
			{
				/* determinant */
				fDet = delX;
				pLine->f1OverDet = 1.0f/fDet;
				fZInvDet = delZ * pLine->f1OverDet;
				
				ftmp = -delX * pVert1->fY;
				fwtmp = fDet * width;
				
				pLine->fEdge[0][0] = 0.0f; 
				pLine->fEdge[0][1] = delX; 
				pLine->fEdge[0][2] = ftmp + fwtmp;

				pLine->fEdge[1][0] = 1.0f;
				pLine->fEdge[1][1] = 0.0f;
				pLine->fEdge[1][2] = -pVert1->fX;
				
				pLine->fEdge[2][0] = 0.0f;
				pLine->fEdge[2][1] = -delX;
				pLine->fEdge[2][2] = -ftmp + fwtmp;

				pLine->fEdge[3][0] = -1.0f;
				pLine->fEdge[3][1] = 0.0f;
				pLine->fEdge[3][2] = pVert2->fX;
			}

			/* Figure out the front face */
			pLine->fA = fZInvDet;
			pLine->fB = 0.0f;
			pLine->fC = -fZInvDet*pVert1->fX + pLine->fZ1;

		}
		else
		{

			length = delY*delY + delX*delX;
			if(length < 0.5f)
			{
				continue;
			}
			/* if there is a better way !!! */
#if 1
			invLength = InvSqrt(length);
#else
			invLength = 1.0f / ssqrt(length);
#endif
			shiftY = invLength * delX;
			shiftX = -invLength * delY;
			
			/* determinant */
			fDet = ( shiftY * delX - shiftX * delY);

			ftmp = pVert1->fX*pVert2->fY - pVert2->fX*pVert1->fY;
			fwtmp = fDet * width;

			pLine->fEdge[0][0] = -delY; 
			pLine->fEdge[0][1] = delX; 
			pLine->fEdge[0][2] = ftmp + fwtmp;

			pLine->fEdge[1][0] = shiftY;
			pLine->fEdge[1][1] = -shiftX;
			pLine->fEdge[1][2] = ( pVert1->fY * shiftX - pVert1->fX * shiftY);

			pLine->fEdge[2][0] = delY;
			pLine->fEdge[2][1] = -delX;
			pLine->fEdge[2][2] = -ftmp + fwtmp;

			pLine->fEdge[3][0] = -shiftY;
			pLine->fEdge[3][1] = shiftX;
			pLine->fEdge[3][2] = -( pVert2->fY * shiftX - pVert2->fX * shiftY);

			ftmp =  pVert1->fY * shiftX - pVert1->fX * shiftY;
			pLine->f1OverDet = 1.0f/fDet;
			fZInvDet = delZ * pLine->f1OverDet;

			/* Figure out the front face */
			pLine->fA = fZInvDet * shiftY;
			pLine->fB = -fZInvDet * shiftX;
			pLine->fC = fZInvDet * ftmp + pLine->fZ1;
		}

		if (uDefaultType == PACKED_TYPE_TRANSLUCENT)
		{
			sgl_uint32 uDFIndex = (uCurrentType & 1);

			if (!uDoneFirst[uDFIndex])
			{
				uCurrentType |= PACKED_TRANSTYPE_SETMARK;
				uDoneFirst[uDFIndex] = TRUE;
			}
		}

		/* determine region in which line falls - max of 1 regions */
		pLine->reg.u = ENCODE_OBJXYDATA( uCurrentType, rX0, rY0, rX1, rY1 );
		pLine->TSPControlWord = 	gLineDC.TSPControlWord;
		pLine->BaseColour = 		pVert1->u32Colour;
		
		
		/* call the per line func here */
		if(pPerLinefn)
		{
			pPerLinefn(pLine);
		}
		
		if(ProcessFlatTexFn)
		{
			ProcessFlatTexFn(pLine);
		}
		
		if ( ++pLine == &gpLines[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}
		pGDepth++;

	}/*end while*/

	/* Return number in intermediate buffer this time */
	return( (int) ( pLine - gpLines ) );

}

/**********************************************************************/

static void ProcessLines( PPIR pPerLinefn, PBPR pPerBuffn, sgl_uint32 TSPWords, sgl_uint32 nNextLineInc)
{
	/* function to process the context data for the lines */
	sgl_uint32	TSPAddr;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpLineDC->nInputLines != 0 )
	{
		int nBurst;

		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */

		nBurst = ProcessLinesCore( pPerLinefn , NewObject, nNextLineInc);
		NewObject = FALSE;

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
			gpLineDC->nInputLines = 0;
			
			/*
			   This division should only take place if the buffer
			   is nearly full
			*/
			TSPSpaceAvailable /= TSPWords;
			nBurst = TSPSpaceAvailable;
		}

		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;

		PackISPLines(gpLines, gpMat, nBurst, TSPAddr >> 1, TSPWords >> 1);

		/* Call TSP packer */
		pPerBuffn(gpLines, gpMat, nBurst, pTSP);

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)	
	}

}

/**********************************************************************/

void DirectLines( PSGLCONTEXT	pContext,
				  int			nLines,
				  sgl_uint16	pLines[][2],
				  PSGLVERTEX	pVertices,
				  sgl_uint32		nNextLineInc)
{
	TEXAS_PRECOMP_STRUCT 	TPS;
	PIFUNCBLOCK				pFuncBlock;
	PPIR					fnPerPoly;
	sgl_uint32	 			uFuncBlockIndex;
	sgl_int32 				ISPSpaceAvailable;

	ISPSpaceAvailable = ( (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit) -
						  (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos) );

	if ( ISPSpaceAvailable < (nLines * WORDS_PER_PLANE * 5) )
	{
		if (ISPSpaceAvailable > 0)
		{
			/* only once per frame! */
			nLines = ISPSpaceAvailable / (WORDS_PER_PLANE * 5);
		}
		else
		{
			DPFDEV ((DBG_ERROR, "DirectLines: ISP space overflowing"));
			return;
		}
	}

	if(pContext->u32Flags & SGLTT_DEPTHBIAS)
	{
		gfDepthBias = BOGUSINVZ_INCREMENT * 
			(float)(pContext->uDepthBias);
	}
	else
	{
		gfDepthBias = 0.0f;
	}

	/* ini PDC */
	gpLineDC->Context = *pContext; 
	gpLineDC->nInputLines = nLines;
	gpLineDC->pVertices = pVertices; 
	gpLineDC->pLines = (void*) pLines; 

	/*
	// init TSP control word to 0 or not fogged
	*/

	if (!gpLineDC->Context.bFogOn)
	{
		gpLineDC->TSPControlWord = MASK_DISABLE_FOG;
	}
	else
	{
		gpLineDC->TSPControlWord = 0;
		FogUsed = 1;
	}

	/* Need to set the Boguz Inv Z value if it is provided by the user.
	 * This is to be used for the ISP in the depth calculation.
	 */
	if (gpLineDC->Context.u32Flags & SGLTT_BOGUSINVZPROVIDED)
	{
		/* Read the bogus inverse Z provided.
		 * Scale the value correctly.
		 */
		gfBogusInvZ = (gpLineDC->Context.fBogusInvZ * fMinInvZ);
	}

#if PCX2_003
	if (!(gpLineDC->Context.u32Flags & SGLTT_BLENDDISABLE))
	{
		/* Source */
		switch (gpLineDC->Context.uBlendModes & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpLineDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break; 
			case sgl_blend_one: /* Set alpha bit */
				gpLineDC->TSPControlWord |= MASK_BLEND_ALPHA | MASK_TRANS;
				gpLineDC->Context.u32GlobalTrans = 0x0;
				break;
			case sgl_blend_srcalpha: /* Clear alpha bit */
				gpLineDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break;
		}
		/* Destination */
		switch ((gpLineDC->Context.uBlendModes >> 8) & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpLineDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break; 
			case sgl_blend_one: /* Set 1/alpha bit */
				gpLineDC->TSPControlWord |= MASK_BLEND_ONE_OVER_ALPHA | MASK_TRANS | MASK_GLOBAL_TRANS;
  				gpLineDC->Context.u32GlobalTrans = 0x0;
				break;
			case sgl_blend_invsrcalpha: /* Clear 1/alpha bit */
				gpLineDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break;
		}
	}
#endif /* PCX2_003 */

	/*
	// work out which function block we are using
	*/

	/*
	**uFuncBlockIndex = 
	**	gpLineDC->Context.u32Flags & (SGLTT_TEXTURE | SGLTT_HIGHLIGHT);
	*/
	uFuncBlockIndex = 0;

	if (gpLineDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
	{
		pFuncBlock = NoVolFuncs;
	}
	else
	{
		if (gpLineDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
		{
			pFuncBlock = ShadowFuncs;
		}
		else
		{
			pFuncBlock = LightVolFuncs;
		}
	}

	if(gpLineDC->Context.u32Flags & SGLTT_GOURAUD)
	{
		uFuncBlockIndex = 1;
	}
	else if(gpLineDC->Context.u32Flags & SGLTT_HIGHLIGHT)
	{
		uFuncBlockIndex = 2; /* flat shading only just now */
	}
	
	/* set up for global translucency if necessary */

	if ((gpLineDC->Context.u32Flags & SGLTT_GLOBALTRANS) 
		&& (gpLineDC->Context.u32GlobalTrans < 0xF0))
	{
		uFuncBlockIndex |= 0x04;		

		if (!(gpLineDC->Context.u32Flags & SGLTT_VERTEXTRANS))
		{
			sgl_uint32 Alpha = 0xF - (gpLineDC->Context.u32GlobalTrans >> 4);
			gpLineDC->TSPControlWord |= Alpha << 13;
		}
	}
	
	/* 
	// set up for vertex trans 
	// To speed up the Vertex translucency averaging calc,
	// pre-compute 85 * the global trans value / 128.
	*/
	if (gpLineDC->Context.u32Flags & SGLTT_VERTEXTRANS)
	{
		uFuncBlockIndex |= 0x8;
		gpLineDC->u32GlobalTransBy85div128 = (85 * gpLineDC->Context.u32GlobalTrans) >> 7;
	}
	
	/* Y coordinate is in LINES NOT normal REGIONS */
	gpLineDC->Context.FirstYRegion *= RegionInfo.YSize;
	
	if ( ( (gpLineDC->Context.LastYRegion+1) <= RegionInfo.NumYRegions ) ||
		 ( !RegionInfo.HasLeftOver ) )
	{
		/* Calculate accurate end of the Y Region */
		gpLineDC->Context.LastYRegion = ( ( gpLineDC->Context.LastYRegion + 1 ) *
									 RegionInfo.YSize ) - 1;
	}
	else
	{
		/* Last region was not full size, go to last line on screen */
		gpLineDC->Context.LastYRegion = ( ( RegionInfo.NumYRegions - 1 ) *
									 RegionInfo.YSize ) + RegionInfo.LeftOverY;
	}
	
	/* XRegion scaling can be achieved with a shift */
	gpLineDC->ShiftRegX = 5;		/* Start at 32 */
	
	if ( RegionInfo.XSize > (1<<gpLineDC->ShiftRegX) )
	{
		do
		{
			/* Larger shifts, for 64 perhaps */
			gpLineDC->ShiftRegX++;
		}
		while ( RegionInfo.XSize > (1<<gpLineDC->ShiftRegX) );
	}
	else
	{
		while ( RegionInfo.XSize < (1<<gpLineDC->ShiftRegX) )
		{
			/* Smaller Shifts ??? lets be future proof */
			gpLineDC->ShiftRegX--;
		}
	}
	
	ASSERT( ( RegionInfo.XSize == (1<<gpLineDC->ShiftRegX) ) );
	
	/* validate the texture if it's there */
	ProcessFlatTexFn = NULL;	

	if (gpLineDC->Context.u32Flags & SGLTT_TEXTURE)
	{
		sgl_bool fTexIsOK = TRUE;
		HTEXTURE hTexture;

		if (hTexture = GetTextureHandle (gpLineDC->Context.nTextureName))
		{			
			TPS.TexAddress = hTexture->TSPTextureControlWord;
			gpLineDC->TexAddress = hTexture->TSPTextureControlWord;
			gpLineDC->invTexSize = TexasGetInvTextureDimension (&TPS);
		}
		else
		{
			fTexIsOK = FALSE;
		}
		
		if (fTexIsOK)
		{
			gpLineDC->TexAddress = TPS.TexAddress;
			TPS.LowWord = MASK_TEXTURE;
			
			if (gpLineDC->Context.bDoUVTimesInvW)
			{
				ProcessFlatTexFn = ProcessD3DLineTex;
				if (gpLineDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
				{
					gpLineDC->n32MipmapOffset = gpLineDC->Context.n32MipmapOffset;
					ProcessFlatTexFn = ProcessD3DLineTexMipMap;	
				}
			
			}
			else
			{
				ProcessFlatTexFn = ProcessLiteLineTex;
				if (gpLineDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
				{
					gpLineDC->n32MipmapOffset = gpLineDC->Context.n32MipmapOffset;
					ProcessFlatTexFn = ProcessLiteLineTexMipMap;	
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

	ASSERT ((uFuncBlockIndex & 0xFFFFFFE0) == 0);

#ifdef DLL_METRIC
	if (gpLineDC->TSPControlWord & MASK_TRANS)
	{
		nTransPolygonsInFrame += nLines;
	}
	else
	{
		nOpaquePolygonsInFrame += nLines;
	}
#endif
	
	pFuncBlock += uFuncBlockIndex;
	
	gpLineDC->TSPControlWord |= pFuncBlock->TSPControlWord;

	fnPerPoly = pFuncBlock->fnPerPoly;

	/* Set the vertex index to zero.
	 */	
	nLineVertexIndex = 0;

	ProcessLines (fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize, nNextLineInc);
	
	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif


}

/**********************************************************************/

void sgltri_lines ( PSGLCONTEXT  pContext,
					int  nLines,
					sgl_uint16  pLines[][2],
					PSGLVERTEX  pVertices )
{
#ifdef DLL_METRIC   	
   	nTotalPolygonsInFrame += nLines;
#endif
		
	SGL_TIME_START(SGLTRI_TRIANGLES_TIME);
	
	/*
	// ----------------------
	// Check input parameters
	// ----------------------
	*/
	gu32UsedFlags |= pContext->u32Flags;

	if (nLines == 0)
	{
		SglError(sgl_no_err);
	}
	else if (pContext == NULL || pVertices == NULL || pLines==NULL || nLines< 0)
	{
		DPFDEV ((DBG_ERROR, "sgltri_lines: calling with bad parameters"));		
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
		/* Set the index parameter to 1. This is only set to something else
		 * by D3D if DrawPrimitive is the caller.
		 */
		DirectLines (pContext, nLines, pLines, pVertices,1);

		SglError(sgl_no_err);
	}
	SGL_TIME_STOP(SGLTRI_TRIANGLES_TIME)
}

/**********************************************************************/
