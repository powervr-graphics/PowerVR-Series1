/******************************************************************************
 * Name : dvdevice.c
 * Title : 
 * Author : Stel Michael
 * Created : 22/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : SGL Routines for creating and editing device nodes
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dvdevice.c,v $
 * Revision 1.52  1997/11/20  15:36:21  sxy
 * moved the function OutputTotalMetric() to metrics.c
 *
 * Revision 1.51  1997/11/05  14:57:18  sxy
 * fixed a bug for OutputTotalMetric when the total render time is 0.
 *
 * Revision 1.50  1997/10/24  15:44:50  sxy
 * changed timing unit into millisecond.
 *
 * Revision 1.49  1997/10/22  17:18:56  sxy
 * added tow functions for timing metrics in developer's driver.
 *
 * Revision 1.48  1997/09/24  10:40:27  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values
 *
 * Revision 1.47  1997/07/25  09:10:48  gdc
 * chnaged app hint name to be move obvious
 *
 * Revision 1.46  1997/07/25  09:04:09  sks
 * Mech 2 fix - increases x and y device size by 1 via
 * App Hint
 *
 * Revision 1.45  1997/07/19  16:22:32  mjg
 * Did a passing optimisation (maybe) and re-formated some DPFs
 *
 * Revision 1.44  1997/06/24  18:03:51  gdc
 * fixed last thing
 *
 * Revision 1.43  1997/06/24  16:27:04  gdc
 * changed cast
 *
 * Revision 1.42  1997/06/24  16:26:22  gdc
 * put an app hint override called DisplayDoubleBuffer to force
 * double buffering
 *
 * Revision 1.41  1997/05/28  10:21:31  mjg
 * Added call to SaveDeviceName.
 *
 * Revision 1.40  1997/05/13  17:54:11  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.39  1997/05/10  13:46:57  mjg
 * Moved a PVROSFree statement.
 *
 * Revision 1.38  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.37  1997/04/30  19:19:18  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.36  1997/04/29  14:46:33  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.35  1997/04/24  21:00:25  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.34  1997/04/09  17:15:22  gdc
 * ... and fixed my typo
 *
 * Revision 1.33  1997/04/09  17:07:18  gdc
 * removed refernece to projection_mat global
 *
 * Revision 1.32  1997/04/08  13:54:17  jop
 * Cleaned up includes a bit
 *
 * Revision 1.31  1997/04/04  17:39:56  gdc
 * removed gfRegionScales as unique globals and stuffed in the global
 * projection mat struct
 *
 * Revision 1.30  1997/04/02  21:28:31  mjg
 * Updated calling conventions.
 *
 * Revision 1.29  1997/04/01  19:16:03  mjg
 * Removed mydpf and replaced it with PVROSPrintf
 *
 * Revision 1.28  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.27  1997/03/14  11:06:53  jwf
 * changed some stuff to make it compile on the macintosh
 *
 * Revision 1.26  1997/02/21  20:46:45  mjg
 * Removed the fairly nasty RegionMask reversal and the UNIX compiler warnings.
 *
 * Revision 1.25  1997/02/06  14:23:33  mjg
 * Forced the regions to be created and reset during create screen device.
 *
 * Revision 1.24  1997/02/04  10:30:04  gdc
 * Replaced call to RnMeshAtListTraversal with guts of function, as the func
 * name is a bit confusing - it is actually called on device creation not
 * list traversal. Also moved the defintion of the to globals it sets to this
 * module from rnmesh.c
 *
 * Revision 1.23  1997/02/03  11:09:29  gdc
 * did properly what I just said I'd done
 *
 * Revision 1.22  1997/02/03  10:24:11  gdc
 * removed D3DOPTIM_SGL conditional as this is now the only variant
 *
 * Revision 1.21  1997/01/10  19:46:02  mjg
 * Reversed the regionMask for optimised SGL viewport subtraction.
 *
 * Revision 1.20  1997/01/07  11:40:05  ncj
 * Fixed arcade build
 *
 * Revision 1.19  1997/01/06  16:36:55  jrc
 * Fixed bug where for some reason we were decrementing the X and Y
 * dimensions of the created device by one.  (In practice this led to a blank
 * pixel line at the bottom of the screen.)
 *
 * Revision 1.18  1996/12/10  23:38:39  erf
 * Changed PVR1 to ISPTSP.
 *
 * Revision 1.17  1996/11/18  17:04:08  jop
 * Removed // comment
 *
 * Revision 1.16  1996/11/15  09:17:59  rgi
 * Reset the end of line and carriage returns inserted previously.
 * Added support for METRIC2 level information in DLL_METRIC case.
 *
 * Revision 1.15  1996/11/04  11:21:50  rgi
 * Added DLL_METRIC compile option that builds a self dumping
 * feature into the SGL DLL. This is useful when running with
 * standard D3D performance tools et al.
 *
 * Revision 1.14  1996/10/09  18:30:51  jrc
 * Moved mesh region size setup from each display list traversal to within
 * sgl_create_screen_device call.  The previous scenario caused pauses and
 * crashes and slowdowns on some machines.
 *
 * Revision 1.13  1996/08/13  13:08:45  jop
 * Should fix mouse corruption and d3d bugs
 *
 * Revision 1.12  1996/07/12  14:26:55  sjf
 * Put #if's around the count of regions as that's only needed for
 * PVR1
 *
 * Revision 1.11  1996/07/12  13:48:33  sjf
 * Fixed bug in calculating the number of regions in the default
 * viewport of a device. This was introduced in the code to allow
 * viewport masking.
 *
 * Revision 1.10  1996/07/11  15:55:51  sjf
 * 1) Corrected a bug where incorrect device id was being passed in
 * 2) Added a count of the number of regions in a viewport. This was needed
 *    in PVR1 to support the region masking.
 *
 * Revision 1.9  1996/07/11  10:59:14  sjf
 * Added sgl_subtract_viewport and the region masking code.
 *
 * Revision 1.8  1995/07/30  20:52:10  sjf
 * Changed debug output.
 *
 * Revision 1.7  1995/07/28  14:41:39  sm
 * *** empty log message ***
 *
 * Revision 1.6  1995/07/27  11:56:56  sm
 * added ref to device for defualt viewport
 *
 * Revision 1.6  1995/07/27  11:56:56  sm
 * added ref to device for defualt viewport
 *
 * Revision 1.5  1995/07/26  14:57:21  sm
 * added hwregion header
 *
 * Revision 1.4  1995/07/21  09:41:05  sjf
 * Changed virtual screen to camera rect in line with new SGL spec.
 *
 * Revision 1.3  1995/07/20  16:25:46  sm
 * added default cam rect for device.
 *
 * Revision 1.2  1995/07/20  11:03:18  sm
 * added virtual screen which I'm now about to change
 *
 * Revision 1.1  1995/06/23  17:12:46  sm
 * Initial revision
 *
 *
 *****************************************************************************/



#define MODULE_ID MODID_DVDEVICE 


#include <stdio.h>

#include "sgl.h"
#include "sgl_defs.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "rnglobal.h"
#include "sgl_init.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "pmsabre.h"
#include "dvdevice.h"
#include "hwinterf.h"
#include "dregion.h"	/* JWF added to fix missing prototypes */


#ifdef DLL_METRIC
#define SGL_APP 1
#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/*static int nRendersRecorded = -10;*/
extern void OutputTotalMetric(void);

static void OutputStats( void )
{
	FILE *fp;
	int YReg;

	/* End recorded time */
	SGL_TIME_STOP(TOTAL_APP_TIME)

#if ! defined (MIDAS_ARCADE) /* No file system on an arcade machine */
	fp = fopen("metrics.csv","wt");
#endif

	/* Dump out the data */
	for ( YReg = 0; YReg < NUM_TIMERS; YReg++ )
	{
		if ( Times[YReg].Count != 0 )
		{
#if defined (MIDAS_ARCADE)
			fprintf(stderr, "%s, %d, %d, %d\n", timer_names[YReg].name,
								(int)Times[YReg].Count, (int)Times[YReg].Total, (int)Times[YReg].Max);
#else
			PVROSPrintf("%s, %d, %d, %d\n", timer_names[YReg].name,
						 Times[YReg].Count, Times[YReg].Total, Times[YReg].Max);
#endif
		}
	}

#if ! defined (MIDAS_ARCADE)
	fclose(fp);
#endif
}

void InResetRegionData( void )
{
   	SGL_TIME_STOP(SGLTRI_STARTOFFRAME1_TIME)
#if 0	
	if ( ++nRendersRecorded == 0 )
	{
		/* Set it to zero after the first few frames */
		memset( &Times, 0, NUM_TIMERS*sizeof(Temporal_Data) );

		/* Start recorded time */
	   	SGL_TIME_START(TOTAL_APP_TIME)
	}
	else
#endif
	{
		/* Monitor each frame period as the top level call */
	  	SGL_TIME_STOP(TOTAL_APP_TIME)
		SGL_TIME_START(TOTAL_APP_TIME)	
	}
	SGL_TIME_START(SGLTRI_STARTOFFRAME2_TIME)
}
#else
#include "metrics.h"	
#endif

extern void SaveDeviceName(int name);

/**************************************************************************
 * Function Name  : sgl_create_screen_device
 * Inputs         : device_number
					x_dimension
					y_dimension
					device_mode
					double_buffer 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : logical device name or error
 * Global Used    : 
 * Description    : creates a new device
 *
 **************************************************************************/


int	CALL_CONV sgl_create_screen_device ( int device_number, 
						 				 int x_dimension, int y_dimension,
										 sgl_device_colour_types device_mode, 
										 sgl_bool double_buffer)
{
	int error = sgl_no_err;
	int name  = NM_INVALID_NAME;
	int y;
	DEVICE_NODE_STRUCT * pNode;
	float XCentre,YCentre;
	sgl_int32 Mech2 = 0;
#if ISPTSP
	DEVICE_REGION_INFO_STRUCT RegionInfo;
#endif
  	/*	
		Initialise sgl if this hasn't yet been done		
	*/

#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		error = sgl_err_failed_init;

		SglError(error);
		return error;
	}
#endif

	/* old apps didn't set their double buffering correctly as they 
	** always got double buffering - allow override potentially
	*/
	double_buffer = (sgl_bool)HWRdValFileUInt("DisplayDoubleBuffer", double_buffer);

	/* Fix for Mech2 etc to make the screen the correct size */
	Mech2 = HWRdValFileInt("IncScreenXYby1", Mech2);

	if (Mech2)
	{
		x_dimension++;  
		y_dimension++;
	}

	if ( HWCreateDevice( &device_number, 
						 &x_dimension, &y_dimension, 
						 &device_mode, &double_buffer) !=0 )
	{
		/*
		//	We failed to create_device
		*/

		error = sgl_err_bad_device;

		SglError(error);
		return error;
	} 

	/*
	// Set the region size once the device has been created.  This function
	// name needs changing sometime (It used to be called before each render).
	*/
	/*RnMeshAtListTraversal(); -- guts of function now here instead of call */
	{
		int RegXSize, RegYSize;
		PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

		HWGetRegionSize (&RegXSize, &RegYSize);

		pProjMat->fRegionXScale = 1.0f / (float) RegXSize;
	
		/* For optimisation absolute Y co-ordinates required.
		 */
		pProjMat->fRegionYScale = 1.0f;
	}

	/* 
	//	Create a new device structure 
	*/

	pNode = NEW(DEVICE_NODE_STRUCT);

	if(pNode == NULL)
	{
		/*
			Abort	   
		*/
		error = sgl_err_no_mem;

		SglError(error);
		return error;
	} 


	/*
 		we need to generate a name, and add the device to
		the name table 
	*/
	name = AddNamedItem(dlUserGlobals.pNamtab,
   		   				pNode,
	   					nt_device);

	/*
	  	If there were no free spaces, then the name will
	  	contain an error value (i.e.negative)
		
	  	In this situation TIDY UP and ABORT.
	*/

   	if(name < 0)
	{
   		error = name;
	   	name = NM_INVALID_NAME;

	   	SGLFree(pNode);

	   	SglError(error);

	   	return error;
	}


	/* set fields in device node to the values set by the hardware device routine */

	
	pNode->node_hdr.n16_node_type = (sgl_int16) nt_device;
	pNode->node_hdr.n16_name	  = (sgl_int16) name;
	pNode->node_hdr.next_node	  = NULL;

	pNode->pLastViewport = NULL; 
	pNode->PhDeviceID = device_number;
	pNode->xDim = x_dimension;
	pNode->yDim = y_dimension;
	pNode->DeviceMode = device_mode;
	pNode->DoubleBuffer = double_buffer;



	/*
	// Set up the viewport as well
	*/
	pNode->defaultViewport.Left = 0;
	pNode->defaultViewport.Top = 0;
	pNode->defaultViewport.Right = x_dimension -1;
	pNode->defaultViewport.Bottom = y_dimension -1;




	/*
	// Set the region mask so that all regions are included
	*/
	for(y = 0; y < MAX_Y_REGIONS; y++)
	{
		pNode->defaultViewport.regionMask[y] = 0xFFFFFFFF;
	}

	/*
	// Also actually set the number of regions covered. This is needed for
	// Midas3.
	//
	// NOTE We need to allow for partial regions, hence the addition of
	/ the region size -1
	*/
#if ISPTSP
	HWGetRegionInfo(device_number, &RegionInfo);
	pNode->defaultViewport.numRegions = 
				((x_dimension + RegionInfo.XSize -1) / RegionInfo.XSize) *
				((y_dimension + RegionInfo.YSize -1) / RegionInfo.YSize);
#endif

 /* just is case some wierdo wants tall thin device (ooh err!) */
	if (x_dimension >= y_dimension)
	{			
		XCentre = ((float)(x_dimension - 2))* 0.5f; 
		YCentre = ((float) y_dimension)		* 0.5f;
		
		pNode->defaultViewport.fCamLeft = XCentre-YCentre;
		pNode->defaultViewport.fCamTop = 0.0f;
		pNode->defaultViewport.fCamRight = XCentre+YCentre;
		pNode->defaultViewport.fCamBottom = (float) (y_dimension - 1);
	}
	else
	{
		XCentre = ((float)(x_dimension))	* 0.5f; 
		YCentre = ((float)(y_dimension - 2))* 0.5f; 

		pNode->defaultViewport.fCamLeft = 0.0f;
		pNode->defaultViewport.fCamTop = XCentre-YCentre;
		pNode->defaultViewport.fCamRight = (float) (x_dimension - 1);
		pNode->defaultViewport.fCamBottom = XCentre+YCentre;
	}

	DPF((DBG_MESSAGE, 
		"******** VLEFT:   %7.02f ********",pNode->defaultViewport.fCamLeft));
	DPF((DBG_MESSAGE, 
		"******** VTOP:    %7.02f ********",pNode->defaultViewport.fCamTop));
	DPF((DBG_MESSAGE, 
		"******** VRIGHT:  %7.02f ********",pNode->defaultViewport.fCamRight));
	DPF((DBG_MESSAGE, 
		"******** VBOTTOM: %7.02f ********",pNode->defaultViewport.fCamBottom));

	pNode->defaultViewport.pParentDevice = pNode;

	/* Force the regions to be cleared and created at this point, in case 
     * the application has changed screen mode between renders.
     */

	ResetRegionDataL(TRUE);
	
	SaveDeviceName(name);

	SglError(error);

	return name;
}


  


/**************************************************************************
 * Function Name  : sgl_get_device
 * Inputs         :	device_name
 * Outputs        : 
 * Input/Output	  : device_number
					x_dimension
					y_dimension
					device_mode
					double_buffer 
 * Returns        : error
 * Global Used    : 
 * Description    : returns setting for a specified device
 *
 **************************************************************************/

int	CALL_CONV sgl_get_device ( int device_name, int *device_number, 
							   int *x_dimension, int *y_dimension,
							   sgl_device_colour_types	* device_mode, 
							   sgl_bool *double_buffer )
{
	DEVICE_NODE_STRUCT * pNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return(sgl_err_failed_init);
	}
#endif

	/* Make sure that given name is the name of a device */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, device_name) != nt_device )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return(sgl_err_bad_name); 
	}


	/*	get reference to device from the name table */

    pNode = GetNamedItem(dlUserGlobals.pNamtab, device_name);


	*device_number = pNode->PhDeviceID;
	*x_dimension = pNode->xDim;
	*y_dimension = pNode->yDim;
	*device_mode = pNode->DeviceMode;
	*double_buffer = pNode->DoubleBuffer;


	SglError(sgl_no_err);

	return(sgl_no_err);
}



 
 


/**************************************************************************
 * Function Name  : sgl_delete_device
 * Inputs         :	device
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : 
 * Description    : deletes a device along with all its viewports
 *
 **************************************************************************/


void CALL_CONV sgl_delete_device( int device )
{

	DEVICE_NODE_STRUCT * dNode;
	VIEWPORT_NODE_STRUCT *vNode;
	DL_NODE_STRUCT	*nextNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/

		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/* Make sure that given name is the name of a device */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, device) != nt_device )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 

	}


	/*	get reference to device from the name table */

    dNode = GetNamedItem(dlUserGlobals.pNamtab, device);


   	nextNode = (DL_NODE_STRUCT*) dNode->node_hdr.next_node;

  	/* delete all the viewports */

	while (nextNode!=NULL)
	{
		vNode = (VIEWPORT_NODE_STRUCT*) nextNode;

	   	nextNode = nextNode->next_node;

	    DeleteNamedItem(dlUserGlobals.pNamtab,vNode->node_hdr.n16_name);

		SGLFree(vNode);
	}

	/*
		Remove from the name table
	*/

    DeleteNamedItem(dlUserGlobals.pNamtab,device);

	HWDeleteDevice(dNode->PhDeviceID);
#ifdef DLL_METRIC
	OutputTotalMetric();
   /*	OutputStats();*/
#endif

	SGLFree(dNode);
}





/**************************************************************************
 * Function Name  : sgl_create_viewport
 * Inputs         :	parent_name
					left
					top
					right
					bottom
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : viewport name or error
 * Global Used    : 
 * Description    : creates a viewport for a given device
 *
 **************************************************************************/

int CALL_CONV sgl_create_viewport( int parent_device, 
								   int left, int top, 
								   int right, int bottom, 
								   float cam_rect_left, float cam_rect_top,	
								   float cam_rect_right, float cam_rect_bottom )
{
	int error = sgl_no_err;
	DEVICE_NODE_STRUCT * dNode;
	VIEWPORT_NODE_STRUCT * vNode;
	int name  = NM_INVALID_NAME;
	int y;
	sgl_uint32 XRegionBits;
	DEVICE_REGION_INFO_STRUCT RegionInfo;
  	/*	
		Initialise sgl if this hasn't yet been done		
	*/

#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return(sgl_err_failed_init);
	}
#endif

	/* Make sure that given device name is the name of a device */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, parent_device) != nt_device )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return(sgl_err_bad_name); 
	}


	/*	get reference to device from the name table */

    dNode = GetNamedItem(dlUserGlobals.pNamtab, parent_device);


	/* 
	//	Create a new viewport structure 
	*/

	vNode = NEW(VIEWPORT_NODE_STRUCT);

	if(vNode == NULL)
	{
		/*
			Abort	   
		*/
		error = sgl_err_no_mem;

		SglError(error);
		return (error);
	} 


	/*
 	//	we need to generate a name for the viewport
	*/
	name = AddNamedItem(dlUserGlobals.pNamtab,
   		   				vNode,
	   					nt_viewport);

	/*
	//  	If there were no free spaces, then the name will
	//  	contain an error value (i.e.negative)
	//
	*/

   	if(name < 0)
	{
   		error = name;
	   	name = NM_INVALID_NAME;

	   	SGLFree(vNode);

	   	SglError(error);

	   	return (error);
	}



	/* if viewport size isnt available for device, viewport coordinates
	   are changed to ones that are in hardware routine */

	HWSetViewport(dNode->PhDeviceID, &left,&top,&right,&bottom);


	vNode->node_hdr.n16_node_type = (sgl_int16) nt_viewport;
	vNode->node_hdr.n16_name	  = (sgl_int16) name;
	vNode->node_hdr.next_node	  = NULL;

	vNode->Left = left;
	vNode->Top = top;
	vNode->Right = right;
	vNode->Bottom = bottom;

	vNode->fCamLeft = cam_rect_left;
	vNode->fCamTop = cam_rect_top;
	vNode->fCamRight = cam_rect_right;
	vNode->fCamBottom = cam_rect_bottom;

	vNode->pParentDevice = dNode;


	/* ///////////////////////////
	// Set the region mask so that all regions within this viewport are
	// included, but no others.
	/////////////////////////// */
	HWGetRegionInfo(dNode->PhDeviceID, &RegionInfo);

	/*
	// First clear out the structure
	*/
	for(y = 0; y < MAX_Y_REGIONS; y++)
	{
		vNode->regionMask[y] = 0;
	}

	/*
	// Calc the bits for the x dimension. First set them all, then clear off the
	// lower ones, then the upper ones
	*/
	XRegionBits = 0xFFFFFFFFUL;
	XRegionBits = XRegionBits >> (left / RegionInfo.XSize);
	XRegionBits = XRegionBits << (left / RegionInfo.XSize + 
								  (31 - right / RegionInfo.XSize));

	XRegionBits = XRegionBits >> (31 - right / RegionInfo.XSize);

	/*
	// Also actually set the number of regions covered. This is needed for
	// Midas3
	*/
#if ISPTSP
	vNode->numRegions = ((right / RegionInfo.XSize) - 
						(left / RegionInfo.XSize) + 1) * 

						((bottom / RegionInfo.YSize) - 
						 (top / RegionInfo.YSize) + 1);
#endif

	/*
	// Now put this in all the rows. Note that bottom is set to the last
	// pixel of the last row of regions, and hence the comparison must
	// be "<="
	*/
	
	for(y = (top / RegionInfo.YSize); y <= (bottom / RegionInfo.YSize); y++)
	{
		vNode->regionMask[y] = 	XRegionBits;

	}/*end for y*/




	/* ///////////////////////////
	// Add the viewport to its parent device
	/////////////////////////// */

	if (dNode->node_hdr.next_node == NULL)
	{
		/* no viewports have been defined yet */

		/* the previous node is the device node */

		vNode->pPrevNode = (DL_NODE_STRUCT*) dNode;  

		dNode->node_hdr.next_node = (DL_NODE_STRUCT*) vNode; 

	}
	else
	{
		vNode->pPrevNode = dNode->pLastViewport; 		

		dNode->pLastViewport->next_node = (DL_NODE_STRUCT*) vNode; 
	}


 	dNode->pLastViewport=(DL_NODE_STRUCT*) vNode;


	SglError(error);

	return (name);

}





/**************************************************************************
 * Function Name  : sgl_get_viewport
 * Inputs         :	viewport
 * Outputs        : 
 * Input/Output	  :	left
					top
					right
					bottom
 * Returns        : error
 * Global Used    : 
 * Description    : returns setting for a specified viewport
 *
 **************************************************************************/


int CALL_CONV sgl_get_viewport( int viewport, 
								int * left, int * top,
								int *right, int *bottom,
								float *cam_rect_left, float *cam_rect_top, 
								float *cam_rect_right, float *cam_rect_bottom )
{
	int error = sgl_no_err;

    VIEWPORT_NODE_STRUCT * vNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		error = sgl_err_failed_init;

		SglError(error);

		return error;
	}
#endif

	/* Make sure that given name is the name of a device */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, viewport) != nt_viewport )
	{
		/* the given name is invalid */

		error = sgl_err_bad_name;

    	SglError(error);
		return error; 
	}


	/*	get reference to viewport from the name table */

    vNode = GetNamedItem(dlUserGlobals.pNamtab, viewport);


	*left = vNode->Left;
	*top = vNode->Top;
	*right = vNode->Right;
	*bottom = vNode->Bottom;

	*cam_rect_left = vNode->fCamLeft;
	*cam_rect_top = vNode->fCamTop;
	*cam_rect_right = vNode->fCamRight;
	*cam_rect_bottom = vNode->fCamBottom;


	SglError(sgl_no_err);

	return (sgl_no_err);
}





/**************************************************************************
 * Function Name  : sgl_set_viewport
 * Inputs         :	viewport
					left
					top
					right
					bottom
 * Outputs        : 
 * Input/Output	  :	
 * Returns        : 
 * Global Used    : 
 * Description    : sets new values for a given viewport
 *
 **************************************************************************/

int	CALL_CONV sgl_set_viewport( int viewport,
								int left, int top,
								int right, int bottom,
								float cam_rect_left, float cam_rect_top,
								float cam_rect_right, float cam_rect_bottom )
{

    VIEWPORT_NODE_STRUCT * vNode;
	int		DeviceID;

	int y;
	sgl_uint32 XRegionBits;
	DEVICE_REGION_INFO_STRUCT RegionInfo;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return(sgl_err_failed_init);
	}
#endif

	/* Make sure that given name is the name of a viewport */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, viewport) != nt_viewport )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return(sgl_err_bad_name); 
	}


	/*	get reference to viewport from the name table */

    vNode = GetNamedItem(dlUserGlobals.pNamtab, viewport);


	/* need to know viewports parent device before we can set it */

	DeviceID = vNode->pParentDevice->PhDeviceID;

	HWSetViewport(DeviceID,&left,&top,&right,&bottom);

	vNode->Left = left;
	vNode->Top = top;
	vNode->Right = right;
	vNode->Bottom = bottom;

	vNode->fCamLeft = cam_rect_left;
	vNode->fCamTop = cam_rect_top;
	vNode->fCamRight = cam_rect_right;
	vNode->fCamBottom = cam_rect_bottom;


	/* ///////////////////////////
	// Set the region mask so that all regions within this viewport are
	// included, but no others.
	/////////////////////////// */
	HWGetRegionInfo(DeviceID, &RegionInfo);

	/*
	// First clear out the structure
	*/
	for(y = 0; y < MAX_Y_REGIONS; y++)
	{
		vNode->regionMask[y] = 0;
	}

	/*
	// Calc the bits for the x dimension. First set them all, then clear off the
	// lower ones, then the upper ones
	*/
	XRegionBits = 0xFFFFFFFFUL;
	XRegionBits = XRegionBits >> (left / RegionInfo.XSize);
	XRegionBits = XRegionBits << (left / RegionInfo.XSize + 
								  (31 - right / RegionInfo.XSize));

	XRegionBits = XRegionBits >> (31 - right / RegionInfo.XSize);

	/*
	// Also actually set the number of regions covered. This is needed for
	// Midas3
	*/
#if ISTSP
	vNode->numRegions = ((right / RegionInfo.XSize) - 
						(left / RegionInfo.XSize) + 1) * 

						((bottom / RegionInfo.YSize) - 
						 (top / RegionInfo.YSize) + 1);

#endif
	/*
	// Now put this in all the rows.
	*/
	for(y = (top / RegionInfo.YSize); y <= (bottom / RegionInfo.YSize); y++)
	{
		vNode->regionMask[y] = 	XRegionBits;

	}/*end for y*/


	SglError(sgl_no_err);
	
	return(sgl_no_err);
}


/**************************************************************************
 * Function Name  : sgl_subtract_viewport
 * Inputs         :	viewport
					removed_viewport
 * Outputs        : 
 * Input/Output	  :	
 * Returns        : 
 * Global Used    : 
 * Description    : Removes the regions/tiles of the removed_viewport from
 *					the viewport.
 *
 *					They must BOTH belong to the same device.
 *
 **************************************************************************/

void CALL_CONV sgl_subtract_viewport( int viewport, 
									  int removed_viewport )
{

    VIEWPORT_NODE_STRUCT * vNode, *vRemovedNode;
	int y;

  	/*	
		Initialise sgl if this hasn't yet been done		
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

	/* Make sure that given names are the name of viewports */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, viewport) != nt_viewport )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
	}

    if ( GetNamedItemType(dlUserGlobals.pNamtab, removed_viewport) != nt_viewport )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
	}


	/*	get reference to viewport from the name table */

    vNode		= GetNamedItem(dlUserGlobals.pNamtab, viewport);
    vRemovedNode= GetNamedItem(dlUserGlobals.pNamtab, removed_viewport);


	/*
	// Check that they both belong to the same device
	*/
	if(vNode->pParentDevice != vRemovedNode->pParentDevice)
	{
    	SglError(sgl_err_bad_parameter);
	}


#if ISPTSP
	/*
	// Also actually set the number of regions covered. This is needed for
	// Midas3. I think the simplest way is just to count the number of bits
	// set.
	*/
	vNode->numRegions = 0;
#endif

	/*
	// Ok, now subtract any region that is in both the viewports
	*/
	for(y = 0; y < MAX_Y_REGIONS; y++)
	{
	
		vNode->regionMask[y] &= ~ vRemovedNode->regionMask[y];

#if ISPTSP
	{
		int x;
		/*
		// Add the number of bits set
		*/
		for(x=0; x < MAX_X_REGIONS; x++)
		{
			vNode->numRegions += (vNode->regionMask[y] >> x) & 1;
		}/*end for x*/
	}
#endif
		

	}/*end for y*/


	SglError(sgl_no_err);
	
}




/**************************************************************************
 * Function Name  : sgl_delete_viewport
 * Inputs         :	viewport
 * Outputs        : 
 * Input/Output	  :	
 * Returns        : 
 * Global Used    : 
 * Description    : deletes a viewport
 *
 **************************************************************************/


void CALL_CONV sgl_delete_viewport( int viewport )
{

	VIEWPORT_NODE_STRUCT	* pNext;
    VIEWPORT_NODE_STRUCT 	* vNode;
    DEVICE_NODE_STRUCT 		* dNode;
	DL_NODE_STRUCT			* pPrevious;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	/* Make sure that given name is the name of a device */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, viewport) != nt_viewport )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}


	/*	get reference to viewport from the name table */

    vNode = GetNamedItem(dlUserGlobals.pNamtab, viewport);

	pNext = (VIEWPORT_NODE_STRUCT*) vNode->node_hdr.next_node;

	pPrevious = vNode->pPrevNode;


	if (pNext!=NULL)
	{
		/* node is in middle of list */

		/* need to connect the next node to previous */

		pNext->pPrevNode = vNode->pPrevNode;

		/* and previous node to the next */

		pPrevious->next_node = vNode->node_hdr.next_node;

	}
	else
	{
		/* node is at end of list */

		/* previous node becomes NULL */		

		pPrevious->next_node = NULL;
		
		if (vNode->pPrevNode->n16_node_type==nt_device)
		{
			/* viewport was the only one defined so the previous node is a device */		

		    dNode = vNode->pParentDevice;

			ASSERT(dNode->node_hdr.n16_node_type==nt_device);

			dNode->pLastViewport = NULL;  

		}	
		else
		{		
			/* need to make the device node reference the previous node as the last viewport */

		    dNode = vNode->pParentDevice;

			ASSERT(dNode->node_hdr.n16_node_type==nt_device);

			dNode->pLastViewport = pPrevious;  

		}		
	}

	/* set the node free */

    DeleteNamedItem(dlUserGlobals.pNamtab,vNode->node_hdr.n16_name);

	SGLFree(vNode);

	SglError(sgl_no_err);

}













