/******************************************************************************
 * Name : hwsabsim.h
 * Title : 
 * Author : Stel Michael
 * Created : 23/06/1995 ported from version started OCT 94
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : headers for an implementation in software of the ISP
				 ASIC (sabre)
				 
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwsabsim.h,v $
 * Revision 1.3  1995/07/26  16:09:30  sm
 * changed it
 *
 * Revision 1.2  1995/06/29  15:52:14  sm
 * added regs
 *
 * Revision 1.1  1995/06/27  17:22:55  sm
 * Initial revision
 *
 *
 *****************************************************************************/

/* global registers	*/

extern	UINT32	FogRegister;

extern	UINT32	NumRegionsReg;

extern	UINT32 ParamStartAddrReg;

extern void	InitPowerTable();


extern	double	InitialCalc(long x, long y, long a, long b, long c);


extern	double Adder(long A,double C);


extern	cell_control ExpandInstruction(int instr,BOOL do_nop,BOOL sec_object);

extern	cell_control expand_instruction(int instr,BOOL do_nop,BOOL sec_object);


extern	int	SurfProcess(cell_state *cell,cell_control	instruction,long  C,unsigned long index,BOOL *shadow,long *U_plane_depth);


extern	long conv20_to_30(long	number);

extern	void	HWISPRenderer();

/*---------------------------- End of File -------------------------------*/
