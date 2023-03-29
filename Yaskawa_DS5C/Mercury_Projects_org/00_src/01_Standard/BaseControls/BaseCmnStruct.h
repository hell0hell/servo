#ifndef	_COMMON_STR_H /* �ēǂݍ��ݖh�~�p */
//****************************************************************************************************
//
//
//		BaseCmnStruct.h : Local Header File for Common Structure
//
//
//****************************************************************************************************
//
//
//************** Copyright (C) Yaskawa Electric Corporation ******************************************
#include "ServoIf.h"

/*--------------------------------------------------------------------------------------------------*/
/*		��{���䃂�[�h��` : ��ʃo�C�g�̓[�����w�肷�邱��											*/
/*--------------------------------------------------------------------------------------------------*/
#define	CTRLMODE_SPD		0x0000			/* ���x���䃂�[�h(�A�i���O�w��)							*/
#define	CTRLMODE_POS		0x0001			/* �ʒu���䃂�[�h(�p���X��w��)							*/
#define	CTRLMODE_TRQ		0x0002			/* �g���N���䃂�[�h(�A�i���O�w��)						*/
#define	CTRLMODE_JOG		0x0003			/* �i�n�f�^�]���[�h										*/
#define CTRLMODE_ZSRCH		0x0004			/* ���_�T�[�`���[�h										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�g�����䃂�[�h��` : ���ʃo�C�g�͊�{���䃂�[�h���w�肷�邱��								*/
/*--------------------------------------------------------------------------------------------------*/
#define CTRLMODE_CSPD		0x0100			/* �����ݒ葬�x���䃂�[�h(�ړ_�w��)						*/
#define CTRLMODE_PJOG		0x0101			/* �v���O�����i�n�f�^�]���[�h							*/
#define CTRLMODE_NOCMD		0xFFFF			/* ���䃂�[�h�w��Ȃ�(ScanC��p)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		�[�����䃂�[�h��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CTRLMODE_ZNONE		0x00			/* �[������Ȃ�											*/
#define CTRLMODE_ZSTOP		0x01			/* �[������~���[�h										*/
#define CTRLMODE_LDSTOP		0x02			/* ���������x�ɂ�錸����~���[�h						*/ /* <S1F5> */
#define CTRLMODE_ZCLMP		0x03			/* �[���N�����v���[�h									*/ /* <S1F5> */
// #define CTRLMODE_ZCLMP		0x02			/* �[���N�����v���[�h									*/ /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------*/
/*		���䃂�[�h��`																				*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef __BIG_ENDIAN__
typedef union CTRLMD {						/* ���䃂�[�h�\���̒�` (4byte�ȓ�)						*/
	struct	{								/* ���䃂�[�h�����o��`									*/
		UCHAR	ot;							/* �n�s�������t���O										*/
		UCHAR	zm;							/* �[�����䃂�[�h										*/
		UCHAR	cx;							/* �g�����䃂�[�h										*/
		UCHAR	cm;							/* ��{���䃂�[�h										*/
	} b;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* ���䃂�[�h�ҏW�p(Copy,etc)							*/
	struct  {
		USHORT	uwd_h;						/* ���䃂�[�h�ҏW�p(Copy,etc)							*/
		USHORT	uwd_l;						/* ���䃂�[�h�ҏW�p(Copy,etc)							*/
	} us;
} CTRLMD;
#else  /* Little Endian */
typedef union CTRLMD {						/* ���䃂�[�h�\���̒�` (4byte�ȓ�)						*/
	struct	{								/* ���䃂�[�h�����o��`									*/
		UCHAR	cm;							/* ��{���䃂�[�h										*/
		UCHAR	cx;							/* �g�����䃂�[�h										*/
		UCHAR	zm;							/* �[�����䃂�[�h										*/
		UCHAR	ot;							/* �n�s�������t���O										*/
	} b;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* ���䃂�[�h�ҏW�p(Copy,etc)							*/
	struct  {
		USHORT	uwd_l;						/* ���䃂�[�h�ҏW�p(Copy,etc)							*/
		USHORT	uwd_h;						/* ���䃂�[�h�ҏW�p(Copy,etc)							*/
	} us;
} CTRLMD;
#endif /* __BIG_ENDIAN__ */


/*--------------------------------------------------------------------------------------------------*/
/*		�[�����䃂�[�h��`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ZCTRMD {						/* �[�����䃂�[�h�\���̒�` (4byte�ȓ�)					*/
	UCHAR	zm;								/* �[�����䃂�[�h										*/
	UCHAR	zmchg;							/* �[�����䃂�[�h�ω����								*/
	UCHAR	spare0;							/* �\��													*/
	UCHAR	spare1;							/* �\��													*/
} ZCTRMD;


typedef struct CTRLMD_SET {					/* ���䃂�[�h�֘A��`									*/
	CTRLMD			CtrlMode;				/* Control Mode											*/
	CTRLMD			CtrlModeLst;			/* Last Control Mode									*/
	CTRLMD			CtrlModeChg;			/* Control Mode Change Info (XOR�ō쐬)					*/
	ZCTRMD			ZctrlMode;				/* Zero Control Mode									*/
} CTRLMD_SET;



/*--------------------------------------------------------------------------------------------------*/
/*	���x���o�ړ����σt�B���^		<V110>															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MOTSPDMAFIL {
	KSGAIN	mabuf[16];			/* �ړ����ϗp�o�b�t�@	[2^24/OvrSpd]						<S00A>	*/
	LONG	mabufSumx;			/* �ړ����ϗp�o�b�t�@�ώZ�l											*/
	LONG	idx;				/* �o�b�t�@�C���f�b�N�X												*/
	LONG	manumBit;			/* �ړ����ω�(2�̗ݏ�)											*/
	LONG	defaultNumBit;		/* �ړ����ω�(2�̗ݏ�)�̏����l									*/
	LONG	ksub;				/* ���Z�ʉ��Z�Q�C��(0/1)											*/
} MOTSPDMAFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		Kernel Mode Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum FUN_KNLMD_T {
	KPI_KMD_NORMAL		  = 0x00,		/* �ʏ탂�[�h												*/
	KPI_KMD_ADJREFAD		,			/* �w�߂`�c������											*/
	KPI_KMD_ADJCURAD		,			/* �d�����o�`�c������										*/
	KPI_KMD_SOFTRESET		,			/* �\�t�g���Z�b�g��											*/
	KPI_KMD_PARAMRESET		,			/* �p�����[�^���Z�b�g��										*/
	KPI_KMD_PARAMCPY		,			/* <S031> �p�����[�^�R�s�[									*/
	KPI_KMD_ADJANLGMON		,			/* �A�i���O���j�^������										*/	/* <S0A9> */
	KPI_KMD_PBLEDTEST		,			/* �v�b�V���{�^�� & 7SegLed�m�F������						*/	/* <S0CB>  */
	KPI_KMD_ADJGAIN			,			/* �Q�C��������												*/	/* <S0CB>  */
	KPI_KMD_ANLGMONVOLTTEST	,			/* �A�i���O���j�^�o�͓d��������								*/	/* <S0FD>  */
	KPI_KMD_RIPPLECOMPWRITE	,			/* ���x���b�v���⏞��񏑂����ݒ�							*/	/* <S14B>  */
} FUN_KNLMD_T;


/*--------------------------------------------------------------------------------------------------*/
/*		Fn�@�\���� �T�[�{����f�[�^																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FUN_CMN_CONTROL {
	volatile BOOL	FnSvControl;			/* ROUND�^�]��											*/
	LONG			FnCtrlMcmd;				/* ROUND�^�]�ł̃R���g���[�����[�h						*/
	BOOL			FnSvonReq;				/* ROUND�^�]�ł̃T�[�{�I���v��							*/
	BOOL			FnSensOnReq;			/* ROUND�^�]�ł̃T�[�{�I���v��							*/
	BOOL			PdetReq;				/* ���Ɍ��o�v��(�ʓd�v��)								*/
	BOOL			forcedAlmReady;			/* ����AlmReady�t���O									*//* <S215> */
	LONG			JogSpeed;				/* JOG���x�w�� +:���]�A-:�t�]							*/
	USHORT			SafeStopTime;			/* JOG���S��~����										*/
	SHORT			OperatorSts;			/* �I�y���[�^�\���X�e�[�^�X		*//* <S085> */
	BOOL			SvOnPdetReq;			/* �T�[�{�I�����Ɍ��o�v��					 	<S21C> 	*/

	USHORT			ForceOutSignals;		/* �����M���o�͎w��										*/
	USHORT			ForceOutSignalsHW;		/* �����M���o�͎w��(�g�v�o�͐M���ɔ��f)					*/	/* <S0CB> */
	UCHAR			OutModeFlg;				/* �����o�̓��[�h�t���O									*/	/* <S0CB> */

	ULONG			LightIntervalTime;		/* 7SegLed�_������[100ms]								*/	/* <S0CB> */
	UCHAR			PbAndLedChkTestFlag;	/* �v�b�V���{�^��&7SegLed�m�F�t���O						*/	/* <S0CB> */
	USHORT			AnlgLedChkInitFlag;		/* AP�` �v�b�V���{�^��&7SegLed�m�F�J�n�t���O			*/	/* <S0CB> */
	UINT			AnlgLedChkCnt;			/* AP�` �v�b�V���{�^��&7SegLed�m�F �J�E���^				*/	/* <S0CB> */
	UINT			AnlgLedChkCnt2;			/* AP�` �v�b�V���{�^��&7SegLed�m�F �J�E���^(������LED)	*/	/* <S0CB> */
	UINT			AnlgLedChkLstPnlBtn;	/* AP�` �v�b�V���{�^��&7SegLed�m�F �O�񉟉��{�^��		*/	/* <S0CB> */
	CHAR			AnlgMonVoltTstFlag;		/* �A�i���O���j�^�o�͓d���m�F�������t���O				*/	/* <S0FD> */
	USHORT			AnlgMonVoltTstChSel;	/* �A�i���O���j�^�o�͓d���m�F���� Ch�I��				*/	/* <S0FD> */
	SHORT			OutputVoltVal_Ch0;		/* �A�i���O���j�^�o�͓d���m�F���� �o�͓d���ݒ�(Ch0)		*/	/* <S0FD> */
	SHORT			OutputVoltVal_Ch1;		/* �A�i���O���j�^�o�͓d���m�F���� �o�͓d���ݒ�(Ch1)		*/	/* <S0FD> */
	FUN_KNLMD_T		KnlMode;				/* Tester mode											*/	/* <S0CB> */

	//���_�T�[�`���샌�W�X�^��`�ꏊ�͂�����OK�H�H
	struct {
		USHORT	ZsrchState;					/* 0x2101 : 0x030: ���_�T�[�`����						*/
	} OpeReg;

	struct {
		BOOL		FnSvCtrlDisable;		/* Fn�^�]�s���										*/
		BOOL		LockNormalOpeDrive;		/* Fn�^�]��A�ʏ�^�]�֎~�t���O							*/

		BOOL		TuningRun;				/* �T�[�{������											*/
		BOOL		TuningPctrlRun;			/* �T�[�{����P���䒆									*/
	} State;

	struct {								/* <S031> Flags */
		UCHAR PrmCopy;						/* <S031> Lcd Operator Parameter copy flag */
		UCHAR Reserved0;					/* <S031> reserved */
		UCHAR Reserved1;					/* <S031> reserved */
		UCHAR Reserved2;					/* <S031> reserved */
	} f;
} FUN_CMN_CONTROL;


/*--------------------------------------------------------------------------------------------------*/
/*		���䃋�[�v�o�̓f�[�^�\����																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CTRL_LOOP_OUT {
//	LONG			MotPos;					/* ���[�^�ʒu						[Pulse]				*/
	LONG			MotPosErr;				/* �ʒu�A���v�΍�					[Pulse]				*/
	LONG			SpdRefo;				/* ���x�w�ߏo��						[2^24/OvrSpd]		*/
	LONG			SpdFFC;					/* ���xFF�⏞						[2^24/OvrSpd]		*/
	LONG			MotSpd_a;				/* ���[�^���x���ϒl					[2^24/OvrSpd]		*//* 2013/04/01��MD�ł́A���ı�Ă���Ă���	*/
	LONG			TrqRefo;				/* �g���N�w�ߏo��					[2^24/MaxTrq]		*/
	LONG			TrqRefo_a;				/* �g���N�w�ߏo��(���ϒl)			[2^24/MaxTrq]		*/
	LONG			TrqRefMon;				/* �g���N�w�߃��j�^					[2^24/MaxTrq]		*/
	LONG			TrqRefMon_a;			/* �g���N�w�߃��j�^					[2^24/MaxTrq]		*/
	SHORT			IdRefMon;				/* d���d���w�߃��j�^				[15000/Imax]		*/
	SHORT			IqRefMon;				/* q���d���w�߃��j�^				[15000/Imax]		*/
	SHORT			IqRefMon_a;				/* q���d���w�߃��j�^(���ϒl)		[15000/Imax]		*/
	SHORT			IqRefMon_l;				/* q���d���w�߃��j�^(�O��l)		[15000/Imax]		*/
	LONG			IqFbMon;				/* q���d��FB���j�^										*/
	LONG			IdFbMon;				/* d���d��FB���j�^										*/
	LONG			IqRefFilMon;			/* q���d���w�߃��j�^(�t�B���^��)						*/
//	BOOL			PConActFlg;				/* P�����Ԋm�F�t���O									*//* <S064> */
	BOOL			SvonSpdLmtFlag;			/* �T�[�{ON�����x�������t���O							*/
//	BOOL			SpdClamp;				/* �g���N���䎞���x�������t���O							*//* <S04B> */
	BOOL			TrqClamp;				/* �g���N�������t���O									*/
} CTRL_LOOP_OUT;


/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X����o�̓f�[�^�\����																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct BASE_CTRL_OUT {
	CTRLMD			CtrlModeOut;			/* ���䃂�[�h(ScanB�o��)								*/
	LONG			TrqCtrlRef;				/* �g���N���䎞�̃g���N�w��				[2^24/MaxTrq]	*/
	LONG			SpdCtrlRef;				/* ���x���䎞�̑��x�w��					[2^24/OvrSpd]	*/
	LONG			OutputSpdRef;			/* ���x�w�ߏo�͒l(ScanB��ScanC)			[2^24/OvrSpd]	*/
//	LONG			SpdRefSum;				/* ���x�w�ߏo�͒l(Ref+FF)				[2^24/OvrSpd]	*/
	LONG			MotPos;					/* ���[�^�ʒu(ScanB�ώZ�l)				[32bitRingPulse]*/
	LONG			FencPos;				/* ���[�^�ʒu(ScanB�ώZ�l)				[32bitRingPulse]*/
	LONG			PcmdCntr;				/* �w�߃p���X�J�E���g�l									*/
	LONG			MotSpdSum;				/* ���[�^���x���Z�l(ScanC���ϑ��x���Z�p)				*/
	LONG			MotSpdCnt;				/* ScanC���ϑ��x���Z�pScanB�J�E���^						*/
	LONG			PositionError;			/* �ʒu�΍�												*/
	LONG			SvonSpdLmtReq;			/* �T�[�{ON�����x�����v��								*/
	LONG			CmdSeqBit;				/* �V�[�P���X����w��(ScanB��ScanC)						*/
	BOOL			TblZclamp;				/* �e�[�u���^�]�[���N�����v���t���O						*/
	LONG			IdRefMon;				/* d���d���w�߃��j�^				[15000/Imax]		*/
	LONG			IqRefMon;				/* q���d���w�߃��j�^				[15000/Imax]		*/
	LONG			TrqRefMon;				/* �g���N�w�߃��j�^					[2^24/MaxTrq]		*/
} BASE_CTRL_OUT;


typedef	struct SEQ_CTRL_OUT {
/* Request commands to the Control module */
	CTRLMD			CtrlModeReq;			/* ScanC Control Mode�v��								*/
	VBOOL			BaseEnableReq;			/* ScanC�쐬�x�[�X�C�l�[�u���v��						*//* <S193> */
	BOOL			BeComplete;				/* �x�[�X�C�l�[�u���������								*/
	BOOL			UdlBeStatus;			/* �x�[�X�C�l�[�u���������(UDL����ԃ��j�^)			*/
	BOOL			BBSvcRefClrReq;			/* Servo Control Ref. Clear Request Flag				*/
	BOOL			TrqLimitReq;			/* OT�g���N�����v��(OT�V�[�P���X�����o��)				*/
	LONG			ModeSwAcc;				/* Motor Acceleration fow ModeSw		[NorSpd/ScanC]	*/

/* Sequence Status */
	BOOL			HwbbSts;				/* HWBB���												*/
	BOOL			FstpSts;				/* FSTP���												*/ /* <S1F5> */
	BOOL			MainPowerOn;			/* ���HON���											*/
	BOOL			MotStop;				/* ���[�^��~���										*/
	BOOL			OverTrvlSts;			/* OT���(OT�V�[�P���X�����o��)							*/
	BOOL			SvRdyPhaseIgnore;		/* Servo Ready (Ignore Phase)							*/

	BOOL			PotSigSts;				/* ���]�쓮�֎~���										*/
	BOOL			NotSigSts;				/* �t�]�쓮�֎~���										*/
	BOOL			BrkSigSts;				/* �u���[�L������										*/
	BOOL			EstpSigSts;				/* E-STOP���											*/

	BOOL			TgOnSts;				/* ���[�^��]���										*/
	BOOL			VcmpSts;				/* ���x��v���											*/
	BOOL            MotSuspLvl;                /*Motor Suspend Level Flag <S201> */
	BOOL			RndReady;				/* 2013/04/01MD�łł́ARndReady�͂Ȃ��B�b��I�ɁA�����Œ�`����	*/

	LONG			RspSeqBits;				/* �V�[�P���X������(ALM/WRN/SVON/SENON/PDET/BK/SRDY/MRDY)*/
	LONG			RspSigBits;				/* ���͐M�����(HWBB/P-OT/N-OT)							*/
	LONG			RspCtrlBits;			/* ������(HBB/TGON/VCMP)								*/
/*--------------------------------------------------------------------------------------------------*/
	struct 	{								/* AP	<S00C>											*/
		UINT	Vrefon              :1;		/* 0x10 : Speed  Reference Input On						*/
		UINT	Trefon              :1;		/* 0x11 : Torque Reference Input On						*/
		UINT	Prefon              :1;		/* 0x12 : Pulse  Reference Input On						*/
		UINT	PerrClr             :1;		/* 0x13 : PosErr Clear     Input On						*/
		UINT	PrefSign            :1;		/* 0x14 : Pulse  Reference Sign  (plus:1, minus:0)		*/
	}f;
/*--------------------------------------------------------------------------------------------------*/
	UINT	AlmRstLast;						/* �A���[�����Z�b�g�M���O��l @@ �ǂ����Ɉړ�			*/
/*--------------------------------------------------------------------------------------------------*/
} SEQ_CTRL_OUT;


typedef	struct ANLG_UN_MON_DATA {
/* �b�菈�� 2013/04/01MD�́A���ı�Ă��Ă���B	*/
	LONG			AmonSpdRef;				/* AnaMon : ���x�w��(���x�e�e��)	[2^24/OvrSpd]		*/
	LONG			AmonTrqRef;				/* AnaMon : �g���N�w��(�d�͕⏞��)	[2^24/MaxTrq]		*/
/* �b�菈�� 2013/04/01MD�́A���ı�Ă��Ă���B	*/

	LONG			UnMotSpd;				/* Un000  : ���[�^���x				[pulse/64ms]		*/
	LONG			UnMotSpdx;				/* Un000  : ���[�^���x(2ms���ϑ��x)	[2^24/OvrSpd]		*/
	LONG			UnPfbk64ms;				/* Un000  : ���[�^���x				[pulse/64ms]		*/
	LONG			UnSpdRef;				/* Un001  : ���x�w��				[2^24/OvrSpd]		*/
	LONG			UnTrqRef;				/* Un002  : �g���N�w��				[2^24/MaxTrq]		*/
	LONG			UnMotAngle;				/* Un003  : ��]�p�P(�p���X)		[pulse]				*/
	LONG			UnMotPhase;				/* Un004  : ��]�p�Q(�d�C�p)		[deg]				*/
	LONG			UnPcmd64ms;				/* Un007  : �w�߃p���X���x			[pulse/64ms]		*/
	LONG			UnIsqrSum;				/* Un009  : �ݐϕ��ח�				[(15000/MaxTrq)/10sec]*/
	LONG			UnMeanRgPower;			/* Un00A  : �񐶏���d�͕��ϒl		[%]					*/
	LONG			UnMeanDbPower;			/* Un00B  : �c�a����d�͕��ϒl 		[%]					*/
	LONG			UnPcmdCntr;				/* Un00C  : �w�߃p���X�J�E���^		[pulse]				*/
	LONG			UnPfbkCntr;				/* Un00D  : �e�a�p���X�J�E���^		[pulse]				*/
	LONG			UnPfbkCntr2;			/* Un013  : �e�a�p���X�J�E���^		[�w�ߒP��]			*//* <S056> */
	LONG			RemCntr2;				/* Un013  : �e�a�p���X�J�E���^�̗]��[�w�ߒP��]			*//* <S056> */
	LONG			UnInstEnvMonAmp;		/* Un025 : �ݒu�����j�^(2s�X�V)	[0.001%]			*//* <S1D7> */
	LONG			UnInstEnvMonMot;		/* Un026 : �ݒu�����j�^(2s�X�V)	[0.001%]			*//* <S1D7> */
	ULONG			UnCrntDet;				/* Un141 : �Q��d���e�a�l			[(15000/MAXTRQ)^2]	*/
	ULONG			UnCrntRef;				/* Un142 : �Q��d���w�ߒl			[(15000/MAXTRQ)^2]	*/
	LONG			UnRgPower;				/* Un143  : �񐶏���d�͒l			[%]					*/
	LONG			UnDbPower;				/* Un144  : �c�a����d�͒l 			[%]					*/
	ULONG			UnIsqrSumMax;			/* Un145 : �ݐϕ��ח��ő�l			[(15000/MaxTrq)/10sec]*/

	LONG			UnOLRate;				/* Un1F0  : �ߕ��׃��x����			[%]					*/
	LONG			UnOLType;				/* Un1F1  : �ߕ��׎�� (0�`4)		[]					*/
											/*			0: �ߕ��ׂȂ� 1:���[�^�A��  2:���[�^�u��  	*/
											/*						  3:�A���v�A��  4:�A���v�u��	*/
	LONG			TrcRgPower;				/* Trace  : �񐶏���d�͒l			[%]					*/
	LONG			TrcDbPower;				/* Trace  : �c�a����d�͒l 			[%]					*/

	LONG			UnPcmd64Sum;			/* �w�߃p���X���x <S1B6>			[64ms]				*/

	USHORT			SvStatus;				/* �T�[�{�X�e�[�^�X���j�^								*//* <S049> */
	USHORT			LedDispStatus;			/* �l�b�g���[�N7Seg�\���X�e�[�^�X						*//* <S0A1> */

} ANLG_UN_MON_DATA;


/***************************************** end of file **********************************************/
#define	_COMMON_STR_H
#endif
