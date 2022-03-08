/**************************************************************************
 * Name         : d3dtsort.h
 * Title        : Direct 3D Translucency sorting for SGL
 * Author       : Michael Green
 * Created      : 30th August 1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Prototypes, defines and structures for DAG sorting
 *              : sorting routines. Created to save having a d3dreg.h,
 *				: d3dtri.h or a number of externs.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: d3dtsort.h,v $
 * Revision 1.10  1997/09/24  16:42:12  mjg
 * Updated the prototype for DoTheStuff.
 *
 * Revision 1.9  1997/09/17  13:27:23  mjg
 *  Removed Zs from TRANSTRI_STRUCT
 *
 * Revision 1.8  1997/09/15  16:07:52  mjg
 * Changed InitialiseTransortMemory prototype, so the function
 * could return a Success/Fail boolean.
 *
 * Revision 1.7  1997/09/09  16:41:27  mjg
 * Moved assembly file define to dtri.h
 *
 * Revision 1.6  1997/09/08  12:29:44  mjg
 * Changed assembly .
 *
 * Revision 1.5  1997/09/05  15:21:00  mjg
 * Increased (???) the maximum number of translucent triangles and
 * changed the prototype for DoTheStuff
 *
 * Revision 1.4  1997/09/04  13:58:49  mjg
 * Moved the definition of TRIANGLE_REFERENCE from d3dtsort.c and added
 * the members (depth, locks and children) from TRIANGLE_NODE to
 * the TRANSTRI_STRUCT structure.
 *
 * Revision 1.3  1997/09/02  14:12:05  mjg
 * Altered the prototype for FixForVignetting so Edwards' changes
 * would compile.
 *
 * Revision 1.2  1997/09/01  20:04:25  mjg
 * Added FixForVignetting prototype.
 *
 * Revision 1.1  1997/09/01  12:17:58  mjg
 * Initial revision
 *
 *
 *************************************************************************/

#ifndef __D3DTSORT_H__
#define __D3DTSORT_H__


/* Defines */
#define NUM_INDICES_IN_BLOCK 32
#define MAX_NUM_SCENE_TRIS 16000 /* Max number in Islands = 12689 */
#define MAX_NUM_REGION_TRIS 2048/*2048*/
#define MAX_NUM_INDEX_BLOCKS (MAX_NUM_REGION_TRIS * 300) / NUM_INDICES_IN_BLOCK


/* Structures */

/* Extra information, additional to ITRI structure */
typedef struct _itriplus_struct{
	float	fX[3];
	float	fY[3];
	float	fA;
	float	fB;
	float	fC;
} ITRIPLUS, *PITRIPLUS;

/* Details about each triangle kept until GenerateObjectPtr... */
typedef struct _transtri_struct 
{
	float fVerts[3][2];
	float fMeanZ;
	float fAdjoint[3][3];
	float fA;
	float fB;
	float fC;
	sgl_uint32 uObjPtr;
	struct _triangle_reference	*psChildren;
	sgl_uint16 usNumLocks;
	sgl_uint16 usMaxDepth;			/* Max depth of subgraph below this node */
} TRANSTRI_STRUCT, *PTRANSTRI_STRUCT;

typedef struct _triangle_reference
{
	PTRANSTRI_STRUCT			psTriNode;
	struct _triangle_reference	*psNext;
} TRIANGLE_REFERENCE, *PTRIANGLE_REFERENCE;

/* For each region, record which triangles are in it */
typedef struct _transtriindex_struct 
{
	sgl_uint16 	usIndex[NUM_INDICES_IN_BLOCK]; 
	struct _transtriindex_struct *pNext;
} TRANSTRIINDEX_STRUCT, *PTRANSTRIINDEX_STRUCT;


/* Prototypes */
void DoTheStuff( sgl_uint32 uPolys, PTRANSTRIINDEX_STRUCT *prTriangleIndicesList );
sgl_uint32* Traverse( sgl_uint32 nPolys, sgl_uint32* pAddr, sgl_uint32 *rpPlanes );
sgl_uint32 FixForVignetting( sgl_uint32 uPolys );

sgl_bool InitialiseTransortMemory( void );
void FinalizeTransortMemory( void );


extern void AddRegionD3DTransTris( PITRI *rpTri, int nXYDataInc,
								   PITRIPLUS *rpTriPlus, sgl_uint32 PlanesPerPoly,
								   int nPolys, sgl_uint32 ISPAddr );

extern void AddRegionD3DTransTrisExtra( PITRI *rpTri, int nXYDataInc,
							   			PITRIPLUS *rpTriPlus, sgl_uint32 PlanesPerPoly,
									    int nPolys, sgl_uint32 ISPAddr,
							     		PFOGHIGHLIGHT pFogHighlight );

extern int PackISPD3DTriTrans(PITRI rpTri, PITRIPLUS rpTriPlus, int nTriangles, 
							  sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement);
extern int PackISPD3DTriTransExtra (PITRI rpTri, PITRIPLUS rpTriPlus,
							 		PIMATERIAL rpMat, int nTriangles, 
							 		sgl_uint32 TSPAddr, sgl_uint32 TSPIncrement);



#endif	/* __D3DTSORT_H__ */
/* end of $RCSfile: d3dtsort.h,v $ */
