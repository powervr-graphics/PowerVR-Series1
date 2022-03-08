/******************************************************************************
 * Name			: vds.h
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


#define EDDS_PHYSICAL_REGIONS 256

typedef struct VDSINFO
{
	sgl_uint8  MajorSpecNumber;
	sgl_uint8  MinorSpecNumber;
	sgl_uint16 ProductNumber;
	sgl_uint16 ProductRevNumber;
	sgl_uint32 MaximumDmaBuffer;
	sgl_uint16 Flags;
} VDSINFO;

typedef struct DDS
{
	sgl_uint32 RegionSize;
	sgl_uint32 Offset;
	sgl_uint16 SegOrSelect;
	sgl_uint16 BufferID;
	sgl_uint32 PhysicalAddress;
} DDS;

/* Extended Dma Descriptor Structure */

typedef struct EDDS
{
	sgl_uint32 RegionSize;
	sgl_uint32 Offset;
	sgl_uint16 SegOrSelect;
	sgl_uint16 Reserved;
	sgl_uint16 NumberAvailable;
	sgl_uint16 NumberUsed;
	sgl_uint32 DataRegion[EDDS_PHYSICAL_REGIONS * 2];
} EDDS;


sgl_uint16 GetVbsVersion            (VDSINFO *vdsinfo);
sgl_uint16 LockDmaBufferRegion      (sgl_uint32 regionSize, void *dmaBuffer, DDS  *dmaDescriptorStructure);
sgl_uint16 ScatterGatherLockRegion  (sgl_uint32 regionSize, void *dmaBuffer, EDDS *dmaDescriptorStructure);
sgl_uint16 UnlockDmaBufferRegion    (DDS  *dmaDescriptorStructure);
sgl_uint16 ScatterGatherUnockRegion (EDDS *dmaDescriptorStructure);
sgl_uint16 RequestDmaBuffer         (sgl_uint32 regionSize, DDS *dmaDescriptorStructure);
sgl_uint16 ReleaseDmaBuffer         (DDS *dmaDescriptorStructure);
