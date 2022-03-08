/******************************************************************************
 * Name			: defs.h
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
 * Log: $Log: defs.h,v $
 * Revision 1.3  1997/04/03  14:03:06  jop
 * moved static def out of here
 *
 * Revision 1.2  1997/03/31  19:01:14  mlj
 * Added log macro
 *
 *
 *****************************************************************************/

#ifndef __DEFS_H__
#define __DEFS_H__

#define INSTALLATION_CHECK_16           0xB101
#define FIND_PCI_DEVICE_16              0xB102
#define FIND_PCI_CLASS_CODE_16          0xB103
#define PCI_BUS_SPECIFIC_OPERATIONS_16  0xB106
#define READ_CONFIGURATION_BYTE_16      0xB108
#define READ_CONFIGURATION_WORD_16      0xB109
#define READ_CONFIGURATION_DWORD_16     0xB10A
#define WRITE_CONFIGURATION_BYTE_16     0xB10B
#define WRITE_CONFIGURATION_WORD_16     0xB10C
#define WRITE_CONFIGURATION_DWORD_16    0xB10D

/* PCI Product ID Numbers */
#define WEITEK_VENDOR_ID                0x100E  /* Weitek         */ 
#define P9100_DEVICE_ID                 0x9100  /* Power 9100     */ 

#define S3_VENDOR_ID                    0x5333  /* S3             */ 
#define S732_DEVICE_ID                  0x8810  /* 732 Trio32     */
#define S764_DEVICE_ID                  0x8811  /* 764 Trio64     */
#define S864_DEVICE_ID                  0x88C0  /* 864            */
#define S868_DEVICE_ID                  0x8880  /* 868            */
#define S968_DEVICE_ID                  0x88F0  /* 968            */

#define NEC_VENDOR_ID                   0x1033  /* NEC            */
#define MIDAS_PCI_BRIDGE_DEVICE_ID      0x001F  /* ISP/TSP Bridge */
#define PCX1_DEVICE_ID                  0x002A  /* PCX1           */
#define PCX2_DEVICE_ID                  0x0046  /* PCX2           */

typedef struct tagISRBLOCK
{
        unsigned long   dwIRQ;                 /* IRQ parameter for VxD */
        unsigned long   EndOfRender;           /* semaphore twiddled by ISR */
        unsigned long   FrameBufferControl;    /* semaphore read by ISR */
        unsigned long   RenderMode;            /* semaphore read by ISR - 0=DAC, 1=XY, 2=Raster */
        unsigned long   BridgeRegs;            /* io port register base */
        void *          ISPRegs;               /* memory mapped register base */
        void *          TSPRegs;               /* memory mapped register base */
} ISRBLOCK, *PISRBLOCK;

typedef struct rminfo
{
	long EDI;
	long ESI;
	long EBP;
	long reserved_by_system;
	long EBX;
	long EDX;
	long ECX;
	long EAX;
	short flags;

	short ES,DS,FS,GS,IP,CS,SP,SS;
} rminfo;

unsigned long GetPDBR (void);

#pragma aux GetPDBR =         \
	"mov   eax, cr3"          \
	modify [eax]              \
	value  [eax];

unsigned long GetControlRegister0 (void);

#pragma aux GetControlRegister0 = \
	"mov   eax, cr0"              \
	modify [eax]                  \
	value  [eax];

#endif
