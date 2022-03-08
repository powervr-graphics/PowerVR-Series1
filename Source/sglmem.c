/*****************************************************************************
 *;++
 * Name            :    sglmem.c
 * Title           :    SGL Memory Managment debugging routines
 * C Author        :    Edward Forde
 * Created         :    27/03/97
 *
 * Copyright	   : 1995-2022 Imagination Technologies (c)
 * License		   : MIT
 *
 * Description     :	Routines to help people get log memory allocation deallocation
 *						calls from SGL and the other tools.
 *                    
 * Program Type    :   C module (ANSI)
 *
 * RCS info:
 * $Log: sglmem.c,v $
 * Revision 1.1  1997/05/13  17:56:37  mjg
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#define __SGLMEM_C__

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "sglmem.h"

#if LogRelease || DEBUG

sgl_bool LogMemoryCalls;
FILE	 *LogMemFile;

/* ////////////////////////////////////////////////////////////////
// Define Memory Function call logging routines
// ////////////////////////////////////////////////////////////////
*/

#define DO_MEM_CHECKS 1

#if WIN32 && DO_MEM_CHECKS
static long LastPhysMem;

#include "windows.h"

/*
// See how much memory has just been used by code other than SGL
*/
static void OutsidePhysMemUsage(void)
{
	long PhysFree;

	MEMORYSTATUS MStat;

	MStat.dwLength = sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&MStat);
	
	PhysFree = MStat.dwAvailPhys;
	
	if(PhysFree > LastPhysMem)
	{
		fprintf(LogMemFile, "EXF  	\t\t%ld  Phys Ram RELEASED Externally:\n", 
						PhysFree - LastPhysMem);

		LastPhysMem = PhysFree;
	}
	else if(PhysFree < LastPhysMem)
	{
		fprintf(LogMemFile, "EXA	\t\t%ld Phys Ram ALLOCATED Externally:\n", 
						LastPhysMem - PhysFree );

		LastPhysMem = PhysFree;
	}
}


static void PrintGlobalMemStatus(void)
{
	MEMORYSTATUS MStat;

	MStat.dwLength = sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&MStat);

	fprintf(LogMemFile, 
	   "Phys Mem Change %ld AVAIL:%ld",
			 MStat.dwAvailPhys - LastPhysMem, 
			 MStat.dwAvailPhys);

	LastPhysMem = MStat.dwAvailPhys;
}

#endif /*WIN32 && DO_MEM_CHECKS*/

/**************************************************************************
 * Function Name  : InitLogMemFile
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Opens a file to catch memory activities
 *
 **************************************************************************/
void CALL_CONV InitLogMemFile(void)
{
	LogMemoryCalls = SglReadPrivateProfileInt("Log", "Memory", 
													FALSE, "sgl.ini");
	if(LogMemoryCalls)
	{
		LogMemFile = fopen("logmem.txt","w");

		if(LogMemFile == NULL)
		{
			PVROSPrintf("Failed to open Memory Log file: Diabling Logging\n");
			LogMemoryCalls = FALSE;
		}
		else
		{
			fprintf(LogMemFile, "OP\t   PTR  \t AMOUNT\t FILE    Line\n");
			fprintf(LogMemFile, "==\t =======\t ======\t ============\n");
		}

		#if WIN32 && DO_MEM_CHECKS
		{
			MEMORYSTATUS MStat;
			GlobalMemoryStatus(&MStat);
			LastPhysMem = MStat.dwAvailPhys;
		}
		#endif
	}

}

/**************************************************************************
 * Function Name  : SGLLogMalloc
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Logs the malloc calls
 *
 **************************************************************************/
void * CALL_CONV SGLLogMalloc(size_t size, char * fname, int lineNum)
{
	void * ptr;

	ptr = malloc(size);

	if(LogMemoryCalls)
	{
		#if WIN32 && DO_MEM_CHECKS
			OutsidePhysMemUsage();
		#endif 

		fprintf(LogMemFile, "M\t %8lx\t %6ld\t%s %d\t", 
							(long)ptr, size, fname, lineNum);

		#if WIN32 && DO_MEM_CHECKS
			PrintGlobalMemStatus();
		#endif
		fprintf(LogMemFile,"\n");
	}

	return ptr;
}

/**************************************************************************
 * Function Name  : SGLLogCalloc
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Logs the calloc calls
 *
 **************************************************************************/
void * CALL_CONV SGLLogCalloc(size_t count, size_t size, char * fname, int lineNum)
{
	void * ptr;

	ptr = calloc(count, size);

	if(LogMemoryCalls)
	{
		#if WIN32 && DO_MEM_CHECKS
			OutsidePhysMemUsage();
		#endif 

		fprintf(LogMemFile, "M M\t %8lx\t %6ld\t%s %d\t", 
							(long)ptr, size*count, fname, lineNum);
		#if WIN32 && DO_MEM_CHECKS
			PrintGlobalMemStatus();
		#endif
		fprintf(LogMemFile,"\n");
	}

	return ptr;

}

/**************************************************************************
 * Function Name  : SGLLogRealloc
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Logs the Realloc calls
 *
 **************************************************************************/
void * CALL_CONV SGLLogRealloc(void* ptr, size_t size, char * fname, int lineNum)
{
	void * NewPtr;

	NewPtr = realloc(ptr, size);

	if(LogMemoryCalls)
	{
		#if WIN32 && DO_MEM_CHECKS
			OutsidePhysMemUsage();
		#endif 

		fprintf(LogMemFile, "FR\t %8lx\t ------\t%s %d\n", 
							(long)ptr, fname, lineNum);
		fprintf(LogMemFile, "MR\t %8lx\t %6ld\t%s %d\t", 
							(long)NewPtr, size, fname, lineNum);
		#if WIN32 && DO_MEM_CHECKS
			PrintGlobalMemStatus();
		#endif
		fprintf(LogMemFile,"\n");
	}

	return NewPtr;

}

/**************************************************************************
 * Function Name  : SGLLogFree
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Logs the Realloc calls
 *
 **************************************************************************/
void CALL_CONV SGLLogFree(void* ptr, char * fname, int lineNum)
{
	free(ptr);

	if(LogMemoryCalls)
	{
		#if WIN32 && DO_MEM_CHECKS
			OutsidePhysMemUsage();
		#endif 

		fprintf(LogMemFile, "F \t %8lx\t ------\t%s %d\t", 
							(long)ptr, fname, lineNum);
		#if WIN32 && DO_MEM_CHECKS
			PrintGlobalMemStatus();
		#endif
		fprintf(LogMemFile,"\n");
	}
}


#else

void CALL_CONV SGLLogFree(void* ptr, char * fname, int lineNum)
{

}

void * CALL_CONV SGLLogRealloc(void* ptr, size_t size, char * fname, int lineNum)
{
	return NULL;
}

void * CALL_CONV SGLLogMalloc(size_t size, char * fname, int lineNum)
{
	return NULL;
}

void * CALL_CONV SGLLogCalloc(size_t count, size_t size, char * fname, int lineNum)
{
	return NULL;
}

void CALL_CONV InitLogMemFile(void)
{
}

#endif /*DEBUG*/

#undef __SGLMEM_C__

/* end of $RCSfile: sglmem.c,v $ */

