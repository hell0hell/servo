/****************************************************************************************************/
/*																									*/
/*																									*/
/*		JL076Hw.h : JL076B用H/W定義																	*/
/*			※ファイル名はJL086だが、中身はJL076※													*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.11.01	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_JL076HW_H
#define	_JL076HW_H

#include "JL086ServoIpReg.h"
#include "MLib.h"
#include "KLib.h"

/****************************************************************************************************/
/*		STRUCTURES																					*/
/****************************************************************************************************/
typedef struct _SVIP_INIT_PRM
{
	USHORT	prstsw;		/* Pn200: 位置制御指令形態選択スイッチ */
	USHORT	syssw1;		/* PnE0B: システムスイッチ1 */
	USHORT	syssw2;		/* PnE0C: システムスイッチ2 */
	USHORT	comfil;		/* PnE0E: 指令入力フィルタ選択時の指令フィルタ設定 */
	DBYTEX	odt_c;		/* PnE2D: 下位:オンディレイ時間, 上位:オンディレイ補償定数 */
} SVIP_INIT_PRM;

typedef enum _SVIP_INIT_ERR {
	SVIP_NOALM		= 0,
	SVIP_PRMERR		= 0x00000001,		/* A.040 : パラメータエラーの設定	*/
} SVIP_INIT_ERR;


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/* 不要 */
//#define JL076B_ASICID_CODE	0x3400							/* JL-077 rev.B ID					*/
//#define JL076B_COMADR_OFS		0x80000							/* 全軸共通ASICレジスタオフセット数	*/
#define JL086_MICRO_OFS			0x7000							/* μレジスタオフセット数			*/
#define JL086_MICRO_AXIS_OFS	0x01000000						/* 1軸毎のASICレジスタオフセット数	*/

//#define JL076B_OFS			0x10000							/* 1軸毎のASICレジスタオフセット数	*/
//#define JL076B_MICRO_OFS		0x7000							/* μレジスタオフセット数			*/
//#define JL076B_COMADR_OFS		0x80000							/* 全軸共通ASICレジスタオフセット数	*/
#define ASIC_MREGSIZE			0x0E00							/* ASIC MP Reg Byte Size			*/


/* ハードウェア出力信号初期値定義 */
#define ASIC_CLKMHZ				80							/* ASIC Clock [MHz]					*/

/*--------------------------------------------------------------------------------------------------*/
/*		分周モード状態及び分周Ｃ相マスク状態設定値用定マクロ										*/
/*--------------------------------------------------------------------------------------------------*/
#define DIV_MODE_NORMAL	0x00			/* 分周スルーモード時の設定値						*/
#define DIV_MODE_DCC		0x01			/* 分周モード1時の設定値							*/
#define DIV_MODE_DCC2		0x02			/* 分周モード2時の設定値							*/
#define DIV_CMASK_ON		0x01			/* 分周C相マスクON時の設定値						*/
#define DIV_CMASK_OFF		0x00			/* 分周C相マスクOFF時の設定値						*/

/* エンコーダ通信用定義 */
#define SENC_COMRATE_4MBPS	0x0000				/* エンコーダ通信速度4M BPS 設定					*/
#define SENC_COMRATE_8MBPS	0x0001				/* エンコーダ通信速度8M BPS 設定					*/
#define ENC_BUF_SIZE_S		8					/* Encoder communication Buffer Size (SHORT)		*/


/****************************************************************************************************/
/*		Low level ASIC Access API's																	*/
/****************************************************************************************************/
#define SHAL_FAULT_OH		0x0010		/* Over Current */
#define SHAL_FAULT_OC		0x0020		/* Over Current */
#define SHAL_FAULT_WDOUT	0x0200		/* host cpu watch dog time over. */
#define SHAL_FAULT_BB		0x0400		/* Base Block */
#define SHAL_FAULT_GUP1		0x0800		/* Watch dog time over in the Micro.*/
#define SHAL_FAULTS			0xffff		/* All bit */

/*--------------------------------------------------------------------------------------------------*/
/*		カウンタラッチ(CMD,FB,FC)																	*/
/*--------------------------------------------------------------------------------------------------*/
#define LODSET_SET_LDCMD	0x02C0


/* JL-077 故障入力状態取得 */
__inline USHORT SHalDrv_GetFaultStatus(ASIC_HW_IF_STR *AsicHwReg, USHORT mask)
{
	return (AsicHwReg->AREG_FLTSTAT & mask);
}

/* JL-077 マイクロ演算部WDT状態取得 */
__inline USHORT SHalDrv_GetMicroWdtStatus(ASIC_HW_IF_STR *AsicHwReg)
{
	return (AsicHwReg->AREG_WDT1SET & 0x0040);
}

/* JL-077 アラームラッチリセット */
__inline void SHalDrv_ResetAlarmLtStatus(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_ALMCLR = 0x802F;
}

/* AD変換デジメーションフィルタのアラームクリア */
__inline void SHalDrv_ResetCrtAdAlarmStatus(ASIC_U_IF_STR *AsicMicroReg)
{
	AsicMicroReg->MREG_FCCRST = 0x01;
}

/* JL-077 BBON信号ラッチリセット */
__inline void SHalDrv_ResetBBONLtStatus(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_ALMCLR |= (BIT2 | BIT3);
}

/* JL-077 BBON信号状態取得 */
__inline void SHalDrv_GetBBONStatus(ASIC_HW_IF_STR *AsicHwReg, BOOL *Bbon1, BOOL *Bbon2)
{
	USHORT reg = AsicHwReg->AREG_ALMSET;
	*Bbon1 = (reg >> 11) & 0x1;
	*Bbon2 = (reg >> 12) & 0x1;
}

/* JL-077 BBON信号ラッチ状態取得 */
__inline void SHalDrv_GetBBONLtStatus(ASIC_HW_IF_STR *AsicHwReg, BOOL *Bbon1, BOOL *Bbon2)
{
	USHORT reg = AsicHwReg->AREG_SETREG1;
	*Bbon1 = (reg >> 8) & 0x1;
	*Bbon2 = (reg >> 9) & 0x1;
}

/* JL-077 DBON信号出力有効 */
__inline void SHalDrv_EnableDynamicBrake(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_TIM1SET = 0x009C;
}

/* JL-077 DBON信号ON */
__inline void SHalDrv_DynamicBrakeON(ASIC_HW_IF_STR *AsicHwReg)
{
#ifndef CSW_CSTR_DB_REVERSE
	AsicHwReg->AREG_ALMSET &= ~(BIT5);
#else
	AsicHwReg->AREG_ALMSET |= BIT5;
#endif
}

/* JL-077 DBON信号OFF */
__inline void SHalDrv_DynamicBrakeOFF(ASIC_HW_IF_STR *AsicHwReg)
{
#ifndef CSW_CSTR_DB_REVERSE
	AsicHwReg->AREG_ALMSET |= BIT5;
#else
	AsicHwReg->AREG_ALMSET &= ~(BIT5);
#endif
}

/* JL-077 Charge Pump ON */
__inline void SHalDrv_ChargePumpON(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_PUMPON = 0x02FF;
}

/* JL-077 Charge Pump OFF */
__inline void SHalDrv_ChargePumpOFF(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_PUMPON = 0x00FF;
}

/* <S0CB>	STart */
/* JL-077 Charge Pump(P側) ON */
__inline void SHalDrv_ChargePumpON_P(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_PUMPON = 0x04FF;
}
/* <S0CB>	End */

/* JL-077 Base Block ON */
__inline void SHalDrv_BaseBlock(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_SETREG1 = 0x0011;/* BaseBlock */
	AsicHwReg->MREG_CTSTR = MPCMD_SVOFF;/* Micro ServoOff */
	AsicHwReg->AREG_WDT1SET |= BIT6;/*  Stop Micro WDT */
}

/* JL-077 Base Enable ON */
__inline void SHalDrv_BaseEnable(ASIC_HW_IF_STR *AsicHwReg, ASIC_U_IF_STR *AsicMicroReg)
{
	AsicMicroReg->MREG_FCCRST = 0x01;	/* Reset Current A/D */
	AsicHwReg->MREG_CTSTR = MPCMD_SVON;/* Micro ServoOn */
	AsicHwReg->AREG_WDT1SET &= ~(BIT6);/* start Micro WDT for JL076FPGA */
	AsicHwReg->AREG_SETREG1 = 0x0010;/* Base Enable */
}

/* JL-077 WDT Kick */
__inline void SHalDrv_KickWDT( ASIC_HW_IF_STR *AsicHwReg, BOOL TestMode4 )
{
	if( TestMode4 == FALSE)
	{
		AsicHwReg->AREG_HINT1 = 0x0000;
	}
}

/* JL-077 Latch ASIC Pulse Counter */
__inline void SHalDrv_LatchPulseCntr( ASIC_HW_IF_STR *AsicHwReg )
{
	AsicHwReg->AREG_LODSET = LODSET_SET_LDCMD;
}

/* JL-077 Read Reference A/D */
__inline LONG SHalDrv_ReadRefAd( ASIC_HW_IF_STR *AsicHwReg )
{
#if 0/* 電圧指令が負の値であるときはｷｬｽﾄする必要がある。	*/
	AsicHwReg->AREG_ADSET1 |= BIT14;
	return (AsicHwReg->AREG_ACUL);
#endif
	SHORT	RefAd;
	AsicHwReg->AREG_ADSET1 |= BIT14;
	RefAd = (SHORT)AsicHwReg->AREG_ACUL;
	return ((LONG)RefAd);
}

/* Set Encoder Transmit Data to JL-086 */
__inline void SHalDrv_SencSetTxData( void *TxBufAdr, void *TxData )
{
	UCHAR	i;
	for( i=0; i<ENC_BUF_SIZE_S; i++ )
	{
		((USHORT*)TxBufAdr)[i] = ((USHORT*)TxData)[i];
	}
}

/* Get Encode Received Data from JL-086 */
__inline void SHalDrv_SencGetRxData( void *RxBufAdr, void *RxData )
{
	UCHAR	i;
	for( i=0; i<ENC_BUF_SIZE_S; i++ )
	{
		((USHORT*)RxData)[i] = ((USHORT*)RxBufAdr)[i];
	}
}

/* Set Encode Transmission Start Request to JL-086 */
__inline void SHalDrv_SencStartTx( void *TxStartAdr, void *TxCmd )
{
	*((USHORT*)TxStartAdr) = *((USHORT*)TxCmd);
}

/* Get Encode Receive Status from JL-086 */
__inline USHORT SHalDrv_SencGetRxStatus( void *RxFlgAdr )
{
	return	*((USHORT*)RxFlgAdr);
}

/* Enable the Encoder Interrupt */
__inline void SHalDrv_EnableEncoderInterrupt( ASIC_HW_IF_STR *AsicHwReg )
{
	AsicHwReg->AREG_INT1SET |= BIT5;
}

/* Sync the Encoder Communication with CPU Operation */
__inline void SHalDrv_SyncEncoderCommunication( ASIC_HW_IF_STR *AsicHwReg, USHORT* RxFlgAdr )
{
	*RxFlgAdr |= BIT8;				/* TXKCS    = 1	*/
	AsicHwReg->AREG_ESYNC = 0x0000;	/* ESYNC0,1 = 0	*/
	KlibWaitns( 200 );				/* Wait 200ns + ReadModifyWrite AREG	*/
	*RxFlgAdr &= ~BIT8;				/* TXKCS    = 0	*/
}

/* Enable the Encoder Interrupt */
__inline USHORT SHalDrv_AckEncoderInterrupt( ASIC_HW_IF_STR *AsicHwReg )
{
	USHORT dummy;
	dummy = AsicHwReg->AREG_INT1SET;
	return dummy;
}

/* Change Encoder Communication Mode to P0 */
__inline void SHalDrv_ChangeToP0Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
		AsicHwReg->AREG_SEPGSET00	&= (~BIT0);
		AsicHwReg->AREG_SEPGSET01	= 0x0001;
		AsicHwReg->AREG_SE0_TXD0	= 0x0000;
		AsicHwReg->AREG_SRCNT0		= 0x03E7;		/* 125us */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10	&= (~BIT0);
		AsicHwReg->AREG_SEPGSET11	= 0x0001;
		AsicHwReg->AREG_SE1_TXD0	= 0x0000;
		AsicHwReg->AREG_SRCNT1		= 0x03E7;		/* 125us */
	}
}

/* Change Encoder Communication Mode to P1 */
__inline void SHalDrv_ChangeToP1Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
//		AsicHwReg->AREG_SEPGSET00 = (AsicHwReg->AREG_SEPGSET00 & ~(BIT12|BIT13)) | BIT0;
		AsicHwReg->AREG_SEPGSET00 &= (~BIT13);
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F4FF;
		AsicHwReg->AREG_SEPGSET01 |= 0x00000400;	/*P1モード設定 */
		AsicHwReg->AREG_SRCNT0 = 0x000001F3;		/* 62.5us */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10 = (AsicHwReg->AREG_SEPGSET10 & ~(BIT12|BIT13)) | BIT0;
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F4FF;
		AsicHwReg->AREG_SEPGSET11 |= 0x00000400;	/*P1モード設定 */
		AsicHwReg->AREG_SRCNT1 = 0x000001F3;		/* 62.5us */
	}
}

/* Set Encoder communication Rate */
__inline void SHalDrv_SetSencComRate( ASIC_HW_IF_STR *AsicHwReg, UCHAR ComRate )
{
	if( ComRate == SENC_COMRATE_4MBPS )
	{
		AsicHwReg->AREG_SETREG1 = 0x0005;		/* ASIC Write Enable		*/
		AsicHwReg->AREG_CLKO &= 0xF0FF;			/* Set Ch0 and ch1 to 4Mbps	*/
		AsicHwReg->AREG_SETREG1 = 0x0001;		/* ASIC Write Disable		*/
	}
	else
	{
		// ★8MBPS設定追加予定★
	}
}

/* Set Current Position for Divide Out */
__inline void SHalDrv_SetDivOutPosition( ASIC_HW_IF_STR *AsicHwReg, LONG Pos )
{
	AsicHwReg->AREG_POSET0 = (USHORT)Pos;
}

/* Set Origin Correction 1 for Divide Out */
__inline void SHalDrv_SetOriginCorrect1( ASIC_HW_IF_STR *AsicHwReg, LONG CorrectVal )
{
	AsicHwReg->AREG_POSET1 = (USHORT)CorrectVal;
}

/* Set Origin Correction 2 for Divide Out */
__inline void SHalDrv_SetOriginCorrect2( ASIC_HW_IF_STR *AsicHwReg, LONG CorrectVal )
{
	AsicHwReg->AREG_POSET2 = (USHORT)CorrectVal;
}

/* Get H/W Fault Status from MicroProgram */
__inline USHORT SHalDrv_GetMicroFaultSts( ASIC_U_IF_STR *AsicMicroReg )
{
	return	AsicMicroReg->MREG_FCCSTMON;
}

/* Set Carrer Frequency for MicroProgram */
__inline void SHalDrv_SetCarrerFreq( ASIC_U_IF_STR *AsicMicroReg, SHORT Carrer )
{
	AsicMicroReg->MREG_CRFQ = Carrer;
}

/* Set Carrer Frequency for MicroProgram */
__inline void SHalDrv_SetMotSpdToMicro( ASIC_U_IF_STR *AsicMicroReg, LONG MotSpd )
{
	SHORT	MotSpdMp;
	MotSpdMp = (SHORT)MlibMulhigh32( MotSpd, 3840000 );	/* Motor Speed for Micro 		*/
	AsicMicroReg->MREG_VELIN = MotSpdMp;				/* ＥＭＦ補償計算用速度			*/
}


/****************************************************************************************************/
/*		Latch Function 																				*/
/****************************************************************************************************/
#define SHAL_LTTIME_CLOCK			4									/* 4MHz */
#define SHAL_ENCLT_DELAY			((SHAL_LTTIME_CLOCK*565)/10)		/* 53.5us = 62.5us - 9(Tx delay) + 3(Signal delay)us */
//#define SHAL_LTPOS_GAIN			((0x7FFFFFFF/(KPI_SBCYCLEUS*SHAL_LTTIME_CLOCK))*2)
#define SHAL_LTPOS_GAIN(SBCycleUs)	((0x7FFFFFFF/(SBCycleUs*SHAL_LTTIME_CLOCK))*2)

typedef struct SHAL_LATCH_STS {
	VBOOL			Complete;
	VUSHORT			BaseTime;
	VUSHORT			LatchTime;

	LONG			LtPosCalcGain;
} SHAL_LATCH_STS;

typedef struct SHAL_LTSIGNALS {
	SHAL_LATCH_STS	ltSts[MAX_SLOT_NUMBER];
	ASIC_HW_IF_STR*	AsicHwReg[MAX_SLOT_NUMBER];
	INT				NumberOfSignals;
} SHAL_LTSIGNALS;

/* JL-077 Select an edge type of the latch signal  */
__inline void SHalDrv_SelectLatchEdge(BOOL posEdge, ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_STIMSET = posEdge << 1;
}

/* JL-077 Read Latch timer status */
__inline void SHalDrv_ReadLatchTime(SHAL_LATCH_STS *LtSts, ASIC_HW_IF_STR *AsicHwReg)
{
	LtSts->BaseTime = AsicHwReg->AREG_STIMLT0;
	LtSts->LatchTime = AsicHwReg->AREG_STIMLT1;
//	LtSts->Complete = (0 != (AsicHwReg->AREG_STIMSET & BIT9));
	LtSts->Complete = (AsicHwReg->AREG_STIMSET >> 9) & 1;
}

/* JL-077 Calculate delta latch position from latched time. */
__inline LONG SHalDrv_CalcDeltaLatchPos(SHAL_LATCH_STS *LtSts, LONG dPos)
{
	LONG dtime, dcmpPos;
	dtime = LtSts->BaseTime - LtSts->LatchTime - SHAL_ENCLT_DELAY;
	dcmpPos = MlibMulhigh32( dPos * dtime, LtSts->LtPosCalcGain);
	return dcmpPos;
}


/****************************************************************************************************/
/*		Initializing Servo ASIC functions 															*/
/****************************************************************************************************/
void SHal_InitAsicA( ASIC_HW_IF_STR* pAsicHwReg );
BOOL SHal_InitAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm );
BOOL SHal_PrstAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm );

BOOL SHal_InitMicroProgram( ASICS *SvAsicRegs );
BOOL SHal_LoadAsicMicroProgram( ASICS *SvAsicRegs, CUSHORT *src_adr, LONG u_size);
BOOL SHal_ResetAsicMicroRegister( ASICS *SvAsicRegs );
void SHal_StartMicroProgram( ASICS *SvAsicRegs );
void SHal_StopMicroProgram( ASICS *SvAsicRegs );
ULONG SHal_ClockToTime( USHORT clock_diff );


/****************************************************************************************************/
/*		Micro Program code																			*/
/****************************************************************************************************/
extern	const USHORT	YecAsicPrg_INGRAM[ ];
extern	const USHORT	JL076UprgSz_INGRAM;
extern	const USHORT	YecAsicPrg_SGDV[ ];
extern	const USHORT	JL076UprgSz_SGDV;



#endif	/* _JL076HW_H */
/***************************************** end of file **********************************************/
