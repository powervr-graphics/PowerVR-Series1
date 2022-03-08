/******************************************************************************
 * Name         : d3dtri.c
 * Title        : Low level API top level code for PowerVR Direct D3D interface.
 * Author       : Edward Forde
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
 * $Log: d3dtri.c,v $
 * Revision 1.29  1998/07/16  16:21:37  ls
 * added a larger bogusinvz_increment for z biasing to work
 *
 * Revision 1.28  1998/01/26  18:14:35  erf
 * Added to code to set fMinInvZ and gfInvertZ depending
 * on whether Z mode <= or >= has been set.
 *
 * Revision 1.27  1997/12/08  13:19:17  erf
 * Fixed vertex fog with DrawPrimitive.
 *
 * Revision 1.26  1997/11/26  17:35:56  erf
 * 1. When SGLTT_DECAL selected then we always use flat shading. ie ignore
 *    smooth shading and highlight flags.
 * 2. Added extra plane stuff to DirectTriPrimitives() routine since it
 *    was left out.
 *
 * Revision 1.25  1997/11/07  15:57:40  gdc
 * added depth bias stuff - untested so will probably need tweaking
 *
 * Revision 1.24  1997/10/28  10:30:30  erf
 * Removed ExtendTSPBuffer() routine and changed to calls to
 * PVROSExtendTSPBuffer() routine.
 *
 * Revision 1.23  1997/10/23  10:13:15  erf
 * Implemented decal texturing. Passed in context flags (0x8).
 *
 * Revision 1.22  1997/10/20  11:34:20  erf
 * Fixed TSP allocation buffer stuff in ProcessTriPrimitives() which was
 * causing crashing on WinBench. Sorry ho ....
 *
 * Revision 1.21  1997/10/16  11:44:58  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.20  1997/10/15  13:31:08  erf
 * Fixed for PCX1. ie different number of DrawPrimitive function available
 * when DAG sorting unavailable. Based on define DAG_TRANS_SORTING.
 *
 * Revision 1.19  1997/10/15  13:28:00  sxy
 * added DPFDEVF messages.
 *
 * Revision 1.18  1997/10/14  18:05:36  erf
 * Added DAG sorting to DrawPrimitive control flow.
 *
 * Revision 1.17  1997/10/13  11:09:07  erf
 * Removed ProcessTris() routine. Now incorporated in ProcessD3DTriPrimitives().
 * ProcessD3DTransTri() will now be used for DAG sorting for both execute
 * buffers and draw primitive stuff.
 *
 * Revision 1.16  1997/09/24  14:24:47  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.15  1997/09/15  16:07:14  mjg
 * Removed DAG_TRANS_SORTING define
 *
 * Revision 1.14  1997/09/10  13:38:36  erf
 * 1. Removed process routines to d3dcore.h and dcore.h, or to the
 *    assembler files d3dtriml.asm and dtriml.asm .
 * 2. Removed dtri.c from project so that this is the only file.
 *
 * Revision 1.13  1997/09/09  14:08:10  erf
 * Changed a few parameters to extern as they are defined in dtri.c
 * and are required to be linked to the assembler functions.
 *
 * Revision 1.12  1997/09/08  12:29:11  mjg
 * Changed assembly define
 *
 * Revision 1.11  1997/09/05  13:03:40  sxy
 * Added a large Epsilon to cull more small triangles.
 *
 * Revision 1.10  1997/09/04  18:16:19  erf
 * 1. Removed redundant code from new translucent triangle sorting
 *    code at the process triangle stage.
 * 2. Bilinear filtering stuff for PCX2-003.
 *
 * Revision 1.9  1997/09/03  15:32:30  erf
 * 1. Changed DirectD3DPolyons to pass bFullSort parameter. D3D now
 *    always calls this routine instead of DirectPolygons. This
 *    chooses which sorting to use.
 *
 * Revision 1.8  1997/09/01  16:29:21  erf
 * Removed unused code from new translucent path. ie average colour stuff,
 * different texture processing etc.
 *
 * Revision 1.7  1997/09/01  15:38:38  erf
 * Added vertex fog and gouraud specular functionality to draw primitive.
 *
 * Revision 1.6  1997/09/01  12:16:51  mjg
 * Started a D3D specific version of DirectPolygons and added versions of
 * functions for D3D translucency sorting.
 *
 * Revision 1.5  1997/08/26  16:29:23  erf
 * Fixed flat shaded triangles with highlights. PCX1 abd PCX2 require the
 * same fix.
 *
 * Revision 1.4  1997/08/26  13:39:06  erf
 * Added fix for flat shaded triangles with specular highlights.
 * This assures that the start TSP address is properly aligned
 * for a bunch of triangles.
 *
 * Revision 1.3  1997/08/21  15:57:29  erf
 * 1. Replaced i in Primitive .
 * 2. Removed static from variables so that they can be used in
 *    the assembler.
 * 3. Moved the process function array to after the process routines.
 *
 * Revision 1.2  1997/08/21  11:23:32  erf
 * Removed defines to enable\disable triangle processing code to dtri.h .
 *
 * Revision 1.1  1997/08/20  16:45:35  erf
 * Initial revision
 *
 *
 *****************************************************************************/
#define MODULE_ID MODID_D3DTRI

#include "sgl_defs.h"
#include "sgl_init.h"
#include "pvrosapi.h"
#include "metrics.h"
#include "hwinterf.h"
#include "sgl_math.h"

#include "nm_intf.h"
#include "getnamtb.h"
#include "dlntypes.h"

#include "dtri.h"
#include "dshade.h"
#include "dtex.h"
#include "disp.h"
#include "dtsp.h"
#include "pmsabre.h"
#include "dregion.h"
#include "parmbuff.h"
#include "pvrlims.h"
#include "texapi.h"

#include "pvrd.h"

#if DAG_TRANS_SORTING
	#include "d3dtsort.h"
#endif /*DAG_TRANS_SORTING*/

SGL_EXTERN_TIME_REF /* if we are timing code */

#define BIGGER_BOGUSINVZ_INCREMENT	((1.0)/(0x8000000))

#if WIN32 || DOS32

	#define DO_FPU_PRECISION TRUE

	void __cdecl SetupFPU (void);
	void __cdecl RestoreFPU (void);

#else

	#define DO_FPU_PRECISION FALSE

#endif

static int TSPDoubleBuffer = 0; /* disable TSP double buffing till PVROS sorted */ 
#pragma message ("Another extern to sort out")
#pragma message ("TSP Double buffer hard coded to OFF")

/*
// Maximum number of vertices in a convex flat fan that can form a polyhedron
// sent to the hardware as a single object.  Direct3D supports 32 vertex flat
// fans, but the current code splits these up into polygons with smaller
// numbers of vertices.
*/
#define MAX_POLYGON_VERTICES  8

/* Used to enable decal texturing as selected by D3D. Defined here
 * as it is only applicable to D3D.
 */
#define	SGLTT_DECAL		0x00000008

/* This is a flag to be used only by D3D DrawPrimitive, not SGL Direct.
 * It uses one of the old flag positions no longer used by SGL Direct.
 * There must be an identical definition to this in the HAL's pvrd3dcb.c
 * source file.
 */
#define SGLTT_ZGREATEREQUAL 0x00000010

/*
// ============================================================================
// 								STATIC VARIABLES:
// ============================================================================
*/
#define IBUFFERSIZE	 16
#define IEDGEBUFFERSIZE  64

const PROCESSDATACONTEXT gPDC;
PROCESSDATACONTEXT *const gpPDC = (PROCESSDATACONTEXT*) &gPDC;

ITRI		gpTri[IBUFFERSIZE];
IEDGE		gpEdge[IEDGEBUFFERSIZE];
IMATERIAL 	gpMat[IBUFFERSIZE];
PIMATERIAL	gpMatCurrent = gpMat;

#if DAG_TRANS_SORTING
	ITRIPLUS	gpTriPlus[IBUFFERSIZE];
#else
	/* Need to create a dummy variable, so the assmbler file will link */
	sgl_uint32 gpTriPlus;
#endif /*DAG_TRANS_SORTING*/

float gfDepthBias;

extern HLDEVICE gHLogicalDev;

extern float fMinInvZ;
extern float gfBogusInvZ;
extern float gfInvertZ;
extern sgl_bool FogUsed;
extern DEVICE_REGION_INFO_STRUCT RegionInfo; /* In dregion.c */

/* Variable to fix the pixel offset problem. The value is added to
 * all vertices in D3D and SGL-Lite.
 */
extern float 	fAddToXY;

/* typedef for the pack TSP parameter functions.
 */
typedef	int (*PACKTSPFUNCTION) (PIMATERIAL, int, int, sgl_uint32, sgl_uint32 *, int);

PACKTSPFUNCTION PackTSPTexFn;

static PACKTSPFUNCTION PackTSPFunctions[4] = {	PackTSPTex,
												PackTSPMipMapTex,
									   			PackTSPTexNonPer,
								   				PackTSPMipMapTexNonPer
									   			};

/* typedef for the process TSP parameter functions.
 */
typedef	void (*PROCESSTSPFUNCTION)(PITRI);

PROCESSTSPFUNCTION ProcessFlatTexFn;

/* Initialise the 8 D3D pack functions. These are indexed as required.
 * The SGLDirect and D3D non-perspective correct flat textured cases are
 * the same.
 */
static PROCESSTSPFUNCTION ProcessD3DTSPFunctions[8] = {	ProcessD3DFlatTex,
														ProcessD3DFlatTexWrapU,
														ProcessD3DFlatTexWrapV,
														ProcessD3DFlatTexWrapUV,
														ProcessFlatTexNonPer,
														ProcessD3DFlatTexWrapUNonPer,
														ProcessD3DFlatTexWrapVNonPer,
														ProcessD3DFlatTexWrapUVNonPer
														};

/* Flag needed to indicate whether triangles strips being used. Needed
 * to swap vertices for every second triangle in the strip.
 */
sgl_bool bStrip;

/* Need to have a global face index.
 */
int	nFaces = 0;

/* typedef for the process core functions.
 */
typedef	int (*PROCESSFUNCTIONPRIM)( PPIR pAddfn, sgl_uint32 NewObject, sgl_int32 nNextTriInc);

#if PCX2_003
/* In order to insert textures into the correct pass, 
 * we have to examine the alphamask to identify the type.
 */
#include "tmalloc.h"
typedef struct
{
  PTEXTURESPEC pTextureSpec;
  MNODE_BLOCK MemBlock;

} TPRIVATEDATA;	
#endif /* PCX2_003 */

#define FLATTEX 0x00000000
#define SMOOTHTEX 0x00000001
#define HIGHTEX 0x00000002
#define SMOOTHSHADTEX 0x00000004

#include "dpack.h"
#include "dprocess.h"
#include "d3dcore.h"
#include "dfunc.h"

/*
//===================================================
*/

/* Select which draw primitive function to select.
 */
static PROCESSFUNCTIONPRIM	ProcessFunctionPrim;

#if DAG_TRANS_SORTING
static PROCESSFUNCTIONPRIM	ProcessCorePrimtives[4] = {	ProcessD3DCorePrimitive,
														ProcessD3DFanCorePrimitive,
														ProcessD3DTransCorePrimitive,
														ProcessD3DTransFanCorePrimitive
														};
#else
static PROCESSFUNCTIONPRIM	ProcessCorePrimtives[2] = {	ProcessD3DCorePrimitive,
														ProcessD3DFanCorePrimitive,
														};
#endif

/**********************************************************************
**
**
**
**
**********************************************************************/

static void ProcessD3DPolys (PVRHANDLE TexHeap, PPIR pPerPolyfn, PBPR pPerBuffn, sgl_uint32 TSPWords)
{
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpPDC->nInputTriangles != 0 )
	{
		int nBurst;
		
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */
		
		if (gpPDC->TSPControlWord & MASK_TRANS)
		{
			nBurst = ProcessD3DStripTransCore ( pPerPolyfn, NewObject );

			NewObject = FALSE;
		}
		else
		{
			nBurst = ProcessD3DStripCore ( pPerPolyfn );
		}
		
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
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) && 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 16);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
				(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
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
					gpPDC->nInputTriangles = 0;
					
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
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);

				if (TSPSpaceAvailable < (nBurst * TSPWords))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
				
					/*
					   This division should only take place if the buffer
				   	is nearly full
					*/
					TSPSpaceAvailable /= TSPWords;
					nBurst = TSPSpaceAvailable;
				}
			}
		}

		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculted for the planes STP pointer
			*/

			TSPInc = PackISPPolygonExtra(gpTri, gpMat, gpEdge, nBurst, 
									 TSPAddr >> 1, TSPWords >> 1);
			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
			/* I'm not sure if the TSP offset is correct */
			PackExtra(gpTri, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
	  		PackISPPolygon (gpTri, gpMat, gpEdge, nBurst, TSPAddr >> 1, TSPWords >> 1);
			/* all objects are of known TSP size */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;
		
			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
		}
		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

/**********************************************************************
**
**
**
**
**********************************************************************/

static void ProcessD3DTriPrimitives (	PVRHANDLE TexHeap,
										PPIR pPerPolyfn,
										PBPR pPerBuffn,
										sgl_uint32 TSPWords,
										sgl_int32 nNextTriInc)
{
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpPDC->nInputTriangles != 0 )
	{
		int nBurst;
		
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder       
		 */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */

		/* Call the Process function. One of four possible types
		 *
		 *		ProcessD3DCorePrimitive
		 *		ProcessD3DFanCorePrimitive
		 *		ProcessTriCoreLite
		 */
		nBurst = ProcessFunctionPrim (pPerPolyfn , NewObject, nNextTriInc);

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
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) && 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 16);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
				(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
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
					gpPDC->nInputTriangles = 0;
					
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
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * TSPWords))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= TSPWords;
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculted for the planes STP pointer
			*/

			TSPInc = PackISPTriExtra(gpTri, gpMat, nBurst, 
									 TSPAddr >> 1, TSPWords >> 1);
			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
			/* I'm not sure if the TSP offset is correct */
			PackExtra(gpTri, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
			PackISPTri(gpTri, nBurst, TSPAddr >> 1, TSPWords >> 1);
			/* all objects are of known TSP size */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
		}

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

/**********************************************************************
**
**
**
**
**********************************************************************/

#if DAG_TRANS_SORTING
static void ProcessD3DTransTris(PVRHANDLE TexHeap, 
								PPIR pPerPolyfn, 
								PBPR pPerBuffn, 
								sgl_uint32 TSPWords,
								sgl_int32 nNextTriInc)
{
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpPDC->nInputTriangles != 0 )
	{
		int nBurst;
		
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */


		/* Call the Process function. One of four possible types
		 *
		 *		ProcessD3DTransTriCoreLite
		 *		ProcessD3DTransCorePrimitive
		 *		ProcessD3DTransFanCorePrimitive
		 */
		nBurst = ProcessFunctionPrim (pPerPolyfn , NewObject, nNextTriInc);

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
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) && 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 16);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
				(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
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
					gpPDC->nInputTriangles = 0;
					
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
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * TSPWords))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= TSPWords;
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculted for the planes STP pointer
			*/

			TSPInc = PackISPD3DTriTransExtra(gpTri, gpTriPlus, gpMat, nBurst, 
											 TSPAddr >> 1, TSPWords >> 1);
  
			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
			/* I'm not sure if the TSP offset is correct */
			PackExtra(gpTri, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
			PackISPD3DTriTrans(gpTri, gpTriPlus, nBurst, TSPAddr >> 1, TSPWords >> 1);

			/* all objects are of known TSP size */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
		}

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

#endif /* DAG_TRANS_SORTING */

/*
// ============================================================================
// 							  SGL EXTERNAL ROUTINES:
// ============================================================================
*/

void DirectTriPrimitive(PVRHANDLE 		TexHeap,
						PSGLCONTEXT  	pContext,
						int				nNumFaces,
					    sgl_uint16  	*pFaces,
						PSGLVERTEX  	pVertices,
						sgl_bool		bFullSort,
						PRIMITIVETYPE	PrimitiveType)
{
	TEXAS_PRECOMP_STRUCT 	TPS;
	PIFUNCBLOCK				pFuncBlock;
	PPIR					fnPerPoly;
	sgl_uint32				uFuncBlockIndex;
	sgl_int32 				ISPSpaceAvailable;
	sgl_int32				nNextTriInc, nFuncIndex;
	sgl_uint32				nExtraPlanes = 0;  

	/* ini PDC
	 */
	gpPDC->Context = *pContext; 
	gpPDC->nInputTriangles = nNumFaces;
	gpPDC->pVertices = pVertices; 
	gpPDC->pFace = pFaces;

	/* Setup whether DECAL or not.
	 * If DECAL then we can't have smooth shading or highlights.
	 */
	if (gpPDC->Context.u32Flags & SGLTT_DECAL)
	{
		PackFlat = PackFlatDecalAndStore;

		/*
		// work out which function block we are using
		*/
		uFuncBlockIndex = gpPDC->Context.u32Flags & SGLTT_TEXTURE;
	}
	else
	{
		PackFlat = PackFlatAndStore;

		/*
		// work out which function block we are using
		*/
		uFuncBlockIndex = gpPDC->Context.u32Flags & (SGLTT_GOURAUD | SGLTT_TEXTURE | SGLTT_HIGHLIGHT);

		if(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD))
		{
			nExtraPlanes++;
		}
	}

	/* Calculate extra planes needed for pervertex for and gouraud highlights */
	/* this will be the maximum - we might not need all this as if we detect
	** that a poly has zero fog  we will not add the extra plane used for
	** fogging
	*/
	if(gpPDC->Context.u32Flags & SGLTT_VERTEXFOG)
	{
		nExtraPlanes++;
	}

	/* Is it in Z greater than equal or Z less than equal mode.
	 */
	if (gPDC.Context.u32Flags & SGLTT_ZGREATEREQUAL) /* not an SGL Direct flag */
	{
		/* To implement Z greater than equal to simply set the fields as
		 * follows. Then when 1/w is being calculated then the equation becomes
		 *
		 *	 	fZ * fMinInvZ
		 *
		 * which is what is required for this mode.
		 */
		gfInvertZ = 0.0f;
		FLOAT_TO_LONG(fMinInvZ) |= 0x80000000;		/* negate the value.	*/
	}
	else
	{
		/* To implement Z less than equal to simply set the fields as
		 * follows. Then when 1/w is being calculated then the equation becomes
		 *
		 *	 	(gfInvertZ - fZ) * fMinInvZ
		 *
		 * which is what is required for this mode.
		 */
		gfInvertZ = 1.0f;
		FLOAT_TO_LONG(fMinInvZ) &= ~0x80000000;	/* make it positive.	*/
	}

	if(gpPDC->Context.u32Flags & SGLTT_DEPTHBIAS)
	{
		gfDepthBias = BIGGER_BOGUSINVZ_INCREMENT * (float)(gpPDC->Context.uDepthBias);
	}
	else
	{
		gfDepthBias = 0.0f;
	}

	ISPSpaceAvailable = ( (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit) -
						  (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos) );

	if ( ISPSpaceAvailable < (nNumFaces * WORDS_PER_PLANE * 
							  (4 + nExtraPlanes)) )
	{
		if (ISPSpaceAvailable > 0)
		{
			/* only once per frame! */

			if(nExtraPlanes)
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * (4+nExtraPlanes));
			}
			else
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * 4);
			}
		}
		else
		{
			DPFDEV ((DBG_ERROR, "DirectTriPrimitive: ISP space overflowing"));
			return;
		}
	}

	/*
	// init TSP control word to 0 or not fogged
	*/
	if (!gpPDC->Context.bFogOn)
	{
		gpPDC->TSPControlWord = MASK_DISABLE_FOG;
	}
	else
	{
		gpPDC->TSPControlWord = 0;
		FogUsed = 1;
	}

	if (gpPDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
	{
		if(gpPDC->Context.u32Flags & SGLTT_VERTEXFOG)
		{
			sgl_uint32 uFogColour;

			uFogColour = 
				((sgl_uint32)(gpPDC->Context.fFogR*255)<<16) |
				((sgl_uint32)(gpPDC->Context.fFogG*255)<<8) |
				((sgl_uint32)(gpPDC->Context.fFogB*255));

			gpPDC->VFogTSPControlWord[0] = ((uFogColour >> 16) & 0x000000FF);
			gpPDC->VFogTSPControlWord[0] |= (MASK_TEXTURE | MASK_TRANS | MASK_DISABLE_FOG);
			gpPDC->VFogTSPControlWord[1] = (uFogColour << 16);
			pFuncBlock = NoVolVFogFuncs;
		}
		else
#if 0
		if (gpPDC->Context.u32Flags & SGLTT_NATIVESHADING)
		{
			pFuncBlock = NoVolNativeFuncs;
		}
		else
#endif
		{
			pFuncBlock = NoVolFuncs;
		}
	}
	else
	{
		if (gpPDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
		{
			pFuncBlock = ShadowFuncs;
		}
		else
		{
			pFuncBlock = LightVolFuncs;
		}
	}

	/* set up for global translucency if necessary
	 */
	if ((gpPDC->Context.u32Flags & SGLTT_GLOBALTRANS) 
		&& (gpPDC->Context.u32GlobalTrans < 0xF0))
	{
		uFuncBlockIndex |= 0x08;		

		if (!(gpPDC->Context.u32Flags & SGLTT_VERTEXTRANS))
		{
			sgl_uint32 Alpha = 0xF - (gpPDC->Context.u32GlobalTrans >> 4);
			gpPDC->TSPControlWord |= Alpha << 13;
		}
	}
	
	/* 
	// set up for vertex trans 
	// To speed up the Vertex translucency averaging calc,
	// pre-compute 85 * the global trans value / 128.
	*/
	if (gpPDC->Context.u32Flags & SGLTT_VERTEXTRANS)
	{
		uFuncBlockIndex |= 0x10;
		gpPDC->u32GlobalTransBy85div128 = (85 * gpPDC->Context.u32GlobalTrans) >> 7;
	}

#if PCX2_003

	/* Set the (few) blending modes that we have */
	if (!(gPDC.Context.u32Flags & SGLTT_BLENDDISABLE))
	{
		/* Source */
		switch (gpPDC->Context.uBlendModes & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break; 
			case sgl_blend_one: /* Set alpha bit */
				gpPDC->TSPControlWord |= MASK_BLEND_ALPHA | MASK_TRANS;
				break;
			case sgl_blend_srcalpha: /* Clear alpha bit */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break;
		}
		/* Destination */
		switch ((gpPDC->Context.uBlendModes >> 8) & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break; 
			case sgl_blend_one: /* Set 1/alpha bit */
				gpPDC->TSPControlWord |= MASK_BLEND_ONE_OVER_ALPHA | MASK_TRANS | MASK_GLOBAL_TRANS;
				break;
			case sgl_blend_invsrcalpha: /* Clear 1/alpha bit */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break;
		}
	}

#endif /* PCX2_003 */
	
	/* Y coordinate is in LINES NOT normal REGIONS
	 */
	gpPDC->Context.FirstYRegion *= RegionInfo.YSize;

	if ( ( (gpPDC->Context.LastYRegion+1) <= RegionInfo.NumYRegions ) ||
		 ( !RegionInfo.HasLeftOver ) )
	{
		/* Calculate accurate end of the Y Region */
		gpPDC->Context.LastYRegion = ( ( gpPDC->Context.LastYRegion + 1 ) *
									 RegionInfo.YSize ) - 1;
	}
	else
	{
		/* Last region was not full size, go to last line on screen */
		gpPDC->Context.LastYRegion = ( ( RegionInfo.NumYRegions - 1 ) *
									 RegionInfo.YSize ) + RegionInfo.LeftOverY;
	}

	/* XRegion scaling can be achieved with a shift */
	gpPDC->ShiftRegX = 5;		/* Start at 32 */
	
	if ( RegionInfo.XSize > (1<<gpPDC->ShiftRegX) )
	{
		do
		{
			/* Larger shifts, for 64 perhaps */
			gpPDC->ShiftRegX++;
		}
		while ( RegionInfo.XSize > (1<<gpPDC->ShiftRegX) );
	}
	else
	{
		while ( RegionInfo.XSize < (1<<gpPDC->ShiftRegX) )
		{
			/* Smaller Shifts ??? lets be future proof */
			gpPDC->ShiftRegX--;
		}
	}
	
	ASSERT( ( RegionInfo.XSize == (1<<gpPDC->ShiftRegX) ) );
	
	/* validate the texture if it's there */
	ProcessFlatTexFn = NULL;	

	/* Are we textured.
	 */
	if (gpPDC->Context.u32Flags & SGLTT_TEXTURE)
	{
		int			nIndex = 0;
		
		/* We always use the Texture API entry with D3D DrawPrimitve.
		 */
		TPS.TexAddress = (sgl_uint32) gpPDC->Context.nTextureName;
		gpPDC->invTexSize = TexasGetInvTextureDimension (&TPS);
		gpPDC->TexSize = 1.0f/gpPDC->invTexSize;
		
#if PCX2_003
		/* for D3D only, set translucent flag from the value of gpPDC->Context.u32Flags
		 */
		if(!(gpPDC->Context.u32Flags & SGLTT_BLENDDISABLE))
		{
			HasTranslucent = TRUE;
		}		
#endif

		gpPDC->TexAddress = TPS.TexAddress;
		TPS.LowWord = MASK_TEXTURE;
		
#if PCX2_003
		/* Disabled until PCX2-003.
		 */
		/* Need to set flag to indicate if bilinear filtering enabled.
		 * Used to indicate if texel offset to be utilised.
		 * Currently only available for PCX2.
		 * Has to be done here since it requires the result of ->invTexSize.
		 */
		if (!bFilteringOverRide)
		{
			/* We haven't overriden the application settings so continue.
			 * Bilinear enabled or not ????
			 */
			bBilinearEnabled = (gpPDC->Context.u32Flags & SGLTT_BILINEAR);
		}
#endif

#if PCX2
		/* We have overriden the application settings so fix texel if
		 * bilinear.
		 */
		if (bBilinearEnabled)
		{
			/* Bilinear.
			 */
			gpPDC->fHalfTexel = gpPDC->invTexSize * 0.5f;
		}
		else
		{
			/* non-Bilinear.
			 */
			gpPDC->fHalfTexel = 0.0f;
		}
#else
		/* If any card other than PCX2 then it is assumed to be non-Bilinear.
		 */
		gpPDC->fHalfTexel = 0.0f;
#endif

		if (gpPDC->Context.bFlipU)
		{
			gpPDC->TSPControlWord |= MASK_FLIP_U;
		}

		if (gpPDC->Context.bFlipV)
		{
			gpPDC->TSPControlWord |= MASK_FLIP_V;
		}
	
		if (TPS.TexAddress & MASK_4444_555)
		{
			gpPDC->TSPControlWord |= MASK_TRANS;
			/* if it is a translucent texture we should make sure and
			** flatten off smooth shading if there are shadows or L-Vs
			** i.e. treat it as if global translucency is set 
			*/
			#define SMFLATTENMASK	(SGLTT_GOURAUD | SGLTT_GLOBALTRANS | SGLTT_VERTEXTRANS)
			#define SMFLATTENRESULT	(SGLTT_GOURAUD)
			
			if (((gpPDC->Context.u32Flags & SMFLATTENMASK) == SMFLATTENRESULT)
				&& (gpPDC->Context.eShadowLightVolMode != NO_SHADOWS_OR_LIGHTVOLS))
			{
				/* 
				   no need to worry about the actual global trans
				   value in TSPControlWord, it's 0 by default
				*/
				uFuncBlockIndex |= 0x08;
			}
		}

		if (gpPDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
		{
			gpPDC->n32MipmapOffset = gpPDC->Context.n32MipmapOffset;
		}

		/* Setup correct process texture handler. 8 choices:
		 *
		 *		0. D3D flat texture, perspective correct.
		 *		1. D3D flat texture, wrap U, perspective correct.
		 *		2. D3D flat texture, wrap V, perspective correct.
		 *		3. D3D flat texture, wrap U, wrap V, perspective correct.
		 *		4. D3D flat texture, non-perspective correct.
		 *		5. D3D flat texture, wrap U, non-perspective correct.
		 *		6. D3D flat texture, wrap V, non-perspective correct.
		 *		7. D3D flat texture, wrap U, wrap V, non-perspective correct.
		 */
		/* Determine which function to call.
		 */
		nIndex = 0;

		if (gpPDC->Context.u32Flags & SGLTT_WRAPU)
			nIndex += 0x1;

		if (gpPDC->Context.u32Flags & SGLTT_WRAPV)
			nIndex += 0x2;

		if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
			nIndex += 0x4;

		ProcessFlatTexFn = ProcessD3DTSPFunctions[nIndex];

		/* Setup correct pack texture handler. 4 choices:
		 *
		 *		0. Non mip-mapped, perspective correct.
		 *		1. Mip-mapped, perspective correct.
		 *		2. Non mip-mapped, non-perspective correct.
		 *		3. Mip mapped, non-perspective correct.
		 */
		/* Determine which function to call.
		 */
		nIndex = 0;

		if (gpPDC->TexAddress & MASK_MIP_MAPPED)
			nIndex += 0x1;

		if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
			nIndex += 0x2;

		PackTSPTexFn = PackTSPFunctions[nIndex];
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
	
	pFuncBlock += uFuncBlockIndex;
	
	gpPDC->TSPControlWord |= pFuncBlock->TSPControlWord;

	fnPerPoly = pFuncBlock->fnPerPoly;

	if ( ( fnPerPoly == ProcessFlatTex ) && ( ProcessFlatTexFn != NULL ) )
	{
		/* Process loops do this for free if ProcessFlatTexFn != NULL */
		fnPerPoly = NULL;
	}
	
#ifdef METRIC
	Times[INPUT_D3DTRI_COUNT].Count += 1;
	Times[INPUT_D3DTRI_COUNT].Total += nNumFaces;
#endif


	/* Both opaque and translucent triangles are handled by the same
	 * Core functions.
	 */
	/* Need to determine the type of DrawPrimitive triangle.
	 */
	switch(PrimitiveType)
	{
		/* Need to determine the type of DrawPrimitive triangle.
		 */
	case TRIANGLELIST:
		/* Triangle lists.
		 * The triangle index count here is 1.
		 *
		 * T1 = V1, V2, V3
		 * T2 = V4, V5, V6
		 * .
		 * Tn = V3n-2, V3n-1, V3n
		 *
		 * After processing triangle n the face index count
		 * is 3n. The first vertex for Tn+1 is V3(n+1)-2
		 * which equals 3n+1. Therefore the face index increment
		 * is 1.
		 */
		nFuncIndex = 0;
		nNextTriInc = 1;

		/* Initialise the first face index value.
		 */
		nFaces = 0;

		bStrip = FALSE;

		break;

	case TRIANGLESTRIP:
		/* Triangle strips.
		 * The triangle index count here is -1.
		 *
		 * T1 = V1, V2, V3
		 * T2 = V2, V3, V4
		 * .
		 * Tn = Vn, Vn+1, Vn+2
		 *
		 * After processing triangle n the face index count
		 * is n+2. The first vertex for Tn+1 is V(n+1)
		 * Therefore the face index increment is -1.
		 */
		nFuncIndex = 0;
		nNextTriInc = -1;

		/* Initialise the first face index value.
		 */
		nFaces = 0;

		bStrip = TRUE;

		break;

	case TRIANGLEFAN:
		/* Triangle Fans.
		 */
		nFuncIndex = 1;
		nNextTriInc = 0;

		/* Initialise the first face index value.
		 * For fans it always starts at the 2nd vertex.
		 */
		nFaces = 1;

		break;

	default:
		/* Well look here, you got to specify some triangle type.
		 */
		DPFDEV((DBG_ERROR,"DirectTriPrimitive: Invalid Primitive type."));	
		nFuncIndex = -1;
		break;
	}

	if (nFuncIndex >= 0)
	{
	#if DAG_TRANS_SORTING
		/* Do we use the full sort which is better or not.
		 */
		if ((gpPDC->TSPControlWord & MASK_TRANS) &&
			!(gpPDC->Context.u32Flags & SGLTT_OPAQUE) &&
			bFullSort)
		{
			ProcessFunctionPrim = ProcessCorePrimtives[nFuncIndex + 2];

			ProcessD3DTransTris (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize, nNextTriInc);
		}
		else
	#endif /*DAG_TRANS_SORTING	*/
		{
			ProcessFunctionPrim = ProcessCorePrimtives[nFuncIndex];

			/* Always call this D3D routine.
			 */
			ProcessD3DTriPrimitives (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize, nNextTriInc);
		}
	}
	
	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif
}


/**************************************************************************
 * Function Name  : DirectD3DPolygons
 * Inputs         : PVRHANDLE TexHeap
 *					PSGLCONTEXT pContext
 *					int  nNumFaces
 *					int  pFaces[][3]
 *					PSGLVERTEX  pVertices
 *					sgl_bool bFullSort - selects between sorting algorithm.
 * Outputs        : None
 * Returns        : Nonw
 * Global Used    : gPDC, gpTriPlus, gpgpPDC
 * Description    : D3D Version of DirectPolygons
 *	
 **************************************************************************/

void DirectD3DPolygons ( PVRHANDLE TexHeap,
					  	 PSGLCONTEXT  pContext,
						 int  nNumFaces,
					     int  pFaces[][3],
						 PSGLVERTEX  pVertices,
						 sgl_bool	bFullSort)
{
	TEXAS_PRECOMP_STRUCT  TPS;
	PIFUNCBLOCK			  pFuncBlock;
	PPIR				  fnPerPoly;
	sgl_uint32			  uFuncBlockIndex;
	sgl_int32 			  ISPSpaceAvailable;
	sgl_uint32            nExtraPlanes = 0;  

	/* ini PDC
	 */
	gpPDC->Context = *pContext; 
	gpPDC->nInputTriangles = nNumFaces;
	gpPDC->pVertices = pVertices; 
	gpPDC->pFace = pFaces;

	/* Setup whether DECAL or not.
	 * If DECAL then we can't have smooth shading or highlights.
	 */
	if (gpPDC->Context.u32Flags & SGLTT_DECAL)
	{
		PackFlat = PackFlatDecalAndStore;

		/*
		// work out which function block we are using
		*/
		uFuncBlockIndex = gpPDC->Context.u32Flags & SGLTT_TEXTURE;
	}
	else
	{
		PackFlat = PackFlatAndStore;

		/*
		// work out which function block we are using
		*/
		uFuncBlockIndex = gpPDC->Context.u32Flags & (SGLTT_GOURAUD | SGLTT_TEXTURE | SGLTT_HIGHLIGHT);

		if(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD))
		{
			nExtraPlanes++;
		}
	}

	/* Calculate extra planes needed for pervertex for and gouraud highlights */
	/* this will be the maximum - we might not need all this as if we detect
	** that a poly has zero fog  we will not add the extra plane used for
	** fogging
	*/
	if(gpPDC->Context.u32Flags & SGLTT_VERTEXFOG)
	{
		nExtraPlanes++;
	}

	/* Is it in Z greater than equal or Z less than equal mode.
	 */
	if (gPDC.Context.u32Flags & SGLTT_ZGREATEREQUAL) /* not an SGL Direct flag */
	{
		/* To implement Z greater than equal to simply set the fields as
		 * follows. Then when 1/w is being calculated then the equation becomes
		 *
		 *	 	fZ * fMinInvZ
		 *
		 * which is what is required for this mode.
		 */
		gfInvertZ = 0.0f;
		FLOAT_TO_LONG(fMinInvZ) |= 0x80000000;		/* negate the value.	*/
	}
	else
	{
		/* To implement Z less than equal to simply set the fields as
		 * follows. Then when 1/w is being calculated then the equation becomes
		 *
		 *	 	(gfInvertZ - fZ) * fMinInvZ
		 *
		 * which is what is required for this mode.
		 */
		gfInvertZ = 1.0f;
		FLOAT_TO_LONG(fMinInvZ) &= ~0x80000000;	/* make it positive.	*/
	}

	if(gpPDC->Context.u32Flags & SGLTT_DEPTHBIAS)
	{
		gfDepthBias = BIGGER_BOGUSINVZ_INCREMENT * (float)(gpPDC->Context.uDepthBias);
	}
	else
	{
		gfDepthBias = 0.0f;
	}

	ISPSpaceAvailable = ( (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit) -
						  (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos) );

	if ( ISPSpaceAvailable < (nNumFaces * WORDS_PER_PLANE * 
							  (4 + nExtraPlanes)) )
	{
		if (ISPSpaceAvailable > 0)
		{
			/* only once per frame! */

			if(nExtraPlanes)
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * (4+nExtraPlanes));
			}
			else
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * 4);
			}
		}
		else
		{
			DPFDEV ((DBG_ERROR, "DirectD3DPolygons: ISP space overflowing"));
			return;
		}
	}

	/*
	// init TSP control word to 0 or not fogged
	*/

	if (!gpPDC->Context.bFogOn)
	{
		gpPDC->TSPControlWord = MASK_DISABLE_FOG;
	}
	else
	{
		gpPDC->TSPControlWord = 0;
		FogUsed = 1;
	}

	/* Need to set the Boguz Inv Z value if it is provided by the user.
	 * This is to be used for the ISP in the depth calculation.
	 */
	if (gpPDC->Context.u32Flags & SGLTT_BOGUSINVZPROVIDED)
	{
		/* Read the bogus inverse Z provided.
		 * Scale the value correctly.
		 */
		gfBogusInvZ = (gpPDC->Context.fBogusInvZ * fMinInvZ);
	}

	if (gpPDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
	{
		if(gpPDC->Context.u32Flags & SGLTT_VERTEXFOG)
		{
			sgl_uint32 uFogColour;

			uFogColour = 
				((sgl_uint32)(gpPDC->Context.fFogR*255)<<16) |
				((sgl_uint32)(gpPDC->Context.fFogG*255)<<8) |
				((sgl_uint32)(gpPDC->Context.fFogB*255));

			gpPDC->VFogTSPControlWord[0] = ((uFogColour >> 16) & 0x000000FF);
			gpPDC->VFogTSPControlWord[0] |= (MASK_TEXTURE | MASK_TRANS | MASK_DISABLE_FOG);
			gpPDC->VFogTSPControlWord[1] = (uFogColour << 16);
			pFuncBlock = NoVolVFogFuncs;
		}
		else
#if 0
		if (gpPDC->Context.u32Flags & SGLTT_NATIVESHADING)
		{
			pFuncBlock = NoVolNativeFuncs;
		}
		else
#endif
		{
			pFuncBlock = NoVolFuncs;
		}
	}
	else
	{
		if (gpPDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
		{
			pFuncBlock = ShadowFuncs;
		}
		else
		{
			pFuncBlock = LightVolFuncs;
		}
	}

	/* set up for global translucency if necessary
	 */
	if ((gpPDC->Context.u32Flags & SGLTT_GLOBALTRANS) 
		&& (gpPDC->Context.u32GlobalTrans < 0xF0))
	{
		uFuncBlockIndex |= 0x08;		

		if (!(gpPDC->Context.u32Flags & SGLTT_VERTEXTRANS))
		{
			sgl_uint32 Alpha = 0xF - (gpPDC->Context.u32GlobalTrans >> 4);
			gpPDC->TSPControlWord |= Alpha << 13;
		}
	}
	
	/* 
	// set up for vertex trans 
	// To speed up the Vertex translucency averaging calc,
	// pre-compute 85 * the global trans value / 128.
	*/
	if (gpPDC->Context.u32Flags & SGLTT_VERTEXTRANS)
	{
		uFuncBlockIndex |= 0x10;
		gpPDC->u32GlobalTransBy85div128 = (85 * gpPDC->Context.u32GlobalTrans) >> 7;
	}
	
#if PCX2_003

	/* Set the (few) blending modes that we have */
	if (!(gPDC.Context.u32Flags & SGLTT_BLENDDISABLE))
	{
		/* Source */
		switch (gpPDC->Context.uBlendModes & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break; 
			case sgl_blend_one: /* Set alpha bit */
				gpPDC->TSPControlWord |= MASK_BLEND_ALPHA | MASK_TRANS;
				break;
			case sgl_blend_srcalpha: /* Clear alpha bit */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break;
		}
		/* Destination */
		switch ((gpPDC->Context.uBlendModes >> 8) & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break; 
			case sgl_blend_one: /* Set 1/alpha bit */
				gpPDC->TSPControlWord |= MASK_BLEND_ONE_OVER_ALPHA | MASK_TRANS | MASK_GLOBAL_TRANS;
				break;
			case sgl_blend_invsrcalpha: /* Clear 1/alpha bit */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break;
		}
	}

#endif /* PCX2_003 */
	
	/* Y coordinate is in LINES NOT normal REGIONS */
	gpPDC->Context.FirstYRegion *= RegionInfo.YSize;
	
	if ( ( (gpPDC->Context.LastYRegion+1) <= RegionInfo.NumYRegions ) ||
		 ( !RegionInfo.HasLeftOver ) )
	{
		/* Calculate accurate end of the Y Region */
		gpPDC->Context.LastYRegion = ( ( gpPDC->Context.LastYRegion + 1 ) *
									 RegionInfo.YSize ) - 1;
	}
	else
	{
		/* Last region was not full size, go to last line on screen */
		gpPDC->Context.LastYRegion = ( ( RegionInfo.NumYRegions - 1 ) *
									 RegionInfo.YSize ) + RegionInfo.LeftOverY;
	}
	
	/* XRegion scaling can be achieved with a shift */
	gpPDC->ShiftRegX = 5;		/* Start at 32 */
	
	if ( RegionInfo.XSize > (1<<gpPDC->ShiftRegX) )
	{
		do
		{
			/* Larger shifts, for 64 perhaps */
			gpPDC->ShiftRegX++;
		}
		while ( RegionInfo.XSize > (1<<gpPDC->ShiftRegX) );
	}
	else
	{
		while ( RegionInfo.XSize < (1<<gpPDC->ShiftRegX) )
		{
			/* Smaller Shifts ??? lets be future proof */
			gpPDC->ShiftRegX--;
		}
	}
	
	ASSERT( ( RegionInfo.XSize == (1<<gpPDC->ShiftRegX) ) );
	
	/* validate the texture if it's there */
	ProcessFlatTexFn = NULL;	

	if (gpPDC->Context.u32Flags & SGLTT_TEXTURE)
	{
		int			nIndex = 0;
		
		/* We always use the Texture API entry with D3D DrawPrimitve.
		 */
		TPS.TexAddress = (sgl_uint32) gpPDC->Context.nTextureName;
		gpPDC->invTexSize = TexasGetInvTextureDimension (&TPS);
		gpPDC->TexSize = 1.0f/gpPDC->invTexSize;
		
#if PCX2_003
		/* for D3D only, set translucent flag from the value of gpPDC->Context.u32Flags
		 */
		if(!(gpPDC->Context.u32Flags & SGLTT_BLENDDISABLE))
		{
			HasTranslucent = TRUE;
		}		
#endif

		gpPDC->TexAddress = TPS.TexAddress;
		TPS.LowWord = MASK_TEXTURE;
		
#if PCX2_003
		/* Disabled until PCX2-003.
		 */
		/* Need to set flag to indicate if bilinear filtering enabled.
		 * Used to indicate if texel offset to be utilised.
		 * Currently only available for PCX2.
		 * Has to be done here since it requires the result of ->invTexSize.
		 */
		if (!bFilteringOverRide)
		{
			/* We haven't overriden the application settings so continue.
			 * Bilinear enabled or not ????
			 */
			bBilinearEnabled = (gpPDC->Context.u32Flags & SGLTT_BILINEAR);
		}
#endif

#if PCX2
		/* We have overriden the application settings so fix texel if
		 * bilinear.
		 */
		if (bBilinearEnabled)
		{
			/* Bilinear.
			 */
			gpPDC->fHalfTexel = gpPDC->invTexSize * 0.5f;
		}
		else
		{
			/* non-Bilinear.
			 */
			gpPDC->fHalfTexel = 0.0f;
		}
#else
		/* If any card other than PCX2 then it is assumed to be non-Bilinear.
		 */
		gpPDC->fHalfTexel = 0.0f;
#endif

		if (gpPDC->Context.bFlipU)
		{
			gpPDC->TSPControlWord |= MASK_FLIP_U;
		}

		if (gpPDC->Context.bFlipV)
		{
			gpPDC->TSPControlWord |= MASK_FLIP_V;
		}
	
		if (TPS.TexAddress & MASK_4444_555)
		{
			gpPDC->TSPControlWord |= MASK_TRANS;
			/* if it is a translucent texture we should make sure and
			** flatten off smooth shading if there are shadows or L-Vs
			** i.e. treat it as if global translucency is set 
			*/
			#define SMFLATTENMASK	(SGLTT_GOURAUD | SGLTT_GLOBALTRANS | SGLTT_VERTEXTRANS)
			#define SMFLATTENRESULT	(SGLTT_GOURAUD)
			
			if (((gpPDC->Context.u32Flags & SMFLATTENMASK) == SMFLATTENRESULT)
				&& (gpPDC->Context.eShadowLightVolMode != NO_SHADOWS_OR_LIGHTVOLS))
			{
				/* 
				   no need to worry about the actual global trans
				   value in TSPControlWord, it's 0 by default
				*/
				uFuncBlockIndex |= 0x08;
			}
		}

		if (gpPDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
		{
			gpPDC->n32MipmapOffset = gpPDC->Context.n32MipmapOffset;
		}

		/* Setup correct process texture handler. 8 choices:
		 *
		 *		0. D3D flat texture, perspective correct.
		 *		1. D3D flat texture, wrap U, perspective correct.
		 *		2. D3D flat texture, wrap V, perspective correct.
		 *		3. D3D flat texture, wrap U, wrap V, perspective correct.
		 *		4. D3D flat texture, non-perspective correct.
		 *		5. D3D flat texture, wrap U, non-perspective correct.
		 *		6. D3D flat texture, wrap V, non-perspective correct.
		 *		7. D3D flat texture, wrap U, wrap V, non-perspective correct.
		 */
		/* Determine which function to call.
		 */
		nIndex = 0;

		if (gpPDC->Context.u32Flags & SGLTT_WRAPU)
			nIndex += 0x1;

		if (gpPDC->Context.u32Flags & SGLTT_WRAPV)
			nIndex += 0x2;

		if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
			nIndex += 0x4;

		ProcessFlatTexFn = ProcessD3DTSPFunctions[nIndex];

		/* Setup correct pack texture handler. 4 choices:
		 *
		 *		0. Non mip-mapped, perspective correct.
		 *		1. Mip-mapped, perspective correct.
		 *		2. Non mip-mapped, non-perspective correct.
		 *		3. Mip mapped, non-perspective correct.
		 */
		/* Determine which function to call.
		 */
		nIndex = 0;

		if (gpPDC->TexAddress & MASK_MIP_MAPPED)
			nIndex += 0x1;

		if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
			nIndex += 0x2;

		PackTSPTexFn = PackTSPFunctions[nIndex];
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
	
	pFuncBlock += uFuncBlockIndex;

	gpPDC->TSPControlWord |= pFuncBlock->TSPControlWord;

	fnPerPoly = pFuncBlock->fnPerPoly;

	if ( ( fnPerPoly == ProcessFlatTex ) && ( ProcessFlatTexFn != NULL ) )
	{
		/* Process loops do this for free if ProcessFlatTexFn != NULL */
		fnPerPoly = NULL;
	}
	
	if (gpPDC->Context.u32Flags & SGLTT_USED3DSTRIPFLAGS)
	{
		#ifdef METRIC
			Times[INPUT_D3DPOLY_COUNT].Count += 1;
			Times[INPUT_D3DPOLY_COUNT].Total += nNumFaces;
		#endif
		ProcessD3DPolys (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
	}		
	else
	{
		#ifdef METRIC
			Times[INPUT_D3DTRI_COUNT].Count += 1;
			Times[INPUT_D3DTRI_COUNT].Total += nNumFaces;
		#endif
		#if DAG_TRANS_SORTING
			/* Do we use the full sort which is better or not.
			 */
			if ((gpPDC->TSPControlWord & MASK_TRANS) &&
				!(gpPDC->Context.u32Flags & SGLTT_OPAQUE) &&
				bFullSort)
			{
				/* Setup the core triangle process routine as the DAG sorter.
				 */
				ProcessFunctionPrim = ProcessD3DTransTriCoreLite;

				ProcessD3DTransTris (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize ,0);
			}
			else
		#endif /*DAG_TRANS_SORTING	*/
			{
				/* Setup the core triangle process routine as the regular triangle process routine.
				 */
				ProcessFunctionPrim = ProcessTriCoreLite;
				ProcessD3DTriPrimitives (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize, 0);
			}
	}

	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif
}


/*------------------------------- End of File -------------------------------*/

