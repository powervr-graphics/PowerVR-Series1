/******************************************************************************
 * Name         : parmbuff.h
 * Title        : PowerVR Parameter buffers
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: parmbuff.h,v $
 * Revision 1.9  1997/07/14  19:29:15  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.8  1997/07/09  09:48:02  erf
 * Changed condition from '>' to '>=' in GetStartOfObject() since
 * Limit is the start of the next page.
 *
 * Revision 1.7  1997/06/04  15:29:32  gdc
 * removed compiler warnings
 *
 * Revision 1.6  1997/05/19  15:39:30  gdc
 * removed some ^M and prettified
 *
 * Revision 1.5  1997/05/19  09:00:28  gdc
 * put GetStartOfObject and GetSabreLimit here so ther is only one
 * version rather than 4
 *
 * Revision 1.4  1997/04/25  11:25:08  gdc
 * virtualised parabuff to use the ones in the logical device
 *
 * Revision 1.3  1997/04/21  20:00:25  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.2  1997/03/10  15:48:22  gdc
 * removed ^M's to stop gcc warning
 *
 * Revision 1.1  1996/11/19  00:59:40  jop
 * Initial revision
 *
 *
 *****************************************************************************/


#ifndef __PARMBUFF_H__
#define __PARMBUFF_H__

#include "debug.h"

extern HLDEVICE gHLogicalDev;
#define PVRParamBuffs (gHLogicalDev->Buffers)

/**********************************************************************/

static INLINE sgl_uint32 GetSabreLimit (sgl_uint32 CurrentPosition)
{

	/* don't worry about mac (or DOS!) as sabre mem is phys contig JWF */
#if ((PCX1 || PCX2 || PCX2_003) && WIN32)	
	PCXBUFFER *pPCXBuffer;
	sgl_uint32 ThisPage;

	pPCXBuffer = &(gHLogicalDev->VirtualBuffer->PCXBuffer);

	ThisPage = CurrentPosition >> (10 + pPCXBuffer->PageSize);

	ASSERT ((ThisPage & ~127) == 0);

	if(ThisPage >= pPCXBuffer->SlotsUsed)
	{
		return (pPCXBuffer->Limit[pPCXBuffer->SlotsUsed-1]);
	}
	else
	{
		return (pPCXBuffer->Limit[ThisPage]);
	}
#else

	return (SabreLimit);

#endif
}

/**********************************************************************/

static INLINE sgl_uint32 GetStartOfObject (sgl_uint32 StartOfObject, sgl_uint32 LengthInDW)
{
	#if ((PCX1 || PCX2 || PCX2_003) && WIN32)
	
		sgl_uint32	ThisPage;
		sgl_uint32	Limit;

		ThisPage = StartOfObject >> (10 + gHLogicalDev->VirtualBuffer->PCXBuffer.PageSize);
		
		ASSERT ((ThisPage & ~127) == 0);

		Limit = gHLogicalDev->VirtualBuffer->PCXBuffer.Limit[ThisPage];
 		
		/* The limit is the beginning of the next page so we need to have the condition
		 * greater than or equal to (ie >=).
		 */
		if ((StartOfObject + LengthInDW) >= Limit)
		{
			return (Limit);
		}
		else
		{
			return (StartOfObject);
		}
	
	#else

		if ((StartOfObject + LengthInDW) > SabreLimit)
		{
			return (SabreLimit);
		}
		else
		{
			return (StartOfObject);
		}

	#endif
}

/**********************************************************************/

#endif
