/****************************************************************************************************/
/*																									*/
/*																									*/
/*		GainChange.c : �Q�C���؂�ւ��֘A������`													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �Q�C���؂�ւ��֘A����																	*/
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
#include "GainChange.h"
#include "MLib.h"

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	GselInitAutoGainChange( ATGSEL *pAtGselA, ATGSEL *pAtGselB );							*/
/*	LONG	GselDetGainChngTiming( GSELSIG *GselSig, RSP_CTRLBITS *rspctrl_ptr,						*/
/*														POS_CTRL_STS *PosCtrlSts, LONG Condition );	*/
/*	void	GselAutoGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG triger );			*/
/*	void	GselManualGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData,						*/
/*																LONG GselNum, BOOL TuningLess );	*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		�����Q�C���ؑ֏�����																		*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S09B> */
void	GselInitAutoGainChange( ATGSEL *pAtGselA, ATGSEL *pAtGselB )
{
		pAtGselA->var.Timer = 0;					/* �҂�����1�^�C�}������						*/
		pAtGselB->var.Timer = 0;					/* �҂�����2�^�C�}������						*/
		return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		�T�[�{�Q�C���ؑ֕ϐ�������																	*/
/*																									*/
/****************************************************************************************************/
void	GselInitServoGainChange( GAIN_CHNG_HNDL *hGainChg )
{
ATGSEL *pAtGsel;													/* <S09B> */

	/* ���������͑�1�Q�C����ݒ� */
	hGainChg->GselGains[0] = hGainChg->GselGains[1];
	hGainChg->GselDobs[0] = hGainChg->GselDobs[1];					/* <S09B> */
	hGainChg->AmonActGain = GAIN1;
	hGainChg->ActGain = 1;

	/* �Q�C���؂�ւ��p�^�C�}������ */
	hGainChg->AtGselA.var.Timer = 0;
	hGainChg->AtGselB.var.Timer = 0;

#if 0 /* 2010.11.10 Y.Oka �����Q�C���؂�ւ����̃X���[�W���O���Ή��̂��߃R�����g�A�E�g */
	MlibResetLongMemory( &(hGainChg->AtGselA.var), sizeof(hGainChg->AtGselA.var)/4 );
	MlibResetLongMemory( &(hGainChg->AtGselB.var), sizeof(hGainChg->AtGselB.var)/4 );
	MlibResetLongMemory( &(hGainChg->AtGselDobsA.var), sizeof(hGainChg->AtGselDobsA.var)/4 );
	MlibResetLongMemory( &(hGainChg->AtGselDobsB.var), sizeof(hGainChg->AtGselDobsB.var)/4 );

	hGainChg->AtGselA.var.prevGain = &(hGainChg->GselGains[1]);
	hGainChg->AtGselA.var.nextGain = &(hGainChg->GselGains[2]);
	hGainChg->AtGselB.var.prevGain = &(hGainChg->GselGains[2]);
	hGainChg->AtGselB.var.nextGain = &(hGainChg->GselGains[1]);

	hGainChg->AtGselDobsA.var.prevGain = &(hGainChg->GselDobs[1]);
	hGainChg->AtGselDobsA.var.nextGain = &(hGainChg->GselDobs[2]);
	hGainChg->AtGselDobsB.var.prevGain = &(hGainChg->GselDobs[2]);
	hGainChg->AtGselDobsB.var.nextGain = &(hGainChg->GselDobs[1]);

	hGainChg->GselTrqFilMin = MlibPcalKf1gain( 10, KPI_SACYCLEUS, 0 );
#endif /* #if 0 */
/*--------------------------------------------------------------------------------------------------*/
/*	<S09B> Auto Gain Change																			*/
/*--------------------------------------------------------------------------------------------------*/
	GselInitAutoGainChange( &(hGainChg->AtGselA),				/* condition A */
							&(hGainChg->GselGains[1]),			/* prevGain */
							&(hGainChg->GselGains[2]),			/* nextGain */
							&(hGainChg->GselDobs[1]),			/* prevGain(Dobs) */
							&(hGainChg->GselDobs[2]) );			/* nextGain(Dobs) */
	GselInitAutoGainChange( &(hGainChg->AtGselB),				/* condition B */
							&(hGainChg->GselGains[2]),			/* prevGain */
							&(hGainChg->GselGains[1]),			/* nextGain */
							&(hGainChg->GselDobs[2]),			/* prevGain(Dobs) */
							&(hGainChg->GselDobs[1]) );			/* nextGain(Dobs) */
/*--------------------------------------------------------------------------------------------------*/
/*	<S09B> ���Q�C�� �p�����[�^�Čv�Z(�O�̂���)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAtGsel = &(hGainChg->AtGselA);								/* condition A */
	PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kp */
	PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kv, Kvi */
	PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Klpf */
	PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate delta Ks */
	pAtGsel = &(hGainChg->AtGselB);								/* condition B */
	PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kp */
	PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kv, Kvi */
	PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Klpf */
	PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate delta Ks */
/*--------------------------------------------------------------------------------------------------*/

	/* CoinOn, NearOn, RefZOn */
//	hGainChg->GselSig.CoinOn  = TRUE;
//	hGainChg->GselSig.CoinOff = FALSE;
//	hGainChg->GselSig.NearOn  = TRUE;
//	hGainChg->GselSig.NearOff = FALSE;
//	hGainChg->GselSig.RefZOn  = TRUE;
//	hGainChg->GselSig.RefZOff = FALSE;

}


/****************************************************************************************************/
/*																									*/
/*		�Q�C���ؑ֏������菈��																		*/
/*																									*/
/****************************************************************************************************/
LONG	GselDetGainChngTiming( BOOL CoinSignal, BOOL NearSignal, BOOL RefZSignal, LONG Condition )
{
	LONG	triger;

	/* �����`�����ő�Q�Q�C���J�ڂ܂��͑�Q�Q�C���I�� */
	switch( Condition )
	{
	case 0x00:
		triger = CoinSignal;
		break;
	case 0x01:
		triger = CoinSignal ^ 0x01;
		break;
	case 0x02:
		triger = NearSignal;
		break;
	case 0x03:
		triger = NearSignal ^ 0x01;
		break;
	case 0x04:
		triger = RefZSignal;
		break;
	case 0x05:
		triger = RefZSignal ^ 0x01;
		break;
	default:
		triger = 0;
		break;
	}
	return triger;
}
#if 0
LONG	GselDetGainChngTiming( GAIN_CHNG_HNDL *hGainChg, BOOL CoinSignal,
    	                       BOOL NearSignal, BOOL RefZSignal, LONG Condition )
{
	LONG	triger;

	/* ���[�J���ϐ��̏����� */
	triger = 0;

	/* �����Q�C���ؑ֗p�g���K�M���쐬 */
	GselSig->CoinOn		= PosCtrlSts->CoinSignal;
	GselSig->CoinOff	= PosCtrlSts->CoinSignal ^ 0x01;
	GselSig->NearOn		= PosCtrlSts->NearSignal;
	GselSig->NearOff	= PosCtrlSts->NearSignal ^ 0x01;
	GselSig->RefZOn		= PosCtrlSts->RefZSignal;
	GselSig->RefZOff	= PosCtrlSts->RefZSignal ^ 0x01;

	/* �����`�����ő�Q�Q�C���J�ڂ܂��͑�Q�Q�C���I�� */
	switch( Condition )
	{
	case 0x00:
		triger = GselSig->CoinOn;
		break;
	case 0x01:
		triger = GselSig->CoinOff;
		break;
	case 0x02:
		triger = GselSig->NearOn;
		break;
	case 0x03:
		triger = GselSig->NearOff;
		break;
	case 0x04:
		triger = GselSig->RefZOn;
		break;
	case 0x05:
		triger = GselSig->RefZOff;
		break;
	default:
		triger = 0;
		break;
	}
	return triger;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		�����Q�C���ؑ�																				*/
/*																									*/
/*		�Q�C���ؑ֏������ʃv���O�����ɂĎ��{����BCPU���ł́A��������y�ё҂����ԊǗ����s���A		*/
/*		�ʃv���O�����փQ�C���ؑ֗v���𑗂�(MREG_BITCMDI.BITCMD_GAINCHNGON)�B						*/
/*		��MREG_BITCMDI.BITCMD_GAINCHNGON = ON�ő��Q�C���ɐ؂�ւ��B								*/
/*																									*/
/*		�Ȃ��A���Q�C���ؑ֋@�\(Pn131)�͔�Ή��Ƃ���B												*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S09B> AutoGainChange.c�Ɉړ� */
void	GselAutoGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG triger, CTRL_CMD_PRM *CtrlCmdPrm)
{
	ATGSEL			*atgsela_ptr;
	ATGSEL			*atgselb_ptr;

	atgsela_ptr		= &( hGainChg->AtGselA );
	atgselb_ptr		= &( hGainChg->AtGselB );

	if( triger )
	{
		if( atgsela_ptr->var.Timer >= atgsela_ptr->conf.Wait )
		{ /* ��1����2�Q�C���҂����Ԍo�ߌ� */
			/* ��2�Q�C�� */
		/*------------------------------------------------------------------------------------------*/
		/*	�ȉ��̏������ʃv���O�����ɂĎ��{����B													*/
		/*		hGainChg->GselGains[0]	= hGainChg->GselGains[2];									*/
		/*		pDobsData->param[0]		= pDobsData->param[2];										*/
		/*------------------------------------------------------------------------------------------*/
			hGainChg->GselGains[0].VirtualPosKp = hGainChg->GselGains[2].VirtualPosKp;
			hGainChg->AmonActGain = GAIN2;
			hGainChg->ActGain = 2;
			atgselb_ptr->var.Timer = 0; /* �҂�����2�^�C�}������ */
		}
		else
		{
			atgsela_ptr->var.Timer++; /* �҂�����1�^�C�}�J�E���g�A�b�v */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		����B�����ő�1�Q�C���J�ڂ܂��͑�1�Q�C���I��												*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		if( atgselb_ptr->var.Timer >= atgselb_ptr->conf.Wait )
		{ /* ��2����1�Q�C���҂����Ԍo�ߌ� */
			/* ��1�Q�C�� */
		/*------------------------------------------------------------------------------------------*/
		/*	�ȉ��̏������ʃv���O�����ɂĎ��{����B													*/
		/*		hGainChg->GselGains[0]	= hGainChg->GselGains[1];									*/
		/*		pDobsData->param[0]		= pDobsData->param[1];										*/
		/*------------------------------------------------------------------------------------------*/
			hGainChg->GselGains[0].VirtualPosKp = hGainChg->GselGains[1].VirtualPosKp;
			hGainChg->AmonActGain = GAIN1;
			hGainChg->ActGain = 1;
			atgsela_ptr->var.Timer = 0; /* �҂����ԂP�^�C�}������ */
		}
		else
		{
			atgselb_ptr->var.Timer++; /* �҂����ԂQ�^�C�}�J�E���g�A�b�v */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		����A�ł�B�ł��Ȃ���Ԃ̎��͑O��̃Q�C���̂܂�												*/
/*--------------------------------------------------------------------------------------------------*/
	/* MFC�͎����Q�C���ؑւ��Ȃ��B��1�Q�C���̂܂� */
	pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];

	/*------------------------------------------------------------------------------------------*/
	/*	�ȉ��̏������ʃv���O�����ɂĎ��{����B													*/
	/*		���x�����p�Q�C���́A�����Q�C���ؑւȂ��B��P�Q�C���̂܂�							*/
	/*		pTrqCtrl->conf.OverTrqLoopGain[0]	= pTrqCtrl->conf.OverTrqLoopGain[1];			*/
	/*		pTrqCtrl->conf.OverTrqIntegGain[0]	= pTrqCtrl->conf.OverTrqIntegGain[1];			*/
	/*------------------------------------------------------------------------------------------*/
	CtrlCmdPrm->OverTrqLoopGain[0]	= CtrlCmdPrm->OverTrqLoopGain[1];
	CtrlCmdPrm->OverTrqIntegGain[0]	= CtrlCmdPrm->OverTrqIntegGain[1];

	return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		�}�j���A���Q�C���ؑ�																		*/
/*																									*/
/*		�Q�C���ؑ֏������ʃv���O�����ɂĎ��{����BCPU���ł́AMECHATROLINK���󂯎�����o���NNo��	*/
/*		�����āA�ʃv���O�����փQ�C���ؑ֗v���𑗂�(MREG_BITCMDI.BITCMD_GAINCHNGON)�B				*/
/*																									*/
/****************************************************************************************************/
//<S09B>void	GselManualGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG GselNum, BOOL TuningLess, 
//<S09B>				CTRL_CMD_PRM *CtrlCmdPrm )
void GselManualGainChange( BASE_CTRL_HNDL *BaseControls )					/* <S09B> */
{
GAIN_CHNG_HNDL *hGainChg	= &(BaseControls->GainChange);					/* <S09B> */
MFCTRL *pMfcData			= &(BaseControls->MFControl);					/* <S09B> */
LONG GselNum				= BaseControls->GselNum;						/* <S09B> */
BOOL TuningLess				= BaseControls->TuneLessCtrl.var.TuneLessAct;	/* <S09B> */
CTRL_CMD_PRM *CtrlCmdPrm	= &(BaseControls->CtrlCmdPrm);					/* <S09B> */
//DOBS_CTRL *pDobs			= &(BaseControls->DobsCtrl);					/* <S09B> *///<S0C7>
	/*------------------------------------------------------------------------------------------*/
	/*	�ȉ��̏������ʃv���O�����ɂĎ��{����B													*/
	/*		�������X�p�Q�C���I�� (5�o���N��)													*/
	/*		if( TuningLess != FALSE )															*/
	/*		{ �������X�L�� -> �Q�C���ؑւ��𖳌��Ƃ��A�������X�p�Q�C�����g�p����				*/
	/*			hGainChg->GselGains[0] = hGainChg->GselGains[5];								*/
	/*			�O���I�u�U�[�o																	*/
	/*			pDobsData->param[0] = pDobsData->param[1];										*/
	/*			���x�������[�v�Q�C��															*/
	/*			pTrqCtrl->conf.OverTrqIntegGain[0] = pTrqCtrl->conf.OverTrqIntegGain[1];		*/
	/*			���x�����ϕ��Q�C��																*/
	/*			pTrqCtrl->conf.OverTrqLoopGain[0] = pTrqCtrl->conf.OverTrqLoopGain[1];			*/
	/*		}																					*/
	/*------------------------------------------------------------------------------------------*/
	if( TuningLess != FALSE )
	{ /* �������X�L�� -> �Q�C���ؑւ��𖳌��Ƃ��A�������X�p�Q�C�����g�p���� */
		/* ��{����p�Q�C�� */
		hGainChg->GselGains[0] = hGainChg->GselGains[5];
		/* �O���I�u�U�[�o <S09B> */
		hGainChg->GselDobs[0] = hGainChg->GselDobs[1];					/* <S09B> */
		/* MFC�Q�C�� */
		pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];
		/* ���݃Q�C���m�F�p */
		hGainChg->ActGain = 1;
		hGainChg->AmonActGain = GAIN1;
		/*			���x�������[�v�Q�C��						*/
		CtrlCmdPrm->OverTrqLoopGain[0] = CtrlCmdPrm->OverTrqLoopGain[1];
		/*			���x�����ϕ��Q�C��							*/
		CtrlCmdPrm->OverTrqIntegGain[0] = CtrlCmdPrm->OverTrqIntegGain[1];
	}
	else
	{
	/*------------------------------------------------------------------------------------------*/
	/*	�ȉ��̏������ʃv���O�����ɂĎ��{����B													*/
	/*		if( GselNum < SVCDEF_NETGSEL_SETNUM )												*/
	/*		{ ��1�Q�C���A��2�Q�C���I����														*/
	/*			hGainChg->GselGains[0] = hGainChg->GselGains[GselNum+1]							*/
	/*			�O���I�u�U�[�o																	*/
	/*			pDobsData->param[0] = pDobsData->param[GselNum+1];								*/
	/*			���x�������[�v�Q�C��															*/
	/*			pTrqCtrl->conf.OverTrqIntegGain[0] = pTrqCtrl->conf.OverTrqIntegGain[GselNum+1];*/
	/*			���x�����ϕ��Q�C��																*/
	/*			pTrqCtrl->conf.OverTrqLoopGain[0] = pTrqCtrl->conf.OverTrqLoopGain[GselNum+1];	*/
	/*		}																					*/
	/*------------------------------------------------------------------------------------------*/
		if( GselNum < SVCDEF_NETGSEL_SETNUM )
		{ /* ��1�Q�C���A��2�Q�C���I���� */
			/* ��{����p�Q�C�� */
			hGainChg->GselGains[0] = hGainChg->GselGains[GselNum + 1];
			/* �O���I�u�U�[�o <S09B> */
			hGainChg->GselDobs[0] = hGainChg->GselDobs[GselNum + 1];			/* <S09B> */
			/* MFC�Q�C�� */
			pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[GselNum + 1];
			/* ���݃Q�C���m�F�p */
			hGainChg->ActGain = GselNum + 1;
			hGainChg->AmonActGain = GAIN1 << GselNum;
			/*			���x�������[�v�Q�C��						*/
			CtrlCmdPrm->OverTrqLoopGain[0] = CtrlCmdPrm->OverTrqLoopGain[GselNum+1];
			/*			���x�����ϕ��Q�C��							*/
			CtrlCmdPrm->OverTrqIntegGain[0] = CtrlCmdPrm->OverTrqIntegGain[GselNum+1];
		}
		else
		{ /* ��3�Q�C���A��4�Q�C���I�����́A��1�Q�C���Œ� */
			/* MFC�Q�C�� */
			pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];
			/* �O���I�u�U�[�o <S09B> */
			hGainChg->GselDobs[0] = hGainChg->GselDobs[1];			/* <S09B> */
			/* ���݃Q�C���m�F�p */
			hGainChg->ActGain = 1;
			hGainChg->AmonActGain = GAIN1;
			/*			���x�������[�v�Q�C��						*/
			CtrlCmdPrm->OverTrqLoopGain[0] = CtrlCmdPrm->OverTrqLoopGain[1];
			/*			���x�����ϕ��Q�C��							*/
			CtrlCmdPrm->OverTrqIntegGain[0] = CtrlCmdPrm->OverTrqIntegGain[1];
		}
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�����Q�C���ؑփp�����[�^�v�Z																*/
/*																									*/
/****************************************************************************************************/
void	GselCalculatePrm( GAIN_CHNG_HNDL *hGainChg, USHORT gnswwait1, USHORT gnswwait2, LONG ScanTime )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�C���֑ؑ҂����ԂP(Pn135) [ms]															*/
/*--------------------------------------------------------------------------------------------------*/
	hGainChg->AtGselA.conf.Wait = gnswwait1 * 1000 / ScanTime;

/*--------------------------------------------------------------------------------------------------*/
/*		�Q�C���֑ؑ҂����ԂQ(Pn136) [ms]															*/
/*--------------------------------------------------------------------------------------------------*/
	hGainChg->AtGselB.conf.Wait = gnswwait2 * 1000 / ScanTime;

}



/****************************************************************************************************/
/*																									*/
/*		�����Q�C���ؑփ|�C���^�擾API																*/
/*																									*/
/****************************************************************************************************/
BOOL	GselGet_AtGselStatus( GAIN_CHNG_HNDL *hGainChg )
{
	return hGainChg->AtGsel;
}

/***************************************** end of file **********************************************/
