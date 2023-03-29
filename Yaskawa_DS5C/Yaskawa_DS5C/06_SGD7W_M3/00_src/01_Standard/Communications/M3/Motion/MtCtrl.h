/****************************************************************************
Description	: MECHATROLINK Motion Control Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef MOTION_CTRL_H_

#include "MtGenerator.h"
#include "Position.h"
#include "AccFilter.h"
#include "MtPRM.h"
#include "ServoIf.h"
#include "PRMBank.h"/* <S003> */


/****************************************************************************
 Defines
****************************************************************************/
#define MOTOR_STP_SPD			0x10000		/* ���[�^��~���x 24bit/OS / 256 */

#define	MCI_LTSGNL_CNFLCT		(-1)		/* ���b�`�M������ @CHECK@ */
#define	MCI_LTNO_ERR			(-2)		/* ���b�`�ԍ��ُ� @CHECK@ */

#define MTLT_NO_SEQ_REQ			0			/* ���b�`�V�[�P���X�Ȃ���� */
#define MTLT_SEQ_REQ			1			/* ���b�`�V�[�P���X�N����� */
#define MTLT_SEQ_DONE			2			/* ���b�`�V�[�P���X������� */
#define MTLT_CMP_STS			0x11		/* ���b�`������� */

enum MT_OTSTATE {
	OTSTAT_NORMAL=0,			/* 0:Disable��� */
	OTSTAT_STOP,				/* 1:��~����� */
	OTSTAT_WAITNORMAL,			/* 2:OT�����҂���� */
	OTSTAT_RETRACTION,			/* 3:�����߂������� */
};

/****************************************************************************
 Structure of the Motion Handle
****************************************************************************/
typedef struct MT_LATCH_ {
	LONG	Req;				/* ���b�`�v�� */
	LONG	Cmp;				/* ���b�`���� */
	LONG	Mode;				/* ���b�`���[�h */
	LONG	Sgnl;				/* �I��M�� */

	LONG	LtDisable;			/* ���b�`���� */
	LONG	SeqReq;				/* �A�����b�`�v�� */
	LONG	SeqCnt;				/* �A�����b�`�V�[�P���X�J�E���^ */
	union {						/* �A�����b�`�X�e�[�^�X */
		USHORT	wdCnt;
		UCHAR	cnt[2];			/* [0]:�A���񐔃J�E���g,[1]:�V�[�P���X�J�E���g */
	}SeqSts;
} MT_LATCH;

typedef struct MT_CMD_ {
	LONG	TPos;					/* �ڕW�ʒu[ref] */
	LONG	TSpeed;					/* �ڕW���x[ref/s] */
	LONG	Velocity;				/* VFF[re/s], ���x�w��[2^30/MAX] */
	LONG	Trq;					/* �g���N�w��[MAX_TRQ/2^30] */
	LONG	HoldMod;				/* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	LONG	AccFilSel;				/* �t�B���^�I�� 0,3: �Ȃ�, 1:�w��,2:�ړ����� */
	LONG	LatchSgnl;				/* ���b�`�M���I�� 0:C��,n:EXTn */
	LONG	Mtype;					/* ���[�V�����^�C�v */	
	LONG	Mtype_Cancel;			/* ���[�V�����^�C�v(�R�}���h�L�����Z���O) */	
	LONG	CtrlMode;				/* ���䃂�[�h */		
	BOOL	CmdRstReq;				/* �R�}���h���Z�b�g�v�� */
	BOOL	MtEenable;				/* ���[�V�����L�� */
	BOOL	ServoOn;				/* �T�[�{ON�w�� */
	BOOL	SensOn;					/* �Z���TON�w�� */
	BOOL	SvonLock;				/* �T�[�{ON Lock */
	BOOL	CmdRateOn;				/* �������w�ߗL��SW<Rev2.01> */
	BOOL	MtLatchReq;				/* ���[�V�������b�`�v�� */
} MT_CMD;

/* MT Manager �n���h�� */
typedef struct MT_HNDL_ {
	BOOL		ltSignlUse[4];		/* ���b�`�M�����p�t���O C��,EXT1,EXT2,EXT3 */
	LONG		seqState;			/* �w�ߐ����V�[�P���X��� */
	LONG		otState;			/* OT��� */
	LONG		mtState;			/* ���[�V������� */
	LONG		crtMtype;			/* ���݃��[�V�����^�C�v */
	LONG		ctrlMode;			/* ���ݐ��䃂�[�h */
	ULONG		timer;				/* RampStop�p�^�C�} */
	LONG		segCnt;				/* ���[�V���������o���񐔃J�E���^ */
	LONG		lstOmnMon[2];		/* �O��I�v�V�������j�^�l */
	MT_CMD		cmdData;			/* ���[�V�����w�߃f�[�^ */
	MG_INOUT	outData;			/* ���[�V�����o�̓f�[�^ */
	MT_LATCH	latch[LT_NUM];		/* ���b�` */
	MG_HNDL		mtgHdl;				/* MotionGenerator�n���h�� */
	PSN_HNDL	psnHdl;				/* Position�n���h�� */
	ACF_HNDL	acfHdl;				/* AccFilter�n���h�� */
	MT_PRM		mtPrm;				/* MotionPRM�n���h�� */
	SV_CYC_CMD	*svCmd;				/* Servo If Cyclic Command Data*/
	SV_CYC_RSP	*svRsp;				/* Servo If Cyclic Response Data*/
} MT_HNDL;




/****************************************************************************
 API's
****************************************************************************/
/* Motion Component�̏��������� */
VOID MtcInit( MT_HNDL *hdl, SV_OPTION_IF *svop_if );
/* ���[�V�����o�͏��� */
VOID MtcOutputData( MT_HNDL *hdl );
/* ���[�V�����f�[�^�X�V���� */
VOID MtcInputData( MT_HNDL *hdl );
/* ���b�`�v�����s */
INT32 MtcReqLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, LONG i );
/* ���b�`�v���L�����Z�� */
VOID MtcCancelLatch( MT_HNDL *hdl, LONG mode, LONG i );
/* �v���t�@�C���^�C�v�Ɉˑ����郂�[�V�������̐ݒ�(Motion Control) */
INT32 MtcSetMtInfoByProfileType( MT_HNDL *hdl, LONG type );

extern	const	UCHAR	LtMonTbl[];



#define MOTION_CTRL_H_
#endif /* MOTION_CTRL_H_ */



