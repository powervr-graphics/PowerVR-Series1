/******************************************************************************
 * Name         : texas.c
 * Title        : This is the texas hardware support routines.
 * Author       : Peter Leaback
 * Created      : 20/10/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: texas.c,v $
 * Revision 1.21  1997/07/21  09:57:12  gdc
 * compiled out some unused code
 *
 * Revision 1.20  1997/07/16  13:27:03  gdc
 * removed unused FetchPixel and WriteTwoPixel functions and
 * moved WritePixel and WriteNPixels to txmops and made the static there
 *
 * Revision 1.19  1997/06/09  09:17:30  gdc
 * got rid of compiler warnings
 *
 * Revision 1.18  1997/06/04  10:34:39  gdc
 * changed function parameter - and associated parameter
 *
 * Revision 1.17  1997/05/14  03:42:43  jop
 * Removed WriteVirtualReg
 *
 * Revision 1.16  1997/05/14  00:24:42  mjg
 * Made file able to be used for PCX1 and PCX2.
 *
 * Revision 1.15  1997/04/25  14:42:06  gdc
 * removed a if PCX1 in a PCX1 file
 *
 * Revision 1.14  1997/04/25  10:36:24  gdc
 * upadted to equvalent pcx2 std - untested
 *
 * Revision 1.13  1997/04/23  20:29:04  mjg
 * New PVROS function calls.
 *
 * Revision 1.12  1997/04/22  18:09:10  mjg
 * Removed w32dll.h include and compiler warnings.
 *
 * Revision 1.11  1997/04/04  19:40:09  mjg
 * Included 'pvrosapi.h'
 *
 * Revision 1.10  1997/04/03  18:02:04  mjg
 * Added more CALL_CONVs.
 *
 * Revision 1.9  1997/04/03  15:31:13  mjg
 * Fixes for DOSing.
 *
 * Revision 1.8  1997/04/03  13:08:28  mjg
 * Updated the calling convention.
 *
 * Revision 1.7  1996/12/10  19:16:17  erf
 * Changed defined (PCX1) to PCX1.
 *
 * Revision 1.6  1996/06/27  12:10:09  sjf
 *  Fixed Signed/Unsigned bug in WriteNPixelPairs.
 *
 * Revision 1.5  1996/06/25  16:36:02  sjf
 * Added WriteNPixelpairs  to speed up texture caching.
 *
 * Revision 1.4  1996/06/10  14:15:11  jop
 * Updated headers
 *
 * Revision 1.3  1996/05/09  17:38:35  jop
 * Fixed tex mem reads in pcx1
 *
 * Revision 1.2  1996/04/30  18:09:29  jop
 * Fixes for pcx1
 *
 * Revision 1.1  1996/04/17  17:47:39  pdl
 * Initial revision
 *
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <math.h>
#include "sgl_defs.h"
#include "dlnodes.h"
#include "rnglobal.h"
#include "ldbmp.h"
#include "txmops.h"
#include "pvrosapi.h"
#include "brdcfg.h"
#include "hwregs.h"

extern HLDEVICE gHLogicalDev;

/*=========================================================================
name	|TexasSetDim
function|temp function for bringing texas simulator up with sgl.
in		|x,
		|y,
out		|-
rd		|-
wr		|Bitmap_ydim,Bitmap_xdim
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasSetDim(int x, int y)
{
}

/*=========================================================================
name	|TexasSetFogColour
function|.
in		|Colour,
out		|-
rd		|-
wr		|fogColour
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasSetFogColour(sgl_map_pixel Colour)
{
	sgl_uint32 Value;

	Value = 0x1000000 | (Colour.red<<16) | (Colour.green<<8) | Colour.blue;

	/*  write the colour into the register */	

	gHLogicalDev->Registers[PCX_FOGCOL] = Value;
}

/*=========================================================================
name	|TexasSetCFRScale
function|This can be the distance between the center of projection and the
		|the projection plane.
in		|Scale,
out		|-
rd		|-
wr		|
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasSetCFRScale(sgl_uint32 Scale)
{
	sgl_uint32 Value;

	Value=Scale;

	/*  write the scale into the register */	

	gHLogicalDev->Registers[PCX_CAMERA] = Value;
}

#if !WIN32

/******************************************************************************
 * Function Name: LoadReciprocalTable
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Loads the reciprocal table into the texas on-chip RAM
 *****************************************************************************/

void CALL_CONV LoadReciprocalTable()
{
#include "rectable.h"
	sgl_uint32	Count;

	for(Count=0;Count<512;Count++)
	{	
		gHLogicalDev->Registers[PCX_DIVIDER_TABLE + Count] = ReciprocalTable[Count];
	}
}

/******************************************************************************
 * Function Name: HWSetupBunchOfTSPRegs
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : This sets up a bunch of TSP registers that have not been
 *				  touched by the software.
 *****************************************************************************/
void CALL_CONV HWSetupBunchOfTSPRegs()
{
	LoadReciprocalTable();

	/*the other regs will be setup in HWSetupBunchofTSPRegs */	
}

#endif
