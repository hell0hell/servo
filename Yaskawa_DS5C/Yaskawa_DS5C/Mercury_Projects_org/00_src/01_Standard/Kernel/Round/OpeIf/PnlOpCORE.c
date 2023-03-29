/*****************************************************************************
Description		: �p�l���I�y���[�^�������W���[��
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	PnlOpManager();						:�p�l���I�y���[�^�Ǘ�
	PnlOpFinish();						:Panel Finish
	PnlOpInit();						:Panel Initialize
	PnlOpMonInit();						:Panel Monitor Initialize
	PnlOpLedDispMonDecWord();			:LED�I�y���[�^ ���j�^�[�\��(10�i���[�h��������^�Ȃ�)
	PnlOpLedDispMonDecLong();			:LED�I�y���[�^ ���j�^�[�\��(10�i�����O ��������^�Ȃ�)
	PnlOpLedSetStatusBits();	 		:LED�I�y���[�^ �X�e�[�^�X�r�b�g�\�����Z�b�g����
	PnlOpLedSetValDec();				:LED�I�y���[�^ �\���l(10�i)�Z�b�g����
	PnlOpLedSetValHex();				:LED�I�y���[�^ �\���l(16�i)�Z�b�g����
	PnlOpLedSetStr();	 				:LED Operator String Set
	PnlOpLedSetBlnk();					:LED Operator Blink Set

	PnlOpDisplayLedData();	  			:7segLED�\���f�[�^�Z�b�g
	PnlOpInitialize();					:Panel Op �֘A������

	-- Locals --
	PnlOpMain();						:Panel Operator Main Program
	PnlOpLedBlink();					:�p�l���I�y���[�^ LED�u�����N����
	PnlOpLedSetKnlBuffer();				:Panel LED Set to Kernel Variables
	PnlOpLedSetBlnkSw();				:Panel LED Set to Same Data
	PnlOpLedDispPrmDecWordUnsigned();	:�p�l���I�y���[�^ LED�\��(�p�����[�^�����Ȃ�10�i���[�h)
	PnlOpLedDispPrmDecWordSigned();		:�p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)
	PnlOpLedDispPrmHexNblWord();		:�p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)
	PnlOpLedDispPrmDecLong();			:�p�l���I�y���[�^ LED�\��(10�i�����O)
	PnlOpLedDispPrmHexLong();			:�p�l���I�y���[�^ LED�\��(16�i�����O)
	PnlOpLedDispMonHexWord();			:�p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)
	PnlOpLedDispMonBit();				:�p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)
	PnlOpStatusDispMode();				:�p�l���I�y���[�^ ��ԕ\������
	PnlOpFunNoMode();					:�p�l���I�y���[�^ Fn�ԍ��ҏW���[�h
	PnlOpFunExeMode();					:�p�l���I�y���[�^ Fn���s���[�h
	PnlOpPrmNoMode();					:�p�l���I�y���[�^ Pn�ԍ��ҏW���[�h
	PnlOpPrmDataMode();					:�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h
	PnlOpMonNoMode();					:�p�l���I�y���[�^ Un�ԍ��ҏW���[�h
	PnlOpMonDataMode();					:�p�l���I�y���[�^ Un�f�[�^�\�����[�h
	PnlOpSetLedMode();					:7segLED ��ԃZ�b�g

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.05.20		created

*****************************************************************************/
#include "PnlOp.h"
#include "PnlOpApi.h"

#define PNLOPE_WORDMON_MAX		9999		//<V610>

#define	PNLLED_BLINK		0xFFU		/* �p�l��LED�����}�X�N(�_�ŗp)									*/
#define	PNLLED_NOBLINK		0x00U		/* �p�l��LED�_���}�X�N(�_�ŗp)									*/
#define	PNLMON_NOSET		0xFFFU		/* �d�����������j�^�f�t�H���g									*/


extern	PRMTBL PnPrmTbl[];
extern	CFUNTBL FnFunTbl[];

/*--------------------------------------------------------------------------------------------------*/
/*		Global Variable Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
static  OPKEYFLG	OpKeyMk;							/* �L�[MAKE(OFF->ON)�t���O					*/
static  OPKEYFLG	OpKeyBrk;							/* �L�[BREAK(ON->OFF)�t���O					*/
static  OPKEYFLG	OpKeyLvl;							/* �L�[���x���t���O							*/
/*--------------------------------------------------------------------------------------------------*/
static  BLINKFLG	OpBlnkFlg;							/* �u�����N�t���O							*/
static  BLINKFLG	OpBlnkKickFlg;						/* �u�����N�N���t���O						*/
static  ULONG		OpBlnkTime;							/* �u�����N�t���O�u�����N���ԑ���p			*/
/*--------------------------------------------------------------------------------------------------*/
static  ACCESS		Access;								/* �A�N�Z�X���x���f�[�^						*/
static  MSGIF		PopMsg;						/* �p�l���I�y���[�^���W�X�^�h�e (�h�e���ʉ��̂���)	*/
static  const    CMONTBL	UnMonTbl[];

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


const ULONG decpowertbl[] =
{
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

const UCHAR ledcode[] =
{/* 0,  1,    2,    3,    4,    5,    6,    7 */
 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8,
/* 8,   9,    A,    b,    C,    d,    E,    F */
 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e,
#if ( CCSW_SFTID == SFTID_EXIST )											/*<V889>Mod:Start*/
/* P,    n,    A.,   H,   b.    t. */
 0x8c, 0xab, 0x08, 0x89, 0x03, 0x07};
#else
/* P,    n,    A.,   H,   b. */
 0x8c, 0xab, 0x08, 0x89, 0x03};
#endif


/****************************************************************************************************/
/*		PROTOTYPE																	    			*/
/****************************************************************************************************/
/* APIs */
void PnlOpManager( PNLOP_UNIT_HANDLE	hPnlOpUnitHndl );	/*	�p�l���I�y���[�^�Ǘ�	*/
void PnlOpFinish(PNLOP_HANDLE *hPnl);						/*	Panel Finish		*/
void PnlOpInit(PNLOP_HANDLE *hPnl);							/*	Panel Initialize	*/
void PnlOpMonInit( PNLOP_HANDLE *hPnl ,ALMMAN_HANDLE	*hAlmManager, USHORT TempPn52F);	/*	Panel Monitor Initialize	*/
void PnlOpLedDispMonDecWord(USHORT data, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl, USHORT LedNum);	/* 	LED�I�y���[�^ ���j�^�[�\��(10�i���[�h��������^�Ȃ�)*/
void PnlOpLedDispMonDecLong(ULONG LData, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl,  USHORT LedNum);	/*	LED�I�y���[�^ ���j�^�[�\��(10�i�����O ��������^�Ȃ�)	*/
void PnlOpLedSetStatusBits(LEDDSP *LedDspp, PNLOP_UNIT_HANDLE	hPnlOpUnitHndl, ULONG BlnkTime);	 	/*	LED�I�y���[�^ �X�e�[�^�X�r�b�g�\�����Z�b�g����	*/
void PnlOpLedSetValDec(USHORT data, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl );				 	/*	LED�I�y���[�^ �\���l(10�i)�Z�b�g����		*/
void PnlOpLedSetValHex(USHORT data, LEDDSP *LedDspp, USHORT StartDgt, USHORT digit);					/*	LED�I�y���[�^ �\���l(16�i)�Z�b�g����		*/
void PnlOpLedSetStr(UCHAR *StrPtr, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl, USHORT LedNum);	 	/*	LED Operator String Set		*/
void PnlOpLedSetBlnk(LEDDSP *LedDspp, UCHAR BlnkBtn,  USHORT LedNum);								 	/*	LED Operator Blink Set	*/

void PnlOpDisplayLedData(PNLOP_LED_DATA_HANDLE *hLedData, ALMMAN_HANDLE  *hAlmManager );	/*  7segLED�\���f�[�^�Z�b�g 		*/
void PnlOpInitialize(PNLOP_LED_DATA_HANDLE *hLedData );										/*  Panel Op �֘A������		*/

/* Locals */
static void PnlOpMain(PNLOP_UNIT_HANDLE hPnlOpUnitHndl );	/*		Panel Operator Main Program				*/
static void PnlOpLedBlink( PNLOP_HANDLE *hPnl );	/*		�p�l���I�y���[�^ LED�u�����N����					*/
static void PnlOpLedSetKnlBuffer( LEDDSP *LedDsp, UCHAR *Dist, LONG size );	/*	Panel LED Set to Kernel Variables	*/
static void PnlOpLedSetBlnkSw( LEDDSP *LedDsp, UCHAR data, LONG size, LONG flag );	/*		Panel LED Set to Same Data	*/
static void PnlOpLedDispPrmDecWordUnsigned(USHORT data, PNLOP_DISP_CTRL PnlOpDispCtrl, LEDDSP *DspData,  USHORT LedNum);/*	�p�l���I�y���[�^ LED�\��(�p�����[�^�����Ȃ�10�i���[�h)		*/
static void PnlOpLedDispPrmDecWordSigned(SHORT data, PNLOP_DISP_CTRL PnlOpDispCtrl, PNLOP_HANDLE *hPnl);/*	�p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)	*/
static void PnlOpLedDispPrmHexNblWord(USHORT data, PNLOP_DISP_CTRL PnlOpDispCtrl, LEDDSP	*DspData,  USHORT LedNum);/* �p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)		*/
static void PnlOpLedDispPrmDecLong(ULONG ldata, PNLOP_DISP_CTRL PnlOpDispCtrl, PNLOP_HANDLE *hPnl );	/*	�p�l���I�y���[�^ LED�\��(10�i�����O)	*/
static void PnlOpLedDispPrmHexLong(ULONG ldata, USHORT CurDigit, PNLOP_HANDLE *hPnl);	/*		�p�l���I�y���[�^ LED�\��(16�i�����O)	*/
static void PnlOpLedDispMonHexWord(USHORT data, PNLOP_HANDLE *hPnl);	/*	�p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)	*/
static void PnlOpLedDispMonBit(UCHAR data, PNLOP_HANDLE *hPnl);	/*		�p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)		*/
static void PnlOpStatusDispMode(PNLOP_UNIT_HANDLE	hPnlOpUnitHndl);	/*		�p�l���I�y���[�^ ��ԕ\������			*/
static void PnlOpFunNoMode(PNLOP_HANDLE  *hPnl);	/*		�p�l���I�y���[�^ Fn�ԍ��ҏW���[�h		*/
static void PnlOpFunExeMode(PNLOP_HANDLE  *hPnl );	/*		�p�l���I�y���[�^ Fn���s���[�h		*/
static void PnlOpPrmNoMode(PNLOP_HANDLE  *hPnl );	/*		�p�l���I�y���[�^ Pn�ԍ��ҏW���[�h		*/
static void PnlOpPrmDataMode(PNLOP_HANDLE  *hPnl );	/*		�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h		*/
static void PnlOpMonNoMode(PNLOP_HANDLE  *hPnl );	/*		�p�l���I�y���[�^ Un�ԍ��ҏW���[�h				*/
static void PnlOpMonDataMode(PNLOP_HANDLE  *hPnl );	/*		�p�l���I�y���[�^ Un�f�[�^�\�����[�h			*/
static UCHAR PnlOpSetLedMode(PNLOP_LED_DATA_HANDLE *hLedData );	/*		7segLED ��ԃZ�b�g			*/



USHORT	RpxFunSearchIdxOp(USHORT SrchNo, USHORT BaseIndex)
{
	return 0U;
}

USHORT	RpxPrmSearchIdxOp(USHORT SrchNo, USHORT BaseIndex)
{
	return 0U;
}

ULONG RpxPrmLowerLimitIdx(USHORT Index)
{
	return (ULONG)( 0 );
}

ULONG RpxPrmUpperLimitIdx(USHORT Index)
{
	return (ULONG)( 0 );
}

LONG	RpxPrmReadIdx(USHORT Index, ULONG *ResData, UCHAR CheckType)
{
	return 0;
}

LONG	RpxPrmUpdateEepromIdx(USHORT Index)
{
	return 0;
}

ULONG RpxPrmRamDigitInc(USHORT Index, ULONG Data, USHORT Digit)
{
	return (ULONG)( 0 );
}

ULONG RpxPrmRamDigitDec(USHORT Index, ULONG Data, USHORT Digit)
{
	return (ULONG)( 0 );
}

LONG	RpxPrmWriteRamIdx(USHORT Index, ULONG Data, UCHAR CheckType, HMSGIF Hmsg)
{
	return 0;
}

USHORT	RpxPrmGetDecimals( PRM_ATTR_T *pPrmDef )
{
	return 0U;
}

SHORT	RpxFunEndChkPnl(USHORT Index)
{
	return TRUE;
}


SHORT	RpxFunExeBeginPnl(USHORT Index)
{
	return TRUE;
}

LONG	RpiMonSearch(USHORT	SrchNo,  USHORT	BaseIndex,  USHORT *FindIndex,	UCHAR	CheckType	)
{
	return 1;
}

USHORT	RpxMonSearchIdxOp(	USHORT	SrchNo, USHORT	BaseIndex )
{
	return 1U;
}

UCHAR KpiSetPanelDispData( UCHAR *Data , UCHAR DataLen )
{
	return (UCHAR)TRUE;
}

#if	(CCSW_PANELID == PANELID_01A)	
/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^�Ǘ�																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : 100ms��1��̕p�x�Ńp�l���I�y���[�^�������ĂԁB											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	PnlOpManager( PNLOP_UNIT_HANDLE	hPnlOpUnitHndl )
{	/*hPnlOpUnitHndl: �p�l�I�y�֘A�������@*/
	/*	if( RinK.f.Pulse100ms == ON )		*//* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
		if( 1  == ON )
		{
			PnlOpMain( hPnlOpUnitHndl);
		}
		return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		Panel Operator Main Program																	*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpMain( PNLOP_UNIT_HANDLE hPnlOpUnitHndl )
{	/*hPnlOpUnitHndl: �p�l�I�y�֘A�������@*/

	USHORT	CurAlarm;
	LONG	RcTmp;
	PNLOP_HANDLE *hPnl;

	hPnl		= 	hPnlOpUnitHndl.PnlOpHndl;
	
/*	if(RmngV.SerComMode == NOCOM)		*//* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
	if(	TRUE	)
	{
		RpiSetAccsLvl( &Access, PopMsg.AccsLvl );	/* V027 */
		CurAlarm = (USHORT)ALMGetAlarmInfo(hPnlOpUnitHndl.AlmManager);
#if (CCSW_SFTID == SFTID_EXIST)																		/*<V717>Mod:Start*/
		if((CurAlarm != 0U) && ((CurAlarm & 0xF00U) != 0x900U) && (hPnl->LastAlarm == 0U) &&
		   (!((hPnl->DispMode == PNLMODE_FNEXE) && (CurAlarm == 0xEB9U))) )						/* �A���[���������͏�ԕ\�����[�h��	*/
#else
		if((CurAlarm != 0U) && ((CurAlarm & 0xF00U) != 0x900U) && (hPnl->LastAlarm == 0U))	/* �A���[���������͏�ԕ\�����[�h��	*/
#endif																								/*<V717>Mod:End  */
		{
			hPnl->CurPos = 0U;
			if(hPnl->DispMode == PNLMODE_FNEXE)	RpxFunFinishPnl(hPnl->CurFunIndx);
			if(hPnl->DispMode == PNLMODE_PNDATA)
			{
				RcTmp = RpxPrmUpdateEepromIdx(hPnl->CurPrmIndx);
				while((RcTmp == PRM_EEPROM_WRITE_WAIT) || (RcTmp == PRM_EEPROM_WRITE_FAIL))
					RcTmp = RpxPrmUpdateEepromIdx(hPnl->CurPrmIndx);
			}
			hPnl->DispMode = PNLMODE_STATUS;
		}
		hPnl->LastAlarm = CurAlarm;

		PnlKeyTranslate();
		switch(hPnl->DispMode)
		{
			case PNLMODE_STATUS:	/* ��ԕ\��	*/
				if(OpKeyMk.mode == (USHORT)TRUE)
				{
					hPnl->DispMode = PNLMODE_FNNUM;
					hPnl->CurPos = 0U;
				}
				else
				{
					PnlOpStatusDispMode(hPnlOpUnitHndl);
					if(OpKeyMk.rst == (USHORT)TRUE)
					{
						KpiResetAlarmTable();
						NdiClearAlarm();
					}
				}
				break;

			case PNLMODE_FNNUM:		/* Fn�ԍ�	*/
				if(OpKeyMk.mode == (USHORT)TRUE)
				{
					hPnl->DispMode = PNLMODE_PNNUM;
					hPnl->CurPos = 0U;
				}
				else if(OpKeyMk.data == (USHORT)TRUE)
				{
					if(RpxFunExeBeginPnl(hPnl->CurFunIndx) != TRUE)	/* Fn���s�ł��Ȃ���	*/
					{
						OpBlnkKickFlg.no_op = 1U;
					}
					else
					{
						hPnl->DispMode = PNLMODE_FNEXE;
						hPnl->CurPos = 0U;
					}
				}
				else 
				{
					PnlOpFunNoMode( hPnl );
				}
				break;

			case PNLMODE_FNEXE:		/* Fn���s	*/
				if(RpxFunEndChkPnl(hPnl->CurFunIndx) == 0)
				{
					RpxFunFinishPnl(hPnl->CurFunIndx);
					hPnl->CurPos = 0U;
					hPnl->DispMode = PNLMODE_FNNUM;
				}
				else	
				{
					PnlOpFunExeMode( hPnl );
				}
				break;

			case PNLMODE_PNNUM:		/* Pn�ԍ�	*/
				if(OpKeyMk.mode == (USHORT)TRUE)
				{
					hPnl->DispMode = PNLMODE_UNNUM;
					hPnl->CurPos = 0U;
				}
				else if(OpKeyMk.data == (USHORT)TRUE)
				{
					if(	Access.inhibit == INH_PASWRD )			/* �����݋֎~���ɂ�NO-OP	*/						/* <V042> */
					{
						OpBlnkKickFlg.no_op = 1U;
					}
					else
					{
						hPnl->DispMode = PNLMODE_PNDATA;
						hPnl->PrmWrState = 0U;
						hPnl->CurPos = 0U;
					}
				}
				else	
				{
					PnlOpPrmNoMode( hPnl );
				}
				break;

			case PNLMODE_PNDATA:	/* Pn�f�[�^	*/
				PnlOpPrmDataMode( hPnl );
				break;

			case PNLMODE_UNNUM:		/* Un�ԍ�	*/
				if(OpKeyMk.mode == (USHORT)TRUE)
				{
					hPnl->DispMode = PNLMODE_STATUS;
				}
				else if(OpKeyMk.data == (USHORT)TRUE)
				{
					hPnl->DispMode = PNLMODE_UNDATA;
					hPnl->CurPos = 0U;
				}
				else
				{
					PnlOpMonNoMode( hPnl );
				}
				break;

			case PNLMODE_UNDATA:	/* Un�f�[�^	*/
				PnlOpMonDataMode( hPnl );
				break;

			default:
				break;
		}
		PnlOpLedBlink( hPnl );
	}
	else
	{
		PnlOpStatusDispMode( hPnlOpUnitHndl );
		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_NOBLINK, sizeof(hPnl->PnlLed), ((KlibGetLongTimerMs(hPnl->BlnkTime)/100U) & 0x02U) );
	}
	PnlOpLedSetKnlBuffer( &hPnl->DspData, hPnl->PnlLed, sizeof(hPnl->PnlLed) );

	return;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ �L�[�ϊ�����																*/
/*																									*/
/****************************************************************************************************/
void	PnlKeyTranslate(void)
{

	*((UINT *)&OpKeyMk) = *((UINT *)&OpKeyBrk) = *((UINT *)&OpKeyLvl) = 0;

	if(*((UINT *)&OpBlnkFlg) != 0)	/* �u�����N���̓L�[���͂𖳎�����	*/
	{
		Pnl.BtnLast = RinK.PnlBtn;
		return;
	}

	/* �L�[���̓��x���̏���	*/
	if(RinK.PnlBtn & PNLKEY_MODE)	OpKeyLvl.mode = TRUE;
	if(RinK.PnlBtn & PNLKEY_UP)		OpKeyLvl.up = TRUE;
	if(RinK.PnlBtn & PNLKEY_DOWN)	OpKeyLvl.down = TRUE;
	if(RinK.PnlBtn & PNLKEY_SHUP)	OpKeyLvl.shup = TRUE;
	if((RinK.PnlBtn & PNLKEY_RST) == PNLKEY_RST)	OpKeyLvl.rst = TRUE;


	if(RinK.PnlBtn != Pnl.BtnLast)	/* �L�[���͂ɕω������������̂݃Z�b�g(=�`���^�����O�h�~)		*/
	{
		switch(RinK.PnlBtn)		/* �L�[����MAKE�̏���	*/
		{
			case PNLKEY_MODE:
				OpKeyMk.mode = TRUE;
				break;

			case PNLKEY_UP:
				OpKeyMk.up = TRUE;
				KpiRstShortTimer(&Pnl.KeyTime);
				break;

			case PNLKEY_DOWN:
				OpKeyMk.down = TRUE;
				KpiRstShortTimer(&Pnl.KeyTime);
				break;

			case PNLKEY_SHUP:
				OpKeyMk.shup = TRUE;
				KpiRstShortTimer(&Pnl.KeyTime);
				break;

			case PNLKEY_RST:
				OpKeyMk.rst = TRUE;
				break;

			default:
				break;
		}

		switch(Pnl.BtnLast)		/* �L�[����BREAK�̏���	*/
		{
			case PNLKEY_MODE:
				OpKeyBrk.mode = TRUE;
				break;

			case PNLKEY_UP:
				OpKeyBrk.up = TRUE;
				break;

			case PNLKEY_DOWN:
				OpKeyBrk.down = TRUE;
				break;

			case PNLKEY_SHUP:
				OpKeyBrk.shup = TRUE;
				break;

			case PNLKEY_RST:
				OpKeyBrk.rst = TRUE;
				break;

			default:
				break;
		}
	}
	else						/* �L�[���͂ɕω����Ȃ��ꍇ: ���s�[�g, �z�[���h����	*/
	{
		switch(RinK.PnlBtn)
		{
			case PNLKEY_UP:
				if(KpiGetShortTimer(&Pnl.KeyTime) >= 400)
				{	/* 400ms�����͉������Ȃ�	*/
					if(!((KpiGetShortTimer(&Pnl.KeyTime)/100) & 0x01))
					{	/* �o�ߎ��� 4xx,6xx,8xx...�̎�	*/
						OpKeyMk.up = TRUE;
					}
				}
				break;

			case PNLKEY_DOWN:
				if(KpiGetShortTimer(&Pnl.KeyTime) >= 400)
				{	/* 400ms�����͉������Ȃ�	*/
					if(!((KpiGetShortTimer(&Pnl.KeyTime)/100) & 0x01))
					{	/* �o�ߎ��� 4xx,6xx,8xx...�̎�	*/
						OpKeyMk.down = TRUE;
					}
				}
				break;

			case PNLKEY_SHUP:
				if(KpiGetShortTimer(&Pnl.KeyTime) >= 1000)
				{	/* 1s �ȏ��[DATA]�L�[���͑���	*/
					OpKeyMk.data = TRUE;
					KpiRstShortTimer(&Pnl.KeyTime);
				}
				break;

			default:
				break;
		}
	}

	Pnl.BtnLast = RinK.PnlBtn;

	return;
}
#endif 


/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ ��ԕ\������																*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpStatusDispMode( PNLOP_UNIT_HANDLE 	hPnlOpUnitHndl )
{	/*hPnlOpUnitHndl: �p�l�I�y�֘A�������@*/
	USHORT	CurAlarm;
	PNLOP_HANDLE *hPnl;
	PNLOP_UNIT_HANDLE	PNLOP_UNIT_HANDLETp;

	PNLOP_UNIT_HANDLETp		 =  hPnlOpUnitHndl;

	hPnl		= 	hPnlOpUnitHndl.PnlOpHndl;

	/* ��ԕ\�� */
	hPnl->DspData.LedCode[4] = LEDCODE_B7;
	hPnl->DspData.LedCode[3] = LEDCODE_BLANK;
	PnlOpLedSetStatusBits( &(hPnl->DspData) , PNLOP_UNIT_HANDLETp,hPnl->BlnkTime);

	CurAlarm = (USHORT)ALMGetAlarmInfo(hPnlOpUnitHndl.AlmManager);
	if( PNLOP_UNIT_HANDLETp.AlmManager->AlmFlag )		/* �A���[�������� */
	{
		/* �A���[���R�[�h�\��	*/
		PnlOpLedSetValHex(CurAlarm, &(hPnl->DspData), 2U, 3U);

		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_BLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );
	}
	else
	{
		if( CurAlarm != 0U )		/* ���[�j���O������ */
		{
			/* ���[�j���O�R�[�h�\��	*/
			PnlOpLedSetValHex(CurAlarm, &(hPnl->DspData), 2U, 3U);
		}
		else
		{
			if(PNLOP_UNIT_HANDLETp.SeqCtrlOut->BeComplete )
			{
				if( !PNLOP_UNIT_HANDLETp.MencV->PhaseReady )
				{
					hPnl->DspData.LedCode[3] = LEDCODE_P;	/* " P-dt"	*/
					hPnl->DspData.LedCode[2] = LEDCODE_B6;
					hPnl->DspData.LedCode[1] = LEDCODE_D;
					hPnl->DspData.LedCode[0] = LEDCODE_T;
				}
				else
				{
					hPnl->DspData.LedCode[2] = LEDCODE_R;	/* "  run"	*/
					hPnl->DspData.LedCode[1] = LEDCODE_sU;
					hPnl->DspData.LedCode[0] = LEDCODE_N;
				}
			}
			else
			{
				hPnl->DspData.LedCode[2] = LEDCODE_BLANK;	/* "   bb"	*/
				hPnl->DspData.LedCode[1] = LEDCODE_B;
				hPnl->DspData.LedCode[0] = LEDCODE_B;
			}

			if((PNLOP_UNIT_HANDLETp.BaseCtrlOut->CtrlModeOut.b.cm != (UCHAR)CTRLMODE_JOG) && (PNLOP_UNIT_HANDLETp.BaseCtrlOut->CtrlModeOut.b.cm != (UCHAR)CTRLMODE_ZSRCH))
			{
/*				if(CinV.f.Sv.Pot && CinV.f.Sv.Not)		*/	/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
				if( 1 )	/* "Pot" AND "not" */
				{
					if(!((KlibGetLongTimerMs(hPnl->BlnkTime)/100U) & 0x04U))	/* ����800ms	*/
					{
						hPnl->DspData.LedCode[2] = LEDCODE_P;
						hPnl->DspData.LedCode[1] = LEDCODE_sO;
						hPnl->DspData.LedCode[0] = LEDCODE_T;
					}
					else
					{
						hPnl->DspData.LedCode[2] = LEDCODE_N;
						hPnl->DspData.LedCode[1] = LEDCODE_sO;
						hPnl->DspData.LedCode[0] = LEDCODE_T;
					}
				}
				else if(PNLOP_UNIT_HANDLETp.SeqCtrlData->PotSignal.SignalStatus.v/* <S045> */)				/* "Pot" */
				{
					hPnl->DspData.LedCode[2] = LEDCODE_P;
					hPnl->DspData.LedCode[1] = LEDCODE_sO;
					hPnl->DspData.LedCode[0] = LEDCODE_T;
				}
				else if(PNLOP_UNIT_HANDLETp.SeqCtrlData->NotSignal.SignalStatus.v/* <S045> */)				/* "Not" */
				{
					hPnl->DspData.LedCode[2] = LEDCODE_N;
					hPnl->DspData.LedCode[1] = LEDCODE_sO;
					hPnl->DspData.LedCode[0] = LEDCODE_T;
				}
				else
				{
					/* Do nothing */
				}
			}

/*<V739>SafetyModule���މ��P*/
#if (CCSW_SFTID == SFTID_EXIST)														/*<V729>Add:Start*/
/*			if(CoutSom.UnSomState & SOM_SYS_ACT)		*//* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
			if((0x10U & SOM_SYS_ACT) != (USHORT)FALSE)
			{
				hPnl->DspData.LedCode[2] = LEDCODE_A;
				hPnl->DspData.LedCode[1] = LEDCODE_C;
				hPnl->DspData.LedCode[0] = LEDCODE_T;
			}
#endif																				/*<V729>Add:End  */

			if(PNLOP_UNIT_HANDLETp.DetHwbbSts->HwbbState)				/* "Hbb" <V066> */
			{
				hPnl->DspData.LedCode[3] = LEDCODE_BLANK; /* <V105> ���F���Ɍ��o����OT��ALM�ƂȂ邽�ߕs�v*/
				hPnl->DspData.LedCode[2] = LEDCODE_H;
				hPnl->DspData.LedCode[1] = LEDCODE_B;
				hPnl->DspData.LedCode[0] = LEDCODE_B;
			}

			/*--------------------------------------------------------------------------*/
			/*	���[�^���X�e�X�g���[�h�\��				<V245>									*/
			/*--------------------------------------------------------------------------*/
			
/*			if(Kprm.f.MotorLessTestMode)		*//* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
			if(	1	)
			{
				/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */

/*				if(!((KpiGetShortTimer(&Pnl.BlnkTime)/100) & 0x04))		*/		/* ����800ms	*/
				if(!((KlibGetLongTimerMs(hPnl->BlnkTime)/100U) & 0x08U))		/* ����1600ms	*//*<V292>*/
				{
					hPnl->DspData.LedCode[3] = LEDCODE_BLANK;
					hPnl->DspData.LedCode[2] = LEDCODE_T;
					hPnl->DspData.LedCode[1] = LEDCODE_5;
					hPnl->DspData.LedCode[0] = LEDCODE_T;
				}
			}

			PnlOpLedSetBlnkSw( &hPnl->DspData, (UCHAR)PNLLED_NOBLINK, sizeof(hPnl->PnlLed), TRUE );
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Fn�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpFunNoMode(PNLOP_HANDLE  *hPnl)
{	/*	hPnl : Panel Op Handler Pointer	*/
	USHORT	i, FnNumTmp;

	if(OpKeyMk.shup == (USHORT)TRUE)
	{
		hPnl->CurPos = ((hPnl->CurPos+1U) == 3U)? 0: (hPnl->CurPos+1U);
	}
	else if(OpKeyMk.up == (USHORT)TRUE)
	{
		FnNumTmp = FnFunTbl[hPnl->CurFunIndx].FnNo + (0x01U << (hPnl->CurPos*4U));
		if(FnNumTmp >= 0x1000U)
		{
			FnNumTmp = 0xFFFU;
		}

		hPnl->CurFunIndx = RpxFunSearchIdxOp(FnNumTmp, hPnl->CurFunIndx);
	}
	else if(OpKeyMk.down == (USHORT)TRUE)
	{
		if(FnFunTbl[hPnl->CurFunIndx].FnNo < (0x01U << (hPnl->CurPos*4U)))
		{
			FnNumTmp = 0x000U;
		}
		else
		{
			FnNumTmp = FnFunTbl[hPnl->CurFunIndx].FnNo - (0x01U << (hPnl->CurPos*4U));
		}

		hPnl->CurFunIndx = RpxFunSearchIdxOp(FnNumTmp, hPnl->CurFunIndx);
	}
	else
	{
		/* Do nothing */
	}
	/* �\������	*/
	hPnl->DspData.LedCode[4] = LEDCODE_F;
	hPnl->DspData.LedCode[3] = LEDCODE_N;
	PnlOpLedSetValHex(FnFunTbl[hPnl->CurFunIndx].FnNo, &(hPnl->DspData), 2U, 3U);

	for(i = 0U; i < sizeof(hPnl->PnlLed); i++)
	{
		hPnl->DspData.LedBlnkSw[i] = (i == hPnl->CurPos)?	(UCHAR)0xFF: (UCHAR)0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Fn���s���[�h																*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpFunExeMode(PNLOP_HANDLE  *hPnl )
{	/*	hPnl : Panel Op Handler Pointer	*/

	RpxFunExePnl( OpKeyMk, OpKeyBrk, OpKeyLvl, hPnl->CurFunIndx, 
				  &(hPnl->DspData), &OpBlnkKickFlg );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Pn�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpPrmNoMode(PNLOP_HANDLE  *hPnl )
{	/*	hPnl : Panel Op Handler Pointer	*/
	USHORT	i, PnNumTmp;
	PRM_ATTR_T	*PrmDefTmp;


	if(OpKeyMk.shup == (USHORT)TRUE)
	{
		hPnl->CurPos = ((hPnl->CurPos+1U) == 3U)? 0U: (hPnl->CurPos+1U);
	}
	else if(OpKeyMk.up == (USHORT)TRUE)
	{
		PnNumTmp = (USHORT)PnPrmTbl[hPnl->CurPrmIndx].Number + (0x01U << (hPnl->CurPos*4U));
		if(PnNumTmp >= 0x1000U)	{PnNumTmp = 0xFFFU;}

		hPnl->CurPrmIndx = RpxPrmSearchIdxOp(PnNumTmp, hPnl->CurPrmIndx);
	}
	else if(OpKeyMk.down == (USHORT)TRUE)
	{
		if(PnPrmTbl[hPnl->CurPrmIndx].Number < (ULONG)(0x01U << (hPnl->CurPos*4U)))
		{
			PnNumTmp = 0x000U;
		}
		else
		{
			PnNumTmp = (USHORT)PnPrmTbl[hPnl->CurPrmIndx].Number - (0x01U << (hPnl->CurPos*4U));
		}

		hPnl->CurPrmIndx = RpxPrmSearchIdxOp(PnNumTmp, hPnl->CurPrmIndx);
	}
	else
	{
		/* Do nothing */
	}

	PrmDefTmp = (PRM_ATTR_T *)PnPrmTbl[hPnl->CurPrmIndx].Attribute;	/* �p�����[�^��`	*/
	if(PrmDefTmp->DataLength == 4U)	/* �����O�p�����[�^	*/
	{
		hPnl->CurPosMax = (PrmDefTmp->FormatType == DSPDEF_BASEDEC)? 10U: 8U;
	}
	else						/* ���[�h�p�����[�^	*/
	{
		if(PrmDefTmp->FormatType == DSPDEF_BASEDEC)
		{
			if( (PrmDefTmp->Sign == (USHORT)DSPDEF_W_SIGN) &&
				( (-10000 < (SHORT)RpxPrmLowerLimitIdx(hPnl->CurPrmIndx)) && 
				  ((SHORT)RpxPrmUpperLimitIdx(hPnl->CurPrmIndx) < 10000) )
			)
			{
				hPnl->CurPosMax = 4U;	/* �����܂�5���ő����ꍇ	*/
			}
			else
			{
				hPnl->CurPosMax = 5U;
			}
		}
		else
		{
			hPnl->CurPosMax = 4U;
		}
	}

	/* �\������	*/
	hPnl->DspData.LedCode[4] = LEDCODE_P;
	hPnl->DspData.LedCode[3] = LEDCODE_N;
	PnlOpLedSetValHex((USHORT)PnPrmTbl[hPnl->CurPrmIndx].Number, &(hPnl->DspData), 2U, 3U);

	for(i = 0U; i < sizeof(hPnl->PnlLed); i++)
	{
		hPnl->DspData.LedBlnkSw[i] = (i == hPnl->CurPos)?	0xFFU: 0U;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Pn�f�[�^�\��/�ҏW���[�h													*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpPrmDataMode(PNLOP_HANDLE  *hPnl )
{	/*	hPnl : Panel Op Handler Pointer	*/
	ULONG	LDataTmp;
	PRM_ATTR_T	*PrmDefTmp;
	LONG	rc;
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));
	
	PrmDefTmp = (PRM_ATTR_T *)PnPrmTbl[hPnl->CurPrmIndx].Attribute;	/* �p�����[�^��`	*/

	switch(hPnl->PrmWrState)
	{
		case OPWM_None:	/* �p�����[�^�l�\���E�ҏW����	*/

			RpxPrmReadIdx(hPnl->CurPrmIndx, &LDataTmp, PCHK_READ);	/* ldata_tmp <-- �p�����[�^�l	*/

			if( (OpKeyMk.data | OpKeyMk.mode) != 0U )	/* EEPROM�������ݏ���	*/
			{
				switch(RpxPrmUpdateEepromIdx(hPnl->CurPrmIndx))
				{
					case OK:	/* ����	*/
						OpBlnkKickFlg.PrmEewrite = 1U;	/* �u�����N�N��	*/
						break;

					case PRM_EEPROM_WRITE_FAIL:	/* �܂�Queue���󂢂Ă��Ȃ�	*/
						hPnl->PrmWrState = OPWM_UpdtWait;
						break;

					case PERR_NO:					/* RAM�݂̂̃p�����[�^								*/
					case PRM_EEPROM_RAM_NO_DIFF:	/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
						hPnl->CurPos = 0U;
						hPnl->DispMode = PNLMODE_PNNUM;	/* �p�����[�^�ԍ����[�h��	*/
/*						return;			*/
						break;	

					default:
						break;
				}
			}
			else if(OpKeyMk.shup == TRUE)
			{
				hPnl->CurPos = ((hPnl->CurPos+1U) == hPnl->CurPosMax)?
					0U: (hPnl->CurPos+1U);
			}
			else if((OpKeyMk.up | OpKeyMk.down) != (USHORT)FALSE )
			{
				LDataTmp = (OpKeyMk.up == 1U)? 
					RpxPrmRamDigitInc(hPnl->CurPrmIndx, LDataTmp, hPnl->CurPos): 
					RpxPrmRamDigitDec(hPnl->CurPrmIndx, LDataTmp, hPnl->CurPos);

				rc = RpxPrmWriteRamIdx(hPnl->CurPrmIndx, LDataTmp, PCHK_WRITE, &PopMsg);
				if( rc == PERR_CALC )
				{
					OpBlnkKickFlg.error = 1U;
				}
			}
			else
			{
				/* Do nothing */
			}
			/* �\������	*/
			if(PrmDefTmp->DataLength == 4U)	/* �����O�p�����[�^	*/
			{
				if(PrmDefTmp->FormatType == DSPDEF_BASEDEC)	/* �ꂪ10�i	*/
				{
					PnlOpDispCtrlTemp.CurDigit = hPnl->CurPos;
					PnlOpDispCtrlTemp.Decimals = RpxPrmGetDecimals( PrmDefTmp );
					PnlOpDispCtrlTemp.Sign = (USHORT)PrmDefTmp->Sign;
					PnlOpLedDispPrmDecLong(LDataTmp, PnlOpDispCtrlTemp, hPnl);
				}
				else								/* �ꂪ10�i�ȊO	*/
				{
					PnlOpLedDispPrmHexLong(LDataTmp, hPnl->CurPos, hPnl);
				}
				
			}
			else						/* ���[�h�p�����[�^	*/
			{
				if(PrmDefTmp->FormatType == DSPDEF_BASEDEC)	/* �ꂪ10�i	*/
				{
					if(PrmDefTmp->Sign == (USHORT)DSPDEF_W_SIGN)
					{
						PnlOpDispCtrlTemp.CurDigit = hPnl->CurPos;
						PnlOpDispCtrlTemp.Decimals = RpxPrmGetDecimals( PrmDefTmp );
						PnlOpDispCtrlTemp.LLimit = (SHORT)RpxPrmLowerLimitIdx(hPnl->CurPrmIndx);
						PnlOpDispCtrlTemp.ULimit = (SHORT)RpxPrmUpperLimitIdx(hPnl->CurPrmIndx);
						PnlOpLedDispPrmDecWordSigned((SHORT)LDataTmp,	PnlOpDispCtrlTemp ,hPnl );
					}
					else
					{
						PnlOpDispCtrlTemp.CurDigit = hPnl->CurPos;
						PnlOpDispCtrlTemp.Decimals = RpxPrmGetDecimals( PrmDefTmp );
						PnlOpLedDispPrmDecWordUnsigned((USHORT)LDataTmp,  PnlOpDispCtrlTemp ,&hPnl->DspData, sizeof(hPnl->PnlLed));
					}
				}
				else								/* �ꂪ10�i�ȊO	*/
				{
					PnlOpDispCtrlTemp.CurDigit = hPnl->CurPos;
					PnlOpDispCtrlTemp.Base = (USHORT)PrmDefTmp->FormatType;
					PnlOpLedDispPrmHexNblWord((USHORT)LDataTmp, PnlOpDispCtrlTemp , &hPnl->DspData, sizeof(hPnl->PnlLed));
				}
			}

			break;

		case OPWM_UpdtWait:	/* EEPROM��������(�v���o�^)����	*/
			switch(RpxPrmUpdateEepromIdx(hPnl->CurPrmIndx))
			{
				case OK:	/* ����	*/
					OpBlnkKickFlg.PrmEewrite = 1U;	/* �u�����N�N��	*/
					hPnl->PrmWrState = OPWM_None;
					break;

				case PRM_EEPROM_RAM_NO_DIFF:	/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
					hPnl->PrmWrState = OPWM_None;
					hPnl->CurPos = 0U;
					hPnl->DispMode = PNLMODE_PNNUM;	/* �p�����[�^�ԍ����[�h��	*/
/*					return;	*/
					break;	

				default:
					break;
			}
			break;

		default:
			break;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Un�ԍ��ҏW���[�h															*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpMonNoMode(PNLOP_HANDLE  *hPnl )
{	/*	hPnl : Panel Op Handler Pointer	*/
	USHORT	i, UnNumTmp;
	MONDEF	*MondefTmp;


	if(OpKeyMk.shup == (USHORT)TRUE)
	{
		hPnl->CurPos = ((hPnl->CurPos+1U) == 3U)? 0U: (hPnl->CurPos+1U);
	}
	else if(OpKeyMk.up == (USHORT)TRUE)
	{
		UnNumTmp = (USHORT)UnMonTbl[hPnl->CurMonIndx].UnNo + (0x01U << (hPnl->CurPos*4U));

		hPnl->CurMonIndx = RpxMonSearchIdxOp(UnNumTmp, hPnl->CurMonIndx);
	}
	else if(OpKeyMk.down == (USHORT)TRUE)
	{
		if(UnMonTbl[hPnl->CurMonIndx].UnNo < (ULONG)(0x01U << (hPnl->CurPos*4U)))
		{
			UnNumTmp = 0x000U;
		}
		else
		{
			UnNumTmp = (USHORT)UnMonTbl[hPnl->CurMonIndx].UnNo - (0x01 << (hPnl->CurPos*4U));
		}

		hPnl->CurMonIndx = RpxMonSearchIdxOp(UnNumTmp, hPnl->CurMonIndx);
	}
	else
	{
		/* Do nothing */
	}
	MondefTmp = (MONDEF *)UnMonTbl[hPnl->CurMonIndx].MonDefPtr;	/* ���j�^��`	*/

	if(MondefTmp->IsLong == (USHORT)TRUE)				/* �T�C�Y�������O	*/
	{
		hPnl->CurPosMax = 3U;	/* �\���ؑւ���(���E���E��)	*/
	}
	else										/* �T�C�Y�����[�h�ȉ�	*/
	{
		hPnl->CurPosMax = 1U;	/* �\���ؑւȂ�	*/
	}

	/* �\������	*/
	hPnl->DspData.LedCode[4] = LEDCODE_U;
	hPnl->DspData.LedCode[3] = LEDCODE_N;
	PnlOpLedSetValHex((USHORT)UnMonTbl[hPnl->CurMonIndx].UnNo, &(hPnl->DspData), 2U, 3U);
	for(i = 0U; i < sizeof(hPnl->PnlLed); i++)
	{
		hPnl->DspData.LedBlnkSw[i] = (i == hPnl->CurPos)?	0xFFU: 0U;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ Un�f�[�^�\�����[�h															*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpMonDataMode(PNLOP_HANDLE  *hPnl )
{	/*	hPnl : Panel Op Handler Pointer	*/
	ULONG	LDataTmp;
	MONDEF	*MonDefTmp;
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));
	
	MonDefTmp = (MONDEF *)UnMonTbl[hPnl->CurMonIndx].MonDefPtr;	/* ���j�^��`	*/

	RpiMonRead(MonDefTmp, &LDataTmp, MCHK_ACCS_LVL);		/* ldata_tmp <-- ���j�^�l	*/

	if(OpKeyMk.data == (USHORT)TRUE)
	{
		hPnl->CurPos = 0U;
		hPnl->DispMode = PNLMODE_UNNUM;	/* �p�����[�^�ԍ����[�h��	*/
	/*	return;	*/
	}
	else if(OpKeyMk.shup == (USHORT)TRUE)
	{
		hPnl->CurPos = ((hPnl->CurPos+1U) == hPnl->CurPosMax)?
			0U: (hPnl->CurPos+1U);
	}
	else if(OpKeyMk.rst == (USHORT)TRUE)
	{
		RpxMonClearVarIdx( hPnl->CurMonIndx );
	}
	else
	{
		/* Do nothing */
	}
	/* �\������	*/
	switch(MonDefTmp->Base)
	{
		case DSPDEF_BASEDEC:
			if(MonDefTmp->IsLong == (USHORT)FALSE )		/* ���[�h	*/
			{

				PnlOpDispCtrlTemp.Decimals = 1U;
				PnlOpDispCtrlTemp.Sign = (USHORT)MonDefTmp->Sign;
				PnlOpLedDispMonDecWord((USHORT)LDataTmp, &(hPnl->DspData), PnlOpDispCtrlTemp, sizeof(hPnl->PnlLed));
			}
			else								/* �����O	*/
			{

				PnlOpDispCtrlTemp.DspPos = hPnl->CurPos;
				PnlOpDispCtrlTemp.Decimals = 1U;
				PnlOpDispCtrlTemp.Sign = (USHORT)MonDefTmp->Sign;
				PnlOpLedDispMonDecLong(LDataTmp, &(hPnl->DspData), PnlOpDispCtrlTemp, sizeof(hPnl->PnlLed));
			}
			break;

		case DSPDEF_BASEHEX:
			PnlOpLedDispMonHexWord((USHORT)LDataTmp, hPnl);	/* ���݂̓��[�h�����Ȃ�	*/
			break;

		case DSPDEF_BASEBIT:
			PnlOpLedDispMonBit((UCHAR)LDataTmp, hPnl );			/* ����8�r�b�g��\������	*/
			break;

		default:
			break;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	none�\���L���̃��j�^ && �\���f�[�^��-1 ==>	nonE�\��							<V873>		*/
	/*----------------------------------------------------------------------------------------------*/

/*	if( (MonDefTmp->NoneDisp==NONEDSP_ENABLE) && ((LONG)LDataTmp==-1) )*//* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
	if( ( 1 ) && ((LONG)LDataTmp == -1) )
	{
			/* " nonE"	*/
			hPnl->DspData.LedCode[4] = LEDCODE_BLANK;
			hPnl->DspData.LedCode[3] = LEDCODE_N;
			hPnl->DspData.LedCode[2] = LEDCODE_sO;
			hPnl->DspData.LedCode[1] = LEDCODE_N;
			hPnl->DspData.LedCode[0] = LEDCODE_E;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�u�����N����															*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedBlink( PNLOP_HANDLE *hPnl )
{	/*	hPnl : Panel Op Handler Pointer	*/

	if(*((UINT *)&OpBlnkFlg) != 0)	/* �u�����N�I���`�F�b�N	*/
	{
		if(KlibGetLongTimerMs(OpBlnkTime) > 1000U)
		{	/* �u�����N���Ԃ��I������	*/
			*((UINT *)&OpBlnkFlg) = 0U;
		}
	}

	/* �u�����N�J�n������	*/
	if(OpBlnkKickFlg.PrmEewrite == (USHORT)TRUE)	/* �p�����[�^�����ݎ�	*/
	{
		*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0U;
		OpBlnkFlg.PrmEewrite = 1U;
		KlibRstLongTimer(&OpBlnkTime);
	}
	/* read/write�L�[�Ȃ�	*/
	else if(OpBlnkKickFlg.no_op == (USHORT)TRUE)
	{
		*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0U;
		OpBlnkFlg.no_op = 1U;
		KlibRstLongTimer(&OpBlnkTime);
	}
	else if(OpBlnkKickFlg.error == (USHORT)TRUE)
	{
		*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0U;
		OpBlnkFlg.error = 1U;
		KlibRstLongTimer(&OpBlnkTime);
	}
	else if(OpBlnkKickFlg.done == (USHORT)TRUE)
	{
		*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0U;
		OpBlnkFlg.done = 1U;
		KlibRstLongTimer(&OpBlnkTime);
	}
	else if(OpBlnkKickFlg.end == (USHORT)TRUE)
	{
		*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0U;
		OpBlnkFlg.end = 1U;
		KlibRstLongTimer(&OpBlnkTime);
	}
	else
	{
		/* Do nothing */
	}
	/* �u�����N������	*/
	if(OpBlnkFlg.PrmEewrite == (USHORT)TRUE)	/* �p�����[�^�����ݎ��u�����N	*/
	{
		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_BLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );
	}
	/* �p�l���ɂ�read/write�L�[�͂Ȃ�	*/
	else if(OpBlnkFlg.no_op == (USHORT)TRUE)
	{
		/* "no_oP"	*/
		hPnl->DspData.LedCode[4] = LEDCODE_N;
		hPnl->DspData.LedCode[3] = LEDCODE_sO;
		hPnl->DspData.LedCode[2] = LEDCODE_B3;
		hPnl->DspData.LedCode[1] = LEDCODE_sO;
		hPnl->DspData.LedCode[0] = LEDCODE_P;

		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_BLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );
	}
	else if(OpBlnkFlg.error == (USHORT)TRUE)
	{
		/* "Error"	*/
		hPnl->DspData.LedCode[4] = LEDCODE_E;
		hPnl->DspData.LedCode[3] = LEDCODE_R;
		hPnl->DspData.LedCode[2] = LEDCODE_R;
		hPnl->DspData.LedCode[1] = LEDCODE_sO;
		hPnl->DspData.LedCode[0] = LEDCODE_R;

		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_BLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );
	}
	else if(OpBlnkFlg.done == (USHORT)TRUE)
	{
		/* " donE"	*/
		hPnl->DspData.LedCode[4] = LEDCODE_BLANK;
		hPnl->DspData.LedCode[3] = LEDCODE_D;
		hPnl->DspData.LedCode[2] = LEDCODE_sO;
		hPnl->DspData.LedCode[1] = LEDCODE_N;
		hPnl->DspData.LedCode[0] = LEDCODE_E;

		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_BLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );
	}
	else if(OpBlnkFlg.end == (USHORT)TRUE)
	{
		/* "  End"	*/
		hPnl->DspData.LedCode[4] = LEDCODE_BLANK;
		hPnl->DspData.LedCode[3] = LEDCODE_BLANK;
		hPnl->DspData.LedCode[2] = LEDCODE_E;
		hPnl->DspData.LedCode[1] = LEDCODE_N;
		hPnl->DspData.LedCode[0] = LEDCODE_D;

		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_BLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );
	}
	else
	{
		/* Do nothing */
	}
	PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_NOBLINK, sizeof(hPnl->PnlLed), ((KlibGetLongTimerMs(hPnl->BlnkTime)/100U) & 0x02U) );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Initialize																			*/
/*																									*/
/****************************************************************************************************/
void	PnlOpInit(PNLOP_HANDLE *hPnl)
{	/*	hPnl : Panel Op Handler Pointer	*/

	/* �\���̕ϐ�Pnl�̃N���A	*/
	MlibResetByteMemory((void *)hPnl, sizeof(PNLOP_HANDLE));


	/* �u�����N�ϐ��̃N���A	*/
	*((UINT *)&OpBlnkFlg) = 0U;
	*((UINT *)&OpBlnkKickFlg) = 0U;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Monitor Initialize																	*/
/*																									*/
/****************************************************************************************************/
void	PnlOpMonInit( PNLOP_HANDLE *hPnl ,ALMMAN_HANDLE	*hAlmManager, USHORT TempPn52F)
{	/*	hPnl : Panel Op Handler Pointer	,hAlmManager �F�A���[������p�\���� �CTempPn52F : �d���������̃��j�^�\��  */

/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
/*	if ( (Prm.Pn52F != PNLMON_NOSET) && (KpiGetAlarmInfo( ) == 0) )		*/
	if ( (TempPn52F != PNLMON_NOSET) && (ALMGetAlarmInfo(hAlmManager) == 0) )
	{
/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */
/*		if ( RpiMonSearch( Prm.Pn52F, 0, &(Pnl.CurMonIndx), MCHK_ACCS_LVL ) == OK )		*/

		if ( RpiMonSearch( TempPn52F , 0U, &(hPnl->CurMonIndx), MCHK_ACCS_LVL ) == OK )
		{
			hPnl->DispMode = PNLMODE_UNDATA;
			hPnl->CurPos = 0U;

			if ( UnMonTbl[ hPnl->CurMonIndx ].MonDefPtr->IsLong == 4U )	/* �T�C�Y�������O	*/
			{
				hPnl->CurPosMax = 3U;	/* �\���ؑւ���(���E���E��)	*/
			}
			else										/* �T�C�Y�����[�h�ȉ�	*/
			{
				hPnl->CurPosMax = 1U;	/* �\���ؑւȂ�	*/
			}
		}
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Finish																				*/
/*																									*/
/****************************************************************************************************/
void	PnlOpFinish(PNLOP_HANDLE *hPnl)
{	/*	hPnl : Panel Op Handler Pointer	*/
	LONG	RcTmp;

	/* ���s���̕⏕�@�\���I�������鏈���B���s�����ۂ��̔���͊֐����ōs���Ă���B	*/
	RpxFunFinishPnl(hPnl->CurFunIndx);
	/* �p�����[�^��EEPROM�ɏ������ޕK�v������ꍇ�̏���	*/
	RcTmp = RpxPrmUpdateEepromIdx(hPnl->CurPrmIndx);
	while((RcTmp == PRM_EEPROM_WRITE_WAIT) || (RcTmp == PRM_EEPROM_WRITE_FAIL))	/* �܂��������Ă��Ȃ�	*/
		RcTmp = RpxPrmUpdateEepromIdx(hPnl->CurPrmIndx);
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(�p�����[�^�����Ȃ�10�i���[�h)										*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedDispPrmDecWordUnsigned(USHORT data, PNLOP_DISP_CTRL PnlOpDispCtrl, LEDDSP *DspData,  USHORT LedNum)
{	/* data:�\������l,PnlOpDispCtrl : LED Display Control Struct,DspData :LED�\���f�[�^�̃A�h���X,	*/
	/* LedNum : LED Number */
	USHORT	i;
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));
	
	if(PnlOpDispCtrl.CurDigit < 5U)
	{
		PnlOpDispCtrlTemp.StartDgt = 3U;
		PnlOpDispCtrlTemp.Digit = 4U;
		PnlOpDispCtrlTemp.SupFlg = (USHORT)FALSE;
		PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals;
		PnlOpLedSetValDec(data, DspData, PnlOpDispCtrlTemp);

		for(i = 0U; i < LedNum; i++)
		{
			DspData->LedBlnkSw[i] = (i == PnlOpDispCtrl.CurDigit)?	0xFFU: 0U;
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(�p�����[�^�����t10�i���[�h)										*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedDispPrmDecWordSigned(SHORT data, PNLOP_DISP_CTRL PnlOpDispCtrl, PNLOP_HANDLE *hPnl)
{	/* data:�\������l, PnlOpDispCtrl : LED Display Control Struct, hPnl : Panel Op Handler Pointer*/

	USHORT	SDataTmp, SDataTmp1, SDataTmp2;
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));
	
	if(PnlOpDispCtrl.CurDigit < 5U)
	{
		PnlOpDispCtrlTemp.SupFlg = (USHORT)FALSE;
		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_NOBLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );

		SDataTmp = (data < 0)? (USHORT)(-data): (USHORT)data;

		if((-10000 < PnlOpDispCtrl.LLimit) && (PnlOpDispCtrl.ULimit < 10000))	/* �����܂�5���ő����ꍇ	*/
		{
			PnlOpDispCtrlTemp.StartDgt = 3U;
			PnlOpDispCtrlTemp.Digit = 4U;
			PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals;
			PnlOpLedSetValDec(SDataTmp, &(hPnl->DspData), PnlOpDispCtrlTemp);
			hPnl->DspData.LedCode[4] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
			hPnl->DspData.LedBlnkSw[PnlOpDispCtrl.CurDigit] = 0xFFU;	/* �J�[�\����	*/
		}
		else							/* �����܂�5���ő���Ȃ��ꍇ-->�ؑ֕\��	*/
		{
			if(hPnl->CurPos < 4U)	/* ��4��	*/
			{
				SDataTmp1 = SDataTmp/10000U;
				SDataTmp2 = SDataTmp - (SDataTmp1*10000U);
				PnlOpDispCtrlTemp.StartDgt = 3U;
				PnlOpDispCtrlTemp.Digit = 4U;
				PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals;
				PnlOpLedSetValDec(SDataTmp2, &(hPnl->DspData), PnlOpDispCtrlTemp);

				hPnl->DspData.LedBlnkSw[PnlOpDispCtrl.CurDigit] = 0xFFU;					/* �J�[�\����	*/
				hPnl->DspData.LedBlnkSw[4] =  LEDCODE_B3;	/* �\�����\��	*/
				hPnl->DspData.LedCode[4] = LEDCODE_B3;
/*				if(data < 0)	Pnl.DspData.LedCode[4] |= LEDCODE_POINT;	*/	/* ���\����'.'	*/
			}
			else				/* ��1��+����	*/
			{
				SDataTmp1 = SDataTmp/10000U;
				PnlOpDispCtrlTemp.StartDgt = 0U;
				PnlOpDispCtrlTemp.Digit = 1U;
				PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals - 4U;
				PnlOpLedSetValDec(SDataTmp1, &(hPnl->DspData), PnlOpDispCtrlTemp);
				hPnl->DspData.LedCode[2] = LEDCODE_BLANK;
				hPnl->DspData.LedCode[1] = LEDCODE_BLANK;
				hPnl->DspData.LedCode[3] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;

				hPnl->DspData.LedBlnkSw[PnlOpDispCtrl.CurDigit-4U] = 0xFFU;				/* �J�[�\����	*/
				hPnl->DspData.LedBlnkSw[4] = hPnl->DspData.LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
			}
			if(data < 0)	
			{
				hPnl->DspData.LedCode[4] |= LEDCODE_POINT;	/* ���\����'.'	*/
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(�p�����[�^16�i���[�h)												*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedDispPrmHexNblWord(USHORT data, PNLOP_DISP_CTRL PnlOpDispCtrl, LEDDSP	*DspData,  USHORT LedNum)
{	/* data:�\������l,PnlOpDispCtrl : LED Display Control Struct,DspData :LED�\���f�[�^�̃A�h���X,	*/
	/* LedNum : LED Number */
	USHORT	i;

	if(PnlOpDispCtrl.CurDigit < 4U)
	{

		DspData->LedCode[4] = (PnlOpDispCtrl.Base == DSPDEF_BASENBL)?
			(LEDCODE_N | LEDCODE_POINT): (LEDCODE_H | LEDCODE_POINT);

		PnlOpLedSetValHex(data, DspData, 3U, 4U);

		for(i = 0U; i < LedNum; i++)
		{
			DspData->LedBlnkSw[i] = (i == PnlOpDispCtrl.CurDigit)?	0xFFU: 0U;
		}
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(10�i�����O)														*/
/*																									*/
/****************************************************************************************************/
static   void	PnlOpLedDispPrmDecLong(ULONG ldata, PNLOP_DISP_CTRL PnlOpDispCtrl, PNLOP_HANDLE *hPnl )
{	/* data:�\������l, PnlOpDispCtrl : LED Display Control Struct, hPnl : Panel Op Handler Pointer*/
	USHORT	SDataTmp2;
	ULONG	LDataAbs, LDataTmp2;
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));

	if(PnlOpDispCtrl.CurDigit < 10U)
	{
		PnlOpDispCtrlTemp.SupFlg = (USHORT)FALSE;
		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_NOBLINK, sizeof(hPnl->PnlLed), (USHORT)TRUE );

		LDataAbs = ((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((LONG)ldata < 0))?	-ldata: ldata;

		if(hPnl->CurPos < 4U)		/* ��4��	*/
		{
			LDataTmp2 = LDataAbs/10000U;
			SDataTmp2 = (USHORT)(LDataAbs - (LDataTmp2*10000U));
			PnlOpDispCtrlTemp.StartDgt = 3U;
			PnlOpDispCtrlTemp.Digit = 4U;
			PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals;
			PnlOpLedSetValDec(SDataTmp2, &(hPnl->DspData), PnlOpDispCtrlTemp);

			hPnl->DspData.LedBlnkSw[PnlOpDispCtrl.CurDigit] = 0xFFU;					/* �J�[�\����	*/
			hPnl->DspData.LedBlnkSw[4] = hPnl->DspData.LedCode[4] = LEDCODE_B3;	/* �\�����\��	*/
/*			if((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0U))		*/
/*				Pnl.DspData.LedCode[4] |= LEDCODE_POINT;			*/	/* ���\����'.'	*/
		}
		else if(hPnl->CurPos < 8U)	/* ��4��	*/
		{
			LDataTmp2 = LDataAbs/100000000U;
			SDataTmp2 = (USHORT)((LDataAbs - (LDataTmp2*100000000U))/10000U);
			PnlOpDispCtrlTemp.StartDgt = 3U;
			PnlOpDispCtrlTemp.Digit = 4U;
			PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals - 4U;
			PnlOpLedSetValDec(SDataTmp2, &(hPnl->DspData), PnlOpDispCtrlTemp);

			hPnl->DspData.LedBlnkSw[PnlOpDispCtrl.CurDigit-4U] = 0xFFU;				/* �J�[�\����	*/
			hPnl->DspData.LedBlnkSw[4] = hPnl->DspData.LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
/*			if((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))		*/
/*				Pnl.DspData.LedCode[4] |= LEDCODE_POINT;			*/	/* ���\����'.'	*/
		}
		else							/* ��2��	*/
		{
			hPnl->DspData.LedCode[3] = ((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((LONG)ldata < 0))?
				LEDCODE_MINUS: LEDCODE_BLANK;
			hPnl->DspData.LedCode[2] = LEDCODE_BLANK;

			SDataTmp2 = (USHORT)(LDataAbs/100000000U);
			PnlOpDispCtrlTemp.StartDgt = 1U;
			PnlOpDispCtrlTemp.Digit = 2U;
			PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals - 8U;
			PnlOpLedSetValDec(SDataTmp2, &(hPnl->DspData), PnlOpDispCtrlTemp);

			hPnl->DspData.LedBlnkSw[PnlOpDispCtrl.CurDigit-8U] = 0xFFU;				/* �J�[�\����	*/
			hPnl->DspData.LedBlnkSw[4] = hPnl->DspData.LedCode[4] = LEDCODE_B0;	/* �\�����\��	*/
		}

		if((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((LONG)ldata < 0))
		{
			hPnl->DspData.LedCode[4] |= LEDCODE_POINT;				/* ���\����'.'	*/
		}
	}
	
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(16�i�����O)														*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedDispPrmHexLong(ULONG ldata, USHORT CurDigit, PNLOP_HANDLE *hPnl)
{	/* ldata:�\������l, CurDigit: ���݃J�[�\���ʒu	, hPnl : Panel Op Handler Pointer*/
	USHORT	SDataTmp2;

	if(CurDigit <= 8U)	
	{

		PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_NOBLINK, sizeof(hPnl->PnlLed), TRUE );

		if(hPnl->CurPos < 4U)	/* ��4��	*/
		{
			SDataTmp2 = (USHORT)(ldata & 0xFFFFU);
			PnlOpLedSetValHex(SDataTmp2, &(hPnl->DspData), 3U, 4U);

			hPnl->DspData.LedBlnkSw[CurDigit] = 0xFFU;					/* �J�[�\����	*/
			hPnl->DspData.LedBlnkSw[4] = LEDCODE_B3;					/* �\�����\��	*/
			hPnl->DspData.LedCode[4] = LEDCODE_B3;

		}
		else						/* ��4��	*/
		{
			SDataTmp2 = (USHORT)((ldata >> 16) & 0xFFFF);
			PnlOpLedSetValHex(SDataTmp2, &(hPnl->DspData), 3U, 4U);

			hPnl->DspData.LedBlnkSw[CurDigit-4U] = 0xFFU;				/* �J�[�\����	*/
			hPnl->DspData.LedBlnkSw[4] = LEDCODE_B6;					/* �\�����\��	*/
			hPnl->DspData.LedCode[4] = LEDCODE_B6;	
		}

	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(���j�^�[16�i���[�h)												*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedDispMonHexWord(USHORT data, PNLOP_HANDLE *hPnl)
{	/* data:�\������l	, hPnl : Panel Op Handler Pointer*/

	hPnl->DspData.LedCode[4] = LEDCODE_H | LEDCODE_POINT;

	PnlOpLedSetValHex(data, &(hPnl->DspData), 3U, 4U);

	PnlOpLedSetBlnkSw( &hPnl->DspData, PNLLED_NOBLINK, sizeof(hPnl->PnlLed), TRUE );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^ LED�\��(���j�^�[�r�b�g�o�C�g)												*/
/*																									*/
/****************************************************************************************************/
static  void	PnlOpLedDispMonBit(UCHAR data, PNLOP_HANDLE *hPnl)
{	/* data:�\������l	, hPnl : Panel Op Handler Pointer*/
	USHORT	i;

	for(i = 0U; i < sizeof(hPnl->PnlLed); i++)
	{
		hPnl->DspData.LedBlnkSw[i] = 0U;
		hPnl->DspData.LedCode[i] = 0U;
	}

	for(i = 0U; i < 8U; i++)	/* 8�r�b�g��\��	*/
	{
		switch(i & 1U)
		{
			case 0U:		/* �����Ԗ� --> �E	*/
				hPnl->DspData.LedCode[i/2U] |= ((data & 1U) != 0U)?
					LEDCODE_B1: LEDCODE_B2;
				break;

			case 1U:		/* ��Ԗ� --> ��	*/
				hPnl->DspData.LedCode[i/2U] |= ((data & 1U) != 0U)?
					LEDCODE_B5: LEDCODE_B4;
				break;

			default:
				break;
		}
		data >>= 1U;
	}

	/* Hi/Lo�w�W����ы�؂�_�̕\�� <V095> */
	hPnl->DspData.LedCode[4] = LEDCODE_B1 | LEDCODE_B2 | LEDCODE_POINT;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ ���j�^�[�\��(10�i���[�h ��������^�Ȃ�)										*/
/*																									*/
/****************************************************************************************************/
void	PnlOpLedDispMonDecWord(USHORT data, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl, USHORT LedNum)
{	/* data:�\������l, LedDspp:LED�\���f�[�^�̃A�h���X, PnlOpDispCtrl : LED Display Control Struct, LedNum : LED Number*/
	USHORT	DataTmp;																		
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));
	PnlOpDispCtrlTemp.SupFlg = (USHORT)FALSE;
	PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals;
	
	DataTmp = ((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((SHORT)data < 0))?	(USHORT)(-data): data;

	if(PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN)
	{
		DataTmp = ((SHORT)data < 0)?	(USHORT)(-data): data;
		if(DataTmp > PNLOPE_WORDMON_MAX) 
		{
			DataTmp = PNLOPE_WORDMON_MAX;	//<V610>
		}
		PnlOpDispCtrlTemp.StartDgt = 3U;
		PnlOpDispCtrlTemp.Digit = 4U;
		PnlOpLedSetValDec(DataTmp, LedDspp, PnlOpDispCtrlTemp );
		LedDspp->LedCode[4] = ((SHORT)data < 0)?	LEDCODE_MINUS: LEDCODE_BLANK;
	}
	else
	{
		PnlOpDispCtrlTemp.StartDgt = 4U;
		PnlOpDispCtrlTemp.Digit = 5U;
		PnlOpLedSetValDec(data, LedDspp, PnlOpDispCtrlTemp);
	}

	PnlOpLedSetBlnkSw( LedDspp, PNLLED_NOBLINK, (LONG)LedNum, TRUE );	

	return;
}




/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ ���j�^�[�\��(10�i�����O ��������^�Ȃ�)										*/
/*																									*/
/****************************************************************************************************/
void	PnlOpLedDispMonDecLong(ULONG LData, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl,  USHORT LedNum)
{	/* LData:�\������l, LedDspp:LED�\���f�[�^�̃A�h���X,	PnlOpDispCtrl : LED Display Control Struct, LedNum : LED Number*/
	USHORT	SDataTmp2;
	ULONG	LDataAbs, LDataTmp2;
	PNLOP_DISP_CTRL	PnlOpDispCtrlTemp;
	
	MlibResetByteMemory((void *)(&PnlOpDispCtrlTemp), sizeof(PnlOpDispCtrlTemp));

	if(PnlOpDispCtrl.DspPos < 3U)
	{
		PnlOpLedSetBlnkSw( LedDspp, PNLLED_NOBLINK, (LONG)LedNum, TRUE );

		LDataAbs = ((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((LONG)LData < 0))?	(ULONG)(-LData): LData;
		PnlOpDispCtrlTemp.SupFlg = (USHORT)FALSE;
		switch(PnlOpDispCtrl.DspPos)
		{
			case 0U:
				LDataTmp2 = LDataAbs/10000U;
				SDataTmp2 = (USHORT)(LDataAbs - (LDataTmp2*10000U));
				PnlOpDispCtrlTemp.StartDgt = 3U;
				PnlOpDispCtrlTemp.Digit = 4U;
				PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals;
				PnlOpLedSetValDec(SDataTmp2, LedDspp, PnlOpDispCtrlTemp );

				LedDspp->LedCode[4] = LEDCODE_B3;	/* �\�����\��	*/
				LedDspp->LedBlnkSw[4] = LEDCODE_B3;
/*				if((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))		*/
/*					leddspp->LedCode[4] |= LEDCODE_POINT;				*/	/* ���\����'.'	*/
				break;

			case 1U:
				LDataTmp2 = LDataAbs/100000000U;
				SDataTmp2 = (USHORT)((LDataAbs - (LDataTmp2*100000000U))/10000U);
				PnlOpDispCtrlTemp.StartDgt = 3U;
				PnlOpDispCtrlTemp.Digit = 4U;
				PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals - 4U;
				PnlOpLedSetValDec(SDataTmp2, LedDspp, PnlOpDispCtrlTemp );

				LedDspp->LedCode[4] = LEDCODE_B6;	/* �\�����\��	*/
				LedDspp->LedBlnkSw[4] = LEDCODE_B6;
/*				if((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))		*/
/*					leddspp->LedCode[4] |= LEDCODE_POINT;				*/	/* ���\����'.'	*/
				break;

			case 2U:
				LedDspp->LedCode[3] = ((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((LONG)LData < 0L))?
					(UCHAR)LEDCODE_MINUS: (UCHAR)LEDCODE_BLANK;
				LedDspp->LedCode[2] = (UCHAR)LEDCODE_BLANK;

				PnlOpDispCtrlTemp.StartDgt = 1U;
				PnlOpDispCtrlTemp.Digit = 2U;
				PnlOpDispCtrlTemp.Decimals = PnlOpDispCtrl.Decimals - 8U;
				PnlOpLedSetValDec(SDataTmp2, LedDspp, PnlOpDispCtrlTemp );

				LedDspp->LedCode[4] = LEDCODE_B0;	/* �\�����\��	*/
				LedDspp->LedBlnkSw[4] = LEDCODE_B0;
				break;

			default:
				break;
		}

		if((PnlOpDispCtrl.Sign == (USHORT)DSPDEF_W_SIGN) && ((LONG)LData < 0L))
		{
			LedDspp->LedCode[4] |= LEDCODE_POINT;					/* ���\����'.'	*/
		}
	}
	
	return;
	
}





/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ �X�e�[�^�X�r�b�g�\�����Z�b�g����											*/
/*																									*/
/****************************************************************************************************/
void	PnlOpLedSetStatusBits(LEDDSP *LedDspp, PNLOP_UNIT_HANDLE	hPnlOpUnitHndl, ULONG BlnkTime)								
{	/* LedDspp:LED�\���f�[�^�̃A�h���X, PnlOpDispCtrl : LED Display Control Struct, BlnkTime : Blink Time*/

	PNLOP_UNIT_HANDLE	PnlOpUnitHndlTp;
	PnlOpUnitHndlTp = hPnlOpUnitHndl;
	
	if(PnlOpUnitHndlTp.BaseCtrlData->PosCtrlSts.CoinSignal || PnlOpUnitHndlTp.SeqCtrlOut->VcmpSts || (PnlOpUnitHndlTp.BaseCtrlOut->CtrlModeOut.b.cm == (UCHAR	)CTRLMODE_TRQ))
	{
		LedDspp->LedCode[4] |= LEDCODE_B0;
	}
	if(!(PnlOpUnitHndlTp.SeqCtrlOut->BeComplete))
	{
		LedDspp->LedCode[4] |= LEDCODE_B6;
	}

/*<V739>SafetyModule���މ��P*/
#if (CCSW_SFTID == SFTID_EXIST)																		  /*<V717>Add:Start*/

/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */

/*	if(!CoutV.f.Sv.HwbbState && CoutSom.UnSomState & SOM_SYS_SFON )		*/

	if((!PnlOpUnitHndlTp.DetHwbbSts->HwbbState) && (1U & SOM_SYS_SFON) )
	{
		if ( !((KlibGetLongTimerMs(BlnkTime)/100U) & 0x08U) )  
		{ 
			LedDspp->LedCode[4] &= ~LEDCODE_B5;
		}
		else													
		{ 
			LedDspp->LedCode[4] |=  LEDCODE_B5;
		}
	}

/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */

/*	if(!CoutV.f.Sv.HwbbState && CoutSom.UnSomState & SOM_SYS_SAFE )		*/

	if((!PnlOpUnitHndlTp.DetHwbbSts->HwbbState) && (1 & SOM_SYS_SAFE) )
	{
		LedDspp->LedCode[4] |=  LEDCODE_B5; 
	}
#endif																								  /*<V717>Add:End  */



	if(PnlOpUnitHndlTp.SeqCtrlOut->TgOnSts)
	{
		LedDspp->LedCode[3] |= LEDCODE_B0;
	}
/* �R���p�C����ʂ��邽�߁A�R�����g�A�E�g���� */

/*	if((AxisHandle->SeqMotSts.var.dPcmdCnt == 0)|| (AxisHandle->BaseControls.CtrlCmdMngr.SpdRefi == 0))		*/
	if((PnlOpUnitHndlTp.SeqCtrlData->dPcmdCnt == 0L) || (PnlOpUnitHndlTp.BaseCtrlData->CtrlCmdMngr.SpdRefi == 0L))
	{
		LedDspp->LedCode[3] |= LEDCODE_B6;
	}
	if((PnlOpUnitHndlTp.BaseCtrlData->CtrlCmdMngr.TrqRefo == 0L) || (PnlOpUnitHndlTp.BaseCtrlData->CtrlCmdMngr.PerrClrReq))
	{
		LedDspp->LedCode[3] |= LEDCODE_B3;
	}
	if(PnlOpUnitHndlTp.SeqCtrlOut->MainPowerOn)
	{
		LedDspp->LedCode[3] |= LEDCODE_B7;
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ �\���l(10�i)�Z�b�g����														*/
/*																									*/
/****************************************************************************************************/
void	PnlOpLedSetValDec(USHORT data, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl )
{	/* data:�\������l, LedDspp:LED�\���f�[�^�̃A�h���X, , PnlOpDispCtrl : LED Display Control Struct	*/
	USHORT	i, Var1Tmp, Var2Tmp;
	UCHAR	*LedBufPtr;

	if(PnlOpDispCtrl.Digit <= 5U)	
	{
		LedBufPtr = (UCHAR	*)&(LedDspp->LedCode[PnlOpDispCtrl.StartDgt]);

		if(data >= decpowertbl[PnlOpDispCtrl.Digit])		/* ��ʌ��𗎂Ƃ�	*/
		{
			Var1Tmp = 	(USHORT)(data/decpowertbl[PnlOpDispCtrl.Digit]);
			data -= (USHORT)(Var1Tmp*decpowertbl[PnlOpDispCtrl.Digit]);
		}

		for(i = 0U; i < PnlOpDispCtrl.Digit; i++)
		{
			Var1Tmp = (USHORT)decpowertbl[PnlOpDispCtrl.Digit-1U-i];	/* ���鐔: �� 1000, 100, 10, 1	*/
			Var2Tmp = data/Var1Tmp;

			if(PnlOpDispCtrl.SupFlg != 0U)	/* ��ʂ�0�̓u�����N�\���Ƃ���	*/
			{
				if(Var2Tmp == 0U) 
				{
					*(LedBufPtr - i) = LEDCODE_BLANK;
				}
				else
				{
					*(LedBufPtr - i) = ledcodetbl[Var2Tmp];
					PnlOpDispCtrl.SupFlg = (USHORT)FALSE;
				}
				
			}
			else
			{
				*(LedBufPtr - i) = ledcodetbl[Var2Tmp];
			}

			data -= (Var1Tmp * Var2Tmp);
		}

		if(( (SHORT)0 < (SHORT)PnlOpDispCtrl.Decimals) && ((SHORT)PnlOpDispCtrl.Decimals < (SHORT)PnlOpDispCtrl.Digit))
		{
			*(LedBufPtr + (PnlOpDispCtrl.Decimals -(PnlOpDispCtrl.Digit-1U))) |= LEDCODE_POINT;
		}
	}
	return;
}




/****************************************************************************************************/
/*																									*/
/*		LED�I�y���[�^ �\���l(16�i)�Z�b�g����														*/
/*																									*/
/****************************************************************************************************/
void	PnlOpLedSetValHex(USHORT data, LEDDSP *LedDspp, USHORT StartDgt, USHORT digit)
{	/* data:�\������l, LedDspp:LED�\���f�[�^�̃A�h���X, StartDgt: �J�n��, digit: ��������	*/
	USHORT	i, Var1Tmp, Var2Tmp;
	UCHAR	*LedBufPtr;

	if(digit <= 4U)		
	{
		LedBufPtr = (UCHAR  *)&(LedDspp->LedCode[StartDgt]);

		if(data >= (0x1U << (digit*4U)))
		{
			data &= ((0x1U << (digit*4U)) -1U);	/* ��ʌ��𗎂Ƃ�	*/
		}

		for(i = 0U; i < digit; i++)
		{
			Var1Tmp = 0x1U << ((digit-1U-i)*4U);	/* ���鐔: �� 0x1000, 0x100, 0x10, 0x1	*/
			Var2Tmp = data/Var1Tmp;
			*(LedBufPtr - i) = ledcodetbl[Var2Tmp];
			data -= (Var1Tmp * Var2Tmp);
		}
	}
	
	return;
}



/****************************************************************************************************/
/*		LED Operator String Set																		*/
/****************************************************************************************************/
void	PnlOpLedSetStr(UCHAR *StrPtr, LEDDSP *LedDspp, PNLOP_DISP_CTRL PnlOpDispCtrl,  USHORT LedNum)
{	/* StrPtr : �\�����镶����A�h���X, LedDspp:LED�\���f�[�^�̃A�h���X, StartDgt: �J�n��, PnlOpDispCtrl : LED Display Control Struct	*/
	/* LedNum : LED Number */
	USHORT	StrVal;
	USHORT	IdxTmp,DgtTmp;
	USHORT	StrIdx;
	UCHAR	*LedBufPtr;

	if((PnlOpDispCtrl.Length != 0U) && (PnlOpDispCtrl.StartDgt < LedNum))	/* ����0 or �J�n�ʒu���\���o�b�t�@�O	*/
	{

		LedBufPtr = &(LedDspp->LedCode[0]);

		StrIdx = 0U;
		DgtTmp = PnlOpDispCtrl.StartDgt;

		while(*(StrPtr + StrIdx) != 0U)
		{
			StrVal = (USHORT)*(StrPtr + StrIdx);
			IdxTmp = StrVal - (USHORT)'-';
			/* �����f�[�^ < '-' �̏ꍇ				*/
			if(StrVal < (USHORT)'-')
			{
				*(LedBufPtr + DgtTmp) = LEDCODE_BLANK;
			}
			/* '-' <= �����f�[�^ <= 'Z' �̏ꍇ		*/
			else if(StrVal <= (USHORT)'Z')
			{
				if( (StrIdx != 0U) && 					/* ������擪�łȂ�		*/
					((UCHAR)'.' == *(StrPtr + StrIdx)) && 	/* '.'�ł�����			*/
					((UCHAR)'.' != *(StrPtr + (StrIdx -1U)))	/* �O�̕�����'.'�łȂ�	*/
				)
				{	/* '.'�ȊO�̕����̌��'.'������ꍇ�͑O�̕����ɕt������B[5][.] -> [5.]	*/
					DgtTmp++;
					*(LedBufPtr + DgtTmp) |= LEDCODE_POINT;
				}
				else	
				{
					*(LedBufPtr + DgtTmp) = ledconvtbl[IdxTmp];
				}
			}
			/* �����f�[�^���������̏ꍇ				*/
			else if(((USHORT)'a' <= StrVal) && (StrVal <= (USHORT)'z'))
			{
				if((USHORT)'h' == StrVal)
				{
					*(LedBufPtr + DgtTmp) = LEDCODE_sH;
				}
				else if((USHORT)'o' == StrVal)
				{
					*(LedBufPtr + DgtTmp) = LEDCODE_sO;
				}
				else if((USHORT)'u' == StrVal)
				{
					*(LedBufPtr + DgtTmp) =  LEDCODE_sU;
				}
				else
				{
					*(LedBufPtr + DgtTmp) = ledconvtbl[IdxTmp - (USHORT)('a' - 'A')];
				}
			}
			/* ��L�ȊO�̏ꍇ						*/
			else
			{
				if((USHORT)'_' == StrVal)
				{
					*(LedBufPtr + DgtTmp) = LEDCODE_B3;
				}
				else if((USHORT)'~' == StrVal)
				{
					*(LedBufPtr + DgtTmp) = LEDCODE_B0;
				}
				else
				{
					*(LedBufPtr + DgtTmp) = LEDCODE_BLANK;
				}
			}

			if(DgtTmp == 0U)	/* �o�b�t�@�E�[�ɂ���	*/
			{
				if( ((StrIdx+1U) < PnlOpDispCtrl.Length) && 				/* ���̓��͕�����������		*/
					((USHORT)'.' == *(StrPtr + StrIdx + 1U)) && 	/* ���̕�����'.'�ł�����	*/
					((USHORT)'.' != *(StrPtr + StrIdx)) )			/* ����̕�����'.'�łȂ���	*/
				{
					*(LedBufPtr + DgtTmp) |= LEDCODE_POINT;	/* '.'��t������		*/
				}

				StrIdx = PnlOpDispCtrl.Length;			/*	break;		*/
			}
			DgtTmp--;
			
			StrIdx++;
			if(StrIdx >= PnlOpDispCtrl.Length)
			{
				break;
			}
		}
	}

	return;
}



/****************************************************************************************************/
/*		LED Operator Blink Set																		*/
/****************************************************************************************************/
void	PnlOpLedSetBlnk(LEDDSP *LedDspp, UCHAR BlnkBtn,  USHORT LedNum)			
{	/* LedDspp:LED�\���f�[�^�̃A�h���X,	BlnkBtn: �_�Ńp�^�[��(1bit=1������), LedNum : LED Number	*/
	USHORT	i;

	for(i = 0U; i < LedNum; i++)					
	{
		LedDspp->LedBlnkSw[i] = ((BlnkBtn & (1U << i)) != 0U)? 0xFFU: 0U;
	}

	return;
}



/****************************************************************************************************/
/*		Panel LED Set to Kernel Variables															*/
/****************************************************************************************************/
static  void	PnlOpLedSetKnlBuffer( LEDDSP *LedDsp, UCHAR *Dist, LONG size )
{	/* LedDspp:LED�\���f�[�^�̃A�h���X,	Dist : Kernel  Buffer  Address  , size : LED �� 	*/
	LONG	i;

		for ( i = 0L; i < size; i++ )
		{
			Dist[i] = (UCHAR)(LedDsp->LedCode[i]) & (~((UCHAR)(LedDsp->LedBlnkSw[i])));
		}
		return;
}



/****************************************************************************************************/
/*		Panel LED Set to Same Data																	*/
/****************************************************************************************************/
static  void PnlOpLedSetBlnkSw( LEDDSP *LedDsp, UCHAR data, LONG size, LONG flag )
{	/* LedDspp:LED�\���f�[�^�̃A�h���X,	data : �\������l  , size : LED �� 	, flag : Enable  Flag */
	LONG	i;

		if ( flag != (LONG)FALSE )
		{
			for ( i = 0L; i < size; i++ )
			{
				LedDsp->LedBlnkSw[i] = data;
			}
		}
		return;
}



/*****************************************************************************
  Description: Panel Op �֘A������
------------------------------------------------------------------------------
  Parameters:
	hLedData;		�\���f�[�^���
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void PnlOpInitialize(PNLOP_LED_DATA_HANDLE *hLedData )
{
	LONG i;
	UCHAR* pBuf;

	pBuf = (UCHAR*)hLedData;

	for(i = 0L; i < (LONG)(sizeof(PNLOP_LED_DATA_HANDLE)); i++)
	{
		*(pBuf+i) = 0U;
	}

	KlibRstLongTimer(&hLedData->var.SysTime);
	KlibRstLongTimer(&hLedData->var.SysTime2);

	hLedData->var.status = (USHORT)(STS_DEN);
	hLedData->var.cnt = 0U;
	hLedData->var.cnt_limt = 0U;
	hLedData->var.timer[0] = 10U;

	return;
}

/*****************************************************************************
  Description: 7segLED�\���f�[�^�Z�b�g
------------------------------------------------------------------------------
  Parameters:
	hLedData;		�\���f�[�^���
	hAlmManager		�A���[�����
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void PnlOpDisplayLedData(PNLOP_LED_DATA_HANDLE *hLedData, ALMMAN_HANDLE  *hAlmManager )
{
	UCHAR code;

	hLedData->var.alm_code = (USHORT)ALMGetAlarmInfo(hAlmManager);

	if(KlibGetLongTimerMs(hLedData->var.SysTime) > hLedData->var.timer[hLedData->var.cnt])
	{
		KlibRstLongTimer(&hLedData->var.SysTime);
		KlibRstLongTimer(&hLedData->var.SysTime2);

		hLedData->var.led[0] = ~(hLedData->var.status);
		hLedData->var.mode = PnlOpSetLedMode( hLedData );	/* ��ԃZ�b�g */

		hLedData->var.cnt++;
		if(hLedData->var.cnt > hLedData->var.cnt_limt)
		{
			hLedData->var.cnt = 0U;	/* �J�E���^���Z�b�g */
		}

		code = ~hLedData->var.led[hLedData->var.cnt];
		KpiSetPanelDispData(&code,1U);	/* �\���X�V */
	}
	else
	{
		if((hLedData->var.cnt == 0U) && (KlibGetLongTimerMs(hLedData->var.SysTime2) > 20U))
		{
			KlibRstLongTimer(&hLedData->var.SysTime2);
			hLedData->var.led[0] = ~(hLedData->var.status);
			code = ~hLedData->var.led[0];
			KpiSetPanelDispData(&code,1U);	/* �\���X�V */
		}
	}

	return;
}

/*****************************************************************************
  Description: 7segLED ��ԃZ�b�g
------------------------------------------------------------------------------
  Parameters:
	hLedData;		�\���f�[�^���
  Return:
	UCHAR			: Servo Mode
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static UCHAR PnlOpSetLedMode(PNLOP_LED_DATA_HANDLE *hLedData )
{
	UCHAR Status;

	Status = 0;

	/* �A���[����� */
	if(hLedData->var.alm_code != 0U) /* alarm */
	{
		if(hLedData->var.alm_code != hLedData->var.lst_alm)
		{
			hLedData->var.timer[0] = 700U;
			hLedData->var.timer[1] = 200U;hLedData->var.led[1] = 0xFFU;
			hLedData->var.timer[2] = 350U;hLedData->var.led[2] = ledcode[LED_A_BITS];
			hLedData->var.timer[3] = 100U;hLedData->var.led[3] = 0xFFU;
			hLedData->var.timer[4] = 350U;hLedData->var.led[4] = ledcode[(hLedData->var.alm_code >> 8U) & 0xFU];
			hLedData->var.timer[5] = 100U;hLedData->var.led[5] = 0xFFU;
			hLedData->var.timer[6] = 350U;hLedData->var.led[6] = ledcode[(hLedData->var.alm_code >> 4U) & 0xFU];
			hLedData->var.timer[7] = 100U;hLedData->var.led[7] = 0xFFU;
			hLedData->var.timer[8] = 350U;hLedData->var.led[8] = ledcode[hLedData->var.alm_code & 0xFU];
			hLedData->var.timer[9] = 100U;hLedData->var.led[9] = 0xFFU;

			hLedData->var.cnt = 0U;
			hLedData->var.cnt_limt = 9U;
			hLedData->var.lst_alm = hLedData->var.alm_code;
		}
		Status = (UCHAR)ALARM_MODE;
	}
	else
	{
		/* �ʏ�^�]��� */
		hLedData->var.cnt = 0U;
		hLedData->var.cnt_limt = 0U;
		hLedData->var.timer[0] = 10U;

		Status = (UCHAR)NORMAL_MODE;
	}
	return	Status;
}


/***************************************** end of file **********************************************/
