/**************************************************************************
 * Name         : d3dregion.c
 * Title        : D3D parameter management for sabre
 * Author       : Edward Forde
 * Created      : 7th August 1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Based on dregion.c but has SGL stuff removed. Doesn't
 *				  distinguish between front facing and back facing triangles
 *				  with translucency.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: d3dreg.c,v $
 * Revision 1.31  1997/11/04  17:29:31  sxy
 * added function NoSortTransFaceLists() for processing no sort trans face lists.
 *
 * Revision 1.30  1997/10/15  17:05:43  mjg
 * Fixed GenerateObjectPtrLiteStrip as translucent objects with Need3Don2D
 * set were causing Page Faults.
 *
 * Revision 1.29  1997/10/13  09:53:32  erf
 * Forget what I said in the last check in.
 *
 * Revision 1.28  1997/10/10  16:45:25  erf
 * In AddRegionSeeThru() added check to see if CurTSetId[] set
 * zero. If it is we should alloc again. Need to find out why
 * it is zero, though.
 *
 * Revision 1.27  1997/10/02  11:23:38  mjg
 * put #if - #endif round InsertionSort
 *
 * Revision 1.26  1997/10/01  13:07:26  mjg
 * Reverted to the PassPerTri (OldSort) method from 4.0.0.81
 * slightly slower yes, but works for Wipeout etc.
 *
 * Revision 1.25  1997/09/24  16:43:18  mjg
 * Removed the number-of-indices-in-a-block count, so the Region Header
 * is smaller and DoTheStuff has less parameters.
 *
 * Revision 1.24  1997/09/17  13:28:10  mjg
 *  Removed Zs from TRANSTRI_STRUCT
 *
 * Revision 1.23  1997/09/16  18:43:21  mjg
 * Now releases transorting memory if not to be used.
 *
 * Revision 1.22  1997/09/16  14:23:55  erf
 * Removed SetId = 0 from AllocTransFaceList() routine. It got in
 * by mistake. Prevented new pass per tri from being disabled.
 *
 * Revision 1.21  1997/09/16  12:10:24  erf
 * nPassCountCat needs to be sgl_int32 as the result could be negative.
 * Very, very important !!!!
 *
 * Revision 1.20  1997/09/16  11:41:58  mjg
 * If bFullSort is off, then don't allocate any memory.
 *
 * Revision 1.19  1997/09/15  16:16:37  mjg
 * Added checking of allocated memory.
 *
 * Revision 1.18  1997/09/13  20:23:26  mjg
 * Made all the various plane count sgl_uint16's as it is fair to
 * assume that there won't be > 65535 planes (16,000 tris) in a single
 * region. Therefore saving a bit of time, not writing 0's.
 *
 * Revision 1.17  1997/09/11  18:06:20  mjg
 * Removed thre reset of guTransTriCounter, now done in pvrd.c
 *
 * Revision 1.16  1997/09/10  22:31:13  mjg
 * Swapped a couple of lines over.
 *
 * Revision 1.15  1997/09/10  10:44:52  erf
 * Added GenerateDAGTransPtr() routine to add the new sorted translucet
 * objects. As with these functions it removes the objects if we go
 * over the ISP limit.
 *
 * Revision 1.14  1997/09/09  17:45:19  mjg
 * Uses the average Z, rather than just the first.
 *
 * Revision 1.13  1997/09/08  12:28:59  mjg
 * Made counter non-static
 *
 * Revision 1.12  1997/09/05  15:20:14  mjg
 * Added a count of the total number of transtris in a region.
 *
 * Revision 1.11  1997/09/04  18:12:40  erf
 * 1. Removed redundant code from new sorting algorithm.
 * 2. Count number of translucent triangles when using bog standard
 *    sort so we can adaptivily switch between sorters.
 *
 * Revision 1.10  1997/09/03  11:17:13  dm
 * protected an extra add region func from pcx1 builds
 *
 * Revision 1.9  1997/09/03  10:05:55  erf
 * Put VIGNETTE_FIX define around CountTotalTranslucentPlanes() to
 * keep Davey Boy happy.
 *
 * Revision 1.8  1997/09/02  16:29:12  erf
 * Fixed new sorting algorithm. Problems added when trying to do
 * vignetting fix. Sorry ho !!!
 *
 * Revision 1.7  1997/09/02  15:31:54  erf
 * 1. Moved some defines to dregion.h since they are common between
 *    dregion.c, d3dreg.c and d3dtsort.c .
 * 2. Tidied up the vignetting fix code.
 *
 * Revision 1.6  1997/09/02  13:12:34  erf
 * Fixed vignetting fix for new sorting algorithm. Simplified CountTotalTranslucentPlanes()
 * so that it only counts translucent planes. Only called if required now.
 *
 * Revision 1.5  1997/09/01  19:57:41  mjg
 * Made a couple more of the dummy objects non-static and reordered the
 * SortTranslucentTrangles routine to attempt to get rid of vignetting -
 * seems to work, except (for now) when tiles are very complicated
 * (see Islands) oh yes, and fog seems to totally break it too.
 *
 * Revision 1.4  1997/09/01  15:24:06  dm
 * Reorders�e���� aasrgg��bllaaahchchch
 * Moved functions as new Proton complains more.
 *
 * Revision 1.3  1997/09/01  12:21:25  mjg
 * Added D3D translucency sorting routines..
 *
 * Revision 1.2  1997/08/28  12:04:15  sxy
 * Updated GenerateObjectPtrExtra().
 *
 * Revision 1.1  1997/08/21  11:31:43  erf
 * Initial revision
 *
 * Revision 1.7  1997/08/20  16:56:42  erf
 * Added new routines for vertex fogging etc so that it would compile.
 * NOT TESTED !!!!!
 *
 * Revision 1.6  1997/08/18  11:30:17  erf
 * Changed condition from '<=' to '<' on whether a pass is put at the head
 * of the translucent pass list. This ensures that translucent triangles
 * added with Z-buffering disabled are drawn in the order they are added.
 *
 * Revision 1.5  1997/08/14  08:22:40  mjg
 * Changed some of the hardcoded 3's to 7's in the pointer macros.
 *
 * Revision 1.4  1997/08/11  13:42:54  mjg
 * Changed size of TRANSFACE_LIST structure.
 *
 * Revision 1.3  1997/08/08  19:40:11  mjg
 * Added PCX2_003 compatibility and tidied up thw compile switches.
 *
 * Revision 1.2  1997/08/08  10:52:08  erf
 * Changed MinInvZ to MaxInvZ to be consistent.
 *
 * Revision 1.1  1997/08/08  08:37:28  erf
 * Initial revision
 *
 *
 *************************************************************************/

#define MODULE_ID MODID_DREGION

#include "sgl_defs.h"
#include "metrics.h"
#include <limits.h>
#include "hwinterf.h"
#include "pmsabre.h" 
#include "pvrlims.h"
#include "syscon.h"
#include "dregion.h"
#include <stdlib.h>		/* JWF added for missing prototypes */
#include <stdio.h>		/* JWF added for missing prototypes */
#include <string.h>		/* JWF added for missing prototypes */
#include "pvrosapi.h"
#include "parmbuff.h"
#include "sglmem.h"

#if DAG_TRANS_SORTING
	#include "dtri.h" /* for ITRI structure */
	#include "sgl_math.h" /* for sfabs */
	#include "d3dtsort.h"
#endif /*DAG_TRANS_SORTING*/

SGL_EXTERN_TIME_REF /* if we are timing code */

#undef OPAQUE
#define OPAQUE		PACKED_TYPE_OPAQUE
#define OPAQUETRANS	PACKED_TYPE_OPAQUETRANS
#define LIGHTVOL	PACKED_TYPE_LIGHTVOL
#define SHADOW		PACKED_TYPE_SHADOW

#define TRANSLUCENT			PACKED_TYPE_TRANSLUCENT
#define GOURAUDHIGHLIGHT	PACKED_TYPE_GOURAUDHIGHLIGHT
#define VERTEXFOG			PACKED_TYPE_VERTEXFOG
#define TRANS_GOURAUDHIGHLIGHT	PACKED_TYPE_TRANS_GOURAUDHIGHLIGHT
#define TRANS_VERTEXFOG			PACKED_TYPE_TRANS_VERTEXFOG

#define DOING_SGL_STYLE_SORTING 1
/**************************************************************************
 **************************************************************************

	typedefs and defines (with explanations of the way things work together)

 **************************************************************************
 **************************************************************************/

/*****************************************************************************

	Lists in this module have common characteristics that make them faster.

	All the lists are external lists of sgl_uint32 items with a single pPrev
	field at the start of each set of entries that points to the previous
	block of entries in the same list or NULL if no previous block exists.

	A pointer reference to a block always points at the last sgl_uint32 entry
	in the block or the link field at the base of the block in the
	degenerate 'empty' case corresponding to blocks on the free list.
	
	Blocks are always aligned on a sizeof(BLOCK) boundary so that the LSB's
	of the pointer correspond directly to the number of data items currently
	stored in the block (not counting the link field as an item).

	In the case of the OBJECT_BLOCK lists used for OPAQUE objects a sgl_uint32
	slot is reserved at the end of each block to permit the blocks to be 
	directly allocated from parameter memory and a PCX2 link to be inserted
	at the end of even a 'full' block so that the copying of the lists by
	the GeneratePtr phase can be avoided.
	
	To allow for this we or 3 into the LSB of any Entry count as we expect
	the number of Entries quoted to be one or two less than the size of a
	block in sgl_uint32's.
	
*****************************************************************************/

/* Empty SET pointers (set has all slots free) point at the LINK field */
#define PTR_SET_EMPTY( Ptr, Entries )\
	 ( ( (((sgl_uint32) (Ptr))/sizeof(sgl_uint32)) & (Entries|3) ) == 0 )

/* Full SET pointers (set has no slots free) point at the end of the SET */
#define PTR_SET_FULL( Ptr, Entries ) ( ( (Ptr) == NULL ) || \
	 ( ( (((sgl_uint32) (Ptr))/sizeof(sgl_uint32)) & (Entries|3) ) == (Entries) ) )

/* You can find out the number of entries currently in any SET */
#define PTR_SET_SIZE( Ptr, Entries )\
	 ( (((sgl_uint32) (Ptr))/sizeof(sgl_uint32)) & (Entries|3) )

/* To add an entry to a SET; please make sure it's not FULL first */
#define PTR_SET_ADD( Ptr, Entry )\
	 *(++(Ptr)) = (sgl_uint32) (Entry)

/* To read the next entry out of a SET, make sure it's not EMPTY first */
#define PTR_SET_SUB( Ptr ) *(Ptr)--

/*****************************************************************************

	Translucent objects are grouped into sets that can be drawn in a single
	rendering pass.	No two translucent objects in the same pass can overlap
	in X and Y. Multiple passes over the same area need to be done in deepest
	first order.
	
	We have to optimise for the case where the set size is very small because
	of the division of translucent forms (which may be a simple as 2 objects
	in the first place) into these smaller sub-sets.

	The sub-sets are called FACEs as they ideally consist of two groups that
	form the entire FRONT and the entire BACK of a large composite translucent
	object within the scene. Sadly most translucent objects are not that
	complicated but we have to deal with general case.

    This structure only stores the fragment of a FACE object set that lies
    within the same region of the screen in X and Y.

	If translucent object FACE sets extend beyond 3 objects then we allocate
	extension blocks as required.

*****************************************************************************/

typedef struct _transface_list {
	sgl_uint32 *pLastSlot;				/* Pointer to last object inserted.     */
	float MaxInvZ;						/* Sort FACE objects into inverse Z order*/
	struct _transface_list *pPre;		/* During the Z sort we doubly link the */
	struct _transface_list *pPost;		/* list of faces in a region.           */
} TRANSFACE_LIST;

/* A TRANSOBJ_BLOCK needs to be the same size as a TRANSFACE_LIST */
#define TRANSOBJS_PER_BLOCK ((sizeof(TRANSFACE_LIST)/sizeof(sgl_uint32))-1)

typedef struct _transobj_block {
	sgl_uint32 *pPrev;
	sgl_uint32 Objects[TRANSOBJS_PER_BLOCK];
} TRANSOBJ_BLOCK;

/*****************************************************************************

	Opaque objects need to be stored in large numbers so we use sets of
	objects that are a multiple of the set size used for translucent objects.

	The multiple has to be a power of two to sustain the list pointer
	characteristics we utilise in the code.

*****************************************************************************/

#define	OBJECTS_PER_BLOCK	(((TRANSOBJS_PER_BLOCK+1)*16)-2)

typedef struct _object_block {
	sgl_uint32 *pPrev;							/* Previous block	    */
	sgl_uint32 Objects[OBJECTS_PER_BLOCK];		/* Object pointer data  */
	struct _object_block *pNext;			/* See OutputOpaqueList */
} OBJECT_BLOCK;

/*****************************************************************************

   To achieve sizeof(OBJECT_BLOCK) alignment of OBJECT_BLOCKS we need to
   allocate chunks of OBJECT_BLOCKs with space for an extra block in order
   to then align the OBJECT_BLOCK pointers within it.
   
   We use up to half the 'wasted' space as a permanent free list using a
   'chunk header' with the same alignment tricks used throughout. The size of
   this header is half the size of an OBJECT_BLOCK so it can be itself aligned
   within the bigger of the gaps left to align the OBJECT_BLOCKs. We hence
   only waste 1/2 an OBJECT_BLOCK of memory per chunk at the end of the day.

*****************************************************************************/

#define	BLOCKS_PER_CHUNK (((OBJECTS_PER_BLOCK+2)/2)-1)

typedef struct _object_chunk_hdr {
	sgl_uint32 *pPrev;							/* Previous chunk of blocks	*/
	sgl_uint32 FreeBlks[BLOCKS_PER_CHUNK];		/* Object block pointers	*/
} OBJECT_CHUNK_HDR;

/************************************************************************

	Each region has five lists of objects; Simple OBJECT_BLOCK lists for
	opaque, lights, and shadows with seperate indirect lists of FRONT and
	BACK TRANSFACE_LIST headers for translucent.

	TRANSFACE_LISTs each hold a sub-set of the translucent objects that
	form a feature in the scene. An id value is used to ensure objects
	from different features in the scene don't get put in the same
	sub-set. Hence we track the id of each currently forming FRONT or BACK
	translucent object face sub-set within ech region.

	We also need to support an intelligent decision about the partitioning
	of the screen into horizontal strips of regions (commonly called tiles).
	We can aid this process by tracking the number of objects that fall into
	the following catagories-

		(a) cross from this tile into the tile below it; SAD.
    	(b) lie solely in the top or bottom half of this tile; BUSY.
		(c) cross from top to bottom halves of this tile without
		    effecting the tile below it; HAPPY.

	Every object placed in one of the lists of each region will be recorded
	in one (and only one) of these counters.
	
************************************************************************/

#define HAPPY 0					/* HeightStats counter offsets			*/
#define BUSY  1
#define SAD   2

typedef struct	_region_header {
	sgl_uint32 *pLastSlots[5];		/* Opaque, LightVol, and Shadow lists	*/
	sgl_uint32 *pExtraSlots[4];		/* Gouraud Highlight and Vertex Fog lists	*/ 
#if PCX1
	/* Disable for PCX2.
	 */
	sgl_uint32 HeightStats[3];		/* How we are doing on variable height?	*/
#endif
#if DAG_TRANS_SORTING
	PTRANSTRIINDEX_STRUCT uTransTriList;
	sgl_uint16 usTransTriCounter;	  /* Total number of tris */
#endif /* DAG_TRANS_SORTING */
	sgl_uint32 CurTSetId[2];			/* Current TRANSSET Ids					*/
	sgl_uint16 nPassCount;			/* Number of translucent passes in a tile.	*/
	TRANSFACE_LIST *pCurTSet[2];		/* Current TRANSSET Objects	- reqd for strips */
	sgl_uint16 OpaquePlanes; 		/* Total planes added to pLastSlots[0]  */
	sgl_uint16 TransOpaquePlanes; 	/* Total planes added to pLastSlots[OPAQUETRANS]  */
	sgl_uint16 ExtraPlanes[4]; 	    /* Total planes added to pLastSlots[GOURAUDHIGHLIGHT->TRANSVERTEXFOG]  */
	sgl_uint32 PrevOpaqueId;		/* Can combine adjacent OPAQUE objects  */
	sgl_uint32 PrevTransOpaqueId;	/* Can combine adjacent OPAQUE objects  */

} REGION_HEADER;

/************************************************************************

	The regions are formed into strips of tiles with a variable height
	in terms of pixels on the screen.

	By tracking if any region in the strip has overloaded (and could be
	benificially divided vertically). We allow strips to be divided to
	cope better with the next frame while the tile load is reasonably
	predictable.

************************************************************************/

typedef struct tagREGION_STRIP_EXTRA
{
	sgl_uint32	*StartOfFirstRegion;/* ptr to first object in first tile	  */
	sgl_uint32	*EndOfLastRegion;	/* ptr to end of last visible region	  */
	int		nRegionsInStrip;	/* number of vis. contiguous regions	  */
	int		nLeftX;				/* start of leftmost region in pixels 	  */
	int		nRightX;			/* right of rightmost region in pixels 	  */
} REGION_STRIP_EXTRA;

typedef struct _region_strip {
	REGION_HEADER Regions[MAX_X_REGIONS];
	sgl_uint32 RegionWordY;			/* Region word with YPos and YSize set  */
#if PCX1
	/* Disable for PCX2.
	 */
	int PlaneTally;				/* Average number of planes in a strip  */
#endif
	int Width; 					/* Boolean, 0 -> XSize, 1 -> (XSize*2)  */
	int Height;					/* 2 to 64 in powers of two				*/
	REGION_STRIP_EXTRA *pExtra; /* ptr to extra data for strips			*/ 
	struct _region_strip *pNext;/* Next strip down the screen           */
} REGION_STRIP;

/**************************************************************************
 **************************************************************************

	Global variables used within this module

 **************************************************************************
 **************************************************************************/

/* Defined in PVRD.c .
 */
extern sgl_uint32	nMaxPassCount;
extern sgl_uint32	bFullSort;

/* No sort method, 1 fro no sort, 2 for reversed no sort */
extern int gNoSortTransFaces;


#if DUMP_PARAMS
	extern sgl_uint32 *curAddr;
#endif

#define MAX_Y_RESOLUTION (768+64)	/* Allow for MAX overlap in Y */

/* To directly access a region strip use the Y coordinate in here */
static REGION_STRIP *pRegionStrips[(MAX_Y_RESOLUTION/2)+1];
static int BaseOfStrips[(MAX_Y_RESOLUTION/2)+1], OutputHeight, LastTileLimit;

/* strip extra data */
static REGION_STRIP_EXTRA RegionStripExtra[(MAX_Y_RESOLUTION/2)+1];

/* We can use the following mask to aid object merging in AddRegionObjects */
static sgl_uint32 SigXYMask;

/* To combine OPAQUE objects we use this per frame identifier */
static sgl_uint32 OpaqueId;
static sgl_uint32 TransOpaqueId;

/* To comprehend the screen and tile sizes we retrieve RegionInfo */
DEVICE_REGION_INFO_STRUCT RegionInfo;

/* We must globally allocate CurrentTransSetId to control set formation */
#if DOING_SGL_STYLE_SORTING
static sgl_uint32 CurrentTransSetId[2];
#else
static sgl_uint32 CurrentTransSetId;
#endif


/* As we go along we may build up some free STRIPs */
	static REGION_STRIP *FreeRegionStrip;

/* OBJECT_BLOCKs can be reused via the permanent Chunk header list */
static sgl_uint32 **FreeObjChunk, *AllChunkHdrs;

/* TRANSFACE_LISTs are allocated from an OBJECT_BLOCK */
static TRANSFACE_LIST *pNextTransFace;

/* Special object addresses or pointers */
static sgl_uint32 DummyObjData = ULONG_MAX;

	#if DAG_TRANS_SORTING /* Required by DoDisplay */
	sgl_uint32 DummyFlushData = ULONG_MAX;
	sgl_uint32 DummyTransData = ULONG_MAX;
	sgl_uint32 DummyTransFlushData = ULONG_MAX;
	sgl_uint32 DummyObjDataLarge = ULONG_MAX;
	#else
	static sgl_uint32 DummyFlushData = ULONG_MAX;
	static sgl_uint32 DummyTransFlushData = ULONG_MAX;
	static sgl_uint32 DummyTransData = ULONG_MAX;

	#if VIGNETTE_FIX
	/* Required for large dummy object if vignetting fix required.
	 */
	static sgl_uint32 DummyObjDataLarge = ULONG_MAX;
	#endif

	#endif /* DAG_TRANS_SORTING */

static sgl_uint32 CompleteShadowAddress= ULONG_MAX;
#if PCX1
static sgl_uint32 DummyTransObjData = ULONG_MAX;
#endif

/* When do we stop merging strips together? */
static int MaxHeight;

/* Similarly, when do we stop splitting? */
static int MinHeight;

/* At what point can we split/merge tiles horizontally */
static int MergeHeight;

#if DAG_TRANS_SORTING
/* Space for translucent triangles */
sgl_uint16 guTransTriCounter;
TRANSTRI_STRUCT *gpTransTris;

/* Space for translucent triangles per region */
static char *gpcTransTriIndicesWorkspace; 
static char* gpcTransTriIndicesWorkspacePtr;
static sgl_uint32 gpcTransTriIndicesWorkspaceStart;
#endif /* DAG_TRANS_SORTING */

#if WIN32 || DOS32 || MAC

	#define NoOfSabres 0

	#if (PCX1 || ((PCX2 || PCX2_003) && !(POINTERS_IN_TLB && POINTERS_FOLLOW_PLANES)))

		extern int PCXRegionDataPages;		

	#endif
	
#endif

/* We use RegionXMagic value to improove performance in RegionWord calc. */
static sgl_uint32 RegionXMagic = 32;

#define CalcRegionXMagicVal( RegionXSize ) \
	for ( RegionXMagic = 0;\
		  ( (1<<RegionXMagic) != ((RegionXSize) / NUM_SABRE_CELLS) );\
		  RegionXMagic++ )\
		ASSERT(( (1<<RegionXMagic) < ((RegionXSize) / NUM_SABRE_CELLS) ))

/* This calculates the RegionWordY which is stored on each strip */
#define RegionWordYVal( YBase, Lines, Width ) (\
	REG_COMPULSARY_BITS + (( 1 << ( (Width) + RegionXMagic ) )-1) +\
	( (( (Lines) / (NoOfSabres+1) )-1) << REG_YSIZE_SHIFT ) +\
	(  ( ((YBase)<<Y_SHIFT) / (NoOfSabres+1) ) << REG_YPOS_SHIFT ) )
	
/* To generate for a specified X region on a strip add this to RegionWordY */
#define AddRegionWordXData( XReg, Width ) (\
	( (XReg) << ( (Width) + RegionXMagic + REG_XPOS_SHIFT ) ) )

/* To move between X regions on a strip add/sub this to/from RegionWord */
#define DeltaRegionWordX( Width ) \
	( 1 << ( (Width) + RegionXMagic + REG_XPOS_SHIFT ) )

	
/**************************************************************************
 **************************************************************************

	Region handling code (Not called in performance critical periods)

 **************************************************************************
 **************************************************************************/

/**************************************************************************
 * Function Name  : AllocRegionStrip
 * Inputs         : int YBase  - Base of strip = (Y/MIN_REGION_HEIGHT)
 *                : int Height - Height of new strip / MIN_REGION_HEIGHT
 *                : int Width  - Height of new strip / MIN_REGION_HEIGHT
 * Outputs        : 
 * Input/Output	  : 
 * Returns        :	REGION_STRIP * - new region strip or NULL
 * Global Used    : pRegionStrips
 * Description    : Creates a new region strip at generates references to
 *					it in pRegionStrips vector.
 *				   
 **************************************************************************/
static INLINE REGION_STRIP *AllocRegionStrip( int YBase, int Height, int Width )
{
	REGION_STRIP *pStrip = FreeRegionStrip;
	int YReg;
	
	if ( pStrip == NULL )
	{
		/* Need to allocate one it seems */
		pStrip = NEW(REGION_STRIP);
		ASSERT(( pStrip != NULL));
	}
	else
	{
		/* Got it */
		FreeRegionStrip = pStrip->pNext;
	}

	if ( pStrip != NULL )
	{
		/* Initialise whole object to 0 */
		memset(pStrip, 0, sizeof(REGION_STRIP));

		/* Set height and width */
		pStrip->Height = Height;
		pStrip->Width = Width;

		if ( ( Height + YBase ) <= (LastTileLimit>>Y_SHIFT) )
		{
			/* Calculate the base descriptor for regions in the strip */
			pStrip->RegionWordY = RegionWordYVal( YBase,
									Height<<Y_SHIFT, Width );
												
			DPF((DBG_VERBOSE,"Strip at %d, H %d, W %d - RWord %X", YBase,
										Height, Width, pStrip->RegionWordY));
		}
		else
		if ( (YBase<<Y_SHIFT) < LastTileLimit )
		{
			/* Sadly this one overlaps the bottom of the screen */
			pStrip->RegionWordY = RegionWordYVal( YBase,
							LastTileLimit - (YBase<<Y_SHIFT), Width );
							
			DPF((DBG_VERBOSE,"Last strip at %d H < %d, W %d - RWord %X",
								YBase, Height, Width,  pStrip->RegionWordY));
		}
		else
		{
			/* This forms a guard band past the bottom of the screen */
			pStrip->RegionWordY = 0;
			
			DPF((DBG_VERBOSE,"Strip at %d height %d off bottom of screen",
													YBase, Height));
		}

		/* Create references */
		YReg = YBase;

		if ( YReg > 0 )
		{
			/* Create link to strip from previous strip */
			pRegionStrips[YReg-1]->pNext = pStrip;
		}

		while ( Height-- != 0 )
		{
			/* Create references */
			pRegionStrips[YReg] = pStrip;
			BaseOfStrips[YReg]  = YBase;
			YReg++;
		}

		/* And setup the next strip pointer */
		pStrip->pNext = pRegionStrips[YReg];
	}

	return (pStrip);
}

static void AllocObjectChunk( void );

/**************************************************************************
 * Function Name  : InitRegionDataL
 * Inputs         :  
 * Outputs        : 
 * Input/Output	  : 
 * Returns        :
 * Global Used    : All
 * Description    : Sets up the global structures at the start
 *				   
 **************************************************************************/
void InitRegionDataL()
{
	/* Reset all globals */
#if DOING_SGL_STYLE_SORTING
	CurrentTransSetId[0] = 0;
	CurrentTransSetId[1] = 1;
#else
	CurrentTransSetId    = 0;
#endif


	FreeRegionStrip		 = NULL;
	pNextTransFace       = NULL;
	OutputHeight         = 0;
	RegionXMagic         = 32;
	MaxHeight			 = 0;
	MinHeight			 = 0;
	MergeHeight			 = 0;
	LastTileLimit        = 0;
	SigXYMask            = (sgl_uint32) ~0;
	OpaqueId             = 0;
	TransOpaqueId        = 0;
	memset( &pRegionStrips, 0, sizeof(pRegionStrips) );
	memset( &BaseOfStrips, 0, sizeof(BaseOfStrips) );
	memset( &RegionInfo, 0, sizeof(DEVICE_REGION_INFO_STRUCT) );

	#if DAG_TRANS_SORTING
	/* Grab some space for triangle data and references */
	if (bFullSort)
	{
		gpcTransTriIndicesWorkspaceStart = sizeof(TRANSTRIINDEX_STRUCT)* MAX_NUM_INDEX_BLOCKS;
		gpcTransTriIndicesWorkspace = SGLMalloc( gpcTransTriIndicesWorkspaceStart );

		guTransTriCounter = 0;
		gpTransTris = SGLMalloc(sizeof(TRANSTRI_STRUCT) * MAX_NUM_SCENE_TRIS); 
		if (!(gpcTransTriIndicesWorkspace && gpTransTris && InitialiseTransortMemory()))
		{
			PVROSPrintf("Transorting Memory Allocation failed - using Depth sorting.\n");
			bFullSort = FALSE;		
		}
	}
	#endif /*DAG_TRANS_SORTING*/

	/* We need at least one chunk's worth of space to simplify startup */
	FreeObjChunk = &AllChunkHdrs;
	AllocObjectChunk();

}

#if DAG_TRANS_SORTING
/**************************************************************************
 * Function Name  : CloseRegionDataL
 * Inputs         :  -
 * Outputs        :  -
 * Input/Output	  : 
 * Returns        :
 * Global Used    : gpcTransTriIndicesWorkspace, gpTransTris
 * Description    : Frees allocated memory - called during process detach
 *				   
 **************************************************************************/
void CloseRegionDataL()
{
	if (gpcTransTriIndicesWorkspace)
	{
		DPF((DBG_MESSAGE,"Releasing Triangle Indices Workspace memory"));
		SGLFree( gpcTransTriIndicesWorkspace );
	}
	if (gpTransTris)	
	{
		DPF((DBG_MESSAGE,"Releasing Triangle memory"));
		SGLFree( gpTransTris );		
	}

	/* Reset the pointers */
	gpTransTris = NULL;
	gpcTransTriIndicesWorkspace = NULL;
	
	FinalizeTransortMemory();
}
#endif /*DAG_TRANS_SORTING*/

/**************************************************************************
 * Function Name  : ResetRegionStrip
 * Inputs         : int YBase  - Base of current region
 *                : int Height - Height of current region
 *                : int Width  - Width of current region
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : int YNext  - Base of next uneffected region
 * Global Used    : FreeRegion list
 * Description    : Modifies the pRegionStrips table and allocates/releases
 *				      REGION_STRIPs to tile the area effected.
 **************************************************************************/
static INLINE int ResetRegionStrip( int YBase, int Height, int Width )
{
	REGION_STRIP *pStrip, *pNext, *pTemp;
	int YNext;

	/* Access current strip object in same base slot */
	pStrip = pRegionStrips[YBase];

	/* Normally the next uneffected one is what you might expect */
	YNext = YBase + Height;

	if ( ( Height != pStrip->Height ) || ( pStrip->Width != Width ) )
	{
		/* Recalculate size dependant data stored on a strip */
		int Lines;
		
		if ( ( Height + YBase ) <= (LastTileLimit>>Y_SHIFT) )
		{
			/* Normal full height strip */
			Lines = Height<<Y_SHIFT;
		}
		else
		if ( (YBase<<Y_SHIFT) < LastTileLimit )
		{
			/* Last strip */
			Lines = LastTileLimit - (YBase<<Y_SHIFT);			
		}
		else
		{
			/* This forms a guard band past the bottom of the screen */
			Lines = 0;
		}
		
		/* Recalculate the RegionWordY for this strip */		
		pStrip->RegionWordY = RegionWordYVal( YBase, Lines, Width );

		/* Update Width */
		pStrip->Width = Width;

		DPF((DBG_VERBOSE,"Strip at %d, Lines %d, W %d - RWord %X", YBase,
										Lines, Width, pStrip->RegionWordY));
	}
	
	if ( pStrip->Height != Height )
	{
		/* If the height has changed we need more or fewer strips.
		   We always keep this one at the same YBase and play with others. */
		
		while ( pStrip->Height < Height )
		{
			/* Need to merge strips next will start after current strip */
			for ( YBase += pStrip->Height; ( YBase != YNext ); YBase++ )
			{
				/* Create references to new bigger object */
				pRegionStrips[YBase] = pStrip;
				BaseOfStrips[YBase]  = YNext - Height;
			}

			/* Access next strip */
			pNext = pStrip->pNext;

			do
			{
				/* Merge with next strip */
				pStrip->Height += pNext->Height;

				/* Get next next strip */
				pTemp = pNext->pNext;

				/* Put strip into free Region structure list */
				pNext->pNext = FreeRegionStrip;
				FreeRegionStrip = pNext;

				/* Advance to next next strip for next time */
				pStrip->pNext = pTemp;

				/* Don't go on for ever if things have gone wrong */
				ASSERT((pStrip->Height <= Height));
			}
			while ( pStrip->Height != Height );
		}	 
		
		if ( pStrip->Height > Height )
		{
			/* Need to fragment strips, next uneffected based on old Height */
			YNext = YBase + pStrip->Height;

			/* Convert current strip into first strip of new height */
			pStrip->Height = Height;

			/* Move to the base of the next strip needed */
			YBase += Height;

			while ( ( YBase + Height ) < YNext )
			{
				/* New objects of same Width and Height */
				(void) AllocRegionStrip( YBase, Height, Width );
				YBase += Height;
			}

			/* Final fragment, allow for it to be an irregular height */						
			AllocRegionStrip( YBase, YNext - YBase, Width );
		}
	}
	else
	{
		/* Update Width parameter and RegionWordY */		
		
		if ( ((YBase + Height)<<Y_SHIFT) >= LastTileLimit ) /* Last strip */
		{
			pStrip->RegionWordY = RegionWordYVal( YBase, LastTileLimit - (YBase<<Y_SHIFT), Width );
		}
		else
		{
			pStrip->RegionWordY = RegionWordYVal( YBase, Height<<Y_SHIFT, Width );
		}
		pStrip->Width = Width;
	}

	/* Setup for next pass; quickest to set all region descriptors to 0 */
	memset( pStrip->Regions, 0, sizeof(REGION_HEADER)  *
	                            RegionInfo.NumXRegions   );

#if PCX1
	/* Disable for PCX2.
	 */
	/* Plus per frame statistics */
	pStrip->PlaneTally = 0;
#endif

	return (YNext);
}

/**************************************************************************
 * Function Name  : ResetRegionDataL
 * Inputs         : bForceReset
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : pRegionStrips
 * Description    : Prepares for a further render frame, can redistribute
 *					tiles by altering the height of REGION_STRIPs.
 *				   
 **************************************************************************/
void ResetRegionDataL(sgl_bool bForceReset)
{
#if PCX1
	REGION_STRIP *pStrip;
#endif
	int YReg, YBase = 0, Width = 0;
	int CurrentMinHeight = 0xff;

#ifdef DLL_METRIC
	InResetRegionData();
#endif

	if ( (OutputHeight == 0) || (bForceReset) )
	{
		/* We need RegionInfo to setup the RegionWords on strips */
		YReg = HWGetRegionInfo(0, &RegionInfo);
		ASSERT(YReg==0); 
	
		/* Precalculate magic value for RegionWord calcs based on XSize */
		CalcRegionXMagicVal( RegionInfo.XSize );
		
		/* Set values from ini file for dynamic tile sizing boundaries */
		MaxHeight = RegionInfo.YSize >> Y_SHIFT;	
		MinHeight = RegionInfo.MinYSize >> Y_SHIFT;
		MergeHeight = RegionInfo.MergeHeight >> Y_SHIFT;
		
		/* Set the initial Width using the same formula as we use later */		
		 Width = ( (MaxHeight > MergeHeight) &&
		           (RegionInfo.XSize == 32)     ) ? 1 : 0; 
		/* Because the .ini file setting is altered in win32/hwdevice.c 
		 * this line will not incorrectly resize a 32x32 region - but is
		 * still needed as it's presence improves D3D test.
		 */

		/* Calculate accurate height of screen */
		OutputHeight = RegionInfo.NumYRegions * RegionInfo.YSize;
		LastTileLimit = OutputHeight;
		
		if ( RegionInfo.HasLeftOver )
		{
			/* Allow for overlap at the bottom of the screen */
			LastTileLimit -= (RegionInfo.YSize - RegionInfo.LeftOverY);
		}

		for ( YBase = 0, YReg = RegionInfo.NumYRegions;
	    	  ( YReg-- != 0 ); YBase += RegionInfo.YSize )
		{
			/* All new region strips have a nominal height to start with */
#if PCX1
			pStrip = 
#endif			
			AllocRegionStrip( YBase>>Y_SHIFT, RegionInfo.YSize>>Y_SHIFT, Width);			
#if PCX1
	/* Disable for PCX2.
	 */
			/* Respond immediately/agressively to overload on 1st frame */
			pStrip->PlaneTally = REGION_PLANE_LIM >> 1;
#endif
		}

		/* Have deal in minimum Y units */
		OutputHeight = YBase>>Y_SHIFT;

		/* Terminate the lists without ever needing any special cases */
		AllocRegionStrip( OutputHeight, 1, 0);
		
		/* All strips can be treated as equal height to start with */
		CurrentMinHeight = RegionInfo.YSize>>Y_SHIFT;
	}
else
	{			
		while ( YBase < OutputHeight )
		{
			int Height = pRegionStrips[YBase]->Height;
			
			Width = pRegionStrips[YBase]->Width;

#if PCX1
/* Disable dynamic tile sizing for PCX2. Optimum is 32 by 32.
 */

			if ( (pRegionStrips[YBase]->PlaneTally > ((REGION_PLANE_LIM * 9)/10) ) &&
				 (Height > MinHeight) )
			{
				/* This strip has overloaded tiles, do something about it  - the 90% of the
				 * maximum limit is to avoid D3DTest balls having holes.
				 */

				if ( ( Height != 1 ) &&
					 ( (Height + YBase) != OutputHeight) )				
				{
					/* Sub-divide strip - as the plane limit will be 
					 * known during compilation, we can approximate the 
					 * division. The figures are adjusted for the D3D 
					 * Polygon throughput test.
					 */

					int nShiftDown = 1+(pRegionStrips[YBase]->PlaneTally >> 13);

					if (( (Height >> nShiftDown) != 0 ) &&
						( (Height >> nShiftDown) >= MinHeight) )
					{
						/* Apply full shift */
						Height >>= nShiftDown;
					}
					else
					{
						/* Don't overshoot */
						Height >>= 1;
					}
					/* Leave the plane counter, with some idea of its previous size 
					 */

					pRegionStrips[YBase]->PlaneTally >>= 1;

					/* Adjust Width too */
					Width = ( (RegionInfo.XSize == 32) &&
					          ( Height > MergeHeight )) ? 1 : 0 ;
				}
			}
			else if( (pRegionStrips[YBase]->PlaneTally < -( SAFETY_MARGIN_OPAQ * 2 )) 
					&& ( Height == pRegionStrips[YBase+Height]->Height ) 
					&& ( ( YBase & ((Height<<Y_SHIFT)-1) ) == 0 ))
			{
				/* This strip is too small, do something about it, splitting at twice the
				 * safety margin favours the D3D Intersection, but severely hampers the
				 * D3D Fill Rate tests.
				 */
				
				/* Set this counter to zero, 
				 * to try to maintain this tile size for a while 
				 */
				pRegionStrips[YBase]->PlaneTally = 0;

				if ( Height < MaxHeight )
				{
					/* Double size of this strip */
					Height <<= 1;
					Width = ( (Height > MergeHeight) &&
							  (RegionInfo.XSize == 32)) ? 1 : 0;
				}
			}
			/* else... do nothing, leave the PlaneTally count as it is. */
#endif

			/* Reset the strip */
			YBase = ResetRegionStrip( YBase, Height, Width );

			if ( Height < CurrentMinHeight )
			{
				/* Record minimum height */
				CurrentMinHeight = Height;
			}
		}

		/* Reset translucent set counter */
#if DOING_SGL_STYLE_SORTING
		CurrentTransSetId[0] = 0;
		CurrentTransSetId[1] = 1;
#else
		CurrentTransSetId = 0;
#endif
		/* Recycle all OBJECT_BLOCKs */
		FreeObjChunk = (sgl_uint32 **) AllChunkHdrs;

		/* Reset TRANSFACE_LIST allocator too */
		pNextTransFace = NULL;
	}

	/* Always set OpaqueId to a value unlikely to match an ISPAddr later */
	OpaqueId = 0x80000000;
	TransOpaqueId = 0x80000000;
	
	if ( CurrentMinHeight > MinHeight )
	{
		/* Normally we need to be accurate to half the current minimum */
		CurrentMinHeight = ( CurrentMinHeight >> 1 ) | 1;
	}
	else
	{
		/* Since we are limited by the INI setting,
		   we don't need to preserve half-tile accuracy as we
		   never sub-divide strips of this height anyway.     */
		CurrentMinHeight = MinHeight;
	}

	/* Initially SigXYMask contains all possible bits */
	for ( SigXYMask = (sgl_uint32) ~0; ( CurrentMinHeight > 1 ); CurrentMinHeight >>= 1 )
	{
		/* Halve the Y accuracy required during this frame */
		#define Y_ONLY ((0x1ff<<19)+(0x1ff<<10))
		SigXYMask = ( SigXYMask & (~Y_ONLY) )                  |
		            ( ( (SigXYMask & Y_ONLY) << 1 ) & Y_ONLY )   ;
		#undef Y_ONLY
	}

#if DAG_TRANS_SORTING
	gpcTransTriIndicesWorkspacePtr = gpcTransTriIndicesWorkspace;
#endif /* DAG_TRANS_SORTING */
}

/**************************************************************************
 * Function Name  : MaxRegionHeight
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : int - maximum height of a tile that may be used
 * Global Used    : RegionInfo
 * Description    : Currently quite simple, but lets allow for future
 *                  change to be implemented cleanly.
 **************************************************************************/
int MaxRegionHeight( void )
{

	if ( OutputHeight == 0 )
	{   /* We need RegionInfo to setup the RegionWords on strips */		
		#if DEBUG
		 sgl_uint32 YReg;
		 YReg = 	
		#endif
		HWGetRegionInfo(0, &RegionInfo);
		ASSERT(YReg==0); 
	}
		
	/* Currently it's what you might have assumed */
	return ( RegionInfo.MinYSize );
}

/**************************************************************************
 * Function Name  : MaxRegionWidth
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : int - maximum width of a tile that may be used
 * Global Used    : RegionInfo
 * Description    : We can double the nominal width of a tile if we can
 *                  reach the MergeHeight in Y.
 **************************************************************************/
int MaxRegionWidth( void )
{
	
	if ( OutputHeight == 0 )
	{   /* We need RegionInfo to setup the RegionWords on strips */
		#if DEBUG
		 sgl_uint32 YReg;
		 YReg = 
		#endif
		HWGetRegionInfo(0, &RegionInfo);
		ASSERT(YReg==0); 
	}
		
	if ( ( RegionInfo.MergeHeight >= RegionInfo.YSize ) ||
	     ( RegionInfo.XSize != 32 )                        )
	{
		/* We are limited to the basic minimum width */
		return ( RegionInfo.XSize );
	}
	else
	{
		/* We could start using double width tiles at any time */
		return ( RegionInfo.XSize*2 );
	}
}

/**************************************************************************
 **************************************************************************

	OBJECT_BLOCK, TRANSOBJ_BLOCK, and TRANSLIST_FACE allocation code

 **************************************************************************
 **************************************************************************/

/**************************************************************************
 * Function Name  : AllocObjectChunk
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : FreeObjChunk and indirectly AllChunkHdrs for 1st block
 * Description    : Called when FreeObjChunk points at the link field of the
 *                  last currently allocated chunk object.
 *                  We need to allocate a new FULL chunk of EMPTY blocks
 *                  and link it into the free list at 'FreeObjChunk'.
**************************************************************************/

struct _object_chunk {
	OBJECT_BLOCK Blocks[BLOCKS_PER_CHUNK+1];
};

static void AllocObjectChunk( void )
{
	sgl_uint8 *pData = (sgl_uint8 *) NEW(struct _object_chunk);	/* Never returns NULL */
	OBJECT_BLOCK *pBlock, *pEnd;
	sgl_uint32 *pChunkHdr;
	unsigned int FirstFragSize;
	
	ASSERT((pData != NULL));

	/* Calculate gap at start to achieve OBJECT_BLOCK alignment */
	FirstFragSize = sizeof(OBJECT_BLOCK) -
	                ( ((sgl_uint32) pData) & (sizeof(OBJECT_BLOCK)-1) );

	/* Position the OBJECT_BLOCKS */
	pBlock = (OBJECT_BLOCK *) (pData + FirstFragSize);
	pEnd   = pBlock + BLOCKS_PER_CHUNK;

	if ( FirstFragSize < sizeof(OBJECT_CHUNK_HDR) )
	{
		/* Allocate chunk header after objects */
		pChunkHdr = (sgl_uint32 *) pEnd;
	}
	else
	{
		/* Allocate before objects */
		pChunkHdr = (sgl_uint32 *) (((OBJECT_CHUNK_HDR *) pBlock) - 1);
	}

	/* Terminate chunk header list */
	pChunkHdr[0] = (sgl_uint32) NULL;

	do
	{
		/* Setup OBJECT_BLOCK pointers to indicate blocks are empty */
		PTR_SET_ADD( pChunkHdr, (sgl_uint32) pBlock );
	}
	while ( ++pBlock != pEnd );

	/* Extend free list and set next free */
	*FreeObjChunk = pChunkHdr;
	FreeObjChunk = (sgl_uint32 **) pChunkHdr;
}

/**************************************************************************
 * Function Name  : AllocObjectBlock
 * Inputs         : rpLastSlot - Pointer to FULL block in current list
 *                : Entry     - We need the space to store this entry
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : sgl_uint32 * - pointer to base of new object block.
 * Globals Used   : FreeObjChunk
 * Description    : Called when an OBJECT_BLOCK list is to be extended
 **************************************************************************/
static INLINE sgl_uint32 *AllocObjectBlock( sgl_uint32 **rpLastSlot, sgl_uint32 Entry )
{
	sgl_uint32 *pBlock = PTR_SET_SUB( FreeObjChunk );

	if ( PTR_SET_EMPTY(FreeObjChunk, BLOCKS_PER_CHUNK) )
	{
		/* Don't leave the FreeObjChunk pointer on EMPTY */

		if ( *FreeObjChunk == NULL )
		{
			/* Allocate a new chunk of blocks */
			AllocObjectChunk();
		}
		else
		{
			/* Advance to the next pre-allocated chunk */
			FreeObjChunk = (sgl_uint32 **) *FreeObjChunk;
		}
	}

	ASSERT( PTR_SET_EMPTY( pBlock, OBJECTS_PER_BLOCK ) );

	/* Setup link to previous block */
	pBlock[0] = (sgl_uint32) *rpLastSlot;

	/* Insert entry */
	pBlock[1] = Entry;

	/* Prepare for next entry */
	*rpLastSlot = &pBlock[1];
	
	return ( pBlock );
}


/**************************************************************************
 * Function Name  : AllocTransObjBlock
 * Inputs         : rpLastSlot - By ref. pointer to FULL end of list block
 *                : Entry      - We need the space to store this entry
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : pNextTransFace, FreeObjChunk
 * Description    : Called when an TRANSFACE_LIST is to be extended
 *                  We can re-use all the complex stuff for OBJECT_BLOCK
 *                  lists provided we reserve the remainder of a new
 *                  OBJECT_BLOCK for further TRANSOBJ_BLOCKs.
 **************************************************************************/
static INLINE void AllocTransObjBlock( sgl_uint32 **rpLastSlot, sgl_uint32 Entry )
{
	if ( PTR_SET_EMPTY( pNextTransFace, OBJECTS_PER_BLOCK ) )
	{
		/* Allocate new OBJECT_BLOCK to store this entry */
		pNextTransFace = ((TRANSFACE_LIST *)
								AllocObjectBlock( rpLastSlot, Entry )) + 1;
	}
	else
	{
		/* Allocate from remainder of current pNextTransFace block */
		TRANSOBJ_BLOCK *pBlock = (TRANSOBJ_BLOCK *) pNextTransFace++;
		
		ASSERT( PTR_SET_FULL( *rpLastSlot, TRANSOBJS_PER_BLOCK ) );
		ASSERT( PTR_SET_EMPTY( pBlock, TRANSOBJS_PER_BLOCK ) );

		/* Setup link to previous block (if any) */
		pBlock->pPrev = *rpLastSlot;

		/* Insert entry, and prepare for next */
		pBlock->Objects[0] = Entry;
		*rpLastSlot = &pBlock->Objects[0];
	}
}

#if !DOING_SGL_STYLE_SORTING
/**************************************************************************
 * Function Name  : InsertionSort
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : pHead		- pRegion->pCurTSet
 *                : pElement	- Element to add
 * Returns        : New pHead value, first TRANSFACE_LIST to process
 * Global Used    : None
 * Description    : Sorts the translucent face sets into increasing 
 *                  values of inverse depth.
 *
 **************************************************************************/
static INLINE TRANSFACE_LIST *InsertionSort(TRANSFACE_LIST *pHead,
							  				TRANSFACE_LIST *pElement )
{
	TRANSFACE_LIST	*pNext;

	if (pHead == NULL)
	{
		/* First time for every thing.
		 */
		pHead = pElement;
		pHead->pPre = NULL;
		pHead->pPost = NULL;
	}
	else
	{
		TRANSFACE_LIST	*p;
		float			MaxInvZ;

		/* Try to sort things into their correct place in life.
		 */
		MaxInvZ = pElement->MaxInvZ;

		/* Does it go into the first position.
		 */
		if (MaxInvZ < pHead->MaxInvZ)
		{
			/* Put into the first position.
			 */
			pElement->pPost = pHead;
			pHead->pPre = pElement;
			pHead = pElement;
			pHead->pPre = NULL;
		}
		else
		{
			/* Search for insertion point. Start at the head.
			 */
			p = pHead;
			while ((MaxInvZ >= p->MaxInvZ) && (p->pPost != NULL))
			{
				/* Next element.
				 */
				p = p->pPost;
			}

			/* Insert in after p.
			 */
			pElement->pPre = p;
			pElement->pPost = p->pPost;
			p->pPost = pElement;
		}
	}

	/*  Sorted baby !!!
	 */
	return (pHead);
}
#endif /*!DOING_SGL_STYLE_SORTING*/

/**************************************************************************
 * Function Name  : AllocTransFaceList
 * Inputs         : pRegion   - region requiring a new TRANSFACE_LIST object
 *				  : SetId     - TransSetId of new set requested
 *                : Entry     - First entry in new set
 *                : EntryZ    - Z of first entry in new set
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : pNextTransFace, FreeObjChunk
 * Description    : Called when pRegion->CurTSetId needs to change
 *				   
**************************************************************************/
static INLINE void AllocTransFaceList( REGION_HEADER *pRegion,
						sgl_uint32 SetId, sgl_uint32 Entry, float EntryZ )
{
	TRANSFACE_LIST *pNew = pNextTransFace++;

	if ( PTR_SET_EMPTY( pNew, OBJECTS_PER_BLOCK ) )
	{
		sgl_uint32 *pBlock = NULL;
		
		/* Allocate new OBJECT_BLOCK to hold head of TRANSOBJ_BLOCK list */
		pNew = ((TRANSFACE_LIST *) AllocObjectBlock( &pBlock, Entry )) + 1;

		/* We use the next segment of the OBJECT_BLOCK as the TRANSFACE_LIST */
		pNew->pLastSlot = pBlock;

		/* Setup next allocation */
		pNextTransFace = pNew + 1;
	}
	else
	{
		/* List is initially empty so insert first entry */
		pNew->pLastSlot = NULL;
		AllocTransObjBlock( &pNew->pLastSlot, Entry );
	}


	/* Initialise new TRANSFACE_LIST with NearestZ of first EntryZ */
	pNew->MaxInvZ   = EntryZ;

ASSERT(pNew);

#if DOING_SGL_STYLE_SORTING
	pNew->pPre       = pRegion->pCurTSet[SetId & 1];
	/* pNew->pPost     = ?; is done later when list is sorted */

	/* Setup new TRANS SET fields on region */
	pRegion->pCurTSet[SetId & 1] = pNew;
	pRegion->CurTSetId[SetId & 1] = SetId;
#else
	/* Setup new TRANS SET fields on region */
	/* Try and insertion sort the passes.
	 */
	pRegion->pCurTSet = InsertionSort(pRegion->pCurTSet,pNew);
	pRegion->CurTSetId = SetId;
#endif
}

/**************************************************************************
 **************************************************************************

	Object handling code, first we Add all objects then generate the output

 **************************************************************************
 **************************************************************************/

/**************************************************************************
 * Function Name  : AddRegionSolid
 * Inputs         : XYData  - 2D Extent encoded into a DWORD
 *                : Planes  - Planes in object
 *                : ISPAddr - ISP address of object
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a single OPAQUE object to the scene
**************************************************************************/
void AddRegionSolid( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr )
{
	sgl_uint32 NextISPAddr;
	int RY0, RY1, RX0, RX1;
#if PCX1 
	int HSlot;
#endif

	ASSERT((ISPAddr<(PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit - Planes)));

	/* Need to predict NextISPAddr for future use */	
#if PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)
	ASSERT(WORDS_PER_PLANE == 3);
	NextISPAddr = ISPAddr + Planes + Planes + Planes;
#else
	ASSERT(WORDS_PER_PLANE == 4);
	NextISPAddr = ISPAddr + (Planes << 2) ;
#endif

	OpaqueId++;

	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;
		
	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;

		/* Advance RY0 to first line of next strip */
#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
 		RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
		
		if ( RY0 <= RY1 )
		{
			/* If object extends to next strip it is SAD */
			HSlot = SAD;
		}
		else
		if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
		{
			/* If strip crossed half point of strip it is HAPPY */
			HSlot = HAPPY;
		}
		else
		{
			/* It is BUSY if the strip should be divided */
			HSlot = BUSY;
		}
#else
		RY0 = BaseOfStrips[RY0] + pStrip->Height;
#endif

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			sgl_uint32 DeltaId;

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			/* Record object catagory within each region */
			pRegion->HeightStats[HSlot] += Planes;
#endif

			if ( ( pRegion->OpaquePlanes += Planes ) >=
				 (REGION_PLANE_LIM-(SAFETY_MARGIN_OPAQ*2)) )
			{
				/* Too many opaque objects make you sad */
				 pRegion->OpaquePlanes -=Planes;
				continue;
			}
		
			/* Setup for recognition of next object */
			DeltaId = OpaqueId - pRegion->PrevOpaqueId;
			pRegion->PrevOpaqueId = NextISPAddr;
	
			if ( DeltaId == 1 )
			{
				/* Previous OPAQUE can merge with this object */
				DeltaId = *(pRegion->pLastSlots[OPAQUE]);
						
				/* Accumulate plane count */
				DeltaId += ( Planes << OBJ_PCOUNT_SHIFT );

				*(pRegion->pLastSlots[OPAQUE]) = DeltaId;
			}
			else
			if ( !PTR_SET_FULL( pRegion->pLastSlots[OPAQUE],
													OBJECTS_PER_BLOCK ) )
			{
				/* Add the data */
				PTR_SET_ADD( pRegion->pLastSlots[OPAQUE],
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
			else
			{
				/* New block to hold this entry please */
				AllocObjectBlock( pRegion->pLastSlots+OPAQUE,
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
		}
		while ( ++pRegion != pLastReg );
	}
#if PCX1
	while ( HSlot == SAD );
#else
	while ( RY0 <= RY1 );
#endif
}

/**************************************************************************
 * Function Name  : AddRegionAtmos
 * Inputs         : XYData  - 2D Extent and type encoded into a DWORD
 *                : Planes  - Planes in object
 *                : ISPAddr - ISP address of 1st object description
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a LIGHTVOL or SHADOW object to the scene for SGL
 *                  This uses long lists as auto generated shadows can cause
 *                  Hundreds off shadow objects to be added
**************************************************************************/
void AddRegionAtmos( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr )
{
	sgl_uint32 NextISPAddr;
	int RY0, RY1, RX0, RX1;
	int Type = (XYData>>29); /* Shadow or Light Vol */
#if PCX1
	int HSlot;
#endif

	ASSERT((ISPAddr<(PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit - Planes)));

	/* Need to predict NextISPAddr for future use */	
#if PCX1   || ((PCX2 || PCX2_003) && FORCE_NO_FPU)
	ASSERT(WORDS_PER_PLANE == 3);
	NextISPAddr = ISPAddr + Planes + Planes + Planes;
#else
	ASSERT(WORDS_PER_PLANE == 4);
	NextISPAddr = ISPAddr + (Planes << 2) ;
#endif

	OpaqueId++;

	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;
		
	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;

		/* Advance RY0 to first line of next strip */

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
 		RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
		
		if ( RY0 <= RY1 )
		{
			/* If object extends to next strip it is SAD */
			HSlot = SAD;
		}
		else
		if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
		{
			/* If strip crossed half point of strip it is HAPPY */
			HSlot = HAPPY;
		}
		else
		{
			/* It is BUSY if the strip should be divided */
			HSlot = BUSY;
		}
#else
		RY0 = BaseOfStrips[RY0] + pStrip->Height;
#endif

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			sgl_uint32 DeltaId;
			
#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			/* Record object catagory within each region */
			pRegion->HeightStats[HSlot] += Planes;
#endif

			/* Setup for recognition of next object */
			DeltaId = OpaqueId - pRegion->PrevOpaqueId;
			pRegion->PrevOpaqueId = NextISPAddr;
	
			if ( DeltaId == 1 )
			{
				/* Previous OPAQUE can merge with this object */
				DeltaId = *(pRegion->pLastSlots[Type]);
						
				/* Accumulate plane count */
				DeltaId += ( Planes << OBJ_PCOUNT_SHIFT );

				*(pRegion->pLastSlots[Type]) = DeltaId;
			}
			else
			if ( !PTR_SET_FULL( pRegion->pLastSlots[Type],
													OBJECTS_PER_BLOCK ) )
			{
				/* Add the data */
				PTR_SET_ADD( pRegion->pLastSlots[Type],
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
			else
			{
				/* New block to hold this entry please */
				AllocObjectBlock( pRegion->pLastSlots+Type,
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
		}
		while ( ++pRegion != pLastReg );
	}
#if PCX1
	while ( HSlot == SAD );
#else
	while ( RY0 <= RY1 );
#endif
}

/**************************************************************************
 * Function Name  : AddRegionShadorLV
 * Inputs         : XYData  - 2D Extent and type encoded into a DWORD
 *                : Planes  - Planes in object
 *                : ISPAddr - ISP address of 1st object description
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a single LIGHTVOL or SHADOW object to the scene 
 *                  for SGLLite - thus this routine is local to this module.
**************************************************************************/
static void AddRegionShadorLV( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr )
{
	int RY0, RY1, RX0, RX1;
#if PCX1
	int HSlot;
#endif
	
	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;

	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;

		/* Advance RY0 to first line of next strip */

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
		RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
		
		if ( RY0 <= RY1 )
		{
			/* If object extends to next strip it is SAD */
			HSlot = SAD;
		}
		else
		if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
		{
			/* If strip crossed half point of strip it is HAPPY */
			HSlot = HAPPY;
		}
		else
		{
			/* It is BUSY if the strip should be divided */
			HSlot = BUSY;
		}
#else
		RY0 = BaseOfStrips[RY0] + pStrip->Height;
#endif

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			sgl_uint32 **rpLastSlot = pRegion->pLastSlots + (XYData>>29);

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			/* Record object catagory within each region */
			pRegion->HeightStats[HSlot] += Planes;
#endif

			if ( !PTR_SET_FULL( *rpLastSlot, TRANSOBJS_PER_BLOCK) )
			{
				/* Add the data */
				PTR_SET_ADD( *rpLastSlot, ISPAddr +
							(Planes<<OBJ_PCOUNT_SHIFT) );
			}
			else
			{
				/* New block to hold this entry please */
				AllocTransObjBlock( rpLastSlot, ISPAddr +
								    (Planes<<OBJ_PCOUNT_SHIFT) );
			}
		}
		while ( ++pRegion != pLastReg );
	}
#if PCX1
	while ( HSlot == SAD );
#else
	while ( RY0 <= RY1 );
#endif
}

/**************************************************************************
 * Function Name  : AddRegionSeeThru
 * Inputs         : XYData     - 2D Extent encoded into a DWORD
 *				  : TransSetId - Identifier of translucent set
 *                : Planes     - Planes in object
 *                : ISPAddr    - ISP address of 1st object description
 *                : ZData      - invZ = Base + Xpix*Dx + Yline*Dy OR
 *                :            - (-Z) = Base + Xpix*Dx + Yline*Dy
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a single Translucent object to the scene.
 *                  
**************************************************************************/
void AddRegionSeeThru( sgl_uint32 XYData, sgl_uint32 TransSetId, sgl_uint32 Planes,
					   sgl_uint32 ISPAddr, const TRANS_REGION_DEPTHS_STRUCT *pZData)
{
	int RY0, RY1, RX0, RX1;
#if  PCX1
	int HSlot;
#endif

   	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;
	
	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;
		float Depth;

		/* Advance RY0 to first line of next strip, then Calculate 
		   starting depth of object on this strip (mid Y used) */
#if PCX1
		RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
		Depth = pZData->BaseDepth;
#else
		RY0 = BaseOfStrips[RY0] + pStrip->Height;
		Depth = pZData->BaseDepth;
#endif


#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
		if ( RY0 <= RY1 )
		{
			/* If object extends to next strip it is SAD */
			HSlot = SAD;
		}
		else
		if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
		{
			/* If strip crossed half point of strip it is HAPPY */
			HSlot = HAPPY;
		}
		else
		{
			/* It is BUSY if the strip should be divided */
			HSlot = BUSY;
		}
#endif

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			TRANSFACE_LIST *pList;

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			/* Record object catagory within each region */
			pRegion->HeightStats[HSlot] += Planes;
#endif
			/* Here is where we decide if a new pass is needed.
			 * Need to check if maximum allowable number of passes
			 * has been exceeded.
			 */
#if DOING_SGL_STYLE_SORTING
			if (TransSetId != pRegion->CurTSetId[TransSetId&1])
#else
			if (TransSetId != pRegion->CurTSetId)
#endif
			{
				/* Increment pass count.
				 */
				pRegion->nPassCount++;

				/* Setup a new list */
				AllocTransFaceList( pRegion, TransSetId, ISPAddr+
									(Planes<<OBJ_PCOUNT_SHIFT), Depth );
				continue;
			}
						
			/* Add to current set */
#if DOING_SGL_STYLE_SORTING
			pList = pRegion->pCurTSet[TransSetId&1];

			if(!pList)
			{
				pList = pRegion->pCurTSet[(~TransSetId)&1];
			}

#else
			pList = pRegion->pCurTSet;

			ASSERT(pList);
#endif

			if ( Depth > pList->MaxInvZ )
			{
				/* New nearest object invZ gets smaller with deeper objects */
				pList->MaxInvZ = Depth;
			}

			if ( !PTR_SET_FULL(pList->pLastSlot, TRANSOBJS_PER_BLOCK) )
			{
				/* Add the data */
				PTR_SET_ADD( pList->pLastSlot, ISPAddr +
								(Planes<<OBJ_PCOUNT_SHIFT) );
			}
			else
			{
				/* New block to hold this entry please */
				AllocTransObjBlock( &(pList->pLastSlot), ISPAddr +
								    (Planes<<OBJ_PCOUNT_SHIFT) );
			}
		}
		while ( ++pRegion != pLastReg );
	}
#if PCX1
	while ( HSlot == SAD );
#else
	while ( RY0 <= RY1 );
#endif

#if DAG_TRANS_SORTING
	/* Increment the tranlucent triangle count.
	 */
	guTransTriCounter++;
#endif
}

#if DAG_TRANS_SORTING
/**************************************************************************
 * Function Name  : AllocNewTransTriIndices
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  : Global TRANSTRIINDEX_STRUCT space and counter
 * Returns        : New PTRANSTRIINDEX_STRUCT
 * Globals Used   : gpcTransTriIndicesWorkspace{Ptr | Counter}
 * Description    : Grabs more (pre-allocated) space for translucent
 *					triangles in this region.
**************************************************************************/

static PTRANSTRIINDEX_STRUCT AllocNewTransTriIndices( void )
{
	PTRANSTRIINDEX_STRUCT	psTriIndex;
	
	if (gpcTransTriIndicesWorkspacePtr >
		(gpcTransTriIndicesWorkspace + gpcTransTriIndicesWorkspaceStart) )
	{
		DPF((DBG_ERROR,"AllocNewTransTriIndices: ws=%08X, ptr=%08x, ws+limit=%08X\n",
					 gpcTransTriIndicesWorkspace, gpcTransTriIndicesWorkspacePtr,
					 (gpcTransTriIndicesWorkspace + gpcTransTriIndicesWorkspaceStart)));

		return (NULL);
	}
	psTriIndex = (PTRANSTRIINDEX_STRUCT)gpcTransTriIndicesWorkspacePtr;
	gpcTransTriIndicesWorkspacePtr +=sizeof( TRANSTRIINDEX_STRUCT );
	
	return (psTriIndex);
}

/**************************************************************************
 * Function Name  : AddRegionSeeThruD3D
 * Inputs         : XYData     - 2D Extent encoded into a DWORD
 *				  : TransSetId - Identifier of translucent set
 *                : Planes     - Planes in object
 *                : ISPAddr    - ISP address of 1st object description
 *                : ZData      - invZ = Base + Xpix*Dx + Yline*Dy OR
 *                :            - (-Z) = Base + Xpix*Dx + Yline*Dy
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a single Translucent object to the scene.
 *                  
**************************************************************************/
static void AddRegionSeeThruD3D( PITRI *rpTri, sgl_uint32 XYData, 
								 PITRIPLUS *rpTriPlus, sgl_uint32 Planes,
								 sgl_uint32 ISPAddr )
{
	int RY0, RY1, RX0, RX1;
	PITRI	pTri = *rpTri;	
	PITRIPLUS	pTriPlus = *rpTriPlus;
	float fInvNorm[3];
	float fTemp0, fTemp1, fTemp2;

#if PCX1
	int HSlot;
#endif

	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;

	/* Add the triangle (once) to the global array */

	fTemp0 = sfabs(pTri->fAdjoint[0][0]) + sfabs(pTri->fAdjoint[1][0]);
	fTemp1 = sfabs(pTri->fAdjoint[0][1]) + sfabs(pTri->fAdjoint[1][1]);
	fTemp2 = sfabs(pTri->fAdjoint[0][2]) + sfabs(pTri->fAdjoint[1][2]);

	fInvNorm[0] = 1.0f / fTemp0;
	fInvNorm[1] = 1.0f / fTemp1;
	fInvNorm[2] = 1.0f / fTemp2;

	/* Try to save a bit of time by using the reciprocal in the normalisation */

	fTemp0 = pTri->fAdjoint[0][0] * fInvNorm[0];
	fTemp1 = pTri->fAdjoint[0][1] * fInvNorm[1];	
	fTemp2 = pTri->fAdjoint[0][2] * fInvNorm[2];	
		
	gpTransTris[guTransTriCounter].fAdjoint[0][0] = fTemp0;
	gpTransTris[guTransTriCounter].fAdjoint[0][1] = fTemp1;
	gpTransTris[guTransTriCounter].fAdjoint[0][2] = fTemp2;

	fTemp0 = pTri->fAdjoint[1][0] * fInvNorm[0];
	fTemp1 = pTri->fAdjoint[1][1] * fInvNorm[1];	
	fTemp2 = pTri->fAdjoint[1][2] * fInvNorm[2];	

	gpTransTris[guTransTriCounter].fAdjoint[1][0] = fTemp0;
	gpTransTris[guTransTriCounter].fAdjoint[1][1] = fTemp1;
	gpTransTris[guTransTriCounter].fAdjoint[1][2] = fTemp2;

	fTemp0 = pTri->fAdjoint[2][0] * fInvNorm[0];
	fTemp1 = pTri->fAdjoint[2][1] * fInvNorm[1];	
	fTemp2 = pTri->fAdjoint[2][2] * fInvNorm[2];	

	gpTransTris[guTransTriCounter].fAdjoint[2][0] = fTemp0;
	gpTransTris[guTransTriCounter].fAdjoint[2][1] = fTemp1;
	gpTransTris[guTransTriCounter].fAdjoint[2][2] = fTemp2;

	/* Calculate the centroid Z, as only fVerts[0][2] is used */
	fTemp0 = (pTri->fZ[0] + pTri->fZ[1] + pTri->fZ[2]) * (1.0f/3.0f);

	gpTransTris[guTransTriCounter].fVerts[0][0] = pTriPlus->fX[0];
	gpTransTris[guTransTriCounter].fVerts[0][1] = pTriPlus->fY[0];

	gpTransTris[guTransTriCounter].fVerts[1][0] = pTriPlus->fX[1];
	gpTransTris[guTransTriCounter].fVerts[1][1] = pTriPlus->fY[1];
	
	gpTransTris[guTransTriCounter].fVerts[2][0] = pTriPlus->fX[2];
	gpTransTris[guTransTriCounter].fVerts[2][1] = pTriPlus->fY[2];													  

	gpTransTris[guTransTriCounter].fA = pTriPlus->fA;
	gpTransTris[guTransTriCounter].fB = pTriPlus->fB;			
	gpTransTris[guTransTriCounter].fC = pTriPlus->fC;			

	gpTransTris[guTransTriCounter].fMeanZ = fTemp0;

	gpTransTris[guTransTriCounter].uObjPtr = ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT);

#if 0
	DPF((DBG_FATAL,"G %i: V0 {%7.03f,%7.03f,%7.03f)",guTransTriCounter,gpTransTris[guTransTriCounter].fVerts[0][0],gpTransTris[guTransTriCounter].fVerts[0][1],gpTransTris[guTransTriCounter].fMeanZ));
	DPF((DBG_FATAL,"G %i: V1 {%7.03f,%7.03f)",guTransTriCounter,gpTransTris[guTransTriCounter].fVerts[1][0],gpTransTris[guTransTriCounter].fVerts[1][1]));
	DPF((DBG_FATAL,"G %i: V2 {%7.03f,%7.03f)",guTransTriCounter,gpTransTris[guTransTriCounter].fVerts[2][0],gpTransTris[guTransTriCounter].fVerts[2][1]));	

	DPF((DBG_FATAL,"G %i: A {%7.04f,%7.04f,%7.04f)",guTransTriCounter,gpTransTris[guTransTriCounter].fAdjoint[0][0],gpTransTris[guTransTriCounter].fAdjoint[0][1],gpTransTris[guTransTriCounter].fAdjoint[0][2]));
	DPF((DBG_FATAL,"G %i: A {%7.04f,%7.04f,%7.04f)",guTransTriCounter,gpTransTris[guTransTriCounter].fAdjoint[1][0],gpTransTris[guTransTriCounter].fAdjoint[1][1],gpTransTris[guTransTriCounter].fAdjoint[1][2]));
	DPF((DBG_FATAL,"G %i: A {%7.04f,%7.04f,%7.04f)",guTransTriCounter,gpTransTris[guTransTriCounter].fAdjoint[2][0],gpTransTris[guTransTriCounter].fAdjoint[2][1],gpTransTris[guTransTriCounter].fAdjoint[2][2]));	
#endif
	
	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;
		float Depth;

		/* Advance RY0 to first line of next strip, then Calculate 
		   starting depth of object on this strip (mid Y used) */

#if PCX1
		RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
#else
		RY0 = BaseOfStrips[RY0] + pStrip->Height;
#endif


#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
		if ( RY0 <= RY1 )
		{
			/* If object extends to next strip it is SAD */
			HSlot = SAD;
		}
		else
		if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
		{
			/* If strip crossed half point of strip it is HAPPY */
			HSlot = HAPPY;
		}
		else
		{
			/* It is BUSY if the strip should be divided */
			HSlot = BUSY;
		}
#endif

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			TRANSFACE_LIST *pList;
#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			/* Record object catagory within each region */
			pRegion->HeightStats[HSlot] += Planes;
#endif

			if (pRegion->usTransTriCounter != MAX_NUM_REGION_TRIS)
			{
				sgl_uint32 uBlockCount = (sgl_uint32) pRegion->usTransTriCounter % NUM_INDICES_IN_BLOCK;
				if (uBlockCount == 0)
				{				
					PTRANSTRIINDEX_STRUCT	psTriIndex;				
					psTriIndex = AllocNewTransTriIndices();

					psTriIndex->pNext = pRegion->uTransTriList;
					
					pRegion->uTransTriList = psTriIndex;
				}
				pRegion->usTransTriCounter++;
			 	pRegion->uTransTriList->usIndex[uBlockCount] = guTransTriCounter;
			}

		}
		while ( ++pRegion != pLastReg );
	}
#if PCX1
	while ( HSlot == SAD );
#else
	while ( RY0 <= RY1 );
#endif
	
	guTransTriCounter++;
}
#endif /* DAG_TRANS_SORTING */

/**************************************************************************
 * Function Name  : AddRegionTransOpaque
 * Inputs         : XYData  - 2D Extent and type encoded into a DWORD
 *                : Planes  - Planes in object
 *                : ISPAddr - ISP address of 1st object description
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a combined TransOpaque object to the scene for SGLlite
 *                  This uses long lists as they are more memory efficient. 
**************************************************************************/
static void AddRegionTransOpaque( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr )
{
	sgl_uint32 NextISPAddr;
	int RY0, RY1, RX0, RX1;
#if PCX1
	int HSlot;
#endif
	int Type = (XYData>>29); /* TransOpaque - hopefully */

	ASSERT((ISPAddr<(PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit - Planes)));

	/* Need to predict NextISPAddr for future use */	
#if PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)
	ASSERT(WORDS_PER_PLANE == 3);
	NextISPAddr = ISPAddr + Planes + Planes + Planes;
#else
	ASSERT(WORDS_PER_PLANE == 4);
	NextISPAddr = ISPAddr + (Planes << 2) ;
#endif

	TransOpaqueId++;

	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;
		
	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;

		/* Advance RY0 to first line of next strip */

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
 		RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
		
		if ( RY0 <= RY1 )
		{
			/* If object extends to next strip it is SAD */
			HSlot = SAD;
		}
		else
		if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
		{
			/* If strip crossed half point of strip it is HAPPY */
			HSlot = HAPPY;
		}
		else
		{
			/* It is BUSY if the strip should be divided */
			HSlot = BUSY;
		}
#else
		RY0 = BaseOfStrips[RY0] + pStrip->Height;
#endif

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			sgl_uint32 DeltaId;
			
#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			/* Record object catagory within each region */
			pRegion->HeightStats[HSlot] += Planes;
#endif
			pRegion->TransOpaquePlanes += Planes;

			/* Setup for recognition of next object */
			DeltaId = TransOpaqueId - pRegion->PrevTransOpaqueId;
			pRegion->PrevTransOpaqueId = NextISPAddr;
	
			if ( DeltaId == 1 )
			{
				/* Previous TRANSOPAQUE can merge with this object */
				DeltaId = *(pRegion->pLastSlots[Type]);
						
				/* Accumulate plane count */
				DeltaId += ( Planes << OBJ_PCOUNT_SHIFT );

				*(pRegion->pLastSlots[Type]) = DeltaId;
			}
			else
			if ( !PTR_SET_FULL( pRegion->pLastSlots[Type],
													OBJECTS_PER_BLOCK ) )
			{
				/* Add the data */
				PTR_SET_ADD( pRegion->pLastSlots[Type],
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
			else
			{
				/* New block to hold this entry please */
				AllocObjectBlock( pRegion->pLastSlots+Type,
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
		}
		while ( ++pRegion != pLastReg );
	}
#if PCX1
	while ( HSlot == SAD );
#else
	while ( RY0 <= RY1 );
#endif
}

/**************************************************************************
 * Function Name  : AddRegionObjects
 * Inputs         : pXYData       - Type and XY data for each object
 *                : nXYDataInc    - sizeof structure containing XYData
 *                : PlanesPerPoly - Planes in each object
 *                : nPolys        - Number of objects
 *                : ISPAddr       - ISP address of 1st object description
 *                : pZData        - Z data on each translucent object
 *                : nZDataInc     - sizeof structure containing ZData
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : The routine by which all objects are introduced into
 *                  this module for rendering.
 *
 *                  Please 
 *				   
**************************************************************************/
void AddRegionObjects( sgl_uint32 *pXYData, int nXYDataInc,
                       sgl_uint32 PlanesPerPoly, int nPolys, sgl_uint32 ISPAddr,
					   const TRANS_REGION_DEPTHS_STRUCT *pZData, int nZDataInc)
{
	sgl_uint32 Mask = SigXYMask;
	sgl_uint32 Type = (*pXYData)>>29;
		
	if ( Type == OPAQUE )
	{
		/* Opaque objects need to go as fast as possible (all same type) */
		SGL_TIME_START(ADD_REGION_OPAQUE_TIME);

		/* Merging code in this case has no guard cases for ptr overflow */
		ASSERT( ( ( PlanesPerPoly * nPolys ) < OBJ_PCOUNT_MASK ) );
	
	   	/* Start new block of OpaqueIds for this group */
		OpaqueId++;

		do
		{	/* Process each object */
			sgl_uint32 XYData = Mask & *pXYData;
			sgl_int32 Planes = PlanesPerPoly;
			int RY0, RY1, RX0, RX1;
#if PCX1
			int HSlot;
#endif		
			*((sgl_uint8 **) &pXYData) += nXYDataInc;
	
			while ( ( nPolys != 1 )                   &&
					( XYData == ( Mask & *pXYData ) )    )
			{
				/* Next object requires identical processing */
				Planes += PlanesPerPoly;
				nPolys--;
				*((sgl_uint8 **) &pXYData) += nXYDataInc;
			}

			/* Allocate one OpaqueId to each insertion */
			OpaqueId++;
						
			/* Start at the first Y line effected */
			RX1 =  XYData      & 0x1f;
			RX0 = (XYData>> 5) & 0x1f;
			RY1 = (XYData>>10) & 0x1ff;
			RY0 = (XYData>>19) & 0x1ff;
		
			do
			{
				/* Get pointer to the strip containing this ROW */
				REGION_STRIP *pStrip = pRegionStrips[RY0];
				REGION_HEADER *pRegion, *pLastReg;

				/* Advance RY0 to first line of next strip */

		#if PCX1
		/* Disable dynamic tile sizing for PCX2.
		 */
 				RY0 = BaseOfStrips[HSlot = RY0] + pStrip->Height;
				
				if ( RY0 <= RY1 )
				{
					/* If object extends to next strip it is SAD */
					HSlot = SAD;
				}
				else
				if ( ( (HSlot ^ RY1) & (pStrip->Height>>1) ) != 0 )
				{
					/* If strip crossed half point of strip it is HAPPY */
					HSlot = HAPPY;
				}
				else
				{
					/* It is BUSY if the strip should be divided */
					HSlot = BUSY;
				}
		#else
				RY0 = BaseOfStrips[RY0] + pStrip->Height;
		#endif

				/* Get pointers to the region of interest */
				pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
				pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

				do
				{
					sgl_int32 DeltaId;
			
#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
					/* Record object catagory within each region */
					pRegion->HeightStats[HSlot] += Planes;
#endif

					if ( ( pRegion->OpaquePlanes += Planes ) >
								(REGION_PLANE_LIM-(SAFETY_MARGIN_OPAQ*2)) )
					{
						/* Too many opaque objects make you sad */
						 pRegion->OpaquePlanes -=Planes;
						continue;
					}

					/* Setup for recognition of next object */
					DeltaId = OpaqueId - pRegion->PrevOpaqueId;
					pRegion->PrevOpaqueId = OpaqueId;
	
					if ( DeltaId == 1 )
					{
						/* Previous OPAQUE merges object */
						DeltaId = *(pRegion->pLastSlots[Type]);
						
						/* Accumulate plane count */
						DeltaId += ( Planes << OBJ_PCOUNT_SHIFT );

						/* Update object pointer and we are done */
						*(pRegion->pLastSlots[Type]) = DeltaId;
					}
					else
					if ( !PTR_SET_FULL( pRegion->pLastSlots[Type],
													OBJECTS_PER_BLOCK ) )
					{
						/* Add the data */
						PTR_SET_ADD( pRegion->pLastSlots[Type],
									ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
					}
					else
					{
						/* New block to hold this entry please */
						AllocObjectBlock( pRegion->pLastSlots+Type,
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
					}
				}
				while ( ++pRegion != pLastReg );
			}
#if PCX1
			while ( HSlot == SAD );
#else
			while ( RY0 <= RY1 );
#endif
	
			/* Advance ISPAddr */
#if PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)
			ASSERT(WORDS_PER_PLANE == 3);
			ISPAddr += Planes + Planes + Planes;
#else
			ASSERT(WORDS_PER_PLANE == 4);
			ISPAddr += (Planes << 2) ;
#endif
		} while ( --nPolys != 0 );

		SGL_TIME_STOP(ADD_REGION_OPAQUE_TIME);
	}
	else /* Not OPAQUE */
	{
		do
		{	/* Process each object */
			sgl_uint32 XYData;

			XYData = Mask & *pXYData;
			*((sgl_uint8 **) &pXYData) += nXYDataInc;

			/* Decode the type of each complex object */
			Type = (XYData)>>29;
	
	
			if ( Type == PACKED_TYPE_SHADOW || Type == PACKED_TYPE_LIGHTVOL )
			{
				/* Adds SHADOW and LIGHTVOL objects */
				AddRegionShadorLV( XYData, PlanesPerPoly, ISPAddr );
			}
			else if( Type == PACKED_TYPE_OPAQUETRANS )
			{
				AddRegionTransOpaque(XYData, PlanesPerPoly, ISPAddr );
			}
			else
			{
				sgl_int32	TransSetId;

				if 	( ( Type & PACKED_TRANSTYPE_SETMARK ) == 0 )
				{
					/* Object is part of current FRONT/BACK object set */
				#if DOING_SGL_STYLE_SORTING
					TransSetId = CurrentTransSetId[Type & 1];
				#else
					TransSetId = CurrentTransSetId;
				#endif
				}
				else
				{
					/* Object is part of next FRONT/BACK object set */
				#if DOING_SGL_STYLE_SORTING
					TransSetId = ( CurrentTransSetId[Type & 1] += 2 );
				#else
					TransSetId = ( CurrentTransSetId += 2 );
				#endif
				}

				/* Add translucent object to scene */
				AddRegionSeeThru( XYData, TransSetId, PlanesPerPoly,
														ISPAddr, pZData );

				*((sgl_uint8 **) &pZData) += nZDataInc;
			}
		
			/* Calculate ISPAddr of next object */
#if PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)
			ASSERT(WORDS_PER_PLANE == 3);
			ISPAddr = ISPAddr + PlanesPerPoly + PlanesPerPoly + PlanesPerPoly;
#else
			ASSERT(WORDS_PER_PLANE == 4);
			ISPAddr = ISPAddr + (PlanesPerPoly << 2) ;
#endif
		} while ( --nPolys != 0 );
	}
}

#if PCX2 || PCX2_003

/**************************************************************************
 * Function Name  : AddRegionHighlightFog
 * Inputs         : XYData  - 2D Extent and type encoded into a DWORD
 *                : Planes  - Planes in object
 *                : ISPAddr - ISP address of 1st object description
 *				  : type -    the type of GOURAUDHIGHLIGHT of VERTEXFOG	
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : Adds a combined VertexFog or GOURAUDHhighlight object to the scene
 *                  for SGLlite. This uses long lists as they are more memory efficient. 
**************************************************************************/
static void AddRegionHighlightFog( sgl_uint32 XYData, sgl_uint32 Planes, sgl_uint32 ISPAddr,
 									int Type)
{
	int RY0, RY1, RX0, RX1;
	
	ASSERT((ISPAddr<(PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit - Planes)));

	/* Start at the first Y line effected */
	RX1 =  XYData      & 0x1f;
	RX0 = (XYData>> 5) & 0x1f;
	RY1 = (XYData>>10) & 0x1ff;
	RY0 = (XYData>>19) & 0x1ff;
		
	do
	{
		/* Get pointer to the strip containing this ROW */
		REGION_STRIP *pStrip = pRegionStrips[RY0];
		REGION_HEADER *pRegion, *pLastReg;

		/* Advance RY0 to first line of next strip */
		RY0 = BaseOfStrips[RY0] + pStrip->Height;

		/* Get pointers to the region of interest */
		pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
		pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

		do
		{
			pRegion->ExtraPlanes[Type] += Planes;

			if ( !PTR_SET_FULL( pRegion->pExtraSlots[Type],
													OBJECTS_PER_BLOCK ) )
			{
				/* Add the data */
				PTR_SET_ADD( pRegion->pExtraSlots[Type],
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
			else
			{
				/* New block to hold this entry please */
				AllocObjectBlock( pRegion->pExtraSlots+Type,
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
			}
		}
		while ( ++pRegion != pLastReg );
	}
	while ( RY0 <= RY1 );
}

/**************************************************************************
 * Function Name  : AddRegionObjectsExtra
 * Inputs         : pXYData       - Type and XY data for each object
 *                : nXYDataInc    - sizeof structure containing XYData
 *                : PlanesPerPoly - Planes in each object
 *                : nPolys        - Number of objects
 *                : ISPAddr       - ISP address of 1st object description
 *                : pZData        - Z data on each translucent object
 *                : nZDataInc     - sizeof structure containing ZData
 *                : FogHighlight  - flag for smooth shading fog and high light
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : The routine by which all objects are introduced into
 *                  this module for rendering.
 *
 *					This routine takes an extra parameter pFogHighlight for vertex fog
 *                  and GOURAUD highlight.       
 *				   
**************************************************************************/
void AddRegionObjectsExtra( sgl_uint32 *pXYData, int nXYDataInc,
                       sgl_uint32 PlanesPerPoly, int nPolys, sgl_uint32 ISPAddr,
					   const TRANS_REGION_DEPTHS_STRUCT *pZData, int nZDataInc, 
					   PFOGHIGHLIGHT pFogHighlight)
{
	sgl_uint32 Mask = SigXYMask;
	sgl_uint32 Type = (*pXYData)>>29;
		
	if ( Type == OPAQUE )
	{
		/* Opaque objects need to go as fast as possible (all same type) */
		SGL_TIME_START(ADD_REGION_OPAQUE_TIME);

		/* Merging code in this case has no guard cases for ptr overflow */
		ASSERT( ( ( PlanesPerPoly * nPolys ) < OBJ_PCOUNT_MASK ) );
	
		/* Start new block of OpaqueIds for this group */
		OpaqueId++;

		do
		{	/* Process each object */
			sgl_uint32 XYData = Mask & *pXYData;
			sgl_int32 Planes = PlanesPerPoly;
			sgl_int32 VFogPlanes;
			sgl_int32 SmoothHighPlanes;
			int RY0, RY1, RX0, RX1;
	
			*((sgl_uint8 **) &pXYData) += nXYDataInc;

			SmoothHighPlanes = (*pFogHighlight & GOURAUD_HIGHLIGHT)?1:0;	
			VFogPlanes = (*pFogHighlight & VERTEX_FOG)?1:0;	
			pFogHighlight++;
			/* Allocate one OpaqueId to each insertion */
			OpaqueId++;
						
			/* Start at the first Y line effected */
			RX1 =  XYData      & 0x1f;
			RX0 = (XYData>> 5) & 0x1f;
			RY1 = (XYData>>10) & 0x1ff;
			RY0 = (XYData>>19) & 0x1ff;
		
			do
			{
				/* Get pointer to the strip containing this ROW */
				REGION_STRIP *pStrip = pRegionStrips[RY0];
				REGION_HEADER *pRegion, *pLastReg;

				/* Advance RY0 to first line of next strip */
				RY0 = BaseOfStrips[RY0] + pStrip->Height;

				/* Get pointers to the region of interest */
				pRegion	 = pStrip->Regions + (RX0>>pStrip->Width);
				pLastReg = pStrip->Regions + (RX1>>pStrip->Width) + 1;

				do
				{
					if ( ( pRegion->OpaquePlanes += Planes ) >
								(REGION_PLANE_LIM-(SAFETY_MARGIN_OPAQ*2)) )
					{
						/* Too many opaque objects make you sad */
						 pRegion->OpaquePlanes -=Planes;
						continue;
					}

					/* Setup for recognition of next object */
					pRegion->PrevOpaqueId = OpaqueId;
	
					if ( !PTR_SET_FULL( pRegion->pLastSlots[Type],
													OBJECTS_PER_BLOCK ) )
					{
						/* Add the data */
						PTR_SET_ADD( pRegion->pLastSlots[Type],
									ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
					}
					else
					{
						/* New block to hold this entry please */
						AllocObjectBlock( pRegion->pLastSlots+Type,
								ISPAddr + (Planes<<OBJ_PCOUNT_SHIFT) );
					}

					/* Add vertex fog and GOURAUD highlight to Opaque objects */
					if (VFogPlanes|SmoothHighPlanes)
					{

						if(SmoothHighPlanes)
						{
							pRegion->ExtraPlanes[GOURAUDHIGHLIGHT] += Planes+1;
							if ( !PTR_SET_FULL( pRegion->pExtraSlots[GOURAUDHIGHLIGHT],
												OBJECTS_PER_BLOCK ) )
							{
								/* Add the data */
								PTR_SET_ADD( pRegion->pExtraSlots[GOURAUDHIGHLIGHT],
											 ISPAddr + 
											 ((Planes+1)<<OBJ_PCOUNT_SHIFT) );
							}
							else
							{
								/* New block to hold this entry please */
								AllocObjectBlock( pRegion->pExtraSlots+GOURAUDHIGHLIGHT,
												  ISPAddr + 
												  ((Planes+1)<<OBJ_PCOUNT_SHIFT) );
							}
						}

						if(VFogPlanes)
						{
							pRegion->ExtraPlanes[VERTEXFOG] += (Planes+1+SmoothHighPlanes);
							if ( !PTR_SET_FULL( pRegion->pExtraSlots[VERTEXFOG],
												OBJECTS_PER_BLOCK ) )
							{
								/* Add the data */
								PTR_SET_ADD( pRegion->pExtraSlots[VERTEXFOG],
											 ISPAddr + 
											 ((Planes+1+SmoothHighPlanes)<<OBJ_PCOUNT_SHIFT) );
							}
							else
							{
								/* New block to hold this entry please */
								AllocObjectBlock( pRegion->pExtraSlots+VERTEXFOG,
												  ISPAddr + 
												  ((Planes+1+SmoothHighPlanes)<<OBJ_PCOUNT_SHIFT) );
							}
						}
					}
				}
				while ( ++pRegion != pLastReg );
			}
			while ( RY0 <= RY1 );

			/* Advance ISPAddr */
			ISPAddr += (Planes+VFogPlanes+SmoothHighPlanes) * WORDS_PER_PLANE;

		} while ( --nPolys != 0 );

		SGL_TIME_STOP(ADD_REGION_OPAQUE_TIME);
	}
	else
	{
		do
		{	/* Process each object */
			sgl_uint32 XYData;
			sgl_uint32 VFogPlanes=0;
			sgl_uint32 SmoothHighPlanes=0;

			SmoothHighPlanes = (*pFogHighlight & GOURAUD_HIGHLIGHT)?1:0;	
			VFogPlanes = (*pFogHighlight & VERTEX_FOG)?1:0;	

			pFogHighlight++;
			XYData = Mask & *pXYData;
			*((sgl_uint8 **) &pXYData) += nXYDataInc;

			/* Decode the type of each complex object */
			Type = (XYData)>>29;
	
	
			if ( Type == PACKED_TYPE_SHADOW || Type == PACKED_TYPE_LIGHTVOL )
			{
				/* Adds SHADOW and LIGHTVOL objects */
				AddRegionShadorLV( XYData, PlanesPerPoly, ISPAddr );
			}
			else if( Type == PACKED_TYPE_OPAQUETRANS )
			{
				AddRegionTransOpaque(XYData, PlanesPerPoly, ISPAddr );
				
				if(SmoothHighPlanes)
					AddRegionHighlightFog(XYData, PlanesPerPoly+1, 
									   ISPAddr, GOURAUDHIGHLIGHT);
				
				if(VFogPlanes)
					AddRegionHighlightFog(XYData, PlanesPerPoly+SmoothHighPlanes+1, 
									   ISPAddr, VERTEXFOG );
			}
			else
			{
				sgl_int32	TransSetId;
			
				if 	( ( Type & PACKED_TRANSTYPE_SETMARK ) == 0 )
				{
					/* Object is part of current FRONT/BACK object set */
				#if DOING_SGL_STYLE_SORTING
					TransSetId = CurrentTransSetId[Type & 1];
				#else
					TransSetId = CurrentTransSetId;
				#endif
				}
				else
				{
					/* Object is part of next FRONT/BACK object set */
				#if DOING_SGL_STYLE_SORTING
					TransSetId = ( CurrentTransSetId[Type & 1] += 2 );
				#else
					TransSetId = ( CurrentTransSetId += 2 );
				#endif
				}

				/* Add translucent object to scene */
				AddRegionSeeThru( XYData, TransSetId, PlanesPerPoly,
														ISPAddr, pZData );
				*((sgl_uint8 **) &pZData) += nZDataInc;

				if(SmoothHighPlanes)
					AddRegionHighlightFog(XYData, PlanesPerPoly+1, 
									   ISPAddr, TRANS_GOURAUDHIGHLIGHT);
				
				if(VFogPlanes)
					AddRegionHighlightFog(XYData, PlanesPerPoly+SmoothHighPlanes+1, 
									   ISPAddr, TRANS_VERTEXFOG );
			}
			
			/* Calculate ISPAddr of next object */
			ISPAddr = ISPAddr + (PlanesPerPoly+VFogPlanes+SmoothHighPlanes) * WORDS_PER_PLANE;
		} while ( --nPolys != 0 );
	}
}/* end of AddRegionObjectsExtra */

#endif /* PCX2 || PCX2_003 */

#if DAG_TRANS_SORTING
/**************************************************************************
 * Function Name  : AddRegionD3DTransTris
 * Inputs         : rpTri         - Triangles to be added
 *                : nXYDataInc    - Size of triangle structure
 *                : rpTriPlus     - Extra Tri information for sorting
 *                : PlanesPerPoly - Planes in each object
 *                : nPolys        - Number of objects
 *                : ISPAddr       - ISP address of 1st object description
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : The routine by which all objects are introduced into
 *                  this module for rendering.
 *				   
**************************************************************************/
void AddRegionD3DTransTris( PITRI *rpTri, int nXYDataInc,
						    PITRIPLUS *rpTriPlus, sgl_uint32 PlanesPerPoly,
						    int nPolys, sgl_uint32 ISPAddr )
{
	sgl_uint32 Mask = SigXYMask;
	PITRI	pTri = *rpTri;
	PITRIPLUS	pTriPlus = *rpTriPlus;
	sgl_uint32 *pXYData = (sgl_uint32*)&(pTri->reg);
	
	do
	{	/* Process each object */
		sgl_uint32 XYData;

		XYData = Mask & *pXYData;
		*((sgl_uint8 **) &pXYData) += nXYDataInc;

		if (guTransTriCounter == MAX_NUM_SCENE_TRIS)
		{
			DPF((DBG_ERROR,"Too many tris in the scene (%d)\n",guTransTriCounter));
			break;
		}
		else
		{
			/* Add translucent triangle to scene */
			AddRegionSeeThruD3D( &pTri, XYData, 
								 &pTriPlus, PlanesPerPoly, ISPAddr );
		}
	
		/* Calculate ISPAddr of next object */
#if PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)
		ASSERT(WORDS_PER_PLANE == 3);
		ISPAddr = ISPAddr + PlanesPerPoly + PlanesPerPoly + PlanesPerPoly;
#elif PCX2 || PCX2_003
		ASSERT(WORDS_PER_PLANE == 4);
		ISPAddr = ISPAddr + (PlanesPerPoly << 2) ;
#endif
		pTriPlus++;
		pTri++;
	} while ( --nPolys != 0 );
}

#if (PCX2 || PCX2_003) 

/**************************************************************************
 * Function Name  : AddRegionD3DTransTrisExtra
 * Inputs         : pXYData       - Type and XY data for each object
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Globals Used   : 
 * Description    : The routine by which all objects are introduced into
 *				   
**************************************************************************/
void AddRegionD3DTransTrisExtra( PITRI *rpTri, int nXYDataInc,
						    PITRIPLUS *rpTriPlus, sgl_uint32 PlanesPerPoly,
						    int nPolys, sgl_uint32 ISPAddr,
						    PFOGHIGHLIGHT pFogHighlight )
{
	sgl_uint32 Mask = SigXYMask;
	PITRI	pTri = *rpTri;
	PITRIPLUS	pTriPlus = *rpTriPlus;
	sgl_uint32 *pXYData = (sgl_uint32*)&(pTri->reg);
		
	do
	{	/* Process each object */
		sgl_uint32 XYData;
		sgl_uint32 VFogPlanes=0;
		sgl_uint32 SmoothHighPlanes=0;

		SmoothHighPlanes = (*pFogHighlight & GOURAUD_HIGHLIGHT)?1:0;	
		VFogPlanes = (*pFogHighlight & VERTEX_FOG)?1:0;	

		pFogHighlight++;
		XYData = Mask & *pXYData;
		*((sgl_uint8 **) &pXYData) += nXYDataInc;

		/* Add translucent object to scene */
		
		if (guTransTriCounter == MAX_NUM_SCENE_TRIS)
		{
			DPF((DBG_ERROR,"Too many tris in the scene (%d)\n",guTransTriCounter));
			break;
		}
		else
		{
			AddRegionSeeThruD3D( &pTri, XYData, 
								 &pTriPlus, PlanesPerPoly, ISPAddr );
		}
		
		if(SmoothHighPlanes)
			AddRegionHighlightFog(XYData, PlanesPerPoly+1, 
							   ISPAddr, TRANS_GOURAUDHIGHLIGHT);
		
		if(VFogPlanes)
			AddRegionHighlightFog(XYData, PlanesPerPoly+SmoothHighPlanes+1, 
							   ISPAddr, TRANS_VERTEXFOG );
		
		/* Calculate ISPAddr of next object */
		ISPAddr = ISPAddr + (PlanesPerPoly+VFogPlanes+SmoothHighPlanes) * WORDS_PER_PLANE;

		pTriPlus++;
		pTri++;
	} while ( --nPolys != 0 );

}/* end of AddRegionD3DTransTrisExtra */
#endif /* PCX2 || PCX2_003 */

#endif /* DAG_TRANS_SORTING */

#if DOING_SGL_STYLE_SORTING
/**************************************************************************
 * Function Name  : SortTransFaceLists
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : pRegion - REGION to process
 *                : pHead   - pRegion->pCurTSet[0]
 *                : pTail   - pRegion->pCurTSet[1]
 * Returns        : New pHead value, first TRANSFACE_LIST to process
 * Global Used    : None
 * Description    : Sorts the translucent face sets into increasing 
 *                  values of inverse depth.
 *
 *                  On entry pCurTSet[0 and 1] point at seperate singly
 *                  linked lists of TRANSFACE_LIST structures.
 *
 *                  On return pCurTSet[0] points at deepest set to process
 *                  and pCurTSet[1] points at shallowest set to process.
 *
 **************************************************************************/
static INLINE TRANSFACE_LIST *SortTransFaceLists( REGION_HEADER *pRegion,
							  TRANSFACE_LIST *pHead, TRANSFACE_LIST *pTail )
{
	TRANSFACE_LIST *pEntry, *pNext;
	float HeadZ, TailZ, Depth;

	if ( pHead != NULL )
	{
		/* Take first entry on first list as starting point */
		pTail = pHead;
	}
	else
	{
		/* Take first entry on second list as starting point */
		pHead = pTail;
	}

	/* Next entry after Head of list is first to process */
	pNext = pHead->pPre;

	/* Starting state please */
	HeadZ = TailZ = pHead->MaxInvZ;
	pHead->pPre = pHead->pPost = NULL;

	do
	{
		while ( ( pEntry = pNext ) != NULL )
		{
			/* Get next entry */
			pNext = pEntry->pPre;
			Depth = pEntry->MaxInvZ;

			if ( Depth <= HeadZ )
			{
				/* New entry is further away put at head of list */
				(pEntry->pPost = pHead)->pPre = pEntry;
				pEntry->pPre = NULL;
				pHead = pEntry;
				HeadZ = Depth;
			}
			else
			if ( Depth >= TailZ )
			{
				/* New entry is closer put at tail of list */
				pEntry->pPost = NULL;
				(pEntry->pPre = pTail)->pPost = pEntry;
				pTail = pEntry;
				TailZ = Depth;
			}
			else
			{
				TRANSFACE_LIST *p;
				
				/* Search for insertion point */
				for ( p = pHead->pPost; ( Depth > p->MaxInvZ ); p = p->pPost );

				/* Insert before pNext */
				pEntry->pPost = p;
				(pEntry->pPre = p->pPre)->pPost = pEntry;
				p->pPre = pEntry;
			}
		}

		if ( pRegion->pCurTSet[0] != NULL )
		{
			/* Done first list this time so do the next */
			pRegion->pCurTSet[0] = NULL;
			pNext = pRegion->pCurTSet[1];
		}
	}
	while ( pNext != NULL );

	/* Setup sorted list */
	pRegion->pCurTSet[0] = pHead;
	pRegion->pCurTSet[1] = pTail;

	return (pHead);
}

/**************************************************************************
 * Function Name  : NoSortTransFaceLists
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : pRegion - REGION to process
 *                : pHead   - pRegion->pCurTSet[0]
 *                : pTail   - pRegion->pCurTSet[1]
 * Returns        : New pHead value, first TRANSFACE_LIST to process
 * Global Used    : None
 * Description    : Process ( Not Sort) the translucent face sets.
 *
 *                  On entry pCurTSet[0] point at seperate singly
 *                  linked lists of TRANSFACE_LIST structures.
 *
 *                  On return pCurTSet[0] points at deepest set to process.
 *
 **************************************************************************/
static INLINE TRANSFACE_LIST *NoSortTransFaceLists( REGION_HEADER *pRegion,
							  TRANSFACE_LIST *pHead)
{
	TRANSFACE_LIST *pEntry, *pNext, *pThis;
   
   	/* Next entry after Head of list is first to process */
	pNext = pHead->pPre;
   
	/* Starting state please */
	pHead->pPre = pHead->pPost = NULL;

	if(gNoSortTransFaces == NO_SORT)
	{
		/* NO_SORT */
		while ( ( pEntry = pNext ) != NULL )
		{
			/* Get next entry */
			pNext = pEntry->pPre;

			/* New entry is further away put at head of list */
			(pEntry->pPost = pHead)->pPre = pEntry;
			pEntry->pPre = NULL;
			pHead = pEntry;
		}
	}
	else
	{
		/* REVERSED_NO_SORT */
		pThis = pHead;
		while ( ( pEntry = pNext ) != NULL )
		{
			/* Get next entry */
			pNext = pEntry->pPre;

			/* New entry is closer put at tail of list */
		   	pEntry->pPost = NULL;
			(pEntry->pPre = pThis)->pPost = pEntry;
			pThis = pEntry;

		}
	}

	/* Setup sorted list */
	pRegion->pCurTSet[0] = pHead;
   
	return (pHead);
}

#endif

#if VIGNETTE_FIX

/**************************************************************************
 * Function Name  : CountShortList (internal only  Assumed INLINE!)
 * Inputs         : pLast    - pointer to the last object inserted in the list
 * Input/Output   : 
 * Returns        : rPlanes  - accumulated count of planes described
 * Description    : Flattens the content of the list into pAddr.
 *                : Since block size is small do a whole block at a time.
 *                :
 *                : We count the number of planes in each structure. This is
 *				  : used to determine whether or not with put the translucent
 *				  : passes for a tile in or out of cache (ie 1024 plane
 *				  : boundary.
 *				  :
 *                : We also have to output the list in last block first
 *                : order to preserve the original order of the objects
 *                : supplied. As these blocks are so small we have to re-use
 *                : the pPrev pointers as the pNext pointers of the list in
 *                : the opposite order. The pPrev list needs to be restored
 *                : so each list can be output multiple times.
 **************************************************************************/

static sgl_uint32 CountShortList( const sgl_uint32 *pObjData)
{
#define PTR_MASK ( sizeof(sgl_uint32) * TRANSOBJS_PER_BLOCK )
	sgl_uint32 tmp0, tmp1, Planes = 0;
	TRANSOBJ_BLOCK *pNext = NULL, *pBlock;
	const sgl_uint32 *pPtr = pObjData;

	do
	{	
		/* Derive pBlock from pPtr which points at last item in block */
		pBlock = (TRANSOBJ_BLOCK *) (((sgl_uint32) pPtr) & (~PTR_MASK));

		/* Get pointer to previous block */
		pPtr = (const sgl_uint32 *) pBlock->pPrev;
		
		/* Reverse pPrev list order as we go */
		pBlock->pPrev = (sgl_uint32 *) pNext;
		
		/* Previous block needs to point at this block */
		pNext = pBlock;
	}
	while ( pPtr != NULL );

	while ( ( pNext = (TRANSOBJ_BLOCK *) pBlock->pPrev ) != NULL )
	{
		/* Restore pointer to previous block */
		pBlock->pPrev = (sgl_uint32 *) pPtr;
		
		/* pPrev points at last item stored in each block */
		pPtr = ((const sgl_uint32 *) pBlock->Objects) + TRANSOBJS_PER_BLOCK - 1;
		
		/* Extension blocks must be full, model 2 write buffers as tmp0/1 */
		tmp0 = pPtr[-2];
		tmp1 = pPtr[-1];
		Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		tmp0 = pPtr[0];		
		Planes += (tmp1 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;				
		
		/* Goto next block and restore original pPrev list as we go */
		pBlock = pNext;
		
		/* Goto next block and restore original pPrev list as we go */
		pBlock = pNext;
	}

	/* Restore pointer to previous block */
	pBlock->pPrev = (sgl_uint32 *) pPtr;
	
	/* Start at beginning of last block */
	pPtr = (const sgl_uint32 *) pBlock->Objects;


	/* Fill up the block, rather than a loop of unknown size, more optimal
	   code should be generated by the long switch statement.
	 */ 

	switch ( PTR_SET_SIZE( pObjData, TRANSOBJS_PER_BLOCK ) )
	{
		case 3:
		{
			tmp0 = *pPtr++;
			Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		}
		/* Drop Through */
		case 2:
		{
			/* Two objects in first block */
			tmp0 = *pPtr++;
			Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		}
		/* Drop Through */
		case 1:
		{
			/* One object in first block */
			tmp0 = *pPtr++;
			Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
			break;
		}		
#if _DEBUG
		/* No default should be needed */
		default:
		{
			DPF((DBG_ERROR,"Size of TRANSOBJS_PER_BLOCK changes to %d",TRANSOBJS_PER_BLOCK));
			break;
		}
#endif

	}

	return (Planes);
#undef PTR_MASK
}
#endif


/**************************************************************************
 * Function Name  : mymemcpy (internal only  Assumed INLINE!)
 * Inputs         : pDest    - pointer to place to copy data to
 *				  : pSrc    -  pointer to place to copy data from
 *                : Bytes - how many bytes to copy
 *
 * Returns        : sgl_uint32 * - address last used to store output
 * Description    : Copies object pointers to reserved memory 
 **************************************************************************/

static INLINE sgl_uint32 *mymemcpy( sgl_uint32 *pDest, const sgl_uint32 *pSrc, int Bytes )
{
	sgl_uint32 tmp0, tmp1;

	/* Pre-load first source item */
	tmp0 = *pSrc++;

	for(;;)
	{
		if ( ( Bytes -= ( sizeof(sgl_uint32) * 2 ) ) <= 0 )
		{
			/* Stop when we reach the last block */
			break;
		}

		/* Output a block */
		tmp1 = pSrc[0];
		IW( pDest, 0, tmp0);
		IW( pDest, 1, tmp1);
		pDest += 2;
		
		/* Pre-load next source item */
		tmp0 = pSrc[1];
		pSrc += 2;
	}
		
	if ( Bytes == 0 )
	{
		/* Last block has 2 items */
		tmp1 = pSrc[0];
		IW( pDest, 0, tmp0);
		IW( pDest, 1, tmp1);

		return (pDest+2);
	}
	else
	{
		/* Last has 1 item */
		IW( pDest, 0, tmp0);

		return (pDest+1);
	}
}


/**************************************************************************
 * Function Name  : OutputOpaqueList (internal only  Assumed INLINE!)
 * Inputs         : pLast    - pointer to the last object inserted in the list
 *				  : pAddr    - address last used to store output
 * Returns        : sgl_uint32 * - address last used to store output
 * Description    : Flattens the content of the list into pAddr, performs
 *                : post-increment on destination address.
 *                : Outputs the list of items in their original insertion
 *                : order by doing the deepest block first.
 *                : On PCX2 the blocks should already be in the parameter
 *                : space and it will only be necessary to setup the pointer.
 **************************************************************************/
static sgl_uint32 *OutputOpaqueList(sgl_uint32 *pObjData, sgl_uint32 *pAddr )
{
#define PTR_MASK ( sizeof(sgl_uint32) * (OBJECTS_PER_BLOCK|3) )
	sgl_uint32 *pNext, *pPtr = pObjData;
	OBJECT_BLOCK *pBlock;

	/* Terminate block list for later processing */
	pNext = NULL;

	do
	{
		/* Derive pBlock from pPtr which points at last item in block */
		pBlock = (OBJECT_BLOCK *) (((sgl_uint32) pPtr) & (~PTR_MASK));
		
		/* Set pNext of this block for processing later */
		pBlock->pNext = (OBJECT_BLOCK *) pNext;
		
		/* Previous block needs to point at this block */
		pNext = (sgl_uint32 *) pBlock;
	}
	while ( ( pPtr = pBlock->pPrev ) != NULL );

	while ( pBlock->pNext != NULL )
	{
		/* Extension blocks must be full */
		pAddr = mymemcpy( pAddr, (const sgl_uint32 *) pBlock->Objects, 
						  OBJECTS_PER_BLOCK*sizeof(sgl_uint32) );

		/* Move to next block */
		pBlock = pBlock->pNext;
	}
	
	/* First block may be partially full */
	pAddr = mymemcpy( pAddr, (const sgl_uint32 *) pBlock->Objects,
	                  ((unsigned int) pObjData) & PTR_MASK );

	return (pAddr);
#undef PTR_MASK
}

/**************************************************************************
 * Function Name  : OutputShortList (internal only  Assumed INLINE!)
 * Inputs         : pLast    - pointer to the last object inserted in the list
 *				  : pAddr    - address last used to store output
 * Input/Output   : rPlanes  - accumulated count of planes described
 * Returns        : sgl_uint32 * - address last used to store output
 * Description    : Flattens the content of the list into pAddr, performs
 *                : post-increment on destination address.
 *                : Since block size is small do a whole block at a time.
 *                :
 *                : We accumulate plane counts here as it stops us having
 *                : to keep lots of count fields on the list structures and
 *                : the processor isn't very busy when it's pushing all this
 *                : data into what has to be a non-cached buffer anyway.
 *                :
 *                : We also have to output the list in last block first
 *                : order to preserve the original order of the objects
 *                : supplied. As these blocks are so small we have to re-use
 *                : the pPrev pointers as the pNext pointers of the list in
 *                : the opposite order. The pPrev list needs to be restored
 *                : so each list can be output multiple times.
 **************************************************************************/

static sgl_uint32 *OutputShortList( const sgl_uint32 *pObjData, sgl_uint32 *pAddr,
														sgl_uint32 *rPlanes )
{
#define PTR_MASK ( sizeof(sgl_uint32) * TRANSOBJS_PER_BLOCK )
	sgl_uint32 Planes = *rPlanes, tmp0, tmp1;
	TRANSOBJ_BLOCK *pNext = NULL, *pBlock;
	const sgl_uint32 *pPtr = pObjData;

	do
	{	
		/* Derive pBlock from pPtr which points at last item in block */
		pBlock = (TRANSOBJ_BLOCK *) (((sgl_uint32) pPtr) & (~PTR_MASK));

		/* Get pointer to previous block */
		pPtr = (const sgl_uint32 *) pBlock->pPrev;
		
		/* Reverse pPrev list order as we go */
		pBlock->pPrev = (sgl_uint32 *) pNext;
		
		/* Previous block needs to point at this block */
		pNext = pBlock;
	}
	while ( pPtr != NULL );

	while ( ( pNext = (TRANSOBJ_BLOCK *) pBlock->pPrev ) != NULL )
	{
		/* Restore pointer to previous block */
		pBlock->pPrev = (sgl_uint32 *) pPtr;
		
		/* pPrev points at last item stored in each block */
		pPtr = ((const sgl_uint32 *) pBlock->Objects) + TRANSOBJS_PER_BLOCK - 1;
		
		/* Extension blocks must be full, model 2 write buffers as tmp0/1 */

		tmp0 = pPtr[-2];
		tmp1 = pPtr[-1];
		IW( pAddr, 0, tmp0);		
		Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		tmp0 = pPtr[0];		
		IW( pAddr, 1, tmp1);
		IW( pAddr, 2, tmp0);
		Planes += (tmp1 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		pAddr += 3;
		
		/* Goto next block and restore original pPrev list as we go */
		pBlock = pNext;
	}

	/* Restore pointer to previous block */
	pBlock->pPrev = (sgl_uint32 *) pPtr;
	
	/* Start at beginning of last block */
	pPtr = (const sgl_uint32 *) pBlock->Objects;

	/* Fill up the block */
  	switch ( PTR_SET_SIZE( pObjData, TRANSOBJS_PER_BLOCK ) )
	{
		case 3:
		{
			tmp0 = *pPtr++;
			IW( pAddr++, 0, tmp0);
			Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		}
		/* Drop Through */
		case 2:
		{
			/* Two objects in first block */
			tmp0 = *pPtr++;
			IW( pAddr++, 0, tmp0);
			Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
		}
		/* Drop Through */
		case 1:
		{
			/* One object in first block */
			tmp0 = *pPtr++;
			IW( pAddr++, 0, tmp0);
			Planes += (tmp0 >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
			break;
		}		
#if _DEBUG
		/* No default should be needed */
		default:
		{
			DPF((DBG_ERROR,"Size of TRANSOBJS_PER_BLOCK changes to %d",TRANSOBJS_PER_BLOCK));
			break;
		}
#endif
	}

	/* Update secondary result */
	*rPlanes = Planes;

	return (pAddr);
#undef PTR_MASK
}

/**************************************************************************
 * Function Name  : GenerateOpaquePtr (internal only  Assumed INLINE!)
 * Inputs         : pRegion - pointer to region array
 *				  : curAddr - current address
 *                : rPlanes - number of planes in region
 *				  : dPlanes - number of discarded planes in region
 *				  : Type    - Opaque, TransOpaque, GOURAUDHighlight or VertexFog
 *
 * Returns        : sgl_uint32 * - address last used to store output
 * Description    : Generate Opaque, TransOpaque, GOURAUDHighlight or VertexFog Ptr's 
 **************************************************************************/

static INLINE sgl_uint32 *GenerateOpaquePtr ( REGION_HEADER *pRegion, sgl_uint32 *curAddr,
							  sgl_uint32 *rPlanes, sgl_uint32 *dPlanes, int Type, int PlaneLimit) 
{
	sgl_uint32 RegionPlanes = *rPlanes;
	sgl_uint32 nDiscardedPlanes = *dPlanes;

	curAddr = OutputOpaqueList(pRegion->pLastSlots[Type], curAddr );

	if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
	{
		/* Copy all the light volumes */
		curAddr = OutputShortList(pRegion->pLastSlots[LIGHTVOL], curAddr, &RegionPlanes );
	}
				
	if ( pRegion->pLastSlots[SHADOW] != NULL )
	{
		/* Copy all the shadows */
		curAddr = OutputShortList(pRegion->pLastSlots[SHADOW], curAddr, &RegionPlanes );
	}
		
	/* Remove objects that put number of planes over the limit.*/
	nDiscardedPlanes += RegionPlanes;
	
	while (RegionPlanes > PlaneLimit)
	{					
		/* RegionPlanes-=( (*--curAddr) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK; */
		RegionPlanes -= ( IR( --curAddr, 0) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
	}
	nDiscardedPlanes -= RegionPlanes;

	*rPlanes = RegionPlanes;
	*dPlanes = nDiscardedPlanes;

	return (curAddr);
} /* end of GenerateOpaquePtr */

/**************************************************************************
** Function Name  : GenerateExtraPtr (internal only  Assumed INLINE!)
** Inputs         : pRegion - pointer to region array
**				  : curAddr - current address
**                : rPlanes - number of planes in region
**				  : dPlanes - number of discarded planes in region
**				  : Type    - GouraudHighlight VertexFog, 
**                            Trans_GouraudHighlight or Trans_VertexFog
**
** Returns        : sgl_uint32 * - address last used to store output
** Description    : Generate GouraudHighlight or VertexFog Ptr's 
***************************************************************************/

static INLINE sgl_uint32 *GenerateExtraPtr ( REGION_HEADER *pRegion, sgl_uint32 *curAddr,
							  sgl_uint32 *rPlanes, sgl_uint32 *dPlanes, int Type, int PlaneLimit) 
{
	sgl_uint32 RegionPlanes = *rPlanes;
	sgl_uint32 nDiscardedPlanes = *dPlanes;

	curAddr = OutputOpaqueList(pRegion->pExtraSlots[Type], curAddr );
		
	/* Remove objects that put number of planes over the limit.*/
	nDiscardedPlanes += RegionPlanes;
	
	while (RegionPlanes > PlaneLimit)
	{					
		/* RegionPlanes-=( (*--curAddr) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK; */
		RegionPlanes -= ( IR( --curAddr, 0) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
	}
	nDiscardedPlanes -= RegionPlanes;

	*rPlanes = RegionPlanes;
	*dPlanes = nDiscardedPlanes;

	return (curAddr);
} /* end of GenerateExtraPtr */

#if DAG_TRANS_SORTING
/**************************************************************************
** Function Name  : GenerateDAGTransPtr (internal only  Assumed INLINE!)
** Inputs         : pRegion - pointer to region array
**				  : curAddr - current address
**                : rPlanes - number of planes in region
**				  : dPlanes - number of discarded planes in region
**
** Returns        : sgl_uint32 * - address last used to store output
** Description    : Generate DAG translucent ptrs to hardware ISP space. 
***************************************************************************/

static INLINE sgl_uint32 *GenerateDAGTransPtr ( REGION_HEADER *pRegion, sgl_uint32 *curAddr,
							  sgl_uint32 *rPlanes, sgl_uint32 *dPlanes, int PlaneLimit) 
{
	sgl_uint32 RegionPlanes = *rPlanes;
	sgl_uint32 nDiscardedPlanes = *dPlanes;

	/* Output the graph in passes */
  	curAddr = Traverse(pRegion->usTransTriCounter, curAddr, &RegionPlanes);

	/* Remove objects that put number of planes over the limit.*/
	nDiscardedPlanes += RegionPlanes;
	
	while (RegionPlanes > PlaneLimit)
	{					
		/* RegionPlanes-=( (*--curAddr) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK; */
		RegionPlanes -= ( IR( --curAddr, 0) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
	}
	nDiscardedPlanes -= RegionPlanes;

	*rPlanes = RegionPlanes;
	*dPlanes = nDiscardedPlanes;

	return (curAddr);
} /* end of GenerateDAGTransPtr */
#endif

/**************************************************************************
 * Function Name  : GenerateTransPtr (internal only  Assumed INLINE!)
 * Inputs         : pRegion - pointer to region array
 *				  : curAddr - current address
 *                : rPlanes - number of planes in region
 *				  : dPlanes - number of discarded planes in region
 *				  : tPlanes - number of translucent planes in region
 *				  : pHead   - pointer of the head of trans list
 *
 * Returns        : sgl_uint32 * - address last used to store output
 * Description    : Generate Opaque, TransOpaque, GOURAUDHighlight or VertexFog Ptr's 
 **************************************************************************/

static INLINE sgl_uint32 *GenerateTransPtr ( REGION_HEADER *pRegion, sgl_uint32 *curAddr,
							  sgl_uint32 *rPlanes, sgl_uint32 *dPlanes, sgl_uint32 *tPlanes,
							  TRANSFACE_LIST *pHead) 
{
	sgl_uint32 RegionPlanes = *rPlanes;
	sgl_uint32 nDiscardedPlanes = *dPlanes;
	sgl_uint32 TransPassPlanes = *tPlanes;

	curAddr = OutputShortList( pHead->pLastSlot, curAddr, &TransPassPlanes);

   	if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
   	{
   		/* Copy all the light volumes */
   		curAddr = OutputShortList(pRegion->pLastSlots[LIGHTVOL],
   										curAddr, &TransPassPlanes);
   	}

   	if ( pRegion->pLastSlots[SHADOW] != NULL )
   	{
   		/* Copy all the shadows */
   		curAddr = OutputShortList(pRegion->pLastSlots[SHADOW],
   										curAddr, &TransPassPlanes);
   	}					  				

   	RegionPlanes += TransPassPlanes;
   	nDiscardedPlanes += RegionPlanes;

   	while (RegionPlanes > (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES) )
   	{												
   		/* RegionPlanes-=( (*--curAddr) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK; */
   		RegionPlanes -= ( IR( --curAddr, 0) >> OBJ_PCOUNT_SHIFT) & OBJ_PCOUNT_MASK;
   	}

   	nDiscardedPlanes -= RegionPlanes;
	*rPlanes = RegionPlanes;
	*dPlanes = nDiscardedPlanes;
	*tPlanes = TransPassPlanes;

	return (curAddr);
} /* end of GenerateTransPtr */

/**************************************************************************
 * Function Name  : OutputFlushingPlanes (internal only  Assumed INLINE!)
 * Inputs         : pRegion - pointer to region array
 *				  : curAddr - current address
 *                : rPlanes - number of planes in region
 *				  : dPlanes - number of discarded planes in region
 *				  : Type    - DummyFlushData of DummyTransFlushData
 *
 * Returns        : sgl_uint32 * - address last used to store output
 **************************************************************************/

static INLINE sgl_uint32 *OutputFlushingPlanes ( sgl_uint32 *curAddr, sgl_uint32 *rPlanes, 
									sgl_uint32 DataType)
{
	sgl_uint32 RegionPlanes = *rPlanes;

	IW(curAddr++ , 0, DataType);	
	RegionPlanes += FLUSH_PLANE;

	/* Fix for hardware bug - if the flush plane falls
	** on a 512/513 plane pos (i.e. forces the chip for 
	** double buffering the internal cache to single buffer
	** the flush plane can get lost
	** with the effect that this pass and the next pass
	** concatenate therby losing a translucent plane
	*/ 
	if(RegionPlanes==0x200 || RegionPlanes==0x201)
	{
		IW( curAddr++, 0, DataType);
		RegionPlanes+= FLUSH_PLANE;
		IW( curAddr++, 0, DataType);
		RegionPlanes+= FLUSH_PLANE;
	}

	*rPlanes = RegionPlanes;
	return (curAddr);
}/* end of OutputFlushingPlanes */

#if VIGNETTE_FIX

/**************************************************************************
 * Function Name  : CountTotalTranslucentPlanes (internal only  Assumed INLINE!)
 * Input		  : RegionPlanes - 	 number of planes in region
 * Inputs/Output  : pRegion - pointer to region array
 *				  : tPlanes - number of translucent planes in region  
 *				  : pHead   - pointer of the head of trans list
 *
 * Returns        : TRANSFACE_LIST * - pointer of the head of trans list
 * Description    : It simply counts all the planes in translucent pass for vignette fix.
 *					
 **************************************************************************/

static INLINE CountTotalTranslucentPlanes ( REGION_HEADER	*pRegion,
											sgl_uint32		RegionPlanes,
											sgl_uint32		*tPlanes,
#if DAG_TRANS_SORTING
											sgl_uint32		uSortedPlanes,
#endif
											TRANSFACE_LIST	*pHead) 
{
	sgl_uint32	TransPassPlanes = 0, i;

	/* This needs to be a sgl_int32 since the result could be negative.
	 */
	sgl_int32	nPassCountCat;

	/* Count the OpaqueTrans pass 
	 */
	if (pRegion->pLastSlots[OPAQUETRANS] != NULL)
	{
		/* Add the dummy begin translucent pass object for this pass  */
		TransPassPlanes += NUM_TRANS_PASS_START_PLANES;
		/* Count planes. OpaqueTrans planes 	*/
		TransPassPlanes += pRegion->TransOpaquePlanes;

		if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
		{
			/* Count planes. light volumes.	*/
			TransPassPlanes += CountShortList(pRegion->pLastSlots[LIGHTVOL]);
		}
				
		if ( pRegion->pLastSlots[SHADOW] != NULL )
		{
			/* Count all the shadows. */
			TransPassPlanes += CountShortList(pRegion->pLastSlots[SHADOW]);
		}
		/* Add the flushing plane for this pass !!!!!!!	*/
		TransPassPlanes+= FLUSH_PLANE;

		/* we'll add some flushing planes if this pass goes over
		** the internal cache halfway point
		*/ 
		if((RegionPlanes+TransPassPlanes)==0x200 || 
		   (RegionPlanes+TransPassPlanes)==0x201)
		{
			TransPassPlanes+= FLUSH_PLANE;
			TransPassPlanes+= FLUSH_PLANE;
		}
	}

	/* Count the Extra passes  */
	for(i=GOURAUDHIGHLIGHT; i<=TRANS_VERTEXFOG; i++)
	{
		if (pRegion->pExtraSlots[i] != NULL)
		{
			/* Add the dummy begin translucent pass object for this pass  */
			TransPassPlanes += NUM_TRANS_PASS_START_PLANES;
			/* Count extra planes */
			TransPassPlanes += pRegion->ExtraPlanes[i];
			/* Add the flushing plane for this pass !!!!!!!	*/
			TransPassPlanes+= FLUSH_PLANE;
			
			/* we'll add some flushing planes if this pass goes over
			** the internal cache halfway point
			*/ 
			if((RegionPlanes+TransPassPlanes)==0x200 || 
			   (RegionPlanes+TransPassPlanes)==0x201)
			{
				TransPassPlanes+= FLUSH_PLANE;
				TransPassPlanes+= FLUSH_PLANE;
			}
		}
	}

	/* Count the original translucent pass planes and sort them. 
	 */
	if (pHead)
	{
		TRANSFACE_LIST *pLocalHead = pHead;

		/* Calculate the number of translucent passes to concatenate.  */
		nPassCountCat = pRegion->nPassCount - nMaxPassCount + 1;
						
		/* Stuff in the (m - n + 1) passes together.  */
		if (nPassCountCat > 0)
		{
			/* Count begin translucent pass plane.	*/
			TransPassPlanes += NUM_TRANS_PASS_START_PLANES;

			while (	( nPassCountCat > 0 ) && ( pLocalHead != NULL ))
			{
				/* Decrement the count.	*/
				nPassCountCat--;
						
				/* Count planes. Translucent planes first.	*/
				TransPassPlanes += CountShortList( pLocalHead->pLastSlot);

				if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
				{
					/* Count all the light volume planes  */
					TransPassPlanes += CountShortList(pRegion->pLastSlots[LIGHTVOL]);
				}

				if ( pRegion->pLastSlots[SHADOW] != NULL )
				{
					/* Count all the shadows planes	*/
					TransPassPlanes += CountShortList(pRegion->pLastSlots[SHADOW]);
				}

				pLocalHead = pLocalHead->pPost;
			}

			/* Count flushing plane.  */
			TransPassPlanes += FLUSH_PLANE;
			if((RegionPlanes+TransPassPlanes)==0x200 || 
			   (RegionPlanes+TransPassPlanes)==0x201)
			{
				TransPassPlanes+= FLUSH_PLANE;
				TransPassPlanes+= FLUSH_PLANE;
			}
		}

		/* Check every pass. */
		while ( pLocalHead != NULL )
		{
			/* Count begin translucent pass plane. */
			TransPassPlanes += NUM_TRANS_PASS_START_PLANES;

			/* Count planes. Translucent planes first. */
			TransPassPlanes += CountShortList( pLocalHead->pLastSlot);

			if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
			{
				/* Count all the light volume planes  */
				TransPassPlanes += CountShortList(pRegion->pLastSlots[LIGHTVOL]);
			}
			if ( pRegion->pLastSlots[SHADOW] != NULL )
			{
				/* Count all the shadows planes	 */
				TransPassPlanes += CountShortList(pRegion->pLastSlots[SHADOW]);
			}

			/* Count flushing plane. */
			TransPassPlanes += FLUSH_PLANE;
			if((RegionPlanes+TransPassPlanes)==0x200 || 
			   (RegionPlanes+TransPassPlanes)==0x201)
			{
				TransPassPlanes+= FLUSH_PLANE;
				TransPassPlanes+= FLUSH_PLANE;
			}
			pLocalHead = pLocalHead->pPost;
		}
				
#if PCX1
		/* Extra translucent pass for PCX1. */
		TransPassPlanes += NUM_TRANS_PASS_START_PLANES +  FLUSH_PLANE +
						   NUM_DUMMY_OBJECT_PLANES;
#endif

	}

#if DAG_TRANS_SORTING
	/* Add the number of planes belonged to the sorted triangles..
	 */
	TransPassPlanes += uSortedPlanes;
#endif /*DAG_TRANS_SORTING*/

	*tPlanes = TransPassPlanes;

}/* end of CountTotalTranslucentPlanes */
#endif

#if _DEBUG
static int DumpedOne = 1;
#endif

#define FRAME_STATS 0
#define REGION_STATS 0

int number = 0;

/*****************************************************************************
 * Function Name  : GenerateObjectPtrLite
 * Inputs         : bRenderAllRegions - if nonzero then all regions are
 *					 rendered, irrespective of whether they are empty.
 * Returns        : Number of regions rendered (nNumRegionsRendered).
 * Global Used    : RegionData, and ISP parameter buffer in rnglobals
 * Description    : 
 *                  Generates object reference part of sabre parameter data
 *					using the pRegionStrips object list structure.
 *
 *				    This is the version for SGL Lite, which does not render
 *					empty regions (for speeding up Direct3D benchmarks).
 *****************************************************************************/
int GenerateObjectPtrLite( const REGIONS_RECT_STRUCT *const pRegionsRect,
						   sgl_bool  bRenderAllRegions)
{
	REGION_STRIP *pStrip, *pLastStrip;
#if !DUMP_PARAMS
	/*
	// If we are dumping parameter files then this becomes a global variable to
	// be read by DumpSabreAndTexas in rnrender.c
	*/
	sgl_uint32 *curAddr;
#endif
		/* Convert LastYRegion to actual lines on the screen */
		int YRegLines = RegionInfo.YSize >> Y_SHIFT;
		int LastYLine = (pRegionsRect->LastYRegion+1) * YRegLines;
		
	int nNumRegionsRendered = 0;
#if _DEBUG
	int FoundOverload = DumpedOne;
#endif

	int FrameTotalPlanes=0, FrameTransPasses=0, FrameTransPlanes=0, FrameViFixes=0;
	static FILE *dump = NULL;

#if REGION_STATS
	if(dump==NULL)
	{
		dump = fopen("region.out","w");
	}
#endif
	/* Get pointer to where we are building this info */
	curAddr = PVRParamBuffs[PVR_PARAM_TYPE_REGION].pBuffer + 
		PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos;
	
	/* Check the region range */
	ASSERT(pRegionsRect->LastXRegion < MAX_X_REGIONS);
	ASSERT(pRegionsRect->LastYRegion < MAX_Y_REGIONS);
	ASSERT(pRegionsRect->FirstXRegion >= 0);
	ASSERT(pRegionsRect->FirstYRegion >= 0);
	ASSERT(pRegionsRect->LastXRegion >= pRegionsRect->FirstXRegion);
	ASSERT(pRegionsRect->LastYRegion >= pRegionsRect->FirstYRegion);

	{
		if ( LastYLine > OutputHeight )
		{
			/* Limit to precise screen size */
			LastYLine = OutputHeight;
		}

		/* Start on the first strip, externally nominal region height applies */
		pStrip     = pRegionStrips[pRegionsRect->FirstYRegion * YRegLines];
		pLastStrip = pRegionStrips[LastYLine-1];
	}
	
	
	/* Reset render counter */
	nNumRegionsRendered = 0;


#if REGION_STATS
	if(dump!=NULL)
	{
		fprintf(dump,"VF   AllPlanes   TransPlanes   TransPasses\n");
	}
#endif

	for ( ;; pStrip = pStrip->pNext )
	{
		REGION_HEADER *pRegion;
		sgl_uint32 RegionWord;
		sgl_uint32 uBusiestTile = 0;
		sgl_uint32 uSadness = 0;
		int Width = pStrip->Width, Regions;
		sgl_int32	nPassCountCat;

		/* Start one after the last region of size XSize<<Width */
		Regions = (pRegionsRect->LastXRegion >> Width) + 1;

		/* Calculate starting address in region array */
		pRegion = pStrip->Regions +	Regions;

		/* Calculate region word corresponding to our starting point */
		RegionWord = pStrip->RegionWordY + AddRegionWordXData(Regions, Width);

		/* Number of regions to be processed */
		Regions = Regions - (pRegionsRect->FirstXRegion >> Width);

		do
		{
			TRANSFACE_LIST *pHead;
			#if DOING_SGL_STYLE_SORTING
				TRANSFACE_LIST *pTail;
			#endif
			sgl_uint32 RegionPlanes;
			sgl_uint32 nDiscardedPlanes = 0;
			sgl_uint32 TransPassPlanes = 0;
			int RegionTotalPlanes=0, RegionTransPasses=0, RegionTransPlanes=0, ViFix=0;
#if DAG_TRANS_SORTING
			sgl_uint32 uSortedPlanes = 0;
#endif
			/* Previous region please, get Opaque counter */
			RegionPlanes = (--pRegion)->OpaquePlanes;
						
			/* Pre-adjust running values for every tile scanned */
			RegionWord -= DeltaRegionWordX( Width );

			/* Ignore the background OPAQUE object which is always there */
			if ( ( RegionPlanes < 2 )             &&
		#if DAG_TRANS_SORTING
				 ( pRegion->uTransTriList == NULL ) &&
		#endif /*DAG_TRANS_SORTING*/
				 (!bRenderAllRegions)             &&
				 ( pRegion->pLastSlots[OPAQUETRANS] == NULL ) &&
#if DOING_SGL_STYLE_SORTING
				 ( pRegion->pCurTSet[0] == NULL ) &&
				 ( pRegion->pCurTSet[1] == NULL )    )
#else
				 ( pRegion->pCurTSet == NULL ) )
#endif
			{
				/* Next region please, increase size of strip if possible */
				continue;
			}
						
			/* Need to render this region */
			nNumRegionsRendered++;

				/* Start first pass */
			IW( curAddr++, 0, RegionWord);

			if ( pRegion->pLastSlots[OPAQUE] != NULL )
			{
				/* Add opaque objects */
				curAddr = GenerateOpaquePtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
											  OPAQUE, (REGION_PLANE_LIM - SAFETY_MARGIN_OPAQ));
			}

			/* Each pass must have at least REGION_PLANE_MIN planes for 
			   the sabre chip to work so pad the region  up to meet this
			   condition.												*/

			while(RegionPlanes < REGION_PLANE_MIN)
			{				
				/* Add dummy objects until we meet our min criterion */
  				IW(curAddr++, 0, DummyObjData);
  				RegionPlanes += NUM_DUMMY_OBJECT_PLANES;
				/*	DPF((DBG_VERBOSE, "---- Padding Region with extra planes to make > 33 ----"));*/
			}


		  	/* Add the flushing plane for the opaque pass !!!!!!! */
			curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyFlushData);


			/* Translucent object handling	*/
#if DOING_SGL_STYLE_SORTING
			pHead = pRegion->pCurTSet[0];
			pTail = pRegion->pCurTSet[1];
#else
			pHead = pRegion->pCurTSet;
#endif

#if DAG_TRANS_SORTING
			/* We need to fiddle with the new sorting algorithm.
			 */
			if ( pRegion->uTransTriList != NULL )
			{	/* Create the graph
				 */
				DoTheStuff( (sgl_uint32) pRegion->usTransTriCounter, 
							&pRegion->uTransTriList );

				/* Calculate the number of planes need for the new sorting algorothm.
				 * This is required regardless if vignetting fix is being used or
				 * not. Needed to cycle through the graph.
				 */
				uSortedPlanes = FixForVignetting(pRegion->usTransTriCounter);
			}

#endif /*DAG_TRANS_SORTING*/		 
			
#if DOING_SGL_STYLE_SORTING
			if (( pHead != pTail ) && (RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Need to sort them. 
				 */
				if(gNoSortTransFaces == NO_SORT || gNoSortTransFaces == REVERSED_NO_SORT)
				{			 
				 	pHead = NoSortTransFaceLists( pRegion, pHead);
				}
				else
				{
					pHead = SortTransFaceLists( pRegion, pHead, pTail );
				}
			}
			else
			{
				/* Set it to NULL if we aren't happy. ie no translucent triangles.
				 */
				pHead = NULL;
			}
#endif

#if VIGNETTE_FIX
			/* Only do vignetting fix if plane count is less than cache boundary.
			 */
			if (RegionPlanes < IN_PLANE_CACHE_BOUNDARY)
			{
				/* This is the vignetting fix. It simply counts all the planes in every pass.
				 * If the total pass count puts the plane count for a tile over 1024 then
				 * the first pass is started out of cache.
				 */
				CountTotalTranslucentPlanes (pRegion, RegionPlanes, &TransPassPlanes,
	#if DAG_TRANS_SORTING
											 uSortedPlanes,
	#endif
											 pHead);
				
				/* Pad out the opaque pass over the cache boundary if the sum of
				 * the translucent planes plus the current tile plane count exceeds
				 * the cache boundary.
				 *
				 * This fix prevents vignetting since this is due to the translucent
				 * pass crossing the cache boundary.
				 */
				if ((RegionPlanes + TransPassPlanes) > IN_PLANE_CACHE_BOUNDARY)
				{
					/* Overwrite old flushing plane
					 */
					curAddr--;
							
					while(RegionPlanes < IN_PLANE_CACHE_BOUNDARY)
					{				
						/* Add dummy objects until we meet our min criterion
						 */
						IW( curAddr++, 0, DummyObjDataLarge);
						RegionPlanes += NUM_DUMMY_OBJECT_PLANES_LARGE;
					}
							
					/* Replace flushing plane
					 */
					IW( curAddr++, 0, DummyFlushData);
					ViFix = 1;
				}
			}
#endif		/* #if VIGNETTE_FIX	*/

			/* Jim's new D3D translucent solution. Only inserted if plane count below
			 * the safety margin.
			 */
			if ((pRegion->pLastSlots[OPAQUETRANS] != NULL ) &&
				(RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Add the dummy begin translucent pass object for this pass.
				 */
				IW(curAddr++ , 0, DummyTransData);
				RegionPlanes += 
					NUM_TRANS_PASS_START_PLANES + pRegion->TransOpaquePlanes;

				/* Add OpaqueTrans objects */
				curAddr = GenerateOpaquePtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
										 OPAQUETRANS, (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));
			   
				/* Add the flushing plane for this pass !!!!!!!	*/
				curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 
			}

			/* GOURAUD Highlight objects pass */
			if ((pRegion->pExtraSlots[GOURAUDHIGHLIGHT] != NULL ) &&
				(RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Add the dummy begin translucent pass object for this pass.*/
				IW(curAddr++ , 0, DummyTransData);
				RegionPlanes += 
					NUM_TRANS_PASS_START_PLANES + pRegion->ExtraPlanes[GOURAUDHIGHLIGHT];

				/* Add GOURAUD Highlight objects */
				curAddr = GenerateExtraPtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
										 GOURAUDHIGHLIGHT, (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));
			   
				/* Add the flushing plane for this pass !!!!!!!	*/
				curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 
			}
		   
		    /* Vertex Fog objects pass */
			if ((pRegion->pExtraSlots[VERTEXFOG] != NULL ) &&
				(RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Add the dummy begin translucent pass object for this pass. */
				IW(curAddr++ , 0, DummyTransData);
				RegionPlanes += 
					NUM_TRANS_PASS_START_PLANES + pRegion->ExtraPlanes[VERTEXFOG];

				/* Add GOURAUD Highlight objects */
				curAddr = GenerateExtraPtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
										 VERTEXFOG, (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));
			   
				/* Add the flushing plane for this pass !!!!!!!	*/
				curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 
			}

			/* Initialise count. */
			TransPassPlanes = 0;


#if DAG_TRANS_SORTING
			if ( pRegion->uTransTriList != NULL )
			{
				/* Output the graph in passes */
				curAddr = GenerateDAGTransPtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
										 (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));
			   
			}

#endif /*DAG_TRANS_SORTING*/

#if VIGNETTE_FIX
			/* Normal translucent pass handling. The are translucent passes if pHead != NULL.
			 * The translucent passes have already been sorted if vignetting fix used. */

			if ( pHead && (RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* No need to sort them as already sorted.*/
#else
			/* Count the original translucent pass planes and sort them.
			 */
			if ( pHead && (RegionPlanes < SAFETY_MARGIN_TRANS))
			{
#endif
				RegionTransPlanes = RegionPlanes;

				/* Calculate the number of translucent passes to concatenate.*/
				nPassCountCat = pRegion->nPassCount - nMaxPassCount + 1;
						
				/* Stuff in the (m - n + 1) passes together. */
				if (nPassCountCat > 0)
				{
					RegionTransPasses++;

					/* Add the dummy begin translucent pass object for this pass  */
					IW( curAddr++, 0, DummyTransData);
					RegionPlanes += NUM_TRANS_PASS_START_PLANES;

					while ( (nPassCountCat > 0) &&
							(RegionPlanes < SAFETY_MARGIN_TRANS) && 
							( pHead != NULL ) )
					{
						/* Decrement the count.	*/
						nPassCountCat--;

						/* Zero the plane count for upcoming pass */
						TransPassPlanes = 0;
						
						/* Add all the translucent objects */
						curAddr = GenerateTransPtr ( pRegion, curAddr, &RegionPlanes,
									   &nDiscardedPlanes, &TransPassPlanes, pHead); 
	 	
						pHead = pHead->pPost;

					}

					/* Add the flushing plane for this pass !!!!!!!	 */
					curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 	 				
				}

				/* Stuff in the rest of the passes.	  */
				while ( ( pHead != NULL ) && (RegionPlanes < SAFETY_MARGIN_TRANS))
				{
					RegionTransPasses++;

					/* Add the dummy begin translucent pass object for this pass */
					IW( curAddr++, 0, DummyTransData);
					RegionPlanes += NUM_TRANS_PASS_START_PLANES;

					/* Zero the plane count for upcoming pass */
					TransPassPlanes = 0;
					
					/* Add all the translucent objects */
					curAddr = GenerateTransPtr ( pRegion, curAddr, &RegionPlanes,
									   &nDiscardedPlanes, &TransPassPlanes, pHead); 

				   	/* Add the flushing plane for this pass !!!!!!!	*/
					curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 
					
					pHead = pHead->pPost;
				}

				#if PCX1
					/* Size of the above loop is always known, unroll it? */
					IW( curAddr, 0, (DummyTransData + (1 << OBJ_PCOUNT_SHIFT)));
					IW( curAddr, 1, DummyTransObjData);
					IW( curAddr, 2, DummyTransFlushData);
					curAddr+=3;

					RegionPlanes+= NUM_TRANS_PASS_START_PLANES +
								   FLUSH_PLANE +
								   NUM_DUMMY_OBJECT_PLANES;
				#endif
				RegionTransPlanes = RegionPlanes - RegionTransPlanes;
			}

			/* Gouraud Highlight for Translucent objects pass */
			if ((pRegion->pExtraSlots[TRANS_GOURAUDHIGHLIGHT] != NULL ) &&
				(RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Add the dummy begin translucent pass object for this pass.*/
				IW(curAddr++ , 0, DummyTransData);
				RegionPlanes += 
					NUM_TRANS_PASS_START_PLANES + pRegion->ExtraPlanes[TRANS_GOURAUDHIGHLIGHT];

				/* Add GOURAUD Highlight objects */
				curAddr = GenerateExtraPtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
										 TRANS_GOURAUDHIGHLIGHT, (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));
			   
				/* Add the flushing plane for this pass !!!!!!!	*/
				curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 
			}
		   
		    /* Vertex Fog  for Translucent objects pass */
			if ((pRegion->pExtraSlots[TRANS_VERTEXFOG] != NULL ) &&
				(RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Add the dummy begin translucent pass object for this pass. */
				IW(curAddr++ , 0, DummyTransData);
				RegionPlanes += 
					NUM_TRANS_PASS_START_PLANES + pRegion->ExtraPlanes[TRANS_VERTEXFOG];

				/* Add GOURAUD Highlight objects */
				curAddr = GenerateExtraPtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
										 TRANS_VERTEXFOG, (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));
			   
				/* Add the flushing plane for this pass !!!!!!!	*/
				curAddr = OutputFlushingPlanes ( curAddr, &RegionPlanes, DummyTransFlushData); 
			}

			FrameTotalPlanes+=RegionPlanes;
			FrameTransPlanes+=RegionTransPlanes;
			FrameTransPasses+=RegionTransPasses;
			FrameViFixes+=ViFix;
			/* Update the plane information this strip */
#if REGION_STATS
	if(dump!=NULL)
	{
		fprintf(dump,"%-5.1d%-12.d%-14.1d%-d\n", ViFix, RegionPlanes, RegionTransPlanes, RegionTransPasses);	
	}
#endif
								
	RegionPlanes += nDiscardedPlanes;			

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			if ( RegionPlanes > ((REGION_PLANE_LIM * 3)/4) ) /* 75% of maximum as threshold */
			{
				/* DECIDE WHAT TO DO */
				if ( (pRegion->HeightStats[BUSY] > ((pRegion->HeightStats[SAD])<<1)) && (RegionPlanes > uBusiestTile) )
				{					
					/* For splitting the tile - note the busiest tile*/
						uBusiestTile = RegionPlanes;
				}
			}
			else if ((SAFETY_MARGIN_OPAQ << 1) > RegionPlanes) 				
			{
				if ( (pRegion->HeightStats[SAD] > ((pRegion->HeightStats[HAPPY])<<1)) && (RegionPlanes > uSadness) )
				{
					/* Attempt to merge */
						uSadness += RegionPlanes;
				}
			}
#endif

		} while ( --Regions != 0 );

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
		/* Record info on whether to split or merge */
		pStrip->PlaneTally = uBusiestTile - uSadness;
#endif

		/* Last strip done */
		if ( pStrip == pLastStrip ) break;
	}

	/* With PCX1 and PCX2 the very last pointer must be marked with an end bit. */
	IORW( curAddr, -1, OBJ_VERY_LAST_PTR);

	/* Update the Sabre index so that we know how much data has been written */
#if _DEBUG
	if ( FoundOverload )
	{
#endif

		/*
		// Update the Sabre index so that we know how much data has been written
		*/

		PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos += ((sgl_uint32)(curAddr - &PVRParamBuffs[PVR_PARAM_TYPE_REGION].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos]));
#if _DEBUG
	}
	else
	if ( nNumRegionsRendered != 0 )
	{
		sgl_uint32 Words = ((sgl_uint32)(curAddr - &PVRParamBuffs[PVR_PARAM_TYPE_REGION].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos]));
		int tileObjCtr = 0, tileCtr = 0, Left;

		DumpedOne = 1;

		DPF((DBG_VERBOSE,"Generate output at 0x%x words %d",&PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos, Words));

/*		if ( Words > 256 )
		{
			Left = Words - 256;
			Words = 256;
		}
*/
		Left = 0;

		while (Words-- != 0)
		{
			sgl_uint32 Word = PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos++;
			
			switch((Word >> 30) & 0xf)
			{
				case 0:
					if ((Word>>29) & 0x1) /* linked list pointer */
					{
						DPF((DBG_VERBOSE," :     ERROR - Bit 29 set %X",Word));
						Word &= ~0x20000000;
					}

					DPF((DBG_VERBOSE," :     OBJECT Ptr - %d Plane(s) at %X", ((Word >> 19) & 0x3ff), (Word & 0x7ffff) * 4));
					tileObjCtr++;
				break;
				case 1:
					if (tileCtr != 0)
					{
						DPF((DBG_VERBOSE,"         %3d\n", tileObjCtr));
						tileObjCtr =0;
					}
					DPF((DBG_VERBOSE," : TILE at (%d, %d), %d by %d", (((Word >> 15) & 0x1f) * 32), ((Word >> 20) & 0x3ff), (((Word & 0x1f)+ 1) * 32), ((Word >> 5) & 0x3ff)+1));
					DPF((DBG_VERBOSE," %3d    (%3d,%3d)   %2d by %2d ",tileCtr++, (((Word >> 15) & 0x1f) * 32), ((Word >> 20) & 0x3ff), (((Word & 0x1f)+ 1) * 32), ((Word >> 5) & 0x3ff)+1));
				break;
				case 2:
					DPF((DBG_VERBOSE," :   LAST OBJ Ptr - %d Plane(s) at %X", ((Word >> 19) & 0x3ff), (Word & 0x7ffff) * 4));
					DPF((DBG_VERBOSE,"         %3d            0\n", ++tileObjCtr));
					tileObjCtr =0;
				break;	
				default:
				    DPF((DBG_VERBOSE," : Unexpected word %X", Word));
			}
		}

		PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos += Left;
	}
#endif

#if FRAME_STATS
	if(dump!=NULL)
	{
		fprintf(dump,"FRAME : Rendered %d Regions\n",nNumRegionsRendered);
		fprintf(dump,"FRAME : VF   AllPlanes   TransPlanes   TransPasses\n");
		fprintf(dump,"FRAME : %-5.1d%-12.1d%-14.1d%-d\n", FrameViFixes, FrameTotalPlanes, FrameTransPlanes, FrameTransPasses);	
	}
#elif DEBUG
	DPF((DBG_VERBOSE, "FRAME : Rendered %d Regions",nNumRegionsRendered));
	DPF((DBG_VERBOSE, "FRAME : VF   AllPlanes   TransPlanes   TransPasses"));
	DPF((DBG_VERBOSE, "FRAME : %-5.1d%-12.1d%-14.1d%-d", FrameViFixes, FrameTotalPlanes, FrameTransPlanes, FrameTransPasses));
#endif

	return ( nNumRegionsRendered );
}


/*****************************************************************************
 * Function Name  : GenerateObjectPtrLiteStrip
 * Inputs         : bRenderAllRegions - if nonzero then all regions are
 *					 rendered, irrespective of whether they are empty.
 * Returns        : void
 * Global Used    : RegionData, and ISP parameter buffer in rnglobals
 * Description    : 
 *                  Generates object reference part of sabre parameter data
 *					using the pRegionStrips object list structure.
 *
 *				    This is the version for SGL Lite, which does not render
 *					empty regions (for speeding up Direct3D benchmarks).
 *
 *					Used for 2d/3d compositing/strip rendering
 *****************************************************************************/
void GenerateObjectPtrLiteStrip( const REGIONS_RECT_STRUCT *const pRegionsRect,
								 sgl_bool bRenderAllRegions)
{
	REGION_STRIP 		*pStrip, *pLastStrip;
#if !DUMP_PARAMS
	/*
	// If we are dumping parameter files then this becomes a global variable to
	// be read by DumpSabreAndTexas in rnrender.c
	*/
	sgl_uint32 				*curAddr;
#endif
	REGION_STRIP_EXTRA 	*pRegionStripExtra = RegionStripExtra;
	sgl_uint32				RegionDataStart;
	
	/* Get pointer to where we are building this info */
	curAddr = PVRParamBuffs[PVR_PARAM_TYPE_REGION].pBuffer + 
		PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos;

	RegionDataStart = (sgl_uint32) curAddr;

	/* Check the region range */
	ASSERT(pRegionsRect->LastXRegion < MAX_X_REGIONS);
	ASSERT(pRegionsRect->LastYRegion < MAX_Y_REGIONS);
	ASSERT(pRegionsRect->FirstXRegion >= 0);
	ASSERT(pRegionsRect->FirstYRegion >= 0);
	ASSERT(pRegionsRect->LastXRegion >= pRegionsRect->FirstXRegion);
	ASSERT(pRegionsRect->LastYRegion >= pRegionsRect->FirstYRegion);
	{
		/* Convert LastYRegion to actual lines on the screen */
		int YRegLines = RegionInfo.YSize >> Y_SHIFT;
		int LastYLine = (pRegionsRect->LastYRegion+1) * YRegLines;
		
		if ( LastYLine > OutputHeight )
		{
			/* Limit to precise screen size */
			LastYLine = OutputHeight;
		}

		/* Start on the first strip, externally nominal region height applies */
		pStrip     = pRegionStrips[pRegionsRect->FirstYRegion * YRegLines];
		pLastStrip = pRegionStrips[LastYLine-1];
	}

	for ( ;; pStrip = pStrip->pNext, pRegionStripExtra++ )
	{
		REGION_HEADER *pRegion;
		sgl_uint32 uBusiestTile = 0;
		sgl_uint32 uSadness = 0;
		sgl_uint32 RegionWord;
		int Width = pStrip->Width, Regions;
		int nLeftX;

		/* Start one after the last region of size XSize<<Width */
		Regions = (pRegionsRect->LastXRegion >> Width) + 1;
		nLeftX = Regions;

		/* Calculate starting address in region array */
		pRegion = pStrip->Regions +	Regions;
				
		/* Calculate region word corresponding to our starting point */
		RegionWord = pStrip->RegionWordY + AddRegionWordXData(Regions, Width);

		/* Number of regions to be processed */
		Regions = Regions - (pRegionsRect->FirstXRegion >> Width);
	
		/* save pointer to extra data */
		pStrip->pExtra = pRegionStripExtra;

		/* set strip data to 'no regions in this strip' values */
		pRegionStripExtra->StartOfFirstRegion = (sgl_uint32*)(((sgl_uint32) curAddr) - RegionDataStart);	/* JWF added cast to pointer */
		pRegionStripExtra->EndOfLastRegion = curAddr;
		pRegionStripExtra->nRegionsInStrip = 0;
		pRegionStripExtra->nRightX = nLeftX;
		pRegionStripExtra->nLeftX = nLeftX - Regions;
		
		do
		{
			TRANSFACE_LIST *pHead;
			#if DOING_SGL_STYLE_SORTING
				TRANSFACE_LIST *pTail;
			#endif
			sgl_uint32 RegionPlanes;
			sgl_uint32 nDiscardedPlanes = 0;
			sgl_uint32 TransPassPlanes = 0;
			sgl_uint32 RegionIsEmpty = FALSE;

			/* Previous region please, get Opaque counter */
			RegionPlanes = (--pRegion)->OpaquePlanes;
						
			/* Pre-adjust running values for every tile scanned */
			RegionWord -= DeltaRegionWordX( Width );
			
			/* Ignore the background OPAQUE object which is always there */
			if ( (!bRenderAllRegions)             &&
				 ( RegionPlanes < 2 )             &&
				 ( pRegion->pLastSlots[OPAQUETRANS] == NULL ) &&
#if DOING_SGL_STYLE_SORTING
				 ( pRegion->pCurTSet[0] == NULL ) &&
				 ( pRegion->pCurTSet[1] == NULL ) )
#else
				 ( pRegion->pCurTSet == NULL ))
#endif	
			{
				/* finished boring region case, so go straight to next one */
				RegionIsEmpty = TRUE;
			}
			else 
			{	
				if (!pRegionStripExtra->nRegionsInStrip)
				{
					/* first non-empty region in this strip */
					pRegionStripExtra->nRightX = nLeftX;
					pRegionStripExtra->StartOfFirstRegion = (sgl_uint32*)(((sgl_uint32) curAddr) - RegionDataStart);	/* JWF added cast to pointer */
				}
				
				/* Need to render this region */
				pRegionStripExtra->nRegionsInStrip ++;
			}

			/* Start first pass */
			IW( curAddr++, 0, RegionWord);

			if ( pRegion->pLastSlots[OPAQUE] != NULL )
			{
				/* Add opaque objects */
				curAddr = GenerateOpaquePtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
											  OPAQUE, (REGION_PLANE_LIM - SAFETY_MARGIN_OPAQ));	
				
			}

			/* Each pass must have at least REGION_PLANE_MIN planes for 
			** the sabre chip to work so pad the region  up to meet this
			** condition.
			*/
			while(RegionPlanes < REGION_PLANE_MIN)
			{				
				/* Add dummy objects until we meet our min criterion */
				IW( curAddr++, 0, DummyObjData);
				RegionPlanes += NUM_DUMMY_OBJECT_PLANES;
			}

		  	/* Add the flushing plane for the opaque pass !!!!!!! */
			IW( curAddr++, 0, DummyFlushData);
			RegionPlanes += FLUSH_PLANE;

			/* Translucent object handling
			 */
#if DOING_SGL_STYLE_SORTING
			pHead = pRegion->pCurTSet[0];
			pTail = pRegion->pCurTSet[1];
#else
			pHead = pRegion->pCurTSet;
#endif

#if VIGNETTE_FIX
			/* This is the vignetting fix. It simply counts all the planes in every pass.
			 * If the total pass count puts the plane count for a tile over 1024 then
			 * the first pass is started out of cache.
			 */

			/* Initialise count.
			 */
			TransPassPlanes = 0;
			/* Count the new translucent pass format.
			 */
			if ((pRegion->pLastSlots[OPAQUETRANS] != NULL) &&
				(RegionPlanes < IN_PLANE_CACHE_BOUNDARY))
			{
				/* Add the dummy begin translucent pass object for this pass
				 */
				TransPassPlanes += NUM_TRANS_PASS_START_PLANES;

				/* Count planes. Translucent planes first.
				 */
				TransPassPlanes += pRegion->TransOpaquePlanes;

				if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
				{
					/* Count planes. light volumes.
					 */
					TransPassPlanes += CountShortList(pRegion->pLastSlots[LIGHTVOL]);
				}
				
				if ( pRegion->pLastSlots[SHADOW] != NULL )
				{
					/* Count all the shadows.
					 */
					TransPassPlanes += CountShortList(pRegion->pLastSlots[SHADOW]);
				}
		
				/* Add the flushing plane for this pass !!!!!!!
				 */
				TransPassPlanes+= FLUSH_PLANE;
			}
			/* Count the original translucent pass planes and sort them.
			 */
		
#if DOING_SGL_STYLE_SORTING
			if (( pHead != pTail ) && (RegionPlanes < SAFETY_MARGIN_TRANS))
#else
			if (pHead && (RegionPlanes < SAFETY_MARGIN_TRANS))
#endif
			{

#if DOING_SGL_STYLE_SORTING
				/* Sort the two lists */
				pHead = SortTransFaceLists( pRegion, pHead, pTail );
#endif
			
				if (RegionPlanes < IN_PLANE_CACHE_BOUNDARY)
				{
					TRANSFACE_LIST *pLocalHead = pHead;

					/* Check every pass.
					 */
					do
					{
						/* Count begin translucent pass plane.
						 */	
						TransPassPlanes += NUM_TRANS_PASS_START_PLANES;

						/* Count planes. Translucent planes first.
						 */
						TransPassPlanes += CountShortList( pLocalHead->pLastSlot);

						if ( pRegion->pLastSlots[LIGHTVOL] != NULL )
						{
							/* Count all the light volume planes
							 */
							TransPassPlanes += CountShortList(pRegion->pLastSlots[LIGHTVOL]);
						}

						if ( pRegion->pLastSlots[SHADOW] != NULL )
						{
							/* Count all the shadows planes
							 */
							TransPassPlanes += CountShortList(pRegion->pLastSlots[SHADOW]);
						}

						/* Count flushing plane.
						 */
						TransPassPlanes += FLUSH_PLANE;

						pLocalHead = pLocalHead->pPost;
					} while ( pLocalHead != NULL );
					
#if PCX1
					/* Extra translucent pass for PCX1.
					 */
					TransPassPlanes += NUM_TRANS_PASS_START_PLANES +
									   FLUSH_PLANE +
									   NUM_DUMMY_OBJECT_PLANES;
#endif

				}
			}
			else
			{
				/* No tranlucent passes. Setting pHead to NULL prevents us
				 * from checking everthing again if the vignetting fix is being
				 * used.
				 */
				pHead = NULL;
			}
			
			/* Pad out the opaque pass over the cache boundary if the sum of
			 * the translucent planes plus the current tile plane count exceeds
			 * the cache boundary.
			 *
			 * This fix prevents vignetting since this is due to the translucent
			 * pass crossing the cache boundary.
			 */
			if ((RegionPlanes < IN_PLANE_CACHE_BOUNDARY) &&
				((RegionPlanes + TransPassPlanes) > IN_PLANE_CACHE_BOUNDARY))
			{
				/* Overwrite old flushing plane
				 */
				curAddr--;
						
				while(RegionPlanes < IN_PLANE_CACHE_BOUNDARY)
				{				
					/* Add dummy objects until we meet our min criterion
					 */
					IW( curAddr++, 0, DummyObjDataLarge);
					RegionPlanes += NUM_DUMMY_OBJECT_PLANES_LARGE;
				}
						
				/* Replace flushing plane
				 */
				IW( curAddr++, 0, DummyFlushData);
			}
#endif		/* #if VIGNETTE_FIX	*/
			
			/* Jim's new D3D translucent solution. Only inserted if plane count below
			 * the safety margin.
			 */
			if ((pRegion->pLastSlots[OPAQUETRANS] != NULL ) &&
				(RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Add the dummy begin translucent pass object for this pass.
				 */
				IW(curAddr++ , 0, DummyTransData);
				RegionPlanes += 
					NUM_TRANS_PASS_START_PLANES + pRegion->TransOpaquePlanes;

				/* Add OpaqueTrans objects */
				curAddr = GenerateOpaquePtr ( pRegion, curAddr, &RegionPlanes, &nDiscardedPlanes,
											 OPAQUETRANS, (SAFETY_MARGIN_TRANS - MIN_TRANS_PLANES));

				/* Add the flushing plane for this pass !!!!!!!
				 */
				IW( curAddr++, 0, DummyTransFlushData);
				RegionPlanes+= FLUSH_PLANE;
			}

			/* Initialise count.
			 */
			TransPassPlanes = 0;

#if VIGNETTE_FIX
			/* Normal translucent pass handling. The are translucent passes if pHead != NULL.
			 * The translucent passes have already been sorted if vignetting fix used.
			 */
			if ( pHead )
			{
				/* No need to sort them as already sorted.
				 */
#else
			/* Count the original translucent pass planes and sort them.
			 */
	#if DOING_SGL_STYLE_SORTING
			if (( pHead != pTail ) && (RegionPlanes < SAFETY_MARGIN_TRANS))
			{
				/* Need to sort them. */
				pHead = SortTransFaceLists( pRegion, pHead, pTail );
	#else
			if ( pHead && (RegionPlanes < SAFETY_MARGIN_TRANS))
			{
	#endif

#endif
				do
				{
					/* Add the dummy begin translucent pass object for this pass */
					IW( curAddr++, 0, DummyTransData);
					RegionPlanes += NUM_TRANS_PASS_START_PLANES;

					/* Zero the plane count for upcoming pass */
					TransPassPlanes = 0;
					
					/* Add all the translucent objects */
					curAddr = GenerateTransPtr ( pRegion, curAddr, &RegionPlanes,
									   &nDiscardedPlanes, &TransPassPlanes, pHead); 

					/* Add the flushing plane for this pass !!!!!!!	 */
					IW( curAddr++, 0, DummyTransFlushData);
					RegionPlanes+= FLUSH_PLANE;

					pHead = pHead->pPost;
				}
				while ( ( pHead != NULL ) && (RegionPlanes < SAFETY_MARGIN_TRANS) );


				#if PCX1 
				
					IW( curAddr, 0, (DummyTransData + (1 << OBJ_PCOUNT_SHIFT)));
					IW( curAddr, 1, DummyTransObjData);
					IW( curAddr, 2, DummyTransFlushData);
					curAddr += 3;

					RegionPlanes+= NUM_TRANS_PASS_START_PLANES +
								   FLUSH_PLANE +
								   NUM_DUMMY_OBJECT_PLANES;
				#endif
			}

			/* Update the plane information this strip */
								
			RegionPlanes += nDiscardedPlanes;			

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
			if ( RegionPlanes > ((REGION_PLANE_LIM * 3)/4) ) /* 75% of maximum as threshold */
			{
				/* DECIDE WHAT TO DO */
				if ( (pRegion->HeightStats[BUSY] > ((pRegion->HeightStats[SAD])<<1)) && (RegionPlanes > uBusiestTile) )
				{					
					/* For splitting the tile - note the busiest tile*/
					uBusiestTile = RegionPlanes;
				}
			}
			else if ((SAFETY_MARGIN_OPAQ << 1) > RegionPlanes) 				
			{
				if ( (pRegion->HeightStats[SAD] > ((pRegion->HeightStats[HAPPY])<<1)) && (RegionPlanes > uSadness) )
				{
					/* Attempt to merge */
					uSadness += RegionPlanes;
				}
			}
#endif
		
			--nLeftX;
			
			if (!RegionIsEmpty)
			{
				/* So we know where to truncate the strip */
				pRegionStripExtra->nLeftX = nLeftX;
				pRegionStripExtra->EndOfLastRegion = curAddr;
			}

		} while ( --Regions != 0 );

#if PCX1
/* Disable dynamic tile sizing for PCX2.
 */
		/* Record the emotive details for this strip */
		pStrip->PlaneTally = uBusiestTile - uSadness;
#endif

		/* Last strip done */
		if ( pStrip == pLastStrip ) break;
	}

	/* With PCX1 and PCX2 the very last pointer must be marked with an end bit. */
	IORW( curAddr, -1, OBJ_VERY_LAST_PTR);

	/*
	// Update the Sabre index so that we know how much data has been written
	*/

	PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos += ((sgl_uint32)(curAddr - &PVRParamBuffs[PVR_PARAM_TYPE_REGION].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_REGION].uBufferPos]));
}

/*****************************************************************************
 * Function Name  : SetupStripLite
 * Inputs         : Number of regions in strip 
 * Input/Output	  : 
 * Returns        : 
 * Global Used    : 
 *
 * Description    : 
 *****************************************************************************/
int SetupStripLite (int nStrip,
					const REGIONS_RECT_STRUCT *const pRegionsRect,
					PREGION_STRIP_DATA pRegionStripData)
{
	REGION_STRIP *pStrip;
	/* Convert LastYRegion to actual lines on the screen */
	int YRegLines = RegionInfo.YSize >> Y_SHIFT;
	int HeightSoFar = 0;
	int RegionsSoFar = 0;

	nStrip <<= (5 - Y_SHIFT);

	pRegionStripData->fObjectsPresent = FALSE;

	if ((nStrip >= (pRegionsRect->FirstYRegion * YRegLines)) && 
		(nStrip <= (pRegionsRect->LastYRegion * YRegLines)))
	{
		/* get pointer to strip */
		pStrip = pRegionStrips[nStrip];
		
		/* X extents (pixels) */
		pRegionStripData->nXExtents[0] = 0x7FFF;
		pRegionStripData->nXExtents[1] = 0;
				
		/* collect up all strips up to 32 pixels in height */
		while (pStrip && (HeightSoFar < 32))
		{
			HeightSoFar += (pStrip->Height << Y_SHIFT);

			if (!RegionsSoFar)
			{
				pRegionStripData->FirstObjectOffset = (sgl_uint32) pStrip->pExtra->StartOfFirstRegion;
			}

			if (pStrip->pExtra->nRegionsInStrip)
			{
				/* This region has some data */
				int nLeftX, nRightX;
			   	int nWidthShift = 6;

				if ( (RegionInfo.XSize == 32) && (pStrip->Width == 0) )
				{					
					nWidthShift = 5;
				}

				nLeftX = pStrip->pExtra->nLeftX << nWidthShift;
				nRightX = pStrip->pExtra->nRightX << nWidthShift;

		 	 	pRegionStripData->pLastObject = pStrip->pExtra->EndOfLastRegion - 1;
				pRegionStripData->fObjectsPresent = TRUE;

		 		if (nLeftX < pRegionStripData->nXExtents[0])
				{
					pRegionStripData->nXExtents[0] = nLeftX;
				}
				
				if (nRightX > pRegionStripData->nXExtents[1])
				{
					pRegionStripData->nXExtents[1] = nRightX ;
				}	 
			}

			RegionsSoFar += pStrip->pExtra->nRegionsInStrip;

			pStrip = pStrip->pNext;
		}
		
		if (pRegionStripData->fObjectsPresent)
		{
			/* 
			// With PCX1 and PCX2 the very last pointer must be marked with an end bit.
			*/

			*(pRegionStripData->pLastObject) |= OBJ_VERY_LAST_PTR;
		}
		else
		{
			RegionsSoFar = 0;
		}
	}

	return (RegionsSoFar);
}

void	AddFlushingPlaneL(const sgl_uint32  ObjectAddr)
{
	/* Calc address of the dummy object to be placed at the end of passes */
 	ASSERT((ObjectAddr & (~ OBJ_ADDRESS_MASK)) == 0);
	DummyFlushData = (1 << OBJ_PCOUNT_SHIFT) | ObjectAddr;
}

void AddTransFlushingPlaneL(const sgl_uint32  ObjectAddr)
{
	/* Calc address of the dummy translucent object to be placed
	   at the end of all translucent passes                      */
 	ASSERT((ObjectAddr & (~ OBJ_ADDRESS_MASK)) == 0);
	DummyTransFlushData = (1 << OBJ_PCOUNT_SHIFT) | ObjectAddr;
}

void AddRegionBeginTransDummyL(const sgl_uint32  ObjectAddr)
{
	/* Calc address of the dummy translucent object to be placed
	   at the start of all translucent passes                    */
	ASSERT((ObjectAddr & (~ OBJ_ADDRESS_MASK)) == 0);
	DummyTransData = (NUM_TRANS_PASS_START_PLANES << OBJ_PCOUNT_SHIFT) | ObjectAddr;
}

void	AddRegionOpaqueDummyL(const sgl_uint32  ObjectAddr)
{
	/* Calc pointer and Num planes of the Dummy opaque object */
	ASSERT((ObjectAddr & (~ OBJ_ADDRESS_MASK)) == 0);
	DummyObjData = (NUM_DUMMY_OBJECT_PLANES << OBJ_PCOUNT_SHIFT) | ObjectAddr;
}											

#if VIGNETTE_FIX
/* Required to give large dummy object to fill up to the 1024 cache boundary
 * if vignetting fix required.
 */
void	AddRegionOpaqueDummyLargeL(const sgl_uint32  ObjectAddr)
{
	/* Calc pointer and Num planes of the Dummy opaque object */
	ASSERT((ObjectAddr & (~ OBJ_ADDRESS_MASK)) == 0);
	DummyObjDataLarge = (NUM_DUMMY_OBJECT_PLANES_LARGE << OBJ_PCOUNT_SHIFT) | ObjectAddr;
}
#endif

void	AddRegionTransDummyL(const sgl_uint32  ObjectAddr)
{
#if PCX1
	/* Calc pointer and Num planes of the Dummy opaque object */
	ASSERT((ObjectAddr & (~ OBJ_ADDRESS_MASK)) == 0);
	DummyTransObjData = (NUM_DUMMY_OBJECT_PLANES << OBJ_PCOUNT_SHIFT) | ObjectAddr;
#endif
}											

void	AddCompleteShadowDummyL(const sgl_uint32  ObjectAddr)
{
	/* Set address of Complete Shadow Object */

	CompleteShadowAddress = ObjectAddr;
}

/**************************************************************************
 * Function Name  : AddRegionOpaqueL
 * Inputs         : pRegionsRect - Rectangle of regions
					ObjectAddr - Address of object first plane in sabre 
								 parameter memory
					NumPlanes - total number of planes for object  
 * Outputs        : 
 * Input/Output	  : 
 * Returns        :
 * Global Used    : RegionData
 * Description    : Adds an object reference for an opaque object to
					to region object structure 
 *				   
**************************************************************************/

void AddRegionOpaqueL(const REGIONS_RECT_STRUCT *const pRegionsRect,
					const sgl_uint32  ObjectAddr, const int NumPlanes)
{
	sgl_uint32 XYData;

	/* Pack description, round up LastYRegion to required step size */
	XYData = ENCODE_OBJXYDATA( OPAQUE,
	                           pRegionsRect->FirstXRegion,
	                   pRegionsRect->FirstYRegion*RegionInfo.YSize,
	                           pRegionsRect->LastXRegion,
	         ((pRegionsRect->LastYRegion+1) *RegionInfo.YSize)-1  );

	/* Use the new solid API */
	AddRegionSolid( XYData, NumPlanes, ObjectAddr );
}

/**************************************************************************
 * Function Name  : AddRegionLightVolL
 * Inputs         : pRegionsRect - Rectangle of regions
					ObjectAddr - Address of object first plane in sabre 
								 parameter memory
					NumPlanes - total number of planes for object  
 * Outputs        : 
 * Input/Output	  : 
 * Returns        :
 * Global Used    : RegionData
 * Description    : Adds an object reference for a light volume object to
					the region object structure 
 *				   
**************************************************************************/
void	AddRegionLightVolL(const REGIONS_RECT_STRUCT *const pRegionsRect,
						   const sgl_uint32  ObjectAddr, const int NumPlanes)
{
	sgl_uint32 XYData;

	/* Pack description, round up LastYRegion to required step size */
	XYData = ENCODE_OBJXYDATA( LIGHTVOL,
	                           pRegionsRect->FirstXRegion,
	                   pRegionsRect->FirstYRegion*RegionInfo.YSize,
	                           pRegionsRect->LastXRegion,
	         ((pRegionsRect->LastYRegion+1) *RegionInfo.YSize)-1  );

	/* Use the new Atmos API */
	AddRegionShadorLV( XYData, NumPlanes, ObjectAddr );
}

/**************************************************************************
 * Function Name  : AddRegionShadowL
 * Inputs         : pRegionsRect - Rectangle of regions
					ObjectAddr - Address of object first plane in sabre 
								 parameter memory
					NumPlanes - total number of planes for object  
 * Outputs        : 
 * Input/Output	  : 
 * Returns        :
 * Global Used    : RegionData
 * Description    : Adds an object reference for a shadow object to
					to region object structure 
 *				   
**************************************************************************/
void	AddRegionShadowL(const REGIONS_RECT_STRUCT *const pRegionsRect,
						 const sgl_uint32  ObjectAddr, const int NumPlanes)
{
	sgl_uint32 XYData;

	/* Pack description, round up LastYRegion to required step size */
	XYData = ENCODE_OBJXYDATA( SHADOW,
	                           pRegionsRect->FirstXRegion,
	                   		   pRegionsRect->FirstYRegion*RegionInfo.YSize,
	                           pRegionsRect->LastXRegion,
	         ((pRegionsRect->LastYRegion+1) *RegionInfo.YSize)-1  );

	/* Use the new Atmos API */
	AddRegionShadorLV( XYData, NumPlanes, ObjectAddr );
}

/**************************************************************************
 * Function Name  : AllowLightVolAdditionL and AllowLightVolAdditionSGL 
 * Inputs         : pRegionsRect - Rectangle of regions
					ObjectAddr - Address of object first plane in sabre 
								 parameter memory
					NumPlanes - total number of planes for object 
					NearZ - Z value of closest z of primitives bounding box	 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        :
 * Global Used    : RegionData
 * Description    : Adds an object reference for a translucent object to
					the region object structure 
 *				   
**************************************************************************/

void AllowLightVolAdditionL(const REGIONS_RECT_STRUCT *const pRegionsRect)

{
	/*
	// We'll be up the creek if the number of planes isn't 2
	*/
	const int numPlanes = 2;
	/*
	// Simply call Add Light Volume with the complete shadow obejct
	*/

	AddRegionLightVolL(pRegionsRect, CompleteShadowAddress, numPlanes);
}

/*------------------------------ End of File --------------------------------*/
