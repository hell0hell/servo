/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PlsIf.c : Command Pulse Interface															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function : Command Pulse Interface																*/
/*																									*/
/*			1)Initialize Pulse Interface Module														*/
/*			2)Pulse Interface ScanA Handler															*/
/*			3)Pulse Interface ScanB Handler 														*/
/*			4)Pulse Interface Parameter Calculation 												*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*																									*/
/*		Rev.1.00 : 2013.8.13	K.Ozaki																*/
/*		Rev.1.10 : 2013.10.1	K.Ozaki			<S00C>												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxPlsScanAService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
void	KpxPlsScanBService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
//void	KpxPlsScanCService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
void	KpxInitPls( void *pAxRsc ); 		/* Initialize Command Pulse Interface 					*/
/*--------------------------------------------------------------------------------------------------*/
LONG	KpiPcalPerClrMode( PLSIFV *pPls, LONG data );	/* Initial Parameter calculation			*/
void	KpiPcalEdgeClrDisTime( 							/* Perr Clear Edge Clear Disable Time		*/
			PLSIFV *pPls, 
			USHORT time, 
			ULONG cycle );	
/*--------------------------------------------------------------------------------------------------*/
void	KpiChkPerClearSignal( PLSIFV *pPls ); 	/* Perr Clear Latch Signal Detect Service			*/
/*--------------------------------------------------------------------------------------------------*/
void	KpiPlsPerClrLatchPosition( 				/* Input PERR Clear Latched Position 				*/
			PLSIFV *pPls, 						/* 			 										*/
			LONG PosFbki,						/* Feedback Position 								*/
			LONG MotAngle,						/* Motor Angle  									*/
			LONG *pClrPos );					/* Latched Position 								*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Functions : Command Pulse Interface													*/
/*--------------------------------------------------------------------------------------------------*/
static void LpxPlsServiceA( struct _PLSIFV *this );	/* ScanA command pulse handler					*/
static void LpxPlsServiceB( struct _PLSIFV *this );	/* ScanB command pulse handler					*/
/*--------------------------------------------------------------------------------------------------*/
static void	LpxSetCmdFilSelPort( struct _PLSIFV *this ); /* set command pulse filter selection port	*/

/****************************************************************************************************/
/*																									*/
/*		Variables																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define _UDL_REG_ADDR_  (ULONG)0xC0000000
#define _UDL_REG_ADDR_OFFSET_  (ULONG)0x00001000
#define AREG_LODSET_SETV			0x02C0
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define ASIC_WR( reg, value ) *(pPls->##reg) = (value)
#define ASIC_RD( reg, value ) (value) = (SHORT)*(pPls->##reg)

/*--------------------------------------------------------------------------------------------------*/
#define KPI_LATCH_PULSECNTR( ch ) { 																\
ASIC_HW_IF_STR *pAsic = (ASIC_HW_IF_STR *)(_UDL_REG_ADDR_ + (_UDL_REG_ADDR_OFFSET_ * ch)); 			\
		pAsic->AREG_LODSET = AREG_LODSET_SETV;														\
}
/*--------------------------------------------------------------------------------------------------*/
void	KpxLatchPulsCntr0( void ) { KPI_LATCH_PULSECNTR( 0 ); }	/* Latch Pulse Counter(Ch:0) */
void	KpxLatchPulsCntr1( void ) { KPI_LATCH_PULSECNTR( 1 ); }	/* Latch Pulse Counter(Ch:1) */

/*--------------------------------------------------------------------------------------------------*/
#define KPI_CHK_LATCHTIMER_CCU( ch )						/* Check Timer Latch 3u					*/\
LONG	lwk0,lwk1;											/*										*/\
ASIC_HW_IF_STR *pAsic = (ASIC_HW_IF_STR *)(_UDL_REG_ADDR_ + (_UDL_REG_ADDR_OFFSET_ * ch)); 			\
		lwk0 = (pAsic->AREG_STIMSTAT); 						/* Read Latch Status 					*/\
		lwk0 = (( lwk0 & (1<<11) ) ? (TRUE) : FALSE);		/* lwk == TRUE ? LatchComplete			*/\
		lwk1 = (pAsic->AREG_STIMLT3);						/* read Latch Timer						*/\
		lwk1 = ((lwk0) ? (lwk1) : 0 );						/* set Latch Value						*/\

#define KPI_CHK_LATCHTIMER_CCD( ch )						/* Check Timer Latch 3d					*/\
LONG	lwk0,lwk1;											/*										*/\
ASIC_HW_IF_STR *pAsic = (ASIC_HW_IF_STR *)(_UDL_REG_ADDR_ + (_UDL_REG_ADDR_OFFSET_ * ch)); 			\
		lwk0 = (pAsic->AREG_STIMSTAT); 						/* Read Latch Status 					*/\
		lwk0 = (( lwk0 & (1<<15) ) ? (TRUE) : FALSE);		/* lwk == TRUE ? LatchComplete			*/\
		lwk1 = (pAsic->AREG_STIMLT7);						/* read Latch Timer						*/\
		lwk1 = ((lwk0) ? (lwk1) : 0 );						/* set Latch Value						*/\
/*--------------------------------------------------------------------------------------------------*/
LONG	KpxChkClrLatchTimer0u( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCU( 0 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
LONG	KpxChkClrLatchTimer0d( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCD( 0 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
LONG	KpxChkClrLatchTimer1u( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCU( 1 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
LONG	KpxChkClrLatchTimer1d( USHORT *pTim ) 
{ 
		KPI_CHK_LATCHTIMER_CCD( 1 ); 
		*pTim = (SHORT)lwk1;
		return lwk0;
}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Initialize Virtual Serial Message Interface													*/
/*																									*/
/****************************************************************************************************/
void	KpxInitPls( void *pAxis )
{
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)pAxis;
PLSIFV *pPls = pAxRsc->pPlsIfV;
ASIC_HW_IF_STR *pAsic = pAxRsc->SvAsicRegs->AsicHwReg;
LONG ch = pAxRsc->AxisNo;
/*--------------------------------------------------------------------------------------------------*/
/*	JL086のPulseIF初期化は、SHal_InitAsicPulseIF( )にて実行											*/
/*--------------------------------------------------------------------------------------------------*/
	;
/*--------------------------------------------------------------------------------------------------*/
/* 	Reset Memory 																					*/
/*--------------------------------------------------------------------------------------------------*/
	/*
	 * ※ 初期パラメータ計算で書き換えるものだけ、残しておく
	 *
	 *
	 */
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( (ULONG *)(&(pPls->f)), (sizeof( PLSIFV )-sizeof( PLSIFP ))/4 );	

/*--------------------------------------------------------------------------------------------------*/
/*		Read Initial Pulse Command Counter															*/
/*--------------------------------------------------------------------------------------------------*/
	KPI_LATCH_PULSECNTR( ch );							/* Latch ASIC  Pulse Counter				*/
	pPls->PcmdHwCntr  = KPI_READ_PCMDCNTR( );			/* Read  Pulse Command Counter				*/
	pPls->dPcmdHwCntr = 0;								/* Reset Delta Pulse Command				*/	
/*--------------------------------------------------------------------------------------------------*/
	pPls->CMDLDL = &(pAsic->AREG_CMDLDL);				/* pointer to the command pulse load latch  */
	pPls->POCSET = &(pAsic->AREG_POCSET);				/* pointer to the command pulse data value  */
	pPls->CMDCLML = &(pAsic->AREG_CMDCLML);				/* pointer to the command pulse latch value */
/*--------------------------------------------------------------------------------------------------*/
	pPls->TIM = &(pAsic->AREG_TIM);						/* @@										*/
	pPls->SFBLT = &(pAsic->AREG_SFBLT4);				/* @@										*/
	pPls->MposSftR = pAxRsc->MencV->MposSftR;			/* @@										*/
	/*----------------------------------------------------------------------------------------------*/
	/*	暫定でSGDVと同じ補正値にする																*/
	/*----------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrMecha )
	{
		pAsic->AREG_ENCOFS = (ULONG)(-PFBKCOMPDELAY); /* set ENCOFS with Machine Base Timer Offset */
	}
	else
	{
		pAsic->AREG_ENCOFS = (ULONG)PFBKPOUTDELAY;  /* set ENCOFS with Controler Base Timer Offset */
	}
	/*----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*	エッジ検出メソッドの設定																		*/
/*	※先にパラメータ計算すること																	*/
/*	※エッジ検出論理はUDLの論理設定で設定する。														*/
/*--------------------------------------------------------------------------------------------------*/
	pPls->CHK_LATCHTIMER = ((ch == 0 ) ? KpxChkClrLatchTimer0u : KpxChkClrLatchTimer1u);
	pPls->CHK_LATCHTIMER( &(pPls->PerClrEdgeTime) );	/* Clear Latch Status (念のため)			*/

/*--------------------------------------------------------------------------------------------------*/
	if( ch == 0 )
	{
		pPls->LATCH_PULSCNTR = KpxLatchPulsCntr0;		/* pointer to the Function to Latch Puls	*/
	}
	else
	{
		pPls->LATCH_PULSCNTR = KpxLatchPulsCntr1;		/* pointer to the Function to Latch Puls	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	LpxSetCmdFilSelPort( pPls );						/* set hardware command pulse filter port	*/
/*--------------------------------------------------------------------------------------------------*/
	if( pAxRsc->UniPrms->Bprm->RvsDir )
	{
		pPls->RvsDir = (-1);							/* Reverse Mode */
	}
	else
	{
		pPls->RvsDir = (1);								/* Normal Mode */
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Command Pulse Manager( ScanA Cycle )														*/
/*																									*/
/****************************************************************************************************/
void	KpxPlsScanAService( PLSIFV *pPls )
{
	if(PLS_CH0_USE == 1){ LpxPlsServiceA( pPls ); }
//	if(PLS_CH1_USE == 1){ LpxPlsServiceA( &(LPlsIfV[1]) ); }
}

/****************************************************************************************************/
/*																									*/
/*		Command Pulse Manager( ScanB Cycle )														*/
/*																									*/
/****************************************************************************************************/
void	KpxPlsScanBService( PLSIFV *pPls )
{
	if(PLS_CH0_USE == 1){ LpxPlsServiceB( pPls ); }
//	if(PLS_CH1_USE == 1){ LpxPlsServiceB( &(LPlsIfV[1]) ); }
}

/****************************************************************************************************/
/*																									*/
/*		ScanA command pulse handler																	*/
/*																									*/
/****************************************************************************************************/
void	LpxPlsServiceA( struct _PLSIFV *pPls )	
{
/*--------------------------------------------------------------------------------------------------*/
/*		Read Latch Timer Current Value																*/
/*--------------------------------------------------------------------------------------------------*/
		ASIC_RD( TIM, pPls->LatchBaseTimeA );
/*--------------------------------------------------------------------------------------------------*/
/* 		Latch Asic Pulse Counter																	*/ 
/*--------------------------------------------------------------------------------------------------*/
		pPls->LATCH_PULSCNTR( );
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		ScanB command pulse handler																	*/
/*																									*/
/****************************************************************************************************/
void	LpxPlsServiceB( struct _PLSIFV *pPls )	
{
LONG	lwk0;
LONG	lwk1;
/*--------------------------------------------------------------------------------------------------*/
/*		Read Pulse Command Counter																	*/
/*--------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	前回指令位置の更新																		*/
		/*------------------------------------------------------------------------------------------*/
		if( pPls->f.PerClrEdge )							/* 偏差エッジクリアあり					*/
		{
			lwk0 = pPls->ClrPcmdPastCntr;					/* 前回指令位置を補正位置に変更			*/
			lwk1 = pPls->PcmdHwCntr;
		}
		else
		{
			lwk0 = pPls->PcmdHwCntr;						/* 前回指令位置を更新					*/
			lwk1 = pPls->PcmdHwCntr;
		}
		/*------------------------------------------------------------------------------------------*/
		/* Read and Renewal Pulse Command Counter Info. 											*/
		/*------------------------------------------------------------------------------------------*/
		pPls->PcmdHwCntr = KPI_READ_PCMDCNTR( );			/* Read Pulse Command Counter			*/
		pPls->dPcmdHwCntr = (SHORT)(pPls->PcmdHwCntr - lwk0);
		pPls->dPcmdInputHwCntr = (SHORT)(pPls->PcmdHwCntr - lwk1);
/*--------------------------------------------------------------------------------------------------*/
		pPls->PcmdRingCntr += pPls->dPcmdHwCntr;			/* Pulse Counter for Network I/F		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Pulse Command Input Signal																	*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_READ_PCMD_STATUS( lwk0 );						/* Read Pcmd Status						*/
		pPls->f.TrcPcmdInon = KPX_CHECK_PCMD_CADAT( lwk0 );	/* Trace : Pulse Command Input On		*/
		pPls->f.TrcPcmdSign = KPX_CHECK_PCMD_CBDAT( lwk0 );	/* Trace : Pulse Command Input Sign		*/
		pPls->f.PerClrSignal = KPX_CHECK_PCMD_CCDAT( lwk0 );/* Position Error Clear Signal			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Read Latch Base Timer		 																*/
/*--------------------------------------------------------------------------------------------------*/
		pPls->LatchBaseTimeB = pPls->LatchBaseTimeA;

/*--------------------------------------------------------------------------------------------------*/
/*		Check CLR Signal Edge, Read Latch Timer		 												*/
/*--------------------------------------------------------------------------------------------------*/
		pPls->f.EdgeDetected = pPls->CHK_LATCHTIMER( &(pPls->PerClrEdgeTime) );

/*--------------------------------------------------------------------------------------------------*/
/*		Check Perr Clear Signal (LTM)		 														*/
/*--------------------------------------------------------------------------------------------------*/
		KpiChkPerClearSignal( pPls );						/* check perr clear signal edge			*/

/*--------------------------------------------------------------------------------------------------*/
/*		@@ Next Command Position																	*/
/*--------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	位置指令補正演算		次スキャンの前回指令位置 -> BoutK.ClrPcmdPastCntr				*/
		/*------------------------------------------------------------------------------------------*/
		/*		■指令ﾗｯﾁ完了／未完時の処理（SGDSと同仕様にする）									*/
		/*			・完了⇒次ｽｷｬﾝの前回指令位置をエッジ入力（ラッチ）時の指令位置とする			*/
		/*			・未完⇒次ｽｷｬﾝの前回指令位置を今回ｽｷｬﾝの指令位置とする							*/
		/*------------------------------------------------------------------------------------------*/
		if( pPls->f.ClrEnableFlg )							/*	指令ラッチ完了時					*/
		{
			pPls->ClrPcmdPastCntr = pPls->ClrLatchRefPos;	/* ラッチ指令位置を前回値とする			*/
		}
		else												/* 指令ラッチ未完時						*/
		{
			pPls->ClrPcmdPastCntr = pPls->PcmdHwCntr;		/* 前回指令位置は今回指令位置とする		*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		位置偏差クリア信号監視サービス																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		<V904>  位置偏差クリアのチャタリング防止として、位置偏差クリア（エッジクリア）を実行した後	*/
/*				位置偏差クリア信号入力最小時間間隔(Pn240)経過中はクリア信号無視かつラッチ状態のクリ	*/
/*				アを行うように修正した。															*/
/*				また、経過中以外でも、「ASICのラッチ完了」&&「CPU認識無し」が２回連続した場合は、	*/
/*				ASICのラッチ完了を解除するように修正した。これは、ASICのみラッチした状態が残ると	*/
/*				次にCPUがラッチ信号を認識したときのFBカウンタと以前のラッチカウンタで位置指令が		*/
/*				生成され暴走する危険性があるためである。											*/
/****************************************************************************************************/
void	KpiChkPerClearSignal( PLSIFV *pPls )
{
LONG	lwk;
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア信号無効時間経過？															*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPls->EdgeClr_DisableCnt == 0 )
	{
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア信号無効時間経過後															*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		位置偏差クリア信号有り																	*/
	/*----------------------------------------------------------------------------------------------*/
		if( pPls->f.EdgeDetected  )							/* 割込み要求チェック					*/
		{
			KPX_READ_PCMD_STATUS( lwk );					/* AREG_POCSETの読込み					*/
			if( lwk & 0x8000 )								/* 指令カウンタのラッチ完了				*/
			{
				pPls->f.ClrEnableFlg = TRUE;				/* 指令カウンタのラッチ完了				*/
				pPls->ClrLatchRefPos = (SHORT)*(pPls->CMDCLML);/* エッジ入力時の指令カウンタ読込み	*/
			}
			else
			{
				pPls->f.ClrEnableFlg = FALSE;				/* 指令カウンタのラッチ未完				*/
				pPls->ClrLatchRefPos = 0;					/* この場合未使用：０にしておく			*/
			}
			pPls->f.PerClrEdge = TRUE;						/* 位置偏差クリアエッジ信号設定			*/
			ASIC_WR( POCSET, (lwk & 0xFEFF));				/* 指令カウンタのラッチ解除				*/
			KlibWaitus( 1 );								/* ＠暫定処置	待ち時間の確認要		*/
			ASIC_WR( POCSET, (lwk | 0x0100));				/* 指令カウンタのラッチ許可				*/
			pPls->EdgeClr_DisableCnt = pPls->P.EdgeClr_DisableTime;		/* 経過時間セット			*/
			pPls->f.EdgeClrUnmatch = FALSE;					/* 不一致状態のクリア					*/
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		位置偏差クリア信号無し																	*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			pPls->f.PerClrEdge = FALSE;						/* 位置偏差クリアエッジ信号クリア		*/
			pPls->f.ClrEnableFlg = FALSE;
			pPls->ClrLatchRefPos = 0;						/* この場合未使用：０にしておく			*/

			KPX_READ_PCMD_STATUS( lwk );					/* AREG_POCSETの読込み					*/
			if( lwk & 0x8000 )								/* 指令カウンタのラッチ完了?			*/
			{
				if( pPls->f.EdgeClrUnmatch == FALSE )		/* 前回不一致状態ではない？				*/
				{
					pPls->f.EdgeClrUnmatch = TRUE;			/* 不一致状態のセット					*/
				}
				else
				{
					ASIC_WR( POCSET, (lwk & 0xFEFF));		/* 指令カウンタのラッチ解除				*/
					KlibWaitus( 1 );						/* ＠暫定処置	待ち時間の確認要		*/
					ASIC_WR( POCSET, (lwk | 0x0100));		/* 指令カウンタのラッチ許可				*/
					pPls->f.EdgeClrUnmatch = FALSE;			/* 不一致状態のクリア					*/
				}
			}
			else
			{
				pPls->f.EdgeClrUnmatch = FALSE;				/* 不一致状態のクリア					*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア信号無効時間経過前（無効中）													*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		pPls->f.PerClrEdge = FALSE;							/* 位置偏差クリアエッジ信号クリア		*/
		pPls->f.ClrEnableFlg = FALSE;
		pPls->ClrLatchRefPos = 0;							/* この場合未使用：０にしておく			*/

		KPX_READ_PCMD_STATUS( lwk );						/* AREG_POCSETの読込み					*/
		if( lwk & 0x8000 )									/* 指令カウンタのラッチ完了?			*/
		{
			ASIC_WR( POCSET, (lwk & 0xFEFF));				/* 指令カウンタのラッチ解除				*/
			KlibWaitus( 1 );								/* ＠暫定処置	待ち時間の確認要		*/
			ASIC_WR( POCSET, (lwk | 0x0100));				/* 指令カウンタのラッチ許可				*/
		}

		pPls->f.EdgeClrUnmatch = FALSE;						/* 信号認識不一致状態のクリア			*/
		pPls->EdgeClr_DisableCnt--;							/* 経過時間のデクリメント				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*	位置偏差クリアエッジ信号設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrEdgeMode == FALSE ) { pPls->f.PerClrEdge = FALSE;	}	
/*--------------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		Initial Parameter calculation (Pn200)														*/
/*																									*/
/****************************************************************************************************/
LONG	KpiPcalPerClrMode( PLSIFV *pPls, LONG data )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.1 : クリア信号形態選択スイッチ														*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( data >> 4 ) & 0x0F )
	{
		case 0x00:	pPls->P.PerClrSigSel = 0x00; break;	/* 信号のＨｉレベルで偏差クリア				*/
		case 0x01:  pPls->P.PerClrSigSel = 0x01; break;	/* 信号の立ち上がりで偏差クリア				*/
		case 0x02:  pPls->P.PerClrSigSel = 0x02; break;	/* 信号のＬｏレベルで偏差クリア				*/
		case 0x03:  pPls->P.PerClrSigSel = 0x03; break;	/* 信号の立ち下がりで偏差クリア				*/
		default : return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.2 : クリア動作選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( data >> 8 ) & 0x0F )
	{
		case 0x00:	pPls->P.PerClrMode = 0x00; break;	/* ベースブロック時＋CLR信号で、偏差クリア	*/
		case 0x01:	pPls->P.PerClrMode = 0x01; break;	/* CLR信号でのみ偏差クリア					*/
		case 0x02:	pPls->P.PerClrMode = 0x02; break;	/* アラーム発生時＋CLR信号で、偏差クリア	*/
		case 0x03:	pPls->P.PerClrMode = 0x03; break;	/* 偏差クリアしない(CLR信号無視)			*/
		default : return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差レベルクリア選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrSigSel == 0x00 )	 	{ pPls->P.PerClrLvlSel = 0x01;}	/* High Level			*/
	else if( pPls->P.PerClrSigSel == 0x02 )	{ pPls->P.PerClrLvlSel = 0x02;}	/* Low  Level			*/
	else                                 	{ pPls->P.PerClrLvlSel = 0x00;}	/* No   Level			*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差エッジクリア選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ((pPls->P.PerClrSigSel == 0x01)||(pPls->P.PerClrSigSel == 0x03))&&(pPls->P.PerClrMode != 0x03) )
	{
		pPls->P.PerClrEdgeMode = TRUE;					/* CLR信号のエッジで、偏差クリアする		*/
	}
	else
	{
		pPls->P.PerClrEdgeMode = FALSE;					/* CLR信号のエッジで、偏差クリアしない		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.3 : フィルタ選択設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (data >> 12) & 0x0F )
	{
	case 0x00: pPls->P.CmdOpenCollector = FALSE;   break; /* ラインドライバ信号用指令フィルタ１	*/
	case 0x01: pPls->P.CmdOpenCollector = TRUE;    break; /* オープンコレクタ信号用指令フィルタ	*/
	case 0x02: pPls->P.CmdOpenCollector = FALSE;   break; /* ラインドライバ信号用指令フィルタ２	*/
	default :  return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
/*	FB位置補正演算フラグ：暫定でSGDVと同じにする。													*/
/*--------------------------------------------------------------------------------------------------*/
	pPls->P.PerClrMecha = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	return (OK);
}


/****************************************************************************************************/
/*																									*/
/*		Pn240: 位置偏差クリア信号入力最小時間間隔計算	<V904>										*/
/*			位置偏差クリア（エッジクリア）の有効エッジ認識から一定時間クリア信号を無視する			*/
/*			時間を計算する。																		*/
/*																									*/
/*		@@ parameters																				*/
/*		1) ch : Command Pulse Interface Channel Number												*/
/*		2) time : Perr Clear Disable Time [ms]														*/
/*		3) cycle : Scan cycle[us]																	*/
/*																									*/
/****************************************************************************************************/
void	KpiPcalEdgeClrDisTime( PLSIFV *pPls, USHORT time, ULONG cycle )	/* 偏差クリア無効時間計算	*/
{
		pPls->P.EdgeClr_DisableTime = (LONG)( (ULONG)time * 1000 / cycle );
		return;
}

/****************************************************************************************************/
/*																									*/
/*		偏差クリア信号ラッチ位置計算																*/
/* 		Input PERR Clear Latched Position 															*/
/*																									*/
/****************************************************************************************************/
#if 0
void	KpiPlsPerClrLatchPosition( 				/* Input PERR Clear Latched Position 				*/
			PLSIFV *pPls, 						/* Channel No. 										*/
			LONG PosFbki,						/* Feedback Position 								*/
			LONG dPosFbki,						/* Delta Position 									*/
			LONG *pClrPos )						/* Latched Position 								*/
{
LONG dFbPosCmp;
	/*----------------------------------------------------------------------------------------------*/
	/*	ＦＢ位置補正演算		エッジ入力時のＦＢ位置増分値 -> dFbPosCmp							*/
	/*----------------------------------------------------------------------------------------------*/
	if( pPls->P.PerClrMecha )							/* 機械位置基準偏差クリア					*/
	{
		dFbPosCmp = KPI_PFBKCOMPEN( dPosFbki, pPls->PerClrEdgeTime );
	}
	else												/* 上位コントローラ基準偏差クリア			*/
	{
		dFbPosCmp = KPI_PFBKCOMPENHC( dPosFbki, pPls->PerClrEdgeTime );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	@@ ＦＢ位置計算			エッジ入力時のＦＢ位置 -> BoutK.ClrFbPos							*/
	/*----------------------------------------------------------------------------------------------*/
	if ( pPls->f.EdgeDetected )
	{
		*pClrPos = PosFbki - dFbPosCmp;
//		pPls->ClrFbPosSft = PosFbki - dFbPosCmp; 		/* @@  後で消す								*/
	}
	/*----------------------------------------------------------------------------------------------*/
	return;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		偏差クリア信号ラッチ位置計算																*/
/* 		(UDLのラッチ機能を使用)																		*/
/*		TODO: リニア、フルク																		*/
/*																									*/
/****************************************************************************************************/
void	KpiPlsPerClrLatchPosition( 				/* Input PERR Clear Latched Position 				*/
			PLSIFV *pPls, 						/* Pulse Interface 									*/
			LONG PosFbki,						/* Feedback Position [pulse] 						*/
			LONG MotAngle,						/* Motor Angle [pulse] 								*/
			LONG *pClrPos )						/* Latched Position [pulse]							*/
{
LONG dFbPosCmp;
LONG LatchPos = (((LONG)*(pPls->SFBLT)) << (pPls->MposSftR));
	/*----------------------------------------------------------------------------------------------*/
	/*	ＦＢ位置補正演算		エッジ入力時のＦＢ位置増分値 -> dFbPosCmp							*/
	/*----------------------------------------------------------------------------------------------*/
	dFbPosCmp = (pPls->RvsDir) * ((MotAngle - LatchPos) >> (pPls->MposSftR));
	/*----------------------------------------------------------------------------------------------*/
	/*	@@ ＦＢ位置計算			エッジ入力時のＦＢ位置 -> BoutK.ClrFbPos							*/
	/*----------------------------------------------------------------------------------------------*/
	if ( pPls->f.EdgeDetected )
	{
		*pClrPos = PosFbki - dFbPosCmp;
		pPls->ClrFbPos = PosFbki - dFbPosCmp; /* @@ モニタ用：後で消す */ 
	}
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		パルス指令フィルタ：フィルタ選択ポートの設定												*/
/*																									*/
/****************************************************************************************************/
void	LpxSetCmdFilSelPort( PLSIFV *pPls )
{
/*--------------------------------------------------------------------------------------------------*/
/*	Iprm.f.CmdOpenCollector == TRUE (Pn200.3=1) ==> オープンコレクタ信号用指令フィルタ				*/
/*	Iprm.f.CmdOpenCollector == FALSE(Pn200.3=0) ==> ラインドライバ信号用指令フィルタ				*/
/*--------------------------------------------------------------------------------------------------*/
	if(pPls->P.CmdOpenCollector == TRUE)
	{
		KPI_PCMDFIL_ON( );		/* FILSEL ==> High Level	*/
	}
	else
	{
		KPI_PCMDFIL_OFF( );		/* FILSEL ==> Lo Level	*/
	}
}

/***************************************** end of file **********************************************/
