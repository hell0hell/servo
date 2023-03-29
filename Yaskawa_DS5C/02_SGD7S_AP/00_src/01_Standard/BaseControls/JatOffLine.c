/****************************************************************************************************/
/*																									*/
/*	JatOffLine.c : �I�t���C���I�[�g�`���[�j���O���W���[��											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	�T�v�F	�I�t���C�������̃C�i�[�V��������s���B�ʒu����ł̎��s�̂��߁C�������ł�����\�B		*/
/*			�c�c���[�^�C���j�A���[�^������\														*/
/*			�C�i�[�V����́C���]�^�t�]���P�Z�b�g�ōs���B���̂��߁C									*/
/*			�C�i�[�V�����莞�̓���́C���]�^�t�](����)���t�]�^���](����)��Jrate�X�V�ƂȂ�			*/
/*																									*/
/*                         ������s(��)   ������s(�t)           J�X�V								*/
/*                       |--------------|---------------|       |-----|								*/
/*               _______________                                									*/
/*           ____|              |_______________                  ____  							*/
/*                                              |________________|									*/
/*																									*/
/*		���ӁF																						*/
/*		  ����w��																					*/
/*			�@���]���t�]�C�t�]�����]�ؑ֎��̒�~���Ԃ͂قڃ[���ł���K�v������܂��B				*/
/*			  ��~���Ԃ������ƁC�Î~���C�̉e���œ��萸�x���������܂��B								*/
/*			�A���x=500min-1�ȏ�(��i3000min-1)�C�������Ԃ́C30ms�ȏオ���z�ł��B					*/
/*			  �O�p�w�߂̏ꍇ�C������s���ʐ��x�������Ȃ�̂ŁC�o���邾���C							*/
/*			  ��葬���Ԃ�݂��������ǂ��ł��B														*/
/*			�B����l�̔��f�́C�u���芮�����[�����v�Ŕ��ʁC�w�ߒ�~���Ԃ́C300ms���x�݂��邱��		*/
/*																									*/
/*		  �T�[�{�Q�C��																				*/
/*			�@�{�����́C���������ɃC�i�[�V�����X�V���邽�߁C�o�׎��Q�C���ł�						*/
/*			  ��C�i�[�V�����̓����Kp>>Kv�ƂȂ��āC�ʒu���[�v�̐U������������B					*/
/*			  �]�������ݽ�ʒu����Ŏ��s���邪�C���m�̃C�i�[�V���𓯒肷�邽�߁CKp/Kv�������K�v		*/
/*			  �����Kp=Kv/10�CVFF=100%�Ƃ���B														*/
/*			�A�T�[�{�Q�C���́CPn100�CPn102�F���Kv�CKp���g�p����(���蒆�̃Q�C���ؑւ��s��)			*/
/*			�B�C�i�[�V������J�n���̃C�i�[�V����ݒ��Pn103�ł͂Ȃ��C��p�p�����[�^�Ƃ���			*/
/*			  �C�i�[�V����=300%�̏ꍇ�́C��100�{���ׂ܂œ���\(�������g���N�O�a���Ȃ�����)�B		*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*																									*/
/*	T.Kira 2011.03.01   ����																		*/
/****************************************************************************************************/
#include "Basedef.h"
#include "JatOffLine.h"
#include "ServoIf.h"
#include "PnPrmListTbl.h"

/****************************************************************************************************/
/*																									*/
/*		�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
static void jat_offline_st_detect( JATV *jatv, LONG ActPos);
static void jat_estimate( JATHNDL *JatHdl );
static void jat_modelcal( JATHNDL *JatHdl, CTRL_CMD_PRM *CtrlCmdPrm, LONG spdref );
static void jat_sumtrf_offline( JATHNDL *JatHdl, LONG TrqRef);


/****************************************************************************************************/
/*																									*/
/*		�I�t���C���C�i�[�V������X�L�����b����														*/
/*																									*/
/*	�T�v:	����J�n���ʁC�C�i�[�V����v�Z�C�U�����o���s���B										*/
/*																									*/
/*	arg		:																						*/
/*	g-ref	:	LONG	CoutV.UnMotSpdx			:	ScanC���[�^���x				[2^24/OvrSpd]		*/
/*				UINT	BoutV.f.Ctrl.TrqClamp	:	�g���N�������t���O								*/
/*				UINT	BinV.f.BaseEnable		:	�x�[�X�C�l�[�u���t���O							*/
/*				UINT	BoutV.f.PerClrCmd		:	�΍��N���A�t���O								*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
void CpxJatOfflineCalc( JATHNDL *JatHdl, LONG UnMotSpdx, DETVIB *DetVib, BOOL PConActFlag, BOOL PerrClrReq )
{
	LONG	wk;
//	BOOL	PConActFlag;
	JATV	*jatv;
	JATP	*jatp;

	jatv = &JatHdl->jatv;
	jatp = &JatHdl->jatp;
//	PConActFlag = ((CmdCtrlBit >> ENBPCTRL_BITNO) & 0x01);

	/*--------------------------------------------------------------------------*/
	/*	�@�\����																*/
	/*--------------------------------------------------------------------------*/
	if (jatv->enable == TRUE)				/* �C�i�[�V������L��				*/
	{
		/*----------------------------------------------------------------------*/
		/*	����G���[���o														*/
		/*----------------------------------------------------------------------*/
		wk = jatv->mnfb - UnMotSpdx;			/* ���f�����x �| �e�a���x		*/
		jatv->jverr = MlibLpfilter1(wk, jatp->jverrfil, jatv->jverr);

		if((DetVib->conf.JstVibCheckLevel << 1) < MlibABS(jatv->jverr))
		{	/* �G���[���o���x������	*/
			jatv->status |= JATERR;				/* �G���[�X�e�[�^�X�Z�b�g		*/
		}

		if( ( PConActFlag == TRUE) || ( PerrClrReq == TRUE ))
		{	/* ��ᐧ�䒆 or �΍��N���A�� */
			jatv->status |= JATEXEERR;			/* ���s�G���[�X�e�[�^�X�Z�b�g	*/
		}
		/*----------------------------------------------------------------------*/
		/*	����J�n����														*/
		/*----------------------------------------------------------------------*/
		jat_offline_st_detect(jatv, jatv->fbsum);

		/*----------------------------------------------------------------------*/
		/*	����C�i�[�V����v�Z												*/
		/*----------------------------------------------------------------------*/
		jat_estimate( JatHdl );
	}
}

/****************************************************************************************************/
/*																									*/
/*		�I�t���C���C�i�[�V������J�n�^�I������														*/
/*																									*/
/*	�T�v:�e�a�ʒu���Ď����ăC�i�[�V�����菈���̎��s�J�n�^�I���t���O���쐬����						*/
/*		 �܂��C��C�i�[�V�����̓��菉��̐U�������o����B�U�����o�����瓯��𒆎~���C				*/
/*		 �G���[�X�e�[�^�X��Ԃ��B																	*/
/*																									*/
/*	arg		:	LONG	ActPos				: ���ړ���		[�w�ߒP��]								*/
/*	g-ref	:	UINT	CoutV.f.Sv.MotStop	: ���[�^��~�t���O										*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_offline_st_detect( JATV *jatv, LONG ActPos )
{
	LONG	wk1, wk2;

	wk1 = MlibABS(jatv->TargetDist);				/* �ݒ�ړ���	[�w�ߒP��]						*/
	wk2 = MlibABS(ActPos);							/* ���ړ���		[�w�ߒP��]						*/

	switch(jatv->jarte_start)
	{
	case JATINITIAL:	/* �C�i�[�V������(�g���N�ώZ)�����s	*/
		if(wk1 <= wk2)								/* ����J�n����̈ړ��ʂ��ݒ�l�̂P�^�Q�ȏ�		*/
		{
			jatv->jarte_start = JATCWSTART;			/* ���]���t�]�C�i�[�V��������sON				*/
		}
		break;
	case JATCWSTART:	/* ���]���t�]�C�i�[�V������(�g���N�ώZ)���s��	*/
		if(wk1 >= wk2)								/* ����J�n����̈ړ��ʂ��ݒ�l�̂P�^�Q�Ȉȉ�	*/
		{
			jatv->jarte_start = JATCCWSTART;		/* �t�]�����]�C�i�[�V��������sON				*/
		}
		break;
	case JATCCWSTART:	/* �t�]�����]�C�i�[�V������(�g���N�ώZ)���s��	*/
		if(wk1 <= wk2)								/* ����J�n����̈ړ��ʂ��ݒ�l�̂P�^�Q�ȏ�		*/
		{
			jatv->jarte_start = JATSUMEND;			/* �g���N�ώZ���s����							*/
		}
		break;
	case JATSUMEND:		/* �g���N�ώZ����	*/
		if(wk1 >= wk2)								/* ����J�n����̈ړ��ʂ��ݒ�l�̂P�^�Q�ȉ�		*/
		{
			jatv->jarte_start = JATEXEEND;			/* �C�i�[�V��������s����						*/
		}
		break;
	case JATEXEEND:		/* �C�i�[�V�����蓮�슮��		*/
		/* �������Ȃ�	*/
	default:
		break;
	}
}

/****************************************************************************************************/
/*																									*/
/*		�C�i�[�V���v�Z																				*/
/*																									*/
/*	�T�v:�C�i�[�V���𐄒肵�C�C�i�[�V�����K���ɐݒ肷��B											*/
/*			�{�����̓��A���^�C������v������Ȃ����߁C�X�L�����b�ōs��								*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_estimate( JATHNDL *JatHdl )
{
	LONG	wk,wk1,wk2,s;
	JATV	*jatv;
	JATP	*jatp;

	jatv = &JatHdl->jatv;
	jatp = &JatHdl->jatp;

	if(jatv->jarte_start == JATEXEEND )
	{	/* �g���N�ώZ����	*/
		/*----------------------------------------------------------------------*/
		/* �C�i�[�V������l�v�Z 												*/
		/*		�d�͎��Ȃǈ��O��������ꍇ�Cjat.j0�Cj2�͕��l�����ꍇ������	*/
		/*		���t�]�̕��ς���邱�Ƃň��O���̉e�����������Ă���			*/
		/*----------------------------------------------------------------------*/
		/* ���]���t�]����	*/
		if(jatv->msum1 != 0)
		{
			s = 0;
			wk1 = MlibPcalKxgain(jatv->sum1, (jatp->mjrate << JSHBITNUM), jatv->msum1, &s, 24);
			jatv->j0 = MlibKsgain2Long(wk1);
			jatv->status |= JATCWCOMP;								/* ���]���t�]�C�i�[�V��������s����	*/
		}
		/* �t�]�����]����	*/
		if(jatv->msum2 != 0)
		{
			s = 0;
			wk1 = MlibPcalKxgain(jatv->sum2, (jatp->mjrate << JSHBITNUM), jatv->msum2, &s, 24);
			jatv->j2 = MlibKsgain2Long(wk1);
			jatv->status |= JATCCWCOMP;								/* �t�]�����]�C�i�[�V��������s����	*/
		}
	}

	/* ���j�^�̍X�V �C�i�[�V���� */
	if((jatv->status & JATCOMP) == JATCOMP)
	{	/* ���������芮��	*/
		wk = (jatv->j0 + jatv->j2) >> 1;							/* ���C�i�[�V�����ϒl�Z�o			*/

		/* �㉺���`�F�b�N	*/
		wk1  = (LONG)( pndef_jrate.UpperLimit + 100 );
		wk2  = (LONG)( pndef_jrate.LowerLimit + 100 );
		wk  = MlibLimitul(wk, wk1, wk2);
		jatv->jrate = (USHORT)(wk - 100);							/* �C�i�[�V����Z�o					*/

		JatHdl->EstimatJrat = jatv->jrate;							/* ����l�Z�b�g						*/

		jatv->status |= JATJRATECOMP;								/* �C�i�[�V�����芮���X�e�[�^�X		*/
	}
}

/****************************************************************************************************/
/*																									*/
/*		�I�t���C���C�i�[�V������X�L�����a����														*/
/*																									*/
/*	�T�v:	���f���v�Z�C�g���N�ώZ���s���B															*/
/*			���f���ւ̑��x�w�ߓ��͂͑��x�e�e���܂ށC�g���N�w�ߓ��͂̓t�B���^��g���N�w�߂��g�p		*/
/*																									*/
/*	arg		:																						*/
/*	g-ref	:	LONG	AoutV.SpdRefi	:	���x�w��				[2^24/OvrSpd]					*/
/*				LONG	BoutV.SpdFFCx	:	���x�e�e�⏞			[2^24/OvrSpd]					*/
/*				LONG	BoutV.dPosFbki	:	�ʒu�e�a����			[Pulse/Scan]					*/
/*				LONG	AoutV.TrqRefo_a	:	�t�B���^��g���N�w��	[2^24/MaxTrq]					*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
void BpxJatOfflineCalc( JATHNDL	*JatHdl, EGEAR *Egear, CTRL_CMD_PRM *CtrlCmdPrm, 
														CTRL_CMD_MNG *CtrlCmdMngr, LONG	TrqRefo_a )
{
	LONG spdref;
	JATV *jatv;
	
	jatv = &JatHdl->jatv;

	/*--------------------------------------------------------------------------*/
	/*	�@�\����																*/
	/*--------------------------------------------------------------------------*/
	if (jatv->enable == TRUE)				/* �C�i�[�V������L��				*/
	{
		//BoutV.f.InertiaTuning = TRUE;		/* �C�i�[�V�����蒆					*/
		/*----------------------------------------------------------------------*/
		/*	�p�����[�^�Z�b�g�v����TRUE�̂Ƃ��́A�p�����[�^���Z�b�g				*/
		/*----------------------------------------------------------------------*/
		if( jatv->JatPrmSetReq == TRUE )
		{
			MlibCopyLongMemory( &JatHdl->jatp, &JatHdl->jatp_wk, sizeof( JatHdl->jatp )>>2 );
			jatv->JatPrmSetReq = FALSE;
		}

		spdref = CtrlCmdMngr->SpdRefi + CtrlCmdMngr->SpdFFCx;		/* <S160>	*/
//		spdref = CtrlCmdMngr->SpdRefo;

		/*----------------------------------------------------------------------*/
		/*	���[�^���f���v�Z													*/
		/*----------------------------------------------------------------------*/
		jat_modelcal( JatHdl, CtrlCmdPrm, spdref );

		/*----------------------------------------------------------------------*/
		/*	�g���N�ώZ															*/
		/*----------------------------------------------------------------------*/
		jat_sumtrf_offline( JatHdl, TrqRefo_a );
	}
	//else
	//{
		//BoutV.f.InertiaTuning = FALSE;	/* �C�i�[�V�����薢���s				*/
	//}
	/*--------------------------------------------------------------------------*/
	/*	����J�n��t�B�[�h�o�b�N�ʒu�v�Z										*/
	/*--------------------------------------------------------------------------*/
	jatv->fbsum += MlibEgearRvscnv( CtrlCmdMngr->dPosFbki,
									Egear,
									&jatv->fbsumrem );	/*  ���ʒu	[�w�ߒP��]	*/
}

/****************************************************************************************************/
/*																									*/
/*		���f���v�Z																					*/
/*																									*/
/*	�T�v:���x�w�߂���C����̃C�i�[�V�����f���̉������v�Z����B										*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_modelcal( JATHNDL *JatHdl, CTRL_CMD_PRM *CtrlCmdPrm, LONG spdref )
{
	LONG	wk1, wk2, wk3, wk4;
	JATV	*jatv;
	JATP	*jatp;

	jatv = &JatHdl->jatv;
	jatp = &JatHdl->jatp;

	jatv->mvref = spdref; 		/* ���x�w�ߓ���->���f�����x�w�� */
#if (FLOAT_USE==TRUE)
	/* ���xFB�v�Z */
	jatv->mnfb  = MlibIntegral( jatv->mtrf , jatp->mkj, jatv->mnfbwrk );
	/* ���x���� */
	wk1 = (LONG)( (float)jatv->mvref * CtrlCmdPrm->PI_rate);
	jatv->mnerp = wk1 - jatv->mnfb;
	wk3 = jatv->mnerp * jatp->mkv;

	jatv->mneri = jatv->mvref - jatv->mnfb;
	wk2 = MlibIntegral(jatv->mneri, jatp->mkvi, jatv->mneribf);
	wk4	= wk3 + wk2;

	/* ���f���g���N */
	jatv->mtrf = MlibLpfilter1(wk4, jatp->mklpf, jatv->mtrf);
#else
	/* ���xFB�v�Z */
	jatv->mnfb  = MlibIntegral( jatv->mtrf , jatp->mkj, jatv->mnfbwrk );
	/* ���x���� */
	wk1 = MlibMulgainNolim( jatv->mvref, CtrlCmdPrm->PI_rate );
	jatv->mnerp = wk1 - jatv->mnfb;
	wk3 = MlibMulgain(jatv->mnerp, jatp->mkv);

	jatv->mneri = jatv->mvref - jatv->mnfb;
	wk2 = MlibIntegral(jatv->mneri, jatp->mkvi, jatv->mneribf);
	wk4	= wk3 + wk2;

	/* ���f���g���N */
	jatv->mtrf = MlibLpfilter1(wk4, jatp->mklpf, jatv->mtrf);
#endif /* FLOAT_USE */
}

/****************************************************************************************************/
/*																									*/
/*		�I�t���C���p�g���N�ώZ																		*/
/*																									*/
/*	�T�v:�C�i�[�V������p�Ƀ��f�����Z�ƃg���N�̐ώZ������B											*/
/*																									*/
/*	arg		:	LONG	TrqRef	�F�g���N�w��						[2^24/MaxTrq]					*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_sumtrf_offline( JATHNDL *JatHdl, LONG TrqRef )
{
	JATV	*jatv;
	jatv = &JatHdl->jatv;

	if(jatv->jarte_start == JATCWSTART)
	{	/* ���]���t�]����J�n	*/
		jatv->sum1  += (TrqRef >> JSHBITNUM);
		jatv->msum1 += jatv->mtrf;
		if( (MlibABS(jatv->sum1) > 0x7D000000) || (MlibABS(jatv->msum1) > 0x7D000000) )
		{
			jatv->sum1 = (jatv->sum1 >> 1);
			jatv->msum1 = (jatv->msum1 >> 1);
		}
	}
	else if(jatv->jarte_start == JATCCWSTART)
	{	/* �t�]�����]����J�n	*/
		jatv->sum2  += (TrqRef >> JSHBITNUM);
		jatv->msum2 += jatv->mtrf;
		if( (MlibABS(jatv->sum2) > 0x7D000000) || (MlibABS(jatv->msum2) > 0x7D000000) )
		{
			jatv->sum2 = (jatv->sum2 >> 1);
			jatv->msum2 = (jatv->msum2 >> 1);
		}
	}
}

