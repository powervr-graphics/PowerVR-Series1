/*****************************************************************************
;++
Name            :   $Source: /user/rcs/revfiles/sabre/sgl/RCS/profile.c,v $
Title           :   Portable profile (ini file) services
C Author        :   Jim Page
Created         :   19/04/95

Copyright		: 	1995-2022 Imagination Technologies (c)
License			: 	MIT

Description     :   Like Windows profile commands
                    
Program Type    :   C module (ANSI)

RCS info:

  $Date: 1997/12/15 16:04:41 $
  $Revision: 1.26 $
  $Locker:  $
  $Log: profile.c,v $
 * Revision 1.26  1997/12/15  16:04:41  erf
 * Need to add HWINI\\ so registry search path in GetHwIniStringFromReg().
 *
 * Revision 1.25  1997/12/12  12:26:19  erf
 * Use define WHICH_CARD_REGISTRY_STRING (defined in pvrosapi.h)
 * to determine where to look for the manufacturer.
 *
 * Revision 1.24  1997/12/11  20:05:18  erf
 * Added GetManStringFromRegistry() routine which is used to
 * select the correct registry area depending on the PCX
 * manufacturer.
 *
 * Revision 1.23  1997/09/24  14:43:39  gdc
 * read private profile string now DOES NOT read registry
 *
 * Revision 1.22  1997/09/17  13:27:13  sxy
 * Removed warnings.
 *
 * Revision 1.21  1997/07/28  10:38:42  gdc
 * removed ^M's
 *
 * Revision 1.20  1997/07/14  10:00:44  gdc
 * tried to turn off windows include file warnings and failed
 *
 * Revision 1.19  1997/06/03  18:41:29  jop
 * Err, couldn't think what else to do other than check the buffer in
 * while jimbo's not about... (jwh).
 *
 * Revision 1.18  1997/06/02  02:45:55  jop
 * WIN32 builds use native private profile stuff
 *
 * Revision 1.17  1997/05/08  10:43:20  jop
 * Fixed bug shown up by debug c library
 *
 * Revision 1.16  1997/04/26  18:48:10  erf
 * Added USE_HEAPMAN
 *
 * Revision 1.15  1997/04/04  13:04:19  jop
 * Downgraded a trace message
 *
 * Revision 1.14  1997/04/03  17:52:42  mjg
 * Another CALL_CONV.
 *
 * Revision 1.13  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.12  1997/03/14  11:06:53  jwf
 * changed some stuff to make it compile on the macintosh
 *
 * Revision 1.11  1996/10/03  16:32:29  mal
 * Reverted back to previous version.
 *
 * Revision 1.10  1996/10/02  17:19:58  mal
 * Added some profiling functions:
 * 1. SglTimeNow()
 * 2. SglCPuFreq()
 * These will be used by WIN32 code when -DMETRIC flag
 * is enabled at compile time.
 *
 * Revision 1.9  1996/07/19  10:37:14  msh
 * Certain bits NOP'ed for MIDAS Arcade
 *
 * Revision 1.8  1996/05/11  12:54:45  sjf
 * Started to make the INI files case insensitive. So far I've made the section
 * identifiers CI.
 *
 * Revision 1.7  1996/04/26  16:52:07  pdl
 * changed MIDASPC1 to WIN32.
 *
 * Revision 1.6  1995/12/18  18:54:30  sjf
 * stroul was missing on suns ....
 *
 * Revision 1.5  1995/11/27  10:50:21  pdl
 * Changed atoi to strtoul
 *
 * Revision 1.4  1995/10/27  18:11:45  sjf
 * Added comment facility to config file.
 *
 * Revision 1.3  1995/09/30  19:56:07  jop
 * Fixed bug - entries read correctly now
 *
 * Revision 1.2  1995/09/25  11:25:38  jop
 * Made entry detection case sensitive - strnicmp not supported in UNIX
 *
 * Revision 1.1  1995/09/22  19:18:05  jop
 * Initial revision
 *
;--
*****************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sgl.h"
#include "sgl_defs.h"
#include "profile.h"

#include "pvrosapi.h"

#if WIN32
#pragma message ("Using windows ReadPrivateProfileString")
#pragma warning ( disable : 117 )
#include <windows.h>
#pragma warning ( default : 117 )
#endif

#if !WIN32

typedef struct tagPROFILE
{
	char *szFilename;
	char *Buffer;
	int	 nSection;
	int  nStartEntry;
	int  nEndEntry;
	long lSize;
	
} PROFILE, *PPROFILE;



/*===========================================
 * Function:	SglOpenProfile
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		A quick change to produce a case insensitive compare function
 *
 * Params:		As for strncmp
 *
 * Return:		0 if the same, NON zero if different
 *========================================================================================*/
static int strncmpCI (char * pS1, char * pS2, int len)
{
	char c1, c2;

	for(/*NIL*/; len != 0; len --, pS1++, pS2++)
	{
		c1 = tolower(*pS1);
		c2 = tolower(*pS2);

		if(c1 == '\0')
		{
			if(c2 == '\0')
				return (0);
			else
				return(1);
		}
		else if(c2 == '\0')
		{
			return(1);
		}
		else if(c1 != c2)
		{
			return(1);
		}
		/*
		// Move on to the next one
		*/
	}


	return (0);
}




/*===========================================
 * Function:	SglOpenProfile
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		Copies the contents of pP->szFilename to a buffer
 *
 * Params:		PPROFILE pP: ini file contex
 *
 * Return:		TRUE if load successful, FALSE if error or no file
 *========================================================================================*/
static sgl_bool SglOpenProfile (PPROFILE pP)
{
	sgl_bool bRet = FALSE;
	FILE *fp = fopen (pP->szFilename, "rt");

	pP->Buffer = NULL;
	pP->lSize = 0;
		
	if (fp)
	{
		fseek (fp, 0, SEEK_END);
		
		pP->lSize = ftell (fp);

		fseek (fp, 0, SEEK_SET);

		bRet = TRUE;
		
		if (pP->lSize > 0)
		{
			pP->Buffer = PVROSMalloc (pP->lSize + 1);

			if (!pP->Buffer)
			{
				DPF ((DBG_ERROR, "Error allocating profile buffer"));
				bRet = FALSE;
			}
			else
			{
				pP->lSize = fread (pP->Buffer, 1, pP->lSize, fp);

				// zero terminate the buffer
				pP->Buffer[pP->lSize] = 0;
			}
		}

		fclose (fp);
	}

	return (bRet);
}

/*===========================================
 * Function:	SglFindProfileSection
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		Finds a section in an ini file
 *
 * Params:		PPROFILE pP: ini file contex
 *				char *szEntry: find the named section in the buffer and set 'Section'
 *				pointer
 *
 * Return:		TRUE if section found
 *========================================================================================*/
static sgl_bool SglFindProfileSection (PPROFILE pP, char *szEntry)
{
	if (pP->Buffer)
	{
		char *pS, *pE;
		char *pBr[2] = {"[", "\n["}, *pB;

		pP->nSection = -1;

		pB = pBr[0];
		pS = strstr (pP->Buffer, pB);
			
		while (pS)
		{
			pS += strlen (pB);
			pE = strstr (pS, "]\n");

			if (pE && !strncmpCI (pS, szEntry, (int) pE - (int) pS))
			{
				/* section found */
				pP->nSection = (int) (pE + 2) - (int) pP->Buffer;
				return (TRUE);
			}

			pB = pBr[1];
			pS = strstr (pS, pB);
		}
	}

	return (FALSE);
}

/*===========================================
 * Function:	SglFindProfileEntry
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		finds an ini file entry in an already found section
 *
 * Params:		PPROFILE pP: ini file contex
 *				char *szEntry: find the named entry in the selected section and
 *				set 'StartEntry' and 'EndEntry' ptrs.
 *				pointer
 *
 * Return:		TRUE if entry found
 *========================================================================================*/
static sgl_bool SglFindProfileEntry (PPROFILE pP, char *szEntry)
{
	sgl_bool bRet = FALSE;
	
	pP->nStartEntry = -1;
	pP->nEndEntry = -1;

	if (pP->Buffer && (pP->nSection != -1))
	{
		char *pNextSection = strstr (&pP->Buffer[pP->nSection], "\n[");
		char szTemp[128];
		char *pEntry;
		char *pEndEntry;

		strcpy (szTemp, szEntry);
		strcat (szTemp, "=");

		/*
		// Search for the Entry with = concatentated.  BUT if the previous
		// character was a ";" (i.e. a comment) then search again
		*/
		pEntry = strstr (&pP->Buffer[pP->nSection], szTemp);
		while(pEntry!=NULL)
		{
			if(pEntry[-1]!= ';')
			{
				break;
			}
			/*
			// Search again
			*/
			pEntry = strstr (pEntry+1, szTemp);
			
		}


		if (pEntry!=NULL)
		{
			pEntry += strlen (szTemp);
			pEndEntry = strchr (pEntry, '\n');
			
			if (pNextSection)
			{
				if (pEntry < pNextSection)
				{
					pP->nStartEntry = (int) (pEntry - pP->Buffer);
					pP->nEndEntry = (int) (pEndEntry - pP->Buffer);
					bRet = TRUE;
				}
			}
			else
			{
				pP->nStartEntry = (int) (pEntry - pP->Buffer);
				pP->nEndEntry = (int) (pEndEntry - pP->Buffer);
				bRet = TRUE;
			}
		}
	}		

	return (bRet);
}

/*===========================================
 * Function:	SglWriteProfile
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		Writes profile to disk. Makes backup if necessary; creates an new
 *				file, section or entry if necessary.
 *
 * Params:		PPROFILE pP: file context
 *				char *szSection: section to modify
 *				char *szEntry: entry to modify
 *				char *szText: replacement string for entry
 *
 * Return:		TRUE if operation successful
 *========================================================================================*/
static sgl_bool SglWriteProfile (PPROFILE pP, char *szSection, char *szEntry, char *szText)
{
#ifndef MIDAS_ARCADE
	sgl_bool bRet = FALSE, bBackup = TRUE;
	char szTempfile[256];
	FILE *fp;
	
	strcpy (szTempfile, pP->szFilename);
	strcat (szTempfile, ".temp");
	
	if (rename (pP->szFilename, szTempfile))
	{
		DPF ((DBG_MESSAGE, "Error renaming temp ini file"));
		bBackup = FALSE;
	}

	fp = fopen (pP->szFilename, "wt");

	if (fp)
	{
		if (pP->Buffer)
		{
			if (pP->nSection != -1)
			{
				if (pP->nStartEntry != -1)
				{
					if (fwrite (pP->Buffer, 1, pP->nStartEntry, fp) > 0)
					{
						if (fprintf (fp, "%s", szText) > 0)
						{
							if (fwrite (&pP->Buffer[pP->nEndEntry], 1, pP->lSize - pP->nEndEntry, fp) > 0)
							{
								bRet = TRUE;
							}
						}
					}
				}
				else
				{
					if (fwrite (pP->Buffer, 1, pP->nSection, fp) > 0)
					{
						if (fprintf (fp, "%s=%s\n", szEntry, szText) > 0)
						{
							if (fwrite (&pP->Buffer[pP->nSection], 1, pP->lSize - pP->nSection, fp) > 0)
							{
								bRet = TRUE;
							}
						}
					}
				}
			}
			else
			{
				if (fwrite (pP->Buffer, 1, pP->lSize, fp) > 0)
				{
					if (fprintf (fp, "\n\n[%s]\n%s=%s\n", szSection, szEntry, szText) > 0)
					{
						bRet = TRUE;
					}
				}
			}
		}
		else
		{
			if (fprintf (fp, "SGL private initialisation file: %s\n\n[%s]\n%s=%s\n", pP->szFilename, szSection, szEntry, szText) > 0)
			{
				bRet = TRUE;
			}
		}

		fclose (fp);
	}

	if (bBackup)
	{
		if (bRet)
		{
			remove (szTempfile);
		}
		else
		{
			remove (pP->szFilename);
			rename (szTempfile, pP->szFilename);
		}
	}
	
	return (bRet);
#else
	return (TRUE);
#endif
}

/*===========================================
 * Function:	SglWriteProfile
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		cleans up after profile operation.
 *
 * Params:		PPROFILE pP: file context
 *
 * Return:		always TRUE
 *========================================================================================*/
static sgl_bool SglCloseProfile (PPROFILE pP)
{
	if (pP->Buffer)
	{
		PVROSFree (pP->Buffer);
		pP->Buffer = NULL;
	}

	return (TRUE);
}

#else /* !WIN32 */

/*===========================================
 * Function:	GetManStringFromRegistry
 *===========================================
 *
 * Scope:		Global
 *
 * Purpose:		Gets the manufacturers registry area.
 *
 * Params:		
 *
 * Return:		
 *========================================================================================*/
sgl_bool GetManStringFromRegistry (char *pszBuffer)
{
	sgl_bool 	fRet = FALSE;
	char 		szText[32];
	HKEY 		hKey;
	MANUFACTURER_TYPE	eManufacturerType = VIDEOLOGIC_3D;
	DWORD		dwType, dwSize = sizeof(szText);

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
					case VIDEOLOGIC_3D:	strcpy(pszBuffer, VL_REGISTRY_STRING);
										break;

					case MATROX_M3D:	strcpy(pszBuffer, MATROX_REGISTRY_STRING);
										break;

					default:			strcpy(pszBuffer, VL_REGISTRY_STRING);
										break;
				}

				fRet = TRUE;
			}
			else
			{
				PVROSPrintf("Weird data type SOFTWARE\\PowerVR\\StartUp BoardType\n");
			}
		}
		else
		{
			PVROSPrintf("Couldn't access string SOFTWARE\\PowerVR\\StartUp BoardType\n");
		}

		RegCloseKey(hKey);
	}
	else
	{
		PVROSPrintf("Couldn't access key SOFTWARE\\PowerVR\\StartUp\n");
	}

	return (fRet);
}


/*===========================================
 * Function:	GetHwIniStringFromRegistry
 *===========================================
 *
 * Scope:		static
 *
 * Purpose:		Reads ini string from registry
 *
 * Params:		
 *
 * Return:		
 *========================================================================================*/
sgl_bool GetHwIniStringFromRegistry (char *pszSection, char *pszEntry, char *pszBuffer, int pszBufferSize)
{
	sgl_bool 	fRet = FALSE;
	char 		szKey[512];
	HKEY 		hKey;
	MANUFACTURER_TYPE	eManufacturerType = VIDEOLOGIC_3D;
	DWORD		dwType;

	/* Get the registry area depending on the hardware present ie Maxtrox m3D or PowerVR.
	 */
	if (GetManStringFromRegistry (szKey))
	{
		/* Go to hardware section.
		 */
		strcat (szKey, "HWINI\\");
		strcat (szKey, pszSection);

		if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx (hKey, pszEntry, 0, &dwType, (LPBYTE)pszBuffer, (LPDWORD)&pszBufferSize) == ERROR_SUCCESS)
			{
				if (dwType == REG_SZ)
				{
					fRet = TRUE;
				}
				else
				{
					DPF ((DBG_ERROR, "Weird data type %s %s %lx\n", szKey, pszEntry, dwType));
				}
			}
			else
			{
				DPF ((DBG_MESSAGE, "Couldn't access key %s %s\n", szKey, pszEntry));
			}

			RegCloseKey (hKey);
		}
	}

	return (fRet);
}

#endif

/*===========================================
 * Function:	SglWritePrivateProfileString
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		Write a string into an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: secion in ini file
 *				char *szEntry: entry in section
 *				char *szText: text to insert
 *				char *szFilename: name of initialisation file
 *
 * Return:		TRUE if successfully written
 *========================================================================================*/
sgl_bool CALL_CONV SglWritePrivateProfileString (char *szSection, char *szEntry, char *szText, char *szFilename)
{
	#if WIN32

		return (WritePrivateProfileString (szSection, szEntry, szText, szFilename));

	#else

		sgl_bool bRet = FALSE;
		PROFILE P;

		P.szFilename = szFilename;

		if (SglOpenProfile (&P))
		{
			SglFindProfileSection (&P, szSection);
			SglFindProfileEntry (&P, szEntry);
		}

		if (SglWriteProfile (&P, szSection, szEntry, szText))
		{
			bRet = TRUE;
		}

		return (bRet);

	#endif
}

/*===========================================
 * Function:	SglWritePrivateProfileInt
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		Write a number (int) into an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: secion in ini file
 *				char *szEntry: entry in section
 *				int nValue: number to insert
 *				char *szFilename: name of initialisation file
 *
 * Return:		TRUE if successfully written
 *========================================================================================*/
sgl_bool CALL_CONV SglWritePrivateProfileInt (char *szSection, char *szEntry, int nValue, char *szFilename)
{
	char szText[32];

	sprintf (szText, "%d", nValue);
	
	return (SglWritePrivateProfileString (szSection, szEntry, szText, szFilename));
}

/*===========================================
 * Function:	SglReadPrivateProfileString
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		read a string from an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: ini file section
 *				char *szEntry: entry in section
 *				char *szDefault: default string to return if unsuccsesful (non NULL!)
 *				char *szText: buffer for return string
 *				int nTextSize: length of buffer
 *				char *szFilename: name of ini file
 *
 * Return:		TRUE if successfully read
 *========================================================================================*/
sgl_bool CALL_CONV SglReadPrivateProfileString (char *szSection, char *szEntry, char *szDefault, char *szText, int nTextSize, char *szFilename)
{
	#if WIN32

		return (GetPrivateProfileString (szSection, szEntry, szDefault, szText, nTextSize, szFilename));

	#else

		sgl_bool bRet = FALSE;
		PROFILE P;

		P.szFilename = szFilename;

		if (!SglOpenProfile (&P))
		{
			/* Mac has no profile/prefs file yet so this gets called a lot! */
			DPF ((DBG_VERBOSE, "Error opening profile"));
		}
		else
		{
			if (SglFindProfileSection (&P, szSection))
			{
				if (SglFindProfileEntry (&P, szEntry))
				{
					int nProfileSize = (P.nEndEntry - P.nStartEntry);

					CHOOSE_MIN (nTextSize, nProfileSize);

					strncpy (szText, &P.Buffer[P.nStartEntry], nTextSize);
					szText[nTextSize] = 0;
					bRet = TRUE;
				}
			}

			if (!bRet)
			{
				strncpy (szText, szDefault, nTextSize);
			}
			
			SglCloseProfile (&P);
		}

		return (bRet);

	#endif
}

/*===========================================
 * Function:	SglReadPrivateProfileInt
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		read a string from an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: ini file section
 *				char *szEntry: entry in section
 *				char *Default: default number to return on failure
 *				char *szFilename: name of ini file
 *
 * Return:		int value from ini file if successful, or nDefault on failure
 *========================================================================================*/
int CALL_CONV SglReadPrivateProfileInt (char *szSection, char *szEntry, int nDefault, char *szFilename)
{
	int nResult;
	char szText[32], szDefault[32];

	nResult = nDefault;
	sprintf (szDefault, "%d", nDefault);
	
	if (SglReadPrivateProfileString (szSection, szEntry, szDefault, szText, 
									sizeof (szText), szFilename))
	{
		#if WIN32
		nResult = (int) strtoul (szText, NULL, 0);
		#else
		nResult = (int) strtol (szText, NULL, 0);
		#endif
	}

	return (nResult);
}

/*===========================================
 * Function:	SglReadPrivateProfileFloat
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		read a string from an ini file, section and entry,
 *
 * Params:		char *szSection: ini file section
 *				char *szEntry: entry in section
 *				float Default: default number to return on failure
 *				char *szFilename: name of ini file
 *
 * Return:		int value from ini file if successful, or nDefault on failure
 *========================================================================================*/
float CALL_CONV SglReadPrivateProfileFloat (char *szSection, char *szEntry, float fDefault, char *szFilename)
{
	float fResult;
	char szText[32], szDefault[32];

	fResult = fDefault;
	sprintf (szDefault, "%f", fDefault);
	
	if (SglReadPrivateProfileString (szSection, szEntry, szDefault, szText, 
									sizeof (szText), szFilename))
	{
		fResult = (float) atof (szText);
	}

	return (fResult);
}
/* end of $Source: /user/rcs/revfiles/sabre/sgl/RCS/profile.c,v $ */

