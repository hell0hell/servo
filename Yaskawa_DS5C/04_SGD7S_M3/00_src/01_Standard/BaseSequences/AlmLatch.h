/*****************************************************************************
Description		: �A���[�����b�`�@�\�w�b�h�t�B�[��
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.10.11		created

*****************************************************************************/

#ifndef ALARM_LATCH_API_H
#define ALARM_LATCH_API_H

#include "BaseControls.h"
#include "SequenceIo.h"
#include "CheckMotSts.h"
#include "PowerManager.h"
#include "Pls.h"
#include "SvOptCardIf.h"
#include "UnitPrmStruct.h"					/* <S212> */

#define EEPSIZE_ALMMON		64

/*--------------------------------------------------------------------------------------------------*/
/*	�A���[�����b�`���j�^																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _ALMLTMON
{
	/*----------------------------------------------------------------------------------------------*/
		SHORT	MotSpd;					/* ���[�^�ʒu����(2ms)				[min-1/mm/s]			*/
		SHORT	SpdRef;					/* ���x�w��							[min-1/mm/s]			*/
		SHORT	TrqRef;					/* �g���N�w��						[%]						*/
		SHORT	PcmdSpd;				/* �ʒu�w�ߍ���(2ms)				[min-1/mm/s]			*/
		LONG	PosErr;					/* �ʒu�΍��o��(�l�̌ܓ�)									*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	InterSignal;			/* �����M��(���j�^�p)										*/
		ULONG	InSignal;				/* ���͐M��(���j�^�p)										*/
		ULONG	OutSignal;				/* �o�͐M��(���j�^�p)										*/
	/*----------------------------------------------------------------------------------------------*/
		SHORT	DcVoltage;				/* ���H����d��					[V]						*/
		SHORT	IFBMon;					/* �d���e�a���j�^					[15000/Imax]			*/
		USHORT	MotLoad;				/* �ݐϕ��ח� 						[%]						*/
		USHORT	RgPower;				/* �񐶏���d��						[%]						*/
		USHORT	DbPower;				/* �c�a����d�� 					[%]						*/
		USHORT	MaxMotLoad10s;			/* �ő�ݐϕ��ח� 					[%�i����10s�j]			*/
		USHORT	JLRatio;				/* �������[�����g��/�d�ʔ�			[%]						*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	EncComErrCnt;			/* �V���A���G���R�[�_�ʐM�ُ��							*/
		USHORT	M2ComErrCnt;			/* MECHATROLINK�ʐM�ُ��									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	SafetyIOSignal;			/* �Z�[�t�e�B���o�͐M��(���j�^�p) <V066>					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	AlmCode;				/* �A���[���R�[�h											*/
		USHORT	spare1;					/* �\��														*/
		ULONG	TmStamp;				/* �A���[���^�C���X�^���v									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	spare2[6];				/* �\��														*/
	/*----------------------------------------------------------------------------------------------*/
} ALMLTMON;

/*--------------------------------------------------------------------------------------------------*/
/*	�A���[�����b�`�p�f�[�^��`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _ALMLATCH_HNDL
{
	struct {
		BOOL	LastAlmStatus;					/* �O��A���[���X�e�[�^�X							*/
	} var;
	/*----------------------------------------------------------------------------------------------*/
	ALMLTMON			AlmLtMon;			/* �A���[�����b�`���j�^(�X�V)						*/
	ALMLTMON			LatchMon;			/* �A���[�����b�`���j�^(�A���[���������X�V)			*/
	ANLG_UN_MON_DATA	*UnMonitor;			/* Amon/Un���j�^�f�[�^�ւ̃|�C���^						*/
	SEQ_MOT_STS			*SeqMotSts;			/* ScanC�ɂĎ擾�E�쐬���郂�[�^���					*/
	BPRMDAT				*Bprm;				/* ��{�p�����[�^��`									*/
	BASE_LOOPCTRLS		*BaseLoops;			/* ���䃋�[�v���Z�p�f�[�^								*/
	POW_MNG_HNDL		*PowerManager;		/* �d���Ǘ��p�f�[�^(���j�b�g����)						*/
	SEQ_CTRL_OUT		*SeqCtrlOut;		/* ScanC�o�̓f�[�^										*/
	MENCV				*FencV;				/* �t���N�G���R�[�_�f�[�^								*/
	MENCV				*MencV;				/* ���[�^�G���R�[�_�f�[�^�ւ̃|�C���^					*/
	BE_SEQ_HNDL			*BeSequence;		/* Base Enable Sequence Handle							*/
	BASE_CTRL_HNDL		*BaseControls;		/* ��{����p���L�f�[�^									*/
	BASE_CTRL_OUT		*BaseCtrlOut;		/* ScanB�o�̓f�[�^										*/
	ALMMAN_HANDLE		*AlmManager;		/* �A���[������p�\����									*/
	SEQ_IO_HNDL			*IoSignals;			/* �V�[�P���XI/O 										*/
	SERVO_CONTROL_IF	*SvControlIf;		/* M3 I/F�f�[�^											*/
	PLSIFV 				*pPlsIfV;			/* <S00C> Command Pulse Interface						*/
	OPSFT_HNDL			*SftOpt;			/* �Z�[�t�e�B�[���W���[���f�[�^�ւ̃|�C���^		*/
	UNI_PRM_STR			*UniPrm;			/* �p�����[�^�n���h���ւ̃|�C���^				 <S212> */

} ALMLATCH_HNDL;





/****************************************************************************************************/
/*																									*/
/*		�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
extern void AlmLatchMonitorInitial( ALMLATCH_HNDL *hAlmLatchHndl );
extern void AlmLatchMonitorSequence( ALMLATCH_HNDL *hAlmLatchHndl );




#endif	/* ALARM_LATCH_API_H */

/***************************************** end of file **********************************************/

