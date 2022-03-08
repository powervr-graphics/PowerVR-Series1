/*************************************************************************
* Name : nm_intf.h
* Title : Prototypes for name table interface
* Author : Simon Fenney
* Created : 3/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Prototypes of name table functions for SGL.
*			  See nm_imp.c for a full description of the functionality of the
*			  name table.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: nm_intf.h,v $
 * Revision 1.8  1997/05/08  10:13:26  erf
 * Added prototype for GetTextureHandle().
 *
 * Revision 1.7  1996/07/18  14:38:02  sjf
 * Added routine to return number of named items in particular categories.
 *
 * Revision 1.6  1996/05/08  14:54:48  sjf
 * Added new routine to return the pointer to the item PLUS its
 * type.
 *
 * Revision 1.5  1995/05/30  12:28:19  sjf
 * fixed comment about returned error.
 *
 * Revision 1.4  1995/05/26  12:38:20  sjf
 * Added invalid name #define.
 *
 * Revision 1.3  1995/05/25  15:26:46  sjf
 *  wrong style for GetNamedItemType..
 *
 * Revision 1.2  1995/05/25  14:20:35  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.1  1995/05/04  17:19:10  sjf
 * Initial revision
 *
*
*
**************************************************************************/


/*
// Define an invalid name. In fact, any negative integer is an INVALID name
// but SETTING a name field using this value will be a little more obvious.
// The testing should just see if the number is < 0.
*/
#define NM_INVALID_NAME (-1)



/*
// Define a "dummy" structure/pointer type to access the name table
// without the internal structure being globally visible.
//
// We may potentially have a number of name tables if we ever go to
// a multi user system (e.g. under an operating system), so might as
// well program in the available functionality now.
//
// The user will then refer to a name table by declaring a variable of type
// namtab.
*/

typedef struct
{
	int dummy;
} EXTERNAL_NAMTAB_STRUCT;

typedef EXTERNAL_NAMTAB_STRUCT * P_NAMTAB_STRUCT;





/*///////////////////////////////////////////////////////////////
// Define the interface routines
/////////////////////////////////////////////////////////////////
*/

/*
// Allocate and initialise the name table.
// Returns a non-zero number if an error occured.
*/
extern int InitNamtab(P_NAMTAB_STRUCT * pnamtab);


/*
// Removes all data from the name table
*/
extern void ClearNamtab(P_NAMTAB_STRUCT pnamtab);



/*
// Add an item to the name table, and get a name.
// It returns either the name (if positive) or sgl_err_no_name 
// if the table is full.
*/
extern int AddNamedItem( P_NAMTAB_STRUCT pnamtab,
					void * item,
					int	   entity_type);



/*
// Retrieve a pointer to a named object. If the returned pointer
// is NULL then there is no such entry.
*/
extern void * GetNamedItem(P_NAMTAB_STRUCT pnamtab,
						int	 name);


/*
// Retrieve the type of the stored entity - this is for 
// checking user parameters
*/
extern int GetNamedItemType(P_NAMTAB_STRUCT pnamtab,
						int	 name);


/*
// Retrieve the Pointer and type of the stored entity
*/
extern void * GetNamedItemAndType(P_NAMTAB_STRUCT pnamtab,
						int	 name,
						/*
						// returned values
						*/
						int  *  nType);

/* Get the texture handle.
 */
extern void * GetTextureHandle (int nTextureName);



/*
// Routines to increment and decrement the reference count for a named
// item. It is assumed a valid name is passed. (Do an assert check).
*/
extern void IncNamedItemUsage(P_NAMTAB_STRUCT pnamtab,
	  					int		 name);

extern void DecNamedItemUsage(P_NAMTAB_STRUCT pnamtab,
	  					int		 name);


/*
// Routine to delete a name from the table
*/
extern void  DeleteNamedItem(P_NAMTAB_STRUCT pnamtab,
						int	 name);



/*
// Debug/Statistics routine
*/
typedef struct
{
	int numNamedLists;
	int numNamedMaterials;
	int numNamedMeshes;
	int numNamedConvex;
	int numNamedTextures;
	int numNamedCachedTextures;

} NAME_STATS_STRUCT;

extern void GetNameStats(P_NAMTAB_STRUCT pnamtab,
						NAME_STATS_STRUCT *NameStats);

/*
// END OF FILE
*/

