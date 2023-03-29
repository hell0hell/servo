/****************************************************************************************************/
/*																									*/
/*																									*/
/*		IniPrmCalc.c : 初期パラメータ計算処理定義													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 電源投入時初期パラメータ計算処理														*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "AlarmManager.h"
#include "MLib.h"
#include "MicroPrmCalc.h"
#include "SysConfig.h"
#include "BaseControlPrmCalc.h"
#include "ExCtrlPrmCalc.h"
#include "PnPrmListTbl.h"
#include "TuneLib.h"
#include "FnPrgJog.h"
#include "FnEasyFFT.h"
#include "ModelControl.h"
#include "JL086ServoIpHw.h"/* <S004> */
#include "RippleComp.h"	/* <S1FD> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void		pIprmcalMain( AXIS_HANDLE *Axis, LONG caller_id );									*/
/*	void		LpxPcalKioManager( LONG ax_no );													*/
/****************************************************************************************************/
static	void	LpxPcalSpdDetMaFilter( AXIS_HANDLE *Axis );		/* モータ速度検出用移動平均初期化	*/
static	void	IprmcalElectronicGear( AXIS_HANDLE *Axis );		/* 位置指令電子ギヤ演算				*/
static	void	IprmcalAlarmMask( AXIS_HANDLE *Axis );			/* アラームマスク設定(PnE00)		*/
static	void	IprmcalSystemSwitch( AXIS_HANDLE *Axis );		/* システムスイッチ設定(PnE01)		*/
static	void	IprmcalVdetfilSwitch( AXIS_HANDLE *Axis );		/* 電圧検出フィルタ選択SW設定(PnE7A)*/
static	void	IprmcalACInput( AXIS_HANDLE *Axis );			/* AC電源入力選択(PnE0C,Pn00B)		*/
static	void	IprmcalBitParam000( AXIS_HANDLE *Axis );		/* 機能選択基本SW(Pn000)			*/
static	void	IprmcalBitParam001( AXIS_HANDLE *Axis );		/* 機能選択応用SW(Pn001)			*/
static	void	IprmcalBitParam002( AXIS_HANDLE *Axis );		/* 機能選択応用SW(Pn002)			*/
static	void	IprmcalBitParam008( AXIS_HANDLE *Axis );		/* 機能選択応用SW(Pn008)			*/
static	void	IprmcalBitParam009( AXIS_HANDLE *Axis );		/* 機能選択応用スイッチ(Pn009)		*/
static	void	IprmcalBitParam00A( AXIS_HANDLE *Axis );		/* 機能選択応用スイッチ(Pn00A)		*/    /* <S1F5> */
static	void	IprmcalBitParam00B( AXIS_HANDLE *Axis );		/* 機能選択応用スイッチ(Pn00B)		*/
static	void	IprmcalBitParam00D( AXIS_HANDLE *Axis );		/* 機能選択応用スイッチ(Pn00D)		*/
static	void	IprmcalBitParam00E( AXIS_HANDLE *Axis );		/* 機能選択応用スイッチ(Pn00E)		*//* <S21D> */
static	void	IprmcalBitParam00F( AXIS_HANDLE *Axis );		/* 機能選択応用スイッチ(Pn00F)		*/	/* < S154 > */
static	void	IprmcalBitParam10B( AXIS_HANDLE *Axis );		/* ゲイン関係応用SW(Pn10B)			*/
static	void	IprmcalBitParam200( AXIS_HANDLE *Axis );		/* 位置制御指令形態選択SW(Pn200)	*/
static	void	IprmcalBitParam207( AXIS_HANDLE *Axis );		/* 位置制御機能SW(Pn207)			*/
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
static	void	IprmcalBitParam230( AXIS_HANDLE *Axis );		/* 位置制御拡張機能スイッチ(Pn230)	*/
#endif
//static	void	IprmcalBitParam408( AXIS_HANDLE *Axis );		/* トルク関係機能SW(Pn408)		*/
static	void	IprmcalBitVelRippleCompSW( AXIS_HANDLE *Axis );	/* 速度リップル補償機能選択SW(Pn423)*/	/* <S14B> */
static	void	IprmcalVelRippleCompSpd( AXIS_HANDLE *Axis );	/* 速度リップル補償機能選択SW(Pn423)*/	/* <S14B> */

static	void	IprmcalBitParam514( AXIS_HANDLE *Axis );		/* 出力信号選択4(Pn514)				*//* <S21D> */
static	void	IprmcalVoltageCheckLevel( AXIS_HANDLE *Axis );	/* ＤＣ電源電圧監視レベル計算		*/
static	void	IprmcalInstEnvMon( AXIS_HANDLE *Axis );			/* 設置環境モニタ用パラメータ計算	*//* <S03A> */
static	void	IprmcalOverLoadLevelAmpMot( AXIS_HANDLE *Axis );/* アンプ＆モータ過負荷レベルの計算	*/
static	void	IprmcalMotorOverrun( AXIS_HANDLE *Axis );		/* モータ暴走監視パラメータ計算		*/
static	void	IprmcalMaxBbonTimeDiff( AXIS_HANDLE *Axis );	/* HWBBタイミング異常パラメータ計算 */
static	void	IprmcalSeqInputMap( AXIS_HANDLE *Axis );		/* <S045> シーケンス入力マップ計算	*/
/* SGD7W IO入出力対応<S118> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
static	void	LpxPcalSetInputMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, 	/* <S045> シーケンス入力マップ計算（7W用）	*/
						ULONG prm, ULONG LogicH );
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
static	void	LpxPcalSetInputMap( SEQ_IOSIGNAL *IoSignal, 	/* <S045> シーケンス入力マップ計算 	*/
						ULONG prm, ULONG LogicH ); 
#endif
static	void	IprmcalSeqOutputMap( AXIS_HANDLE *Axis );		/* <S045> シーケンス出力マップ計算	*/
/* SGD7W IO入出力対応<S118> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
static  void	LpxPcalSetSoutMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, 		/* シーケンス出力マップ計算			*//* <S04B> */
						ULONG prm, ULONG OppositeReq );
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
//static  void	LpxPcalSetSoutMap( SEQ_IOSIGNAL *IoSignal, 		/* <S045> シーケンス出力マップ計算	*//* <S04B> */
//						ULONG prm ); 
static  void	LpxPcalSetSoutMap( SEQ_IOSIGNAL *IoSignal, 		/* シーケンス出力マップ計算			*//* <S04B> */
						ULONG prm, ULONG OppositeReq );
#endif

/*--------------------------------------------------------------------------------------------------*/
/* SGD7W IO入出力対応2<S1B5> 																		*/
/*--------------------------------------------------------------------------------------------------*/
static	void	LpxSetInputMapCmn( AXIS_HANDLE *Axis, LONG Mode );						/* <S1B5> */
static	void	LpxSetInputMapW( AXIS_HANDLE *Axis );									/* <S1B5> */
static  void	LpxPcalSetInputMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH );	/* <S1B5> */
static	void	LpxSetOutputMapCmn( AXIS_HANDLE *Axis );								/* <S1B5> */
static	void	LpxSetOutputMapW( AXIS_HANDLE *Axis );									/* <S1B5> */
static  void	LpxPcalSetSoutMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH );	/* <S1B5> */
static  LONG	LpxGetInSel( LONG Axis, ULONG prm );									/* <S1B5> */
static  void	LpxInitLatchSignal( AXIS_HANDLE *Axis );								/* <S1B5> */
/*--------------------------------------------------------------------------------------------------*/
static	void	IprmcalPcmdOvrSpd( AXIS_HANDLE *Axis );			/* 位置指令過速度(ＯＳ)の計算		*/
static	void	IprmcalZeroStopControl( AXIS_HANDLE *Axis );	/* ゼロ速停止制御パラメータ計算		*/
static	void	IprmcalCarrerChange( AXIS_HANDLE *Axis );		/* キャリア切り替えパラメータ計算	*/
static	void	IprmcalTorqueLevel( AXIS_HANDLE *Axis );		/* トルクレベルの計算				*/
static	void	IprmcalOtMask( AXIS_HANDLE *Axis );				/* オーバトラベルマスク暫定！！！<S032>*/
static	void	IprmcalPowerCircuitSetting( AXIS_HANDLE *Axis );/* パワー基板関連設定				*/
static	void	IprmcalCopyParamToIprm( AXIS_HANDLE *Axis );	/* コピーパラメータ(Prm --> Iprm)	*/
static	void	LpxPcalPcmpCalGain( AXIS_HANDLE	*Axis );		/* 位相補償定数の計算				*/
static	void	IprmcalVoltageCheckTime( AXIS_HANDLE *Axis );	/* DC電源電圧監視時間計算 <S053>	*/
static	void	IprmcalFanLockDetectTime( AXIS_HANDLE *Axis );	/* 内蔵ファン停止検出時間計算		*//* <S0A3> */
static	void	IprmcalSpdFbFil( AXIS_HANDLE *Axis );			/* モータ静音用速度ＦＢLPF初期設定 <S0DA>	*/
static  void	AdetCheckMotorOverSpeed(PRMDATA *Prm, BPRMDAT *Bprm, LONG AxisMotType );	/* <S1D0> */
static void	LpxMakeCarrCnt( PRMDATA *Prm, CARRCHNG	*CarrerChange );	/* キャリア周波数カウント作成処理	*/
static	void	IprmcalFanLockAlmDetectTime( AXIS_HANDLE *Axis );	/* サーボオン時内蔵ファン停止アラーム検出時間設定	*//* <S219> */


/* 外部関数 */
extern void InitInterruptStart( void );
extern LONG KpiPcalPerClrMode( PLSIFV *pPls, LONG data );

/****************************************************************************************************/
/*																									*/
/*		初期共通パラメータ計算メイン(ユニット共通)													*/
/*																									*/
/****************************************************************************************************/
void	IprmcalMainCommon( AXIS_HANDLE *Axis )
{
	IprmcalVdetfilSwitch( Axis );				/* 電圧検出フィルタ選択スイッチ設定(PnE7A)			*/
	IprmcalACInput( Axis );						/* AC電源入力選択(PnE0C,Pn00B)						*/

	IprmcalVoltageCheckLevel( Axis );			/* DC電源電圧監視レベル計算							*/
	IprmcalPowerCircuitSetting( Axis );			/* パワー基板関連設定								*/

	IprmcalVoltageCheckTime( Axis );			/* DC電源電圧監視時間計算 <S053>					*/

	IprmcalFanLockDetectTime( Axis );			/* 内蔵ファン停止検出時間計算						*//* <S0A3> */
}

/****************************************************************************************************/
/*																									*/
/*		初期共通パラメータ計算メイン（軸依存）														*/
/*																									*/
/****************************************************************************************************/
void	IprmcalMainAxis( AXIS_HANDLE *Axis )
{
	BOOL			enable;
	LONG			PrmSetting;
	TUNELESS_CFGPRM	TuneLessCfgPrm;
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM 	MfcCfgPrm;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		制御基本,単位変換パラメータ																	*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalElectronicGear( Axis );					/* 位置指令電子ギヤ演算							*/

/*--------------------------------------------------------------------------------------------------*/
/*		機能選択ビットパラメータ																	*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalAlarmMask( Axis );						/* アラームマスク設定(PnE00)					*/
	IprmcalSystemSwitch( Axis );					/* システムスイッチ設定(PnE01)					*/
//	IprmcalVdetfilSwitch( Axis );					/* 電圧検出フィルタ選択スイッチ設定(PnE7A)		*/
//	IprmcalACInput( Axis );							/* AC電源入力選択(PnE0C,Pn00B)					*/
	IprmcalBitParam000( Axis );						/* 機能選択基本スイッチ(Pn000)					*/
	IprmcalBitParam001( Axis );						/* 機能選択応用スイッチ(Pn001)					*/
	IprmcalBitParam002( Axis );						/* 機能選択応用スイッチ(Pn002)					*/
	IprmcalBitParam008( Axis );						/* 機能選択応用スイッチ(Pn008)					*/
	IprmcalBitParam009( Axis );						/* 機能選択応用スイッチ(Pn009)					*/
	IprmcalBitParam00A( Axis );						/* 機能選択応用スイッチ(Pn00A)					*/   /* <S1F5> */
	IprmcalBitParam00B( Axis );						/* 機能選択応用スイッチ(Pn00B)					*/
	IprmcalBitParam00D( Axis );						/* 機能選択応用スイッチ(Pn00D)					*/
	IprmcalBitParam00E( Axis );						/* 機能選択応用スイッチ(Pn00E)					*//* <S21D> */
	IprmcalBitParam10B( Axis );						/* ゲイン関係応用スイッチ(Pn10B)				*/
	IprmcalBitParam00F( Axis );						/* 機能選択応用スイッチ(Pn00F)					*/	/* < S154 > */
	IprmcalBitParam200( Axis );						/* 位置制御指令形態選択スイッチ(Pn200)			*/
	IprmcalBitParam207( Axis );						/* 位置制御機能スイッチ(Pn207)					*/
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
	IprmcalBitParam230( Axis );						/* 位置制御拡張機能スイッチ(Pn230)				*/
#endif
//#if( FRICTRQ_COMP != 0 )	/*<S0CD>*/
	 Axis->BaseControls->FrictionM.InitEnable = TRUE;					/* <S0CD>摩擦トルク補正機能関連パラメータ計算			*//*<S007>*/
//#endif /*<S0CD>*/
	//	IprmcalBitParam408( Axis );						/* トルク関係機能スイッチ(Pn408)			*/

	IprmcalBitVelRippleCompSW( Axis );				/* 速度リップル補償機能選択SW(Pn423)			*/	/* <S14B> */
	IprmcalVelRippleCompSpd( Axis );				/* 速度リップル補償有効速度(Pn427)				*/	/* <S14B> */
	IprmcalBitParam514( Axis );						/* 出力信号選択4(Pn514)						*//* <S21D> */

/*--------------------------------------------------------------------------------------------------*/
/*		振動検出速度オブザーバ用パラメータ計算														*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObsCalculateInitPrm( &(Axis->DetVib->DetVibObs),
							   Axis->UniPrms->Bprm->Kvx,
							   Axis->UniPrms->Prm->jrate,
							   Axis->UniPrms->Bprm->SvCycleNs );

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス機能選択																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* 調整レス機能選択スイッチ(Pn170/Pn14F) */
	TuneLessCfgPrm.gnmode	= (Axis->UniPrms->Prm)->gnmode;
	TuneLessCfgPrm.mfctype	= (Axis->UniPrms->Prm)->mfctype;
	TuneLessCfgPrm.avibsw	= (Axis->UniPrms->Prm)->avibsw;
	TuneLessCfgPrm.DatLevel	= (Axis->UniPrms->Prm)->DatLevel;
	TuneLessCfgPrm.tfuncsw 	= (Axis->UniPrms->Prm)->tfuncsw;
	TuneLessCfgPrm.flg_wf 	= (Axis->UniPrms->Prm)->MencP.flg_wf;
	PrmSetting = TuneLessCalculateInitPrm( Axis->BaseControls,
										   Axis->UniPrms->Bprm,
										   Axis->MencV,
										   Axis->SvAsicRegs,
										   &TuneLessCfgPrm );
	if( PrmSetting & PRMERR_TUNELESS_LVL )
	{
		ALMSetPramError( Axis->AlmManager, pndef_DatLevel.RegNumber );
	}
	if( PrmSetting & PRMERR_TUNELESS_TYPE )
	{
		ALMSetPramError( Axis->AlmManager, pndef_mfctype.RegNumber );
	}
	/* 調整レス用トルク補償後トルクフィルタ選択 */
	Axis->BaseControls->CtrlCmdPrm.LpassFil3 = TuneLessSetTrqFil3(
						Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse,
						Axis->BaseControls->TuneLessCtrl.conf.TuningLessEx,
						(Axis->UniPrms->Prm)->MencP.flg_wf,
						Axis->MencV->TuningLessLowGainMotor );

#if ( CSW_SVCF_MFC_USE == TRUE )
/*--------------------------------------------------------------------------------------------------*/
/*		モデル追従制御機能選択																		*/
/*--------------------------------------------------------------------------------------------------*/
	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	if( FALSE != MfcCalculateInitPrm( &(Axis->BaseControls->MFControl),
									  &MfcCfgPrm,
									  Axis->UniPrms->Prm->jrate,
									  Axis->UniPrms->Bprm,
									  Axis->UniPrms->Prm->mfctype) )
	{
		ALMSetPramError( Axis->AlmManager, pndef_mfctype.RegNumber );
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		アラーム監視関連パラメータ																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* ＤＣ電源電圧監視レベル計算 */
//	IprmcalVoltageCheckLevel( Axis );
	/* アンプ＆モータ過負荷レベルの計算 */
	IprmcalOverLoadLevelAmpMot( Axis );
	/* モータ暴走監視パラメータ計算 */
	IprmcalMotorOverrun( Axis );
	/* 入力信号タイミング異常パラメータ計算 */
	IprmcalMaxBbonTimeDiff( Axis );
	/* 設置環境モニタ用パラメータ計算 *//* <S03A> */
	IprmcalInstEnvMon( Axis );

/*--------------------------------------------------------------------------------------------------*/
/*		シーケンス入出力マップ																		*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalSeqInputMap( Axis );					/* <S045> シーケンス入力マップ計算					*/
	IprmcalSeqOutputMap( Axis );				/* <S045> シーケンス出力マップ計算					*/

/*--------------------------------------------------------------------------------------------------*/
/*		制御関連パラメータ																			*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalSpdFbFil( Axis );					/* <S0DA> モータ静音用速度ＦＢLPF初期設定		*/
	IprmcalPcmdOvrSpd( Axis );					/* 位置指令過速度(ＯＳ)の計算					*/
	IprmcalZeroStopControl( Axis );				/* ゼロ速停止制御パラメータ計算					*/

	/* 振動周波数解析初期設定 */
	FftCalculateInitPrm( Axis->FftAna, (Axis->UniPrms->Bprm)->SvCycleUs );

	/* キャリア周波数切り替えパラメータ計算 */
	IprmcalCarrerChange( Axis );

	/* 自動ノッチ用初期設定 */
	AutoNotchCalculateInitPrm( Axis->ANotchSeq,
							   Axis->ANotch,
							   Axis->MencV->AxisMotType,
							   Axis->UniPrms );

#if (CSW_SVCF_BACKLASHCOMP == TRUE)
	/* バックラッシ補正量制限値計算 */
	BlshCalcMaxBlshCompPulse( &(Axis->BaseControls->PosManager.BlshCmp),
							  &(Axis->BaseControls->PosManager.conf.Egear),
							  Axis->UniPrms->Bprm );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		プログラムJOG初期パラメータ計算																*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalPrgJog( &Axis->BaseControls->PJogHdl,
					Axis->UniPrms->Bprm,
				   &Axis->BaseControls->PosManager );

/*--------------------------------------------------------------------------------------------------*/
/*		その他																						*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalTorqueLevel( Axis );					/* トルクレベルの計算							*/
//	IprmcalVdetPrmChk( Axis );					/* 主回路検出パラメータチェック					*/
	IprmcalOtMask( Axis );						/* オーバトラベルマスク暫定！！！！kira<S032>		*/
	IprmcalCopyParamToIprm( Axis );				/* コピーパラメータ(Prm --> Iprm)				*/

	/* EasyFFT初期パラメータ計算 */
	EasyFftInitialize( Axis->EasyFFT,
					   Axis->TableRefBuffer,
					   Axis->TraceBuffer,
					   Axis->UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		Reset and Init motor speed ma filetr detas													*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSpdDetMaFilter( Axis );

/*----------------------------------------------------------------------------------------------*/
/*		PnE39 Bit2 : 主回路電圧補正機能選択															*/
/*----------------------------------------------------------------------------------------------*/
	enable = FALSE;
	if( ((Axis->UniPrms->Cprm->syssw2 >> 8) & 0x03) == VDET_AD )
	{
		enable = (0 != (Axis->UniPrms->Prm->dq_sw2 & 0x0004));
	}
	PcmKvvIniPrmCalc(&(Axis->BaseControls->MpKvvComp),
	                 Axis->UniPrms->Bprm,
	                 Axis->UniPrms->Cprm->kvvfil,
	                 Axis->UniPrms->Cprm->pnvoltgn,
	                 Axis->UniPrms->Prm->vrevgn,
	                 enable );

	/* 位相補償定数の計算 */
	LpxPcalPcmpCalGain( Axis );

/*<S1D0> START*/
/*----------------------------------------------------------------------------------------------*/
/*		モータ最高速度処理																				*/
/*----------------------------------------------------------------------------------------------*/
	AdetCheckMotorOverSpeed( Axis->UniPrms->Prm, Axis->UniPrms->Bprm, Axis->MencV->AxisMotType);
/*<S1D0> START*/

/* <S219> Start */
/*----------------------------------------------------------------------------------------------*/
/*		 PnEE6 : サーボオン時内蔵ファン停止アラーム検出時間 									*/
/*----------------------------------------------------------------------------------------------*/
	IprmcalFanLockAlmDetectTime( Axis );
/* <S219> End */


}


/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		電圧検出フィルタ選択スイッチ設定(PnE7A)														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PnE7A.Bit0,1 : 主回路電圧検出異常検出フィルタ	( 00-11 / 0us-750us , 250us刻み )			*/
/*		PnE7A.Bit2,3 : 過電圧アラーム検出フィルタ		( 00-11 / 0us-750us , 250us刻み )			*/
/*		PnE7A.Bit4,5 : 回生動作開始フィルタ				( 00-11 / 0us-750us , 250us刻み )			*/
/*		PnE7A.Bit6,7 : 回生動作終了フィルタ				( 00-11 / 0us-750us , 250us刻み )			*/
/*		PnE7A.Bit8-14: 空き																			*/
/*		PnE7A.Bit15	 : 主回路ＡＤ検出（フィルタ前）トレース選択 (=1:トレース選択0x05でトレース可能)	*/
/*						※デバッグ用																*/
/*--------------------------------------------------------------------------------------------------*/
void	IprmcalVdetfilSwitch( AXIS_HANDLE *Axis )
{
	LONG				swbuf;
	REGENE_CTRL			*RegCtrl;
	DET_DC_VOLT			*DetectDc;
	CHECK_MAIN_POWER	*PowerChecker;
	UNI_PRM_STR			*UniPrm;
	PCOV_ERR_DETECT		*PcovErrDetect;									/* <S053> */

	UniPrm = Axis->UniPrms;
	swbuf = UniPrm->Cprm->vdefilsw;
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	DetectDc = &(Axis->PowerManager->DetectDcVolt);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);
	PcovErrDetect= &(Axis->PowerManager->PcovErrDetect);				/* <S053> */

/*--------------------------------------------------------------------------------------------------*/
/*		各種フィルタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	DetectDc->conf.VdcdetAlmFilter		= swbuf & 0x0003;				/* 主回路異常検出フィルタ	*/
	PowerChecker->conf.OvdetAlmFilter	= ( swbuf >> 2 ) & 0x0003;		/* 過電圧検出フィルタ		*/
	RegCtrl->conf.RegStartFilter		= ( swbuf >> 4 ) & 0x0003;		/* 回生動作開始フィルタ		*/
	RegCtrl->conf.RegStopFilter			= ( swbuf >> 6 ) & 0x0003;		/* 回生動作終了フィルタ		*/
	

#if 0 /* 2010.06.25 Y.Oka INGRAM中間電位異常検出機能現状未対応 */
/*--------------------------------------------------------------------------------------------------*/
/*		中間電位異常検出機能有効			<V576>													*/
/*							検出時定数		<V582>													*/
/*--------------------------------------------------------------------------------------------------*/
	wk = ( swbuf >> 8 ) & 0x0f;

	if( wk == 0 )
	{
		Iprm->PcovEna = FALSE;								/* 中間電位異常検出無効					*/
	}
	else
	{
		Iprm.f.PcovEna = TRUE;								/* 中間電位異常検出有効					*/
	}
	Iprm.PcovErrCntLmt = wk * (100/KPI_SCCYCLEMS);			/* 中間電位異常検出時定数				*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		<S053> 中間電位異常検出時定数，機能有効/無効												*/
/*--------------------------------------------------------------------------------------------------*/
	PcovErrDetect->conf.PcovErrCntLmt = (( swbuf >> 8 ) & 0x0f) * (100/KPI_SCCYCLEMS);

	if( PcovErrDetect->conf.PcovErrCntLmt == 0 )
	{
		PcovErrDetect->conf.PcovEna = FALSE;				/* 中間電位異常検出無効					*/
	}
	else
	{
		PcovErrDetect->conf.PcovEna = TRUE;					/* 中間電位異常検出有効					*/
	}
	
}


/****************************************************************************************************/
/*																									*/
/*		AC電源入力選択(PnE0C,Pn00B)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0C.Bit1-2 : サーボパックの電源入力仕様													*/
/*		PnE0C.Bit3   : 電源欠相検出方式																*/
/*--------------------------------------------------------------------------------------------------*/
/*		Pn00B.2      : 三相入力仕様サーボパックの電源入力選択	 									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalACInput( AXIS_HANDLE *Axis )
{
	LONG				lwk;
	CMN_PRMDATA			*Cprm;
	PRMDATA				*Prm;
	CHECK_MAIN_POWER	*PowerChecker;
	CHECK_LOWOL			*pLowOL;		/* <S1F5> */

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;		// パラメータ設定データ
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);
	pLowOL = &(Axis->CheckAlarm->AmpLowOL);					/* <S1F5> */

/*--------------------------------------------------------------------------------------------------*/
/*	Bit 1,2: AC電源入力選択																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Cprm->syssw2 >> 1) & 0x03 )
	{
		case 0x00:											/* 単相電源入力のみ可能					*/
			PowerChecker->conf.Ac3Phase = FALSE;
			pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
			break;
		case 0x01:											/* 三相電源入力のみ可能					*/
			PowerChecker->conf.Ac3Phase = TRUE;
			pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
			break;
			/* <S1F5> Start */
		case 0x02:
			lwk = (Prm->b_prmB>>8) & 0x0F;
			if( lwk <= 1 )
			{
				if(lwk == 0)
				{
					PowerChecker->conf.Ac3Phase = TRUE;		/* 三相電源入力で使用する				*/
					pLowOL->conf.BaseCurrentDerateEnable = FALSE;
				}
				else
				{
					PowerChecker->conf.Ac3Phase = FALSE;	/* 単相電力入力で使用する				*/
					/* 単相電源入力選択時のベース電流ディレーティングあり */
					pLowOL->conf.BaseCurrentDerateEnable = TRUE;
				}
			}
			else
			{
				PowerChecker->conf.Ac3Phase = TRUE;
				pLowOL->conf.BaseCurrentDerateEnable = FALSE;
				ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
			}
			break;
			/* <S1F5> End */
		case 0x03:											/* 三相入力仕様で単相電源入力も可能		*/
			lwk = (Prm->b_prmB>>8) & 0x0F;
			if( lwk <= 1 )
			{
				if(lwk == 0)
				{
					PowerChecker->conf.Ac3Phase = TRUE;		/* 三相電源入力で使用する				*/
					pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
				}
				else
				{
					PowerChecker->conf.Ac3Phase = FALSE;	/* 単相電力入力で使用する				*/
					pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
				}
			}
			else
			{
				pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
				PowerChecker->conf.Ac3Phase = TRUE;
				ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
			}
			break;
		default:											/* 予約									*/
			pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
			PowerChecker->conf.Ac3Phase = FALSE;
			ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Bit 3: 電源欠相検出方式																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Cprm->syssw2 & 0x0008 )
	{ /* PnE0C.bit3で選択 */
		PowerChecker->conf.DetectAcOn2	= TRUE;
	}
	else
	{
		PowerChecker->conf.DetectAcOn2	= FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	電源欠相検出方式:ACON1/ACON2の場合、3相入力仕様サーボパックでないならパラメータ設定異常		*/
	/*----------------------------------------------------------------------------------------------*/
	if( PowerChecker->conf.DetectAcOn2 )
	{
		if( ((Cprm->syssw2 >> 1) & 0x01) == 0 )							/* PnE0C.bit1直接参照に変更	*/
		{
			ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
		}
	}
}


/****************************************************************************************************/
/*																									*/
/*		機能選択基本スイッチ(Pn000)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn000.0 : 回転方向選択																		*/
/*		Pn000.1 : 制御方式選択																		*/
/*		Pn000.2 : 軸アドレス																		*/
/*		Pn000.3 : 回転型、リニア型選択	2009.09.17 Y.Oka											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam000( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	Prm  = Axis->UniPrms->Prm;		// パラメータ設定データ
	Bprm = Axis->UniPrms->Bprm;		// 初期パラメータ定義(電源再投入で変更有効)

/*--------------------------------------------------------------------------------------------------*/
/*		Pn000.0 : 回転方向選択(逆回転モード)														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->b_prm0 & 0x0F )
	{
	case 0 :
		Bprm->RvsDir = FALSE;
		Bprm->DirSign = +1;
		break;
	case 1 :
		Bprm->RvsDir = TRUE;
		Bprm->DirSign = -1;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prm0.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn000.1 : 制御方式(制御モード)選択															*/
/*--------------------------------------------------------------------------------------------------*/
// M3より設定されるので削除

/*--------------------------------------------------------------------------------------------------*/
/*		Pn000.3 : 回転型、リニア型選択	2009.09.17 Y.Oka											*/
/*--------------------------------------------------------------------------------------------------*/
// イニシャル処理にて設定

}


/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn001)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn001.0 : サーボオフ及びアラーム発生時の停止方法											*/
/*		Pn001.1 : オーバとラベル(ＯＴ)時の停止方法													*/
/*		Pn001.2 : ＡＣ／ＤＣ電源入力の選択															*/
/*		Pn001.3 : ワーニングコード出力の選択														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam001( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	CHECK_MAIN_POWER	*PowerChecker;
	ALM_STOP			*AlmStop;
	OT_SEQ_DATA			*OtSeqData;

	Prm  = Axis->UniPrms->Prm;		// パラメータ設定データ
	Bprm = Axis->UniPrms->Bprm;		// 初期パラメータ定義(電源再投入で変更有効)
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);
	AlmStop = &(Axis->BeSequence->AlmStop);
	OtSeqData = &(Axis->BeSequence->OtSeqData);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.0 : サーボオフ及びアラーム発生時の停止方法											*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->b_prm1 & 0x0F )
	{
		case 0x00: AlmStop->conf.StopMode = STOPMODE_DBSTOPDB; break;
		case 0x01: AlmStop->conf.StopMode = STOPMODE_DBSTOPFR; break;
		case 0x02: AlmStop->conf.StopMode = STOPMODE_FREERUN;  break;
		default:   ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber ); break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.1 : オーバトラベル(OT)時の停止方法													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Prm->b_prm1>>4) & 0x0F )
	{
		case 0x00: OtSeqData->conf.OtStopMode = OTSTOPMODE_DEFAULT; break;
		case 0x01: OtSeqData->conf.OtStopMode = OTSTOPMODE_ZSTOPZC; break;
		case 0x02: OtSeqData->conf.OtStopMode = OTSTOPMODE_ZSTOPFR; break;
		case 0x03: OtSeqData->conf.OtStopMode = OTSTOPMODE_LDSTOPZC; break;    /* <S1F5> */
		case 0x04: OtSeqData->conf.OtStopMode = OTSTOPMODE_LDSTOPFR; break;    /* <S1F5> */
		default:   ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber ); break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.2 : AC/DC電源入力の選択																*/
/*--------------------------------------------------------------------------------------------------*/
/* 2013.01.07 Y.Oka 以下コンパイルSW不要 */
//#if( SVFSEL_DCBUS == TRUE )	/* 強制的にDC電源入力モードとする場合 */
	switch( (Prm->b_prm1>>8) & 0x0F )
	{
	case 0x00: /* AC電源入力選択 */
		PowerChecker->conf.Dcbus = FALSE;								/* AC電源入力				*/
		break;

	case 0x01: /* ＤＣ電源入力選択	*/
		if( Bprm->AmpInVcode == 0x00 )
		{ /* 倍電圧仕様、100Ｖ入力仕様ならばDC電源入力選択不可 */
			PowerChecker->conf.Dcbus = FALSE;
		}
		else
		{
			PowerChecker->conf.Dcbus = TRUE;							/* DC電源入力				*/
		}
		break;
	default:
		PowerChecker->conf.Dcbus = FALSE;
		break;
	}
//#endif
/* 2013.01.07 Y.Oka 以下コンパイルSW不要 */

//#if 0	/* <S1A8> : 「Pn001.3 : ワーニングコード出力の選択」復帰 */
/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.3 : ワーニングコード出力の選択														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Prm->b_prm1>>12) & 0x0F )
	{
//		case 0x00: Iprm->WrnOutSel = FALSE; break;			/* <S1A8> */
//		case 0x01: Iprm->WrnOutSel = TRUE;  break;			/* <S1A8> */
		case 0x00: Axis->AlmManager->WrnOutSel = FALSE; break;	/* <S1A8> */
		case 0x01: Axis->AlmManager->WrnOutSel = TRUE;  break;	/* <S1A8> */
		default:   ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber ); break;
	}
//#endif	/* <S1A8> : 「Pn001.3 : ワーニングコード出力の選択」復帰 */
}


/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn002)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn002.0 : 速度制御オプション(T-REF割り当て) 		 										*/
/*		Pn002.1 : トルク制御オプション(V-REF割り当て)												*/
/*		Pn002.2 : 絶対値エンコーダの使用方法														*/
/*		Pn002.3 : フルクローズドエンコーダの使用方法												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam002( AXIS_HANDLE *Axis )
{
	TRQ_LMT_DATA	*TrqLimits;
	PRMDATA			*Prm;

	TrqLimits = &(Axis->BaseControls->TrqLimitData);
	Prm  = Axis->UniPrms->Prm;		// パラメータ設定データ

/*--------------------------------------------------------------------------------------------------*/
/*		Pn002.0 : 速度制御オプション(T-REF割り当て) 		 										*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->b_prm2 & 0x0F )
	{
	case 0x00:	/* なし							*/
//		TrqLimits->conf.TrefTrqFF    = FALSE;
		TrqLimits->conf.TrefTrqLmt   = FALSE;
		TrqLimits->conf.TrefTrqLmtCL = FALSE;
		break;
	case 0x01:	/* 外部トルク制限入力として使用 */
//		TrqLimits->conf.TrefTrqFF    = FALSE;
		TrqLimits->conf.TrefTrqLmt   = TRUE;
		TrqLimits->conf.TrefTrqLmtCL = FALSE;
		break;
	case 0x02:	/* トルクフィードフォワード入力として使用 */
//		TrqLimits->conf.TrefTrqFF    = TRUE;
		TrqLimits->conf.TrefTrqLmt   = FALSE;
		TrqLimits->conf.TrefTrqLmtCL = FALSE;
		break;
	case 0x03:	/* 外部トルク制限入力(制限信号(P-CL,N-CL)有効時)として使用 */
//		TrqLimits->conf.TrefTrqFF    = FALSE;
		TrqLimits->conf.TrefTrqLmt   = FALSE;
		TrqLimits->conf.TrefTrqLmtCL = TRUE;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prm2.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn002.1 : トルク制御オプション(V-REF割り当て)												*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Prm->b_prm2>>4) & 0x0F )
	{
	case 0x00:	/* なし							*/
		Axis->BaseControls->CtrlCmdPrm.VrefSpdLmt = FALSE;
		break;
	case 0x01:	/* 外部速度制限入力として使用 */
		Axis->BaseControls->CtrlCmdPrm.VrefSpdLmt = TRUE;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prm2.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn002.2 : 絶対値エンコーダの使用方法			--> Sencif.c								*/
/*		Pn002.3 : フルクローズドエンコーダの使用方法	--> Fencif.c								*/
/*--------------------------------------------------------------------------------------------------*/

}


/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn008)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn008.0 : バッテリ電圧低下のアラーム／ワーニング選択										*/
/*		Pn008.1 : 									 		 										*/
/*		Pn008.2 : ワーニング検出選択				 		 										*/
/*		Pn008.3 : 																					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam008( AXIS_HANDLE *Axis )
{
	UV_TLIM_DATA	*UvTrqLimits;
//	IPRMDAT			*Iprm;
//	CMN_PRMDATA		*Cprm;
	PRMDATA			*Prm;
	ULONG			uswk;			/* <S0AA> */

	UvTrqLimits = &(Axis->BaseControls->TrqLimitData.UvTrqLimits);
//	Cprm = Axis->UniPrms->Cprm;		// パラメータ設定データ
	Prm  = Axis->UniPrms->Prm;		// パラメータ設定データ
//	Iprm = Axis->prm_hndl_ptr->iprm_ptr;		// 初期パラメータ定義(電源再投入で変更有効)

	/*--------------------------------------------------------------------------------------------------*/
	/*		バッテリ電圧低下のアラーム／ワーニング選択													*/
	/*--------------------------------------------------------------------------------------------------*/
//	switch( Cprm->b_prm8 & 0x0F )
	switch( Prm->b_prm8 & 0x0F )
	{
	case 0x00:	/* バッテリ電圧低下をアラームとする		*/
		Axis->MencV->BatWrnMode = FALSE;
		break;
	case 0x01:	/* バッテリ電圧低下をワーニングとする	*/
		Axis->MencV->BatWrnMode = TRUE;
		break;
	default:	/* パラメータ設定異常					*/
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		主回路電圧降下時の機能選択																	*//* <V200> */
	/*--------------------------------------------------------------------------------------------------*/
//	switch(( Cprm->b_prm8 >> 4 ) & 0x0F )
	switch(( Prm->b_prm8 >> 4 ) & 0x0F )
	{
	case 0x00:	/* 主回路電圧降下ワーニングを検出しない */
		UvTrqLimits->conf.UvTrqLmtCtrl = FALSE;
		ALMSetMask( Axis->AlmManager, WRN_UV, TRUE );	/* A.971 : 不足電圧ワーニング		*/
		break;
	case 0x01:	/* 主回路電圧降下ワーニングを検出する */
		UvTrqLimits->conf.UvTrqLmtCtrl = FALSE;
		ALMSetMask( Axis->AlmManager, WRN_UV, FALSE );	/* A.971 : 不足電圧ワーニング		*/
		break;
	case 0x02:	/* 主回路電圧降下ワーニングを検出し、Pn424,Pn425によるトルク制限をおこなう */
		UvTrqLimits->conf.UvTrqLmtCtrl = TRUE;
		ALMSetMask( Axis->AlmManager, WRN_UV, FALSE );	/* A.971 : 不足電圧ワーニング		*/
		break;
	default:	/* パラメータ設定異常					*/
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		ワーニング検出選択																			*/
	/*--------------------------------------------------------------------------------------------------*/
//	switch( (Cprm->b_prm8>>8) & 0x0F )
	switch( (Prm->b_prm8>>8) & 0x0F )
	{
	case 0x00:	/* ワーニングを検出する		*/
		ALMSetMask( Axis->AlmManager, WRN_OLF,      FALSE );/* A.910 : 過負荷ワーニング					*/
		ALMSetMask( Axis->AlmManager, WRN_VIB,      FALSE );/* A.911 : 発振ワーニング						*/
		ALMSetMask( Axis->AlmManager, WRN_CTRLBRD,  FALSE );/* A.912 : 内部温度警告1（制御基板温度警告）*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_PWRBRD,   FALSE );/* A.913 : 内部温度警告2（パワー基板温度警）*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_RGOLF,    FALSE );/* A.920 : 回生過負荷ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_DATA,     FALSE );/* A.940 : データ異常ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_CMD,      FALSE );/* A.950 : コマンド異常ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_OPT,      FALSE );/* A.9F0 : オプション異常ワーニング			*/
		ALMSetMask( Axis->AlmManager, WRN_PGBAT,    FALSE );/* A.930 : 絶対値エンコーダバッテリ異常		*/
		ALMSetMask( Axis->AlmManager, WRN_FPGBAT,   FALSE );/* A.930 : ＦＰＧバッテリ異常ワーニング		*/
		ALMSetMask( Axis->AlmManager, WRN_OF,       FALSE );/* A.900 : 位置偏差過大ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_BEOF,     FALSE );/* A.901 : サーボオン時位置偏差過大			*/
		ALMSetMask( Axis->AlmManager, WRN_BOOT,     FALSE );/* A.941 : 電源断要ユーザ定数変更ワーニング	*/
		ALMSetMask( Axis->AlmManager, WRN_DBOL_LVL, FALSE );/* A.921 : ＤＢ過負荷ワーニング				*/
//		ALMSetMask( Axis->AlmManager, WRN_RIPPLE,   FALSE );/* A.942 : リップル補正情報不一致ワーニング	*//* <S1FD>削除 */
		/* ---futami---> */
		ALMSetMask( Axis->AlmManager, WRN_MBOH,     FALSE );/* A.981 : ユニット内部温度過熱ワーニング	*/
		ALMSetMask( Axis->AlmManager, WRN_CONVOH,   FALSE );/* A.982 : コンバータ温度過熱ワーニング		*/
		ALMSetMask( Axis->AlmManager, WRN_IFOH,     FALSE );/* A.983 : 制御基板温度過熱					*/
		/* <---futami--- */
		ALMSetMask( Axis->AlmManager, WRN_PREVENTMAINT, FALSE );/* A.9B0:予防保全ワーニング				*/ /* < S154 > */
		break;
	case 0x01:	/* ワーニングを検出しない	*/
		ALMSetMask( Axis->AlmManager, WRN_OLF,      TRUE  );/* A.910 : 過負荷ワーニング					*/
		ALMSetMask( Axis->AlmManager, WRN_VIB,      TRUE  );/* A.911 : 発振ワーニング						*/
		ALMSetMask( Axis->AlmManager, WRN_CTRLBRD,  TRUE  );/* A.912 : 内部温度警告1（制御基板温度警告）*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_PWRBRD,   TRUE  );/* A.913 : 内部温度警告2（パワー基板温度警）*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_RGOLF,    TRUE  );/* A.920 : 回生過負荷ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_DATA,     TRUE  );/* A.940 : データ異常ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_CMD,      TRUE  );/* A.950 : コマンド異常ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_OPT,      TRUE  );/* A.9F0 : オプション異常ワーニング			*/
		ALMSetMask( Axis->AlmManager, WRN_PGBAT,    TRUE  );/* A.930 : 絶対値エンコーダバッテリ異常		*/
		ALMSetMask( Axis->AlmManager, WRN_FPGBAT,   TRUE  );/* A.930 : ＦＰＧバッテリ異常ワーニング		*/
		ALMSetMask( Axis->AlmManager, WRN_OF,       TRUE  );/* A.900 : 位置偏差過大ワーニング				*/
		ALMSetMask( Axis->AlmManager, WRN_BEOF,     TRUE  );/* A.901 : サーボオン時位置偏差過大			*/
		ALMSetMask( Axis->AlmManager, WRN_BOOT,     TRUE  );/* A.941 : 電源断要ユーザ定数変更ワーニング	*/
		ALMSetMask( Axis->AlmManager, WRN_DBOL_LVL, TRUE  );/* A.921 : ＤＢ過負荷ワーニング				*/
//		ALMSetMask( Axis->AlmManager, WRN_RIPPLE,   TRUE  );/* A.942 : リップル補正情報不一致ワーニング	*//* <S1FD>削除 */
		/* ---futami---> */
		ALMSetMask( Axis->AlmManager, WRN_MBOH,     TRUE  );/* A.981 : ユニット内部温度過熱ワーニング	*/
		ALMSetMask( Axis->AlmManager, WRN_CONVOH,   TRUE  );/* A.982 : コンバータ温度過熱ワーニング		*/
		ALMSetMask( Axis->AlmManager, WRN_IFOH,     TRUE  );/* A.983 : 制御基板温度過熱					*/
		/* <---futami--- */
		ALMSetMask( Axis->AlmManager, WRN_PREVENTMAINT,   TRUE  );/* A.9B0:予防保全ワーニング			*/ /* < S154 > */
		break;
	default:	/* パラメータ設定異常		*/
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		ワーニング検出マスク	<S0AA> SGDVと同様の処理に変更。				 						*/
	/*--------------------------------------------------------------------------------------------------*/
//<S0AA>	ALMSetMask( Axis->AlmManager, WRN_BOOT, TRUE );			/* A.941 : 電源断要ユーザ定数変更ワーニング	*/
	uswk = (Prm->b_prm8>>12) & 0x0F;
	if( uswk <= 7 )
	{
		if( uswk & 0x01 )
		{
			ALMSetMask( Axis->AlmManager, WRN_BEOF, TRUE  ); 	/* A.901 : サーボオン時位置偏差過大		*/
		}
		if( uswk & 0x02 )
		{
			ALMSetMask( Axis->AlmManager, WRN_VIB, TRUE  ); 	/* A.911 : 発振ワーニング				*/
		}
		if( uswk & 0x04 )
		{
			ALMSetMask( Axis->AlmManager, WRN_BOOT, TRUE );		/* A.941 : 電源断要ユーザ定数変更ワーニング		*/
		}
	}
	else
	{
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
	}
	/*--------------------------------------------------------------------------------------------------*/

	/*--------------------------------------------------------------------------------------------------*/
	/*		モータレステストモードアラームマスク	<S1B7>						 						*/
	/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )	/* モータレステスト時の処理					*/
	{
		ALMSetMask(Axis->AlmManager, WRN_DBOL_LVL, TRUE);	/* A.921 : ＤＢ過負荷ワーニング				*/
	}
	/* <S21D> Start */
	/*--------------------------------------------------------------------------------------------------*/
	/*		ワーニング検出マスク	A.924マスクしない													*/
	/*--------------------------------------------------------------------------------------------------*/
	ALMSetMask( Axis->AlmManager, WRN_EXREG, FALSE );		/* A.924 : 回生警告							*/
	/* <S21D> End */
#if defined(CSW_NETIF_SYNQNET_TYPE)
	/*--------------------------------------------------------------------------------------------------*/
	/*		過速度検出周期設定		SGDZ-BS61の仕様に合わせる											*/
	/*--------------------------------------------------------------------------------------------------*/
//	uswk = ( Cprm->b_prm8 >> 12 ) & 0x0F;
	uswk = ( Prm->b_prm8 >> 12 ) & 0x0F;
	if( uswk == 0 )
	{
		Axis->UniPrms->Bprm->OSCountMax = 32;				/* 2.000ms周期 = 62.5us×32					*/
	}
	else
	{
		Axis->UniPrms->Bprm->OSCountMax = uswk * 4 + 2;		/* 3.125ms周期 = 62.5us×(12×4 + 2)		*/
	}
#else
	Axis->UniPrms->Bprm->OSCountMax = 0;					/* 2.000ms周期 <S0E5> 						*/
#endif

}


/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn009)		<V259>														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn009.0 : 予約定数（静音モード選択）	※ΣＶでは機能なし									*/
/*		Pn009.1 : 電流制御モード選択			--> IprmcalCarrerChange()							*/
/*		Pn009.2 : 速度検出移動平均フィルタ選択	 													*/
/*		Pn009.3 : 予約定数																			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam009( AXIS_HANDLE *Axis )
{
	UNI_PRM_STR		*UniPrm;
	UniPrm = Axis->UniPrms;
/*--------------------------------------------------------------------------------------------------*/
/*		速度検出移動平均フィルタ選択																*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( UniPrm->Prm->b_prm9 >> 8 ) & 0x0F )
	{
		case 0x00:	/* 速度検出移動平均フィルタ無効(エンコーダ分解能によって移動平均回数が決まる)	*/
					UniPrm->Bprm->SpdDetMaFil = FALSE;
					break;

		case 0x01:	/* 速度検出移動平均フィルタ有効(エンコーダ分解能によらず移動平均回数16回)		*/
					UniPrm->Bprm->SpdDetMaFil = TRUE;
					break;

		default:	/* パラメータ設定異常		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prm9.RegNumber );
					break;
	}

}

/* <S1F5> Start */
/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn00A)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn00A.0 : Gr.2アラーム発生時の停止方法選択													*/
/*		Pn00A.1 : 強制停止時の停止方法選択				 		 									*/
/*		Pn00A.2 : 予約パラメータ							 										*/
/*		Pn00A.3 : 予約パラメータ																	*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam00A( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	ALM_STOP	*AlmStop;
	FORCE_STOP	*ForceStop;

	Prm = Axis->UniPrms->Prm;		// パラメータ設定データ
	AlmStop = &(Axis->BeSequence->AlmStop);
	ForceStop = &(Axis->BeSequence->ForceStop);

/*--------------------------------------------------------------------------------------------------*/
/*		Ｇ２アラームの停止方法選択																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmB >> 4 ) & 0x0F )
	{
		case 0x00:	/* Pn406の設定トルクを最大値としてモータを減速停止								*/
					switch( Prm->b_prm1 & 0x0F )
					{
						case 0x00:	/* Pn406の設定トルクを最大値としてモータを減速停止＆DB停止状態	*/
									AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPDB ;
									break;
						case 0x01:
						case 0x02:	/* Pn406の設定トルクを最大値としてモータを減速停止＆フリーラン状態	*/
									AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPFR ;
									break;
						default:	/* パラメータ設定異常		*/
									ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
									break;
					}
					break;

		case 0x01:	/* ＤＢ停止またはフリーラン停止する(Pn001.0と同じ停止方法)						*/
					AlmStop->conf.G2AlmStopMode = GR2STOPMODE_DEFAULT ;
					break;

		case 0x02:
				switch( Prm->b_prmA & 0x0F )
				{
					case 0x00:	/* ＤＢ停止またはフリーラン停止する(Pn001.0と同じ停止方法)						*/
								AlmStop->conf.G2AlmStopMode = GR2STOPMODE_DEFAULT ;
								break;

					case 0x01:	/* Pn406の設定トルクを最大値としてモータを減速停止								*/
								switch( Prm->b_prm1 & 0x0F )
								{
									case 0x00:	/* Pn406の設定トルクを最大値としてモータを減速停止＆DB停止状態	*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPDB ;
												break;
									case 0x01:
									case 0x02:	/* Pn406の設定トルクを最大値としてモータを減速停止＆フリーラン状態	*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPFR ;
												break;
									default:	/* パラメータ設定異常		*/
												ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
												break;
								}
								break;

					case 0x02:	/* Pn406の設定トルクを最大値としてモータを減速停止＆フリーラン状態				*/
								AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPFR ;
								break;

					case 0x03:	/* Pn30Aに従ってモータを減速停止												*/
								switch( Prm->b_prm1 & 0x0F )
								{
									case 0x00:	/* Pn30Aに従ってモータを減速停止＆DB停止状態					*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_LDSTOPDB ;
												break;
									case 0x01:
									case 0x02:	/* Pn30Aに従ってモータを減速停止＆フリーラン状態				*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_LDSTOPFR ;
												break;
									default:	/* パラメータ設定異常		*/
												ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
												break;
								}
								break;

					case 0x04:	/* Pn30Aに従ってモータを減速停止＆フリーラン状態								*/
								AlmStop->conf.G2AlmStopMode = GR2STOPMODE_LDSTOPFR ;
								break;

					default:	/* パラメータ設定異常		*/
								ALMSetPramError( Axis->AlmManager, pndef_b_prmA.RegNumber );
								break;
				}
				break;
		default:	/* パラメータ設定異常		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
					break;

	}

/*--------------------------------------------------------------------------------------------------*/
/*		強制停止時の停止方法選択																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmA >> 4 ) & 0x0F )
	{
		case 0x00:	/* ＤＢ停止またはフリーラン停止する(Pn001.0と同じ停止方法)						*/
					ForceStop->conf.ForceStopMode = FSTOPMODE_DEFAULT ;
					break;

		case 0x01:	/* Pn406の設定トルクを最大値としてモータを減速停止								*/
					switch( Prm->b_prm1 & 0x0F )
					{
						case 0x00:	/* Pn406の設定トルクを最大値としてモータを減速停止＆DB停止状態	*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_ZSTOPDB ;
									break;
						case 0x01:
						case 0x02:	/* Pn406の設定トルクを最大値としてモータを減速停止＆フリーラン状態	*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_ZSTOPFR ;
									break;
						default:	/* パラメータ設定異常		*/
									ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
									break;
					}
					break;

		case 0x02:	/* Pn406の設定トルクを最大値としてモータを減速停止＆フリーラン状態				*/
					ForceStop->conf.ForceStopMode = FSTOPMODE_ZSTOPFR ;
					break;

		case 0x03:	/* Pn30Aに従ってモータを減速停止												*/
					switch( Prm->b_prm1 & 0x0F )
					{
						case 0x00:	/* Pn30Aに従ってモータを減速停止＆DB停止状態					*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_LDSTOPDB ;
									break;
						case 0x01:
						case 0x02:	/* Pn30Aに従ってモータを減速停止＆フリーラン状態				*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_LDSTOPFR ;
									break;
						default:	/* パラメータ設定異常		*/
									ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
									break;
					}
					break;

		case 0x04:	/* Pn30Aに従ってモータを減速停止＆フリーラン状態								*/
					ForceStop->conf.ForceStopMode = FSTOPMODE_LDSTOPFR ;
					break;

		default:	/* パラメータ設定異常		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prmA.RegNumber );
					break;
	}
}
/* <S1F5> End */

/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn00B)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn00B.0 : オペレータのユーザー定数表示選択													*/
/*		Pn00B.1 : Ｇ２アラームの停止方法選択			 		 									*/
/*		Pn00B.2 : 三相入力仕様サーボパックの電源入力選択	 										*/
/*		Pn00B.3 : 予約定数（セミクローズエンコーダ使用方法）										*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam00B( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
//	ALM_STOP	*AlmStop;

	Prm = Axis->UniPrms->Prm;		// パラメータ設定データ
//	AlmStop = &(Axis->BeSequence->AlmStop);

/* <S113> Start*/
/*--------------------------------------------------------------------------------------------------*/
/*	Bit 0: オペレータのユーザー定数表示選択															*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Prm->b_prmB & 0x000F) > 2 )
		{
			/* パラメータ設定異常 */
			ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
			
		}
/* <S113> End */
#if 0	/* <S1F5> Start	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Ｇ２アラームの停止方法選択																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmB >> 4 ) & 0x0F )
	{
		case 0x00:	/* ゼロ速停止する										*/
					AlmStop->conf.G2AlmStopMode = FALSE ;
					break;

		case 0x01:	/* ＤＢ停止またはフリーラン停止する(Pn001.0と同じ停止方法)	*/
					AlmStop->conf.G2AlmStopMode = TRUE ;
					break;

		default:	/* パラメータ設定異常		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
					break;
	}
#endif /* <S1F5> End	*/
}


/****************************************************************************************************/
/*																									*/
/*		機能選択応用スイッチ(Pn00D)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn00B.3 : オーバートラベルワーニング検出選択												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam00D( AXIS_HANDLE *Axis )
{
	PRMDATA			*Prm;
	OT_SEQ_DATA		*OverTravelSeq;

	Prm = Axis->UniPrms->Prm;
	OverTravelSeq = &(Axis->BeSequence->OtSeqData);

/*--------------------------------------------------------------------------------------------------*/
/*		オーバートラベルワーニング検出選択															*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmD >> 12 ) & 0x0F )
	{
		case 0x00:	/* OTワーニングを検出しない */
			OverTravelSeq->conf.OtWarningSw = FALSE;
			break;

		case 0x01:	/* OTワーニングを検出する */
			OverTravelSeq->conf.OtWarningSw = TRUE;
			break;

		default:	/* パラメータ設定異常 */
			ALMSetPramError( Axis->AlmManager, pndef_b_prmD.RegNumber );
			break;
	}
}
/* <S21D> Start */
/*****************************************************************************
  Description: 機能選択応用スイッチ(Pn00E)
------------------------------------------------------------------------------
  Parameters:
	Axis;		Pointer to Axis Handle
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static	void	IprmcalBitParam00E( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	BPRMDAT				*Bprm;
	CHECK_MAIN_POWER	*PowerChecker;
	REGENE_CTRL			*RegCtrl;

	Cprm  = Axis->UniPrms->Cprm;	/* ユニット共通パラメータ設定データ */
	Bprm = Axis->UniPrms->Bprm;		/* 初期パラメータ定義(電源再投入で変更有効) */
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn00E.1 : DC電源入力時の急速放電選択														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Cprm->b_prmE >> 4) & 0x0F )
	{
	case 0x00: /* DC電源入力時は急速放電を行いません */
		PowerChecker->conf.DcDischarge = FALSE;
		break;

	case 0x01: /* DC電源入力時、コンバータレディ信号の状態に応じて急速放電を行います */
		if( PowerChecker->conf.Dcbus == TRUE )
		{
			PowerChecker->conf.DcDischarge = TRUE;
		}
		else
		{
			PowerChecker->conf.DcDischarge = FALSE;
		}
		break;

	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prmE.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn00E.3 : 回生同期機能の使用方法															*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Cprm->b_prmE >> 12) & 0x0F )
	{
	case 0x00: /* 回生同期機能を使用しません */
		RegCtrl->conf.RegSyn = REGSYNOFF;
		break;

	case 0x01: /* PN母線共通システムにおいて回生同期機能を使用します */
		RegCtrl->conf.RegSyn = PNREGSYN;
		break;

	case 0x02: /* PN母線共通システムにて外部回生機能を利用します */
		RegCtrl->conf.RegSyn = PNEXREG;
		break;

	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prmE.RegNumber );
		break;
	}
}
/* <S21D> End */

/* < S154 > Start */
/*****************************************************************************
  Description: 機能選択応用スイッチ(Pn00F)
------------------------------------------------------------------------------
  Parameters:
	Axis;		Pointer to Axis Handle
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static	void	IprmcalBitParam00F( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA		*Cprm;
	CHECK_MAIN_POWER	*PowerChecker;

	Cprm  = Axis->UniPrms->Cprm;		/* パラメータ設定データ */
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn00F.0 : 予防保全ワーニング選択															*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Cprm->b_prmF & 0x0FU )
	{
	case 0x00U: /* 予防保全ワーニングを検出しない */
		PowerChecker->conf.PreWarningSw = FALSE;
		break;
	case 0x01U: /* 予防保全ワーニングを検出する */
		PowerChecker->conf.PreWarningSw = TRUE;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prmF.RegNumber );
		break;
	}
}
/* < S154 > End  */

/****************************************************************************************************/
/*																									*/
/*		ゲイン関係応用スイッチ(Pn10B)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn10B.0 : モードスイッチ選択スイッチ --> PcalBitParamMSW()									*/
/*		Pn10B.1 : 速度ループ制御選択スイッチ														*/
/*		Pn10B.2 : 位置ループ制御選択スイッチ														*/
/*		Pn10B.3 : 制振制御選択スイッチ																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam10B( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BOOL		PrmSetting; /* <S066> */

	Prm = Axis->UniPrms->Prm;		// パラメータ設定データ

/*--------------------------------------------------------------------------------------------------*/
/*		Pn10B.1 : 速度ループ制御選択設定															*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->gnmode >> 4 ) & 0x0F )
	{
		case 0x00:
			Axis->BaseControls->CtrlCmdPrm.IPSpdControl = FALSE;
			break;
		case 0x01:
			Axis->BaseControls->CtrlCmdPrm.IPSpdControl = TRUE;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, pndef_gnmode.RegNumber );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn10B.3 : 位相補償速度オブザーバ設定														*/
/*--------------------------------------------------------------------------------------------------*/
	PrmSetting = EhVobsCalculatePrmSW( &(Axis->BaseControls->EhVobsCtrl.conf.EhVobsUse),
									   Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse,
									   (Prm->gnmode >> 12) );

	if( PrmSetting == PRM_SETTING_NG )
	{
		ALMSetPramError( Axis->AlmManager, pndef_gnmode.RegNumber );
	}
}


/****************************************************************************************************/
/*																									*/
/*		位置制御指令形態選択スイッチ(Pn200)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn200.0 : 指令パルス形態選択スイッチ --> JL056HW.C											*/
/*		Pn200.1 : クリア信号形態選択スイッチ --> V850E.C											*/
/*		Pn200.2 : クリア動作選択スイッチ	 														*/
/*		Pn200.3 : フィルタ選択スイッチ																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam200( AXIS_HANDLE *Axis )
{
#if SVFSEL_CMDPLS_USE == 1		/* <S182>コード整理 */
	if(OK != KpiPcalPerClrMode( Axis->pPlsIfV, Axis->UniPrms->Prm->prstsw ) ) /* Pn200: Perr Clear Mode */
	{
		ALMSetPramError( Axis->AlmManager, pndef_prstsw.RegNumber );
	}
#endif
}


/****************************************************************************************************/
/*																									*/
/*		位置制御機能スイッチ(Pn207)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn207.0 : 位置指令フィルタ選択スイッチ														*/
/*		Pn207.1 : 位置制御オプション選択スイッチ													*/
/*		Pn207.2 : バックラッシ補正選択スイッチ														*/
/*		Pn207.3 : /COIN出力タイミング選択スイッチ													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam207( AXIS_HANDLE *Axis )
{
	ULONG		ucwk;
	PRMDATA		*Prm;
//	IPRMDAT		*Iprm;

	Prm  = Axis->UniPrms->Prm;		// パラメータ設定データ
//	Iprm = Axis->prm_hndl_ptr->iprm_ptr;		// 初期パラメータ定義(電源再投入で変更有効)

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.0 : 位置指令フィルタ選択設定															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	ucwk = Prm->cmfilsw & 0x0F;
	switch( ucwk )
	{
		case 0x00:					/* 加減速フィルタ												*/
		case 0x01:					/* 移動平均フィルタ												*/
			Iprm->PcmdFilter = (UCHAR)ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, &pndef_cmfilsw );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.1 : 位置制御オプション選択設定														*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->cmfilsw >> 4 ) & 0x0F )
	{
		case 0x00:
			Iprm->VrefSpdFF = FALSE;
			break;
		case 0x01:
			Iprm->VrefSpdFF = TRUE;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, &pndef_cmfilsw );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.2 : バックラッシ補正選択設定															*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = ( Prm->cmfilsw >> 8 ) & 0x0F;
	switch( ucwk )
	{
		case 0x00:					/* バックラッシ補正無効											*/
		case 0x01:					/* 正転方向にバックラッシ補正									*/
		case 0x02:					/* 逆転方向にバックラッシ補正									*/
			Iprm->BacklashDir = (UCHAR)ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, &pndef_cmfilsw );
			break;
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.3 : /COIN出力タイミング選択設定														*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = (Prm->cmfilsw >> 12) & 0x0F;
	switch( ucwk )
	{
		case 0x00:
		case 0x01:
		case 0x02:
			Axis->BaseControls->PosManager.conf.CoinOutSel = (UCHAR)ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, pndef_cmfilsw.RegNumber );
			break;
	}
}

#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		位置制御拡張機能スイッチ(Pn230)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn230.0 : バックラッシ補正方向																*/
/*		Pn230.1 : 予約定数																			*/
/*		Pn230.2 : 予約定数																			*/
/*		Pn230.3 : 予約定数																			*/
/*																									*/
/****************************************************************************************************/
void	IprmcalBitParam230( AXIS_HANDLE *Axis )
{
	UCHAR		ucwk;
	PRMDATA		*Prm;
	BLSHCMP		*BlshCmp;

	Prm = Axis->UniPrms->Prm;
	BlshCmp = &(Axis->BaseControls->PosManager.BlshCmp);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn230.0 : バックラッシ補正方向																*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = (Prm->cmctrlexsw) & 0x0F;
	switch( ucwk )
	{
	case 0x00: /* 正転方向の指令で補正する */
	case 0x01: /* 逆転方向の指令で補正する */
		BlshCmp->conf.BacklashDir = ucwk;
		break;
	default :
		ALMSetPramError( Axis->AlmManager, pndef_cmctrlexsw.RegNumber );
		break;
	}
}
#endif

#if 0	/* Pn408.1:オンラインに変更 */
/****************************************************************************************************/
/*																									*/
/*		トルク関係機能スイッチ(Pn408)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn408.0 : ノッチフィルタ選択１スイッチ（オンライン）--> pncal_tfuncsw()						*/
/*		Pn408.1 : 速度制限選択スイッチ（オフライン）												*/
/*		Pn408.2 : ノッチフィルタ選択２スイッチ（オンライン）--> pncal_tfuncsw()						*/
/*		Pn408.3 : 外乱オブザーバ選択スイッチ（オフライン）（不使用）								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam408( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	IPRMDAT		*Iprm;

	Prm  = Axis->UniPrms->Prm;		// パラメータ設定データ
	Iprm = Axis->UniPrms->iprm_ptr;		// 初期パラメータ定義(電源再投入で変更有効)

/*--------------------------------------------------------------------------------------------------*/
/*		Pn408.1 : 速度制限選択設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->tfuncsw >> 4 ) & 0x0F )
	{
		case 0x00:	Iprm->OvrSpdLimit = FALSE; break;
		case 0x01:	Iprm->OvrSpdLimit = TRUE;  break;
		default :	ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber ); break;
	}

}
#endif

/* <S14B> Start */
/****************************************************************************************************/
/*		Pn423 : 速度リップル補償機能選択スイッチ 		[－]										*/
/****************************************************************************************************/
/*																									*/
/*		Pn423.0 : 速度リップル補償機能の使用／不使用の選択				（オンライン）				*/
/*		Pn423.1 : 速度リップル補償情報不一致ワーニング(A.942)検出の選択	（オフライン）				*/
/*		Pn423.2 : 速度リップル補償機能の有効化しきい値条件の選択		（オフライン）				*/
/*		Pn423.3 : 速度リップル補償機能位相選択（非公開）				（オフライン）				*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitVelRippleCompSW( AXIS_HANDLE *Axis )/* <S1FD> */
{
//	ALMMAN_HANDLE	*AlmManager;	/* <S1FD> */
	PRMDATA			*Prm;
	RPLCMP			*RippleCmp;
	ULONG			MotorSnReadRslt;

//	AlmManager	= Axis->AlmManager;	/* <S1FD> */
	Prm			= Axis->UniPrms->Prm;
	RippleCmp	= &(Axis->BaseLoops->RippleCmp);

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.1 : 速度リップル補償情報不一致ワーニング(A.942)検出の選択									*/
/*--------------------------------------------------------------------------------------------------*/
	if( (( Prm->RippleCompSw >> 4 ) & 0x0F ) == 0x00 )
	{
		/* ワーニングを検出する		*/
		ALMSetMask( Axis->AlmManager, WRN_RIPPLE, FALSE );/* A.942 : リップル補正情報不一致ワーニング	*/
		RippleCmp->RplCmpPrm.RippleWrnMode = FALSE;	/*<S1FD>*/
	}
	else
	{
		/* ワーニングを検出しない		*/
		ALMSetMask( Axis->AlmManager, WRN_RIPPLE, TRUE  );/* A.942 : リップル補正情報不一致ワーニング	*/
		RippleCmp->RplCmpPrm.RippleWrnMode = TRUE;	/*<S1FD>*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.2 : 速度リップル補償機能の有効化しきい値条件の選択										*/
/*--------------------------------------------------------------------------------------------------*/
	if( (( Prm->RippleCompSw >> 8 ) & 0x0F ) == 0x00 )
	{
		/* 速度リップル補償機能の有効化しきい値条件 = 「指令速度」 */
		RippleCmp->RippleCmpThresholdSel = CMD_SPD;
	}
	else
	{
		/* 速度リップル補償機能の有効化しきい値条件 = 「フィードバック速度」 */
		RippleCmp->RippleCmpThresholdSel = FB_SPD;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.3 : 速度リップル補償機能位相選択（非公開）	<TODO>										*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	if( (( Prm->RippleCompSw >> 12 ) & 0x0F ) == 0x00 )
	{
		/* モータの電気角（0～360°）を使用 */

	}
	else
	{
		/* モータの機械角（0～360°）を使用 (リニアモータの場合は無効) */

	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.0 : 速度リップル補償機能の使用／不使用の選択												*/
/*--------------------------------------------------------------------------------------------------*/
	if( (Prm->RippleCompSw & 0x0F) == 0x00 )
	{
		/* 速度リップル補償情報状態 : 正常 */
		RippleCmp->var.RippleWrnState = TRUE;

		/* Pn423.0 = 0 : 速度リップル補償機能を使用しない */
		RippleCmp->RplCmpPrm.RplCmpUse = FALSE;
	}
	else
	{
		/* 今回取得モータシリアルNo.、前回取得モータシリアルNo. */
		MotorSnReadRslt = ChkMotorSerialNo( Axis->AxisNo, Axis->MencV );
//		if( MotorSnReadRslt != 0 )/*<S1FD>*/
		if( (MotorSnReadRslt != 0) && (RippleCmp->RplCmpPrm.RippleWrnMode == FALSE) )
		{	/* モータシリアルNo.が異なる && Pn423.1=0 のとき */
//			ALMSetServoAlarm( AlmManager, WRN_RIPPLE );
			/* 速度リップル補償情報状態 : A.942発生させる */
			RippleCmp->var.RippleWrnState = FALSE;

			/* Pn423.0 = 0 : 速度リップル補償機能を使用しない */
			RippleCmp->RplCmpPrm.RplCmpUse = FALSE;
		}
		else
		{
			/* 速度リップル補償情報状態 : 正常 */
			RippleCmp->var.RippleWrnState = TRUE;

			/* Pn423.0 = 1 : 速度リップル補償機能を使用する*/
			RippleCmp->RplCmpPrm.RplCmpUse = TRUE;
		}
	}
//	return;/* <S1FD> */
}

/****************************************************************************************************/
/*		Pn427 : 速度リップル補償有効速度 		[min-1]												*/
/****************************************************************************************************/
static	void	IprmcalVelRippleCompSpd( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	RPLCMP		*RippleCmp;

	Prm			= Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	RippleCmp	= &(Axis->BaseLoops->RippleCmp);


	/* Speed [0.1r/min, 0.1mm/s] --> NormalizedSpeed [2^24/OvrSpd] */
	RippleCmp->VelRippleCompSpd = BprmSpeedLevelCal( Bprm, 10 * Prm->VelRippleCompSpd, 0 );
}
/* <S14B> End */
/* <S21D> Start */
/*****************************************************************************
  Description: 出力信号選択4(Pn514)
------------------------------------------------------------------------------
  Parameters:
	Axis;		Pointer to Axis Handle
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static	void	IprmcalBitParam514( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	CHECK_MAIN_POWER	*PowerChecker;

	Prm  = Axis->UniPrms->Prm;		/* パラメータ設定データ */
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	switch( (Prm->outsel4 >> 4) & 0x0F )
	{
		case 0x00:
			PowerChecker->conf.CapaLinkFlag = FALSE;
			break;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
			PowerChecker->conf.CapaLinkFlag = TRUE;
			break;
		default:
		ALMSetPramError( Axis->AlmManager, pndef_outsel4.RegNumber );
		break;
	}
}
/* <S21D> End*/

/****************************************************************************************************/
/*																									*/
/*		ＤＣ電源電圧監視レベル計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ＤＣ電源電圧監視レベルの計算を行う。													*/
/*																									*/
/*			1. 電圧レベル																			*/
/*				1) ＯＶ検出レベル																	*/
/*				2) ＵＶ検出レベル																	*/
/*				3) ＵＶ検出フィルタ時定数															*/
/*				4) 回生オンレベル/オフレベル														*/
/*																									*/
/*			2. パラメータ																			*/
/*				1) PnE70 : P-N電圧検出レベル														*/
/*		 		2) PnE71 : OV検出レベル   [V]														*/
/*				3) PnE72 : 回生オンレベル [V]														*/
/*		 		4) PnE73 : UV検出レベル   [V]														*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalVoltageCheckLevel( AXIS_HANDLE *Axis )
{
	LONG				s;
	CMN_PRMDATA			*Cprm;
	BPRMDAT				*Bprm;
	REGENE_CTRL			*RegCtrl;
	DET_DC_VOLT			*DetectDc;
	CHECK_MAIN_POWER	*PowerChecker;

	s = 0;
	Cprm  = Axis->UniPrms->Cprm;		// パラメータ設定データ
	Bprm = Axis->UniPrms->Bprm;		// 初期パラメータ定義(電源再投入で変更有効)
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	DetectDc = &(Axis->PowerManager->DetectDcVolt);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	/*------------------------------------------------------------------------------------------*/
	/*		OVレベルの計算																		*/
	/*------------------------------------------------------------------------------------------*/
	/* 回生を使用する場合 */
	PowerChecker->conf.OvLevel = (USHORT)(Cprm->ovlvl * Bprm->VdetMax / Cprm->pnvoltgn);
	/* 回生不使用の場合 */
	PowerChecker->conf.NonRegOvLevel = (USHORT)(Cprm->NonRegOvlvl * Bprm->VdetMax / Cprm->pnvoltgn); /* <S0F7> */

	/*------------------------------------------------------------------------------------------*/
	/*		UVレベルの計算																		*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.UvLevel = (USHORT)(Cprm->uvlvl * Bprm->VdetMax / Cprm->pnvoltgn);
	PowerChecker->conf.UvWrnLevel = (USHORT)(Cprm->UvWrnLvl * Bprm->VdetMax / Cprm->pnvoltgn);
	PowerChecker->conf.UvFilter = Cprm->uvfil;

	/*------------------------------------------------------------------------------------------*/
	/*		主回路電圧検出補正 基準電圧レベルの計算																		*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.TestDcLevel = (USHORT)(Cprm->DcBaseLvl * Bprm->VdetMax / Cprm->pnvoltgn);

	/*------------------------------------------------------------------------------------------*/
	/*		回生レベルの計算																	*/
	/*------------------------------------------------------------------------------------------*/
	/* <S21D> Start */
#if 0
	RegCtrl->conf.RegOnLevel  = (USHORT)( Cprm->regonlvl * Bprm->VdetMax / Cprm->pnvoltgn );
#else
	if( ((Cprm->b_prmE >> 12) & 0x0F) == PNREGSYN )
	{	/* 回生同期有効の場合 */
		RegCtrl->conf.RegOnLevel = (USHORT)( (Cprm->regonlvl + Cprm->regsynonlvl) * (Bprm->VdetMax / Cprm->pnvoltgn) );
	}
	else
	{
//		RegCtrl->conf.RegOnLevel = (USHORT)( Cprm->regonlvl * (Bprm->VdetMax / Cprm->pnvoltgn) );/* <S21E> */
		RegCtrl->conf.RegOnLevel = (USHORT)( (Cprm->regonlvl * Bprm->VdetMax) / Cprm->pnvoltgn );/* <S21E> */
	}
#endif
	/* <S21D> End */
	RegCtrl->conf.RegOffLevel = (USHORT)( ((Cprm->regonlvl - Cprm->regoffhys) * Bprm->VdetMax) / Cprm->pnvoltgn );

	/*------------------------------------------------------------------------------------------*/
	/*		AD変換方式で読出した値の正規化定数													*/
	/*------------------------------------------------------------------------------------------*/
	/* 2010.06.16 Y.Oka (>_<) for INGRAM PnE70[V]入力時のA/D値をパラメータ化すること。ついでにVdetMaxも！？ */
//	Iprm->VdetAdNorm = MlibPcalKxgain( Bprm->VdetMax, 1, Bprm->VdetAdMax, &s, 24 );
//	DetectDc->conf.VdetAdNorm = MlibPcalKxgain( Bprm->VdetMax, 1, PRM_VDETADMAX, &s, 24 );
	DetectDc->conf.VdetAdNorm = MlibPcalKxgain( Bprm->VdetMax, 1, Cprm->VdetADMax, &s, 24 );

	/*------------------------------------------------------------------------------------------*/
	/*		電圧検出最大値の取得																*/
	/*------------------------------------------------------------------------------------------*/
	DetectDc->conf.VdetMaxVolt = Bprm->VdetMax;

	/* 主回路検出AD用フィルタ時定数計算 */
	/* 2011.03.29 Y.Oka ScanB周期変更に対応 */
//	DetectDc->conf.VdetAdFil = MlibPcalKf1gain( Cprm->vdetadfil, (2 * Bprm->SvCycleUs), 0 );
	PcmCalculateVdetAdFil( DetectDc, Cprm->vdetadfil, Bprm->SvCycleUs );

	/*------------------------------------------------------------------------------------------*/
	/*		急速放電完了レベル 急速放電時間の計算								<S1C6>			*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.DischCheckVlt = (USHORT)(DISCH_CHK_VOLT * Bprm->VdetMax / Cprm->pnvoltgn);	  /* <S1D1> */
	PowerChecker->conf.DischCompVlt = (USHORT)(Cprm->DischCompVlt * Bprm->VdetMax / Cprm->pnvoltgn);  /* <S1D1> */
	PowerChecker->conf.MainDischActTime = KPI_SCANC_MS( Cprm->ChgWaitTime );

/*--------------------------------------------------------------------------------------------------*/
/*							P-N電圧検出レベル[V]   													*/
/*		KioP.Kdcvolt = -------------------------- 													*/
/*						  		VdetMax	   			VdetMax：正規化定数								*/
/*--------------------------------------------------------------------------------------------------*/
	DetectDc->conf.Kdcvolt = MlibScalKxgain( Cprm->pnvoltgn, 1, Bprm->VdetMax, NULL, 24 );

}

/****************************************************************************************************/
/*																									*/
/*		ＤＣ電源電圧監視時間計算 <S053>																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ＤＣ電源電圧監視時間の計算を行う。														*/
/*																									*/
/*			1. 電圧監視時間																			*/
/*				1) ＤＣ充電完了待ち時間																*/
/*				2) パワーレディ待ち時間																*/
/*				3) 投入前地絡待ち時間																*/
/*																									*/
/*			2. パラメータ																			*/
/*				1) PnE88 : ＤＣ充電完了待ち時間 [ms]												*/
/*		 		2) PnE89 : パワーレディ待ち時間 [ms]												*/
/*				3) PnE8A : 投入前地絡待ち時間   [ms]												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalVoltageCheckTime( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	CHECK_MAIN_POWER	*PowerChecker;

	Cprm  = Axis->UniPrms->Cprm;
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	/*------------------------------------------------------------------------------------------*/
	/*		ＤＣ充電完了待ち時間の計算															*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.PowerChargeWaitTime = KPI_SCANC_MS(Cprm->ChgWaitTime);

	/*------------------------------------------------------------------------------------------*/
	/*		パワーレディ待ち時間の計算															*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.PowerReadyWaitTime = KPI_SCANC_MS(Cprm->PrdyWaitTime);

	/*------------------------------------------------------------------------------------------*/
	/*		投入前地絡待ち時間の計算															*/
	/*------------------------------------------------------------------------------------------*/
//	PowerChecker->conf.GndFaultWaitTime = KPI_SCANC_MS(Cprm->GFWaitTime);

	/*------------------------------------------------------------------------------------------*/
	/*		@@ パラメータ整合性チェック															*/
	/*		PowerReadyWaitTime <= PowerChargeWaitTime の場合、シーケンス上不整合が起こる		*/
	/*		(ChargeOkがTRUEにならなくなる)ため、パラメータ計算エラーにしておく。				*/
	/*------------------------------------------------------------------------------------------*/
	if ( ( PowerChecker->conf.PowerChargeWaitTime == 0 ) || 
		( PowerChecker->conf.PowerReadyWaitTime <= PowerChecker->conf.PowerChargeWaitTime ) )
	{
		/* A.040 : パラメータ設定異常 */
		ALMSetPramError( Axis->AlmManager, pndef_ChgWaitTime.RegNumber );
	}
}


/****************************************************************************************************/
/*																									*/
/*		サーボパック内蔵ファン停止検出時間 <S0A3>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ファン停止検出時間の計算を行う。														*/
/*																									*/
/*			1. パラメータ																			*/
/*				1) PnEE5 : ファン停止アラーム遷移時間 0.1[s]										*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalFanLockDetectTime( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	Cprm  = Axis->UniPrms->Cprm;

	/* 演算用変数初期化 */
	MlibResetLongMemory( &(Axis->PowerManager->FanLock.var), sizeof( Axis->PowerManager->FanLock.var )/4 );

	Axis->PowerManager->FanLock.conf.FanLockDetectTime = KPI_SCANC_MS( Cprm->FanLockTime * 100 );
	return;
}


/****************************************************************************************************/
/*																									*/
/*		アンプ＆モータ過負荷レベルの計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		LpxPcalOverLoadLevelAmpMot(				アンプ＆モータ過負荷ﾚﾍﾞﾙ計算関数(低速OL:SGDS方式)	*/
/*			OLPRM	*OlPrm,						過負荷パラメータ									*/
/*			CHKOLP	*ChkOLP1,					過負荷チェック用パラメータ(演算結果)				*/
/*			CHKOLP	*ChkOLP2,					過負荷チェック用パラメータ(異常時設定用)			*/
/*			CHKOLP	*ChkOLP3					過負荷チェック用パラメータ(検出ﾚﾍﾞﾙｹﾞｲﾝ計算用)		*/
/*			PRMDATA *prm_ptr																		*/
/*			IPRMDAT *iprm_ptr	);																	*/
/*																									*/
/*		補足）ChkOLP1 == ChkOLP2の場合には、例外処理として	ChkOLP2⇒ChkOLP1のコピー処理は			*/
/*			  行わないでエラーとする。																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static	LONG	LpxPcalOverLoadLevelAmpMot( OLPRM *OlPrm, CHECK_OL *ChkOLP1, CHECK_OL *ChkOLP2,
//											CHECK_OL *ChkOLP3, PRMDATA *prm_ptr, KSGAIN	LinearCoolRate100PerGain )		/* <S154> */
static	LONG	LpxPcalOverLoadLevelAmpMot( OLPRM *OlPrm, CHECK_OLP *ChkOLP1, CHECK_OLP *ChkOLP2,
											CHECK_OLP *ChkOLP3, PRMDATA *prm_ptr, KSGAIN	LinearCoolRate100PerGain )	/* <S154> */
{
	LONG	Ix;
	LONG	Ibsqr;
	LONG	Ixsqr;
	LONG	Iysqr;
	LONG	Izsqr;
	LONG	rc;
	LONG	wk;
	LONG	cflg;

	rc = TRUE;

	/********************************************************************************************/
	/*		起動条件の設定																		*/
	/*		  FLASE ：ChkOLP2からの設定不可														*/
	/*		  TRUE  ：ChkOLP2からの設定可														*/
	/********************************************************************************************/
	if( ChkOLP1 != ChkOLP2 )
	{
		cflg = TRUE;
	}
	else
	{
		cflg = FALSE;
	}

	/********************************************************************************************/
	/*		連続過負荷レベルの計算																*/
	/********************************************************************************************/
	/*																							*/
	/*		Ibsqr = Ibse * Ibse																	*/
	/*																							*/
	/*				  (Imid^2 - Ibse^2) * Tmid[0.1s]											*/
	/*		AlmLvl = --------------------------------											*/
	/*						  Cycle[ms]															*/
	/*																							*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/* 	ベース電流のチェック：	OlPrm->Ibse														*/
	/*------------------------------------------------------------------------------------------*/
	if( OlPrm->Ibse > 15000 )						/* 下記値を片側の設定値に合わせる			*/
	{
		if( cflg == TRUE )
		{
#if 1	/* <S154> */
			/* 連続ＯＬベース電流  */
			ChkOLP1->Ibsqrmid = ChkOLP2->Ibsqrmid;
			/* 連続ＯＬ検出レベル */
			ChkOLP1->AlmLvlmid = ChkOLP2->AlmLvlmid;
			/* 連続ＯＬワーニングレベル  */
			ChkOLP1->WrnLvlmid = ChkOLP2->WrnLvlmid;
			/* 連続ＯＬ検出レベルゲイン */
			ChkOLP1->OlLvlmidGain = ChkOLP2->OlLvlmidGain;
			/* 瞬時ＯＬベース電流  */
			ChkOLP1->Ibsqrmax = ChkOLP2->Ibsqrmax;
			/* 瞬時ＯＬ検出レベル */
			ChkOLP1->AlmLvlmax = ChkOLP2->AlmLvlmax;
			/* 瞬時ＯＬワーニングレベル */
			ChkOLP1->WrnLvlmax = ChkOLP2->WrnLvlmax;
			/* 瞬時ＯＬ検出レベルゲイン */
			ChkOLP1->OlLvlmaxGain = ChkOLP2->OlLvlmaxGain;
#else	/* <S154> */
			/* 連続ＯＬベース電流  */
			ChkOLP1->conf.Ibsqrmid = ChkOLP2->conf.Ibsqrmid;
			/* 連続ＯＬ検出レベル */
			ChkOLP1->conf.AlmLvlmid = ChkOLP2->conf.AlmLvlmid;
			/* 連続ＯＬワーニングレベル  */
			ChkOLP1->conf.WrnLvlmid = ChkOLP2->conf.WrnLvlmid;
			/* 連続ＯＬ検出レベルゲイン */
			ChkOLP1->conf.OlLvlmidGain = ChkOLP2->conf.OlLvlmidGain;
			/* 瞬時ＯＬベース電流  */
			ChkOLP1->conf.Ibsqrmax = ChkOLP2->conf.Ibsqrmax;
			/* 瞬時ＯＬ検出レベル */
			ChkOLP1->conf.AlmLvlmax = ChkOLP2->conf.AlmLvlmax;
			/* 瞬時ＯＬワーニングレベル */
			ChkOLP1->conf.WrnLvlmax = ChkOLP2->conf.WrnLvlmax;
			/* 瞬時ＯＬ検出レベルゲイン */
			ChkOLP1->conf.OlLvlmaxGain = ChkOLP2->conf.OlLvlmaxGain;
#endif	/* <S154> */

			/*----------------------------------------------------------------------------------*/
			/*	モータ自走空冷係数設定															*/
			/*		アンプ過負荷係数をモータ過負荷係数に合わせる場合は自走空冷100%				*/
			/*		に設定してアンプ過負荷をモータ過負荷よりレベルを下げる						*/
			/*		ChkOLP1がモータの場合は、Pn586計算で上書きされるため、						*/
			/*		ここでは、ChkOLP1に100%を設定する											*/
			/*----------------------------------------------------------------------------------*/
//			ChkOLP1->conf.CoolRateGain = LinearCoolRate100PerGain;	/* <S154> */
			ChkOLP1->CoolRateGain = LinearCoolRate100PerGain;		/* <S154> */

			return( TRUE );								/* パラメータ設定異常にしない			*/
		}
		else
		{
			return( FALSE );							/* パラメータ設定異常にする				*/
		}
	}
	else
	{
		Ix = OlPrm->Ibse;
	}
	Ibsqr = (Ix * Ix) >> 16;							/* Ibsqr：ベース電流^2					*/

	/*------------------------------------------------------------------------------------------*/
	/* 	中間ＯＬ電流のチェック：	OlPrm->Imid													*/
	/*------------------------------------------------------------------------------------------*/
	if( OlPrm->Imid > 0x7FFF )
	{
		Ix = 0x7FFF;
	}
	else
	{
		Ix = OlPrm->Imid;
	}
	Ixsqr = (Ix * Ix) >> 16;							/* Ixsqr：中間ＯＬ電流^2				*/

	/*------------------------------------------------------------------------------------------*/
	/* 	連続ＯＬベース電流^2、アラーム検出レベル、ワーニングレベル、レベルゲインの計算			*/
	/*------------------------------------------------------------------------------------------*/
	/*	Izsqr		：連続ＯＬ検出アラームレベルの分子項 ( = 中間ＯＬ電流^2 - ベース電流^2)		*/
	/*	Ibsqrmid	：連続ＯＬベース電流^2														*/
	/*	AlmLvlmid	：連続ＯＬアラーム検出レベル												*/
	/*	WrnLvlmid	：連続ＯＬワーニングレベル													*/
	/*	OlLvlmidGain：連続ＯＬ検出レベルゲイン													*/
	/*------------------------------------------------------------------------------------------*/
	Izsqr = Ixsqr - Ibsqr;
#if 1	/* <S154> */
	ChkOLP1->Ibsqrmid  = Ibsqr;
#if (FLOAT_USE==TRUE)
	ChkOLP1->AlmLvlmid = ( (float)Izsqr * (float)(100*OlPrm->Tmid) / (float)KPI_SCCYCLEMS );
	ChkOLP1->WrnLvlmid = ( (float)ChkOLP1->AlmLvlmid * (float)prm_ptr->olwarlvl / (float)100 );
	ChkOLP1->OlLvlmidGain = (float)ChkOLP3->AlmLvlmid / (float)ChkOLP1->AlmLvlmid;
#else
	ChkOLP1->AlmLvlmid = MlibScalKxgain( Izsqr, (100*OlPrm->Tmid), KPI_SCCYCLEMS, NULL, -30 );
	ChkOLP1->WrnLvlmid = MlibScalKxgain( ChkOLP1->AlmLvlmid, prm_ptr->olwarlvl, 100, NULL, -30 );
	ChkOLP1->OlLvlmidGain = MlibScalKxgain( ChkOLP3->AlmLvlmid, 1, ChkOLP1->AlmLvlmid, NULL, 24 );
#endif /* FLOAT_USE */
#else	/* <S154> */
	ChkOLP1->conf.Ibsqrmid  = Ibsqr;
#if (FLOAT_USE==TRUE)
	ChkOLP1->conf.AlmLvlmid = ( (float)Izsqr * (float)(100*OlPrm->Tmid) / (float)KPI_SCCYCLEMS );
	ChkOLP1->conf.WrnLvlmid = ( (float)ChkOLP1->conf.AlmLvlmid * (float)prm_ptr->olwarlvl / (float)100 );
	ChkOLP1->conf.OlLvlmidGain = (float)ChkOLP3->conf.AlmLvlmid / (float)ChkOLP1->conf.AlmLvlmid;
#else
	ChkOLP1->conf.AlmLvlmid = MlibScalKxgain( Izsqr, (100*OlPrm->Tmid), KPI_SCCYCLEMS, NULL, -30 );
	ChkOLP1->conf.WrnLvlmid = MlibScalKxgain( ChkOLP1->conf.AlmLvlmid, prm_ptr->olwarlvl, 100, NULL, -30 );
	ChkOLP1->conf.OlLvlmidGain = MlibScalKxgain( ChkOLP3->conf.AlmLvlmid, 1, ChkOLP1->conf.AlmLvlmid, NULL, 24 );
#endif /* FLOAT_USE */
#endif	/* <S154> */

//	if( (ChkOLP1->conf.AlmLvlmid >= 0x40000000) || (ChkOLP1->conf.AlmLvlmid == 0) )		/* <S154> */
	if( (ChkOLP1->AlmLvlmid >= 0x40000000) || (ChkOLP1->AlmLvlmid == 0) )		/* <S154> */
	{
		rc = FALSE;
	}

	/********************************************************************************************/
	/*		瞬時過負荷レベルの計算																*/
	/********************************************************************************************/
	/*																							*/
	/*				 (Imid^2 * Tmid) - (Imax^2 * Tmax)											*/
	/*		Ibsqr = -----------------------------------											*/
	/*						 (Tmid - Tmax)														*/
	/*																							*/
	/*				  (Imax^2 - Imsqr) * Tmax													*/
	/*		AlmLvl = ----------------------------												*/
	/*						   Cycle															*/
	/*																							*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/* 	最大ＯＬ電流のチェック：	 OlPrm->Imax												*/
	/*------------------------------------------------------------------------------------------*/
	if( OlPrm->Imax > 0x7FFF)						/* 下記値を片側の設定値に合わせる			*/
	{
		if( cflg == TRUE )
		{
#if 1	/* <S154> */
			/* 瞬時ＯＬベース電流 */
			ChkOLP1->Ibsqrmax = ChkOLP2->Ibsqrmax;
			/* 瞬時ＯＬ検出レベル */
			ChkOLP1->AlmLvlmax = ChkOLP2->AlmLvlmax;
			/* 瞬時ＯＬワーニングレベル */
			ChkOLP1->WrnLvlmax = ChkOLP2->WrnLvlmax;
			/* 瞬時ＯＬ検出レベルゲイン */
			ChkOLP1->OlLvlmaxGain = ChkOLP2->OlLvlmaxGain;
#else	/* <S154> */
			/* 瞬時ＯＬベース電流 */
			ChkOLP1->conf.Ibsqrmax = ChkOLP2->conf.Ibsqrmax;
			/* 瞬時ＯＬ検出レベル */
			ChkOLP1->conf.AlmLvlmax = ChkOLP2->conf.AlmLvlmax;
			/* 瞬時ＯＬワーニングレベル */
			ChkOLP1->conf.WrnLvlmax = ChkOLP2->conf.WrnLvlmax;
			/* 瞬時ＯＬ検出レベルゲイン */
			ChkOLP1->conf.OlLvlmaxGain = ChkOLP2->conf.OlLvlmaxGain;
#endif	/* <S154> */

			/*----------------------------------------------------------------------------------*/
			/*	モータ自走空冷係数設定															*/
			/*		アンプ過負荷係数をモータ過負荷係数に合わせる場合は自走空冷100%				*/
			/*		に設定してアンプ過負荷をモータ過負荷よりレベルを下げる						*/
			/*		ChkOLP1がモータの場合は、Pn586計算で上書きされるため、						*/
			/*		ここでは、ChkOLP1に100%を設定する											*/
			/*----------------------------------------------------------------------------------*/
//			ChkOLP1->conf.CoolRateGain = LinearCoolRate100PerGain;	/* <S154> */
			ChkOLP1->CoolRateGain = LinearCoolRate100PerGain;		/* <S154> */

			return( TRUE );								/* パラメータ設定異常にしない			*/
		}
		else
		{
			return( FALSE );							/* パラメータ設定異常にする				*/
		}
	}
	else
	{
		Ix = OlPrm->Imax;
	}
	Iysqr = (Ix * Ix) >> 16;						/* Iysqr：最大ＯＬ電流^2					*/

	/*------------------------------------------------------------------------------------------*/
	/* 	瞬時ＯＬベース電流^2の計算：	 Ibsqr													*/
	/*------------------------------------------------------------------------------------------*/
	wk = ( Ixsqr * OlPrm->Tmid ) - ( Iysqr * OlPrm->Tmax );		/* 瞬時ＯＬベース電流^2の分子項 */
//	Ibsqr = MlibScalKxgain( wk, 1, (OlPrm->Tmid - OlPrm->Tmax), NULL, -30 );
	Ibsqr = (LONG)( (float)wk / (float)(OlPrm->Tmid - OlPrm->Tmax) );

	/*------------------------------------------------------------------------------------------*/
	/* 	瞬時ＯＬベース電流^2の分子項チェック 													*/
	/*------------------------------------------------------------------------------------------*/
	if( wk < 0 )
	{
#if 1	/* <S154> */
		ChkOLP1->Ibsqrmax		= 0;			/* 瞬時ＯＬベース電流^2	= 0					*/
		ChkOLP1->AlmLvlmax		= 0;			/* 瞬時ＯＬレベル = 0						*/
		ChkOLP1->WrnLvlmax		= 0;			/* 瞬時ＯＬワーニングレベル = 0				*/
		ChkOLP1->OlLvlmaxGain	= 0;			/* 瞬時ＯＬワーニングレベル = 0				*/
#else	/* <S154> */
		ChkOLP1->conf.Ibsqrmax		= 0;			/* 瞬時ＯＬベース電流^2	= 0					*/
		ChkOLP1->conf.AlmLvlmax		= 0;			/* 瞬時ＯＬレベル = 0						*/
		ChkOLP1->conf.WrnLvlmax		= 0;			/* 瞬時ＯＬワーニングレベル = 0				*/
		ChkOLP1->conf.OlLvlmaxGain	= 0;			/* 瞬時ＯＬワーニングレベル = 0				*/
#endif	/* <S154> */
		return( FALSE );							/* パラメータ設定異常にする					*/
	}
	else
	{
		/*--------------------------------------------------------------------------------------*/
		/* 	瞬時ＯＬベース電流^2のチェック														*/
		/*--------------------------------------------------------------------------------------*/
		if( Ibsqr > 0x7fff)							/* 本条件は発生しないがとりあえずいれておく	*/
		{
			if(cflg == TRUE)
			{
//				Ibsqr = ChkOLP2->conf.Ibsqrmax;		/* 瞬時OLベース電流を片側の設定値に合わせる	*/	/* <S154> */
				Ibsqr = ChkOLP2->Ibsqrmax;		/* 瞬時OLベース電流を片側の設定値に合わせる	*/		/* <S154> */
			}
			else
			{
				return( FALSE );					/* パラメータ設定異常にする					*/
			}
		}

		/*--------------------------------------------------------------------------------------*/
		/* 瞬時ＯＬベース電流^2と連続ＯＬベース電流^2の大小関係チェック 						*/
		/*--------------------------------------------------------------------------------------*/
//		if( Ibsqr < ChkOLP1->conf.Ibsqrmid )		/* 下記値を連続ＯＬの設定値に合わせる	*/	/* <S154> */
		if( Ibsqr < ChkOLP1->Ibsqrmid )				/* 下記値を連続ＯＬの設定値に合わせる	*/	/* <S154> */
		{
#if 1 /* <S154> */
			/* 瞬時ＯＬベース電流^2 */
			ChkOLP1->Ibsqrmax  = ChkOLP1->Ibsqrmid;
			/* 瞬時ＯＬレベル */
			ChkOLP1->AlmLvlmax = ChkOLP1->AlmLvlmid;
			/* 瞬時ＯＬワーニングレベル */
			ChkOLP1->WrnLvlmax = ChkOLP1->WrnLvlmid;
			 /* 瞬時ＯＬレベルゲイン */
#if (FLOAT_USE==TRUE)
			ChkOLP1->OlLvlmaxGain =
					(float)ChkOLP3->AlmLvlmax / (float)ChkOLP1->AlmLvlmax;
#else
			ChkOLP1->OlLvlmaxGain =
					MlibScalKxgain( ChkOLP3->AlmLvlmax, 1, ChkOLP1->AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#else /* <S154> */
			/* 瞬時ＯＬベース電流^2 */
			ChkOLP1->conf.Ibsqrmax  = ChkOLP1->conf.Ibsqrmid;
			/* 瞬時ＯＬレベル */
			ChkOLP1->conf.AlmLvlmax = ChkOLP1->conf.AlmLvlmid;
			/* 瞬時ＯＬワーニングレベル */
			ChkOLP1->conf.WrnLvlmax = ChkOLP1->conf.WrnLvlmid;
			 /* 瞬時ＯＬレベルゲイン */
#if (FLOAT_USE==TRUE)
			ChkOLP1->conf.OlLvlmaxGain =
					(float)ChkOLP3->conf.AlmLvlmax / (float)ChkOLP1->conf.AlmLvlmax;
#else
			ChkOLP1->conf.OlLvlmaxGain =
					MlibScalKxgain( ChkOLP3->conf.AlmLvlmax, 1, ChkOLP1->conf.AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#endif /* <S154> */
		}
		/*--------------------------------------------------------------------------------------*/
		/* 	瞬時ＯＬベース電流^2の設定、瞬時ＯＬレベル、ワーニングレベル、レベルゲインの計算	*/
		/*--------------------------------------------------------------------------------------*/
		/*	Izsqr		：瞬時ＯＬ検出アラームレベルの分子項(最大ＯＬ電流^2 - 瞬時ベース電流^2)	*/
		/*	Ibsqrmax	：瞬時ＯＬベース電流^2													*/
		/*	AlmLvlmax	：瞬時ＯＬアラーム検出レベル											*/
		/*	WrnLvlmax	：瞬時ＯＬワーニングレベル												*/
		/*	OlLvlmaxGain：瞬時ＯＬ検出レベルゲイン												*/
		/*--------------------------------------------------------------------------------------*/
		else
		{
			Izsqr = Iysqr - Ibsqr;
#if 1 /* <S154> */
			ChkOLP1->Ibsqrmax  = Ibsqr;
#if (FLOAT_USE==TRUE)
			ChkOLP1->AlmLvlmax = ( (float)Izsqr * (float)(100*OlPrm->Tmax) / (float)KPI_SCCYCLEMS );
			ChkOLP1->WrnLvlmax = ( (float)ChkOLP1->AlmLvlmax * (float)prm_ptr->olwarlvl / (float)100 );
			ChkOLP1->OlLvlmaxGain = (float)ChkOLP3->AlmLvlmax / (float)ChkOLP1->AlmLvlmax;
#else
			ChkOLP1->AlmLvlmax = MlibScalKxgain( Izsqr, (100*OlPrm->Tmax), KPI_SCCYCLEMS, NULL, -30 );
			ChkOLP1->WrnLvlmax = MlibScalKxgain( ChkOLP1->AlmLvlmax, prm_ptr->olwarlvl, 100, NULL, -30 );
			ChkOLP1->OlLvlmaxGain = MlibScalKxgain( ChkOLP3->AlmLvlmax, 1, ChkOLP1->AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#else /* <S154> */
			ChkOLP1->conf.Ibsqrmax  = Ibsqr;
#if (FLOAT_USE==TRUE)
			ChkOLP1->conf.AlmLvlmax = ( (float)Izsqr * (float)(100*OlPrm->Tmax) / (float)KPI_SCCYCLEMS );
			ChkOLP1->conf.WrnLvlmax = ( (float)ChkOLP1->conf.AlmLvlmax * (float)prm_ptr->olwarlvl / (float)100 );
			ChkOLP1->conf.OlLvlmaxGain = (float)ChkOLP3->conf.AlmLvlmax / (float)ChkOLP1->conf.AlmLvlmax;
#else
			ChkOLP1->conf.AlmLvlmax = MlibScalKxgain( Izsqr, (100*OlPrm->Tmax), KPI_SCCYCLEMS, NULL, -30 );
			ChkOLP1->conf.WrnLvlmax = MlibScalKxgain( ChkOLP1->conf.AlmLvlmax, prm_ptr->olwarlvl, 100, NULL, -30 );
			ChkOLP1->conf.OlLvlmaxGain = MlibScalKxgain( ChkOLP3->conf.AlmLvlmax, 1, ChkOLP1->conf.AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#endif /* <S154> */

//			if( ChkOLP1->conf.AlmLvlmax >= 0x40000000 )	/* <S154> */
			if( ChkOLP1->AlmLvlmax >= 0x40000000 )		/* <S154> */
			{
				rc = FALSE;
			}
		}
	}
	return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		設置環境モニタ用パラメータ計算												<S03A>			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalInstEnvMon( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	Cprm	= Axis->UniPrms->Cprm;

	/*----------------------------------------------------------------------------------------------*/
	/*	PnE0F(下位Byte)：設置環境モニタ基準温度 (設定環境モニタ100%時の内気温度)					*/
	/*----------------------------------------------------------------------------------------------*/
	Axis->EnvMonitor->conf.AmpEnvMonOffset = 100 - (CHAR)(Cprm->InstEnvTmp_sw1.b.l);
	Axis->EnvMonitor->conf.MotEnvMonOffset = 100 - (CHAR)(Cprm->InstEnvTmp_sw1.b.l); /* TODO */
	return;

}



/****************************************************************************************************/
/*																									*/
/*		アンプ＆モータ過負荷レベルの計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnF09 : USHORT	irat;			モータ定格電流						[0.1Ao-p]				*/
/*		PnF0A : USHORT	imax;			モータ瞬時最大電流					[0.1Ao-p]				*/
/*		PnF0F : USHORT	bastrq;			モータ過負荷検出ベース電流			[%]						*/
/*		PnF10 : USHORT	midtrq;			モータ過負荷検出中間電流			[%]						*/
/*		PnF11 : USHORT	midolt;			モータ過負荷検出中間時間			[10s]					*/
/*		PnF12 : USHORT	mtrq2;			モータ過負荷検出中間電流２			[%]						*/
/*		PnF13 : USHORT	mtolt2;			モータ過負荷検出中間時間２			[s]						*/
/*																									*/
/*		PnE14 :	USHORT	irats;			アンプ定格電流						[0.1Ao-p]				*/
/*		PnE15 : USHORT	imaxs;			アンプ瞬時最大電流					[0.1Ao-p]				*/
/*		PnE17 : DBYTE	ibs_md.b.l;		アンプ過負荷検出ベース電流			[%]						*/
/*						ibs_md.b.h;		アンプ過負荷検出中間電流			[10%]					*/
/*		PnE18 : DBYTE	imdt_mxt.b.l;	アンプ過負荷検出中間時間			[10s]					*/
/*						imdt_mxt.b.h;	アンプ過負荷検出最大時間			[s]						*/
/*																									*/
/*		Pn52C : USHORT	mtbastrq_drt;	モータ過負荷検出ベース電流ディレーティング					*/
/*																									*/
/*		PnE60 : DBYTE	ratbt_mt.b.l;	ＯＬ特性ディレーティング(Base電流)	[%]						*/
/*						ratbt_mt.b.h;	ＯＬ特性ディレーティング(中間電流)	[%]						*/
/*		PnE61 : DBYTE	ratmtt_2.b.l;	ＯＬ特性ディレーティング(中間時間)	[%]						*/
/*						ratmtt_2.b.h;	ＯＬ特性ディレーティング(最大電流)	[%]						*/
/*		PnE62 : DBYTE	ratmt2_os.b.l;	ＯＬ特性ディレーティング(最大時間)	[%]						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PnE90 : DBYTE	olspdlvl.b.l;	アンプ低速ＯＬ低速区間変曲点の電気角速度	[0.1Hz]			*/
/*						olspdlvl.b.h;	アンプ低速ＯＬロック区間変曲点の電気角速度	[0.1Hz]			*/
/*		PnE91 : DBYTE	ollspdprm.b.l;	アンプ低速ＯＬ検出中間電流					[%]				*/
/*						ollspdprm.b.h;	アンプ低速ＯＬ検出最大時間					[0.1s]			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalOverLoadLevelAmpMot( AXIS_HANDLE *Axis )
{
	OLPRM		Mot, Amp;
	BOOL		prmchk;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	CHECK_OL	*pMotOL;
	CHECK_OL	*pAmpOL;
	KSGAIN		ks, ks2;
#if (FLOAT_USE==FALSE)
	LONG		kx, sx;
#endif /* FLOAT_USE */

	KSGAIN		LinearCoolRate100PerGain;

/* <S154> Start */
#if CSW_SVCF_CHKAMPLOL == 1
	CHECK_LOWOL *pLowOL;
	LONG	wk;
	LONG	s;
	LONG	calcwk;
	OLPRM	AmpLock,AmpLow;
	CHECK_OLP	*AmpLockOLP;
	LOWOLPRM	*LowOLPrm;
	LOWOLCHKP	*LowOLChkP;
	pLowOL    = &(Axis->CheckAlarm->AmpLowOL);
	AmpLockOLP = &(Axis->CheckAlarm->AmpOL.conf.LockArea);
	LowOLPrm = &(pLowOL->LowOLPrm);
	LowOLChkP = &(pLowOL->LowOLChkP);
#endif
/* <S154> End */

	Prm       = Axis->UniPrms->Prm;		// パラメータ設定データ
	Bprm      = Axis->UniPrms->Bprm;		// 基本パラメータ定義
	pMotOL    = &(Axis->CheckAlarm->MotOL);
	pAmpOL    = &(Axis->CheckAlarm->AmpOL);


	/********************************************************************************************/
	/*		過負荷基本パラメータ計算 ==> [0.1s], [15000/MaxTrq(Imax)]							*/
	/********************************************************************************************/
	/*	モータ側ＯＬ関連基本パラメータ計算	*/
	Mot.Tmid = (Prm->MencP.midolt * Prm->ratmtt_2.b.l);
	Mot.Tmax = (Prm->MencP.mtolt2 * Prm->ratmt2_os.b.l) / 10;
#if (FLOAT_USE==TRUE)
	ks = (float)100 * (float)Bprm->PerMaxTrq / (float)15000;
	ks2 = (float)10000 * (float)Bprm->PerMaxTrq / (float)(15000 * Prm->mtbastrq_drt);
	Mot.Ibse = ( (float)Prm->MencP.bastrq * (float)Prm->ratbt_mt.b.l / ks2 );
	Mot.Imid = ( (float)Prm->MencP.midtrq * (float)Prm->ratbt_mt.b.h / ks );
	Mot.Imax = ( (float)Prm->MencP.mtrq2 * (float)Prm->ratmtt_2.b.h / ks );
#else
	ks = MlibScalKxgain( 100, Bprm->PerMaxTrq, 15000, NULL, -1 );
	ks2 = MlibScalKxgain( 10000, Bprm->PerMaxTrq, 15000 * Prm->mtbastrq_drt, NULL, -1 );
	Mot.Ibse = MlibScalKxkxks( Prm->MencP.bastrq, Prm->ratbt_mt.b.l, ks2, NULL, -30 );
	Mot.Imid = MlibScalKxkxks( Prm->MencP.midtrq, Prm->ratbt_mt.b.h, ks, NULL, -30 );
	Mot.Imax = MlibScalKxkxks( Prm->MencP.mtrq2,  Prm->ratmtt_2.b.h, ks, NULL, -30 );
#endif /* FLOAT_USE */
	/*-------------------------------------------------------------------------------------------*/
	/*	アンプ側ＯＬ関連基本パラメータ計算	*/
	Amp.Tmid = (Prm->imdt_mxt.b.l * Prm->ratmtt_2.b.l);
	Amp.Tmax = (Prm->imdt_mxt.b.h * Prm->ratmt2_os.b.l) / 10;
#if (FLOAT_USE==TRUE)
	ks = (float)100 * (float)(Prm->MencP.irat*Bprm->PerMaxTrq) / (float)15000;
	/* <S1F5> Start */
	if ( pLowOL->conf.BaseCurrentDerateEnable == TRUE )
	{	/* 単相電源入力選択時のベース電流ディレーティングあり */
		ks2 = (float)10000 * (float)(Prm->MencP.irat*Bprm->PerMaxTrq) /
					(float)( 15000 * Prm->svbastrq_drt );
	}
	else
	{	/* 単相電源入力選択時のベース電流ディレーティングなし */
		ks2 = ks ;
	}
//	Amp.Ibse = ( (float)(Prm->ibs_md.b.l * Prm->irats) * Prm->ratbt_mt.b.l / ks );
	Amp.Ibse = ( (float)(Prm->ibs_md.b.l * Prm->irats) * Prm->ratbt_mt.b.l / ks2 );
	/* <S1F5> End */
	Amp.Imid = ( (float)(10*Prm->ibs_md.b.h * Prm->irats) * Prm->ratbt_mt.b.h / ks );
	Amp.Imax = ( (float)(Prm->ratmtt_2.b.h * Prm->imaxs) * Prm->ratmtt_2.b.h / ks );
#else
	ks = MlibScalKxgain( 100, (Prm->MencP.irat*Bprm->PerMaxTrq), 15000, NULL, -1 );
	/* <S1F5> Start */
	if ( pLowOL->conf.BaseCurrentDerateEnable == TRUE )
	{	/* 単相電源入力選択時のベース電流ディレーティングあり */
		ks2 = MlibScalKxgain( 10000, (Prm->MencP.irat*Bprm->PerMaxTrq), (15000 * Prm->svbastrq_drt), NULL, -1 );
	}
	else
	{	/* 単相電源入力選択時のベース電流ディレーティングなし */
		ks2 = ks;
	}
//	Amp.Ibse = MlibScalKxkxks( (Prm->ibs_md.b.l * Prm->irats), Prm->ratbt_mt.b.l, ks, NULL, -30 );
	Amp.Ibse = MlibScalKxkxks( (Prm->ibs_md.b.l * Prm->irats), Prm->ratbt_mt.b.l, ks2, NULL, -30 );
	/* <S1F5> End */
	Amp.Imid = MlibScalKxkxks( (10*Prm->ibs_md.b.h * Prm->irats), Prm->ratbt_mt.b.h, ks, NULL, -30 );
	Amp.Imax = MlibScalKxkxks( (Prm->ratmtt_2.b.h  * Prm->imaxs), Prm->ratmtt_2.b.h, ks, NULL, -30 );
#endif /* FLOAT_USE */

	/********************************************************************************************/
	/*		過負荷パラメータ整合性チェック														*/
	/********************************************************************************************/
	prmchk = TRUE;								/* prmchk：パラメータチェック確認フラグ			*/
	/*------------------------------------------------------------------------------------------*/
	/*	モータパラメータ整合性チェック															*/
	/*------------------------------------------------------------------------------------------*/
	if( Mot.Tmid <= Mot.Tmax )					/* 中間電流ＯＬ時間 <= 最大電流ＯＬ時間			*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_midolt.RegNumber );
		prmchk = FALSE;
	}
	if( Mot.Imid >= Mot.Imax )					/* 中間ＯＬ電流 > 最大ＯＬ電流					*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		prmchk = FALSE;
	}
	if( Mot.Imid <= Mot.Ibse )					/* 中間ＯＬ電流 < ベースＯＬ電流				*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_bastrq.RegNumber );
		prmchk = FALSE;
	}
	/*------------------------------------------------------------------------------------------*/
	/*	アンプパラメータ整合性チェック															*/
	/*------------------------------------------------------------------------------------------*/
	if( Amp.Tmid <= Amp.Tmax )					/* 中間電流ＯＬ時間 < 最大電流ＯＬ時間			*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_imdt_mxt.RegNumber );
		prmchk = FALSE;
	}
	if( Amp.Imid >= Amp.Imax )					/* 中間ＯＬ電流 > 最大ＯＬ電流					*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
		prmchk = FALSE;
	}
	if( Amp.Imid <= Amp.Ibse )					/* 中間ＯＬ電流 < ベースＯＬ電流				*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
		prmchk = FALSE;
	}

	/********************************************************************************************/
	/*	モータ自走冷却率パラメータ計算															*/
	/*		初期計算で必要なため初期設定値で計算												*/
	/********************************************************************************************/
//	IprmcalCoolRateGainMot( );
	/*------------------------------------------------------------------------------------------*/
	/*	アンプ自走冷却率パラメータ計算															*/
	/*------------------------------------------------------------------------------------------*/
//	pAmpOL->conf.CoolRateGain = 0;											/* 0固定			*/	/* <S154> */
	pAmpOL->conf.NormalArea.CoolRateGain = 0;								/* 0固定			*/	/* <S154> */

#if 0 /* 2009.09.25 Y.Oka 低速OLなし */
	if(LowOlChkP.LowSpdOLChkEnable == TRUE)		/* 低速ＯＬ検出実行フラグ：検出可？		*/
	{
	    LowOlChkP.AmpLockOLP.CoolRateGain = ChkAlmP.AmpOLP.CoolRateGain;
	}
#endif

	/*----------------------------------------------------------------------------------*/
	/*	モータ自走冷却率 100%計算														*/
	/*----------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
#if (FLOAT_USE==TRUE)
		LinearCoolRate100PerGain = 16384.0f / (float)Bprm->NorMaxSpd;
#else
		kx = MlibScalKxgain( 16384, 100, 100, &sx, 0 );
		LinearCoolRate100PerGain = MlibPcalKxgain( kx, 1, Bprm->NorMaxSpd, &sx, 24 );
#endif /* FLOAT_USE */
	}
	else
	{
		LinearCoolRate100PerGain = 0;
	}

	/********************************************************************************************/
	/*		過負荷チェック用パラメータ計算														*/
	/********************************************************************************************/
	if( prmchk == TRUE )						/* パラメータ設定異常の有無判別					*/
	{
		/*--------------------------------------------------------------------------------------*/
		/*	各過負荷チェック用パラメータ計算													*/
		/*--------------------------------------------------------------------------------------*/
		if( Amp.Imax > Mot.Imax )
		{
			/*----------------------------------------------------------------------------------*/
			/*	モータ過負荷チェック用パラメータ計算											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Mot, pMotOL, pMotOL, pMotOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Mot, &(pMotOL->conf.NormalArea), &(pMotOL->conf.NormalArea),&(pMotOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			{
				/* A.040 : パラメータ設定異常 */
				ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
			}
			/*----------------------------------------------------------------------------------*/
			/*	アンプ過負荷チェック用パラメータ計算											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Amp, pAmpOL, pMotOL, pAmpOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Amp, &(pAmpOL->conf.NormalArea), &(pMotOL->conf.NormalArea), &(pAmpOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */

			{
				/* A.040 : パラメータ設定異常 */
				ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
			}
		}
		else
		{
			/*----------------------------------------------------------------------------------*/
			/*	アンプ過負荷チェック用パラメータ計算											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Amp, pAmpOL, pAmpOL, pAmpOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Amp, &(pAmpOL->conf.NormalArea), &(pAmpOL->conf.NormalArea), &(pAmpOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			{
				/* A.040 : パラメータ設定異常 */
				ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
			}
			/*----------------------------------------------------------------------------------*/
			/*	モータ過負荷チェック用パラメータ計算											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Mot, pMotOL, pAmpOL, pMotOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Mot, &(pMotOL->conf.NormalArea), &(pAmpOL->conf.NormalArea), &(pMotOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			{
				/* A.040 : パラメータ設定異常 */
				ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
			}
		}
/* <S154> Start */
#if CSW_SVCF_CHKAMPLOL == 1 			/* CSW_SVCF_CHKAMPLOL：機種定義（機種による低速ＯＬ機能の有無）	*/
		/****************************************************************************************/
		/*		アンプ低速過負荷実行条件判別													*/
		/****************************************************************************************/
		/*		1.( (PnE90.L==0)&&(PnE90.H==0) )：PnE90が両方とも0								*/
		/*			→各区間の変曲点速度がないため、低速ＯＬ実行ＮＧ							*/
		/*		2.( PnE90.L <= PnE90.H )：ロック区間変曲点速度≦低速区間変曲点速度				*/
		/*			→変曲点速度の大小関係が逆のため、低速ＯＬ実行ＮＧ							*/
		/*		3.( Amp.Ibse >= 0x4000 )：連続ＯＬベース電流が最大電流(15000≒16384)以上		*/
		/*			→ＯＬカーブが正規化電流以上の範囲となるため、低速ＯＬ実行ＮＧ				*/
		/*			（計算しても意味のないＯＬカーブになる）									*/
		/*	⇒低速ＯＬ実行ＮＧの場合は通常の過負荷検出を行う。低速ＯＬパラメータ計算しない。	*/
		/*--------------------------------------------------------------------------------------*/
		if(	((Prm->olspdlvl.b.l!= (UCHAR)0)||(Prm->olspdlvl.b.h!= (UCHAR)0)) && ( Prm->olspdlvl.b.l > Prm->olspdlvl.b.h )
			&& ( Amp.Ibse < 0x4000 ) )
		{
			/************************************************************************************/
			/*		アンプ低速過負荷  検出実行フラグ処理										*/
			/************************************************************************************/
			/*	低速ＯＬ検出実行フラグをTRUEにする：検出可										*/
			/*		LowSpdOLChkEnableで低速ＯＬの検出可否をコントロールする。					*/
			/*----------------------------------------------------------------------------------*/
			LowOLChkP->conf.LowSpdOLChkEnable = (UCHAR)TRUE;			/* 低速ＯＬ検出実行フラグ：検出可		*/

			/************************************************************************************/
			/*		アンプ低速過負荷  ロック区間パラメータ計算									*/
			/************************************************************************************/
			/*----------------------------------------------------------------------------------*/
			/*	アンプ低速ＯＬ：ロック区間側基本パラメータ計算									*/
			/*----------------------------------------------------------------------------------*/
			/*	Tmid：ロック区間中間ＯＬ時間…通常区間と同じ									*/
			/*	Imid：ロック区間中間ＯＬ電流…下記式にて算出									*/
			/*			式：(PnE91.L*PnE14*PnE60.H*15000)/(PnF09*100*Bprm.PerMaxTrq)			*/
			/*	Tmax：ロック区間最大ＯＬ時間…下記式にて算出									*/
			/*			式：(PnE91.H*PnE62.L)/100												*/
			/*	Imax：ロック区間最大ＯＬ電流…通常区間と同じ									*/
			/*	Ibse：ロック区間連続ＯＬベース電流…通常区間と同じ								*/
			/*----------------------------------------------------------------------------------*/
			AmpLock.Tmid = Amp.Tmid;
			ks = MlibScalKxgain( 100, (LONG)(Prm->MencP.irat*Bprm->PerMaxTrq), 15000, NULL, -1 );
			AmpLock.Imid = MlibScalKxkxks( (LONG)(Prm->ollspdprm.b.l * Prm->irats), (LONG)Prm->ratbt_mt.b.h, ks, NULL, -30 );
			AmpLock.Tmax = MlibScalKxgain( (LONG)Prm->ollspdprm.b.h, (LONG)Prm->ratmt2_os.b.l, 100, NULL, -30 );
			AmpLock.Imax = Amp.Imax;
			AmpLock.Ibse = Amp.Ibse;
			LowOLPrm->AmpLock = AmpLock;

			/*----------------------------------------------------------------------------------*/
			/*	アンプ低速ＯＬ：ロック区間チェック用パラメータ計算								*/
			/*----------------------------------------------------------------------------------*/
			if( LpxPcalOverLoadLevelAmpMot( &AmpLock, AmpLockOLP , &(pMotOL->conf.NormalArea),
				&(pAmpOL->conf.NormalArea) , Prm , LinearCoolRate100PerGain) != TRUE )
			{
				ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );		/* A.040 : パラメータ設定異常			*/
			}

			/*----------------------------------------------------------------------------------*/
			/*	アンプ低速ＯＬ：ロック区間処理（通常区間とロック区間のＯＬ検出レベルの統一）	*/
			/*----------------------------------------------------------------------------------*/
			AmpLockOLP->AlmLvlmid = pAmpOL->conf.NormalArea.AlmLvlmid;/* 連続ＯＬﾚﾍﾞﾙ			*/
			AmpLockOLP->AlmLvlmax = pAmpOL->conf.NormalArea.AlmLvlmax;/* 瞬時ＯＬﾚﾍﾞﾙ			*/
			AmpLockOLP->WrnLvlmid = pAmpOL->conf.NormalArea.WrnLvlmid;/* 連続ＯＬﾜｰﾆﾝｸﾞﾚﾍﾞﾙ		*/
			AmpLockOLP->WrnLvlmax = pAmpOL->conf.NormalArea.WrnLvlmax;/* 瞬時ＯＬﾜｰﾆﾝｸﾞﾚﾍﾞﾙ		*/

			/************************************************************************************/
			/*		アンプ低速過負荷  低速区間パラメータ計算									*/
			/************************************************************************************/
			/*----------------------------------------------------------------------------------*/
			/*	アンプ低速ＯＬ：低速区間側基本パラメータ計算									*/
			/*----------------------------------------------------------------------------------*/
			/*	Tmid：低速区間中間ＯＬ時間…通常区間と同じ										*/
			/*	Imid：低速区間中間ＯＬ電流…ｵﾝﾗｲﾝで算出											*/
			/*	Tmax：低速区間最大ＯＬ時間…ｵﾝﾗｲﾝで算出											*/
			/*	Imax：低速区間最大ＯＬ電流…通常区間と同じ										*/
			/*	Ibse：低速区間連続ＯＬベース電流…通常区間と同じ								*/
			/*----------------------------------------------------------------------------------*/
			AmpLow.Tmid = Amp.Tmid;
			AmpLow.Imax = Amp.Imax;
			AmpLow.Ibse = Amp.Ibse;
			LowOLPrm->AmpLow = AmpLow;

			/*----------------------------------------------------------------------------------*/
			/*	アンプ低速ＯＬ：低速区間補間演算用係数計算１									*/
			/*----------------------------------------------------------------------------------*/
			/*	calcwk			：変曲点速度換算ゲイン											*/
			/* 	LowSpdOLchg		：低速区間変曲点速度(PnE90.L)	[0.1Hz]→[2^24/OvrSpd]			*/
			/*	LockSpdOLchg	：ロック区間変曲点速度(PnE90.H) [0.1Hz]→[2^24/OvrSpd]			*/
			/*	LowSpdOffsetImid：ロック区間変曲点中間ＯＬ電流	[0.1sec]						*/
			/*	LowSpdOffsetTmax：ロック区間変曲点最大ＯＬ時間	[15000/MaxTrq]					*/
			/*----------------------------------------------------------------------------------*/
			calcwk = MlibScalKxgain( 1, 1, 10, &s, 0 );
			calcwk = MlibPcalKxkxks( calcwk, 1, Bprm->Keangle, &s, 0 );
			calcwk = MlibPcalKxgain( calcwk, C2PAIE7, 10000000, &s, 0 );
			calcwk = MlibPcalKxkxks( calcwk, 0x1000000, Bprm->OvrSpd, &s, 24 );
			LowOLChkP->conf.LowSpdOLchg = MlibMulgain((LONG)Prm->olspdlvl.b.l, calcwk);
			LowOLChkP->conf.LockSpdOLchg = MlibMulgain((LONG)Prm->olspdlvl.b.h, calcwk);
			LowOLChkP->conf.LowSpdOffsetImid = AmpLock.Imid;
			LowOLChkP->conf.LowSpdOffsetTmax = AmpLock.Tmax;

			/*----------------------------------------------------------------------------------*/
			/*	アンプ低速ＯＬ：低速区間補間演算用係数計算２									*/
			/*----------------------------------------------------------------------------------*/
			/*	TransOLImid：[電気角速度]→[中間ＯＬ電流]補間係数								*/
			/*					(通常区間中間OL電流―ロック区間中間OL電流)						*/
			/*			＝	----------------------------------------------------				*/
			/*					(低速区間変曲点速度―ロック区間変曲点速度)						*/
			/*																					*/
			/*	TransOLTmax：[電気角速度]→[ＯＬ検出時間]補間係数								*/
			/*						(瞬時中間OL時間―低速区間最大OL時間)						*/
			/*			＝	---------------------------------------------------					*/
			/*					√(低速区間変曲点速度―ロック区間変曲点速度)					*/
			/*		・瞬時中間OL時間(PnE18.H[sec])			:	imdt_mxt.b.h					*/
			/*		・低速区間最大OL時間(PnE91.H[0.1sec])	:	ollspdprm.b.h					*/
			/*----------------------------------------------------------------------------------*/
			wk = LowOLChkP->conf.LowSpdOLchg - LowOLChkP->conf.LockSpdOLchg;
			LowOLChkP->conf.TransOLImid = MlibScalKxgain( (Amp.Imid - AmpLock.Imid), 1, wk, NULL, 24 );
			wk = (LONG)MlibSqrtu32( (ULONG)LowOLChkP->conf.LowSpdOLchg - (ULONG)LowOLChkP->conf.LockSpdOLchg );
			LowOLChkP->conf.TransOLTmax = MlibScalKxgain( (((LONG)Prm->imdt_mxt.b.h*10) - (LONG)Prm->ollspdprm.b.h), 1, wk, NULL, 24 );
		}
		else
		{
			/************************************************************************************/
			/*		アンプ低速過負荷  検出実行フラグ処理										*/
			/************************************************************************************/
			/*	低速ＯＬ検出実行フラグをFALSEにする：検出不可									*/
			/*		LowSpdOLChkEnableで低速ＯＬの検出可否をコントロールする。					*/
			/*----------------------------------------------------------------------------------*/
			LowOLChkP->conf.LowSpdOLChkEnable = (UCHAR)FALSE;		/* 低速ＯＬ検出実行フラグ：検出不可		*/
		}
#endif
/* <S154> End */
	}
}


/****************************************************************************************************/
/*																									*/
/*		モータ暴走監視パラメータ計算(回転形モータ)													*//* <V051> */
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 暴走監視トルクレベル及び速度レベルの計算を行う											*/
/*																									*/
/*			暴走監視トルク		：PnE44.h															*/
/*			暴走監視速度レベル	：オーバースピードの20%												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalMotorOverrun( AXIS_HANDLE *Axis )
{
	ULONG			work1;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	CHECK_OVRRUN	*pOvrRun;

	Prm = Axis->UniPrms->Prm;		// パラメータ設定データ
	Bprm = Axis->UniPrms->Bprm;		// 基本パラメータ定義
	pOvrRun = &(Axis->CheckAlarm->OvrRun);
	work1 = Prm->ipg_rwt.b.h;

	/* 暴走監視トルク指令レベル */
	if ( work1 > (ULONG)Bprm->PerMaxTrq )
	{
		pOvrRun->OvrTrqLevel = 0x01000000;
	}
	else
	{
#if (FLOAT_USE==TRUE)
		pOvrRun->OvrTrqLevel = work1 * 0x01000000 / Bprm->PerMaxTrq;
#else
		pOvrRun->OvrTrqLevel = MlibScalKxgain( work1, 0x01000000, Bprm->PerMaxTrq, NULL, -24);
#endif /* FLOAT_USE */

	}

	/* 暴走監視モータ速度レベル */
	pOvrRun->OvrSpdLevel = 0x01000000 / 5;

}


/****************************************************************************************************/
/*																									*/
/*		入力信号タイミング最大許容時間の計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn620 : USHORT	bbondiff0;			STOP信号タイミング最大許容時間（Pn620）[10ms]			*/
/*		PnE4C : USHORT	bbondiff1;			内部信号タイミング最大許容時間（PnE4C）[10ms]			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalMaxBbonTimeDiff( AXIS_HANDLE *Axis )
{
	CHECK_HWBB_TIMING *HwbbTiming;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;		// パラメータ設定データ
	HwbbTiming = &(Axis->CheckAlarm->HwbbTiming);
	HwbbTiming->conf.MaxBbonTimeDiff = 10 * Prm->bbondiff0;
}


/****************************************************************************************************/
/*																									*/
/*		位置指令過速度(ＯＳ)の計算																	*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalPcmdOvrSpd( AXIS_HANDLE *Axis )
{
#if 0	// 現状未対応
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	IPRMDAT		*Iprm;
#if (FLOAT_USE==TRUE)
	float		fw;
#else
	LONG		kx, sx;
#endif /* FLOAT_USE */

	Prm = Axis->UniPrms->Prm;		// パラメータ設定データ
	Bprm = Axis->UniPrms->Bprm;		// 基本パラメータ定義
	Iprm = Axis->UniPrms->iprm_ptr;		// 基本パラメータ定義

#if (FLOAT_USE==TRUE)
	fw = Bprm->OvrSpd / Bprm->Kfbpls;
	fw = fw * (float)Prm->rata2l / (float)Prm->ratb2l;
	Iprm->PcmdOvrSpd = fw * (float)Iprm->SvCycleNs / 1000000000.0F;
#else

	kx = MlibScalKxksks( 1,  Bprm->OvrSpd,    Bprm->Kfbpls,  &sx,  0 );
	kx = MlibPcalKxgain( kx, Prm->rata2l,     Prm->ratb2l,   &sx,  0 );
	kx = MlibPcalKxgain( kx, Iprm->SvCycleNs, 1000000000,    &sx, 24 );
	Iprm->PcmdOvrSpd = kx;
#endif /* FLOAT_USE */
#endif
}


/****************************************************************************************************/
/*																									*/
/*		トルクレベルの計算	<S00C>																	*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalTorqueLevel( AXIS_HANDLE *Axis )
{
	BPRMDAT		*Bprm;

	Bprm = Axis->UniPrms->Bprm;		// 基本パラメータ定義

	Bprm->TrefonLevel = MlibScalKxgain( 0x01000000, 10, Bprm->PerMaxTrq, NULL, -24 );
}

/****************************************************************************************************/
/*																									*/
/*		オーバトラベルマスク 	暫定！！！<S032>																*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalOtMask( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	Prm = Axis->UniPrms->Prm;

	Axis->IoSignals->pot_mask = (( Prm->insel1 & 0xF000 ) == 0x8000)? 0 : 1;
	Axis->IoSignals->not_mask = (( Prm->insel2 & 0x000F ) == 0x0008)? 0 : 1;

}

/****************************************************************************************************/
/*																									*/
/*		パワー基板関連設定																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0C.Bit0 : 回生抵抗内蔵																	*/
/*		PnE0C.Bit4 : 突入/ＤＢ制御方式																*/
/*		PnE0C.Bit5 : 突入/ＤＢ制御方式																*/
/*		PnE0C.Bit6 : 回生過負荷検出用初期値割合選択													*/
/*		PnE0C.Bit7 : 回生過負荷検出用初期値割合選択													*/
/*		PnE0C.Bit8 : 主回路母線電圧検出方式															*/
/*		PnE0C.Bit9 : 主回路母線電圧検出方式															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		主回路検出パラメータ整合性確認																*/
/*			以下の条件成立時、異常とする。異常時、回生動作禁止フラグをオンする。					*/
/*				ＰＷＭ方式の場合：主回路検出最大値 > PWM出力幅(KPX_VDET_PWM_MAX)					*/
/*				ＡＤ方式の場合　：主回路検出最大値 > AD変換機最大読出し値(KPX_DCVOLT_ADMAX)			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalPowerCircuitSetting( AXIS_HANDLE *Axis )
{
//	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	CMN_PRMDATA			*Cprm;
	REGENE_CTRL			*RegCtrl;
	DET_DC_VOLT			*DetectDc;
	CHECK_MAIN_POWER	*PowerChecker;

	Bprm 	= Axis->UniPrms->Bprm;		// 基本パラメータ定義
	Cprm	= Axis->UniPrms->Cprm;
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	DetectDc = &(Axis->PowerManager->DetectDcVolt);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	/* 初期化 */
	DetectDc->conf.VdetPrmOk = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		PnE0C : システムスイッチ																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( Cprm->syssw2 & 0x0001 )
	{
		RegCtrl->conf.RegRxIn = TRUE;
	}
	else
	{
		RegCtrl->conf.RegRxIn = FALSE;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		bit4,5 : 突入/DB制御方法																*/
	/*			00	突入+DB：リレー(×1)															*/
	/*			01 	突入：リレー；DB:サイリスタ														*/
	/*			10	突入：リレー；DB：リレー														*/
	/*			11	突入+DB：リレー+サイリスタ														*/
	/*----------------------------------------------------------------------------------------------*/
	switch( (Cprm->syssw2 >> 4) & 0x03 )
	{
	case 0x00:
		PowerChecker->conf.RlyDbWay = DBCTRL_RLY_COMMON;
		break;
	case 0x01:
		PowerChecker->conf.RlyDbWay = DBCTRL_TRIACK_SEPARATE;
		break;
	case 0x02:
		PowerChecker->conf.RlyDbWay = DBCTRL_RLY_SEPARATE;
		break;
	case 0x03:
		PowerChecker->conf.RlyDbWay = DBCTRL_TRIACK_COMMON;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		bit6,7 : 回生過負荷検出用初期値割合選択													*/
	/*			00：RgolAlmLevel * 0.95																*/
	/*			01：RgolAlmLevel * 0.50																*/
	/*			10：予約 (01と同じ)																	*/
	/*			11：予約 (01と同じ)																	*/
	/*----------------------------------------------------------------------------------------------*/
	switch( (Cprm->syssw2 >> 6) & 0x03 )
	{
	case 0x00:
		RegCtrl->RegOL.conf.RgSumSel = REGPOWSUM_RATE95PER;
		break;
	case 0x01:
	case 0x02:
	case 0x03:
		RegCtrl->RegOL.conf.RgSumSel = REGPOWSUM_RATE50PER;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		Bit8,9 : 主回路母線電圧検出方式															*/
	/*----------------------------------------------------------------------------------------------*/
	switch((UCHAR)( Cprm->syssw2 >> 8 ) & 0x03 )
	{
		case 0x01:									/* PWM-コンパレータ方式 : 固定値(1)による追従	*/
			DetectDc->conf.VdetMode = VDET_PWM_FIX_FOLLOW;
			break;
		case 0x02:									/* PWM-コンパレータ方式 : 可変値(1-3)による追従	*/
			DetectDc->conf.VdetMode = VDET_PWM_VAR_FOLLOW;
//			Iprm->VdetFollowWidthMax = 0x03;		/* 追従幅最大加算幅								*/
			break;
		case 0x03:									/* A/D変換方式									*/
			DetectDc->conf.VdetMode = VDET_AD;
			break;
		default:									/* ソフトウェアによる検出なし					*/
			DetectDc->conf.VdetMode = VDET_NONE;
//			Iprm->VdetFollowWidthMax = 0;

			/* 検出なし時、パラメータエラー */
			ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＨＷによる上限値とソフト検出上限値の関係チェック											*/
/*--------------------------------------------------------------------------------------------------*/
	switch( DetectDc->conf.VdetMode )
	{
	/*------------------------------------------------------------------------------------------*/
	/*		ＰＷＭ方式の場合																	*/
	/*------------------------------------------------------------------------------------------*/
		case VDET_PWM_FIX_FOLLOW:
		case VDET_PWM_VAR_FOLLOW:
			if( Bprm->VdetMax > KPX_VDET_PWM_MAX )
			{
				DetectDc->conf.VdetPrmOk = FALSE;
			}
			break;
	/*------------------------------------------------------------------------------------------*/
	/*		ＡＤ方式の場合																		*/
	/*------------------------------------------------------------------------------------------*/
		case VDET_AD:
			if( Bprm->VdetMax > KPX_DCVOLT_ADMAX )
			{
				DetectDc->conf.VdetPrmOk = FALSE;
			}
			break;
	/*------------------------------------------------------------------------------------------*/
	/*		ソフトによる電圧検出なしの場合														*/
	/*------------------------------------------------------------------------------------------*/
		default:
			DetectDc->conf.VdetPrmOk = FALSE;
			break;
	}

	if( DetectDc->conf.VdetPrmOk == FALSE )
	{
		ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
	}

}


/****************************************************************************************************/
/*																									*/
/*		コピーパラメータ(Prm --> Iprm)																*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalCopyParamToIprm( AXIS_HANDLE *Axis )
{
#if 0
	PRMDATA		*Prm;
	IPRMDAT		*Iprm;

	Prm = Axis->UniPrms->Prm;		// 基本パラメータ定義
	Iprm = Axis->UniPrms->iprm_ptr;	// 基本パラメータ定義

	Iprm->mtlmt = Prm->mtlmt;		/* マルチターンリミット設定						*/
#endif
}



/****************************************************************************************************/
/*																									*/
/*		位置指令電子ギヤ演算																		*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalElectronicGear( AXIS_HANDLE *Axis )
{
	EGEAR		*Egear;
	EGEAR		*InvalidEgear;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	LONG		Ratb_a, Rata_b;
	LONG		i, sht;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	Egear = &(Axis->BaseControls->PosManager.conf.Egear);
	InvalidEgear = &(Axis->BaseControls->PosManager.conf.InvalidEgear);

/*--------------------------------------------------------------------------------------------------*/
/*		電子ギヤ演算																				*/
/*--------------------------------------------------------------------------------------------------*/
	MlibPcalKegear( Prm->rata2l, Prm->ratb2l, Egear );

/*--------------------------------------------------------------------------------------------------*/
/*		演算エラーチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
	Ratb_a = (Egear->k2 == 0) ? Egear->k1 : Egear->k1 + 1;
	Rata_b = (Egear->g2 == 0) ? Egear->g1 : Egear->g1 + 1;

	if( ( Prm->rata2l > 0x40000000 ) || ( Prm->ratb2l > 0x40000000 ) ||
		( Egear->a == 0 ) || ( Egear->b == 0 ) ||
//		( Ratb_a > 4000 ) || ( Rata_b > 1000 ) )	/* <S17C> */
		( Ratb_a > 64000 ) || ( Rata_b > 1000 ) )	/* <S17C> 上限値変更(4000⇒64000) */
	{
		MlibPcalKegear( 1, 1, Egear );
		ALMSetPramError( Axis->AlmManager, pndef_rata2l.RegNumber );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差モニタ用時間補正演算																*/
/*--------------------------------------------------------------------------------------------------*/
	sht = 0;
	for(i = 2; i <= (Bprm->SvCycleNs/CFG_POSERR_DELAY); i*=2)
	{	sht++;}
	Axis->BaseControls->PosManager.conf.PosErrDelayShift = sht;
/*--------------------------------------------------------------------------------------------------*/
/*		電子ギア機能無効用ギア比演算																*/
/*--------------------------------------------------------------------------------------------------*/
	MlibPcalKegear( 1, 1, InvalidEgear );
}


/****************************************************************************************************/
/*																									*/
/*		アラームマスク設定(PnE05/E06)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : アラームマスクパラメータ(PnE05/PnE06)に従って、アラームマスクフラグを設定する。			*/
/*																									*/
/*																									*/
/*	補 足 : アラームマスク解除機能は不要と思われるが、とりあえず、ＳＧＤＳに合わせておく。			*/
/*																									*/
/*			1) servo_alarm_mask( ) : アラームマスクの設定／解除(引数により設定／解除を指定)			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalAlarmMask( AXIS_HANDLE *Axis )
{
	ULONG		Mskx;
	PRMDATA		*Prm;

	Prm		= Axis->UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		アラームマスク : PnE05																		*/
/*--------------------------------------------------------------------------------------------------*/
	Mskx = Prm->almmsk1;
	/* エンコーダ接続異常ですでにモータ/サーボ容量アンマッチ(A.050)がマスクされているかのチェック *//* <S19C> */
	if( !ALMCheckSetMask( Axis->AlmManager, ALM_UNMATCH ) )
	{	/* マスクされていない場合のみPnE05のマスクビットの反映 */
		ALMSetMask(Axis->AlmManager, ALM_UNMATCH,	((Mskx & 0x0001)!=0));/* A.050 : タイプアンマッチ			*/
	}	/* <S19C> */
	ALMSetMask(Axis->AlmManager, ALM_RGOL,		((Mskx & 0x0002)!=0));/* A.320 : 回生過負荷 				*/
	ALMSetMask(Axis->AlmManager, ALM_PUV,		((Mskx & 0x0004)!=0));/* A.410 : 不足電圧 					*/
	ALMSetMask(Axis->AlmManager, ALM_OSF1,		((Mskx & 0x0008)!=0));/* A.510 : 過速度(ＯＳ)				*/
	if((Mskx & 0x0010)!=0)
	{	ALMSetMask(Axis->AlmManager, ALM_OLF1,	TRUE);}				  /* A.710 : 過負荷(瞬時最大負荷)		*/
	if((Mskx & 0x0020)!=0)
	{	ALMSetMask(Axis->AlmManager, ALM_OLF2,	TRUE);}				  /* A.720 : 過負荷(連続最大負荷)		*/
	ALMSetMask(Axis->AlmManager, ALM_DBOL,		((Mskx & 0x0040)!=0));/* A.730 : ＤＢ過負荷 				*/
	ALMSetMask(Axis->AlmManager, ALM_RSOL,		((Mskx & 0x0080)!=0));/* A.740 : 突入抵抗過負荷 			*/
	ALMSetMask(Axis->AlmManager, ALM_RWF,		((Mskx & 0x0100)!=0));/* A.C10 : 暴走防止検出				*/
	ALMSetMask(Axis->AlmManager, ALM_OF,		((Mskx & 0x0200)!=0));/* A.D00 : 位置偏差過大 				*/
	ALMSetMask(Axis->AlmManager, ALM_PWPHASE,	((Mskx & 0x0400)!=0));/* A.F10 : 電源欠相			 		*/
	ALMSetMask(Axis->AlmManager, ALM_FSOF,		((Mskx & 0x0800)!=0));/* A.D10 : モータ-負荷位置間偏差過大	*/
	ALMSetMask(Axis->AlmManager, ALM_OPSET,		((Mskx & 0x1000)!=0));/* A.E00 : 同期Acc要求受信TimeOut		*/
	ALMSetMask(Axis->AlmManager, ALM_TIMEOUT,	((Mskx & 0x1000)!=0));/* A.E01 : 運転開始要求受信TimeOut	*/
	ALMSetMask(Axis->AlmManager, ALM_WDC_ERR,	((Mskx & 0x2000)!=0));/* A.E02 : WDC＋SyncFalg同期異常		*/
	ALMSetMask(Axis->AlmManager, ALM_WIR,		((Mskx & 0x4000)!=0));/* A.330 : 主回路配線エラー			*/
	ALMSetMask(Axis->AlmManager, ALM_VREFADCHK,	((Mskx & 0x8000)!=0));/* A.B11 : 速度指令ＡＤデータ異常		*/

/*--------------------------------------------------------------------------------------------------*/
/*		アラームマスク : PnE06																		*/
/*--------------------------------------------------------------------------------------------------*/
	Mskx = Prm->almmsk2;
	ALMSetMask(Axis->AlmManager, ALM_NOTSUPPORT,((Mskx & 0x0001)!=0));/* A.051 : 製品未サポート				*/
	ALMSetMask(Axis->AlmManager, ALM_VREFADERR, ((Mskx & 0x0002)!=0));/* A.B10 : 速度指令用ＡＤ異常			*/
	ALMSetMask(Axis->AlmManager, ALM_POSOF,     ((Mskx & 0x0004)!=0));/* A.D30 : 位置データオーバーフロー	*/
	ALMSetMask(Axis->AlmManager, ALM_FANLOCK,	((Mskx & 0x0008)!=0));/* A.7AB : サーボパック内蔵ファン停止	*/
	ALMSetMask(Axis->AlmManager, ALM_CTRLBRD,	((Mskx & 0x0010)!=0));/* A.7A1 : 内部温度異常1(制御基板)	*//* <S0DE> */
	ALMSetMask(Axis->AlmManager, ALM_PWRBRD,	((Mskx & 0x0010)!=0));/* A.7A2 : 内部温度異常2(パワー基板)	*//* <S0DE> */

/*--------------------------------------------------------------------------------------------------*/
/*		モータレステストモードアラームマスク処理	<S1B7>											*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )				/* モータレステスト時の処理			*/
	{
		ALMSetMask(Axis->AlmManager, ALM_DBOL, TRUE);					/* A.730 : ＤＢ過負荷 				*/
		ALMSetMask(Axis->AlmManager, ALM_DBOL_LVL, TRUE);				/* A.731 : ＤＢ過負荷 				*/
	}

}



/****************************************************************************************************/
/*																									*/
/*		システムスイッチ設定(PnE0B/E0C/E0D)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0B.Bit0 : セミクローズ/フルクローズ形式選択												*/
/*		PnE0B.Bit1 : 回転型/リニア型起動選択														*/
/*		PnE0B.Bit4 : CA,CBフィルタ設定		--> JL056HW.c											*/
/*		PnE0B.Bit5 : CA,CBフィルタ設定		--> JL056HW.c											*/
/*		PnE0B.Bit8 : 指令パルスフィルタ設定 --> JL056HW.c											*/
/*		PnE0B.Bit9 : 指令パルスフィルタ設定 --> JL056HW.c											*/
/*		PnE0B.BitA : 指令パルスフィルタ設定 --> JL056HW.c											*/
/*		PnE0B.BitB : 指令パルスフィルタ設定 --> JL056HW.c											*/
/*--------------------------------------------------------------------------------------------------*/
/*		PnE0B.BitC : 速度制御積分保持																*/
/*		PnE0B.BitD : ｼﾘｱﾙｴﾝｺｰﾀﾞｸﾛｸﾄｰｸ		--> JL056HW.c											*/
/*		PnE0B.BitE : エンコーダ位置補正選択1														*/
/*		PnE0B.BitF : エンコーダ位置補正選択2														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0C.Bit1 : 電源入力３相																	*/
/*		PnE0C.Bit2 : 電源欠相検出方式																*/
/*		PnE0C.BitB : ヒートシンク過熱検出方式														*/
/*		PnE0C.BitC : 温度上昇ＤＢ過負荷検出有効														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0D.Bit0 : ユーザ定数書込み禁止															*/
/*		PnE0D.Bit1 : ユーザアクセスレベル選択														*/
/*		PnE0D.Bit4 : 原点位置設定実行																*/
/*		PnE0D.Bit5 : 電流零位相位置保存実行															*/
/*		PnE0D.Bit8 : 分周出力選択																	*//*<S02D>*/
/*		PnE39.Bit2 : 主回路電圧補正機能選択		<V079>												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalSystemSwitch( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	CMN_PRMDATA			*Cprm;
	FUN_AXCOMMON	*AxCommon;				/* <S056> */

	Prm		= Axis->UniPrms->Prm;
	Cprm	= Axis->UniPrms->Cprm;
	AxCommon = Axis->FnManager->AxCommon;	/* <S056> */


/*--------------------------------------------------------------------------------------------------*/
/*		PnE0B : システムスイッチ																	*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		BitC : 速度制御積分保持																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->syssw1 & 0x1000 )
	{
		Axis->BaseControls->CtrlCmdPrm.SpdIctrlKeep = TRUE;
	}
	else
	{
		Axis->BaseControls->CtrlCmdPrm.SpdIctrlKeep = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		BitE : エンコーダ位置補正選択(0:補正する,1:補正しない)									*/
	/*		BitF : エンコーダ位置補正選択(0:JL-086にて処理(通常処理),1:SWにて処理(Σ-Ⅴ互換))		*/
	/*----------------------------------------------------------------------------------------------*//*<S0C5>rename*/
	if( Prm->syssw1 & 0x4000 )
	{	/* 位置補正しない */
		(Axis->MencV)->PosCompMode = POSCOMP_NOP;
	}
	else
	{	/* 位置補正する */
		if( Prm->syssw1 & 0x8000 )
		{	/* SWにて処理		*/
			(Axis->MencV)->PosCompMode = POSCOMP_SW;
		}
		else
		{	/* JL-086にて処理 	*/
			(Axis->MencV)->PosCompMode = POSCOMP_ASIC;
		}
	}
	(Axis->FencV)->PosCompMode = (Axis->MencV)->PosCompMode;		/* <S04D> */
/*--------------------------------------------------------------------------------------------------*/
/*		PnE0C : システムスイッチ																	*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		Bit12 : 温度上昇によるＤＢ過負荷検出有無												*/
	/*----------------------------------------------------------------------------------------------*/
	if( Cprm->syssw2 & 0x1000 )
	{
		(Axis->CheckAlarm)->DbOL.conf.DbOHeatCheck = TRUE;
	}
	else
	{
		(Axis->CheckAlarm)->DbOL.conf.DbOHeatCheck = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		PnE0D : システムスイッチ																	*/
/*--------------------------------------------------------------------------------------------------*/

/* <S056>	Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		Bit1 : ユーザアクセスレベル選択															*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->syssw3  & 0x0002 )	/* PnE0D.1 = 1 ユーザレベル3 */
	{
//		AxCommon->AccessLevel = 2;	/* ACCLVL_USER2 */	/* <S1B0> */
		AxCommon->AccessLevel = 3;	/* ACCLVL_USER3 */	/* <S1B0> */

	}
	else	/* PnE0D.1 = 0 ユーザレベル1 */
	{
//		AxCommon->AccessLevel = 1;	/* ACCLVL_USER1 */
		AxCommon->AccessLevel = 2;	/* ACCLVL_USER2 *//* <S20B> */
	}
/* <S056>	End */

	/*----------------------------------------------------------------------------------------------*/
	/*		Bit8 : 分周出力選択	(1:JL-086にて処理,0:SWにて処理(Σ-Ⅴ互換))							*//*<S02D>*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->syssw3 & 0x0100 )
	{
		(Axis->MencV)->DivInfo.DivOutASIC = TRUE; 
	}
	else
	{
		(Axis->MencV)->DivInfo.DivOutASIC = FALSE;
	}

}


/****************************************************************************************************/
/*																									*/
/*		ゼロ速停止制御パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ゼロ速停止制御用の初期パラメータ計算を行う												*/
/*																									*/
/*			1) 原点サーチ速度																		*/
/*			   原点サーチで使用するＪＯＧ速度の計算を行う。											*/
/*																									*/
/*			2) 速度指令リミット																		*/
/*			   ＯＳ割合の半分の割合で、速度指令リミットを0x1000000／ＯＳ速度に正規化する。			*/
/*			   本速度指令リミットは、ゼロクランプ及び原点サーチにて使用する。						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalZeroStopControl( AXIS_HANDLE *Axis )
{
	LONG	SpeedX;
	BPRMDAT	*Bprm;

	Bprm	= Axis->UniPrms->Bprm;

/*--------------------------------------------------------------------------------------------------*/
/*		原点サーチ速度の計算 : 60.0r/min, 15.0mm/s --> NorSpeed [2^24/OvrSpd]						*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		SpeedX = 15;											/* リニア型モータ : 15.0 r/min		*/
	}
	else
	{ /* 回転型 */
		SpeedX = 60;											/* 回転型モータ : 60.0 r/min		*/
	}
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->LowSpdMotor )
	{ /* 低速モータ(ＤＤモータ) */
		;
	}
	else
	{ /* 低速モータ(ＤＤモータ)以外 */
		SpeedX = 10 * SpeedX;
	}
/*--------------------------------------------------------------------------------------------------*/
	/* 正規化速度計算 [2^24/OvrSpd] */
	Axis->BaseControls->ZeroClampCtrl.conf.OrgSrchSpeed = BprmSpeedLevelCal( Bprm, SpeedX, 0 );

/*--------------------------------------------------------------------------------------------------*/
/*		速度指令リミットの計算																		*/
/*--------------------------------------------------------------------------------------------------*/
	Axis->BaseControls->ZeroClampCtrl.conf.SpdLimit = 0x01000000 - (Bprm->PerOvrSpd * 838);
	Axis->BaseLoops->ZxCtrl.P.SpdLimit = Axis->BaseControls->ZeroClampCtrl.conf.SpdLimit;			/* <S１C2> */
}



/****************************************************************************************************/
/*																									*/
/*		キャリア周波数切り替えパラメータ計算									<V057>				*/
/*																									*/
/****************************************************************************************************/
void	IprmcalCarrerChange( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
//	CMN_PRMDATA	*Cprm;
	CARRCHNG	*CarrerChange;
	LONG		MotStopDef;		/* モータ停止検出速度 */
	LONG		kx, sx, sx2;
	USHORT		Imax;
	SHORT		tmp_ilim;

	Prm = Axis->UniPrms->Prm;
//	Cprm = Axis->UniPrms->Cprm;
	CarrerChange = &(Axis->BaseLoops->CarrerChange);
	MotStopDef = 60000;

	/* キャリア周波数切り替え機能スイッチフラグ */
	if( (Prm->b_prm9 & 0x0010) && (Prm->pwmf <= 4000) )
	{ /* (Pn009.1 == 1) && (PnE2C <= 4000) */
		CarrerChange->conf.CarrChange = TRUE;
	}
	else
	{
		CarrerChange->conf.CarrChange = FALSE;
	}

	/* 最大電流Imaxを定義 */
	if( Prm->MencP.imax <= Prm->imaxs )
	{
		Imax = Prm->MencP.imax;
	}
	else
	{
		Imax = Prm->imaxs;
	}

#if (FLOAT_USE==TRUE)
	/* トルク閾値Aの計算：PnEA6 */
	kx = 15000 * Prm->MencP.irat / Imax;
	tmp_ilim = (SHORT)(kx * Prm->ilimlvl / 100.0f);
	CarrerChange->conf.IqLimitA = (LONG)tmp_ilim * (LONG)tmp_ilim;
		
	/* トルク閾値Aの計算：PnEA6+10% */
	tmp_ilim = (SHORT)( kx * (Prm->ilimlvl+10) / 100.0f);
	CarrerChange->conf.IqLimitB = (LONG)tmp_ilim * (LONG)tmp_ilim;
#else
	/* トルク閾値Aの計算：PnEA6 */
	kx = MlibScalKxgain( 15000, Prm->MencP.irat, Imax, &sx, 0 );
	sx2 = sx;
	tmp_ilim = (SHORT)MlibPcalKxgain( kx, Prm->ilimlvl, 100, &sx, -24 );
	CarrerChange->conf.IqLimitA = (LONG)tmp_ilim * (LONG)tmp_ilim;
		
	/* トルク閾値Aの計算：PnEA6+10% */
	tmp_ilim = (SHORT)MlibPcalKxgain( kx, Prm->ilimlvl+10, 100, &sx2, -24 );
	CarrerChange->conf.IqLimitB = (LONG)tmp_ilim * (LONG)tmp_ilim;
#endif /* FLOAT_USE */
		
	/* 速度閾値Aの計算：MOTSPD_DBLVL/2 */
	CarrerChange->conf.VelLimitA = MotStopDef >> 1;
		
	/* 速度閾値Bの計算：MOTSPD_DBLVL */
	CarrerChange->conf.VelLimitB = MotStopDef;

	/* 通常キャリアカウント選択		SGD7W IO入出力対応<S118> */
#if 0	/* モータキャリア周波数が入っている場合は常に確認するためシステムスイッチ不要<S11E> */
	if( (Cprm->syssw2 & 0x2000) != 0 )
	{/* @@zantei エンコーダパラメータによるキャリア切り替え機能有効(PnE0C.bit13) */
		if( (0 == Prm->MencP.motfuncf.f.chgPwm) && (0 != Prm->MencP.mot_pwmf) )
		{/* PnF24.bit0=0でモータPWM周波数(PnF25)が0でなければモータ周波数とアンプ周波数の低い方を使用する */
			if( Prm->MencP.mot_pwmf >= Prm->pwmf )
			{/* モータPWM>=アンプPWMの場合、アンプPWMを使用 */
				CarrerChange->conf.MotCarrUse = FALSE;
			}
			else
			{/* モータPWM<アンプPWMの場合、モータPWMを使用 */
				CarrerChange->conf.MotCarrUse = TRUE;
			}
		}
		else
		{
			CarrerChange->conf.MotCarrUse = FALSE;
		}
	}
	else
	{
		CarrerChange->conf.MotCarrUse = FALSE;
	}
#else	/* #if 0	 モータキャリア周波数が入っている場合は常に確認するためシステムスイッチ不要<S11E> */
	if( 0 == Prm->MencP.motfuncf.f.chgPwm )
	{/* PnF24.bit0=0の場合、モータキャリア周波数の値を確認して、どちらのキャリア周波数を使うか選択する */
		if( (0 < Prm->MencP.mot_pwmf) || (16000 >= Prm->MencP.mot_pwmf) )		/* 上限16KHｚとする(コード整理)  <S191> */
		{/* モータキャリアPWM周波数が範囲内の場合、モータ周波数を使用 */
			CarrerChange->conf.MotCarrUse = TRUE;
		}
		else
		{/* モータキャリアPWM周波数が範囲外の場合、アンプ周波数を使用 */
			CarrerChange->conf.MotCarrUse = FALSE;
		}
	}
	else
	{/* PnF24.bit0=1であればアンプ周波数を使用する */
		CarrerChange->conf.MotCarrUse = FALSE;
	}
#endif	/* #if 0	 モータキャリア周波数が入っている場合は常に確認するためシステムスイッチ不要<S11E> */
	LpxMakeCarrCnt( Prm, CarrerChange );			/* キャリア周波数カウント作成 */

	return;
}



/****************************************************************************************************/
/*																									*/
/*		キャリア周波数カウント作成処理							SGD7W IO入出力対応<S118>					*/
/*																									*/
/****************************************************************************************************/
static void	LpxMakeCarrCnt( PRMDATA *Prm, CARRCHNG *CarrerChange )
{
	if( TRUE == CarrerChange->conf.MotCarrUse )
	{/* モータPWMを使用 */
		/* PnF25で設定する通常キャリアカウント値の計算 */
		CarrerChange->conf.NormCarrCnt =
			(SHORT)KpiChangePwmFrequency( 0, Prm->MencP.mot_pwmf, Prm->pwmf3, Prm->MencP.flg_wf );

		/* PnE25で設定する高キャリアカウント値の計算 */
		CarrerChange->conf.HighCarrCnt =
			(SHORT)KpiChangePwmFrequency( Prm->pwmf2, Prm->MencP.mot_pwmf, Prm->pwmf3, Prm->MencP.flg_wf );
	}
	else
	{/* アンプPWMを使用 */
		/* PnE2Cで設定する通常キャリアカウント値の計算 */
		CarrerChange->conf.NormCarrCnt =
			(SHORT)KpiChangePwmFrequency( 0, Prm->pwmf, Prm->pwmf3, Prm->MencP.flg_wf );

		/* PnE25で設定する高キャリアカウント値の計算 */
		CarrerChange->conf.HighCarrCnt =
			(SHORT)KpiChangePwmFrequency( Prm->pwmf2, Prm->pwmf, Prm->pwmf3, Prm->MencP.flg_wf );
	}

	CarrerChange->conf.CarrCnt = CarrerChange->conf.NormCarrCnt;		/* 有効キャリアカウントセット 	*/

	return;
}


/****************************************************************************************************/
/*																									*/
/*		シーケンス入力マップ計算																	*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S045> */
static	void	IprmcalSeqInputMap( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
#if 0/* ｼｰｹﾝｽIOは未対応 */
	BPRMDAT				*Bprm;
	SERVO_CONTROL_IF	*SvControlIf;
	SHAL_LTSIGNALS		*LatchSignals;
	ULONG				i;
#endif
	SEQ_IOSIGNAL		*IoSignal;

	Prm	= Axis->UniPrms->Prm;
#if 0/* ｼｰｹﾝｽIOは未対応 */
	Bprm= Axis->UniPrms->Bprm;
	LatchSignals = Axis->LatchSignals;
	SvControlIf = Axis->SvControlIf;
#endif

	/* P-OT mapping */
	IoSignal = &(Axis->IoSignals->PotSignal);
	switch(Prm->pot_selsw >> 12)
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->pot_selsw & 0xFF);
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->pot_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

	/* N-OT mapping */
	IoSignal = &(Axis->IoSignals->NotSignal);
	switch(Prm->not_selsw >> 12)
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->not_selsw & 0xFF);
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->not_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

	/* DEC mapping */
	IoSignal = &(Axis->IoSignals->DecSignal);
	switch(Prm->dec_selsw >> 12)
	{
	case 1:	/* Switch ON => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->dec_selsw & 0xFF);
		break;
	case 2:	/* Switch ON => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->dec_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

#if	(CSW_SVCF_FBLATCH == TRUE)
	/* <S04D>:フルクローズドエンコーダ対応のため第一引数変更 */
	if(Axis->UniPrms->Bprm->FencUse != FALSE)
	{
		axisHwReg = Axis->FencV->EncIf.PosCmpSetReg;
	}
	else
	{
		axisHwReg = Axis->MencV->EncIf.PosCmpSetReg;
	}
	if(TRUE != SHalDrv_InitLatchSignal(axisHwReg, Axis->LtStsAdr, Prm->inselA, Axis->SvControlIf->ExtLtEnable))
	{
		/* A.040 : パラメータ設定異常 */
//		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		ALMSetPramError( Axis->AlmManager, pndef_inselA.RegNumber );	/* <S226> */
	}
#endif

#ifdef CSW_BSP_SGDVMD_B01	/* <S053> */
	/* E-STOP mapping */
	IoSignal = &(Axis->IoSignals->EstpSignal);
	switch(Prm->emg_selsw >> 12)
	{
	case 1:	/* Switch ON => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	case 2:	/* Switch ON => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}
#endif

#ifdef CSW_BSP_SGDVMD_B01 /* <S053> */
	/* BK mapping */
	IoSignal = &(Axis->IoSignals->BkSignal);
	switch(Prm->bk_selsw >> 12)
	{
	case 1:	/* Active low(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = KPX_SO1BITNO;
		break;
	case 2:	/* Active high(Reverse) */ // Active Highは危険なのでなし
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = KPX_SO1BITNO;
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 31;	/* Selects a reserved pin */
		break;
	}
#else
	/* BK mapping */
	IoSignal = &(Axis->IoSignals->BkSignal);
	switch(Prm->bk_selsw >> 12)
	{
	case 1:	/* Active low(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->bk_selsw & 0xFF);
		break;
	case 2:	/* Active high(Reverse) */ // Active Highは危険なのでなし
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->bk_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 31;	/* Selects a reserved pin */
		break;
	}
#endif

}
#endif

/****************************************************************************************************/
/*																									*/
/*		シーケンス入力マップ計算(M2,M3,CMD専用) <S045>												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalSeqInputMap( AXIS_HANDLE *Axis )
{
#define Lpx_GetDefInSig( prm, pin ) (((((prm) & 0x0F) < 7) || (((prm) & 0x0F) > 8)) ? (pin) : ((prm) & 0x0F))
#if CSW_BOARD_TYPE == SGD7W_TYPE	/* <S200-0> */
#define Lpx_GetExtInSig( prm )      ((((prm) & 0x0F) < 3) || ((((prm) & 0x0F) > 8) && (((prm) & 0x0F) < 0x0C))) \
									? 8 : ((prm) & 0x0F)
#else
#define Lpx_GetExtInSig( prm )      ((((prm) & 0x0F) < 4) || ((((prm) & 0x0F) > 8) && (((prm) & 0x0F) < 0x0D))) \
									? 8 : ((prm) & 0x0F)
#endif
	LONG				Mode;
	PRMDATA				*Prm;

	Prm	= Axis->UniPrms->Prm;
/*--------------------------------------------------------------------------------------------------*/
/*		入力信号割付モード																			*/
/*--------------------------------------------------------------------------------------------------*/
	Mode = Prm->insel1 & 0x0F;

	if ( Mode == 2 )
	{
		LpxSetInputMapW( Axis );						/* Only SGD7W */
	}
	else
	{
		LpxSetInputMapCmn( Axis, Mode );				/* Common */
	}

/*--------------------------------------------------------------------------------------------------*/
/* <S04A> Estp?信号割り付け。念のため															 	*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef CSW_BSP_SGDVMD_IFECAT						
	/* E-STOP mapping */
	IoSignal = &(Axis->IoSignals->EstpSignal);
	switch(Prm->emg_selsw >> 12)
	{
	case 1:	/* Switch ON => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	case 2:	/* Switch ON => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}
#endif

}

/****************************************************************************************************/
/*																									*/
/*		シーケンス入力マップ計算(共通) <S045>														*/
/*																									*/
/****************************************************************************************************/
void	LpxSetInputMapCmn( AXIS_HANDLE *Axis, LONG Mode )
{
LONG				lwk;
PRMDATA				*Prm;
SEQ_IO_HNDL	 		*IoSignals;

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
/*--------------------------------------------------------------------------------------------------*/
/*		入力信号デフォルト割付																		*/
/*--------------------------------------------------------------------------------------------------*/
	if ( Mode == 0 )
	{
	/*----------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	 SGD7W IO入出力対応<S118> */
		lwk = Lpx_GetDefInSig( (Prm->insel1 >> 12), 1 );				/* P-OT (M2,M3,CmdはSI1) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PotSignal), lwk, 0x01 );	/* P-OT (M2,M3,CmdはSI1) */
		lwk = Lpx_GetDefInSig( (Prm->insel2 >> 0), 2 );					/* N-OT (M2,M3,CmdはSI2) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NotSignal), lwk, 0x01 );	/* N-OT (M2,M3,CmdはSI2) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PclSignal), 8, 0x00 );	/* P-CL (M2,M3,CmdはNA) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NclSignal), 8, 0x00 );	/* N-CL (M2,M3,CmdはNA) */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
		lwk = Lpx_GetDefInSig( (Prm->insel1 >> 12), 1 );			/* P-OT (M2,M3,CmdはSI1) */
		LpxPcalSetInputMap( &(IoSignals->PotSignal), lwk, 0x01 );	/* P-OT (M2,M3,CmdはSI1) */
		lwk = Lpx_GetDefInSig( (Prm->insel2 >> 0), 2 );				/* N-OT (M2,M3,CmdはSI2) */
		LpxPcalSetInputMap( &(IoSignals->NotSignal), lwk, 0x01 );	/* N-OT (M2,M3,CmdはSI2) */
		LpxPcalSetInputMap( &(IoSignals->PclSignal), 8, 0x00 );		/* P-CL (M2,M3,CmdはNA) */
		LpxPcalSetInputMap( &(IoSignals->NclSignal), 8, 0x00 );		/* N-CL (M2,M3,CmdはNA) */
#endif
	/*----------------------------------------------------------------------------------------------*/
	}
	else
	{
	/*----------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ  SGD7W IO入出力対応<S118> */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PotSignal), (Prm->insel1 >> 12), 0x01 );	/* P-OT */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NotSignal), (Prm->insel2 >> 0),  0x01 );	/* N-OT */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PclSignal), (Prm->insel2 >> 8),  0x00 );	/* P-CL */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NclSignal), (Prm->insel2 >> 12), 0x00 );	/* N-CL */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
		LpxPcalSetInputMap( &(IoSignals->PotSignal), (Prm->insel1 >> 12), 0x01 );	/* P-OT */
		LpxPcalSetInputMap( &(IoSignals->NotSignal), (Prm->insel2 >> 0),  0x01 );	/* N-OT */
		LpxPcalSetInputMap( &(IoSignals->PclSignal), (Prm->insel2 >> 8),  0x00 );	/* P-CL */
		LpxPcalSetInputMap( &(IoSignals->NclSignal), (Prm->insel2 >> 12), 0x00 );	/* N-CL */
#endif
	/*----------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ  SGD7W IO入出力対応<S118> */
	LpxPcalSetInputMap2( Axis, &(IoSignals->DecSignal), (Prm->inselA >> 0), 0x00 );	/* DEC */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 4) );
	LpxPcalSetInputMap2( Axis, &(IoSignals->ExtSignal[0]), lwk , 0x00 );				/* EXT1 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 8) );
	LpxPcalSetInputMap2( Axis, &(IoSignals->ExtSignal[1]), lwk , 0x00 );				/* EXT2 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 12) );
	LpxPcalSetInputMap2( Axis, &(IoSignals->ExtSignal[2]), lwk , 0x00 );				/* EXT3 */
	lwk = /*Lpx_GetExtInSig( (*/Prm->insel6/* >> 0) )*/;		/* <S1F5> */
	LpxPcalSetInputMap2( Axis, &(IoSignals->FstpSignal), lwk , 0x01/* @@ 0x00 */); 					/* FSTP */   /* <S1F5> */
	/* <S21D> Start */
	LpxPcalSetInputMap2( Axis, &(IoSignals->ClinkansSignal), Prm->insel7 >> 0 , 0x01 ); 	/* /CLINKANS */
	LpxPcalSetInputMap2( Axis, &(IoSignals->CmcansSignal), Prm->insel7 >> 4 , 0x01 ); 		/* /CMCANS */
	LpxPcalSetInputMap2( Axis, &(IoSignals->ConvreadySignal), Prm->insel7 >> 8 , 0x01 ); 	/* /CONV_READY */
	LpxPcalSetInputMap2( Axis, &(IoSignals->ConvmconSignal), Prm->insel7 >> 12 , 0x01 ); 	/* /CONV_MCON */
	/* <S21D> End */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
	LpxPcalSetInputMap( &(IoSignals->DecSignal), (Prm->inselA >> 0), 0x00 );	/* DEC */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 4) );
	LpxPcalSetInputMap( &(IoSignals->ExtSignal[0]), lwk , 0x00 );				/* EXT1 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 8) );
	LpxPcalSetInputMap( &(IoSignals->ExtSignal[1]), lwk , 0x00 );				/* EXT2 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 12) );
	LpxPcalSetInputMap( &(IoSignals->ExtSignal[2]), lwk , 0x00 );				/* EXT3 */
	lwk = /*Lpx_GetExtInSig( (*/Prm->insel6 /*>> 0) )*/;	/* <S1F5> */
	LpxPcalSetInputMap( &(IoSignals->FstpSignal), lwk , 0x01/* @@ 0x00 */ ); 				/* FSTP */ /* <S1F5> */
	/* <S21D> Start */
	LpxPcalSetInputMap( &(IoSignals->ClinkansSignal), Prm->insel7 >> 0 , 0x00 ); 	/* /CLINKANS */
	LpxPcalSetInputMap( &(IoSignals->CmcansSignal), Prm->insel7 >> 4 , 0x00 ); 		/* /CMCANS */
	LpxPcalSetInputMap( &(IoSignals->ConvreadySignal), Prm->insel7 >> 8 , 0x00 ); 	/* /CONV_READY */
	LpxPcalSetInputMap( &(IoSignals->ConvmconSignal), Prm->insel7 >> 12 , 0x00 ); 	/* /CONV_MCON */
	/* <S21D> End */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*	<S04A> Initialize External Signal Latch Assign													*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FBLATCH == TRUE)
	if(TRUE != SHalDrv_InitLatchSignal(Axis->SvAsicRegs->AsicHwReg, 
										Axis->LtStsAdr, Prm->inselA, Axis->SvControlIf->ExtLtEnable))
	{
		/* A.040 : パラメータ設定異常 */
//		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		ALMSetPramError( Axis->AlmManager, pndef_inselA.RegNumber );	/* <S226> */
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		シーケンス入力マップ計算(SGD7W専用) <S1B5>													*/
/*																									*/
/****************************************************************************************************/
void	LpxSetInputMapW( AXIS_HANDLE *Axis )
{
PRMDATA				*Prm;
SEQ_IO_HNDL	 		*IoSignals;

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
/*--------------------------------------------------------------------------------------------------*/
/*	Set Input Signal Map for SGD7W																	*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetInputMap3( &(IoSignals->PotSignal), Prm->pot_selsw, 1 );
	LpxPcalSetInputMap3( &(IoSignals->NotSignal), Prm->not_selsw, 1 );
	LpxPcalSetInputMap3( &(IoSignals->DecSignal), Prm->dec_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ExtSignal[0]), Prm->ext_selsw[0], 0 );
	LpxPcalSetInputMap3( &(IoSignals->ExtSignal[1]), Prm->ext_selsw[1], 0 );
	LpxPcalSetInputMap3( &(IoSignals->ExtSignal[2]), Prm->ext_selsw[2], 0 );
	/*----------------------------------------------------------------------------------------------*/
	LpxPcalSetInputMap3( &(IoSignals->FstpSignal), Prm->fstp_selsw, 1 );
	LpxPcalSetInputMap3( &(IoSignals->PclSignal), Prm->pcl_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->NclSignal), Prm->ncl_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ClinkansSignal), Prm->clinkans_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->CmcansSignal), Prm->cmcans_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ConvreadySignal), Prm->convready_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ConvmconSignal), Prm->convmcon_selsw, 0 );
/*--------------------------------------------------------------------------------------------------*/
/*	Initialize External Signal Latch Assign															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxInitLatchSignal( Axis );
	return;
}


/****************************************************************************************************/
/*																									*/
/*		シーケンス入力マップ計算3(SGD7W専用) <S1B5>													*/
/*																									*/
/****************************************************************************************************/
void	LpxPcalSetInputMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH ) 
{
LONG	Pin;
ULONG	LogicX;
ULONG	LogicL = (LogicH) ? 0x00 : 0x01;
/*--------------------------------------------------------------------------------------------------*/
#define Lpx_GetBcd( prm )			((((prm) & 0x0F) * 1) 			/* <S1BF> */					\
									+ ((((prm)>>4) & 0x0F) * 10))
/*--------------------------------------------------------------------------------------------------*/
#define INPUT_MAP3_LGC_NONACTIVE 	(0)
#define INPUT_MAP3_LGC_NORMAL 		(1)
#define INPUT_MAP3_LGC_OPPOSITE 	(2)
#define INPUT_MAP3_LGC_ACTIVE 		(3)
/*--------------------------------------------------------------------------------------------------*/
	Pin = HALdrv_GetPsBitNumber( Lpx_GetBcd( prm ) );				/* Get Pin <S1BF> */
	LogicX = ( prm >> 12 ) & 0x0F;									/* Get Logic */
//	LogicX = (( Pin < 0 ) ? ((LogicH) ? INPUT_MAP3_LGC_ACTIVE 		/* Disable Pin <S1BF> */
//					: INPUT_MAP3_LGC_NONACTIVE) : LogicX);
	LogicX = (( Pin < 0 ) ? INPUT_MAP3_LGC_NONACTIVE : LogicX);		/* Disable Pin <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
/*	Set Input Signal Map																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( LogicX )
	{
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_NONACTIVE:									/* 0: Non Active				*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = KPI_SINLOBITNO;
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_NORMAL:										/* 1: Normal					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= LogicL;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_OPPOSITE:									/* 2: Opposite					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= LogicH;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_ACTIVE:										/* 3: Always active				*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = KPI_SINHIBITNO;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= 0;
			break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = KPI_SINLOBITNO;
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> Get insel Parameter Number															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetInSel( LONG Axis, ULONG prm )
{
LONG	wkx;
LONG	Pin;
ULONG	LogicX;
/*--------------------------------------------------------------------------------------------------*/
	Pin = HALdrv_GetPsBitNumber( Lpx_GetBcd( prm ) );				/* Get Pin <S1BF> */
	LogicX = ( prm >> 12 ) & 0x0F;									/* Get Logic */
	LogicX = (( Pin < 0 ) ? INPUT_MAP3_LGC_NONACTIVE : LogicX);		/* Disable Pin */
/*--------------------------------------------------------------------------------------------------*/
	if ( LogicX == INPUT_MAP3_LGC_NONACTIVE )
	{
		return (8);													/* Not Available */
	}
	else if ( ( LogicX == INPUT_MAP3_LGC_ACTIVE ) )
	{
		return (7);
	}
	else if ( LogicX == INPUT_MAP3_LGC_NORMAL ) 
	{
		wkx = 0;
	}
	else if ( LogicX == INPUT_MAP3_LGC_OPPOSITE )
	{
		wkx = 9;
	}
	else 
	{
		return (8);													/* Not Available */
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( Axis == 0 ) 												/* Axis 0 */
	{
		switch ( Pin )
		{
		case KPI_SI5BITNO: 	wkx++;
		case KPI_SI4BITNO: 	wkx++;
		case KPI_SI3BITNO: 	wkx++;
		case KPI_SI2BITNO: 	wkx++;
		case KPI_SI1BITNO: 	wkx++;
		case KPI_SI0BITNO: 	break;
		default: return (8);										/* Not available */
		}
	}
	else															/* Axis 1 */
	{
		switch ( Pin )
		{
		case KPI_SI11BITNO:	wkx++;
		case KPI_SI10BITNO:	wkx++;
		case KPI_SI9BITNO: 	wkx++;
		case KPI_SI8BITNO: 	wkx++;
		case KPI_SI7BITNO: 	wkx++;
		case KPI_SI6BITNO: 	break;
		default: return (8);										/* Not available */
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	return (wkx);
}

/****************************************************************************************************/
/*																									*/
/*		ラッチ信号割付(SGD7W専用) <S1B5>															*/
/*																									*/
/****************************************************************************************************/
void	LpxInitLatchSignal( AXIS_HANDLE *Axis )
{
LONG				inselA;
PRMDATA				*Prm;

	Prm	= Axis->UniPrms->Prm;
/*--------------------------------------------------------------------------------------------------*/
/*	Make inselA																						*/
/*--------------------------------------------------------------------------------------------------*/
	inselA = 0;
	inselA |= (LpxGetInSel( Axis->AxisNo, Prm->ext_selsw[0] ) << 4 );
	inselA |= (LpxGetInSel( Axis->AxisNo, Prm->ext_selsw[1] ) << 8 );
	inselA |= (LpxGetInSel( Axis->AxisNo, Prm->ext_selsw[2] ) << 12);
/*--------------------------------------------------------------------------------------------------*/
/*	 Initialize External Signal Latch Assign														*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FBLATCH == TRUE)
	if(TRUE != SHalDrv_InitLatchSignal(Axis->SvAsicRegs->AsicHwReg, 
										Axis->LtStsAdr, inselA, Axis->SvControlIf->ExtLtEnable))
	{
		/* A.040 : パラメータ設定異常 */
//		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		ALMSetPramError( Axis->AlmManager, pndef_inselA.RegNumber );	/* <S226> */
	}
#endif
}

/****************************************************************************************************/
/*																									*/
/*		シーケンス入力マップ計算(M2,M3,CMD専用) <S045>												*/
/*																									*/
/****************************************************************************************************/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ SGD7W IO入出力対応<S118>	*/
static	void	LpxPcalSetInputMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH )
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
static	void	LpxPcalSetInputMap( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH ) 
#endif
{
ULONG	LogicL = (LogicH) ? 0x00 : 0x01;
/*--------------------------------------------------------------------------------------------------*/
	IoSignal->Mask		= 1;
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	SGD7W IO入出力対応<S118>	*/
	if(Axis->AxisNo == 0 )	/*	1軸目	*/
	{	/*	1軸目	*/
		switch ( prm & 0x0F )
		{
			case 0x00: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicL; break;
			case 0x01: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicL; break;
			case 0x02: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicL; break;
			case 0x03: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicL; break;
			case 0x04: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicL; break;
			case 0x05: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicL; break;
//-defaultへ	case 0x06: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicL; break;
			case 0x07:
			default  : IoSignal->PinNumber = KPI_SINHIBITNO; IoSignal->XorMask = 0x00; break;
			case 0x08: IoSignal->PinNumber = KPI_SINLOBITNO; IoSignal->XorMask = 0x00; break;
			case 0x09: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0A: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0B: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0C: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0D: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0E: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicH; break;
//--defaultへcase 0x0F: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicH; break;
		}
	}else
	{	/*	2軸目	*/
		switch ( prm & 0x0F )
		{
			case 0x00: IoSignal->PinNumber = KPI_SI0BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x01: IoSignal->PinNumber = KPI_SI1BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x02: IoSignal->PinNumber = KPI_SI2BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x03: IoSignal->PinNumber = KPI_SI3BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x04: IoSignal->PinNumber = KPI_SI4BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x05: IoSignal->PinNumber = KPI_SI5BITNO - 6; IoSignal->XorMask = LogicL; break;
//-defaultへ	case 0x06: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicL; break;
			case 0x07:
			default  : IoSignal->PinNumber = KPI_SINHIBITNO; IoSignal->XorMask = 0x00; break;
			case 0x08: IoSignal->PinNumber = KPI_SINLOBITNO; IoSignal->XorMask = 0x00; break;
			case 0x09: IoSignal->PinNumber = KPI_SI0BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0A: IoSignal->PinNumber = KPI_SI1BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0B: IoSignal->PinNumber = KPI_SI2BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0C: IoSignal->PinNumber = KPI_SI3BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0D: IoSignal->PinNumber = KPI_SI4BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0E: IoSignal->PinNumber = KPI_SI5BITNO - 6; IoSignal->XorMask = LogicH; break;
//-defaultへ	case 0x0F: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicH; break;
		}
	}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
	switch ( prm & 0x0F )
	{
		case 0x00: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicL; break;
		case 0x01: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicL; break;
		case 0x02: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicL; break;
		case 0x03: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicL; break;
		case 0x04: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicL; break;
		case 0x05: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicL; break;
		case 0x06: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicL; break;
		case 0x07:
		default  : IoSignal->PinNumber = KPI_SINHIBITNO; IoSignal->XorMask = 0x00; break;
		case 0x08: IoSignal->PinNumber = KPI_SINLOBITNO; IoSignal->XorMask = 0x00; break;
		case 0x09: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0A: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0B: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0C: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0D: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0E: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0F: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicH; break;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		シーケンス出力マップ計算(M2,M3,CMD専用) <S045>												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalSeqOutputMap( AXIS_HANDLE *Axis )
{
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SETSOUTPTR( signal, ptr ) 	\
		SeqIo = &(IoSignals->##signal); \
		SeqIo->SignalStatus.p = (ptr);
/*--------------------------------------------------------------------------------------------------*/
	PRMDATA				*Prm;
	SEQ_IOSIGNAL		*SeqIo;
	SEQ_IO_HNDL	 		*IoSignals;
	BASE_CTRL_HNDL		*BaseControls;
	SEQ_CTRL_OUT		*SeqCtrlOut;
	BE_SEQ_HNDL			*BeSequence;
	ALMMAN_HANDLE		*AlmManager;
	CHECK_MAIN_POWER	*PowerChecker;/* <S154> */

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
	BaseControls = Axis->BaseControls;
	SeqCtrlOut = Axis->SeqCtrlOut;
	BeSequence = Axis->BeSequence;
	AlmManager = Axis->AlmManager;
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);/* <S154> */

/*--------------------------------------------------------------------------------------------------*/
/*		入力信号割付モード																			*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ( Prm->insel1 & 0x0F ) == 2 )
	{
		LpxSetOutputMapW( Axis );						/* Only SGD7W */
	}
	else
	{
		LpxSetOutputMapCmn( Axis );						/* Common */
	}

//<S081>  #if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/* Set Output Signal Pointers (M2/M3/CMD Only) 														*/
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( CoinSignal, &(BaseControls->PosCtrlSts.CoinSignal) );	/* /COIN */
	LPX_SETSOUTPTR( VcmpSignal, &(SeqCtrlOut->VcmpSts) );					/* /VCMP */
	LPX_SETSOUTPTR( TgonSignal, &(SeqCtrlOut->TgOnSts) );					/* /TGON */
	LPX_SETSOUTPTR( SrdySignal, &(BeSequence->ServoReady) );				/* /S-RDY */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( CltSignal,  &(BaseControls->CtrlCmdMngr.TrqClamp) );	/* /CLT */
	LPX_SETSOUTPTR( VltSignal,  &(BaseControls->CtrlCmdMngr.SpdClamp) );	/* /VLT */
	LPX_SETSOUTPTR( BkSignal,   &(SeqCtrlOut->BrkSigSts) );					/* /BK */
	LPX_SETSOUTPTR( WarnSignal, &(AlmManager->WrnFlagX) );					/* /WARN <S0FF> */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( NearSignal, &(BaseControls->PosCtrlSts.NearSignal) );	/* /NEAR */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( PreWrnSignal,  &(PowerChecker->var.PreSigOut) );		/* /PreWrn */ /* <S154> */
	LPX_SETSOUTPTR( ClinkSignal,  &(PowerChecker->var.ClinkOut) );		/* /CLINK *//* <S21D> */
	LPX_SETSOUTPTR( CmconSignal,  &(PowerChecker->var.CmconOut) );		/* /CMCON *//* <S21D> */
//<S081>  #endif /* CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE */

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Maps (A/P Only)																		*/
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( Alo1Signal,  &(AlmManager->AlmCodeOut.Alo1) );			/* ALO1 */
	LPX_SETSOUTPTR( Alo2Signal,  &(AlmManager->AlmCodeOut.Alo2) );			/* ALO2 */
	LPX_SETSOUTPTR( Alo3Signal,  &(AlmManager->AlmCodeOut.Alo3) );			/* ALO3 */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> シーケンス出力マップ計算(共通) 														*/
/*																									*/
/****************************************************************************************************/
void	LpxSetOutputMapW( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	SEQ_IO_HNDL	 		*IoSignals;

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Map 																					*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap3( &(IoSignals->CoinSignal), Prm->coin_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->VcmpSignal), Prm->vcmp_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->TgonSignal), Prm->tgon_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->SrdySignal), Prm->srdy_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->CltSignal), Prm->clt_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->VltSignal), Prm->vlt_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->BkSignal), Prm->bk_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->WarnSignal), Prm->warn_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->NearSignal), Prm->near_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->CpulseSignal), Prm->cpulse_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->ClinkSignal), Prm->clink_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->CmconSignal), Prm->cmcon_selsw, 0 );
	//	LpxPcalSetSoutMap3( &(IoSignals->PmSignal), Prm->pm_selsw, 0 );			/* <S21D> */
	LpxPcalSetSoutMap3( &(IoSignals->PreWrnSignal), Prm->pm_selsw, 0 );			/* <S21D> */
	LpxPcalSetSoutMap3( &(IoSignals->DaoSignal[0]), Prm->dao_selsw[0], 0 );
	LpxPcalSetSoutMap3( &(IoSignals->DaoSignal[1]), Prm->dao_selsw[1], 0 );
/*--------------------------------------------------------------------------------------------------*/
	return; 
}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> シーケンス出力マップ計算(共通) 														*/
/*																									*/
/****************************************************************************************************/
void	LpxSetOutputMapCmn( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	SEQ_IO_HNDL	 		*IoSignals;
	ULONG				OppositeMode; /* <S04B> */

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
	OppositeMode = Prm->sorev1 + (Prm->sorev2 << 16); /* <S04B> */

/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Maps (M2/M3/CMD Only) 																*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	 SGD7W IO入出力対応<S118>	*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->CoinSignal), (Prm->outsel1 >> 0),  OppositeMode );	/* /COIN */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->VcmpSignal), (Prm->outsel1 >> 4),  OppositeMode );	/* /VCMP */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->TgonSignal), (Prm->outsel1 >> 8),  OppositeMode );	/* /TGON */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->SrdySignal), (Prm->outsel1 >> 12), OppositeMode );	/* /S-RDY */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->CltSignal),  (Prm->outsel2 >> 0),  OppositeMode );	/* /CLT */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->VltSignal),  (Prm->outsel2 >> 4),  OppositeMode );	/* /VLT */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->BkSignal),   (Prm->outsel2 >> 8),  OppositeMode );	/* /BK */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->WarnSignal), (Prm->outsel2 >> 12), OppositeMode );	/* /WARN */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->NearSignal), (Prm->outsel3 >> 0), OppositeMode );	/* /NEAR */

	LpxPcalSetSoutMap2( Axis, &(IoSignals->PreWrnSignal), (Prm->outsel4 >> 8), OppositeMode );	/* /PreWrn */ /*<S154>*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->ClinkSignal), (Prm->outsel4 >> 0), OppositeMode );	/* /CLINK *//* <S21D> */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->CmconSignal), (Prm->outsel4 >> 4), OppositeMode );	/* /CMCON *//* <S21D> */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
	LpxPcalSetSoutMap( &(IoSignals->CoinSignal), (Prm->outsel1 >> 0),  OppositeMode );	/* /COIN */
	LpxPcalSetSoutMap( &(IoSignals->VcmpSignal), (Prm->outsel1 >> 4),  OppositeMode );	/* /VCMP */
	LpxPcalSetSoutMap( &(IoSignals->TgonSignal), (Prm->outsel1 >> 8),  OppositeMode );	/* /TGON */
	LpxPcalSetSoutMap( &(IoSignals->SrdySignal), (Prm->outsel1 >> 12), OppositeMode );	/* /S-RDY */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap( &(IoSignals->CltSignal),  (Prm->outsel2 >> 0),  OppositeMode );	/* /CLT */
	LpxPcalSetSoutMap( &(IoSignals->VltSignal),  (Prm->outsel2 >> 4),  OppositeMode );	/* /VLT */
	LpxPcalSetSoutMap( &(IoSignals->BkSignal),   (Prm->outsel2 >> 8),  OppositeMode );	/* /BK */
	LpxPcalSetSoutMap( &(IoSignals->WarnSignal), (Prm->outsel2 >> 12), OppositeMode );	/* /WARN */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap( &(IoSignals->NearSignal), (Prm->outsel3 >> 0), OppositeMode );	/* /NEAR */

	LpxPcalSetSoutMap( &(IoSignals->PreWrnSignal), (Prm->outsel4 >> 8), OppositeMode );	/* /PreWrn */ /*<S154>*/
	LpxPcalSetSoutMap( &(IoSignals->ClinkSignal), (Prm->outsel4 >> 0), OppositeMode );	/* /CLINK *//* <S21D> */
	LpxPcalSetSoutMap( &(IoSignals->CmconSignal), (Prm->outsel4 >> 4), OppositeMode );	/* /CMCON *//* <S21D> */
#endif
/*--------------------------------------------------------------------------------------------------*/

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Maps (A/P Only)																		*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap( &(IoSignals->Alo1Signal), (Prm->outsel6 >> 0), OppositeMode );	/* ALO1 *//* <S04B> */
	LpxPcalSetSoutMap( &(IoSignals->Alo2Signal), (Prm->outsel6 >> 4), OppositeMode );	/* ALO2 *//* <S04B> */
	LpxPcalSetSoutMap( &(IoSignals->Alo3Signal), (Prm->outsel6 >> 8), OppositeMode );	/* ALO3 *//* <S04B> */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */
				
	return;
}

/****************************************************************************************************/
/*																									*/
/*		シーケンス出力マップ計算 <S1B5>																*/
/*																									*/
/****************************************************************************************************/
void	LpxPcalSetSoutMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH )
{
LONG	Pin;
ULONG	LogicX;
//ULONG	LogicL = (LogicH) ? 0x00 : 0x01;
#define OUTPUT_MAP3_LGC_NONACTIVE 	(0)
#define OUTPUT_MAP3_LGC_NORMAL 		(1)
#define OUTPUT_MAP3_LGC_OPPOSITE 	(2)
/*--------------------------------------------------------------------------------------------------*/
	Pin = HALdrv_GetSpBitNumber( Lpx_GetBcd( prm ) );				/* Get Pin <S1BF> */
	LogicX = ( prm >> 12 ) & 0x0F;									/* Get Logic */
	LogicX = (( Pin < 0 ) ? OUTPUT_MAP3_LGC_NONACTIVE : LogicX);	/* Disable Pin */
/*--------------------------------------------------------------------------------------------------*/
/*	Set Output Signal Map																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( LogicX )
	{
	/*----------------------------------------------------------------------------------------------*/
	case OUTPUT_MAP3_LGC_NONACTIVE:									/* 0: Non Active				*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = 31;								/* 暫定 */
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case OUTPUT_MAP3_LGC_NORMAL:									/* 1: Normal					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= 0/*LogicL*/;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case OUTPUT_MAP3_LGC_OPPOSITE:									/* 2: Opposite					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= 1/*LogicH*/;
			break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = 31;								/* 暫定 */
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		シーケンス出力マップ計算 <S045>																*/
/*																									*/
/****************************************************************************************************/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	SGD7W IO入出力対応<S118>	*/
void	LpxPcalSetSoutMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG OppositeReq )
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
void	LpxPcalSetSoutMap( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG OppositeReq )
#endif
{
/*--------------------------------------------------------------------------------------------------*/
	IoSignal->Mask		= 1;
//	IoSignal->XorMask   = 0; /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	SGD7W IO入出力対応<S118>	*/
		switch( prm & 0x0F )
		{
		/*--------------------------------------------------------------------------------------------------*/
			case 1:													/* Output to SO1 */
		/*--------------------------------------------------------------------------------------------------*/
				if(Axis->AxisNo == 0 )	/*	1軸目	*/
				{	/*	1軸目	*/
					IoSignal->PinNumber = KPI_SO1BITNO;		/*	CN1-23,24	*/
					//IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 0) & 0x01) ? 1 : 0;	/* <S1DA> */
				}else
				{	/*	2軸目	*/
					IoSignal->PinNumber = KPI_SO2BITNO;		/*	CN1-25,26	*/
					//IoSignal->XorMask = ((OppositeReq >> 8) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 0) & 0x01) ? 1 : 0;	/* <S1DA> */
				}
				break;
			case 2:													/* Output to SO2 */
		/*--------------------------------------------------------------------------------------------------*/
				if(Axis->AxisNo == 0 )	/*	1軸目	*/
				{	/*	1軸目	*/
					IoSignal->PinNumber = KPI_SO3BITNO;		/*	CN1-27,28	*/
					//IoSignal->XorMask = ((OppositeReq >> 12) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0;	/* <S1DA> */
				}else
				{	/*	2軸目	*/
					IoSignal->PinNumber = KPI_SO4BITNO;		/*	CN1-29,30	*/
					//IoSignal->XorMask = ((OppositeReq >> 16) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0;	/* <S1DA> */
				}
				break;
		/*--------------------------------------------------------------------------------------------------*/
			default:												/* Always inactive */
		/*--------------------------------------------------------------------------------------------------*/
				IoSignal->Mask		= 0;
				IoSignal->XorMask   = 0;
				IoSignal->PinNumber = 31;			/* Selects a reserved pin */
				break;
		}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
	switch( prm & 0x0F )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 1:													/* Output to SO1 */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO1BITNO;
		IoSignal->XorMask = ((OppositeReq >> 0) & 0x01) ? 1 : 0; /* <S04B> */
		break;
	case 2:													/* Output to SO2 */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO2BITNO;
		IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0; /* <S04B> */
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:													/* Output to SO3 */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO3BITNO;
		IoSignal->XorMask = ((OppositeReq >> 8) & 0x01) ? 1 : 0; /* <S04B> */
		break;

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
/* <S1AA> : SO4～SO6の出力信号反転を廃止 */
/*--------------------------------------------------------------------------------------------------*/
	case 4:													/* Output to SO4 *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO4BITNO;
		//IoSignal->XorMask = ((OppositeReq >> 12) & 0x01) ? 0 : 1; /* <S04B> */	/* <S1AA> */
		IoSignal->XorMask = ((OppositeReq >> 12) & 0x01) ? 1 : 0;					/* <S1AA> */

		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:													/* Output to SO5 *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO5BITNO;
		//IoSignal->XorMask = ((OppositeReq >> 16) & 0x01) ? 0 : 1; /* <S04B> */	/* <S1AA> */
		IoSignal->XorMask = ((OppositeReq >> 16) & 0x01) ? 1 : 0;					/* <S1AA> */

		break;
/*--------------------------------------------------------------------------------------------------*/
	case 6:													/* Output to SO6 *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO6BITNO;
		//IoSignal->XorMask = ((OppositeReq >> 20) & 0x01) ? 0 : 1; /* <S04B> */	/* <S1AA> */
		IoSignal->XorMask = ((OppositeReq >> 20) & 0x01) ? 1 : 0;					/* <S1AA> */
		break;
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

/*--------------------------------------------------------------------------------------------------*/
	default:												/* Always inactive */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 31;			/* Selects a reserved pin */
		break;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		モータ速度検出用移動平均初期化処理	<V259>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : パラメータ設定(Pn009.2)および接続されたエンコーダ分解能によって移動平均回数の初期値を	*/
/*			設定する。ただし、調整レスが有効な場合は、移動平均回数を0回に変更しておく。				*/
/*			移動平均回数のオンラインでの変更は、BpiChangeSpdDetMaFil( )にて行うこと。				*/
/*																									*/
/*			<回転形>							移動平均回数										*/
/*			エンコーダ分解能	速度分解能		通常(Pn009.2=0)	(Pn009.2=1)	磁極検出	調整レス	*/
/*			  13bit以下			117.12min-1以上		4回				16回	  16回		  0回		*/
/*			  16bit				 14.64min-1			2回				16回	  16回		  0回		*/
/*			  17bit以上			  7.32min-1以下		0回				16回	  16回		  0回		*/
/*																									*/
/*			<リニア>							移動平均回数										*/
/*			スケールピッチ		速度分解能		通常(Pn009.2=0)	(Pn009.2=1)	磁極検出	調整レス	*/
/*			  200um以上			 12.50mm/s以上		4回				16回	  16回		  0回		*/
/*			  100um				  6.25mm/s			2回				16回	  16回		  0回		*/
/*			  100um未満			  6.25mm/s未満		0回				16回	  16回		  0回		*/
/*																									*/
/*			※速度分解能(1パルス当りの速度)の値は、サンプリング周期62.5usの場合						*/
/*			※リニアモータの場合は、256てい倍で計算													*/
/*																									*/
/****************************************************************************************************/
#define	SPDDET13BIT		KPI_SACOUNTMS*600000/8192			/* 13bitエンコーダ速度検出分解能		*/
#define	SPDDET16BIT		KPI_SACOUNTMS*600000/65536			/* 16bitエンコーダ速度検出分解能		*/
#define	SPDDET100UM		KPI_SACOUNTMS*10000/256*100/1000	/* スケールピッチ100umの速度検出分解能	*/
#define	SPDDET200UM		KPI_SACOUNTMS*10000/256*200/1000	/* スケールピッチ200umの速度検出分解能	*/
/*--------------------------------------------------------------------------------------------------*/
static void	LpxPcalSpdDetMaFilter( AXIS_HANDLE *Axis )
{
	BPRMDAT			*Bprm;

	Bprm	= Axis->UniPrms->Bprm;
	MlibResetLongMemory( Axis->MotSpdMafil, sizeof( *Axis->MotSpdMafil )/4 );
/*--------------------------------------------------------------------------------------------------*/
/*		移動平均回数の初期値を設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if ( Bprm->SpdDetMaFil == TRUE )
	{ /* Pn009.2 = 1の場合16回 */
		Axis->MotSpdMafil->defaultNumBit = 4;
	}
	else if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニアの場合 */
		if( Bprm->SpdDetectUnit >= SPDDET200UM )
		{ /* 200um以上は4回 */
			Axis->MotSpdMafil->defaultNumBit = 2;
		}
		else if( Bprm->SpdDetectUnit >= SPDDET100UM )
		{ /* 100um～200umは2回 */
			Axis->MotSpdMafil->defaultNumBit = 1;
		}
		else
		{ /* 100um未満は0回 */
			Axis->MotSpdMafil->defaultNumBit = 0;
		}
	}
	else
	{ /* 回転型の場合 */
		if( Bprm->SpdDetectUnit >= SPDDET13BIT )
		{ /* 13bit以下は4回 */
			Axis->MotSpdMafil->defaultNumBit = 2;
		}
		else if( Bprm->SpdDetectUnit >= SPDDET16BIT )
		{ /* 14～16bitは2回 */
			Axis->MotSpdMafil->defaultNumBit = 1;
		}
		else
		{ /* 17bit以上は0回 */
			Axis->MotSpdMafil->defaultNumBit = 0;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		移動平均回数を設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	Axis->MotSpdMafil->manumBit = Axis->MotSpdMafil->defaultNumBit;
	if ( Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{ /* 調整レス時は0固定 */
		Axis->MotSpdMafil->manumBit = 0;
	}
	Axis->MotSpdMafil->idx = 0;
	Axis->MotSpdMafil->ksub = 0;
	Axis->MotSpdMafil->mabufSumx = 0;
/*--------------------------------------------------------------------------------------------------*/

}



/****************************************************************************************************/
#define SVCDEF_PCMP_DLYUS			( 80 )		/* 位相遅れ補償遅れ時間(エンコーダ:50us+ScanA:30us)	*/
#define SVCDEF_PCMP_DLYCX			( 16 )				/* 位相遅れ補償補正係数 [-]					*/
/****************************************************************************************************/
/*																									*/
/*		位相補償演算係数の計算																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 遅れ時間(エンコーダなど)から、位相遅れ補償演算係数を算出する。							*/
/*			パラメータによる位相補償から、位相速度補償演算係数を算出する。							*/
/*																									*/
/*	補 足 : 位相補償計算をサーボパックパラメータが０の場合、モータパラメータを使う。				*/
/*			(万が一、モータ側パラメータがＦ詰めの場合はサーボパックパラメータを使う)				*/
/*			サーボパックパラメータが０以外の場合、サーボパックパラメータを使う。					*/
/*																									*/
/*	注 意 : サーボパックパラメータは０以外でないと使う事ができない。								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxPcalPcmpCalGain( AXIS_HANDLE	*Axis )
{
#if (FLOAT_USE==TRUE)
	float			fx;
#else
	LONG			kx;
	LONG			sx;
#endif /* FLOAT_USE */
	LONG			PhsCmpDegx;
	BPRMDAT			*Bprm;
	PRMDATA			*Prm;

	Bprm	= Axis->UniPrms->Bprm;
	Prm		= Axis->UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		位相補償パラメータ計算：遅れ補償															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				         80 * nos * Pole * 65536       PhsCmp * 65536 * (PerOvrSpd+10000)			*/
/*		SGDS  Leang = -----------------------------	+ ------------------------------------			*/
/*		Rotary	       10^6 * 60 * 2 * 15000 * Cx             360 * 10000 * 15000	       			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      Dx * OvrSpd * Pole * 65536		  Dx * OvrSpd * Pole					*/
/*		SVOS  Kpcmpd = --------------------------------	= ---------------------------				*/
/*		Rotary		    10^6 * 2 * PAI * 2 * 2^24 * Cx		10^6 * 3216.990877 * Cx					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PerOvrSpd : ＯＳ検出レベル [0.01%]															*/
/*		Pole      : モータポール数																	*/
/*		nos       : ＯＳ速度 [r/min],[mm/s]															*/
/*		OvrSpd    : ＯＳ速度 [rad/s],[m/s]															*/
/*		Dx        : 遅れ時間 [us] --> カーネル構成定義(*.cfg)で指定									*/
/*		Cx        : バグ補正 [--] --> カーネル構成定義(*.cfg)で指定									*/
/*		注意(Cx=16) : モータが本条件に合わせてある場合は、バグ修正不可、リニアはバグなし			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
		if( ((Prm->dq_sw & 0x0800) == 0 ) || ((Prm->MencP.flg_wf & 0x0001 ) == 0) )
		{
#if (FLOAT_USE==TRUE)
			fx = Bprm->OvrSpd * (LONG)SVCDEF_PCMP_DLYUS / 321699088.0f;
			Axis->BaseControls->PcmpCalcData.Kpcmpd = fx * (float)Prm->MencP.oslv_pol.b.h / (10.0f * (LONG)SVCDEF_PCMP_DLYCX);
#else
			kx = MlibScalKskxkx( Bprm->OvrSpd, (LONG)SVCDEF_PCMP_DLYUS, 321699088, &sx, 0 );
			Axis->BaseControls->PcmpCalcData.Kpcmpd =
					MlibPcalKxgain( kx, (LONG)Prm->MencP.oslv_pol.b.h, (10 * (LONG)SVCDEF_PCMP_DLYCX), &sx, 24 );
#endif /* FLOAT_USE */

		}
		else
		{
#if (FLOAT_USE==TRUE)
			fx = Bprm->OvrSpd * (float)SVCDEF_PCMP_DLYUS / 321699088.0f;
			Axis->BaseControls->PcmpCalcData.Kpcmpd = fx * (float)Prm->MencP.oslv_pol.b.h / 10.0f;
#else
			kx = MlibScalKskxkx( Bprm->OvrSpd, (LONG)SVCDEF_PCMP_DLYUS, 321699088, &sx, 0 );
			Axis->BaseControls->PcmpCalcData.Kpcmpd =
											MlibPcalKxgain( kx, (LONG)Prm->MencP.oslv_pol.b.h, 10, &sx, 24 );
#endif /* FLOAT_USE */
		}
	}
	else if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
#if (FLOAT_USE==TRUE)
		fx = Bprm->OvrSpd * (LONG)SVCDEF_PCMP_DLYUS / 51200.0f;
		Axis->BaseControls->PcmpCalcData.Kpcmpd = fx / Prm->MencP.ppitch;
		Axis->BaseControls->PcmpCalcData.Kpcmpd2 = Axis->BaseControls->PcmpCalcData.Kpcmpd;
#else
		kx = MlibScalKskxkx( Bprm->OvrSpd, (LONG)SVCDEF_PCMP_DLYUS, 51200, &sx, 0 );
		Axis->BaseControls->PcmpCalcData.Kpcmpd = MlibPcalKxgain( kx, 1, Prm->MencP.ppitch, &sx, 24 );
		Axis->BaseControls->PcmpCalcData.Kpcmpd2 = Axis->BaseControls->PcmpCalcData.Kpcmpd;
#endif /* FLOAT_USE */
	}

	/* 位相補間量演算係数計算 */
#if (FLOAT_USE==TRUE) /* SVN_IRIS_MODE || INGRAM_MODE */
		fx = Bprm->OvrSpd * KPI_MACYCLENS / 160849544.0f;
		Axis->BaseControls->PcmpCalcData.KpcmpdItplt = fx * Bprm->Keangle / 10000.0f;
#else
	kx = MlibScalKskxkx( Bprm->OvrSpd, KPI_MACYCLENS, 160849544, &sx, 0 );
	Axis->BaseControls->PcmpCalcData.KpcmpdItplt =
							MlibPcalKxkskx( kx, Bprm->Keangle, 10000, &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		位相補償パラメータ計算：位相補償１															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PcmpSpd1 = NorMaxSpd * PhsCmpSpd1 / MotMaxSpd		位相補償開始速度						*/
/*																									*/
/*						  PhsCmpDeg1 * 65536														*/
/*		Kpcmpsp1 = --------------------------------			位相補償演算ゲイン						*/
/*					 360 * (NorMaxSpd - PcmpSpd1)													*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PhsCmpDeg1 : 位相補償値１			[deg/MaxSpd]											*/
/*		PhsCmpSpd1 : 位相補償開始速度１		[100r/min]												*/
/*		MotMaxSpd  : モータ最大速度			[100r/min]												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Prm->phscmp != 0 )							/* サーボパックパラメータ != 0				*/
	{
		PhsCmpDegx = (LONG)Prm->phscmp;				/* サーボパックパラメータ使用				*/
	}
	else if( Prm->MencP.phscmpM1.b.l == 0xFF )			/* モータパラメータが、Ｆ詰めの場合			*/
	{
		PhsCmpDegx = (LONG)Prm->phscmp;				/* サーボパックパラメータ使用				*/
	}
	else											/* モータパラメータが、Ｆ詰めでない場合		*/
	{
		PhsCmpDegx = (LONG)Prm->MencP.phscmpM1.b.l;		/* モータパラメータ使用						*/
	}
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fx = (KSGAIN)Bprm->NorMaxSpd * (KSGAIN)Prm->MencP.phscmpM1.b.h / (KSGAIN)Bprm->MaxVel;
	Axis->BaseControls->PcmpCalcData.PcmpSpd1 = fx;
#else
	kx = MlibScalKxgain( Bprm->NorMaxSpd, Prm->MencP.phscmpM1.b.h, Bprm->MaxVel, NULL, -30 );
	Axis->BaseControls->PcmpCalcData.PcmpSpd1 = kx;
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	if( Prm->MencP.phscmpM1.b.h >= Bprm->MaxVel )		/* 開始速度チェック						*/
	{
		Axis->BaseControls->PcmpCalcData.Kpcmpsp1 = 0;
		/* A.040 : パラメータ設定異常 */
		ALMSetPramError( Axis->AlmManager, pndef_phscmpM1.RegNumber );
	}
	else
	{
#if (FLOAT_USE==TRUE)
		fx = (KSGAIN)(Bprm->NorMaxSpd - Axis->BaseControls->PcmpCalcData.PcmpSpd1) * 360.0f;
		Axis->BaseControls->PcmpCalcData.Kpcmpsp1 = (KSGAIN)PhsCmpDegx * 65536.0f / fx;
#else
		kx = MlibScalKxgain( PhsCmpDegx, 65536, 360, &sx,  0 );
		kx = MlibPcalKxgain( kx, 1, (Bprm->NorMaxSpd - Axis->BaseControls->PcmpCalcData.PcmpSpd1), &sx, 24 );
		Axis->BaseControls->PcmpCalcData.Kpcmpsp1 = kx;
#endif /* FLOAT_USE */
	}

#ifdef CSW_SCANA_ENABLE
	Axis->BaseLoops->PhaseComp.conf.Kpcmpd = Axis->BaseControls->PcmpCalcData.Kpcmpd;
	Axis->BaseLoops->PhaseComp.conf.Kpcmpd2 = Axis->BaseControls->PcmpCalcData.Kpcmpd2;
	Axis->BaseLoops->PhaseComp.conf.Kpcmpsp1 = Axis->BaseControls->PcmpCalcData.Kpcmpsp1;
	Axis->BaseLoops->PhaseComp.conf.PcmpSpd1 = Axis->BaseControls->PcmpCalcData.PcmpSpd1;
	Axis->BaseLoops->PhaseComp.conf.KpcmpdItplt = Axis->BaseControls->PcmpCalcData.KpcmpdItplt;
#endif /* CSW_SCANA_ENABLE */

/*--------------------------------------------------------------------------------------------------*/
/*	位相補償２は機能未使用かつイニシャル処理時間短縮のため削除する。								*/
/*--------------------------------------------------------------------------------------------------*/

}


/****************************************************************************************************/
/*																									*/
/*		 モータ静音用速度ＦＢLPF初期設定 <S0DA>														*/
/*																									*/
/*		概要：エンコーダパラメータにより速度ＦＢにローパスフィルタを追加する						*/
/*				実際の処理はPn308：速度FBフィルタ時定数にモータ静音用フィルタ時定数を加算する		*/
/*																									*/
/****************************************************************************************************/
static void IprmcalSpdFbFil( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	MENCV		*MencV;

	Prm   = Axis->UniPrms->Prm;
	Bprm  = Axis->UniPrms->Bprm;
	MencV = Axis->MencV;

	if( ( Prm->MencP.flg_wf & 0x0004 )								/* モータ静音設定選択フラグ		*/
			|| (MencV->TuningLessLowGainMotor == TRUE))				/* モータ機種判別				*/
	{
		Bprm->SpdFbLpassFil = Prm->fbfil2;							/* 速度FBﾌｨﾙﾀ追加				*/
	}
	else
	{
		Bprm->SpdFbLpassFil = 0;									/* 速度FBﾌｨﾙﾀ0.1[ms]追加		*/
	}

	return;

}

/*<S1D0> START*/
/*****************************************************************************
  Description: モータ最高速度処理
------------------------------------------------------------------------------
  Parameters:
	CfgPrm              cfgパラメータ定義
	Bprm				基本パラメータ定義
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdetCheckMotorOverSpeed(PRMDATA *Prm, BPRMDAT *Bprm, LONG AxisMotType )
{
	if( AxisMotType == MOTTYPE_LINEAR )
	{/* リニア型 */
		Bprm->OsAlmLvl = Bprm->NorOvrSpd;
	}
	else
	{/* 回転型 */
		Bprm->OsAlmLvl = MlibDiv6432( Prm->maxvel, Bprm->NorOvrSpd, ( Bprm->MaxVel * 110 ) );
	}
}
/*<S1D0> END*/

/* <S219> Start */
/****************************************************************************************************/
/*																									*/
/*		サーボオン時内蔵ファン停止アラーム検出時間の設定											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ファン停止ワーニング発生後の時間(アラームへの移行時間)の設定を行う。					*/
/*																									*/
/*			1. パラメータ																			*/
/*				1) PnEE6 : サーボオン時内蔵ファン停止アラーム検出時間 [min]							*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalFanLockAlmDetectTime( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA		*Cprm;
	POW_MNG_HNDL	*PowerManager;

	Cprm = Axis->UniPrms->Cprm;
	PowerManager = Axis->PowerManager;

	/* サーボオン時内蔵ファン停止アラーム検出時間取得(単位:[min]) */
	PowerManager->FanLock.conf.FanLockAlmDetectTime = (ULONG)(Cprm->SvonFanLock_Time * 60 * 10);
	return;
}
/* <S219> End */

/***************************************** end of file **********************************************/
