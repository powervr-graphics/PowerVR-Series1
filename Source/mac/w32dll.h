/*****************************************************************************
 *;++
 * Name           : $RCSfile: w32dll.h,v $
 * Title          : W32DLL.H
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :   globals available from W32DLL.C
 *                   
 * Program Type   : header file
 *
 * RCS info:
 *
 * $Date: 1997/04/03 19:44:40 $
 * $Revision: 1.4 $
 * $Locker:  $
 * $Log: w32dll.h,v $
 * Revision 1.4  1997/04/03  19:44:40  mjg
 * Removed OutputTo
 *
 * Revision 1.3  1997/04/01  17:10:39  mjg
 * Removed notprintf
 *
 * Revision 1.2  1997/03/15  13:32:10  jwf
 * New Mac versions
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

#ifdef WIN32
#include "windows.h"
#endif

#if MAC
#include "windoz.h"
#endif

typedef enum
{
	PCI_PARAMS,
	SDRAM_PARAMS,
	DRAM_PARAMS
} PARAMLOCATION;

extern PARAMLOCATION TexasParamLocation;
extern PARAMLOCATION SabreParamLocation;

typedef enum
{
	FRAMEBUFFER_16_BIT,
	FRAMEBUFFER_32_BIT
} FRAMEBUFFER_DEPTH;

extern FRAMEBUFFER_DEPTH FramebufferDepth;

typedef enum
{
	SLEEP,
	CALCULATING_PARAMETERS,
	CALCULATING_REGION_DATA,
	UPLOADING_PARAMETERS,
	HARDWARE_RENDER,
	WAITING_FOR_VSYNC
} PROCESS_STATE;

#define COMBINEDLOCATION(x,y) (x | (y << 8))

extern HINSTANCE ghVxD;

extern UINT32 physOutputToPCI;
extern long physFrameBufferStride;
extern DWORD dwTSPParamSize;
extern DWORD dwISPParamSize;
extern UINT32 PCISabreBufferSize;
extern UINT32 PCITexasBufferSize;
extern UINT32 ISPDoubleBuffer;
extern UINT32 TSPDoubleBuffer;
extern UINT32 VersionID;
extern UINT8 bTSPRev;
extern UINT8 bTSPMem;
extern UINT8 bISPRev;
extern UINT16 wEnableIrqs;
extern UINT16 nSGLHardwareMode;
extern BOOL bDDrawBypass;

/* SGL mode literals */
#define MODE_OVERLAY24		1
#define MODE_STANDALONE		2
#define MODE_DDRAW			3
#define MODE_DESKTOP		4
#define MODE_RASTER			5

int SetProcessState (PROCESS_STATE ProcessState);
void FlushCPUCache (void);
void CopyBufferToHardware (void *Src, void *Dst, UINT32 Bytes, UINT32 CheckCopy);
int EnableFrameBufferAccess (int Enable);
int GetSelectorbase (UINT16 wSelector, UINT32 * pBase);
int MapLinearToPhysical(UINT32 dwLinear, UINT32 * pPhysical);
sgl_bool GetScreenBaseAndStride (UINT32 *pu32Base, UINT32 *pu32Stride);

#ifdef VSGL_ALLOCATEBLOCK

	int AllocatePhysicalAlias (SHAREDBLOCK *blk);
	int FreePhysicalAlias (SHAREDBLOCK *blk);
	int AllocateSharedBlock (DWORD ParamPages, SHAREDBLOCK *blk);
	int FreeSharedBlock (SHAREDBLOCK *blk);

#endif
