/****************************************************************************
Description	: Serial Encoder Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes	      :
	Name   Date          Description
	------------------------------------------------------------------------
	Y.Oka  2010.02.23    created
	I.M.   2011.02.08    reviewed

****************************************************************************/
#ifndef SERIAL_ENC_IF_H
#define SERIAL_ENC_IF_H


#include "EncData.h"
#include "JL086ServoIpHw.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"
#include "AlarmManager.h"		/* <S018> */
#include "SvOptCardIf.h"		/* <S04D> */

#define ECHO_DATA_NUM	7
#define	ENCODER_WAIT_TIMEOUT	2000	/* 2000[ms](2[s]) *//* <S223> */

/****************************************************************************************************/
/*		Encoder Error Definition																	*/
/****************************************************************************************************/
typedef enum SENC_ERROR_ {
	SENCERR_SUCCESS		= 0x0000,		/* 戻り値：通信正常											*/
	SENCERR_BUSY		= 0x0001,		/* 戻り値：処理中											*/
	/*----------------------------------------------------------------------------------------------*/
	SENCERR_PGCOM		= 0x0002,		/* 戻り値：A.C90 : エンコーダ通信異常						*/
	SENCERR_PGECHO		= 0x0004,		/* 戻り値：A.CB0 : エンコーダエコーバック異常				*/
	SENCERR_PRMDATA		= 0x0008,		/* 戻り値：A.CA0 : エンコーダパラメータ異常					*/
	SENCERR_PRMSUM		= 0x0010,		/* 戻り値：A.820 : エンコーダサムチェックアラーム			*/
	SENCERR_MTLMT		= 0x0020,		/* 戻り値：A.CC0 : マルチターンリミット値不一致				*/
	SENCERR_SCALE		= 0x0040,		/* 戻り値：A.080 : スケールピッチ設定異常					*/
	SENCERR_PHASEANGLE	= 0x0080,		/* 戻り値：A.C22 : 位相情報不一致							*/
	SENCERR_POLESENSOR	= 0x0100,		/* 戻り値：A.C21 : ポールセンサ異常							*/
	SENCERR_BRCHNGFAIL	= 0x0200,		/* 戻り値：A.C93 : エンコーダ通信システム異常				*/
	SENCERR_PGCLR		= 0x0400,		/* 戻り値：A.C80 : エンコーダクリア異常						*//* <S1BE> */
	SENCERR_NOTSUPPORT	= 0x0800,		/* 戻り値：A.051 : 製品未サポートアラーム					*//* <S1BE> */
	/*----------------------------------------------------------------------------------------------*/
} SENC_ERROR;



 /* <S003> */
/****************************************************************************************************/
/*		Baud Rate Exchange Selection																*/
/****************************************************************************************************/
typedef enum SENC_BRCHNG_SUP_ {
	SENCBR_AUTO_SEL		= 0x0000,		/* エンコーダ通信速度切り替え選択							*/
	SENCBR_8MBPS_SEL	= 0x0001,		/* エンコーダ通信速度8MBps選択								*/
	SENCBR_4MBPS_SEL	= 0x0002,		/* エンコーダ通信速度4MBps選択								*/
} SENC_BRCHNG_SUP;
 /* <S003> */
 


/****************************************************************************************************/
/*		Baud Rate Exchange Definition																*/
/****************************************************************************************************/
typedef enum SENC_BRCHNG_SET_ {
	SENCBR_SUPPORT		= 0x0001,		/* エンコーダ通信速度切り替え対応							*/
	SENCBR_4MBPS_SUP	= 0x0002,		/* エンコーダ通信速度4MBps対応								*/
	SENCBR_8MBPS_SUP	= 0x0004,		/* エンコーダ通信速度8MBps対応								*/
} SENC_BRCHNG_SET;


/****************************************************************************************************/
/*		Expand area Use Definition										<S1A6>						*/
/****************************************************************************************************/
typedef enum SENC_EXAREA_SET_ {
	SENCBR_EXAREA1		= 0x0001,		/* エンコーダ拡張領域1使用									*/
} SENC_EXAREA_SET;



/****************************************************************************************************/
/*		DivSet Error Definition																		*/
/****************************************************************************************************/
typedef enum DIVSET_ERROR_ {
	DIVSET_SUCCESS = 0,	/* 戻り値：分周設定正常						*/
	DIVSET_PRMERR_R,	/* 戻り値：パラメータ設定異常(回転)　:A.040	*/
	DIVSET_PRMERR_L,	/* 戻り値：パラメータ設定異常(リニア):A.040	*/
	DIVSET_DIVERR,		/* 戻り値：分周パルス設定異常:A.041			*/
} DIVSET_ERROR;

/* API's for Serial Encoder Operations */
/* for scan service */
BOOL LmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq );
BOOL RmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq );
LONG FencReadPosition(MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	/* <S04D> *//* <S0E1>:AlmManager追加 */
						void *BaseLoops );/*<S1B7>*/
/* for initializing Motor encoder *//* <S04D>:FbkOpt,SencFbOptUse追加 */
SENC_ERROR	SencInitMotorEncoder( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,	ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse );
										
SENC_ERROR	SencCalcBaseParameters( MENCV *MencV, MENC_INIT_PRM *MencInitPrm );
SENC_ERROR SencInitMencVariables(MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencInPrm);
void	SencMakeAbsPosition(MENCV *MencV, USHORT limmlt, DBYTEX bit_dp, BOOL RvsDir);
DIVSET_ERROR SencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );
BOOL	SencSetEncoderType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2, BOOL SencFbOptUse ); /* <S16A> */
/*--------------------------------------------------------------------------------------------------*/
/* シリアルエンコーダＰＡＯ相シリアル出力												<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void	SencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							 BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag );
/*--------------------------------------------------------------------------------------------------*/
/* 初期インクレパルス出力処理															<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void	SencOutIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg );				/* 回転		*/
void	LmxOutSencIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg, UCHAR Sftx );/* リニア	*//* <S209> */
/*--------------------------------------------------------------------------------------------------*/


/* for initializing Fully-closed encoder */
SENC_ERROR FencInitEncoder( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,	ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse, BOOL rvsDir);/*<S02D>*//* <S04D> */
void FencBasePrmCalc(MENCV *FencV, MENCPRM *MencP, USHORT b_prm2, ULONG fcplsml, BOOL rvsDir);/*<S02D>*/


SENC_ERROR FencInitVariables(MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg,
                             MENCPRM *MencP, USHORT scnvorgoffset, BOOL OrgOffsetEnable,
                             LONG KConvLoadSpd, LONG sfgfail_b_h);
UCHAR FencGetFencAlarm(MENCV *FencV);
/*--------------------------------------------------------------------------------------------------*/
/* フルクエンコーダ：分周パルスＩＦ初期化												<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
DIVSET_ERROR FencSencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *FencV, UNI_PRM_STR *UniPrms );
/*--------------------------------------------------------------------------------------------------*/
/* フルクエンコーダ：初期インクレパルス出力処理											<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void  FencSencOutIniIncPulse( MENCV  *FencV, ASIC_HW_IF_STR  *AsicHwReg );
/*--------------------------------------------------------------------------------------------------*/
/* フルクエンコーダ：シリアルエンコーダＰＡＯ相シリアル出力								<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void  FencSencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							   BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag );
/*--------------------------------------------------------------------------------------------------*/

/* common functions(SerialEnc.c) */
void SencInitChannel( MENCV *MencV, UINT ch, ASIC_HW_IF_STR *encAsicHwReg, ASIC_HW_IF_STR *posCmpAsicHwReg );/* <S04D> */
SENC_ERROR SencConnect( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg, USHORT EncBaudrate,  
										OPFBK_HNDL *FbkOpt, BOOL SencFbOptUse );/* <S04D>:FbkOpt,SencFbOptUse追加 */
SENC_ERROR SencExchngBaudrate( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,
										ASIC_HW_IF_STR *AsicHwReg, UCHAR BaudRate, BOOL *EncExchgSupported);/*<S09C>*/
SENC_ERROR SencReadParamAll( MENCV *MencV, UCHAR *PrmBuf );
SENC_ERROR SencSendRstOrgCmd(MENCV *MencV);
SENC_ERROR SencReadInitialP1Data(MENCV *MencV);
void SencChangeP0Mode( MENCV *MencV );
void SencInitChangeP0Mode( MENCV *MencV );			/* <S0F8> */
void SencChangeP1Mode( MENCV *MencV );
void SencSetIntCycleP0Mode( MENCV *MencV );			/* <S1D3> */
void SencSetIntCycleP1Mode( MENCV *MencV );
BOOL SencReadParam8byte( SENCX *Enc, USHORT Addr, UCHAR *PrmBuf );
BOOL SencBankChange( SENCX *Enc, USHORT BankNum );/* <S0D7> */
void SencSetIDINFO( USHORT addr, UCHAR *buffer, IDINFO *idinfo );
void SencFillNullIDINFO( IDINFO *idinfo );

SENC_ERROR SencChkConnect( SENCX *Enc );/* <S04D> */

SENC_ERROR SencResetMultiturn(MENCV *MencV, ULONG limmlt, ULONG bit_dp_l_h);
SENC_ERROR SencChangeMultiturnLimit(MENCV *MencV, USHORT mtlmt);
SENC_ERROR SencResetScaleZeroPosition( MENCV *MencV );
SENC_ERROR SencWriteMotorPhaseOffset( MENCV *MencV, USHORT PhaseOffset );
SENC_ERROR SencReadMotorParameter(MENCV *MencV, USHORT address, UINT num, USHORT *buffer);
SENC_ERROR SencWriteMotorParameter(MENCV *MencV, USHORT address, UINT num, USHORT *buffer);
#if 0 /*<S02D>*/
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV );
#else/*<S02D>*/
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV, PRM_ACCCMD Cmd, LONG Data );
#endif/*<S02D>*/

/* 2012.08.24 Y.Oka Make Mercury Base Software */
void	SencReadPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
											ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops );/* <S018> *//*<S0C5>*//*<S1B7>*/
/* 2012.08.24 Y.Oka Make Mercury Base Software */
LONG	TimeBasesMotSpdCalc( VFBTIMBASE *Vfbtimbase, MENCV *MencV, LONG dPos, LONG dPosSpd,
																	BOOL VcompSts, BOOL EnableFlag ); /* <S067> */
/* <S1B7> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/* モータレステスト																		<S00X>		*/
/*--------------------------------------------------------------------------------------------------*/
/* --- セミクローズドエンコーダ --- */
/* SencScanService.cに定義 */
/* モータレス：エンコーダ変数初期化処理	*/
void LpxSencMotLessInitVariables( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg );
/* モータレス：回転型位置データ読込み	*/
void RmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	void *BaseLoops );
/* モータレス：リニア位置データ読込み	*/
void LmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	void *BaseLoops );
/* モータレス：エンコーダパラメータ設定処理	*/
void LpxSetSencInfoMotorLessTestMode( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 );

/* --- フルクローズドエンコーダ --- */
/* SencScanService.cに定義 */
/* モータレス：エンコーダ変数初期化処理	*/
void LpxFencMotLessInitVariables( MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg );
/* モータレス：エンコーダパラメータ設定処理	*/
void LpxSetFencInfoMotorLessTestMode( MENCV *FencV, MENCPRM *MencP );
/* モータレス：位置データ読込み	*/
LONG LpxReadMotorLessFencPosition( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops );
/* <<<<< <S1B7> */


#endif /* SERIAL_ENC_IF */
/***************************************** end of file **********************************************/
