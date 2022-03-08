/*****************************************************************************
 *;++
 * Name	   : $RCSfile: dlmesh.h,v $
 * Title	  : DLMESH.H
 * C Author       : Jim Page
 * Created	: 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :     Structs, defines and prototypes for SGL Mesh functions
 *		   
 * Program Type   :     C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1997/05/02 07:45:14 $	
 * $Revision: 1.31 $
 * $Locker:  $
 * $Log: dlmesh.h,v $
 * Revision 1.31  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.30  1996/11/08  14:14:05  gdc
 * changes to get mesh editing functions working again
 *
 * Revision 1.29  1996/11/05  14:50:32  gdc
 * moved TRANSEDGE and TRANSVERTEX struct defs from dlmesh.h to rnmesh.h
 *
 * Revision 1.28  1996/05/01  14:00:16  sjf
 * Fixed bug I introduced when deleting "unused" local material entries.
 *
 * Revision 1.27  1996/04/24  18:00:52  sjf
 * Expanded face structure so that it is a mulitple of 4 bytes in size.
 *
 * Revision 1.26  1996/04/23  16:45:14  sjf
 * 1)re-arranged some fields in mesh structure so that they more
 *   reflect there order of use. (This may help caching)
 * 2) Anonymous meshes now delete the DL vertex and DL edge stuff since
 *    they arent needed by rendering, and it should reduce the overall
 *    memory usage.
 *
 * Revision 1.25  1996/04/20  16:17:31  jop
 * Refined edge and vertex structures further
 *
 * Revision 1.24  1996/04/20  13:16:27  sjf
 * Killed off Inverse Z from transformed points structure, and
 * the clip flags from the transformed edges.
 *
 * Revision 1.23  1996/04/19  18:21:49  jop
 * Chopped VERTEX struct into 2 bits
 *
 * Revision 1.22  1996/04/11  14:20:16  jop
 * Added points data array to mesh structure
 *
 * Revision 1.21  1996/04/03  17:39:15  jop
 * Changed some member sizes
 *
 * Revision 1.20  1996/04/02  12:32:57  sjf
 * many changes for optimistation
 *
 * Revision 1.19  1996/03/25  19:35:29  sjf
 * Changed bounding box to the standard variety.
 *
 * Revision 1.18  1996/03/25  16:38:49  jop
 * Optimisations
 *
 * Revision 1.17  1995/11/09  18:19:36  jop
 * New transformed vertex and edge structures
 *
 * Revision 1.16  1995/11/02  19:48:21  sjf
 * Added user defineable culling.
 *
 * Revision 1.15  1995/10/28  23:17:07  jop
 * Added some more to the mesh node
 *
 * Revision 1.14  1995/09/12  18:37:16  jop
 * Changed Flags and Regions to INT16 - needed more space, and makes
 * writes more efficient.
 *
 * Revision 1.13  1995/08/14  12:50:46  jop
 * Changed PEDGE structure to use pre-packed parameters
 *
 * Revision 1.12  1995/08/14  12:07:50  jop
 * removed and added some data members
 *
 * Revision 1.11  1995/08/02  20:34:44  jop
 * Added/removed various items from structures (rnmesh stuff)
 *
 * Revision 1.10  1995/07/27  18:44:23  jop
 * Added EdgeFlags to face struct
 *
 * Revision 1.9  1995/07/25  16:39:18  jop
 * removed region data from face - not needed
 *
 * Revision 1.8  1995/07/25  12:29:22  jop
 * Put BW_ flags into file
 *
 * Revision 1.7  1995/07/23  17:11:52  jop
 * removed adjacency typedefs
 *
 * Revision 1.6  1995/07/15  20:10:54  jop
 * Added adjacency stuff back in
 *
 * Revision 1.5  1995/07/15  18:35:46  jop
 * Added clipping flags and rendering stuff to mesh structures
 *
 * Revision 1.4  1995/07/05  17:03:52  jrc
 * Added prototype for DlDeleteMeshNodeRefs.
 *
 * Revision 1.3  1995/06/05  15:41:05  jop
 * Removed adjacency stuff
 *
 * Revision 1.2  1995/06/05  15:27:44  jop
 * Completed coding and added adjacency checking
 *
 * Revision 1.1  1995/06/02  20:09:13  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

/* get list.h if not already got */

#ifndef __LIST__
#include "list.h"
#endif

#ifndef __ARRAY__
#include "array.h"
#endif

/* Vertex structure */

#define BW_VISIBLE		      0x0400

#define CLIPPED_MINUS_X	 0x01
#define CLIPPED_PLUS_X	  0x02
#define CLIPPED_MINUS_Y	 0x04
#define CLIPPED_PLUS_Y	  0x08
#define CLIPPED_Z		       0x10
#define CLIPPED_FLAGS	   (CLIPPED_Z | CLIPPED_MINUS_X | CLIPPED_PLUS_X | CLIPPED_MINUS_Y | CLIPPED_PLUS_Y)

#define VERTEX_HAS_NORMAL       0x20
#define VERTEX_HAS_UV	   0x40
#define VERTEX_TRANSFORMED      0x80
#define VERTEX_UPDATED      0x100

typedef struct tagVERTEX
{
	sgl_vector      vVertex;
	
} VERTEX, *PVERTEX;

typedef struct tagDLVERTEXDATA
{
	sgl_vector      vNormal;
	sgl_2d_vec      v2UV;
	sgl_int32	    	Flags;
	PVERTEX			pV;
	
} DLVERTEXDATA, *PDLVERTEXDATA;

/* edge structure */

#define MAX_FACES_PER_EDGE 6

typedef struct tagEDGE
{
	sgl_uint32   nIDV1;
	sgl_uint32	 nIDV2;
	
} EDGE, *PEDGE;

typedef struct tagDLEDGEDATA
{
	sgl_uint16   nFaces;
	PEDGE	 pE;
	
} DLEDGEDATA, *PDLEDGEDATA;

/* Face structure */

typedef struct tagFACE
{
	int		 nVertices;
	int	     pnVertexIDs[4];
	int	     pnEdgeIDs[4];
	int		 EdgeFlags;
	CONV_PLANE_STRUCT  PlaneData;
				
} FACE, *PFACE;

typedef struct tagMESHMATERIAL
{
	MATERIAL_NODE_STRUCT    *pMaterial;
	int						nMaterialUsage;
	
} MESHMATERIAL, *PMESHMATERIAL;

/*////////////////////////////////////////////////////////////////////
//
// Define the mesh node type struct
//
//////////////////////////////////////////////////////////////////////
*/
typedef struct
{
	/*
	// Starts with a basic node header
	*/

	DL_NODE_STRUCT node_hdr;

	/*
	// The mesh bounding box
	*/
	BBOX_CENT_STRUCT CentBBox;

	/*
	// Store the polygon culling mode for the mesh
	*/
	sgl_cull_mode CullMode;

	/*
	// Edge and vertex data needed by renderer
	*/
	PVERTEX					pVertexData;
	PEDGE					pEdgeData;
	int						nVertices;
	int						nEdges;
	/* 
	// these are all pointers to lists (list.h) 
	*/
	PLIST   	Faces;
	PLIST   	Materials;
	sgl_bool	HasLocalMaterials;

	/*
	// The vertex and edge info here is only needed for
	// building/editing meshes. (It can therefore be deleted
	// if a mesh is anonymous.)
	*/
	PLIST   Vertices;
	PLIST   Edges;

	sgl_uint32   ANDedPlaneFlags;
	sgl_uint32	 ORedPlaneFlags;
	
	sgl_bool		nNeedsUpdating;
	
	ARRAY			VertexIDMap;
	int				nRemapped;
	
	CONV_SHADING_STRUCT     *pShadingData;
	CONV_TEXTURE_UNION      *pTextureData;
	CONV_POINTS_STRUCT      *pPointsData;

} MESH_NODE_STRUCT, *PMESH_NODE_STRUCT;

/*
// Utility Routines
*/
void DlCompleteCurrentMesh(void);
void DlDeleteMeshNodeRefs(DL_NODE_STRUCT *pNode);
int DlMeshAddLocalMaterial(MATERIAL_NODE_STRUCT *pMaterial);

/* end of $RCSfile: dlmesh.h,v $ */
