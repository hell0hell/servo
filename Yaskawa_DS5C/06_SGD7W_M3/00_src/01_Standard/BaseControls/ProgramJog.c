/****************************************************************************
Description	: ProgramJog.c : �v���O����JOG�֘A���W���[��
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira 2011.03.8    ����
****************************************************************************/
//#include "SystemSetting.h"
#include "ProgramJog.h"

/****************************************************************************
		�v���O����JOG�����p�����[�^�v�Z
****************************************************************************/
/****************************************************************************
	MlibIpcalPcmdMaker()�̑�1������kxgain{kx,sx}:-2^23��kx��2^23�C0��sx��24�ł��邪�C	
	���ԕϐ���kxgain{kx,sx}:-2^23��kx��2^23�C-127��sx��127���\�ɂȂ����ꍇ��<V507>��	
	�폜����K�v������<V527>
****************************************************************************/
void	IprmcalPrgJog( PJOGHNDL *PJogHdl, BPRMDAT *Bprm, POS_MNG_HNDL *PosManager )
{
	LONG PcmdOvrSpd, kx, sx;
	LONG Egear_a, Egear_b;

	PCMKPV  *PJogPcmk;
	PJOGV	*PJogV;

	PJogV = &PJogHdl->PJogV;
	PJogPcmk = &PJogHdl->PJogPcmk;
	Egear_a = PosManager->conf.Egear.a;
	Egear_b = PosManager->conf.Egear.b;

	kx = MlibScalKxksks( 1,  Bprm->OvrSpd,    Bprm->Kfbpls,  &sx,  0 );
	kx = MlibPcalKxgain( kx, Egear_a, Egear_b, &sx, 0 );
	kx = MlibPcalKxgain( kx, Bprm->SvCycleNs, 1000000000,   &sx, 24 );
	PcmdOvrSpd = kx;

	MlibIpcalPcmdMaker( PcmdOvrSpd, Bprm->NorMaxSpd, Bprm->NorOvrSpd, FALSE, &PJogPcmk->P );
	/* <V507> add start */
	if( PcmdOvrSpd == 0x7FFFFF )
	{
		PJogV->Cnst.PrmUnMatch2 = TRUE;
	}
	/* <V507> add end */
}

/****************************************************************************
		�v���O����JOG�^�]�w�ߍ쐬�ϐ����Z�b�g
****************************************************************************/
void PrgJogReset( PJOGHNDL *PJogHdl )
{
	PJogHdl->PJogV.Var.AbsPosCmd = 0;
	MlibResetLongMemory( &PJogHdl->PJogPcmk.V, sizeof( PJogHdl->PJogPcmk.V )>>2 );
}
/****************************************************************************************************/
/*																									*/
/*		�v���O����JOG�^�]�w�߃V�[�P���X(ScanC��CALL)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���E���h����̃v���O����JOG�^�]���삩��A�萔�ݒ肳�ꂽ�^�]�p�^�[����					*/
/*			�w�߂��X�L����B�֑���B																	*/
/*																									*/
/*		 �V�[�P���X 0: �^�]�J�n�҂�																	*/
/*				���E���h����̉^�]�J�n�w�߂ɂ��A�^�]�p�^�[�����{�̃V�[�P���X						*/
/*				�����������Ȃ��B�V�[�P���X1�ցB														*/
/*																									*/
/*		 �V�[�P���X 1: �҂����Ԍo�ߑ҂�																*/
/*				�҂����Ԍo�ߌ�A�ŏ��̉^�]�w�߂��X�L����B�֑���B									*/
/*				�V�[�P���X2�ցB																		*/
/*																									*/
/*		 �V�[�P���X 2: �^�]�w�ߕ��o���҂�															*/
/*				�X�L����B����̎w�ߕ����o��������҂B												*/
/*				�w�ߕ����o�������ɂ��A�V�[�P���X3�ցB												*/
/*																									*/
/*		 �V�[�P���X 3: �^�]�I������																	*/
/*				�J�Ԃ��񐔍X�V�Ɖ^�]�񐔂̔�r����^�]���I�����������肷��B						*/
/*				�^�]�I���̏ꍇ�A�V�[�P���X5�ցB														*/
/*				�^�]���I���̏ꍇ�A�V�[�P���X4�ցB													*/
/*																									*/
/*		 �V�[�P���X 4: �^�]�p�^�[���Ɉ˂镪��														*/
/*				�^�]�p�^�[���ɏ]���āA�^�]�w�߂��X�L����B�֑���B									*/
/*				�V�[�P���X2�ցB�ُ�n�̓V�[�P���X5�ցB												*/
/*																									*/
/*		 �V�[�P���X 5: �^�]�I��																		*/
/*				������A�V�[�P���X0�ցB																*/
/*																									*/
/*		 �V�[�P���X 6: �^�]���f																		*/
/*				������A�V�[�P���X0�ցB																*/
/*																									*/
/*		 �V�[�P���X 7: �A���[��������																*/
/*				������A�V�[�P���X0�ցB																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	CpxPrgJogPosSequence( PJOGHNDL *PJogHdl, SEQ_CTRL_OUT *SeqCtrlOut,
													ALMMAN_HANDLE *AlmManager, LONG FnCtrlMcmd ) 
{
	PJOGV	*PJogV;
	PCMKPV  *PJogPcmk;

	PJogV = &PJogHdl->PJogV;
	PJogPcmk = &PJogHdl->PJogPcmk;

	/* �v���O����JOG���[�h�`�F�b�N */
	if( FnCtrlMcmd != CTRLMODE_PJOG )
	{
		return;
	}
	
	/* �v���O����JOG�^�]�����`�F�b�N						*/
	/* �A���[��������/HWBB��Ԓ�/���H�Ւf/�^�]����OT�L��	*/	
	if( ((AlmManager)->Status.AlmFlag != FALSE)			/* <S0E5> */ /* <S0FF> */
		|| (SeqCtrlOut->HwbbSts == TRUE)
		|| (SeqCtrlOut->MainPowerOn != TRUE)
		|| ((PJogV->Var.State == PJOG_START) && (SeqCtrlOut->OverTrvlSts == TRUE)) )
	{
		PJogV->Var.SeqPtr = 7;
	}
	else if( PJogV->Var.RoutCmd == PJOGCMD_ABORT )	/* �^�]���f */
	{
		PJogV->Var.SeqPtr = 6;
	}
	else if( (PJogV->Var.State == PJOG_START) && (SeqCtrlOut->BaseEnableReq == FALSE) )	/* �^�]���̃T�[�{�I�t */
	{
		PJogV->Var.SeqPtr = 6;
	}
	if( PJogV->Var.RoutCmd == PJOGCMD_INIT )
	{
		PJogV->Var.SeqPtr = 0;
		PJogV->Var.CoutCmd = PJOGCMD_INIT;
		PJogV->Var.State = PJOG_INIT;
	}

	/* �v���O����JOG�^�]�w�߃V�[�P���X */
	switch ( PJogV->Var.SeqPtr )
	{
		case 0 :	/* �^�]�������	*/
			if( (PJogV->Var.RoutCmd == PJOGCMD_START) && (SeqCtrlOut->BaseEnableReq == TRUE) )
			{
				PJogV->Var.MCntr = 0;
				KlibRstLongTimer(&PJogV->Var.TimeWork);
				if( (PJogV->Cnst.MTimes == 0) && ((PJogV->Cnst.Pattern == 2) || (PJogV->Cnst.Pattern == 3)) )
				{
					PJogV->Var.SeqPtr = 7;
				}
				else
				{
					PJogV->Var.SeqPtr = 1;
				}
				PJogV->Var.State = PJOG_START;
			}
			break;

		case 1 :	/* �^�]�҂����	*/
			if( KlibGetLongTimerMs(PJogV->Var.TimeWork) >= (ULONG)PJogV->Cnst.WaitTime )
			{
				PrgJogReset( PJogHdl );
				PJogV->Var.CoutCmd = PJOGCMD_START;
				if( PJogV->Cnst.Pattern & 0x01 )
				{
					PJogV->Var.AbsPosCmd = -PJogV->Cnst.Distance;
				}
				else
				{
					PJogV->Var.AbsPosCmd = PJogV->Cnst.Distance;
				}
				PJogV->Var.SeqPtr = 2;
			}
			break;

		case 2 :	/* �ʒu�w�ߕ��o����	*/
			if( PJogPcmk->V.calendf )
			{
				PJogV->Var.SeqPtr = 3;
			}
			break;

		case 3 :	/* �^�]�I������	*/
			if( (++PJogV->Var.MCntr >= PJogV->Cnst.MTimes) && (PJogV->Cnst.MTimes != 0) )
			{
				PJogV->Var.SeqPtr = 5;
			}
			else
			{
				KlibRstLongTimer(&PJogV->Var.TimeWork);
				PJogV->Var.SeqPtr = 4;
			}
			break;

		case 4 :	/* �^�]�p�^�[������	*/
			if( KlibGetLongTimerMs(PJogV->Var.TimeWork) >= (ULONG)PJogV->Cnst.WaitTime )
			{
				switch ( PJogV->Cnst.Pattern )
				{
					case 0 :
						PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						break;
					case 1 :
						PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						break;
					case 2 :
						if( PJogV->Var.MCntr < (PJogV->Cnst.MTimes>>1) )
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						break;
					case 3 :
						if( PJogV->Var.MCntr < (PJogV->Cnst.MTimes>>1) )
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						break;
					case 4 :
						if( PJogV->Var.MCntr & 0x01 )
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						break;
					case 5 :
						if( PJogV->Var.MCntr & 0x01 )
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						break;
					default :
						break;
				}
				if( PJogV->Cnst.Pattern < 6 )
				{
					PJogV->Var.SeqPtr = 2;
				}
				else
				{
					PJogV->Var.SeqPtr = 5;
				}
			}
			break;

		case 5 :	/* �^�]�I��	*/
			PJogV->Var.State = PJOG_END;
			PJogV->Var.CoutCmd = PJOGCMD_INIT;
			PJogV->Var.SeqPtr = 0;
			break;

		case 6 :	/* �^�]���f	*/
			PrgJogReset( PJogHdl );		/* ��~	*/
			PJogV->Var.CoutCmd = PJOGCMD_ABORT;
			PJogV->Var.State = PJOG_ABORT;
			PJogV->Var.SeqPtr = 0;
			break;

		case 7 :	/* �A���[��������	*/
			PrgJogReset( PJogHdl );		/* ��~	*/
			PJogV->Var.CoutCmd = PJOGCMD_ABORT;
			PJogV->Var.State = PJOG_ERROR;
			PJogV->Var.SeqPtr = 0;
			break;

		default :
			break;
	}

	/* ���샌�W�X�^�ݒ�	*/
	PJogHdl->PJogState = (PJogV->Var.State & 0x0F);

	/* ScanC��ON,Round��OFF����t���O	*/
	PJogV->Var.ScanCRunFlg = TRUE;
}

/****************************************************************************
		�ʒu�w�ߓ��͏���(�v���O����JOG�^�]���[�h)(ScanB��CALL)
****************************************************************************/
BOOL PrgJogMakePositionReference( PJOGHNDL *PJogHdl, LONG *dPosRefo )
{
	PJOGV	*PJogV;
	PCMKPV  *PJogPcmk;

	PJogV = &PJogHdl->PJogV;
	PJogPcmk = &PJogHdl->PJogPcmk;

	/* �ʒu�w�ߍ쐬����	[�w�ߒP��/Scan]	*/
	*dPosRefo = MlibPcmdMaker( PJogV->Var.AbsPosCmd, 0, PJogPcmk->P.maxspd, PJogPcmk, PCMKMODE_POS );
	return( (BOOL)(PJogPcmk->V.calendf ));
}

/****************************************************************************************************/

