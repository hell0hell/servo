#ifndef _OVERHEAT_MANAGER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		OverheatManager.h : ���x�ߔM�Ď�������`							<S0ED>					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���x�ߔM�Ď��Ɏg�p���鏈�����`														*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2014.1.22	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"Basedef.h"
#include	"AlarmManager.h"

/*--------------------------------------------------------------------------------------------------*/
/*		�������x�p��`																				*//* <S0DE> */
/*--------------------------------------------------------------------------------------------------*/
#define CTRL_BOARD_TMPR				0U			/* �����H���x										*/
#define POWER_BOARD_TMPR			1U			/* �p���[��H���x									*/
#define SERXERR_THREE				2U			/* �ʐM�G���[�̃J�E���^���O�񔭐�����				*/
#define CTRL_BOARD					1			/* ������											*/
#define POWER_BOARD					2			/* �p���[���										*/
#define CHK_OVERHEAT_NO_OPERATION	0x0000		/* �A���[���Ȃ�(�����s�v)							*/
#define CHK_ERROR_SET_ALARM			0x0001		/* �������x���o���A���[���o��						*/
#define CHK_OVERHEAT_SET_ALARM		0x0002		/* �������x�ُ�A���[���o��							*/
#define CHK_OVERHEAT_SET_WARNING	0x0003		/* �������x���[�j���O�o��							*/
#define CHK_OVERHEAT_CLR_WARNING	0x0004		/* �������x���[�j���O����							*/



/*--------------------------------------------------------------------------------------------------*/
/*		�ݒu�����j�^�p�\���̒�`												<S03A>				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ENVMON {
	struct	{							/* �ݒu�����j�^�p�萔��`									*/
		SHORT	AmpEnvMonOffset;			/* �A���v�ݒu�����j�^�I�t�Z�b�g�l						*/
		SHORT	MotEnvMonOffset;			/* ���[�^�ݒu�����j�^�I�t�Z�b�g�l						*/
	} conf;
	struct	{							/* �ݒu�����j�^�p�ϐ���`									*/
		USHORT	EnvMonSumCnt20ms;			/* �ݒu�����j�^�ώZ�J�E���g(20ms����p)				*/
		USHORT	EnvMonSumCnt2s;				/* �ݒu�����j�^�ώZ�J�E���g(2s����p)					*/
		LONG	AmpEnvMonSum;				/* �A���v�ݒu�����j�^�ώZ�l							*//* <S050> */
		LONG	MotEnvMonSum;				/* ���[�^�ݒu�����j�^�ώZ�l							*//* <S050> */
		LONG	UnInstEnvMonAmp;			/* Un025 : �ݒu�����j�^(2s�X�V)	[0.001%]			*//* <S050> */
		LONG	UnInstEnvMonMot;			/* Un026 : �ݒu�����j�^(2s�X�V)	[0.001%]			*//* <S050> */
	} var;
} ENVMON;


/*--------------------------------------------------------------------------------------------------*/
/*		�I�[�o�[�q�[�g�Ď��p�\���̒�`											<S03A>				*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumBoardTmprState/* <S0DE> */
{
	BRD_TEMP_NORMAL		= 0L,							/* ����x����								*/
	BRD_WRN_DURING		= 1L,							/* ��x��������							*/
	BRD_ALARM_DURING	= 2L,							/* ��A���[����Ԓ�						*/
	BRD_ERROR			= 3L							/* ���ERROR								*/
};

typedef	struct CHECK_INTTMPR {/* <S0DE> */
	struct	{
		USHORT	BrdAlmTmpr;								/* ��ُ퉷�x[10��]						*/
		USHORT	BrdWrnTmpr;								/* ��x�����x[10��]						*/
		USHORT	BrdAlmClrTmpr;							/* ��ُ�N���A�\���x[10��]				*/
		USHORT	BrdAlmDetTime;							/* ��ُ퉷�x���o����[10��]				*/
	} conf;
	struct	{
		enum _enumBoardTmprState	BrdState;			/* ����									*/
//		ULONG						IntTmprTimeCnt;		/* �A���[���o�͂̂��߂̃J�E���^				*//* <S1D1> */
		ULONG						SerxErrCnt;			/* �ʐM�G���[�̃J�E���^						*/
//		USHORT	DirectAlmDetTime;						/* ��ُ퉷�x���o����[10��]				*//* <S1D1> */
		USHORT	WrnDetectTimer;							/* ��x�����x���o����[10�b]				*//* <S1D1> */
		USHORT	AlmDetectTimer;							/* ��ُ퉷�x���o����[10�b]				*//* <S1D1> */
	} var;
} CHECK_INTTMPR;

typedef	struct CHECK_OH {
	CHECK_INTTMPR	CtrlTmprAlm;			/* �������x�ُ�Ď��p								*//* <S0DE> */
	CHECK_INTTMPR	PwrTmprAlm;				/* �p���[����x�ُ�Ď��p								*//* <S0DE> */
	struct	{
		SHORT	CtrlBoadTempSum;			/* �������͉��x���Z�p								*//* <S050> */
		SHORT	PowerBoadTempSum;			/* �p���[����͉��x���Z�p								*//* <S050> */
		USHORT	TemperatureSumCnt;			/* �I�[�o�[�q�[�g���Z�p									*/
		USHORT	TemperatureErrCnt;			/* �I�[�o�[�q�[�g�ُ�J�E���g							*/
		USHORT	TemperatureMonCnt;			/* �I�[�o�[�q�[�g�`�F�b�N�o�ߎ��ԃJ�E���g				*/
		USHORT	TemperatureScanCnt;			/* �I�[�o�[�q�[�g�ُ�X�L�����J�E���g					*/
		LONG	DetTempFistPastTime;		/* ������ԊĎ��p�J�E���^								*/
		BOOL	DetTempFirstPass;			/* ���x���o����Ď��p�t���O								*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
	struct	{
		SHORT	UnAverageTemp;				/* Un170 : ���ϓ��C���x				[0.01deg]			*//* <S050> */
		SHORT	UnCtrlBoadTemp;				/* Un171 : �������͉��x			[0.1deg]			*//* <S050> */
		SHORT	UnPowerBoadTemp;			/* Un172 : �p���[����͉��x		[0.1deg]			*//* <S050> */
	} UnMon;
} CHECK_OH;



/****************************************************************************************************/
/*		Global's																						*/
/****************************************************************************************************/
void	OhmInitialize( CHECK_OH* pCheckOh );
void	EnvMonInitialize( ENVMON* pEnvMon );
void	OhmCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,
													SHORT CtrlBoadTemp, SHORT PowerBoadTemp );
void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt,
																	USHORT PowerBoadErrCnt );
void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp );



#define _OVERHEAT_MANAGER_H
#endif	// _OVERHEAT_MANAGER_H
/***************************************** end of file **********************************************/
