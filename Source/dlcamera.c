/******************************************************************************
 * Name         : dlcamera.c
 * Title        : Camera display list editing SGL functions.
 * Author       : John Catchpole
 * Created      : 01/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dlcamera.c,v $
 * Revision 1.24  1997/07/14  19:27:09  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.23  1997/06/04  14:08:19  gdc
 * removed compiler warnings
 *
 * Revision 1.22  1997/05/13  17:50:39  mjg
 * Replaced PVROS memory management call with an SGL one.
 *
 * Revision 1.21  1997/04/30  19:17:53  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.20  1997/04/29  14:41:14  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.19  1997/04/24  20:44:26  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.18  1997/04/02  21:26:16  mjg
 * Updated calling conventions.
 *
 * Revision 1.17  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.16  1997/01/27  13:34:21  erf
 * Set fast fog colour value here to prevent scaling on every render.
 *
 * Revision 1.15  1996/10/11  12:07:54  msh
 * "Small number" checks modified for Midas Arcade
 *
 * Revision 1.14  1996/09/24  15:25:04  sjf
 * Hopefully, I've made the bounds on the camera zoom factor tighter
 * so that the system won't get its proverbials in a twist. NECHE reported
 * that a zoom factor of 0.1 (Ridiculously wide angled!) caused problems.
 *
 * Revision 1.13  1996/03/22  11:09:05  jrc
 * Removed some unused #define constants.
 *
 * Revision 1.12  1996/03/19  17:10:28  pdl
 * changed opacity to alpha
 *
 * Revision 1.11  1995/08/18  17:04:18  sjf
 * Added correct fogging.
 *
 * Revision 1.10  1995/08/01  11:26:04  sjf
 * Fixed bug in camera setup - was setting parent list to the
 * parents parent.
 *
 * Revision 1.9  1995/07/27  10:55:09  sjf
 * Fixed bug : foreground distance wasn't set up.
 *
 * Revision 1.8  1995/07/21  09:40:07  sjf
 * Removed aspect factor as no longer part of camera spec.
 *
 * Revision 1.7  1995/06/26  09:15:35  jrc
 * Removed sgl.h from #includes because it is now included by sgl_defs.h
 *
 * Revision 1.6  1995/06/13  12:06:12  jrc
 * No longer assume that NULL is zero.
 *
 * Revision 1.5  1995/06/07  16:03:58  jrc
 * 1) Added fog and background colour functionality.
 * 2) Improved the range check macro (now called MESSAGE_CLAMP).
 *
 * Revision 1.4  1995/06/06  15:37:04  jrc
 * Changed WriteCameraData to WriteData since it is a static function.
 *
 * Revision 1.3  1995/06/06  11:54:32  jrc
 * Made internal function static.
 *
 * Revision 1.2  1995/06/06  10:21:10  jrc
 * Fixed so it now compiles ok.
 *
 * Revision 1.1  1995/06/06  10:14:53  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_DLCAMERA

#include <math.h>
#include <limits.h>

#include "sgl_defs.h"
#include "sgl_init.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"

#define INV_LOG_OF_2 (1.442695041f)


#define MESSAGE_CLAMP(out,in,min,max,name)\
  if ((in) < (min))\
  {\
      DPF(( DBG_WARNING,"%s was too small.",(name) ));\
  }\
  else if ((in) > (max))\
  {\
      DPF(( DBG_WARNING,"%s was too large.",(name) ));\
  }\
  (out) = CLAMP( (in),(min),(max) )

#if PCX2 || PCX2_003
/* If PCX2 drivers and hardware need to set fog colour.
 */
extern sgl_colour	cFastFogColour;
#endif



/******************************************************************************
 * Function Name: WriteData (static)
 *
 * Inputs       : pNode,fZoomFactor,fForegound,fInvBackground
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Installs the given camera information, adjusting it to the
 *                requirements of the render traversal calculations.
 *
 *                pNode:            The destination camera display list node
 *                                  structure.
 *                other parameters: The new camera information.
 *****************************************************************************/
static void WriteData(CAMERA_NODE_STRUCT *pNode, float fZoomFactor,
						float fForeground, 
						float fInvBackground)
{
	ASSERT(pNode)

	MESSAGE_CLAMP(pNode->zoom_factor, fZoomFactor, 0.4f, 200.0f,
	  "Zoom factor");
	  
	MESSAGE_CLAMP(pNode->foreground_dist, fForeground, 1.0E-6f,
					1.0E+10f, "Foregound dist");

	DPF((DBG_MESSAGE," Not sure of limits on foreground distance"));

	MESSAGE_CLAMP(pNode->inv_background_dist, fInvBackground, 0.0f,
	  1.0f/pNode->foreground_dist, "Background distance");
}


/******************************************************************************
 * Function Name: sgl_create_camera
 *
 * Inputs       : fZoomFactor,fAspectFactor,fForegound,fInvBackground
 * Outputs      : -
 * Returns      : name or error value
 * Globals Used : dlUserGlobals.pNamtab, dlUserGlobals.pCurrentList
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_create_camera( float fZoomFactor,
								 float fForeground,
								 float fInvBackground )
{
    /*
	// -----------
	// INITIALISE:
	// -----------
	*/
    CAMERA_NODE_STRUCT *pNode;

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	/*
	// ----------
	// MAKE NODE:
	// ----------
	*/
    pNode = NEW(CAMERA_NODE_STRUCT);
	if (pNode == NULL)
	{
		return SglError(sgl_err_no_mem);
	}

    pNode->node_hdr.n16_node_type = nt_camera;
    pNode->node_hdr.n16_name =
      AddNamedItem(dlUserGlobals.pNamtab, pNode, nt_camera);
	if (pNode->node_hdr.n16_name == sgl_err_no_name)
	{
		SGLFree(pNode);
		return SglError(sgl_err_no_name);
	}
	
	/*
	// Save a pointer to the cameras list
	*/
	pNode->pparent = dlUserGlobals.pCurrentList;

    WriteData(pNode,fZoomFactor, fForeground,fInvBackground);

	pNode->backgroundColour[0] = 0.0f;
	pNode->backgroundColour[1] = 0.0f;
	pNode->backgroundColour[2] = 0.0f;

	/*
	// pack the default fog colour
	*/
	pNode->FogCol.red	 =	0;
	pNode->FogCol.green  =	0;
	pNode->FogCol.blue	 =	0;
	pNode->FogCol.alpha  =	0;

	/*
	// store the default fog density values, so that we get NO
	// fog- use a very negative number for the power.
	*/ 
	pNode->invlogfogFraction = 1.0f;
	pNode->logfogPower = (INT_MIN / 2); 


	/*
	// ------------
	// ADD TO LIST:
	// ------------
	*/
    AppendNodeToList(dlUserGlobals.pCurrentList, pNode);

    /*
	// -------
	// RETURN:
	// -------
	*/
    return SglError(pNode->node_hdr.n16_name);
}


/******************************************************************************
 * Function Name: sgl_get_camera
 *
 * Inputs       : nName
 * Outputs      : fpZoomFactor,fpAspectFactor,fpForegound,fpInvBackground
 * Returns      : error status
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_get_camera( int nName, float *fpZoomFactor, 
 							  float *fpForeground, float *fpInvBackground )
{
    CAMERA_NODE_STRUCT *pNode;

	ASSERT(fpZoomFactor)
	ASSERT(fpForeground)
	ASSERT(fpInvBackground)

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif
    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_camera)
	{
        return SglError(sgl_err_bad_name);
	}

    pNode = GetNamedItem(dlUserGlobals.pNamtab, nName);

	*fpZoomFactor    = pNode->zoom_factor;
	*fpForeground    = pNode->foreground_dist;
	*fpInvBackground = pNode->inv_background_dist;

    return SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_set_camera
 *
 * Inputs       : nName, fZoomFactor,fAspectFactor,fForegound,fInvBackground
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_camera( int nName, float fZoomFactor,
							   float fForeground, float fInvBackground )
{
    CAMERA_NODE_STRUCT *pNode;

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_camera)
    {
		SglError(sgl_err_bad_name);
		return;
	}

    pNode = GetNamedItem(dlUserGlobals.pNamtab, nName);

    WriteData(pNode,fZoomFactor,fForeground,fInvBackground);

    SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_set_fog
 *
 * Inputs       : nCamera,colour,density
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_fog( int nCamera, sgl_colour colour, 
							float density )
{
	CAMERA_NODE_STRUCT *pNode;

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nCamera) != nt_camera)
    {
		SglError(sgl_err_bad_name);
		return;
	}

    pNode = GetNamedItem(dlUserGlobals.pNamtab, nCamera);
	ASSERT(pNode)

	ASSERT(colour)


	/*
	// pack the fog colour
	*/
	pNode->FogCol.red	 =	(int) (255.0 * CLAMP(colour[0], 0.0f, 1.0f));
	pNode->FogCol.green  =	(int) (255.0 * CLAMP(colour[1], 0.0f, 1.0f));
	pNode->FogCol.blue	 =	(int) (255.0 * CLAMP(colour[2], 0.0f, 1.0f));
	pNode->FogCol.alpha  =	0; /*This shouldnt matter*/
	
#if PCX2 || PCX2_003
	/* Set fast fog value for background plane.
	 */
	cFastFogColour[0] = CLAMP(colour[0], 0.0f, 1.0f);
	cFastFogColour[1] =	CLAMP(colour[1], 0.0f, 1.0f);
	cFastFogColour[2] = CLAMP(colour[2], 0.0f, 1.0f);
#endif

	/*
	// Get 1-fog density and clip to legal range
	*/
	density = CLAMP((1.0f - density), 0.0f, 1.0f);

	/*
	// Get the log of this to the base 2, but check that we CAN actually
	// get this value
	*/
  #if defined (MIDAS_ARCADE)

	/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
	if(density > 1.0E-3)

  #else

	if(density > 1.0E-20)

  #endif
	{
		/*
		// Get the log and negate it
		*/
		density = -(float)log(density) * INV_LOG_OF_2;
		/*
		// Get the fraction and power of two from this...
		*/
		pNode->invlogfogFraction = (float)frexp(density, & pNode->logfogPower);

		/*
		// if we got zero, then the fog density is Very low, set the
		// shift value to be extremely negative
		*/
		if(pNode->invlogfogFraction == 0.0)
		{
			pNode->invlogfogFraction = 1.0f;
			pNode->logfogPower = -1000;
		}
		/*
		// Else it is in the normal sort of range
		*/
		else
		{
			pNode->invlogfogFraction = 1.0f/pNode->invlogfogFraction;
		}
	}
	/*
	// Else the fog denisty is VERY high, so set the shift value high
	*/
	else
	{
		pNode->invlogfogFraction = 1.0f;
		pNode->logfogPower = 1000;
	}

		

	SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_set_background_colour
 *
 * Inputs       : nCamera,colour
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_background_colour( int nCamera, sgl_colour colour )
{
	CAMERA_NODE_STRUCT *pNode;

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif

    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nCamera) != nt_camera)
    {
		SglError(sgl_err_bad_name);
		return;
	}

    pNode = GetNamedItem(dlUserGlobals.pNamtab, nCamera);
	ASSERT(pNode)

	ASSERT(colour)
	MESSAGE_CLAMP(pNode->backgroundColour[0], colour[0], 0.0f, 1.0f,
	  "Background colour");
	MESSAGE_CLAMP(pNode->backgroundColour[1], colour[1], 0.0f, 1.0f,
	  "Background colour");
	MESSAGE_CLAMP(pNode->backgroundColour[2], colour[2], 0.0f, 1.0f,
	  "Background colour");

	SglError(sgl_no_err);
}

/*------------------------------- End of File -------------------------------*/
