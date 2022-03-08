/*****************************************************************************
 *;++
 * Name           : $RCSfile: w32dll.c,v $
 * Title          : W32DLL.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :	WIN32 DLL init and midas initialisation
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1997/11/24 15:12:17 $
 * $Revision: 1.74 $
 * $Locker:  $
 * $Log: w32dll.c,v $
 * Revision 1.74  1997/11/24  15:12:17  gdc
 * removed reference to SGLTT_TEXAPI
 *
 * Revision 1.73  1997/11/21  12:07:28  sxy
 * made DebugDeinit() available to TIMING build.
 *
 * Revision 1.72  1997/11/11  12:14:49  gdc
 * removed two flags from flags warning - DEPTHBIAS ans SELECTTRANSSORT
 *
 * Revision 1.71  1997/11/10  16:29:13  sxy
 * added function call DebugDeinit() when PROCESS_DETACH for developer's driver
 * to enable the PVRDebug window.
 *
 * Revision 1.70  1997/10/16  11:10:18  gdc
 * chnaged developer build stuff to make it tidier
 *
 * Revision 1.69  1997/10/14  17:04:18  sxy
 * Added a feature to write debug message to a log file for DEBUGDEV.
 *
 * Revision 1.68  1997/09/24  13:54:05  gdc
 * typo
 *
 * Revision 1.67  1997/09/24  13:29:13  gdc
 * chnaged resevered flags from enums to consts cause sgl.h had changed
 *
 * Revision 1.66  1997/09/24  09:46:31  sxy
 * Changed a DPF to DPFDEV and made PrintDLL() available to DEBUGDEV.
 *
 * Revision 1.65  1997/09/10  17:13:54  erf
 * SGL flag 0x00800000 is now used to disable dithering.
 *
 * Revision 1.64  1997/08/28  13:48:52  gdc
 * added facility to print context flags used during an application
 * and indicate if any reserved flags have been used
 *
 * Revision 1.63  1997/07/25  10:35:07  mjg
 * Renamed the PCX2_003 Dll name and cleaned up InitEnvironment.
 *
 * Revision 1.62  1997/07/24  14:02:53  gdc
 * added a debug function to print dll name
 *
 * Revision 1.61  1997/07/18  15:36:39  gdc
 * chnaged printfs to dpf's and added a module id
 *
 * Revision 1.60  1997/06/09  09:23:35  gdc
 * put in a short list of prototypes to stop compiler warnings
 *
 * Revision 1.59  1997/06/02  02:43:20  jop
 * Converted to use global heap manager stuff on heap.c/h
 *
 * Revision 1.58  1997/05/15  20:15:56  jop
 * Rmoved some useless stuff
 *
 * Revision 1.57  1997/05/07  23:46:06  jop
 * Fixed some vague messages
 *
 * Revision 1.56  1997/05/01  08:25:30  jop
 * Instantiated sgl interface and added a few functions that are in
 * sgl.h but are not implemented in code!
 *
 * Revision 1.55  1997/05/01  02:44:51  jop
 * pvrosapi stuff
 *
 * Revision 1.54  1997/04/29  15:28:39  erf
 * 1. Added new PVROS API function calling mechanism.
 * 2. In dllmain() code added to load the PVROS API functions.
 *
 * Revision 1.53  1997/04/28  00:26:01  mjg
 * Removed pvrosif thing-ummy-jings.
 *
 * Revision 1.52  1997/04/27  22:57:35  jop
 * made sglmid? use pvrosif.c instead of import lib (pointless? could be ...)
 *
 * Revision 1.51  1997/04/24  20:42:33  gdc
 * added a return val check on previous
 *
 * Revision 1.50  1997/04/24  20:37:50  gdc
 * explicitly initilaise sgl form DllMain
 *
 * Revision 1.49  1997/04/23  14:56:34  gdc
 * added sensible return value from InitEnvironment
 *
 * Revision 1.48  1997/04/22  18:04:59  mjg
 * Removed w32dll.h include and unused globals.
 *
 * Revision 1.47  1997/04/22  00:55:03  jop
 * Made w32dll.c temporarily a bit more context aware
 *
 * Revision 1.46  1997/04/21  22:26:49  gdc
 * changes to separate off pvros.dll
 *
 * Revision 1.45  1997/04/04  21:41:59  mjg
 * Moved the 'pvrosapi.h' include.
 *
 * Revision 1.44  1997/04/04  13:05:16  mjg
 * Removed a lot of globals to hwregs.h and the .ini file reading section to
 * brdsetup.c
 *
 * Revision 1.43  1997/04/03  19:11:03  mjg
 * Yet another CALL_CONV.
 *
 * Revision 1.42  1997/04/01  18:46:11  mjg
 * Changed notprintf to PVROSPrintf.
 *
 * Revision 1.41  1997/03/26  15:15:02  jop
 * Fixed bug where the board was released when the wrong context
 * was released.
 *
 * Revision 1.40  1997/03/25  15:50:48  erf
 * Removed #include "sglddraw.h".
 *
 * Revision 1.39  1997/03/25  10:19:28  jop
 * Removed loads of stuff and implemented PVROS stuff
 *
 * Revision 1.38  1997/03/11  19:22:20  erf
 * 1. SetCacheMode() returns TRUE, always, if not PCX2.
 * 2. InitPentiumMode() only compiled in for PCX2.
 *
 * Revision 1.37  1997/03/10  13:40:44  jop
 * Went to atom startegy for D3D/SGL
 *
 * Revision 1.36  1997/03/07  17:27:09  erf
 * Added PENTIUM_PRO_MODEL_KLAMATH = 3 to enable write combining for the
 * Pentium Klamath processor.
 *
 * Revision 1.35  1997/03/04  18:15:49  sxy
 * Moved D3D flags into private section.
 *
 * Revision 1.34  1997/03/04  15:47:55  jop
 * Fixed buggy d3d detection
 *
 * Revision 1.33  1997/02/28  12:34:02  erf
 * Changed HookIRQ(-1) to HookIRQ(FALSE) as requested by Jim.
 *
 * Revision 1.32  1997/02/06  11:11:27  ncj
 * Restrict write combining enhancements to pcx2
 *
 * Revision 1.31  1997/02/05  16:06:29  ncj
 * Fixed last minute 'tidy up' typo
 *
 * Revision 1.30  1997/02/05  14:12:28  ncj
 * Enabled Write Combining feature of Pentium Pro CPU for the TSP DRAM
 *
 * Revision 1.29  1997/01/17  19:11:08  jrc
 * Disabled SetCacheMode because of a bug in the VxD.
 *
 * Revision 1.28  1997/01/17  12:57:54  sxy
 * added a flag gbD3DProcessID for D3D mode
 *
 * Revision 1.27  1997/01/13  17:18:43  sxy
 * The same as Revision 1.26
 *
 * Revision 1.26  1997/01/13  14:34:31  sxy
 * Added conditional display error message in MessageDialog()
 *
 * Revision 1.25  1997/01/13  14:16:03  gdc
 * re-enabled Page caching call to VxD
 *
 * Revision 1.24  1996/12/19  18:04:55  mjg
 * Reverted to pre-tidied version, with one exeception, to get Midas3 strips
 * and ViRGE to work.
 *
 * Revision 1.22  1996/12/12  11:14:17  erf
 * Changed compile switches:
 * 1. Remove defined from WIN32 etc.
 * 2. MIDASPC1 to ISPTSP.
 *
 * Revision 1.21  1996/12/02  18:39:45  jop
 * Disabled cache code
 *
 * Revision 1.20  1996/11/07  16:00:31  jop
 * Changed 'Default' ini settings to go through HWRdValFile
 *
 * Revision 1.19  1996/10/02  16:44:47  ncj
 * Compaq / Midas 3 / D3D / Virge combination 555/565 fix
 *
 * Revision 1.18  1996/09/18  17:10:45  jrc
 * Set gbD3D to FALSE until proved otherwise (instead of always TRUE).
 *
 * Revision 1.17  1996/09/10  06:03:33  jop
 * changed a variable to a new segment
 *
 * Revision 1.16  1996/09/09  17:27:50  ncj
 * Fixed thunk unload problem with midas 3
 *
 * Revision 1.15  1996/09/05  22:36:50  jop
 * Fixed (hopefully) midas3 mode change in D3D land
 *
 * Revision 1.14  1996/09/05  20:55:54  jop
 * Fixed midas3 re-init bug
 *
 * Revision 1.13  1996/09/05  20:39:53  jop
 * printfs removed
 *
 * Revision 1.12  1996/09/03  18:23:42  jrc
 * 1. Removed a duplicate condition in an 'if' statement.
 * 2. A global variable (gbD3D) has been set up for use by sgltri_render that
 *    indicates whether we are being run from Direct3D.
 *
 * Revision 1.11  1996/08/22  11:12:24  sjf
 * Once again MIDAS1/Standalone was messed up. With the wonderful
 * assistance of Nick Jones, we've got the code to skip over D3D
 * initialisation for the earlier systems.
 *
 * Revision 1.10  1996/08/13  15:21:06  jop
 * Compaq fix - checks if we are being loaded from d3d hal
 *
 * Revision 1.9  1996/08/02  18:04:17  jop
 * Got rid of DDraw init from ProcessAttach
 *
 * Revision 1.8  1996/07/31  21:56:28  jop
 * Did all one-time init stuff
 *
 * Revision 1.7  1996/07/19  19:42:32  jop
 * Added pc98 support
 *
 * Revision 1.6  1996/07/11  16:43:29  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.5  1996/07/11  12:23:52  jop
 * Added stuff to avoid ddraw calls in d3d driver
 *
 * Revision 1.4  1996/07/08  15:49:53  mal
 * Modified ISPInDram default to 1
 *
 * Revision 1.3  1996/06/12  14:09:36  ncj
 * Changed sglmid1.dll to sglmid3.dll for thunk connect
 *
 * Revision 1.2  1996/06/10  16:28:16  jop
 * Fixed init and thunk issues
 *
 * Revision 1.1  1996/06/10  11:51:12  jop
 * Initial revision
 *
 * Revision 1.51  1996/06/03  15:29:55  jop
 * Init now seperate from DllMain
 *
 * Revision 1.50  1996/05/28  14:46:04  ncj
 * Added new SGL modes
 *
 * Revision 1.49  1996/05/13  12:53:10  ncj
 * Corrected ini file debug setting
 *
 * Revision 1.48  1996/05/10  12:17:48  ncj
 * Compaq virge fix
 *
 * Revision 1.47  1996/05/07  18:27:16  ncj
 * RCS problems
 *
 * Revision 1.45  1996/04/12  16:00:10  ncj
 * Added debug break for winice in debug build
 *
 * Revision 1.44  1996/04/12  10:46:58  ncj
 * Clean up before b5 release
 *
 * Revision 1.43  1996/04/10  14:26:12  ncj
 * Used DCI if direct draw doesn't work, and gives better error dialogs
 *
 * Revision 1.42  1996/04/04  15:41:59  ncj
 * Fixed 'direct draw not available' error for standalone mode
 *
 * Revision 1.41  1996/04/04  13:08:53  ncj
 * Added support for INI settings to disable interrupts, disable dependancy
 * on DCI, and disable direct draw for testing, along with support for
 * strict locking . Also fixed DCI selector bug.
 *
 * Revision 1.40  1996/03/28  16:20:33  ncj
 * Added facility to enable sgl to work without using interrupts.
 *
 * Revision 1.39  1996/03/25  13:37:54  ncj
 * Had to undo error messages because of runtime library error
 *
 * Revision 1.38  1996/03/25  11:12:43  ncj
 * Improved error reporting with MessageBox
 *
 * Revision 1.37  1996/03/20  16:53:33  ncj
 * Adjust PCI bridge rev 2 latency to 0xF8
 *
 * Revision 1.36  1996/03/04  17:45:15  ncj
 * Retain some version info as global instead of local
 *
 * Revision 1.35  1996/02/27  12:25:45  ncj
 * Moved sgl_get_win_versions to sglwin.c because only MS compiler does DDraw
 *
 * Revision 1.34  1996/02/26  16:46:19  jrc
 * Added sgl_get_win_versions.
 *
 * Revision 1.33  1996/02/19  13:48:54  ncj
 * Added window moved stub
 *
 * Revision 1.32  1996/02/12  11:01:53  ncj
 * Allows compilation without DDRAW being defined
 *
 * Revision 1.31  1996/02/07  15:13:54  ncj
 * Oops, I forgot to initialise the screen address before setting the hw regs
 *
 * Revision 1.30  1996/02/06  12:25:27  ncj
 * Calculates the screen address automatically, though ScreenBase INI overrides it
 *
 * Revision 1.29  1996/02/02  17:18:24  jop
 * Added texas/sabre double buffering
 *
 * Revision 1.28  1996/01/22  10:36:15  jop
 * Added code for running ISP out of DRAM
 *
 * Revision 1.27  1996/01/19  14:23:47  jop
 * Added performance measurement support
 *
 * Revision 1.26  1996/01/18  11:11:10  jop
 * Fixed for use with new VxD version (raster and direct mode now supported)
 *
 * Revision 1.25  1996/01/15  12:44:17  jop
 * Did more work on video over PCI
 *
 * Revision 1.24  1996/01/05  16:51:57  jop
 * Routes PCI config 32 bit read/write through BIOS
 *
 * Revision 1.23  1996/01/04  11:19:09  jop
 * Bridge revision auto-detect added
 *
 * Revision 1.22  1995/12/04  17:21:02  jop
 * Fixed ISP/TSP cut and paste bug
 *
 * Revision 1.21  1995/11/27  18:29:15  jop
 * Added DCI calls to get primary surface for sending video over
 * PCI
 *
 * Revision 1.20  1995/11/24  15:43:00  jop
 * Fixed bug in parameter copy
 *
 * Revision 1.19  1995/11/23  15:49:59  jop
 * Now sets the PCI screen address correctly
 *
 * Revision 1.18  1995/11/23  14:38:18  jop
 * Added extra parameter to copy buffer fucntion
 *
 * Revision 1.17  1995/11/20  12:04:27  jop
 * Screen alias now properly freed
 *
 * Revision 1.16  1995/11/20  11:05:12  jop
 * Added support for output over PCI
 *
 * Revision 1.15  1995/11/15  14:28:43  jop
 * fixed pc98 bug
 *
 * Revision 1.14  1995/11/08  20:16:27  jop
 * y
 * Outputs pointer to shared EndOfRender pointer
 *
 * Revision 1.13  1995/11/07  18:27:59  jop
 * Enabled interrupts through the VxD
 *
 * Revision 1.12  1995/11/03  17:14:04  jop
 * Added CopyBufferToHardware
 *
 * Revision 1.11  1995/11/02  10:58:05  jop
 * Configured to stop stack faults
 *
 * Revision 1.10  1995/10/27  11:34:57  jop
 * Simplified i/f to VxD and added VxD version ID output
 *
 * Revision 1.9  1995/10/23  12:39:29  jop
 * De-exported module handles and exported FlushCPUCache
 *
 * Revision 1.8  1995/10/22  18:36:06  sjf
 * Jim added support for parameters over PCI.
 *
 * Revision 1.7  1995/10/21  21:49:59  sjf
 * More debug added (by Jim I think).
 *
 * Revision 1.6  1995/10/20  19:36:06  pdl
 * *** empty log message ***
 *
 * Revision 1.5  1995/10/20  18:41:27  pdl
 * changed char * to UINT32 *
 *
 * Revision 1.4  1995/10/20  18:22:36  sjf
 * Implemented VxD physical address thingo (said Ji, cause he's the
 * one who really did it).
 *
 * Revision 1.3  1995/10/20  10:13:41  jop
 * Removed last traces of 16 bit driver
 *
 * Revision 1.2  1995/10/20  10:05:22  jop
 * Converted to VxD from installable driver
 *
 * Revision 1.1  1995/10/19  19:00:13  jop
 * Initial revision

 *;--
 *****************************************************************************/

#define MODULE_ID MODID_W32DLL

#include <windows.h>
#include <process.h>

#include "sgl.h"
#include "sgl_defs.h"
#include "pvrosapi.h"
#include "hwregs.h"
#include "heap.h"

#define API_FNBLOCK
#include "sgl.h"
#undef API_FNBLOCK

#define API_INSTANTIATE
#include "sgl.h"
#undef API_INSTANTIATE

/*------------------- INSTANCE DATA SECTION ---------------------*/

HINSTANCE 	ghInstance = 0;

/*-------------------- SHARED DATA SECTION ----------------------*/

#pragma data_seg(".onetime")

HINSTANCE	hSystemInstance = NULL;

int gnInstances = 0;

BOOL bStrictLocks=FALSE;

#pragma data_seg()

/*************** External Prototypes ********************************/
int   CALL_CONV SglInitialise(void);
void  CALL_CONV PVROSAPIExit ();
int   CALL_CONV PVROSAPIInit ();
/********************************************************************/


int CALL_CONV LoadSglBin (char *pszFileName,	
					 int *noMeshes,		int **meshIDs, 	
					 int *noMaterials,	int **materialIDs,
					 int *noBMPTextures, int **BMPTextureIDs)
{
	return (-1);
}	

void CALL_CONV FreeSglBinTextures (int noBMPTextures, int *BMPTextureIDs)
{
}	

void CALL_CONV TexasWriteBMP (char * filename)
{
}	

#if DEBUG || DEBUGDEV
#if PCX1
char sDllName[] = "SGLMID4: ";
#elif PCX2
char sDllName[] = "SGLMID5: ";
#elif PCX2_003
char sDllName[] = "SGLMID5a: ";
#endif
#endif


/******************************************************************************
 * Function Name: InitEnvironment
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : TRUE!
 * Globals Used : ghInstance
 *
 * Description  : init board - now done mostly in VxD, now just sets a global
 *****************************************************************************/
sgl_bool CALL_CONV InitEnvironment ()
{
	if (!hSystemInstance)
	{
		hSystemInstance = ghInstance;		
	}
	
	return(TRUE);
}


sgl_uint32 gu32UsedFlags;
#define RESERVED_FLAGS (      \
	0x00000008 | 0x00000010 | \
	SGLTT_WRAPU |             \
	SGLTT_WRAPV |             \
	0x00000100  |             \
	SGLTT_TRANSBACKGROUND |   \
	0x00100000 |              \
	0x04000000 |              \
	0x08000000 | 0x10000000 | \
	0x20000000 |              \
	SGLTT_DIRECT_EXTENSIONS | \
	SGLTT_HAL_RESERVED )


/******************************************************************************
 * Function Name: DllMain
 *
 * Inputs       : -
 * Outputs      : see SDK
 * Returns      : see SDK
 * Globals Used : -
 *
 * Description  : 32 bit DLL entry point - see SDK
 *****************************************************************************/

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	BOOL fRet = FALSE;
   
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			if (gnInstances == 0)
			{
				fRet = MemInit ();
			}

			if (fRet && PVROSAPIInit ())
			{
				if(SglInitialise())
				{
					DPF((DBG_ERROR,"Failed to Initialise SGL"));
					fRet = FALSE;

					MemFini ();
				}
				else
				{
					DPF((DBG_MESSAGE,"SGLMID DLL_PROCESS_ATTACH"));
					
					ghInstance = hinstDLL;

					++gnInstances;
					
					fRet = TRUE;

				}
			}
			break;
		}

		case DLL_PROCESS_DETACH:
		{
			PVROSPrintf("SGLMID : MESSAGE : Context Flags set by App : %#08lx\n",gu32UsedFlags); 
			if(gu32UsedFlags & RESERVED_FLAGS)
			{
				PVROSPrintf("SGLMID : WARNING : Reserved Context Flags set by App : %#08lx\n",
							gu32UsedFlags & RESERVED_FLAGS); 
			}

			DPF((DBG_MESSAGE,"SGLMID DLL_PROCESS_DETACH"));
						
			--gnInstances;
			
			if (hSystemInstance == ghInstance)
			{
				hSystemInstance = NULL;
			}

			PVROSAPIExit ();
			
			if (gnInstances == 0)
			{
				MemFini ();
			}

			fRet = TRUE;

#if DEBUGDEV || TIMING
			DebugDeinit ();
#endif
			break;
		}

		case DLL_THREAD_ATTACH:
		{
			DPF((DBG_MESSAGE,"SGLMID DLL_THREAD_ATTACH"));
			fRet = TRUE;
			break;
		}

		case DLL_THREAD_DETACH:
		{
			DPF((DBG_MESSAGE,"SGLMID DLL_THREAD_DETACH"));
			fRet = TRUE;
			break;
		}
	}
	
	return (fRet);
}

/* end of $RCSfile: w32dll.c,v $ */
