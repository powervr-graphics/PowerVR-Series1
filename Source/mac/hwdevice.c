/******************************************************************************
 * Name : hwdevice.c
 * Title : 
 * Author : Stel Michael
 * Created : 23/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : hardware interface layer for simulator version of device management
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwdevice.c,v $
 * Revision 1.3  1997/03/31  22:15:23  jop
 * Got rid of some windows references
 *
 * Revision 1.2  1997/03/15  13:32:10  jwf
 * New Mac versions
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
/*
// Enable/disable simwin... 
*/
#define USE_SIMWIN 0

#define MODULE_ID MODID_HWDEVICE

#include <math.h>

#include "sgl_defs.h"
#include "dvregion.h"
#include "hwinterf.h"
#include "syscon.h"
#include "sglmacro.h"
#include "pvrlims.h"

#include "hwregs.h"
#include "w32dll.h"
#include "pmsabre.h"

#ifdef DDRAW
#include "windows.h"
#include "sglddraw.h"
#endif

#if MAC
#include "Null Functions.h"
#endif

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


/* only one device is supported for the simulator */

static sgl_bool device_free=TRUE;

static int	DeviceX;
static int	DeviceY;

static int	RegionXSize;
static int	RegionYSize;

static int 	HasLeftOver;
static int  LeftOverY; 

static int NumXRegions;
static int NumYRegions;

static int MergeHeight; /* Dynamic Region, control paramters */
static int MinYSize;

#ifdef DDRAW
extern void* pDDObject;
#endif


typedef enum
{
	REGION_SIZE_32x8,
	REGION_SIZE_32x12,
	REGION_SIZE_32x16,
	REGION_SIZE_32x32,
	REGION_SIZE_64x32,
	REGION_SIZE_64x64,
	NUM_REGION_SIZES

} REGION_SIZE;

typedef enum
{
	BUMP_NONE,
	BUMP_UP,
	BUMP_DOWN,
	BUMP_RESTART

} BUMP_DIRECTION;

typedef struct tagREGION_PERF_DATA
{
	int			xSize;
	int			ySize;
	REGION_SIZE	eRegionSize;
	float		PlaneRate;
	int			Samples;

} REGION_PERF_DATA;


BUMP_DIRECTION		eBumpDir = BUMP_NONE;
REGION_SIZE			eCurrentRegionSize = REGION_SIZE_32x32;
REGION_PERF_DATA 	RegionPerfData[NUM_REGION_SIZES] =
{
	{
		32, 8,
		REGION_SIZE_32x8,
		1e8f,
		0
	},
	{
		32, 12,
		REGION_SIZE_32x12,
		1e8f,
		0
	},
	{
		32, 16,
		REGION_SIZE_32x16,
		1e8f,
		0
	},
	{
		32, 32,
		REGION_SIZE_32x32,
		1e8f,
		0
	},
	{
		64, 32,
		REGION_SIZE_64x32,
		1e8f,
		0
	},
	{
		64, 64,
		REGION_SIZE_64x64,
		1e8f,
		0
	}
};

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
	
	/* only one device is supported  */
	if (device_free)
	{
		*device_number=0;

		/*
		// if the board has its own frame buffer, then restrict the
		// size of the device to 640x480
		*/
		if( (nSGLHardwareMode == MODE_OVERLAY24) || 
			(nSGLHardwareMode == MODE_STANDALONE))
		{
			*x_dimension = 640;
			*y_dimension = 480;
			*device_mode=sgl_device_24bit;
		}
		
		DeviceX = *x_dimension;
		DeviceY = *y_dimension;

		*double_buffer=FALSE;

		device_free = FALSE;

		/*
		// Get the region size... and then if not a legal size, choose one that
		// is!
		//
		// NOTE (for Midas3) the X sizes MUST be a multiple of the number of
		// sabre cells, and (for all boards) Y MUST be a multiple of the number
		// of sabres.  Note we are assuming 1 sabre!
		*/
		RegionXSize = HWRdValFile ("SW_XRegionSize", X_REGION_SIZE);
		RegionYSize = HWRdValFile ("SW_YRegionSize", Y_REGION_SIZE);

		/* Dynamic Region settings */
		MergeHeight = HWRdValFile ("SW_MergeHeight", MERGE_HEIGHT);
		MinYSize	= HWRdValFile ("SW_MinYSize", MIN_Y_SIZE);
		
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
		
		#ifdef DDRAW

		error = DDrawCreateDevice(*device_number, *x_dimension, *y_dimension, *device_mode, *double_buffer);

		#endif

	}	
	else
	{
		/* the device is already in use */

		error = -1;
	}	
	
	return error;

} /* HWCreateDevice */


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


int    	HWSetViewport(int	parent_device, int	* left, int	* top, int	* right, int	* bottom)
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

		#ifdef DDRAW

		if (pDDObject)
		{
			error = DDrawSetViewport (parent_device, *left, *top, *right, *bottom);
		}

		#endif

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

	#ifdef DDRAW

	if (pDDObject)
	{
		error = DDrawDeleteDevice(deviceID);
	}

	#endif

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
	*pnXSize = HWRdValFile ("SW_XRegionSize", X_REGION_SIZE);

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

	*pnYSize = HWRdValFile ("SW_YRegionSize", Y_REGION_SIZE);
}

/**************************************************************************
 * Function Name  : HWAdaptRegionSizeToScene
 * Inputs         : Pointers to placeholders
 * Input/Output	  : X,Y region size
 * Returns        : -
 * Global Used    : 
 * Description 	  : Frees specified device
 **************************************************************************/
void HWAdaptRegionSizeToScene (int nMaxPlanesInARegion, int nTotalPlanesProcessed)
{
	static int bInit = FALSE;
	static float fCps;
	LARGE_INTEGER  PerfCnt;

	if (!bInit)
	{
		bInit = TRUE;
		#if !MAC
		QueryPerformanceFrequency (&PerfCnt);
		fCps = PerfCnt.LowPart;
		#endif
	}
}

