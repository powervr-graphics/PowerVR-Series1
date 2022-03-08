/*****************************************************************************
 *;++
 * Name           : $RCSfile: w32dll.h,v $
 * Title          : W32DLL.H
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright      : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :   globals available from W32DLL.C
 *                   
 * Program Type   : header file
 *
 * RCS info:
 *
 * $Date: 1997/04/22 18:05:24 $
 * $Revision: 1.6 $
 * $Locker:  $
 * $Log: w32dll.h,v $
 * Revision 1.6  1997/04/22  18:05:24  mjg
 * Removed everything, as it wasn't needed anymore - execpt for windows
 * which been included where this file used to.
 *
 * Revision 1.5  1997/04/04  13:06:12  mjg
 * Removed several globals and enumerated types, changed the type of a
 * couple and got rid of all traces of 'OutputTo'.
 *
 * Revision 1.4  1997/04/01  18:51:01  mjg
 * Removed notprintf.
 *
 * Revision 1.3  1996/09/05  20:40:02  jop
 * substitute for init time printfs
 *
 * Revision 1.2  1996/07/31  21:56:52  jop
 * Added some protos
 *
 * Revision 1.1  1996/06/10  11:51:12  jop
 * Initial revision
 *
 * Revision 1.21  1996/05/28  14:44:53  ncj
 * Added new SGL modes
 *
 * Revision 1.20  1996/03/28  16:18:15  ncj
 * Added no interrupts feature
 *
 * Revision 1.19  1996/03/25  11:13:18  ncj
 * Added exports
 *
 * Revision 1.18  1996/02/05  17:37:10  ncj
 * Added Selector/Linear/Physical address mapping functions
 *
 * Revision 1.17  1996/02/02  17:19:00  jop
 * Added texas/sabre double buffering
 *
 * Revision 1.16  1996/01/19  14:23:47  jop
 * Added performance measurement support
 *
 * Revision 1.15  1996/01/15  12:46:05  jop
 * Did more work on Video over PCI
 *
 * Revision 1.14  1996/01/04  11:19:09  jop
 * Bridge revision auto-detect added
 *
 * Revision 1.13  1995/11/27  18:30:34  jop
 * Added exports to support video over PCI
 *
 * Revision 1.12  1995/11/23  14:38:53  jop
 * Extra parameter in copy buffer function
 *
 * Revision 1.11  1995/11/20  11:20:08  jop
 * Now exports the PCI output address as well
 *
 * Revision 1.10  1995/11/20  11:05:33  jop
 * Added export for OutputTo variable
 *
 * Revision 1.9  1995/11/07  18:29:34  jop
 * Added shared access to VxD ISR semaphores
 *
 * Revision 1.8  1995/11/03  16:30:47  jop
 * Added CopyBufferToHardware proto
 *
 * Revision 1.7  1995/10/27  11:34:57  jop
 * Simplified i/f to VxD and added VxD version ID output
 *
 * Revision 1.6  1995/10/23  12:39:29  jop
 * De-exported module handles and exported FlushCPUCache
 *
 * Revision 1.5  1995/10/23  10:08:47  jop
 * Exported DLL and VxD module handles
 *
 * Revision 1.4  1995/10/22  18:36:26  sjf
 * Jim added support for Params over PCI.
 *
 * Revision 1.3  1995/10/20  19:35:02  pdl
 * *** empty log message ***
 *
 * Revision 1.2  1995/10/20  18:41:55  pdl
 * changed char * to UINT32 *
 *
 * Revision 1.1  1995/10/19  19:00:13  jop
 * Initial revision
 *
 * Revision 1.1  1995/10/19  18:37:50  jop
 * Initial revision
 *
 *;--
 *****************************************************************************/

#ifndef _W32DLL_H_
#define _W32DLL_H_

#pragma message ("w32dll.h - nothing in here any more!")

#endif /* _W32DLL_H_ */
