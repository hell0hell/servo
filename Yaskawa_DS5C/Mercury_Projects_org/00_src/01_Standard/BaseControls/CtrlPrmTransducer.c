/****************************************************************************************************/
/*																									*/
/*																									*/
/*		����p�p�����[�^�ϐ��ꊇ�ϊ�������`														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			1. �������X�A�O���I�u�U�[�o���̃p�����[�^�ϐ��ꊇ�ϊ�����								*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2013.08.16	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "CtrlPrmTransducer.h"
#include "Mlib.h"
#include "Klib.h"



/****************************************************************************************************/
/*																									*/
/*		����p�p�����[�^�ϐ��ꊇ�ϊ��T�[�r�X����������												*/
/*																									*/
/****************************************************************************************************/
void	TranlateCtrlPrmInitialize( TRANSDUCE_CTRL *TransDuceCtrl )
{
	/* �ϊ��T�[�r�X�p�ϐ������� */
	MlibResetLongMemory( TransDuceCtrl, sizeof(*TransDuceCtrl)>>2 );
	/* Initial����DIRECT���[�h */
	TransDuceCtrl->TranslateMode = TMODE_DIRECT;
}



/****************************************************************************************************/
/*																									*/
/*		����p�p�����[�^�ϐ��ꊇ�ϊ��v������														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v�F	�������X��O���I�u�U�[�o���̃p�����[�^��ύX�����ꍇ�A�����̊֘A����ϐ��𓯎���	*/
/*				�؂�ւ���K�v������BSGDV�ł͊��荞�݋֎~�ɂ��������Ă������ASGD7�V���[�Y�ł�	*/
/*				�{�֐���p���Ĉꊇ�ϊ��v�����s���AScanA��������ScanB�����ɂĎ��ۂɕϊ������s����	*/
/*				���ƂŊ��֏����̃��X�N���������B													*/
/*				�Ȃ��AInitial���Ŋ��荞�݂��N������Ȃ��ꍇ�́A�{�����ɂĒ��ڕϊ������{����B		*/
/*																									*/
/*		���ӁF	�T�C�Y�̍ő��32�Ƃ��ALONG�T�C�Y(32bit)�ł̓]�����s���B�{�֐����g�p���ĕϐ��l��	*/
/*				�X�V���邽�߂ɂ́A�]�����A�]���悪���ꂼ��4byte�ŃA���C�����g����Ă���K�v������B	*/
/*				�܂��A2ms�ȏ�o�߂��Ă��]�����������Ȃ��ꍇ�̓^�C���A�E�g�ƂȂ�B					*/
/*																									*/
/****************************************************************************************************/
LONG	TranlateCtrlPrmRequest( TRANSDUCE_CTRL *TransDuceCtrl, void *Data, ULONG *Address,
																		UCHAR Size, CHAR Timing )
{
	LONG			i;
	LONG			ErrSts;
	ULONG			TimeOut;

	ErrSts = TR_SUCCESS;

	/* Check Interrupt Starting */
	if( TransDuceCtrl->TranslateMode == TMODE_DIRECT )
	{ /* Interrupt Stopped State */
		for( i=0; i<Size; i++ )
		{
			( (ULONG*)Address )[i] = ( (ULONG*)Data )[i];
		}
	}
	else
	{ /* Interrupt Active State */
		/* Check Request Data Size */
		if( Size > TR_DATA_MAX )
		{
			ErrSts = TR_DATA_ERROR;
		}

		/* Check Src Data Address and Dst Data Address */
		if( (Data == NULL) || (Address == NULL) )
		{
			ErrSts = TR_DATA_ERROR;
		}

		if( ErrSts != TR_DATA_ERROR )
		{
			/* Set Target Address */
			TransDuceCtrl->TargetAddress = Address;

			/* Set Parameter Data */
			for( i=0; i<Size; i++ )
			{
				TransDuceCtrl->DataArray[i] = ( (ULONG*)Data )[i];
			}

			/* Set Prameter Number (4byte unit) */
			TransDuceCtrl->TranslateSize = Size;

			/* Set Translate Request and Timing(ScanA or ScanB) */
			TransDuceCtrl->TranslateSts = Timing;

			/* Check and Wait Finish Last Translation */
			TimeOut = 0;
			while( TransDuceCtrl->TranslateSts != TR_READY )
			{
				/* Wait for Ready */
				KlibWaitus( KPI_SACYCLEUS );
				TimeOut++;
				if( TimeOut > (KPI_SCCYCLEMS * KPI_SACOUNTMS) )
				{
					ErrSts = TR_DATA_ERROR;
					TransDuceCtrl->TranslateSts = TR_READY;											  /* <S066> */
					break;																			  /* <S002> */
				}
			}
		}
	}
	return	ErrSts;
}



#pragma arm section code = "CALL_SCANA" /* <S1D2> */
/****************************************************************************************************/
/*																									*/
/*		����p�p�����[�^�ϐ��ꊇ�ϊ�����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�T�v�F	�ꊇ�ϊ��v�����m�F���A�v��������ꍇ�ɂ͑Ώۃf�[�^��ΏۃA�h���X�փR�s�[����B		*/
/*																									*/
/*		���ӁF	ScanA��������ScanB�ɂėv�����m�F���A��Scan�ł̕ϊ��v�����������ꍇ�ɖ{�������R�[��	*/
/*				����B���荞�ݏ����擪�A���Ȃ��Ƃ��ϊ��Ώەϐ����g�p���鏈���̎��s�O�ɕϊ����s��	*/
/*				�K�v������B																		*/
/*																									*/
/****************************************************************************************************/
void	TranlateCtrlPrm( TRANSDUCE_CTRL *TransDuceCtrl )
{
	LONG	i;

	/* �ϊ����s */
	for( i=0; i<TransDuceCtrl->TranslateSize; i++ )
	{
		TransDuceCtrl->TargetAddress[i] = TransDuceCtrl->DataArray[i];
	}

	/* �ϊ����� */
	TransDuceCtrl->TranslateSts = TR_READY;

	return;
}
#pragma arm section code /* <S1D2> */


/***************************************** end of file **********************************************/
