/**************************************************************************
 * Name         : dlglobal.h
 * Title        : Global variables Prototypes for display list editing in SGL
 * Author       : Simon James Fenney
 * Created      : 22 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Global variables Prototypes for display list editing in SGL
 *
 * Platform     : ANSI 
 *
 * Modifications:-
 * $Log: dlglobal.h,v $
 * Revision 1.2  1995/06/02  15:35:45  jop
 * Added current mesh to global structure
 *
 * Revision 1.1  1995/05/26  12:36:35  sjf
 * Initial revision
 *
 *
 **************************************************************************/


/*
// Define a structure type to hold the global display list variables.
// This should make it easier to extend the system to support multiple
// clients later on...
*/
typedef struct
{
	/*
	// Pointer to the default list, as described in SGL doc
	*/
	LIST_NODE_STRUCT	*pDefaultList;

	/*
	// pointer to the clients name table
	*/
	P_NAMTAB_STRUCT		pNamtab;

	/*
	// pointers to the various current items...
	// Current list, current primitive, current transform, current material and current mesh
	*/
	LIST_NODE_STRUCT		*pCurrentList;
	CONVEX_NODE_STRUCT		*pCurrentConvex;
	TRANSFORM_NODE_STRUCT	*pCurrentTransform;
	MATERIAL_NODE_STRUCT	*pCurrentMaterial;
	MESH_NODE_STRUCT		*pCurrentMesh;

} DL_USER_GLOBALS_STRUCT;





/*
// Declare a single global struct for a single "user" system. This could
// be extended later
*/
extern DL_USER_GLOBALS_STRUCT	dlUserGlobals;
		     



/*
// Routine to initialise the Display Variables
*/
extern int InitDisplayLists(void);

/*--------------------------- End of File --------------------------------*/
