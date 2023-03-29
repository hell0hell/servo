/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SigmaMemobus.c : Memobus I/F Module															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����o�X���b�Z�[�W����(Message���,Read/WriteRegisters)���s���B							*/
/*																									*/
/*			 1) Loopback Test																		*/
/*			 2) System Download																		*/
/*			 3) Read  One Register																	*/
/*			 4) Write One Register																	*/
/*			 5) Read  Sequential Registers															*/
/*			 6) Write Sequential Registers															*/
/*			 7) Read  Random     Registers															*/
/*			 8) Write Random     Registers															*/
/*			 9) Others																				*/
/*																									*/
/*	�� �� : �{���W���[���́A�����ӏ�����Q�Ƃ����B												*/
/*			�{���W���[�����ŁA�ÓI�ϐ��̒�`�͍s��Ȃ����ƁB										*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2008.02.08  T.Taniguchi															*/
/*		Rev.1.01 : 2013.07.03  K.Ozaki																*/
/*		Rev.1.02 : 2013.09.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include 	"KnlApi.h" 							//#include "Round.h"
//#include 	"Information.h"						/* Information */
//#include	"UsbFunctionIF.h"					/* TODO: ��ŏ��� */
//#include	"Memory.h"							/* Data Trace */
#include	"FirmDownload.h"					/* FirmDownload(50h)	<S077>	*/
#include	"MemobusFunctions.h"				/* <S150> */
#include	"HwDeviceIf.h"						/* <S1A2> */

/* <boot> >>>>> */
#ifndef NULL
#define NULL	0
#endif
/* <<<<< <boot> */
/****************************************************************************************************/
/*																									*/
/*		�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
extern SHORT	MbusFuncSystemDownLoad( HMSGIF Hmsg );	/*<S150>*/

/*--------------------------------------------------------------------------------------------------*/
/*		Rpx�֐���`�F�֐����ύX�s��																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxMsgifMainProcedure( HMSGIF Hmsg );			/* MessageI/F�F���C������					*/
void	RpxMsgifFuncExecResponse( HMSGIF Hmsg );		/* MessageI/F�F�⏕�@�\���s���ʉ�������		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Lpx�֐���`�F�֐����ύX���R																	*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusLoopbackTest( HMSGIF Hmsg );				/* Memobus�F���[�v�o�b�N�e�X�g				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg );			/* Memobus�F�P�ꃌ�W�X�^������				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg );			/* Memobus�F�A�����W�X�^�Ǐo��				*/
void	LpxMbusWriteSeqRegisters( HMSGIF Hmsg );		/* Memobus�F�A�����W�X�^������				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadMaxNumberOfRegisters( HMSGIF Hmsg );	/* Memobus�F�ő僌�W�X�^���Ǐo��			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxGetMaxNumberOfRdRegisters( HMSGIF Hmsg, LONG FixedLen );
LONG	LpxGetMaxNumberOfWrRegisters( HMSGIF Hmsg, LONG FixedLen );
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusSendErrorResponse( HMSGIF Hmsg, ULONG ErrCode );
LONG	LpxRegifRegNoError( LONG RegNo, LONG Number, USHORT *RdBuf, HMSGIF Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/

/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Memory Object Register Read/Write															*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadMemObject(					/* �p�����[�^�ǂݏo��(���W�X�^�w��)						*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �Ǐo���f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �Ǐo���f�[�^�o�b�t�@									*/
		HMSGIF	Hmsg			);			/* �Ǐo��IF�n���h��										*/
LONG	LpxWriteMemObject(					/* �p�����[�^�������ݏ���(���W�X�^�w��)					*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �������݃f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �������݃f�[�^�o�b�t�@								*/
		HMSGIF	Hmsg			);			/* �Ǐo��IF�n���h��										*/



/****************************************************************************************************/
/*																									*/
/*		���W�X�^�h�e�֐��e�[�u��																	*/
/*																									*/
/****************************************************************************************************/
#define	RFTBLIDX( RegNo )	((RegNo>>12) & 0x0F)			/* Get RegFuncTbl Index					*/
/*--------------------------------------------------------------------------------------------------*/
typedef	LONG	(*REGFUN)( LONG RegNo, LONG Number, USHORT *RdBuf, HMSGIF Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_BIG_ENDIAN == 1
#define GET_LWD_REGDATA( xlong )		(USHORT)((xlong)>>0 )
#define GET_HWD_REGDATA( xlong )		(USHORT)((xlong)>>16)
#define ULONGOF_REGDATA( lwd, hwd )		(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#else
#define GET_LWD_REGDATA( xlong )		(USHORT)((xlong)>>0 )
#define GET_HWD_REGDATA( xlong )		(USHORT)((xlong)>>16)
#define ULONGOF_REGDATA( lwd, hwd )		(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#endif

/****************************************************************************************************/
/*																									*/
/*		Define Macros, etc																			*/
/*																									*/
/****************************************************************************************************/
#define LPX_CHK_SLOT( SfcH )			(SfcH == 0x01)
#define LPX_GET_SLOT_BYTELEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 2 : 0)
#define LPX_GET_SLOT_WORDLEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 1 : 0)
/*--------------------------------------------------------------------------------------------------*/
/*	Test Macro																						*/
/*--------------------------------------------------------------------------------------------------*/
#undef MlibCnvBig16
#define MlibCnvBig16( x ) ( (__rev( x )>>16) )  /* ARMv5��REV16���߂��Ȃ� */
#define MSGIF_SEND( resbuf, reslen )   ( Hmsg->Sif->StxRes( Hmsg->Sif, (resbuf), (reslen) ) )

/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		MessageI/F : Main Procedure																	*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifMainProcedure( HMSGIF Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Reset Hmsg Variables																		*/
/*--------------------------------------------------------------------------------------------------*/
		Hmsg->LstErrRno = Hmsg->ErrRno;					/* �ُ탌�W�X�^�ԍ��O��l					*/
		Hmsg->ErrRno    = 0xFFFF;						/* �ُ탌�W�X�^�ԍ����Z�b�g					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Call the Function													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Hmsg->CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 LpxMbusLoopbackTest( Hmsg );
			 break;
		case MFC_SVXMSG16:
			 switch( Hmsg->CmdBuf[3] )
			 {
//			 case SFC_WR_ONEREG: LpxMbusWriteOneRegister( Hmsg ); break;
			 case SFC_RD_SEQREG: LpxMbusReadSeqRegisters( Hmsg ); break;
//			 case SFC_RD_MAXNUM: LpxMbusReadMaxNumberOfRegisters( Hmsg ); break;
			 default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
			 }
			 break;
 
		case MFC_SYSDWNLD:
			 MbusFuncSystemDownLoad( Hmsg );
			 break;

		default:
			 LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR );
			 break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F���[�v�o�b�N�e�X�g																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusLoopbackTest( HMSGIF Hmsg )
{
ULONG	i;
USHORT	*wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT	*wResBuf = (USHORT *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		�����f�[�^�쐬																				*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < (8/2); i++ )
		{
			wResBuf[i] = wCmdBuf[i];					/* Set LoopBack Response					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
/*		MlibSetCRC16MB(  Hmsg->ResBuf, 8 ); */			/* CRC-16�̌v�Z�͕s�v						*/
		MSGIF_SEND( Hmsg->ResBuf, 8 );				/* Start Transmit Response Message			*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F�P�ꃌ�W�X�^������																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg )
{
#if 0	/*<boot>*/
LONG	rc;
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݊��������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����f�[�^���M�w��																			*/
/*--------------------------------------------------------------------------------------------------*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
#endif	/*<boot>*/
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F�A�����W�X�^�Ǐo��																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg )
{
LONG	i;
LONG	Length;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
LONG	RegNum  = MlibCnvBig16( wCmdBuf[3+s] );
USHORT	*Ptr;
ULONG	BoardId;	/*<S1A2>*/
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�ԍ�����																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( RegNo )
		{
		case 0xC800:	/* �T�[�{�p�b�N�`�� */
//<S1A2>			Ptr = (USHORT*)"SGD7X-XXXXXXX\0\0\0";
			BoardId = HALdrv_ReadBoardID();
			switch( BoardId )
			{
			case BOARD_ID_S_AP:
				Ptr = (USHORT*)"SGD7S-XXA00A\0\0\0\0";
				break;
			case BOARD_ID_S_M2:
				Ptr = (USHORT*)"SGD7S-XXA10A\0\0\0\0";
				break;
			case BOARD_ID_S_M3:
				Ptr = (USHORT*)"SGD7S-XXA20A\0\0\0\0";
				break;
			case BOARD_ID_W_M3:
				Ptr = (USHORT*)"SGD7W-XXA20A\0\0\0\0";
				break;
			default:
				Ptr = (USHORT*)"SGD7X-XXAXXA\0\0\0\0";
				break;
			}

			break;
		case 0xC009:	/* �T�[�{�e�� */
			Ptr = (USHORT*)"\x00\x32\x00\x00";
			break;
		case 0xC00B:	/* �T�[�{�d�� */
			Ptr = (USHORT*)"\x01\x01";
			break;
		case 0xC001:	/* �p�����[�^�Z�b�gID */
			Ptr = (USHORT*)"\x00\x00\x00\x00\x00\x00\x00\x00";
			break;
		case 0xC007:	/* Y�d�l */
			Ptr = (USHORT*)"\x00\x00";
			break;
		default:
			LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo�����������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = MlibCnvBig16( RegNum * 2 );		/* Data Byte Number							*/
/*--------------------------------------------------------------------------------------------------*/
//		for( i=0; i < RegNum; i++ ){ wResBuf[3+s+i] = MlibCnvBig16( wResBuf[3+s+i] );}
		for( i=0; i < RegNum; i++ )
		{
			wResBuf[3+s+i] = Ptr[i];
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		Length = (RegNum * 2) + 8 + (2*s);				/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, Length );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, Length );				/* Start Transmit Response Message			*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ő僌�W�X�^���Ǐo��																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadMaxNumberOfRegisters( HMSGIF Hmsg )
{
LONG	i;
LONG	MaxNum;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		Check SFC with Slot																			*/
/*--------------------------------------------------------------------------------------------------*/
		i = LPX_GET_SLOT_BYTELEN( Hmsg->CmdBuf[4+(2*s)] );
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Calculate MaxNumber													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Hmsg->CmdBuf[5+(2*s)] )
		{
		case SFC_RD_ONEREG: MaxNum = 1; break;
		case SFC_WR_ONEREG: MaxNum = 1; break;
		case SFC_RD_SEQREG: MaxNum = LpxGetMaxNumberOfRdRegisters( Hmsg, i +  8 ); break;
		case SFC_WR_SEQREG: MaxNum = LpxGetMaxNumberOfWrRegisters( Hmsg, i + 12 ); break;
		case SFC_RD_RANREG: MaxNum = LpxGetMaxNumberOfRdRegisters( Hmsg, i +  8 ); break;
		case SFC_WR_RANREG: MaxNum = LpxGetMaxNumberOfWrRegisters( Hmsg, i + 10 ) >> 1; break;
		case SFC_RD_SEQMEM: MaxNum = LpxGetMaxNumberOfRdRegisters( Hmsg, i +  8 ); break;
		case SFC_WR_SEQMEM: MaxNum = LpxGetMaxNumberOfWrRegisters( Hmsg, i + 12 ); break;
		default: 			MaxNum = 0; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����f�[�^�쐬																				*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* SubFuncCode for MaxNum					*/
		wResBuf[3+s] = MlibCnvBig16( MaxNum );			/* MaxNumber of Registers					*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 10+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�ő�Ǐo�����W�X�^���̎擾																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetMaxNumberOfRdRegisters( HMSGIF Hmsg, LONG FixedLen )
{
		return( ((LONG)Hmsg->SndBufSize - FixedLen) >> 1 );
}



/****************************************************************************************************/
/*																									*/
/*		�ő发���݃��W�X�^���̎擾																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetMaxNumberOfWrRegisters( HMSGIF Hmsg, LONG FixedLen )
{
		return( ((LONG)Hmsg->RcvBufSize - FixedLen) >> 1 );
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ُ퉞���ԐM����																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusSendErrorResponse( HMSGIF Hmsg, ULONG ErrCode )
{
UCHAR	*CmdBuf = (UCHAR *)(Hmsg->CmdBuf);
UCHAR	*ResBuf = (UCHAR *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		�ُ퉞���f�[�^�쐬																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch ( (USHORT)ErrCode )
		{
		case REGIF_CMDERR	: ErrCode = SMBUS_EC_FNCDERR;	break;
		case REGIF_REGNOERR	: ErrCode = SMBUS_SEC_ACCSERR;	break;
		case REGIF_DALMTERR	: ErrCode = SMBUS_SEC_DALMTERR;	break;
		case REGIF_DACALERR	:
		case REGIF_DAMTHERR	: ErrCode = SMBUS_SEC_DAMTHERR;	break;
		case REGIF_DAMTHERR2: ErrCode = SMBUS_SEC_CONDERR;	break;
		case REGIF_DANUMERR	: ErrCode = SMBUS_EC_DANUMERR;	break;
		case REGIF_FOPEERR1	:
		case REGIF_FOPEERR2	:
		case REGIF_FOPEERR3	: ErrCode = SMBUS_SEC_INTLOCK;	break;
		default:			 /* ErrCode = ErrCode; */		break;	/* <S004> */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�ُ퉞���f�[�^�쐬																			*/
/*--------------------------------------------------------------------------------------------------*/
		ResBuf[0] = CmdBuf[0];							/* AxisAddr									*/
		ResBuf[1] = CmdBuf[1] | 0x80;					/* Function Code							*/
		ResBuf[2] = (UCHAR)(ErrCode);					/* Error Code								*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 5 );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 5 );				/* Start Transmit Response Message			*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		���W�X�^�h�e�֐��F���W�X�^�ԍ��ُ�															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxRegifRegNoError( LONG RegNo, LONG Number, USHORT *RwBuf, HMSGIF Hmsg )
{
		Hmsg->ErrRno = (USHORT)RegNo;					/* �ُ탌�W�X�^�ԍ�							*/
		return ( SMBUS_SEC_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
}


/***************************************** end of file **********************************************/
