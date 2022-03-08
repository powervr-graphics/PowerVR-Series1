/******************************************************************************
 * Name : hwregs.h
 * Title : 
 * Author : Peter Leaback
 * Created : 17/4/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : hardware registers for the real HW
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwregs.h,v $
 * Revision 1.18  1997/09/10  17:09:48  erf
 * Added bDither field to HWStartRender() prototype definition.
 *
 * Revision 1.17  1997/07/24  17:07:56  gdc
 * moved HWStartRender Prototype here
 *
 * Revision 1.16  1997/05/14  00:23:57  mjg
 * Made file able to be used for PCX1 and PCX2.
 *
 * Revision 1.15  1997/04/22  18:08:58  mjg
 * Tidy up
 *
 * Revision 1.14  1997/04/22  09:11:37  mjg
 * Removed GetSystemHandle prototype, in line with new PVROS dll.
 *
 * Revision 1.13  1997/04/04  13:18:12  mjg
 * Included the globals removed from w32dll.h (mostly unused).
 *
 * Revision 1.12  1997/04/03  18:01:19  mjg
 * Added the prototype for GetSystemHandle.
 *
 * Revision 1.11  1997/04/03  13:08:21  mjg
 * Updated the calling convention.
 *
 * Revision 1.10  1997/01/14  12:58:21  erf
 * Define PCX2 hardware register PCX_POWERDOWN.
 *
 * Revision 1.9  1997/01/10  16:21:55  erf
 * Added new PCX2 registers.
 *
 * Revision 1.8  1996/12/20  16:25:09  erf
 * Added PCX_FASTFOG register define.
 *
 * Revision 1.7  1996/10/25  09:58:34  mal
 * Added some register definitions for PCX2.
 *
 * Revision 1.6  1996/07/12  14:14:29  ncj
 * Added 24 bit direct draw support
 *
 * Revision 1.5  1996/06/21  11:02:18  mal
 * Added DetermineTextureMemorySize() prototype
 *
 * Revision 1.4  1996/06/19  09:01:35  mal
 * Added function prototype for get_texture_memory_size();
 *
 * Revision 1.3  1996/06/10  14:14:50  jop
 * got SOF and stride protos right
 *
 * Revision 1.2  1996/04/30  18:08:25  jop
 * Abstract for old bridge calls
 *
 * Revision 1.1  1996/04/17  17:45:17  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __HWREGS_H__
#define __HWREGS_H__

/*
** The following defines are ***DWORD*** resolution offsets from the start of 
** the PCX register base address.
*/

#define PCX_ID				0x000
#define PCX_REVISION   		0x001
#define PCX_SOFTRESET      	0x002
#define PCX_INTSTATUS      	0x003
#define PCX_INTMASK      	0x004
#define PCX_STARTRENDER     0x005
#define PCX_FOGAMOUNT	    0x006
#define PCX_OBJECT_OFFSET	0x007
#define PCX_PAGE_CTRL		0x008

#define PCX_ISP_BASE		0x00A
#define PCX_PREC_BASE		0x00B
#define PCX_TMEM_SETUP		0x00C
#define PCX_TMEM_REFRESH	0x00D
#define PCX_FOGCOL			0x00E
#define PCX_CAMERA			0x00F
#define PCX_PACKMODE   		0x010
#define PCX_ARBMODE   		0x011
#define PCX_LSTRIDE   		0x012
#define PCX_SOFADDR   		0x013
#define PCX_XCLIP   		0x014
#define PCX_ABORTADDR   	0x015
#define PCX_GPPORT		   	0x016

/* The following for PCX2 only    */
#define PCX_IEEEFP  		0x018
#define PCX_BILINEAR		0x019
#define PCX_PCI21COMP   	0x01B
#define PCX_CLKSELECT		0x01C
#define PCX_FASTFOG			0x01D
#define PCX_POWERDOWN		0x01E
/* End of PCX2 Specific Registers */

#define PCX_MEMTEST_DATA	0X07D
#define PCX_MEMTEST_MODE	0X07E
#define PCX_MEMTEST_RES		0X07F

#define PCX_FOG_TABLE		0X080
#define PCX_TLB				0X100
#define PCX_DIVIDER_TABLE	0X200

extern sgl_uint32 TSPDoubleBuffer; /* Used in dtri.c */

void CALL_CONV HWStartRender(sgl_bool bFlipRequested, HDISPLAY hDisplay, sgl_bool bDither);
void CALL_CONV ProgramSOFAddressReg (sgl_uint32 uSOFAddress);
void CALL_CONV ProgramStrideReg (sgl_uint32 uStride);
sgl_uint32 CALL_CONV get_texture_memory_size( void );
sgl_uint32 CALL_CONV DetermineTextureMemorySize(void);
int CALL_CONV SetPCIPixelFormat(sgl_uint16 wBitsPerPixel);

#endif /*__HWREGS_H__*/
/*
// End of file
*/

