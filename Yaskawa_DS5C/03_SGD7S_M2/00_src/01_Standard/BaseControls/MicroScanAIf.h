/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MicroScanAIf.c : μプログラムScanA I/F処理定義												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : μプログラムのScanAとCPU間のI/F処理														*/
/*																									*/
/*		μプログラム－CPU間のデータ授受を行う。														*/
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
#ifndef MICRO_SCANA_IF_H
#define MICRO_SCANA_IF_H


typedef struct MICRO_SA_IPRMS {
	BOOL	RvsDir;				/* 逆回転モード 													*/
	BOOL	CarrChange;			/* キャリア周波数切り替えフラグ [0:OFF,1:ON]						*/
	USHORT	flg_wf;				/* PnF1E : フラグ													*/
	DBYTEX	spgfail;			/* PnE1E : 下位:S-PG受信失敗許容回数, 上位:FS-PG受信失敗許容回数	*/
	LONG	SpdMaNumBit;		/* モータ速度検出移動平均フィルタ回数(2の累乗)の初期値				*/
	LONG	KmotspdA;			/* ScanA Motor Speed Cal. Gain										*/
	LONG	SvCycleNs;			/* サーボ制御周期 [ns]												*/
} MICRO_SA_IPRMS;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void MicroSA_InitVariables( BASE_CTRL_HNDL *pBaseCtrl, MENCV *MencV, ASICS *SvAsicRegs, MICRO_SA_IPRMS *uIniPrm );
//void MicroSA_ResetVariables( AXIS_HANDLE *Axis );
void MicroSA_InitGainParameters( ASIC_U_IF_STR *pAsicMicroReg );

void MicroSA_InputCycDataB( ASICS *SvAsicRegs, MENCV *MencV, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm );
void MicroSA_OutputCycDataB( ASICS *SvAsicRegs, MENCV *MencV, BASE_CTRL_HNDL *pBaseCtrl, LONG CmdCtrlBit );
void MicroSA_InputCycDataExB( ASICS *SvAsicRegs, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm );
void MicroSA_OutputCycDataExB( ASICS *SvAsicRegs, BASE_CTRL_HNDL *pBaseCtrl);

void MicroSA_InputCycDataA(ASICS *SvAsicRegs, MENCV *MencV, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm);
void MicroSA_OutputCycDataA(ASICS *SvAsicRegs, BASE_CTRL_HNDL *pBaseCtrl);

void MicroIfInputCycData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs );
void MicroIfOutputCycData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs );

#endif /* MICRO_SCANA_IF_H */
/***************************************** end of file **********************************************/
