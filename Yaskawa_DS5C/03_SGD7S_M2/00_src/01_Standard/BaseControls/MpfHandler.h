#ifndef MPF_HANDLER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MpfHandler.h : ���Ɍ��o�p�n���h����`														*/
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

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���Ɍ��o���s�p��ԊǗ��ϐ���`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MPFEXEINFO {				/* �\���̒�`												*/
	BOOL	HaltCondition;					/* Over Travel or not Servo Ready State					*/
	BOOL	MotStopSts;						/* ���[�^��~���										*/
	LONG	dMotPosition;					/* �ʒuFB�����l						[pulse/ScanB]		*/
	LONG	TrefMonitor;					/* �g���N�w�ߏo��(���ϒl)			[2^24/MaxTrq]		*/
	LONG	SpdFbMonitor;					/* ���xFB����						[2^24/OvrSpd]		*/
	KSGAIN	KVfbFil;						/* ���xFB�t�B���^�Q�C��									*/
	KSGAIN	Kphasepos;						/* Gain for Motor Phase Position Cal.					*/
	KSGAIN	Kmovpos;						/* Gain for Linear Motor Move Position Cal.				*/
} MPFEXEINFO;



/*--------------------------------------------------------------------------------------------------*/
/*		���Ɍ��o�p�ϐ���`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MPFIND {						/* �\���̒�`											*/
	struct	{								/* ���Ɍ��o�p�萔�f�[�^��`								*/
		KSGAIN	Kv;								/* ���x���[�v�Q�C��				[0.1Hz]				*/
		KSGAIN	Ki;								/* ���x���[�v�ϕ��Q�C��			[0.01ms]			*/
		KSGAIN	InteglessGain;					/* �ϕ����X�Q�C��				[%]					*/
		KSGAIN	Klpf;							/* 1���x��t�B���^�Q�C��							*/
		LONG	MaxSpd;							/* ���x�w�߃s�[�N�l				[2^24/OvrSpd]		*/
		LONG	dSpd;							/* ���x�w�ߍ����l				[2^24/OvrSpd/Cycle]	*/
		LONG	ReverseSpd;						/* �t�����x��(���x�w�ߎ�)		[2^24/OvrSpd]		*/
		LONG	ReverseSpd2;					/* �t�����x��(�҂����Ԏ�)		[2^24/OvrSpd]		*/
		LONG	OverSpd;						/* ���x�I�[�o�[���o�l			[2^24/OvrSpd]		*/
		LONG	DetectRange;					/* ���͈�						[0.01mm]			*/
		LONG	MaxTrq;							/* ���͎w�߃s�[�N�l				[2^24/MaxTrq]		*/
		LONG	dTrq;							/* ���͎w�ߍ����l				[2^24/MaxTrq/Cycle]	*/
		LONG	ErrOKLevel;						/* ���o�덷OK���x��				[Pluse]				*/
		LONG	AccTime;						/* ����������					[Times/Cycle]		*/
		LONG	ClmpTime;						/* ��葬����					[Times/Cycle]		*/
		LONG	WaitTime;						/* �҂�����						[Times/Cycle]		*/
		LONG	InteglessTime;					/* �ϕ����X�J�n����				[Times/Cycle]		*/
		LONG	IncTime;						/* ���͎w�߉�������				[Times/Cycle]		*/
		LONG	ConstTime;						/* ���͎w��莞��				[Times/Cycle]		*/
		LONG	RepeatNum;						/* ���Ɍ��o�J��Ԃ���								*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* ���Ɍ��o�p�萔�f�[�^��`								*/
		BOOL	Fail;							/* �G���[����(���x�΍� or �g���N����)				*/
		BOOL	Direction;						/* �^�]�w�ߕ���										*/
		BOOL	AxisChange;						/* d-q���g���N����t���O							*/
		BOOL	ClrIntegral;					/* �ϕ��o�b�t�@�N���A								*/
		BOOL	PhaseChange;					/* �ʑ��ύX											*/
		BOOL	ZeroTorque;						/* �[���g���N�w�ߏo�̓t���O							*/
		BOOL	RangeFix;						/* ���o�̈�m��										*/
		BOOL	InteglessON;					/* 0�F�ϕ����X�����A1�F�ϕ����X���s					*/
		BOOL	ReverseLevel;					/* �t�����背�x��(0�F���x�w�ߒ��A1�F�҂����Ԓ�)		*/
		BOOL	OSFail;							/* �ߑ��x�I�[�o�[									*/
		BOOL	PhaseConfFlg;					/* ���Ɍ��o���ʊm�F�p�g���N����v���t���O			*/
		BOOL	NoGood;							/* �p�x�v�Z�G���[���(0�FOK�A1�F�Q�C�����グ��A	*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	SpdErr;							/* ���x�΍�											*/
		LONG	Ivar64[2];						/* ���x���[�v�ϕ��o�b�t�@							*/
		LONG	dAxisMaxTrqPlus;				/* ������d���ő�g���N								*/
		LONG	qAxisMaxTrqPlus;				/* ������q���ő�g���N								*/
		LONG	dAxisMaxTrqMinus;				/* ������d���ő�g���N								*/
		LONG	qAxisMaxTrqMinus;				/* ������q���ő�g���N								*/
		LONG	dAxisMaxSpdPlus;				/* ������d���ő呬�x								*/
		LONG	qAxisMaxSpdPlus;				/* ������q���ő呬�x								*/
		LONG	dAxisMaxSpdMinus;				/* ������d���ő呬�x								*/
		LONG	qAxisMaxSpdMinus;				/* ������q���ő呬�x								*/
		LONG	AxisErr;						/* d-q������										*/
		LONG	MovPos;							/* ���Ɍ��o�ړ���									*/
		LONG	TrqRefPhaConf;					/* ���Ɍ��o�m�F�p���͎w��							*/
		LONG	MovPosPhaConf;					/* ���Ɍ��o�m�F�p�ʒu								*/
		LONG	PhsOffsetComp;					/* ���Ɍ��o�ʒu�␳��								*/
		LONG	SpdOffset;						/* ����J�n�O�̑��x									*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	AdjSwpDeg;						/* �����ʑ��ύX����									*/
		LONG	AdjDegree;						/* �����ʑ�											*/
		LONG	PastTime;						/* �^�]�J�n��o�ߎ���								*/
		LONG	RefOutTime;						/* ���ԃ��[�N�o�b�t�@(���x�w�ߍ쐬�֐����̂�)		*/
		LONG	PhsOffsetCalRslt;				/* ���Ɍ��o�ʒu(���_�l�Z�o����)						*/
		LONG	PhaseOffsetCalComp;				/* ���Ɍ��o����m�F��␳��(���j�^�p)				*/
		LONG	DegReverse[4];					/* �t�����̊p�x										*/
		LONG	DegRev;							/* �t�����̍����p�x									*/
		LONG	DegFinal;						/* �ŏI�����p�x										*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	KvGain;							/* �Q�C�����������p�����[�^							*/
		LONG	Step;							/* ���Ɍ��o�V�[�P���X�X�e�b�v						*/
		LONG	RefOutStep;						/* �w�ߏo�̓V�[�P���X�X�e�b�v						*/
		LONG	RangeFixStep;					/* ���ɗ̈挟�o�V�[�P���X�X�e�b�v					*/
		LONG	Times;							/* �J��Ԃ���										*/
		LONG	FailTimes;						/* ���s��											*/
		LONG	AxisErrSign;					/* d-q���������(�g���N)							*/
		LONG	AreaCheckStep;					/* 8�����̈�`�F�b�N�X�e�b�v						*/
		LONG	ReverseCtr;						/* �t����											*/
		LONG	ConfirmStep;					/* ���Ɍ��o���ʊm�F�X�e�b�v							*/
		LONG	SpdFbFilo;						/* ���xFB�t�B���^�o��	<V302>						*/
	} var;
	BOOL	MpfSeqStartReq;						/* ���Ɍ��o�V�[�P���X�J�n�v��						*/
	LONG	MpFindSpdRef;						/* ���Ɍ��o���x�w�ߏo��								*/
	LONG	TrqFilOut;							/* �g���N�t�B���^�o��								*/
	LONG	MpfPhaseOffset;						/* ���Ɍ��o�ʑ��I�t�Z�b�g 		[65536/360deg]		*/
	LONG	MPJudgeMonitor[3];					/* ���Ɍ��o���胂�j�^								*/
	LONG	UnMpFindingTime;					/* Un082  : ���Ɍ��o���v����		[0.1s]			*/
	LONG	UnMpfMovePosP;						/* Un080  : ���Ɍ��o�������ő�ړ���[0.01mm]		*/
	LONG	UnMpfMovePosN;						/* Un081  : ���Ɍ��o�t�����ő�ړ���[0.01mm]		*/
} MPFIND;


#define MPF_HANDLER_H
#endif /* MPF_HANDLER_H */
/***************************************** end of file **********************************************/
