/******************************************************************************
 * Name			: adjacenc.c
 * Title		: Adjacency calculation for convex primitives.
 * Author		: Simon Fenney.  Conversion to SGL: John Catchpole
 * Created		: 10/06/1993
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	: Routine for calculating adjacency (edge) information of
 *				  convex objects.  Used by dlconvex.c
 *				  See the design document trdd0050.doc for details.
 *
 * Platform		: ANSI
 *
 * Modifications:
 * $Log: adjacenc.c,v $
 * Revision 1.15  1997/06/04  15:31:19  gdc
 * stopped compiler warnings
 *
 * Revision 1.14  1997/05/13  17:49:46  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.13  1997/04/30  19:17:00  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.12  1997/04/29  14:36:41  erf
 * Added new PVROS API function calls.
 *
 * Revision 1.11  1997/04/23  09:15:00  mjg
 * Removed compiler warnings.
 *
 * Revision 1.10  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.9  1997/01/13  11:54:04  gdc
 * added some ASSERTS beside mallocs to flag bad malloc in debug builds
 *
 * Revision 1.8  1996/10/11  12:07:29  msh
 * "small number" checks modified for Midas Arcade
 *
 * Revision 1.7  1996/09/12  15:10:57  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_INTERNAL_PLANES.
 * Shadows may now have more than SGL_MAX_PLANES.
 * Now set to twice SGL_MAX_PLANES. Re: NHE 044A, 049A.
 *
 * Revision 1.6  1996/06/03  15:27:52  jop
 * Changed edges constant
 *
 * Revision 1.5  1995/10/24  17:23:29  jrc
 * Added handling for no planes in GenerateAdjacencyInfo.
 *
 * Revision 1.4  1995/08/03  10:42:13  jrc
 * Fixed MAX_VERTS for hexagon mesh type objects.
 *
 * Revision 1.3  1995/07/30  10:54:29  sjf
 * Changed typedef name of bounding box.
 *
 * Revision 1.2  1995/07/27  12:18:48  jrc
 * Now should accept a single plane ok.
 *
 * Revision 1.1  1995/06/30  12:17:20  jrc
 * Initial revision
 *
 *
 * RGL HISTORY:
 *
 * Revision 1.6  94/01/04  10:50:06  SimonF
 * Added code to keep a small cache of vertex/edge nodes,
 * because a lot of time is otherwise spent in the memory
 * manager.
 * 
 * Revision 1.5  93/12/13  10:06:18  SimonF
 * Attempted to make the inverse matrix routine a bit faster.
 * 
 * Revision 1.4  93/11/11  08:58:40  SimonF
 * Fixed bug where accessing the 'external' adjacency info
 * structure results in accesses to incorrect parts of memory.
 * 
 * Revision 1.3  93/10/11  10:05:15  SimonF
 * 1) Removed a dead parameter from one of the functions.
 * 2) Cleaned up warnings about type conversions.
 * 
 * Revision 1.2  93/09/16  15:19:42  SimonF
 * Removed a debug error check that would cause problems
 * with unbounded objects.
 * 
 * Revision 1.1  93/09/03  10:59:45  SimonF
 * Initial revision
 *
 *  		10th June 1993		Initial Version		Simon Fenney
 *
 *****************************************************************************/


#define MODULE_ID MODID_DL

/* For fabs().  Should eventually be replaced with our float version: */
#include <math.h>

#include "sgl_defs.h"
#include "sgl_math.h"
	/* Required by dlconvex.h: */
	#include "dlntypes.h"
	#include "dlmater.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "dlconvex.h"
#include "rnconvst.h"

/*
// Declare the Maximum number of edges we can have. This is
// based on the number of planes in an object. See Shadow
// document (trdd0050.doc) for details. Note that the extra
// 6 planes are for coping with unbounded objects.
//
// (ACTUALLY, THESE AREN'T BIG ENOUGH BECAUSE DURING THE PROCESSING,
// EXTRA TEMPORARY EDGES AND VERTICES ARE CREATED. HOPEFULLY WE
// SHAN'T ENCOUNTER THE SUCH A PROBLEM.)
*/
#define MAX_EDGES (3 * (SGL_MAX_INTERNAL_PLANES + 6))
#define MAX_VERTS (2 * (SGL_MAX_INTERNAL_PLANES + 6))


/*
// There are 2 forms of the adjacency information. The first is the external
// structure which is used to store information and for building shadow
// volumes. The second is the structure which is used when the adjacency 
// information is being constructed. This is only used in internally in
// this routines in this file, and is converted back to the other form
// when the operations are complete.
*/


/*
// Internal Only Form of the structure: This is the one used to
// calculate the adjacency information by gradually adding planes.
//
// It consists of a list (array) of vertices, each of which has a list
// of references to edges.
//
// There is also an array of edges, which references the vertices that
// are at the ends of the edge, and the planes making up the edge.
*/

/*
// Structure type for storing a particular vertex's list of edges
*/
typedef struct _intrn_vertex_edge_type
{
	int edge_id;
	struct _intrn_vertex_edge_type * next_edge; /*to keep linked list*/

} intrn_vertex_edge_type;

/*
// Structure type for storing vertex information
*/
typedef struct
{
	sgl_bool   used; /* is this entry in use */
	sgl_vector pt;   /* position of vertex   */

	intrn_vertex_edge_type * first_edge;  /*ptr to edge list*/

}intrn_vertex_type;

/*
// List type of all the vertices
*/
typedef intrn_vertex_type intrn_vertex_list_type[MAX_VERTS];


/*
// Structure type for storing edge information
*/
typedef struct
{
	sgl_bool used;						/* Is entry in list in use ? */

	int orig_vert1, orig_vert2;  /* real vertices at the end of the edge */

	int plane1, plane2;			  /* planes forming the edge */
} intrn_edge_type;

/*
// List of the edges
*/
typedef intrn_edge_type intrn_edge_list_type[MAX_EDGES];


/*//////////////////////////////////////////////////////////////
//
// "Global" variables
//
////////////////////////////////////////////////////////////// */

/*
// To save time continually allocating and deallocating
// vertex_edge nodes, keep a central pool of spare ones
//
// Initialise the list to be empty
*/
static intrn_vertex_edge_type * vertex_edge_node_pool = NULL;

#define MAX_SAVED_VE_NODES 30


/*//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// Vertex-edge list manipulation routines
//
// These are used by later routines to manipulate part of the 
// the adjacency information.
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// */


/*///////////////////////////////
//
// new_vertex_edge_node
//
// Creates a new vertex_edge node - user supplies edge id.
//
/////////////////////////////// */

static intrn_vertex_edge_type * new_vertex_edge_node(int edge_id)
{
	intrn_vertex_edge_type * new_node;

	/*
	// First see if there is one free in the
	// pool of vertex_edge nodes.
	*/
	if( vertex_edge_node_pool != NULL )
	{
		/*
		// Grab one off this list.
		*/
		new_node = vertex_edge_node_pool;
		vertex_edge_node_pool = vertex_edge_node_pool->next_edge;
	}
	else   
	{
		new_node = SGLMalloc ( sizeof(intrn_vertex_edge_type) ); 
		ASSERT (( new_node != NULL));
	}

	/*
	// If we have a structure, then initialise the contents.
	// (initialise the pointer for safety!)
	*/
	if(new_node != NULL)
	{
		new_node->edge_id   = edge_id;
		new_node->next_edge = NULL;    
	}
	else
	{
		SglError(sgl_err_no_mem);
	}

	return new_node;
}


/*///////////////////////////////
//
// free_vertex_edge_node
//
// Releases the vertex_edge_node (putting it onto the pool of free nodes)
// NOTE: It is assumed that the ve_node is NOT CURRENTLY PART OF ANY LIST.
//
/////////////////////////////// */

static void free_vertex_edge_node(intrn_vertex_edge_type * ve_node)
{
	/*
	// If this is a valid pointer, then add it to the list
	// of free nodes
	*/
	if(ve_node != NULL)
	{
		ve_node->next_edge = vertex_edge_node_pool;

		vertex_edge_node_pool = ve_node;
	}

}


/*///////////////////////////////
//
// delete_vertex_edge_from_list
//
// Given a pointer to a vertex's edge list, and a pointer to a node
// in that list, this cuts the vert_edge node from the list. The
// node is then "deallocated" using "free_vertex_edge_node".
//
/////////////////////////////// */
static void delete_vertex_edge_from_list(intrn_vertex_edge_type * ve_node,
												intrn_vertex_type * vertex)
{
	intrn_vertex_edge_type * predecessor;

	/*
	// If the list is empty, do nothing
	*/
	if(vertex->first_edge == NULL)
	{
		/* nothing */
	}
	/*
	//	else if the node is the first thing in the list, remove it
	*/
	else if(vertex->first_edge == ve_node)
	{
	 	vertex->first_edge = ve_node->next_edge;
	}
	/*
	// else search for the node predecessor
	*/
	else
	{
		predecessor = vertex->first_edge;

		/*
		// In the debug version, check we don't run out of list.
		// This should never happen but .....
		*/
		#ifdef _DEBUG
		while((predecessor->next_edge != ve_node)	&&
				(predecessor->next_edge != NULL))
		{
			predecessor = predecessor->next_edge;
		}
		ASSERT(predecessor != NULL)

		#else
		while(predecessor->next_edge != ve_node)
			predecessor = predecessor->next_edge;
		#endif


		/*
		// Remove the unwanted facial blemish
		*/
		predecessor->next_edge = ve_node->next_edge;

	} /*end else*/

	free_vertex_edge_node(ve_node);
}


/*///////////////////////////////
//
// append_vertex_edge_node
//
// Adds a vertex_edge node to the end of the list belonging to a particular
// vertex. (If the supplied pointer is null, it does nothing).
//
// (Because there is only a pointer to the head of the list, appending
// currently requires searching down the list to find the end.  If the lists
// are small, then this should not be a problem. Otherwise it should be
// modified.)
//
/////////////////////////////// */
static void append_vertex_edge_node(intrn_vertex_edge_type * ve_node,
												intrn_vertex_type * vertex)
{
	intrn_vertex_edge_type * current_end;

	/*
	// if we have a null pointer, do nothing
	*/
	if(ve_node == NULL)
	{
		/* nothing */
	}
	/*
	// If the list is empty, just add it.
	*/
	else if(vertex->first_edge == NULL)
	{
		vertex->first_edge = ve_node;

		ve_node->next_edge = NULL;
	}
	/*
	// else search down the list till we reach the end
	*/
	else
	{
		current_end = vertex->first_edge;

		/*
		// search for the end
		*/
		while(current_end->next_edge != NULL)
			current_end = current_end->next_edge;

		/*
		// add the new node onto the end
		*/
		current_end->next_edge = ve_node;

		ve_node->next_edge = NULL;
	}
}


/*///////////////////////////////
//
// locate_vertex_edge_node
//
// Given a vertex and an edge id, this searches down the list,
// and returns a pointer to the node with the matching edge id.
//
// If it returns NULL, then the edge wasn't found.
//
/////////////////////////////// */
static intrn_vertex_edge_type *  locate_vertex_edge_node(int edge_id,
												intrn_vertex_type * vertex)

{
	intrn_vertex_edge_type * located_ve_node;

	/*
	// start with the beginning of the list, and keep going
	// till we find it (or run out of list)
	*/
	located_ve_node = vertex->first_edge;

	while((located_ve_node != NULL) && (located_ve_node->edge_id != edge_id))
		located_ve_node = located_ve_node->next_edge;

	return(located_ve_node);
}


/*///////////////////////////////
//
// insert_nodes_before
//
// Given a vertex_edge node, 'insertion_point', that is in a list and
// another 'other_nodes', which is the first of a linked list of
// series of nodes, this inserts the other_nodes etc BEFORE the 
// insertion point.
//
/////////////////////////////// */
static void insert_nodes_before(intrn_vertex_edge_type * other_nodes,
									intrn_vertex_edge_type * insertion_point,
									intrn_vertex_type * vertex)
{
	/*
	// end of new list
	*/
	intrn_vertex_edge_type * end_of_new_nodes;
	intrn_vertex_edge_type * predecessor;

	/*
	// check weird conditions
	*/
	if( (other_nodes==NULL) || (insertion_point == NULL))
	{
		/*do nothing*/
	}
	else
	{
		/*
		// find the end of the list of new nodes
		*/
		end_of_new_nodes= other_nodes;
		while(end_of_new_nodes->next_edge != NULL)
			end_of_new_nodes = end_of_new_nodes->next_edge;

		/*
		// if the insertion point is at the start of the
		// list, put the new nodes at the start of the list.
		*/
		if(vertex->first_edge == insertion_point)
		{
			vertex->first_edge = other_nodes;
			end_of_new_nodes->next_edge = insertion_point;
		}
		/*
		// else search for the insertion point's predecessor
		*/
		else
		{
			predecessor = vertex->first_edge;
			while(predecessor->next_edge != insertion_point)
			{
				predecessor = predecessor->next_edge;
				ASSERT(predecessor != NULL)
			}

			/*
			// add the new nodes in here
			*/
			predecessor->next_edge = other_nodes;
			end_of_new_nodes->next_edge = insertion_point;

		}/*else insertion point is NOT the first*/

	}/*end else (valid data) */
}


/*///////////////////////////////
//
// insert_nodes_after
//
// Given a vertex_edge node, 'insertion_point', that is in a list and
// another 'other_nodes', which is the first of a linked list of
// series of nodes, this inserts the other nodes etc after 
// the insertion point.
//
/////////////////////////////// */
static void insert_nodes_after(intrn_vertex_edge_type * other_nodes,
									intrn_vertex_edge_type * insertion_point)
{
	/*
	// end of new list
	*/
	intrn_vertex_edge_type * end_of_new_nodes;

	/*
	// check weird conditions
	*/
	if( (other_nodes==NULL) || (insertion_point == NULL))
	{
		/*do nothing*/
	}
	else
	{
		/*
		// find the end of the list of new nodes
		*/
		end_of_new_nodes = other_nodes;
		while(end_of_new_nodes->next_edge != NULL)
		{
			end_of_new_nodes = end_of_new_nodes->next_edge;
			ASSERT(end_of_new_nodes != NULL)
		}

		/*
		// attach the nodes after the insertion point
		*/
		end_of_new_nodes->next_edge = insertion_point->next_edge;

		/*
		// put the new nodes after the insertion point
		*/
		insertion_point->next_edge = other_nodes;
	}
}


/*///////////////////////////////
//
// remove_successors
//
// Given a vertex_edge node, ve_node, this returns a pointer to the node
// (and hence subsequent nodes) that comes after ve_node, and removes
// the next_reference, so that ve_node will be at the end of the list.
//
/////////////////////////////// */
static intrn_vertex_edge_type *
			remove_successors(intrn_vertex_edge_type * ve_node)
{
	intrn_vertex_edge_type * successor;
	if(ve_node != NULL)
	{
		successor = ve_node->next_edge;
		ve_node->next_edge = NULL;
	}
	else
	{
		successor = NULL;
	}

	return(successor);
}


/*///////////////////////////////
//
// detach_list
//
// Given a vertex, vert, this returns a pointer to first vertex_edge
// node in its list (which is still attached to all the other nodes),
// and sets the vertex's list to be empty.
//
/////////////////////////////// */
static intrn_vertex_edge_type * detach_list(intrn_vertex_type * vertex)
{
	intrn_vertex_edge_type * list_start;

	list_start = vertex->first_edge;
	vertex->first_edge = NULL;

	return list_start;
}


/*//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
//
//
// Routines to actually build adjacency information
//
//
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// */

/*//////////////////////////////////////////////////////////////
//
// initialise_internal_structure
//
// This routine allocates the structures for the internal
// adjacency information, and initialises everything to be
// empty.
//
// It also checks that there is (probably) enough spare space
// for a reasonable number of vertex-edge nodes. 
//
// The routine returns TRUE if this was successful (ie sufficient
// memory) else FALSE
////////////////////////////////////////////////////////////// */
static sgl_bool initialise_internal_structure(
  intrn_vertex_list_type **vertices, intrn_edge_list_type **edges)
{
	int i;
	void * spare_mem;


 	/*
	// Allocate space for the vertices and the edges
	*/
	*vertices = SGLMalloc ( sizeof(intrn_vertex_list_type) );
	*edges    = SGLMalloc ( sizeof(intrn_edge_list_type) );

	/*
	// Check that we have enough room for some vertex_edge nodes
	*/
	spare_mem = SGLMalloc ( 20 * sizeof(intrn_vertex_edge_type) );


	/*
	// If either of these failed, then release the memory, and exit, reporting
	// an error:
	*/
	if( (*vertices == NULL) || (*edges == NULL) || (spare_mem == NULL) )
	{
		if(*vertices != NULL)
			SGLFree(*vertices);
		if(*edges != NULL)
			SGLFree(*edges);
		if(spare_mem != NULL)
			SGLFree(spare_mem);

		return FALSE;
	}

	/*
	// Release the spare memory
	*/
	SGLFree(spare_mem);


	/*
	// Success, so initialise the contents of the structures to be empty.
	*/
	for(i = 0; i < MAX_VERTS; i++)
	{
		(**vertices)[i].used = FALSE;
		(**vertices)[i].first_edge = NULL;
	}

	for(i = 0; i < MAX_EDGES; i++)
	{
		(**edges)[i].used = FALSE;

		#ifdef _DEBUG
		(**edges)[i].plane1 = -200;
		(**edges)[i].plane2 = -200;
		(**edges)[i].orig_vert1 = -200;
		(**edges)[i].orig_vert2 = -200;
		#endif
	}


	/*
	// Return in triumph
	*/
	return TRUE;
}


/*//////////////////////////////////////////////////////////////
//
// destroy_structure
//
//	Releases the memory used by the internal structure
//
////////////////////////////////////////////////////////////// */
static void destroy_structure(intrn_vertex_list_type * vertices,
								intrn_edge_list_type * edges)
{
	int i;

	intrn_vertex_edge_type * ve_node1;
	intrn_vertex_edge_type * ve_node2;
      
      
	/*
	// Release any list nodes attached to the vertices
	*/
	for(i = 0; i < MAX_VERTS; i++)
	{
        if((*vertices)[i].used)
        {     
        	ve_node1 = (*vertices)[i].first_edge;
			while(ve_node1 !=NULL)
			{
	  			ve_node2 = ve_node1;
				ve_node1 = ve_node1->next_edge;

				free_vertex_edge_node(ve_node2);
				
				
			} /*end while*/

        }/*end if*/
        
	}/*end for*/
     
    
     
	/*
	// Clear all but a few of the nodes in the main pool.
	// A considerable amount of time is spent in the memory
	// manager otherwise, so keeping a few saves some time.
	*/
	ve_node1 = vertex_edge_node_pool;
	i = 0;
	while( (ve_node1 != NULL) && (i < MAX_SAVED_VE_NODES))
	{
		ve_node1 = ve_node1->next_edge;

		i++;
	}

	/*
	// Delete the excess
	*/
	if( (i == MAX_SAVED_VE_NODES) && (ve_node1 != NULL))
	{
		/*
		// terminate the end of the list that is kept.
		*/
		ve_node2 = ve_node1->next_edge;
		ve_node1->next_edge = NULL;

		/*
		// pick through the remains
		*/
		while(ve_node2 != NULL)
		{
			/*
			// Save where we currently are, and step to the next
			// node
			*/
			ve_node1 = ve_node2;
			ve_node2 = ve_node2->next_edge;

			/*
			// delete the node where we were...
			*/
			SGLFree(ve_node1);
		}
	}

	/*
	// delete the main lists
	*/
	SGLFree(vertices);
	SGLFree(edges);
}


/*
// ----------------------------
// pBoundaryPlanes, fpBoundaryDs
// ----------------------------
// Definition of the bounding box planes. These are used to get intersections
// with real planes.
//
// Note the first plane, ie 0, is a dummy value, as to distinguish these planes
// from the real ones, a negative index will be used. 
//
// The flags value is unused.
//
// The 'd' value is set up depending on the 'max_dimension' needed for the
// bounding box region.
//
// Note also that the representative point is not needed, so won't be defined.
*/

CONV_PLANE_STRUCT pBoundaryPlanes[7] =
{
	{ 0, { 0.0f, 0.0f, 0.0f}, {0.0f,0.0f,0.0f} },
	
	{ 0, {-1.0f, 0.0f, 0.0f}, {0.0f,0.0f,0.0f} },
	{ 0, { 1.0f, 0.0f, 0.0f}, {0.0f,0.0f,0.0f} },
	
	{ 0, { 0.0f, 0.0f,-1.0f}, {0.0f,0.0f,0.0f} },
	{ 0, { 0.0f, 0.0f, 1.0f}, {0.0f,0.0f,0.0f} },
	
	{ 0, { 0.0f,-1.0f, 0.0f}, {0.0f,0.0f,0.0f} },
	{ 0, { 0.0f, 1.0f, 0.0f}, {0.0f,0.0f,0.0f} }
};

float fpBoundaryDs[7] = { 0.0f, 1.0f,1.0f, 1.0f,1.0f, 1.0f,1.0f };


/*//////////////////////////////////////////////////////////////
//
// create_initial_boundary_object
//
// The algorithm used to generate adjacency information assumes that
// objects are always bounded. To make this the case, the adjacency
// information is initialised as if a large bounding cube were part
// of the object. If at the end of processing, there are intersections
// with the 'planes' that make up the cube, then they aren't included,
// (and the object is (probably) unbounded).
//
// The sie of the bounding cube is determined by examining the
// representative points of the planes, and then choosing something
// larger.
//
// ERROR HANDLING: None at present!
// 
////////////////////////////////////////////////////////////// */
static void create_initial_boundary_object(intrn_vertex_list_type i_vertices,
  intrn_edge_list_type i_edges, CONV_PLANE_STRUCT *pPlanes, int num_planes)
{
	float max_dimension;

	/*
	// Tables that define the cube
	*/
	static sgl_vector vertices[8] = { {-1.0f, -1.0f, -1.0f},
											{ 1.0f, -1.0f, -1.0f},
											{ 1.0f, -1.0f,  1.0f},
											{-1.0f, -1.0f,  1.0f},
											{-1.0f,  1.0f, -1.0f},
											{ 1.0f,  1.0f, -1.0f},
											{ 1.0f,  1.0f,  1.0f},
											{-1.0f,  1.0f,  1.0f}};

	static int   v_e[8][3]= { { 0, 3, 4},
									  { 1, 0, 5},
									  { 2, 1, 6},
								     { 3, 2, 7},
								     { 8, 4,11},
								     { 9, 5, 8},
								     {10, 6, 9},
								     {11, 7,10}};

	/*
	// which faces make the edges: Note these are negative id's
	// so that they can be identified as being part of the massive
	// bounding box
	*/
	static int e_ps[12][2] = { {-3,-5},{-2,-5},{-4,-5},{-1,-5},
										{-1,-3},{-3,-2},{-2,-4},{-4,-1},
										{-3,-6},{-2,-6},{-4,-6},{-1,-6}};


	int i,j;

	intrn_vertex_edge_type * new_vert_edge;


	ASSERT(pPlanes != NULL);

	/*
	//examine the planes, and determine a boundary size
	*/
	max_dimension = 1.0f;
	for(i = 0; i < num_planes; i ++)
	{
		for(j = 0; j < 3; j ++)
		{
			float tmp;
			tmp = sfabs(pPlanes[i].rep_point[j]);

			if (max_dimension < tmp)
			{
				max_dimension = tmp;
			}
		}

	}
	max_dimension = max_dimension * 100.0f;



	/*
	// Initialise part of the edges array
	*/
	for(i=0; i < 12; i ++)
	{
		i_edges[i].used = TRUE;

		i_edges[i].orig_vert1 = -1;  /*don't yet know which vert to use*/
		i_edges[i].orig_vert2 = -1;

		i_edges[i].plane1 = e_ps[i][0];
		i_edges[i].plane2 = e_ps[i][1];

	} /*end for i*/


	/*
	// set the distances for the boundary planes
	*/
	for(i = 1; i < 7; i ++)
		fpBoundaryDs[i] = max_dimension;
  	
	/*
	// Initialise the vertices
	*/
	for(i = 0; i < 8; i++)
	{
		/*
		// mark vertex as in use
		*/
		i_vertices[i].used = TRUE;

		/*
		// copy the vertex coordinates
		*/
		i_vertices[i].pt[0] = vertices[i][0] * max_dimension;
		i_vertices[i].pt[1] = vertices[i][1] * max_dimension;
		i_vertices[i].pt[2] = vertices[i][2] * max_dimension;

		/*
		// Set up the list of edges belonging to the vertex
		*/
		i_vertices[i].first_edge = NULL;
		for(j = 0; j < 3; j ++)
		{
			/*
			// In non-debug mode we are not checking whether the pointer
			// returned from new_vert_edge is NULL:
			*/
			new_vert_edge = new_vertex_edge_node(v_e[i][j]);
			if (new_vert_edge == NULL)
			{
				DPF((DBG_WARNING, "Possible use of bad pointer?"));
			}

			append_vertex_edge_node(new_vert_edge, & i_vertices[i]);

			/*
			// update the vertex info for the edge (find which vertex hasn't
			// been filled in)
			*/
			if(i_edges[(v_e[i][j])].orig_vert1 < 0 )
			{
				i_edges[(v_e[i][j])].orig_vert1 = i;
			}
			else
			{
				ASSERT(i_edges[(v_e[i][j])].orig_vert2 < 0)

				i_edges[(v_e[i][j])].orig_vert2 = i;
			}
		} /*end for j*/
	} /*end for i*/
}


/*///////////////////////////////////////////////////////////
//
// Definition of arrays used to classify the vertices
//
//////////////////////////////////////////////////////////// */
typedef enum
{
	inside_plane, outside_plane, on_plane, been_processed
} vertex_classification_type;


/*//////////////////////////////////////////////////////////////
//
// merge_outside_vertices
//
// This routine is called by "include_plane_in_adj_info". It finds
// a vertex which is outside the current plane, and traverses all the
// edges leaving that vertex. 
//
//	If an edge joins an inside (or an "on the plane") vertex, it is
// left as is.
// If the edge joins another outside vertex, then the new vertex's
// edges are included in with the list from the first ...
//
// It returns an index to the merged vertex.
//
// This corresponds to section 6.2.1 of TRDD0050.doc
//
////////////////////////////////////////////////////////////// */
static int merge_outside_vertices(intrn_vertex_list_type i_vertices,
  intrn_edge_list_type i_edges,
  vertex_classification_type vertex_class[MAX_VERTS])
{
	int merged_vertex;

	int other_vertex;

	intrn_vertex_edge_type * current_edge_node;
	intrn_vertex_edge_type * matching_edge_node;
	intrn_vertex_edge_type * tmp_edge_node;
	intrn_vertex_edge_type * predecessors;
	intrn_vertex_edge_type * successors;
	int edge_id;

	/*
	// find a vertex which is outside the plane. The code
	// assumes there is one!
	//
	// mark the vertex as 'done'
	*/
	merged_vertex = 0;
	while(vertex_class[merged_vertex] != outside_plane)
		merged_vertex ++;

	vertex_class[merged_vertex] = been_processed;

	/*
	// Step through the list of edges that emanate from this vertex
	*/
	current_edge_node = i_vertices[merged_vertex].first_edge;

	while(current_edge_node != NULL)
	{
		/*
		// get id of this edge
		*/
		edge_id = current_edge_node->edge_id;

		/*
		// get the id of the vertex connected to the edge's other end.
		// AT LEAST one of the ends has been merged. If the first one
		// we pick has been merged, then try the other. (Note it may
		// also have been merged, but this will be handled in the
		// next bit of code.)
		*/
		other_vertex = i_edges[edge_id].orig_vert1;
		if(vertex_class[other_vertex] == been_processed)
			other_vertex = i_edges[edge_id].orig_vert2;
			
		ASSERT( (other_vertex >= 0) && (other_vertex < MAX_VERTS) )

		/*
		// determine what to do based on the classification of
		// the other vertex
		*/
		switch (vertex_class[other_vertex])
		{
			/*//////////////
			// if the other end is inside the object OR on the
			// border, then just move to the next edge
			////////////// */
			case inside_plane:
			case on_plane:
				current_edge_node = current_edge_node->next_edge;
			break;

			/*//////////////
			// else if the vertex at the other end has already been
			// processed, then just delete this edge.
			////////////// */
			case been_processed:
				/*
				// save where we currently are and move on, and delete
				// the old edge.
				*/
				tmp_edge_node = current_edge_node;
				current_edge_node = current_edge_node->next_edge;

				delete_vertex_edge_from_list(tmp_edge_node,
							&i_vertices[merged_vertex]);

			break;

			/*//////////////
			// Outside also (the complex case). The contents of the other
			// node are moved into the merged nodes list. The order of the
			// edges is maintained.
			////////////// */
			case outside_plane:
				/*
				// locate the matching edge in the other vertex's list
				*/
				matching_edge_node = locate_vertex_edge_node(edge_id,
											&i_vertices[other_vertex]);

				ASSERT(matching_edge_node != NULL)

				/*
				// remove the successors of this other edge (IF ANY)
				*/
				successors = remove_successors(matching_edge_node);

				/*
				// delete the matching node
				*/
				delete_vertex_edge_from_list(matching_edge_node,
											&i_vertices[other_vertex]);

				/*
				// add the remaining nodes of the other vertex (ie the ones
				// that were BEFORE "matching_edge_node" to be just
				// after current_edge_node (IF ANY)
				*/
				predecessors = detach_list(&i_vertices[other_vertex]);
				insert_nodes_after(predecessors, current_edge_node);

				/*
				// Add the successors to also be after current_edge_node
				// (and hence BEFORE the predecessors).
				*/
				insert_nodes_after(successors, current_edge_node);


				/*
				// get the Next node in the list (NOTE either of the
				// the predessor or successor list may have been empty),
				// and delete the "current_edge_node"
				*/
				tmp_edge_node = current_edge_node;
				current_edge_node = current_edge_node->next_edge;

				delete_vertex_edge_from_list(tmp_edge_node,
							&i_vertices[merged_vertex]);

				/*
				// mark the other vertex as done
				*/
				vertex_class[other_vertex] = been_processed;


			break;

			/*//////////////
			// Else ERROR!
			////////////// */
			default:
				ASSERT(FALSE);
			break;

		}/*end switch*/

	} /*end while (traversing edges) */

	/*
	// go through edges and delete all those which connected 
	// vertices that were merged together.
	*/
	for(edge_id=0; edge_id < MAX_EDGES; edge_id++)
	{
		if(i_edges[edge_id].used &&
			(vertex_class[i_edges[edge_id].orig_vert1] == been_processed) &&
			(vertex_class[i_edges[edge_id].orig_vert2] == been_processed) )
		{
		 	i_edges[edge_id].used = FALSE;
		}

	}

	return(merged_vertex);
}

/*//////////////////////////////////////////////////////////////
//
// find_planes_intersections
//
// Given three planes, this routine calculates there intersection
// point. It assumes there actually is one, and will return an
// invalid answer if there isn't.
//
// It calculates the answer by inverting a 3x3 matrix made up of
// the normals, and multiplying it by the d values.
//
// NOTE the matrix invert routine is not very efficient!
//
////////////////////////////////////////////////////////////// */
static void find_planes_intersections(
  CONV_PLANE_STRUCT *p0, CONV_PLANE_STRUCT *p1, CONV_PLANE_STRUCT *p2,
  float fD0, float fD1, float fD2, sgl_vector intersection)
{
	sgl_vector matrix[3];
	sgl_vector inverse[3];

	int i,j,k;

	int largest_row;
	float largest_val, val, inv_val;

	/*
	// in the event of a singular matrix, set the intersection
	// to be something "safe"
	*/
	intersection[0] = 0.0f;
	intersection[1] = 0.0f;
	intersection[2] = 0.0f;


	/*
	// initialise the matrix and inverse (unwinding the loops)
	*/
	matrix[0][0] = p0->normal[0];
	matrix[0][1] = p0->normal[1];
	matrix[0][2] = p0->normal[2];

	matrix[1][0] = p1->normal[0];
	matrix[1][1] = p1->normal[1];
	matrix[1][2] = p1->normal[2];

	matrix[2][0] = p2->normal[0];
	matrix[2][1] = p2->normal[1];
	matrix[2][2] = p2->normal[2];


	/*
	// Put the identity matrix in the inverse
	*/

	for(i = 0; i < 3 ; i ++)
		for(j = 0; j < 3 ; j ++)
			inverse[i][j] = 0.0f;

	for(i = 0; i < 3 ; i ++)
		inverse[i][i] = 1.0f;


	/*
	// Step through the matrix during row reduction, and
	// apply the same to the inverse: Leap out of the routine
	// in the event of failure.
	*/
	for(i=0; i < 3; i ++)
	{
	  	/*
		//  starting from row 'i', find the largest entry in column 'i'
		*/
		largest_row = i;
		largest_val = sfabs(matrix[i][i]);

		for(j = i + 1; j < 3; j++)
		{
			float tmp;

			tmp = sfabs(matrix[j][i]);
			if(largest_val < tmp )
			{
			 	largest_val = tmp;
				largest_row = j;
			}
		}

		/*
		// if the lagest found is 0, then the matrix is singular!
		*/
	  #if defined (MIDAS_ARCADE)

		/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
		if(largest_val < 1.0E-3f)
			return;

	  #else

		if(largest_val < 1.0E-20f)
			return;

	  #endif


		/*
		// else if the current row is already the largest, then
		// leave it as it is...
		*/
		else if(largest_row == i)
		{
			/*do nothing*/
		}
		/*
		// Else make sure our current row has a good sized value
		// in entry [i][i]. If they are of opposite signs, then subtract
		// one from the other, else add them. Also do the same to the
		// inverse matrix.
		*/
		else if( ((matrix[i][i] < 0.0f) && (matrix[largest_row][i] > 0.0f)) ||
			 ((matrix[i][i] > 0.0f) && (matrix[largest_row][i] < 0.0f)) )
		{
			for(j = 0; j < 3; j ++)
			{
				matrix[i][j]  = matrix[i][j]  - matrix[largest_row][j];
				inverse[i][j] = inverse[i][j] - inverse[largest_row][j];
			}
		}
		else
		{
			for(j = 0; j < 3; j ++)
			{
				matrix[i][j]  = matrix[i][j]  + matrix[largest_row][j];
				inverse[i][j] = inverse[i][j] + inverse[largest_row][j];
			}/*end for*/

		}/*end else*/


		/*
		// Set entry [i][i] to be 1 by dividing the row, because of
		// all the messing about above, we know that this division
		// won't produce bad results.
		*/
		inv_val = 1.0f / matrix[i][i];
		for(j = 0; j < 3; j++)
		{
			matrix[i][j]   = matrix[i][j] * inv_val;
			inverse[i][j]  = inverse[i][j] * inv_val;
		}

		/*
		// eliminate the column from the other rows
		*/
		for(k=0; k < 3; k++)
			if(k != i)
			{
				val = matrix[k][i];
				for(j = 0; j < 3; j ++)
				{
					matrix[k][j]   = matrix[k][j]  - val * matrix[i][j];
					inverse[k][j]  = inverse[k][j] - val * inverse[i][j];
				}
			}/*end for k*/

	}/*end for i*/

	/*
	// Multiply the inverse by the d values to get
	// the intersection point
	*/
	for(j = 0; j < 3; j ++)
	{
		intersection[j] =
		  inverse[j][0] * fD0 + inverse[j][1] * fD1 + inverse[j][2] * fD2;
	} /*end for*/
}


/*//////////////////////////////////////////////////////////////
//
//	create_new_vertices
//
// This routine is called by "include_plane_in_adj_info". It processes
// the merged vertex's list of edges, and creates new vertices where
// necessary. It also creates a list of references to vertices so that
// each edge's vertex (whether an original one or one that has been
// just created) can be found.
//
// This corresponds to section 6.2.2 of  TRDD0050.doc (trident shadow
// document)
////////////////////////////////////////////////////////////// */

static void	create_new_vertices(intrn_vertex_list_type i_vertices,
							intrn_edge_list_type i_edges,
							int merged_vertex,

							CONV_PLANE_STRUCT *pPlanes,
							CONV_PLANE_STRUCT *pPlane,
							float *fpDs, float fD,

							vertex_classification_type vertex_class[MAX_VERTS],
							int vertex_refs[MAX_VERTS])
{
	/*
	// keep track of the curent edge, its index, and the number of edges we've
	// done so far
	*/
	intrn_vertex_edge_type * current_edge_node;
	int edge_id;
	int edge_count;

	/*
	// Variable for finding free vertex indices
	*/
	int free_vertex;

	/*
	// the vertices at the ends of the edge. "this_vertex" is the
	// one that was merged, and "other_vertex" is the one that
	// wasn't
	*/
	int this_vertex;
	int other_vertex;

	/*
	// values used to calculate positions of new vertices
	*/
	CONV_PLANE_STRUCT *pPlane1,*pPlane2;
	float			  fD1,fD2;
	
	/*/////////////////////////////////////// */
	/*
	// initialisations
	*/
	edge_count = 0;
   	free_vertex = 0;
	current_edge_node = i_vertices[merged_vertex].first_edge;

	/*
	// step through the list of edges hanging off the merged vertex
	*/
	while(current_edge_node != NULL)
	{
		/*
		// get the edges id
		*/
		edge_id = current_edge_node->edge_id;

		/*
		// Get the vertices at each end of the edge. Note that
		// the other vertex is the one that HASN'T been processed 
		// (i.e. merged).
		*/
		this_vertex = i_edges[edge_id].orig_vert1;
		if(vertex_class[this_vertex] == been_processed)
		{
			other_vertex = i_edges[edge_id].orig_vert2;
		}
		else
		{
			other_vertex = this_vertex;
			this_vertex  = i_edges[edge_id].orig_vert2;
		}/*end if*/


		/*
		// Does the plane cut through the edge at the 'other_vertex'
		// or does it cut somewhere in between the two?
		*/
		if(vertex_class[other_vertex] == on_plane)
		{
		 	/*
			// Use the existing vertex and record this.
			*/
			vertex_refs[edge_count] = other_vertex;
		}

		/*
		// ELSE it cuts in between the two vertices. Must create a
		// new vertex
		*/
		else
		{
			/*
			// find a free vertex slot in the list, start searching from
			// where we last left off. This assumes that there IS a free
			// place.
			*/
			while(i_vertices[free_vertex].used)
				free_vertex++;

			ASSERT(free_vertex < MAX_VERTS);

		 	/*
			// Record which vertex we are using
			*/
			vertex_refs[edge_count] = free_vertex;



			/*
			// Calculate where the intersection will be.
			// If one (or more) of the planes is one of the original
			// bounding box's, then find the "approximate" intersection
			// by intersecting the edge with the plane.
			//
			// if there are boundary planes, then get pointers to them
			*/
			if(i_edges[edge_id].plane1 < 0)
			{
				pPlane1 = & pBoundaryPlanes[ - i_edges[edge_id].plane1];
				fD1		=   fpBoundaryDs   [ - i_edges[edge_id].plane1];
			}
			else
			{
				pPlane1 = & pPlanes[ i_edges[edge_id].plane1 ];
				fD1     =   fpDs   [ i_edges[edge_id].plane1 ];
			}

			if(i_edges[edge_id].plane2 < 0)
			{
				pPlane2 = & pBoundaryPlanes[ - i_edges[edge_id].plane2];
				fD2		=   fpBoundaryDs   [ - i_edges[edge_id].plane2];
			}
			else
			{
				pPlane2 = & pPlanes[ i_edges[edge_id].plane2 ];
				fD2     =   fpDs   [ i_edges[edge_id].plane2 ];
			}

			find_planes_intersections(pPlane1, pPlane2, pPlane, fD1,fD2,fD,
					 i_vertices[free_vertex].pt);


			/*
			// Mark this new vertex as used, and initialise its list
			// of edges
			*/
			i_vertices[free_vertex].used = TRUE;
			i_vertices[free_vertex].first_edge = NULL;

		}/*end if*/


		/*
		// step to the next edge, and increment the edge count
		*/
		current_edge_node = current_edge_node->next_edge;
		edge_count ++;

	}/*end while*/

}


/*//////////////////////////////////////////////////////////////
//
//	create_and_modify_edges
//
// This routine is called by "include_plane_in_adj_info". It processes
// the merged vertex's list of edges along with the list of new vertices,
// and generates new edges and modifies existing ones. It adds these
// new edges into the lists held by the vertices.
//
// This corresponds to section 6.2.3 of  TRDD0050.doc (trident shadow
// document)
//
// Finally, it deletes all the vertices that are now disconnected
// from the object.
////////////////////////////////////////////////////////////// */

static void	create_and_modify_edges(intrn_vertex_list_type i_vertices,

							intrn_edge_list_type i_edges,
							int merged_vertex,
	  						
							int plane_id,

							vertex_classification_type vertex_class[MAX_VERTS],
							int vertex_refs[MAX_VERTS])
{
	/*
	// keep track of the curent edge, its index, and the number of edges
	// weve done so far.
	*/
	intrn_vertex_edge_type * current_edge_node;
	int current_edge_id;

	int edge_count;

	/*
	// The non merged vertex at the other end of the edge, and
	// the intersection point of the plane and the edge.
	*/
	int current_non_merged_vertex;
	int current_intersection_vertex;


	/*
	// We also need to store the above information for the 'next'
	// edge as well.
	*/
	intrn_vertex_edge_type * next_edge_node;
	int next_edge_id;
	int next_non_merged_vertex;
	int next_intersection_vertex;


	/*
	// Variable for finding free edge indices
	*/
	int free_edge_id;

	int i;

 	/*
	// Which other planes form new edges
	*/
	int common_plane;

	/*
	// Variable for newly created vertex_edge nodes,
	// and for locating one in a vertex's list.
	*/
	intrn_vertex_edge_type * new_ve;
	intrn_vertex_edge_type * found_ve;

	/*//////////////////////////////////////////// */


	/*
	// initialise variables
	*/
	edge_count = 0;
	free_edge_id = 0;



	/*
	// Initialise the current pointer value. I assume there is one!
	*/
	current_edge_node = i_vertices[merged_vertex].first_edge;
	ASSERT(current_edge_node != NULL)

	/*
	// Initialise the 'next' pointer to also point to the first node
	// It will be advanced as the first step of the processing loop.
	// Also extract the 'next' values - these will be assigned to the
	// 'current' values during the initial part of the loop.
	*/
	next_edge_node = current_edge_node;
	next_edge_id   = next_edge_node->edge_id;

	next_intersection_vertex = vertex_refs[0];

	/*
	// find the vertex which wasn't merged
	*/
	next_non_merged_vertex = i_edges[next_edge_id].orig_vert1;
	if(vertex_class[next_non_merged_vertex] == been_processed)
			next_non_merged_vertex = i_edges[next_edge_id].orig_vert2;
	

	/*
	// Step through the edges of the merged vertex
	*/
	while(current_edge_node != NULL)
	{
		/*/////
		// Get the details for the current edge from the 
		// 'previous' next edge
		///// */
		current_edge_id = next_edge_id;
		current_intersection_vertex = next_intersection_vertex;
		current_non_merged_vertex = next_non_merged_vertex;


		/*/////
		// Advance the 'next' pointer, wrapping around to the first
		// if we go past the last edge. Extract the details of this
		// edge.
		///// */
		next_edge_node = current_edge_node->next_edge;
		if(next_edge_node == NULL)
		{
			/*
			// reset back to the start
			*/
			next_edge_node = i_vertices[merged_vertex].first_edge;
			next_intersection_vertex = vertex_refs[0];

		}
		/*
		// else use the next values. 
		*/
		else
		{
			next_intersection_vertex = vertex_refs[edge_count + 1];
		}/*end else*/

		next_edge_id = next_edge_node->edge_id;

		/*
		// find the next edge's vertex which wasn't merged
		*/
		next_non_merged_vertex = i_edges[next_edge_id].orig_vert1;
		if(vertex_class[next_non_merged_vertex] == been_processed)
			next_non_merged_vertex = i_edges[next_edge_id].orig_vert2;



		/*/////////////////////////
		//
		// Two cases to do. Either the edge is intersected midway, or
		// it has been intersected at the "non merged vertex".
		//
		// If "midway" along the edge....
		//
		///////////////////////// */
		if(vertex_class[current_non_merged_vertex] == inside_plane)
		{
			ASSERT(current_non_merged_vertex != current_intersection_vertex)

			/*
			// modify the current edge data: This edge is bounded by 
			// "current_intersection_vertex" and "current_non_merged_vertex"
			*/
			i_edges[current_edge_id].orig_vert1 = current_intersection_vertex;
			i_edges[current_edge_id].orig_vert2 = current_non_merged_vertex;

			/*
			// NOTE: When we eventually reach the last edge, and we access the
			// first edge again as the "next" one, care must be taken. We may
			// have just modified the 'first' edge's vertex information, and
			// we don't want this to cause problems. In particular, the 
			// "non_merged_vertex" index may have changed, and we use this to
			// determine how join up the edges. The problem may arise when we
			// access the vertex_class - so make sure we store the same details
			// in that array as for the original vertex.
			*/
			vertex_class[current_intersection_vertex] = inside_plane;

			/*
			// Append the edge to the list belonging to the
			// current intersection vertex
			*/
			new_ve = new_vertex_edge_node(current_edge_id);
			append_vertex_edge_node(new_ve,
					& i_vertices[current_intersection_vertex]);

			/*
			// A new edge must be created, which will join the current
			// intersection vertex with the next edges intersection vertex.
			// Find a free spot in the edge table for this edge.
			*/
			while(i_edges[free_edge_id].used)
				free_edge_id ++;
			ASSERT(free_edge_id < MAX_EDGES)


			/*
			// Apart from the new plane, find the other plane which makes
			// up this new edge. It will be the one which is in common with
			// the current edge and the next edge.
			*/
			common_plane = i_edges[current_edge_id].plane1;
			if((common_plane == i_edges[next_edge_id].plane1) ||
				(common_plane == i_edges[next_edge_id].plane2) )
			{
				ASSERT(common_plane != i_edges[current_edge_id].plane2)
			}
			else
			{
			 	common_plane = i_edges[current_edge_id].plane2;
				ASSERT((common_plane == i_edges[next_edge_id].plane1) ||
						 (common_plane == i_edges[next_edge_id].plane2) )
			}

			/*
			// Save the new edge's details
			*/
			i_edges[free_edge_id].used = TRUE;
			i_edges[free_edge_id].orig_vert1 = current_intersection_vertex;
			i_edges[free_edge_id].orig_vert2 = next_intersection_vertex;
			i_edges[free_edge_id].plane1     = plane_id;
			i_edges[free_edge_id].plane2	 = common_plane;


			/*
			// Append the new edge to the list for the current intersection
			*/
			new_ve = new_vertex_edge_node(free_edge_id);
			append_vertex_edge_node(new_ve,
					& i_vertices[current_intersection_vertex]);

			/*
			// The new edge must also be included in the list for
			// the next_intersection_vertex. There are 2 cases to
			// consider.
			//
			// If the next intersection also occurs 'midway' then the new
			// edge can be simply appended onto its list.
			*/
			if(vertex_class[next_non_merged_vertex] == inside_plane)
			{
				new_ve = new_vertex_edge_node(free_edge_id);
				append_vertex_edge_node(new_ve,
						& i_vertices[next_intersection_vertex]);
			}
			/*
			// ELSE the intersection occurs at an existing vertex. The
			// new edge must be inserted into the list just AFTER the
			// the next edge, in order to maintain correct order.
			*/
			else
			{
				ASSERT(vertex_class[next_non_merged_vertex] == on_plane)

				/*
				// create a new vertex_edge list node
				*/
				new_ve = new_vertex_edge_node(free_edge_id);
				
				/*
				// find where the next edge is in the next
				// vertex's list, and add the new edge node after it.
				*/
				found_ve = locate_vertex_edge_node(next_edge_id,
								 &i_vertices[next_intersection_vertex]);
				insert_nodes_after(new_ve, found_ve);
		
			}/*end if */
	
		}
		/*///////////////////////////
		// else it intersected through the vertex. This means that the
		// current edge will no longer be part of the object, and will
		// have to be deleted later. (We can't delete it immediately,
		// because  if this is the first edge we are doing, then it
		// it is required by the last edge).
		/////////////////////////// */
		else
		{
			ASSERT(current_non_merged_vertex == current_intersection_vertex)
			
       		/*
			// Find which plane was in common to this edge and the next: ie the
			// one currently forming the face.
			*/
			common_plane = i_edges[current_edge_id].plane1;
			if((common_plane == i_edges[next_edge_id].plane1) ||
				(common_plane == i_edges[next_edge_id].plane2) )
			{
				ASSERT(common_plane != i_edges[current_edge_id].plane2)
			}
			else
			{
				common_plane = i_edges[current_edge_id].plane2;
				ASSERT((common_plane == i_edges[next_edge_id].plane1) ||
					 	(common_plane == i_edges[next_edge_id].plane2) )
			} /*end else*/
 
			/*
			// Once again there are 2 cases to consider.
			// If the next vertex has occured midway along the next
			// edge, then we create a new edge.
			*/
			if(vertex_class[next_non_merged_vertex] == inside_plane)
			{
			 	/*
				// Create a new edge
				*/
				while(i_edges[free_edge_id].used)
					free_edge_id ++;
				ASSERT(free_edge_id < MAX_EDGES)
				
				/*
				// Save the new edge's details
				*/
				i_edges[free_edge_id].used = TRUE;
				i_edges[free_edge_id].orig_vert1 = current_intersection_vertex;
				i_edges[free_edge_id].orig_vert2 = next_intersection_vertex;
				i_edges[free_edge_id].plane1     = plane_id;
				i_edges[free_edge_id].plane2	 = common_plane;

				/*
				// Add the new edge to the list for the current vertex. It
				// must be inserted BEFORE the current edge in the 
				// vertex_edge list.
				*/
				new_ve = new_vertex_edge_node(free_edge_id);
				
				found_ve = locate_vertex_edge_node(current_edge_id,
								 &i_vertices[current_intersection_vertex]);

				insert_nodes_before(new_ve, found_ve,
						&i_vertices[current_intersection_vertex]);

				/*
				// Simply append the edge to the next vertex's list
				*/
				new_ve = new_vertex_edge_node(free_edge_id);
				append_vertex_edge_node(new_ve,
						& i_vertices[next_intersection_vertex]);

			}
			/*
			// Else the subsequent intersection is also at an existing
			// vertex. There are 2 cases to consider here. If the two vertices
			// are already connected, then the existing edge must be modified
			// to contain the new plane information.
			//
			// If they are not joined, then a new edge must be created to
			// connect them together.
			*/
			else
			{
				ASSERT(vertex_class[next_non_merged_vertex] == on_plane)
                 
				/*
				// Find if there is an edge which already joining
				// "current_non_merged_vertex" and "next_non_merged_vertex".
				// Step through each node in the current vertex's list, 
				// and see if it is in the next's.
				*/
				found_ve = i_vertices[current_non_merged_vertex].first_edge;

				while((found_ve !=NULL) && locate_vertex_edge_node(found_ve->edge_id,
							& i_vertices[next_non_merged_vertex]) == NULL)
				{
					/*
					// there was no match with this edge in the first list
					// so try the next one
					*/
				 	found_ve = found_ve->next_edge;

				} /*end while */
                
                
                /*
                // If no connecting edge was found, then create a new edge
                */
                if(found_ve == NULL)
                {   
                 	/*
					// Create a new edge
					*/
					while(i_edges[free_edge_id].used)
						free_edge_id ++;
					ASSERT(free_edge_id < MAX_EDGES)
				    
				    /*
				    // Fill in the details
					*/
					i_edges[free_edge_id].used = TRUE;
					i_edges[free_edge_id].orig_vert1= current_intersection_vertex;
					i_edges[free_edge_id].orig_vert2= next_intersection_vertex;
					i_edges[free_edge_id].plane1    = plane_id;
					i_edges[free_edge_id].plane2	= common_plane;
	              
	              	/*
	              	// Add the new edge to the list for the current vertex.
	              	// It is added BEFORE the current edge.
	              	*/
	              	new_ve   = new_vertex_edge_node(free_edge_id);
	              	found_ve = locate_vertex_edge_node(current_edge_id, 
	              							&i_vertices[current_intersection_vertex]);
	              	insert_nodes_before(new_ve, found_ve,	
	              							&i_vertices[current_intersection_vertex]);
	              							
	              	/*
	              	// Add the new plane to the next vertex. It is added AFTER the
	              	// next edge
	              	*/
	              	new_ve   = new_vertex_edge_node(free_edge_id);
	              	found_ve = locate_vertex_edge_node(next_edge_id,
	              							&i_vertices[next_intersection_vertex]);
	              	insert_nodes_after(new_ve, found_ve);	              						
                }
                /*
               	// else modify the data in the existing edge.
               	*/
                else
                { 
                     /*
					// Alter the plane details in the edge linking the
					// the two vertices. The common plane is replaced with
					// the new cutting plane.
					*/
					if(i_edges[ found_ve->edge_id ].plane1 == common_plane)
					{
						i_edges[ found_ve->edge_id ].plane1 = plane_id;
					}
					else
					{
						ASSERT(i_edges[ found_ve->edge_id ].plane2 == common_plane)
						i_edges[ found_ve->edge_id ].plane2 = plane_id;
	
					} /*end else*/

                } /*end else (connecting edge already exists)*/
                
                


			}/*end else*/

		}/*end if else*/



		/*
		// increment the edge count, and advance the pointer
		*/
		edge_count ++;
		current_edge_node = current_edge_node->next_edge;

	}/*end while*/


	/*
	// Go through deleting unneeded edges etc
	*/
	current_edge_node = i_vertices[merged_vertex].first_edge;
	while(current_edge_node != NULL)
	{
		/*
		// extract the details as before
		*/
		current_edge_id = current_edge_node->edge_id;

		/*
		// find the edge's vertex which wasn't merged
		*/
		current_non_merged_vertex = i_edges[current_edge_id].orig_vert1;
		if(vertex_class[current_non_merged_vertex] == been_processed)
			current_non_merged_vertex = i_edges[current_edge_id].orig_vert2;

		/*
		// if this edge is no longer part of the object, then
		// delete its entry in the main table.
		// Also, it will no longer be connected to the non merged
		// vertex.
		*/
		if( vertex_class[current_non_merged_vertex] == on_plane )
		{
			/*
			// delete the edge from the main table
			*/
			i_edges[current_edge_id].used = FALSE;

			/*
			// IN DEBUG MODE: Put in Dud values in the edge table
			*/
			#ifdef _DEBUG
			i_edges[current_edge_id].plane1 = -100;
			i_edges[current_edge_id].plane2 = -100;
			i_edges[current_edge_id].orig_vert1 = -100;
			i_edges[current_edge_id].orig_vert2 = -100;
			#endif

			/*
			// delete the vertex_edge list entry from the 
			// NON merged vertex.
			*/
			found_ve = locate_vertex_edge_node(current_edge_id,
								 &i_vertices[current_non_merged_vertex]);

			delete_vertex_edge_from_list(found_ve,
								 &i_vertices[current_non_merged_vertex]);
		} /*end if*/

		/*
		// move to the next edge
		*/
		next_edge_node = current_edge_node->next_edge;

		/*
		// delete the list entry from the merged vertex
		*/
		delete_vertex_edge_from_list(current_edge_node,
								 &i_vertices[merged_vertex]);

		current_edge_node = next_edge_node;

	}/*end while*/

	/*
	// Finally delete all the vertices that were merged together.
	*/
	for(i=0; i < MAX_VERTS; i++)
	{
		if(i_vertices[i].used && (vertex_class[i] == been_processed))
			i_vertices[i].used = FALSE;
	}
}


/*//////////////////////////////////////////////////////////////
//
//	include_plane_in_adj_info
//
// Given the adjacency information generated for planes 0 to
// n-1, this adds plane 'n's effect to the adjacency info.
//
// If the plane causes the object to be empty, the routine
// returns FALSE, otherwise TRUE.
//
////////////////////////////////////////////////////////////// */

static sgl_bool include_plane_in_adj_info(intrn_vertex_list_type i_vertices,
  intrn_edge_list_type i_edges, int new_plane_id, CONV_PLANE_STRUCT *pPlanes,
  CONV_PLANE_STRUCT *pPlane, float *fpDs, float fD)
{
	/*//////////////////////////////////////////////////////// */

	static float vertex_dot_prod[MAX_VERTS];
	static vertex_classification_type vertex_class[MAX_VERTS];
	static int vertex_refs[MAX_VERTS];

	int i;

	/*
	// Stats on where this plane goes..
	*/
	int num_strictly_inside;
	int num_strictly_outside;

	int merged_vertex;

	/*
	//  Return value
	*/
	sgl_bool object_not_null;

	/*//////////////////////////////////////////////////////// */

	num_strictly_inside = 0;
	num_strictly_outside= 0;
	object_not_null = TRUE;
	/*
	// Step through the vertices, seeing where they are in relation
	// to the new plane
	*/
	for(i = 0; i < MAX_VERTS; i ++)
	{
		if(i_vertices[i].used)
		{
			/*
			// As this vertex slot is used, get the dot product
			*/
			vertex_dot_prod[i] = DotProd(i_vertices[i].pt, pPlane->normal);

			/*
			// Where is the vertex in relation to the plane ? Allow some
			// tolerance to class some points as "on the plane".
			*/
			if(vertex_dot_prod[i] < (fD - fabs(fD * 0.0001f)) )
			{
			 	vertex_class[i] = inside_plane;
				num_strictly_inside ++;
			} 
			else if(vertex_dot_prod[i] > (fD + fabs(fD * 0.0001f)) )
			{
			 	vertex_class[i] = outside_plane;
				num_strictly_outside ++;
			}
			else
			{
			 	vertex_class[i] = on_plane;
			}
		}/*end if*/
	}/*end for*/


	/*
	// Determine two special cases.
	// 1) Either the plane has no effect on the object
	// 2) The plane causes the object to be NULL.
	//
	*/
	if(num_strictly_inside == 0)
	{
		/*
		// The object is empty
		*/
		object_not_null = FALSE;
	}
	else if(num_strictly_outside == 0)
	{
		/*
		// There is no point in doing this plane: It has no
		// additional effect
		*/
	}
	/*
	// Else process the plane
	*/
	else
	{
		/*
		// first pick an outside vertex, and merge all other
		// outside vertices with it.
		*/
		merged_vertex =
		  merge_outside_vertices(i_vertices, i_edges, vertex_class);

		/*
		// create list of references to vertices- one vertex for each edge
		// connected to the merged vertex
		*/
		create_new_vertices(i_vertices, i_edges, merged_vertex,
		  pPlanes,pPlane, fpDs,fD, vertex_class, vertex_refs);

		/*
		// create and modify the edges
		*/
		create_and_modify_edges(i_vertices, i_edges, merged_vertex,
				new_plane_id, vertex_class, vertex_refs);

		
	}/*end if classifying plane vs object so far*/


	/*
	// return the result
	*/
	return (object_not_null);
}


/******************************************************************************
 * Function Name: ConvertInternalToExternal
 *
 * Inputs       : pConvexNode->edge_info, pConvexNode->u16_flags,
 *				  i_vertices, i_edges, num_planes, non_null_object
 *
 * Outputs      : pConvexNode->edge_info, pConvexNode->u16_flags
 *
 * Returns      : error status: sgl_no_err or sgl_err_no_mem
 *
 * Globals Used : -
 *
 * Description  : Converts the internal representation to the external
 *				  representation.
 *****************************************************************************/
static int ConvertInternalToExternal(CONVEX_NODE_STRUCT *pConvexNode,
  intrn_vertex_list_type i_vertices, intrn_edge_list_type i_edges,
  sgl_bool non_null_object)
{
	/*
	// Arrays for remapping vertex and edge ids to a compact form
	*/
	static int edge_map[MAX_EDGES];

	/*
	// how many vertices, edges and vertex_edge pairs there are
	*/
	int num_vertices;
	int num_edges;
	int edge_size;

	/*
	// Pointer into the external table of edges:
	*/
	EDGE_DATA_STRUCT *pThisEdge;

	/*
	// Pointers into the INTERNAL table of vertices
	*/
	intrn_vertex_type * p_vert1;
	intrn_vertex_type * p_vert2;

	EDGE_DATA_STRUCT *edgs_ptr;

	int i;
	intrn_vertex_edge_type * ve_node;

	sgl_bool object_bounded;


	ASSERT(pConvexNode != NULL);

	/*
	// if the object is worthwhile processing
	*/
	if(non_null_object)
	{
		object_bounded = TRUE;

		/*
		// count the edges. NOTE any edge which is formed with a plane
		// belonging to the original "LARGE" bounding box is not included,
		// and the object is therefore unbounded.
		*/
		num_edges = 0;
		for(i = 0; i < MAX_EDGES; i++)
		{
			if(i_edges[i].used)
			{
				if( (i_edges[i].plane1 >= 0) && (i_edges[i].plane2 >= 0))
				{
					/*
					// set up the mapping for the edges
					*/
					edge_map[i] = num_edges;

					/*
					// increment the edge count
					*/
					num_edges++;
				}
				else
				{
					object_bounded = FALSE;
				}
			}/*end if*/
		}/*end for*/
		/*
		// count the number of vertices, and the total number of 
		// vertex/edge nodes.
		//
		// At the same time, get the mapping that will allow us
		// to renumber the vertices in a compact way.
		*/
		num_vertices = 0;
		edge_size = 0;

		for(i = 0; i < MAX_VERTS; i++)
		{
			/*
			// if this spot is used
			*/
			if(i_vertices[i].used)
			{
				/*
				// count how many edges are attached to this vertex, and
				// add this to the running total. (Theoretically with bounded 
				// objects, this can be computed from Eulers formula, but I'll 
				// use it for error checking).
				//
				// Note that some of these may have been formed with the
				// one of the large boundary planes (if the object
				// is unbounded).
				*/
				ve_node = i_vertices[i].first_edge;
				while(ve_node != NULL)
				{
					edge_size ++;
					ve_node = ve_node->next_edge;
				}

				/*
				// increment the vertex count
				*/
				num_vertices ++;
			} /*end if*/
		} /*end for*/


		/*
		// in debug mode: CHECK Bounded objects seem sensible.
		*/
		if (object_bounded)
		{
			ASSERT(edge_size == (2 * num_edges));
		}

		/*
		// ============================
		// ALLOCATE EDGE INFO STRUCTURE
		// ============================
		*/
		if (pConvexNode->edge_info != NULL)
		{
			SGLFree(pConvexNode->edge_info);
		}
		ASSERT(num_edges >= 0);
		if (num_edges == 0)  /* If we have a single plane... */
		{
			pConvexNode->edge_info = NULL;
		}
		else
		{
			pConvexNode->edge_info = SGLMalloc((size_t)( (long)sizeof(int) +
			  (long)num_edges * sizeof(EDGE_DATA_STRUCT) ));
			if (pConvexNode->edge_info == NULL)
			{
				ASSERT(( pConvexNode->edge_info != NULL));
				return sgl_err_no_mem;
			}

			/*
			// Get access to the various parts of the structure. This uses 
			// pointer arithmetic to add multiples of structure sizes. 
			*/
			edgs_ptr = pConvexNode->edge_info->edges;
		}

		/*
		// ============
		// BOUNDING BOX
		// ============
		*/
		if (object_bounded)
		{
			sgl_bool    bDoneFirst = FALSE;
			sgl_vector  minCorner,maxCorner;
			BBOX_CENT_STRUCT *pBox = &pConvexNode->bbox;

			/* A single plane should not have a bounding box. */
			ASSERT(pConvexNode->edge_info != NULL);

			/* The object has a bounding box: */
			pConvexNode->u16_flags |= cf_has_bbox;

			for (i = 0; i < MAX_VERTS; i++)
			{
				if (i_vertices[i].used)  /* If the vertex is used... */
				{
					if (!bDoneFirst)
					{
						minCorner[0] = maxCorner[0] = i_vertices[i].pt[0];
						minCorner[1] = maxCorner[1] = i_vertices[i].pt[1];
						minCorner[2] = maxCorner[2] = i_vertices[i].pt[2];
						bDoneFirst = TRUE;
					}
					else
					{
						/* OPTIMISATION: ASSIGN ONLY WHEN CHANGES REQUIRED? */
						minCorner[0] = MIN(minCorner[0], i_vertices[i].pt[0]);
						minCorner[1] = MIN(minCorner[1], i_vertices[i].pt[1]);
						minCorner[2] = MIN(minCorner[2], i_vertices[i].pt[2]);

						maxCorner[0] = MAX(maxCorner[0], i_vertices[i].pt[0]);
						maxCorner[1] = MAX(maxCorner[1], i_vertices[i].pt[1]);
						maxCorner[2] = MAX(maxCorner[2], i_vertices[i].pt[2]);
					}
				}
			}
			ASSERT(bDoneFirst);

			pBox->boxCentre[0] = (minCorner[0]+maxCorner[0]) * 0.5f;
			pBox->boxCentre[1] = (minCorner[1]+maxCorner[1]) * 0.5f;
			pBox->boxCentre[2] = (minCorner[2]+maxCorner[2]) * 0.5f;

			pBox->boxOffsets[0] = maxCorner[0] - pBox->boxCentre[0];
			pBox->boxOffsets[1] = maxCorner[1] - pBox->boxCentre[1];
			pBox->boxOffsets[2] = maxCorner[2] - pBox->boxCentre[2];
		}
		else
		{
			/* Does not have bounding box: */
			pConvexNode->u16_flags &= ~cf_has_bbox;
		}

		/*
		// ===============
		// STORE EDGE INFO
		// ===============
		*/
		/* If we have more than a single plane... */
		if (pConvexNode->edge_info != NULL)
		{
			pConvexNode->edge_info->num_edges = num_edges;
			for(i = 0; i < MAX_EDGES; i ++)
			{
				/*
				// if edge is used, and is not one formed by the large
				// bounding cube, then add it
				*/
				if(i_edges[i].used &&
					(i_edges[i].plane1 >= 0) && (i_edges[i].plane2 >= 0))
				{
					/*
					// get a pointer to the external edge, allowing for the
					// the mapping compacting everything down.
					*/
					#if 0
					/* RGL VERSION: */
					pThisEdge = & (*edgs_ptr)[ edge_map[i] ];
					#endif
					pThisEdge = edgs_ptr + edge_map[i];

					/*
					// Get pointers to the internal vertices at the ends of the
					// edge
					*/
					p_vert1 = & i_vertices[ i_edges[i].orig_vert1 ];
					p_vert2 = & i_vertices[ i_edges[i].orig_vert2 ];


					/*
					// fill in the plane data for this edge
					*/
					pThisEdge->u16_plane1 = i_edges[i].plane1;
					pThisEdge->u16_plane2 = i_edges[i].plane2;


					/*
					// calculate the edge point to be midway between two
					// vertices
					*/
					pThisEdge->mid_point[0] =
						 (p_vert1->pt[0] + p_vert2->pt[0]) * 0.5f;
					pThisEdge->mid_point[1] =
						 (p_vert1->pt[1] + p_vert2->pt[1]) * 0.5f;
					pThisEdge->mid_point[2] =
						 (p_vert1->pt[2] + p_vert2->pt[2]) * 0.5f;

				} /*end if*/
			}
		}
	}  /* end first part of if (non null object) */
	else
	{
		/*
		// ===================
		// NO EDGE INFORMATION
		// ===================
		*/
		if (pConvexNode->edge_info != NULL)
		{
			SGLFree(pConvexNode->edge_info);
			pConvexNode->edge_info = NULL;
		}
		pConvexNode->u16_flags &= ~cf_has_bbox; /*There is no bounding box.*/
	}

	return sgl_no_err;
}


/*
// ============================================================================
// 							  SGL INTERNAL ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: GenerateAdjacencyInfo
 *
 * Inputs       : pConvexNode->edge_info, pConvexNode->plane_data,
 *				  pConvexNode->u16_num_planes, pConvexNode->u16_flags
 *
 * Outputs      : pConvexNode->edge_info, pConvexNode->u16_flags
 *
 * Returns      : error status: sgl_no_err or sgl_err_no_mem
 *
 * Globals Used : -
 *
 * Description  : See header file.
 *****************************************************************************/
int GenerateAdjacencyInfo(CONVEX_NODE_STRUCT *pConvexNode)
{
	int	     nNumPlanes,nPlane, nError;
	float    *fpDs;
	sgl_bool bObjectHasVolume;

	CONV_PLANE_STRUCT	   *pPlanes;
	intrn_vertex_list_type *pInternalVertices;
	intrn_edge_list_type   *pInternalEdges;

	ASSERT(pConvexNode != NULL);
	nNumPlanes = pConvexNode->u16_num_planes;
	pPlanes    = pConvexNode->plane_data;
	ASSERT(pPlanes != NULL);

	/*
	// Make sure there is no edge information if there are no planes.
	*/
	if (nNumPlanes == 0)
	{
		SGLFree(pConvexNode->edge_info);
		return sgl_no_err;
	}

	/*
	// ==================
	// CALCULATE D VALUES
	// ==================
	*/
	fpDs = SGLMalloc(nNumPlanes * sizeof(float));
	if (fpDs == NULL)
	{
		ASSERT((fpDs != NULL ));
		return sgl_err_no_mem;
	}
	for (nPlane=0; nPlane < nNumPlanes; nPlane++)
		fpDs[nPlane] =
		  DotProd(pPlanes[nPlane].normal, pPlanes[nPlane].rep_point);

	/*
	// ===========================
	// INITIALISE VERTEX/EDGE DATA
	// ===========================
	*/
	if (!initialise_internal_structure(&pInternalVertices, &pInternalEdges))
	{
		SGLFree(fpDs);
		return sgl_err_no_mem;
	}

	/* Initialise the internal structure to contain a large bounding box: */
	create_initial_boundary_object(*pInternalVertices,*pInternalEdges,
	  pPlanes, nNumPlanes);

	/*
	// ===========================
	// BUILD ADJACENCY INFORMATION
	// ===========================
	*/
	bObjectHasVolume = TRUE;

	for (nPlane = 0; nPlane < nNumPlanes; nPlane++)
	{
		bObjectHasVolume = include_plane_in_adj_info(*pInternalVertices,
		  *pInternalEdges,nPlane, pPlanes,&pPlanes[nPlane], fpDs,fpDs[nPlane]);

		/* Do not bother doing any more if the object is empty: */
		if (!bObjectHasVolume)
		{
			break;
		}
	}

	/*
	// =============================
	// CONVERT ADJACENCY INFORMATION
	// =============================
	*/
	nError = ConvertInternalToExternal(pConvexNode, *pInternalVertices,
	  *pInternalEdges, bObjectHasVolume);

	/*
	// ===================
	// CLEAN UP AND RETURN
	// ===================
	*/

	/* Release the internal structure: */
	destroy_structure(pInternalVertices, pInternalEdges);

	SGLFree(fpDs);
	return nError;
}


/*------------------------------- End of File -------------------------------*/
