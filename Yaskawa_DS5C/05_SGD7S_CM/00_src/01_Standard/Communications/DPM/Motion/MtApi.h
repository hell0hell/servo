/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion API �w�b�_�t�@�C��											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	�����x�w�ߒǉ�									*/
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
#ifndef		MOTION_API_H_

#include "MtCtrl.h"
#include "ComCfg.h"

/****************************************************************************
 Structure of the Motion Handle
****************************************************************************/
/* ���[�V�����w�ߍ\���̒�` */
typedef struct MCI_CMD_ {
	LONG		TPos;		/* �ڕW�ʒu[ref] */
	LONG		TSpeed;		/* �ڕW���x[ref/s] */
	LONG		Velocity;	/* VFF/���x�w�� [ref/s],[2^30/MAX] */
	LONG		Trq;		/* �g���N�w��[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;	/* �����g���N����[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;	/* �����g���N����[MAX_TRQ/2^30] */
	ULONG		AccRate;	/* �����萔<Rev2.01> */
	ULONG		DecRate;	/* �����萔<Rev2.01> */
	ULONG		Option;		/* �I�v�V�����w�� */
	LONG		HoldMod;	/* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	LONG		LatchSgnl;	/* ���b�`�M���I�� 0:C��,n:EXTn */
	BOOL		EnableRate;	/* �����x�w�ߗL��<Rev2.01> */
	BOOL		RstReq;		/* ���[�V�������Z�b�g�v�� */
} MCI_CMD;

/****************************************************************************
 API's
****************************************************************************/
/* INTERPOLATE�w�ߐݒ� */
INT32 MciSetInterpolateCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* LATCH�w�ߐݒ� */
INT32 MciSetLatchCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* POSING�w�ߐݒ� */
INT32 MciSetPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* EX_POSING�w�ߐݒ� */
INT32 MciSetExPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* HOMING�w�ߐݒ� */
INT32 MciSetHomingCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* FEED�w�ߐݒ� */
INT32 MciSetFeedCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* HOLD�w�ߐݒ� */
INT32 MciSetHoldCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* VELCTRL�w�ߐݒ� */
INT32 MciSetVelCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* TRQCTRL�w�ߐݒ� */
INT32 MciSetTrqCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* OPTION�w�ߐݒ� */
VOID MciSetOptionCmd(MT_HNDL* hdl, ULONG option);

/* ���[�V������Ԑ؂�ւ� */
INT32 MciEnableState(MT_HNDL* hdl, BOOL enable);
/* ���[�V������ԃ��Z�b�g */
INT32 MciResetMotion(MT_HNDL* hdl);
/* �T�[�{�I���w�ߔ��s */
INT32 MciSetServoOn(MT_HNDL* hdl, BOOL enable, BOOL reset);
/* �Z���T�I���w�ߔ��s */
INT32 MciSetSensOn(MT_HNDL* hdl, BOOL enable);

/* ���b�`�v�����s(Motion�p) */
INT32 MciRequestMtLatch(MT_HNDL* hdl, LONG mode, LONG signl);
/* Modal���b�`�v�����s */
INT32 MciRequestMdlLatch(MT_HNDL* hdl, UCHAR mode, LONG signl);
/* ���b�`�L�����Z������ */
INT32 MciCancelLatch(MT_HNDL* hdl, LONG mode);
/* ���W�n�ݒ� */
INT32 MciSetCordinatePos(MT_HNDL* hdl, LONG data, BOOL ref);
/* �����[�gI/O�o��	*/
VOID MciOutputSignals(MT_HNDL* hdl, USHORT signl);

/* Motion��Ԏ擾API */
/* ���j�^�f�[�^�擾	*/
LONG MciGetMonitorData(MT_HNDL* hdl, USHORT sel, UCHAR unit);
/* LATCH�X�e�[�^�X�擾	*/
BOOL MciGetLatchStatus(MT_HNDL* hdl ,LONG *ltEdge);
/* I/O���j�^�f�[�^�擾	*/
USHORT MciGetInputSignals(MT_HNDL* hdl);
/* ���j�^�f�[�^(�|�[�g����)�擾	*/
USHORT MciGetInPortSignals(MT_HNDL *hdl);
#if 0
#if ( CCSW_SFTID == SFTID_EXIST )
/* �Z�[�t�e�B���W���[�����j�^�f�[�^�擾	*/
USHORT MciGetSomStatus(void);
#endif
#endif
/****************************************************************************
 Defines
****************************************************************************/
/* API�߂�l*/
#define	MCI_WAIT_COMP		ERR_WAIT_COMP
#define	MCI_SUCCESS			ERR_NONE
#define	MCI_CNDTN_ERR		ERR_CMD_CNDTN
#define	MCI_EMGBB_ERR		ERR_CMD_CNDTN
#define	MCI_SVRDY_ERR		ERR_CMD_CNDTN
#define	MCI_LTCNFLCT_ERR	ERR_CMD_CNFLCT
#define	MCI_SPD_ERR			ERR_DATA_SET
#define	MCI_LTMOD_ERR		ERR_DATA_LTMOD

/* ���b�`���[�h(API�ݒ�p) */
#define MOTION_LTMD			1	/* ���[�V�������b�`���[�h */
#define MOTION_MDL_LTMD		2	/* ���[�V�������b�`���[�h(SVCTRL) */
#define MODAL_LTMD			3	/* ���[�_�����b�`���[�h(Normal) */
/* ���b�`���[�h(���������p) */
#define INTERPOL_LTMD		4	/* ���[�V�������b�`���b�`���[�h(INTERPOLATE) */
#define EXPOSING_LTMD		5	/* ���[�V�������b�`���b�`���[�h(EXPOSING) */
#define ZRET_LTMD			6	/* ���[�V�������b�`���b�`���[�h(ZRET) */
//#define ZRET_LOCK_LTMD		7	/* ���[�V�������b�`���b�`���[�h(ZRET��) */
#define NONE_LTMD			0	/* ���b�`���[�h�Ȃ� */

/* ���䃂�[�h */
#define MT_NO_CTRLMD	0		/* ���䖳�����[�h */
#define MT_POS_CTRLMD	1		/* �ʒu���䃂�[�h */
#define MT_VEL_CTRLMD	2		/* ���x���䃂�[�h */
#define MT_TRQ_CTRLMD	3		/* �g���N���䃂�[�h */

/* ���[�V�����^�C�v */
enum MOTIONTYPE {
	MTYPE_NONE=0,	/* 0:���[�V������~�� */ 
	MTYPE_POSING,	/* 1:�ʒu���� */ 
	MTYPE_EXPOSING,	/* 2:�O���ʒu���� */ 
	MTYPE_HOMING,	/* 3:���_���A */
	MTYPE_FEED,		/* 4:�葬���� */
	MTYPE_HOLD,		/* 5:��~ */
	MTYPE_INTERPOL,	/* 6:��ԑ��� */ 
	MTYPE_VELCTRL,	/* 7:���x���� */
	MTYPE_TRQCTRL,	/* 8:�g���N���� */
	MTYPE_FOLLOW,	/* 9:�t�H���[�A�b�v */ 
};

/* ���[�V������� */
enum MT_STATE {
	MSTAT_DISABLE=0,	/* Disable��� */
	MSTAT_WAITPG_RDY,	/* PG RDY�҂���� */
	MSTAT_READY,		/* Ready��� */
	MSTAT_RUN,			/* Run��� */
	MSTAT_WAIT_SVOFF,	/* �T�[�{OFF�҂� */
	MSTAT_RAMP_STOP,	/* �T�[�{OFF������~��� */
};

/* ���j�^�I��ԍ� */
enum MCI_MONITOR {
	POS_MON = 0,	/* 0: POS */
	MPOS_MON,		/* 1: MPOS */
	PERR_MON,		/* 2: PERR */
	APOS_MON,		/* 3: APOS */
	LPOS_MON,		/* 4: LPOS */
	IPOS_MON,		/* 5: IPOS */
	TPOS_MON,		/* 6: TPOS */
	DUMMY1_MON,		/* 7: ---- */
	FSPD_MON,		/* 8: FSPD */
	CSPD_MON,		/* 9: CSPD */
	TSPD_MON,		/* a: TSPD */
	TRQ_MON,		/* b: TRQ */
	DUMMY2_MON,		/* c: ---- */
	DUMMY3_MON,		/* d: ---- */
	OMN1_MON,		/* e: OMN1 */
	OMN2_MON,		/* f: OMN2 */
};

/* I/O�r�b�g���t�ʒu */
enum MCI_SGNLBIT {
	P_OT_SGNL=0,
	N_OT_SGNL,
	DEC_SGNL,
	PA_SGNL,
	PB_SGNL,
	PC_SGNL,
	EXT1_SGNL,
	EXT2_SGNL,
	EXT3_SGNL,
	BK_SGNL,
	EMG_SGNL,
};


/* defines */
#define TREFMODE_NON		0
#define TREFMODE_TLIM1		1
#define TREFMODE_TFF		2
#define TREFMODE_TLIM2		3
#define TREFMODE_TFF_TLIM	4

#define		MOTION_API_H_
#endif /* MOTION_API_H_ */
