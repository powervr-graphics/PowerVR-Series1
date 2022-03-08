/*****************************************************************************
 *;++
 * Name	   : $RCSfile: dlmesh.c,v $
 * Title	  : DLMESH.C
 * C Author       : Jim Page
 * Created	: 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :     SGL Mesh functions
 *		   
 * Program Type   :     C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1997/10/16 13:07:47 $	
 * $Revision: 1.92 $
 * $Locker:  $
 * $Log: dlmesh.c,v $
 * Revision 1.92  1997/10/16  13:07:47  sxy
 * deliberatly over wrote previous version, this is why there is a version number missing.
 *
 * Revision 1.90  1997/09/25  15:24:14  sxy
 * changed some DPFDEVs.
 *
 * Revision 1.89  1997/09/24  15:31:25  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.88  1997/07/18  13:58:58  gdc
 * PC'ified
 *
 * Revision 1.87  1997/06/26  15:36:00  gdc
 * removed printf's
 *
 * Revision 1.86  1997/06/26  15:34:16  gdc
 * added some input parameter checking to fix GroundMaster start up page
 * fault
 *
 * Revision 1.85  1997/06/05  13:56:08  mjg
 * ... and some more.
 *
 * Revision 1.84  1997/06/04  15:31:04  gdc
 * stopped some compiler warnings
 *
 * Revision 1.83  1997/05/19  19:50:06  mjg
 * Changed a PVROSPrintf to a DBG_WARNING.
 *
 * Revision 1.82  1997/05/13  17:52:24  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.81  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.80  1997/04/30  19:18:35  jop
 * Put Ed's changes back to how they were
 * y
 *
 * Revision 1.79  1997/04/29  14:45:01  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.78  1997/04/22  10:06:06  gdc
 * removed an unused function to stop a compiler warning ->
 *  Function:    EnumUpdateFacesContainingVertex
 *
 * Revision 1.77  1997/04/21  09:17:29  gdc
 * removed an unused varaible to elimatate compiler warning
 *
 * Revision 1.76  1997/04/07  12:14:12  gdc
 * killed of gnWarning global
 *
 * Revision 1.75  1997/04/02  21:27:16  mjg
 * Updated calling conventions.
 *
 * Revision 1.74  1997/04/01  19:15:36  mjg
 * sprintf to PVROSPrintf
 *
 * Revision 1.73  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.72  1997/03/25  10:14:35  jop
 * Removed unnecessary board dependancies
 *
 * Revision 1.71  1997/03/11  11:48:02  gdc
 * another potential fix for a Virtuality bug - but this hasn't been
 * checked
 *
 * Revision 1.70  1997/02/27  12:43:01  gdc
 * set_face_data function now sets up the PointsData structure against
 * the possiblity of texture wrapping being used on a flat shaded mesh
 *
 * Revision 1.69  1997/02/27  10:43:44  gdc
 * potentially fixed virtuality bug, whereby setting a normal via
 * sgl_set_vertex would be unable to set normal
 *
 * Revision 1.68  1997/02/25  11:31:04  gdc
 * added a check for an invalid (NULL) pointer to input data
 *
 * Revision 1.67  1997/02/10  16:05:20  sxy
 * Changed the return value of set_face_data and error condition for do not add
 * face in sgl_add_face, fixed bug neche-addface.
 *
 * Revision 1.66  1996/11/29  10:18:32  erf
 * Changed condition in set_face_data() to check for all 0 normals.
 *
 * Revision 1.65  1996/11/28  14:51:34  erf
 * Removed ASSERT problems with sgl_add_face() calls. If error
 * do not add face. Remove ASSERT to check for invalid normals.
 *
 * Revision 1.64  1996/11/27  18:39:46  erf
 * When sgl_modify_mesh() called with clear_mesh = TRUE then need to
 * clear ->nEdges and ->nVertices.
 *
 * Revision 1.63  1996/11/14  16:31:03  gdc
 * fixed a bug that caused gcc to croak, but icl swallowed and (probably
 * accidently) did the right thing
 *
 * Revision 1.62  1996/11/08  17:54:10  gdc
 * fix so that when vertices are modified the plane data for
 * the faces using the vertex is updated
 *
 * Revision 1.61  1996/11/08  15:37:52  gdc
 * sorted out the mesh editing API functions - its not fast, not clever -
 * - but it works (maybe).
 *
 * Revision 1.60  1996/09/18  12:43:02  gdc
 * removed the local material clean up that was causing mech II
 * crash - but this will leave a small memory leak if local materials
 * are defined in meshes
 *
 * Revision 1.59  1996/09/11  10:53:57  gdc
 * added code to delete a possible name table entry when local materials are deleted
 * with the mesh, this may be related to the mech bug if they are naming local meterials
 * then attempting to use them after a mesh is deleted
 *
 * Revision 1.58  1996/09/10  08:15:09  gdc
 * Added more free()'s for local material index and an array struct
 *
 * Revision 1.57  1996/09/09  08:42:03  gdc
 * Added free()'s for optionally created structures when mesh is deleted
 * i.e. pVertexData, pEdgeData, pTextureData, pShadingData
 * these were already being freed in sgl_modify_mesh if the mesh was being
 * cleaned on modify.
 *
 * Revision 1.56  1996/08/21  08:29:54  sjf
 * Fixed bug in bounding box generation. It was "effectively" including the
 * origin in the bounding box (oops).
 *
 * Revision 1.55  1996/07/17  13:58:52  sjf
 * Got rid of an uneeded include.
 *
 * Revision 1.54  1996/07/11  16:39:25  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.53  1996/07/10  10:53:29  msh
 * DisplayStatusMessage conditional on WIN32
 *
 * Revision 1.52  1996/07/05  07:53:00  sjf
 * 1)Added colin face points warnining
 * 2) removed old colinear UV modifying code
 * 3) sgl delete face and sgl set face were utterly brain dead - code
 *    just does nothing now... this will have t be fixed properly later.
 *
 * Revision 1.51  1996/06/28  13:07:35  jrc
 * Added status message for UV warnings from sgl_add_face.
 *
 * Revision 1.50  1996/06/27  11:41:06  jrc
 * Added a warning parameter to call of MapExternalToInternal.
 *
 * Revision 1.49  1996/06/26  16:17:29  sm
 * disabled co-linear point stuff
 *
 * Revision 1.48  1996/06/21  14:37:53  jrc
 * Surrounded #include <windows.h> and OutputDebugString by #if WIN32...#endif.
 *
 * Revision 1.47  1996/06/17  19:09:07  jop
 * HasLocalMaterials is now set for the first local material too
 *
 * Revision 1.46  1996/06/13  18:49:13  jrc
 * Colinear UVs have been made slightly non-colinear instead of resorting to
 * flat shading.  There is a small possibility that this may cause previously
 * non-colinear UVs of adjacent faces to go colinear, but I should think the
 * chance of this is very small indeed since the adjustments are very small
 * fractions of texels.
 *
 * Revision 1.45  1996/05/21  14:00:34  sjf
 * Fixed V.Bad bug with delele mesh - would leave stuff in display list.
 *
 * Revision 1.44  1996/05/20  19:29:11  jop
 * Fixed unlikely bug in modify_clear which qoul;d have
 * set up fHasLocalMaterials wrongly set up
 *
 * Revision 1.43  1996/05/17  18:56:35  jop
 * Fixed bad quad bug - now cleans up face husk
 *
 * Revision 1.42  1996/05/16  23:53:46  jop
 * Reinstated zero uv determinant check, and checked face entry
 * for non-existant vertices
 *
 * Revision 1.41  1996/05/01  14:00:03  sjf
 * Removed texel sampling.
 *
 * Revision 1.40  1996/04/23  16:44:31  sjf
 * 1) Fixed bug in that anonymous meshes were not initialises the name field
 * 2) Added code to delete unused portions of anonymous meshes.
 *
 * Revision 1.39  1996/04/22  12:56:51  sjf
 * "optimised" where you have local materials, and the first one affects
 * the first face. Removed the uneeded reference to the "global" material.
 *
 * Revision 1.38  1996/04/20  16:17:31  jop
 * Refined edge and vertex structures further
 *
 * Revision 1.37  1996/04/19  19:29:24  jop
 * fixed assert
 *
 * Revision 1.36  1996/04/19  18:21:49  jop
 * Chopped VERTEX struct into 2 bits
 *
 * Revision 1.35  1996/04/11  14:20:16  jop
 * Added points data array to mesh structure
 *
 * Revision 1.34  1996/04/04  11:21:50  sjf
 * Code now only allows a MAX of SGL_MAX_PLANES faces per material .. any over
 * this limit causes a new "dummy" material to be created.
 *
 * Revision 1.33  1996/04/02  12:31:56  sjf
 * Changed the way local materials VS faces were stored. Now done such that for every
 * material there are a count of faces.
 *
 * Revision 1.32  1996/03/25  19:34:23  sjf
 * 1) Now uses standard bounding box.
 * 2) Changed min and max region values to Region Rectangle structure.
 *
 * Revision 1.31  1996/03/25  16:38:35  jop
 * Optimisations
 *
 * Revision 1.30  1996/03/20  15:56:34  jrc
 * It should now be possible to automatically create a mesh without calling
 * sgl_create_mesh (when you don't need a name for the mesh).
 *
 * Revision 1.29  1996/03/13  10:43:47  jop
 * Fixed virtuality bug changing vertex info
 *
 * Revision 1.28  1996/03/07  17:14:53  sjf
 * Removed an illegal parameter in a debug message.
 *
 * Revision 1.27  1996/02/09  16:22:23  jop
 * Converted to new list way of doing things
 *
 * Revision 1.26  1996/01/09  12:05:09  jrc
 * Changed sgl_err_no_name to sgl_err_no_mesh for 'no current mesh' errors.
 *
 * Revision 1.25  1995/11/07  14:53:57  sjf
 * Fixed param checking on cull mode (I had changed the order of
 * the ENUMs in sgl.h).
 *
 * Revision 1.24  1995/11/02  19:48:07  sjf
 * Added culling choice function.
 *
 * Revision 1.23  1995/11/01  19:05:06  jop
 * Stopped bug in Optimise mode add_face quad cutter
 *
 * Revision 1.22  1995/10/31  17:00:26  jop
 * Improved (subjective, I admit) the quad cutter
 *
 * Revision 1.21  1995/10/30  22:22:34  jop
 * Added co-planarity test (and .ini setting) to get rid of
 * twisted (and no doubt bitter) quads
 *
 * Revision 1.20  1995/10/28  23:16:39  jop
 * Added redundant vertex elimination
 *
 * Revision 1.19  1995/10/27  19:13:45  jop
 * Removed bounding box update bug - bad 'else'
 *
 * Revision 1.18  1995/09/14  12:04:42  jop
 * Moved smooth shading data precalc to rnmesh.c
 *
 * Revision 1.17  1995/09/12  18:36:29  jop
 * Corrected SetVertex bug - normals etc didn't change. Bit clumsy.
 *
 * Revision 1.16  1995/09/04  13:16:24  jop
 * changed delta normal calculation to use normalised vectors
 *
 * Revision 1.15  1995/08/24  15:34:32  jop
 * ORed in the pre-mapped flag when the user specifies UV coords
 *
 * Revision 1.14  1995/08/23  15:55:10  jop
 * Shading normals now checked for correct orientation and if necessary
 * negated
 *
 * Revision 1.12  1995/08/21  21:58:07  jop
 * Outputs bad UV determinant value to debug window
 *
 * Revision 1.10  1995/08/14  12:06:53  jop
 * Improved efficiency of add_face and added smooth shading precalculations
 *
 * Revision 1.9  1995/08/04  17:16:44  jop
 * Added code to validate UV data when a face is added, and set
 * flags accordingly
 *
 * Revision 1.8  1995/08/02  20:34:07  jop
 * Added plane flag calculation
 *
 * Revision 1.7  1995/07/27  18:44:44  jop
 * Added edge flags process
 *
 * Revision 1.6  1995/07/23  17:11:24  jop
 * replaced adjacency stuff and removed ifdef. Here to stay now
 *
 * Revision 1.5  1995/07/15  19:58:05  jop
 * Reinstated adjacency stuff
 *
 * Revision 1.4  1995/06/08  17:11:21  jop
 * Changed #include "memory.h" to "string.h"
 *
 * Revision 1.3  1995/06/05  15:41:05  jop
 * Removed adjacency stuff
 *
 * Revision 1.2  1995/06/05  15:27:44  jop
 * Completed coding and added adjacency checking
 *
 * Revision 1.1  1995/06/02  20:08:43  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/
/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++					      includes				      ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     
	
#include <string.h>
#include <limits.h>
#include <float.h>
#include "math.h"

#define  MODULE_ID      MODID_DLMESH

#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"   /* current mesh ptr, etc */
#include "dlconvex.h"   /* MapExternalToInternal() */
#include "sgl_math.h"

#include "list.h"
#include "error.h"

#include "dlshade.h"
#include "profile.h"
#include "pvrosapi.h"
#include "sglmem.h"

#ifndef NULL
#define NULL    0
#endif

#if DEBUG
#define static
#endif

typedef struct tagENUMSTRUCT
{
	PMESH_NODE_STRUCT pMesh;
	int	  nVertexID;

} ENUMSTRUCT, *PENUMSTRUCT;

typedef struct tagOPTVERTEX
{
	float *Vertex;
	float *Normal;
	float *UV;
		
} OPTVERTEX, *POPTVERTEX;

static sgl_bool gDlMeshInitialised = FALSE;
static sgl_bool gOptimise = FALSE;
static float gfBlurPos = 0.0001f;
static float gfBlurUV = 0.0001f;
static float gfBlurNormal = 0.0001f;
static int Vertices=0, Remapped=0;
static float gfCoPlanarity = 0.9999f;



/*
// For debugging purposes, optionally add/setup an ID on each of
// the meshes.
*/
#if ENUMERATE_MESHES
static int CurrentMeshNumber=0;
#endif




/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++					      Macros					  ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     

#define CLAMP(x,min,max)  (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++			      static function prototypes		      ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     

static void     TidyUpCurrentState (void);
static int      CheckCurrentMesh (void);

static int      AddEdgeToAdjacencyInfo (PMESH_NODE_STRUCT pMesh, sgl_uint32 nIDV1, sgl_uint32 nIDV2, int nFaceID);
static int      RemoveEdgeFromAdjacencyInfo (PMESH_NODE_STRUCT pMesh, int nEdgeID);
static int 		set_face_data (PMESH_NODE_STRUCT pMesh, PFACE pFace, int nNumToAdd, int *pnVertexIDs, int fDoAdjacency);

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++				      static functions			      ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     
/*===========================================
 * Function:    DlMeshInitialise
 *===========================================
 *
 * Scope:	    static to this module
 *
 * Purpose:		initialises module (optimisation etc)
 *
 * Params:	    void
 *
 * Return:	    void
 *
 * Globals accessed: none
 *========================================================================================*/
static void DlMeshInitialise (void)
{
	gDlMeshInitialised = TRUE;

	gOptimise = SglReadPrivateProfileInt ("Mesh", "Optimise", FALSE, "sgl.ini");

	if (gOptimise)
	{
		DPF ((DBG_MESSAGE, "Mesh optimisation enabled!"));
		
		gfBlurPos = 	(float) SglReadPrivateProfileInt ("Mesh", "BlurPos", 1, "sgl.ini") * 0.0001f;
		gfBlurUV = 		(float) SglReadPrivateProfileInt ("Mesh", "BlurUV", 1, "sgl.ini") * 0.0001f;
		gfBlurNormal = 	(float) SglReadPrivateProfileInt ("Mesh", "BlurNormal", 1, "sgl.ini") * 0.0001f;
	}
	else
	{
		DPF ((DBG_VERBOSE, "Mesh optimisation disabled"));
	}

	gfCoPlanarity = 1.0f - ((float) SglReadPrivateProfileInt ("Mesh", "CoPlanarity", 1, "sgl.ini") * 0.0001f);
}

/*===========================================
 * Function:    TidyUpCurrentState
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Completes all 'current' definitions except current list
 *
 * Params:	    void
 *
 * Return:	    void
 *
 * Globals accessed: none
 *========================================================================================*/
static void TidyUpCurrentState (void)
{
	/* complete all except current list */

	DlCompleteCurrentConvex ();
	DlCompleteCurrentTransform ();
	DlCompleteCurrentMaterial ();   
}

/*===========================================
 * Function:    CheckCurrentMesh
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Checks if there is a current mesh. If not, creates an anonymous
 *			    one (non-local). If there is an error, it is returned raw.
 *
 * Params:	    void
 *
 * Return:	    int sgl_no_err if a current mesh is in place, sgl_err_xxx if there
 *			    was an error creating a new one.
 *
 * Globals accessed:   dlUserGlobals.pCurrentMesh
 *========================================================================================*/
static int CheckCurrentMesh (void)
{
	int nError;

	if (dlUserGlobals.pCurrentMesh)
	{
		/* we are already in a mesh definition */

		nError = sgl_no_err;
	}
	else
	{
		/* no current mesh, so create an anonymous one */

		nError = sgl_create_mesh (FALSE);

		/* it might return a name (despite being anonymous), so be safe ... */
		
		if (nError > 0)
		{
			nError = sgl_no_err;
		}
	}

	return (nError);
}

/*===========================================
 * Function:    OnDeleteVertex
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Called when deleting a vertex data structure
 *
 * Params:	    void *pData - pointer to the vertex to be deleted
 *
 * Return:	    void
 *========================================================================================*/
static void OnDeleteVertex (PCVOID pData)
{
	PDLVERTEXDATA pDLVertexData = (PDLVERTEXDATA) pData;

	ASSERT (pDLVertexData != NULL);

	DPF ((DBG_VERBOSE, "Vertex deleted"));
}

/*===========================================
 * Function:    OnDeleteFace
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Called when deleting a nFace data structure
 *
 * Params:	    void *pData - pointer to the nFace to be deleted
 *
 * Return:	    void
 *========================================================================================*/
static void OnDeleteFace (PCVOID pData)
{
	PFACE pFace = (PFACE) pData;

	ASSERT (pFace != NULL);

	DPF ((DBG_VERBOSE, "Face deleted"));
}

/*===========================================
 * Function:    OnDeleteEdge
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Called when deleting an edge data structure
 *
 * Params:	    void *pData - pointer to the edge to be deleted
 *
 * Return:	    void
 *========================================================================================*/
static void OnDeleteEdge (PCVOID pData)
{
	PDLEDGEDATA pDLEdgeData = (PDLEDGEDATA) pData;

	ASSERT (pDLEdgeData != NULL);
	DPF ((DBG_VERBOSE, "Edge deleted"));
}

/*===========================================
 * Function:    OnDeleteMaterial
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Called when deleting an local material data structure
 *
 * Params:	    void *pData - pointer to the edge to be deleted
 *
 * Return:	    void
 *========================================================================================*/
static void OnDeleteMaterial (PCVOID pData)
{
	PMESHMATERIAL pMaterial = (PMESHMATERIAL) pData;

	ASSERT (pMaterial != NULL);
	DPF ((DBG_VERBOSE, "Local mesh material deleted"));
}


/**************************************************************************
 * Function Name  : CheckFaceCount
 * Inputs         : pMesh - pointer to the mesh to test
 *
 * Outputs        : TRUE if face count seems ok.
 *
 * Description    : This compares the face count that is stored in the face
 *					structure with that in all of the local materials. If they
 *					are the same, return TRUE.
 **************************************************************************/
#if DEBUG
static sgl_bool CheckFaceCount(PMESH_NODE_STRUCT pMesh)
{
	PMESHMATERIAL pLocalMat;
	int NumLocalMats;
	int i;
	int FCount=0;
	/*
	// How many materials have we got?
	*/
	NumLocalMats = ListGetItemsInList(pMesh->Materials);

	/*
	// Step through all the local materials, counting up the
	// number of faces each affects
	*/
	pLocalMat = ListFindItemFast (pMesh->Materials, 0);

	for(i = 0; i < NumLocalMats; i++, pLocalMat++)
	{
		ASSERT(pLocalMat->nMaterialUsage >=0);
		ASSERT(pLocalMat->nMaterialUsage < 10000);

		FCount+= pLocalMat->nMaterialUsage;

		ASSERT(FCount <= ListGetItemsInList(pMesh->Faces)); 

	}/*end for*/

	return(FCount == ListGetItemsInList(pMesh->Faces));
}
#endif

/**************************************************************************
 * Function Name  : UpdateLocalMaterialUsage
 * Inputs         : LocMaterials: The list of local materials for the
 *								 the current Mesh
 *
 *					LocMaterialsUsage: Number of polys/planes affected
 *									   by each local material.
 *
 *					addPlane:if we are adding a new poly.
 *					
 *					planeRemoved
 *					
 * Outputs        : TransformedPlanes - the planes after transformation
 *
 * Description    : This is used whenever we add or delete a face. It basically
 *					updates the use count of the latest local material, if we
 *					are adding a new face, or decreases a particular materials
 *					count when we are deleting.
 *
 *					If we have no local materials, then
 **************************************************************************/
static void UpdateLocalMaterialUsage( PLIST   LocMaterials,
								   sgl_bool   addPlane,
								  const int   planeRemoved)
{
	PMESHMATERIAL pLocalMat;
	int NumLocalMats;

	/*
	// How many materials have we got?
	*/
	NumLocalMats = ListGetItemsInList(LocMaterials);

	/*
	// OK, are we adding or deleting a face?
	*/
	if(addPlane)
	{

		/*
		// get a pointer to the last local material added
		*/
		pLocalMat = ListFindItemFast (LocMaterials, NumLocalMats - 1);


		/*
		// If we have reached the maximum number of surfaces a material
		// can have, then put a "null" material on the list and re-start
		// the count
		*/
		if(pLocalMat->nMaterialUsage == SGL_MAX_PLANES)
		{
			pLocalMat = ListAddItem(LocMaterials);

			pLocalMat->nMaterialUsage=1;
			pLocalMat->pMaterial = NULL;
		}
		else
		{
			/*
			// Simply increment the count on the usage for the material
			*/
			pLocalMat->nMaterialUsage++;

			ASSERT(pLocalMat->nMaterialUsage > 0);
			ASSERT(pLocalMat->nMaterialUsage <= SGL_MAX_PLANES);
		}
	}
	/*
	// else we are removing a face. Find the face which is guilty
	*/
	else
	{
		#if DEBUG
			int matCount=1;
		#endif
		int FaceCount;

		/*
		// Ok, get the first element in the list
		*/
		pLocalMat = ListFindItemFast (LocMaterials, 0);

		FaceCount = pLocalMat->nMaterialUsage;

		/*
		// While we havent reached the material we want
		*/
		while(FaceCount <= planeRemoved)
		{
			/*
			// Move to the next local material
			*/
			pLocalMat++;

			FaceCount += pLocalMat->nMaterialUsage;

			#if DEBUG
				matCount ++;
				ASSERT(NumLocalMats >= matCount);
				ASSERT(FaceCount >= 0);
			#endif
		}/*end while*/

		/*
		// decrease the usage of this material
		*/
		pLocalMat->nMaterialUsage --;

		ASSERT(pLocalMat->nMaterialUsage>= 0);
		
	}/*end else removing a face*/

}


/*===========================================
 * Function:    FindIdenticalVertex
 *===========================================
 *
 * Scope:	    static to this module
 *
 * Purpose:	  	Check if a vertex is identical within reason to another vertex
 *
 * Params:	    void *pvVertex: vertex to test
 *			    sgl_uint32 pvData: OPTVERTEX struct
 *
 * Return:	    TRUE if identical (sic) FALSE if not
 *========================================================================================*/
static sgl_bool FindIdenticalVertex (void *pvVertex, sgl_uint32 pvData)
{
	PDLVERTEXDATA pDLVertexData = pvVertex;
	POPTVERTEX pOV = (POPTVERTEX) pvData;
	int k;
	
	
	for (k = 0; k < 3; ++k)
	{
		if (fabs (pDLVertexData->pV->vVertex[k] - pOV->Vertex[k]) > gfBlurPos)	
		{
			return (FALSE);
		}
	}

	if (pOV->Normal)
	{
		for (k = 0; k < 3; ++k)
		{
			if (fabs (pDLVertexData->vNormal[k] - pOV->Normal[k]) > gfBlurNormal)
			{
				return (FALSE);
			}
		}
	}

	if (pOV->UV)
	{
		for (k = 0; k < 2; ++k)
		{
			if (fabs (pDLVertexData->v2UV[k] - pOV->UV[k]) > gfBlurUV)
			{
				return (FALSE);
			}
		}
	}

	return (TRUE);
}

/*===========================================
 * Function:    AddEdgeToAdjacencyInfo
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Adds an edge to adjacency info (edge list and vertex edge reference data)
 *
 * Params:	    PMESH_NODE_STRUCT pMesh: pointer to mesh structure
 *			    int nIDV1: ID of vertex 1 in edge
 *			    int nIDV2: ID of vertex 2 in edge
 *			    int nFaceID: id of face that the edge belongs to
 *
 * Return:	    void
 *========================================================================================*/
static int AddEdgeToAdjacencyInfo (PMESH_NODE_STRUCT pMesh, 
								   sgl_uint32 nIDV1, sgl_uint32 nIDV2, int nFaceID)
{
    int nError = sgl_no_err;
    int i;
    PDLEDGEDATA pLEdgeData;
	PEDGE pNew;

    ASSERT(pMesh);
    ASSERT(nIDV1 <= (sgl_uint32) pMesh->nVertices);
    ASSERT(nIDV2 <= (sgl_uint32) pMesh->nVertices);
	/* I really want to check this - but no-one keeps a record of num faces
	** ASSERT(nFaceID < pMesh->nFaces);
	*/

	/* Edge refs are stored in the vertex who's id is lower, so ... */
	if (nIDV1 < nIDV2)
	{
		/* order ok! */
		nError = sgl_no_err;
	}
	else if (nIDV1 > nIDV2)
	{
		/* swap em over */

		int nTemp = nIDV2;

		nIDV2 = nIDV1;
		nIDV1 = nTemp;

		nError = sgl_no_err;
	}
	else
	{
		DPF((DBG_WARNING, 
			 "AddEdgeToAdjacencyInfo: Bad parameter - 2 vertices with same ID"));
		nError = sgl_err_bad_parameter;

		/* Go no further if error. ie no more processing !!!
		 */
		return(nError);
	}

	/* this should be simple - look down the edge list to see if we
	** have this edge already - if not add it to the edge list
	** return the index for the edge
	*/
	for(i=0; i<pMesh->nEdges; i++)
	{
		pLEdgeData = ListFindItemFast(pMesh->Edges, i);
		if(nIDV1==pLEdgeData->pE->nIDV1 && nIDV2==pLEdgeData->pE->nIDV2)
		{
			/* the edge should already refer to atleast one face if it exists */
			ASSERT(pLEdgeData->nFaces); 

			/* found the edge - increment its ref count */
			pLEdgeData->nFaces++; 

			/* return its ID */
			return i;
		}
	}

	/* didnt find the edge - add it on */
	pLEdgeData = ListAddItem(pMesh->Edges);

	if (!pLEdgeData)
	{
		DPF((DBG_ERROR, 
			 "AddEdgeToAdjacencyInfo: error adding edge to edge list!"));
		nError = sgl_err_no_mem;
		SglError(nError);
		return(nError);
	}

	pLEdgeData->nFaces = 1; 
	/* expand the actual array which references the vertices */
	if(pMesh->pEdgeData)
	{
		pNew = SGLRealloc(pMesh->pEdgeData, sizeof(EDGE) * (pMesh->nEdges + 1));
	}
	else
	{
		pNew = SGLMalloc(sizeof(EDGE) * (pMesh->nEdges + 1));
	}
	
	if(!pNew)
	{
		DPF((DBG_ERROR, 
			 "AddEdgeToAdjacencyInfo: error expanding edge to edge list!"));
		nError = sgl_err_no_mem;
		SglError(nError);
		return(nError);
	}
	else if(pNew != pMesh->pEdgeData) 
	{
		pMesh->pEdgeData = pNew;
		/* array has moved - fix up all pointers */
		for(i=0; i<pMesh->nEdges; i++)
		{
			pLEdgeData = ListFindItemFast(pMesh->Edges, i);
			pLEdgeData->pE = &(pMesh->pEdgeData[i]); 
		}
	}
	
	pLEdgeData = ListFindItemFast(pMesh->Edges, pMesh->nEdges);
	pLEdgeData->pE = &(pMesh->pEdgeData[pMesh->nEdges]); 
	pLEdgeData->pE[0].nIDV1 = nIDV1;
	pLEdgeData->pE[0].nIDV2 = nIDV2;
	pMesh->nEdges++;
		
	return(pMesh->nEdges-1);
}


/*===========================================
 * Function:    RemoveEdgeFromAdjacencyInfo
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	  Removes an edge from adjacency info (edge list and vertex edge reference data)
 *
 * Params:	    PMESH_NODE_STRUCT pMesh: pointer to mesh structure
 *			    int nIDV1: ID of vertex 1 in edge
 *			    int nIDV2: ID of vertex 2 in edge
 *			    int nFaceID: id of face that the edge belongs to
 *
 * Return:	    void
 *========================================================================================*/
static int RemoveEdgeFromAdjacencyInfo (PMESH_NODE_STRUCT pMesh, int nEdgeID)
{
    int nError = sgl_no_err;
    int i, j, nFaces;
    PDLEDGEDATA pLEdgeData;
	PFACE pLFaceData;

    ASSERT(pMesh);

	if(nEdgeID >= ListGetItemsInList(pMesh->Edges))
	{
		nError = sgl_err_bad_parameter;
		return(nError);
	}
  
    pLEdgeData = ListFindItemFast(pMesh->Edges, nEdgeID);

    /* if an edge has a ref count of zero - something is wrong */
    ASSERT(pLEdgeData->nFaces);
  
    pLEdgeData->nFaces--;
    if(pLEdgeData->nFaces)
    {
		/* the edge is still in use by other faces */
		return(nError);
    }

    /* edge is unused - get rid of it */
    ListRemoveItem (pMesh->Edges, NULL, nEdgeID);
    
    for(i=nEdgeID; i<(pMesh->nEdges-1); i++)
    {
		/* bump down the actual edge array */
		pMesh->pEdgeData[i].nIDV1 = pMesh->pEdgeData[i+1].nIDV1;
		pMesh->pEdgeData[i].nIDV2 = pMesh->pEdgeData[i+1].nIDV2;
		/* bump down the list pointers to the edges */
		pLEdgeData = ListFindItemFast(pMesh->Edges, i);
		pLEdgeData->pE = &(pMesh->pEdgeData[i]);

    }


    /* fix up the edge ids in the faces */
	nFaces = ListGetItemsInList(pMesh->Faces);
    for(i=0; i<nFaces; i++)
    {
		pLFaceData = ListFindItemFast(pMesh->Faces, i);
		for(j=0; j<pLFaceData->nVertices; j++)
		{
			if(pLFaceData->pnEdgeIDs[j] > nEdgeID)
			{
				pLFaceData->pnEdgeIDs[j]--;
			}
		}
	}

	pMesh->nEdges--;

    return (nError);
}

/*===========================================
 * Function:    UpdateBoundingBoxInfo
 *===========================================
 *
 * Scope:	   static to this module
 *
 * Purpose:	 Compares a vertex to existing bounding box and expands it if necessary
 *
 * Params:	  	  BBOX_CENT_STRUCT CentBBox: The current bounding box in 
											 Centre/offset format.
 *			      sgl_vector fVertex: new vertex
 *
 * Return:	  void
 *========================================================================================*/
static void UpdateBoundingBoxInfo (BBOX_CENT_STRUCT *pCentBBox, float *fVertex)
{
	int k;
	float min, max;
	/* update bounding box info */

	for (k = 0; k < 3; ++k)
	{
		/*
		// Compute the current extremes
		*/
		min = pCentBBox->boxCentre[k] - pCentBBox->boxOffsets[k];
		max = pCentBBox->boxCentre[k] + pCentBBox->boxOffsets[k];

		if (max < fVertex[k])
		{
			max = fVertex[k];
		}

		if (min > fVertex[k])
		{
			min = fVertex[k];
		}

		/*
		// Put it back into centre/offset format
		*/
		pCentBBox->boxCentre[k]  = (max + min) * 0.5f;
		pCentBBox->boxOffsets[k] = (max - min) * 0.5f;
	}
}


/*===========================================
 * Function:    set_face_data
 *===========================================
 *
 * Scope:	    static to this module
 *
 * Purpose:	  	Set up face data struct - used by add/set_face
 *
 * Params:	    PMESH_NODE_STRUCT pMesh: Parent mesh ptr
 *				PFACE pFace: face ptr
 *				int nNumToAdd: number of vertices
 *				int *pnVertexIDs: vertex IDs
 *				int fDoAdjacency: if TRUE add face to adjacency info
 *
 * Return:	    void
 *========================================================================================*/
static int set_face_data (PMESH_NODE_STRUCT pMesh, PFACE pFace, int nNumToAdd, int *pnVertexIDs, int fDoAdjacency)
{
	int	 	nVertices = CLAMP (nNumToAdd, 3, 4);
	PDLVERTEXDATA pV[4];
	sgl_uint32   ANDedVertexFlags;
	int 	k, nFaceID, nFacesInList;
	int 	nError = sgl_no_err;
	CONV_SHADING_STRUCT	*pShadingData;
	CONV_TEXTURE_UNION	*pTextureData;
	CONV_POINTS_STRUCT	*pPointsData;
	int nEdges = 0;
	
	ASSERT (pMesh);
	ASSERT (pFace);
	
	nFaceID = ListGetItemID (pMesh->Faces, pFace);
	nFacesInList = ListGetItemsInList (pMesh->Faces);
	
	pV[0] = ListFindItemFast (pMesh->Vertices, pnVertexIDs[0]);
	pV[1] = ListFindItemFast (pMesh->Vertices, pnVertexIDs[1]);
	pV[2] = ListFindItemFast (pMesh->Vertices, pnVertexIDs[2]);

	/* no point to assertion as ListFindItemFast never returns a NULL */
	ASSERT (pV[0]);
	ASSERT (pV[1]);
	ASSERT (pV[2]);
	
	if (nVertices == 3)
	{
		ANDedVertexFlags = pV[0]->Flags & pV[1]->Flags & pV[2]->Flags;
	}
	else
	{
		pV[3] = ListFindItemFast (pMesh->Vertices, pnVertexIDs[3]);

		/* no point to assertion as ListFindItemFast never returns a NULL */
		ASSERT (pV[3]);

		ANDedVertexFlags = pV[0]->Flags & pV[1]->Flags & pV[2]->Flags & pV[3]->Flags;
	}

	/*
	// Check for colinearity:
	*/
	{
		#define MIN_CROSS_PROD  0.00001

		sgl_vector  vDiff1, vDiff2, vCrossProd;

		vDiff1[0] = pV[0]->pV->vVertex[0] - pV[1]->pV->vVertex[0];
		vDiff1[1] = pV[0]->pV->vVertex[1] - pV[1]->pV->vVertex[1];
		vDiff1[2] = pV[0]->pV->vVertex[2] - pV[1]->pV->vVertex[2];

		vDiff2[0] = pV[0]->pV->vVertex[0] - pV[2]->pV->vVertex[0];
		vDiff2[1] = pV[0]->pV->vVertex[1] - pV[2]->pV->vVertex[1];
		vDiff2[2] = pV[0]->pV->vVertex[2] - pV[2]->pV->vVertex[2];

		CrossProd(vDiff1, vDiff2, vCrossProd);

		if (fabs(vCrossProd[0]) < MIN_CROSS_PROD &&
		    fabs(vCrossProd[1]) < MIN_CROSS_PROD &&
		    fabs(vCrossProd[2]) < MIN_CROSS_PROD)
		{
			if (nVertices == 3)
			{
				DPFDEV((DBG_MESSAGE,"Colinear face points"));
				SglWarning(sgl_warn_colinear_face_points);
			}
			else
			{
				ASSERT(nVertices == 4);

				vDiff2[0] = pV[0]->pV->vVertex[0] - pV[3]->pV->vVertex[0];
				vDiff2[1] = pV[0]->pV->vVertex[1] - pV[3]->pV->vVertex[1];
				vDiff2[2] = pV[0]->pV->vVertex[2] - pV[3]->pV->vVertex[2];

				CrossProd(vDiff1, vDiff2, vCrossProd);

				if (fabs(vCrossProd[0]) < MIN_CROSS_PROD &&
				    fabs(vCrossProd[1]) < MIN_CROSS_PROD &&
				    fabs(vCrossProd[2]) < MIN_CROSS_PROD)
				{
					DPFDEV((DBG_MESSAGE,"Colinear face points"));
					SglWarning(sgl_warn_colinear_face_points);
				}

			} /* if (nVertices == 3) ... else */

		} /* if first cross product too small */

		#undef MIN_CROSS_PROD

	} /* local variables */

	if (fDoAdjacency)
	{
		/* compile edge list and vertex references as we go along ... */
	
		for (k = 0; (nError >= 0) && (k < nVertices); ++k)
		{
			int nIDV2 = pnVertexIDs[k];
			int nIDV1 = (!k) ? pnVertexIDs[nVertices - 1] : pnVertexIDs[k - 1];
			
			nEdges = AddEdgeToAdjacencyInfo (pMesh, nIDV1, nIDV2, nFaceID);
			
			if (nEdges >= 0)
			{
				/* pointer into edge list */
				
				pFace->pnEdgeIDs[k] = nEdges;
	
				if (nIDV1 > nIDV2)
				{
					/* edge equation info will be reversed */
					pFace->EdgeFlags |= 1 << k;
				}
				else
				{
					pFace->EdgeFlags &= ~(1 << k);
				}
			}
			else
			{
				nError = nEdges;
			}
		}
	}

	if (nError < 0)
	{
		ListRemoveItem (pMesh->Faces, NULL, nFaceID);

		DPF ((DBG_ERROR, "set_face_data: error compiling adjacency info"));
	}
	else
	{
		sgl_vector   fpa, fpb;
				
		VecSub (pV[1]->pV->vVertex, pV[0]->pV->vVertex, fpa);
		VecSub (pV[2]->pV->vVertex, pV[0]->pV->vVertex, fpb);
		CrossProd (fpa, fpb, pFace->PlaneData.normal);

		/* Is normal valid ? Are all values 0.
		 */
		if ((pFace->PlaneData.normal[0] != 0.0f) ||
			(pFace->PlaneData.normal[1] != 0.0f) ||
			(pFace->PlaneData.normal[2] != 0.0f))
		{
			VecNormalise (pFace->PlaneData.normal);
		}
#if DEBUG || DEBUGDEV
		else
		{
			/* Output a message that normal not valid.
			 * Dummy instruction also included.
			 */
			DPFDEV((DBG_MESSAGE,"Normal is not valid. Colinear points."));
		}
#endif
		
		VecCopy (pV[0]->pV->vVertex, pFace->PlaneData.rep_point);
		
		/* all is well so copy data into structure */

		memcpy (pFace->pnVertexIDs, pnVertexIDs, nVertices * sizeof (int));

		pFace->nVertices = nVertices;


		/* set up plane flags in face and mesh structures */

		pFace->PlaneData.flags = 0;
		


		/* points data must always exist as it is used by texture wrapping 
		** no easy way around as wrapping uses pt2 and pt3 deltas
		*/
		if (!pMesh->pPointsData)
		{
			pMesh->pPointsData = SGLMalloc (sizeof (CONV_POINTS_STRUCT) * nFacesInList);
			if(!pMesh->pPointsData)
			{
				DPF ((DBG_WARNING, "set_face_data: error adding points structure"));
				nError = sgl_err_no_mem;
			}
		}
		else
		{
			void *pNew;
			
			pNew = SGLRealloc (pMesh->pPointsData, sizeof (CONV_POINTS_STRUCT) * nFacesInList);
			
			if (pNew)
			{
				pMesh->pPointsData = pNew;
			}
			else
			{
				DPF ((DBG_WARNING, "set_face_data: error increasing points structure"));
				nError = sgl_err_no_mem;
			}
		}

		if ((nError == sgl_no_err) && (ANDedVertexFlags & VERTEX_HAS_UV))
		{
			if (!pMesh->pTextureData)
			{
				pMesh->pTextureData = SGLMalloc (sizeof (CONV_TEXTURE_UNION) * nFacesInList);
				if(!pMesh->pTextureData)
				{
					DPF ((DBG_WARNING, "set_face_data: error adding texture structure"));
					nError = sgl_err_no_mem;
				}
			}
			else
			{
				void *pNew;
				
				pNew = SGLRealloc (pMesh->pTextureData, sizeof (CONV_TEXTURE_UNION) * nFacesInList);
				
				if (pNew)
				{
					pMesh->pTextureData = pNew;
				}
				else
				{
					DPF ((DBG_WARNING, "set_face_data: error increasing texture structure"));
					nError = sgl_err_no_mem;
				}
			}
			
			if (pMesh->pTextureData)
			{
				pTextureData = pMesh->pTextureData + nFaceID;
				pFace->PlaneData.flags |= (pf_textured | pf_pre_mapped);
					
				MapExternalToInternal (pV[0]->pV->vVertex, 
									   pV[1]->pV->vVertex, 
									   pV[2]->pV->vVertex,
									   pV[0]->v2UV, 
									   pV[1]->v2UV, 
									   pV[2]->v2UV,
									   pTextureData->pre_mapped.u_vect,
									   pTextureData->pre_mapped.v_vect,
									   pTextureData->pre_mapped.o_vect);
			}
			else
			{
				DPF ((DBG_WARNING, 
					"set_face_data: error adding texture structure"));
			}
		}
		
		if ((nError == sgl_no_err) && (ANDedVertexFlags & VERTEX_HAS_NORMAL))
		{
			if (!pMesh->pShadingData)
			{
				pMesh->pShadingData = SGLMalloc (sizeof (CONV_SHADING_STRUCT) * nFacesInList);
				if (!pMesh->pShadingData)
				{
					DPF ((DBG_WARNING, "set_face_data: error adding shading structure"));
					nError = sgl_err_no_mem;
				}
			}
			else
			{
				void *pNew;
				
				pNew = SGLRealloc (pMesh->pShadingData, sizeof (CONV_SHADING_STRUCT) * nFacesInList);
				
				if (pNew)
				{
					pMesh->pShadingData = pNew;
				}
				else
				{
					DPF ((DBG_WARNING, "set_face_data: error increasing shading structure"));
					nError = sgl_err_no_mem;
				}
			}
			
			if (pMesh->pShadingData)
			{
			
				pShadingData = 	pMesh->pShadingData + nFaceID;
				pPointsData =	pMesh->pPointsData + nFaceID;
			
				VecCopy (pV[0]->pV->vVertex, pPointsData->pt1);
				VecSub (pV[1]->pV->vVertex, pV[0]->pV->vVertex, pPointsData->pt2_delta);
				VecSub (pV[2]->pV->vVertex, pV[0]->pV->vVertex, pPointsData->pt3_delta);

				if (SmoothPlaneDataPrecalc (pShadingData,
											pV[0]->pV->vVertex, pV[1]->pV->vVertex, pV[2]->pV->vVertex,
											pV[0]->vNormal, pV[1]->vNormal, pV[2]->vNormal))
				{
					pFace->PlaneData.flags |= pf_smooth_shad;
				}
				else
				{
					DPF ((DBG_WARNING, "set_face_data: invalid shading data"));
				}
			}
			else
			{
				DPF ((DBG_WARNING, "set_face_data: error adding shading structure"));
			}
		}
		else if (nError == sgl_no_err)
		{
			/* The mesh has no normals but we need to set the 
			** Points data structure incase there is texture wrapping
			*/
			pPointsData =	pMesh->pPointsData + nFaceID;
			
			VecCopy (pV[0]->pV->vVertex, pPointsData->pt1);
			VecSub (pV[1]->pV->vVertex, pV[0]->pV->vVertex, pPointsData->pt2_delta);
			VecSub (pV[2]->pV->vVertex, pV[0]->pV->vVertex, pPointsData->pt3_delta);

			if (pMesh->pTextureData)
			{
				pTextureData = pMesh->pTextureData + nFaceID;
				pFace->PlaneData.flags |= pf_textured;
			}
		}

		pMesh->ANDedPlaneFlags &= pFace->PlaneData.flags;
		pMesh->ORedPlaneFlags |= pFace->PlaneData.flags;
	}
	
	return (nError);

} /* set_face_data */


/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++			      SGL internal entry points			++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     
/*===========================================
 * Function:    DlCompleteCurrentMesh
 *===========================================
 *
 * Scope:	      SGL
 *
 * Purpose:	  Completes a mesh definition; sets the current mesh ptr to NULL.
 *
 * Params:	    void
 *
 * Return:	    void
 *
 * Globals accessed:   dlUserGlobals.pCurrentMesh
 *========================================================================================*/
void DlCompleteCurrentMesh (void)
{
	if (dlUserGlobals.pCurrentMesh)
	{
		#if ENUMERATE_MESHES
		if(dlUserGlobals.pCurrentMesh->MeshNumber==174)
		{
			DPF((DBG_MESSAGE, "At funny mesh"));
		}
		#endif


		/* could tidy up lists here, couldn't we. */
		
		if (dlUserGlobals.pCurrentMesh->nNeedsUpdating)
		{
			int k, nVertices;
			PDLVERTEXDATA pDLVertexData;
						
			/* reset the Flags */
			nVertices = ListGetItemsInList (dlUserGlobals.pCurrentMesh->Vertices);
			pDLVertexData=ListFindItemFast(dlUserGlobals.pCurrentMesh->Vertices,0);
			
			for (k = 0; k < nVertices; ++k, ++pDLVertexData)
			{
				if (pDLVertexData->Flags & VERTEX_UPDATED)
				{
					pDLVertexData->Flags &= ~VERTEX_UPDATED;
				}
			}

			dlUserGlobals.pCurrentMesh->nNeedsUpdating = FALSE;
		}

		if (gOptimise)
		{
			Vertices += ListGetItemsInList (dlUserGlobals.pCurrentMesh->Vertices);
			Remapped += dlUserGlobals.pCurrentMesh->nRemapped;
			
			DPF ((DBG_MESSAGE, "Faces %d, vertices %d/%d, edges %d (%8.2f)", 
								ListGetItemsInList (dlUserGlobals.pCurrentMesh->Faces),
								ListGetItemsInList (dlUserGlobals.pCurrentMesh->Vertices),
								dlUserGlobals.pCurrentMesh->nRemapped,
								ListGetItemsInList (dlUserGlobals.pCurrentMesh->Edges),
								((float) Vertices / (float) (Vertices + Remapped)) * 100.0f));
		}
		else
		{		
			DPF ((DBG_MESSAGE, "Faces %d, vertices %d, edges %d", 
								ListGetItemsInList (dlUserGlobals.pCurrentMesh->Faces),
								ListGetItemsInList (dlUserGlobals.pCurrentMesh->Vertices),
								ListGetItemsInList (dlUserGlobals.pCurrentMesh->Edges)));
		}


		/*
		// Save the number of edges. These are actually stored
		// int the vertex and face lists, but these may get deleted
		*/
		/*		dlUserGlobals.pCurrentMesh->nVertices = 
		**		ListGetItemsInList(dlUserGlobals.pCurrentMesh->Vertices);
		**
		**      dlUserGlobals.pCurrentMesh->nEdges =
		**		ListGetItemsInList(dlUserGlobals.pCurrentMesh->Edges);
		*/

		/*
		// If this is an anonymous mesh, then delete the bits we dont
		// need for rendering
		*/
		if(dlUserGlobals.pCurrentMesh->node_hdr.n16_name == NM_INVALID_NAME)
		{
			/*
			// We dont need the edge and vertex structures that are used
			// when building the mesh
			*/
			ListDeleteList(&dlUserGlobals.pCurrentMesh->Vertices);
			ListDeleteList(&dlUserGlobals.pCurrentMesh->Edges);

			/*
			// We also dont need the optimise re-mapping structure
			*/
			ArrayDeleteArray(&dlUserGlobals.pCurrentMesh->VertexIDMap);

		}/*end if anonymous*/


		/* finished with current mesh - set ptr to NULL */

		dlUserGlobals.pCurrentMesh = NULL;
	}
}

/*===========================================
 * Function:    DlDeleteMeshNodeRefs
 *===========================================
 *
 * Scope:	      SGL
 *
 * Purpose:	  'destructor' for mesh nodes; deletes texture wrap data if it exists
 *
 * Params:	    DL_NODE_STRUCT *pNode: mesh node to clean up
 *
 * Return:	    void
 *
 * Globals accessed:   none
 *========================================================================================*/
void DlDeleteMeshNodeRefs (DL_NODE_STRUCT *pNode)
{
	MESH_NODE_STRUCT *pMesh = (MESH_NODE_STRUCT *) pNode;
	
	/* if either of these ASSERTs fail, something is badly wrong in the list department */

	ASSERT (pMesh);
	ASSERT (pMesh->node_hdr.n16_node_type == nt_mesh);

	DPF ((DBG_VERBOSE, "deleting mesh structure (name = %d)", pMesh->node_hdr.n16_name));

	/* clean up vertex and nFace lists */
	
	ListDeleteList (&pMesh->Vertices);
	ListDeleteList (&pMesh->Faces);
	ListDeleteList (&pMesh->Materials);
	ListDeleteList (&pMesh->Edges);
 	ArrayDeleteArray (&pMesh->VertexIDMap);
	
	/* clean up the bits which are optionally created */
			
	if (pMesh->pVertexData)
	{
	  SGLFree (pMesh->pVertexData);
	  pMesh->pVertexData = NULL;
	}

	if (pMesh->pEdgeData)
	{
	  SGLFree (pMesh->pEdgeData);
	  pMesh->pEdgeData = NULL;
	}

	if (pMesh->pTextureData)
	{
	  SGLFree (pMesh->pTextureData);
	  pMesh->pTextureData = NULL;
	}

	if (pMesh->pShadingData)
	{
	  SGLFree (pMesh->pShadingData);
	  pMesh->pShadingData = NULL;
	}

	if (pMesh->pPointsData)
	{
	  SGLFree (pMesh->pPointsData);
	  pMesh->pPointsData = NULL;
	}

}

/*===========================================
 * Function:    DlMeshAddLocalMaterial
 *===========================================
 *
 * Scope:	      SGL
 *
 * Purpose:	  Adds a local material definition to a mesh
 *
 * Params:	    MATERIAL_NODE_STRUCT *pMaterial: pointer to material struct to add
 *
 * Return:	    sgl_err_xxx
 *
 * Globals accessed:   dlUserGlobals.pCurrentMesh
 *========================================================================================*/
int DlMeshAddLocalMaterial (MATERIAL_NODE_STRUCT *pMaterial)
{
	int nError;
	MESH_NODE_STRUCT *pMesh = dlUserGlobals.pCurrentMesh;
	PMESHMATERIAL pMeshMaterial;
	int NumLocalMats;

	ASSERT (pMesh);
	ASSERT (pMaterial);

	/*
	// mark that we have some real local materials
	*/
	pMesh->HasLocalMaterials=TRUE;

	/*
	// Have a look at the previous material entry... if
	// it is a null pointer and its usage is zero, then
	// it serves no purpose whatsoever, and so may as well
	// be overwritten. This can occur if the first face
	// has a local material.
	//
	// If this is the case, replace the value and exit the
	// routine.
	*/
	NumLocalMats = ListGetItemsInList(pMesh->Materials);
	if(NumLocalMats > 0)
	{
		pMeshMaterial = ListFindItemFast (pMesh->Materials, NumLocalMats - 1);
		if((pMeshMaterial->nMaterialUsage == 0) &&
									 (pMeshMaterial->pMaterial == NULL))
		{
			/*
			// Ok we can replace this entry with the new one
			*/
			pMeshMaterial->pMaterial = pMaterial;
			return (sgl_no_err);
		}
	}

	/*
	// Else we need to allocate a new entry
	*/

	pMeshMaterial = ListAddItem (pMesh->Materials);

	if (pMeshMaterial)
	{
		/*
		// Add the reference to the new material, and set the usage to 0
		*/
		pMeshMaterial->pMaterial 	  = pMaterial;
		pMeshMaterial->nMaterialUsage = 0;

		nError = sgl_no_err;
	}
	else
	{
		DPF ((DBG_ERROR, 
		"DlMeshAddLocalMaterial: error adding material to local material list"));
		nError = sgl_err_no_mem;
	}
	
	return (nError);
}

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++			      SGL API entry points		      ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     
/*===========================================
 * Function:    sgl_create_mesh
 *===========================================
 *
 * Scope:	      SGL (API)
 *
 * Purpose:	  Creates a new mesh. A new list is created that will be filled polygon
 *			    by polygon as the faces are defined by the user.
 *
 * Params:	    sgl_bool bGenerateName: TRUE if the user requires a name for the mesh
 *
 * Return:	    Success: a new name (if bGenerateName is set), or 'sgl_no_err'
 *			    Failure: -ve error code
 *========================================================================================*/
int CALL_CONV sgl_create_mesh ( sgl_bool bGenerateName )
{
	int nError;
	MESH_NODE_STRUCT *pMesh;

	if (!gDlMeshInitialised)
	{
		DlMeshInitialise ();
	}
	
	TidyUpCurrentState ();

	/* starting a new mesh, so complete the previous one if it exists */

	DlCompleteCurrentMesh ();

	/* allocate a new mesh structure */
	
	pMesh = SGLMalloc (sizeof (MESH_NODE_STRUCT));

	/* did it work? */
	
	if (!pMesh)
	{
		DPF ((DBG_ERROR, "sgl_create_mesh: SGLMalloc failed"));
		nError = sgl_err_no_mem;
	}
	else
	{
		/*
		// Set up the node header
		*/
		pMesh->node_hdr.n16_node_type = nt_mesh;
		pMesh->node_hdr.n16_name	  = NM_INVALID_NAME;

	#if ENUMERATE_MESHES
		pMesh->MeshNumber = CurrentMeshNumber;
		CurrentMeshNumber ++;

	#endif
		
		/*
		// Set the default Culling mode to include all polys
		*/
		pMesh->CullMode= sgl_no_cull;

		pMesh->Vertices = NULL;
		pMesh->nVertices = 0;
		pMesh->Faces = NULL;
		pMesh->Materials = NULL;
		pMesh->Edges = NULL;
		pMesh->nEdges = 0;

		pMesh->nRemapped = 0;

		pMesh->pVertexData = NULL;
		pMesh->pEdgeData = NULL;

		pMesh->pTextureData = NULL;
		pMesh->pShadingData = NULL;
		pMesh->pPointsData = NULL;

		if (!ListInitialiseList (&pMesh->Vertices, sizeof (DLVERTEXDATA), 16, OnDeleteVertex) ||
			!ListInitialiseList (&pMesh->Faces, sizeof (FACE), 16, OnDeleteFace) ||
			!ListInitialiseList (&pMesh->Edges, sizeof (DLEDGEDATA), 16, OnDeleteEdge) ||
			!ListInitialiseList (&pMesh->Materials, sizeof (MESHMATERIAL), 4, OnDeleteMaterial) ||
			!ArrayInitialiseArray (&pMesh->VertexIDMap, sizeof (int), 16))
		{
			/* bad news - no memory to initialise lists */

			ListDeleteList (&pMesh->Vertices);
			ListDeleteList (&pMesh->Faces);
			ListDeleteList (&pMesh->Materials);
			ListDeleteList (&pMesh->Edges);
			ArrayDeleteArray (&pMesh->VertexIDMap);
			
			SGLFree (pMesh);
			
			DPF ((DBG_ERROR, "sgl_create_mesh: error initialising lists"));
			nError = sgl_err_no_mem;
		}
		else
		{
			if (bGenerateName)
			{
				int nName;

				/* add name to name table */
			
				ASSERT (dlUserGlobals.pNamtab);

				nName = AddNamedItem (dlUserGlobals.pNamtab, pMesh, nt_mesh);

				if (nName < 0)
				{
					SGLFree (pMesh);
			
					DPF ((DBG_ERROR, "sgl_create_mesh: couldn't add mesh block to name table"));
					nError = sgl_err_no_name;
				}
				else
				{
					/* the user wants the name returned */

					pMesh->node_hdr.n16_name = nName;
					nError = nName;
				}
			}
			else
			{
				/* no name, no error */

				pMesh->node_hdr.n16_name = NM_INVALID_NAME;
				nError = sgl_no_err;
			}

			if ((nError == sgl_no_err) || (nError > 0))
			{
				PMESHMATERIAL pFirstLocalMaterial;
				

				ASSERT (dlUserGlobals.pCurrentList);
			
				AppendNodeToList (dlUserGlobals.pCurrentList, pMesh);

				/* OK, set the current mesh global to point to new one */
	
				dlUserGlobals.pCurrentMesh = pMesh;

				/*
				// Initialse the local materials. The first one is always 
				// assumed to be the global one.
				// Since there are no faces, the usage count should be set
				// to zero
				//
				// SORRY I'm being lazy here. I should check that we haven't run
				// out of memory! SJF
				*/
				pFirstLocalMaterial = ListAddItem (pMesh->Materials);
				pMesh->HasLocalMaterials = FALSE;

				/*
				// Set the usage to zero, and for safety, the pointer to
				// the local material to NULL
				*/
				pFirstLocalMaterial->pMaterial = NULL;
				pFirstLocalMaterial->nMaterialUsage = 0;
				

				/*
				// Set up a null bounding box
				*/
				pMesh->CentBBox.boxCentre[0] = 0.0f;
				pMesh->CentBBox.boxCentre[1] = 0.0f;
				pMesh->CentBBox.boxCentre[2] = 0.0f;

				pMesh->CentBBox.boxOffsets[0] = -1.0E20f;
				pMesh->CentBBox.boxOffsets[1] = -1.0E20f;
				pMesh->CentBBox.boxOffsets[2] = -1.0E20f;


				pMesh->ANDedPlaneFlags = 0xFF;
				pMesh->ORedPlaneFlags = 0;

				pMesh->nNeedsUpdating = FALSE;


				ASSERT(CheckFaceCount(pMesh));
			}
		}
	}
	
	return (SglError (nError));
}

/*===========================================
 * Function:    sgl_delete_mesh
 *===========================================
 *
 * Scope:	      SGL (API)
 *
 * Purpose:	  Deletes a mesh and tidies up.
 *
 * Params:	    int nMeshName: mesh to delete.
 *
 * Return:	    void - but may cause an error condition in SGL
 *========================================================================================*/
void CALL_CONV sgl_delete_mesh ( int nMeshName )
{
	int     nError;

	TidyUpCurrentState ();

	DlCompleteCurrentMesh ();

	ASSERT (dlUserGlobals.pNamtab);

	if (GetNamedItemType (dlUserGlobals.pNamtab, nMeshName) != nt_mesh)
	{
		DPFDEV ((DBG_ERROR, 
		"sgl_delete_mesh: bad mesh name %d (doesn't exist or not a mesh)", 
						  nMeshName));
		nError = sgl_err_bad_name;
	}
	else
	{
		PMESH_NODE_STRUCT pMesh;

		/*
		// if the name exists and is of the correct type then the
		// pointer must be valid ... 
		*/

		pMesh = GetNamedItem (dlUserGlobals.pNamtab, nMeshName);

		/* ... splat */

		ASSERT (pMesh);
		ASSERT (pMesh->node_hdr.n16_node_type == nt_mesh);

		/* delete edges, vVertices and faces */
		
		DlDeleteMeshNodeRefs ((DL_NODE_STRUCT *) pMesh);

		/* from name table */

		DeleteNamedItem (dlUserGlobals.pNamtab, nMeshName);


		/*
		// "Remove" it from the display list by marking it as a
		// dummy node. This will get cleaned up when the list itself
		// is deleted.
		*/
		pMesh->node_hdr.n16_node_type = nt_dummy;
		pMesh->node_hdr.n16_name	  = NM_INVALID_NAME;

		nError = sgl_no_err;
	}

	SglError (nError);
}

/*===========================================
 * Function:    sgl_modify_mesh
 *===========================================
 *
 * Scope:	      SGL (API)
 *
 * Purpose:	  Prepare a mesh for modification and (optionally) delete current contents
 *
 * Params:	    int nMeshName: mesh to modify.
 *			    sgl_bool bClearMesh: TRUE if the mesh is to be cleared, FALSE if not
 *
 * Return:	    void - but may cause an error condition in SGL
 *========================================================================================*/
void CALL_CONV sgl_modify_mesh ( int nMeshName, sgl_bool bClearMesh )
{
	int     nError;

	TidyUpCurrentState ();

	DlCompleteCurrentMesh ();

	ASSERT (dlUserGlobals.pNamtab);

	if (GetNamedItemType (dlUserGlobals.pNamtab, nMeshName) != nt_mesh)
	{
		DPFDEV ((DBG_ERROR, "sgl_modify_mesh: bad mesh name %d (doesn't exist or not a mesh)", nMeshName));
		nError = sgl_err_bad_name;
	}
	else
	{
		PMESH_NODE_STRUCT pMesh;

		/* if the name exists and is of the correct type then the pointer must be valid ... */

		pMesh = GetNamedItem (dlUserGlobals.pNamtab, nMeshName);

		ASSERT(CheckFaceCount(pMesh));

		/* ... splat */

		ASSERT (pMesh);
		ASSERT (pMesh->node_hdr.n16_node_type == nt_mesh);

		dlUserGlobals.pCurrentMesh = pMesh;

		if (bClearMesh)
		{
			ListDeleteList (&pMesh->Vertices);
			ListDeleteList (&pMesh->Faces);
			ListDeleteList (&pMesh->Materials);
			ListDeleteList (&pMesh->Edges);
			ArrayDeleteArray (&pMesh->VertexIDMap);

			pMesh->nRemapped = 0;
			
			/* Reset the number of edges in the mesh.
			 */
			pMesh->nEdges = 0;
			
			/* Reset the number of Vertices in the mesh.
			 */
			pMesh->nVertices = 0;
			
			if (pMesh->pVertexData)
			{
				SGLFree (pMesh->pVertexData);
				pMesh->pVertexData = NULL;
			}

			if (pMesh->pEdgeData)
			{
				SGLFree (pMesh->pEdgeData);
				pMesh->pEdgeData = NULL;
			}

			if (pMesh->pTextureData)
			{
				SGLFree (pMesh->pTextureData);
				pMesh->pTextureData = NULL;
			}

			if (pMesh->pShadingData)
			{
				SGLFree (pMesh->pShadingData);
				pMesh->pShadingData = NULL;
			}

			if (pMesh->pPointsData)
			{
				SGLFree (pMesh->pPointsData);
				pMesh->pPointsData = NULL;
			}

			if (ListInitialiseList (&pMesh->Vertices, sizeof (DLVERTEXDATA), 16, 
													  OnDeleteVertex) &&
				ListInitialiseList (&pMesh->Faces, sizeof (FACE), 16, 
													OnDeleteFace) &&
				ListInitialiseList (&pMesh->Edges, sizeof (DLEDGEDATA), 16, 
													OnDeleteEdge) &&
				ListInitialiseList (&pMesh->Materials, sizeof (MESHMATERIAL), 4, 
													 OnDeleteMaterial) &&
				ArrayInitialiseArray (&pMesh->VertexIDMap, sizeof (int), 16))

			{
				PMESHMATERIAL pFirstLocalMaterial;

				/*
				// Initialse the local materials. The first one is always 
				// assumed to be the global one.
				// Since there are no faces, the usage count should be set
				// to zero
				//
				// SORRY I'm being lazy here. I should check that we haven't run
				// out of memory! SJF
				*/
				pFirstLocalMaterial = ListAddItem (pMesh->Materials);

				/*
				// Set the usage to zero, and for safety, the pointer to
				// the local material to NULL
				*/
				pFirstLocalMaterial->pMaterial = NULL;
				pFirstLocalMaterial->nMaterialUsage = 0;
				pMesh->HasLocalMaterials = FALSE;


				/*
				// Set up a null bounding box
				*/
				pMesh->CentBBox.boxCentre[0] = 0.0f;
				pMesh->CentBBox.boxCentre[1] = 0.0f;
				pMesh->CentBBox.boxCentre[2] = 0.0f;

				pMesh->CentBBox.boxOffsets[0] = -1.0f;
				pMesh->CentBBox.boxOffsets[1] = -1.0f;
				pMesh->CentBBox.boxOffsets[2] = -1.0f;

				pMesh->ANDedPlaneFlags = 0xFF;
				pMesh->ORedPlaneFlags = 0;

				nError = sgl_no_err;

				ASSERT(CheckFaceCount(pMesh));
			}
			else
			{
				DPF ((DBG_ERROR, "sgl_modify_mesh: error initialising lists"));
				nError = sgl_err_no_mem;
			}
		}
		else
		{
			nError = sgl_no_err;
		}	      
	}

	SglError (nError);
}


/*===========================================
 * Function:    sgl_set_cull_mode
 *===========================================
 *
 * Scope:	      SGL (API)
 *
 * Purpose:	  Set the face culling mode of the mesh
 *
 * Params:	     cull_mode - Sets culling mode one of
 *						keep all faces,
 *						remove clockwise,
 *						remove anticlockwise.
 *
 * Return:	    void - but may cause an error condition in SGL
 *============================================*/
void CALL_CONV sgl_set_cull_mode( sgl_cull_mode cull_mode )
{
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;
	int nError= sgl_no_err;

	TidyUpCurrentState ();
	CheckCurrentMesh ();
	if (!pMesh)
	{
		DPF ((DBG_WARNING, "set_cull_mode: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if((cull_mode < sgl_no_cull) || (cull_mode > sgl_cull_clockwise))
	{
		DPFDEV ((DBG_WARNING, "set_cull_mode: Invalid Params!"));
		nError = sgl_err_bad_parameter;
	}
	/*
	// Else all is ok, so set the cull mode
	*/
	else
	{
		pMesh->CullMode = cull_mode;
	}
	SglError (nError);
	
}

/*===========================================
 * Function:    sgl_add_vertices
 *===========================================
 *
 * Scope:	      SGL (API)
 *
 * Purpose:	  Add some vertices to the current mesh
 *
 * Params:	    int nNumToAdd: number of vertices to add to list
 *			    sgl_vector *vVertices: vertex position info
 *			    sgl_vector *vNormals: normal of object at vertex
 *			    sgl_2d_vec *v2UVs: texturing uv data
 *
 * Return:	    void - but may cause an error condition in SGL
 *===========================================================================*/
void CALL_CONV sgl_add_vertices ( int nNumToAdd, sgl_vector *vVertices, 
								  sgl_vector *vNormals, sgl_2d_vec *v2UVs )
{
	int nError;
	PMESH_NODE_STRUCT pMesh;

	TidyUpCurrentState ();
	CheckCurrentMesh ();
	pMesh = dlUserGlobals.pCurrentMesh;
	
	if (!pMesh)
	{
		/* This should not be possible */
		DPF ((DBG_WARNING, "sgl_add_vertices: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nNumToAdd < 1)
	{
		DPFDEV ((DBG_WARNING, "sgl_add_vertices: bad number of vVertices to add: %d", nNumToAdd));
		nError = sgl_err_bad_parameter;
	}
	else if (!vVertices)
	{
		DPFDEV ((DBG_WARNING, "sgl_add_vertices: NULL ptr to vertex array"));
		nError = sgl_err_bad_parameter;
	}
	else
	{
		int				k, nVerticesInList;
		sgl_int8		Flags = 0;
		PDLVERTEXDATA 	pVS;
		PVERTEX			pV;

		ASSERT (pMesh->node_hdr.n16_node_type == nt_mesh);

		/* allocate dl vertex data array */

		nVerticesInList = ListGetItemsInList (pMesh->Vertices);

		if (!pMesh->pVertexData)
		{
			pMesh->pVertexData = SGLMalloc (sizeof (VERTEX) * (nVerticesInList + nNumToAdd));
		}
		else
		{
			void *pNew;
			
			pNew = SGLRealloc (pMesh->pVertexData, sizeof (VERTEX) * (nVerticesInList + nNumToAdd));
			
			if (pNew)
			{
				pMesh->pVertexData = pNew;
			}
			else
			{
				DPF ((DBG_ERROR, "Unable to allocate DLVertexData"));
				SglError (sgl_err_no_mem);
				return;
			}
		}
			
		/* base address of dl vertex data may have changed */

		pVS = ListFindItemFast (pMesh->Vertices, 0);
		pV = pMesh->pVertexData;

		for (k = 0; k < nVerticesInList; ++k, ++pVS, ++pV)
		{
			pVS->pV = pV;
		}

		/* set up the vertex flags according to the data we are given */

		if (vNormals)
		{
			Flags |= VERTEX_HAS_NORMAL;
		}

		if (v2UVs)
		{
			Flags |= VERTEX_HAS_UV;
		}

		/* optimistic! */
		
		nError = sgl_no_err;

		/* add each vertex in turn and copy the data into the vertex structure */

		for (k = 0; (nError == sgl_no_err) && (k < nNumToAdd); ++k)
		{
			PDLVERTEXDATA pDLVertexData;

			if (gOptimise)
			{
				OPTVERTEX OV;

				int *pMap = ArrayAddItem (&pMesh->VertexIDMap);

				ASSERT (pMap);
				
				OV.Vertex = vVertices[k];
				OV.Normal = (vNormals) ? vNormals[k] : NULL;
				OV.UV = (v2UVs) ? v2UVs[k] : NULL;
				
				pDLVertexData = ListFindItem (pMesh->Vertices, 
											  FindIdenticalVertex, (sgl_uint32) &OV);

				if (!pDLVertexData)
				{
					pDLVertexData = ListAddItem (pMesh->Vertices);
					if (!pDLVertexData)
					{
						DPF ((DBG_ERROR, 
							  "sgl_add_vertices: error adding vertex to list (%d of %d)", 
							  k, nNumToAdd));
						nError = sgl_err_no_mem;
						SglError (nError);
						return;
					}
					else
					{
						pMesh->nVertices++;
					}
				}
				else
				{
					pMesh->nRemapped++;
				}

				ASSERT (pDLVertexData);
				
				*pMap = ListGetItemID (pMesh->Vertices, pDLVertexData);
			}
			else
			{
				pDLVertexData = ListAddItem (pMesh->Vertices);
				if (!pDLVertexData)
				{
					DPF ((DBG_ERROR, 
						  "sgl_add_vertices: error adding vertex to list (%d of %d)", 
						  k, nNumToAdd));
					nError = sgl_err_no_mem;
					SglError (nError);
					return;
				}
				else
				{
					pMesh->nVertices++;
				}
			}


			pDLVertexData->pV = pV++;
			pDLVertexData->Flags  = Flags;
			
			memcpy (pDLVertexData->pV->vVertex, vVertices[k], 
					sizeof (sgl_vector));
			
			if (vNormals)
			{
				memcpy (pDLVertexData->vNormal, vNormals[k], 
						sizeof (sgl_vector));
				VecNormalise (pDLVertexData->vNormal);
			}
			
			if (v2UVs)
			{
				memcpy (pDLVertexData->v2UV, v2UVs[k], sizeof (sgl_2d_vec));
			}
			
			UpdateBoundingBoxInfo (&pMesh->CentBBox, vVertices[k]);
			/* successfully added a new vertex if we got here */
		}
	}

	SglError (nError);
}

/*===========================================
 * Function:    sgl_add_face
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Add a face to the current mesh
 *
 * Params:	    int num_face_points: number of vertices face has
 *			    int *vertex_ids: ids of vertices
 *
 * Return:	    void
 *========================================================================================*/
void CALL_CONV sgl_add_face ( int nNumToAdd, int *pnVertexIDs )
{
	int nError= sgl_no_err;
	int nMappedIDs[4], *pIDList = pnVertexIDs;

	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();
	
	#if ENUMERATE_MESHES
		if(dlUserGlobals.pCurrentMesh->MeshNumber==174)
		{
			DPF((DBG_MESSAGE, "At funny mesh"));
		}
	#endif


	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_add_face: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (!pnVertexIDs)
	{
		DPFDEV ((DBG_WARNING, "sgl_add_face: NULL ptr to vertex array"));
		nError = sgl_err_bad_parameter;
	}
	else if ((pnVertexIDs[0] < 0) ||
			 (pnVertexIDs[1] < 0) ||
			 (pnVertexIDs[2] < 0) ||
			 ((nNumToAdd == 4) && (pnVertexIDs[3] < 0)))
	{
		DPFDEV ((DBG_WARNING, "sgl_add_face: face refers to negative vertices"));
		nError = sgl_err_bad_parameter;
	}
	else
	{
		int nMaxVertexID;
		
		ASSERT(CheckFaceCount(pMesh));

		nMaxVertexID = ListGetItemsInList (pMesh->Vertices);

		if ((pnVertexIDs[0] > nMaxVertexID) ||
			(pnVertexIDs[1] > nMaxVertexID) ||
			(pnVertexIDs[2] > nMaxVertexID) ||
			((nNumToAdd == 4) && (pnVertexIDs[3] > nMaxVertexID)))
		{
			DPFDEV ((DBG_WARNING, "sgl_add_face: face refers to non-existent vertices"));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			PFACE   pFace;
			int 	k;

			ASSERT (pMesh->node_hdr.n16_node_type == nt_mesh);

			/* create a new face and add it to the list */

			pFace = ListAddItem (pMesh->Faces);

			if (!pFace)
			{
				DPF ((DBG_ERROR, "sgl_add_face: error adding nFace to list"));
				nError = sgl_err_no_mem;
			}
			else
			{
				if (gOptimise)
				{
					for (k = 0; k < nNumToAdd; ++k)
					{
						int *pMap = (int *) ArrayGetItem (&pMesh->VertexIDMap, pnVertexIDs[k]);

						if (pMap)
						{
							nMappedIDs[k] = *pMap;
						}
						else
						{
							ASSERT (FALSE);
						}
					}

					pIDList = nMappedIDs;
				}

				if (nNumToAdd == 4)
				{
					PDLVERTEXDATA 	pV[4];
					sgl_vector 	vNorm1, vNorm2, Vec1, Vec2;
					float		Dot;
					int			nV0 = 0;
					int			nV1 = 1;
					int			nV2 = 2;
					int			nV3 = 3;
					int 		nRotation = 0;
					sgl_bool	fSuccess = FALSE;
#if 0
					int			ANDedVertexFlags;
#endif

					/* check if we have dodgy non-coplanar quad */
					
					pV[0] = ListFindItemFast (pMesh->Vertices, pIDList[0]);
					pV[1] = ListFindItemFast (pMesh->Vertices, pIDList[1]);
					pV[2] = ListFindItemFast (pMesh->Vertices, pIDList[2]);
					pV[3] = ListFindItemFast (pMesh->Vertices, pIDList[3]);

					/* there is absolutely no point
					** to these assertions as ListFindItemFast never
					** returns a NULL, as it just calculates its return
					** without checking its validity
					*/
					ASSERT (pV[0]);
					ASSERT (pV[1]);
					ASSERT (pV[2]);
					ASSERT (pV[3]);
					
#if 0
					
					ANDedVertexFlags = pV[0]->Flags & pV[1]->Flags & pV[2]->Flags & pV[3]->Flags;

					if (ANDedVertexFlags & VERTEX_HAS_NORMAL)
					{
						float I[4], dI[3], f1overY1X2minusX1Y2, a, b, error;
						sgl_vector	N0, N1, N2, N3;
						sgl_vector  L = {0.577f, 0.577f, 0.577f};

						VecSub (pV[1]->pV->vVertex, pV[0]->pV->vVertex, Vec1);
						VecSub (pV[2]->pV->vVertex, pV[0]->pV->vVertex, Vec2);
						VecSub (pV[3]->pV->vVertex, pV[0]->pV->vVertex, Vec3);
						
						VecCopy (pV[0]->pV->vNormal, N0);
						VecCopy (pV[1]->pV->vNormal, N1);
						VecCopy (pV[2]->pV->vNormal, N2);
						VecCopy (pV[3]->pV->vNormal, N3);
						
						VecNormalise (N0);
						VecNormalise (N1);
						VecNormalise (N2);
						VecNormalise (N3);
						
						I[0] = DotProd (L, N0);
						I[1] = DotProd (L, N1);
						I[2] = DotProd (L, N2);
						I[3] = DotProd (L, N3);
						
						dI[0] = I[1] - I[0];
						dI[1] = I[2] - I[0];
						dI[2] = I[3] - I[0];
						
						/* work out delta Is on an arbitrary vector L */
						
						f1overY1X2minusX1Y2 = 1.0f / (Vec1[1]*Vec2[0] - Vec1[0]*Vec2[1]);
						a = (dI[1]*Vec1[1] - dI[0]*Vec2[1]) * f1overY1X2minusX1Y2;
						b = (dI[0]*Vec2[0] - dI[1]*Vec1[0]) * f1overY1X2minusX1Y2;
						
						/* see if the third checks out ... */
						
						error = sfabs (sfabs (a*Vec3[0] + b*Vec3[1]) - sfabs (dI[2]));
						
						if (error > 0.01f)
						{
							int nIDs[4];
							int nWarning;

							DPF ((DBG_WARNING, "Quad normals inconsistent - splitting"));

							nWarning = sgl_warn_noncoplanar_quad;
							SglWarning( nWarning );
							nIDs[0] = pnVertexIDs[0];
							nIDs[1] = pnVertexIDs[1];
							nIDs[2] = pnVertexIDs[2];
							
							nError = set_face_data (pMesh, pFace, 3, nIDs, TRUE);

							UpdateLocalMaterialUsage( pMesh->Materials, TRUE, 0);

							/*
							// Now add the other half of the quad by calling this
							// routine recursively
							*/			
							if (nError >= sgl_no_err)
							{
								/*
								// Only update local materials if addition was successful.
								*/
								nIDs[0] = pnVertexIDs[0];
								nIDs[1] = pnVertexIDs[2];
								nIDs[2] = pnVertexIDs[3];
								
								sgl_add_face (3, nIDs);
							}
							
							fSuccess = TRUE;
						}
						else
						{
							DPF ((DBG_VERBOSE, "Quad normals consistent\n"));
						}
					}
#endif
					while (!fSuccess && (nRotation <= 3))
					{
						VecSub (pV[nV1]->pV->vVertex, pV[nV0]->pV->vVertex, Vec1);
						VecSub (pV[nV2]->pV->vVertex, pV[nV0]->pV->vVertex, Vec2);
						CrossProd (Vec1, Vec2, vNorm1);
						
						VecSub (pV[nV2]->pV->vVertex, pV[nV0]->pV->vVertex, Vec1);
						VecSub (pV[nV3]->pV->vVertex, pV[nV0]->pV->vVertex, Vec2);
						CrossProd (Vec1, Vec2, vNorm2);
						
						if ((sfabs (vNorm1[0]) + sfabs (vNorm1[1]) + sfabs (vNorm1[2]) != 0) &&
							(sfabs (vNorm2[0]) + sfabs (vNorm2[1]) + sfabs (vNorm2[2]) != 0))
						{
							int nIDs[4];

							fSuccess = TRUE;
							
							VecNormalise (vNorm1);
							VecNormalise (vNorm2);

							Dot = DotProd (vNorm1, vNorm2);
			
							if (Dot < gfCoPlanarity)
							{
								/* non-coplanar data. 
								   Cut into two triangles */
								
								nIDs[0] = pIDList[nV0];
								nIDs[1] = pIDList[nV1];
								nIDs[2] = pIDList[nV2];

								DPFDEV ((DBG_WARNING, "Non coplanar quad - cutting into trangles (%f)", Dot));
								
								nError = set_face_data (pMesh, pFace, 3, nIDs, TRUE);
							
								if(nError != sgl_no_err)
								{
									SglWarning( sgl_warn_noncoplanar_quad );
								}

								UpdateLocalMaterialUsage( pMesh->Materials, TRUE, 0);

								/*
								// Now add the other half of the quad by calling this
								// routine recursively
								*/			
								if (nError >= sgl_no_err)
								{
									/*
									// Only update local materials if addition was successful.
									*/

									nIDs[0] = pnVertexIDs[nV0];
									nIDs[1] = pnVertexIDs[nV2];
									nIDs[2] = pnVertexIDs[nV3];
								
									sgl_add_face (3, nIDs);
									nError = sgl_warn_noncoplanar_quad;
								}
							}
							else
							{
								/* use rotated values to ensure a valid normal */
								
								nIDs[0] = pIDList[nV0];
								nIDs[1] = pIDList[nV1];
								nIDs[2] = pIDList[nV2];
								nIDs[3] = pIDList[nV3];
								
								nError = set_face_data (pMesh, pFace, nNumToAdd, nIDs, TRUE);
								if (nError >= 0)
								{
									UpdateLocalMaterialUsage( pMesh->Materials, TRUE, 0);
								}

							}
						}
						else
						{
							/* bad normal - rotate */

							DPF ((DBG_WARNING, "Quad with invalid normal: attempt %d", nRotation));

							if (++nV0 > 3) nV0 = 0;
							if (++nV1 > 3) nV1 = 0;
							if (++nV2 > 3) nV2 = 0;
							if (++nV3 > 3) nV3 = 0;
							++nRotation;
						}
					}/*end while*/

					if (!fSuccess)
					{
						/* bad normal - quad - kill it */

						DPFDEV ((DBG_WARNING, "Quad with invalid normal"));

						ListRemoveItem (pMesh->Faces, NULL, ListGetItemID (pMesh->Faces, pFace));
						nError = sgl_err_bad_parameter;
					}
				}
				/*
				// Else is a triangle
				*/
				else
				{
					/* triangle - be happy */
					nError = set_face_data (pMesh, pFace, nNumToAdd, pIDList, TRUE);

					if (nError >= 0)
					{
						UpdateLocalMaterialUsage( pMesh->Materials, TRUE, 0);
					}
				}/*end if quad/triangle testing*/

			}/*end if/else allocated face ok*/

			/*
			// Check that stuff is consistent
			*/
			ASSERT(CheckFaceCount(pMesh));
		}
	}

	if (nError >= 0)
	{
		static int  nWarnings = 0, nTotal = 0;

		nTotal++;

		if (SglGetLastWarning() == sgl_warn_colinear_face_points)
		{
			nWarnings++;

			DPF((DBG_WARNING,"sgl_add_face: %d colinear face warning(s) given for %d faces",
			nWarnings, nTotal));
		}
		
	} /* if (nError >= 0) */
	
	SglError (nError);

} /* sgl_add_face */


/*===========================================
 * Function:    sgl_num_vertices
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Count vertices in current mesh
 *
 * Params:	    void
 *
 * Return:	    number of vertices or -ve error
 *====================================================================*/
int CALL_CONV sgl_num_vertices ( void )
{
	int nError;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_num_vertices: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else
	{
		/* call list function to get number of items in list */

		nError = pMesh->nVertices;
	}
	
	return (SglError (nError));
}

/*===========================================
 * Function:    sgl_num_faces
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Count faces in current mesh
 *
 * Params:	    void
 *
 * Return:	    number of faces or -ve error
 *===============================================================*/
int CALL_CONV sgl_num_faces ( void )
{
	int nError;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_num_faces: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else
	{
		/* call list function to get number of items in list */

		nError = ListGetItemsInList (pMesh->Faces);
	}
	
	return (SglError (nError));
}

/*===========================================
 * Function:    sgl_num_face_vertices
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Count vertices in given face in current mesh
 *
 * Params:	    int face: ID of face in current mesh
 *
 * Return:	    number of vertices or -ve error
 *==================================================================*/
int CALL_CONV sgl_num_face_vertices ( int nFace )
{
	int nError;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_num_face_vertices: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nFace < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_num_face_vertices: bad nFace ID %d", nFace));
		nError = sgl_err_bad_parameter;
	}
	else
	{


		if (nFace >= ListGetItemsInList(pMesh->Faces))
		{
			DPFDEV ((DBG_WARNING, "sgl_num_face_vertices: bad nFace ID %d", nFace));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			PFACE pFace;

			/* return number of vertices in face */
			pFace = ListFindItemFast (pMesh->Faces, nFace);
			nError = pFace->nVertices;
		}
	}
	
	return (SglError (nError));
}

/*===========================================
 * Function:    sgl_get_vertex
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Get parameters of given vertex in current mesh
 *
 * Params:	    int vertex: vertex to query
 *			    sgl_vector position: position data placed in here
 *			    sgl_vector normal: normal data placed in here
 *			    sgl_2d_vec uv: smooth shading uv's placed in here
 *
 * Return:	    void
 *====================================================================*/
int CALL_CONV sgl_get_vertex ( int nVertex, sgl_vector vPosition,
							   sgl_vector vNormal, sgl_2d_vec v2UV )
{
	int nError;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	ASSERT (vPosition);

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_get_vertex: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nVertex < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_get_vertex: bad vertex ID %d", nVertex));
		nError = sgl_err_bad_parameter;
	}
	else
	{
		/* get the vertex from the list */

		if (nVertex >= ListGetItemsInList(pMesh->Vertices))
		{
			DPFDEV ((DBG_WARNING, "sgl_get_vertex: bad nFace ID %d", nVertex));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			PDLVERTEXDATA pDLVertexData;
			nError = sgl_no_err;

			pDLVertexData = ListFindItemFast (pMesh->Vertices, nVertex);
			/* copy the data into the user's buffers */

			memcpy (vPosition, pDLVertexData->pV->vVertex, sizeof (sgl_vector));
			
			/* check that the user has given us the buffers he was supposed to */
			
			if (pDLVertexData->Flags & VERTEX_HAS_NORMAL)
			{
				if (!vNormal)
				{
					DPFDEV ((DBG_WARNING, "sgl_get_vertex: NULL normal parameter"));
					nError = sgl_err_bad_parameter;
				}
				else
				{
					memcpy (vNormal, pDLVertexData->vNormal, sizeof (sgl_vector));
				}
			}

			if (pDLVertexData->Flags & VERTEX_HAS_UV)
			{
				if (!v2UV)
				{
					DPFDEV ((DBG_WARNING, "sgl_get_vertex: NULL uv parameter"));
					nError = sgl_err_bad_parameter;
				}
				else
				{
					memcpy (v2UV, pDLVertexData->v2UV, sizeof (sgl_2d_vec));
				}
			}
		}
	}
	
	return (SglError (nError));
}

/*===========================================
 * Function:    sgl_set_vertex
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Set parameters of given vertex in current mesh
 *
 * Params:	    int vertex: vertex to update
 *			    sgl_vector position: position data
 *			    sgl_vector normal: normal data
 *			    sgl_2d_vec uv: smooth shading uv's
 *
 * Return:	    void
 *======================================================================*/
void CALL_CONV sgl_set_vertex ( int nVertex, sgl_vector vPosition,
					 			sgl_vector vNormal, sgl_2d_vec v2UV )
{
	int nError, i, k, nFaces;
	PFACE pFace;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	ASSERT (vPosition);

	TidyUpCurrentState ();

	if(!vPosition)
	{
		DPFDEV ((DBG_WARNING, "sgl_set_vertex: invalid pointer to vPostion"));
		nError = sgl_err_bad_parameter;
	}
	else if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_set_vertex: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nVertex < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_set_vertex: bad vertex ID %d", nVertex));
		nError = sgl_err_bad_parameter;
	}
	else if (gOptimise)
	{
		DPF ((DBG_ERROR, "sgl_set_vertex: You cannot set vertices in an optimised mesh! Sorry."));
		nError = sgl_err_bad_parameter;
	}
	else
	{
		PDLVERTEXDATA pDLVertexData;

		ASSERT(CheckFaceCount(pMesh));

		/* get ptr to vertex structure from list */
		if (nVertex >= ListGetItemsInList(pMesh->Vertices))
		{
			DPFDEV ((DBG_WARNING, "sgl_set_vertex: bad nFace ID %d", nVertex));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			/* reinitialise structure and copy new data */
			pDLVertexData = ListFindItemFast (pMesh->Vertices, nVertex);


			/* leave the flags as they are, as currently vertices cannot
			** have their types changed
			*/
			/* pDLVertexData->Flags = 0;*/

			memcpy (pDLVertexData->pV->vVertex, vPosition, sizeof (sgl_vector));
			
			if (vNormal && (pDLVertexData->Flags & VERTEX_HAS_NORMAL))
			{
				memcpy (pDLVertexData->vNormal, vNormal, sizeof (sgl_vector));
				VecNormalise (pDLVertexData->vNormal);
				/* pDLVertexData->Flags |= VERTEX_HAS_NORMAL;*/
			}

			if (v2UV && (pDLVertexData->Flags & VERTEX_HAS_UV))
			{
				memcpy (pDLVertexData->v2UV, v2UV, sizeof (sgl_2d_vec));
				/* pDLVertexData->Flags |= VERTEX_HAS_UV;*/
			}

			UpdateBoundingBoxInfo (&pMesh->CentBBox, vPosition);

			pDLVertexData->Flags |= VERTEX_UPDATED;
			pMesh->nNeedsUpdating = TRUE;

			nError = sgl_no_err;

			ASSERT(CheckFaceCount(pMesh));

			nFaces = ListGetItemsInList(pMesh->Faces);
			for(i = 0; i<nFaces; i++)
			{
				pFace = ListFindItemFast(pMesh->Faces, i);
				for (k = 0; k < pFace->nVertices; k++)
				{
					if(pFace->pnVertexIDs[k] == nVertex)
					{
						nError = set_face_data (pMesh, pFace, 
												pFace->nVertices, 
												pFace->pnVertexIDs, FALSE);
						break;
					}
				}
				/* inner loop may have reported an error which 
				** we can catch here - break if an error
				*/
				if(nError != sgl_no_err)
					break;
			}
		}
	}
	
	SglError (nError);
}

/*===========================================
 * Function:    sgl_get_face
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Gets face data for given face
 *
 * Params:	    int face: ID of face in current mesh
 *			    int *vertex_indices: buffer in which to place vertex info
 *
 * Return:	    void
 *=====================================================================*/
int CALL_CONV sgl_get_face ( int nFace, int *pnVertexIDs )
{
	int nError=sgl_no_err;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_get_face: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nFace < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_get_face: bad nFace ID %d", nFace));
		nError = sgl_err_bad_parameter;
	}
	else if (!pnVertexIDs)
	{
		DPFDEV ((DBG_WARNING, "sgl_get_face: null vertex IDs array"));
		nError = sgl_err_bad_parameter;
	}
	else
	{

		/* get face ptr from list */
		if( nFace >= ListGetItemsInList(pMesh->Faces))
		{
			DPFDEV ((DBG_WARNING, "sgl_delete_face: bad nFace ID %d", nFace));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			PFACE pFace;

			/* get face ptr */
			pFace = ListFindItemFast (pMesh->Faces, nFace);

			/* copy new vertex data */
			memcpy (pnVertexIDs, pFace->pnVertexIDs, 
					pFace->nVertices * sizeof (int));
		}
	}
	
	return (SglError (nError));
}

/*===========================================
 * Function:    sgl_set_face
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Modify a face in the current mesh
 *
 * Params:	    int face: face to modify
 *			    int num_face_points: number of vertices face has
 *			    int *vertex_ids: ids of vertices
 *
 * Return:	    void
 *========================================================================================*/
void CALL_CONV sgl_set_face ( int nFace, int nNumVertices, 
							  int *pnVertexIDs )
{
	int nError;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_set_face: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nFace < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_set_face: bad nFace ID %d", nFace));
		nError = sgl_err_bad_parameter;
	}
	else if (!pnVertexIDs)
	{
		DPFDEV ((DBG_WARNING, "sgl_set_face: null vertex IDs array"));
		nError = sgl_err_bad_parameter;
	}
	else if ((pnVertexIDs[0] < 0) ||
			 (pnVertexIDs[1] < 0) ||
			 (pnVertexIDs[2] < 0) ||
			 ((nNumVertices == 4) && (pnVertexIDs[3] < 0)))
	{
		DPFDEV ((DBG_WARNING, "sgl_set_face: face refers to negative vertices"));
		nError = sgl_err_bad_parameter;
	}
	else if ((pnVertexIDs[0] >= pMesh->nVertices) ||
			 (pnVertexIDs[1] >= pMesh->nVertices) ||
			 (pnVertexIDs[2] >= pMesh->nVertices) ||
			 ((nNumVertices == 4) && (pnVertexIDs[3] >= pMesh->nVertices)))
	{
		DPFDEV ((DBG_WARNING, "sgl_set_face: face refers to non-existent vertices"));
		nError = sgl_err_bad_parameter;
	}
	else
	{
		ASSERT(CheckFaceCount(pMesh));

		/* get face ptr from list */
		if( nFace >= ListGetItemsInList(pMesh->Faces))
		{
			DPFDEV ((DBG_WARNING, "sgl_set_face: bad nFace ID %d", nFace));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			int k;
			PFACE pFace;

			/* get face ptr from list */
			pFace = ListFindItemFast (pMesh->Faces, nFace);
			
			/* remove old edge data from adjacency info */

			for (k = 0; k < pFace->nVertices; ++k)
			{
				int nEdgeID = pFace->pnEdgeIDs[k];
				
				RemoveEdgeFromAdjacencyInfo (pMesh, nEdgeID);
			}

			if (gOptimise)
			{
				for (k = 0; k < nNumVertices; ++k)
				{
					int *pMap = (int *) ArrayGetItem (&pMesh->VertexIDMap, pnVertexIDs[k]);

					if (pMap)
					{
						pnVertexIDs[k] = *pMap;
					}
					else
					{
						ASSERT (FALSE);
					}
				}
			}

			nError = set_face_data (pMesh, pFace, nNumVertices, pnVertexIDs, TRUE);

			ASSERT(CheckFaceCount(pMesh));
		}
	}
	
	SglError (nError);

}

/*===========================================
 * Function:    sgl_delete_vertex
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Remove a vertex from the current mesh; also removes faces that
 *			    refer to this vertex, and ripples down face and vertex IDs
 *
 * Params:	    int vertex: vertex to delete
 *
 * Return:	    void
 *========================================================================================*/
void CALL_CONV sgl_delete_vertex ( int nVertex )
{
	int nError;
	int nFaces;
	PFACE pFace;
	PDLVERTEXDATA pVertex;
	int i,k;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;

	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_delete_vertex: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nVertex < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_delete_vertex: bad vertex ID %d", nVertex));
		nError = sgl_err_bad_parameter;
	}
	else
	{
		/* get vertex ptr from list */

		if (nVertex >= ListGetItemsInList(pMesh->Vertices))
		{
			DPFDEV ((DBG_WARNING, "sgl_delete_vertex: bad nFace ID %d", nVertex));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			/* delete the faces which refer to this vertex */
			nFaces = ListGetItemsInList(pMesh->Faces);
			for(i = 0; i<nFaces; i++)
			{
				pFace = ListFindItemFast(pMesh->Faces, i);
				for (k = 0; k < pFace->nVertices; k++)
				{
					if(pFace->pnVertexIDs[k] == nVertex)
					{
						sgl_delete_face(i);
						i--;
						nFaces--;
						break;
					}
				}
			}
			
			/* delete the vertex */
			ListRemoveItem(pMesh->Vertices, NULL, nVertex);

			/* move down all the data on the vertex array above the deleted vertex */
			if(pMesh->pVertexData)
			{
				for(i=nVertex; i< pMesh->nVertices-1; i++)
				{
					memcpy(&pMesh->pVertexData[i],&pMesh->pVertexData[i+1],
						   sizeof(VERTEX));
				}
			}
			pMesh->nVertices--;
			
			/* now fix up the pointers in the vertex list */
			for(i=nVertex; i<pMesh->nVertices; i++)
			{
				pVertex = ListFindItemFast(pMesh->Vertices, i);
				pVertex->pV = &pMesh->pVertexData[i];
			}
	
			/* decrement all vertex references above this vertex id */
			/* edge list first */
			for(i=0; i<pMesh->nEdges; i++)
			{
				ASSERT(!(pMesh->pEdgeData[i].nIDV1 == (sgl_uint32) nVertex));
				if(pMesh->pEdgeData[i].nIDV1> (sgl_uint32) nVertex)
					pMesh->pEdgeData[i].nIDV1--;

				ASSERT(!(pMesh->pEdgeData[i].nIDV2 == (sgl_uint32) nVertex));
				if(pMesh->pEdgeData[i].nIDV2> (sgl_uint32) nVertex)
					pMesh->pEdgeData[i].nIDV2--;
			}

			/* now the face list */
			nFaces = ListGetItemsInList(pMesh->Faces);
			for(i = 0; i<nFaces; i++)
			{
				pFace = ListFindItemFast(pMesh->Faces, i);
				for (k = 0; k < pFace->nVertices; k++)
				{
					ASSERT(!(pFace->pnVertexIDs[k] == nVertex));

					if(pFace->pnVertexIDs[k] > nVertex)
					{
						pFace->pnVertexIDs[k]--;
					}
				}
			}
		}
	}
	
	SglError (nError);
}

/*===========================================
 * Function:    sgl_delete_face
 *===========================================
 *
 * Scope:	      SGL recorder
 *
 * Purpose:	  Remove a face from the current mesh
 *
 * Params:	    int face: face to delete
 *
 * Return:	    void
 *=======================================================================*/
void CALL_CONV sgl_delete_face( int nFace )
{
	int nError, i;
	int nEdgeID;
	PMESH_NODE_STRUCT pMesh = dlUserGlobals.pCurrentMesh;


	TidyUpCurrentState ();

	if (!pMesh)
	{
		DPF ((DBG_WARNING, "sgl_delete_face: no current mesh selected!"));
		nError = sgl_err_no_mesh;
	}
	else if (nFace < 0)
	{
		DPFDEV ((DBG_WARNING, "sgl_delete_face: bad nFace ID %d", nFace));
		nError = sgl_err_bad_parameter;
	}
	else
	{
	    PFACE pFace;
		int nTotalFaces;

		ASSERT(CheckFaceCount(pMesh));

		nTotalFaces = ListGetItemsInList(pMesh->Faces);

		if( nFace >= nTotalFaces )
		{
			DPFDEV ((DBG_WARNING, "sgl_delete_face: bad nFace ID %d", nFace));
			nError = sgl_err_bad_parameter;
		}
		else
		{
			int k;

			/* get face ptr from list */
			pFace = ListFindItemFast (pMesh->Faces, nFace);

			/*
			// Update the Local Materials Table
			*/
			UpdateLocalMaterialUsage(pMesh->Materials, FALSE, nFace);

			/* 
			// update edge list and vertex references as we go along ... 
			*/

			/* 
			// do it here as putting it in OnDeleteMesh would take too long
			// when deleting the whole mesh 
			*/
			ASSERT((pFace->nVertices == 3) || (pFace->nVertices == 4));

			for (k = 0; k < pFace->nVertices; ++k)
			{
				nEdgeID = pFace->pnEdgeIDs[k];
				RemoveEdgeFromAdjacencyInfo (pMesh, nEdgeID);
			}
			
			/* remove it from list */

			ListRemoveItem (pMesh->Faces, NULL, nFace);

			/* move the texture shading and points data down the list */
			if(pMesh->pTextureData)
			{
				for(i=nFace; i< nTotalFaces-1; i++)
				{
					memcpy(&pMesh->pTextureData[i],&pMesh->pTextureData[i+1],
						   sizeof(CONV_TEXTURE_UNION));
				}
			}
			if(pMesh->pShadingData)
			{
				for(i=nFace; i< nTotalFaces-1; i++)
				{
					memcpy(&pMesh->pShadingData[i],&pMesh->pShadingData[i+1],
						   sizeof(CONV_SHADING_STRUCT));
				}
			}
			if(pMesh->pPointsData)
			{
				for(i=nFace; i< nTotalFaces-1; i++)
				{
					memcpy(&pMesh->pPointsData[i],&pMesh->pPointsData[i+1],
						   sizeof(CONV_POINTS_STRUCT));
				}
			}
			nError = sgl_no_err;

			ASSERT(CheckFaceCount(pMesh));
		}
	}
	
	SglError (nError);

}

/* end of $RCSfile: dlmesh.c,v $ */
