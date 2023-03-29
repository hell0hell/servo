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
#include "Basedef.h"
#include "BaseControls.h"
#include "MicroScanAIf.h"
#include "BaseControlPrmCalc.h"
#include "JL086ServoIpHw.h"


/* 外部関数 */
extern void ApiSetCurDatasMotorLessTestMode( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut );


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************************************/
/*																									*/
/*		マイクロプログラムＩＦ変数の初期化															*/
/*																									*/
/****************************************************************************************************/
//void	KpxInitMicroIFVariables( AXIS_HANDLE *Axis )
void MicroSA_InitVariables( BASE_CTRL_HNDL *pBaseCtrl,
    	                    MENCV *MencV, ASICS *SvAsicRegs, MICRO_SA_IPRMS *uIniPrm )
{
	USHORT			uswk;
	ASIC_U_IF_STR	*pAsicMicroReg;
	pAsicMicroReg	= SvAsicRegs->AsicMicroReg;

/*--------------------------------------------------------------------------------------------------*/
/*		マイクロＩＦレジスタへの書き込み															*/
/*--------------------------------------------------------------------------------------------------*/
	/* bit情報設定 */
	uswk = SvAsicRegs->MicroBitIprm.data;
	if( MencV->Sigma3Encoder == TRUE ){
		uswk |= BITIPRM_SGM3ENC;}
	if( pBaseCtrl->CtrlCmdPrm.IPSpdControl == TRUE ){
		uswk |= BITIPRM_IPSPDCONTROL;}
	if( pBaseCtrl->CtrlCmdPrm.SpdIctrlKeep == TRUE ){
		uswk |= BITIPRM_SPDICTRLKEEP;}
	if( MencV->DivOut == TRUE ){
		uswk |= BITIPRM_DIVOUT;}
	if( uIniPrm->RvsDir == TRUE ){
		uswk |= BITIPRM_RVSDIR;}
	if( MencV->AxisMotType == MOTTYPE_LINEAR ){
		uswk |= BITIPRM_SERIALCONV;}
	if( MencV->withPoleSensor == TRUE ){
		uswk |= BITIPRM_POLESENSOR;}
	if( MencV->PhaseOrderUWV == TRUE ){
		uswk |= BITIPRM_PHSUWV;}
	if( pBaseCtrl->TuneLessCtrl.conf.TuningLessUse == TRUE ){
		uswk |= BITIPRM_TUNELESSON;}
	if( pBaseCtrl->TuneLessCtrl.conf.TuningLessVComp == TRUE ){
		uswk |= BITIPRM_TLSPDPHACOMP;}
	if( (pBaseCtrl->TuneLessCtrl.conf.TuningLessMode & TLMODE_SPDDETCHNG) != FALSE ){
		uswk |= BITIPRM_TLSPDDETCHNG;}
	if( pBaseCtrl->CtrlCmdPrm.LpassFil3 == TRUE ){
		uswk |= BITIPRM_LPASSFIL3ON;}
	if( (uIniPrm->flg_wf && 0x0011) != FALSE ){
		uswk |= BITIPRM_MOTWEAKENFIELD;}
	if( (uIniPrm->CarrChange) == TRUE ){
		uswk |= BITIPRM_CARRERCHNG;}
	SvAsicRegs->MicroBitIprm.data = uswk;

	/* BITDAT */
	uswk = SvAsicRegs->MicroBitDat.data;
	if( MencV->Cset == TRUE ){
		uswk |= BITDAT_CSET;}
	if( MencV->Cpass == TRUE ){
		uswk |= BITDAT_CPASS;}
	if( MencV->OrgOk == TRUE ){
		uswk |= BITDAT_ORGOK;}
	if( MencV->EncDataNG == TRUE ){
		uswk |= BITDAT_AC90ALM;}
	if( MencV->IncreCposLatch == TRUE ){
		uswk |= BITDAT_CPOSLATCH;}
	if( MencV->DivCpass == TRUE ){
		uswk |= BITDAT_DIVCPASS;}
	if( MencV->AccChkEnable == TRUE ){
		uswk |= BITDAT_ACCCHKENABLE;}
	SvAsicRegs->MicroBitDat.data = uswk;
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_BITIPRM		= (USHORT)( SvAsicRegs->MicroBitIprm.data );
	pAsicMicroReg->MREG_BITDAT		= (USHORT)( SvAsicRegs->MicroBitDat.data );
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		pAsicMicroReg->MREG_MPOSSFTR		= (USHORT)(LMX_MPOS_SFT);
		pAsicMicroReg->MREG_MPOSSFTX		= (USHORT)(LMX_MPOS_SFT);
		pAsicMicroReg->MREG_ABSMOTPOSPG		= (USHORT)(MencV->MotAbsPos);
		pAsicMicroReg->MREG_ABSMOTPOSPGH	= (USHORT)(MencV->MotAbsPos>>16);
		pAsicMicroReg->MREG_MOTPHAX			= (USHORT)(MencV->MotPhaX);
		pAsicMicroReg->MREG_MOTPHAXH		= (USHORT)(MencV->MotPhaX>>16);
		pAsicMicroReg->MREG_PHASEOFST		= (USHORT)(MencV->PhaseOffset);
		pAsicMicroReg->MREG_PHASEOFSTH		= (USHORT)(MencV->PhaseOffset>>16);
		pAsicMicroReg->MREG_MPFPHASEOFST	= (USHORT)(MencV->MpfPhaseOffset);
		pAsicMicroReg->MREG_MPFPHASEOFSTH	= (USHORT)(MencV->MpfPhaseOffset>>16);
		pAsicMicroReg->MREG_MOTPULSENUM		= (USHORT)(MencV->PulseNo);
		pAsicMicroReg->MREG_MOTPULSENUMH	= (USHORT)(MencV->PulseNo>>16);
		pAsicMicroReg->MREG_HALFPULSENUM	= (USHORT)(MencV->HalfPulseNo);
		pAsicMicroReg->MREG_HALFPULSENUMH	= (USHORT)(MencV->HalfPulseNo>>16);
		pAsicMicroReg->MREG_PULSE30DEG		= (USHORT)(MencV->PulseNo30deg);
		pAsicMicroReg->MREG_PULSE30DEGH		= (USHORT)(MencV->PulseNo30deg>>16);
		pAsicMicroReg->MREG_SCALEPITCH		= (USHORT)(MencV->NormScalePitch);
		pAsicMicroReg->MREG_SCALEPITCHH		= (USHORT)(MencV->NormScalePitch>>16);
		pAsicMicroReg->MREG_MOTORGXCHK0		= (USHORT)(MencV->MotOrgXchk[0]);
		pAsicMicroReg->MREG_MOTORGXCHK0H	= (USHORT)(MencV->MotOrgXchk[0]>>16);
		pAsicMicroReg->MREG_MOTORGXCHK1		= (USHORT)(MencV->MotOrgXchk[1]);
		pAsicMicroReg->MREG_MOTORGXCHK1H	= (USHORT)(MencV->MotOrgXchk[1]>>16);
		pAsicMicroReg->MREG_ORGDETDELAY		= (USHORT)(MencV->OrgDetDelayComp);
		pAsicMicroReg->MREG_ORGDETDELAYH	= (USHORT)(MencV->OrgDetDelayComp>>16);
		pAsicMicroReg->MREG_POLEINFO		= (USHORT)(MencV->MotPoleInfo[0]);
		pAsicMicroReg->MREG_LSTPOLEINFO		= (USHORT)(MencV->MotPoleInfo[1]);
		pAsicMicroReg->MREG_DETMOTORGREQ	= (USHORT)(MencV->MotOrgChkAlways);
		pAsicMicroReg->MREG_CPHASELESS		= (USHORT)(MencV->CphaseLess);
		pAsicMicroReg->MREG_MOTORGCHKCNT	= (USHORT)(MencV->MotOrgChkCnt);
		pAsicMicroReg->MREG_MOTORGCHKEND	= (USHORT)(MencV->MotOrgChkEnd);
		pAsicMicroReg->MREG_DIVOUTGAIN		= (USHORT)(MencV->DivOutGain);
		pAsicMicroReg->MREG_DIVOUTGAINH		= (USHORT)(MencV->DivOutGain>>16);
	}
	else
	{ /* 回転型 */
		pAsicMicroReg->MREG_MPOSSFTR		= (USHORT)(MencV->MposSftR);
		pAsicMicroReg->MREG_MPOSSFTX		= (USHORT)(MencV->MposSftX);
		pAsicMicroReg->MREG_INCRECMPSFT		= (USHORT)(MencV->IncreCompSft);
		pAsicMicroReg->MREG_MORGSFT			= (USHORT)(MencV->MorgSft);
		pAsicMicroReg->MREG_MOTPULSENUM		= (USHORT)(MencV->PulseNo);
		pAsicMicroReg->MREG_MOTPULSENUMH	= (USHORT)(MencV->PulseNo>>16);
	}
/*--------------------------------------------------------------------------------------------------*/
	/* 回転型/リニア型共通 */
	pAsicMicroReg->MREG_MPOSSIGN		= (USHORT)(MencV->MposSign);
	pAsicMicroReg->MREG_KMOTPHASE		= (USHORT)(MencV->Kmotphase);
	pAsicMicroReg->MREG_KMOTPHASEH		= (USHORT)(MencV->Kmotphase>>16);
	pAsicMicroReg->MREG_ACCERRLVL		= (USHORT)(MencV->AccErrLvl);
	pAsicMicroReg->MREG_ACCERRLVLH		= (USHORT)(MencV->AccErrLvl>>16);
	pAsicMicroReg->MREG_DIVOUTSFT		= (USHORT)(MencV->DivOutSft);
	pAsicMicroReg->MREG_ENCLCHNUM		= (USHORT)(MencV->EncLchNum);
	pAsicMicroReg->MREG_SPDMANUMBITI	= (USHORT)(uIniPrm->SpdMaNumBit);
	pAsicMicroReg->MREG_SPGFAIL			= (USHORT)(uIniPrm->spgfail.b.l);
	pAsicMicroReg->MREG_MOTPOS			= (USHORT)(MencV->MotPos);
	pAsicMicroReg->MREG_MOTPOSH			= (USHORT)(MencV->MotPos>>16);
	pAsicMicroReg->MREG_MOTPOSX0		= (USHORT)(MencV->MotPosX[0]);
	pAsicMicroReg->MREG_MOTPOSX0H		= (USHORT)(MencV->MotPosX[0]>>16);
	pAsicMicroReg->MREG_MOTPOSX1		= (USHORT)(MencV->MotPosX[1]);
	pAsicMicroReg->MREG_MOTPOSX1H		= (USHORT)(MencV->MotPosX[1]>>16);
	pAsicMicroReg->MREG_MOTPOSX			= (USHORT)(MencV->MotPosX[2]);
	pAsicMicroReg->MREG_MOTPOSXH		= (USHORT)(MencV->MotPosX[2]>>16);
	pAsicMicroReg->MREG_MOTORGX			= (USHORT)(MencV->MotOrgX);
	pAsicMicroReg->MREG_MOTORGXH		= (USHORT)(MencV->MotOrgX>>16);
	pAsicMicroReg->MREG_MTURN_SHIFT		= (USHORT)(MencV->MultiturnSft);
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_KMOTSPDA		= (USHORT)(uIniPrm->KmotspdA);
	pAsicMicroReg->MREG_KMOTSPDAH		= (USHORT)(uIniPrm->KmotspdA>>16);
	pAsicMicroReg->MREG_KPCMPD			= (USHORT)(pBaseCtrl->PcmpCalcData.Kpcmpd);
	pAsicMicroReg->MREG_KPCMPDH			= (USHORT)(pBaseCtrl->PcmpCalcData.Kpcmpd>>16);
	pAsicMicroReg->MREG_KPCMPSP1		= (USHORT)(pBaseCtrl->PcmpCalcData.Kpcmpsp1);
	pAsicMicroReg->MREG_KPCMPSP1H		= (USHORT)(pBaseCtrl->PcmpCalcData.Kpcmpsp1>>16);
	pAsicMicroReg->MREG_PCMPSPD1		= (USHORT)(pBaseCtrl->PcmpCalcData.PcmpSpd1);
	pAsicMicroReg->MREG_PCMPSPD1H		= (USHORT)(pBaseCtrl->PcmpCalcData.PcmpSpd1>>16);
	pAsicMicroReg->MREG_KPCMPDITPLT		= (USHORT)(pBaseCtrl->PcmpCalcData.KpcmpdItplt);
	pAsicMicroReg->MREG_KPCMPDITPLTH	= (USHORT)(pBaseCtrl->PcmpCalcData.KpcmpdItplt>>16);
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_SBCYCLE			= (USHORT)(uIniPrm->SvCycleNs/KPI_SBCYCLE_BASE_NS);
#if defined(CSW_NETIF_SYNQNET_TYPE)
	pAsicMicroReg->MREG_ASYNC_HSUR2		= (USHORT)(1);
	pAsicMicroReg->MREG_SBMONTIMING		= (USHORT)(0);
#else
	pAsicMicroReg->MREG_ASYNC_HSUR2		= (USHORT)(0);
	pAsicMicroReg->MREG_SBMONTIMING		= (USHORT)(1);
#endif
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_DUMMY = 0x0000;									/* ダミーライト				*/

}
#endif


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************************************/
/*																									*/
/*		位置・速度ゲイン初期パラメータのマイクロ書き込み処理										*/
/*																									*/
/****************************************************************************************************/
void MicroSA_InitGainParameters( ASIC_U_IF_STR *pAsicMicroReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		マイクロＩＦレジスタへの書き込み															*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_SVGKP		= pAsicMicroReg->MREG_BNK1SVGKP;
	pAsicMicroReg->MREG_SVGKPH		= pAsicMicroReg->MREG_BNK1SVGKPH;
	pAsicMicroReg->MREG_SVGKV		= pAsicMicroReg->MREG_BNK1SVGKV;
	pAsicMicroReg->MREG_SVGKVH		= pAsicMicroReg->MREG_BNK1SVGKVH;
	pAsicMicroReg->MREG_SVGKVI		= pAsicMicroReg->MREG_BNK1SVGKVI;
	pAsicMicroReg->MREG_SVGKVIH		= pAsicMicroReg->MREG_BNK1SVGKVIH;
	pAsicMicroReg->MREG_SVGKV2		= pAsicMicroReg->MREG_BNK1SVGKV2;
	pAsicMicroReg->MREG_SVGKV2H		= pAsicMicroReg->MREG_BNK1SVGKV2H;
	pAsicMicroReg->MREG_SVGKV2IV	= pAsicMicroReg->MREG_BNK1SVGKV2IV;
	pAsicMicroReg->MREG_SVGKV2IVH	= pAsicMicroReg->MREG_BNK1SVGKV2IVH;
	pAsicMicroReg->MREG_KTRQFIL		= pAsicMicroReg->MREG_BNK1KTRQFIL;
	pAsicMicroReg->MREG_KTRQFILH	= pAsicMicroReg->MREG_BNK1KTRQFILH;
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_DOBSKS		= pAsicMicroReg->MREG_BNK1DOBSKS;
	pAsicMicroReg->MREG_DOBSKSH		= pAsicMicroReg->MREG_BNK1DOBSKSH;
	pAsicMicroReg->MREG_DOBSKSJ		= pAsicMicroReg->MREG_BNK1DOBSKSJ;
	pAsicMicroReg->MREG_DOBSKSJH	= pAsicMicroReg->MREG_BNK1DOBSKSJH;
	pAsicMicroReg->MREG_DOBSKJ		= pAsicMicroReg->MREG_BNK1DOBSKJ;
	pAsicMicroReg->MREG_DOBSKJH		= pAsicMicroReg->MREG_BNK1DOBSKJH;
	pAsicMicroReg->MREG_DOBSKD		= pAsicMicroReg->MREG_BNK1DOBSKD;
	pAsicMicroReg->MREG_DOBSKDH		= pAsicMicroReg->MREG_BNK1DOBSKDH;
	pAsicMicroReg->MREG_DOBSKLPF	= pAsicMicroReg->MREG_BNK1DOBSKLPF;
	pAsicMicroReg->MREG_DOBSKLPFH	= pAsicMicroReg->MREG_BNK1DOBSKLPFH;
/*--------------------------------------------------------------------------------------------------*/
	pAsicMicroReg->MREG_OVTRQIG		= pAsicMicroReg->MREG_BNK1OVTRQIG;
	pAsicMicroReg->MREG_OVTRQIGH	= pAsicMicroReg->MREG_BNK1OVTRQIGH;
	pAsicMicroReg->MREG_OVTRQKV		= pAsicMicroReg->MREG_BNK1OVTRQKV;
	pAsicMicroReg->MREG_OVTRQKVH	= pAsicMicroReg->MREG_BNK1OVTRQKVH;

	pAsicMicroReg->MREG_DUMMY = 0x0000;									/* ダミーライト				*/

}
#endif


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************************************/
/*																									*/
/*		マイクロ応答データ入力処理																	*/
/*																									*/
/****************************************************************************************************/
void MicroSA_InputCycDataB( ASICS *SvAsicRegs, MENCV *MencV, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm )
{
//	ASIC_HW_IF_STR		*pAsicHwReg;
	MICRO_BITDAT		*pMicroBitDat;
	BOOL				CposLatch;
	MICRO_CYC_INPUTS	*uCycInputs;
#if (CSW_MAICRO_DMA_TRANSFER == 0)
	ASIC_U_IF_STR		*pAsicMicroReg;
	pAsicMicroReg	= SvAsicRegs->AsicMicroReg;
	uCycInputs		= (MICRO_CYC_INPUTS*)&pAsicMicroReg->MREG_BITDAT;
#else
	uCycInputs		= SvAsicRegs->uDMAInBuffer;
#endif

	/* ローカル変数の初期化 */
	pMicroBitDat	= &( SvAsicRegs->MicroBitDat );

/*--------------------------------------------------------------------------------------------------*/
/*		Get Encoder Information																		*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->CphaseDist	= uCycInputs->MREG_CPHDIST + ( (uCycInputs->MREG_CPHDISTH) << 16 );
	MencV->RxAlmCode	= uCycInputs->MREG_RXALMCODE;	/* 今回値読込み：Alarm Code			*/
//	MencV->RxTimex.dw	= pAsicMicroReg->MREG_RXTIMEBASE0;	/* 今回値読込み：TimeBase			*/
//	MencV->MstErrCnt	= pAsicMicroReg->MREG_MSTERRCNT;
//	MencV->AccErrCnt	= pAsicMicroReg->MREG_ACCERRCNT;
//	MencV->ComErrCnt	= pAsicMicroReg->MREG_COMERRCNT;

/*--------------------------------------------------------------------------------------------------*/
/*		Get Control Loop Output																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reference and Feedback Monitor */
	CtrlLoopOut->MotSpd_a	= uCycInputs->MREG_MOTSPDA + ( (uCycInputs->MREG_MOTSPDAH) << 16 );
	CtrlLoopOut->IdRefMon = (LONG)((SHORT)uCycInputs->MREG_IDO);
//	CtrlLoopOut->TrqRefMon_a = CtrlLoopOut->TrqRefMon;
//	CtrlLoopOut->IqRefMon_a = CtrlLoopOut->IdRefMon;


	/* Control Loop Status Monitor */
	pMicroBitDat->data		= uCycInputs->MREG_BITDAT;
/*--------------------------------------------------------------------------------------------------*/
/*		Make Bit Status																				*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->SeRxErr	= pMicroBitDat->data & BITDAT_SERXERR;
	MencV->Cset		= pMicroBitDat->data & BITDAT_CSET;
	MencV->Cpass	= pMicroBitDat->data & BITDAT_CPASS;
	MencV->OrgOk	= pMicroBitDat->data & BITDAT_ORGOK;
	CposLatch		= pMicroBitDat->data & BITDAT_CPOSLATCH;
	if( MencV->IncreCposLatch != CposLatch )
	{
		MencV->IncreCposLatch = CposLatch;
	}
	MencV->DivCpass	= pMicroBitDat->data & BITDAT_DIVCPASS;

	CtrlLoopOut->SvonSpdLmtFlag	= pMicroBitDat->data & BITDAT_SVONSPDLMT;
	CtrlLoopOut->SpdClamp			= pMicroBitDat->data & BITDAT_SPDCLAMP;
	CtrlLoopOut->PConActFlg		= pMicroBitDat->data & BITDAT_PCONACTFLG;
	CtrlLoopOut->TrqClamp = ((uCycInputs->MREG_CTSTRW & 0x0020) != 0) ? TRUE : FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		Get Encoder Error Status																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->EncDataNG = (0 != ( pMicroBitDat->data & BITDAT_ENCDATANG ));

}

void MicroSA_InputCycDataExB( ASICS *SvAsicRegs, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm )
{
	MICRO_CYC_INPUTS	*uCycInputs;
#if (CSW_MAICRO_DMA_TRANSFER == 0)
	ASIC_U_IF_STR		*pAsicMicroReg;
	pAsicMicroReg	= SvAsicRegs->AsicMicroReg;
	uCycInputs		= (MICRO_CYC_INPUTS*)&pAsicMicroReg->MREG_BITDAT;
#else
	uCycInputs		= SvAsicRegs->uDMAInBuffer;
#endif

	CtrlLoopOut->MotPosErr	= uCycInputs->MREG_MOTPOSERR + ( (uCycInputs->MREG_MOTPOSERRH) << 16 );
	CtrlLoopOut->SpdRefo	= uCycInputs->MREG_MONSPDREF + ( (uCycInputs->MREG_MONSPDREFH) << 16 );
	CtrlLoopOut->SpdFFC	= uCycInputs->MREG_MONSPDFFC + ( (uCycInputs->MREG_MONSPDFFCH) << 16 );
	CtrlLoopOut->TrqRefo	= uCycInputs->MREG_TRQFILOUT + (uCycInputs->MREG_TRQFILOUTH << 16);
	CtrlLoopOut->TrqRefo_a	= uCycInputs->MREG_TRQREFOA + ( (uCycInputs->MREG_TRQREFOAH) << 16 );

/*--------------------------------------------------------------------------------------------------*/
/*		Get Current Loop Output																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* μ内最終q軸電流指令(平均値) */
	CtrlLoopOut->IqRefMon_a = (LONG)((SHORT)uCycInputs->MREG_IQMONA) * Bprm->DirSign;
	CtrlLoopOut->TrqRefMon_a = MlibMulhigh32( (CtrlLoopOut->IqRefMon_a << 16), NORM_IQ_TO_TRQ );

}
#endif


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************************************/
/*																									*/
/*		マイクロサーボ制御指令出力処理																*/
/*																									*/
/****************************************************************************************************/
void MicroSA_OutputCycDataB( ASICS *SvAsicRegs, MENCV *MencV, BASE_CTRL_HNDL *pBaseCtrl, LONG CmdCtrlBit )
{
	ASIC_U_IF_STR		*pAsicMicroReg;
//	MICRO_BITCMD		*pMicroBitCmd;
	USHORT				MicroBitCmd;
	MICRO_CYC_OUTPUTS	*uCycOutputs;

	/* ローカル変数の初期化 */
	pAsicMicroReg		= SvAsicRegs->AsicMicroReg;
//	pMicroBitCmd		= &(SvAsicRegs->MicroBitCmd);

#if (CSW_MAICRO_DMA_TRANSFER != 0)
	uCycOutputs		= SvAsicRegs->uDMAOutBuffer;
#else
	uCycOutputs		= (MICRO_CYC_OUTPUTS*)&pAsicMicroReg->MREG_BITCMDI;
#endif

	/* bit情報設定 */
	MicroBitCmd = 0;
	/* P/PI切り替え */
	MicroBitCmd |= (((CmdCtrlBit >> ENBPCTRL_BITNO) & 0x01) * BITCMD_PCON);
	/* モードSW設定 */
	MicroBitCmd |= (pBaseCtrl->ModeSwData.var.ModeSWFlag * BITCMD_MODESWON);
	/* 位置偏差クリア(テーブル運転/PJOG専用) */
	MicroBitCmd |= (pBaseCtrl->CtrlCmdMngr.PerrClrReq * BITCMD_PERCLRCMD);
	/* 位置積分クリア */
	MicroBitCmd |= (((CmdCtrlBit >> CLRPOSINTG_BITNO) & 0x01) * BITCMD_PICVCLRCMD);
	/* ScanA停止要求 */
	MicroBitCmd |= (MencV->EncDisable * BITCMD_SCANAPASSREQ);
	/* テーブル運転要求 */
	if( pBaseCtrl->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{
		MicroBitCmd |= BITCMD_TBLDRVON;
	}
	/* Cphase Latch Clear Request */
	MicroBitCmd |= (pBaseCtrl->CLatchClrReq * BITCMD_CLATCHCLR);
	pBaseCtrl->CLatchClrReq = pBaseCtrl->CLatchClrReq^0x1;
	/* 調整レス有効/無効選択 */
	MicroBitCmd |= (pBaseCtrl->TuneLessCtrl.var.TuneLessAct * BITCMD_TUNELESSREQ);
	/* 摩擦補償選択 (1:摩擦補償有効) */
	MicroBitCmd |= (pBaseCtrl->DobsCtrl.DobsAct * BITCMD_DOBSON);
	/* 自動ゲイン切り替え選択 */
	MicroBitCmd |= (pBaseCtrl->GainChange.AtGsel * BITCMD_AUTOGAINCHNG);
	/* ゲイン切り替え要求 */
	if( pBaseCtrl->GainChange.ActGain == 2 )
	{
		MicroBitCmd |= BITCMD_GAINCHNGON;
	}
	/* BBSvcRefClr */
	MicroBitCmd |= (pBaseCtrl->BBRefClrReq * BITCMD_BBSVCREFCLR);
	/* トルク指令フィルタ無効選択 */
	MicroBitCmd |= (pBaseCtrl->TblDrive.var.TrqFilDisable * BITCMD_TRQFILINVLD);
	/* 一次LPF積分初期化要求 */
	MicroBitCmd |= (pBaseCtrl->CtrlCmdMngr.TrqFilClrCmd * BITCMD_LPFINTEGINIT);
	/* 二次LPF要求 */
	MicroBitCmd |= (pBaseCtrl->CtrlCmdPrm.LpassFil2 * BITCMD_LPASSFIL2ON);

/*--------------------------------------------------------------------------------------------------*/
	uCycOutputs->MREG_BITCMDI  = (USHORT)MicroBitCmd;
	uCycOutputs->MREG_ZCTRMDI  = (USHORT)(pBaseCtrl->CtrlModeSet.ZctrlMode.zm);

	switch( pBaseCtrl->CtrlCmdMngr.CtrlMode )
	{
	case CTRLMODE_TRQ:	/* トルク制御モード */
		uCycOutputs->MREG_CTRLMDI	= (USHORT)(CTRLMODE_TRQ);
//		uCycOutputs->MREG_TRQREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqRefo);
//		uCycOutputs->MREG_TRQREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqRefo>>16);
		uCycOutputs->MREG_SPDREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdLimit);
		uCycOutputs->MREG_SPDREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdLimit>>16);
		break;

	case CTRLMODE_POS:	/* 位置制御モード */
		uCycOutputs->MREG_CTRLMDI	= (USHORT)(CTRLMODE_POS);
		uCycOutputs->MREG_DPOSREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.dPosRefo);
		uCycOutputs->MREG_DPOSREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.dPosRefo>>16);
		uCycOutputs->MREG_SPDREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdFFCx);
		uCycOutputs->MREG_SPDREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdFFCx>>16);
		uCycOutputs->MREG_TRQREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqFFCx);
		uCycOutputs->MREG_TRQREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqFFCx>>16);
		if( pBaseCtrl->CtrlCmdMngr.LastCtrlMode != CTRLMODE_POS )
		{
			pAsicMicroReg->MREG_BASEMOTPOS	= (USHORT)(pBaseCtrl->CtrlCmdMngr.BaseFbkPos);
			pAsicMicroReg->MREG_BASEMOTPOSH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.BaseFbkPos>>16);
		}
		break;

	case CTRLMODE_SPD:	/* 速度制御モード */
	case CTRLMODE_JOG:	/* ＪＯＧ運転モード */
	case CTRLMODE_ZSRCH:/* 原点サーチモード */
		uCycOutputs->MREG_CTRLMDI	= (USHORT)(CTRLMODE_SPD);
		uCycOutputs->MREG_SPDREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdRefo);
		uCycOutputs->MREG_SPDREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdRefo>>16);
		uCycOutputs->MREG_TRQREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqFFCx);
		uCycOutputs->MREG_TRQREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqFFCx>>16);
		if( pBaseCtrl->CtrlCmdMngr.LastCtrlMode == CTRLMODE_TRQ )
		{ /* 制御切り替え時速度積分値 */
			pAsicMicroReg->MREG_IVARL	= (USHORT)((LONG)pBaseCtrl->CtrlCmdMngr.VelIvar64[0]);
			pAsicMicroReg->MREG_IVARLH	= (USHORT)(((LONG)pBaseCtrl->CtrlCmdMngr.VelIvar64[0])>>16);
		}
		break;

	default:
		break;
	}

	pBaseCtrl->CtrlCmdMngr.LastCtrlMode = pBaseCtrl->CtrlCmdMngr.CtrlMode;

	/*----------------------------------------------------------------------------------------------*/
	/*		Set Torque Limit Value to Micro Program													*/
	/*----------------------------------------------------------------------------------------------*/
	/* 正側トルク(電流)制限値 */
	uCycOutputs->MREG_TLMTP24IN		= (USHORT)(pBaseCtrl->TrqLimitData.var.PosTrqLmtOut);
	uCycOutputs->MREG_TLMTP24INH	= (USHORT)(pBaseCtrl->TrqLimitData.var.PosTrqLmtOut >> 16);
	/* 負側トルク(電流)制限値 */
	uCycOutputs->MREG_TLMTM24IN		= (USHORT)(pBaseCtrl->TrqLimitData.var.NegTrqLmtOut);
	uCycOutputs->MREG_TLMTM24INH	= (USHORT)(pBaseCtrl->TrqLimitData.var.NegTrqLmtOut >> 16);

	/*----------------------------------------------------------------------------------------------*/
	/*		Set DC Volt Avarage Value to Micro Program for Weaken Field Param Calc					*/
	/*----------------------------------------------------------------------------------------------*/
	uCycOutputs->MREG_DCVOLTA = pBaseCtrl->WeakenField->var.VdcAsic;

	/*----------------------------------------------------------------------------------------------*/
	/*		Set Voltage Compensation Gain to Micro Program											*/
	/*----------------------------------------------------------------------------------------------*/
	uCycOutputs->MREG_KVVIN = pBaseCtrl->MpKvvComp->var.MpKvv;

#if (CSW_MAICRO_DMA_TRANSFER == 0)
	/* サーボASIC不具合No.3 対応(ダミーライト) *//* 2008.04.02 */
	pAsicMicroReg->MREG_DUMMY = 0x0000;									/* ダミーライト				*/
#endif

}

void MicroSA_OutputCycDataExB( ASICS *SvAsicRegs, BASE_CTRL_HNDL *pBaseCtrl)
{
	ASIC_U_IF_STR		*pAsicMicroReg;
	MICRO_CYC_OUTPUTS	*uCycOutputs;

	/* ローカル変数の初期化 */
	pAsicMicroReg		= SvAsicRegs->AsicMicroReg;

#if (CSW_MAICRO_DMA_TRANSFER != 0)
	uCycOutputs		= SvAsicRegs->uDMAOutBuffer;
#else
	uCycOutputs		= (MICRO_CYC_OUTPUTS*)&pAsicMicroReg->MREG_BITCMDI;
#endif

	/* テーブル運転要求 */
	if( pBaseCtrl->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{
		pAsicMicroReg->MREG_DISTRQINTMG = (USHORT)pBaseCtrl->TblDrive.var.DisTrqInTiming;
		if( pBaseCtrl->TblDrive.var.NotchLpassDisable == TRUE )
		{ /* ノッチ＆ローパスフィルタ無効要求時 */
			pAsicMicroReg->MREG_CSW = SvAsicRegs->MicroCsw.data | 0x00F0;
		}
		else
		{
			pAsicMicroReg->MREG_CSW = SvAsicRegs->MicroCsw.data;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		拡張制御出力設定(速度FF/トルクFFは制御指令として設定済み)									*/
/*--------------------------------------------------------------------------------------------------*/
	uCycOutputs->MREG_SPDFBI		= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdFBCx);
	uCycOutputs->MREG_SPDFBIH		= (USHORT)(pBaseCtrl->CtrlCmdMngr.SpdFBCx>>16);

	/*----------------------------------------------------------------------------------------------*/
	/*		Set Table Torque reference													*/
	/*----------------------------------------------------------------------------------------------*/
	uCycOutputs->MREG_TRQTBL	= (USHORT)(pBaseCtrl->TblDrive.var.TrqRef);
	uCycOutputs->MREG_TRQTBLH	= (USHORT)(pBaseCtrl->TblDrive.var.TrqRef>>16);

}
#endif


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************
  Input the monitor data from JL-077 ScanA
*****************************************************************************/
void MicroSA_InputCycDataA(ASICS *SvAsicRegs, MENCV *MencV, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm)
{
	MICRO_CYC_INPUTS	*uCycInputs;
	ASIC_U_IF_STR		*pAsicMicroReg;
	DWORDX				pos;

	pAsicMicroReg	= SvAsicRegs->AsicMicroReg;
	uCycInputs		= (MICRO_CYC_INPUTS*)&pAsicMicroReg->MREG_BITDAT;

	pos.w.l = uCycInputs->MREG_RSPMOTPOS;
	pos.w.h = uCycInputs->MREG_RSPMOTPOSH;

	/* トルク制限/μ内最終q軸電流指令	*/
	CtrlLoopOut->IqRefMon = (LONG)((SHORT)uCycInputs->MREG_IQMON) * Bprm->DirSign;
	CtrlLoopOut->IdRefMon = (LONG)((SHORT)uCycInputs->MREG_IDO);
	CtrlLoopOut->TrqRefMon = MlibMulhigh32( (CtrlLoopOut->IqRefMon << 16), NORM_IQ_TO_TRQ );
	MencV->MotAbsPos += (pos.Long - MencV->MotPos);
	MencV->MotPos = pos.Long;
}
#endif


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************
  Output the command to JL-077 ScanA
*****************************************************************************/
void MicroSA_OutputCycDataA(ASICS *SvAsicRegs, BASE_CTRL_HNDL *pBaseCtrl)
{
	ASIC_U_IF_STR		*pAsicMicroReg;
	MICRO_CYC_OUTPUTS	*uCycOutputs;

	pAsicMicroReg	= SvAsicRegs->AsicMicroReg;
	uCycOutputs		= (MICRO_CYC_OUTPUTS*)&pAsicMicroReg->MREG_BITCMDI;

	if( pBaseCtrl->CtrlCmdMngr.CtrlMode == CTRLMODE_TRQ )	/* トルク制御モード */
	{
		uCycOutputs->MREG_TRQREFI	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqRefo);
		uCycOutputs->MREG_TRQREFIH	= (USHORT)(pBaseCtrl->CtrlCmdMngr.TrqRefo>>16);
	}
}
#endif


/****************************************************************************
  Baseloop Output to JL-086 MicroProgram
*****************************************************************************/
void MicroIfInputCycData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs )
{
	BPRMDAT			*Bprm;
	BOOL			BeComplete;
	SHORT			wkIqfb;
	SHORT			wkIdfb;

	Bprm = BaseLoops->Bprm;
	BeComplete = (BaseLoops->BaseCtrls)->BeComplete;

/*--------------------------------------------------------------------------------------------------*/
/*		Latch ASIC Pulse Counter																	*/
/*--------------------------------------------------------------------------------------------------*/
	//SHalDrv_LatchPulseCntr( SvAsicRegs->AsicHwReg );

#ifndef _JL076_USE_
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロプログラムからのマイクロ内最終電流指令取得処理								*/
/*--------------------------------------------------------------------------------------------------*/
	if( Bprm->RvsDir )
	{ /* 逆回転モード */
		/* マイクロ内最終d軸電流指令 */
		CtrlLoopOut->IdRefMon = SvAsicRegs->AsicMicroReg->WeakFV.IdOut;
		/* マイクロ内最終q軸電流指令 */
		CtrlLoopOut->IqRefMon = -SvAsicRegs->AsicMicroReg->IntAdV.IqMon;

#if CSW_POWER_MONITOR_USE == TRUE /* for 消費電力モニタ <S020> */
		BaseLoops->PowMonVq = -SvAsicRegs->AsicMicroReg->VcmpV.VqOut;
		BaseLoops->PowMonIqd = -SvAsicRegs->AsicMicroReg->IntAdV.IqInData;
		BaseLoops->PowMonVd = SvAsicRegs->AsicMicroReg->VcmpV.VdOut;
		BaseLoops->PowMonIdd = SvAsicRegs->AsicMicroReg->IntAdV.IdInData;
#endif

	}
	else
	{ /* 正回転モード */
		/* マイクロ内最終d軸電流指令 */
		CtrlLoopOut->IdRefMon =  SvAsicRegs->AsicMicroReg->WeakFV.IdOut;
		/* マイクロ内最終q軸電流指令 */
		CtrlLoopOut->IqRefMon =  SvAsicRegs->AsicMicroReg->IntAdV.IqMon;

#if CSW_POWER_MONITOR_USE == TRUE /* for 消費電力モニタ <S020> */
		BaseLoops->PowMonVq = SvAsicRegs->AsicMicroReg->VcmpV.VqOut;
		BaseLoops->PowMonIqd = SvAsicRegs->AsicMicroReg->IntAdV.IqInData;
		BaseLoops->PowMonVd = SvAsicRegs->AsicMicroReg->VcmpV.VdOut;
		BaseLoops->PowMonIdd = SvAsicRegs->AsicMicroReg->IntAdV.IdInData;
#endif
	}
	/*----------------------------------------------------------------------------------------------*/
	CtrlLoopOut->IqRefMon_a = ( CtrlLoopOut->IqRefMon + CtrlLoopOut->IqRefMon_l ) >> 1;
	CtrlLoopOut->IqRefMon_l = CtrlLoopOut->IqRefMon;

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用電流FB値処理：BB時はモニタ用電流FB値を0にする										*/
/*			⇒Unモニタ、トレースデータにてサーボオフ時に電流検出の最終保持値が出力されるため		*/
/*--------------------------------------------------------------------------------------------------*/
	wkIqfb = SvAsicRegs->AsicMicroReg->IntAdV.IqInData;			/* マイクロ内q軸電流FB				*/
	wkIdfb = SvAsicRegs->AsicMicroReg->IntAdV.IdInData;			/* マイクロ内d軸電流FB				*/
	if( BeComplete == FALSE )
	{ /* BaseBlock中 */
		/* モニタ用電流FB値 = 0 */
		CtrlLoopOut->IqFbMon = 0;
		CtrlLoopOut->IdFbMon = 0;
	}
	else
	{ /* Base Enable */
		/* モニタ用電流FB値設定 */
		CtrlLoopOut->IqFbMon = wkIqfb;
		CtrlLoopOut->IdFbMon = wkIdfb;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		q軸電流指令モニタ値取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	CtrlLoopOut->IqRefFilMon = SvAsicRegs->AsicMicroReg->IntAdV.IqMonFil;
	/* q軸電流指令モニタ(フィルタ後)符号計算 */
	CtrlLoopOut->IqRefFilMon *= Bprm->DirSign;

/*--------------------------------------------------------------------------------------------------*/
/*		トルクリミット情報取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	CtrlLoopOut->TrqClamp = ((SvAsicRegs->AsicMicroReg->StsFlg.CtrlStsRW & 0x0020) != 0) ? TRUE : FALSE; /* <S04B> */

#else /* _JL076_USE_ */

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロプログラムからのマイクロ内最終電流指令取得処理								*/
/*--------------------------------------------------------------------------------------------------*/
	if( Bprm->RvsDir )
	{ /* 逆回転モード */
		/* マイクロ内最終d軸電流指令 */
		CtrlLoopOut->IdRefMon = SvAsicRegs->AsicMicroReg->MREG_IDO;
		/* マイクロ内最終q軸電流指令 */
		CtrlLoopOut->IqRefMon = -SvAsicRegs->AsicMicroReg->MREG_IQMON;
	}
	else
	{ /* 正回転モード */
		/* マイクロ内最終d軸電流指令 */
		CtrlLoopOut->IdRefMon =  SvAsicRegs->AsicMicroReg->MREG_IDO;
		/* マイクロ内最終q軸電流指令 */
		CtrlLoopOut->IqRefMon =  SvAsicRegs->AsicMicroReg->MREG_IQMON;
	}
	/*----------------------------------------------------------------------------------------------*/
	CtrlLoopOut->IqRefMon_a = ( CtrlLoopOut->IqRefMon + CtrlLoopOut->IqRefMon_l ) >> 1;
	CtrlLoopOut->IqRefMon_l = CtrlLoopOut->IqRefMon;

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用電流FB値処理：BB時はモニタ用電流FB値を0にする										*/
/*			⇒Unモニタ、トレースデータにてサーボオフ時に電流検出の最終保持値が出力されるため		*/
/*--------------------------------------------------------------------------------------------------*/
	wkIqfb = SvAsicRegs->AsicMicroReg->MREG_IQD;				/* マイクロ内q軸電流FB				*/
	wkIdfb = SvAsicRegs->AsicMicroReg->MREG_IDD;				/* マイクロ内d軸電流FB				*/
	if( BeComplete == FALSE )
	{ /* BaseBlock中 */
		/* モニタ用電流FB値 = 0 */
		CtrlLoopOut->IqFbMon = 0;
		CtrlLoopOut->IdFbMon = 0;
	}
	else
	{ /* Base Enable */
		/* モニタ用電流FB値設定 */
		CtrlLoopOut->IqFbMon = wkIqfb;
		CtrlLoopOut->IdFbMon = wkIdfb;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		q軸電流指令モニタ値取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	CtrlLoopOut->IqRefFilMon = SvAsicRegs->AsicMicroReg->MREG_IQMONFIL;
	/* q軸電流指令モニタ(フィルタ後)符号計算 */
	CtrlLoopOut->IqRefFilMon *= Bprm->DirSign;

#endif /* _JL076_USE_ */

/* <S1B7> */
/*--------------------------------------------------------------------------------------------------*/
/*		モータレステストモード用電流データ設定														*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->MotorLessTest->MotorLessTestMode == TRUE )
	{
		ApiSetCurDatasMotorLessTestMode( BaseLoops, CtrlLoopOut );
	}

	return;
}


/****************************************************************************
  Baseloop Output to JL-086 MicroProgram
*****************************************************************************/
void MicroIfOutputCycData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs )
{
	BASE_CTRL_HNDL	*BaseCtrls;
	PHASECMP		*PhaseComp;
	MENCV			*MencV;
	BPRMDAT			*Bprm;

	BaseCtrls = BaseLoops->BaseCtrls;
	PhaseComp = &(BaseLoops->PhaseComp);
	MencV = BaseLoops->MencV;
	Bprm = BaseLoops->Bprm;

#ifndef _JL076_USE_
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロプログラムへのモータ位相出力 [65536/360deg]									*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->PhaseOrderUWV == FALSE )
	{
		/* 相順：UVW */
		SvAsicRegs->AsicMicroReg->AdinV.PhaseHIn = MencV->MotPhaseX;
		/* 位相補間量 */
		SvAsicRegs->AsicMicroReg->PhaseV.PhaseIpIn = PhaseComp->var.PcmpdItplt;
		/* 位相補間フラグ */
		SvAsicRegs->AsicMicroReg->PhaseV.PhaseIpFIn = 0;
	}
	else
	{
		/* 相順：UWV */
		SvAsicRegs->AsicMicroReg->AdinV.PhaseHIn = (0x8000 - MencV->MotPhaseX);
		/* 位相補間量 */
		SvAsicRegs->AsicMicroReg->PhaseV.PhaseIpIn = -PhaseComp->var.PcmpdItplt;
		/* 位相補間フラグ */
		SvAsicRegs->AsicMicroReg->PhaseV.PhaseIpFIn = 0;
	}
#else /* _JL076_USE_ */
	if( MencV->PhaseOrderUWV == FALSE )
	{
		/* 相順：UVW */
		SvAsicRegs->AsicMicroReg->MREG_PHAIN = MencV->MotPhaseX;
		/* 位相補間量 */
		SvAsicRegs->AsicMicroReg->MREG_PHAIPIN = PhaseComp->var.PcmpdItplt;
		/* 位相補間フラグ */
		SvAsicRegs->AsicMicroReg->MREG_PHAIPFLGIN = 0;
	}
	else
	{
		/* 相順：UWV */
		SvAsicRegs->AsicMicroReg->MREG_PHAIN = (0x8000 - MencV->MotPhaseX);
		/* 位相補間量 */
		SvAsicRegs->AsicMicroReg->MREG_PHAIPIN = -PhaseComp->var.PcmpdItplt;
		/* 位相補間フラグ */
		SvAsicRegs->AsicMicroReg->MREG_PHAIPFLGIN = 0;
	}
#endif /* _JL076_USE_ */

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロプログラムへの電流指令・電流制限値出力処理									*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef _JL076_USE_
	if( Bprm->RvsDir )
	{ /* 逆回転モード */
		/* d軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->AdinV.IdIn = BaseLoops->IdRef;
		/* q軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->AdinV.IqIn = -BaseLoops->IqRef;

		/* 正側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->AdinV.TLimPIn =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.NegTrqLmtOut, -3840000 );
		/* 負側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->AdinV.TLimMIn =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.PosTrqLmtOut,  3840000 );
	}
	else
	{ /* 正回転モード */
		/* d軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->AdinV.IdIn = BaseLoops->IdRef;
		/* q軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->AdinV.IqIn = +BaseLoops->IqRef;

		/* 正側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->AdinV.TLimPIn =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.PosTrqLmtOut,  3840000 );
		/* 負側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->AdinV.TLimMIn =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.NegTrqLmtOut, -3840000 );
	}


#else /* _JL076_USE_ */
	if( Bprm->RvsDir )
	{ /* 逆回転モード */
		/* d軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->MREG_IDIN = BaseLoops->IdRef;
		/* q軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->MREG_IQIN = -BaseLoops->IqRef;

		/* 正側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->MREG_TLMTPIN =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.NegTrqLmtOut, -3840000 );
		/* 負側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->MREG_TLMTMIN =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.PosTrqLmtOut,  3840000 );
	}
	else
	{ /* 正回転モード */
		/* d軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->MREG_IDIN = BaseLoops->IdRef;
		/* q軸電流指令 [15000/Imax] */
		SvAsicRegs->AsicMicroReg->MREG_IQIN = +BaseLoops->IqRef;

		/* 正側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->MREG_TLMTPIN =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.PosTrqLmtOut,  3840000 );
		/* 負側トルク(電流)制限値 */
		SvAsicRegs->AsicMicroReg->MREG_TLMTMIN =
					(USHORT)MlibMulhigh32( BaseCtrls->TrqLimitData.var.NegTrqLmtOut, -3840000 );
	}

#endif /* _JL076_USE_ */

// <S1B7> 完全削除する (MicroIfInputCycData()にて設定を行う)
//#if 0 /* ★モータレステストモード現状未対応★ */
///*--------------------------------------------------------------------------------------------------*/
///*		モータレステストモード用電流データ設定														*/
///*--------------------------------------------------------------------------------------------------*/
//	if(Kprm.f.MotorLessTestMode)
//	{
//		ApiSetCurDatasMotorLessTestMode( );
//	}
//#endif /* モータレステストモード現状未対応 */

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロプログラムへの電圧補償ゲイン出力											*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseCtrls->MpKvvComp.conf.MpKvvFlg )
	{
		/* 電圧補償ゲイン出力 [8192/VdcBase] */
#ifndef _JL076_USE_
		SvAsicRegs->AsicMicroReg->IntAdP.KvvIn = BaseCtrls->MpKvvComp.var.MpKvv;
#else /* _JL076_USE_ */
		SvAsicRegs->AsicMicroReg->MREG_KVVIN = BaseCtrls->MpKvvComp.var.MpKvv;
#endif /* _JL076_USE_ */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロプログラムへの弱め界磁制御関連変数出力		<S024>							*/
/*--------------------------------------------------------------------------------------------------*/
	/* 電圧FB比例ゲイン(下位16bit) */
	SvAsicRegs->AsicMicroReg->WeakFV.WfKpVLIn = BaseCtrls->WeakenField.var.KpvL;
	/* 電圧FB比例ゲイン(上位16bit) */
	SvAsicRegs->AsicMicroReg->WeakFV.WfKpVHIn = BaseCtrls->WeakenField.var.KpvH;
	/* 電圧FB積分ゲイン(下位16bit) */
	SvAsicRegs->AsicMicroReg->WeakFV.WfKiVLIn = BaseCtrls->WeakenField.var.KivL;
	/* 電圧FB積分ゲイン(上位16bit) */
	SvAsicRegs->AsicMicroReg->WeakFV.WfKiVHIn = BaseCtrls->WeakenField.var.KivH;
	/* 電圧指令制限値 */
	SvAsicRegs->AsicMicroReg->WeakFV.WfV1MaxIn = BaseCtrls->WeakenField.conf.V1Max;
	/* d軸電流指令リミット */
	SvAsicRegs->AsicMicroReg->WeakFV.WfIdRefLimIn = BaseCtrls->WeakenField.var.IdrefLim;

/*--------------------------------------------------------------------------------------------------*/
/*		q軸電流指令モニタ，q軸外乱トルク書き込み処理												*/
/*--------------------------------------------------------------------------------------------------*/
	/* q軸外乱トルク指令符号計算 */
	BaseLoops->IqDist *= (SHORT)Bprm->DirSign;
	/* q軸外乱トルク指令設定 */
#ifndef _JL076_USE_
	SvAsicRegs->AsicMicroReg->IntAdV.IqDistIn = BaseLoops->IqDist;
#else /* _JL076_USE_ */
	SvAsicRegs->AsicMicroReg->MREG_IQDISTIN = BaseLoops->IqDist;
#endif /* _JL076_USE_ */

#if 0 /* 2012.09.04 Y.Oka ★アナログモニタ現状未対応★ */
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ最終電流指令＆アナログモニタ用ＡＳＩＣデータ読込み									*/
/*--------------------------------------------------------------------------------------------------*/
	BinK.AsicMon0 = ASIC_RD( *Gprm.AsicMonAdr0 );		/* アナログモニタCH0						*/
	BinK.AsicMon1 = ASIC_RD( *Gprm.AsicMonAdr1 );		/* アナログモニタCH1						*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣマイクロへの同期指令																*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->AsicHwReg->MREG_HSUR = MPCMD_SYNC1;		/* 同期指令 : Host-CPU --> Asic-CPU			*/
	return;
}



/***************************************** end of file **********************************************/
