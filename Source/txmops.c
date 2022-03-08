/******************************************************************************
 * Name         : txmops.c
 * Title        : Performs all the operations on texture pixels.
 * Author       : Peter Leaback
 * Created      : 23/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: txmops.c,v $
 * Revision 1.94  1998/01/27  16:18:08  dm
 * made sgl_create_texture only create texture memory if pixel_data
 * is NULL.
 *
 * Revision 1.93  1997/11/17  10:40:54  sxy
 * Added extra check for API bad parameters.
 *
 * Revision 1.92  1997/10/16  12:52:19  sxy
 * deliberatly over wrote previous version, this is why there is a version number missing.
 *
 * Revision 1.90  1997/09/29  10:59:28  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.89  1997/09/24  10:17:18  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.88  1997/08/12  13:24:42  sxy
 * Made arbitrary format textures work in sgl_direct_set_texture().
 *
 * Revision 1.87  1997/08/08  16:15:53  sxy
 * Make more texture formats work in sgl_direct_set_texture, and clean up the unused
 * code.
 *
 * Revision 1.86  1997/07/18  14:04:46  gdc
 * changed pvrosprintf to DPF
 *
 * Revision 1.85  1997/07/16  13:29:28  gdc
 * moved write pixels functions here and made them static
 *
 * Revision 1.84  1997/07/14  19:36:03  mjg
 * Added PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.83  1997/06/12  14:06:01  sxy
 * restore the features of dither and 4x4 filtered Mipmap.
 *
 * Revision 1.82  1997/06/10  16:50:56  sxy
 * added the feature that if any level of filterd_maps[] is missing, call
 * AutoMipmap to generate the rest of them.
 *
 * Revision 1.81  1997/06/09  17:19:44  sxy
 * moved AutoMipmap into texapi.c.
 *
 * Revision 1.80  1997/06/05  14:37:24  sxy
 * added fast palettised texture loading in sgl_direct_set_texture.
 *
 * Revision 1.80  1997/06/05  14:11:08  sxy
 * added fast palettised texture loading in sgl_direct_set_texture.
 *
 * Revision 1.79  1997/05/16  11:33:51  sxy
 * sorry, made a mistake.
 *
 * Revision 1.78  1997/05/16  11:27:57  sxy
 * tidy up.
 *
 * Revision 1.77  1997/05/16  10:35:48  sxy
 * the problem of texture memory running out may be fixed.
 *
 * Revision 1.76  1997/05/15  11:02:28  mjg
 * Removed a couple of unused externs.
 * ,
 *
 * Revision 1.75  1997/05/13  17:56:57  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.74  1997/05/13  17:14:08  sxy
 * 1. using TextureCopy to copy preprocessed textures into memory.
 * 2. changed the Pitch values from pixels to bytes.
 * 3. using new texture API in sgl_set_texture_extended.
 *
 * Revision 1.73  1997/05/10  13:47:18  mjg
 * Removed a dodgy PVROSFreeo.
 *
 * Revision 1.72  1997/05/08  15:33:58  sxy
 * replaced sgl_preprocess_texture with AutoMipmap and TextureLoad
 * in sgl_set_texture.
 *
 * Revision 1.71  1997/05/08  13:51:57  sxy
 * Fixed a bug in sgl_set_texture.
 *
 * Revision 1.70  1997/05/08  13:06:15  sxy
 * Fixed the translucency problem in AutoMipmap.
 *
 * Revision 1.69  1997/05/08  08:19:27  sxy
 * get AutoMipmap working.
 *
 * Revision 1.68  1997/05/06  11:26:08  jop
 * Added a hacky mipmapper and fixed some bugs
 *
 * Revision 1.67  1997/05/02  16:36:27  jop
 * Reconfigure to use global texture api
 *
 * Revision 1.66  1997/05/02  15:14:47  gdc
 * removed call to get tex interface, and put in an extren to refernce the
 * interface which is now set extternally
 *
 * Revision 1.65  1997/05/01  16:28:53  mjg
 * Changes for PVROSification:
 * 1. Added a function to get the Texture API Inferface.
 * 2. All Texture... calls are made through said Interface.
 * 3. Replaced UINT defintions as sgl_uint.
 *
 * Revision 1.64  1997/05/01  02:42:42  jop
 * A faint heart never won a fair maiden
 *
 * Revision 1.63  1997/04/30  19:26:12  mjg
 * Rolled in new Texture API changes.
 *
 * Revision 1.62  1997/04/29  14:53:48  erf
 * 1. Added new PVROS API function call mechanism.
 * 2. Changed the operation of TMalloc() and TFree().
 *
 * Revision 1.61  1997/04/25  16:35:53  mjg
 * TFree, TMalloc and Texture heaps - still copious compiler warnings.
 *
 * Revision 1.60  1997/04/24  21:03:13  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.59  1997/04/14  16:10:46  pdl
 * corrected the x_dim in the preprocessed format.
 *
 * Revision 1.58  1997/04/10  20:07:04  mjg
 * Removed 'dvregion.h' include and all the compiler warnings.
 *
 * Revision 1.57  1997/04/09  08:22:37  sxy
 * Fixed a potential bug in HandleGreatBiggyTexture.
 *
 * Revision 1.56  1997/04/03  17:54:49  mjg
 * Included hwtexas.h for DOSing.
 *
 * Revision 1.55  1997/04/02  21:30:29  mjg
 * Updated calling conventions.
 *
 * Revision 1.54  1997/04/02  12:42:40  mjg
 * Changed printf to PVROSPrintf.
 *
 * Revision 1.53  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.52  1997/03/15  12:54:31  jop
 * Added sgl.h include for sgl_direct* types
 *
 * Revision 1.51  1997/03/14  11:06:53  jwf
 * changed some stuff to make it compile on the macintosh
 *
 * Revision 1.50  1997/03/13  11:47:55  sxy
 * Added parameter MemoryRoot to TMalloc() and TFree().
 *
 * Revision 1.49  1997/01/29  10:58:41  sjf
 * Ooops! Messed up the parameter checking in direct texture setting.
 *
 * Revision 1.48  1997/01/28  16:32:02  sjf
 * Added 1555 and 555 specific formats for sgl_direct_set_texture
 *
 * Revision 1.47  1997/01/28  10:55:04  mjg
 * SJF here pretending to be Mike... Fixed silly bug
 * with direct load of non-MIP textures on Midas1/2/3.
 *
 * Revision 1.46  1997/01/24  12:25:43  jwh
 * Added a couple more direct FN's for texture stuff,
 * not complete so DON'T use them !
 *
 * Revision 1.45  1997/01/17  14:13:19  jwh
 * Added extra param to sgl_direct_set_texture for enum src
 * types.
 *
 * Revision 1.44  1997/01/16  17:27:33  jwh
 * Added opaque<->trans type conversion to sgl_direct_set_texture.
 *
 * Revision 1.43  1996/12/10  23:01:35  erf
 * Changed #ifdef PCX1 to #if PCX1 || PCX2.
 *
 * Revision 1.42  1996/11/26  22:27:11  sjf
 * Fixed incorrect count of pixel pairs in..... well, i'll leave that
 * to you to guess.
 *
 * Revision 1.41  1996/11/26  19:25:41  sjf
 * Ooops. Forgot about the HW tex memory bug. The direct fn now
 * makes sure the render has completed b4 copying the texture data.
 * Still hasn't been tested tho'. :)
 *
 * Revision 1.40  1996/11/26  19:11:28  sjf
 * 1) Fixed some glaring errors in the sgl_direct_set_texture etc routines
 *    BUT it still hasn't been tested
 * 2) Added some comments (gosh. Aren't I good)
 *
 * Revision 1.39  1996/11/22  17:59:37  sjf
 * Added "sgl_direct_set_texture" for better texture support in
 * D3D.
 * WARNING WARNING WARNING. This compiles, but that's probably it.
 * I have done no testing WHATSOEVER.
 *
 * Revision 1.38  1996/11/21  17:11:06  jop
 * Added extended texture set routine
 *
 * Revision 1.37  1996/10/28  17:45:12  sjf
 * Fixed stupid bug in sgl_set_texture. It wasn't correctly reading the
 * pre-processed texture header, and hence aborting with "bad parameter" error.
 *
 * Revision 1.36  1996/09/09  12:49:18  jrc
 * Added sgl_free_pixels function.
 *
 * Revision 1.35  1996/08/30  13:51:24  mjg
 * changed write pixel to writetwopixels.
 *
 * Revision 1.34  1996/08/09  11:36:25  jop
 * Fixed bug in sgl_set_texture
 *
 * Revision 1.33  1996/07/19  19:35:17  jop
 * Moved sglinit to more sensible place
 *
 * Revision 1.32  1996/07/17  14:01:21  sjf
 * 1) Got rid of the stuff to read back texels - it wasn't being used anywhere
 * 2) Made the Twiddle stuff conditional on it being a Simulator build
 *    as I threatened to do last check in.
 *
 * Revision 1.31  1996/07/17  11:51:32  sjf
 * 1) Got rid of the inverse twiddling .. We no longer require this. This
 *    gets rid of a 1k array at the same time (which can't be bad)
 * 2) Reduced the size of the twiddle array by changing to UINT16. This
 *    should also help. I'm not entirely sure, but we MAY be able to eliminate
 *    this array entirely when we have a non-simulator build.
 *
 * Revision 1.30  1996/06/27  13:09:12  sjf
 * Changed the nonMIP mapped texture loading to use the fast method.
 *
 * Revision 1.29  1996/06/27  12:10:54  sjf
 * Removed old code that copied to texture memory.
 *
 * Revision 1.28  1996/06/25  16:38:11  sjf
 * Added WriteNPixelpairs  to speed up texture caching.
 *
 * Revision 1.27  1996/06/17  19:12:06  jop
 * MAX_CAHED_TEXTURES now increased, and we now don't bother
 * reporting cached textures non-used, non-loaded textures
 *
 * Revision 1.26  1996/06/07  15:59:04  sjf
 * Writing to texture memory while rendering is a complete No No,
 * (at least on TSP A&B and PCX1) so I've put in a guard to wait until
 * rendering is complete.
 *
 * Revision 1.25  1996/05/30  21:00:18  sjf
 * Added texture free space to the texture callback function.
 *
 * Revision 1.24  1996/05/30  20:33:55  sjf
 *  Added texture caching.
 *
 * Revision 1.23  1996/05/24  17:42:17  sjf
 * Made a twiddle call inline.
 *
 * Revision 1.22  1996/05/09  17:36:52  jop
 * Killed PCX1 texture mem reads hopefully
 *
 * Revision 1.21  1996/04/30  18:22:40  sjf
 * Fixed a bug with my pre-processing that was found by Diamond when they
 * compiled the code under Microsoft Visual C (HOW EMBARRASING). I didnt
 * have the internal pre-processed structure long enough.
 *
 * Revision 1.20  1996/04/19  17:23:38  pdl
 * removed a return.
 *
 * Revision 1.19  1996/04/15  11:41:59  pdl
 * added some PCX1 support
 *
 * Revision 1.18  1996/03/19  14:49:10  jrc
 * sgl_create_texture now returns the name rather than the texture size (bug in
 * a recent revision).
 *
 * Revision 1.17  1996/03/18  15:25:25  sjf
 * 1) Got rid of a debugging routine
 * 2) Changed sgl_preprocess_texture  so that it returned the size
 *    in bytes of the preprocessed pixel data AND made it store
 *    the error value (a V minor bug fix :)  )
 *
 * Revision 1.16  1996/03/14  13:45:37  sjf
 * changed opacity to alpha, as it described the opposite of what it should be.
 *
 * Revision 1.15  1996/03/07  17:16:10  sjf
 * Changed the halve resolution routine to one that now does some
 * filtering. This should improve the look of MIP maps to some extent.
 *
 * Revision 1.14  1996/02/19  17:19:29  sjf
 * OOPS fixed some cut and paste bugs where "type" should have been "size".
 *
 * Revision 1.13  1996/02/19  16:46:53  sjf
 * 1) Added sgl_texture_size function, and
 * 2) Did a more sensible ID for the preprocessed maps.
 *
 * Revision 1.12  1996/02/19  11:30:51  sjf
 * Rewrote the texture processing code to produce a pre-processed texture map
 * so that it uses this internally, or can be saved out (to file) by the user
 * and re-read in later. This should vastly increase the speed of loading
 * texture maps.
 *
 * Revision 1.11  1995/12/05  10:19:04  pdl
 * coded the PCX1 textures.
 *
 * Revision 1.10  1995/11/09  12:14:33  pdl
 * removed the forced mip mappping..
 *
 * Revision 1.9  1995/11/09  11:55:34  pdl
 * optimised the texture loading
 * added FFT code (but dormant)
 *
 * Revision 1.8  1995/08/23  19:47:34  sjf
 * Fixed bug in dithering of translucent mip maps.
 *
 * Revision 1.7  1995/08/23  17:29:27  pdl
 * fixed the opacity dithering.
 *
 * Revision 1.6  1995/08/22  15:13:24  pdl
 * changed the interface to InternalGetTexel.
 *
 * Revision 1.5  1995/08/22  11:41:13  pdl
 * added InternalGetTexel.
 *
 * Revision 1.4  1995/08/09  12:24:38  pdl
 * added SetupTransPixel.
 *
 * Revision 1.3  1995/07/31  17:26:28  pdl
 * added sgl_set_texture.
 *
 * Revision 1.2  1995/07/26  15:24:23  sjf
 * Stopped GCC's over fussy warnings.
 *
 * Revision 1.1  1995/07/17  16:04:25  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_TXMOPS

#include <math.h>
#include "sgl.h"
#include "sgl_defs.h"
#include "sgl_init.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "txmops.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "hwinterf.h"
#include "texapi.h"

#if DOS32
#include "hwtexas.h"
#endif

extern HLDEVICE 	gHLogicalDev;
extern PTEXAPI_IF 	gpTextureIF;

/*
// Define the Offsets for for the various levels in a MIP map. Note that
// there are currently two different sets: One for Texas 1, and the other
// for PCX1 and external File format.
//
// The offsets are expressed as 16bit pixel positions.
*/

static const sgl_uint32 TEX1MapOffset[]= { 0x0000,0x0001,0x0005,
									   0x0015,0x0055,0x0155,
									   0x0555,0x1555,0x5555};

/* External & PCX1 */
static const sgl_uint32 EXTMapOffset[]= { 0x0001,0x0002,0x0006,
									   0x0016,0x0056,0x0156,
									   0x0556,0x1556,0x5556};

/*
// Define one or the other as the "internal" format depending on
// the compile flag settings
*/
#if PCX1 || PCX2 || PCX2_003
#define  MAP_OFFSET EXTMapOffset
#else
#define  MAP_OFFSET TEX1MapOffset
#endif

/*
// The following is the "raw" size of the texture data for Non MIP and
// MIP mapped textures.
// NOTE: For the MIP mapped textures IT DOES NOT include the space used
// to pad out the textures so that they align on page boundaries.
// IT DOES however, account for the fact that PCX1 textures are "one pixel"
// larger than PVR1, ones, by choosing the larger of the two.
*/
static const sgl_uint32 MapLength[2][4]={
						 /* Non-mip mapped in bytes*/
						{0x00800,0x02000,0x08000,0x20000},
						 /* Mip mapped in bytes*/
						{0x00AAC,0x02AAC,0x0AAAC,0x2AAAC} 
				   			  		  };
/*
// Specify how much padding (in BYTES) is used to get better page alignment.
// PLUS specify the alignment offset IN sgl_uint16s. 
*/
#define ALIGNMENT_PADDING_U8s  (340)
#define ALIGNMENT_OFFSET_U16s  (170)



/*
// Bit Twiddling variables. These are used to arrange the pixels
// in the "Z" format used by "texas".
//
// Only needed for the Simulators
*/
#if SIMULATOR
static sgl_uint16 twiddle[256];	 /*the twiddle lookup table*/
#endif


/*********************************************************/
/**************  TEXTURE CACHING STRUCTURES **************/
/*********************************************************/

/*
// Define the structure that stores a cached texture data (not the actual
// pixels tho'
*/
typedef struct tagCACHED_TEXT_STRUCT_TYPE
{
	/*
	// The id's used by the user, and the SGL name
	//
	// If this entry is unused, the Sgl name contains INVALID name.
	*/
	int sglName;
	long UserId;

	/*
	// To make finding new free entrys faster, keep a pointer to the
	// next free entry (if this one itself is free). A null pointer
	// otherwise
	*/
	struct tagCACHED_TEXT_STRUCT_TYPE * pNextFree;

	/*
	// When the texture is loaded, we store the name of a normal,
	// non-cacheable texture. If it isnt, we put in the invalid name
	// constant.
	*/
	int loadedName;


	/*
	// Values stored when traversing the display list
	*/
	sgl_bool bTextureNeeded;
	sgl_bool bUsedLastRender;
	int		 size;

}CACHED_TEXT_STRUCT_TYPE;



/*
// create an array of them
*/
#define MAX_CACHED_TEXTURES (10000)
static CACHED_TEXT_STRUCT_TYPE CachedTextures[MAX_CACHED_TEXTURES];

/*
// Keep a pointer to where the next free entry in this table is, PLUS
// how many are actually in use.
*/
static CACHED_TEXT_STRUCT_TYPE * pFreeCTEntry;
int nCachedTextures;

/*
// Data registered with the call back routine
*/
static sgl_tex_callback_func TextureCallBackFunction;
static sgl_tex_callback_struct *UserTexDataArray;
static int					   UserTexDataSize;

static void delete_cached_texture(int 	name);

/*********************************************************/
/*********************************************************/
/*********************************************************/
/*********************************************************/


/*
// Optional FFT filtering code.
*/

#ifdef FFT_FILTER

typedef struct
	float *red;
	float *green;
	float *blue;
	float *alpha;
}FFT;

static int OrigSize;
static FFT SafeImage,WorkingImage;

/******************************************************************************
 * Function Name: fourn
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : This is the N dimentional FFT from Numerical recipies
 *****************************************************************************/
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

static void fourn(float data[], unsigned long nn[], int ndim, int isign)
{
	int idim,i;
	unsigned long i1,i2,i3,i2rev,i3rev,ip1,ip2,ip3,ifp1,ifp2;
	unsigned long ibit,k1,k2,n,nprev,nrem,ntot;
	float tempi,tempr;
	double theta,wi,wpi,wpr,wr,wtemp;

	for (ntot=1,idim=1;idim<=ndim;idim++)
		ntot *= nn[idim];
	nprev=1;
	for (idim=ndim;idim>=1;idim--) {
		n=nn[idim];
		nrem=ntot/(n*nprev);
		ip1=nprev << 1;
		ip2=ip1*n;
		ip3=ip2*nrem;
		i2rev=1;
		for (i2=1;i2<=ip2;i2+=ip1) {
			if (i2 < i2rev) {
				for (i1=i2;i1<=i2+ip1-2;i1+=2) {
					for (i3=i1;i3<=ip3;i3+=ip2) {
						i3rev=i2rev+i3-i2;
						SWAP(data[i3],data[i3rev]);
						SWAP(data[i3+1],data[i3rev+1]);
					}
				}
			}
			ibit=ip2 >> 1;
			while (ibit >= ip1 && i2rev > ibit) {
				i2rev -= ibit;
				ibit >>= 1;
			}
			i2rev += ibit;
		}
		ifp1=ip1;
		while (ifp1 < ip2) {
			ifp2=ifp1 << 1;
			theta=isign*6.28318530717959/(ifp2/ip1);
			wtemp=sin(0.5*theta);
			wpr = -2.0*wtemp*wtemp;
			wpi=sin(theta);
			wr=1.0;
			wi=0.0;
			for (i3=1;i3<=ifp1;i3+=ip1) {
				for (i1=i3;i1<=i3+ip1-2;i1+=2) {
					for (i2=i1;i2<=ip3;i2+=ifp2) {
						k1=i2;
						k2=k1+ifp1;
						tempr=(float)wr*data[k2]-(float)wi*data[k2+1];
						tempi=(float)wr*data[k2+1]+(float)wi*data[k2];
						data[k2]=data[k1]-tempr;
						data[k2+1]=data[k1+1]-tempi;
						data[k1] += tempr;
						data[k1+1] += tempi;
					}
				}
				wr=(wtemp=wr)*wpr-wi*wpi+wr;
				wi=wi*wpr+wtemp*wpi+wi;
			}
			ifp1=ifp2;
		}
		nprev *= n;
	}


	/*this is pete's modification */


	if(isign<0)
	{
		wtemp=1.0/ntot;
		for(i=1;i<=nn[1]*nn[2]*2;i++)
			data[i]*=wtemp;
	}

}
#undef SWAP
/* (C) Copr. 1986-92 Numerical Recipes Software 0N%. */


static void FilterMul(float *array,int size,int x,int y,float value)
{
  	array[x*2+y*2*size]*=value;
	array[1+x*2+y*2*size]*=value;

	if(x>0)
   	{
  		array[(size-x)*2+y*2*size]*=value;
   		array[1+(size-x)*2+y*2*size]*=value;
	}

   	if(y>0)
	{
		array[x*2+(size-y)*2*size]*=value;
		array[1+x*2+(size-y)*2*size]*=value;
	}

	if(y>0 && x>0)
	{
		array[(size-x)*2+(size-y)*2*size]*=value;
		array[1+(size-x)*2+(size-y)*2*size]*=value;
	}
}

/******************************************************************************
 * Function Name: Filter
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : This is truncates the image in the frequency domain.
 *****************************************************************************/
static void	FilterFFT(FFT image,int corner)
{
	int	x,y;
	int size;
	int Maximum;
	int halfc;
	float value,fraction;

	size=OrigSize;
	

	halfc=(corner+1)/2;
	fraction=3.141592654f/2.0f;

	if(corner>0)
	{
		fraction=fraction/(float)halfc;

	   	for(y=0;y<=size/2;y++)
		{
			for(x=0;x<=size/2;x++)
			{
				if(x > corner || y > corner)
				{
					FilterMul(image.red,size,x,y,0.0);
					FilterMul(image.green,size,x,y,0.0);
					FilterMul(image.blue,size,x,y,0.0);

			  	}
if(0)
{
				if(x > halfc && x <= corner && y <= halfc)
				{
					value=cos((x-halfc)*fraction);
					FilterMul(image.red,size,x,y,value);
					FilterMul(image.green,size,x,y,value);
					FilterMul(image.blue,size,x,y,value);
				}

				if(y > halfc && y <= corner && x <= halfc)
				{
					value=cos((y-halfc)*fraction);
					FilterMul(image.red,size,x,y,value);
					FilterMul(image.green,size,x,y,value);
					FilterMul(image.blue,size,x,y,value);
				}

				if(x > halfc && y > halfc && x <= corner && y <= corner)
				{
					Maximum = (x > y ) ? x : y;
					value=cos((Maximum-halfc)*fraction);
					FilterMul(image.red,size,x,y,value);
					FilterMul(image.green,size,x,y,value);
					FilterMul(image.blue,size,x,y,value);
				}
}
			}
		}
	}
	else
	{
	   	for(y=0;y<=size/2;y++)
		{
			for(x=0;x<=size/2;x++)
			{
				if(x > corner || y > corner)
				{
					FilterMul(image.red,size,x,y,0.0);
					FilterMul(image.green,size,x,y,0.0);
					FilterMul(image.blue,size,x,y,0.0);

			  	}
			}
		}
	}

}

/******************************************************************************
 * Function Name: CopyImage
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : 
 *****************************************************************************/

static void	CopyImage(FFT from,FFT to)
{
	int	i;
	int size;

	size=OrigSize;

	for(i=0;i<size*size*2;i++)
	{
		*to.red++=*from.red++;
		*to.green++=*from.green++;
		*to.blue++=*from.blue++;
		*to.alpha++=*from.alpha++;
	}
}



/******************************************************************************
 * Function Name: FromSglToFloat
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : 
 *****************************************************************************/
static void	FromSglToFloat(sgl_intermediate_map *PixelData,FFT image)
{
	int	i;
	int size;

	size=OrigSize;

	for(i=0; i<size*size ;i++)
	{
		image.red[i*2]=(float)PixelData->pixels[i].red;	/*real part	*/
		image.red[i*2+1]=0.0;						/*imaginary part*/

		image.green[i*2]=(float)PixelData->pixels[i].green;
		image.green[i*2+1]=0.0;

		image.blue[i*2]=(float)PixelData->pixels[i].blue;
		image.blue[i*2+1]=0.0;

		image.alpha[i*2]=(float)PixelData->pixels[i].alpha;
		image.alpha[i*2+1]=0.0;
	}

} 

static void	ConvertToFD(FFT image)
{
	unsigned long dim[3];
	int size;
	
	size=OrigSize;

	dim[1]=size;     /*dimensions for the FFT*/
	dim[2]=size;

	/*transform to the frequency domain*/

	fourn(image.red-1,dim,2,1);
	fourn(image.green-1,dim,2,1);
	fourn(image.blue-1,dim,2,1); 
	fourn(image.alpha-1,dim,2,1); 

}

static void	ConvertFromFD(FFT image)
{
	unsigned long dim[3];
	int size;

	size=OrigSize;

	dim[1]=size;     /*dimensions for the FFT*/
	dim[2]=size;

	/*transform from the frequency domain*/

   	fourn(image.red-1,dim,2,-1);
   	fourn(image.green-1,dim,2,-1);
   	fourn(image.blue-1,dim,2,-1); 
	fourn(image.alpha-1,dim,2,-1); 


}

/******************************************************************************
 * Function Name: clip
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : 
 *****************************************************************************/
static float	clip(float in)
{
	if(in<0)
		return 0;
	if(in>255)
		return 255;
	return in;
}


/******************************************************************************
 * Function Name: FromFloatToSgl
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : 
 *****************************************************************************/
static void	FromFloatToSgl(sgl_intermediate_map *PixelData,FFT image,int size)
{
	int	x,y;

	int Skip=OrigSize/size;


	for(x=0;x<size;x++)
		for(y=0;y<size;y++)
		{
		/*ignore the imaginary part*/

			PixelData->pixels[y*size + x].red	=clip(image.red[(y*Skip*OrigSize + x*Skip)*2]);
			PixelData->pixels[y*size + x].green	=clip(image.green[(y*Skip*OrigSize + x*Skip)*2]);
			PixelData->pixels[y*size + x].blue	=clip(image.blue[(y*Skip*OrigSize + x*Skip)*2]);
			PixelData->pixels[y*size + x].alpha=clip(image.alpha[(y*Skip*OrigSize + x*Skip)*2]);

		}

}

static sgl_bool	InitFFT(sgl_intermediate_map *PixelData,int size)
{
	OrigSize=size;

	SafeImage.red		= SGLMalloc(size*size*2*4);
	SafeImage.green		= SGLMalloc(size*size*2*4);
	SafeImage.blue		= SGLMalloc(size*size*2*4);
	SafeImage.alpha		= SGLMalloc(size*size*2*4);

	WorkingImage.red	= SGLMalloc(size*size*2*4);
	WorkingImage.green	= SGLMalloc(size*size*2*4);
	WorkingImage.blue	= SGLMalloc(size*size*2*4);
	WorkingImage.alpha	= SGLMalloc(size*size*2*4);

	if(	SafeImage.red==NULL || 
		SafeImage.green==NULL || 
		SafeImage.blue==NULL || 
		SafeImage.alpha==NULL ||
	   	WorkingImage.red==NULL || 
		WorkingImage.green==NULL || 
		WorkingImage.blue==NULL || 
		WorkingImage.alpha==NULL)
	{
		/*deallocate what we have allocated, and return with an error*/

		if(SafeImage.red)
			SGLFree(SafeImage.red);
		if(SafeImage.green)
			SGLFree(SafeImage.green);
		if(SafeImage.blue)
			SGLFree(SafeImage.blue);
		if(SafeImage.alpha)
			SGLFree(SafeImage.alpha);

		if(WorkingImage.red)
			SGLFree(WorkingImage.red);
		if(WorkingImage.green)
			SGLFree(WorkingImage.green);
		if(WorkingImage.blue)
			SGLFree(WorkingImage.blue);
		if(WorkingImage.alpha)
			SGLFree(WorkingImage.alpha);

		return(0);
	}

	FromSglToFloat(PixelData,SafeImage);
	ConvertToFD(SafeImage);

   	return(1);

}

static void	FinishFFT()
{

	SGLFree(SafeImage.red);
	SGLFree(SafeImage.green);
	SGLFree(SafeImage.blue);
	SGLFree(SafeImage.alpha);

	SGLFree(WorkingImage.red);
	SGLFree(WorkingImage.green);
	SGLFree(WorkingImage.blue);
	SGLFree(WorkingImage.alpha);

}
#endif /*FFT_FILTER*/


/*
// Defines for X & Y twiddling, so that we can increment the each easiliy
*/
#define INCREMENT_X_TWIDDLE 	0x5556L
#define INCREMENT_BY4_X_TWIDDLE 0x5560L
#define X_TWIDDLE_MASK			0xAAAAL

#define INCREMENT_Y_TWIDDLE 	0xAAABL
#define INCREMENT_BY4_Y_TWIDDLE 0xAAB0L
#define Y_TWIDDLE_MASK			0x5555L



#if SIMULATOR
/*=========================================================================
name	|InitTwiddle
function|Initialises the bit twiddling table (used for texture indexing)
		|11111111 becomes 101010101010101.Also it initialised the inverse
		|bit twiddling table,giving a y3y2y1y0x3x2x1x0 from an 8 bit twid.
in		|-
out		|-
rd		|-
wr		|twiddle
		|INVtwiddle
pre		|-
post	|-		 
=========================================================================*/

void InitTwiddle()
{
	int i;

	for(i=0;i<256;i++)
	{	
		twiddle[i] =  i     & 1;
		twiddle[i]|=((i>>1)	& 1)<< 2;
		twiddle[i]|=((i>>2)	& 1)<< 4;
		twiddle[i]|=((i>>3)	& 1)<< 6;
		twiddle[i]|=((i>>4)	& 1)<< 8;
		twiddle[i]|=((i>>5)	& 1)<< 10;
		twiddle[i]|=((i>>6)	& 1)<< 12;
		twiddle[i]|=((i>>7)	& 1)<< 14;
	}
}

/*=========================================================================
name	|Twid
function|given an x,y coordinate, generate the memory offset using 
		|twiddling
in		|x, the x coordinate
		|y, the y coordinate
out		|address, the memory offset
rd		|twiddle
wr		|-
pre		|0<x<256
		|0<y<256
		|InitTwiddle has been called.
post	|-		 
=========================================================================*/

unsigned long Twid(int x,int y)
{
	unsigned long address;
	
	address=(twiddle[x]<<1) | twiddle[y];

	return(address);
}

#endif

/******************************************************************************
 * Function Name: Pack32ToTexas16
 *
 * Inputs       : Intermediate map of sufficient size
 *				  Map dimension
 *				  Map Type
 *				  
 * Outputs      : 16 bit/pixel texture data
 * Returns      : None
 * Globals Used : 
 *
 * Description  : This performs the truncation, packing and twiddling from RGBA
 *				  quad-bytes, and places the result in the location pointed to...
 *				  
 *****************************************************************************/
static void Pack32ToTexas16( sgl_uint16 * pOutputPixels,
			const sgl_intermediate_map *  pSrcMap,
							const int	  MapDim,
				const sgl_map_types		  MapType)
{
	int x,y;
	sgl_uint32 PixelVal;

	int yTwiddlePart;
	int xTwiddlePart;

	sgl_map_pixel *pSrcPixel, *pSrcRow;

	ASSERT(pSrcMap->x_dim >= MapDim);
	ASSERT(pSrcMap->y_dim >= MapDim);

	/*
	// Set up pointer to the first source row
	*/
	pSrcRow = pSrcMap->pixels;


	/*
	// Determine the type of the Map. IE is it translucent or opaque
	//
	// If opaque...
	*/
	if((MapType==sgl_map_16bit) || (MapType==sgl_map_16bit_mm))
	{
		/*
		// Initialise the Y part of the twiddle
		*/
		yTwiddlePart = 0;

		for(y = MapDim; y != 0 ; y--)
		{
			/*
			// Initialise the X twiddle part
			*/
			xTwiddlePart = 0;

			/*
			// Set up pointer to the first pixel in the source row.
			*/
			pSrcPixel = pSrcRow;

			/*
			// Step through the row
			*/
			for(x = MapDim; x != 0 ; x--)
			{
				/*
				// Compute the 16 bit pixel value
				*/
				PixelVal  = (pSrcPixel->red  & 0xf8) << 7;
				PixelVal |= (pSrcPixel->green& 0xf8) << 2;
				PixelVal |=  pSrcPixel->blue >> 3;

				/*
				// Save this result at the "twiddled" position.
				*/
				pOutputPixels[xTwiddlePart | yTwiddlePart] = (sgl_uint16) PixelVal;

				/*
				// Go to the next pixel
				*/
				pSrcPixel++;

				/*
				// "increment" the x twiddle. Note only every 2nd bit is used
				*/
				xTwiddlePart= (xTwiddlePart+INCREMENT_X_TWIDDLE) & X_TWIDDLE_MASK;


			}/*end for x*/

			/*
			// Move to the next row
			*/
			pSrcRow += pSrcMap->x_dim;

			/*
			// "increment" the y twiddle. Note only every 2nd bit is used
			*/
			yTwiddlePart= (yTwiddlePart+INCREMENT_Y_TWIDDLE) & Y_TWIDDLE_MASK;
			
		}/*end for y*/
	}
	/*
	// Else is translucent
	*/
	else
	{
		ASSERT((MapType==sgl_map_trans16) || (MapType==sgl_map_trans16_mm));

		/*
		// Initialise the Y part of the twiddle
		*/
		yTwiddlePart = 0;

		for(y = MapDim; y != 0; y--)
		{
			/*
			// Set up pointer to the first pixel in the source row.
			*/
			pSrcPixel = pSrcRow;

			/*
			// Initialise the X twiddle part
			*/
			xTwiddlePart = 0;

			/*
			// Step through the row
			*/
			for(x = MapDim; x != 0 ; x--)
			{
				/*
				// Compute the 16 bit pixel value
				*/
				PixelVal  = (pSrcPixel->alpha  & 0xf0) << 8;
				PixelVal |= (pSrcPixel->red		 & 0xf0) << 4;
				PixelVal |= (pSrcPixel->green    & 0xf0);
				PixelVal |=  pSrcPixel->blue >> 4;

				/*
				// Save this result at the "twiddled" position.
				*/
				pOutputPixels[xTwiddlePart | yTwiddlePart] = (sgl_uint16) PixelVal;

				/*
				// Go to the next pixel
				*/
				pSrcPixel++;

				/*
				// "increment" the x twiddle. Note only every 2nd bit is used
				*/
				xTwiddlePart= (xTwiddlePart+INCREMENT_X_TWIDDLE) & X_TWIDDLE_MASK;

			}/*end for x*/

			/*
			// Move to the next row
			*/
			pSrcRow += pSrcMap->x_dim;

			/*
			// "increment" the y twiddle. Note only every 2nd bit is used
			*/
			yTwiddlePart= (yTwiddlePart+INCREMENT_Y_TWIDDLE) & Y_TWIDDLE_MASK;
			
		}/*end for y*/

	}/*end if/else*/
	

}
					


/******************************************************************************
 * Function Name: Map8to3
 *
 * Inputs       : Component
 * Outputs      : -
 * Returns      : Result
 * Globals Used : -
 *
 * Description  : This maps one of the colour components from 8 bits to 3 bits
 *				  for use in the 8bpp conversion.
 *****************************************************************************/
static INLINE unsigned char Map8to3(unsigned char Component)
{
	unsigned char Result;

	if(Component<128)
		Result=3;
	else
		Result=4;

	if(Component<112)
		Result=(Component+16)>>5;

	if(Component>143)
		Result=(Component-15)>>5;

	return(Result);
}

/******************************************************************************
 * Function Name: Pack32ToTexas8
 *
 * Inputs       : Intermediate map of sufficient size
 *				  Map dimension
 *				  Map Type
 *				  
 * Outputs      : 16 bit/2pixels texture data
 * Returns      : None
 * Globals Used : 
 *
 * Description  : This performs the truncation, packing and twiddling from RGBA
 *				  quad-bytes, and places the result in the location pointed to...
 *				  
 *****************************************************************************/
static void Pack32ToTexas8( sgl_uint16 * pOutputPixels,
			const sgl_intermediate_map *  pSrcMap,
							const int	  MapDim)
{
	int x,y;
	sgl_uint32 PixelVal;

	int yTwiddlePart;
	int xTwiddlePart;

	sgl_map_pixel *pSrcPixel, *pSrcRow;

	ASSERT(pSrcMap->x_dim >= MapDim);
	ASSERT(pSrcMap->y_dim >= MapDim);
	ASSERT((MapDim&0x1) == 0);    /* X MUST be even*/

	/*
	// Set up pointer to the first source row
	*/
	pSrcRow = pSrcMap->pixels;

	/*
	// Initialise the Y part of the twiddle
	*/
	yTwiddlePart = 0;

	for(y = MapDim; y != 0 ; y--)
	{
		/*
		// Set up pointer to the first pixel in the source row.
		*/
		pSrcPixel = pSrcRow;

		/*
		// Initialise the X twiddle part
		*/
		xTwiddlePart = 0;

		/*
		// Step through the row, doing two at a time
		*/
		for(x = 0; x < MapDim/2; x++)
		{
			/*
			// Do the first of the pair of pixels
			*/
			PixelVal  = Map8to3(pSrcPixel->red)   << 5;
			PixelVal |= Map8to3(pSrcPixel->green) << 2;
			PixelVal |= Map8to3(pSrcPixel->blue ) >> 1;

			PixelVal<<=8;	/*make room for the other pixel*/

			/*
			// evaluate the second pixel 
			*/
			PixelVal |= Map8to3(pSrcPixel[1].red)   << 5;
			PixelVal |= Map8to3(pSrcPixel[1].green) << 2;
			PixelVal |= Map8to3(pSrcPixel[1].blue ) >> 1;

			/*
			// Save this result at the "twiddled" position.
			*/
			pOutputPixels[xTwiddlePart | yTwiddlePart] = (sgl_uint16) PixelVal;

			/*
			// "increment" the x twiddle. Note only every 2nd bit is used
			*/
			xTwiddlePart= (xTwiddlePart+INCREMENT_X_TWIDDLE) & X_TWIDDLE_MASK;

			/*
			// Go to the next pixel pair
			*/
			pSrcPixel+=2;

		}/*end for x*/

		/*
		// Move to the next row
		*/
		pSrcRow += pSrcMap->x_dim;

		/*
		// "increment" the y twiddle. Note only every 2nd bit is used
		*/
		yTwiddlePart= (yTwiddlePart+INCREMENT_Y_TWIDDLE) & Y_TWIDDLE_MASK;
			
	}/*end for y*/


}




/******************************************************************************
 * Function Name: Err5bit
 *
 * Inputs       : 8 bit colour channel
 * Returns      : Error between the equivalent 5 bit value and the 
 *				  original 8 bit one
 * Returns      : -
 * Globals Used : 
 *
 * Description  : Used as part of dithering process.
 *****************************************************************************/
static INLINE int Err5bit(int Colour)
{
	return(Colour - ((Colour & 0xf8) | 4));
}

/******************************************************************************
 * Function Name: Err4bit
 *
 * Inputs       : 8 bit colour channel
 * Returns      : Error between the equivalent 4 bit value and the 
 *				  original 8 bit one
 * Returns      : -
 * Globals Used : 
 *
 * Description  : Used as part of dithering process.
 *****************************************************************************/
static INLINE int Err4bit(int Colour)
{
	return(Colour -  ((Colour & 0xf0) | 4 | ((Colour >> 4) & 8)) );
}

/******************************************************************************
 * Function Name: Err3bit
 *
 * Inputs       : 8 bit colour channel
 * Returns      : Error between the equivalent 3 bit value and the 
 *				  original 8 bit one
 * Returns      : -
 * Globals Used : 
 *
 * Description  : Used as part of dithering process.
 *****************************************************************************/
static int Err3bit(int Colour)
{
	sgl_uint32 tmp;

	/*
	// Convert it to a 3 bit number, then shift back to an 8 bit
	*/
	tmp = (sgl_uint32) Map8to3((unsigned char)Colour);
	tmp <<= 5;
	
	/*
	// replicate the top bit in the lower bits, and get the error
	*/
	if(tmp & 0x80)
	{
		return (Colour - (tmp + 0x1f));
	}
	else
	{
		return (Colour - tmp);
	}
}

/******************************************************************************
 * Function Name: Err2bit
 *
 * Inputs       : 8 bit colour channel
 * Returns      : Error between the equivalent 2 bit value and the 
 *				  original 8 bit one
 * Returns      : -
 * Globals Used : 
 *
 * Description  : Used as part of dithering process.
 *****************************************************************************/
static int Err2bit(int Colour)
{
	sgl_uint32 tmp;

	/*
	// Convert it to a 3/2 bit number, then shift back to an 8 bit
	*/
	tmp = (sgl_uint32) Map8to3((unsigned char)Colour) >> 1;
	tmp <<= 6;
	
	/*
	// replicate the top bit in the lower bits, and get the error
	*/
	if(tmp & 0x80)
	{
		return (Colour - (tmp + 0x3f));
	}
	else
	{
		return (Colour - tmp);
	}
}


/******************************************************************************
 * Function Name: DitherMap
 *
 * Inputs       : Pixels,the full colour map
 *				  MapType,
 * Outputs      : Pixels this time dithered
 * Returns      : -
 * Globals Used : 
 *
 * Description  : Performs Floyd Steinberg Dithering.
 *				  Multi-pass dither may be required for the edges of textures.
 *				  There is room for optimizaion here.
 * Pre-condition: Pixels->x_dim==Pixels->y_dim==power of 2.
 *****************************************************************************/
static void DitherMap( sgl_intermediate_map *  	Pixels,   	
					   sgl_map_types 			MapType)
{

   	int Red,Green,Blue,Alpha;
	int Err38ths;

	/*
	// Keep track of the errors passed on from the pixel to the
	// left of the current one.
	// Also do the same for the pixels below
	*/
	int LeftErr[4];
	int BelowLeftErr[4];

	/*
	// Also keep track of the errors that will be added to the line below.
	*/
	#define MAX_X 256
	int BelowErr[MAX_X][4];
	int *pBErr;

	int x,y;

	sgl_map_pixel *pPixel;


	ASSERT(Pixels->y_dim == Pixels->x_dim);
	ASSERT(Pixels->x_dim <= MAX_X);

	/*
	// if this map is too small to dither, skip it
	*/
	if(Pixels->x_dim == 1)
	{
		return;
	}

	/*
	// Initialize the error values passed onto the pixels below
	*/
	pBErr = BelowErr[0];
	for(x=Pixels->x_dim; x!=0; x --)
	{
		pBErr[0] = 0;
		pBErr[1] = 0;
		pBErr[2] = 0;
		pBErr[3] = 0;

		pBErr += 4;
	}


	/*
	// Initialise the error passed from the pixel to the left, and the
	// one below.
	*/
	LeftErr[0] = 0;
	LeftErr[1] = 0;
	LeftErr[2] = 0;
	LeftErr[3] = 0;

	BelowLeftErr[0] = 0;
	BelowLeftErr[1] = 0;
	BelowLeftErr[2] = 0;
	BelowLeftErr[3] = 0;

	/*
	// Set pointer to the first pixel
	*/
	pPixel = Pixels->pixels;

	/*
	// Step through the rows and columns
	*/
	for(y = Pixels->y_dim; y != 0; y --)
	{
		/*
		// Reset the pointer to the errors passed to
		// the row below
		*/
		pBErr = BelowErr[0];

		/*
		// Step through this row
		*/
		for(x = Pixels->x_dim; x != 0; x --)
		{
			/*
			// Add the passed on errors to this pixel.
			// These are the errors supplied from the row
			// above, and the pixel to the left.
			// Clamp the result to the legal range.
			*/
			Red     = pPixel->red     + pBErr[0] + LeftErr[0];
			Green   = pPixel->green	  + pBErr[1] + LeftErr[1];
			Blue    = pPixel->blue	  + pBErr[2] + LeftErr[2];
			Alpha = pPixel->alpha + pBErr[3] + LeftErr[3];

			Red		= CLAMP(Red,	 0, 255);
			Green	= CLAMP(Green,	 0, 255); 
			Blue	= CLAMP(Blue,	 0, 255); 
			Alpha = CLAMP(Alpha, 0, 255); 

			/*
			// Save the Result back in the pixels
			*/
			pPixel->red		= Red;
			pPixel->green	= Green;
			pPixel->blue	= Blue;
			pPixel->alpha	= Alpha;

			/*
			// Compute the error when dithering
			*/
			switch(MapType)
			{
				case sgl_map_8bit:
				{
					Red   = Err3bit(Red);
					Green = Err3bit(Green);
					Blue  = Err2bit(Blue);
					Alpha = 0;
				}
				break;


				case sgl_map_16bit:
				case sgl_map_16bit_mm:
				{
					Red   = Err5bit(Red);
					Green = Err5bit(Green);
					Blue  = Err5bit(Blue);
					Alpha = 0;
				}
				break;


				default:
				{					
					ASSERT( (MapType== sgl_map_trans16) || 
							(MapType== sgl_map_trans16_mm));

					Red   = Err4bit(Red);
					Green = Err4bit(Green);
					Blue  = Err4bit(Blue);
					Alpha = Err4bit(Alpha);
				}
			}/*end switch*/


			/*
			// Distribute the error.
			// 3/8ths goes to the pixel to the left and to the one below.
			// For the one below, remember to add on the bit to its left.
			//
			// Also distribute 1/4 of the error to the "below right" pixel
			*/
			Err38ths = ((3*Red) + 4)>> 3;
			LeftErr[0] = Err38ths;
			pBErr[0]   = BelowLeftErr[0] + Err38ths;

			BelowLeftErr[0] = Red   - (Err38ths << 1);

			/*green*/
			Err38ths = ((3*Green) + 4) >> 3;
			LeftErr[1] = Err38ths;
			pBErr[1]   = BelowLeftErr[1] + Err38ths;

			BelowLeftErr[1] = Green  - (Err38ths << 1);

			/*blue*/
			Err38ths = ((3*Blue) + 4) >> 3;
			LeftErr[2] = Err38ths;
			pBErr[2]   = BelowLeftErr[2] + Err38ths;

			BelowLeftErr[2] = Blue  - (Err38ths << 1);

			/*alpha*/
			Err38ths = ((3*Blue)+4) >> 3;
			LeftErr[3] = Err38ths;
			pBErr[3]   = BelowLeftErr[3] + Err38ths;

			BelowLeftErr[3] = Alpha  - (Err38ths << 1);


			/*
			// Move to the next pixel along
			*/
			pPixel++;
			pBErr += 4;

		}/* end for x */

		/*
		// Note: we pass on the "Left" errors on to the next row.
		// This should prevent any obvious differences with the first
		// column
		*/

	}/*end for y*/

}



#ifdef FFT_FILTER
/******************************************************************************
 * Function Name: HalveResolution   FFT VERSION
 *
 * Inputs       : Pixels,the high resolution map.
 *				  
 * Outputs      : Pixels,the filtered and decimated map.
 * Returns      : -
 * Globals Used : 
 *
 * Description  : This 'halves' the Pixels resolution.
 *				  There is room for optimizaion here.
 * Pre-condition: Pixels->x_dim==Pixels->y_dim==power of 2.
 * 				  Pixels->pixels has been allocated.
 *****************************************************************************/
static void HalveResolution(sgl_intermediate_map *  Pixels)
{
	int corner;

	corner=(Pixels->x_dim)-1;

	corner/=4; /*REMEMBER THIS IS ONLY TO LOOK AT IT*/

	DPF((DBG_MESSAGE,"corner=%d\n",corner));

	CopyImage(SafeImage,WorkingImage);
	FilterFFT(WorkingImage,corner);
	ConvertFromFD(WorkingImage);

	FromFloatToSgl(Pixels,WorkingImage,Pixels->x_dim);

}

#else 

/******************************************************************************
 * Function Name: HalveResolution
 *
 * Inputs       : SourcePixels,the high resolution map.
 *				  
 * Outputs      : DestPixels,the filtered and decimated map.
 * Returns      : -
 * Globals Used : 
 *
 * Description  : This 'halves' the Pixels resolution.
 *				  There is room for optimizaion here.
 * Pre-condition: Pixels->x_dim==Pixels->y_dim==power of 2.
 * 				  Pixels->pixels has been allocated.
 *
 *
 *	Quote:	"That's an exciting comment... Have you put any quotes in there?"
 *													Pete Leaback May 1996
 *
 *	Design Requirements: 
 *				1) Have to implement a filter that prevents the frequencies
 *				   of lower mip maps becoming excessive. IE when producing
 *				   a pixel in the 1/2 size map, it needs to take into account
 *				   some of the surrounding pixels.
 *
 *				2) It should be able to do the halving of the texture in place.
 *				   ie. We don't have to allocate another buffer to put the
 *				   results in. This should reduce cache usage/ and memory
 *				   paging etc.
 *
 *				3) It's got to be "fast" :)
 *
 * Implementation Details.
 *
 *		The filter I (Simon sjf) have decided to implement looks like
 *
 *						     COLUMN
 *						  W   X   Y   Z
 *						-----------------
 *					A	| 1 | 2 | 2 | 1 |
 *						-----------------
 *					B	| 2 | 4 | 4 | 2 |
 *		ROW				-----------------
 *					C	| 2 | 4 | 4 | 2 |
 *						-----------------
 *					D	| 1 | 2 | 2 | 1 |
 *						-----------------
 *
 *		Basically 16 pixels are used in a weighted sum to produce a single
 *		pixel. The pixels with the "4" weights are the ones originally
 *		used to produce the half-size polygons.
 *
 *
 *		When doing the next 1/2 size pixel to the right, Columns Y and Z must
 *		be re-used as columns W & X. The weightings are chosen so that the X &
 *		Y weightings are 2x the W & Z ones. This allows us to sum up Y and Z
 *		columns using the weightings (1,2,2,1) and then optionally multiply by
 *		2 to generate the (2,4,4,2) columns.
 *
 *		IE in summary, each column only needs to be computed once as we generate
 *		a row of pixels of the 1/2 size map.
 *
 *		Note that we must allow for the wrap around of the filter from one
 *		side of the texture to the other, and from top to bottom.
 *
 *		The weights of the textures sum up to 36. We therefore have to
 *		divide by 36 to get the final summed value. Integer Division is
 *		(usually) MUCH slower than multiplication, so I'm using the
 *		approximation...
 *					1/36 ~ 57/2048
 *
 *		This is accurate enough for this function.
 *		
 *
 *	IMPLEMENTATION
 *		The function is split into 2 functions. The first does a row at
 *		a time - given pointers to lines A,B,C, & D, and a pointer to where
 *		the results go.
 *
 *		The second initially makes a copy of the first row of the source, 
 *		then steps through the 1/2 size output rows calling the other
 *		function. The last 1/2 size row uses the copy of the first source
 *		row as it gets destroyed in the process.
 *
 *****************************************************************************/
typedef struct
{
	int red;
	int green;
	int blue;
	int alpha;
}SUMMED_PIXEL;


/*//////////////////////////
// Function  HalveLine
//
// This function simply takes 4 input rows and outputs the 1/2 size
// destination row.  It is effectively a part of the Hal
////////////////////////////
*/

static void HalveLine(sgl_map_pixel *pLineA, 
					  sgl_map_pixel *pLineB,
					  sgl_map_pixel *pLineC,
					  sgl_map_pixel *pLineD,
					  sgl_map_pixel *pDest,
					  const int		HalfSize)
{
	
	SUMMED_PIXEL ColumnW, ColumnX;
	SUMMED_PIXEL FirstColumn, LastColumn;

	int x;
	int Component;

	/*
	// Set up the W & X columns for the first pixel in the row
	// ColumnW actually is the last pixels in the rows.
	*/
	ColumnW.red = 	  pLineA[HalfSize * 2 - 1].red + 
				  2 * pLineB[HalfSize * 2 - 1].red + 
				  2 * pLineC[HalfSize * 2 - 1].red + 
					  pLineD[HalfSize * 2 - 1].red;

	ColumnW.green =   pLineA[HalfSize * 2 - 1].green + 
				  2 * pLineB[HalfSize * 2 - 1].green + 
				  2 * pLineC[HalfSize * 2 - 1].green + 
					  pLineD[HalfSize * 2 - 1].green;

	ColumnW.blue =    pLineA[HalfSize * 2 - 1].blue + 
				  2 * pLineB[HalfSize * 2 - 1].blue + 
				  2 * pLineC[HalfSize * 2 - 1].blue + 
					  pLineD[HalfSize * 2 - 1].blue;

	ColumnW.alpha = pLineA[HalfSize * 2 - 1].alpha + 
				  2 * pLineB[HalfSize * 2 - 1].alpha + 
				  2 * pLineC[HalfSize * 2 - 1].alpha + 
					  pLineD[HalfSize * 2 - 1].alpha;

	
	ColumnX.red = 	  pLineA[0].red + 
				  2 * pLineB[0].red + 
				  2 * pLineC[0].red + 
					  pLineD[0].red;

	ColumnX.green =   pLineA[0].green + 
				  2 * pLineB[0].green + 
				  2 * pLineC[0].green + 
					  pLineD[0].green;

	ColumnX.blue =    pLineA[0].blue + 
				  2 * pLineB[0].blue + 
				  2 * pLineC[0].blue + 
					  pLineD[0].blue;

	ColumnX.alpha = pLineA[0].alpha + 
				  2 * pLineB[0].alpha + 
				  2 * pLineC[0].alpha + 
					  pLineD[0].alpha;

	/*
	// Copy the first and last columns, as we reuse these to do the last
	// pixel in the row.
	*/
	FirstColumn = ColumnX;
	LastColumn  = ColumnW;

	/*
	// Do all but the last column
	*/
	for(x = HalfSize-1; x != 0; x --)
	{
		/*
		//  //////// Red ////////
		//
		// Add the weighted W & X column components
		*/
		Component = ColumnW.red + 2 * ColumnX.red;
		
		/*
		// Compute the Y & Z column components BUT store them back
		// in W & X values for the NEXT pixel
		*/
		ColumnW.red = pLineA[1].red + 
				  2 * pLineB[1].red + 
				  2 * pLineC[1].red + 
					  pLineD[1].red;

		ColumnX.red = pLineA[2].red + 
				  2 * pLineB[2].red + 
				  2 * pLineC[2].red + 
					  pLineD[2].red;

		/*
		// Add the weighted "Y & Z" values.
		*/
		Component += 2 * ColumnW.red + ColumnX.red;
		
		/*
		// "divide" by the sum of the weightings
		// ie 1/36 ~ 57/2048 
		*/
		pDest->red = (Component * 57) >> 11;


		/*
		//  //////// Green ////////
		//
		// Add the weighted W & X column components
		*/
		Component = ColumnW.green + 2 * ColumnX.green;
		
		/*
		// Compute the Y & Z column components BUT store them back
		// in W & X values for the NEXT pixel
		*/
		ColumnW.green = pLineA[1].green + 
				  2 * pLineB[1].green + 
				  2 * pLineC[1].green + 
					  pLineD[1].green;

		ColumnX.green = pLineA[2].green + 
				  2 * pLineB[2].green + 
				  2 * pLineC[2].green + 
					  pLineD[2].green;

		/*
		// Add the weighted "Y & Z" values.
		*/
		Component += 2 * ColumnW.green + ColumnX.green;
		
		/*
		// "divide" by the sum of the weightings
		*/
		pDest->green = (Component * 57) >> 11;


		/*
		//  //////// Blue ////////
		//
		// Add the weighted W & X column components
		*/
		Component = ColumnW.blue + 2 * ColumnX.blue;
		
		/*
		// Compute the Y & Z column components BUT store them back
		// in W & X values for the NEXT pixel
		*/
		ColumnW.blue = pLineA[1].blue + 
				  2 * pLineB[1].blue + 
				  2 * pLineC[1].blue + 
					  pLineD[1].blue;

		ColumnX.blue = pLineA[2].blue + 
				  2 * pLineB[2].blue + 
				  2 * pLineC[2].blue + 
					  pLineD[2].blue;

		/*
		// Add the weighted "Y & Z" values.
		*/
		Component += 2 * ColumnW.blue + ColumnX.blue;
		

		/*
		// "divide" by the sum of the weightings
		*/
		pDest->blue = (Component * 57) >> 11;

		/*
		//  //////// Alpha ////////
		//
		// Add the weighted W & X column components
		*/
		Component = ColumnW.alpha + 2 * ColumnX.alpha;
		
		/*
		// Compute the Y & Z column components BUT store them back
		// in W & X values for the NEXT pixel
		*/
		ColumnW.alpha = pLineA[1].alpha + 
					  2 * pLineB[1].alpha + 
					  2 * pLineC[1].alpha + 
						  pLineD[1].alpha;

		ColumnX.alpha = pLineA[2].alpha + 
					  2 * pLineB[2].alpha + 
					  2 * pLineC[2].alpha + 
						  pLineD[2].alpha;

		/*
		// Add the weighted "Y & Z" values.
		*/
		Component += 2 * ColumnW.alpha + ColumnX.alpha;
		

		/*
		// "divide" by the sum of the weightings
		*/
		pDest->alpha = (Component * 57) >> 11;


		/*
		// Move on to the next pixels
		*/
		pDest ++;
		pLineA += 2;
		pLineB += 2;
		pLineC += 2;
		pLineD += 2;

	}/*end for*/


	/*
	// Finally do the last pixel.
	//
	//  //////// RED ////////
	//
	// Add the weighted W, X, Y, & Z column components
	*/
	Component = ColumnW.red + 2 * ColumnX.red + 
							  2 * LastColumn.red + FirstColumn.red;
		
	/*
	// "divide" by the sum of the weightings
	*/
	pDest->red = (Component * 57) >> 11;

	/*
	//  //////// GREEN ////////
	//
	// Add the weighted W, X, Y, & Z column components
	*/
	Component = ColumnW.green + 2 * ColumnX.green + 
								2 * LastColumn.green + FirstColumn.green;
		
	/*
	// "divide" by the sum of the weightings
	*/
	pDest->green = (Component * 57) >> 11;

	/*
	// //////// BLUE ////////
	//
	// Add the weighted W, X, Y, & Z column components
	*/
	Component = ColumnW.blue + 2 * ColumnX.blue + 
							   2 * LastColumn.blue + FirstColumn.blue;
		
	/*
	// "divide" by the sum of the weightings
	*/
	pDest->blue = (Component * 57) >> 11;

	/*
	// //////// ALPHA ////////
	//
	// Add the weighted W, X, Y, & Z column components
	*/
	Component = ColumnW.alpha + 2 * ColumnX.alpha + 
								  2 * LastColumn.alpha + FirstColumn.alpha;

	/*
	// "divide" by the sum of the weightings
	*/
	pDest->alpha = (Component * 57) >> 11;
}


/*////////////////////////////
// The main "guts" of Halve resolution routine
////////////////////////////
*/

static void HalveResolution(sgl_intermediate_map *  Pixels)
{
	int y;

	sgl_map_pixel *pLineA,*pLineB;
	sgl_map_pixel *pLineC,*pLineD;
	sgl_map_pixel *pDst;

	int OrigSize;
	int HalfSize;

	sgl_map_pixel FirstRowCopy[256];

	OrigSize = Pixels->y_dim;
	HalfSize = OrigSize/ 2;

	ASSERT(OrigSize == Pixels->x_dim);
	ASSERT(OrigSize >= 2);


	/*
	// As it will be destroyed when processing the first row,
	// but will be needed to do the last row, make a copy of the
	// first row
	*/
	ASSERT(OrigSize <= 256);
	pLineA = Pixels->pixels;
	pLineB = FirstRowCopy;
	for(y = OrigSize; y != 0 ; y --)
	{
		*pLineB = * pLineA;
		pLineA ++;
		pLineB ++;
	}

	/*
	// Set pointers to the destination pixels, and to the source rows.
	//
	// Line A should point to last row of the source pixels
	//
	*/
	pDst = Pixels->pixels;
	pLineA = &Pixels->pixels[(OrigSize - 1)* OrigSize];
	pLineB = Pixels->pixels;
	
	/*
	// Step through all but the last row of the destination
	*/
	for(y = HalfSize - 1; y != 0; y --)
	{
		/*
		// Line C and Line D just come after B
		*/
		pLineC = pLineB + OrigSize;
		pLineD = pLineB + (2 * OrigSize);

		/*
		// Process the line
		*/
		HalveLine(pLineA, pLineB, pLineC, pLineD, pDst, HalfSize);


		/*
		// Move on to the next line
		*/
		pLineA = pLineC;
		pLineB = pLineD;
		pDst += HalfSize;
	}


	/*
	// Do the final row: Note that row D wraps around to the 
	// original first row.
	*/
	pLineC = pLineB + OrigSize;
	pLineD = FirstRowCopy;
	HalveLine(pLineA, pLineB, pLineC, pLineD, pDst, HalfSize);
	

	/*
	// Store the new sizes of the texture
	*/
	Pixels->x_dim = HalfSize;
	Pixels->y_dim = HalfSize;
}


#endif /*FFT etc */


/******************************************************************************
 * Function Name: TextureSize    INTERNAL ONLY
 *
 * Inputs       : map_type,
 *				  map_size

 * Outputs      : None

 * Returns      : Size of map in bytes
 *
 * Description  : Given map type and map size, it computes the size in bytes
 *				  of data needed. It assumes that the input parameters are valid.
 *****************************************************************************/
static long TextureSize(const sgl_map_types map_type,
						const sgl_map_sizes map_size)
{
	long SpaceRequired;
	
	/*work out the size of the of the texture to be allocated*/

	if(map_type==sgl_map_trans16_mm || map_type==sgl_map_16bit_mm)
		SpaceRequired=MapLength[1][map_size];
	else
		SpaceRequired=MapLength[0][map_size];

	/*
	// if 8 instead of 16 bit...
	*/	
	if(map_type==sgl_map_8bit)
		SpaceRequired>>=1;

	return SpaceRequired;
}



/******************************************************************************
 * Function Name: sgl_texture_size
 *
 * Inputs       : texture_map

 * Outputs      : None

 * Returns      : Size of map in bytes, OR an error if the parameters are invalid.
 *
 * Description  : Given a pointer to an sgl intermediate map, this returns the size
 *				  of the pixel data required. Note that the fields in the map
 *				  must be valid.
 *****************************************************************************/
extern long CALL_CONV sgl_texture_size ( sgl_intermediate_map *texture_map)
{

	sgl_map_types map_type;
	sgl_map_sizes map_size;


	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		**	We failed to initialise sgl
		*/

		return(SglError(sgl_err_failed_init));
	}
#endif
	if(texture_map==NULL)
	{
		DPFDEV((DBG_ERROR,"sgl_texture_size: NULL texture_map"));
		return(SglError(sgl_err_bad_parameter));
	}

	/*
	// Look at the type of the supplied intermediate map. If this
	// is a user defined map, then "preprocess" it into 
	*/
	if((texture_map->id[0]=='I') && (texture_map->id[1]=='M') &&
	   (texture_map->id[2]=='A') && (texture_map->id[3]=='P'))
	{

		if((texture_map->x_dim > 0) && (texture_map->y_dim > 0))
		{
			return (texture_map->x_dim * 
				    texture_map->y_dim * sizeof(sgl_map_pixel));
		}
		else
			return(SglError(sgl_err_bad_parameter));
		
	}
	/*
	// Else is this a valid pre-processed map?
	*/
	else 
	{
		map_type = (sgl_map_types) texture_map->id[2];
		map_size = (sgl_map_sizes) texture_map->id[3];

		/*
		// Is this data valid?
		*/
		if( (texture_map->id[0] != 'P')   || (texture_map->id[1] != 'T') ||
			(map_type < sgl_map_16bit) || (map_type > sgl_map_trans16_mm) ||
			(map_size < sgl_map_32x32) || (map_size > sgl_map_256x256))
		{
			return(SglError(sgl_err_bad_parameter));
		}
		/*
		// Else, look up the size
		*/
		else
		{
			return (TextureSize(map_type, map_size));
		}

	}/* check the type of the input pixels */

}



/******************************************************************************
 * Function Name: CopyTexture    INTERNAL ONLY
 *
 * Inputs       : SrcPixels,
 *				  MapDim

 * Outputs      : Copy padded/truncated to fit

 * Returns      : Nil
 *
 * Description  : Given a source map and a size, PLUS a destination, it
 *				  copies the source to the destionation, truncating/padding
 *				  out the pixels to the correct dimensions.
 *				  It also fills in the Dimensions for the header in the Destination.
 *****************************************************************************/
static void CopyTexture(const sgl_intermediate_map * SrcPixels,
							  sgl_intermediate_map * DstPixels,
						const int					 MapDim)
{
	int x,y;
	int xLim, yLim;
	sgl_map_pixel *pSrcPixel, *pDstPixel;
	sgl_map_pixel *pSrcRow, *pDstRow;

	const sgl_map_pixel black={0,0,0,0};


	/*
	// To make things realy simple, if the source is smaller in either
	// dimension to the destination, we'll first fill the destination
	// with black, then copy over as much as we need from the source.
	//
	// If SRC smaller, then fill the destination with black
	// (This case shouldn't occur too often)
	*/
	if((MapDim > SrcPixels->x_dim) || (MapDim > SrcPixels->y_dim))
	{
		if(MapDim > SrcPixels->x_dim)
		{
			xLim = SrcPixels->x_dim;
		}
		if(MapDim > SrcPixels->y_dim)
		{
			yLim = SrcPixels->y_dim;
		}

		/*
		// Get pointer to first row of the destination
		*/
		pDstRow = DstPixels->pixels;

		for(y = MapDim; y != 0;  y --)
		{
			/*
			// get pointer to first pixel in the current row
			*/
			pDstPixel = pDstRow;

			for(x = MapDim; x != 0; x--, pDstPixel++)
			{
				/*
				// Copy the pixel
				*/
				*pDstPixel = black;

			}/*end for x*/

			/*
			// Step to the next row
			*/
			pDstRow += MapDim;
		}/*end for y*/
		
	}
	/*
	// Else plenty of source pixels
	*/
	else
	{
		xLim = MapDim;
		yLim = MapDim;
	}


	/*
	// Now just copy everything we can out of the source
	//
	// Get pointer to first rows of both maps
	*/
	pSrcRow = SrcPixels->pixels;
	pDstRow = DstPixels->pixels;

	for(y = yLim; y != 0;  y --)
	{
		/*
		// get pointers to first pixels in the current rows
		*/
		pSrcPixel = pSrcRow;
		pDstPixel = pDstRow;

		for(x = xLim; x != 0; x--)
		{
			/*
			// Copy the pixel
			*/
			*pDstPixel = *pSrcPixel;

			/*
			// Move to the next pixels
			*/
			pSrcPixel ++;
			pDstPixel ++;

		}/*end for x*/
			
		/*
		// Step to the next rows
		*/
		pSrcRow += SrcPixels->x_dim;
		pDstRow += MapDim;

	}/*end for y*/


	/*
	// Fill in the relevant header bits
	*/
	DstPixels->x_dim = MapDim;
	DstPixels->y_dim = MapDim;
}

/******************************************************************************
 * Function Name: GenerateTextureSpec
 *
 * Inputs       : map_type,
 *				  map_size,
 * Outputs      : Texture Specification
 * Returns      : A Filled in texture specification
 * Globals Used : None
 *
 * Description  : New for PVROS
 *****************************************************************************/

static TEXTUREFORMAT TexFormat888[2] =
{
	{TF_RGB | TF_CANMIPMAP, 
		0, 32, 0, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000},
	{TF_TRANSLUCENT | TF_OGLALPHA | TF_RGB | TF_CANMIPMAP, 
		0 ,32, 0, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000}
};

static TEXTUREFORMAT TexFormat[5] =
{  /* Flags, FourCC, BitDepth, KeyColour, Red, Green, Blue, Alpha */
	{TF_HWSUPPORT | TF_RGB | TF_CANMIPMAP, 0, 16, 0, 0x7C00, 0x03E0, 0x001F, 0x0000}, 
	{TF_HWSUPPORT | TF_RGB | TF_CANMIPMAP, 0, 16, 0, 0x7C00, 0x03E0, 0x001F, 0x0000},
	{TF_PALETTISED8, 0, 8, 0, 0, 0, 0, 0},
	{TF_TRANSLUCENT | TF_HWSUPPORT | TF_OGLALPHA | TF_RGB | TF_CANMIPMAP, 0, 16, 0, 0x0F00, 0x00F0, 0x000F, 0xF000},
	{TF_TRANSLUCENT | TF_HWSUPPORT | TF_OGLALPHA | TF_RGB | TF_CANMIPMAP, 0 ,16, 0, 0x0F00, 0x00F0, 0x000F, 0xF000}
};

static sgl_uint32 MapSize[4] = {32,64,128,256};

void GenerateTextureSpec( sgl_map_types map_type,
						  sgl_map_sizes map_size,
						  PTEXTURESPEC pTextureSpec )
{
	switch(map_type)
	{
		case sgl_map_16bit:
		case sgl_map_8bit:
		case sgl_map_trans16:
			pTextureSpec->MipMapped	= FALSE;
			break;
		case sgl_map_16bit_mm:
		case sgl_map_trans16_mm:
			pTextureSpec->MipMapped	= TRUE;
			break;	
	}

	pTextureSpec->pFormat 	= (PTEXTUREFORMAT) &TexFormat[map_type];				 
	pTextureSpec->Width		= MapSize[map_size];
	pTextureSpec->Height	= MapSize[map_size];
}

void Generate888TextureSpec( sgl_map_types map_type,
						  sgl_map_sizes map_size,
						  PTEXTURESPEC pTextureSpec )
{
	pTextureSpec->MipMapped	= FALSE;
	
	switch(map_type)
	{
		case sgl_map_16bit_mm:
			pTextureSpec->MipMapped	= TRUE;
		case sgl_map_16bit:
		case sgl_map_8bit:
			pTextureSpec->pFormat 	= &TexFormat888[0];				 
			break;

		case sgl_map_trans16_mm:
			pTextureSpec->MipMapped	= TRUE;
		case sgl_map_trans16:
			pTextureSpec->pFormat 	= &TexFormat888[1];				 
			break;	
	}

	pTextureSpec->Width		= MapSize[map_size];
	pTextureSpec->Height	= MapSize[map_size];
}

/******************************************************************************
 * Function Name: sgl_preprocess_texture
 *
 * Inputs       : map_type,
 *				  map_size,
 *				  generate_mipmap,
 *				  dither,
 *				  *pixel_data, read only
 *				  *filtered_maps, read only
 * Outputs      : Preprocessed map values.
 * Returns      : error value OR the size of the preprocessed Pixel Data
 * Globals Used : None
 *
 * Description  : Ref: the API document.
 *****************************************************************************/
extern int CALL_CONV sgl_preprocess_texture ( sgl_map_types map_type,
											  sgl_map_sizes map_size,
											  sgl_mipmap_generation_options  generate_mipmap,
											  sgl_bool dither,
											  const sgl_intermediate_map *src_pixel_data,
											  const sgl_intermediate_map *filtered_maps[],
											  sgl_intermediate_map *processed_map )

{

	long MapSize;

	int MapDim;
	sgl_intermediate_map LocalMap;

	int index;

	sgl_uint16 *pResultPixels;
	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		return(SglError(sgl_err_failed_init));
	}
#endif
	/*
	// Check that the parameters are valid
	//
	// I (sjf) should also check if the filtered maps are valid also, but
	// couldn't be bothered
	*/
	if((src_pixel_data == NULL) || (processed_map == NULL))
	{
		DPFDEV((DBG_ERROR,"sgl_preprocess_texture: NULL src_pixel_data or processed_map"));
		return(SglError(sgl_err_bad_parameter));
	}
	
	if((map_type < sgl_map_16bit) || (map_type > sgl_map_trans16_mm))
	{
		DPFDEV((DBG_ERROR,"sgl_preprocess_texture: bad map_type"));
		return(SglError(sgl_err_bad_parameter));
	}

	if((map_size < sgl_map_32x32) || (map_size > sgl_map_256x256))
	{
		DPFDEV((DBG_ERROR,"sgl_preprocess_texture: bad map_size"));
		return(SglError(sgl_err_bad_parameter));
	}

	/*
	// Decide how big (in bytes) the final processed texture data should be
	*/
	MapSize= TextureSize(map_type,map_size);

	/*
	// Allocate this block. If this fails report error and exit.
	*/
	pResultPixels = (sgl_uint16 *) SGLMalloc(MapSize);
	if(pResultPixels == NULL)
	{
		return(SglError(sgl_err_no_mem));
	}


	/*
	// To make life easy, make a local copy of the pixels of the input texture,
	// which are relevant to the texture. IE we may truncate or pad up bits.
	//
	// First work out how big this should be. Note that we are using 32 bits
	// per pixel at this point, rather than 16
	//
	// SGLMalloc this, and exit if error.
	*/
	MapDim	= 1 << (map_size + 5);
	MapSize = sizeof(sgl_map_pixel) * MapDim * MapDim;
	
	LocalMap.pixels = (sgl_map_pixel*) SGLMalloc(MapSize);

	if(LocalMap.pixels == NULL)
	{
		SGLFree(pResultPixels);
		return(SglError(sgl_err_no_mem));
	}

	/*
	// Make the local copy.
	*/
	CopyTexture(src_pixel_data, &LocalMap, MapDim);


	/*
	// Now decide what to do with these pixels...
	*/
	switch(map_type)
	{
		/*
		// 8 Bit texture
		*/
		case sgl_map_8bit:
			if(dither)
			{
				DitherMap(&LocalMap, sgl_map_8bit);
			}

			Pack32ToTexas8(pResultPixels, &LocalMap, MapDim);
			break;


		/*
		// 16 Bit NON MIP mapped
		*/
		case sgl_map_16bit:
		case sgl_map_trans16:
			if(dither)
			{
				DitherMap(&LocalMap, map_type);
			}

			Pack32ToTexas16(pResultPixels, &LocalMap, MapDim, map_type);
			break;


		/*
		// 16 Bit MIP mapped
		*/
		case sgl_map_16bit_mm:
		case sgl_map_trans16_mm:
			 
			/*
			// Get the "index" of the largest Map. This is the Log base 2 of
			// the XY dimensions.
			*/
			index = map_size + 5;

			/*
			// Do the highest resolution map.
			*/
			if(dither)
			{
				DitherMap(&LocalMap, map_type);
			}
			/*
			// Store it at the correct position in the map.
			*/
			Pack32ToTexas16(pResultPixels + EXTMapOffset[index], 
							&LocalMap, MapDim, map_type);
			 
			/*
			// Go into a loop processing the remaining mip maps
			*/
			index --;
			MapDim >>= 1;
			do
			{
				/*
				// if we have to generate the lower mip maps (or the
				// parameters passed are crud) do so.
				*/
			 	if(generate_mipmap || (filtered_maps == NULL) 
								   || (filtered_maps[index] == NULL))
				{
					/*
					// Halve the resolution of the previous map.
					*/
					HalveResolution(&LocalMap);

				}
				/*
				// Else use the supplied map. Copy it into our local
				// map, since it is large enough.
				*/
				else
				{
					CopyTexture(filtered_maps[index], &LocalMap, MapDim);
				}

				ASSERT(LocalMap.x_dim == MapDim);
				ASSERT(LocalMap.y_dim == MapDim);

				/*
				// Dither it if necessary
				*/
				if(dither)
				{
					DitherMap(&LocalMap, map_type);
				}

				/*
				// And finally store it at the required index
				*/
				Pack32ToTexas16(pResultPixels + EXTMapOffset[index], 
							&LocalMap, MapDim, map_type);
				

				/*
				// Move to next smaller map
				*/
			 	index --;
				MapDim >>= 1;
				
			}while(index >= 0);

			break;		

		default:
			ASSERT(FALSE);
			break;
				
	}/*end switch*/


	/*
	// Finally, put the results in the returned intermediate map, 
	// free the temporary local map, and exit with no error.
	*/
	processed_map->pixels = (sgl_map_pixel*)pResultPixels;

	processed_map->x_dim  = 1<< (map_size + 5);
	processed_map->y_dim  = processed_map->x_dim;

	processed_map->id[0]  = 'P';
	processed_map->id[1]  = 'T';
	processed_map->id[2]  = map_type;
	processed_map->id[3]  = map_size;

	SGLFree(LocalMap.pixels);
	SglError(sgl_no_err);

	/*
	// Return the size of the Preprocessed Texture
	*/
	return (TextureSize(map_type,map_size));

} /* sgl_preprocess_texture */


/******************************************************************************
 * Function Name: sgl_free_pixels
 *
 * Inputs       : pimMap - address of sgl_intermediate_map structure containing
 *				    pointer to pixels to be freed.
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 * Description  : Frees the pixel memory pointed to by the
 *				  sgl_intermediate_map structure, and sets the 'pixels'
 *				  pointer to NULL.
 *
 *				  This function should be called after sgl_preprocess_texture
 *				  or after ConvertBMPtoSGL when the intermediate map is no
 *				  longer required for a sgl_create_texture call.
 *
 *				  This is required if the data was allocated by SGL and is not
 *				  able to be freed by the application (or later on if SGL ends
 *				  up having its own memory allocation system).
 *
 *				  Errors:
 *				    sgl_err_bad_parameter: pimTexture was a NULL pointer.
 *****************************************************************************/
extern void CALL_CONV sgl_free_pixels( sgl_intermediate_map *pimMap )
{
#if !WIN32
	if (SglInitialise() != 0)
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	if (pimMap == NULL)
	{
		DPFDEV((DBG_ERROR,"sgl_free_pixels: NULL pimMap"));
		SglError(sgl_err_bad_parameter);
		return;
	}

	if (pimMap->pixels != NULL)
	{
		SGLFree(pimMap->pixels);
		pimMap->pixels = NULL;
	}

	SglError(sgl_no_err);
}


/******************************************************************************
 * Function Name: sgl_create_texture_internal
 *
 * Inputs       : map_type,
 *				  map_size,
 *				  generate_mipmap,
 *				  dither,
 *				  *pixel_data, read only
 *				  *filtered_maps, read only
 * Outputs      : -
 * Returns      : name or error.
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : SIMILAR to the API version, except that it allows the 
 *				  map_type and map_size to overide the embedded texture type
 *				  and size.
 *****************************************************************************/
extern int CALL_CONV create_texture_internal (sgl_map_types 		map_type,
									sgl_map_sizes 		map_size, 
								sgl_mipmap_generation_options 	generate_mipmap,
									sgl_bool 			dither,
							const	sgl_intermediate_map *pixel_data,
							const	sgl_intermediate_map *filtered_maps[],
									sgl_bool			 overide)
{
	TEXTURESOURCE 	TextureSource;
	TEXTURESPEC 	TextureSpec;
	HTEXTURE 		hTex;
	int 			name;
	sgl_intermediate_map LocalMap;
	int MapDim, index;
	sgl_uint32 nMapSize; 

	if((map_type < sgl_map_16bit) || (map_type > sgl_map_trans16_mm))
	{
		DPFDEV((DBG_ERROR,"sgl_create_texture: bad map_type"));
		return (SglError (sgl_err_bad_parameter));
	}
   	
	if((map_size < sgl_map_32x32) || (map_size > sgl_map_256x256))
	{
		DPFDEV((DBG_ERROR,"sgl_create_texture: bad map_size"));
		return (SglError (sgl_err_bad_parameter));
	}
	
	/* only create texure memory, no loading */
   	if(pixel_data==NULL)
   	{
   		GenerateTextureSpec (map_type, map_size, &TextureSpec);

		hTex = gpTextureIF->pfnTextureCreate (gHLogicalDev->TexHeap, &TextureSpec);

		if (!hTex)
		{
			DPFDEV ((DBG_WARNING, "Unable to create texture - no more tex mem"));
			return(SglError(sgl_err_texture_memory_full));
		}
		else
		{
			/* add the texture map structure to the nametable */
		    name = AddNamedItem (dlUserGlobals.pNamtab, hTex, nt_texture);
	
			/* if no name space left */
			if(name < 0)
			{					
				DPF ((DBG_WARNING, "Unable to create texture - no more room in name table"));
				gpTextureIF->pfnTextureFree (gHLogicalDev->TexHeap, hTex);
				return (SglError (name)); 	
			}

   			DPFDEV((DBG_WARNING,"sgl_create_texture: NULL pixel_data, only create texure memory"));
   			SglError(sgl_no_err);
   			return(name);
		}
   	}
	/*
	// Look at the type of the supplied intermediate map. If this
	// is a user defined map, then "preprocess" it into 
	*/
	else if((pixel_data->id[0]=='I') && (pixel_data->id[1]=='M') &&
	   (pixel_data->id[2]=='A') && (pixel_data->id[3]=='P'))
	{
		GenerateTextureSpec (map_type, map_size, &TextureSpec);

		hTex = gpTextureIF->pfnTextureCreate (gHLogicalDev->TexHeap, &TextureSpec);

		if (!hTex)
		{
			DPFDEV ((DBG_WARNING, "Unable to create texture - no more tex mem"));
			return(SglError(sgl_err_texture_memory_full));
		}
		else
		{
			/*
			** add the texture map structure to the nametable
			*/
		    name = AddNamedItem (dlUserGlobals.pNamtab, hTex, nt_texture);
	
			/*
			// if no name space left
			*/
			if(name < 0)
			{					
				DPF ((DBG_WARNING, "Unable to create texture - no more room in name table"));
				gpTextureIF->pfnTextureFree (gHLogicalDev->TexHeap, hTex);
				return (SglError (name)); 	
			}
			
			MapDim = MapSize[map_size];
			nMapSize = sizeof(sgl_map_pixel) * MapDim * MapDim;
			
			LocalMap.pixels = (sgl_map_pixel*) SGLMalloc(nMapSize);

			if(LocalMap.pixels == NULL)
			{
				return(SglError(sgl_err_no_mem));
			}

			/* Make the local copy.	*/
			CopyTexture(pixel_data, &LocalMap, MapDim);

			Generate888TextureSpec (map_type, map_size, &TextureSpec);
			TextureSource.Pitch		= MapSize[map_size] << 2;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			
			/*  Do the dither. 	*/
			if(dither)
			{
				DitherMap(&LocalMap, map_type);
			}
			
			TextureSource.pPixels	= LocalMap.pixels;
		
			if (TextureSpec.MipMapped)
			{
				/*  using 2x2 filter to generate the lower mip maps */
			 	if(generate_mipmap == sgl_mipmap_generate_2x2)
				{
					gpTextureIF->pfnAutoMipmap (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(map_size+6),
												&TextureSource);
				}
				/* loading every level of the texture map */
				else 
				{
					index = map_size+5;
					
					/* Do the highest resolution map. */
					gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(index+1),
												&TextureSource);
					index --;
					MapDim >>= 1;
					do
					{
						/* if any level is missing, generate it.  */
						if(generate_mipmap || (filtered_maps == NULL) 
										   || (filtered_maps[index] == NULL))
						{
							/* Halve the resolution of the previous map.  */
							HalveResolution(&LocalMap);
						}
						else
						{
							CopyTexture(filtered_maps[index], &LocalMap, MapDim);
						}
						
						/*  Do the dither. 	*/
						if(dither)
						{
							DitherMap(&LocalMap, map_type);
						}
			
						TextureSource.pPixels	= LocalMap.pixels;
						TextureSource.Pitch		= MapDim << 2;
						gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex,  
													(WHICH_MAP)(index+1), &TextureSource);
					 	index --;
						MapDim >>= 1;
					}while(index >= 0);
				}
			}
			else
			{
				gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED, &TextureSource);
			}
			
			SGLFree(LocalMap.pixels);
		}
	}
	/*
	// Else is this a valid pre-processed map?
	*/
	else 
	{
		sgl_map_types 	p_map_type;
		sgl_map_sizes 	p_map_size;
		
		p_map_type = (sgl_map_types) pixel_data->id[2];
		p_map_size = (sgl_map_sizes) pixel_data->id[3];
		/*
		// Is this data valid?
		*/
		if( (pixel_data->id[0] != 'P') || (pixel_data->id[1] != 'T') ||
			(p_map_type < sgl_map_16bit) || (p_map_type > sgl_map_trans16_mm) ||
			(p_map_size < sgl_map_32x32) || (p_map_size > sgl_map_256x256) )
		{
			return(SglError(sgl_err_bad_parameter));
		}

		/*
		// Have we been asked to overide the texture type?
		*/
		if(overide)
		{
			/*
			// OK we will only do this if 
			//  1) the supplied preprocessed texture is a MIP map
			//	2) the requested type is a mip map (we could also do
			//	   non mip mapped 16 bit as well but I couldnt give a
			//	   damn at the moment.
			//	3) The supplied type is >= in size to the current one
			//
			//	4) They are BOTH translucent OR non-translucent.
			//
			// NOTE we can do this because of the order in which textures
			// are stored.
			*/
			if((p_map_type == map_type) &&  /*Both same type*/

			   /*They are MIP mapped*/
			   ((p_map_type == sgl_map_16bit_mm) ||
			    (p_map_type == sgl_map_trans16_mm)) &&

			   /* The new size is < the current size */
			   (map_size <  p_map_size))
			{
				p_map_type = (sgl_map_types) map_type;
				p_map_size = (sgl_map_sizes) map_size;
			}			  
		}/*end if overide the types*/
			
		GenerateTextureSpec (p_map_type, p_map_size, &TextureSpec);

		hTex = gpTextureIF->pfnTextureCreate (gHLogicalDev->TexHeap, &TextureSpec);

		if (!hTex)
		{
			DPFDEV ((DBG_WARNING, "Unable to create texture - no more tex mem"));
			return(SglError(sgl_err_texture_memory_full));			
		}
		else
		{
			/*
			** add the texture map structure to the nametable
			*/
		    name = AddNamedItem (dlUserGlobals.pNamtab, hTex, nt_texture);
	
			/*
			// if no name space left
			*/
			if(name < 0)
			{					
				DPF ((DBG_WARNING, "Unable to create texture - no more room in name table"));
				gpTextureIF->pfnTextureFree (gHLogicalDev->TexHeap, hTex);
				return (SglError (name)); 	
			}
			
			if((p_map_type == sgl_map_trans16_mm) || (p_map_type == sgl_map_16bit_mm))
			{
				gpTextureIF->pfnTextureCopy (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(p_map_size+6),
										(sgl_uint16 *)pixel_data->pixels);
			}
			else
			{
				gpTextureIF->pfnTextureCopy (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED,
											(sgl_uint16 *)pixel_data->pixels);
			}
		}
		
	}/* check the type of the input pixels */

	SglError(sgl_no_err);
	return(name);
}



/******************************************************************************
 * Function Name: sgl_create_texture
 *
 * Inputs       : map_type,
 *				  map_size,
 *				  generate_mipmap,
 *				  dither,
 *				  *pixel_data, read only
 *				  *filtered_maps, read only
 * Outputs      : -
 * Returns      : name or error.
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Ref: the API document.
 *****************************************************************************/
extern int CALL_CONV sgl_create_texture ( sgl_map_types map_type,
										  sgl_map_sizes map_size, 
										  sgl_mipmap_generation_options  generate_mipmap,
										  sgl_bool dither,
										  const sgl_intermediate_map *pixel_data,
										  const	sgl_intermediate_map *filtered_maps[] )
{
	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		**	We failed to initialise sgl
		*/

		return(SglError(sgl_err_failed_init));
	}
#endif
	/*
	// Simply call the internal version of the function with overide set to
	// false
	*/	
	return(create_texture_internal (map_type, map_size, 
										generate_mipmap, dither,
										pixel_data, filtered_maps, 
										FALSE));
}


/******************************************************************************
 * Function Name: sgl_direct_set_texture_type
 *
 * Inputs       : name,
 *				  new_map_type
 * Outputs      : -
 * Returns      : name or error.
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : 
  *****************************************************************************/
extern void CALL_CONV sgl_direct_set_texture_type( int Name, 
												   sgl_map_types NewMapType )
{
	HTEXTURE hTex;

	if((NewMapType < sgl_map_16bit) || (NewMapType > sgl_map_trans16_mm))
	{
		DPFDEV((DBG_ERROR,"sgl_create_texture: bad NewMapType"));
		return;
	}

	/*
	// Get hold of the texture
	*/
	hTex = GetNamedItem(dlUserGlobals.pNamtab, Name);

	/*
	//	Insert new map type (only trans<->opaque).
	*/
	hTex->TSPTextureControlWord &= ~MASK_4444_555;

	if ((NewMapType == sgl_map_trans16) || (NewMapType == sgl_map_trans16_mm))
	{
		hTex->TSPTextureControlWord |= MASK_4444_555;
	}
}

/******************************************************************************
 * Function Name: sgl_direct_create_texture
 *
 * Inputs       : map_type,
 *				  map_size,
 *				  *pTextureMem
 * Outputs      : -
 * Returns      : name or error.
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : This version of create makes no attempt at texture processing,
 *				  instead it returns a pointer to the memory to the client can
 *				  fill it instead. I KNOW, this means the client has know about
 *				  twiddling, but thats tough ! This is just the easiest way for
 *				  me to prototype up max speed load FN's without buggering 
 *				  around with cross platform stuff. 
  *****************************************************************************/
extern int CALL_CONV sgl_direct_create_texture( sgl_map_types map_type,
											    sgl_map_sizes map_size, 
						/* Unused parameter */	sgl_uint32 *pTextureMem )
{

	sgl_uint32		SpaceRequired;
	HTEXTURE		hTex;
	TEXTURESPEC 	TextureSpec;
	int 			name;
	
	if((map_type < sgl_map_16bit) || (map_type > sgl_map_trans16_mm))
	{
		DPFDEV((DBG_ERROR,"sgl_direct_create_texture: bad map_type"));
		return(SglError(sgl_err_bad_parameter));
	}

	if((map_size < sgl_map_32x32) || (map_size > sgl_map_256x256))
	{
		DPFDEV((DBG_ERROR,"sgl_direct_create_texture: bad map_size"));
		return(SglError(sgl_err_bad_parameter));
	}

	/*
	//work out the size of the of the texture to be allocated
	// If a MIP map, increase the size to improve page alignment.
	*/
	SpaceRequired = TextureSize(map_type, map_size);

	if(map_type == sgl_map_trans16_mm || map_type == sgl_map_16bit_mm)
	{
		SpaceRequired += ALIGNMENT_PADDING_U8s;
	}

	/*
	** construct the name table entry.
	*/
	GenerateTextureSpec(map_type, map_size, &TextureSpec);

	hTex = gpTextureIF->pfnTextureCreate(gHLogicalDev->TexHeap, &TextureSpec);

	if(hTex == NULL)
	{		
		DPFDEV((DBG_ERROR,"sgl_direct_create_texture: no more texture memory"));
		gpTextureIF->pfnTextureFree(gHLogicalDev->TexHeap, hTex);
		return (SglError (sgl_err_no_mem));
	}

	/*
	** add the texture map structure to the nametable
	*/
    name=AddNamedItem(dlUserGlobals.pNamtab, hTex, nt_texture);
	
	/*
	// if no name space left
	*/
	if(name < 0)
	{					
		gpTextureIF->pfnTextureFree(gHLogicalDev->TexHeap, hTex);
		return(SglError(name)); 	
	}

	SglError(sgl_no_err);
	return(name);

}

#include "tmalloc.h"
typedef struct
{
  PTEXTURESPEC pTextureSpec;
  MNODE_BLOCK MemBlock;

} TPRIVATEDATA;

/******************************************************************************
 * Function Name: sgl_direct_set_texture
 *
 * Inputs       : 
 *				  
 * Outputs      : 
 * Returns      : -
 * Globals Used : 
 *
 * Description  : This function takes a myriad of pixel formats and converts 
 *				  these to an appropriate PVR format and stores it in the 
 *				  texture memory directly. It avoids making copies/shifting
 *				  data to a reasonable extent.
 *
 *				  The formats supported are:  4444, 555, 1555, 555ck, 565, 565ck,
 *				  4444 and 555 palettes.  4444 palette can have colour key.			
 *				  				  
 * Pre-condition: 
 *****************************************************************************/

extern void CALL_CONV sgl_direct_set_texture( int TextureName,
											  int MapLevel,

									/* IGNORED!!
									// New map type, can't switch between 
									// mipmap and non mipmap, 
									// trans <-> opaque switch only.
									*/
											  sgl_map_types NewMapType,

									/*
									// Faster enumerated types
									*/
											  sgl_direct_srctype EnumSrcType,

									/* IGNORED!!
									// Integer source scale factors (1,2,3 etc).
									*/
		   									  int SrcX,
											  int SrcY,

											  const void *pPixelData,

									/*
									// The stride value of source pixels in the texture,
									// is only used for palettised textures now.
									*/
											  sgl_uint32 StrideInUINT32,
									/*
									// Pixel Format Either RAW or the
									// format in the palette, the field
									// pPixformat->Alpha0IsOpaque must be set (TURE/FALSE).
									*/
											  const sgl_direct_pixformat_struct *pPixFormat,

									/*
									// Special Alpha Information
									*/
											  sgl_bool UseChromaKey,
											  sgl_uint32   ChromaColourOrIndex,

									/*
									// Palette: Set to NULL if not palettised
									*/
											  const void *Palette )
{

	/*
	// The type of named entity. To check against user supplied crap.
	*/
	int type;

	/*
	// Control data for the named texture
	*/
	HTEXTURE hTex;
	int map_size; 
	sgl_bool OGLAlpha;
		
	TEXTURESPEC TextureSpec;
	TEXTURESOURCE TextureSource;
	sgl_map_types map_type;
	TPRIVATEDATA *pTPD;

	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		**	We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	if(pPixelData==NULL)
	{
		DPFDEV((DBG_ERROR,"sgl_direct_set_texture: NULL pPixelData"));
		SglError(sgl_err_bad_parameter);
		return;
	}		
	
	/*
	** read in the name table entry.
	*/
	type=GetNamedItemType(dlUserGlobals.pNamtab,TextureName);

	if(type != nt_texture)
	{
		DPFDEV((DBG_ERROR,"sgl_direct_set_texture: bad TextureName"));
		SglError(sgl_err_bad_name);
		return;
	}

	/*
	// Check that the enumerated type is valid
	*/
	if((EnumSrcType < sgl_direct_src_use_pixformat) || 
	   (EnumSrcType > sgl_direct_src_fast_palette_555))
	{
		DPFDEV((DBG_ERROR,"sgl_direct_set_texture: bad EnumSrcType"));
		SglError(sgl_err_bad_parameter);
		return;
	}

	/*
	// Get hold of the texture
	*/
	hTex = GetNamedItem(dlUserGlobals.pNamtab, TextureName);
		
	/*
	// pick out the map size and type etc.
	*/
	map_size=(hTex->TSPTextureControlWord >> SHIFT_MAP_SIZE) & 3;

	/* if the destination (Spec) has the flag TF_OGLALPHA set? */
	pTPD = hTex->pPrivateData;				
	OGLAlpha = (pTPD->pTextureSpec->pFormat->Flags & TF_OGLALPHA) != 0;

	/*
	// handle the format cases
	*/
	switch(EnumSrcType)
	{
	   	/*
		// 4444 format
		*/    
		case sgl_direct_src_4444:
		{
		    if(!(hTex->TSPTextureControlWord & MASK_4444_555))
		    {
				SglError(sgl_err_bad_parameter);
				return;
		    }
#if 0
		    /*  make sure MASK_4444_555 and AlphaMask is set correctly */ 
			hTex->TSPTextureControlWord |= MASK_4444_555; 
			pTPD->pTextureSpec->pFormat->AlphaMask =  0x0000F000;
#endif			
		    if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_trans16_mm;
			else
					map_type = sgl_map_trans16;
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;
		   
		   	/* reversed alpha */
		   	if((pPixFormat->Alpha0IsOpaque && !OGLAlpha) ||
				(!pPixFormat->Alpha0IsOpaque && OGLAlpha))
			{
				TextureSource.pFormat->Flags &= ~TF_OGLALPHA;
			}		
			break;
		}

		/*
		// 555 format
		*/    
		case sgl_direct_src_555:
		{
		    if(hTex->TSPTextureControlWord & MASK_4444_555)
		    {
				SglError(sgl_err_bad_parameter);
				return;
		    }
#if 0
		    /*  make sure MASK_4444_555 is set correctly */ 
		   	hTex->TSPTextureControlWord &= ~MASK_4444_555;
			pTPD->pTextureSpec->pFormat->AlphaMask =  0x00000000;
#endif
		    if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_16bit_mm;
			else
					map_type = sgl_map_16bit;
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;

		   	break;
		}

		/*
		// 1555 format
		*/    
		case sgl_direct_src_1555:
		{
		    if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_trans16_mm;
			else
					map_type = sgl_map_trans16;
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;
		   	
			/* set the RGBA Masks for 1555 textures */
		   	TextureSource.pFormat->rgbyuv.rgb.RedMask =   0x00007C00;
			TextureSource.pFormat->rgbyuv.rgb.GreenMask = 0x000003E0;
			TextureSource.pFormat->rgbyuv.rgb.BlueMask =  0x0000001F;
			TextureSource.pFormat->AlphaMask =            0x00008000;

		   	/* reversed alpha */
		   	if((pPixFormat->Alpha0IsOpaque && !OGLAlpha) ||
			   (!pPixFormat->Alpha0IsOpaque && OGLAlpha))
			{
				TextureSource.pFormat->Flags &= ~TF_OGLALPHA;
			}		
									
			break;
		}

		/*
		// 555CK format
		*/    
		case sgl_direct_src_555CK:
		{
		    if(!UseChromaKey)
			{
				SglError(sgl_err_bad_parameter);
				return;
			}

		    if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_16bit_mm;
			else
					map_type = sgl_map_16bit;
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;	
		   	TextureSource.pFormat->Flags |= TF_COLOURKEY;
		   	TextureSource.pFormat->KeyColour = ChromaColourOrIndex;
			TextureSource.pFormat->Flags &= ~TF_HWSUPPORT;
			
			break;
		}

		/*
		// 565 format
		*/    
		case sgl_direct_src_565:
		{
		    if(hTex->TSPTextureControlWord & MASK_4444_555)
		    {
				SglError(sgl_err_bad_parameter);
				return;
		    }
#if 0
		    /*  make sure MASK_4444_555 is set correctly */ 
			hTex->TSPTextureControlWord &= ~MASK_4444_555;
			pTPD->pTextureSpec->pFormat->AlphaMask =  0x00000000;
#endif
		    if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_16bit_mm;
			else
					map_type = sgl_map_16bit;
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;
		
			/* set the RG Masks for 565 textures */
		   	TextureSource.pFormat->rgbyuv.rgb.RedMask =   0x0000F800;
			TextureSource.pFormat->rgbyuv.rgb.GreenMask = 0x000007E0;
			TextureSource.pFormat->Flags &= ~TF_HWSUPPORT;
			
			break;
		}

		/*
		// 565CK format
		*/    
		case sgl_direct_src_565CK:
		{
		    if(!UseChromaKey)
			{
				SglError(sgl_err_bad_parameter);
				return;
			}

		    if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_16bit_mm;
			else
					map_type = sgl_map_16bit;
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;	
		   	TextureSource.pFormat->Flags |= TF_COLOURKEY;
		   	TextureSource.pFormat->KeyColour = ChromaColourOrIndex;

			/* set the RG Masks for 565 textures */
		   	TextureSource.pFormat->rgbyuv.rgb.RedMask =   0x0000F800;
			TextureSource.pFormat->rgbyuv.rgb.GreenMask = 0x000007E0;
			TextureSource.pFormat->Flags &= ~TF_HWSUPPORT;
			
			break;
		}
		
		/*  fast palettised 4444 or 555 formats  */    
		case sgl_direct_src_fast_palette_4444:
		case sgl_direct_src_fast_palette_555:
		{			
			if(EnumSrcType == sgl_direct_src_fast_palette_4444)
			{
				if(!(hTex->TSPTextureControlWord & MASK_4444_555))
		   		{
					SglError(sgl_err_bad_parameter);
					return;
		   		}
#if 0
				/*  make sure MASK_4444_555 is set correctly */ 
				hTex->TSPTextureControlWord |= MASK_4444_555;	 
				pTPD->pTextureSpec->pFormat->AlphaMask =  0x0000F000;
#endif				
				if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_trans16_mm;
				else
					map_type = sgl_map_trans16;
			}
			else
			{
				if(hTex->TSPTextureControlWord & MASK_4444_555)
		    	{
				 	SglError(sgl_err_bad_parameter);
					return;
		  		}
#if 0
				/*  make sure MASK_4444_555 is set correctly */ 
				hTex->TSPTextureControlWord &= ~MASK_4444_555;
#endif				
				if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
					map_type = sgl_map_16bit_mm;
				else
					map_type = sgl_map_16bit;
			} 
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= StrideInUINT32;
			TextureSource.pFormat 	= TextureSpec.pFormat;
			TextureSource.pPixels	= (void*) pPixelData;
			TextureSource.pFormat->Flags &= ~TF_HWSUPPORT;
			TextureSource.pFormat->Flags |= TF_PALETTISED8;
			TextureSource.pPalette	= (void*) Palette;
		
			if(EnumSrcType == sgl_direct_src_fast_palette_4444)
			{
				/* have colour key */
				if(UseChromaKey)
				{
					TextureSource.pFormat->Flags |= TF_COLOURKEY;
					TextureSource.pFormat->KeyColour = ChromaColourOrIndex;
				}
				/* reversed alpha */
				if((pPixFormat->Alpha0IsOpaque && !OGLAlpha) ||
					(!pPixFormat->Alpha0IsOpaque && OGLAlpha))
				{
					TextureSource.pFormat->Flags &= ~TF_OGLALPHA;
				}
			}
			break;
		}
		
	   	case sgl_direct_src_use_pixformat:
		{
			/* we only support 16 or 32 bit textures */
			if((pPixFormat->BytesPerPixel != 2) && (pPixFormat->BytesPerPixel != 4))
			{
				SglError(sgl_err_bad_parameter);
				return;
			}

			if((!(hTex->TSPTextureControlWord & MASK_4444_555) &&  pPixFormat->AlphaMask) ||
			   ((hTex->TSPTextureControlWord & MASK_4444_555) &&  
			    !(pPixFormat->AlphaMask) && !UseChromaKey))  
		   	{
				SglError(sgl_err_bad_parameter);
				return;
		   	}

			map_type = sgl_map_16bit; 
			
			/* set texture spec  */
			GenerateTextureSpec (map_type, (sgl_map_sizes)map_size, &TextureSpec);

			/* set texture source  */
			TextureSource.Pitch		= MapSize[map_size] << 1;
			TextureSource.pFormat 	= TextureSpec.pFormat;
		   	TextureSource.pPixels	= (void*) pPixelData;
			TextureSource.pFormat->Flags &= ~TF_HWSUPPORT;

			/* 32 bit texture   */
			if(pPixFormat->BytesPerPixel == 4)
			{
				TextureSource.Pitch		= MapSize[map_size] << 2;
				TextureSource.pFormat->BitDepth = 32;
			}
					   	
			/* set the RGBA Masks */
		   	TextureSource.pFormat->rgbyuv.rgb.RedMask =   pPixFormat->RedMask;
			TextureSource.pFormat->rgbyuv.rgb.GreenMask = pPixFormat->GreenMask;
			TextureSource.pFormat->rgbyuv.rgb.BlueMask =  pPixFormat->BlueMask;
			TextureSource.pFormat->AlphaMask =            pPixFormat->AlphaMask;

		   	/* have colour key */
			if(UseChromaKey)
			{
				TextureSource.pFormat->Flags |= TF_COLOURKEY;
				TextureSource.pFormat->KeyColour = ChromaColourOrIndex;
			}

		   	/* translucent textures */
			if(hTex->TSPTextureControlWord & MASK_4444_555)
			{
				TextureSource.pFormat->Flags |= TF_TRANSLUCENT |  TF_OGLALPHA;
			}

		   	/* reversed alpha */
		   	if((pPixFormat->Alpha0IsOpaque && !OGLAlpha) ||
			   (!pPixFormat->Alpha0IsOpaque && OGLAlpha))
			{
				TextureSource.pFormat->Flags &= ~TF_OGLALPHA;
			}
					
			break;						
		}
	}/*end switch*/

	if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
	{
		gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(MapLevel+1),
										&TextureSource);
	}
	else
	{
		gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED,
										&TextureSource);
	}
	
	return;

}/*end sgl_direct_set_texture */

/******************************************************************************
 * Function Name: sgl_set_texture
 *
 * Inputs       : texture_name,
 *				  generate_mipmap,
 *				  dither,
 *				  *pixel_data, read only
 *				  *filtered_maps, read only
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Ref: the API document.
 *****************************************************************************/
extern void CALL_CONV sgl_set_texture ( int texture_name,
									 	sgl_mipmap_generation_options  generate_mipmap,
									 	sgl_bool dither,
										const sgl_intermediate_map *pixel_data,
							   			const sgl_intermediate_map *filtered_maps[] )
{
	int type;
	sgl_map_types map_type;
	sgl_map_sizes map_size;
	sgl_intermediate_map LocalMap;
	int MapDim, index;
	sgl_uint32 nMapSize; 
	
	HTEXTURE hTex;
	TEXTURESPEC TextureSpec;
	TEXTURESOURCE TextureSource;
	sgl_bool Translucent;

	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		**	We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	if(pixel_data==NULL)
	{
		DPFDEV((DBG_ERROR,"sgl_set_texture: NULL pixel_data"));

		SglError(sgl_err_bad_parameter);
		return;
	}		
	
	/*
	** read in the name table entry.
	*/

	type=GetNamedItemType(dlUserGlobals.pNamtab,texture_name);

	if(type != nt_texture)
	{
		DPFDEV((DBG_ERROR,"sgl_set_texture: bad texture_name"));
		SglError(sgl_err_bad_name);
		return;
	}

	/*
	** Decode the texture control word.
	*/


	hTex = GetNamedItem(dlUserGlobals.pNamtab,texture_name);

   	/*pick out the map size*/

	map_size=(sgl_map_sizes)((hTex->TSPTextureControlWord >> SHIFT_MAP_SIZE)& 3);

    Translucent=(hTex->TSPTextureControlWord & MASK_4444_555) != 0;

	if(hTex->TSPTextureControlWord & MASK_8_16_MAPS==0)
		map_type = sgl_map_8bit;
	else
	{
		if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
		{
			if(Translucent)
				map_type = sgl_map_trans16_mm;
			else
				map_type = sgl_map_16bit_mm;
		}
		else
		{
			if(Translucent)
				map_type = sgl_map_trans16;
			else
				map_type = sgl_map_16bit;
		}
	}

	GenerateTextureSpec (map_type, map_size, &TextureSpec);

	/*********************/
	/*
	// Look at the type of the supplied intermediate map. If this
	// is a user defined map, then "preprocess" it into 
	*/
	if((pixel_data->id[0]=='I') && (pixel_data->id[1]=='M') &&
	   (pixel_data->id[2]=='A') && (pixel_data->id[3]=='P'))
	{
		MapDim = MapSize[map_size];
		nMapSize = sizeof(sgl_map_pixel) * MapDim * MapDim;
	
		LocalMap.pixels = (sgl_map_pixel*) SGLMalloc(nMapSize);

		if(LocalMap.pixels == NULL)
		{
			return;
		}

		/* Make the local copy.	*/
		CopyTexture(pixel_data, &LocalMap, MapDim);

		Generate888TextureSpec (map_type, map_size, &TextureSpec);
		TextureSource.Pitch		= MapSize[map_size] << 2;
		TextureSource.pFormat 	= TextureSpec.pFormat;
			
		/*  Do the dither. 	*/
		if(dither)
		{
			DitherMap(&LocalMap, map_type);
		}
			
		TextureSource.pPixels	= LocalMap.pixels;
		
		if (TextureSpec.MipMapped)
		{
			/*  using 2x2 filter to generate the lower mip maps */
		 	if(generate_mipmap == sgl_mipmap_generate_2x2)
			{
				gpTextureIF->pfnAutoMipmap (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(map_size+6),
											&TextureSource);
			}
			/* loading every level of the texture map */
			else 
			{
				index = map_size+5;
					
				/* Do the highest resolution map. */
				gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(index+1),
												&TextureSource);
				index --;
				MapDim >>= 1;
				do
				{
					/* if any level is missing, generate it.  */
					if(generate_mipmap || (filtered_maps == NULL) 
									   || (filtered_maps[index] == NULL))
					{
						/* Halve the resolution of the previous map.  */
						HalveResolution(&LocalMap);
					}
					else
					{
						CopyTexture(filtered_maps[index], &LocalMap, MapDim);
					}
						
					/*  Do the dither. 	*/
					if(dither)
					{
						DitherMap(&LocalMap, map_type);
					}
			
					TextureSource.pPixels	= LocalMap.pixels;
					TextureSource.Pitch		= MapDim << 2;
					gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex,  
												(WHICH_MAP)(index+1), &TextureSource);
				 	index --;
					MapDim >>= 1;
				}while(index >= 0);
			}
		}
		else
		{
			gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED, &TextureSource);
		}
		SGLFree(LocalMap.pixels);
	}
	/*
	// Else is this a valid pre-processed map?
	*/
	else 
	{
		sgl_map_types pre_map_type;
		sgl_map_sizes pre_map_size;


		pre_map_type = (sgl_map_types) pixel_data->id[2];
		pre_map_size = (sgl_map_sizes) pixel_data->id[3];

		/*
		// Is this data valid?
		*/

		if( (pixel_data->id[0] != 'P') || (pixel_data->id[1] != 'T') ||
			(pre_map_type < sgl_map_16bit) ||(pre_map_type > sgl_map_trans16_mm)||
			(pre_map_size < sgl_map_32x32) ||(pre_map_size > sgl_map_256x256))
		{
			SglError(sgl_err_bad_parameter);
			return;
		}

		/*
		// Does it match the already stored version
		*/
		if((pre_map_type != map_type) || (pre_map_size != map_size))
		{
			SglError(sgl_err_bad_parameter);
			return;
		}

		if((pre_map_type == sgl_map_trans16_mm) || (pre_map_type == sgl_map_16bit_mm))
		{
			gpTextureIF->pfnTextureCopy (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(map_size+6),
											(sgl_uint16 *)pixel_data->pixels);
		}
		else
		{
			gpTextureIF->pfnTextureCopy (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED,
											(sgl_uint16 *)pixel_data->pixels);
		}
	}/* check the type of the input pixels */

	SglError(sgl_no_err);
}

/******************************************************************************
 * Function Name: sgl_set_texture_extended
 *
 * Inputs       : texture_name,
 *				  new_map_type,
 *				  new_map_size, 
 *				  generate_mipmap,
 *				  dither,
 *				  *pixel_data, read only
 *				  *filtered_maps, read only
 * Outputs      : -
 * Returns      : sgl_err
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Ref: the API document.
 *****************************************************************************/
extern int CALL_CONV sgl_set_texture_extended (
									int texture_name,
									sgl_map_types new_map_type,
									sgl_map_sizes new_map_size, 
									sgl_mipmap_generation_options  generate_mipmap,
									sgl_bool dither,
							   		const sgl_intermediate_map *pixel_data,
							   		const sgl_intermediate_map *filtered_maps[] )
{
	int type;
	sgl_map_types map_type;
	sgl_map_sizes map_size;
	sgl_intermediate_map LocalMap;
	int MapDim, index;
	sgl_uint32 nMapSize; 
	
	HTEXTURE hTex,hTexNew;
	TEXTURESPEC TextureSpec;			
	TEXTURESOURCE TextureSource;

	sgl_bool Translucent;

	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		**	We failed to initialise sgl
		*/
		SglError (sgl_err_failed_init);
		return(sgl_err_failed_init);
	}
#endif
	if(pixel_data==NULL)
	{
		DPFDEV((DBG_ERROR,"sgl_set_texture_extended: NULL pixel_data"));
		SglError (sgl_err_bad_parameter);
		return (sgl_err_bad_parameter);
	}		
	
	/*
	** read in the name table entry.
	*/

	type=GetNamedItemType(dlUserGlobals.pNamtab,texture_name);

	if(type != nt_texture)
	{
		DPFDEV((DBG_ERROR,"sgl_set_texture_extended: texture_name"));
		SglError (sgl_err_bad_name);
		return (sgl_err_bad_name);
	}

	/*
	** Decode the texture control word.
	*/

	hTex = GetNamedItem(dlUserGlobals.pNamtab,texture_name);

	/*pick out the map size*/

	map_size=(sgl_map_sizes)((hTex->TSPTextureControlWord >> SHIFT_MAP_SIZE) & 3);

    Translucent=(hTex->TSPTextureControlWord & MASK_4444_555) != 0;

	if(hTex->TSPTextureControlWord & MASK_8_16_MAPS==0)
		map_type = sgl_map_8bit;
	else
	{
		if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
		{
			if(Translucent)
				map_type = sgl_map_trans16_mm;
			else
				map_type = sgl_map_16bit_mm;
		}
		else
		{
			if(Translucent)
				map_type = sgl_map_trans16;
			else
				map_type = sgl_map_16bit;
		}
	}

	/*********************/
	/*
	// Look at the type of the supplied intermediate map. If this
	// is a user defined map, then "preprocess" it into 
	*/
	if((pixel_data->id[0]=='I') && (pixel_data->id[1]=='M') &&
	   (pixel_data->id[2]=='A') && (pixel_data->id[3]=='P'))
	{
		if ((new_map_size != map_size) || (new_map_type != map_type))
		{
			/*
			//try and allocate the space in the texture ram.
			*/

			GenerateTextureSpec(new_map_type, new_map_size, &TextureSpec);
			hTexNew = gpTextureIF->pfnTextureCreate(gHLogicalDev->TexHeap, &TextureSpec);

			if (hTexNew == NULL)
			{
				/* NB the current texture should still be valid! */
				return (SglError (sgl_err_texture_memory_full));
			}

			/* free old texture */
		
			gpTextureIF->pfnTextureFree(gHLogicalDev->TexHeap, hTex);
		
			hTex = hTexNew;
		}
		else
		{
			GenerateTextureSpec(map_type, map_size, &TextureSpec);
		}
		
		MapDim = MapSize[map_size];
		nMapSize = sizeof(sgl_map_pixel) * MapDim * MapDim;
	
		LocalMap.pixels = (sgl_map_pixel*) SGLMalloc(nMapSize);

		if(LocalMap.pixels == NULL)
		{
			return(SglError(sgl_err_no_mem));
		}

		/* Make the local copy.	*/
		CopyTexture(pixel_data, &LocalMap, MapDim);

		Generate888TextureSpec (map_type, map_size, &TextureSpec);
		TextureSource.Pitch		= MapSize[map_size] << 2;
		TextureSource.pFormat 	= TextureSpec.pFormat;
			
		/*  Do the dither. 	*/
		if(dither)
		{
			DitherMap(&LocalMap, map_type);
		}
			
		TextureSource.pPixels	= LocalMap.pixels;
		
		if (TextureSpec.MipMapped)
		{
			/*  using 2x2 filter to generate the lower mip maps */
		 	if(generate_mipmap == sgl_mipmap_generate_2x2)
			{
				gpTextureIF->pfnAutoMipmap (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(map_size+6),
											&TextureSource);
			}
			/* loading every level of the texture map */
			else 
			{
				index = map_size+5;
					
				/* Do the highest resolution map. */
				gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(index+1),
												&TextureSource);
				index --;
				MapDim >>= 1;
				do
				{
					/* if any level is missing, generate it.  */
					if(generate_mipmap || (filtered_maps == NULL) 
									   || (filtered_maps[index] == NULL))
					{
						/* Halve the resolution of the previous map.  */
						HalveResolution(&LocalMap);
					}
					else
					{
						CopyTexture(filtered_maps[index], &LocalMap, MapDim);
					}
						
					/*  Do the dither. 	*/
					if(dither)
					{
						DitherMap(&LocalMap, map_type);
					}
			
					TextureSource.pPixels	= LocalMap.pixels;
					TextureSource.Pitch		= MapDim << 2;
					gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex,  
												(WHICH_MAP)(index+1), &TextureSource);
				 	index --;
					MapDim >>= 1;
				}while(index >= 0);
			}
		}
		else
		{
			gpTextureIF->pfnTextureLoad (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED, &TextureSource);
		}
		SGLFree(LocalMap.pixels);
	}
	/*
	// Else is this a valid pre-processed map?
	*/
	else 
	{
		sgl_map_types pre_map_type;
		sgl_map_sizes pre_map_size;

		pre_map_type = (sgl_map_types) pixel_data->id[2];
		pre_map_size = (sgl_map_sizes) pixel_data->id[3];

		/*
		// Is this data valid?
		*/

		if( (pixel_data->id[0] != 'P') || (pixel_data->id[1] != 'T') ||
			(pre_map_type < sgl_map_16bit) ||(pre_map_type > sgl_map_trans16_mm)||
			(pre_map_size < sgl_map_32x32) ||(pre_map_size > sgl_map_256x256))
		{
			SglError (sgl_err_bad_parameter);
			return (sgl_err_bad_parameter);
		}
		
		if ((pre_map_size != map_size) || (pre_map_type != map_type))
		{
			/*
			//try and allocate the space in the texture ram.
			*/

			GenerateTextureSpec(pre_map_type, pre_map_size, &TextureSpec);
			hTexNew = gpTextureIF->pfnTextureCreate(gHLogicalDev->TexHeap, &TextureSpec);

			if (hTexNew == NULL)
			{
				/* NB the current texture should still be valid! */
				return (SglError (sgl_err_texture_memory_full));
			}

			/* free old texture */
		
			gpTextureIF->pfnTextureFree(gHLogicalDev->TexHeap, hTex);
		
			hTex = hTexNew;
		}
		else
		{
			GenerateTextureSpec(map_type, map_size, &TextureSpec);
		}
		
		if((pre_map_type == sgl_map_trans16_mm) || (pre_map_type == sgl_map_16bit_mm))
		{
			gpTextureIF->pfnTextureCopy (gHLogicalDev->TexHeap, hTex, (WHICH_MAP)(pre_map_size+6),
										(sgl_uint16 *)pixel_data->pixels);
		}
		else
		{
			gpTextureIF->pfnTextureCopy (gHLogicalDev->TexHeap, hTex, NON_MIPMAPED,
											(sgl_uint16 *)pixel_data->pixels);
		}

	}/* check the type of the input pixels */

	return (SglError(sgl_no_err));
}

/******************************************************************************
 * Function Name: sgl_delete_texture
 *
 * Inputs       : texture_name
 * Outputs      : -
 * Returns      : -
 * Globals Used : dlUserGlobals.pNamtab
 *
 * Description  : Deallocates the texture space and removes the name.
 *****************************************************************************/
extern void  CALL_CONV sgl_delete_texture ( int texture_name )
{
	HTEXTURE hTex;
	int	type;

	/*	
	**	Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
		**	We failed to initialise sgl
		*/

		SglError(sgl_err_failed_init);
		return;
	}
#endif
	type=GetNamedItemType(dlUserGlobals.pNamtab,texture_name);


	/*
	// If this is a cached texture, call its delete routine
	*/
	if(type == nt_cached_texture)
	{
		delete_cached_texture(texture_name);
		return;
	}
	else if(type != nt_texture)
	{
		DPFDEV((DBG_ERROR,"sgl_delete_texture: bad texture_name"));
		SglError(sgl_err_bad_name);
		return;
	}

	hTex = GetNamedItem(dlUserGlobals.pNamtab,texture_name);
	
	/*
	**	Free the texture memory.
	*/

	gpTextureIF->pfnTextureFree(gHLogicalDev->TexHeap, hTex);
	 
	/*
	**	Remove the name
	*/

	DeleteNamedItem(dlUserGlobals.pNamtab,texture_name);

	SglError(sgl_no_err);
}

/**************************************************************************
 *		Texture Caching Routines
 **************************************************************************/

/**************************************************************************
 * Function Name  : InitCachedTextures
 * Inputs         : NONE
 * Outputs        : NONE
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : cached Texture array
 *
 * Description    : Initialises the cached texture structure to empty.
 *					Should be called by sgkInit (or whatever it is called)
 **************************************************************************/
void InitCachedTextures()
{
	int i;
	CACHED_TEXT_STRUCT_TYPE *pEntry;

	/*
	// Step through all the cached texture entrys, setting up pointers to
	// the next free entry
	*/
	pEntry = CachedTextures;
	for(i = MAX_CACHED_TEXTURES; i != 0; i--, pEntry++)
	{
		pEntry->sglName = NM_INVALID_NAME;
		
		/*
		// For safety sake, also initialise the actual texture name field
		// to invalid as well
		*/
		pEntry->loadedName = NM_INVALID_NAME;

		/*
		// Set up a pointer to the next free entry
		*/
		pEntry->pNextFree = pEntry+1;


		/*
		// For safety sake, initialise these as well
		*/
		pEntry->bTextureNeeded	  = FALSE;
		pEntry->bUsedLastRender  = FALSE;

	}/*end for*/

	/*
	// Make sure the last one points to NULL!
	*/
	pEntry[-1].pNextFree = NULL;


	/*
	// Set the free pointer to point to the first entry.
	*/
	pFreeCTEntry= CachedTextures;

	/*
	// Init the number used
	*/
	nCachedTextures = 0;


	/*
	// Initialise the texture callback stuff
	*/
	TextureCallBackFunction = NULL;
	UserTexDataArray		= NULL;
	UserTexDataSize			= -1;

}



/**************************************************************************
 * Function Name  : sgl_create_cached_texture
 * Inputs         : user_id
 * Outputs        : 
 *
 * Input/Output	  : None
 * Returns        : SGL Name for cached texture, or
 * Global Used    : None
 *
 * Description    : 
 **************************************************************************/
int CALL_CONV sgl_create_cached_texture( long user_id )
{
	int name;
	CACHED_TEXT_STRUCT_TYPE * pNextFree;

	/*
	//initialise the system if need be
	*/
#if !WIN32
    if (SglInitialise())
	{
		return SglError(sgl_err_failed_init);
	}
#endif
	/*
	//First find a free spot in the list
	*/
	if(pFreeCTEntry == NULL)
	{
		/*
		// If there are no free spaces, then get out of here quickly
		//
		//Should really come up with a better error value.
		*/
		return SglError(sgl_err_no_name);
	}


	/*
	// See if we can allocate a name for this entry
	*/
	name = AddNamedItem(dlUserGlobals.pNamtab,
			  			pFreeCTEntry, nt_cached_texture);


	if(name == NM_INVALID_NAME)
	{
		return SglError(sgl_err_no_name);
	}

	/*
	// Ok, fill in the details. Note we havent loaded a texture yet, so
	// initialises the loadedName to Invalid
	*/
	pFreeCTEntry->sglName   = name;
	pFreeCTEntry->UserId = user_id;
	pFreeCTEntry->loadedName= NM_INVALID_NAME;

	/*
	// Initialise useage
	*/
	pFreeCTEntry->bTextureNeeded 	= FALSE;
	pFreeCTEntry->bUsedLastRender  = FALSE;

	/*
	// Set up the next free entry, and for safety, clear the next free
	// pointer
	*/
	pNextFree = pFreeCTEntry->pNextFree;
	pFreeCTEntry->pNextFree = NULL;
	pFreeCTEntry=pNextFree;
	
	/*
	// Increment the number used
	*/
	nCachedTextures++;
	ASSERT(nCachedTextures > 0);

	SglError(sgl_no_err);

	return(name);
	
}


/**************************************************************************
 * Function Name  : GetRealTexture
 * Inputs         : 
 * Outputs        : 
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : Returns a pointer to the real texture data - if any - else
 *					NULL
 **************************************************************************/
void* GetRealTexture(void *CachedTexturePointer)
{
	CACHED_TEXT_STRUCT_TYPE * pCachedTex = CachedTexturePointer;

	
	if(pCachedTex->loadedName == NM_INVALID_NAME)
	{
		return NULL;
	}
	else
	{
		return(GetNamedItem(dlUserGlobals.pNamtab, 	pCachedTex->loadedName));
	}

}

/**************************************************************************
 * Function Name  : sgl_load_cached_texture
 * Inputs         : 
 * Outputs        : 
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : 
 **************************************************************************/
void CALL_CONV sgl_load_cached_texture ( int name, 
										 sgl_map_types map_type,
										 sgl_map_sizes map_size, 
										 sgl_mipmap_generation_options  generate_mipmap,
										 sgl_bool dither,
										 const	sgl_intermediate_map *pixel_data,
										 const sgl_intermediate_map *filtered_maps[],
										 sgl_bool overide_prepocessed_type )
{
	int itemType;
	CACHED_TEXT_STRUCT_TYPE * pCachedTex;
	
	int internalTextName;
	
	if((map_type < sgl_map_16bit) || (map_type > sgl_map_trans16_mm))
	{
		DPFDEV((DBG_ERROR,"sgl_load_cached_texture: bad map_type"));
		return;
	}

	if((map_size < sgl_map_32x32) || (map_size > sgl_map_256x256))
	{
		DPFDEV((DBG_ERROR,"sgl_load_cached_texture: bad map_size"));
		return;
	}

	/*
	//initialise the system if need be
	*/
#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// First find the texture name by looking up the name table
	*/
	pCachedTex = GetNamedItemAndType(dlUserGlobals.pNamtab,
						name,&itemType);

	/*
	// Check that it's kosher
	*/
	if((pCachedTex==NULL) || (itemType!=nt_cached_texture))
	{
		/*
		// Bad name. ABORT ABORT ABORT
		*/
		DPFDEV((DBG_ERROR,"sgl_load_cached_texture: bad name"));
		SglError(sgl_err_bad_name);
		
		return;		
	}

	/*
	// Ok, it is valid. Is there a texture already loaded for this? If so
	// DELETE it
	*/
	if(pCachedTex->loadedName != NM_INVALID_NAME)
	{
		/*
		// Debug check. make sure we are referecing a valid texture
		*/
		#if DEBUG
			ASSERT(GetNamedItemType(dlUserGlobals.pNamtab, 
					pCachedTex->loadedName) == nt_texture);
		#endif

		/*
		// Ok delete it
		*/
		sgl_delete_texture(pCachedTex->loadedName);


	}/*end if clear out existing texture*/


	/*
	// Create the texture
	*/
	internalTextName= create_texture_internal (map_type, map_size, 
									generate_mipmap, dither,
									pixel_data, filtered_maps,
									overide_prepocessed_type);


	/*
	// If there was an error creating the texture (then it will have
	// already been recorded, so just get out of here
	*/
	if(internalTextName < 0)
	{
		pCachedTex->loadedName = NM_INVALID_NAME;
		return;
	}

	/*
	// Save the name in the structure
	*/
	pCachedTex->loadedName = internalTextName;


	/*
	// And that should be it.
	*/
	SglError(sgl_no_err);
}
						

/**************************************************************************
 * Function Name  : sgl_unload_cached_texture
 * Inputs         : 
 * Outputs        : 
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : 
 **************************************************************************/
void CALL_CONV sgl_unload_cached_texture( int name ) 
{
	int itemType;
	CACHED_TEXT_STRUCT_TYPE * pCachedTex;
	
	/*
	//initialise the system if need be
	*/
#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// First find the texture name by looking up the name table
	*/
	pCachedTex = GetNamedItemAndType(dlUserGlobals.pNamtab,
						name,&itemType);

	/*
	// Check that it's kosher
	*/
	if((pCachedTex==NULL) || (itemType!=nt_cached_texture))
	{
		/*
		// Bad name. ABORT ABORT ABORT
		*/
		DPFDEV((DBG_ERROR,"sgl_unload_cached_texture: bad name"));
		SglError(sgl_err_bad_name);
		
		return;		
	}

	/*
	// Ok, it is valid. Is there a texture already loaded for this? If so
	// DELETE it
	*/
	if(pCachedTex->loadedName != NM_INVALID_NAME)
	{
		/*
		// Debug check. make sure we are referecing a valid texture
		*/
		#if DEBUG
			ASSERT(GetNamedItemType(dlUserGlobals.pNamtab, 
					pCachedTex->loadedName) == nt_texture);
		#endif

		/*
		// Ok delete it
		*/
		sgl_delete_texture(pCachedTex->loadedName);

		pCachedTex->loadedName = NM_INVALID_NAME;

	}/*end if clear out existing texture*/

	SglError(sgl_no_err);

}
						

/**************************************************************************
 * Function Name  : delete_cached_texture
 * Inputs         : 
 * Outputs        : 
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : This routine may be called by sgl_delete_texture
 **************************************************************************/
static void delete_cached_texture(int 	name)
{
	int itemType;
	CACHED_TEXT_STRUCT_TYPE * pCachedTex;
	
	/*
	// First find the texture name by looking up the name table
	*/
	pCachedTex = GetNamedItemAndType(dlUserGlobals.pNamtab,
						name,&itemType);

	/*
	// Check that it's kosher
	*/
	if((pCachedTex==NULL) || (itemType!=nt_cached_texture))
	{
		/*
		// Bad name. ABORT ABORT ABORT
		*/
		SglError(sgl_err_bad_name);
		
		return;		
	}

	/*
	// Ok, it is valid. Is there a texture already loaded for this? If so
	// DELETE it
	*/
	if(pCachedTex->loadedName != NM_INVALID_NAME)
	{
		/*
		// Debug check. make sure we are referecing a valid texture
		*/
		#if DEBUG
			ASSERT(GetNamedItemType(dlUserGlobals.pNamtab, 
					pCachedTex->loadedName) == nt_texture);
		#endif

		/*
		// Ok delete it
		*/
		sgl_delete_texture(pCachedTex->loadedName);

		/*
		// Do this for paranoia's sake
		*/
		pCachedTex->loadedName = NM_INVALID_NAME;

	}/*end if clear out existing texture*/

	/*
	// Delete the name from the table
	*/
	DeleteNamedItem(dlUserGlobals.pNamtab, name);

	/*
	// remove it from the table as well. 
	// Basically point its "next free" to the start of the current
	// list.
	*/
	pCachedTex->pNextFree = pFreeCTEntry;
	pFreeCTEntry		  = pCachedTex;

	/*
	// For safety clear the rest of the rubbish
	*/
	pCachedTex->sglName = NM_INVALID_NAME;

	/*
	// Decrement the number used
	*/
	nCachedTextures--;
	ASSERT(nCachedTextures>= 0);

	/*
	// All OK
	*/
	SglError(sgl_no_err);
	
}

/**************************************************************************
 * Function Name  : sgl_register_texture_callback
 * Inputs         : func: 		User texture cache call back function
 *				    data_array: Pointer to user data area for where to put
 *								result
 *					array_size: Number of elements in the above array.
 * Outputs        : 
 *
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : 
 **************************************************************************/
void CALL_CONV sgl_register_texture_callback( 
									sgl_tex_callback_func func,
									sgl_tex_callback_struct tex_data_array[],
									int	array_size )
{
	/*
	//initialise the system if need be
	*/
#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return;
	}
#endif
	/*
	// If the parameters are suspect
	*/
	if( (func == NULL) ||
		(tex_data_array	== NULL)  ||
		(array_size	<= 0)	 ||
		(array_size	> 100000))
	{
		TextureCallBackFunction = NULL;
		DPFDEV((DBG_ERROR,"sgl_register_texture_callback: bad parameters"));
		SglError(sgl_err_bad_parameter);
	}
	/*
	//Store the values
	*/
	else
	{
		TextureCallBackFunction = func;
		UserTexDataArray		= tex_data_array;
		UserTexDataSize			= array_size;
		SglError(sgl_no_err);
	}
}


/**************************************************************************
 * Function Name  : ResetCachedTextureUsage
 * Inputs         : 
 * Outputs        : 
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : Simply steps through the cached texture structure, resetting
 *					the "needed" flag
 **************************************************************************/
void ResetCachedTextureUsage()
{
	int NumNotDone;
	CACHED_TEXT_STRUCT_TYPE * pCachedTex;
	
	/*
	// Step through all the cacheable textures
	*/
	NumNotDone = nCachedTextures;
	pCachedTex = CachedTextures;


	ASSERT(NumNotDone>=0);

	while(NumNotDone)
	{
		/*
		// if this is not in use, move along to the next one
		*/
		while(pCachedTex->sglName == NM_INVALID_NAME)
		{
			pCachedTex++;
			ASSERT(pCachedTex < (CachedTextures + MAX_CACHED_TEXTURES));
		}


		/*
		// OK, move the "current" usage to the previous render value
		//
		// (REALLY OUGHT TO TAKE ACCOUNT OF WHETHER RENDERING HAS COMPLETED
		// OR NOT)
		*/
		pCachedTex->bUsedLastRender = pCachedTex->bTextureNeeded;
		pCachedTex->bTextureNeeded  = FALSE;
		pCachedTex->size			 = -1;

		NumNotDone--;
		pCachedTex++;

	}/*end while*/

}

/**************************************************************************
 * Function Name  : MarkCachedTextureUsed
 * Inputs         : CachedTexturePointer: Pointer to the cached texture.
 *					size:	The approx size of the object (in pixels) that uses it.
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 *
 * Description    : 
 **************************************************************************/
void MarkCachedTextureUsed(void * CachedTexturePointer, int size)
{

	CACHED_TEXT_STRUCT_TYPE * pCachedTex = (void*) CachedTexturePointer;

	pCachedTex->bTextureNeeded   = TRUE;

	if(size > pCachedTex->size)
	{
		pCachedTex->size = size;
	}
}


/**************************************************************************
 * Function Name  : ReportCachedTexturesToUser
 * Inputs         : 
 * Outputs        : 
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : None
 *
 * Description    : 
 **************************************************************************/
void ReportCachedTexturesToUser()
{
	int NumToReport;
	int NumToDo;
	CACHED_TEXT_STRUCT_TYPE * pCachedTex;
	sgl_tex_callback_struct * pUserTexData;	
	long free_space;


	/*
	// if there is no call back routine, do nothing!
	*/
	if(TextureCallBackFunction == NULL)
	{
		return;
	}

	/*
	// Determine how many textures to report
	*/
	NumToReport = MIN(nCachedTextures, UserTexDataSize);

	ASSERT((NumToReport>0) && (NumToReport<=MAX_CACHED_TEXTURES));

	/*
	//Step through them
	*/
	pCachedTex	= CachedTextures;
	NumToDo		= NumToReport;
	pUserTexData= UserTexDataArray;

	while(NumToDo)
	{
		/*
		// Ok if this entry is unused, find the next one
		*/
		while(pCachedTex->sglName == NM_INVALID_NAME)
		{
			pCachedTex++;
			ASSERT(pCachedTex < (CachedTextures + MAX_CACHED_TEXTURES));
		}

		/*
		// check that it's worth reporting
		*/
		if ((pCachedTex->loadedName==NM_INVALID_NAME) && 
			!(pCachedTex->bTextureNeeded || pCachedTex->bUsedLastRender))
		{
			--NumToReport;
		}
		else
		{
			/*
			// Ok, copy over the details
			*/
			pUserTexData->texture_name = pCachedTex->sglName;
			pUserTexData->user_id	   = pCachedTex->UserId;

			pUserTexData->loaded	   = (pCachedTex->loadedName!=NM_INVALID_NAME);
			 
			pUserTexData->needed	   = pCachedTex->bTextureNeeded;
			pUserTexData->used_previous_render	= pCachedTex->bUsedLastRender;
			pUserTexData->approx_object_size	= pCachedTex->size;

			pUserTexData++;
		}

		/*
		// Move along to the next one
		*/
		NumToDo--;
		pCachedTex++;

	}/*end while*/


	/*
	// get the amount of free texture space
	*/
	free_space =sgl_get_free_texture_mem();
	

	/*
	// Call the user with this information
	*/
	(*TextureCallBackFunction)(UserTexDataArray, NumToReport, free_space);

	/*
	// That's all folks
	*/
}

/******************************************************************************
 * Function Name: sgl_get_free_texture_mem_info
 *
 * Inputs       : pInfo: pointer to an sgl_texture_mem_info structure.
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Fills in the structure with the amount of the different types
 *				  of free texture memory.
 *
 *				  sgl_err_bad_parameter is generated if the pointer is NULL.
 *
 * 				  Assumes that InitTextureMemory has been called.
 *****************************************************************************/

extern void CALL_CONV sgl_get_free_texture_mem_info (
								sgl_texture_mem_info *pInfo )
{
	SglError(sgl_no_err);
	gpTextureIF->pfnTextureGetFreeMemoryInfo(gHLogicalDev->TexHeap, pInfo);

} /* sgl_get_free_texture_mem_info */


/******************************************************************************
 * Function Name: sgl_get_free_texture_mem
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : MemoryLeft,the number of free bytes in the texture memory. 
 * Globals Used : MemoryLeft
 * Description  : Fragmentation may limit the amount of texture memory that is
 *				  usable.
 * Pre-condition: InitTextureMemory has been called.
 *****************************************************************************/

extern unsigned long CALL_CONV sgl_get_free_texture_mem()
{
	SglError(sgl_no_err);
	return(gpTextureIF->pfnTextureGetFreeMemory(gHLogicalDev->TexHeap));
}

/*
// End of file
*/
