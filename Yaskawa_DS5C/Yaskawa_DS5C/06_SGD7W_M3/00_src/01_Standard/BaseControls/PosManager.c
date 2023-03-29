/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PosManager.c : �ʒu�Ǘ��֘A������`															*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �ʒu�Ǘ��֘A����																		*/
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
/*				<S00C> �\�[�X�R�[�h����																*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"			/* <S053> */
#include "BaseControls.h"
#include "PosManager.h"
#include "ModelControl.h"

/* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "SerialEncIf.h"
/* 2012.08.18 Y.Oka Make Mercury Base Software */

#include "KnlApi.h"	/* @@ ��ŏ��� */

/****************************************************************************************************/
/*		for Position Reference Filter																*/
/****************************************************************************************************/
//#define	AVFILCALCYCLE	KPI_SBCYCLENS				/* �U���}���t�B���^���Z�T�C�N���^�C��(ScanB)	[ns]	*/
//#define	MAXOMEGA2(AvfCycleNs)		(127323954/AvfCycleNs)	/* 127323954 = 10^9 * 0.8 / (2*PAI)	���g��2�ő�l[Hz]	*//* <S065> */

/****************************************************************************************************/
/*		for Position Reference Filter																*/
/****************************************************************************************************/
#define MAFIL_BUFNUM	256									/* �ړ����σt�B���^�o�b�t�@��			*/


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	PosMngInitPositionManager(BASE_CTRL_HNDL *BaseControls );									*/
/*	LONG	PosMngNetPosCmdManager( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf );		*/
/*	void	PosMngSenOnReCalcAbsoPos( BASE_CTRL_HNDL *BaseControls, MENCV *MencV,					*/
/*																	UNI_PRM_STR *uni_prm_ptr );		*/
/*	void	PosMngLatchNetFbkPos( BASE_CTRL_HNDL *BaseControls,										*/
/*												 SERVO_CONTROL_IF *SvControlIf, BPRMDAT *Bprm  );	*/
/*	void	StlgInitSettlingTime( SETTLINGTIME *SettlingTime, LONG CycleTimeUs );					*/
/*	void	StlgCalculateSettlingTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts );		*/
/*	void	StlgMakeMaxOverShoot( SETTLINGTIME *SettlingTime, POS_MNG_HNDL *PosManager,				*/
/*																			BOOL PosCtrlMode );		*/
/*	void	StlgMakeCoinOffTime( SETTLINGTIME *SettlingTime, LONG dPcmda,							*/
/*																BOOL CoinSts, BOOL PosCtrlMode );	*/
/****************************************************************************************************/
/*	void	BpiEgearFuncSwitch( POS_MNG_HNDL *PosManager, BOOL Switch );							*/
/*	void	BpiPcmdFilSwitch( POS_MNG_HNDL *PosManager, BOOL Switch );								*/
/****************************************************************************************************/
/*	void	BpiRsetMaxOverShoot( SETTLINGTIME *SettlingTime );										*/
/*	void	BpiRsetCoinOffTime( SETTLINGTIME *SettlingTime );										*/
/****************************************************************************************************/
//#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE ) /* <S065> */
//static	void	posMngInitPcmdFilter( VIBSUPFIL *VibSupFil );
//static	LONG	posMngVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal );
//#endif
static	void	posMngCtrlPosSignal( POS_MNG_HNDL *PosManager, POS_CTRL_STS *PosCtrlSts
									, PCMDFIL *pPcmdFil /* <S081> */);

//���󖢑Ή�	static	void	BpxMakeMotABCphaseSignal( BASE_CTRL_HNDL *BaseControls );
/****************************************************************************************************/
static void	BpxPerClrSignalProcedure( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm );/* @@ �ʒu�΍��N���A�M������ */
static void	BpxEdgeClrSvcPositionManager( BASE_LOOPCTRLS *BaseLoops ); 	/* @@ �T�[�{����ʒu�΍��G�b�W�N���A�ϐ�������	*/
/****************************************************************************************************/




/****************************************************************************************************/
/*																									*/
/*		�T�[�{����ʒu�Ǘ��ϐ�������																*/
/*																									*/
/****************************************************************************************************/
void	PosMngInitPositionManager( BASE_CTRL_HNDL *BaseControls )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{����ʒu�Ǘ��ϐ�������																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʒu�Ǘ��p�ϐ������� */
	MlibResetLongMemory( &(BaseControls->PosManager.var), sizeof(BaseControls->PosManager.var)/4 );
/*--------------------------------------------------------------------------------------------------*/
//	MlibResetLongMemory( &(BaseControls->PosManager.PosErrA), sizeof(PERRB)/4 );	/* @@ CLR�M�� */ /* <S107> */
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->PosManager.Egear = &(BaseControls->PosManager.conf.Egear);

/* 2012.09.07 Y.Oka Amon���󖢑Ή� */
//	BoutV.AmondPosCmd  = 0;								/* �ʒu�w�߃��j�^�ϐ�						*/
/* 2012.09.07 Y.Oka ��ScanA�p�ϐ��͂�����ŏ���������邽�߁A�����ł͏��������Ȃ��� */
	BaseControls->PosManager.dPosRefi = 0;				/* �ʒu�w�ߍ�������[Pulse/Scan]�̃N���A		*//* <S187> */
	BaseControls->PosManager.CompdPosRefi = 0;			/* �ʒu�w�ߍ�������(ScanB->ScanA)			*/
	BaseControls->PosManager.CompdPcmda = 0;			/* �ʒu�w�ߑ����l����(ScanB->ScanA)			*/

	/* ��SGDV�ɍ��킹���B�s�v�ł���΍폜���遚 */
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
	{
		BaseControls->PosCtrlSts.CoinSignal = TRUE;
		BaseControls->PosCtrlSts.NearSignal = TRUE;
	}
	else
	{
		BaseControls->PosCtrlSts.CoinSignal = FALSE;
		BaseControls->PosCtrlSts.NearSignal = FALSE;
	}

	/* �ʒu�w�߃t�B���^���������� */
//	posMngInitPcmdFilter( &(BaseControls->VibSupFil) );												  /* <S065> */
	PcmdFilInitialize( &(BaseControls->PcmdFil), &(BaseControls->VibSupFil) );
	/* MFC�ϐ������� */
	MfcInitModelControl( &(BaseControls->MFControl) );

#if (CSW_SVCF_BACKLASHCOMP == TRUE)
		/* �o�b�N���b�V�␳���� */
		BlshInvaildCompensate( &(BaseControls->PosManager.BlshCmp) );	/* <S18B> */
#endif

	BaseControls->CtrlCmdMngr.dPFfFilo = 0;
	BaseControls->CtrlCmdMngr.PicvClrCmd = FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		�T�[�{�ʐM�ʒu�w�ߊǗ�																		*/
/*																									*/
/*			��AP�^�����������g��																	*/
/*																									*/
/****************************************************************************************************/
LONG	PosMngNetPosCmdManager( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
															FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm )/* <S04D> */
{
	LONG			dPosRefo;
	BASE_CTRL_HNDL	*BaseControls;
	POS_MNG_HNDL	*PosManager;
	MFCTRL			*MFControl;
	BOOL 			den;
#if ( CSW_SVCF_BACKLASHCOMP )
	BOOL			BlshActive;
#endif

	BaseControls = BaseLoops->BaseCtrls;	
	PosManager	= &(BaseControls->PosManager);
	MFControl	= &(BaseControls->MFControl);

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߓ��͏���(�ʐM,PJOG,etc)�FPosManager->var.dPcmda	[�w�ߒP��/Scan]						*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{ /* �e�[�u���^�]�� */
		BaseControls->CtrlCmdMngr.PicvClrCmd = FALSE;			/* �ʒu����ϕ��N���A�w�߃��Z�b�g		*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->BeComplete == TRUE )
		{
			/* �e�[�u���^�]�ʒu�w�� */
			dPosRefo = BaseControls->TblDrive.var.dPosRef;
			/* �ʒu�΍��N���A�v������ */
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;
/* <S212> */
			/* �ʒu�w�߃����O�J�E���^�X�V */
			BaseControls->CtrlCmdMngr.PosRef += dPosRefo;
			/* �����ʒu�w�߂̍X�V */
			PosManager->var.dPcmda = dPosRefo;
//			BaseControls->AmondPosCmd = dPosRefo;		/* <S226> */
/* <S212> */
		}
		else
		{
			/* �T�[�{����ʒu�Ǘ��ϐ������� */
			PosMngInitPositionManager( BaseControls );
			/* �ʒu�΍��N���A�v�� */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			/* �ʒu����I�� */
			return( 0 );
		}
	}
	else if( BaseControls->CtrlModeSet.CtrlMode.dw == CTRLMODE_PJOG )
	{ /* �v���O����JOG���[�h�� */
		BaseControls->CtrlCmdMngr.PicvClrCmd = FALSE;			/* �ʒu����ϕ��N���A�w�߃��Z�b�g		*/
		BaseControls->PosCtrlSts.DenSignal   = FALSE;			/* Den Signal							*/

		if( BaseControls->BeComplete == TRUE )
		{ /* �ʒu������s */
			/* �ʒu�w�ߍ쐬����	[�w�ߒP��/Scan]	*/
			den = PrgJogMakePositionReference( &BaseControls->PJogHdl, &dPosRefo );

			/* Den Signal */
			BaseControls->PosCtrlSts.DenSignal  = den;
			BaseControls->PosCtrlSts.RefZSignal = den; /* <S066> */

			/* �ʒu�΍��N���A�v�� */
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;

			/* �ʒu�w�߃����O�J�E���^�X�V */
			BaseControls->CtrlCmdMngr.PosRef += dPosRefo;	/* <S1BA> */
			/* �����ʒu�w�߂̍X�V */
			PosManager->var.dPcmda = dPosRefo;				/* <S1BA> */
//			BaseControls->AmondPosCmd = dPosRefo;			/* <S1BA> */	/* <S226> */
		}
		else
		{ /* �ʒu����I�� */
			/* �T�[�{����ʒu�Ǘ��ϐ������� */
			PosMngInitPositionManager( BaseControls );
			/* �ʒu�΍��N���A�v�� */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			return( 0 );							
		}
	}
	else
	{ /* �ʏ�ʒu���䎞 */
	/*----------------------------------------------------------------------------------------------*/
	/*	@@ �΍��ر����																				*/
	/*----------------------------------------------------------------------------------------------*/
/* <S1B6> */
//		if( ((SvControlIf->CmdCtrl.CmdCtrlBit) >> CLRPOSERR_BITNO) & 1 )
//		{
//			PosMngInitPositionManager( BaseControls ); 		/* �T�[�{����ʒu�Ǘ��ϐ�������			*/
//			BaseControls->CtrlCmdMngr.PerClrEdge = FALSE; 	/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
//			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE; 	/* �ʒu�΍��N���A�v�� 					*/
//			return( 0 );									/* �ʒu����I��							*/
//		}
	/*----------------------------------------------------------------------------------------------*/
#if (CSW_CMDRENEW_PCMD_FIL_USE == TRUE) /* <S065> */
		/* �ʒu�w�߈ړ����Ϗ���(ScanB������ScanN�������قȂ�ꍇ�̕�ԏ���) */
		dPosRefo = MlibPcmdMafil( (SvControlIf->NetPosRef - BaseControls->CtrlCmdMngr.PosRefLo),
								  SvControlIf->CmdOptionIf.CycCtrl.CmdUpdateCycle,
								  &(BaseControls->CtrlCmdMngr.Pmafvar),
								  &(BaseControls->CtrlCmdMngr.Pmafbuf[0]) );
#else
		dPosRefo = SvControlIf->NetPosRef - BaseControls->CtrlCmdMngr.PosRefLo;
#endif

		/* �ʒu�w�߃����O�J�E���^�X�V */
		BaseControls->CtrlCmdMngr.PosRef += dPosRefo;	/* <S1B6> */
		/* �����ʒu�w�߂̍X�V */
		PosManager->var.dPcmda = dPosRefo;				/* <S1B6> */
//		BaseControls->AmondPosCmd = dPosRefo;			/* <S1B6> */	/* <S226> */
		
/* <S1B6> */
		if( ((SvControlIf->CmdCtrl.CmdCtrlBit) >> CLRPOSERR_BITNO) & 1 )
		{
			PosMngInitPositionManager( BaseControls ); 		/* �T�[�{����ʒu�Ǘ��ϐ�������			*/
			BaseControls->CtrlCmdMngr.PerClrEdge = FALSE; 	/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE; 	/* �ʒu�΍��N���A�v�� 					*/
			return( 0 );									/* �ʒu����I��							*/
		}

		/* �ʒu�΍��N���A�v������ */
		BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*	@@ �ʒu�΍��G�b�W�N���A����																	*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->PerClrEdge )
		{
			BpxPerClrSignalProcedure( BaseLoops, Bprm ); /* <S04D>:�����ǉ� */
			BaseControls->CtrlCmdMngr.PerClrEdge = TRUE; /* �ʒu�΍��N���A�G�b�W�M���Z�b�g			*/
			dPosRefo = 0;
		}
		else
		{
			BaseControls->CtrlCmdMngr.PerClrEdge = FALSE; /* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		@@ �΍��N���A���́A�ȉ������͖���															*/
/*--------------------------------------------------------------------------------------------------*/

	/* �ʒu�w�߃����O�J�E���^�X�V */
//	BaseControls->CtrlCmdMngr.PosRef += dPosRefo;	/* <S1B6> */
	/* �����ʒu�w�߂̍X�V */
//	PosManager->var.dPcmda = dPosRefo;				/* <S1B6> */

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߓd�q�M�A�ϊ��FPosManager->var.dPcmdEgear [Pulse/Scan]								*/
/*--------------------------------------------------------------------------------------------------*/
	PosManager->var.dPcmdEgear = MlibPcmdEgear( dPosRefo,
												PosManager->Egear,
												&(PosManager->var.dPcmdEgrem) );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^���Z(�w��������,�ړ�����)�FBaseControls->CtrlCmdMngr.dPcmdFilo [Pulse/Scan]	*//* <S065> */
/*--------------------------------------------------------------------------------------------------*/
	/* ���o�������M������(���o���������W���[����FALSE�ɂ���) */
//	BaseControls->PosCtrlSts.RefZSignal = ( PosManager->var.dPcmdEgear != 0 ) ? FALSE : TRUE;		  /* <S065> */
	PcmdFilSetRefZStatus( &(BaseControls->PcmdFil), TRUE );
	/* �ʒu�w�߃t�B���^���� */
	BaseControls->CtrlCmdMngr.dPcmdFilo = PcmdFilRuntimeService( &(BaseControls->PcmdFil),
																 PosManager->var.dPcmdEgear,
																 PosManager->var.PosErrA.per32s,
																 PosManager->var.PcmdFilStop );
	/* ���o��������Ԏ擾 */
	BaseControls->PosCtrlSts.DenSignal = PcmdFilGetRefZStatus( &(BaseControls->PcmdFil) );
	BaseControls->PosCtrlSts.RefZSignal = BaseControls->PosCtrlSts.DenSignal;

/*--------------------------------------------------------------------------------------------------*/
/*		�g��(FF)���䉉�Z(MFC,���ǂߐ���)															*/
/*--------------------------------------------------------------------------------------------------*/
/*		BaseControls->CtrlCmdMngr.dPosRefo [Pulse/Scan]�F�ʒu����ʒu�w�ߍ�������					*/
/*--------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�o�b�N���b�V�␳����																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_BACKLASHCOMP )
	if( (BaseControls->BeComplete == FALSE)
		|| (BaseControls->MencV->PhaseReady == FALSE)		/* <S18B> */
		|| (BaseControls->OverTravel == TRUE) )
	{
		BlshActive = FALSE;
	}
	else
	{
		BlshActive = TRUE;
	}
	BlshCompensateBacklash( &(PosManager->BlshCmp),
							&(PosManager->conf.Egear),
//							&(PosManager->var.dPcmdEgear),											  /* <S065> */
							&(BaseControls->CtrlCmdMngr.dPcmdFilo),									  /* <S065> */
							&(PosManager->var.dPcmda),/* &(dPosRefo),*/			/* <S18B> */
							&(BaseControls->PosCtrlSts.RefZSignal),
							BlshActive );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		�U���}���t�B���^����																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
//	if( PosManager->var.PcmdFilStop == FALSE ) /* <S065> */
	{ /* �ʒu�w�߃t�B���^�����v���Ȃ� */
		/* �U���}���t�B���^��������������(�ʒu����ȊO�͖���) */
		if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
		{
			BaseControls->VibSupFil.conf.Pexe.enable = 0;
		}
		/* �U���}���t�B���^���Z */
//		BaseControls->CtrlCmdMngr.dPcmdFilo = posMngVibSupFilter(									  /* <S065> */
		BaseControls->CtrlCmdMngr.dPcmdFilo = PcmdFilVibSupFilter(
												&(BaseControls->VibSupFil),
//												PosManager->var.dPcmdEgear,							  /* <S065> */
												BaseControls->CtrlCmdMngr.dPcmdFilo,				  /* <S065> */
												&(BaseControls->PosCtrlSts.RefZSignal) );
	}
//	else /* <S065> */
//	{ /* �ʒu�w�߃t�B���^�����v������ */
//		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
//	}
#else
//		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear; /* <S065> */
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		MFC����																					*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_MFC_USE == TRUE )
	/* MFC��������������(�ʒu����ȊO�͖���) */
	if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
	{ /* �O��̐��䃂�[�h���ʒu����łȂ��ꍇ */
		/* MFC�����ɏ����� */
		MFControl->conf.MfcPexe.MFCModel = 0;
	}
	/* MFC���Z */
	dPosRefo = MfcControlMain( &(BaseControls->MFControl),
							   BaseControls->CtrlCmdMngr.dPcmdFilo,
							   &(BaseControls->CtrlCmdMngr.SpdFFCx),
							   &(BaseControls->CtrlCmdMngr.TrqFFCx) );
#else
	MFControl->conf.MfcPexe.MFCModel = 0;
	dPosRefo = BaseControls->CtrlCmdMngr.dPcmdFilo;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		����ʒu�M������																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Make Coin and Near Status */
	posMngCtrlPosSignal( PosManager, &(BaseControls->PosCtrlSts)
					,&(BaseControls->PcmdFil) /* <S081> */ );

	return (dPosRefo);
}

/****************************************************************************************************/
/*																									*/
/*		SEN ON���̐�Έʒu�Čv�Z																	*/
/*																									*/
/****************************************************************************************************/
void	PosMngSenOnReCalcAbsoScalePos(MENCV *MencV, APOSRG *EncPos, EGEAR *Egear, BOOL RvsDir)
{
	LONG	IniPos[2];
	UCHAR	Sftx;

	/* ���[�J���ϐ��̏����� */
	Sftx	= FSX_FPOS_SFT;

	if( MencV->AbsoEncType == MENC_INCRE_TYPE )
	{
		MlibResetLongMemory(EncPos, sizeof(APOSRG)/4);
	}
	else
	{
		IniPos[1] = (LONG)((SHORT)MencV->RxPosH[0] >> Sftx);		/* <S209> *//* <S211> */
		IniPos[0] = (LONG)( ((ULONG)MencV->RxPosH[0] << (32-Sftx))
						+ ((ULONG)MencV->RxPosL[0].dw >> Sftx) );	/* <S209> */

		if( MencV->RevAsmMotor ^ RvsDir )/* ���]�ڑ� XOR �t��]���[�h */
		{
			IniPos[1] = (~IniPos[1]);		/* �������]����	*/
			IniPos[0] = (~IniPos[0]) + 1;	/* �������]���� */
			if( IniPos[0] == 0 )
			{	/* Carry�̏ꍇ�́A��ʂ�+1 */
				++IniPos[1];
			}
		}
		/*	�������ԑ�̊֐������g�p����B
		 * (�T�[�{OFF���̏����̂��߁A�������ԑ����]�T����)	*/
		MlibAposRg64iv( IniPos[0], IniPos[1], Egear, EncPos );
	}
}

/****************************************************************************************************/
/*																									*/
/*		SEN ON���̐�Έʒu�Čv�Z																	*/
/*																									*/
/****************************************************************************************************/
void	PosMngSenOnReCalcAbsoPos( MENCV *MencV, APOSRG *EncPos,
								  EGEAR *Egear, DBYTEX bit_dp, USHORT limmlt, BOOL RvsDir )
{
	SencMakeAbsPosition(MencV, limmlt, bit_dp, RvsDir);
	MlibAposRg64iv( MencV->SenOnScalePosL, MencV->SenOnScalePosH, Egear, EncPos );
}



/****************************************************************************************************/
/*																									*/
/*		�ʒu�e�a���b�`����																			*/
/*																									*/
/****************************************************************************************************/
void	PosMngLatchNetFbkPos( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf, BPRMDAT *Bprm )/* <S04D> */
{
	POS_MNG_HNDL	*PosManager;
	LONG			RspLtBit = 0;
	LONG			dPosCmp;
	LONG			motLpos, motPos;																/* <S004> */
	LONG			posSftR;																		/* <S04D> */
	LONG			rvsDir;																			/* <S221> */

	PosManager = &(BaseControls->PosManager);
	motPos = BaseControls->MencV->MotPosX[0];														/* <S007> */
	rvsDir = 1;																						/* <S221> */

	if(Bprm->FencUse != FALSE)/* <S04D>:�ٸ�����ǉ� */
	{/* �ٸ	*/
		posSftR = BaseControls->FencV->MposSftR;
	}
	else
	{/* �и	*/
		posSftR = BaseControls->MencV->MposSftR;
		if(Bprm->RvsDir)/* <S221> */
		{
			rvsDir = -1;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�b�����b�`����																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqCphs != FALSE )
	{ /* ���b�`�v������ */
//		if(BaseControls->MotSts.MotCphPass != FALSE)/* <S04D> */
		if((Bprm->FencUse != FALSE)&&(BaseControls->MotSts.FencCphPass != FALSE)
			||(Bprm->FencUse == FALSE)&&(BaseControls->MotSts.MotCphPass != FALSE))/* <S04D>:�ٸ�Ή� */
		{/* ���b�`���� */
			dPosCmp = BaseControls->CtrlCmdMngr.FbkPosFromCpos;

			/* ��Έʒu�X�V�v�Z */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonCphaLpos );

			SvControlIf->LtReqCphs = FALSE;
			RspLtBit |= ((SvControlIf->LtReqCphs^1) * C_PHASE_COMP_BIT);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext1���b�`����																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqExt1 != FALSE )
	{ /* ���b�`�v������ */
		if((SvControlIf->ExtLtSts[0]).Complete != FALSE)
		{/* ���b�`���� */
			motLpos = (LONG)SvControlIf->ExtLtSts[0].LatchPosition << posSftR;	/* <S004> *//* <S04D> */
			dPosCmp = rvsDir * ((motPos - motLpos) >> posSftR);					/* <S004> *//* <S04D> *//* <S221> */
			/* ��Έʒu�X�V�v�Z */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonExt1Lpos);

			SvControlIf->LtReqExt1 = FALSE;
			RspLtBit |= ((SvControlIf->LtReqExt1^1) * EXT_SIG1_COMP_BIT);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext2���b�`����																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqExt2 != FALSE )
	{ /* ���b�`�v������ */
		if((SvControlIf->ExtLtSts[1]).Complete != FALSE)
		{/* ���b�`���� */
			motLpos = (LONG)SvControlIf->ExtLtSts[1].LatchPosition << posSftR;	/* <S004> *//* <S04D> */
			dPosCmp = rvsDir * ((motPos - motLpos) >> posSftR);					/* <S004> *//* <S04D> *//* <S221> */
			/* ��Έʒu�X�V�v�Z */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonExt2Lpos);

			SvControlIf->LtReqExt2 = FALSE;
			RspLtBit |= ((SvControlIf->LtReqExt2^1) * EXT_SIG2_COMP_BIT);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext3���b�`����																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqExt3 != FALSE )
	{ /* ���b�`�v������ */
		if(SvControlIf->ExtLtSts[2].Complete != FALSE)
		{/* ���b�`���� */
			motLpos = (LONG)SvControlIf->ExtLtSts[2].LatchPosition << posSftR;	/* <S004> *//* <S04D> */
			dPosCmp = rvsDir * ((motPos - motLpos) >> posSftR);					/* <S004> *//* <S04D> *//* <S221> */
			/* ��Έʒu�X�V�v�Z */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonExt3Lpos);

			SvControlIf->LtReqExt3 = FALSE;
			RspLtBit |= ((SvControlIf->LtReqExt3^1) * EXT_SIG3_COMP_BIT);
		}
	}

	/* ���b�`������Ԑݒ� */
//	RspLtBit = ((SvControlIf->LtReqCphs^1) * C_PHASE_COMP_BIT)
//			 | ((SvControlIf->LtReqExt1^1) * EXT_SIG1_COMP_BIT)
//			 | ((SvControlIf->LtReqExt2^1) * EXT_SIG2_COMP_BIT)
//			 | ((SvControlIf->LtReqExt3^1) * EXT_SIG3_COMP_BIT);

	/* ���b�`�v�����x���ݒ� */
	SvControlIf->RspLtBit = RspLtBit | (SvControlIf->CmdLtBit & 0xF);
}


/****************************************************************************************************/
/*																									*/
/*		����ʒu�M������																			*/
/*																									*/
/****************************************************************************************************/
static	void	posMngCtrlPosSignal( POS_MNG_HNDL *PosManager, POS_CTRL_STS *PosCtrlSts,
									PCMDFIL	*pPcmdFil /* <S081> */ )
{
	LONG	abs_poserr;

	abs_poserr = PosManager->var.PosErrA.per32a;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���ߊ����M���쐬																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( abs_poserr <= PosManager->conf.CoinLevel )
	{
		switch( PosManager->conf.CoinOutSel )
		{
		case POSERR:		/* �ʒu�΍���/COIN�o�� */
			PosCtrlSts->CoinSignal = TRUE;
			break;

		case POSERR_REFOUT:	/* �ʒu�΍��ƈʒu�w�߃t�B���^�o�͂�/COIN�o�� */
//<S081>			PosCtrlSts->CoinSignal = PosCtrlSts->RefZSignal;
			PosCtrlSts->CoinSignal = PcmdFilGetRefZStatus( pPcmdFil );	/* <S081> */
			break;

		case POSERR_REFIN:	/* �ʒu�΍��ƈʒu�w�ߓ��͂�/COIN�o�� */
			PosCtrlSts->CoinSignal = (PosManager->var.dPcmda == 0) ? TRUE : FALSE;
			break;

		default:
			break;
		}
	}
	else
	{
		PosCtrlSts->CoinSignal = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���ߋߖT�M���쐬																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( abs_poserr <= PosManager->conf.NearLevel )
	{
		PosCtrlSts->NearSignal = TRUE;
	}
	else
	{
		PosCtrlSts->NearSignal = FALSE;
	}
}


#if 0 /* <S065> */
#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�߃t�B���^������																		*/
/*																									*/
/****************************************************************************************************/
static	void	posMngInitPcmdFilter( VIBSUPFIL *VibSupFil )
{
	MlibResetLongMemory( &VibSupFil->var, sizeof(VibSupFil->var)/4 );
	VibSupFil->conf.Pexe = VibSupFil->conf.VibSupPrm;
}


/****************************************************************************************************/
/*																									*/
/*		�U���}���t�B���^���Z																		*/
/*																									*/
/*	�T�v:	�U���}���t�B���^�́C�ݒ肳�ꂽ���g���ɑ΂���m�b�`�������������t�B���^�ł���B			*/
/*			�@��U���Ȃǈʒu���ߎ��̐U����}������B												*/
/*			�ړ����ϓ��ʒu�w�߃t�B���^�CMFC�Ƃ�����ɐڑ��\�B										*/
/*																									*/
/****************************************************************************************************/
static	LONG	posMngVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal )
{
	LONG	AvffFili;
	LONG	AvffFilo;
	LONG	x1;
	LONG	x2;
	LONG	x3;
	LONG	wk = 0;

/*--------------------------------------------------------------------------------------------------*/
/*	�w�ߊ����������o�������`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (dPcmd == 0) && (VibSupFil->var.Buf == 0) )
	{ /* �w�ߊ��� && �t�B���^���܂�Ȃ�->�����o������ */
		VibSupFil->conf.Pexe = VibSupFil->conf.VibSupPrm;		/* �p�����[�^�ؑւ����s				*/
	}
	AvffFili = dPcmd;											/* �U���}���t�B���^���ʒu�w�߃p���X	*/

/*--------------------------------------------------------------------------------------------------*/
/*		�U���}���t�B���^���Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( VibSupFil->conf.Pexe.enable )
	{	/* �@�\�L��	*/
		x1 = MlibPfbkxremNolim( AvffFili, VibSupFil->conf.Pexe.Kff1, &VibSupFil->var.rem1 );
		x2 = MlibPfbkxremNolim( VibSupFil->var.wkbf1, VibSupFil->conf.Pexe.Kff2, &VibSupFil->var.rem2 );
		x3 = MlibPfbkxremNolim( VibSupFil->var.wkbf2, VibSupFil->conf.Pexe.Kff3, &VibSupFil->var.rem3 );

		VibSupFil->var.wkbf1 += (x1 - x3 - AvffFili);
		VibSupFil->var.wkbf2 += (x1 - x3 + x2);

		AvffFilo = x1 - x3;										/* �U���}���t�B���^�o��				*/

/*--------------------------------------------------------------------------------------------------*/
/*		�t�B���^���o�͕΍�(���܂�)																	*/
/*--------------------------------------------------------------------------------------------------*/
		VibSupFil->var.Buf = MlibPerrcalx( AvffFili, AvffFilo, VibSupFil->var.FilioErr );
		if( VibSupFil->var.Buf != 0 )
		{ /* �ʒu�w�ߕ����o�������� */
			*RefZSignal = FALSE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�U���}���t�B���^�ϐ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (AvffFilo == 0)	&& (VibSupFil->var.wkbf1 == 0) && (VibSupFil->var.wkbf2 == 0) )
		{ /* �U���}���t�B���^�o��=0 && �U���}���t�B���^BUF=0 */
			/* �U���}���t�B���^�ϐ������� */
			MlibResetLongMemory( &VibSupFil->var, sizeof(VibSupFil->var)/4 );
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�t�B���^�o�͏���																			*/
/*--------------------------------------------------------------------------------------------------*/
		wk += AvffFilo;											/* �@�\������Ԏ���					*/
		VibSupFil->var.Filo = wk;								/* �t�B���^�o�́{�w�ߓ��͂��o��		*/
	}
	else
	{	/* �@�\����	*/
		wk = AvffFili;											/* ���͂����̂܂܏o��				*/
		VibSupFil->var.Filo = wk;
	}

	return( wk );
}


/****************************************************************************************************/
/*																									*/
/*		�U���}���t�B���^�I��																		*/
/*																									*/
/****************************************************************************************************/
BOOL	PosMngCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw, ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs )
{
	BOOL	PrmSetErr;
	LONG	wk;

	PrmSetErr = FALSE;
	wk = (mdlsw >> 4) & 0x0F;

	switch( wk )
	{
	case 0x00: /* �@�\���� */
	case 0x01:
		VibSupFil->conf.VibSupSetting = FALSE;
		break;
	case 0x02: /* �U���}���t�B���^�L�� */
		VibSupFil->conf.VibSupSetting = TRUE;
		break;
	default :
		PrmSetErr = TRUE;
		break;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	MFC �X�C�b�`�������͐U���}���t�B���^OFF													*/
	/*------------------------------------------------------------------------------------------*/
	wk = mdlsw & 0x0F;
	switch( wk )
	{
	case 0x00: /* �@�\���� */
		/* �U���}���t�B���^����OFF */
		VibSupFil->conf.VibSupSetting = FALSE;
		break;
	}

	/* ���s�p�̋@�\�I���X�C�b�`�֐ݒ蔽�f */
	PosMngCalculatePrmVibSupFil( VibSupFil, ff_feq, ff_fil, ScanTimeNs );

	return	PrmSetErr;
}




/****************************************************************************************************/
/*																									*/
/*		�U���}���t�B���^�p�����[�^�v�Z																*/
/*																									*/
/****************************************************************************************************/
void	PosMngCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil, LONG ScanTimeNs )
{
	VIBSUPFILPRM	wrkp;
	LONG			u1;
	LONG			s, sx;
	LONG			kx;
	LONG			wk;
	LONG			omega2;
//	LONG			scantime;

/*--------------------------------------------------------------------------------------------------*/
/*		���Z�����ݒ�																				*/
/*--------------------------------------------------------------------------------------------------*/
//	scantime = AVFILCALCYCLE;							/* ���Z�T�C�N���^�C��	[ns]				*/

/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�I��																					*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.enable = VibSupFil->conf.VibSupSetting;
	u1 = ff_fil;

/*--------------------------------------------------------------------------------------------------*/
/*		�ւQ�ő�l																					*/
/*																									*/
/*					      (ff_frq/10)[Hz] * (ff_fil[%]/100) * (80/100)�@* 10^9						*/
/*			omega2_limit = --------------------------------------------------------					*/
/*				   		               2 * PAI * CycleNs											*/
/*--------------------------------------------------------------------------------------------------*/
	wk = (LONG)ff_frq * (LONG)ff_fil;					/* �p�����[�^�ݒ��2*1000[Hz]				*/
	omega2 = MlibMIN( wk, (MAXOMEGA2(ScanTimeNs) * 1000) );	/* *1000[Hz]���~�b�g�ƃp�����[�^�l�̍ŏ��l	*/

/*--------------------------------------------------------------------------------------------------*/
/*		�U���}���t�B���^�Q�C��1 Avff.P.Kff1															*/
/*--------------------------------------------------------------------------------------------------*/
/*					      ff_fil^2																	*/
/*				Kff1 = --------------																*/
/*				   		   10000																	*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Kff1 = MlibScalKxgain( u1 * u1, 1, 10000, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		�U���}���t�B���^�Q�C��2 Avff.P.Kff2															*/
/*--------------------------------------------------------------------------------------------------*/
/*		�U���}���t�B���^�Q�C��3 Avff.P.Kff3															*/
/*--------------------------------------------------------------------------------------------------*/
/*					     PAI * omega2 * CycleNs														*/
/*				Kff2 = --------------------------													*/
/*				   		       1000 * 10^9															*/
/*--------------------------------------------------------------------------------------------------*/
/*					     4*PAI * omega2 * CycleNs													*/
/*				Kff3 = -----------------------------												*/
/*				   		       1000 * 10^9															*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( omega2, ScanTimeNs, C10POW9, &s, 0 );
	kx = MlibPcalKxgain( kx, 1, 1000000000, &s, 0 );
	sx = s;

	wrkp.Kff2 = MlibPcalKxgain( kx, 3141593, 1, &s, 24 );				/* 3141593  =   PAI*1000000	*/
	wrkp.Kff3 = MlibPcalKxgain( kx, 12566371, 1, &sx , 24 );			/* 12566371 = 4*PAI*1000000	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Set Parameters																				*/
/*--------------------------------------------------------------------------------------------------*/
//	KPI_DI();		/* Disable Interrupt	*/
	VibSupFil->conf.VibSupPrm = wrkp;
//	KPI_EI();		/* Enable Interrupt		*/

}

#endif /* #if( CSW_SVCF_RESVIB_FIL_USE != 0 ) */
#endif /* #if 0 */ /* <S065> */


/****************************************************************************************************/
/*																									*/
/*		�d�q�M�A�@�\�L��/�����؂�ւ�����															*/
/*																									*/
/****************************************************************************************************/
void	BpiEgearFuncSwitch( POS_MNG_HNDL *PosManager, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* �d�q�M�A�@�\�L�����v���� */
		PosManager->var.EgearDisable = FALSE;
		PosManager->Egear = &(PosManager->conf.Egear);
	}
	else
	{ /* �d�q�M�A�@�\�������v���� */
		PosManager->var.EgearDisable = TRUE;
		PosManager->Egear = &(PosManager->conf.InvalidEgear);
	}
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�߃t�B���^�L��/�����؂�ւ�����														*/
/*																									*/
/****************************************************************************************************/
void	BpiPcmdFilSwitch( POS_MNG_HNDL *PosManager, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* �ʒu�w�߃t�B���^�L�����v���� */
		PosManager->var.PcmdFilStop = FALSE;
	}
	else
	{ /* �ʒu�w�߃t�B���^�������v���� */
		PosManager->var.PcmdFilStop = TRUE;
	}
}


/****************************************************************************************************/
/*																									*/
/*		���莞�Ԋ֘A�����p�ϐ�����������															*/
/*																									*/
/****************************************************************************************************/
void	StlgInitSettlingTime( SETTLINGTIME *SettlingTime, LONG CycleTimeUs )
{
	/* Reset Settling Time Variable */
	MlibResetLongMemory( SettlingTime, sizeof(*SettlingTime)/4 );

	/* Set Unit Change Gain [cyc] �� [1us] */
	//SettlingTime->UnitChngGain = MlibPcalKxgain( CycleTimeUs, 1, 100, NULL, 24 );//kira�C��_forAAT_110418
	SettlingTime->UnitChngGain = CycleTimeUs;

#if 0		/* <S0DA> */
	/* Start Settling Time Process */
	KlibRstLongTimer( &(SettlingTime->ProcessTimer) );
#endif

}


/****************************************************************************************************/
/*																									*/
/*		���莞�ԎZ�o�p�֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v�F�ʒu�w�ߕ����o���������玟�̈ʒu�w�ߕ����o���J�n�܂ł̊Ԃ܂��́C�J�E���^��MAX�l�ɂȂ�܂�	*/
/*		�@�Ō��/COIN�M����OFF�ɂȂ�܂ł̎���(�J�E���^�l)�𑪒肷��B								*/
/*																									*/
/****************************************************************************************************/
void	StlgCalculateSettlingTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts )
{
#if 0		/* <S0DA> */
	/* Check Timing of Measuring Settling Time */
	if( 1 > KlibGetLongTimerMs(SettlingTime->ProcessTimer) )			/* 1[ms]���ɏ���				*/
	{
		return;
	}
	/* Reset Timing of Measuring Settling Time */
	KlibRstLongTimer( &(SettlingTime->ProcessTimer) );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*	���莞�ԎZ�o���Z���t���O�쐬																*/
	/*----------------------------------------------------------------------------------------------*/
	if( dPcmda != 0 )
	{ /* ���쒆 */
		if( SettlingTime->StlgLastdPcmd == 0 )
		{ /* �ʒu�w�߂����͂����ŏ��̃X�L�����̏ꍇ */
			/* �ێ����Ă��鐮�莞�Ԃ��o�͗p�ϐ��ɑ�� */
			SettlingTime->RsltStlgTime = SettlingTime->TmpStlgTime;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	���쒆�͕ϐ����N���A����																*/
		/*------------------------------------------------------------------------------------------*/
		SettlingTime->StlgCalcReq = FALSE;				/* ���莞�ԉ��Z�v���t���O��OFF				*/
		SettlingTime->StlgLastCoinSts = FALSE;			/* �O��COIN�M����OFF						*/
		SettlingTime->StlgTimeCntr = 0;					/* �J�E���^���N���A							*/
	}
	else if( SettlingTime->StlgLastdPcmd != 0 )
	{ /* �ʒu�w�ߕ����o��������(�ŏ�����) */
		SettlingTime->StlgCalcReq = TRUE;				/* ���莞�ԉ��Z�v���t���O��ON				*/
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	���莞�ԎZ�o���Z																			*/
	/*----------------------------------------------------------------------------------------------*/
	if( SettlingTime->StlgCalcReq == TRUE )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	SettlingTime->StlgTimeCntr = 65535�̎��ɐ��莞�Ԃ��o�͗p�ϐ��ɑ��	(��8[s])			*/
		/*------------------------------------------------------------------------------------------*/
		if( SettlingTime->StlgTimeCntr == 65535 )
		{
			SettlingTime->RsltStlgTime = SettlingTime->TmpStlgTime;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	�ʒu���ߊ����M����ON�ɂȂ������̂ݐ��莞�Ԃ��o�b�t�@									*/
		/*------------------------------------------------------------------------------------------*/
		if( ( CoinSts == TRUE ) && ( SettlingTime->StlgLastCoinSts == FALSE ) )
		{
			SettlingTime->TmpStlgTime = SettlingTime->StlgTimeCntr;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	�J�E���^���C���N�������g(������65535�Ń��~�b�g)											*/
		/*------------------------------------------------------------------------------------------*/
		if( SettlingTime->StlgTimeCntr != 65535 )
		{
			SettlingTime->StlgTimeCntr++;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�O��l��ێ�																				*/
	/*----------------------------------------------------------------------------------------------*/
	SettlingTime->StlgLastdPcmd = dPcmda;						/* �����ʒu�w�߂�ێ�				*/
	SettlingTime->StlgLastCoinSts = CoinSts;					/* COIN�M����ێ�					*/

//	BoutV.OutSettlingTime = SettlingTime->RsltStlgTime;

}


/****************************************************************************************************/
/*																									*/
/*		�ő�I�[�o�[�V���[�g�ʍ쐬 --> Un106														*/
/*																									*/
/****************************************************************************************************/
void	StlgMakeMaxOverShoot( SETTLINGTIME *SettlingTime, POS_MNG_HNDL *PosManager, BOOL PosCtrlMode )
{

	if( (PosCtrlMode == TRUE) && (PosManager->var.dPcmda == 0) )
	{
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���ߕ�������																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( SettlingTime->LastdPcmda > 0 )		/* �������ւ̈ʒu���߁@�w�ߕ����o������			*/
		{
			SettlingTime->UnMaxOverShoot = 0;		/* �ő�I�[�o�[�V���[�g�ʃN���A					*/
			SettlingTime->OverErrDir = -1;			/* �I�[�o�[�V���[�g����΍��̕���(-)			*/
		}
		else if( SettlingTime->LastdPcmda < 0 )	/* �������ւ̈ʒu���߁@�w�ߕ����o������			*/
		{
			SettlingTime->UnMaxOverShoot = 0;		/* �ő�I�[�o�[�V���[�g�ʃN���A					*/
			SettlingTime->OverErrDir = 1;			/* �I�[�o�[�V���[�g����΍��̕���(+)			*/
		}

		SettlingTime->OverShoot = SettlingTime->OverErrDir * PosManager->var.PosErrA.per32s;

/*--------------------------------------------------------------------------------------------------*/
/*		�I�[�o�[�V���[�g�ʔ���																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( SettlingTime->UnMaxOverShoot < SettlingTime->OverShoot )
		{
			SettlingTime->UnMaxOverShoot = SettlingTime->OverShoot;
		}
	}
	else
	{
		BpiRsetMaxOverShoot( SettlingTime );			/* �I�[�o�[�V���[�g�ʃN���A����				*/
	}
	SettlingTime->LastdPcmda = PosManager->var.dPcmda;
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu���ߊ������s�ݐώ��ԍ쐬 --> Un10B														*/
/*																									*/
/****************************************************************************************************/
void	StlgMakeCoinOffTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts, BOOL PosCtrlMode )
{

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���ߊ����m�F																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (PosCtrlMode == TRUE) && (dPcmda == 0) )
	{
		if( CoinSts == TRUE )
		{
			SettlingTime->StlgCoinLatch = TRUE;
		}
	}
	else
	{
		SettlingTime->StlgCoinLatch = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		COIN���ꌟ�o --> �ʒu���ߊ������s�ݐώ��ԍ쐬												*/
/*--------------------------------------------------------------------------------------------------*/
	if( (SettlingTime->StlgCoinLatch == TRUE) && (CoinSts == FALSE) )
	{
		/* �J�E���^������~�b�g */
		if( SettlingTime->UnCoinOffTimer < 0x1000000 )
		{
			/* �ʒu���ߊ������s�ݐώ���[cycle] */
			SettlingTime->UnCoinOffTimer++;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���ߊ������s�ݐώ��Ԃ̃N���A���� 														*/
/*			�EFn01C --> Un10B = 0																	*/
/*			�E�ʒu���䃂�[�h�ȊO																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosCtrlMode == FALSE )
	{
		BpiRsetCoinOffTime( SettlingTime );
	}

}

/****************************************************************************************************/
/*																									*/
/*		���e�I�[�o�[�V���[�g�ʃp�����[�^�v�Z														*/
/*																									*/
/****************************************************************************************************/
void	StlgCalculateOverShootErrLevel( SETTLINGTIME *SettlingTime, LONG CoinLevel, LONG ovserrdetlvl )
{
	LONG	sx;
	sx = 0;
	SettlingTime->OvsErrDetLvl = MlibScalKxgain( CoinLevel,ovserrdetlvl, 100, &sx, -24 );
}


/****************************************************************************************************/
/*																									*/
/*		�I�[�o�[�V���[�g�ʃN���A����																*/
/*																									*/
/****************************************************************************************************/
void	BpiRsetMaxOverShoot( SETTLINGTIME *SettlingTime )
{
	SettlingTime->UnMaxOverShoot = 0;
	SettlingTime->OverErrDir = 0;
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu���ߊ������s�ݐώ��ԃN���A����															*/
/*																									*/
/****************************************************************************************************/
void	BpiRsetCoinOffTime( SETTLINGTIME *SettlingTime )
{
	SettlingTime->UnCoinOffTimer = 0;
	SettlingTime->StlgCoinLatch = FALSE;
	SettlingTime->StlgCoinLatch = FALSE;
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu���ߊ������s�ݐώ��Ԏ擾����															*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetCoinOffTime( SETTLINGTIME *SettlingTime )
{
#if (FLOAT_USE==TRUE)
	return ( SettlingTime->UnCoinOffTimer * SettlingTime->UnitChngGain );
#else
	return ( MlibMulgain( SettlingTime->UnCoinOffTimer, SettlingTime->UnitChngGain) );
#endif /* FLOAT_USE */
}


/****************************************************************************************************/
/*																									*/
/*		�ő�I�[�o�[�V���[�g��[�w�ߒP��]�擾����													*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetMaxOverShoot( SETTLINGTIME *SettlingTime )
{
	return (SettlingTime->UnMaxOverShoot);
}


/****************************************************************************************************/
/*																									*/
/*		COIN���b�`��Ԏ擾����																		*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetCoinLatchSts( SETTLINGTIME *SettlingTime )
{
	return (SettlingTime->StlgCoinLatch);
}


/****************************************************************************************************/
/*																									*/
/*		���莞�ԉ��Z���ʎ擾����																	*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetSettlingTime( SETTLINGTIME *SettlingTime )
{
	return (SettlingTime->RsltStlgTime);
}


/****************************************************************************************************/
/*																									*/
/*		�T�[�{����l�b�g�ʒu�����Ǘ�														<S14C>	*/
/*																									*/
/****************************************************************************************************/
void	PosMngResponseManager( BASE_CTRL_HNDL *BaseControls )
{

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�`�a�b���M���쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���[�^�G���R�[�_�`�a�b���M���쐬 */
	/* 2009.06.04 Y.Oka (>_<) ���󖢑Ή� */
//	BpxMakeMotABCphaseSignal( BaseControls );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���䃂�[�h�ȊO�̎�																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS )
	{
		/* �T�[�{����ʒu�Ǘ��ϐ������� */
		PosMngInitPositionManager( BaseControls );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�[������~���[�h(�X�L�����b�Ō��o)�̎�														*/
/*--------------------------------------------------------------------------------------------------*/
	else if( BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_ZSTOP )
	{
		;
	}

}


#if 0 /* ���󖢑Ή� */
/****************************************************************************************************/
/*																									*/
/*		���[�^�G���R�[�_�`�a�b���M���쐬															*/
/*																									*/
/****************************************************************************************************/
static	void	BpxMakeMotABCphaseSignal( BASE_CTRL_HNDL *BaseControls )
{
	MENCV			*MencV;
	POS_MNG_HNDL	*PosManager;
	BASE_CTRL_IO	*BaseCtrlIo;

	MencV		= BaseControls->MencV;
	PosManager		= BaseControls->sv_ctrl_ptr->PosMngV_ptr;
	BaseCtrlIo		= &( BaseControls->scanb_io );

/*--------------------------------------------------------------------------------------------------*/
/*		�b���ʉ߃`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->ScanBMotCposOk )
	{
		if( (BaseControls->ScanBMotCphPass == TRUE) )
		{
			/* �b���o�͎��� : 750[us] */
			PosManager->MCphOutTime = ( ((750) * US_CHANGE_MS) / KPI_SBCYCLENS );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���o�͐M���쐬																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosManager->MCphOutTime > 0 )
	{
		PosManager->MCphOutTime--;
		BaseCtrlIo->PgCphase = TRUE;
	}
	else
	{
		BaseCtrlIo->PgCphase = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�`��,�a���M���쐬																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		MencV.ABphCntr	A��,B��																		*/
/*		  xxxxxx00  -->  1 , 1 																		*/
/*		  xxxxxx01	-->  1 , 0 																		*/
/*		  xxxxxx10	-->  0 , 0 																		*/
/*		  xxxxxx11	-->  0 , 1 																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MencV->ABphCntr & 0x03 )
	{
	case 0x00: BaseCtrlIo->PgAphase = TRUE;  BaseCtrlIo->PgBphase = TRUE;  break;
	case 0x01: BaseCtrlIo->PgAphase = TRUE;  BaseCtrlIo->PgBphase = FALSE; break;
	case 0x02: BaseCtrlIo->PgAphase = FALSE; BaseCtrlIo->PgBphase = FALSE; break;
	default  : BaseCtrlIo->PgAphase = FALSE; BaseCtrlIo->PgBphase = TRUE;  break;
	}
	return;
}
#endif /* #if 0 */



/***************************************** ��������A/P **********************************************/



#if 0 //(CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
/****************************************************************************************************/
/*																									*/
/*		�T�[�{����ʒu�w�ߊǗ�																		*/
/*																									*/
/****************************************************************************************************/
LONG	BpxSvcPosCommandManager( BASE_CTRL_HNDL *BaseControls, FUN_CMN_CONTROL *FnCmnCtrl )
{
	POS_MNG_HNDL	*PosManager;
	MFCTRL			*MFControl;
	LONG			dPosRefo;
	LONG			rc;
#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A���󖢑Ή��� */
	LONG			lwk_dPcmdHwCntr;
#endif

	PosManager	= &(BaseControls->PosManager);
	MFControl	= &(BaseControls->MFControl);

#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A�M�����͖��Ή��� */
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��N���A�M���Ď�																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( SVFSEL_LTM_PERCLR == 1 )
	{
		KpiChkPerClearSignal( );						/* �J�[�l���ʒu�΍��N���A�M���Ď�			*/
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���䃂�[�h�ȊO�̎�																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS )
	{
		BaseControls->PosCtrlSts.DenSignal = FALSE;
		return( 0 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���o�������M�������F���o���������W���[�����e�`�k�r�d�ɂ���B								*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->PosCtrlSts.DenSignal = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߓ��͏���(�p���X��,PJOG,etc)�FPosMngV.dPcmda	[�w�ߒP��/Scan]							*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�e�[�u���^�]���[�h																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseControls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{ /* �e�[�u���^�]�� */
#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A���󖢑Ή��� */
		BoutV.f.PerClrEdge = FALSE;							/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
		BoutK.f.PerClrEdge = FALSE;							/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
#endif
	/*------------------------------------------------------------------------------------------*/
		if( BaseControls->BeComplete == TRUE )
		{
			/* �e�[�u���^�]�ʒu�w�� */
			dPosRefo = BaseControls->TblDrive.var.dPosRef;
			/* �ʒu�΍��N���A�w�� */
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;
		}
		else
		{
#if ( SVFSEL_BLSH_COMMON_PROC )		/* <V868> �W���EY522���p	*/
			/* �o�b�N���b�V�␳���� */
			BlshInvaildCompensate( &(BaseControls->PosManager.BlshCmp) );
#endif
			/* �T�[�{����ʒu�Ǘ��ϐ������� */
			PosMngInitPositionManager( BaseControls );
			/* �ʒu�΍��N���A�v�� */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			/* ScanB�ʒu�΍��N���A */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrA), sizeof(BaseControls->PosManager.PosErrA)/4 ); 	/* <S107> */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrAmp), sizeof(BaseControls->PosManager.PosErrAmp)/4 );
			/* �ʒu����I�� */
			return( 0 );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�v���O����JOG�^�]���[�h																	*/
	/*----------------------------------------------------------------------------------------------*/
	else if( FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG )
	{ /* �v���O����JOG���[�h�� */
#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A���󖢑Ή��� */
		BoutV.f.PerClrEdge = FALSE;						/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
		BoutK.f.PerClrEdge = FALSE;						/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
#endif
	/*------------------------------------------------------------------------------------------*/
		if( FnCmnCtrl->FnCtrlMcmd != CTRLMODE_PJOG )
		{												/* ���䃂�[�h���؂�ւ�������_			*/
			PrgJogReset( &(BaseControls->PJogHdl) );		/* �v���O����JOG�^�]�f�[�^���Z�b�g		*/
		}
	/*------------------------------------------------------------------------------------------*/
		if( BaseControls->BeComplete == TRUE )
		{ /* �ʒu������s */

			/* �ʒu�w�ߍ쐬����	[�w�ߒP��/Scan]	*/
			rc = PrgJogMakePositionReference( &BaseControls->PJogHdl, &dPosRefo );

			if( rc == TRUE )
			{/* �v���O����JOG���Z�I�� */
				BaseControls->PosCtrlSts.DenSignal  = TRUE;
			}
			else
			{/* �v���O����JOG���Z�� */
				BaseControls->PosCtrlSts.DenSignal  = FALSE;
				BaseControls->PosCtrlSts.RefZSignal = FALSE;
			}
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;
		}
		else
		{
#if ( SVFSEL_BLSH_COMMON_PROC )
			/* �o�b�N���b�V�␳���� */
			BlshInitializeCompensate( &(BaseControls->PosManager.BlshCmp) );
#endif
			/* �T�[�{����ʒu�Ǘ��ϐ������� */
			PosMngInitPositionManager( BaseControls );
			/* �ʒu�΍��N���A�v�� */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			/* ScanB�ʒu�΍��N���A */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrA), sizeof(BaseControls->PosManager.PosErrA)/4 );	/* <S107> */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrAmp), sizeof(BaseControls->PosManager.PosErrAmp)/4 );
			return( 0 );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�ʏ�^�]���[�h																			*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
/* 2012.09.06 Y.Oka ���p���X���͔{���ύX�@�\���󖢑Ή��� */
#if SVFSEL_PULSE_INPUT_SWITCHING == 1
	/*------------------------------------------------------------------------------------------*/
	/*		�w�߃p���X�{������															<V677>	*/
	/*		BoutK.PcmdRingCntr�̍X�V�����́ALpxMultiplyInputPulse()���ɂĎ��{					*/
	/*------------------------------------------------------------------------------------------*/
		/* ����dPosRefo�ւ̑����Y�ꂸ�ɁI�I���� */
		lwk_dPcmdHwCntr     = LpxMultiplyInputPulse( );	/* �ʒu�w�ߍ����l�X�V[�w�ߒP��/Scan]	*/
														/* �ʒu�w�߃����O�J�E���^�X�V			*/
#else
#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A���󖢑Ή��� */
		lwk_dPcmdHwCntr = PosManager->var.dPcmdHwCntr;	/* �ʒu�w�ߍ����l�X�V[�w�ߒP��/Scan]	*/
#endif
//		BoutK.PcmdRingCntr += BinK.dPcmdInputHwCntr;	/* �ʒu�w�߃����O�J�E���^�X�V			*/
		dPosRefo = PosManager->var.dPcmdInputHwCntr;	/* �ʒu�w�ߍ����l�X�V[�w�ߒP��/Scan]	*/
#endif

#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A���󖢑Ή��� */
	/*------------------------------------------------------------------------------------------*/
	/*		�ʒu�΍��N���A����																	*/
	/*------------------------------------------------------------------------------------------*/
		BoutV.f.PerClrCmd = BinV.f.PerClrCmd;			/* �ʒu�΍��N���A�w��					*/
		if( BoutV.f.PerClrCmd )							/* �ʒu�΍��N���A�w��					*/
		{
			BpxEdgeClrSvcPositionManager( );			/* �ʒu�΍��G�b�W�N���A�ϐ�������		*/
		//	BoutV.AmondPosCmd = BinK.dPcmdHwCntr;		/* �ʒu�w�߃��j�^�p	[�w�ߒP��/Scan]		*/
			BoutV.AmondPosCmd =  lwk_dPcmdHwCntr;		/* �ʒu�w�߃��j�^�p	[�w�ߒP��/Scan]		*//*<V677>*/
			BoutV.f.PerClrEdge = FALSE;					/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
			BoutK.f.PerClrEdge = FALSE;
			return( FALSE );							/* �ʒu����I��							*/
		}
#endif

#if 0 /* 2012.09.06 Y.Oka ��INHIBIT�@�\���󖢑Ή��� */
	/*------------------------------------------------------------------------------------------*/
	/*		�p���X��ʒu�w�ߓ���																*/
	/*------------------------------------------------------------------------------------------*/
		dPosRefo = ( BinV.f.Sv.Inhibit ) ? 0 : dPosRefo;
#endif /* 2012.09.06 Y.Oka ��INHIBIT�@�\���󖢑Ή��� */

#if 0 /* 2012.09.06 Y.Oka ���ʒu�΍��N���A���󖢑Ή��� */
	/*------------------------------------------------------------------------------------------*/
	/*		�ʒu�΍��G�b�W�N���A����															*/
	/*------------------------------------------------------------------------------------------*/
		if( Iprm.f.PerClrEdge && BinK.f.PerClrEdge )
		{
			BoutV.f.PerClrEdge = TRUE;					/* �ʒu�΍��N���A�G�b�W�M���Z�b�g		*/
			BpxPerClrSignalProcedure( );				/* �ʒu�΍��G�b�W�N���A�M������			*/
			BoutK.f.PerClrEdge = TRUE;					/* FB�ʒu�E�w�߈ʒu�X�V�p�t���O			*/
		}
		else
		{
			BoutV.f.PerClrEdge = FALSE;					/* �ʒu�΍��N���A�G�b�W�M�����Z�b�g		*/
			BoutK.f.PerClrEdge = FALSE;					/* FB�ʒu�E�w�߈ʒu�X�V�p�t���O			*/
		}
	/*------------------------------------------------------------------------------------------*/
#endif
	}
	/* �ʒu�w�߃����O�J�E���^�X�V */
	BaseControls->CtrlCmdMngr.PosRef += dPosRefo;
	/* �����ʒu�w�߂̍X�V */
	PosManager->var.dPcmda = dPosRefo;


/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߓd�q�M�A�ϊ��FPosMngV.dPcmdEgear [Pulse/Scan]										*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosManager->var.EgearDisable )
	{ /* �d�q�M������ */
		PosManager->var.dPcmdEgear = PosManager->var.dPcmda;
	}
	else
	{ /* �d�q�M���L�� */
		/* �d�q�M�����Z */
		PosManager->var.dPcmdEgear = MlibPcmdEgear( PosManager->var.dPcmda,
													PosManager->Egear,
													&(PosManager->var.dPcmdEgrem) );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���o�������M�������F���o���������W���[����FALSE�ɂ���B										*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->PosCtrlSts.RefZSignal = ( PosManager->var.dPcmdEgear != 0 ) ? FALSE : TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^���Z(������,�ړ�����,�U���}��)�FBoutV.dPcmdFilo  [Pulse/Scan]				*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2012.08.07 Y.Oka �ʒu�w�߃t�B���^���󖢑Ή� */
	BaseControls->CtrlCmdMngr.dPcmdFilo = BpxPcmdFilter( PosManager->var.dPcmdEgear );
#else
	BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�g��(�e�e)���䉉�Z(�l�e�b,���ǂߐ���)														*/
/*--------------------------------------------------------------------------------------------------*/
/*		BoutV.dPosRefi [Pulse/Scan]�F�ʒu����ʒu�w�ߍ�������										*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�o�b�N���b�V�␳����																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_BACKLASHCOMP )
	if( (BaseControls->BeComplete == FALSE)
		|| (BaseControls->PhaseReady == FALSE)
		|| (BaseControls->OverTravel == TRUE) )
	{
		BlshActive = FALSE;
	}
	else
	{
		BlshActive = TRUE;
	}
	BlshCompensateBacklash( &(PosManager->BlshCmp),
							&(PosManager->conf.Egear),
							&(PosManager->var.dPcmdEgear),
							&(dPosRefo),
							&(BaseControls->PosCtrlSts.RefZSignal),
							BlshActive );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		�U���}���t�B���^����																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
	if( PosManager->var.PcmdFilStop == FALSE )
	{ /* �ʒu�w�߃t�B���^�����v���Ȃ� */
		/* �U���}���t�B���^��������������(�ʒu����ȊO�͖���) */
		if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
		{
			BaseControls->VibSupFil.conf.Pexe.enable = 0;
		}
		/* �U���}���t�B���^���Z */
		BaseControls->CtrlCmdMngr.dPcmdFilo = posMngVibSupFilter(
												&(BaseControls->VibSupFil),
												PosManager->var.dPcmdEgear,
												&(BaseControls->PosCtrlSts.RefZSignal) );
	}
	else
	{ /* �ʒu�w�߃t�B���^�����v������ */
		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
	}
#else
		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		MFC����																					*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_MFC_USE == TRUE )
	/* MFC��������������(�ʒu����ȊO�͖���) */
	if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
	{ /* �O��̐��䃂�[�h���ʒu����łȂ��ꍇ */
		/* MFC�����ɏ����� */
		MFControl->conf.MfcPexe.MFCModel = 0;
	}
	/* MFC���Z */
	dPosRefo = MfcControlMain( &(BaseControls->MFControl),
							   BaseControls->CtrlCmdMngr.dPcmdFilo,
							   &(BaseControls->CtrlCmdMngr.SpdFFCx),
							   &(BaseControls->CtrlCmdMngr.TrqFFCx) );
#else
	MFControl->conf.MfcPexe.MFCModel = 0;
	dPosRefo = BaseControls->CtrlCmdMngr.dPcmdFilo;
#endif


/*--------------------------------------------------------------------------------------------------*/
/*		����ʒu�M������																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Make Coin and Near Status */
	posMngCtrlPosSignal( PosManager, &(BaseControls->PosCtrlSts) );

	return dPosRefo;
}
#endif /* (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) */



#if 0 /* 2012.08.07 Y.Oka �ʒu�w�߃t�B���^���󖢑Ή� */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�߃t�B���^�������C���֐�																*/
/*																									*/
/****************************************************************************************************/
LONG	BpxPcmdFilter( LONG pcmdin )
{
LONG	pcmdout;
LONG	orgpcmdin;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^�I������																	*/
/*--------------------------------------------------------------------------------------------------*/
	FilMng.FilSel = Iprm.PcmdFilter;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^�ؑւ��֎~�����F�ؑւ��s���W���[�����s�q�t�d�ɂ��邱��					*/
/*--------------------------------------------------------------------------------------------------*/
	FilMng.FilChgDisable = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^���s�^��~����																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( FilMng.FilStop == TRUE )
	{
		orgpcmdin = pcmdin;
		pcmdin = 0;
	}
	else
	{
		orgpcmdin = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^�I�����[�U�萔�ɂ��t�B���^�I�������s����									*/
/*--------------------------------------------------------------------------------------------------*/
	pcmdout = PcmdMoveAvergeFilter( pcmdin );			/* �ړ����σt�B���^���Z						*/
	pcmdout = PcmdExpFilter( pcmdout );					/* �������t�B���^���Z						*/
	pcmdout += orgpcmdin;
	
	return( pcmdout );
}
#endif /* (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) */
#endif /* 2012.08.07 Y.Oka �ʒu�w�߃t�B���^���󖢑Ή� */



/****************************************************************************************************/
/*																									*/
/*		�ʒu�΍��N���A�M������																		*/
/*																									*/
/****************************************************************************************************/
void	BpxPerClrSignalProcedure( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm )
{
BASE_CTRL_HNDL *BaseControls;
LONG	lwk;

		BaseControls = BaseLoops->BaseCtrls;	
		/*------------------------------------------------------------------------------------------*/
		/*	�T�[�{����ʒu�Ǘ��ϐ�������															*/
		/*------------------------------------------------------------------------------------------*/
		BpxEdgeClrSvcPositionManager( BaseLoops );

		/*------------------------------------------------------------------------------------------*/
		/*	�e�a�ʒu�␳���Z		-> (PlsIf.c)													*/
		/*------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	�e�a�ʒu�v�Z			-> (PlsIf.c)													*/
		/*------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.ClrFbPos = BaseControls->ClrFbPos;	/* �G�b�W���͎��̂e�a�ʒu	*/
		if(Bprm->FencUse != FALSE)/* <S04D>:�ٸ�����ǉ� */
		{
			lwk = BaseControls->MotSts.FencPos - BaseControls->ClrFbPos;/* set /CLR Position 		*/
		}
		else
		{
			lwk = BaseControls->MotSts.MotPos - BaseControls->ClrFbPos;	/* set /CLR Position 		*/
		}
		MlibAposRg64dp(
					-lwk,
					&(BaseControls->PosManager.conf.Egear), 
					&(BaseControls->PosManager.PgPos), 
					&(BaseControls->MonPerrLpos[0]) );
		/*------------------------------------------------------------------------------------------*/
		/*	<S112> Set position error (Un008)														*/
		/*------------------------------------------------------------------------------------------*/
#if	0	/* �s�v�H <S14C> */
		lwk = (BaseControls->PosManager.PgPos.apos[0] - BaseControls->MonPerrLpos[0]);
		BaseControls->PosManager.var.PosErrA.per32s = -lwk;			/* set position error [ref]		*/
		BaseControls->PosManager.var.PosErrA.per32a = MlibABS(lwk);	/* set abs. position error [ref]*/
#endif
		/*------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		�T�[�{����ʒu�΍��G�b�W�N���A�ϐ�������													*/
/*																									*/
/****************************************************************************************************/
void	BpxEdgeClrSvcPositionManager( BASE_LOOPCTRLS *BaseLoops )
{
BASE_CTRL_HNDL	*BaseControls;
POSCTRL			*PosCtrl;
POS_MNG_HNDL	*PosMngV;
	
		BaseControls = BaseLoops->BaseCtrls;	
		PosCtrl = &(BaseLoops->PosCtrl);
		PosMngV = &(BaseControls->PosManager);
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{����ʒu�Ǘ��ϐ�������																*/
/*--------------------------------------------------------------------------------------------------*/
//TODO:�A�i���O���j�^�p�̈ʒu�w�߂��N���A	BoutV.AmondPosCmd  = 0;		/* �ʒu�w�߃��j�^�ϐ�		*/
		/* �b���o�͎��Ԃ��N���A���Ă����̂ŁA����ȊO�̕ϐ����N���A����								*/
		MlibResetLongMemory( &(PosMngV->var.Per64),sizeof(PosMngV->var.Per64)/4); /* <S14D>			*/
		MlibResetLongMemory( &(PosMngV->var.PosErrA), sizeof(PERRA)/4 );/* �ʒu�Ǘ��p�ϐ�������<S14C> */
		PosMngV->var.dPcmdEgear = 0;								/* �ʒu�w�ߓd�q�M���o��			*/
		PosMngV->var.dPcmdEgrem = 0;								/* �ʒu�w�ߓd�q�M�����Z�]��		*/
		PosMngV->var.dPcmda = 0;									/* �ʒu�w�ߑ����l����[�w�ߒP��]	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{����ʒu�Ǘ��ϐ�������																*/
/*--------------------------------------------------------------------------------------------------*/
		BaseControls->SettlingTime.LastdPcmda = 0;			/* �O��ʒu�w�ߑ����l[�w�ߒP��]			*/
		
		MlibResetLongMemory( &(PosMngV->BlshCmp.var), sizeof(PosMngV->BlshCmp.var)/4 );	/* �o�b�N���b�V�␳�ϐ�������		*/

		PosCtrl->V.PerIvar64[0] = 0;						/* �ʒu�ϕ��o�b�t�@�̃N���A		<S14C>	*/
		PosCtrl->V.PerIvar64[1] = 0;						/* �ʒu�ϕ��o�b�t�@�̃N���A				*/
		PosCtrl->V.LastPacOut = 0;							/* �ʒu�����O��l�̃N���A				*/
		PosCtrl->V.dPFfFilo = 0;							/* �ʒu�e�e�t�B���^�o�b�t�@�̃N���A		*/

		BaseControls->CtrlCmdMngr.dPFfFilo = 0;				/* �ʒu�����O��l�̃N���A				*/

		BaseControls->PosManager.dPosRefi = 0;				/* �ʒu�w�ߍ�������[Pulse/Scan]			*/
		BaseControls->PosManager.CompdPosRefi = 0;			/* �ʒu�w�ߍ�������(ScanB->ScanA)		*/
		BaseControls->PosManager.CompdPcmda = 0;			/* �ʒu�w�ߑ����l����(ScanB->ScanA)		*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
	{
		BaseControls->PosCtrlSts.CoinSignal = TRUE;			/* @CHK@:SGDS�ɂ��킹��					*/
		BaseControls->PosCtrlSts.NearSignal = TRUE;			/* @CHK@:SGDS�ɂ��킹��					*/
	}
	else
	{
		BaseControls->PosCtrlSts.CoinSignal = FALSE;		/* @CHK@:SGDS�ɂ��킹��					*/
		BaseControls->PosCtrlSts.NearSignal = FALSE;		/* @CHK@:SGDS�ɂ��킹��					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߊ֘A�ϐ��������F�ʒu�w�߃t�B���^,�l�e�b�Ȃ�											*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʒu�w�߃t�B���^���������� */
//	posMngInitPcmdFilter( &(BaseControls->VibSupFil) );												  /* <S065> */
	PcmdFilInitialize( &(BaseControls->PcmdFil), &(BaseControls->VibSupFil) );
	/* MFC�ϐ������� */
	MfcInitModelControl( &(BaseControls->MFControl) );

	return;

}
/***************************************** end of file **********************************************/
