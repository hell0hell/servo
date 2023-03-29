/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Control �w�b�_�t�@�C��										*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	POSING�p�����x�w�ߒǉ�							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTION_CTRL_H_

#include "MtGenerator.h"
#include "Position.h"
#include "AccFilter.h"
#include "MtPRM.h"
#include "ServoIf.h"
#include "PRMBank.h"/* <S004> */

/****************************************************************************
 Defines
****************************************************************************/
#define MOTOR_STP_SPD	0x10000		/* ���[�^��~���x 24bit/OS / 256 */

enum MT_OTSTATE {
	OTSTAT_NORMAL=0,	/* Disable��� */
	OTSTAT_STOP,		/* ��~����� */
	OTSTAT_WAITNORMAL,	/* OT�����҂���� */
	OTSTAT_RETRACTION,	/* �����߂������� <V679> */
};

#define MTLT_NO_SEQ_REQ	0	/* ���b�`�V�[�P���X�Ȃ���� */
#define MTLT_SEQ_REQ	1	/* ���b�`�V�[�P���X�N����� */
#define MTLT_SEQ_DONE	2	/* ���b�`�V�[�P���X������� */
#define MTLT_CMP_STS	0x11	/* ���b�`������� */

#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
enum MT_AMSTATE {
	AMSTAT_NORMAL=0,	/* Disable��� */
	AMSTAT_STOP,		/* ��~����� */
	AMSTAT_WAITNORMAL,	/* OT�����҂���� */
};
#endif

/****************************************************************************
 Structure of the Motion Handle
****************************************************************************/
typedef struct MT_LATCH_ {
	LONG	Req;		/* ���b�`�v�� */
	LONG	Cmp;		/* ���b�`���� */
	LONG	Mode;		/* ���b�`���[�h */
	LONG	Sgnl;		/* �I��M�� */

	LONG	LtDisable;	/* ���b�`���� */
	LONG	SeqReq;		/* �A�����b�`�v�� */
	LONG	SeqCnt;		/* �A�����b�`�V�[�P���X�J�E���^ */
	union {				/* �A�����b�`�X�e�[�^�X */
		USHORT	wdCnt;
		UCHAR	cnt[2];	/* [0]:�A���񐔃J�E���g,[1]:�V�[�P���X�J�E���g */
	}SeqSts;
} MT_LATCH;

typedef struct MT_CMD_ {
	LONG	TPos;		/* �ڕW�ʒu[ref] */
	LONG	TSpeed;		/* �ڕW���x[ref/s] */
	LONG	Velocity;	/* VFF[re/s], ���x�w��[2^30/MAX] */
	LONG	Trq;		/* �g���N�w��[MAX_TRQ/2^30] */
	LONG	HoldMod;	/* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	LONG	AccFilSel;	/* �t�B���^�I�� 0,3: �Ȃ�, 1:�w��,2:�ړ����� */
	LONG	LatchSgnl;	/* ���b�`�M���I�� 0:C��,n:EXTn */
	LONG	Mtype;		/* ���[�V�����^�C�v */
	LONG	CtrlMode;	/* ���䃂�[�h */
	BOOL	CmdRstReq;	/* �R�}���h���Z�b�g�v�� */
	BOOL	MtEenable;	/* ���[�V�����L�� */
	BOOL	ServoOn;	/* �T�[�{ON�w�� */
	BOOL	SensOn;		/* �Z���TON�w�� */
	BOOL	SvonLock;	/* �T�[�{ON Lock */
	BOOL	CmdRateOn;	/* �������w�ߗL��SW<Rev2.01> */
} MT_CMD;



typedef struct MT_HNDL_ {
	LONG		seqState;	/* �w�ߐ����V�[�P���X��� */
	LONG		otState;	/* OT��� */
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
	UCHAR		amState;	/* AM��� */
#endif
	LONG		mtState;	/* ���[�V������� */
	LONG		crtMtype;	/* ���݃��[�V�����^�C�v */
	LONG		ctrlMode;	/* ���ݐ��䃂�[�h */
	ULONG		timer;		/* RampStop�p�^�C�} */
	LONG		segCnt;		/* ���[�V���������o���񐔃J�E���^ */
	MT_CMD		cmdData;	/* ���[�V�����w�߃f�[�^ */
	MG_INOUT	outData;	/* ���[�V�����o�̓f�[�^ */
	MT_LATCH	latch;		/* ���b�` */
	MG_HNDL		mtgHdl;		/* MotionGenerator�n���h�� */
	PSN_HNDL	psnHdl;		/* Position�n���h�� */
	ACF_HNDL	acfHdl;		/* AccFilter�n���h�� */
	MT_PRM		mtPrm;		/* MotionPRM�n���h�� */
	SV_CYC_CMD*	svCmd;		/* Servo If Cyclic Command Data*/
	SV_CYC_RSP*	svRsp;		/* Servo If Cyclic Response Data*/
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
INT32 MtcReqLatch(MT_HNDL* hdl, LONG mode, LONG sgnl);
/* ���b�`�v���L�����Z�� */
VOID MtcCancelLatch(MT_HNDL* hdl, LONG mode);


#define		MOTION_CTRL_H_
#endif /* MOTION_CTRL_H_ */
