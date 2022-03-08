/*****************************************************************************
 *;++
 * Name            :    $Source: /user/rcs/revfiles/sabre/sgl/RCS/pvrd.c,v $
 * Title           :    PVRDirect function entry points.
 * C Author        :    Edward Forde
 * Created         :    21/04/97
 * 
 * Copyright	   : 1995-2022 Imagination Technologies (c)
 * License		   : MIT
 *
 * Description     :	Entry routines to PVRDirect.
 *                    
 * Program Type    :	C module (ANSI)
 *
 * RCS info:
 * $Log: pvrd.c,v $
 * Revision 1.91  1998/01/15  16:35:40  dm
 * Initialised variables when setting defaults for
 * Masking PLane and Masking Colour
 *
 * Revision 1.90  1997/12/15  16:28:10  erf
 * Removed useless calls to SglReadPrivateProfileInt() in
 * PVRDCreateRenderContext() since they will be cancelled by
 * the next call anyway.
 *
 * Revision 1.89  1997/11/05  15:17:02  mjg
 * Extended the DAG_SORTING compile switches around the no-sort
 * pieces of code so that it would build for PCX1.
 *
 * Revision 1.88  1997/11/04  17:45:18  sxy
 * added a global gNoSortTransFaces for no sort trans face lists.
 *
 * Revision 1.87  1997/10/28  16:09:44  mjg
 * Added Developer build stuff that was missing.
 *
 * Revision 1.86  1997/10/23  10:14:33  erf
 * Added gfInvertZ which is used to calculate the 1/w for the ISP
 * from the provided Z.
 *
 * Revision 1.85  1997/10/17  08:34:39  erf
 * Added app hint dithering override control.
 *
 * Revision 1.84  1997/10/13  11:07:43  erf
 * We now pass the bFullSortInUse flag to the drawprimitive handler.
 *
 * Revision 1.83  1997/09/25  12:03:28  sxy
 * changed debug level to MESSAGE for DEBUGDEV.
 *
 * Revision 1.82  1997/09/24  14:45:01  gdc
 * made ini file reads etc. follow new way of doing it
 *
 * Revision 1.81  1997/09/24  10:56:12  gdc
 * rationalised some SglReadPrivateProfile and GetAppHint stuff to read
 * using the new HWRdValFileXXX functions which read in a defined order
 *
 * Revision 1.80  1997/09/24  09:40:48  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values
 *
 * Revision 1.79  1997/09/22  11:32:08  gdc
 * chnaged AddToXY to -0.505 from -0.5 to get rid of ShadowsOfTheEmpire dropped
 * polys, and upped the render timeout for winbench
 *
 * Revision 1.78  1997/09/19  15:13:38  mjg
 * Removed the per-frame old/new sort decision as the current
 * sorting method should be fast enough.
 *
 * Revision 1.77  1997/09/17  15:19:54  sxy
 * Removed warnings.
 *
 * Revision 1.76  1997/09/17  13:27:36  mjg
 * Increased the size of the old/new sort cutoff.
 *
 * Revision 1.75  1997/09/16  18:42:30  mjg
 * Now releases transorting memory if not to be used.
 *
 * Revision 1.74  1997/09/15  16:09:51  mjg
 * Moved around the bFullSort code so that it won't crash if
 * the memort malloc fails.
 *
 * Revision 1.73  1997/09/11  18:07:20  mjg
 * Put back the sorting method selection.
 *
 * Revision 1.72  1997/09/10  17:16:22  erf
 * Added control from HAL to enable disable dithering based on the
 * render state set.
 *
 * Revision 1.71  1997/09/05  15:04:59  mjg
 * Removed adaptive translucent sorting switch, it does it itself
 *
 * Revision 1.70  1997/09/04  18:44:24  erf
 * Fixed app hint disabling mechanism.
 *
 * Revision 1.69  1997/09/04  18:14:48  erf
 * 1. Adaptive switch between translucent sorters based on the number
 *    of translucent triangles in the last frame.
 * 2. Bilinear stuff for PCX2-003.
 *
 * Revision 1.68  1997/09/03  15:31:00  erf
 * 1. Added FullSort application hint to the registry.
 * 2. Changed DirectD3DPolyons to pass bFullSort parameter. D3D now
 *    always calls this routine instead of DirectPolygons.
 *
 * Revision 1.67  1997/09/03  12:00:44  erf
 * Changed DirectD3DPolygons prototype definition.
 *
 * Revision 1.66  1997/09/01  12:18:20  mjg
 * Changed call for DirectPolygons to DirectD3DPolygons
 * ,.
 *
 * Revision 1.65  1997/08/28  15:57:13  gdc
 * added region grid lines to pvrd
 *
 * Revision 1.64  1997/08/26  16:38:28  erf
 * Added functionality to read if masking plane colour can be specified.
 * Again, it only effects blending.
 *
 * Revision 1.63  1997/08/22  09:59:36  gdc
 * set VertexFogControlWord
 *
 * Revision 1.62  1997/08/21  11:16:35  sks
 * Added VertexFogControlWord.
 *
 * Revision 1.61  1997/08/20  16:54:26  erf
 * Added D3D DrawPrimitive support. New routine PVRDTrianglesPrimitive added
 * and change to PVRDPoints so that a point list is passed in. Needed
 * for DrawIndexedPrimitive.
 * So full DrawPrimitive support has been added but is needs to be enabled
 * in the hal. ie DRAW_PRIM = 1 in pvrhal32.c .
 *
 * Revision 1.60  1997/08/13  16:55:09  mjg
 * Rather than just the current TSP, DumpSabreAndTexas outputs all
 * 4MB of texture memory.
 *
 * Revision 1.59  1997/08/13  09:12:34  mjg
 * Resurrected the Dump Sabre and Texas (and Object pointer) routine.
 *
 * Revision 1.58  1997/08/08  19:44:24  mjg
 * Removed global bilinear option for PCX2_003 (no longer a feature)
 *
 * Revision 1.57  1997/08/05  16:10:50  erf
 * Added facility to specify the maximum allowable translucent passes.
 *
 * Revision 1.56  1997/07/28  13:59:20  mjg
 * Removed the choice of masking plane type for PCX2_003
 *
 * Revision 1.55  1997/07/24  11:08:30  gdc
 * made a full region reset when the region size is chnaged
 *
 * Revision 1.54  1997/07/21  08:52:22  gdc
 * tightened a check
 *
 * Revision 1.53  1997/07/18  08:52:28  gdc
 * added code to allow the HAL to change Y region height
 *
 * Revision 1.52  1997/07/14  19:31:04  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.51  1997/07/14  13:57:43  gdc
 * added a module id
 *
 * Revision 1.50  1997/07/14  13:55:44  gdc
 * removed repeptitive string
 *
 * Revision 1.49  1997/07/03  17:56:36  erf
 * Added PVRDShadows() routine to enable shadow and light volumes
 * in D3D.
 *
 * Revision 1.48  1997/06/23  09:47:48  erf
 * Read PVRHAL section in the registry or sglhw.ini to enable/disable
 * full masking plane. The full masking plane is disabled by default.
 * The default setting WILL cause problems with sparkles in D3D apps
 * that required 2D on 3D.
 *
 * Revision 1.47  1997/06/22  17:37:42  jop
 * Added adaptive TSP parameter stuff
 *
 * Revision 1.46  1997/06/18  14:41:21  mjg
 * Fixed PCX2 slow down with a mask-plane fix - made it PCX2 only.
 *
 * Revision 1.45  1997/06/17  11:52:26  mjg
 * Added in john's changes
 *
 * Revision 1.44  1997/06/17  09:35:11  mjg
 * Added the FORCE_NO_FPU option.
 *
 * Revision 1.42  1997/06/11  11:00:49  erf
 * Initialise fMinInvZ with INVZ_DEFAULT_SCALE_VALUE which is defined in
 * sgl.h .
 *
 * Revision 1.41  1997/06/09  08:38:23  erf
 * 1. Fixed some compiler warnings.
 * 2. Tidied code for proper error reporting. The Hal still requires
 *    PVROS_DODGY to be returned from the strip rendering. Changes
 *    required in the Hal. OK for the time being.
 *
 * Revision 1.40  1997/05/23  19:02:36  mjg
 * Fixed XCLIP and 32-bits-per-pixel colour for Strip Rendering.
 *
 * Revision 1.39  1997/05/22  23:26:16  jop
 * Enabled strip rendering
 *
 * Revision 1.38  1997/05/21  16:25:39  erf
 * D3D Hal no automatically set the bits per pixel. ie 15, 16 etc...
 *
 * Revision 1.37  1997/05/21  09:43:32  jrc
 * Changed pixel offset to -0.5f for 1:1 texel:pixel mapping in bilinear.
 *
 * Revision 1.36  1997/05/20  09:30:01  mjg
 * Fixed bug with the right XCLIP value.
 *
 * Revision 1.35  1997/05/19  15:50:19  erf
 * Simply set the fgBogusInvZ to 0 at the begining of each frame.
 *
 * Revision 1.34  1997/05/19  15:16:36  erf
 * 1. Added prototype definitions for ISP and TSP functions.
 * 2. Set fgBogusInvZ to some crazy value.
 * 3. No need to copy end of render status word. Hal now reads it directly.
 *
 * Revision 1.33  1997/05/16  17:19:27  erf
 * Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
 *
 * Revision 1.32  1997/05/16  13:33:09  erf
 * Change where the end of render status word comes from.
 *
 * Revision 1.31  1997/05/15  15:11:30  erf
 * Set X clip register directly.
 *
 * Revision 1.30  1997/05/15  10:35:06  erf
 * 1. No need to check render address received from hal.
 * 2. Tidied code.
 * 3. Removed WaitForHardware() stuff from strip rendering.
 *
 * Revision 1.29  1997/05/15  09:12:47  erf
 * 1. Removed WaitForHardware() calls since this is handled by the hal.
 * 2. Set filter register on start of frame.
 *
 * Revision 1.28  1997/05/14  22:15:02  erf
 * 1. Write directly to most of the virtual registers.
 * 2. Moved tile and region info to here, to remove the need for
 *    hwdevice.c
 * 3. Removed #if'd out code.
 *
 * Revision 1.27  1997/05/14  17:28:46  gdc
 * removed call to set sabre ptr reg as this is done when the
 * virtual buffer is got
 *
 * Revision 1.26  1997/05/14  17:09:48  erf
 * Setup end of render pointer. Temporary solution, need to look at
 * WaitForHardware() to read this value from the logical device structure.
 *
 * Revision 1.25  1997/05/14  08:21:01  erf
 * Fixed so that it builds.
 *
 * Revision 1.24  1997/05/14  07:07:02  erf
 * 1. Removed ^M's.
 * 2. Added comments for last check in !!!!
 * 3. Changed fAddToXY to -0.125.
 * 4. Removed callback function.
 * 5. Added code to the prerender function.
 * 6. Removed thread priority stuff.
 *
 * Revision 1.23  1997/05/13  21:23:44  mjg
 * Changes to get the file to build - haven't a clue if it'll run or not.
 *
 * Revision 1.22  1997/05/13  19:43:20  erf
 * Some changes.
 *
 * Revision 1.21  1997/05/12  15:17:22  erf
 * Implemented PVRDirect lines.
 *
 * Revision 1.20  1997/05/12  09:21:27  erf
 * Reverted last changes to do with inverse square root tables.
 *
 * Revision 1.19  1997/05/09  17:51:48  erf
 * Removed call to create inverse square root table since maths functions
 * are only needed by SGL.
 *
 * Revision 1.18  1997/05/09  15:17:22  erf
 * 1. Removed AddDummyPlanes() and replaced with call to AddDummyPlanesL().
 * 2. Added #include "version.h", and print out version info.
 *
 * Revision 1.17  1997/05/08  14:13:31  erf
 * Reset the region data when the render context is setup, not
 * when the library is attacked.
 *
 * Revision 1.16  1997/05/08  11:29:38  erf
 * PVRDirect now a seperate library. Yahoo !!!
 *
 * Revision 1.15  1997/05/06  14:04:03  jop
 * Fails cleanly if the render address isn't initialised
 *
 * Revision 1.14  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.13  1997/05/01  10:13:29  erf
 * Added #include "pvrd.h".
 *
 * Revision 1.12  1997/05/01  08:31:31  erf
 * Added PVRDirect interface.
 *
 * Revision 1.11  1997/05/01  02:41:43  jop
 *  Undid Ed's fixes
 *
 * Revision 1.10  1997/04/30  22:39:09  erf
 * Still attempting to get D3D interface operational.
 *
 * Revision 1.9  1997/04/29  16:53:39  erf
 * Added new PVROS API function calling mechanism.
 *
 * Revision 1.8  1997/04/27  22:41:55  jop
 * Removed extra parameter from DestroyLogicalDevice call
 *
 * Revision 1.7  1997/04/26  18:50:42  erf
 * Separation changes.
 *
 * Revision 1.6  1997/04/24  12:00:59  erf
 * Use PVROSGetEndOfRenderPtr() to get the end of render status pointer.
 *
 * Revision 1.5  1997/04/23  15:03:18  erf
 * Fixed so that it compiles with the code.
 *
 * Revision 1.4  1997/04/22  17:26:28  gdc
 * added its own include (which could potentially reference JWH's boardif.h
 *
 * Revision 1.3  1997/04/22  17:10:50  erf
 * Changed long to sgl_uint32 for lStrip.
 *
 * Revision 1.2  1997/04/22  17:03:14  erf
 * Changed DWORD to sgl_uint32.
 *
 * Revision 1.1  1997/04/22  16:41:44  erf
 * Initial revision
 *
 *;--
 *****************************************************************************/

#define MODULE_ID MODID_PVRD

#include "sgl.h"
#include "sgl_init.h"
#include "pvrosapi.h"
#include "texapi.h"
#include "pvrd.h"
#include "debug.h"
#include "config.h"
#include "parmbuff.h"
#include "hwinterf.h"
#include "pmsabre.h"
#include "dregion.h"
#include "windows.h"
#include "version.h"
#include "hwregs.h"		/* needed for the virtual buffer addresses.	*/
#include "syscon.h"
#include "pvrlims.h"
#include "sglmacro.h"

#define SGL_APP 1
#include "metrics.h"

#ifdef DLL_METRIC


SGL_GLOBAL_TIME_DEFN

SGL_EXTERN_TIME_REF /* if we are timing code */

static int nRendersRecorded = -10;
static char szBuffer[128];

void InResetRegionData( void )
{
	SGL_TIME_STOP(SGLTRI_STARTOFFRAME1_TIME)
	if ( ++nRendersRecorded == 0 )
	{
		/* Set it to zero after the first few frames */
		memset( &Times, 0, NUM_TIMERS*sizeof(Temporal_Data) );
		
		/* Start recorded time */
		SGL_TIME_START(TOTAL_APP_TIME)
	}
	else
	{
		/* Monitor each frame period as the top level call */
		SGL_TIME_STOP(TOTAL_APP_TIME)
		SGL_TIME_START(TOTAL_APP_TIME)
	}
	SGL_TIME_START(SGLTRI_STARTOFFRAME2_TIME)
}

#endif

/* Default region sizes.
 */
#define	X_REGION_SIZE	32
#define	Y_REGION_SIZE	32	

/* Dynamic Regions defaults
 */
#define MERGE_HEIGHT	8
#define MIN_Y_SIZE		2

/* Used to select between the two sorting algorithms. Adaptivily varies
 * between frames based on the last frame translucent triangle count.
 */

#define	FULL_SORT_FINAL_CUTOFF	4000

/* Variable to fix the pixel offset problem. The value is added to
 * all vertices in D3D and SGL-Lite.
 * The offset is required because of the differences in precision,
 * in hardware, when storing the A, B and C parameters. In PCX1 and
 * PCX2 the C parameters has more bits of precision than the A and B
 * parameters.
 * The differences in precision is 2^(-16).
 */
float		fAddToXY			= -0.505f; /* must be the same as in sgllite.c */
float		fMinInvZ   			= INVZ_DEFAULT_SCALE_VALUE;
float		gfBogusInvZ			= 1.0f;
float		gfInvertZ			= 1.0f;
sgl_bool	FogUsed;
int			nTimeOutTenths		= 5;

static	sgl_uint32	bUseFullSort 	= 0x0;
static	sgl_uint32	uDithering		= 0x2;

/* No sort method, 1 fro no sort, 2 for reversed no sort, 3 for min z sort */
int gNoSortTransFaces = MINZ_SORT;

#if DAG_TRANS_SORTING
sgl_uint32	bFullSort = 0x0FFFFFFF;	/* Use full sorting as preference.	*/
extern void CloseRegionDataL( void );
#endif

/* This is maximum allowable translucent passes per tile.
 * Initialise to a crazy large value.
 */
sgl_uint32	nMaxPassCount		= 0x0FFFFFFF;

#if PCX2 || PCX2_003
/* Variable to decide whether to use full masking plane or not.
 * Defined in pktsp.c. Default is FALSE. Read from the registry.
 */
extern		sgl_bool	bFullMaskingPlane;
sgl_bool	bFilteringOverRide	= FALSE;
sgl_bool	bBilinearEnabled	= FALSE;
extern		sgl_bool	bSetMaskingBGColour;

extern sgl_uint16 guTransTriCounter; /* For swapping sorting */
#endif

/* Global device handles, texture handles etc...
 */
DEVICE_TYPE	gDeviceType;
DEVICEID	gDeviceID;
HINSTANCE	gDllHandle;
HLDEVICE	gHLogicalDev;
PTEXAPI_IF	gpTextureIF;

/* The translucent control word is the TEXAS control word that
 * 'points' to a single white pixel.This is used for non-textured
 * translucency.
 */
sgl_uint32 TranslucentControlWord;

/* The vertex fog control word is the TEXAS control word that
 * 'points' to a 16 graded level translucent texture.This is used
 * for per vertex fog.
 */
sgl_uint32 VertexFogControlWord;

/* Variable to decide if we have initialised the system or not
 *    0 - UNinitialised, NonZero - Initialised
 *
 * Set it to indicate that the system is uninitialised.
 */
static	int	sglSystemInitialised = 0;

/* Size of the device.
 */
static	int	nDeviceX;
static	int	nDeviceY;

/* Region informatin structure.
 */
static DEVICE_REGION_INFO_STRUCT	RegionInfo;

/* External prototype definitions. These are currently the SGLDirect routines.
 */
extern void DirectD3DPolygons ( PVRHANDLE TexHeap,
					 			PSGLCONTEXT  pContext,
								int  nNumFaces,
							    int  pFaces[][3],
								PSGLVERTEX  pVertices,
								sgl_bool	bUseFullSort);

extern void DirectPoints   (PSGLCONTEXT pContext, int nPoints, sgl_uint16 *pPoints, PSGLVERTEX pVertices);

extern void DirectLines	   (PSGLCONTEXT pContext, int nLines, 
							sgl_uint16 pLines[][2], PSGLVERTEX pVertices, sgl_uint32 nNextLineInc);

extern void DirectShadows	(PSGLCONTEXT pContext, int nNumFaces, int pFaces[][3],
			 	   			PSGLVERTEX pVertices, float fBoundingBox[2][2]);


extern void DirectTriPrimitive(PVRHANDLE TexHeap,
							PSGLCONTEXT  pContext,
							int  nNumFaces,
					    	sgl_uint16  *pFaces,
							PSGLVERTEX  pVertices,
							sgl_bool	bUseFullSort,
							PRIMITIVETYPE PrimitiveType);

/* External prototypes so that include files pkisp.h and pktsp.h
 * are not required.
 */
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
extern void PackBackgroundPlane ( const sgl_uint32 u32TexasTag,
							 const float  BackgroundDistance);
extern sgl_uint32 PackTexasMask(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn);
#else
extern void PackBackgroundPlane ( const sgl_uint32 u32TexasTag,
							 const sgl_int32  BackgroundDistance);
#endif

extern void AddDummyPlanesL(sgl_bool fShadowsOn);
extern sgl_uint32 PackTexasTransparent(sgl_bool ShadowsOn);
extern sgl_uint32 PackTexasFlat(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn);
extern void PackTexasFlatAtAddr(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn, sgl_uint32 Addr);

#define DUMP_SABRE_AND_TEXAS 0
#if DUMP_SABRE_AND_TEXAS
void DumpSabreAndTexas(void);
#pragma message (">>> Build set to dump Sabre and Texas parameters")
#endif

#define	API_FNBLOCK
#include "pvrd.h"
#undef	API_FNBLOCK

#define	API_INSTANTIATE
#include "pvrd.h"
#undef	API_INSTANTIATE


/******************************************************************************
 * Function Name: SglError
 *
 * Inputs       : nError
 * Outputs      : -
 * Returns      : nError
 * Globals Used : None.
 *
 * Description  : Stub function that should do nothing.
 *****************************************************************************/
int CALL_CONV SglError ( int nError )
{
	ASSERT(NULL);

	return nError;

}

#if DEBUG
/**********************************************************************
**
** Draw grid lines along region boundaries, useful aid when trying
** to determine why a vignette or other region related problem occurs
** as you can easily identify the region
**
**********************************************************************/

void DrawPVRDRegionGrid(PPVR_RENDERCONTEXT pRenderContext)
{
	PVR_RENDERCONTEXT GridContext = *pRenderContext;
	SGLVERTEX line[2];
	sgl_uint16 pLine[][2] = {0,1};
	int i;

	/* make the context safe for flat shaded */
	GridContext.SGLContext.u32Flags &= ~SGLTT_TEXTURE;
	GridContext.SGLContext.u32Flags &= ~SGLTT_GOURAUD;
	GridContext.SGLContext.u32Flags &= ~SGLTT_HIGHLIGHT;
	GridContext.SGLContext.u32Flags &= ~SGLTT_VERTEXTRANS;
	GridContext.SGLContext.u32Flags &= ~SGLTT_GLOBALTRANS;
	GridContext.SGLContext.u32Flags &= ~SGLTT_DISABLEZBUFFER;
	GridContext.SGLContext.uLineWidth = 1;

	line[0].u32Colour = 0x00ff00ff;
	line[1].u32Colour = 0x00ff00ff;

	line[0].fInvW = 1.0;
	line[1].fInvW = 1.0;
	line[0].fY = 0.0f; 
	line[1].fY = (float)(nDeviceY - 1);

	/* draw Y lines */
	for(i=RegionInfo.XSize-1; i<nDeviceX-1; i+=RegionInfo.XSize)
	{
		line[0].fX = (float)i;
		line[1].fX = (float)i;
		
		PVRDLines(&GridContext, 1, pLine, line, (PRIMITIVETYPE)0);
	}

	line[0].fX = 0.0f; 
	line[1].fX = (float)(nDeviceX - 1);
	/* draw X lines */
	for(i=RegionInfo.YSize-1; i<nDeviceY-1; i+=RegionInfo.YSize)
	{
		line[0].fY = (float)i;
		line[1].fY = (float)i;
		
		PVRDLines(&GridContext, 1, pLine, line, (PRIMITIVETYPE)0);
	}
}
#endif

/**************************************************************************
 * Function Name  : GetTextureHandle
 * Inputs Only    : 
 * Outputs Only   : 
 * Input/output	  : nTextureName    - The texture name
 * Returns        : NULL.
 * Global Used    : None
 * Description    : Returns the dummy texture handle. Stub function which
 *					should never be called.
 *
 **************************************************************************/

void * GetTextureHandle (int nTextureName)
{
	return(NULL);
}

/**************************************************************************
 * Function Name  : RnGlobalGetFixedClipDist
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : float or int
 * Global Used    : 
 * Description    : Gets the Fixed clipping distance value set in the
 *					projection matrix. For PCX2 this value is a float and
 *					for PCX1/ISPTSP this value is an int.
 *					Stub function wich replaces the one in rnglobal.c.
 *				   
 **************************************************************************/

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
float RnGlobalGetFixedClipDist ()
{
	return (1.0f);
}
#else
int RnGlobalGetFixedClipDist ()
{
	return (0x7FFFFFFF);
}
#endif

/**************************************************************************
 * Function Name  : sglInitialise
 * Inputs         : None
 * Outputs        : None
 * Returns        : 0 if OK, non zero otherwise.
 * Global Used    : sglSystemInitialised
 * Description    : Initialises the system by calling individual initialisation
 *					routines.
 *					Should only be called once.
 *
 **************************************************************************/

int CALL_CONV SglInitialise(void)
{
	/*
	// If we havent initialised the system, do so
	*/
	if(sglSystemInitialised == 0)
	{
		/* Print out the version information.
		 */
		PVROSPrintf("Initialising SGL Ver:%s\n", VER_LIB_TXT);

		#if DEBUG

			DebugInit (DBGPRIV_WARNING);

		#endif
		
		#if DEBUGDEV

			DebugInit (DBGPRIV_MESSAGE);

		#endif
		
		/* init fast inverse sqrt lookup table
		 */
		MakeInvSqrtLookupTable ();

		sglSystemInitialised = 1;
	}
	return(0);
}

/**************************************************************************
 * Function Name  : PVRDSetGlobal (internal only)
 * Inputs         : enumerated value, void function
 * Outputs        : 
 * Returns        : none
 * Global Used    : 
 * Description    : Sets up global variables used per instance
 *	
 **************************************************************************/

static void CALL_CONV PVRDSetGlobal (SGL_Globals eGlobal, void* pValue)
{
	switch (eGlobal)
	{
		case SGL_DeviceID:
		{
			gDeviceID = (DEVICEID) pValue;		
			break;
		}
		case SGL_DllHandle:
		{
			gDllHandle = (HINSTANCE) pValue;
			break;
		}
		case SGL_DeviceType:
		{
			gDeviceType = (DEVICE_TYPE) pValue;
			break;
		}
		case SGL_LogicalDev:
		{
			gHLogicalDev = (HLDEVICE) pValue;

			/* The translucent pixel HAS to be set up after the
			 * texture memory
			 */
			TranslucentControlWord = ((HTEXHEAP) gHLogicalDev->TexHeap)->TranslucentControlWord;
			VertexFogControlWord = ((HTEXHEAP) gHLogicalDev->TexHeap)->VertexFogControlWord;

			/* Setup a bunch of hardware registers.
			 */
			HWSetupBunchOfISPRegs();
			break;
		}
		case SGL_TextureIF:
		{
			gpTextureIF = (PTEXAPI_IF) pValue;
			break;
		}
		default:
		{
			DPF ((DBG_ERROR, "Invalid Global specified"));	
			break;
		}
	}
}

/**************************************************************************
 * Function Name  : SetRegionInfo (Internal)
 * Inputs         : None.
 * Outputs        : None.
 * Input/Output	  : x_dimension
 *					y_dimension
 * Returns        : None.
 * Global Used    : RegionInfo
 *
 * Description 	  : Sets up the region sizes etc.
 *				   
 **************************************************************************/

static void SetRegionInfo(int *x_dimension, int *y_dimension)
{
	nDeviceX = *x_dimension;
	nDeviceY = *y_dimension;

	/*
	// Get the region size... and then if not a legal size, choose one that
	// is!
	//
	// NOTE (for Midas3) the X sizes MUST be a multiple of the number of
	// sabre cells, and (for all boards) Y MUST be a multiple of the number
	// of sabres.  Note we are assuming 1 sabre!
	*/
	RegionInfo.XSize = HWRdValFileUInt ("SW_XRegionSize", X_REGION_SIZE);
	RegionInfo.YSize = HWRdValFileUInt ("SW_YRegionSize", Y_REGION_SIZE);

	/* Dynamic Region settings */
	RegionInfo.MergeHeight = HWRdValFileUInt ("SW_MergeHeight", MERGE_HEIGHT);
	RegionInfo.MinYSize	= HWRdValFileUInt ("SW_MinYSize", MIN_Y_SIZE);
	
	switch(RegionInfo.XSize) 
	{			/* See 'HWGetRegionSize' below for an explanation */
		case 0: 
			RegionInfo.XSize = NUM_SABRE_CELLS;
			break;
		case 32: 				/* Don't merge tiles sideways */
			RegionInfo.MergeHeight = 512;  /* Or any other big number would do */
			break;
		case 64:
			RegionInfo.XSize = 32;   /* Do merge tiles sideways */
			break;
		default:
			RegionInfo.XSize = (RegionInfo.XSize+ NUM_SABRE_CELLS -1)/ NUM_SABRE_CELLS;
			RegionInfo.XSize *= NUM_SABRE_CELLS;
		break;
	}		
		
	
	if(RegionInfo.YSize == 0)
	{
		RegionInfo.YSize = 1;
	}
	
	if(RegionInfo.MinYSize == 0)
	{
		RegionInfo.MinYSize = RegionInfo.YSize;
	}
	
	if(RegionInfo.MergeHeight == 0)
	{
		RegionInfo.MergeHeight = 2;
	}

	/*
	// Calculate how many regions there are
	*/
    RegionInfo.NumXRegions = (nDeviceX + RegionInfo.XSize - 1) / RegionInfo.XSize;
    RegionInfo.NumYRegions = (nDeviceY + RegionInfo.YSize - 1) / RegionInfo.YSize;

	/*
	// Check that we don't exceed the max number of regions we
	// can handle
	*/
	RegionInfo.NumXRegions = MIN(RegionInfo.NumXRegions, MAX_X_REGIONS);
	RegionInfo.NumYRegions = MIN(RegionInfo.NumYRegions, MAX_Y_REGIONS);

	/*
	// See if there is a small portion left over for y.
	*/
	RegionInfo.LeftOverY =	nDeviceY - ((RegionInfo.NumYRegions-1)*RegionInfo.YSize);
	RegionInfo.HasLeftOver = (RegionInfo.LeftOverY!=0);
}

/**************************************************************************
 * Function Name  : HWGetRegionInfo
 * Inputs         : None
 * Outputs        : pRegionInfo 
 * Input/Output	  : None.
 * Returns        : Always return 0.
 * Global Used    : RegionInfo
 * Description 	  : return device region info for a specified device
 *				   
 **************************************************************************/

int HWGetRegionInfo( int hDisplay,DEVICE_REGION_INFO_STRUCT * pRegionInfo )
{
	*pRegionInfo = RegionInfo; 

	/* To satisfy dregion.c
	 */
	return(0);
}

#define FULL_SORT 4
int nNoSortAppHint;
/**************************************************************************
 * Function Name  : PVRDCreateRenderContext
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    :	PVRDirect routine to create a render context. It
 *					initialises different variables to do with tile management,
 *					sets up the viewport, creates a passive device and
 *					adds pre render callback routine.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDCreateRenderContext(PPVR_RENDERCONTEXT pRenderContext)

{
	PVROSERR	Error = PVROS_GROOVY;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDCreateRenderContext() call.\n"));

	/* Create a logical device.
	 *
	pRenderContext->hLogicalDevice = PVROSCreateLogicalDevice(	pRenderContext->hDevice,
								 								pRenderContext->hTextureHeap);
	*/

	/* Check that a logical device was created.
	 */
	if (pRenderContext->hLogicalDevice != NULL)
	{
		if (pRenderContext->hDevice != NULL)
		{
			/* set the logical device in sglmid
			 */
			PVRDSetGlobal(SGL_LogicalDev, pRenderContext->hLogicalDevice);

			/* Call create device. Double buffer and device mode are invalid with a passive
			 * device. Hence pass dummy values tmp0 and tmp1.
			 */
			SetRegionInfo((int *)&pRenderContext->dwWidth, (int *)&pRenderContext->dwHeight);

	#if DAG_TRANS_SORTING
			/* What sorting algorithm is to be used, 
			 * The default is full sorting ON. Read default section first
			 * and if not set in default section check for application hint.
			 * Done now, incase memory allocation in InitRegionDataL fails
			 * and we can default to the old way of sorting.
			 */
			bFullSort = HWRdValFileInt("FullSort", TRUE);
			
			/* Read the no sort setting if present.	*/
			nNoSortAppHint = HWRdValFileUInt ("SGLTRANSSORT", 0); 
			if(nNoSortAppHint)
			{
				if(nNoSortAppHint == FULL_SORT)
				{
					/* force full sort on */
					bFullSort = TRUE;
				}
				else
				{
					/* force full sort off for NO_SORT, REVERSED_NO_SORT and MINZ_SORT */
					bFullSort = FALSE;
				}
			}
	#endif
  
			/* Initialise region stuff.
			 */
			InitRegionDataL();

			/* Read the near Z application hint.
			 */
			fMinInvZ = HWRdValFileFloat("Near Z Clip", fMinInvZ);

			/* Get custom offset value
			*/
			fAddToXY = HWRdValFileFloat ("AddToXY", fAddToXY);

			/* Read the maximum allowable translucent passes per tile. Read default section first
			 * and then read the app hint section.
			 */
			nMaxPassCount = HWRdValFileUInt ("MaxNumPasses", 0x0FFFFFFF);

			/* Read the dithering setting if present.
			 */
			uDithering = HWRdValFileUInt ("Dithering", uDithering);

#if DAG_TRANS_SORTING
			/* Set the global used for deciding which sort */
			bUseFullSort = bFullSort;
			if (bUseFullSort == FALSE)
			{
				/* Free the memory that we won't be using */
				CloseRegionDataL();
			}
#endif
#if PCX2
			/* Do we use the full masking plane or not. The full masking plane
			 * is required to prevent sparkles in D3D applications when the
			 * masking plane is being used. This is due to a bug in the hardware.
			 * Should be fixed in PCX2-003. This has a side effect of some
			 * applications running slower.
			 * The default is with the minimum masking plane.
			 */
			bFullMaskingPlane = (sgl_bool) HWRdValFileUInt("FullMaskingPlane", FALSE);

			/* Do we allow the user to set the colour of the masking plane.
			 */
			bSetMaskingBGColour = (sgl_bool) HWRdValFileUInt("SetMaskingBGColour", FALSE);
#endif
		}
		else
		{
			/* Error creating the callback.
			 */
			DPF((DBG_MESSAGE, "Couldn't create logical device.\n"));
			Error = PVROS_DODGY;
		}
	}
	else
	{
		/* No logical device available.
		 */
		DPF((DBG_MESSAGE, "No logical device available.\n"));
		Error = PVROS_DODGY;
	}

	/* Return the error condition.
	 */
	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDDestroyRenderContext
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    :	PVRDirect routine to destroy the specified context. It
 *					releases the pre render callback function and frees memory.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDDestroyRenderContext(PPVR_RENDERCONTEXT pRenderContext)
{
	PVROSERR	Error = PVROS_GROOVY;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDDestroyRenderContext() call.\n"));

	/* Check that a proper render context structure is passed.
	 */
	if (pRenderContext != NULL)
	{
		/* Destroy the logical device.
		 */
		//PVROSDestroyLogicalDevice(pRenderContext->hLogicalDevice);
	}
	else
	{
		/* the given name is invalid
		 */
		Error = PVROS_DODGY;
	}

	/* Return the error condition.
	 */
	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDStartFrame
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    : PVRDirect start of frame call.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDStartFrame(PPVR_RENDERCONTEXT pRenderContext)

{
	PVROSERR			Error = PVROS_GROOVY;
	SGLCONTEXT			*pContext = &pRenderContext->SGLContext;
	REGIONS_RECT_STRUCT	RegionRect;
	sgl_int32  			BackGroundStart;
	sgl_uint32			TSPBackgroundAddress = 0;

#if PCX2 || PCX2_003
	sgl_texture_filter_type		eFilterType;
#endif

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDStartFrame() call.\n"));

	/* If the render context has a valid Y region size - use it
	 */
	if(pRenderContext->RegionYHeight == 16 || pRenderContext->RegionYHeight == 32 || 
	   pRenderContext->RegionYHeight == 64 )
	{
		RegionInfo.YSize = pRenderContext->RegionYHeight;

		/*
		** Calculate how many regions there are
		*/
		RegionInfo.NumYRegions = 
			(pRenderContext->dwHeight + RegionInfo.YSize - 1) / RegionInfo.YSize;

		/*
		** Check that we don't exceed the max number of regions we
		** can handle
		*/
		RegionInfo.NumYRegions = MIN(RegionInfo.NumYRegions, MAX_Y_REGIONS);

		/*
		** See if there is a small portion left over for y.
		*/
		RegionInfo.LeftOverY =	pRenderContext->dwHeight - 
			((RegionInfo.NumYRegions-1)*RegionInfo.YSize);
		RegionInfo.HasLeftOver = (RegionInfo.LeftOverY!=0);
		

		/* Reset the region lists structures to be empty
		 */
		ResetRegionDataL (TRUE);
	}
	else
	{
		/* Reset the region lists structures to be empty
		 */
		ResetRegionDataL (FALSE);
	}   

	/* Get parameter memory, if available...
	 * Make call directly to the VXD.
	 */
	if (PVROSAssignVirtualBuffers(PVRParamBuffs, gHLogicalDev) == PVROS_GROOVY)
	{
		/* Add some "special" objects direct to the parameter store.
		 */
		AddDummyPlanesL(FALSE);

		RegionRect.FirstXRegion	= 0;
		RegionRect.FirstYRegion	= 0;
		RegionRect.LastXRegion	= RegionInfo.NumXRegions - 1;
		RegionRect.LastYRegion	= RegionInfo.NumYRegions - 1;

		BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;


	#if PCX2 || PCX2_003
		/* Masking support. Only available with PCX2.
		 */
		if (pContext->u32Flags & SGLTT_TRANSBACKGROUND)
		{
			/**************************************************
			 * When masking support is enabled fast fog must be
			 * disabled. Currently the assumption is that fast
			 * fogging will be disabled in the sglhw.ini or in
			 * the registry settings.
			 *
			 * This is only relevant to PCX2 drivers (ie sglmid5)
			 * since PCX1 drivers never have fast fog even when
			 * using PCX2 hardware.
			 **************************************************/

			/* Pack a textured plane for the masking plane. The routine
			 * PackTexasMask() automatically sets the TSP tag for the
			 * masking plane to 2. ie no need to save and restore TSP tags.
			 */
			TSPBackgroundAddress = PackTexasMask (pContext->cBackgroundColour,
												 FALSE, 
												 FALSE);
		}
		else
	#endif	/* #if PCX2	*/
		{
			/* Masking not required.
			 */
			TSPBackgroundAddress = PackTexasFlat(pContext->cBackgroundColour, 
												pContext->bFogOn, 
												pContext->eShadowLightVolMode);
		}

		/* Add the background plane. The TSP tag used is either a flat
		 * textured plane or a full textured masking plane.
		 */
	#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackBackgroundPlane (TSPBackgroundAddress, 0.0f);
	#else
		PackBackgroundPlane (TSPBackgroundAddress, 0);
	#endif
		AddRegionOpaqueL (&RegionRect, BackGroundStart, 1);


		/* Add the opaque pass flushing plane. Use a seperate flat textured
		 * plane since if D3D a full textured plane may be specified in
		 * TSPBackgroundAddress.
		 */
		BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackBackgroundPlane ( PackTexasFlat(pContext->cBackgroundColour, 
												FALSE, FALSE), -1.0f);
#else
		PackBackgroundPlane ( PackTexasFlat(pContext->cBackgroundColour, 
												FALSE, FALSE), -64);
#endif
		AddFlushingPlaneL (BackGroundStart);

			
		/* Add translucent flushing plane.
		 */
		BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackBackgroundPlane (PackTexasTransparent (FALSE), -1.0f);
#else
		PackBackgroundPlane (PackTexasTransparent (FALSE), -64);
#endif
		AddTransFlushingPlaneL (BackGroundStart);

		pContext->FirstXRegion = RegionRect.FirstXRegion;
		pContext->FirstYRegion = RegionRect.FirstYRegion;
		pContext->LastXRegion = RegionRect.LastXRegion;
		pContext->LastYRegion = RegionRect.LastYRegion;

		/* Used to calculate the bounding box for shadow and light volumes.
		 */
		pContext->invRegX = 1.0f / RegionInfo.XSize;
		pContext->invRegY = 1.0f / RegionInfo.YSize;

		if (pContext->eShadowLightVolMode == ENABLE_LIGHTVOLS)
		{
			AllowLightVolAdditionL (&RegionRect);
		}

		/* Set bogus Z to 0.
		 */
		gfBogusInvZ = 0.0f;

	#if PCX2
		/* For PCX2-002 the filter selection is completely determined by
		 * eFilterType - i.e. the SGLTT_BILINEAR flag is ignored.
		 */
		bFilteringOverRide	=  PVROSFilteringOveride();
		bBilinearEnabled	=  PVROSBilinearEnabled();

		eFilterType = pContext->eFilterType;

		if (!bFilteringOverRide)
		{
			/* We haven't overriden the application settings so continue.
			 * Bilinear enabled or not ????
			 */
			bBilinearEnabled = (eFilterType == sgl_tf_point_sample) ? FALSE : TRUE;
			HWSetBilinearRegister(eFilterType);
		}
	#endif

	#if DAG_TRANS_SORTING
		/* Decision sorting method based on the number of translucent triangles. */

		if (bFullSort)
		{
			if (guTransTriCounter > FULL_SORT_FINAL_CUTOFF)
			{
				/* New sorting no more. */
				bUseFullSort = FALSE;
				bFullSort = FALSE;
				CloseRegionDataL();
			}
			else
			{
				bUseFullSort = TRUE;
			}
		}

		/* Reset Translucent triangle count */

		guTransTriCounter = 0;

		/* no sort method */
		if(!nNoSortAppHint)
		{
			if(pContext->u32Flags & SGLTT_SELECTTRANSSORT)
			{
				gNoSortTransFaces = (int)pContext->eTransSortMethod;
				if(gNoSortTransFaces && gNoSortTransFaces != FULL_SORT)
				{
				   	/* force full sort off for NO_SORT, REVERSED_NO_SORT and MINZ_SORT */
					bUseFullSort = FALSE;
					bFullSort = FALSE;
					CloseRegionDataL();
				}
			}
		}
		else
		{
			gNoSortTransFaces = nNoSortAppHint;
		}	
	#endif
	}
	else
	{
		/* No logical device available.
		 */
		DPF((DBG_MESSAGE, "No buffers avaiable.\n"));
		Error = PVROS_DODGY;
	}

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDTriangles
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 *				 	sgl_uint32				dwNumFaces
 *					int					pFaces[][3]
 *					PSGLVERTEX			pVertices
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    : PVRDirect interface to draw triangles.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDTriangles(	PPVR_RENDERCONTEXT	pRenderContext,
									sgl_uint32			dwNumFaces,
									int 				pFaces[][3],
									PSGLVERTEX			pVertices)

{
	PVROSERR Error = PVROS_GROOVY;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDTriangles() call.\n"));

	/* bFullSort selects which sorting algorithm to use.
	 */
	DirectD3DPolygons(	pRenderContext->hTextureHeap, &pRenderContext->SGLContext, 
						dwNumFaces, pFaces, pVertices, bUseFullSort );

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDTrianglesPrimitive
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 *				 	sgl_uint32			dwNumFaces
 *					int					pFaces[][3]
 *					PSGLVERTEX			pVertices
 *					PRIMITIVETYPE		PrimitiveType
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    : PVRDirect interface to DrawPrimitive and DrawIndexPrimitive
 *					triangles.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDTrianglesPrimitive(	PPVR_RENDERCONTEXT	pRenderContext,
									sgl_uint32			dwNumFaces,
									sgl_uint16			*pFaces,
									PSGLVERTEX			pVertices,
									PRIMITIVETYPE		PrimitiveType)

{
	PVROSERR Error = PVROS_GROOVY;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDTrianglesPrimitive() call.\n"));

	DirectTriPrimitive (pRenderContext->hTextureHeap, &pRenderContext->SGLContext, 
						dwNumFaces, pFaces, pVertices, bUseFullSort, PrimitiveType);

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDLines
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 *				 	sgl_uint32			dwNumLines
 *					PSGLVERTEX			pVertices
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    : PVRDirect interface to draw lines.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDLines(	PPVR_RENDERCONTEXT	pRenderContext,
								sgl_uint32			dwNumLines,
								sgl_uint16			pLines[][2],
								PSGLVERTEX			pVertices,
								PRIMITIVETYPE		PrimitiveType)

{
	PVROSERR	Error = PVROS_GROOVY;
	sgl_uint32	nNextLineInc;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDLines() call.\n"));

	/* Have we been called from D3D Execute buffer or DrawPrimitive.
	 */
	switch(PrimitiveType)
	{
		/* Need to determine the type of DrawPrimitive triangle.
		 */
	case LINELIST:
		/* Line lists.
		 * The line index count here is 1.
		 *
		 * L1 = V1, V2
		 * L2 = V3, V4
		 * .
		 * Ln = V2n-1, V2n
		 *
		 * After processing line n the line index count
		 * is 2n. The first vertex for Ln+1 is V2(n+1)-1
		 * which equals 2n+1. Therefore the line index increment
		 * is 1.
		 */
		nNextLineInc = 1;
		break;

	case LINESTRIP:
		/* Line strips.
		 * The line index count here is 0.
		 *
		 * L1 = V1, V2
		 * L2 = V2, V3
		 * .
		 * Ln = Vn, Vn+1
		 *
		 * After processing line n the line index count
		 * is n+1. The first vertex for Ln+1 is V(n+1)
		 * Therefore the face index increment is 0.
		 */
		nNextLineInc = 0;
		break;

	default:
		/* Default to normal line mode. ie D3D execute buffer.
		 */
		nNextLineInc = 1;
		break;
	}

	/* Call the low level lines function.
	 */
	DirectLines(&pRenderContext->SGLContext, dwNumLines, pLines, pVertices, nNextLineInc);

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDPoints
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 *				 	sgl_uint32			dwNumPoints
 *					PSGLVERTEX			pVertices
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    : PVRDirect interface to draw points.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDPoints(	PPVR_RENDERCONTEXT	pRenderContext,
				 				sgl_uint32			dwNumPoints,
								sgl_uint16			*pPoints,
								PSGLVERTEX			pVertices)

{
	PVROSERR Error = PVROS_GROOVY;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDPoints() call.\n"));

	/* Call with the a vertex and point list. The point list will only
	 * be used in DrawIndexPrimitive.
	 */
	DirectPoints (&pRenderContext->SGLContext, dwNumPoints, pPoints, pVertices);

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDShadows
 * Inputs         : PPVR_RENDERCONTEXT	pRenderContext
 *				   	sgl_uint32			nNumFaces,
 *				   	sgl_uint32			pFaces[][3],
 *			 	   	PSGLVERTEX			pVertices,
 *				   	float				fBoundingBox[2][2]
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    : PVRDirect interface to draw shadows.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDShadows(	PPVR_RENDERCONTEXT	pRenderContext,
							   	int					nNumFaces,
							   	int					pFaces[][3],
						 	   	PSGLVERTEX			pVertices,
							   	float				fBoundingBox[2][2] )
{
	PVROSERR Error = PVROS_GROOVY;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDShadows() call.\n"));

	DirectShadows(	&pRenderContext->SGLContext,
					nNumFaces,
					pFaces,
					pVertices,
					fBoundingBox);

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDPreRender
 * Inputs         : 
 * Outputs        : 
 * Returns        : Error
 * Global Used    : 
 * Description    :
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDPreRender(PPVR_RENDERCONTEXT pRenderContext)

{
	PVROSERR				Error = PVROS_GROOVY;
	SGLCONTEXT				*pContext = &pRenderContext->SGLContext;
	REGIONS_RECT_STRUCT		RegionRect;
	sgl_bool				bRenderAllRegions;
#if DEBUG
	static int snDrawGrid = -1;
#endif

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDPreRender() call.\n"));

#if DEBUG
	if(snDrawGrid == -1)
	{
		snDrawGrid = SglReadPrivateProfileInt ("DEBUG", "RegionGrid", 0, "sglhw.ini");
	}
	if(snDrawGrid == 1)
	{
		DrawPVRDRegionGrid(pRenderContext);
	}
#endif
	/* Setup the region information.
	 */
	RegionRect.FirstXRegion	= pContext->FirstXRegion;
	RegionRect.FirstYRegion = pContext->FirstYRegion;
	RegionRect.LastXRegion	= pContext->LastXRegion;
	RegionRect.LastYRegion	= pContext->LastYRegion;

	/* D3D decides whether or not to render all the regions.
	 */
	if (pContext->RenderRegions == DONT_RENDER_EMPTY_REGIONS)
	{
		bRenderAllRegions = FALSE;
	}
	else
	{
		bRenderAllRegions = TRUE;
	}

	GenerateObjectPtrLite( &RegionRect, bRenderAllRegions );

	/* Convert the supplied fog colour ... providing it's valid,
	 * u32FogDensity is an unsigned int, so it's always >= 0
	 */
	if( (pContext->u32FogDensity<=31) &&
		(pContext->fFogR >= 0.0f) &&
		(pContext->fFogR <= 1.0f) &&
		(pContext->fFogG >= 0.0f) &&
		(pContext->fFogG <= 1.0f) &&
		(pContext->fFogB >= 0.0f) &&
		(pContext->fFogB <= 1.0f))
	{
		sgl_map_pixel  fogColour;

		/* Set the fog virtual register directly.
		 */
		gHLogicalDev->Registers[PCX_FOGAMOUNT] = pContext->u32FogDensity;	

		fogColour.red	 =	(unsigned char) (pContext->fFogR * 255);
		fogColour.green  =	(unsigned char) (pContext->fFogG * 255);
		fogColour.blue	 =	(unsigned char) (pContext->fFogB * 255);
		fogColour.alpha  =	0;

		TexasSetFogColour (fogColour);
	}
	/* Else set the least dense, black fog
	 */
	else
	{
		sgl_map_pixel  fogColour;

		/* Set the fog virtual register directly.
		 */
		gHLogicalDev->Registers[PCX_FOGAMOUNT] = 0;

		fogColour.red	 =	0;
		fogColour.green  =	0;
		fogColour.blue	 =	0;
		fogColour.alpha  =	0;

		TexasSetFogColour (fogColour);
	}	

	/*  write the scale into the virtual register directly.
	 */
	gHLogicalDev->Registers[PCX_CAMERA] = MAX_CFR_VALUE;

	/* Set the X clip value. If (nDeviceX + 1) is more than 1023,
	   fix it to be 1023 as there are only 10 bits in the register
	   for the right clip value. Left clipping has been disabled.
	 */
	
	gHLogicalDev->Registers[PCX_XCLIP] = 
		((1 << 28) | (((nDeviceX > 1022) ? 1023 : nDeviceX+1) << 16));

	/* Set the pixel bit depth and whether dithering is enabled or not.
	 * If the uDithering value is 0x2 then we don't want to override
	 * the dithering setting.
	 */
	if (uDithering == 0x2)
	{
		if (pContext->u32Flags & SGLTT_DISABLEDITHERING)
		{
			/* Disable dithering.
			 */
			PVROSSetPCIPixelFormat ((sgl_uint16) pRenderContext->RenderSurfFormat.BitDepth,
									FALSE);
		}
		else
		{
			/* Enable dithering.
			 */
			PVROSSetPCIPixelFormat ((sgl_uint16) pRenderContext->RenderSurfFormat.BitDepth,
									TRUE);
		}
	}
	else
	{
		/* Set dithering based on uDithering field.
		 */
		PVROSSetPCIPixelFormat ((sgl_uint16) pRenderContext->RenderSurfFormat.BitDepth,
								(sgl_bool) uDithering);
	}
	
	/* Program virtual SOF register directly.
	 */
	gHLogicalDev->Registers[PCX_SOFADDR] = pRenderContext->dwPhysRenderAddress;

	/* Program virtual stride register directly.
	 */
	gHLogicalDev->Registers[PCX_LSTRIDE] = pRenderContext->lStride;

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDRender
 * Inputs         : PPVR_RENDERCONTEXT pRenderContext
 * Outputs        : 
 * Returns        : Error
 * Global Used    : pEndOfRender, end of render status
 * Description    :	PVRDirect interface to the rendering routine.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDRender(PPVR_RENDERCONTEXT	pRenderContext)

{
	PVROSERR Error = PVROS_GROOVY;

	#if DUMP_SABRE_AND_TEXAS
		DumpSabreAndTexas();
	#endif
	/* Start render.
	 */
	Error = PVROSScheduleRender(gHLogicalDev);

	DPF((DBG_MESSAGE, "Done HWStartRender !!!!"));

	return(Error);
}

/**************************************************************************
 * Function Name  : PVRDRenderStrip
 * Inputs         : PPVR_RENDERCONTEXT pRenderContext
 *					sgl_uint32 lStrip
 * Outputs        : 
 * Returns        : Error
 * Global Used    : pEndOfRender, end of render status
 * Description    :	PVRDirect interface to the strip rendering routine.
 *
 **************************************************************************/

PVROSERR CALL_CONV PVRDRenderStrip(	PPVR_RENDERCONTEXT pRenderContext,
									int nStrip,
									int nXExtent[2])

{
	PVROSERR						Error = PVROS_GROOVY;
	SGLCONTEXT						*pContext = &pRenderContext->SGLContext;
	REGIONS_RECT_STRUCT				RegionRect;
	sgl_bool						bRenderAllRegions;

	/* Debug message.
	 */
	DPF((DBG_MESSAGE, "PVRDRenderStrip() call.\n"));
	
	RegionRect.FirstXRegion = pContext->FirstXRegion;
	RegionRect.FirstYRegion = pContext->FirstYRegion;
	RegionRect.LastXRegion = pContext->LastXRegion;
	RegionRect.LastYRegion = pContext->LastYRegion;
	
	if (nStrip == -2)
	{
		/* End of strip render. decrement in use count, free the buffer
		*/
		gHLogicalDev->VirtualBuffer->bInUse--;
	}
	else if (nStrip == -1)
	{
		/*
		// If we are called by Direct3D and there is no fog then render only those
		// regions that contain objects, else render all regions in the device.
		*/
		if (pContext->RenderRegions == DONT_RENDER_EMPTY_REGIONS)
		{
			bRenderAllRegions = FALSE;
		}
		else
		{
			bRenderAllRegions = TRUE;
		}

		GenerateObjectPtrLiteStrip (&RegionRect, bRenderAllRegions);

		/*
		// Convert the supplied fog colour ... providing it's valid
		*/
		if(	(pContext->u32FogDensity<=31) &&
			(pContext->fFogR >= 0.0f) &&
			(pContext->fFogR <= 1.0f) &&
			(pContext->fFogG >= 0.0f) &&
			(pContext->fFogG <= 1.0f) &&
			(pContext->fFogB >= 0.0f) &&
			(pContext->fFogB <= 1.0f))
		{
			sgl_map_pixel  fogColour;

			/* Set the fog virtual register directly.
			 */
			gHLogicalDev->Registers[PCX_FOGAMOUNT] = pContext->u32FogDensity;	

			fogColour.red	 =	(unsigned char) (pContext->fFogR * 255);
			fogColour.green  =	(unsigned char) (pContext->fFogG * 255);
			fogColour.blue	 =	(unsigned char) (pContext->fFogB * 255);
			fogColour.alpha  =	0;

			TexasSetFogColour (fogColour);
		}
		/*
		// Else set the least dense, black fog
		*/
		else
		{
			sgl_map_pixel  fogColour;

			/* Set the fog virtual register directly.
			 */
			gHLogicalDev->Registers[PCX_FOGAMOUNT] = 0;	

			fogColour.red	 =	0;
			fogColour.green  =	0;
			fogColour.blue	 =	0;
			fogColour.alpha  =	0;
	
			TexasSetFogColour (fogColour);
		}	

		/*  write the scale into the virtual register directly.
		 */
		gHLogicalDev->Registers[PCX_CAMERA] = MAX_CFR_VALUE;

		/* Set the X clip value.
		 */
		gHLogicalDev->Registers[PCX_XCLIP] = 
			((1 << 28) | (((nDeviceX > 1022) ? 1023 : nDeviceX+1) << 16));	

		/* Set the pixel bit depth and whether dithering is enabled or not.
		 */
		if (pContext->u32Flags & SGLTT_DISABLEDITHERING)
		{
			/* Disable dithering.
			 */
			PVROSSetPCIPixelFormat ((sgl_uint16) pRenderContext->RenderSurfFormat.BitDepth,
									FALSE);
		}
		else
		{
			/* Enable dithering.
			 */
			PVROSSetPCIPixelFormat ((sgl_uint16) pRenderContext->RenderSurfFormat.BitDepth,
									TRUE);
		}
	}
	else
	{
		REGION_STRIP_DATA	RegionStrip; 
		REGION_STRIP_DATA	*pRegionStrip = &RegionStrip;
		sgl_uint32 			dwSOFScanlineOffset;

		pRegionStrip->fObjectsPresent = FALSE;

		SetupStripLite (nStrip, &RegionRect, pRegionStrip);

		if (pRegionStrip->fObjectsPresent)
		{
			sgl_uint32 ObjectOffset;

			/*
			// Set up the hardware registers:
			*/

			dwSOFScanlineOffset = RegionInfo.YSize * pRenderContext->lStride * nStrip;

			/* Program virtual SOF register directly.
			 */
			gHLogicalDev->Registers[PCX_SOFADDR] = pRenderContext->dwPhysRenderAddress -
												   dwSOFScanlineOffset;

			/* Program virtual stride register directly.
			 */
			gHLogicalDev->Registers[PCX_LSTRIDE] = pRenderContext->lStride;

			/* save current reg and add offset onto current object offset address
			 */
			ObjectOffset = gHLogicalDev->Registers[PCX_OBJECT_OFFSET]; 
			gHLogicalDev->Registers[PCX_OBJECT_OFFSET] = ObjectOffset + pRegionStrip->FirstObjectOffset; 

			/* increment in use count so we don't lose the buffer; vxd will decrement
			*/
			gHLogicalDev->VirtualBuffer->bInUse++;

			/* Schedule the hardware render.
			 */
			Error = PVROSScheduleRender(gHLogicalDev);

			/* restore object reg
			 */
			gHLogicalDev->Registers[PCX_OBJECT_OFFSET] = ObjectOffset;

			/* Fill out the extent business.
			 */
			nXExtent[0] = pRegionStrip->nXExtents[0];
			nXExtent[1] = pRegionStrip->nXExtents[1];
		}
	}

	/* Set the return to dodgy since it doesn't work with groovy.
	 * This needs to be fixed in the hal.
	 */
	Error = PVROS_DODGY;

	return(Error);
}

/**************************************************************************
 * Function Name  : DumpSabreAndTexas ()
 * Inputs         : 
 * Input/Outputs  : 
 * Returns        : 
 * Global Used    : gHLogicalDev
 * Description    : Dumps the contents of the parameter memory to a file.
 *	
 **************************************************************************/

#if DUMP_SABRE_AND_TEXAS

#define TIMED_CAPTURE			FALSE
#define CAPTURE_RATE			40
#define TEXPARAMS_ONLY			FALSE
#define OUTPUT_LITTLE_END		0
#define COMPRESSOR				FALSE

#if TIMED_CAPTURE
	/* Write parameters to the desktop.
	 */
	#define DUMP_PATH "c:\\Windows\\Desktop\\"
#else
	/* Set up a mapped drive. This is used to initiate the parameter dump
	 * and also the parameters are written here.
	 */
	#define DUMP_PATH "e:\\"
#endif

void DumpSabreAndTexas(void)
{
	FILE *fp;
	FILE *fp2;
	sgl_uint32 *pISP, *pTSP, *pObj;
	sgl_uint32 i, PlanesLimit, TexasLimit, RegionLimit, Val;
   	char fileEnding[100]="\0", paramFile[100]="\0", InputFile[100]="\0", info[100]="\0";
	char path[] = DUMP_PATH;
	static int frame;
	static int filenumber=0;
	int captureFrame;

	frame++;

	captureFrame=FALSE;

#if TIMED_CAPTURE
	if (frame==CAPTURE_RATE)
		captureFrame=TRUE;
#else
	strcpy(InputFile, DUMP_PATH);
	strcat(InputFile, "capstat.txt");

	if( (fp = fopen(InputFile, "r"))==NULL);
	else if(fgetc(fp)!=0)
	{
		captureFrame=TRUE;
		fclose(fp);
		fp = fopen(InputFile, "w");
		fputc(0, fp);
		fclose(fp);
	}
	else
	{
		fclose(fp);
	}
#endif

	if(captureFrame)
	{
#if COMPRESSOR
		/* wait until the compressor is ready */
		fp = fopen("e:\\compstat.txt", "r");
		while(fgetc(fp)==1)
	  	{
			fclose(fp);
			PVROSDelay(PVR_DELAY_MS, 100);
	  		fp = fopen("e:\\compstat.txt", "r");
	 	}
		fclose(fp);
#endif

		frame=0;
		filenumber++;

		itoa(filenumber, fileEnding, 10);
    	
    	/* set limits */
		PlanesLimit = gHLogicalDev->Buffers[0].uBufferPos;
		TexasLimit	= 1024 * 1024; /*gHLogicalDev->Buffers[1].uBufferLimit;*/
		RegionLimit = gHLogicalDev->Buffers[2].uBufferPos;
		
		pISP = gHLogicalDev->Buffers[0].pBuffer;
		pTSP = gHLogicalDev->TexHeap->pTextureMemory;/*gHLogicalDev->Buffers[1].pBuffer;*/
		pObj = gHLogicalDev->Buffers[2].pBuffer;

		/* open info file */	 
		strcat(info, path);
		strcat(info, "info");
		strcat(info, fileEnding);
		strcat(info, ".txt");
		fp2 = fopen(info, "w");

		/* open sabre file */
		strcat(paramFile, path);
		strcat(paramFile, "sab");
		strcat(paramFile, fileEnding);
		//strcat(paramFile, ".Z");
		fp = fopen(paramFile, "wb");
	 
		if( (fp!=NULL) && (fp2!=NULL) )
		{	
			/* dump plane data */

			for(i = 0; i <= PlanesLimit; i++, pISP++)
			{
				Val = *pISP;
		
				#if OUTPUT_LITTLE_END
					fputc(Val & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 24 & 0xff, fp);
					#else
					fputc(Val >> 24 & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val & 0xff, fp);
				#endif		
			}	  

			/* output address of plane/tile division pointer in 32 bit words */
			fprintf(fp2, "%d", i);
	
			/* dump region data (NOTE region data is NOT in pParamBlock for PCX1) */ 
		 
			for(i = 0; i <= RegionLimit; i++, pObj++)
			{
				Val = *pObj;
		
				#if OUTPUT_LITTLE_END
					fputc(Val & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 24 & 0xff, fp);
				#else
					fputc(Val >> 24 & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val & 0xff, fp);
				#endif		
			} 

			fclose(fp);
			fclose(fp2);
		}
		else
			fprintf(stderr, "Couldn't write sabre file");
			  
		/* open Texas files */ 
		strcpy(paramFile, path);
		strcat(paramFile, "tex");
		strcat(paramFile, fileEnding);
		//strcat(paramFile, ".Z");
		fp = fopen(paramFile, "wb");
	
		if(fp!=NULL)
		{
		
		/* we may only want the texels on the first frame */
		#if TEXPARAMS_ONLY==TRUE
		if(filenumber != 1)
			TexasLimit = 0;	
		#endif
			
			for(i = 0; i<TexasLimit; i++, pTSP++)
			{		
				Val = *pTSP; 
  	
			#if OUTPUT_LITTLE_END
				fputc(Val & 0xff, fp);
				fputc(Val >> 8  & 0xff, fp);
				fputc(Val >> 16 & 0xff, fp);
				fputc(Val >> 24 & 0xff, fp);
			#else
				fputc(Val >> 24 & 0xff, fp);
				fputc(Val >> 16 & 0xff, fp);
				fputc(Val >> 8  & 0xff, fp);
				fputc(Val & 0xff, fp);
			#endif
			}

			fclose(fp);
		}
		else
		{
			fprintf(stderr, "Couldn't write texas file");
		}
		fclose(fp);	 

#if COMPRESSOR
		/* indicate to compressor that ready */
		fp = fopen("e:\\compstat.txt", "w");
		fputc(1, fp);
		fclose(fp);

#endif
	} /* if (captureFrame) */

} /* DumpSabreAndTexas */

#endif /* #if DUMP_SABRE_AND_TEXAS */


/* end of $RCSfile: pvrd.c,v $ */

