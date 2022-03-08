/*****************************************************************************
;++
Name           	:   $Source: /user/rcs/revfiles/sabre/sgl/RCS/debug.c,v $
Title           :   SGL Debugging routines
C Author       	:   Jim Page
Created         :   19/04/95

Copyright		: 	1995-2022 Imagination Technologies (c)
License			: 	MIT

Description     :   Routines to help people get meaningful debugging output
					from SGL and the other tools.
                    
Program Type    :   C module (ANSI)

RCS info:

  $Date: 1997/11/28 15:23:32 $
  $Revision: 1.64 $
  $Locker:  $
  $Log: debug.c,v $
 * Revision 1.64  1997/11/28  15:23:32  sxy
 * added a seperate control gnOutputToConsole for developer and timing builds.
 *
 * Revision 1.63  1997/11/24  10:40:26  sxy
 * added a macro DPFTIME(()) for timing build.
 *
 * Revision 1.62  1997/11/10  16:24:43  sxy
 * added disable the PVRDebug window in DebugInit() and enable the window
 * in DebugDeinit().
 *
 * Revision 1.61  1997/10/23  17:12:06  sxy
 * changed the number of shared buffer for DPFDEV into a parameter NUMBER_OF_SHARED_BUFFER.
 *
 * Revision 1.60  1997/10/22  17:16:07  sxy
 * added more shared buffer in dfprintf() for PostMessage(0.
 *
 * Revision 1.59  1997/10/16  15:50:25  sxy
 * changed some things in dfprintf(0.
 *
 * Revision 1.58  1997/10/16  11:03:19  gdc
 * chnaged developer build stuff to make it tidier
 *
 * Revision 1.57  1997/10/14  14:10:41  sxy
 * added a function dfprintf() for output to log file.
 *
 * Revision 1.56  1997/09/24  09:04:45  sxy
 * Made it available to both DEBUG and DEBUGDEV.
 *
 * Revision 1.55  1997/07/24  14:01:38  gdc
 * debug now make DLL its linked to emit correct dll name
 *
 * Revision 1.54  1997/07/11  11:52:25  gdc
 * now reads the sgl.ini file again for debug messages
 *
 * Revision 1.53  1997/04/30  19:17:41  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.52  1997/04/29  14:39:41  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.51  1997/04/27  22:41:01  jop
 * Added preceding 'SGL: ' back into debug messages
 *
 * Revision 1.50  1997/04/21  22:43:33  gdc
 * added stub for non debug builds to avoid export errors
 *
 * Revision 1.49  1997/04/21  19:03:58  gdc
 * got rid of a couple of warnings
 *
 * Revision 1.48  1997/04/07  11:03:05  mjg
 * Removed an amperstand for WIN32 builds.
 *
 * Revision 1.47  1997/04/04  13:01:54  jop
 * Fixed a couple of small bugs
 *
 * Revision 1.46  1997/04/03  19:28:28  jop
 * Cleaned things up a bit
 *
 * Revision 1.45  1997/04/02  11:53:45  mjg
 * Silly syntactical slip.
 *
 * Revision 1.44  1997/04/01  19:15:05  mjg
 * Removed bulk of dassert to system.c
 *
 * Revision 1.43  1997/04/01  17:42:01  jop
 * Half finished OS specific stuff, Mike can have the rest
 * of it, lucky chap.
 *
 * Revision 1.42  1997/03/29  22:00:06  mjg
 * Inserted new standardised PVROSDelay function.
 *
 * Revision 1.41  1997/03/27  19:20:35  erf
 * Removed routines to monitoring memory management.
 *
 * Revision 1.40  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.39  1997/03/25  10:13:58  jop
 * Fixed a small bug to shorten file names in debug messages
 *
 * Revision 1.38  1996/11/14  20:04:20  mjg
 * Set the level of DPF messages for Simulation files to MESSAGE,
 * as it's quite handy and left all other debug output to default to
 * WARNING.
 *
 * Revision 1.37  1996/09/27  14:26:27  jop
 * Fiddled with filenames and ODS
 *
 * Revision 1.36  1996/07/10  09:28:35  pdl
 * added some ifwin32
 *
 * Revision 1.35  1996/07/08  04:14:37  sjf
 * Added temp define to allow mem logging  of release builds.
 *
 * Revision 1.34  1996/07/05  15:11:15  jop
 * Added some ini settings for D3D
 *
 * Revision 1.33  1996/05/09  12:03:02  sjf
 * Added "Ignore" option to assert.
 *
 * Revision 1.32  1996/04/02  16:19:10  jop
 * Re-enabled asserts in DD mode
 *
 * Revision 1.31  1996/03/22  16:50:03  sjf
 * Put mem logging back in AND it works on *NIX.
 *
 * Revision 1.30  1996/03/11  12:08:36  sjf
 * Hopefully, fixed up problems with DDRAW and DEBUG in WIN32 versions.
 *
 * Revision 1.29  1996/03/07  17:14:22  sjf
 * Added more memory log info for Windows.
 *
 * Revision 1.28  1996/02/28  10:00:19  ncj
 * Compatible with direct draw 2.0a2
 *
 * Revision 1.27  1996/02/28  09:42:47  sjf
 * changed formating of memory loging a little AND added in win32 physical
 * memory logging
 *
 * Revision 1.26  1996/02/26  15:36:01  sjf
 * Added memory usage trace functions
 *
 * Revision 1.25  1996/02/12  10:45:33  ncj
 * The debug dialog messes up direct draw, so if DDRAW defined it is excluded
 *
 * Revision 1.24  1995/11/27  12:19:46  pdl
 * added re-enter checking.
 *
 * Revision 1.23  1995/10/22  18:42:02  sjf
 * Disabled Simulator window stuff.
 *
 * Revision 1.22  1995/09/25  15:29:31  jop
 * Moved saveinisettings outside ifdef
 *
 * Revision 1.21  1995/09/22  19:17:36  jop
 * Added support for portable ini file stuff
 *
 * Revision 1.20  1995/09/05  09:07:57  sjf
 * Unfortunately, the last change meant that debug code
 * automatically got included in the NON debug versions. I have
 * changed the #if defined  back to #if  BUT have made sure
 * that DEBUG and _DEBUG are always defined. (to be 0 if they
 * were undefined)
 *
 * Revision 1.19  1995/09/04  13:13:57  jop
 * changed #if to #if defined
 *
 * Revision 1.18  1995/08/24  17:34:46  jop
 * Did it properly this time
 *
 * Revision 1.16  1995/08/24  17:09:52  jop
 * Changed to new export keyword.
 *
 * Revision 1.15  1995/08/21  11:48:45  jop
 * Added support for debugger button
 *
 * Revision 1.14  1995/08/01  16:59:12  jop
 * Stopped debug window being 'Always on top'
 *
 * Revision 1.13  1995/07/27  20:40:24  jop
 * Made the appropriate changes to window version
 *
 * Revision 1.12  1995/07/26  19:21:19  jop
 * Did it properly this time
 *
 * Revision 1.11  1995/07/26  19:19:08  jop
 * DebugInit() now initialised with default debug level
 *
 * Revision 1.10  1995/07/26  19:14:59  sjf
 * Made debug more verbose (prints file and line)
 *
 * Revision 1.9  1995/07/26  16:26:32  jop
 * preview button in debug dlg
 *
 * Revision 1.8  1995/07/25  12:20:47  jop
 * made more UNIX compatible
 *
 * Revision 1.7  1995/07/23  17:04:05  jop
 * Added NT debug window support (maintaining UNIX version OK??)
 *
 * Revision 1.6  1995/07/17  16:03:30  jop
 * changed uses of PSTR type to char *
 *
 * Revision 1.5  1995/06/06  09:46:47  sjf
 * Changed character input routine to use an ANSI standard (I think).
 *
 * Revision 1.4  1995/06/05  15:48:21  jop
 * removed BOOL typedef - changed to int
 *
 * Revision 1.3  1995/05/09  11:48:09  sjf
 * Changed #includes to use sgl_defs.h
 *
 * Revision 1.2  1995/04/27  13:42:32  jop
 * Changed text format
 *
 * Revision 1.1  1995/04/20  14:55:35  jop
 * Initial revision
 *

;--
*****************************************************************************/

#define __DEBUG_C__
#define MODULE_ID MODID_DEBUG

#ifdef WIN32
#include <direct.h>
#define getcwd _getcwd
#endif

#include "sgl.h"
#include "sgl_defs.h"
#include "pvrosapi.h"
#include "debug.h"
#include "modauto.h"	/* auto-generated Module data definition */
#include "profile.h"

static sgl_int32	snModuleDebugLevels[NUM_ITEMS_IN_MODULES_ARRAY];
static char		*pszLevelStrings[] = {"Fatal: ", "Error: ", "Warning: ", "Message: ", ""};

/* if use the log file(sgldebug.txt) for developer's DPFDEV messages or not*/
#if DEBUGDEV || TIMING
   	#define APPNAME "PVRDEBUG"
	sgl_bool	gbUseLogFile = FALSE;
    sgl_uint32 hwnd;
	#define NUMBER_OF_SHARED_BUFFER 20
	char *szSharedBuffer[NUMBER_OF_SHARED_BUFFER];
	int gnOutputToConsole;
#endif

#if DEBUG || DEBUGDEV  || TIMING 

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>

#if DEBUG

extern char sDllName[];

void PrintDLL()
{
	PVROSPrintf(sDllName);
}

/*===========================================
 * Function:	dprintf
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		allows manageable module/level based tracing with a variable
 *				arg list. Outputs to stderr.
 *
 * Params:		sgl_int32	nCodeModule:	from defines in mod_id.h
 * 				sgl_int32	nDebugLevel:	from defines in debug.h
 * 				PSTR	pszFormat:		format string
 * 						...:			parameters
 *
 * Return:		Always TRUE
 *========================================================================================*/
void dprintf ( sgl_int32 nCodeModule, 
					  sgl_int32 nDebugLevel, 
					  char *FileName,
					  int  Line,
					  char *pszFormat, ...)
{
	char *pLeafName;
	
	pLeafName = (char *)strrchr (FileName, '\\');
	
	if (pLeafName)
	{
		FileName = pLeafName;
	}
		
	if (snModuleDebugLevels[nCodeModule] >= nDebugLevel)
	{
		va_list vaArgs;
		char szBuffer[256];

		va_start (vaArgs, pszFormat);

		strcpy (szBuffer, pszLevelStrings[nDebugLevel]);
#if DOS32
		vsprintf (&szBuffer[strlen(szBuffer)], pszFormat, &vaArgs);
#else
		vsprintf (&szBuffer[strlen(szBuffer)], pszFormat, vaArgs);
#endif
		sprintf (&szBuffer[strlen(szBuffer)], " [%d, %s]\r\n", Line, FileName);

		/* print the name of the module were in */
		PrintDLL();
		PVROSPrintf (szBuffer);

		va_end (vaArgs);
	}
}

void dassert (char *szAssertText, char *szFileName, sgl_int32 nLine, int * ask)
{
	char *pLeafName;
	
	pLeafName = (char *)strrchr (szFileName, '\\');
	
	if (pLeafName)
	{
		szFileName = pLeafName;
	}
		
	PVROSAssert( szAssertText, szFileName, nLine, ask);
}

void DebugSetDebugLevel (sgl_int32 nCodeModule, sgl_int32 nDebugLevel)
{
	snModuleDebugLevels[nCodeModule] = nDebugLevel;
	SglWritePrivateProfileInt ("ModuleDebugLevels", Modules[nCodeModule].pszName, nDebugLevel, "SGL.INI");
}

void DebugInit (sgl_uint32 DebugLevel)
{
	int k;
	char buffer[_MAX_PATH+16];

	/* Get the current working directory: */
	if( getcwd( buffer, _MAX_PATH ) == NULL )
	{
		PVROSPrintf ("_getcwd error\n");
	}	
	else
	{
		strcat(buffer,"\\SGL.INI");
	}

	for (k = 0; k < NUM_ITEMS_IN_MODULES_ARRAY; ++k)
	{
		snModuleDebugLevels[k] = (int) SglReadPrivateProfileInt ("ModuleDebugLevels", Modules[k].pszName, DebugLevel, buffer);	
	}
}


void DebugDeinit ()
{
	int k;

	/* save debug levels for modules */
			
	if (SglReadPrivateProfileInt ("Debug", "SaveSettings", 1, "SGL.INI"))
	{
		for (k = 0; k < NUM_ITEMS_IN_MODULES_ARRAY; ++k)
		{
			SglWritePrivateProfileInt ("ModuleDebugLevels", Modules[k].pszName, snModuleDebugLevels[k], "SGL.INI");
		}
	}
}

#endif

#if DEBUGDEV

extern char sDllName[];

void PrintDLL()
{
	PVROSPrintf(sDllName);
}

void DebugInit (sgl_uint32 DebugLevel)
{
	int k;
	char buffer[_MAX_PATH+16];

	/* Get the current working directory: */
	if( getcwd( buffer, _MAX_PATH ) == NULL )
	{
		PVROSPrintf ("_getcwd error\n");
	}	
	else
	{
		strcat(buffer,"\\SGL.INI");
	}


	DebugLevel = (int) SglReadPrivateProfileInt ("DEBUG", "GlobalLevel", DebugLevel, buffer);	

	for (k = 0; k < NUM_ITEMS_IN_MODULES_ARRAY; ++k)
	{
		snModuleDebugLevels[k] = (int) SglReadPrivateProfileInt ("ModuleDebugLevels", Modules[k].pszName, DebugLevel, buffer);	
	}

	/* open the log file for DPFDEV */
	gbUseLogFile = SglReadPrivateProfileInt ("DEBUG", "UseLogFile", 0, buffer);

	if(gbUseLogFile)
	{
		hwnd = (sgl_uint32)FindWindow (APPNAME, NULL);
		if(!hwnd)
		{  
			PVROSPrintf("Fail to find PVRDEBUG!\n");
			gbUseLogFile = FALSE;
			return;
		}
		
		for(k=0; k<NUMBER_OF_SHARED_BUFFER; k++)
		{
			if (!szSharedBuffer[k])
			{
				szSharedBuffer[k] = PVROSDbgDevBuffer(k);
			}
			if(!((sgl_uint32)szSharedBuffer[k] & 0x80000000))
			{
				PVROSPrintf("Cant Get szSharedBuffer\n");
				gbUseLogFile = FALSE;
				return;
			} 										
		}
		/* disable PVRDebug window */
		PostMessage( (HWND)hwnd, WM_ENABLE, (BOOL)FALSE, (LPARAM)NULL);
							
	}
}

void DebugDeinit ()
{
	/* enable PVRDebug window */
	PostMessage( (HWND)hwnd, WM_ENABLE, (BOOL)TRUE, (LPARAM)NULL);
}

/*===========================================
 * Function:	dfprintf
 *===========================================
 *
 * Purpose:		allows manageable module/level based tracing with a variable
 *				arg list. Outputs to a log file.
 *
 * Params:		sgl_int32	nCodeModule:	from defines in mod_id.h
 * 				sgl_int32	nDebugLevel:	from defines in debug.h
 *				HFILE	hfLogFile:		file handle
 * 				PSTR	pszFormat:		format string
 * 						...:			parameters
 *
 *========================================================================================*/
void dfprintf ( sgl_int32 nCodeModule, 
					  sgl_int32 nDebugLevel, 
					  char *FileName,
					  int  Line,
					  char *pszFormat, ...)
{
	char *pLeafName;
	
	pLeafName = (char *)strrchr (FileName, '\\');
	
	if (pLeafName)
	{
		FileName = pLeafName;
	}
	if (snModuleDebugLevels[nCodeModule] >= nDebugLevel)
	{
		va_list vaArgs;
		char szBuffer[256];
		int k, n=0;
		   
	   	strcpy(szBuffer,sDllName);

		va_start (vaArgs, pszFormat);

		strcat (szBuffer, pszLevelStrings[nDebugLevel]);
#if DOS32
		vsprintf (&szBuffer[strlen(szBuffer)], pszFormat, &vaArgs);
#else
		vsprintf (&szBuffer[strlen(szBuffer)], pszFormat, vaArgs);
#endif
		sprintf (&szBuffer[strlen(szBuffer)], " [%d, %s]\r\n", Line, FileName);

		if(gnOutputToConsole)
		{	
			PVROSPrintf (szBuffer);
		}

		if(gbUseLogFile && hwnd)
		{		
			while(n < 3)
			{
				for(k=0; k<NUMBER_OF_SHARED_BUFFER; k++)
				{
					if(szSharedBuffer[k][0] == '8')
					{
						strcpy(szSharedBuffer[k], szBuffer);
						PostMessage( (HWND)hwnd, WM_USER, 0, (LPARAM) szSharedBuffer[k]);
						va_end (vaArgs);
						return;
					}
				}
				/* wait for a buffer to be ready */
				n++;
				Sleep(300);
			}
			/* give up to post messages */
			PVROSPrintf("Time is up, give up to post messages to PVRDebug\n");
			gbUseLogFile = FALSE;
		}

		va_end (vaArgs);
	}
}

#endif

#if TIMING

void DebugInit (sgl_uint32 DebugLevel)
{
	int k;
	char buffer[_MAX_PATH+16];

	/* Get the current working directory: */
	if( getcwd( buffer, _MAX_PATH ) == NULL )
	{
		PVROSPrintf ("_getcwd error\n");
	}	
	else
	{
		strcat(buffer,"\\SGL.INI");
	}
	
	/* open the log file for DPFTIME */
	gbUseLogFile = SglReadPrivateProfileInt ("DEBUG", "UseLogFile", 0, buffer);

	if(gbUseLogFile)
	{
		hwnd = (sgl_uint32)FindWindow (APPNAME, NULL);
		if(!hwnd)
		{  
			PVROSPrintf("Fail to find PVRDEBUG!\n");
			gbUseLogFile = FALSE;
			return;
		}
		
		for(k=0; k<NUMBER_OF_SHARED_BUFFER; k++)
		{
			if (!szSharedBuffer[k])
			{
				szSharedBuffer[k] = PVROSDbgDevBuffer(k);
			}
			if(!((sgl_uint32)szSharedBuffer[k] & 0x80000000))
			{
				PVROSPrintf("Cant Get szSharedBuffer\n");
				gbUseLogFile = FALSE;
				return;
			} 										
		}
		/* disable PVRDebug window */
		PostMessage( (HWND)hwnd, WM_ENABLE, (BOOL)FALSE, (LPARAM)NULL);
							
	}
}

void DebugDeinit ()
{
	/* enable PVRDebug window */
	PostMessage( (HWND)hwnd, WM_ENABLE, (BOOL)TRUE, (LPARAM)NULL);
}

/*===========================================
 * Function:	dtprintf
 *===========================================
 *
 * Purpose:		allows manageable variable arg list outputs to a log file.
 *
 * Params:		PSTR	pszFormat:		format string
 * 				  			  ...:	   	parameters
 *
 *========================================================================================*/
void dtprintf (char *pszFormat, ...)
{
	va_list vaArgs;
	char szBuffer[256];
	int k, n=0;
		   
	if(gnOutputToConsole || (gbUseLogFile && hwnd))
	{
		va_start (vaArgs, pszFormat);

		strcpy(szBuffer, "");
#if DOS32
   		vsprintf (&szBuffer[strlen(szBuffer)], pszFormat, &vaArgs);
#else
   		vsprintf (&szBuffer[strlen(szBuffer)], pszFormat, vaArgs);
#endif
  	 	sprintf (&szBuffer[strlen(szBuffer)], "\r\n");

		if(gnOutputToConsole)
		{	
   			PVROSPrintf (szBuffer);
		}

  	 	if(gbUseLogFile && hwnd)
   		{		
   			while(n < 3)
   			{
   				for(k=0; k<NUMBER_OF_SHARED_BUFFER; k++)
   				{
   					if(szSharedBuffer[k][0] == '8')
   					{
   						strcpy(szSharedBuffer[k], szBuffer);
   						PostMessage( (HWND)hwnd, WM_USER, 0, (LPARAM) szSharedBuffer[k]);
   						va_end (vaArgs);
   						return;
   					}
   				}
   				/* wait for a buffer to be ready */
   				n++;
   				Sleep(300);
   			}
   			/* give up to post messages */
   			PVROSPrintf("Time is up, give up to post messages to PVRDebug\n");
   			gbUseLogFile = FALSE;
  	 	}
		va_end (vaArgs);
	}
}

#endif

#else
void DebugInit (sgl_uint32 DebugLevel)
{

}

#endif /*DEBUG*/

/* end of $RCSfile: debug.c,v $ */






