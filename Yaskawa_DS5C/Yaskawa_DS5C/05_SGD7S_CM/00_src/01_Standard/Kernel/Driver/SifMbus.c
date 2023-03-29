/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SifMbus.c : �V���A���ʐM�C���^�[�t�F�C�X													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A���ʐM�̑���M���s���B															*/
/*																									*/
/*			1) �V���A���ʐM�w�߃��b�Z�[�W����M���A��M�����ʒm���s���B								*/
/*			2) �V���A���ʐM�������b�Z�[�W�̑��M�J�n�v�����󂯁A���M���J�n����B						*/
/*			3) ����M�Ď��V�[�P���X�ɂāA�V���A���ʐM�f�[�^�̎�M�Ƒ��M�̐�����s���B				*/
/*			4) ���b�Z�[�W�̃w�b�_��͂́AMsgMngr.c�ɂčs���悤�ɕύX�B								*/
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
/*																									*/
/*		Rev.2.xx : 2013.4.23	K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"
#include	"MsgIfDef.h"
#include	"UartMsgIF.h" /* Zantei */
#include 	"FirmDownload.h"		/* <S041> */

#define SIF_DEBUG (0)		/* 1:Debug Mode */

/****************************************************************************************************/
/*																									*/
/*		�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�j�����֐���`�F�֐����ύX�s��																*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxSifMsgManager( void );			/* �V���A���ʐM���b�Z�[�W����M�Ď�						*/
void	KpxInitSifMsgManager( 				/* �V���A���ʐM���b�Z�[�W�Ǘ�������						*/
				LONG ch,					/* UART channel number (0,1,2)							*/
				CSIFCOMPORT *Com,			/* UART Com port 										*/
				LONG Axis);					/* Axis Number											*/
//<S150>				void *pUartMbusServer);		/* Zantei USB���l��ŏ���								*/
extern LONG RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen );	/* <S150> */

/*--------------------------------------------------------------------------------------------------*/
/*		�k�����֐���`�F�֐����ύX���R																*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxSifStartRxDMA( CSIF *this );		/* �r�h�e��M�c�l�`�X�^�[�g����				*/
void	LpxSifStopxRxDMA( CSIF *this );		/* �r�h�e��M�c�l�`�X�g�b�v����							*/
void	LpxSifStartTxDMA( CSIF *this );		/* �r�h�e���M�c�l�`�X�^�[�g����							*/
/*--------------------------------------------------------------------------------------------------*/
//LONG	LpxSifChkRxHeader( CSIF *this );	/* Check receive message header							*/
//LONG	LpxSifChkRxMsgEnd( CSIF *this );	/* Check receive message end							*/
LONG	LpxSifMbusChkMyMsg( CSIF *this );	/* Check receive message address						*/
#if 0	/* RkiMsgifGetCmdMsgLength���g�p���� <S150> */
LONG	LpxSifGetCmdMsgLength(				/* Check recieve message length							*/
				UCHAR *CmdBuf,
				LONG CmdBufLen );
#endif
/*--------------------------------------------------------------------------------------------------*/
//LONG	LpxSifCheckRxDMA( CSIF *this );		/* �c�l�`��M�m�F										*//* @@DMA�s�g�p */
LONG	LpxSifCheckRxData( CSIF *this,
						   LONG mode );		/* UART��M�m�F											*/
LONG	LpxSifSetTxData( CSIF *this );		/* UART���M����											*/

/*--------------------------------------------------------------------------------------------------*/
/* 		Serial interface function																	*/
/*		(�����A�߂�l�ύX�s��																		*/
/*--------------------------------------------------------------------------------------------------*/
void	LpiSifMsgManager( CSIF *this );		/* �V���A���ʐM���b�Z�[�W����M�Ď�						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpiCheckRxSerialMsg( 				/* �V���A���ʐM�w�߃��b�Z�[�W��M�`�F�b�N				*/
		CSIF *this,							/* serial I/F class pointer								*/
		USHORT *pCmdMsgLen );				/* �w�߃��b�Z�[�W���i�[��|�C���^						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpiStartTxSerialMsg(				/* �V���A���ʐM�������b�Z�[�W���M�J�n�w��				*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* ���M�f�[�^�o�b�t�@									*/
		LONG	TxSize		);				/* ���M�f�[�^�T�C�Y(ByteSize,���������̓[��)			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpiStartTxSerialMsgNoResponse(		/* �V���A���ʐM�������b�Z�[�W���M�J�n�w��(�������p)		*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* ���M�f�[�^�o�b�t�@									*/
		LONG	TxSize		);				/* ���M�f�[�^�T�C�Y(ByteSize,���������̓[��)			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		�ϐ���`																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
CSIF	LSif[SIF_N_CH];						/* Serial Communication Management IF Class				*/
/*--------------------------------------------------------------------------------------------------*/
/* <S150> >>>>> */
typedef	struct {							/*														*/
	UCHAR	RxBuf[MSG_MAXBUFSZB];			/* ��M�f�[�^�o�b�t�@									*/
	UCHAR	TxBuf[MSG_MAXBUFSZB];			/* ���M�f�[�^�o�b�t�@									*/
} MSIFV;
/*--------------------------------------------------------------------------------------------------*/
MSIFV	MSifV[SIF_N_CH];					/* Local Variables for Serial Message				*/
/* <<<<< <S150> */
/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#undef RESMT_COMPLETE
#define	RESMT_COMPLETE		(0x01)			/* MsgType : ��������									*/
#undef RESMT_NORESPONSE
#define	RESMT_NORESPONSE	(0x00)			/* MsgType : ������(���b�Z�[�W�ُ�)						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIFREQ_STARTTX		(0x01)			/* ���M�J�n�w��											*/
#define	SIFREQ_NOANSWER		(0x02)			/* �������w��											*/
/*--------------------------------------------------------------------------------------------------*/
#define	MSG_ERROR			(0x00)			/* ���b�Z�[�W�ُ�				for MEMOBUS				*/
#define	LENGTH_FIX			(0x01)			/* ��M���b�Z�[�W���m��			for MEMOBUS				*/
#define	LENGTH_NOFIX		(0x02)			/* ��M���b�Z�[�W�����m��		for MEMOBUS				*/

#define	DIGOP_MINSIZE		(6)				/* �f�W�I�y���b�Z�[�W�ŏ��o�C�g��						*/
#define	DIGOP_MAXSIZE		(256)			/* �f�W�I�y���b�Z�[�W�ő�o�C�g��						*/
#define	MBUS_MINSIZE		(8)				/* MEMOBUS���b�Z�[�W�ŏ��o�C�g��						*/
#define	MBUS_MAXSIZE		(256)			/* MEMOBUS���b�Z�[�W�ő�o�C�g��						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF_GETHEADER		(0)				/* UART�f�[�^��M �w�b�_�[���擾���[�h(�擪2Byte�ȏ�)	*/
#define	SIF_GETALLDATA		(1)				/* UART�f�[�^��M �S�f�[�^�擾���[�h					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF_RX_SUCCESS		(1)				/* UART���b�Z�[�W��M����								*/
#define	SIF_RX_CONTINUE		(0)				/* UART���b�Z�[�W��M�p��								*/
#define	SIF_RX_ERROREND		(-1)			/* UART���b�Z�[�W�ُ�I��								*/
/*--------------------------------------------------------------------------------------------------*/
const LONG DefBaudRate[8] = {               /* Serial Communication Baudrate Definition             */
	0x00002580,     /*   9600 bps */
	0x00004b00,     /*  19200 bps */
	0x00009600,     /*  38400 bps */
	0x0000E100,     /*  57600 bps(reserved) */
	0x0001C200,     /* 115200 bps(reserved) */
	0x0001F400,     /* 128000 bps(reserved) */
	0x0003D090,     /* 250000 bps(reserved) */
	0x000F4240,     /*1000000 bps(reserved) */
};
#define LPX_CHK_BRTCHG( com, bidx )          /* check baurate parameter */                           \
		( DefBaudRate[ (bidx) ] != ((com)->BRT) )  
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�r�h�e�G���[��`																			*/
/*																									*/
/****************************************************************************************************/
#define SET_SIFERR( csif, code )	{ (csif)->ErrCod = code; ++((csif)->ErrCnt);}
/*--------------------------------------------------------------------------------------------------*/
/*		Error Code Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIFERR_RXTMOVR		0x01			/* �`������M�^�C���I�[�o								*/
#define SIFERR_RXDMAERR 	0x02			/* ��MDMA�G���[										*/
#define	SIFERR_RXHDRERR		0x03			/* ��M�w�b�_�G���[										*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIFERR_USERERR		0x10			/* ���[�U�G���[(������)									*/
#define	SIFERR_OTHERERR		0xFF			/* ���̑��G���[											*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		Timer Value Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define SIF_TXWAITTIME		  20			/* ��M��������̑��M�J�n�҂����� [ms]					*/
#define	SIF_RXTIMEOVER		 100			/* �`������M�^�C���I�[�o�ݒ�l   [ms]					*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		�r�h�e���c�l�`�C���^�[�t�F�C�X�}�N����`													*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		DMA Receive Size Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	DMA_RXSIZE			(SIF_RX_BUFSIZE - 1)  		/* DMA Receive RxBuf  Size					*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_RXDMA_CNT( Com ) ( *(Com->RxDMA_CNT) )
/*--------------------------------------------------------------------------------------------------*/
#define LPX_V850_BUG1( ) {;} 
/*--------------------------------------------------------------------------------------------------*/
#define LPX_TXCHKF(Com)  (((*((Com)->TXCHKF))&0x040))	/* Check TX FIFO EMPTY						*/
#define LPX_TXCHKL(Com)  (((*((Com)->TXCHKF))&0x020))	/* Check TX LATCH EMPTY						*/
#define LPX_CHK_RXRDY( com ) ((*((com)->RXCHKF)) & 0x01)

/****************************************************************************************************/
/*																									*/
/*		�V���A���ʐM���b�Z�[�W����������															*/
/*																									*/
/****************************************************************************************************/
void	KpxInitSifMsgManager( LONG ch, CSIFCOMPORT *Com, LONG Axis
//<S150>			,void *pUartMbusServer				/* zantei */
		)
{
CSIF *pSif = &(LSif[ch]);										/* get serial IF pointer */
/*--------------------------------------------------------------------------------------------------*/
/*  DMA��~(�O�̂���)																				*/
/*--------------------------------------------------------------------------------------------------*/
	Com->RXDISABLE( );											/* Disable SIF Receive				*/
	Com->STOP_RxDMA( );											/* Stop SIF RxDMA					*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( pSif, sizeof( CSIF )/4 );				/* reset data	*/
/*--------------------------------------------------------------------------------------------------*/
	pSif->Com = Com; 											/* set com port */
	pSif->AxisNumber = Axis;									/* set axis number */
	pSif->DoRxTx = LpiSifMsgManager;							/* RxTx sequence */
	pSif->ChkCmd = LpiCheckRxSerialMsg;							/* check receive cmd method */
	pSif->StxRes = LpiStartTxSerialMsg;							/* start tx response method */
/*--------------------------------------------------------------------------------------------------*/
	pSif->RcvBufSize = SIF_RX_BUFSIZE;							/* rcv buf size */
	pSif->SndBufSize = SIF_TX_BUFSIZE;							/* send buf size */
/*--------------------------------------------------------------------------------------------------*/
//	pSif->pBrtPrm = &(Iprm.SifCfg[ ch ].BaudRateSel);           /* TODO:set baudrate parameter pointer   */
/*--------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------*/
	/*	�ȉ��b��SigmaWin+�Ή� ������42H�R�}���h�ɑΉ����邱��										*/
	/*----------------------------------------------------------------------------------------------*/
/* <S150> >>>>> */
//	pSif->ComMbus42 = pUartMbusServer;							/* TODO: Zantei(USB���l��ŏ���)	*/
////	((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.RxBuf = pSif->RxBuf;	/* <S01C> */
////	((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.TxBuf = pSif->TxBuf;	/* <S01C> */
//	pSif->RxBuf = ((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.RxBuf;	/* <S01C> */
//	pSif->TxBuf = ((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.TxBuf;	/* <S01C> */
	pSif->RxBuf = MSifV[ch].RxBuf;								/* rcv buf */
	pSif->TxBuf = MSifV[ch].TxBuf;								/* send buf */
/* <<<<< <S150> */

	return;
}

/****************************************************************************************************/
/*																									*/
/*		�V���A���ʐM���b�Z�[�W������Ď����� ( ScanC Cycle )										*/
/*																									*/
/****************************************************************************************************/
void	KpxSifMsgManager( void )
{
	if( UART0_PORT_USE == 1) { LSif[COM0_UART].DoRxTx( &(LSif[COM0_UART]) ); /* UART ch:0 */ }
	if( UART1_PORT_USE == 1) { LSif[COM1_UART].DoRxTx( &(LSif[COM1_UART]) ); /* UART ch:1 */ }
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�V���A���ʐM���b�Z�[�W����M�Ď��V�[�P���X													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ����M�Ď��V�[�P���X�ɂ��A�V���A���ʐM�f�[�^�̎�M�Ƒ��M�̐�����s���B				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			0) ����STEP		: �ϐ��̃��Z�b�g���s���A��M�J�n����STEP�ɑJ�ڂ���B					*/
/*			1) ��M�J�n���� : �r�h�e��M�c�l�`���X�^�[�g���A��M�҂�STEP�ɑJ�ڂ���B				*/
/*			2) ��M�҂�		: �r�h�e��M�c�l�`���Ď����A1byte�ȏ��M�����ꍇ�́A					*/
/*							  1byte��MSTEP�ɑJ�ڂ���B												*/
/*			3) 1byte��M    : �r�h�e��M�c�l�`���Ď����A4byte�ȏ��M�����ꍇ�́A					*/
/*							  4byte��MSTEP�ɑJ�ڂ���B												*/
/*							  �{STEP����A�`������M�^�C���I�[�o���Ď�����B						*/
/*			4) 4byte��M    : �r�h�e��M�c�l�`���Ď����A�S���b�Z�[�W����M�����ꍇ��				*/
/*							  (�w�b�_���̃��b�Z�[�W���Ŕ��f)�A��M��������STEP�ɑJ�ڂ���B			*/
/*							  �{STEP�܂ŁA�`������M�^�C���I�[�o���Ď�����B						*/
/*			5) ��M��������	: �r�h�e��M�c�l�`���X�g�b�v����B										*/
/*							  �p���`�����[�h�ł���΁A�p���`������STEP�ɑJ�ڂ���B					*/
/*							  �ʏ�`�����[�h�ł���΁A��M���b�Z�[�W�̃`�F�b�N���s���A				*/
/*							  ���ǈ��Ẵ��b�Z�[�W�ł���΁A��M�����X�e�[�^�X���Z�b�g���A			*/
/*							  ���M�J�n�҂�STEP�ɑJ�ڂ���B											*/
/*							  ���ǈ��Ẵ��b�Z�[�W�łȂ���΁A��M�J�n����STEP�ɑJ�ڂ���B			*/
/*			6) ���M�J�n�҂�	: �A�v������̑��M�J�n�v���ɂ�著�M�J�n����STEP�ɑJ�ڂ���B			*/
/*							  �Ȃ��A�A�v������̑��M�J�n�v���͖{STEP�ɂ���ꍇ�̂ݎ󂯕t����B		*/
/*							  �p���`�����[�h�ڍs�`�F�b�N���ԓ��ɁA�A�v������̑��M�J�n�v��			*/
/*							  ���Ȃ��ꍇ�́A�p���`�����[�h�Ƃ��A�p���������b�Z�[�W���쐬���A		*/
/*							  ���M�J�n����STEP�ɑJ�ڂ���B											*/
/*			7) ���M�J�n���� : �r�h�e���M�c�l�`�Ƃr�h�e��M�c�l�`���X�^�[�g���A						*/
/*							  ���M�����҂�STEP�ɑJ�ڂ���B											*/
/*			8) ���M�����҂� : �r�h�e���M�c�l�`�̑��M�������Ď����A�c�l�`���M�����ŁA				*/
/*							  ���M�����m�FSTEP�ɑJ�ڂ���B											*/
/*			9) ���M�����m�F	: �r�h�e�̑��M�������Ď����A�r�h�e���M�����ŁA���M��������STEP 			*/
/*							  �ɑJ�ڂ���B															*/
/*		   10) ���M��������	: �r�h�e�𑗐MDisable�Ƃ��A��M�҂�STEP�ɑJ�ڂ���B						*/
/*																									*/
/*			*) �p���`������ : �p���`�����[�h���̊����m�F���b�Z�[�W�ɑ΂��鉞���������s���B			*/
/*							  �Ȃ��A�������b�Z�[�W�쐬��A���M�J�n����STEP�ɑJ�ڂ���B				*/
/*																									*/
/*			*) �e��ُ폈�� : ��M�^�C���I�[�o���ُ̈폈�����s���B									*/
/*							  �ُ퉞����ԐM����ꍇ�́A���M�J�n����STEP�ɑJ�ڂ���B				*/
/*							  �ُ퉞����ԐM���Ȃ��ꍇ(SumCheckErr�Ȃ�)�́A����STEP�ɖ߂�B			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
enum	SQSTEP	{
	SQSTEP_INITIAL = 0,
	SQSTEP_STARTRX,
	SQSTEP_WAITRX,
	SQSTEP_CHKHEADER,
	SQSTEP_RXCMPLT,
	SQSTEP_WAITSTX,
	SQSTEP_STARTTX,
	SQSTEP_WAITETX,
	SQSTEP_CHKASIF,
	SQSTEP_TXCMPLT,
/*--------------------------------------------------------------------------------------------------*/
	SQSTEP_SIFRXERR,
};
/*--------------------------------------------------------------------------------------------------*/
void	LpiSifMsgManager( CSIF *this )
{
USHORT	timer;
LONG	rc;
ULONG	UartSpeed;				/*+	UART�]�����x(bps)	�@<S041> */
LONG	DataAddr;				/*+	���M�f�[�^�A�h���X <S041>	*/
UCHAR	*pTxBuf = this->pTxRes;	/*+	���M�f�[�^�|�C���^�[�擪 <S041>	*/
register CSIFCOMPORT *Com = (CSIFCOMPORT *)this->Com;
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM����M�Ď��V�[�P���X 															*/
/*--------------------------------------------------------------------------------------------------*/
		switch( this->SqStep )
		{
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_INITIAL: 	/* Initial Step														*/
/*--------------------------------------------------------------------------------------------------*/
			Com->RSTRXSTS( );										/* reset recv. status			*/
//TODO		Com->SETBRT( Com, DefBaudRate[ *(this->pBrtPrm) ] );	/* set baudrate					*/
			Com->RESET( );											/* reset serial					*/
/*--------------------------------------------------------------------------------------------------*/
			this->ReqStx = 0x00;									/* reset Tx request				*/
			this->SqStep = SQSTEP_STARTRX;							/* start Rx						*/
/*--------------------------------------------------------------------------------------------------*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_STARTRX: 	/* Start Receive Message											*/
/*--------------------------------------------------------------------------------------------------*/
//			LpxSifStartRxDMA( this );								/* ��M�c�l�`�X�^�[�g			*//* @@DMA�s�g�p */
			Com->RXENABLE( );										/* Enable SIF Receive			*/
			this->SqStep = SQSTEP_WAITRX;							/* ��M�҂�STEP�ɑJ��			*/
			this->RxCmdCnt = 0;										/* ��M�f�[�^���N���A			*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_WAITRX:		/* Wait Receive Message												*/
/*--------------------------------------------------------------------------------------------------*/
			if( Com->CHK_RXERR() )                  				/* Check Serial Com. Error		*/
			{ /* OverRun, Parity, Framing Error */
				Com->RXDISABLE( );									/* Disable SIF Receive			*/
//				Com->STOP_RxDMA( );									/* Stop SIF RxDMA				*//* @@DMA�s�g�p */
				this->SqStep = SQSTEP_INITIAL;						/* Goto Initial Step			*/
				break;
			}
			else
			{ /* Communication Error is Notiong */
				rc = LpxSifCheckRxData( this, SIF_GETHEADER );		/* Check Receive data			*/
				if( rc == SIF_RX_SUCCESS )
				{ /* Get Header Data */
					this->SqStep = LpxSifMbusChkMyMsg( this );	/* TODO: ��M���b�Z�[�W�̃`�F�b�N����	*/
//					this->SqStep = SQSTEP_CHKHEADER;				/* Check the Header Data		*/
//					this->RxCmdCnt = rc;							/* Set Rx Count					*/
//					this->SqStep = SQSTEP_RXCMPLT;					/* Receive Complete				*/
				}
				else if( rc == SIF_RX_ERROREND )
				{ /* Error End */
					Com->RXDISABLE( );								/* Disable SIF Receive			*/
					this->SqStep = SQSTEP_INITIAL;					/* Goto Initial Step			*/
				}
				else /* SIF_RX_CONTINUE */
				{
//					if(	SysDl.DlStatus == 0x0001 )	/* <S041> */
					if(	SYSDL_CHK_DLMODE_ON() )	/* <S077> */
					{	/* Bps Time Out Check 10sec	*/
						if( KpiGetLongTimer( &(SysDl.BpsChkTime) ) > 10000 )
						{	/*	�_�E�����[�h�J�n��Ԃ���10�b�Ԏ��֐i�܂Ȃ��ꍇ19200bps�֖߂�	*/
							if(SysDl.NowBps != 19200)
							{	/* ���݂̂a�o�r�l��19200bps�ȊO�̂� */
								KpxInitCpuSerialIF( COM0_UART, 19200, 8, 2, 1);
							}
							KpiRstLongTimer( &(SysDl.BpsChkTime) );	/* Reset BpsChkTimer			*/
							SYSDL_DLMODE_RESET();					/* DownLoad mode stop <S077>	*/
						}
					}
					; /* CONTINUE */
				}
			}
			break;
#if 0
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_CHKHEADER:	/* receive complete													*/
/*--------------------------------------------------------------------------------------------------*/
			this->SqStep = LpxSifMbusChkMyMsg( this );	/* TODO: ��M���b�Z�[�W�̃`�F�b�N����		*/
			break;
#endif
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_RXCMPLT:	/* receive complete													*/
/*--------------------------------------------------------------------------------------------------*/
			if( Com->CHK_RXERR() )                  				/* Check Serial Com. Error		*/
			{ /* OverRun, Parity, Framing Error */
				Com->RXDISABLE( );									/* Disable SIF Receive			*/
//				Com->STOP_RxDMA( );									/* Stop SIF RxDMA				*//* @@DMA�s�g�p */
				this->SqStep = SQSTEP_INITIAL;						/* Goto Initial Step			*/
//				break;
			}
			else
			{ /* Communication Error is Notiong */
				rc = LpxSifCheckRxData( this, SIF_GETALLDATA );		/* Check Receive data			*/
				if( rc == SIF_RX_SUCCESS )
				{ /* Get Header Data */
//					LpxSifStopxRxDMA( this );						/* Stop RxDMA					*/
					Com->RXDISABLE( );								/* Disable SIF Receive			*/
					KpiRstLongTimer( &(this->StxTimer) );			/* Reset TxTimer				*//* @@DMA�s�g�p */
//					this->SqStep = LpxSifMbusChkMyMsg( this );/* TODO: ��M���b�Z�[�W�̃`�F�b�N����	*/
					this->SqStep = SQSTEP_WAITSTX;					/* Check the Header Data		*/
				}
				else if( rc == SIF_RX_ERROREND )
				{ /* Error End */
					Com->RXDISABLE( );								/* Disable SIF Receive			*/
					this->SqStep = SQSTEP_INITIAL;					/* Goto Initial Step			*/
				}
				else /* SIF_RX_CONTINUE */
				{
					; /* CONTINUE */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_WAITSTX:	/* ���M�J�n�҂� 													*/
/*--------------------------------------------------------------------------------------------------*/
			if( this->ReqStx == SIFREQ_STARTTX )					/* ���M�J�n�v���`�F�b�N			*/
			{
				this->ReqStx  = 0x00;								/* Reset ReqStx					*/
				this->SqStep  = SQSTEP_STARTTX;						/* ���M�J�n����STEP�ɑJ��		*/
			}
			else if( this->ReqStx == SIFREQ_NOANSWER )				/* ������(MsgErr)�v���`�F�b�N	*/
			{
				this->ReqStx  = 0x00;								/* Reset ReqStx					*/
				this->SqStep  = SQSTEP_INITIAL;						/* �����X�e�b�v�ɖ߂�			*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_STARTTX:	/* ���M�J�n����														*/
/*--------------------------------------------------------------------------------------------------*/
			timer = KpiGetLongTimer( &(this->StxTimer) );	/* �����ɉ���Message��ԐM����ƁA		*/
			if( timer >= SIF_TXWAITTIME )					/* ���肪��M�ł��Ȃ��ꍇ������̂ŁA	*/
			{												/* �����őҎ��Ԃ𒲐�����				*/
#if TX_DMA_USE == 1
				LpxSifStartTxDMA( this );					/* ���M�c�l�`�X�^�[�g					*//* @@DMA�s�g�p */
#else
				Com->TXENABLE( );							/* SIF Transmit Enable					*/
				this->TxResCnt = this->TxResLen;			/* Get Data Size						*/
#endif
				this->SqStep = SQSTEP_WAITETX;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_WAITETX:	/* ���M�f�[�^�ݒ�													*/
/*--------------------------------------------------------------------------------------------------*/
#if TX_DMA_USE == 1
			if( Com->CHK_TxDMA_TC( ) )								/* �c�l�`���M�����`�F�b�N		*//* @@DMA�s�g�p */
#else
			if( TRUE == LpxSifSetTxData(this) )						/* UART���M����					*/
#endif
			{ /* When All Data Tx Complete */
				this->SqStep = SQSTEP_CHKASIF;						/* ���M�����m�FSTEP�ɑJ��		*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_CHKASIF:	/* ���M�����m�F														*/
/*--------------------------------------------------------------------------------------------------*/
			if( LPX_TXCHKF( Com ) )				    /* �r�h�e���M�����`�F�b�N					*/
			{/*�V���A�����x�ύX���ύX�����m�F<S041>*/
				DataAddr = 0;	/*	���M�f�[�^�擪	*/
				/*�@�V�X�e���_�E�����[�h���̂݁@*/
				if(pTxBuf[DataAddr+1] == 0x50)
				{
					/*�@�V���A���h�e���x�ύX�w�߂̉������̂݁@*/
					if((pTxBuf[DataAddr+2]==0x00) && (pTxBuf[DataAddr+3]==0x0D))
					{
						/*�@�ؑ֓]�����x�f�[�^�쐬�@*/
						UartSpeed = pTxBuf[DataAddr+9];
						UartSpeed += (pTxBuf[DataAddr+8] * 0x100);
						UartSpeed += (pTxBuf[DataAddr+7] * 0x10000);
						UartSpeed += (pTxBuf[DataAddr+6] * 0x1000000);
					/*	�]�����x�ؑւ̎��s	*/
					KpxInitCpuSerialIF( COM0_UART, UartSpeed, 8, 2, 1);	/*	�w��]�����x��	*/
					}
				}
				this->SqStep = SQSTEP_TXCMPLT;		/* ���M��������STEP�ɑJ��					*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_TXCMPLT:	/* ���M�������� 													*/
/*--------------------------------------------------------------------------------------------------*/
			Com->STOP_TxDMA( );							/* stop DMA 								*/
			Com->TXDISABLE( );							/* Transmit Disable							*/
			LPX_V850_BUG1( );							/* V850E2/ME3�FTxDMA�̌듮��΍�			*/
/*--------------------------------------------------------------------------------------------------*/
//			if ( LPX_CHK_BRTCHG( Com, *(this->pBrtPrm) ) )             /* Check baudrate chg  */
//			{
//				Com->RXDISABLE( );					                 /* Disable SIF Receive	*/
//				Com->STOP_RxDMA( );					                 /* Stop SIF RxDMA		*/
//				this->SqStep = SQSTEP_INITIAL;				                 /* Initial step		*/
//			}
/*--------------------------------------------------------------------------------------------------*/
//			else 
			{
				//this->SqStep = SQSTEP_WAITRX;			/* ��M�҂�STEP�ɑJ��						*/
				this->SqStep = SQSTEP_INITIAL;			/* Goto Initial Step					*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_SIFRXERR:	/* ��M�ُ� 														*/
/*--------------------------------------------------------------------------------------------------*/
//			LpxSifStopxRxDMA( this );					/* Stop RxDMA								*//* @@DMA�s�g�p */
			Com->RXDISABLE( );							/* Disable SIF Receive						*/
			this->SqStep = SQSTEP_INITIAL;				/* Goto Initial Step						*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		default:
/*--------------------------------------------------------------------------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		MEMOBUS���b�Z�[�W��M�����m�F�`�o�h�֐��T�[�r�X�iRound�p�j									*/
/*																									*/
/*			�߂�l�FMEMOBUS���b�Z�[�W�𐳏�Ɏ�M�����ꍇ	: TRUE									*/
/*					����M�̏ꍇ							: FALSE									*/
/*																									*/
/*			�Ăяo�����́A���̊֐��̖߂�l��TRUE�ł��邱�Ƃ��m�F���Ď�M�o�b�t�@���Q�Ƃ��Ă�������	*/
/*			�}�l�[�W���́A����DpxSetSifMbusTrnsStart()�ɂ���đ��M�J�n�w�߂��󂯂�܂Ŏ�M�o�b�t�@	*/
/*			�ɂ̓A�N�Z�X���܂���B																	*/
/****************************************************************************************************/
LONG	LpiCheckRxSerialMsg( CSIF *this, USHORT *pCmdMsgLen )
{
/*--------------------------------------------------------------------------------------------------*/
/*		��M������																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->SqStep == SQSTEP_WAITSTX )
		{
			*pCmdMsgLen = this->RxCmdCnt;
			//*pCmdMsgLen = this->RxCmdLen;
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		��M������																					*/
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUS���b�Z�[�W���M�J�n�v���`�o�h�֐��T�[�r�X�iRound�p�j									*/
/*																									*/
/*			�߂�l�F																				*/
/*						(1)����I��																	*/
/*						(0)�V�[�P���X�ُ�															*/
/*																									*/
/****************************************************************************************************/
LONG	LpiStartTxSerialMsg(				/* �V���A���ʐM�������b�Z�[�W���M�J�n�w��				*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* ���M�f�[�^�o�b�t�@									*/
		LONG	TxSize		)				/* ���M�f�[�^�T�C�Y(ByteSize,���������̓[��)			*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		�������b�Z�[�W�^�C�v�̃`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->SqStep != SQSTEP_WAITSTX )		/* �V�[�P���X�`�F�b�N							*/
		{
			return( FALSE );						/* �ُ탊�^�[��									*/
		}
		else if( TxSize > 0 )						/* ���M�f�[�^�T�C�Y�`�F�b�N						*/
		{
			this->pTxRes   = TxBuf;					/* �����o�b�t�@�A�h���X�ݒ�						*/
			this->TxResLen = (USHORT)TxSize;		/* �������b�Z�[�W��(Byte��)�ݒ�					*/
			this->ReqStx   = REQSTX_STARTTX;		/* ���M�J�n���N�G�X�g�ݒ�						*/
			return( TRUE );
		}
		else
		{
			this->pTxRes   = NULL;					/* �����o�b�t�@�A�h���X�ݒ�						*/
			this->TxResLen = 0;						/* �������b�Z�[�W��(Byte��)�ݒ�					*/
			this->ReqStx   = REQSTX_NORESPONSE;		/* ���������N�G�X�g�ݒ�							*/
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
//		return( FALSE );							/* �ُ탊�^�[��									*/
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUS���b�Z�[�W���M�J�n�v���`�o�h�֐��T�[�r�X�i�����������j								*/
/*																									*/
/*			�߂�l�F																				*/
/*						(1)����I��																	*/
/*						(0)�V�[�P���X�ُ�															*/
/*																									*/
/****************************************************************************************************/
LONG	LpiStartTxSerialMsgNoResponse(		/* �V���A���ʐM�������b�Z�[�W���M�J�n�w��(�������p)		*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* ���M�f�[�^�o�b�t�@									*/
		LONG	TxSize		)				/* ���M�f�[�^�T�C�Y(ByteSize,���������̓[��)			*/
{
	return LpiStartTxSerialMsg( this, NULL, 0 );
}



/****************************************************************************************************/
/*																									*/
/*		�r�h�e��M�c�l�`�X�^�[�g����																*/
/*																									*/
/****************************************************************************************************/
void	LpxSifStartRxDMA( CSIF *this )
{
CSIFCOMPORT *pCom = (CSIFCOMPORT *)this->Com;						/* get com port 				*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set RxDMA 																					*/
/*--------------------------------------------------------------------------------------------------*/
		this->RxDmaScnt = DMA_RXSIZE;
		pCom->SETX_RxDMA( (UINT)(pCom->RXBUF), (UINT)(this->RxBuf), this->RxDmaScnt ); 
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`���r�h�e�X�^�[�g����																	*/
/*--------------------------------------------------------------------------------------------------*/
		pCom->START_RxDMA( );							/* Enable RxDMA								*/
		pCom->RXENABLE( );								/* Enable SIF Receive						*/
/*--------------------------------------------------------------------------------------------------*/
		return;

}


/****************************************************************************************************/
/*																									*/
/*		�r�h�e��M�c�l�`�X�g�b�v����																*/
/*																									*/
/****************************************************************************************************/
void	LpxSifStopxRxDMA( CSIF *this )
{
CSIFCOMPORT *Com = (CSIFCOMPORT *)this->Com;
/*--------------------------------------------------------------------------------------------------*/
/*		�r�h�e�X�g�b�v����																			*/
/*--------------------------------------------------------------------------------------------------*/
		Com->RXDISABLE( );								/* Disable SIF Receive						*/
		Com->STOP_RxDMA( );								/* Stop SIF RxDMA							*/
/*--------------------------------------------------------------------------------------------------*/
		return;

}


/****************************************************************************************************/
/*																									*/
/*		�r�h�e���M�c�l�`�X�^�[�g����																*/
/*																									*/
/****************************************************************************************************/
void	LpxSifStartTxDMA( CSIF *this )
{
CSIFCOMPORT *Com = this->Com;
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`�]���J�n																				*/
/*--------------------------------------------------------------------------------------------------*/
		Com->TXENABLE( );								/* SIF Transmit Enable						*/
		KlibWaitus( 1 );								/* Wait	1 us								*/
		Com->SETX_TxDMA( ((UINT)(this->pTxRes)),      	/* source address 							*/
						(UINT)(Com->TXBUF), 		    /* destination address						*/
						(this->TxResLen) ); 		    /* tx count									*/
		Com->START_TxDMA( );						    /* Start SIF TxDMA							*/
		//KlibWaitus(1);								/* Wait 1us									*/
		//Com->SWTRG_TxDMA( );							/* Start Tx ( software trigger )            */
/*--------------------------------------------------------------------------------------------------*/
		return;

}



/****************************************************************************************************/
/*																									*/
/*		UART�f�[�^���M���m�F																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifSetTxData( CSIF *this )
{
	LONG		i;
	LONG		DataAddr;
	LONG		rc = FALSE;
	UCHAR		*pTxBuf = this->pTxRes;
	CSIFCOMPORT	*Com = (CSIFCOMPORT *)this->Com;

	/* Set Current TxData Address */
	DataAddr = this->TxResLen - this->TxResCnt;

	if( this->TxResCnt != 0 )
	{ /* When Tx Request Data Exist */
//		if( LPX_TXCHKL(Com) )
		if( Com->CHK_TXEMPTY( ) )
		{ /* Tx FIFO is Empty */
			for( i=0; i<16; i++ )											/* 16 = FIFO size		*/
			{
				*(Com->TXBUF) = pTxBuf[ DataAddr++ ];						/* get a charactor		*/
				this->TxResCnt--;											/* Tx Cnt Decriment		*/

				if( this->TxResCnt == 0 )
				{ /* TXDATA is Noting */
					rc = TRUE;
					break;
				}
			}
		}
	}

	return ( rc );
}



/****************************************************************************************************/
/*																									*/
/*		UART�f�[�^��M�m�F																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		1) FIFO���g�p����2ms�����̃f�[�^��M�B														*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifCheckRxData( CSIF *this, LONG mode )
{
	LONG		i;
	LONG		HeaderSize;
	LONG		rc = SIF_RX_CONTINUE;
	UCHAR		*pRxBuf = this->RxBuf;
	CSIFCOMPORT	*Com = (CSIFCOMPORT *)this->Com;

	if( Com->CHK_TIMEOUT( ) || Com->CHK_RXRDY( ) ) 	/* Check Charactor Timeout or Trigger Interrupt	*/
	{ /* When exist RXDATA */
		KpiRstLongTimer( &(this->RxTimer) );								/* Reset RxTimer		*/
		for( i=0; i<16; i++ )												/* 16 = FIFO size		*/
		{
			if( !LPX_CHK_RXRDY(Com) )
			{ /* RXDATA not ready */
				break;
			}
			pRxBuf[ this->RxCmdCnt++ ] = *(Com->RXBUF);						/* get a charactor		*/
		}
	}

	if( mode == SIF_GETHEADER )
	{ /* �w�b�_�[�擾�v�� */
		/* �w�b�_�[�T�C�Y�擾 */
		if( KPI_CHK_OPEMSG( pRxBuf[1]) ){ HeaderSize = DIGOP_MINSIZE; }
/* <S150> >>>>> */
		/* �t�@�[���_�E�����[�h���̃w�b�_�� */
		else if( KPI_CHK_SYSDLMSG( pRxBuf[1] ) )
		{
			HeaderSize = MBUS_MINSIZE;	/* �ʏ� */
			if( this->RxCmdCnt > MBUS_DL_SF_IDX )
			{
				switch( pRxBuf[MBUS_DL_SF_IDX] )
				{
				/* �v���O���������݁^���M�w�߂̓f�[�^���܂ł��w�b�_�T�C�Y�Ƃ��� */
				case MBUS_DL_PRGWR:
					/* fall through */
				case MBUS_DL_PRGSND:
					HeaderSize = MBUS_DL_DATANUM_LEN;
					break;
				default:
					break;
				}
			}
		}
/* <<<<< <S150> */
		else							{ HeaderSize = MBUS_MINSIZE; }

		if( this->RxCmdCnt >= HeaderSize )
		{
			rc = SIF_RX_SUCCESS;										/* �w�b�_�[��M����		*/
		}
	}
	else /* mode == SIF_GETALLDATA */
	{ /* �S�f�[�^�擾�v�� */
		if( this->RxCmdCnt >= this->RxCmdLen )
		{
			rc = SIF_RX_SUCCESS;										/* �S�f�[�^��M����		*/
		}
	}

	/* Check Time Over */
	if( (rc == SIF_RX_CONTINUE) && (SIF_RXTIMEOVER <= KlibGetLongTimerMs(this->RxTimer)) )
	{ /* �`������M�^�C���I�[�o */
		KpiRstLongTimer( &(this->RxTimer) );							/* Reset RxTimer		*/
		SET_SIFERR( this, SIFERR_RXTMOVR );								/* �`������MT.O.		*/
		rc = SIF_RX_ERROREND;											/* �G���[�I��			*/
	}


	if( mode == SIF_GETALLDATA )
	{
//		RxCmdCtr = this->RxCmdCnt;
	}

	return ( rc );
}



/****************************************************************************************************/
/*																									*/
/*		�c�l�`��M�m�F																				*/
/*																									*/
/*		1)MICROSSP��UART FIFO���g�p����B															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifCheckRxDMA( CSIF *this )
{
int i;
int rxcnt;
CSIFCOMPORT *Com = (CSIFCOMPORT *)this->Com;
UCHAR *pRxBuf = this->RxBuf;

/* for debug */
//ULONG *IIRFCR_Address;
//VULONG IIRFCR_data;
//IIRFCR_Address = (ULONG*)0xEFFF4808;
/* for debug */
/*--------------------------------------------------------------------------------------------------*/
	if ( Com->CHK_TIMEOUT( ) ) 								/* Check Charactor Timeout Interrupt */
//	IIRFCR_data = ((VULONG)*IIRFCR_Address & 0x0F);
//	if ( (IIRFCR_data == 0x0C) || (IIRFCR_data == 0x04) )
//	if ( (IIRFCR_data & 0x0F) != 0x01 )
	{
	/*----------------------------------------------------------------------------------------------*/
		Com->RXDISABLE( );									/* Disable SIF Receive */
		Com->STOP_RxDMA( );									/* stop DMA */
		this->RxDmaRcnt = LPX_GET_RXDMA_CNT( Com );			/* save RxDMA count	*/
		rxcnt = this->RxDmaScnt - this->RxDmaRcnt;			/* set receive count */
//		rxcnt = 0;
	/*----------------------------------------------------------------------------------------------*/
		for( i=0; i<16; i++)								/* 16 = FIFO size */
		{
			if ( ! LPX_CHK_RXRDY( Com ) ) { break; }		/* check Rx ready flag */
			pRxBuf[ rxcnt++ ] = *(Com->RXBUF);				/* get a charactor */
		}
	/*----------------------------------------------------------------------------------------------*/
		if( rxcnt == 0 ) { Com->START_RxDMA( );	}			/* Enable RxDMA							*/
	/*----------------------------------------------------------------------------------------------*/
		return (rxcnt);
	}
	else if( Com->CHK_RxDMA_TC( ) )							/* Check Transaction Complete */
	{
		Com->RXDISABLE( );									/* Disable SIF Receive */
		Com->STOP_RxDMA( );									/* stop DMA */
		this->RxDmaRcnt = LPX_GET_RXDMA_CNT( Com );			/* save RxDMA count	*/
		rxcnt = this->RxDmaScnt - this->RxDmaRcnt;			/* set receive count */
		return (rxcnt);
	}
/*--------------------------------------------------------------------------------------------------*/
	return (0);
}



/****************************************************************************************************/
/*																									*/
/*		���ǈ����b�Z�[�W�̃`�F�b�N																	*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifMbusChkMyMsg( CSIF *this )	/* ���ǃA�h���X�`�F�b�N									*/
{
		if( SIF_DEBUG == 1 )
		{
			return ( SQSTEP_WAITSTX );							/* debug */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����M�������b�Z�[�W�̃A�h���X��=="0" or ==���ǃA�h���X�̏ꍇTRUE��Ԃ�					*/
/*		�ǃA�h���X=="0"�̏ꍇ�A���b�Z�[�W�͏������邪�������Ƃ���B									*/
/*		�������A�f�W�I�y���b�Z�[�W�A�_�E�����[�h���b�Z�[�W�͉�����Ԃ��B							*/
/*--------------------------------------------------------------------------------------------------*/
		if( KPI_CHK_OPEMSG( this->RxBuf[1] ) )
		{														/* Operator Message					*/
//<S150>			this->RxCmdLen = LpxSifGetCmdMsgLength(				/* Get Message Length				*/
			this->RxCmdLen = RkiMsgifGetCmdMsgLength(			/* Get Message Length				*/
									this->RxBuf,
									this->RcvBufSize );
			this->StxRes = LpiStartTxSerialMsg;                 /* set method for start Tx			*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA�s�g�p						*/
			return ( SQSTEP_RXCMPLT );
		}
		else if ( KPI_CHK_SYSDLMSG( this->RxBuf[1] ) )
		{														/* System Download Message			*/
//<S150>			this->RxCmdLen = LpxSifGetCmdMsgLength(				/* Get Message Length				*/
			this->RxCmdLen = RkiMsgifGetCmdMsgLength(			/* Get Message Length				*/
									this->RxBuf,
									this->RcvBufSize );
			this->StxRes = LpiStartTxSerialMsg;                 /* set method for start Tx			*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA�s�g�p						*/
			return ( SQSTEP_RXCMPLT );
		}
		else if( this->RxBuf[0] == 0x00 )
		{														/* Other Message( address = 0 )		*/
			this->StxRes = LpiStartTxSerialMsgNoResponse;       /* set method for no response		*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA�s�g�p						*/
			return ( SQSTEP_RXCMPLT );
		}
		else if ( this->RxBuf[0] == (this->AxisNumber) )
		{														/* Other Message(address = my addr)	*/
//<S150>			this->RxCmdLen = LpxSifGetCmdMsgLength(				/* Get Message Length				*/
			this->RxCmdLen = RkiMsgifGetCmdMsgLength(			/* Get Message Length				*/
									this->RxBuf,
									this->RcvBufSize );
			this->StxRes = LpiStartTxSerialMsg;                 /* set method for start Tx			*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA�s�g�p						*/
			return ( SQSTEP_RXCMPLT );
		}
		else
		{
			return ( SQSTEP_INITIAL );
		}
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : �w�߃��b�Z�[�W���擾															*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> �폜 */
#define LPX_GET_CMDMSGLEN( i, Mul, Add )\
		if( CmdBufLen < (i+2) ){ return( 0 );}\
		else{ return( (((CmdBuf[i]<<8) + CmdBuf[i+1]) * Mul) + Add );}
#define LPX_CHK_SLOT( SfcH )			(SfcH == 0x01)
#define LPX_GET_SLOT_BYTELEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 2 : 0)
#define LPX_GET_SLOT_WORDLEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 1 : 0)
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxSifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen )
{
LONG	s = LPX_GET_SLOT_BYTELEN( CmdBuf[2] );
/*--------------------------------------------------------------------------------------------------*/
/*		Check CmdBufLen																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( CmdBufLen < 4 )
		{
			return( 0 );								/* CmdBufLen is Short						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Calculate CmdMsgLen													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 return( 8 );
		case MFC_SVXMSG16:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_ONEREG: return(  8 + s );
			 case SFC_WR_ONEREG: return( 10 + s );
			 case SFC_RD_SEQREG: return( 10 + s );
			 case SFC_WR_SEQREG: LPX_GET_CMDMSGLEN( 6 + s, 2, 12 + s );
			 case SFC_RD_RANREG: LPX_GET_CMDMSGLEN( 4 + s, 2,  8 + s );
			 case SFC_WR_RANREG: LPX_GET_CMDMSGLEN( 4 + s, 4, 10 + s );
			 case SFC_RD_MAXNUM: return(  8 + s );
			 default: return( -1 );
			 }
//			 return( -1 );//<S0C7>statement is unreachable
		case MFC_SVXMSG32:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_SEQMEM: return( 12 + s );
			 case SFC_WR_SEQMEM: LPX_GET_CMDMSGLEN( 8 + s, 2, 12 + s );
			 default: return( -1 );
			 }
//			 return( -1 );//<S0C7>statement is unreachable
		case MFC_MECHATROLINK:
			 switch( CmdBuf[4] )
			 {
			 case SFC_RD_REQREG_42:
				 return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + MBUS42_CRC_SIZE );
			 case SFC_RD_RANREG_42:
			 	return( MBUS42_HEADERSIZE + (MBUS42_ADRSIZE * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 case SFC_WR_SEQREG_42:
			 	switch( CmdBuf[5] & 0x0F )
			 	{
		 		case MBUS42_DTYPE_B: /* �f�[�^�^�FByte(8bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (1 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_S: /* �f�[�^�^�FByte(16bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (2 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_L: /* �f�[�^�^�FByte(32bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (4 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 	default: return( -1 );
			 	}
			 case SFC_WR_RANREG_42:
			 	return( MBUS42_HEADERSIZE + ((MBUS42_ADRSIZE + 4) * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 case SFC_RD_MAXNUM_42:
				 	return( MBUS42_HEADERSIZE + MBUS42_CRC_SIZE );
			 case SFC_RD_PRDCTID_42:
			 default: return( -1 );
			 }
		case MFC_SYSDWNLD:								/* @@@ Add Later @@@						*/
			 return( MbusFuncSysDLGetMsgLength(CmdBuf) );
		default:
			 if( KPI_CHK_OPEMSG( CmdBuf[1] ) )
			 {
				return( CmdBuf[2] + (CmdBuf[3]<<8) );
			 }
			 return( -1 );
		}
/*--------------------------------------------------------------------------------------------------*/
		/* unreachable return( -1 );  */				/* Unknown Function Code					*/
}
#endif	/* <S150> */

/***************************************** end of file **********************************************/
