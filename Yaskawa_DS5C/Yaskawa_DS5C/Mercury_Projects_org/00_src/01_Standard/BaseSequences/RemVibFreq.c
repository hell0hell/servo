/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RemVibFreq.c : �c���U�����g���v�Z������`													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �c���U�����g���v�Z����																	*/
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
/*	Note	:	����	2011.02.01	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MLib.h"
#include "KLib.h"
#include "RemVibFreq.h"



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	RemVibChkCondition( REMVIBFREQ *RemVibFreq, BOOL PosCtrlMode );							*/
/*	void	RemVibCalculateFreq( REMVIBFREQ *RemVibFreq, LONG CtrlPosErr,							*/
/*																BOOL CoinSts, BOOL RefzSts );		*/
/****************************************************************************************************/

/****************************************************************************************************/
/*		API																							*/
/****************************************************************************************************/
/*	LONG	RemVibGetRemVibFreq( REMVIBFREQ *RemVibFreq );											*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		�c���U�����g���Z�o����������																*/
/*																									*/
/****************************************************************************************************/
void	RemVibChkCondition( REMVIBFREQ *RemVibFreq, BOOL PosCtrlMode )
{
	if( PosCtrlMode != TRUE )
	{ /* �ʒu���䃂�[�h�ȊO */
		/* �����I��(���g���v�Z�Ȃ�) */
		RemVibFreq->var.State = RVM_INIT;
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�c���U�����g���Z�o																			*/
/*																									*/
/*		�T�v: �ʒu�w�ߕ����o��������̎c���U�����g�����Z�o����B									*/
/*			�@�c���U�����g���́A�ʒu�w�ߕ����o��������̃I�[�o�[�V���[�g�C							*/
/*			�@�A���_�[�V���[�g�ʂ̊e�X2��̃s�[�N�l�����߁A�I�[�o�[�V���[�g�ԁA						*/
/*			�@�A���_�[�V���[�g�Ԃ̎��Ԃ̕��ϒl�����߁A���g���ϊ����Ďc���U���Ƃ���B				*/
/*			�@���g���v�Z�́C10�̃s�[�N�܂��́C500ms�ԃs�[�N�������ꍇ�C�I������B					*/
/*			�@�v�Z���Ɉʒu�w�߂����͂��ꂽ�ꍇ�́C�v�Z�\�ȏꍇ�́C���g���v�Z�����s���C			*/
/*			�@�v�Z�s�̏ꍇ�́C�����������֖߂�B													*/
/*																									*/
/****************************************************************************************************/
void	RemVibCalculateFreq( REMVIBFREQ *RemVibFreq, LONG CtrlPosErr, BOOL CoinSts, BOOL RefzSts )
{
	LONG	lwk;
	ULONG	ulwk;
	ULONG	Sumtime;
	ULONG	tmpdiff;

	/*----------------------------------------------------------------------------------------------*/
	/*	�ʒu�΍��t�B���^����(�ʒu�΍��̃m�C�Y�J�b�g)												*/
	/*----------------------------------------------------------------------------------------------*/
	lwk = CtrlPosErr;								/* �ʒu�A���v�΍�		[pulse]					*/
	RemVibFreq->var.PosErrFilOut2 = MlibLpfilter1( lwk,
												   RemVibFreq->conf.RemVibFil,
												   RemVibFreq->var.PosErrFilOut2 );

	RemVibFreq->var.PosErrFilOut = MlibLpfilter1( RemVibFreq->var.PosErrFilOut2,
												  RemVibFreq->conf.RemVibFil,
												  RemVibFreq->var.PosErrFilOut );

	switch( RemVibFreq->var.State )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	������																						*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_INIT:
		/*------------------------------------------------------------------------------------------*/
		/*	�c���U�����g���Z�o�֘A�f�[�^�N���A 														*/
		/*------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( &(RemVibFreq->var), sizeof( RemVibFreq->var )>>2 );
		RemVibFreq->var.State = RVM_COIN;
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	�ʒu���ߊ����҂�																			*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_COIN:
		if( (RefzSts == TRUE) && (CoinSts == TRUE) )
		{ /* �ʒu�w�ߕ����o������ && �ʒu���ߊ��� */
			RemVibFreq->mon.UnRvibFrequency = 0;				/* Un�\���N���A	<S0DA>				*/
			/*--------------------------------------------------------------------------------------*/
			/*	�ʒu�΍���������				 													*/
			/*		�����[�^����~���Ă���ꍇ�́C�΍����������Ȃ��̂ł��̃V�[�P���X�ɗ��܂�		*/
			/*--------------------------------------------------------------------------------------*/
			if( (RemVibFreq->var.LastPosErr - RemVibFreq->var.PosErrFilOut) < 0 )
			{ /* �΍����{�����ɑ��� */
				RemVibFreq->var.Dir = 1;
				RemVibFreq->var.State = RVM_PEAKCHK_F;
			}
			else if( (RemVibFreq->var.LastPosErr - RemVibFreq->var.PosErrFilOut) > 0 )
			{ /* �΍����|�����ɑ��� */
				RemVibFreq->var.Dir = -1;
				RemVibFreq->var.State = RVM_PEAKCHK_F;
			}
			/* �I�[�o�[�V���[�g�^�C�}�J�E���^������ */
			RemVibFreq->var.PeakCnt = 0;
			/* �I�[�o�[�V���[�g�ʕۑ� */
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			/* �s�[�N���o�^�C���A�E�g�̂��߁A�ŏ����o���g���̔����̃J�E���^�l�ƂȂ� */
			KlibRstLongTimer( &RemVibFreq->var.TimeOutCtr );
			/* �c���U�����ԑ���p�^�C�}(ScanC����)������ */
			KlibRstLongTimer( &RemVibFreq->var.PeakCntTimer );
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	�s�[�N���o(�U�����g�����o�O)																*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_PEAKCHK_F:
		/*------------------------------------------------------------------------------------------*/
		/*	�ʒu�w�ߔ��ʁ��^�C���A�E�g����															*/
		/*------------------------------------------------------------------------------------------*/
		if( (RefzSts == FALSE)
			|| (KlibGetLongTimerMs(RemVibFreq->var.TimeOutCtr) >= REMVIB_TIMEOUT) )
		{ /* �ʒu�w�ߕ����o���� || �^�C���A�E�g�� */
			RemVibFreq->var.State = RVM_INIT;					/* �����I��(���g���v�Z�Ȃ�)			*/
			break;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�s�[�N�X�V����																			*/
		/*		�I�[�o�[�V���[�g�ʂ��������ɑ������Ă���ꍇ�Ƀs�[�N�̃J�E���^���X�V����			*/
		/*------------------------------------------------------------------------------------------*/
		if( (RemVibFreq->var.Dir * (RemVibFreq->var.PosErrFilOut - RemVibFreq->var.PeakValue)) > 0 )
		{ /* �s�[�N�X�V�� */
			/* �I�[�o�[�V���[�g�ʍő�l�X�V */
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			/* �I�[�o�[�V���[�g�ő�l�^�C�}�J�E���^(ScanC����)�X�V */
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�s�[�N���]����																			*/
		/*------------------------------------------------------------------------------------------*/
		if( ((RemVibFreq->var.Dir * (RemVibFreq->var.PeakValue - RemVibFreq->var.PosErrFilOut))
			> RemVibFreq->conf.RemVibDetWidth) )
		{ /* �s�[�N���] */
			KlibRstLongTimer( &RemVibFreq->var.TimeOutCtr );	/* �^�C���A�E�g�J�E���^�X�V			*/

			/*--------------------------------------------------------------------------------------*/
			/*	�s�[�N���]4��܂ł́C�U�����g���v�Z���Ȃ��B											*/
			/*	(���ۂɂ̓s�[�N���]3��Ōv�Z�\�����C����̔��]�̓m�C�Y�̉e�������邽�ߖ�������)	*/
			/*--------------------------------------------------------------------------------------*/
			if( RemVibFreq->var.RevCnt >= 3 )
			{ /* 4��ڂ̃s�[�N -> ���g���v�Z */
				/* �������s�[�N�̍� */
				lwk = (LONG)RemVibFreq->var.PeakCnt - (LONG)RemVibFreq->var.LLastpeakCnt;
				/* �ŏ����o���g���ŃN�����v */
				tmpdiff = MlibLimitul( lwk, REMVIB_MINDET, 0 );

				Sumtime = tmpdiff * REMVIB_DENO_OFFSET;
				RemVibFreq->var.MeanDeno =  REMVIB_DENO_OFFSET;

				/* ���ϒl�v�Z */
				ulwk = Sumtime / RemVibFreq->var.MeanDeno;
				/* �ŏ����o���g���ŃN�����v */
				RemVibFreq->var.FreqCnt = MlibLimitul( ulwk, REMVIB_MINDET, 2 );

				/* �U�����g�����o�J�n�� */
				RemVibFreq->var.State = RVM_PEAKCHK_L;

			}
			/* �s�[�N�f�[�^�X�V */
			RemVibFreq->var.LLastpeakCnt = RemVibFreq->var.LastPeakCnt;
			RemVibFreq->var.LastPeakCnt = RemVibFreq->var.PeakCnt;
			RemVibFreq->var.RevCnt++;
			RemVibFreq->var.Dir *= -1;
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	�s�[�N���o(�U�����g�����o�J�n)																*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_PEAKCHK_L:
		/*------------------------------------------------------------------------------------------*/
		/*	�ʒu�w�ߔ��ʁ��^�C���A�E�g����															*/
		/*------------------------------------------------------------------------------------------*/
		if( (RefzSts == FALSE)
			|| (KlibGetLongTimerMs( RemVibFreq->var.TimeOutCtr ) >= REMVIB_TIMEOUT) )
		{ /* �ʒu�w�ߕ����o���� || �^�C���A�E�g�� */
			RemVibFreq->var.State = RVM_COMP;					/* �����I��(���g���v�Z��)			*/
			break;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�s�[�N�X�V����																			*/
		/*		�I�[�o�[�V���[�g�ʂ��������ɑ������Ă���ꍇ�Ƀs�[�N�̃J�E���^���X�V����			*/
		/*------------------------------------------------------------------------------------------*/
		if( (RemVibFreq->var.Dir * (RemVibFreq->var.PosErrFilOut - RemVibFreq->var.PeakValue)) > 0 )
		{	/* �s�[�N�X�V�� */
			/* �I�[�o�[�V���[�g�ʍő�l�X�V */
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			/* �I�[�o�[�V���[�g�ő�l�^�C�}�J�E���^(ScanC����)�X�V */
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�s�[�N���]����																			*/
		/*------------------------------------------------------------------------------------------*/
		if( ((RemVibFreq->var.Dir * (RemVibFreq->var.PeakValue - RemVibFreq->var.PosErrFilOut))
			> RemVibFreq->conf.RemVibDetWidth) )
		{ /* �s�[�N���]�@*/
			KlibRstLongTimer( &RemVibFreq->var.TimeOutCtr );	/* �^�C���A�E�g�J�E���^�X�V			*/

			if (RemVibFreq->var.RevCnt >= 9)
			{	/* �s�[�N��10�ȏ�̏ꍇ */
				RemVibFreq->var.State = RVM_COMP;				/* �����I��(���g���v�Z��)			*/
			}
			/* �������s�[�N�̍� */
			lwk = (LONG)RemVibFreq->var.PeakCnt - (LONG)RemVibFreq->var.LLastpeakCnt;
			/* �ŏ����o���g���Ÿ���� */
			tmpdiff = MlibLimitul( lwk, REMVIB_MINDET, 0 );

			/*--------------------------------------------------------------------------------------*/
			/*	���ϒl�v�Z																			*/
			/*--------------------------------------------------------------------------------------*/
			/*	���q�����v�Z																		*/
			/*		SUM = Mean(n-1):�O�񕽋ϒl * MeanDeno(n-1):�O�񕪕� * r:�d�� + X(n):����l		*/
			/*																						*/
			/*		���j�ŏ����o���g���F0.8Hz�C�d��1.2�C�I�t�Z�b�g1000�{��10��̐����v�Z��			*/
			/*			�ȉ��̎��̕��q��0xB23E7EC�ɂȂ�LONG�T�C�Y�ȓ��B								*/
			/*--------------------------------------------------------------------------------------*/
			Sumtime =
				((RemVibFreq->var.FreqCnt * RemVibFreq->var.MeanDeno * REMVIB_WEIGHT_NUME) / REMVIB_WEIGHT_DENO)
				+ (tmpdiff * REMVIB_DENO_OFFSET);

			/*--------------------------------------------------------------------------------------*/
			/*	���ꐋ���v�Z																		*/
			/*		SUM = MeanDeno(n-1):�O�񕪕� * r:�d�� + 1										*/
			/*																						*/
			/*		���j�d��1.2�C�I�t�Z�b�g1000�{��10��̐����v�Z��SUM��25959�ɂȂ�̂�				*/
			/*			65536�𒴂��邱�Ƃ͂Ȃ��B													*/
			/*--------------------------------------------------------------------------------------*/
			RemVibFreq->var.MeanDeno =
				((RemVibFreq->var.MeanDeno * REMVIB_WEIGHT_NUME) / REMVIB_WEIGHT_DENO) + REMVIB_DENO_OFFSET;

			/* ���ϒl�v�Z */
			ulwk = Sumtime / RemVibFreq->var.MeanDeno;
			/* �ŏ����o���g���ŃN�����v*/
			RemVibFreq->var.FreqCnt = MlibLimitul( ulwk, REMVIB_MINDET, 2 );

			/* �s�[�N�f�[�^�X�V */
			RemVibFreq->var.LLastpeakCnt = RemVibFreq->var.LastPeakCnt;
			RemVibFreq->var.LastPeakCnt = RemVibFreq->var.PeakCnt;
			RemVibFreq->var.RevCnt++;
			RemVibFreq->var.Dir *= -1;
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	�U�����g���v�Z																				*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_COMP:
			if( (RemVibFreq->var.FreqCnt <= REMVIB_MINOUT) && (RemVibFreq->var.FreqCnt > 0) )
			{
				/* ���g���v�Z [0.1Hz] */
				RemVibFreq->mon.UnRvibFrequency = 10000 /(RemVibFreq->var.FreqCnt * KPI_SCCYCLEMS);	/* <S0DA> */
			}
			else
			{
				/* �ŏ��o�͎��g��������0.0Hz�Ƃ��� */
				RemVibFreq->mon.UnRvibFrequency = 0;	/* <S0DA> */
			}
			if( RefzSts == FALSE )
			{ /* �ʒu�w�ߕ����o���� */
				/* ������������ */
				RemVibFreq->var.State = RVM_INIT;
			}
		break;
	default:
		break;
	}

	RemVibFreq->var.LastPosErr = RemVibFreq->var.PosErrFilOut;
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�c���U�����g���擾�pAPI																		*/
/*																									*/
/****************************************************************************************************/
LONG	RemVibGetRemVibFreq( REMVIBFREQ *RemVibFreq )
{
	return	RemVibFreq->mon.UnRvibFrequency;		/* <S0DA> */
}

/****************************************************************************************************/
/*																									*/
/*		�c���U�����g���N���A���� 																	*/
/*																									*/
/****************************************************************************************************/
void	RemVibRsetRemVibrationFreq( REMVIBFREQ *RemVibFreq )
{
	RemVibFreq->mon.UnRvibFrequency = 0;			/* �c���U�����g�����j�^�N���A <S0DA>			*/
	RemVibFreq->var.FreqCnt = 0;					/* �c���U�����g���J�E���^�N���A					*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		�c���U�����g�����j�^�p�����[�^�v�Z															*/
/*																									*/
/****************************************************************************************************/
void	RemVibCalculateMonitorPrm( REMVIBFREQ *RemVibFreq, EGEAR *Egear,
    	                           USHORT remdetw, LONG CoinLevel, LONG ScanTime )
{
	LONG		kx, sx;

	sx = 0;
	kx = MlibScalKxgain( CoinLevel, remdetw, 1000, &sx, 0 );
	kx = MlibPcalKxgain( kx, Egear->b, Egear->a, &sx,  -24 );

	/* �ŏ����o���Ń��~�b�g */
	RemVibFreq->conf.RemVibDetWidth = MlibLimitul( kx, 0x1000000, REMVIB_MINDETWIDTH );
	/* �ʒu�΍��̃m�C�Y�J�b�g�p */
	RemVibFreq->conf.RemVibFil = MlibPcalKf1gain( REMVIB_FILFREQ, ScanTime, 1 );
}



/***************************************** end of file **********************************************/
