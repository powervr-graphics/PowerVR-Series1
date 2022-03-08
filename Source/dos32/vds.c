/******************************************************************************
 * Name			: vds.c
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
 *
 *
 *  		20th March 1997		Initial Version		Mick Jones
 *
 *****************************************************************************/


#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "vds.h"
#include "defs.h"
#include "vesa.h"
#include "pci.h"

sgl_uint16 GetVbsVersion (VDSINFO *vdsinfo)
{
	union  REGS       regs;
	sgl_uint8        *pByteLin;

	
	pByteLin = (char *)0x0000047B;

	memset (vdsinfo,0,sizeof(VDSINFO));

	if (*pByteLin & 0x10) // is bit 5 set 
	{
		regs.w.ax  = 0x8102;
		regs.w.dx  = 0x0000;
		int386(0x4B, &regs, &regs);

		if ( regs.w.cflag != 0 )
		{
		    PVROSPrintf ("VDS INT 8102h FAILED \n");
		    return ( regs.w.ax );
		}

		vdsinfo->MajorSpecNumber  = regs.h.ah;
		vdsinfo->MinorSpecNumber  = regs.h.al;
		vdsinfo->ProductNumber    = regs.w.bx;
		vdsinfo->ProductRevNumber = regs.w.cx;
		vdsinfo->MaximumDmaBuffer = ((sgl_uint32)regs.w.si << 16) + regs.w.di;
		vdsinfo->Flags            = regs.w.dx;
	}
	else
	{
		return 1;
	}

	return 0;
}


sgl_uint16 LockDmaBufferRegion (sgl_uint32 regionSize, void *dmaBuffer, DDS *dmaDescriptorStructure)
{
	DDS          far *pDmaDescriptorStructure;
	
	union  REGS       regs;
	struct SREGS      sregs;
	short             selector;
	short             segment;

	rminfo 	RMI;

	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 1; /* 16 bytes */
	int386( 0x31, &regs, &regs);	
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	pDmaDescriptorStructure = MK_FP(selector,0);

	/* tell VDS how much memory were hopeing */
	/* to get from the buffer                */
	pDmaDescriptorStructure->RegionSize      = regionSize;
	pDmaDescriptorStructure->Offset          = FP_OFF(dmaBuffer);
	pDmaDescriptorStructure->SegOrSelect     = FP_SEG(dmaBuffer);
	pDmaDescriptorStructure->BufferID        = 0;
	pDmaDescriptorStructure->PhysicalAddress = 0;

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00008103;  /* VDS BIOS call 8103h lock DMA buffer              */
	RMI.EDX = 0x00000000;  /* copy data from buffer to VDS auto alloced buffer */	
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x4B;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if (RMI.flags & 1)
	{
	    PVROSPrintf ("VDS INT 8103h FAILED, AL = %02X\n",RMI.EAX & 0xFF);
	    return (RMI.EAX & 0xFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED \n");
	    return ( regs.w.ax );
	}

	_fmemcpy (dmaDescriptorStructure,pDmaDescriptorStructure,sizeof(DDS));

	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = selector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
		PVROSPrintf ("DPMI INT 101h FAILED\n");
		return ( regs.w.ax );
	}

	return 0;
}


sgl_uint16 UnlockDmaBufferRegion (DDS *dmaDescriptorStructure)
{
	DDS          far *pDmaDescriptorStructure;
	
	union  REGS       regs;
	struct SREGS      sregs;
	short             selector;
	short             segment;
	
	rminfo 	RMI;
	
	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 1; /* 16 bytes */
	int386( 0x31, &regs, &regs);	
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	pDmaDescriptorStructure = MK_FP(selector,0);

	pDmaDescriptorStructure->RegionSize      = dmaDescriptorStructure->RegionSize;
	pDmaDescriptorStructure->Offset          = dmaDescriptorStructure->Offset;
	pDmaDescriptorStructure->SegOrSelect     = dmaDescriptorStructure->SegOrSelect;
	pDmaDescriptorStructure->BufferID        = dmaDescriptorStructure->BufferID;
	pDmaDescriptorStructure->PhysicalAddress = dmaDescriptorStructure->PhysicalAddress;

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00008104;  /* VDS BIOS call 8104h unlock DMA buffer            */
	RMI.EDX = 0x00000000;  /* copy data from buffer to VDS auto alloced buffer */	
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x4B;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if (RMI.flags & 1)
	{
	    PVROSPrintf ("VDS INT 8104h FAILED, AL = %02X\n",RMI.EAX & 0xFF);
	    return (RMI.EAX & 0xFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED \n");
	    return ( regs.w.ax );
	}

	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = selector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
		PVROSPrintf ("DPMI INT 101h FAILED\n");
		return ( regs.w.ax );
	}

	
	return 0;
}


sgl_uint16 ScatterGatherLockRegion (sgl_uint32 regionSize, void *dmaBuffer, EDDS *dmaDescriptorStructure)
{
	EDDS         far *pDmaDescriptorStructure;

	union  REGS       regs;
	struct SREGS      sregs;
	short             selector;
	short             segment;

	sgl_uint32        regionsRequired;

	rminfo 	RMI;

	regionsRequired = (((sgl_uint32)dmaBuffer & 0x0FFF) + regionSize + 0x0FFF) / 0x1000;

	if (regionsRequired > EDDS_PHYSICAL_REGIONS)
	{
		PVROSPrintf ("Regions Required = %d\n",regionsRequired);
		return regionsRequired;
	}

	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 129; /* 2064 bytes */
	int386( 0x31, &regs, &regs);
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	pDmaDescriptorStructure = MK_FP(selector,0);

	/* tell VDS how much memory were hopeing */
	/* to get from the buffer                */
	pDmaDescriptorStructure->RegionSize      = regionSize;
	pDmaDescriptorStructure->Offset          = FP_OFF(dmaBuffer);
	pDmaDescriptorStructure->SegOrSelect     = FP_SEG(dmaBuffer);
	pDmaDescriptorStructure->NumberAvailable = EDDS_PHYSICAL_REGIONS;

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00008105;  /* VDS BIOS call 8105h scater/gather lock DMA buffer */
	RMI.EDX = 0x00000000;
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x4B;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if (RMI.flags & 1)
	{
	    PVROSPrintf ("VDS INT 8105h FAILED, AL = %02X\n",RMI.EAX & 0xFF);
	    return (RMI.EAX & 0xFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED \n");
	    return ( regs.w.ax );
	}

	_fmemcpy (dmaDescriptorStructure,pDmaDescriptorStructure,sizeof(EDDS));

	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = selector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
		PVROSPrintf ("DPMI INT 101h FAILED\n");
		return ( regs.w.ax );
	}

	return 0;
}


sgl_uint16 ScatterGatherUnockRegion (EDDS *dmaDescriptorStructure)
{
	EDDS         far *pDmaDescriptorStructure;
	
	union  REGS       regs;
	struct SREGS      sregs;
	short             selector;
	short             segment;

	rminfo 	RMI;
	
	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 129; /* 2064 bytes */
	int386( 0x31, &regs, &regs);	
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	pDmaDescriptorStructure = MK_FP(selector,0);

	_fmemcpy (pDmaDescriptorStructure,dmaDescriptorStructure,sizeof(EDDS));

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00008106;  /* VDS BIOS call 8106h scater/gather unlock DMA buffer */
	RMI.EDX = 0x00000000; 
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x4B;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if (RMI.flags & 1)
	{
	    PVROSPrintf ("VDS INT 8106h FAILED, AL = %02X\n",RMI.EAX & 0xFF);
	    return (RMI.EAX & 0xFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED \n");
	    return ( regs.w.ax );
	}

	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = selector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
		PVROSPrintf ("DPMI INT 101h FAILED\n");
		return ( regs.w.ax );
	}

	return 0;
}


sgl_uint16 RequestDmaBuffer (sgl_uint32 regionSize, DDS *dmaDescriptorStructure)
{
	DDS          far *pDmaDescriptorStructure;
	
	union  REGS       regs;
	struct SREGS      sregs;
	short             selector;
	short             segment;

	rminfo 	RMI;

	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 1; /* 16 bytes */
	int386( 0x31, &regs, &regs);	
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	pDmaDescriptorStructure = MK_FP(selector,0);

	/* tell VDS how much memory were hopeing */
	/* to get from the buffer                */
	pDmaDescriptorStructure->RegionSize      = regionSize;

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00008107;  /* VDS BIOS call 8107h request DMA buffer           */
	RMI.EDX = 0x00000000;  
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x4B;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if (RMI.flags & 1)
	{
	    PVROSPrintf ("VDS INT 8107h FAILED, AL = %02X\n",RMI.EAX & 0xFF);
	    return (RMI.EAX & 0xFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED \n");
	    return ( regs.w.ax );
	}

	_fmemcpy (dmaDescriptorStructure,pDmaDescriptorStructure,sizeof(DDS));

	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = selector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
		PVROSPrintf ("DPMI INT 101h FAILED\n");
		return ( regs.w.ax );
	}

	return 0;
}


sgl_uint16 ReleaseDmaBuffer (DDS *dmaDescriptorStructure)
{
	DDS          far *pDmaDescriptorStructure;
	
	union  REGS       regs;
	struct SREGS      sregs;
	short             selector;
	short             segment;

	rminfo 	RMI;

	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 1; /* 16 bytes */
	int386( 0x31, &regs, &regs);	
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	pDmaDescriptorStructure = MK_FP(selector,0);

	pDmaDescriptorStructure->BufferID = dmaDescriptorStructure->BufferID;

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00008108;  /* VDS BIOS call 8108h release DMA buffer           */
	RMI.EDX = 0x00000000;  
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x4B;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if (RMI.flags & 1)
	{
	    PVROSPrintf ("VDS INT 8108h FAILED, AL = %02X\n",RMI.EAX & 0xFF);
	    return (RMI.EAX & 0xFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED \n");
	    return ( regs.w.ax );
	}

	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = selector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
		PVROSPrintf ("DPMI INT 101h FAILED\n");
		return ( regs.w.ax );
	}

	return 0;
}



