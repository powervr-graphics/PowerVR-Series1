/**************************************************************************
 * Name         : dlglobal.c
 * Title        : Global variables Prototypes for display list editing in SGL
 * Author       : Simon James Fenney
 * Created      : 22 May 1995
 * 
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Global variables Prototypes for display list editing in SGL
 *
 * Platform     : ANSI 
 *
 * Modifications:-
 * $Log: dlglobal.c,v $
 * Revision 1.8  1997/05/13  17:51:18  mjg
 * Added sglmem include.
 *
 * Revision 1.7  1997/04/04  13:02:07  jop
 * Added pvrosapi.h explicit include
 *
 * Revision 1.6  1997/02/04  11:37:46  gdc
 * moved actual definition of dlUserGlobals to rnglobal.c
 * and moved GetNamedItem there aswell
 *
 * Revision 1.5  1996/11/19  00:54:00  jop
 * Added a function to get the name table without having to
 * include a zillion header files
 *
 * Revision 1.4  1996/07/11  16:39:02  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.3  1995/06/29  12:52:14  jrc
 * Changed name in header from dlglobal.h to dlglobal.c
 *
 * Revision 1.2  1995/06/02  15:36:51  jop
 * Added current mesh to global initialisation routine
 *
 * Revision 1.1  1995/05/26  12:36:09  sjf
 * Initial revision
 *
 *
 **************************************************************************/

#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"

#include "nm_intf.h"
#include "dlglobal.h"
#include "getnamtb.h"

#include "pvrosapi.h"
#include "sglmem.h"

/*
// Define the global editing variables
*/
extern DL_USER_GLOBALS_STRUCT	dlUserGlobals;

/**************************************************************************
 * Function Name  : InitDisplayLists
 * Inputs         : None
 * Outputs        : None
 * Returns        : 0 if OK, non zero otherwise.
 * Global Used    : The Display list globals struct etc...
 * Description    : Initialises the display list global variables.
 *
 *					This currently assumes a SINGLE user, but could be easily
 *					modified to support multi process users..
 **************************************************************************/

int InitDisplayLists(void)
{
	int result;
	
	/*
	// Assume all is OK until proven otherwise
	*/
	result = 0;

	/*
	// intialise the various fields in the User Globals structure
	// (This "globals" structure really applies to one user process.
	//
	// Create and initialise a default list
	*/
	dlUserGlobals.pDefaultList = NEW(LIST_NODE_STRUCT);
	if(dlUserGlobals.pDefaultList == NULL)
	{
		/*
		// WHOOPS an error has occurred
		*/
		result = -1;
	}
	else
	{
		InitListNode(dlUserGlobals.pDefaultList, NM_INVALID_NAME, NULL);
	}


	/*
	// create the name table (if the last bit was successfull
	*/
	if(result == 0)
	{
		/*
		// Create and initialise the name table (for this client....)
		*/
		result = InitNamtab( & dlUserGlobals.pNamtab);
	}


	/*
	// Fill in the other fields (regardless of error status)
	*/
	dlUserGlobals.pCurrentList = dlUserGlobals.pDefaultList;
	dlUserGlobals.pCurrentConvex = NULL;
	dlUserGlobals.pCurrentTransform = NULL;
	dlUserGlobals.pCurrentMaterial = NULL;
	dlUserGlobals.pCurrentMesh = NULL;

	return result;
}



/*
// END OF FILE
*/

