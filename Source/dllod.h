/******************************************************************************
 * Name         : dllod.h
 * Title        : Definition of the level of detail display list node structure
 * Author       : John Catchpole
 * Created      : 23/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 * 
 * Description  :
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dllod.h,v $
 * Revision 1.6  1997/06/18  14:38:29  mjg
 * Replaced windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.5  1995/09/22  12:23:34  jrc
 * The stored thresholds are now the squares of half the pixel sizes.
 *
 * Revision 1.4  1995/09/15  18:02:57  jrc
 * Changed to use bounding box instead of sphere.
 *
 * Revision 1.3  1995/06/07  11:13:18  jrc
 * Tidied up comments and added DlDeleteLodNodeRefs declaration.
 *
 * Revision 1.2  1995/06/05  16:37:30  jrc
 * Changed int in display list structures to INT16.
 *
 * Revision 1.1  1995/05/25  16:19:27  jrc
 * Initial revision
 *
 *****************************************************************************/


typedef struct
{
	/*
	// Basic node header:
	*/
	DL_NODE_STRUCT node_hdr;

	/*
	// User's definition of the box kept only for reading by
	// sgl_get_detail_levels:
	*/
	sgl_vector boxCorner1,boxCorner2;

	/*
	// Position of the centre of the bounding sphere in local coordinates:
	*/
	sgl_vector boxCentre;

	/*
	// Two box vertices at either end of one of the longest edges in local
	// coordinates:
	*/
	sgl_vector boxVertex1,boxVertex2;

	/*
	// User's definition of the threshold change sizes kept only for reading by
	// sgl_get_detail_levels:
	*/
	sgl_int16 pn16ChangeSizes[2];

    /*
	// Squares of half of the specified threshold pixel sizes, ordered with the
	// smallest first.
	*/
    float pfHalfThresholds[2];

	/*
	// Names of the three alternate models, the most detailed first.
    // SGL_NULL_LIST is used for an empty model:
    */
	sgl_int16 pn16Models[3];

} LOD_NODE_STRUCT;


extern void DlDeleteLodNodeRefs(DL_NODE_STRUCT *pNodeHdr);


/*------------------------------- End of File -------------------------------*/
