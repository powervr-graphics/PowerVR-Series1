/******************************************************************************
 * Name         : dpack.h
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
 * $Log: dpack.h,v $
 * Revision 1.5  1997/11/25  12:55:27  gdc
 * removed redundant code
 *
 * Revision 1.4  1997/10/23  09:53:04  erf
 * Enabled decal texture mode. Implemented using a function ptr
 * variable which selects between PackFlatAndStore() and
 * PackFlatDecalAndStore() etc...
 *
 * Revision 1.3  1997/08/18  16:27:22  gdc
 * removed vertex fog stuff - sort off
 *
 * Revision 1.2  1997/08/13  16:19:32  gdc
 * started adding stuff for vertex fogging
 *
 * Revision 1.1  1997/08/13  10:30:53  gdc
 * Initial revision
 *
 *
 *****************************************************************************/

/*
// ============================================================================
// 						 PER BUFFER PACK ROUTINES (PBPRs)
// ============================================================================
*/

/* Used to select decal texturing mode. Only used for D3D.
 */
typedef	void (*PACKFLAT) (PITRI, PIMATERIAL, int);

PACKFLAT PackFlat = PackFlatAndStore;


static void PackTriHigh (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPHigh (gpTri, gpMat, nPolys, 4, pTSP);
}


static void PackTriFlatTex (sgl_uint32 *pTSP, int nPolys)
{
	/* Call either PackFlatAndStore() or PackFlatDecalAndStore() depending
	 * on whether DECAL selected.
	 */
	PackFlat (gpTri, gpMat, nPolys);

	SGL_TIME_START(TEXTURE_TIME)

	PackTSPTexFn (gpMat, nPolys, FLATTEX, 8, pTSP, sizeof(IMATERIAL));

	SGL_TIME_STOP(TEXTURE_TIME)

}

static void PackTriHighTex (sgl_uint32 *pTSP, int nPolys)
{
	/* Call either PackFlatAndStore() or PackFlatDecalAndStore() depending
	 * on whether DECAL selected.
	 */
	PackFlat (gpTri, gpMat, nPolys);

	SGL_TIME_START(TEXTURE_TIME)

	PackTSPTexFn (gpMat, nPolys, HIGHTEX, 10, pTSP, sizeof(IMATERIAL));

	SGL_TIME_STOP(TEXTURE_TIME)
}

static void PackTriSmooth (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPSmooth (gpTri, gpMat, nPolys, 4, 0, pTSP);
}

static void PackTriNativeSmooth (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPSmooth (gpTri, gpMat, nPolys, 4, 0, pTSP);
}

static void PackTriSmoothTex (sgl_uint32 *pTSP, int nPolys)
{
	PackSmoothAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)

	PackTSPTexFn (gpMat, nPolys, SMOOTHTEX, 10, pTSP, sizeof(IMATERIAL));

	SGL_TIME_STOP(TEXTURE_TIME)
}

static void PackTriNativeSmoothTex (sgl_uint32 *pTSP, int nPolys)
{
	PackSmoothAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)
	PackTSPTexFn (gpMat, nPolys, SMOOTHTEX, 10, pTSP, sizeof(IMATERIAL));
	SGL_TIME_STOP(TEXTURE_TIME)
}

/*
// ============================================================================
// 			 PER BUFFER PACK ROUTINES WITH SHADOWS (PBPRs)
// ============================================================================
*/

static void PackTriFlatShad (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPFlatShad (gpTri, gpMat, nPolys, 2, pTSP);
}

static void PackTriHighShad (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPHighShad (gpTri, gpMat, nPolys, 4, pTSP);
}

static void PackTriFlatTexShad (sgl_uint32 *pTSP, int nPolys)
{
	PackFlatShadAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)
	PackTSPTexFn (gpMat, nPolys, FLATTEX, 8, pTSP, sizeof(IMATERIAL));
	SGL_TIME_STOP(TEXTURE_TIME)
}

static void PackTriHighTexShad (sgl_uint32 *pTSP, int nPolys)
{
	PackFlatShadAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)
	PackTSPTexFn (gpMat, nPolys, HIGHTEX, 10, pTSP, sizeof(IMATERIAL));
	SGL_TIME_STOP(TEXTURE_TIME)
}

static void PackTriSmoothShad (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPSmoothShad (gpTri, gpMat, nPolys, 6, 0, pTSP);
}

static void PackTriSmoothTexShad (sgl_uint32 *pTSP, int nPolys)
{
	PackSmoothShadAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)

	PackTSPTexFn (gpMat, nPolys, SMOOTHSHADTEX, 12, pTSP, sizeof(IMATERIAL));

	SGL_TIME_STOP(TEXTURE_TIME)
}

/*
// ============================================================================
// 				 PER BUFFER PACK ROUTINES WITH LIGHT VOLS (PBPRs)
// ============================================================================
*/

static void PackTriFlatLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPFlatLiVol (gpTri, gpMat, nPolys, 2, pTSP);
}

static void PackTriHighLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPHighLiVol (gpTri, gpMat, nPolys, 4, pTSP);
}

static void PackTriFlatTexLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackFlatLiVolAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)
	PackTSPTexFn (gpMat, nPolys, FLATTEX, 8, pTSP, sizeof(IMATERIAL));
	SGL_TIME_STOP(TEXTURE_TIME)
}

static void PackTriHighTexLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackFlatLiVolAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)
	PackTSPTexFn (gpMat, nPolys, HIGHTEX, 10, pTSP, sizeof(IMATERIAL));
	SGL_TIME_STOP(TEXTURE_TIME)
}

static void PackTriSmoothLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPSmoothLiVol (gpTri, gpMat, nPolys, 6, 0, pTSP);
}

static void PackTriSmoothTexLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackSmoothLiVolAndStore (gpTri, gpMat, nPolys);
	SGL_TIME_START(TEXTURE_TIME)
	PackTSPTexFn (gpMat, nPolys, SMOOTHSHADTEX, 12, pTSP, sizeof(IMATERIAL));
	SGL_TIME_STOP(TEXTURE_TIME)
}


#undef FLATTEX
#undef SMOOTHTEX
#undef HIGHTEX
#undef SMOOTHSHADTEX

/*
// ============================================================================
// 		  PER BUFFER PACK ROUTINES WITH NON-TEXTURED TRANSLUCENCY (PBPRs)
// ============================================================================
*/

static void PackTriFlatTrans (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPFlatTrans (gpTri, gpMat, nPolys, 8, pTSP);
}


static void PackTriSmoothTrans (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPSmooth (gpTri, gpMat, nPolys, 10, 6, pTSP);
	PackTSPTransTex (gpTri, gpMat, nPolys, -2, 10, pTSP + 2);
}

static void PackTriNativeSmoothTrans (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPSmooth (gpTri, gpMat, nPolys, 10, 6, pTSP);
	PackTSPTransTex (gpTri, gpMat, nPolys, -2, 10, pTSP + 2);
}

static void PackTriHighTrans (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPHighTrans (gpTri, gpMat, nPolys, 10, pTSP);
}

static void PackTriFlatTransShad (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPFlatTransShad (gpTri, gpMat, nPolys, 8, pTSP);
}

static void PackTriHighTransShad (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPHighTransShad (gpTri, gpMat, nPolys, 10, pTSP);
}

static void PackTriFlatTransLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPFlatTransLiVol (gpTri, gpMat, nPolys, 8, pTSP);
}

static void PackTriHighTransLiVol (sgl_uint32 *pTSP, int nPolys)
{
	PackTSPHighTransLiVol (gpTri, gpMat, nPolys, 10, pTSP);
}





