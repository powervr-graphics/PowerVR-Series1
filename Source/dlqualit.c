/******************************************************************************
 * Name : dlqualit.c
 * Title : 
 * Author : Simon Fenney
 * Created : 25/09/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : SGL routines for setting quality parameters
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dlqualit.c,v $
 * Revision 1.15  1997/09/11  10:32:31  erf
 * Changed sgl_qual_dithering to sgl_qual_dither(). Looks and feels
 * better.
 *
 * Revision 1.14  1997/09/11  10:21:49  erf
 * Added new function sgl_qual_dithering() to enable or disable hardware
 * dithering.
 *
 * Revision 1.13  1997/06/04  15:31:51  gdc
 * stopped compiler warnings
 *
 * Revision 1.12  1997/05/13  17:53:31  mjg
 * Added sglmem include.
 *
 * Revision 1.11  1997/04/30  19:19:00  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.10  1997/04/29  15:02:58  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.9  1997/04/25  16:30:09  gdc
 * got rid of some more compiler warnings
 *
 * Revision 1.8  1997/04/24  20:55:31  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.7  1997/04/04  13:03:09  jop
 * Added explicit #include "pvrosapi.h"
 *
 * Revision 1.6  1997/04/02  21:27:46  mjg
 * Updated calling conventions.
 *
 * Revision 1.5  1997/03/25  10:20:56  erf
 * Added texture filtering quality routine. Used to set the filtering mode.
 *
 * Revision 1.4  1996/07/11  16:39:38  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.3  1995/11/02  19:48:45  sjf
 * Added fog on/off function to quality.
 *
 * Revision 1.2  1995/11/02  16:55:31  jop
 * Added quality default flags init from ini file
 *
 * Revision 1.1  1995/09/25  18:07:40  sjf
 * Initial revision
 *04
 *
 *
 *****************************************************************************/

#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"
#include "sgl_init.h"
#include "nm_intf.h"
#include "dlglobal.h"

#include "profile.h"
#include "rnstate.h"

#include "pvrosapi.h"
#include "sglmem.h"

static int gfDefQualFlags = 0xFFFFFFFF;

/**************************************************************************
 * Function Name  : QualSet   (internal only routine)
 * Inputs Only    :	WhatToSet  - which feature is being set
 *					enable    - either enable or disable
 *					enable2   - either enable or disable second flag
 *					eFilterType - bilinear filtering setting.
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : creates quality node, and uses the passed parameters to
 *					store values.
 **************************************************************************/

static void QualSet( const QUAL_ENUM WhatToSet,
					 const sgl_bool enable, 
					 const sgl_bool enable2,
					 const sgl_texture_filter_type eFilterType)
{
	QUALITY_NODE_STRUCT * pNode;

	/*	
	// Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
	}
#endif
	/*
	//   Tidy up current transforms etc...
	*/
	DlCompleteCurrentMaterial();
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMesh();


	pNode = NEW(QUALITY_NODE_STRUCT);

	if(pNode == NULL)
	{
		/*
		// Abort	   
		*/
		SglError(sgl_err_no_mem);
	} 

	/*
	// Set the header details
	*/
	pNode->node_hdr.n16_node_type = nt_quality;
	pNode->node_hdr.n16_name	  = NM_INVALID_NAME;


	/*
	// Set the type of data we are setting and the flag
	*/
	pNode->WhatsSet = WhatToSet;
	pNode->Enable	= enable;
	pNode->Enable2	= enable2;
	pNode->eFilterType	= eFilterType;

	/*
	// Add it to the end of the current list
	*/
	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);
	

	/*
	// All went well
	*/
	SglError(sgl_no_err);

}


/**************************************************************************
 * Function Name  : sgl_qual_texture_filter
 * Inputs Only    : enable    - either enable or disable smooth shading
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : Set the current quality filtering setting.
 *
 **************************************************************************/
void CALL_CONV sgl_qual_texture_filter(const sgl_texture_filter_type eFilterType)
{
	QualSet(QFE_TEXTURE_FILTERING, FALSE, FALSE, eFilterType);
}


/**************************************************************************
 * Function Name  : sgl_qual_dither
 * Inputs Only    : enable    - either enable or disable dithering
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : Set the current quality filtering setting.
 *
 **************************************************************************/
void CALL_CONV sgl_qual_dither(const sgl_bool enable)
{
	QualSet(QFE_DITHERING, enable, FALSE, (sgl_texture_filter_type)0);
}


/**************************************************************************
 * Function Name  : sgl_qual_smooth_shading
 * Inputs Only    : enable    - either enable or disable smooth shading
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : see SGL interface spec.  Adds a quality node to the end
 *					of the current display list that enables/disables smooth
 *					shading.
 **************************************************************************/
void CALL_CONV sgl_qual_smooth_shading( const sgl_bool enable )
{
	QualSet(QFE_SMOOTH_SHAD, enable, FALSE, (sgl_texture_filter_type)0);
}


/**************************************************************************
 * Function Name  : sgl_qual_texturing
 * Inputs Only    : enable    - either enable or disable texturing
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : see SGL interface spec.  Adds a quality node to the end
 *					of the current display list that enables/disables textures
 **************************************************************************/
void CALL_CONV sgl_qual_texturing(const sgl_bool enable )
{
	QualSet(QFE_TEXTURE, enable, FALSE, (sgl_texture_filter_type)0);
}

/**************************************************************************
 * Function Name  : sgl_qual_generate_shadows
 * Inputs Only    : enable    - either enable or disable fog
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : see SGL interface spec.  Adds a quality node to the end
 *					of the current display list that enables/disables shadows
 **************************************************************************/
void CALL_CONV sgl_qual_generate_shadows( const sgl_bool enable )
{
	QualSet(QFE_SHADOWS, enable, FALSE, (sgl_texture_filter_type)0);
}

/**************************************************************************
 * Function Name  : sgl_qual_fog
 * Inputs Only    : enable    - either enable or disable fogging
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : see SGL interface spec.  Adds a quality node to the end
 *					of the current display list that enables/disables fogging
 **************************************************************************/
void CALL_CONV sgl_qual_fog( const sgl_bool enable )
{
	QualSet(QFE_FOG, enable, FALSE, (sgl_texture_filter_type)0);
}


/**************************************************************************
 * Function Name  : sgl_qual_collision_detection
 * Inputs Only    : enable    - either enable or disable collision detect
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : Current display list
 * Description    : see SGL interface spec.  Adds a quality node to the end
 *					of the current display list that enables/disables cool detect
 **************************************************************************/
void CALL_CONV sgl_qual_collision_detection( const sgl_bool enable,
											 const sgl_bool enable_offscreen )
{
	QualSet(QFE_COLLISIONS, enable, enable_offscreen, (sgl_texture_filter_type)0);
}

/**************************************************************************
 * Function Name  : GetDefaultQualityFlags
 * Inputs Only    : NONE
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : gnDefQualFlags
 * Description    : 
 **************************************************************************/
int GetDefaultQualityFlags (void)
{
	return (gfDefQualFlags);
}

/**************************************************************************
 * Function Name  : InitDefaultQualityFlags
 * Inputs Only    : NONE
 * Outputs Only   : NONE
 * Input/Output   : NONE
 * Returns        : NONE
 *
 * Globals Used	  : gnDefQualFlags
 * Description    : Set up default quality flags - read values from sgl.ini
 **************************************************************************/
void InitDefaultQualityFlags (void)
{
	sgl_bool bSmoothShade = 		SglReadPrivateProfileInt ("Quality", "SmoothShading", 1, "sgl.ini");
	sgl_bool bTexture = 			SglReadPrivateProfileInt ("Quality", "Textures", 1, "sgl.ini");
	sgl_bool bShadows = 			SglReadPrivateProfileInt ("Quality", "Shadows", 1, "sgl.ini");
	sgl_bool bCollisions = 			SglReadPrivateProfileInt ("Quality", "Collisions", 1, "sgl.ini");
	sgl_bool bOffScreenCollisions = SglReadPrivateProfileInt ("Quality", "OffscreenCollisions", 1, "sgl.ini");
	sgl_bool bDithering =			SglReadPrivateProfileInt ("Quality", "Dithering", 1, "sgl.ini");

	gfDefQualFlags = 0;

	if (bSmoothShade)
	{
		gfDefQualFlags |= qf_smooth_shading;
	}

	if (bTexture)
	{
		gfDefQualFlags |= qf_textures;
	}

	if (bShadows)
	{
		gfDefQualFlags |= qf_cast_shadows;
	}

	if (bCollisions)
	{
		gfDefQualFlags |= qf_full_collision;
	}

	if (bOffScreenCollisions)
	{
		gfDefQualFlags |= qf_onscreen_collision;
	}

	if (bDithering)
	{
		gfDefQualFlags |= qf_dithering;
	}

	/*
	// We don't set the Fog value here, because the initial setting depends
	// on what the camera settings are
	*/
}

/*
//  ******    END OF FILE    ******
*/


