/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MtApi.h : Motion API header file															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Histroy:																						*/
/*		Rev2.01 2007.12.14	�����x�w�ߒǉ�															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef		MOTION_API_H_
#define		MOTION_API_H_

#include "Basedef.h"
#include "error_no.h"

/****************************************************************************************************/
/*																									*/
/*		Global Definition																			*/
/*																									*/
/****************************************************************************************************/
#define	MCI_WAIT_COMP		ERR_WAIT_COMP
#define	MCI_SUCCESS			ERR_NONE
#define	MCI_CNDTN_ERR		(0x100 | ERR_CMD_CNDTN)
#define	MCI_EMGBB_ERR		(0x200 | ERR_CMD_CNDTN)
#define	MCI_SVRDY_ERR		(0x300 | ERR_CMD_CNDTN)
#define	MCI_LTCNFLCT_ERR	(0x400 | ERR_CMD_CNFLCT)
#define	MCI_SPD_ERR			(0x500 | ERR_DATA_SET)
#define	MCI_LTMOD_ERR		(0x600 | ERR_DATA_LTMOD)

/*--------------------------------------------------------------------------------------------------*/
/* defines : ���b�`���[�h(API�ݒ�p) */
/*--------------------------------------------------------------------------------------------------*/
#if 0
#define MOTION_LTMD			1	/* ���[�V�������b�`���[�h */
#define MOTION_MDL_LTMD		2	/* ���[�V�������b�`���[�h(SVCTRL) */
#define MODAL_LTMD			3	/* ���[�_�����b�`���[�h(Normal) */
#endif
/*--------------------------------------------------------------------------------------------------*/
/* defines : ���b�`���[�h(���������p) */
/*--------------------------------------------------------------------------------------------------*/
#if 0
#define INTERPOL_LTMD		4	/* ���[�V�������b�`���b�`���[�h(INTERPOLATE) */
#define EXPOSING_LTMD		5	/* ���[�V�������b�`���b�`���[�h(EXPOSING) */
#define ZRET_LTMD			6	/* ���[�V�������b�`���b�`���[�h(ZRET) */
//#define ZRET_LOCK_LTMD		7	/* ���[�V�������b�`���b�`���[�h(ZRET��) */
#define NONE_LTMD			0	/* ���b�`���[�h�Ȃ� */
#endif

/*--------------------------------------------------------------------------------------------------*/
/* defines : ���䃂�[�h */
/*--------------------------------------------------------------------------------------------------*/
#if 0
#define MT_NO_CTRLMD	0		/* ���䖳�����[�h */
#define MT_POS_CTRLMD	1		/* �ʒu���䃂�[�h */
#define MT_VEL_CTRLMD	2		/* ���x���䃂�[�h */
#define MT_TRQ_CTRLMD	3		/* �g���N���䃂�[�h */
#endif

/*--------------------------------------------------------------------------------------------------*/
/* enums : ���[�V�����^�C�v */
/*--------------------------------------------------------------------------------------------------*/
#if 0
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
#endif

/*--------------------------------------------------------------------------------------------------*/
/* enums : ���[�V������� */
/*--------------------------------------------------------------------------------------------------*/
enum MT_STATE {
	MSTAT_DISABLE=0,	/* Disable��� */
	MSTAT_WAITPG_RDY,	/* PG RDY�҂���� */
	MSTAT_READY,		/* Ready��� */
	MSTAT_RUN,			/* Run��� */
	MSTAT_WAIT_SVOFF,	/* �T�[�{OFF�҂� */
	MSTAT_RAMP_STOP,	/* �T�[�{OFF������~��� */
};

/*--------------------------------------------------------------------------------------------------*/
/* enums : ���j�^�I��ԍ� */
/*--------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------*/
/* enums : I/O�r�b�g���t�ʒu */
/*--------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------*/
/* typedefs: struct */
/* ���[�V�����w�ߍ\���̒�` */
/*--------------------------------------------------------------------------------------------------*/
#if 0
typedef struct MCI_CMD_ {
	LONG		TPos;		/* �ڕW�ʒu[ref] */
	LONG		TSpeed;		/* �ڕW���x[ref/s] */
	LONG		Velocity;	/* VFF/���x�w�� [ref/s],[2^30/MAX] */
	LONG		Trq;		/* �g���N�w��[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;	/* �����g���N����[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;	/* �����g���N����[MAX_TRQ/2^30] */
	LONG		AccRate;	/* �����萔<Rev2.01> */
	LONG		DecRate;	/* �����萔<Rev2.01> */
	ULONG		Option;		/* �I�v�V�����w�� */
	UCHAR		HoldMod;	/* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	UCHAR		LatchSgnl;	/* ���b�`�M���I�� 0:C��,n:EXTn */
	BOOL		EnableRate;	/* �����x�w�ߗL��<Rev2.01> */
	BOOL		RstReq;		/* ���[�V�������Z�b�g�v�� */
} MCI_CMD;
#endif

#ifndef		MOTION_CTRL_H_

#if 0
#ifndef __MTHNDL
#define __MTHNDL
typedef struct MTHNDL_	MTHNDL;
#endif
#ifndef __MTPRM
#define __MTPRM
typedef struct MTPRM_	MTPRM;
#endif
#ifndef __SV_OPTION_IF
#define __SV_OPTION_IF
typedef struct SV_OPTION_IF_	SV_OPTION_IF;
#endif
#endif

#endif	/* MOTION_CTRL_H_ */

/****************************************************************************************************/
/*																									*/
/*		Motion Control APIs																			*/
/*																									*/
/****************************************************************************************************/
#if 0
#ifdef __cplusplus
extern	"C"	{
#endif
/*--------------------------------------------------------------------------------------------------*/
/* Motion����API */
/*--------------------------------------------------------------------------------------------------*/
extern INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern VOID MciSetOptionCmd(MTHNDL* hdl, ULONG option);

extern INT32 MciEnableState(MTHNDL* hdl, BOOL enable);
extern INT32 MciResetMotion(MTHNDL* hdl);
extern INT32 MciSetServoOn(MTHNDL* hdl, BOOL enable, BOOL reset);
extern INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable);

extern INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode);
extern INT32 MciSetCordinatePos(MTHNDL* hdl, LONG data, BOOL ref);

extern VOID MciOutputSignals(MTHNDL* hdl, USHORT signl);
extern VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable);

/*--------------------------------------------------------------------------------------------------*/
/* Motion��Ԏ擾API */
/*--------------------------------------------------------------------------------------------------*/
extern UCHAR MciGetMType(MTHNDL* hdl);
extern BOOL MciGetServoOnStatus(MTHNDL* hdl);
extern BOOL MciGetSensOnStatus(MTHNDL* hdl);
extern UCHAR MciGetCtrlMode(MTHNDL* hdl);
extern LONG MciGetMonitorData(MTHNDL* hdl, USHORT sel, UCHAR unit);
extern BOOL MciGetDenStatus(MTHNDL* hdl);
extern BOOL MciGetLatchStatus(MTHNDL* hdl ,UCHAR *ltEdge);
extern USHORT MciGetLatchSeqStatus(MTHNDL* hdl);

extern BOOL MciGetInPosStatus(MTHNDL* hdl);
extern BOOL MciGetNearStatus(MTHNDL* hdl);
extern BOOL MciGetZpointStatus(MTHNDL* hdl);
extern UCHAR MciGetSoftLimitStatus(MTHNDL* hdl);
extern LONG MciGetOutputCmdDist(MTHNDL* hdl);

extern BOOL MciGetZspdStatus(MTHNDL* hdl);
extern BOOL MciGetMainPowerStatus(MTHNDL* hdl);
extern BOOL MciGetVlimitStatus(MTHNDL* hdl);
extern BOOL MciGetTrqLimitStatus(MTHNDL* hdl);
extern BOOL MciGetVcmpStatus(MTHNDL* hdl);

extern USHORT MciGetInputSignals(MTHNDL* hdl);
extern USHORT MciGetInPortSignals(MTHNDL* hdl);

#if ( CCSW_SFTID == SFTID_EXIST )						/*<V717>Add:Start*/
extern USHORT MciGetSomStatus(void);
#endif													/*<V717>Add:End  */

#ifdef __cplusplus
}
#endif
#endif


#endif /* MOTION_API_H_ */
