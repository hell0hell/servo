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
#define MOT_ENC_MODEL_STR	22				/* Model												*/
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

/*--------------------------------------------------------------------------------------------------*/
/*		PaoSeqMcmd Interface Definition					<S02D>										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_PAOSEQ_NOCMD		0x00		/* No Command											*/
#define	KPX_MTLATCH_REQUEST		0x10		/* Multiturn Latch Request (ScanC ==> ScanA)			*/
#define	KPX_MTLATCH_FINISH		0x11		/* Multiturn Latch Finish  (ScanC <== ScanA)			*/
#define	KPX_MTLATCH_ERROR		0x1E		/* Multiturn Latch Error   (ScanC <== ScanA)			*/
#define	KPX_IPLSOUT_REQUEST		0x20		/* IncPulse Output Request (ScanC ==> ScanA)			*/
#define	KPX_IPLSOUT_FINISH		0x21		/* IncPulse Output Finish  (ScanC <== ScanA)			*/

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�ω������^�C�}�ɂ��ʒu�E���x�␳�p�萔��`					<S067>						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#define	VFBTINMINPLS	2								/* �ω��^�C�}�������s�ŏ������ʒu�p���X[pluse]	*/
#define	VFBTINMINCNT	5								/* �ω��^�C�}�������s�ŏ������ʒu��ٽ�A����	*/
#define	TIMBASECLK		6								/* �G���R�[�_�N���b�N���g��[MHz]: 6 MHz			*/
#define	SAENCCLKNUM		(KPI_SACYCLENS*TIMBASECLK/1000)	/* ���Z�����̴ݺ��޸ۯ���:	62.5us=375			*/
#define	SAENCCLKNUMC2	(SAENCCLKNUM*2)					/* �[���p���X�ω����ԍő�l 375 * 2 = 750		*/
#define	SAENCCLKNUMC2D3	(SAENCCLKNUM*2/3)				/* �ω����ԍŏ��l VFBTINMINPLS=2 -> 250			*/
#define	ENCLCHCLKNUM	16								/* �G���R�[�_���b�`����(2.667[us])�̃N���b�N��	*/


/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�h�e��`																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�R�}���h��`																*/
/*--------------------------------------------------------------------------------------------------*/
#define		SPG_NOP					0x0000		/* Nop   Command									*/
#define		SPG_BREAK				0x0001		/* Break Command									*/
#define		SPG_ORGCLR				0x0007		/* Origin Clear Command								*/
#define		SPG_PREAD				0x0008		/* Parameter Read Command							*/
#define		SPG_PWRITE				0x0010		/* Parameter Write Command							*/
#define		SPG_ECHO				0x00FF		/* Echo  Command									*/
#define		SPG_BANK				0x000A		/* Bank Command										*//* <S0D7> */
/*--------------------------------------------------------------------------------------------------*/
#define		SPG_RDADR				0x0041		/* Read  Address Command							*/
#define		SPG_WRADR				0x0042		/* Write Address Command							*/
#define		SPG_OPENKEY				0x594F		/* Open  Key Command								*/
#define		SPG_CLOSEKEY			0x004F		/* Close Key Command								*/
/*--------------------------------------------------------------------------------------------------*/
#define		SPG_MTRST				0x0004		/* MultiTurn Reset Command							*/
#define		SPG_MTLMTCHG			0x0006		/* MultiTurn Change Command							*/
#define		SPG_PMODEOFF			0x00F0		/* Protect Mode Off Command							*/
#define		SPG_ZEROSET				0x0012		/* Zero Set Command									*/
#define		SPG_SETCOM				0x00FA		/* Set Com Command									*/
/*--------------------------------------------------------------------------------------------------*/
#define		KPX_CDAT_ENC0_ESYNC		0x0002		/* CH0�O�������L���ACH1�O����������					*//* <S04D>:rename */
//#define		KPX_CDAT_SENC_ESYNC		0x0003		/* CH0,CH1�O���������� 							*/
#define		KPX_CDAT_ENC1_ESYNC		0x0001		/* CH0�O�����������ACH1�O�������L��					*//* <S04D> */
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���[�h�����ݒx�����Ԓ�`															*/
/*--------------------------------------------------------------------------------------------------*/
#define		ENCRDDLY_4MBPS			0x1100		/* 4Mbps�̂Ƃ�(54.4us)								*//* <S09C> */
#define		ENCRDDLY_8MBPS			0x8F0		/* 8Mbps�̂Ƃ�(28.6us)								*/


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
	UCHAR	CmdCode;
	UCHAR	Status;
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

/* SET_COM */
typedef union _ENC_RSP_SETCOM
{
	struct {
		ENC_RSP_HEADER		header;
		UCHAR				Echo;
		UCHAR				BaudRate;
		UCHAR				dummy2[12];
	} cdata;
	struct {
		USHORT				header;
		USHORT				Echo_Brate;
		USHORT				dummy[6];
	} sdata;
} ENC_RSP_SETCOM;

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

/* BANK *//* <S0D7> */
typedef union _ENC_RSP_BANK
{
	struct {
		ENC_RSP_HEADER		header;			/* D0,D1	*/
		UCHAR				BankNum;		/* D2		*/
		UCHAR				Echo;			/* D3		*/
		UCHAR				BankSizeL;		/* D4		*/
		UCHAR				BankSizeH;		/* D5		*/
		UCHAR				Cdummy[10];		/* D6~D15	*/
	} cdata;
	struct {
		USHORT				header;			/* D0,D1	*/
		USHORT				Echo_BankNum;	/* D2,D3	*/
		USHORT				Sdummy[6];		/* D4~D15	*/
	} sdata;
	ULONG		ldata[4];
} ENC_RSP_BANK;


//****************************************************************************************************
/*		Command Data Buffer																			*/
//****************************************************************************************************
/* Header Structure */
typedef struct _ENC_CMD_HEADER
{
	UCHAR	CmdCode;
	UCHAR	InvalidData;
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
		USHORT				CmdCode;
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
		USHORT				CmdCode;
		USHORT				Operand;
		USHORT				PrmData[6];
	} sdata;
	ULONG		ldata[4];
} ENC_CMD_PRMRD;

/* SET_COM */
typedef union _ENC_CMD_SETCOM
{
	struct {
		ENC_CMD_HEADER		header;
		UCHAR				blank;
		UCHAR				BaudRate;
		UCHAR				dummy[12];
	} cdata;
	struct {
		USHORT				CmdCode;
		USHORT				blank_Brate;
		USHORT				dummy[6];
	} sdata;
} ENC_CMD_SETCOM;


/* BANK *//* <S0D7> */
typedef union _ENC_CMD_BANK
{
	struct {
		ENC_RSP_HEADER		header;			/* D0,D1	*/
		UCHAR				CBankNum;		/* D2		*/
		UCHAR				ReadData[13];	/* D3~D15	*/
	} cdata;
	struct {
		USHORT				CmdCode;		/* D0,D1	*/
		USHORT				SBankNum;		/* D2,D3	*/
		USHORT				dummy[6];		/* D4~D15	*/
	} sdata;
	ULONG		ldata[4];
} ENC_CMD_BANK;

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
	ENC_RSP_SETCOM		EcnRspSetCom;
	ENC_RSP_BANK		EncRspBank;			/* <S0D7> */
	ENC_CMD_NOP			EncCmdNormal;
	ENC_CMD_OPENKEY		EncCmdKeyCntrl;
	ENC_CMD_MTLIMIT		EncDmcMTLimit;
	ENC_CMD_PRMRD		EncCmdPrmAccess;
	ENC_CMD_SETCOM		EcnCmdSetCom;
	ENC_CMD_BANK		EncCmdBank;			/* <S0D7> */
	USHORT				UsBuf[8];
	ULONG				UlBuf[8];
} ENC_DATA_BUF;


//****************************************************************************************************
/*		�V���A���G���R�[�_IF��`																	*/
//****************************************************************************************************
typedef	struct SENCX {
	VULONG			*TxBuf;					/* ���M�o�b�t�@�A�h���X									*/
	VULONG			*RxBuf;					/* ��M�o�b�t�@�A�h���X									*/
	VULONG			*RxFlg;					/* ��M�����t���O�A�h���X								*/
	VULONG			*TxStart;				/* ���M�J�n�p�������݃��W�X�^�A�h���X					*/
	VULONG			TxCmd;					/* ���M�J�n�R�}���h�f�[�^(ESYNC0/ESYNC1)				*/
	UINT			ch;						/* �ڑ�Channel											*/
	ENC_DATA_BUF	TxData;					/* ���M�f�[�^�i�[�p�o�b�t�@								*/
	ENC_DATA_BUF	RxData;					/* ��M�f�[�^�i�[�p�o�b�t�@								*/
	ASIC_HW_IF_STR	*SencSetReg;			/* Asic H/W Register Address							*/
											/*	Encoder I/F��p SGD7W�̏ꍇ�A2���ڂ�1���ڂ̃A�h���X	*/
	ASIC_HW_IF_STR	*PosCmpSetReg;			/* Asic H/W Register Address							*/
											/*	�ʒu�␳Ӽޭ�� I/F��p Ch0��1����/Ch1��2����		*/
} SENCX;

/****************************************************************************************************/
/*		�G���R�[�_�����[�^ID����`																*/
/****************************************************************************************************/
typedef struct IDINFO {
	UCHAR	Model[MOT_ENC_MODEL_STR];		/* Model[22]											*/
	UCHAR	Date[MOT_ENC_DATE_STR];			/* Date[6]												*/
	UCHAR	SNo[MOT_ENC_SNO_STR];			/* Serial No.[22]										*/
} IDINFO;

/****************************************************************************************************/
/*		���[�^�@�\�t���O(PnF24)����`				���[�^�L�����A���g���؂�ւ��Ή�<S117>		*/
/****************************************************************************************************/
typedef union _MFUNCF {
	struct {
		UINT		chgPwm:1;					/* 0:PMW���g���ύX										*/
		UINT		LspdOl:1;					/* 1:���[�^�ᑬOL���o									*/
		UINT		OCset:1;					/* 2:OC���o�d���ݒ�										*/
		UINT		VLimlv:1;					/* 3:PnEB3 �d���w�ߐ������x���ύX						*/
		UINT		IdrefLimlv:1;				/* 4:PnEB4 d���d���w�߃��~�b�g�p�d���������x��			*/
		UINT		wfMode3:1;					/* 5:��ߊE������mode3									*/
		UINT		rgonPhaseCmp:1;				/* 6:�񐶎��ʑ��i�ݕ⏞									*/
		UINT		IPMmode:1;					/* 7:IPM����											*/
		UINT		dummy:24;					/* 8-31:dummy											*/
	}f;
	ULONG	ul;
} MFUNCF;


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
	USHORT	exmotprm;				/* PnF1F : ���[�^�p�����[�^�g���̈�I��								*/
	/*------------------------------------------------------------------------------------------------------*/
	USHORT	chng_baudrate;			/* PnF20 : �G���R�[�_�ʐM���x�ݒ�(�Ή��ʐM���x�A�؂�ւ���)		*/
	/*------------------------------------------------------------------------------------------------------*//* <S0D7>:start */
	MFUNCF	motfuncf;				/* PnF24 : 	���[�^�@�\�t���O2										*//*���[�^�L�����A���g���؂�ւ��Ή�<S117>*/
	USHORT	mot_pwmf;				/* PnF25 : 	���[�^PWM���g��											*/
	USHORT	mot_ibs;				/* PnF26 : 	���[�^�ᑬ�ߕ��׌��o�x�[�X�d��							*/
	USHORT	mot_imd;				/* PnF27 : 	���[�^�ᑬ�ߕ��׌��o���ԓd��							*/
	USHORT	mot_imdt;				/* PnF28 : 	���[�^�ᑬ�ߕ��׌��o���Ԏ���							*/
	USHORT	mot_imd2;				/* PnF29 : 	���[�^�ᑬ�ߕ��׌��o���ԓd���Q							*/
	USHORT	mot_imdt2;				/* PnF2A : 	���[�^�ᑬ�ߕ��׌��o���Ԏ��ԂQ							*/
	USHORT	mot_ichgspd;			/* PnF2B : 	���[�^�ᑬ�ߕ��׌��o��]���x臒l						*/
	USHORT	mot_oc;					/* PnF2C : 	���[�^OC���o�d��										*/
	USHORT	mot_TunLess;			/* PnF2D : 	�������X���[�h�I��										*/
	DBYTEX	RefAdjust_ForWF;		/* PnF2E : 	��ߊE�����[�^�����l(�d���w��,d���d���w��)				*/
	USHORT	LCompForWF;				/* PnF2F : 	��ߊE��L�␳�W��										*/
	DBYTEX	RegPhscmp;				/* PnF30 : 	�񐶎��ʑ��i�ݕ⏞(�J�n���x,�ő��]���x)				*/
	/*------------------------------------------------------------------------------------------------------*//* <S0D7>:end */
} MENCPRM;

/****************************************************************************************************/
/*		�G���R�[�_�����p�����[�^��`																*/
/****************************************************************************************************/
typedef struct MENC_REF_PRM {
//	USHORT	b_prm2;					/* Pn002 : �@�\�I�����p�X�C�b�`�Q								*/
//	USHORT	b_prm80;				/* Pn080 : �@�\�I�����p�X�C�b�`�W�O								*/
//	USHORT	mtlmt;					/* Pn205 : �}���`�^�[�����~�b�g�ݒ�								*/
//	ULONG	scalepitch2l;			/* Pn282 : ���j�A�X�P�[���s�b�`									*/
	USHORT	syssw3;					/* PnE0D : �V�X�e���X�C�b�`�R									*/
	USHORT	scnvorgoffset;			/* PnE8C : �V���A���ϊ����j�b�g���_�I�t�Z�b�g					*/
	USHORT	phsofst;				/* PnE8D : �d����ʑ��ʒu										*/

	KSGAIN	MaxTrq;					/* Max.Torque						[Nm]		[N]				*/
	KSGAIN	Kfbpls;					/* FB Pulse Factor (Menc/Fenc)		[rad/p]		[m/p]			*/
	KSGAIN	Jmot;					/* Motor Rotor Inertia				[kg*m^2]	[kg]			*/
} MENC_REF_PRM;

/****************************************************************************************************/
/*		�ω������^�C�}�ɂ�鑬�x�␳���Z�p�f�[�^													*/
/****************************************************************************************************/
typedef struct {
	LONG	MotSpeed;										/* ���[�^���x			[2^24/OvrSpd]	*/
	LONG	dPos;											/* ���񍷕��ʒu�e�a		[pulse/scan]	*/
	LONG	LastdPos;										/* �O�񍷕��ʒu�e�a		[pulse/scan]	*/
	USHORT	HenkaItiTimer;									/* �ω��ʒu�^�C�}						*/
	USHORT	TimeInterval;									/* �ω����ԊԊu							*/
	USHORT	BaseTimeInterval;								/* �x�[�X�^�C�}�ω����ԊԊu				*/
	USHORT	BaseTimer;										/* �x�[�X�^�C�}							*/
	USHORT	HenkaTime;										/* �ω�����								*/
	USHORT	LastHenkaTime;									/* �O��ω�����							*/
	USHORT	LastBaseTimer;									/* �O��x�[�X�^�C�}						*/
	USHORT	ModChgCnt;										/* ���x���o�����ؑ֗p�J�E���^			*/
	UCHAR	Modflag;										/* �ް���ϑ��x���Z���s�t���O			*/
	UCHAR	LastModflag;									/* �O���ް���ϑ��x���Z���s�t���O		*/
	UCHAR	LastSeRxErr;									/* �O���M�װ�t���O					*/
	UCHAR	Dummy;											/* for Alignment						*/
} VFBTIMBASE;

/****************************************************************************************************/
/*		�ω������^�C�}�ɂ�鑬�x�␳���Z�p�f�[�^(�t���N�p)											*/
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
	USHORT	fcplsml;				/* Pn20A : �O�t���X�P�[���s�b�`��								*//* <S04D> */
	DBYTEX	spgfail;				/* PnE1E : ����:S-PG��M���s���e��, ���:FS-PG��M���s���e��*/
	USHORT	syssw1;					/* PnE0B: �V�X�e���X�C�b�`1										*//* <S1BE> */
} MENC_INIT_PRM;

/****************************************************************************************************/
/*		�����o�͋@�\�p��`															<S02D>			*/
/****************************************************************************************************/
typedef struct DIVINFO {
	BOOL	DivOut;							/* �����p���X�o��Flag									*/
	BOOL	DivOutEnable;					/* �����p���X�o�͗L��Flag (�T���v���n�[�h�p)			*/
	BOOL	PlsTrn13bit;					/* �p���X�ϊ�13bit�w��Flag(ASIC�ݒ�p)					*/
	BOOL	DivPcmaskCtrl;					/* ����C���o�̓}�X�N����Flag							*/
	BOOL	DivRatFix;						/* ������^�ő呬�x : �Œ�^�v�Z (���j�A��p)			*/
	BOOL	DivModeStatus ;					/* �������[�h�ݒ��� 0:Normal 1:DCC1 2:DCC2			*/
	BOOL	DivCMaskStatus ;				/* ����C���}�X�N���	0:Mask Off  1: Mask On			*/
//----------------------------------------------------------------------------------------------------
	BOOL	DivCpass;						/* DivCphase Pass										*/
	LONG	DivCpassPos;					/* Cphase Pass Div.Position								*/
//----------------------------------------------------------------------------------------------------
	UCHAR	DivMode;						/* �������[�h(�s�g�p)					<S03F>			*/
	UCHAR	DivPass;						/* �����X���[���[�h						<S03F>			*/
	UCHAR	DivOutSft;						/* �����o�̓V�t�g��							<S03F>		*/
	UCHAR	Dummy;							/* for Alignment						<S03F>			*/
	USHORT	DivRate;						/* ������ݒ�l							<S03F>			*/
	LONG	DivOutGain;						/* �����o�̓Q�C��(���j�A)								*/
	LONG	DivOutCmskChk;					/* �����o��C���}�X�N�`�F�b�N�f�[�^(���j�A)				*/
	LONG	DivOvrSpd;						/* �����o�̓I�[�o�X�s�[�h���x�� [2^24/OvrSpd]			*/
	LONG	MaxDivSpd;						/* �����o�͎��g��������̍ō����x [mm/s]				*/
	LONG	MaxDivRat;						/* �ō����x���̕�����ݒ����l [pulse/spitch]			*/
//----------------------------------------------------------------------------------------------------
	CHAR	PaoSeqStep;						/* PAO Serial Output Sequence Step			<S03F>		*/
	UCHAR	PaoSeqWait;						/* PAO Serial Output Sequence Wait Counter		<S03F>	*/
	UCHAR	IncPulseReq;					/* PAO IncrePulse Output Request TRUE/FALSE		<S03F>	*/
	UCHAR	PaoSeqMcmd;						/* PAO Serial Output Sequence Cmd/Response	<S03F>		*/
	USHORT	PaoSeqData;						/* PAO Serial Output Sequence Work Data		<S03F>		*/
	SHORT	MturnLatch;						/* PAO Multiturn Latch Data				<S03F>			*/
	LONG	IncPulseOut;					/* PAO IncrePulse Output Data							*/
	UCHAR	PsoSeqStep;						/* PSO Serial Output Sequence Step	<S0C2>			 */
	UCHAR	PsoSeqWait;						/* PSO Serial Output Sequence Wait Counter<S0C2>      */
//	SHORT	PsoMturnLatch;					/* PSO Serial Multiturn Latch Data	<S0C2>			 */
	USHORT	PsoMturnLatch;					/* PSO Serial Multiturn Latch Data	<S0C2>	<S226>		 */
	ULONG   PsoLinearData_L;                /* PSO Serial Output Linear Data_L  <S0C2>            */
	SHORT   PsoLinearData_H;                /* PSO Serial Output Linear Data_H   <S0C2>            */
	LONG	PsoIncPulseOut;					/* PSO Serial IncrePulse Output Data <S0C2>         */
//----------------------------------------------------------------------------------------------------
	LONG	DivOutPos;						/* �����o�̓p���X										*/
	LONG	DivOutRem;						/* �����o�̓p���X�]��									*/
	LONG	InitDivPosOffset;				/* �����o�̓p���X�I�t�Z�b�g								*/
//----------------------------------------------------------------------------------------------------
	DBYTEX	Bit_dp;							/* PnF03 : �G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu		*/
	USHORT	Limmlt;							/* PnF04 : �}���`�^�[�����~�b�g							*/
	BOOL	RvsDir;							/* �t��]���[�h	(Pn000.0 : ��]�����I��(�t��]���[�h))	*/
//----------------------------------------------------------------------------------------------------
	BOOL	DivOutASIC;						/* �����o�͂̃G���R�[�_�ʒu�f�[�^�̑I��(0:S/W; 1:H/W)	*/
											/*   1(H/W):�����o�͂̈ʒu�ް���JL-086�Œ�����ٽ�ϊ�	*/
											/*   0(S/W):�����o�͂̈ʒu�ް���CPU�o�R����ٽ�ϊ�		*/
//----------------------------------------------------------------------------------------------------
	UCHAR	IncreBit_dp;					/* �C���N���␳�l�ŏ��r�b�g�ʒu							*//*<S161>*/
//----------------------------------------------------------------------------------------------------
} DIVINFO;


/****************************************************************************************************/
/*		�G���R�[�_IF�f�[�^��`																		*/
/****************************************************************************************************/
typedef	struct MENCV {
	//�p�����[�^�̈�
//----------------------------------------------------------------------------------------------------
	IDINFO	MotInfo;						/* Motor Information									*/
	IDINFO	EncInfo;						/* Motor Encoder Information							*/
	IDINFO	*pActMotInfo;					/* Active Motor Information								*//* <S04D> */
	IDINFO	*pActEncInfo;					/* Active Motor Encoder Information						*//* <S04D> */
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
#if 0 /*<S02D>*/
	BOOL	DivOut;							/* �����p���X�o��Flag									*/
	BOOL	DivOutEnable;					/* �����p���X�o�͗L��Flag (�T���v���n�[�h�p)			*/
	BOOL	PlsTrn13bit;					/* �p���X�ϊ�13bit�w��Flag(ASIC�ݒ�p)					*/
	BOOL	DivPcmaskCtrl;					/* ����C���o�̓}�X�N����Flag							*/
#endif/*<S02D>*/
	BOOL	CphaseLess;						/* CphaseLess Encoder									*/
	BOOL	PhaseOrderUWV;					/* �p���[�o�͑����t�v�u									*/
	BOOL	withPoleSensor;					/* �|�[���Z���T�L�� (���j�A��p)						*/
	BOOL	MotOrgChkAlways;				/* ���[�^���_�펞�Ď� (���j�A��p)						*/
#if 0 /*<S02D>*/
	BOOL	DivRatFix;						/* ������^�ő呬�x : �Œ�^�v�Z (���j�A��p)			*/
#endif/*<S02D>*/
	BOOL	CoreLessLinearMotor;			/* CoreLess Linear Motor								*/
	BOOL	SigmaTracMotor;					/* Sigma Trac Linear Motor								*/
	BOOL	ScaleWriteEnable;				/* �X�P�[�������݉�Flag								*/
	BOOL	OrgClrByWrAdrCmd;				/* Origin Clear By Write Address Command 				*/
	BOOL	TuningLessLowGainMotor;			/* �������X����ᑛ���ݒ胂�[�^(�������X��p)			*/
	BOOL	WeakenFieldEnable;				/* �d��FB������ߊE�����䋭���L���t���O					*/
	BOOL	SenSpecEnable;					/* �ݺ���Prm 0x43(�Z���T�d�l)(TRUE=�L��)				*//* <S04D> */
	BOOL	PoleWriteEnable;				/* �d���ʑ��p�����[�^�X�P�[���������݉ۃt���O			*//* <S04D> */
//----------------------------------------------------------------------------------------------------
	LONG	PulseNo;						/* Pulse Number	[pulse/rev] or [pulse/360deg]			*/
	LONG	HalfPulseNo;					/* PulseNo/2											*/
	LONG	PulseNo30deg;					/* Pulse Number [pulse/30deg]							*/
	LONG	SoftVer;						/* �ݺ��޿���ް�ޮ� for �ٸ�ݺ���						*//* <S04D> */
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
#if 0 /*<S02D>*/
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
#endif/*<S02D>*/
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
	USHORT	PosCompMode;					/* �G���R�[�_�ʒu�␳���[�h								*//* <S004> *//*<S0C5>rename*/
	USHORT	SenSpec;						/* Bit Data for Sensor Specification					*//* <S04D> */

//----------------------------------------------------------------------------------------------------
//	�ȉ����[�N�̈�
//----------------------------------------------------------------------------------------------------
	BOOL	EncReady;						/* Encoder Ready										*/
	BOOL	SenReady;						/* SEN Signal Sequence Ready							*/
	VBOOL	EncDisable;						/* Encoder Disable (Mturn Reset/Change)					*/
	BOOL	PhaseReady;						/* Motor Phase Ready (Phase Found Finish)				*/
	BOOL	EncConnect;						/* Encoder Connected									*/
	BOOL	PaoseqNormal;					/* PAO Outout Seq. FALSE = Initial Mode					*/
	LONG	PgBaudRate;						/* Encoder AB Pulse Counter for AB Phase Signal			*/
//----------------------------------------------------------------------------------------------------
	BOOL	SeRxErr;						/* Senc Receive Error									*/
	BOOL	SeRxComp;						/* Senc Receive Complete (IntSync:Set,ScanA:Clear)		*/
	BOOL	EncDataNG;						/* Encoder Data NG Flag									*/
	BOOL	AccChkEnable;					/* Encoder Acc. Check Enable Flag						*/
	BOOL	SpdCmpEnable;					/* ���x�␳���s����										*/
	BOOL	MotOrgChkEnd;					/* Motor Origin Check End Flag							*/
//----------------------------------------------------------------------------------------------------
	BOOL	SetP0ModeIntCycleReq;			/* Set Interrupt Cycle Request Flag	(P0Mode)			*//* <S1D3> */
	BOOL	SetP1ModeIntCycleReq;			/* Set Interrupt Cycle Request Flag	(P1Mode)			*/
	BOOL	ChangeP1toP0Enable;				/* Interrupt Cycle Enable Flag							*/
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
	BOOL	IncDetOhFlg;					/* IncEncoder OverHeat Active/NonActive Select Flag		*//* <S03A> */
	BOOL	BatWrnMode;						/* �G���R�[�_�o�b�e���d���ቺ�����[�j���O�Ƃ���			*/
//----------------------------------------------------------------------------------------------------
	LONG	MtrSeqStep;						/* Multiturn Reset Sequence Step						*/
	ULONG	MtrSeqWait;						/* Multiturn Reset Sequence Wait Counter				*/
	LONG	MotPrmNum;						/* Motor Parameter Read/Write Data Size					*/
//----------------------------------------------------------------------------------------------------
#if 0 /*<S02D>*/
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
#endif  /*<S02D>*/
//----------------------------------------------------------------------------------------------------
	LONG	PsoSeqStep;						/* PSO Serial Output Sequence Step						*/
	LONG	PsoSeqWait;						/* PSO Serial Output Sequence Wait Counter				*/
	LONG	PsoMtnData;						/* PSO Serial Output Multiturn Data						*/
	LONG	PsoIncPulse;					/* PSO Serial Output Incremental Pulse					*/
//----------------------------------------------------------------------------------------------------
#if (FLOAT_USE==TRUE)
	float	MotPos;							/* Motor Position		  [32bitRingPulse]				*/
	LONG	MotAbsPos;						/* Motor Absolute Position[32bitRingPulse]				*/
	LONG	MotPosX[3];						/* Motor Encoder Position(�␳��ʒu) [DependOnEncoder]	*/
	LONG	RcvPosX[3];						/* Motor Encoder Position(��M�ʒu)	  [DependOnEncoder]	*/
#else
	LONG	MotPos;							/* Motor Position		  [32bitRingPulse]				*/
	LONG	MotAbsPos;						/* Motor Absolute Position[32bitRingPulse]				*/
	LONG	MotPosX[3];						/* Motor Encoder Position(�␳��ʒu) [DependOnEncoder]	*/
	LONG	RcvPosX[3];						/* Motor Encoder Position(��M�ʒu)	  [DependOnEncoder]	*/
#endif /* FLOAT_USE */
//----------------------------------------------------------------------------------------------------
	KSGAIN	MotSpd;							/* Motor Speed			  [2^24/OvrSpd]					*//*<S00A>*/
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
#if 0 /*<S02D>*/
	BOOL	DivCpass;						/* DivCphase Pass										*/
#endif  /*<S02D>*/
	BOOL	OrgOk;							/* Motor Origin OK										*/
	BOOL	CposOk;							/* Cphase Position Ok Flag								*/
	BOOL	IncreCposLatch;					/* Incre Cphase Position Latch Signal (ScanA --> ScanB)	*/
	BOOL	MotCphPass;						/* Motor Cphase Pass Signal fro ScanControl				*/
	LONG	MotCposX;						/* Motor C-Phase Position  [DependOnEncoder]			*/
	LONG	MposFromCpos;					/* Motor Pos. from C-Phase [+-(PulseNo/2)],[LM:+-31bit]	*/
	LONG	CphaseDist;						/* Motor Pos. from C-Phase make Micro Program			*/
//----------------------------------------------------------------------------------------------------
	LONG	MotAngle;						/* Motor RM-Cphase/LM-Pole Angle for Monitor			*/
#if 0 /*<S02D>*/
	LONG	DivCpassPos;					/* Cphase Pass Div.Position								*/
//----------------------------------------------------------------------------------------------------
	LONG	DivOutPos;						/* �����o�̓p���X										*/
	LONG	DivOutRem;						/* �����o�̓p���X�]��									*/
	LONG	InitDivPosOffset;				/* �����o�̓p���X�I�t�Z�b�g								*/
//----------------------------------------------------------------------------------------------------
#endif  /*<S02D>*/
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
	USHORT	BaseTimer;						/* Basetimer value for Speed Comp. Module				*//* <S067> */
	USHORT	HenkaItiTimer;					/* Henkaititimer value for Speed Comp. Module			*//* <S067> */
//----------------------------------------------------------------------------------------------------
	UCHAR	ABphCntr;						/* Encoder AB Pulse Counter for AB Phase Signal			*/
	UCHAR	MotOrgChkCnt;					/* Motor Origin Check Counter							*/
	UCHAR	MotPoleInfo[2];					/* Motor Pole UVW Signal Information 					*/
//----------------------------------------------------------------------------------------------------
	USHORT	AccChkWaitCnt;					/* �����x�`�F�b�N�J�n�J�E���g							*/
//----------------------------------------------------------------------------------------------------
	LONG	EncLchNum;						/* �G���R�[�_�ʒu�T���v�����O����		(SVN_IRIS_MODE)	*/

	ENCVFBTIMBASE	FbTimeBase;				/* �x�[�X�^�C�}										*/
//----------------------------------------------------------------------------------------------------
	DIVINFO DivInfo;						/* �����o�͊֘A���										*//*<S02D>*/
	//----------------------------------------------------------------------------------------------------
	USHORT	FnMturnClrCmdSndNum;			/* �}���`�^�[�����Z�b�g(Fn008) �R�}���h���M��			*/	/* <S0BB> */
	LONG	EncBitNo;						/* �G���R�[�_�r�b�g��									*/	/* <S14B> */
	ULONG	EncComErrCnt;					/* Un102���Z�b�g�p�ۑ��l								*/
//----------------------------------------------------------------------------------------------------
	BOOL	PdetReq587;						/* Pn587.0:���Ɍ��o�v��						 <S21C> 	*/

} MENCV;



#define ENC_DATA_H
#endif /* ENC_DATA_H */
/***************************************** end of file **********************************************/
