/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ServoIf.h : Servo���䕔I/F������`															*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*						2013.02.23	K.Ozaki	IgnoreModeSW�ǉ��A�\�[�X�R�[�h������ 					*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef __SERVO_IF_H__	
#define	__SERVO_IF_H__

/****************************************************************************************************/
/*																									*/
/*	Cyclic Interface Definition 																	*/
/*																									*/
/****************************************************************************************************/
typedef struct	CMD_CTRLBITS_ {
	UCHAR	ModSel;						/* Control Mode Selection	*/
	UCHAR	SelBank;					/* Gain Selection			*/
	USHORT	CmdCtrlBit;					/* Command Control Bits		*/
	BOOL	vffFilSts;					/* VFF Moving Average Filer Status *//* <S1D0> */
	BOOL	tffFilSts;					/* TFF Moving Average Filer Status  *//* <S1D0> */
} CMD_CTRLBITS;

/*--------------------------------------------------------------------------------------------------*/
/* 	Defines of Control Mode 																		*/
/*--------------------------------------------------------------------------------------------------*/
#define POS_CTRL_MODE	(1)								/* Position Control */
#define SPD_CTRL_MODE	(2)								/* Velocity Control */
#define TRQ_CTRL_MODE	(3)								/* Torque Control */
#define NO_CTRL_MODE	(0)								/* No Control */

/*--------------------------------------------------------------------------------------------------*/
/* CMD_CTRLBITS 																					*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumCmdCtrlBits {
	ENBPCTRL_BITNO = 2,									/* P/PI�؂�ւ�			*/
	CLRPOSERR_BITNO,									/* �ʒu�΍��N���A 		*/
	CLRPOSINTG_BITNO,									/* �ʒu�ϕ��N���A		*/
	IGNOREMODESW_BITNO,									/* ���[�hSW����			*/
	TRACE_TRIGGER_BITNO = 15,							/* �l�b�g���[�N�g���K�v��			*//* <S012> */
};

#define	ENBPCTRL_BIT		(1<<ENBPCTRL_BITNO)			/* P/PI�؂�ւ�			*/
#define CLRPOSERR_BIT		(1<<CLRPOSERR_BITNO)		/* �ʒu�΍��N���A		*/
#define	CLRPOSINTG_BIT		(1<<CLRPOSINTG_BITNO)		/* �ʒu�ϕ��N���A		*/
#define IGNOREMODESW_BIT	(1<<IGNOREMODESW_BITNO)		/* ���[�hSW����			*/
#define TRACE_TRG_BIT		(1<<TRACE_TRIGGER_BITNO)	/* �l�b�g���[�N�g���K�v��			*//* <S012> */
/* <S1D0> Start */
/* <VFF_TFF_MAFIL> */
//#define	TFFFILINV_BIT		5							/* �g���N�t�B�[�h�t�H���[�h�ړ����σt�B���^�@�\�L���E����*/
//#define	VFFFILINV_BIT		6							/* ���x�t�B�[�h�t�H���[�h�ړ����σt�B���^�@�\�L���E����*/
/* <VFF_TFF_MAFIL> */
//#define	TFFFILINV_BITMSK	0x01
//#define	VFFFILINV_BITMSK	0x01
/* <S1D0> End */
#if 0
typedef struct	CMD_CTRLBITS_ {
	UINT	ModSel			:2;	/* ���䃂�[�h�I�� */
	UINT	EnbPCtrl		:1;	/* P/PI�؂�ւ� */
	UINT	ClrPosErr		:1;	/* �ʒu�΍��N���A */
	UINT	ClrPosIntg		:1;	/* �ʒu�ϕ��N���A */
	UINT	ClrVPosErr		:1;	/* ���z�ʒu�΍��ر�v�� *//* <0FJ29> */
	UINT	Reserved67		:2;	/* �\�� *//* <0FJ29> */

	UINT	SelBank			:4;	/* �Q�C���I�� */
	UINT	Reserved1215	:4;	/* �\��	*/
} CMD_CTRLBITS;
#endif

/*--------------------------------------------------------------------------------------------------*/
/* CMD_LTBITS 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define	REQ_C_PHASE_BIT		0x01		/* C�����b�`�v��		*/
#define	REQ_EXT_SIG1_BIT	0x02		/* Ext1���b�`�v��		*/
#define	REQ_EXT_SIG2_BIT	0x04		/* Ext2���b�`�v��		*/
#define	REQ_EXT_SIG3_BIT	0x08		/* Ext3���b�`�v��		*/
#define	SELENC_C_PHS_BIT	0x10		/* C�G���R�[�_�I��		*/
#define	SELENC_EXT1_BIT		0x20		/* Ext1�G���R�[�_�I��	*/
#define	SELENC_EXT2_BIT		0x40		/* Ext2�G���R�[�_�I��	*/
#define	SELENC_EXT3_BIT		0x80		/* Ext3�G���R�[�_�I��	*/

/*--------------------------------------------------------------------------------------------------*/
/* 			CMD_SEQBITS 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SERVO_ON_BITNO			0		/* �T�[�{�I��			*/
#define	SENSOR_ON_BITNO			1		/* �Z���T�I��			*/
#define	ALM_RESET_BITNO			2		/* �A���[�����Z�b�g		*/
#define	WRN_RESET_BITNO			3		/* ���[�j���O���Z�b�g	*/
#define	POL_DETECT_BITNO		4		/* ���Ɍ��o				*/
#define	BRK_RELEASE_BITNO		5		/* �u���[�L�����[�X		*/
#define	ENB_EXTPTRQLMT_BITNO	6		/* �����g���N�����L��	*/
#define	ENB_EXTNTRQLMT_BITNO	7		/* �����g���N�����L��	*/
#define	POSITIVE_OT_BITNO		8		/* �����\�t�gOT			*/
#define	NEGATIVE_OT_BITNO		9		/* �����\�t�gOT			*/
#define	ZCLAMP_BITNO			10		/* @@ Zero Clamp		*/
/* <S03D> Start */
#define	SPDD_BITNO				12		/* SPD-D */
#define	SPDA_BITNO				13		/* SPD-A */
#define	SPDB_BITNO				14		/* SPD-B */
#define	CSEL_BITNO				15		/* C-SEL */
/* <S03D> End */



#define	SERVO_ON_BIT		0x01		/* �T�[�{�I��			*/
#define	SENSOR_ON_BIT		0x02		/* �Z���T�I��			*/
#define	ALM_RESET_BIT		0x04		/* �A���[�����Z�b�g		*/
#define	WRN_RESET_BIT		0x08		/* ���[�j���O���Z�b�g	*/
#define	POL_DETECT_BIT		0x10		/* ���Ɍ��o				*/
#define	BRK_RELEASE_BIT		0x20		/* �u���[�L�����[�X		*/
#define	ENB_EXTPTRQLMT_BIT	0x40		/* �����g���N�����L��	*/
#define	ENB_EXTNTRQLMT_BIT	0x80		/* �����g���N�����L��	*/
#define	POSITIVE_OT_BIT		0x100		/* �����\�t�gOT			*/
#define	NEGATIVE_OT_BIT		0x200		/* �����\�t�gOT			*/
#define	ZEROCLAMP_BIT		0x400		/* Zero Clamp			*/
/* <S03D> Start */
#define	SPDD_BIT			0x1000		/* SPD-D */
#define	SPDA_BIT			0x2000		/* SPD-A */
#define	SPDB_BIT			0x4000		/* SPD-B */
#define	CSEL_BIT			0x8000		/* C-SEL */
/* <S03D> End */


/*--------------------------------------------------------------------------------------------------*/
/*	RSP_LTBITS 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define	C_PHASE_REQLVL_BIT	0x01		/* C�����b�`�v�����x��	*/
#define	EXT_SIG1_REQLVL_BIT	0x02		/* Ext1���b�`�v�����x��	*/
#define	EXT_SIG2_REQLVL_BIT	0x04		/* Ext2���b�`�v�����x��	*/
#define	EXT_SIG3_REQLVL_BIT	0x08		/* Ext3���b�`�v�����x��	*/
#define	C_PHASE_COMP_BIT	0x10		/* C�����b�`����		*/
#define	EXT_SIG1_COMP_BIT	0x20		/* Ext1���b�`����		*/
#define	EXT_SIG2_COMP_BIT	0x40		/* Ext2���b�`����		*/
#define	EXT_SIG3_COMP_BIT	0x80		/* Ext3���b�`����		*/

/*--------------------------------------------------------------------------------------------------*/
/*	RSP_CTRLBITS 																					*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	RSP_CTRLBITS_ {
	UCHAR	ExeMode;					/* ���䃂�[�h�I����� */
	UCHAR	dummy;						/* reserved */
	USHORT	RspCtrlBit;					/* ���X�|���X�R���g���[���r�b�g�Ǘ� */
} RSP_CTRLBITS;


enum RSP_CTRL_BITNO {
	COIN_BITNO			= 2,			/* �ʒu���ߊ���			*/
	MOTOR_MOVING_BITNO,					/* ���[�^��]			*/
	REACH_SPD_BITNO,					/* ���x��v				*/
	SPD_CLAMP_BITNO,					/* ���x������			*/
	TRQ_CLAMP_BITNO,					/* �g���N������			*/
	OP_ENABLED_BITNO,					/* �w�ߎ�t�\���		*/
	SAFETY_STOP_BITNO,					/* Safety Stop State	*/
	PERCLREDGE_BITNO,					/* @@ Perr Clear Edge 	*/
};

#define	COIN_BITMSK				0x04	/* �ʒu���ߊ���		*/
#define	MOTOR_MOVING_BITMSK		0x08	/* ���[�^��]		*/
#define	REACH_SPD_BITMSK		0x10	/* ���x��v			*/
#define	SPD_CLAMP_BITMSK		0x20	/* ���x������		*/
#define	TRQ_CLAMP_BITMSK		0x40	/* �g���N������		*/
#define	OP_ENABLED_BITMSK		0x80	/* �w�ߎ�t�\���	*/
#define SAFETY_STOP_BITMSK		0x100	/* Safety Stop State*/
#define PERCLREDGE_BITMSK		0x200	/* @@ Perr Clear Edge */


/*--------------------------------------------------------------------------------------------------*/
/*	RSP_SEQBITS 																					*/
/*--------------------------------------------------------------------------------------------------*/
enum RSP_SEQ_BITNO {
	ALM_DETECTED_BITNO = 0,				/* 0:�T�[�{�A���[���������		*/
	WRN_DETECTED_BITNO,					/* 1:�T�[�{���[�j���O�������	*/
	ALM_RESET_COMP_BITNO,				/* 2:�A���[�����Z�b�g����		*/
	WRN_RESET_COMP_BITNO,				/* 3:���[�j���O�N���A����		*/
	/*----------------------------------------------------------------------------------------------*/
	SERVO_ON_COMP_BITNO,				/* 4:�T�[�{�I������				*/
	SENS_ON_COMP_BITNO,					/* 5:�Z���T�I������				*/
	POL_DET_COMP_BITNO,					/* 6:���Ɍ��o����				*/
	BRK_RELEASING_BITNO,				/* 7:�u���[�L������				*/
	/*----------------------------------------------------------------------------------------------*/
	MAIN_POWER_ON_BITNO,				/* 8:���H�d��ON				*/
	SERVO_READY_BITNO,					/* 9:�T�[�{���f�B				*/
	MOTOR_READY_BITNO,					/* A:�ʑ����Ȃ��T�[�{���f�B	*/
};

#define	ALM_DETECTED_BITMSK		0x01	/* �T�[�{�A���[���������	*/
#define	WRN_DETECTED_BITMSK		0x02	/* �T�[�{���[�j���O�������	*/
#define	ALM_RESET_COMP_BITMSK	0x04	/* �A���[�����Z�b�g����		*/
#define	WRN_RESET_COMP_BITMSK	0x08	/* ���[�j���O�N���A����		*/
#define	SERVO_ON_COMP_BITMSK	0x10	/* �T�[�{�I������			*/
#define	SENS_ON_COMP_BITMSK		0x20	/* �Z���T�I������			*/
#define	POL_DET_COMP_BITMSK		0x40	/* ���Ɍ��o����				*/
#define	BRK_RELEASING_BITMSK	0x80	/* �u���[�L������			*/
#define	MAIN_POWER_ON_BITMSK	0x100	/* ���H�d��ON				*/
#define	SERVO_READY_BITMSK		0x200	/* �T�[�{���f�B				*/
#define	MOTOR_READY_BITMSK		0x400	/* �ʑ����Ȃ��T�[�{���f�B	*/


/* RSP_SIGNLBITS */
enum RSP_SIG_BITNO {
	N_OT_SIG_BITNO= 0,					/*  0:N-OT�M������	*/
	P_OT_SIG_BITNO,						/*  1:P-OT�M������	*/
	DEC_SIG_BITNO,						/*  2:DEC�M������	*/
	EXT1_SIG_BITNO,						/*  3:EXT1�M������	*/
	EXT2_SIG_BITNO,						/*  4:EXT2�M������	*/
	EXT3_SIG_BITNO,						/*  5:EXT3�M������	*/
	HWBB_SIG_BITNO,						/*  6:HWBB�M������	*/
	ESTP_SIG_BITNO,						/*  7:ESTP�M������	*/
	
	HWBB1_SIG_BITNO=24,					/* 24:HWBB1�M������*/
	HWBB2_SIG_BITNO,					/* 25:HWBB2�M������*/
};

#define	N_OT_SIG_BITMSK		0x01		/*  0:N-OT�M������	*/
#define	P_OT_SIG_BITMSK		0x02		/*  1:P-OT�M������	*/
#define	DEC_SIG_BITMSK		0x04		/*  2:DEC�M������	*/
#define	EXT1_SIG_BITMSK		0x08		/*  3:EXT1�M������	*/
#define	EXT2_SIG_BITMSK		0x10		/*  4:EXT2�M������	*/
#define	EXT3_SIG_BITMSK		0x20		/*  5:EXT3�M������	*/
#define	HWBB_SIG_BITMSK		0x40		/*  6:HWBB�M������	*/
#define	ESTP_SIG_BITMSK		0x80		/*  7:ESTP�M������	*/

#if 0
/* Defines of Control Mode */
#define	POS_CTRL_MODE		1			/* �ʒu���䃂�[�h	*/
#define	SPD_CTRL_MODE		2			/* ���x���䃂�[�h	*/
#define	TRQ_CTRL_MODE		3			/* �g���N���䃂�[�h	*/
#define	NO_CTRL_MODE		0			/* ���䃂�[�h����	*/
#endif

/* Defines of Extended Command Code */
enum EXTCMD_CODE {
	EXTCMD_MONSEL1 = 0x1,				/* 1: Monitor Selector 5,6 */
	EXTCMD_MONSEL2,						/* 2: Monitor Selector 7,8 */
	EXTCMD_KPGAIN,						/* 3: Position Loop Gain */
	EXTCMD_TRQLMT_P,					/* 4: Positive Torque Limit */
	EXTCMD_TRQLMT_N,					/* 5: Negative Torque Limit */
	EXTCMD_OUTSGNL						/* 6: SOn Output Signals */
};


/* Defines of Select Monitor Code */
enum MONSEL_CODE {
	MONSEL_FBSPD = 0,					/* 00: Feedback Speed */
	MONSEL_SPDREF,						/* 01: Command Speed */
	MONSEL_TRQREF,						/* 02: Command Torque */
	MONSEL_POSERR_L,					/* 03: Position Error(Low) */
	MONSEL_POSERR_H,					/* 04: Position Error(High) */
	MONSEL_PGCNT_L = 0xA,				/* 0A: PG Counter(Low) */
	MONSEL_PGCNT_H,						/* 0B: PG Counter(High) */
	MONSEL_FPGCNT_L,					/* 0C: FPG Counter(Low) */
	MONSEL_FPGCNT_H,					/* 0D: FPG Counter(High) */
	MONSEL_FBPOS_L,						/* 0E: Actual FB Position(Low) */
	MONSEL_FBPOSH,						/* 0F: Actual FB Position(High) */

	MONSEL_FBSPD_RPM,					/* 10: FB Speed = Un000 */
	MONSEL_SPDREF_RPM,					/* 11: Command Speed = Un001 */
	MONSEL_TRQREF_RAT,					/* 12: Command Torque = Un002 */
	MONSEL_ROT_PLS,						/* 13: Number of pulses from C-Phase = Un003 */
	MONSEL_ROT_DEG,						/* 14: Angle from C-Phase = Un004 */
	MONSEL_INSGNL2,						/* 15: Input Signals = Un005 */
	MONSEL_OUTSGNL2,					/* 16: Output Signals = Un006 */
	MONSEL_POSSPDREF,					/* 17: Command Position Speed = Un007 */
	MONSEL_POSERR2,						/* 18: Position Error = Un008 */
	MONSEL_LOAD_RATE,					/* 19: Accumulated load rate = Un009 */
	MONSEL_REGENE_RATE,					/* 1A: Regenarative load rate = Un00A */
	MONSEL_DBPWR_RATE,					/* 1B: DB resistance Power rate = Un00B */
	MONSEL_POSREF_PLS,					/* 1C: Input Position Pulses = Un00C */
	MONSEL_PGPLS,						/* 1D: FB Position Pulses of PG = Un00D */
	MONSEL_FPGPLS,						/* 1E: FB Position Pulses of FPG = Un00E */
	MONSEL_FPGSPD,						/* 1F: FB Speed of FPG = Un00F */

	MONSEL_TIMECNT = 0x22,				/* 22:  = Un012 */
	MONSEL_MULTI_TURN,					/* 23: Multi turn value of Sensor ON */
	MONSEL_INCR_PLS,					/* 24: Incremental puleses of Sensor ON */
	MONSEL_ABS_SCALEPOS_L,				/* 25: Absolute Linear Scale Position of Sensor ON */
	MONSEL_ABS_SCALEPOS_H,				/* 26: Absolute Linear Scale Position of Sensor ON */
	MONSEL_ENC_TEMPERATURE = 0x28,		/* 28: Encoder_temperature	*//* <S1D7> *//* <S1DE> */

	MONSEL_LTPOS_C_L = 0x30,			/* 30: Latch Position of C-Phase(Low) */
	MONSEL_LTPOS_C_H ,					/* 31: Latch Position of C-Phase(High) */
	MONSEL_LTPOS_EXT1_L ,				/* 32: Latch Position of EXT1(Low) */
	MONSEL_LTPOS_EXT1_H ,				/* 33: Latch Position of EXT1(High) */
	MONSEL_LTPOS_EXT2_L ,				/* 34: Latch Position of EXT2(Low) */
	MONSEL_LTPOS_EXT2_H ,				/* 35: Latch Position of EXT2(High) */
	MONSEL_LTPOS_EXT3_L ,				/* 36: Latch Position of EXT3(Low) */
	MONSEL_LTPOS_EXT3_H ,				/* 37: Latch Position of EXT3(High) */
	MONSEL_VPOSERR_L ,					/* 38: Virtual Position error(Low) */
	MONSEL_VPOSERR_H ,					/* 39: Virtual Position error(High) */
	MONSEL_INSGNL,						/* 3A: Input Signals Status */
	MONSEL_ALMWRN_CODE,					/* 3B: Alarm, Warning Code */
	MONSEL_INSGNL_2AXIS,				/* 3C: Input Signals Status 2-axis type *//* <S1C0> */

/* <S1D7>:start */
	MONSEL_ENVIRONMENT_MON_AMP = 0x40,	/* 40: Install Environment monitor AMP */
	MONSEL_ENVIRONMENT_MON_MOT,			/* 41: Install Environment monitor MOT */
	MONSEL_FAN_SURVIVOR,				/* 42: Fan Survivor	*/
	MONSEL_CAPA_SURVIVOR,				/* 43: Capa Survivor */
	MONSEL_RUSH_SURVIVOR,				/* 44: Rush Survivor */
	MONSEL_DB_SURVIVOR,					/* 45: DB Surivior */
	MONSEL_CONSUMED_POWER,				/* 46: Consumed Power */
	MONSEL_CONSUMPTION_ENE,				/* 47: Energy Consumption */
	MONSEL_CUMULATIVE_ENE,				/* 48: Cumulative Energy */
/*  <S1D7> */
	MONSEL_ANALOG_AD2 = 0x3C,			/* @@: Analog A/D value2 */
	MONSEL_ANALOG_AD1 = 0x3D,			/* @@: Analog A/D value1 */
	MONSEL_PERCLRLATCHPOS = 0x3E,		/* @@: Analog Perr Clear Latch Position */
	MONSEL_ANALOG_PCNTR = 0x3F,			/* @@: Analog Pulse Counter */
};



/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/* M-III InterFace */
typedef union EXTENDED_DATA_ {
	LONG	dwVal;
//	USHORT	SelMon[2]; //uniond�ł̔z��g�p�֎~
} EXTENDED_DATA;


/* Cyclic Command�\���̒�` */
typedef struct SV_CYC_CMD_ {
	CMD_CTRLBITS	CmdCtrl;	/* ����w�� */
	UINT			CmdSeqBit;	/* �V�[�P���X����w�� */
	UINT			CmdLtBit;	/* ���b�`�v�� */
	UINT			DOSignal;	/* I/O�o�͎w�� */
	LONG			Position;	/* �ʒu�w�� / -------  / --------   */
	LONG			Speed;		/* ���xFF   / ���x�w�� / ���x����   */
	LONG			Torque;		/* �g���NFF / �g���NFF / �g���N�w�� */
	LONG			SelMon1;	/* ���j�^�I��1 */
	LONG			SelMon2;	/* ���j�^�I��2 */
	LONG			SelMon3;	/* ���j�^�I��3 */
	LONG			SelMon4;	/* ���j�^�I��4 */
	LONG			SelMon5;	/* ���j�^�I��5 */
	LONG			SelMon6;	/* ���j�^�I��6 */
	LONG			SelMon7;	/* ���j�^�I��7 */
	LONG			SelMon8;	/* ���j�^�I��8 */
	EXTENDED_DATA	ExtCmd1;	/* �g���w��1 */
	EXTENDED_DATA	ExtCmd2;	/* �g���w��2 */
	EXTENDED_DATA	ExtCmd3;	/* �g���w��3 */
	EXTENDED_DATA	ExtCmd4;	/* �g���w��4 */
} SV_CYC_CMD;


/* Cyclic Response�\���̒�` */
typedef struct SV_CYC_RSP_ {
	RSP_CTRLBITS	RspCtrl;	/* ������ */
	UINT			RspSeqBit;	/* �V�[�P���X������ */
	UINT			RspLtBit;	/* ���b�`��� */
	UINT			RspSigBit;	/* ���͐M����� */
	LONG			FbPosition;	/* FB�ʒu */
	LONG			PosError;	/* �ʒu�΍� */
	LONG			Mon1;		/* ���j�^�f�[�^1 */
	LONG			Mon2;		/* ���j�^�f�[�^2 */
	LONG			Mon3;		/* ���j�^�f�[�^3 */
	LONG			Mon4;		/* ���j�^�f�[�^4 */
	LONG			Mon5;		/* ���j�^�f�[�^5 */
	LONG			Mon6;		/* ���j�^�f�[�^6 */
	LONG			Mon7;		/* ���j�^�f�[�^7 */
	LONG			Mon8;		/* ���j�^�f�[�^8 */
	LONG			SelMon1;	/* ���j�^�I��1 */
	LONG			SelMon2;	/* ���j�^�I��2 */
	LONG			SelMon3;	/* ���j�^�I��3 */
	LONG			SelMon4;	/* ���j�^�I��4 */
	LONG			SelMon5;	/* ���j�^�I��5 */
	LONG			SelMon6;	/* ���j�^�I��6 */
	LONG			SelMon7;	/* ���j�^�I��7 */
	LONG			SelMon8;	/* ���j�^�I��8 */
} SV_CYC_RSP;


typedef	struct	SV_CYC_CTRL_ {
	BOOL		CmdUpdate;		/* ��������F�R�}���h�X�V�v��	*/
	BOOL		RspUpdate;		/* ��������F���X�|���X�쐬�v��	*/

	LONG		CmdUpdateCycle;	/* �ʒu�w�ߍX�V����[ScanB�J�E���^]	*/

	ULONG		ExtCmdSel1;		/* Extended Command1 No. */
	ULONG		ExtCmdSel2;		/* Extended Command2 No. */
	ULONG		ExtCmdSel3;		/* Extended Command3 No. */
	ULONG		ExtCmdSel4;		/* Extended Command4 No. */
} SV_CYC_CTRL;


/* OPTION - SERVO I/F �\���̒�` */
typedef struct SV_OPTION_IF_ {
	SV_CYC_CTRL		CycCtrl;		/* Extended Command 1�`4 */
	SV_CYC_CMD		CycCmd;			/* Cyclic Command */
	SV_CYC_RSP		CycRsp;			/* Cyclic Response */
} SV_OPTION_IF;


#endif 																		/* SERVO_IF_H_ */
/***************************************** end of file **********************************************/
