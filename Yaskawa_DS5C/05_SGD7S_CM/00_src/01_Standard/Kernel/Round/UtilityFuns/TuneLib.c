/****************************************************************************
Description	: Tuning Function Library
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.11.16    created

*****************************************************************************/
#include "TuneLib.h"
#include "PnPrmListTbl.h"
#include "ResVibCtrl.h"
#include "MLib.h"



/****************************************************************************************************/
/*																									*/
/*		Function Definition																			*/
/*																									*/
/****************************************************************************************************/
/* PRM_RSLT	TunePrmWriteRam( REG_ACC_T *AccessPrm, CPRM_ATTR_T *PrmAttr, LONG Data, LONG Size );	*/
/* USHORT	TunePrmLimitUnsigned( ULONG Input, CPRM_ATTR_T *PrmAttr );								*/
/* SHORT	TunePrmLimitSigned( LONG Input, CPRM_ATTR_T *PrmAttr );									*/
/* USHORT	TuneKvFuncTi( USHORT Kv, BOOL IPSpdControl );											*/
/* USHORT	TuneKvFuncKp( USHORT Kv, USHORT Ti, BOOL IPSpdControl );								*/
/* USHORT	TuneKvFuncDobsgn( USHORT Kv );															*/
/* SHORT	TuneKvFuncDobLpFil( USHORT Kv, USHORT Dobsgn );											*/
/* SHORT	TuneNotchQ( AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, USHORT kv,						*/
/*													USHORT jrate, UCHAR sel, PRMDATA *Prm );		*/
/* void		TuneGetNotchQSetting( AUTONOTCH *ANotch, USHORT *NotchQ1,								*/
/*															USHORT *NotchQ2, USHORT *TrqFil );		*/
/* USHORT	TuneNotchModKv( USHORT kv, USHORT jrate, USHORT j1rate );								*/
/* SHORT	TuneNotchQset( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq,			*/
/*																USHORT Antfreq, USHORT *autoQ );	*/
/* USHORT	TuneNotchTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv,									*/
/*													USHORT jrate, USHORT Nfreq, USHORT autoQ_Q1 );	*/
/* USHORT	TuneOnePrmTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate );					*/
/* SHORT	TuneNotchQset2( USHORT freq, USHORT Nfreq, USHORT *autoQ );								*/
/* void		TuneFilGainCalbyFilType( AUTONOTCH *ANotch, UCHAR Type );								*/
/* void		TuneMfcActive( PRMDATA *Prm );															*/
/* void		TuneMfcActiveStMode( PRMDATA *Prm );													*/
/* void		TuneDobsActive( PRMDATA *Prm );															*/
/* SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm );							*/
/* SHORT	TuneNotchFilterSetting( AUTONOTCH *ANotch, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );		*/
/* SHORT	TuneVibrationTrace( DETVIB *DetVib, FFTANALYZE *FftAna,							*/
/*											UCHAR TraceRequest, SHORT TrgLvl, BOOL TuneLessSts );	*/
/* USHORT	TuneMfcKpmCalcAntFreq( USHORT AntFreq );												*/
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		AAT�A�����p���ł�Tf�v�Z�A�m�b�`Q�l�v�Z�Ɏg�p�����Kv��AJ1�̃e�[�u���B						*/
/*		�������x���ݒ�ɂ���ăe�[�u�����I�������B												*/
/*																									*/
/*			�z��̗v�f																				*/
/*				�@����Tf�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�					[�{]							*/
/*				�A�m�b�`ON���̎���Tf�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�		[�{]							*/
/*				�B�m�b�`Q�l�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�				[�{]							*/
/*				�C����Tf�v�Z�̃��[�^���������[�����g�� 				[%]								*/
/*				�D�m�b�`ON���̎���Tf�v�Z�̃��[�^���������[�����g��	[%]								*/
/*				�E�m�b�`ON����Q�l�v�Z�̃��[�^���������[�����g��		[%]								*/
/*				�F����Tf�v�Z�ŏ��ݒ�l								[0.01ms]						*/
/*				�G�m�b�`Q�l�ݒ�ŏ��l								[0.01]							*/
/*																									*/
/****************************************************************************************************/
const AUTONOTCH_PRM ANotchTbl[ 4 ] =
{
	{	4,	6,	6,	65535,	65535,	65535,	10,		50},	/* ���� 0 �Q�C�� */
	{	4,	6,	6,	200,	200,	400,	10,		50},	/* ���� 1 �Q�C�� */
	{	4,	4,	6,	400,	400,	200,	10,		150},	/* ���� 2 �Q�C�� */
	{	4,	4,	6,	65535,	65535,	65535,	10,		70},	/* ���� 3 �Q�C�� */
};



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^����RAM��������(�p�����[�^�v�Z����)												*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TunePrmWriteRam( REG_ACC_T *AccessPrm, CPRM_ATTR_T *PrmAttr, LONG Data, LONG Size )
{
	PRM_RSLT	rslt;

	AccessPrm->cPrmDef = PrmAttr;
	rslt = PrmWriteValue( AccessPrm, &Data, Size );

	return	rslt;
}



/****************************************************************************************************/
/*																									*/
/*		SHORT�T�C�Y�p�����[�^�ݒ�l�㉺�����~�b�g����(�����Ȃ��p�����[�^�p)							*/
/*																									*/
/****************************************************************************************************/
USHORT	TunePrmLimitUnsigned( ULONG Input, CPRM_ATTR_T *PrmAttr )
{
	USHORT	Output;

	if( Input > (USHORT)(PrmAttr->UpperLimit) )
	{
		Output = (USHORT)(PrmAttr->UpperLimit);
	}
	else if( Input < (USHORT)(PrmAttr->LowerLimit) )
	{
		Output = (USHORT)(PrmAttr->LowerLimit);
	}
	else
	{
		Output = (USHORT)Input;
	}

	return	Output;
}



/****************************************************************************************************/
/*																									*/
/*		SHORT�T�C�Y�p�����[�^�ݒ�l�㉺�����~�b�g����(��������p�����[�^�p)							*/
/*																									*/
/****************************************************************************************************/
SHORT	TunePrmLimitSigned( LONG Input, CPRM_ATTR_T *PrmAttr )
{
	SHORT	Output;

	if( Input > (SHORT)(PrmAttr->UpperLimit) )
	{
		Output = (SHORT)(PrmAttr->UpperLimit);
	}
	else if( Input < (SHORT)(PrmAttr->LowerLimit) )
	{
		Output = (SHORT)(PrmAttr->LowerLimit);
	}
	else
	{
		Output = (SHORT)Input;
	}

	return	Output;
}



/****************************************************************************************************/
/*																									*/
/*		Kv�A����Ti�ݒ�																				*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	AAT�ƃ����p���ɂĎg�p�����B														*/
/****************************************************************************************************/
USHORT	TuneKvFuncTi( USHORT Kv, BOOL IPSpdControl )
{
	ULONG	work;
	USHORT	Ti;

	if( IPSpdControl )
	{ /* I-P���䎞 */
		/* ���x���[�v�ϕ����萔�ݒ� */
		work = (ULONG)(TUNE_IP_TIKV / Kv);
	}
	else
	{ /* PI���䎞 */
		/* ���x���[�v�ϕ����萔�ݒ� */
		work = (ULONG)(TUNE_PI_TIKV / Kv);
	}

	Ti = TunePrmLimitUnsigned( work, &pndef_pitime );

	return	Ti;
}


/****************************************************************************************************/
/*																									*/
/*		Kv�A����Kp�ݒ�																				*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	AAT�̃��f���Ǐ]����̂Ƃ��̃o�����X�A���f���Ǐ]����̂Ƃ��̐����o�����X				*/
/*				AAT�ƃ����p���ɂĎg�p�����B														*/
/****************************************************************************************************/
USHORT	TuneKvFuncKp( USHORT Kv, USHORT Ti, BOOL IPSpdControl )
{
	ULONG	work;
	USHORT	Kp;

	if( IPSpdControl )
	{ /* I-P���䎞	��=1.0 */
		work = (ULONG)(TUNE_IP_KPTI / Ti);
	}
	else
	{ /* PI���䎞	��=1.25 */
		/* Kp[1/s] = Kv[rad/s] / 4 �� 1.5 *Kv[Hz] */
		work = (ULONG)(Kv * 3) >> 1;
	}

	Kp = TunePrmLimitUnsigned( work, &pndef_posgn );

	return	Kp;
}


/****************************************************************************************************/
/*																									*/
/*		Kv�A���̊O���I�u�U�[�o�ݒ�																	*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	AAT�ƃ����p���ɂĎg�p�����B														*/
/****************************************************************************************************/
USHORT	TuneKvFuncDobsgn( USHORT Kv )
{
	ULONG	work;
	USHORT	dobgn;

	/* Kds = Kv * 100% */
	work = TUNE_DOBGNRATIO;

	dobgn = TunePrmLimitUnsigned( work, &pndef_dobgn );

	return	dobgn;
}



/****************************************************************************************************/
/*																									*/
/*		Kv�A���̊O���I�u�U�[�o�ݒ�																	*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	AAT�ƃ����p���ɂĎg�p�����B														*/
/****************************************************************************************************/
SHORT	TuneKvFuncDobLpFil( USHORT Kv, USHORT Dobsgn )
{
	ULONG	work;
	SHORT	dlpfil;

	/* Kv * 4 */
	work = 4 * Kv;

	/* 400Hz �Ń��~�b�g */
	if( work > TUNE_MAX_DLPFIL )
	{
		work = TUNE_MAX_DLPFIL;
	}

	dlpfil = (SHORT)( Dobsgn - (USHORT)work );
	dlpfil = TunePrmLimitSigned( (LONG)dlpfil, &pndef_dlpfil );

	return	dlpfil;
}




/****************************************************************************************************/
/*																									*/
/*		�m�b�`�t�B���^Q�l�����v�Z�I��																*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	1�i�ځA2�i�ڃm�b�`�t�B���^�̗L���^�����A�m�b�`���g���̑召�֌W�𔻕ʂ��A			*/
/*				Kv�ƃC�i�[�V���ɉ�����Q�l�̍Čv�Z���s���B											*/
/*				�m�b�`�t�B���^�������́A���݂̐ݒ�l��Ԃ��B										*/
/****************************************************************************************************/
SHORT	TuneNotchQ( AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, USHORT kv, USHORT jrate, UCHAR sel, PRMDATA *Prm )
{
	SHORT			rc;	
	USHORT			wk_notchQ;
	REG_ACC_T		AccessPrm;
	AUTONOTCH_PRM	*ANotchPrm;
	AUTONOTCH_DATA	*ANotchData;

	rc = OK;
	ANotchPrm = &(ANotch->ANotchPrm);
	ANotchData = &(ANotch->ANotchData);

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/*----------------------------------------------------------------------------------*/
	/*	�ϐ��̏������i���݂̐ݒ�l�j													*/
	/*----------------------------------------------------------------------------------*/
	/* 1�i�ڃm�b�`Q�l */
	ANotchData->var.NotchQ1 = Prm->ntchq1;
	/* 2�i�ڃm�b�`Q�l */
	ANotchData->var.NotchQ2 = Prm->ntchq2;
	/* 1�i�ڃg���N�w�߃t�B���^ */
	ANotchData->var.TrqFil = Prm->trqfil11;

	if( (Prm->tfuncsw & 0x000F) == 0x0001 )
	{ /* 1�i�ڃm�b�`�L�� */
		if( (Prm->tfuncsw & 0x0F00) == 0x0100 )
		{ /* 1�i�ځA2�i�ڃm�b�`�L�� */
			/*--------------------------------------------------------------------------*/
			/*	�m�b�`���g���̒Ⴂ���́AKv�EJL�ɘA������Q�l���v�Z����B					*/
			/*	�m�b�`���g���̍������́A�Ⴂ���̃m�b�`���g���ɘA������Q�l���v�Z����B	*/
			/*--------------------------------------------------------------------------*/
			if( Prm->ntchfil1 > Prm->ntchfil2 )
			{
				rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil2, TUNE_ANTFRQ_NTQ2, &wk_notchQ );
				/*----------------------------------------------------------------------*/
				/*	2�i�ڃm�b�`Q�l�v�Z													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x0F00) == 0x0100 )
				{
					ANotchData->var.NotchQ2 = wk_notchQ;
					/*------------------------------------------------------------------*/
					/*	Tf�v�Z															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = 
						TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil2, ANotchData->var.NotchQ2 );
				}
				else
				{
					/*------------------------------------------------------------------*/
					/*	Tf�v�Z															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
				}
				/*----------------------------------------------------------------------*/
				/*	1�i�ڃm�b�`Q�l�v�Z													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x000F) == 0x0001 )
				{
					rc |= TuneNotchQset2( Prm->ntchfil2, Prm->ntchfil1, &ANotchData->var.NotchQ1 );
				}
			}
			else
			{
				rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil1, TUNE_ANTFRQ_NTQ1, &wk_notchQ );
				/*----------------------------------------------------------------------*/
				/*	1�i�ڃm�b�`Q�l�v�Z													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x000F) == 0x0001 )
				{
					ANotchData->var.NotchQ1 = wk_notchQ;
					/*------------------------------------------------------------------*/
					/*	Tf�v�Z															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = 
						TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil1, ANotchData->var.NotchQ1 );
				}
				else
				{
					/*------------------------------------------------------------------*/
					/*	Tf�v�Z															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
				}
				/*----------------------------------------------------------------------*/
				/*	2�i�ڃm�b�`Q�l�v�Z													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x0F00) == 0x0100 )
				{
					rc |= TuneNotchQset2( Prm->ntchfil1,Prm->ntchfil2,&ANotchData->var.NotchQ2 );
				}
			}
		}
		else
		{ /* 1�i�ڃm�b�`�̂ݗL�� */
			rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil1, TUNE_ANTFRQ_NTQ1, &wk_notchQ );
			/*--------------------------------------------------------------------------*/
			/*	1�i�ڃm�b�`Q�l�v�Z														*/
			/*--------------------------------------------------------------------------*/
			if( (Prm->anotchsw & 0x000F) == 0x0001 )
			{
				ANotchData->var.NotchQ1 = wk_notchQ;
			/*----------------------------------------------------------------------*/
			/*	Tf�v�Z																*/
			/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = 
					TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil1, ANotchData->var.NotchQ1 );
			}
			else
			{
				/*----------------------------------------------------------------------*/
				/*	Tf�v�Z																*/
				/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
			}
		}
	}
	else
	{ /* 1�i�ڃm�b�`���� */
		if( (Prm->tfuncsw & 0x0F00) == 0x0100 )
		{ /* 2�i�ڃm�b�`�̂ݗL�� */
			rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil2, TUNE_ANTFRQ_NTQ2, &wk_notchQ );
			/*--------------------------------------------------------------------------*/
			/*	2�i�ڃm�b�`Q�l�v�Z														*/
			/*--------------------------------------------------------------------------*/
			if( (Prm->anotchsw & 0x0F00) == 0x0100 )
			{
				ANotchData->var.NotchQ2 = wk_notchQ;
				/*----------------------------------------------------------------------*/
				/*	Tf�v�Z																*/
				/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = 
					TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil2, ANotchData->var.NotchQ2 );
			}
			else
			{
				/*----------------------------------------------------------------------*/
				/*	Tf�v�Z																*/
				/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
			}
		}
		else
		{ /* 1�i�ځA2�i�ڂƂ��ɖ��� */
			/* �m�b�`�t�B���^���� */
			rc = -3;
			ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
		}
	}
	/*----------------------------------------------------------------------------------*/
	/*	�v�Z�l��RAM�ɔ��f																*/
	/*----------------------------------------------------------------------------------*/
	if( (sel == 1) || ((sel == 2) && (rc == OK)) ) /* sel==2 ���́A�v�ZOK���̂ݕύX����	*/
	{ /* �v�Z���e��RAM�ɔ��f(����!! EEPROM�ɂ͏����Ȃ�) */
		/* Pn401 */
		TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (LONG)ANotchData->var.TrqFil, 2 );
		/* Pn40A */
		TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchData->var.NotchQ1, 2 );
		/* Pn40D */
		TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchData->var.NotchQ2, 2 );
	}
	return	rc;
}



/****************************************************************************************************/
/*																									*/
/*		�m�b�`�t�B���^Q�l�v�Z���ʎ擾API															*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	�m�b�`�t�B���^Q�l�����v�Z�I������(TuneNotchQ)�̌v�Z���ʂ��擾����B					*/
/*				1�i�ڃm�b�`Q�l�A2�i�ڃm�b�`Q�l�A1�i�ڃg���N�w�߃t�B���^��Ԃ��B						*/
/****************************************************************************************************/
void	TuneGetNotchQSetting( AUTONOTCH *ANotch, USHORT *NotchQ1, USHORT *NotchQ2, USHORT *TrqFil )
{
	/* 1�i�ڃm�b�`Q�l */
	*NotchQ1 = ANotch->ANotchData.var.NotchQ1;
	/* 2�i�ڃm�b�`Q�l */
	*NotchQ2 = ANotch->ANotchData.var.NotchQ2;
	/* 1�i�ڃg���N�w�߃t�B���^ */
	*TrqFil = ANotch->ANotchData.var.TrqFil;
}



/****************************************************************************************************/
/*																									*/
/*		Tf�E�m�b�`Q�l�v�Z�̂��߂�Kv�␳�v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	�����Ŏw�肳�ꂽKv�A�C�i�[�V����A�{������Kv�ɑ΂���C�i�[�V���␳���s���B			*/
/*																									*/
/*							Jrate[%] + 100															*/
/*				Kv' = SQRT(-----------------) * Kv													*/
/*								J1[%]																*/
/*													kv		�F�␳�OKv						[Hz]	*/
/*													Jrate	�F�␳�p���׊������[�����g��	[%]		*/
/*													J1		�F���[�^���������[�����g��		[%]		*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneNotchModKv( USHORT kv, USHORT jrate, USHORT j1rate )
{
	LONG	work;
	USHORT	TLV_kv;

	if( j1rate == 0 )
	{	/* J1��0[%]�̎� �� ���U�A�����U���g������v�Z */
		/*----------------------------------------------------------*/
		/*	�C�i�[�V����␳�v�Z									*/
		/*				  		Freq_res[Hz]						*/
		/*		Kv'[0.1Hz] = ----------------- * Kv[0.1Hz]			*/
		/*						Freq_ant[Hz]						*/
		/*----------------------------------------------------------*/
		work = (LONG)TUNE_RESFRQ_KV * (LONG)kv / (LONG)TUNE_ANTFRQ_KV;
	}
	else if( (jrate + 100) <= j1rate )
	{
		/* J1�ȉ��͍��̂ƌ��Ȃ� */
		work = (LONG)kv ;
	}
	else
	{
		/*----------------------------------------------------------*/
		/*	�C�i�[�V����␳�v�Z									*/
		/*				  			Jrate[%] + 100					*/
		/*		Kv'[0.1Hz] = SQRT(-----------------) * Kv[0.1Hz]	*/
		/*								J1[%]						*/
		/*----------------------------------------------------------*/
		work = 
			(LONG)kv * (LONG)MlibSqrtu32(((LONG)jrate + 100) * 10000 / (LONG)j1rate) /100;
	}

	/*--------------------------------------------------------------*/
	/*	�␳Kv �㉺���`�F�b�N										*/
	/*--------------------------------------------------------------*/
	if( work > (LONG)(pndef_loophz.UpperLimit) )
	{
		TLV_kv = (USHORT)pndef_loophz.UpperLimit;
	}
	else if( work < (LONG)kv )
	{
		TLV_kv = kv;
	}
	else
	{
		TLV_kv = (USHORT)work;
	}

	return	TLV_kv;
}



/****************************************************************************************************/
/*																									*/
/*		�m�b�`Q�l�̎����v�Z																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	�����Ŏw�肳�ꂽKv����A�m�b�`���g���ɑ΂���m�b�`Q���v�Z����B						*/
/*				�m�b�`�㉺���`�F�b�N�A�m�b�`ON�̉ۃ`�F�b�N���s���B								*/
/*																									*/
/*					   �� * Kv * fn								fn�F�m�b�`���g��			[Hz]	*/
/*				Q = ------------------							kv�F�␳��Kv				[Hz]	*/
/*					 fn^2 - ��^2 * Kv^2							���F�m�b�`Q�l�v�Z�p��Kv��	[�{]	*/
/*																									*/
/****************************************************************************************************/
SHORT	TuneNotchQset( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, 
																	USHORT Antfreq, USHORT *autoQ )
{
	SHORT	check;
	USHORT	TLV_kv;
	LONG	work1, work2, work3;
	LONG	freq_cl;
	LONG	sx, sy;
	LONG	fcl_Tf;
	SHORT	swk;

	check = OK;

	/*--------------------------------------------------------------*/
	/*	�C�i�[�V����␳�v�Z										*/
	/*--------------------------------------------------------------*/
	TLV_kv = TuneNotchModKv( kv, jrate, ANotchPrm->J1Q );

	/*--------------------------------------------------------------*/
	/*	�m�b�`������Tf[0.01ms]�v�Z									*/
	/*--------------------------------------------------------------*/
	swk = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );

	/*--------------------------------------------------------------*/
	/*	Tf����t�Z�����m�b�`�J�b�g�I�t���g��						*/
	/*	 fcl[Hz] = 1000 * 100 / (2��Tf[0.01ms])						*/
	/*--------------------------------------------------------------*/
	/* �ŏ�Tf�����l�� */
	work1 = (LONG)swk - ((LONG)ANotchPrm->TfMin - 5);
	if(work1 <= 0)
	{ /* Tf���[���ȉ� */
		/* �ݒ�s�̂��ߎ��g�����ő�l�ɂ��� */
		fcl_Tf = 10000;
	}
	else
	{
		/* �m�b�`���g���ŏ��ݒ�l 159160 = 1000 *100 /6.283 */
		fcl_Tf = (LONG)(15916 / work1);
	}

	/*--------------------------------------------------------------*/
	/*	�m�b�`�J�b�g�I�t���g���v�Z									*/
	/*--------------------------------------------------------------*/
	if( ANotchPrm->J1Q == 0 )
	{ /* ���U�A�����U���g������v�Z */
		/* (freq_res[Hz] + freq_ant[Hz]) / 2 */
		freq_cl = ((LONG)Nfreq + (LONG)Antfreq) / 2;
	}
	else
	{ /* �␳Kv�l����v�Z */
		/* �� �~ Kv'[Hz] */
		freq_cl = (LONG)ANotchPrm->KvrateQ * (LONG)TLV_kv / TUNE_ANOTCH_DECA;
	}

	/*--------------------------------------------------------------*/
	/*	�m�b�`�J�b�g�I�t���g������									*/
	/*		Tf����̌v�Z�l�ƕ␳Kv����v�Z�������g���̍�������I��	*/
	/*--------------------------------------------------------------*/
	if( freq_cl < fcl_Tf )
	{ /* �ŏ��ݒ���g�����freq_cl�����������m�b�`�ݒ�s�� */
		freq_cl = fcl_Tf;
	}

	/*--------------------------------------------------------------*/
	/*	�m�b�`Q�l�v�Z												*/
	/*					  �� * Kv'[Hz] * fn[Hz]						*/
	/*		Q[0.01] = -------------------------------				*/
	/*					fn[Hz]^2 - ��^2 * Kv'[Hz]^2					*/
	/*																*/
	/*	���U�����U���g������v�Z����ꍇ�A�㎮�̃� * Kv'[Hz]��		*/
	/*	�m�b�`�J�b�g�I�t���g��(f_cl)�Ƃ���							*/
	/*--------------------------------------------------------------*/
	if( freq_cl >= (LONG)Nfreq )
	{ /* ��Kv��Ńm�b�`Q�l�v�Z�s�\ */
		/* Q�l����l�ɐݒ� */
		work1 = TUNE_ANOTCH_Q_MAX;
		check = NG;
	}
	else
	{
		/* ���q�̌v�Z */
		/* ���~Kv[0.1Hz]'�~fn (�~100��Q�l�P��[0.01]�̂���) */
		sx = 0;
		work1 = MlibPcalKxmulx( freq_cl, (LONG)Nfreq, 100, &sx );

		/* ����̌v�Z */
		/* fn[Hz]^2 */
		work2 = (LONG)Nfreq * (LONG)Nfreq;
		work3 = freq_cl;

		sy = 0;
		/* ��^2�~Kv[Hz]'^2 */
		work3 = MlibPcalKxmulx( work3, work3, 1, &sy );
		/* fn[Hz]^2 - ��^2�~Kv[Hz]'^2 */
		work2 = MlibPcalKxsubx( work2, 0, work3, sy, &sy );

		if( work2 <= 0 )
		{
			/* Q�l����l�ɐݒ� */
			work1 = TUNE_ANOTCH_Q_MAX;
		}
		else
		{
			/* �m�b�`Q�l [0.01] */
			work1 = MlibPcalKxdivx( work1, sx, work2, sy, 24 );
			work1 = MlibKsgain2Long(work1);
		}
		/*--------------------------------------------------------------*/
		/*	�㉺���`�F�b�N												*/
		/*--------------------------------------------------------------*/
		if( work1 < ANotchPrm->NotchQMin )
		{ /* Q�l�������l��菬�����ꍇ�́A�����l�ɃN�����v */
			work1 = ANotchPrm->NotchQMin;
		}
		else if( work1 > TUNE_ANOTCH_Q_MAX_DET )
		{ /* Q�l�����o����l���傫���ꍇ�́A����l�ɃN�����v */
			work1 = TUNE_ANOTCH_Q_MAX;
			check = -2;
		}
		else if( work1 > TUNE_ANOTCH_Q_MAX )
		{ /* Q�l������l���傫���ꍇ�́A����l�ɃN�����v */
			work1 = TUNE_ANOTCH_Q_MAX;
		}
	}
	*autoQ = (USHORT)work1;

	return	check;
}



/****************************************************************************************************/
/*																									*/
/*		�m�b�`ON���̃g���N�w��1�����[�p�X�t�B���^�����v�Z											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	�����Ŏw�肳�ꂽKv����A�m�b�`���g���ɑ΂���g���N�w�߃t�B���^���萔���v�Z����B	*/
/*				�m�b�`ON�̏ꍇ�A�m�b�`�̓����x�����������Tf���Z�o����B							*/
/*				�␳�́A�m�b�`OFF����Tf���瓙���x�ꕪ�������B										*/
/*				�������A�m�b�`�����x��␳���Tf�͍ŏ��l�ŃN�����v����B							*/
/*																									*/
/*								1																	*/
/*				Tn[s] = ------------------															*/
/*						 2 * �� * f_cl[Hz]				f_cl�F�m�b�`�����J�b�g�I�t���g��	[Hz]	*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneNotchTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, USHORT autoQ_Q1 )
{
	USHORT	TLV_kv;
	LONG	work1, work2;
	LONG	sx;

	/*--------------------------------------------------------------*/
	/*	�C�i�[�V����␳�v�Z										*/
	/*--------------------------------------------------------------*/
	TLV_kv = TuneNotchModKv( kv, jrate, ANotchPrm->J1TfonNt );

	/*--------------------------------------------------------------*/
	/*	�m�b�`�t�B���^�����J�b�g�I�t���g��	[Hz]					*/
	/*																*/
	/*					 8*Q^2										*/
	/*		f_cl = ------------------- * fn[Hz]						*/
	/*				8*Q^2 + 4*Q + 1									*/
	/*																*/
	/*	�m�b�`�t�B���^�����x�ꎞ�萔	[sec]						*/
	/*																*/
	/*						1										*/
	/*		Tn[s] = ------------------								*/
	/*				 2 * �� * f_cl[Hz]								*/
	/*--------------------------------------------------------------*/
	/*	autoQ_Q1 < 1000 */
	work1 = 8 * (LONG)autoQ_Q1 * (LONG)autoQ_Q1;
	/* 400�A10000��autoQ_Q1��[0.01]�P�ʂ̂��߂̊��Z */
	work2 = work1 + 400 * (LONG)autoQ_Q1 + 10000;

	/* �J�b�g�I�t���g���Z�o	[Hz] */
	sx = 0;
	work1 = MlibPcalKxgain(work1, (LONG)Nfreq, work2 ,&sx, 0);

	/* 15915 = 100000[s->0.01ms] / 2�� */
	work1 = MlibPcalKxdivx( 15915, 0, work1, sx, 24 );
	/* �m�b�`�����x�ꎞ�萔	[0.01ms] */
	work1 = MlibKsgain2Long(work1);

	/*--------------------------------------------------------------*/
	/*	�m�b�`OFF���̃t�B���^���萔�v�Z								*/
	/*	 Tf[0.01ms] = 100000 * 10 / (2��Kv'[0.1Hz]* AUTOTf_KVRAT_NT)*/
	/*--------------------------------------------------------------*/
	/* 159160 = 1000 * 100 / 6.283 * 10 */
	work2 = 159160 / (LONG)ANotchPrm->KvrateTfonNt / (LONG)TLV_kv;

	if( ANotchPrm->J1TfonNt == 0 )
	{ /* ���U�A�����U���g������v�Z */
		/* �m�b�`�����x��������Ȃ� */
		work1 = work2;
	}
	else
	{
		/* �m�b�`�␳��g���N�w�߃t�B���^���萔 [0.01ms] */
		work1 = work2 - work1;
	}

	if( work1 < (LONG)ANotchPrm->TfMin )
	{
		/* Tf�ݒ�ŏ��l�ŃN�����v */
		work1 = (LONG)ANotchPrm->TfMin;
	}

	return	(USHORT)work1;
}


/****************************************************************************************************/
/*																									*/
/*		�g���N�w��1�����[�p�X�t�B���^�����v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	�����Ŏw�肳�ꂽKv�ɑ΂���g���N�w�߃t�B���^���萔���v�Z����B						*/
/*																									*/
/*;							1																		*/
/*				Tf = ----------------																*/
/*						2��Kv'�~��																	*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneOnePrmTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate )
{
	USHORT	TLV_kv;
	LONG	work1;

	/*--------------------------------------------------------------*/
	/*	�C�i�[�V����␳�v�Z										*/
	/*--------------------------------------------------------------*/
	TLV_kv = TuneNotchModKv( kv, jrate, ANotchPrm->J1Tf );

	/*--------------------------------------------------------------*/
	/*	�t�B���^���萔�v�Z											*/
	/*	 Tf[0.01ms] = 100000 * 10 / (2��Kv'[0.1Hz]* AUTOTf_KVRAT)	*/
	/*--------------------------------------------------------------*/
	/* 159160 = 1000 * 100 / 6.283 * 10 */
	work1 = 159160 / (LONG)ANotchPrm->KvrateTf / (LONG)TLV_kv;

	if( work1 < (LONG)ANotchPrm->TfMin )
	{
		/* Tf�ݒ�ŏ��l�ŃN�����v */
		work1 = (LONG)ANotchPrm->TfMin;
	}
	
	return	(USHORT)work1;
}



/****************************************************************************************************/
/*																									*/
/*		�m�b�`�t�B���^Q�l�����v�Z2																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	�����Ŏw�肳�ꂽ���g�����J�b�g�I�t���g���Ƃ���m�b�`Q�l���v�Z����B					*/
/*																									*/
/*;					  Nfreq �~ freq																	*/
/*				Q = -----------------																*/
/*					 Nfreq^2 - freq^2																*/
/*																									*/
/****************************************************************************************************/
SHORT	TuneNotchQset2( USHORT freq, USHORT Nfreq, USHORT *autoQ )
{
	SHORT	check;
	ULONG	autoQ_Q2;
	LONG	work1, work2;

	check = OK;

	if( freq >= Nfreq )
	{	/* �U�����g�����ڕW�J�b�g�I�t���g���ȏ� �� �v�Z�s�\ */
		autoQ_Q2 = TUNE_ANOTCH_Q_MAX;
		check = NG;
	}
	else
	{
		/* 100�{����̂́AQ�l�̒P�ʂ��p�����[�^�ɍ��킹�邽�� */
		work1 = Nfreq * Nfreq - freq * freq;
		work2 = Nfreq * freq * 100;

		autoQ_Q2 = work2 /work1;

		/* �㉺���`�F�b�N */
		if( autoQ_Q2 < TUNE_ANOTCH_Q_MIN )
		{ /* Q�l��1.50�ȉ��̏ꍇ */
			/* 1.50�ɐݒ� */
			autoQ_Q2 = TUNE_ANOTCH_Q_MIN;
		}
		else if( autoQ_Q2 > TUNE_ANOTCH_Q_MAX )
		{ /* Q�l��3.00�ȏ�̏ꍇ */
			/* 3.00�ɐݒ� */
			autoQ_Q2 = TUNE_ANOTCH_Q_MAX;
			check = -2;
		}
	}
	*autoQ = (USHORT)autoQ_Q2;

	return	check;
}


/****************************************************************************************************/
/*																									*/
/*		AAT�E�����p���@�\�������x���I������															*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	AAT�⃏���p���Őݒ肳�ꂽ�@�B�������x����������e�[�u�����Ăяo���A�ϐ��ɐݒ肷��B	*/
/****************************************************************************************************/
void	TuneFilGainCalbyFilType( AUTONOTCH *ANotch, UCHAR Type )
{
	/* ����Tf�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�[�{] */
	ANotch->ANotchPrm.KvrateTf = ANotchTbl[ Type ].KvrateTf;

	/* �m�b�`ON���̎���Tf�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�[�{] */
	ANotch->ANotchPrm.KvrateTfonNt = ANotchTbl[ Type ].KvrateTfonNt;

	/* �m�b�`Q�l�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�[�{] */
	ANotch->ANotchPrm.KvrateQ = ANotchTbl[ Type ].KvrateQ;

	/* ����Tf�v�Z�� ���������[�����g��[%] */
	ANotch->ANotchPrm.J1Tf = ANotchTbl[ Type ].J1Tf;

	/* �m�b�`ON���̎���Tf�v�Z�̃��[�^���������[�����g��[%] */
	ANotch->ANotchPrm.J1TfonNt = ANotchTbl[ Type ].J1TfonNt;

	/* �m�b�`ON����Q�l�v�Z�̃��[�^���������[�����g��[%] */
	ANotch->ANotchPrm.J1Q = ANotchTbl[ Type ].J1Q;

	/* ����Tf�v�Z�ŏ��ݒ�l[0.01ms] */
	ANotch->ANotchPrm.TfMin = ANotchTbl[ Type ].TfMin;

	/* �m�b�`Q�l�ŏ��ݒ�l[-] */
	ANotch->ANotchPrm.NotchQMin = ANotchTbl[ Type ].NotchQMin;
	
	return;
}



#if 0 /* SGDV�s�g�p�̂��߃R�����g�A�E�g */
/****************************************************************************************************/
/*																									*/
/*		���f���Ǐ]/�O���I�u�U�[�o�̊J�n																*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	���f���Ǐ]�E�O���I�u�U�[�o���J�n����B												*/
/****************************************************************************************************/
void	TuneMfcDobsActive( HMSGIF Hmsg )
{
	/* �����g�p���̂Ƃ��A�ȊO�͋���ON���p�����[�^�㏑�� */
	if( (Gprm.f.Dobs == FALSE) || (Gprm.MFCModel == 0) )
	{
		TuneMfcActive( Hmsg );
		TuneDobsActive( Hmsg );
	}

	return;
}
#endif



/****************************************************************************************************/
/*																									*/
/*		���f���Ǐ]�̊J�n																			*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	���f���Ǐ]���J�n����B���̂Ƃ��A�Ă�1�ATFF��100�AVFF��100�ɏ㏑������B				*/
/*				AAT�ƃ����p���ɂĎg�p�����B														*/
/****************************************************************************************************/
//void	TuneMfcActive( PRMDATA *Prm )
void	TuneMfcActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*--------------------------------------------------------------*/
	/* RAM�������ݗp���W�X�^���ݒ�								*/
	/*--------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	Prm->mdlzeta = TUNE_MDLZETA; /* Pn142 */
	Prm->mdlptff = TUNE_MFCKTFF; /* Pn143 */
	Prm->mdlntff = TUNE_MFCKTFF; /* Pn144 */
	Prm->mdlvff  = TUNE_MFCKVFF; /* Pn147 */

	/* Pn140 */
	data = Prm->mdlsw;
	data |= 0x0001;
	//Prm->mdlsw = data;
	TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)(data), 2 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		���f���Ǐ]�̊J�n																			*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	���f���Ǐ]���J�n����B���̂Ƃ��A�Ă�1�ATFF��100�AVFF��95�ɏ㏑������B				*/
/*				AAT�ƃ����p���ɂĎg�p�����B														*/
/****************************************************************************************************/
//void	TuneMfcActiveStMode( PRMDATA *Prm )
void	TuneMfcActiveStMode( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*--------------------------------------------------------------*/
	/* RAM�������ݗp���W�X�^���ݒ�								*/
	/*--------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	Prm->mdlzeta = TUNE_MDLZETA;  /* Pn142 */
	Prm->mdlptff = TUNE_MFCKTFF;  /* Pn143 */
	Prm->mdlntff = TUNE_MFCKTFF;  /* Pn144 */
	Prm->mdlvff  = TUNE_MFCKVFF2; /* Pn147 */

	/* Pn140 */
	data = Prm->mdlsw;
	data |= 0x0001;
	//Prm->mdlsw = data;
	TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)(data), 2 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�O���I�u�U�[�o�̊J�n																		*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	�O���I�u�U�[�o���J�n����B���̂Ƃ��AFB�Q�C����85���A�C�i�[�V���␳��100%�ɏ㏑��	*/
/*				����BAAT�ƃ����p���ɂĎg�p�����B													*/
/****************************************************************************************************/
//void	TuneDobsActive( PRMDATA *Prm )
void	TuneDobsActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*--------------------------------------------------------------*/
	/* RAM�������ݗp���W�X�^���ݒ�								*/
	/*--------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	//Prm->dtrqgn = TUNE_DTRQGN; /* Pn123 */
	//Prm->dobjgn = TUNE_DOBJGN; /* Pn125 */
	TunePrmWriteRam( &AccessPrm, &pndef_dtrqgn,  (LONG)(TUNE_DTRQGN), 2 );	/* Pn123 */
	TunePrmWriteRam( &AccessPrm, &pndef_dobjgn,  (LONG)(TUNE_DOBJGN), 2 );	/* Pn125 */

	/* Pn408 */
	data = Prm->tfuncsw;
	data |= 0x1000;
	//Prm->tfuncsw = data;
	TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)(data), 2 );			/* Pn408 */

	return;
}



/****************************************************************************************************/
/*																									*/
/*		A�^���U����ݒ菈��																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	���͂��ꂽ���g���ɑ΂��AA�^���U��L���Ƃ���B�_���s���O�Q�C���͌Œ�l�B				*/
/*																									*/
/*		arg	:	USHORT	freq			:	�U�����g��		[Hz]									*/
/*				USHORT	auton_sw		:	��������SW		[-]										*/
/*																									*/
/*		out	:	SHORT	rc				:	0x01:����ݒ芮��										*/
/*											0x10:�㉺���ُ�											*/
/*											0x11:�����ݒ�s��										*/
/*											0x12:�p�����[�^�������ُ݈�								*/
/*																									*/
/****************************************************************************************************/
//SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm )
SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	SHORT		rc;
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*------------------------------------------------------------------*/
	/* RAM�������ݗp���W�X�^���ݒ�									*/
	/*------------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/*------------------------------------------------------------------*/
	/*	�����ݒ�\�`�F�b�N											*/
	/*------------------------------------------------------------------*/
	data = Prm->avibsw;
	if( ((data & 0x0001) == 0x0001) || (auton_sw == FALSE) )
	{	/* ����A�^���U�L�� or ������������ */
		rc = 0x11;
		return	rc;
	}

	/*------------------------------------------------------------------*/
	/*	���g���㉺���`�F�b�N											*/
	/*------------------------------------------------------------------*/
	if( (freq >= AVIB_FRQ_MIN) && (freq <= AVIB_FRQ_MAX) )
	{
		/*--------------------------------------------------------------*/
		/*	A�^���U�p�����[�^�ݒ�										*/
		/*--------------------------------------------------------------*/
		//Prm->avibfrq = freq * 10;	/* Pn161 */
		//Prm->avibdamp = AVIBDAMPGN; /* Pn163 */
		TunePrmWriteRam( &AccessPrm, &pndef_avibfrq,  (LONG)(freq * 10), 2 );	/* Pn161 */
		TunePrmWriteRam( &AccessPrm, &pndef_avibdamp, (LONG)AVIBDAMPGN,  2 );	/* Pn163 */

		/*--------------------------------------------------------------*/
		/*	A�^���U�@�\�X�C�b�`�ݒ�										*/
		/*--------------------------------------------------------------*/
		data = Prm->avibsw; /* Pn160 */
		data |= 0x0001;
		//Prm->avibsw = data;
		TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (LONG)data,  2 );
		rc = 0x01;
	}
	else
	{
		rc = 0x10;
	}

	return	rc;
}



/****************************************************************************************************/
/*																									*/
/*		�m�b�`�t�B���^�ݒ菈��																		*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	��1�A��2�m�b�`�t�B���^��ݒ肷��B�m�b�`Q�l�͎����I�Ɍv�Z�����B					*/
/****************************************************************************************************/
SHORT	TuneNotchFilterSetting( AUTONOTCH *ANotch, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )
{
	CHAR			NotchType;
	SHORT			rc;
	USHORT			NotchQ;
	USHORT			freq;
	USHORT			KvGain;
	USHORT			Jrate;
	REG_ACC_T		AccessPrm;
	AUTONOTCH_DATA	ANotchWk;
	AUTONOTCH_PRM	*ANotchPrm;

	NotchType = -1;
	rc = 0x00;
	ANotchPrm = &(ANotch->ANotchPrm);
	freq = ANotch->ANotchData.conf.freq;
	KvGain = ANotch->ANotchData.conf.KvGain;
	Jrate = ANotch->ANotchData.conf.Jrate;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* ���݂�RAM���ǂݏo�� */
	ANotchWk.var.NotchSw = Prm->tfuncsw;	/* Pn408 */
	ANotchWk.var.NotchQ1 = Prm->ntchq1;		/* Pn40A */
	ANotchWk.var.NotchQ2 = Prm->ntchq2;		/* Pn40D */
	ANotchWk.var.NotchFil1 = Prm->ntchfil1;	/* Pn409 */
	ANotchWk.var.NotchFil2 = Prm->ntchfil2;	/* Pn40C */
	ANotchWk.var.TrqFil = Prm->trqfil11;	/* Pn401 */

	/* �m�b�`�^�C�v�ݒ� */
	if( ((ANotchWk.var.NotchSw & 0x0F00) == 0x0000) && ((Prm->anotchsw & 0x0F00) == 0x0100) )
	{	/* 2�i�ڃm�b�`�������ݒ�\ */
		if( (ANotchWk.var.NotchSw & 0x000F) == 0x0000 )
		{	/* 1�i�ڃm�b�`���g�p -> 2�i�ڃm�b�`��1�ڂƂ��ē���� */
			NotchType = 0;
		}
		else
		{	/* 1�i�ڃm�b�`�g�p�� -> 2�i�ڃm�b�`��2�ڂƂ��ē���� */
			if( freq > ANotchWk.var.NotchFil1 )
			{
				NotchType = 1;
			}
			else
			{
				NotchType = 4;
			}
		}
	}
	else
	{
		if( ((ANotchWk.var.NotchSw & 0x000F) == 0x0000) && ((Prm->anotchsw & 0x000F) == 0x0001) )
		{ /* 1�i�ڃm�b�`�������ݒ�\ */
			if( (ANotchWk.var.NotchSw & 0x0F00) == 0x0000 )
			{	/* 2�i�ڃm�b�`���g�p -> 1�i�ڃm�b�`��1�ڂƂ��ē���� */
				NotchType = 3;
			}
			else
			{	/* 2�i�ڃm�b�`�g�p�� -> 1�i�ڃm�b�`��2�ڂƂ��ē���� */
				if( freq > ANotchWk.var.NotchFil2 )
				{
					NotchType = 2;
				}
				else
				{
					NotchType = 5;
				}
			}
		}
	}

	/*----------------------------------------------------------------------------------*/
	/*	�m�b�`�t�B���^�ݒ� ����Q�l�v�Z													*/
	/*----------------------------------------------------------------------------------*/
	switch ( NotchType )
	{
		case 0:
		case 4:
			/*------------------------------------------------------------------------------*/
			/*	��2�m�b�`�i�Ⴂ���g���j�ݒ�													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil2.UpperLimit )
			{ /* ����l�𒴂���ꍇ�͏���l�Ƃ��� */
				freq = (USHORT)pndef_ntchfil2.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q�l�v�Z & �p�����[�^�v�Z													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset( ANotchPrm, KvGain, Jrate, freq, TUNE_ANTFRQ_NTQ2, &NotchQ ) != OK )
			{ /* Q�l�ݒ�ُ�̏ꍇ�́A�ݒ�l�����ɖ߂��m�b�`�������� */
				/* 2�i�ڃm�b�`Q�l�ُ� */
				rc = 0x08;
			}
			else
			{	/* Q�l�v�Z����I�� */
				ANotchWk.var.TrqFil = TuneNotchTfcalc( ANotchPrm, KvGain, Jrate, freq, NotchQ );
				/* 2���ڂ�1�ɂ��� */
				ANotchWk.var.NotchSw |= 0x0100;
				/* �m�b�`���g�� */
				ANotchWk.var.NotchFil2 = freq;
				/* �m�b�`Q�l */
				ANotchWk.var.NotchQ2 = NotchQ;

				if( NotchType == 4 )
				{ /* 1�i�ڃm�b�`��Q�l�X�V */
					TuneNotchQset2( ANotchWk.var.NotchFil2, ANotchWk.var.NotchFil1, &NotchQ );
					/* �m�b�`Q�l */
					ANotchWk.var.NotchQ1 = NotchQ;
				}

				ANotchWk.var.TrqFil = TunePrmLimitUnsigned( (ULONG)ANotchWk.var.TrqFil, &pndef_trqfil11 );

				/* Pn401 */
				TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (LONG)ANotchWk.var.TrqFil, 2 );
				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn40A */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchWk.var.NotchQ1, 2 );
				/* Pn40C */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil2, (LONG)ANotchWk.var.NotchFil2, 2 );
				/* Pn40D */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchWk.var.NotchQ2, 2 );

				/* 2�i�ڃm�b�`���튮�� */
				rc = 0x02;
			}
			break;

		case 1:
			/*------------------------------------------------------------------------------*/
			/*	��2�m�b�`�i�������g���j�ݒ�													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil2.UpperLimit )
			{ /* ����l�ł̃��~�b�g */
				freq = (USHORT)pndef_ntchfil2.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q�l�v�Z & �p�����[�^�v�Z													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset2( ANotchWk.var.NotchFil1, freq, &NotchQ ) != OK )
			{ /* Q�l�ݒ�ُ�̏ꍇ�́A�ݒ�l�����ɖ߂��m�b�`�������� */
				/* 2�i�ڃm�b�`Q�l�ُ� */
				rc = 0x08;
			}
			else
			{ /* Q�l�v�Z����I�� */
				/* 2���ڂ�1�ɂ��� */
				ANotchWk.var.NotchSw |= 0x0100;
				/* �m�b�`���g�� */
				ANotchWk.var.NotchFil2 = freq;
				/* �m�b�`Q�l */
				ANotchWk.var.NotchQ2 = NotchQ;

				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn40C */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil2, (LONG)ANotchWk.var.NotchFil2, 2 );
				/* Pn40D */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchWk.var.NotchQ2, 2 );

				/* 2�i�ڃm�b�`���튮�� */
				rc = 0x02;
			}
			break;

		case 2:
			/*------------------------------------------------------------------------------*/
			/*	��1�m�b�`�i�������g���j�ݒ�													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil1.UpperLimit )
			{ /* ����l�ł̃��~�b�g */
				freq = (USHORT)pndef_ntchfil1.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q�l�v�Z & �p�����[�^�v�Z													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset2( ANotchWk.var.NotchFil2, freq, &NotchQ ) != OK )
			{ /* Q�l�ݒ�ُ�̏ꍇ�́A�ݒ�l�����ɖ߂��m�b�`�������� */
				/* 1�i�ڃm�b�`Q�l�ُ� */
				rc = 0x04;
			}
			else
			{ /* Q�l�v�Z����I�� */
				/* �O���ڂ�1�ɂ��� */
				ANotchWk.var.NotchSw |= 0x0001;
				/* �m�b�`���g�� */
				ANotchWk.var.NotchFil1 = freq;
				/* �m�b�`Q�l */
				ANotchWk.var.NotchQ1 = NotchQ;

				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn409 */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil1, (LONG)ANotchWk.var.NotchFil1, 2 );
				/* Pn40A */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchWk.var.NotchQ1, 2 );

				/* 1�i�ڃm�b�`���튮�� */
				rc = 0x01;
			}
			break;

		case 3:
		case 5:
			/*------------------------------------------------------------------------------*/
			/*	��1�m�b�`�i�Ⴂ���g���j�ݒ�													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil1.UpperLimit )
			{ /* ����l�ł̃��~�b�g */
				freq = (USHORT)pndef_ntchfil1.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q�l�v�Z & �p�����[�^�v�Z													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset( ANotchPrm, KvGain, Jrate, freq, TUNE_ANTFRQ_NTQ1, &NotchQ ) != OK )
			{ /* Q�l�ݒ�ُ�̏ꍇ�́A�ݒ�l�����ɖ߂��m�b�`�������� */
				/* 1�i�ڃm�b�`Q�l�ُ� */
				rc = 0x04;
			}
			else
			{ /* Q�l�v�Z����I�� */
				ANotchWk.var.TrqFil = TuneNotchTfcalc( ANotchPrm, KvGain, Jrate, freq, NotchQ );
				/* 0���ڂ�1�ɂ��� */
				ANotchWk.var.NotchSw |= 0x0001;
				/* �m�b�`���g�� */
				ANotchWk.var.NotchFil1 = freq;
				/* �m�b�`Q�l */
				ANotchWk.var.NotchQ1 = NotchQ;

				if( NotchType == 5 )
				{ /* 2�i�ڃm�b�`��Q�l�X�V */
					TuneNotchQset2( ANotchWk.var.NotchFil1, ANotchWk.var.NotchFil2, &NotchQ );
					/* �m�b�`Q�l */
					ANotchWk.var.NotchQ2 = NotchQ;
				}

				ANotchWk.var.TrqFil = TunePrmLimitUnsigned( (ULONG)ANotchWk.var.TrqFil, &pndef_trqfil11 );

				/* Pn401 */
				TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (LONG)ANotchWk.var.TrqFil, 2 );
				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn409 */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil1, (LONG)ANotchWk.var.NotchFil1, 2 );
				/* Pn40A */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchWk.var.NotchQ1, 2 );
				/* Pn40D */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchWk.var.NotchQ2, 2 );

				/* 1�i�ڃm�b�`���튮�� */
				rc = 0x01;
			}
			break;

		default:
			/*------------------------------------------------------------------------------*/
			/* 1�i�ځA2�i�ڂ����ɗL���̎� or �����m�b�`OFF									*/
			/*	�� �������Ȃ�																*/
			/*------------------------------------------------------------------------------*/
			rc = 0x10;									/* �����s����						*/
			break;
	}
	return rc;
}



/****************************************************************************************************/
/*																									*/
/*			�����m�b�`�p�T�[�{����g���[�X�R���g���[���iRound)										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@�\�F	�����m�b�`�̐U����͂Ɏg�p����f�[�^���g���[�X�ɂ��T���v�����O����B					*/
/*				�@�g���[�X�������v��������ꍇ														*/
/*						ScanB�փg���[�X�𖳌��ɒʒm													*/
/*						ScanC�փg���[�X������������ʒm												*/
/*				�A�T�[�{����g���[�X���s�v���̃`�F�b�N												*/
/*					ScanC����̃g���[�X���s�v���̗����オ��G�b�W���󂯂Ĉȉ��̏������s���B			*/
/*						ScanB�̃g���[�X�ݒ���s��													*/
/*						ScanC�փg���[�X�J�n������ʒm												*/
/*				�B�g���[�X�����̃`�F�b�N															*/
/*					ScanB����̃g���[�X�������󂯂Ĉȉ��̏������s���B								*/
/*						ScanC�փg���[�X������ʒm													*/
/*																									*/
/*			���{�����œ����T���v�����O�f�[�^�͂��̌㒼���ɐU����͂Ŏg�p�����B					*/
/*			  �U����͂�Round�ōs���B																*/
/*																									*/
/****************************************************************************************************/
SHORT	TuneVibrationTrace( DETVIB *DetVib, FFTANALYZE *FftAna, 
								UCHAR TraceRequest, SHORT TrgLvl, BOOL TuneLessSts )
{
	SHORT	rc;
	SHORT	status;
	SHORT	*DataAdr;

	rc = TUNE_FREQ_SEARCH;

	/* �T�[�{����g���[�X�������v���`�F�b�N */
	if( TraceRequest == FALSE )
	{
		/* �T�[�{����g���[�X�������f */
		DetVibTraceStopReq( DetVib );
		rc = TUNE_FREQ_SEARCHINIT;
	}
	else
	{
		if( DetVibTraceGetStatus(DetVib) == DETVIBTRC_TRCEND )
		{
			/* �g���[�X�o�b�t�@�A�h���X�擾 */
			DataAdr = DetVibTraceGetDataAdr( DetVib );

			/* �U�����g����͗p�f�[�^�ݒ� */
//			FftSetAnaCondition( FftAna, 200, 2000, 80, TuneLessSts );	/* <S21A> */
			FftSetAnaCondition( FftAna, 200, 4000, 80, TuneLessSts );	/* <S21A> */

			/* �U�����g����� */
			status = FftAnalyzeExecute( FftAna, DataAdr );

			if( status != OK )
			{ /* ��̓G���[ */
				rc = TUNE_FREQ_SEARCHERR;
			}
			else
			{ /* ���g����͐���I�� */
				rc = TUNE_FREQ_SEARCHEND;
			}
		}
		else if( DetVibTraceGetIndex(DetVib) == 0 )
		{ /* �T�[�{����p�g���[�X���s�\ */
			/*----------------------------------------------------------------------------------*/
			/*	�T�[�{����g���[�X�J�n�������A�g���[�X�ݒ肵�N������							*/
			/*----------------------------------------------------------------------------------*/
			/* �g���[�X�ݒ�(�f�[�^��:200, ScanB����) */
			DetVibTraceSetCondition( DetVib, 200, 1, TrgLvl );
			/* �g���[�X�J�n�v�� */
			DetVibTraceStartReq( DetVib );
		}
	}
	return rc;
}



/****************************************************************************************************/
/*																									*/
/*		�񊵐��l�e�b�L������Kpm�ݒ�																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v:	Pn11B�ݒ�̓񎟊֐���Kpm���Z�o����B												*/
/*				�񎟊֐��FKpm[0.1s-1] = -0.0036 * Pn11B^2 + 3.8647 * Pn11B - 28.285					*/
/*					Pn11B =  1.0[Hz] �� Kpm =   1.0[s-1]											*/
/*					Pn11B = 20.0[Hz] �� Kpm =  60.0[s-1]											*/
/*					Pn11B = 50.0[Hz] �� Kpm = 100.0[s-1]											*/
/*				�ƂȂ�B�������APn11B > 50.0[Hz]��Kpm = 100.0[s-1]�ŃN�����v����B					*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneMfcKpmCalcAntFreq( USHORT AntFreq )
{
	LONG	wk, wk1, wk2, wk3;

	if( AntFreq > 500 )
	{ /* ���g����50.0Hz���傫���ꍇ */
		/* ���f�����[�v�Q�C����100.0[s-1]�Œ� */
		wk = 1000;
	}
	else
	{ /* ���g����50.0Hz�ȉ��̏ꍇ */
		/*----------------------------------------------------------------------*/
		/* ���Ӂj�{�񎟊֐��́A���g��<50.0�ł̂ݎg�p�B						*/
		/*		50Hz�𒴂���ꍇ�́AKpm�l�����X�ɏ������Ȃ�						*/
		/*----------------------------------------------------------------------*/
		/*	�񎟌W��  362 = 0.0036 * 10000										*/
		/*----------------------------------------------------------------------*/
		wk1 = (LONG)AntFreq * (LONG)AntFreq * 36;

		/*----------------------------------------------------------------------*/
		/*	�ꎟ�W��  38647 = 3.8647 * 10000									*/
		/*----------------------------------------------------------------------*/
		wk2 = (LONG)AntFreq * 38647;
		wk3 = -wk1 + wk2 - 282850;			/* 282850 = 28.285 *10000			*/
		wk = wk3 / 10000;
	}

	return (USHORT)wk;
}



/****************************************************************************************************/
/*																									*/
/*		�m�b�`�t�B���^�ݒ�p���͏��ݒ�API															*/
/*																									*/
/****************************************************************************************************/
/*		�T�v:	��1�A��2�m�b�`�t�B���^��ݒ肷��B�m�b�`Q�l�͎����I�Ɍv�Z�����B					*/
/****************************************************************************************************/
void	TuneSetNotchFilSettingData( AUTONOTCH *ANotch, USHORT freq, USHORT KvGain, USHORT Jrate )
{
	ANotch->ANotchData.conf.freq = freq;		/* �����m�b�`���g��									*/
	ANotch->ANotchData.conf.KvGain = KvGain;	/* Tf, �m�b�`Q�l�Z�o�p�Q�C��						*/
	ANotch->ANotchData.conf.Jrate = Jrate;		/* Tf, �m�b�`Q�l�Z�o�p���׊������[�����g			*/

	return;
}



/***************************************** end of file **********************************************/
