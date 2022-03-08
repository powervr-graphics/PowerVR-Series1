/******************************************************************************
 * Name         : ldbmp.c
 * Title        : loads a bmp into an intermediate map..
 * Author       : Peter Leaback
 * Created      : 29/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: ldbmp.c,v $
 * Revision 1.29  1997/10/16  11:21:52  sxy
 * deliberatly over wrote previous version, this is why there is a missing version number.
 *
 * Revision 1.27  1997/09/29  09:33:40  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.26  1997/06/11  16:53:27  sxy
 * changed all generate_mipmap from type sgl_bool into sgl_mipmap_generation_options.
 *
 * Revision 1.25  1997/06/05  08:42:23  erf
 * Fixed compiler warning.
 *
 * Revision 1.24  1997/06/02  02:45:31  jop
 * Sanitised comments
 *
 * Revision 1.23  1997/05/13  17:54:16  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.22  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.21  1997/04/30  19:19:27  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.20  1997/04/29  14:46:51  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.19  1997/04/04  13:03:43  jop
 * Fixes to get rid of some annoying dependancies
 *
 * Revision 1.18  1997/04/02  21:28:43  mjg
 * Updated calling conventions.
 *
 * Revision 1.17  1997/03/31  22:08:09  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.16  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.15  1996/07/19  11:19:14  msh
 * fseek doesn't work with MIDAS Arcade so alternative implemented
 *
 * Revision 1.14  1996/03/14  13:42:31  sjf
 * changed opacity field to alpha (ie new name).
 *
 * Revision 1.13  1996/02/19  15:51:41  sjf
 * Curse: In the previous version I had an out-of-date list.h
 *
 * Revision 1.12  1996/02/19  11:27:09  sjf
 * Changed way in which files are loaded so that
 * a) It handles rows which aren't multiples of 4 in length (see bug database)
 * b) Reads in a whole row at a time, using fread, which should make the reading
 *    much faster.
 *
 * Revision 1.11  1996/02/09  16:23:20  jop
 * Moved to new list way of doing things
 *
 * Revision 1.10  1996/01/25  09:55:16  pdl
 * removed the include of sgl.h for reasons that will become clear
 * in the fullness of time.
 *
 * Revision 1.9  1995/10/24  18:38:35  sjf
 * fixed mip map genration.
 *
 * Revision 1.8  1995/08/23  12:28:41  sjf
 * Fixed error message.
 *
 * Revision 1.7  1995/08/21  16:45:44  sjf
 * Whoops, was trying to open a dud file for translucency.
 *
 * Revision 1.6  1995/08/21  12:25:01  jop
 * corrected win32 nonsense
 *
 * Revision 1.5  1995/08/19  09:09:57  sjf
 * Changed bmp map loading so it could handle paths (irrespective of OP sys)
 * and still support the automatic loading of transparency.
 *
 * Revision 1.4  1995/08/03  12:53:05  pdl
 * changed the endian processing..
 *
 * Revision 1.3  1995/08/02  20:35:59  jop
 * Did a little debugging
 *
 * Revision 1.2  1995/08/02  14:41:54  jop
 * Added higher level i/f for loading BMPs as textures, including caching.
 *
 * Revision 1.1  1995/07/30  20:01:20  pdl
 * Initial revision
 *
 *****************************************************************************/

#define MODULE_ID	MODID_LDBMP

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sgl_defs.h"
#include "list.h"
#include "pvrosapi.h"
#include "sglmem.h"

#define DOS_SEP  '\\'
#define UNIX_SEP '/'

#if WIN32 || DOS32
#define DIR_SEP  DOS_SEP 
#else
#define DIR_SEP  UNIX_SEP
#endif

/*========================================================================================*/
/*========================================================================================*/
/*										STRUCTURES										  */
/*========================================================================================*/
/*========================================================================================*/

typedef struct tagTEXCACHEENTRY				/* structure stord in cache list */
{
	int		nTextureName;					/* name returned by sgl_create_texture */
	int		nUsageCount;					/* number of times this texture has been 'Load'ed */
	
	struct
	{
		char			szFileName[128];	/* upper case filename of texture (with/without path??) */
		sgl_bool 		bTranslucent;		/* parameters given to LoadBMPTexture */
		sgl_mipmap_generation_options 		generate_mipmap;
		sgl_bool 		dither;
	} data;

} TEXCACHEENTRY, *PTEXCACHEENTRY;

/*========================================================================================*/
/*========================================================================================*/
/*									STATIC VARIABLES									  */
/*========================================================================================*/
/*========================================================================================*/

char TexturePath[]="";						/* path to texture directory */

static PLIST 	gpTextureCache = NULL;		/* root of texture cache list */
static sgl_bool gbCacheInitialised = FALSE;	/* TRUE if cache is initialised, FALSE if not */

/*========================================================================================*/
/*========================================================================================*/
/*									STATIC FUNCTIONS									  */
/*========================================================================================*/
/*========================================================================================*/

/*===========================================
 * Function:    OnDeleteCacheEntry
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	      Called when deleting a cache entry data structure - deletes texture
 *
 * Params:	      void *pData - pointer to the nFace to be deleted
 *
 * Return:	      void
 *========================================================================================*/
static void OnDeleteCacheEntry (PCVOID pData)
{
	PTEXCACHEENTRY pCE = (PTEXCACHEENTRY) pData;

	ASSERT (pCE != NULL);
	
	sgl_delete_texture (pCE->nTextureName);

	DPF ((DBG_VERBOSE, "Cache entry (%s) deleted", pCE->data.szFileName));
}

/*===========================================
 * Function:    OnFindCacheEntryByData
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	      Called when finding a cache entry data structure
 *
 * Params:	      void *pData - pointer to the nFace to be deleted
 *
 * Return:	      TRUE if match found, FALSE if not
 *========================================================================================*/
static sgl_bool OnFindCacheEntryByData (PCVOID pData, sgl_uint32 u32Extra)
{
	PTEXCACHEENTRY pCE = (PTEXCACHEENTRY) pData;
	PTEXCACHEENTRY pCmp = (PTEXCACHEENTRY) u32Extra;

	ASSERT (pCE != NULL);
	ASSERT (pCmp != NULL);

	return ((strncmp (pCE->data.szFileName, pCmp->data.szFileName, 128) == 0) &&
			(pCE->data.bTranslucent == pCmp->data.bTranslucent) &&
			(pCE->data.generate_mipmap == pCmp->data.generate_mipmap) &&
			(pCE->data.dither == pCmp->data.dither));
}

/*===========================================
 * Function:    OnFindCacheEntryByName
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	      Called when finding a cache entry data structure
 *
 * Params:	      void *pData - pointer to the nFace to be deleted
 *
 * Return:	      TRUE if match found, FALSE if not
 *========================================================================================*/
static int OnFindCacheEntryByName (PCVOID pData, sgl_uint32 u32Extra)
{
	PTEXCACHEENTRY pCE = (PTEXCACHEENTRY) pData;

	ASSERT (pCE != NULL);

	return (pCE->nTextureName == (int) u32Extra);
}

/*===========================================
 * Function:    InitBMPTextureCache
 *===========================================
 *
 * Scope:	      static to this module
 *
 * Purpose:	      Checks to see if cache list is initialised, and inits list if necessary
 *
 * Params:	      void
 *
 * Return:	      void
 *========================================================================================*/
static sgl_bool InitBMPTextureCache ()
{
	if (!gbCacheInitialised)
	{
		gbCacheInitialised = ListInitialiseList (&gpTextureCache, sizeof (TEXCACHEENTRY), 20, OnDeleteCacheEntry);
	}
	return (gbCacheInitialised);
}

/*========================================================================================*/
/*========================================================================================*/
/*									PUBLIC FUNCTIONS									  */
/*========================================================================================*/
/*========================================================================================*/

/*===========================================
 * Function:    LoadBMPTexture
 *===========================================
 *
 * Scope:	    SGL
 *
 * Purpose:	    Loads a texture from a 24bit .BMP file. If translucent, a second BMP is
 *				is opened with a t prepended to the filename, and the red channel is used
 *				for alpha. If an identical texture has already been created the handle
 *				from the original is returned.
 *
 * Params:	    char *pszFilename: bitmap file to load
 *				sgl_bool bTranslucent: TRUE if extra alpha data is to be loaded
 *				sgl_bool generate_mipmap: TRUE if texture is to be mipmapped
 *				sgl_bool dither: TRUE if texture should be dithered into mipmaps
 *
 * Return:	    >= 0 name, or -ve if error
 *========================================================================================*/
int CALL_CONV LoadBMPTexture( char *pszFilename, sgl_bool bTranslucent,
							  sgl_mipmap_generation_options  generate_mipmap, sgl_bool dither )
{
	int nTexture, nNameLen = strlen (pszFilename);
	
	if ((nNameLen == 0) || (nNameLen > 127))
	{
		DPFDEV ((DBG_ERROR, "LoadBMPTexture: Filename %s bad length (%d)!", pszFilename, nNameLen));
		nTexture = sgl_err_bad_parameter;
	}
	else
	{
		if (!InitBMPTextureCache ())
		{
			DPF ((DBG_ERROR, "Cache did not initialise properly"));
			nTexture = sgl_err_no_mem;
		}
		else
		{		
			TEXCACHEENTRY	CE;
			PTEXCACHEENTRY	pFCE;
			
			#if 0

			int k;

			strcpy (CE.data.szFileName, pszFilename);

			/* make filename upper case */
			
			for (k = 0; k < nNameLen; ++k)
			{
				if (islower (CE.data.szFileName[k]))
				{
					CE.data.szFileName[k] -= 'a' - 'A';
				}
			}
			
			#else
			
			{
				char *pSrc, *pDst;

				pSrc = pszFilename;
				pDst = CE.data.szFileName;

				while (*pSrc)
				{
					if ((*pSrc >= 'a') && (*pSrc <= 'z'))
					{
						*pDst = *pSrc - ('a' - 'A');
					}
					else
					{
						*pDst = *pSrc;
					}

					pSrc++;
					pDst++;
				}

				*pDst = 0;
			}

			#endif

			CE.data.bTranslucent = bTranslucent;
			CE.data.generate_mipmap = generate_mipmap;
			CE.data.dither = dither;
	
			pFCE = ListFindItem (gpTextureCache, (FINDITEMFN)OnFindCacheEntryByData,
												 (sgl_uint32) &CE);
			
			if (pFCE)
			{
				DPF ((DBG_VERBOSE, "Identical texture already loaded [%s %s]", pszFilename, pFCE->data.szFileName));
				pFCE->nUsageCount++;
				nTexture = pFCE->nTextureName;
			}
			else
			{
				sgl_intermediate_map Imap = ConvertBMPtoSGL (pszFilename, bTranslucent);
		
				if (!Imap.pixels)	
				{
					DPF ((DBG_ERROR, "Error loading %s to intermediate map", pszFilename));
					nTexture = sgl_err_bad_parameter;
				}
				else
				{
					sgl_map_types map_type;
					sgl_map_sizes map_size;


					if(generate_mipmap)
						map_type = bTranslucent ? sgl_map_trans16_mm : sgl_map_16bit_mm;
					else
						map_type = bTranslucent ? sgl_map_trans16 : sgl_map_16bit;
			
					if ((Imap.x_dim >= 256) && (Imap.y_dim >= 256))
					{
						map_size = sgl_map_256x256;
					}
					else if ((Imap.x_dim >= 128) && (Imap.y_dim >= 128))
					{
						map_size = sgl_map_128x128;
					}
					else if ((Imap.x_dim >= 64) && (Imap.y_dim >= 64))
					{
						map_size = sgl_map_64x64;
					}
					else if ((Imap.x_dim >= 32) && (Imap.y_dim >= 32))
					{
						map_size = sgl_map_32x32;
					}
					else
					{
						DPF ((DBG_ERROR, "Imap is weird size"));
						nTexture = sgl_err_no_mem;
					}

					nTexture = sgl_create_texture (map_type, map_size, generate_mipmap, dither, &Imap, NULL);
				
					if (nTexture < 0)
					{
						DPF ((DBG_ERROR, "Error %d creating texture from %s", nTexture, pszFilename));
						nTexture = sgl_err_bad_parameter;
					}
					else
					{
						pFCE = ListAddItem (gpTextureCache);
	
						if (pFCE)
						{
							DPF ((DBG_VERBOSE, "Texture %s loaded and cached", pszFilename));
							pFCE->nTextureName = nTexture;
							pFCE->nUsageCount = 1;
							pFCE->data = CE.data;
						}
						else
						{
							DPF ((DBG_ERROR, "Error adding texture %s to cache", pszFilename));
							sgl_delete_texture (nTexture);
							nTexture = sgl_err_no_mem;
						}
					}
		
					SGLFree (Imap.pixels);
				}
			}
		}
	}

	return (nTexture);
}

/*===========================================
 * Function:    FreeBMPTexture
 *===========================================
 *
 * Scope:	    SGL
 *
 * Purpose:	    Looks for the given texture in the cache - if it's there, decrements
 *				the usage count. If result is 0, the texture is deleted
 *
 * Params:	    int nTextureName - name of texture to delete
 *
 * Return:	    void
 *========================================================================================*/
void CALL_CONV FreeBMPTexture( int nTextureName )
{
	PTEXCACHEENTRY pFCE;
	
	if (!InitBMPTextureCache ())
	{
		DPF ((DBG_ERROR, "Texture cache wouldn't initialise"));
	}
	else
	{
		pFCE = ListFindItem (gpTextureCache, (FINDITEMFN)OnFindCacheEntryByName, 
											 (sgl_uint32) nTextureName);
	
		if (pFCE)
		{
			if (--pFCE->nUsageCount == 0)
			{
				ListRemoveItem (gpTextureCache, (FINDITEMFN)OnFindCacheEntryByName, 
												(sgl_uint32) nTextureName);
			}
			else
			{
				DPF ((DBG_VERBOSE, "Texture %s - usage count decremented to %d", pFCE->data.szFileName, pFCE->nUsageCount));
			}
		}
		else
		{
			DPFDEV ((DBG_WARNING, "FreeBMPTexture: Unable to find texture %d in cache", nTextureName));
		}
	}
}

/*===========================================
 * Function:    FreeAllBMPTextures
 *===========================================
 *
 * Scope:	    SGL
 *
 * Purpose:	    deletes all loaded textures and reinitialises the cache
 *
 * Params:	    void
 *
 * Return:	    void
 *========================================================================================*/
void CALL_CONV FreeAllBMPTextures()
{
	ListDeleteList (&gpTextureCache);
	gbCacheInitialised = FALSE;
}



/******************************************************************************
 * Function Name: GetShort
 *
 * Inputs       : fp,a file pointer.
 * Outputs      : -
 * Returns      : ,the short in the correct endian
 * Globals Used : -
 *
 * Description  : Gets a short from a little endian file and puts it into the
 *				  correct endian for the computer running this function.
 *****************************************************************************/
unsigned int GetShort(FILE *fp)
{
  int c, c1;
  c = fgetc(fp);  c1 = fgetc(fp);
  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/******************************************************************************
 * Function Name: GetLong
 *
 * Inputs       : fp,a file pointer.
 * Outputs      : -
 * Returns      : ,the long in the correct endian
 * Globals Used : -
 *
 * Description  : Gets a long from a little endian file and puts it into the
 *				  correct endian for the computer running this function.
 *****************************************************************************/
unsigned long GetLong(FILE *fp)
{
  int c, c1, c2, c3;
  c = fgetc(fp);  c1 = fgetc(fp);  c2 = fgetc(fp);  c3 = fgetc(fp);
  return ((unsigned long) c) +
         (((unsigned long) c1) << 8) + 
	 (((unsigned long) c2) << 16) +
	 (((unsigned long) c3) << 24);
}

/******************************************************************************
 * Function Name: PutShort
 *
 * Inputs       : fp,a file pointer.
 *				: i,the short to be written
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Puts a short into a little endian file. It does not matter
 *				  what endian the computer is.
 *****************************************************************************/
void PutShort(FILE *fp,int i)
{
  int c, c1;

  c = ((unsigned int ) i) & 0xff;  c1 = (((unsigned int) i)>>8) & 0xff;
  fputc(c, fp);   fputc(c1,fp);
}

/******************************************************************************
 * Function Name: PutLong
 *
 * Inputs       : fp,a file pointer.
 *				: i,the long to be written
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Puts a long into a little endian file. It does not matter
 *				  what endian the computer is.
 *****************************************************************************/
void PutLong(FILE *fp,long i)
{
  int c, c1, c2, c3;
  c  = ((unsigned long ) i)      & 0xff;  
  c1 = (((unsigned long) i)>>8)  & 0xff;
  c2 = (((unsigned long) i)>>16) & 0xff;
  c3 = (((unsigned long) i)>>24) & 0xff;

  fputc(c, fp);   fputc(c1,fp);  fputc(c2,fp);  fputc(c3,fp);
}

/******************************************************************************
 * Function Name: ConvertBMPtoSGL
 *
 * Inputs       : filename,
 *				  Translucent,TRUE/FALSE
 * Outputs      : -
 * Returns      : ReturnMap,a sgl_intermediate_map
 * Globals Used : TexturePath
 *
 * Description  : This loads a BMP from disk, and converts it to a
 *				  sgl_intermediate_map. If the texture is requested to be 
 *				  translucent, a 24 bit DIB with a "t" on the begining of the 
 *				  filename is loaded with the translucent information in the 
 *				  red component.
 *				  The file is loaded from the "TexturePath" directory
 *				  This routine malloc's the required space.
 *				  A 2x2 WILL NOT LOAD CORRECTLY BECAUSE A BMP LINE HAS TO BE
 *				  4 BYTE ALIGNED.
 *****************************************************************************/
sgl_intermediate_map CALL_CONV ConvertBMPtoSGL( char *filename,
												sgl_bool Translucent )
{
	sgl_intermediate_map ReturnMap;
	char CombinedFilename[100];

	char *pLineBuffer;

	char BaseName[100];
	char PathName[100];
	char *pC;

	FILE *infile;
	sgl_uint32 x,y,TransX,TransY;
	sgl_uint32 i,j;

	sgl_map_pixel *OutDib;

	
	/*
	// Convert the filename into a base and path.
	*/
	strcpy(PathName, filename);

	/*
	// Convert the slash types if we are working in DOS based environment
	*/
	#if WIN32 || DOS32
		pC = strrchr(PathName, UNIX_SEP);
		while (pC != NULL)
		{
			*pC = DOS_SEP;
			pC = strrchr(PathName, UNIX_SEP);
		}
	#endif

	/*
	// find the last separator
	*/
	pC = strrchr(PathName, DIR_SEP);
	/*
	// No path
	*/
	if(pC == NULL)
	{	  
		strcpy(PathName, "");
		strcpy(BaseName, filename);
	}
	/*
	// Else extract the base name and path
	*/
	else
	{
		pC ++;
		strcpy(BaseName, pC);

		/*
		// terminate the path string
		*/
		*pC = '\0';
	}/*end if*/
	

	/*
	** read the dib in from disk 
	*/
	strcpy(CombinedFilename, PathName);
   	strcat(CombinedFilename,BaseName);	

	infile=fopen(CombinedFilename,"rb");
	if (!infile)
	{
		PVROSPrintf ("ConvertBMPtoSGL: Could not open %s ABORTING PROGRAM\n", 
						CombinedFilename);
		exit(1);

		ReturnMap.x_dim = 0;
		ReturnMap.y_dim = 0;
		ReturnMap.pixels = 0;
	}
	else
	{
		int DibLineLength;

		/*
		** work out the texture dimensions.
		*/
		#ifndef MIDAS_ARCADE
		fseek(infile,18,0);
		#else
		/* fseek doesn't seem to work properly on MIDAS Arcade so do it differently */
		#pragma message( "** FSEEK replaced by FREAD for MIDAS Arcade **" )
		if (1)
		{
			char 	ReadBuff[18];
			size_t	BytesRead;
			BytesRead = fread((void *)ReadBuff, 1, 18, infile);
		}
		#endif

		x=GetLong(infile);
	
		y=GetLong(infile);
	
		/*
		// allocate the correct space
		*/
		OutDib=(sgl_map_pixel *) SGLMalloc(x*y*sizeof(sgl_map_pixel));

		/*
		// Allocate a line buffer, so we can read in a whole line at a time.
		// NOTE that this is rounded UP to the nearest legal size for a Windows
		// DIB. All rows are padded up to a multiple of 4 bytes.
		*/
		DibLineLength = ((x * 3) +3) & (~3);
		pLineBuffer = (char *) SGLMalloc(DibLineLength);

		if (OutDib && pLineBuffer)
		{
			/*
			// keep a pointer to the pixel we are storing, and where we are
			// reading from.
			*/
			sgl_map_pixel * pPixel;
			char *pSrc;

			/*skip to the pixel data */
			#ifndef MIDAS_ARCADE
			fseek(infile,28,1);
			#else
			#pragma message( "** FSEEK replaced by FREAD for MIDAS Arcade **" )
			/* fseek doesn't seem to work properly on MIDAS Arcade so do it differently */
			if (1)
			{
				char 	ReadBuff[28];
				size_t	BytesRead;
				BytesRead = fread((void *)ReadBuff, 1, 28, infile);
			}
			#endif

			/*
			** load the BMP and
			** Convert from RGB to sgl_map_pixel.
			*/
			pPixel = OutDib;

			for(i=y; i!= 0; i--)
			{
				/*
				// Read in the next line, and put the pointer back to the
				// start of the line
				*/
				fread(pLineBuffer, 1, DibLineLength, infile);
				pSrc = pLineBuffer;

				/*
				// Copy over the pixels
				*/
				for(j=x; j!=0; j--)
				{
					pPixel->blue  = pSrc[0];
					pPixel->green = pSrc[1];
					pPixel->red	  = pSrc[2];
					pPixel->alpha = 0;

					pPixel ++;
					pSrc +=3;
				}/*end for j*/
			}/*end for y*/
		
			fclose(infile);
		
			if(Translucent)
			{
				/*
				// append a 't' to the begining of the filename BUT after any
				// directory paths.
				//
				// First find the last / (or \ depending on operating system)
				*/
				strcpy(CombinedFilename, PathName);
				strcat(CombinedFilename,"t");	
				strcat(CombinedFilename,BaseName);	

				infile=fopen(CombinedFilename,"rb");
		
				if (!infile)
				{
					PVROSPrintf ("ConvertBMPtoSGL: Could not open %s ABORTING PROGRAM\n", 
						CombinedFilename);

					exit(1);

					ReturnMap.x_dim = 0;
					ReturnMap.y_dim = 0;
					ReturnMap.pixels = 0;
				}
				else
				{
					/*
					** work out the texture dimensions.
					*/
			
					fseek(infile,18,0);
					TransX=GetLong(infile);
			
					TransY=GetLong(infile);
			
					if ((TransX != x) || (TransY != y)) 
					{
						DPF ((DBG_ERROR, "ConvertBMPtoSGL: %s. The translucent dib is not the same size", filename));
						fclose (infile);
						ReturnMap.x_dim = 0;
						ReturnMap.y_dim = 0;
						ReturnMap.pixels = 0;
					}
					else
					{
						fseek(infile,28,1);
				

						/* place the translucent info in the RGBA bitmap */
						pPixel = OutDib;

						for(i=0;i<y;i++)
						{
							/*
							// Read in the next line, and put the pointer back to
							// the start of the line
							*/
							fread(pLineBuffer, 1, DibLineLength, infile);
							pSrc = pLineBuffer;

							/*
							// Copy over the pixels
							*/
							for(j=x; j!=0; j--)
							{
								pPixel->alpha = pSrc[2];

								pPixel ++;
								pSrc +=3;
							}/*end for j*/

						}
						fclose(infile);
				
						/*fill out the sgl_intermediate_map */
				
						ReturnMap.id[0]='I';
						ReturnMap.id[1]='M';
						ReturnMap.id[2]='A';
						ReturnMap.id[3]='P';
				
						ReturnMap.x_dim=x;
						ReturnMap.y_dim=y;
				
						ReturnMap.pixels=OutDib;
					}
				}

			}
			/*
			// ELSE not translucent
			*/
			else
			{
				/*fill out the sgl_intermediate_map */
			
				ReturnMap.id[0]='I';
				ReturnMap.id[1]='M';
				ReturnMap.id[2]='A';
				ReturnMap.id[3]='P';
				
				ReturnMap.x_dim=x;
				ReturnMap.y_dim=y;
				
				ReturnMap.pixels=OutDib;
			}
			/*
			// Free up the temporary space
			*/
			SGLFree(pLineBuffer);
		}
		/*
		// Else failed to allocate memory. Free up the space we did manage to
		// allocate
		*/
		else
		{
			if(pLineBuffer)
			{
				SGLFree(pLineBuffer);
			}
			if(OutDib)
			{
				SGLFree(OutDib);
			}
		}/*end if/else malloced OK*/

	}/*end else FILE exists*/
	
	return(ReturnMap);
}



/*
** end of file.
*/
