/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PnlOp.c : �p�l���I�y���[�^�������W���[��													*/
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
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Round.h"





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
	LEDDSP	DspData;		/* �\���f�[�^															*/
	ULONG	BlnkTime;		/* �p�l���\���_�Ŏ�������p												*/
	USHORT	CurPos;			/* �p�l�� �p�����[�^���݌�/���j�^�[���ݕ\����							*/
	USHORT	CurPosMax;		/* �p�l�� �p�����[�^���݌�/���j�^�[���ݕ\���� �͈�						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CurFunIndx;		/* �p�l���⏕�@�\���݈ʒu�C���f�b�N�X									*/
	USHORT	CurPrmIndx;		/* �p�l���p�����[�^���݈ʒu�C���f�b�N�X									*/
	USHORT	PrmWrState;		/* �p�l���p�����[�^�������ݎ����										*/
	USHORT	CurMonIndx;		/* �p�l�����j�^�[���݈ʒu�C���f�b�N�X									*/
/*--------------------------------------------------------------------------------------------------*/
} PNL_STRUC;
/*--------------------------------------------------------------------------------------------------*/
/*		Local Variable Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
#pragma section sidata begin
PNL_STRUC	Pnl;			/* Panel Operator Variable												*/
extern MSGIF PopMsg;		/* Panel Operator Message I/F Data										*/
#pragma section sidata end
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Function List																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�l���I�y���[�^�h�e�֐�																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxPnlOpMain( LONG ComMode );		/* Panel Operator Main Program							*/
void	RpxPnlOpInit( void );				/* Panel Operator Initialize							*/
void	RpxPnlOpMonInit( void );			/* Panel Monitor Initialize								*/
void	RpxPnlOpFinish( void );				/* Panel Operator Finish								*/
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
		LEDDSP	*leddspp		);			/* LED�\���f�[�^�̃A�h���X								*/
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
void	PnlKeyTranslate( void );			/* �p�l���I�y���[�^ �L�[�ϊ�����						*/
void	PnlStatusDispMode( void );			/* �p�l���I�y���[�^ ��ԕ\������						*/
void	PnlFunNoMode( void );				/* �p�l���I�y���[�^ Fn�ԍ��ҏW���[�h					*/
void	PnlFunExeMode( void );				/* �p�l���I�y���[�^ Fn���s���[�h						*/
void	PnlPrmNoMode( void );				/* �p�l���I�y���[�^ Pn�ԍ��ҏW���[�h					*/
void	PnlPrmDataMode( void );				/* �p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h				*/
void	PnlMonNoMode( void );				/* �p�l���I�y���[�^ Un�ԍ��ҏW���[�h					*/
void	PnlMonDataMode( void );				/* �p�l���I�y���[�^ Un�f�[�^�\�����[�h					*/
void	PnlLedBlink( void );				/* �p�l���I�y���[�^ LED�u�����N����						*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecWordUnsigned(		/* �p�l���I�y���[�^ LED�\��(�p�����[�^������10�i���[�h)	*/
		USHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals		);			/* �����_�ȉ�����										*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecWordSigned(			/* �p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)	*/
		SHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals, 					/* �����_�ȉ�����										*/
		SHORT	llimit,						/* �����l												*/
		SHORT	ulimit			);			/* ����l												*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmHexNblWord(			/* �p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)		*/
		USHORT	data,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	base			);			/* �\���̒�												*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecLong(				/* �p�l���I�y���[�^ LED�\��(10�i�����O)					*/
		ULONG	ldata,						/* �\������l											*/
		USHORT	cur_digit,					/* ���݃J�[�\���ʒu										*/
		USHORT	decimals,					/* �����_�ȉ�����										*/
		USHORT	sign			);			/* �����L��												*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmHexLong(				/* �p�l���I�y���[�^ LED�\��(16�i�����O)					*/
		ULONG	ldata,						/* �\������l											*/
		USHORT	cur_digit		);			/* ���݃J�[�\���ʒu										*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispMonHexWord(				/* �p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)			*/
		USHORT	data			);			/* �\������l											*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispMonBit(					/* �p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)		*/
		UCHAR	data			);			/* �\������l											*/
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





/****************************************************************************************************/
/*																									*/
/*		Panel Operator Main Program																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpMain( LONG ComMode )
{
USHORT	CurAlarm;
LONG	rc_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM��ڑ���																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( ComMode == SCM_NOCOM )
		{
			CurAlarm = KpiGetAlarmInfo();
		/*------------------------------------------------------------------------------------------*/
		/*	�A���[���������͏�ԕ\�����[�h��														*/
		/*------------------------------------------------------------------------------------------*/
			if( (CurAlarm != 0) && ((CurAlarm & 0xF00) != 0x900) && (Pnl.LastAlarm == 0) )
			{
				Pnl.CurPos = 0;
				if(Pnl.DispMode == PNLMODE_FNEXE)	RpxFunFinishPnl(Pnl.CurFunIndx, &PopMsg);
				if(Pnl.DispMode == PNLMODE_PNDATA)
				{
					rc_tmp = RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx);
					while(rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL)
					{
						rc_tmp = RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx);
					}
				}
				Pnl.DispMode = PNLMODE_STATUS;
			}
			Pnl.LastAlarm = CurAlarm;
		/*------------------------------------------------------------------------------------------*/
		/*																							*/
		/*------------------------------------------------------------------------------------------*/
			PnlKeyTranslate();
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
					PnlStatusDispMode();
					if(OpKeyMk.rst)
					{
						KriResetSystemAlarm();
						NdiClearAlarm();
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
					if(RpxFunExeBeginPnl(Pnl.CurFunIndx, &PopMsg) != TRUE)	/* Fn���s�ł��Ȃ���	*/
					{
						OpBlnkKickFlg.no_op = 1;
					}
					else
					{
						Pnl.DispMode = PNLMODE_FNEXE;
						Pnl.CurPos = 0;
					}
				}
				else	PnlFunNoMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_FNEXE:		/* Fn���s	*/
		/*------------------------------------------------------------------------------------------*/
				if(RpxFunEndChkPnl(Pnl.CurFunIndx, &PopMsg))
				{
					RpxFunFinishPnl(Pnl.CurFunIndx, &PopMsg);
					Pnl.CurPos = 0;
					Pnl.DispMode = PNLMODE_FNNUM;
				}
				else	PnlFunExeMode();
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
					if( Iprm.f.PrmWrtInhibit && (PopMsg.AccessLvl != ACCLVL_SYSTEM) )
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
				else	PnlPrmNoMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_PNDATA:	/* Pn�f�[�^	*/
		/*------------------------------------------------------------------------------------------*/
				PnlPrmDataMode();
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
				else	PnlMonNoMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_UNDATA:	/* Un�f�[�^	*/
		/*------------------------------------------------------------------------------------------*/
				PnlMonDataMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			default:
		/*------------------------------------------------------------------------------------------*/
				break;
			}
			PnlLedBlink();
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM�ڑ���																			*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			PnlStatusDispMode();
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK,
							 sizeof(RoutK.PnlLed), ((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x02) );
		}
/*--------------------------------------------------------------------------------------------------*/
		PnlLedSetKnlBuffer( &Pnl.DspData, RoutK.PnlLed, sizeof(RoutK.PnlLed) );
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ �L�[�ϊ�����																*/
/*																									*/
/****************************************************************************************************/
void	PnlKeyTranslate( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�ϐ�������																					*/
/*--------------------------------------------------------------------------------------------------*/
//		*((UINT *)&OpKeyMk)  = 0;
//		*((UINT *)&OpKeyBrk) = 0;
//		*((UINT *)&OpKeyLvl) = 0;
		*((UINT *)&OpKeyMk)  = 	*((UINT *)&OpKeyBrk) = *((UINT *)&OpKeyLvl) = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N���̓L�[���͂𖳎�����																*/
/*--------------------------------------------------------------------------------------------------*/
		if( *((UINT *)&OpBlnkFlg) != 0 )
		{
			Pnl.BtnLast = RinK.PnlBtn;
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���̓��x���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( RinK.PnlBtn & PNLKEY_MODE	){ OpKeyLvl.mode = TRUE;}
		if( RinK.PnlBtn & PNLKEY_UP		){ OpKeyLvl.up   = TRUE;}
		if( RinK.PnlBtn & PNLKEY_DOWN	){ OpKeyLvl.down = TRUE;}
		if( RinK.PnlBtn & PNLKEY_SHUP	){ OpKeyLvl.shup = TRUE;}
		if( (RinK.PnlBtn & PNLKEY_RST) == PNLKEY_RST ){ OpKeyLvl.rst = TRUE;}
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͂ɕω������������̂݃Z�b�g(=�`���^�����O�h�~)										*/
/*--------------------------------------------------------------------------------------------------*/
		if( RinK.PnlBtn != Pnl.BtnLast )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�L�[����MAKE�̏���																		*/
		/*------------------------------------------------------------------------------------------*/
			switch( RinK.PnlBtn )
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
			switch(RinK.PnlBtn)
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
	Pnl.BtnLast = RinK.PnlBtn;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ ��ԕ\������																*/
/*																									*/
/****************************************************************************************************/
void	PnlStatusDispMode( void )
{
USHORT	CurAlarm;

/*--------------------------------------------------------------------------------------------------*/
/*		��ԕ\��																					*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_B7;
		Pnl.DspData.LedCode[3] = LEDCODE_BLANK;
		RpxLedSetStatusBits( &(Pnl.DspData) );
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[��������																				*/
/*--------------------------------------------------------------------------------------------------*/
		CurAlarm = KpiGetAlarmInfo();
		if( Alm.f.AlmFlag )
		{
			/* �A���[���R�[�h�\��	*/
			RpxLedSetValHex(CurAlarm, &(Pnl.DspData), 2, 3);
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���[�j���O������																			*/
/*--------------------------------------------------------------------------------------------------*/
		else if( CurAlarm != 0 )
		{
			/* ���[�j���O�R�[�h�\��	*/
			RpxLedSetValHex(CurAlarm, &(Pnl.DspData), 2, 3);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʏ�\��(�A���[�������[�j���O����)															*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( RinV.f.BaseEnable )
			{
				if( !MencV.f.PhaseReady )
				{
					Pnl.DspData.LedCode[3] = LEDCODE_P;	/* " P-dt"	*/
					Pnl.DspData.LedCode[2] = LEDCODE_B6;
					Pnl.DspData.LedCode[1] = LEDCODE_D;
					Pnl.DspData.LedCode[0] = LEDCODE_T;
				}
				else
				{
					Pnl.DspData.LedCode[2] = LEDCODE_R;	/* "  run"	*/
					Pnl.DspData.LedCode[1] = LEDCODE_sU;
					Pnl.DspData.LedCode[0] = LEDCODE_N;
				}
			}
			else
			{
				Pnl.DspData.LedCode[2] = LEDCODE_BLANK;	/* "   bb"	*/
				Pnl.DspData.LedCode[1] = LEDCODE_B;
				Pnl.DspData.LedCode[0] = LEDCODE_B;
			}
		/*------------------------------------------------------------------------------------------*/
			if( BoutV.CtrlMode.b.cm != CTRLMODE_JOG && BoutV.CtrlMode.b.cm != CTRLMODE_ZSRCH )
			{
				if(CinV.f.Sv.Pot && CinV.f.Sv.Not)	/* "Pot" AND "not" */
				{
					if(!((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x04))	/* ����800ms	*/
					{
						Pnl.DspData.LedCode[2] = LEDCODE_P;
						Pnl.DspData.LedCode[1] = LEDCODE_sO;
						Pnl.DspData.LedCode[0] = LEDCODE_T;
					}
					else
					{
						Pnl.DspData.LedCode[2] = LEDCODE_N;
						Pnl.DspData.LedCode[1] = LEDCODE_sO;
						Pnl.DspData.LedCode[0] = LEDCODE_T;
					}
				}
				else if(CinV.f.Sv.Pot)				/* "Pot" */
				{
					Pnl.DspData.LedCode[2] = LEDCODE_P;
					Pnl.DspData.LedCode[1] = LEDCODE_sO;
					Pnl.DspData.LedCode[0] = LEDCODE_T;
				}
				else if(CinV.f.Sv.Not)				/* "Not" */
				{
					Pnl.DspData.LedCode[2] = LEDCODE_N;
					Pnl.DspData.LedCode[1] = LEDCODE_sO;
					Pnl.DspData.LedCode[0] = LEDCODE_T;
				}
			}
		/*------------------------------------------------------------------------------------------*/
			if( CoutV.f.Sv.HwbbState )				/* "Hbb" */
			{
				Pnl.DspData.LedCode[3] = LEDCODE_BLANK; /* ���F���Ɍ��o����OT��ALM�ƂȂ邽�ߕs�v*/
				Pnl.DspData.LedCode[2] = LEDCODE_H;
				Pnl.DspData.LedCode[1] = LEDCODE_B;
				Pnl.DspData.LedCode[0] = LEDCODE_B;
			}
		/*------------------------------------------------------------------------------------------*/
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Fn�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlFunNoMode( void )
{
USHORT	i, fnnum_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.shup )
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if( OpKeyMk.up )
		{
			fnnum_tmp = FnFunTbl[Pnl.CurFunIndx].FnNo + (0x01 << (Pnl.CurPos*4));
			if(fnnum_tmp >= 0x1000)	fnnum_tmp = 0xFFF;
			Pnl.CurFunIndx = RpxFunSearchIdxOp(fnnum_tmp, Pnl.CurFunIndx, &PopMsg);
		}
		else if( OpKeyMk.down )
		{
			if(FnFunTbl[Pnl.CurFunIndx].FnNo < (0x01 << (Pnl.CurPos*4)))
			{
				fnnum_tmp = 0x000;
			}
			else
			{
				fnnum_tmp = FnFunTbl[Pnl.CurFunIndx].FnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurFunIndx = RpxFunSearchIdxOp(fnnum_tmp, Pnl.CurFunIndx, &PopMsg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\������																					*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_F;
		Pnl.DspData.LedCode[3] = LEDCODE_N;
		RpxLedSetValHex(FnFunTbl[Pnl.CurFunIndx].FnNo, &(Pnl.DspData), 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Fn���s���[�h																*/
/*																									*/
/****************************************************************************************************/
void	PnlFunExeMode( void )
{
		RpxFunExePnl( OpKeyMk, OpKeyBrk, OpKeyLvl, Pnl.CurFunIndx,
							&(Pnl.DspData), &OpBlnkKickFlg, &PopMsg );
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Pn�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlPrmNoMode( void )
{
USHORT	i, pnnum_tmp;
PRMDEF	*prmdef_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.shup )
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if( OpKeyMk.up )
		{
			pnnum_tmp = PnPrmTbl[Pnl.CurPrmIndx].PnNo + (0x01 << (Pnl.CurPos*4));
			if(pnnum_tmp >= 0x1000)	pnnum_tmp = 0xFFF;
			Pnl.CurPrmIndx = RpxPrmSearchIdxOp(pnnum_tmp, Pnl.CurPrmIndx, &PopMsg);
		}
		else if( OpKeyMk.down )
		{
			if(PnPrmTbl[Pnl.CurPrmIndx].PnNo < (0x01 << (Pnl.CurPos*4)))
			{
				pnnum_tmp = 0x000;
			}
			else
			{
				pnnum_tmp = PnPrmTbl[Pnl.CurPrmIndx].PnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurPrmIndx = RpxPrmSearchIdxOp(pnnum_tmp, Pnl.CurPrmIndx, &PopMsg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���݌��ő�l																				*/
/*--------------------------------------------------------------------------------------------------*/
		prmdef_tmp = (PRMDEF *)PnPrmTbl[Pnl.CurPrmIndx].PrmDefPtr;	/* �p�����[�^��`	*/
		if( prmdef_tmp->Attr.IfIsLong )	/* �����O�p�����[�^	*/
		{
			Pnl.CurPosMax = (prmdef_tmp->Attr.Base == DSPDEF_BASEDEC)? 10: 8;
		}
		else							/* ���[�h�p�����[�^	*/
		{
			if( prmdef_tmp->Attr.Base == DSPDEF_BASEDEC )
			{
				if( (prmdef_tmp->Attr.Sign == DSPDEF_W_SIGN) &&
					( (-10000 < (SHORT)RpxPrmLowerLimitIdx(Pnl.CurPrmIndx)) && 
					  ((SHORT)RpxPrmUpperLimitIdx(Pnl.CurPrmIndx) < 10000) )
				)
				{
					Pnl.CurPosMax = 4;	/* �����܂�5���ő����ꍇ	*/
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
		Pnl.DspData.LedCode[4] = LEDCODE_P;
		Pnl.DspData.LedCode[3] = LEDCODE_N;
		RpxLedSetValHex(PnPrmTbl[Pnl.CurPrmIndx].PnNo, &(Pnl.DspData), 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h													*/
/*																									*/
/****************************************************************************************************/
void	PnlPrmDataMode( void )
{
ULONG	ldata_tmp;
PRMDEF	*prmdef_tmp;
LONG	rc;

		prmdef_tmp = (PRMDEF *)PnPrmTbl[Pnl.CurPrmIndx].PrmDefPtr;	/* �p�����[�^��`	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW����														*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Pnl.PrmWrState )
		{
	  /*--------------------------------------------------------------------------------------------*/
		case OPWM_None:	/* �p�����[�^�l�\���E�ҏW����	*/
	  /*--------------------------------------------------------------------------------------------*/
			RpxPrmReadIdx(Pnl.CurPrmIndx, &ldata_tmp, PCHK_OPE_READ);	/* ldata_tmp <-- Prm�l		*/
			if( OpKeyMk.data | OpKeyMk.mode )	/* EEPROM�������ݏ���	*/
			{
				switch(RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx))
				{
				case OK:	/* ����	*/
					OpBlnkKickFlg.PrmEewrite = 1;	/* �u�����N�N��	*/
					break;
				case PRM_EEPROM_WRITE_FAIL:	/* �܂�Queue���󂢂Ă��Ȃ�	*/
					Pnl.PrmWrState = OPWM_UpdtWait;
					break;
				case PERR_NO:					/* RAM�݂̂̃p�����[�^								*/
				case PRM_EEPROM_RAM_NO_DIFF:	/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
					Pnl.CurPos = 0;
					Pnl.DispMode = PNLMODE_PNNUM;	/* �p�����[�^�ԍ����[�h��	*/
					return;
					break;
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
				ldata_tmp = (OpKeyMk.up)? 
					RpxPrmRamDigitInc(Pnl.CurPrmIndx, ldata_tmp, Pnl.CurPos):
					RpxPrmRamDigitDec(Pnl.CurPrmIndx, ldata_tmp, Pnl.CurPos);
				rc = RpxPrmWriteRamIdx(Pnl.CurPrmIndx, ldata_tmp, PCHK_OPE_WRITE, &PopMsg);
				if( rc == PERR_CALC )
				{
					OpBlnkKickFlg.error = 1;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�\������																				*/
		/*------------------------------------------------------------------------------------------*/
			if( prmdef_tmp->Attr.IfIsLong )	/* �����O�p�����[�^	*/
			{
				if(prmdef_tmp->Attr.Base == DSPDEF_BASEDEC)	/* �ꂪ10�i	*/
				{
					PnlLedDispPrmDecLong(ldata_tmp, Pnl.CurPos, 
						(USHORT)RpxPrmGetDecimals( prmdef_tmp ), (USHORT)prmdef_tmp->Attr.Sign);
				}
				else								/* �ꂪ10�i�ȊO	*/
				{
					PnlLedDispPrmHexLong(ldata_tmp, Pnl.CurPos);
				}
				
			}
			else							/* ���[�h�p�����[�^	*/
			{
				if(prmdef_tmp->Attr.Base == DSPDEF_BASEDEC)	/* �ꂪ10�i	*/
				{
					if(prmdef_tmp->Attr.Sign == DSPDEF_W_SIGN)
					{
						PnlLedDispPrmDecWordSigned((SHORT)ldata_tmp, Pnl.CurPos, 
							(USHORT)RpxPrmGetDecimals( prmdef_tmp ),
							(SHORT)RpxPrmLowerLimitIdx(Pnl.CurPrmIndx), 
							(SHORT)RpxPrmUpperLimitIdx(Pnl.CurPrmIndx));
					}
					else
					{
						PnlLedDispPrmDecWordUnsigned((USHORT)ldata_tmp, Pnl.CurPos, 
							(USHORT)RpxPrmGetDecimals( prmdef_tmp ) );
					}
				}
				else								/* �ꂪ10�i�ȊO	*/
				{
					PnlLedDispPrmHexNblWord((USHORT)ldata_tmp, Pnl.CurPos, 
						(USHORT)prmdef_tmp->Attr.Base);
				}
			}
			break;
	  /*--------------------------------------------------------------------------------------------*/
		case OPWM_UpdtWait:	/* EEPROM��������(�v���o�^)����	*/
	  /*--------------------------------------------------------------------------------------------*/
			switch(RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx))
			{
			case OK:	/* ����	*/
				OpBlnkKickFlg.PrmEewrite = 1;	/* �u�����N�N��	*/
				Pnl.PrmWrState = OPWM_None;
				break;
			case PRM_EEPROM_RAM_NO_DIFF:	/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
				Pnl.PrmWrState = OPWM_None;
				Pnl.CurPos = 0;
				Pnl.DispMode = PNLMODE_PNNUM;	/* �p�����[�^�ԍ����[�h��	*/
				return;
				break;
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
void	PnlMonNoMode( void )
{
USHORT	i, unnum_tmp;
MONDEF	*mondef_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpKeyMk.shup)
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if(OpKeyMk.up)
		{
			unnum_tmp = UnMonTbl[Pnl.CurMonIndx].UnNo + (0x01 << (Pnl.CurPos*4));
			Pnl.CurMonIndx = RpxMonSearchIdxOp(unnum_tmp, Pnl.CurMonIndx, &PopMsg);
		}
		else if(OpKeyMk.down)
		{
			if(UnMonTbl[Pnl.CurMonIndx].UnNo < (0x01 << (Pnl.CurPos*4)))
			{
				unnum_tmp = 0x000;
			}
			else
			{
				unnum_tmp = UnMonTbl[Pnl.CurMonIndx].UnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurMonIndx = RpxMonSearchIdxOp(unnum_tmp, Pnl.CurMonIndx, &PopMsg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���݌��ő�l																				*/
/*--------------------------------------------------------------------------------------------------*/
		mondef_tmp = (MONDEF *)UnMonTbl[Pnl.CurMonIndx].MonDefPtr;	/* ���j�^��`	*/
		if(mondef_tmp->IfIsLong == TRUE)			/* �T�C�Y�������O	*/
		{
			Pnl.CurPosMax = 3;	/* �\���ؑւ���(���E���E��)	*/
		}
		else										/* �T�C�Y�����[�h�ȉ�	*/
		{
			Pnl.CurPosMax = 1;	/* �\���ؑւȂ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�k�d�c�\������																				*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_U;
		Pnl.DspData.LedCode[3] = LEDCODE_N;
		RpxLedSetValHex(UnMonTbl[Pnl.CurMonIndx].UnNo, &(Pnl.DspData), 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Un�f�[�^�\�����[�h															*/
/*																									*/
/****************************************************************************************************/
void	PnlMonDataMode( void )
{
ULONG	ldata_tmp;
MONDEF	*mondef_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		�\���f�[�^�擾																				*/
/*--------------------------------------------------------------------------------------------------*/
		mondef_tmp = (MONDEF *)UnMonTbl[Pnl.CurMonIndx].MonDefPtr;	/* ���j�^��`					*/
		RpxMonReadIdx( Pnl.CurMonIndx, &ldata_tmp, MCHK_ACCS_LVL );	/* ldata_tmp <-- ���j�^�l		*/
/*--------------------------------------------------------------------------------------------------*/
/*		�L�[���͏���																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpKeyMk.data)
		{
			Pnl.CurPos = 0;
			Pnl.DispMode = PNLMODE_UNNUM;	/* �p�����[�^�ԍ����[�h��	*/
			return;
		}
		else if(OpKeyMk.shup)
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == Pnl.CurPosMax)?  0 : Pnl.CurPos+1;
		}
		else if(OpKeyMk.rst)
		{
			RpxMonClearVarIdx( Pnl.CurMonIndx );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\������																					*/
/*--------------------------------------------------------------------------------------------------*/
		switch(mondef_tmp->Base)
		{
		case DSPDEF_BASEDEC:
			if(mondef_tmp->IfIsLong == FALSE)	/* ���[�h	*/
			{
				RpxLedDispMonDecWord((USHORT)ldata_tmp, &(Pnl.DspData),
					mondef_tmp->Decimals, mondef_tmp->Sign);
			}
			else								/* �����O	*/
			{
				RpxLedDispMonDecLong(ldata_tmp, &(Pnl.DspData), Pnl.CurPos, 
					mondef_tmp->Decimals, mondef_tmp->Sign);
			}
			break;
		case DSPDEF_BASEHEX:
			PnlLedDispMonHexWord((USHORT)ldata_tmp);	/* ���݂̓��[�h�����Ȃ�	*/
			break;
		case DSPDEF_BASEBIT:
			PnlLedDispMonBit((UCHAR)ldata_tmp);			/* ����8�r�b�g��\������	*/
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
void	PnlLedBlink( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N�I���`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
		if(*((UINT *)&OpBlnkFlg) != 0)
		{
			if(KpiGetLongTimer(&OpBlnkTime) > 1000)
			{	/* �u�����N���Ԃ��I������	*/
				*((UINT *)&OpBlnkFlg) = 0;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N�J�n������																			*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpBlnkKickFlg.PrmEewrite)	/* �p�����[�^�����ݎ�	*/
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.PrmEewrite = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.no_op)	/* read/write�L�[�Ȃ�	*/
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
		if(OpBlnkFlg.PrmEewrite)	/* �p�����[�^�����ݎ��u�����N	*/
		{
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.no_op)	/* �p�l���ɂ�read/write�L�[�͂Ȃ�	*/
		{
			/* "no_oP"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_N;
			Pnl.DspData.LedCode[3] = LEDCODE_sO;
			Pnl.DspData.LedCode[2] = LEDCODE_B3;
			Pnl.DspData.LedCode[1] = LEDCODE_sO;
			Pnl.DspData.LedCode[0] = LEDCODE_P;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.error)
		{
			/* "Error"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_E;
			Pnl.DspData.LedCode[3] = LEDCODE_R;
			Pnl.DspData.LedCode[2] = LEDCODE_R;
			Pnl.DspData.LedCode[1] = LEDCODE_sO;
			Pnl.DspData.LedCode[0] = LEDCODE_R;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.done)
		{
			/* " donE"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_BLANK;
			Pnl.DspData.LedCode[3] = LEDCODE_D;
			Pnl.DspData.LedCode[2] = LEDCODE_sO;
			Pnl.DspData.LedCode[1] = LEDCODE_N;
			Pnl.DspData.LedCode[0] = LEDCODE_E;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.end)
		{
			/* "  End"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_BLANK;
			Pnl.DspData.LedCode[3] = LEDCODE_BLANK;
			Pnl.DspData.LedCode[2] = LEDCODE_E;
			Pnl.DspData.LedCode[1] = LEDCODE_N;
			Pnl.DspData.LedCode[0] = LEDCODE_D;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed),
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
/*		Panel Monitor Initialize																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpMonInit( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Panel Monitor Initialize																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (pPrm->Pn52F != PNLMON_NOSET) && (KpiGetAlarmInfo( ) == 0) )
		{
			if( RpiMonSearch( pPrm->Pn52F, 0, &(Pnl.CurMonIndx), MCHK_ACCS_LVL, &PopMsg ) == OK )
			{
				Pnl.DispMode = PNLMODE_UNDATA;
				Pnl.CurPos = 0;
				if ( UnMonTbl[ Pnl.CurMonIndx ].MonDefPtr->IfIsLong == TRUE )	/* �T�C�Y�������O	*/
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
void	RpxPnlOpFinish( void )
{
LONG	rc_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		���s���̕⏕�@�\���I�������鏈���B���s�����ۂ��̔���͊֐����ōs���Ă���B					*/
/*--------------------------------------------------------------------------------------------------*/
		RpxFunFinishPnl( Pnl.CurFunIndx, &PopMsg );
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^��EEPROM�ɏ������ޕK�v������ꍇ�̏���											*/
/*--------------------------------------------------------------------------------------------------*/
		rc_tmp = RpxPrmUpdateEepromIdx( Pnl.CurPrmIndx );
		while( rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL )	/* ����	*/
		{
			rc_tmp = RpxPrmUpdateEepromIdx( Pnl.CurPrmIndx );
		}
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
		USHORT	decimals		)			/* �����_�ȉ�����										*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED�\��(�p�����[�^�����Ȃ�10�i���[�h)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 5 ){ return;}
		RpxLedSetValDec( data, &(Pnl.DspData), 4, 5, FALSE, decimals );
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == cur_digit)? 0xFF: 0;
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
		SHORT	ulimit			)			/* ����l												*/
{
USHORT	sdata_tmp, sdata_tmp1, sdata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 5 ){ return;}
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		sdata_tmp = (data < 0)? -data: data;
/*--------------------------------------------------------------------------------------------------*/
/*		�����܂�5���ő����ꍇ																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (-10000 < llimit) && (ulimit < 10000) )
		{
			RpxLedSetValDec(sdata_tmp, &(Pnl.DspData), 3, 4, FALSE, decimals);
			Pnl.DspData.LedCode[4] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
			Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;	/* �J�[�\����	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����܂�5���ő���Ȃ��ꍇ-->�ؑ֕\��														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if(Pnl.CurPos < 4)	/* ��4��	*/
			{
				sdata_tmp1 = sdata_tmp/10000;
				sdata_tmp2 = sdata_tmp - sdata_tmp1*10000;
				RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 3, 4, FALSE, decimals);
				Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;						/* �J�[�\����	*/
				Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B3;	/* �\�����\��	*/
//				if( data < 0 ){ Pnl.DspData.LedCode[4] |= LEDCODE_POINT;}		/* ���\����'.'	*/
			}
			else				/* ��1��+����	*/
			{
				sdata_tmp1 = sdata_tmp/10000;
				RpxLedSetValDec(sdata_tmp1, &(Pnl.DspData), 0, 1, FALSE, (USHORT)(decimals-4));
				Pnl.DspData.LedCode[2] = Pnl.DspData.LedCode[1] = LEDCODE_BLANK;
				Pnl.DspData.LedCode[3] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
				Pnl.DspData.LedBlnkSw[cur_digit-4] = 0xFF;						/* �J�[�\����	*/
				Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
			}
			if( data < 0 ){ Pnl.DspData.LedCode[4] |= LEDCODE_POINT;}			/* ���\����'.'	*/
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
		USHORT	base			)			/* �\���̒�												*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED�\��(�p�����[�^16�i���[�h)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 4 ){ return;}
		Pnl.DspData.LedCode[4] = (base == DSPDEF_BASENBL)? 
			(LEDCODE_N | LEDCODE_POINT): (LEDCODE_H | LEDCODE_POINT);
		RpxLedSetValHex( data, &(Pnl.DspData), 3, 4 );
		for(i = 0; i < sizeof(RoutK.PnlLed); i++)
		{
			Pnl.DspData.LedBlnkSw[i] = (i == cur_digit)? 0xFF: 0;
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
		USHORT	sign			)			/* �����L��												*/
{
USHORT	sdata_tmp2;
ULONG	ldata_abs, ldata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 10 ){ return;}
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		ldata_abs = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?	-(LONG)ldata: ldata;
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		if(Pnl.CurPos < 4)
		{
			ldata_tmp2 = ldata_abs/10000;
			sdata_tmp2 = (USHORT)(ldata_abs - ldata_tmp2*10000);
			RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 3, 4, FALSE, decimals);
			Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;						/* �J�[�\����	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B3;	/* �\�����\��	*/
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
			RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 3, 4, FALSE, (USHORT)(decimals-4));
			Pnl.DspData.LedBlnkSw[cur_digit-4] = 0xFF;						/* �J�[�\����	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
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
			Pnl.DspData.LedCode[3] = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?
				LEDCODE_MINUS : LEDCODE_BLANK;
			Pnl.DspData.LedCode[2] = LEDCODE_BLANK;
			sdata_tmp2 = (USHORT)(ldata_abs/100000000);
			RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 1, 2, FALSE, (USHORT)(decimals-8));
			Pnl.DspData.LedBlnkSw[cur_digit-8] = 0xFF;						/* �J�[�\����	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B0;	/* �\�����\��	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���\����'.'																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
		{
			Pnl.DspData.LedCode[4] |= LEDCODE_POINT;
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
		USHORT	cur_digit		)			/* ���݃J�[�\���ʒu										*/
{
USHORT	sdata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit > 8 ){ return;}
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		if(Pnl.CurPos < 4)
		{
			sdata_tmp2 = (USHORT)(ldata & 0xFFFF);
			RpxLedSetValHex(sdata_tmp2, &(Pnl.DspData), 3, 4);
			Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;						/* �J�[�\����	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B3;	/* �\�����\��	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		��4��																						*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			sdata_tmp2 = (USHORT)((ldata >> 16) & 0xFFFF);
			RpxLedSetValHex(sdata_tmp2, &(Pnl.DspData), 3, 4);
			Pnl.DspData.LedBlnkSw[cur_digit-4] = 0xFF;						/* �J�[�\����	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
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
		USHORT	data			)			/* �\������l											*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		LED�\��(���j�^�[16�i���[�h)																	*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_H | LEDCODE_POINT;
		RpxLedSetValHex(data, &(Pnl.DspData), 3, 4);
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispMonBit(					/* �p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)		*/
		UCHAR	data			)			/* �\������l											*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		�ϐ�������																					*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = Pnl.DspData.LedCode[i] = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		8�r�b�g��\��																				*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < 8; i++)
		{
			switch(i & 1)
			{
			case 0:		/* �����Ԗ� --> �E	*/
				Pnl.DspData.LedCode[i/2] |= (data & 1)? LEDCODE_B1 : LEDCODE_B2;
				break;
			case 1:		/* ��Ԗ� --> ��	*/
				Pnl.DspData.LedCode[i/2] |= (data & 1)? LEDCODE_B5 : LEDCODE_B4;
				break;
			default:
				break;
			}
			data >>= 1;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Hi/Lo�w�W����ы�؂�_�̕\�� 																*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_B1 | LEDCODE_B2 | LEDCODE_POINT;
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
		PnlLedSetBlnkSw( leddspp, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
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
		PnlLedSetBlnkSw( leddspp, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
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
		LEDDSP	*leddspp		)			/* LED�\���f�[�^�̃A�h���X								*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		�X�e�[�^�X�r�b�g�\�����Z�b�g																*/
/*--------------------------------------------------------------------------------------------------*/
		if( BoutV.f.Ctrl.Coin || CoutV.f.Ctrl.ReachSpd || (BoutV.CtrlMode.b.cm == CTRLMODE_TRQ) )
		{
			leddspp->LedCode[4] |= LEDCODE_B0;
		}
		if( !(RinV.f.BaseEnable)					){ leddspp->LedCode[4] |= LEDCODE_B6;}
		if( CoutV.f.Ctrl.MotorMoving				){ leddspp->LedCode[3] |= LEDCODE_B0;}
		if( CoutV.f.Sv.Prefon || CoutV.f.Sv.Vrefon	){ leddspp->LedCode[3] |= LEDCODE_B6;}
		if( CoutV.f.Sv.Trefon || CoutV.f.Sv.PerrClr	){ leddspp->LedCode[3] |= LEDCODE_B3;}
		if( CoutV.f.Seq.MainPowerOn					){ leddspp->LedCode[3] |= LEDCODE_B7;}
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
			var1_tmp = (USHORT)MlibDecPowerTbl[digit-1-i];	/* ���鐔: �� 1000, 100, 10, 1	*/
			var2_tmp = data/var1_tmp;
			if( sup_flg )	/* ��ʂ�0�̓u�����N�\���Ƃ���	*/
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
			data &= ((0x1 << (digit*4)) -1);	/* ��ʌ��𗎂Ƃ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\���l(10�i)�Z�b�g����																		*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < digit; i++)
		{
			var1_tmp = 0x1 << ((digit-1-i)*4);	/* ���鐔: �� 0x1000, 0x100, 0x10, 0x1	*/
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
		if( (length == 0) || (startdgt >= sizeof(RoutK.PnlLed)) )
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
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
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








/***************************************** end of file **********************************************/
