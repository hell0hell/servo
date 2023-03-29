/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ComRegTbl.c : Option initial register load table  											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Read initial data from servo module	 													*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"
#include "Parameter.h"
#include "Information.h"
#include "Monitor.h"

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define OBJ_READ( obj, idx, pdata, hmsg ) 															\
		((obj)->Read( (obj), (idx), (pdata), MCHK_REG_READ, hmsg ))	/* <S073> */
#define OBJ_SEARCH( obj, num, pidx, hmsg ) 															\
		((obj)->Search( (COBJECT *)(obj), (num), 0, pidx, MCHK_REG_READ, hmsg ))

/*--------------------------------------------------------------------------------------------------*/
/* アンプ情報 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define AMP_LINEAR_BIT		(10)	/* リニア */
#define AMP_FULC_BIT		(8)		/* フルクローズド */

/*--------------------------------------------------------------------------------------------------*/
/* CN1信号情報 	 <S162>  																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CN1_40				0
#define	CN1_41				1
#define	CN1_42				2
#define	CN1_43				3
#define	CN1_44				4
#define	CN1_45				5
#define	CN1_46				6
#define	CN1_XX				8

/****************************************************************************************************/
/*																									*/
/* 		Type definition																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Initial Register Load Table 																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INIT_LOAD_REGS 
{
/*--------------------------------------------------------------------------------------------------*/
	USHORT  Number;													/* Register Number */
	COBJECT *pObj;													/* Object Pointer */
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*LoadFunc)(											/* Load Function */
					struct _INIT_LOAD_REGS *pLoadReg, 				/* Pointer of this table */
					void *hdl,	 									/* Handle of Motion */
					NMSG *Hmsg );									/* Message Interface */
} INIT_LOAD_REGS;

/****************************************************************************************************/
/*																									*/
/* 		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Public Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG	IxiLoadInitPrm( MTHNDL *mthndl, NMSG *Nmsg );

/*--------------------------------------------------------------------------------------------------*/
/*  regLoadTbl[]																					*/
/*--------------------------------------------------------------------------------------------------*/
LONG regSetAmpInfo 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetABSEncMode 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetPGPls 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetOTStopMode 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetTrefAndABSMode 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetFullcScaleType 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetFullcPls 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetEGearB 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetEGearA 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetMaxTrq 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetOverSpd 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetOverSpdExp 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetMaxSpd 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetMaxSpdExp 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetScalePitch 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetScalePitchExp 
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetCtrlMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkPerrClrMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkVffMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkInSig50A
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkInSig50B
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkInSig50C
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkInSig50D
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regChkInSig515
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
/* <S1F5> Start */
LONG regChkInSig516
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
/* <S1F5> End */
LONG regSetOutSig512
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetOutSig50E
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetOutSig50F
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
LONG regSetOutSig510
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
/* <S17E> Start */
LONG regSetOutSig513
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg);
/* <S17E> End */
/*--------------------------------------------------------------------------------------------------*/
/*	Private Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
static LONG  LpxComRegRead( COBJECT *pObj, LONG RegNumber, ULONG *pdata, MSGIF *Hmsg );
static void LpxSetInputSignalLgc( LONG prm, LONG bno, LONG mode, USHORT *dst );
//static void LpxSetOutputSignalLgc( LONG prm, LONG mode, USHORT *dst, ULONG *XorMsk ); /* <S04B> */
static void LpxSetOutputSignalLgc( LONG prm, LONG mode, USHORT *dst, ULONG *XorMsk );


/****************************************************************************************************/
/*																									*/
/* 初期レジスタ情報ロードテーブル(テーブルの先頭からロード)											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*<V652> 初期レジスタ情報をロードする順番に依存する処理があるので、									*/
/*		 テーブルの登録順番を変更しないこと															*/
/*--------------------------------------------------------------------------------------------------*/
/* ※このテーブル内ではMtHdl,NoutV,NinV以外の変数は使用禁止。(その他変数は直後の処理で初期化する)	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
const INIT_LOAD_REGS RegLoadTblIx[] = 
{
/*--------------------------------------------------------------------------------------------------*/
/*	Reg. Number |	Object Pointer |	Load Function   	                                        */
/*--------------------------------------------------------------------------------------------------*/
	0xE703, 		(COBJECT *)CUNREGX, 	regSetAmpInfo,			/* 0:Servo Internal Information	*/
	0xC00D, 		(COBJECT *)CINFREG, 	regSetABSEncMode,		/* 1:エンコーダID/分解能 		*/
	0xC028, 		(COBJECT *)CINFREG, 	regSetFullcScaleType,	/* 2:フルクPG Type 				*/
	0x0002, 		(COBJECT *)CPARAMX, 	regSetTrefAndABSMode,	/* 3:トルク指令モード,ABSモード	*/
	0xC022, 		(COBJECT *)CINFREG, 	regSetPGPls,			/* 4:エンコーダパルス数 		*/
	0x020A, 		(COBJECT *)CPARAMX, 	regSetFullcPls,			/* 5:フルクPG[pitch/rev] 		*/
	0x0001, 		(COBJECT *)CPARAMX, 	regSetOTStopMode,		/* 6:OT停止モード(1桁目) 		*/
	0x020E, 		(COBJECT *)CPARAMX, 	regSetEGearB,			/* 7:電子ギアB 					*/
/*--------------------------------------------------------------------------------------------------*/
	0x0210, 		(COBJECT *)CPARAMX, 	regSetEGearA,			/* 8:電子ギアA 					*/
	0xE701, 		(COBJECT *)CUNREGX, 	regSetMaxTrq,			/* 9:最大トルク[%] 				*/
	0xE702, 		(COBJECT *)CUNREGX, 	regSetOverSpd,			/* 10:OS速度[rpm, mm/s] 		*/
	0xE704, 		(COBJECT *)CUNREGX, 	regSetOverSpdExp,		/* 11:OS速度指数 				*/
	0xC027, 		(COBJECT *)CINFREG, 	regSetMaxSpd,			/* 12:MAX速度[rpm, mm/s] 		*/
	0xC01E, 		(COBJECT *)CINFREG, 	regSetMaxSpdExp,		/* 13:MAX速度指数 				*/
/*--------------------------------------------------------------------------------------------------*/
	0x0000,			(COBJECT *)CPARAMX,		regSetCtrlMode,			/* 14:Control Mode				*/
//未対応	0xE084, 		(COBJECT *)CUNREGX, 	regSetScalePitch,		/* 14:リニアスケールピッチ[pm] 	*/
//未対応	0xE086, 		(COBJECT *)CUNREGX, 	regSetScalePitchExp,	/* 15:リニアスケールピッチ指数 	*/
/*--------------------------------------------------------------------------------------------------*/
	0x0200, 		(COBJECT *)CPARAMX, 	regChkPerrClrMode,		/* 16:偏差クリアモード, etc 	*/
	0x0207, 		(COBJECT *)CPARAMX, 	regChkVffMode,			/* 17:位置制御機能SW			*/
	0x050A, 		(COBJECT *)CPARAMX, 	regChkInSig50A,			/* 18:入力信号割付1 			*/
	0x050B, 		(COBJECT *)CPARAMX, 	regChkInSig50B,			/* 18:入力信号割付2 			*/
	0x050C, 		(COBJECT *)CPARAMX, 	regChkInSig50C,			/* 19:入力信号割付3 			*/
	0x050D, 		(COBJECT *)CPARAMX, 	regChkInSig50D,			/* 20:入力信号割付4 			*/
	0x0515, 		(COBJECT *)CPARAMX, 	regChkInSig515,			/* 20:入力信号割付6 			*/
	0x0516,			(COBJECT *)CPARAMX, 	regChkInSig516,			/* 20:入力信号割付7 			*/  /* <S1F5> */
	0x0512, 		(COBJECT *)CPARAMX, 	regSetOutSig512,		/* 21:出力信号反転設定 			*/
	0x0513, 		(COBJECT *)CPARAMX, 	regSetOutSig513,		/* 22:出力信号反転設定 2		*/	/* <S17E> *//* <S181> */
	0x050E, 		(COBJECT *)CPARAMX, 	regSetOutSig50E,		/* 23:出力信号割付1 			*/
	0x050F, 		(COBJECT *)CPARAMX, 	regSetOutSig50F,		/* 24:出力信号割付2 			*/
	0x0510, 		(COBJECT *)CPARAMX, 	regSetOutSig510,		/* 25:出力信号割付3 			*/
/*--------------------------------------------------------------------------------------------------*/
	0xFFFF, 		NULL, 				NULL					/* end of table */
};

/****************************************************************************************************/
/*																									*/
/*  サーボ初期パラメータの一括ロード用 データ設定処理												*/
/*																									*/
/****************************************************************************************************/
LONG	IxiLoadInitPrm( MTHNDL *mthndl, NMSG *Nmsg )
{
LONG ret = OK;
const INIT_LOAD_REGS *pLoadRegs;
/*--------------------------------------------------------------------------------------------------*/
/* 通信初期パラメータ設定(要確認) 																	*/
/*--------------------------------------------------------------------------------------------------*/
	mthndl->iniPrm.mt.Cycle = CFG_TIO_BASE_TIME * 1;  
	mthndl->iniPrm.mt.SegNum = 1;
	mthndl->iniPrm.mt.SvCycle = 1;							/* SCAN_N_DEFCYCLE;			*/
	mthndl->iniPrm.mt.dlyTime = 1;				          	/* svif->OptSetting.CmdDelay */
	mthndl->iniPrm.mt.SpdDiv = 10; 							/* OpPrm.spdDivision */
	mthndl->iniPrm.mt.TspdGain = 1; 						/* OpPrm.TspdGain */
/*--------------------------------------------------------------------------------------------------*/
/*	Load Registers																					*/
/*--------------------------------------------------------------------------------------------------*/
	for( pLoadRegs = &RegLoadTblIx[0] ; pLoadRegs->Number != 0xFFFF ; pLoadRegs++ )
	{
		ret = pLoadRegs->LoadFunc(										/* Load Register */
					(struct _INIT_LOAD_REGS *)pLoadRegs, 
					mthndl, 
					Nmsg); 
		if( ret != OK ) { break; }										/* Break */
	}
/*--------------------------------------------------------------------------------------------------*/
	return (ret);
}


/****************************************************************************************************/
/*																									*/
/*		サーボ動作モード設定 																		*/
/*																									*/
/****************************************************************************************************/
LONG regSetAmpInfo (struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
/*--------------------------------------------------------------------------------------------------*/
/*		TODO: レジスタE703は未対応。暫定で、回転、セミク型とする。									*/
//	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
	data = 0;	/* 暫定 */
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.Linear =    (BOOL)(data >> AMP_LINEAR_BIT) & 0x1;
	mthdl->iniPrm.mt.FullcCtrl = (BOOL)(data >> AMP_FULC_BIT) & 0x1;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


/****************************************************************************************************/
/*																									*/
/*		エンコーダABSモード設定 																	*/
/*																									*/
/****************************************************************************************************/
LONG regSetABSEncMode (struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.absPG = (BOOL)(data >> 8) & 0x1;
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*		エンコーダPGパルス数設定 																	*/
/*																									*/
/****************************************************************************************************/
LONG regSetPGPls (struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.PgPuls = data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


/****************************************************************************************************/
/*																									*/
/*	OT停止方法設定 																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetOTStopMode (struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.ot_zstop = (((data>>4) & 0xF) == 1) ? TRUE: FALSE;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	フルクPG ABSモード設定 																			*/
/*	※設定順序に注意!! 																				*/
/*																									*/
/****************************************************************************************************/
LONG regSetFullcScaleType (struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	if(mthdl->iniPrm.mt.FullcCtrl) {
		mthdl->iniPrm.mt.absPG = (BOOL)(data & 0x1);						/* ABSモードセット */
	}
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	Trefモード、ABSモード設定 																		*/
/*																									*/
/****************************************************************************************************/
LONG regSetTrefAndABSMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.trefMode = (USHORT)(data&0xF);							/* TRefモードセット */
	if((0x100 == (data&0xF00)) && (mthdl->iniPrm.mt.FullcCtrl == FALSE))	/* <S16A> */
	{
		mthdl->iniPrm.mt.absPG = FALSE;		/* ABSモードセット */
	}
/*--------------------------------------------------------------------------------------------------*/
	NinV.P.AbsUse = (mthdl->iniPrm.mt.absPG) ? TRUE : FALSE;				/* set Absolute mode */
/*--------------------------------------------------------------------------------------------------*/
//	return ( (data & 0x0F) != 0x01 ) ? OK : NG; 							/* <S00C> */
	return  (OK);															/* <S00C> */
}

/****************************************************************************************************/
/*																									*/
/*	フルクPGパルス数設定 																			*/
/*																									*/
/****************************************************************************************************/
LONG regSetFullcPls
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	if ( mthdl->iniPrm.mt.FullcCtrl )
	{
		mthdl->iniPrm.mt.PgPuls = mthdl->iniPrm.mt.PgPuls*data;  			/* [pitch/rev]*[pls/pitch] */
	}
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	電子ギアB設定 																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetEGearB
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.EGearB = data;
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	電子ギアA設定 																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetEGearA
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.EGearA = data;
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	最大トルク設定 																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetMaxTrq
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.MaxTrq = data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


/****************************************************************************************************/
/*																									*/
/*	OSレベル設定 																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetOverSpd
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.OverSpd = data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	OSレベル指数設定 																				*/
/*																									*/
/****************************************************************************************************/
LONG regSetOverSpdExp
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.OverSpdExp = (SHORT)data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	最高速度設定 																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetMaxSpd
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.MaxSpd = data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	最高速度指数設定 																				*/
/*																									*/
/****************************************************************************************************/
LONG regSetMaxSpdExp
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.MaxSpdExp = (SHORT)data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	スケールピッチ数設定 																			*/
/*																									*/
/****************************************************************************************************/
LONG regSetScalePitch
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.ScalePitch = data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	スケールピッチ数指数設定 																		*/
/*																									*/
/****************************************************************************************************/
LONG regSetScalePitchExp
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MTHNDL *mthdl = (MTHNDL *)hdl;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mthdl->iniPrm.mt.ScalePitchExp = (SHORT)data;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	Pn000: Control Mode			 																	*/
/*																									*/
/****************************************************************************************************/
LONG regSetCtrlMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MSGIF *Hmsg = Nmsg->Super;
NINV *pninv = &NinV;							/* <S162>  */
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	pninv->P.CtrlMode = (data>>4) & 0x0F;		/* <S162>  */
/*--------------------------------------------------------------------------------------------------*/
/*	Set Analog I/F Control Mode 																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (data>>4) & 0x0F )
	{
	case 0:											/* Velocity Control */
		NoutV.CtrlModeA = ANLG_REQ_VELCTRL;
		NoutV.CtrlModeB = ANLG_REQ_VELCTRL;			/* <S03D> */
		break;
	case 1:											/* Position Control */
		NoutV.CtrlModeA = ANLG_REQ_INTERPOL;
		NoutV.CtrlModeB = ANLG_REQ_INTERPOL;		/* <S03D> */
		break;
	case 2:											/* Torque Control */
		NoutV.CtrlModeA = ANLG_REQ_TRQCTRL;
		NoutV.CtrlModeB = ANLG_REQ_TRQCTRL;			/* <S03D> */
		break;
/* <S03D> Start */
	case 3:											/* TODO: Internal Velocity0 */
		NoutV.CtrlModeA = ANLG_REQ_INTEVELCTRL;
		NoutV.CtrlModeB = ANLG_REQ_INTEVELCTRL;
		break;
	case 4:											/* TODO: Internal Velocity1 */
		NoutV.CtrlModeA = ANLG_REQ_INTEVELCTRL;
		NoutV.CtrlModeB = ANLG_REQ_VELCTRL;
		break;
	case 5:											/* TODO: Internal Velocity2 */
		NoutV.CtrlModeA = ANLG_REQ_INTEVELCTRL;
		NoutV.CtrlModeB = ANLG_REQ_INTERPOL;
		break;
	case 6:											/* TODO: Internal Velocity3 */
		NoutV.CtrlModeA = ANLG_REQ_INTEVELCTRL;
		NoutV.CtrlModeB = ANLG_REQ_TRQCTRL;
		break;
	case 7:											/* TODO: Position <-> Velocity */
		NoutV.CtrlModeA = ANLG_REQ_INTERPOL;
		NoutV.CtrlModeB = ANLG_REQ_VELCTRL;
		break;
	case 8:											/* TODO: Position <-> Torque */
		NoutV.CtrlModeA = ANLG_REQ_INTERPOL;
		NoutV.CtrlModeB = ANLG_REQ_TRQCTRL;
		break;
	case 9:											/* TODO: Torque <-> Velocity */
		NoutV.CtrlModeA = ANLG_REQ_TRQCTRL;
		NoutV.CtrlModeB = ANLG_REQ_VELCTRL;
		break;
	case 0xA:										/* TODO: Velocity <-> Velocity with Zero clamp  */
		NoutV.CtrlModeA = ANLG_REQ_VELCTRL;
		NoutV.CtrlModeB = ANLG_REQ_VELCTRL;
		break;
	case 0xB:										/* TODO: Position <-> Position with Inhibit */
		NoutV.CtrlModeA = ANLG_REQ_INTERPOL;
		NoutV.CtrlModeB = ANLG_REQ_INTERPOL;
		break;
/* <S03D> End */

	default:
		NoutV.CtrlModeA = ANLG_REQ_NOP; 			/* Not Supported */
		NoutV.CtrlModeB = ANLG_REQ_NOP;				/* <S03D> */
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*	出力信号割付設定チェック 																		*/
/*																									*/
/****************************************************************************************************/
INT32 regSetOutSignEnable(REGHNDL* hdl, UCHAR idx, LONG data)
{
	if(data != 0)	/* 割付有の場合はDISABLE */
	{	hdl->iniPrm.mt.OutSigEnable = FALSE;}
	return REG_SUCCESS;
}
#endif

/****************************************************************************************************/
/*																									*/
/*	Position Control Option (Pn200) 																*/
/*																									*/
/****************************************************************************************************/
LONG regChkPerrClrMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MSGIF *Hmsg = Nmsg->Super;
NINV *ninv = &NinV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.1 : クリア信号形態選択スイッチ														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (data >> 4)& 0x000F )
	{
		case 0x00:	ninv->P.PerClrSigSel = 0x00; break;	/* 信号のＨｉレベルで偏差クリア				*/
		case 0x01:  ninv->P.PerClrSigSel = 0x01; break;	/* 信号の立ち上がりで偏差クリア				*/
		case 0x02:  ninv->P.PerClrSigSel = 0x02; break;	/* 信号のＬｏレベルで偏差クリア				*/
		case 0x03:  ninv->P.PerClrSigSel = 0x03; break;	/* 信号の立ち下がりで偏差クリア				*/
		default :	return(NG); 
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Pn200.2 : クリア動作選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (data >> 8)& 0x000F )
	{
		case 0x00:	ninv->P.PerClrMode = 0x00; break;	/* ベースブロック時＋CLR信号で、偏差クリア	*/
		case 0x01:	ninv->P.PerClrMode = 0x01; break;	/* CLR信号でのみ偏差クリア					*/
		case 0x02:	ninv->P.PerClrMode = 0x02; break;	/* アラーム発生時＋CLR信号で、偏差クリア	*/
		case 0x03:	ninv->P.PerClrMode = 0x03; break;	/* 偏差クリアしない(CLR信号無視)			*/
		default :	return (NG); 
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差レベルクリア選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ninv->P.PerClrMode == 0x03        ){ ninv->P.PerClrLvlSel = 0x00;}	/* No   Level	*/
	else if( ninv->P.PerClrSigSel == 0x00 ){ ninv->P.PerClrLvlSel = 0x01;}	/* High Level	*/
	else if( ninv->P.PerClrSigSel == 0x02 ){ ninv->P.PerClrLvlSel = 0x02;}	/* Low  Level	*/
	else                                	{ ninv->P.PerClrLvlSel = 0x00;}	/* No   Level	*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差エッジクリア選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S112> いらなかった。 */
	if( ((ninv->P.PerClrSigSel == 0x01)||(ninv->P.PerClrSigSel == 0x03)) )
	{
		/*
		 * ※偏差エッジクリアの場合、通信側からの偏差クリア指令は無効。
		 */
		ninv->P.PerClrMode = 0x03;
	}
#endif
	return (OK);
}

/****************************************************************************************************/
/*																									*/
/*	位置制御機能SW(Pn207)																			*/
/*																									*/
/****************************************************************************************************/
LONG regChkVffMode
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
MSGIF *Hmsg = Nmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
/*	Position Reference Option																		*/
/*--------------------------------------------------------------------------------------------------*/
//<S00C>	switch( (data >> 4)& 0x000F )
//	{
//		case 0: break;	/* TODO: not assigned */
//		case 1: break;	/* TODO: use v-ref as vff */
//		default:
//			break;
//	}
/*--------------------------------------------------------------------------------------------------*/
/*	/COIN Output Timing <S00C>																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (data >> 12)& 0x000F )
	{
		case 0: 							/* output /COIN when perr is less than Pn522 */
			NoutV.P.CoinOut = INPOUTSEL_PWIDTH; break;	
		case 1: 							/* idem and position filter remain is zero */
			NoutV.P.CoinOut = INPOUTSEL_PWIDTH_FILTER; break;	
		case 2: 							/* output /COIN when perr is less than Pn522 and den = 1 */
			NoutV.P.CoinOut = INPOUTSEL_PWIDTH_DEN; break;	
		default:
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	入力信号割付設定チェック1																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn50A.0: 入力信号割付モード																		*/
/*	Pn50A.1: /S-ON信号割付																			*/
/*	Pn50A.2: /P-CON信号割付																			*/
/*	Pn50A.3: /P-OT信号割付																			*/
/*																									*/
/****************************************************************************************************/
LONG regChkInSig50A
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
#define 	_OT_MODE( data, mode ) ((( ((data)&0x0F) < 7 ) || ( ((data) & 0x0F) > 8 ) ) ? (mode) : SQI_ASSIGN_PRM )	/* <S055> */
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NINV *pninv = &NinV; /* @@ */
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
/* set signal assign mode 																			*/
/*--------------------------------------------------------------------------------------------------*/
	mode = ((data & 0x0F) ? SQI_ASSIGN_PRM : SQI_ASSIGN_DEFAULT); 
	pninv->P.AssignMode = mode;
/*--------------------------------------------------------------------------------------------------*/
/*  set /S-ON, /P-CON, P-OT assign 																	*/
/*--------------------------------------------------------------------------------------------------*/
	//LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_SON, _OT_MODE(data>>4, mode), &(pPrm->SONLgc) );		/* <S055> */
	LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_SON, _OT_MODE(data>>4, mode), &(pPrm->SONLgc) );		/* <S214> */
	//LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_PCON, mode, &(pPrm->PCONLgc) );
	if(( pninv->P.CtrlMode > 2 )&&( mode == SQI_ASSIGN_DEFAULT ))		/* <S162>  */
	{
		LpxSetInputSignalLgc( CN1_XX, SQI_BNO_PCON, SQI_ASSIGN_PRM, &(pPrm->PCONLgc) );	/* PCON無効		*/
	}
	else
	{
		LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_PCON, mode, &(pPrm->PCONLgc) );
	}
	LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_POT, _OT_MODE(data>>12, mode), &(pPrm->POTLgc) );	/* <S055> */
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


/****************************************************************************************************/
/*																									*/
/*	入力信号割付設定チェック2																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn50B.0: N-OT信号割付																			*/
/*	Pn50B.1: /ALM-RST信号割付																		*/
/*	Pn50B.2: /P-CL信号割付																			*/
/*	Pn50B.3: /N-CL信号割付																			*/
/*																									*/
/****************************************************************************************************/
LONG regChkInSig50B
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NINV *pninv = &NinV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pninv->P.AssignMode;
/*--------------------------------------------------------------------------------------------------*/
/* set N-OT, ALM-RST, P-CL, N-CL assign 															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetInputSignalLgc( (data)&0x0F, SQI_BNO_NOT, _OT_MODE(data, mode), &(pPrm->NOTLgc) );	/* <S055> */
	LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_ALMRST, mode, &(pPrm->ALMRSTLgc) );
	//LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_PCL, mode, &(pPrm->PCLLgc) );
	////LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_NCL, mode, &(pPrm->NCLLgc) );	/* <S03D> */
	//LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_NCL, mode, &(pPrm->NCLLgc) );	/* <S03D> シフト演算が間違っていたので修正 */
	if(( pninv->P.CtrlMode > 2 )&&( pninv->P.CtrlMode < 7 )&&( mode == SQI_ASSIGN_DEFAULT ))		/* <S162>  */
	{
		LpxSetInputSignalLgc( CN1_XX, SQI_BNO_PCL, SQI_ASSIGN_PRM, &(pPrm->PCLLgc) );	/* PCL無効	*/
		LpxSetInputSignalLgc( CN1_XX, SQI_BNO_NCL, SQI_ASSIGN_PRM, &(pPrm->NCLLgc) );	/* NCL無効	*/
	}
	else
	{
		LpxSetInputSignalLgc( (data>>8)&0x0F,  SQI_BNO_PCL, mode, &(pPrm->PCLLgc) );
		LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_NCL, mode, &(pPrm->NCLLgc) );
	}
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


/****************************************************************************************************/
/*																									*/
/*	入力信号割付設定チェック3																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn50C.0: /SPD-D信号割付																			*/
/*	Pn50C.1: /SPD-A信号割付																			*/
/*	Pn50C.2: /SPD-B信号割付																			*/
/*	Pn50C.3: /C-SEL信号割付																			*/
/*																									*/
/****************************************************************************************************/
LONG regChkInSig50C
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
/* <S03D> Start */
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NINV *pninv = &NinV;
/* <S03D> End */

/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/

/* <S03D> Start */
	mode = pninv->P.AssignMode;
/*--------------------------------------------------------------------------------------------------*/
/* set SPD-D, SPD-A, SPD-B, C-SEL assign 																*/
/*--------------------------------------------------------------------------------------------------*/
//	LpxSetInputSignalLgc( (data)&0x0F, SQI_BNO_SPDD, mode, &(pPrm->SPDDLgc) );
//	LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_SPDA, mode, &(pPrm->SPDALgc) );
//	LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_SPDB, mode, &(pPrm->SPDBLgc) );
//	LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_CSEL, mode, &(pPrm->CSELLgc) );
	if(( pninv->P.CtrlMode > 2 )&&( pninv->P.CtrlMode < 7 )&&( mode == SQI_ASSIGN_DEFAULT ))		/* <S162>  */
	{
		LpxSetInputSignalLgc( CN1_41, SQI_BNO_SPDD, SQI_ASSIGN_PRM, &(pPrm->SPDDLgc) );	/* PCON */
		LpxSetInputSignalLgc( CN1_45, SQI_BNO_SPDA, SQI_ASSIGN_PRM, &(pPrm->SPDALgc) );	/* PCL  */
		LpxSetInputSignalLgc( CN1_46, SQI_BNO_SPDB, SQI_ASSIGN_PRM, &(pPrm->SPDBLgc) );	/* NCL  */
	}
	else
	{
		LpxSetInputSignalLgc( (data)&0x0F,    SQI_BNO_SPDD, mode, &(pPrm->SPDDLgc) );
		LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_SPDA, mode, &(pPrm->SPDALgc) );
		LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_SPDB, mode, &(pPrm->SPDBLgc) );
	}
	if(( pninv->P.CtrlMode > 6 )&&( pninv->P.CtrlMode < 0x0A )&&( mode == SQI_ASSIGN_DEFAULT ))		/* <S162>  */
	{
		LpxSetInputSignalLgc( CN1_41, SQI_BNO_CSEL, SQI_ASSIGN_PRM, &(pPrm->CSELLgc) );	/* PCON */
	}
	else
	{
		LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_CSEL, mode, &(pPrm->CSELLgc) );
	}
/* <S03D> End */

/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	入力信号割付設定チェック4																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn50D.0: /ZCLAMP信号割付																		*/
/*	Pn50D.1: /INHIBIT信号割付																		*/
/*	Pn50D.2: /G-SEL信号割付																			*/
/*	Pn50D.3: /P-DET信号割付(SGD7Sのみ) <S21C>														*/
/*																									*/
/****************************************************************************************************/
LONG regChkInSig50D
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NINV *pninv = &NinV;
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
AXIS_HANDLE *Axis  = (AXIS_HANDLE*)Hmsg->AxisMap->Axis;
MENCV       *MencV = Axis->MencV;
#endif								/* <S21C> */
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pninv->P.AssignMode;
/*--------------------------------------------------------------------------------------------------*/
/* set ZCLAMP, INHIBIT, G-SEL assign 																*/
/*--------------------------------------------------------------------------------------------------*/
//	LpxSetInputSignalLgc( (data)&0x0F, SQI_BNO_ZCLAMP, mode, &(pPrm->ZCLAMPLgc) );
//	LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_INHIBIT, mode, &(pPrm->INHIBITLgc) );
	if(( pninv->P.CtrlMode == 0x0A )&&( mode == SQI_ASSIGN_DEFAULT ))						/* <S162>  */
	{
		LpxSetInputSignalLgc( CN1_41, SQI_BNO_ZCLAMP, SQI_ASSIGN_PRM, &(pPrm->ZCLAMPLgc) );		/* PCON */
	}
	else
	{
		LpxSetInputSignalLgc( (data)&0x0F, SQI_BNO_ZCLAMP, mode, &(pPrm->ZCLAMPLgc) );
	}
	if(( pninv->P.CtrlMode == 0x0B )&&( mode == SQI_ASSIGN_DEFAULT ))						/* <S162>  */
	{
		LpxSetInputSignalLgc( CN1_41, SQI_BNO_INHIBIT, SQI_ASSIGN_PRM, &(pPrm->INHIBITLgc) );	/* PCON */
	}
	else
	{
		LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_INHIBIT, mode, &(pPrm->INHIBITLgc) );
	}
	LpxSetInputSignalLgc( (data>>8)&0x0F, SQI_BNO_GSEL, mode, &(pPrm->GSELLgc) );         /* <S09B> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	if( pninv->P.AssignMode == SQI_ASSIGN_PRM )
	{
		LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_PDET, SQI_ASSIGN_PRM, &(pPrm->PdetLgc) );
	}
	else
	{
		LpxSetInputSignalLgc( (data>>12)&0x0F, SQI_BNO_PDET, SQI_ASSIGN_DEFAULT, &(pPrm->PdetLgc) );
	}
#endif								/* <S21C> */
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	入力信号割付設定チェック6																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn515.0: Reserved																				*/
/*	Pn515.1: /PSEL信号割付																			*/
/*	Pn515.2: Reserved																				*/
/*	Pn515.3: Reserved																				*/
/*																									*/
/****************************************************************************************************/
LONG regChkInSig515
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NINV *pninv = &NinV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pninv->P.AssignMode;
/*--------------------------------------------------------------------------------------------------*/
/* set /SEN assign 																					*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetInputSignalLgc( (data)&0x0F, SQI_BNO_SEN, mode, &(pPrm->SENLgc) );
/*--------------------------------------------------------------------------------------------------*/
/* set /PSEL assign 																				*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetInputSignalLgc( (data>>4)&0x0F, SQI_BNO_PSEL, mode, &(pPrm->PSELLgc) );
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/* <S1F5> Start */
/****************************************************************************************************/
/*																									*/
/*	入力信号割付設定チェック7																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn516.0: /Fstp 信号割付																		*/
/*	Pn516.1: Reserved																				*/
/*	Pn516.2: Reserved																				*/
/*	Pn516.3: Reserved																				*/
/*																									*/
/****************************************************************************************************/
LONG regChkInSig516
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NINV *pninv = &NinV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pninv->P.AssignMode;
/*--------------------------------------------------------------------------------------------------*/
/* set /FSTP assign 																					*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetInputSignalLgc( (data)&0x0F, SQI_BNO_FSTP, mode, &(pPrm->FstpLgc) );

	return OK;
}
/* <S1F5> End */


/****************************************************************************************************/
/*																									*/
/*	出力信号反転設定																				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn512.0: SO1信号割付																			*/
/*	Pn512.1: SO2信号割付																			*/
/*	Pn512.2: SO3信号割付																			*/
/*	Pn512.3: SO4信号割付 <S175>																		*/
/*																									*/
/****************************************************************************************************/
LONG regSetOutSig512
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
UINT mode;
MSGIF *Hmsg = Nmsg->Super;
NOUTV *pnoutv = &NoutV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = 0;
	mode |= (( data & 0x0F ) ? (1<<0) : 0 );
	mode |= (( (data>>4) & 0x0F ) ? (1<<1) : 0 );
	mode |= (( (data>>8) & 0x0F ) ? (1<<2) : 0 );
	mode |= (( (data>>12) & 0x0F ) ? (1<<3) : 0 );	/* <S175> */
	pnoutv->P.OppositeMode = mode;
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/* <S17E> Start */
/****************************************************************************************************/
/*																									*/
/*	出力信号反転設定2																				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn513.0: SO5信号割付																			*/
/*	Pn513.1: SO6信号割付																			*/
/*	Pn513.2: 予約																					*/
/*	Pn513.3: 予約																					*/
/*																									*/
/****************************************************************************************************/
LONG regSetOutSig513
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
UINT mode;
MSGIF *Hmsg = Nmsg->Super;
NOUTV *pnoutv = &NoutV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = 0;
	mode |= (( data & 0x0F ) ? (1<<4) : 0 );
	mode |= (( (data>>4) & 0x0F ) ? (1<<5) : 0 );
	pnoutv->P.OppositeMode |= mode;														/* <S188>	*/
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}
/* <S17E> End */

/****************************************************************************************************/
/*																									*/
/*	出力信号割付1 																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn50E.0: /COIN信号割付																			*/
/*	Pn50E.1: /VCMP信号割付																			*/
/*	Pn50E.2: /TGON信号割付																			*/
/*	Pn50E.3: /S-RDY信号割付																			*/
/*																									*/
/****************************************************************************************************/
LONG regSetOutSig50E
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NOUTV *pnoutv = &NoutV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pnoutv->P.OppositeMode;
/*--------------------------------------------------------------------------------------------------*/
/*	Set /COIN, /VCMP, /TGON, /SRDY assign															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetOutputSignalLgc( (data>>0)&0x0F, mode, &(pPrm->bINPOSLgc), &pnoutv->XorMask );			  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>4)&0x0F, mode, &(pPrm->VCMPLgc), &pnoutv->XorMask );				  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>8)&0x0F, mode, &(pPrm->TGONLgc), &pnoutv->XorMask );				  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>12)&0x0F, mode, &(pPrm->bSRDYLgc), &pnoutv->XorMask );			  /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	出力信号割付2 																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn50F.0: /CLT信号割付																			*/
/*	Pn50F.1: /VLT信号割付																			*/
/*	Pn50F.2: /BK信号割付																			*/
/*	Pn50F.3: /WARN信号割付																			*/
/*																									*/
/****************************************************************************************************/
LONG regSetOutSig50F
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NOUTV *pnoutv = &NoutV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pnoutv->P.OppositeMode;
/*--------------------------------------------------------------------------------------------------*/
/*	Set /CLT, /VLT, /BK, /WARN assign															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetOutputSignalLgc( (data>>0)&0x0F, mode, &(pPrm->CLTLgc), &pnoutv->XorMask );				  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>4)&0x0F, mode, &(pPrm->VLTLgc), &pnoutv->XorMask );				  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>8)&0x0F, mode, &(pPrm->bBKLgc), &pnoutv->XorMask );				  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>12)&0x0F, mode, &(pPrm->bWRNLgc), &pnoutv->XorMask );				  /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}

/****************************************************************************************************/
/*																									*/
/*	出力信号割付3 																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	Pn510.0: /NEAR信号割付																			*/
/*	Pn510.1: Reserved																				*/
/*	Pn510.2: /PSELA信号割付																			*/
/*	Pn510.3: Reserved																				*/
/*																									*/
/****************************************************************************************************/
LONG regSetOutSig510
(struct _INIT_LOAD_REGS *pLoadReg, void *hdl, NMSG *Nmsg)
{
ULONG data;
LONG mode;
MSGIF *Hmsg = Nmsg->Super;
OPPRMDAT *pPrm = Nmsg->pPrm;
NOUTV *pnoutv = &NoutV;
/*--------------------------------------------------------------------------------------------------*/
	if( LpxComRegRead( pLoadReg->pObj, pLoadReg->Number, &data, Hmsg ) != OK ) { return (NG); }
/*--------------------------------------------------------------------------------------------------*/
	mode = pnoutv->P.OppositeMode;
/*--------------------------------------------------------------------------------------------------*/
/*	Set /NEAR, /PSELA, assign																		*/
/*--------------------------------------------------------------------------------------------------*/
	LpxSetOutputSignalLgc( (data>>0)&0x0F, mode, &(pPrm->NEARLgc), &pnoutv->XorMask );				  /* <S04B> */
	LpxSetOutputSignalLgc( (data>>8)&0x0F, mode, &(pPrm->PSELALgc), &pnoutv->XorMask );				  /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
	return OK;
}


/****************************************************************************************************/
/*																									*/
/*		Read One Register	 																		*/
/*																									*/
/****************************************************************************************************/
LONG  LpxComRegRead( COBJECT *pObj, LONG RegNumber, ULONG *pdata, MSGIF *Hmsg )
{
LONG objidx;
/*--------------------------------------------------------------------------------------------------*/
	if( OK != OBJ_SEARCH( pObj, (RegNumber&0x0FFF), &objidx, Hmsg ) )
	{ 
		return (NG);
	}
/*--------------------------------------------------------------------------------------------------*/
	OBJ_READ( pObj, objidx, pdata, Hmsg );
	return (OK);
}

/****************************************************************************************************/
/*																									*/
/*	入力信号割付セット																				*/
/*																									*/
/****************************************************************************************************/
void LpxSetInputSignalLgc( LONG prm, LONG bno, LONG mode, USHORT *dst )
{
#define LPX_GETINSIGMAP( prm ) (( prm < 9 ) ? (prm) : (prm-9))
#define LPX_GETINSIGLGC( prm ) (( prm >= 9 ) ? SQI_LGC_OPPOSITE /* <S168> */\
								: (( prm == 7 ) ? SQI_LGC_ACTIVE \
								: (( prm == 8 ) ? SQI_LGC_NONACTIVE \
								: SQI_LGC_DEFAULT)))
UINT map;
UINT lgc;
UINT set;
/*--------------------------------------------------------------------------------------------------*/
	map = LPX_GETINSIGMAP( prm ); 
	lgc = LPX_GETINSIGLGC( prm );
	set = ((mode == SQI_ASSIGN_DEFAULT) ? CSqiAssignTbl[(bno)] : (((map)<<4) | lgc));
	*dst = (USHORT)set;
/*--------------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*	出力信号割付セット																				*/
/*																									*/
/****************************************************************************************************/
//void LpxSetOutputSignalLgc( LONG prm, LONG mode, USHORT *dst ) /* <S04B> */
void LpxSetOutputSignalLgc( LONG prm, LONG mode, USHORT *dst, ULONG *XorMsk ) /* <S04B> */
{
#define LPX_GETOUTSIGMAP( prm ) ( ( (prm) == 0 ) ? 0 : ((prm)-1) )
#define LPX_GETOUTSIGLGC( prm, map, mode ) (( prm == 0 ) ? SQO_LGC_NONACTIVE \
								: (((1<<(map)) & (mode)) ? SQO_LGC_OPPOSITE \
								: SQO_LGC_DEFAULT))

UINT map;
UINT lgc;
/*--------------------------------------------------------------------------------------------------*/
	map = LPX_GETOUTSIGMAP( prm ); 
	lgc = LPX_GETOUTSIGLGC( prm, map, mode );
	*XorMsk |= ( lgc == SQO_LGC_NONACTIVE ) ? 0 : (1<<map); /* <S04B> */
	*dst = (USHORT)((map<<4) | lgc);
/*--------------------------------------------------------------------------------------------------*/
	return;
}
