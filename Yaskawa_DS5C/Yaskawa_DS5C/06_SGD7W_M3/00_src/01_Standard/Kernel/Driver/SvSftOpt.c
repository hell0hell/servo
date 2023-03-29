/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SvSftOpt.c : �Z�[�t�e�B�I�v�V�����J�[�h���o����												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	�@ �\ : �Z�[�t�e�B�I�v�V�����J�[�h�̌��o�������s���B											*/
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
/*		Rev.1.00 : 2007.02.01  (Den-S-Kai)															*/
/*					Comment rearranging for Sigma5 BaseSoft											*/
/*		Rev.1.01 : 2007.02.03	R.Uda																*/
/*					�I�v�V�����\���̂̌������ɂ��I�v�V�����ϐ��̑S�ʉ���							*/
/*		Rev.2.00 : 2014.01.07	<S0C1>																*/
/*					SigmaV->Sigma7�ڐA																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/****************************************************************************************************/
#include	"SvOptCardIf.h"
#include	"HwDeviceIf.h"



/****************************************************************************************************/
/*																									*/
/*		Initialize Servo-Safety Option I/F Variables												*/
/*																									*/
/****************************************************************************************************/
void	SftOptInitSvSafetyOptVar( OPSFT_HNDL *SftOpt )
{

/*--------------------------------------------------------------------------------------------------*/
/*		Reset SftOpt Parameters/Variables															*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( SftOpt, sizeof(*SftOpt)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		Initialize ID/Version/Number																*/
/*--------------------------------------------------------------------------------------------------*/
		SftOpt->conf.OpInfo.IdType = SFTOP_ID_NONE;				/* �I�v�V�����h�c			 		*/
		SftOpt->conf.OpInfo.YNumber = 0xFFFF;					/* �I�v�V�����x�d�l�ԍ�				*/
		SftOpt->conf.OpInfo.SoftVer = 0xFFFF;					/* �I�v�V�����\�t�g�u����			*/
		SftOpt->conf.OpInfo.SoftYspVer = 0xFFFF;				/* �I�v�V�����\�t�g�x�d�l�u����		*/
		SftOpt->conf.OpInfo.SoftTstVer = 0xFFFF;				/* �I�v�V�����\�t�g�e�X�g�u����		*/
		SftOpt->conf.OpInfo.PrmVer = 0xFFFF;					/* �I�v�V�����p�����[�^�u����		*/

		return;
}



/****************************************************************************************************/
/*																									*/
/*		Detect Safety Option Card																	*/
/*																									*/
/****************************************************************************************************/
void	SftOptDetectSafetyOption( OPSFT_HNDL *SftOpt, ALMMAN_HANDLE *AlmManager )
{
	USHORT	i,cnt = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		ID�M���ǂݏo�������i3��ǂݑ������t�B���^�j													*/
/*--------------------------------------------------------------------------------------------------*/
		for(i=0; i<3; i++) 
		{
			/* �I�v�V�����J�[�h�h�c�R�[�h�Ǐo�� */
			if( HALDRV_GET_GPIO02 )
			{
				cnt++;
			}
			
			/* ����Ǐo���҂����� [250us] */
//			KpiResetWatchdogWaitus( 250 );
		}

		/* 2��/3��ȏ�ŃI�v�V�����J�[�h�ڑ��Ƃ݂Ȃ� */
		if( cnt >= 2 )
		{
			SftOpt->conf.OpInfo.f.Connect = TRUE;
		}
		else
		{
			SftOpt->conf.OpInfo.f.Connect = FALSE;
		}
#if	CSW_OPT_CARD_SAFETY == CSW_SAFETY_NO_SUPPORT
		if ( SftOpt->conf.OpInfo.f.Connect == TRUE )
		{
// 			�R�����g�F�b�菈�u		
//			ALMSetServoAlarm( AlmManager, ALM_SFTOP_NSUP );	/* A.E74 : �Z�[�t�e�B�I�v�V�����J�[�h���T�|�[�g */
		}
#endif
		return;
}
/***************************************** end of file **********************************************/