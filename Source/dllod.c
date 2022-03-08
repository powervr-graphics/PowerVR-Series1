/******************************************************************************
 * Name         : dllod.c
 * Title        : Level of detail display list editing SGL functions.
 * Author       : John Catchpole
 * Created      : 01/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dllod.c,v $
 * Revision 1.24  1997/06/04  15:33:15  gdc
 * stopped compiler warnings
 *
 * Revision 1.23  1997/05/13  17:51:45  mjg
 * Added sglmem include.
 *
 * Revision 1.22  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.21  1997/04/24  20:51:07  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.20  1997/04/04  13:02:22  jop
 * Added explicit #include "pvrosapi.h"
 *
 * Revision 1.19  1997/04/03  13:12:22  mjg
 * Updated the calling convention.
 *
 * Revision 1.18  1996/01/09  12:02:11  jrc
 * sgl_err_bad_model and sgl_err_bad_size have been renamed to
 * sgl_err_bad_parameter for consistency with other functions.
 *
 * Revision 1.17  1995/09/22  12:22:27  jrc
 * The stored thresholds are now the squares of half the pixel sizes.
 *
 * Revision 1.16  1995/09/15  18:00:37  jrc
 * Changed method to use bounding box instead of sphere.
 *
 * Revision 1.15  1995/06/26  09:27:48  jrc
 * Removed sgl.h #include because it is now included by sgl_defs.h
 *
 * Revision 1.14  1995/06/21  16:12:16  jrc
 * 1) No longer assumes that NULL is zero in ASSERTs.
 * 2) Partitioned functions into different scopes.
 *
 * Revision 1.13  1995/06/20  15:26:47  jrc
 * Added DlComplete* to sgl_get_detail_levels function.
 *
 * Revision 1.12  1995/06/13  12:27:14  jrc
 * No longer assumes that NULL is zero (except in ASSERTs).
 *
 * Revision 1.11  1995/06/13  10:34:08  jrc
 * 1) Defined an 'undefined' return value for sgl_create_detail_levels.
 * 2) Changed n16 internal variable to int.
 *
 * Revision 1.10  1995/06/09  15:46:52  jrc
 * Fixed name error handling in sgl_create_detail_levels.
 *
 * Revision 1.9  1995/06/07  11:34:53  jrc
 * Added DlDeleteLodNodeRefs and code for incrementing and decrementing usage
 * counts for the model list nodes.
 *
 * Revision 1.8  1995/06/06  12:01:54  jrc
 * Made internal function static.
 *
 * Revision 1.7  1995/06/05  15:02:06  jrc
 * Added calls to DlCompleteCurrentMesh.
 *
 * Revision 1.6  1995/06/05  12:24:12  jrc
 * Added some { }s to 'if' statements.
 *
 * Revision 1.5  1995/06/02  14:30:29  jrc
 * Slight optimisation to SglWriteLodData.
 *
 * Revision 1.4  1995/06/02  12:08:58  jrc
 * Added some debug messages.
 *
 * Revision 1.3  1995/06/01  12:37:53  jrc
 * No longer checks sglSystemInitialised value.
 *
 * Revision 1.2  1995/06/01  12:00:37  jrc
 * Simplified #includes by using dlnodes.h.
 *
 * Revision 1.1  1995/06/01  11:45:34  jrc
 * Initial revision
 *
 *****************************************************************************/


#define MODULE_ID MODID_DLLOD

#include "sgl_defs.h"
#include "sgl_init.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "sgl_math.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"

/*
// ============================================================================
// 								STATIC FUNCTIONS:
// ============================================================================
*/

/******************************************************************************
 * Function Name: WriteData
 *
 * Inputs       : pNode, boxCorner1,boxCorner2, npModels, pnChangeSizes
 * Outputs      : -
 * Returns      : nError
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Installs the given level of detail information, adjusting it
 *                to the requirements of the render traversal calculations.
 *
 *                pNode:            The destination level of detail display
 *                                  list node structure.
 *                other parameters: The new level of detail information.
 *****************************************************************************/
static int WriteData(LOD_NODE_STRUCT *pNode, sgl_vector boxCorner1,
  sgl_vector boxCorner2, int pnModels[3], int pnChangeSizes[2])
{
    int	  nError = sgl_no_err, nAxis,nLongestAxis=0, nSize, nModel,nList;
	float fLength, fLongest = 0.0f;

	ASSERT(pNode != NULL)
	ASSERT(boxCorner1 != NULL)
	ASSERT(boxCorner2 != NULL)
	ASSERT(pnModels != NULL)
	ASSERT(pnChangeSizes != NULL)

	/*
	// -------------------
	// pNode->boxCorner1|2
	// -------------------
	*/
	VecCopy(boxCorner1, pNode->boxCorner1);
	VecCopy(boxCorner2, pNode->boxCorner2);

	/*
	// ----------------
	// pNode->boxCentre
	// ----------------
	*/
	pNode->boxCentre[0] = (boxCorner1[0] + boxCorner2[0]) * 0.5f;
	pNode->boxCentre[1] = (boxCorner1[1] + boxCorner2[1]) * 0.5f;
	pNode->boxCentre[2] = (boxCorner1[2] + boxCorner2[2]) * 0.5f;

	/*
	// -------------------
	// pNode->boxVertex1|2
	// -------------------
	*/
	for (nAxis=0; nAxis <= 2; nAxis++)
	{
		fLength = sfabs(boxCorner1[nAxis] - boxCorner2[nAxis]);
		if (fLength > fLongest)
		{
			fLongest = fLength;
			nLongestAxis = nAxis;
		}
	}
	VecCopy(boxCorner1, pNode->boxVertex1);
	VecCopy(boxCorner1, pNode->boxVertex2);
	pNode->boxVertex2[nLongestAxis] = boxCorner2[nLongestAxis];

	/*
	// ----------------------
	// pNode->pn16ChangeSizes
	// ----------------------
	*/
	pNode->pn16ChangeSizes[0] = pnChangeSizes[0];
	pNode->pn16ChangeSizes[1] = pnChangeSizes[1];

    for (nSize=0; nSize <= 1; nSize++)
        if (pnChangeSizes[nSize] <= 0)
        {
            pNode->pn16ChangeSizes[nSize] = 1;
            nError = sgl_err_bad_parameter;
        }
    if (pNode->pn16ChangeSizes[0] > pNode->pn16ChangeSizes[1])
    {
        sgl_int16 n16Temp = pNode->pn16ChangeSizes[0];
		pNode->pn16ChangeSizes[0] = pNode->pn16ChangeSizes[1];
		pNode->pn16ChangeSizes[1] = n16Temp;
    }

	/*
	// -----------------------
	// pNode->pfHalfThresholds
	// -----------------------
	// Squares of half the specified threshold pixel sizes.
	*/
    pNode->pfHalfThresholds[0] =
	  0.25f * (float)(pNode->pn16ChangeSizes[0] * pNode->pn16ChangeSizes[0]);
    pNode->pfHalfThresholds[1] =
	  0.25f * (float)(pNode->pn16ChangeSizes[1] * pNode->pn16ChangeSizes[1]);

	/*
	// -----------------
	// pNode->pn16Models
	// -----------------
	*/
    pNode->pn16Models[0] = (sgl_int16)pnModels[0];
    pNode->pn16Models[1] = (sgl_int16)pnModels[1];
    pNode->pn16Models[2] = (sgl_int16)pnModels[2];

    for (nModel=0; nModel <= 2; nModel++)
	{
		nList = pnModels[nModel];
		if (nList != SGL_NULL_LIST)
		{
			if (nList != SGL_DEFAULT_LIST &&
			  GetNamedItemType(dlUserGlobals.pNamtab, nList) == nt_list_node)
			{
				IncNamedItemUsage(dlUserGlobals.pNamtab, nList);
			}
			else
			{
            	pNode->pn16Models[nModel] = SGL_NULL_LIST;
				nError = sgl_err_bad_parameter;
			}
		}
	}

    return nError;
}


/******************************************************************************
 * Function Name: DecrementOldModels
 *
 * Inputs       : pNode
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Decrements the usage count for any lists that we do not need
 *                any more.
 *****************************************************************************/
static void DecrementOldModels(LOD_NODE_STRUCT *pNode)
{
	int   nModel;
	sgl_int16 n16ModelList;

	for (nModel=0; nModel<=2; nModel++)
	{
		n16ModelList = pNode->pn16Models[nModel];
		if (n16ModelList != SGL_NULL_LIST)
		{
			DecNamedItemUsage(dlUserGlobals.pNamtab, n16ModelList);
		}
	}
}


/*
// ============================================================================
// 							  SGL INTERNAL ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: DlDeleteLodNodeRefs
 *
 * Inputs       : pNodeHdr
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Cleanup processing before deletion of a level of detail
 *				  display list node.
 *****************************************************************************/
void DlDeleteLodNodeRefs(DL_NODE_STRUCT *pNodeHdr)
{
	ASSERT(pNodeHdr != NULL)
	DecrementOldModels( (LOD_NODE_STRUCT*)pNodeHdr );
}


/*
// ============================================================================
// 								SGL API ROUTINES:
// ============================================================================
*/

/******************************************************************************
 * Function Name: sgl_create_detail_levels
 *
 * Inputs       : bGenerateName, boxCorner1,boxCorner2, npModels, pnChangeSizes
 * Outputs      : -
 * Returns      : name or error
 * Globals Used : dlUserGlobals.pNamtab, dlUserGlobals.pCurrentList
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_create_detail_levels( sgl_bool bGenerateName,
										sgl_vector boxCorner1,
									    sgl_vector boxCorner2,
										int pnModels[3],
										int pnChangeSizes[2] )
{
    /*
	// ==========
	// INITIALISE
	// ==========
	*/
    LOD_NODE_STRUCT *pNode;
    int nReturn=0; /* This zero value is for 'undefined' return conditions. */


#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif

    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

	/*
	// =========
	// MAKE NODE
	// =========
	*/
    pNode = NEW(LOD_NODE_STRUCT);
	if (pNode == NULL)
	{
		return SglError(sgl_err_no_mem);
	}

    pNode->node_hdr.n16_node_type = nt_lod;

	if (bGenerateName)
	{
		int nName;

		nName = AddNamedItem(dlUserGlobals.pNamtab, pNode, nt_lod);
		pNode->node_hdr.n16_name =
		  nName == sgl_err_no_name ? NM_INVALID_NAME : (sgl_int16)nName;
		nReturn = nName;
	}
	else
	{
    	pNode->node_hdr.n16_name = NM_INVALID_NAME;
		/* Return value undefined. */
	}

    {
		int nWriteError;

		nWriteError =
		  WriteData(pNode, boxCorner1,boxCorner2, pnModels, pnChangeSizes);

		if (nWriteError != sgl_no_err)
		{
			nReturn = nWriteError;
		}
	}

	/*
	// ===========
	// ADD TO LIST
	// ===========
	*/
    AppendNodeToList(dlUserGlobals.pCurrentList, pNode);

    /*
	// ======
	// RETURN
	// ======
	// The return value is only meaningful if a name was requested.
	*/
    return SglError(nReturn);
}


/******************************************************************************
 * Function Name: sgl_get_detail_levels
 *
 * Inputs       : nName
 * Outputs      : boxCorner1,boxCorner2, pnModels, pnChangeSizes
 * Returns      : error status
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
int CALL_CONV sgl_get_detail_levels( int nName,
									 sgl_vector boxCorner1,
  									 sgl_vector boxCorner2, 
									 int pnModels[3], int pnChangeSizes[2] )
{
    LOD_NODE_STRUCT *pNode;

	ASSERT(boxCorner1 != NULL)
	ASSERT(boxCorner2 != NULL)
	ASSERT(pnModels != NULL)
	ASSERT(pnChangeSizes != NULL)

#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_lod)
	{
        return SglError(sgl_err_bad_name);
	}

    pNode = GetNamedItem(dlUserGlobals.pNamtab, nName);

	VecCopy(pNode->boxCorner1, boxCorner1);
	VecCopy(pNode->boxCorner2, boxCorner2);

	pnModels[0] = pNode->pn16Models[0];
	pnModels[1] = pNode->pn16Models[1];
	pnModels[2] = pNode->pn16Models[2];

    pnChangeSizes[0] = (int)pNode->pn16ChangeSizes[0];
	pnChangeSizes[1] = (int)pNode->pn16ChangeSizes[1];

    return SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_set_detail_levels
 *
 * Inputs       : nName, boxCorner1,boxCorner2, pnModels, pnChangeSizes
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SGL API function: see specification document.
 *****************************************************************************/
void CALL_CONV sgl_set_detail_levels( int nName,
									  sgl_vector boxCorner1,
									  sgl_vector boxCorner2,
									  int pnModels[3], int pnChangeSizes[2] )
{
    LOD_NODE_STRUCT *pNode;

	ASSERT(boxCorner1 != NULL)
	ASSERT(boxCorner2 != NULL)
	ASSERT(pnModels != NULL)
	ASSERT(pnChangeSizes != NULL)

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init); return;
	}
#endif
    DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();

    if (GetNamedItemType(dlUserGlobals.pNamtab, nName) != nt_lod)
    {
		SglError(sgl_err_bad_name);
		return;
	}

    pNode = GetNamedItem(dlUserGlobals.pNamtab, nName);

	DecrementOldModels(pNode);

    SglError(WriteData(pNode, boxCorner1,boxCorner2, pnModels, pnChangeSizes));
}

/*------------------------------- End of File -------------------------------*/
