/******************************************************************************
 * Name : hwsabren.c
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
 * Revision 1.5  1996/12/20  16:50:51  erf
 * Fixed tranlucent pass bug.
 * Fixed link list support.
 *
 * Revision 1.4  1996/12/11  11:55:39  erf
 * Changed PVR3 to PCX2. Initialised texas simulator with direct call.
 *
 * Revision 1.3  1996/12/09  12:26:10  erf
 * Need to initialise texas simulator to open files etc.
 *
 * Revision 1.2  1996/11/21  15:17:23  erf
 * Implement object pointer linked list support. Code handles both object
 * pointer formats. ie contiguous object pointers and linked object pointers.
 *
 * Revision 1.1  1996/11/14  14:13:26  erf
 * Initial revision
 *
 * Revision 1.1  1996/11/14  12:33:14  erf
 * Initial revision
 *
 * Revision 1.4  1996/03/05  12:47:59  pdl
 * wrote code that allows reading back in sabreout.txt files.
 *
 * Revision 1.3  1996/01/23  15:07:14  sm
 * added fixes to pcx bugs
 *
 * Revision 1.1  1995/12/18  14:22:18  sjf
 * Initial revision
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
#include <stdio.h>

#include "../sgl_defs.h"
#include "../dlnodes.h"
#include "../dvregion.h"
#include "../hwinterf.h"
#include "syscon.h"
#include "sabre.h"
#include "../rnstate.h"
#include "../rnglobal.h"
#include "hwsabsim.h"
#include "../pvrlims.h"
#include "../pkisp.h"
#include "../sgl_math.h"

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


#ifdef READ_BACK

/*
** this version of HWISPRenderer reads back in the sabreout.txt file, and calls texas
*/

void	HWISPRenderer()	
{
	FILE *ReadSabreOut;
	unsigned long Position,Data;
	int i,XPos,YPos,PlaneID,Shadow,Fog;

	ReadSabreOut=fopen("sabreout.txt","r");

	if(ReadSabreOut)
	{
		while(fscanf(ReadSabreOut,"01%08lX\n",&Position)!=EOF)
   		{

			/* (YLine<<8)+((XSpan+cl)>>5) */
	
			YPos=(Position>>8) & 0x1ff;
			XPos=(Position & 0xff) <<5;

			for(i=0;i<32;i++)
			{
				fscanf(ReadSabreOut,"00%08lX\n",&Data);

				/*  (shadow[cl]<<28)+(fogFactor<<20)+U_plane_id[cl]);  */

				Shadow =(Data>>28) & 1;
				Fog    =(Data>>20) & 0xff;
				PlaneID= Data & 0xfffff;

				if(PlaneID)
			   		Texas(XPos+i,YPos,PlaneID<<1,Shadow,Fog);

			}



		}


		fclose(ReadSabreOut);
	}
}


#else

void	HWISPRenderer()	
{
	int 	FirstPete;
	float   fMaxVal,fA,fB,fC;
	UINT32	XYData;
	UINT32	ObjectData;
	INT32	Aparam,Bparam,Cparam;
	UINT32	PlaneWord;

	double	Creg40;
	INT32	Ctop32;

	INT32	U_plane_depth[NUM_SABRE_CELLS];  
	UINT32	U_plane_id[NUM_SABRE_CELLS];
	BOOL	shadow[NUM_SABRE_CELLS];

	cell_control	WideInstr;

	INT32  	curWord = 0, safetyCnt = 0, curLocalWord = 0;
	INT32	ObjectOff;

	static	BOOL	secObj;
	static  BOOL	firstObj;

	static	cell_state Cell[NUM_SABRE_CELLS];

	INT32	Region;
	INT32 	XRegionSize,YRegionSize;
	INT32 	XRegionPos,YRegionPos;

	INT32	XSpan,YLine;
	INT32	Plane;

	BOOL	LastInRegion;
	BOOL	DoneAllRegions;
	BOOL	Translucent;
	INT32	RegionCount;
	INT32	fogFactor;
	UINT32	cl;	
	UINT32	Index;
	UINT32	NumPlanes;
	UINT32	*PlaneAddr;
	UINT32	*pStartObjectData, *pLocalStartObjectData;
	UINT32	PlaneOffAddr;
	INSTR_CODE_ENUMS	Instr;

	double	Avalue;
	float	Fplane;


	secObj = FALSE;
	firstObj = FALSE;

	InitPowerTable();

	/* Need to be called to open output files.
	 */
	InitTexasSimulator();

	pStartObjectData = ParamBufferInfo.isp.pParamStore + ParamStartAddrReg;

	/* Step through all the regions
	 */ 
	DoneAllRegions = FALSE;

	RegionCount = 0;
	pLocalStartObjectData = pStartObjectData;
	curLocalWord =  curWord;
	
	while(!DoneAllRegions)
	{
		fprintf(stderr,"Region: %ld\n",(long)RegionCount);	

		RegionCount++;

		/* NEED TO CHECK IF ObjectData IS POINTER TO PARAMETER INFO OR
		 * A LINK LIST POINTER.
		 * Only do after tile ID.
		 *
		 */
		ObjectData = *(pStartObjectData + curWord);

		/* Strip bit 31. PCX2 start of translucent pass bit.
		 */
		if ((ObjectData & 0x80000000UL) &&
			(ObjectData & 0x20000000UL))
		   ObjectData &= 0x5FFFFFFFUL;
				
		safetyCnt = 0;
		
		/* Used to handle linking of regions together.
		 */
		while (	((ObjectData & LINK_LIST_BIT) != 0) &&
				(safetyCnt < 10))
		{
			/* Maximum of 10 link lists supported.
			 */				
			/* New start of object data.
			 *
			pStartObjectData = ObjectData & 0x0007FFFFL;
			 */
			pStartObjectData = ObjectData & 0x00FFFFFFL;
					
			curWord = 0;
					
			ObjectData = *pStartObjectData;
			safetyCnt++;
		}
				
		/* Error if more 10 linked lists.
		 */
		if (safetyCnt == 10)
			return;
					
		/* Extract data from first word in region data
		 */
		XYData = *(pStartObjectData + curWord);

		/* Make sure first word is xy data
		 */
		ASSERT( (XYData & 0x40000000L)!=0);

		/* x region data is in multiples of number of cells
		 */ 
		XRegionSize = ( (XYData & 0x0000001FL) + 1 ) * NUM_SABRE_CELLS;
		YRegionSize = ( ((XYData & 0x00007FE0L)>>5) + 1) * NUM_SABRES;

		/* x pixel position of region start
		 */
		XRegionPos = ( (XYData & 0x000F8000L)>>15)*NUM_SABRE_CELLS;

		/* y pixel position of region start
		 */
		YRegionPos = ( (XYData & 0x3FF00000L)>>20)*NUM_SABRES;

		/* 
		// render the current region, stepping across in steps of 32 pixels in x 
		// and single lines in y for each 32 (NUM_SABRE_CELLS) span, all the
		// planes for all the objects are passed through every cell 
		*/
		FirstPete=TRUE;

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

			   	/* reset the state data for all the sabre cells
			   	 */
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
					Cell[cl].Delayed_clear_uid = FALSE;
				}

				/* Use local copies of start address and position index to handle link
				 * lists if present. Allows return to start of region if multiple linked 
				 * objects in a region.
				 */
				pLocalStartObjectData = pStartObjectData;
				curLocalWord = curWord;
				
				ObjectOff = 1;
				NoObjectYet = TRUE;
				
				while (!LastInRegion) /* loop through all the objects for the region */
				{
					/* Extract data for object
					 */
					ObjectData = *(pLocalStartObjectData + curLocalWord + ObjectOff);
					 
					/* Strip bit 31. PCX2 start of translucent pass bit.
					 */
					if ((ObjectData & 0x80000000UL) &&
						(ObjectData & 0x20000000UL))
					   ObjectData &= 0x5FFFFFFFUL;

					/* Used to handle linking of objects together within one region.
					 */
					while (	((ObjectData & LINK_LIST_BIT) != 0) &&
							(safetyCnt < 10))
					{
						/* Maximum of 10 link lists supported.
						 * New start of object data.
						 *
						pStartObjectData = (NextObjectData & 0x0007FFFFL);
						 */
						pLocalStartObjectData = (ObjectData & 0x00FFFFFFL);

						curLocalWord = 0;
						ObjectOff = 0;	/* to prevent skip of object pointer.	*/
						
						ObjectData = *pLocalStartObjectData;

						safetyCnt++;
					}
				
					/* Error if more 10 linked lists.
					 */
					if (safetyCnt == 10)
						return;

					NumPlanes = (ObjectData & 0x1FF80000L)>>19;

					PlaneOffAddr = (ObjectData & 0x0007FFFFL);

					PlaneAddr = ParamBufferInfo.isp.pParamStore + PlaneOffAddr;

					Translucent=FALSE;
					for (Plane=0;Plane<NumPlanes;Plane++)
					{
						/*
						// Unpack the 3 words... into A,B,C, instruction and
						// index
						//
						// Offset 0 contains A param, the instruction,
						// and the upper 6 bits of the 18 bit tag
						*/

#if PCX2
						/* PCX2 version. Floating point, linked lists, 4 word parameter
						 * format and perpendicular plane scaling.
						 */
/*
						if(FirstPete && DUMP_PARAM_FILES == 1)
						{
							fprintf(FsabreFloat,"%08X %08X %08X %08X ",
							PlaneAddr[0],PlaneAddr[1],PlaneAddr[2],PlaneAddr[3]);
						}
*/

						fA=LONG_TO_FLOAT(PlaneAddr[0]);
						fB=LONG_TO_FLOAT(PlaneAddr[1]);
						fC=LONG_TO_FLOAT(PlaneAddr[2]);

   						Instr = PlaneAddr[3] & 0xf;
						Index = PlaneAddr[3] >> 4;
						
						/* If a perpendicular plane then scale.
						 */
						if(Instr==2 || Instr==4 || Instr==0xA || Instr==0xC)
						{
						   	#define MAX_CENTRE  (1024.0f)
							#define HALF_OFFSET (1024.0f)
							fMaxVal = sfabs(fC) + 2.0f*MAX_CENTRE*(sfabs(fA) + sfabs(fB));

							#undef MAX_CENTRE
							#undef HALF_OFFSET

							fMaxVal = ApproxRecip(fMaxVal);
									
							/* If PackTo20Bit() being used then need to correct for
							 * overflow. ie x0.125.
							 *
							fMaxVal *= 0.125f;
							 */
						}
						else
						{
							/* If PackTo20Bit() being used then need to correct for
							 * overflow. ie x0.125.
							 *
							fMaxVal = 0.125f;
							 */
							fMaxVal=1.0f;
						}
						Aparam = PackTo20Bit(fA * fMaxVal);
						Bparam = PackTo20Bit(fB * fMaxVal);
						Cparam = FLOAT_TO_FIXED * fC * fMaxVal;

						if(FirstPete && DUMP_PARAM_FILES == 1)
						{
/*
							fprintf(FsabreFloat,"%08X %08X %08X\n",
									(Instr<<26) |((0x3f000 & Index) << 8) | Aparam,
									Bparam|(Index<<20),
									Cparam);
*/
						}
#elif 0
						/* PCX2 version. Floating point, linked lists, 4 word parameter
						 * format. ie no perpendicular scaling.
						 */
						if(FirstPete && DUMP_PARAM_FILES == 1)
						{
							fprintf(FsabreFloat,"%X %X %X %X ",
							PlaneAddr[0],PlaneAddr[1],PlaneAddr[2],PlaneAddr[3]);
						}
						Aparam = PackTo20Bit(LONG_TO_FLOAT(PlaneAddr[0]));
						Bparam = PackTo20Bit(LONG_TO_FLOAT(PlaneAddr[1]));
						Cparam = FLOAT_TO_FIXED*LONG_TO_FLOAT(PlaneAddr[2]);

   						Instr = PlaneAddr[3] & 0xf;
						Index = PlaneAddr[3] >> 4;
						if(FirstPete && DUMP_PARAM_FILES == 1)
						{
							fprintf(FsabreFloat,"%X %X %X\n",Aparam,Bparam,Cparam);
						}
#else
						/* PCX1version.
						 */
						Aparam = PlaneAddr[0] & 0xFFFFF;
						Instr  = PlaneAddr[0]  & 0x3c000000;
						Index =  (PlaneAddr[0] >> (20 - 12)) & 0x3F000;
							
						/*
						// Offset 1 contains the B param and lower 12 bits
						*/
					    Bparam = PlaneAddr[1] & 0xFFFFF;
						Index |= PlaneAddr[1] >> 20;

						/*
						// C is in offset 2
						*/
					    Cparam = PlaneAddr[2];			
#endif						
						/*
						// Advance the pointer
						*/
						PlaneAddr += WORDS_PER_PLANE;			

						/* 
						// must check to see if this is second object given 
						// to sabre for this span 
						*/
						if ( (Instr == forw_visib_fp) || (Instr == forw_invis_fp)
						    || (Instr == test_shad_forw_fp) )
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

						if(Translucent==FALSE)
						{
							/* decode compact instruction onto wide format
							 */
							WideInstr=ExpandInstruction(Instr, FALSE, secObj, FALSE);


		   					/* calculate depth of current plane at x,y pixel position
	    					 */
		    	  			Creg40 = InitialCalc(XSpan, YLine, Aparam, Bparam, Cparam );

							/* Creg40 is divided by 256
							 */

							 /* 
							// Adder is 40 bits wide therefore C has to be kept as a 
							// 40 bit number 
							*/
							Ctop32 = (long) (Creg40 * (3.90625E-3));

							Avalue = (double) conv20_to_30(Aparam); 
	
	   		   				/* process the 32 'NUM_SABRE_CELLS' cells
							 */
							 for(cl=0; cl<NUM_SABRE_CELLS; cl++)
							 {
							 	U_plane_id[cl] = SurfProcess(&Cell[cl], WideInstr, Ctop32, Index, &shadow[cl], &U_plane_depth[cl]);

								Creg40 += Avalue; 	

								Ctop32 = (long) (Creg40*(3.90625E-3));/* Ctop32 which is passed to comparator in surf_process */
							}	
						}
						if(Instr == begin_trans)
						{
							Translucent=TRUE;
						}
					} /* end of plane loop */

					/*
					// If this plane indicates the beginning of a new
					// translucent pass, texas must 
					//	be called so it can store the current span 
					*/
   					if (Translucent)
					{
	   					for(cl=0;cl<NUM_SABRE_CELLS;cl++)
	   					{
							/*  
							// return the fogging factor for visible depth 
							//at pixel 
							*/
							fogFactor = Fog(U_plane_depth[cl]);

							if(DUMP_PARAM_FILES==1)
							{
						  		if((XSpan+cl)%32==0)
								{
									fprintf(FtexasInputSabOutputFormat,
									  "01%08lX\n",(YLine<<8)+((XSpan+cl)>>5));
								}
								fprintf(FtexasInputSabOutputFormat,
										"00%08lX\n",
								  (shadow[cl]<<28)+(fogFactor<<20)+U_plane_id[cl]);  

								fprintf(FtexasInput,
		"%d %d %d %d %d\n",XSpan+cl,YLine,U_plane_id[cl],shadow[cl],fogFactor); 

					 		}

	   		   				if(U_plane_id[cl])
	   		   					Texas(XSpan+cl,YLine,U_plane_id[cl]<<1,
										(unsigned char)shadow[cl],
										(unsigned char) fogFactor);
						}
					}

					/*
					// There are two things we must do to determine if this is
					// the last object of the region..
					//
					// 1) If it is marked as the LAST Pointer, then there is
					//	  no more data (pointers or regions)
					//
					// 2) If the above is not set, then check if the NEXT word
					//	  is region data
					*/
					if(ObjectData & OBJ_VERY_LAST_PTR)
					{
						DoneAllRegions = TRUE;
						LastInRegion = TRUE;
					}

					/*
					// Else see if the next lot of data is the start of a 
					// new region. ie is it a tile ID.
					*/
					else
					{
						UINT32	NextObjectData;
					
						NextObjectData =
							*(pLocalStartObjectData + curLocalWord + ObjectOff + 1);

						/* Strip bit 31. PCX2 start of translucent pass bit.
						 */
						if ((NextObjectData & 0x80000000UL) &&
							(NextObjectData & 0x20000000UL))
						   NextObjectData &= 0x5FFFFFFFUL;
							
						/* NEED TO CHECK IF NextObjectData IS POINTER TO PARAMETER INFO OR
						 * A LINK LIST POINTER.
						 * Simple check. Linking is performed else where.
						 */
						if ((NextObjectData & LINK_LIST_BIT) != 0)
						{
							UINT32	*pStartObject;
							safetyCnt = 0;
							
							/* Handle multiple linked lists.
							 */
							while (	((NextObjectData & LINK_LIST_BIT) != 0) &&
									(safetyCnt < 10))
							{
								pStartObject = (NextObjectData & 0x00FFFFFFL);
								safetyCnt++;
								
								NextObjectData = *pStartObject;
							}
				
							/* Error if more 10 linked lists.
							 */
							if (safetyCnt >= 10)
								return;
								
							/* Check if last in region.
							 */
							LastInRegion = NextObjectData & REG_COMPULSARY_BITS;
						}
						else
						{
							/* Not a link list pointer. continue as normally.
							 */
							LastInRegion =
								*(pLocalStartObjectData + curLocalWord + ObjectOff + 1) & 
															REG_COMPULSARY_BITS;
						}
					}
					
					/* Is the next word a tile ID or is current object last in last tile.
					 */
	   	   			if (LastInRegion)
		   			{
						/*
						// First issue a flush down the pipeline
						// NOTE only the "do_flush" is imporatant here
						*/
					#if 1
   		  				WideInstr=ExpandInstruction(Instr, FALSE, FALSE, TRUE);
		   	    		for(cl = 0; cl < NUM_SABRE_CELLS; cl++)
						{
		   					U_plane_id[cl] = SurfProcess(&Cell[cl], WideInstr, 
									0, 0,&shadow[cl], &U_plane_depth[cl]);
						}
						
					#endif
		   	    		for(cl = 0; cl < NUM_SABRE_CELLS; cl++)
		   	    		{
							/*  return the fogging factor for visible depth at pixel
							 */
							fogFactor = Fog(U_plane_depth[cl]);

if(DUMP_PARAM_FILES==1)
{
	if((XSpan+cl)%32==0)
	{
		fprintf(FtexasInputSabOutputFormat,"01%08lX\n",(YLine<<8)+((XSpan+cl)>>5));
	}
	fprintf(FtexasInputSabOutputFormat,
		 "00%08lX\n",(shadow[cl]<<28)+(fogFactor<<20)+U_plane_id[cl]);  

	fprintf(FtexasInput,
		"%d %d %d %d %d\n",XSpan+cl,YLine,U_plane_id[cl],shadow[cl],fogFactor);
}

	   						if(U_plane_id[cl])
   								Texas(	XSpan+cl, YLine, U_plane_id[cl] << 1,
   										(unsigned char)shadow[cl], (unsigned char) fogFactor);
			  	 		}
					}

	   				ObjectOff++;

			  	} /* while for all objects in region */

				FirstPete=FALSE;
			} /* x spans */

		} /*y line */

		/* Move to data for next region
		 */
		curWord = curLocalWord + ObjectOff;
		
		/* Used to set start address to one before next region ID. Needed for linked
		 * list support. Doesn't effect ordinary ISP object pointers. ie unlinked case.
		 */
		pStartObjectData = pLocalStartObjectData;
	} /*  regions loop */

	FinishTexasSimulator();
}
#endif

/*
// END OF FILE
*/

