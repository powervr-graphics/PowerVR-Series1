/******************************************************************************
 * Name			: pci.h
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


typedef struct PciConfigSpace
{
        sgl_uint32 PciBusID;
        sgl_uint32 PciDevID;
        sgl_uint32 PciFncID;
        
        sgl_uint32 VendorID;
        sgl_uint32 DeviceID;
        sgl_uint32 Command;
        sgl_uint32 Status;
        sgl_uint32 RevisionID;
        sgl_uint32 ClassCode;
        sgl_uint32 CacheLineSize;
        sgl_uint32 LatencyTimer;
        sgl_uint32 HeaderType;
        sgl_uint32 BIST;
        sgl_uint32 BaseAddress [6];
        sgl_uint32 CardbusCISPointer;
        sgl_uint32 SubsystemVendorID;
        sgl_uint32 SubsystemID;
        sgl_uint32 ExpansionROMBaseAddress;
        sgl_uint32 Reserved1;
        sgl_uint32 Reserved2;
        sgl_uint32 InterruptLine;
        sgl_uint32 InterruptPin;
        sgl_uint32 Min_Gnt;
        sgl_uint32 Max_Lat;
} PciConfigSpace;

/* both these functions set the device to be writen to */
/* however PCISetDevice will force the device to be    */
/* used even if it finds it does not exist             */
sgl_uint32   PCISetDevice              (sgl_uint8 busNum,  sgl_uint8 devNum,  sgl_uint8 fncNum);
sgl_uint32   PCIFindDevice             (sgl_uint16 device, sgl_uint16 vendor, sgl_uint16 index);

sgl_uint32   PCIReadByte               (sgl_uint16 wReg,   sgl_uint8  *Data);
sgl_uint32   PCIReadWord               (sgl_uint16 wReg,   sgl_uint16 *Data);
sgl_uint32   PCIReadDword              (sgl_uint16 wReg,   sgl_uint32 *Data);
sgl_uint32   PCIWriteByte              (sgl_uint16 wReg,   sgl_uint8   Data);
sgl_uint32   PCIWriteWord              (sgl_uint16 wReg,   sgl_uint16  Data);
sgl_uint32   PCIWriteDword             (sgl_uint16 wReg,   sgl_uint32  Data);

sgl_uint32   PrintPciConfigSpace       (void);
sgl_uint32   PCIFindAllDevices         (PciConfigSpace *pciConfigSpace);
sgl_uint32   PCIGetConfigSpace         (PciConfigSpace *pciConfigSpace);

/* Generic PCI Functions */
sgl_uint32 GenericPCIFindDevice( sgl_uint8 *bBusIDNum, 
                                 sgl_uint8 *bDevFncID, 
                                 sgl_uint16 vendor, 
                                 sgl_uint16 device,
                                 sgl_uint16 index );
sgl_uint32 GenericPCIReadByte ( sgl_uint8 bBusIDNum,
                                sgl_uint8 bDevFncID,
                                sgl_uint16 wReg, 
                                sgl_uint8 *Data );
sgl_uint32 GenericPCIReadWord ( sgl_uint8 bBusIDNum,
                                sgl_uint8 bDevFncID,
                                sgl_uint16 wReg, 
                                sgl_uint16 *Data );
sgl_uint32 GenericPCIWriteByte ( sgl_uint8 bBusIDNum,
                                 sgl_uint8 bDevFncID,
                                 sgl_uint16 wReg, 
                                 sgl_uint8  Data );

