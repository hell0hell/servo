/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MsgIfDef.h : Message Interface Define Header File											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Message Interface Data Definition														*/
/*																									*/
/*		2. Message Interface Macro Definition														*/
/*																									*/
/*		3.																							*/
/*																									*/
/*		4.																							*/
/*																									*/
/*		5.																							*/
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
/*		Rev.1.00 : 2008.06.11  T.Taniguchi															*/
/*		Rev.1.01 : 2013.04.15  K.Ozaki																*/
/*		Rev.1.02 : 2013.08.10  K.Ozaki		##Panel Operator##										*/
/*																									*/
/*																									*/
/****************************************************************************************************/

#ifndef		MSFGIF_H_
#define		MSFGIF_H_

/****************************************************************************************************/
/*																									*/
/*		Define Function Code, Error Code, etc														*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Main Function Code Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MFC_NONE			0x00				/* �s�g�p											*/
#define	MFC_LOOPBACK		0x08				/* ���[�v�o�b�N�e�X�g								*/
#define MFC_SVXMSG16		0x40				/* �T�[�{�g�����b�Z�[�W								*/
#define MFC_SVXMSG32		0x41				/* �T�[�{�g�����b�Z�[�W								*/
#define	MFC_MECHATROLINK	0x42				/* MECHATROLINK Message								*/
#define	MFC_SYSDWNLD		0x50				/* �V�X�e���_�E�����[�h								*/
#define	MFC_RELAY 			0x51				/* ���p�R�}���h										*/

/*--------------------------------------------------------------------------------------------------*/
/*		Sub Function Code Definition for 40H/41H													*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_RD_ONEREG		0x40				/* �P�ꃌ�W�X�^�Ǐo��								*/
#define	SFC_WR_ONEREG		0x06				/* �P�ꃌ�W�X�^������								*/
#define	SFC_RD_SEQREG		0x03				/* �A�����W�X�^�Ǐo��								*/
#define	SFC_WR_SEQREG		0x10				/* �A�����W�X�^������								*/
#define	SFC_RD_RANREG		0x0D				/* ��A�����W�X�^�Ǐo��								*/
#define	SFC_WR_RANREG		0x0E				/* ��A�����W�X�^������								*/
#define	SFC_RD_MAXNUM		0x41				/* �ő僌�W�X�^���Ǐo��							*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_RD_SEQMEM		0x50				/* �A���������Ǐo��									*/
#define	SFC_WR_SEQMEM		0x51				/* �A��������������									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Sub Function Code Definition for 42H														*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_RD_REQREG_42	0x01				/* �������̓ǂݏo��										*/
#define	SFC_WR_SEQREG_42	0x02				/* �������̏�������										*/
#define	SFC_RD_RANREG_42	0x03				/* �������̓ǂݏo��(��A��)								*/
#define	SFC_WR_RANREG_42	0x04				/* �������̏�������(��A��)								*/
#define	SFC_RD_MAXNUM_42	0x11				/* �ő僁�b�Z�[�W�T�C�Y�ǂݏo��							*/
#define	SFC_RD_PRDCTID_42	0x7F				/* �@����ǂݏo��									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS42_HEADERSIZE	0x08				/* 42H�R�}���h�w�b�_�[���T�C�Y								*/
#define	MBUS42_ADRSIZE		0x04				/* 42H�R�}���h�A�h���X���T�C�Y								*/
#define MBUS42_DATASIZEL	0x04				/* 42H�R�}���h�f�[�^���T�C�Y�i��A���������ݎ��j				*/
#define	MBUS42_CRC_SIZE		0x02				/* 42H�R�}���hCRC���T�C�Y								*/
#define	MBUS42_DTYPE_B		0x01				/* �f�[�^�^�FByte�^(8bit)								*/
#define	MBUS42_DTYPE_S		0x02				/* �f�[�^�^�FShort�^(16bit)							*/
#define	MBUS42_DTYPE_L		0x03				/* �f�[�^�^�FLong�^(32bit)								*/
/* <S150> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/*		Sub Function Code Definition for 51H														*/
/*--------------------------------------------------------------------------------------------------*/
#define MBUS51_FIX_SIZE			0x0E			/* ���p�R�}���h�Œ�T�C�Y								*/
#define MBUS51_PASS_DATA_SIZE	0x08			/* ���p�R�}���h�σT�C�Y(���̃T�C�Y�̔{���ŕω�)				*/
#define MBUS51_MAX_ROUTENUM		0x0A			/* ���p�i���@�ő�l									*/
/* <<<<< <S150> */


/*--------------------------------------------------------------------------------------------------*/
/*		Download			SubFuncCode		   S:Support, -:Not Support								*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_DLID_RD			0x0000				/* S : �h�c���Ǐo��								*/
#define	SFC_SWVER_RD		0x0001				/* S : ��۸��я����ݏ����擾						*/
#define	SFC_DLWAY_RD		0x0002				/* S : ��۸��я����ݏ����擾						*/
#define	SFC_DL_START		0x0003				/* S : �޳�۰�ފJ�n									*/
#define	SFC_DL_PRGDEL		0x0004				/* S : ��ۯ�����									*/
#define	SFC_DL_PRGWR		0x0005				/* S : ��۸��я�����								*/
#define	SFC_DL_DELSND		0x0006				/* - : ��ۯ��������(�������]���̂�.���������Ȃ�)	*/
#define	SFC_DL_PRGSND		0x0007				/* - : ��۸��ё��M(���M�̂�.���������Ȃ�)			*/
#define	SFC_DL_PRGDELA		0x0008				/* - : ��ۯ��ꊇ�����w��							*/
#define	SFC_DL_PRGWRA		0x0009				/* - : ��۸��шꊇ�����ݎw��						*/
#define	SFC_DL_END			0x000A				/* S : �޳�۰�ޏI���w��								*/
#define	SFC_DL_PRGRD		0x000B				/* S : ��۸��ѓǏo���w��							*/
#define	SFC_DL_REBOOT		0x000C				/* S : ��۸��эċN���w��							*/
#define	SFC_DL_SIFCHG		0x000D				/* S : �رقh�e���x�ύX								*/
#define	SFC_DL_HIS_RD		0x000E				/* S : �޳�۰�ޗ���Ǐo��							*/
#define	SFC_DL_HIS_WR		0x000F				/* S : �޳�۰�ޗ���Ǐ�����							*/
/*--------------------------------------------------------------------------------------------------*/
/*		Error Code Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define ECD_ERR_MFC			0x00				/* Error : Main Function Code						*/
#define ECD_ERR_SFC			0x00				/* Error : Sub  Function Code						*/
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMemobus Error Code Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SMBUS_EC_FNCDERR	(0x01 | 0x200)		/* �t�@���N�V�����R�[�h�ُ�							*/
#define	SMBUS_EC_REGNOERR	(0x02 | 0x200)		/* ���W�X�^�ԍ��s��									*/
#define	SMBUS_EC_DANUMERR	(0x03 | 0x200)		/* ���s�ǈُ�										*/
/*--------------------------------------------------------------------------------------------------*/
#define	SMBUS_SEC_REGNOERR	(0x30 | 0x200)		/* ���W�X�^�ԍ��s��(���x����)						*/
#define	SMBUS_SEC_ACCSERR	(0x31 | 0x200)		/* �A�N�Z�X�����G���[								*/
#define	SMBUS_SEC_DALMTERR	(0x32 | 0x200)		/* �ݒ�l�͈͊O�G���[								*/
#define	SMBUS_SEC_DAMTHERR	(0x33 | 0x200)		/* �f�[�^�����G���[ 								*/
#define	SMBUS_SEC_CONDERR	(0x34 | 0x200)		/* �����G���[										*/
#define	SMBUS_SEC_INTLOCK	(0x35 | 0x200)		/* ���������G���[									*/
/*--------------------------------------------------------------------------------------------------*/
/*		���M�J�n���̉����E��������`																*/
/*--------------------------------------------------------------------------------------------------*/
#define	RESMT_COMPLETE		0x01				/* MsgType : ��������								*/
#define	RESMT_NORESPONSE	0x00				/* MsgType : ������(���b�Z�[�W�ُ�)					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�V�O�}���b�Z�[�W�G���[�R�[�h��`															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SMSGEC_FNCDERR		0x10				/* �t�@���N�V�����R�[�h�ُ�							*/
#define	SMSGEC_SEQXERR		0x10				/* �ʐM�V�[�P���X�ُ�(�p���`��)		@@@�d�l����@@@	*/
#define	SMSGEC_CMDXERR		0x14				/* �_�E�����[�h�J�n�R�}���h�ُ�		@@@�d�l����@@@	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�⏕�@�\�����z�[���h																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	REGRES_NOHOLD		0x00				/* �⏕�@�\�����z�[���h����			@@@�������v@@@	*/
#define	REGRES_HOLD			0x01				/* �⏕�@�\�����z�[���h				@@@�������v@@@	*/

/****************************************************************************************************/
/*																									*/
/*		Register IF Data Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�h�e�֐�����߂�l��`																*/
/*--------------------------------------------------------------------------------------------------*/
#define	REGIF_COMPLETE		0x00		/* ��������							*/
#define	REGIF_CONTINUE		0x01		/* �����p��							*/
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�h�e�֐��ُ�߂�l��`(�V�O�}���b�Z�[�W�G���[�R�[�h�ƌ��p)	*/
/*--------------------------------------------------------------------------------------------------*/
#define	REGIF_CMDERR		0x10		/* ���W�X�^�h�e�R�}���h�ُ�			*/
#define	REGIF_REGNOERR		0x11		/* ���W�X�^�ԍ��ُ� 				*/
#define	REGIF_DALMTERR		0x12		/* �f�[�^�ُ�(�㉺���ُ�)   		*/
#define	REGIF_DACALERR		0x13		/* �f�[�^�ُ�(���Z�ُ�) 			*/
#define	REGIF_DAMTHERR		0x14		/* �f�[�^�������ُ� 				*/
#define	REGIF_DAMTHERR2		(0x14 | 0x100)		/* �f�[�^�������ُ� 		*/
#define	REGIF_DANUMERR		0x15		/* �f�[�^���ُ�   				*/
#define	REGIF_FOPEERR1		0x30		/* ���b�g�ɂ�鑀�샂�[�h��L�� 	*/
#define	REGIF_FOPEERR2		0x31		/* ���b�g�ɂ�鑀�샂�[�h��L����	*/
#define	REGIF_FOPEERR3		0x3F		/* ���b�g�Ƃ̏�������				*/
#define	REGIF_FOPEERR4		0x3F		/* �⏕�@�\���s�ُ�					*/


/****************************************************************************************************/
/*																									*/
/*		Message Interface Struct Definition															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Hmsg���ʗpFlag�ݒ�	|	PnlOpe	LcdOpe	SifMsg	UsbMsg	NetMsg  LnkMsg						*/
/*	  ----------------------+-------------------------------------------------------------------	*/
/*		XxxMsg.f.OpeMsg		|	TRUE	TRUE														*/
/*		XxxMsg.f.PopMsg		|	TRUE																*/
/*		XxxMsg.f.LopMsg		|			TRUE														*/
/*	  ----------------------+-------------------------------------------------------------------	*/
/*		XxxMsg.f.ComMsg		|					TRUE	TRUE	TRUE	TRUE						*/
/*		XxxMsg.f.SifMsg		|					TRUE												*/
/*		XxxMsg.f.UsbMsg		|							TRUE										*/
/*		XxxMsg.f.NetMsg		|									TRUE								*/
/*		XxxMsg.f.LnkMsg		|											TRUE						*/
/*	  ----------------------+-------------------------------------------------------------------	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
typedef	struct	_MSGIF {						/* Message IF Definition 							*/
	/*----------------------------------------------------------------------------------------------*/
		struct	{								/* TODO:Hmsg���ʗp�t���O							*/
		UINT	OpeMsgIF		:1;				/* �I�y���[�^���ʃt���O								*/
		UINT	PopMsgIF		:1;				/* �p�l���I�y���[�^���ʃt���O						*/
		UINT	LopMsgIF		:1;				/* �k�b�c�I�y���[�^���ʃt���O						*/
		UINT	XxxOpeIF		:1;				/* �\��												*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	ComMsgIF		:1;				/* �ʐM���b�Z�[�W���ʃt���O							*/
		UINT	SifMsgIF		:1;				/* �V���A���ʐM���b�Z�[�W���ʃt���O					*/
		UINT	UsbMsgIF		:1;				/* �t�r�a�ʐM���b�Z�[�W���ʃt���O					*/
		UINT	NetMsgIF		:1;				/* �l�b�g���[�N���b�Z�[�W���ʃt���O					*/
		UINT	LnkMsgIF		:1;				/* �����N�ʐM���b�Z�[�W���ʃt���O					*/
		UINT	MlnkMsgIF		:1;				/* ���J�g�������N�ʐM���b�Z�[�W���ʃt���O			*//*<MEMO_001T>*/
	/*----------------------------------------------------------------------------------------------*/
//		UINT	SpareFlags		:23;			/* Spare											*/
		UINT	SpareFlags		:22;			/* Spare											*//*<S150>*/
		} f;									/*													*/
/*--------------------------------------------------------------------------------------------------*/
	CSIF   *Sif;								/* Pointer to the Serial interface					*/
	UCHAR  *CmdBuf;								/* Pointer to the Command Message Buffer			*/
	UCHAR  *ResBuf;								/* Pointer to the Response Message Buffer			*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RcvBufSize;							/* Receive Message Buffer Size						*/
	USHORT	SndBufSize;							/* Send Message Buffer Size							*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CmdMsgLen;							/* Command Message Length							*/
	USHORT	AccessLvl;							/* Access Level USER1/USER2/SYSTEM)					*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ComMode;							/* Communication Mode (LCDOPE,MSG,etc)				*/
	USHORT	ComPortType;						/* Channel Information ( not used )					*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ErrRno;								/* Error Register Number							*/
	USHORT	LstErrRno;							/* Last Error Register Number						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	FunSts;								/* Function Execution Status						*/
	UCHAR	LastAxisIdx;						/* Multi-Axis : Last Axis Index						*/
	UCHAR	ReserveChar0;						/* Reserved											*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	ComChkTime;							/* Timeout Timer									*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
	AXIS_HANDLE *pAxRsc;						/* TODO: ���Ƃŏ��� 								*/
#else	/*_MERCURY_PLATFORM_*/
	void		 *pAxRsc;						/* TODO: ���Ƃŏ��� 								*/
#endif	/*_MERCURY_PLATFORM_*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	OpeDspMode;							/* �I�y���[�^�̃��[�U�[�萔�\���I��					*/	/* <S113> */
	USHORT	LstOpeDspMode;						/* �I�y���[�^�̃��[�U�[�萔�\���I��					*/	/* <S197> */
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
	MEMOBUS_ADDR_MAP	*AxisMap;				/* <S150> */
#else	/*_MERCURY_PLATFORM_*/
	void	*AxisMap;				/* <S150> */
#endif	/*_MERCURY_PLATFORM_*/
//	LONG  (*StxFunc)( UCHAR *, USHORT );		/* TODO: �������b�Z�[�W���M�J�n�`�o�h�֐�	*/
//	void  (*ResFunc)( struct _MSGIF * );		/* TODO: �e�����s���ʉ������b�Z�[�W�����֐�*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
//	USHORT	FunExeIdx;							/* TODO:�⏕�@�\���s�e�[�u���w��C���f�b�N�X	*/
//	USHORT	FnStsToOpi;							/* TODO:�⏕�@�\���s����(OPTION-SERVO I/F�p)	*/
/*----------------------------------------------------------------------------------------------*/
//	USHORT	FunResHold;							/* TODO:�⏕�@�\�����z�[���h				*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
//	USHORT	ComTimeShort;						/* TODO:�I�v�V�����ʐM�^�C���A�E�g�v���p	*/
//	USHORT	RcvWait;							/* TODO:SC���b�Z�[�W�ʐM��M�҂��t���O	*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
//	void *PrmDef;								/* TODO: Pointor to the Parameter Definition 	*/	
//	void *MonDef;								/* TODO: Pointer to the Monitor Definition 		*/	
//	void *FunDef;								/* TODO: Pointer to the Fn Definition 			*/
//TODO: void *AxDef;							/* TODO: Pointer to the Axis Definition			*/
} MSGIF;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*//* <S0E4> */
/****************************************************************************************************/
/*		EXTERN LIST																					*/
/****************************************************************************************************/
/* M-3 Message Interface */
extern	MSGIF	LopMsg;					/* LCD Operator Messsage I/F 						*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
#endif
typedef	MSGIF*	HMSGIF;							/* ����p�n���h����`				*/

#if 0											
/*--------------------------------------------------------------------------------------------------*/
/* 
 * �ȉ�Jupiter(�Q�l�p)								
 */
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	_MSGIF {						/* ���b�Z�[�W�h�e�p�\���̒�`						*/
/*--------------------------------------------------------------------------------------------------*/
		struct	{								/* Hmsg���ʗp�t���O(���������ɐݒ�)					*/
		UINT	OpeMsgIF		:1;				/* �I�y���[�^���ʃt���O								*/
		UINT	PopMsgIF		:1;				/* �p�l���I�y���[�^���ʃt���O						*/
		UINT	LopMsgIF		:1;				/* �k�b�c�I�y���[�^���ʃt���O						*/
		UINT	XxxOpeIF		:1;				/* �\��												*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	ComMsgIF		:1;				/* �ʐM���b�Z�[�W���ʃt���O							*/
		UINT	SifMsgIF		:1;				/* �V���A���ʐM���b�Z�[�W���ʃt���O					*/
		UINT	UsbMsgIF		:1;				/* �t�r�a�ʐM���b�Z�[�W���ʃt���O					*/
		UINT	NetMsgIF		:1;				/* �l�b�g���[�N���b�Z�[�W���ʃt���O					*/
		UINT	LnkMsgIF		:1;				/* �����N�ʐM���b�Z�[�W���ʃt���O					*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	SpareFlags		:23;			/* Spare											*/
		} f;									/*													*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR  *CmdBuf;							/* �w�߃��b�Z�[�W�o�b�t�@�|�C���^					*/
		UCHAR  *ResBuf;							/* �������b�Z�[�W�o�b�t�@�|�C���^					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	CmdBufSize;						/* �w�߃��b�Z�[�W�o�b�t�@�T�C�Y(Byte��)				*/
		USHORT	ResBufSize;						/* �������b�Z�[�W�o�b�t�@�T�C�Y(Byte��)				*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	ComMode;						/* �ʐM���[�h(�ڑ���/��ڑ�/etc)					*/
		UCHAR	AccessLvl;						/* �A�N�Z�X���x��(USER1/USER2/SYSTEM)				*/
		USHORT	CmdMsgLen;						/* �w�߃��b�Z�[�W��									*/
		ULONG	ComChkTime;						/* �ʐM�^�C���A�E�g�v���p							*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	FunIdx;							/* �⏕�@�\�e�[�u���C���f�b�N�X						*/
		USHORT	FunSts;							/* �⏕�@�\���s����									*/
		USHORT	ErrRno;							/* �ُ탌�W�X�^�ԍ�									*/
		USHORT	LstErrRno;						/* �O��R�}���h�ُ̈탌�W�X�^�ԍ�					*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	FunResHold;						/* �⏕�@�\�����z�[���h								*/
		UCHAR	LastAxisIdx;					/* �����h���C�u�F�O�񎲃C���f�N�X					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	DldAllPrmRdCount;				/* Read  Count  for Download All Parameter			*/
		USHORT	DldAllPrmRdChkSum;				/* Read  ChkSum for Download All Parameter			*/
		USHORT	DldAllPrmWrCount;				/* Write Count  for Download All Parameter			*/
		USHORT	DldAllPrmWrChkSum;				/* Write ChkSum for Download All Parameter			*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	LopAccessLvl;					/* LCD Operator access level for Converter <0BJ14>	*/
} MSGIF, *HMSGIF;								/* �\���̌^�����n���h����`							*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*
 * 	Message Handle Check Macro Definition
 *
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_CHK_OpeHmsg( Hmsg )		( (Hmsg)->f.OpeMsgIF )
#define	KPI_CHK_LopHmsg( Hmsg )		( (Hmsg)->f.LopMsgIF )
#define	KPI_CHK_ComHmsg( Hmsg )		( (Hmsg)->f.ComMsgIF )
#define	KPI_CHK_SifHmsg( Hmsg )		( (Hmsg)->f.SifMsgIF )
#define	KPI_CHK_UsbHmsg( Hmsg )		( (Hmsg)->f.UsbMsgIF )
#define	KPI_CHK_NetHmsg( Hmsg )		( (Hmsg)->f.NetMsgIF )
#define	KPI_CHK_LnkHmsg( Hmsg )		( (Hmsg)->f.LnkMsgIF )
#define	KPI_CHK_PopHmsg( Hmsg )		( (Hmsg)->f.PopMsgIF )

/*--------------------------------------------------------------------------------------------------*/
/*
 *	Massage Handle Initialization
 *
 *  KRI_INIT_MSGIF() : Initialize Message Handle
 *  KRI_BIND_MSGIF() : Bind a Message Handle to Other Message Handle
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define KRI_INIT_MSGIF( hmsg, csif )	{ 		/* bind CSIF to Message IF */ 	\
		MlibResetLongMemory( hmsg, sizeof(MSGIF)/4 );	/* reset memory */		\
		(hmsg)->Sif = (csif);					/* set sif */			\
		(hmsg)->CmdBuf = (csif)->RxBuf;			/* set cmd buf */ 		\
		(hmsg)->ResBuf = (csif)->TxBuf;			/* set res buf */		\
		(hmsg)->RcvBufSize = (csif)->RcvBufSize;/* set rcv buf size */ \
		(hmsg)->SndBufSize = (csif)->RcvBufSize;/* set send buf size */\
	}
/*--------------------------------------------------------------------------------------------------*/
#define KRI_BIND_MSGIF( hmsg, src )	{ 			/* bind Message IF to other Message IF */ 	\
		(hmsg)->Sif = (src)->Sif;				/* set sif */			\
		(hmsg)->CmdBuf = (src)->CmdBuf;			/* set cmd buf */ 		\
		(hmsg)->ResBuf = (src)->ResBuf;			/* set res buf */		\
		(hmsg)->RcvBufSize = (src)->RcvBufSize; /* set rcv buf size */ \
		(hmsg)->SndBufSize = (src)->RcvBufSize; /* set send buf size */\
	}

/*--------------------------------------------------------------------------------------------------*/
/*
 * Message Handle Flag Initialization
 * 
 * KRI_SET_FLG_LCDOPE() : for Lcd Operator
 * KRI_SET_FLG_SRLMSG() : for UART message 
 * KRI_SET_FLG_USBMSG() : for USB message
 * KRI_SET_FLG_PNLOPE() : for Panel Operator
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define KRI_SET_FLG_LCDOPE(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.OpeMsgIF = 1; \
					(hMsg)->f.LopMsgIF = 1; \
				}
#define KRI_SET_FLG_SRLMSG(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.ComMsgIF = 1; \
					(hMsg)->f.SifMsgIF = 1; \
				}
#define KRI_SET_FLG_USBMSG(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.ComMsgIF = 1; \
					(hMsg)->f.UsbMsgIF = 1; \
				}
#define KRI_SET_FLG_PNLOPE(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.OpeMsgIF = 1; \
					(hMsg)->f.PopMsgIF = 1; \
				}
/* <S150> >>>>> */
#define KRI_SET_FLG_MLNKMSG(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.ComMsgIF = 1; \
					(hMsg)->f.MlnkMsgIF = 1; \
				}
/* <<<<< <S150> */


/*--------------------------------------------------------------------------------------------------*/
/*
 *  Message Check Macro
 *
 *  KPI_CHK_OPEMSG() : LCD Operator
 *  KPI_CHK_SYSDLMSG() : Download Message
 *  KPI_CHK_M3MSG() : Mechatrolink Message
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_CHK_OPEMSG( fc ) ((0x60 <= fc) && (fc <= 0x64))		/* �I�y���[�^���b�Z�[�W�`�F�b�N		*/
#define	KPI_CHK_SYSDLMSG( fc ) (0x50 == fc)						/* �_�E�����[�h���b�Z�[�W�`�F�b�N	*/
#define	KPI_CHK_M3MSG( fc ) (0x42 == fc)						/* M3���b�Z�[�W�`�F�b�N				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Modes (for SerComMode)																*/
/*--------------------------------------------------------------------------------------------------*/
#define SCM_NOCOM		0							/* �ڑ��@��Ȃ�									*/
#define SCM_LCDCOM		1							/* LCD�I�y���[�^								*/
#define SCM_MSGCOM		2							/* Message�@��(PC,etc)							*/
#define SCM_TSTCOM		3							/* Tester�@��(Debug)							*/

/*--------------------------------------------------------------------------------------------------*/
/*		�A�N�Z�X���x��(���ӁF0 �͎g�p�s��)															*/
/*--------------------------------------------------------------------------------------------------*/
#define ACCLVL_USER1			1				/* ���[�U�[�P										*/
#define ACCLVL_USER2			2				/* ���[�U�[�Q										*/
#define ACCLVL_USER3			3				/* ���[�U�[�R										*//* <S0FE> */
//#define ACCLVL_SYSTEM			3				/* �V�X�e��											*//* <S0FE> */
#define ACCLVL_SYSTEM			4				/* �V�X�e��											*//* <S0FE> *//*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*
 * Function Message Definition 
 * 
 * Lcd operator and panel operator use this for executing  function.
 *
 */
/*--------------------------------------------------------------------------------------------------*/
typedef struct _FUNMSG {
	MSGIF	*Super;							/* @@ Super Class (Message Interface) */
/*--------------------------------------------------------------------------------------------------*/
	UINT 	*pKeyMk;						/* Pointer to the Key Make (OFF->ON) Flag */
	UINT 	*pKeyBrk;						/* Pointer to the Key Break(ON->OFF) Flag */
	UINT 	*pKeyLvl;						/* Pointer to the Key Level Flag */
	UINT	*pBlnkKick;						/* Pointer to the Blink Flag */
	UCHAR	*pTxBuf;						/* Pointer to the Display Inforamtion Buffer */
/*--------------------------------------------------------------------------------------------------*/
	void	*pVar;							/* Pointer to the Function common variable area */
} FUNMSG;


/*--------------------------------------------------------------------------------------------------*/
/*
 * Panel Operator Message Definition
 *
 * Command :
 * 		includes panel button information. (RinK.Pnl)
 * 
 * Response :
 * 		includes panel led display code information. (RoutK.PnlLed)
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define PNL_N_LED  (5)
#define PNLCMD_PNLBTN_IDX (0)
typedef struct _PnlCmdMsg {
//	UCHAR Magic[2];						/* Magic Number { 'P', 'O' } */
	UCHAR PnlBtn;						/* Panel Operator Input Button Switch Code */
} PNLCMDMSG;

#define PNLRSP_LEDCODE_IDX (0)
typedef struct _PnlRspMsg {
//	UCHAR Magic[2];						/* Magic Number { 'P', 'O' } */
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * LEDDSP;
	 *	UCHAR	LedCode[5]			:	�\������R�[�h	
	 *	UCHAR	LedBlnkSw[5]		:	1�̕������u�����N�Ώ�
	 */
	/*----------------------------------------------------------------------------------------------*/
	UCHAR LedCode[PNL_N_LED];			/* Panel Operator Output Led Code */
	UCHAR LedBlnkSw[PNL_N_LED];			/* Panel Operator Blink Switch */
} PNLRSPMSG;



#endif /* MSFGIF_H_ */
/***************************************** end of file **********************************************/
