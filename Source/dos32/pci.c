/******************************************************************************
 * Name			: pci.c
 * Title		: PowerVR Operating System dependent code (DOS4GW)
 * Author		: Mick Jones
 * Created		: 20/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	: Part of the low level operarting system specific API
 *                for DOS4GW, this requires an SVGA card with a VESA
                  UVB v2.0 BIOS 
 *
 * Platform		: ANSI
 *
 * Modifications:
 *  		20th March 1997		Initial Version		Mick Jones
 *
 *		$Log: pci.c,v $
 * Revision 1.6  1997/09/05  13:45:06  jmd
 * Added Generic PCI functions
 *
 * Revision 1.5  1997/07/08  13:38:43  mlj
 * added PC98 support.
 *
 * Revision 1.4  1997/06/05  12:07:23  sjf
 * Added RCS Log Macro.
 *
 *
 *****************************************************************************/


#include <stdio.h>
#include <i86.h>
#include <io.h>
#include <conio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>   
#include <dos.h>
#include <ctype.h>
#include <string.h>

#include "defs.h"
#include "sgl.h"
#include "pvrosapi.h"
#include "pci.h"

/* Globals containing PCI config address locations */
sgl_uint8  gPciBusID    = 0;
sgl_uint8  gPciDevID    = 0;
sgl_uint8  gPciFncID    = 0;
sgl_uint8  gDeviceSetup = 0;

/* The NEC PC98 uses a different interupt for the    */
/* PCI BIOS and different op-codes, this global will */
/* enable an application select which one to use     */
sgl_uint32 gNECPC98     = 0; 



sgl_uint32 PCISetDevice (sgl_uint8 busNum, sgl_uint8 devNum, sgl_uint8 fncNum)
{
	sgl_uint16  Data;
	
	
	gPciBusID    = busNum;
	gPciDevID    = devNum;
	gPciFncID    = fncNum;
	gDeviceSetup = 1;

	PCIReadWord (0, &Data);   

	if (Data == 0xFFFF) return 1;

	gDeviceSetup = 1;

	return 0;
}


sgl_uint32 PCIFindDevice (sgl_uint16 device, sgl_uint16 vendor, sgl_uint16 index)
{
	union _REGS inregs, outregs;
        
	inregs.w.cx = device;
	inregs.w.dx = vendor;
	inregs.w.si = index;
	
	if (gNECPC98)
	{
		inregs.w.ax = (FIND_PCI_DEVICE_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = FIND_PCI_DEVICE_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
	
	gPciBusID    = outregs.h.bh;
	gPciDevID    = (outregs.h.bl >> 3);
	gPciFncID    = (outregs.h.bl &  3);
	gDeviceSetup = 1;	

	return 0;
}


sgl_uint32 PrintPciConfigSpace (void)
{
	sgl_uint16  i;
	sgl_uint32  result;
	sgl_uint32  error;

	for (i = 0; i < 64; i += 4)
	{
		error = PCIReadDword (i, &result);
		if (error != 0) return error;
                    
		PVROSPrintf("%08X : %2X\n",result,i);
	}
	return 0;
}


sgl_uint32 PCIFindAllDevices (PciConfigSpace *pciConfigSpace)
{
	sgl_uint32   i;

	sgl_uint8    BusID;
	sgl_uint8    DevID;
	sgl_uint8    FncID;



	i = 0;


	if (pciConfigSpace == NULL)
	{
		for (BusID = 0; BusID < 255; BusID++)
		{
			for (DevID = 0; DevID < 32; DevID++)
			{
				for (FncID = 0; FncID < 1; FncID++) /* ignore multi function devices */
				{
					if (PCISetDevice(BusID, DevID, FncID) == 0) i++;
				}
			}
		}
	}
	else
	{
		for (BusID = 0; BusID < 255; BusID++)
		{
			for (DevID = 0; DevID < 32; DevID++)
			{
				for (FncID = 0; FncID < 1; FncID++) /* ignore multi function devices */
				{
					if (PCISetDevice(BusID, DevID, FncID) == 0)
					{
						memset(&(pciConfigSpace[i]),0,sizeof(PciConfigSpace));
						
						PCIGetConfigSpace (&(pciConfigSpace[i]));

						i++;
					}
				}
			}
		}
		pciConfigSpace[i].PciBusID = 0xFFFFFFFF;
	}

	return i;
}


sgl_uint32 PCIGetConfigSpace (PciConfigSpace *pciConfigSpace)
{
	sgl_uint32     Data;


	
	if (gDeviceSetup == 0) return 1;
	
	pciConfigSpace->PciBusID = gPciBusID;
	pciConfigSpace->PciDevID = gPciDevID;
	pciConfigSpace->PciFncID = gPciFncID;

	PCIReadWord  (0x00, (sgl_uint16 *)&Data);
	pciConfigSpace->VendorID = (sgl_uint16)Data;
	PCIReadWord  (0x02, (sgl_uint16 *)&Data);
	pciConfigSpace->DeviceID = (sgl_uint16)Data;
	PCIReadWord  (0x04, (sgl_uint16 *)&Data);
	pciConfigSpace->Command = (sgl_uint16)Data;
	PCIReadWord  (0x06, (sgl_uint16 *)&Data);
	pciConfigSpace->Status = (sgl_uint16)Data;
	PCIReadByte  (0x08, (sgl_uint8 *)&Data);
	pciConfigSpace->RevisionID = (sgl_uint8)Data;
	PCIReadDword (0x08, (sgl_uint32 *)&Data);
	pciConfigSpace->ClassCode = (sgl_uint32)(Data >> 8);
	PCIReadByte  (0x0C, (sgl_uint8 *)&Data);
	pciConfigSpace->CacheLineSize = (sgl_uint8)Data;
	PCIReadByte  (0x0D, (sgl_uint8 *)&Data);
	pciConfigSpace->LatencyTimer = (sgl_uint8)Data;
	PCIReadByte  (0x0E, (sgl_uint8 *)&Data);
	pciConfigSpace->HeaderType = (sgl_uint8)Data;
	PCIReadByte  (0x0F, (sgl_uint8 *)&Data);
	pciConfigSpace->BIST = (sgl_uint8)Data;
	PCIReadDword (0x10, (sgl_uint32 *)&Data);
	pciConfigSpace->BaseAddress[0] = Data;
	PCIReadDword (0x14, (sgl_uint32 *)&Data);
	pciConfigSpace->BaseAddress[1] = Data;
	PCIReadDword (0x18, (sgl_uint32 *)&Data);
	pciConfigSpace->BaseAddress[2] = Data;
	PCIReadDword (0x1C, (sgl_uint32 *)&Data);
	pciConfigSpace->BaseAddress[3] = Data;
	PCIReadDword (0x20, (sgl_uint32 *)&Data);
	pciConfigSpace->BaseAddress[4] = Data;
	PCIReadDword (0x24, (sgl_uint32 *)&Data);
	pciConfigSpace->BaseAddress[5] = Data;
	PCIReadDword (0x28, (sgl_uint32 *)&Data);
	pciConfigSpace->CardbusCISPointer = Data;
	PCIReadWord  (0x2C, (sgl_uint16 *)&Data);
	pciConfigSpace->SubsystemVendorID = (sgl_uint16)Data;
	PCIReadWord  (0x2E, (sgl_uint16 *)&Data);
	pciConfigSpace->SubsystemID = (sgl_uint16)Data;
	PCIReadDword (0x30, (sgl_uint32 *)&Data);
	pciConfigSpace->ExpansionROMBaseAddress = Data;
	PCIReadDword (0x34, (sgl_uint32 *)&Data);
	pciConfigSpace->Reserved1 = Data;
	PCIReadDword (0x38, (sgl_uint32 *)&Data);
	pciConfigSpace->Reserved2 = Data;
	PCIReadByte  (0x3C, (sgl_uint8 *)&Data);
	pciConfigSpace->InterruptLine = (sgl_uint8)Data;
	PCIReadByte  (0x3D, (sgl_uint8 *)&Data);
	pciConfigSpace->InterruptPin = (sgl_uint8)Data;
	PCIReadByte  (0x3E, (sgl_uint8 *)&Data);
	pciConfigSpace->Min_Gnt = (sgl_uint8)Data;
	PCIReadByte  (0x3F, (sgl_uint8 *)&Data);
	pciConfigSpace->Max_Lat = (sgl_uint8)Data;

	return 0;
}


sgl_uint32 PCIReadByte (sgl_uint16 wReg, sgl_uint8 *Data)
{
	union _REGS inregs, outregs;


	if (gDeviceSetup == 0) return 1;
	
	inregs.h.bh = gPciBusID;
	inregs.h.bl = (gPciDevID << 3) + gPciFncID;
	inregs.w.di = wReg;


	if (gNECPC98)
	{
		inregs.w.ax = (READ_CONFIGURATION_BYTE_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = READ_CONFIGURATION_BYTE_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
		
	*Data = outregs.h.cl;
		
	return 0 ;
}


sgl_uint32 PCIReadWord (sgl_uint16 wReg, sgl_uint16 *Data)
{
	union _REGS inregs, outregs;


	if (gDeviceSetup == 0) return 1;
	
	inregs.h.bh = gPciBusID;
	inregs.h.bl = (gPciDevID << 3) + gPciFncID;
	inregs.w.di = wReg;

	if (gNECPC98)
	{
		inregs.w.ax = (READ_CONFIGURATION_WORD_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = READ_CONFIGURATION_WORD_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
		
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
	
	*Data = outregs.w.cx;
		
	return 0;
}


sgl_uint32 PCIReadDword (sgl_uint16 wReg, sgl_uint32 *Data)
{
	sgl_uint16 DataHi;
	sgl_uint16 DataLo;
	sgl_uint32 error;


	if (gDeviceSetup == 0) return 1;
		
	error = PCIReadWord (wReg+0, &DataLo);
	if (error != 0) return error;
	error = PCIReadWord (wReg+2, &DataHi);
	if (error != 0) return error;
	
	*Data = ((sgl_uint32)DataHi << 16) + DataLo;
	
	return 0;
}


sgl_uint32 PCIWriteByte (sgl_uint16 wReg, sgl_uint8  Data)
{
	union _REGS inregs, outregs;


	if (gDeviceSetup == 0) return 1;
		
	inregs.h.bh = gPciBusID;
	inregs.h.bl = (gPciDevID << 3) + gPciFncID;
	inregs.w.di = wReg;
	inregs.h.cl = Data;

	if (gNECPC98)
	{
		inregs.w.ax = (WRITE_CONFIGURATION_BYTE_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = WRITE_CONFIGURATION_BYTE_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
	
	return 0;
}


sgl_uint32 PCIWriteWord (sgl_uint16 wReg, sgl_uint16 Data)
{
	union _REGS inregs, outregs;

	
	if (gDeviceSetup == 0) return 1;

	inregs.h.bh = gPciBusID;
	inregs.h.bl = (gPciDevID << 3) + gPciFncID;
	inregs.w.di = wReg;
	inregs.w.cx = Data;

	if (gNECPC98)
	{
		inregs.w.ax = (WRITE_CONFIGURATION_WORD_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = WRITE_CONFIGURATION_WORD_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
		
	return 0;
}


sgl_uint32 PCIWriteDword (sgl_uint16 wReg, sgl_uint32 Data)
{
	sgl_uint16 DataHi;
	sgl_uint16 DataLo;
	sgl_uint32 error;


	if (gDeviceSetup == 0) return 1;
		
	DataLo = (Data & 0xFFFF);
	DataHi = (Data >> 16);
	
	error = PCIWriteWord (wReg+0, DataLo);
	if (error != 0) return error;
		
	error = PCIWriteWord (wReg+2, DataHi);
	if (error != 0) return error;
	
	return 0;
}

/*****************************************************************************/
/* Generic PCI Functions */

sgl_uint32 GenericPCIFindDevice( sgl_uint8 *bBusIDNum, 
                                 sgl_uint8 *bDevFncID, 
                                 sgl_uint16 vendor, 
                                 sgl_uint16 device,
                                 sgl_uint16 index )
{
	union _REGS inregs, outregs;
        
	inregs.w.cx = device;
	inregs.w.dx = vendor;
	inregs.w.si = index;
	
	if (gNECPC98)
	{
		inregs.w.ax = (FIND_PCI_DEVICE_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = FIND_PCI_DEVICE_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
	
	*bBusIDNum    = outregs.h.bh;
	*bDevFncID    = outregs.h.bl;

	return 0;
}

sgl_uint32 GenericPCIReadByte ( sgl_uint8 bBusIDNum,
                                sgl_uint8 bDevFncID,
                                sgl_uint16 wReg, 
                                sgl_uint8 *Data )
{
	union _REGS inregs, outregs;


	inregs.h.bh = bBusIDNum;
	inregs.h.bl = bDevFncID;
	inregs.w.di = wReg;


	if (gNECPC98)
	{
		inregs.w.ax = (READ_CONFIGURATION_BYTE_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = READ_CONFIGURATION_BYTE_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
		
	*Data = outregs.h.cl;
		
	return 0 ;
}


sgl_uint32 GenericPCIReadWord ( sgl_uint8 bBusIDNum,
                                sgl_uint8 bDevFncID,
                                sgl_uint16 wReg, 
                                sgl_uint16 *Data )
{
	union _REGS inregs, outregs;


	inregs.h.bh = bBusIDNum;
	inregs.h.bl = bDevFncID;
	inregs.w.di = wReg;

	if (gNECPC98)
	{
		inregs.w.ax = (READ_CONFIGURATION_WORD_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = READ_CONFIGURATION_WORD_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
		
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
	
	*Data = outregs.w.cx;
		
	return 0;
}

sgl_uint32 GenericPCIWriteByte ( sgl_uint8 bBusIDNum,
                                 sgl_uint8 bDevFncID,
                                 sgl_uint16 wReg, 
                                 sgl_uint8  Data )
{
	union _REGS inregs, outregs;


	inregs.h.bh = bBusIDNum;
	inregs.h.bl = bDevFncID;
	inregs.w.di = wReg;
	inregs.h.cl = Data;

	if (gNECPC98)
	{
		inregs.w.ax = (WRITE_CONFIGURATION_BYTE_16 & 0x00FF) | 0xCC00;
		int386 ( 0x1F, &inregs, &outregs );
	}
	else
	{
		inregs.w.ax = WRITE_CONFIGURATION_BYTE_16;
		int386 ( 0x1A, &inregs, &outregs );
	}
	
	if ( outregs.x.cflag != 0 ) return ( outregs.h.ah );
	
	return 0;
}

