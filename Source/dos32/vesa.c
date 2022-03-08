/******************************************************************************
 * Name			: vesa.c
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
 *  		20th March 1997		Initial Version		Mick Jones
 *
 *	$Log: vesa.c,v $
 * Revision 1.8  1997/06/05  12:08:01  sjf
 * Added RCS Log Macro.
 *
 *
 *****************************************************************************/


#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>

#include "sgl.h"
#include "pvrosapi.h"
#include "defs.h"
#include "vesa.h"


/* globals for the save/restore function */
short     gSelector;
short     gSegment;
	

sgl_uint32 GetCurrentVGAModeNumber (void)
{
	union REGS regs;
	
	regs.w.ax = 0x0F00;
	int386 (0x10, &regs, &regs);
	
	return (regs.w.ax & 0x00FF);	
}	

sgl_uint16 EnumerateVBE2Modes (ModeInfoBlockTable *ModeInfoTable)
{
	VbeInfoBlock         VBEInfo;
	ModeInfoBlock        ModeInfo;

	sgl_uint16           i;
	sgl_uint16          *modePtr;
	sgl_uint16           numberOfModes;
	

	ReturnVBE2ControllerInformation (&VBEInfo);

	
	if (ModeInfoTable == NULL)
	{
		for (i = 0; i < MAX_MODES; i++)
		{
			modePtr = (sgl_uint16*)(((VBEInfo.VideoModePtr & 0xFFFF0000) >> 12) + (VBEInfo.VideoModePtr & 0xFFFF) + (i << 1));

			if (*modePtr == 0xFFFF)
			{
				return ((sizeof(ModeInfoBlockTable) * i) + 4);
			}
		}
		return ((sizeof(ModeInfoBlockTable) * i) + 4);
	}
	else
	{
		for (i = 0; i < MAX_MODES; i++)
		{
			modePtr = (sgl_uint16*)(((VBEInfo.VideoModePtr & 0xFFFF0000) >> 12) + (VBEInfo.VideoModePtr & 0xFFFF) + (i << 1));

			if (*modePtr == 0xFFFF)
			{
				ModeInfoTable[i].Mode = 0xFFFFFFFF;
				numberOfModes = i;
				break;
			}

			ModeInfoTable[i].Mode = *modePtr;
		}
	}

	for (i = 0; i < numberOfModes; i++)
	{
		ReturnVBE2ModeInformation (ModeInfoTable[i].Mode, &ModeInfo);

		ModeInfoTable[i].XResolution  = ModeInfo.XResolution;
		ModeInfoTable[i].YResolution  = ModeInfo.YResolution;
		ModeInfoTable[i].BitsPerPixel = ModeInfo.BitsPerPixel;
	}

	return numberOfModes;
}



sgl_uint16 ReturnVBE2ControllerInformation (VbeInfoBlock *VBEInfo)
{
	union  REGS         regs;
	struct SREGS        sregs;
	short               selector;
	short               segment;
	VbeInfoBlock far    *VBEI;

	rminfo 	RMI;

	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 32; /* 512 bytes */
	int386( 0x31, &regs, &regs);	
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	VBEI = MK_FP(selector,0);

	/* let the VBE2 BIOS know we know about it, this */
	/* will cause the BIOS to fill out a 512 byte    */
	/* block insted of the usual 256 bytes           */
	VBEI->VbeSignature[0] = 'V';
	VBEI->VbeSignature[1] = 'B';
	VBEI->VbeSignature[2] = 'E';
	VBEI->VbeSignature[3] = '2';

	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00004F00;  /* VESA BIOS call 4F00h       */
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x10;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if ((RMI.EAX & 0xFFFF) != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F00h FAILED\n");
	    return (RMI.EAX & 0xFFFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED\n");
	    return ( regs.w.ax );
	}

	_fmemcpy (VBEInfo,VBEI,sizeof(VbeInfoBlock));

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


sgl_uint16 ReturnVBE2ModeInformation (sgl_uint16 Mode, ModeInfoBlock *ModeInfo)
{
	union  REGS         regs;
	struct SREGS        sregs;
	short               selector;
	short               segment;
	ModeInfoBlock far   *VBEMI;

	rminfo 	RMI;

	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = 16; /* 256 bytes */
	int386( 0x31, &regs, &regs);
	segment   = regs.w.ax;
	selector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}

	VBEMI = MK_FP(selector,0);
	
	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00004F01;  /* VESA BIOS call 4F01h       */
	RMI.ECX = Mode;        /* VESA mode                  */
	RMI.ES  = segment;     /* put DOS seg:off into ES:DI */
	RMI.EDI = 0x00000000;  /* DOS ignores EDI high word  */

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));

	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x10;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if ((RMI.EAX & 0xFFFF) != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F01h FAILED\n");
	    return (RMI.EAX & 0xFFFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED\n");
	    return ( regs.w.ax );
	}

	_fmemcpy (ModeInfo,VBEMI,sizeof(ModeInfoBlock));

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


sgl_uint16 SetVBE2Mode (sgl_uint16 Mode)
{
	union  REGS         regs;

	if (Mode < 0x100)
	{
		regs.w.ax  = Mode;                   /* VESA non-extended mode call  */
		int386( 0x10, &regs, &regs);
		
		// errors???
	}
	else
	{
		regs.w.ax  = 0x4F02;                   /* VESA BIOS call 4F02h  */
		regs.w.bx  = 0x4000 + (Mode & 0x1FF);  /* linear VESA mode      */
		int386( 0x10, &regs, &regs);

		if (regs.w.ax != 0x4F)
		{
		    PVROSPrintf ("VESA INT 4F02h FAILED\n");
		    return regs.w.ax;
		}
	}

	return 0;
}


sgl_uint16 ReturnCurrentVBE2Mode (void)
{
	union  REGS         regs;

	regs.w.ax  = 0x4F03;  /* VESA BIOS call 4F03h       */
	int386( 0x10, &regs, &regs);

	if (regs.w.ax != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F03h FAILED\n");
	    return regs.w.ax;
	}

	return (regs.w.bx & 0x3FFF);
}


sgl_uint16 SetVBE2DisplayStart (sgl_uint16 x, sgl_uint16 y, sgl_uint16 waitForVerticalRetrace)
{
	union  REGS         regs;

	regs.w.ax  = 0x4F07;        /* VESA BIOS call 4F07h  */

	if (waitForVerticalRetrace)
	{
		regs.w.bx = 0x0080;    /* set display during v-sync */
	}
	else
	{
		regs.w.bx = 0x0000;    /* set display */
	}
	
	regs.w.cx  = x;
	regs.w.dx  = y;
	int386( 0x10, &regs, &regs);

	if (regs.w.ax != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F07h FAILED\n");
	    return regs.w.ax;
	}

	return 0;
}


sgl_uint16 SaveVBE2State (void)
{
	union  REGS         regs;
	struct SREGS        sregs;
	short               bufferSize;

	rminfo 	RMI;

	memset(&sregs,0,sizeof(sregs));
	/* Get save/restore buffer size */
	regs.w.ax = 0x4F04;
	regs.w.cx = 0x000F;
	regs.w.dx = 0x0000;
	int386x(0x10, &regs, &regs, &sregs);

	if (regs.w.ax != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F04h FAILED\n");
	    return regs.w.ax;
	}
	
	bufferSize = regs.w.bx;


	/* DPMI call 100h allocates DOS memory */
	regs.w.ax = 0x100;
	regs.w.bx = bufferSize;
	int386( 0x31, &regs, &regs);
	gSegment   = regs.w.ax;
	gSelector  = regs.w.dx;

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 100h FAILED\n");
	    return ( regs.w.ax );
	}
	
	
	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00004F04;  
	RMI.EBX = 0x00000000;
	RMI.ECX = 0x0000000F;
	RMI.EDX = 0x00000001;       
	RMI.ES  = gSegment;

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));
	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x10;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs);

	if ((RMI.EAX & 0xFFFF) != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F04h FAILED\n");
	    return (RMI.EAX & 0xFFFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED\n");
	    return ( regs.w.ax );
	}
	
	
	return 0;
}


sgl_uint16 RestoreVBE2State (void)
{
	union  REGS         regs;
	struct SREGS        sregs;

	rminfo 	RMI;
	
	/* Set up real-mode call structure */
	memset(&RMI,0,sizeof(RMI));
	RMI.EAX = 0x00004F04;  
	RMI.EBX = 0x00000000;
	RMI.ECX = 0x0000000F;
	RMI.EDX = 0x00000002;       
	RMI.ES  = gSegment;

	/* zero contents of sregs */
	memset(&sregs,0,sizeof(sregs));
	/* Use DMPI call 300h to issue the DOS interrupt */
	regs.w.ax  = 0x300;
	regs.h.bl  = 0x10;
	regs.h.bh  = 0;
	regs.w.cx  = 0;
	sregs.es   = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( 0x31, &regs, &regs, &sregs );

	if ((RMI.EAX & 0xFFFF) != 0x4F)
	{
	    PVROSPrintf ("VESA INT 4F04h FAILED\n");
	    return (RMI.EAX & 0xFFFF);
	}

	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 300h FAILED\n");
	    return ( regs.w.ax );
	}


	/* DPMI call 101h deallocates DOS memory */
	regs.w.ax = 0x0101;
	regs.w.dx = gSelector;
	int386( 0x31, &regs, &regs);

	/* carry flag set if error, code in ax */
	if ( regs.w.cflag != 0 )
	{
	    PVROSPrintf ("DPMI INT 101h FAILED\n");
	    return ( regs.w.ax );
	}
	
	return 0;
}
