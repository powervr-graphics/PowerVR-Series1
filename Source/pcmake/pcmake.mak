#-----------------------------------------------------------------------
# NMAKE Compatible makefile for PC Builds
#
# Created 14-Apr-1997 by Graham Connor
#
# Copyright		: 1995-2022 Imagination Technologies (c)
# License		: MIT
#
# $Log: pcmake.mak,v $
# Revision 1.39  1997/11/21  13:14:59  sxy
# added a timing build for developers, its a release build with state bar and
# polygon count per frame as options.
#
# Revision 1.38  1997/11/07  17:30:04  gdc
# added stuff to build a pvrsmx file
#
# Revision 1.37  1997/10/23  16:53:27  sxy
# added a compile option DLL_METRIC in developer build.
#
# Revision 1.36  1997/10/20  10:14:39  gdc
# incorporated build info so we know if its a Debug, Developer or Release build
#
# Revision 1.35  1997/10/16  10:43:06  gdc
# changed tmp dir use in developer builds cause it wasn't a DOS valid name
#
# Revision 1.34  1997/09/25  08:19:45  gdc
# removed unused build targets and changed debugdev target name to developer
# as it seem more intuitive
#
# Revision 1.33  1997/09/24  10:02:56  sxy
# Added a new build 'debugdev' to display DPFDEV messages as a developer's version.
#
# Revision 1.32  1997/09/03  10:32:55  dm
# removed adm to __asm def
#
# Revision 1.31  1997/07/24  14:34:20  gdc
# tidied up a bit, removed ISPTSP refrences and FORCE setting for PCX1 links
#
# Revision 1.30  1997/07/14  19:43:04  mjg
# Added PCX2_003 hardware build option.
#
# Revision 1.29  1997/07/11  16:20:16  gdc
# added a new build define so we shouldn't have to ship pvrif.h
#
# Revision 1.28  1997/06/05  16:06:06  dm
# When using VisualC define asm to be __asm to stop compile errors
# on PCX1
#
# Revision 1.27  1997/06/05  14:47:11  gdc
# added ICL=1 to OS_CFLAGS to get the better maths stuff for pentiums
# and stop disappearing tris in meshes
#
# Revision 1.26  1997/06/04  09:21:31  gdc
# hopefully finally...
#
# Revision 1.25  1997/06/04  09:07:48  gdc
# typo
#
# Revision 1.24  1997/06/04  09:02:09  gdc
# modfied what might be a typo - a DXDDKROOT to DXSDKROOT
#
# Revision 1.23  1997/06/04  08:34:47  gdc
# let us use the CC32ROOT var if C32_ROOT is not defined
#
# Revision 1.22  1997/06/03  18:59:03  jop
# jwhjwh, not sure why this was check out, hope I'm not messing
# things up by checking it in...
#
# Revision 1.21  1997/05/21  13:16:28  erf
# Added PVROSNAME define.
#
# Revision 1.20  1997/05/14  08:05:20  gdc
# chnaged hardware source to always point at pcx
#
# Revision 1.19  1997/05/08  11:30:25  mjg
# Made PCX1 use the FORCE
#
# Revision 1.18  1997/05/08  11:03:30  erf
# Changs to generate PVRDirect libraries.
#
# Revision 1.17  1997/05/01  08:57:54  jop
# Made all DLLs link to DLL c library to fix sharing issues and
# added the WIN32_LEAN_AND_MEAN def to fix compiler bug when
# compiling pvrosif.c
#
# Revision 1.16  1997/04/25  16:32:14  gdc
# added a target for a PVRMID?.DLL - this must be made explicitly
# i.e. nmake ...blah.. pvrd -> untested as I dont have enough for a build
#
# Revision 1.15  1997/04/21  22:31:38  gdc
# added new pvros target for dynamiclib build
#
# Revision 1.14  1997/04/18  16:34:48  gdc
# tried to turn off no return at end of file warning
#
# Revision 1.13  1997/04/18  13:13:26  gdc
# changed debug flags for VC++ to make them icl compatable
#
# Revision 1.12  1997/04/17  11:54:44  gdc
# as far a possible VER flags now come from version.h NOT command line
#
# Revision 1.11  1997/04/16  18:03:14  jop
# Fixed DOS32 release and debugopt builds
#
# Revision 1.10  1997/04/16  17:06:53  gdc
# added LIBNAME to default C flags
#
# Revision 1.9  1997/04/16  15:06:33  jop
# Fixed debug build flags for DOS build
#
# Revision 1.8  1997/04/16  13:52:36  jop
# changed WLIB flags to WLIBFLAGS
#
# Revision 1.7  1997/04/16  10:54:16  gdc
# added WLIB var
#
# Revision 1.6  1997/04/16  10:29:25  gdc
# added a target to update an auto generated version.h file,
# and turned up warning level
#
# Revision 1.5  1997/04/15  18:48:48  gdc
# version numbers now not controlled from here
#
# Revision 1.4  1997/04/14  18:18:39  gdc
# added different variable for hardware source
#
# Revision 1.3  1997/04/14  16:45:12  gdc
# made default path used to find MSVC stuff CC32ROOT rather than MSVC20PATH
#
# Revision 1.2  1997/04/14  16:34:22  gdc
# made DOS32 ASM be 'ml'
#
# Revision 1.1  1997/04/14  16:18:05  gdc
# Initial revision
#
#
#
#-----------------------------------------------------------------------


.SILENT:
.SUFFIXES:
.SUFFIXES: .h .c .asm .obj .res .rc .d
.c.obj:
.c.exe:
.asm.obj:
.asm.exe:


# define build

! if "$(HARDWARE)"!="PCX2_003" && "$(HARDWARE)"!="PCX2" && "$(HARDWARE)"!="PCX1"
!  error HARDWARE  must be PCX1, PCX2 or PCX2_003
! endif

! if "$(OS)"!="WIN32" && "$(OS)"!="DOS32"
!  error OS must be WIN32 or DOS32
! endif

!message WORKDIR: $(WORKDIR) ($(SECOND_PASS))

!ifdef SECOND_PASS
! include $(WORKDIR)\$(OS)$(HARDWARE).bld
BUILD = $(LAST_BUILD)
!else
! if exist($(WORKDIR)\$(OS)$(HARDWARE).bld)
!  include $(WORKDIR)\$(OS)$(HARDWARE).bld
!  ifdef BUILD
!   if "$(LAST_BUILD)"!="$(BUILD)"
PRE_CLEAN = clean_bin
!   endif
!  else
BUILD = $(LAST_BUILD)
!  endif
! else
!  ifndef BUILD
BUILD = debug
!  endif
PRE_CLEAN = clean_bin
! endif

! if "$(BUILD)"!="debug" && "$(BUILD)"!="debugopt" && "$(BUILD)"!="release" && "$(BUILD)"!="developer" && "$(BUILD)"!="timing"
!  error BUILD must be set to debug, debugopt, release, developer, timing.
! endif

! message HARDWARE: $(HARDWARE)
! message OS: $(OS)
! message BUILD: $(BUILD)

! if [@echo LAST_BUILD = $(BUILD) > $(WORKDIR)\$(OS)$(HARDWARE).bld]
! endif

!endif

# define tools and environment

###################################
# Direct Draw support
#

!if "$(DXSDKROOT)" == ""
!error Error : DXSDKROOT environment var must be defined for ddraw.h etc.
!endif
!if !exist($(DXSDKROOT)\SDK\INC\ddraw.h)
!error Bad environment variable DXSDKROOT : $(DXSDKROOT)\INC\ddraw.h not found
!endif
!if "$(SDKROOT)" == ""
!error Error : SDKROOT environment var must be defined for objbase.h etc.
!endif
!if !exist($(SDKROOT)\INCLUDE\objbase.h)
!error Bad environment variable SDKROOT : $(SDKROOT)\INCLUDE\objbase.h not found
!endif
!if "$(C32_ROOT)" == ""
!message Warning : C32_ROOT environment var must be defined to be your MSVC top level
!if "$(CC32ROOT)" == ""
!error
!else
!message Warning : Using CC32ROOT as C32_ROOT
C32_ROOT = $(CC32ROOT)
!endif
!endif
!if !exist($(PROTON)\bin\icl.exe)
!message Proton compiler not found - not generating dependancies!
NODEPS = 1
!endif

#
# End of direct draw support
###################################

CC = icl
DEP_CC = icl
SRC = $(SRCDIR)
BIN = $(BUILDDIR)

# Intel Compiler warning level
!ifndef w
w=3
!endif

# Auto generated file built by UNIX make pcmake
# Version information
!include $(SRC)\pcmake\version.dat

# Auto generated file built by UNIX make pcmake
# Objects
!include $(SRC)\pcmake\sglobjs.dat

! if "$(HARDWARE)"=="PCX2_003" || "$(HARDWARE)"=="PCX2" || "$(HARDWARE)"=="PCX1"
HARDWARE_FAMILY = PCX
HW_SRC = pcx
SGL_OBJ= $(SGL_OBJ) $(PCX_OBJ)
PVRD_OBJ= $(PVRD_OBJ) $(PCX_OBJ)
PVROSNAME = PVROS
!if "$(HARDWARE)"=="PCX2_003"
HW_DIR = MID5A
LIBNAME = SGLMID5A
PVRDNAME = PVRMID5A
!elseif "$(HARDWARE)"=="PCX2"
HW_DIR = MID5
LIBNAME = SGLMID5
PVRDNAME = PVRMID5
!elseif  "$(HARDWARE)"=="PCX1"
HW_DIR = MID4
LIBNAME = SGLMID4
PVRDNAME = PVRMID4
!endif
!endif

!if  "$(OS)"=="WIN32"
OS_DIR = W32
SGL_OBJ = $(SGL_OBJ) $(WIN32_OBJ)
PVRD_OBJ = $(PVRD_OBJ) $(PVRD_WIN32_OBJ)
!elseif  "$(OS)"=="DOS32"
SGL_OBJ = $(SGL_OBJ) $(DOS32_OBJ)
OS_DIR = D32
!endif

# define build specific environment and flags

DLL=dll
BUILDDIR = $(WORKDIR)\$(OS_DIR)$(HW_DIR)
#note this var over written in developer build because file name too long!!!
TMP = $(WORKDIR)\$(OS_DIR)$(HW_DIR)\$(BUILD).tmp
!ifdef NOTMPSRC
!message Not using intermediate source directory
TMPSRC = $(SRC)
!else
TMPSRC = $(WORKDIR)\source.tmp
!endif
INCLUDE = $(DXSDKROOT)\sdk\inc;$(C32_ROOT)\include
INC_COMMON = -I$(TMPSRC)\. -I$(TMPSRC)\$(HW_SRC) -I$(TMPSRC)\$(OS)
BASEFLAGS = -D_BUILDING_SGL_=1 -DLIBNAME=\"$(LIBNAME)\" -D$(OS)=1 -D$(HARDWARE)=1 $(INC_COMMON) 
RCBASEFLAGS = -dBUILD=\"$(BUILD)\" -d$(OS)=1 -d$(HARDWARE)=1 $(INC_COMMON)
CFLAGS = $(BASEFLAGS) -DBUILD=\"$(BUILD)\" -DWIN32_LEAN_AND_MEAN -DFULL_ON -DICL=1 -Gy -Zp4 -QIfdiv- -W$(w) -Qwr1 -nologo -Qpc32 -Qrct -Op-
RCFLAGS = $(RCBASEFLAGS)
DEP_FLAGS = -D_BUILDING_SGL_=1 -nologo -D$(OS)=1 -D$(HARDWARE)=1 $(INC_COMMON) 

!if "$(OS)"=="WIN32"
LINK = link
RC = rc
ASM = ml
ASMFLAGS = $(BASEFLAGS) -nologo -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6
OS_CC = $(C32_ROOT)\bin\cl
OS_CFLAGS = -c -DWIN32_LEAN_AND_MEAN $(BASEFLAGS) -DICL=1 -Gy -Zp4 -W$(w) -nologo
CFLAGS = $(CFLAGS)
INC_OS =  -I$(SRC)\..\vsgl -I$(DXSDKROOT)\sdk\INC -I$(SDKROOT)\INCLUDE
#VXDLINK = $(PROTON)\VBIN\link
VXDLINK = $(LINK)
LFLAGS = /PDB:NONE /NOLOGO /DLL /VERSION:4 /SUBSYSTEM:WINDOWS /MACHINE:IX86
TARGET = dynamiclib

!else if "$(OS)"=="DOS32"
ASM = ml
ASMFLAGS = $(VER_LIB) $(INC_COMMON) -nologo -c -Cx -Zm -Zd -Zi
CFLAGS = $(CFLAGS) -UWIN32
OS_CC = $(WATCOM)\binnt\wcc386
DEP_FLAGS = $(DEP_FLAGS) -UWIN32
OS_CFLAGS = -D$(OS)=1 -D$(HARDWARE)=1 $(INC_COMMON) -ei -fpi87 -fp5 -mf -w4 -5r -zq
INC_OS = -I$(WATCOM)\h -I$(WATCOM)\h\nt 
WLIBFLAGS = -b -c -n -q -s
WLIB = $(WATCOM)\binnt\wlib
TARGET = staticlib
!endif

!if "$(BUILD)"=="debug"
! if "$(OS)"=="WIN32"
CFLAGS = $(CFLAGS) -G5 -Od -Zi -DDEBUG
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=1 -Z7 -Yd
! else if "$(OS)"=="DOS32"
CFLAGS = $(CFLAGS) -G5 -Oi -DDEBUG
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=1 -d3
! endif
LFLAGS = $(LFLAGS) /DEBUG /DEBUGTYPE:BOTH /INCREMENTAL:NO
RCFLAGS = $(RCFLAGS)
ASMFLAGS = $(ASMFLAGS) -DDEBUG -Zd -Zi -DDEBLEVEL=1
!endif

!if "$(BUILD)"=="debugopt"
CFLAGS = $(CFLAGS) -DDEBUG -G5 -O2 -Oy -Qip -Qmem -Qscope_alloc -QW0,-mo_alternate_loops,-mo_block_loops,-mo_strip_mine -Oi
! if "$(OS)"=="WIN32"
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=1 -Zi
! else if "$(OS)"=="DOS32"
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=1
! endif
LFLAGS = $(LFLAGS) /DEBUG /DEBUGTYPE:BOTH /INCREMENTAL:NO
RCFLAGS = $(RCFLAGS)
ASMFLAGS = $(ASMFLAGS) -DDEBUG -Zd -Zi -DDEBLEVEL=1
!endif

!if "$(BUILD)"=="developer"
TMP = $(WORKDIR)\$(OS_DIR)$(HW_DIR)\develop.tmp
! if "$(OS)"=="WIN32"
CFLAGS = $(CFLAGS) -G5 -Od -Zi -DDEBUGDEV -DDLL_METRIC=1
OS_CFLAGS = $(OS_CFLAGS) -DDEBUGDEV=1 -Z7 -Yd -DDLL_METRIC=1
! else if "$(OS)"=="DOS32"
CFLAGS = $(CFLAGS) -G5 -Oi -DDEBUGDEV -DDLL_METRIC=1
OS_CFLAGS = $(OS_CFLAGS) -DDEBUGDEV=1 -d3 -DDLL_METRIC=1
! endif
LFLAGS = $(LFLAGS) /INCREMENTAL:NO
RCFLAGS = $(RCFLAGS)
ASMFLAGS = $(ASMFLAGS) 
!endif

!if "$(BUILD)"=="timing"
CFLAGS = $(CFLAGS) -G5 -O2 -Oy -Qip -DTIMING -DDLL_METRIC -Qmem -Qscope_alloc -QW0,-mo_alternate_loops,-mo_block_loops,-mo_strip_mine -Oi
! if "$(OS)"=="WIN32"
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=0 -DDEBUGDEV=0 -DTIMING=1 -DDLL_METRIC=1 -Gs -Gf -G5 -Ox 
! else if "$(OS)"=="DOS32"
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=0 -DDEBUGDEV=0 -DTIMING=1 -DDLL_METRIC=1
! endif
LFLAGS = $(LFLAGS) /INCREMENTAL:NO
RCFLAGS = $(RCFLAGS)
ASMFLAGS = $(ASMFLAGS)
!endif


!if "$(BUILD)"=="release"
CFLAGS = $(CFLAGS) -G5 -O2 -Oy -Qip -Qmem -Qscope_alloc -QW0,-mo_alternate_loops,-mo_block_loops,-mo_strip_mine -Oi
! if "$(OS)"=="WIN32"
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=0 -Gs -Gf -G5 -Ox
! else if "$(OS)"=="DOS32"
OS_CFLAGS = $(OS_CFLAGS) -DDEBUG=0
! endif
LFLAGS = $(LFLAGS) /INCREMENTAL:NO
RCFLAGS = $(RCFLAGS)
ASMFLAGS = $(ASMFLAGS)
!endif

! message SRC: $(SRC)
! message TMPSRC: $(TMPSRC)

# Inference rules
!include $(SRC)\pcmake\infrules.dat


# SGL specific

!ifdef SECOND_PASS 

all: $(TARGET)


staticlib:  $(BIN)\$(LIBNAME).lib

!ifdef NOCOPYFILES
dynamiclib: pvros $(BIN)\$(LIBNAME).dll $(BIN)\$(LIBNAME).lib pvrd vxd thin pvrsmx
!else
dynamiclib: pvros $(BIN)\$(LIBNAME).dll $(BIN)\$(LIBNAME).lib pvrd vxd thin pvrsmx copybin
!endif

copybin:
	@copy $(BIN)\$(PVRDNAME).dll $(WINBOOTDIR)
	@copy $(BIN)\$(LIBNAME).dll $(WINBOOTDIR)
	@copy $(WORKDIR)\sglthin\sgl.dll $(WINBOOTDIR)
	@copy $(BIN)\$(PVROSNAME).dll $(WINBOOTDIR)
	@copy $(BIN)\pvrsmx.dll $(WINBOOTDIR)


pvrd: $(BIN)\$(PVRDNAME).dll $(BIN)\$(PVRDNAME).lib

pvrsmx: $(BIN)\pvrsmx.dll

vxd: $(BIN)\vsgl.vxd

thin: $(WORKDIR)\sglthin\sgl.lib $(WORKDIR)\sglthin\sgl.dll

pvros: $(BIN)\$(PVROSNAME).lib $(BIN)\$(PVROSNAME).dll


!else

all: $(PRE_CLEAN) DIRS_$(OS) HEADERS_$(OS) $(TARGET)

#pvrd: $(PRE_CLEAN) DIRS_$(OS) HEADERS_$(OS) pvrd2

DIRS_WIN32:
	@-mkdir $(BUILDDIR)
	@-mkdir $(TMP)
	@-mkdir $(WORKDIR)\sglthin
	@-mkdir $(WORKDIR)\sglthin\$(BUILD).tmp
	@-mkdir $(TMPSRC)
	@-mkdir $(TMPSRC)\$(OS)
	@-mkdir $(TMPSRC)\$(HW_SRC)

DIRS_DOS32:
	@-mkdir $(BUILDDIR)
	@-mkdir $(TMP)
	@-mkdir $(TMPSRC)
	@-mkdir $(TMPSRC)\$(OS)
	@-mkdir $(TMPSRC)\$(HW_SRC)

clean_bin:
	@echo Cleaning binaries from last ($(LAST_BUILD)) build ...
	@-del $(BIN)\*.dll
	@-del $(BIN)\*.lib
	@-del $(BIN)\*.vxd

HEADERS_WIN32: $(BIN)\sgl.h $(BIN)\sglwin32.h

HEADERS_DOS32: $(BIN)\sgl.h

!ifdef NODEPS
$(TARGET): $(TMPSRC)\$(OS)\version.h second_pass
!else
$(TARGET): $(TMPSRC)\$(OS)\version.h $(TMPSRC)\sgl.hd $(TMPSRC)\sgl.cd $(TMP)\sgl.dep second_pass
!endif

#pvrd2: $(TMPSRC)\$(OS)\version.h $(TMPSRC)\sgl.hd $(TMPSRC)\sgl.cd $(TMP)\sgl.dep second_pass

$(TMPSRC)\$(OS)\version.h: $(SRC)\pcmake\version.h
	echo updating $(@)
	copy $(SRC)\pcmake\version.h $(@) 
	touch $(@)

!endif

!if "$(OS)"=="WIN32"
!include $(SRC)\pcmake\wintgt.dat
!elseif "$(OS)"=="DOS32"
!include $(SRC)\pcmake\dostgt.dat
!endif

# sgl.h

$(BIN)\sgl.h: $(SRC)\.\sgl.h
	@echo Copying sgl.h
	@copy $(SRC)\.\sgl.h $(BIN)\sgl.h


!ifdef NODEPS
second_pass:
	echo $(CMDLINE) SECOND_PASS=1
	@$(CMDLINE) SECOND_PASS=1
!else
# Auto generated file built by UNIX make pcmake
# Auto dependancies
!include $(SRC)\pcmake\autodep.dat
!endif
