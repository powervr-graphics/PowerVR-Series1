/******************************************************************************
 * Name : hwregs.c
 * Title : 
 * Author : Stel Michael
 * Created : 27/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : hardware register setting  functions for simulator
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwregs.c,v $
 * Revision 1.3  1997/04/10  20:04:24  mjg
 * Removed HWScheduleScreenSwap
 *
 * Revision 1.2  1997/01/13  10:58:57  erf
 * Starting TSP parameter ID is 6. 2 and 4 reserved for masking support
 * and fast fogging.
 *
 * Revision 1.1  1996/11/14  14:13:07  erf
 * Initial revision
 *
 * Revision 1.1  1996/11/14  12:32:44  erf
 * Initial revision
 *
 * Revision 1.2  1996/04/22  15:46:52  pdl
 * pTexasStore now points to the bottom of texture memory.
 *
 * Revision 1.1  1995/12/18  14:22:16  sjf
 * Initial revision
 *
 * Revision 1.15  1995/11/16  11:45:46  pdl
 * made it compatable with multiple sabre stuff.
 *
 * Revision 1.14  1995/10/23  10:25:31  jrc
 * Added dummy functions for HWSetupBunchOfBRIDGE|ISP|TSP|Regs()
 *
 * Revision 1.13  1995/10/20  10:47:33  sjf
 * Added support for independant single/double buffering of texas
 * and sabre params.
 *
 * Revision 1.12  1995/10/12  11:01:28  sjf
 * Fixed cut-and-paste bug.  Wasn't setting bufferB in use flag.
 *
 * Revision 1.11  1995/10/11  14:21:26  sjf
 * 1) Added parameter double buffering
 * 2) Fixed size of Sabre parameter store - it was larger than could be accessed
 *    by sabre.
 *
 * Revision 1.10  1995/07/26  18:15:24  sm
 * fixed typo
 *
 * Revision 1.9  1995/07/26  18:00:52  sm
 * added index in GetRegParamM...
 *
 * Revision 1.8  1995/07/26  17:36:37  sm
 * made setregionreg subtract one
 *
 * Revision 1.7  1995/07/26  15:34:50  sm
 * changed to not free
 *
 * Revision 1.6  1995/07/26  15:15:52  sm
 * changed mem management routines
 *
 * Revision 1.5  1995/07/21  12:26:56  sm
 * forgot cast!
 *
 * Revision 1.4  1995/07/21  12:21:00  sm
 * added param store allocation routine
 *
 * Revision 1.3  1995/07/19  12:48:22  sm
 * changed type of param address
 *
 * Revision 1.2  1995/06/29  15:55:12  sm
 * change a name
 *
 * Revision 1.1  1995/06/29  14:40:40  sm
 * Initial revision
 *
 *
 *****************************************************************************/

#include "../sgl.h"
#include "../sgl_defs.h"
#include "sabre.h"
#include "hwsabsim.h"
#include "../dvregion.h"
#include "../hwinterf.h"


/*
// Specify the sizes of parameter space.
// These are bit a nasty at the mo' .. must fix with better external consts
*/
#define TEXAS_MEM_SIZE	((1L << 18) << 1) /*18 bits of address - each address 
										  2 words */
#define OBJ_PCOUNT_SHIFT (19)
#define SABRE_MEM_SIZE	(1L<<OBJ_PCOUNT_SHIFT) 

UINT32 TexParamSize;
static	UINT32	* pTexasStore;
static	UINT32	* pSabreStore;

static	UINT32	SabreMemLimit;
static	UINT32	TexasMemLimit;			

static	sgl_bool	Allocated=FALSE;	


static sgl_bool		SabreBuffAInUse= FALSE;
static sgl_bool		SabreBuffBInUse= FALSE;

static sgl_bool		TexasBuffAInUse= FALSE;
static sgl_bool		TexasBuffBInUse= FALSE;

UINT32 NoOfSabres=0;


UINT32	FogRegister;
UINT32	NumRegionsReg;
UINT32  ParamStartAddrReg;



void	HWSetFogRegister(UINT32	FogShiftValue)
{
	/* set a global register variable */ 	

	FogRegister = FogShiftValue;   

	DPF((DBG_WARNING, "Not a warning... Fog level %lx", FogRegister));
}




void	HWSetRegionsRegister(UINT32	NumRegions)
{
	/* set a global register variable */ 	
	/*
	// Note that hardware actually takes one less than the actual
	// number of regions
	*/
	
	NumRegionsReg = NumRegions-1;
}


void HWSetSabParamBuffRegister(const PARAM_BUFFER_IDS WhichBuffer)
{
	DPF((DBG_MESSAGE, "Setting sabre buffer ptr to buffer:%d",WhichBuffer));
	/* 
	// set a global register variable to point to the correct buffer.
	//
	// NOTE address is the same for Both buffers in the simulator!
	*/ 	
	if(WhichBuffer == PARAM_BUFFER_ID_A)
	{
		ParamStartAddrReg = pSabreStore;
	}
	else if(WhichBuffer == PARAM_BUFFER_ID_B)
	{
		ParamStartAddrReg = pSabreStore;
	}
	else
	{
		ASSERT(FALSE);
	}
}

void HWSetSabPtrRegister(const UINT32 RegionBase, const UINT32 RegionOffset)
{
	DPF((DBG_MESSAGE, "Setting sabre Region index to:0x%lx",RegionBase));
	ParamStartAddrReg = RegionBase;
}




int  HWFinishedRender(void)
{
	/* parameter store is always free with sim */

	return 1;
}






void	HWReset(void)
{

	/* do nothing in simulator */

}

void	HWStartRender()
{

	/* call the simulated renderer */
	
	HWISPRenderer();

}

/**************************************************************************
* Function Name  : HWGetSabreParamMemRefs
* Inputs         : none
* Outputs        : Parameter Buffer management structure 
*
* Returns        : TRUE if a buffer was allocated.
*
* Global Used    : Local allocation "flag" values
*
* Description    : This returns a set of pointers and indices to manage
*				   Hardware parameter buffers.
*
*		THERE ARE TWO VERSIONS HERE: One assumes the hardware parameter
*		memory space can double buffer the Sabre params,
*		while the 2nd assumes that we can only single buffer the Sabre
*		params. In the latter situation, a "Software" buffer is used and
*		then this is copied to the hardware param space when that becomes
*		available.
*
*
**************************************************************************/

#define SIM_SABRE_DBL_BUFF 1

sgl_bool HWGetSabreParamMemRefs( HW_PARAM_BUFF_STRUCT *pSabreParamBuf)
{

	BOOL GotBuffer;
	ASSERT(Allocated);
	
#if SIM_SABRE_DBL_BUFF
	/*
	// Divide the parameter stores into 2 halves. The first will
	// be A, and the second B. The halves will be determined by
	// the indices.
	*/
	if (!SabreBuffAInUse)
	{
		/*
		// Sabre Values
		*/
		pSabreParamBuf->BufferId 	= PARAM_BUFFER_ID_A;
		pSabreParamBuf->pParamStore = pSabreStore;
		pSabreParamBuf->StartIndex	= 0;
		pSabreParamBuf->IndexLimit	= SabreMemLimit/2; 

		SabreBuffAInUse = TRUE;

		GotBuffer = TRUE;
	}
	else if(!SabreBuffBInUse)
	{
		/*
		// Sabre Values
		// NOTE texas HAS to start on an even word boundary, so
		// we might as well start sabre on one as well
		*/
		pSabreParamBuf->BufferId 	= PARAM_BUFFER_ID_B;
		pSabreParamBuf->pParamStore = pSabreStore;
		pSabreParamBuf->StartIndex	= (SabreMemLimit / 2 + 2) & ~1L;
		pSabreParamBuf->IndexLimit	= SabreMemLimit; 

		SabreBuffBInUse = TRUE;


		GotBuffer = TRUE;
	}
	/*
	// Nothing is available
	*/
	else
	{
		pSabreParamBuf->BufferId 	= PARAM_BUFFER_ID_NONE;
		pSabreParamBuf->pParamStore = NULL;
		pSabreParamBuf->StartIndex	= -1000;
		pSabreParamBuf->IndexLimit	= -1000; 

		GotBuffer = FALSE;
	}

#else 

    /* ****************************
	// "Single" buffer sabre params 
	// For the purposes of the simulator, this just means saying that
	// Buffer "B" is a buffer in the processor memory for Sabre, I.E
	// Must copy it to Buffer A when it becomes available.
	**************************** */

	if (!SabreBuffAInUse)
	{
		pSabreParamBuf->BufferId 	= PARAM_BUFFER_ID_A;
		pSabreParamBuf->pParamStore = pSabreStore;
		pSabreParamBuf->StartIndex	= 0;
		pSabreParamBuf->IndexLimit	= SabreMemLimit/2; 

		SabreBuffAInUse = TRUE;


		GotBuffer = TRUE;
	}
	/*
	// If the main buffer is not available, then use the software
	// buffer for sabre.
	*/
	else if(!SabreBuffBInUse)
	{
		/*
		// Sabre Values
		// NOTE texas HAS to start on an even word boundary, so
		// we might as well start sabre on one as well
		*/
		pSabreParamBuf->BufferId 	= PARAM_BUFFER_ID_SW;
		pSabreParamBuf->pParamStore = pSabreStore;
		pSabreParamBuf->StartIndex	= (SabreMemLimit / 2 + 2) & ~1L;
		pSabreParamBuf->IndexLimit	= SabreMemLimit; 

		SabreBuffBInUse = TRUE;

		GotBuffer = TRUE;
	}
	/*
	// Nothing is available
	*/
	else
	{
		pSabreParamBuf->BufferId 	= PARAM_BUFFER_ID_NONE;
		pSabreParamBuf->pParamStore = NULL;
		pSabreParamBuf->StartIndex	= -1000;
		pSabreParamBuf->IndexLimit	= -1000; 

		GotBuffer = FALSE;
	}
#endif

 
	return GotBuffer;
}



/**************************************************************************
* Function Name  : HWGetTexasParamMemRefs
* Inputs         : none
* Outputs        : Parameter Buffer management structure 
*
* Returns        : TRUE if a buffer was allocated.
*
* Global Used    : Local allocation "flag" values
*
* Description    : This returns a set of pointers and indices to manage
*				   Hardware parameter buffers.
*
*
**************************************************************************/

sgl_bool HWGetTexasParamMemRefs(HW_PARAM_BUFF_STRUCT *pTexasParamBuf)
{

	BOOL GotBuffer;
	ASSERT(Allocated);
	
	/*
	// Divide the parameter stores into 2 halves. The first will
	// be A, and the second B. The halves will be determined by
	// the indices.
	*/
	if (!TexasBuffAInUse)
	{
		/*
		// Texas Values
		// NOTE We can't use the 0 entry as this is reserved, 
		// and all Texas indexes MUST be even
		*/
		pTexasParamBuf->BufferId 	= PARAM_BUFFER_ID_A;
		pTexasParamBuf->pParamStore = pTexasStore;
		pTexasParamBuf->StartIndex	= 6;
		pTexasParamBuf->IndexLimit  = TexasMemLimit/2;
	
		TexasBuffAInUse = TRUE;

		GotBuffer = TRUE;
	}
	else if(!TexasBuffBInUse)
	{
		/*
		// Texas Values
		// NOTE all Texas indexes MUST be even
		*/
		pTexasParamBuf->BufferId 	= PARAM_BUFFER_ID_B;
		pTexasParamBuf->pParamStore = pTexasStore;
		pTexasParamBuf->StartIndex	= (TexasMemLimit / 2 + 6) & ~1L;
		pTexasParamBuf->IndexLimit  = TexasMemLimit;
	
		TexasBuffBInUse = TRUE;

		GotBuffer = TRUE;
	}
	/*
	// Nothing is available
	*/
	else
	{
		pTexasParamBuf->BufferId 	= PARAM_BUFFER_ID_NONE;
		pTexasParamBuf->pParamStore = NULL;
		pTexasParamBuf->StartIndex	= -1000;
		pTexasParamBuf->IndexLimit	= -1000; 

		GotBuffer = FALSE;
	}

	return GotBuffer;
}





/**************************************************************************
* Function Name  : HWReleaseSabreParamMem
* Inputs         : Sabre Buffer IDs
* Outputs        : None
*
* Returns        : None
*
* Global Used    : Local allocation "flag" values
*
* Description    : Releases the parameter memory blocks.
*
*					There are 2 versions - one where Sabre
*					has double buffered Hardware Parameter memory, and
*					the other Sabre has one and the other must be in "host" memory
*					and copied to the correct palce at the end of rendering.
*
**************************************************************************/
void HWReleaseSabreParamMem(const PARAM_BUFFER_IDS SabreBufferIdAllocated)
{

#if SIM_SABRE_DBL_BUFF

	if(SabreBufferIdAllocated == PARAM_BUFFER_ID_A)
	{
		ASSERT(SabreBuffAInUse);
		SabreBuffAInUse = FALSE;
	}
	else if(SabreBufferIdAllocated == PARAM_BUFFER_ID_B)
	{
		ASSERT(SabreBuffBInUse);
		SabreBuffBInUse = FALSE;
	}
	else
	{
		ASSERT(FALSE);
	}

#else
	/*
	// Sabre is single buffered mode
	*/
	if(SabreBufferIdAllocated == PARAM_BUFFER_ID_A)
	{
		ASSERT(SabreBuffAInUse);
		SabreBuffAInUse = FALSE;
	}
	else if(SabreBufferIdAllocated == PARAM_BUFFER_ID_SW)
	{
		ASSERT(SabreBuffBInUse);
		SabreBuffBInUse = FALSE;
	}
	else
	{
		ASSERT(FALSE);
	}
#endif
}


/**************************************************************************
* Function Name  : HWReleaseTexasParamMem
* Inputs         : Texas Buffer IDs
* Outputs        : None
*
* Returns        : None
*
* Global Used    : Local allocation "flag" values
*
* Description    : Releases the parameter memory blocks.
*
**************************************************************************/
void HWReleaseTexasParamMem(const PARAM_BUFFER_IDS TexasBufferIdAllocated)
{
	/*
	// Release Texas
	*/
	if(TexasBufferIdAllocated == PARAM_BUFFER_ID_A)
	{
		ASSERT(TexasBuffAInUse);
		TexasBuffAInUse = FALSE;
	}
	else if(TexasBufferIdAllocated == PARAM_BUFFER_ID_B)
	{
		ASSERT(TexasBuffBInUse);
		TexasBuffBInUse = FALSE;
	}
	else
	{
		ASSERT(FALSE);
	}
	
}





/**************************************************************************
* Function Name  : HWInitParamMem
* Inputs         : 
* Outputs        : None
*
* Returns        : 0 if OK.
*
* Global Used    : lots
*
* Description    : Initialises the parameter memory and control flags.
*
**************************************************************************/

int	HWInitParamMem()
{
	int err=-1;

	if (!Allocated)
	{
		pSabreStore = (UINT32 *) calloc(SABRE_MEM_SIZE,sizeof(UINT32));
		if ( pSabreStore ==NULL)
		{
	   		return err;
		}

		/*PCX1 stores parameters in texture memory */

		pTexasStore = (UINT32 *) textureMemory; 
		if ( pTexasStore ==NULL )
		{
			return err;
		}

		SabreMemLimit = SABRE_MEM_SIZE-1;
		TexasMemLimit = (TEX_PARAM_SIZE>>2)-1; /* I THINK this is words*/

		Allocated = TRUE;

		err = 0;
	}
	
    return err;
}


/******************************************************************************
* Function Name  : HWSetupBunchOfBRIDGERegs
* Inputs         : None
* Outputs        : None
*
* Returns        : -
*
* Global Used    : None
*
* Description    : Dummy replacement in the simulator for the hardware function
*
******************************************************************************/
void HWSetupBunchOfBRIDGERegs() {}


/******************************************************************************
* Function Name  : HWSetupBunchOfISPRegs
* Inputs         : None
* Outputs        : None
*
* Returns        : -
*
* Global Used    : None
*
* Description    : Dummy replacement in the simulator for the hardware function
*
******************************************************************************/
void HWSetupBunchOfISPRegs() {}


/******************************************************************************
* Function Name  : HWSetupBunchOfTSPRegs
* Inputs         : None
* Outputs        : None
*
* Returns        : -
*
* Global Used    : None
*
* Description    : Dummy replacement in the simulator for the hardware function
*
******************************************************************************/
void HWSetupBunchOfTSPRegs() {}


/**************************************************************************
* Function Name  : HWRdValFile
* Inputs         : NONE
* Outputs        : NONE
* Input/Output	  : NONE
*
* Returns        : Returns either value in file or specified default.
*
* Global Used    : Reads config file out of windows
*
* Description    : Dummy replacement in the simulator for the hardware function
*
*
**************************************************************************/

UINT32 HWRdValFile(char * RegName , UINT32 DefaultVal)
{
	DPF((DBG_MESSAGE, 
 	   "Reg \"%s\" Using Default  %lx", RegName, DefaultVal));
	/*do nothing*/
	
	return DefaultVal;
}

/*
// END OF FILE
*/

