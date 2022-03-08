/******************************************************************************
 * Name         : parmbuff.c
 * Title        : PowerVR Parameter buffers
 * Author       : Michael Green
 * Created      : 21/04/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : Global
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: parmbuff.c,v $
 * Revision 1.4  1997/05/19  09:01:03  gdc
 * added a config.h include after changing parambuff.h
 *
 * Revision 1.3  1997/05/01  02:41:12  jop
 * Undid Ed's fixes
 *
 * Revision 1.2  1997/04/25  11:26:05  gdc
 * removed local definition of pvr param buffs a a define now
 * masks this onto the logical device
 *
 * Revision 1.1  1997/04/21  20:00:18  mjg
 * Initial revision
 *

 *****************************************************************************/
#include "config.h"
#define API_TYPESONLY
#include "sgl.h"
#include "pvrosapi.h"
#undef API_TYPESONLY
#include "parmbuff.h"


PVR_PARAM_BUFF* GetISPParamBuff( void )
{
	return(&PVRParamBuffs[PVR_PARAM_TYPE_ISP]);
}

PVR_PARAM_BUFF* GetTSPParamBuff( void )
{
	return(&PVRParamBuffs[PVR_PARAM_TYPE_TSP]);
}

PVR_PARAM_BUFF* GetRegionParamBuff( void )
{
	return(&PVRParamBuffs[PVR_PARAM_TYPE_REGION]);
}

