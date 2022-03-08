/**************************************************************************
 * Name         : sgllite.c
 * Title        : SGL Lite (SGL Lite version of rnrender.c)
 * Author       : Graham Connor
 * Created      : 03 February 1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Renders the given scene, traversing the database etc.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: sgllite.c,v $
 * Revision 1.89  1997/11/28  15:27:04  sxy
 * added two App hints "OutputToConsole" and "FramesToAverage", added a yellow
 * bar for total frame time in developer and timing builds.
 *
 * Revision 1.88  1997/11/26  09:56:07  sxy
 * changed the way to read "StateBar" to HWRdValFileUInt().
 *
 * Revision 1.87  1997/11/24  11:14:58  sxy
 * made fTotalSWTimeInFrame and fTotalHWTimeInFrame extern.
 *
 * Revision 1.86  1997/11/24  10:43:45  sxy
 * added state bar for developer and timing builds.
 *
 * Revision 1.85  1997/10/31  14:56:32  sxy
 * changed the App Hint name for no sort to SGLTRANSSORT.
 *
 * Revision 1.84  1997/10/31  10:56:43  sxy
 * added an App hint "NoSort" for no sort trans face lists, 1 for no sort,
 * 2 for reversed no sort.
 *
 * Revision 1.83  1997/10/30  17:37:17  sxy
 * added a global gNoSortTransFaces for no sort trans face lists.
 *
 * Revision 1.82  1997/10/24  15:41:22  sxy
 * changed timing unit into millisecond.
 *
 * Revision 1.81  1997/10/22  17:12:15  sxy
 * added timing metrics for developer's driver.
 *
 * Revision 1.80  1997/10/17  08:42:04  erf
 * Added app hint dithering override control.
 *
 * Revision 1.79  1997/10/16  13:44:25  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.78  1997/10/15  09:00:40  sxy
 * changed some DPFDEV messages.
 *
 * Revision 1.77  1997/10/14  16:50:14  sxy
 * read from sgl.ini for debug level if in DEBUGDEV.
 *
 * Revision 1.76  1997/09/25  11:52:37  sxy
 * changed debug level to MESSAGE for DEBUGDEV, and added some DPFDEV messages.
 *
 * Revision 1.75  1997/09/24  15:59:24  jrc
 * API developers need not use app hint, but do need to set
 * SGLTT_TRANSBACKGROUND.  (They need to call register function to enable
 * this.)
 *
 * Revision 1.74  1997/09/24  14:20:53  gdc
 * added stuff for API developers
 *
 * Revision 1.73  1997/09/24  11:02:41  gdc
 * rationalised some SglReadPrivateProfile and GetAppHint stuff to read
 * using the new HWRdValFileXXX functions which read in a defined order
 *
 * Revision 1.72  1997/09/24  10:12:30  sxy
 * Changed some DPF to DPFDEV.
 *
 * Revision 1.71  1997/09/10  17:20:59  erf
 * Added control to enable\disable dithering using the flag
 * SGLTT_DISABLEDITHERING. Dithering is enabled by default.
 *
 * Revision 1.70  1997/08/28  15:41:14  gdc
 * added bits to draw a grid of lines at region edges in debug drivers
 * cotrolled by a DEBUG section setting called "RegionGrid"
 *
 * Revision 1.69  1997/08/28  13:50:52  gdc
 * added stuff to follow which context flags are used
 *
 * Revision 1.68  1997/08/26  16:36:29  erf
 * Add support for masking plane based on a application hint. As default
 * no masking plane, since no support for PCX1.
 *
 * Revision 1.67  1997/08/08  19:45:03  mjg
 * Removed global bilinear option for PCX2_003
 *
 * Revision 1.66  1997/08/05  16:10:35  erf
 * Added facility to specify the maximum allowable translucent passes.
 *
 * Revision 1.65  1997/07/28  21:35:14  mjg
 * Added a '|' to one of the pre-processor compile switches,
 *
 * Revision 1.64  1997/07/28  10:34:20  gdc
 * removed windows.h include, and tweaked code slightly to accomodate this
 *
 * Revision 1.63  1997/07/24  17:05:07  gdc
 * changed sgltri_render etc so that a HDISPLAY is passed in to
 * HWStartRender and the render in waited for there before calling
 * 2-d callback
 *
 * Revision 1.62  1997/07/14  19:33:16  mjg
 * Added PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.61  1997/07/14  09:59:30  gdc
 * tried to turn off windows include file warnings and failed
 *
 * Revision 1.60  1997/06/23  08:38:23  erf
 * Removed all references to masking for PCX2 since it is not needed
 * for SGLDirect. Only required for D3D which now goes through the
 * PVRD.c route.
 *
 * Revision 1.59  1997/06/22  17:38:15  jop
 * Added adaptive TSP parameter stuff
 *
 * Revision 1.58  1997/06/11  16:34:39  mjg
 * Moved PreRenderCallback and register initialisation to HWStartRender.
 *
 * Revision 1.57  1997/06/11  12:34:31  jop
 * Reoprganised render and findished render calls
 *
 * Revision 1.56  1997/06/11  11:00:49  erf
 * Initialise fMinInvZ with INVZ_DEFAULT_SCALE_VALUE which is defined in
 * sgl.h .
 *
 * Revision 1.55  1997/06/09  09:15:58  gdc
 * got rid of some compiler warnings
 *
 * Revision 1.54  1997/05/30  16:36:35  mjg
 * Passed the logicaldevice handle to PVROSCallback, rather than NULL.
 *
 * Revision 1.53  1997/05/28  10:23:52  mjg
 * Added the function SaveDeviceName, to record the most recent display
 * device and put a check on the return value from the 2D callback.
 *
 * Revision 1.52  1997/05/27  17:17:02  erf
 * Added 2D callback to SGLDirect. Needed to include "dvdevice.h" for
 * node structure definition. Assumption is that SGLDirect only creates
 * one display device.
 *
 * Revision 1.51  1997/05/22  23:27:26  jop
 * Got rid of syntax error
 *
 * Revision 1.50  1997/05/21  09:43:32  jrc
 * Changed pixel offset to -0.5f for 1:1 texel:pixel mapping in bilinear.
 *
 * Revision 1.49  1997/05/21  09:37:44  mjg
 * Now the process is exited if the Virtual Buffers can't be assigned.
 *
 * Revision 1.48  1997/05/20  13:56:50  gdc
 * removed thread priority code
 *
 * Revision 1.47  1997/05/16  17:19:27  erf
 * Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
 *
 * Revision 1.46  1997/05/16  01:46:27  mjg
 * Tidy up for PCX1.
 *
 * Revision 1.45  1997/05/15  20:14:56  jop
 * Removed synchronisation stuff
 *
 * Revision 1.44  1997/05/14  17:13:52  gdc
 * HWSetSabPtrReg not needed for windows
 *
 * Revision 1.43  1997/05/14  03:41:40  jop
 * We'll fix strip rendering later, won't we
 *
 * Revision 1.42  1997/05/13  14:48:51  gdc
 * put in call to schedule render rather than copying params and
 * hwstart render
 *
 * Revision 1.41  1997/05/13  13:40:32  gdc
 * get param buff directly from call to PVROS
 *
 * Revision 1.40  1997/05/13  12:44:41  erf
 * Changed fAddToXY to -0.125f. Now we comply with Microsofts Top-Left fill
 * convention in Direct3D Drivers. To their example at least. More work is
 * needed.
 *
 * Revision 1.39  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.38  1997/04/30  19:22:06  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.37  1997/04/29  14:53:02  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.36  1997/04/25  17:36:25  mjg
 *  Removed more texturing functions.
 *
 * Revision 1.35  1997/04/25  09:58:05  gdc
 * hardware and param buff initialisations now done by create physical/logical
 * device for win32 builds
 *
 * Revision 1.34  1997/04/25  09:51:25  mjg
 * Removed Texture memory initialisation.
 *
 * Revision 1.33  1997/04/23  18:03:14  gdc
 * put bilinear control globals in here but they get set by asking PVROS
 *
 * Revision 1.32  1997/04/23  17:10:51  erf
 * 1. Changed operation of texture filtering for PCX2-002. The filter selection
 *    is now made at sgltri_startofframe().
 * 2. Changed fAddToXY back to -0.25f. It was changed in version 1.13 without
 *    comment.
 *
 * Revision 1.31  1997/04/22  17:41:21  mjg
 * Removed brdsetup.h include and changed a ghTexHeap pointer
 *
 * Revision 1.30  1997/04/21  20:01:57  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.29  1997/04/21  18:45:01  sxy
 * Added a global variable ghTexHeap for texture heap.
 *
 * Revision 1.28  1997/04/21  15:30:22  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.27  1997/04/16  10:35:15  gdc
 * added a version.h include
 *
 * Revision 1.26  1997/04/16  10:31:47  jop
 * Did something very dodgy in sgltri_isrendercomplete
 *
 * Revision 1.25  1997/04/15  13:15:51  mjg
 * Tidied up and started on Parameter Buffer handling.
 *
 * Revision 1.24  1997/04/10  19:55:06  mjg
 * Removed code that was redundant and giving compiler warnings.
 *
 * Revision 1.23  1997/04/09  17:09:55  gdc
 * removed refernce to project_mat global
 *
 * Revision 1.22  1997/04/08  11:54:02  jop
 * Set default trace output level to warning rather than message
 *
 * Revision 1.21  1997/04/04  11:05:56  gdc
 * removed all reference to current_trans_set_id from this module
 *
 * Revision 1.20  1997/04/03  17:55:18  mjg
 * Added an include.
 *
 * Revision 1.19  1997/04/03  13:14:19  mjg
 * Updated the calling convention.
 *
 * Revision 1.18  1997/04/01  19:18:00  mjg
 * Changed notprintf to PVROSPrintf.
 *
 * Revision 1.17  1997/03/31  23:41:20  mlj
 * Fixes to get DSO32 driver to build
 *
 * Revision 1.16  1997/03/31  22:10:15  jop
 * Sorted out WIN32/DOS32 issues
 * y
 *
 * Revision 1.15  1997/03/26  06:52:06  erf
 * Removed stuff which allows sglmid4.dll to use PCX2 functionality when
 * running on a PCX2.
 *
 * Revision 1.14  1997/03/25  12:24:35  erf
 * 1. Removed callback calls with #if 0 around them for Jim.
 * 2. Added texture filtering selection for SGL-Lite. ie bilinear etc..
 * 3. Removed use of u32Reserve9[] assignments. Not needed, I hope.
 *
 * Revision 1.13  1997/03/25  10:16:41  jop
 * Removed some board dependancies and updated to use generic callbacks
 *
 * Revision 1.12  1997/03/15  13:39:01  jop
 * Added global dec for MemoryRoot for sgllite builds
 * Revision 1.11  1997/03/14  13:04:19  jop
 * Fixed some // comments fletch put in
 *
 * Revision 1.10  1997/03/14  11:06:53  jwf
 * changed some stuff to make it compile on the macintosh
 *
 * Revision 1.9  1997/03/13  11:42:22  sxy
 * Added a global MemoryRoot, and pass it to InitTextureMemory().
 *
 * Revision 1.8  1997/03/11  08:31:05  erf
 * No need to save and restore TSP flags since PackTexasMask() already
 * does this.
 *
 * Revision 1.7  1997/03/10  21:03:37  erf
 * Disable fast fog planes if masking plane enabled. This should
 * of been done before.
 * Remove fast fog from strip rendering since it will only be
 * used with PCX2.
 *
 * Revision 1.6  1997/03/07  11:48:32  gdc
 * moved a # line to stop potential name space conflict in sgllite/sgl builds
 *
 * Revision 1.5  1997/03/05  08:50:34  erf
 * 1. Removed AddToXY application hint.
 * 2. fAddToXY set to -0.25f. This defines the offset to add
 *    to the vertices of any object. Value should be OK.
 *
 * Revision 1.4  1997/02/12  12:36:09  erf
 * In sgltri_startoffrnender() create new translucnet pass flushing object.
 * Also create new flat textured plane for opaque pass. To prevent the
 * flushing plane from using the masking plane if masking plane in use.
 *
 * Revision 1.3  1997/02/12  11:22:43  jop
 * Did Ed's fix for dummy plane tags and made fAddToXYT default to
 * -0.5
 *
 * Revision 1.2  1997/02/06  14:04:21  mjg
 * Added a parameter to ResetRegionDataL, as SGL need to force a reset.
 *
 * Revision 1.1  1997/02/04  12:19:26  gdc
 * Initial revision
 *
 **************************************************************************/

#define MODULE_ID MODID_SGLLITE

#include "sgl_defs.h" /* Already includes 'sgl.h' and 'debug.h' */
#include "sgl_init.h"
#include "pvrosapi.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "getnamtb.h"
#include "dlglobal.h"
#include "pmsabre.h"
#include "dregion.h"
#include "rnglobal.h"
#include "pktsp.h"
#include "pkisp.h"
#include "texapi.h"
#include "parmbuff.h"
#include "metrics.h"
#include "txmops.h" 	/* JWF Added to fix missing prototypes */
#include "sgl_math.h"	/* JWF */
#include "dvdevice.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

#if WIN32 || DOS32
    #include "version.h" /* auto-generated version number file */
	#include "hwregs.h" /* For get_texture_memory_size */
	#include "brdcfg.h"
	#define DO_FPU_PRECISION TRUE

	void SetupFPU (void);
	void RestoreFPU (void);
#else

	#define DO_FPU_PRECISION FALSE
#endif


#if MAC
#include "windoz.h"
#include "hwregs.h"
#define SET_PROCESS_STATE(x) 
#else
#define SET_PROCESS_STATE(x) 
#endif

#define WAIT_FOR_FRAME	0

/* Variable to fix the pixel offset problem. The value is added to
 * all vertices in D3D and SGL-Lite.
 * The offset is required because of the differences in precision,
 * in hardware, when storing the A, B and C parameters. In PCX1 and
 * PCX2 the C parameters has more bits of precision than the A and B
 * parameters.
 * The differences in precision is 2^(-16).
 */
float fAddToXY = -0.5f; /* must be the same as in pvrd.c */

#if PCX2
	extern sgl_bool PVROSFilteringOveride( void );
	extern sgl_bool PVROSBilinearEnabled( void );

	sgl_bool	bFilteringOverRide	= FALSE;
	sgl_bool	bBilinearEnabled	= FALSE;

	extern	sgl_bool	bFullMaskingPlane;
	extern	sgl_bool	bSetMaskingBGColour;
	static	sgl_bool	bUseMaskingPlane = FALSE;
#endif

static	sgl_uint32	TSPBackgroundAddress = 0;
static	sgl_uint32	uDithering		= 0x2;

/* No sort method, 1 fro no sort, 2 for reversed no sort, 3 for min Z sort */
int gNoSortTransFaces= MINZ_SORT;

/* these used to be in sgltri.c but as they are only used here... */
float gfBogusInvZ = 1.0f;
sgl_bool FogUsed;					   

/* This is maximum allowable translucent passes per tile.
 * Initialise to a crazy large value.
 */
sgl_uint32	nMaxPassCount		= 0x0FFFFFFF;

#if ISPTSP
#include "profile.h"
#endif

#if PCX1 || PCX2 || PCX2_003
extern sgl_uint32 TexParamSize;
#endif

extern sgl_uint32 gu32UsedFlags;

/*
// Variable to decide if we have initialised the system or not
//    0 - UNinitialised, NonZero - Initialised
//
// Set it to indicate that the system is uninitialised.
*/
extern int sglSystemInitialised;

extern int nTimeOutTenths;

/*
// the translucent control word is the TEXAS control word that
// 'points' to a single white pixel.This is used for non-textured
// translucency.
*/
extern sgl_uint32 TranslucentControlWord;

/*
// Define the global editing variables
*/
extern DL_USER_GLOBALS_STRUCT	dlUserGlobals;

/* extern definiton for the display current display handle.
 * Needed for the 2d callback function.
 */
extern HDISPLAY hDisplayHandle;

/* JWF Added
 */
int SglLiteInit(void);

extern void DirectPolygons ( PVRHANDLE TexHeap,
					  PSGLCONTEXT  pContext,
						int  nNumFaces,
					    int  pFaces[][3],
						PSGLVERTEX  pVertices,
						sgl_bool bQuads );

#if DEBUG
extern void DrawRegionGrid(PSGLCONTEXT pContext);
#endif

#ifdef DLL_METRIC
static int nRendersRecorded = -1;
static int snStateBar = -1; 
static float fScale=0.0f;
static float fTotalTime, fTotalTimeInFrame;
static float fTotalSWTime, fTotalSWTimeInFrame, fTotalSWTimeToAverage;
static float fTotalHWTime, fTotalHWTimeInFrame, fTotalHWTimeToAverage;
static int gnFramesToAverage, nNumberOfFrames;
static sgl_uint32 nPolygonsToAverage, nTransPolygonsToAverage, nOpaquePolygonsToAverage;
static sgl_uint32 nTotalPolygons, nTotalTransPolygons, nTotalOpaquePolygons; 
static float gfCPUSpeed, gfInvsFrames;

extern int gnOutputToConsole;
extern int DeviceX, DeviceY;
extern sgl_uint32 nTotalPolygonsInFrame, nTransPolygonsInFrame, nOpaquePolygonsInFrame;

void  GetFrameTime(void)
{
	static sgl_uint32 Time, LastTime=0;

	__asm
		{
			push	eax
			push	edx

			_emit 0Fh
			_emit 31h	

			shrd	eax, edx, 4

			mov		Time, eax
			pop		edx
			pop		eax
		}

	fTotalTimeInFrame = (Time - LastTime)*gfCPUSpeed;

	LastTime = Time;
}


void OutputMetric(void)
{  
#if TIMING
	/* Dump out the data */ 
   	DPFTIME(("%4d,      %5d,       %5d,       %5d",
   		Times[TOTAL_RENDER_TIME].Count,
		(int)(nPolygonsToAverage*gfInvsFrames),
		(int)(nTransPolygonsToAverage*gfInvsFrames),
		(int)(nOpaquePolygonsToAverage*gfInvsFrames) ));

#else
	
	/* Dump out the data */ 		
   	DPFDEV((DBG_MESSAGE, "%4d,   %10.2f,       %10.2f,            %5d,          %5d,    %5d",
		Times[TOTAL_RENDER_TIME].Count,
		fTotalSWTimeToAverage*gfInvsFrames,
		fTotalHWTimeToAverage*gfInvsFrames,
		(int)(nPolygonsToAverage*gfInvsFrames),
		(int)(nTransPolygonsToAverage*gfInvsFrames),
		(int)(nOpaquePolygonsToAverage*gfInvsFrames) ));
#endif
}

void OutputTotalMetric(void)
{   
	/* End recorded time */
	SGL_TIME_STOP(TOTAL_APP_TIME)

	/* Dump out the data */ 		
   	if(Times[TOTAL_RENDER_TIME].Count)
	{
#if TIMING

   		DPFTIME(("Total Frame                             %10d", 
   			Times[TOTAL_RENDER_TIME].Count));
		DPFTIME(("Average Frame Time                      %10.2f", 
  			(fTotalTime/Times[TOTAL_RENDER_TIME].Count) ));
		DPFTIME(("Average Software Time per Frame         %10.2f",
			(fTotalSWTime+fTotalSWTimeToAverage)/Times[TOTAL_RENDER_TIME].Count ));
		DPFTIME(("Average Waiting for Hardware            %10.2f", 
  			(fTotalHWTime+fTotalHWTimeToAverage)/Times[TOTAL_RENDER_TIME].Count ));
		DPFTIME(("Average Polygons per Frame              %10d", 
			(nTotalPolygons+nPolygonsToAverage)/Times[TOTAL_RENDER_TIME].Count));
		DPFTIME(("Average Translucent Polygons            %10d",
			(nTotalTransPolygons+nTransPolygonsToAverage)/Times[TOTAL_RENDER_TIME].Count));
		DPFTIME(("Average Opaque Polygons                 %10d",
			(nTotalOpaquePolygons+nOpaquePolygonsToAverage)/Times[TOTAL_RENDER_TIME].Count));  	

#else

		DPFDEV((DBG_FATAL, "Total Frame                             %10d", 
   			Times[TOTAL_RENDER_TIME].Count));
		DPFDEV((DBG_FATAL, "Average Frame Time                      %10.2f", 
  			(fTotalTime/Times[TOTAL_RENDER_TIME].Count) ));
		DPFDEV((DBG_FATAL, "Average Software Time per Frame         %10.2f",
			(fTotalSWTime+fTotalSWTimeToAverage)/Times[TOTAL_RENDER_TIME].Count ));
  		DPFDEV((DBG_FATAL, "Average Waiting for Hardware            %10.2f", 
  			(fTotalHWTime+fTotalHWTimeToAverage)/Times[TOTAL_RENDER_TIME].Count ));
		DPFDEV((DBG_FATAL, "Average Polygons per Frame              %10d", 
			(nTotalPolygons+nPolygonsToAverage)/Times[TOTAL_RENDER_TIME].Count));
		DPFDEV((DBG_FATAL, "Average Translucent Polygons            %10d",
			(nTotalTransPolygons+nTransPolygonsToAverage)/Times[TOTAL_RENDER_TIME].Count));
		DPFDEV((DBG_FATAL, "Average Opaque Polygons                 %10d",
			(nTotalOpaquePolygons+nOpaquePolygonsToAverage)/Times[TOTAL_RENDER_TIME].Count));  	
#endif
		nRendersRecorded = -1;
	}
}

void DrawStateBar(PSGLCONTEXT pContext)
{
	SGLCONTEXT StateBarContext = *pContext;
	SGLVERTEX Vertices[2];
	sgl_uint16 pSprites[][2] = {0,1};

	/* make the context safe for flat shaded */
	StateBarContext.u32Flags &= ~SGLTT_TEXTURE;
	StateBarContext.u32Flags &= ~SGLTT_GOURAUD;
	StateBarContext.u32Flags &= ~SGLTT_HIGHLIGHT;
	StateBarContext.u32Flags &= ~SGLTT_VERTEXTRANS;
	StateBarContext.u32Flags &= ~SGLTT_GLOBALTRANS;
	StateBarContext.u32Flags &= ~SGLTT_DISABLEZBUFFER;
	
	Vertices[0].u32Colour = 0x00ffffff;
	Vertices[1].u32Colour = 0x00ffffff;

	Vertices[0].fInvW = 10.0f;
	Vertices[1].fInvW = 10.0f;
	
	/* draw bachground white bar */
	Vertices[0].fX = 1.0f; 
	Vertices[1].fX = (float)(DeviceX - 2);
	Vertices[0].fY = (float)(DeviceY - 2);
	Vertices[1].fY = (float)(DeviceY - 6);
		
	sgltri_sprites(&StateBarContext, 1, pSprites, Vertices);

	/* draw state bar for Frame time */
   	Vertices[0].u32Colour = 0x00ffff00;
	Vertices[1].u32Colour = 0x00ffff00;
	Vertices[0].fY = (float)(DeviceY - 3);
	Vertices[1].fY = (float)(DeviceY - 5);

	Vertices[0].fX = 2.0f;
	Vertices[1].fX = 2.0f + fTotalTimeInFrame*fScale;
	
	if(Vertices[1].fX > (float)(DeviceX - 3))
	{
		Vertices[1].fX = (float)(DeviceX - 3);
	}
		
	sgltri_sprites(&StateBarContext, 1, pSprites, Vertices);

	/* draw state bar for SW time */
   	Vertices[0].u32Colour = 0x00ff00ff;
	Vertices[1].u32Colour = 0x00ff00ff;
	Vertices[0].fY = (float)(DeviceY - 3);
	Vertices[1].fY = (float)(DeviceY - 5);

	Vertices[0].fX = 2.0f;
	Vertices[1].fX = 2.0f + fTotalSWTimeInFrame*fScale;
	
	if(Vertices[1].fX > (float)(DeviceX - 3))
	{
		Vertices[1].fX = (float)(DeviceX - 3);
	}
		
	sgltri_sprites(&StateBarContext, 1, pSprites, Vertices);

	if(fTotalHWTimeInFrame >= 0.01)
	{
		/* draw state bar for HW time */
		Vertices[0].u32Colour = 0x000000ff;
		Vertices[1].u32Colour = 0x000000ff;
		Vertices[0].fX = Vertices[1].fX; 
		Vertices[1].fX = Vertices[0].fX + fTotalHWTimeInFrame*fScale;

		if(Vertices[1].fX > (float)(DeviceX - 3))
		{
			Vertices[1].fX = (float)(DeviceX - 3);
		}
		
		sgltri_sprites(&StateBarContext, 1, pSprites, Vertices);
	}
}

#endif


/**************************************************************************
 * Function Name  : GetNameTable
 * Inputs         : None
 * Outputs        : Default name table ptr
 * Returns        : Valid ptr assuming SglInitialise has been called ...
 * Global Used    : The Display list globals struct
 * Description    : Gets the name table for people who don't like too
 *					many header files in their lives
 **************************************************************************/
P_NAMTAB_STRUCT GetNameTable (void)
{
	return (dlUserGlobals.pNamtab);
}

#ifdef SGLLITE

/******************************************************************************
 * Function Name: sgl_get_versions
 *
 * Inputs		: -
 * Outputs		: -
 * Returns		: Pointer to internal versions structure
 * Globals Used	: -
 *
 * Description  : See SGL API document
 *****************************************************************************/
sgl_versions * CALL_CONV sgl_get_versions()
{
	static sgl_versions  versions;

	versions.library = VER_LIB_TXT;
	versions.required_header = SGL_HEADER_VERSION;

	return &versions;
}

/**************************************************************************
 * Function Name  : SglInitialise 
 * Description    : Dummy initialistion routine to link a 'lite' library
 *					routines.
 **************************************************************************/

int SglInitialise(void)
{
	return SglLiteInit();
}

/**************************************************************************
 * Function Name  : SglLiteInit
 * Inputs         : None
 * Outputs        : None
 * Returns        : 0 if OK, non zero otherwise.
 * Global Used    : sglSystemInitialised
 * Description    : Initialises the system by calling individual initialisation
 *					routines.
 **************************************************************************/

int SglLiteInit(void)
{
	int result;

	/*
	// Assume success until proven otherwise
	*/
	result = 0;

	/*
	// If we havent initialised the system, do so
	*/
	if(sglSystemInitialised == 0)
	{
		#if !MAC

			/* The Mac driver gets loaded as required, so this
			 * just causes an ugly console window to open every
			 * time the user runs a Rave App
			 */
			PVROSPrintf("Initialising SGL Ver:%s\n", VER_LIB_TXT);

		#endif
		
		#if WIN32 || DOS32

			if (!InitEnvironment ())
			{
				exit (1);
			}

		#endif

		#if DEBUG
				DebugInit(DBGPRIV_WARNING);
		#elif DEBUGDEV || TIMING
				DebugInit(DBGPRIV_MESSAGE);
	   	#endif

		
#if !WIN32		
		/* get the render timeout in tenths of a second */
		nTimeOutTenths = HWRdValFile( "RenderTimeout", 2);

		HWReset(); /* this is a full on reset*/

		#if ISPTSP
			HWSetupBunchOfBRIDGERegs();
		#endif
		
		HWSetupBunchOfISPRegs();

		HWSetupBunchOfTSPRegs();
#endif
		/*
		** Set up the name table
		*/
		result = InitNamtab( & dlUserGlobals.pNamtab);
			
		if(result == 0)
		{
			InitRegionDataL(); /* Previously in the  RnInitialise funcion */
		}

#if !WIN32
		if(result==0)
		{
		  	/* read the size for the texture parameter space in 4k pages */
		    TexParamSize = 1024*HWRdValFile( "TSPParamSize", (TEX_PARAM_SIZE/1024));
			result = HWInitParamMem();
		}
#endif

		/* Initialise the bit twiddling table IF SIMULATOR*/
#if SIMULATOR
		InitTwiddle();
#endif

		/*
		// Initialse the texture cache control structure
		*/
		InitCachedTextures();
#if !WIN32

		/* this allocates the overflow area for sabre parameters.
		** It HAS to be done in this position/
		*/

		SetupOverflowArea(TexParamSize);

		/*	the translucent pixel HAS to be set up after the
			texture memory */
	
		TranslucentControlWord=SetupTransPixel();
#endif

		/* init fast inverse sqrt lookup table */

		MakeInvSqrtLookupTable ();
		
		if(result == 0)
		{
			sglSystemInitialised = 1;
		}

		/*
		// Initialise whether we are logging memory acesses
		*/
#if WIN32 && (DEBUG || LogRelease)
			InitLogMemFile();
#endif
		
	}/*End if system not initialised*/

	return result;
}

#endif


/******************************************************************************
 * Function Name  : GetApplicationHints
 *
 * Inputs         : -
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : -
 *
 *****************************************************************************/

float 	fMinInvZ = INVZ_DEFAULT_SCALE_VALUE;
int nNoSortAppHint;

static void GetApplicationHints ()
{
	static sgl_uint32 GotHints = FALSE;

	if (!GotHints)
	{
		#if WIN32
		fMinInvZ = HWRdValFileFloat ("Near Z Clip", fMinInvZ);

		/* Read the maximum allowable translucent passes per tile. Read default section first
		 * and then read the app hint section.
		 */
		nMaxPassCount = HWRdValFileUInt ("MaxNumPasses", 0x0FFFFFFF);

		/* Read the dithering setting if present.
		 */
		uDithering = HWRdValFileUInt ("Dithering", uDithering);

		/* Read the no sort setting if present.	*/
		nNoSortAppHint = HWRdValFileUInt ("SGLTRANSSORT", 0);

#if DEBUGDEV || TIMING
		gnOutputToConsole = HWRdValFileUInt("OutputToConsole", 0);
		gnFramesToAverage = HWRdValFileUInt("FramesToAverage", 100);
		if(gnFramesToAverage)
		{
			gfInvsFrames = 1.0/gnFramesToAverage;
		}
#endif
		
#if PCX2
		/* Do we allow the user the masking plane.
		 */
		bUseMaskingPlane = HWRdValFileUInt ( "UseMaskingPlane", 0);

		if (bUseMaskingPlane)
		{
			/* Do we use the full masking plane or not. The full masking plane
			 * is required to prevent sparkles in D3D applications when the
			 * masking plane is being used. This is due to a bug in the hardware.
			 * Should be fixed in PCX2-003. This has a side effect of some
			 * applications running slower.
			 * The default is with the minimum masking plane.
			 */
			bFullMaskingPlane =  HWRdValFileUInt( "FullMaskingPlane", 0);

			/* Do we allow the user to set the colour of the masking plane.
			 */
			bSetMaskingBGColour = HWRdValFileUInt("SetMaskingBGColour", 0);
		}
#endif
		#else
		#pragma message ("Application hints disabled!")
		#endif

		GotHints = TRUE;
	}
}

/******************************************************************************
 * Function Name  : SaveDeviceName
 *
 * Inputs         : Name table entry from sgl_create_screen_device
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : gDeviceName
 *
 * Description    :	Saves the name of the most recently created device.
 *				    NOTE with SGLDirect, there is expected to be only
 *				 	device at a name.
 *****************************************************************************/
static int gDeviceName = 0;

void SaveDeviceName(int name)
{
	gDeviceName = name;
}


/******************************************************************************
 * Function Name  : sgltri_startofframe
 *
 * Inputs         : -
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : PVRParamBuffs
 *
 * Description    : see sgltri.h header file
 *****************************************************************************/
/* used when getting parameter buffers */
extern HLDEVICE        gHLogicalDev;

extern void CALL_CONV sgltri_startofframe(PSGLCONTEXT pContext)
{
	DEVICE_REGION_INFO_STRUCT	RegionInfo;
	REGIONS_RECT_STRUCT			RegionRect;
	sgl_int32  						BackGroundStart;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	PVROSERR err ;									  

#ifdef DLL_METRIC
	if( ++nRendersRecorded == 0 )
	{
		/* Set it to zero the first frame */
		memset( &Times, 0, NUM_TIMERS*sizeof(Temporal_Data) );
		nTotalPolygons = nTotalTransPolygons = nTotalOpaquePolygons = 0;
		nNumberOfFrames = 0;
		nPolygonsToAverage = 0;
		nTransPolygonsToAverage = 0;
		nOpaquePolygonsToAverage = 0;
		fTotalSWTimeToAverage = 0.0;
		fTotalHWTimeToAverage = 0.0;
		fTotalTime = 0.0;
		fTotalSWTime = 0.0;
		fTotalHWTime = 0.0;
#if TIMING		
		DPFTIME(("             Performence Frame Rate")); 
		DPFTIME(("Frame,  Total Polygons,  Translucent,  Opaque")); 
#elif DEBUGDEV		
		DPFDEV((DBG_MESSAGE,"					  Performence Frame Rate                          ")); 
		DPFDEV((DBG_MESSAGE,"Frame,  Software Time, Waiting for Hardware, Total Polygons, Translucent, Opaque")); 
#endif
		GET_TICK_FREQ(gfCPUSpeed);
		if(!gfCPUSpeed)
		{
			DPFDEV((DBG_MESSAGE, "Failed to get CPU Frequence"));
		}
		else
		{
			gfCPUSpeed = 1000.0f/gfCPUSpeed;
		}

		/* Start recorded time */
	   	SGL_TIME_START(TOTAL_APP_TIME)
		GetFrameTime();
	}
	nTotalPolygonsInFrame = 0;
	nTransPolygonsInFrame = 0;
	nOpaquePolygonsInFrame = 0;
#endif
	
	SGL_TIME_START(SGLTRI_STARTOFFRAME1_TIME)

	gu32UsedFlags |= pContext->u32Flags;
#if !WIN32
    if (SglInitialise ())
	{
		SglError (sgl_err_failed_init);
		return;
	}
#endif

	GetApplicationHints ();

	/*
	// Reset the region lists structures to be empty
	*/
	ResetRegionDataL (FALSE);

	/*
	// Get parameter memory, if available...
	*/
	
#if WIN32
	err = PVROSAssignVirtualBuffers(PVRParamBuffs, gHLogicalDev);
	if(err!=PVROS_GROOVY)
	{
		SglError (sgl_err_failed_init);
		return;
	}
#else
	GetParameterSpace(PVRParamBuffs);
#endif
	
	/*
	// Reset the "fog used" global value auntil proven incorrect.
	*/
	FogUsed = 0;

#if PCX2
	bFilteringOverRide =  PVROSFilteringOveride();
	bBilinearEnabled =  PVROSBilinearEnabled();
#endif	

#if PCX2 || PCX2_003
	pProjMat->f32FixedClipDist = 1.0f;
#else
	pProjMat->n32FixedClipDist = 0x7FFFFFFF;
#endif

	/* no sort method */
	if(!nNoSortAppHint)
	{
		if(pContext->u32Flags & SGLTT_SELECTTRANSSORT)
			gNoSortTransFaces = (int)pContext->eTransSortMethod;
	}
	else
	{
		gNoSortTransFaces = nNoSortAppHint;
	}

	/* //////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Add some "special" objects direct to the parameter
	// store. THESE SHOULD BE MOVED OUT AND SET UP ONCE ONLY
	// DURING INTIALISATION (obviously the initial pointers would have
	// to take account of these).
	/////////////////////////////////////////////////////
	////////////////////////////////////////////////// */
	AddDummyPlanesL (pContext->eShadowLightVolMode);

	/* device 0 for now
	 */
	HWGetRegionInfo (0, &RegionInfo);

	RegionRect.FirstXRegion = 0;
	RegionRect.FirstYRegion = 0;
	RegionRect.LastXRegion = RegionInfo.NumXRegions - 1;
	RegionRect.LastYRegion = RegionInfo.NumYRegions - 1;

	BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

	#if PCX2 || PCX2_003
	if (bExternalAPI && (pContext->u32Flags & SGLTT_TRANSBACKGROUND))
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
		/* Masking not required. No such thing in SGLDirect as it is only
		 * needed for D3D.
		 */
		TSPBackgroundAddress = PackTexasFlat(pContext->cBackgroundColour, 
											pContext->bFogOn, 
											pContext->eShadowLightVolMode);
	}

	/* Add the background plane. The TSP tag used is either a flat
	 * textured plane or a full textured masking plane.
	 */
#if PCX2 || PCX2_003
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

#if PCX2 || PCX2_003
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

#if PCX2 || PCX2_003
	PackBackgroundPlane (PackTexasTransparent (FALSE), -1.0f);
#else
	PackBackgroundPlane (PackTexasTransparent (FALSE), -64);
#endif
	AddTransFlushingPlaneL (BackGroundStart);

	pContext->FirstXRegion = RegionRect.FirstXRegion;
	pContext->FirstYRegion = RegionRect.FirstYRegion;
	pContext->LastXRegion = RegionRect.LastXRegion;
	pContext->LastYRegion = RegionRect.LastYRegion;

	pContext->invRegX = 1.0f / RegionInfo.XSize;
	pContext->invRegY = 1.0f / RegionInfo.YSize;

	if (pContext->eShadowLightVolMode == ENABLE_LIGHTVOLS)
	{
		AllowLightVolAdditionL (&RegionRect);
	}

	gfBogusInvZ = 0.0f;
	
#if PCX2_003
	/* For PCX2-003 the filter selection is determined by both
	 * eFilterType and SGLTT_BILINEAR flag. 
	 */

	if (pContext->u32Flags & SGLTT_BILINEAR)
	{
		pProjMat->eFilterType = pContext->eFilterType;
	}
	else
	{
		pProjMat->eFilterType = sgl_tf_point_sample;
	}

#elif PCX2
	/* For PCX2-002 the filter selection is completely determined by
	 * eFilterType. ie the SGLTT_BILINEAR flag is ignored.
	 */
	pProjMat->eFilterType = pContext->eFilterType;

	if (!bFilteringOverRide)
	{
		/* We haven't overriden the application settings so continue.
		 * Bilinear enabled or not ????
		 */
		bBilinearEnabled = (pProjMat->eFilterType == sgl_tf_point_sample) ? FALSE : TRUE;
	}
#endif /* #if PCX2_003	*/

	SGL_TIME_STOP(SGLTRI_STARTOFFRAME2_TIME)
} /* sgltri_startofframe */



/******************************************************************************
 * Function Name  : sgltri_render
 *
 * Inputs         : cBackground
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : PVRParamBuffs
 *
 * Description    : see sgltri.h header file
 *****************************************************************************/

extern void CALL_CONV sgltri_render (PSGLCONTEXT pContext)
{
#if !WIN32
	sgl_uint32 				SabreRegionInfoStart;
#endif
	HDISPLAY hDisplay;
	REGIONS_RECT_STRUCT	RegionRect;
	sgl_bool			bRenderAllRegions;
	DEVICE_NODE_STRUCT	*dNode;
#if DEBUG
	static int snDrawGrid = -1;
#endif

#if ISPTSP
	int  nNumRegionsRendered;
#endif
#if PCX2 || PCX2_003
	/* PCX2 Fast fog colour value. Set by SGL or SGL-Lite.
	 */
	sgl_colour	cFastFogColour = {0.0f, 0.0f, 0.0f};

	/* Needed to read the texture filtering setting.
	 */
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
#endif

	SGL_TIME_START(TOTAL_RENDER_TIME);

	gu32UsedFlags |= pContext->u32Flags;

	/*
	// ---------------------------
	// Ensure system is initialsed
	// ---------------------------
	*/

    if (SglInitialise ())
	{
		SglError (sgl_err_failed_init);
		SGL_TIME_STOP(TOTAL_RENDER_TIME);
		return;

	}

	/*
	// ---------------------------
	// Execute the hardware render
	// ---------------------------
	*/
#if DEBUG
	if(snDrawGrid == -1)
	{
		snDrawGrid = SglReadPrivateProfileInt ("DEBUG", "RegionGrid", 0, "sglhw.ini");
	}
	if(snDrawGrid == 1)
	{
		DrawRegionGrid(pContext);
	}
#endif

#ifdef DLL_METRIC
	if(snStateBar == -1)
	{
		snStateBar = HWRdValFileUInt ("StateBar", 0);
		if(snStateBar > 0)	 fScale = (float)((DeviceX - 4)/snStateBar);
	}
	if(snStateBar > 0)
	{
		DrawStateBar(pContext);
	}
#endif

	RegionRect.FirstXRegion = pContext->FirstXRegion;
	RegionRect.FirstYRegion = pContext->FirstYRegion;
	RegionRect.LastXRegion = pContext->LastXRegion;
	RegionRect.LastYRegion = pContext->LastYRegion;

#if PCX2 || PCX2_003
	/* Fast fogging. Pack a plane for fogging. Only used by PCX2.
	 * Set colour of plane to FOG COLOUR !!!!
	 */
	{
		sgl_uint32		nCurrentTSPAddr;

		cFastFogColour[0] =	pContext->fFogR;
		cFastFogColour[1] =	pContext->fFogG;
		cFastFogColour[2] =	pContext->fFogB;

		/* Save current TSP index.
		 */
		nCurrentTSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Tag ID of 2 (4/2) used for fogging.
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = 4;
		
		/* Pack a flat plane.
		 */
		PackTexasFlat (cFastFogColour, pContext->bFogOn, FALSE);

		/* Restore the TSP index.
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = nCurrentTSPAddr;
	}
#endif
   
   	/*
	// If we are called by Direct3D and there is no fog then render only those
	// regions that contain objects, else render all regions in the device.
	*/
#if WIN32
	if ((pContext->RenderRegions == DONT_RENDER_EMPTY_REGIONS) &&
		(!FogUsed ||
		 (
		  (pContext->fFogR == pContext->cBackgroundColour[0]) &&
		  (pContext->fFogG == pContext->cBackgroundColour[1]) &&
		  (pContext->fFogB == pContext->cBackgroundColour[2])
		 )
		))
	{
		bRenderAllRegions = FALSE;
	}
	else
	{
		bRenderAllRegions = TRUE;
	}

#else
	/*
	// Convert the regions lists to ones understood by Sabre.
	// Remember where the pointer data begins though.
 	*/
	SabreRegionInfoStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	
	bRenderAllRegions = TRUE;

#endif

	SGL_TIME_SUSPEND(TOTAL_RENDER_TIME);
    SGL_TIME_START(GENERATE_TIME);

#if ISPTSP
	nNumRegionsRendered = 
#endif	
	GenerateObjectPtrLite( &RegionRect, bRenderAllRegions );

    SGL_TIME_STOP(GENERATE_TIME);
	SGL_TIME_RESUME(TOTAL_RENDER_TIME);
	
	/* //////////////////////////////////////////////////
	// Wait till the hardware is available,
	////////////////////////////////////////////////// */

#if PCX2 || PCX2_003
	/* Set the texture filtering register.
	 * Need to wait for the hardware to become available.
	 */
	HWSetBilinearRegister(pProjMat->eFilterType);
#endif /* #if PCX2	*/

	/*
	// Set up the hardware registers:
	*/

#if !WIN32
	/* Sabre pointer in windows builds set on virtual buffer allocation */
	#if ISPTSP
	HWSetRegionsRegister(nNumRegionsRendered);
	#endif
	HWSetSabPtrRegister(SabreRegionInfoStart, 0);
#endif
	/*
	// Convert the supplied fog colour ... providing it's valid,
	// u32FogDensity is an unsigned int, so it's always >= 0
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

		HWSetFogRegister (pContext->u32FogDensity);

		fogColour.red	 =	(unsigned char)(pContext->fFogR * 255);
		fogColour.green  =	(unsigned char)(pContext->fFogG * 255);
		fogColour.blue	 =	(unsigned char)(pContext->fFogB * 255);
		fogColour.alpha  =	0;

		TexasSetFogColour (fogColour);
	}
	/*
	// Else set the least dense, black fog
	*/
	else
	{
		sgl_map_pixel  fogColour;

		HWSetFogRegister (0);

		fogColour.red	 =	0;
		fogColour.green  =	0;
		fogColour.blue	 =	0;
		fogColour.alpha  =	0;

		TexasSetFogColour (fogColour);
	}	

	/*
	// Set the texture scale flag: ****** TEXTURING NOT IMPLEMENTED YET ******
	*/
	TexasSetCFRScale(MAX_CFR_VALUE /*projection_mat.n32CFRValue*/);
	DPF((DBG_MESSAGE, "CFR Scale is 0x%lX",
	  (long)MAX_CFR_VALUE/*projection_mat.n32CFRValue*/));

	/*
	// Device hardwired to 0 because I don't understand what is going on:
	*/
	{
		int  nXDimension, nYDimension;

		DPFOO((DBG_WARNING, "Device in RN render hardwired to 0"));
		HWGetDeviceSize(0, &nXDimension,&nYDimension);
#if PCX1 || PCX2 || PCX2_003
	#if !MAC
		/* Mac driver handles this differently at present */
		HWSetXClip(FALSE, 0, TRUE, nXDimension+1);
	#endif
#endif
		TexasSetDim(nXDimension, nYDimension);
	}

	DPF((DBG_MESSAGE, "Calling HWStartRender"));	

	/* Call the 2D callback function, using the stored device name */

    dNode = (PVRHANDLE) GetNamedItem(dlUserGlobals.pNamtab, gDeviceName);
	
	if (dNode)
	{
		hDisplay = (HDISPLAY)(dNode->PhDeviceID);
	}
	else
	{
 		DPFDEV ((DBG_ERROR, "Cant get device for 2D Callback")); 						
	}
			
#if !WIN32
 	/* If we had to use a software buffer for either sabre/texas (or both) then
	   copy them into the correct buffer space. */
	
	PVROSCopyParamsIfRequired(PVRParamBuffs);
#endif

	/************* RENDER IS STARED HERE ****************/
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
			HWStartRender(TRUE, hDisplay, FALSE);
		}
		else
		{
			/* Enable dithering.
			 */
			HWStartRender(TRUE, hDisplay, TRUE);
		}
	}
	else
	{
		/* Set dithering based on uDithering field.
		 */
		HWStartRender(TRUE, hDisplay, (sgl_bool) uDithering);
	}
	
	DPF((DBG_MESSAGE, "Done HWStartRender !!!!"));

#if DUMP_PARAMS
	/*
	// For Sabre/Texas Debugging, output files of the parameter store contents.
	*/
#if DUMP_WHEN_FIXED
	if (nFrameNum == FIX_FRAME_NUM)
#endif
	if (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferpos == 0)
	{
		DumpSabreAndTexas(
		  SabreRegionInfoStart, PVRParamBuffs, MAX_CFR_VALUE);
	}
#endif /* DUMP_PARAMS */

	/*
	// Code to dump out all the hardware regsiters
	*/
#define DUMP_HW_REGS 0
#if DUMP_HW_REGS && WIN32 && DEBUG
	{
		FILE * outfile;

		outfile = fopen("regdump.txt", "w");
		
		HWDumpRegisters(outfile);

		fclose(outfile);
	}
#endif

#if WAIT_FOR_FRAME

	SGL_TIME_SUSPEND(TOTAL_RENDER_TIME);
	SGL_TIME_START(RENDER_WAITING_TIME);

	while(!HWFinishedRender());

	SGL_TIME_STOP(RENDER_WAITING_TIME);
	SGL_TIME_RESUME(TOTAL_RENDER_TIME);

#endif

	PVROSCallback (gHLogicalDev, CB_POST_RENDER, NULL);
	
	SglError(sgl_no_err);
	SGL_TIME_STOP(TOTAL_RENDER_TIME);
   
#ifdef DLL_METRIC
	
	nPolygonsToAverage += nTotalPolygonsInFrame;
	nTransPolygonsToAverage += nTransPolygonsInFrame;
	nOpaquePolygonsToAverage += nOpaquePolygonsInFrame;
	fTotalSWTimeInFrame = (Times[TOTAL_RENDER_TIME].Stop +  Times[GENERATE_TIME].Stop +
		Times[SGLTRI_TRIANGLES_TIME].Stop +  Times[SGLTRI_STARTOFFRAME1_TIME].Stop +
		Times[SGLTRI_STARTOFFRAME2_TIME].Stop +  Times[SGLTRI_PROCESS_TIME].Stop +
		Times[SGLTRI_PACKTRI_TIME].Stop)*gfCPUSpeed;
	fTotalHWTimeInFrame = (Times[RENDER_WAITING_TIME].Stop)*gfCPUSpeed;
	GetFrameTime();	
	fTotalTime += fTotalTimeInFrame;
	fTotalSWTimeToAverage += fTotalSWTimeInFrame;
	fTotalHWTimeToAverage += fTotalHWTimeInFrame;
	nNumberOfFrames++;

	if(nNumberOfFrames == gnFramesToAverage)
	{
		nTotalPolygons += nPolygonsToAverage;
   		nTotalTransPolygons += nTransPolygonsToAverage;
   		nTotalOpaquePolygons += nOpaquePolygonsToAverage;
		fTotalSWTime += fTotalSWTimeToAverage;
		fTotalHWTime += fTotalHWTimeToAverage;
		OutputMetric();
		nPolygonsToAverage = 0;
		nTransPolygonsToAverage = 0;
		nOpaquePolygonsToAverage = 0;
		fTotalSWTimeToAverage = 0.0;
		fTotalHWTimeToAverage = 0.0;
		nNumberOfFrames = 0;
	}
#endif

} /* sgltri_render */

/******************************************************************************
 * Function Name  : sgltri_rerender
 *
 * Inputs         : pContext  (not currently used)
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : -
 *
 * Description    : See API document (just starts hardware render again for the
 *					previously created scene - repeatedly calling
 *					sgltri_rerender for a scene gives ISVs a guide to our
 *					hardware performance for their benchmarks)
 *****************************************************************************/

extern void CALL_CONV sgltri_rerender (PSGLCONTEXT pContext)
{
    if (SglInitialise ())
	{
		SglError(sgl_err_failed_init);
		return;
	}
	
	/************* RENDER IS STARED HERE ****************/
	if (pContext->u32Flags & SGLTT_DISABLEDITHERING)
	{
		/* Disable dithering.
		 */
		HWStartRender(TRUE, NULL, FALSE);
	}
	else
	{
		/* Enable dithering.
		 */
		HWStartRender(TRUE, NULL, TRUE);
	}

	SglError(sgl_no_err);

} /* sgltri_rerender */

/******************************************************************************
 * Function Name  : sgltri_isrendercomplete
 *
 * Inputs         : -
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : -
 *
 * Description    : see sgltri.h header file
 *****************************************************************************/
extern IRC_RESULT CALL_CONV sgltri_isrendercomplete (PSGLCONTEXT  pContext, 
											  sgl_uint32 u32Timeout)
{
	gu32UsedFlags |= pContext->u32Flags;

	if (u32Timeout > 0)
	{
		if (HWFinishedRender ())
		{
			return (IRC_RENDER_COMPLETE);
		}
		else
		{
			/* I'm going to do something horrible. Hee Hee. */
			sgl_largeint LI;
			
			float fSh32Left = 256.0f*256.0f*256.0f*256.0f;
			float fStartTime, fCurrentTime, fTimeout;
			
			PVROSQueryPerformanceCounter (&LI);
			
			fTimeout = ((fSh32Left * LI.HighPart) + LI.LowPart) * (1.0f/1000.0f);
			
			PVROSQueryPerformanceCounter (&LI);

			fStartTime = (fSh32Left * LI.HighPart) + LI.LowPart;

			do
			{
				if (HWFinishedRender ())
				{
					return (IRC_RENDER_COMPLETE);
				}

				PVROSQueryPerformanceCounter (&LI);

				fCurrentTime = (fSh32Left * LI.HighPart) + LI.LowPart;
			}
			while ((fCurrentTime - fStartTime) < fTimeout);

		}
			
		return (IRC_RENDER_TIMEOUT);
	}
	else
	{
		if (HWFinishedRender ())
		{
			return (IRC_RENDER_COMPLETE);
		}
		else
		{
			return (IRC_RENDER_NOT_COMPLETE);
		}
	}
}

/******************************************************************************
 * Function Name  : sgltri_renderstrip
 *
 * Inputs         : cBackground
 * Outputs        : -
 * Input/Outputs  : -
 * Returns        : -
 * Globals Used   : PVRParamBuffs
 *
 * Description    : see sgltri.h header file
 *****************************************************************************/

extern sgl_bool CALL_CONV sgltri_renderstrip (PSGLCONTEXT pContext, int nStrip, int nXExtent[2])
{
	REGIONS_RECT_STRUCT	RegionRect;
	sgl_bool			bRenderAllRegions;
	static DEVICE_REGION_INFO_STRUCT RegionInfo;
	sgl_bool			fRet = FALSE;
#if !WIN32
	static sgl_uint32		SabreRegionInfoStart;
#endif
	
	/*
	// ---------------------------
	// Ensure system is initialsed
	// ---------------------------
	*/

    if (SglInitialise ())
	{
		SglError (sgl_err_failed_init);
	}
	else
	{
		SGL_TIME_START(TOTAL_RENDER_TIME);
		
		RegionRect.FirstXRegion = pContext->FirstXRegion;
		RegionRect.FirstYRegion = pContext->FirstYRegion;
		RegionRect.LastXRegion = pContext->LastXRegion;
		RegionRect.LastYRegion = pContext->LastYRegion;
	
		if (nStrip == -1)
		{
			/*
			// If we are called by Direct3D and there is no fog then render only those
			// regions that contain objects, else render all regions in the device.
			*/
			#if WIN32
	
				if ((pContext->RenderRegions == DONT_RENDER_EMPTY_REGIONS) &&
					(!FogUsed ||
					 (
					  (pContext->fFogR == pContext->cBackgroundColour[0]) &&
					  (pContext->fFogG == pContext->cBackgroundColour[1]) &&
					  (pContext->fFogB == pContext->cBackgroundColour[2])
					 )
					))
				{
					bRenderAllRegions = FALSE;
				}
				else
				{
					bRenderAllRegions = TRUE;
				}
	
			#else
				/* save the start of region data */
				
				SabreRegionInfoStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
						
				bRenderAllRegions = TRUE;
	
			#endif
	
			GenerateObjectPtrLiteStrip (&RegionRect, bRenderAllRegions);
	
			/* //////////////////////////////////////////////////
			// Wait till the hardware is available,
			////////////////////////////////////////////////// */
			
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
	
				HWSetFogRegister (pContext->u32FogDensity);
	
				fogColour.red	 =	(unsigned char)(pContext->fFogR * 255);
				fogColour.green  =	(unsigned char)(pContext->fFogG * 255);
				fogColour.blue	 =	(unsigned char)(pContext->fFogB * 255);
				fogColour.alpha  =	0;
	
				TexasSetFogColour (fogColour);
			}
			/*
			// Else set the least dense, black fog
			*/
			else
			{
				sgl_map_pixel  fogColour;
	
				HWSetFogRegister (0);
	
				fogColour.red	 =	0;
				fogColour.green  =	0;
				fogColour.blue	 =	0;
				fogColour.alpha  =	0;
		
				TexasSetFogColour (fogColour);
			}	
	
			/*
			// Set the texture scale flag
			*/
			TexasSetCFRScale (MAX_CFR_VALUE);
			DPF((DBG_MESSAGE, "CFR Scale is 0x%lX", (long) MAX_CFR_VALUE));
	
			/*
			// Device hardwired to 0 because I don't understand what is going on:
			*/
			{
				int  nXDimension, nYDimension;
	
				DPFOO ((DBG_MESSAGE, "Device in RN render hardwired to 0"));
				HWGetDeviceSize (0, &nXDimension,&nYDimension);
	#if PCX1 || PCX2 || PCX2_003
		#if !MAC
				/* Mac driver handles this differently at present */
				HWSetXClip(FALSE, 0, TRUE, nXDimension+1);
		#endif
	#endif
				TexasSetDim (nXDimension, nYDimension);
			}
			
			/* device 0 for now */
			HWGetRegionInfo (0, &RegionInfo);
	
			fRet = TRUE;
		}
		else
		{
			REGION_STRIP_DATA RegionStrip; 
			REGION_STRIP_DATA *pRegionStrip = &RegionStrip;
			#if ISPTSP
				sgl_int32 nNumRegionsRendered;
			#endif
			pRegionStrip->fObjectsPresent = FALSE;
				
			#if ISPTSP
			nNumRegionsRendered = 
			#endif
			SetupStripLite (nStrip, &RegionRect, pRegionStrip);
	
			if (pRegionStrip->fObjectsPresent)
			{
				PRE_RENDER_CB_STRUCT PRCS;
				
				/* //////////////////////////////////////////////////
				// Wait till the hardware is available,
				////////////////////////////////////////////////// */
		
				/*
				// Set up the hardware registers:
				*/
#if !WIN32
				/* Sabre pointer in windows builds set on virtual buffer allocation */
				HWSetSabPtrRegister (SabreRegionInfoStart, pRegionStrip->FirstObjectOffset);
			
				#if ISPTSP
					HWSetRegionsRegister( nNumRegionsRendered );
				#endif
#endif				
				while (!HWFinishedRender ())
				{
					/* Do nothing */
				}

				DPF((DBG_VERBOSE, "Calling HWStartRender"));	
	
				PRCS.FlipRequested = FALSE;
	
				if (PVROSCallback (gHLogicalDev, CB_PRE_RENDER, &PRCS) != PVROS_GROOVY)
				{
					DPFDEV ((DBG_ERROR, "PVROSCallback failed"));
				}
				else
				{
					sgl_uint32 dwSOFScanlineOffset;
					
					if (pContext->u32Flags & SGLTT_DISABLEDITHERING)
					{
						/* Disable dithering.
						 */
						PVROSSetPCIPixelFormat ((sgl_uint16) PRCS.PhysRenderBufferBitsPerPixel,
												FALSE);
					}
					else
					{
						/* Enable dithering.
						 */
						PVROSSetPCIPixelFormat ((sgl_uint16) PRCS.PhysRenderBufferBitsPerPixel,
												TRUE);
					}

					dwSOFScanlineOffset = RegionInfo.YSize * PRCS.PhysRenderBufferStride * nStrip;
	
					ProgramSOFAddressReg (PRCS.PhysRenderBufferAddress - dwSOFScanlineOffset);
	
					ProgramStrideReg (PRCS.PhysRenderBufferStride);
	
					HWStartRenderStrip();
					
					DPF ((DBG_VERBOSE, "Done HWStartRender !!!!"));
	
					PVROSCallback (gHLogicalDev, CB_POST_RENDER, NULL);
				
					nXExtent[0] = pRegionStrip->nXExtents[0];
					nXExtent[1] = pRegionStrip->nXExtents[1];
				}
				
				fRet = TRUE;
			}
		}
		
		SglError(sgl_no_err);
		SGL_TIME_STOP(TOTAL_RENDER_TIME);
#ifdef DLL_METRIC   	
	OutputMetric();
#endif

	}
	return (fRet);

} /* sgltri_renderstrip */

/**********************************************************************/

void CALL_CONV sgltri_triangles ( PSGLCONTEXT pContext,
								  int nNumFaces,
								  int pFaces[][3],
								  PSGLVERTEX pVertices )
{
#ifdef DLL_METRIC   	
   	nTotalPolygonsInFrame += nNumFaces;
#endif
		
	SGL_TIME_START(SGLTRI_TRIANGLES_TIME);

	gu32UsedFlags |= pContext->u32Flags;

	/*
	// ----------------------
	// Check input parameters
	// ----------------------
	*/

	if (nNumFaces == 0)
	{
		SglError(sgl_no_err);
	}
	else if (pContext == NULL || pFaces == NULL || pVertices == NULL || nNumFaces < 0)
	{
		DPFDEV ((DBG_ERROR, "sgltri_triangles: calling with bad parameter."));			
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
		DirectPolygons (gHLogicalDev->TexHeap, pContext, nNumFaces, pFaces, pVertices, FALSE);
		SglError(sgl_no_err);
	}

	SGL_TIME_STOP(SGLTRI_TRIANGLES_TIME)
}


/**********************************************************************/

void CALL_CONV sgltri_quads ( PSGLCONTEXT  pContext,
							  int nNumFaces,
							  int pFaces[][4],
							  PSGLVERTEX  pVertices )
{
#ifdef DLL_METRIC   	
   	nTotalPolygonsInFrame += nNumFaces;
#endif
	
	SGL_TIME_START(SGLTRI_TRIANGLES_TIME);

	gu32UsedFlags |= pContext->u32Flags;
	/*
	// ----------------------
	// Check input parameters
	// ----------------------
	*/

	if (nNumFaces == 0)
	{
		SglError(sgl_no_err);
	}
	else if (pContext == NULL || pFaces == NULL || pVertices == NULL || nNumFaces < 0)
	{
		DPFDEV ((DBG_ERROR, "sgltri_quads: calling with bad parameter."));				
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
		/* Mac requires a cast in the following param list */
		DirectPolygons (gHLogicalDev->TexHeap, pContext, nNumFaces, (int(*)[3])pFaces, pVertices, TRUE);
		SglError(sgl_no_err);
	}

	SGL_TIME_STOP(SGLTRI_TRIANGLES_TIME)
}

/*------------------------------- End of File -------------------------------*/
