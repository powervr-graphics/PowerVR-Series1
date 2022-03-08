/**************************************************************************
 * Name         : rnglobal.c
 * Title        : Definition of the rendering global vars (other than
 *				  state variables).
 * Author       : Simon Fenney
 * Created      : 11 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Defines the SGL render globals as defined in TRDD027x.doc
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnglobal.c,v $
 * Revision 1.60  1997/09/24  14:20:34  gdc
 * added stuff for API developers
 *
 * Revision 1.59  1997/08/20  10:08:16  sks
 * Added VertexFogControlWord.
 *
 * Revision 1.58  1997/07/24  17:01:32  gdc
 * removed a windows.h, and function WaitForHardware as it is redundant
 * This was also setting thread priorities which we shouldn't really be doing
 *
 * Revision 1.57  1997/07/14  19:31:48  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.56  1997/07/14  09:59:46  gdc
 * tried to turn off windows include file warnings and failed
 *
 * Revision 1.55  1997/05/14  03:40:52  jop
 * Did some init time changes
 *
 * Revision 1.54  1997/05/13  10:47:23  gdc
 * not win32'd out the getparameterspace function as PVROSGetVitualBuffers
 * now called direct
 *
 * Revision 1.53  1997/05/09  09:22:40  erf
 * Moved AddDummyPlanesL() function to pkisp.c.
 *
 * Revision 1.52  1997/05/08  08:26:54  erf
 * 1. Added get fixed clip distance routine.
 * 2. Initialised translucent control word in SGLSetGlobal.
 *
 * Revision 1.51  1997/05/06  09:48:01  gdc
 * added an assert to check that texheap = htexheap
 *
 * Revision 1.50  1997/05/02  16:35:25  jop
 * Added texure interface global
 *
 * Revision 1.49  1997/05/02  14:42:19  jop
 * Can't remember doing anything to this but hey ...
 *
 * Revision 1.48  1997/05/01  02:42:07  jop
 *  Undid Ed's fixes
 *
 * Revision 1.47  1997/04/30  19:20:37  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.46  1997/04/29  15:01:14  erf
 * 1. Added new PVROS API function call mechanism.
 * 2. Defined PPVROSAPI_IF pIf. Pointer to PVROS API structure.
 * 3. Removed extern reference to CopyParamsIfRequired().
 *
 * Revision 1.45  1997/04/24  19:40:21  gdc
 * added write to gHLogicalDev global
 *
 * Revision 1.44  1997/04/24  12:05:15  mjg
 * Started on SglSetGlobal
 *
 * Revision 1.43  1997/04/23  18:21:19  gdc
 * chnages for multi-context
 *
 * Revision 1.42  1997/04/22  17:39:58  mjg
 * Tided up includes
 *
 * Revision 1.41  1997/04/21  20:01:08  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.40  1997/04/21  18:40:13  gdc
 * removed HSYSTEM parameter on PVROSCopyBuffertoHardware as HSYSTEM is now local
 * to pvros
 *
 * Revision 1.39  1997/04/21  15:28:23  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.38  1997/04/15  13:09:26  mjg
 * Tided up, put in the PVR_PARAM_BUFF functions and started to use them.
 *
 * Revision 1.37  1997/04/09  17:12:28  gdc
 * projection_mat now static module to this module
 *
 * Revision 1.36  1997/04/09  10:59:07  gdc
 * added another function to aid in de-globalisation of projection_mat
 *
 * Revision 1.35  1997/04/08  16:49:37  gdc
 * more chnages to elimate projection_mat as a global
 *
 * Revision 1.34  1997/04/08  14:38:05  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.33  1997/04/08  09:09:58  gdc
 * added windows.h for WIN32 builds to use GetCurrentThread etc.
 *
 * Revision 1.32  1997/04/08  08:51:30  gdc
 * made current light slot, wrapping transform, and absolute coord transform
 * local to this modeule with their value got as set via functions
 *
 * Revision 1.31  1997/04/07  14:54:25  gdc
 * added a new GetDefaultCamera function so that DefaultCamera is
 * now local to this module
 *
 * Revision 1.30  1997/04/03  17:53:46  mjg
 * Included 'hwregs.h'
 *
 * Revision 1.29  1997/04/03  11:45:59  gdc
 * trying to make param buffer handling work correctly after window
 * re-iconisation
 *
 * Revision 1.28  1997/04/01  15:25:04  mjg
 * Changed CopyBufferToHardware to PVROSCopyBufferToHardware.
 *
 * Revision 1.27  1997/03/31  23:40:29  mlj
 * Fixes to get DOS32 driver to build
 *
 * Revision 1.26  1997/03/31  22:09:04  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.25  1997/03/29  22:03:22  mjg
 * Inserted PVROSDelay.
 *
 * Revision 1.24  1997/03/14  11:06:53  jwf
 * changed some stuff to make it compile on the macintosh
 *
 * Revision 1.23  1997/02/19  14:35:59  erf
 * Create large dummy object to fill to 1024 cache boundary to prevent
 * vignetting.
 *
 * Revision 1.22  1997/02/12  12:34:36  erf
 * In AddDummyPlanes() call new translucent pass dummy objects.
 *
 * Revision 1.21  1997/02/04  12:29:09  gdc
 * moved a lot of functionality that was previously in rnrender into here
 * in particular functions common to rnrender.c and sgllite.c are now kept
 * here, and some global definitions were moved into this module
 *
 * Revision 1.20  1997/02/03  10:48:11  gdc
 * removed D3DOPTIM_SGL compile conditional form this module as this is now the
 * only option
 *
 * Revision 1.19  1996/12/10  23:48:39  erf
 * Changed #ifndef D3DOPTIM_SGL to #if !D3DOPTIM_SGL.
 *
 * Revision 1.18  1996/12/10  19:10:05  gdc
 * bracketed InitRegionData call with ifndef D3DOPTIM_SGL, aiming toward
 * D3DOPTIM stuff becoming default build
 *
 * Revision 1.17  1996/12/03  02:09:18  erf
 * Call InitRegionDataL() directly is D3DOPTIM.
 *
 * Revision 1.16  1996/09/12  13:18:51  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES.
 * Shadows may now have more than SGL_MAX_PLANES.
 * (Actually twice as many). re: NHE 044A, 049A.
 *
 * Revision 1.15  1996/03/22  16:51:22  sjf
 * Made shading, texturing, and transformed plane structures global.
 *
 * Revision 1.14  1995/10/19  14:21:33  sjf
 * Now allows independant double/single buffering of texas and sabre params.
 *
 * Revision 1.13  1995/09/28  10:33:10  pdl
 * added the WrappingTransform.
 *
 * Revision 1.12  1995/09/18  20:04:07  sjf
 * Added a flag to indicate if weve processed a light volume yet
 *
 * Revision 1.11  1995/09/15  18:13:08  jrc
 * Added absoluteCoordTransform for position point calculations.
 *
 * Revision 1.10  1995/08/09  12:26:23  pdl
 * added TranslucentControlWord.
 *
 * Revision 1.9  1995/07/30  23:22:56  sjf
 * Fixed brain dead assertion (was wrong way round (logically)).
 *
 * Revision 1.8  1995/07/30  20:52:41  sjf
 * FIXED ****** bug of putting necessary code in an ASSERT.
 *
 * Revision 1.7  1995/07/27  10:57:23  sjf
 * Added state stack init.
 *
 * Revision 1.6  1995/07/26  16:27:15  sjf
 * Added region data init.
 *
 * Revision 1.5  1995/07/25  17:08:55  sjf
 * Added parameter buffer management initialisation.
 *
 * Revision 1.4  1995/07/21  11:41:35  sjf
 * Added dlnodes.h 'cause of default camera addition.
 *
 * Revision 1.3  1995/07/20  18:00:49  sjf
 * added sgl_defs.h
 *
 * Revision 1.2  1995/05/25  14:20:54  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.1  1995/05/11  17:32:30  sjf
 * Initial revision
 *
 *
 *
 **************************************************************************/

#define MODULE_ID MODID_RNGLOBAL

#include "sgl_defs.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "sgl_init.h"
#include "dvdevice.h"
#include "rncamera.h"
#include "pmsabre.h"
#include "rnstate.h"
#include "rnconvst.h"
#include "rnfshade.h"
#include "rnglobal.h"
#include "pktsp.h"
#include "dregion.h"
#include "pvrlims.h"

#include "debug.h"
#include "metrics.h"

#include "pkisp.h"	
#include "pvrosapi.h"
#include "parmbuff.h"
#include "texapi.h"

#include "sgl_api.h"

#if WIN32 || DOS32 || MAC

	#include "brdcfg.h"

#endif

/* JWF added following missing prototype */
#if VIGNETTE_FIX
void	AddRegionOpaqueDummyLargeL(const sgl_uint32  ObjectAddr);
#endif


/*
// Define the arrays of transformed planes, and the shading
// and texturing results of a convex or mesh.
*/
TRANS_PLANE_ARRAY_TYPE 	GlobalTransformedPlanes;
SHADING_RESULT_STRUCT   GlobalShadingResults[SGL_MAX_INTERNAL_PLANES];
TEXTURING_RESULT_STRUCT GlobalTextureResults[SGL_MAX_INTERNAL_PLANES];


/*
// Define the global editing variables
*/
DL_USER_GLOBALS_STRUCT	dlUserGlobals;

/*
// declare the two special projection matrices
*/
static PROJECTION_MATRIX_STRUCT projection_mat;
static LOCAL_PROJECTION_STRUCT	local_projection_mat;


int sgl_render_start=0;
int sgl_render_end=0;
int sgl_render_start_old=0;
int sgl_render_end_old=0;

/*
// Variable to decide if we have initialised the system or not
//    0 - UNinitialised, NonZero - Initialised
//
// Set it to indicate that the system is uninitialised.
*/
int sglSystemInitialised = 0;

sgl_bool bExternalAPI = FALSE;

int nTimeOutTenths = 2;

int HighPrioritySetting = 0;
int DefaultPrioritySetting = 0;

/*
// Flag to indicate if we have processed a light volume
// at all in this render
*/
sgl_bool doneALightVol;

/*
// Default camera for when the user hasn't specified one
*/
static CAMERA_NODE_STRUCT DefaultCamera;


/*/////////////////////////////////////////////////////
// Declare the current light slot variable
//
// This must be intialised at the start of every render
// (to 0 - see TRDD027x.doc)
///////////////////////////////////////////////////////
*/
static int gnCurrentLightSlot;

/*/////////////////////////////////////////////////////
// This stores the current local coordinate system at the point that 
// a wrapping intermediate surface has been defined.
// NOTE that this is not stacked.
///////////////////////////////////////////////////////
*/
static TRANSFORM_STRUCT WrappingTransform;

/*/////////////////////////////////////////////////////
// Camera transform that is used for converting camera-relative world
// coordinates into absolute coordinates for point positions read by
// sgl_query_point.  The inverse matrix and scale values are not used, but
// are retained so we can use the TransformVector function.
///////////////////////////////////////////////////////
*/
static TRANSFORM_STRUCT absoluteCoordTransform;

/**********************************************************************/
sgl_bool RnGlobalProjMatRegionOnScreen(int XRegion, int YRegion)
{
	return (sgl_bool)(XRegion >= projection_mat.FirstXRegion &&
					  XRegion <= projection_mat.LastXRegion &&
					  YRegion >= projection_mat.FirstYRegionExact &&
					  YRegion <= projection_mat.LastYRegionExact);
}

sgl_bool RnGlobalProjMatRegionBoxOnScreen(REGIONS_RECT_STRUCT *Regions)
{
	return (sgl_bool)((int)&Regions->FirstYRegion >= projection_mat.FirstYRegionExact &&
					  (int)&Regions->LastYRegion <= projection_mat.LastYRegionExact &&
					  (int)&Regions->FirstXRegion >= projection_mat.FirstXRegion &&
					  (int)&Regions->LastXRegion  <= projection_mat.LastXRegion);
}

sgl_bool RnGlobalProjMatAllRegionsPositive()
{
	DPFOO((DBG_MESSAGE, "Viewport  X: %d-%d  Y: %d-%d\n",
		   projection_mat.FirstXRegion, projection_mat.LastXRegion,
		   projection_mat.FirstYRegion, projection_mat.LastYRegion));

	return (sgl_bool)(projection_mat.FirstXRegion >= 0 &&
					  projection_mat.LastXRegion  >= 0 &&
					  projection_mat.FirstYRegion >= 0 &&
					  projection_mat.LastYRegion  >= 0 );
}

sgl_bool RnGlobalProjMatRegLastGtRegFirst()
{
	return (sgl_bool)(projection_mat.LastXRegion >= projection_mat.FirstXRegion &&
					  projection_mat.LastYRegion >= projection_mat.FirstYRegion);
}

void RnGlobalCopyProjMat (PROJECTION_MATRIX_STRUCT  *pProjMat)
{
	*pProjMat = projection_mat;
}

void RnGlobalSetProjMat (PROJECTION_MATRIX_STRUCT  *pProjMat)
{
	projection_mat = *pProjMat;
}

PROJECTION_MATRIX_STRUCT *RnGlobalGetProjMat ()
{
	return &projection_mat;
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
 *				   
 **************************************************************************/

#if PCX2 || PCX2_003
float RnGlobalGetFixedClipDist ()
{
	return(projection_mat.f32FixedClipDist);
}
#else
int RnGlobalGetFixedClipDist ()
{
	return(projection_mat.n32FixedClipDist);
}
#endif

/**********************************************************************/

void RnGlobalSetLocalProjMat (LOCAL_PROJECTION_STRUCT  *pLocalProjMat)
{
	local_projection_mat = *pLocalProjMat;
}

sgl_bool RnGlobalLocalProjMatIsValid ()
{
	return (sgl_bool)local_projection_mat.valid;
}

sgl_bool RnGlobalLocalProjMatTextureIsValid (int TexSize)
{
	DPF((DBG_MESSAGE, "This Texture Size:%d, Previous TextureSize:%d",
						TexSize,local_projection_mat.LastTextureSize));

	return (sgl_bool)(local_projection_mat.LastTextureSize == TexSize);
}

LOCAL_PROJECTION_STRUCT	*RnGlobalGetLocalProjMat ()
{
	return &local_projection_mat;
}

void RnGlobalCopyLocalProjMat (LOCAL_PROJECTION_STRUCT  *pLocalProjMat)
{
	*pLocalProjMat = local_projection_mat;
}

/**********************************************************************/

void RnGlobalSetAbsoluteCoordTransform (TRANSFORM_STRUCT *pTransformState)
{
	absoluteCoordTransform = *pTransformState;
}

TRANSFORM_STRUCT *RnGlobalGetAbsoluteCoordTransform ()
{
	return &absoluteCoordTransform;
}

/**********************************************************************/

void RnGlobalSetWrappingTransform (TRANSFORM_STRUCT *pTransformState)
{
	WrappingTransform = *pTransformState;
}

void RnGlobalCopyWrappingTransform (TRANSFORM_STRUCT *pTransformState)
{
	*pTransformState = WrappingTransform;
}

TRANSFORM_STRUCT *RnGlobalGetWrappingTransform ()
{
	return &WrappingTransform;
}

/**************************************************************************
 * Function Name  : RnGlobalSetCurrentLightSlot
 * Inputs         : slot - the value to set
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Does Nothing relly, but helps remove a global -- SGL Only
 *				   
 **************************************************************************/

void RnGlobalSetCurrentLightSlot(int slot)
{
	gnCurrentLightSlot = slot;
}

/**************************************************************************
 * Function Name  : RnGlobalGetCurrentLightSlot
 * Inputs         : None
 * Outputs        : current light slot
 * Input/Output	  : None
 * Returns        : Nothing
 * Global Used    : 
 * Description    : Does Nothing relly, but helps remove a global -- SGL Only
 *				   
 **************************************************************************/

int RnGlobalGetCurrentLightSlot()
{
	return gnCurrentLightSlot;
}

/**************************************************************************
 * Function Name  : GetDefaultCamera
 * Inputs         : Nothing
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : Returns pointer to the default camera
 * Global Used    : 
 * Description    : Does Nothing relly, but helps remove a global -- SGL Only
 *				   
 **************************************************************************/

CAMERA_NODE_STRUCT *GetDefaultCamera()
{
	return(&DefaultCamera);
}

#if !WIN32
/**************************************************************************
 * Function Name  : GetParameterSpace (internal only)
 * Inputs         : 
 * Outputs        : A parameter block
 * Returns        : 
 * Global Used    : 
 * Description    : Calls the hardware parameter space allocation
 *					routine and waits until it gets a valid parameter
 *					block. If there isn't one, it waits until the
 *					current render is complete, and releases that one.
 *	
 **************************************************************************/
extern sgl_bool CALL_CONV HWGetParamMemRefs (PVR_PARAM_BUFF *pPVRParamBuf);

void GetParameterSpace(PVR_PARAM_BUFF pBuff[3])
{
	HWGetParamMemRefs (pBuff);
}
#endif

/**************************************************************************
 * Function Name  : SGLSetGlobal (internal only)
 * Inputs         : enumerated value, void function
 * Outputs        : 
 * Returns        : none
 * Global Used    : 
 * Description    : Sets up global variables used per instance
 *	
 **************************************************************************/

typedef PVRHANDLE HINSTANCE;

DEVICE_TYPE		gDeviceType;
DEVICEID		gDeviceID;
HINSTANCE		gDllHandle;
HLDEVICE        gHLogicalDev;
PTEXAPI_IF		gpTextureIF;

/*
// the translucent control word is the TEXAS control word that
// 'points' to a single white pixel.This is used for non-textured
// translucency.
*/
sgl_uint32 TranslucentControlWord;
/*
// the vertex fog control word is the TEXAS control word that
// 'points' to a graded 16 level translucency texture used
// for per vertex fogging.
*/
sgl_uint32 VertexFogControlWord;

void CALL_CONV SglSetGlobal (SGL_Globals eGlobal, void* pValue)
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
			DPF((DBG_WARNING,"Dll handle is being set!!"));
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
			ASSERT(gHLogicalDev->TexHeap == gpTextureIF->hTexHeap);

			/*	the translucent pixel HAS to be set up after the
				texture memory */
			TranslucentControlWord = ((HTEXHEAP) gHLogicalDev->TexHeap)->TranslucentControlWord;

			VertexFogControlWord = ((HTEXHEAP) gHLogicalDev->TexHeap)->VertexFogControlWord;

			HWSetupBunchOfISPRegs();

			break;
		}
		case SGL_TextureIF:
		{
			gpTextureIF = (PTEXAPI_IF) pValue;
			break;
		}
		case SGL_APIDev:
		{
			if((sgl_uint32)pValue == API_DEV_MAGIC_NUM)
			{
				bExternalAPI = TRUE;
			}
		}
		default:
		{
			DPF ((DBG_ERROR, "Invalid Global specified"));	
			break;
		}
	}
}

/*--------------------------- End of File --------------------------------*/

