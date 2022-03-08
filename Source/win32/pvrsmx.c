/******************************************************************************
 * Name         : pvrsmx.c
 * Title        : Compiled in resources for cards
 * Author       : Graham Connor
 * Created      : 07/11/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: pvrsmx.c,v $
 * Revision 1.1  1997/11/07  17:30:52  gdc
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl.h"

unsigned short PCIList[][2] = { 
 /*  Vendor   Device  */
	{0x1013, 0x00D6}, /* GS560 */
	{0x1023, 0x9320}, /* Trident9320 */
	{0x1023, 0x9397}, /* Trident */
	{0x1023, 0x9440}, /* Trident */
	{0x1023, 0x9470}, /* Trident */
	{0x1023, 0x9660}, /* Trident */
	{0x1023, 0x9750}, /* Trident */
	{0x1023, 0x9850}, /* Trident */
	{0x102B, 0x0519}, /* Matrox MilleniumI */
	{0x102B, 0x051B}, /* Matrox MilleniumII */
	{0x102B, 0x051A} /* Matrox Mystique */
};

unsigned short SLList[][2] = { 
	{0x105D, 0x2339}
};


__declspec(dllexport) unsigned short CALL_CONV  GetSLList(unsigned short *puVendor, 
														  unsigned short *puDevice,
														  unsigned short uIndex)
{
	if(puVendor == NULL && puDevice == NULL)
	{
		return (sizeof(SLList)>>2);
	}
	else if (uIndex < sizeof(SLList)>>2)
	{
		*puVendor = SLList[uIndex][0];
		*puDevice = SLList[uIndex][1];
		return uIndex;
	}

	return 0;
}

__declspec(dllexport) unsigned short CALL_CONV  GetPCIList(unsigned short *puVendor, 
														   unsigned short *puDevice,
														   unsigned short uIndex)
{
	if(puVendor == NULL && puDevice == NULL)
	{
		return (sizeof(PCIList)>>2);
	}
	else if (uIndex < sizeof(PCIList)>>2)
	{
		*puVendor = PCIList[uIndex][0];
		*puDevice = PCIList[uIndex][1];
		return uIndex;
	}

	return 0;

}





