/*************************************************************************
* Name : nm_imp.c
* Title : Code for name table
* Author : Simon Fenney
* Created : 3/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Implementation of name table functions for SGL
*				This code is based on the concept of the name tables
*				from RGL.
*				Basically, a name table maintains a mapping from a
*				system assigned name (a positive integer (16 bit))
*				to a pointer to the named item. 
*
*				The mapping from Name to Pointer must be "fast".
*
*				When a name is released, it should be a while before it
*				is re-used. This makes it easier for the user to catch
*				errors where an attempt to access a "deleted" item is made.
*
*				The name table can also maintain a "reference count" for a
*				name. This is to keep track of instance usage of a particular
*				item. If that item is deleted, the references to it may not have
*				and to reduce the risk of the re-use of the name, the reference
*				count will prevent the "deleted" name being re-used until the count
*				reaches zero.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: nm_imp.c,v $
 * Revision 1.20  1997/06/04  15:29:57  gdc
 * removed compiler warnings
 *
 * Revision 1.19  1997/05/27  16:26:32  mjg
 * Added the sgl_delete_all_textures function, the include of texapi.h
 * and a couple of externs for the interfaces.
 *
 * Revision 1.18  1997/05/13  17:54:56  mjg
 * Added sglmem include an a cast.
 *
 * Revision 1.17  1997/05/09  10:26:46  erf
 * Tidied up GetTextureHandle() function.
 *
 * Revision 1.16  1997/05/08  10:12:27  erf
 * Added GetTextureHandle() function to simplify named texture business in
 * dtri.c and dpoint.c.
 *
 * Revision 1.15  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.14  1997/04/04  13:04:06  jop
 * Added explicit #include "pvrosapi.h"
 *
 * Revision 1.13  1996/07/18  14:37:27  sjf
 * Added routine to return number of named items in particular categories.
 *
 * Revision 1.12  1996/06/26  10:34:27  sjf
 * Hopefully have implemented a better method of resetting the name
 * base value.
 *
 * Revision 1.11  1996/05/23  16:56:51  sjf
 * Increased the size of the name table (at request of
 * Activision/NonLinearArts).
 *
 * Revision 1.10  1996/05/08  14:54:25  sjf
 * Added new routine to return the pointer to the item PLUS its
 * type.
 *
 * Revision 1.9  1995/09/01  16:42:03  jrc
 * Fixed incorrect comment for AddNamedItem (returns sgl_err_no_name if it
 * fails, not -1).
 *
 * Revision 1.8  1995/07/14  15:00:58  sjf
 * Moved module id def to correct place.
 *
 * Revision 1.7  1995/07/11  10:59:01  sm
 * fixed bug in DeleteItem
 *
 * Revision 1.6  1995/06/01  10:44:30  sjf
 * Added debugging module id.
 *
 * Revision 1.5  1995/05/30  12:27:48  sjf
 * changed error to return one of the standard sgl error codes.
 *
 * Revision 1.4  1995/05/26  12:37:50  sjf
 * moved ANSI .h's to sgl_defs.h
 *
 * Revision 1.3  1995/05/25  15:26:14  sjf
 * wrong style for GetNamedItemType..
 *
 * Revision 1.2  1995/05/25  14:20:20  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.1  1995/05/04  17:19:10  sjf
 * Initial revision
 *
*
*
**************************************************************************/


/*
// Define the Debugging ID
*/
#define MODULE_ID MODID_NM

#include <limits.h>
#include "sgl_defs.h"
#include "nm_intf.h"
#include "dlntypes.h"
#include "pvrosapi.h"
#include "texapi.h"
#include "sglmem.h"
#include "getnamtb.h"

/*
// For the initial version, stick with a fixed sized name table.
// Stay with the KISS priciple for the moment.
//
// To produce a very efficient "hash" function, use a power of 2, and
// simply use the lower bits of the name to access the table of entries.
*/
#define SHIFT_UP	15 /*how many bits are used for indexing in the table*/
#define MAX_NAM_TAB (1 << (SHIFT_UP-1))
#define HASH_MASK   (MAX_NAM_TAB - 1)


/*
// Assume a MAX number of times that an item is EVER going to be referenced
// (This is to prevent overflow hassles
*/
#define MAX_REF_COUNT 20000

/* ////////////////////////////////////////////
// Define the structure type for each entry in the table
// ////////////////////////////////////////////
*/
typedef struct
{
	/* 
	// Name stored at this location, or -1 if the "slot" is
	// free OR is one which is to be deleted BUT the reference
	// count was not ZERO
	*/
	sgl_int16	n16name;

	/* Reference count (eg for instances)*/
	sgl_int16	n16count;	
	
	/*
	// The following field is used to speed up allocation of new
	// names once the name table starts to get fragmented.
	*/
	sgl_int16	n16nextfree;	

	/*
	// WHEN a valid name is stored, this is "type" of named entity.
	// IF the name has been deleted, this stores the original name UNTIL
	// the reference count drops to 0. In the free case, it should be
	// set to -1.
	*/
	sgl_int16	n16entity_type; 

	/* Pointer to the data */
	void	* p_entity;	
	
} TAB_ENT_STRUCT;


/*
// Define the entire name table structure
*/
typedef struct
{
	/* 
	// The basis for the next name to use. This is incremented, with
	// each allocation, and then the lower bits are masked off and
	// combined with the index to produce a name.
	//
	// When it gets too big, it is reset to zero.
	*/
	int name_base;

	/*
	// Keep track of which entries are free in the table via a simple
	// linked list. To make life REALLY simple for us, we consider there
	// to be no available names when the first and last free indices are
	// equal. That is, there actually is one free element left, however
	// ignoring it just reduces the number of cases to handle a bit.....
	*/
	int first_free;	/* The free entry in a "linked List" to use*/
	int last_free;	/* The last entry in that list */
	
	TAB_ENT_STRUCT entries[MAX_NAM_TAB];

}NAMTAB_STRUCT;


extern HLDEVICE 	gHLogicalDev;
extern PTEXAPI_IF 	gpTextureIF;


/**************************************************************************
 * Function Name  : clear_entries		INTERNAL ONLY ROUTINE
 * Inputs Only    : 
 * Outputs Only   : 
 * Input/output	  : p_namtab  - reinitialises the contents of a name table
 * Returns        : None
 * Global Used    : None
 * Description    : Reinitialises the entries of a name table.
 *					INTERNAL ONLY
 **************************************************************************/
static void clear_entries(NAMTAB_STRUCT * p_namtab)
{
	/*
	// Pointer to an individual entry
	*/
	TAB_ENT_STRUCT * p_ent;

	/*
	// generic loop counter
	*/
	int i;


	/*
	// Pointer to step through the table entries
	*/
	p_ent = & p_namtab->entries[0];

	/*
	// step through the struct initialising the names to
	// be unallocated (i.e. -1) and setting up the next free
	// spot (i.e. the next one - except for the very last
	*/
	for(i=0; i < MAX_NAM_TAB; i ++, p_ent++)
	{
		/*
		// fill in its details
		*/
		p_ent->n16name = -1;	/*indicates free (ish)*/
		p_ent->n16count=  0;
		p_ent->n16nextfree = i+1;
		p_ent->n16entity_type = -1; /* indicates free*/
		p_ent->p_entity = NULL;

	}/*end for*/

	/*
	// tidy up the last one as there is no next index...
	*/
	p_namtab->entries[MAX_NAM_TAB -1 ].n16nextfree = -1;

	/*
	// Set up where to go looking for a free spot,
	// and the base to produce names.
	*/
	p_namtab->name_base = 0;

	p_namtab->first_free = 0;
	p_namtab->last_free  = (MAX_NAM_TAB - 1);

}



/**************************************************************************
 * Function Name  : InitNamtab
 * Inputs Only    : 
 * Outputs Only   : 
 * Input/output	  : namtab    - the name table to construct
 * Returns        : non zero if error occured.
 * Global Used    : None
 * Description    : Allocates space for the name table, initialises the 
 *					the housekeeping values, and returns the pointer in the
 *					the variable. Returns a non zero value if an error occurs
 *					(i.e. out of memory)
 **************************************************************************/

int InitNamtab(P_NAMTAB_STRUCT * namtab_param)
{
	/*
	// initialise result to assume success
	*/
	int result = 0;

	/*
	// Pointer to the real data
	*/
	NAMTAB_STRUCT * p_namtab;

	/*
	// Allocate the name table structure
	*/
	p_namtab = NEW(NAMTAB_STRUCT);

	/*
	// If the allocation failed, then report an error (non zero)
	*/
	if(p_namtab == NULL)
	{
		result = -1;
		*namtab_param = NULL;
	}
	/*
	// Else save the pointer to it in the parameter, and intialise
	// the internals
	*/
	else	
	{
		/*
		// Save the pointer to the struct
		*/
		*namtab_param = (P_NAMTAB_STRUCT)p_namtab;

		/*
		// Initialise all the entries
		*/
		clear_entries(p_namtab);

	}/* end else */

	return(result);
}


/**************************************************************************
 * Function Name  : ClearNamtab
 * Inputs Only    : 
 * Outputs Only   : 
 * Input/output	  : namtab    - the name table to clear
 * Returns        : none.
 * Global Used    : None
 * Description    : Clears out all the contents of the name table.
 **************************************************************************/

void ClearNamtab(P_NAMTAB_STRUCT namtab)
{
	/*///////
	// Variable defs
	/////// */

	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;

	ASSERT(namtab != NULL)

	/*
	// Recast the pointer for convenience
	// (with a bit of luck, the compiler will optimise this out)
	*/
	p_namtab = (NAMTAB_STRUCT *) namtab;

	clear_entries(p_namtab);
}


/**************************************************************************
 * Function Name  : AddNamedItem
 * Inputs Only    : item (a pointer), and entity type
 * Outputs Only   : 
 * Input/output	  : namtab    - the name table to add to
 * Returns        : assigned name of added entity
 * Global Used    : None
 * Description    : Adds a new entry to the name table, and returns
 *					a name for it. If there was an error (no free names),
 *					it returns sgl_err_no_name.
 **************************************************************************/

int AddNamedItem( P_NAMTAB_STRUCT namtab,
					void *	   item,
						int	   entity_type)
{
	int name;
	int free_slot;

	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;
	
	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;


	/*
	// retype the namtab to the internal format
	*/
	ASSERT(namtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) namtab;



	/*
	// Get the next free spot in the table
	*/
	free_slot = p_namtab->first_free;
	
	/*
	// if the table is "full"...
	*/
	if(free_slot == p_namtab->last_free)
	{
		name =  sgl_err_no_name;
	}
	/*
	// else fill in the details
	*/
	else
	{
		/*
		// get easy access to this entry
		*/
		p_ent =& p_namtab->entries[free_slot];

		/*
		// Update the next free entry in the main name table
		//
		// (And, when debugging, in the entry as well)
		*/
		p_namtab->first_free = p_ent->n16nextfree;

		#ifdef DEBUG
		p_ent->n16nextfree = -2;
		#endif

		/*
		// Compute a name for this thing, by masking off the index
		// bits in the name base, and then putting in the entry index.
		*/
		name = (p_namtab->name_base & ~HASH_MASK) | free_slot;


		/*
		// Store the data in the entry
		*/
		p_ent->n16name = (sgl_int16) name;
		p_ent->n16count=  0;
		p_ent->n16entity_type = (sgl_int16) entity_type;
		p_ent->p_entity = item;


		/*
		// move the name base on to the next potential entry,
		// reseting it back to zero if it grows too large.
		//
		// NOTE Dont let it get bigger than our 16Bit names! Say
		// allow it to 3/4s of the max value.
		*/
		p_namtab->name_base += MAX_NAM_TAB;
		if(p_namtab->name_base > ((3*SHRT_MAX)/4) )
		{
			p_namtab->name_base = 0;
		} /*end if*/
		
	} /*end else*/
	

	return (name);
}

/**************************************************************************
 * Function Name  : GetNamedItem
 * Inputs Only    : name, namtab
 * Outputs Only   : 
 * Returns        : A pointer to the item etc
 * Global Used    : None
 * Description    : Finds the named entity in the table, and returns a pointer
 *					to it, or NULL if its not in the table.
 **************************************************************************/
void * GetNamedItem(P_NAMTAB_STRUCT namtab,
					int		 name)
{
	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;
	
	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;

	void * result;

	/*
	// retype the namtab to the internal format
	*/
	ASSERT(namtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) namtab;


	/*
	// Check that the name is at least positive.
	*/
	if(name < 0)
	{
		result = NULL;
	}
	else
	{
		/*
		// Map the name to the unique index, and get a pointer
		// to that entry
		*/
		p_ent =& p_namtab->entries[name & HASH_MASK];

		/*
		// Do the names match? If so get the pointer to the data
		*/
		if(p_ent->n16name == name)
		{
			result = p_ent->p_entity;
		}
		/*
		// Else this name is unknown
		*/
		else
		{
			result = NULL;
		}
		
	}/*end else*/

	return (result);
}

/**************************************************************************
 * Function Name  : GetNamedItemType
 * Inputs Only    : name, namtab
 * Outputs Only   : 
 * Returns        : the type of the entity - -1 if the name is invalid
 * Global Used    : None
 * Description    : Finds the named entity in the table, and returns its type
 *					or -1 if its not in the table etc
 **************************************************************************/
int  GetNamedItemType(P_NAMTAB_STRUCT namtab,
							int		 name)
{
	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;
	
	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;

	int result;

	/*
	// retype the namtab to the internal format
	*/
	ASSERT(namtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) namtab;

	/*
	// Check that the name is at least positive.
	*/
	if(name < 0)
	{
		result = -1;
	}
	else
	{
		/*
		// Map the name to the unique index, and get a pointer
		// that entry
		*/
		p_ent =& p_namtab->entries[name & HASH_MASK];

		/*
		// Do the names match? If so get the entity type
		*/
		if(p_ent->n16name == name)
		{
			result = p_ent->n16entity_type;
		}
		/*
		// Else this name is unknown
		*/
		else
		{
			result = -1;
		}
		
	}/*end else*/

	return (result);
}

/**************************************************************************
 * Function Name  :  GetNamedItemAndType
 * Inputs Only    : name, namtab
 * Outputs Only   : the type of the entity - -1 if the name is invalid
 * Returns        : The pointer to the entity.
 * Global Used    : None
 * Description    : Finds the named entity in the table, and returns its type
 *					or -1 if its not in the table etc
 **************************************************************************/
void * GetNamedItemAndType(P_NAMTAB_STRUCT pExternNamtab,
						int	 name,
						int  *  nType)
{
	/*
	// Pointer to the item
	*/
	void * pItem;

	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;
	
	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;

	/*
	// retype the namtab to the internal format
	*/
	ASSERT(pExternNamtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) pExternNamtab;


	/*
	// Check that the name is at least positive.
	*/
	if(name < 0)
	{
		pItem = NULL;
		*nType = -1;
	}
	else
	{
		/*
		// Map the name to the unique index, and get a pointer
		// to that entry
		*/
		p_ent =& p_namtab->entries[name & HASH_MASK];

		/*
		// Do the names match? If so get the pointer to the data
		*/
		if(p_ent->n16name == name)
		{
			pItem = p_ent->p_entity;
			*nType = p_ent->n16entity_type;
		}
		/*
		// Else this name is unknown
		*/
		else
		{
			pItem = NULL;
			*nType = -1;
		}
		
	}/*end else*/

	return pItem;
}


/**************************************************************************
 * Function Name  : GetTextureHandle
 * Inputs Only    : 
 * Outputs Only   : 
 * Input/output	  : nTextureName    - The texture name
 * Returns        : Texture handle
 * Global Used    : None
 * Description    : Returns the texture handle.
 *
 **************************************************************************/

void * GetTextureHandle (int nTextureName)
{
	void 			*hTexture= NULL;
	P_NAMTAB_STRUCT pNamtab;
	
	if (pNamtab = (P_NAMTAB_STRUCT) GetNameTable ())
	{
		if (GetNamedItemType (pNamtab, nTextureName) == nt_texture)
		{
			hTexture = GetNamedItem (pNamtab, nTextureName);
		}
	}
	return(hTexture);
}

/**************************************************************************
 * Function Name  : sgl_delete_all_textures
 * Inputs Only    : -
 * Outputs Only   : -
 * Input/output	  : -
 * Returns        : -
 * Global Used    : None
 * Description    : Searches the entire name-table for all the 
 * 					textures and removes them. 
 **************************************************************************/

void CALL_CONV sgl_delete_all_textures ( void )
{
	void 			*hTexture= NULL;
	P_NAMTAB_STRUCT pNamtab;
	TAB_ENT_STRUCT * pEnt;			/*  Pointer to an individual entry */
	int i;

	if (pNamtab = (P_NAMTAB_STRUCT) GetNameTable ())
	{
		/* Step through the name table */
		NAMTAB_STRUCT * p_namtab = (NAMTAB_STRUCT *) pNamtab;		

		ASSERT(p_namtab != NULL);
	
		pEnt = & p_namtab->entries[0];
		for(i= MAX_NAM_TAB; i != 0 ; i --, pEnt++)
		{
			if (pEnt->n16entity_type == nt_texture)
			{
				gpTextureIF->pfnTextureFree (gHLogicalDev->TexHeap, pEnt->p_entity);
			}	
		}	 
	}	
}


/**************************************************************************
 * Function Name : add_to_free_list 	INTERNAL ONLY ROUTINE
 * Inputs Only   : index, namtab
 * Outputs Only  : 
 * Returns       : None
 * Global Used   : None
 * Description   : Given the index of an entry, it cleans up the values contained
 *				   and adds it on to the free list of entries. It is put on the
 *				   end to reduce the chance of the same name be reproduced...
 **************************************************************************/
static void add_to_free_list(NAMTAB_STRUCT *p_namtab,
						int  index)
{
	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;

	/*
	// Get access to the current last entry and set its next
	// index to be the entry which is being added on...
	*/
	p_ent = & p_namtab->entries[p_namtab->last_free];
	p_ent->n16nextfree = index;

	/*
	// get access to the entry being plocked on the end
	*/
	p_ent = & p_namtab->entries[index];

	/*
	// clear out the two relevent fields that mark it as free,
	// (and for neatness, set the next free to be -1)
	*/
	p_ent->n16name = -1;
	p_ent->n16entity_type = -1;
	p_ent->n16nextfree = -1;

	/*
	// finally, set the last_free to index this entry
	*/
	p_namtab->last_free = index;

}


/**************************************************************************
 * Function Name : change_named_item_usage		INTERNAL ONLY ROUTINE
 * Inputs Only   : name, namtab
 * Outputs Only  : 
 * Returns       : None
 * Global Used   : None
 * Description   : Finds the named entity in the table, and either increments 
 *				or decrements the count	on the number of times it is
 *				referenced (mainly for instances).
 *				This reference count prevents the name from being re-used by
 *				by accident.
 *
 *				If the name has been deleted (i.e. name is -1, but the entity
 *				field matches the name) then decrement the count, and if it
 *				hits zero, then add it back onto the free list.
 *
 *				This routine will ASSERT an error (in the debug version)
 *				if an invalid name is given (as this shouldn't occur, I think)
 *
 *				It also tests that the count does not get too large - 
 *				In the debug version it asserts, and in the non debug, just
 *				prevents the counts from going out of control...
 **************************************************************************/
static void change_named_item_usage(NAMTAB_STRUCT * p_namtab,
				  					sgl_int16		 name,
									int			 increment)
{

	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;

	/*
	// Check that the name is at least positive.
	*/
	if(name >= 0)
	{
		/*
		// Map the name to the unique index, and get a pointer
		// that entry
		*/
		p_ent =& p_namtab->entries[name & HASH_MASK];

		/* /////////////
		// Has this entry been deleted?
		///////////// */
		if(p_ent->n16name == -1)
		{
			/* 
			// Then check if this is the one we want, and we are just waiting
			// for the usage count to hit zero.
			*/
			if(p_ent->n16entity_type == name)
			{
				ASSERT(p_ent->n16count > 0)

				if(increment)
				{
					/*
					// This shouldn't be happening!!!
					*/
					ASSERT(FALSE);
				}
				/*
				// Decrement, and free up the entry if the count
				// hits zero
				*/
				else
				{
					p_ent->n16count --;

					/*
					// Do we really delete it this time?
					// If so, add it to the free list
					*/
					if(p_ent->n16count == 0)
					{
						add_to_free_list(p_namtab, name & HASH_MASK);
					}
				}/* Else decrementing */

			} /*end if name matches */
		}

		/* /////////////
		// Else, is this name unrecognised?
		///////////// */
		else if(p_ent->n16name != name)
		{
			/* Do nothing*/
		}

		/* /////////////
		// Else, do we increment?
		///////////// */
		else if(increment)
		{
			p_ent->n16count ++;

			/*
			// Prevent Overflow - also in DEBUG assert if too large as this
			// probably indicates an error somewhere...
			*/
			ASSERT(p_ent->n16count < 1000);
			if(p_ent->n16count > MAX_REF_COUNT)
			{
				p_ent->n16count = MAX_REF_COUNT;
			}
		}
		/* /////////////
		// Else decrement the usage count
		///////////// */
		else
		{
			p_ent->n16count --;

			/*
			// Dont allow it to go negative. In fact it should NEVER
			// do this...
			*/
			ASSERT(p_ent->n16count >= 0)
			if(p_ent->n16count < 0)
			{
				p_ent->n16count = 0;
			}
		}/*end else decrement*/
		
	}/*end IF - name is positive*/
}
									


			
/**************************************************************************
 * Function Name  : IncNamedItemUsage
 * Inputs Only    : name, namtab
 * Outputs Only   : 
 * Returns        : None
 * Global Used    : None
 * Description    : Finds the named entity in the table,increments the count
 *					on the number of times it is referenced (mainly for instances)
 *					This reference count prevents the name from being re-used by
 *					by accident.
 **************************************************************************/
void IncNamedItemUsage(P_NAMTAB_STRUCT namtab,
							int		 name)
{
	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;

	/*
	// retype the namtab to the internal format
	*/
	ASSERT(namtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) namtab;
	
	change_named_item_usage( p_namtab, (sgl_int16)name, 1);

}


/**************************************************************************
 * Function Name  : DecNamedItemUsage
 * Inputs Only    : name, namtab
 * Outputs Only   : 
 * Returns        : None
 * Global Used    : None
 * Description    : Finds the named entity in the table, decrements the count
 *					on the number of times it is referenced (mainly for instances)
 *					This reference count prevents the name from being re-used by
 *					by accident.
 **************************************************************************/
void DecNamedItemUsage(P_NAMTAB_STRUCT namtab,
							int		 name)
{
	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;

	/*
	// retype the namtab to the internal format
	*/
	ASSERT(namtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) namtab;
	
	change_named_item_usage( p_namtab, (sgl_int16)name, 0);
}



/**************************************************************************
 * Function Name  : DeleteNamedItem
 * Inputs Only    : name
 * Outputs Only   : 
 * In Out		  : namtab
 * Returns        : None
 * Global Used    : None
 * Description    : Finds the named entity in the table,and deletes the name.
 **************************************************************************/
extern void DeleteNamedItem(P_NAMTAB_STRUCT namtab,
						int	 name)
{

	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab;

	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;


	/*
	// retype the namtab to the internal format
	*/
	ASSERT(namtab != NULL)
	p_namtab = (NAMTAB_STRUCT *) namtab;


	if(name >= 0)
	{
		/*
		// Map the name to the unique index, and get a pointer
		// that entry
		*/
		p_ent = & p_namtab->entries[name & HASH_MASK];
		
		/* /////////////
		// Does the name match?
		///////////// */
		if(p_ent->n16name == name)
		{
			/*
			// If the usage count is Zero, then really delete it
			*/
			if(p_ent->n16count == 0)
			{
				add_to_free_list(p_namtab, name & HASH_MASK);
			}
			/*
			// Else, copy the name into the entity_type (i.e. its other
			// role in life), and mark the entry as deleted
			*/
			else
			{
				p_ent->n16entity_type = name;
				p_ent->n16name = -1;
			}

		}/*end if name matches*/

	} /* end if */

}


/**************************************************************************
 * Function Name  : GetNameStats
 * Inputs Only    : namtab
 * Outputs Only   : NameStats
 * In Out		  : 
 * Returns        : None
 * Global Used    : None
 * Description    : Simply counts up the number of names in a variety of
 *					categories.
 *					This routine is part of debug (for the user)
 **************************************************************************/

void GetNameStats(P_NAMTAB_STRUCT pnamtab,	NAME_STATS_STRUCT *pNameStats)
{
	/*  Pointer to the real data */
	NAMTAB_STRUCT * p_namtab = (NAMTAB_STRUCT *) pnamtab;

	/*  Pointer to an individual entry */
	TAB_ENT_STRUCT * p_ent;

	int i;

	ASSERT(p_namtab != NULL);

	/*
	// Initialise the counts of each item type
	*/
	pNameStats->numNamedLists 	  =	0;
	pNameStats->numNamedMaterials =	0;
	pNameStats->numNamedMeshes	  = 0;
	pNameStats->numNamedConvex	  = 0;
	pNameStats->numNamedTextures  =	0;
	pNameStats->numNamedCachedTextures = 0;
	

	/*
	// Step through the name table and count the number of each type
	*/
	p_ent = & p_namtab->entries[0];
	for(i= MAX_NAM_TAB; i != 0 ; i --, p_ent++)
	{
		switch(p_ent->n16entity_type)
		{
			case nt_list_node: 
			{
				pNameStats->numNamedLists ++;
				break;
			}

			case nt_material: 
			{
				pNameStats->numNamedMaterials++;
				break;
			}

			case nt_convex: 
			{
				pNameStats->numNamedConvex++;
				break;
			}

			case nt_mesh: 
			{
				pNameStats->numNamedMeshes++;
				break;
			}

			case nt_texture: 
			{
				pNameStats->numNamedTextures++;
				break;
			}

			case nt_cached_texture: 
			{
				pNameStats->numNamedCachedTextures ++;
				break;
			}


			/*
			// Default - either the entry is empty, or we aren't interested
			// in this type of named item.
			*/
			default:
			{
				break;
			}
		}

	}/*end for*/
	

}




/*
// END OF FILE
*/


