/******************************************************************************
 * Name			: vesa.h
 * Title		: PowerVR Operating System dependent code (DOS4GW)
 * Author		: Mick Jones
 * Created		: 20/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	: Part of the low level operarting system specific API
 *                for DOS4GW, this requires an SVGA card with a VESA
                  UVB v2.0 BIOS 
 *
 * Platform		: ANSI
 *
 * Modifications:
 *
 *
 *  		20th March 1997		Initial Version		Mick Jones
 *
 *****************************************************************************/


#define MAX_MODES 64


typedef struct VbeInfoBlock
{
	sgl_uint8	VbeSignature[4];
	sgl_uint16	VbeVersion;
	sgl_uint32	OemStringPtr;
	sgl_uint32	Capabilities;
	sgl_uint32	VideoModePtr;
	sgl_uint16	TotalMemory;
	sgl_uint16	OemSoftwareRev;
	sgl_uint32	OemVendorNamePtr;
	sgl_uint32	OemProductNamePtr;
	sgl_uint32	OemProductRevPtr;
	sgl_uint8	Reserved[222];
	sgl_uint8	OemData[256];
} VbeInfoBlock;

typedef struct ModeInfoBlock
{
	sgl_uint16	ModeAttributes;
	sgl_uint8 	WinAAttributes;
	sgl_uint8 	WinBAttributes;
	sgl_uint16	WinGranularity;
	sgl_uint16	WinSize;
	sgl_uint16	WinASegment;
	sgl_uint16	WinBSegment;
	sgl_uint32	WinFuncPtr;
	sgl_uint16	BytesPerScanLine;

	sgl_uint16	XResolution;
	sgl_uint16	YResolution;
	sgl_uint8	XCharSize;
	sgl_uint8	YCharSize;
	sgl_uint8	NumberOfPlanes;
	sgl_uint8	BitsPerPixel;
	sgl_uint8	NumberOfBanks;
	sgl_uint8	MemoryModel;
	sgl_uint8	BankSize;
	sgl_uint8	NumberOfImagePages;
	sgl_uint8	Reserved1;

	sgl_uint8	RedMaskSize;
	sgl_uint8	RedFieldPosition;
	sgl_uint8	GreenMaskSize;
	sgl_uint8	GreenFieldPosition;
	sgl_uint8	BlueMaskSize;
	sgl_uint8	BlueFieldPosition;
	sgl_uint8	RsvdMaskSize;
	sgl_uint8	RsvdFieldPosition;
	sgl_uint8	DirectColorModeInfo;

	sgl_uint32	PhysBasePtr;
	sgl_uint32	OffScreenMemOffset;
	sgl_uint16	OffScreenMemSize;
	sgl_uint8	Reserved2[206];
} ModeInfoBlock;

typedef struct SupVbeInfoBlock
{
	char	    VbeSignature[7];
	sgl_uint16	VbeVersion;
	sgl_uint8	VbeSubFuncSup[8];
	sgl_uint16	OemSoftwareRev;
	sgl_uint32	OemVendorNamePtr;
	sgl_uint32	OemProductNamePtr;
	sgl_uint32	OemProductRevPtr;
	sgl_uint32	OemStringPtr;
	sgl_uint8	Reserved[221];
} SupVbeInfoBlock;

typedef struct ModeInfoBlockTable
{
	sgl_uint32  Mode;
	sgl_uint32	XResolution;
	sgl_uint32	YResolution;
	sgl_uint32	BitsPerPixel;
} ModeInfoBlockTable;


sgl_uint32 GetCurrentVGAModeNumber 		   (void);

sgl_uint16 EnumerateVBE2Modes              (ModeInfoBlockTable *ModeInfoTable);
sgl_uint16 ReturnVBE2ControllerInformation (VbeInfoBlock *VBEInfo);
sgl_uint16 ReturnVBE2ModeInformation       (sgl_uint16 Mode, ModeInfoBlock *ModeInfo);
sgl_uint16 SetVBE2Mode                     (sgl_uint16 Mode);
sgl_uint16 ReturnCurrentVBE2Mode           (void);
sgl_uint16 SetVBE2DisplayStart             (sgl_uint16 x, sgl_uint16 y, sgl_uint16 waitForVerticalRetrace);
sgl_uint16 SaveVBE2State                   (void);
sgl_uint16 RestoreVBE2State                (void);
