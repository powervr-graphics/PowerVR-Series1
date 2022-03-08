/******************************************************************************
 * Name : sabre.h
 * Title : 
 * Author : Stel Michael
 * Created : OCT 94
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : defines for simuator of ISP chip
				 
				 
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: sabre.h,v $
 * Revision 1.1  1995/12/18  14:22:23  sjf
 * Initial revision
 *
 * Revision 1.2  1995/07/26  16:06:21  sm
 * removed instr codes
 *
 * Revision 1.1  1995/06/29  14:58:37  sm
 * Initial revision
 *
 *
 *****************************************************************************/


/************************
*  intruction codes    *
************************/


#ifndef BOOL
#define BOOL long
#endif




/****************************************
*	 Sabre parameter store structure
****************************************/



typedef struct {

	long a;		/* 20-bit a value; */
	long b;		/* 20-bit b value; */
	long c;		/* 32-bit c value; */

	int instr;	/* 4-bit instruction */

	unsigned long index;	/* 18-bit index; */

} sabre_params;



typedef struct {
	int last_in_tile;
	int num_planes;
	unsigned long plane_addr;
} object_ref;





typedef struct {
	int		i_load;	/* inter load command */
	int		u_load;	/* union load command */
	int 	test_shad;	/* shadow testing commands */
	BOOL	plane_visib;
	BOOL	delayed_clear_u_id;
	BOOL	plane_perp;
	BOOL	mux_sel;
} cell_control;


/*	inter load command constants */

#define	load_i_nop	0
#define load_i		8
#define load_i_further	4
#define load_i_closer	2
#define load_i_invis_forw	1


/*	union load command constants */


#define	load_u_nop	0
#define load_u		2
#define	load_u_closer	3
#define	load_u_invis_forw	1	



/*	shadow testing command constants */



#define	test_shad_nop	0
#define test_shad_closer	2

#define test_shadow_further 	1
#define test_light_further	    3



/* previous instruction constants */

#define prev_none	0
#define prev_shad_forw	1
#define prev_shadow		3
#define prev_light		2
#define	prev_trans		6
#define prev_def_rep	7


typedef struct {
	long	I_depth; 
	long	U_depth; 

    int		I_id; 
	int		U_id; 
	BOOL	Delayed_clear_uid;
	/*
		 control flag registers
	*/
	BOOL	I_visib; 
	BOOL	I_forward; 
	BOOL	shad_temp; 
	BOOL	U_visib; 
	BOOL	U_shadow; 

} cell_state;



/*---------------------------- End of File -------------------------------*/
