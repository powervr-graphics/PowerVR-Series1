/******************************************************************************
 * Name         : irq.c
 * Title        : Mick and Jims little ISR module
 * Author       : Mick Jones and Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: irq.c,v $
 * Revision 1.6  1997/05/12  12:53:40  mlj
 * removed the irq mask set to 0 from KillISR
 *
 * Revision 1.5  1997/04/15  17:29:22  jop
 * Tidied up isr exported data and reconfigure the file for
 * added real mode interrupt handler
 *
 * Revision 1.4  1997/04/15  10:42:52  jop
 * Added call to DPMI fn 303 to handle RM ins better
 *
 * Revision 1.3  1997/04/09  02:52:51  jop
 * Completely changed to a bi-modal scheme
 *
 * Revision 1.2  1997/04/08  11:11:12  jop
 * Hopefully improved irq hooking
 *
 * Revision 1.1  1997/04/04  16:30:50  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>
#include <string.h>

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "hwregs.h"

#include "defs.h"
#include "irq.h"
#include "isr.h"
#include "pci.h"

#define D32RealSeg(P)   ((((sgl_uint32) (P)) >> 4) & 0xFFFF)
#define D32RealOff(P)   (((sgl_uint32) (P)) & 0xF)
#define D32RealMkP(S,O)   ((((sgl_uint32) (S) << 4)) | ((sgl_uint32) (O) & 0xF))

sgl_uint32 *pStatusFlags;

static void 		*pRMIntHandler = NULL;
static void __far 	*ISR_old_protmode = NULL;
static sgl_uint32	ISR_old_realmode = NULL;
static sgl_uint32 	VectorIsEnabled = FALSE;
static sgl_uint32 	VectorIsSet = FALSE;

extern BoardDataBlock gBoardDataBlock;

RMDATA *pLocalRMData = 0;

void RetriggerPIC (sgl_uint8 IRQ);

#pragma aux RetriggerPIC =    \
	"pushf "             \
	"pushf "             \
	"pop   bx"             \
	"test  bh,2"			\
	"jz	   RPIC_1"			\
	"cli"					\
	"RPIC_1:"				\
	"cmp   al, 7"             \
	"jle   isr_001"           \
	"mov   dx, 0A0h"          \
	"mov   al, 20h"           \
	"out   dx, al"            \
"isr_001:"                    \
	"mov   dx, 20h"           \
	"mov   al, 20h"           \
	"out   dx, al"            \			 
	"pop   bx"             \
	"test  bh,2"			\
	"jz	   RPIC_2"			\
	"sti"					\
	"RPIC_2:"				\
	parm [al]                 \
	modify [dx bx];

sgl_uint32 SetupPIC (sgl_uint8 IRQ, sgl_uint8 Disable);

#pragma aux SetupPIC =        \
	"mov   dx, 021h"          \
	"cmp   cl, 8"             \
	"jb    SetIntMask_Master" \
	"and   cl, 07h"		  	  \
	"mov   dx, 0A1h"          \
"SetIntMask_Master:"          \
	"mov   ch, 1"             \
	"shl   ch, cl"            \
	"mov   cl, al"            \
	"or    cl, cl"            \
	"jz    SetIntMask_UnMask" \
	"mov   cl,ch"             \
"SetIntMask_UnMask:"          \
	"not   ch"                \
	"pushf"                   \
	"pushf"                   \
	"pop   bx"                \
	"test  bh,2"              \
	"jz    loc_0309"          \
	"cli"                     \
"loc_0309:"                   \
	"in    al, dx"            \ 
	"mov   ah, al"            \
	"and   al, ch"            \
	"or    al, cl"            \
	"cmp   al, ah"            \
	"je    SetIntMask_Same"   \
	"out   dx, al"            \
"SetIntMask_Same:"            \
	"pop   bx"                \
	"test  bh,2"              \
	"jz    loc_0311"          \
	"sti"                     \
"loc_0311:"	                  \
	"not   ch"                \
	"mov   al, ah"            \
	"and   al, ch"            \
	"xor   ah, ah"            \
	parm [cl] [al]            \
	modify [eax ebx ecx edx]  \
	value [eax];	

static sgl_uint8 MapIRQ (sgl_uint8 irq)
{
	/* IRQ to vector table index */

	if (irq == 0)  return 0x08;
	if (irq == 1)  return 0x09;
	if (irq == 2)  return 0x0A;
	if (irq == 3)  return 0x0B;
	if (irq == 4)  return 0x0C;
	if (irq == 5)  return 0x0D;
	if (irq == 6)  return 0x0E;
	if (irq == 7)  return 0x0F;
	if (irq == 8)  return 0x70;
	if (irq == 9)  return 0x71;
	if (irq == 10) return 0x72;
	if (irq == 11) return 0x73;
	if (irq == 12) return 0x74;
	if (irq == 13) return 0x75;
	if (irq == 14) return 0x76;
	if (irq == 15) return 0x77;

	/* as close as you   */
	/* can get to a safe */
	/* IRQ .....!        */

	return 0x71;
}

void *D32DosMemAlloc (sgl_uint32 size)
{
	union REGS r;

	r.x.eax = 0x0100;           /* DPMI allocate DOS memory */
	r.x.ebx = (size + 15) >> 4; /* Number of paragraphs requested */

	int386 (0x31, &r, &r);

	if( r.x.cflag )  /* Failed */
		return ((sgl_uint32) 0);
		
	return (void *) ((r.x.eax & 0xFFFF) << 4);
}

void far *GetPMInterruptHandler (sgl_uint32 IRQ)
{
	union REGS 		r;
	struct SREGS    sr;

	r.x.eax = 0x3500 | IRQ;   /* DOS get vector */
	sr.ds = sr.es = 0;
	
	int386x (0x21, &r, &r, &sr);
	
	return (MK_FP (sr.es, r.x.ebx));
}	

sgl_uint32 GetRMInterruptHandler (sgl_uint32 IRQ)
{
	union REGS r;

	r.x.eax = 0x0200;   /* DPMI get real mode vector */
	r.h.bl = IRQ;

	int386 (0x31, &r, &r);

	return (D32RealMkP (r.x.ecx, r.x.edx));
}	

void SetPMInterruptHandler (sgl_uint32 IRQ, void far *fn)
{
	union REGS 		r;
	struct SREGS    sr;

	r.x.eax = 0x2500 | IRQ;   /* DOS set vector (INT 0Ch) */
	/* DS:EDX == &handler */
	sr.ds = FP_SEG (fn);
	r.x.edx = FP_OFF (fn);
	sr.es = 0;

	int386x (0x21, &r, &r, &sr);
}	

void SetRMInterruptHandler (sgl_uint32 IRQ, sgl_uint32 fn)
{
	union REGS r;

	r.x.eax = 0x0201;

	r.h.bl = IRQ;
	/* CX:DX == real mode &handler */
	r.x.ecx = D32RealSeg(fn);
	r.x.edx = D32RealOff(fn);
	
	int386 (0x31, &r, &r);
}	

#if 0
/* would have been nice - DOS4G/W doesn't support it. */
sgl_uint32 GetRMPtrToPMCallback (void far *fn, void far *RMI)
{
	union REGS r;
	struct SREGS    sr;

	r.x.eax = 0x0303;

	/* DS:ESI == &PM fn */
	sr.ds = FP_SEG (fn);
	r.x.esi = FP_OFF (fn);
	/* ES:EDI == &RMI data */
	sr.es = FP_SEG (RMI);
	r.x.edi = FP_OFF (RMI);
	
	int386x (0x31, &r, &r, &sr);
	
	if (!r.x.cflag)
	{
		return (D32RealMkP (r.x.ecx & 0xFFFF, r.x.edx & 0xFFFF));
	}
	else
	{
		PVROSPrintf ("GetRMPtrToPMCallback: failed\n");
	}
	
	return (0);
}	

void FreeRMPtrToPMCallback (sgl_uint32 fn)
{
	union REGS r;
	struct SREGS    sr;

	r.x.eax = 0x0304;

	/* CX:DX == real mode &handler */
	r.x.ecx = D32RealSeg(fn);
	r.x.edx = D32RealOff(fn);

	int386 (0x31, &r, &r);
}	
#endif

sgl_bool SetupISR (void)
{
	if (!VectorIsSet)
	{
		pRMIntHandler = D32DosMemAlloc (256);
		
		if (!pRMIntHandler)
		{
			DPF ((DBG_ERROR, "SetIRQ: error getting low dos mem"));
		}
		else
		{
			sgl_uint32 LocalDataOffset;
								
			memcpy (pRMIntHandler, (void *) ISR_pcx_realmode, 256);

			LocalDataOffset = (sgl_uint32) &RMData - (sgl_uint32) ISR_pcx_realmode;

			ASSERT (LocalDataOffset < (256 - sizeof (RMDATA)));

			pLocalRMData = (sgl_uint32 *) ((sgl_uint32) pRMIntHandler + LocalDataOffset);
			
			pLocalRMData->RMStatusFlags = 0;
			pLocalRMData->RMBusDevFnc = ((gBoardDataBlock.PCIBus << 8) & 0xFF00) |
										((gBoardDataBlock.PCIDev << 3) & 0x00F8) |
										(gBoardDataBlock.PCIFunc & 0x0007);
			pLocalRMData->RMIRQ = gBoardDataBlock.IRQ;
			
			pStatusFlags = &pLocalRMData->RMStatusFlags;
			
			VectorIsSet = TRUE;
			
			return (TRUE);
		}
	}

	return (FALSE);
}

PVROSERR EnableISR (DEVICE_TYPE DeviceType)
{
	switch (DeviceType)
	{
		case MIDAS3:
		{
			break;
		}

		case MIDAS4:
		case MIDAS5:
		{
			if (VectorIsSet)
			{
				if (!VectorIsEnabled)
				{
					sgl_uint32 MappedIRQ = MapIRQ (gBoardDataBlock.IRQ);
										
					DPF ((DBG_MESSAGE, "Enabling IRQ vector %d ...", gBoardDataBlock.IRQ));

					PVROSPrintf("Enabling IRQ vector %d ...", gBoardDataBlock.IRQ);
					
					PMData.PMStatusFlags = (sgl_uint32) &pLocalRMData->RMStatusFlags;
					PMData.PMRegs = gBoardDataBlock.LinearMemWindows[0];
					PMData.PMIRQ = gBoardDataBlock.IRQ;

					ISR_old_protmode = GetPMInterruptHandler (MappedIRQ);
					ISR_old_realmode = GetRMInterruptHandler (MappedIRQ);
					SetPMInterruptHandler (MappedIRQ, (void far *) (void *) ISR_pcx_protmode);
					SetRMInterruptHandler (MappedIRQ, (sgl_uint32) (void *) pRMIntHandler);
			
					SetupPIC (gBoardDataBlock.IRQ, 0);
					
					VectorIsEnabled = TRUE;
		
					return (PVROS_GROOVY);
				}
			}

			break;
		}

		case DISABLE_BOARD:
		default:
		{
			if (VectorIsSet)
			{
				if (VectorIsEnabled)
				{
					sgl_uint32 MappedIRQ = MapIRQ (gBoardDataBlock.IRQ);

					VectorIsEnabled = FALSE;

					DPF ((DBG_MESSAGE, "Disabling IRQ vector %d", gBoardDataBlock.IRQ));

					PVROSPrintf("Disabling IRQ vector %d", gBoardDataBlock.IRQ);

					KillISR ();

					SetupPIC (gBoardDataBlock.IRQ, 1);

					SetRMInterruptHandler (MappedIRQ, ISR_old_realmode);
					SetPMInterruptHandler (MappedIRQ, ISR_old_protmode);
				}
			}
						
			return (PVROS_GROOVY);

			break;
		}
	}

	return (PVROS_DODGY);
}

void RetriggerISR (void)
{
#if 0
	if (VectorIsSet && VectorIsEnabled)
	{
		sgl_uint32 *pPCXRegs = (sgl_uint32 *) PCXRegs;
		
		DPF ((DBG_WARNING, "RetriggerISR"));
		
		if (pPCXRegs[PCX_INTSTATUS] & pPCXRegs[PCX_INTMASK])
		{
			RetriggerPIC (gBoardDataBlock.IRQ);
		}
	}
#endif
}	

void KillISR (void)
{
	sgl_uint32 *pPCXRegs = (sgl_uint32 *) gBoardDataBlock.LinearMemWindows[0];
	sgl_uint32 IntStatus;
	
	IntStatus = pPCXRegs[PCX_INTSTATUS] & pPCXRegs[PCX_INTMASK];
	
//	pPCXRegs[PCX_INTMASK] = 0; 
}	

/* end of $RCSfile: irq.c,v $ */
