/******************************************************************************
 * Name : hwdevice.c
 * Title : 
 * Author : Stel Michael
 * Created : 23/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : hardware interface layer for simulator version of device management
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwdevice.c,v $
 * Revision 1.37  1997/11/21  11:51:15  sxy
 * made DeviceX and DeviceY not static.
 *
 * Revision 1.36  1997/09/24  09:50:44  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values, and removed a couple of these calls
 *
 * Revision 1.35  1997/09/17  15:12:16  sxy
 * Removed warnings.
 *
 * Revision 1.34  1997/08/28  15:27:29  gdc
 * added region grid lines in debug builds
 *
 * Revision 1.33  1997/06/09  09:26:21  gdc
 * got rid of compiler warnings
 *
 * Revision 1.32  1997/05/30  16:38:47  mjg
 * Added the logical device as an argument to the DiplayDevice fns.
 *
 * Revision 1.31  1997/05/27  14:59:52  erf
 * Fixed create HW create device when Y size of around 200 or 600 is
 * requested. Needed to special case these since they aren't divisable
 * by 16.
 *
 * Revision 1.30  1997/05/13  17:57:56  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.29  1997/05/06  08:54:21  erf
 * Changed UINT32 to sgl_uint32.
 *
 * Revision 1.28  1997/05/01  02:43:23  jop
 *  Undid Ed's fixes
 *
 * Revision 1.27  1997/04/29  15:13:17  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.26  1997/04/22  18:02:40  mjg
 * Replaced w32dll.h include with windows.h and removed
 * PVROSCallbackUnRegister calls as Ed said they were in
 * PVROSCloseDisplayDevice.#
 *
 * Revision 1.25  1997/04/10  20:07:46  mjg
 * Removed warnings and redundant code.
 *
 * Revision 1.24  1997/04/08  13:56:30  jop
 * Cleaned up includes a bit
 *
 * Revision 1.23  1997/04/04  21:41:31  mjg
 * Moved 'pvrosapi.h' include.
 *
 * Revision 1.22  1997/04/03  13:09:23  mjg
 * Updated the calling convention.
 *
 * Revision 1.21  1997/03/27  12:52:01  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.20  1997/03/25  15:21:01  erf
 * Removed *double_buffer = TRUE;.
 *
 * Revision 1.19  1997/03/25  12:46:54  jop
 * 24 bit screen modes now selected if 32 bit requested and no
 * 32 bit modes.
 *
 * Revision 1.18  1997/03/25  10:18:16  jop
 * went over to PVROS API
 *
 * Revision 1.17  1997/01/06  19:12:30  jrc
 * No longer rounding up the reported device X size to a whole number of
 * regions for PCX.
 *
 * Revision 1.16  1996/12/18  13:21:07  mjg
 * Changed the internal interpretation of the X and Y ( now maximum )
 * sglhw.ini file settings.
 *
 * Revision 1.15  1996/12/12  11:18:36  erf
 * Removed best fit region stuff.
 *
 * Revision 1.14  1996/12/11  01:17:52  jop
 * Removed ../'s from include files
 *
 * Revision 1.13  1996/11/20  23:33:34  jrc
 * Removed notprintf("Cps") that was of no use because it did not diaplay the
 * value.
 *
 * Revision 1.12  1996/11/20  19:01:29  mjg
 * Added support for ini file definable dynamicly sized tiles
 *
 * Revision 1.11  1996/10/11  17:10:36  jop
 * Did mod 32 x region fix (paul edelstein)
 *
 * Revision 1.10  1996/10/03  14:23:59  jop
 * Best fit debugging off
 *
 * Revision 1.9  1996/10/02  16:51:32  jop
 * Added more adapt stuff
 *
 * Revision 1.8  1996/09/10  08:55:53  sjf
 * 1) Cleaned up a whole stack of ^M's
 * 2) Fixed a problem with the "rounding" to nearest legal viewports. The
 *    code could cause gaps to appear between what should be abutting
 *    viewports.
 *
 * Revision 1.7  1996/09/08  02:36:11  jop
 * Made adaptive stuff even more complicated and useless
 *
 * Revision 1.6  1996/09/05  20:39:27  jop
 * printfs removed
 *
 * Revision 1.5  1996/09/04  11:53:31  jop
 * added adaptive rigion sizing (disabled for now)
 *
 * Revision 1.4  1996/07/12  14:09:41  ncj
 * Added 24 bit direct draw support
 *
 * Revision 1.3  1996/07/02  08:39:22  mal
 * Changed Default tile size to 32x32 as requested by MarkH.
 * Was previously 64x64.
 *
 * Revision 1.2  1996/06/12  10:51:14  sjf
 * Fixed a problem with viewports rendering more than they said they would in the
 * y dimension. The fix I've made isn't brilliant, but will do .. it contrains Y's to
 * lie on multiples of the Y region size, even tho' we could set the Y anywhere with
 * PCX1 and single ISP systems.
 * (This code should also be put in the arcade system (and probably the simulator as well)
 * but I don't have time at the moment).
 *
 * Revision 1.1  1996/06/10  11:51:12  jop
 * Initial revision
 *
 * Revision 1.1  1996/06/10  11:39:13  jop
 * Initial revision
 *
 * Revision 1.9  1996/04/26  14:32:19  sm
 * added support for any y dimension
 *
 * Revision 1.7  1996/03/25  11:09:35  ncj
 * Includes windows.h if DDRAW is defined
 *
 * Revision 1.6  1996/02/12  10:50:49  ncj
 * Added direct draw calls if DDRAW is defined
 *
 * Revision 1.5  1995/10/27  14:28:21  jop
 * Region sizes now read from ini file
 *
 * Revision 1.4  1995/10/25  17:51:24  sjf
 * Now reads screen size from a file.
 *
 * Revision 1.3  1995/10/24  11:49:25  sjf
 * resized the screen to 448 in Y for the mo, to make sure the screen is
 * a multiple of  64.
 *
 * Revision 1.2  1995/10/21  21:48:58  sjf
 * Disabled simwin...
 *
 * Revision 1.1  1995/10/20  19:09:38  sjf
 * Initial revision
 *
 * Revision 1.10  1995/07/28  15:38:12  sjf
 * Changed the way the viewport gets locked to region space..
 *
 * Revision 1.9  1995/07/27  10:48:15  sm
 * fixed bug in set viewport
 *
 * Revision 1.8  1995/07/26  17:35:04  jop
 * Added simulator window support
 *
 * Revision 1.7  1995/07/26  14:20:45  sm
 * fixed a couple of bugs
 *
 * Revision 1.6  1995/07/26  14:18:01  sm
 * changed to include hwregion.h
 *
 * Revision 1.5  1995/07/26  12:07:49  sm
 * moved syscon
 *
 * Revision 1.4  1995/07/26  11:58:15  sm
 * added getregioninfo function
 *
 * Revision 1.3  1995/07/20  16:31:24  sm
 * change setting of x values
 *
 * Revision 1.2  1995/06/30  12:52:37  sjf
 * Didn't include defs of UINT etc.
 *
 * Revision 1.1  1995/06/27  10:28:23  sm
 * Initial revision
 *
 * Revision 1.2  1995/06/26  12:16:59  sm
 * slight mods
 *
 * Revision 1.1  1995/06/26  12:04:27  sm
 * Initial revision
 *
 *
 *****************************************************************************/

#include <math.h>
#include <windows.h> 

#include "sgl_defs.h"
#include "hwinterf.h"
#include "syscon.h"
#include "sglmacro.h"
#include "pvrlims.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "hwregs.h"
#include "pmsabre.h"

#include "debug.h"

/*
// Stand alone and overlay configs....
*/
#define SA_HIGH_X_RES	640
#define SA_HIGH_Y_RES	448    /* BUG Work around - we dont do partial sized yet*/

#define OV_HIGH_X_RES	640
#define OV_HIGH_Y_RES	448    /* BUG Work around - we dont do partial sized yet*/

#define	X_REGION_SIZE	32
#define	Y_REGION_SIZE	32	


#define LOW_X_RES	320
#define LOW_Y_RES1	200
#define LOW_Y_RES2	240


#define MID_X_RES  640
#define MID_Y_RES1 400
#define MID_Y_RES2 480


#define HIGH_X_RES 800
#define HIGH_Y_RES 600

#define MERGE_HEIGHT	8 /* Dynamic Regions defaults */
#define MIN_Y_SIZE		2

extern HLDEVICE gHLogicalDev;

/* only one device is supported for the simulator */

static sgl_bool device_free=TRUE;

int	DeviceX;
int	DeviceY;

static int	RegionXSize;
static int	RegionYSize;

static int 	HasLeftOver;
static int  LeftOverY; 

static int NumXRegions;
static int NumYRegions;

static int MergeHeight; /* Dynamic Region, control paramters */
static int MinYSize;

static FRAME_BUFFER_MODE 	gFrameBufferMode = FRAME_BUFFER_ACTIVE;

#if DEBUG
/**********************************************************************
**
** Draw grid lines along region boundaries, useful aid when trying
** to determine why a vignette or other region related problem occurs
** as you can easily identify the region
**
**********************************************************************/

void DrawRegionGrid(PSGLCONTEXT pContext)
{
	SGLCONTEXT GridContext = *pContext;
	SGLVERTEX line[2];
	sgl_uint16 pLine[][2] = {0,1};
	int i;

	/* make the context safe for flat shaded */
	GridContext.u32Flags &= ~SGLTT_TEXTURE;
	GridContext.u32Flags &= ~SGLTT_GOURAUD;
	GridContext.u32Flags &= ~SGLTT_HIGHLIGHT;
	GridContext.u32Flags &= ~SGLTT_VERTEXTRANS;
	GridContext.u32Flags &= ~SGLTT_GLOBALTRANS;
	GridContext.u32Flags &= ~SGLTT_DISABLEZBUFFER;
	GridContext.uLineWidth = 1;

	line[0].u32Colour = 0x00ff00ff;
	line[1].u32Colour = 0x00ff00ff;

	line[0].fInvW = 1.0f;
	line[1].fInvW = 1.0f;
	line[0].fY = 0.0f; 
	line[1].fY = (float)(DeviceY - 1);

	/* draw Y lines */
	for(i=RegionXSize-1; i<DeviceX-1; i+=RegionXSize)
	{
		line[0].fX = (float)i;
		line[1].fX = (float)i;
		
		sgltri_lines(&GridContext, 1, pLine, line);
	}

	line[0].fX = 0.0f; 
	line[1].fX = (float)(DeviceX - 1);
	/* draw X lines */
	for(i=RegionYSize-1; i<DeviceY-1; i+=RegionYSize)
	{
		line[0].fY = (float)i;
		line[1].fY = (float)i;
		
		sgltri_lines(&GridContext, 1, pLine, line);
	}
}
#endif

/**************************************************************************
 * Function Name  : HWSetFrameBufferMode
 * Inputs         : 
 * Outputs        :  
 * Input/Output	  : FrameBufferMode - ACTIVE/PASSIVE buffer mode
 * Global Used    : 
 * Description 	  : Allows calls like sgl_use_address_mode to disable
 *					automatic frame buffer management
 **************************************************************************/
void CALL_CONV HWSetFrameBufferMode (FRAME_BUFFER_MODE FrameBufferMode)
{
	gFrameBufferMode = FrameBufferMode;
}	

/**************************************************************************
 * Function Name  : HWCreateDevice
 * Inputs         : 

 * Outputs        :  
 * Input/Output	  : device_number - Physical device ID
					x_dimension
					y_dimension
					device_mode
					double_buffer
 * Returns        : error
 * Global Used    : 

 * Description 	  : Creates requested device if it is free
 *					
 *				   
 **************************************************************************/

int	CALL_CONV HWCreateDevice( int *device_number,
							  int *x_dimension,
							  int *y_dimension,
							  sgl_device_colour_types *device_mode,
							  sgl_bool *double_buffer )
{
	HDISPLAY hDisplay;
	
	int error = -1;
	
	/* only one device is supported  */
	if (device_free)
	{
		DeviceX = *x_dimension;
		DeviceY = *y_dimension;

		device_free = FALSE;

		/*
		// Get the region size... and then if not a legal size, choose one that
		// is!
		//
		// NOTE (for Midas3) the X sizes MUST be a multiple of the number of
		// sabre cells, and (for all boards) Y MUST be a multiple of the number
		// of sabres.  Note we are assuming 1 sabre!
		*/
		RegionXSize = HWRdValFileUInt ("SW_XRegionSize", X_REGION_SIZE);
		RegionYSize = HWRdValFileUInt ("SW_YRegionSize", Y_REGION_SIZE);

		/* Dynamic Region settings */
		MergeHeight = HWRdValFileUInt ("SW_MergeHeight", MERGE_HEIGHT);
		MinYSize	= HWRdValFileUInt ("SW_MinYSize", MIN_Y_SIZE);
		
		switch(RegionXSize) 
		{			/* See 'HWGetRegionSize' below for an explanation */
			case 0: 
				RegionXSize = NUM_SABRE_CELLS;
				break;
			case 32: 				/* Don't merge tiles sideways */
				MergeHeight = 512;  /* Or any other big number would do */
				break;
			case 64:
				RegionXSize = 32;   /* Do merge tiles sideways */
				break;
			default:
				RegionXSize = (RegionXSize+ NUM_SABRE_CELLS -1)/ NUM_SABRE_CELLS;
				RegionXSize *= NUM_SABRE_CELLS;
			break;
		}		
			
		
		if(RegionYSize == 0)
		{
			RegionYSize = 1;
		}
		
		if(MinYSize == 0)
		{
			MinYSize = RegionYSize;
		}
		
		if(MergeHeight == 0)
		{
			MergeHeight = 2;
		}

		/*
		// Calculate how many regions there are
		*/
	    NumXRegions = (DeviceX + RegionXSize - 1) / RegionXSize;
	    NumYRegions = (DeviceY + RegionYSize - 1) / RegionYSize;

		/*
		// Check that we don't exceed the max number of regions we
		// can handle
		*/
		NumXRegions = MIN(NumXRegions, MAX_X_REGIONS);
		NumYRegions = MIN(NumYRegions, MAX_Y_REGIONS);

		/*
		// For Midas 3 and earlier, make sure X is a multiple of the size of
		// the regions.
		*/
#if ISPTSP
		*x_dimension = NumXRegions * RegionXSize;
#endif

		/*
		// See if there is a small portion left over for y.
		*/
		LeftOverY =	DeviceY - ((NumYRegions-1)*RegionYSize);
		HasLeftOver = (LeftOverY!=0);

		if (gFrameBufferMode == FRAME_BUFFER_PASSIVE)
		{
			DPF ((DBG_MESSAGE, "Opening passive device"));			

			hDisplay = PVROSOpenDisplayDevice (gHLogicalDev, NULL, FRAME_BUFFER_PASSIVE);
			
			if (hDisplay)
			{
				*device_number = (int) hDisplay;
				
				/* success */
				error = 0;
			}
		}
		else
		{
			ASSERT (gFrameBufferMode == FRAME_BUFFER_ACTIVE);
			
			/* 
				old apps request sizes slightly smaller than they 
				actually want. Ah well, snap x and y to nearest
				16 pixel boundary and try that.
			*/
			*x_dimension = (*x_dimension + 0x0000000F) & 0xFFFFFFF0;
			*y_dimension = (*y_dimension + 0x0000000F) & 0xFFFFFFF0;

			/* Need to special case Y sizes of 200 and 600 since
			 * these aren't divisable by 16.
			 *
			 * i.e. if Y set to 208 or 192 then set to 200, and
			 * if Y set to 608 or 592 then set to 600.
			 */
			if ((*y_dimension == 208) || (*y_dimension == 192))
				*y_dimension = 200;
			
			if ((*y_dimension == 608) || (*y_dimension == 592))
				*y_dimension = 600;
			
			/* GUID = null is the primary ddraw provider
			 */
			hDisplay = PVROSOpenDisplayDevice (gHLogicalDev, NULL, FRAME_BUFFER_ACTIVE);
			
			if (hDisplay)
			{
				sgl_uint32 nModes;
				
				nModes = PVROSEnumDisplayModes (hDisplay, NULL);
				
				if (nModes)
				{
					DisplayModeTable *pDMT = SGLMalloc (sizeof (DisplayModeTable) * nModes);
					
					if (pDMT)
					{
						sgl_uint32 k;
						
						PVROSEnumDisplayModes (hDisplay, pDMT);
						
						for (k = 0; k < nModes; ++k)
						{
							if (((int)(pDMT[k].XResolution) == *x_dimension) &&
								((int)(pDMT[k].YResolution) == *y_dimension) &&
								(((pDMT[k].BitDepth == 16) && (*device_mode == sgl_device_16bit)) ||
								 (((pDMT[k].BitDepth == 32) || (pDMT[k].BitDepth == 24)) && 
								  (*device_mode == sgl_device_24bit))))
							{
								PVROSERR Err;
								
								if (*double_buffer)
								{
									Err = PVROSSelectDisplayMode (hDisplay, pDMT[k].hMode, 2);
								}
								else
								{
									Err = PVROSSelectDisplayMode (hDisplay, pDMT[k].hMode, 1);
								}
								
								if (Err == PVROS_GROOVY)
								{
									*device_number = (int) hDisplay;

									/* success */
									error = 0;
								}
							}
						}
						
						SGLFree (pDMT);					
					}
				}
			}
		}
	}	
	else
	{
		/* the device is already in use */

	}	
	
	return error;

} /* HWCreateDevice */


/**************************************************************************
 * Function Name  : HWGetDeviceSize
 * Inputs         : hDisplay

 * Outputs        : x_dimension
					y_dimemsion 
 * Input/Output	  :
						  
 * Returns        : 
 * Global Used    : 

 * Description 	  : returns the size of a specified device
 *					
 *				   
 **************************************************************************/

int	CALL_CONV HWGetDeviceSize( int hDisplay,
							   int *x_dimension, int *y_dimension )
{
	int error = 0;

	if (!device_free)
	{
		*x_dimension = DeviceX;
		*y_dimension = DeviceY;
	}
	else
		error = -1; /* no device has been created */

	return error;
}


/**************************************************************************
 * Function Name  : HWGetRegionInfo
 * Inputs         : 

 * Outputs        : pRegionInfo 
 * Input/Output	  : 
						  
 * Returns        : 
 * Global Used    : 

 * Description 	  : return device region info for a specified device
 *					
 *				   
 **************************************************************************/

int CALL_CONV HWGetRegionInfo( int hDisplay,
							   DEVICE_REGION_INFO_STRUCT * pRegionInfo )
{
	int error = 0;

	if (!device_free)
	{
		/* 	have to calculate the number of regions require for the given devices size */

		pRegionInfo->NumXRegions = NumXRegions; 
		pRegionInfo->NumYRegions = NumYRegions; 

		pRegionInfo->LeftOverY = LeftOverY;
		pRegionInfo->HasLeftOver = HasLeftOver;		
		
		pRegionInfo->XSize = RegionXSize;
		pRegionInfo->YSize = RegionYSize;

		/* Dynamic region settings */
		pRegionInfo->MinYSize    = MinYSize;
		pRegionInfo->MergeHeight = MergeHeight;

	}
	else
		error = -1; /* no device has been created */

	return error;

}



/**************************************************************************
 * Function Name  : HWSetViewport
 * Inputs         : parent_device

 * Outputs        : left
					top	
					right
					bottom
 
 * Input/Output	  : 
						  
 * Returns        : 
 * Global Used    : 

 * Description 	  :
 *					
 *				   
 **************************************************************************/


int CALL_CONV HWSetViewport( int parent_device,
							 int * left, int * top,
							 int * right, int * bottom )
{

	int error = 0;
	int tem;
	
	if (device_free)
	{
		/* no device has been defined */
	 
		error = -1;
	}
	else
	{
		/* Clamp given coord to within device boundaries */

		*left	= CLAMP(*left,  0, (DeviceX-1));
		*right	= CLAMP(*right, 0, (DeviceX-1));
		*top	= CLAMP(*top,	0, (DeviceY-1));
		*bottom = CLAMP(*bottom,0, (DeviceY-1));

		/* make sure left is smaller than right and top is smaller than bottom */		

		if (*left > *right)
		{
			tem=*right;
			*right=*left;
			*left=tem; 
		}

		if (*top > *bottom)
		{
			tem=*top;
			*top=*bottom;
			*bottom=tem; 
		}
		
		/* x values must be multiples of num sabre cells and
		// y multiples of  num sabres... 
		//
		// HOWEVER to make the coding easier, we'll make the further
		// restriction that they align with the region size.
		//
		// We need to make sure that if the input values for viewports abut,
		// then after rounding then they still join up. They will join up if
		// the edge positions differ by only one pixel, so if we add one to
		// the right value, round and then subtract one off it should still
		// join up with a matching left-hand edge. Similarly with top and
		// bottom edges.
		*/
		*left = ((*left + RegionXSize/2) / RegionXSize) * RegionXSize;
		*left = CLAMP(*left, 0, (DeviceX-1));

		*right =((*right + 1 + RegionXSize/2) / RegionXSize)* RegionXSize -1;
		*right = CLAMP(*right, 0, (DeviceX-1));

		*top 	= ((*top + RegionYSize/2) 	/ RegionYSize) * RegionYSize;
		*top	= CLAMP(*top,	0, (DeviceY-1));

		*bottom = ((*bottom + 1 + RegionYSize/2)/ RegionYSize) * RegionYSize -1;
		*bottom	= CLAMP(*bottom, 0, (DeviceY-1));


		/* what about if they are the same ??????????? Have left it for now */

	}

	return error;

}


/**************************************************************************
 * Function Name  : HWDeleteDevice
 * Inputs         : 

 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : 
 * Global Used    : 

 * Description 	  : Frees specified device
 *					
 *				   
 **************************************************************************/

int	CALL_CONV HWDeleteDevice( int hDisplay )
{
	int error=0;

	/* there is only one device in the simulator */	

	device_free = TRUE;	

	if (hDisplay)
	{
		PVROSCloseDisplayDevice( gHLogicalDev, (HDISPLAY) hDisplay );		

		(void*) hDisplay = NULL;
	}

	return error;
}

/**************************************************************************
 * Function Name  : HWGetRegionSize
 * Inputs         : Pointers to placeholders
 * Input/Output	  : X,Y region size
 * Returns        : -
 * Global Used    : 
 * Description 	  : Frees specified device
 **************************************************************************/
void CALL_CONV HWGetRegionSize ( int *pnXSize, int *pnYSize )
{
	*pnXSize = RegionXSize;

	/* Since the XSize used by 'dregion.c' _has_ to be the minimum
	 * width, the values read in from sglhw.ini are twisted to maintain
	 * a clear external interface. Therefore, the region sizes in the .ini
	 * file should be interpreted as the Maximum X and Y values:
	 * 32 x 32  => never go higher than 32 (but can go lower) and don't 
	 *             alter the width of the tiles.
	 * 64 x 32  => as above for Y, but the tile can be 32 or 64 pixels 
	 *             wide, depending on the number of planes within it.
	 */

	switch(*pnXSize)
	{
		case 0: 
			*pnXSize = NUM_SABRE_CELLS;
			break;
		case 32: 				    /* Don't merge tiles sideways */
			MergeHeight = 512;		/* Or any other big number would do */
			break;
		case 64:
			*pnXSize = 32;			/* Do merge tiles sideways */
			break;
		default:
			*pnXSize = (*pnXSize+ NUM_SABRE_CELLS -1)/ NUM_SABRE_CELLS;
			*pnXSize *= NUM_SABRE_CELLS;
		break;
	}		

	*pnYSize = RegionYSize;
}
