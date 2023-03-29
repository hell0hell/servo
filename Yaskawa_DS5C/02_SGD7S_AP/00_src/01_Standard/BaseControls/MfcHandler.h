#ifndef	_MFC_HANDLER_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MfcHandler.h : ���f���Ǐ]����p�n���h����`													*/
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
/*	Note	:	����	2009.12.22	Y.Oka	SGDV-M3 �� SVN											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�l�e�b���Z�p�萔��`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MFCPRM {							/* �l�e�b�p�p�����[�^��`							*/
#if (FLOAT_USE==TRUE)
	float	Fsft;									/* �c�s�q�����t�B���^����\�V�t�g��				*/
	float	InvFsft;								/* �c�s�q�����t�B���^����\�V�t�g�ʂ̋t��		*/
	float	Kf1[2];									/* �Q���t�B���^�Q�C���P							*/
	float	Kf2;									/* �Q���t�B���^�Q�C���Q(�P�K�����t�B���^�p)		*/
	float	Kf3;									/* �Q���t�B���^�Q�C���R(�P�K�����t�B���^�p)		*/
	float	Kj1;									/* ���J�W���Q�C���P								*/
	float	Kj2;									/* ���J�W���Q�C���Q								*/
	float	Kj2_sft;								/* ���J�W���Q�C���Q(����\UP�p��ĂȂ�)			*/
	float	Kj3;									/* ���J�W���Q�C���R								*/
	float	Kvff;									/* ���x�t�B�[�h�t�H���[�h�Q�C��					*/
	float	Ktff1;									/* �g���N�t�B�[�h�t�H���[�h�Q�C���P(���]�p)		*/
	float	Ktff2;									/* �g���N�t�B�[�h�t�H���[�h�Q�C���Q(�t�]�p)		*/
	float	Cfric;									/* �N�[�������C�⏞�g���N						*/
	float	Kfcf;									/* �N�[�������C�⏞�t�B���^�Q�C��				*/
	float	Fccflag;								/* 2-mass && Full-Closed Control Flag			*/
	float	PSFccflag;								/* 2-mass && PS Type Full-Closed Control Flag	*/
	LONG	MFCModel;								/* �@�B���f��									*/
	float	MFCType;								/* MFC�����^�C�v								*/
#else
	LONG	Fsft;									/* �c�s�q�����t�B���^����\�V�t�g��				*/
	LONG	InvFsft;								/* �c�s�q�����t�B���^����\�V�t�g�ʂ̋t��		*/
	LONG	Kf1[2];									/* �Q���t�B���^�Q�C���P							*/
	LONG	Kf2;									/* �Q���t�B���^�Q�C���Q(�P�K�����t�B���^�p)		*/
	LONG	Kf3;									/* �Q���t�B���^�Q�C���R(�P�K�����t�B���^�p)		*/
	LONG	Kj1;									/* ���J�W���Q�C���P								*/
	LONG	Kj2;									/* ���J�W���Q�C���Q								*/
	LONG	Kj2_sft;								/* ���J�W���Q�C���Q(����\UP�p��ĂȂ�)			*/
	LONG	Kj3;									/* ���J�W���Q�C���R								*/
	LONG	Kvff;									/* ���x�t�B�[�h�t�H���[�h�Q�C��					*/
	LONG	Ktff1;									/* �g���N�t�B�[�h�t�H���[�h�Q�C���P(���]�p)		*/
	LONG	Ktff2;									/* �g���N�t�B�[�h�t�H���[�h�Q�C���Q(�t�]�p)		*/
	LONG	Cfric;									/* �N�[�������C�⏞�g���N						*/
	LONG	Kfcf;									/* �N�[�������C�⏞�t�B���^�Q�C��				*/
	LONG	Fccflag;								/* 2-mass && Full-Closed Control Flag			*/
	LONG	PSFccflag;								/* 2-mass && PS Type Full-Closed Control Flag	*/
	LONG	MFCModel;								/* �@�B���f��									*/
	LONG	MFCType;								/* MFC�����^�C�v								*/
#endif /* FLOAT_USE */
} MFCPRM;



/*--------------------------------------------------------------------------------------------------*/
/*		�l�e�b���Z�p�ϐ���`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MFCTRL {							/* �l�e�b�p�p�����[�^���ϐ���`						*/
	struct	{									/* �l�e�b�p�p�����[�^��`							*/
		MFCPRM		MfcPrm[3];						/* �l�e�b�p�p�����[�^							*/
		MFCPRM		MfcPexe;						/* ���s�p�l�e�b�p�p�����[�^						*/
		LONG		MFCTypeSetting;					/* MFC�����^�C�v(�p�����[�^�ݒ�l)				*/
		LONG		MFCModelSetting;				/* �@�B���f��(�p�����[�^�ݒ�l)					*/
		BOOL		MFCFF;							/* �l�e�b���̊O���e�e(���x�C�ٸ)�L���^�����ݒ�	*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* �l�e�b�p�ϐ��f�[�^��`							*/
		LONG		CurMFCModel;					/* �@�B���f��(���݂̐ݒ�l)						*/
		LONG		execnt;							/* Execute Count for MFC Execute Control		*/
		LONG		MfcPosErr;						/* ���f���ʒu�΍�(���Z�p)	[pulse]				*/
		LONG		PosErr;							/* ���f���ʒu�΍�(���j�^�p)	[pulse]				*/
		LONG		Per64[2];						/* ���f���ʒu�΍�(���Z�p)	[pulse]				*/
		LONG		Vref;							/* ���f�����x�w��			[2^24/OvrSpd]		*/
		LONG		Pfbk;							/* ���f���ʒu�e�a			[pulse/Scan]		*/
	/*----------------------------------------------------------------------------------------------*/
		KSGAIN		Fwk1[2];						/* �Q���t�B���^���Z�p							*//*<S00A>*/
		KSGAIN		Fwk2[2];						/* �Q���t�B���^���Z�p							*//*<S00A>*/
		KSGAIN		Fwk3[2];						/* �Q���t�B���^���Z�p							*//*<S00A>*/
		KSGAIN		Fwk4[2];						/* �Q���t�B���^���Z�p							*//*<S00A>*/
		LONG		Filo;							/* �P�K�����Q���t�B���^��ʒu�w��				*/
		LONG		FCmdF;							/* ����\UP�Q���t�B���^��ʒu�w��				*/
		LONG		dFilwk;							/* �P�K�����Q���t�B���^���Z�p					*/
		LONG		Fwkrem1;						/* �P�K�����Q���t�B���^���Z�p					*/
		LONG		Fwkrem2;						/* �P�K�����Q���t�B���^���Z�p					*/
	/*----------------------------------------------------------------------------------------------*/
		KSGAIN		PCmdsub;						/* �ʒu�w�ߓ���									*//*<S00A>*/
		KSGAIN		FCmd[3];						/* �����t�B���^��ʒu�w��						*//*<S00A>*/
		LONG		dFCmd[2];						/* �����t�B���^��ʒu�w�߂P�K�����l				*/
		LONG		d2FCmd[2];						/* �����t�B���^��ʒu�w�߂Q�K�����l				*/
		LONG		d3FCmd;							/* �����t�B���^��ʒu�w�߂R�K�����l				*/
		LONG		wkrem1;							/* �ʒu�w�߉��Z�p								*/
		LONG		wkrem2;							/* �ʒu�w�߉��Z�p								*/
		LONG		PosSum[2];						/* �ʒu�w�߉��Z�ώZ�l							*/
		LONG		TrqRef;							/* ���f���g���N�w��(���j�^��)					*/
		LONG		Ktffcmdx;						/* �g���N�e�e�Q�C��								*/
		LONG		FricCmpTrq;						/* �N�[�������C�⏞�g���N						*/
		LONG		FCmdFilo[3];					/* �����t�B���^��ʒu�w��(����\UP�v�Z)			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG		PffCmd;							/* �ʒu�e�e�w��(���j�^��)						*/
		LONG		VffCmd;							/* ���x�e�e�w�� 								*/
		LONG		TffCmd;							/* �g���N�e�e�w��(���j�^��)						*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL		MfcStopReq;						/* MFC��~�v��(Fn����̗v��)					*/
	} var;
} MFCTRL;



#define _MFC_HANDLER_H /* �ēǂݍ��ݖh�~�p */
#endif /* _MFC_HANDLER_H */
/***************************************** end of file **********************************************/
