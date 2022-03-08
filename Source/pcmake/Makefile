#-----------------------------------------------------------------
# Makefile for PC Builds
#
# Created 14-Apr-1997 by Graham Connor
#
# Copyright		: 1995-2022 Imagination Technologies (c)
# License		: MIT
#
# $Log: Makefile,v $
# Revision 1.65  1997/10/27  10:28:56  erf
# Added new files for D3D d3dlines.c and d3dpoints to replace
# dlines.c and dpoints.c respectively. This is required so that
# we can select between using Z and 1/w.
#
# Revision 1.64  1997/09/22  09:48:26  erf
# Changed heap.c to sglheap.c for SGL and SGLDirect dlls since non-shareable
# memory heap is required.
#
# Revision 1.63  1997/09/19  13:44:03  erf
# PVRMID.dlls now use d3dheap.c for memory heap. This is because it
# has different memory requirements.
#
# Revision 1.62  1997/09/10  13:53:32  erf
# Removed dtri.c from PVRMID projects. Only d3dtri.c is needed.
#
# Revision 1.61  1997/09/08  13:37:43  jrc
# Corrected spelling of dependancy.
#
# Revision 1.60  1997/09/08  10:40:47  erf
# D3D now uses d3dtriml.asm for the ProcessTriCoreLite assembler routine.
# Cut down version that is in dtriml.asm.
#
# Revision 1.59  1997/09/01  12:19:54  mjg
# Added D3D translucency sorting routines.
#
# Revision 1.58  1997/08/21  12:37:38  erf
# Changed name of d3dregion.c to d3dreg.c .
#
# Revision 1.57  1997/08/20  16:57:25  erf
# New file d3dtri.c added to implement DrawPrimitive at a low level.
#
# Revision 1.56  1997/08/08  08:42:24  gdc
# added dsprite to sgl build
#
# Revision 1.55  1997/08/08  08:33:03  erf
# Added new file for D3D d3dregion.c which replaces dregion.c .
#
# Revision 1.54  1997/07/24  14:41:32  gdc
# introduced notion of hardware family and tidied up
#
# Revision 1.53  1997/07/14  19:42:24  mjg
# Added PCX2_003 hardware build option.
#
# Revision 1.52  1997/07/04  08:22:25  erf
# Added sgltri.c to PVRDirect since it is required for shadow and
# light volumes.
#
# Revision 1.51  1997/06/27  15:11:23  mlj
# Added a new module to talk to VxD's under Windoes
#
# Revision 1.50  1997/06/22  17:15:34  erf
# Added dtexnpml.asm to the PVRMID and SGLMID builds. Used for the assembler
# routines to process non-perspective textures.
#
# Revision 1.49  1997/06/02  03:25:26  jop
# Added new heap struff and sorted things out for DX5 build
#
# Revision 1.48  1997/05/21  13:15:12  erf
# Added version info to PVRMID.DLL and PVROS.DLL.
#
# Revision 1.47  1997/05/14  22:10:22  erf
# Removed hwdevice.c from PVRDirect library.
#
# Revision 1.46  1997/05/14  03:44:22  jop
# Merged PCX1/PCX2 stuff into pcx directory - PCX2 directory
# is now obsolete
#
# Revision 1.45  1997/05/13  19:05:35  mjg
# Removed pvrosmem.
#
# Revision 1.44  1997/05/13  18:00:58  mjg
# Added sglmem.c to PVRD and SGL.
#
# Revision 1.43  1997/05/13  17:55:58  erf
# Added dtexnp.c to pvrmid and sglmid libraries.
#
# Revision 1.42  1997/05/12  12:55:32  erf
# Added dlines.c to PVRDirect library.
#
# Revision 1.41  1997/05/12  09:11:43  erf
# Reverted what I did in Version 1.39.
#
# Revision 1.40  1997/05/09  19:03:25  gdc
# added dlines.c
#
# Revision 1.39  1997/05/09  17:53:53  erf
# Removed singmath.c and sgl_math.c files from PVRDirect library.
#
# Revision 1.38  1997/05/08  13:12:32  jop
# Fixed PVRD build
#
# Revision 1.37  1997/05/08  11:28:13  erf
# Now builds PVRDirect as seperate libraries.
#
# Revision 1.36  1997/05/07  23:09:53  mjg
# Removed heapman.c from PVROS (and everything else for that matter...)
#
# Revision 1.35  1997/04/30  22:04:31  jop
# Converted to PVRIF interface defs
#
# Revision 1.34  1997/04/29  15:21:18  erf
# 1. Added pvrdebug.c as the debug file for pvros.dll.
# 2. Removed heapman.c from sglmidX.dll.
#
# Revision 1.33  1997/04/28  21:13:16  mjg
# Updated the naming convention of texintf.
#
# Revision 1.32  1997/04/28  01:47:55  mjg
# Moved heapman to win32.
#
# Revision 1.31  1997/04/28  00:17:35  mjg
# Removed pvrosif.c
#
# Revision 1.30  1997/04/27  22:44:18  jop
# Added pvrosif.c to sglmid5
#
# Revision 1.29  1997/04/26  16:01:57  erf
# Removed ^M's. Yahooo !!!
#
# Revision 1.28  1997/04/26  14:12:44  erf
# Added heapman
#
# Revision 1.27  1997/04/25  16:48:58  mjg
# Added texintf.c
#
# Revision 1.26  1997/04/25  16:33:43  gdc
# started adding in stuff for a pvrd target - not complete yet
# but wont break anything
#
# Revision 1.25  1997/04/23  20:30:43  mjg
# Moved brdsetup from sgl into sgl/win32
#
# Revision 1.24  1997/04/23  14:25:48  gdc
# but boardsetup in pvros - delete any local versions of brsdetup.*
# and the ones in the unix pcx? dirs when you get brdsetup.c in sgl
#
# Revision 1.23  1997/04/22  16:28:52  gdc
# added Ed's new pvrd.c file containing JWH's entry points
#
# Revision 1.22  1997/04/22  10:57:15  gdc
# dos, dos, dos
#
# Revision 1.21  1997/04/22  10:31:30  gdc
# please, please build Mr.Dos...
#
# Revision 1.20  1997/04/22  10:25:46  gdc
# ...maybe the dos build will work now?
#
# Revision 1.19  1997/04/22  10:11:47  gdc
# mod to hopefully allow dos buidls to ignore PVROS stuff
#
# Revision 1.18  1997/04/21  22:31:05  gdc
# added stuff for new pvros target
#
# Revision 1.17  1997/04/18  13:27:01  gdc
# added a rule to build mod_auto.h
#
# Revision 1.16  1997/04/17  16:54:34  sxy
# Added texapiml.asm
#
# Revision 1.15  1997/04/17  15:18:03  sxy
# changed textapi to texapi
#
# Revision 1.14  1997/04/17  13:07:08  gdc
# more versioning changes
#
# Revision 1.13  1997/04/17  11:52:13  gdc
# more changes to versioning, and depandancy for verrc.rc file
#
# Revision 1.12  1997/04/16  15:49:14  gdc
# made it sun friendly but with less conditionals
#
# Revision 1.11  1997/04/16  15:00:37  gdc
# typo
#
# Revision 1.10  1997/04/16  14:56:42  gdc
# fixed awk stuff to be sun compatable
#
# Revision 1.9  1997/04/16  14:02:21  gdc
# more messing around with fmts
#
# Revision 1.8  1997/04/16  13:56:39  gdc
# fixed an awk fmt line
#
# Revision 1.7  1997/04/16  13:53:13  gdc
# chnaged obj file output to one per line
#
# Revision 1.6  1997/04/16  10:25:21  gdc
# created reference to an auto generated version.h file,
# and removed separate VXD version number (now uses VER_LIB_ values)
#
# Revision 1.5  1997/04/15  18:42:11  gdc
# started autmating versioning
#
# Revision 1.4  1997/04/15  09:41:13  gdc
# fixed AWK lines
#
# Revision 1.3  1997/04/14  18:44:21  gdc
# regularised AWK formats into variables
#
# Revision 1.2  1997/04/14  17:55:38  gdc
# changed unix directory for pcx1 stuff from pcx1 to pcx
#
# Revision 1.1  1997/04/14  17:11:17  gdc
# Initial revision
#
#
#-----------------------------------------------------------------

AWK = gawk
ECHO = echo
HCF = /user/rcs/revfiles/sabre/configs/sgl.hcf

ifndef $(USER)
USER=$(shell whoami)
endif

ifndef $(PRODUCT)
PRODUCT="Internal VideoLogic Build"
endif

ifndef $(REV)
REV =$(shell rlog -b $(HCF) | grep 'revision '| head -1 | sed s\/'revision '\/''\/)
endif

AWKLINE_0 = 'BEGIN{RS=FS}; END{print}; { printf(" $$(TMP)\\%s\\\n",$$1) }'
AWKLINE_1 = 'BEGIN{RS=FS}; END{print}; (NR%3==0){print "\\"}; { printf("\t$$(TMP)\\%s ",$$1) }'
AWKLINE_2 = 'BEGIN{RS=FS}; { print "!include $$(TMP)\\" $$1 }'
AWKLINE_3 = 'BEGIN{RS=FS}; (NR%3==0){print "\\"}; { printf("\t$$(TMP)\\%s ",$$1) }'
AWKLINE_4 = 'BEGIN{RS=FS}; { printf("$$(TMP)\\%s:%s\n",$$1,$$1) }'
AWKLINE_5 = 'BEGIN{RS=FS}; END{print}; (NR%3==0){print "\\"}; { printf("\t$$(TMPSRC)\\%s ",$$1) }'
AWKLINE_6 = 'BEGIN{RS=FS}; (NR%3==0){print "\\"}; { printf("\t$$(TMPSRC)\\%s ",$$1) }'
AWKLINE_7 = 'BEGIN{FS="."}; {printf("-DVER_LIB_MAX=%d -DVER_LIB_MA=%d -DVER_LIB_MI=%d -DVER_LIB_MIN=%d\n",$$1,$$2,$$3,$$4)}'
AWKLINE_8 = 'BEGIN{FS="."}; {printf("\#define VER_LIB_MAX %d\n\#define VER_LIB_MA %d\n\#define VER_LIB_MI %d\n\#define VER_LIB_MIN %d\n",$$1,$$2,$$3,$$4) }'

ifeq ($(shell arch), ip22)
	AWK = nawk
    # echo on Irix follow SysV rules so will "mess" up
    # see if an alternative can be found
    ifneq ($(notdir $(shell which xecho)), xecho)
        CAUTION = "Warning building NMAKE Makefiles on Batman produces invalid files"
        CURE = "Try copying file /user/gdc/bin/Irix5/xecho into your path"
    else
        ECHO = xecho
    endif
endif


#dumb way to get .h files - but if you can think of a better!?
SGL_H = $(strip $(subst /,\\,$(subst ../,,$(wildcard ../*.h))))
WIN32_H = $(strip $(subst /,\\,$(subst ../,,$(wildcard ../win32/*.h))))
DOS32_H = $(strip $(subst /,\\,$(subst ../,,$(wildcard ../dos32/*.h))))
PCX_H = $(strip $(subst /,\\,$(subst ../,,$(wildcard ../pcx/*.h))))

WIN32_H += $(strip $(subst /,\\,win32/verrc.rc))

SGL_H += modauto.h

PVRD_SRC=	d3dlines.c	d3dpoint.c	dtex.c		dtexnp.c	disp.c \
            d3dtsort.c  d3disp.c  	dtsp.c		d3dreg.c	d3dtri.c \
			metrics.c	parmbuff.c	dshade.c    pvrd.c	\
			pkisp.c		pktsp.c		debug.c	    sgl_math.c	sgltri.c \
			singmath.c

SGL_LITE=  dsprite.c	dlines.c 	dpoint.c	dtex.c		dtexnp.c	disp.c\
           dtsp.c       dshade.c	dtri.c		sgltri.c	sgllite.c

SGL_COMMON= error.c 	rnglobal.c	txmops.c	ldbmp.c		nm_imp.c \
            sgl_math.c	singmath.c	dvdevice.c	metrics.c  	parmbuff.c \
            list.c		dregion.c	pkisp.c		pktsp.c    	debug.c

SGL_STD =	dlconvex.c  dldelete.c  dlcamera.c	dllists.c	dlglobal.c \
            dllod.c 	dlmater.c   dlmesh.c  	dlpoint.c   dltransf.c \
            rnlights.c	rnmater.c 	rnrender.c 	\
            rnstate.c  	rntransf.c  rntrav.c    rnlod.c	    rnpoint.c  \
            rnqualit.c	adjacenc.c  rnconvex.c  dlobject.c  rncamera.c \
            rnconvpr.c  rnfshade.c	array.c	   	rnreject.c \
            rnmesh.c    dllights.c  rnshadow.c	rnshadrj.c	dlnewtrn.c \
            dlqualit.c  rntex.c		rnshade.c	sgl_init.c


PVROS_WIN32_SRC=	win32/system.c	win32/display.c	 win32/pvros.rc \
			win32/texapiml.asm win32/pci.asm win32/fast.asm win32/brdcfg.c \
			win32/brdsetup.c win32/texif.c win32/heap.c

PVROS_SRC= texapi.c tmalloc.c profile.c callback.c debug.c

PCX_SRC = pcx/hwregs.c pcx/texas.c

WIN32_SRC = win32/sgl.rc win32/w32dll.c win32/fast.asm win32/hwdevice.c \
	    win32/sglwin.c win32/dispml.asm win32/dtexml.asm win32/dtriml.asm \
	    win32/pvrosif.c win32/sglheap.c win32/dtexnpml.asm

PVRD_WIN32_SRC = win32/pvrd.rc win32/pvrddll.c win32/fast.asm\
                 win32/dispml.asm win32/dtexml.asm win32/d3dtriml.asm \
                 win32/pvrosif.c win32/d3dheap.c win32/dtexnpml.asm

DOS32_SRC = dos32/fast.asm dos32/pci.c dos32/hwdevice.c dos32/brdcfg.c \
	        dos32/vds.c	dos32/vesa.c dos32/pvrosapi.c dos32/rizla.asm \
	        dos32/irq.c	dos32/isr.asm	dos32/vxd.c \
			dos32/dispml.asm dos32/dtexml.asm dos32/dtriml.asm

PVROS_PC_SRC = $(strip $(subst /,\\,$(PVROS_WIN32_SRC)) $(PVROS_SRC))
PVROS_ASM = $(filter %.asm,$(PVROS_PC_SRC))
PVROS_C = $(filter %.c,$(PVROS_PC_SRC))
PVROS_RC = $(filter %.rc,$(PVROS_PC_SRC))
PVROS_OBJ = $(subst win32\\,,$(PVROS_C:.c=.obj) $(PVROS_ASM:.asm=.obj) $(PVROS_RC:.rc=.res))
PVROS_DOBJ = $(subst win32\\,,$(PVROS_C:.c=.obj))
PVROS_DEP  = $(subst .c,.d,$(subst win32\\,,$(PVROS_C)))

PVRD_PC_SRC = $(strip $(subst /,\\,$(PVRD_WIN32_SRC)) $(PVRD_SRC))
PVRD_C = $(filter %.c,$(PVRD_PC_SRC))
PVRD_RC = $(filter %.rc,$(PVRD_PC_SRC))
PVRD_ASM = $(filter %.asm,$(PVRD_PC_SRC))
PVRD_DOBJ = $(subst win32\\,,$(PVRD_C:.c=.obj))
PVRD_OBJ = $(subst win32\\,,$(PVRD_C:.c=.obj) $(PVRD_ASM:.asm=.obj) $(PVRD_RC:.rc=.res))
PVRD_DEP = $(subst .obj,.d,$(PVRD_DOBJ))

SGL_SRC = $(SGL_STD) $(SGL_COMMON) $(SGL_LITE)
SGL_PC_SRC = $(strip $(subst /,\\,$(SGL_SRC)))
SGL_C = $(strip $(SGL_PC_SRC))
SGL_OBJ = $(SGL_C:.c=.obj)
SGL_DEP = $(subst .c,.d,$(SGL_C))

PCX_PC_SRC = $(strip $(subst /,\\,$(PCX_SRC)))
PCX_C = $(filter %.c,$(PCX_PC_SRC))
PCX_OBJ = $(subst pcx\\,,$(PCX_C:.c=.obj))
PCX_DEP = $(subst .c,.d,$(subst pcx\\,,$(PCX_C)))

WIN32_PC_SRC = $(strip $(subst /,\\,$(WIN32_SRC)))
WIN32_C = $(filter %.c,$(WIN32_PC_SRC))
WIN32_RC = $(filter %.rc,$(WIN32_PC_SRC))
WIN32_ASM = $(filter %.asm,$(WIN32_PC_SRC))
WIN32_OBJ = $(subst win32\\,,$(WIN32_C:.c=.obj) $(WIN32_ASM:.asm=.obj) $(WIN32_RC:.rc=.res))
WIN32_DOBJ = $(subst win32\\,,$(WIN32_C:.c=.obj))
WIN32_DEP = $(subst .c,.d,$(subst win32\\,,$(WIN32_C)))

DOS32_PC_SRC = $(strip $(subst /,\\,$(DOS32_SRC) $(PVROS_SRC)))
DOS32_C = $(filter %.c,$(DOS32_PC_SRC))
DOS32_ASM = $(filter %.asm,$(DOS32_PC_SRC))
DOS32_OBJ = $(subst dos32\\,,$(DOS32_C:.c=.obj) $(DOS32_ASM:.asm=.obj))
DOS32_DOBJ = $(subst dos32\\,,$(DOS32_C:.c=.obj))
DOS32_DEP = $(subst .c,.d,$(subst dos32\\,,$(DOS32_C)))

OUTFILE = "autodep.dat"
SGLOBJS = "sglobjs.dat"
# version.dat only used for basic versioning of the VxD - full versions
# come by use of the ADRC2VXD program
VERFILE = "version.dat"
VERRC = "version.h"

pcmake: ../modauto.h version autodepend sglobjs
	@$(ECHO) Making PCMAKE

######################################################
#
# Target to automatically generate the file modauto.h
#
../modauto.h:  ../mod_id.h  ../enum2h.c
	cd ..; cc -o enum2h.exe enum2h.c; ./enum2h.exe	mod_id.h modauto.h Modules

version:
	@rm -f $(VERFILE) $(VERRC)
	@$(ECHO) Versioning to version $(REV)
	@$(ECHO) "#define __VERSION__" >> $(VERRC)
	@$(ECHO) "$(REV)" | $(AWK) $(AWKLINE_8) >> $(VERRC)
	@$(ECHO) "#define VER_LIB_TXT "\""$(REV)$(USER)"\" >> $(VERRC)
	@$(ECHO) "#define VER_PROD_TXT "\""$(PRODUCT)"\" >> $(VERRC)
	@$(ECHO) -n "VER_LIB=" >> $(VERFILE)
	@$(ECHO) "$(REV)" | $(AWK) $(AWKLINE_7) >> $(VERFILE)

sglobjs:
	@$(ECHO) Creating Objects file...
	@rm -f $(SGLOBJS)
	@$(ECHO) "SGL_OBJ= \\" >> $(SGLOBJS)
	@$(ECHO) "$(SGL_OBJ)" | $(AWK) $(AWKLINE_0) >> $(SGLOBJS)
	@$(ECHO) "" >> $(SGLOBJS)
	@$(ECHO) "DOS32_OBJ= \\" >> $(SGLOBJS)
	@$(ECHO) "$(DOS32_OBJ)" | $(AWK) $(AWKLINE_0) >> $(SGLOBJS)
	@$(ECHO) "" >> $(SGLOBJS)
	@$(ECHO) "WIN32_OBJ= \\" >> $(SGLOBJS)
	@$(ECHO) "$(WIN32_OBJ)" | $(AWK) $(AWKLINE_0) >> $(SGLOBJS)
	@$(ECHO) "" >> $(SGLOBJS)
	@$(ECHO) "PCX_OBJ= \\" >> $(SGLOBJS)
	@$(ECHO) "$(PCX_OBJ)" | $(AWK) $(AWKLINE_0) >> $(SGLOBJS)
	@$(ECHO) "" >> $(SGLOBJS)
	@$(ECHO) "PVROS_OBJ= \\" >> $(SGLOBJS)
	@$(ECHO) "$(PVROS_OBJ)" | $(AWK) $(AWKLINE_0) >> $(SGLOBJS)
	@$(ECHO) "" >> $(SGLOBJS)
	@$(ECHO) "PVRD_OBJ= \\" >> $(SGLOBJS)
	@$(ECHO) "$(PVRD_OBJ)" | $(AWK) $(AWKLINE_0) >> $(SGLOBJS)
	@$(ECHO) "" >> $(SGLOBJS)

autodepend:
ifdef CAUTION
	@$(ECHO) $(CAUTION)
	@$(ECHO) $(CURE)
endif
	@$(ECHO) Creating Dependancy file...
	@rm -f $(OUTFILE)
	@$(ECHO) "!ifdef SECOND_PASS" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$(SGL_DEP)" | $(AWK) $(AWKLINE_2) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "!if \"$$(OS)\"==\"WIN32\"" >> $(OUTFILE)
	@$(ECHO) "$(PVROS_DEP)" | $(AWK) $(AWKLINE_2) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$(PVRD_DEP)" | $(AWK) $(AWKLINE_2) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$(WIN32_DEP)" | $(AWK) $(AWKLINE_2) >> $(OUTFILE)
	@$(ECHO) "!elseif \"$$(OS)\"==\"DOS32\"" >> $(OUTFILE)
	@$(ECHO) "$(DOS32_DEP)" | $(AWK) $(AWKLINE_2) >> $(OUTFILE)
	@$(ECHO) "!endif" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "!if \"$$(HARDWARE_FAMILY)\"==\"PCX\"" >> $(OUTFILE)
	@$(ECHO) "$(PCX_DEP)" | $(AWK) $(AWKLINE_2) >> $(OUTFILE)
	@$(ECHO) "!endif" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "!else " >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$$(TMP)\\sgl.dep: $$(HARDWARE_FAMILY)_d $$(OS)_d \\" >> $(OUTFILE)
	@$(ECHO) "$(SGL_DEP)" | $(AWK) $(AWKLINE_3) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "	@echo Dependancy file update complete >> $$(TMP)\\sgl.dep" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "DOS32_d: \\" >> $(OUTFILE)
	@$(ECHO) "$(DOS32_DEP)" | $(AWK) $(AWKLINE_1) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "WIN32_d: PVROS_d PVRD_d \\" >> $(OUTFILE)
	@$(ECHO) "$(WIN32_DEP)" | $(AWK) $(AWKLINE_1) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PCX_d: \\" >> $(OUTFILE)
	@$(ECHO) "$(PCX_DEP)" | $(AWK) $(AWKLINE_1) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PVROS_d: \\" >> $(OUTFILE)
	@$(ECHO) "$(PVROS_DEP)" | $(AWK) $(AWKLINE_1) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PVRD_d: \\" >> $(OUTFILE)
	@$(ECHO) "$(PVRD_DEP)" | $(AWK) $(AWKLINE_1) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$$(TMPSRC)\\sgl.cd: $$(HARDWARE_FAMILY)_c $$(OS)_c \\" >> $(OUTFILE)
	@$(ECHO) "$(SGL_PC_SRC)" | $(AWK) $(AWKLINE_6) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "	@echo C file update complete >> $$(TMPSRC)\\sgl.cd" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "DOS32_c: \\" >> $(OUTFILE)
	@$(ECHO) "$(DOS32_PC_SRC)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "WIN32_c: PVROS_c PVRD_c \\" >> $(OUTFILE)
	@$(ECHO) "$(WIN32_PC_SRC)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PCX_c: \\" >> $(OUTFILE)
	@$(ECHO) "$(PCX_PC_SRC)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PVROS_c: \\" >> $(OUTFILE)
	@$(ECHO) "$(PVROS_PC_SRC)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PVRD_c: \\" >> $(OUTFILE)
	@$(ECHO) "$(PVRD_PC_SRC)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "#Include Files " >> $(OUTFILE)
	@$(ECHO) "$$(TMPSRC)\\sgl.hd: $$(HARDWARE_FAMILY)_h $$(OS)_h \\" >> $(OUTFILE)
	@$(ECHO) "$(SGL_H)" | $(AWK)  $(AWKLINE_6) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "	@echo H file update complete >> $$(TMPSRC)\\sgl.hd" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "DOS32_h: $$(TMPSRC)\\dos32\\$(VERRC) \\" >> $(OUTFILE)
	@$(ECHO) "$(DOS32_H)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "WIN32_h: $$(TMPSRC)\\win32\\$(VERRC) \\" >> $(OUTFILE)
	@$(ECHO) "$(WIN32_H)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "PCX_h: \\" >> $(OUTFILE)
	@$(ECHO) "$(PCX_H)" | $(AWK) $(AWKLINE_5) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "second_pass:" >> $(OUTFILE)
	@$(ECHO) "	echo $$(CMDLINE) SECOND_PASS=1" >> $(OUTFILE)
	@$(ECHO) "	@$$(CMDLINE) SECOND_PASS=1" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "!endif" >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "#Dummy Targets for Auto Dependancies " >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$(SGL_OBJ)" | $(AWK)  $(AWKLINE_4) >> $(OUTFILE)
	@$(ECHO) "!if \"$$(OS)\"==\"WIN32\"" >> $(OUTFILE)
	@$(ECHO) "$(PVROS_DOBJ)" | $(AWK)  $(AWKLINE_4) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$(PVRD_DOBJ)" | $(AWK)  $(AWKLINE_4) >> $(OUTFILE)
	@$(ECHO) "" >> $(OUTFILE)
	@$(ECHO) "$(WIN32_DOBJ)" | $(AWK) $(AWKLINE_4) >> $(OUTFILE)
	@$(ECHO) "!elseif \"$$(OS)\"==\"DOS32\"" >> $(OUTFILE)
	@$(ECHO) "$(DOS32_DOBJ)" | $(AWK) $(AWKLINE_4) >> $(OUTFILE)
	@$(ECHO) "!endif" >> $(OUTFILE)
	@$(ECHO) "!if \"$$(HARDWARE_FAMILY)\"==\"PCX\"" >> $(OUTFILE)
	@$(ECHO) "$(PCX_OBJ)" | $(AWK) $(AWKLINE_4) >> $(OUTFILE)
	@$(ECHO) "!endif" >> $(OUTFILE)
	@$(ECHO) ...Done creating Dependancy file
