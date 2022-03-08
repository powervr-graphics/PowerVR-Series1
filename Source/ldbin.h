/******************************************************************************
 * Name         : 
 * Title        : LDBIN.H
 * Author       : Peter Leaback
 * Created      : 03/08/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Header file for ldbin.c
 * 
 * Platform     : ANSI
 *
*/

#ifndef SGL_BIN
#define SGL_BIN

/* the current version number of SGLB */
#define SGL_BIN_VER	"010d1"

/* what value the first command takes on */
#define COMMAND_NUM_START 1

typedef struct
{
	unsigned short versionNo;
	char text[80]; 
} SGL_BINARY_HEADER;

typedef struct
{
	int (*fn)(FILE *);
	char *CommandName;
} SGL_COMMAND;

/* 
// The command list.
// Order MUST be maintained and correspond to SGL_COMMAND list in ldbin.c
*/
enum {
	CEndSglBin=-1,
	CVersionNumber=COMMAND_NUM_START,
	CObjectFile,
	CObjectMesh,
	CObjectConvex,
	CrecordPreviousValue,
	CexpectVariable,
	Csgl_create_convex,
	Csgl_create_mesh,
	Csgl_add_vertices,
	Csgl_create_material,
	CLoadBMPTexture,
	Csgl_add_simple_plane,
	Csgl_add_plane,
	Csgl_set_diffuse,
	Csgl_add_face,
	Csgl_set_texture_map
};

/* 
// used by (read/write)_recordPreviousValue 
*/
#define KeepForBMPID		0x1
#define KeepForReference	0x2
#define KeepForMeshID		0x4
#define KeepForMaterialID	0x8

#endif
