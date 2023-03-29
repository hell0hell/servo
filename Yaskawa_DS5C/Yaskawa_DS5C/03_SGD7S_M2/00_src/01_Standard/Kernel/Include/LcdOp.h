/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdOp.h : LCD Operator Common Header File													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Lcd�I�y���[�^�h�e��`																	*/
/*																									*/
/*		2. �p�l���I�y���[�^�h�e��`																	*/
/*																									*/
/*		3. 																							*/
/*																									*/
/*		4. 																							*/
/*																									*/
/*		5. 																							*/
/*																									*/
/*		6. 																							*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.04.15  K.Ozaki																*/
/*								LcdOp.h <= RndApi.h + RndRpx.h +LcdOp.c								*/
/*		Rev.1.10 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___LCDOP___
#define ___LCDOP___

#include "Object.h"

#define ADJUST_STRUCT_ALIGN( x ) 
#define CCSW_HWID  (0x55AA) /* Dummy */
#define CCSW_CMDID  (0xAA55) /* Dummy */


/****************************************************************************************************/
/*																									*/
/*		�I�y���[�^�h�e�}�N����`																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		SGDV�݊����m�ۃ}�N��(��������)																*/
/*--------------------------------------------------------------------------------------------------*/
#define KpiGetAlarmInfo() ALMGetAlarmInfo( pAxRsc->AlmManager ) /* TODO: �b�� */
#define KriResetSystemAlarm() ALMSetAlarmResetRequest( pAxRsc->AlmManager ) /* TODO: �b�� */

#if 0 															/* SGDV and Jupiter					*/
/*--------------------------------------------------------------------------------------------------*/
/*	Status Display Check Macro																		*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_BB()       ( ! RinV.f.BaseEnable )
#define CHK_PDET()     ( ! MencV.f.PhaseReady )
#define CHK_TUNING()   ( RoutV.f.TuningRun )
#define CHK_JOGMODE()  ( BoutV.CtrlMode.b.cm == CTRLMODE_JOG )
#define CHK_ZRETMODE() ( BoutV.CtrlMode.b.cm == CTRLMODE_ZSRCH )
#define CHK_POT()      ( CoutV.f.Sv.Pot )
#define CHK_NOT()      ( CoutV.f.Sv.Not )
#define CHK_PLS()      ( CoutV.f.Sv.Pls )
#define CHK_NLS()      ( CoutV.f.Sv.Nls )
#define CHK_ESTOP()    ( CoutV.f.Sv.EmgStop )
#define CHK_HBB()      ( CoutV.f.Sv.HwbbState )

/*--------------------------------------------------------------------------------------------------*/
/*	Status LED Check Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_SVON()     ( ! CHK_BB() )
#define CHK_TGON()     ( CoutV.f.Ctrl.MotorMoving )
#define CHK_CHG()      ( CoutV.f.Seq.MainPowerOn )
#define CHK_COIN()     ( BoutV.f.Ctrl.Coin ) 
#define CHK_VCMP()     ( CoutV.f.Ctrl.ReachSpd )
#define CHK_TRQMODE()  ( BoutV.CtrlMode.b.cm == CTRLMODE_TRQ )
#define CHK_PREFON()   ( CoutV.f.Sv.Prefon )
#define CHK_VREFON()   ( CoutV.f.Sv.Vrefon )
#define CHK_TREFON()   ( CoutV.f.Sv.Trefon )

#elif CCSW_HWID == HWID_JPCV										/* Jupiter Converter 			*/

/*--------------------------------------------------------------------------------------------------*/
/*	Status Display Check Macro																		*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_BB()       ( ! RinV.f.BaseEnable )
#define CHK_PDET()     ( 0 ) 
#define CHK_TUNING()   ( 0 )
#define CHK_JOGMODE()  ( 0 )
#define CHK_ZRETMODE() ( 0 )
#define CHK_POT()      ( 0 )
#define CHK_NOT()      ( 0 )
#define CHK_PLS()      ( 0 )
#define CHK_NLS()      ( 0 )
#define CHK_ESTOP()    ( 0 )
#define CHK_HBB()      ( 0 )

/*--------------------------------------------------------------------------------------------------*/
/*	Status LED Check Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_SVON()     ( ! CHK_BB() )
#define CHK_TGON()     ( RinV.f.DrvAxRunning )
#define CHK_CHG()      ( RinV.f.DcPowerOn )
#define CHK_COIN()     ( 0 ) 
#define CHK_VCMP()     ( 0 )
#define CHK_TRQMODE()  ( 0 )
#define CHK_PREFON()   ( RinV.f.DrvReady )
#define CHK_VREFON()   ( 0 )
#define CHK_TREFON()   ( 0 )

#else 																/* Mercury ???	 				*/

/*--------------------------------------------------------------------------------------------------*/
/*	Status Display Check Macro																		*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_BB()       (!(pAxRsc->SeqCtrlOut->BeComplete))
#define CHK_PDET()     (!(pAxRsc->MencV->PhaseReady))
#define CHK_TUNING()   ( pAxRsc->FnCmnCtrl->State.TuningRun )
#define CHK_JOGMODE()  ( pAxRsc->BaseCtrlOut->CtrlModeOut.b.cm == CTRLMODE_JOG )
#define CHK_ZRETMODE() ( pAxRsc->BaseCtrlOut->CtrlModeOut.b.cm == CTRLMODE_ZSRCH )
#define CHK_POT()      ( pAxRsc->SeqCtrlOut->PotSigSts | (pAxRsc->BaseCtrlOut->CmdSeqBit & POSITIVE_OT_BIT))
#define CHK_NOT()      ( pAxRsc->SeqCtrlOut->NotSigSts | (pAxRsc->BaseCtrlOut->CmdSeqBit & NEGATIVE_OT_BIT))
#define CHK_PLS()      ( 0/* TODO */ )
#define CHK_NLS()      ( 0/* TODO */ )
#define CHK_ESTOP()    ( 0/* TODO */ )
#define CHK_HBB()      ( pAxRsc->SeqCtrlOut->HwbbSts )
#define CHK_ALARM()	   ( pAxRsc->AlmManager->Status.AlmFlag )	/* <S0FF> */
#define CHK_FSTP()	   (pAxRsc->SeqCtrlOut->FstpSts )												  /* <S1F5> */
#define CHK_MOTOR_LESS_TEST()	( pAxRsc->MotorLessTest->MotorLessTestMode == TRUE )				  /* <S1B7> */

/*--------------------------------------------------------------------------------------------------*/
/*	Status LED Check Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_SVON()     ( ! CHK_BB() )
#define CHK_TGON()     ( pAxRsc->SeqCtrlOut->TgOnSts )
#define CHK_CHG()      ( pAxRsc->PowerManager->PowerSts.PowerOn )
#define CHK_COIN()     ( pAxRsc->BaseControls->PosCtrlSts.CoinSignal )
#define CHK_VCMP()     ( pAxRsc->SeqCtrlOut->VcmpSts )
#define CHK_TRQMODE()  ( pAxRsc->BaseCtrlOut->CtrlModeOut.b.cm == CTRLMODE_TRQ ) 
#define CHK_PREFON()   ( pAxRsc->SeqCtrlOut->f.Prefon ) 
#define CHK_VREFON()   ( pAxRsc->SeqCtrlOut->f.Vrefon ) 
#define CHK_TREFON()   ( pAxRsc->SeqCtrlOut->f.Trefon ) 
#define CHK_PERRCLR()  ( pAxRsc->SeqCtrlOut->f.PerrClr )

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�\�����̒�																					*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_BASENBL			(OBJDEF_BASENBL)			/* �����F�j�u���\��						*/
#define DSPDEF_BASEDEC			(OBJDEF_BASEDEC)			/* �����F10�i�\��						*/
#define DSPDEF_BASEHEX			(OBJDEF_BASEHEX)			/* �����F16�i�\��						*/
#define DSPDEF_BASEBIT			(OBJDEF_BASEBIT)			/* �����F�r�b�g�\��						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�����t�����ۂ�																				*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_W_SIGN			(TRUE)			/* �����t											*/
#define DSPDEF_NOSIGN			(FALSE)			/* �����Ȃ�											*/

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator PRM/MON Display IDs															*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCDPMDSP_MAX		4					/* ��ʐ�											*/
#define	LCDPMDSP_PM			0					/* PRM/MON���										*/
#define	LCDPMDSP_ZSRCH		1					/* ���_�T�[�`���									*/
#define	LCDPMDSP_JOG		2					/* JOG���											*/
#define	LCDPMDSP_PJOG		3					/* PROGRAM JOG���									*/

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Drive Selection 																*/
/*--------------------------------------------------------------------------------------------------*/
#define DRVSEL_MAXUNIT 1
#define DRVSEL_MAXAXISPERUNIT 3

/*--------------------------------------------------------------------------------------------------*/
/*		Drive Selection Type Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define DRVSEL_TYPE_DRVAXIS		0			/* Drive Axis											*/
#define DRVSEL_TYPE_COMMCPU		1			/* Communication CPU									*/
#define DRVSEL_TYPE_CONVERTER	2			/* Converter for Converter Operator						*/

/****************************************************************************************************/
/*																									*/
/*		Sigma Message for LCD operator Message definitions (Unit: Byte)								*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Sigma Message for LCD operator Message definitions (Unit: Byte)								*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSG_AXSIZ		1			/* ���A�h���X�T�C�Y											*/
#define LCDMSG_FUNSIZ		1			/* �R�}���h�T�C�Y											*/
#define LCDMSG_LENSIZ		2			/* ���b�Z�[�W���T�C�Y										*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGR_LEDSIZ		2			/* LED�X�e�[�^�X�T�C�Y										*/
#define LCDMSGR_DSPSIZ		16			/* �\���X�e�[�^�X�T�C�Y										*/
#define LCDMSGR_CHRSIZ		(5*18)		/* �\�������T�C�Y											*/
#define LCDMSGR_SUMSIZ		2			/* �`�F�b�N�T���T�C�Y										*/
#define LCDMSGR_FONSIZ		(5*16)		/* �t�H���g�f�[�^�T�C�Y										*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCDMSG_HDRSIZ		(LCDMSG_AXSIZ  + LCDMSG_FUNSIZ  + LCDMSG_LENSIZ)
#define	LCDMSGR_BDYSIZ		(LCDMSGR_LEDSIZ + LCDMSGR_DSPSIZ + LCDMSGR_CHRSIZ)
/*--------------------------------------------------------------------------------------------------*/
/*		0x64/0x60�������b�Z�[�W��																	*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGR_LEN60		(LCDMSG_HDRSIZ + LCDMSGR_BDYSIZ + LCDMSGR_SUMSIZ)
#define LCDMSGR_LEN64		(LCDMSG_HDRSIZ + LCDMSGR_FONSIZ + LCDMSGR_SUMSIZ)
/*--------------------------------------------------------------------------------------------------*/
/*		Check FEXE_LCDBUF_SIZE																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 //TODO:
#if		FEXE_LCDBUF_SIZE != LCDMSGR_LEN60
******* Build Error : FEXE_LCDBUF_SIZE/LCDMSGR_LEN60 Unmatch Error *******
#endif
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Message Index																				*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGR_AXIDX		0								/* ���A�h���X�C���f�b�N�X				*/
#define LCDMSGR_FUNIDX		(LCDMSGR_AXIDX+LCDMSG_AXSIZ)	/* ���A�h���X�C���f�b�N�X				*/
#define LCDMSGR_LENIDX		(LCDMSGR_FUNIDX+LCDMSG_FUNSIZ)	/* ���b�Z�[�W���C���f�b�N�X				*/
#define LCDMSGR_LEDIDX		(LCDMSGR_LENIDX+LCDMSG_LENSIZ)	/* LED�X�e�[�^�X�C���f�b�N�X			*/
#define LCDMSGR_DSPIDX		(LCDMSGR_LEDIDX+LCDMSGR_LEDSIZ)	/* �\���X�e�[�^�X�C���f�b�N�X			*/
#define LCDMSGR_CHRIDX		(LCDMSGR_DSPIDX+LCDMSGR_DSPSIZ)	/* �\�������C���f�b�N�X					*/
#define LCDMSGR_SUMIDX60	(LCDMSGR_CHRIDX+LCDMSGR_CHRSIZ)	/* �`�F�b�N�T���C���f�b�N�X(60�p)		*/
#define LCDMSGR_FONIDX64	(LCDMSGR_LENIDX+LCDMSG_LENSIZ)	/* �t�H���g�f�[�^�C���f�b�N�X(64�p)		*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGC_KEYIDX		LCDMSGR_LEDIDX					/* �L�[�R�[�h�C���f�b�N�X(�R�}���h)		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Message Buffer Operation Macro																*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_LCDKEY( Buf )		(*(USHORT*)&(Buf[LCDMSGC_KEYIDX]))
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_MSGLEN( Buf )		(*(USHORT*)&(Buf[LCDMSGR_LENIDX]))
#define LPX_SET_MSGLEN( Buf, Set )	(*(USHORT*)&(Buf[LCDMSGR_LENIDX])) = Set
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_LEDSTS( Buf )		(*(USHORT*)&(Buf[LCDMSGR_LEDIDX]))
#define LPX_SET_LEDSTS( Buf, Set )	(*(USHORT*)&(Buf[LCDMSGR_LEDIDX])) = Set
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Macro Definition																*/
/*																									*/
/****************************************************************************************************/
//#if 0 /* TODO: */
#if 1														 				/* <S18F> */
typedef	struct	{
		struct	{								/* �\�����											*/
		USHORT	PnUn[4];						/* Pn/Un���	0 = Un, 1 = Pn						*/
		USHORT	UnNo[4];						/* Un�ԍ�											*/
		USHORT	PnNo[4];						/* Pn�ԍ�											*/
		}		dispinf[LCDPMDSP_MAX];			/* index: PRM/MON Display ID						*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	accesslvl[LCDPMDSP_MAX];		/* �A�N�Z�X���x��(0xFFFF�̓f�[�^����)				*/
		USHORT	Old_OpeDspMode;					/* �O��̃I�y���[�^�̃��[�U�[�萔�\���I��			*/
		USHORT	Old_FnNo;						/* �ۑ�����Ă���Fn�ԍ�								*/
		USHORT	chksum;							/* �`�F�b�N�T��										*/
} LCDEEP;
#define	LCDEEP_WDSIZE		55
/*--------------------------------------------------------------------------------------------------*/
//#if		LCDEEP_WDSIZE != EEPSIZE_LCDINF									/* <S18F> */
//******* Build Error : LCDEEP_WDSIZE/EEPSIZE_LCDINF Unmatch Error *******
//#endif
#define	EEPADR_LCDINF	EEP_NOSUM_ADDRESS( 0, EEP_LCDINF_OFS )				/* <S18F> */
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Keys																			*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDKEY_MODE		0x0001
#define LCDKEY_SHUP		0x0002
#define LCDKEY_UP		0x0004
#define LCDKEY_DOWN		0x0008
#define LCDKEY_SCRL		0x0010
#define LCDKEY_SVON		0x0020
#define LCDKEY_DATA		0x0040
#define LCDKEY_SHDOWN	0x0080
#define LCDKEY_READ		0x0100
#define LCDKEY_WRITE	0x0200
#define LCDKEY_ALMRST	0x0400
#define LCDKEY_RST		(LCDKEY_UP | LCDKEY_DOWN)
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator LED display bits																*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_REF		0x0001
#define LCDLED_TGON		0x0002
#define LCDLED_SVON		0x0004
#define LCDLED_COIN		0x0008
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_READ		0x0020
#define LCDLED_WRITE	0x0040
#define LCDLED_CHG		0x0080
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_REFB		0x0100
#define LCDLED_TGONB	0x0200
#define LCDLED_SVONB	0x0400
#define LCDLED_COINB	0x0800
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_READB	0x2000
#define LCDLED_WRITEB	0x4000
#define LCDLED_CHGB		0x8000

/*--------------------------------------------------------------------------------------------------*/
/*		Online modes (for LopV.Online)																*/
/*--------------------------------------------------------------------------------------------------*/
#define LCD_OFFLINE		0				/* LCD�I�y���[�^���ڑ�����Ă��Ȃ�							*/
#define LCD_ONLINE1		1				/* �t�H���g�����[�h����(0x64�R�}���h)						*/
#define LCD_ONLINE2		2				/* �ʐM�m������(�t�H���g���[�h�����LCD�֌W�ϐ���������)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Edit modes (for LopV.EditMode)																*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDEM_PnUnNum	0				/* Pn/Un�ԍ��ҏW��											*/
#define LCDEM_PnUnSel	1				/* Pn/Un�I��												*/
#define LCDEM_PnData	2				/* �p�����[�^�l�ҏW��										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Pn/Un selection (for Display Inf Tables)													*/
/*--------------------------------------------------------------------------------------------------*/
#define UnSelected		0				/* Un�I��													*/
#define PnSelected		1				/* Pn�I��													*/
/*--------------------------------------------------------------------------------------------------*/
/*		Operator Parameter Write Modes																*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPWM_None				0					/*												*/
#define	OPWM_UpdtWait			1					/*												*/
#define	OPWM_UpdtBlnk			2					/* LCD�̂�										*/
#define	OPWM_InfWrWait			3					/* LCD�̂�										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Function modes (for LopV.FunMode)															*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDFM_Num		0				/* Fn�ԍ��I��												*/
#define LCDFM_Exe		1				/* Pn���s��													*/
/*--------------------------------------------------------------------------------------------------*/
/*		Function number of the Password inpu mode (fn010)											*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDPASSWD_FNNUM (0x010)

#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
#define LCDFM_OPTLIST_RES	2	/* �I�v�V�����⏕�@�\���X�g�擾�R�}���h���X�|���X�������			*/
#define LCDFM_OPTBEGIN_RES	3	/* �I�v�V�����⏕�@�\�J�n�R�}���h���X�|���X�������					*/
#define LCDFM_OPTEXE		4	/* �I�v�V�����⏕�@�\���s��											*/
#define LCDFM_OPTMODE_RES	5	/* �I�v�V�����⏕�@�\����R�}���h���X�|���X�������					*/
#define LCDFM_OPTEND_RES	6	/* �I�v�V�����⏕�@�\�I���R�}���h���X�|���X�������					*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Drive operation status (for LopV.DrvOpStat)													*/
/*--------------------------------------------------------------------------------------------------*/
#define DRVOPST_WAITKEYSEND 0			/* �L�[���]���҂�		*/
#define DRVOPST_SENDKEYINFO 1			/* �L�[���]����		*/
#define DRVOPST_FORCECON    2			/* �����ڑ���	*/
#define DRVOPST_ERROR	    3			/* �G���[���	*/


/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Message I/F																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�⑫�F#define�����G�Ȃ̂ŁA�\���̒�`+#define�ɕύX����B(�����p)							*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�W�I�y�ʐM�R�}���h�R�[�h																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_CMD_NOR				(0x60)			/* �f�W�I�y �ʏ�ʐM								*/
#define	LCD_CMD_LOAD			(0x61)			/* �f�W�I�y EEPROM�Ǐo��							*/
#define	LCD_CMD_SAVE			(0x62)			/* �f�W�I�y EEPROM������							*/
#define	LCD_CMD_DIAG			(0x63)			/* �f�W�I�y ���Ȑf�f�R�}���h						*/
#define	LCD_CMD_INIT			(0x64)			/* �f�W�I�y �d�������R�}���h(�����ʐM)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʏ�ʐM�F�󑗐M�o�b�t�@�f�[�^�\����`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x60									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h									*/
		USHORT	Length;							/* ��M���b�Z�[�W��(Byte��)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	OpeKey;							/* �I�y���[�^����L�[								*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
} RXBUF_NOR;									/* �g�p���@�FRxBuf=(RXBUF_NOR*)LopMsg.CmdBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x60									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h									*/
		USHORT	Length;							/* �������b�Z�[�W��(Byte��)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	LedSts;							/* �k�d�c�\�����									*/
		UCHAR	DispSts[16];					/* �X�e�[�^�X�\��									*/
		UCHAR	DispLine[5][17+1];				/* ��ʕ\�����(Title + 4*DispLine)					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
} TXBUF_NOR;									/* �g�p���@�FTxBuf=(TXBUF_NOR*)LopMsg.ResBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_NOR_LEN  (4 + 2+16+(5*18) + 2)	/* �������b�Z�[�W��(Byte��)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		�d�������F�󑗐M�o�b�t�@�f�[�^�\����`(�����ʐM)											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x64									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h									*/
		USHORT	Length;							/* �������b�Z�[�W��(Byte��)							*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
} RXBUF_INIT;									/* �g�p���@�FRxBuf=(TXBUF_INIT*)LopMsg.CmdBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x64									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h									*/
		USHORT	Length;							/* �������b�Z�[�W��(Byte��)							*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	LcdFont[(5*16)];				/* �k�b�c�I�y���[�^�Ƀ��[�h����t�H���g				*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
} TXBUF_INIT;									/* �g�p���@�FTxBuf=(TXBUF_INIT*)LopMsg.ResBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_INIT_LEN  (4 + (5*16) + 2)		/* �������b�Z�[�W��(Byte��)							*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		<S031> LcdPrmCopy LOAD�R�}���h����M�o�b�t�@�f�[�^�\����`									*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x61									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h									*/
		USHORT	Length;							/* ��M���b�Z�[�W��(Byte��)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	OpeKey;							/* �I�y���[�^����L�[								*/
		USHORT	Data[8];						/* ��M�f�[�^										*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
} RXBUF_LOAD;									/* �g�p���@�FRxBuf=(RXBUF_LOAD*)LopMsg.CmdBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_LOAD_LEN  (LCD_RES_NOR_LEN  +  4)	/* �������b�Z�[�W��(Byte��)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		@@ LcdPrmCopy SAVE�R�}���h����M�o�b�t�@�f�[�^�\����`										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x62									*/
		UCHAR	AxisAdr;						/* ���A�h���X										*/
		UCHAR	CmdCode;						/* �R�}���h�R�[�h									*/
		USHORT	Length;							/* �������b�Z�[�W��									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	LedSts;							/* �k�d�c�\�����									*/
		UCHAR	DispSts[16];					/* �X�e�[�^�X�\��									*/
		UCHAR	DispLine[5][17+1];				/* ��ʕ\�����(Title + 4*DispLine)					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	EepAddr;						/* EEPROM�A�h���X(WORD)								*/
		USHORT	DataNum;						/* ���M�f�[�^��(0�`8)								*/
		USHORT	Data[8];						/* ���M�f�[�^										*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* �`�F�b�N�T��										*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	DispLineBlink[5];				/* Work : ��ʕ\���u�����N���						*/
} TXBUF_SAVE;									/* �g�p���@�FTxBuf=(TXBUF_SAVE*)LopMsg.ResBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_SAVE_LEN  (LCD_RES_LOAD_LEN + 16)	/* �������b�Z�[�W��(Byte��)						*/

/****************************************************************************************************/
/*																									*/
/*		Operator I/F Struct Definition																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�I�y���[�^����L�[��`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {
		UINT	mode		:1;					/* Mode Key											*/
		UINT	up			:1;					/* Up Key											*/
		UINT	down		:1;					/* Down Key											*/
		UINT	shup		:1;					/* Shift-up Key : <-								*/
		UINT	shdown		:1;					/* Shift-down Key : ->								*/
		UINT	data		:1;					/* Data Key											*/
		UINT	svon		:1;					/* Svon Key											*/
		UINT	almrst		:1;					/* Alarm Reset Key									*/
		UINT	scroll		:1;					/* Scroll Key										*/
		UINT	read		:1;					/* Read Key											*/
		UINT	write		:1;					/* Write Key										*/
		UINT	rst			:1;					/* Up+Down��������									*/
} OPKEYFLG;
/*--------------------------------------------------------------------------------------------------*/
/*		�I�y���[�^�_�ŗp�t���O��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {
		UINT	PrmEewrite	:1;					/* Writing Parameter to EEPROM						*/
		UINT	read		:1;					/* Read												*/
		UINT	write		:1;					/* Write											*/
		UINT	no_op		:1;					/* No Operation										*/
		UINT	error		:1;					/* Error											*/
		UINT	done		:1;					/* Done												*/
		UINT	end			:1;					/* End												*/
} BLINKFLG;
/*--------------------------------------------------------------------------------------------------*/
/*		LED�\���f�[�^																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
		UCHAR	LedCode[5];						/* �\������R�[�h									*/
		UCHAR	LedBlnkSw[5];					/* 1�̕������u�����N�Ώ�							*/
} LEDDSP;
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Operation modes (for Lcd.OpMode)															*/
/*--------------------------------------------------------------------------------------------------*/
enum _LCDOM_DISPSTATE {
		LCDOM_StartDisp = 0,		/* 0:�X�^�[�g��ʕ\����											*/
		LCDOM_PrmMon,				/* 1:PRM/MON��ʃ��[�h											*/
		LCDOM_Fun,					/* 2:�⏕�@�\��ʃ��[�h											*/
		LCDOM_SelectUnit,			/* 3:���j�b�g�I����ʃ��[�h										*/
		LCDOM_SelectAxis,			/* 4:���I����ʃ��[�h											*/
		LCDOM_Passwd,				/* 5:�p�X���[�h���͉�ʃ��[�h									*/
		LCDOM_Error,				/* 6:�G���[���(������)											*/
		LCDOM_PrmCopy,				/* 7:<S031> �p�����[�^�R�s�[									*/
		LCDOM_DISPNUM
};

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Op �u�����N�X�C�b�`: TRUE�̊ԃu�����N����												*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	UINT	read	:1;				/* Read�L�[ LED�p												*/
	UINT	write	:1;				/* Write�L�[ LED�p												*/
} LCDBLNKSW;

/*--------------------------------------------------------------------------------------------------*/
/*		�\�����e�[�u��																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {
	USHORT	PnUn;					/* Pn/Un���	0 = Un, 1 = Pn									*/
	USHORT	UnIdx;					/* Un�C���f�b�N�X PnUN == Un���̂ݗL�� <ROM�e�[�u���̂�Un�ԍ�>	*/
	USHORT	PnIdx;					/* Pn�C���f�b�N�X PnUN == Pn���̂ݗL�� <ROM�e�[�u���̂�Pn�ԍ�>	*/
}	LOPDIT;

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator DispInfo Variables																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	USHORT	CurFunIndx;				/* �⏕�@�\���݈ʒu�C���f�b�N�X									*/
	LOPDIT	InfTbl[4][4];			/* �\�����e�[�u��(PnXxx/UnXxx)								*/
} LOPDSP;

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Drive Selection Variables	<0BJ14>												*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	UCHAR	DispNumber;				/* Display Number												*/
	UCHAR	DispCurIdx;				/* Display Current Index										*/
	UCHAR	DispTopIdx;				/* Display TopLine Index										*/
	UCHAR	Spare00001;				/* Spare														*/
/*--------------------------------------------------------------------------------------------------*/
	struct	{						/* Display Drive Data											*/
		UCHAR	Type;				/* Unit Type (Converter, Drive)									*/
		UCHAR	Address;			/* Unit Address		 											*/
		UCHAR	AxisNumber;			/* Drive Axis Number											*/
		UCHAR	DispCurAxis;		/* Display Current Index										*/
/*--------------------------------------------------------------------------------------------------*/
		struct {
			UCHAR	Type;			/* Drive Type (Converter, DriveAxis)							*/
			UCHAR	AxisNo;			/* Drive Axis Number											*/
			USHORT	AlmCode;		/* Drive Alarm Code												*/
		} Axis[DRVSEL_MAXAXISPERUNIT];
/*--------------------------------------------------------------------------------------------------*/
	} Unit[DRVSEL_MAXUNIT];
/*--------------------------------------------------------------------------------------------------*/
	ADJUST_STRUCT_ALIGN( Dummy )	/* Adjust HEW Structure to Long Align							*/
} DRVSEL;

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Common Variables																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	UCHAR	CurMsgCmd;				/* ���񃁃b�Z�[�W�R�}���h <0BJ14>								*/
	UCHAR	DrvSelDispMode;			/* �h���C�u�I����ʃ��[�h <0BJ14> 								*/
	USHORT	PasswdIdx;				/* Fn010��index <0BJ14>											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	Online;					/* LCD�I�y���[�^�ڑ����										*/
	USHORT	OpMode;					/* LCD�I�y���[�^������										*/
	ULONG	StDispTime;				/* �N�����\���v���p												*/
	USHORT	RawKeyLast;				/* �L�[�f�[�^�O��l												*/
	USHORT	KeyHoldCnt;				/* �L�[�z�[���h�E���s�[�g�p�J�E���g								*/
	USHORT	ActiveLine;				/* �A�N�e�B�u�s													*/
	USHORT	EditMode;				/* �ҏW���[�h													*/
	USHORT	DataDigit;				/* �f�[�^����													*/
	USHORT	CurDigit;				/* ���ݕҏW�ʒu													*/
	USHORT	EewriteMode;			/* EEPROM�����ݏ��												*/
	USHORT	FunMode;				/* LCD�I�y���[�^�⏕�@�\���									*/
/*--------------------------------------------------------------------------------------------------*/
	LCDBLNKSW  LedBlnkSw;			/* �u�����N�X�C�b�`												*/
	DRVSEL	*pDrvSel;				/* �h���C�u�I���f�[�^�|�C���^									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT  PrmInfoGetDone;			/* <S031> Parameter Information Get Complete					*/
/*--------------------------------------------------------------------------------------------------*/
} LOPV;



/****************************************************************************************************/
/*																									*/
/*		Operator I/F Functions 																		*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdOpMain(						/* Lcd Operator Main Program							*/
		MSGIF	*hMsg,						/* Message Interface									*/
		DRVSEL	*pDrvSel	);				/* Drive Selection Data for Multi-Axis System			*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdOpFinish( MSGIF *hMsg );		/* Lcd Operator Finish Procedure						*/
/*--------------------------------------------------------------------------------------------------*/
//void	RpxLcdOpInitDispInfo( void );		/* Lcd Operator Init. Display Info. Table				*/
void	RpxLcdOpInitDispInfo(  AXIS_HANDLE* );	/* Lcd Operator Init. Display Info. Table  <S213> 	*/

/*--------------------------------------------------------------------------------------------------*/
/*		�k�b�c�I�y���[�^�����֐�																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdClrTxBuf(						/* Clear ResBuf(TxBuf)									*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	RpxLcdPnUnEditSub(					/* Pn/Un Edit Mode Procedure							*/
		USHORT	dispID,						/* �f�B�X�v���[ID(LCDPMDSP_PM/JOG/etc)					*/
		OPKEYFLG *pKeyMk,					/* �L�[�������̃A�h���X								*/
		BLINKFLG *pBlnkKickFlg	);			/* �u�����N�N���t���O�|�C���^�[							*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdMkDspDataPnUnLines(			/* Make Display Data for Pn/Un Lines					*/
		USHORT	dispID,						/* �f�B�X�v���[ID(LCDPMDSP_PM/JOG/etc)					*/
		USHORT	BlnkSupFlg,					/* �u�����N�}�~�t���O									*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdRstPnUnDspData( void );		/* Reset Pn/UN Display Data 							*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetPnUnData(					/* Pn/Un Display Data Set								*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		ULONG	Data,						/* �\���f�[�^											*/
		USHORT	IsLong,						/* �����O�p�����[�^��TRUE								*/
		USHORT	Base,						/* ��(�\������)											*/
		USHORT	Sign,						/* �����L��(�\������)									*/
		SHORT	Decimals,					/* �����_�ȉ�����(�\������)								*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetSignedDecData(				/* Lcd Operator �����t10�i���\��						*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		LONG	Data,						/* �\���f�[�^											*/
		USHORT	Digit,						/* �\�����錅��(�����_�܂܂�)							*/
		USHORT	SupFlg,						/* TRUE=��ʌ���0�̏ꍇ�A'0'��\�����Ȃ�				*/
		SHORT	Decimals,					/* �����_�ȉ�����(���̏ꍇ�̂ݗL��)						*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetStr(						/* Lcd Operator String Set								*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		UCHAR	*pStr,						/* �\�����镶����A�h���X								*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxLcdSetSignChar(					/* Lcd Operator Sign Char Set							*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		LONG	Data,						/* �\������f�[�^										*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetDecPnt(					/* Lcd Operator Decimal w/point Set						*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		ULONG	Data,						/* �\���f�[�^											*/
		USHORT	Digit,						/* �\�����錅��(�����_�܂܂�)							*/
		USHORT	SupFlg,						/* TRUE=��ʌ���0�̏ꍇ�A'0'��\�����Ȃ�				*/
		SHORT	Decimals,					/* �����_�ȉ�����(���̏ꍇ�̂ݗL��)						*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetValHex(					/* Lcd Operator HEX Set									*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		ULONG	Data,						/* �\���f�[�^											*/
		USHORT	Digit,						/* �\�����錅��											*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetBit(						/* Lcd Operator BIT Set									*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		UCHAR	Data,						/* �\���f�[�^											*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetBlnk(						/* Lcd Operator LCD Blink Set							*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		USHORT	Dpos,						/* �J�n���ʒu(���[��0)									*/
		USHORT	Length,						/* ������												*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdClrLineBlnk(					/* Lcd Operator LCD Blink Stop							*/
		USHORT	Lpos,						/* �J�n�s�ʒu(�ŏ�s��0)								*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetResStatus( UCHAR *ResBuf );/* <S031> Lcd Operator Set Status						*/
void 	RpxLcdSetBlink (UCHAR *ResBuf);		/* <S031> Lcd Set Blink									*/


/*--------------------------------------------------------------------------------------------------*/
/*		<S031> �k�b�c�p�����[�^�R�s�[�h�e�֐�														*/
/*--------------------------------------------------------------------------------------------------*/
#if	( SVFSEL_LCDPRMCOPY_USE == 1 )
void	RpxLcdPrmCopyInit( void );			/* �p�����[�^�R�s�[�@�\����������						*/
LONG	RpxLcdPrmInfoGet( void );			/* �p�����[�^�u���b�N���擾����						*/
void	RpxLcdPrmCopyMain( MSGIF *Hmsg, DRVSEL *pDrvSel );	/* �p�����[�^�R�s�[�@�\���C������		*/
#define __LCD_PRM_COPY_MAIN(x)  (x)
#define RpxChkPrmCopyInfoGet( ) (LopV.PrmInfoGetDone)
#else
#define	RpxLcdPrmCopyInit( ) {;}
#define	RpxLcdPrmInfoGet( )  (TRUE)
#define	RpxLcdPrmCopyMain( ) {;}
#define __LCD_PRM_COPY_MAIN(x)  (LcdError)
#define RpxChkPrmCopyInfoGet( ) (TRUE)
#endif
/*------------------------------------------------------------------------------------------*///<S197>
/*		�I�y���[�^���[�U�[�萔�\�� �����ύX�֐��iPn00B�ŕ\�����Ȃ��p�����[�^�ɋ����I�ɃA�N�Z�X���邽�߁j			*/
/*------------------------------------------------------------------------------------------*/
void LpxPrmChangeOpeDspMode(void);			/* ���ׂẴ��[�U�[�萔��\��						*/
void LpxPrmReChangeOpeDspMode(void);		/* �\���ݒ�����ɖ߂�								*/

/****************************************************************************************************/
/*																									*/
/*		�I�y���[�^�h�e �\�����e�[�u��																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
// Jupiter Converter
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_JPCV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		PRM/MON						*/ \
		UnSelected, 0x007, 0x001,	/* Un, Un007, Pn001									*/ \
		UnSelected, 0x009, 0x002,	/* Un, Un009, Pn002									*/ \
		UnSelected, 0x00A, 0x00B	/* Un, Un00A, Pn00B									*/ \
	}
#define CINITDISP_ZSET_JPCV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		���_�T�[�`					*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x003, 0x002,	/* Un, Un003, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_JOG_JPCV  { \
		PnSelected, 0x000, 0x304,	/* Pn, Un000, Pn304		JOG							*/ \
		UnSelected, 0x000, 0x001,	/* Un, Un000, Pn001									*/ \
		UnSelected, 0x002, 0x002,	/* Un, Un002, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_PJOG_JPCV  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x533,	/* Pn, Un007, Pn533									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}
/*--------------------------------------------------------------------------------------------------*/
// Jupiter Spindle Analog I/F
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_JPAN  { \
		UnSelected, 0x000, 0x006,	/* Un, Un000, Pn006		PRM/MON						*/ \
		UnSelected, 0x002, 0x007,	/* Un, Un002, Pn007									*/ \
		UnSelected, 0x008, 0x008,	/* Un, Un008, Pn008									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_ZSET_JPAN  { \
		UnSelected, 0x000, 0x006,	/* Un, Un000, Pn006		���_�T�[�`					*/ \
		UnSelected, 0x002, 0x007,	/* Un, Un002, Pn007									*/ \
		UnSelected, 0x003, 0x008,	/* Un, Un003, Pn008									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_JOG_JPAN  { \
		PnSelected, 0x000, 0x304,	/* Pn, Un000, Pn304		JOG							*/ \
		UnSelected, 0x000, 0x007,	/* Un, Un000, Pn007									*/ \
		UnSelected, 0x002, 0x008,	/* Un, Un002, Pn008									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_PJOG_JPAN  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x533,	/* Pn, Un007, Pn533									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}
/*--------------------------------------------------------------------------------------------------*/
// SGDV Standard (Rotary)
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_SGDV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		PRM/MON						*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x008, 0x002,	/* Un, Un008, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_ZSET_SGDV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		���_�T�[�`					*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x003, 0x002,	/* Un, Un003, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_JOG_SGDV  { \
		PnSelected, 0x000, 0x304,	/* Pn, Un000, Pn304		JOG							*/ \
		UnSelected, 0x000, 0x001,	/* Un, Un000, Pn001									*/ \
		UnSelected, 0x002, 0x002,	/* Un, Un002, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_PJOG_SGDV  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x533,	/* Pn, Un007, Pn533									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}

/*--------------------------------------------------------------------------------------------------*/
// SGDV Standard (Linear)
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_SGDV_L  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		PRM/MON						*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x008, 0x002,	/* Un, Un008, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_ZSET_SGDV_L  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		���_�T�[�`					*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x003, 0x002,	/* Un, Un003, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_JOG_SGDV_L  { \
		PnSelected, 0x000, 0x383,	/* Pn, Un000, Pn383		JOG							*/ \
		UnSelected, 0x000, 0x001,	/* Un, Un000, Pn001									*/ \
		UnSelected, 0x002, 0x002,	/* Un, Un002, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_PJOG_SGDV_L  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x585,	/* Pn, Un007, Pn585									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}

/*--------------------------------------------------------------------------------------------------*/
/*  Mercury Standard ?																				*/
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON  CINITDISP_PRMMON_SGDV
#define CINITDISP_ZSET    CINITDISP_ZSET_SGDV
#define CINITDISP_JOG     CINITDISP_JOG_SGDV
#define CINITDISP_PJOG    CINITDISP_PJOG_SGDV

/****************************************************************************************************/
/*																									*/
/*		�I�y���[�^�h�e�֐���`																		*/
/*																									*/
/****************************************************************************************************/
#if		CCSW_HWID == HWID_JPCV
void	LcdStartDisp( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 0:������ʕ\��							*/
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 1:�p�����[�^/���j�^���					*/
void	LcdFun( MSGIF *hMsg, DRVSEL *pDrvSel );			/* 2:�⏕�@�\���s���						*/
void	LcdSelectUnit( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 3:���j�b�g�I�����						*/
void	LcdSelectAxis( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 4:���I�����								*/
void	LcdPasswd( MSGIF *hMsg, DRVSEL *pDrvSel );		/* 5:�p�X���[�h���͉�ʃ��[�h				*/
void	LcdError( MSGIF *hMsg, DRVSEL *pDrvSel );		/* 6:�G���[���(������)						*/
#else
void	LcdStartDisp( MSGIF *hMsg, DRVSEL *pDrvSel );
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel );
void	LcdFun( MSGIF *hMsg, DRVSEL *pDrvSel );
//#define	LcdSelectUnit LcdSelectDrive
#define LcdSelectUnit LcdError
void	LcdSelectAxis( MSGIF *hMsg, DRVSEL *pDrvSel );
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i�Q���Ή��j�̂�	*//* <S0E4> */
void	LcdDspDatSelAxs( UINT ax_no );	/*	Lcd Display Data Select Axis	*/
#endif
#define	LcdPasswd LcdError
void	LcdError( MSGIF *hMsg, DRVSEL *pDrvSel );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		CMD Option Function Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#if CCSW_CMDID != CMDID_EXIST

#define	LcdOptGetNextOpPrmNo(x1, x2, x3, x4) (0)
#define	LcdOptGetNextSvPrmIdx(x1, x2, x3)    (0)
#define	LcdOptSetOpPrmReadReq(x1, x2, x3)    (0)
#define	LcdOptUpdateResponseData(x1, x2)     (0)
#define	LcdOptSetDataBlinkDPos(x1)  (0)
#define LCD_CHKCMD_RCVWAIT() ( TRUE )

#else

USHORT	LcdOptGetNextOpPrmNo(USHORT, USHORT, USHORT, DEGOP_PRM *);
USHORT	LcdOptGetNextSvPrmIdx(USHORT, USHORT, DEGOP_OPTCOM *);
LONG	LcdOptSetOpPrmReadReq(USHORT, USHORT, DEGOP_OPTCOM *);
LONG	LcdOptUpdateResponseData(USHORT, DEGOP_OPTCOM *);
USHORT	LcdOptSetDataBlinkDPos(DEGOP_PRM *);
#define LCD_CHKCMD_RCVWAIT() ( LcdOpt[LCDPMDSP_PM].Prm[LopV.ActiveLine].State.RcvWait != TRUE )

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�����\��																					*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
#define CSTR_INITDSP_L1 "Unit Detecting"
#define CSTR_INITDSP_L2 "Please Wait..."
#else
#define CSTR_INITDSP_L1 "Initializing"
#define CSTR_INITDSP_L2 "Please Wait..."
#endif
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i�Q���Ή��j�̂�	*//* <S0E4> */
#define CSTR_SELAXS_TITLE "-AXIS SELECTION-"	/*	���I����ʂP�s�ڂ̃^�C�g���\��	*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		���ԍ��\��																					*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
#define CCHR_AXISNUM 'C'
#else
#define CCHR_AXISNUM '1' /* Single Axis */
#endif

/* <S113> Start */
/*--------------------------------------------------------------------------------------------------*/
/*	�I�y���[�^�̃��[�U�[�萔�\���I��																*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEDSP_SETUPPRM			0x0000		/* SETUP�p���[�U��`�p�����[�^�̂�						*/
#define OPEDSP_ALLPRM			0x0001		/* �S���[�U��`�p�����[�^								*/
/* <S113> End */


/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�h�e��`																			*/
/*																									*/
/****************************************************************************************************/
#if 0 /* SGDV */
/*--------------------------------------------------------------------------------------------------*/
USHORT	RpxMonSearchIdxOp(					/* ���j�^�e�[�u���̃C���f�b�N�X��������					*/
		LONG	SrchNo,						/* ���j�^�ԍ�											*/
		LONG	BaseIdx,					/* �����J�n�C���f�b�N�X									*/
		HMSGIF	Hmsg			);			/* ���b�Z�[�WIF�n���h��									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxPrmSearchIdxOp(					/* �p�����[�^�e�[�u���̃C���f�b�N�X��������				*/
		LONG	SrchNo,						/* �p�����[�^�ԍ�										*/
		LONG	BaseIndex,					/* �����J�n�C���f�b�N�X									*/
		HMSGIF	Hmsg			);			/* IF�n���h��											*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_WRITEINHIBIT()  (Iprm.f.PrmWrtInhibit )
#else
//#define RpxMonSearchIdxOp( x1, x2, x3 ) (0) /* TODO */
//#define RpxPrmSearchIdxOp( x1, x2, x3 ) (0) /* TODO */

#define CHK_WRITEINHIBIT()  (pAxRsc->RegManager->hPnReg->WriteAccessLock)	/* check write inhibit */
#endif

/****************************************************************************************************/
/*																									*/
/*		�p�l���I�y���[�^�h�e��`																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�l���I�y���[�^�h�e�֐�																	*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PANELOP_USE == 0											/* Not Use Panel */
/*--------------------------------------------------------------------------------------------------*/
#define RpxPnlOpMain( x )							{ ;}
#define RpxPnlOpInit( )								{ ;}
#define RpxPnlOpMonInit( x )						{ ;}
#define RpxPnlOpFinish( x )							{ ;}
/*--------------------------------------------------------------------------------------------------*/
#define RpxLedSetStatusBits( x1, x2 )				{ ;}
#define RpxLedSetValDec( x1, x2, x3, x4, x5, x6 )	{ ;}
#define RpxLedSetValHex( x1, x2, x3, x4 ) 			{ ;}
#define RpxLedSetStr( x1, x2, x3, x4 )				{ ;}
#define RpxLedDispMonDecWord( x1, x2, x3, x4 )		{ ;}
#define RpxLedDispMonDecLong( x1, x2, x3, x4, x5 )	{ ;}
#define RpxLedSetBlnk( x1, x2 )						{ ;}
/*--------------------------------------------------------------------------------------------------*/
#else
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
		HMSGIF	Hmsg );						/* ��ŏ��� */
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
#endif

/****************************************************************************************************/
/*																									*/
/*		�I�y���[�^�h�e ���b�Z�[�W�]��																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Dummy Message Declaration														*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
	#define DECL_DUMMY_MSG()            \
		const UCHAR dummyMsg[] = {      \
			0x00, /* Slave Address */   \
			0x60, /* Function code */   \
			0x08, /* Message length */  \
			0x00, /* Message length */  \
			0x00, /* Key information */ \
			0x00, /* Key information */ \
			0xF8, /* check sum */       \
			0x9F  /* check sum */       \
		}
#else
	#define DECL_DUMMY_MSG() /* nothing */ 
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Message Transfar Function 														*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
	#define LCD_TRANSMESSAGE() { \
		LONG Axis = pDrvSel->Unit[ pDrvSel->DispCurIdx ].DispCurAxis + 1; \
		KriAddLbusMsgQueue ( LMQUEID_MSGTRANSFER, \
							 (Axis << 16) | pDrvSel->Unit[ pDrvSel->DispCurIdx ].Address, \
							 sizeof(dummyMsg), \
							 dummyMsg ); \
		KpiRstLongTimer (&LopV.StDispTime); /* For timeout measure */  \
	}
#else
	#define LCD_TRANSMESSAGE() {;}
#endif

/****************************************************************************************************/
/*																									*/
/*		�I�y���[�^�h�e�ϐ���`																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Axis Variables																	*/
/*--------------------------------------------------------------------------------------------------*/
extern	LOPDSP	LopDsp;						/*														*/
extern	LCDEEP	LcdEep[MAX_AXIS_NUMBER];	/* �k�b�c�I�y���[�^�F�\��EEPROM���				 <S208> */
extern	LCDEEP	*pLcdEep;					/* �k�b�c�\�����|�C���^						 <S208> */
extern	USHORT	LcdWtReq;					/* �k�b�c�\�����EEPROM�������ݗv��				 <S208> */
extern	USHORT	LcdWtCnt;					/* �k�b�c�\����񏑂����ݒ��J�E���^�t���O		 <S208> */
extern	ULONG	LcdRomAdr;					/* �k�b�c�\����񏑂�����EEPROM�A�h���X			 <S208> */
extern	ULONG	LcdRomBaseAdr;				/* �k�b�c�\����񏑂�����EEPROM�A�h���X			 <S208> */
extern	USHORT	*LcdMemAdr;					/* �k�b�c�\����񏑂����݃������[�A�h���X		 <S208> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i�Q���Ή��j�̂�	*//* <S0E4> */
extern	DRVSEL	DrvSel;					/* �k�b�c�I�y���[�^�F�h���C�u�i���j�I��	*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Common Variables																*/
/*--------------------------------------------------------------------------------------------------*/
extern	LOPV		LopV;					/*														*/
extern	OPKEYFLG	OpKeyMk;				/*														*/
extern	OPKEYFLG	OpKeyBrk;				/*														*/
extern	OPKEYFLG	OpKeyLvl;				/*														*/
extern	BLINKFLG	OpBlnkFlg;				/*														*/
extern	BLINKFLG	OpBlnkKickFlg;			/*														*/
extern	ULONG		OpBlnkTime;				/*														*/
extern  FUNMSG		OpFunMsg;				/*														*/

extern	FUN_MANAGER	FnManagerSto;			/*														*/
#endif
/***************************************** end of file **********************************************/
