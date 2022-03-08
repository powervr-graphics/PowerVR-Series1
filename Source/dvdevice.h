/*************************************************************************
* Name : dvdevice.h
* Title : device nodes definitions
* Author : Stel Michael
* Created : 22/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of device and viewport node types for SGL
*
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: dvdevice.h,v $
 * Revision 1.12  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.11  1996/12/10  23:36:24  erf
 * Changed PVR1 to ISPTSP.
 *
 * Revision 1.10  1996/07/12  14:27:38  sjf
 *  Put #if's around the count of regions as that's only needed for
 * PVR1.
 *
 * Revision 1.9  1996/07/11  15:56:43  sjf
 * Added a count of the number of regions in a viewport. This was needed
 * in PVR1 to support the region masking.
 *
 * Revision 1.8  1996/07/11  11:00:06  sjf
 * Added region masking.
 *
 * Revision 1.7  1995/07/26  14:16:38  sm
 * remove onfo struct
 *
 * Revision 1.6  1995/07/26  10:56:59  sm
 * changed an x to a y!
 *
 * Revision 1.5  1995/07/26  10:54:59  sm
 * added device region infi structure
 *
 * Revision 1.4  1995/07/20  17:58:59  sjf
 * Changed fields in viewport node to describe camera rectangle.
 *
 * Revision 1.3  1995/07/20  16:29:54  sm
 * fixed Vs to be floats
 *
 * Revision 1.2  1995/07/20  11:05:37  sm
 * added virtual stuff
 *
 * Revision 1.1  1995/06/23  17:14:05  sm
 * Initial revision
 *
*
**************************************************************************/

#ifndef __DVDEVICE_H__
#define __DVDEVICE_H__

#include "pmsabre.h"

/*********************************************************
*
*	definition of viewport node structure
*
*********************************************************/



typedef struct _VIEWPORT_NODE_STRUCT
{

	/*		basic node header */

	DL_NODE_STRUCT node_hdr;


	/* 		pointer to previous node in list */

	DL_NODE_STRUCT	* pPrevNode;


	/*		pointer to parent device */

	struct _DEVICE_NODE_STRUCT	*pParentDevice;


	/*		coordinates of viewports top left and bottom right corners  */

	int Left,Top,Right,Bottom;

	/*
	// Location of the camera rectangle ...
	*/
	float fCamLeft,fCamTop,fCamRight,fCamBottom;

	/*
	// The region "bit mask" so that we can avoid overwriting certain tiles
	// This consists of 1 32 bit word per row of regions.
	*/
	sgl_uint32 regionMask[MAX_Y_REGIONS];

#if ISPTSP
	/*
	// Number of regions in the viewport. This is needed by Midas3
	//  (ie midas 32 and initial arcade boards)
	*/
	sgl_int32 numRegions;
#endif

} VIEWPORT_NODE_STRUCT;





/*********************************************************
*
*	definition of device node structure
*
*********************************************************/


typedef struct _DEVICE_NODE_STRUCT
{

	/*		basic node header */

	DL_NODE_STRUCT node_hdr;


	/* 		pointer to last in list of viewports */

	DL_NODE_STRUCT	* pLastViewport;

	
	/* 		Physical device ID */

	int	PhDeviceID;


	/* 		x and y dimensions for device */

	int	xDim,yDim;


	/* 		colour depth of device */

	sgl_device_colour_types	DeviceMode;


	/* 		boolean specifying whether device is double buffered */

	int DoubleBuffer;

	VIEWPORT_NODE_STRUCT defaultViewport;

} DEVICE_NODE_STRUCT;



#endif /* __DVDEVICE_H__  */





/*------------------------------ End of File --------------------------------*/
