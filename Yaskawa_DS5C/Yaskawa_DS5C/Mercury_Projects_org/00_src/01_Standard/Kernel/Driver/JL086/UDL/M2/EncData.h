/****************************************************************************************************/
/*																									*/
/*																									*/
/*		EncData.h : エンコーダデータ構造体定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef ENC_DATA_H


#include "MLib.h"
#include "JL086ServoIpHw.h"


/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダ＆モータID情報定義																*/
/*--------------------------------------------------------------------------------------------------*/
#define MOT_ENC_MODEL_STR	24				/* Model												*//* <S145> */
#define MOT_ENC_DATE_STR	6				/* Date													*/
#define MOT_ENC_SNO_STR		22				/* Serial No.											*/

/****************************************************************************************************/
/*		リニアモータ用定義																			*/
/****************************************************************************************************/
#define	LMX_MPOS_SFT		12				/* 相位置計算用シフト数									*/
#define	HPOS_TO_XPOS		20				/* 32bit - LMX_MPOS_SFT									*/
#define	FSX_FPOS_SFT		12


/*--------------------------------------------------------------------------------------------------*/
/*		Encoder Type Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	MENCTYPE_SERIAL			0x00		/* シリアルエンコーダ									*/
#define	MENCTYPE_PULSE			0x10		/* パルスエンコーダ										*/
#define	MENCTYPE_ANALOG 		0x20		/* アナログエンコーダ									*/
#define	MENCTYPE_ENCLESS		0xFF		/* エンコーダレス										*/

#define	MENC_INCRE_TYPE			0x00		/* アブソをインクレとして使用							*/
#define	MENC_MABSO_TYPE			0x10		/* 多回転アブソエンコーダ								*/
#define	MENC_SABSO_TYPE			0x20		/* １回転アブソエンコーダ								*/

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Converter Type Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SCONVTYPE_NONE			0x00		/* シリアルコンバータなし								*/
#define	SCONVTYPE_STANDARD_INC	0x01		/* 標準インクレタイプ									*/
#define SCONVTYPE_MITUTOYO_ABS	0x02		/* ミツトヨ製アブソタイプ								*/
#define	SCONVTYPE_ENDAT_ABS		0x03		/* EnDatタイプ											*/
#define	SCONVTYPE_SONY_INC		0x04		/* SMS製インクレタイプ									*/
#define	SCONVTYPE_SONY_ABS		0x05		/* SMS製アブソタイプ									*/

/*--------------------------------------------------------------------------------------------------*/
#define SCALE_EXP_PM			4			/* リニアスケールピッチ指数[pm]							*/
#define SCALE_EXP_UM			1			/* リニアスケールピッチ指数[um]							*/

/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダＩＦ定義																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダコマンド定義																*/
/*--------------------------------------------------------------------------------------------------*/
#define		SPG_NOP			0x0000					/* Nop   Command								*/
#define		SPG_BREAK		0x0001					/* Break Command								*/
#define		SPG_ORGCLR		0x0007					/* Origin Clear Command							*//* <V272> */
#define		SPG_PREAD		0x0008					/* Parameter Read Command						*/
#define		SPG_PWRITE		0x0010					/* Parameter Write Command						*//* <V209> */
#define		SPG_ECHO		0x00FF					/* Echo  Command								*/
/*--------------------------------------------------------------------------------------------------*/
#define		SPG_RDADR		0x0041					/* Read  Address Command						*/
#define		SPG_WRADR		0x0042					/* Write Address Command						*/
#define		SPG_OPENKEY		0x594F					/* Open  Key Command							*/
#define		SPG_CLOSEKEY	0x004F					/* Close Key Command							*/
/*--------------------------------------------------------------------------------------------------*/
#define		SPG_MTRST		0x0004					/* MultiTurn Reset Command						*/
#define		SPG_MTLMTCHG	0x0006					/* MultiTurn Change Command						*/
#define		SPG_PMODEOFF	0x00F0					/* Protect Mode Off Command						*/
#define		SPG_ZEROSET		0x0012					/* Zero Set Command					<V252>		*/
//#define		KPX_CDAT_SENC_ESYNC				0x0000	/* CH0外部同期有効、CH1外部同期無効 */
#define		KPX_CDAT_SENC_ESYNC		0x0002		/* CH0外部同期有効、CH1外部同期無効					*/
//#define	KPX_CDAT_SENC_ESYNC		0x0003		/* CH0,CH1外部同期無効 *///kira

/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダＩＦ定義																	*/
/*																									*/
/****************************************************************************************************/
//****************************************************************************************************
/*		Response Data Buffer																		*/
//****************************************************************************************************
/* Header Structure */
typedef struct _ENC_RSP_HEADER
{
	UINT	CmdCode		: 8;
	UINT	Status		: 8;
} ENC_RSP_HEADER;

/* Motor Position Data Low Order */
typedef union _MOT_POS_LOW
{
	struct {
		unsigned int InvalidData	: 4;
		unsigned int PositionL		: 4;
	} Data;
	unsigned char PositionL;
} MOT_POS_LOW;

/* Incre Comp. Value and Pole Center */
typedef union _INCRE_UNIQUE_DATA
{
	struct {
		unsigned int Signal_Z		: 1;
		unsigned int Pole_W			: 1;
		unsigned int Pole_V			: 1;
		unsigned int Pole_U			: 1;
		unsigned int IncreComp		: 12;
	} Data;
	unsigned char IncreData;
} INCRE_UNIQUE_DATA;

/* NOP, BREAK, RESET, ECHO, INIT */
typedef union _ENC_RSP_NOP
{
	struct {
		ENC_RSP_HEADER		header;
		UCHAR				Echo[14];
	} cdata;
	struct {
		USHORT				header;
		USHORT				Echo[7];
	} sdata;
	ULONG		ldata[4];
} ENC_RSP_NOP;

/* OPEN_KEY */
typedef union _ENC_RSP_OPENKEY
{
	struct {
		ENC_RSP_HEADER		header;
		UCHAR				Echo[2];
		UCHAR				EncVersion;
		UCHAR				dummy[11];
	} cdata;
	struct {
		USHORT				header;
		USHORT				Echo;
		USHORT				dummy[6];
	} sdata;
	ULONG		ldata[4];
} ENC_RSP_OPENKEY;

/* MUL_LIMIT */
typedef union _ENC_RSP_MTLIMIT
{
	struct {
		ENC_RSP_HEADER		header;
		UCHAR				MultiTurnLimitL;
		UCHAR				MultiTurnLimitH;
		UCHAR				CntrReloadData1;
		UCHAR				CntrReloadData2;
		UCHAR				CntrReloadData3;
		UCHAR				CntrReloadData4;
		UCHAR				PreChangeMTDataL;
		UCHAR				PreChangeMTDataH;
		UCHAR				PostChangeMTDataL;
		UCHAR				PostChangeMTDataH;
		UCHAR				dummy[4];
	} cdata;
	struct {
		USHORT				header;
		USHORT				MultiTurnLimit;
		USHORT				CntrReloadData12;
		USHORT				CntrReloadData34;
		USHORT				PreChangeMTData;
		USHORT				PostChangeMTData;
		USHORT				dummy[2];
	} sdata;
	ULONG		ldata[4];
} ENC_RSP_MTLIMIT;

/* READ_PRM, WRITE_PRM */
typedef union _ENC_RSP_PRMRD
{
	struct {
		ENC_RSP_HEADER		header;
		UCHAR				PrmAddress;
		UCHAR				ReadSize;
		UCHAR				ReadData[12];
	} cdata;
	struct {
		USHORT				header;
		USHORT				Operand;
		USHORT				PrmData[6];
	} sdata;
	ULONG		ldata[4];
} ENC_RSP_PRMRD;

/* P1Mode */
typedef union _ENC_RSP_P1MODE
{
	struct {
		UCHAR				dummy[2];
		UCHAR				AlmCode;
		UCHAR				Data1;
		UCHAR				Data2;
		UCHAR				TimeBaseTimerL;
		UCHAR				TimeBaseTimerH;
		UCHAR				LastChangePosTimeL;
		UCHAR				LastChangePosTimeH;
//		MOT_POS_LOW			PositionL;
		UCHAR				PositionL;
		UCHAR				PositionM;
		UCHAR				PositionH;
//		INCRE_UNIQUE_DATA	IncreData;
		UCHAR				IncreData;
	} cdata;
	USHORT				sdata[8];
	ULONG				ldata[4];
} ENC_RSP_P1MODE;


//****************************************************************************************************
/*		Command Data Buffer																			*/
//****************************************************************************************************
/* Header Structure */
typedef struct _ENC_CMD_HEADER
{
	UINT	CmdCode		: 8;
	UINT	InvalidData	: 8;
} ENC_CMD_HEADER;

/* NOP, BREAK, RESET, ECHO, INIT */
typedef union _ENC_RESP_NOP
{
	struct {
		ENC_CMD_HEADER		header;
		UCHAR				TestPattern[14];
	} cdata;
	struct {
		USHORT				CmdCode;
		USHORT				TestPattern[7];
	} sdata;
	ULONG		ldata[4];
} ENC_CMD_NOP;

/* OPEN_KEY */
typedef union _ENC_CMD_OPENKEY
{
	struct {
		UCHAR		CmdCode;
		UCHAR		KeyWord[15];
	} cdata;
	struct {
		USHORT		CmdCode;
		USHORT		KeyWord[7];
	} sdata;
	ULONG		ldata[4];
} ENC_CMD_OPENKEY;

/* MUL_LIMIT */
typedef union _ENC_CMD_MTLIMIT
{
	struct {
		ENC_CMD_HEADER		header;
		UCHAR				MultiTurnLimitL;
		UCHAR				MultiTurnLimitH;
		UCHAR				dummy[12];
	} cdata;
	struct {
		USHORT				header;
		USHORT				MultiTurnLimit;
		USHORT				dummy[6];
	} sdata;
	ULONG		ldata[4];
} ENC_CMD_MTLIMIT;

/* READ_PRM, WRITE_PRM */
typedef union _ENC_CMD_PRMRD
{
	struct {
		ENC_CMD_HEADER		header;
		UCHAR				PrmAddress;
		UCHAR				ReadSize;
		UCHAR				dummy[12];
	} cdata;
	struct {
		USHORT				header;
		USHORT				Operand;
		USHORT				PrmData[6];
	} sdata;
	ULONG		ldata[4];
} ENC_CMD_PRMRD;


//****************************************************************************************************
/*		シリアルエンコーダIF用データバッファ構造定義												*/
//****************************************************************************************************
typedef union _ENC_DATA_BUF
{ /* for Encoder communication */
	ENC_RSP_NOP			EncRspNormal;
	ENC_RSP_OPENKEY		EncRspKeyCntrl;
	ENC_RSP_MTLIMIT		EncRspMTLimit;
	ENC_RSP_PRMRD		EncRspPrmAccess;
	ENC_RSP_P1MODE		EncRspP1Mode;
	ENC_CMD_NOP			EncCmdNormal;
	ENC_CMD_OPENKEY		EncCmdKeyCntrl;
	ENC_CMD_MTLIMIT		EncDmcMTLimit;
	ENC_CMD_PRMRD		EncCmdPrmAccess;
	USHORT				UsBuf[8];
	ULONG				UlBuf[8];
} ENC_DATA_BUF;


//****************************************************************************************************
/*		シリアルエンコーダIF定義																	*/
//****************************************************************************************************
typedef	struct SENCX {
	ASIC_HW_IF_STR	*AsicHwReg;				/* Asic H/W レジスタアドレス							*/
	VUSHORT			*TxBuf;					/* 送信バッファアドレス									*/
	VUSHORT			*RxBuf;					/* 受信バッファアドレス									*/
	VUSHORT			*RxFlg;					/* 受信完了フラグアドレス								*/
	VUSHORT			*TxStart;				/* 送信開始用書き込みレジスタアドレス					*/
	VUSHORT			TxCmd;					/* 送信開始コマンドデータ(ESYNC0/ESYNC1)				*/
	UINT			ch;						/* 接続Channel											*/
	ENC_DATA_BUF	TxData;					/* 送信データ格納用バッファ								*/
	ENC_DATA_BUF	RxData;					/* 受信データ格納用バッファ								*/
} SENCX;

/****************************************************************************************************/
/*		エンコーダ＆モータID情報定義																*/
/****************************************************************************************************/
typedef struct IDINFO {
	UCHAR	Model[MOT_ENC_MODEL_STR];		/* Model[12]											*/
	UCHAR	Date[MOT_ENC_DATE_STR];			/* Date[3]												*/
	UCHAR	SNo[MOT_ENC_SNO_STR];			/* Serial No.[11]										*/
} IDINFO;


/****************************************************************************************************/
/*		エンコーダパラメータ定義																	*/
/****************************************************************************************************/
typedef struct MENCPRM {
	DBYTEX	typm_e;					/* PnF00 : モータ形式(8bit), 入力電圧(4bit), エンコーダタイプ(4bit)	*/
	USHORT	encver;					/* PnF01 : エンコーダソフトバージョン								*/
	USHORT	motw;					/* PnF02 : モータ容量												*/
	DBYTEX	bit_dp;					/* PnF03 : エンコーダビット数, 位置データ小数点位置					*/
	USHORT	limmlt;					/* PnF04 : マルチターンリミット										*/
	DBYTEX	vrat_max;				/* PnF05 : 定格速度, 最大速度										*/
	DBYTEX	oslv_pol;				/* PnF06 : ＯＳ検出レベル, ポール数									*/
	USHORT	rattrq;					/* PnF07 : 定格トルク												*/
	USHORT	maxtrq;					/* PnF08 : 最大トルク												*/
	USHORT	irat;					/* PnF09 : 定格電流													*/
	USHORT	imax;					/* PnF0A : 瞬時最大電流												*/
	USHORT	emfcmp;					/* PnF0B : ＥＭＦ定数												*/
	USHORT	jmot;					/* PnF0C : ロータイナーシャ											*/
	USHORT	motr;					/* PnF0D : 電機子巻線抵抗											*/
	USHORT	motl;					/* PnF0E : 電機子インダクタンス										*/
	USHORT	bastrq;					/* PnF0F : 過負荷検出ベース電流										*/
	/*------------------------------------------------------------------------------------------------------*/
	USHORT	midtrq;					/* PnF10 : 過負荷検出中間電流										*/
	USHORT	midolt;					/* PnF11 : 過負荷検出中間時間										*/
	USHORT	mtrq2;					/* PnF12 : 過負荷検出中間電流２										*/
	USHORT	mtolt2;					/* PnF13 : 過負荷検出中間時間２										*/
	DBYTEX	phscmpM1;				/* PnF14 : 位相補償１(補償値,開始速度)								*/
	USHORT	ppitch;					/* PnF15 : ポールピッチ												*/
	USHORT	motlq0;					/* PnF16 : q軸インダクタンスLq0										*/
	USHORT	motlq1;					/* PnF17 : q軸インダクタンスLq1										*/
	DBYTEX	exp_trat_jmot;			/* PnF18 : 下位：定格トルク指数、上位：ロータイナーシャ指数			*/
	DBYTEX	exp_motw_spd;			/* PnF19 : 下位：定格出力指数、上位：回転数指数						*/
	DBYTEX	BitnoDpoint;			/* PnF1A : フルクエンコーダビット数, 位置データ小数点位置			*/
	USHORT	poleoffset;				/* PnF1B : ポールセンサオフセット量									*/
	USHORT	motresfrq;				/* PnF1C : モータ共振抑制用ノッチフィルタ周波数						*/
	USHORT	phscmpM2;				/* PnF1D : 位相角補正値												*/
	USHORT	flg_wf;					/* PnF1E : フラグ													*/
} MENCPRM;

/****************************************************************************************************/
/*		エンコーダ初期パラメータ定義																*/
/****************************************************************************************************/
typedef struct MENC_REF_PRM {
//	USHORT	b_prm2;					/* Pn002 : 機能選択応用スイッチ２								*/
//	USHORT	b_prm80;				/* Pn080 : 機能選択応用スイッチ８０								*/
//	USHORT	mtlmt;					/* Pn205 : マルチターンリミット設定									*/
//	ULONG	scalepitch2l;			/* Pn282 : リニアスケールピッチ									*/
	USHORT	syssw3;					/* PnE0D : システムスイッチ３									*/
	USHORT	scnvorgoffset;			/* PnE8C : シリアル変換ユニット原点オフセット					*/
	USHORT	phsofst;				/* PnE8D : 電流零位相位置										*/

	KSGAIN	MaxTrq;					/* Max.Torque						[Nm]		[N]				*/
	KSGAIN	Kfbpls;					/* FB Pulse Factor (Menc/Fenc)		[rad/p]		[m/p]			*/
	KSGAIN	Jmot;					/* Motor Rotor Inertia				[kg*m^2]	[kg]			*/
} MENC_REF_PRM;

/****************************************************************************************************/
/*		変化時刻タイマ演算実行用定義																*/
/****************************************************************************************************/
typedef struct ENCVFBTIMBASE {
	LONG	fencspeed;										/* 負荷速度				[2^24/OvrSpd]	*/
	USHORT	BaseTimeInterval;								/* ベースタイマ変化時間間隔				*/
	USHORT	BaseTimer;										/* ベースタイマ							*/
	USHORT	LastBaseTimer;									/* 前回ベースタイマ						*/
	UCHAR	Modflag;										/* ベースタイマ速度演算実行フラグ		*/
	UCHAR	LastModflag;									/* 前回ベースタイマ速度演算実行フラグ	*/
} ENCVFBTIMBASE;

/****************************************************************************************************/
/*		エンコーダ関連初期化用パラメータ定義														*/
/****************************************************************************************************/
typedef struct MENC_INIT_PRM {
	MENCPRM	*MencP;					/* PnF** : エンコーダパラメータ定義へのポインタ					*/
	LONG	scalepitch2l;			/* Pn282 : リニアスケールピッチ									*/
	USHORT	b_prm2;					/* Pn002 : 機能選択応用スイッチ2								*/
	USHORT	b_prm80;				/* Pn080 : 機能選択応用スイッチ80								*/
	USHORT	mtlmt;					/* Pn205 : マルチターンリミット設定								*/
	DBYTEX	spgfail;				/* PnE1E : 下位:S-PG受信失敗許容回数, 上位:FS-PG受信失敗許容回数*/
} MENC_INIT_PRM;

/****************************************************************************************************/
/*		エンコーダIFデータ定義																		*/
/****************************************************************************************************/
typedef	struct MENCV {
	//パラメータ領域
//----------------------------------------------------------------------------------------------------
	IDINFO	MotInfo;						/* Motor Information									*/
	IDINFO	EncInfo;						/* Motor Encoder Information							*/
//----------------------------------------------------------------------------------------------------
	SENCX	EncIf;							/* Serial Encoder Interface Definitions					*/
//----------------------------------------------------------------------------------------------------
	LONG	AxisMotType;					/* Motor Type Rotary or Linear							*/
//----------------------------------------------------------------------------------------------------
	BOOL	EncCom8Mbps;					/* Encoder ComSpeed 8Mbps								*/
	BOOL	AbsoEncoder;					/* Absolute Encoder										*/
	BOOL	Sigma3Encoder;					/* Sigma-3 Encoder										*/
	BOOL	LowSpdMotor;					/* Low Speed Motor										*/
	BOOL	RevAsmMotor;					/* モータ反転接続Flag									*/
	BOOL	DivOut;							/* 分周パルス出力Flag									*/
	BOOL	DivOutEnable;					/* 分周パルス出力有効Flag (サンプルハード用)			*/
	BOOL	PlsTrn13bit;					/* パルス変換13bit指定Flag(ASIC設定用)					*/
	BOOL	DivPcmaskCtrl;					/* 分周C相出力マスク制御Flag							*/
	BOOL	CphaseLess;						/* CphaseLess Encoder									*/
	BOOL	PhaseOrderUWV;					/* パワー出力相順ＵＷＶ									*/
	BOOL	withPoleSensor;					/* ポールセンサ有り (リニア専用)						*/
	BOOL	MotOrgChkAlways;				/* モータ原点常時監視 (リニア専用)						*/
	BOOL	DivRatFix;						/* 分周比／最大速度 : 固定／計算 (リニア専用)			*/
	BOOL	CoreLessLinearMotor;			/* CoreLess Linear Motor								*/
	BOOL	SigmaTracMotor;					/* Sigma Trac Linear Motor								*/
	BOOL	ScaleWriteEnable;				/* スケール書込み可否Flag								*/
	BOOL	OrgClrByWrAdrCmd;				/* Origin Clear By Write Address Command 				*/
	BOOL	TuningLessLowGainMotor;			/* 調整レス制御低騒音設定モータ(調整レス専用)			*/
	BOOL	WeakenFieldEnable;				/* 電圧FB方式弱め界磁制御強制有効フラグ					*/
//----------------------------------------------------------------------------------------------------
	LONG	PulseNo;						/* Pulse Number	[pulse/rev] or [pulse/360deg]			*/
	LONG	HalfPulseNo;					/* PulseNo/2											*/
	LONG	PulseNo30deg;					/* Pulse Number [pulse/30deg]							*/
//----------------------------------------------------------------------------------------------------
//	BOOL	MotType;						/* Motor Type											*/
	LONG	EncType;						/* Motor Encoder Type									*/
	LONG	AbsoEncType;					/* Absolute Encoder Type								*/
	LONG	TypeCode;						/* Type Code for EndatType								*/
	LONG	SerialConvType;					/* Serial Converter Type								*/
//----------------------------------------------------------------------------------------------------
	LONG	MorgSft;						/*														*/
	LONG	MorgPassSft;					/*														*/
	LONG	IncreCompSft;					/*														*/
	LONG	MultiturnSft;					/*														*/
//----------------------------------------------------------------------------------------------------
	LONG	DivMode;						/* 分周モード(不使用)									*/
	LONG	DivPass;						/* 分周スルーモード										*/
	LONG	DivOutSft;						/* 分周出力シフト数										*/
	LONG	DivRate;						/* 分周比設定値											*/
	LONG	DivOutGain;						/* 分周出力ゲイン(リニア)								*/
	LONG	DivOutCmskChk;					/* 分周出力C相マスクチェックデータ(リニア)				*/
//----------------------------------------------------------------------------------------------------
	LONG	DivOvrSpd;						/* 分周出力オーバスピードレベル [2^24/OvrSpd]			*/
	LONG	MaxDivSpd;						/* 分周出力周波数上限時の最高速度 [mm/s]				*/
	LONG	MaxDivRat;						/* 最高速度時の分周比設定上限値 [pulse/spitch]			*/
//----------------------------------------------------------------------------------------------------
	LONG	MposSign;						/* Sign  Data for Motor Position Cal.					*/
	LONG	MposSftX;						/* Shift Data for Motor Position Cal.					*/
	LONG	MposSftR;						/* Shift Data for Motor Position Cal.					*/
//----------------------------------------------------------------------------------------------------
	LONG	Kmotphase;						/* Gain for Motor Phase Cal.							*/
	LONG	Kinvphase;						/* Gain for Inverse Motor Phase Cal.					*/
//----------------------------------------------------------------------------------------------------
	LONG	ComErrLimit;					/* Motor Encoder Com. Error Limit						*/
	LONG	AccErrLvl;						/* Motor Encoder Acc. Error Check						*/
	LONG	PhaChkSpd;						/* Motor Phase(UVW) Check Speed Upper Limit				*/
//----------------------------------------------------------------------------------------------------
	KSGAIN	Kmovpos;						/* Gain for Linear Motor Move Position Cal.				*/
	LONG	PhaseOffset;					/* Pole Sensor Offset 65536/360deg (リニア専用)			*/
	LONG	OrgDetDelayComp;				/* シリアル変換ユニット原点信号検出遅れ補正値			*/
	LONG	MpfPhaseOffset;					/* Pole Phase Offset 65536/360deg						*/
	KSGAIN	Kphasepos;						/* Gain for Motor Phase Position Cal.					*/
//----------------------------------------------------------------------------------------------------
	LONG	ScalePitch[3];					/* Scale Pitch From Serial Converter					*/
	LONG	ScalePitchExp;					/* Scale Pitch Exponent From Serial Converter			*/
	ULONG	MonScalePitch;					/* Scale Pitch for Monitor				[pm]			*/
	LONG	MonScaleExp;					/* Scale Pitch Exponent for Monitor		[-]				*/
	ULONG	NormScalePitch;					/* Scale Pitch							[nm]			*/
	LONG	NormScaleExp;					/* Scale Pitch Exponent					[-]				*/
//----------------------------------------------------------------------------------------------------
	LONG	KConvLoadSpd;					/* 負荷速度変換係数	[pls/ScanA] -> [2^24/OvrSpd]		*/
	//----------------------------------------------------------------------------------------------------
	USHORT	MotPrmAddress;					/* Motor Parameter Read/Write Top Address				*/
	USHORT	MotPrmSize;						/* Motor Parameter Read/Write Data Size					*/

//----------------------------------------------------------------------------------------------------
//	以下ワーク領域
//----------------------------------------------------------------------------------------------------
	BOOL	EncReady;						/* Encoder Ready										*/
	BOOL	SenReady;						/* SEN Signal Sequence Ready							*/
	VBOOL	EncDisable;						/* Encoder Disable (Mturn Reset/Change)					*/
	BOOL	PhaseReady;						/* Motor Phase Ready (Phase Found Finish)				*/
	BOOL	EncConnect;						/* Encoder Connected									*/
	BOOL	PaoseqNormal;					/* PAO Outout Seq. FALSE = Initial Mode					*/
//----------------------------------------------------------------------------------------------------
	BOOL	SeRxErr;						/* Senc Receive Error									*/
	BOOL	SeRxComp;						/* Senc Receive Complete (IntSync:Set,ScanA:Clear)		*/
	BOOL	EncDataNG;						/* Encoder Data NG Flag									*/
	BOOL	AccChkEnable;					/* Encoder Acc. Check Enable Flag						*/
	BOOL	SpdCmpEnable;					/* 速度補正実行許可										*/
	BOOL	MotOrgChkEnd;					/* Motor Origin Check End Flag							*/
//----------------------------------------------------------------------------------------------------
//	BOOL	PoleZeroWrSelect;				/* PhaseOffset・OrignPosition Zero Write Request		*/
	BOOL	ReCalcAbsPosReq;				/* Abso Position Recalculate Request					*/
//----------------------------------------------------------------------------------------------------
	LONG	MstErrCnt;						/* Encoder Mst. Error Counter							*/
	LONG	ComErrCnt;						/* Encoder Com. Error Counter for Monitor				*/
	LONG	AccErrCnt;						/* Encoder Acc. Error Counter for Monitor				*/
//----------------------------------------------------------------------------------------------------
	LONG	PhaErrChkCnt;					/* Encoder Phase Error Count							*/
	LONG	PhaErrMonCnt;					/* Encoder Phase Error Count for Monitor				*/
//----------------------------------------------------------------------------------------------------
	LONG	ChkAlarmCnt;					/* Encoder Alarm Check Count							*/
	LONG	ChkBaseTimer;					/* Encoder TimeBaseTimer Check							*/
	LONG	ChkAlmCode[2];					/* Encoder Alarm Code Check								*/
	LONG	Temperature;					/* Encoder Temperature									*/
	LONG	OverHeatCnt;					/* Encoder OverHeat Check Count							*/
	BOOL	BatWrnMode;						/* エンコーダバッテリ電圧低下をワーニングとする			*/
//----------------------------------------------------------------------------------------------------
	LONG	MtrSeqStep;						/* Multiturn Reset Sequence Step						*/
	ULONG	MtrSeqWait;						/* Multiturn Reset Sequence Wait Counter				*/
	LONG	MotPrmNum;						/* Motor Parameter Read/Write Data Size					*/
//----------------------------------------------------------------------------------------------------
	BOOL	DivModeStatus ;					/* 分周モード設定状態 0:Normal 1:DCC1 2:DCC2			*/
	BOOL	DivCMaskStatus ;				/* 分周C相マスク状態	0:Mask Off  1: Mask On			*/
	//----------------------------------------------------------------------------------------------------
	LONG	PaoSeqStep;						/* PAO Serial Output Sequence Step						*/
	LONG	PaoSeqWait;						/* PAO Serial Output Sequence Wait Counter				*/
//----------------------------------------------------------------------------------------------------
	BOOL	IncPulseReq;					/* PAO IncrePulse Output Request TRUE/FALSE				*/
	LONG	PaoSeqMcmd;						/* PAO Serial Output Sequence Cmd/Response				*/
	LONG	PaoSeqData;						/* PAO Serial Output Sequence Work Data					*/
	LONG	MturnLatch;						/* PAO Multiturn Latch Data								*/
	LONG	IncPulseOut;					/* PAO IncrePulse Output Data							*/
//----------------------------------------------------------------------------------------------------
	LONG	PsoSeqStep;						/* PSO Serial Output Sequence Step						*/
	LONG	PsoSeqWait;						/* PSO Serial Output Sequence Wait Counter				*/
	LONG	PsoMtnData;						/* PSO Serial Output Multiturn Data						*/
	LONG	PsoIncPulse;					/* PSO Serial Output Incremental Pulse					*/
//----------------------------------------------------------------------------------------------------
	LONG	MotPos;							/* Motor Position		  [32bitRingPulse]				*/
	LONG	MotAbsPos;						/* Motor Absolute Position[32bitRingPulse]				*/
	LONG	MotPosX[3];						/* Motor Encoder Position(補正後位置) [DependOnEncoder]	*/
	LONG	RcvPosX[3];						/* Motor Encoder Position(受信位置)	  [DependOnEncoder]	*/
//----------------------------------------------------------------------------------------------------
	LONG	MotSpd;							/* Motor Speed			  [2^24/OvrSpd]					*/
//----------------------------------------------------------------------------------------------------
	LONG	MotPhaX;						/* Motor Phase   Counter  [DependOnEncoder]				*/
	LONG	MotOrgX;						/* Motor Origin  Position [DependOnEncoder]				*/
	LONG	MotOrgXchk[2];					/* Motor Origin  PosCheck [DependOnEncoder]				*/
//----------------------------------------------------------------------------------------------------
	USHORT	MotPhase;						/* Motor Pole Phase for Monitor							*/
	USHORT	MotPhaseX;						/* Motor Pole Phase for Micro-Program					*/
	USHORT	MotPhaseC;						/* Motor Pole Phase for Trq Ripple Comp					*/
//----------------------------------------------------------------------------------------------------
	BOOL	Cset;							/* Cphase Set											*/
	BOOL	Cpass;							/* Cphase Pass											*/
	BOOL	DivCpass;						/* DivCphase Pass										*/
	BOOL	OrgOk;							/* Motor Origin OK										*/
	BOOL	CposOk;							/* Cphase Position Ok Flag								*/
	BOOL	IncreCposLatch;					/* Incre Cphase Position Latch Signal (ScanA --> ScanB)	*/
	BOOL	MotCphPass;						/* Motor Cphase Pass Signal fro ScanControl				*/
	LONG	MotCposX;						/* Motor C-Phase Position  [DependOnEncoder]			*/
	LONG	MposFromCpos;					/* Motor Pos. from C-Phase [+-(PulseNo/2)],[LM:+-31bit]	*/
	LONG	CphaseDist;						/* Motor Pos. from C-Phase make Micro Program			*/
//----------------------------------------------------------------------------------------------------
	LONG	MotAngle;						/* Motor RM-Cphase/LM-Pole Angle for Monitor			*/
	LONG	DivCpassPos;					/* Cphase Pass Div.Position								*/
//----------------------------------------------------------------------------------------------------
	LONG	DivOutPos;						/* 分周出力パルス										*/
	LONG	DivOutRem;						/* 分周出力パルス余り									*/
	LONG	InitDivPosOffset;				/* 分周出力パルスオフセット								*/
//----------------------------------------------------------------------------------------------------
	LONG	IniMturn;						/* Initial Multiturn Data								*/
	LONG	IniIncre;						/* Initial Incremental Pulse							*/
//----------------------------------------------------------------------------------------------------
//	APOSRG	IniAbsPos;						/* Initial Absolute Position [CmdPulseUnit]				*/
//----------------------------------------------------------------------------------------------------
	ULONG	SenOnMtnData;					/* SEN ON時の多回転量データ								*/
	ULONG	SenOnIncPulse;					/* SEN ON時の1回転内パルスデータ						*/
//----------------------------------------------------------------------------------------------------
	LONG	SenOnScalePosL;					/* SEN ON時の絶対位置データ(下位32Bit)					*/
	LONG	SenOnScalePosH;					/* SEN ON時の絶対位置データ(上位32Bit)					*/
//----------------------------------------------------------------------------------------------------
	LONG	PoleWrSeqStep;					/* PhaseOffset・OrignPosition Write	Sequence Step		*/
//	LONG	PoleWrState;					/* PhaseOffset・OrignPosition Wirte Status				*/
	ULONG	PoleWrSeqWait;					/* PhaseOffset・OrignPosition Write Wait Timer			*/
	LONG	PoleWrSum;						/* PhaseOffset・OrignPosition Write CheckSum Value		*/
//----------------------------------------------------------------------------------------------------
//	ULONG	ScanBPosL;						/* Latch Encoder Low  Data for ScanB					*/
//	LONG	ScanBPosH;						/* Latch Encoder High Data for ScanB					*/
//----------------------------------------------------------------------------------------------------
	USHORT	RxAlmCode;						/* Encoder Alarm Code									*/
	SHORT	RxPosH[2];						/* Encoder Position Data High							*/
	DWORDX	RxTimex;						/* Encoder Timex										*/
	DWORDX	RxPosL[2];						/* Encoder Position Data Low							*/
//----------------------------------------------------------------------------------------------------
	UCHAR	ABphCntr;						/* Encoder AB Pulse Counter for AB Phase Signal			*/
	UCHAR	MotOrgChkCnt;					/* Motor Origin Check Counter							*/
	UCHAR	MotPoleInfo[2];					/* Motor Pole UVW Signal Information 					*/
//----------------------------------------------------------------------------------------------------
	USHORT	AccChkWaitCnt;					/* 加速度チェック開始カウント							*/
//----------------------------------------------------------------------------------------------------
	LONG	EncLchNum;						/* エンコーダ位置サンプリング周期		(SVN_IRIS_MODE)	*/

	ENCVFBTIMBASE	FbTimeBase;				/* ベースタイマ											*/
	USHORT	FnMturnClrCmdSndNum;			/* マルチターンリセット(Fn008) コマンド送信回数			*/	/* <S0BB> */
	LONG	EncBitNo;						/* エンコーダビット数									*/	/* <S14B> */

} MENCV;



#define ENC_DATA_H
#endif /* ENC_DATA_H */
/***************************************** end of file **********************************************/
