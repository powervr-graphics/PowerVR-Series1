/******************************************************************************
 * Name         : dprocess.h
 * Title        : Low level API top level code for PowerVR
 * Author       : Graham Connor
 * Created      : 13/8/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dprocess.h,v $
 * Revision 1.14  1997/11/11  10:21:18  gdc
 * made gouraud highlight white and changed its sape a bit
 *
 * Revision 1.13  1997/08/29  11:42:28  gdc
 * fixed bug where wrong translucency level was being set
 * and trans bits were overwriting the texture exponent
 *
 * Revision 1.12  1997/08/29  09:57:21  gdc
 * removed ^M's and made sure vfog and smooth highlight indices are
 * zeroed if not in use (and on or other feature is on)
 *
 * Revision 1.11  1997/08/27  15:48:43  ls
 * Renamed IMPLEMENT_VERTEX_TRANS IMPLEMENT_AVG_VERTEX TRANS
 * renamed IMPLEMENT_VERTEX_GLOBAL_TRANS IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
 * introduced non-averaging vertex Alpha: uses 1st vertex Alpha only
 * Any Process which uses Smooth or Flatten/Fltn uses the average vertex alpha
 * otherwise use 1st vertex alpha
 *
 * Revision 1.10  1997/08/27  08:48:02  sks
 * Added new functions for smooth specular highlights.
 *
 * Revision 1.9  1997/08/26  14:14:30  sks
 * Removed stuff from FakeHighlight().
 *
 * Revision 1.8  1997/08/26  12:40:23  sks
 * More work on FakeHighlight function
 *
 * Revision 1.7  1997/08/22  16:36:10  sks
 * Started work on smooth highlights function: FakeHighlight().
 *
 * Revision 1.6  1997/08/22  10:36:39  gdc
 * added other vertex fog routines for flat and avg flat, and a non functional
 * stub for smooth highlights
 *
 * Revision 1.5  1997/08/19  14:38:32  gdc
 * added average colour with veretx fog functions
 *
 * Revision 1.4  1997/08/19  11:50:41  gdc
 * got sense of fog index right way around
 *
 * Revision 1.3  1997/08/18  16:26:31  gdc
 * dummied up routines to test vertex fog
 *
 * Revision 1.2  1997/08/13  16:19:58  gdc
 * added process functions for per vertex fogging
 *
 * Revision 1.1  1997/08/13  10:29:54  gdc
 * Initial revision
 *
 *
 *****************************************************************************/
/**********************************************************************/

/* these defines are ripped off from dshade.c */ 
#define  READ_RBGS_VIA_BYTE_PTRS (HAS_FAST_BYTE_LOAD)

#if READ_RBGS_VIA_BYTE_PTRS

	#if BIG_ENDIAN
		#define R_BYTE_OFFSET 3
		#define G_BYTE_OFFSET 2
		#define B_BYTE_OFFSET 1
	#else
		#define R_BYTE_OFFSET 0
		#define G_BYTE_OFFSET 1
		#define B_BYTE_OFFSET 2
	#endif

#define GET_R(UINT32_LOCATION) (((const sgl_uint8 *) & UINT32_LOCATION) [R_BYTE_OFFSET])
#define GET_G(UINT32_LOCATION) (((const sgl_uint8 *) & UINT32_LOCATION) [G_BYTE_OFFSET])
#define GET_B(UINT32_LOCATION) (((const sgl_uint8 *) & UINT32_LOCATION) [B_BYTE_OFFSET])

#else

#define GET_R(x)				(((x) >> 16) & 0x000000FF)
#define GET_G(x)				(((x) >> 8) & 0x000000FF)
#define GET_B(x)				((x) & 0x000000FF)

#endif 

#define fIntensityScale (15.0f / (3.0f * 255.0f))	

static int LookUp[16] = { 0x0, 0x0, 0x0, 0x0,
						  0x0, 0x1, 0x3, 0x5, 0x7, 0x9, 0xb, 0xd,
						  0xf, 0xf, 0xf, 0xf };

static sgl_uint32 FakeHighlight (PROCESSDATACONTEXT pPDC, sgl_uint32 *SpecularHighlightBaseColour) 
{
	sgl_uint32 RTotal, GTotal, BTotal;
	sgl_uint V0Int,	V1Int,	V2Int;
	sgl_uint  nTmp;
	float fLargest;
	float fColourScale;

	sgl_uint32 FinalColour;
	sgl_uint32 Colour;
	sgl_uint32 Index;
	/*
	// Extract each of the red green and blue components from each of the
	// three vertex colours, and sum up the totals of the R,G,Bs PLUS the
	// total of each component for the three verts, which will be used to
	// compute an "intensity"
	*/
#if READ_RBGS_VIA_BYTE_PTRS
	/* Vertex 0 */
	RTotal = GET_R(pPDC.pV0->u32Specular);
	GTotal = GET_G(pPDC.pV0->u32Specular);
	BTotal = GET_B(pPDC.pV0->u32Specular);
	V0Int  = RTotal + GTotal + BTotal;
	/* Vertex 1 */
	V1Int  = GET_R(pPDC.pV1->u32Specular);
	RTotal+= V1Int;

	nTmp   = GET_G(pPDC.pV1->u32Specular);
	GTotal+= nTmp;
	V1Int += nTmp;

	nTmp   = GET_B(pPDC.pV1->u32Specular);
	BTotal+= nTmp;
	V1Int += nTmp;
	/* Vertex 2 */
	V2Int  = GET_R(pPDC.pV2->u32Specular);
	RTotal+= V2Int;

	nTmp   = GET_G(pPDC.pV2->u32Specular);
	GTotal+= nTmp;
	V2Int += nTmp;

	nTmp   = GET_B(pPDC.pV2->u32Specular);
	BTotal+= nTmp;
	V2Int += nTmp;

#else
	/* Vertex 0 */
	Colour = pPDC.pV0->u32Specular;
	RTotal = GET_R(Colour);
	GTotal = GET_G(Colour);
	BTotal = GET_B(Colour);
	V0Int  = RTotal + GTotal + BTotal;

	/* Vertex 1 */
	Colour = pPDC.pV1->u32Specular;
	V1Int  = GET_R(Colour);
	RTotal+= V1Int;

	nTmp   = GET_G(Colour);
	GTotal+= nTmp;
	V1Int += nTmp;

	nTmp   = GET_B(Colour);
	BTotal+= nTmp;
	V1Int += nTmp;

	/* Vertex 2 */
	Colour = pPDC.pV2->u32Specular;
	V2Int  = GET_R(Colour);
	RTotal+= V2Int;

	nTmp   = GET_G(Colour);
	GTotal+= nTmp;
	V2Int += nTmp;

	nTmp   = GET_B(Colour);
	BTotal+= nTmp;
	V2Int += nTmp;

#endif

#define COLOUR_HIGHLIGHT 0
#if COLOUR_HIGHLIGHT
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
		*SpecularHighlightBaseColour = 0;
		return 0;
	}

	/* Calculate the base colour */

	fColourScale = ((3.0f * 255.0f) / (fLargest));

	RTotal = (sgl_uint32)( (float)(RTotal) * fColourScale );
	GTotal = (sgl_uint32)( (float)(GTotal) * fColourScale );
	BTotal = (sgl_uint32)( (float)(BTotal) * fColourScale );

	FinalColour = ((RTotal & 0xff)<<16) | ((GTotal & 0xff)<<8) | (BTotal & 0xff);

	*SpecularHighlightBaseColour = FinalColour;
#else
	*SpecularHighlightBaseColour = 0x00ffffff;
#endif
	/*
	// Calculate the texture coordinates
	*/

	V0Int = (sgl_uint)( (float)(V0Int) * fIntensityScale );
	V1Int = (sgl_uint)( (float)(V1Int) * fIntensityScale );
	V2Int = (sgl_uint)( (float)(V2Int) * fIntensityScale );

	Index = (LookUp[(V0Int & 0xf)] << 8) | (LookUp[(V1Int & 0xf)] << 4) |
		LookUp[(V2Int & 0xf)] ;

	return (Index);
}

/*
// ============================================================================
// 			   PER POLYGON INTERMEDIATE ROUTINE MACROS AND GLOBALS
// ============================================================================
*/

/* 
//	Average flat shading parameters
//
//	This is for when smooth shading is knocked of by translucency   
//	average the smooth colour into the base colour   
//
//	instead of doing the 1/3 in fp land, do it in integer
//	land. 85/256 = 0.332. Save some shifts and go straight
//	to the correct bit range.
*/
/*
// New Implementation:
// Leave all the numbers in the same place: Note we have
// JUST enough head room for Red as worst case is
//			   (0xFF * 3) * 85 = 0xFE01
//
// By the same idea, we can keep R & B in the SAME variable!
// since they are separated by enough space.
//
// Furthermore we can add ALL the RBG values together, and then
// at the end get just the RB by subtracting out G's total.
//
// Since we can't guarantee the content of very top 8 bits
// of each input colour, accumulate these in the G as well.
// 
// Saves loads of instructions - Sometimes I amaze even myself ;)
// Who needs MMX!
// Simon 
*/
#define IMPLEMENT_AVG_FLAT(Result, Src)					    \
{															\
	sgl_uint32 RedNBlue, GreenNTop;							\
	RedNBlue   = Src.pV0->u32Colour;						\
	GreenNTop  = Src.pV0->u32Colour & 0xFF00FF00;			\
	RedNBlue  += Src.pV1->u32Colour;						\
	GreenNTop += Src.pV1->u32Colour & 0xFF00FF00;			\
	RedNBlue  += Src.pV2->u32Colour;						\
	GreenNTop += Src.pV2->u32Colour & 0xFF00FF00;			\
	RedNBlue  -= GreenNTop;									\
	GreenNTop *= 85;										\
	RedNBlue  *= 85;										\
	GreenNTop >>= 8;										\
	RedNBlue  >>= 8;										\
	Result=	   (RedNBlue  & 0x00FF0000)						\
			 | (GreenNTop & 0x0000FF00)						\
			 | (RedNBlue  & 0x000000FF);					\
}


/*********************************/

/*
// Ok the rest have had the same treatment
*/
/*
// For the highlight we are going down to just 5 bits
//
// Target positions:
// 	 Red:30-26, Green:25-21, Blue:20-16
//
// After the multiply by 85, we end up with
//   R&B:  Red:31-16  Blue:15-0
// and
//   G:	   Green:23-8
//
// Now the shift for Green is UP two places (since the
// most sig bit moves from 23 to 25). Since the last thing
// we did was a multiply we can combine the two to give a
// multiply by (85*4) instead!
//
// Red must go down one spot and Blue up lots. Rather than
// do a multiply by 85 and then an effective shift down, may
// as well only multiply by 42. Since we are only going to
// 5 bits in the end, it comes out with the same accuracy.
// Thus we really have R&B in the positions
//   R&B:  Red:30-15  Blue:14-0
//
*/
#define IMPLEMENT_AVG_HIGHLIGHT(Result, Src)				\
		AVERAGE_RBG_TO_555_TOP(Result, Src.pV0->u32Specular,\
									   Src.pV1->u32Specular,\
									   Src.pV2->u32Specular)

/*********************************/

#define IMPLEMENT_HIGHLIGHT									\
{															\
	sgl_uint32 H = gPDC.pV0->u32Specular;					\
	pMat->Shading.u.Highlight = ((H << 07) & 0x7C000000) |	\
								((H << 10) & 0x03E00000) |	\
								((H << 13) & 0x001F0000);	\
}

/*********************************/

#define IMPLEMENT_SMOOTH									\
	pMat->Shading.u.Smooth.Col0 = gPDC.pV0->u32Colour;		\
	pMat->Shading.u.Smooth.Col1 = gPDC.pV1->u32Colour;		\
	pMat->Shading.u.Smooth.Col2 = gPDC.pV2->u32Colour;		\
	pMat->Shading.u.Smooth.nX = (int) gPDC.pV0->fX;			\
	pMat->Shading.u.Smooth.nY = (int) gPDC.pV0->fY;			

#if 0
#define IMPLEMENT_NATIVE_SMOOTH											\
	pMat->Shading.u.NativeSmooth.fIntensity[0] = gPDC.pV0->fIntensity;	\
	pMat->Shading.u.NativeSmooth.fIntensity[1] = gPDC.pV1->fIntensity;	\
	pMat->Shading.u.NativeSmooth.fIntensity[2] = gPDC.pV2->fIntensity;	\
	pMat->Shading.u.NativeSmooth.nX = (int) gPDC.pV0->fX;				\
	pMat->Shading.u.NativeSmooth.nY = (int) gPDC.pV0->fY;			
#else
#define IMPLEMENT_NATIVE_SMOOTH								\
	pMat->Shading.u.Smooth.Col0 = gPDC.pV0->u32Colour;		\
	pMat->Shading.u.Smooth.Col1 = gPDC.pV1->u32Colour;		\
	pMat->Shading.u.Smooth.Col2 = gPDC.pV2->u32Colour;		\
	pMat->Shading.u.Smooth.nX = (int) gPDC.pV0->fX;			\
	pMat->Shading.u.Smooth.nY = (int) gPDC.pV0->fY;			
#endif

/*****************************************************************/
#define IMPLEMENT_VERTEX_TRANS                    \
{                                                 \
	sgl_uint32 Alpha;                             \
    Alpha = gPDC.pV0->u32Colour >> 28;            \
	pTri->TSPControlWord |= (0xF - Alpha) << 13;  \
}


/*
// We want to get the average of three 8 bit integer Alpha values,
// which are stored in the top bits of the colours, get only
// the top 4 bits of the average, reverse the meaning of alpha
// (ie by  subtracting the 4 bit value from 0xF) and then store
// the 4 bit result back in bits 16..13 (inclusive and counting
// from bit 0) of the TSP control word.
//
// First get the top 8 bits only by shifting down by 24, and add
// them. Now if we were to multiply this by 85 (which effectively
// is a division by 3 if you shift down by 8), we get the most
// sig bit in bit 15. 
//
// Since we want the most sig bit in bit 16 we instead multiply
// by (2*85).
//
// We then mask to get just the 4 most sig bits, and subtract
// this from "F" in the correct pos to reverse the sense of alpha
// from D3D style to PCX1 style.
*/
#define IMPLEMENT_AVG_VERTEX_TRANS								\
{																\
	sgl_uint32 Alpha;											\
	Alpha = (gPDC.pV0->u32Colour >> 24) +						\
			(gPDC.pV1->u32Colour >> 24) +						\
			(gPDC.pV2->u32Colour >> 24);						\
	Alpha *= (2*85);											\
	Alpha &= (0xF << 13);										\
	Alpha  = (0xF << 13) - Alpha;								\
	pTri->TSPControlWord |= Alpha;								\
}


/****************************************************************/

/*
// This macro is similar to the previous EXCEPT we must scale
// everything by the global alpha value. This is an 8 bit value,
// so after multiplying by it, everything is shifted up by a
// further 8 bits.
//
//  Thus after we multiply by (85 * gPDC.Context.u32GlobalTrans)
//  the most sig bit of the Alpha is in bit 23 (i.e. 15+8). To move
//	it into bit 16 (where we want it), shift down by 7.
//
//	To eliminate the shift by 7 and 85 * gPDC.Context.u32GlobalTrans
// these have been precalculated in gPDC.u32GlobalTransBy85div128
// This does introduce some errors, but having checked it on a
// spreadsheet and noting that we only want 4 bits as a final
// result, the error is insignificant.
*/
#define IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS						\
{																\
	sgl_uint32 Alpha;											\
	Alpha = (gPDC.pV0->u32Colour >> 24) +						\
			(gPDC.pV1->u32Colour >> 24) +						\
			(gPDC.pV2->u32Colour >> 24);						\
	Alpha *= gPDC.u32GlobalTransBy85div128;						\
	Alpha &= (0xF << 13);										\
	Alpha  = (0xF << 13) - Alpha;								\
	pTri->TSPControlWord |= Alpha;								\
}

#define IMPLEMENT_VERTEX_GLOBAL_TRANS			  \
{												  \
    sgl_uint32 Alpha;                             \
    Alpha = gPDC.pV0->u32Colour >> 24;            \
	Alpha += (Alpha + Alpha);					  \
	Alpha *= gPDC.u32GlobalTransBy85div128;		  \
	Alpha &= (0xF << 13);						  \
	Alpha  = (0xF << 13) - Alpha;				  \
	pTri->TSPControlWord |= Alpha;                \
}


/*********************************/

#define IMPLEMENT_SHADOW_LIGHTVOL \
	pMat->v.LightVolCol = gPDC.Context.u.u32LightVolColour;

/*********************************/

/*
// To implement vertex fogging we add an extra plane in a special
// fog pass, this plane uses a special fog texture a la NTTrans
// but this must have texture coords to map a texture with linear
// alpha variation in the U direction to an appropriate bit of
// fog - this is done via a look up, the index of which is calculated
// below
*/

#define IMPLEMENT_VERTEX_FOG									\
{																\
	sgl_uint32 nFogIndex;										\
	nFogIndex = (((gPDC.pV0->u32Specular & 0xF0000000) >> 20) |	\
			((gPDC.pV1->u32Specular & 0xF0000000)>> 24) |		\
			((gPDC.pV2->u32Specular & 0xF0000000)>> 28));		\
	pMat->nFogIndex = (~nFogIndex)&0xfffUL;                 	\
	pMat->nSmoothSpecularIndex = 0;        								\
}

/* 
// As above but in flat shading mode the fog is flat
// Only use the first vertex for the properties
*/

#define IMPLEMENT_FLAT_VERTEX_FOG								\
{																\
	sgl_uint32 nFogIndex;										\
	sgl_uint32 nFogTmp;										    \
	nFogTmp = ((gPDC.pV0->u32Specular) >> 28);	                \
	nFogIndex = nFogTmp | nFogTmp << 4 | nFogTmp << 8;          \
	pMat->nFogIndex = (~nFogIndex);                 	        \
	pMat->nSmoothSpecularIndex = 0;        								\
}

/* 
// As above but in average colour flat shading mode the fog is flat
// But uses average of the all vertices for the properties
*/

#define IMPLEMENT_AVG_VERTEX_FOG								\
{																\
	sgl_uint32 nFogIndex;										\
	sgl_uint32 nFogTmp;										    \
	nFogTmp = (gPDC.pV0->u32Specular >> 28)+	                \
	          (gPDC.pV1->u32Specular >> 28)+	                \
	          (gPDC.pV2->u32Specular >> 28);	                \
	nFogTmp = (nFogTmp * 85)>> 8;                               \
	nFogIndex = nFogTmp | nFogTmp << 4 | nFogTmp << 8;          \
	pMat->nFogIndex = (~nFogIndex)&0xfffUL;                 	\
	pMat->nSmoothSpecularIndex = 0;        								\
}


/*
// ============================================================================
// 			        PER POLYGON INTERMEDIATE ROUTINES (PPIRs)
// ============================================================================
*/
static void ProcessFlatten( PITRI pTri)
{
	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
}

static void ProcessFlatHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
}

static void ProcessFlattenHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
}

static void ProcessFlattenTex (PITRI pTri)
{
    /* No gpMatCurrent increment*/

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlatTexHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessSmooth (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_SMOOTH
}

static void ProcessSmoothHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_SMOOTH;
	pMat->nFogIndex = 0;
	pMat->nSmoothSpecularIndex= FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessSmoothTexHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_SMOOTH;
	pMat->nFogIndex = 0;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessSmoothTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SMOOTH
}

static void ProcessSmoothTransHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SMOOTH
	pMat->nFogIndex = 0;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessSmoothGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SMOOTH
}

static void ProcessSmoothGTransHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SMOOTH
	pMat->nFogIndex = 0;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessSmoothTex (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_SMOOTH
/*	ProcessFlatTexFn( pTri ); */
}


static void ProcessSmoothTexTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SMOOTH
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessSmoothTexTransHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SMOOTH
	pMat->nFogIndex = 0;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessSmoothTexGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SMOOTH
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessSmoothTexGTransHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SMOOTH
	pMat->nFogIndex = 0;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
/*	ProcessFlatTexFn( pTri ); */
}

/*
// ============================================================================
// PER TRIANGLE VERTEX FOG PROCESS ROUTINES
// ============================================================================
*/

static void ProcessFlatVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessSmoothVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_FOG;
	IMPLEMENT_SMOOTH
}

static void ProcessSmoothVFogHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_VERTEX_FOG;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessFlatTexVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessSmoothTexVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_FOG;
	IMPLEMENT_SMOOTH
}

static void ProcessSmoothTexVFogHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_VERTEX_FOG;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessFlatHighlightVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessFlatTexHighlightVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_FLAT_VERTEX_FOG;
}

/*
// ========================================
// PER TRIANGLE VERTEX FOG PROCESS ROUTINES
// WITH AVERAGE COLOUR
// ========================================
*/

static void ProcessFlattenVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_FOG;
}


static void ProcessFlattenHighlightVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessFlattenTexVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessFlattenTexHighlightVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_FOG;
}

/*
// ========================================
// PER TRIANGLE VERTEX FOG PROCESS ROUTINES
// WITH VERTEX TRANSLUCENCY
// ========================================
*/

static void ProcessFlatTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessSmoothTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_VERTEX_FOG;
}

static void ProcessSmoothTransVFogHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_VERTEX_FOG;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessFlatTexTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessSmoothTexTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_VERTEX_FOG;
}

static void ProcessSmoothTexTransVFogHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_VERTEX_FOG;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessHighTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessHighTexTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

/*
// ========================================
// PER TRIANGLE VERTEX FOG PROCESS ROUTINES
// WITH VERTEX TRANSLUCENCY
// AND AVERAGE COLOUR
// ========================================
*/

static void ProcessFlattenTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}


static void ProcessHighFltnTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessHighFltnTexTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessFlattenTexTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}

/*
// ========================================
// PER TRIANGLE VERTEX FOG PROCESS ROUTINES
// WITH VERTEX & GLOBAL TRANSLUCENCY
// ========================================
*/

static void ProcessFlatGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessSmoothGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_VERTEX_FOG;
}

static void ProcessSmoothGTransVFogHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_VERTEX_FOG;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessFlatTexGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessSmoothTexGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_VERTEX_FOG;
}

static void ProcessSmoothTexGTransVFogHighlight (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_VERTEX_FOG;
	pMat->nSmoothSpecularIndex = FakeHighlight(gPDC, &pMat->nSmoothSpecularColour);
}

static void ProcessHighGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_GLOBAL_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

static void ProcessHighTexGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_GLOBAL_TRANS
	IMPLEMENT_FLAT_VERTEX_FOG;
}

/*
// ========================================
// PER TRIANGLE VERTEX FOG PROCESS ROUTINES
// WITH VERTEX & GLOBAL TRANSLUCENCY 
// AND AVERAGE COLOUR
// ========================================
*/

static void ProcessFlattenGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessFlattenTexGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessHighFltnGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}

static void ProcessHighFltnTexGTransVFog (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_AVG_VERTEX_FOG;
}
/*
// ============================================================================
// NATIVE SMOOTH SHADING:
// ============================================================================
*/

static void ProcessNativeSmooth (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_NATIVE_SMOOTH
}

static void ProcessNativeSmoothTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_NATIVE_SMOOTH
}

static void ProcessNativeSmoothGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_NATIVE_SMOOTH
}

static void ProcessNativeSmoothTex (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;
	
	IMPLEMENT_NATIVE_SMOOTH
/*	ProcessFlatTexFn( pTri ); */
}


static void ProcessNativeSmoothTexTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_NATIVE_SMOOTH
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessNativeSmoothTexGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_NATIVE_SMOOTH
/*	ProcessFlatTexFn( pTri ); */
}

/*
// ============================================================================
// 		   PER POLYGON INTERMEDIATE ROUTINES WITH SHADOWS OR L-Vs (PPIRs)
// ============================================================================
*/

static void ProcessFlatShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessFlattenShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessFlatHighlightShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessFlattenHighlightShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessFlatTexShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlatTexHighlightShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexHighlightShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessSmoothShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessSmoothTexShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_SMOOTH
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}


/**********************************************************************/
/* PROCESS FUNCTIONS FOR PER VERTEX TRANSLUCENT TRIS (NON-TEXTURED)   */
/**********************************************************************/

static void ProcessFlatTrans (PITRI pTri)
{
	IMPLEMENT_VERTEX_TRANS
}

static void ProcessFlattenTrans (PITRI pTri)
{
	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
}

static void ProcessHighTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_TRANS
}

static void ProcessHighFltnTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
}

static void ProcessFlatGTrans (PITRI pTri)
{
	IMPLEMENT_VERTEX_GLOBAL_TRANS
}

static void ProcessFlattenGTrans (PITRI pTri)
{
	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
}

static void ProcessHighGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_GLOBAL_TRANS
}

static void ProcessHighFltnGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
}

static void ProcessFlatTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessFlattenTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessHighTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessHighFltnTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessFlatGTransShadOrLiVol (PITRI pTri)
{
	IMPLEMENT_VERTEX_GLOBAL_TRANS
}

static void ProcessFlattenGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessHighGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

static void ProcessHighFltnGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
}

/**********************************************************************/
/* PROCESS FUNCTIONS FOR PER VERTEX TRANSLUCENT TRIS (TEXTURED)       */
/**********************************************************************/

static void ProcessFlatTexTrans (PITRI pTri)
{
	IMPLEMENT_VERTEX_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexTrans (PITRI pTri)
{
	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighTexTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighFltnTexTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlatTexGTrans (PITRI pTri)
{
	IMPLEMENT_VERTEX_GLOBAL_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexGTrans (PITRI pTri)
{
	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighTexGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_GLOBAL_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighFltnTexGTrans (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlatTexTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighTexTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighFltnTexTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlatTexGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessFlattenTexGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighTexGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_HIGHLIGHT
	IMPLEMENT_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

static void ProcessHighFltnTexGTransShadOrLiVol (PITRI pTri)
{
	PIMATERIAL pMat = gpMatCurrent;

	IMPLEMENT_AVG_FLAT(pTri->BaseColour, gPDC);
	IMPLEMENT_AVG_HIGHLIGHT(pMat->Shading.u.Highlight, gPDC);
	IMPLEMENT_AVG_VERTEX_GLOBAL_TRANS
	IMPLEMENT_SHADOW_LIGHTVOL
/*	ProcessFlatTexFn( pTri ); */
}

