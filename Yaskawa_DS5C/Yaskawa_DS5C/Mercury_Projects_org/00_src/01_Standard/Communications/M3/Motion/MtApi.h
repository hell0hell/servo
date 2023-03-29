/****************************************************************************
Description	: MECHATROLINK Motion API
----------------------------------------------------------------------------
File name		: MtApi.h			<S008>

Author			: Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project			: Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef MOTION_API_H_


#include "MtCtrl.h"
#include "ComCfg.h"


/* ���[�V�����w�ߍ\���̒�` */
typedef struct MCI_CMD_ {
	LONG		TPos;				/* �ڕW�ʒu[ref] */
	LONG		TSpeed;				/* �ڕW���x[ref/s] */
	LONG		Velocity;			/* VFF/���x�w�� [ref/s],[2^30/MAX] */
	LONG		Trq;				/* �g���N�w��[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;			/* �����g���N����[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;			/* �����g���N����[MAX_TRQ/2^30] */
	ULONG		AccRate;			/* �����萔 */
	ULONG		DecRate;			/* �����萔 */
	LONG		HoldMod;			/* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	LONG		LatchSgnl;			/* ���b�`�M���I�� 0:C��,n:EXTn */
	LONG		RateType;			/* �������x�w�߃^�C�v [RATE_CMD/ACC_TIME/RATE_PRM] */
	BOOL		RstReq;				/* ���[�V�������Z�b�g�v�� */
	BOOL		MtLatchReq;			/* ���[�V�������b�`�v�� */
	BOOL		HomeDir;			/* ���_���A���� [0�F������ 1:������] */
	LONG		Sratio;				/* SPOSING�pS����Ԕ䗦 */
} MCI_CMD;



/****************************************************************************
 API's
****************************************************************************/
/* Motion Api */
/* INTERPOLATE�w�ߐݒ� */
INT32 MciSetInterpolateCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* LATCH�w�ߐݒ� */
INT32 MciSetLatchCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* POSING�w�ߐݒ� */
INT32 MciSetPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* SPOSING�w�ߐݒ�	*/
INT32 MciSetSposingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* EX_POSING�w�ߐݒ� */
INT32 MciSetExPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* HOMING�w�ߐݒ� */
INT32 MciSetHomingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* HOMING�w�ߐݒ�(DEC�M������) */
INT32 MciSetHomingCmdWithDec( MT_HNDL *hdl, MCI_CMD *cmd );
/* FEED�w�ߐݒ� */
INT32 MciSetFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* EX_FEED�w�ߐݒ� */
INT32 MciSetExFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* HOLD�w�ߐݒ� */
INT32 MciSetHoldCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* VELCTRL�w�ߐݒ� */
INT32	MciSetVelCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* VELCTRL�w�ߐݒ�(�������x�w�߂���) */
INT32 MciSetVelCmdWithAccr( MT_HNDL *hdl, MCI_CMD *cmd );
/* TRQCTRL�w�ߐݒ� */
INT32 MciSetTrqCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* CMD_PAUSE �w�ߐݒ� */
INT32 MciSetCmdPause( MT_HNDL *hdl, MCI_CMD *cmd );
/* CMD_CANCEL �w�ߐݒ� */
INT32 MciSetCmdCancel( MT_HNDL *hdl, MCI_CMD *cmd );
/* �ړ��R�}���h�ꎞ��~�w�ߎ��̃A�v���[�`���x�[���N���A�ݒ� */
VOID MciClearZretSpd( MT_HNDL *hdl );
/* �ړ��R�}���h�ꎞ��~�w�ߎ��̃A�v���[�`���x���A�ݒ� */
VOID MciReturnZretSpd( MT_HNDL *hdl );
/* OPTION�w�ߐݒ� */
VOID MciSetOptionCmd( MT_HNDL *hdl, ULONG option );
/* ���[�V������Ԑ؂�ւ� */
INT32 MciEnableState( MT_HNDL *hdl, BOOL enable );
/* �T�[�{�I���w�ߔ��s */
INT32 MciSetServoOn( MT_HNDL *hdl, BOOL enable, BOOL reset );
/* �Z���T�I���w�ߔ��s */
INT32 MciSetSensOn( MT_HNDL *hdl, BOOL enable );
/* ���b�`�v�����s(Motion�p) */
INT32 MciRequestMtLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, UCHAR i );
/* Modal���b�`�v�����s */
INT32 MciRequestMdlLatch( MT_HNDL *hdl, UCHAR mode, LONG sgnl, LONG i );
/* ���b�`�L�����Z������ */
INT32 MciCancelLatch( MT_HNDL *hdl, LONG mode, LONG i );
/* ���W�n�ݒ� */
INT32 MciSetCordinatePos( MT_HNDL *hdl, LONG data, BOOL ref, BOOL ref_clr );
/* ���[�V������ԃ��Z�b�g */
INT32 MciResetMotion( MT_HNDL *hdl );

/* Motion��Ԏ擾API */
/* ���j�^�f�[�^�擾(M3�W���T�[�{�v���t�@�C��) */
LONG MciGetM3StdMonitorData( MT_HNDL *hdl, LONG sel );
/* ���j�^�f�[�^�擾(M2�݊��v���t�@�C��) */
LONG MciGetM2MonitorData( MT_HNDL *hdl, USHORT sel, UCHAR unit );
/* ���j�^�f�[�^(�|�[�g����)�擾(M2�݊��v���t�@�C��) */
USHORT MciGetInPortSignals( MT_HNDL *hdl );
/* LATCH�X�e�[�^�X�擾 */
BOOL MciGetLatchStatus( MT_HNDL *hdl, LONG *ltEdge, LONG i );
/* I/O���j�^�f�[�^�擾 */
USHORT	MciGetInputSignals( MT_HNDL *hdl );
/* LtRdy(LT_EXE1,2)�̃��j�^�M���쐬 */
LONG MciGetLtRdy( MT_HNDL *hdl, LONG i );
/* �w�ߑ��x�쐬����(���x����ŉ������x�ݒ肪����̏ꍇ) */
LONG MciMakeCrtVref( MG_HNDL *hdl, MG_INOUT *inOut );



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
#define MCI_ACCR_ERR		ERR_DATA_SET
#define MCI_DATA_CLAMP		ERR_DATA_CLAMP

/* ���b�`���[�h(API�ݒ�p) */
#define MOTION_LTMD			1			/* ���[�V�������b�`���[�h */
#define MOTION_MDL_LTMD		2			/* ���[�V�������b�`���[�h(SVCTRL) */
#define MODAL_LTMD			3			/* ���[�_�����b�`���[�h(Normal) */

/* defines : ���b�`���[�h(���������p) */
#define INTERPOL_LTMD		4			/* ���[�V�������b�`���b�`���[�h(INTERPOLATE) */
#define EXPOSING_LTMD		5			/* ���[�V�������b�`���b�`���[�h(EXPOSING) */
#define ZRET_LTMD			6			/* ���[�V�������b�`���b�`���[�h(ZRET) */
//#define ZRET_LOCK_LTMD	7			/* ���[�V�������b�`���b�`���[�h(ZRET��) */
#define EXFEED_LTMD			8			/* ���[�V�������b�`���b�`���[�h(EX_FEED) */
#define NONE_LTMD			0			/* ���b�`���[�h�Ȃ� */

/* ���䃂�[�h */
#define MT_NO_CTRLMD		0			/* ���䖳�����[�h */
#define MT_POS_CTRLMD		1			/* �ʒu���䃂�[�h */
#define MT_VEL_CTRLMD		2			/* ���x���䃂�[�h */
#define MT_TRQ_CTRLMD		3			/* �g���N���䃂�[�h */

/* �v���t�@�C���^�C�v */
#define M2_PROFILE			0x00
#define M3STD_PROFILE		0x10
#define EVTID_PROFILE		0x01

/* �������w�߃^�C�v */
#define RATE_PRM			0			/* �p�����[�^�ɂ��������w�� */
#define RATE_ACC_TIME		1			/* �������Ԃɂ��������x�w�� */
#define RATE_CMD			2			/* �ʐM�f�[�^�ɂ��������x�w�� */

/* ���b�`�ԍ� */
#define MTLATCH_NUM			0			/* ���[�V�������b�`�p���b�`�ԍ� */
#define SEQLATCH_NUM		1			/* �A�����b�`�p���b�`�ԍ� */

/* VFF,TFF,TLIM, ACCR �̍ő�l */
#define	MT_MAXTRQ_30		0x40000000	/* TRQ�w�߂�MAX�l   2^30 */
#define	MT_OSVEL_30			0x40000000	/* ���x�w�߂�MAX�l  2^30 */
#define MT_MAX_ACCR 		0x1FFFFFFF  /* ACCR,DECR[ref/cyc^2] �ő�l 2^30-1 */
#define MT_MAX_ACCR_VEL		0x7FFFFFFF  /* ACCR,DECR[ref/cyc^2] �ő�l 2^31-1 (���x���䎞) */
#define MT_LONG_MAX			0x7FFFFFFF 

/* TREF MODE */
#define TREFMODE_NON		0
#define TREFMODE_TLIM1		1
#define TREFMODE_TFF		2
#define TREFMODE_TLIM2		3
#define TREFMODE_TFF_TLIM	4

/* VLIM */
#define VLIM_MODE_DISABLE	0
#define VLIM_MODE_ENABLE	1

/* HOLD MODE */
#define HOLD_MODE_RSLOWDOWN_STOP	0
#define HOLD_MODE_RAPID_STOP		1

/* define : ���j�^�I���R�[�h  */
#define SELMON_FBPOS64L		0xE
#define SELMON_FBPOS64H		0xF


/* enums : ���[�V�����^�C�v */
enum MOTIONTYPE {
	MTYPE_NONE = 0,				/* 0:���[�V������~�� */ 
	MTYPE_POSING,				/* 1:�ʒu���� */ 
	MTYPE_EXPOSING,				/* 2:�O���ʒu���� */ 
	MTYPE_HOMING,				/* 3:���_���A */
	MTYPE_HOMING_WITH_DEC,		/* 4:���_���A(DEC����) */
	MTYPE_FEED,					/* 5:�葬���� */
	MTYPE_EXFEED,				/* 6:�O�����͈ʒu���ߒ葬���� */
	MTYPE_HOLD,					/* 7:��~ */
	MTYPE_INTERPOL,				/* 8:��ԑ��� */ 
	MTYPE_VELCTRL,				/* 9:���x���� */
	MTYPE_VELCTRL_WITH_ACCR,	/* 10:���x����(�������w�肠��) */
	MTYPE_TRQCTRL,				/* 11:�g���N���� */
	MTYPE_FOLLOW,				/* 12:�t�H���[�A�b�v */
	MTYPE_SPOSING,				/* 13:S���ʒu���� */
	MTYPE_SHOLD,				/* 14:S����~ */
};

/* enums : ���[�V������� */
enum MT_STATE {
	MSTAT_DISABLE = 0,			/* 0:Disable��� */
	MSTAT_WAITPG_RDY,			/* 1:PG RDY�҂���� */
	MSTAT_READY,				/* 2:Ready��� */
	MSTAT_RUN,					/* 3:Run��� */
	MSTAT_WAIT_SVOFF,			/* 4:�T�[�{OFF�҂� */
	MSTAT_RAMP_STOP,			/* 5:�T�[�{OFF������~��� */
};

/* enums : ���j�^�I��ԍ� */
enum MCI_M3STD_MONITOR {
	M3STD_APOS_MON = 0,			/* 0: APOS */
	M3STD_CPOS_MON,				/* 1: CPOS */
	M3STD_PERR_MON,				/* 2: PERR */
	M3STD_LPOS1_MON,			/* 3: LPOS1 */
	M3STD_LPOS2_MON,			/* 4: LPOS2 */
	M3STD_FSPD_MON,				/* 5: FSPD */
	M3STD_CSPD_MON,				/* 6: CSPD */
	M3STD_TRQ_MON,				/* 7: TRQ */
	M3STD_ALM_DTL_MON,			/* 8: ALM_DTL */
	M3STD_MPOS_MON,				/* 9: MPOS */
	M3STD_DUMMY2_MON,			/* A: ---- */
	M3STD_DUMMY3_MON,			/* B: ---- */
	M3STD_CMN1_MON,				/* C: CMN1 */
	M3STD_CMN2_MON,				/* D: CMN2 */
	M3STD_OMN1_MON,				/* E: OMN1 */
	M3STD_OMN2_MON,				/* F: OMN2 */
};

enum MCI_M2_MONITOR {
	M2_POS_MON = 0,				/* 0: POS */
	M2_MPOS_MON,				/* 1: MPOS */
	M2_PERR_MON,				/* 2: PERR */
	M2_APOS_MON,				/* 3: APOS */
	M2_LPOS_MON,				/* 4: LPOS */
	M2_IPOS_MON,				/* 5: IPOS */
	M2_TPOS_MON,				/* 6: TPOS */
	M2_DUMMY1_MON,				/* 7: ---- */
	M2_FSPD_MON,				/* 8: FSPD */
	M2_CSPD_MON,				/* 9: CSPD */
	M2_TSPD_MON,				/* a: TSPD */
	M2_TRQ_MON,					/* b: TRQ */
	M2_DUMMY2_MON,				/* c: ---- */
	M2_DUMMY3_MON,				/* d: ---- */
	M2_OMN1_MON,				/* e: OMN1 */
	M2_OMN2_MON,				/* f: OMN2 */
};

/* enums : ���ʃ��j�^�I��ԍ� */
enum MCI_COMMON_MONITOR {
	TPOS_MON = 0,				/* 0: TPOS */
	IPOS_MON,					/* 1: IPOS */
	POS_OFST_MON,				/* 2: POS_OFSET */
	TSPD_MON,					/* 3: TSPD */
	SPD_LIM_MON,				/* 4: SPD_LIM */
	TRQ_LIM_MON,				/* 5: TRQ_LIM */
	SV_STAT_MON,				/* 6: SV_STAT */
	RESERVE_MON,				/* 7: Reserve */
	INIT_PGPOS_L_MON,			/* 8: INIT_PGPOS(Low) */
	INIT_PGPOS_H_MON,			/* 9: INIT_PGPOS(High)*/
};

/* enums : ���ʃ��j�^�I��ԍ� */
enum MCI_OPTION_MONITOR {
	LST_LPOS1_MON = 0x80,		/* 0x80: ���b�`�ʒuLPOS1�O��l 								*/
	LST_LPOS2_MON,				/* 0x81: ���b�`�ʒuLPOS2�O��l 								*/
	SEQ_LATCH_STS = 0x84,		/* 0x84: �A�����b�`�X�e�[�^�X 								*/
/* <S1D7>:Start */
	HIGH_ORDER32_POS 		,	/* 0x85: �w�ߍ��W�n�̎w�߈ʒu(�t�B���^��)	���32Bit�f�[�^	*/
	HIGH_ORDER32_MPOS,			/* 0x86: �w�߈ʒu							���32Bit�f�[�^	*/
	HIGH_ORDER32_PERR,			/* 0x87: �ʒu�΍�							���32Bit�f�[�^	*/
	HIGH_ORDER32_APOS,			/* 0x88: �@�B���W�n�t�B�[�h�o�b�N�ʒu		���32Bit�f�[�^	*/
	HIGH_ORDER32_LPOS1,			/* 0x89: �@�B���W�n�t�B�[�h�o�b�N���b�`�ʒu	���32Bit�f�[�^	*/
	HIGH_ORDER32_LPOS2,			/* 0x8A: �@�B���W�n�t�B�[�h�o�b�N���b�`�ʒu	���32Bit�f�[�^	*/
	HIGH_ORDER32_IPOS,			/* 0x8B: �w�ߍ��W�n�w�߈ʒu(�t�B���^�O)		���32Bit�f�[�^	*/
	HIGH_ORDER32_TPOS,			/* 0x8C: �w�ߍ��W�n�ڕW�ʒu					���32Bit�f�[�^	*/
/* <S1D7>:End */
};

/* enums : I/O�r�b�g���t�ʒu */
enum MCI_SGNLBIT {
	P_OT_SGNL_BNO=0,
	N_OT_SGNL_BNO,
	DEC_SGNL_BNO,
	PA_SGNL_BNO,
	PB_SGNL_BNO,
	PC_SGNL_BNO,
	EXT1_SGNL_BNO,
	EXT2_SGNL_BNO,
	EXT3_SGNL_BNO,
	BK_SGNL_BNO,
	EMG_SGNL_BNO,
};
#define	P_OT_SGNL_MSK	0x0001
#define	N_OT_SGNL_MSK	0x0002
#define	DEC_SGNL_MSK	0x0004
#define	PA_SGNL_MSK		0x0008
#define	PB_SGNL_MSK		0x0010
#define	PC_SGNL_MSK		0x0020
#define	EXT1_SGNL_MSK	0x0040
#define	EXT2_SGNL_MSK	0x0080
#define	EXT3_SGNL_MSK	0x0100
#define	BK_SGNL_MSK		0x0200
#define	EMG_SGNL_MSK	0x0400
#define	PA_SGNL_SFT		3



#define MOTION_API_H_
#endif /* MOTION_API_H_ */




