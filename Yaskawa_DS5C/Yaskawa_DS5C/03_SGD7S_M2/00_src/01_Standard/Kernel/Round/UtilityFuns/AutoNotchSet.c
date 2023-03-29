/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AutoNotchSet.c : �����m�b�`�t�B���^�ݒ�@�\���C�����W���[��									*/
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
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2007.02.04  Y.Takei	<V109>														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "AutoNotchSet.h"
#include "TuneLib.h"
#include "PrmAccessIf.h"
#include "PnPrmListTbl.h"
#include "KLib.h"
#include "MLib.h"



/****************************************************************************************************/
/*																									*/
/*		Function Definition																			*/
/*																									*/
/****************************************************************************************************/
/* void		AutoNotchFilterSetting( ANOTCHSEQCTRL *ANotchCtrl );									*/
/* void		AutoNotchInitialize( ANOTCHSEQ *ANotchSeq );											*/
/* void		AutoNotchCalculatePrm( ANOTCHSEQ *ANotchSeq, USHORT kv, USHORT jrate);					*/
/* void		AutoNotchCalculateInitPrm( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch,						*/
/*															LONG AmpType, UNI_PRM_STR *UniPrm );	*/
/* void		RpiResetStatusANotch1( ANOTCHSEQ *ANotchSeq );											*/
/* void		RpiResetStatusANotch2( ANOTCHSEQ *ANotchSeq );											*/
/* UCHAR	RpiGetANotchPreStat( ANOTCHSEQ *ANotchSeq );											*/
/* USHORT	RpiGetANotchSetStat( ANOTCHSEQ *ANotchSeq );											*/
/* void		RpiSetRegSpatANotchState( ANOTCHSEQ *ANotchSeq );										*/
/* void		RpiANotchFftReqSet( ANOTCHSEQ *ANotchSeq, FFTANALYZE *FftAna );							*/
/* void		RpiANotchTrqFilCalc( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch,							*/
/*														REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );		*/
/* void		RpiANotchDisableReqSet( ANOTCHSEQ *ANotchSeq );											*/
/* void		AutoNotchSetCondition( ANOTCHSEQ *ANotchSeq, BOOL EepromWriteProtect,					*/
/* 									BOOL BaseEnableSts, BOOL TuneLessSts, BOOL OverTravelSts );		*/
/****************************************************************************************************/
static	SHORT		autoNotchCheckCondition( ANOTCHSEQ *ANotchSeq, DETVIB *DetVib, PRMDATA *Prm );
static	LONG		autoNotchFreqComp( ANOTCHSEQ *ANotchSeq, LONG DFreq );
static	PRM_RSLT	autoNotchPrmWriteEeprom( REGIF_CONFIG_T *RegCfg, BOOL EepromWriteProtect );



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�ϐ�������																		*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchInitialize( ANOTCHSEQ *ANotchSeq )
{
	MlibResetLongMemory( &(ANotchSeq->var), sizeof(ANotchSeq->var) / 4 );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�pKv�ݒ�																			*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchCalculatePrm( ANOTCHSEQ *ANotchSeq, USHORT kv, USHORT jrate)
{
	/*----------------------------------------------------------------------------------------------*/
	/*	�����m�b�`�v�Z�p�Q�C���p�����[�^�擾														*/
	/*----------------------------------------------------------------------------------------------*/
	ANotchSeq->conf.kv = kv;
	ANotchSeq->conf.jrate = jrate;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�p�����ݒ�																		*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchCalculateInitPrm( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, LONG AmpType, UNI_PRM_STR *UniPrm )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	LONG		VibDetectLevel;

	Prm = UniPrm->Prm;
	Bprm = UniPrm->Bprm;

	if( AmpType == MOTTYPE_LINEAR )
	{
		VibDetectLevel = NOTCH_BASIC_VIB_LEVEL_L;
	}
	else
	{
		VibDetectLevel = NOTCH_BASIC_VIB_LEVEL;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�����m�b�`�p�U�����x��																		*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	ANotchSeq->conf.VibCheckLevelForTunLess =
		BprmSpeedLevelCal( Bprm, (VibDetectLevel + (Bprm->SpdDetectUnit /2 )), 0 );
#else
	ANotchSeq->conf.VibCheckLevelForTunLess =
		BprmSpeedLevelCal( Bprm, (VibDetectLevel + (Bprm->SpdDetectUnit >> 1)), 0 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*	�����l�͑�PKv�A�C�i�[�V����ݒ�l															*/
	/*----------------------------------------------------------------------------------------------*/
	ANotchSeq->conf.kv = Prm->loophz;
	ANotchSeq->conf.jrate = Prm->jrate;
	/*----------------------------------------------------------------------------------------------*/
	/*	�����m�b�`�t�B���^�^�C�v�����l																*/
	/*----------------------------------------------------------------------------------------------*/
	TuneFilGainCalbyFilType( ANotch, NOTCH_FILTYPE );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�������X�����m�b�`�V�[�P���X																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v:�ʏ�^�]��ԂŐU�����o����ю��g����͂��s���m�b�`�t�B���^��ݒ肷��						*/
/*		 �ݒ肷��̂́A�m�b�`���g���A�m�b�`�p�l����уg���N�w�߃t�B���^								*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchFilterSetting( ANOTCHSEQCTRL *ANotchCtrl )
{
	LONG			Freq;
	LONG			FreqCmp;
	LONG			DetectFreqMin;
	ANOTCHSEQ		*ANotchSeq;
	AUTONOTCH		*ANotch;
	FFTANALYZE		*FftAna;
	DETVIB			*DetVib;
	REGIF_CONFIG_T	*RegCfg;
	PRMDATA			*Prm;

	/* �e�|�C���^�̐ݒ� */
	ANotchSeq = ANotchCtrl->ANotchSeq;
	ANotch = ANotchCtrl->ANotch;
	FftAna = ANotchCtrl->FftAna;
	DetVib = ANotchCtrl->DetVib;
	RegCfg = ANotchCtrl->RegCfg;
	Prm = ANotchCtrl->Prm;

	/*----------------------------------------------------------------------------------------------*/
	/*	�����m�b�`���s����																			*/
	/*----------------------------------------------------------------------------------------------*/
	if( ANotchSeq->conf.TuneLessSts == FALSE )
	{ /* �������X������� */
		return;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�T�[�{����p�g���[�X																		*/
	/*----------------------------------------------------------------------------------------------*/
	ANotchSeq->var.FftStat =
		TuneVibrationTrace( DetVib, FftAna, ANotchSeq->var.FftReq, 0, ANotchSeq->conf.TuneLessSts );

	/*----------------------------------------------------------------------------------------------*/
	/*	�����m�b�`���s��������																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F00) == 0x0000 )
	{ /* �����ݒ�\�ȃm�b�`���Ȃ� */
		/* �����ݒ�s�X�e�[�^�X�ݒ� */
		ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	OT���͒��̓m�b�`�ݒ�҂�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( (ANotchSeq->conf.OverTravelSts == TRUE) && (FftGetForceAnaReq( FftAna ) == FALSE) )
	{ /* OT�� && �T�[�{�����m�b�`���g�����o�������s�Ȃ� */
		/* �U�����o�҂��t���OON */
		ANotchSeq->var.NotchChk = TRUE;
		/* �U�����o�҂��^�C�}���Z�b�g */
		KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	���g����͗v��																				*/
	/*----------------------------------------------------------------------------------------------*/
	if( autoNotchCheckCondition(ANotchSeq, DetVib, Prm) == OK )
	{
		/* �O�̂��ߋ������g����͗v���N���A */
		FftSetForceAnaReq( FftAna, FALSE );
		ANotchSeq->var.FftReq = TRUE;
		ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DETECT;

		/* �����x�s�[�N�l�N���A */
		DetVibResetMaxMinSpeedError( DetVib );
	}

	/*------------------------------------------------------------------------------------------*/
	/*	�m�b�`�t�B���^�ݒ�																		*/
	/*------------------------------------------------------------------------------------------*/
	if( ANotchSeq->var.FftStat == TUNE_FREQ_SEARCHEND )
	{ /* ���g����͐���I���� */
		/* �g���[�X�v��OFF */
		ANotchSeq->var.FftReq = FALSE;

		/*--------------------------------------------------------------------------------------*/
		/*	���o���g�������ݒ�																	*/
		/*--------------------------------------------------------------------------------------*/
		if( FftGetForceAnaReq( FftAna ) == FALSE )
		{	/* �������oOFF */
			DetectFreqMin = ANOTCH_MIN_SET_FREQ;
		}
		else
		{	/* �������o */
			DetectFreqMin = ANOTCH_MIN_SET_FREQ2;
			/* �������g����͗v���N���A */
			FftSetForceAnaReq( FftAna, FALSE );
		}

		/*--------------------------------------------------------------------------------------*/
		/*	�m�b�`���g���␳																	*/
		/*--------------------------------------------------------------------------------------*/
		/* �m�b�`���g���␳�l�擾 */
		FreqCmp = (LONG)FftGetAnaRsltVibmRf( FftAna );
		/* �����m�b�`���g���␳ */
		Freq = autoNotchFreqComp( ANotchSeq, FreqCmp );

		/*--------------------------------------------------------------------------------------*/
		/*	�����m�b�`�t�B���^�^�C�v�ݒ�														*/
		/*--------------------------------------------------------------------------------------*/
		if( ANotchSeq->conf.TuneLessSts == TRUE )
		{ /* �������X�L�� */
			if( Freq >= 2000 )
			{	/* 2000Hz�ȏ�̓t�B���^�^�C�v3 */
				TuneFilGainCalbyFilType( ANotch, NOTCH_FILTYPE2 );
			}
			else
			{
				TuneFilGainCalbyFilType( ANotch, NOTCH_FILTYPE );
			}
		}

		/* �m�b�`�ݒ�@*/
		if( Freq >= DetectFreqMin )
		{ /* 200Hz�����̓m�b�`�t�B���^�g�p���Ȃ� */
			/* �����m�b�`�t�B���^�ݒ���Z�b�g */
			TuneSetNotchFilSettingData( ANotch, (USHORT)Freq, ANotchSeq->conf.kv, ANotchSeq->conf.jrate );

			/* �����m�b�`�t�B���^�ݒ菈�� */
			ANotchSeq->var.ANotchSetStat = TuneNotchFilterSetting( ANotch, Prm, RegCfg );

			/* �m�b�`�ݒ�X�e�[�^�X�쐬 */
			switch( ANotchSeq->var.ANotchSetStat & 0x0F )
			{
			case OPTUNE_STS_NOTCH1_OK: /* 1�i�ڃm�b�`�ݒ萳�튮�� */
				/* �m�b�`�ݒ�EEPROM�������� */
				autoNotchPrmWriteEeprom( RegCfg, ANotchSeq->conf.EepromWriteProtect );
				ANotchSeq->var.ANotch1Stat = OPTUNE_STS_NT1_COMP;
				ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NT1_COMP;
				break;

			case OPTUNE_STS_NOTCH2_OK: /* 2�i�ڃm�b�`�ݒ萳�튮�� */
				/* �m�b�`�ݒ�EEPROM�������� */
				autoNotchPrmWriteEeprom( RegCfg, ANotchSeq->conf.EepromWriteProtect );
				ANotchSeq->var.ANotch2Stat = OPTUNE_STS_NT2_COMP;
				ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NT2_COMP;
				break;

			case OPTUNE_STS_NOTCHQ1_ERR: /* 1�i�ڃm�b�`Q�l�ُ� */
				ANotchSeq->var.ANotch1Stat = OPTUNE_STS_NT1_NG;
				break;

			case OPTUNE_STS_NOTCHQ2_ERR: /* 2�i�ڃm�b�`Q�l�ُ� */
				ANotchSeq->var.ANotch2Stat = OPTUNE_STS_NT2_NG;
				break;

			default:
				break;
			}

			/* �U�������o�X�e�[�^�X�ݒ� */
			ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NODETECT;
		}
		else
		{ /* ���o�ŏ��l�ȉ��̏ꍇ */
			/* ���o�G���[�X�e�[�^�X�ݒ� */
			ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DET_ERR;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	�U�����o�҂�����																		*/
		/*		���g����͌�́A�m�b�`ON/OFF�Ɋւ�炸�A��U���o�𒆎~����B						*/
		/*		����ɍēx�A���g����͂��Ă��������ʂɂ����Ȃ�Ȃ����߁B							*/
		/*------------------------------------------------------------------------------------------*/
		/* �U�����o�҂��t���OON */
		ANotchSeq->var.NotchChk = TRUE;
		/* �U�����o�҂��^�C�}���Z�b�g */
		KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );
	}
	else if( ANotchSeq->var.FftStat == TUNE_FREQ_SEARCHERR )
	{	/* �G���[���� */
		ANotchSeq->var.FftReq = FALSE;
		/* �������g����͗v���N���A */
		FftSetForceAnaReq( FftAna, FALSE );
		/* ���o�G���[�X�e�[�^�X�ݒ� */
		ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DET_ERR;

		/*------------------------------------------------------------------------------------------*/
		/*	�U�����o�҂�����																		*/
		/*		���g����͌�́A�m�b�`ON/OFF�Ɋւ�炸�A��U���o�𒆎~����B						*/
		/*		����ɍēx�A���g����͂��Ă��������ʂɂ����Ȃ�Ȃ����߁B							*/
		/*------------------------------------------------------------------------------------------*/
		/* �U�����o�҂��t���OON */
		ANotchSeq->var.NotchChk = TRUE;
		/* �U�����o�҂��^�C�}���Z�b�g */
		KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );
	}

	/*------------------------------------------------------------------------------------------*/
	/*	�m�b�`�ݒ�҂�																			*/
	/*------------------------------------------------------------------------------------------*/
	if( ANotchSeq->var.NotchChk == TRUE )
	{	/* �m�b�`�ݒ芮���� */
		if( KlibGetLongTimerMs( ANotchSeq->var.NotchChkTimer ) > ANOTCH_NOTCH_SET_CHK_TIME )
		{
			/* �U�������҂����̓s�[�N�l���펞�N���A */
			DetVibResetMaxMinSpeedError( DetVib );
			/* �`�F�b�N�҂����� */
			ANotchSeq->var.NotchChk = FALSE;
			/* �m�b�`�ݒ菉����Ԃֈڍs */
			ANotchSeq->var.ANotchSetStat = OPTUNE_STS_NOTCH_INIT;
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�����`�F�b�N																		*/
/*																									*/
/****************************************************************************************************/
static	SHORT	autoNotchCheckCondition( ANOTCHSEQ *ANotchSeq, DETVIB *DetVib, PRMDATA *Prm )
{
	SHORT	rc;
	LONG	SpdErrPeak;
	rc = NG;

	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F00) != 0x0000 )
	{ /* �����ݒ�\�ȃm�b�`���� */
		if( (ANotchSeq->var.NotchChk == FALSE) && (ANotchSeq->conf.BaseEnableSts == TRUE) )
		{ /* �`�F�b�N�҂������� && �T�[�{�I���� */
			/* �U�����o�p���x�΍��� Peak to Peak �擾 */
			SpdErrPeak = DetVibGetSpdErrPeak( DetVib );
			if( SpdErrPeak > ANotchSeq->conf.VibCheckLevelForTunLess )
			{ /* �U�����o���x�����U�����x������ */
				rc = OK;
			}
		}
	}
	return( rc );
}





/****************************************************************************************************/
/*																									*/
/*		�����m�b�`���g���␳(�␳���͎������ʂ�蓱�o)												*/
/*																									*/
/****************************************************************************************************/
static	LONG	autoNotchFreqComp( ANOTCHSEQ *ANotchSeq, LONG DFreq )
{
	LONG	Freq;

	/* �␳���g�������� */
	Freq = DFreq;

	if( ANotchSeq->conf.TuneLessSts == TRUE )
	{ /* �������X�L�� */
		if( DFreq < NOTCH_COMP_MAX_FREQ )
		{
			Freq = DFreq;
		}
		else
		{
			/* ���g����͕���\�~�Q��␳(���o�덷) */
			Freq = DFreq + (FREQDETUNIT << 1);
		}
	}

	if( Freq < 0 )
	{
		Freq = 0;
	}

	return( Freq );
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�p�����[�^EEPROM�������ݏ���														*/
/*																									*/
/****************************************************************************************************/
static	PRM_RSLT	autoNotchPrmWriteEeprom( REGIF_CONFIG_T *RegCfg, BOOL EepromWriteProtect )
{
	PRM_RSLT	rc = PRM_RSLT_SUCCESS;

	if( EepromWriteProtect == FALSE )
	{ /* EEPROM�������݋֎~��(AAT��)�łȂ� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );	/* Pn409 */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );		/* Pn40A */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );	/* Pn40C */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );		/* Pn40D */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );	/* Pn401 */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );	/* Pn408 */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
	}
	return	rc;
}



/****************************************************************************************************/
/*																									*/
/*		��P�m�b�`�����X�e�[�^�X������API															*/
/*																									*/
/****************************************************************************************************/
void	RpiResetStatusANotch1( ANOTCHSEQ *ANotchSeq )
{
	ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NODETECT;			/* �U�������o�X�e�[�^�X�ݒ�			*/
	ANotchSeq->var.ANotch1Stat = OPTUNE_STS_NT1_NODETECT;		/* 1�i�ڃX�e�[�^�X�𖢎��{�ɏ�����	*/
	ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;		/* ���O�m�b�`�𖢎��{�ɏ�����		*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		��Q�m�b�`�����X�e�[�^�X������API															*/
/*																									*/
/****************************************************************************************************/
void	RpiResetStatusANotch2( ANOTCHSEQ *ANotchSeq )
{
	ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NODETECT;			/* �U�������o�X�e�[�^�X�ݒ�			*/
	ANotchSeq->var.ANotch2Stat = OPTUNE_STS_NT2_NODETECT;		/* 2�i�ڃX�e�[�^�X�𖢎��{�ɏ�����	*/
	ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;		/* ���O�m�b�`�𖢎��{�ɏ�����		*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�O��ݒ�m�b�`�X�e�[�^�X�擾API													*/
/*																									*/
/****************************************************************************************************/
UCHAR	RpiGetANotchPreStat( ANOTCHSEQ *ANotchSeq )
{
	return	ANotchSeq->var.ANotchPreStat;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�X�e�[�^�X�擾API																	*/
/*																									*/
/****************************************************************************************************/
USHORT	RpiGetANotchSetStat( ANOTCHSEQ *ANotchSeq )
{
	return	ANotchSeq->var.ANotchSetStat;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�X�e�[�^�X�擾API(���W�X�^i/F)													*/
/*																									*/
/****************************************************************************************************/
void	RpiSetRegSpatANotchState( ANOTCHSEQ *ANotchSeq )
{
	USHORT	uswk;

	uswk = (ANotchSeq->var.ANotchPreStat << 12)
			+ (ANotchSeq->var.ANotch2Stat << 8)
			+ (ANotchSeq->var.ANotch1Stat << 4)
			+ (ANotchSeq->var.ANotchStat);

	ANotchSeq->OpeReg.SpatANotchState = uswk;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�������g����͏����v���Z�b�g																*/
/*																									*/
/****************************************************************************************************/
void	RpiANotchFftReqSet( ANOTCHSEQ *ANotchSeq, FFTANALYZE *FftAna )
{
	ANotchSeq->var.FftReq = TRUE;
	ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DETECT;

	/* �������g����͗v�� */
	FftSetForceAnaReq( FftAna, TRUE );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�g���N�t�B���^�v�Z																			*/
/*																									*/
/****************************************************************************************************/
void	RpiANotchTrqFilCalc( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm )
{
	TuneNotchQ( ANotch, RegCfg, ANotchSeq->conf.kv, ANotchSeq->conf.jrate, 1, Prm );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�U�����o���������v���Z�b�g		3�b�ԐU�����o����											*/
/*																									*/
/****************************************************************************************************/
void	RpiANotchDisableReqSet( ANOTCHSEQ *ANotchSeq )
{
	ANotchSeq->var.NotchChk = TRUE;									/* �U�����o�҂��t���OON			*/
	KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );				/* �U�����o�҂��^�C�}���Z�b�g	*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�����m�b�`�t�B���^�ݒ�p�f�[�^�ݒ�API														*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchSetCondition( ANOTCHSEQ *ANotchSeq, BOOL EepromWriteProtect,
										BOOL BaseEnableSts, BOOL TuneLessSts, BOOL OverTravelSts )
{
	/* �e���s�����̐ݒ� */
	ANotchSeq->conf.EepromWriteProtect = EepromWriteProtect;		/* EEPROM�������݋֎~�t���O		*/
	ANotchSeq->conf.BaseEnableSts = BaseEnableSts;					/* �T�[�{ON/OFF��ԃt���O		*/
	ANotchSeq->conf.TuneLessSts = TuneLessSts;						/* �������X�L���E�����t���O		*/
	ANotchSeq->conf.OverTravelSts = OverTravelSts;					/* P-OT/N-OT��ԃt���O			*/
}



/***************************************** end of file **********************************************/
