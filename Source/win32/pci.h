/*****************************************************************************
 *;++
 * Name           : $RCSfile: pci.h,v $
 * Title          : PCI.H
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	services available from PCI.ASM
 *                   
 * Program Type   : header file
 *
 * RCS info:
 *
 * $Date: 1997/05/02 07:27:09 $
 * $Revision: 1.3 $
 * $Locker:  $
 * $Log: pci.h,v $
 * Revision 1.3  1997/05/02  07:27:09  erf
 * Removed UINT32's etc and replaced with sgl_uint32 etc.
 *
 * Revision 1.2  1996/07/19  19:42:13  jop
 * Added pc98 support
 *
 * Revision 1.1  1996/06/10  11:51:12  jop
 * Initial revision
 *
 * Revision 1.1  1996/06/10  11:39:13  jop
 * Initial revision
 *
 * Revision 1.4  1996/01/05  16:51:57  jop
 * Routes PCI config 32 bit read/write through BIOS
 *
 * Revision 1.3  1995/11/02  13:40:17  jop
 * Calling convention now specified
 *
 * Revision 1.2  1995/10/20  15:56:14  jop
 * Added word and dword io func prototypes and removed redundant ones
 *
 * Revision 1.1  1995/10/19  18:37:50  jop
 * Initial revision
 *
 *;--
 *****************************************************************************/

int  __cdecl PCIFindBoard	(sgl_int8 *bBusIDNum, sgl_int8 *bDevFncID, sgl_int16 vendor, sgl_int16   device);
int  __cdecl PCIRead8		(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int8  *Data);
int  __cdecl PCIRead16		(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int16  *Data);
int  __cdecl PCIRead32		(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int32 *Data);
int  __cdecl PCIRead32Hack	(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int32 *Data);
int  __cdecl PCIWrite8		(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int8   Data);
int  __cdecl PCIWrite16		(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int16   Data);
int  __cdecl PCIWrite32		(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int32  Data);
int  __cdecl PCIWrite32Hack	(sgl_int8  bBusIDNum, sgl_int8  bDevFncID, sgl_int16 wReg,   sgl_int32  Data);

void __cdecl OutByte		(sgl_int16 port, sgl_int8 value);
void __cdecl OutWord		(sgl_int16 port, sgl_int16 value);
void __cdecl OutDword		(sgl_int16 port, sgl_int32 value);

sgl_int8	__cdecl InByte	(sgl_int16 port);
sgl_int16	__cdecl InWord	(sgl_int16 port);
sgl_int32	__cdecl InDword	(sgl_int16 port);

extern sgl_int16 wIsPC98;

/* end of pci.h */

