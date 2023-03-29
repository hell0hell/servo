/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsPrmSgdvCmOpt.h : �p�����[�^��`�w�b�_�t�@�C��											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. �p�����[�^��`�\����																		*/
/*																									*/
/*		2. �p�����[�^�ݒ�f�[�^�\����																*/
/*																									*/
/*		3. �W���p�����[�^�v�Z�֐�																	*/
/*																									*/
/*		4. �p�����[�^��`Extern�錾																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2013.01.11 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/

#define FORMAT_ID			0x0909





/****************************************************************************************************/
/*																									*/
/*		�G�N�Z���\���ʒ�`																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�\�����̒�																					*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_BASENBL			0				/* �����F�j�u���\��									*/
#define DSPDEF_BASEDEC			1				/* �����F10�i�\��									*/
#define DSPDEF_BASEHEX			2				/* �����F16�i�\��									*/
#define DSPDEF_BASEBIT			3				/* �����F�r�b�g�\��									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����t�����ۂ�																				*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_W_SIGN			TRUE			/* �����t											*/
#define DSPDEF_NOSIGN			FALSE			/* �����Ȃ�											*/
/*--------------------------------------------------------------------------------------------------*/
/*		�A�N�Z�X���x��(���ӁF0 �͎g�p�s��)															*/
/*--------------------------------------------------------------------------------------------------*/
#define ACCLVL_USER1			1				/* ���[�U�[�P										*/
#define ACCLVL_USER2			2				/* ���[�U�[�Q										*/
#define ACCLVL_USER3			3				/* ���[�U�[�R										*//* <S0FE> */
//#define ACCLVL_SYSTEM			3				/* �V�X�e��											*//* <S0FE> */
#define ACCLVL_SYSTEM			4				/* �V�X�e��											*//* <S0FE> */
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^��`�\���̒�`																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^��`�\����																		*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	PRMDEF_VERSION
typedef struct	{
	struct	{
		UINT	IsLong     : 1;					/* �����O�p�����[�^(TRUE/FALSE)						*/
		UINT	WordOrder  : 1;					/* �p�����[�^���́A���̃��[�h�̏���					*/
		UINT	AdjustPrm  : 1;					/* �����p�����[�^��?(ADJUST/NORMAL)					*/
		UINT	NeedReboot : 1;					/* �d���ė��グ(NEEDBOOT/NON_BOOT)					*/
		UINT	Place      : 1;					/* �i�[�ꏊ(BOTH_RAM_EEPROM/ONLY_RAM)				*/
		UINT	Base       : 2;					/* �\�����̒�(DEC/HEX/NBL)							*/
		UINT	Sign       : 1;					/* �\�����̕����L��(W_SIGN/NOSIGN)					*/
		UINT	Decimals   : 3;					/* �����_�ȉ�����(0-4)								*/
		UINT	ChgDpLsm   : 1;					/* �����_�ʒu�ύX(�ᑬ���[�^)						*/
		UINT	ReadOnly   : 1;					/* ���[�h�I�����[(WRITABLE/READONLY)				*/
		UINT	AdjOpeIn   : 1;					/* �I�y���[�^���͒���(TRUE/FALSE)					*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	SpareBits  : 2;					/* �\��												*/
		UINT	FormatRev  : 8;					/* FORMAT_ID���ŗ��� 1byte�f�[�^�Ŏg�p���܂��̂Ŕz�u�ύX���Ȃ��ł�������(�ޯ�̨���ޓ�16�`23bit�ɒ�`����Ă��邱��)	*/
		UINT	Unit       : 8;					/* �P�� (Reserved)									*/
	} Attr;										/* �p�����[�^����									*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ChkSpUse;							/* �޼�OPE�\���I��(���:4Bit)/��p�g�p����(����:4Bit) 	*/
	UCHAR	AccessLevel;						/* 1:User1, 2:User2, 3:System						*/
	USHORT	EepromAddr;							/* EEPROM�A�h���X + ����(DefaultPrm����)			*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*FuncPtr)( ULONG Cmd, ULONG Data, ULONG Digit );	/* �����֐��ւ̃|�C���^				*/
	USHORT	*RamPtr;							/* RAM�i�[�ꏊ�ւ̃|�C���^							*/
	USHORT	LowerLimit;							/* �����l											*/
	USHORT	UpperLimit;							/* ����l											*/
} PRMDEF;
typedef const PRMDEF CPRMDEF;					/* const �p�����[�^��`								*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�Ǘ��e�[�u���\����																*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	PRMTBL_VERSION
typedef struct	{
	USHORT	PnNo;								/* �o���ԍ�											*/
	CPRMDEF	*PrmDefPtr;							/* �p�����[�^��`�ւ̃|�C���^						*/
} PRMTBL;
typedef const PRMTBL CPRMTBL;					/* const �p�����[�^�Ǘ��e�[�u��						*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		PRMDEF Version																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_VERSION			0x0001			/* PRMDEF Version									*/
#define PRMTBL_VERSION			0x0001			/* PRMTBL Version									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^���																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_ADJUST			TRUE			/* �����p�����[�^									*/
#define PRMDEF_NORMAL			FALSE			/* �ʏ�p�����[�^									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�ύX��ė��グ�̗v�E�s�v															*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_NEEDBOOT			TRUE			/* �ė��グ�K�v										*/
#define PRMDEF_NON_BOOT			FALSE			/* �ė��グ�s�v										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�i�[�ꏊ																			*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_RAM_EEPROM		TRUE			/* RAM��EEPROM										*/
#define PRMDEF_ONLY_RAM			FALSE			/* RAM�̂�											*/
/*--------------------------------------------------------------------------------------------------*/
/*		���[�h�I�����[																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_READONLY			TRUE			/* ���[�h�I�����[									*/
#define PRMDEF_WRITABLE			FALSE			/* �����݉�											*/
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM�A�h���X������`(PRMDEF.EepromAddr)													*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_EEPADRMSK		0x0FFF			/* �A�h���X�}�X�N									*/
#define PRMDEF_NODEFTPRM		0x8000			/* �f�t�H���g�p�����[�^������`						*/
/*--------------------------------------------------------------------------------------------------*/
/*		�P �� (Reserved)																			*/
/*--------------------------------------------------------------------------------------------------*/
#define UNIT_NONE				0x00			/* No Unit											*/
#define UNIT_TIME_SEC			0x01			/* time [sec]										*/
#define UNIT_TIME_MS			0x02			/* time [ms]										*/
#define UNIT_TIME_US			0x03			/* time [us]										*/
#define UNIT_SECINV				0x04			/* [1/s]											*/
#define UNIT_FREQ				0x05			/* Frequency [Hz]									*/
#define UNIT_PERCENT			0x06			/* percent (Numerical value 0-100)					*/
#define UNIT_ROT_SPEED			0x07			/* Speed [min-1]									*/
#define UNIT_LIN_SPEED			0x08			/* Speed [mm/s]										*/
#define UNIT_ROT_ACCEL			0x09			/* Acceleration [r/min/s]							*/
#define UNIT_LIN_ACCEL			0x0A			/* Acceleration [mm/s^2]							*/
#define UNIT_REFPULS			0x0B			/* length [Pulse]									*/
#define UNIT_ENCPULS			0x0C			/* length [Pulse]									*/
#define UNIT_PPR				0x0D			/* others [Pulse/rev]								*/
#define UNIT_REVOLUTION			0x0E			/* length revolution [rev]							*/
#define UNIT_REFPULS_SPD		0x0F			/* Pulse Speed [P/s]								*/
#define UNIT_ENCPULS_PITCH		0x10			/* length [Pulse/Pitch]								*/
#define UNIT_VOLT_PER_SPD		0x11			/* Voltage [V] per rate speed						*/
#define UNIT_VOLT_PER_TRQ		0x12			/* Voltage [V] per rate torque						*/
#define UNIT_VOLTAGE			0x13			/* Voltage [V]										*/
#define UNIT_CURRENT			0x14			/* Current [A]										*/
#define UNIT_RESIST				0x15			/* Resistance [Ohm]									*/
#define UNIT_WATTAGE			0x16			/* Wattage [W]										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�v�Z�R�}���h��`																	*/
/*--------------------------------------------------------------------------------------------------*/
#define PNCAL_PRMCAL			0x00			/* Parameter Calculation							*/
#define PNCAL_ADJINCDATA		0x01			/* Adjust Increment Data							*/
#define PNCAL_ADJDECDATA		0x02			/* Adjust Decrement Data							*/
#define PNCAL_PRMCALFROMREG		0x03			/* Parameter Calculation(From Register I/F)			*/
#define PNCAL_COMPRMEEPROM_WR	0x04			/* �p�����[�^EEPROM��������							*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�ݒ�f�[�^�\���̒�`																*/
/*																									*/
/****************************************************************************************************/
typedef struct
{
	USHORT	comCtrl;			/* Pn800 : �ʐM����@�\�I��											*/
	USHORT	sLimitSw;			/* Pn801 : �\�t�g���~�b�g�@�\�I��									*/
	USHORT	cmdCtrlSw;			/* Pn802 : �R�}���h����ݒ�											*/
	USHORT	zpWidth;			/* Pn803 : ���_�ʒu�͈�												*/
	USHORT	p_sLimitL;			/* Pn804 : �����\�t�g���~�b�g�l L									*/
	USHORT	p_sLimitH;			/* Pn805 : �����\�t�g���~�b�g�l H									*/
	USHORT	n_sLimitL;			/* Pn806 : �����\�t�g���~�b�g�l L									*/
	USHORT	n_sLimitH;			/* Pn807 : �����\�t�g���~�b�g�l H									*/
	USHORT	absOffsetL;			/* Pn808 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g L							*/
	USHORT	absOffsetH;			/* Pn809 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g H							*/
	USHORT	accRate1;			/* Pn80A : �P�i�ڒ��������萔										*/
	USHORT	accRate2;			/* Pn80B : �Q�i�ڒ��������萔										*/
	USHORT	accChgSpd;			/* Pn80C : �����萔�ؑւ����x										*/
	USHORT	decRate1;			/* Pn80D : �P�i�ڒ��������萔										*/
	USHORT	decRate2;			/* Pn80E : �Q�i�ڒ��������萔										*/
	USHORT	decChgSpd;			/* Pn80F : �����萔�ؑւ����x										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	filExpBias;			/* Pn810 : �w���֐��������o�C�A�X									*/
	USHORT	filExpTb;			/* Pn811 : �w���֐����������萔										*/
	USHORT	filMaTb;			/* Pn812 : �ړ����ώ���												*/
	USHORT	resrv2;				/* Pn813 : �\��														*/
	USHORT	exDistL;			/* Pn814 : �O���ʒu���ߍŏI���s���� L								*/
	USHORT	exDistH;			/* Pn815 : �O���ʒu���ߍŏI���s���� H								*/
	USHORT	zretDir;			/* Pn816 : ���_���A���[�h�ݒ�										*/
	USHORT	zretSpd1;			/* Pn817 : ���_���A�A�v���[�`���x�P									*/
	USHORT	zretSpd2;			/* Pn818 : ���_���A�A�v���[�`���x�Q									*/
	USHORT	zretDistL;			/* Pn819 : ���_���A�ŏI���s���� L									*/
	USHORT	zretDistH;			/* Pn81A : ���_���A�ŏI���s���� H									*/
	USHORT	resrv3;				/* Pn81B : �\��														*/
	USHORT	resrv4;				/* Pn81C : �\��														*/
	USHORT	resrv5;				/* Pn81D : �\��														*/
	USHORT	ioMonSel;			/* Pn81E : ���͐M�����j�^�I��										*/
	USHORT	optBitSw;			/* Pn81F : �R�}���h�f�[�^���t�I��									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	p_ltLimitL;			/* Pn820 : �������b�`�\�̈� L										*/
	USHORT	p_ltLimitH;			/* Pn821 : �������b�`�\�̈� H										*/
	USHORT	n_ltLimitL;			/* Pn822 : �t�����b�`�\�̈� L										*/
	USHORT	n_ltLimitH;			/* Pn823 : �t�����b�`�\�̈� H										*/
	USHORT	opMon1;				/* Pn824 : �I�v�V�������j�^�P�I��									*/
	USHORT	opMon2;				/* Pn825 : �I�v�V�������j�^�Q�I��									*/
	USHORT	decRate1Emg;		/* Pn826 : ����~�p�P�i�ڒ��������萔								*/
	USHORT	decRate2Emg;		/* Pn827 : ����~�p�Q�i�ڒ��������萔								*/
	USHORT	decChgSpdEmg;		/* Pn828 : ����~�p�����萔�ؑւ����x								*/
	USHORT	svOffTime;			/* Pn829 : SVOFF�҂�����(������~SVOFF��)							*/
	USHORT	optbit1;			/* Pn82A : OPTION�@�\�r�b�g���t�P									*/
	USHORT	optbit2;			/* Pn82B : OPTION�@�\�r�b�g���t�Q									*/
	USHORT	optbit3;			/* Pn82C : OPTION�@�\�r�b�g���t�R									*/
	USHORT	optbit4;			/* Pn82D : OPTION�@�\�r�b�g���t�S									*/
	USHORT	optbit5;			/* Pn82E : OPTION�@�\�r�b�g���t�T									*/
	USHORT	resrv6;				/* Pn82F : �\��														*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	rotrefL;			/* Pn830 : �@�B���]������̎w�ߒP�ʗ�L							*/
	USHORT	rotrefH;			/* Pn831 : �@�B���]������̎w�ߒP�ʗ�H							*/
	USHORT	st_num;				/* Pn832 : �@�B���]������̃X�e�[�V������							*/
	USHORT	rateChgSw;			/* Pn833 : POSING�萔�I��											*/
	USHORT	accRate12L;			/* Pn834 : �P�i�ڒ��������萔 2nd L									*/
	USHORT	accRate12H;			/* Pn835 : �P�i�ڒ��������萔 2nd H									*/
	USHORT	accRate22L;			/* Pn836 : �Q�i�ڒ��������萔 2nd L									*/
	USHORT	accRate22H;			/* Pn837 : �Q�i�ڒ��������萔 2nd H									*/
	USHORT	accChgSpd2L;		/* Pn838 : �����萔�ؑւ����x 2nd L									*/
	USHORT	accChgSpd2H;		/* Pn839 : �����萔�ؑւ����x 2nd H									*/
	USHORT	decRate12L;			/* Pn83A : �P�i�ڒ��������萔 2nd L									*/
	USHORT	decRate12H;			/* Pn83B : �P�i�ڒ��������萔 2nd H									*/
	USHORT	decRate22L;			/* Pn83C : �Q�i�ڒ��������萔 2nd L									*/
	USHORT	decRate22H;			/* Pn83D : �Q�i�ڒ��������萔 2nd H									*/
	USHORT	decChgSpd2L;		/* Pn83E : �����萔�ؑւ����x 2nd L									*/
	USHORT	decChgSpd2H;		/* Pn83F : �����萔�ؑւ����x 2nd H									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	decRateEmg2L;		/* Pn840 : ����~�p�Q�i�ڒ��������萔�@2nd L						*/
	USHORT	decRateEmg2H;		/* Pn841 : ����~�p�Q�i�ڒ��������萔�@2nd H						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ltSeqNum;			/* Pn850 : �A�����b�`�V�[�P���X��									*/
	USHORT	ltLoopNum;			/* Pn851 : �A�����b�`��											*/
	USHORT	ltSeqSet1;			/* Pn852 : �A�����b�`�V�[�P���X�ݒ�1-4								*/
	USHORT	ltSeqSet2;			/* Pn853 : �A�����b�`�V�[�P���X�ݒ�4-8								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	comDataCtrl;		/* Pn88F : �ʐMCMD/RSP�f�[�^����									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkNum;				/* Pn900 : �o���N��													*/
	USHORT	bnkMemNum;			/* Pn901 : �o���N�����o��											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkMember[15];		/* Pn902 : �o���N�����o												*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	prmVer;				/* Pn9E0 : �p�����[�^Ver.											*/
	USHORT	spdDivision;		/* Pn9E1 : ���x����\�ݒ�											*/
	USHORT	CmdOpSw;			/* Pn9E2 : Command-Option�@�\SW										*/
	USHORT	sys_rsrv2;			/* Pn9E3 : 															*/
	USHORT	sys_rsrv3;			/* Pn9E4 : 															*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	zretSpd12L;			/* Pn842 : ���_���A�A�v���[�`���x�P 2nd L							*/
	USHORT	zretSpd12H;			/* Pn843 : ���_���A�A�v���[�`���x�P 2nd H							*/
	USHORT	zretSpd22L;			/* Pn844 : ���_���A�A�v���[�`���x�Q 2nd L							*/
	USHORT	zretSpd22H;			/* Pn845 : ���_���A�A�v���[�`���x�Q 2nd H							*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	nodeAdr;			/* Pn880 : �ǃA�h���X�ݒ�											*/
	USHORT	trByte;				/* Pn881 : �`���o�C�g��												*/
	USHORT	comTCycle;			/* Pn882 : �`������													*/
	USHORT	comAplCycle;		/* Pn883 : �ʐM����													*/
	USHORT	comErrCnt;			/* Pn88A : ��M�G���[�J�E���^										*/
	USHORT	comErrSts1;			/* Pn88B : (�\��)													*/
	USHORT	comErrSts2;			/* Pn88C : �ʐM�G���[�������́A�G���[�X�e�[�^�X(�ݐ�)				*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	almCmd[16];			/* Pn890 : �A���[�����A�ʐMCMD�f�[�^								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	almRsp[16];			/* Pn8A0 : �A���[�����A�ʐMRSP�f�[�^								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	comCmd[16];			/* Pn8B0 : Online�ʐMCMD�f�[�^										*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	comRsp[16];			/* Pn8C0 : Online�ʐMRSP�f�[�^										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	OpCardID;			/* Pn8E0 : �w��Option�p Option Card ID								*/
	USHORT	OpCardCtrlSw;		/* Pn8E1 : �w��Option�p Option ����ݒ�								*/
	USHORT	OpCardSwVerL;		/* Pn8E2 : �w��Option�p Option S/W Ver.								*/
	USHORT	OpCardSwVerH;		/* Pn8E3 : �w��Option�p Option S/W Ver.								*/
	USHORT	OpCardYNo;			/* Pn8E4 : �w��Option�p Option Y Spec. No.							*/
	USHORT	OpCardDate;			/* Pn8E5 : �w��Option�p ManufactureDate.							*/
	USHORT	OpCardSvSw;			/* Pn8E6 : �w��Option�p �T�[�{SW�ݒ�								*/
	USHORT	OpCardAlmCode;		/* Pn8E8 : �w��Option�p �A���[���ݒ�								*/
	USHORT	OpCardAlmAttr;		/* Pn8E9 : �w��Option�p �A���[���ݒ�								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	OpCardModel[8];		/* Pn8F0 : �w��Option�p Option Model								*/
	USHORT	OpCardSNo[8];		/* Pn8F8 : �w��Option�p Serial No									*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkData[64];		/* Pn920 : �o���N�f�[�^												*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	cmdopv;				/* PnB00 : ���z�w�߃I�v�V�����p�����[�^								*/
} OPPRMDAT;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�ݒ�f�[�^�ϐ�Extern																*/
/*																									*/
/****************************************************************************************************/
#pragma ghs startzda
extern	OPPRMDAT	OpPrm;
#pragma ghs endzda



/****************************************************************************************************/
/*																									*/
/*		�W���p�����[�^�v�Z�֐�																		*/
/*																									*/
/****************************************************************************************************/
LONG	pncal_StdDummy( ULONG Cmd, ULONG Data, ULONG Digit );	/* �W���p�����[�^�v�ZDummy�֐�		*/



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^��`Extern�錾																	*/
/*																									*/
/****************************************************************************************************/
extern	const	PRMDEF	pndef_comCtrl;
extern	const	PRMDEF	pndef_sLimitSw;
extern	const	PRMDEF	pndef_cmdCtrlSw;
extern	const	PRMDEF	pndef_zpWidth;
extern	const	PRMDEF	pndef_p_sLimitL;
extern	const	PRMDEF	pndef_p_sLimitH;
extern	const	PRMDEF	pndef_n_sLimitL;
extern	const	PRMDEF	pndef_n_sLimitH;
extern	const	PRMDEF	pndef_absOffsetL;
extern	const	PRMDEF	pndef_absOffsetH;
extern	const	PRMDEF	pndef_accRate1;
extern	const	PRMDEF	pndef_accRate2;
extern	const	PRMDEF	pndef_accChgSpd;
extern	const	PRMDEF	pndef_decRate1;
extern	const	PRMDEF	pndef_decRate2;
extern	const	PRMDEF	pndef_decChgSpd;
extern	const	PRMDEF	pndef_filExpBias;
extern	const	PRMDEF	pndef_filExpTb;
extern	const	PRMDEF	pndef_filMaTb;
extern	const	PRMDEF	pndef_resrv2;
extern	const	PRMDEF	pndef_exDistL;
extern	const	PRMDEF	pndef_exDistH;
extern	const	PRMDEF	pndef_zretDir;
extern	const	PRMDEF	pndef_zretSpd1;
extern	const	PRMDEF	pndef_zretSpd2;
extern	const	PRMDEF	pndef_zretDistL;
extern	const	PRMDEF	pndef_zretDistH;
extern	const	PRMDEF	pndef_resrv3;
extern	const	PRMDEF	pndef_resrv4;
extern	const	PRMDEF	pndef_resrv5;
extern	const	PRMDEF	pndef_ioMonSel;
extern	const	PRMDEF	pndef_optBitSw;
extern	const	PRMDEF	pndef_p_ltLimitL;
extern	const	PRMDEF	pndef_p_ltLimitH;
extern	const	PRMDEF	pndef_n_ltLimitL;
extern	const	PRMDEF	pndef_n_ltLimitH;
extern	const	PRMDEF	pndef_opMon1;
extern	const	PRMDEF	pndef_opMon2;
extern	const	PRMDEF	pndef_decRate1Emg;
extern	const	PRMDEF	pndef_decRate2Emg;
extern	const	PRMDEF	pndef_decChgSpdEmg;
extern	const	PRMDEF	pndef_svOffTime;
extern	const	PRMDEF	pndef_optbit1;
extern	const	PRMDEF	pndef_optbit2;
extern	const	PRMDEF	pndef_optbit3;
extern	const	PRMDEF	pndef_optbit4;
extern	const	PRMDEF	pndef_optbit5;
extern	const	PRMDEF	pndef_resrv6;
extern	const	PRMDEF	pndef_rotrefL;
extern	const	PRMDEF	pndef_rotrefH;
extern	const	PRMDEF	pndef_st_num;
extern	const	PRMDEF	pndef_rateChgSw;
extern	const	PRMDEF	pndef_accRate12L;
extern	const	PRMDEF	pndef_accRate12H;
extern	const	PRMDEF	pndef_accRate22L;
extern	const	PRMDEF	pndef_accRate22H;
extern	const	PRMDEF	pndef_accChgSpd2L;
extern	const	PRMDEF	pndef_accChgSpd2H;
extern	const	PRMDEF	pndef_decRate12L;
extern	const	PRMDEF	pndef_decRate12H;
extern	const	PRMDEF	pndef_decRate22L;
extern	const	PRMDEF	pndef_decRate22H;
extern	const	PRMDEF	pndef_decChgSpd2L;
extern	const	PRMDEF	pndef_decChgSpd2H;
extern	const	PRMDEF	pndef_decRateEmg2L;
extern	const	PRMDEF	pndef_decRateEmg2H;
extern	const	PRMDEF	pndef_zretSpd12L;
extern	const	PRMDEF	pndef_zretSpd12H;
extern	const	PRMDEF	pndef_zretSpd22L;
extern	const	PRMDEF	pndef_zretSpd22H;
extern	const	PRMDEF	pndef_ltSeqNum;
extern	const	PRMDEF	pndef_ltLoopNum;
extern	const	PRMDEF	pndef_ltSeqSet1;
extern	const	PRMDEF	pndef_ltSeqSet2;
extern	const	PRMDEF	pndef_nodeAdr;
extern	const	PRMDEF	pndef_trByte;
extern	const	PRMDEF	pndef_comTCycle;
extern	const	PRMDEF	pndef_comAplCycle;
extern	const	PRMDEF	pndef_comErrCnt;
extern	const	PRMDEF	pndef_comErrSts1;
extern	const	PRMDEF	pndef_comErrSts2;
extern	const	PRMDEF	pndef_comDataCtrl;
extern	const	PRMDEF	pndef_almCmd0;
extern	const	PRMDEF	pndef_almCmd1;
extern	const	PRMDEF	pndef_almCmd2;
extern	const	PRMDEF	pndef_almCmd3;
extern	const	PRMDEF	pndef_almCmd4;
extern	const	PRMDEF	pndef_almCmd5;
extern	const	PRMDEF	pndef_almCmd6;
extern	const	PRMDEF	pndef_almCmd7;
extern	const	PRMDEF	pndef_almCmd8;
extern	const	PRMDEF	pndef_almCmd9;
extern	const	PRMDEF	pndef_almCmd10;
extern	const	PRMDEF	pndef_almCmd11;
extern	const	PRMDEF	pndef_almCmd12;
extern	const	PRMDEF	pndef_almCmd13;
extern	const	PRMDEF	pndef_almCmd14;
extern	const	PRMDEF	pndef_almCmd15;
extern	const	PRMDEF	pndef_almRsp0;
extern	const	PRMDEF	pndef_almRsp1;
extern	const	PRMDEF	pndef_almRsp2;
extern	const	PRMDEF	pndef_almRsp3;
extern	const	PRMDEF	pndef_almRsp4;
extern	const	PRMDEF	pndef_almRsp5;
extern	const	PRMDEF	pndef_almRsp6;
extern	const	PRMDEF	pndef_almRsp7;
extern	const	PRMDEF	pndef_almRsp8;
extern	const	PRMDEF	pndef_almRsp9;
extern	const	PRMDEF	pndef_almRsp10;
extern	const	PRMDEF	pndef_almRsp11;
extern	const	PRMDEF	pndef_almRsp12;
extern	const	PRMDEF	pndef_almRsp13;
extern	const	PRMDEF	pndef_almRsp14;
extern	const	PRMDEF	pndef_almRsp15;
extern	const	PRMDEF	pndef_comCmd0;
extern	const	PRMDEF	pndef_comCmd1;
extern	const	PRMDEF	pndef_comCmd2;
extern	const	PRMDEF	pndef_comCmd3;
extern	const	PRMDEF	pndef_comCmd4;
extern	const	PRMDEF	pndef_comCmd5;
extern	const	PRMDEF	pndef_comCmd6;
extern	const	PRMDEF	pndef_comCmd7;
extern	const	PRMDEF	pndef_comCmd8;
extern	const	PRMDEF	pndef_comCmd9;
extern	const	PRMDEF	pndef_comCmd10;
extern	const	PRMDEF	pndef_comCmd11;
extern	const	PRMDEF	pndef_comCmd12;
extern	const	PRMDEF	pndef_comCmd13;
extern	const	PRMDEF	pndef_comCmd14;
extern	const	PRMDEF	pndef_comCmd15;
extern	const	PRMDEF	pndef_comRsp0;
extern	const	PRMDEF	pndef_comRsp1;
extern	const	PRMDEF	pndef_comRsp2;
extern	const	PRMDEF	pndef_comRsp3;
extern	const	PRMDEF	pndef_comRsp4;
extern	const	PRMDEF	pndef_comRsp5;
extern	const	PRMDEF	pndef_comRsp6;
extern	const	PRMDEF	pndef_comRsp7;
extern	const	PRMDEF	pndef_comRsp8;
extern	const	PRMDEF	pndef_comRsp9;
extern	const	PRMDEF	pndef_comRsp10;
extern	const	PRMDEF	pndef_comRsp11;
extern	const	PRMDEF	pndef_comRsp12;
extern	const	PRMDEF	pndef_comRsp13;
extern	const	PRMDEF	pndef_comRsp14;
extern	const	PRMDEF	pndef_comRsp15;
extern	const	PRMDEF	pndef_OpCardID;
extern	const	PRMDEF	pndef_OpCardCtrlSw;
extern	const	PRMDEF	pndef_OpCardSwVerL;
extern	const	PRMDEF	pndef_OpCardSwVerH;
extern	const	PRMDEF	pndef_OpCardYNo;
extern	const	PRMDEF	pndef_OpCardDate;
extern	const	PRMDEF	pndef_OpCardSvSw;
extern	const	PRMDEF	pndef_OpCardAlmCode;
extern	const	PRMDEF	pndef_OpCardAlmAttr;
extern	const	PRMDEF	pndef_OpCardModel0;
extern	const	PRMDEF	pndef_OpCardModel1;
extern	const	PRMDEF	pndef_OpCardModel2;
extern	const	PRMDEF	pndef_OpCardModel3;
extern	const	PRMDEF	pndef_OpCardModel4;
extern	const	PRMDEF	pndef_OpCardModel5;
extern	const	PRMDEF	pndef_OpCardModel6;
extern	const	PRMDEF	pndef_OpCardModel7;
extern	const	PRMDEF	pndef_OpCardSNo0;
extern	const	PRMDEF	pndef_OpCardSNo1;
extern	const	PRMDEF	pndef_OpCardSNo2;
extern	const	PRMDEF	pndef_OpCardSNo3;
extern	const	PRMDEF	pndef_OpCardSNo4;
extern	const	PRMDEF	pndef_OpCardSNo5;
extern	const	PRMDEF	pndef_OpCardSNo6;
extern	const	PRMDEF	pndef_OpCardSNo7;
extern	const	PRMDEF	pndef_bnkNum;
extern	const	PRMDEF	pndef_bnkMemNum;
extern	const	PRMDEF	pndef_bnkMember0;
extern	const	PRMDEF	pndef_bnkMember1;
extern	const	PRMDEF	pndef_bnkMember2;
extern	const	PRMDEF	pndef_bnkMember3;
extern	const	PRMDEF	pndef_bnkMember4;
extern	const	PRMDEF	pndef_bnkMember5;
extern	const	PRMDEF	pndef_bnkMember6;
extern	const	PRMDEF	pndef_bnkMember7;
extern	const	PRMDEF	pndef_bnkMember8;
extern	const	PRMDEF	pndef_bnkMember9;
extern	const	PRMDEF	pndef_bnkMemberA;
extern	const	PRMDEF	pndef_bnkMemberB;
extern	const	PRMDEF	pndef_bnkMemberC;
extern	const	PRMDEF	pndef_bnkMemberD;
extern	const	PRMDEF	pndef_bnkMemberE;
extern	const	PRMDEF	pndef_bnkData00;
extern	const	PRMDEF	pndef_bnkData01;
extern	const	PRMDEF	pndef_bnkData02;
extern	const	PRMDEF	pndef_bnkData03;
extern	const	PRMDEF	pndef_bnkData04;
extern	const	PRMDEF	pndef_bnkData05;
extern	const	PRMDEF	pndef_bnkData06;
extern	const	PRMDEF	pndef_bnkData07;
extern	const	PRMDEF	pndef_bnkData08;
extern	const	PRMDEF	pndef_bnkData09;
extern	const	PRMDEF	pndef_bnkData0A;
extern	const	PRMDEF	pndef_bnkData0B;
extern	const	PRMDEF	pndef_bnkData0C;
extern	const	PRMDEF	pndef_bnkData0D;
extern	const	PRMDEF	pndef_bnkData0E;
extern	const	PRMDEF	pndef_bnkData0F;
extern	const	PRMDEF	pndef_bnkData10;
extern	const	PRMDEF	pndef_bnkData11;
extern	const	PRMDEF	pndef_bnkData12;
extern	const	PRMDEF	pndef_bnkData13;
extern	const	PRMDEF	pndef_bnkData14;
extern	const	PRMDEF	pndef_bnkData15;
extern	const	PRMDEF	pndef_bnkData16;
extern	const	PRMDEF	pndef_bnkData17;
extern	const	PRMDEF	pndef_bnkData18;
extern	const	PRMDEF	pndef_bnkData19;
extern	const	PRMDEF	pndef_bnkData1A;
extern	const	PRMDEF	pndef_bnkData1B;
extern	const	PRMDEF	pndef_bnkData1C;
extern	const	PRMDEF	pndef_bnkData1D;
extern	const	PRMDEF	pndef_bnkData1E;
extern	const	PRMDEF	pndef_bnkData1F;
extern	const	PRMDEF	pndef_bnkData20;
extern	const	PRMDEF	pndef_bnkData21;
extern	const	PRMDEF	pndef_bnkData22;
extern	const	PRMDEF	pndef_bnkData23;
extern	const	PRMDEF	pndef_bnkData24;
extern	const	PRMDEF	pndef_bnkData25;
extern	const	PRMDEF	pndef_bnkData26;
extern	const	PRMDEF	pndef_bnkData27;
extern	const	PRMDEF	pndef_bnkData28;
extern	const	PRMDEF	pndef_bnkData29;
extern	const	PRMDEF	pndef_bnkData2A;
extern	const	PRMDEF	pndef_bnkData2B;
extern	const	PRMDEF	pndef_bnkData2C;
extern	const	PRMDEF	pndef_bnkData2D;
extern	const	PRMDEF	pndef_bnkData2E;
extern	const	PRMDEF	pndef_bnkData2F;
extern	const	PRMDEF	pndef_bnkData30;
extern	const	PRMDEF	pndef_bnkData31;
extern	const	PRMDEF	pndef_bnkData32;
extern	const	PRMDEF	pndef_bnkData33;
extern	const	PRMDEF	pndef_bnkData34;
extern	const	PRMDEF	pndef_bnkData35;
extern	const	PRMDEF	pndef_bnkData36;
extern	const	PRMDEF	pndef_bnkData37;
extern	const	PRMDEF	pndef_bnkData38;
extern	const	PRMDEF	pndef_bnkData39;
extern	const	PRMDEF	pndef_bnkData3A;
extern	const	PRMDEF	pndef_bnkData3B;
extern	const	PRMDEF	pndef_bnkData3C;
extern	const	PRMDEF	pndef_bnkData3D;
extern	const	PRMDEF	pndef_bnkData3E;
extern	const	PRMDEF	pndef_bnkData3F;
extern	const	PRMDEF	pndef_prmVer;
extern	const	PRMDEF	pndef_spdDivision;
extern	const	PRMDEF	pndef_CmdOpSw;
extern	const	PRMDEF	pndef_sys_rsrv2;
extern	const	PRMDEF	pndef_sys_rsrv3;
extern	const	PRMDEF	pndef_cmdopv;
/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/
