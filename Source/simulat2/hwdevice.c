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
 * Revision 1.5  1997/04/10  19:58:53  mjg
 * Removed HWAdaptRegionSizeToScene as it wasn't beign called, let alone
 * actually doing anything.
 *
 * Revision 1.4  1997/03/31  22:14:00  jop
 * Got rid of windows references
 *
 * Revision 1.3  1996/11/04  16:04:58  erf
 * Add fake routine HWAadptRegionSizeToScene().
 *
 * Revision 1.2  1996/09/09  09:28:27  mjg
 * Pasted code form the Win32 version of the file to handle
 * tiles of awkward sizes.
 *
 * Revision 1.1  1995/12/18  14:22:12  sjf
 * Initial revision
 *
 * Revision 1.11  1995/10/27  15:50:19  jop
 * Added GetRegionSize
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


#define MODULE_ID MODID_HWDEVICE

#include <math.h>

#include "../sgl_defs.h"
#include "../dvregion.h"
#include "../hwinterf.h"
#include "syscon.h"
#include "../sglmacro.h"


#define LOW_X_RES	320
#define LOW_Y_RES	240

#define HIGH_X_RES	640
#define HIGH_Y_RES	480

#define	X_REGION_SIZE	64
#define	Y_REGION_SIZE	64

#define MAX_X_REGIONS	32
#define MAX_Y_REGIONS	32

/* only one device is supported for the simulator */

static sgl_bool device_free=TRUE;

static int  DeviceX;
static int  DeviceY;

static int  RegionXSize;
static int  RegionYSize;

static int  HasLeftOver;
static int  LeftOverY; 

static int NumXRegions;
static int NumYRegions;

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

int	HWCreateDevice( int *device_number, 
			int *x_dimension, 
			int *y_dimension,
			sgl_device_colour_types *device_mode, 
			sgl_bool *double_buffer)
{

	int error = 0;
	
	/* only one device is supported for the simulator */

	if (device_free)
	{
		*device_number=0;

		if ((*x_dimension<=LOW_X_RES) && (*y_dimension<=LOW_Y_RES))
		{
			*x_dimension = LOW_X_RES;
			*y_dimension = LOW_Y_RES;

			DeviceX = LOW_X_RES;
			DeviceY = LOW_Y_RES;
		}
		else
		{
			*x_dimension = HIGH_X_RES;
			*y_dimension = HIGH_Y_RES; 

			DeviceX = HIGH_X_RES;
			DeviceY = HIGH_Y_RES;
		}

		*device_mode=sgl_device_24bit;
		*double_buffer=FALSE;

		device_free = FALSE;

		RegionXSize = X_REGION_SIZE;
		RegionYSize = Y_REGION_SIZE;

		if(RegionXSize == 0)
		{
			RegionXSize = NUM_SABRE_CELLS;
		}
		else
		{
			RegionXSize = (RegionXSize+ NUM_SABRE_CELLS -1)/ NUM_SABRE_CELLS;
			RegionXSize *= NUM_SABRE_CELLS;
		}

		if(RegionYSize == 0)
		{
			RegionYSize = 1;
		}

		/*
		 * Calculate how many regions there are
		 */

    		NumXRegions = (DeviceX + RegionXSize - 1) / RegionXSize;
	    	NumYRegions = (DeviceY + RegionYSize - 1) / RegionYSize;

		/*
		 * Check that we don't exceed the max number of regions we
		 * can handle
		 */

		NumXRegions = MIN(NumXRegions, MAX_X_REGIONS);
		NumYRegions = MIN(NumYRegions, MAX_Y_REGIONS);

		/*
		 * Make sure X is a multiple of the size of the regions, and also
		 * see if there is a small portion left over for y
		 */

		*x_dimension = NumXRegions * RegionXSize;

		LeftOverY =	DeviceY - ((NumYRegions-1)*RegionYSize);
		HasLeftOver = (LeftOverY!=0);
	}		
	else
	{
		/* the device is already in use */

		error = -1;
	}	
	
	return error;

}





/**************************************************************************
 * Function Name  : HWGetDeviceSize
 * Inputs         : deviceID

 * Outputs        : x_dimension
					y_dimemsion 
 * Input/Output	  :
						  
 * Returns        : 
 * Global Used    : 

 * Description 	  : returns the size of a specified device
 *					
 *				   
 **************************************************************************/

int	HWGetDeviceSize(int 	deviceID, int *x_dimension,int *y_dimension)
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

int HWGetRegionInfo(int	deviceID, DEVICE_REGION_INFO_STRUCT * pRegionInfo)
{
	int error = 0;

	if (!device_free)
	{
		/* 	have to calculate the number of regions require for the given devices size */

		pRegionInfo->NumXRegions = (DeviceX + RegionXSize - 1) / RegionXSize;
		pRegionInfo->NumYRegions = (DeviceY + RegionYSize - 1) / RegionYSize;

		pRegionInfo->LeftOverY = LeftOverY;
		pRegionInfo->HasLeftOver = HasLeftOver;

		pRegionInfo->XSize = RegionXSize;
		pRegionInfo->YSize = RegionYSize;
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


int HWSetViewport(int parent_device, int * left, int * top, int	* right, int * bottom)
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

		*left = CLAMP(*left, 0, (DeviceX-1));
		*right = CLAMP(*right, 0, (DeviceX-1));
		*top = CLAMP(*top, 0, (DeviceY-1));
		*bottom = CLAMP(*bottom, 0, (DeviceY-1));

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
		
		/* x
		// values must be multiples of num sabre cells and y multiples of 
		// num sabres...
		//
		// Effectively.. "round down" from mid way, but take 1 off the right
		// and bottom...
		*/
		*left = ((*left + NUM_SABRE_CELLS/2) / NUM_SABRE_CELLS) * NUM_SABRE_CELLS;
		*left = CLAMP(*left, 0, (DeviceX-1));

		*right =((*right + NUM_SABRE_CELLS/2)/NUM_SABRE_CELLS)* NUM_SABRE_CELLS -1;
		*right = CLAMP(*right, 0, (DeviceX-1));

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

int		HWDeleteDevice(int deviceID)
{
	int error=0;
	
	/* there is only one device in the simulator */	

	device_free = TRUE;	

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
void HWGetRegionSize (int *pnXSize, int *pnYSize)
{
	*pnXSize = X_REGION_SIZE;
	*pnYSize = Y_REGION_SIZE;
}

