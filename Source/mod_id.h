/*****************************************************************************
;++
Name           	:   $RCSfile: mod_id.h,v $
Title           :   MOD_ID.H
C Author       	:   Jim Page
Created         :   20/04/94

Copyright		: 	1995-2022 Imagination Technologies (c)
License			:	MIT

Description     :   List of single bit moduole IDs for all modules
                    
Program Type    :   C Header

RCS info:

  $Date: 1997/10/15 16:10:10 $
  $Revision: 1.23 $
  $Locker:  $
  $Log: mod_id.h,v $
 * Revision 1.23  1997/10/15  16:10:10  sxy
 * added module ids for d3disp and d3dtri.
 *
 * Revision 1.22  1997/08/08  15:15:32  gdc
 * added module id for sprites
 *
 * Revision 1.21  1997/07/11  15:45:46  gdc
 * frigging typo!?
 *
 * Revision 1.20  1997/07/11  13:24:47  gdc
 * ... and more - should now cover all the current win32 modules
 *
 * Revision 1.19  1997/07/11  11:52:51  gdc
 * addded lots more module id's
 *
 * Revision 1.18  1995/09/12  18:39:15  jop
 * Added MODID_HWSABREN
 *
 * Revision 1.17  1995/08/02  15:35:14  jop
 * Added MODID_LDBMP
 *
 * Revision 1.16  1995/07/30  18:59:30  pdl
 * added init.
 *
 * Revision 1.15  1995/07/24  23:16:20  jop
 * Added pack module id
 *
 * Revision 1.14  1995/07/23  17:18:01  jop
 * Changed module ids from define to enum to support NT window stuff
 * and enum2h.exe
 *
 * Revision 1.13  1995/07/14  16:23:51  jop
 * Added RNMESH ID
 *
 * Revision 1.12  1995/07/14  14:42:01  sjf
 * Added a default module id in case it isn't defined.
 *
 * Revision 1.11  1995/07/06  16:12:50  jop
 * added DLOBJECT id
 *
 * Revision 1.10  1995/06/08  13:07:01  sm
 * Added some IDs
 *
 * Revision 1.9  1995/06/08  12:54:35  jop
 * Did something - can't remember what; probably added an ID
 *
 * Revision 1.8  1995/06/02  16:47:29  jrc
 * Added MODID_DLPOINT.
 *
 * Revision 1.7  1995/06/02  14:59:59  jrc
 * Added MODID_DLCAMERA.
 *
 * Revision 1.6  1995/06/02  12:00:39  jrc
 * Added MODID_DLLOD.
 *
 * Revision 1.5  1995/06/01  19:46:39  jop
 * added MODID_DLMATER
 *
 * Revision 1.4  1995/05/31  17:58:10  sjf
 * Added some more Module IDs.
 *
 * Revision 1.3  1995/05/31  15:48:45  jop
 * Added MODID_DLMESH
 *
 * Revision 1.2  1995/05/02  12:17:26  jop
 * Added MODID_PLAYBACK
 *
 * Revision 1.1  1995/04/27  13:44:38  jop
 * Initial revision
 *

;--
*****************************************************************************/

enum
{
	MODID_DEFAULT, 	/* Other unknown modules */
	MODID_RN, 		/* Other render routines */
	MODID_DL, 		/* Other display list routines */
	MODID_NM, 		/* Name table */
	MODID_RECORDER, /* Recorder */
	MODID_PLAYBACK,	/* Playback */
	MODID_ADJACENC, /* Generate convex adjacency info */
	MODID_ARRAY,    /* Array routines */
	MODID_CALLBACK, /* Callback routines */
	MODID_DEBUG,    /* Debug module */
	MODID_DISP,     /* SGLDirect ISP functions */
	MODID_DLCAMERA, /* SGL Camera display list */
	MODID_DLCONVEX, /* SGL Convex display list */
	MODID_DLDELETE, /* SGL Display list editing */
	MODID_DLGLOBAL, /* SGL Display list globals */
	MODID_DLINES,   /* SGLDirect Line functions */
	MODID_DLLIGHTS, /* SGL Lights display list */
	MODID_DLLISTS,  /* SGL Display list editing */
	MODID_DLLOD,    /* SGL Display list level of detail */
	MODID_DLMATER,
	MODID_DLMESH,
	MODID_DLNEWTRN,
	MODID_DLOBJECT,
	MODID_DLPOINT,
	MODID_DLQUALIT,
	MODID_DLTRANSF,
	MODID_DPOINT,
	MODID_DREGION,
	MODID_DSHADE,
	MODID_DSPRITE,
	MODID_DTEX,
	MODID_DTEXNP,
	MODID_DTRI,
	MODID_DTSP,
	MODID_DVDEVICE,
	MODID_ERROR,
	MODID_LDBIN,
	MODID_LDBMP,
	MODID_LIST,
	MODID_METRICS,
	MODID_NM_IMP,
	MODID_PARMBUFF,
	MODID_PKISP,
	MODID_PKTSP,
	MODID_PROFILE,
	MODID_PVRD,
	MODID_RNCAMERA,
	MODID_RNCONVEX,
	MODID_RNCONVPR,
	MODID_RNFSHADE,
	MODID_RNGLOBAL,
	MODID_RNLIGHTS,
	MODID_RNLOD,
	MODID_RNMATER,
	MODID_RNMESH,
	MODID_RNPOINT,
	MODID_RNQUALIT,
	MODID_RNREJECT,
	MODID_RNRENDER,
	MODID_RNSHADE,
	MODID_RNSHADOW,
	MODID_RNSHADRJ,
	MODID_RNSTATE,
	MODID_RNTEX,
	MODID_RNTRANSF,
	MODID_RNTRAV,
	MODID_SGL_INIT,
	MODID_SGL_MATH,
	MODID_SGLLITE,
	MODID_SGLMEM,
	MODID_SGLTRI,
	MODID_SINGMATH,
	MODID_TEXAPI,
	MODID_TMALLOC,
	MODID_TXMOPS,
	MODID_BRDCFG,
	MODID_BRDSETUP,
	MODID_DISPLAY,
	MODID_HEAP,
	MODID_HWDEVICE,
	MODID_PVRDDLL,
	MODID_PVROSIF,
	MODID_SGLWIN,
	MODID_SYSTEM,
	MODID_TEXIF,
	MODID_VERRC,
	MODID_W32DLL,
	MODID_HWREGS,
	MODID_TEXAS,
	MODID_NEW_THIN,
	MODID_D3DISP,
	MODID_D3DTRI
};

/*
// END OF FOOL
*/

