/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RippleComp.c : ���b�v���␳�@���W���[��														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2013.09.25	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include	"RippleComp.h"
#include 	"BaseCmnStruct.h"

/****************************************************************************************************/
/*																									*/
/*		�萔��`																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���b�v���␳������`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	RIPCOMPORDER_MECHA		RPLCMP_ORDER_MECHA		/* ���b�v���␳�ő原��					*/
#define	RIPCOMPORDER_ELE		RPLCMP_ORDER_ELE		/* ���b�v���␳�ő原��					*/

/*--------------------------------------------------------------------------------------------------*/
/*		���b�v���X�y�N�g����̓��[�h																*/
/*--------------------------------------------------------------------------------------------------*/
#define	RPLSP_ANALYZE_STOP	0					/* ���b�v���X�y�N�g�������~					*/
#define	RPLSP_ANALYZE_SPD	2					/* ���x���b�v���X�y�N�g������					*/
#define	RPLSP_ANALYZE_TRQ	3					/* �g���N���b�v���X�y�N�g������					*/

/*--------------------------------------------------------------------------------------------------*/
/*		 ���x���b�v���⏞��񏑂����݃��[�h															*/
/*--------------------------------------------------------------------------------------------------*/
#define	MOTOR_SN_DATA_SIZE	8					/* ���[�^�V���A��No.�f�[�^�T�C�Y[word]			*/
#define	SN_READ_SUCCESS		0x0000				/* ���[�^�V���A��No.�Ǐo�����튮��				*/
#define	SN_READ_ERROR		0x0001				/* ���[�^�V���A��No.�Ǐo���ُ튮��				*/
#define	SN_DIFFERENT		0x0002				/* ���[�^�V���A��No.���قȂ�					*/


/****************************************************************************************************/
/*		Local Function Prototype Definition															*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞																			*/
/*																									*/
/*	�T�v:	�p�����[�^�ɕۑ����ꂽ���b�v�����(�U���A�ʑ�)����ɐ����g�⏞�g���N�M�����쐬���A		*/
/*			�⏞�g���N�M�����g���N�w�߂ɂe�e���Ƃ��ĉ��Z���邱�Ƃɂ���đ��x���b�v����ጸ����		*/
/*																									*/
/*			�⏞�g���N	= ��(�␳�U�� * sin(���[�^�ʑ� * ���� + �ʑ��␳�l))						*/
/*																									*/
/* arg		: USHORT	MotPhase		:	���[�^�ʑ�			[65536/360deg]						*/
/* out		: LONG		TrqComp			:	�g���N�␳�l		[2^24/MaxTrq]						*/
/* g-out	: 																						*/
/*																									*/
/****************************************************************************************************/
LONG	RippleCompensation( RPLCMP *RippleCmp, USHORT MechaPhase, USHORT MotPhase, BOOL BaseEnable )
{
	RPLCMP_PRM			*RippleCmpP;
//	RPLCMP_PRM_MECHA 	*RippleCmpP_mecha;	/*<S1FD>*/
	RPLCMP_PRM_ELE 		*RippleCmpP_ele;

	LONG				i, lwk, TrqComp;
//	LONG				TrqCompMecha		/*<S1FD>*/
	LONG				TrqCompEle;
	USHORT				uswk[RPLCMP_ORDER_MECHA];
	BOOL				RippleCompMon;

	RippleCmpP       = &(RippleCmp->RplCmpPrm);
//	RippleCmpP_mecha = &(RippleCmp->RplCmpPrm.RplCmpPrmMecha);/*<S1FD>*/
	RippleCmpP_ele   = &(RippleCmp->RplCmpPrm.RplCmpPrmEle);

/*--------------------------------------------------------------------------------------------------*/
/*		�@�B�p�␳																					*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* ���x���b�v���⏞ �@�B�p�␳����(�����p�Ƃ��ăR�����g�A�E�g) */
	lwk = 0;
	for(i=0; i<RIPCOMPORDER_MECHA; i++)
	{
		/*------------------------------------------------------------------------------------------*/
		/*		�ʑ��v�Z																			*/
		/*------------------------------------------------------------------------------------------*/
		uswk[i]  = RippleCmpP_mecha->PhaseOfset[i]  + (MechaPhase * RippleCmpP_mecha->CompFrq[i]);
		uswk[i]  = uswk[i] >> 6;									/* [65536/360] --> [1024/360]	*/

		/*------------------------------------------------------------------------------------------*/
		/*		�␳�g���N�v�Z																		*/
		/*																							*/
		/*	[15000/MaxTrq]�ւ̐��K���ɂ� >>14 ���K�v�����A�������ԑ΍�̂��߁A						*/
		/*	[2^24/ MaxTrq]�ւ̐��K�����ɂ܂Ƃ߂ăV�t�g���Z����										*/
		/*------------------------------------------------------------------------------------------*/
		lwk += (RippleCmpP_mecha->CompAmpValue[i] * MlibFASTSINS16(uswk[i]));
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�g���N���K���v�Z[15000/MaxTrq] -> [2^24/MaxTrq]										*/
	/*																								*/
	/*		MlibMulhigh32��p����15000->2^24���K���ϊ����s���ɂ� <<16 ���K�v������̏�����			*/
	/*		�{���������ׂ� >>14 �����{���Ă��Ȃ��̂Ńg�[�^���� <<2 ���s��(�������ԑ΍�)				*/
	/*----------------------------------------------------------------------------------------------*/
	lwk = MlibMulhigh32( (lwk << 2), 73300775 );					/*	[2^24/MaxTrq]				*/
	TrqCompMecha = MlibMulgainNolim( lwk, RippleCmpP_mecha->CompGain );
#endif	/* ���x���b�v���⏞ �@�B�p�␳����(�����p�Ƃ��ăR�����g�A�E�g) */

	lwk = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		�d�C�p�␳																					*/
/*--------------------------------------------------------------------------------------------------*/
	for(i=0; i<RIPCOMPORDER_ELE; i++)
	{
		/*------------------------------------------------------------------------------------------*/
		/*		�ʑ��v�Z																			*/
		/*------------------------------------------------------------------------------------------*/
		uswk[i]  = RippleCmpP_ele->PhaseOfset[i]  + (MotPhase * RippleCmpP_ele->CompFrq[i]);
		uswk[i]  = uswk[i] >> 6;									/* [65536/360] --> [1024/360]	*/

		/*------------------------------------------------------------------------------------------*/
		/*		�␳�g���N�v�Z																		*/
		/*																							*/
		/*	[15000/MaxTrq]�ւ̐��K���ɂ� >>14 ���K�v�����A�������ԑ΍�̂��߁A						*/
		/*	[2^24/ MaxTrq]�ւ̐��K�����ɂ܂Ƃ߂ăV�t�g���Z����										*/
		/*------------------------------------------------------------------------------------------*/
		lwk += (RippleCmpP_ele->CompAmpValue[i] * MlibFASTSINS16(uswk[i]));

	}

#if 0
	lwk = RippleCmp->var.TrqComp[0] + RippleCmp->var.TrqComp[1] + RippleCmp->var.TrqComp[5];
	if(RippleCmpP->RplCmpMode6f == TRUE)
	{ /* 1�`6���g�p���[�h�I���� */
		lwk = lwk + RippleCmp->var.TrqComp[2] + RippleCmp->var.TrqComp[3] + RippleCmp->var.TrqComp[4];
	}
#endif
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�g���N���K���v�Z[15000/MaxTrq] -> [2^24/MaxTrq]										*/
	/*																								*/
	/*		MlibMulhigh32��p����15000->2^24���K���ϊ����s���ɂ� <<16 ���K�v������̏�����			*/
	/*		�{���������ׂ� >>14 �����{���Ă��Ȃ��̂Ńg�[�^���� <<2 ���s��(�������ԑ΍�)				*/
	/*----------------------------------------------------------------------------------------------*/
	lwk = MlibMulhigh32( (lwk << 2), 73300775 );					/*	[2^24/MaxTrq]				*/
	TrqCompEle = MlibMulgainNolim( lwk, RippleCmpP_ele->CompGain );

/*--------------------------------------------------------------------------------------------------*/
/*		�␳�g���N�v�Z	�i�@�B�p�␳�g���N �{ �d�C�p�␳�g���N�j									*/
/*--------------------------------------------------------------------------------------------------*/
//	RippleCmp->var.TrqCompAll = TrqCompMecha + TrqCompEle;	// ���x���b�v���⏞ �@�B�p�␳����(�����p�Ƃ��ăR�����g�A�E�g)
	RippleCmp->var.TrqCompAll = TrqCompEle;

/*--------------------------------------------------------------------------------------------------*/
/*		�␳���s����																				*/
/*--------------------------------------------------------------------------------------------------*/
//	if( RippleCmpP->RplCmpUse == TRUE)/*<S1FD>*/
	if( (RippleCmpP->RplCmpUse == TRUE) && (RippleCmp->var.RippleCmpExe == TRUE) )
	{	/* �␳�L��	*/
		TrqComp = RippleCmp->var.TrqCompAll;
		RippleCompMon = TRUE;
	}
	else
	{	/* �␳����	*/
		TrqComp = 0;
		RippleCompMon = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�u���b�N���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseEnable == FALSE )
	{
		RippleCmp->var.TrqCompAll = 0;
		TrqComp = 0;
		RippleCompMon = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		���x���b�v���⏞���j�^�i�[																	*/
/*--------------------------------------------------------------------------------------------------*/
	RippleCmp->RippleCompMon    = RippleCompMon;
	RippleCmp->RippleTrqCompMon = TrqComp;

	return(TrqComp);

}

/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞�@�\�̗L�������`�F�b�N														*/
/*																									*/
/****************************************************************************************************/
void	RippleCompChkThresholdVal( RPLCMP *RippleCmp, USHORT CtrlMode, LONG SpdRef, LONG SpdFb, BOOL TuneLessCtrl )
{
	USHORT	RippleCmpThresholdSel;
	BOOL	RippleCmpExe;

	RippleCmpExe = FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*	���x���b�v���⏞�@�\�̗L�����������l����(Pn423.2=0(�w�ߑ��x)/1(�t�B�[�h�o�b�N���x))			*/
	/*		�E�ʒu���䃂�[�h	�FPn423.2�ɏ]��														*/
	/*		�E���x���䃂�[�h	�FPn423.2�ɏ]��														*/
	/*		�E�g���N���䃂�[�h	�F�����I��1(�t�B�[�h�o�b�N���x)�ɂ���								*/
	/*----------------------------------------------------------------------------------------------*/
	if( CtrlMode== CTRLMODE_TRQ )	/* �g���N���䃂�[�h */
	{
		RippleCmpThresholdSel = FB_SPD;
	}
	else
	{
		RippleCmpThresholdSel = RippleCmp->RippleCmpThresholdSel;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	��������																					*/
	/*----------------------------------------------------------------------------------------------*/
	if( SpdRef < 0 )
	{
		SpdRef = -SpdRef;
	}
	if( SpdFb < 0 )
	{
		SpdFb = -SpdFb;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	���菈��																					*/
	/*----------------------------------------------------------------------------------------------*/
	if( RippleCmp->VelRipplePhaseReady == TRUE ) 	/* ���Ɍ��o����(�ʑ����o����) */
	{
		if( RippleCmpThresholdSel == CMD_SPD )
		{	/* ���x���b�v���⏞�@�\�̗L�����������l���� = �u�w�ߑ��x�v */
			if( RippleCmp->VelRippleCompSpd <= SpdRef )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
		else
		{/* ���x���b�v���⏞�@�\�̗L�����������l���� = �u�t�B�[�h�o�b�N���x�v */
			if( RippleCmp->VelRippleCompSpd <= SpdFb )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
	}
	else	/* ���Ɍ��o������(�ʑ����o������) */
	{
		RippleCmpExe = FALSE;
	}

	/* �������X�L�����͑��x���b�v���⏞���Ȃ� */
	if(TuneLessCtrl == TRUE)
	{
		RippleCmpExe = FALSE;
	}

	/* �L�������`�F�b�N���ʂ��i�[ */
	RippleCmp->var.RippleCmpExe = RippleCmpExe;
}

#if 0	/* ���j�^�o�͑I�� *//* @@���󖢑Ή� */
/****************************************************************************************************/
/*																									*/
/*		���b�v���␳���j�^�o�͑I���擾API															*/
/*																									*/
/****************************************************************************************************/
LONG	RippleCompGetMonOutSel( RPLCMP_PRM *RippleCmpP )
{
	return	(RippleCmpP->RplCmpDobsMon);
}
#endif	/* ���j�^�o�͑I�� *//* @@���󖢑Ή� */

/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞�p�����[�^�v�Z(�d�C�p)														*/
/*																									*/
/****************************************************************************************************/
BOOL	CalVelRippleCompE( RPLCMP *RippleCmp, BPRMDAT *Bprm, USHORT RippleCompGainE, 
							USHORT *RippleCompCycleE, SHORT *RippleCompAmplitudeE, USHORT *RippleCompPhaseE)
{
//	RPLCMP_PRM	wrkp;	/*<S1FD>*/
	USHORT		RippleCompCycle_ele[6];
	SHORT		RippleCompAmplitude_ele[6];
	USHORT		RippleCompPhase_ele[6];

	LONG		i;
//	BOOL		err;

//	err = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*		���x���b�v���⏞�p�����[�^�R�s�[														*/
	/*----------------------------------------------------------------------------------------------*/
	for( i = 0; i < RIPCOMPORDER_ELE; i++ )
	{	/* �d�C�p�␳ */
		RippleCompCycle_ele[i]     = RippleCompCycleE[i];
		RippleCompAmplitude_ele[i] = RippleCompAmplitudeE[i];
		RippleCompPhase_ele[i]     = RippleCompPhaseE[i];
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�d�C�p�␳�p�����[�^ 																		*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�Q�C��	[%] 																		*/
	/*----------------------------------------------------------------------------------------------*/
	RippleCmp->RplCmpPrm.RplCmpPrmEle.CompGain = MlibScalKxgain( RippleCompGainE, 1, 100, NULL, 24 );

	for(i=0; i<RIPCOMPORDER_ELE; i++)
	{
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompFrq[i] = RippleCompCycle_ele[i];
		/*------------------------------------------------------------------------------------------*/
		/*		�␳�U���v�Z	[0.1%] --> [15000/MaxTrq]											*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompAmpValue[i]  = BprmTorqueLevelCal( Bprm,
														(LONG)(RippleCompAmplitude_ele[i]), 1 );

		/*------------------------------------------------------------------------------------------*/
		/*		�ʑ��f�[�^�v�Z	[deg] --> [65536/360]												*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.PhaseOfset[i]  = (USHORT)(SHORT)MlibScalKxgain
									((LONG)((SHORT)RippleCompPhase_ele[i]), 65536, 360, NULL,  -24 );
	}

//	return	(err);
	return	TRUE;
}

/****************************************************************************************************/
/*																									*/
/*		SigmaWin+���烌�W�X�^�փ��b�v���⏞�����������ݎ��̃��b�v���␳�p�����[�^�v�Z(�d�C�p�␳)	*/
/*																									*/
/****************************************************************************************************/
void	FnRegCalVelRippleCompE( BPRMDAT *Bprm, RPLCMP *RippleCmp )/*<S1FD>*/
{
//	RPLCMP_PRM	wrkp;		/*<S1FD>*/
	USHORT		RippleCompCycle_ele[6];
	SHORT		RippleCompAmplitude_ele[6];
	USHORT		RippleCompPhase_ele[6];
//	BPRMDAT		*Bprm;		/*<S1FD>*/
//	RPLCMP		*RippleCmp;	/*<S1FD>*/
	LONG		i;

//	Bprm = Axis->UniPrms->Bprm;/*<S1FD>*/
//	RippleCmp = &(Axis->BaseLoops->RippleCmp);


	/*----------------------------------------------------------------------------------------------*/
	/*		���b�v���␳�p�����[�^�R�s�[															*/
	/*----------------------------------------------------------------------------------------------*/
	for( i = 0; i < RIPCOMPORDER_ELE; i++ )
	{	/* �d�C�p�␳ */
		RippleCompCycle_ele[i]     = RippleCmp->RplCmpPrm.RplCompCycleEle[i];
		RippleCompAmplitude_ele[i] = RippleCmp->RplCmpPrm.RplCompAmplitudeEle[i];
		RippleCompPhase_ele[i]     = RippleCmp->RplCmpPrm.RplCompPhaseEle[i];
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�d�C�p�␳�p�����[�^ 																		*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�Q�C��	[%] 																		*/
	/*----------------------------------------------------------------------------------------------*/
	RippleCmp->RplCmpPrm.RplCmpPrmEle.CompGain = MlibScalKxgain( RippleCmp->RplCmpPrm.RplCompGainEle,
																					1, 100, NULL, 24 );

	for(i=0; i<RIPCOMPORDER_ELE; i++)
	{
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompFrq[i] = RippleCompCycle_ele[i];
		/*------------------------------------------------------------------------------------------*/
		/*		�␳�U���v�Z	[0.1%] --> [15000/MaxTrq]											*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompAmpValue[i]  = BprmTorqueLevelCal( Bprm,
														(LONG)(RippleCompAmplitude_ele[i]), 1 );

		/*------------------------------------------------------------------------------------------*/
		/*		�ʑ��f�[�^�v�Z	[deg] --> [65536/360]												*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.PhaseOfset[i]  = (USHORT)(SHORT)MlibScalKxgain
									((LONG)((SHORT)RippleCompPhase_ele[i]), 65536, 360, NULL,  -24 );
	}

//	return;	/*<S1FD>*/
}

/****************************************************************************************************/
/*																									*/
/*		����ڑ����[�^�ƑO��ڑ����[�^�̃V���A��No.���r 											*/
/*																									*/
/****************************************************************************************************/
ULONG ChkMotorSerialNo( UINT AxisNo, MENCV *MencV )/*<S1FD>*/
{
//	EEP_DEVICE_HANDLE	*EepDevice; /* <S1D2> */
	ULONG				rc;
	ULONG				ReadRslt;
//	UINT				AxisNo;/*<S1FD>*/
	USHORT				SerialNo[MOTOR_SN_DATA_SIZE];
	USHORT				Lst_SerialNo[MOTOR_SN_DATA_SIZE];
	LONG				ArrayIdx;

	/* �ϐ������� */
	rc = SN_READ_SUCCESS;
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev; /* <S1D2> */
//	AxisNo = Axis->AxisNo;/*<S1FD>*/

	/* ����ڑ����[�^�ƑO��ڑ����[�^�̃V���A��No.���r */
	for(ArrayIdx = 0;  ArrayIdx < MOTOR_SN_DATA_SIZE; ArrayIdx++)
	{
		/* ����ڑ����[�^�̃V���A��No.�擾 */
		SerialNo[ArrayIdx] = (USHORT)(MencV->MotInfo.SNo[ArrayIdx*2 + 1] & 0x00FF);
		SerialNo[ArrayIdx] |= (USHORT)((MencV->MotInfo.SNo[ArrayIdx*2] & 0x00FF) << 8);

		/* �O��ڑ����[�^�̃V���A��No.�擾 */
		/* read from EEPROM */
		ReadRslt = EepdevReadValues(															/* <S1D2> */
								0,																/* EEPROM Device ID(0: SPI(IF), 1: I2C(SDA) */
								EEP_NOSUM_ADDRESS(AxisNo, EEP_MOTORSN_OFS) + (ArrayIdx*2),		/* Eeprom Address in bytes */
								&Lst_SerialNo[ArrayIdx],										/* Pointer to the buffer for reading */
								1 );															/* reading size in words */
		if(ReadRslt == SN_READ_SUCCESS)	/* �O��ڑ����[�^�̃V���A��No.�Ǐo�����튮�� */
		{
			if(Lst_SerialNo[ArrayIdx] != SerialNo[ArrayIdx] )	/* �O��ڑ����[�^�ƍ���ڑ����[�^�̃V���A��No.���قȂ� */
			{
				rc = SN_DIFFERENT;
				break;
			}
		}
		else	/* �O��ڑ����[�^�̃V���A��No.�Ǐo���ُ튮�� */
		{
				rc = SN_READ_ERROR;
				break;
		}
	}

	return(rc);
}

/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞���s��v���[�j���O��Ԏ擾API												*/
/*																									*/
/****************************************************************************************************/
BOOL	RippleCompGetRippleWrnState( RPLCMP *RippleCmp )/*<S1FD>*/
{
	return	(RippleCmp->var.RippleWrnState);
}

/***************************************** end of file **********************************************/
