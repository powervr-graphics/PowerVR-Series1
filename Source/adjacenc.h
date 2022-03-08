/******************************************************************************
 * Name			: adjacenc.h
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
 * $Log: adjacenc.h,v $
 * Revision 1.2  1995/06/30  12:52:29  jrc
 * Changed #include "dlconvex.h" to #include "dlnodes.h".
 *
 * Revision 1.1  1995/06/30  12:17:20  jrc
 * Initial revision
 *
 *
 * RGL HISTORY:
 *
 * Revision 1.1  93/09/03  11:02:24  SimonF
 * Initial revision
 * 
 *  		10th June 1993		Initial Version		Simon Fenney
 *
 *****************************************************************************/
#ifndef __ADJACENC_H
#define __ADJACENC_H


#include "dlnodes.h"


/* Old RGL adjacency stuff that hopefully we do not need any more: */
#if 0
	/*
	// Declare the 'header' of the packed adjacency structure. Since
	// this is packed, the outside world will only have access to the
	// header. The interface routines must be used to manipulate it.
	*/
	typedef struct
	{
	 	int  num_object_planes;
		int  num_edges;
		int  num_vertices;
		BOOL closed_structure; 

	} adjacency_hdr_type;


	/*
	// ROUTINES: The following are the interface routines used to build/
	// manipulate the adjacency information structure.
	//
	//
	// specify_adjacency_structure:
	//		Given the number of planes, edges, and vertices, along with the
	// arrays of data (see above) this allocates and builds a compact structure
	// to store the data, and returns a pointer to it.
	//
	// 		If it returns a Null pointer, then something went wrong!
	*/

	adjacency_hdr_type * specify_adjacency_structure(int num_planes,
				int num_edges,
				int num_vertices,

				edge_array_type 		edges,
				vertex_array_type		vertices,
				vertices_edges_type  vertices_edges);


	/*
	// extract_adjacency_edges:
	//			Examines the packed adjacency information and returns a pointer
	// INTO THAT STRUCTURE to a "short" edge_array_type. Note that this array
	// only has as many entries as there are edges specified in the header.
	*/
	edge_array_type * extract_adjacency_edges(adjacency_hdr_type * adj_info);
#endif


/* Code for creating adjacency information from scratch: */
#if 0
	/* Old RGL version: */

	typedef struct
	{
		vector normal;
		double d;  /* This is the dot product of 'normal' and 'rep_point'. */
		vector rep_point;
	} plane_eqn_type;

	adjacency_hdr_type *generate_adjacency_info(
	  adjacency_hdr_type *old_adj_info,
	  plane_eqn_type planes[MAX_OBJECT_PLANES], int num_planes);
#endif

/*
// ---------------------
// GenerateAdjacencyInfo
// ---------------------
// Generates adjacency information from the set of planes that comprise the
// object.
//
// Possible return values are sgl_no_err and sgl_err_no_mem.
*/
int GenerateAdjacencyInfo(CONVEX_NODE_STRUCT *pConvexNode);


#endif
/*------------------------------- End of File -------------------------------*/
