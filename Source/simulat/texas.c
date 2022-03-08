/******************************************************************************
 * Name         : texas.c
 * Title        : This is the texas software simulator.
 * Author       : Peter Leaback
 * Created      : 9/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : This program should be functionally identical to the real texas.
 * 				  Where it may part from the ASIC is how it deals with incorrect input.
 *				  eg mipMapped and 8Bit texture bits set
 *				  It looks a bit of a mess at the moment because of the debugging
 *				  output the hardware boys wanted.
 * 				  This a convert from RGL.
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: texas.c,v $
 * Revision 1.24  1997/04/02  19:39:18  mjg
 * Included the calling convention on TexasWriteBMP.
 *
 * Revision 1.23  1997/03/31  22:13:47  jop
 * Got rid of windows references
 *
 * Revision 1.22  1996/12/11  01:24:34  jop
 * Removed ../'s from includes
 *
 * Revision 1.21  1996/07/18  09:10:26  mal
 * changed -- to ++
 *
 * Revision 1.20  1996/06/27  12:04:03  sjf
 *  Fixed Signed/Unsigned bug in WriteNPixelPairs.
 *
 * Revision 1.19  1996/06/25  16:37:51  sjf
 * Added WriteNPixelpairs  to speed up texture caching.
 *
 * Revision 1.18  1996/02/20  10:11:06  jrc
 * Including smtexas.h for DUMP_PARAM_FILES value.
 *
 * Revision 1.17  1996/02/19  14:19:40  sjf
 * Moved Twiddle functions to txmops
 *
 * Revision 1.16  1995/12/05  10:21:23  pdl
 * coded the PCX1 textures.
 *
 * Revision 1.15  1995/11/28  11:58:50  pdl
 * enabled more parameter file writing.
 *
 * Revision 1.14  1995/11/24  17:30:51  pdl
 * added two pixel access.
 *
 * Revision 1.13  1995/10/20  10:48:37  sjf
 * Changed param buffer variable.
 *
 * Revision 1.12  1995/10/06  08:37:18  sjf
 * Fixed order of texture memory dump so it was in little endian.
 *
 * Revision 1.11  1995/08/19  11:16:17  sjf
 * Simulator now reads fog disable flag.
 *
 * Revision 1.10  1995/08/08  12:21:20  pdl
 * removed some warnings.
 *
 * Revision 1.9  1995/08/03  12:53:50  pdl
 * changed the endian processing.
 *
 * Revision 1.8  1995/08/02  09:28:13  pdl
 * commented out the cache simulation.
 *
 * Revision 1.7  1995/07/26  17:33:57  pdl
 * added some includes.
 *
 * Revision 1.6  1995/07/26  17:23:09  pdl
 * changed the parameter store.
 *
 * Revision 1.5  1995/07/26  15:14:31  jop
 * Did it better
 *
 * Revision 1.3  1995/07/26  09:53:27  jop
 * Added simulator window support
 * /
 *
 * Revision 1.2  1995/07/23  16:27:06  pdl
 * Solved the alignment problem with bmp's.
 *
 * Revision 1.1  1995/07/17  16:08:34  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#include <math.h>
#include "sgl_defs.h"
#include "dlnodes.h"
#include "rnglobal.h"
#include "ldbmp.h"
#include "txmops.h"
#include "smtexas.h" /* for DUMP_PARAM_FILES */

int iter_count; /*this is for ians simulation testing*/


typedef struct
{
	long m;
	int  e;
} pfloat;

typedef struct {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
}RGB;

typedef struct {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
}RGBA;


unsigned short cfrScale;			/*the scalar for 'c','f' and 'r' */

unsigned short textureMemory[TEXTURE_MEMORY_SIZE];

RGB fogColour;

int Bitmap_ydim,Bitmap_xdim; /*this is for bringing texas simulator up with sgl*/

RGB frameBuffer[MAX_X_DIM * MAX_Y_DIM]; /*what texas writes into*/


int spanflag=0; /*just to show where the spans starts for TOIAN stuff*/
int previousPlaneWasTextured=1; /*for ians pre-calc stuff */


/*
**    The following variables are used for the cache simulation.
**
*/

long missCount=0;
long Mtag,Mblock;

unsigned char replace[1<<CACHE_ENTRIES]; /* the toggle for which set to replace*/

struct tagEntry {
	long address;
	unsigned char empty;
};

struct tagEntry tags[2][1<<CACHE_ENTRIES]; /* the array of cache tags */

FILE *FshadePreCalc;
FILE *FtexPreCalc;
FILE *FrawTexPixels;
FILE *FtexasInput;
FILE *Fpixels;


/*=========================================================================
name	|InitTexasSimulator
function|opens the param files for writing
in		|
out		|-
rd		|
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void InitTexasSimulator()
{


	FshadePreCalc=fopen("params/shadeprecalc.txt","wb"); 
	FtexPreCalc=fopen("params/texprecalc.txt","wb"); 
	FrawTexPixels=fopen("params/rawtexpixels.txt","wb"); 
	FtexasInput=fopen("params/texasinput.txt","wb"); 
	Fpixels=fopen("params/pixels.txt","wb"); 

}

/*=========================================================================
name	|FinishTexasSimulator
function|closes the param files.
in		|
out		|-
rd		|
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void FinishTexasSimulator()
{
	/*complete the last span */

   	fprintf(FtexPreCalc,"%d\n",iter_count);

	/*close all those files*/

	fclose(FshadePreCalc); 
	fclose(FtexPreCalc); 
	fclose(FrawTexPixels);
	fclose(FtexasInput); 
	fclose(Fpixels); 
}

/*=========================================================================
name	|TexasWriteBMP
function|temp function for writing out the frame buffer as a bmp.
		|I haven't coded the alignment correction yet.
in		|filename, does not have to be j000.bmp !
out		|-
rd		|frameBuffer
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void CALL_CONV TexasWriteBMP( char * filename )
{

	FILE *imageFile;
	int y;

	imageFile=fopen(filename,"wb"); 

	if (imageFile==NULL)
	{

		printf("TexasWriteBMP: could not create the image file %s\n",filename);
		ASSERT(FALSE);
	}

	PutShort(imageFile,0x4d42);
	PutLong(imageFile,54+Bitmap_xdim*Bitmap_ydim*sizeof(RGB));
	PutLong(imageFile,0);
	PutLong(imageFile,0x36);
	PutLong(imageFile,0x28);
	PutLong(imageFile,Bitmap_xdim);
	PutLong(imageFile,Bitmap_ydim);
	PutShort(imageFile,1);
	PutShort(imageFile,0x18);
	PutLong(imageFile,0);
	PutLong(imageFile,0);
	PutLong(imageFile,0);
	PutLong(imageFile,0);
	PutLong(imageFile,0);
	PutLong(imageFile,0);


	for(y=Bitmap_ydim-1;y>=0;y--)
	{
		fwrite(&(frameBuffer[y*Bitmap_xdim]),sizeof(RGB),Bitmap_xdim,imageFile);
	}


	fclose(imageFile);

}



/*=========================================================================
name	|TexasSetDim
function|temp function for bringing texas simulator up with sgl.
in		|x,
		|y,
out		|-
rd		|-
wr		|Bitmap_ydim,Bitmap_xdim
pre		|-
post	|-		 
=========================================================================*/
void TexasSetDim(int x,int y)
{
	Bitmap_ydim =(y > MAX_Y_DIM) ? MAX_Y_DIM : y;
	Bitmap_xdim =(x > MAX_X_DIM) ? MAX_X_DIM : x;
}

/*=========================================================================
name	|TexasSetFogColour
function|.
in		|Colour,
out		|-
rd		|-
wr		|fogColour
pre		|-
post	|-		 
=========================================================================*/
void TexasSetFogColour(sgl_map_pixel Colour)
{
	fogColour.Red  =Colour.red;
	fogColour.Green=Colour.green;
	fogColour.Blue =Colour.blue;
	DPF((DBG_MESSAGE, "fog colour is %02X%02X%02X",Colour.red,Colour.green,Colour.blue));

}

/*=========================================================================
name	|TexasSetCFRScale
function|This can be the distance between the center of projection and the
		|the projection plane.This is a function,because the HW may need it.
in		|Scale,
out		|-
rd		|-
wr		|fogColour
pre		|-
post	|-		 
=========================================================================*/
void TexasSetCFRScale(UINT16 Scale)
{
	cfrScale=Scale;
}


/*=========================================================================
name	|FlushCache
function|makes each cache slot empty
in		|-
out		|-
rd		|-
wr		|tags
	   	|missCount
pre		|-
post	|-		 
=========================================================================*/
void FlushCache()
{
	int i;

	for(i=0;i<(1<<CACHE_ENTRIES);i++)
	{
		tags[0][i].empty=TRUE;
		tags[1][i].empty=TRUE;
	}

    missCount=0;
}

/*=========================================================================
name	|FetchParameter
function|
in		|-
out		|-
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
unsigned long FetchParameter(unsigned long address)
{
	unsigned long tag;
	unsigned long block_address;
	unsigned char miss;


if(0) /*remove the cache simulation for speed*/
{
	
	block_address=address >> CACHE_BLOCK_SIZE; /*reduce the granularity of the cache entries*/
	
	tag=block_address & ((1<<CACHE_ENTRIES)-1);   /*direct map the address into the cache */

	Mblock=block_address;
	Mtag=tag;

	if((tags[0][tag].address==block_address) && (tags[0][tag].empty==FALSE))    
	{
		/*
		** there is a hit in bank 0
		*/

		miss=FALSE;
		replace[tag]=1;   /* if we have to replace, replace bank 1*/

	}
	else
	{
		if((tags[1][tag].address==block_address) && (tags[1][tag].empty==FALSE))    
		{
			/*
			** there is a hit in bank 1
			*/
			miss=FALSE;
			replace[tag]=0;    /* if we have to replace, replace bank 0*/


		}
		else
		{
			miss=TRUE;
			missCount++;
			
		}

	}

	if(miss==TRUE)
	{
		if(tags[0][tag].empty && tags[1][tag].empty)
		{
			/*
			** both entries are empty
			*/
			
			tags[0][tag].empty=FALSE;
			tags[0][tag].address=block_address;

			replace[tag]=1; /* not vital*/
		}
		else
		{
			if(tags[0][tag].empty)
			{
				tags[0][tag].empty=FALSE;
				tags[0][tag].address=block_address;

				replace[tag]=1; 
				
			}
			else
			{
				if(tags[1][tag].empty)
				{
					tags[1][tag].empty=FALSE;
					tags[1][tag].address=block_address;

					replace[tag]=0; 
				
				}
				else
				{
					/*
					** we have to throw away some data.
					*/

					tags[replace[tag]][tag].address=block_address;
					replace[tag]^=1;    /*toggle replace bank*/

				}

			}
		}
	}

}

	return(ParamBufferInfo.tsp.pParamStore[address]);

}


/*=========================================================================
name	|DumpParmeterMemory
function|Dumps 1Mb of the parameter store to a file called "parameters.bin"
in		|-
out		|-
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void DumpParameterMemory()
{

	FILE *dumpFile;
	long i;
	unsigned char ch;

	dumpFile=fopen("parameters.bin","wb");

	for(i=0;i<262144;i++)
	{
		ch=(ParamBufferInfo.tsp.pParamStore[i]>>24) & 0xff;
		putc(ch,dumpFile); 
		ch=(ParamBufferInfo.tsp.pParamStore[i]>>16) & 0xff;
		putc(ch,dumpFile); 
		ch=(ParamBufferInfo.tsp.pParamStore[i]>>8) & 0xff;
		putc(ch,dumpFile); 
		ch=(ParamBufferInfo.tsp.pParamStore[i]) & 0xff;
		putc(ch,dumpFile); 

	}

	fclose(dumpFile);


}

/*=========================================================================
name	|DumpTextureMemory
function|Dumps the texture memory to a file called "texturemem.bin"
in		|-
out		|-
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
void DumpTextureMemory()
{
	FILE *dumpFile;
	long i;

	dumpFile=fopen("params/text.bin","wb");


	/*
	// Assumes an even number of pairs of words
	*/

#if 0 /*little endian version*/

	for(i=0;i<TEXTURE_MEMORY_SIZE;i+=2)
	{
		/*
		// Dump in little endian mode (sort of)
		*/
		putc( textureMemory[i+1]&0xff,		dumpFile);	/*lo byte*/
		putc((textureMemory[i+1]>>8) &0xff,	dumpFile); /*hi byte*/

		putc( textureMemory[i]&0xff,		dumpFile);	/*lo byte*/
		putc((textureMemory[i]>>8) &0xff,	dumpFile); /*hi byte*/
	}

#else /*big endian version*/

	for(i=0;i<TEXTURE_MEMORY_SIZE;i++)
	{
		putc((textureMemory[i]>>8) &0xff,	dumpFile); /*hi byte*/
		putc( textureMemory[i]&0xff,		dumpFile);	/*lo byte*/
	}
#endif
	fclose(dumpFile);


}


/*=========================================================================
name	|ToPfloat
function|returns the pfloat version of a 32bit int
in		|x, the number to be converted
out		|temp, the Pfloat number
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/

pfloat ToPfloat(long x)
{
	pfloat temp;

	/*where is the top bit of x ??? */

	temp.e=0;

	if (x<0)
		for(temp.e=0;  1<<temp.e<= -x     ;temp.e++);
	else if(x>0)
		for(temp.e=0;  1<<temp.e<= x      ;temp.e++);

	temp.m=x<<(31-temp.e);
	temp.m>>=16;

	return temp;
}

/*=========================================================================
name	|AShift
function|returns x >> shift
		|allows negative shifts and greater than 31
in		|x, 
		|shift, the shift factor
out		|x,
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/

long AShift(long x,int shift)
{
	if(shift<0)
	{
		if(shift<-31)
		{
			if(x<0)
				x=-1;
			else
				x=0;
		}
		else
			x>>=-shift;
	}
	else
	{
		if(shift>31)
			x=0;
		else
			x<<=shift;

	}
	return(x);
}



/*=========================================================================
name	|AddressCalc
function|Calculate a pixel address given the u,v coordinates, base address
		|etc. 
in		|u, the u coordinate
		|v, the v coordinate
		|address,
		|mapSize, 00=256x256
		|whichMap,the exponent of 'd'-1
		|colourDepth, 1=16 bit 0=8 bit
		|mipMapped,
		|flipUV, reflect either U or V
out		|pixel_address
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/

unsigned long AddressCalc(long u,long v,unsigned long address,unsigned char mapSize,
						  int whichMap,unsigned char colourDepth,unsigned char mipMapped,
						  unsigned char flipUV)
{

	unsigned long pixel_address;
#ifdef PCX1  /*align the maps with 2 pixels */

	static unsigned long mmc[] ={0x5556,  	/*mip map offset*/
								 0x1556,
								 0x0556,
								 0x0156,
								 0x0056,
								 0x0016,
								 0x0006,
								 0x0002,
								 0x0001};
#else
	static unsigned long mmc[] ={0x5555,  	/*mip map offset*/
								 0x1555,
								 0x0555,
								 0x0155,
								 0x0055,
								 0x0015,
								 0x0005,
								 0x0001,
								 0x0000};
#endif
	static unsigned long mapMask[]={0x00ff,	/*where u and v should wrap*/
									0x007f,
									0x003f,
									0x001f};



	if(mapSize && flipUV) /*if the map size is not 256 and either U or V are to be reflected */
	{
		if( ((mapMask[mapSize]+1) & u ) && (flipUV & 2))
			u=~u;
		if( ((mapMask[mapSize]+1) & v ) && (flipUV & 1))
			v=~v;

	}

	if(mapSize==0 && flipUV) /*if the map size is  256,flip as if it is 128 */
	{
		if( (128 & u ) && (flipUV & 2))
			u=~u;
		if( (128 & v ) && (flipUV & 1))
			v=~v;

	}

	u &= mapMask[mapSize];
	v &= mapMask[mapSize];

	if(mipMapped==FALSE)
	{
		if(colourDepth==1)
			pixel_address=address+Twid(u,v);
		else
			pixel_address=address+Twid(u>>1,v);
	}
	else
	{
		if((mapSize+whichMap) & 1) /*if odd map toggle big bank*/ 
			address^=BIG_BANK;

		pixel_address=address+mmc[mapSize+whichMap]+Twid(u>>whichMap,v>>whichMap);

	}
	return(pixel_address);
}

/*=========================================================================
name	|FetchPixel
function|returns a raw 16 bit pixel given an address
		|this may perform page break counting some time.
in		|address,
out		|pixel,
rd		|textureMemory
wr		|-
pre		|-
post	|-		 
=========================================================================*/
unsigned short FetchPixel(unsigned long address)
{
	unsigned short pixel=0;
/*	static unsigned long Abase=-1;
	static unsigned long Bbase=-1; */
	
	if(address < (TEXTURE_MEMORY_SIZE/2))

		pixel=textureMemory[address];

	else if( (address >= BIG_BANK) && (address < (BIG_BANK+(TEXTURE_MEMORY_SIZE/2))) )

		pixel=textureMemory[(address ^ BIG_BANK)+(TEXTURE_MEMORY_SIZE/2)];

	else

		fprintf(stderr,"FetchPixel: Out of address range :%lX\n",address);

/*
	if(address < 0x40000)
	{
		if((address & 0xfffffe00)!=Abase)
		{
			Abase=address & 0xfffffe00;
			pixel=0xff00;
		}
	}
	else
	{
		if((address & 0xfffffe00)!=Bbase)
		{
			Bbase=address & 0xfffffe00;
			pixel=0xff00;
		}

	}
*/

	return(pixel);

}

/*=========================================================================
name	|WritePixel
function|writes a pixel into the texture memory
in		|address,
		|pixel,
out		|-
rd		|-
wr		|textureMemory
pre		|-
post	|-		 
=========================================================================*/
void WritePixel(unsigned long address,unsigned short pixel)
{



	if(address < (TEXTURE_MEMORY_SIZE/2))

		textureMemory[address]=pixel;

	else if( (address >= BIG_BANK) && (address < (BIG_BANK+(TEXTURE_MEMORY_SIZE/2))) )

		textureMemory[(address ^ BIG_BANK)+(TEXTURE_MEMORY_SIZE/2)]=pixel;

	else

		fprintf(stderr,"WritePixel: Out of address range :%lX\n",address);


}
/*=========================================================================
name	|WriteTwoPixels
function|writes a pair of adjacent pixels into the texture memory
in		|address,
		|pixels,
out		|-
rd		|-
wr		|textureMemory
pre		|address is two word aligned.
post	|-		 
=========================================================================*/
void WriteTwoPixels(unsigned long address,unsigned long pixels)
{



	if(address < (TEXTURE_MEMORY_SIZE/2))
	{

		textureMemory[address  ]=pixels>>16;
		textureMemory[address+1]=pixels & 0xffff;
	}
  
	else if( (address >= BIG_BANK) && (address < (BIG_BANK+(TEXTURE_MEMORY_SIZE/2))) )
	{
		textureMemory[(address     ^ BIG_BANK)+(TEXTURE_MEMORY_SIZE/2)]=pixels>>16;
		textureMemory[((address+1) ^ BIG_BANK)+(TEXTURE_MEMORY_SIZE/2)]=pixels & 0xffff;
	}
	else

		fprintf(stderr,"WriteTwoPixels: Out of address range :%lX\n",address);


}


/*=========================================================================
name	|WriteNPixelPairs
function|writes an even number of pixels to the texture memory, starting from
		|the given addresses.
		|
out		|-
rd		|-
wr		|textureMemory
pre		|address is two word aligned.
post	|-		 
=========================================================================*/
void WriteNPixelPairs( unsigned long address,
					   UINT16 *pSrc,
					   long  numPixelPairs)
{
	int i;
	UINT32 	pair;

	ASSERT((address & 1) == 0);

	/*
	// Step through all the pixel pairs, 2 at a time
	*/
	for(i = 0; i < numPixelPairs ; i++)
	{
		pair = ((pSrc[0]) << 16) | (pSrc[1]);

		WriteTwoPixels(address, pair);

		pSrc  +=2;
		address +=2;
	}

}




/*=========================================================================
name	|ColourConvert
function|given a raw 16bit pixel, it returns a 15 or 24 bit RGB.
		|it unpacks either 332, 4444 or 555.
		|whichPixel specifies the required pixel in 8 bit mode.
in		|raw_pixel,
		|colourDepth,
		|whichPixel,0 means the high byte and 1 means the low byte.
		|translucent,1 means 4444 colour format. 0 means not translucent
out		|pixel
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/

RGBA ColourConvert(unsigned short raw_pixel,unsigned char colourDepth,
				   unsigned char whichPixel,unsigned char translucent)
{
	RGBA pixel;

	if(translucent)
	{
		/*
		** unpack the 4444 pixel
		*/

		pixel.Blue =(raw_pixel & 0x000f);
		pixel.Green=(raw_pixel & 0x00f0)>>4;
		pixel.Red  =(raw_pixel & 0x0f00)>>8;
		pixel.Alpha=(raw_pixel & 0xf000)>>12;

		/* expand up to 555 */

		pixel.Red  <<=1;
		pixel.Green<<=1;
		pixel.Blue <<=1;

		/* repeat the top bit */

		pixel.Red  |=pixel.Red  >>4;
		pixel.Green|=pixel.Green>>4;
		pixel.Blue |=pixel.Blue >>4;

	}
	else if(colourDepth)
	{
		/*
		** unpack the 555 pixel
		*/
		
		pixel.Red  =(raw_pixel>>10 ) & 0x1f;
		pixel.Green=(raw_pixel>>5  ) & 0x1f;
		pixel.Blue = raw_pixel       & 0x1f;
		pixel.Alpha=0;

	}
	else
	{
	 	/*
		** unpack the 332 pixel
		*/

		/*put the wanted pixel in the bottom byte*/

		if(whichPixel==0)
			raw_pixel>>=8;

		pixel.Red  = raw_pixel     & 0xe0;
		pixel.Green=(raw_pixel<<3) & 0xe0;
		pixel.Blue =(raw_pixel<<6) & 0xc0;
		pixel.Alpha=0;

		/*replicate the top bit*/

		if(pixel.Blue  & 0x80)
			pixel.Blue |=0x3f;

		if(pixel.Green & 0x80)
			pixel.Green|=0x1f;

		if(pixel.Red   & 0x80)
			pixel.Red  |=0x1f;

	}

	return(pixel);

}


/*=========================================================================
name	|TexturePixel
function|given an x,y coordinate, and the texture information, return a
		|colour. 
in		|x, the x coordinate
		|y, the y coordinate
		|a,b,c,d,e,f,p,q,r, the texture coefficients
		|exp, the exponent 
		|address, the base address of the texture
		|mapSize,
		|pmip,the coefficient for mip-mapping
		|colourDepth, 1=16 bit 0=8 bit
		|mipMapped,
		|col4444or555, a bool that describes the MIP-Map format
		|globalTrans, a global translucency for textures
		|flipUV, reflect U or V
		|translucent, not used for my logic.
out		|tcol, the output colour
rd		|cfrScale
wr		|-
pre		|0<x<2048
		|0<y<2048
post	|-		 
        | currently the 332 colour is expanded up to 888 incorrectly
=========================================================================*/

RGBA TexturePixel(int x,int y,int a,int b,int c,int d,int e,int f,int p,int q,int r,
				  int exp,unsigned long address,unsigned char mapSize,
				  pfloat pmip,unsigned char colourDepth,
				  unsigned char mipMapped,unsigned char col4444or555,
				  int globalTrans, unsigned char flipUV,
				  unsigned char translucent)

{

	RGBA tcol,high_pixel,low_pixel;
	long abc,def,pqr;
	int shift;
	int powerTwo;
	pfloat top,bot;
	long u,v;
	unsigned long high_res_address,low_res_address;
	unsigned short raw_pixel;


	abc=a*x + b*y + c*cfrScale;
	def=d*x + e*y + f*cfrScale;
	pqr=p*x + q*y + r*cfrScale;



if(DUMP_PARAM_FILES==1 && spanflag)
{

fprintf(FtexPreCalc,"%d %d %ld %d %ld %d %ld %ld %d %ld %d %d %d %d %d %d %d ",
			   exp,a,abc,d,def,p,pqr,pmip.m,pmip.e,address,(int)mapSize,(int)colourDepth,
			   (int)mipMapped,(int)col4444or555,(int)globalTrans,(int) flipUV,(int) translucent); 
}

	/* convert the bottom 29bit	number to floating point */

	bot=ToPfloat(pqr);
	
	bot.m>>=1; /* the bottom is only accurate to 14bits */


	/*
	** work out the reciprocal
	*/

	if(bot.m>0)
		bot.m=0x8000000/bot.m;
	else
		bot.m=0x4000;	

	powerTwo=(bot.m==0x4000);

	if(powerTwo)
		bot.m=0x2000;

	/*
	** calculate u 
	*/

	top=ToPfloat(abc);
	top.e+=exp;					/* add on the exponent to the top*/ 

	u=(top.m*bot.m)>>14;

	if(powerTwo)
		shift=top.e-(bot.e+13);
	else
		shift=top.e-(bot.e+14);

	u=AShift(u,shift) & 255;

	/*
	** calculate v 
	*/

	top=ToPfloat(def);
	top.e+=exp;					/* add on the exponent to the top*/ 

	v=(top.m*bot.m)>>14;

	if(powerTwo)
		shift=top.e-(bot.e+13);
	else
		shift=top.e-(bot.e+14);

	v=AShift(v,shift) & 255;

	/* 
	** calculate d
	*/

	bot.m>>=6;  /*reduce resolution to 8 bits*/

	bot.m=bot.m*bot.m;  /*square the bottom*/

	/* now normalise */

	if(powerTwo)
		bot.e--; 

	if(bot.m & 0x8000)
	{
		bot.e=bot.e*2;
		bot.m>>=8;
	}
	else
	{
		bot.e=bot.e*2 + 1;
		bot.m>>=7;
	}

	/*multiply by pmip*/

	bot.m=bot.m*pmip.m;

	if(bot.m & 0x8000)
	{
		bot.e=pmip.e - (bot.e - 2);
		bot.m>>=8;
	}
	else
	{
		bot.e=pmip.e - (bot.e - 1);
		bot.m>>=7;
	}


	if(bot.e<0)
		bot.e=0;

	if(bot.e>15)	/*could change*/
		bot.e=15;


	/*
	** now fetch the pixel information from RAM 
	*/


	mapSize=3-mapSize; /*flip 00=32x32 to 00=256x256 for ease of programming*/

	if(bot.e<1 || mipMapped==FALSE || colourDepth==0 )
	{
		/*
		** only fetch the highest resolution map
		*/

		high_res_address=AddressCalc(u,v,address,mapSize,0,colourDepth,mipMapped,flipUV);

		raw_pixel=FetchPixel(high_res_address);
		tcol=ColourConvert(raw_pixel,colourDepth,u & 1,col4444or555);
 
		if(colourDepth==1)
		{
			/*convert from 555 to 24Bit colour*/

			tcol.Red  =(tcol.Red  <<3)+4;
			tcol.Green=(tcol.Green<<3)+4;
			tcol.Blue =(tcol.Blue <<3)+4;

		}
	}
	else if(mapSize+bot.e>8)
	{
		/*
		** only fetch from lowest resolution map (1x1) 
		*/

		low_res_address=AddressCalc(u,v,address,0,8,colourDepth,mipMapped,flipUV);

		raw_pixel=FetchPixel(low_res_address);
		tcol=ColourConvert(raw_pixel,colourDepth,0,col4444or555);

		/*now convert to 24Bit colour*/

		tcol.Red  =(tcol.Red  <<3)+4;
		tcol.Green=(tcol.Green<<3)+4;
		tcol.Blue =(tcol.Blue <<3)+4;
	}
	else
	{
		/*
		** fetch both pixels and interpolate
		*/

		high_res_address=AddressCalc(u,v,address,mapSize,bot.e-1,colourDepth,mipMapped,flipUV);
		low_res_address	=AddressCalc(u,v,address,mapSize,bot.e  ,colourDepth,mipMapped,flipUV);

		raw_pixel=FetchPixel(high_res_address);
		high_pixel=ColourConvert(raw_pixel,colourDepth,0,col4444or555); 

		raw_pixel=FetchPixel(low_res_address);
		low_pixel=ColourConvert(raw_pixel,colourDepth,0,col4444or555);  

		/*now interpolate between the two colours*/

		bot.m  &=0x7f; 	/*same as subtracting 128*/
		bot.m >>=2; 	/*reduce the resolution by 2 bits*/

		tcol.Red  =(high_pixel.Red  <<3) + ( (bot.m*(low_pixel.Red  - high_pixel.Red  ))>>2 );
		tcol.Green=(high_pixel.Green<<3) + ( (bot.m*(low_pixel.Green- high_pixel.Green))>>2 );
		tcol.Blue =(high_pixel.Blue <<3) + ( (bot.m*(low_pixel.Blue - high_pixel.Blue ))>>2 );
			 
		/* add 4 to the resulting colour. This is to conform to the agreed colour format */

		tcol.Red  +=4;
		tcol.Green+=4;
		tcol.Blue +=4;

		/* pick the lower resolution Alpha*/

		tcol.Alpha=low_pixel.Alpha;
	}

	/* add in the global translucency */

	tcol.Alpha+=globalTrans;
	if(tcol.Alpha>15)
		tcol.Alpha=15;


/*
    if(TOIAN==1)
    {
		if(spanflag)
		{
			if(lastMiss!=missCount)
			{
				printf("MISS %d %d\n",Mtag,Mblock);
				lastMiss=missCount;
			}
			else
			{
				printf("HIT %d %d\n",Mtag,Mblock);

			}
		}
	}
*/
	return(tcol);
}

/*=========================================================================
name	|ToInt
function|Converts the bottom 16 bits of an unsigned long into a *signed* int.
		|the top 16 bits are ignored. This is hopefully implementation
 		|independant.
in		|x, 
out		|y, 
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
int	ToInt(unsigned long x)
{
	int	y;

	x=x & 0xffff;

	if((x & 0x8000)==0)
		y=x;
	else
	{
	    y=-(x ^ 0xffff);
		y--;
	}	
	return(y);

}


/*=========================================================================
name	|ConvertFrom16to24
function|converts from the 16 bit colour format to a 24Bit RGB
in		|raw_pixel, 
out		|pixel, 
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
RGB ConvertFrom16to24(unsigned short raw_pixel)
{
	RGB	pixel;

	pixel.Red  =(raw_pixel>>10 ) & 0x1f;
	pixel.Green=(raw_pixel>>5  ) & 0x1f;
	pixel.Blue = raw_pixel       & 0x1f;

	pixel.Red  =(pixel.Red  <<3)+4;
	pixel.Green=(pixel.Green<<3)+4;
	pixel.Blue =(pixel.Blue <<3)+4;

	return(pixel);
}

/*=========================================================================
name	|LinearShade
function|Calculates the intensity of the shading function at a local
        |coordinate.
in		|t0,t1,t2, the linear shading coefficients
        |x_local, y_local, the screen coordinates relative to a point on
		|                  the polygon.
out		|intensity, 
rd		|-
wr		|-
pre		|-
post	|-		 
=========================================================================*/
long LinearShade(int t0,int t1,int t2,int x_local,int y_local)
{
	long intensity;

	intensity =t0<<2;
	intensity+=(t1*y_local);
	intensity+=(t2*x_local);

	return(intensity);
}

/*=========================================================================
name	|Texas
function|This is the pixel interface with Sabre. Sabre communucates x,y
		|,instruction address etc and the routine colours that pixel.
in		|x, the x coordinate
		|y, the y coordinate
		|address, the address of the shading instruction
		|shadow, the shadow bit 1==in shadow
		|fog, the fogging interpolation factor
out		|-
rd		|parameterStore
wr		|frameBuffer
pre		|0<x<2048
		|0<y<2048
post	|-		 
=========================================================================*/

void Texas(int x,int y,unsigned long address,unsigned char shadow,unsigned char fog)
{
	

	RGBA colour;
	RGB base,holdCol,holdCol1,shadowColour,highlightCol,shadowHighlightCol;
	int	red,green,blue;
	
	long fraction;
	int t0,t1,t2;
	int a,b,c,d,e,f,p,q,r;
	int exp,globalTrans;
	int x_offset,y_offset;
	unsigned long texture_address;
	unsigned char mapSize,colourDepth,mipMapped,transPass,col4444or555,flipUV;
	pfloat pmip;
	unsigned long inc_address;

	/*
	// 'last' means previous, not final
	*/
	static int last_plane=-1; /*this is for ians simulation testing*/
	static int last_x=-1;
	static int last_y=-1;

	static long lastMiss=-1;


	if(address==last_plane && ((x % 32)!=0) && last_y==y && last_x==x-1) 
/*	if(address==last_plane && iter_count<31 && last_y==y && last_x==x-1)  */
	{
		iter_count++;
		last_x=x;
		spanflag=0;
	}
	else
	{
		if(DUMP_PARAM_FILES==1)
		{
		   fprintf(FshadePreCalc,"%d\n%d %d %d %d %d ", iter_count,
				  (FetchParameter(address) & MASK_TEXTURE)>0,
				  (FetchParameter(address) & MASK_SMOOTH_SHADE)>0,
				  (FetchParameter(address) & MASK_SHADOW_FLAG)>0,
				  (FetchParameter(address) & MASK_FLAT_HIGHLIGHT)>0,
			  	  (FetchParameter(address) & MASK_TRANS) > 0 );
		}
		if(DUMP_PARAM_FILES==1 && previousPlaneWasTextured)
		{
			fprintf(FtexPreCalc,"%d\n",iter_count);
		} 
		iter_count=0;
		last_plane=address;
		last_x=x;
		last_y=y;
		spanflag=1;

	}

	previousPlaneWasTextured=(FetchParameter(address) & MASK_TEXTURE)>0; /*for texprecalc*/

	inc_address=address;

	/*
	** unpack the base colour or offsets 
	*/

	if(FetchParameter(address) & MASK_SMOOTH_SHADE)
	{
		x_offset=ToInt(FetchParameter(address+1)>>16);
		y_offset=ToInt(FetchParameter(address+1));

if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"0 0 0 0 0 0 ");

}
	}
	else
	{
		base.Red  =FetchParameter(address) & 0xff;
		base.Green=(FetchParameter(address+1)>>24 ) & 0xff;
		base.Blue =(FetchParameter(address+1)>>16 ) & 0xff;

		shadowColour=ConvertFrom16to24(FetchParameter(address+1) & 0xffff);

if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"%d %d %d %d %d %d ",(int)base.Red,(int)base.Green,(int)base.Blue,
		   (int)shadowColour.Red>>3,(int)shadowColour.Green>>3,(int)shadowColour.Blue>>3);

}

		if((FetchParameter(address) & MASK_SHADOW_FLAG) && (shadow==0))
		{
			/*
			** add in the shadow light
			*/
			
			red  =base.Red   + shadowColour.Red;
			green=base.Green + shadowColour.Green;
			blue =base.Blue  + shadowColour.Blue;

			/*clip to 255*/

			base.Red  =(red  >255) ? 255 : red;
			base.Green=(green>255) ? 255 : green;
			base.Blue =(blue >255) ? 255 : blue;


		}

	}

	/*
	** perform the texture calculation
	*/

	if(FetchParameter(address) & MASK_TEXTURE) 
	{


		/*
		** unpack the parameters 
		*/

		a=ToInt(FetchParameter(address+5));
		b=ToInt(FetchParameter(address+5)>>16);
		c=ToInt(FetchParameter(address+4));

		d=ToInt(FetchParameter(address+7));
		e=ToInt(FetchParameter(address+7)>>16);
		f=ToInt(FetchParameter(address+6));

		p=ToInt(FetchParameter(address+3));
		q=ToInt(FetchParameter(address+3)>>16);
		r=ToInt(FetchParameter(address+2));

		exp=(FetchParameter(address) & MASK_EXPONENT) >> SHIFT_EXPONENT;

		globalTrans=(FetchParameter(address) & MASK_GLOBAL_TRANS) >> SHIFT_GLOBAL_TRANS;

		flipUV=(FetchParameter(address) & MASK_FLIP_UV) >> SHIFT_FLIP_UV;

		transPass=(FetchParameter(address) & MASK_TRANS) > 0;

		texture_address=(FetchParameter(address+4) >> 16) | (FetchParameter(address+6) & 0x00ff0000);

		mapSize=(FetchParameter(address+6) & MASK_MAP_SIZE) >> SHIFT_MAP_SIZE;

		pmip.m=(FetchParameter(address+2) & MASK_PMIP_M) >> SHIFT_PMIP_M;
		pmip.e=(FetchParameter(address+2) & MASK_PMIP_E) >> SHIFT_PMIP_E;

		colourDepth=(FetchParameter(address+6) & MASK_8_16_MAPS) > 0;

		mipMapped=(FetchParameter(address+6) & MASK_MIP_MAPPED) > 0;

		col4444or555=(FetchParameter(address+6) & MASK_4444_555) > 0;

		colour=TexturePixel(x,y,a,b,c,d,e,f,p,q,r,exp,texture_address,mapSize,pmip,colourDepth,mipMapped,
							col4444or555,globalTrans,flipUV,transPass);

if(DUMP_PARAM_FILES==1)
{
	fprintf(FrawTexPixels,"%d %d %d %d\n",(int)colour.Red,(int)colour.Green,
										  (int)colour.Blue,(int)colour.Alpha);

}


		/*
		** perform the flat shading
		*/

		if((FetchParameter(address) & MASK_SMOOTH_SHADE)==0)
		{
			/*multiply by the base colour*/

			colour.Red  =((int)colour.Red  *base.Red  )>>8;
			colour.Green=((int)colour.Green*base.Green)>>8;
			colour.Blue =((int)colour.Blue *base.Blue )>>8; 


		}
		inc_address+=8;
	}
	else
	{
		colour.Red  =base.Red;
		colour.Green=base.Green;
		colour.Blue =base.Blue;
		colour.Alpha=0;

 		inc_address+=2;
	}

	if(FetchParameter(address) & MASK_SMOOTH_SHADE)
	{

		/*
		**  Non shadowed smooth shade white.
		*/

		t0=ToInt(FetchParameter(inc_address));
		t1=ToInt(FetchParameter(inc_address+1)>>16);
		t2=ToInt(FetchParameter(inc_address+1));
			
		fraction=LinearShade(t0,t1,t2,x-x_offset,y-y_offset);

   		holdCol=ConvertFrom16to24(FetchParameter(inc_address)>>16);

		holdCol.Red>>=3;
		holdCol.Green>>=3;
		holdCol.Blue>>=3;

if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"%ld %d %d %d %d ",fraction,t2,(int)holdCol.Red,
											(int)holdCol.Green,(int)holdCol.Blue);

}


        /* clamp to 0 */

	    fraction =(fraction <0) ? 0 : fraction;
	
	    /* clamp to '1' */

	    fraction =(fraction >0x10000) ? 0x10000 : fraction;

		fraction>>=8; /*reduce resolution to 9 bits*/

	  	/* multply the base colour by the fraction */

		holdCol.Red  =(holdCol.Red  *fraction)>>5;
		holdCol.Green=(holdCol.Green*fraction)>>5;
		holdCol.Blue =(holdCol.Blue *fraction)>>5;

		if(fraction==0x100)
		{
			holdCol.Red+=4;
			holdCol.Green+=4;
			holdCol.Blue+=4;
		}

		inc_address+=2;

		/*
		**  Deal with the one possible shadow light.
		*/
			
		if((FetchParameter(address) & MASK_SHADOW_FLAG) && (shadow==0))
		{

			t0=ToInt(FetchParameter(inc_address));
			t1=ToInt(FetchParameter(inc_address+1)>>16);
			t2=ToInt(FetchParameter(inc_address+1));

			fraction=LinearShade(t0,t1,t2,x-x_offset,y-y_offset);

			holdCol1=ConvertFrom16to24(FetchParameter(inc_address)>>16);

			holdCol1.Red>>=3;
			holdCol1.Green>>=3;
			holdCol1.Blue>>=3;
	
if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"%ld %d %d %d %d ",fraction,t2,(int)holdCol1.Red,
											(int)holdCol1.Green,(int)holdCol1.Blue);

}

            /* clamp to 0 */

            fraction =(fraction <0) ? 0 : fraction;
	
	        /* clamp to '1' */

	        fraction =(fraction >0x10000) ? 0x10000 : fraction;

			
			fraction>>=8; /*reduce resolution to 9 bits*/
				
			red  =(holdCol1.Red  *fraction)>>5;
			green=(holdCol1.Green*fraction)>>5;
			blue =(holdCol1.Blue *fraction)>>5;

			if(fraction==0x100)
			{
				red+=4;
				green+=4;
				blue+=4;
			}

			red  +=holdCol.Red;
			green+=holdCol.Green;
			blue +=holdCol.Blue;

				
			/*clip to 255*/

			holdCol.Red  =(red  >255) ? 255 : red;
			holdCol.Green=(green>255) ? 255 : green;
			holdCol.Blue =(blue >255) ? 255 : blue;

			inc_address+=2;


		}
        else
        {
if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"0 0 0 0 0 ");

}
        }


		if((FetchParameter(address) & MASK_TEXTURE)==0) 
		{

			colour.Red  =holdCol.Red;
			colour.Green=holdCol.Green;
			colour.Blue =holdCol.Blue;
			colour.Alpha=0;


		}
		else
		{
			/* multply the colour by the holdCol */

			colour.Red  =((int)colour.Red  *holdCol.Red)>>8;
			colour.Green=((int)colour.Green*holdCol.Green)>>8;
			colour.Blue =((int)colour.Blue *holdCol.Blue)>>8;

		}

	}
    else
    {
if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"0 0 0 0 0 0 0 0 0 0 ");

}

	}
   	/*
	**  Deal with the flat highlights.
	*/

    if(FetchParameter(address) & MASK_FLAT_HIGHLIGHT)
    {
		highlightCol      =ConvertFrom16to24(FetchParameter(inc_address)>>16);
		shadowHighlightCol=ConvertFrom16to24(FetchParameter(inc_address));

		highlightCol.Red=  highlightCol.Red>>3;
		highlightCol.Green=highlightCol.Green>>3;
		highlightCol.Blue= highlightCol.Blue>>3;
        
		shadowHighlightCol.Red=  shadowHighlightCol.Red>>3;
		shadowHighlightCol.Green=shadowHighlightCol.Green>>3;
		shadowHighlightCol.Blue= shadowHighlightCol.Blue>>3;


if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"%d %d %d %d %d %d ",(int)highlightCol.Red>>3,(int)highlightCol.Green>>3,
											   (int)highlightCol.Blue>>3,(int)shadowHighlightCol.Red>>3,
											   (int)shadowHighlightCol.Green>>3,(int)shadowHighlightCol.Blue>>3);

}		
		if((FetchParameter(address) & MASK_SHADOW_FLAG) && (shadow==0))
		{
			/* add in the shadow light's highlight */
			
			red  =highlightCol.Red  +shadowHighlightCol.Red;
			green=highlightCol.Green+shadowHighlightCol.Green;
			blue =highlightCol.Blue +shadowHighlightCol.Blue;

			/*clip to 31*/

			highlightCol.Red  =(red  >31) ? 31 : red;
			highlightCol.Green=(green>31) ? 31 : green;
			highlightCol.Blue =(blue >31) ? 31 : blue;

		}

		/*add in the highlight*/
		
		red  =colour.Red  +(highlightCol.Red<<3);
		green=colour.Green+(highlightCol.Green<<3);
		blue =colour.Blue +(highlightCol.Blue<<3);

   		/*clip to 255*/

   		colour.Red  =(red  >255) ? 255 : red;
   		colour.Green=(green>255) ? 255 : green;
   		colour.Blue =(blue >255) ? 255 : blue;

		inc_address+=1;
	}
    else
    {
if(DUMP_PARAM_FILES==1 && spanflag)
{
	fprintf(FshadePreCalc,"0 0 0 0 0 0 ");

}

    }


	/* now interpolate with the fog colour */
	if((FetchParameter(address) & MASK_DISABLE_FOG) == 0)
	{

	/*	colour.Red   =( (colour.Red  *(256-fog)) + (fogColour.Red  *fog) )>>8;
		colour.Green =( (colour.Green*(256-fog)) + (fogColour.Green*fog) )>>8;
		colour.Blue  =( (colour.Blue *(256-fog)) + (fogColour.Blue *fog) )>>8; */
	
		red  =((int)fogColour.Red  -colour.Red  )*fog;
		green=((int)fogColour.Green-colour.Green)*fog;
		blue =((int)fogColour.Blue -colour.Blue )*fog;

		colour.Red  =(red  >>8)+(int)colour.Red;
		colour.Green=(green>>8)+(int)colour.Green;
		colour.Blue =(blue >>8)+(int)colour.Blue; 
	}

	if((FetchParameter(address) & MASK_TRANS) > 0)
	{
		/* make 15 completely translucent */

		if(colour.Alpha==15)
			colour.Alpha=16;

		/*interpolate with the colour in the frame store*/

/*		THIS IS THE OLD WAY WE DID THE ALPHA BLENDING
		red  =(int)frameBuffer[y][x].Red  *colour.Alpha + (16-colour.Alpha)*colour.Red;
		green=(int)frameBuffer[y][x].Green*colour.Alpha + (16-colour.Alpha)*colour.Green;
		blue =(int)frameBuffer[y][x].Blue *colour.Alpha + (16-colour.Alpha)*colour.Blue; 

		colour.Red  =red  >>4;
		colour.Green=green>>4;
		colour.Blue =blue >>4;
*/

		red  =( (int)frameBuffer[y*Bitmap_xdim + x].Red  *colour.Alpha )>>4;
		green=( (int)frameBuffer[y*Bitmap_xdim + x].Green*colour.Alpha )>>4;
		blue =( (int)frameBuffer[y*Bitmap_xdim + x].Blue *colour.Alpha )>>4;

		red  +=( (int)(16-colour.Alpha)*colour.Red   )>>4; 
		green+=( (int)(16-colour.Alpha)*colour.Green )>>4;
		blue +=( (int)(16-colour.Alpha)*colour.Blue  )>>4;

		colour.Red  =red;
		colour.Green=green;
		colour.Blue =blue;
	}		

if(DUMP_PARAM_FILES==1)
{

        fprintf(Fpixels,"%d %d %d\n",(int)colour.Red,(int)colour.Green,(int)colour.Blue);


}
/*	   	if(lastMiss!=missCount)
	   	{
			colour.Red  =255;
			colour.Green=255;
			colour.Blue =255;
	   		lastMiss=missCount;
	   	} */

	frameBuffer[y*Bitmap_xdim + x].Red  =colour.Red;
	frameBuffer[y*Bitmap_xdim + x].Green=colour.Green;
   	frameBuffer[y*Bitmap_xdim + x].Blue =colour.Blue; 
}

