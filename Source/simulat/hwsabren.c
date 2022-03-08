/******************************************************************************
 * Name : hwsabrend.c
 * Title : 
 * Author : Stel Michael
 * Created : 24/06/1995 ported from version started OCT 94
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : an implementation in software of the ISP
				 ASIC (sabre) : Top level Interface
				 
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwsabren.c,v $
 * Revision 1.17  1996/12/11  01:24:34  jop
 * Removed ../'s from includes
 *
 * Revision 1.16  1996/02/19  14:26:35  jrc
 * Added #include "smtexas.h" for DUMP_PARAM_FILES value
 *
 * Revision 1.15  1995/11/28  11:58:15  pdl
 * enabled more parameter file writing.
 *
 * Revision 1.14  1995/10/20  10:48:12  sjf
 * changed param buffer variable.
 *
 * Revision 1.13  1995/10/03  18:54:40  jop
 * Fixed detection of first and second object per region
 *
 * Revision 1.12  1995/08/17  17:09:11  sm
 * made it init fog table
 *
 * Revision 1.11  1995/08/02  09:50:09  sm
 * changed double divde to a mult
 *
 * Revision 1.10  1995/07/28  10:20:18  sm
 * made output region num
 *
 * Revision 1.9  1995/07/27  10:18:52  sm
 * reveome some printing
 *
 * Revision 1.8  1995/07/27  10:16:07  sm
 * added second texas call
 *
 * Revision 1.7  1995/07/27  09:47:21  sm
 * added call to Texas and changed mem address name
 *
 * Revision 1.6  1995/07/26  16:13:44  sm
 * added dvregion.h
 *
 * Revision 1.5  1995/07/26  12:08:18  sm
 * moved syscon
 *
 * Revision 1.4  1995/07/19  12:26:46  sm
 * fixed several minor but critical bugs
 * it should now work!
 *
 * Revision 1.3  1995/06/29  15:56:14  sm
 * no much
 *
 * Revision 1.2  1995/06/29  12:25:51  sm
 * added some comments
 *
 * Revision 1.1  1995/06/29  12:16:32  sm
 * Initial revision
 *
 *
 *****************************************************************************/


#define MODULE_ID MODID_HWSABREN

#include <math.h>

#include "sgl_defs.h"
#include "dlnodes.h"
#include "dvregion.h"
#include "hwinterf.h"
#include "syscon.h"
#include "sabre.h"
#include "rnstate.h"
#include "rnglobal.h"
#include "hwsabsim.h"
#include "smtexas.h" /* for DUMP_PARAM_FILES */


extern PARAM_BUF_MANAGEMENT_STRUCT ParamBufferInfo;


 /********************************************** 
 *
 *    fogging power table and other variables
 *
 ***********************************************/

int	PowerTable[128]; /*power table*/


/**************************************************************************
 * Function Name  : InitPowerTable
 * Inputs         : 

 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : 
 * Global Used    : PowerTable

 * Description    : calculated all the entries for the power table
 *					
 *				   
 **************************************************************************/

void	InitPowerTable()

{
	int i=0;
	double x;

	for(x=0.0;x>-1.0;x-=1/128.0)
	{
		PowerTable[i]=pow(2.0,x)*256;
		i++;
	}

	PowerTable[0]=255;
}




/**************************************************************************
 * Function Name  : Fog
 * Inputs         : Depth

 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : 
 * Global Used    : PowerTable

 * Description    : return for factor for a given depth using the power table
 *					
 *				   
 **************************************************************************/

INT32		Fog(INT32	Depth)
{

	INT32	fogIndex,fogFac;


   	fogIndex = Depth>>FogRegister; 
    
   	fogFac = fogIndex>>7;

  	if (fogFac>8)
		fogFac=9;

    fogFac = PowerTable[fogIndex & 0x7f]>>fogFac;

	return fogFac;

}









/**************************************************************************
 * Function Name  : HWISPRenderer
 * Inputs         : pSabreParamMem

 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : 
 * Global Used    : NumRegionsReg
					FogRegister

 * Description    : Simulated top level of ISP chip. Takes in parameter
					memory and steps though all regions rendering each
					in turn.
 *					
 *				   
 **************************************************************************/

void	HWISPRenderer()	
{
	UINT32	XYData;
	UINT32	ObjectData;
	INT32	Aparam,Bparam,Cparam;
	UINT32	PlaneWord;

	double	Creg40;
	INT32	Ctop32;

	INT32         U_plane_depth[NUM_SABRE_CELLS];  
	UINT32		  U_plane_id[NUM_SABRE_CELLS];
	BOOL	      shadow[NUM_SABRE_CELLS];

	cell_control	WideInstr;

	INT32  	curWord=0;
	INT32	ObjectOff;

	static BOOL	secObj;
	static  BOOL	firstObj;

	static	cell_state Cell[NUM_SABRE_CELLS];

	INT32	Region;
	INT32 	XRegionSize,YRegionSize;
	INT32 	XRegionPos,YRegionPos;

	INT32	XSpan,YLine;
	INT32	Plane;

	BOOL	LastInRegion;
		
	INT32	RegionCount;
	INT32	fogFactor;
	UINT32	cl;	
	UINT32	Index;
	UINT32	NumPlanes;
	UINT32	*PlaneAddr;
	UINT32	*pStartObjectData;
	UINT32	PlaneOffAddr;
	UINT32	Instr;

	double  Avalue;

	secObj = FALSE;
	firstObj = FALSE;


	InitPowerTable();

	pStartObjectData = ParamBufferInfo.isp.pParamStore + ParamStartAddrReg;


	/* Step through all the regions */ 

	RegionCount = 0;

	for (Region=0;Region<=NumRegionsReg;Region++)
	{

		fprintf(stderr,"Region: %ld\n",(long)RegionCount);	

		RegionCount++;

		/* Extract data from first word in region data */

		XYData = *(pStartObjectData+curWord);


		/* Make sure first word is xy data */

		ASSERT( (XYData & 0x40000000L)!=0);

		XRegionSize = ( (XYData & 0x0000001FL) + 1 ) * NUM_SABRE_CELLS; /* x region data is in multiples of number of cells */ 
		YRegionSize = ( ((XYData & 0x00007FE0L)>>5) + 1) * NUM_SABRES;
		XRegionPos = ( (XYData & 0x000F8000L)>>15)*NUM_SABRE_CELLS; /* x pixel position of region start */
		YRegionPos = ( (XYData & 0x3FF00000L)>>20)*NUM_SABRES; /* y pixel position of region start */


		/* 
		   render the current region, stepping across in steps of 32 pixels in x and single lines in y  
		   for each 32 (NUM_SABRE_CELLS) span, all the planes for all the objects are passed through every cell 
		*/

   		for (YLine = YRegionPos; YLine<(YRegionPos+YRegionSize); YLine++) 
		{
			for (XSpan = XRegionPos; XSpan<(XRegionPos+XRegionSize); XSpan+=NUM_SABRE_CELLS)
			{
				sgl_bool	NoObjectYet;
				
				/********************************************************
				 Run through all the planes for all the objects 
				 for a single 32 pixel (NUM_SABRE_CELLS) span  	
				*********************************************************/

				LastInRegion = FALSE;


			   	/* reset the state data for all the sabre cells */

			   	for(cl=0;cl<NUM_SABRE_CELLS;cl++)
			   	{
					Cell[cl].I_depth = 0;
					Cell[cl].U_depth = 0;
					Cell[cl].I_id = 0;
					Cell[cl].U_id = 0;
					Cell[cl].I_visib = FALSE;
					Cell[cl].I_forward = FALSE;
					Cell[cl].shad_temp = FALSE;
					Cell[cl].U_visib = FALSE;
					Cell[cl].U_shadow = FALSE;
				}

				ObjectOff=1;
				NoObjectYet = TRUE;

				while (!LastInRegion) /* loop through all the objects for the region */
				{
					/* Extract data for object */

					ObjectData = *(pStartObjectData+curWord+ObjectOff);
		
					NumPlanes = (ObjectData & 0x1FF80000L)>>19;

					PlaneOffAddr = (ObjectData & 0x0007FFFFL);

					PlaneAddr = ParamBufferInfo.isp.pParamStore + PlaneOffAddr;

					for (Plane=0;Plane<NumPlanes;Plane++)
					{

					    Aparam = *(PlaneAddr++);			
					    Bparam = *(PlaneAddr++);			
					    Cparam = *(PlaneAddr++);			
			
						PlaneWord =  *(PlaneAddr++);			
  
						Instr = (PlaneWord & 0x0000000FL);
						Index = (PlaneWord>>4);
						
/*
						printf("%05x%01x%08lx%05lx%05lx\n",Index,Instr,Cparam,Bparam,Aparam);
*/
						/*
							If this plane indicates the beginning of a new translucent pass, texas must 
							be called so it can store the current span 
						*/

	   					if (Instr==begin_trans)
						{
		   					for(cl=0;cl<NUM_SABRE_CELLS;cl++)
		   					{
								fogFactor = Fog(U_plane_depth[cl]);  /*  return the fogging factor for visible depth at pixel */
/*
		   						if((XSpan+cl)%32==0)
		   							printf("01%08lX\n",(YLine<<8)+((XSpan+cl)>>5));

		  						printf("00%08lX\n",(shadow[cl]<<28)+(fogFactor<<20)+U_plane_id[cl]);  
*/
if(DUMP_PARAM_FILES==1)
{
	fprintf(FtexasInput,"%d %d %d %d %d\n",XSpan+cl,YLine,U_plane_id[cl],shadow[cl],fogFactor);

}

		   		   				if(U_plane_id[cl])
		   		   					Texas(XSpan+cl,YLine,U_plane_id[cl]<<1,(unsigned char)shadow[cl],(unsigned char) fogFactor);
							}
						}


						/* must check to see if this is second object given to sabre for this span */

						if ( (Instr == forw_visib_fp) || (Instr == forw_invis_fp) || (Instr == test_shad_forw_fp) )
												/*		|| (Instr == test_shad_perp_fp)	|| (Instr == forw_perp_fp) )  */   	
						{
							if (NoObjectYet)
		   	   				{
	   							secObj=FALSE;
	   							firstObj=TRUE;
								NoObjectYet = FALSE;
		   	   				}
		   	   				else if ( (firstObj == TRUE) && (secObj == FALSE) )  	
		   	   				{
	   							firstObj=FALSE;
	   							secObj=TRUE;
		   	   				}
	   		   				else
							{
	   	   						secObj=FALSE;
							}
 	   	   				}

      					/* decode compact instruction onto wide format */

   		  				WideInstr=ExpandInstruction(Instr, FALSE, secObj);


	   					/* calculate depth of current plane at x,y pixel position */

		   	  			Creg40 = InitialCalc(XSpan, YLine, Aparam, Bparam, Cparam );

						/* Creg40 is divided by 256 */

		   	  			Ctop32 = (long) (Creg40*(3.90625E-3)); /* Adder is 40 bits wide therefore C has to be kept as a 
   											  40 bit number */

   						Avalue = (double) conv20_to_30(Aparam); 
	
	   		   			/* process the 32 'NUM_SABRE_CELLS' cells */

		   	   			for(cl=0; cl<NUM_SABRE_CELLS; cl++)
		   				{
		   					U_plane_id[cl] = SurfProcess(&Cell[cl], WideInstr, Ctop32, Index, &shadow[cl], &U_plane_depth[cl]);

 							Creg40 += Avalue; 	

		   	        		Ctop32 = (long) (Creg40*(3.90625E-3));/* Ctop32 which is passed to comparator in surf_process */
		  				}	

					} /* end of plane loop */
			    
			
					LastInRegion = (ObjectData	& 0x80000000L);

	   	   			if (LastInRegion)
		   			{
		   	    		for(cl=0; cl<NUM_SABRE_CELLS; cl++)
		   	    		{
   							fogFactor = Fog(U_plane_depth[cl]); /*  return the fogging factor for visible depth at pixel */

				/*
		   					if((XSpan+cl)%32==0)
		   			   			printf("01%08lX\n",(YLine<<8)+((XSpan+cl)>>5));

		  			  		printf("00%08lX\n",(shadow[cl]<<28)+(fogFactor<<20)+U_plane_id[cl]);  
				*/
if(DUMP_PARAM_FILES==1)
{
	fprintf(FtexasInput,"%d %d %d %d %d\n",XSpan+cl,YLine,U_plane_id[cl],shadow[cl],fogFactor);

}
	   						if(U_plane_id[cl])
   								Texas(XSpan+cl,YLine,U_plane_id[cl]<<1,(unsigned char)shadow[cl],(unsigned char) fogFactor);

			  	 		}
					}

	   			ObjectOff++;

			  	} /* while for all objects in region */

			} /* x spans */

		} /*y line */

		/* Move to data for next region */

		curWord+=ObjectOff;	


	} /*  regions loop */

}




