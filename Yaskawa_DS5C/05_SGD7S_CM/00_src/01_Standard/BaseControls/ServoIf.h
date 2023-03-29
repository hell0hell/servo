/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ServoIf.h : Servo制御部I/F処理定義															*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*						2013.02.23	K.Ozaki	IgnoreModeSW追加、ソースコード整理中 					*/
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
	ENBPCTRL_BITNO = 2,									/* P/PI切り替え			*/
	CLRPOSERR_BITNO,									/* 位置偏差クリア 		*/
	CLRPOSINTG_BITNO,									/* 位置積分クリア		*/
	IGNOREMODESW_BITNO,									/* モードSW無効			*/
	TRACE_TRIGGER_BITNO = 15,							/* ネットワークトリガ要求			*//* <S012> */
};

#define	ENBPCTRL_BIT		(1<<ENBPCTRL_BITNO)			/* P/PI切り替え			*/
#define CLRPOSERR_BIT		(1<<CLRPOSERR_BITNO)		/* 位置偏差クリア		*/
#define	CLRPOSINTG_BIT		(1<<CLRPOSINTG_BITNO)		/* 位置積分クリア		*/
#define IGNOREMODESW_BIT	(1<<IGNOREMODESW_BITNO)		/* モードSW無効			*/
#define TRACE_TRG_BIT		(1<<TRACE_TRIGGER_BITNO)	/* ネットワークトリガ要求			*//* <S012> */
/* <S1D0> Start */
/* <VFF_TFF_MAFIL> */
//#define	TFFFILINV_BIT		5							/* トルクフィードフォワード移動平均フィルタ機能有効・無効*/
//#define	VFFFILINV_BIT		6							/* 速度フィードフォワード移動平均フィルタ機能有効・無効*/
/* <VFF_TFF_MAFIL> */
//#define	TFFFILINV_BITMSK	0x01
//#define	VFFFILINV_BITMSK	0x01
/* <S1D0> End */
#if 0
typedef struct	CMD_CTRLBITS_ {
	UINT	ModSel			:2;	/* 制御モード選択 */
	UINT	EnbPCtrl		:1;	/* P/PI切り替え */
	UINT	ClrPosErr		:1;	/* 位置偏差クリア */
	UINT	ClrPosIntg		:1;	/* 位置積分クリア */
	UINT	ClrVPosErr		:1;	/* 仮想位置偏差ｸﾘｱ要求 *//* <0FJ29> */
	UINT	Reserved67		:2;	/* 予約 *//* <0FJ29> */

	UINT	SelBank			:4;	/* ゲイン選択 */
	UINT	Reserved1215	:4;	/* 予約	*/
} CMD_CTRLBITS;
#endif

/*--------------------------------------------------------------------------------------------------*/
/* CMD_LTBITS 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define	REQ_C_PHASE_BIT		0x01		/* C相ラッチ要求		*/
#define	REQ_EXT_SIG1_BIT	0x02		/* Ext1ラッチ要求		*/
#define	REQ_EXT_SIG2_BIT	0x04		/* Ext2ラッチ要求		*/
#define	REQ_EXT_SIG3_BIT	0x08		/* Ext3ラッチ要求		*/
#define	SELENC_C_PHS_BIT	0x10		/* Cエンコーダ選択		*/
#define	SELENC_EXT1_BIT		0x20		/* Ext1エンコーダ選択	*/
#define	SELENC_EXT2_BIT		0x40		/* Ext2エンコーダ選択	*/
#define	SELENC_EXT3_BIT		0x80		/* Ext3エンコーダ選択	*/

/*--------------------------------------------------------------------------------------------------*/
/* 			CMD_SEQBITS 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SERVO_ON_BITNO			0		/* サーボオン			*/
#define	SENSOR_ON_BITNO			1		/* センサオン			*/
#define	ALM_RESET_BITNO			2		/* アラームリセット		*/
#define	WRN_RESET_BITNO			3		/* ワーニングリセット	*/
#define	POL_DETECT_BITNO		4		/* 磁極検出				*/
#define	BRK_RELEASE_BITNO		5		/* ブレーキリリース		*/
#define	ENB_EXTPTRQLMT_BITNO	6		/* 正側トルク制限有効	*/
#define	ENB_EXTNTRQLMT_BITNO	7		/* 負側トルク制限有効	*/
#define	POSITIVE_OT_BITNO		8		/* 正側ソフトOT			*/
#define	NEGATIVE_OT_BITNO		9		/* 負側ソフトOT			*/
#define	ZCLAMP_BITNO			10		/* @@ Zero Clamp		*/
/* <S03D> Start */
#define	SPDD_BITNO				12		/* SPD-D */
#define	SPDA_BITNO				13		/* SPD-A */
#define	SPDB_BITNO				14		/* SPD-B */
#define	CSEL_BITNO				15		/* C-SEL */
/* <S03D> End */



#define	SERVO_ON_BIT		0x01		/* サーボオン			*/
#define	SENSOR_ON_BIT		0x02		/* センサオン			*/
#define	ALM_RESET_BIT		0x04		/* アラームリセット		*/
#define	WRN_RESET_BIT		0x08		/* ワーニングリセット	*/
#define	POL_DETECT_BIT		0x10		/* 磁極検出				*/
#define	BRK_RELEASE_BIT		0x20		/* ブレーキリリース		*/
#define	ENB_EXTPTRQLMT_BIT	0x40		/* 正側トルク制限有効	*/
#define	ENB_EXTNTRQLMT_BIT	0x80		/* 負側トルク制限有効	*/
#define	POSITIVE_OT_BIT		0x100		/* 正側ソフトOT			*/
#define	NEGATIVE_OT_BIT		0x200		/* 負側ソフトOT			*/
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
#define	C_PHASE_REQLVL_BIT	0x01		/* C相ラッチ要求レベル	*/
#define	EXT_SIG1_REQLVL_BIT	0x02		/* Ext1ラッチ要求レベル	*/
#define	EXT_SIG2_REQLVL_BIT	0x04		/* Ext2ラッチ要求レベル	*/
#define	EXT_SIG3_REQLVL_BIT	0x08		/* Ext3ラッチ要求レベル	*/
#define	C_PHASE_COMP_BIT	0x10		/* C相ラッチ完了		*/
#define	EXT_SIG1_COMP_BIT	0x20		/* Ext1ラッチ完了		*/
#define	EXT_SIG2_COMP_BIT	0x40		/* Ext2ラッチ完了		*/
#define	EXT_SIG3_COMP_BIT	0x80		/* Ext3ラッチ完了		*/

/*--------------------------------------------------------------------------------------------------*/
/*	RSP_CTRLBITS 																					*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	RSP_CTRLBITS_ {
	UCHAR	ExeMode;					/* 制御モード選択状態 */
	UCHAR	dummy;						/* reserved */
	USHORT	RspCtrlBit;					/* レスポンスコントロールビット管理 */
} RSP_CTRLBITS;


enum RSP_CTRL_BITNO {
	COIN_BITNO			= 2,			/* 位置決め完了			*/
	MOTOR_MOVING_BITNO,					/* モータ回転			*/
	REACH_SPD_BITNO,					/* 速度一致				*/
	SPD_CLAMP_BITNO,					/* 速度制限中			*/
	TRQ_CLAMP_BITNO,					/* トルク制限中			*/
	OP_ENABLED_BITNO,					/* 指令受付可能状態		*/
	SAFETY_STOP_BITNO,					/* Safety Stop State	*/
	PERCLREDGE_BITNO,					/* @@ Perr Clear Edge 	*/
};

#define	COIN_BITMSK				0x04	/* 位置決め完了		*/
#define	MOTOR_MOVING_BITMSK		0x08	/* モータ回転		*/
#define	REACH_SPD_BITMSK		0x10	/* 速度一致			*/
#define	SPD_CLAMP_BITMSK		0x20	/* 速度制限中		*/
#define	TRQ_CLAMP_BITMSK		0x40	/* トルク制限中		*/
#define	OP_ENABLED_BITMSK		0x80	/* 指令受付可能状態	*/
#define SAFETY_STOP_BITMSK		0x100	/* Safety Stop State*/
#define PERCLREDGE_BITMSK		0x200	/* @@ Perr Clear Edge */


/*--------------------------------------------------------------------------------------------------*/
/*	RSP_SEQBITS 																					*/
/*--------------------------------------------------------------------------------------------------*/
enum RSP_SEQ_BITNO {
	ALM_DETECTED_BITNO = 0,				/* 0:サーボアラーム発生状態		*/
	WRN_DETECTED_BITNO,					/* 1:サーボワーニング発生状態	*/
	ALM_RESET_COMP_BITNO,				/* 2:アラームリセット完了		*/
	WRN_RESET_COMP_BITNO,				/* 3:ワーニングクリア完了		*/
	/*----------------------------------------------------------------------------------------------*/
	SERVO_ON_COMP_BITNO,				/* 4:サーボオン完了				*/
	SENS_ON_COMP_BITNO,					/* 5:センサオン完了				*/
	POL_DET_COMP_BITNO,					/* 6:磁極検出完了				*/
	BRK_RELEASING_BITNO,				/* 7:ブレーキ解除中				*/
	/*----------------------------------------------------------------------------------------------*/
	MAIN_POWER_ON_BITNO,				/* 8:主回路電源ON				*/
	SERVO_READY_BITNO,					/* 9:サーボレディ				*/
	MOTOR_READY_BITNO,					/* A:位相情報なしサーボレディ	*/
};

#define	ALM_DETECTED_BITMSK		0x01	/* サーボアラーム発生状態	*/
#define	WRN_DETECTED_BITMSK		0x02	/* サーボワーニング発生状態	*/
#define	ALM_RESET_COMP_BITMSK	0x04	/* アラームリセット完了		*/
#define	WRN_RESET_COMP_BITMSK	0x08	/* ワーニングクリア完了		*/
#define	SERVO_ON_COMP_BITMSK	0x10	/* サーボオン完了			*/
#define	SENS_ON_COMP_BITMSK		0x20	/* センサオン完了			*/
#define	POL_DET_COMP_BITMSK		0x40	/* 磁極検出完了				*/
#define	BRK_RELEASING_BITMSK	0x80	/* ブレーキ解除中			*/
#define	MAIN_POWER_ON_BITMSK	0x100	/* 主回路電源ON				*/
#define	SERVO_READY_BITMSK		0x200	/* サーボレディ				*/
#define	MOTOR_READY_BITMSK		0x400	/* 位相情報なしサーボレディ	*/


/* RSP_SIGNLBITS */
enum RSP_SIG_BITNO {
	N_OT_SIG_BITNO= 0,					/*  0:N-OT信号入力	*/
	P_OT_SIG_BITNO,						/*  1:P-OT信号入力	*/
	DEC_SIG_BITNO,						/*  2:DEC信号入力	*/
	EXT1_SIG_BITNO,						/*  3:EXT1信号入力	*/
	EXT2_SIG_BITNO,						/*  4:EXT2信号入力	*/
	EXT3_SIG_BITNO,						/*  5:EXT3信号入力	*/
	HWBB_SIG_BITNO,						/*  6:HWBB信号入力	*/
	ESTP_SIG_BITNO,						/*  7:ESTP信号入力	*/
	
	HWBB1_SIG_BITNO=24,					/* 24:HWBB1信号入力*/
	HWBB2_SIG_BITNO,					/* 25:HWBB2信号入力*/
};

#define	N_OT_SIG_BITMSK		0x01		/*  0:N-OT信号入力	*/
#define	P_OT_SIG_BITMSK		0x02		/*  1:P-OT信号入力	*/
#define	DEC_SIG_BITMSK		0x04		/*  2:DEC信号入力	*/
#define	EXT1_SIG_BITMSK		0x08		/*  3:EXT1信号入力	*/
#define	EXT2_SIG_BITMSK		0x10		/*  4:EXT2信号入力	*/
#define	EXT3_SIG_BITMSK		0x20		/*  5:EXT3信号入力	*/
#define	HWBB_SIG_BITMSK		0x40		/*  6:HWBB信号入力	*/
#define	ESTP_SIG_BITMSK		0x80		/*  7:ESTP信号入力	*/

#if 0
/* Defines of Control Mode */
#define	POS_CTRL_MODE		1			/* 位置制御モード	*/
#define	SPD_CTRL_MODE		2			/* 速度制御モード	*/
#define	TRQ_CTRL_MODE		3			/* トルク制御モード	*/
#define	NO_CTRL_MODE		0			/* 制御モード無効	*/
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
//	USHORT	SelMon[2]; //uniondでの配列使用禁止
} EXTENDED_DATA;


/* Cyclic Command構造体定義 */
typedef struct SV_CYC_CMD_ {
	CMD_CTRLBITS	CmdCtrl;	/* 制御指令 */
	UINT			CmdSeqBit;	/* シーケンス制御指令 */
	UINT			CmdLtBit;	/* ラッチ要求 */
	UINT			DOSignal;	/* I/O出力指令 */
	LONG			Position;	/* 位置指令 / -------  / --------   */
	LONG			Speed;		/* 速度FF   / 速度指令 / 速度制限   */
	LONG			Torque;		/* トルクFF / トルクFF / トルク指令 */
	LONG			SelMon1;	/* モニタ選択1 */
	LONG			SelMon2;	/* モニタ選択2 */
	LONG			SelMon3;	/* モニタ選択3 */
	LONG			SelMon4;	/* モニタ選択4 */
	LONG			SelMon5;	/* モニタ選択5 */
	LONG			SelMon6;	/* モニタ選択6 */
	LONG			SelMon7;	/* モニタ選択7 */
	LONG			SelMon8;	/* モニタ選択8 */
	EXTENDED_DATA	ExtCmd1;	/* 拡張指令1 */
	EXTENDED_DATA	ExtCmd2;	/* 拡張指令2 */
	EXTENDED_DATA	ExtCmd3;	/* 拡張指令3 */
	EXTENDED_DATA	ExtCmd4;	/* 拡張指令4 */
} SV_CYC_CMD;


/* Cyclic Response構造体定義 */
typedef struct SV_CYC_RSP_ {
	RSP_CTRLBITS	RspCtrl;	/* 制御状態 */
	UINT			RspSeqBit;	/* シーケンス制御状態 */
	UINT			RspLtBit;	/* ラッチ状態 */
	UINT			RspSigBit;	/* 入力信号状態 */
	LONG			FbPosition;	/* FB位置 */
	LONG			PosError;	/* 位置偏差 */
	LONG			Mon1;		/* モニタデータ1 */
	LONG			Mon2;		/* モニタデータ2 */
	LONG			Mon3;		/* モニタデータ3 */
	LONG			Mon4;		/* モニタデータ4 */
	LONG			Mon5;		/* モニタデータ5 */
	LONG			Mon6;		/* モニタデータ6 */
	LONG			Mon7;		/* モニタデータ7 */
	LONG			Mon8;		/* モニタデータ8 */
	LONG			SelMon1;	/* モニタ選択1 */
	LONG			SelMon2;	/* モニタ選択2 */
	LONG			SelMon3;	/* モニタ選択3 */
	LONG			SelMon4;	/* モニタ選択4 */
	LONG			SelMon5;	/* モニタ選択5 */
	LONG			SelMon6;	/* モニタ選択6 */
	LONG			SelMon7;	/* モニタ選択7 */
	LONG			SelMon8;	/* モニタ選択8 */
} SV_CYC_RSP;


typedef	struct	SV_CYC_CTRL_ {
	BOOL		CmdUpdate;		/* 同期制御：コマンド更新要求	*/
	BOOL		RspUpdate;		/* 同期制御：レスポンス作成要求	*/

	LONG		CmdUpdateCycle;	/* 位置指令更新周期[ScanBカウンタ]	*/

	ULONG		ExtCmdSel1;		/* Extended Command1 No. */
	ULONG		ExtCmdSel2;		/* Extended Command2 No. */
	ULONG		ExtCmdSel3;		/* Extended Command3 No. */
	ULONG		ExtCmdSel4;		/* Extended Command4 No. */
} SV_CYC_CTRL;


/* OPTION - SERVO I/F 構造体定義 */
typedef struct SV_OPTION_IF_ {
	SV_CYC_CTRL		CycCtrl;		/* Extended Command 1～4 */
	SV_CYC_CMD		CycCmd;			/* Cyclic Command */
	SV_CYC_RSP		CycRsp;			/* Cyclic Response */
} SV_OPTION_IF;


#endif 																		/* SERVO_IF_H_ */
/***************************************** end of file **********************************************/
