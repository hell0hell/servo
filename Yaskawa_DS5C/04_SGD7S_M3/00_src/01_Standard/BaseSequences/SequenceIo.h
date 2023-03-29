/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SequenceIo.h : Local Header File for ScanC Software											*/
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
#ifndef	_SEQUENCE_CTRL_H
#define	_SEQUENCE_CTRL_H


#include "HwDeviceIf.h"
#include "BaseCmnStruct.h"
#include "BaseEnableSequence.h"
#include "AlarmManager.h"
#include "SftDiagFunc.h"	/*<S1F6>*/
#include "PnPrmStruct.h"	/*<S21D>*/


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/



/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/


/*--------------------------------------------------------------------------------------------------*/
/*		シーケンスI/O信号構造体																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct SEQ_IOSIGNAL {
	ULONG	PinNumber;		/* Sequence I/O pin number(=right shift number) */
	ULONG	XorMask;		/* Mask setting */
	ULONG	Mask;			/* Mask setting */

//<S045>	BOOL	SignalStatus;	/* SignalStatus = ((read value)>>PinNumber)^XorMask */
	union 
	{
		BOOL v;		/* <S045> SignalStatus(Input) = ((read value)>>PinNumber)^XorMask */
		BOOL *p;	/* <S045> SignalStatus(Output) = ((read value)>>PinNumber)^XorMask */
	} SignalStatus;
} SEQ_IOSIGNAL;

#if 0	/*<S1F6>*/
/*--------------------------------------------------------------------------------------------------*/
/*		HWBB検出用変数定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DET_HWBB_STS {
	BOOL			HwbbState;				/* Hard-wired Baseblock Status							*//* <HWBB> */
	BOOL			Bbon1;					/* BBON1 Signal											*//* <HWBB> */
	BOOL			Bbon2;					/* BBON2 Signal											*//* <HWBB> */
	BOOL			Bbon1Edge;				/* BBON1 Signal Rising Edge								*//* <HWBB> */
	BOOL			Bbon2Edge;				/* BBON2 Signal Rising Edge								*//* <HWBB> */
	BOOL			GateDrvDefect;			/* Gate Drive Defect Status Flag						*//* <HWBB> */
	BOOL			LackOneBbon;			/* One BBON Signal Input Only							*//* <HWBB> */
} DET_HWBB_STS;

/*--------------------------------------------------------------------------------------------------*/
/*		内部信号異常検出(Test Pulse Diagnostic)用定義									<S0F1>		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TEST_PULSE_DIAG {
	BOOL			DpIn1Enable;			/* DPIN1 Signal Enable									*/
	BOOL			DpIn2Enable;			/* DPIN2 Signal Enable									*/
	BOOL			DpIn1LastEnable;		/* DPIN1 Signal Enable (前回値)							*/
	BOOL			DpIn2LastEnable;		/* DPIN2 Signal Enable (前回値)							*/
	BOOL			DpIn1State;				/* DPIN1 Signal Status									*/
	BOOL			DpIn2State;				/* DPIN2 Signal Status									*/
	BOOL			DpIn1LastState;			/* DPIN1 Signal Status (前回値)							*/
	BOOL			DpIn2LastState;			/* DPIN2 Signal Status (前回値)							*/
} TEST_PULSE_DIAG;

#define	GET_DPIN1( )	(HALDRV_GET_GPIO06)
#define	GET_DPIN2( )	(HALDRV_GET_GPIO07)
#endif	/*<S1F6>*/


typedef	struct SEQ_IO_HNDL {
	DET_HWBB_STS		DetHwbbSts;				/* HWBB状態検出用データ								*/
	TEST_PULSE_DIAG		TestPulseDiag[2];		/* 内部信号異常検出用データ							*//*<S1F6>*/

	SEQ_IOSIGNAL		PotSignal;				/* P-OT Signal Information */
	SEQ_IOSIGNAL		NotSignal;				/* N-OT Signal Information */
	SEQ_IOSIGNAL		PclSignal;				/* <S045> P-CL Signal Information */
	SEQ_IOSIGNAL		NclSignal;				/* <S045> N-CL Signal Information */
	SEQ_IOSIGNAL		DecSignal;				/* DEC Signal Information */
	SEQ_IOSIGNAL		ExtSignal[3];			/* EXT Signal Information */
	SEQ_IOSIGNAL		EstpSignal;				/* E-Stop Signal Information */

	SEQ_IOSIGNAL		FstpSignal;				/* <S1B5> F-Stop Signal Information */
	SEQ_IOSIGNAL		ClinkansSignal;			/* <S1B5> CLINKANS Signal Information */
	SEQ_IOSIGNAL		CmcansSignal;			/* <S1B5> CMCANS Signal Information */
	SEQ_IOSIGNAL		ConvreadySignal;		/* <S1B5> CONV_READY Signal Information */
	SEQ_IOSIGNAL		ConvmconSignal;			/* <S1B5> CONV_MCON Signal Information */

	SEQ_IOSIGNAL		BkSignal;				/* BK Signal Information */

	SEQ_IOSIGNAL		CoinSignal;				/* <S045> COIN Signal Information */
	SEQ_IOSIGNAL		VcmpSignal;				/* <S045> VCMP Signal Information */
	SEQ_IOSIGNAL		TgonSignal;				/* <S045> TGON Signal Information */
	SEQ_IOSIGNAL		SrdySignal;				/* <S045> SRDY Signal Information */
	SEQ_IOSIGNAL		CltSignal;				/* <S045> CLT Signal Information */
	SEQ_IOSIGNAL		VltSignal;				/* <S045> VLT Signal Information */
	SEQ_IOSIGNAL		WarnSignal;				/* <S045> WARN Signal Information */
	SEQ_IOSIGNAL		NearSignal;				/* <S045> NEAR Signal Information */

	SEQ_IOSIGNAL		PreWrnSignal;			/* PreWrn Signal Information *//*<S154>*/

	SEQ_IOSIGNAL		Alo1Signal;				/* ALO1 Signal Information *//* <S04B> */
	SEQ_IOSIGNAL		Alo2Signal;				/* ALO2 Signal Information *//* <S04B> */
	SEQ_IOSIGNAL		Alo3Signal;				/* ALO3 Signal Information *//* <S04B> */

	SEQ_IOSIGNAL		CpulseSignal;			/* <S1B5> C-PULSE Signal Information */
	SEQ_IOSIGNAL		ClinkSignal;			/* <S1B5> CLINK Signal Information */
	SEQ_IOSIGNAL		CmconSignal;			/* <S1B5> CMCON Signal Information */
//	SEQ_IOSIGNAL		PmSignal;				/* <S1B5> PM Signal Information */	/* <S21D> */
	SEQ_IOSIGNAL		DaoSignal[2];			/* <S1B5> DAO Signal Information */
#ifdef HW_FBA_IO_AXIS
	SERIAL_INPUT_AX		SerialInputAx;			/* 同期シリアル入力							*/
	SERIAL_OUTPUT_AX	SerialOutputAx;			/* 同期シリアル出力							*/
#endif
#ifdef __BIG_ENDIAN__
	struct {
		UINT16		resvd3;
		UINT8		resvd2;
		UINT		resvd1:1;
		UINT		Ext3Signal:1;				/* EXT3 Signal Information */
		UINT		Ext2Signal:1;				/* EXT2 Signal Information */
		UINT		Ext1Signal:1;				/* EXT1 Signal Information */
		UINT		DecSignal:1;				/* DEC Signal Information */
		UINT		EstpSignal:1;				/* E-Stop Signal Information */
		UINT		NotSignal:1;				/* N-OT Signal Information */
		UINT		PotSignal:1;				/* P-OT Signal Information */
	} CmdIoInput;
#else
	struct {
		UINT		PotSignal:1;				/* P-OT Signal Information */
		UINT		NotSignal:1;				/* N-OT Signal Information */
		UINT		EstpSignal:1;				/* E-Stop Signal Information */
		UINT		DecSignal:1;				/* DEC Signal Information */
		UINT		Ext1Signal:1;				/* EXT1 Signal Information */
		UINT		Ext2Signal:1;				/* EXT2 Signal Information */
		UINT		Ext3Signal:1;				/* EXT3 Signal Information */
		UINT		resvd1:1;
		UINT8		resvd2;
		UINT16		resvd3;
	} CmdIoInput;
#endif

	UINT			HwSin;						/* @@ Analog/Pulse: Hardware Signal Input */
	ULONG			pot_mask;					/* P-ot Mask 暫定！！！kira<S032> */
	ULONG			not_mask;					/* N-ot Mask 暫定！！！kira<S032> */

} SEQ_IO_HNDL;


/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
BOOL SeqIoDetectHardWiredBB( DET_HWBB_STS *DetHwbb, LONG svon_req,
												ASIC_HW_IF_STR *pAsicHwReg, BOOL HwbbSupport );
#ifndef HW_FBA_IO_AXIS
void SeqIoBrakeControl( SEQ_IOSIGNAL*, BOOL*, BOOL, BOOL );
#else
void SeqIoBrakeControlAx( SEQ_IOSIGNAL*, BOOL*, BOOL, BOOL, SERIAL_OUTPUT_AX* );
#endif
void SeqIoOutputNetIf( SEQ_IO_HNDL *SeqIo, BE_SEQ_HNDL *BeSeq, SEQ_CTRL_OUT *SeqCtrlOut,
								 ALMMAN_HANDLE *AlmManager, MENCV *MencV/* <S082> BOOL PhaseReady */, BOOL LockNormalOpeDrive,
								 BOOL MotorLessTestMode );												/* <S1B7> */

void SeqIoSetOtSigMask( SEQ_IO_HNDL *SeqIo );
void SeqIoClearOtSigMask( SEQ_IO_HNDL *SeqIo, USHORT pot_selsw, USHORT not_selsw );

void SeqIoInputSeqSignals( SEQ_IO_HNDL *IoSignals, ASIC_HW_IF_STR *AsicHwReg );
void SeqIoInputSeqSignalsCom( SEQ_IO_HNDL *IoSignals, PRMDATA *Prm );		/* <S21D> */
void SeqIoOutputSeqSignals( long ax, SEQ_IO_HNDL *SeqIo );								/* <S045> *//* SGD7W IO入出力対応<S118> */

void SeqIoDetectTestPulseSignal( SEQ_IO_HNDL *pIoSignals, LONG AxisNo);						/* <S0F1> *//*<S1F6>*/
BOOL SeqIoDetectFstp( BOOL FstpSignal, SEQ_CTRL_OUT *hSeqCtrlOut, LONG Svon_req );   /* <S1F5> */


/*--------------------------------------------------------------------------------------------------*/
/*	<S045> Sequence Signal Input Macro Definition													*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	#define SEQIO_START_INSIGX( sigs )  					\
	{														\
		SEQ_IOSIGNAL		*SeqIo;							\
		SEQ_IO_HNDL 		*Sigs = (sigs);			
/*--------------------------------------------------------------------------------------------------*/
	#define SEQIO_GET_INSIGX( signal, in ) 															\
			SeqIo = &(Sigs->##signal);															\
			SeqIo->SignalStatus.v = 																\
				((((in) >> (SeqIo->PinNumber)) ^ (SeqIo->XorMask)) & (SeqIo->Mask));
/*--------------------------------------------------------------------------------------------------*/
	#define SEQIO_END_INSIGX( )  							\
	}
#else
	#define SEQIO_START_INSIGX( sigs ) 		{;}
	#define SEQIO_GET_INSIGX( signal, in ) 	{;}
	#define SEQIO_END_INSIGX( )  			{;}
/* <S21D> Start */
#define SEQIO_START_INSIGX_AP( sigs )  					\
{														\
	SEQ_IOSIGNAL		*SeqIo;							\
	SEQ_IO_HNDL 		*Sigs = (sigs);
/*--------------------------------------------------------------------------------------------------*/
#define SEQIO_GET_INSIGX_AP( signal, in, map ) 												\
		SeqIo = &(Sigs->##signal);															\
		SeqIo->SignalStatus.v = ( ((map) < 7) ? ((((in) >> (map)) ^ (SeqIo->XorMask)) & (SeqIo->Mask)) 	\
								: ( ((map) == 7) ? 1 										\
								: ( ((map) == 8) ? 0 										\
								: ((((in) >> ((map) - 9)) ^ (SeqIo->XorMask)) & (SeqIo->Mask))	\
								   ) ) );
/*--------------------------------------------------------------------------------------------------*/
#define SEQIO_END_INSIGX_AP( )  							\
}
/* <S21D> End */
#endif

/*--------------------------------------------------------------------------------------------------*/
/*	<S045> Sequence Signal Output Macro Definition													*/
/*--------------------------------------------------------------------------------------------------*/
//#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) /* Almcode出力のみA/P形も使用 *//* <S04B> */
	#define SEQIO_START_OUTSIGX( sigs )														\
	{																						\
		SEQ_IO_HNDL 		*Sigs = (sigs);													\
		SEQ_IOSIGNAL		*SeqIo;				
/*--------------------------------------------------------------------------------------------------*/
	#define SEQIO_SET_OUTSIGX( signal, outx )	 											\
		SeqIo = &(Sigs->##signal);															\
		(outx) |= 																			\
			((( (ULONG)(*(SeqIo->SignalStatus.p)) ^ SeqIo->XorMask) & SeqIo->Mask) << (SeqIo->PinNumber));
/*--------------------------------------------------------------------------------------------------*/
	#define SEQIO_END_OUTSIGX_ScanC( ax, outx )						/*<S118>*/							\
		HALdrv_SetSOutC( ax, outx );															\
	}
/*--------------------------------------------------------------------------------------------------*/
//<S081> #if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* <S04B> */
	#define SEQIO_END_OUTSIGX_ScanB( ax, outx )						/*<S118>*/						\
		HALdrv_SetSOutB( ax, outx );															\
	}
//<S081> #else
//	#define SEQIO_START_OUTSIGX( seqio )				{;}/* <S04B> */
//	#define SEQIO_SET_OUTSIGX( signal, outx )		  	{;}/* <S04B> */
//	#define SEQIO_END_OUTSIGX_ScanC( outx )				{;}/* <S04B> */
//<S081> 	#define SEQIO_END_OUTSIGX_ScanB( outx )				{;}
//<S081> #endif


#endif	//_SEQUENCE_CTRL_H
/***************************************** end of file **********************************************/
