 /******************************************************************************
 * Name : brdcfg.c
 * Title : Midas Board Config
 * Author : Simon Fenney
 * Created : 27/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : Reads configuration file from windows to decide what
 *				 kind of board is being used...
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: brdcfg.c,v $
 * Revision 1.3  1997/04/03  19:45:00  mjg
 * Added a CALL_CONV.
 *
 * Revision 1.2  1997/04/01  18:43:33  mjg
 * Changed notprintf to PVROSPrintf.
 *
 * Revision 1.1  1997/03/31  19:01:02  mlj
 * Initial revision
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

#include "sgl_defs.h"
#include "sgl.h"
#include "profile.h"
#include "brdcfg.h"
#include "string.h"







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
	int dummyLen;
	if(!ModeDetermined)
	{

		/*
		// Init the name of the config file
		*/
		strcpy(ConfigFile, getenv(WINDOWS_ENV));

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
#if 0
sgl_bool GetApplicationHintString (char *pszHint, char *pszBuffer, int pszBufferSize)
{
	sgl_bool fRet = FALSE;
	char szCaller[256];
	char szKey[512] = "SOFTWARE\\VideoLogic\\PowerVR\\App Hints\\";

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

	return (fRet);
}


sgl_bool GetApplicationHintInt (char *pszHint, INT32 *pData)
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
		*pData = atof (szBuffer);
		PVROSPrintf("%s: %s\n", pszHint, szBuffer);
		return (TRUE);
	}
	return (FALSE);
}
#endif

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

UINT32 CALL_CONV HWRdValFile(char * RegName , UINT32 DefaultVal)
{
	char ReadVal[100];
	int DummyInt;

	InitDetermineMode();

#if 0
	if (GetApplicationHintInt (RegName, &DefaultVal))
	{
		DPF((DBG_MESSAGE, 
			"Reading Reg \"%s\" with val \"%s\" and got %lx from registry",
						  RegName, ReadVal, DefaultVal, HWMode));
	}
	/*
	// See if this is in a specific section
	*/
	else
#endif
	 if(SglReadPrivateProfileString(HWMode, RegName, " ", 
						ReadVal, 100, ConfigFile))
	{
		if(ReadVal[0]=='0' && ReadVal[1]=='x')
		{
			DefaultVal = strtol(ReadVal, NULL, 16);
		}
		else
		{
			DefaultVal = strtol(ReadVal, NULL, 10);
		}

		DPF((DBG_MESSAGE, 
			"Reading Reg \"%s\" with val \"%s\" and got %lx from [%s]",
						  RegName, ReadVal, DefaultVal, HWMode));
	}
	/*
	// Else try the default section
	*/
	else if(SglReadPrivateProfileString("default", RegName, " ", 
						ReadVal, 100, ConfigFile))
	{
		if(ReadVal[0]=='0' && ReadVal[1]=='x')
		{
			DefaultVal = strtol(ReadVal, NULL, 16);
		}
		else
		{
			DefaultVal = strtol(ReadVal, NULL, 10);
		}

		DPF((DBG_MESSAGE, 
		"Reading Reg \"%s\" with val \"%s\" and got %lx from [default]",
						  RegName, ReadVal, DefaultVal));
	}
	/*
	// Else use the default supplied value
	*/
	else
	{
		DPF((DBG_MESSAGE, 
	 	   "Reg \"%s\" Using Default  %lx", RegName, DefaultVal));
		/*do nothing*/
	}
	
	return DefaultVal;
}


sgl_uint32 CALL_CONV InitEnvironment(void)
{
	 return FindBoardAndInitAddresses ();
}


/*
// End of da file
*/

