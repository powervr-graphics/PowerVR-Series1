/******************************************************************************
 * Name         : dtri.h
 * Title        : Header for private interface to PowerSGL direct
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dtri.h,v $
 * Revision 1.47  1997/10/14  18:05:04  erf
 * Added defines to enable/disable assembler version of DrawPrim DAG sorting
 * routines.
 *
 * Revision 1.46  1997/10/13  08:05:30  erf
 * Enabled assembler versions of draw primitive triangle core routines.
 *
 * Revision 1.45  1997/09/09  21:30:50  mjg
 * Toggled the assembler mentioned below, on.
 *
 * Revision 1.44  1997/09/09  16:41:46  mjg
 * Added define for D3D trans. tri. process routine (currently off).
 *
 * Revision 1.43  1997/08/22  10:38:12  sks
 * Added nSmoothSpecularColour to the material structure.
 *
 * Revision 1.42  1997/08/21  16:05:02  erf
 * Added defines to enable\disable assembler version of the processing
 * code.
 *
 * Revision 1.41  1997/08/21  11:28:17  erf
 * Added defines to enable\disable assembler processing code.
 *
 * Revision 1.40  1997/08/18  16:26:03  gdc
 * added stuff for vertex fog smooth highlights
 *
 * Revision 1.39  1997/08/13  16:18:39  gdc
 * added new field to material struct for per vertex fogging
 *
 * Revision 1.38  1997/07/14  19:28:56  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.37  1997/06/22  17:14:41  erf
 * New assembler routines for processing non-perspective textures.
 *
 * Revision 1.36  1997/06/03  13:59:57  erf
 * Added defines for the PackISPClipTested routines added to disp.c .
 *
 * Revision 1.35  1997/05/13  17:49:15  erf
 * 1. Added C defines for new TSP packing functions.
 * 2. Added defines to use the assembler versions of PackTSPTex and
 *    PackTSPMipMapTex.
 *
 * Revision 1.34  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.33  1997/04/21  14:36:44  erf
 * Seperated ISPTSP, PCX1 and PCX2 assembler routine definitions.
 *
 * Revision 1.32  1997/04/18  16:28:04  erf
 * Moved the position of fHalfTexel within PROCESSDATACONTEXT so it is
 * accesable by the assembler.
 *
 * Revision 1.31  1997/04/04  13:03:24  jop
 * DOS builds now get Rob's asm code
 *
 * Revision 1.30  1997/03/31  22:06:54  jop
 * just shoved in a pragma message to remind us to fix up the DOS driver
 *
 * Revision 1.29  1997/03/21  17:51:10  erf
 * Created new variable fHalfTexel to fix texel offset.
 *
 * Revision 1.28  1997/03/05  12:22:57  gdc
 * added a TexSize entry to the PDC struct
 *
 * Revision 1.27  1997/03/04  20:46:57  erf
 * Defined MODIFYXY as 1. Used to enable/disable pixel offset fix.
 *
 * Revision 1.26  1997/02/03  16:14:49  rgi
 * Turned D3DTriangle assembler optimisation back on for PCX1
 * and PCX2. Added support for assemblerisation of D3D Strip
 * handling code, not yet complete.
 *
 * Revision 1.25  1997/01/27  13:48:48  rgi
 * Moved the invTexSize field now used in dtriml.asm into
 * the fixed assembler known section of gPDC. Matching updates
 * to all win32\*.asm files.
 *
 * Revision 1.24  1997/01/27  12:30:22  erf
 * Enable assembler texturing for PCX2.
 *
 * Revision 1.23  1997/01/24  18:04:23  gdc
 * 1. Disabled TEX ASM stuff as I've re-worked the interface
 * 2. Added another facet into the Shading data union to allow above to work
 * When texturing enabled flat/high/shad/LV/smooth packing writes back into
 * this struct instead of TSP direct, then TEXFN accesses this and writes
 * sequentially
 *
 * Revision 1.22  1997/01/24  09:29:31  rgi
 * #Turned PROCESSTRICORELITE back on as this has the clipping
 * bug fixed now. Temporarily disabled assembler for
 * PROCESSD3DTRICORE as this does not work with the new structure.
 *
 * Revision 1.21  1997/01/20  21:25:54  jrc
 * Switched off assembly code for PROCESSTRICORELITE because of a clipping
 * bug that comes up in Ultimate Race.
 * Switched off assembly code for PACKTSPTEX because of a accuracy bug in D3D
 * apps.
 *
 * Revision 1.20  1997/01/18  01:01:41  jrc
 * First go at native shading API (not finished).
 *
 * Revision 1.19  1997/01/16  13:07:47  rgi
 * Added support for assembler version of ProcessTriCoreLite
 * in dtri.c
 *
 * Revision 1.18  1997/01/15  13:21:40  rgi
 * Moved the extra fields now known to the assembler into the
 * assembler region of gPDC context structure.
 *
 * Revision 1.17  1997/01/09  17:10:22  rgi
 * Changed to export gpMatCurrent to dtex.c ProcessFlatTex
 * routines. TexMatFn removed as the decision is now done
 * by us not dtex.c
 *
 * Revision 1.16  1997/01/07  19:41:04  sjf
 * Added "u32GlobalTransBy85div128;" to structure, which is used
 * in the optimised translucency packing/averaging code.
 *
 * Revision 1.15  1997/01/06  13:05:36  erf
 * Use assembler versions of Polygon core for PCX2.
 *
 * Revision 1.14  1996/12/12  08:04:54  jop
 * Fixed defines so they definitely default to using the C versions
 * of the Core code
 *
 * Revision 1.13  1996/12/10  23:46:00  erf
 * Changed PVR2 to PCX1.
 *
 * Revision 1.12  1996/12/10  20:06:20  jrc
 * Enabled C code version of ProcessD3DStripClippingCore for full
 * implementation of polygons.
 *
 * Revision 1.11  1996/12/09  18:01:19  gdc
 * renamed Direct* to sgltri_* and removed conditional on D3DOPTIM as
 * this is now the default
 *
 * Revision 1.10  1996/12/05  20:47:38  jop
 * Put conditional assembly flags in one place.
 *
 * Revision 1.9  1996/12/02  16:47:40  jop
 * Added qurad support
 *
 * Revision 1.8  1996/11/30  04:56:48  jop
 * Added IEDGE structure for poly code.
 *
 * Revision 1.7  1996/11/28  12:32:55  rgi
 * Made gPDC a const structure outside the parts of dtri.c
 * that actually set it up. This allows the compiler to assume
 * that fields in gPDC do not change every time a pointer to
 * something else is assigned to. The alternative was to change
 * all the code written so far to load gPDC values to locals
 * before use. This was not considered to be ideal.
 *
 * Revision 1.6  1996/11/23  04:16:04  jop
 * Made some structure changes
 *
 * Revision 1.5  1996/11/23  02:34:49  jop
 * Added some structs and published some globals (boo hiss)
 *
 * Revision 1.4  1996/11/22  02:27:33  jrc
 * Removed IVERTEX structure for the time being.
 *
 * Revision 1.3  1996/11/21  16:22:27  jrc
 * Added NumExtraVertices member to ITRI.  Will be moved elsewhere for
 * performance reasons later.
 *
 * Revision 1.2  1996/11/19  00:58:25  jop
 * Cleaned up unnecessary garbage
 *
 * Revision 1.1  1996/11/18  18:30:12  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __DTRI_H__
#define __DTRI_H__


/* used in disp.c and dtri.c */
#define IBUFFERSIZE	 16
#define IEDGEBUFFERSIZE  64

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
			sgl_uint32	Col0;
			sgl_uint32	Col1;
			sgl_uint32	Col2;
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

/* TEXDATA structure known by assembler, do not change. RGI */
typedef struct tagTEXDATA
{
	float	u0;
	float	v0;
	float	w0;
	float	u1;
	float	v1;
	float	w1;
	float	u2;
	float	v2;
	float	w2;
	
} TEXDATA, *PTEXDATA;
/* TEXDATA structure known by assembler, do not change. RGI */

/* The size of this structure is hard coded into the assembler modules */
typedef struct tagITRI
{
	float	fAdjoint[3][3];
	float	f1OverDet;
	float	fZ[3];

	union	
	{
		sgl_int8	b[4];
		sgl_uint32	u;
	}		reg;
	
	sgl_uint32	TSPControlWord;
	sgl_uint32	BaseColour;

} ITRI, *PITRI;
/* The size of this structure is hard coded into the assembler modules */


/* The size of this structure is hard coded into the assembler modules */
typedef struct tagIMATERIAL
{
	TEXDATA		Tex;	/* TEXDATA known by assembler to be first struct,
														do not change. RGI */
	SHADINGDATA	Shading; /* SHADINGDATA will soon known by assembler to
							be second struct, do not change. */

	union
	{
		sgl_uint32	LightVolCol;
		float	ShadowBrightness;
	} v;

	sgl_uint32  NumExtraVertices;  /* 0 for triangle, 1 for quad, etc. */
	sgl_uint32  nFogIndex; /* look up index for per vertex fog */
	sgl_uint32  nSmoothSpecularIndex; /* look up index for smooth specular shading */
	sgl_uint32  nSmoothSpecularColour;
} IMATERIAL, *PIMATERIAL;
/* The size of this structure is hard coded into the assembler modules */

typedef struct tagIEDGE
{
	float	fA;
	float	fB;
	float	fC;	
} IEDGE, *PIEDGE;

typedef enum
{
	NoMul,
	Mul,
	MulWrapU,
	MulWrapV,
	MulWrapUV,
} TEXMATFN;

/* per polygon intermediate routines */
typedef void (* PPIR)(PITRI pTri);

/* per buffer pack routines */
typedef void (* PBPR)(sgl_uint32 *pTSP, int nPolys);

typedef int (* PROCESSCOREFN)(PPIR pAddfn);

typedef int (* PACKISPFN)(void *pData, void *pData2, int nTriangles,
						  int nIncrement, sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement);

typedef struct tagPROCESSDATACONTEXT
{
	int				nInputTriangles;   /* Known to assember do not re-order */
	PSGLVERTEX		pVertices; 
	sgl_uint32		TSPControlWord;

	void 			*pFace;
	PSGLVERTEX		pV0;
	PSGLVERTEX		pV1; 
	PSGLVERTEX		pV2;

	int				ShiftRegX;
	
	sgl_uint32		TexAddress;			/* New bits known to assember     */
	int				n32MipmapOffset;	/* in win32\d*ml.asm, must update */
	float 			invTexSize;			/* these simultaneously. RGI      */
	
	float           fHalfTexel;			/* used to implement half texel fix */

	SGLCONTEXT		Context;		   /* Known to assember do not re-order */

	/* Extend structure (or SGLCONTEXT) into this area do not change above */

    /*
	// When doing Per Vertex translucency (which we can't do with PCX1 etc) we
	// need (85 * the global value)/128, so may as well compute it just the once.
	// This won't be needed when we get PCX3-style hardware.
	*/
	sgl_uint32		u32GlobalTransBy85div128;

	float           TexSize;	/* used in d3d texture func			*/
	sgl_uint32	    VFogTSPControlWord[2]; /* TSP control word for per vertex fog */
	
} PROCESSDATACONTEXT;

extern const PROCESSDATACONTEXT	gPDC;
extern PIMATERIAL gpMatCurrent;

extern void sgltri_triangles ( PSGLCONTEXT  pContext,
						int  nNumFaces,
					    int  pFaces[][3],
						PSGLVERTEX  pVertices );

extern void sgltri_quads ( PSGLCONTEXT  pContext,
					int  nNumFaces,
				    int  pFaces[][4],
					PSGLVERTEX  pVertices );

/* Enable modification of X Y for D3D.
 */
#define MODIFYXY	1
					
/* 
	control which bits of asm are built
	
	when macros are set to 1, asm versions are built
	when macros are set to 0, C versions are used instead
	
*/

#define PACKISPCORE				1		/* 1 for C code version */
#define PACKISPPOLYGONCORE		1		/* 1 for C code version */
#define PROCESSD3DTRICORE  		1		/* 1 for C code version */
#define PROCESSD3DSTRIPCORE		1		/* 1 for C code version */
#define PROCESSTRICORELITE		1		/* 1 for C code version */
#define PACKTSPTEX				1		/* 1 for C code version */
#define PACKTSPMIPMAPTEX		1		/* 1 for C code version */
#define PACKTSPTEXNONPER		1		/* 1 for C code version */
#define PACKTSPMIPMAPTEXNONPER	1		/* 1 for C code version */
#define PACKISPCORECLIPTESTED	1		/* 1 for C code version */
#define PACKISPPOLYGONCORECLIPTESTED	1	/* 1 for C code version */
#define PROCESSD3DCOREPRIMITIVE			1	/* 1 for C code version */
#define PROCESSD3DFANCOREPRIMITIVE		1	/* 1 for C code version */
#define PROCESSD3DTRANSTRICORELITE		1	/* 1 for C code version */
#define PROCESSD3DTRANSCOREPRIMITIVE	1	/* 1 for C code version */
#define PROCESSD3DTRANSFANCOREPRIMITIVE	1	/* 1 for C code version */


#if WIN32 || DOS32

	/* dtri.c stuff
	 * Use assembler version of some routines for PCX1, PCX2 & ISPTSP.
	 */

	/* Midas3 (ISPTSP) assembler routines.
	 */
	#if ISPTSP

		#undef PROCESSTRICORELITE
		#define PROCESSTRICORELITE	0	/* Assembler version in dtriml.asm */
		
		#undef PROCESSD3DTRICORE
		#define PROCESSD3DTRICORE	0	/* Assembler version in dtriml.asm */

	#endif

	/* PCX1 assembler routines.
	 */
	#if PCX1
	
		#undef PACKISPCORE
		#define PACKISPCORE 0		/* Assembler version in dispml.asm */

		#undef PACKISPPOLYGONCORE
		#define PACKISPPOLYGONCORE 0	/* Asm   version in dispml.asm */

		#undef PROCESSTRICORELITE
		#define PROCESSTRICORELITE	0	/* Assembler version in dtriml.asm */
		
		#undef PROCESSD3DTRICORE
		#define PROCESSD3DTRICORE	0	/* Assembler version in dtriml.asm */

        #undef PROCESSD3DCOREPRIMITIVE
		#define PROCESSD3DCOREPRIMITIVE		0 /* Asm in d3dtriml.asm */

        #undef PROCESSD3DFANCOREPRIMITIVE
		#define PROCESSD3DFANCOREPRIMITIVE	0 /* Asm in d3dtriml.asm */

	#endif

	/* PCX2 assembler routines.
	 */
	#if PCX2 || PCX2_003				/* New v4.0 optimisations for PCX2 */

		#undef PACKTSPTEX
		#define PACKTSPTEX			0	/* Assembler version in dtexml.asm */
		#define PackTSPTex			_PackTSPTex

		#undef PACKTSPTEXNONPER
		#define PACKTSPTEXNONPER	0	/* Assembler version in dtexml.asm */
		#define PackTSPTexNonPer	_PackTSPTexNonPer

		#undef PACKTSPMIPMAPTEX
		#define PACKTSPMIPMAPTEX 	0	/* Assembler version in dtexml.asm */
		#define PackTSPMipMapTex	_PackTSPMipMapTex
		
		#undef PACKTSPMIPMAPTEXNONPER
		#define PACKTSPMIPMAPTEXNONPER 	0	/* Assembler version in dtexml.asm */
		#define PackTSPMipMapTexNonPer	_PackTSPMipMapTexNonPer
		
		#undef PROCESSD3DSTRIPCORE
		#define PROCESSD3DSTRIPCORE	1	/* Assembler version in dtriml.asm */

		#undef PROCESSTRICORELITE
		#define PROCESSTRICORELITE	0	/* Assembler version in dtriml.asm */
		
		#undef PROCESSD3DTRICORE
		#define PROCESSD3DTRICORE	0	/* Assembler version in dtriml.asm */

        #undef PROCESSD3DTRANSTRICORELITE
        #define PROCESSD3DTRANSTRICORELITE	0 /* Asm in d3dtriml.asm */

        #undef PROCESSD3DCOREPRIMITIVE
		#define PROCESSD3DCOREPRIMITIVE		0 /* Asm in d3dtriml.asm */

        #undef PROCESSD3DFANCOREPRIMITIVE
		#define PROCESSD3DFANCOREPRIMITIVE	0 /* Asm in d3dtriml.asm */

        #undef PROCESSD3DTRANSCOREPRIMITIVE
		#define PROCESSD3DTRANSCOREPRIMITIVE	0 /* Asm in d3dtriml.asm */

        #undef PROCESSD3DTRANSFANCOREPRIMITIVE
		#define PROCESSD3DTRANSFANCOREPRIMITIVE	0 /* Asm in d3dtriml.asm */
	#endif
#endif

#endif
   
