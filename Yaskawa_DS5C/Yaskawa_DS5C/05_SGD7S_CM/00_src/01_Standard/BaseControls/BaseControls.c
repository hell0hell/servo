/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControls.c : �T�[�{�x�[�X�R���g���[���Ǘ�������`										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
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
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "BaseControls.h"
#include "SerialEncIf.h"
#include "PosManager.h"
#include "ResVibCtrl.h"
#include "GainChange.h"
//#include "SequenceControls.h"
#include "PowerManager.h"
#include "MotorPoleFind.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "AlarmManager.h"



/****************************************************************************************************/
/*		DEFINES																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Output Mapping Data and Output Macro Definition												*/
/*--------------------------------------------------------------------------------------------------*/
#define	SOUT00	5
#define	SOUT01	6
#define	SOUT02	7
#define	SOUT03	4

/*--------------------------------------------------------------------------------------------------*/
/*		Mechatrolink Monitor Table																	*/
/*--------------------------------------------------------------------------------------------------*/
#define MONTBL10	0x10
#define MONTBL20	0x20
#define MONTBL30	0x30
#define MONTBL40	0x40
#define MONTBL50	0x50
#define MONTBL60	0x60


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void BpxBinputNetIf( BASE_CTRL_HNDL*, SERVO_CONTROL_IF*, TRQ_LMT_DATA* );						*/
/*	void BpxYecServoControl( BASE_CTRL_HNDL*, SERVO_CONTROL_IF*, CTRL_LOOP_OUT*, FUN_CMN_CONTROL* );*/
/*	void BpiMpoleFindMakePosCmd( BASE_LOOPCTRLS*, SERVO_CONTROL_IF*, FN_CMN_CONTROL* );		<S02F>	*/
/*	void BpxTorqueLimitControl( TRQ_LMT_DATA*, BASE_CTRL_HNDL*, BOOL );								*/
/*	void BpxBoutputNetIf( SERVO_CONTROL_IF*, SEQ_CTRL_OUT*, LONG, LONG );							*/
/*	void BpiChangeSpdDetMaFil( MOTSPDMAFIL*, LONG num );								<S02F>		*/
/****************************************************************************************************/
static void LpxNetScanBInputExtCommand( TRQ_LMT_DATA *TrqLimits, USHORT ExtSel,
																		EXTENDED_DATA ExtData );
#if	(CSW_SVCF_FBLATCH == TRUE)
static void LpxLatchRequestGet( SERVO_CONTROL_IF *SvControlIf, LONG CmdLtBit );
#endif
//static void ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err );<S0BA>
static LONG BaseSoftStartSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRef, LONG SpdRefx );
static LONG	BaseLinDecStopSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRefo ); /* <S1F5> */
static LONG ZeroSearchControl( ZX_CTRL_HNDL *ZeroClampCtrl, BASE_MOT_STS *MotSts,
																	KSGAIN ZcKp, LONG JogSpeed );
static LONG OverTorqueCalculation( CTRL_CMD_MNG *CtrlCmdMng, CTRL_CMD_PRM *CtrlCmdPrm );
static void LpxSetNetSelMonitor( LONG **MonTbl, LONG SelMon,
															LONG *RspSelMon, LONG *RspMonData );
static void PosRefRenewal( POS_MNG_HNDL *PosManager, CTRLMD CtrlMode );

static void	BpxBinputNetIf( BASE_CTRL_HNDL*, SERVO_CONTROL_IF*, TRQ_LMT_DATA*, LONG ax_no );/* <S1C0>�F���ǉ� */
static void	BpxBoutputNetIf( SERVO_CONTROL_IF*, SEQ_CTRL_OUT*, LONG, LONG );

extern void HALdrv_SetSOut( int axis, UINT x );
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		�X�L�����a�g���w�ߓ��͏���																	*/
/*																									*/
/****************************************************************************************************/
static void LpxNetScanBInputExtCommand( TRQ_LMT_DATA *TrqLimits, USHORT ExtSel, EXTENDED_DATA ExtData )
{
	switch ( ExtSel )
	{
	case	EXTCMD_TRQLMT_P :	/* �����g���N����	*/
		TrqLimits->var.FwTrqLmt = ExtData.dwVal;
		break ;

	case	EXTCMD_TRQLMT_N :	/* �t���g���N����	*/
		TrqLimits->var.RvTrqLmt = ExtData.dwVal;
		break ;

	default					:	/* ���̑�			*/
		break ;
	}
}

#if	(CSW_SVCF_FBLATCH == TRUE)
/****************************************************************************************************/
/*																									*/
/*		���b�`�v���擾����																			*/
/*																									*/
/****************************************************************************************************/
static void LpxLatchRequestGet( SERVO_CONTROL_IF *SvControlIf, LONG CmdLtBit )
{
	LONG	CmdLtEdge;

	CmdLtEdge = SvControlIf->CmdLtBit ^ CmdLtBit;
/*--------------------------------------------------------------------------------------------------*/
/*		�b�����b�`�v�����o����(�ω��G�b�W)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_C_PHASE_BIT) )
	{/* �O�񃉃b�`�v�������񃉃b�`�v�� */
		SvControlIf->LtReqCphs = TRUE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext1���b�`�v�����o����(�ω��G�b�W)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_EXT_SIG1_BIT) )
	{/* �O�񃉃b�`�v�������񃉃b�`�v�� */
		SvControlIf->LtReqExt1 = SvControlIf->ExtLtEnable[0];
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext2���b�`�v�����o����(�ω��G�b�W)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_EXT_SIG2_BIT) )
	{/* �O�񃉃b�`�v�������񃉃b�`�v�� */
		SvControlIf->LtReqExt2 = SvControlIf->ExtLtEnable[1];
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext3���b�`�v�����o����(�ω��G�b�W)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_EXT_SIG3_BIT) )
	{/* �O�񃉃b�`�v�������񃉃b�`�v�� */
		SvControlIf->LtReqExt3 = SvControlIf->ExtLtEnable[2];
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�O�񃉃b�`�v���̕ۑ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->CmdLtBit = CmdLtBit;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		�ʐM���j�^�f�[�^�ݒ�																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T �� :	���j�^�I��(RspSelMon)�A���j�^�f�[�^(RspMonData)����������ɍX�V����B					*/
/*			�ᑬ�p���j�^�f�[�^�̂��߁ANetIfCmdMonSel[]�Ƀ��j�^�I�����i�[����B						*/
/*																									*/
/****************************************************************************************************/
static void LpxSetNetSelMonitor( LONG **MonTbl, LONG SelMon, LONG *RspSelMon, LONG *RspMonData )
{
/* <S1DE>:start */
//		*RspSelMon  = (UCHAR)SelMon&(0x4F);		 /* <S1D7>:�v�f���ύX */
//		*RspMonData = *(MonTbl[SelMon&(0x4F)]);	 /* <S1D7>:�v�f���ύX */

	if(SelMon < 0x50)
	{
		*RspSelMon  = (UCHAR)SelMon;
		*RspMonData = *(MonTbl[SelMon]);
	}
	else
	{
		*RspSelMon  = (UCHAR)SelMon;
		*RspMonData = 0;
	}
/* <S1DE>:end */}


/****************************************************************************************************/
/*																									*/
/*		�X�L�����a�l�b�g���[�N�h�e���͏���															*/
/*																									*/
/****************************************************************************************************/
static void BpxBinputNetIf( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
																		TRQ_LMT_DATA *TrqLimits, LONG ax_no )
{
	SV_CYC_CMD	*CyclicCMD;
	SV_CYC_CTRL	*CyclicCTRL;

	CyclicCMD	= &( SvControlIf->CmdOptionIf.CycCmd );
	CyclicCTRL	= &( SvControlIf->CmdOptionIf.CycCtrl );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�w�߃f�[�^��M�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( CyclicCTRL->CmdUpdate == FALSE )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�V�[�P���X�M������																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* �l�b�g���[�N�����PCL/NCL�M������ */
	TrqLimits->var.PclSignal = (SvControlIf->CmdSeqBit >> ENB_EXTPTRQLMT_BITNO) & 0x1;
	TrqLimits->var.NclSignal = (SvControlIf->CmdSeqBit >> ENB_EXTNTRQLMT_BITNO) & 0x1;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�r�b�g�M������																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->CmdCtrl	= CyclicCMD->CmdCtrl;
	SvControlIf->CmdSeqBit	= CyclicCMD->CmdSeqBit;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�X�L�����a�g���w�ߓ��͏���															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxNetScanBInputExtCommand( TrqLimits, (USHORT)CyclicCTRL->ExtCmdSel1, CyclicCMD->ExtCmd1 );
	LpxNetScanBInputExtCommand( TrqLimits, (USHORT)CyclicCTRL->ExtCmdSel2, CyclicCMD->ExtCmd2 );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F���j�^�I���w�ߓ��͏���																*/
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->MonSel.MonSel1 = CyclicCMD->SelMon1 ;
	SvControlIf->MonSel.MonSel2 = CyclicCMD->SelMon2 ;
	SvControlIf->MonSel.MonSel3 = CyclicCMD->SelMon3 ;
	SvControlIf->MonSel.MonSel4 = CyclicCMD->SelMon4 ;
	SvControlIf->MonSel.MonSel5 = CyclicCMD->SelMon5 ;
	SvControlIf->MonSel.MonSel6 = CyclicCMD->SelMon6 ;
	SvControlIf->MonSel.MonSel7 = CyclicCMD->SelMon7 ;
	SvControlIf->MonSel.MonSel8 = CyclicCMD->SelMon8 ;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�FI/O�o�͎w��																			*/
/*--------------------------------------------------------------------------------------------------*/
//#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)						/* <S045> */
	//ulwk = (CyclicCMD->DOSignal) & 0x0000000F;
	//SvControlIf->NetSout = ( (((ulwk >> 0) & 1) << SOUT01)
	//				  	   | (((ulwk >> 1) & 1) << SOUT02)
	//				  	   | (((ulwk >> 2) & 1) << SOUT03) );
//#else																/* <S045> */
//	HALdrv_SetSOut ( 0, (CyclicCMD->DOSignal) & 0x0000000F );		/* ��ŏ��� */
	HALdrv_SetSOut ( (int)ax_no, (CyclicCMD->DOSignal) & 0x0000007F );		/* ��ŏ��� *//* <S04B> *//* <S1C0>:���ǉ� */
//#endif															/* <S045> */

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�Q�C���ؑ֗p�Q�C���ԍ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if(CyclicCMD->CmdCtrl.SelBank < SVCDEF_NETGSEL_SETNUM)
	{	BaseControls->GselNum = (LONG)CyclicCMD->CmdCtrl.SelBank;}	/* <S145> */

#if	(CSW_SVCF_FBLATCH == TRUE)
/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F���b�`�I���r�b�g�M��(Cphase,Ext1,Ext2,Ext3)											*/
/*--------------------------------------------------------------------------------------------------*/
	LpxLatchRequestGet( SvControlIf, CyclicCMD->CmdLtBit);
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F���䃂�[�h���T�[�{����w�ߓ��͏���													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( CyclicCMD->CmdCtrl.ModSel )
	{
	case NO_CTRL_MODE:	/*--- ���䃂�[�h���� ---------------------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_TRQ;
		SvControlIf->NetTrqRef		= 0;
		SvControlIf->NetSpdLmt		= 0x01000000;
		TrqLimits->var.FwTrqLmt	= NORMAXVALUE;
		TrqLimits->var.RvTrqLmt	= NORMAXVALUE;
		SvControlIf->NetSpdRef		= 0;
		SvControlIf->CmdSeqBit		&= ~(SERVO_ON_BIT);
		break;
	case POS_CTRL_MODE:	/*--- �ʒu���䃂�[�h ---------------------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_POS;
		SvControlIf->NetPosRef		= CyclicCMD->Position;
		SvControlIf->NetSpdFFC		= CyclicCMD->Speed;
		SvControlIf->NetTrqFFC		= CyclicCMD->Torque;
		SvControlIf->NetSpdRef		= CyclicCMD->Speed;
		break;
	case SPD_CTRL_MODE:	/*--- ���x���䃂�[�h ---------------------------------------------------*/
		/* ���j�g���N�����́A�g���w�߂Őݒ肳���B */
		SvControlIf->NetCtrlMcmd	= CTRLMODE_SPD;
		SvControlIf->NetSpdRef		= CyclicCMD->Speed;
		SvControlIf->NetTrqFFC		= CyclicCMD->Torque;
		break;
	case TRQ_CTRL_MODE:	/*--- �g���N���䃂�[�h -------------------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_TRQ;
		SvControlIf->NetTrqRef		= CyclicCMD->Torque;
		SvControlIf->NetSpdLmt		= CyclicCMD->Speed;
		TrqLimits->var.FwTrqLmt	= NORMAXVALUE;
		TrqLimits->var.RvTrqLmt	= NORMAXVALUE;
		SvControlIf->NetSpdRef		= CyclicCMD->Speed;
		break;
	default:	/*--------------- �f�t�H���g(����)���[�h ---------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_TRQ;
		SvControlIf->NetTrqRef		= 0;
		SvControlIf->NetSpdLmt		= 0;
		TrqLimits->var.FwTrqLmt	= NORMAXVALUE;
		TrqLimits->var.RvTrqLmt	= NORMAXVALUE;
		SvControlIf->CmdSeqBit		&= ~(SERVO_ON_BIT);
		SvControlIf->NetSpdRef		= 0;
		break;
	}

}


/****************************************************************************************************/
/*																									*/
/*		�X�L�����a�l�b�g���[�N�h�e�o�͏���															*/
/*																									*/
/****************************************************************************************************/
static void BpxBoutputNetIf( SERVO_CONTROL_IF *SvControlIf, SEQ_CTRL_OUT *SeqCtrlOut,
																		LONG apos, LONG PosErr )
{
	SV_CYC_RSP			*SvCycRsp;
	LONG				**MonTbl;

	SvCycRsp	= &( SvControlIf->CmdOptionIf.CycRsp );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�����f�[�^���M�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->CmdOptionIf.CycCtrl.RspUpdate == FALSE )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F���䃂�[�h����																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvCycRsp->RspCtrl.ExeMode = SvControlIf->CmdCtrl.ModSel;
	SvCycRsp->RspCtrl.RspCtrlBit =
		(USHORT)(SeqCtrlOut->RspCtrlBits | SvControlIf->RspCtrl.RspCtrlBit);

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�����r�b�g�M���o��																	*/
/*--------------------------------------------------------------------------------------------------*/
	SvCycRsp->RspSeqBit	= SeqCtrlOut->RspSeqBits;
	SvCycRsp->RspLtBit	= SvControlIf->RspLtBit;
//	SvCycRsp->RspSigBit	= SvControlIf->RspSigBit;
	SvCycRsp->RspSigBit	= SeqCtrlOut->RspSigBits;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F�������l�f�[�^�o��																	*/
/*--------------------------------------------------------------------------------------------------*/
	SvCycRsp->FbPosition = apos;
	SvCycRsp->PosError = PosErr;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�F���j�^�f�[�^�o��																		*/
/*--------------------------------------------------------------------------------------------------*/
	MonTbl = SvControlIf->MonRamTable;
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel1,
							&SvCycRsp->SelMon1, &SvCycRsp->Mon1 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel2,
							&SvCycRsp->SelMon2, &SvCycRsp->Mon2 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel3,
							&SvCycRsp->SelMon3, &SvCycRsp->Mon3 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel4,
							&SvCycRsp->SelMon4, &SvCycRsp->Mon4 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel5,
							&SvCycRsp->SelMon5, &SvCycRsp->Mon5 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel6,
							&SvCycRsp->SelMon6, &SvCycRsp->Mon6 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel7,
							&SvCycRsp->SelMon7, &SvCycRsp->Mon7 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel8,
							&SvCycRsp->SelMon8, &SvCycRsp->Mon8 );

}



/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Kernel Input Service for ScanB																*/
/*																									*/
/****************************************************************************************************//* <S04D>:�������� *//* <S086>*/
void BpiInputMotorPosition(BASE_CTRL_HNDL *BaseControls, ASIC_U_IF_STR *pAsicMicroReg,
											BPRMDAT *Bprm, PRMDATA *Prm, CTRL_LOOP_OUT *CtrlLoopOut)
{
	LONG				dMotPos, MotPos;
	LONG			dFencPos, FencPos;	/* <S04D> */
	MENCV 			*MencV, *FencV;		/* <S04D> */
	POS_MNG_HNDL		*PosManager;

	PosManager		= &(BaseControls->PosManager);
	MencV	= BaseControls->MencV;	/* <S04D> */
	FencV	= BaseControls->FencV;	/* <S04D> */
/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Motor Encoder Data for Servo Control												*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A�^ */
		BaseControls->MotSts.MotCphPass = LmxSencScanServiceB( MencV,
							 	 	 	 	 	 	 	 	&(BaseControls->CLatchClrReq) );
	}
	else
	{ /* ��]�^ */
		BaseControls->MotSts.MotCphPass = RmxSencScanServiceB( MencV,
															&(BaseControls->CLatchClrReq) );
	}
	BaseControls->MotSts.MposFromCpos = Bprm->DirSign * MencV->MposFromCpos;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�a�ʒu�X�V																				*/
/*--------------------------------------------------------------------------------------------------*/
//	MotPos = MencV->MotPos;									/* Load MotorPosition					*//* <S038> */
	MotPos = Bprm->DirSign * MencV->MotPos;					/* Load MotorPosition					*/
	dMotPos = MotPos - BaseControls->MotSts.MotPos;			/* Cal. Delta Position					*/
	BaseControls->MotSts.MotPos += dMotPos;					/* Update Motor Position				*/
	BaseControls->MotSts.dMotPos = dMotPos;					/* Update Delta Motor Position			*/

/*--------------------------------------------------------------------------------------------------*/
/*		<S0E5> �e�a���x�X�V (�V�[�P���X�����p)														*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->MotSts.MotSpdForSeq = MlibMulgain( dMotPos, Bprm->KmotspdB );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�e�a��������(Semi/Full�ؑւ�), ���[�^��Έʒu�X�V, �t���N��Έʒu�X�V					*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D> */
	if(Bprm->FencUse != FALSE)
	{
		/*------------------------------------------------------------------------------------------*/
		/*		�e�a�ʒu�X�V																		*/
		/*------------------------------------------------------------------------------------------*/
		BaseControls->MotSts.FencCphPass = FencV->MotCphPass;	/* Set FencCphPass					*//* <S220> */
		FencPos = Bprm->DirSign * FencV->MotPos;								/* Load MotorPosition					*/
		dFencPos = FencPos - BaseControls->MotSts.FencPos;		/* Cal. Delta Position					*/
		BaseControls->MotSts.FencPos += dFencPos;				/* Update Motor Position				*/
		BaseControls->MotSts.dFencPos = dFencPos;				/* Update Delta Motor Position			*/

		MlibAposRg64dp( dFencPos, &(PosManager->conf.Egear), &(PosManager->FencPos), NULL );
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
			MlibAposRg64dp( (dFencPos - PosManager->BlshCmp.var.BlshCmpAmountPls),		/* <S18B> */
							&(PosManager->conf.Egear),
							&(PosManager->PgPos),
							NULL );
#else
		PosManager->PgPos = PosManager->FencPos;										/* [�w�ߒP��]	*/
#endif/* (CSW_SVCF_BACKLASHCOMP == TRUE) */
		BaseControls->CtrlCmdMngr.dPosFbki = dFencPos;
		BaseControls->CtrlCmdMngr.PosFbki = BaseControls->MotSts.FencPos;
		BaseControls->CtrlCmdMngr.FbkPosFromCpos = BaseControls->MotSts.FposFromCpos;
	}
	else
#endif	/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT) */
	{
		if( MencV->ReCalcAbsPosReq != FALSE )			/* ��Έʒu�Čv�Z�v������H				*/
		{
			/* �G���R�[�_�����ʒu�擾	*/
			/* RxPos*[0] -> RmxReadSencPosition�Ŏ擾���� */
			/* SEN ON���̐�Έʒu�Čv�Z����	*/
			PosMngSenOnReCalcAbsoPos( MencV,
			                          &(PosManager->MencPos),
			                          &(PosManager->conf.Egear),
			                          Prm->MencP.bit_dp,
			                          Prm->MencP.limmlt,
									  Bprm->RvsDir);
			MencV->ReCalcAbsPosReq = FALSE;
			PosManager->PgPos = PosManager->MencPos;								/* [�w�ߒP��]	*/
		}
		else
		{
			MlibAposRg64dp( dMotPos, &(PosManager->conf.Egear), &(PosManager->MencPos), NULL );
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
			MlibAposRg64dp( (dMotPos - PosManager->BlshCmp.var.BlshCmpAmountPls),	/* <S18B> */
							&(PosManager->conf.Egear),
							&(PosManager->PgPos),
							NULL );
#else
			PosManager->PgPos = PosManager->MencPos;								/* [�w�ߒP��]	*/
#endif/* (CSW_SVCF_BACKLASHCOMP == TRUE) */
		}
		BaseControls->CtrlCmdMngr.dPosFbki = dMotPos;
		BaseControls->CtrlCmdMngr.PosFbki = BaseControls->MotSts.MotPos;
//		pBaseCtrl->CtrlCmdMngr.PosErr32s = CtrlLoopOut->MotPosErr;
		BaseControls->CtrlCmdMngr.FbkPosFromCpos = BaseControls->MotSts.MposFromCpos;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���x�e�a���Z																			*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D>:�ٸ�����ǉ� */
	if(Bprm->PsFulcSelect != FALSE)
	{
		BaseControls->CtrlCmdMngr.SpdFbki = FencV->MotSpd;
		BaseControls->CtrlCmdMngr.SpdObsFbki = FencV->MotSpd;
	}
	else
#endif/* (CSW_SVCF_BACKLASHCOMP == TRUE) */
	{
//		BaseControls->CtrlCmdMngr.SpdFbki = MencV->MotSpd;
//		BaseControls->CtrlCmdMngr.SpdObsFbki = MencV->MotSpd;
		BaseControls->CtrlCmdMngr.SpdFbki = CtrlLoopOut->MotSpd_a;
		BaseControls->CtrlCmdMngr.SpdObsFbki = CtrlLoopOut->MotSpd_a;/*<S086>*/
	}

}

/****************************************************************************************************/
/*																									*/
/*		Output Network monitor value																*/
/*																									*/
/****************************************************************************************************/
void BpiOutputNetStatus(BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                        	CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm, PLSIFV *pPls)/* <S04D> *//* <S121> */
{
//	LONG				dPcmda;
	LONG				MotSpd;
	LONG				VPosErrKp;
	USHORT				RspCtrlBits;
	POS_MNG_HNDL		*PosManager;

	PosManager		= &(BaseControls->PosManager);

	/* ���z�ʒu�΍��⏞�pFB���x�Z�o */
	if(Bprm->PsFulcSelect != FALSE)/* <S04D>:�ٸ�����ǉ� */
	{/* �t���N���[�Y�h����	*/
#if (FLOAT_USE==TRUE)
		MotSpd = (BaseControls->MotSts.FencSpd + BaseControls->CtrlCmdMngr.SpdFbki) / 2.0f;
#else
		MotSpd = (BaseControls->MotSts.FencSpd + BaseControls->CtrlCmdMngr.SpdFbki) >> 1;
#endif
		BaseControls->MotSts.FencSpd = BaseControls->CtrlCmdMngr.SpdFbki;
		BaseControls->MotSts.MotSpd = BaseControls->MencV->MotSpd;
	}
	else
	{/* �Z�~�N���[�Y�h����	*/
#if (FLOAT_USE==TRUE)
	MotSpd = (BaseControls->MotSts.MotSpd + BaseControls->CtrlCmdMngr.SpdFbki) / 2.0f;
#else
	MotSpd = (BaseControls->MotSts.MotSpd + BaseControls->CtrlCmdMngr.SpdFbki) >> 1;
#endif
	BaseControls->MotSts.MotSpd = BaseControls->CtrlCmdMngr.SpdFbki;
	}
//<S0E5>	BaseControls->MotSts.MotSpdSumB += BaseControls->MotSts.MotSpd;
	BaseControls->MotSts.MotSpdSumB += BaseControls->MotSts.MotSpdForSeq;	/* <S0E5> */
	BaseControls->MotSts.MotSpdCntB++;

/*--------------------------------------------------------------------------------------------------*/
/*		���z�ʒu�΍��X�V																	<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
	if(BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS)
	{ /* ���x/�g���N���䎞 */
		/* ���z�ʒu���[�v�Q�C���擾 */
		VPosErrKp = 1 ^ (SeqCtrlOut->OverTrvlSts & SeqCtrlOut->MotStop);
		VPosErrKp *= (BaseControls->GainChange.GselGains[0].VirtualPosKp);

		/* ���z�ʒu�΍��Z�o  */
#if (FLOAT_USE==TRUE)
		PosManager->VirtualPositionError = MotSpd * VPosErrKp;
#else
		PosManager->VirtualPositionError = MlibMulgain32( MotSpd, VPosErrKp );
#endif /* FLOAT_USE */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�X�e�[�^�X�X�V																				*/
/*--------------------------------------------------------------------------------------------------*/
	/*	�d�������t���O : �x�[�X�C�l�[�u�� && �ʃv���O�����d�������X�e�[�^�X	*/
//	RspCtrlBits = (SeqCtrlOut->BeComplete & CtrlLoopOut->TrqClamp) << TRQ_CLAMP_BITNO; /* <S04B> */
//	BaseControls->CtrlCmdMngr.TrqClamp = (SeqCtrlOut->BeComplete & CtrlLoopOut->TrqClamp);	/* <S045> Torque Limit *//* <S04B> */
	RspCtrlBits = BaseControls->CtrlCmdMngr.TrqClamp << TRQ_CLAMP_BITNO; /* <S04B> */
	/* Speed Clamp Status */
	//RspCtrlBits |= ( 0 != CtrlLoopOut->SpdClamp ) << SPD_CLAMP_BITNO;
	RspCtrlBits |= ( 0 != BaseControls->CtrlCmdMngr.SpdClamp ) << SPD_CLAMP_BITNO;/* @@ VLT status */
/*--------------------------------------------------------------------------------------------------*/
	RspCtrlBits |= ( 0 != BaseControls->PosCtrlSts.CoinSignal ) << COIN_BITNO;	/* @@ COIN status */
/*--------------------------------------------------------------------------------------------------*/
	RspCtrlBits |= ( 0 != BaseControls->PerClrEdge ) << PERCLREDGE_BITNO;		/* @@ CLREDGE status */

	/* @@ CLREDGE status (AP ONLY) */
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->RspCtrl.RspCtrlBit = RspCtrlBits;

#if !defined(CSW_NETIF_SYNQNET_TYPE)

#if	(CSW_SVCF_FBLATCH == TRUE)
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�e�a���b�`����																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Cphase, Ext1 Latch */
	PosMngLatchNetFbkPos( BaseControls, SvControlIf, Bprm );									/* <S04D> */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�l�b�g���[�N���j�^�f�[�^�X�V																*/
/*--------------------------------------------------------------------------------------------------*/
	BpxBoutputNetIf( SvControlIf,
					 SeqCtrlOut,
					 PosManager->PgPos.apos[0],
					 PosManager->var.PosErrA.per32s );
//					 pBaseCtrl->CtrlCmdMngr.PosErr32s );
#endif
}


/****************************************************************************************************/
/*																									*/
/*		Input ScanB Control command																	*/
/*																									*/
/****************************************************************************************************/
void BpiInputControlCMD( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                         CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, USHORT uwd_l, LONG ax_no )/* <S1C0>�F���ǉ� */
{
	LONG				GainChgTriger;
	TBLCHK_SV_STS		TblDriveChkSts;
	CTRLMD				CtrlMode;
//	POS_MNG_HNDL		*PosManager;	//<S0C7>
//	LONG				lwk0, lwk1;		//<S0C7>

//	PosManager			= &(BaseControls->PosManager);//<S0C7>


/*--------------------------------------------------------------------------------------------------*/
/*		�d��������Ԏ擾																			*/
/*--------------------------------------------------------------------------------------------------*/
	/*	�d�������t���O : �x�[�X�C�l�[�u�� && �ʃv���O�����d�������X�e�[�^�X	*/
	BaseControls->CtrlCmdMngr.TrqClamp = (SeqCtrlOut->BeComplete & CtrlLoopOut->TrqClamp);			  /* <S04B> */

#ifndef _IO_SERVO_ON_
/*--------------------------------------------------------------------------------------------------*/
/*		�l�b�g���[�N�w�߃f�[�^����																	*/
/*--------------------------------------------------------------------------------------------------*/
#if !defined(CSW_NETIF_SYNQNET_TYPE)
	BpxBinputNetIf( BaseControls, SvControlIf, &(BaseControls->TrqLimitData), ax_no );/* <S1C0>�F���ǉ� */
#endif
#endif /* _IO_SERVO_ON_ */

	/* �ʒu�ϕ��N���A�w�ߎ擾 */
	BaseControls->CtrlCmdMngr.PicvClrCmd =
			( SvControlIf->CmdCtrl.CmdCtrlBit >> CLRPOSINTG_BITNO ) & 1;

	/* ���[�h�X�C�b�`�p�����x�f�[�^�X�V */
	BaseControls->ModeSwData.var.Acceleration = SeqCtrlOut->ModeSwAcc;

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{���䃂�[�h�Ǘ�����																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���䃂�[�h�F�X�L�����b�w�� */
	CtrlMode.dw = SeqCtrlOut->CtrlModeReq.dw;

	/* �X�L�����b���䃂�[�h�w�߃`�F�b�N */
	if( SeqCtrlOut->CtrlModeReq.us.uwd_l == CTRLMODE_NOCMD )
	{
		CtrlMode.us.uwd_l = (USHORT)SvControlIf->NetCtrlMcmd;
	}
	else
	{
		SvControlIf->CmdReady = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* ���䃂�[�h�O��l */
	BaseControls->CtrlModeSet.CtrlModeLst.dw = BaseControls->CtrlModeSet.CtrlMode.dw;
	/* ���䃂�[�h�ύX��� */
	BaseControls->CtrlModeSet.CtrlModeChg.dw = CtrlMode.dw ^ BaseControls->CtrlModeSet.CtrlMode.dw;
	/* ���䃂�[�h����l */
	BaseControls->CtrlModeSet.CtrlMode.dw	 = CtrlMode.dw;

/*--------------------------------------------------------------------------------------------------*/
/*		Get Sequence Control Output																	*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->BBRefClrReq = SeqCtrlOut->BBSvcRefClrReq;
	BaseControls->BeComplete = SeqCtrlOut->BeComplete;
	BaseControls->OverTravel = SeqCtrlOut->OverTrvlSts;			/* <S18B> */

#ifdef _VC_DEBUG_ /* 2012.09.11 Y.Oka �������T�[�{�I�� �b��Ή��� */
	BaseControls->BeComplete = TRUE;
#endif /* 2012.09.11 Y.Oka �������T�[�{�I�� �b��Ή��� */
	

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�Q�C���ؑ֏���																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* �T�[�{�Q�C���ؑ֏��� */
	if( (BaseControls->GainChange.AtGsel == TRUE)
		&& (BaseControls->TuneLessCtrl.var.TuneLessAct == FALSE) 
		&& (SVFSEL_ATGAINCHANGE_USE == 1) )							/* <S09B> */
	{ /* �����Q�C���ؑ� && �������X������ */
		/* �؂�ւ����菈�� */
		GainChgTriger = GselDetGainChngTiming( BaseControls->PosCtrlSts.CoinSignal,
											   BaseControls->PosCtrlSts.NearSignal,
											   BaseControls->PosCtrlSts.RefZSignal,
											   BaseControls->GainChange.AtGselTrgA );
		/* �Q�C���؂�ւ����Z���� */
//<S09B>	GselAutoGainChange( &(BaseControls->GainChange),
//							&(BaseControls->MFControl),
//							GainChgTriger,
//							&(BaseControls->CtrlCmdPrm) );
			GselAutoGainChange( BaseControls, GainChgTriger );	/* <S09B> */
	}
	else
	{ /* �}�j���A���Q�C���؂�ւ�(�Q�C���؂�ւ��Ȃ�) */
		/* �}�j���A���Q�C���؂�ւ����� */
//<S09B>		GselManualGainChange( &(BaseControls->GainChange),
//							  &(BaseControls->MFControl),
//							  BaseControls->GselNum,
//							  BaseControls->TuneLessCtrl.var.TuneLessAct,
//							  &(BaseControls->CtrlCmdPrm) );
			GselManualGainChange( BaseControls );					/* <S09B> */
		/* �����Q�C���؂�ւ������� */
//<S09B>	GselInitAutoGainChange(
//							&(BaseControls->GainChange.AtGselA),
//							&(BaseControls->GainChange.AtGselB) );
			GselRstAutoGainChange( &(BaseControls->GainChange) );	/* <S09B> */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s����																		*/
/*--------------------------------------------------------------------------------------------------*/
//	TblDriveChkSts.AlmFlag = AxisRscB->AlmManager->AlmFlag;
	TblDriveChkSts.AlmFlag = (SeqCtrlOut->RspSeqBits >> ALM_DETECTED_BITNO) & 0x1;
	TblDriveChkSts.BeComplete = BaseControls->BeComplete;
	TblDriveChkSts.HwbbSts = SeqCtrlOut->HwbbSts;
	TblDriveChkSts.MainPowerOn = SeqCtrlOut->MainPowerOn;
	TblDriveChkSts.OverTrvlSts = SeqCtrlOut->OverTrvlSts;
//	TblDriveChkSts.TrqClamp = CtrlLoopOut->TrqClamp; /* <S04B> */
	TblDriveChkSts.TrqClamp = BaseControls->CtrlCmdMngr.TrqClamp; /* <S04B> */
	TblDriveChkSts.dMotPos = BaseControls->MotSts.dMotPos;
	TblDriveExecute( &(BaseControls->TblDrive),
					 &TblDriveChkSts,
					 &(BaseControls->PosManager),
					 &(BaseControls->MFControl) );

/*--------------------------------------------------------------------------------------------------*/
/*		�[�����䃂�[�h�w�ߏ����F�X�L����B���ʓ��͏�������̎w��										*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseControls->TblDrive.var.TblCmd == TBLCMD_NOCMD)
		|| (BaseControls->CtrlModeSet.CtrlMode.b.zm != CTRLMODE_ZNONE) )
	{ /* �e�[�u���^�]�Ȃ� || �X�L����B���ʓ��͏����w�߂��� */
		BaseControls->CtrlModeSet.ZctrlMode.zm    = BaseControls->CtrlModeSet.CtrlMode.b.zm;
		BaseControls->CtrlModeSet.ZctrlMode.zmchg = BaseControls->CtrlModeSet.CtrlModeChg.b.zm;
	}
	else
	{
		BaseControls->CtrlModeSet.ZctrlMode.zm    = BaseControls->TblDrive.var.TblZmd.b.zm;
		BaseControls->CtrlModeSet.ZctrlMode.zmchg = BaseControls->TblDrive.var.TblZmd.b.zmchg;
	}

	/* �g������o�͕ϐ��[���N���A */
	BaseControls->CtrlCmdMngr.SpdFFCx = 0;		/* �g������F���xFF�⏞				*/
	BaseControls->CtrlCmdMngr.SpdFBCx = 0;		/* �g������F���xFB�⏞				*/
	BaseControls->CtrlCmdMngr.TrqFFCx = 0;		/* �g������F�g���NFF�⏞			*/
	BaseControls->CtrlCmdMngr.TrqFBCx = 0;		/* �g������F�g���NFB�⏞			*/

}



/****************************************************************************************************/
/*																									*/
/*		����T�[�{���䏈�����C��																	*/
/*																									*/
/****************************************************************************************************/
void BpiYecServoControl( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
							CTRL_LOOP_OUT *CtrlLoopOut, FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm )/* <S04D> */
{
	BASE_CTRL_HNDL		*BaseControls;
	POS_MNG_HNDL		*PosManager;
	MFCTRL				*MFControl;
	LONG				SpdFFx;
	KSGAIN				lwk;		/*<S00A>*/
	CMD_CTRLBITS		*CmdCtrl;			/* <S1D0> */

	/* ���[�J���ϐ��̏����� */
	BaseControls= BaseLoops->BaseCtrls;
	PosManager	= &(BaseControls->PosManager);
	MFControl	= &(BaseControls->MFControl);
	CmdCtrl = &(SvControlIf->CmdOptionIf.CycCmd.CmdCtrl);		/* <S1D0> */

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���x�e�a���Z																			*/
/*--------------------------------------------------------------------------------------------------*/
//	pBaseCtrl->CtrlCmdMngr.SpdFbki		= pBaseCtrl->MotSts.MotSpd;
//	pBaseCtrl->CtrlCmdMngr.SpdObsFbki	= pBaseCtrl->MotSts.MotSpd;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߊǗ�																				*/
/*																									*/
/*		�T�[�{��{�����ScanA�ŏ�������B															*/
/*		�����ōX�V���ꂽ�ʒu�w�߂�ScanA�ŕ�Ԃ�����A�T�[�{��{����̓��͂ƂȂ�B					*/
/*		�ʒu�w�߂̍X�V��125us�̑O��62.5us�ȓ��ŕK���s�����ƁB										*/
/*		�ڍׂ͕ʓr�d�l�����Q�Ƃ���B�d�l���m���F��������������������								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/

	/* �g���N�w�߃t�B���^�ϕ��������v���N���A */
	BaseControls->CtrlCmdMngr.TrqFilClrCmd = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		���䃂�[�h�ɂ�镪��																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( BaseControls->CtrlModeSet.CtrlMode.b.cm )
	{
/****************************************************************************************************/
	case CTRLMODE_POS:	/* �ʒu���䃂�[�h															*/
/****************************************************************************************************/
	/*----------------------------------------------------------------------------------------------*/
	/*		����؂�ւ�������(���x���� �܂��� �g���N���� ===> �ʒu����)							*/
	/*----------------------------------------------------------------------------------------------*/
		switch(BaseControls->CtrlModeSet.CtrlModeLst.b.cm)
		{
		case CTRLMODE_TRQ:
			if( BaseControls->DobsCtrl.var.DobsAct != FALSE)
			{ /* �O���I�u�U�[�o�L�� -> �g���N�w�߃t�B���^�ϕ������� */
				BaseControls->CtrlCmdMngr.TrqFilClrCmd = TRUE;
			}

			/* �������X�v���ݒ� */
			if( BaseControls->TuneLessCtrl.var.TuneLessInvldReq == FALSE )
			{
				BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;
			}
			/* ���x�w��/���xFF�ʑ��i�ݕ⏞�t�B���^�������̓ʃv���O�����ɂĎ��{ */
			/* break�Ȃ��A���̂܂܉��� */

		case CTRLMODE_SPD:		/* �ʒu�΍��֘A�̌������̂��ߕύX <S14C> */
			/* ���x�w�߃t�B���^�o�͏����� */
			BaseControls->CtrlCmdMngr.SpdRfFilo = 0;

			/* ���䃂�[�h�؂�ւ��ɂ���ʒu�쐬 */
			lwk = MlibPcmdEgear( PosManager->VirtualPositionError,
								 &(PosManager->conf.Egear),
								 &(PosManager->var.dPcmdEgrem) );
#if 0/* SigamV-MD Micro�Ɋ�ʒu�����n����	*/								/* <S007> */
			BaseControls->CtrlCmdMngr.BaseFbkPos = lwk + BaseControls->CtrlCmdMngr.PosFbki;
#else/* Sigam7 ScanA�ɏ���ʒu�΍������n����B	*/							/* <S007> */
			PosManager->var.Per64[0] = 0;
			PosManager->var.Per64[1] = 0;
			MlibPerrcalx(lwk, 0, PosManager->var.Per64);
#endif
			MlibPerrcala( PosManager->VirtualPositionError, 0,
							&PosManager->conf.Egear, &PosManager->var.PosErrA );
			/* ���ݎw�߈ʒu������ */
			BaseControls->CtrlCmdMngr.PosRefLo = PosManager->VirtualPositionError + PosManager->PgPos.apos[0];

			/* �����m�b�`���g����͋������������ǉ��\�� */
			break;

		default:
			break;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		�ʒu�w�ߐ���																			*/
	/*----------------------------------------------------------------------------------------------*/
		/* �T�[�{�ʐM�ʒu�w�ߊǗ����� */
		PosManager->dPosRefi = PosMngNetPosCmdManager( BaseLoops, SvControlIf, FnCmnCtrl, Bprm );  /* <S04D> *//* <S187> */

	/*----------------------------------------------------------------------------------------------*/
	/*		���xFF�⏞/�g���NFF�⏞����																*/
	/*----------------------------------------------------------------------------------------------*/
		/* �ʒu�w��FF���Z --> pBaseCtrl->CtrlCmdMngr.SpdFFCx */
		if( MFControl->var.CurMFCModel == 0 )
		{ /* MFC���� -> MFC�L������FF�@�\�𖳌��ɂ��� */
			/* �ʒu�w��FF���Z */
#if (FLOAT_USE==TRUE)
			SpdFFx = (float)PosManager->dPosRefi * BaseControls->CtrlCmdPrm.KdPff;
#else
			SpdFFx = MlibMulgain( PosManager->dPosRefi, BaseControls->CtrlCmdPrm.KdPff );
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
			/* �ꎟ���[�p�X�t�B���^���Z */
			BaseControls->CtrlCmdMngr.dPFfFilo = FlibLpfilter1(SpdFFx,
														BaseControls->CtrlCmdPrm.KdPffFil,
														BaseControls->CtrlCmdMngr.dPFfFilo );
#else
			/* �ꎟ���[�p�X�t�B���^���Z */
			BaseControls->CtrlCmdMngr.dPFfFilo = MlibLpfilter1(SpdFFx,
														BaseControls->CtrlCmdPrm.KdPffFil,
														BaseControls->CtrlCmdMngr.dPFfFilo );
#endif /* FLOAT_USE */

			/* ���xFF�⏞ [2^24/OvrSpd] */
			BaseControls->CtrlCmdMngr.SpdFFCx += BaseControls->CtrlCmdMngr.dPFfFilo;
		}
		if( (BaseControls->TblDrive.var.TblCmd == TBLCMD_NOCMD)
			&& ((MFControl->var.CurMFCModel == 0) || (MFControl->conf.MFCFF == TRUE)) )
		{ /* �e�[�u���^�]�ȊO && (MFC���� || MFC���O��FF�L��) */
			/* ���x�t�B�[�h�t�H���[�h�w�߈ړ����σt�B���^�������C���֐�		*/			/* <S1D0> */
			SvControlIf->NetSpdFFC = FFcmdFilMoveAverageFilter( &(BaseControls->VFFcmdFil),
																SvControlIf->NetSpdFFC,
																CmdCtrl->vffFilSts);
			BaseControls->CtrlCmdMngr.SpdRfFilo = SvControlIf->NetSpdFFC;
			/* ���xFF�w�߉��Z */
			BaseControls->CtrlCmdMngr.SpdFFCx += BaseControls->CtrlCmdMngr.SpdRfFilo;
			/* 	�g���N�t�B�[�h�t�H���[�h�w�߈ړ����σt�B���^�������C���֐�		*/			/* <S1D0> */
			SvControlIf->NetTrqFFC = FFcmdFilMoveAverageFilter( &(BaseControls->TFFcmdFil),
																SvControlIf->NetTrqFFC,
																CmdCtrl->tffFilSts);
			/* �g���NFF�w�߉��Z */
			BaseControls->CtrlCmdMngr.TrqFFCx += SvControlIf->NetTrqFFC;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		Get Speed Reference for Monitor															*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.SpdRefi = CtrlLoopOut->SpdRefo;
		BaseControls->CtrlCmdMngr.SpdRefo = CtrlLoopOut->SpdRefo;

	/*------------------------------------------------------------------------------------------*/
	/*		�I�t���C���C�i�[�V������															*/
	/*------------------------------------------------------------------------------------------*/
		BpxJatOfflineCalc(	&BaseControls->JatHdl, 
							&BaseControls->PosManager.conf.Egear,
							&BaseControls->CtrlCmdPrm,
							&BaseControls->CtrlCmdMngr,
							BaseLoops->TrqRefo_a );									/* <S160> */
//<S0BA>
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���[�h�X�C�b�`���Z																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
//					(CtrlLoopOut->SpdRefo + CtrlLoopOut->SpdFFC),
					BaseControls->PosManager.PosErrA.per32s );
#endif
	/*----------------------------------------------------------------------------------------------*/
	/*		Get Speed Reference for Monitor															*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.SpdRefo = CtrlLoopOut->SpdRefo;
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		����������~�p���x�l�ۑ�																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */

		break;


/****************************************************************************************************/
	case CTRLMODE_SPD:	/* ���x���䃂�[�h															*/
/****************************************************************************************************/
	/*----------------------------------------------------------------------------------------------*/
	/*		����؂�ւ�������(�ʒu���� �܂��� �g���N���� ===> ���x����)							*/
	/*----------------------------------------------------------------------------------------------*/
		switch(BaseControls->CtrlModeSet.CtrlModeLst.b.cm)
		{
		case CTRLMODE_TRQ:
			if( BaseControls->DobsCtrl.var.DobsAct != FALSE)
			{ /* �O���I�u�U�[�o�L�� -> �g���N�w�߃t�B���^�ϕ������� */
				BaseControls->CtrlCmdMngr.TrqFilClrCmd = TRUE;
			}

			/* �������X�v���ݒ� */
			if( BaseControls->TuneLessCtrl.var.TuneLessInvldReq == FALSE )
			{
				BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;
			}

			/* ���䃂�[�h�؂�ւ����̃V���b�N�ጸ���� */
			lwk = SvControlIf->NetSpdRef - BaseControls->CtrlCmdMngr.SpdObsFbki;
#if (FLOAT_USE==TRUE)
			lwk = FlibIntegral( lwk,
								BaseControls->GainChange.GselGains[0].Kvi,
								BaseControls->CtrlCmdMngr.VelIvar64 );
#else
			lwk = MlibIntegral( lwk,
								BaseControls->GainChange.GselGains[0].Kvi,
								BaseControls->CtrlCmdMngr.VelIvar64 );
#endif /* FLOAT_USE */

			/* ���x�w��/���xFF�ʑ��i�ݕ⏞�t�B���^�������̓ʃv���O�����ɂĎ��{ */
			/* break�Ȃ��A���̂܂܉��� */

		case CTRLMODE_POS:
			/* ���x�w��filter�o�� = ���xFB, ���x�w��SFS�o�� = ���xFB */
			BaseControls->CtrlCmdMngr.SpdRfFilo = BaseControls->CtrlCmdMngr.SpdFbki;
			BaseControls->CtrlCmdMngr.SpdRefo = BaseControls->CtrlCmdMngr.SpdFbki;
			/* �����m�b�`���g����͋������������ǉ��\�� */
			break;

		default:
			break;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		���x�w�ߐ���																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
		{ /* �e�[�u���^�]�� */
			/* �e�[�u���^�]���x�w�ߐݒ� */
			BaseControls->CtrlCmdMngr.SpdRfFilo = 0;
			BaseControls->CtrlCmdMngr.SpdRefi = BaseControls->TblDrive.var.SpdRef;
			BaseControls->VrefInSts = FALSE; /* <S0A1> */
		}
		else
		{ /* �ʏ�^�]�� */
			/* �w�ߓ��͏�Ԑݒ� */
			BaseControls->VrefInSts = (SvControlIf->NetSpdRef != 0) ? TRUE : FALSE; /* <S0A1> */

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) /* ���b�� ���ʏ����Ƃ��邱�Ɓ� */
			/* �ꎟ���[�p�X�t�B���^���Z */
			BaseControls->CtrlCmdMngr.SpdRfFilo = MlibLpfilter1(
													SvControlIf->NetSpdRef,
													BaseControls->CtrlCmdPrm.KVrfFil,
													BaseControls->CtrlCmdMngr.SpdRfFilo );
#else /* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */
			/* �ꎟ���[�p�X�t�B���^�͒ʐM���Ŋ|���� */
			BaseControls->CtrlCmdMngr.SpdRfFilo = SvControlIf->NetSpdRef;

#endif /* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */

			/* ���x�w�ߎ擾 */
			BaseControls->CtrlCmdMngr.SpdRefi = BaseControls->CtrlCmdMngr.SpdRfFilo;
			/* 	�g���N�t�B�[�h�t�H���[�h�w�߈ړ����σt�B���^�������C���֐�		*/			/* <S1D0> */
			SvControlIf->NetTrqFFC = FFcmdFilMoveAverageFilter( &(BaseControls->TFFcmdFil),
																SvControlIf->NetTrqFFC,
																CmdCtrl->tffFilSts);
			/* �g���NFF�⏞���� */
			BaseControls->CtrlCmdMngr.TrqFFCx += SvControlIf->NetTrqFFC;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		�\�t�g�X�^�[�g����																		*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.SpdRefo = BaseSoftStartSpdRef(
												&(BaseControls->CtrlCmdPrm),
												BaseControls->CtrlCmdMngr.SpdRefi,
												BaseControls->CtrlCmdMngr.SpdRefo );
//<S0BA>
/* <S1F5> Start*/
	/*----------------------------------------------------------------------------------------------*/
	/*		����������~�p���x�l�ۑ�																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���[�h�X�C�b�`���Z																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
					BaseControls->PosManager.PosErrA.per32s );
#endif
		break;


/****************************************************************************************************/
	case CTRLMODE_TRQ:	/* �g���N���䃂�[�h  (�[������~�^�[���N�����v�p�ɑ��x��������s)			*/
/****************************************************************************************************/
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���N�w�ߐ���																			*/
	/*----------------------------------------------------------------------------------------------*/
		/* �w�ߓ��͏�Ԑݒ� */
		BaseControls->TrefInSts = (SvControlIf->NetTrqRef != 0) ? TRUE : FALSE; /* <S0A1> */
		/* �g���N�w�ߎ擾 */
		BaseControls->CtrlCmdMngr.TrqRefo = SvControlIf->NetTrqRef;
		/* ���[�p�X�t�B���^���� */
		BaseControls->CtrlCmdMngr.SpdRfFilo = MlibLpfilter1(
												SvControlIf->NetSpdLmt,
												BaseControls->CtrlCmdPrm.KVrfFil,
												BaseControls->CtrlCmdMngr.SpdRfFilo );
		/* �g���N���䎞�������XOFF�ݒ� */
		BaseControls->TuneLessCtrl.var.TuneLessAct = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*		�g���N���䎞�̑��x�������Z																*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.OverTrq = OverTorqueCalculation(
												&(BaseControls->CtrlCmdMngr),
												&(BaseControls->CtrlCmdPrm) );
		break;


/****************************************************************************************************/
	case CTRLMODE_JOG:	/* �i�n�f�^�]���[�h															*/
/****************************************************************************************************/
		SvControlIf->NetSpdRef = FnCmnCtrl->JogSpeed;
		BaseControls->CtrlCmdMngr.SpdRefi = SvControlIf->NetSpdRef;

		/* �������X�@�\�X�C�b�`�����ɖ߂� <S0DA> */
		BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;

		/* �\�t�g�X�^�[�g���� */
		BaseControls->CtrlCmdMngr.SpdRefo = BaseSoftStartSpdRef(
												&(BaseControls->CtrlCmdPrm),
												BaseControls->CtrlCmdMngr.SpdRefi,
												BaseControls->CtrlCmdMngr.SpdRefo );
//<S0BA>
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		����������~�p���x�l�ۑ�																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���[�h�X�C�b�`���Z																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
					BaseControls->PosManager.PosErrA.per32s );
#endif
		break;


/****************************************************************************************************/
	case CTRLMODE_ZSRCH:/* ���_�T�[�`																*/
/****************************************************************************************************/
		/*	�ϐ������� : ���䃂�[�h�ύX���� */
		if( (0 != BaseControls->CtrlModeSet.CtrlModeChg.b.cm)
			|| (FALSE == BaseControls->BeComplete) )
		{
			BaseControls->ZeroClampCtrl.var.PosError = 0;
			BaseControls->ZeroClampCtrl.var.OrgSrchFinish = FALSE;
		}

		/* �������X�@�\�X�C�b�`�����ɖ߂� <S0DA> */
		BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;

		BaseControls->CtrlCmdMngr.SpdRefi = ZeroSearchControl(
											&(BaseControls->ZeroClampCtrl),
											&(BaseControls->MotSts),
											BaseControls->GainChange.GselGains[0].ZcKp,
											FnCmnCtrl->JogSpeed );
		BaseControls->CtrlCmdMngr.SpdRefo = BaseControls->CtrlCmdMngr.SpdRefi;

//<S0BA>
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		����������~�p���x�l�ۑ�																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���[�h�X�C�b�`���Z																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
					BaseControls->PosManager.PosErrA.per32s );
#endif
		break;


/****************************************************************************************************/
	default:			/* �{�P�[�X�͖���															*/
/****************************************************************************************************/
		break;
	}

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*		Update Speed Clamp Status																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != hTrqCtrl->var.SpdClamp )
	{
		SvControlIf->RspCtrl.RspCtrlBit |= SPD_CLAMP_BITMSK;
	}
	else
	{
		SvControlIf->RspCtrl.RspCtrlBit &= ~(SPD_CLAMP_BITMSK);
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߍX�V																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʒu�w�ߍX�V���� */
#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
	BaseControls->CtrlCmdMngr.dPosRefo = dPosRefo;
#else /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
	PosRefRenewal( PosManager, BaseControls->CtrlModeSet.CtrlMode );	/* <S187> */
#endif /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */

	BaseControls->CtrlCmdMngr.PosRefLo = SvControlIf->NetPosRef;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���䃂�[�h�ȊO�y�у[������~���[�h�̎��̈ʒu����ϐ�������								*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS) ||
		(BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_ZSTOP) ||  /* <S1F5> */
		(BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_LDSTOP) )   /* <S1F5> */
	{
		/* �ʒu����ϐ������� <S14C> */
		InitBasePositionControl( &(BaseLoops->PosCtrl) );
	}
/* <S1F5> Start */

	/*----------------------------------------------------------------------------------------------*/
	/*		����������~���x�w�߉��Z																*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_LDSTOP )
	{
		BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseLinDecStopSpdRef( &(BaseControls->CtrlCmdPrm),
															BaseControls->CtrlCmdMngr.SpdLdstpRefo );
	}
/* <S1F5> End */

	BaseControls->CtrlCmdMngr.CtrlMode = BaseControls->CtrlModeSet.CtrlMode.b.cm;

}


/* <S1F5> Start */
/*****************************************************************************
  Description: ���������x�ɂ�錸����~���x�w�߉��Z
------------------------------------------------------------------------------
  Parameters:
	SpdRefo;			���x����F���x�w�߂r�e�r�o��
	SpdLinZeroStopDec;	����������~������
  Return:
	SpdRefx				���x�w��
------------------------------------------------------------------------------
  Note:
    ���x�w�߃\�t�g�X�^�[�g���Z���C���֐�BaseSoftStartSpdRef���Q��
*****************************************************************************/
static LONG	BaseLinDecStopSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRefo )
{
	if( SpdRefo > 0 )
	{
		SpdRefo = SpdRefo - CtrlCmdPrm->SpdLdstpDec;
		if( SpdRefo < 0 )
		{
			SpdRefo = 0;
		}
	}
	else if( SpdRefo < 0 )
	{
		SpdRefo = SpdRefo + CtrlCmdPrm->SpdLdstpDec;
		if( SpdRefo > 0 )
		{
			SpdRefo = 0;
		}
	}
	else
	{
		SpdRefo = 0;
	}
	
	return( SpdRefo );
}
/* <S1F5> End */


/****************************************************************************************************/
/*																									*/
/*		�ʒu����ϐ�������																	<S14C>	*/
/*																									*/
/****************************************************************************************************/
void InitBasePositionControl( POSCTRL *PosCtrl )
{

	MlibResetLongMemory( &(PosCtrl->V), sizeof(PosCtrl->V)/4 );

}

#if	0
#define NRFMAX	16014615
/****************************************************************************************************/
/*																									*/
/*		�ʒu���䉉�Z	���g�p																		*/
/*																									*/
/****************************************************************************************************/
LONG BpiBasePositionControl( POS_LOOP_CTRL *PosLoop, CTRL_CMD_MNG *CtrlCmd,
							 POS_MNG_HNDL *PosManager, KSGAIN Kp, KSGAIN Kpi )
{
	LONG	PosErr;
	LONG	PacOut, IacOut;
	LONG	SpdRefx;

	if(CtrlCmd->CtrlMode != CTRLMODE_POS)
	{
		PosLoop->var.PerIvar64[0] = 0;
		PosLoop->var.PerIvar64[1] = 0;
		PosLoop->var.Per64[0] = 0;
		PosLoop->var.Per64[1] = 0;
//		CtrlCmd->PosErr32s = 0;
		PosManager->PosErrA.per32s = 0;
		PosLoop->var.LoopStart = FALSE;
		return( CtrlCmd->SpdRefo );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��N���A����																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( CtrlCmd->PerrClrReq != FALSE )
	{
		PosLoop->var.PerIvar64[0] = 0;
		PosLoop->var.PerIvar64[1] = 0;
		PosLoop->var.Per64[0] = 0;
		PosLoop->var.Per64[1] = 0;
		CtrlCmd->SpdRefo = 0;
		CtrlCmd->PosErr32s = 0;
		PosManager->PosErrA.per32s = 0;
		return( 0 );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����ϕ��N���A����																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( CtrlCmd->PicvClrCmd != FALSE )
	{
		PosLoop->var.PerIvar64[0] = 0;
		PosLoop->var.PerIvar64[1] = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�ʒu�΍��v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosLoop->var.LoopStart == FALSE )
	{
		PosLoop->var.LoopStart = TRUE;
#if (FLOAT_USE==TRUE)
		PosErr = FlibPerrcalx( CtrlCmd->BaseFbkPos + CtrlCmd->dPosRefo,
		                       CtrlCmd->PosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda);
	}
	else
	{
		PosErr = FlibPerrcalx( CtrlCmd->dPosRefo, CtrlCmd->dPosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda - PosManager->PgPos.dposix);
	}
#else
		PosErr = MlibPerrcalx( CtrlCmd->BaseFbkPos + CtrlCmd->dPosRefo,
		                       CtrlCmd->PosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda);
	}
	else
	{
		PosErr = MlibPerrcalx( CtrlCmd->dPosRefo, CtrlCmd->dPosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda - PosManager->PgPos.dposix);
	}
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F��ᐧ�䉉�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	PacOut = ( (float)PosErr * Kp );
#else
	PacOut = MlibMulgain( PosErr, Kp );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�ϕ����䉉�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( Kpi == 0 )
	{
		IacOut = ((PosLoop->var.PerIvar64[1] + 1) >> 1);
	}
	else
	{
		IacOut = MlibIntegral( PacOut, Kpi, PosLoop->var.PerIvar64 );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�o�h����o��																	*/
/*--------------------------------------------------------------------------------------------------*/
	SpdRefx = PacOut + IacOut;

/*--------------------------------------------------------------------------------------------------*/
/*		���x�o�C�A�X���Z���Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( SpdRefx == 0 )
	{
		if( PosErr > 0 )
		{
			SpdRefx =  1;
		}
		else if( PosErr < 0 )
		{
			SpdRefx = -1;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����o�̓��~�b�g																		*/
/*--------------------------------------------------------------------------------------------------*/
	else if( SpdRefx > NRFMAX )
	{
		SpdRefx =  NRFMAX;
	}
	else if( SpdRefx < -NRFMAX )
	{
		SpdRefx = -NRFMAX;
	}

	CtrlCmd->SpdRefo = SpdRefx;
	CtrlCmd->PosErr32s = PosErr;

	return SpdRefx;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o���䎞�ʒu�w�ߊǗ�����																*/
/*																									*/
/****************************************************************************************************/
void	BpiMpoleFindMakePosCmd( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
													FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm)
{
	BASE_CTRL_HNDL *BaseControls;
	LONG	dPosRefo;
	LONG	lwk;

	/* ���[�J���ϐ��̏����� */
	BaseControls = BaseLoops->BaseCtrls;	
	dPosRefo = 0;

	/* ���x�e�a���Z -> ���[�^�������x�Œ� */
//	pBaseCtrl->CtrlCmdMngr.SpdFbki = pBaseCtrl->MotSts.MotSpd;

	if( BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
	{
		if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
		{
			/* ���䃂�[�h�؂�ւ��ɂ���ʒu�쐬 */
			/* �ʒu�΍��֘A�̌������̂��ߕύX <S14C> */
			lwk = MlibPcmdEgear( BaseControls->PosManager.VirtualPositionError,
								 &(BaseControls->PosManager.conf.Egear),
								 &(BaseControls->PosManager.var.dPcmdEgrem) );
#if 0/* SigamV-MD Micro�Ɋ�ʒu�����n����	*/								/* <S02F> */
			BaseControls->CtrlCmdMngr.BaseFbkPos = lwk + BaseControls->MotSts.MotPos;
#else/* Sigam7 ScanA�ɏ���ʒu�΍������n����B	*/							/* <S02F> */
			BaseControls->PosManager.var.Per64[0] = 0;
			BaseControls->PosManager.var.Per64[1] = 0;
			MlibPerrcalx(lwk, 0, BaseControls->PosManager.var.Per64);
#endif
			MlibPerrcala( BaseControls->PosManager.VirtualPositionError, 0,
							&BaseControls->PosManager.conf.Egear, &BaseControls->PosManager.var.PosErrA );
			/* ���ݎw�߈ʒu������ */
			BaseControls->CtrlCmdMngr.PosRefLo = BaseControls->PosManager.VirtualPositionError
											+ BaseControls->PosManager.PgPos.apos[0];
		}
		else
		{
			BaseControls->CtrlCmdMngr.BaseFbkPos += BaseControls->CtrlCmdMngr.dPosRefo;
		}

		/* �T�[�{�ʐM�ʒu�w�ߊǗ�(dPosRefo,etc) */
		dPosRefo = PosMngNetPosCmdManager( BaseLoops, SvControlIf, FnCmnCtrl, Bprm );/* <S04D> */
	}
	BaseControls->CtrlCmdMngr.dPosRefo = dPosRefo;
	BaseControls->CtrlCmdMngr.PosRefLo = SvControlIf->NetPosRef;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���䃂�[�h�ȊO�̎��̈ʒu����ϐ�������													*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS )
	{
		/* �ʒu�Ǘ��ϐ����������� */
		PosMngInitPositionManager( BaseControls );
	}
}

//<S0BA>
#if 0
/****************************************************************************************************/
/*																									*/
/*		���[�h�X�C�b�`���Z																			*/
/*																									*/
/****************************************************************************************************/
static void ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err )
{
	LONG	ModeSWFlag;
	LONG	wk;

	/* ���[�J���ϐ��̏����� */
	ModeSWFlag = FALSE;
	wk = 0;

	switch( ModeSwData->conf.ModeSWSel )
	{
	case MODESW_TRQREF: /* ���[�h�X�C�b�` �g���N�w�� */
		wk = trq_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWTrqLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_SPDREF: /* ���[�h�X�C�b�` ���x�w�� */
		wk = spd_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWSpdLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_MOTACC: /* ���[�h�X�C�b�` �����x */
		wk = ModeSwData->var.Acceleration;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWAccLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_POSERR: /* ���[�h�X�C�b�` �΍��p���X */
		wk = pos_err;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWErrLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_NOTUSE: /* ���[�h�X�C�b�` �s�g�p */
	default:
		break;
	}
	ModeSwData->var.ModeSWFlag = ModeSWFlag;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		���x�w�߃\�t�g�X�^�[�g���Z																	*/
/*																									*/
/****************************************************************************************************/
static LONG BaseSoftStartSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRef, LONG SpdRefx )
{
	LONG	LastSpdRefx;

	LastSpdRefx = SpdRefx;

/*--------------------------------------------------------------------------------------------------*/
/*		�\�t�g�X�^�[�g���Z																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( SpdRef > SpdRefx )
	{
		if( SpdRefx >= 0 )
		{
			SpdRefx = SpdRefx + CtrlCmdPrm->SpdSfsAcc;
			if( SpdRefx > SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
		else
		{
			SpdRefx = SpdRefx + CtrlCmdPrm->SpdSfsDec;
			if( SpdRefx > SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( SpdRef < SpdRefx )
	{
		if( SpdRefx <= 0 )
		{
			SpdRefx = SpdRefx - CtrlCmdPrm->SpdSfsAcc;
			if( SpdRefx < SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
		else
		{
			SpdRefx = SpdRefx - CtrlCmdPrm->SpdSfsDec;
			if( SpdRefx < SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�[�����o�R���� : ���ŕK�v���ǂ�������Ȃ����A��芸�����A�r�f�c�r�ɍ��킹��					*/
/*--------------------------------------------------------------------------------------------------*/
//	if( (SpdRef != SpdRefx) && (CtrlCmdPrm->SpdSfsAcc != CtrlCmdPrm->SpdSfsDec) )/* SGDV�̕s������W�J */
	if(CtrlCmdPrm->SpdSfsAcc != CtrlCmdPrm->SpdSfsDec)
	{
		if( (LastSpdRefx != 0) && ((LastSpdRefx ^ SpdRefx) < 0) )
		{
			SpdRefx = 0;
		}
	}
	return( SpdRefx );
}



/****************************************************************************************************/
/*																									*/
/*		���_�T�[�`���䉉�Z����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �U�Orpm�̑��x�w�߂Ō��_���T�[�`����B													*/
/*			�X�L�����a�ł͌��_(�b��)�̌��o�ƃ[���N�����v�w�߂��s�Ȃ��B								*/
/*			���E���h�ŁA�L�[����̃V�[�P���X�������s���B											*/
/*																									*/
/*	�� �� : ���[�^�ʒu����ł���A�t���N���[�Y�h����̓T�|�[�g���Ă��Ȃ��̂Œ��ӂ��邱�ƁB			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG ZeroSearchControl( ZX_CTRL_HNDL *ZeroClampCtrl, BASE_MOT_STS *MotSts, KSGAIN ZcKp, LONG JogSpeed )
{
	LONG	spd_ref;

/*--------------------------------------------------------------------------------------------------*/
/*		�i�n�f���x�^�]�F���_�����o����܂�															*/
/*--------------------------------------------------------------------------------------------------*/
	if( !(MotSts->MotCphPass || ZeroClampCtrl->var.OrgSrchFinish) )	/* C���ʉ߃`�F�b�N				*/
	{
		ZeroClampCtrl->var.PosError = 0;
		spd_ref = JogSpeed;											/* ���_�T�[�`���x�w��			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�[���N�����v����F���_�T�[�`����															*/
/*--------------------------------------------------------------------------------------------------*/
	else															/* C���ʉߌ��o��				*/
	{
		if( ZeroClampCtrl->var.OrgSrchFinish == FALSE )
		{ /* ���ꃂ�[�^�̌��_�o���c�L�΍��p */
			/* �ʒu�΍����b�`(����) */
			ZeroClampCtrl->var.PosError = -MotSts->MposFromCpos;
		}
		else
		{ /* �Q��ڈȍ~�̓��[�^�ʒu�������g�p���� */
			/* �ʒu�΍����Z(�Q��ڈȍ~) */
			ZeroClampCtrl->var.PosError -= MotSts->dMotPos;
		}
	/*----------------------------------------------------------------------------------------------*/
		/* ���_�T�[�`����Flag */
		ZeroClampCtrl->var.OrgSrchFinish = TRUE;
		/* �ʒu���䉉�Z(�o����) */
#if (FLOAT_USE==TRUE)
		spd_ref = (LONG)((float)ZeroClampCtrl->var.PosError * ZcKp);
#else
		spd_ref = MlibMulgain( ZeroClampCtrl->var.PosError, ZcKp );
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
		if( spd_ref == 0 )											/* ���x�w�߃[�����̕␳			*/
		{
			if( ZeroClampCtrl->var.PosError > 0 )
			{
				spd_ref =  1;
			}
			else if( ZeroClampCtrl->var.PosError < 0 )
			{
				spd_ref = -1;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		else if( spd_ref > ZeroClampCtrl->conf.SpdLimit )			/* ���x�w�ߏ�����~�b�g			*/
		{
			spd_ref =  ZeroClampCtrl->conf.SpdLimit;
		}
	/*----------------------------------------------------------------------------------------------*/
		else if( spd_ref < -ZeroClampCtrl->conf.SpdLimit )			/* ���x�w�߉������~�b�g			*/
		{
			spd_ref = -ZeroClampCtrl->conf.SpdLimit;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return( spd_ref );

}



/****************************************************************************************************/
/*																									*/
/*		�g���N���䎞�̑��x�����p�g���N�i�]��g���N�j���Z											*/
/*																									*/
/****************************************************************************************************/
#if 0
/* Mercury(SGDV-MD)�̏��� (���x�����������Ȃ�) */
static LONG OverTorqueCalculation( LONG SpdRfFilo, LONG TConSpdLmtLevel, BOOL VrefSpdLmt )
{
	LONG	vref_limit;

	/*----------------------------------------------------------------------------------------------*/
	/*		���x�����l TrqCtrl.V.SpdLimit �I��														*/
	/*----------------------------------------------------------------------------------------------*/
	if( VrefSpdLmt )							/* V-REF���O�����x�������͂Ƃ��Ďg�p����ꍇ		*/
	{
		vref_limit = MlibLABS( SpdRfFilo );

		/* �O��������̏��������x�Ő��� */
		if( vref_limit > TConSpdLmtLevel )
		{
			vref_limit = TConSpdLmtLevel;
		}
	}
	else
	{
		/* Pn407�ɂ��������x���� */
		vref_limit = TConSpdLmtLevel;
	}

	return vref_limit;
}

#else

/* SGDV�̏��� */
/****************************************************************************************************/
/*																									*/
/*		�g���N���䎞�̑��x�����p�g���N�i�]��g���N�j���Z											*/
/*			�Ӗ��̂킩��Ȃ��_������̂ŁA�ϐ��u�������ȊO��SGDS�\�t�g�̂܂܂ɂ��Ă���				*/
/*																	2003.04.16	S.Takamatsu			*/
/****************************************************************************************************/
#define	SPDLMTSW	0x40000					/* OS���x��1/64											*/
#define	INTDECVAL	(2*SPDLMTSW)			/* OS���x��1/32(SPDLMTSW���傫���Ȃ���΂����Ȃ�)		*/
#define	OVERTRQLMT	19573419				/* �]��g���N�̏���l(=2^24*17500/15000)				*/
LONG OverTorqueCalculation( CTRL_CMD_MNG *CtrlCmdMng, CTRL_CMD_PRM *CtrlCmdPrm )
{
LONG	SpdLimitx;
LONG	nerp;
LONG	neri;
LONG	nerpi;
LONG	lwrk;
LONG	overtorque;
LONG	Status;
LONG	SpdFbk = CtrlCmdMng->SpdFbki;
LONG	TrqRefi = CtrlCmdMng->TrqRefo;
	/*----------------------------------------------------------------------------------------------*/
	/*		���x�����l TrqCtrl.V.SpdLimit �I��														*/
	/*----------------------------------------------------------------------------------------------*/
		if( CtrlCmdPrm->VrefSpdLmt )			/* V-REF���O�����x�������͂Ƃ��Ďg�p����ꍇ		*/
		{
			SpdLimitx = MlibAbs32( CtrlCmdMng->SpdRfFilo );	/* �O��������̏��������x�Ő���			*/
			if( SpdLimitx < CtrlCmdPrm->TConSpdLmtLevel )	
			{ 
				CtrlCmdMng->SpdLimit = SpdLimitx; 
			}
			else
			{ 
				CtrlCmdMng->SpdLimit = CtrlCmdPrm->TConSpdLmtLevel; 
			}
		}
		else
		{
			CtrlCmdMng->SpdLimit = CtrlCmdPrm->TConSpdLmtLevel;	/* Pn407�ɂ��������x���� 					*/
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		���x�e�a�Ɛ������x�Ƃ̑��x�΍��쐬														*/
	/*----------------------------------------------------------------------------------------------*/
		if ( SpdFbk > 0 )
		{
			nerp = SpdFbk - CtrlCmdMng->SpdLimit;
		}
		else if ( SpdFbk < 0 )
		{
			nerp = SpdFbk + CtrlCmdMng->SpdLimit;
		}
		else
		{
			nerp = ( TrqRefi >= 0 ) ?	-CtrlCmdMng->SpdLimit : CtrlCmdMng->SpdLimit;
		}

		/* ���x�΍���ፀ */
		lwrk = MlibMulgain29( nerp, CtrlCmdPrm->OverTrqLoopGain[0] );
		
		/* ���x�΍��ϕ��� */
		neri = MlibIntegral( nerp, CtrlCmdPrm->OverTrqIntegGain[0], CtrlCmdPrm->OverTrqIntegBuf );

		nerpi = lwrk + neri;	/* 2^29+2^24�F�I�[�o�[�t���[�͂��Ȃ� */

		/*  ���x�΍�����v�Z�����ϕ��l�����ۂ̃��[�^��]�����ƈႤ�ꍇ�̏��� */
		if ( (((SpdFbk ^ nerpi) < 0) && (TrqRefi >= 0)) || ((((SpdFbk - 1) ^ nerpi) < 0) && (TrqRefi < 0)) )
		{
			/* ���x�������Ⴂ�ꍇ�̏��� */
			if ( CtrlCmdMng->SpdLimit <= SPDLMTSW )
			{
				/* ���X�Ɍ��� */
				if ( neri >= 0 )
				{
					lwrk = -INTDECVAL;
					neri = MlibIntegral( lwrk, CtrlCmdPrm->OverTrqIntegGain[0], CtrlCmdPrm->OverTrqIntegBuf );

					/* �ϕ����܂肪0�ɂȂ�܂ŁA���x���� */
					Status = ( neri >= 0 ) ? TRUE : FALSE;
				}
				else
				{
					lwrk = INTDECVAL;
					neri = MlibIntegral( lwrk, CtrlCmdPrm->OverTrqIntegGain[0], CtrlCmdPrm->OverTrqIntegBuf );

					/* �ϕ����܂肪0�ɂȂ�܂ŁA���x���� */
					Status = ( neri < 0 ) ? TRUE : FALSE;
				}
			}
			else
			{
				Status = FALSE;
			}
		}
		else
		{
			Status = TRUE;
		}

		if ( Status )
		{
			CtrlCmdMng->SpdClamp = TRUE;
		}
		else
		{
			CtrlCmdPrm->OverTrqIntegBuf[ 1 ] = 0;
			CtrlCmdPrm->OverTrqIntegBuf[ 0 ] = 0;
			nerpi = 0;
			CtrlCmdMng->SpdClamp = FALSE;
		}

		/* �t�����ő�g���N�������Ă������ł���ł��낤�傫�� */
		overtorque = MlibLimitul( nerpi, OVERTRQLMT, -OVERTRQLMT );

		return overtorque;

}
#endif


/****************************************************************************************************/
/*																									*/
/*		�s���d�����̃g���N���~�b�g���Z����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �s���d�����[�j���O�������A�g���N����������ꍇ ���� �I�[�o�g���x���łȂ��ꍇ			*/
/*				���[�j���O������	�FPn424�ɏ]�����g���N����										*/
/*				���[�j���O������	�FPn425�ɏ]�����g���N��������									*/
/*				���[�j���O������	�FMAX�g���N�ɂ�鐧��											*/
/*					��Pn425���Ԍo�ߌ�																*/
/*			�������Ȃ��ꍇ			�FMAX�g���N�ɂ�鐧��											*/
/*																									*/
/****************************************************************************************************/
static LONG LpxUvTrqLmtControl( UV_TLIM_DATA *UvTrqLimits, BOOL OtStatus, ALMMAN_HANDLE *AlmMngr ) /* <S145> */
{
	if ( (UvTrqLimits->conf.UvTrqLmtCtrl != FALSE) && (OtStatus == FALSE) )
	{
		if ( FALSE != ALMCheckEachState( AlmMngr, WRN_UV ) ) /* <S145> */
		{
			UvTrqLimits->var.UvTrqLmt = UvTrqLimits->conf.UvTLimSetting;
			UvTrqLimits->var.UvTrqLmtWait = UvTrqLimits->conf.UvTrqLmtOffFil;
		}
		else
		{
			if ( UvTrqLimits->var.UvTrqLmtWait == 0 )
			{
				UvTrqLimits->var.UvTrqLmt = NORMAXVALUE;
			}
			else
			{
				UvTrqLimits->var.UvTrqLmtWait--;
				UvTrqLimits->var.UvTrqLmt += UvTrqLimits->conf.DeltaUvTrqLmt;
			}
		}
	}
	else
	{
		UvTrqLimits->var.UvTrqLmt = NORMAXVALUE;
		UvTrqLimits->var.UvTrqLmtWait = 0;
	}
	return UvTrqLimits->var.UvTrqLmt;
}


/****************************************************************************************************/
/*																									*/
/*		�g���N���~�b�g���Z����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �p�����[�^�Őݒ肳�������g���N����(Pn402,Pn403), �O���g���N����(Pn404,Pn405)			*/
/*			����уg���N�w�߂`�c����A���L�Ɏ��������ɂăg���N�����l�����Z����B					*/
/*			�Ȃ��A�n�s��(�n�s�V�[�P���X����̐M��)�́A����~�g���N(Pn406)�ɂĐ�������B			*/
/*																									*/
/*																									*/
/*		���]���g���N�������Z�u���b�N�} (�t�]�������l)												*/
/*																									*/
/*		Pn406 : EstopTrqLmt -----||-------------------------------+-------> BoutV.FwTrqLmt			*/
/*							    OT-TrqLmt   			 		  | 								*/
/*													   			  | 								*/
/*												+-------+   	  | 								*/
/*		Pn402 : FwIntTrqLmt ------------------->|     	|   	  | 								*/
/*												|   	|   	  | 								*/
/*												|   	|   	  | 								*/
/*		Pn404 : FwExtTrqLmt -----||------------>|   	|   	  | 								*/
/*							    P-CL			|  Min  +---|/|---+ 								*/
/*												|   	|  OT-TrqLmt								*/
/*				TrqRefAd    -----||------------>|   	|   										*/
/*							  Pn002.0=1  		|   	|   										*/
/*												|   	|   										*/
/*				TrqRefAd    -----||-------||--->|   	|   										*/
/*							  Pn002.0=3  P-CL	+-------+   										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void BpiTorqueLimitControl( TRQ_LMT_DATA *TrqLimits, BASE_CTRL_HNDL *pBaseCtrl,	BOOL OtTrqLmtReq, ALMMAN_HANDLE *AlmMngr ) /* <S145> */
{
	LONG			lwk;
	LONG			FwTrqLmt;
	LONG			RvTrqLmt;
	LONG			UvTrqLmt;

/*--------------------------------------------------------------------------------------------------*/
/*		�n�s���̃g���N���~�b�g���Z																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( OtTrqLmtReq )
	{
		FwTrqLmt = TrqLimits->conf.EstopTrqLmt;
		RvTrqLmt = TrqLimits->conf.EstopTrqLmt;

		LpxUvTrqLmtControl( &(TrqLimits->UvTrqLimits), TRUE, AlmMngr );	/* <S145> */
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		���H�d���d���~�����̃g���N���~�b�g���Z													*/
/*--------------------------------------------------------------------------------------------------*/
		UvTrqLmt = LpxUvTrqLmtControl( &(TrqLimits->UvTrqLimits), FALSE, AlmMngr );	/* <S145> */

/*--------------------------------------------------------------------------------------------------*/
/*		��n�s���̃g���N���~�b�g���Z																*/
/*--------------------------------------------------------------------------------------------------*/
		if( TrqLimits->var.PclSignal )
		{
			FwTrqLmt = ( TrqLimits->conf.FwIntTrqLmt < TrqLimits->conf.FwExtTrqLmt ) ?
						 TrqLimits->conf.FwIntTrqLmt:
						 TrqLimits->conf.FwExtTrqLmt;
		}
		else
		{
			FwTrqLmt = TrqLimits->conf.FwIntTrqLmt;
		}

		if( TrqLimits->var.NclSignal )
		{
			RvTrqLmt = ( TrqLimits->conf.RvIntTrqLmt < TrqLimits->conf.RvExtTrqLmt ) ?
						 TrqLimits->conf.RvIntTrqLmt:
						 TrqLimits->conf.RvExtTrqLmt;
		}
		else
		{
			RvTrqLmt = TrqLimits->conf.RvIntTrqLmt;
		}
	/*------------------------------------------------------------------------------------------*/
		if( UvTrqLmt < FwTrqLmt )
		{
			FwTrqLmt = UvTrqLmt;
		}

		if( UvTrqLmt < RvTrqLmt )
		{
			RvTrqLmt =  UvTrqLmt;
		}
	/*------------------------------------------------------------------------------------------*/
		if( (pBaseCtrl->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_TRQ)
			&& (pBaseCtrl->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_JOG) )
		{
			if( TrqLimits->conf.TrefTrqLmt || (TrqLimits->conf.TrefTrqLmtCL && TrqLimits->var.PclSignal) )
			{ /* (Pn002.0=1:TLIM�ɂ��g���N�����L��) || ((Pn002.0=3:PCL/NCL�ɂ��g���N�����L��) && (PCL���)) */
				lwk = MlibLABS( TrqLimits->var.FwTrqLmt );
				if( lwk < FwTrqLmt )
				{
					FwTrqLmt = lwk;
				}
			}
		/*--------------------------------------------------------------------------------------*/
			if( TrqLimits->conf.TrefTrqLmt || (TrqLimits->conf.TrefTrqLmtCL && TrqLimits->var.NclSignal) )
			{ /* (Pn002.0=1:TLIM�ɂ��g���N�����L��) || ((Pn002.0=3:PCL/NCL�ɂ��g���N�����L��) && (NCL���)) */
				lwk = MlibLABS( TrqLimits->var.RvTrqLmt );
				if( lwk < RvTrqLmt )
				{
					RvTrqLmt = lwk;
				}
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�g���N���~�b�g���Z���ʏo�� --> Kernel														*/
/*--------------------------------------------------------------------------------------------------*/
	TrqLimits->var.PosTrqLmtOut =  FwTrqLmt;				/* ���]���g���N���~�b�g					*/
	TrqLimits->var.NegTrqLmtOut = -RvTrqLmt;				/* �t�]���g���N���~�b�g					*/

}



/****************************************************************************************************/
/*																									*/
/*		���[�^���x���o�p�ړ����ώ��萔�ύX�T�[�r�X		<V259>										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^���x���o�p�ړ����σt�B���^�̈ړ����ω񐔂��I�����C���ŕύX����B					*/
/*			������0�`4�ȊO���w�肳�ꂽ�ꍇ�́A�p�����[�^Pn009.2�ƃG���R�[�_����\����				*/
/*			���܂鏉���l���ݒ肳���B																*/
/*																									*/
/*		�i���Ӂj																					*/
/*			���x������Ƃ��ɕύX���s���ƁA�ړ����ϗp�o�b�t�@�̃N���A���s���邽�߁A				*/
/*			���x����u�[���ɂȂ��Ă��܂��B�ړ����ω񐔂̕ύX�͑��x���O�ōs�����ƁB					*/
/*																									*/
/****************************************************************************************************/
void BpiChangeSpdDetMaFil( MOTSPDMAFIL *MotSpdMafil, LONG num )		/* <S02F> */
{
	LONG	lwk0;

	if( ( num >= 0 ) && ( num <= 4 ) )
	{
		lwk0 = num;
	}
	else
	{
		lwk0 = MotSpdMafil->defaultNumBit;
	}
/*--------------------------------------------------------------------------------------------------*/
	if( MotSpdMafil->manumBit != lwk0 )
	{
		MotSpdMafil->manumBit = lwk0;
		MotSpdMafil->idx = 0;
		MotSpdMafil->ksub = 0;
		MotSpdMafil->mabufSumx = 0;
	}
}



/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�ߍX�V����																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*			�p�����[�^�Čv�Z��d����ʑ���񏑍��݂Ȃǂ�ScanA����~����ƁA							*/
/*			ScanA���ʒu�w�߂��󂯎��Ȃ��Ȃ�A�ʒu����̌����ƂȂ�B								*/
/*			ScanA���ʒu�w�߂��󂯎��Ȃ������ꍇ�́AScanB�ňʒu�w�߂𗭂߂Ă��������Ƃ���			*/
/****************************************************************************************************/
void	PosRefRenewal( POS_MNG_HNDL *PosManager, CTRLMD CtrlMode )
{
	if( CtrlMode.b.cm == CTRLMODE_POS )
	{ /* ScanA���ʒu�w�߂��󂯎��Ȃ��ꍇ */
		if( PosManager->RefRenewCntA != PosManager->RefRenewCntB )
		{
			PosManager->CompdPosRefi += PosManager->dPosRefi;							/* <S187>	*/
			PosManager->CompdPcmda   += PosManager->var.dPcmda;
		}
		else												/* �ʏ폈��								*/
		{
			PosManager->CompdPosRefi = PosManager->dPosRefi;							/* <S187>	*/
			PosManager->CompdPcmda   = PosManager->var.dPcmda;
		}
	}
	else													/* �ʒu����ȊO�̏ꍇ�̓N���A����		*/
	{
			PosManager->CompdPosRefi = 0;
			PosManager->CompdPcmda   = 0;
	}
	PosManager->RefRenewCntB++;							/* ScanA�ʒu�w�ߍX�V�p 					*/

	return;
}



/***************************************** end of file **********************************************/
