/****************************************************************************
Description	: Mercury main(Initial Task) module
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

Functions		:
	main()			:Main routine

----------------------------------------------------------------------------
Changes			:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.08.06	created

****************************************************************************/

#include "Basedef.h"
#include "MercuryGlobals.h"

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
#include "bsp_p1010.h"
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#ifdef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
/* global data */
AXIS_HANDLE	*AxisRscI;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

/* global functions */
#ifdef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
LONG GetAxisHndlAddress( void );
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

extern void SysInitMain( AXIS_HANDLE *AxisRscI );
extern void SysRoundTask( void );
extern void InitCorePeripheral( void );

#pragma arm section code = "MERCURY_START_CODE"	/* 暗号化フォーマット対応 <S146> */
/*******************************************************************************
*  Description : Power on Reset task
*  Parameter   : -
*  Return      : -
*******************************************************************************/
int MercuryMain( void )
{
//	ULONG i;//<S0C7>
#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	AXIS_HANDLE	*AxisRscI;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
//	__DI();
	BSP_InitTargetCPU(&SysScanATask, &SysScanBTask, &SysScanCTask);
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

	/* Initialize JL086 Peripheral */
	InitCorePeripheral();

	/* Initialize the data pointer of Servo System firmware */
	AxisRscI = InitializeAxisGlobals();

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
	/* TrigI/O変数初期化 */
	TrigIOInitialize(AxisRscI);
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */
	
	/* Mercury初期化処理メイン */
	SysInitMain( AxisRscI );

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
	BSP_EnableInterrupts();
	__EI();
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	/* ラウンドタスク生成＆起床 */
	SysRoundTask();
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

	return 0;
}


#ifdef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
LONG GetAxisHndlAddress( void )
{
	return (LONG)AxisRscI;
}
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */




/**************************** end of file **********************************/
