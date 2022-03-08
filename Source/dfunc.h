/******************************************************************************
 * Name         : dshade.c
 * Title        : Smooth shading routines for PowerSGL Direct
 * Author       : Jim Page
 * Created      : November 1996
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
 * $Log: dfunc.h,v $
 * Revision 1.17  1997/08/27  08:51:22  sks
 * Added smooth specular highlight functions to funcblocks.
 *
 * Revision 1.16  1997/08/26  12:41:08  gdc
 * added new function entries to fake smooth highlights
 *
 * Revision 1.15  1997/08/19  14:38:07  gdc
 * added average colour with vertex fog
 *
 * Revision 1.14  1997/08/18  16:26:53  gdc
 * chnaged stuff I'd added for vertex fog
 *
 * Revision 1.13  1997/08/13  16:31:16  gdc
 * started adding stuff for vertex fogging
 *
 * Revision 1.12  1997/07/14  19:25:24  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.11  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.10  1997/01/18  01:01:41  jrc
 * First go at native shading API (not finished).
 *
 * Revision 1.9  1996/12/19  00:49:05  jrc
 * Added average colour processing cases.
 *
 * Revision 1.8  1996/12/10  23:36:43  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.7  1996/12/03  15:03:33  mjg
 * Altered the previous correction for consistency and clarity - for the
 * moment, there is only one difference between Midas3 and others. In the
 * future it would be advisable to have a separate block for each compile
 * switch.
 *
 * Revision 1.6  1996/12/03  10:53:05  mjg
 * Removed the option of flat shaded highlights for Midas3.
 *
 * Revision 1.5  1996/11/28  10:29:44  gdc
 * when non-shad/LV smooth shading is now retained at the expense of
 * highlight when translucent - c.f. non-translucent case
 *
 * Revision 1.4  1996/11/27  17:31:27  gdc
 * added handles so smooth shaded translucency is supported when there
 * are no shads/L-Vs; also added V-L file header; and OR MASK_TEXTURE for
 * translucent options so that non-texture trans works properly
 *
 *****************************************************************************/

typedef struct tagIFUNCBLOCK
{
	PPIR	fnPerPoly;
	PBPR	fnPerBuffer;
	sgl_uint32	uSize;
	sgl_uint32	TSPControlWord;
	sgl_uint32	uSize2;
	sgl_uint32	TSPControlWord2;

} IFUNCBLOCK, *PIFUNCBLOCK;

#define NOSMOOTH		MASK_SMOOTH_SHADE
#define NOHIGH			MASK_FLAT_HIGHLIGHT
#define NOSMOOTHNOHIGH	MASK_FLAT_HIGHLIGHT | MASK_SMOOTH_SHADE

#define FLAGS(x,y)		((BASE_FLAGS | (FLAGS_ ## x))&(~(y)))

#define FLAGS_0			0
#define FLAGS_1			MASK_SMOOTH_SHADE
#define FLAGS_2			MASK_TEXTURE
#define FLAGS_3			MASK_TEXTURE | MASK_SMOOTH_SHADE
#define FLAGS_4			MASK_FLAT_HIGHLIGHT
#define FLAGS_5			MASK_FLAT_HIGHLIGHT | MASK_SMOOTH_SHADE
#define FLAGS_6			MASK_FLAT_HIGHLIGHT | MASK_TEXTURE
#define FLAGS_7			MASK_FLAT_HIGHLIGHT | MASK_TEXTURE | MASK_SMOOTH_SHADE


/*
// =======================
// USE FIRST VERTEX COLOUR
// =======================
*/

/*
// NO SHADOWS OR LIGHT VOLUMES
*/

/*
// Traditional/D3D smooth shading
*/

IFUNCBLOCK NoVolFuncs[32] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{NULL, 							NULL, 						2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmooth, 				PackTriSmooth, 				4,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlatTex, 				PackTriFlatTex, 			8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTex, 				PackTriSmoothTex, 			10,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlatHighlight, 			PackTriHigh, 				4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothHighlight, 		PackTriSmooth, 				4,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlatTexHighlight, 		PackTriHighTex, 			10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexHighlight,     PackTriSmoothTex, 			10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global alpha
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE
	{NULL, 							PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmooth,      			PackTriSmoothTrans,			10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlatTex, 				PackTriFlatTex, 			8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTex, 			    PackTriSmoothTex, 			10,	FLAGS(3, 0), 0, 0 },
	{ProcessFlatHighlight, 			PackTriHighTrans, 			10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothHighlight,   		PackTriSmoothTrans,			10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlatTexHighlight, 		PackTriHighTex, 			10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexHighlight,	    PackTriSmoothTex, 			10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Vertex alpha
	*/
	{ProcessFlatTrans, 				PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothTrans,		 	PackTriSmoothTrans,		   10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlatTexTrans, 			PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 },
	{ProcessSmoothTexTrans, 		PackTriSmoothTex,		   10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighTrans, 				PackTriHighTrans, 		   10,	FLAGS(4, 0), 0, 0 },
	{ProcessSmoothTransHighlight,   PackTriSmoothTrans, 	   10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighTexTrans, 			PackTriHighTex,			   10,	FLAGS(6, 0), 0, 0 },
	{ProcessSmoothTexTransHighlight,PackTriSmoothTex,		   10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlatGTrans, 			PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothGTrans,		 	PackTriSmoothTrans,		    10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlatTexGTrans, 			PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 },
	{ProcessSmoothTexGTrans, 		PackTriSmoothTex,			10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighGTrans, 			PackTriHighTrans, 			10,	FLAGS(4, 0), 0, 0 },
	{ProcessSmoothGTrans, 	   	    PackTriSmoothTrans, 		10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighTexGTrans, 			PackTriHighTex,				10,	FLAGS(6, 0), 0, 0 },
	{ProcessSmoothTexGTransHighlight,PackTriSmoothTex,			10,	FLAGS(7, NOHIGH), 0, 0 },
	#undef BASE_FLAGS
};

IFUNCBLOCK NoVolVFogFuncs[32] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{ProcessFlatVFog, 				NULL, 			        2,	FLAGS(0, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothVFog,				PackTriSmooth, 			4,	FLAGS(1, 0), 8, FLAGS(2, 0) }, 
	{ProcessFlatTexVFog, 			PackTriFlatTex, 		8,	FLAGS(2, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothTexVFog, 			PackTriSmoothTex,		10,	FLAGS(3, 0), 8, FLAGS(2, 0) }, 
	{ProcessFlatHighlightVFog, 		PackTriHigh, 			4,	FLAGS(4, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothVFogHighlight,	PackTriSmooth, 			4,	FLAGS(5, NOHIGH), 8, FLAGS(2, 0) },
	{ProcessFlatTexHighlightVFog, 	PackTriHighTex, 		10,	FLAGS(6, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothTexVFogHighlight,	PackTriSmoothTex,		10,	FLAGS(7, NOHIGH), 8, FLAGS(2, 0) },

	/*
	// Global alpha
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE
	{ProcessFlatVFog, 				PackTriFlatTrans,		8,	FLAGS(0, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothVFog,      		PackTriSmoothTrans,		10,	FLAGS(1, 0), 8, FLAGS(2, 0) },
	{ProcessFlatTexVFog, 			PackTriFlatTex, 		8,	FLAGS(2, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothTexVFog, 			PackTriSmoothTex, 		10,	FLAGS(3, 0), 8, FLAGS(2, 0) },
	{ProcessFlatHighlightVFog, 		PackTriHighTrans, 		10,	FLAGS(4, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothVFogHighlight,   	PackTriSmoothTrans,		10,	FLAGS(5, NOHIGH), 8, FLAGS(2, 0) },
	{ProcessFlatTexHighlightVFog, 	PackTriHighTex, 		10,	FLAGS(6, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothTexVFogHighlight,	PackTriSmoothTex, 		10,	FLAGS(7, NOHIGH), 8, FLAGS(2, 0) },

	/*
	// Vertex alpha
	*/
	{ProcessFlatTransVFog, 			PackTriFlatTrans,		8,	FLAGS(0, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothTransVFog,		PackTriSmoothTrans,		10,	FLAGS(1, 0), 8, FLAGS(2, 0) },
	{ProcessFlatTexTransVFog, 		PackTriFlatTex,			8,	FLAGS(2, 0), 8, FLAGS(2, 0) },
	{ProcessSmoothTexTransVFog, 	PackTriSmoothTex,		10,	FLAGS(3, 0), 8, FLAGS(2, 0) },
	{ProcessHighTransVFog, 			PackTriHighTrans, 		10,	FLAGS(4, 0), 8, FLAGS(2, 0) },
	{ProcessSmoothTransVFogHighlight,PackTriSmoothTrans, 	10,	FLAGS(5, NOHIGH), 8, FLAGS(2, 0) },
	{ProcessHighTexTransVFog, 		PackTriHighTex,			10,	FLAGS(6, 0), 8, FLAGS(2, 0) },
	{ProcessSmoothTexTransVFogHighlight,PackTriSmoothTex,		10,	FLAGS(7, NOHIGH), 8, FLAGS(2, 0) },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlatGTransVFog, 		PackTriFlatTrans,		8,	FLAGS(0, 0), 8, FLAGS(2, 0) }, 
	{ProcessSmoothGTransVFog,		PackTriSmoothTrans,		10,	FLAGS(1, 0), 8, FLAGS(2, 0) },
	{ProcessFlatTexGTransVFog, 		PackTriFlatTex,			8,	FLAGS(2, 0), 8, FLAGS(2, 0) },
	{ProcessSmoothTexGTransVFog, 	PackTriSmoothTex,		10,	FLAGS(3, 0), 8, FLAGS(2, 0) },
	{ProcessHighGTransVFog, 		PackTriHighTrans, 		10,	FLAGS(4, 0), 8, FLAGS(2, 0) },
	{ProcessSmoothGTransVFogHighlight,PackTriSmoothTrans, 	10,	FLAGS(5, NOHIGH), 8, FLAGS(2, 0) },
	{ProcessHighTexGTransVFog, 		PackTriHighTex,			10,	FLAGS(6, 0), 8, FLAGS(2, 0) },
	{ProcessSmoothTexGTransVFogHighlight,PackTriSmoothTex,		10,	FLAGS(7, NOHIGH), 8, FLAGS(2, 0) },
	#undef BASE_FLAGS
};


/*
// Native smooth shading
*/

IFUNCBLOCK NoVolNativeFuncs[32] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{NULL, 							NULL, 						2,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmooth, 			PackTriNativeSmooth, 		4,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlatTex, 				PackTriFlatTex, 			8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessNativeSmoothTex, 		PackTriNativeSmoothTex, 	10,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlatHighlight, 			PackTriHigh, 				4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessNativeSmooth, 			PackTriNativeSmooth, 		4,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlatTexHighlight, 		PackTriHighTex, 			10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessNativeSmoothTex,      	PackTriNativeSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global alpha
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE
	{NULL, 							PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmooth,      		PackTriNativeSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlatTex, 				PackTriFlatTex, 			8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessNativeSmoothTex, 		PackTriNativeSmoothTex, 	10,	FLAGS(3, 0), 0, 0 },
	{ProcessFlatHighlight, 			PackTriHighTrans, 			10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessNativeSmooth,         	PackTriNativeSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlatTexHighlight, 		PackTriHighTex, 			10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessNativeSmoothTex,      	PackTriNativeSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Vertex alpha
	*/
	{ProcessFlatTrans, 				PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmoothTrans,		PackTriNativeSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlatTexTrans, 			PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 },
	{ProcessNativeSmoothTexTrans, 	PackTriNativeSmoothTex,		10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighTrans, 				PackTriHighTrans,			10,	FLAGS(4, 0), 0, 0 },
	{ProcessNativeSmoothTrans,  	PackTriNativeSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighTexTrans, 			PackTriHighTex,				10,	FLAGS(6, 0), 0, 0 },
	{ProcessNativeSmoothTexTrans, 	PackTriNativeSmoothTex,		10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlatGTrans, 			PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmoothGTrans,		PackTriNativeSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlatTexGTrans, 			PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 },
	{ProcessNativeSmoothTexGTrans, 	PackTriNativeSmoothTex,		10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighGTrans, 			PackTriHighTrans, 			10,	FLAGS(4, 0), 0, 0 },
	{ProcessNativeSmoothGTrans, 	PackTriNativeSmoothTrans, 	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighTexGTrans, 			PackTriHighTex,				10,	FLAGS(6, 0), 0, 0 },
	{ProcessNativeSmoothTexGTrans, 	PackTriNativeSmoothTex,		10,	FLAGS(7, NOHIGH), 0, 0 },
	#undef BASE_FLAGS
};


/*
// SHADOWS
*/

IFUNCBLOCK ShadowFuncs[32] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency, shadows or lightvols */

	{ProcessFlatShadOrLiVol,			PackTriFlatShad,		2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol, 			PackTriSmoothShad, 		6,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlatTexShadOrLiVol, 		PackTriFlatTexShad,		8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,		PackTriSmoothTexShad,	12,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlatHighlightShadOrLiVol, 	PackTriHighShad, 		4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol, 			PackTriSmoothShad, 		6,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlatTexHighlightShadOrLiVol,PackTriHighTexShad, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,		PackTriSmoothTexShad,	12,	FLAGS(7, NOHIGH), 0, 0 },

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE

	/* global alpha, shadows,
	   no smooth shading */

	{ProcessFlatShadOrLiVol,			PackTriFlatTransShad,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenShadOrLiVol,			PackTriFlatTransShad,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlatTexShadOrLiVol, 		PackTriFlatTexShad,		8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessFlattenTexShadOrLiVol, 		PackTriFlatTexShad,		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessFlatHighlightShadOrLiVol, 	PackTriHighTransShad, 	10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighTransShad, 	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessFlatTexHighlightShadOrLiVol,PackTriHighTexShad, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessFlattenTexHighlightShadOrLiVol,PackTriHighTexShad, 	10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex alpha, shadows,
	   no smooth shading */

	{ProcessFlatTransShadOrLiVol, 	PackTriFlatTransShad,		8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenTransShadOrLiVol, 	PackTriFlatTransShad,		8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlatTexTransShadOrLiVol,PackTriFlatTexShad,			8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexTransShadOrLiVol,PackTriFlatTexShad,			8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighTransShadOrLiVol, 	PackTriHighTransShad,		10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnTransShadOrLiVol, 	PackTriHighTransShad,		10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighTexTransShadOrLiVol,PackTriHighTexShad,			10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexTransShadOrLiVol,PackTriHighTexShad,			10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex + global alpha, no shadows, no lightvols
	   no smooth shading */

	{ProcessFlatGTransShadOrLiVol, 	    PackTriFlatTransShad,		8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenGTransShadOrLiVol, 	    PackTriFlatTransShad,		8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlatTexGTransShadOrLiVol, 	PackTriFlatTexShad,			8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexGTransShadOrLiVol, 	PackTriFlatTexShad,			8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighGTransShadOrLiVol, 	    PackTriHighTransShad,		10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnGTransShadOrLiVol, 	    PackTriHighTransShad,		10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighTexGTransShadOrLiVol,	PackTriHighTexShad,			10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexGTransShadOrLiVol,	PackTriHighTexShad,			10,	FLAGS(7, NOSMOOTH), 0, 0 },

	#undef BASE_FLAGS
};


/*
// LIGHT VOLUMES
*/

IFUNCBLOCK LightVolFuncs[32] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency, no shadows , but lightvols */

	{ProcessFlatShadOrLiVol,			PackTriFlatLiVol,		2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol, 			PackTriSmoothLiVol,		6,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlatTexShadOrLiVol, 		PackTriFlatTexLiVol,	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,		PackTriSmoothTexLiVol,	12,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlatHighlightShadOrLiVol, 	PackTriHighLiVol, 		4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol, 			PackTriSmoothLiVol,   	6,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlatTexHighlightShadOrLiVol,PackTriHighTexLiVol, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,		PackTriSmoothTexLiVol,	12,	FLAGS(7, NOHIGH), 0, 0 },

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE

	/* global alpha, no shadows, lightvols
	   no smooth shading */

	{ProcessFlatShadOrLiVol,			PackTriFlatTransLiVol,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenShadOrLiVol,			PackTriFlatTransLiVol,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlatTexShadOrLiVol, 		PackTriFlatTexLiVol,	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessFlattenTexShadOrLiVol, 		PackTriFlatTexLiVol,	8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessFlatHighlightShadOrLiVol, 	PackTriHighTransLiVol, 	10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighTransLiVol, 	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessFlatTexHighlightShadOrLiVol,PackTriHighTexLiVol, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessFlattenTexHighlightShadOrLiVol,PackTriHighTexLiVol, 	10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex alpha, no shadows, lightvols
	   no smooth shading */

	{ProcessFlatTransShadOrLiVol, 	PackTriFlatTransLiVol,		8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenTransShadOrLiVol, 	PackTriFlatTransLiVol,		8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlatTexTransShadOrLiVol,PackTriFlatTexLiVol, 		8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexTransShadOrLiVol,PackTriFlatTexLiVol, 		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighTransShadOrLiVol, 	PackTriHighTransLiVol,		10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnTransShadOrLiVol, 	PackTriHighTransLiVol,		10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighTexTransShadOrLiVol,PackTriHighTexLiVol, 		10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexTransShadOrLiVol,PackTriHighTexLiVol, 		10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex + global alpha, no shadows, lightvols
	   no smooth shading */

	{ProcessFlatGTransShadOrLiVol, 	    PackTriFlatTransLiVol,		8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenGTransShadOrLiVol, 	    PackTriFlatTransLiVol,		8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlatTexGTransShadOrLiVol, 	PackTriFlatTexLiVol, 		8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexGTransShadOrLiVol, 	PackTriFlatTexLiVol, 		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighGTransShadOrLiVol, 	    PackTriHighTransLiVol,		10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnGTransShadOrLiVol, 	    PackTriHighTransLiVol,		10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighTexGTransShadOrLiVol, 	PackTriHighTexLiVol, 		10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexGTransShadOrLiVol,   PackTriHighTexLiVol, 		10,	FLAGS(7, NOSMOOTH), 0, 0 },

	#undef BASE_FLAGS
};


/*
// ==============
// AVERAGE COLOUR
// ==============
*/

/*
// NO SHADOWS OR LIGHT VOLUMES
*/

/*
// Traditional/D3D smooth shading
*/

IFUNCBLOCK NoVolAverageFuncs[32] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{ProcessFlatten,			    NULL,				2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmooth,					PackTriSmooth, 		4,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlattenTex, 			PackTriFlatTex, 	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTex, 				PackTriSmoothTex, 	10,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlattenHighlight, 		PackTriHigh, 		4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothHighlight,	    PackTriSmooth, 		4,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlight,	PackTriHighTex, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexHighlight,		PackTriSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global alpha
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE
	{ProcessFlatten,				PackTriFlatTrans,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmooth,      			PackTriSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTex, 			PackTriFlatTex, 	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTex, 			    PackTriSmoothTex, 	10,	FLAGS(3, 0), 0, 0 },
	{ProcessFlattenHighlight, 		PackTriHighTrans, 	10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothHighlight,   		PackTriSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlight,	PackTriHighTex, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexHighlight,	    PackTriSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Vertex alpha
	*/
	{ProcessFlattenTrans, 			PackTriFlatTrans,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothTrans,			PackTriSmoothTrans, 10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexTrans,		PackTriFlatTex,		8,	FLAGS(2, 0), 0, 0 },
	{ProcessSmoothTexTrans,			PackTriSmoothTex,	10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighFltnTrans,			PackTriHighTrans, 	10,	FLAGS(4, 0), 0, 0 },
	{ProcessSmoothTransHighlight,	PackTriSmoothTrans, 10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighFltnTexTrans,		PackTriHighTex,		10,	FLAGS(6, 0), 0, 0 },
	{ProcessSmoothTexTransHighlight,PackTriSmoothTex,	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlattenGTrans, 			PackTriFlatTrans,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothGTrans,		 	PackTriSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexGTrans, 		PackTriFlatTex,		8,	FLAGS(2, 0), 0, 0 },
	{ProcessSmoothTexGTrans, 		PackTriSmoothTex,	10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighFltnGTrans, 		PackTriHighTrans, 	10,	FLAGS(4, 0), 0, 0 },
	{ProcessSmoothGTransHighlight,  PackTriSmoothTrans, 10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighFltnTexGTrans, 		PackTriHighTex,		10,	FLAGS(6, 0), 0, 0 },
	{ProcessSmoothTexGTransHighlight,PackTriSmoothTex,	10,	FLAGS(7, NOHIGH), 0, 0 },
	#undef BASE_FLAGS
};

IFUNCBLOCK NoVolAverageVFogFuncs[32] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{ProcessFlattenVFog,			    NULL,				2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothVFog,					PackTriSmooth, 		4,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlattenTexVFog, 			PackTriFlatTex, 	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTexVFog, 				PackTriSmoothTex, 	10,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlattenHighlightVFog, 		PackTriHigh, 		4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothVFogHighlight,	    PackTriSmooth, 		4,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlightVFog,	PackTriHighTex, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexVFogHighlight,		PackTriSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global alpha
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE
	{ProcessFlattenVFog,				PackTriFlatTrans,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothVFog,      			PackTriSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexVFog, 			PackTriFlatTex, 	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTexVFog, 			    PackTriSmoothTex, 	10,	FLAGS(3, 0), 0, 0 },
	{ProcessFlattenHighlightVFog, 		PackTriHighTrans, 	10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothVFogHighlight,   		PackTriSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlightVFog,	PackTriHighTex, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexVFogHighlight,	    PackTriSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Vertex alpha
	*/
	{ProcessFlattenTransVFog, 			PackTriFlatTrans,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothTransVFog,			PackTriSmoothTrans, 10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexTransVFog,		PackTriFlatTex,		8,	FLAGS(2, 0), 0, 0 },
	{ProcessSmoothTexTransVFog,			PackTriSmoothTex,	10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighFltnTransVFog,			PackTriHighTrans, 	10,	FLAGS(4, 0), 0, 0 },
	{ProcessSmoothTransVFogHighlight,	PackTriSmoothTrans, 10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighFltnTexTransVFog,		PackTriHighTex,		10,	FLAGS(6, 0), 0, 0 },
	{ProcessSmoothTexTransVFogHighlight,PackTriSmoothTex,	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlattenGTransVFog, 			PackTriFlatTrans,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothGTransVFog,		 	PackTriSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexGTransVFog, 		PackTriFlatTex,		8,	FLAGS(2, 0), 0, 0 },
	{ProcessSmoothTexGTransVFog, 		PackTriSmoothTex,	10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighFltnGTransVFog, 		PackTriHighTrans, 	10,	FLAGS(4, 0), 0, 0 },
	{ProcessSmoothGTransVFogHighlight,  PackTriSmoothTrans, 10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighFltnTexGTransVFog, 		PackTriHighTex,		10,	FLAGS(6, 0), 0, 0 },
	{ProcessSmoothTexGTransVFogHighlight,PackTriSmoothTex,	10,	FLAGS(7, NOHIGH), 0, 0 },
	#undef BASE_FLAGS
};


/*
// Native smooth shading
*/

IFUNCBLOCK NoVolAverageNativeFuncs[32] =
{
	/*
	// No translucency
	*/
	#define BASE_FLAGS  0
	{ProcessFlatten,			    NULL,					2,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmooth,			PackTriNativeSmooth, 	4,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlattenTex, 			PackTriFlatTex,			8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessNativeSmoothTex, 		PackTriNativeSmoothTex, 10,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlattenHighlight, 		PackTriHigh, 			4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessNativeSmooth, 			PackTriNativeSmooth, 	4,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlight,	PackTriHighTex,			10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessNativeSmoothTex,      	PackTriNativeSmoothTex, 10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global alpha
	*/
	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_TRANS | MASK_TEXTURE
	{ProcessFlatten,				PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmooth,      		PackTriNativeSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTex, 			PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessNativeSmoothTex, 		PackTriNativeSmoothTex, 	10,	FLAGS(3, 0), 0, 0 },
	{ProcessFlattenHighlight, 		PackTriHighTrans,			10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessNativeSmooth,         	PackTriNativeSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlight,	PackTriHighTex,				10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessNativeSmoothTex,      	PackTriNativeSmoothTex, 	10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Vertex alpha
	*/
	{ProcessFlattenTrans, 			PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmoothTrans,		PackTriNativeSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexTrans,		PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 },
	{ProcessNativeSmoothTexTrans,	PackTriNativeSmoothTex,		10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighFltnTrans,			PackTriHighTrans,			10,	FLAGS(4, 0), 0, 0 },
	{ProcessNativeSmoothTrans,		PackTriNativeSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighFltnTexTrans,		PackTriHighTex,				10,	FLAGS(6, 0), 0, 0 },
	{ProcessNativeSmoothTexTrans,	PackTriNativeSmoothTex,		10,	FLAGS(7, NOHIGH), 0, 0 },

	/*
	// Global and vertex alpha
	*/
	{ProcessFlattenGTrans, 			PackTriFlatTrans,			8,	FLAGS(0, 0), 0, 0 },
	{ProcessNativeSmoothGTrans,		PackTriNativeSmoothTrans,	10,	FLAGS(1, 0), 0, 0 },
	{ProcessFlattenTexGTrans, 		PackTriFlatTex,				8,	FLAGS(2, 0), 0, 0 },
	{ProcessNativeSmoothTexGTrans, 	PackTriNativeSmoothTex,		10,	FLAGS(3, 0), 0, 0 },
	{ProcessHighFltnGTrans, 		PackTriHighTrans,			10,	FLAGS(4, 0), 0, 0 },
	{ProcessNativeSmoothGTrans, 	PackTriNativeSmoothTrans,	10,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessHighFltnTexGTrans, 		PackTriHighTex,				10,	FLAGS(6, 0), 0, 0 },
	{ProcessNativeSmoothTexGTrans, 	PackTriNativeSmoothTex,		10,	FLAGS(7, NOHIGH), 0, 0 },
	#undef BASE_FLAGS
};


/*
// SHADOWS
*/

IFUNCBLOCK ShadowAverageFuncs[32] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency, shadows or lightvols */

	{ProcessFlattenShadOrLiVol,				PackTriFlatShad,		2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol,				PackTriSmoothShad, 		6,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlattenTexShadOrLiVol,			PackTriFlatTexShad,		8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,			PackTriSmoothTexShad,	12,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighShad, 		4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol,				PackTriSmoothShad, 		6,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlightShadOrLiVol,	PackTriHighTexShad, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,			PackTriSmoothTexShad,	12,	FLAGS(7, NOHIGH), 0, 0 },

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE

	/* global alpha, shadows,
	   no smooth shading */

	{ProcessFlattenShadOrLiVol,				PackTriFlatTransShad,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenShadOrLiVol,				PackTriFlatTransShad,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexShadOrLiVol,			PackTriFlatTexShad,		8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessFlattenTexShadOrLiVol,			PackTriFlatTexShad,		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighTransShad, 	10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighTransShad, 	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexHighlightShadOrLiVol,	PackTriHighTexShad, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessFlattenTexHighlightShadOrLiVol,	PackTriHighTexShad, 	10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex alpha, shadows,
	   no smooth shading */

	{ProcessFlattenTransShadOrLiVol, 		PackTriFlatTransShad,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenTransShadOrLiVol,		PackTriFlatTransShad,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexTransShadOrLiVol,		PackTriFlatTexShad,		8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexTransShadOrLiVol,		PackTriFlatTexShad,		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighFltnTransShadOrLiVol,		PackTriHighTransShad,	10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnTransShadOrLiVol,		PackTriHighTransShad,	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighFltnTexTransShadOrLiVol,	PackTriHighTexShad,		10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexTransShadOrLiVol,	PackTriHighTexShad,		10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex + global alpha, no shadows, no lightvols
	   no smooth shading */

	{ProcessFlattenGTransShadOrLiVol, 	    PackTriFlatTransShad,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenGTransShadOrLiVol, 	    PackTriFlatTransShad,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexGTransShadOrLiVol, 	PackTriFlatTexShad,		8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexGTransShadOrLiVol, 	PackTriFlatTexShad,		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighFltnGTransShadOrLiVol, 	    PackTriHighTransShad,	10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnGTransShadOrLiVol, 	    PackTriHighTransShad,	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighFltnTexGTransShadOrLiVol,	PackTriHighTexShad,		10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexGTransShadOrLiVol,	PackTriHighTexShad,		10,	FLAGS(7, NOSMOOTH), 0, 0 },

	#undef BASE_FLAGS
};


/*
// LIGHT VOLUMES
*/

IFUNCBLOCK LightVolAverageFuncs[32] =
{
	#define BASE_FLAGS	MASK_SHADOW_FLAG

	/* no translucency, no shadows , but lightvols */

	{ProcessFlattenShadOrLiVol,				PackTriFlatLiVol,		2,	FLAGS(0, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol,				PackTriSmoothLiVol,		6,	FLAGS(1, 0), 0, 0 }, 
	{ProcessFlattenTexShadOrLiVol,			PackTriFlatTexLiVol,	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,			PackTriSmoothTexLiVol,	12,	FLAGS(3, 0), 0, 0 }, 
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighLiVol, 		4,	FLAGS(4, 0), 0, 0 }, 
	{ProcessSmoothShadOrLiVol,				PackTriSmoothLiVol,   	6,	FLAGS(5, NOHIGH), 0, 0 },
	{ProcessFlattenTexHighlightShadOrLiVol,	PackTriHighTexLiVol, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessSmoothTexShadOrLiVol,			PackTriSmoothTexLiVol,	12,	FLAGS(7, NOHIGH), 0, 0 },

	#undef BASE_FLAGS
	#define BASE_FLAGS	MASK_SHADOW_FLAG | MASK_TRANS | MASK_TEXTURE

	/* global alpha, no shadows, lightvols
	   no smooth shading */

	{ProcessFlattenShadOrLiVol,				PackTriFlatTransLiVol,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenShadOrLiVol,				PackTriFlatTransLiVol,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexShadOrLiVol,			PackTriFlatTexLiVol,	8,	FLAGS(2, 0), 0, 0 }, 
	{ProcessFlattenTexShadOrLiVol,			PackTriFlatTexLiVol,	8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighTransLiVol, 	10,	FLAGS(4, 0), 0, 0 }, 
	{ProcessFlattenHighlightShadOrLiVol, 	PackTriHighTransLiVol, 	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexHighlightShadOrLiVol,	PackTriHighTexLiVol, 	10,	FLAGS(6, 0), 0, 0 }, 
	{ProcessFlattenTexHighlightShadOrLiVol,	PackTriHighTexLiVol, 	10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex alpha, no shadows, lightvols
	   no smooth shading */

	{ProcessFlattenTransShadOrLiVol, 		PackTriFlatTransLiVol,	8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenTransShadOrLiVol,		PackTriFlatTransLiVol,	8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexTransShadOrLiVol,		PackTriFlatTexLiVol, 	8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexTransShadOrLiVol,		PackTriFlatTexLiVol, 	8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighFltnTransShadOrLiVol,		PackTriHighTransLiVol,	10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnTransShadOrLiVol,		PackTriHighTransLiVol,	10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighFltnTexTransShadOrLiVol,	PackTriHighTexLiVol, 	10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexTransShadOrLiVol,	PackTriHighTexLiVol, 	10,	FLAGS(7, NOSMOOTH), 0, 0 },

	/* vertex + global alpha, no shadows, lightvols
	   no smooth shading */

	{ProcessFlattenGTransShadOrLiVol, 	    PackTriFlatTransLiVol,		8,	FLAGS(0, 0), 0, 0 }, 
	{ProcessFlattenGTransShadOrLiVol, 	    PackTriFlatTransLiVol,		8,	FLAGS(1, NOSMOOTH), 0, 0 },
	{ProcessFlattenTexGTransShadOrLiVol, 	PackTriFlatTexLiVol, 		8,	FLAGS(2, 0), 0, 0 },
	{ProcessFlattenTexGTransShadOrLiVol, 	PackTriFlatTexLiVol, 		8,	FLAGS(3, NOSMOOTH), 0, 0 },
	{ProcessHighFltnGTransShadOrLiVol, 	    PackTriHighTransLiVol,		10,	FLAGS(4, 0), 0, 0 },
	{ProcessHighFltnGTransShadOrLiVol, 	    PackTriHighTransLiVol,		10,	FLAGS(5, NOSMOOTH), 0, 0 },
	{ProcessHighFltnTexGTransShadOrLiVol, 	PackTriHighTexLiVol, 		10,	FLAGS(6, 0), 0, 0 },
	{ProcessHighFltnTexGTransShadOrLiVol,   PackTriHighTexLiVol, 		10,	FLAGS(7, NOSMOOTH), 0, 0 },

	#undef BASE_FLAGS
};
