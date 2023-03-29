/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PnlOpAp.c : Panel Operator Interface Module (Analog & Pulse)								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	�@ �\ : �p�l���I�y���[�^����̊e�푀������s����B												*/
/*																									*/
/*			1)���j�^�@�\ : �w�胂�j�^�ϐ�(UnXXX)�̕\��												*/
/*			2)�p�����[�^ : �w��p�����[�^(PnXXX)�̕\��,�ҏW,�ύX									*/
/*			3)�⏕�@�\	 : �w��⏕�@�\(FnXXX)�̎��s�Ǝ��s���ʂ̕\��								*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2002.08.20  M.Suga																*/
/*		Rev.1.10 : 2008.04.24  T.Taniguchi															*/
/*		Rev.1.20 : 2013.08.02  K.Ozaki			##Mercury##											*/
/*		Rev.1.30 : 2013.09.26  K.Ozaki		<S00C>													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"		    /*#include "Round.h"*/
#include "LcdOp.h"			/* LcdOpIf Common Header */
#include "Monitor.h"		/* Un Monitor */
#include "Parameter.h"		/* Parameter */
#include "Function.h"		/* Fn Funstion */
#include "PrmAccessIf.h"	/* Parameter Edit */


/****************************************************************************************************/
/*																									*/
/*		Panel Operator Macro Definition																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator LED Codes																			*/
/*																									*/
/* (digit location)        (7seg location)															*/
/*    bit:0																							*/
/*       --                 _   _   _   _   _ 														*/
/*    5 |  | 1             |_| |_| |_| |_| |_|														*/
/*       -- 6              |_| |_| |_| |_| |_|														*/
/*    4 |  | 2      led[?]: 4   3   2   1   0 														*/
/*       -- * 7																						*/
/*       3  logic			0:ON / 1:OFF															*/
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_B0		0x01
#define	LEDCODE_B1		0x02
#define	LEDCODE_B2		0x04
#define	LEDCODE_B3		0x08
#define	LEDCODE_B4		0x10
#define	LEDCODE_B5		0x20
#define	LEDCODE_B6		0x40
#define	LEDCODE_B7		0x80
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_ZERO	0x3F
#define	LEDCODE_1		0x06
#define	LEDCODE_2		0x5B
#define	LEDCODE_3		0x4F
#define	LEDCODE_4		0x66
#define	LEDCODE_5		0x6D
#define	LEDCODE_6		0x7D
#define	LEDCODE_7		0x27
#define	LEDCODE_8		0x7F
#define	LEDCODE_9		0x6F
#define	LEDCODE_A		0x77
#define	LEDCODE_B		0x7C
#define	LEDCODE_C		0x39
#define	LEDCODE_D		0x5E
#define	LEDCODE_E		0x79
#define	LEDCODE_F		0x71
#define	LEDCODE_G		0x3D
#define	LEDCODE_H		0x76
#define	LEDCODE_sH		0x74
#define	LEDCODE_J		0x0E
#define	LEDCODE_L		0x38
#define	LEDCODE_N		0x54
#define	LEDCODE_sO		0x5C
#define	LEDCODE_P		0x73
#define	LEDCODE_Q		0x67
#define	LEDCODE_R		0x50
#define	LEDCODE_S		0x6D		/* <S1F5> */
#define	LEDCODE_T		0x78
#define	LEDCODE_U		0x3E
#define	LEDCODE_sU		0x1C
#define	LEDCODE_Y		0x6E
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_MINUS	0x40
#define	LEDCODE_POINT	0x80
#define	LEDCODE_BLANK	0x00
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator Buttons (Already Inverted)														*/
/*--------------------------------------------------------------------------------------------------*/
#define PNLKEY_MODE		0x1
#define PNLKEY_UP		0x2
#define PNLKEY_DOWN		0x4
#define PNLKEY_SHUP		0x8
#define PNLKEY_RST		(PNLKEY_UP | PNLKEY_DOWN)
#define PNLKEY_SWOFF	0x0
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator Display Modes																		*/
/*--------------------------------------------------------------------------------------------------*/
#define PNLMODE_STATUS		0
#define PNLMODE_FNNUM		1
#define PNLMODE_FNEXE		2
#define PNLMODE_PNNUM		3
#define PNLMODE_PNDATA		4
#define PNLMODE_UNNUM		5
#define PNLMODE_UNDATA		6
/*--------------------------------------------------------------------------------------------------*/
#define	PNLLED_BLINK		0xFF		/* �p�l��LED�����}�X�N(�_�ŗp)								*/
#define	PNLLED_NOBLINK		0x00		/* �p�l��LED�_���}�X�N(�_�ŗp)								*/
/*--------------------------------------------------------------------------------------------------*/
#define	PNLMON_NOSET		0xFFF		/* �d�����������j�^�f�t�H���g								*/
/*--------------------------------------------------------------------------------------------------*/

/* <S0CB> Start */
/*--------------------------------------------------------------------------------------------------*/
/* �v�b�V���{�^���E7LED-LED�m�F���[�h�p 7LED�\����`												*/
/*--------------------------------------------------------------------------------------------------*/
#define	LED_CHK_CNT_LMIT	15U
#define	PATTERN0			LEDCODE_BLANK																			/* �SLED����		*/
#define	PATTERN1			LEDCODE_B0																				/* Bit0 �_��		*/
#define	PATTERN2			LEDCODE_B0|LEDCODE_B1																	/* Bit0 - Bit1 �_��	*/
#define	PATTERN3			LEDCODE_B0|LEDCODE_B1|LEDCODE_B2														/* Bit0 - Bit2 �_��	*/
#define	PATTERN4			LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3												/* Bit0 - Bit3 �_��	*/
#define	PATTERN5			LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4									/* Bit0 - Bit4 �_��	*/
#define	PATTERN6			LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5						/* Bit0 - Bit5 �_��	*/
#define	PATTERN7			LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6			/* Bit0 - Bit6 �_��	*/
#define	PATTERN8			LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6|LEDCODE_B7	/* Bit0 - Bit7 �_��	*/

const UCHAR LedLightPatternTbl[] =
{	/*	0,			1,			2,			3,			*/
		PATTERN1,	PATTERN2,	PATTERN3,	PATTERN4,
	/*	4,			5,			6,			7			*/
		PATTERN5,	PATTERN6,	PATTERN7,	PATTERN8,

	/*	8,			9,			10,			11,			*/
		PATTERN7,	PATTERN6,	PATTERN5,	PATTERN4,
	/*	12,			13,			14,			15			*/
		PATTERN3,	PATTERN2,	PATTERN1,	PATTERN0
};
/* <S0CB> End */

/****************************************************************************************************/
/*																									*/
/*		Panel Operator Constant Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LED Code Table																				*/
/*--------------------------------------------------------------------------------------------------*/
const	UCHAR	ledcodetbl[] =
{
	LEDCODE_ZERO, LEDCODE_1, LEDCODE_2, LEDCODE_3, LEDCODE_4, LEDCODE_5, LEDCODE_6, LEDCODE_7,
	LEDCODE_8,	  LEDCODE_9, LEDCODE_A, LEDCODE_B, LEDCODE_C, LEDCODE_D, LEDCODE_E, LEDCODE_F
};
/*--------------------------------------------------------------------------------------------------*/
/*		LED Convert Table																			*/
/*--------------------------------------------------------------------------------------------------*/
const	UCHAR	ledconvtbl[] =
{
/* '-', */		LEDCODE_MINUS,
/* '.', */		LEDCODE_POINT,
/* '/', */		LEDCODE_BLANK,
/* '0', */		LEDCODE_ZERO ,
/* '1', */		LEDCODE_1    ,
/* '2', */		LEDCODE_2    ,
/* '3', */		LEDCODE_3    ,
/* '4', */		LEDCODE_4    ,
/* '5', */		LEDCODE_5    ,
/* '6', */		LEDCODE_6    ,
/* '7', */		LEDCODE_7    ,
/* '8', */		LEDCODE_8    ,
/* '9', */		LEDCODE_9    ,
/* ':', */		LEDCODE_BLANK,
/* ';', */		LEDCODE_BLANK,
/* '<', */		LEDCODE_BLANK,
/* '=', */		LEDCODE_BLANK,
/* '>', */		LEDCODE_BLANK,
/* '?', */		LEDCODE_BLANK,
/* '@', */		LEDCODE_BLANK,
/* 'A', */		LEDCODE_A    ,
/* 'B', */		LEDCODE_B    ,
/* 'C', */		LEDCODE_C    ,
/* 'D', */		LEDCODE_D    ,
/* 'E', */		LEDCODE_E    ,
/* 'F', */		LEDCODE_F    ,
/* 'G', */		LEDCODE_G    ,
/* 'H', */		LEDCODE_H    ,
/* 'I', */		LEDCODE_1	 ,
/* 'J', */		LEDCODE_J    ,
/* 'K', */		LEDCODE_BLANK,
/* 'L', */		LEDCODE_L    ,
/* 'M', */		LEDCODE_BLANK,
/* 'N', */		LEDCODE_N    ,
/* 'O', */		LEDCODE_ZERO ,
/* 'P', */		LEDCODE_P    ,
/* 'Q', */		LEDCODE_Q    ,
/* 'R', */		LEDCODE_R    ,
/* 'S', */		LEDCODE_5    ,
/* 'T', */		LEDCODE_T    ,
/* 'U', */		LEDCODE_U    ,
/* 'V', */		LEDCODE_BLANK,
/* 'W', */		LEDCODE_BLANK,
/* 'X', */		LEDCODE_BLANK,
/* 'Y', */		LEDCODE_Y    ,
/* 'Z', */		LEDCODE_BLANK
};
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Variable Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Panel Operator Structure Definition															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{
/*--------------------------------------------------------------------------------------------------*/
	USHORT	LastAlarm;		/* �A���[���O��l(�A���[���������ɏ�ԕ\�����[�h�ɔ�Ԃ��߂Ɏg�p����)	*/
	USHORT	BtnLast;		/* �p�l���{�^���O��l													*/
	ULONG	KeyTime;		/* �p�l���{�^�����������m�p												*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	DispMode;		/* �\�����[�h															*/
//	LEDDSP	DspData;		/* �\���f�[�^															*/
	ULONG	BlnkTime;		/* �p�l���\���_�Ŏ�������p												*/
	USHORT	CurPos;			/* �p�l�� �p�����[�^���݌�/���j�^�[���ݕ\����							*/
	USHORT	CurPosMax;		/* �p�l�� �p�����[�^���݌�/���j�^�[���ݕ\���� �͈�						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CurFunIndx;		/* �p�l���⏕�@�\���݈ʒu�C���f�b�N�X									*/
	USHORT	CurPrmIndx;		/* �p�l���p�����[�^���݈ʒu�C���f�b�N�X									*/
	USHORT	PrmWrState;		/* �p�l���p�����[�^�������ݎ����										*/
	USHORT	CurMonIndx;		/* �p�l�����j�^�[���݈ʒu�C���f�b�N�X									*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	ChkTime;		/* AP�` �v�b�V���{�^��&7SegLed�m�F �o�ߎ���								*/	/* <S0CB> */
} PNL_STRUC;
/*--------------------------------------------------------------------------------------------------*/
/*		Local Variable Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
PNL_STRUC	Pnl;			/* Panel Operator Variable												*/
/*--------------------------------------------------------------------------------------------------*/



#if SVFSEL_PANELOP_USE == 1											/* Not Use Panel */
/****************************************************************************************************/
/*																									*/
/*		Panel Operator Function List																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�l���I�y���[�^�h�e�֐�																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxPnlOpMain( HMSGIF Hmsg );		/* Panel Operator Main Program							*/
void	RpxPnlOpInit( void );				/* Panel Operator Initialize							*/
void	RpxPnlOpMonInit( HMSGIF Hmsg );		/* Panel Monitor Initialize								*/
void	RpxPnlOpFinish( HMSGIF Hmsg );		/* Panel Operator Finish								*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedDispMonDecWord(				/* LED�I�y���[�^ ���j�^�[�\��(10�i���[�h�����L/��)		*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign			);			/* �����̗L��											*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedDispMonDecLong(				/* LED�I�y���[�^ ���j�^�[�\��(10�i�����O�����L/��)		*/
		ULONG	ldata,						/* ldata:�\������l										*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	dsp_pos,					/* ���ݕ\���ʒu											*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign			);			/* �����L��												*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetStatusBits(				/* LED�I�y���[�^ �X�e�[�^�X�r�b�g�\�����Z�b�g����		*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		HMSGIF	Hmsg );						/* TODO: ��ŏ���										*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetValDec(					/* LED�I�y���[�^ �\���l(10�i)�Z�b�g����					*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	startdgt,					/* �J�n��												*/
		USHORT	digit,						/* ��������												*/
		USHORT	sup_flg,					/* 0�\���}�~�t���O										*/
		USHORT	decimals		);			/* �����_�ȉ�����										*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetValHex(					/* LED�I�y���[�^ �\���l(16�i)�Z�b�g����					*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	startdgt,					/* �J�n��												*/
		USHORT	digit			);			/* ��������												*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetStr(						/* LED Operator String Set								*/
		UCHAR	*str_ptr,					/* �\�����镶����A�h���X								*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	startdgt,					/* �J�n��												*/
		USHORT	length			);			/* �\��������											*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetBlnk(						/* LED Operator Blink Set								*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		UCHAR	blnkptn			);			/* �_�Ńp�^�[��(1bit=1������)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		Local Function Prototype Definition															*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlKeyTranslate( HMSGIF Hmsg );		/* �p�l���I�y���[�^ �L�[�ϊ�����						*/
void	PnlStatusDispMode( HMSGIF Hmsg );	/* �p�l���I�y���[�^ ��ԕ\������						*/
void	PnlFunNoMode( HMSGIF Hmsg );		/* �p�l���I�y���[�^ Fn�ԍ��ҏW���[�h					*/
void	PnlFunExeMode( HMSGIF Hmsg );		/* �p�l���I�y���[�^ Fn���s���[�h						*/
void	PnlPrmNoMode( HMSGIF Hmsg );		/* �p�l���I�y���[�^ Pn�ԍ��ҏW���[�h					*/
void	PnlPrmDataMode( HMSGIF Hmsg );		/* �p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h				*/
void	PnlMonNoMode( HMSGIF Hmsg );		/* �p�l���I�y���[�^ Un�ԍ��ҏW���[�h					*/
void	PnlMonDataMode( HMSGIF Hmsg );		/* �p�l���I�y���[�^ Un�f�[�^�\�����[�h					*/
void	PnlLedBlink( HMSGIF Hmsg );			/* �p�l���I�y���[�^ LED�u�����N����						*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecWordUnsigned(		/* �p�l���I�y���[�^ LED�\��(�p�����[�^������10�i���[�h)	*/
		USHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		LEDDSP	*PnlDsp );					/*														*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecWordSigned(			/* �p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)	*/
		SHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals, 					/* �����_�ȉ�����										*/
		SHORT	llimit,						/* �����l												*/
		SHORT	ulimit,						/* ����l												*/
		LEDDSP	*PnlDsp );					/*														*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmHexNblWord(			/* �p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)		*/
		USHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	base,						/* �\���̒�												*/
		LEDDSP	*PnlDsp );					/* @@													*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecLong(				/* �p�l���I�y���[�^ LED�\��(10�i�����O)					*/
		ULONG	ldata,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign,						/* �����L��												*/
		LEDDSP	*PnlDsp );					/* @@													*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmHexLong(				/* �p�l���I�y���[�^ LED�\��(16�i�����O)					*/
		ULONG	ldata,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		LEDDSP	*PnlDsp );					/* @@													*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispMonHexWord(				/* �p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)			*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*DspData );					/* @@ Panel Display Data								*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispMonBit(					/* �p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)		*/
		UCHAR	data,						/* �\������l											*/
		LEDDSP	*leddsp			);			/*														*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedSetKnlBuffer(					/* Panel LED Set to Kernel Variables					*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	*dist,						/*														*/
		LONG	size			);			/*														*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedSetBlnkSw(					/* Panel LED Set to Same Data							*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	data,						/*														*/
		LONG	size,						/*														*/
		LONG	flag			);			/*														*/
/*--------------------------------------------------------------------------------------------------*/
void	PbAndLedChkTest( HMSGIF Hmsg );		/* �p�l���I�y���[�^ �v�b�V���{�^�� & 7SegLed �m�F����	*//* <S0CB> */
/*--------------------------------------------------------------------------------------------------*/




/****************************************************************************************************/
/*																									*/
/*		Panel Operator Main Program																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpMain( HMSGIF Hmsg )
{
#define MSGIF_SEND( resbuf, reslen )   ( Hmsg->Sif->StxRes( Hmsg->Sif, (resbuf), (reslen) ) )
USHORT	CurAlarm;
//LONG	rc_tmp;
AXIS_HANDLE	*pAxRsc = Hmsg->pAxRsc;		/* TODO: ���Ƃŏ��� 										*/
LEDDSP *PnlDsp = (LEDDSP *)Hmsg->ResBuf;

/* <S0CB> Start */
	/*--------------------------------------------------------------------------------------------------*/
	/*		�v�b�V���{�^�� & 7SegLed �m�F����															*/
	/*--------------------------------------------------------------------------------------------------*/
	if( pAxRsc->FnCmnCtrl->PbAndLedChkTestFlag )
	{
		/* �v�b�V���{�^�� & 7SegLed �m�F���� �J�n */
		PbAndLedChkTest( Hmsg );
		PnlLedSetKnlBuffer( PnlDsp, PnlDsp->LedCode, PNL_N_LED );
		MSGIF_SEND( Hmsg->ResBuf, sizeof(PNLRSPMSG) );
		return;
	}
/* <S0CB> End */

/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM��ڑ���																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( Hmsg->ComMode == SCM_NOCOM )
		{
			CurAlarm = KpiGetAlarmInfo();
		/*------------------------------------------------------------------------------------------*/
		/*	�A���[���������͏�ԕ\�����[�h��														*/
		/*------------------------------------------------------------------------------------------*/
			if( (CurAlarm != 0) && ((CurAlarm & 0xF00) != 0x900) && (Pnl.LastAlarm == 0) )
			{
				Pnl.CurPos = 0;
				if(Pnl.DispMode == PNLMODE_FNEXE)
				{
					CPNLFUN->Finish( CPNLFUN, Hmsg ); /* RpxFunFinishPnl(Pnl.CurFunIndx, &PopMsg); */
				}
				if(Pnl.DispMode == PNLMODE_PNDATA)
				{
					/* TODO: EEPROM */
					//rc_tmp = RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx);
					//while(rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL)
					//{
					//	rc_tmp = RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx);
					//}
				}
				Pnl.DispMode = PNLMODE_STATUS;
			}
			Pnl.LastAlarm = CurAlarm;
		/*------------------------------------------------------------------------------------------*/
		/*																							*/
		/*------------------------------------------------------------------------------------------*/
			PnlKeyTranslate( Hmsg );
			switch( Pnl.DispMode )
			{
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_STATUS:	/* ��ԕ\��	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_FNNUM;
					Pnl.CurPos = 0;
				}
				else
				{
					PnlStatusDispMode( Hmsg );
					if(OpKeyMk.rst)
					{
						KriResetSystemAlarm();
//TODO:						NdiClearAlarm();
					}
				}
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_FNNUM:		/* Fn�ԍ�	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_PNNUM;
					Pnl.CurPos = 0;
				}
				else if(OpKeyMk.data)
				{
					if( CPNLFUN->Begin( 
							CPNLFUN, 
							Pnl.CurFunIndx, 
							(UINT *)&OpKeyMk,
							(UINT *)&OpKeyBrk,
							(UINT *)&OpKeyLvl,
							(UINT *)&OpBlnkKickFlg,
							(UCHAR *)&(Hmsg->ResBuf[PNLRSP_LEDCODE_IDX]),
							Hmsg ) != OK  )				
					{														/* Fn���s�ł��Ȃ���	*/
						OpBlnkKickFlg.no_op = 1;
					}
					else
					{
						Pnl.DispMode = PNLMODE_FNEXE;
						Pnl.CurPos = 0;
					}
				}
				else
				{
					PnlFunNoMode( Hmsg );
				}
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_FNEXE:		/* Fn���s	*/
		/*------------------------------------------------------------------------------------------*/
				if ( CPNLFUN->Exec( CPNLFUN, Hmsg ) )
				{
					Pnl.CurPos = 0;
					Pnl.DispMode = PNLMODE_FNNUM;
				}

/* <S14F> */
//				KpiRstLongTimer( &(Hmsg->ComChkTime) );				/* Reset ComChkTimer		*/	/* <S142> */

				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_PNNUM:		/* Pn�ԍ�	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_UNNUM;
					Pnl.CurPos = 0;
				}
				else if(OpKeyMk.data)
				{
					if( CHK_WRITEINHIBIT() && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
					{
						OpBlnkKickFlg.no_op = 1;
					}
					else
					{
						Pnl.DispMode = PNLMODE_PNDATA;
						Pnl.PrmWrState = 0;
						Pnl.CurPos = 0;
					}
				}
				else
				{
					PnlPrmNoMode( Hmsg );
				}
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_PNDATA:	/* Pn�f�[�^	*/
		/*------------------------------------------------------------------------------------------*/
				PnlPrmDataMode( Hmsg );
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_UNNUM:		/* Un�ԍ�	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_STATUS;
				}
				else if(OpKeyMk.data)
				{
					Pnl.DispMode = PNLMODE_UNDATA;
					Pnl.CurPos = 0;
				}
				else {	PnlMonNoMode( Hmsg ); }
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_UNDATA:	/* Un�f�[�^	*/
		/*------------------------------------------------------------------------------------------*/
				PnlMonDataMode( Hmsg );
				break;
		/*------------------------------------------------------------------------------------------*/
			default:
		/*------------------------------------------------------------------------------------------*/
				break;
			}
			PnlLedBlink( Hmsg );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM�ڑ���																			*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			PnlStatusDispMode( Hmsg );
			PnlLedSetBlnkSw( PnlDsp, PNLLED_NOBLINK,
							 PNL_N_LED, ((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x02) );
		}
/*--------------------------------------------------------------------------------------------------*/
		PnlLedSetKnlBuffer( PnlDsp, PnlDsp->LedCode, PNL_N_LED );
		MSGIF_SEND( Hmsg->ResBuf, sizeof(PNLRSPMSG) );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ �L�[�ϊ�����																*/
/*																									*/
/****************************************************************************************************/
void	PnlKeyTranslate( HMSGIF Hmsg )
{
PNLCMDMSG *PnlCmd = (PNLCMDMSG *)Hmsg->CmdBuf;
UINT	PnlBtn = PnlCmd->PnlBtn;								/* Get Panel Button */
/*--------------------------------------------------------------------------------------------------*/
/*		�ϐ�������																					*/
/*--------------------------------------------------------------------------------------------------*/
		*((UINT *)&OpKeyMk)  = 	*((UINT *)&OpKeyBrk) = *((UINT *)&OpKeyLvl) = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N���̓L�[���͂𖳎�����																*/
/*--------------------------------------------------------------------------------------------------*/
		if( *((UINT *)&OpBlnkFlg) != 0 )
		{
			Pnl.BtnLast = PnlBtn;
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���̓��x���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( PnlBtn & PNLKEY_MODE	){ OpKeyLvl.mode = TRUE;}
		if( PnlBtn & PNLKEY_UP		){ OpKeyLvl.up   = TRUE;}
		if( PnlBtn & PNLKEY_DOWN	){ OpKeyLvl.down = TRUE;}
		if( PnlBtn & PNLKEY_SHUP	){ OpKeyLvl.shup = TRUE;}
		if( (PnlBtn & PNLKEY_RST) == PNLKEY_RST ){ OpKeyLvl.rst = TRUE;}
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͂ɕω������������̂݃Z�b�g(=�`���^�����O�h�~)										*/
/*--------------------------------------------------------------------------------------------------*/
		if( PnlBtn != Pnl.BtnLast )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�L�[����MAKE�̏���																		*/
		/*------------------------------------------------------------------------------------------*/
			switch( PnlBtn )
			{
			case PNLKEY_MODE:	OpKeyMk.mode = TRUE; break;
			case PNLKEY_UP:		OpKeyMk.up   = TRUE; KpiRstLongTimer(&Pnl.KeyTime); break;
			case PNLKEY_DOWN:	OpKeyMk.down = TRUE; KpiRstLongTimer(&Pnl.KeyTime); break;
			case PNLKEY_SHUP:	OpKeyMk.shup = TRUE; KpiRstLongTimer(&Pnl.KeyTime); break;
			case PNLKEY_RST:	OpKeyMk.rst  = TRUE; break;
			default: break;
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�L�[����BREAK�̏���																		*/
		/*------------------------------------------------------------------------------------------*/
			switch(Pnl.BtnLast)
			{
			case PNLKEY_MODE:	OpKeyBrk.mode = TRUE; break;
			case PNLKEY_UP:		OpKeyBrk.up   = TRUE; break;
			case PNLKEY_DOWN:	OpKeyBrk.down = TRUE; break;
			case PNLKEY_SHUP:	OpKeyBrk.shup = TRUE; break;
			case PNLKEY_RST:	OpKeyBrk.rst  = TRUE; break;
			default: break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͂ɕω����Ȃ��ꍇ: ���s�[�g, �z�[���h����											*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			switch(PnlBtn)
			{
			case PNLKEY_UP:
				if(KpiGetLongTimer(&Pnl.KeyTime) >= 400)
				{	/* 400ms�����͉������Ȃ�	*/
					if(!((KpiGetLongTimer(&Pnl.KeyTime)/100) & 0x01))
					{	/* �o�ߎ��� 4xx,6xx,8xx...�̎�	*/
						OpKeyMk.up = TRUE;
					}
				}
				break;
			case PNLKEY_DOWN:
				if(KpiGetLongTimer(&Pnl.KeyTime) >= 400)
				{	/* 400ms�����͉������Ȃ�	*/
					if(!((KpiGetLongTimer(&Pnl.KeyTime)/100) & 0x01))
					{	/* �o�ߎ��� 4xx,6xx,8xx...�̎�	*/
						OpKeyMk.down = TRUE;
					}
				}
				break;
			case PNLKEY_SHUP:
				if(KpiGetLongTimer(&Pnl.KeyTime) >= 1000)
				{	/* 1s �ȏ��[DATA]�L�[���͑���	*/
					OpKeyMk.data = TRUE;
					KpiRstLongTimer(&Pnl.KeyTime);
				}
				break;
			default:
				break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
	Pnl.BtnLast = PnlBtn;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ ��ԕ\������																*/
/*																									*/
/****************************************************************************************************/
void	PnlStatusDispMode( HMSGIF Hmsg )
{
USHORT	CurAlarm;
AXIS_HANDLE	*pAxRsc = Hmsg->pAxRsc;		/* TODO: ���Ƃŏ��� 										*/
LEDDSP *PnlDsp = (LEDDSP *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		��ԕ\��																					*/
/*--------------------------------------------------------------------------------------------------*/
		PnlDsp->LedCode[4] = LEDCODE_B7;
		PnlDsp->LedCode[3] = LEDCODE_BLANK;
		RpxLedSetStatusBits( PnlDsp, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[��������																				*/
/*--------------------------------------------------------------------------------------------------*/
		CurAlarm = KpiGetAlarmInfo();
		if( CHK_ALARM() /* Alm.f.AlmFlag */ )
		{
			/* �A���[���R�[�h�\��	*/
			RpxLedSetValHex(CurAlarm, PnlDsp, 2, 3);
			PnlLedSetBlnkSw( PnlDsp, PNLLED_BLINK, PNL_N_LED, TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���[�j���O������																			*/
/*--------------------------------------------------------------------------------------------------*/
		else if( CurAlarm != 0 )
		{
			/* ���[�j���O�R�[�h�\��	*/
			RpxLedSetValHex(CurAlarm, PnlDsp, 2, 3);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʏ�\��(�A���[�������[�j���O����)															*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( CHK_SVON() )
			{
				if( CHK_PDET() )
				{
					PnlDsp->LedCode[3] = LEDCODE_P;	/* " P-dt"	*/
					PnlDsp->LedCode[2] = LEDCODE_B6;
					PnlDsp->LedCode[1] = LEDCODE_D;
					PnlDsp->LedCode[0] = LEDCODE_T;
				}
				else
				{
					PnlDsp->LedCode[2] = LEDCODE_R;	/* "  run"	*/
					PnlDsp->LedCode[1] = LEDCODE_sU;
					PnlDsp->LedCode[0] = LEDCODE_N;
				}
			}
			else
			{
				PnlDsp->LedCode[2] = LEDCODE_BLANK;	/* "   bb"	*/
				PnlDsp->LedCode[1] = LEDCODE_B;
				PnlDsp->LedCode[0] = LEDCODE_B;
			}
		/*------------------------------------------------------------------------------------------*/
			if( (!CHK_JOGMODE()) && (!CHK_ZRETMODE()) )
			{
				if( CHK_POT() && CHK_NOT() )	/* "Pot" AND "not" */
				{
					if(!((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x04))	/* ����800ms	*/
					{
						PnlDsp->LedCode[2] = LEDCODE_P;
						PnlDsp->LedCode[1] = LEDCODE_sO;
						PnlDsp->LedCode[0] = LEDCODE_T;
					}
					else
					{
						PnlDsp->LedCode[2] = LEDCODE_N;
						PnlDsp->LedCode[1] = LEDCODE_sO;
						PnlDsp->LedCode[0] = LEDCODE_T;
					}
				}
				else if( CHK_POT() /* CinV.f.Sv.Pot */)				/* "Pot" */
				{
					PnlDsp->LedCode[2] = LEDCODE_P;
					PnlDsp->LedCode[1] = LEDCODE_sO;
					PnlDsp->LedCode[0] = LEDCODE_T;
				}
				else if( CHK_NOT() /* CinV.f.Sv.Not */)				/* "Not" */
				{
					PnlDsp->LedCode[2] = LEDCODE_N;
					PnlDsp->LedCode[1] = LEDCODE_sO;
					PnlDsp->LedCode[0] = LEDCODE_T;
				}
			}
		/* <S1F5> Start */
		/*------------------------------------------------------------------------------------------*/
			if ( CHK_FSTP() )										/* "FSTP" */
			{
				PnlDsp->LedCode[2] = LEDCODE_F;
				PnlDsp->LedCode[1] = LEDCODE_S;
				PnlDsp->LedCode[0] = LEDCODE_T;
			}
		/* <S1F5> End */
		/*------------------------------------------------------------------------------------------*/
			if( CHK_HBB() )											/* "Hbb" */
			{
				PnlDsp->LedCode[3] = LEDCODE_BLANK; /* ���F���Ɍ��o����OT��ALM�ƂȂ邽�ߕs�v*/
				PnlDsp->LedCode[2] = LEDCODE_H;
				PnlDsp->LedCode[1] = LEDCODE_B;
				PnlDsp->LedCode[0] = LEDCODE_B;
			}
/* <S1B7> >>>>> */
		/*------------------------------------------------------------------------------------------*/
			if( CHK_MOTOR_LESS_TEST() )								/* "tSt" */
			{
				if(!((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x08))	/* ����1600ms	*/
				{
					PnlDsp->LedCode[2] = LEDCODE_T;
					PnlDsp->LedCode[1] = LEDCODE_5;
					PnlDsp->LedCode[0] = LEDCODE_T;
				}
			}
/* <<<<< <S1B7> */
		/*------------------------------------------------------------------------------------------*/
			PnlLedSetBlnkSw( PnlDsp, PNLLED_NOBLINK, PNL_N_LED, TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Fn�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlFunNoMode( HMSGIF Hmsg )
{
USHORT	i;
UINT	SrchNo;
CFUNTBL	*pTbl = GET_FUNTBL( CPNLFUN );									/* Get Function Table */
UINT	FnNo = CPNLFUN->GetNumber( CPNLFUN, Pnl.CurFunIndx, Hmsg );		/* Get Function Number */
LEDDSP	*PnlDsp = (LEDDSP *)(Hmsg->ResBuf);								/* 					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.shup )
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if( OpKeyMk.up )
		{
			SrchNo = FnNo + (0x01 << (Pnl.CurPos*4));
			if(SrchNo >= 0x1000){ SrchNo = 0xFFF; }
			Pnl.CurFunIndx = CPNLFUN->SearchB ( CPNLFUN, SrchNo, Pnl.CurFunIndx, Hmsg ); 
		}
		else if( OpKeyMk.down )
		{
			if( FnNo < (0x01 << (Pnl.CurPos*4)) )
			{
				SrchNo = 0x000;
			}
			else
			{
				SrchNo = FnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurFunIndx = CPNLFUN->SearchB ( CPNLFUN, SrchNo, Pnl.CurFunIndx, Hmsg ); 
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\������																					*/
/*--------------------------------------------------------------------------------------------------*/
		PnlDsp->LedCode[4] = LEDCODE_F;
		PnlDsp->LedCode[3] = LEDCODE_N;
		RpxLedSetValHex( CPNLFUN->GetNumber( CPNLFUN, Pnl.CurFunIndx, Hmsg ), PnlDsp, 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PNL_N_LED; i++ )
		{
			PnlDsp->LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Fn���s���[�h																*/
/*																									*/
/****************************************************************************************************/
#if 0
void	PnlFunExeMode( HMSGIF Hmsg )
{
	RpxFunExePnl( OpKeyMk, OpKeyBrk, OpKeyLvl, Pnl.CurFunIndx,
							&(Pnl.DspData), &OpBlnkKickFlg, &PopMsg ); 
}
#endif



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Pn�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlPrmNoMode( HMSGIF Hmsg )
{
UINT	i;
UINT	SrchNo;
CPRMDEF	*prmdef_tmp;
CPRMTBL *pTbl = GET_PNPRMTBL( CPARAMX );
UINT	PnNo = CPARAMX->GetNumber( CPARAMX, Pnl.CurPrmIndx, Hmsg );
LEDDSP *PnlDsp = (LEDDSP *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.shup )
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if( OpKeyMk.up )
		{
			SrchNo = PnNo + (0x01 << (Pnl.CurPos*4));
			if(SrchNo >= 0x1000) {	SrchNo = 0xFFF;  }
			Pnl.CurPrmIndx = CPARAMX->SearchB( (COBJECT *)CPARAMX, SrchNo, Pnl.CurPrmIndx, Hmsg ); 
		}
		else if( OpKeyMk.down )
		{
			if(PnNo < (0x01 << (Pnl.CurPos*4)))
			{
				SrchNo = 0x000;
			}
			else
			{
				SrchNo = PnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurPrmIndx = CPARAMX->SearchB( (COBJECT *)CPARAMX, SrchNo, Pnl.CurPrmIndx, Hmsg); 
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���݌��ő�l																				*/
/*--------------------------------------------------------------------------------------------------*/
		prmdef_tmp = GET_PNPRMDEF( pTbl, Pnl.CurPrmIndx );						/* �p�����[�^��`	*/
		if( CPARAMX->isLong( CPARAMX, Pnl.CurPrmIndx, Hmsg ) )					/* �����O�p�����[�^	*/
		{
			Pnl.CurPosMax = (GET_PNPRMBASE( prmdef_tmp) == DSPDEF_BASEDEC)? 10: 8;
		}
		else																	/* ���[�h�p�����[�^	*/
		{
			if( GET_PNPRMBASE( prmdef_tmp ) == DSPDEF_BASEDEC )
			{
				if( CHK_PNPRM_SIGNED( prmdef_tmp )  &&
					( (-10000 < GET_PNPRM_LOWERLMT( prmdef_tmp ) ) && 
					  ( GET_PNPRM_UPPERLMT( prmdef_tmp ) < 10000) )
				)
				{
					Pnl.CurPosMax = 4;									/* �����܂�5���ő����ꍇ	*/
				}
				else
				{
					Pnl.CurPosMax = 5;
				}
			}
			else
			{
				Pnl.CurPosMax = 4;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�k�d�c�\������																				*/
/*--------------------------------------------------------------------------------------------------*/
		PnlDsp->LedCode[4] = LEDCODE_P;
		PnlDsp->LedCode[3] = LEDCODE_N;
		RpxLedSetValHex( CPARAMX->GetNumber( CPARAMX, Pnl.CurPrmIndx, Hmsg ), 
						PnlDsp, 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(PNL_N_LED); i++ )
		{
			PnlDsp->LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h													*/
/*																									*/
/****************************************************************************************************/
void	PnlPrmDataMode( HMSGIF Hmsg )
{
ULONG	ldata_tmp;
LONG	rc;
CPRMTBL *pTbl = GET_PNPRMTBL( CPARAMX );								/* Get Parameter Table */
CPRMDEF	*prmdef_tmp = GET_PNPRMDEF( pTbl, Pnl.CurPrmIndx );				/* Get Parameter Definition	*/
LEDDSP 	*PnlDsp = (LEDDSP *)(Hmsg->ResBuf);								/* @@ 				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW����														*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Pnl.PrmWrState )
		{
	  /*--------------------------------------------------------------------------------------------*/
		case OPWM_None:	/* �p�����[�^�l�\���E�ҏW����	*/
	  /*--------------------------------------------------------------------------------------------*/
			CPARAMX->Read( (COBJECT *)CPARAMX,   /* <S073> */
						Pnl.CurPrmIndx, 
						&ldata_tmp, 
						PCHK_OPE_READ, 
						Hmsg );
			if( OpKeyMk.data | OpKeyMk.mode )							/* EEPROM�������ݏ���		*/
			{
				rc = PrmWriteValueToEeprom( Hmsg->pAxRsc->RegManager->hPnReg,						  /* <S098> */
											PnPrmTbl[Pnl.CurPrmIndx].Attribute,
											0 );
				switch( rc )
				{
				case PRM_RSLT_SUCCESS:									/* ����	*/
					OpBlnkKickFlg.PrmEewrite = 1;						/* �u�����N�N��	*/
					break;
				/* ����Queue���Ȃ����߁A�ȉ��R�����g�A�E�g Y.Oka */
//				case PRM_EEPROM_WRITE_FAIL:								/* �܂�Queue���󂢂Ă��Ȃ�	*/
//					Pnl.PrmWrState = OPWM_UpdtWait;
//					break;
				case PRM_RSLT_RAMONLY:			/* RAM�݂̂̃p�����[�^								*/
				case PRM_RSLT_NO_DIFF:			/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
					Pnl.CurPos = 0;
					Pnl.DispMode = PNLMODE_PNNUM;						/* �p�����[�^�ԍ����[�h��	*/
					return;
				default:
					break;
				}

			}
			else if( OpKeyMk.shup )
			{
				Pnl.CurPos = ((Pnl.CurPos+1) == Pnl.CurPosMax)?
					0: Pnl.CurPos+1;
			}
			else if( OpKeyMk.up | OpKeyMk.down )
			{
				/*
				 *
				 * 	RpxPrmRamDigitInc() + RpxPrmWriteRamIdx() = DigitInc()
				 * 	RpxPrmRamDigitDec() + RpxPrmWriteRamIdx() = DigitDec()
				 *
				 */
				if( OpKeyMk.up )
				{
					rc = CPARAMX->DigitInc( CPARAMX, Pnl.CurPrmIndx, Pnl.CurPos, Hmsg );
				}
				else
				{
					rc = CPARAMX->DigitDec( CPARAMX, Pnl.CurPrmIndx, Pnl.CurPos, Hmsg );
				}
				/*----------------------------------------------------------------------------------*/
				if( rc == PERR_CALC )
				{
					OpBlnkKickFlg.error = 1;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�\������																				*/
		/*------------------------------------------------------------------------------------------*/
			if( CPARAMX->isLong( CPARAMX, Pnl.CurPrmIndx, Hmsg) )			/* �����O�p�����[�^	*/
			{
				if(GET_PNPRMBASE( prmdef_tmp ) == DSPDEF_BASEDEC)		/* �ꂪ10�i	*/
				{
					PnlLedDispPrmDecLong(ldata_tmp, Pnl.CurPos, 
						/*ET_PNPRM_DECIMALS( prmdef_tmp ), */
						GET_PNPRM_DECIMALS( CPARAMX, prmdef_tmp, Hmsg ), 	/* <S14F> */
						CHK_PNPRM_SIGNED( prmdef_tmp ),
						PnlDsp );
				}
				else													/* �ꂪ10�i�ȊO	*/
				{
					PnlLedDispPrmHexLong(ldata_tmp, Pnl.CurPos, PnlDsp );
				}
				
			}
			else														/* ���[�h�p�����[�^	*/
			{
				if( GET_PNPRMBASE( prmdef_tmp ) == DSPDEF_BASEDEC)		/* �ꂪ10�i	*/
				{
					if( CHK_PNPRM_SIGNED( prmdef_tmp ) )
					{
						PnlLedDispPrmDecWordSigned((SHORT)ldata_tmp, Pnl.CurPos, 
							/*GET_PNPRM_DECIMALS( prmdef_tmp ),*/
	 						GET_PNPRM_DECIMALS( CPARAMX, prmdef_tmp, Hmsg ), 	/* <S14F> */
							GET_PNPRM_LOWERLMT( prmdef_tmp ), 
							GET_PNPRM_UPPERLMT( prmdef_tmp ),
							PnlDsp );
					}
					else
					{
						PnlLedDispPrmDecWordUnsigned((USHORT)ldata_tmp, Pnl.CurPos, 
							/*GET_PNPRM_DECIMALS( prmdef_tmp ),*/
	 						GET_PNPRM_DECIMALS( CPARAMX, prmdef_tmp, Hmsg ), 	/* <S14F> */
							PnlDsp );
					}
				}
				else													/* �ꂪ10�i�ȊO	*/
				{
					PnlLedDispPrmHexNblWord((USHORT)ldata_tmp, Pnl.CurPos, 
						GET_PNPRMBASE( prmdef_tmp ),
						PnlDsp );
				}
			}
			break;
	  /*--------------------------------------------------------------------------------------------*/
		case OPWM_UpdtWait:											/* EEPROM��������(�v���o�^)����	*/
	  /*--------------------------------------------------------------------------------------------*/
			switch( PRM_EEPROM_RAM_NO_DIFF /* TODO: RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx) */)
			{
			case OK:												/* ����	*/
				OpBlnkKickFlg.PrmEewrite = 1;						/* �u�����N�N��	*/
				Pnl.PrmWrState = OPWM_None;
				break;
			case PRM_EEPROM_RAM_NO_DIFF:	/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
				Pnl.PrmWrState = OPWM_None;
				Pnl.CurPos = 0;
				Pnl.DispMode = PNLMODE_PNNUM;						/* �p�����[�^�ԍ����[�h��	*/
				return;
//unreachable				break;
			default:
				break;
			}
			break;
	  /*--------------------------------------------------------------------------------------------*/
		default:
	  /*--------------------------------------------------------------------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Un�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlMonNoMode( HMSGIF Hmsg )
{
UINT	i; 
UINT	SrchNo;
CMONDEF	*mondef_tmp;
CMONTBL *pTbl = GET_UNMONTBL( CUNMONX );
UINT	UnNo = CUNMONX->GetNumber( CUNMONX, Pnl.CurMonIndx, Hmsg );
LEDDSP *PnlDsp = (LEDDSP *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpKeyMk.shup)
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if(OpKeyMk.up)
		{
			SrchNo = UnNo + (0x01 << (Pnl.CurPos*4));
			Pnl.CurMonIndx = CUNMONX->SearchB( (COBJECT *)CUNMONX, SrchNo, Pnl.CurMonIndx, Hmsg );
		}
		else if(OpKeyMk.down)
		{
			if(UnNo < (0x01 << (Pnl.CurPos*4)))
			{
				SrchNo = 0x000;
			}
			else
			{
				SrchNo = UnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurMonIndx = CUNMONX->SearchB( (COBJECT *)CUNMONX, SrchNo, Pnl.CurMonIndx, Hmsg );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���݌��ő�l																				*/
/*--------------------------------------------------------------------------------------------------*/
		mondef_tmp = GET_UNMONDEF( pTbl, Pnl.CurMonIndx );				/* Get monitor definition */
		UnNo = CUNMONX->GetNumber( CUNMONX, Pnl.CurMonIndx, Hmsg );		/* Get monitor number */
		if(  CUNMONX->isLong( CUNMONX, Pnl.CurMonIndx, Hmsg ) )			/* �T�C�Y�������O	*/
		{
			Pnl.CurPosMax = 3;											/* �\���ؑւ���(���E���E��)	*/
		}
		else															/* �T�C�Y�����[�h�ȉ�	*/
		{
			Pnl.CurPosMax = 1;											/* �\���ؑւȂ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�k�d�c�\������																				*/
/*--------------------------------------------------------------------------------------------------*/
		PnlDsp->LedCode[4] = LEDCODE_U;
		PnlDsp->LedCode[3] = LEDCODE_N;
		RpxLedSetValHex( UnNo, PnlDsp, 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(PNL_N_LED); i++ )
		{
			PnlDsp->LedBlnkSw[i] = (i == Pnl.CurPos) ? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Un�f�[�^�\�����[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlMonDataMode( HMSGIF Hmsg )
{
ULONG	ldata_tmp;
CMONTBL *pTbl = GET_UNMONTBL( CUNMONX );							/* get monitor table */
CMONDEF	*mondef_tmp = GET_UNMONDEF( pTbl, Pnl.CurMonIndx );			/* get monitor definition */
LEDDSP *DspData = (LEDDSP *)(Hmsg->ResBuf);							/* @@ Panel Display Data */
/*--------------------------------------------------------------------------------------------------*/
/*		�\���f�[�^�擾																				*/
/*--------------------------------------------------------------------------------------------------*/
		CUNMONX->Read( (COBJECT *)CUNMONX, Pnl.CurMonIndx, &ldata_tmp, MCHK_ACCS_LVL, Hmsg );  /* <S073> */
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpKeyMk.data)
		{
			Pnl.CurPos = 0;
			Pnl.DispMode = PNLMODE_UNNUM;							/* �p�����[�^�ԍ����[�h��	*/
			return;
		}
		else if(OpKeyMk.shup)
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == Pnl.CurPosMax)?  0 : Pnl.CurPos+1;
		}
		else if(OpKeyMk.rst)
		{
			/* TODO: RpxMonClearVarIdx( Pnl.CurMonIndx ); */  
			CUNMONX->Clear( (COBJECT *)CUNMONX, Pnl.CurMonIndx, Hmsg );	/* <S144> */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\������																					*/
/*--------------------------------------------------------------------------------------------------*/
		switch( CHK_UNBASE( mondef_tmp ) )
		{
		case DSPDEF_BASEDEC:
			if( ! CUNMONX->isLong( CUNMONX, Pnl.CurMonIndx, Hmsg ) )	/* ���[�h	*/
			{
				RpxLedDispMonDecWord(
					(USHORT)ldata_tmp, 
					DspData,										/* @@  Panel Display Data */
					CHK_UNDECIMALS( mondef_tmp ), 
					CHK_UN_SIGNED( mondef_tmp )
					);
			}
			else													/* �����O	*/
			{
				RpxLedDispMonDecLong(
					ldata_tmp, 
					DspData, 										/* @@ Panel Display Data */
					Pnl.CurPos, 
					CHK_UNDECIMALS( mondef_tmp ),
					CHK_UN_SIGNED( mondef_tmp )
					);
			}
			break;
		case DSPDEF_BASEHEX:
			if( ! CUNMONX->isLong( CUNMONX, Pnl.CurMonIndx, Hmsg ) )	/* ���[�h	*/
			{
				PnlLedDispMonHexWord((USHORT)ldata_tmp, DspData );	/* ���݂̓��[�h�����Ȃ�	*/
			}
			else 
			{
				/* TODO: */;
			}
			break;
		case DSPDEF_BASEBIT:
			PnlLedDispMonBit((UCHAR)ldata_tmp, DspData );			/* ����8�r�b�g��\������	*/
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�u�����N����															*/
/*																									*/
/****************************************************************************************************/
void	PnlLedBlink( HMSGIF Hmsg )
{
LEDDSP *PnlDsp = (LEDDSP *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N�I���`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
		if(*((UINT *)&OpBlnkFlg) != 0)
		{
			if(KpiGetLongTimer(&OpBlnkTime) > 1000)
			{															/* �u�����N���Ԃ��I������	*/
				*((UINT *)&OpBlnkFlg) = 0;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N�J�n������																			*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpBlnkKickFlg.PrmEewrite)									/* �p�����[�^�����ݎ�	*/
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.PrmEewrite = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.no_op)									/* read/write�L�[�Ȃ�	*/
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.no_op = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.error)
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.error = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.done)
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.done = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.end)
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.end = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N������																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpBlnkFlg.PrmEewrite)									/* �p�����[�^�����ݎ��u�����N	*/
		{
			PnlLedSetBlnkSw( PnlDsp, PNLLED_BLINK, PNL_N_LED, TRUE );
		}
		else if(OpBlnkFlg.no_op)								/* �p�l���ɂ�read/write�L�[�͂Ȃ�	*/
		{
			/* "no_oP"	*/
			PnlDsp->LedCode[4] = LEDCODE_N;
			PnlDsp->LedCode[3] = LEDCODE_sO;
			PnlDsp->LedCode[2] = LEDCODE_B3;
			PnlDsp->LedCode[1] = LEDCODE_sO;
			PnlDsp->LedCode[0] = LEDCODE_P;
			PnlLedSetBlnkSw( PnlDsp, PNLLED_BLINK, PNL_N_LED, TRUE );
		}
		else if(OpBlnkFlg.error)
		{
			/* "Error"	*/
			PnlDsp->LedCode[4] = LEDCODE_E;
			PnlDsp->LedCode[3] = LEDCODE_R;
			PnlDsp->LedCode[2] = LEDCODE_R;
			PnlDsp->LedCode[1] = LEDCODE_sO;
			PnlDsp->LedCode[0] = LEDCODE_R;
			PnlLedSetBlnkSw( PnlDsp, PNLLED_BLINK, PNL_N_LED, TRUE );
		}
		else if(OpBlnkFlg.done)
		{
			/* " donE"	*/
			PnlDsp->LedCode[4] = LEDCODE_BLANK;
			PnlDsp->LedCode[3] = LEDCODE_D;
			PnlDsp->LedCode[2] = LEDCODE_sO;
			PnlDsp->LedCode[1] = LEDCODE_N;
			PnlDsp->LedCode[0] = LEDCODE_E;
			PnlLedSetBlnkSw( PnlDsp, PNLLED_BLINK, PNL_N_LED, TRUE );
		}
		else if(OpBlnkFlg.end)
		{
			/* "  End"	*/
			PnlDsp->LedCode[4] = LEDCODE_BLANK;
			PnlDsp->LedCode[3] = LEDCODE_BLANK;
			PnlDsp->LedCode[2] = LEDCODE_E;
			PnlDsp->LedCode[1] = LEDCODE_N;
			PnlDsp->LedCode[0] = LEDCODE_D;
			PnlLedSetBlnkSw( PnlDsp, PNLLED_BLINK, PNL_N_LED, TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		PnlLedSetBlnkSw( PnlDsp, PNLLED_NOBLINK, PNL_N_LED,
						 ((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x02) );
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Initialize																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpInit( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�\���̕ϐ�Pnl�̃N���A																		*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)&Pnl, sizeof(PNL_STRUC) );
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N�ϐ��̃N���A																		*/
/*--------------------------------------------------------------------------------------------------*/
		*((UINT *)&OpBlnkFlg)     = 0;
		*((UINT *)&OpBlnkKickFlg) = 0;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		<S0B5> Panel Monitor Initialize																*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpMonInit( HMSGIF Hmsg )
{
LONG	wkx;
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
LONG	SrchNo = pAxRsc->UniPrms->Prm->powon_mon;						/* Pn52F */
/*--------------------------------------------------------------------------------------------------*/
/*		Panel Monitor Initialize																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (SrchNo != PNLMON_NOSET) && (KpiGetAlarmInfo( ) == 0) )
		{
			if( CUNMONX->Search( (COBJECT *)CUNMONX, SrchNo, 0, &wkx, MCHK_ACCS_LVL, Hmsg ) == OK )
			{
				Pnl.CurMonIndx = (USHORT)wkx;
				Pnl.DispMode = PNLMODE_UNDATA;
				Pnl.CurPos = 0;
				if ( CUNMONX->isLong( CUNMONX, Pnl.CurMonIndx, Hmsg ) == TRUE )	/* �T�C�Y�������O	*/
				{
					Pnl.CurPosMax = 3;	/* �\���ؑւ���(���E���E��)	*/
				}
				else										/* �T�C�Y�����[�h�ȉ�	*/
				{
					Pnl.CurPosMax = 1;	/* �\���ؑւȂ�	*/
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Finish																		*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpFinish( HMSGIF Hmsg )
{
//LONG	rc_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		���s���̕⏕�@�\���I�������鏈���B���s�����ۂ��̔���͊֐����ōs���Ă���B					*/
/*--------------------------------------------------------------------------------------------------*/
		CPNLFUN->Finish( CPNLFUN, Hmsg ); 

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^��EEPROM�ɏ������ޕK�v������ꍇ�̏���											*/
/*--------------------------------------------------------------------------------------------------*/
//TODO: EEPROM
//
//		rc_tmp = RpxPrmUpdateEepromIdx( Pnl.CurPrmIndx );
//		while( rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL )	/* ����	*/
//		{
//			rc_tmp = RpxPrmUpdateEepromIdx( Pnl.CurPrmIndx );
//		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(�p�����[�^������10�i���[�h)										*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmDecWordUnsigned(		/* �p�l���I�y���[�^ LED�\��(�p�����[�^������10�i���[�h)	*/
		USHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		LEDDSP	*PnlDsp )					/*														*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED�\��(�p�����[�^�����Ȃ�10�i���[�h)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 5 ){ return;}
		RpxLedSetValDec( data, PnlDsp, 4, 5, FALSE, decimals );
		for( i = 0; i < PNL_N_LED; i++ )
		{
			PnlDsp->LedBlnkSw[i] = (i == cur_digit)? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)										*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmDecWordSigned(			/* �p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)	*/
		SHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals, 					/* �����_�ȉ�����										*/
		SHORT	llimit,						/* �����l												*/
		SHORT	ulimit,						/* ����l												*/
		LEDDSP	*PnlDsp )
{
USHORT	sdata_tmp, sdata_tmp1, sdata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 5 ){ return;}
		PnlLedSetBlnkSw( PnlDsp, PNLLED_NOBLINK, PNL_N_LED, TRUE );
		sdata_tmp = (data < 0)? -data: data;
/*--------------------------------------------------------------------------------------------------*/
/*		�����܂�5���ő����ꍇ																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (-10000 < llimit) && (ulimit < 10000) )
		{
			RpxLedSetValDec(sdata_tmp, PnlDsp, 3, 4, FALSE, decimals);
			PnlDsp->LedCode[4] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
			PnlDsp->LedBlnkSw[cur_digit] = 0xFF;							/* �J�[�\����	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����܂�5���ő���Ȃ��ꍇ-->�ؑ֕\��														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if(Pnl.CurPos < 4)												/* ��4��	*/
			{
				sdata_tmp1 = sdata_tmp/10000;
				sdata_tmp2 = sdata_tmp - sdata_tmp1*10000;
				RpxLedSetValDec(sdata_tmp2, PnlDsp, 3, 4, FALSE, decimals);
				PnlDsp->LedBlnkSw[cur_digit] = 0xFF;						/* �J�[�\����	*/
				PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B3;		/* �\�����\��	*/
//				if( data < 0 ){ Pnl.DspData.LedCode[4] |= LEDCODE_POINT;}	/* ���\����'.'	*/
			}
			else															/* ��1��+����	*/
			{
				sdata_tmp1 = sdata_tmp/10000;
				RpxLedSetValDec(sdata_tmp1, PnlDsp, 0, 1, FALSE, (USHORT)(decimals-4));
				PnlDsp->LedCode[2] = PnlDsp->LedCode[1] = LEDCODE_BLANK;
				PnlDsp->LedCode[3] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
				PnlDsp->LedBlnkSw[cur_digit-4] = 0xFF;						/* �J�[�\����	*/
				PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B6;		/* �\�����\��	*/
			}
			if( data < 0 ){ PnlDsp->LedCode[4] |= LEDCODE_POINT;}			/* ���\����'.'	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmHexNblWord(			/* �p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)		*/
		USHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	base,						/* �\���̒�												*/
		LEDDSP 	*PnlDsp )					/* 														*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED�\��(�p�����[�^16�i���[�h)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 4 ){ return;}
		PnlDsp->LedCode[4] = (base == DSPDEF_BASENBL)? 
			(LEDCODE_N | LEDCODE_POINT): (LEDCODE_H | LEDCODE_POINT);
		RpxLedSetValHex( data, PnlDsp, 3, 4 );
		for(i = 0; i < PNL_N_LED; i++)
		{
			PnlDsp->LedBlnkSw[i] = (i == cur_digit)? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(10�i�����O)														*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmDecLong(				/* �p�l���I�y���[�^ LED�\��(10�i�����O)					*/
		ULONG	ldata,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign,						/* �����L��												*/
		LEDDSP	*PnlDsp )					/* 														*/
{
USHORT	sdata_tmp2;
ULONG	ldata_abs, ldata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 10 ){ return;}
		PnlLedSetBlnkSw( PnlDsp, PNLLED_NOBLINK, PNL_N_LED, TRUE );
		ldata_abs = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?	-(LONG)ldata: ldata;
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		if(Pnl.CurPos < 4)
		{
			ldata_tmp2 = ldata_abs/10000;
			sdata_tmp2 = (USHORT)(ldata_abs - ldata_tmp2*10000);
			RpxLedSetValDec(sdata_tmp2, PnlDsp, 3, 4, FALSE, decimals);
			PnlDsp->LedBlnkSw[cur_digit] = 0xFF;							/* �J�[�\����	*/
			PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B3;			/* �\�����\��	*/
//			if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
//			{
//				Pnl.DspData.LedCode[4] |= LEDCODE_POINT;					/* ���\����'.'	*/
//			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		else if(Pnl.CurPos < 8)
		{
			ldata_tmp2 = ldata_abs/100000000;
			sdata_tmp2 = (USHORT)((ldata_abs - ldata_tmp2*100000000)/10000);
			RpxLedSetValDec(sdata_tmp2, PnlDsp, 3, 4, FALSE, (USHORT)(decimals-4));
			PnlDsp->LedBlnkSw[cur_digit-4] = 0xFF;							/* �J�[�\����	*/
			PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B6;			/* �\�����\��	*/
//			if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
//			{
//				Pnl.DspData.LedCode[4] |= LEDCODE_POINT;					/* ���\����'.'	*/
//			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		��2��																						*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			PnlDsp->LedCode[3] = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?
				LEDCODE_MINUS : LEDCODE_BLANK;
			PnlDsp->LedCode[2] = LEDCODE_BLANK;
			sdata_tmp2 = (USHORT)(ldata_abs/100000000);
			RpxLedSetValDec(sdata_tmp2, PnlDsp, 1, 2, FALSE, (USHORT)(decimals-8));
			PnlDsp->LedBlnkSw[cur_digit-8] = 0xFF;							/* �J�[�\����	*/
			PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B0;			/* �\�����\��	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���\����'.'																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
		{
			PnlDsp->LedCode[4] |= LEDCODE_POINT;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(16�i�����O)														*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmHexLong(				/* �p�l���I�y���[�^ LED�\��(16�i�����O)					*/
		ULONG	ldata,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		LEDDSP	*PnlDsp )
{
USHORT	sdata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit > 8 ){ return;}
		PnlLedSetBlnkSw( PnlDsp, PNLLED_NOBLINK, PNL_N_LED, TRUE );
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		if(Pnl.CurPos < 4)
		{
			sdata_tmp2 = (USHORT)(ldata & 0xFFFF);
			RpxLedSetValHex(sdata_tmp2, PnlDsp, 3, 4);
			PnlDsp->LedBlnkSw[cur_digit] = 0xFF;							/* �J�[�\����	*/
			PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B3;			/* �\�����\��	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			sdata_tmp2 = (USHORT)((ldata >> 16) & 0xFFFF);
			RpxLedSetValHex(sdata_tmp2, PnlDsp, 3, 4);
			PnlDsp->LedBlnkSw[cur_digit-4] = 0xFF;							/* �J�[�\����	*/
			PnlDsp->LedBlnkSw[4] = PnlDsp->LedCode[4] = LEDCODE_B6;			/* �\�����\��	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispMonHexWord(				/* �p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)			*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*DspData )					/* @@ Panel Display Data								*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		LED�\��(���j�^�[16�i���[�h)																	*/
/*--------------------------------------------------------------------------------------------------*/
		DspData->LedCode[4] = LEDCODE_H | LEDCODE_POINT;
		RpxLedSetValHex(data, DspData, 3, 4);
		PnlLedSetBlnkSw( DspData, PNLLED_NOBLINK, PNL_N_LED, TRUE );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispMonBit(					/* �p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)		*/
		UCHAR	data,						/* �\������l											*/
		LEDDSP	*leddsp )					/* LED�\���f�[�^�̃A�h���X								*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		�ϐ�������																					*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PNL_N_LED; i++ )
		{
			leddsp->LedBlnkSw[i] = leddsp->LedCode[i] = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		8�r�b�g��\��																				*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < 8; i++)
		{
			switch(i & 1)
			{
			case 0:																/* �����Ԗ� --> �E	*/
				leddsp->LedCode[i/2] |= (data & 1)? LEDCODE_B1 : LEDCODE_B2;
				break;
			case 1:																/* ��Ԗ� --> ��	*/
				leddsp->LedCode[i/2] |= (data & 1)? LEDCODE_B5 : LEDCODE_B4;
				break;
			default:
				break;
			}
			data >>= 1;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Hi/Lo�w�W����ы�؂�_�̕\�� 																*/
/*--------------------------------------------------------------------------------------------------*/
		leddsp->LedCode[4] = LEDCODE_B1 | LEDCODE_B2 | LEDCODE_POINT;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ ���j�^�[�\��(10�i���[�h�����L/����)											*/
/*																									*/
/****************************************************************************************************/
void	RpxLedDispMonDecWord(				/* LED�I�y���[�^ ���j�^�[�\��(10�i���[�h�����L/��)		*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign			)			/* �����̗L��											*/
{
USHORT	data_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		���j�^�[�\��(10�i���[�h�����L/����)															*/
/*--------------------------------------------------------------------------------------------------*/
		data_tmp = ((sign == DSPDEF_W_SIGN) && ((SHORT)data < 0))?	-data: data;
	/*----------------------------------------------------------------------------------------------*/
		if( sign == DSPDEF_W_SIGN )
		{
			data_tmp = ((SHORT)data < 0)?	-data: data;
			RpxLedSetValDec(data_tmp, leddspp, 3, 4, FALSE, decimals);
			leddspp->LedCode[4] = ((SHORT)data < 0)?	LEDCODE_MINUS: LEDCODE_BLANK;
		}
		else
		{
			RpxLedSetValDec(data, leddspp, 4, 5, FALSE, decimals);
		}
	/*----------------------------------------------------------------------------------------------*/
		PnlLedSetBlnkSw( leddspp, PNLLED_NOBLINK, PNL_N_LED, TRUE );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ ���j�^�[�\��(10�i�����O�����L/��)												*/
/*																									*/
/****************************************************************************************************/
void	RpxLedDispMonDecLong(				/* LED�I�y���[�^ ���j�^�[�\��(10�i�����O�����L/��)		*/
		ULONG	ldata,						/* ldata:�\������l										*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	dsp_pos,					/* ���ݕ\���ʒu											*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign			)			/* �����L��												*/
{
USHORT	sdata_tmp2;
ULONG	ldata_abs, ldata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( dsp_pos >= 3 ){ return;}
		PnlLedSetBlnkSw( leddspp, PNLLED_NOBLINK, PNL_N_LED, TRUE );
		ldata_abs = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?	-(LONG)ldata: ldata;
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
		switch(dsp_pos)
		{
		case 0:
			ldata_tmp2 = ldata_abs/10000;
			sdata_tmp2 = (USHORT)(ldata_abs - ldata_tmp2*10000);
			RpxLedSetValDec(sdata_tmp2, leddspp, 3, 4, FALSE, decimals);
			leddspp->LedBlnkSw[4] = leddspp->LedCode[4] = LEDCODE_B3;	/* �\�����\��	*/
//			if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
//			{
//				leddspp->LedCode[4] |= LEDCODE_POINT;					/* ���\����'.'	*/
//			}
			break;
		case 1:
			ldata_tmp2 = ldata_abs/100000000;
			sdata_tmp2 = (USHORT)((ldata_abs - ldata_tmp2*100000000)/10000);
			RpxLedSetValDec(sdata_tmp2, leddspp, 3, 4, FALSE, (USHORT)(decimals-4));
			leddspp->LedBlnkSw[4] = leddspp->LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
//			if((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))
//			{
//				leddspp->LedCode[4] |= LEDCODE_POINT;					/* ���\����'.'	*/
//			}
			break;
		case 2:
			leddspp->LedCode[3] = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?
				LEDCODE_MINUS : LEDCODE_BLANK;
			leddspp->LedCode[2] = LEDCODE_BLANK;
			sdata_tmp2 = (USHORT)(ldata_abs/100000000);
			RpxLedSetValDec(sdata_tmp2, leddspp, 1, 2, FALSE, (USHORT)(decimals-8));
			leddspp->LedBlnkSw[4] = leddspp->LedCode[4] = LEDCODE_B0;	/* �\�����\��	*/
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���\����'.'																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
		{
			leddspp->LedCode[4] |= LEDCODE_POINT;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ �X�e�[�^�X�r�b�g�\�����Z�b�g����											*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetStatusBits(				/* LED�I�y���[�^ �X�e�[�^�X�r�b�g�\�����Z�b�g����		*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		HMSGIF	Hmsg )						/* ��ŏ���												*/
{
AXIS_HANDLE	*pAxRsc = Hmsg->pAxRsc;			/* TODO: ��ŏ��� 										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�X�e�[�^�X�r�b�g�\�����Z�b�g																*/
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_COIN() || CHK_VCMP() || CHK_TRQMODE() )						/* TODO: TRQMODE */
		{
			leddspp->LedCode[4] |= LEDCODE_B0;
		}
		if( CHK_BB() 						){ leddspp->LedCode[4] |= LEDCODE_B6;}
		if( CHK_TGON() 						){ leddspp->LedCode[3] |= LEDCODE_B0;}
		if( CHK_PREFON() || CHK_VREFON() 	){ leddspp->LedCode[3] |= LEDCODE_B6;}
		if( CHK_TREFON() || CHK_PERRCLR() 	){ leddspp->LedCode[3] |= LEDCODE_B3;}
		if( CHK_CHG() 						){ leddspp->LedCode[3] |= LEDCODE_B7;}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ �\���l(10�i)�Z�b�g����														*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetValDec(					/* LED�I�y���[�^ �\���l(10�i)�Z�b�g����					*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	startdgt,					/* �J�n��												*/
		USHORT	digit,						/* ��������												*/
		USHORT	sup_flg,					/* 0�\���}�~�t���O										*/
		USHORT	decimals		)			/* �����_�ȉ�����										*/
{
USHORT	i, var1_tmp, var2_tmp;
UCHAR	*ledbuf_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( digit > 5 ){ return;}
		ledbuf_ptr = &(leddspp->LedCode[startdgt]);
/*--------------------------------------------------------------------------------------------------*/
/*		��ʌ��𗎂Ƃ�																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( data >= MlibDecPowerTbl[digit] )
		{
			var1_tmp = 	(USHORT)(data/MlibDecPowerTbl[digit]);
			data -= (USHORT)(var1_tmp*MlibDecPowerTbl[digit]);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\���l(10�i)�Z�b�g����																		*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < digit; i++)
		{
			var1_tmp = (USHORT)MlibDecPowerTbl[digit-1-i];		/* ���鐔: �� 1000, 100, 10, 1	*/
			var2_tmp = data/var1_tmp;
			if( sup_flg )										/* ��ʂ�0�̓u�����N�\���Ƃ���	*/
			{
				if( var2_tmp == 0 )
				{
					*(ledbuf_ptr - i) = LEDCODE_BLANK;
				}
				else
				{
					*(ledbuf_ptr - i) = ledcodetbl[var2_tmp];
					sup_flg = FALSE;
				}
			}
			else
			{
				*(ledbuf_ptr - i) = ledcodetbl[var2_tmp];
			}
			data -= (var1_tmp * var2_tmp);
		}
/*--------------------------------------------------------------------------------------------------*/
		if( (0 < (SHORT)decimals) && ((SHORT)decimals < (SHORT)digit) )
		{
			*(ledbuf_ptr-(digit-1)+decimals) |= LEDCODE_POINT;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ �\���l(16�i)�Z�b�g����														*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetValHex(					/* LED�I�y���[�^ �\���l(16�i)�Z�b�g����					*/
		USHORT	data,						/* �\������l											*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	startdgt,					/* �J�n��												*/
		USHORT	digit			)			/* ��������												*/
{
USHORT	i, var1_tmp, var2_tmp;
UCHAR	*ledbuf_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( digit > 4 ){ return;}
		ledbuf_ptr = &(leddspp->LedCode[startdgt]);
/*--------------------------------------------------------------------------------------------------*/
/*		��ʌ��𗎂Ƃ�																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( data >= (0x1 << (digit*4)) )
		{
			data &= ((0x1 << (digit*4)) -1);			/* ��ʌ��𗎂Ƃ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\���l(10�i)�Z�b�g����																		*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < digit; i++)
		{
			var1_tmp = 0x1 << ((digit-1-i)*4);			/* ���鐔: �� 0x1000, 0x100, 0x10, 0x1	*/
			var2_tmp = data/var1_tmp;
			*(ledbuf_ptr - i) = ledcodetbl[var2_tmp];
			data -= (var1_tmp * var2_tmp);
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED Operator String Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetStr(						/* LED Operator String Set								*/
		UCHAR	*str_ptr,					/* �\�����镶����A�h���X								*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		USHORT	startdgt,					/* �J�n��												*/
		USHORT	length			)			/* �\��������											*/
{
USHORT	str_val;
USHORT	idx_tmp;
USHORT	str_idx, dgt_tmp;
UCHAR	*ledbuf_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		����0 or �J�n�ʒu���\���o�b�t�@�O															*/
/*--------------------------------------------------------------------------------------------------*/
		if( (length == 0) || (startdgt >= PNL_N_LED) )
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		ledbuf_ptr = &(leddspp->LedCode[0]);
		str_idx = 0;
		dgt_tmp = startdgt;
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
		while(*(str_ptr + str_idx) != 0)
		{
			str_val = (USHORT)*(str_ptr + str_idx);
			idx_tmp = str_val - '-';
		/*------------------------------------------------------------------------------------------*/
		/*	�����f�[�^ < '-' �̏ꍇ																	*/
		/*------------------------------------------------------------------------------------------*/
			if(str_val < '-')
			{
				*(ledbuf_ptr + dgt_tmp) = LEDCODE_BLANK;
			}
		/*------------------------------------------------------------------------------------------*/
		/*	'-' <= �����f�[�^ <= 'Z' �̏ꍇ															*/
		/*------------------------------------------------------------------------------------------*/
			else if( str_val <= 'Z' )
			{
				if( (str_idx != 0) && 					/* ������擪�łȂ�		*/
					('.' == *(str_ptr + str_idx)) && 	/* '.'�ł�����			*/
					('.' != *(str_ptr + str_idx -1))	/* �O�̕�����'.'�łȂ�	*/
				)
				{	/* '.'�ȊO�̕����̌��'.'������ꍇ�͑O�̕����ɕt������B[5][.] -> [5.]	*/
					dgt_tmp++;
					*(ledbuf_ptr + dgt_tmp) |= LEDCODE_POINT;
				}
				else	*(ledbuf_ptr + dgt_tmp) = ledconvtbl[idx_tmp];
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�����f�[�^���������̏ꍇ																*/
		/*------------------------------------------------------------------------------------------*/
			else if( ('a' <= str_val) && (str_val <= 'z') )
			{
				if(      'h' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_sH;}
				else if( 'o' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_sO;}
				else if( 'u' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_sU;}
				else { *(ledbuf_ptr + dgt_tmp) = ledconvtbl[idx_tmp - ('a' - 'A')];}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	��L�ȊO�̏ꍇ																			*/
		/*------------------------------------------------------------------------------------------*/
			else
			{
				if(      '_' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_B3;}
				else if( '~' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_B0;}
				else					 { *(ledbuf_ptr + dgt_tmp) = LEDCODE_BLANK;}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�o�b�t�@�E�[�ɂ���																		*/
		/*------------------------------------------------------------------------------------------*/
			if( dgt_tmp == 0 )
			{
				if( ((str_idx+1) < length) && 				/* ���̓��͕�����������		*/
					('.' == *(str_ptr + str_idx + 1)) && 	/* ���̕�����'.'�ł�����	*/
					('.' != *(str_ptr + str_idx)) 	)		/* ����̕�����'.'�łȂ���	*/
				{
					*(ledbuf_ptr + dgt_tmp) |= LEDCODE_POINT;	/* '.'��t������		*/
				}
				break;
			}
		/*------------------------------------------------------------------------------------------*/
			dgt_tmp--;
			str_idx++;
			if( str_idx >= length ){ break;}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED Operator Blink Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetBlnk(						/* LED Operator Blink Set								*/
		LEDDSP	*leddspp,					/* LED�\���f�[�^�̃A�h���X								*/
		UCHAR	blnkptn			)			/* �_�Ńp�^�[��(1bit=1������)							*/
{
SHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED Operator Blink Set																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PNL_N_LED; i++ )
		{
			leddspp->LedBlnkSw[i] = (blnkptn & (1 << i))? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel LED Set to Kernel Variables															*/
/*																									*/
/****************************************************************************************************/
void	PnlLedSetKnlBuffer(					/* Panel LED Set to Kernel Variables					*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	*dist,						/*														*/
		LONG	size			)			/*														*/
{
LONG	i;

/*--------------------------------------------------------------------------------------------------*/
/*		Panel LED Set to Kernel Variables															*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i = 0; i < size; i++ )
		{
			dist[i] = (UCHAR)(leddsp->LedCode[i]) & (~((UCHAR)(leddsp->LedBlnkSw[i])));
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel LED Set to Same Data																	*/
/*																									*/
/****************************************************************************************************/
void	PnlLedSetBlnkSw(					/* Panel LED Set to Same Data							*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	data,						/*														*/
		LONG	size,						/*														*/
		LONG	flag			)			/*														*/
{
LONG	i;

/*--------------------------------------------------------------------------------------------------*/
/*		Panel LED Set to Same Data																	*/
/*--------------------------------------------------------------------------------------------------*/
		if ( flag != FALSE )
		{
			for ( i = 0; i < size; i++ )
			{
				leddsp->LedBlnkSw[i] = data;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/* <S0CB> Start */
/****************************************************************************************************/
/*																									*/
/*		�v�b�V���{�^�� & 7SegLed �m�F�������s����													*/
/*																									*/
/****************************************************************************************************/
void	PbAndLedChkTest( HMSGIF Hmsg )
{
	AXIS_HANDLE	*pAxRsc = Hmsg->pAxRsc;
	LEDDSP *PnlDsp = (LEDDSP *)(Hmsg->ResBuf);
	PNLCMDMSG *PnlCmd = (PNLCMDMSG *)Hmsg->CmdBuf;
	UINT	PnlBtn = PnlCmd->PnlBtn;					/* Get Panel Button */
	FUN_CMN_CONTROL	*FnCmnCtrl =  pAxRsc->FnCmnCtrl;
	UINT	LedPos;										/* �_��������7SegLed�̏ꏊ */

	ULONG IntervalTimeMs;
	USHORT i;	/* <S0FB> */

	IntervalTimeMs = FnCmnCtrl->LightIntervalTime * 100;	/* �_���Ԋu���Ԑݒ�l��100[ms]�P�ʂł���ׁA[ms]�P�ʂ֕ύX */

	if( FnCmnCtrl->AnlgLedChkInitFlag == 0x00 )
	{
		PnlDsp->LedCode[4] = LEDCODE_BLANK;
		PnlDsp->LedCode[3] = LEDCODE_BLANK;
		PnlDsp->LedCode[2] = LEDCODE_BLANK;
		PnlDsp->LedCode[1] = LEDCODE_BLANK;
		PnlDsp->LedCode[0] = LEDCODE_BLANK;

		pAxRsc->FnCmnCtrl->AnlgLedChkInitFlag = 0x01;
		KpiRstLongTimer(&Pnl.ChkTime);
	}
	else
	{
		if(KlibGetLongTimerMs(Pnl.ChkTime) > IntervalTimeMs)
		{

			KpiRstLongTimer(&Pnl.ChkTime);

			if( FnCmnCtrl->AnlgLedChkLstPnlBtn != PnlBtn )	/* �O��X�L�����Ńv�b�V���{�^���������ƍ���X�L�����ŉ������Ă���v�b�V���{�^�����قȂ�ꍇ */
			{
				/* �J�E���^���Z�b�g */
				FnCmnCtrl->AnlgLedChkCnt = 0U;
			}

			/* �v�b�V���{�^���ɑΉ�����LED�ꏊ��ݒ� */
			switch(PnlBtn)
			{
				case PNLKEY_SHUP:
					/* ���݌o�ߎ��Ԏ擾 */
					KpiRstLongTimer(&Pnl.ChkTime);
					PnlDsp->LedCode[4] = LEDCODE_BLANK;
					PnlDsp->LedCode[3] = LEDCODE_BLANK;
					PnlDsp->LedCode[2] = LEDCODE_BLANK;
					PnlDsp->LedCode[1] = LEDCODE_BLANK;
					PnlDsp->LedCode[0] = LedLightPatternTbl[FnCmnCtrl->AnlgLedChkCnt];
					break;

				case PNLKEY_DOWN:
					/* ���݌o�ߎ��Ԏ擾 */
					KpiRstLongTimer(&Pnl.ChkTime);

					PnlDsp->LedCode[4] = LEDCODE_BLANK;
					PnlDsp->LedCode[3] = LEDCODE_BLANK;
					PnlDsp->LedCode[2] = LEDCODE_BLANK;
					PnlDsp->LedCode[1] = LedLightPatternTbl[FnCmnCtrl->AnlgLedChkCnt];
					PnlDsp->LedCode[0] = LEDCODE_BLANK;
					break;

				case PNLKEY_UP:
					/* ���݌o�ߎ��Ԏ擾 */
					KpiRstLongTimer(&Pnl.ChkTime);

					PnlDsp->LedCode[4] = LEDCODE_BLANK;
					PnlDsp->LedCode[3] = LEDCODE_BLANK;
					PnlDsp->LedCode[2] = LedLightPatternTbl[FnCmnCtrl->AnlgLedChkCnt];
					PnlDsp->LedCode[1] = LEDCODE_BLANK;
					PnlDsp->LedCode[0] = LEDCODE_BLANK;
					break;

				case PNLKEY_MODE:
					/* ���݌o�ߎ��Ԏ擾 */
					KpiRstLongTimer(&Pnl.ChkTime);

					PnlDsp->LedCode[4] = LEDCODE_BLANK;
					PnlDsp->LedCode[3] = LedLightPatternTbl[FnCmnCtrl->AnlgLedChkCnt];
					PnlDsp->LedCode[2] = LEDCODE_BLANK;
					PnlDsp->LedCode[1] = LEDCODE_BLANK;
					PnlDsp->LedCode[0] = LEDCODE_BLANK;
					break;

				default:
					/* ���݌o�ߎ��Ԏ擾 */
					KpiRstLongTimer(&Pnl.ChkTime);

					PnlDsp->LedCode[4] = LedLightPatternTbl[FnCmnCtrl->AnlgLedChkCnt];
					PnlDsp->LedCode[3] = LEDCODE_BLANK;
					PnlDsp->LedCode[2] = LEDCODE_BLANK;
					PnlDsp->LedCode[1] = LEDCODE_BLANK;
					PnlDsp->LedCode[0] = LEDCODE_BLANK;
					break;
			}

			/* �J�E���^�X�V */
			FnCmnCtrl->AnlgLedChkCnt++;

			if(FnCmnCtrl->AnlgLedChkCnt > LED_CHK_CNT_LMIT)
			{
				/* �J�E���^���Z�b�g */
				FnCmnCtrl->AnlgLedChkCnt = 0U;
			}

			KpiRstLongTimer(&Pnl.ChkTime);

			FnCmnCtrl->AnlgLedChkLstPnlBtn = PnlBtn;	/* �O��X�L�������{�^���X�V */

		}
	}

/* <S0FB> Start */
		for( i = 0; i < PNL_N_LED; i++ )
		{
			PnlDsp->LedBlnkSw[i] = 0;
		}
/* <S0FB> End */

	return;
}
/* <S0CB> End */

#endif /* SVFSEL_PANELOP_USE == 1 */
/***************************************** end of file **********************************************/
