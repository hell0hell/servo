/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ModelControl.c : �_�C���N�g�g���N���t�@�����X�������f���Ǐ]����֘A������`					*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���f���Ǐ]����֘A����																	*/
/*																									*/
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
#include "ModelControl.h"
#include "MLib.h"

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	MfcInitModelControl( MFCTRL *MFControl );												*/
/*	LONG	MfcControlMain( MFCTRL *MFControl, EXP_CTRL_DATA *hExpCtrl,								*/
/*														BASE_CTRL_HNDL *pBaseCtrl, LONG dPcmd );	*/
/*	SHORT	MfcGetParamEnableSts( MFCTRL *MFControl );												*/
/*	void	MfcSetMfcCtrlSwitch( MFCTRL *MFControl, BOOL Switch );									*/
/****************************************************************************************************/
static	void	mfcControlOneMass( MFCTRL *MFControl, LONG dPcmd );
static	void	mfcControlTwoMass( MFCTRL *MFControl, LONG dPcmd );
static	LONG	mfcDtrCmdFilter( MFCTRL *MFControl, LONG pcmd );
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		�l�e�b�ϐ�������																			*/
/*																									*/
/****************************************************************************************************/
void	MfcInitModelControl( MFCTRL *MFControl )
{
	MlibResetLongMemory( &(MFControl->var), sizeof(MFControl->var)/4 );
	/* �p�����[�^�������͑�1�Q�C�� */
	MFControl->conf.MfcPrm[0] = MFControl->conf.MfcPrm[1];
	/* ���s�p�p�����[�^������ */
	MFControl->conf.MfcPexe = MFControl->conf.MfcPrm[0];
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�l�e�b���Z																					*/
/*																									*/
/*	�T�v:	�p�����[�^�ݒ�ɉ����Ċe�K�̓��f���̂l�e�b���Z���s��									*/
/*			�p�����[�^���ύX���ꂽ���܂��́C�@�\OFF->ON�C�@�\ON->OFF���ꂽ�Ƃ��́C					*/
/*			�u�ʒu�w�ߊ����v���u���܂���ٽ�Ȃ��v�Ő؂�ւ���B�ʒu����h�~�C�V���b�N�ጸ�̂���	*/
/*			�l�e�b�����O�ɐU���}���t�B���^�������s���B�l�e�b�ւ̓��͂͐U���}���t�B���^��ʒu�w�߁B	*/
/*																									*/
/*																									*/
/* arg		: LONG	dPcmd				:	�ʒu�����w��		[pulse]								*/
/* out		: LONG	pcmdout				:	�l�e�b��ʒu�w��	[pulse]								*/
/* g-out	: LONG	BoutV.SpdFFCx 		:	���x�e�e�⏞		[2^24/OvrSpd]						*/
/*			  LONG	BoutV.TrqFFCx		:	�g���N�e�e�⏞		[2^24/MaxTrq]						*/
/*																									*/
/****************************************************************************************************/
LONG	MfcControlMain( MFCTRL *MFControl, LONG dPcmd, LONG *SpdFFCx, LONG *TrqFFCx )
{
	LONG	pcmdout;

/*--------------------------------------------------------------------------------------------------*/
/*	�l�e�b��������������(MFC��~�t���O)																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MFControl->var.MfcStopReq == TRUE )
	{
		MFControl->conf.MfcPrm[0].MFCModel = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�w�ߊ����������o�������`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (dPcmd == 0)
		&& (MFControl->var.MfcPosErr == 0)
		&& (MFControl->var.FCmd[0] == 0)
		&& (MFControl->var.FCmd[1] == 0)
		&& (MFControl->var.FCmd[2] == 0) )
	{ /* �w�ߊ��� && ���f���ʒu�΍��Ȃ�->�����o������ */
		/* �p�����[�^�ؑւ����s */
		MFControl->conf.MfcPexe = MFControl->conf.MfcPrm[0];
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�l�e�b���Z																						*/
/*--------------------------------------------------------------------------------------------------*/
	/* MFC�����J�E���^�X�V */
	MFControl->var.execnt++;

	switch( MFControl->conf.MfcPexe.MFCModel )
	{
	case ONE_MASS:		/*	���̌n���f��			*/
		mfcControlOneMass( MFControl, dPcmd );
		MFControl->var.CurMFCModel = MFControl->conf.MfcPexe.MFCModel;
		break;
	case TWO_MASS1:		/*	�Q�����n���f��			*/
	case TWO_MASS2:		/*	�Q�����n���f��			*/
	case ONE_BASE:		/*	���̌n�{�@�䃂�f��		*/
		mfcControlTwoMass( MFControl, dPcmd );
		MFControl->var.CurMFCModel = MFControl->conf.MfcPexe.MFCModel;
		break;
	default:
		MFControl->conf.MfcPexe.MFCModel = 0;
		MFControl->var.CurMFCModel = 0;
		break;
	}

	if( MFControl->conf.MfcPexe.MFCModel != 0 )
	{ /* ���f���Ǐ]�L�� */
/*--------------------------------------------------------------------------------------------------*/
/*		���x�e�e�⏞, �g���N�e�e�⏞�l�։��Z														*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MFControl->var.PffCmd != 0) || (MFControl->conf.MfcPexe.MFCType == SGDX_TYPE) )
		{	/* �ʒu�w�߂�����Ƃ��݂̂e�e�L�� �܂��� MFC�����^�C�v(SGDX�݊�)�̏ꍇ�e�e�L�� */
			*SpdFFCx += MFControl->var.VffCmd;				/* �l�e�b���x�e�e���Z		*/
			*TrqFFCx += MFControl->var.TffCmd;				/* �l�e�b�g���N�e�e���Z		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���܂�p���X���Z	��(�ʒu�w�߁|�l�e�b��̈ʒu�w��)										*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		MFControl->var.MfcPosErr = FlibPerrcalx( dPcmd, MFControl->var.PffCmd, MFControl->var.Per64 );
#else
		MFControl->var.MfcPosErr = MlibPerrcalx( dPcmd, MFControl->var.PffCmd, MFControl->var.Per64 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߏo��																				*/
/*--------------------------------------------------------------------------------------------------*/
		pcmdout = MFControl->var.PffCmd;
	}
	else
	{ /* ���f���Ǐ]���� */
		MlibResetLongMemory( &(MFControl->var), sizeof(MFControl->var)/4 );
		MFControl->var.PffCmd = 0;
		MFControl->var.VffCmd = 0;
		MFControl->var.TffCmd = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߏo��																				*/
/*--------------------------------------------------------------------------------------------------*/
		pcmdout = dPcmd;								/* �@�\�������͎w�ߓ��͂����̂܂܏o��		*/
	}

	return pcmdout;
}



/****************************************************************************************************/
/*																									*/
/*		���̂l�e�b���Z																				*/
/*																									*/
/*	�T�v:	���̂l�e�b���Z���s��																	*/
/*			�Q���t�B���^�~�P�i��ʂ��C�l�e�b��ʒu�w�߂��쐬�B										*/
/*			�e�e�p�P�K�����́C����\�t�o�̂��߁C�ʂ̂Q���t�B���^���g�p����							*/
/*																									*/
/* arg		: LONG	dPcmd		:�ʒu�����w��	[pluse]												*/
/* out		: 																						*/
/* g-out	: 																						*/
/*																									*/
/****************************************************************************************************/
static	void	mfcControlOneMass( MFCTRL *MFControl, LONG dPcmd )
{
	LONG	wk;
	KSGAIN	wk_cmd;
	LONG	tffcmd;

	MFControl->var.PCmdsub = dPcmd;							/* �ʒu�w�߃Z�b�g						*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�����t�B���^���Z	���̌n�F�Q���t�B���^												*/
	/*----------------------------------------------------------------------------------------------*/
	/* ���̃��f�� �� �Q���t�B���^�~�P */
	/* FCmd(k-1) */
	MFControl->var.FCmd[1] = MFControl->var.FCmd[0];
#if (FLOAT_USE==TRUE)
	/* �ʒu���S���t�B���^�Ƃ��� */
	wk_cmd = FlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 ); //todo
	/* FCmd(k) */
	MFControl->var.FCmd[0] = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#else
	/* �ʒu���S���t�B���^�Ƃ��� */
	wk_cmd = MlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 );
	/* FCmd(k) */
	MFControl->var.FCmd[0] = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#endif /* FLOAT_USE */


	/* �P�K�����l�Z�o �� �P�K�����p�Q���t�B���^ */
	/* �P�K�����̕���\UP */
#if (FLOAT_USE==TRUE)
	wk_cmd = wk_cmd * MFControl->conf.MfcPexe.Fsft;
#else
	wk_cmd = MlibMulgain32( wk_cmd, MFControl->conf.MfcPexe.Fsft );
#endif /* FLOAT_USE */
	/* �P�K�����l dFCmd(k) */
	MFControl->var.dFCmd[0] = mfcDtrCmdFilter( MFControl, wk_cmd );
#if (FLOAT_USE==TRUE)
	/* ����\UP FCmd(k) */
	MFControl->var.FCmdF = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 ); //todo
#else
	/* ����\UP FCmd(k) */
	MFControl->var.FCmdF = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 );
#endif /* FLOAT_USE */

	/* �ʒu�f�[�^�擾 */
	MFControl->var.FCmdFilo[1] = MFControl->var.FCmdFilo[0];
	MFControl->var.FCmdFilo[0] = MFControl->var.FCmdF;

	/*----------------------------------------------------------------------------------------------*/
	/*		���J�W�����Z																			*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/* ����\��߂� */
	wk = MFControl->var.dFCmd[0] * MFControl->conf.MfcPexe.InvFsft;
	/* TrqRef(k) = dFCmd(k) * Kj1 */
	MFControl->var.TrqRef = wk * MFControl->conf.MfcPexe.Kj1;
#else
	/* ����\��߂� */
	wk = MlibMulgain30( MFControl->var.dFCmd[0], MFControl->conf.MfcPexe.InvFsft );
	/* TrqRef(k) = dFCmd(k) * Kj1 */
	MFControl->var.TrqRef = MlibMulgain( wk, MFControl->conf.MfcPexe.Kj1 );
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	/*		�e�e�w�߉��Z																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* Pfbk(k) = FCmd(k-1) */
	MFControl->var.Pfbk = MFControl->var.FCmd[1];
	/*	�ʒu�e�e�w�� */
	MFControl->var.PffCmd = MFControl->var.Pfbk;
	/*----------------------------------------------------------------------------------------------*/
	if( MFControl->conf.MfcPexe.MFCType == SGDX_TYPE )
	{ /* MFC�����^�C�v(SGDX�݊�) ����\�͂��̂܂� */
		/* Vref(k) = FCmd(k-1) */
		MFControl->var.Vref = MFControl->var.FCmd[1];
	}
	else
	{ /* ����\UP�������xFF */
		/* Vref(k) = FCmd(k-1) */
		MFControl->var.Vref = MFControl->var.FCmdFilo[1];
	}

	/*	���x�e�e�w�� */
#if (FLOAT_USE==TRUE)
	MFControl->var.VffCmd = MFControl->var.Vref * MFControl->conf.MfcPexe.Kvff;
#else
	MFControl->var.VffCmd = MlibMulgain( MFControl->var.Vref, MFControl->conf.MfcPexe.Kvff );
#endif /* FLOAT_USE */

	/* �w�ߕ������� �� �g���NFF�Q�C���ؑւ� */
	if( MFControl->var.PCmdsub > 0 )
	{
		MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff1;
	}
	else if( MFControl->var.PCmdsub < 0 )
	{
		MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff2;
	}

	/* �g���N�e�e�w�� */
	MFControl->var.FricCmpTrq = 0;

#if (FLOAT_USE==TRUE)
	tffcmd = (MFControl->var.TrqRef * MFControl->var.Ktffcmdx ) + MFControl->var.FricCmpTrq;
	MFControl->var.TffCmd = FlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#else
	tffcmd = MlibMulgain( MFControl->var.TrqRef, MFControl->var.Ktffcmdx ) + MFControl->var.FricCmpTrq;
	MFControl->var.TffCmd = MlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		�񊵐��C���́{�@��l�e�b���Z																*/
/*																									*/
/*	�T�v:	�񊵐��l�e�b���Z���s��																	*/
/*			�Q���t�B���^�~�Q�i��ʂ��C�l�e�b��ʒu�w�߂��쐬�B										*/
/*			�e�e�p�P�K�����́C����\�t�o�̂��߁C�ʂ̂Q���t�B���^���g�p����							*/
/*																									*/
/* arg		: LONG	dPcmd		:�ʒu�����w��	[pluse]												*/
/* out		: 																						*/
/* g-out	: 																						*/
/*																									*/
/****************************************************************************************************/
static	void	mfcControlTwoMass( MFCTRL *MFControl, LONG dPcmd )
{
	LONG	wk;
	KSGAIN 	wk_cmd;
	LONG	posref, posref1, posref2;
	LONG	posref_sft;
	LONG	trqrf1;
	LONG	tffcmd;

/*--------------------------------------------------------------------------------------------------*/
/*		DTR 250us Cycle	Control																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MFControl->var.execnt & 0x0001) == 1 )
	{
/*--------------------------------------------------------------------------------------------------*/
/*	�O���T�C�N���̏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		/* ����T�C�N���ʒu�w�߂̕ۑ� */
		MFControl->var.PCmdsub = dPcmd;

	/*----------------------------------------------------------------------------------------------*/
	/*		���J�W�����Z																			*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		/* ����\��߂� */
		wk = MFControl->var.dFCmd[1] * MFControl->conf.MfcPexe.InvFsft;
		/* ����\��߂� */
		trqrf1 = MFControl->var.d3FCmd * MFControl->conf.MfcPexe.Kj3;
		/* TrqRef(k) = ( dfCmd(k-1) + d3FCmd(k)*Kj3 ) * Kj1 */
		MFControl->var.TrqRef = ( wk + trqrf1) * MFControl->conf.MfcPexe.Kj1;
#else
		/* ����\��߂� */
		wk = MlibMulgain30( MFControl->var.dFCmd[1], MFControl->conf.MfcPexe.InvFsft);

		/* ����\��߂� */
		trqrf1 = MlibMulgain30( MFControl->var.d3FCmd, MFControl->conf.MfcPexe.Kj3 );
		/* TrqRef(k) = ( dfCmd(k-1) + d3FCmd(k)*Kj3 ) * Kj1 */
		MFControl->var.TrqRef = MlibMulgain( wk + trqrf1, MFControl->conf.MfcPexe.Kj1 );
#endif /* FLOAT_USE */

		/* ����\��߂� */
		posref = MlibPfbkxremNolim( MFControl->var.d2FCmd[1], MFControl->conf.MfcPexe.Kj2, &MFControl->var.wkrem1 );
		posref_sft = MlibPfbkxremNolim( MFControl->var.d2FCmd[1], MFControl->conf.MfcPexe.Kj2_sft, &MFControl->var.wkrem2 );

		if( MFControl->conf.MfcPexe.Fccflag )
		{ /* Full-Closed && 2-mass */
			posref1 = 0;
		}
		else
		{
			posref1 = posref;
		}

		if(MFControl->conf.MfcPexe.MFCType == SGDX_TYPE)
		{ /* MFC�����^�C�v(SGDX�݊�) ����\�͂��̂܂� */
			if( MFControl->conf.MfcPexe.PSFccflag )
			{/* PS-Full-Closed && 2-mass */
				posref2 = 0;
			}
			else
			{
				posref2 = posref;
			}
		}
		else
		{ /* ����\UP���Čv�Z */
			if( MFControl->conf.MfcPexe.PSFccflag )
			{ /* PS-Full-Closed && 2-mass */
				posref2 = 0;
			}
			else
			{
				posref2 = posref_sft;
			}
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		���������o������																		*/
	/*----------------------------------------------------------------------------------------------*/
		if( (posref1 == 0)
			&& (MFControl->var.FCmd[0] == MFControl->var.FCmd[1])
			&& (MFControl->var.PosSum[0] != 0) )
		{ /* ����ʒu�w�߉��Z�l��0 && 1�K�����l��0 && �ώZ�l��0 */
			if( MFControl->var.PosSum[0] > 0 )
			{
				posref1 = -1;
			}
			else
			{
				posref1 = 1;
			}
		}
		/* �ʒu�w�߉��Z�l�ώZ */
		wk = MlibPerrcalx( posref1, 0, MFControl->var.PosSum );

	/*----------------------------------------------------------------------------------------------*/
	/*		�e�e�w�߉��Z																			*/
	/*----------------------------------------------------------------------------------------------*/
		/* �ʒu�e�e�w�� Pfbk(k-1) */
		MFControl->var.PffCmd = MFControl->var.Pfbk - MFControl->var.PffCmd;
		/* Pfbk(k) */
		MFControl->var.Pfbk = posref1 + MFControl->var.FCmd[2];
	/*----------------------------------------------------------------------------------------------*/
		if(MFControl->conf.MfcPexe.MFCType == SGDX_TYPE)
		{ /* MFC�����^�C�v(SGDX�݊�) ����\�͂��̂܂� */
			/* Vref(k) = d2FCmd[1]*Kj2 + FCmd(k-2) */
			MFControl->var.Vref = posref2 + MFControl->var.FCmd[2];
		}
		else
		{ /* ����\UP�������xFF */
			/* Vref(k) = d2FCmd[1]*Kj2 + FCmd(k-2) */
			MFControl->var.Vref = posref2 + MFControl->var.FCmdFilo[2];
		}
		/* ���x�e�e�w�� Vref(k-1)*Kvff *//* VFF�𔼃T���v���i�߂� */
#if (FLOAT_USE==TRUE)
		MFControl->var.VffCmd = MFControl->var.Vref * MFControl->conf.MfcPexe.Kvff;
#else
		MFControl->var.VffCmd = MlibMulgain( MFControl->var.Vref, MFControl->conf.MfcPexe.Kvff );
#endif /* FLOAT_USE */

		/* �w�ߕ������� �� �g���NFF�Q�C���ؑւ� */
		if( MFControl->var.PCmdsub > 0 )
		{
			MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff1;
		}
		else if( MFControl->var.PCmdsub < 0 )
		{
			MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff2;
		}

		/* �g���N�e�e�w�� */
#if (FLOAT_USE==TRUE)
		MFControl->var.FricCmpTrq = 0;
		tffcmd = (MFControl->var.TrqRef * MFControl->var.Ktffcmdx) + MFControl->var.FricCmpTrq;
		MFControl->var.TffCmd = FlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#else
		MFControl->var.FricCmpTrq = 0;
		tffcmd = MlibMulgain(MFControl->var.TrqRef, MFControl->var.Ktffcmdx) + MFControl->var.FricCmpTrq;
		MFControl->var.TffCmd = MlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#endif /* FLOAT_USE */
		}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*	�㔼�T�C�N���̏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		/* �O��ƍ�������̈ʒu�w�߂����Z */
		MFControl->var.PCmdsub = MFControl->var.PCmdsub + dPcmd;

	/*----------------------------------------------------------------------------------------------*/
	/*	�ʒu�w�ߌv�Z																				*/
	/*----------------------------------------------------------------------------------------------*/
		/* ���Z�����ϊ� */
		MFControl->var.PffCmd = (MFControl->var.Pfbk >> 1);
	/*----------------------------------------------------------------------------------------------*/
	/*	���x�e�e�w�ߌv�Z																			*/
	/*----------------------------------------------------------------------------------------------*/
		/*	���x�e�e�w�� */
#if (FLOAT_USE==TRUE)
		MFControl->var.VffCmd = MFControl->var.Vref * MFControl->conf.MfcPexe.Kvff;
#else
		MFControl->var.VffCmd = MlibMulgain( MFControl->var.Vref, MFControl->conf.MfcPexe.Kvff );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		�����t�B���^���Z	�Q�����n�F�Q���t�B���^�~�Q											*/
	/*----------------------------------------------------------------------------------------------*/
	/* �Q�������f�� �� �Q���t�B���^�~�Q */
		/* FCmd(k-2) */
		MFControl->var.FCmd[2] = MFControl->var.FCmd[1];
		/* FCmd(k-1) */
		MFControl->var.FCmd[1] = MFControl->var.FCmd[0];
		/* FCmd(k) */
#if (FLOAT_USE==TRUE)
		wk_cmd = FlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 );
		MFControl->var.FCmd[0] = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#else
		wk_cmd = MlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 );
		MFControl->var.FCmd[0] = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#endif /* FLOAT_USE */

	/* �P�K�����l�Z�o �� �Q���t�B���^�~�P�{�P�K�����p�Q���t�B���^ */
		/* �P�K�����l dFCmd(k-1) */
		MFControl->var.dFCmd[1] = MFControl->var.dFCmd[0];
#if (FLOAT_USE==TRUE)
		/* �P�K�����̕���\UP */
		wk_cmd = MFControl->var.PCmdsub * MFControl->conf.MfcPexe.Fsft;
#else
		/* �P�K�����̕���\UP */
		wk_cmd = MlibMulgain32(MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Fsft);
#endif /* FLOAT_USE */
		
		/* �P�K�����l dFCmd(k) */
#if (FLOAT_USE==TRUE)
		wk_cmd = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk3 );
		MFControl->var.dFCmd[0] = mfcDtrCmdFilter( MFControl, wk_cmd );
		/* ����\UP FCmd(k) */
		MFControl->var.FCmdF = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 );
#else
		wk_cmd = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk3 );
		MFControl->var.dFCmd[0] = mfcDtrCmdFilter( MFControl, wk_cmd );
		/* ����\UP FCmd(k) */
		MFControl->var.FCmdF = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 );
#endif /* FLOAT_USE */

		/* ����\UP�ʒu�f�[�^�擾 */
		MFControl->var.FCmdFilo[2] = MFControl->var.FCmdFilo[1];
		MFControl->var.FCmdFilo[1] = MFControl->var.FCmdFilo[0];
		MFControl->var.FCmdFilo[0] = MFControl->var.FCmdF;

	/*----------------------------------------------------------------------------------------------*/
	/*		�������Z																				*/
	/*----------------------------------------------------------------------------------------------*/
		/* �Q�K�����l d2FCmd(k-1) */
		MFControl->var.d2FCmd[1] = MFControl->var.d2FCmd[0];
		/* �Q�K�����l d2FCmd(k) */
		MFControl->var.d2FCmd[0] = MFControl->var.dFCmd[0] - MFControl->var.dFCmd[1];
		/* �R�K�����l d3FCmd(k) */
		MFControl->var.d3FCmd = MFControl->var.d2FCmd[0] - MFControl->var.d2FCmd[1];
	}
}



/****************************************************************************************************/
/*																									*/
/*		�P�K�����l�Z�o�p�Q���t�B���^���Z															*/
/*																									*/
/****************************************************************************************************/
static	LONG	mfcDtrCmdFilter( MFCTRL *MFControl, LONG pcmd )
{
#if (FLOAT_USE==TRUE)
	float	wk;
	float	dFilo;

	wk = ( pcmd - MFControl->var.Filo ) - MFControl->var.dFilwk;
	wk = wk * MFControl->conf.MfcPexe.Kf2;
	MFControl->var.dFilwk += wk;

	dFilo = MFControl->var.dFilwk * MFControl->conf.MfcPexe.Kf3;
	
	wk = MFControl->var.dFilwk * MFControl->conf.MfcPexe.Kf1[1];
	MFControl->var.Filo += wk;

#else
	LONG	wk;
	LONG	dFilo;

	wk = ( pcmd - MFControl->var.Filo ) - MFControl->var.dFilwk;
	wk = MlibPfbkxremNolim( wk, MFControl->conf.MfcPexe.Kf2, &MFControl->var.Fwkrem1 );
	MFControl->var.dFilwk += wk;

	dFilo = MlibMulgain32( MFControl->var.dFilwk, MFControl->conf.MfcPexe.Kf3 );

	wk = MlibPfbkxremNolim( MFControl->var.dFilwk, MFControl->conf.MfcPexe.Kf1[1], &MFControl->var.Fwkrem2 );
	MFControl->var.Filo += wk;
#endif /* FLOAT_USE */

	return( dFilo );
}



/****************************************************************************************************/
/*																									*/
/*		MFC�p�����[�^�ؑւ������X�e�[�^�X�擾API													*/
/*																									*/
/*			�����p�����ł̐ؑ֊m�F�Ɏg�p����B���f���̃^�C�v�ƃ��f���Q�C���A���f�����g�����r����B*/
/*			�����@�\�ł̓��f���e�e�͈���Ȃ����߁C���f���^�C�v�ƃ��f���Q�C���A���f�����g����		*/
/*			�Ď�����Ηǂ��B���f���e�e�Q�C���Ȃǂ��̑��̃p�����[�^�ł̔��ʂ͕s�̂��ߒ��ӁB		*/
/*																									*/
/****************************************************************************************************/
SHORT	MfcGetParamEnableSts( MFCTRL *MFControl )
{
	SHORT	rc;

	if( (MFControl->conf.MfcPexe.MFCModel == MFControl->conf.MfcPrm[0].MFCModel)
		&& (MFControl->conf.MfcPexe.Kj2 == MFControl->conf.MfcPrm[0].Kj2)
		&& (MFControl->conf.MfcPexe.Kf2 == MFControl->conf.MfcPrm[0].Kf2) )
	{ /* ���s���f��/���s���f�����g��/���s���f���Q�C�����p�����[�^�Ɠ����ꍇ */
		rc = TRUE;
	}
	else
	{
		rc = FALSE;
	}
	return rc;
}



/****************************************************************************************************/
/*																									*/
/*		���f���Ǐ]�����~API																		*/
/*			�����ۂ�MFC����~����̂́A���[�^��~��ɂȂ�̂Œ���									*/
/*																									*/
/****************************************************************************************************/
void	MfcSetMfcCtrlSwitch( MFCTRL *MFControl, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* ���f���Ǐ]����L�����v���� */
		MFControl->var.MfcStopReq = FALSE;
	}
	else
	{ /* ���f���Ǐ]���䖳�����v���� */
		MFControl->var.MfcStopReq = TRUE;
	}
}





/****************************************************************************************************/
/*																									*/
/*		�l�e�b�^�C�v�I��																			*/
/*																									*/
/****************************************************************************************************/
/*		�߂�l	�F	Error Status (FALSE:�G���[�Ȃ�, TRUE:�p�����[�^�ݒ�ُ�)						*/
/****************************************************************************************************/
BOOL	MfcCalculateInitPrm( MFCTRL *MFControl, MFC_CFG_PRM *MfcCfgPrm,
    	                     USHORT jrate, BPRMDAT *Bprm, USHORT mfctype )
{
	BOOL	PrmErr;

	PrmErr = FALSE;

	switch(mfctype & 0x0F)
	{
	case 0x00: /* MFC�����^�C�v(SGDX�݊�) */
		MFControl->conf.MFCTypeSetting = SGDX_TYPE;
		break;
	case 0x01: /* MFC�����^�C�v(SGDV) */
		MFControl->conf.MFCTypeSetting = SGDV_TYPE;
		break;
	default:
		PrmErr = TRUE;
		break;
	}

	/* ��P�o���N	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 0 );
	/* ��Q�o���N	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 1 );

	return	PrmErr;
}



/****************************************************************************************************/
/*																									*/
/*		�l�e�b�I��																					*/
/*																									*/
/****************************************************************************************************/
/*		�߂�l	�F	Error Status (FALSE:�G���[�Ȃ�, TRUE:�p�����[�^�ݒ�ُ�)						*/
/****************************************************************************************************/
BOOL	MfcCalculatePrmSW( MFCTRL *MFControl, MFC_CFG_PRM *MfcCfgPrm, USHORT jrate, BPRMDAT *Bprm )
{
	BOOL	PrmErr;
	LONG	wk;

	PrmErr = FALSE;
	wk = MfcCfgPrm->mdlsw & 0xFF;

	switch( wk )
	{
	case 0x00: /* �@�\���� */
	case 0x10: /* �@�\���� */
	case 0x20: /* �@�\���� */
		MFControl->conf.MFCModelSetting = 0;
		break;
	case 0x01: /* ���f���Ǐ]���� */
		/* ���̌n���f���Ǐ]���� */
		MFControl->conf.MFCModelSetting = ONE_MASS;
		break;
	case 0x11: /* �U���}���t���f���Ǐ]���� */
	case 0x21: /* �U���}���Q�t���f���Ǐ]���� */
		/* ���̌n�{�@�䃂�f���Ǐ]���� */
		MFControl->conf.MFCModelSetting = ONE_BASE;
		break;
	default:
		PrmErr = TRUE;
		break;
	}

	wk = (MfcCfgPrm->mdlsw >> 12) & 0x0F;
	switch(wk)
	{
	case 0x00:
		MFControl->conf.MFCFF = FALSE;
		break;
	case 0x01:
		MFControl->conf.MFCFF = TRUE;
		break;
	default:
		PrmErr = TRUE;
		break;
	}

	/* ��P�o���N	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 0 );
	/* ��Q�o���N	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 1 );

	return	PrmErr;
}


/****************************************************************************************************/
/*																									*/
/*		�l�e�b�p�����[�^�v�Z																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  LONG		cfric			:	�N�[�������C�g���N�p�����[�^							*/
/*			  LONG		GeslNo			:	�p�����[�^�o���N										*/
/*												0�F��1�o���N�C1:��2�o���N							*/
/*																									*/
/****************************************************************************************************/
void	MfcCalculatePrm( MFCTRL *MFControl, MFC_CFG_PRM	*MfcCfgPrm,
						 USHORT jrate, BPRMDAT *Bprm, LONG cfric, LONG GselNo )
{
	MFCPRM		wrkp;
	USHORT		mdlgn;
	USHORT		mdlzeta;
	KSGAIN		hz;				/*<S00A>*/
	LONG		kx, sx;
	LONG		ky, sy;
	LONG		wk;
	KSGAIN		scantime;		/*<S00A>*/
	KSGAIN		maxpls;			/*<S00A>*/
#if (FLOAT_USE==TRUE)
	float fw;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�Q�C��No.�I��																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( GselNo == 0 )
	{ /* ���Q�C���ݒ莞 */
		mdlgn = MfcCfgPrm->mdlgn;
		mdlzeta = MfcCfgPrm->mdlzeta;
	}
	else
	{ /* ���Q�C���ݒ莞 */
		mdlgn = MfcCfgPrm->mdlgn2;
		mdlzeta = MfcCfgPrm->mdlzeta2;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�@�B���f��																					*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.MFCModel = MFControl->conf.MFCModelSetting;

/*--------------------------------------------------------------------------------------------------*/
/*		MFC�^�C�v		<V509>																		*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.MFCType = MFControl->conf.MFCTypeSetting;

/*--------------------------------------------------------------------------------------------------*/
/*		���Z�T�C�N���^�C��	[ns]																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( wrkp.MFCModel == ONE_MASS )							/* ���̃��f��							*/
	{
		scantime = (LONG)Bprm->SvCycleNs;
//		scantime = (LONG)MFCONEMASSCYCLE;
	}
	else													/* �Q�������f��							*/
	{
		scantime = (LONG)MFCTWOMASSCYCLE(Bprm->SvCycleNs);
//		scantime = (LONG)MFCTWOMASSCYCLE;
	}

#if 0
	if( (Kprm.f.FencUse == TRUE)							/* Full-Closed Control					*/
		&& (wrkp.MFCModel != ONE_BASE) )					/* ���́{�@��łȂ����񊵐����f��		*/
	{
		wrkp.Fccflag = 1;
	}
	else
#endif
	{
		wrkp.Fccflag = 0;
	}

#if 0 /* PS�t���N�p�t���O(���Ή�) */
	wrkp.PSFccflag = (Iprm.f.PsFulcSelect) ? 1 : 0 ;		/* PS-Full-Closed Control				*/
#else
	wrkp.PSFccflag = 0;
#endif

/* 2011.04.04 Y.Oka <V731> */
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^OS���x�p���X�v�Z	[Pluse/Scan]				Bprm.MaxPspd�F[Pulse/sec]				*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	maxpls = (scantime * Bprm->MaxPspd / (float)C10POW9);
#else
	maxpls = MlibScalKxkskx( scantime, Bprm->MaxPspd, C10POW9, NULL, -30 );
#endif /* FLOAT_USE */

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*		�c�s�q�����t�B���^����\�V�t�g�� Fsft														*/
/*--------------------------------------------------------------------------------------------------*/
/*						   2^29																		*/
/*				Fsft = ------------		InvFsft = 1/Fsft											*/
/*				   		  FbPulse																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Fsft = 0x20000000 / Bprm->FbPulse;
	if( wrkp.Fsft < 1.0 )
	{
		wrkp.Fsft = 1.0;
	}
	wrkp.InvFsft = 1.0F / wrkp.Fsft;
#else
	wrkp.Fsft = 0x20000000 / Bprm->FbPulse;
	if( wrkp.Fsft < 1 )
	{
		wrkp.Fsft = 1;
	}
	wrkp.InvFsft = MlibScalKxgain( 1, 1, wrkp.Fsft, NULL, 24 );
#endif /* FLOAT_USE */

#endif
/* 2011.04.04 Y.Oka <V731> */

/*--------------------------------------------------------------------------------------------------*/
/*		�Q���t�B���^�Q�C���P Kf1																	*/
/*--------------------------------------------------------------------------------------------------*/
/*						   Ts[us] * 0x1000000		    Ts[us] * 0x1000000							*/
/*				Kf1[0] = ---------------------- = --------------------------------					*/
/*				    	    Tf[us] + Ts[us]	        (10^6 / 2�ă�n)[us] + Ts[us]					*/
/*																									*/
/*						     ��n * Ts[us]															*/
/*				Kf1[1] = ----------------------														*/
/*						   2 * ��[1.0] * 10^6														*/
/*--------------------------------------------------------------------------------------------------*/
/*				Ts[us]  = SvCycleUs[us]																*/
/*																									*/
/*							mdlzeta[0.0001]															*/
/*				��[1.0] = -------------------														*/
/*							    10000																*/
/*		(����)																						*/
/*											    2*mdlzeta[0.001]*mdlgn[0.1/s]						*/
/*				��n = 2 * ��[1.0] * Km[1/s] = ----------------------------------					*/
/*														1000 * 10									*/
/*		(�񊵐�, ���́{�@��)																		*/
/*		 								        4*mdlzeta[0.001]*mdlgn[0.1/s]						*/
/*				��n = 4 * ��[1.0] * Km[1/s] = ----------------------------------					*/
/*														1000 * 10									*/
/*																									*/
/*					  Km : ���f�����[�v�Q�C��(�w�߃t�B���^�Q�C��)									*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�t�B���^���g�� hz[0.1Hz] = ��n*10 / 2*PAI �̌v�Z										*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)

	if( wrkp.MFCModel == ONE_MASS )							/* ���̃��f��							*/
	{
		hz = 4.0f * (float)mdlzeta * (float)mdlgn * 10.0f / 62832.0f;
	}
	else													/* �񊵐��A���́{�@�䃂�f��				*/
	{
		hz = 4.0f * (float)mdlzeta * (float)mdlgn * 10.0f / 62832.0f;
	}

#else	
	if( wrkp.MFCModel == ONE_MASS )							/* ���̃��f��							*/
	{
		hz = MlibScalKxgain( 4*mdlzeta, mdlgn*10, 62832, NULL, -24 );
	}
	else													/* �񊵐��A���́{�@�䃂�f��				*/
	{
		hz = MlibScalKxgain( 4*mdlzeta, mdlgn*10, 62832, NULL, -24 );
	}
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		�Q���t�B���^�Q�C�� Kf1[0], Kf1[1] �v�Z													*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FlibPcalKf2gain( hz, mdlzeta, scantime, wrkp.Kf1);
#else
	MlibPcalKf2gain( hz, mdlzeta, scantime, wrkp.Kf1, 0x10 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�Q���t�B���^�Q�C���Q Kf2																	*/
/*--------------------------------------------------------------------------------------------------*/
/*						    Kf1[0]																	*/
/*				Kf2 = -----------------																*/
/*				    	  0x1000000																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kf2 = (wrkp.Kf1[0] / (float)0x1000000);
#else
	wrkp.Kf2 = MlibScalKxgain( wrkp.Kf1[0], 1, 0x1000000, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�Q���t�B���^�Q�C���R Kf3																	*/
/*--------------------------------------------------------------------------------------------------*/
/*						Kf1[1] * 1000000000															*/
/*				Kf3 = --------------------															*/
/*				    	 SvCycleUs[ns]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kf3 = wrkp.Kf1[1] * (float)C10POW9 / scantime;
#else
	wrkp.Kf3 = MlibScalKskxkx( wrkp.Kf1[1], C10POW9, scantime, NULL, 24 );
#endif /* FLOAT_USE */

/* 2011.04.04 Y.Oka <V731> */
/*--------------------------------------------------------------------------------------------------*/
/*		DTR�����t�B���^����\�V�t�g�� Fsft															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				Fsft = 2^30/((maxpls/kf[1]))			maxpls�F�ő�p���X/�������					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	kx = wrkp.Kf1[1] * 0x3FFFFFFF / maxpls;

	wk = 0x20000000 / Bprm->FbPulse;
	wrkp.Fsft = FlibLimitul( kx, wk, 1.0f );								/* 1 < kx < (2^29/plsno)	*/

	wrkp.InvFsft = (1.0F / wrkp.Fsft);			/* invFsft = 1/ Fsft		*/
/* 2011.04.04 Y.Oka <V731> */
#else
	kx = MlibScalKskxkx( wrkp.Kf1[1], 0x3FFFFFFF, maxpls, NULL, -24 );

	wk = 0x20000000 / Bprm->FbPulse;
	wrkp.Fsft = MlibLimitul( kx, wk, 1 );								/* 1 < kx < (2^29/plsno)	*/

	wrkp.InvFsft = MlibScalKxgain( 1, 1, wrkp.Fsft, NULL, 24 );			/* invFsft = 1/ Fsft		*/
/* 2011.04.04 Y.Oka <V731> */
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		���J�W���Q�C���P Kj1																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		             			  1000000000       100 + jrate   									*/
/*				Kj1 =  Kpvx * ---------------- * ---------------									*/
/*		            			SvCycleUs[ns]         100        									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kj1 = Bprm->Kpvx * C10POW9 / scantime * ((jrate + 100)/100.0f);
#else
	kx = MlibScalKskxkx( Bprm->Kpvx, C10POW9, scantime, &sx, 0 );
	wrkp.Kj1 = MlibPcalKxgain( kx, (jrate + 100), 100, &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		���J�W���Q�C���Q Kj2																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		             		           100 * 1000000000    											*/
/*				Kj2 = -----------------------------------------------------							*/
/*		            	antfrq[0.1Hz]^2 * (2*PAI)^2 * SvCycleUs[ns] * Fsft							*/
/*																									*/
/*		             	 	           100 * 1000000000    											*/
/*				Kj2_sft = ----------------------------------------------							*/
/*		               	   antfrq[0.1Hz]^2 * (2*PAI)^2 * SvCycleUs[ns]								*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	//wrkp.Kj2 = 1.0 / ((MfcCfgPrm->mdlantfrq*0.1)*(MfcCfgPrm->mdlantfrq*0.1) * (2*PAI)*(2*PAI) * scantime * wrkp.Fsft);
	wrkp.Kj2 = 100.0f * E9C2PAI2 / (MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq * scantime / wrkp.Fsft);

	//wrkp.Kj2_sft = 1.0 / ((MfcCfgPrm->mdlantfrq*0.1)*(MfcCfgPrm->mdlantfrq*0.1) * (2*PAI)*(2*PAI) * scantime * wrkp.Fsft);
	wrkp.Kj2_sft = 100.0f * E9C2PAI2 / (MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq * scantime);
#else
	sx = 0;
	sy = 0;
	wk = MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq;
	kx = MlibPcalKxmulx( 100, E9C2PAI2, 1, &sx );
	ky = MlibPcalKxmulx( wk, scantime, wrkp.Fsft, &sy );
	wrkp.Kj2 = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

	sx = 0;
	sy = 0;
	wk = MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq;
	kx = MlibPcalKxmulx( 100, E9C2PAI2, 1, &sx );
	ky = MlibPcalKxmulx( wk, scantime, 1, &sy );
	wrkp.Kj2_sft = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		���J�W���Q�C���R Kj3																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		             		           100 * 1000000000^2  											*/
/*				Kj3 = --------------------------------------------------------						*/
/*		            	resfrq[0.1Hz]^2 * (2*PAI)^2 * SvCycleUs[ns]^2 * Fsft						*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	//wrkp.Kj3 = 1.0 / ((MfcCfgPrm->mdlresfrq*0.1)*(MfcCfgPrm->mdlresfrq*0.1) * (2*PAI)*(2*PAI) * scantime*scantime * wrkp.Fsft);
	wrkp.Kj3 = (100 * E9C2PAI2 * 1000000000 / wrkp.Fsft) / (MfcCfgPrm->mdlresfrq * MfcCfgPrm->mdlresfrq * scantime * scantime);
#else
	sx = 0;
	sy = 0;
	kx = MlibPcalKxmulx( 100, E9C2PAI2, 1000000000, &sx);
	kx = MlibPcalKxgain( kx, 1, wrkp.Fsft, &sx , 0 );

	wk = MfcCfgPrm->mdlresfrq * MfcCfgPrm->mdlresfrq;
	ky = MlibPcalKxmulx( wk, scantime, scantime, &sy);
	wrkp.Kj3 = MlibPcalKxdivx(  kx, sx, ky, sy, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�c�s�q���x�t�B�[�h�t�H���[�h�Q�C�� Kvff														*/
/*--------------------------------------------------------------------------------------------------*/
/*		             		   mfckvff[0.1%] * 1000000000  											*/
/*				Kvff = Kpx * ---------------------------											*/
/*		            			 1000 * SvCycleUs[ns]												*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	if(wrkp.MFCType == SGDX_TYPE)
	{ /* MFC�����^�C�v(SGDX�݊�) */
		wrkp.Kvff = Bprm->Kpx * 1000000000.0f / scantime * MfcCfgPrm->mdlvff / 1000.0f ;
	}
	else
	{
		wrkp.Kvff = Bprm->Kpx * 1000000000.0f / scantime / wrkp.Fsft * MfcCfgPrm->mdlvff / 1000.0f;
	}
#else
	kx = MlibScalKskxkx( Bprm->Kpx, 1000000000, scantime, &sx, 0 );
	if(wrkp.MFCType == SGDX_TYPE)
	{ /* MFC�����^�C�v(SGDX�݊�) */
		wrkp.Kvff = MlibPcalKxgain( kx, MfcCfgPrm->mdlvff, 1000, &sx, 24 );
	}
	else
	{
		kx = MlibPcalKxgain( kx, 1, wrkp.Fsft, &sx, 0 );
		wrkp.Kvff = MlibPcalKxgain( kx, MfcCfgPrm->mdlvff, 1000, &sx, 24 );
	}
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		�c�s�q�g���N�t�B�[�h�t�H���[�h�Q�C�� Ktff1, Ktff2											*/
/*--------------------------------------------------------------------------------------------------*/
/*		                   mfcktff[0.1%]    			  mfctffgn2[0.1%]							*/
/*				Ktff1 = ------------------		Ktff2 = -------------------							*/
/*		       (���]�p)	       1000 		   (�t�]�p)		   1000									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Ktff1 = MfcCfgPrm->mdlptff / 1000.0f;
	wrkp.Ktff2 = MfcCfgPrm->mdlntff / 1000.0f;
#else
	wrkp.Ktff1 = MlibScalKxgain( MfcCfgPrm->mdlptff, 1, 1000, NULL, 24 );
	wrkp.Ktff2 = MlibScalKxgain( MfcCfgPrm->mdlntff, 1, 1000, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�N�[�������C�⏞�g���N Cfric 																*/
/*--------------------------------------------------------------------------------------------------*/
/*				�g���N���x���v�Z KpiTorqueLevelCal() : Torque[0.01%] --> NorTrq[2^24/MaxTrq]		*/
/*														cfric[0.1%]									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Cfric = cfric*10 * Bprm->Ktrqlvl;
#else
	wrkp.Cfric = MlibMulgain( cfric*10, Bprm->Ktrqlvl );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�N�[�������C�⏞�t�B���^�Q�C��																*/
/*--------------------------------------------------------------------------------------------------*/
/*							    SvCycleUs[us]														*/
/*				Kfcf = -----------------------------------											*/
/*						     5000000																*/
/*						 ---------------- + SvCycleUs[us]											*/
/*						   mdlgn[0.1/s]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = 5000000.0f / mdlgn;
	if( fw > 100000.0f )
	{
		fw = 100000.0f;
	}
	wrkp.Kfcf = FlibPcalKf1gain( fw, (scantime/1000.0f), 0 );
#else
	kx = (LONG)( 5000000 / (SHORT)mdlgn );
	if( kx > 100000 )
	{
		ky = 100000;
	}
	else
	{
		ky = kx;
	}
	wrkp.Kfcf = MlibPcalKf1gain( ky, (scantime/1000), 0 );
#endif /* FLOAT_USE */

/* 2009.12.24 Y.Oka ���荞�݋֎~�K�v�H */
/*--------------------------------------------------------------------------------------------------*/
/*		Set Parameters																				*/
/*--------------------------------------------------------------------------------------------------*/
//	KPI_DI();		/* Disable Interrupt	*/
	MFControl->conf.MfcPrm[GselNo+1] = wrkp;
//	KPI_EI();		/* Enable Interrupt		*/

}



/***************************************** end of file **********************************************/
