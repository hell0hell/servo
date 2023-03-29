/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckMotSts.c : ���[�^��ԊĎ�������`														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^�����ԊĎ�����																	*/
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
#include "CheckMotSts.h"
#include "MLib.h"


/****************************************************************************************************/
/*																									*/
/*		���[�^��Ԍ��o����																			*/
/*																									*/
/****************************************************************************************************/
void SMotSts_InputMotorStatus( SEQ_MOT_STS *MotSts, BASE_CTRL_OUT *BaseCtrlOut,
                               SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm )
{
//	MENCV				*MencV;
	LONG				lwk0;

//	MencV = AxisRscC->MencV;

/*--------------------------------------------------------------------------------------------------*/
/*		Input Motor Pulse Data																		*/
/*--------------------------------------------------------------------------------------------------*/
	lwk0 = BaseCtrlOut->MotPos - MotSts->var.MotPos;			/* Cal. Delta Position				*/
	MotSts->var.MotPos  = BaseCtrlOut->MotPos;					/* Update Motor Position			*/
	MotSts->var.dMotPos = lwk0;									/* Update Delta Motor Position		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Input Pulse Command Data																	*/
/*--------------------------------------------------------------------------------------------------*/
	lwk0 = BaseCtrlOut->PcmdCntr - MotSts->var.PcmdCntr;		/* Cal. Delta Pulse					*/
	MotSts->var.PcmdCntr = BaseCtrlOut->PcmdCntr;				/* Update Pulse Command Counter		*/
	MotSts->var.dPcmdCnt = lwk0;								/* Update Delta Pulse Command		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Motor Speed [2^24/OvrSpd]															*/
/*--------------------------------------------------------------------------------------------------*/
	MotSts->var.LstMotSpd = MotSts->var.MotSpd;
	if( BaseCtrlOut->MotSpdCnt > 0 )
	{
		MotSts->var.MotSpd = BaseCtrlOut->MotSpdSum / BaseCtrlOut->MotSpdCnt;
	}
#if 0
	if( MencV->EncDataNG )									/* Encoder Data NG					*/
	{
		MotSts->var.MotSpd = 0;						/* Signed Motor Speed				*/
	}
#endif
	/* Absolute Motor Speed */
	MotSts->var.AbsMotSpd = MlibLABS( MotSts->var.MotSpd );

	/* Make Acceleration for Mode Switch */
	SeqCtrlOut->ModeSwAcc = MotSts->var.MotSpd - MotSts->var.LstMotSpd;

/*--------------------------------------------------------------------------------------------------*/
/*		Detect Motor Status																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���[�^��]���o����(TGON) */
	SeqCtrlOut->TgOnSts = LpxCheckMotorRun( MotSts, SeqCtrlOut->TgOnSts );

	/* ���[�^��~���o����(MOTSTOP) */
	SeqCtrlOut->MotStop = LpxCheckMotorStop( MotSts->var.AbsMotSpd );

	/* ���x��v���o����(VCMP) */
	SeqCtrlOut->VcmpSts = LpxCheckMotorVcmp( MotSts,
											 BaseCtrlOut->SpdCtrlRef,
	/*										 (CTRLMODE_SPD == BaseCtrlOut->CtrlModeOut.b.cm) );		*/
										 (( CTRLMODE_SPD == BaseCtrlOut->CtrlModeOut.b.cm ) ||
										  ( CTRLMODE_JOG == BaseCtrlOut->CtrlModeOut.b.cm ) ));	/* <S144> */
      /*Motor Suspend Level Flag <S201>*/
	SeqCtrlOut->MotSuspLvl = LpxCheckMotorSuspendLevel(MotSts,Bprm->NorSuspSpd);
/*--------------------------------------------------------------------------------------------------*/
/*		Un000,Un007 : ���[�^���x,�w�߃p���X���x														*/
/*--------------------------------------------------------------------------------------------------*/
	MotSts->var.Pfbk64Sum += MotSts->var.dMotPos;
	MotSts->var.Pcmd64Sum += MotSts->var.dPcmdCnt;
	MotSts->var.MotSpd64Sum += MotSts->var.MotSpd;

	MotSts->var.Pfbk64SumCnt++;
	if( MotSts->var.Pfbk64SumCnt >= SVCDEF_UNSPD_SCMEANNUM )		/* 64ms���ω��Z����					*/
	{
#if (FLOAT_USE==TRUE)
		/* ���[�^���x		[pulse/64ms] -> [rpm, mm/s]	*/
		lwk0 = ((MotSts->var.MotSpd64Sum >> 4) + 1) >> 1;
		MotSts->var.UnMotSpd = lwk0 * Bprm->Kmonspd;
		MotSts->var.UnPfbk64ms = MotSts->var.Pfbk64Sum;

		/* �w�߃p���X���x	[pulse/64ms] -> [rpm, mm/s]	*/
		MotSts->var.UnPcmd64ms = MotSts->var.Pcmd64Sum * Bprm->Kmonpcmdspd ;
		MotSts->var.UnPcmd64Sum = MotSts->var.Pcmd64Sum;		/* <S1B6> */
#else		
		/* ���[�^���x		[pulse/64ms] -> [rpm, mm/s]	*/
		lwk0 = ((MotSts->var.MotSpd64Sum >> 4) + 1) >> 1;
		MotSts->var.UnMotSpd = MlibMulgainSx48( lwk0, Bprm->Kmonspd );
		MotSts->var.UnPfbk64ms = MotSts->var.Pfbk64Sum;

		/* �w�߃p���X���x	[pulse/64ms] -> [rpm, mm/s]	*/
		MotSts->var.UnPcmd64ms = MlibMulgain( MotSts->var.Pcmd64Sum, Bprm->Kmonpcmdspd );
		MotSts->var.UnPcmd64Sum = MotSts->var.Pcmd64Sum;		/* <S1B6> */
#endif /* FLOAT_USE */

		MotSts->var.Pfbk64Sum = 0;
		MotSts->var.Pcmd64Sum = 0;
		MotSts->var.Pfbk64SumCnt = 0;
		MotSts->var.MotSpd64Sum = 0;
	}

	MotSts->var.TrqRefMon = BaseCtrlOut->TrqRefMon;
/*--------------------------------------------------------------------------------------------------*/
/*		Un009 : �����g���N�p�f�[�^ 	( 10sec�Q��d���w�ߕ��ϒl )			UnIsqrSum					*/
/*		Un142 : �����g���N�p�f�[�^	( 2ms�Q��d���w�ߒl)				UnCrntRef					*/
/*		Un145 : �����g���N�p�f�[�^	( 10sec�Q��d���w�ߕ��ύő�l)		UnIsqrSumMax				*/
/*--------------------------------------------------------------------------------------------------*/
	lwk0 = (BaseCtrlOut->IdRefMon * BaseCtrlOut->IdRefMon)
			+ (BaseCtrlOut->IqRefMon * BaseCtrlOut->IqRefMon);
	MotSts->var.UnCrntRef = MlibSqrtu32( lwk0 );
	MotSts->var.IsqrSum += (lwk0 >>14 );

	MotSts->var.IsqrSumCnt++;
	if( MotSts->var.IsqrSumCnt >= KPI_SCANC_MS( 10000 ) )
	{
		lwk0 = MlibSqrtu32( MotSts->var.IsqrSum );
#if (FLOAT_USE==TRUE)
		MotSts->var.UnIsqrSum = lwk0 * Bprm->Kcurload;
#else
		MotSts->var.UnIsqrSum = MlibMulgain( lwk0, Bprm->Kcurload );
#endif /* FLOAT_USE */

		if( MotSts->var.UnIsqrSum > (LONG)MotSts->var.UnIsqrSumMax )	/* �Q��d���ώZ�̍ő�l				*/
		{
			MotSts->var.UnIsqrSumMax = MotSts->var.UnIsqrSum;
		}
		MotSts->var.IsqrSum = 0;
		MotSts->var.IsqrSumCnt = 0;
	}
}


/****************************************************************************************************/
/*																									*/
/*		���[�^��]���o����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^��]���x�̐�Βl�Ɖ�]���o���x��(Pn502)�Ƃ��r���A���[�^��]���x�̕���			*/
/*			�傫���ꍇ�ɁA��]���o�o��(/TGON)���R�l�N�^�b�m�P���o�͂���@�\�B						*/
/*			�ȑO�́A��]���o���x��(Pn502)���[�������x���ƌ����Ă����B								*/
/*																									*/
/*			�{�֐��ł͏o�͗p�ϐ��̃��[�^��]���o�M���t���O(CoutV.f.Tgon)�����Ă�B					*/
/*			��]���o���x��(Pn502)�̏����l��20rpm�Ȃ̂ł��̂܂܎g���΁A								*/
/*			/TGON=LOW�ŉ�]���A/TGON=HIGH�łقڒ�~���ƂȂ�B										*/
/*																									*/
/*			Tgon = OFF��ON �̏����́A																*/
/*				���[�^���x����]���o���x��(TgonSpdLevel)�ȏ�ɂȂ�����								*/
/*				�܂��́ATgonSpdLevel �� TGMGN �ȏ�Ń��[�^���x�� TgonSpdLevel �ȉ��̎�				*/
/*																									*/
/*			Tgon = ON��OFF �̏����́A																*/
/*				��]���o���x��(TgonSpdLevel)�� TGMGN ���傫���A����								*/
/*				���[�^���x�����o���x��(TgonSpdLevel-TGMGN)�ȉ��ɂȂ�����							*/
/*																									*/
/*			TgonSpdLevel : ��]���o���x��(Pn502)�𐳋K�����x�ɕϊ���������							*/
/*			TGMGN        : �`���^�����O�h�~�̂��߂̃q�X�e���V�X�����܂߂��}�[�W��					*/
/*							( 12710 / 16777216(24bit) * 6600(OS) �� 5rpm )							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL LpxCheckMotorRun( SEQ_MOT_STS *MotSts, LONG LstTgonSts )
{
	BOOL	TgOn;
	TgOn = LstTgonSts;

	if( MotSts->var.AbsMotSpd >= MotSts->conf.TgonSpdLevel )
	{ /* ���[�^���x �� ��]���o���x�� */
		TgOn = TRUE;
	}
	else
	{ /* ���[�^���x �� ��]���o���x�� */
		if( MotSts->conf.TgonSpdLevel > MOTSPD_TGMGN )
		{ /* ��]���o���x�� �� 5[min-1] */
			if( MotSts->var.AbsMotSpd < (MotSts->conf.TgonSpdLevel - MOTSPD_TGMGN) )
			{ /* ���[�^���x �� (��]���o���x�� - 5[min-1]) */
				TgOn = FALSE;
			}
		}
		else
		{ /* ��]���o���x�� �� 5[min-1] */
			TgOn = TRUE;
		}
	}
	return TgOn;
}



/****************************************************************************************************/
/*																									*/
/*		���[�^��~���o����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^����~�������Ƃ����o����B														*/
/*																									*/
/*			���[�^��~���o�M���́A�c�a�V�[�P���X�A�n�s�V�[�P���X�A�[���N�����v�V�[�P���X�A			*/
/*			����~�����A�\�t�g���Z�b�g�����A���[�^�d�����o�M���̃I�t�Z�b�g��������(Fn00E)			*/
/*			�ŗp������B																			*/
/*			�c�a��H�̓����[�{�_�C�I�[�h���g���C�A�b�N�ɂ�鐮���ɂ��s����B					*/
/*			�c�a�d�����؂��̂̓��[�^�U�N�d�����_�C�I�[�h�܂��̓g���C�A�b�N�̃I���d����			*/
/*			��������Ƃ��B																			*/
/*			���[�^��~���o�M���́A�c�a�d�����؂ꂽ���Ƃ����o���A�Q�[�g�M���̃h���C�u������			*/
/*			��������Ƃ��Ďg�p����B�i���̏����ɂ��g�����c�a�̏����ɍ��킹�Ă����΂悢�j			*/
/*																									*/
/*			�_�C�I�[�h�܂��̓g���C�A�b�N���؂��d���͖�O.�V�u���炢�ƌ��Ȃ��A�n�r��]��			*/
/*			�̗U�N�d����d���O�a���x���P�V�T�u�ƌ��Ȃ��ƁA											*/
/*																									*/
/*			MOTSPD_DBLVL = 16777216(OvrSpd) x 0.7/175 = 67108 --> �]�T������60000�Ƃ���B			*/
/*																									*/
/*			���[�^��]�����c�a���x����������ď[�����Ԃ��o������A�c�a��H�͐؂ꂽ�ƍl����			*/
/*			�ǂ��B																					*/
/*			�r�f�c�ł͂c�a�̍ہA�����[�Ń��[�^���𒼐ڒZ�����Ă������߁A�c�a�d���̓��[�^��			*/
/*			���S�Ɏ~�܂�܂ŗ��ꑱ����B���̂��߁A���[�^��~�����o������40ms�҂Ă΃��[�^��			*/
/*			���S�Ɏ~�܂���̂ƍl���Ă���B															*/
/*			�r�f�c�l�ł͂c�a�̓_�C�I�[�h�A�g���C�A�b�N�ɂ��̂Ń��[�^�����S�Ɏ~�܂�Ȃ��Ă�		*/
/*			�c�a�d���͎~�܂�B������40ms�҂K�v�͂Ȃ��B											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL LpxCheckMotorStop( LONG AbsMotSpd )
{
	BOOL	MotStop;

	if( AbsMotSpd < MOTSPD_DBLVL )
	{
		MotStop = TRUE;								/* ���[�^��~���o�M���̐ݒ�(ON)					*/
	}
	else
	{
		MotStop = FALSE;							/* ���[�^��~���o�M���̐ݒ�(OFF)				*/
	}

	return	MotStop;
}



/****************************************************************************************************/
/*																									*/
/*		���x��v���o����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^��]���x�Ƒ��x�w��(�\�t�g�X�^�[�g�O)�̍������x��v���o��(Pn503)���				*/
/*			�������ꍇ�A���x��v���o�M���t���O�����Ă�B											*/
/*			���x��v���o�M���o�͊��t(/V-CMP)���ݒ肳��Ă���ꍇ�A�O���M�����o�͂����B			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL	LpxCheckMotorVcmp( SEQ_MOT_STS *MotSts, LONG SpdRef, BOOL SpdCtrlMode )
{
	BOOL	Vcmp;

	if( TRUE == SpdCtrlMode )
	{
		if( MlibLABS(SpdRef - MotSts->var.MotSpd) <= MotSts->conf.VcmpSpdLevel )
		{
			Vcmp = TRUE;
		}
		else
		{
			Vcmp = FALSE;
		}
	}
	else
	{
		Vcmp = FALSE;
	}
	return Vcmp;
}



/****************************************************************************************************/
/*																									*/
/*		���f���x���x�����o����	<S201>																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ :    																			            */
/*			���[�^���x���ō����x��10�����傫���ꍇ�A���f���x���o�M���t���O��TRUE�ɂ���B			*/
/*																									*/
/*			�ō����x��10��(Bprm.NorSuspSpd) = Bprm.NorMaxSpd / 10									*/
/*																							        */
/****************************************************************************************************/
BOOL  LpxCheckMotorSuspendLevel( SEQ_MOT_STS *MotSts,LONG NorSuspSpd)
{
	BOOL MotSuspLvl;  /*define a BOOL */
	if( MotSts->var.AbsMotSpd > NorSuspSpd)
	{
		MotSuspLvl = TRUE;/* ���f���x���o�M���̐ݒ�(TRUE)*/
	}
	else
	{
		MotSuspLvl = FALSE; /* ���f���x���o�M���̐ݒ�(FALSE)*/
	}
	return MotSuspLvl;

}


/***************************************** end of file **********************************************/
