/****************************************************************************************************/
/*																									*/
/*																									*/
/*		EncData.h : �G���R�[�_�f�[�^�\���̒�`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef ENC_DATA_H


#include "MLib.h"
#include "JL086ServoIpHw.h"


/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�����[�^ID����`																*/
/*--------------------------------------------------------------------------------------------------*/
#define MOT_ENC_MODEL_STR	24				/* Model												*//* <S145> */
#define MOT_ENC_DATE_STR	6				/* Date													*/
#define MOT_ENC_SNO_STR		22				/* Serial No.											*/

/****************************************************************************************************/
/*		���j�A���[�^�p��`																			*/
/****************************************************************************************************/
#define	LMX_MPOS_SFT		12				/* ���ʒu�v�Z�p�V�t�g��									*/
#define	HPOS_TO_XPOS		20				/* 32bit - LMX_MPOS_SFT									*/
#define	FSX_FPOS_SFT		12


/*--------------------------------------------------------------------------------------------------*/
/*		Encoder Type Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	MENCTYPE_SERIAL			0x00		/* �V���A���G���R�[�_									*/
#define	MENCTYPE_PULSE			0x10		/* �p���X�G���R�[�_										*/
#define	MENCTYPE_ANALOG 		0x20		/* �A�i���O�G���R�[�_									*/
#define	MENCTYPE_ENCLESS		0xFF		/* �G���R�[�_���X										*/

#define	MENC_INCRE_TYPE			0x00		/* �A�u�\���C���N���Ƃ��Ďg�p							*/
#define	MENC_MABSO_TYPE			0x10		/* ����]�A�u�\�G���R�[�_								*/
#define	MENC_SABSO_TYPE			0x20		/* �P��]�A�u�\�G���R�[�_								*/

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Converter Type Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SCONVTYPE_NONE			0x00		/* �V���A���R���o�[�^�Ȃ�								*/
#define	SCONVTYPE_STANDARD_INC	0x01		/* �W���C���N���^�C�v									*/
#define SCONVTYPE_MITUTOYO_ABS	0x02		/* �~�c�g�����A�u�\�^�C�v								*/
#define	SCONVTYPE_ENDAT_ABS		0x03		/* EnDat�^�C�v											*/
#define	SCONVTYPE_SONY_INC		0x04		/* SMS���C���N���^�C�v									*/
#define	SCONVTYPE_SONY_ABS		0x05		/* SMS���A�u�\�^�C�v									*/

/*--------------------------------------------------------------------------------------------------*/
#define SCALE_EXP_PM			4			/* ���j�A�X�P�[���s�b�`�w��[pm]							*/
#define SCALE_EXP_UM			1			/* ���j�A�X�P�[���s�b�`�w��[um]							*/

/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�h�e��`																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�R�}���h��`																*/
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
//#define		KPX_CDAT_SENC_ESYNC				0x0000	/* CH0�O�������L���ACH1�O���������� */
#define		KPX_CDAT_SENC_ESYNC		0x0002		/* CH0�O�������L���ACH1�O����������					*/
//#define	KPX_CDAT_SENC_ESYNC		0x0003		/* CH0,CH1�O���������� *///kira

/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�h�e��`																	*/
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
/*		�V���A���G���R�[�_IF�p�f�[�^�o�b�t�@�\����`												*/
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
/*		�V���A���G���R�[�_IF��`																	*/
//****************************************************************************************************
typedef	struct SENCX {
	ASIC_HW_IF_STR	*AsicHwReg;				/* Asic H/W ���W�X�^�A�h���X							*/
	VUSHORT			*TxBuf;					/* ���M�o�b�t�@�A�h���X									*/
	VUSHORT			*RxBuf;					/* ��M�o�b�t�@�A�h���X									*/
	VUSHORT			*RxFlg;					/* ��M�����t���O�A�h���X								*/
	VUSHORT			*TxStart;				/* ���M�J�n�p�������݃��W�X�^�A�h���X					*/
	VUSHORT			TxCmd;					/* ���M�J�n�R�}���h�f�[�^(ESYNC0/ESYNC1)				*/
	UINT			ch;						/* �ڑ�Channel											*/
	ENC_DATA_BUF	TxData;					/* ���M�f�[�^�i�[�p�o�b�t�@								*/
	ENC_DATA_BUF	RxData;					/* ��M�f�[�^�i�[�p�o�b�t�@								*/
} SENCX;

/****************************************************************************************************/
/*		�G���R�[�_�����[�^ID����`																*/
/****************************************************************************************************/
typedef struct IDINFO {
	UCHAR	Model[MOT_ENC_MODEL_STR];		/* Model[12]											*/
	UCHAR	Date[MOT_ENC_DATE_STR];			/* Date[3]												*/
	UCHAR	SNo[MOT_ENC_SNO_STR];			/* Serial No.[11]										*/
} IDINFO;


/****************************************************************************************************/
/*		�G���R�[�_�p�����[�^��`																	*/
/****************************************************************************************************/
typedef struct MENCPRM {
	DBYTEX	typm_e;					/* PnF00 : ���[�^�`��(8bit), ���͓d��(4bit), �G���R�[�_�^�C�v(4bit)	*/
	USHORT	encver;					/* PnF01 : �G���R�[�_�\�t�g�o�[�W����								*/
	USHORT	motw;					/* PnF02 : ���[�^�e��												*/
	DBYTEX	bit_dp;					/* PnF03 : �G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu					*/
	USHORT	limmlt;					/* PnF04 : �}���`�^�[�����~�b�g										*/
	DBYTEX	vrat_max;				/* PnF05 : ��i���x, �ő呬�x										*/
	DBYTEX	oslv_pol;				/* PnF06 : �n�r���o���x��, �|�[����									*/
	USHORT	rattrq;					/* PnF07 : ��i�g���N												*/
	USHORT	maxtrq;					/* PnF08 : �ő�g���N												*/
	USHORT	irat;					/* PnF09 : ��i�d��													*/
	USHORT	imax;					/* PnF0A : �u���ő�d��												*/
	USHORT	emfcmp;					/* PnF0B : �d�l�e�萔												*/
	USHORT	jmot;					/* PnF0C : ���[�^�C�i�[�V��											*/
	USHORT	motr;					/* PnF0D : �d�@�q������R											*/
	USHORT	motl;					/* PnF0E : �d�@�q�C���_�N�^���X										*/
	USHORT	bastrq;					/* PnF0F : �ߕ��׌��o�x�[�X�d��										*/
	/*------------------------------------------------------------------------------------------------------*/
	USHORT	midtrq;					/* PnF10 : �ߕ��׌��o���ԓd��										*/
	USHORT	midolt;					/* PnF11 : �ߕ��׌��o���Ԏ���										*/
	USHORT	mtrq2;					/* PnF12 : �ߕ��׌��o���ԓd���Q										*/
	USHORT	mtolt2;					/* PnF13 : �ߕ��׌��o���Ԏ��ԂQ										*/
	DBYTEX	phscmpM1;				/* PnF14 : �ʑ��⏞�P(�⏞�l,�J�n���x)								*/
	USHORT	ppitch;					/* PnF15 : �|�[���s�b�`												*/
	USHORT	motlq0;					/* PnF16 : q���C���_�N�^���XLq0										*/
	USHORT	motlq1;					/* PnF17 : q���C���_�N�^���XLq1										*/
	DBYTEX	exp_trat_jmot;			/* PnF18 : ���ʁF��i�g���N�w���A��ʁF���[�^�C�i�[�V���w��			*/
	DBYTEX	exp_motw_spd;			/* PnF19 : ���ʁF��i�o�͎w���A��ʁF��]���w��						*/
	DBYTEX	BitnoDpoint;			/* PnF1A : �t���N�G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu			*/
	USHORT	poleoffset;				/* PnF1B : �|�[���Z���T�I�t�Z�b�g��									*/
	USHORT	motresfrq;				/* PnF1C : ���[�^���U�}���p�m�b�`�t�B���^���g��						*/
	USHORT	phscmpM2;				/* PnF1D : �ʑ��p�␳�l												*/
	USHORT	flg_wf;					/* PnF1E : �t���O													*/
} MENCPRM;

/****************************************************************************************************/
/*		�G���R�[�_�����p�����[�^��`																*/
/****************************************************************************************************/
typedef struct MENC_REF_PRM {
//	USHORT	b_prm2;					/* Pn002 : �@�\�I�����p�X�C�b�`�Q								*/
//	USHORT	b_prm80;				/* Pn080 : �@�\�I�����p�X�C�b�`�W�O								*/
//	USHORT	mtlmt;					/* Pn205 : �}���`�^�[�����~�b�g�ݒ�									*/
//	ULONG	scalepitch2l;			/* Pn282 : ���j�A�X�P�[���s�b�`									*/
	USHORT	syssw3;					/* PnE0D : �V�X�e���X�C�b�`�R									*/
	USHORT	scnvorgoffset;			/* PnE8C : �V���A���ϊ����j�b�g���_�I�t�Z�b�g					*/
	USHORT	phsofst;				/* PnE8D : �d����ʑ��ʒu										*/

	KSGAIN	MaxTrq;					/* Max.Torque						[Nm]		[N]				*/
	KSGAIN	Kfbpls;					/* FB Pulse Factor (Menc/Fenc)		[rad/p]		[m/p]			*/
	KSGAIN	Jmot;					/* Motor Rotor Inertia				[kg*m^2]	[kg]			*/
} MENC_REF_PRM;

/****************************************************************************************************/
/*		�ω������^�C�}���Z���s�p��`																*/
/****************************************************************************************************/
typedef struct ENCVFBTIMBASE {
	LONG	fencspeed;										/* ���ב��x				[2^24/OvrSpd]	*/
	USHORT	BaseTimeInterval;								/* �x�[�X�^�C�}�ω����ԊԊu				*/
	USHORT	BaseTimer;										/* �x�[�X�^�C�}							*/
	USHORT	LastBaseTimer;									/* �O��x�[�X�^�C�}						*/
	UCHAR	Modflag;										/* �x�[�X�^�C�}���x���Z���s�t���O		*/
	UCHAR	LastModflag;									/* �O��x�[�X�^�C�}���x���Z���s�t���O	*/
} ENCVFBTIMBASE;

/****************************************************************************************************/
/*		�G���R�[�_�֘A�������p�p�����[�^��`														*/
/****************************************************************************************************/
typedef struct MENC_INIT_PRM {
	MENCPRM	*MencP;					/* PnF** : �G���R�[�_�p�����[�^��`�ւ̃|�C���^					*/
	LONG	scalepitch2l;			/* Pn282 : ���j�A�X�P�[���s�b�`									*/
	USHORT	b_prm2;					/* Pn002 : �@�\�I�����p�X�C�b�`2								*/
	USHORT	b_prm80;				/* Pn080 : �@�\�I�����p�X�C�b�`80								*/
	USHORT	mtlmt;					/* Pn205 : �}���`�^�[�����~�b�g�ݒ�								*/
	DBYTEX	spgfail;				/* PnE1E : ����:S-PG��M���s���e��, ���:FS-PG��M���s���e��*/
} MENC_INIT_PRM;

/****************************************************************************************************/
/*		�G���R�[�_IF�f�[�^��`																		*/
/****************************************************************************************************/
typedef	struct MENCV {
	//�p�����[�^�̈�
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
	BOOL	RevAsmMotor;					/* ���[�^���]�ڑ�Flag									*/
	BOOL	DivOut;							/* �����p���X�o��Flag									*/
	BOOL	DivOutEnable;					/* �����p���X�o�͗L��Flag (�T���v���n�[�h�p)			*/
	BOOL	PlsTrn13bit;					/* �p���X�ϊ�13bit�w��Flag(ASIC�ݒ�p)					*/
	BOOL	DivPcmaskCtrl;					/* ����C���o�̓}�X�N����Flag							*/
	BOOL	CphaseLess;						/* CphaseLess Encoder									*/
	BOOL	PhaseOrderUWV;					/* �p���[�o�͑����t�v�u									*/
	BOOL	withPoleSensor;					/* �|�[���Z���T�L�� (���j�A��p)						*/
	BOOL	MotOrgChkAlways;				/* ���[�^���_�펞�Ď� (���j�A��p)						*/
	BOOL	DivRatFix;						/* ������^�ő呬�x : �Œ�^�v�Z (���j�A��p)			*/
	BOOL	CoreLessLinearMotor;			/* CoreLess Linear Motor								*/
	BOOL	SigmaTracMotor;					/* Sigma Trac Linear Motor								*/
	BOOL	ScaleWriteEnable;				/* �X�P�[�������݉�Flag								*/
	BOOL	OrgClrByWrAdrCmd;				/* Origin Clear By Write Address Command 				*/
	BOOL	TuningLessLowGainMotor;			/* �������X����ᑛ���ݒ胂�[�^(�������X��p)			*/
	BOOL	WeakenFieldEnable;				/* �d��FB������ߊE�����䋭���L���t���O					*/
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
	LONG	DivMode;						/* �������[�h(�s�g�p)									*/
	LONG	DivPass;						/* �����X���[���[�h										*/
	LONG	DivOutSft;						/* �����o�̓V�t�g��										*/
	LONG	DivRate;						/* ������ݒ�l											*/
	LONG	DivOutGain;						/* �����o�̓Q�C��(���j�A)								*/
	LONG	DivOutCmskChk;					/* �����o��C���}�X�N�`�F�b�N�f�[�^(���j�A)				*/
//----------------------------------------------------------------------------------------------------
	LONG	DivOvrSpd;						/* �����o�̓I�[�o�X�s�[�h���x�� [2^24/OvrSpd]			*/
	LONG	MaxDivSpd;						/* �����o�͎��g��������̍ō����x [mm/s]				*/
	LONG	MaxDivRat;						/* �ō����x���̕�����ݒ����l [pulse/spitch]			*/
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
	LONG	PhaseOffset;					/* Pole Sensor Offset 65536/360deg (���j�A��p)			*/
	LONG	OrgDetDelayComp;				/* �V���A���ϊ����j�b�g���_�M�����o�x��␳�l			*/
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
	LONG	KConvLoadSpd;					/* ���ב��x�ϊ��W��	[pls/ScanA] -> [2^24/OvrSpd]		*/
	//----------------------------------------------------------------------------------------------------
	USHORT	MotPrmAddress;					/* Motor Parameter Read/Write Top Address				*/
	USHORT	MotPrmSize;						/* Motor Parameter Read/Write Data Size					*/

//----------------------------------------------------------------------------------------------------
//	�ȉ����[�N�̈�
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
	BOOL	SpdCmpEnable;					/* ���x�␳���s����										*/
	BOOL	MotOrgChkEnd;					/* Motor Origin Check End Flag							*/
//----------------------------------------------------------------------------------------------------
//	BOOL	PoleZeroWrSelect;				/* PhaseOffset�EOrignPosition Zero Write Request		*/
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
	BOOL	BatWrnMode;						/* �G���R�[�_�o�b�e���d���ቺ�����[�j���O�Ƃ���			*/
//----------------------------------------------------------------------------------------------------
	LONG	MtrSeqStep;						/* Multiturn Reset Sequence Step						*/
	ULONG	MtrSeqWait;						/* Multiturn Reset Sequence Wait Counter				*/
	LONG	MotPrmNum;						/* Motor Parameter Read/Write Data Size					*/
//----------------------------------------------------------------------------------------------------
	BOOL	DivModeStatus ;					/* �������[�h�ݒ��� 0:Normal 1:DCC1 2:DCC2			*/
	BOOL	DivCMaskStatus ;				/* ����C���}�X�N���	0:Mask Off  1: Mask On			*/
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
	LONG	MotPosX[3];						/* Motor Encoder Position(�␳��ʒu) [DependOnEncoder]	*/
	LONG	RcvPosX[3];						/* Motor Encoder Position(��M�ʒu)	  [DependOnEncoder]	*/
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
	LONG	DivOutPos;						/* �����o�̓p���X										*/
	LONG	DivOutRem;						/* �����o�̓p���X�]��									*/
	LONG	InitDivPosOffset;				/* �����o�̓p���X�I�t�Z�b�g								*/
//----------------------------------------------------------------------------------------------------
	LONG	IniMturn;						/* Initial Multiturn Data								*/
	LONG	IniIncre;						/* Initial Incremental Pulse							*/
//----------------------------------------------------------------------------------------------------
//	APOSRG	IniAbsPos;						/* Initial Absolute Position [CmdPulseUnit]				*/
//----------------------------------------------------------------------------------------------------
	ULONG	SenOnMtnData;					/* SEN ON���̑���]�ʃf�[�^								*/
	ULONG	SenOnIncPulse;					/* SEN ON����1��]���p���X�f�[�^						*/
//----------------------------------------------------------------------------------------------------
	LONG	SenOnScalePosL;					/* SEN ON���̐�Έʒu�f�[�^(����32Bit)					*/
	LONG	SenOnScalePosH;					/* SEN ON���̐�Έʒu�f�[�^(���32Bit)					*/
//----------------------------------------------------------------------------------------------------
	LONG	PoleWrSeqStep;					/* PhaseOffset�EOrignPosition Write	Sequence Step		*/
//	LONG	PoleWrState;					/* PhaseOffset�EOrignPosition Wirte Status				*/
	ULONG	PoleWrSeqWait;					/* PhaseOffset�EOrignPosition Write Wait Timer			*/
	LONG	PoleWrSum;						/* PhaseOffset�EOrignPosition Write CheckSum Value		*/
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
	USHORT	AccChkWaitCnt;					/* �����x�`�F�b�N�J�n�J�E���g							*/
//----------------------------------------------------------------------------------------------------
	LONG	EncLchNum;						/* �G���R�[�_�ʒu�T���v�����O����		(SVN_IRIS_MODE)	*/

	ENCVFBTIMBASE	FbTimeBase;				/* �x�[�X�^�C�}											*/
	USHORT	FnMturnClrCmdSndNum;			/* �}���`�^�[�����Z�b�g(Fn008) �R�}���h���M��			*/	/* <S0BB> */
	LONG	EncBitNo;						/* �G���R�[�_�r�b�g��									*/	/* <S14B> */

} MENCV;



#define ENC_DATA_H
#endif /* ENC_DATA_H */
/***************************************** end of file **********************************************/
