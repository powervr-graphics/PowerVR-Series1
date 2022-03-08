/******************************************************************************
 * Name         : pvrif.c
 * Title        : PVROS library interface module
 * Author       : Jim Page
 * Created      : $Date: 1997/07/28 13:17:42 $
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 3d serial port port driver for output to a palmtop (not)
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: pvrosif.c,v $
 * Revision 1.7  1997/07/28  13:17:42  gdc
 * chnaged so that pvros jump table that is compiled into sgl.dll
 * and sglmid5.dll etc does not export its functions to the world
 *
 * Revision 1.6  1997/06/09  09:22:54  gdc
 * added call convs to functions called externally
 *
 * Revision 1.5  1997/05/13  16:28:17  gdc
 * added a cute windows pupup to give you a message
 *
 * Revision 1.4  1997/05/09  19:18:09  mjg
 * Made bNeedsFree static and used it when PVROSAPI exits.
 *
 * Revision 1.3  1997/05/01  08:04:19  jop
 * Fixed function entry point names
 *
 * Revision 1.2  1997/05/01  02:43:40  jop
 *  Undid Ed's fixes
 *
 *
 *****************************************************************************/

#include <windows.h>

#define API_TYPESONLY
#include "sgl.h"
#undef API_TYPESONLY

/* 
	get types and structures and api fn block 
*/

#define API_FNBLOCK
#include "pvrosapi.h"
#undef API_FNBLOCK

/* 
	declare and initialise jumptable
*/
#define API_JUMPTABLE_NOEXPORT
#include "pvrosapi.h"
#undef API_JUMPTABLE_NOEXPORT

static sgl_bool bNeedsFree = FALSE;

int CALL_CONV PVROSAPIInit ()
{		
	InstanceData.hLib = GetModuleHandle ("PVROS.DLL");

	if (!InstanceData.hLib)
	{
		OutputDebugString ("PVROSAPIInit: Loading library ...\n");
		InstanceData.hLib = LoadLibrary ("PVROS.DLL");
		bNeedsFree = TRUE;
	}
	
	if (InstanceData.hLib == NULL)
	{
		LPVOID lpMsgBuf;

		OutputDebugString ("PVROSAPIInit: Unable to load PVROS.DLL\n");

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL,
					  GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language*/
					  (LPTSTR) &lpMsgBuf,
					  0,
					  NULL );

		/* Display the string.*/
		MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

		/* Free the buffer.*/
		LocalFree( lpMsgBuf );
		
	}			  
	else
	{
		API_GETFNTYPE(PVROSAPI) pfnGetIF;
		
		pfnGetIF = (void *) GetProcAddress (InstanceData.hLib, "GetPVROSAPI_IF");
		
		if (!bNeedsFree)
		{
			InstanceData.hLib = NULL;
		}
		
		if (!pfnGetIF)
		{
			OutputDebugString ("PVROSAPIInit: Unable to get PVROS interface loader\n");
		}
		else if (pfnGetIF (&InstanceData.pIf) != sizeof (API_STRUCTTYPE(PVROSAPI)))
		{
			OutputDebugString ("PVROSAPIInit: PVROS interface is weird\n");
		}
		else
		{
			return (TRUE);
		}

		if (bNeedsFree)
		{
			FreeLibrary (InstanceData.hLib);
			InstanceData.hLib = NULL;
		}
	}
		
	return (FALSE);
}

void  CALL_CONV PVROSAPIExit ()
{
	if (bNeedsFree && InstanceData.hLib)
	{
		FreeLibrary (InstanceData.hLib);
	}
	InstanceData.hLib = NULL;	
}	

/* end of $RCSfile: pvrosif.c,v $ */
