/******************************************************************************
 * Name         : $RCSfile: irq.h,v $
 * Title        : irq.h
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : IRQ utils for bi-modal PVR DOS4GW driver ISRs
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: irq.h,v $
 * Revision 1.3  1997/04/15  17:30:08  jop
 * Nicer exported flags word
 *
 * Revision 1.2  1997/04/09  02:53:15  jop
 * Header for bi-modal services
 *
 *
 *****************************************************************************/

/* pointer to RM ISR flags word */

extern sgl_uint32 *pStatusFlags;

sgl_bool SetupISR (void);
PVROSERR EnableISR (DEVICE_TYPE DeviceType);
void RetriggerISR (void);
void KillISR (void);

/* end of $RCSfile: irq.h,v $ */
