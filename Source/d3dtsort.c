/**************************************************************************
 * Name         : d3dtsort.c
 * Title        : Direct3D translucency sorting
 * Author       : John Russell and Michael Green
 * Created      : 27/08/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Takes all the translucent triangles for a region and
 *				  sorts them by building a Directed Acyclic Graph, thereby
 *				  giving a correct Z ordering and minium number of passes.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: d3dtsort.c,v $
 * Revision 1.18  1997/09/24  16:46:07  mjg
 * Tedious optimisations:
 * 1) Some functions are no INLINE
 * 2) Reordered some code in LineSplitsVertices
 * 3) Unrolled some code in Traverse to remove stalls
 *
 * Revision 1.17  1997/09/19  09:20:25  mjg
 * Added a couple of defines and uped the uNumShared value from 3 to 5
 *
 * Revision 1.16  1997/09/17  13:26:53  mjg
 * Removed Zs from TRANSTRI_STRUCT
 *
 * Revision 1.15  1997/09/16  18:44:01  mjg
 * Now releases transorting memory if not to be used.
 *
 * Revision 1.14  1997/09/16  17:23:00  mjg
 * Implemented the CreatePasses sorting method - it's dead good!
 *
 * Revision 1.13  1997/09/16  09:49:09  mjg
 * Added success return from InitTransortMem. and added some new stuff
 * from John, not used at the moment.
 *
 * Revision 1.12  1997/09/11  15:24:04  mjg
 * 1. Reverted the hash function, as the previous one had an 888 clock
 *    cycle penalty every time it was used.
 * 2. Exapnded, unrolled and optimised the maximum and minimum X and Y
 *    calculations.
 * 3. Expanded the Sgn function in LineSplitsVerts as the compiler didn't
 *
 * Revision 1.11  1997/09/10  22:30:23  mjg
 * Added the ability to select the number of passes (per region)
 *
 * Revision 1.10  1997/09/10  19:13:42  mjg
 * John changed the algorithm to partition the case when there are far too
 * many triangles.
 *
 * Revision 1.9  1997/09/09  17:46:13  mjg
 * New AllIntersects function.
 *
 * Revision 1.8  1997/09/08  12:29:26  mjg
 * Changed new/old sort criterion.
 *
 * Revision 1.7  1997/09/06  15:57:46  mjg
 * No, new hashing function didn't work, so I've reverted it.
 *
 * Revision 1.6  1997/09/05  16:46:57  mjg
 * Now does a quick, depth-based sort if there are too many triangles and
 * the Hash function has been minimised.
 *
 * Revision 1.5  1997/09/04  14:00:56  mjg
 * Bit of an epic one:
 * 1. Removed structure defines, TRIANGLE_NODE is now part of
 *    TRANSTRI_STRUCT and TRIANGLE_REFERENCE is in d3dtsort.h
 * 2. TriNodes is now a list of pointers to structure, not
 *    structures that point to triangles
 * 3. Made some functions INLINE and even more static
 * 4. John R. reworked ChooseSplitLine
 * 5. Hash Table is no longer reset every frame, a count of tiles
 *    processed is kept so that the hash function keeps changing
 * 6. Threw in a couple of FLOAT_TO_LONG for fans of the
 *    Pentium floating point compare.
 *
 * Revision 1.4  1997/09/02  15:35:30  erf
 * Use defines recently moved to dregion.h .
 *
 * Revision 1.3  1997/09/02  13:15:25  erf
 * Fixed vignetting fix for new sorting algorithm. We now count total planes
 * correctly.
 *
 * Revision 1.2  1997/09/01  20:01:21  mjg
 * Added count of children during DoDFS so that the number of passes can be
 * use to create a sort of 'vignette fix' .
 *
 * Revision 1.1  1997/09/01  13:03:59  mjg
 * Initial revision
 *
 * 
 *************************************************************************/

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "sgl_math.h"
#include "sglmem.h"
#include "pvrlims.h"
#include "dtri.h"
#include "dregion.h"
#include "d3dtsort.h"

#if DAG_TRANS_SORTING
/************************************************************************
	My local functions
************************************************************************/

PTRIANGLE_REFERENCE NewTriangleReference( void );

void Intersect( PTRANSTRI_STRUCT *ppsTriNodes, PTRANSTRI_STRUCT *ppsLeftStack, PTRANSTRI_STRUCT *ppsRightStack, unsigned bIsAscending, unsigned uNumTris, unsigned uLevel );

#if FOUL_OR_NEWFOUL_ALLINTERSECTS
	void FoulAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, sgl_uint32 uNumItris ); 
	void NewFoulAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris );
#endif /*FOUL_OR_NEWFOUL_ALLINTERSECTS*/

void NewerFoulAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris );
/* Other prototypes in D3Dreg.h */

/************************************************************************
	My local structures
************************************************************************/

typedef struct _hash_struct
{
	PTRANSTRI_STRUCT p[2];
	sgl_uint16 usRef;
	sgl_uint16 uResult;
} HASH_STRUCT;


typedef struct _LOCAL_REF_
{
	struct _LOCAL_NODE_ *psLocalNode;
	struct _LOCAL_REF_ *psNext;
} LOCAL_REF, *PLOCAL_REF;

typedef struct _LOCAL_NODE_
{
	PTRANSTRI_STRUCT psTriNode;
	struct _LOCAL_NODE_ *psMarkedBy;
	PLOCAL_REF psParents;
	PLOCAL_REF psChildren;
} LOCAL_NODE, *PLOCAL_NODE;

/************************************************************************
	My local defines
************************************************************************/

#define DUMP_TRI_DATA 0
#define DUMP_PATH "C:\\Windows\\Desktop\\"

/* For the local nodes and refs */
#define Disjoint 0
#define AinfrontofB 1
#define BinfrontofA 2

#define A 0
#define B 3
#define C 6
#define X 0
#define Y 1
#define Z 2


/* For CreatePasses */
#define LOCAL_REFS 10000
#define LOCAL_NODES 1000

#define TOP_FEW 10
#define NUM_PASSES_PER_PARTITION 10

#define HASH_SIZE 1024
#define NUM_PPSTRINODEPTRS	(MAX_NUM_REGION_TRIS * 3)

/************************************************************************
	Globals from other files (d3dreg.c)
************************************************************************/

extern TRANSTRI_STRUCT *gpTransTris; /* Array of triangles */
#if FOUL_OR_NEWFOUL_ALLINTERSECTS
	extern sgl_uint16 guTransTriCounter;
#endif /*FOUL_OR_NEWFOUL_ALLINTERSECTS*/
extern sgl_uint32 DummyTransFlushData; /* Data for packing */
extern sgl_uint32 DummyTransData;
extern sgl_uint32 DummyFlushData;
extern sgl_uint32 DummyObjDataLarge;

extern sgl_uint32	nMaxPassCount; /* Defined in PVRD.c  */

/************************************************************************
	Globals visible only in this file
************************************************************************/

/* Edge colourings for the DFS algorithm */
const sgl_uint16 White	= 0x0000;	/* Unvisited */
const sgl_uint16 Grey	= 0x1000;	/* Currently being explored */
const sgl_uint16 Black	= 0x2000;	/* Exploration finished */
const sgl_uint16 Mask	= 0xf000;
const sgl_uint32 ClearMask	= 0xffff0fff;

/* const float TOL = 0.002f; */
const sgl_uint32 TOL = 0x3b03126f;
const float fMaxMinVal = 100000.0f;

char        *pcWorkspace = NULL;;
char        *pcWorkspacePtr;
sgl_uint32  uWorkspaceStart;
sgl_uint16	g_usCellCount = 0; /* For hashing */

static sgl_uint32 uNumPasses;
static sgl_uint32 g_uPasses;

PTRANSTRI_STRUCT  psTriNodes[MAX_NUM_REGION_TRIS];
PTRANSTRI_STRUCT  ppsTriNodePtrs[NUM_PPSTRINODEPTRS];  /* Array of pointers for partitioning */
PTRANSTRI_STRUCT  *ppsDisplayList = ppsTriNodePtrs;

LOCAL_REF *psLocalRefs;
LOCAL_NODE *psLocalNodes;

#if DEBUG
	sgl_uint32 g_uInter;
#endif

static HASH_STRUCT Hash[HASH_SIZE];

/* 
	INLINE functions - Sgn is an expansion of:
	#define SIDE(A) ((fabs(A) > TOL) ? (((A) > 0) ? 1 : -1) : 0)
*/

static INLINE int Sgn(float fA)
{
	float fB = sfabs(fA);

	if (FLOAT_TO_LONG(fB) > TOL)
	{
		if(FLOAT_TO_LONG(fA) > 0)
		{
			return (1);
		}
		else 
		{
			return (-1);
		}
	}
	else 
	{
		return (0);
	}
}

/*
	These functions allocate memory for new edges and nodes in the DAG
	They just bump a pointer through a statically allocated lump called the workspace
*/
sgl_bool InitialiseTransortMemory( void )
{
	/* Calculate in bytes */
#if (MAX_NUM_REGION_TRIS > 512)
	/* Cut down space as tris more tris are less likely to overlap each other */
	uWorkspaceStart = ( ( sizeof( TRIANGLE_REFERENCE ) * ( (MAX_NUM_REGION_TRIS/16) * MAX_NUM_REGION_TRIS -1)) )  ;
#else
	uWorkspaceStart = ( ( sizeof( TRIANGLE_REFERENCE ) * ( (MAX_NUM_REGION_TRIS) * MAX_NUM_REGION_TRIS -1)) )  ;
#endif
	
	pcWorkspace = SGLMalloc( uWorkspaceStart );	

	psLocalRefs = (LOCAL_REF *)SGLMalloc(sizeof(LOCAL_REF)*LOCAL_REFS);
	psLocalNodes = (LOCAL_NODE *)SGLMalloc(sizeof(LOCAL_NODE)*LOCAL_NODES);

	if (!(pcWorkspace && psLocalRefs && psLocalNodes))
	{
		return (FALSE);
	}

	return (TRUE);
}

void FinalizeTransortMemory( void )
{
	if (pcWorkspace)
	{				  
		DPF((DBG_MESSAGE,"Releasing Transorting Workspace memory"));
		SGLFree(pcWorkspace);	
	}
	if (psLocalRefs)
	{
		DPF((DBG_MESSAGE,"Releasing Graph Reference memory"));

		SGLFree(psLocalRefs);
	}
	if (psLocalNodes)
	{
		DPF((DBG_MESSAGE,"Releasing Graph Node memory"));
		SGLFree(psLocalNodes);
	}

	pcWorkspace = NULL;
	psLocalRefs = NULL;
	psLocalNodes = NULL;
}

static INLINE PTRIANGLE_REFERENCE NewTriangleReference( void )
{
    PTRIANGLE_REFERENCE psTriRef;

	if (pcWorkspacePtr > (pcWorkspace + uWorkspaceStart))
	{
		#if 0
 		PVROSPrintf("NewTriangleReference: ws=%08X, ptr=%08x, ws+limit=%08X\n",
					 pcWorkspace, pcWorkspacePtr,(pcWorkspace + uWorkspaceStart));
		#endif
		return NULL;
	}

    psTriRef = (PTRIANGLE_REFERENCE)pcWorkspacePtr;
    pcWorkspacePtr += sizeof( TRIANGLE_REFERENCE );

    return psTriRef;
}


/*
    Builds the new display DAG
	This is the function to call to get it all going -- it builds the graph
	It uses the global psItris array as the input array, and builds the graph inside
	the global pcWorkspace memory. For display, or generation of object lists, we then
	call Traverse, which ouputs all the passes individually.
*/

void DoTheStuff( sgl_uint32 uPolys, PTRANSTRIINDEX_STRUCT *prTriangleIndicesList )
{
    sgl_uint32 uCount,i,j=0;
	PTRANSTRIINDEX_STRUCT pTriangleIndicesList = *prTriangleIndicesList;

    /* Reset the workspace */
    pcWorkspacePtr = pcWorkspace;
	g_uPasses = 0;

#if DEBUG
	g_uInter  = 0;
#endif
	/* The first block will probably be partially full */    
	uCount = uPolys % NUM_INDICES_IN_BLOCK;
	/* Using % is okay, as NUM_INDICES_IN_BLOCK is 32, compiler 
	 * just does uPolys & 0x1f */
		
	/* But if it is exactly full, don't miss it out! */    
	if (uCount == 0)	uCount = NUM_INDICES_IN_BLOCK;

	/* Remaining blocks, if any will contain NUM_INDICES_IN_BLOCK */
	while(pTriangleIndicesList != NULL)
	{
		for(i = 0; i<uCount; i++)
		{
			PTRANSTRI_STRUCT	psTriangle;
			psTriangle = &gpTransTris[ pTriangleIndicesList->usIndex[i] ];

		    psTriangle->psChildren = NULL;
		    psTriangle->usNumLocks = 0;
						
			ppsTriNodePtrs[j] = psTriangle;
			psTriNodes[j++]   = psTriangle;		
		}
		uCount = NUM_INDICES_IN_BLOCK;
		pTriangleIndicesList = pTriangleIndicesList->pNext;
	}														   

	/* Space partitioning stuff */
	Intersect( ppsTriNodePtrs, ppsTriNodePtrs + uPolys, &ppsTriNodePtrs[NUM_PPSTRINODEPTRS - 1], 1, uPolys, 0 );	

	g_usCellCount++;
}

/*
	TRIANGLE - TRIANGLE OVERLAP ROUTINES

	LineSplitsVerts :-

		fLine is an array s.t. fLine[0] * x + fLine[1] * y + fLine[2] = 0 on the line

	This routine returns 1 if the given line splits the given vertices.
	It also increments *puInsides if we one of the vertices is on the 'inside' of the line

	Note that fVert1 has dimension of 3 but we only use the first two elements
*/
static INLINE sgl_uint32 LineSplitsVerts( float *fLine, float fVert1[2], float fVert2[2], sgl_uint32 *puInsides )
{
	sgl_int32 a, b;
	float fA, fB, fFabA, fFabB;

	fA = fLine[A] * fVert1[X] + fLine[B] * fVert1[Y] + fLine[C];
	fB = fLine[A] * fVert2[X] + fLine[B] * fVert2[Y] + fLine[C];
	
	fFabA = sfabs(fA);
	fFabB = sfabs(fB);

	/* if a != -1, then increment puInsides */
	if (FLOAT_TO_LONG(fFabA) > TOL)
	{
		if (FLOAT_TO_LONG(fA) > 0)
		{
			a = 1;
	        *puInsides += 1;
		}
		else
		{
			a = -1;
		}
	}
	else
	{
		a = 0;
        *puInsides += 1;
	}
	/*Sgn( fLine[A] * fVert1[X] + fLine[B] * fVert1[Y] + fLine[C] ); */
	
	/* b = (FLOAT_TO_LONG(fFabB) > TOL) ? ((FLOAT_TO_LONG(fB) > 0) ? 1 : -1) : 0; */
	if (FLOAT_TO_LONG(fFabB) > TOL)
	{
		if (FLOAT_TO_LONG(fB) > 0)
		{
			b = 1;
	        *puInsides += 1;
		}
		else
		{
			b = -1;
		}
	}
	else
	{
        *puInsides += 1;
        return 0; /* Next test will fail, so don't do it */
	}

    if (a != b && a != 0 && b != 0)
    {
        return 1;
    }

    return 0;
}


/*
	This tests two lines, with the given vertices, to see if they intersect, and
	fills out the intersection point if they do, as well as returning 1 (0 if they don't)
*/
static INLINE sgl_uint32 LinesIntersect( float *fLineA, float *fLineB, float fLineAVert1[2], float fLineAVert2[2], float fLineBVert1[2], float fLineBVert2[2], float fPoint[2], sgl_uint32 *puAInsides)
{
    sgl_uint32 uGarbage;
    float fCross;

	if (!LineSplitsVerts( fLineA, fLineBVert1, fLineBVert2, puAInsides ))
	{
		return 0;
	}

	if (!LineSplitsVerts( fLineB, fLineAVert1, fLineAVert2, &uGarbage ))
	{
		return 0;
	}

	/* Locate the line intersection */

	fCross = 1.0f / (fLineA[A] * fLineB[B] - fLineB[A] * fLineA[B]);
	fPoint[0] = (fLineB[C] * fLineA[B] - fLineA[C] * fLineB[B]) * fCross;
	fPoint[1] = (fLineA[C] * fLineB[A] - fLineB[C] * fLineA[A]) * fCross;

    return 1;
}

/*
	Returns 1 if the triangle nodes pointed to by psNodeA and psNodeB overlap
	Fills out fPoint to be a point in the intersection if they do
*/

static sgl_uint32 Overlaps( PTRANSTRI_STRUCT psTriA, PTRANSTRI_STRUCT psTriB, float fPoint[2] )
{
    sgl_uint32 i;
    sgl_uint32 uBInsideA;
  
    uBInsideA = 0;

	/* Test for intersection - it may be worth gathering some statitics about how many 
	 * conditions we have to test against, and reordering the statements */

    if (LinesIntersect( &psTriA->fAdjoint[0][0], &psTriB->fAdjoint[0][0], psTriA->fVerts[1], psTriA->fVerts[2], psTriB->fVerts[1], psTriB->fVerts[2], fPoint, &uBInsideA )) return 1;
    if (LinesIntersect( &psTriA->fAdjoint[0][0], &psTriB->fAdjoint[0][1], psTriA->fVerts[1], psTriA->fVerts[2], psTriB->fVerts[2], psTriB->fVerts[0], fPoint, &uBInsideA )) return 1;
    if (LinesIntersect( &psTriA->fAdjoint[0][0], &psTriB->fAdjoint[0][2], psTriA->fVerts[1], psTriA->fVerts[2], psTriB->fVerts[0], psTriB->fVerts[1], fPoint, &uBInsideA )) return 1;

    if (LinesIntersect( &psTriA->fAdjoint[0][1], &psTriB->fAdjoint[0][0], psTriA->fVerts[2], psTriA->fVerts[0], psTriB->fVerts[2], psTriB->fVerts[1], fPoint, &uBInsideA )) return 1;
    if (LinesIntersect( &psTriA->fAdjoint[0][1], &psTriB->fAdjoint[0][1], psTriA->fVerts[2], psTriA->fVerts[0], psTriB->fVerts[2], psTriB->fVerts[0], fPoint, &uBInsideA )) return 1;
    if (LinesIntersect( &psTriA->fAdjoint[0][1], &psTriB->fAdjoint[0][2], psTriA->fVerts[2], psTriA->fVerts[0], psTriB->fVerts[0], psTriB->fVerts[1], fPoint, &uBInsideA )) return 1;

    if (LinesIntersect( &psTriA->fAdjoint[0][2], &psTriB->fAdjoint[0][0], psTriA->fVerts[0], psTriA->fVerts[1], psTriB->fVerts[1], psTriB->fVerts[2], fPoint, &uBInsideA )) return 1;
    if (LinesIntersect( &psTriA->fAdjoint[0][2], &psTriB->fAdjoint[0][1], psTriA->fVerts[0], psTriA->fVerts[1], psTriB->fVerts[0], psTriB->fVerts[2], fPoint, &uBInsideA )) return 1;
    if (LinesIntersect( &psTriA->fAdjoint[0][2], &psTriB->fAdjoint[0][2], psTriA->fVerts[0], psTriA->fVerts[1], psTriB->fVerts[0], psTriB->fVerts[1], fPoint, &uBInsideA )) return 1;

    if (uBInsideA == 18)
    {
        fPoint[0] = psTriB->fVerts[1][0];
        fPoint[1] = psTriB->fVerts[1][1];
        return 1;
    }

    /*
        If we get here then we know that either A is completely inside B, or
        the two triangles don't overlap at all
        We test which by seeing if a particular vertex in A is on the "inside"
        of every line in B.  In future we should probably use the centroid of A
    */

	fPoint[0] = (psTriA->fVerts[0][0] + psTriA->fVerts[1][0] + psTriA->fVerts[2][0]) * (1.0f / 3.0f);
	fPoint[1] = (psTriA->fVerts[0][1] + psTriA->fVerts[1][1] + psTriA->fVerts[2][1]) * (1.0f / 3.0f);

    for(i = 0; i < 3; i++)
    {
		if (Sgn( psTriB->fAdjoint[0][i] * fPoint[0] + psTriB->fAdjoint[1][i] * fPoint[1] + psTriB->fAdjoint[2][i] ) == -1)			
		{	
			return 0;
		}
    }

    return 1;                   /* B is like totally inside A, man */
}

/*
	Adds an edge to the graph
*/
static INLINE void AddRef( PTRANSTRI_STRUCT psParent, PTRANSTRI_STRUCT psChild )
{	
	PTRIANGLE_REFERENCE psTriRef;

    psTriRef = NewTriangleReference();

	if (psTriRef)
	{
	    psTriRef->psNext = psParent->psChildren;
	    psParent->psChildren = psTriRef;
	    psTriRef->psTriNode = psChild;

	    psChild->usNumLocks++;
	}
}

/*
	Given a point fPoint in x,y space, this tells us which of the two psNodes
	points to the closer triangle
	May need modification.
*/

static INLINE sgl_uint32 isAinfrontofB( PTRANSTRI_STRUCT psNodeA, PTRANSTRI_STRUCT psNodeB, float fPoint[2] )
{
    float diff;

    /* calculate Az - Bz */

    diff =  (psNodeA->fA - psNodeB->fA) * fPoint[0];
	diff += (psNodeA->fB - psNodeB->fB) * fPoint[1];
    diff += (psNodeA->fC - psNodeB->fC);

    if (FLOAT_TO_LONG(diff) <= 0 )
    {
        return 0;
    }

    return 1;
}

/*
	This is called from do DFS -- see below
*/
void DFSVertex( PTRANSTRI_STRUCT psTriNode )
{
    PTRIANGLE_REFERENCE psChild;

	psTriNode->usMaxDepth = 1;

    psTriNode->usNumLocks |= Grey;
    psChild = psTriNode->psChildren;

    while (psChild != NULL)
    {
        if (!(psChild->psTriNode->usNumLocks & Black))
        {
            if (psChild->psTriNode->usNumLocks & Grey)
            {
                /*
                    Break the cycle by decreasing the lock count -- this still leaves
                    the edge actually present, which results in the lock count going below
                    zero during the traversal.  This is OK as we only output the triangle
                    if the lock count is exactly zero
                */
                psChild->psTriNode->usNumLocks--;
            }
            else
            {
                DFSVertex( psChild->psTriNode );
				if (psChild->psTriNode->usMaxDepth + 1 > psTriNode->usMaxDepth)
				{
					psTriNode->usMaxDepth = psChild->psTriNode->usMaxDepth + 1;
				}
            }
        }
		else
		{
			if (psChild->psTriNode->usMaxDepth + 1 > psTriNode->usMaxDepth)
			{
				psTriNode->usMaxDepth = psChild->psTriNode->usMaxDepth + 1;
			}
		}
        psChild = psChild->psNext;
    }
    psTriNode->usNumLocks |= Black;
}

/*
	This breaks all cycles in the graph by doing a depth-first traversal
	I'll probably recode this to use an explicit stack
*/
sgl_uint32 DoDFS( sgl_uint32 uPolys )
{
    sgl_uint32 i;
	sgl_uint16 usMaxDepth;
	usMaxDepth = 0;

    for(i = 0; i < uPolys; i++)
    {
        if (!(psTriNodes[i]->usNumLocks & Black))
        {
            DFSVertex( psTriNodes[i] );
			if (psTriNodes[i]->usMaxDepth > usMaxDepth)
			{
				usMaxDepth = psTriNodes[i]->usMaxDepth;
			}
        }
    }

	return (usMaxDepth);
}

/*
	This function takes start and end indices into the global ppsDisplatList[]
	array. It then displays all of them, but you would probably
	replace this with a function to generate lists of display objects (though that
	could be done directly during Traverse)
*/

sgl_uint32* DoDisplay( sgl_uint32 uStartIndex, sgl_uint32 uEndIndex, sgl_uint32* pAddr, sgl_uint32 *rpPlanes )
{
    sgl_uint32 i;
	sgl_uint32 tmp0;
    PTRANSTRI_STRUCT psItri;

	/* Do we have enough room for the pass to be inserted.
	 * If not don't put it in.
	 */
	if (*rpPlanes < SAFETY_MARGIN_TRANS)
	{
		IW( pAddr++, 0, DummyTransData);
		*rpPlanes += NUM_TRANS_PASS_START_PLANES;

	    for(i = uStartIndex; i < uEndIndex; i++)
	    {	
	        psItri = ppsDisplayList[i];

			tmp0 = psItri->uObjPtr;
			IW( pAddr++, 0, tmp0);
			*rpPlanes += ((tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK);
	    }

		IW( pAddr++, 0, DummyTransFlushData);
		*rpPlanes += FLUSH_PLANE;
	}

	return (pAddr);
}

sgl_uint32* DisplayRemainingTriangles( sgl_uint32 uPolys, sgl_uint32* pAddr, sgl_uint32 *rpPlanes )
{
    sgl_uint32 i, uEnd;
    uEnd = 0;

    for(i = 0; i < uPolys ; i++)
    {
		/* Grab all those that haven't been output yet */
        if (psTriNodes[i]->usNumLocks != 0)
        {
            ppsDisplayList[uEnd++] = psTriNodes[i];
        }
    }

    pAddr = DoDisplay( 0, uEnd, pAddr, rpPlanes);

	return(pAddr);
}

sgl_uint32 FixForVignetting( sgl_uint32 uPolys )
{
	sgl_uint32 uTransPassPlanes;
	
    /* Initial pass -- find and remove all cycles */	
    uNumPasses = DoDFS( uPolys );

	if (uNumPasses > nMaxPassCount) uNumPasses = nMaxPassCount;

	uTransPassPlanes = (uPolys * 4 /* planes in a triangle */) + 
					   (uNumPasses * (NUM_TRANS_PASS_START_PLANES + FLUSH_PLANE));

	/* Return the number of planes required for this new sorting.
	 */
	return(uTransPassPlanes);
}


/*
	Traverse -- this displays all the triangles in the form of sorted passes
	It could be simply modified to add triangles to object lists
*/

sgl_uint32* Traverse( sgl_uint32 uPolys, sgl_uint32* pAddr, sgl_uint32 *rpPlanes )
{
    sgl_uint32 uStart, uEnd, uNewStart, uNewEnd, i;
    PTRIANGLE_REFERENCE psChild;

    /* Second pass -- collect all those triangles that are unlocked */

    uStart = 0;
    uEnd = 0;

#if 1
/* Painful loop unrolling to achieve more overlap */
	{
		i = 0;
		switch (uPolys & 0x3)
		{
			case 3:
			{
				sgl_uint32 t = (sgl_uint32) psTriNodes[i]->usNumLocks;
				t = t & ClearMask;		
			    if (t == 0)
		        {
		            ppsDisplayList[uEnd++] = psTriNodes[i];
		        }
				psTriNodes[i++]->usNumLocks = t;	
			}
			case 2:
			{
				sgl_uint32 t = (sgl_uint32) psTriNodes[i]->usNumLocks;
				t = t & ClearMask;		
			    if (t == 0)
		        {
		            ppsDisplayList[uEnd++] = psTriNodes[i];
		        }
				psTriNodes[i++]->usNumLocks = t;	
			}
			case 1:
			{
				sgl_uint32 t = (sgl_uint32) psTriNodes[i]->usNumLocks;
				t = t & ClearMask;		
			    if (t == 0)
		        {
		            ppsDisplayList[uEnd++] = psTriNodes[i];
		        }
				psTriNodes[i++]->usNumLocks = t;	
			}
		}
		
	    for(; i < uPolys ; i+=4)
	    {
			sgl_uint32 t1 = (sgl_uint32) psTriNodes[i]->usNumLocks;
			sgl_uint32 t2 = (sgl_uint32) psTriNodes[i+1]->usNumLocks;
			sgl_uint32 t3 = (sgl_uint32) psTriNodes[i+2]->usNumLocks;
			sgl_uint32 t4 = (sgl_uint32) psTriNodes[i+3]->usNumLocks;

			t1 = t1 & ClearMask;
			t2 = t2 & ClearMask;
			t3 = t3 & ClearMask;
			t4 = t4 & ClearMask;

		    if (t1 == 0)
	        {
	            ppsDisplayList[uEnd++] = psTriNodes[i];
	        }

		    if (t2 == 0)
	        {
	            ppsDisplayList[uEnd++] = psTriNodes[i+1];
	        }

		    if (t3 == 0)
	        {
	            ppsDisplayList[uEnd++] = psTriNodes[i+2];
	        }

		    if (t4 == 0)
	        {
	            ppsDisplayList[uEnd++] = psTriNodes[i+3];
	        }

			psTriNodes[i]->usNumLocks =t1;	
			psTriNodes[i+1]->usNumLocks =t2;	
			psTriNodes[i+2]->usNumLocks =t3;	
			psTriNodes[i+3]->usNumLocks =t4;	
	    }
	}
#endif

#if 0
    for(i = 0; i < uPolys ; i++)
    {
	    /* Also clear the marks set during DFS */
					
        psTriNodes[i]->usNumLocks &= ~Mask;
        
        if (psTriNodes[i]->usNumLocks == 0)
        {
            ppsDisplayList[uEnd++] = psTriNodes[i];
        }
    }
#endif
    
	/* Pre decrement, taking into account the final pass */
	uNumPasses--; 
    
    /* Third and subsequent passes -- go through the display list unlocking children */
    while (uStart != uEnd)
    {	
		/* Generate a pass now, thank you very much */
	    pAddr = DoDisplay( uStart, uEnd, pAddr, rpPlanes);

   		if (++g_uPasses == uNumPasses)		
		{			
			pAddr = DisplayRemainingTriangles( uPolys, pAddr, rpPlanes);
			break;
		}

        uNewStart = uEnd;
        uNewEnd = uEnd;

        while (uStart != uEnd)
        {
            psChild = ppsDisplayList[uStart]->psChildren;
            while (psChild != NULL)
            {
                psChild->psTriNode->usNumLocks--;

                if (psChild->psTriNode->usNumLocks == 0)
                {
                    /* Add to new display list */                  
					ppsDisplayList[uNewEnd++] = psChild->psTriNode;
                }
                psChild = psChild->psNext;
            }
            uStart++;
        }

        uStart = uNewStart;
        uEnd = uNewEnd;
    }

#if DUMP_TRI_DATA
	if(uPolys > 20)
   	{
		FILE *fp = NULL;
		char szTriCount[10]="\0";
		char szOutputFileName[20]="\0";
		char szPath[] = DUMP_PATH;
		sgl_uint32 i;
	
		itoa(uPolys, szTriCount, 10);
	
		strcat(szOutputFileName, szPath);
		strcat(szOutputFileName, szTriCount);
		strcat(szOutputFileName, "tris");
		strcat(szOutputFileName, ".txt");

		fp = fopen(szOutputFileName, "w");

		if (fp)
		{
			#if DEBUG
				PVROSPrintf("Writing vertex data to %s (took %d passes)\n",szOutputFileName,g_uPasses+1);
			#else
				PVROSPrintf("Writing vertex data to %s\n",szOutputFileName);
			#endif
			for(i=0; i < uPolys; i++)
			{
				fprintf(fp,"%f\t%f\t%g\n",psTriNodes[i]->fVerts[0][0],psTriNodes[i]->fVerts[0][1],psTriNodes[i]->fVerts[0][2]);
				fprintf(fp,"%f\t%f\t%g\n",psTriNodes[i]->fVerts[1][0],psTriNodes[i]->fVerts[1][1],psTriNodes[i]->fVerts[1][2]);
				fprintf(fp,"%f\t%f\t%g\n",psTriNodes[i]->fVerts[2][0],psTriNodes[i]->fVerts[2][1],psTriNodes[i]->fVerts[2][2]);	
			}
		}
		else
		{
			PVROSPrintf("Couldn't open output file\n");
		}
	}
#endif

#if 0
	if(uPolys > 12) 
	{
		PVROSPrintf("%d: %d, ",uPolys,g_uPasses +1 );
	}
#endif
	return (pAddr);
}

/*****************************************************************************
	Spatial partitioning stuff
*****************************************************************************/

/*
    Both stacks are empty, ascending or descending
    The two stacks grow together from opposite ends of an array
*/
static INLINE void Partition( PTRANSTRI_STRUCT *ppsTriNodes, PTRANSTRI_STRUCT **pppsLeftStack, PTRANSTRI_STRUCT **pppsRightStack, unsigned bIsAscending, unsigned uNumTris, float fLine[3], unsigned *puNumLeft, unsigned *puNumShared, unsigned *puNumRight )
{
    sgl_uint32 i,uLineIsHorizontal;
	PTRANSTRI_STRUCT *ppsLeftStack, *ppsRightStack;
    
    sgl_uint32 uNumLeft, uNumRight, uNumShared;
    sgl_uint32 bInLeft, bInRight, uInc;
    sgl_int32 a, b, c;

    ppsLeftStack = *pppsLeftStack;
    ppsRightStack = *pppsRightStack;
	
	uInc = bIsAscending ? 1 : -1;
    
    if (bIsAscending)
    {
        ppsLeftStack -= uNumTris;
    }
    else
    {
        ppsRightStack += uNumTris;
    }

    uNumLeft = 0;
    uNumShared = 0;
    uNumRight = 0;

	uLineIsHorizontal = (FLOAT_TO_LONG(fLine[1]) > 0);

    for(i = 0; i < uNumTris; i++)
    {
	    PTRANSTRI_STRUCT psItri = ppsTriNodes[0];

        a = Sgn( psItri->fVerts[0][uLineIsHorizontal] + fLine[2] );
        b = Sgn( psItri->fVerts[1][uLineIsHorizontal] + fLine[2] );
        c = Sgn( psItri->fVerts[2][uLineIsHorizontal] + fLine[2] );

	    bInLeft = (a > 0) || (b > 0) || (c > 0);
        bInRight = (a < 0) || (b < 0) || (c < 0) || ((a == 0) && (b == 0) && (c == 0));

        if (bInLeft)
        {
            uNumLeft++;
            *ppsLeftStack = ppsTriNodes[0];
            ppsLeftStack += 1;
        }

        if (bInRight)
        {
            uNumRight++;
            *ppsRightStack = ppsTriNodes[0];
            ppsRightStack += -1;
        }

        if (bInLeft && bInRight)
        {
            uNumShared++;
        }

        ppsTriNodes += uInc;
    }

    *pppsLeftStack = ppsLeftStack;
    *pppsRightStack = ppsRightStack;

    *puNumLeft = uNumLeft;
    *puNumRight = uNumRight;
    *puNumShared = uNumShared;
}

static INLINE void MarkAncestors( PLOCAL_NODE psLocalNode, PLOCAL_NODE psMarkedBy )
{
	PLOCAL_REF psLocalRef;

	psLocalRef = psLocalNode->psParents;
	while (psLocalRef != NULL)
	{
		if (psLocalRef->psLocalNode->psMarkedBy != psMarkedBy)
		{
			psLocalRef->psLocalNode->psMarkedBy = psMarkedBy;
			MarkAncestors( psLocalRef->psLocalNode, psMarkedBy );
		}
		psLocalRef = psLocalRef->psNext;
	}
}

static INLINE void MarkDescendents( PLOCAL_NODE psLocalNode, PLOCAL_NODE psMarkedBy )
{
	PLOCAL_REF psLocalRef;

	psLocalRef = psLocalNode->psChildren;
	while (psLocalRef != NULL)
	{
		if (psLocalRef->psLocalNode->psMarkedBy != psMarkedBy)
		{
			psLocalRef->psLocalNode->psMarkedBy = psMarkedBy;
			MarkDescendents( psLocalRef->psLocalNode, psMarkedBy );
		}
		psLocalRef = psLocalRef->psNext;
	}
}

static void NewAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris )
{
	int i, j;
	unsigned h;
	unsigned uLocalRefInd;
	float fPoint[2];
	sgl_uint16 uResult;
	sgl_uint32 uKey;

	/*
		First create the local nodes
	*/
	uLocalRefInd = 0;
	for(i = 0; i < uNumItris; i++)
	{
		psLocalNodes[i].psTriNode = ppsTriNodes[i];		/* Probably can be taken out */
		psLocalNodes[i].psMarkedBy = NULL;
		psLocalNodes[i].psParents = NULL;
		psLocalNodes[i].psChildren = NULL;
	}

	for(i = 0; i < uNumItris; i++)
	{
		for(j = i - 1; j >= 0; j--)
		{
		    h = ((unsigned)ppsTriNodes[i] ^ ((unsigned)ppsTriNodes[j] >> 2)) % HASH_SIZE;

			if (psLocalNodes[j].psMarkedBy != &psLocalNodes[i])
			{
	            if (
	            	 (
	            	  (Hash[h].p[0] == ppsTriNodes[i] && Hash[h].p[1] == ppsTriNodes[j] && Hash[h].usRef == g_usCellCount) ||
	                  (Hash[h].p[0] == ppsTriNodes[j] && Hash[h].p[1] == ppsTriNodes[i])
	                  )
	                )

	            {
					uResult = Hash[h].uResult;
				}
				else if (Overlaps( ppsTriNodes[i], ppsTriNodes[j], fPoint ))
				{
					uResult = isAinfrontofB( ppsTriNodes[i], ppsTriNodes[j], fPoint ) ? AinfrontofB : BinfrontofA;
				}
				else
				{
					uResult = Disjoint;
				}

				if (uResult != Disjoint)
				{
					if (uResult == AinfrontofB)
					{
						MarkAncestors( &psLocalNodes[j], &psLocalNodes[i] );
						/* Insert into A's parent, and B's child list */
						psLocalRefs[uLocalRefInd].psNext = psLocalNodes[i].psParents;
						psLocalNodes[i].psParents = &psLocalRefs[uLocalRefInd];
						psLocalRefs[uLocalRefInd].psLocalNode = &psLocalNodes[j];
						uLocalRefInd++;

						psLocalRefs[uLocalRefInd].psNext = psLocalNodes[j].psChildren;
						psLocalNodes[j].psChildren = &psLocalRefs[uLocalRefInd];
						psLocalRefs[uLocalRefInd].psLocalNode = &psLocalNodes[i];
						uLocalRefInd++;
					}
					else
					{
						MarkDescendents( &psLocalNodes[j], &psLocalNodes[i] );

						/* Insert into B's parent, and A's child list */
						psLocalRefs[uLocalRefInd].psNext = psLocalNodes[j].psParents;
						psLocalNodes[j].psParents = &psLocalRefs[uLocalRefInd];
						psLocalRefs[uLocalRefInd].psLocalNode = &psLocalNodes[i];
						uLocalRefInd++;

						psLocalRefs[uLocalRefInd].psNext = psLocalNodes[i].psChildren;
						psLocalNodes[i].psChildren = &psLocalRefs[uLocalRefInd];
						psLocalRefs[uLocalRefInd].psLocalNode = &psLocalNodes[j];
						uLocalRefInd++;
					}
				}
	
	            Hash[h].p[0] = ppsTriNodes[i];
	            Hash[h].p[1] = ppsTriNodes[j];
				Hash[h].uResult = uResult;
				Hash[h].usRef = g_usCellCount;
			}
		}
	}

	/* Now copy our local nodes back onto the main DAG */

	for(i = 0; i < uNumItris; i++)
	{
		PLOCAL_REF psLocalRef;

		psLocalRef = psLocalNodes[i].psChildren;

		while (psLocalRef != NULL)
		{
			AddRef( ppsTriNodes[i], ppsTriNodes[psLocalRef->psLocalNode - psLocalNodes] );
			psLocalRef = psLocalRef->psNext;
		}
	}
}


#if 0
static void AllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris )
{
    sgl_uint32 h, i, j;
    float fPoint[2];

    for(i = 0; i < uNumItris; i++)
    {
        for(j = 0; j < i; j++)
        {
            /* Check that we havn't done this test recently */
            h = ((unsigned)ppsTriNodes[i] ^ ((unsigned)ppsTriNodes[j] >> 2)) % HASH_SIZE;
#if 0
            if (
            	!(
            	  (Hash[h].p[0] == ppsTriNodes[i] && Hash[h].p[1] == ppsTriNodes[j] && Hash[h].usRef == g_usCellCount) ||
                  (Hash[h].p[0] == ppsTriNodes[j] && Hash[h].p[1] == ppsTriNodes[i] && Hash[h].usRef == g_usCellCount)
                  )
                )
            {
#endif
                if (Overlaps( ppsTriNodes[i], ppsTriNodes[j], fPoint ))
                {
					#if DEBUG
						g_uInter++;
					#endif

					if (isAinfrontofB( ppsTriNodes[i], ppsTriNodes[j], fPoint ))
					{
						AddRef( ppsTriNodes[j], ppsTriNodes[i] );
					}
					else
					{			   
						AddRef( ppsTriNodes[i], ppsTriNodes[j] );
					}
                }
	#if 0
            }

            Hash[h].p[0] = ppsTriNodes[i];
            Hash[h].p[1] = ppsTriNodes[j];
			Hash[h].usRef = g_usCellCount;
	#endif
        }
    }
}
#endif
/*
	This one chooses the bisector of the longest side in the bounding box as the split
	line, in an attempt to avoid being drawn in to the centre of dense clusters
*/
static INLINE void ChooseSplitLine( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumTris, float fLine[3], unsigned bIsAscending )
{
    float maxx, maxy, minx, miny;
	float fTempX,fTempY;
	float mx, my;
	sgl_uint32 i, j, uNumPoints, uInc;

  	maxx = -fMaxMinVal;
	minx = fMaxMinVal;
	maxy = -fMaxMinVal;
	miny = fMaxMinVal;
    uNumPoints = 3 * uNumTris;
	mx = 0.0f;
	my = 0.0f;

	uInc = bIsAscending ? 1 : -1;

	for(i = 0; i < uNumTris; i++)
    {
		PTRANSTRI_STRUCT psItri;
		sgl_uint32 c;

		psItri = ppsTriNodes[0];
				
		/* As the compiler was refusing to INLINE by comparison function
		 * I've just exapnded it all here. Thus making it somewhat unreadable
		 * but doing the previous 'at most' 12 comparisons, now in 'at most' 10
		 * Unrolling the mx and my calculation should hopefully pipeline/overlap
		 * bit better too. */

		#if 0
        for(j = 0; j < 3; j++)
        {
			mx += psItri->fVerts[j][0];
			my += psItri->fVerts[j][1];

			CompareWithMaxAndMin(psItri->fVerts[j][0], &maxx, &minx);			
			CompareWithMaxAndMin(psItri->fVerts[j][1], &maxy, &miny);
        }
		#endif

		fTempX = psItri->fVerts[0][0];
		fTempX += psItri->fVerts[1][0];
		fTempX += psItri->fVerts[2][0];

		mx += fTempX;

		/* Compare Verts 0 and 1 with each other */
		c = (FLOAT_TO_LONG(psItri->fVerts[0][0]) > FLOAT_TO_LONG(psItri->fVerts[1][0]))?0:1;
		
		/* Compare greater of 0 or 1 with MAX */
		if (FLOAT_TO_LONG(psItri->fVerts[c][0]) > FLOAT_TO_LONG(maxx))
			maxx = psItri->fVerts[c][0];
		
		/* Compare lesser of 0 or 1 with MIN */
		if (FLOAT_TO_LONG(psItri->fVerts[~c][0]) < FLOAT_TO_LONG(minx))
			minx = psItri->fVerts[~c][0];

		/* Compare 2 with MAX - if greater, no need to compare with MIN */
		if (FLOAT_TO_LONG(psItri->fVerts[2][0]) > FLOAT_TO_LONG(maxx))
			maxx = psItri->fVerts[2][0];
		else if (FLOAT_TO_LONG(psItri->fVerts[2][0]) < FLOAT_TO_LONG(minx))
			minx = psItri->fVerts[2][0];

		/* repeat the process with Y */
		fTempY = psItri->fVerts[0][1];
		fTempY += psItri->fVerts[1][1];
		fTempY += psItri->fVerts[2][1];

		my += fTempY;

		c = (FLOAT_TO_LONG(psItri->fVerts[0][1]) > FLOAT_TO_LONG(psItri->fVerts[1][1]))?0:1;
		
		if (FLOAT_TO_LONG(psItri->fVerts[c][1]) > FLOAT_TO_LONG(maxy))
			maxy = psItri->fVerts[c][1];
		
		if (FLOAT_TO_LONG(psItri->fVerts[~c][1]) < FLOAT_TO_LONG(miny))
			miny = psItri->fVerts[~c][1];

		if (FLOAT_TO_LONG(psItri->fVerts[2][1]) > FLOAT_TO_LONG(maxy))
			maxy = psItri->fVerts[2][1];
		else if (FLOAT_TO_LONG(psItri->fVerts[2][1]) < FLOAT_TO_LONG(miny))
			miny = psItri->fVerts[2][1];

		/* Update the ol' pointer */
        ppsTriNodes += uInc;
    }

    /* Split along the line of smallest spread */
	fTempX = maxx - minx;
	fTempY = maxy - miny;

    if (FLOAT_TO_LONG(fTempX) < FLOAT_TO_LONG(fTempY))
    {
        /* Line is horizontal */
		my /= uNumPoints;
        fLine[0] = 0.0f;
        fLine[1] = 1.0f;
		fLine[2] = -my;
    }
    else
    {
        /* Line is vertical */
		mx /= uNumPoints;
        fLine[0] = 1.0f;
        fLine[1] = 0.0f;
		fLine[2] = -mx;
    }
}

/*
    Others points to some spare space at the other side of the stack
    Note that the stack pointers always point to spare space
    ppsItris always points to the highest (descending) or
    lowest (ascending) element in the list on entry
*/

static void Intersect( PTRANSTRI_STRUCT *ppsTriNodes, PTRANSTRI_STRUCT *ppsLeftStack, PTRANSTRI_STRUCT *ppsRightStack, unsigned bIsAscending, unsigned uNumTris, unsigned uLevel )
{
    unsigned uNumLeft, uNumRight, uNumShared;
    float fLine[3];

    ChooseSplitLine( ppsTriNodes, uNumTris, fLine, bIsAscending );

    /*
        Partition automatically chooses which bit of the stack to overwrite
    */
    Partition( ppsTriNodes, &ppsLeftStack, &ppsRightStack, bIsAscending, uNumTris, fLine, &uNumLeft, &uNumShared, &uNumRight );

    if (uNumLeft == (uNumLeft + uNumRight - uNumShared))
    {
        NewerFoulAllIntersects( ppsLeftStack - uNumLeft, uNumLeft );
    }
    else if (uNumRight == (uNumLeft + uNumRight - uNumShared))
    {
        NewerFoulAllIntersects( ppsRightStack + 1, uNumRight );
    }
    else if ((uNumShared * 5) > uNumLeft + uNumRight || uNumLeft == 0 || uNumRight == 0)
    {
    	NewerFoulAllIntersects( ppsRightStack + 1, uNumRight );
    	NewerFoulAllIntersects( ppsLeftStack - uNumLeft, uNumLeft );
    }
    else
    {
    	if (uNumLeft > 1) Intersect( ppsLeftStack - uNumLeft, ppsLeftStack, ppsRightStack, 1, uNumLeft, uLevel + 1 );
    	if (uNumRight > 1) Intersect( ppsRightStack + uNumRight, ppsLeftStack, ppsRightStack, 0, uNumRight, uLevel + 1 );
    }
}


#define SWAP(a, b) temp = (a); (a) = (b); (b) = temp;

#if 0
static void OldSortTriNodes( PTRANSTRI_STRUCT *arr, sgl_uint32 uNumItris )
{
	sgl_uint32 i, ir = uNumItris, j, k, l = 1;
	int jstack = 0;
	int istack[64];
	PTRANSTRI_STRUCT a, temp;

	arr -= 1;		/* Tedious subtraction as numerical recipies uses 1-based arrays */
	for (;;)
	{
		if (ir - l < 7)
		{
			/* Use insertion sort */
			for(j = l + 1; j <= ir; j++)
			{
				a = arr[j];
				for(i = j - 1; i >= 1; i--)
				{
					if (arr[i]->fMeanZ >= a->fMeanZ) break;
					arr[i + 1] = arr[i];
				}
				arr[i + 1] = a;
			}

			if (jstack == 0) break;
			ir = istack[jstack--];
			l = istack[jstack--];
		}
		else
		{
			/* Use partitioning, with median-of-three selection */
			k = (l + ir) >> 1;
			SWAP( arr[k], arr[l + 1] );

			/* Use the obligatory FLOAT_TO_LONG macro */
			if (FLOAT_TO_LONG(arr[l + 1]->fMeanZ) < FLOAT_TO_LONG(arr[ir]->fMeanZ))
			{
				SWAP( arr[l + 1], arr[ir] );
			}
			if (FLOAT_TO_LONG(arr[l]->fMeanZ) < FLOAT_TO_LONG(arr[ir]->fMeanZ))
			{
				SWAP( arr[l], arr[ir] );
			}
			if (FLOAT_TO_LONG(arr[l + 1]->fMeanZ) < FLOAT_TO_LONG(arr[l]->fMeanZ))
			{
				SWAP( arr[l + 1], arr[l] );
			}

			i = l + 1;
			j = ir;
			a = arr[l];
			for (;;)
			{
				do i++; while (arr[i]->fMeanZ > a->fMeanZ);
				do j--; while (arr[j]->fMeanZ < a->fMeanZ);
				if (j < i) break;
				SWAP( arr[i], arr[j] );
			}

			arr[l] = arr[j];
			arr[j] = a;
			jstack += 2;

			/* Push pointers to larger subarray onto stack */
			if (ir - i + 1 >= j - l)
			{
				istack[jstack] = ir;
				istack[jstack - 1] = i;
				ir = j - 1;
			}
			else
			{
				istack[jstack] = j - 1;
				istack[jstack - 1] = l;
				l = i;
			}
		}
	}
}
#else
static void SortTriNodes( PTRANSTRI_STRUCT *arr, sgl_uint32 uNumItris )
{
	sgl_uint32 i, ir = uNumItris, j, k, l = 1;
	sgl_int32 jstack = 0;
	sgl_int32 istack[64];
	PTRANSTRI_STRUCT a, temp;

	arr -= 1;		/* Tedious subtraction as numerical recipies uses 1-based arrays */
	for (;;)
	{
		if (ir - l < 7)
		{
			/* Use insertion sort */
			for(j = l + 1; j <= ir; j++)
			{
				a = arr[j];
				for(i = j - 1; i >= 1; i--)
				{
					if (FLOAT_TO_LONG(arr[i]->fMeanZ) <= FLOAT_TO_LONG(a->fMeanZ)) break;
					arr[i + 1] = arr[i];
				}
				arr[i + 1] = a;
			}

			if (jstack == 0) break;
			ir = istack[jstack--];
			l = istack[jstack--];
		}
		else
		{
			/* Use partitioning, with median-of-three selection */
			k = (l + ir) >> 1;
			SWAP( arr[k], arr[l + 1] );

			if (FLOAT_TO_LONG(arr[l + 1]->fMeanZ) > FLOAT_TO_LONG(arr[ir]->fMeanZ))
			{
				SWAP( arr[l + 1], arr[ir] );
			}
			if (FLOAT_TO_LONG(arr[l]->fMeanZ) > FLOAT_TO_LONG(arr[ir]->fMeanZ))
			{
				SWAP( arr[l], arr[ir] );
			}
			if (FLOAT_TO_LONG(arr[l + 1]->fMeanZ) > FLOAT_TO_LONG(arr[l]->fMeanZ))
			{
				SWAP( arr[l + 1], arr[l] );
			}

			i = l + 1;
			j = ir;
			a = arr[l];
			for (;;)
			{
				do i++; while (FLOAT_TO_LONG(arr[i]->fMeanZ) < FLOAT_TO_LONG(a->fMeanZ));
				do j--; while (FLOAT_TO_LONG(arr[j]->fMeanZ) > FLOAT_TO_LONG(a->fMeanZ));
				if (j < i) break;
				SWAP( arr[i], arr[j] );
			}

			arr[l] = arr[j];
			arr[j] = a;
			jstack += 2;

			/* Push pointers to larger subarray onto stack */
			if (ir - i + 1 >= j - l)
			{
				istack[jstack] = ir;
				istack[jstack - 1] = i;
				ir = j - 1;
			}
			else
			{
				istack[jstack] = j - 1;
				istack[jstack - 1] = l;
				l = i;
			}
		}
	}

}
#endif


void CreatePasses( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris )
{
	unsigned i, j, k, t;
	unsigned uSetSize, uEnd;
	float fPoint[2];

	
	if (uNumItris == 0)
	{
		return;
	}

	uEnd = ((int)uNumItris - TOP_FEW) > 0 ? uNumItris - TOP_FEW : 0;

	SortTriNodes( ppsTriNodes, uNumItris );

	uSetSize = uEnd / NUM_PASSES_PER_PARTITION;
	if (uSetSize == 0)
	{
		uSetSize = 1;
	}

	j = 0;
	i = uSetSize;

	while (1)
	{
		if (i > uEnd)
		{
			break;
		}

		/* Connect a single one of the last group to all of this group */
		if (j != i)
		{
			t = i + uSetSize;

			if (t > uEnd)
			{
				t =  uEnd;
			}

			for(k = i; k < t; k++)
			{
				AddRef( ppsTriNodes[j], ppsTriNodes[k] );
			}
		}
		j = i;
		i += uSetSize;
	}

	/* Now connect last i to the top few */
	if (uEnd != 0)
	{
		for(k = uEnd; k < uNumItris; k++)
		{
			AddRef( ppsTriNodes[uEnd - 1], ppsTriNodes[k] );
		}
	}

	/* And do these top few properly */
	if (uNumItris - uEnd > 0)
	{
		NewAllIntersects( &ppsTriNodes[uEnd], uNumItris - uEnd );
	}

}


/*
	In this routine we do a quick sort on the z-coordinates
*/
#if FOUL_OR_NEWFOUL_ALLINTERSECTS
static void FoulAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, sgl_uint32 uNumItris )
{

	SortTriNodes( ppsTriNodes, uNumItris );

	if ((uNumItris > 32) && (guTransTriCounter > 500))
	{
		int i;

		for(i = 0; i < uNumItris - 1; i++)
		{
			AddRef( ppsTriNodes[i], ppsTriNodes[i + 1] );
		}
	}
	else
	{
		NewAllIntersects( ppsTriNodes, uNumItris );
	}
}

static void NewFoulAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris )
{
	unsigned i, j, k, t;
	unsigned uSetStartIndex;
	unsigned uLastSetStartIndex;
	float fPoint[2];


	if ((uNumItris > 32) && (guTransTriCounter > 500))
	{
		SortTriNodes( ppsTriNodes, uNumItris );

		/*
			Sort completed -- now add references
			We bundle the trinodes into sets
		*/
		uSetStartIndex = 0;
		uLastSetStartIndex = 0;
		for(i = 1; i < uNumItris; i++)
		{
			for(j = uSetStartIndex; j < i; j++)
			{
				if (Overlaps( ppsTriNodes[i], ppsTriNodes[j], fPoint ) || (i - uSetStartIndex) > 2)
				{
					/*
						We're starting a new set
						Create references between all tris in this set and the last set
					*/
					for(k = uLastSetStartIndex; k < uSetStartIndex; k++)
					{
						for(t = uSetStartIndex; t < i; t++)
						{
							AddRef( ppsTriNodes[k], ppsTriNodes[t] );
						}
					}
					uLastSetStartIndex = uSetStartIndex;
					uSetStartIndex = i;
					break;
				}
			}
		}
					
		for(k = uLastSetStartIndex; k < uSetStartIndex; k++)
		{
			for(t = uSetStartIndex; t < i; t++)
			{
				AddRef( ppsTriNodes[k], ppsTriNodes[t] );
			}
		}

	}
	else
	{
		SortTriNodes( ppsTriNodes, uNumItris );
		NewAllIntersects( ppsTriNodes, uNumItris );
	}
}
#endif /*FOUL_OR_NEWFOUL_ALLINTERSECTS*/

static void NewerFoulAllIntersects( PTRANSTRI_STRUCT *ppsTriNodes, unsigned uNumItris )
{
	CreatePasses( ppsTriNodes, uNumItris );
}

#endif /* DAG_TRANS_SORTING */

/* Handy stuff for debugging */
#if 0
DPF((DBG_FATAL,"pTri : V0 {%7.02f,%7.02f)",psTriNodes[0].psItri->fVerts[0][0],psTriNodes[0].psItri->fVerts[0][1]));
DPF((DBG_FATAL,"pTri : V1 {%7.02f,%7.02f)",psTriNodes[0].psItri->fVerts[1][0],psTriNodes[0].psItri->fVerts[1][1]));
DPF((DBG_FATAL,"pTri : V2 {%7.02f,%7.02f)",psTriNodes[0].psItri->fVerts[2][0],psTriNodes[0].psItri->fVerts[2][1]));	
DPF((DBG_FATAL,"Plane ABC{%7.02f,%7.02f,%7.02f): %08X",psTriNodes[0].psItri->fA,psTriNodes[0].psItri->fB,psTriNodes[0].psItri->fC,psTriNodes[0].psItri->uObjPtr));

DPF((DBG_FATAL,"pTri : V0 {%7.02f,%7.02f,%7.02f)",psTriNodes[1].psItri->fVerts[0][0],psTriNodes[1].psItri->fVerts[0][1]));
DPF((DBG_FATAL,"pTri : V1 {%7.02f,%7.02f,%7.02f)",psTriNodes[1].psItri->fVerts[1][0],psTriNodes[1].psItri->fVerts[1][1]));
DPF((DBG_FATAL,"pTri : V2 {%7.02f,%7.02f,%7.02f)",psTriNodes[1].psItri->fVerts[2][0],psTriNodes[1].psItri->fVerts[2][1]));	
DPF((DBG_FATAL,"Plane ABC{%7.02f,%7.02f,%7.02f): %08X",psTriNodes[1].psItri->fA,psTriNodes[1].psItri->fB,psTriNodes[1].psItri->fC,psTriNodes[1].psItri->uObjPtr));
#endif

		
