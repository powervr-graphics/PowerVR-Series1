/******************************************************************************
 * Name : hwsabsim.c
 * Title : 
 * Author : Stel Michael
 * Created : 23/06/1995 ported from version started OCT 94
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : an implementation in software of the ISP
				 ASIC (sabre)
				 
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwsabsim.c,v $
 * Revision 1.3  1996/01/23  16:25:49  sm
 * added fixes to pcx bugs
 *
 * Revision 1.1  1995/12/18  14:22:20  sjf
 * Initial revision
 *
 * Revision 1.4  1995/07/26  16:12:47  sm
 * *** empty log message ***
 *
 * Revision 1.3  1995/07/19  12:25:34  sm
 * minor changes.
 *
 * Revision 1.2  1995/06/29  15:58:47  sm
 * removed adder function
 *
 * Revision 1.1  1995/06/27  17:17:54  sm
 * Initial revision
 *
 *
 *****************************************************************************/


#define MODULE_ID MODID_HWSABSIM

#include <math.h>

#include "../sgl.h"
#include "../sgl_defs.h"
#include "../dvregion.h"
#include "../hwinterf.h"
#include "sabre.h"
#include "syscon.h"


#ifndef BOOL
#define BOOL long
#endif



/* size of mantissa for A and B values */

#define MANT_BITS  16


/**************************************************************************
 * Function Name  : conv20_to_30
 * Inputs         : number - in special 20 bit floating format 
 *
 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : num_30 - in 30 bit integer format
 * Global Used    : 
 * Description    : convert a value in floating 20 bit format to
					30 bit integer.
 *
 **************************************************************************/

long conv20_to_30(long	number)
{                           
	long	expo;
    long num_30;


	/*
	// Extract the mantissa (which is the "absolute" value)
	*/
	num_30=(number & 0x00007FFFL);

	/*
	// If the sign bit is set, then negate it
	*/
	if(( 1L<<(MANT_BITS-1)) & number)
	{
		num_30 = -num_30;
	}

	/*
	// get the exponent
	*/
	expo=((number>>MANT_BITS) & 0x0000000FL); 

	/*
	// Shift by the required amount
	*/
	num_30=(num_30 << (expo));

	return num_30;
}



/**************************************************************************
 * Function Name  : InitialCalc
 * Inputs         : x - x screen coord
					y - y screen coord
					a - 'a' parameter : NOTE this has already been conerved to a double
					b - 'b' parameter
					c - 'c' parameter
 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : C depth a x,y pixel
 * Global Used    : 
 * Description    : returns the depth value which is stored in the C register, using
					a,b and c from the parameter store, for a given x,y pixel position
 *
 **************************************************************************/

double	InitialCalc(long x, long y, long a, long b, long c)
{
	double	depth;
	long a_30,b_30;
	double a_40,b_40;

	a_30=conv20_to_30(a);
	b_30=conv20_to_30(b);

	b_40=((double) (b_30));  
	a_40=((double) (a_30));  

	/* C value holds top 32 of 40 bit so mult by 256.   */

 	depth= a_40*((double) x) + b_40*((double) y) + (((double) c)*256.0f);   

	return (depth);
}


/**************************************************************************
 * Function Name  : wideCom - INTERNAL ONLY
 * Inputs         : i,u,shad,pvis,cluid,pper,muxsel - setting for the commands data
 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : command
 * Global Used    : 
 * Description    : sets cell_control data
 *					
 *				   
 **************************************************************************/

cell_control	wideCom(int i,int u,int shad,BOOL pvis,BOOL clearUid,
												  BOOL pper,BOOL muxsel)
{
	cell_control	command;

	command.i_load=i;
	command.u_load=u;
	command.test_shad=shad;
	command.plane_visib=pvis;
	command.delayed_clear_u_id=clearUid;
	command.plane_perp=pper;
	command.mux_sel=muxsel;
	
	return	command;
}



/**************************************************************************
 * Function Name  : ExpandInstruction
 * Inputs         : instr
					do_nop
					sec_object
 * Outputs        :  
 * Input/Output	  : 
						  
 * Returns        : cell_control
 * Global Used    : 
 * Description    : Expands plane instruction from compact form to wide format
 * 					used internally  
 *					
 *				   
 **************************************************************************/

cell_control ExpandInstruction(int instr,BOOL do_nop,BOOL sec_object, 
								   BOOL do_flush)
{
	static int prev_intern;
	static int prev_intern_out;
	cell_control wide;

	if (do_nop)
		wide=wideCom(load_i_nop,load_u_nop,test_shad_nop,FALSE,FALSE,FALSE,FALSE);
	else if(do_flush)
		wide=wideCom(load_i_nop,load_u_closer,test_shad_nop,TRUE,FALSE,FALSE,TRUE);	
	else
	{
		prev_intern=prev_none;		

		switch(instr)	
		{
			case forw_visib:
				wide=wideCom(load_i_further,load_u_nop,test_shad_nop,TRUE,FALSE,FALSE,FALSE);	
   			break;

			case forw_visib_fp:    
				if (sec_object==TRUE)
					wide=wideCom(load_i,load_u,test_shad_nop,TRUE,FALSE,FALSE,TRUE);	
				else
					wide=wideCom(load_i,load_u_closer,test_shad_nop,TRUE,FALSE,FALSE,TRUE);	
   			break;
 
			case forw_invis:
				wide=wideCom(load_i_further,load_u_nop,test_shad_nop,FALSE,FALSE,FALSE,FALSE);	
			break;
 
			case forw_invis_fp:    
				if (sec_object==TRUE)
					wide=wideCom(load_i,load_u,test_shad_nop,FALSE,FALSE,FALSE,TRUE);	
				else
					wide=wideCom(load_i,load_u_closer,test_shad_nop,FALSE,FALSE,FALSE,TRUE);	
			break;
                                                                                            
			case forw_perp:
				wide=wideCom(load_i_further,load_u_nop,test_shad_nop,FALSE,FALSE,TRUE,FALSE);	
			break;
 
			case forw_perp_fp:    
				if (sec_object)
					wide=wideCom(load_i,load_u,test_shad_nop,TRUE,FALSE,TRUE,TRUE);  /* pvis was FALSE */	
				else
					wide=wideCom(load_i,load_u_closer,test_shad_nop,TRUE,FALSE,TRUE,TRUE);	/* and here */
			break;
                                                                                            
			case rev_visib:
					wide=wideCom(load_i_closer,load_u_nop,test_shad_nop,TRUE,FALSE,FALSE,FALSE);
			break;

			case rev_invis:
					wide=wideCom(load_i_closer,load_u_nop,test_shad_nop,FALSE,FALSE,FALSE,FALSE);
			break;

			case rev_replace_if:
				    /* this is a reverse invisible instruction for this cycle. 
					   It is given at the last reverse instruction before 
					   the first reverse planes are passed for the second time */

					wide=wideCom(load_i_closer,load_u_nop,test_shad_nop,FALSE,FALSE,FALSE,FALSE);
					prev_intern=prev_def_rep;
			break;

			case test_shad_forw:
					wide=wideCom(load_i_further,load_u_nop,test_shad_nop,TRUE,FALSE,FALSE,FALSE);
					prev_intern=prev_shad_forw;
			break;

			case test_shad_forw_fp:
				if (sec_object==TRUE)
					wide=wideCom(load_i,load_u,test_shad_nop,TRUE,FALSE,FALSE,TRUE);
				else
					wide=wideCom(load_i,load_u_closer,test_shad_nop,TRUE,FALSE,FALSE,TRUE);
				prev_intern=prev_shad_forw;
 
			break; 

			case test_shad_perp:
				wide=wideCom(load_i_further,load_u_nop,test_shad_nop,FALSE,FALSE,TRUE,FALSE);
				prev_intern=prev_shad_forw;
			break;



			case test_shad_perp_fp:
				if (sec_object==TRUE)
					wide=wideCom(load_i,load_u,test_shad_nop,FALSE,FALSE,TRUE,TRUE);
				else
					wide=wideCom(load_i,load_u_closer,test_shad_nop,FALSE,FALSE,TRUE,TRUE);
				prev_intern=prev_shad_forw;
			break;


			case test_shadow_rev:
				wide=wideCom(load_i_closer,load_u_nop,test_shad_nop,FALSE,FALSE,FALSE,FALSE);
				prev_intern=prev_shadow;
			break;


			case test_light_rev:
				wide=wideCom(load_i_closer,load_u_nop,test_shad_nop,FALSE,FALSE,FALSE,FALSE);
				prev_intern=prev_light;				
			break;


			/*
			// Begin translucent pass
			*/			
			case begin_trans:
				wide=wideCom( load_i_nop, load_u_closer, test_shad_nop, 
							  TRUE,FALSE,FALSE,TRUE);

				prev_intern=prev_trans; 
			break;
		}




		switch(prev_intern_out)
		{
			case prev_shad_forw:
				wide.test_shad=test_shad_closer;
				wide.i_load=load_i;
				wide.mux_sel=TRUE;			
			break;

			case prev_shadow:
				wide.test_shad=test_shadow_further;
				wide.u_load=load_u_nop;
			break;

			case prev_light:
				wide.test_shad=test_light_further;
				wide.u_load=load_u_nop;
			break;

			case prev_trans:
                wide.delayed_clear_u_id=TRUE;		 
				wide.u_load=load_u_nop;
			break;		

			case prev_def_rep:
				wide.i_load=load_i_invis_forw;
			break;


		}

	prev_intern_out=prev_intern;

	}

	return wide;
}





/**************************************************************************
 * Function Name  : SurfProcess
 * Inputs         : instruction - this is in the internal wide format
					C - a 32 bit value since comparator is 32 bit
					index - planes id
					

 * Outputs        : shadow - shadow flag
					U_plane_depth - depth stored in U depth register
 
 * Input/Output	  : cell - holds the cells state information
						  
 * Returns        : U_id - id of plane currently held in U register
 * Global Used    : 
 * Description    : Does the processing for a single sabre cell that has its state 
					stored in 'cell', and is passed a single plane.
*   				Plane has an instruction, 'C' depth and index identifier.
*
*
**************************************************************************/

int	SurfProcess(cell_state *cell,cell_control	instruction,long	C,unsigned long index,BOOL *shadow,long *U_plane_depth)
{
	BOOL	sign_bit;

	/*
		value of flag register at next clock
	*/

	BOOL	I_visib_intern=FALSE;
	BOOL	I_forward_intern=FALSE;
	BOOL	shad_temp_intern=FALSE; 
	BOOL	U_visib_intern=FALSE;
	BOOL	U_shadow_intern=FALSE;
	BOOL	U_ID_RE_intern=FALSE;
	BOOL	U_RE_intern=FALSE;
	BOOL	I_RE_intern=FALSE;

	/*	control lines  */

	BOOL	I_RE;
	BOOL	U_RE;
	BOOL	U_ID_RE;
	
	/* mux variables */

	BOOL	Sel_U;	 /* FALSE selects C, TRUE selects U */
	long	mux_depth;

	BOOL	I_lte_mux;
	BOOL	I_gte_mux;


	/* 
	// If the previous instruction requested that the UID be cleared
	// do so.
	*/ 
	
	if (instruction.delayed_clear_u_id)
	{
		cell->U_id=0; 
	}


	/*
		sign_bit of C depth
	*/

	if ((C & 0x80000000L)==0)
		sign_bit=FALSE;
	else
		sign_bit=TRUE;


	/* modify C if a perpendicular plane */

	Sel_U=instruction.mux_sel;  /* mux selection line */


	/* the multiplexor */

	if (Sel_U==FALSE)
		mux_depth=C;
	else
		mux_depth=cell->U_depth;


	/* comparator ,ALU compares I_depth with mux output */

	if (cell->I_depth>=mux_depth)
		I_gte_mux=TRUE;
	else
		I_gte_mux=FALSE;

	if (cell->I_depth<=mux_depth)
		I_lte_mux=TRUE;
	else
		I_lte_mux=FALSE;
		
	switch(instruction.i_load)
	{
		case load_i_nop:
			I_RE_intern=FALSE;		/* disable read for the I register */
			I_visib_intern=cell->I_visib;
			I_forward_intern=cell->I_forward;
		break;

		case load_i:
			I_RE_intern=TRUE;
			I_forward_intern=TRUE;
			I_visib_intern=instruction.plane_visib;
		break;

		case load_i_further:
			if ( ((instruction.plane_perp!=TRUE) && (I_gte_mux)) || ((instruction.plane_perp) && (sign_bit)) )     
				/* was I_gte_mux  thurs 12th 95 jan */
			{
				I_RE_intern=TRUE;	
				I_forward_intern=TRUE;
				I_visib_intern=instruction.plane_visib;           
			}
			else
			{
				I_RE_intern=FALSE;
				I_visib_intern=cell->I_visib;
				I_forward_intern=cell->I_forward;
			}
		break;

		case load_i_closer :				   
			if (I_gte_mux==0)
			{
				I_RE_intern=TRUE;	
				I_forward_intern=FALSE;
				I_visib_intern=instruction.plane_visib;
			}
			else
			{
				I_RE_intern=FALSE;
				I_visib_intern=cell->I_visib;
				I_forward_intern=cell->I_forward;
			}
			
		break;

		case load_i_invis_forw:

			if (((cell->I_visib)!=TRUE) && (cell->I_forward))			  
			{
				I_RE_intern=TRUE;
				I_forward_intern=FALSE;
				I_visib_intern=instruction.plane_visib;

			}			
 			else
			{
				I_RE_intern=FALSE;
				I_visib_intern=cell->I_visib;
				I_forward_intern=cell->I_forward;
			}

		break;

	}

	switch(instruction.u_load)
	{
		case load_u_nop:
			U_RE_intern=FALSE;
			U_visib_intern=cell->U_visib;
			U_ID_RE_intern=FALSE;
			break;

		case load_u:
			U_RE_intern=TRUE;
			U_ID_RE_intern=TRUE;
			U_visib_intern=cell->I_visib;
			break;
			
		case load_u_closer:
			if (((I_gte_mux) && (cell->I_visib))    || (!(cell->U_visib))) 
			{
				U_RE_intern=TRUE;
				U_ID_RE_intern=TRUE; 
				U_visib_intern=cell->I_visib;
			}
			else
			{
				U_RE_intern=FALSE;
				U_visib_intern=cell->U_visib;
				U_ID_RE_intern=FALSE;  
			}
			break;
	}

	if (U_RE_intern)
		U_shadow_intern=0; 
	else
	{
		switch(instruction.test_shad)
		{
			case test_shad_nop:
				U_shadow_intern=cell->U_shadow;
				shad_temp_intern=cell->shad_temp;
				break;

			case test_shad_closer:
				U_shadow_intern=cell->U_shadow;
				shad_temp_intern=(!I_lte_mux);
				break;

			case test_shadow_further:
				U_shadow_intern=((cell->U_shadow) || ((I_lte_mux)&& (cell->shad_temp)));
				shad_temp_intern=cell->shad_temp;			
				break;

			case test_light_further:
				U_shadow_intern=!(((I_lte_mux) && (cell->shad_temp)) ||(!(cell->U_shadow)));
				shad_temp_intern=cell->shad_temp;			
				break;

		}		

	}

	I_RE=I_RE_intern;

	cell->U_visib=U_visib_intern;
	cell->U_shadow=U_shadow_intern;
    U_RE=U_RE_intern;
	U_ID_RE=U_ID_RE_intern;


	/* comparator ,ALU compares I_depth with mux output */

	/* process the registers */
 
	if (U_RE)
		cell->U_depth=cell->I_depth;

	if (U_ID_RE)
			cell->U_id=cell->I_id;

	/* process the registers */

	cell->I_visib=I_visib_intern;
	cell->I_forward=I_forward_intern;
	cell->shad_temp=shad_temp_intern;

	if (I_RE)
	{
		cell->I_depth=C;
		cell->I_id=index;
	}


	*shadow=cell->U_shadow;

	*U_plane_depth=cell->U_depth;

	return	(cell->U_id);
}



/*---------------------------- End of File -------------------------------*/
