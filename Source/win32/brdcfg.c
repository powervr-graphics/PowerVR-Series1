/******************************************************************************
 * Name : brdcfg.c
 * Title : Midas Board Config
 * Author : Simon Fenney
 * Created : 27/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : Reads configuration file from windows to decide what
 *				 kind of board is being used...
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: brdcfg.c,v $
 * Revision 1.16  1997/12/11  20:06:54  erf
 * Added call to GetManStringFromRegistry() routine which
 * returns the registry area to check for app hints.
 *
 * Revision 1.15  1997/11/10  15:25:54  dm
 * chnaged another default/defaults
 *
 * Revision 1.14  1997/10/16  10:16:56  gdc
 * got rid of warnings
 *
 * Revision 1.13  1997/09/25  09:42:08  gdc
 * default and defaults
 *
 * Revision 1.12  1997/09/24  14:42:58  gdc
 * added string read version of HWRdValFile
 *
 * Revision 1.11  1997/09/24  11:05:24  gdc
 * added new 'recommended order' functions for reading apphints stuff
 *
 * Revision 1.10  1997/06/17  22:00:43  dm
 * gone from default to defaults
 *
 * Revision 1.9  1997/06/04  13:52:47  gdc
 * stopped compiler warnings
 *
 * Revision 1.8  1997/06/02  02:40:51  jop
 * Converted to use windows 'getenv' functions
 *
 * Revision 1.7  1997/05/02  07:24:30  erf
 * Removed UINT32's etc and replaced with sgl_uint32 etc.
 *
 * Revision 1.6  1997/04/04  13:03:44  mjg
 * Added a CALL_CONV.
 *
 * Revision 1.5  1997/04/01  18:45:18  mjg
 * Changed notprintf to PVROSPrintf.
 *
 * Revision 1.4  1996/12/12  01:40:08  jop
 * Got rid of notprintf message when app hint read fails
 *
 * Revision 1.3  1996/12/11  01:17:52  jop
 * Removed ../'s from include files
 *
 * Revision 1.2  1996/11/07  15:59:00  jop
 * Added registry app hints capability to HWReadValFile
 *
 * Revision 1.1  1996/11/07  14:30:22  jop
 * Initial revision
 *
 * Revision 1.4  1996/07/11  16:42:12  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.3  1996/02/02  17:16:54  jop
 * Extra debug for not reading ini file correctly
 *
 * Revision 1.2  1995/10/27  10:01:27  sjf
 * Now reads either Hex or Decimal Format.
 *
 * Revision 1.1  1995/10/25  17:51:03  sjf
 * Initial revision
 *
 ******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sgl.h"
#include "sgl_defs.h"
#include "profile.h"
#include "brdcfg.h"
#include "string.h"
#include "pvrosapi.h"

#include "windows.h"

/*
// Define the name of the ini file which contains all this stuff. It will
// be located in the windows directory
*/
#define CONFIG_FILE "sglhw.ini"
#define WINDOWS_ENV "WINDIR"

/*
// Framestore Mode section
*/
#define MODE  "mode"



static sgl_bool ModeDetermined = FALSE;

static char   HWMode[100];
static char   ConfigFile[100];
/*
// Overlay 
*/
#define OVERLAY "overlay"


/* Prototypes for functions from profile.c
 */
sgl_bool GetHwIniStringFromRegistry (char *pszSection, char *pszEntry, char *pszBuffer, int pszBufferSize);
sgl_bool GetManStringFromRegistry (char *pszBuffer);

/**************************************************************************
* Function Name  : InitDetermineMode
* Inputs         : NONE
* Outputs        : NONE
* Input/Output	  : NONE
*
*
* Global Used    : Reads config file out of windows
* Returns        : 
* Description    : Determines the mode we should set the hardware up for
*				   from the configuration file.
*
*
**************************************************************************/

static void InitDetermineMode(void)
{
	if(!ModeDetermined)
	{
		/*
		// Init the name of the config file
		*/
		GetEnvironmentVariable (WINDOWS_ENV, ConfigFile, sizeof (ConfigFile)-1);

		strcat(ConfigFile, "\\");
		strcat(ConfigFile, CONFIG_FILE);

		DPF((DBG_MESSAGE, "Looking in %s for framestore config", ConfigFile));

		/*
		// get the mode from the file
		*/
		if(SglReadPrivateProfileString(MODE, MODE, "None", 
									HWMode, 100,
									ConfigFile))
		{
			/* ok done...*/
			DPF((DBG_MESSAGE, "Selected mode is %s for HW config", HWMode));
		}
		else
		{
			DPF((DBG_WARNING, "Failed to read the MODE section"));
		}


		ModeDetermined=TRUE;
	}

}

sgl_bool GetApplicationHintString (char *pszHint, char *pszBuffer, int pszBufferSize)
{
	sgl_bool fRet = FALSE;
	char szCaller[256];
	char szKey[512];

	/* Get the registry area depending on the hardware present ie Maxtrox m3D or PowerVR.
	 */
	if (GetManStringFromRegistry (szKey))
	{
		strcat (szKey, "App Hints\\");

		if (GetModuleFileName (NULL, szCaller, sizeof (szCaller)))
		{
			HKEY hKey;
			char *pExe = strrchr (szCaller, '\\');

			if (pExe)
			{
				pExe++;
			}
			else
			{
				pExe = szCaller;
			}

			strcat (szKey, pExe);

			if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				DWORD dwType;

				if (RegQueryValueEx (hKey, pszHint, 0, &dwType, pszBuffer, &pszBufferSize) == ERROR_SUCCESS)
				{
					if (dwType == REG_SZ)
					{
						fRet = TRUE;
					}
					else
					{
						PVROSPrintf("Weird data type %s %s %lx\n", szKey, pszHint, dwType);
					}
				}
				else
				{
					PVROSPrintf("Couldn't access key %s %s\n", szKey, pszHint);
				}

				RegCloseKey (hKey);
			}
		}
	}

	return (fRet);
}


sgl_bool GetApplicationHintInt (char *pszHint, sgl_int32 *pData)
{
	char szBuffer[128];
	
	if (GetApplicationHintString (pszHint, szBuffer, sizeof (szBuffer)))
	{
		*pData = atoi (szBuffer);
		PVROSPrintf("%s: %d\n", pszHint, *pData);
		return (TRUE);
	}
	return (FALSE);
}

sgl_bool GetApplicationHintFloat (char *pszHint, float *pData)
{
	char szBuffer[128];
	
	if (GetApplicationHintString (pszHint, szBuffer, sizeof (szBuffer)))
	{
		*pData = (float)atof (szBuffer);
		PVROSPrintf("%s: %s\n", pszHint, szBuffer);
		return (TRUE);
	}
	return (FALSE);
}


/**************************************************************************
* Function Name  : HWRdValFile
* Inputs         : NONE
* Outputs        : NONE
* Input/Output	  : NONE
*
* Returns        : Returns either value in file or specified default.
*
* Global Used    : Reads config file out of windows
*
* Description    : 
*
*
**************************************************************************/

sgl_int32 CALL_CONV HWRdValFileInt(char * RegName , sgl_int32 DefaultVal)
{
	char szText[32];

	if(GetHwIniStringFromRegistry ("defaults", RegName, szText, 31))
	{
		DefaultVal = (sgl_int32) strtoul (szText, NULL, 0);
	}

	GetApplicationHintInt (RegName, (sgl_int32 *)&DefaultVal);

	/*
	// Else try the default section
	*/
	DefaultVal = SglReadPrivateProfileInt("default", RegName, DefaultVal, "sglhw.ini");
	
	return DefaultVal;
}

float CALL_CONV HWRdValFileFloat(char * RegName , float DefaultVal)
{
	char szText[32];

	if(GetHwIniStringFromRegistry ("defaults", RegName, szText, 31))
	{
		DefaultVal = (float) atof (szText);
	}

	GetApplicationHintFloat (RegName, &DefaultVal);

	/*
	// Else try the default section
	*/
	DefaultVal = SglReadPrivateProfileFloat("default", RegName, DefaultVal, "sglhw.ini");
	
	return DefaultVal;
}


sgl_uint32 CALL_CONV HWRdValFileUInt(char * RegName , sgl_uint32 DefaultVal)
{
	char szText[32];

	if(GetHwIniStringFromRegistry ("defaults", RegName, szText, 31))
	{
		DefaultVal = (sgl_uint32) strtoul (szText, NULL, 0);
	}

	GetApplicationHintInt (RegName, (sgl_int32 *)&DefaultVal);

	/*
	// Else try the default section
	*/
	DefaultVal = SglReadPrivateProfileInt("default", RegName, DefaultVal, "sglhw.ini");
	
	return DefaultVal;
}


sgl_bool CALL_CONV HWRdValFileString(char * RegName , char *pszBuffer,int pszBufferSize)
{
	char szText[128];
	sgl_bool bResult;

	if(bResult = GetHwIniStringFromRegistry ("defaults", RegName, szText, 127))
	{
		strncpy(pszBuffer, szText, pszBufferSize); 
	}

	bResult = GetApplicationHintString (RegName, pszBuffer, pszBufferSize);

	/*
	// Else try the default section
	*/
	bResult = SglReadPrivateProfileString("default", RegName, pszBuffer, pszBuffer, pszBufferSize, "sglhw.ini");
	
	return bResult;
}

/*
// End of da file
*/

