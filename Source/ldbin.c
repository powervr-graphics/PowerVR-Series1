/*************************************************************************
 * Name         : 
 * Title        : LDBIN.C
 * Author       : Paul Hurley
 * Created      : 18/10/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : loads and executes a series of sgl commands stored in 
 *				  binary format.
 * 
 * Platform     : ANSI
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
  #define BIG_ENDIAN
  #include "e:/sabre/sgl/sgl.h"
  #include "e:/sabre/sgl/sgl_defs.h"
#else
  #include "sgl.h"
  #include "sgl_defs.h"
#endif

#include "ldbin.h"

/* Functions to read each command */
int read_VersionNumber(FILE *flIn);
int read_ObjectFile(FILE *flIn);
int read_ObjectMesh(FILE *flIn);
int read_ObjectConvex(FILE *flIn);
int read_recordPreviousValue(FILE *flIn);
int read_expectVariable(FILE *flIn);
int read_sgl_create_convex(FILE *flIn);
int read_sgl_create_mesh(FILE *flIn);
int read_sgl_add_vertices(FILE *flIn);
int read_sgl_create_material(FILE *flIn);
int read_LoadBMPTexture(FILE *flIn);
int read_sgl_add_simple_plane(FILE *flIn);
int read_sgl_add_plane(FILE *flIn);		
int read_sgl_set_diffuse(FILE *flIn);
int read_sgl_add_face(FILE *flIn);
int read_sgl_set_texture_map(FILE *flIn);

/* 
// The command list.
// New commands can (easily?) be added to this provided:
// 		1. a corresponding write_ is added to ./tools/psmtosgl/mshtobin.c. This write
//		has responsibility to write it's command number and corresponding size first. See mshtobin.c
//		for details.
//		2. a corresponding CXXX command number is added to ldbin.h
// 		3. It is placed at the end of both lists so as to make previous files still compatible.
// 		4. NUM_COMMANDS is altered accordingly.
//		5. The files are stored little-Endian. So if you directly write/read data you must make
//		sure that all values are written and subsequently converted as such. Again see code for 
//		examples.
*/

/* NUM_COMMANDS is used to check the validity of any command number */
#define NUM_COMMANDS	16
SGL_COMMAND sgl_command[NUM_COMMANDS]={
	{read_VersionNumber,			"VersionNumber"			},
	{read_ObjectFile,				"ObjectFile"			},
	{read_ObjectMesh,				"ObjectMesh"			},
	{read_ObjectConvex,				"ObjectConvex"			},
	{read_recordPreviousValue,		"recordPreviousValue"	},
	{read_expectVariable,			"expectVariable"		},
	{read_sgl_create_convex, 		"sgl_create_convex" 	}, 
	{read_sgl_create_mesh, 			"sgl_create_mesh"		},
	{read_sgl_add_vertices,	 		"sgl_add_vertices"		},
	{read_sgl_create_material, 		"sgl_create_material"	},
	{read_LoadBMPTexture, 			"LoadBMPTexture" 		},
	{read_sgl_add_simple_plane, 	"sgl_add_simple_plane"	},
	{read_sgl_add_plane,			"sgl_add_plane"			},
	{read_sgl_set_diffuse,			"sgl_set_diffuse"		},
	{read_sgl_add_face,				"sgl_add_face"			},
	{read_sgl_set_texture_map,		"sgl_set_texture_map"	}
};

/*
// The VarTable Structure is a translation table for each variable number.
*/
typedef struct _vartable
{
	int *var;
	int noVars;
} VarTable;

/* 
// The tables. (These must be global because read_XXXX functions can only have one parameter).
// 	RefTable - any variables to be used internally by future commands in this LoadSglBin.
// 	BMPTable - any variables that store texture names that will require
// 	deallocating by FreeSglBinTextures.
// 	MeshIDTable - Mesh ID tables to be stored for later use after calling LoadSglBin.
// 	MaterialIDTable - Material ID tables to be stored for later use after calling LoadSglBin.
*/
static VarTable RefTable, BMPTable, MeshIDTable, MaterialIDTable;

int nStartVarTable(VarTable *vt);
void vFreeVarTable(VarTable *vt);

/*
// the flag assigned by read_expectVariable, which tells which integer parameter(s) in the next
// command represent variables and not values. bit n on => int param n is a variable.
*/
static unsigned int unVarInPara; 

/* 
// the return value from the last read_XXXX function
*/
static int nCurRetValue;

/*===========================================
 * Function:	LoadSglBin
 *===========================================
 *
 * Scope:		SGL
 * Author:		Paul Hurley 
 *
 * Purpose:		Loads a file of SGL binary format.		
 *
 * Inputs:		char *pszFileName: The file to be loaded.
 *
 * Outputs:		int *noMeshes: If not NULL, it stores the number of meshes PLUS the number of 
 *				convex objects created.
 *				int **meshIDs: If not NULL, it is allocated the correct amount of memory and 
 *				stores the names of the meshes and convex objects. 
 *				
 *				int *noMaterials: If not NULL, it stores the number of materials created.
 *				int **materialIDs: If not NULL, it is allocated the correct amount of memory and 
 *				stores the names of the materials created.
 *
 *				int *noBMPTextures: If not NULL, it stores the number of BMP textures loaded.
 *				int **BMPTextureIDs: If not NULL, it is allocated the correct amount of memory and 
 *				stores the names of the bitmaps loaded. When you are finished with the object(s) 
 *				created by this call, you should call FreeSglBinTextures with noBMPTextures and
 *				BMPTextureIDs as parameters.
 *									 	
 * Return:		-ve error
 *
 * Globals accessed:	RefTable, BMPTable, MeshIDTable, MaterialIDTable, unVarInPara
 *========================================================================================*/
int CALL_CONV LoadSglBin( char *pszFileName, int *noMeshes, int **meshIDs,
						  int *noMaterials, int **materialIDs,
						  int *noBMPTextures, int **BMPTextureIDs )
{
	int nCommand, nSizeOfCommand;
	long filePosBefore;
	char IDStr[5];
	FILE *flIn;

	/* 
	// reset these variable tables in case they were used before 
	*/
	nStartVarTable(&RefTable);
	nStartVarTable(&BMPTable);
	nStartVarTable(&MeshIDTable);
	nStartVarTable(&MaterialIDTable);

	flIn=fopen(pszFileName,"rb");
	if(flIn==NULL)
	{
		DPF ((DBG_ERROR, "LoadSglBin: Error opening input binary file %s", pszFileName));
		return sgl_err_bad_parameter;
	}

	DPF ((DBG_MESSAGE, "LoadSglBin: Reading %s", pszFileName));
	/*
	// Read in identification string (without NULL) at start of file, check its validity,
	// and exit if it's not "SGLB"
	*/
	fread(IDStr, sizeof(char), 4, flIn);
	IDStr[4]=0; /* add null terminator */
	if(strcmp(IDStr, "SGLB")!=0) 
	{
		/* doesn't satisfy the criteria */
		DPF ((DBG_ERROR, "LoadSglBin: Not a valid SGL binary file %s", pszFileName));
		return sgl_err_bad_parameter;
	}

	/* initially no integer parameters are variables */
	unVarInPara=0;

	/*
	// main command loader: Read command and it's size and then call it's relevant function.
	*/
	while(!feof(flIn)) /* while still commands in the file */
	{
		fread(&nCommand, sizeof(int), 1, flIn); /* input command number */
		#ifdef BIG_ENDIAN
			nCommand=LITTLE_ENDIAN_32(nCommand);
		#endif

		
		if(nCommand==CEndSglBin)
		{
			/*
			// an explicit end binary file has been called
			*/
			break;
		}
		else if(nCommand<COMMAND_NUM_START || nCommand>=NUM_COMMANDS+COMMAND_NUM_START)
		{
			/*
			// command isn't valid 
			*/
			DPF ((DBG_ERROR, "LoadSglBin: Invalid command #%d at byte %d in %s", 
			  nCommand, ftell(flIn), pszFileName));
			return sgl_err_bad_parameter;
		}
		else
		{
			fread(&nSizeOfCommand, sizeof(int), 1, flIn);
			#ifdef BIG_ENDIAN
				nSizeOfCommand=LITTLE_ENDIAN_32(nSizeOfCommand);
			#endif

			DPF((DBG_MESSAGE, "LoadSglBin: Called %s",sgl_command[nCommand-COMMAND_NUM_START].CommandName));

			filePosBefore=ftell(flIn); /* store file pointer for checksum after */
			nCurRetValue=sgl_command[nCommand-COMMAND_NUM_START].fn(flIn);

			/*
			// checksum - if number of bytes read by command != what it's supposed to be 
			// This check should only be done in Debug mode.
			*/
			if(!feof(flIn) && ftell(flIn)!=nSizeOfCommand+filePosBefore)
			{
				DPF((DBG_WARNING, "LoadSglBin: Checksum error of %ld in command %s", 
				  nSizeOfCommand+filePosBefore-ftell(flIn), 
				  sgl_command[nCommand-COMMAND_NUM_START].CommandName));
			}
		}

		if(nCommand!=CexpectVariable)
		{
			unVarInPara=0; /* reset flag except if CExpectVariable just set it */
		}	
	} /* end while */

	fclose(flIn);

	if(noBMPTextures!=0) 
	{
		*noBMPTextures=BMPTable.noVars;
	}

	if(BMPTextureIDs!=0) /* if texture table is wanted for a later call to FreeSglBinTextures */
	{
		*BMPTextureIDs= BMPTable.var;
	}
	else /* table is no longer needed */
	{
		vFreeVarTable(&BMPTable); 
	}
	
	if(noMeshes!=0) 
	{
		*noMeshes=MeshIDTable.noVars;
	}

	if(meshIDs!=0)	/* if MeshIDTable is wanted for further use */
	{
		*meshIDs= MeshIDTable.var;
	}
	else /* don't need table */
	{
		vFreeVarTable(&MeshIDTable); 
	}

	if(noMaterials!=0)
	{
		*noMaterials=MaterialIDTable.noVars;
	}

	if(materialIDs!=0) /* if MaterialIDTable is wanted for further use */
	{
		*materialIDs=MaterialIDTable.var;
	}
	else /* don't need table */
	{
		vFreeVarTable(&MaterialIDTable); 
	}

	vFreeVarTable(&RefTable); /* don't need the RefTable anymore */
	
	return 0; /* success */
}

/*===========================================
 * Function:	FreeSglBinTextures
 *===========================================
 *
 * Scope:		SGL
 * Author:		Paul Hurley 
 *
 * Purpose:		To free up memory for textures originally allocated upon a call to LoadSglBin.
 *
 * Params:		int noBMPTextures, int *BMPTextureIDs - the data as stored by LoadSglBin.
 *
 * Return:		none
 *
 * Globals accessed: none   
 *========================================================================================*/
void CALL_CONV FreeSglBinTextures( int noBMPTextures, int *BMPTextureIDs )
{
	int i;

	for(i=0;i<noBMPTextures;i++)
	{
		if(BMPTextureIDs[i]!=-1)
		{
			FreeBMPTexture(BMPTextureIDs[i]);
			BMPTextureIDs[i]=-1;
		}
	}
	
	/* no longer need them */
	SGL_FREE(BMPTextureIDs);
}

/*===========================================
 * Function:	nStartVarTable
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley 
 *
 * Purpose:		Initialises a variable table (constructor). A variable table allocates
 *				in chunks of 10. e.g. 12 variables in the table means the table has space for 20.
 *
 * Params:		VarTable *vt: The variable table to be initialised.
 *
 * Return:		zero on failure to allocate memory.
 *
 * Globals accessed:   none
 *========================================================================================*/
int nStartVarTable(VarTable *vt) /* var table constructor */
{
	vt->noVars=0;

	vt->var= (int *) SGL_CALLOC(10,sizeof(int));
	
	return vt->var!=NULL; /* failure if no heap */
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Variable Table manipulation functions
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*===========================================
 * Function:	vFreeVarTable
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley 
 *
 * Purpose:		Destructor for a variable table.
 *
 * Params:		VarTable *vt: The variable table to be deallocated.
 *
 * Return:		none
 *
 * Globals accessed:	none   
 *========================================================================================*/
void vFreeVarTable(VarTable *vt) /* var table destructor */
{
	if(vt->noVars!=0)
	{
		SGL_FREE(vt->var);
	}
	vt->noVars=0;
}

/*===========================================
 * Function:	nGetNextVarName
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley 
 *
 * Purpose:		Returns the next available valid variable number.
 *
 * Params:		VarTable *vt: The variable table.
 *
 * Return:		-1 on failure to allocate new memory
 *
 * Globals accessed:   none
 *========================================================================================*/
int nGetNextVarName(VarTable *vt)
{
	if(vt->noVars%10==0)
	{
		/* we need more memory for the table - allocate another 10 ints */
	 	vt->var=(int *) SGL_REALLOC(vt->var, (vt->noVars+10)*sizeof(int));
		if(!vt->var)
		{
			return -1; /* no heap */
		}
	}
													 
	vt->noVars++;
	return vt->noVars-1;
}

void vSetVarValue(VarTable *vt, int var, int what)
{
	vt->var[var]=what;
}

void vSetNextVar(VarTable *vt, int what)
{
	vSetVarValue(vt, nGetNextVarName(vt), what);
}

int nGetVarValue(VarTable *vt, int what)
{
	return (what<0 || what>=vt->noVars) ? -1:vt->var[what];
}


/*===========================================
 * Function:	getInt
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley 
 *
 * Purpose:		Reads an integer from a file. Also, translates that integer to it's variable
 *				value if it has been identified as a variable by a preceeding call to 
 *				CexpectVariable.
 *
 * Params:		int *x: Pointer to where the integer read is stored.
 				FILE *fl: the file from where the integer is read.
 *
 * Return:		none
 *
 * Globals accessed:	unVarInPara: unVarInPara has bit n on if the nth integer parameter 
 *						in the current command is to be a variable.
 *========================================================================================*/
void getInt(int *x, FILE *fl)
{
	/* TODO: big-endian/little-endian problems */

	fread(x, sizeof(int), 1, fl);

	#ifdef BIG_ENDIAN
		*x=LITTLE_ENDIAN_32(*x);
	#endif

	/* if x represents a variable (as opposed to value) translate it to its value */
	if(unVarInPara & 1)
	{
		*x=nGetVarValue(&RefTable, *x);
	}
	
	unVarInPara>>=1; /* shift over to check next integer flag */
}

/* shorthand for reading an sgl_vector from a binary file */
void getSglVec(sgl_vector x, FILE *fl)
{
	#ifdef BIG_ENDIAN
		unsigned int *pnX;
	#endif

	fread(x, sizeof(sgl_vector), 1, fl);

	#ifdef BIG_ENDIAN
		pnX=(unsigned int *) x;
		pnX[0]=LITTLE_ENDIAN_32(pnX[0]);
		pnX[1]=LITTLE_ENDIAN_32(pnX[1]);
		pnX[2]=LITTLE_ENDIAN_32(pnX[2]);
	#endif
}

/* shorthand for reading an sgl_2d_vec from a binary file */
void getSgl2dVec(sgl_2d_vec x, FILE *fl)
{
	#ifdef BIG_ENDIAN
		unsigned int *pnX;
	#endif

	fread(x, sizeof(sgl_2d_vec), 1, fl);

	#ifdef BIG_ENDIAN
		pnX=(unsigned int *) x;
		pnX[0]=LITTLE_ENDIAN_32(pnX[0]);
		pnX[1]=LITTLE_ENDIAN_32(pnX[1]);
	#endif
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// The command reading functions
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*===========================================
 * Functions:	read_XXXXXXXXX
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley 
 *
 * Purpose:		Each read_XXXXX file is a function that is in the command lookup table,
 *				with command value CXXXXX (no roman numeral jokes please). Each
 *				command has responsibility for reading in its own data. Then each
 *				command must call it's usual command (eg read_sgl_add_vertices reads in
 *				its data and then calls sgl_add_vertices itself). There is a
 *				corresponding write_XXXXX file in the converter which essentially does
 *				the opposite to each of these functions.
 *				Note the special commands such as read_ObjectMesh which are not sgl commands
 *				and how they are treated.
 *
 * Return:		TRUE if the XXXXX file would normally have a void type, and otherwise the return
 *				value given by the sgl call.
 *
 *========================================================================================*/

/*
//
*/
int read_VersionNumber(FILE *flIn)
{
	char verStr[5];
	
	fread(verStr, sizeof(char), 5, flIn);
	/* TODO: verify the version number is SGL_BIN_VER or just skip it */

	return TRUE;
} 

/*
//
*/
int read_ObjectFile(FILE *flIn)
{  
	char *str;

	unsigned len;

	getInt((int *) &len, flIn);

	str= (char *) SGL_MALLOC(len+1);
	if(!str)
	{
		return sgl_err_no_mem;
	}

	fread(str, sizeof(char), len, flIn);
	str[len]=0;

	DPF((DBG_MESSAGE, "read_ObjectFile: Information - %s", str));

	SGL_FREE(str);

	return TRUE;
}

/*
// 
*/
int read_ObjectMesh(FILE *flIn)
{
	int nMeshSize;
	char *fname;
	unsigned len;

	getInt((int *) &len,flIn);

	fname= (char *) SGL_MALLOC(len+1);
	if(!fname)
	{
		return sgl_err_no_mem;
	}

	fread(fname, sizeof(char), len, flIn);
	fname[len]=0;

	getInt(&nMeshSize, flIn);

	DPF((DBG_MESSAGE, "LoadSglBin: Mesh from %s has %d bytes", fname, nMeshSize));

	SGL_FREE(fname);

	return TRUE;
}

/*
//
*/
int read_ObjectConvex(FILE *flIn)
{
	int nConvexSize;
	getInt(&nConvexSize, flIn);

	return TRUE;
}

/*
//
*/
int read_recordPreviousValue(FILE *flIn)
{
	sgl_bool bKeep;
	getInt(&bKeep,flIn);

	if(bKeep & KeepForReference)
	{
		vSetNextVar(&RefTable, nCurRetValue);
	}
	if(bKeep & KeepForBMPID)
	{
		vSetNextVar(&BMPTable, nCurRetValue);
	}
	if(bKeep & KeepForMeshID)
	{
		vSetNextVar(&MeshIDTable, nCurRetValue);
	}
	if(bKeep & KeepForMaterialID)
	{
		vSetNextVar(&MaterialIDTable, nCurRetValue);
	}

	return nCurRetValue;
}

/* 
//  whichparameters is a 32-bit int which has the nth bit on if the nth integer parameter coming up
//  is a variable not a value.
*/
int read_expectVariable(FILE *flIn)
{	 
	fread(&unVarInPara, sizeof(unsigned int), 1, flIn);

	#ifdef BIG_ENDIAN
		unVarInPara=LITTLE_ENDIAN_32(unVarInPara);
	#endif

	return TRUE;
}

int read_sgl_create_convex(FILE *flIn)
{
	sgl_bool generate_name;

	getInt(&generate_name, flIn);

	return sgl_create_convex(generate_name);
}

int read_sgl_create_mesh(FILE *flIn)
{
	sgl_bool generate_name;

	getInt(&generate_name, flIn);

	return sgl_create_mesh(generate_name);
}

int read_sgl_add_vertices(FILE *flIn)
{
	#ifdef BIG_ENDIAN
		int i;
	#endif

	int			num_to_add;
	sgl_vector  *vertices, *vertex_normals;
	sgl_2d_vec  *vertex_uvs;
	sgl_bool	bNormals, bUVs;

	/* setting defaults to NULL pointer */
	vertex_normals=NULL;
	vertex_uvs=NULL;

	getInt(&num_to_add, flIn);

	vertices= (sgl_vector *) SGL_CALLOC(num_to_add, sizeof(sgl_vector));
	if(!vertices)
	{
		return sgl_err_no_mem;
	}
	
	#ifdef BIG_ENDIAN
		for(i=0;i<num_to_add;i++)
		{
			getSglVec(vertices[i], flIn);
		}
	#else
		fread(vertices, sizeof(sgl_vector), num_to_add, flIn);
	#endif

	getInt(&bNormals, flIn);
	if(bNormals)
	{
		vertex_normals= (sgl_vector *) SGL_CALLOC(num_to_add, sizeof(sgl_vector));
		if(!vertex_normals)
		{
			SGL_FREE( vertices );
			return sgl_err_no_mem;
		}
		#ifdef BIG_ENDIAN
			for(i=0;i<num_to_add;i++)
			{
				getSglVec(vertex_normals[i], flIn);
			}
		#else
			fread(vertex_normals, sizeof(sgl_vector), num_to_add, flIn);
		#endif
	}

	getInt(&bUVs, flIn);
	if(bUVs)
	{
		vertex_uvs= (sgl_2d_vec *) SGL_CALLOC(num_to_add, sizeof(sgl_2d_vec));
		if(!vertex_uvs)
		{
			SGL_FREE( vertices );
			if(bNormals)
			{
				SGL_FREE( vertex_normals );
			}
			return sgl_err_no_mem;
		}
		#ifdef BIG_ENDIAN
			for(i=0;i<num_to_add;i++)
			{
				getSgl2dVec(vertex_uvs[i], flIn);
			}
		#else
			fread(vertex_uvs, sizeof(sgl_2d_vec), num_to_add, flIn);
		#endif
	}

	sgl_add_vertices(num_to_add, vertices, vertex_normals, vertex_uvs);

	SGL_FREE(vertices);
	if(bNormals)
	{
		SGL_FREE( vertex_normals );
	}
	if(bUVs)
	{
		SGL_FREE( vertex_uvs );
	}

	return TRUE;
}

int read_sgl_create_material(FILE *flIn)
{
 	sgl_bool generate_name;
	sgl_bool param_is_local;

	getInt(&generate_name, flIn);
	getInt(&param_is_local, flIn);

	return sgl_create_material(generate_name, param_is_local);
}


int read_LoadBMPTexture(FILE *flIn)
{
	char *pszFilename;
	sgl_bool bTranslucent;
	sgl_bool generate_mipmap;
	sgl_bool dither;

	unsigned slen; /* slen is the filename string length */
	int nResult;

	getInt((int *) &slen, flIn);

	pszFilename=(char *) SGL_MALLOC(slen+1);
	if(!pszFilename)
	{
		return sgl_err_no_mem;
	}

	fread(pszFilename, slen, 1, flIn);
	pszFilename[slen]=0;

	getInt(&bTranslucent, flIn);
	getInt(&generate_mipmap, flIn);
	getInt(&dither, flIn);

	nResult=LoadBMPTexture(pszFilename, bTranslucent, generate_mipmap, dither);

	/* DPF((DBG_MESSAGE, "LoadSglBin: Texture: %s with result %d\n", pszFilename, nResult)); */

	SGL_FREE(pszFilename);

	return nResult;
}

int read_sgl_add_simple_plane(FILE *flIn)
{
	sgl_vector surface_point;
	sgl_vector normal;
	sgl_bool invisible;
	
	getSglVec(surface_point, flIn);
	getSglVec(normal, flIn);
	getInt(&invisible, flIn);

	sgl_add_simple_plane( surface_point, normal, invisible );

	return TRUE;
}

int read_sgl_add_plane(FILE *flIn)	
{
	sgl_vector	surface_point, point2, point3;
	sgl_bool	invisible;
	sgl_bool	supply_normals;
	sgl_vector	normal1, normal2, normal3;
	sgl_bool	supply_uv;
	sgl_2d_vec	uv1, uv2, uv3;
		
	float *pN1, *pN2, *pN3;
	float *pUV1,*pUV2,*pUV3;

	getSglVec(surface_point, flIn);
	getSglVec(point2, flIn);
	getSglVec(point3, flIn);
	getInt(&invisible, flIn);

	getInt(&supply_normals, flIn);
	if(supply_normals)
	{
		getSglVec(normal1, flIn);
		getSglVec(normal2, flIn);
		getSglVec(normal3, flIn);

		pN1 = normal1;
		pN2 = normal2;
		pN3 = normal3;
	}
	else
	{
		pN1 = NULL;
		pN2 = NULL;
		pN3 = NULL;
	}

	getInt(&supply_uv, flIn);
	if(supply_uv)
	{
		getSgl2dVec(uv1, flIn);
		getSgl2dVec(uv2, flIn);
		getSgl2dVec(uv3, flIn);

		pUV1 = uv1;
		pUV2 = uv2;
		pUV3 = uv3;
	}
	else
	{
		pUV1 = NULL;
		pUV2 = NULL;
		pUV3 = NULL;
	}

	sgl_add_plane(surface_point, point2, point3, invisible, pN1, pN2, pN3,
										 pUV1, pUV2, pUV3);

	return TRUE;
}
			
int read_sgl_set_diffuse(FILE *flIn)
{
	sgl_colour colour;
	getSglVec(colour, flIn);

	sgl_set_diffuse(colour);
	return TRUE;
}

int read_sgl_add_face(FILE *flIn)
{
	#ifdef BIG_ENDIAN
		int i;
	#endif

	int num_face_points;
	int *vertex_ids;

	getInt(&num_face_points, flIn);

	vertex_ids= (int *) SGL_CALLOC(num_face_points, sizeof(int) );
	if(!vertex_ids)
	{
		return sgl_err_no_mem;
	}

	#ifdef BIG_ENDIAN
		for(i=0;i<num_face_points;i++)
		{
			getInt(&vertex_ids[i], flIn);
		}
	#else
		fread(vertex_ids, sizeof(int), num_face_points, flIn);
	#endif

	sgl_add_face(num_face_points, vertex_ids);

	SGL_FREE(vertex_ids);

	return 1;
}

int read_sgl_set_texture_map(FILE *flIn)
{
	int texture_name;
	sgl_bool flip_u, flip_v;

	getInt(&texture_name, flIn);
	getInt(&flip_u, flIn);
	getInt(&flip_v, flIn);

	sgl_set_texture_map(texture_name, flip_u, flip_v);

	return TRUE;
}

/* end of ldbin.c */
