/*****************************************************************************
 *;++
 * Name       : $RCSfile: rnshade.c,v $
 * Title      : RNSHADE.C
 * C Author       : Jim Page
 * Created    : 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    : Routines for generating smooth shading parameters for
 *          texas
 *           
 * Program Type   : C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1997/06/18 14:41:42 $
 * $Revision: 1.64 $
 * $Locker:  $
 * $Log: rnshade.c,v $
 * Revision 1.64  1997/06/18  14:41:42  mjg
 * Replaced Windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.63  1997/06/04  15:31:35  gdc
 * stopped compiler warnings
 *
 * Revision 1.62  1997/04/24  13:17:40  mjg
 * Removed the compiler warnings.
 *
 * Revision 1.61  1997/04/09  16:55:24  gdc
 * removed reference to projection_mat global
 *
 * Revision 1.60  1997/04/03  10:13:54  rgi
 * Ported ZEUS_ARCADE support to top level
 *
 * Revision 1.59  1997/03/10  17:30:58  sxy
 * removed diffuse and added glow colour for smooth shading planes.
 *
 * Revision 1.58  1997/03/10  09:25:41  sxy
 * Added diffuse and ambient colours from the lights to visible planes, fixed the bug for colour lights.
 *
 * Revision 1.57  1996/12/09  13:41:31  gdc
 * removed all references to Triangle API, assume that D3DOPTIM stuff
 * has taken over and is now only option
 *
 * Revision 1.56  1996/11/29  22:02:07  erf
 * Output warning if deltas are zero in SmoothPlaneDataPrecalc().
 *
 * Revision 1.55  1996/10/25  00:06:25  jop
 * Added clamp for specular highlights
 *
 * Revision 1.54  1996/10/24  14:18:21  jop
 * Partly fixed specular saturation
 *
 * Revision 1.53  1996/10/24  10:35:13  mal
 * Removed some excess calculations and replaced them
 * with indicies used later in the fReverseMap calculations.
 * Also whilst on a performance hunt changed some fpcmp's
 * to integer ones.
 *
 * Revision 1.52  1996/10/14  10:31:16  msh
 * GetIntensities and GetIntensitiesSpecular now modified to
 * be able to handle BLACK (got zero-divide before)
 *
 * Revision 1.51  1996/10/11  12:08:39  msh
 * "Small number" checks modified for Midas Arcade
 *
 * Revision 1.50  1996/10/11  11:59:56  mal
 * Modified some metric macros.
 *
 * Revision 1.49  1996/10/08  18:45:01  jop
 * Fixed highlit smooth shaded bug
 *
 * Revision 1.48  1996/10/04  14:28:10  mal
 * Added the metric code for smooth shading.
 *
 * Revision 1.47  1996/10/04  11:29:30  gdc
 * fixed smooth shading for off-screen point in 2 directions
 * at once in triangle api
 *
 * Revision 1.46  1996/10/02  11:56:36  gdc
 * fixed clipped smooth shading in triangle api
 *
 * Revision 1.45  1996/09/19  14:19:48  mal
 * Added a check of the bNewProjRepPoint flag so that smooth shading
 * uses the off screen projected rep point instead of a clipped one.
 * This fixes the off screen rep point problem. Re: NECHE 094.
 *
 * Revision 1.44  1996/09/05  17:02:02  jop
 * Went back to Adjoint calc method (sparklies fix)
 *
 * Revision 1.43  1996/09/04  00:01:15  jop
 * Moved over to using inverse matrix in triangle code
 *
 * Revision 1.42  1996/08/14  14:41:23  pdl
 * added an #if SLOW_FCMP
 *
 * Revision 1.41  1996/08/09  11:35:30  jop
 * Made a change to the magnitude of the average colours
 *
 * Revision 1.40  1996/08/02  18:01:26  jop
 * Added lookup for scaling, but divisions seem to be faster (!?)
 *
 * Revision 1.39  1996/07/31  15:57:40  gdc
 * changed CHOOSE_MIN/MAX macros to do a long int comparison, and reformatted
 * table at top of file to take up less lines
 *
 * Revision 1.38  1996/07/29  17:05:23  jop
 * Fix for dark smooth shading - uses a division. Must replace with LUT
 *
 * Revision 1.37  1996/07/19  19:33:55  jop
 * Optimised triangle shading
 *
 * Revision 1.36  1996/07/11  16:40:47  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.35  1996/07/08  17:31:03  jop
 * Added new smooth shading mode
 *
 * Revision 1.34  1996/07/05  15:11:36  jop
 * Fixes for clipping bug
 *
 * Revision 1.33  1996/06/28  17:44:01  sjf
 * Just got rid of those horrid ^M's.
 *
 * Revision 1.32  1996/06/20  20:37:06  jop
 * Jim appears to have made a determinant test lest strict (SJF).
 *
 * Revision 1.31  1996/06/05  15:53:28  jop
 * Added clipped version for tri api
 *
 * Revision 1.30  1996/05/22  11:07:27  jop
 * Fixed dark shading bug and added (unused so far) lookup
 * table for colour scaling
 *
 * Revision 1.29  1996/05/20  15:56:33  jop
 * Added colour scaling by lookup table to triangle stuff
 *
 * Revision 1.28  1996/05/15  10:42:29  jop
 * Moved to new triPrecalc structure
 *
 * Revision 1.27  1996/05/10  16:53:09  jrc
 * Changed dwColour to u32Colour for triangle vertices.
 *
 * Revision 1.26  1996/05/10  16:19:13  jop
 * Little bits
 *
 * Revision 1.25  1996/05/10  14:45:34  jop
 * Added triangle support
 *
 * Revision 1.24  1996/05/09  17:34:52  jop
 * Added triangle smooth shading support
 *
 * Revision 1.23  1996/05/03  16:28:15  sjf
 * Changed T0 etc paramter from INT16 to int.
 *
 * Revision 1.22  1996/04/22  11:43:03  sjf
 * Split the lights loops into 2 loops to make use of the fact
 * that lights are grouped into On Parallel, On Point, and Off
 * lights.
 *
 * Revision 1.21  1996/04/20  16:18:35  sjf
 * the last one didn't compile ... Fixed now!
 *
 * Revision 1.20  1996/04/20  15:21:25  sjf
 * Minor Optimisations
 * 1) Changed to use sfabs and made sure FP consts were float not double
 * 2) Did some pointer maths instead of array indexing.
 *
 * Revision 1.19  1996/03/19  16:49:50  jop
 *  Sorted out double slash error
 *
 * Revision 1.18  1996/03/11  16:13:51  jop
 * Re-jigged smooth shading to have more control over truncation - should
 * go faster with multiple lights.
 *
 * Revision 1.17  1996/03/04  16:13:46  jop
 * Now clamps upper value in T0 parameters
 *
 * Revision 1.16  1996/02/20  15:33:06  jrc
 * 1. Removed some unused variables.
 * 2. Removed redundant '&' for copying pointer to inverse transform matrix.
 *
 * Revision 1.15  1996/01/31  14:27:38  jop
 * Updated shading/points struct
 *
 * Revision 1.14  1995/11/07  18:31:52  jop
 * Smooth shading now rep-point independant
 *
 * Revision 1.13  1995/10/06  14:20:39  jop
 * Now moves offscreen rep point on screen and uses private rep point
 * mamber of shading results structure.
 *
 * Revision 1.12  1995/09/25  11:56:49  jop
 * Normals round the other way now.
 *
 * Revision 1.11  1995/09/18  12:02:23  jop
 * Changed de-fringing algorithm, still not quite right
 *
 * Revision 1.10  1995/09/14  12:06:43  jop
 * Added SmoothPlaneDataPrecalc function
 *
 * Revision 1.9  1995/09/13  18:26:25  jop
 * Added precalc function
 *
 * Revision 1.8  1995/09/04  13:17:44  jop
 * optimised code and added Bishop&Weimer point light code
 *
 * Revision 1.7  1995/08/23  15:57:17  jop
 * Texel sampling and lighting corrected
 *
 * Revision 1.6  1995/08/22  17:24:26  jop
 * Now mixes colours properly
 *
 * Revision 1.4  1995/08/21  11:54:04  jop
 * removed rgl stuff and fixed some bugs
 * ,
 *
 * Revision 1.3  1995/08/14  12:11:50  jop
 * First proper stab at getting smooth shading going
 *
 * Revision 1.1  1995/06/29  18:21:14  jop
 * Initial revision
 *
 * Revision 1.1  1995/06/05  16:37:20  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#define MODULE_ID   MODID_RNSHADE

#include <math.h>
#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"
#include "rnstate.h"
#include "rnglobal.h"
#include "sgl_math.h"
#include "rnconvst.h"
#include "rnmesh.h"
#include "rnfshade.h"
#include "rnshade.h"
#include "rnlights.h"
#include "txmops.h"

#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/* macros to do a floating point assignment by comparing as longs
** only works when b and c are constants, b should be the long interpretation
** of c 
*/
#if SLOW_FCMP

#define CHOOSE_FLOAT_MIN(a,b,c)	((b)<FLOAT_TO_LONG(a)?(a=c):(a))
#define CHOOSE_FLOAT_MAX(a,b,c)	((b)>FLOAT_TO_LONG(a)?(a=c):(a))

#endif

#if 1
	#define COLOUR_SCALE(x) (1.0f / (x))
#else
	#define COLOUR_SCALE(x) (ColourScale[(sgl_int32) (x)] >> 18)

/* ScaleFactor = ColourScale[MaxCol]; */
/* NewCol = OldCol * ScaleFactor; */

#define SHF	(256*256*8)

static int ColourScale[765] = 
{
	SHF,	/* shouldn't be needed! */
	SHF / 1,	SHF / 2,	SHF / 3,	SHF / 4,	SHF / 5,	SHF / 6,	SHF / 7,	SHF / 8,	SHF / 9,	SHF / 10,
	SHF / 11,	SHF / 12,	SHF / 13,	SHF / 14,	SHF / 15,	SHF / 16,	SHF / 17,	SHF / 18,	SHF / 19,	SHF / 20,
	SHF / 21,	SHF / 22,	SHF / 23,	SHF / 24,	SHF / 25,	SHF / 26,	SHF / 27,	SHF / 28,	SHF / 29,	SHF / 30,
	SHF / 31,	SHF / 32,	SHF / 33,	SHF / 34,	SHF / 35,	SHF / 36,	SHF / 37,	SHF / 38,	SHF / 39,	SHF / 40,
	SHF / 41,	SHF / 42,	SHF / 43,	SHF / 44,	SHF / 45,	SHF / 46,	SHF / 47,	SHF / 48,	SHF / 49,	SHF / 50,
	SHF / 51,	SHF / 52,	SHF / 53,	SHF / 54,	SHF / 55,	SHF / 56,	SHF / 57,	SHF / 58,	SHF / 59,	SHF / 60,
	SHF / 61,	SHF / 62,	SHF / 63,	SHF / 64,	SHF / 65,	SHF / 66,	SHF / 67,	SHF / 68,	SHF / 69,	SHF / 70,
	SHF / 71,	SHF / 72,	SHF / 73,	SHF / 74,	SHF / 75,	SHF / 76,	SHF / 77,	SHF / 78,	SHF / 79,	SHF / 80,
	SHF / 81,	SHF / 82,	SHF / 83,	SHF / 84,	SHF / 85,	SHF / 86,	SHF / 87,	SHF / 88,	SHF / 89,	SHF / 90,
	SHF / 91,	SHF / 92,	SHF / 93,	SHF / 94,	SHF / 95,	SHF / 96,	SHF / 97,	SHF / 98,	SHF / 99,	SHF / 100,
	SHF / 101,	SHF / 102,	SHF / 103,	SHF / 104,	SHF / 105,	SHF / 106,	SHF / 107,	SHF / 108,	SHF / 109,	SHF / 110,
	SHF / 111,	SHF / 112,	SHF / 113,	SHF / 114,	SHF / 115,	SHF / 116,	SHF / 117,	SHF / 118,	SHF / 119,	SHF / 120,
	SHF / 121,	SHF / 122,	SHF / 123,	SHF / 124,	SHF / 125,	SHF / 126,	SHF / 127,	SHF / 128,	SHF / 129,	SHF / 130,
	SHF / 131,	SHF / 132,	SHF / 133,	SHF / 134,	SHF / 135,	SHF / 136,	SHF / 137,	SHF / 138,	SHF / 139,	SHF / 140,
	SHF / 141,	SHF / 142,	SHF / 143,	SHF / 144,	SHF / 145,	SHF / 146,	SHF / 147,	SHF / 148,	SHF / 149,	SHF / 150,
	SHF / 151,	SHF / 152,	SHF / 153,	SHF / 154,	SHF / 155,	SHF / 156,	SHF / 157,	SHF / 158,	SHF / 159,	SHF / 160,
	SHF / 161,	SHF / 162,	SHF / 163,	SHF / 164,	SHF / 165,	SHF / 166,	SHF / 167,	SHF / 168,	SHF / 169,	SHF / 170,
	SHF / 171,	SHF / 172,	SHF / 173,	SHF / 174,	SHF / 175,	SHF / 176,	SHF / 177,	SHF / 178,	SHF / 179,	SHF / 180,
	SHF / 181,	SHF / 182,	SHF / 183,	SHF / 184,	SHF / 185,	SHF / 186,	SHF / 187,	SHF / 188,	SHF / 189,	SHF / 190,
	SHF / 191,	SHF / 192,	SHF / 193,	SHF / 194,	SHF / 195,	SHF / 196,	SHF / 197,	SHF / 198,	SHF / 199,	SHF / 200,
	SHF / 201,	SHF / 202,	SHF / 203,	SHF / 204,	SHF / 205,	SHF / 206,	SHF / 207,	SHF / 208,	SHF / 209,	SHF / 210,
	SHF / 211,	SHF / 212,	SHF / 213,	SHF / 214,	SHF / 215,	SHF / 216,	SHF / 217,	SHF / 218,	SHF / 219,	SHF / 220,
	SHF / 221,	SHF / 222,	SHF / 223,	SHF / 224,	SHF / 225,	SHF / 226,	SHF / 227,	SHF / 228,	SHF / 229,	SHF / 230,
	SHF / 231,	SHF / 232,	SHF / 233,	SHF / 234,	SHF / 235,	SHF / 236,	SHF / 237,	SHF / 238,	SHF / 239,	SHF / 240,
	SHF / 241,	SHF / 242,	SHF / 243,	SHF / 244,	SHF / 245,	SHF / 246,	SHF / 247,	SHF / 248,	SHF / 249,	SHF / 250,
	SHF / 251,	SHF / 252,	SHF / 253,	SHF / 254,	SHF / 255,	SHF / 256,	SHF / 257,	SHF / 258,	SHF / 259,	SHF / 260,
	SHF / 261,	SHF / 262,	SHF / 263,	SHF / 264,	SHF / 265,	SHF / 266,	SHF / 267,	SHF / 268,	SHF / 269,	SHF / 270,
	SHF / 271,	SHF / 272,	SHF / 273,	SHF / 274,	SHF / 275,	SHF / 276,	SHF / 277,	SHF / 278,	SHF / 279,	SHF / 280,
	SHF / 281,	SHF / 282,	SHF / 283,	SHF / 284,	SHF / 285,	SHF / 286,	SHF / 287,	SHF / 288,	SHF / 289,	SHF / 290,
	SHF / 291,	SHF / 292,	SHF / 293,	SHF / 294,	SHF / 295,	SHF / 296,	SHF / 297,	SHF / 298,	SHF / 299,	SHF / 300,
	SHF / 301,	SHF / 302,	SHF / 303,	SHF / 304,	SHF / 305,	SHF / 306,	SHF / 307,	SHF / 308,	SHF / 309,	SHF / 310,
	SHF / 311,	SHF / 312,	SHF / 313,	SHF / 314,	SHF / 315,	SHF / 316,	SHF / 317,	SHF / 318,	SHF / 319,	SHF / 320,
	SHF / 321,	SHF / 322,	SHF / 323,	SHF / 324,	SHF / 325,	SHF / 326,	SHF / 327,	SHF / 328,	SHF / 329,	SHF / 330,
	SHF / 331,	SHF / 332,	SHF / 333,	SHF / 334,	SHF / 335,	SHF / 336,	SHF / 337,	SHF / 338,	SHF / 339,	SHF / 340,
	SHF / 341,	SHF / 342,	SHF / 343,	SHF / 344,	SHF / 345,	SHF / 346,	SHF / 347,	SHF / 348,	SHF / 349,	SHF / 350,
	SHF / 351,	SHF / 352,	SHF / 353,	SHF / 354,	SHF / 355,	SHF / 356,	SHF / 357,	SHF / 358,	SHF / 359,	SHF / 360,
	SHF / 361,	SHF / 362,	SHF / 363,	SHF / 364,	SHF / 365,	SHF / 366,	SHF / 367,	SHF / 368,	SHF / 369,	SHF / 370,
	SHF / 371,	SHF / 372,	SHF / 373,	SHF / 374,	SHF / 375,	SHF / 376,	SHF / 377,	SHF / 378,	SHF / 379,	SHF / 380,
	SHF / 381,	SHF / 382,	SHF / 383,	SHF / 384,	SHF / 385,	SHF / 386,	SHF / 387,	SHF / 388,	SHF / 389,	SHF / 390,
	SHF / 391,	SHF / 392,	SHF / 393,	SHF / 394,	SHF / 395,	SHF / 396,	SHF / 397,	SHF / 398,	SHF / 399,	SHF / 400,
	SHF / 401,	SHF / 402,	SHF / 403,	SHF / 404,	SHF / 405,	SHF / 406,	SHF / 407,	SHF / 408,	SHF / 409,	SHF / 410,
	SHF / 411,	SHF / 412,	SHF / 413,	SHF / 414,	SHF / 415,	SHF / 416,	SHF / 417,	SHF / 418,	SHF / 419,	SHF / 420,
	SHF / 421,	SHF / 422,	SHF / 423,	SHF / 424,	SHF / 425,	SHF / 426,	SHF / 427,	SHF / 428,	SHF / 429,	SHF / 430,
	SHF / 431,	SHF / 432,	SHF / 433,	SHF / 434,	SHF / 435,	SHF / 436,	SHF / 437,	SHF / 438,	SHF / 439,	SHF / 440,
	SHF / 441,	SHF / 442,	SHF / 443,	SHF / 444,	SHF / 445,	SHF / 446,	SHF / 447,	SHF / 448,	SHF / 449,	SHF / 450,
	SHF / 451,	SHF / 452,	SHF / 453,	SHF / 454,	SHF / 455,	SHF / 456,	SHF / 457,	SHF / 458,	SHF / 459,	SHF / 460,
	SHF / 461,	SHF / 462,	SHF / 463,	SHF / 464,	SHF / 465,	SHF / 466,	SHF / 467,	SHF / 468,	SHF / 469,	SHF / 470,
	SHF / 471,	SHF / 472,	SHF / 473,	SHF / 474,	SHF / 475,	SHF / 476,	SHF / 477,	SHF / 478,	SHF / 479,	SHF / 480,
	SHF / 481,	SHF / 482,	SHF / 483,	SHF / 484,	SHF / 485,	SHF / 486,	SHF / 487,	SHF / 488,	SHF / 489,	SHF / 490,
	SHF / 491,	SHF / 492,	SHF / 493,	SHF / 494,	SHF / 495,	SHF / 496,	SHF / 497,	SHF / 498,	SHF / 499,	SHF / 500,
	SHF / 501,	SHF / 502,	SHF / 503,	SHF / 504,	SHF / 505,	SHF / 506,	SHF / 507,	SHF / 508,	SHF / 509,	SHF / 510,
	SHF / 511,	SHF / 512,	SHF / 513,	SHF / 514,	SHF / 515,	SHF / 516,	SHF / 517,	SHF / 518,	SHF / 519,	SHF / 520,
	SHF / 521,	SHF / 522,	SHF / 523,	SHF / 524,	SHF / 525,	SHF / 526,	SHF / 527,	SHF / 528,	SHF / 529,	SHF / 530,
	SHF / 531,	SHF / 532,	SHF / 533,	SHF / 534,	SHF / 535,	SHF / 536,	SHF / 537,	SHF / 538,	SHF / 539,	SHF / 540,
	SHF / 541,	SHF / 542,	SHF / 543,	SHF / 544,	SHF / 545,	SHF / 546,	SHF / 547,	SHF / 548,	SHF / 549,	SHF / 550,
	SHF / 551,	SHF / 552,	SHF / 553,	SHF / 554,	SHF / 555,	SHF / 556,	SHF / 557,	SHF / 558,	SHF / 559,	SHF / 560,
	SHF / 561,	SHF / 562,	SHF / 563,	SHF / 564,	SHF / 565,	SHF / 566,	SHF / 567,	SHF / 568,	SHF / 569,	SHF / 570,
	SHF / 571,	SHF / 572,	SHF / 573,	SHF / 574,	SHF / 575,	SHF / 576,	SHF / 577,	SHF / 578,	SHF / 579,	SHF / 580,
	SHF / 581,	SHF / 582,	SHF / 583,	SHF / 584,	SHF / 585,	SHF / 586,	SHF / 587,	SHF / 588,	SHF / 589,	SHF / 590,
	SHF / 591,	SHF / 592,	SHF / 593,	SHF / 594,	SHF / 595,	SHF / 596,	SHF / 597,	SHF / 598,	SHF / 599,	SHF / 600,
	SHF / 601,	SHF / 602,	SHF / 603,	SHF / 604,	SHF / 605,	SHF / 606,	SHF / 607,	SHF / 608,	SHF / 609,	SHF / 610,
	SHF / 611,	SHF / 612,	SHF / 613,	SHF / 614,	SHF / 615,	SHF / 616,	SHF / 617,	SHF / 618,	SHF / 619,	SHF / 620,
	SHF / 621,	SHF / 622,	SHF / 623,	SHF / 624,	SHF / 625,	SHF / 626,	SHF / 627,	SHF / 628,	SHF / 629,	SHF / 630,
	SHF / 631,	SHF / 632,	SHF / 633,	SHF / 634,	SHF / 635,	SHF / 636,	SHF / 637,	SHF / 638,	SHF / 639,	SHF / 640,
	SHF / 641,	SHF / 642,	SHF / 643,	SHF / 644,	SHF / 645,	SHF / 646,	SHF / 647,	SHF / 648,	SHF / 649,	SHF / 650,
	SHF / 651,	SHF / 652,	SHF / 653,	SHF / 654,	SHF / 655,	SHF / 656,	SHF / 657,	SHF / 658,	SHF / 659,	SHF / 660,
	SHF / 661,	SHF / 662,	SHF / 663,	SHF / 664,	SHF / 665,	SHF / 666,	SHF / 667,	SHF / 668,	SHF / 669,	SHF / 670,
	SHF / 671,	SHF / 672,	SHF / 673,	SHF / 674,	SHF / 675,	SHF / 676,	SHF / 677,	SHF / 678,	SHF / 679,	SHF / 680,
	SHF / 681,	SHF / 682,	SHF / 683,	SHF / 684,	SHF / 685,	SHF / 686,	SHF / 687,	SHF / 688,	SHF / 689,	SHF / 690,
	SHF / 691,	SHF / 692,	SHF / 693,	SHF / 694,	SHF / 695,	SHF / 696,	SHF / 697,	SHF / 698,	SHF / 699,	SHF / 700,
	SHF / 701,	SHF / 702,	SHF / 703,	SHF / 704,	SHF / 705,	SHF / 706,	SHF / 707,	SHF / 708,	SHF / 709,	SHF / 710,
	SHF / 711,	SHF / 712,	SHF / 713,	SHF / 714,	SHF / 715,	SHF / 716,	SHF / 717,	SHF / 718,	SHF / 719,	SHF / 720,
	SHF / 721,	SHF / 722,	SHF / 723,	SHF / 724,	SHF / 725,	SHF / 726,	SHF / 727,	SHF / 728,	SHF / 729,	SHF / 730,
	SHF / 731,	SHF / 732,	SHF / 733,	SHF / 734,	SHF / 735,	SHF / 736,	SHF / 737,	SHF / 738,	SHF / 739,	SHF / 740,
	SHF / 741,	SHF / 742,	SHF / 743,	SHF / 744,	SHF / 745,	SHF / 746,	SHF / 747,	SHF / 748,	SHF / 749,	SHF / 750,
	SHF / 751,	SHF / 752,	SHF / 753,	SHF / 754,	SHF / 755,	SHF / 756,	SHF / 757,	SHF / 758,	SHF / 759,	SHF / 760,
	SHF / 761,	SHF / 762,	SHF / 763,	SHF / 764,
};

#endif

typedef struct tagINTENSITYDATA
{
	float       *Tn;
	float       T[NUM_LIGHT_SLOTS][3];
	float       fReverseMap[2][3];

} INTENSITYDATA;

INTENSITYDATA   gIData;
#define TWO_TO_MINUS_10 	 (9.765625E-4f)
#define TWO_TO_MINUS_10_LONG (0x2F5BE6FFl)

/*=======================================================================*/
/*=======================================================================*/
/*======================== Jim's bad method  ============================*/
/*=======================================================================*/
/*=======================================================================*/

int SmoothPlaneDataPrecalc (CONV_SHADING_STRUCT *pShading,
							sgl_vector pt1, sgl_vector pt2, sgl_vector pt3,
							sgl_vector n1, sgl_vector n2, sgl_vector n3)
{
	/* calculate delta normal and optimum Q and R values */

	float   f2x2Det, fD0, fD1, fD2;
	int     nQ, nR;
	float   f1OverQpaRpbMinusQpbRpa;
	int		nOK = TRUE;
	sgl_vector fpa, fpb;
		
	SGL_TIME_START(SMOOTH_DATA_PRECALC_TIME)

	VecSub (pt2, pt1, fpa);
	VecSub (pt3, pt1, fpb);

	/* Ensure these deltas have non-zero length: */
#if DEBUG
	if (DotProd (fpa, fpa) == 0.0f )
	{
		DPF ((DBG_WARNING, "SmoothPlaneDataPrecalc: Dodgy fpa vector. Zero lenght"));
	}
	if (DotProd (fpb, fpb) == 0.0f )
	{
		DPF ((DBG_WARNING, "SmoothPlaneDataPrecalc: Dodgy fpb vector. Zero lenght"));
	}
#endif

	fD0 = (fpa[0] * fpb[1]) - (fpb[0] * fpa[1]);
	fD1 = (fpa[0] * fpb[2]) - (fpb[0] * fpa[2]);
	fD2 = (fpa[1] * fpb[2]) - (fpb[1] * fpa[2]);

	/* see whether triangle shows the biggest profile in x,y x,z or y,z plane */

	if (sfabs (fD0) > sfabs (fD1))
	{
		if (sfabs (fD2) > sfabs (fD0))
		{
			f2x2Det = fD2;
			nQ = 1;
			nR = 2;
		}
		else
		{
			f2x2Det = fD0;
			nQ = 0;
			nR = 1;
		}
	}
	else
	{
		if (sfabs (fD2) > sfabs (fD1))
		{
			f2x2Det = fD2;
			nQ = 1;
			nR = 2;
		}
		else
		{
			f2x2Det = fD1;
			nQ = 0;
			nR = 2;
		}
	}

  #if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)

	/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
	if (sfabs (f2x2Det) < 1.0e-3f)

  #else

	if (sfabs (f2x2Det) < 1.0e-20f)

  #endif
	{
		/* points are too close to be accurate */

		DPF ((DBG_WARNING, "SmoothPlaneDataPrecalc: bad shading data - all points (virtually) colinear (%f)", f2x2Det));

		/* non fatal - plane will be flat shaded, but return an error */

		nOK = FALSE;
	}
	else
	{
		sgl_vector   fN1, fN2, AvN;

		/* copy shading normals to our struct */
		
		VecCopy (n1, pShading->norm1);
		VecCopy (n2, pShading->norm2);
		VecCopy (n3, pShading->norm3);

		/* normalise the smooth shading normals */

		VecNormalise (pShading->norm1);
		VecNormalise (pShading->norm2);
		VecNormalise (pShading->norm3);

		/* get the recip of the determinant for calc below */
		
		f1OverQpaRpbMinusQpbRpa = 1.0f / f2x2Det;

		/*
		   see if we need to reverse the normals' direction - they must point in
		   the opposite direction to the plane's normal
		*/

		AvN[0] = (n1[0] + n2[0] + n3[0]) * 0.333333f;
		AvN[1] = (n1[1] + n2[1] + n3[1]) * 0.333333f;
		AvN[2] = (n1[2] + n2[2] + n3[2]) * 0.333333f;
					
		if (DotProd (AvN, n1) < 0.0f)
		{
			DPF ((DBG_VERBOSE, "SmoothPlaneDataPrecalc: negating smooth shading normals"));

			VecNegate (pShading->norm1);
			VecNegate (pShading->norm2);
			VecNegate (pShading->norm3);
		}
				
		/* work out delta normals per movement of 1 in Q and R */

		VecSub (pShading->norm2, pShading->norm1, fN1);
		VecSub (pShading->norm3, pShading->norm1, fN2);

		pShading->nQ = nQ;
		pShading->nR = nR;

		pShading->f1OverQpaRpbMinusQpbRpa = f1OverQpaRpbMinusQpbRpa;
	}

	SGL_TIME_STOP(SMOOTH_DATA_PRECALC_TIME)
	return (nOK);
}

void SmoothObjectPrecalc (const MASTER_STATE_STRUCT *pState, PSMOOTHPARAMS pSP)
{
	sgl_vector VPOrigin;
	float	(*i)[4] = pState->pTransformState->inv;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	float xPerPixel = pProjMat->xPerPixel;
	float yPerPixel = pProjMat->yPerPixel;
	

	SGL_TIME_START(SMOOTH_OBJ_PRECALC_TIME)
	
		
	/* project a representation of the viewplane into object space */
	/* origin -> centre of projection vector */
	VPOrigin[0] = pProjMat->RCentre[0] - pProjMat->xToCorner;
	VPOrigin[1] = pProjMat->RCentre[1] - pProjMat->yToCorner;
	VPOrigin[2] = pProjMat->RCentre[2];
		
	/* world space origin tranformed to object space */
	pSP->vOrigin[0] = i[0][3];
	pSP->vOrigin[1] = i[1][3];
	pSP->vOrigin[2] = i[2][3];
	
	pSP->vVPOrigin[0] = VPOrigin[0] * i[0][0] + VPOrigin[1] * i[0][1] + VPOrigin[2] * i[0][2];
	pSP->vVPOrigin[1] = VPOrigin[0] * i[1][0] + VPOrigin[1] * i[1][1] + VPOrigin[2] * i[1][2];
	pSP->vVPOrigin[2] = VPOrigin[0] * i[2][0] + VPOrigin[1] * i[2][1] + VPOrigin[2] * i[2][2];

	/* used for scaling pixel vectors ... */
	pSP->f1OverVPZ = 1.0f / VPOrigin[2];

	/* movement of one pixel in x on the viewplane mapped to object space */
	pSP->vVPdX[0] = xPerPixel * i[0][0];
	pSP->vVPdX[1] = xPerPixel * i[1][0];
	pSP->vVPdX[2] = xPerPixel * i[2][0];
	
	/* movement of one pixel in y on the viewplane mapped to object space */
	pSP->vVPdY[0] = yPerPixel * i[0][1];
	pSP->vVPdY[1] = yPerPixel * i[1][1];
	pSP->vVPdY[2] = yPerPixel * i[2][1];

	SGL_TIME_STOP(SMOOTH_OBJ_PRECALC_TIME)
}

void GetSmoothShadingParameters (int numPlanes,
								 TRANSFORMED_PLANE_STRUCT *ptransPlanes[],
								 TRANSFORM_STRUCT *t,
								 MATERIAL_STATE_STRUCT *stateMaterial,
								 LIGHTS_STATE_STRUCT *lightState,
								 SHADING_RESULT_STRUCT *pShadingResults,
								 PSMOOTHPARAMS pSP)
{
	int 					 nLight;
	LIGHT_ENTRY_STRUCT		 *pLightEnt;
	TRANSFORMED_PLANE_STRUCT *pPlane;
	CONV_SHADING_STRUCT		 *pS;
	CONV_POINTS_STRUCT		 *pP;
	int						 nQ, nR;
	float					 fAmbientIntensity;
	float					 *pC0, *pC1;
	float 					 *Dest, *Mtrl, *Glow;
			
	SGL_TIME_START(SMOOTH_PARAM_TIME)

	if (lightState->flags & lsf_dirty_smooth)
	{
		CalcAverageCol (lightState);
	}

	if (lightState->flags & lsf_dirty_position)
	{
		float *pW, *pO;

		pLightEnt = lightState->light_entries;

		/*
		// Step through all the ON parallel lights first
		*/
   		for (nLight = lightState->numOnParLights; nLight != 0; 
												  nLight --, pLightEnt++)
	   	{
	   		ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==parallel_light_type);
			
				
			pW = pLightEnt->direction;
			pO = pLightEnt->local_direction;

			pO[0] = pW[0] * t->inv[0][0] + pW[1] * t->inv[0][1] + 
												 pW[2] * t->inv[0][2];
			pO[1] = pW[0] * t->inv[1][0] + pW[1] * t->inv[1][1] + 
												 pW[2] * t->inv[1][2];
			pO[2] = pW[0] * t->inv[2][0] + pW[1] * t->inv[2][1] + 
												 pW[2] * t->inv[2][2];

			VecNormalise (pO);
		}/*end for*/

		/*
		// Step through all the ON point lights
		*/
   		for (nLight = lightState->numOnPntLights; nLight != 0; 
												  nLight --, pLightEnt++)
	   	{
	   		ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==point_light_type);

			pW = pLightEnt->position;
			pO = pLightEnt->local_position;

			pO[0] = pW[0] * t->inv[0][0] + pW[1] * t->inv[0][1] +
									 pW[2] * t->inv[0][2] + t->inv[0][3];

			pO[1] = pW[0] * t->inv[1][0] + pW[1] * t->inv[1][1] +
									 pW[2] * t->inv[1][2] + t->inv[1][3];

			pO[2] = pW[0] * t->inv[2][0] + pW[1] * t->inv[2][1] + 
									pW[2] * t->inv[2][2] + t->inv[2][3];
		}/*end for*/

		lightState->flags &= ~lsf_dirty_position;
	}
		
	Glow = stateMaterial->glow;
	fAmbientIntensity = lightState->ambient_smooth_intensity +
	                    (Glow[0] + Glow[1] + Glow[2])*0.333333f;

	for (/*Nil*/; numPlanes!=0; numPlanes--, ptransPlanes++, pShadingResults++)
	{
		float	*vNormal, *vRepPt, *vProjRepPt;
		float	fIRepPt[NUM_LIGHT_SLOTS];
		float	fIa[NUM_LIGHT_SLOTS];
		float	fIb[NUM_LIGHT_SLOTS];
		int 	nSlot;
		float   fQpa, fRpa, fQpb, fRpb;
		float   fRepDeltaQ, fRepDeltaR;
		float	fIaLocal, fIbLocal, fIRepPtLocal;
				
		pPlane = *ptransPlanes;

		pS = (CONV_SHADING_STRUCT *) pPlane->pShadingData;
		pP = (CONV_POINTS_STRUCT *) pPlane->pPointsData;

		nQ = pS->nQ;
		nR = pS->nR;

		vRepPt = (float *) pPlane->pOriginalData->rep_point;

		/* 
		** Use the Off Screen Rep Point for Smooth Shading 
		** This routine clips the Pt to the H/W limits
		** and moves the origin of the Intensity plane eqn
		** for an off screen Rep Point. The following Flag 
		** is set if the RepPt is not the projected world 
		** space RepPt. Re: RnProjDifficultRepPoint()
		*/
	    if ( ! pPlane->bNewProjRepPoint )
		{
			vProjRepPt = pPlane->projRepPoint;
		}
		else
		{
			vProjRepPt = pPlane->vNewProjRepPoint;
		}

		fRepDeltaQ = vRepPt[nQ] - pP->pt1[nQ];
		fRepDeltaR = vRepPt[nR] - pP->pt1[nR];

		vNormal = (float *) pPlane->pOriginalData->normal;

		fQpa = pP->pt2_delta[nQ];
		fRpa = pP->pt2_delta[nR];
		fQpb = pP->pt3_delta[nQ];
		fRpb = pP->pt3_delta[nR];

	

	    fIRepPt[0] = 	fAmbientIntensity;
	    fIa[0] = 		fAmbientIntensity;
	    fIb[0] = 		fAmbientIntensity;

	    fIRepPt[1] = 	0.0f;
	    fIa[1] = 		0.0F;
	    fIb[1] = 		0.0F;

		/* find the reverse mapped x/y pixel vectors */
		{
			#if 1

				sgl_vector	D1, D2, ovp_raydir, ovp_raypos[2];
				float 		f1OverWDotV;
				float 		fPMinusQDotVOverWDotV;
				float		fZOverVPZ;
					
				ovp_raydir[0] = (vProjRepPt[0] * pSP->vVPdX[0]) + 
								(vProjRepPt[1] * pSP->vVPdY[0]) + pSP->vVPOrigin[0];

				ovp_raydir[1] = (vProjRepPt[0] * pSP->vVPdX[1]) + 
								(vProjRepPt[1] * pSP->vVPdY[1]) + pSP->vVPOrigin[1];

				ovp_raydir[2] = (vProjRepPt[0] * pSP->vVPdX[2]) + 
								(vProjRepPt[1] * pSP->vVPdY[2]) + pSP->vVPOrigin[2];

				fZOverVPZ = pPlane->repPnt[2] * pSP->f1OverVPZ;
				
				ovp_raypos[0][0] = pSP->vOrigin[0] + (pSP->vVPdX[0] * fZOverVPZ); 
				ovp_raypos[0][1] = pSP->vOrigin[1] + (pSP->vVPdX[1] * fZOverVPZ);
				ovp_raypos[0][2] = pSP->vOrigin[2] + (pSP->vVPdX[2] * fZOverVPZ);
		
				ovp_raypos[1][0] = pSP->vOrigin[0] + (pSP->vVPdY[0] * fZOverVPZ);
				ovp_raypos[1][1] = pSP->vOrigin[1] + (pSP->vVPdY[1] * fZOverVPZ);
				ovp_raypos[1][2] = pSP->vOrigin[2] + (pSP->vVPdY[2] * fZOverVPZ);
	
				f1OverWDotV = 1.0f / DotProd (ovp_raydir, vNormal);
			
				VecSub (vRepPt, ovp_raypos[0], D1);
				VecSub (vRepPt, ovp_raypos[1], D2);

				fPMinusQDotVOverWDotV = DotProd (D1, vNormal) * f1OverWDotV;
			
				gIData.fReverseMap[0][nQ] = (ovp_raypos[0][nQ] + 
							(fPMinusQDotVOverWDotV * ovp_raydir[nQ])) - vRepPt[nQ];

				gIData.fReverseMap[0][nR] = (ovp_raypos[0][nR] + 
							(fPMinusQDotVOverWDotV * ovp_raydir[nR])) - vRepPt[nR];

			
				fPMinusQDotVOverWDotV = DotProd (D2, vNormal) * f1OverWDotV;

			
				gIData.fReverseMap[1][nQ] = (ovp_raypos[1][nQ] + 
							(fPMinusQDotVOverWDotV * ovp_raydir[nQ])) - vRepPt[nQ];

				gIData.fReverseMap[1][nR] = (ovp_raypos[1][nR] + 
							(fPMinusQDotVOverWDotV * ovp_raydir[nR])) - vRepPt[nR];

			#else

				sgl_vector	D1, D2, ovp_raydir, ovp_raypos[2];
				float 		f1OverWDotV;
				float 		fPMinusQDotVOverWDotV;
				float		fZOverVPZ;
					
				ovp_raydir[0] =  pSP->vVPOrigin[0];
				ovp_raydir[1] =  pSP->vVPOrigin[1];
				ovp_raydir[2] =  pSP->vVPOrigin[2];

				fZOverVPZ = pPlane->repPnt[2] * pSP->f1OverVPZ;
				
				f1OverWDotV = 1.0f / DotProd (ovp_raydir, vNormal);
			
				D1[0] = pSP->vVPdX[0] * fZOverVPZ;
				D1[1] = pSP->vVPdX[1] * fZOverVPZ;
				D1[2] = pSP->vVPdX[2] * fZOverVPZ;

				D2[0] = pSP->vVPdY[0] * fZOverVPZ;
				D2[1] = pSP->vVPdY[1] * fZOverVPZ;
				D2[2] = pSP->vVPdY[2] * fZOverVPZ;

				fPMinusQDotVOverWDotV = - DotProd (D1, vNormal) * f1OverWDotV;
			
				gIData.fReverseMap[0][nQ] = D1[nQ] + (fPMinusQDotVOverWDotV * ovp_raydir[nQ]);
				gIData.fReverseMap[0][nR] = D1[nR] + (fPMinusQDotVOverWDotV * ovp_raydir[nR]);
			
				fPMinusQDotVOverWDotV = - DotProd (D2, vNormal) * f1OverWDotV;
			
				gIData.fReverseMap[1][nQ] = D2[nQ] + (fPMinusQDotVOverWDotV * ovp_raydir[nQ]);
				gIData.fReverseMap[1][nR] = D2[nR] + (fPMinusQDotVOverWDotV * ovp_raydir[nR]);

			#endif

		}

		/*
		// Get a pointer to the first light entry in the current lights
		*/
		pLightEnt = lightState->light_entries;

		/*
		// Step through the switched on parallel lights
		*/
   		for (nLight = lightState->numOnParLights; nLight !=0 ; 
														 nLight--, pLightEnt++)
	   	{
			float	*L, fIntensity;

			nSlot = 		pLightEnt->light_colour_slot;
			fIntensity = 	pLightEnt->smooth_intensity;


	   		ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==parallel_light_type);


			L = pLightEnt->local_direction;

			/* 
			// light direction vector needs to be reversed so
			// negate dot products 
			*/
					
			if (pPlane->flags & pf_reversed)
			{
				fIRepPtLocal = -DotProd (L, pS->norm1);
				fIaLocal = -DotProd (L, pS->norm2);
				fIbLocal = -DotProd (L, pS->norm3);
			}
			else
			{
				fIRepPtLocal = DotProd (L, pS->norm1);
				fIaLocal = DotProd (L, pS->norm2);
				fIbLocal = DotProd (L, pS->norm3);
			}

#if SLOW_FCMP
			CHOOSE_FLOAT_MAX (fIRepPtLocal, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fIaLocal, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fIbLocal, 0L, 0.0f);
#else			
			CHOOSE_MAX (fIRepPtLocal, 0);
			CHOOSE_MAX (fIaLocal, 0);
			CHOOSE_MAX (fIbLocal, 0);
#endif

			fIa[nSlot] += fIaLocal * fIntensity;
			fIb[nSlot] += fIbLocal * fIntensity;
			fIRepPt[nSlot] += fIRepPtLocal * fIntensity;
		}/*end for parallel lights*/

		/*
		// Step through the ON Point lights
		*/
   		for (nLight = lightState->numOnPntLights; nLight !=0 ; 
												  nLight--, pLightEnt++)
	   	{
			float	*L, fIntensity;
			sgl_vector	vR, vA, vB;

			ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==point_light_type);

			nSlot = 		pLightEnt->light_colour_slot;
			fIntensity = 	pLightEnt->smooth_intensity;
					
			L = pLightEnt->local_position;

			VecSub (L, vRepPt, vR);
			VecAdd (vRepPt, pP->pt2_delta, vA);
			VecSub (L, vA, vA);
			VecAdd (vRepPt, pP->pt3_delta, vB);
			VecSub (L, vB, vB);

			VecNormalise (vR);
			VecNormalise (vA);
			VecNormalise (vB);
					
			if (pPlane->flags & pf_reversed)
			{
				fIaLocal = -DotProd (vA, pS->norm2);
				fIbLocal = -DotProd (vB, pS->norm3);
				fIRepPtLocal = -DotProd (vR, pS->norm1);
			}
			else
			{
				fIaLocal = DotProd (vA, pS->norm2);
				fIbLocal = DotProd (vB, pS->norm3);
				fIRepPtLocal = DotProd (vR, pS->norm1);
			}

#if SLOW_FCMP
			CHOOSE_FLOAT_MAX (fIRepPtLocal, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fIaLocal, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fIbLocal, 0L, 0.0f);
#else
			CHOOSE_MAX (fIRepPtLocal, 0);
			CHOOSE_MAX (fIaLocal, 0);
			CHOOSE_MAX (fIbLocal, 0);
#endif

			fIa[nSlot] += fIaLocal * fIntensity;
			fIb[nSlot] += fIbLocal * fIntensity;
			fIRepPt[nSlot] += fIRepPtLocal * fIntensity;

		}/*end for point lights*/


		for (nSlot = 0; nSlot < NUM_LIGHT_SLOTS; ++nSlot)
		{
			fIaLocal = fIa[nSlot];
			fIbLocal = fIb[nSlot];
			fIRepPtLocal = fIRepPt[nSlot];

#if SLOW_FCMP
			if ((FLOAT_TO_LONG(fIRepPtLocal) > 0) || 
				(FLOAT_TO_LONG(fIaLocal) > 0) || 
				(FLOAT_TO_LONG(fIbLocal) > 0) )
#else
			if ((fIRepPtLocal > 0.0f) || (fIaLocal > 0.0f) || (fIbLocal > 0.0f))
#endif
			{
				float 	fFT1, fFT2, fT2, fT1;

				/* attempt to stop 'fringing' around the terminator */
#if SLOW_FCMP
				CHOOSE_FLOAT_MIN (fIRepPtLocal, 0x3FFEB852L, 1.99f);
				CHOOSE_FLOAT_MIN (fIaLocal, 0x3FFEB852L, 1.99f);
				CHOOSE_FLOAT_MIN (fIbLocal, 0x3FFEB852L, 1.99f);
#else
				CHOOSE_MIN (fIRepPtLocal, 1.99f);
				CHOOSE_MIN (fIaLocal, 1.99f);
				CHOOSE_MIN (fIbLocal, 1.99f);
#endif

#if 0
				CHOOSE_MAX (fIRepPtLocal, 0);
				CHOOSE_MAX (fIaLocal, 0);
				CHOOSE_MAX (fIbLocal, 0);
#endif

				fIaLocal -= fIRepPtLocal;
				fIbLocal -= fIRepPtLocal;

				/* solve for a ... */
				
				fT2 = ((fIaLocal * fRpb) - (fIbLocal * fRpa)) * pS->f1OverQpaRpbMinusQpbRpa;
		
				/* solve for b ... */
		
				fT1 = ((fIbLocal * fQpa) - (fIaLocal * fQpb)) * pS->f1OverQpaRpbMinusQpbRpa;

				/*
				// rep point may have moved! Work out the deltas and use the
				// value we have just computed to bodge up the new fIRepPt
				// value (intensity at NEW rep pt)
				*/

				fIRepPtLocal += (fT2 * fRepDeltaQ) + (fT1 * fRepDeltaR); 

				fFT1 = (fT2 * gIData.fReverseMap[1][nQ]) + (fT1 * gIData.fReverseMap[1][nR]);
				fFT2 = (fT2 * gIData.fReverseMap[0][nQ]) + (fT1 * gIData.fReverseMap[0][nR]);
				
				if ((sfabs (fFT1) > 0.25f) || (sfabs (fFT2) > 0.25f))
				{
					gIData.T[nSlot][0] = (fIRepPtLocal + fIaLocal + fIbLocal) * 0.333f;
					gIData.T[nSlot][1] = 0.0f;
					gIData.T[nSlot][2] = 0.0f;

#if SLOW_FCMP
					CHOOSE_FLOAT_MIN (gIData.T[nSlot][0], 0x3FFEB852L, 1.99f);
#else
					CHOOSE_MIN (gIData.T[nSlot][0], 1.99f);
#endif
				}
				else
				{
					gIData.T[nSlot][0] = fIRepPtLocal;
					gIData.T[nSlot][1] = fFT1;
					gIData.T[nSlot][2] = fFT2;
				}
			}
			else
			{
				gIData.T[nSlot][0] = 0.0f;
				gIData.T[nSlot][1] = 0.0f;
				gIData.T[nSlot][2] = 0.0f;
			}
		}

		/* adjust for offscreen rep point if necessary */
#if SLOW_FCMP	
		if ( FLOAT_TO_LONG(vProjRepPt[0]) < 0)
#else
		if (vProjRepPt[0] < 0.0f)
#endif
		{
			gIData.T[0][0] += (-vProjRepPt[0] * gIData.T[0][2]);
			gIData.T[1][0] += (-vProjRepPt[0] * gIData.T[1][2]);
			pShadingResults->slot[0].smooth.ShadingRepPt[0] = 0.0f;
		}
		else if (vProjRepPt[0] > 1023.0f)
		{
			gIData.T[0][0] -= ((vProjRepPt[0] - 1023.0f) * gIData.T[0][2]);
			gIData.T[1][0] -= ((vProjRepPt[0] - 1023.0f) * gIData.T[1][2]);
			pShadingResults->slot[0].smooth.ShadingRepPt[0] = 1023.0f;
		}
		else
		{
			pShadingResults->slot[0].smooth.ShadingRepPt[0] = vProjRepPt[0];
		}

#if SLOW_FCMP
		if (FLOAT_TO_LONG(vProjRepPt[1]) < 0)
#else
		if (vProjRepPt[1] < 0.0f)
#endif

		{
			gIData.T[0][0] += (-vProjRepPt[1] * gIData.T[0][1]);
			gIData.T[1][0] += (-vProjRepPt[1] * gIData.T[1][1]);
			pShadingResults->slot[0].smooth.ShadingRepPt[1] = 0.0f;
		}
		else if (vProjRepPt[1] > 1023.0f)
		{
			gIData.T[0][0] -= ((vProjRepPt[1] - 1023.0f) * gIData.T[0][1]);
			gIData.T[1][0] -= ((vProjRepPt[1] - 1023.0f) * gIData.T[1][1]);
			pShadingResults->slot[0].smooth.ShadingRepPt[1] = 1023.0f;
		}
		else
		{
			pShadingResults->slot[0].smooth.ShadingRepPt[1] = vProjRepPt[1];
		}

	    /* Non shadowed Smooth Shading Parameters */

	    pShadingResults->slot[0].smooth.t0 = (int)(gIData.T[0][0] * 0x4000);   	
	    pShadingResults->slot[0].smooth.t1 = (int)(gIData.T[0][1] * 0x10000);
	    pShadingResults->slot[0].smooth.t2 = (int)(gIData.T[0][2] * 0x10000);

	    /* Shadow Light Parameters Smooth */
	    	
	    pShadingResults->slot[1].smooth.t0 = (int)(gIData.T[1][0] * 0x4000);    	
		pShadingResults->slot[1].smooth.t1 = (int)(gIData.T[1][1] * 0x10000);
	    pShadingResults->slot[1].smooth.t2 = (int)(gIData.T[1][2] * 0x10000);
		
		/* base colours for shading */
		
		Mtrl = stateMaterial->diffuse;
		/*Glow = stateMaterial->glow;*/
		Dest = pShadingResults->slot[0].smooth.rgbColour;

		pC0 = lightState->light_slots[0].colour;

		Dest[0] = Glow[0] + Mtrl[0]*pC0[0];
		Dest[1] = Glow[1] + Mtrl[1]*pC0[1];
		Dest[2] = Glow[2] + Mtrl[2]*pC0[2];

#if SLOW_FCMP
		CHOOSE_FLOAT_MIN (Dest[0], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[1], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[2], 0x3F800000L, 1.0f);
#else
		CHOOSE_MIN (Dest[0], 1.0f);
		CHOOSE_MIN (Dest[1], 1.0f);
		CHOOSE_MIN (Dest[2], 1.0f);
#endif

		Dest = pShadingResults->slot[1].smooth.rgbColour;

		pC1 = lightState->light_slots[1].colour;

		Dest[0] = Glow[0] + Mtrl[0]*pC1[0];
		Dest[1] = Glow[1] + Mtrl[1]*pC1[1];
		Dest[2] = Glow[2] + Mtrl[2]*pC1[2];

#if SLOW_FCMP
		CHOOSE_FLOAT_MIN (Dest[0], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[1], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[2], 0x3F800000L, 1.0f);
#else
		CHOOSE_MIN (Dest[0], 1.0f);
		CHOOSE_MIN (Dest[1], 1.0f);
		CHOOSE_MIN (Dest[2], 1.0f);
#endif
	}

	SGL_TIME_STOP(SMOOTH_PARAM_TIME)
}

/*=======================================================================*/
/*=======================================================================*/
/*==================== Adjoint calculation method =======================*/
/*=======================================================================*/
/*=======================================================================*/

void GetSmoothShadingParametersAdjoint (int numPlanes,
								 TRANSFORMED_PLANE_STRUCT *ptransPlanes[],
								 TRANSFORM_STRUCT *t,
								 MATERIAL_STATE_STRUCT *stateMaterial,
								 LIGHTS_STATE_STRUCT *lightState,
								 SHADING_RESULT_STRUCT *pShadingResults,
								 PSMOOTHPARAMS pSP)
{
	int 					 nLight;
	LIGHT_ENTRY_STRUCT		 *pLightEnt;
	TRANSFORMED_PLANE_STRUCT *pPlane;
	CONV_SHADING_STRUCT		 *pS;
	CONV_POINTS_STRUCT		 *pP;
	float					 fAmbientIntensity;
	float					 *pC0, *pC1;
	float 					 *Dest, *Mtrl, *Glow;
			
	SGL_TIME_START(SMOOTH_ADJ_PARAM_TIME)

	if (lightState->flags & lsf_dirty_smooth)
	{
		CalcAverageCol (lightState);
	}

	if (lightState->flags & lsf_dirty_position)
	{
		float *pW, *pO;

		pLightEnt = lightState->light_entries;

		/*
		// Step through all the ON parallel lights first
		*/
   		for (nLight = lightState->numOnParLights; nLight != 0; 
												  nLight --, pLightEnt++)
	   	{
	   		ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==parallel_light_type);
				
			pW = pLightEnt->direction;
			pO = pLightEnt->local_direction;
	
			#define I(x,y) t->inv[x][y]

			pO[0] = pW[0]*I(0,0) + pW[1]*I(0,1) + pW[2]*I(0,2);
			pO[1] = pW[0]*I(1,0) + pW[1]*I(1,1) + pW[2]*I(1,2);
			pO[2] = pW[0]*I(2,0) + pW[1]*I(2,1) + pW[2]*I(2,2);
	
			#undef I

			VecNormalise (pO);
		}/*end for*/

		/*
		// Step through all the ON point lights
		*/
   		for (nLight = lightState->numOnPntLights; nLight != 0; 
												  nLight --, pLightEnt++)
	   	{
	   		ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==point_light_type);

			pW = pLightEnt->position;
			pO = pLightEnt->local_position;

			#define I(x,y) t->inv[x][y]

			pO[0] = pW[0]*I(0,0) + pW[1]*I(0,1) + pW[2]*I(0,2) + I(0,3);
			pO[1] = pW[0]*I(1,0) + pW[1]*I(1,1) + pW[2]*I(1,2) + I(1,3);
			pO[2] = pW[0]*I(2,0) + pW[1]*I(2,1) + pW[2]*I(2,2) + I(2,3);
	
			#undef I

		}/*end for*/

		lightState->flags &= ~lsf_dirty_position;
	}
		
	Glow = stateMaterial->glow;
	fAmbientIntensity = lightState->ambient_smooth_intensity +
	                    (Glow[0] + Glow[1] + Glow[2])*0.333333f;

	for (/*Nil*/; numPlanes!=0; numPlanes--, ptransPlanes++, pShadingResults++)
	{
		float	*vProjRepPt;
		float	fI0[NUM_LIGHT_SLOTS];
		float	fI1[NUM_LIGHT_SLOTS];
		float	fI2[NUM_LIGHT_SLOTS];
		int 	nSlot;
		float	fI1Local, fI2Local, fI0Local;
		float	fRepX, fRepY, fRepDX, fRepDY;
				
		pPlane = *ptransPlanes;

		pS = (CONV_SHADING_STRUCT *) pPlane->pShadingData;
		pP = (CONV_POINTS_STRUCT *) pPlane->pPointsData;
		vProjRepPt = pPlane->projRepPoint;

	    fI0[0] = fAmbientIntensity;
	    fI1[0] = fAmbientIntensity;
	    fI2[0] = fAmbientIntensity;

	    fI0[1] = 0.0f;
	    fI1[1] = 0.0F;
	    fI2[1] = 0.0F;

		/*
		// Get a pointer to the first light entry in the current lights
		*/
		pLightEnt = lightState->light_entries;

		/*
		// Step through the switched on parallel lights
		*/
   		for (nLight = lightState->numOnParLights; nLight !=0 ; 
														 nLight--, pLightEnt++)
	   	{
			float	*L, fIntensity;

			nSlot = 		pLightEnt->light_colour_slot;
			fIntensity = 	pLightEnt->smooth_intensity;

	   		ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==parallel_light_type);

			L = pLightEnt->local_direction;

			/* 
			// light direction vector needs to be reversed so
			// negate dot products 
			*/
					
			if (pPlane->flags & pf_reversed)
			{
				fI0Local = -DotProd (L, pS->norm1);
				fI1Local = -DotProd (L, pS->norm2);
				fI2Local = -DotProd (L, pS->norm3);
			}
			else
			{
				fI0Local = DotProd (L, pS->norm1);
				fI1Local = DotProd (L, pS->norm2);
				fI2Local = DotProd (L, pS->norm3);
			}

#if SLOW_FCMP
			CHOOSE_FLOAT_MAX (fI0Local, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fI1Local, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fI2Local, 0L, 0.0f);
#else
			CHOOSE_MAX (fI0Local, 0);
			CHOOSE_MAX (fI1Local, 0);
			CHOOSE_MAX (fI2Local, 0);
#endif

			fI0[nSlot] += fI0Local * fIntensity;
			fI1[nSlot] += fI1Local * fIntensity;
			fI2[nSlot] += fI2Local * fIntensity;

		}/*end for parallel lights*/

		/*
		// Step through the ON Point lights
		*/
   		for (nLight = lightState->numOnPntLights; nLight !=0 ; 
												  nLight--, pLightEnt++)
	   	{
			float	*L, fIntensity;
			sgl_vector	vR, vA, vB;

			ASSERT(pLightEnt->light_flags & light_on);
			ASSERT((pLightEnt->light_flags&mask_light_types)==point_light_type);

			nSlot = 		pLightEnt->light_colour_slot;
			fIntensity = 	pLightEnt->smooth_intensity;
					
			L = pLightEnt->local_position;

			VecSub (L, pP->pt1, vR);
			VecAdd (pP->pt1, pP->pt2_delta, vA);
			VecSub (L, vA, vA);
			VecAdd (pP->pt1, pP->pt3_delta, vB);
			VecSub (L, vB, vB);

			VecNormalise (vR);
			VecNormalise (vA);
			VecNormalise (vB);
					
			if (pPlane->flags & pf_reversed)
			{
				fI0Local = -DotProd (vR, pS->norm1);
				fI1Local = -DotProd (vA, pS->norm2);
				fI2Local = -DotProd (vB, pS->norm3);
			}
			else
			{
				fI0Local = DotProd (vR, pS->norm1);
				fI1Local = DotProd (vA, pS->norm2);
				fI2Local = DotProd (vB, pS->norm3);
			}

#if SLOW_FCMP
			CHOOSE_FLOAT_MAX (fI0Local, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fI1Local, 0L, 0.0f);
			CHOOSE_FLOAT_MAX (fI2Local, 0L, 0.0f);
#else
			CHOOSE_MAX (fI0Local, 0);
			CHOOSE_MAX (fI1Local, 0);
			CHOOSE_MAX (fI2Local, 0);
#endif

			fI0[nSlot] += fI0Local * fIntensity;
			fI1[nSlot] += fI1Local * fIntensity;
			fI2[nSlot] += fI2Local * fIntensity;

		}/*end for point lights*/

		for (nSlot = 0; nSlot < NUM_LIGHT_SLOTS; ++nSlot)
		{
			fI0Local = fI0[nSlot];
			fI1Local = fI1[nSlot];
			fI2Local = fI2[nSlot];
#if SLOW_FCMP
			if ((FLOAT_TO_LONG(fI0Local) > 0) || 
			    (FLOAT_TO_LONG(fI1Local) > 0) || 
			    (FLOAT_TO_LONG(fI2Local) > 0) )
#else
			if ((fI0Local > 0.0f) || (fI1Local > 0.0f) || (fI2Local > 0.0f))
#endif
			{
				float 	fT2, fT1;

				/* attempt to stop 'fringing' around the terminator */
#if SLOW_FCMP
				CHOOSE_FLOAT_MIN (fI0Local, 0x3FFEB852L, 1.99f);
				CHOOSE_FLOAT_MIN (fI1Local, 0x3FFEB852L, 1.99f);
				CHOOSE_FLOAT_MIN (fI2Local, 0x3FFEB852L, 1.99f);
#else
				CHOOSE_MIN (fI0Local, 1.99f);
				CHOOSE_MIN (fI1Local, 1.99f);
				CHOOSE_MIN (fI2Local, 1.99f);
#endif

				#define ADJ(x,y)	pS->fAdjoint[x][y]

				/* pre-multiply intensities by inverse of face matrix */

				fT2 = (ADJ(0,0)*fI0Local + 
					   ADJ(0,1)*fI1Local + 
					   ADJ(0,2)*fI2Local) * pS->f1OverDet;
				fT1 = (ADJ(1,0)*fI0Local + 
					   ADJ(1,1)*fI1Local + 
					   ADJ(1,2)*fI2Local) * pS->f1OverDet;

				#undef ADJ

				if ((sfabs (fT1) > 0.25f) || (sfabs (fT2) > 0.25f))
				{
					gIData.T[nSlot][0] = (fI0Local + fI1Local + fI2Local) * 0.333f;
					gIData.T[nSlot][1] = 0.0f;
					gIData.T[nSlot][2] = 0.0f;
#if SLOW_FCMP
					CHOOSE_FLOAT_MIN (gIData.T[nSlot][0], 0x3FFEB852L, 1.99f);
#else
					CHOOSE_MIN (gIData.T[nSlot][0], 1.99f);
#endif
				}
				else
				{
					gIData.T[nSlot][0] = fI0Local;
					gIData.T[nSlot][1] = fT1;
					gIData.T[nSlot][2] = fT2;
				}
			}
			else
			{
				gIData.T[nSlot][0] = 0.0f;
				gIData.T[nSlot][1] = 0.0f;
				gIData.T[nSlot][2] = 0.0f;
			}
		}

		/* adjust for offscreen rep point if necessary */

		fRepDX = pS->fRepDeltaX;
		fRepDY = pS->fRepDeltaY;

		fRepX = vProjRepPt[0];
		fRepY = vProjRepPt[1];

		if (sfabs (fRepX - 512.0f) > 512.0f)
		{
#if SLOW_FCMP
			if ( FLOAT_TO_LONG(fRepX) < 0)
#else
			if (fRepX < 0.0f)
#endif
			{
				fRepDX -= fRepX;
				fRepX = 0.0f;
			}
			else
			{
				fRepDX -= fRepX - 1023.0f;
				fRepX = 1023.0f;
			}
		}

		if (sfabs (fRepY - 512.0f) > 512.0f)
		{
#if SLOW_FCMP
			if ( FLOAT_TO_LONG(fRepY) < 0)
#else
			if (fRepY < 0.0f)
#endif
			{
				fRepDY -= fRepY;
				fRepY = 0.0f;
			}
			else
			{
				fRepDY -= fRepY - 1023.0f;
				fRepY = 1023.0f;
			}
		}

		gIData.T[0][0] += fRepDX*gIData.T[0][2] + fRepDY*gIData.T[0][1];
		gIData.T[1][0] += fRepDX*gIData.T[1][2] + fRepDY*gIData.T[1][1];

		pShadingResults->slot[0].smooth.ShadingRepPt[0] = fRepX;
		pShadingResults->slot[0].smooth.ShadingRepPt[1] = fRepY;

	    /* Non shadowed Smooth Shading Parameters */

	    pShadingResults->slot[0].smooth.t0 = (int)(gIData.T[0][0] * 0x4000);   	
	    pShadingResults->slot[0].smooth.t1 = (int)(gIData.T[0][1] * 0x10000);
	    pShadingResults->slot[0].smooth.t2 = (int)(gIData.T[0][2] * 0x10000);

	    /* Shadow Light Parameters Smooth */
	    	
	    pShadingResults->slot[1].smooth.t0 = (int)(gIData.T[1][0] * 0x4000);    	
		pShadingResults->slot[1].smooth.t1 = (int)(gIData.T[1][1] * 0x10000);
	    pShadingResults->slot[1].smooth.t2 = (int)(gIData.T[1][2] * 0x10000);
		
		/* base colours for shading */
		
		Mtrl = stateMaterial->diffuse;
		/*Glow = stateMaterial->glow;*/
		Dest = pShadingResults->slot[0].smooth.rgbColour;

		pC0 = lightState->light_slots[0].colour;

		Dest[0] = Glow[0] + Mtrl[0]*pC0[0];
		Dest[1] = Glow[1] + Mtrl[1]*pC0[1];
		Dest[2] = Glow[2] + Mtrl[2]*pC0[2];

#if SLOW_FCMP
		CHOOSE_FLOAT_MIN (Dest[0], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[1], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[2], 0x3F800000L, 1.0f);
#else
		CHOOSE_MIN (Dest[0], 1.0f);
		CHOOSE_MIN (Dest[1], 1.0f);
		CHOOSE_MIN (Dest[2], 1.0f);
#endif

		Dest = pShadingResults->slot[1].smooth.rgbColour;

		pC1 = lightState->light_slots[1].colour;

		Dest[0] = Glow[0] + Mtrl[0]*pC1[0];
		Dest[1] = Glow[1] + Mtrl[1]*pC1[1];
		Dest[2] = Glow[2] + Mtrl[2]*pC1[2];

#if SLOW_FCMP
		CHOOSE_FLOAT_MIN (Dest[0], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[1], 0x3F800000L, 1.0f);
		CHOOSE_FLOAT_MIN (Dest[2], 0x3F800000L, 1.0f);
#else
		CHOOSE_MIN (Dest[0], 1.0f);
		CHOOSE_MIN (Dest[1], 1.0f);
		CHOOSE_MIN (Dest[2], 1.0f);
#endif
	}

	SGL_TIME_STOP(SMOOTH_ADJ_PARAM_TIME)
}

/* end of $RCSfile: rnshade.c,v $ */


