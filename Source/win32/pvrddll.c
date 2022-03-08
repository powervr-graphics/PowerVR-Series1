/*****************************************************************************
 *;++
 * Name           : $RCSfile: pvrddll.c,v $
 * Title          : PVRDDLL.C
 * C Author       : Edward Forde
 * Created        : 07/05/97
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	PVRD DLL init and midas initialisation
 *                   
 * Program Type   :	PVRD DLL
 *
 * RCS info:
 *
 * $Log: pvrddll.c,v $
 * Revision 1.19  1997/12/12  12:26:49  erf
 * Use define WHICH_CARD_REGISTRY_STRING (defined in pvrosapi.h)
 * to determine where to look for the manufacturer.
 *
 * Revision 1.18  1997/12/11  20:07:45  erf
 * When dll starts we need to find out where in the
 * registry to put LastD3DApp.
 *
 * Revision 1.17  1997/11/21  12:05:49  sxy
 * made DebugDeinit() available to TIMING build.
 *
 * Revision 1.16  1997/11/19  16:03:13  erf
 * Now writes LastD3DApp in LastApp registry key on process attach.
 *
 * Revision 1.15  1997/11/10  16:31:31  sxy
 * added function call DebugDeinit() when PROCESS_DETACH for developer's driver
 * to enable the PVRDebug window.
 *
 * Revision 1.14  1997/10/16  14:00:36  sxy
 * removed some unused code for DEBUGDEV.
 *
 * Revision 1.13  1997/10/16  11:10:40  gdc
 * chnaged developer build stuff to make it tidier
 *
 * Revision 1.12  1997/10/14  17:06:40  sxy
 * added a feature to write debug messages to a log file for DEBUGDEV.
 *
 * Revision 1.11  1997/09/24  09:41:11  sxy
 * Made PrintDLL() available to DEBUGDEV.
 *
 * Revision 1.10  1997/09/15  16:11:18  mjg
 * Put DAG_TRANS_SORTING compile switches around some code.
 *
 * Revision 1.9  1997/09/01  12:19:01  mjg
 * Added call to tidy up translucency sorting workspace memory.
 *
 * Revision 1.8  1997/08/28  13:50:15  gdc
 * added a new external for build compatability with sgllite
 *
 * Revision 1.7  1997/07/25  10:36:23  mjg
 * Renamed the PCX2_003 Dll name.
 *
 * Revision 1.6  1997/07/24  14:02:13  gdc
 * added a debug function to print dll name
 *
 * Revision 1.5  1997/06/09  09:34:47  gdc
 * got rid of a couple of warnings
 *
 * Revision 1.4  1997/06/02  02:42:13  jop
 * Converted to ue heap.c/h stuff
 *
 * Revision 1.3  1997/05/12  12:54:39  erf
 * Changed order of PVROSPrintf() function call.
 *
 * Revision 1.2  1997/05/09  15:19:35  erf
 * Print out messages on DLL attach and detach.
 *
 * Revision 1.1  1997/05/08  11:27:58  erf
 * Initial revision
 *
 *;--
 *****************************************************************************/

#include <windows.h>
#include <process.h>

#include "sgl.h"
#include "sgl_defs.h"
#include "pvrosapi.h"
#include "hwregs.h"
#include "heap.h"
#include "pvrlims.h" /* for DAG_TRANS_SORTING */ 

/*------------------- INSTANCE DATA SECTION ---------------------*/

HINSTANCE 	ghInstance = 0;

/*-------------------- SHARED DATA SECTION ----------------------*/

#pragma data_seg(".onetime")

HINSTANCE	hSystemInstance = NULL;

int gnInstances = 0;

sgl_uint32 gu32UsedFlags;

#pragma data_seg()

/*************** External Prototypes ********************************/
int   CALL_CONV SglInitialise(void);
void  CALL_CONV PVROSAPIExit ();
int   CALL_CONV PVROSAPIInit ();
#if DAG_TRANS_SORTING
	extern void CloseRegionDataL();
#endif /* DAG_TRANS_SORTING */
/********************************************************************/

#if DEBUG || DEBUGDEV
#if PCX1
char sDllName[] = "PVRMID4: ";
#elif PCX2
char sDllName[] = "PVRMID5: ";
#elif PCX2_003
char sDllName[] = "PVRMID5a: ";
#endif
#endif


/******************************************************************************
 * Function Name: WriteLastAppName
 *
 * Inputs       : -
 * Outputs      : Outputs string to registry.
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Fills out LastD3DApp in LastApp key of registry.
 *****************************************************************************/

static void WriteLastAppName()
{
	HKEY	hKey;
	char	szCallingProcess[256];
	char 	pszKey[512], szText[32];
	MANUFACTURER_TYPE	eManufacturerType = VIDEOLOGIC_3D;
	DWORD	dwType, dwSize = sizeof(szText);

	/* Detect the hardware present ie Maxtrox m3D or PowerVR.
	 */
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, WHICH_CARD_REGISTRY_STRING, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx (hKey, "BoardType", 0, &dwType, (LPBYTE)szText, (LPDWORD)&dwSize) == ERROR_SUCCESS)
		{
			if (dwType == REG_SZ)
			{
				eManufacturerType = (MANUFACTURER_TYPE) strtoul (szText, NULL, 0);

				/* Which registry to use.
				 */
				switch (eManufacturerType)
				{
					case VIDEOLOGIC_3D:	strcpy(pszKey, VL_REGISTRY_STRING);
										break;

					case MATROX_M3D:	strcpy(pszKey, MATROX_REGISTRY_STRING);
										break;

					default:			strcpy(pszKey, VL_REGISTRY_STRING);
										break;
				}
				strcat(pszKey,"LastApp");

				/* 
					Sort out optional extra's.
				*/
				GetModuleFileName (NULL, szCallingProcess, sizeof (szCallingProcess));

				if (!strstr(szCallingProcess, "DDHELP.EXE"))
				{
					DWORD dwDisposition;

					/*
						Set the D3DLastApp registry string to help the "user" to determine
						the executable name to use in app hints.
					*/
					if (RegCreateKeyEx (HKEY_LOCAL_MACHINE, 
										pszKey,
										0,
										"", 
										REG_OPTION_NON_VOLATILE,
										KEY_READ | KEY_WRITE,
										NULL,
										&hKey, 
										&dwDisposition) == ERROR_SUCCESS)
					{
						char *pszExe;

						pszExe = strrchr (szCallingProcess, '\\');

						if (pszExe)
						{
							pszExe++;
						}
						else
						{
							pszExe = szCallingProcess;
						}

						RegSetValueEx(hKey,"LastD3DApp",0,REG_SZ,pszExe,strlen(pszExe)+1);
						RegCloseKey(hKey);
					}
					 
				}
			}
		}

		RegCloseKey(hKey);
	}
}


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
			/* Try and write the name of the app before we do anything
			** as this will leave the name of a failed app regardless
			*/
			WriteLastAppName();

			if (gnInstances == 0)
			{
				fRet = MemInit ();
			}

			if (fRet && PVROSAPIInit ())
			{
				PVROSPrintf("PVRDMID DLL_PROCESS_ATTACH\n");

				if(SglInitialise())
				{
					PVROSPrintf("Failed to Initialise SGL\n");
					fRet = FALSE;
					
					MemFini ();
				}
				else
				{
					ghInstance = hinstDLL;

					++gnInstances;
					
					fRet = TRUE;

				}
			}
			break;
		}

		case DLL_PROCESS_DETACH:
		{
			PVROSPrintf("PVRDMID DLL_PROCESS_DETACH\n");
			
			#if DAG_TRANS_SORTING			
				CloseRegionDataL();
			#endif /* DAG_TRANS_SORTING */
				
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
			PVROSPrintf("Calling application is attaching a thread\n");
			fRet = TRUE;
			break;
		}

		case DLL_THREAD_DETACH:
		{
			PVROSPrintf("Calling application is killing a thread\n");
			fRet = TRUE;
			break;
		}
	}
	
	return (fRet);
}

/* end of $RCSfile: pvrddll.c,v $ */
