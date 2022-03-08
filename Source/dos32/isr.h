/******************************************************************************
 * Name         : $RCSfile: isr.h,v $
 * Title        : isr.h
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : publics in bi-modal ISRs for PVR DOS (DOS4GW)
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: isr.h,v $
 * Revision 1.2  1997/04/15  17:30:52  jop
 * Niced up exports a bit
 *
 * Revision 1.1  1997/04/09  02:53:34  jop
 * Initial revision
 *
 *
 *****************************************************************************/

typedef struct 
{
	sgl_uint32	RMStatusFlags;
	sgl_uint32	RMBusDevFnc;
	sgl_uint32	RMIRQ;
} RMDATA;

typedef struct 
{
	sgl_uint32	PMStatusFlags;
	sgl_uint32	PMRegs;
	sgl_uint32	PMIRQ;
} PMDATA;

extern void __interrupt ISR_pcx_protmode (void);
extern void __interrupt __far ISR_pcx_realmode (void);

extern RMDATA	RMData;
extern PMDATA	PMData;

/* end of $RCSfile: isr.h,v $ */

