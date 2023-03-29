/****************************************************************************************************/
/*																									*/
/*																									*/
/*		UsbMsgIf.c : USB function Server	 														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : USB�ʐM�̃��b�Z�[�W����M�̊Ǘ����s���B													*/
/*																									*/
/*			1) �t�r�a�ʐM�w�߃��b�Z�[�W����M������B												*/
/*			2) �t�r�a�ʐM�w�߃��b�Z�[�W��M�m�F�ɂāA��M�����ʒm���s���B							*/
/*			3) �t�r�a�ʐM�������b�Z�[�W���M�J�n�v���ɂāA�����̑��M���J�n����B						*/
/*			4) ����M�Ď��V�[�P���X�ɂāA�V���A���ʐM�f�[�^�̎�M�Ƒ��M�̐�����s���B				*/
/*																									*/
/*																									*/
/*	�� �� : (Jupiter)UsbMsgIf.c + (Ingram)UsbFunctionIF.c => UsbMsgIf								*/
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
/*		Rev.1.00 : 2005.10.17  K.Ando																*/
/*		Rev.1.10 : 2008.05.21  T.Taniguchi															*/
/*		Rev.1.20 : 2013.07.04  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/* Author        : Yaskawa Electric Corp.,															*/
/* Project       : Mercury																			*/
/* Functions	 :																					*/
/*																									*/
/**** APIs ******************************************************************************************/
/* 																									*/
/**** Locals ****************************************************************************************/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"											/* Kernel API */
//<S146>#include "arm.h"
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
#include "UsbFunctionIF.h"
#endif	/*_MERCURY_PLATFORM_*/
#include "usb_f.h"

#ifdef	_MERCURY_PLATFORM_			/*<S190>*/
#ifndef NULL
	#define NULL 0
#endif
const UCHAR	MsgReadSvSerial[] = {0x01,0x40,0x00,0x03,0xc8,0x08,0x00,0x08,0x3a,0x67};
#define	LEN_MSGREADSVSERIAL		sizeof(MsgReadSvSerial)
const UCHAR	ResReadSvSerial[] = {0x01,0x40,0x00,0x03,0x00,0x08,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x7f,0xfa};
#define	LEN_RESREADSVSERIAL		sizeof(ResReadSvSerial)
const UCHAR	MsgReadMaxRegSizeA[] = {0x01,0x40,0x00,0x41,0x00,0x03,0x10,0x10};
#define	LEN_MSGREADMAXREGSIZEA	sizeof(MsgReadMaxRegSizeA)
const UCHAR	ResReadMaxRegSizeA[] = {0x01,0x40,0x00,0x41,0x00,0x03,0x00,0x1b,0x4c,0x07};
#define	LEN_RESREADMAXREGSIZEA	sizeof(ResReadMaxRegSizeA)
const UCHAR	MsgReadMaxRegSizeB[] = {0x01,0x40,0x00,0x41,0x00,0x0d,0x91,0xd4};
#define	LEN_MSGREADMAXREGSIZEB	sizeof(MsgReadMaxRegSizeB)
const UCHAR	ResReadMaxRegSizeB[] = {0x01,0x40,0x00,0x41,0x00,0x0d,0x00,0x1b,0x2d,0xc4};
#define	LEN_RESREADMAXREGSIZEB	sizeof(ResReadMaxRegSizeB)
const UCHAR	MsgReadMaxRegSizeC[] = {0x01,0x40,0x00,0x41,0x00,0x10,0x51,0xdd};
#define	LEN_MSGREADMAXREGSIZEC	sizeof(MsgReadMaxRegSizeC)
const UCHAR	ResReadMaxRegSizeC[] = {0x01,0x40,0x00,0x41,0x00,0x10,0x00,0x15,0x3c,0x06};
#define	LEN_RESREADMAXREGSIZEC	sizeof(ResReadMaxRegSizeC)
const UCHAR	MsgReadMaxRegSizeD[] = {0x01,0x40,0x00,0x41,0x00,0x0e,0xd1,0xd5};
#define	LEN_MSGREADMAXREGSIZED	sizeof(MsgReadMaxRegSizeD)
const UCHAR	ResReadMaxRegSizeD[] = {0x01,0x40,0x00,0x41,0x00,0x0e,0x00,0x17,0xdd,0xc1};
#define	LEN_RESREADMAXREGSIZED	sizeof(ResReadMaxRegSizeD)
#endif	/*_MERCURY_PLATFORM_*/

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Kpx Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitUsbMsgIf(						/* �t�r�a�ʐM���b�Z�[�W�h�e������					*/
		LONG	DriveID,						/* �h���C�u�@��h�c									*/
		LONG	ComAddr,						/* �ʐM�A�h���X										*/
		LONG	AxisNumber						/* �����h���C�u����(0/1/2/...)						*/
		);
/*--------------------------------------------------------------------------------------------------*/
void	KpxSetUsbComAddr(						/* �t�r�a�ʐM�A�h���X�ݒ菈��						*/
		LONG	ComAddr,						/* �ʐM�A�h���X										*/
		LONG	AxisNumber	);					/* �����h���C�u����(0/1/2/...)						*/
/*--------------------------------------------------------------------------------------------------*/
//<S150>�錾�ړ� void	KpxUsbMsgManager( void );				/* �t�r�a�ʐM���b�Z�[�W����M�Ď�					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Kpi Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
void	KriInitUsbHmsg(							/* �t�r�a�ʐMHmsg����������							*/
		HMSGIF	Hmsg		);					/* ���b�Z�[�W�h�e�n���h��(�|�C���^)					*/
/*--------------------------------------------------------------------------------------------------*/
void	KriUsbMsgManager( CSIF *this );			/* �t�r�a�ʐM���b�Z�[�W����M�Ď�					*/
/*--------------------------------------------------------------------------------------------------*/
LONG	KriCheckRxUsbMsg(						/* �t�r�a�ʐM�w�߃��b�Z�[�W��M�`�F�b�N				*/
		struct _CSIF *this,						/* this pointer										*/
		USHORT* pCmdMsgLen	);					/* �w�߃��b�Z�[�W���i�[��|�C���^					*/
/*--------------------------------------------------------------------------------------------------*/
LONG	KriStartTxUsbMsg(						/* �t�r�a�ʐM�������b�Z�[�W���M�J�n�w��				*/
		struct _CSIF *this,						/* this pointer										*/
		UCHAR*	TxBuf,							/* ���M�f�[�^�o�b�t�@								*/
		LONG	TxSize		);					/* ���M�f�[�^�T�C�Y(ByteSize,���������̓[��)		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Local Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
UCHAR	LpxUsbChkRxMessage( CSIF *this );		/* ��M���b�Z�[�W�`�F�b�N							*/

/****************************************************************************************************/
/*																									*/
/*		�t�r�a�n�[�h�E�F�A�h�e�֐��ꗗ																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		����������																					*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		�X�L���������^�����ݏ���																	*/
/*--------------------------------------------------------------------------------------------------*/
//void	DpiUsbConnectSequence( void );			/* �t�r�a�@�ڑ��|�ؒf�����V�[�P���X					*/
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Local Parameter Structure Definition														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {							/*														*/
	USHORT	DriveID;						/* �h���C�u�@��h�c										*/
	UCHAR	ComAddr;						/* �ʐM�A�h���X(����Addr)								*/
	UCHAR	AxisNum;						/* �����h���C�u����										*/
	UCHAR	AxisAddrBgn;					/* �����h���C�u�擪���A�h���X							*/
	UCHAR	AxisAddrEnd;					/* �����h���C�u�ŏI���A�h���X							*/
} MUSBP;
/*--------------------------------------------------------------------------------------------------*/
/*		Local Variables Structure Definition														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {							/*														*/
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
	UCHAR	RxBuf[MSG_MAXBUFSZB];			/* ��M�f�[�^�o�b�t�@									*/
	UCHAR	TxBuf[MSG_MAXBUFSZB];			/* ���M�f�[�^�o�b�t�@									*/
#else	/*_MERCURY_PLATFORM_*/
	UCHAR	RxBuf[512];			/* ��M�f�[�^�o�b�t�@									*/
	UCHAR	TxBuf[512];			/* ���M�f�[�^�o�b�t�@									*/
#endif	/*_MERCURY_PLATFORM_*/
} MUSBV;
/*--------------------------------------------------------------------------------------------------*/
#define	REQSTX_NONE			0x00			/* �w�߂Ȃ�												*/
#define	REQSTX_STARTTX		0x01			/* �������M�J�n�w��										*/
#define	REQSTX_NORESPONSE	0x02			/* �������w��(MsgErr)									*/
/*--------------------------------------------------------------------------------------------------*/
MUSBP	MusbP;								/* Local Parameter for USB Message						*/
MUSBV	MusbV;								/* Local Variables for USB Message						*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
CSIF		USif[USBIF_N_CH];				/* USB Interface Instance								*/
#else	/*_MERCURY_PLATFORM_*/
CSIF		USif[1];				/* USB Interface Instance								*/
#endif	/*_MERCURY_PLATFORM_*/

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Error Code Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	USBERR_RXTMOVER		0x01				/* ��M�^�C���I�[�o									*/
#define	USBERR_USERERR		0x10				/* ���[�U�G���[(������)								*/
#define	USBERR_OTHERERR		0xFF				/* ���̑��G���[										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set Sif Error Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define SET_USBERR( code )	{ this->ErrCod = code; ++(this->ErrCnt);}
/*--------------------------------------------------------------------------------------------------*/
/*		Message Buffer Size Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define USB_CMDBUF_SIZE		(sizeof( MusbV.RxBuf ) - 2)		/* Command  Message Buffer Size			*/
#define USB_RESBUF_SIZE		(sizeof( MusbV.TxBuf ) - 2)		/* Response Message Buffer Size			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		�t�r�a�ʐM���b�Z�[�W�h�e������																*/
/*																									*/
/****************************************************************************************************/
void	KpxInitUsbMsgIf( LONG DriveID, LONG ComAddr, LONG AxisNumber
				)
{
/*--------------------------------------------------------------------------------------------------*/
/*		�t�r�a�ʐM�h�e����������																	*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( &MusbV, sizeof(MusbV)/4 );		/* USB���b�Z�[�W�h�e�p�ϐ�������		*/
/*--------------------------------------------------------------------------------------------------*/
/*		�t�r�a�ʐM�A�h���X�ݒ菈��																	*/
/*--------------------------------------------------------------------------------------------------*/
		MusbP.DriveID = (USHORT)DriveID;					/* Set DriveID							*/
		KpxSetUsbComAddr( ComAddr, AxisNumber );			/* Set ComAddr, AxisNumber				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�t�r�a�n�[�h�h�e����������																	*/
/*--------------------------------------------------------------------------------------------------*/
		UsbFuncInitializeDriver( ComAddr );					/* USB�n�[�h�h�e������					*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	{
CSIF *pSif = &(USif[0]);
		MlibResetLongMemory( pSif, sizeof(CSIF)/4 );		/* USB���b�Z�[�W�h�e�p�ϐ�������		*/
/*--------------------------------------------------------------------------------------------------*/
		pSif->Com = NULL;									/* 										*/
		pSif->AxisNumber = AxisNumber;						/* set axis number						*/
		pSif->DoRxTx = KriUsbMsgManager;					/* RxTx sequence						*/
		pSif->ChkCmd = KriCheckRxUsbMsg;					/* check receive cmd method				*/
		pSif->StxRes = KriStartTxUsbMsg;					/* start tx response method				*/
/*--------------------------------------------------------------------------------------------------*/
		pSif->RcvBufSize = USB_CMDBUF_SIZE;					/* rcv buf size */
		pSif->SndBufSize = USB_RESBUF_SIZE;					/* send buf size */
/*--------------------------------------------------------------------------------------------------*/
		pSif->RxBuf = MusbV.RxBuf;							/* set receive buffer					*/
		pSif->TxBuf = MusbV.TxBuf;							/* set send buffer						*/
	}
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�t�r�a�ʐM�A�h���X�ݒ菈��																	*/
/*																									*/
/****************************************************************************************************/
void	KpxSetUsbComAddr( LONG ComAddr, LONG AxisNumber )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�t�r�a�ʐM�h�e�p�����[�^������																*/
/*--------------------------------------------------------------------------------------------------*/
		MusbP.ComAddr = (UCHAR)ComAddr;						/* Com Address							*/
		MusbP.AxisNum = (UCHAR)AxisNumber;					/* Multi-Axis Number					*/
/*--------------------------------------------------------------------------------------------------*/
		if( AxisNumber < 0 )								/* Disable USB							*/
		{
			MusbP.AxisAddrBgn = 0xFF;						/* Axis Addr Begin						*/
			MusbP.AxisAddrEnd = 0x00;						/* Axis Addr End						*/
		}
		else if( AxisNumber <= 0 )							/* For SGDV								*/
		{
			MusbP.AxisAddrBgn = (UCHAR)ComAddr;				/* Axis Addr Begin						*/
			MusbP.AxisAddrEnd = (UCHAR)ComAddr;				/* Axis Addr End						*/
		}
		else												/* For Jupiter							*/
		{
			MusbP.AxisAddrBgn = 1;							/* Axis Addr Begin						*/
			MusbP.AxisAddrEnd = (UCHAR)(AxisNumber);		/* Axis Addr End						*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�t�r�a�ʐMHmsg����������																	*/
/*																									*/
/****************************************************************************************************/
void	KriInitUsbHmsg( HMSGIF Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Init Hmsg Data																				*/
/*--------------------------------------------------------------------------------------------------*/
//		Hmsg->CmdBuf = MusbV.RxBuf;
//		Hmsg->ResBuf = MusbV.TxBuf;
//		Hmsg->CmdBufSize = USB_CMDBUF_SIZE;
//		Hmsg->ResBufSize = USB_RESBUF_SIZE;
//		Hmsg->ChkCmdMsg  = KriCheckRxUsbMsg;
//		Hmsg->StxResMsg  = KriStartTxUsbMsg;
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�t�r�a�ʐM���b�Z�[�W������Ď����� ( ScanC Cycle )											*/
/*																									*/
/****************************************************************************************************/
void	KpxUsbMsgManager( void )
{
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
	if( USB0_PORT_USE == 1) { USif[COM0_USB].DoRxTx( &(USif[COM0_USB]) ); /* USB ch:0 */ }
#else	/*_MERCURY_PLATFORM_*/
	USif[0].DoRxTx( &(USif[0]) ); /* USB ch:0 */
#endif	/*_MERCURY_PLATFORM_*/
	return;
}

enum	SQSTEP	{
		SQSTEP_INITIAL		= 0,
		SQSTEP_CHK_USB		,
		SQSTEP_START_RX		,
		SQSTEP_WAIT_SRX		,
		SQSTEP_WAIT_ERX		,
		SQSTEP_WAIT_STX		,
		SQSTEP_START_TX		,
		SQSTEP_WAIT_ETX		,
/*------------------------------------------*/
		SQSTEP_USBRXERR,
};
/*--------------------------------------------------------------------------------------------------*/
void	KriUsbMsgManager( CSIF *this )
{
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�����������`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
//		if( MusbP.AxisAddrBgn > MusbP.AxisAddrEnd )
//		{
//			if( KsysV.MainTimer < 20000 ){ return;}			/* 20sec�o�ߌ�́ADownload�Ή�			*/
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		�t�r�a�n�[�h�E�F�A�h�e����																	*/
/*--------------------------------------------------------------------------------------------------*/
//		DpiUsbConnectSequence( );							/* USB�o�X�ڑ��|�ؒf�V�[�P���X			*/
//		UsbFuncIntIntu2f();									/* �v���O�C���^�v���O�A�E�g����			*/
//		UsbFuncIntIntu2fepc();								/* �o�X�E���Z�b�g�^�o�X�E�X�s�[�h�ύX	*/
															/* EP0 SETUP���o����					*/

		UsbFuncPollBulkOutIn( );							/* USB�h���C�o�o���N�]���T�[�r�X		*/
/*--------------------------------------------------------------------------------------------------*/
/*		�t�r�a���b�Z�[�W����M�Ď��V�[�P���X														*/
/*--------------------------------------------------------------------------------------------------*/
		switch( this->SqStep )
		{
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_INITIAL:		/* Initial Step													*/
	/*----------------------------------------------------------------------------------------------*/
			 UsbFuncResetBuffer( );								/* Endpoint1/2 ����M�o�b�t�@�N���A	*/
			 this->SqStep = SQSTEP_CHK_USB;
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_CHK_USB:		/* Check USB Port												*/
	/*----------------------------------------------------------------------------------------------*/
			 if( UsbFuncPortConfiguration() == TRUE )			/* USB�|�[�g��ԃ`�F�b�N			*/
			 {
				this->SqStep = SQSTEP_START_RX;					/* ��M�J�nSTEP�ɑJ��				*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_START_RX:		/* Start Receive Message										*/
	/*----------------------------------------------------------------------------------------------*/
			 if( UsbFuncPortConfiguration() == FALSE )			/* USB�|�[�g��ԃ`�F�b�N			*/
			 {
				this->SqStep = SQSTEP_INITIAL;					/* �������Ƽ�قɖ߂�				*/
			 }
			 else if( UsbFuncStartBkout( this->RxBuf, USB_CMDBUF_SIZE ) == TRUE )	/* ��M�J�n		*/
			 {
				this->SqStep = SQSTEP_WAIT_SRX;					/* ��M�J�n�҂�STEP�ɑJ��			*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_WAIT_SRX:		/* Wait Start of Receive Message								*/
	/*----------------------------------------------------------------------------------------------*/
			 if( UsbFuncPortConfiguration() == FALSE )			/* USB�|�[�g��ԃ`�F�b�N			*/
			 {
				this->SqStep = SQSTEP_INITIAL;					/* �������Ƽ�قɖ߂�				*/
			 }
			 else if( UsbFuncChkRcvComp() == BLKTRNS_END )		/* �S�f�[�^��M�����H				*/
			 {
				this->SqStep = LpxUsbChkRxMessage( this );		/* ��M���b�Z�[�W�`�F�b�N			*/
			 }
			 else if( UsbFuncGetRcvSize() > 0 )					/* ��M�J�n�H						*/
			 {
				KpiRstLongTimer( &this->RxTimer );				/* ��M�Ď��^�C�}���Z�b�g			*/
				this->SqStep = SQSTEP_WAIT_ERX;					/* ��M�����҂�STEP�ɑJ��			*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_WAIT_ERX:		/* Wait End of Receive Message									*/
	/*----------------------------------------------------------------------------------------------*/
			 if( UsbFuncPortConfiguration() == FALSE )			/* USB�|�[�g��ԃ`�F�b�N			*/
			 {
				this->SqStep = SQSTEP_INITIAL;					/* �������Ƽ�قɖ߂�				*/
			 }
			 else if( UsbFuncChkRcvComp() == BLKTRNS_END )		/* �S�f�[�^��M�����H				*/
			 {
				this->SqStep = LpxUsbChkRxMessage( this );		/* ��M���b�Z�[�W�`�F�b�N			*/
			 }
			 else if( KpiGetLongTimer(&this->RxTimer) >= 500 )	/* ��M�Ď��^�C���I�[�o				*/
			 {
				this->SqStep = SQSTEP_INITIAL;					/* �ُ펞INITIAL-STEP�ɑJ��			*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_WAIT_STX:		/* Wait Start Transmit											*/
	/*----------------------------------------------------------------------------------------------*/
			 if( this->ReqStx == REQSTX_STARTTX )				/* ���M�J�n�v���`�F�b�N				*/
			 {
				this->ReqStx = 0x00;							/* Reset ReqStx						*/
				this->SqStep = SQSTEP_START_TX;					/* ���M�J�n����STEP�ɑJ��			*/
			 }
			 else if( this->ReqStx == REQSTX_NORESPONSE )		/* ������(MsgErr)�v���`�F�b�N		*/
			 {
				this->ReqStx = 0x00;							/* Reset ReqStx						*/
//				SET_USBERR( USBERR_USERERR );					/* Set User Error					*/
//				this->SqStep = SQSTEP_START_TX;					/* ���M�J�n����STEP�ɑJ��			*/
				this->SqStep = SQSTEP_INITIAL;					/* INITIAL-STEP�ɑJ��			*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_START_TX:		/* Start Transmit Message										*/
	/*----------------------------------------------------------------------------------------------*/
			 if( UsbFuncPortConfiguration() == FALSE )			/* USB�|�[�g��ԃ`�F�b�N			*/
			 {
				this->SqStep = SQSTEP_INITIAL;					/* �������Ƽ�قɖ߂�				*/
			 }
			 else if( UsbFuncStartBkin( this->TxBuf, this->TxResLen ) == TRUE )		/* ���M�J�n		*/
			 {
				this->SqStep = SQSTEP_WAIT_ETX;					/* ���M�����҂�STEP�֑J��			*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		case SQSTEP_WAIT_ETX:		/* Wait End of Transmit Message									*/
	/*----------------------------------------------------------------------------------------------*/
			 if( UsbFuncPortConfiguration() == FALSE )			/* USB�|�[�g��ԃ`�F�b�N			*/
			 {
				this->SqStep = SQSTEP_INITIAL;					/* �������Ƽ�قɖ߂�				*/
			 }
			 else if( UsbFuncChkTrnsComp() == BLKTRNS_END )		/* �S�f�[�^���M�����⍇��			*/
			 {
				this->SqStep = SQSTEP_START_RX;					/* ��M�J�nSTEP�ɑJ��				*/
			 }
			 break;
	/*----------------------------------------------------------------------------------------------*/
		default:					/* Not Used														*/
	/*----------------------------------------------------------------------------------------------*/
			 break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�t�r�a��M���b�Z�[�W�`�F�b�N																*/
/*																									*/
/****************************************************************************************************/
UCHAR	LpxUsbChkRxMessage( CSIF *this )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�X���[�u�A�h���X�̃`�F�b�N 																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->RxBuf[0] == 0x00 )
		{
			;											/* OK										*/
		}
		else if( this->RxBuf[0] == MusbP.ComAddr )
		{
			;											/* OK										*/
		}
		else if( this->RxBuf[0] < MusbP.AxisAddrBgn )
		{
			return( SQSTEP_INITIAL );					/* ��M�J�n����Step�ɖ߂�B					*/
		}
		else if( this->RxBuf[0] > MusbP.AxisAddrEnd )
		{
			return( SQSTEP_INITIAL );					/* ��M�J�n����Step�ɖ߂�B					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�t�@���N�V�����R�[�h�̃`�F�b�N 																*/
/*--------------------------------------------------------------------------------------------------*/
		if( 1 )											/* ����										*/
		{
			this->RxCmdLen = UsbFuncGetRcvSize( );		/* ��M�����o�C�g���𓾂�					*/
			return(	SQSTEP_WAIT_STX );					/* ���M�J�n�҂�Step�ɑJ��					*/
		}
/*--------------------------------------------------------------------------------------------------*/
//		return( SQSTEP_INITIAL );						/* ��L�ȊO�́A�S�ď���Step�ɖ߂�			*/
}


/****************************************************************************************************/
/*																									*/
/*		�t�r�a�ʐM�w�߃��b�Z�[�W��M�`�F�b�N														*/
/*																									*/
/****************************************************************************************************/
LONG	KriCheckRxUsbMsg( 
		struct _CSIF *this,						/* this pointer										*/
		USHORT* pCmdMsgLen )					/* pointer to the message length					*/
{
	if( UsbFuncChkAvailable() == FALSE )
	{
		return( FALSE );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		��M������																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->SqStep == SQSTEP_WAIT_STX )
		{
			*pCmdMsgLen = this->RxCmdLen;
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		��M������																					*/
/*--------------------------------------------------------------------------------------------------*/
	return( FALSE );
}

/****************************************************************************************************/
/*																									*/
/*		�t�r�a�ʐM�������b�Z�[�W���M�J�n�w��														*/
/*																									*/
/****************************************************************************************************/
LONG	KriStartTxUsbMsg(							/* �������b�Z�[�W���M�J�n�w�ߏ���				*/
		struct _CSIF *this,						/* this pointer										*/
		UCHAR*	TxBuf,								/* ���M�f�[�^�o�b�t�@							*/
		LONG	TxSize )							/* ���M�f�[�^�T�C�Y(ByteSize,���������̓[��)	*/
{
	if( UsbFuncChkAvailable() == FALSE )
	{
		return( FALSE );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�������b�Z�[�W�^�C�v�̃`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->SqStep != SQSTEP_WAIT_STX )		/* �V�[�P���X�`�F�b�N							*/
		{
			return( FALSE );						/* �ُ탊�^�[��									*/
		}
		else if( TxSize > 0 )						/* ���M�f�[�^�T�C�Y�`�F�b�N						*/
		{
			this->ReqStx   = REQSTX_STARTTX;		/* ���M�J�n���N�G�X�g�ݒ�						*/
			this->pTxRes   = TxBuf;					/* �����o�b�t�@�A�h���X�ݒ�						*/
			this->TxResLen = (USHORT)TxSize;		/* �������b�Z�[�W��(Byte��)�ݒ�					*/
			return( TRUE );
		}
		else
		{
			this->ReqStx   = REQSTX_NORESPONSE;		/* ���������N�G�X�g�ݒ�							*/
			this->pTxRes   = NULL;					/* �����o�b�t�@�A�h���X�ݒ�						*/
			this->TxResLen = 0;						/* �������b�Z�[�W��(Byte��)�ݒ�					*/
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
}



/****************************************************************************************************/
/*																									*/
/*																									*/
/*		�t�r�a�n�[�h�E�F�A�h�e����																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
// �ʃt�@�C��










/**************************** Start of Local Functions **********************************************/

/***************************************** end of file **********************************************/
