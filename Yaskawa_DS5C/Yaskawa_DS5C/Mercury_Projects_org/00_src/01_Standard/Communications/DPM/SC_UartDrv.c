/****************************************************************************/
/*																			*/
/*		SC Uart data Control �\�[�X�t�@�C��									*/
/*																			*/
/****************************************************************************/
/*																			*/
/*	Target model: SigmaV Command-Option	ServoDrive							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	History:																*/
/*	2007.9.12	created														*/
/*																			*/
/*																			*/
/************** Copyright(c) Yaskawa Electric Corporation *******************/

#define	KPI_IOREG


/* includes */
#include "KnlStd.h"
#include "DeviceMngr.h"
#include "SC_UartDrv.h"


/* defines */
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_MINSIZE		5				/* MEMOBUS���b�Z�[�W�ŏ��o�C�g��						*/
#define	MBUS_MAXSIZE		256				/* MEMOBUS���b�Z�[�W�ő�o�C�g��						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF_RXTIMEOVER		 100			/* �`������M�^�C���I�[�o�ݒ�l   [ms]					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SQSTEP_INITIAL			 0
#define	SQSTEP_STARTRX			 1
#define	SQSTEP_WAITRX			 2
#define	SQSTEP_RX1ST	  		 3
#define	SQSTEP_RX2ND	  		 4
#define	SQSTEP_RXLAST	  		 5
#define	SQSTEP_RXCMPLT			 6
#define	SQSTEP_WAITSTX			 7
#define	SQSTEP_WAITSTX2			 8
#define	SQSTEP_STARTTX			 9
#define	SQSTEP_WAITETX			10
#define	SQSTEP_CHKASIF	 		11
#define	SQSTEP_TXCMPLT			12
#define	SQSTEP_RXTMOVER			13
#define	SQSTEP_RXSEQERR			14
#define	SQSTEP_XMDCHKRM			15
#define	SQSTEP_XMDMKANS			16


/****************************************************************************************************/
/*																									*/
/*		V850 ME3 �b�o�t�F�V���A���C���^�t�F�C�X��`													*/
/*																									*/
/****************************************************************************************************/
#define	CPU_SIF2_RXBUF		UB0RX							/* Serial Receive  Data	Buffer			*/
//#define	CPU_SIF2_TXBUF		UB0TX							/* Serial Transmit Data Buffer			*/
#define	CPU_SIF2_RXSTS		UB0STR							/* Serial Receive  Status Reg.			*/
//#define	CPU_SIF2_RXCHKF		URIF0							/* Serial Receive  Check Flag (Int.Req)	*/
//#define	CPU_SIF2_TXCHKF		UTIF0							/* Serial Transmit Check Flag (Int.Req)	*/
/*--------------------------------------------------------------------------------------------------*/
//#define	KPX_SIF2_RSTRXSTS()		{ CPU_SIF2_RXSTS = 0x00;}		/* Serial Reset Rx Status			*/
//#define	KPX_SIF2_CHKTXBUSY()	( UB0TSF == 1 )					/* Serial TxBuf    Busy   Check		*/
#define	KPX_SIF2_CHKTXFNSH()	( UB0TSF == 0x00 )				/* Serial Transmit Finish Check		*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_SIF2_RXENABLE()		( UB0RXE=0x01 )					/* Serial Receive  Enable			*/
#define	KPX_SIF2_RXDISABLE()	( UB0RXE=0x00 )					/* Serial Receive  Disable			*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_SIF2_TXENABLE()		( UB0TXE=0x01 )					/* Serial Transmit Enable			*/
#define	KPX_SIF2_TXDISABLE()	( UB0TXE=0x00 )					/* Serial Transmit Disable			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		�b�o�t�F�c�l�`�C���^�t�F�C�X��`															*/
/*																									*/
/****************************************************************************************************/
//#define SIF2_SETERR( code )	{ OpSif.ErrCod = code; ++OpSif.ErrCnt;}
#define SIF2_SETERR( code )	 (OpSif.ErrCod = (code))
/*--------------------------------------------------------------------------------------------------*/
/*		Address Definition for Setting SIF DMA														*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_SET_RxDTFR		0x3B							/* RxDMA TrgSel:INTSR0 (UART IntRx)		*/
#define	SIF2_SET_TxDTFR		0x3C							/* TxDMA TrgSel:INTST0 (UART IntTx)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_SET_RxDADC		0xE800							/* RxDMA Addr. Control Register			*/
#define	SIF2_SET_TxDADC		0xB200							/* TxDMA Addr. Control Register			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_SET_RXB_HIADR	0x0FFF							/* RxDMA Serial RXB0 High Address		*/
#define	SIF2_SET_RXB_LOADR	0xFA06							/* RxDMA Serial RXB0 Low  Address		*/
#define	SIF2_SET_TXB_HIADR	0x0FFF							/* TxDMA Serial TXB0 High Address		*/
#define	SIF2_SET_TXB_LOADR	0xFA08							/* TxDMA Serial TXB0 Low  Address		*/
/*--------------------------------------------------------------------------------------------------*/
/*		DMA I/F Register Definition for SIF															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_RxDBC			DBC0							/* RxDMA Transfer Count Register		*/
#define	SIF2_RxDSAL			DSA0L							/* RxDMA Source Address Register		*/
#define	SIF2_RxDSAH			DSA0H							/* RxDMA Source Address Register		*/
#define	SIF2_RxDDAL			DDA0L							/* RxDMA Destination Address Register	*/
#define	SIF2_RxDDAH			DDA0H							/* RxDMA Destination Address Register	*/
#define	SIF2_RxDADC			DADC0							/* RxDMA Addressing Control Register  	*/
#define	SIF2_RxDTFR			DTFR0							/* RxDMA Trigger Factor Register		*/
#define	SIF2_RxDCHC			DCHC0							/* RxDMA Channel Control Register		*/
#define	SIF2_RxTC			TC0								/* RxDMA Transfer Complete Flag			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_TxDBC			DBC1							/* TxDMA Transfer Count Register		*/
#define	SIF2_TxDSAL			DSA1L							/* TxDMA Source Address Register		*/
#define	SIF2_TxDSAH			DSA1H							/* TxDMA Source Address Register		*/
#define	SIF2_TxDDAL			DDA1L							/* TxDMA Destination Address Register	*/
#define	SIF2_TxDDAH			DDA1H							/* TxDMA Destination Address Register	*/
#define	SIF2_TxDADC			DADC1							/* TxDMA Addressing Control Register  	*/
#define	SIF2_TxDTFR			DTFR1							/* TxDMA Trigger Factor Register		*/
#define	SIF2_TxDCHC			DCHC1							/* TxDMA Channel Control Register		*/
#define	SIF2_TxTC			TC1								/* TxDMA Transfer Complete Flag			*/
#define	SIF2_TxDF			DF1								/* TxDMA Transfer Request Flag			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Stop DMA CPU Bug Fix for V850E/MA1															*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_TC01_RxDMA_BUGFIX( )
#define KPX_TC01_TxDMA_BUGFIX( )
/*--------------------------------------------------------------------------------------------------*/
#define KPX_STOP_DMA01_BUGFIXA( )
#define KPX_STOP_DMA01_BUGFIXB( )
/*--------------------------------------------------------------------------------------------------*/
/*		SIF RxDMA/TxDMA Operation Macro																*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_STOP_RxDMA( )	 (SIF2_RxDCHC = 0x04)		/* Stop SIF RxDMA (DMA CH0)				*/
#define KPX_SIF2_STOP_TxDMA( )	 (SIF2_TxDCHC = 0x04)		/* Stop SIF TxDMA (DMA CH1)				*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_ENABLE_RxDMA( )	(SIF2_RxDCHC = 0x01)		/* Enable SIF RxDMA					*/
#define KPX_SIF2_ENABLE_TxDMA( )	(SIF2_TxDCHC = 0x01)		/* Enable SIF TxDMA					*/
#define KPX_SIF2_SWTRIG_TxDMA( )	(SIF2_TxDCHC = 0x03)		/* S/W Trigger SIF TxDMA			*/
/*--------------------------------------------------------------------------------------------------*/
//#define KPX_SIF_RESET_TxDMA( )	{ SIF_TxDTFR = SIF_SET_TxDTFR;}	/* Reset TxDMA Request Flag		*/
#define KPX_SIF2_RESET_TxDMA( )	(SIF2_TxDF = 0x00)			/* Reset TxDMA Request Flag				*/
/*--------------------------------------------------------------------------------------------------*/
/*		V850E2/ME3 UART���M+DMA�̑��M�J�n�s��΍��p												*/
/*			( TxDMA DTFR3�i���MDMA�̗v�����W�X�^�j���N���A������ADCHC3�i���MDMA����j���~		*/
/*			  RxDMA ������O���B-> NEC-EL����񎦂��ꂽ�C�����@										*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_RESET_TxDMA_BUGFIXA( )	do{						/* Reset SIF TxDMA					*/\
		SIF2_TxDF   = 0x00;										/* Reset DMA Request 				*/\
		SIF2_TxDCHC = 0x04;										/* Stop TxDMA						*/\
		SIF2_TxDCHC = 0x00;										/* DMA Channel Control Register		*/\
		SIF2_TxDF   = 0x00;										/* Reset TxDMA Request Flag			*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_RxDMA_CNT( )	( SIF2_RxDBC )				/* SIF RxDMA Count						*/
#define KPX_SIF2_TxDMA_CNT( )	( SIF2_TxDBC )				/* SIF TxDMA Count						*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_CHK_RxDMA_TC( )	( SIF2_RxTC != 0)		/* Check RxDMA Transfer Complete		*/
#define KPX_SIF2_CHK_TxDMA_TC( )	( SIF2_TxTC != 0)		/* Check TxDMA Transfer Complete		*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_SIF2_CHK_TxDMA_DTFR( )	( (SIF2_TxDTFR & 0x80) != 0 )/* Check DMA END for only V850ME3	*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_INIT_RxDMA( ) do{								/* Init SIF RxDMA					*/\
		SIF2_RxDSAL = SIF2_SET_RXB_LOADR;						/* DMA Src Low  Address Register	*/\
		SIF2_RxDSAH = SIF2_SET_RXB_HIADR;						/* DMA Src High Address Register	*/\
		SIF2_RxDADC = SIF2_SET_RxDADC;							/* DMA Addressing Control Register	*/\
		SIF2_RxDTFR = SIF2_SET_RxDTFR;							/* DMA Trigger Register				*/\
		SIF2_RxDCHC = 0x00;										/* DMA Channel Control Register		*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_INIT_TxDMA( ) do{								/* Init SIF TxDMA					*/\
		SIF2_TxDDAL = SIF2_SET_TXB_LOADR;						/* DMA Des. Low  Address Register	*/\
		SIF2_TxDDAH = SIF2_SET_TXB_HIADR;						/* DMA Des. High Address Register	*/\
		SIF2_TxDADC = SIF2_SET_TxDADC;							/* DMA Addressing Control Register	*/\
		SIF2_TxDTFR = SIF2_SET_TxDTFR;							/* DMA Trigger Register				*/\
		SIF2_TxDCHC = 0x00;										/* DMA Channel Control Register		*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_SETX_RxDMA( Adr, Cnt ) do{						/* Set SIF RxDMA					*/\
		SIF2_RxDDAL = KPX_SIF_SET_DMA_LOADR( Adr );				/* DMA Source Low  Address Register	*/\
		SIF2_RxDDAH = KPX_SIF_SET_DMA_HIADR( Adr );				/* DMA Source High Address Register	*/\
		SIF2_RxDBC  = (Cnt);									/* DMA Count Register				*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_SETX_TxDMA( Adr, Cnt ) do{						/* Set SIF TxDMA					*/\
		SIF2_TxDF   = 0x00;										/* Reset DMA Request Flag			*/\
		SIF2_TxDSAL = KPX_SIF_SET_DMA_LOADR( Adr );				/* DMA Source Low  Address Register	*/\
		SIF2_TxDSAH = KPX_SIF_SET_DMA_HIADR( Adr );				/* DMA Source High Address Register	*/\
		SIF2_TxDBC  = (Cnt);									/* DMA Count Register				*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/



/* globals */
#pragma ghs startzda
static SIF		OpSif;								/* �V���A���ʐM�h�e�ϐ�									*/
#pragma ghs endzda


/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif

/* Private functions */
static void	opSifStartRxDMA(const UCHAR * pRxBuf, USHORT rxSz );
static void	opSifStartTxDMA(const UCHAR * pTxBuf, USHORT transSz );
static void	opSifStopxRxDMA( void );
static USHORT	opMsg1stAnalyze(const UCHAR *pRcvBuf, UCHAR *TblIdx, UCHAR *ConfBLen);
static USHORT	opMsg2ndAnalyze(const UCHAR *pRcvBuf, UCHAR TblIdx);

#ifdef __cplusplus
}
#endif



/****************************************************************************
****************************************************************************/
/* SC Message Data Exchange Functions */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	SC Message��Ԏ擾
-----------------------------------------------------------------------------
paramter:	none
return;		SCMSG_ENABLE	- ready state
			SCMSG_BUSY		- not ready state
*****************************************************************************/
LONG MLOpiGetSCMsgState(void)
{
	LONG	rc = SCMSG_BUSY;
	if( (OpSif.SqStep == SQSTEP_WAITSTX) && (OpSif.f.MemobusMode != FALSE) )
	{
		rc = SCMSG_ENABLE;
	}
	return rc;
}

/*****************************************************************************
	SC Message�f�[�^�]���v�����s
-----------------------------------------------------------------------------
paramter:	pMsgBuf			- pointer to message data
			bSz				- size of message data
return;		SCMSG_BUSY		- request error
			SCMSG_SUCCESS	- normal end
*****************************************************************************/
LONG MLOpiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz)
{
	LONG	i, rc;
	UCHAR	*pUCbuf = (UCHAR*)pMsgBuf;

	if(OpSif.SqStep != SQSTEP_WAITSTX)
	{/* ������ */
		rc = SCMSG_BUSY;
	}
	else
	{
	/* ���X�|���X�Z�b�g */
		for(i = 0; i < bSz; i++)
		{
			OpSif.TxBuf[i] = pUCbuf[i];
		}
		OpSif.MbusTrnLength = (USHORT)bSz;
		KPX_SIF2_RESET_TxDMA_BUGFIXA( );	/* V850E2/ME3�FTxDMA�듮��΍�		*/
		opSifStartTxDMA(OpSif.TxBuf, (USHORT)(bSz - 1)); 			/* ���M�c�l�`�X�^�[�g	*/

		OpSif.RxDbcSv = sizeof(OpSif.RxBuf) - 1;
		opSifStartRxDMA(OpSif.RxBuf, OpSif.RxDbcSv);	/* ��M�c�l�`�X�^�[�g	*/
		OpSif.SqStep = SQSTEP_WAITETX;		/* ���̎w��Message�������Ɏ󂯂���*/
		rc = SCMSG_SUCCESS;
	}
	return rc;
}

/*****************************************************************************
	SC Message�����f�[�^�擾
-----------------------------------------------------------------------------
paramter:	pMsgBuf			- pointer to message data
			bSz				- size of message data
return;		SCMSG_BUSY		- busy state
			SCMSG_SUCCESS	- normal end
*****************************************************************************/
LONG MLOpiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz)
{
	LONG	i, rc;
	UCHAR	*pUCbuf = (UCHAR*)pMsgBuf;

	*bSz = 0;

	if(OpSif.SqStep != SQSTEP_WAITSTX)
	{/* ������ */
		rc = SCMSG_BUSY;
	}
	else
	{
	/* ���X�|���X�Z�b�g */
		for(i = 0; i < OpSif.RcvbLength; i++)
		{
			pUCbuf[i] = OpSif.RxBuf[i];
		}
		*bSz = (LONG)OpSif.RcvbLength;
		rc = SCMSG_SUCCESS;
	}
	return rc;
}

/*****************************************************************************
	SC Message�L�����Z���v��
-----------------------------------------------------------------------------
paramter:	none
return;		none
*****************************************************************************/
void MLOpiCancelSCMsgReq(void)
{
	if(OpSif.SqStep != SQSTEP_WAITSTX)
	{
		KPX_SIF2_TXDISABLE( );			/* Transmit Disable						*/
		opSifStopxRxDMA( );				/* Stop RxDMA							*/
		KPX_SIF2_RESET_TxDMA_BUGFIXA();	/* Stop TxDMA							*/
		KPX_SIF2_INIT_RxDMA( );			/* Init SIF RxDMA for RXB0  --> RxBuf	*/
		KPX_SIF2_INIT_TxDMA( );			/* Init SIF TxDMA for TxBuf --> TXB0	*/
		OpSif.SqStep = SQSTEP_INITIAL;	/* Goto Initial Step					*/
	}

	return;
}


#ifdef	__ghs__
#pragma ghs section text=default
#endif



#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	SC Message�L���ݒ�
*****************************************************************************/
void OpxEnableSCSif( USHORT data )
{
	OpSif.f.MemobusMode = (data != 0) ? TRUE: FALSE;
}


/****************************************************************************************************/
/*																									*/
/*		�V���A���ʐM���b�Z�[�W����������															*/
/*																									*/
/****************************************************************************************************/
void	OpxInitSCSerialMsg( void )
{
		KpiResetWatchdog( );							/* Reset Watchdog							*/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM�h�e�f�[�^������																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( &OpSif, sizeof( OpSif )/4 );	/* Reset All Data to Zero					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM�h�e������																		*/
/*--------------------------------------------------------------------------------------------------*/
//		KPX_SIF2_TXDISABLE( );							/* Transmit Disable							*/
//		KPX_SIF2_RXDISABLE( );							/* Transmit Disable							*/
		KpxInitCpuSerialIF( 0, 19200, 0x0E );			/* Initialize CPU SIF CH0					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM�p�c�l�`������																	*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_INIT_RxDMA( );							/* Init SIF RxDMA for RXB0  --> RxBuf		*/
		KPX_SIF2_RESET_TxDMA_BUGFIXA();
		KPX_SIF2_INIT_TxDMA( );							/* Init SIF TxDMA for TxBuf --> TXB0		*/

/*--------------------------------------------------------------------------------------------------*/
		KpiResetWatchdog( );							/* Reset Watchdog							*/
		return;
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif



/****************************************************************************************************/
/*																									*/
/*		�V���A���ʐM���b�Z�[�W������Ď����� ( ScanC Cycle )										*/
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
void	OpxSCSifMsgManager( void )
{
USHORT	timer, ucwk;
LONG	rxcnt;
/*--------------------------------------------------------------------------------------------------*/
/*		Reset Error Status																			*/
/*--------------------------------------------------------------------------------------------------*/
		if((CPU_SIF2_RXSTS & 0x7) != 0)						/* Read	Error check						*/
		{
			ucwk = CPU_SIF2_RXBUF;							/* Dummy Read							*/
			CPU_SIF2_RXSTS = 0;								/* Reset Error Status					*/
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM����M�Ď��V�[�P���X (��M:DMA-CH0,SIF-RXB0, ���M:DMA-CH1,SIF-TXB0)				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( OpSif.SqStep )
		{
		case SQSTEP_INITIAL: /*--------------------------- �����X�e�b�v ----------------------------*/
			OpSif.SqStep = SQSTEP_WAITSTX;				/* ��M�J�n����Step�ɑJ��					*/
			break;
		case SQSTEP_WAITSTX: /*--------------------------- ���M�J�n�҂� ----------------------------*/
			break;

		case SQSTEP_WAITETX: /*--------------------------- ���M�����҂� ----------------------------*/
			if( KPX_SIF2_CHK_TxDMA_TC( ) )				/* �c�l�`���M�����`�F�b�N					*/
			{
				KPX_TC01_TxDMA_BUGFIX( );					/* @@@V850E/MA1�s��Ή�@@@				*/
				OpSif.SqStep = SQSTEP_TXCMPLT;			/* ���M�����m�FSTEP�ɑJ��					*/
			}
			break;
		case SQSTEP_TXCMPLT: /*-------------------------- ���M�������� -----------------------------*/
			if( KPX_SIF2_CHKTXFNSH( ) )					/* �r�h�e���M�����`�F�b�N					*/
			{
				KPX_SIF2_TXDISABLE( );					/* Transmit Disable							*/
				if( KPX_SIF2_CHK_TxDMA_DTFR( ) )		/* �r�h�e���M�����`�F�b�N					*/
				{										/* V850E2/ME3�FTxDMA�̌듮��΍�			*/
					KPX_SIF2_RESET_TxDMA( );			/* V850E2/ME3�FTxDMA�̌듮��΍�			*/
				}										/* V850E2/ME3�FTxDMA�̌듮��΍�			*/
				OpSif.SqStep = SQSTEP_WAITRX;			/* ��M�҂�STEP�ɑJ��						*/
			}
			break;


		case SQSTEP_WAITRX: /*---------------------------- ��M�҂� --------------------------------*/
			if( KPX_SIF2_RxDMA_CNT() != OpSif.RxDbcSv )		/* Check Receive						*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				OpSif.SqStep = SQSTEP_RX1ST;				/* 1byte��MSTEP�ɑJ��					*/
			}
			break;
		case SQSTEP_RX1ST:/*------------------------------ �ŏ��̂S�o�C�g��M(�P�����)-------------*/
			if( KPX_SIF2_RxDMA_CNT( ) != OpSif.RxDbcRv )	/* ��M�`�F�b�N							*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				/*----------------------------------------------------------------------------------*/
				rxcnt = OpSif.RxDbcSv - OpSif.RxDbcRv;		/* Receive Count						*/
				if( rxcnt >= 4 )							/* Receive 4 byte ?						*/
				{
					OpSif.RcvbLength = opMsg1stAnalyze(OpSif.RxBuf,/* ����̃��b�Z�[�W���			*/
											 &OpSif.MbusTblIndex, &OpSif.ConfbLength);
					OpSif.SqStep = (UCHAR)((OpSif.RcvbLength != 0) ? SQSTEP_RXLAST: SQSTEP_RX2ND);
				}
			}
			else											/* ��M�Ȃ�								*/
			{
				timer = KpiGetShortTimer(&OpSif.RxTimer);	/* Get Timer							*/
				if( timer > SIF_RXTIMEOVER )				/* Check Receive TimeOver				*/
				{
					OpSif.SqStep = SQSTEP_RXTMOVER;			/* ��M�^�C���I�[�o����STEP�ɑJ��		*/
				}
			}
			break;
		case SQSTEP_RX2ND:	/*---------------------------- ��M���b�Z�[�W����͕��܂Ŏ�M(2nd���)--*/
			if( KPX_SIF2_RxDMA_CNT( ) != OpSif.RxDbcRv )	/* ��M�`�F�b�N							*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				/*----------------------------------------------------------------------------------*/
				rxcnt = OpSif.RxDbcSv - OpSif.RxDbcRv;		/* Receive Count						*/
				if( rxcnt > OpSif.ConfbLength )				/* ���b�Z�[�W����`���܂Ŏ�M ?			*/
				{
					OpSif.RcvbLength = opMsg2ndAnalyze(OpSif.RxBuf, OpSif.MbusTblIndex);
					OpSif.SqStep = SQSTEP_RXLAST;			/* �ŏI��MSTEP�ɑJ��					*/
				}
			}
			else											/* ��M�Ȃ�								*/
			{
				timer = KpiGetShortTimer(&OpSif.RxTimer);	/* Get Timer							*/
				if( timer > SIF_RXTIMEOVER )				/* Check Receive TimeOver				*/
				{
					OpSif.SqStep = SQSTEP_RXTMOVER;			/* ��M�^�C���I�[�o����STEP�ɑJ��		*/
				}
			}
			break;
		case SQSTEP_RXLAST:	/*---------------------------- ���b�Z�[�W�S�̎�M ----------------------*/
			if( KPX_SIF2_RxDMA_CNT( ) != OpSif.RxDbcRv )	/* ��M�`�F�b�N							*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				/*----------------------------------------------------------------------------------*/
				rxcnt = OpSif.RxDbcSv - OpSif.RxDbcRv;		/* Receive Count						*/
				if( rxcnt >= OpSif.RcvbLength )				/* Receive Complete ?					*/
				{
					opSifStopxRxDMA( );						/* Stop RxDMA							*/
					KpiRstShortTimer( &OpSif.SxTimer );		/* Reset SxTimer						*/
					OpSif.SqStep = SQSTEP_WAITSTX;			/* Adr=0 or ���ǈ�:���M�J�n�҂���		*/
				}
			}
			else											/* ��M�Ȃ�								*/
			{
				timer = KpiGetShortTimer(&OpSif.RxTimer);	/* Get Timer							*/
				if( timer > SIF_RXTIMEOVER )				/* Check Receive TimeOver				*/
				{
					OpSif.SqStep = SQSTEP_RXTMOVER;			/* ��M�^�C���I�[�o����STEP�ɑJ��		*/
				}
				/*----------------------------------------------------------------------------------*/
				if( KPX_SIF2_CHK_RxDMA_TC( ) )				/* �c�l�`��~�`�F�b�N					*/
				{
					KPX_TC01_RxDMA_BUGFIX( );					/* @@@V850E/MA1�s��Ή�@@@			*/
					OpSif.ErrFlag = TRUE;					/* �r�h�e�G���[�t���O�Z�b�g				*/
					SIF2_SETERR( 0xFF );					/* Set ErrCode and ++ErrCnt				*/
					OpSif.SqStep = SQSTEP_INITIAL;			/* ����STEP�ɑJ��						*/
				}
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
/*		�`������M�^�C���I�[�o�ُ폈��																*/
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_RXTMOVER: /*-------------------------- ��M�^�C���I�[�o ------------------------*/
			OpSif.RcvbLength = OpSif.RxDbcSv - OpSif.RxDbcRv;
			OpSif.ErrFlag = TRUE;						/* �r�h�e�G���[�t���O�Z�b�g					*/
			opSifStopxRxDMA( );							/* Stop RxDMA								*/
			OpSif.SqStep = SQSTEP_INITIAL;				/* Goto Initial Step						*/
			break;
		default: /*--------------------------------------- �f�t�H���g ------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/

		return;
}



/****************************************************************************************************/
/*																									*/
/*		�r�h�e��M�c�l�`�X�^�[�g����																*/
/*																									*/
/****************************************************************************************************/
static void	opSifStartRxDMA(const UCHAR * pRxBuf, USHORT rxSz )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`�A�h���X�ݒ� : �ʏ�`�����[�h															*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_SETX_RxDMA( pRxBuf, rxSz );			/* Set DMA CH0 Address/Count			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`���r�h�e�X�^�[�g����																	*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_ENABLE_RxDMA( );						/* Enable RxDMA								*/
		KPX_SIF2_RXENABLE( );							/* Enable SIF Receive						*/
/*--------------------------------------------------------------------------------------------------*/
		return;

}


/****************************************************************************************************/
/*																									*/
/*		�r�h�e��M�c�l�`�X�g�b�v����																*/
/*																									*/
/****************************************************************************************************/
static void	opSifStopxRxDMA( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�r�h�e�X�g�b�v����																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_RXDISABLE( );							/* Disable SIF Receive						*/
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`�X�g�b�v����								@@@ V850E/MA1�s��΍� @@@					*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_DI( );										/* Disable Interrupt						*/
	/*----------------------------------------------------------------------------------------------*/
		KPX_STOP_DMA01_BUGFIXA( );						/* BugFix for V850E/MA1						*/
	/*----------------------------------------------------------------------------------------------*/
		KPX_SIF2_STOP_RxDMA( );							/* Stop SIF RxDMA							*/
		KPX_SIF2_STOP_TxDMA( );							/* Stop SIF TxDMA							*/
	/*----------------------------------------------------------------------------------------------*/
		KPX_STOP_DMA01_BUGFIXB( );						/* BugFix for V850E/MA1		 				*/
	/*----------------------------------------------------------------------------------------------*/
		KPI_EI( );										/* Enable Interrupt							*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�r�h�e���M�c�l�`�X�^�[�g����																*/
/*																									*/
/****************************************************************************************************/
static void	opSifStartTxDMA(const UCHAR * pTxBuf, USHORT transSz  )
{
		KPX_SIF2_TXENABLE( );							/* SIF Transmit Enable						*/
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`�]���A�h���X���]���J�E���g�ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_SETX_TxDMA( pTxBuf,  transSz);
/*--------------------------------------------------------------------------------------------------*/
/*		�c�l�`�]���J�n																				*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_ENABLE_TxDMA( );						/* Enable SIF TxDMA							*/
		KlibWaitus( 1 );								/* Wait 1 us								*/
		KPX_SIF2_SWTRIG_TxDMA( );						/* Start SIF TxDMA (Software Trigger)		*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}







/*--------------------------------------------------------------------------------------------------*/
/*		�T�|�[�g�R�}���h�o�^�e�[�u��																*/
/*--------------------------------------------------------------------------------------------------*/
/*	��`���@�F�ȉ��̂悤�Ƀ��X�|���X���b�Z�[�W�������߂���悤�ɒ�`����							*/
/*																									*/
/*		���X�|���X���b�Z�[�W�o�C�g�� = FixPktSz + AltSz*AltDataSz									*/
/*			( AltSz = ���X�|���X��AltSzPos�Ŏw�肵���o�C�g�ʒu�̒l (AltSzLen�̃T�C�Y�Ń��[�h) )		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct tagMBUSRSP_TBL {
		UCHAR	FuncCode;			/* �t�@���N�V�����E�R�[�h��`									*/
		USHORT	FuncSubCode;		/* �t�@���N�V�����E�T�u�R�[�h��`								*/
		UCHAR	FixPktSz;			/* �Œ蕔�̃o�C�g�T�C�Y											*/
		UCHAR	AltSzPos;			/* �ϒ��f�[�^����`�o�C�g�ʒu									*/
		UCHAR	AltSzLen;			/* �ϒ��f�[�^���̃f�[�^��										*/
		UCHAR	AltDataSz;			/* �ϒ��f�[�^�P�ʁi�o�C�g���j									*/
} MBUSRSP_TBL;
static const MBUSRSP_TBL MbusRspTbl[] = {
/*--------------------------------------------------------------------------------------------------*/
/*	���C���R�[�h		�T�u�R�[�h			�Œ蒷		��`�ʒu	��`�T�C�Y	�f�[�^�P��			*/
/*--------------------------------------------------------------------------------------------------*/
/*	  �W���R�}���h�o�^�e�[�u��																		*/
	{ S_FCID_SREG_RD,	0x0000,				0x05,		0x02,		0x01,		0x01	},
	{ S_FCID_SREG_WR,	0x0000,				0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_LOOPBACK,	0x0000,				0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_PREG_WR,	0x0000,				0x08,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
/*	  �g���R�}���h(�P�U�r�b�g�A�N�Z�X�p)�o�^�e�[�u��												*/
/*--------------------------------------------------------------------------------------------------*/
	{ EX16_FCID,		E_SFCID_REG_RD,		0x08,		0x04,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_SREG_WR,	0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_DCREG_RD,	0x08,		0x04,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_DCREG_WR,	0x08,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_PREG_WR,	0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_SREG_RD,	0x08,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_MAXSIZE_RD,	0x0A,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
	{ EX16_FCID,		E_SFCID_REG_RD_SL,	0x0A,		0x06,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_SREG_WR_SL,	0x0C,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_DCREG_RD_SL,0x0A,		0x06,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_DCREG_WR_SL,0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_PREG_WR_SL,	0x0C,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_SREG_RD_SL,	0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_MAXSIZE_RD_SL,0x0C,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
/*	  �g���R�}���h(�R�Q�r�b�g�A�N�Z�X�p)�o�^�e�[�u��												*/
/*--------------------------------------------------------------------------------------------------*/
	{ EX32_FCID,		E_SFCID_REG_RD,		0x08,		0x04,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_SREG_WR,	0x0C,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_DCREG_RD,	0x08,		0x04,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_DCREG_WR,	0x08,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_PREG_WR,	0x0C,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_SREG_RD,	0x08,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_MAXSIZE_RD,	0x0A,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_MEM_RD,		0x08,		0x04,		0x02,		0x02	},
	{ EX32_FCID,		E_SFCID_MEM_WR,		0x0C,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
	{ EX32_FCID,		E_SFCID_REG_RD_SL,	0x0A,		0x06,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_SREG_WR_SL,	0x0E,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_DCREG_RD_SL,0x0A,		0x06,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_DCREG_WR_SL,0x0A,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_PREG_WR_SL,	0x0E,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_SREG_RD_SL,	0x0A,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_MAXSIZE_RD_SL,0x0C,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
/*	  �_�E�����[�h�R�}���h�o�^�e�[�u��																*/
/*--------------------------------------------------------------------------------------------------*/
	{ S_FCID_SYSDL,		S_SFCID_DLID_RD,	0x14,		0x06,		0x01,		0x04	},
	{ S_FCID_SYSDL,		S_SFCID_SWVER_RD,	0x0E,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DLWAY_RD,	0x0C,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_START,	0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGDEL,	0x0E,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGWR,	0x10,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_DELSND,	0x0E,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGSND,	0x10,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGWRA,	0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_END,		0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGRD,	0x10,		0x08,		0x02,		0x01	},
	{ S_FCID_SYSDL,		S_SFCID_DL_REBOOT,	0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_SIFCHG,	0x0C,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_HIS_RD,	0x0A,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_HIS_WR,	0x08,		0x00,		0x00,		0x00	},
	/* Delimiter Code.  Don't Change!! */
	{ 0xFF,				0xFFFF,				0xFF,		0xFF,		0xFF,		0xFF	}
};





/****************************************************************************************************/
/*																									*/
/*		�ʐM�J�n��Q��ڂ̃��b�Z�[�W�����															*/
/*																									*/
/*			���b�Z�[�W�����m��\�ȕ����܂Ŏ�M�����ケ�̊֐���CALL���܂��B						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/****************************************************************************************************/
static USHORT	opMsg2ndAnalyze(const UCHAR *pRcvBuf, UCHAR TblIdx)
{
	LONG  pos; 
	ULONG  AltSz = 0;
	const MBUSRSP_TBL *tblptr = &MbusRspTbl[TblIdx];

	pos = (LONG)tblptr->AltSzPos;
	switch(tblptr->AltSzLen)
	{
	case 1: AltSz = pRcvBuf[pos]; 
		break;
	case 2: AltSz = ((ULONG)pRcvBuf[pos]<<8) | (ULONG)pRcvBuf[pos+1]; 
		break;
	case 4: AltSz =  ((ULONG)pRcvBuf[pos]<<24) | ((ULONG)pRcvBuf[pos+1]<<16)
				   | ((ULONG)pRcvBuf[pos+2]<< 8) | ((ULONG)pRcvBuf[pos+3]); 
		break;
	default: break;
	}

	return ((USHORT)(tblptr->FixPktSz + (AltSz * tblptr->AltDataSz)));
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUS�ʐM�J�n�㏉��̃��b�Z�[�W�����														*/
/*																									*/
/*			MEMOBUS�͉ϒ����b�Z�[�W�ł��邪�ASigmaMessage�Ƃ͈قȂ蒼�ڃ��b�Z�[�W����\���f�[�^��	*/
/*			�Ȃ��B�]���āA��M�r���Ńp�P�b�g�^�C�v��f�[�^���ɂ�胁�b�Z�[�W��(��M�f�[�^��)���v�Z	*/
/*			����K�v������B																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		(1)�T�|�[�g�R�}���h���ǂ����̔���															*/
/*		(2)���b�Z�[�W��(��M�f�[�^��)�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
static USHORT	opMsg1stAnalyze(const UCHAR *pRcvBuf, UCHAR *TblIdx, UCHAR *ConfBLen)
{
	USHORT	i, rc = 0; 
	UCHAR	code = pRcvBuf[1]; 
	USHORT	subcode = (((USHORT)pRcvBuf[2]<<8) | (USHORT)pRcvBuf[3]);
	const MBUSRSP_TBL *tblptr = MbusRspTbl;

	*ConfBLen = 0;	/* ��M���b�Z�[�W����`�ӏ��̃o�C�g�ʒu		*/
	*TblIdx = 0;	/* MEMOBUS�e�[�u���A�N�Z�X�C���f�b�N�X		*/
/*--------------------------------------------------------------------------------------------------*/
/*	�t�@���N�V�����T�u�R�[�h�̌���																	*/
/*--------------------------------------------------------------------------------------------------*/
	rc = MBUS_MAXSIZE;										/* ��M���b�Z�[�W����͖����Z�b�g		*/
	for( i = 0 ; (i < MCOM_MAX_REGIST) && (tblptr[i].FuncCode != 0xff) ; i++ )
	{
		if( (( code == STD_FCID )&&( tblptr[i].FuncCode == code )) 
			|| (( tblptr[i].FuncCode == code )&&( tblptr[i].FuncSubCode == subcode )) )
		{/*	�Y���R�[�h�������̏���																	*/

			if(tblptr[i].AltDataSz != 0)
			{/*	�ϒ��p�P�b�g�̏ꍇ																*/
				*ConfBLen = tblptr[i].AltSzPos + tblptr[i].AltSzLen;
				rc = 0;										/* ��M���b�Z�[�W����͖����Z�b�g		*/
			}
			else
			{/*	�Œ蒷�p�P�b�g�̏ꍇ																*/
				rc = tblptr[i]. FixPktSz;					/* ��M���b�Z�[�W���L��					*/
			}
			break;											/* �������������ɂ��ALOOP�I��			*/
		}
	}
	*TblIdx = (UCHAR)i;										/* MEMOBUS�e�[�u���A�N�Z�X�C���f�b�N�X		*/
/*--------------------------------------------------------------------------------------------------*/
/*	�t�@���N�V�����G���[																			*/
/*--------------------------------------------------------------------------------------------------*/
	if((code & 0x80) != 0)
	{
		rc = 5;
	}

	return( rc );
}







