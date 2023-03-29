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
#include	"Monitor.h"							/* Un Monitor */
#include	"Parameter.h"						/* Parameter */
#include 	"Operation.h"						/* Fn Function */
#include 	"Information.h"						/* Information */
#include	"UsbFunctionIF.h"					/* TODO: ��ŏ��� */
#include	"Memory.h"							/* Data Trace */
#include	"FirmDownload.h"					/* FirmDownload(50h)	<S077>	*/
#include	"MemobusFunctions.h"				/* <S150> */

/* <S205>	Start */
/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/
/* �ǂݏo��ID�̃��W�X�^�A�h���XTable */
const LONG IdRegAddTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(�^��)	ID1(�d�l)	ID2(�d�l��)	ID3(�V���A����)	ID4(Soft Ver)	ID5(���W�X�^ Ver)	ID6(BTO ��)	 */
/* 00:�T�[�{�p�b�N */	{	0x80019000,	0x80018000,	0,			0x80019010,		0x8001800A,		0x80018200,			0x800191E0	},
/* 01:CMD */			{	0x800190C8,	0,			0,			0x800190E0,		0x80018082,		0,					0			},
/* 02:Safty */			{	0x800190FA,	0,			0,			0x80019112,		0x8001808A,		0,					0			},
/* 03:FB */				{	0x8001912C,	0,			0,			0x80019144,		0,				0,					0			},
/* 04:���[�^ */			{	0x80019022,	0,			0,			0x80019038,		0,				0,					0			},
/* 05:�G���R�[�_ */		{	0x8001904A,	0,			0,			0x8001905A,		0x8001801C,		0,					0			}
};

/* �ǂݏo��ID�̃f�[�^�T�C�YTable[byte] */
const CHAR IdDataSizeTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(�^��)	ID1(�d�l)	ID2(�d�l��)	ID3(�V���A����)	ID4(Soft Ver)	ID5(���W�X�^ Ver)	ID6(BTO ��)	 */
/* 00:�T�[�{�p�b�N */	{	2,			2,			4,			2,				4,				4,					2			},
/* 01:CMD */			{	2,			0,			0,			2,				4,				0,					0			},
/* 02:Safty */			{	2,			0,			0,			2,				4,				0,					0			},
/* 03:FB */				{	2,			0,			0,			2,				0,				0,					0			},
/* 04:���[�^ */			{	2,			0,			0,			2,				0,				0,					0			},
/* 05:�G���R�[�_ */		{	2,			0,			0,			2,				2,				0,					0			}
};

/* �ǂݏo��ID�̃f�[�^��Table */
const CHAR IdDataNumTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(�^��)	ID1(�d�l)	ID2(�d�l��)	ID3(�V���A����)	ID4(Soft Ver)	ID5(���W�X�^ Ver)	ID6(BTO ��)	 */
/* 00:�T�[�{�p�b�N */	{	8,			1,			1,			8,				1,				1,					32			},
/* 01:CMD */			{	8,			0,			0,			8,				1,				0,					0			},
/* 02:Safty */			{	12,			0,			0,			12,				1,				0,					0			},
/* 03:FB */				{	12,			0,			0,			12,				0,				0,					0			},
/* 04:���[�^ */			{	11,			0,			0,			8,				0,				0,					0			},
/* 05:�G���R�[�_ */		{	8,			0,			0,			8,				1,				0,					0			}
};

/*  ����(0�`9)�̂`�r�b�h�h�R�[�h�e�[�u�� */
const	UCHAR	NumAsciiTbl[ ] =
		{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

/*  �����̂`�r�b�h�h�R�[�h�e�[�u�� */
const	UCHAR	StrAsciiTbl[ ] =
		{ 'F', 'E', 'Y'};
/* <S205>	End */

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
void	RpxMsgifFinishProcedure( HMSGIF Hmsg );			/* MessageI/F�F�I������						*/
#if 0
LONG	RpxMsgifCheckRecvMessage( HMSGIF Hmsg );		/* MessageI/F�F��M���b�Z�[�W�`�F�b�N		*/
#endif
LONG	RpxMsgifGetCmdMsgSubAddr( HMSGIF Hmsg );		/* MessageI/F�FCmdMsg�T�u�A�h���X�擾		*/
void	RpxMsgifSendErrResSubAddr( HMSGIF Hmsg );		/* MessageI/F�FSubAddr�ُ퉞�����M����		*/
void	RpxMsgifFuncExecResponse( HMSGIF Hmsg );		/* MessageI/F�F�⏕�@�\���s���ʉ�������		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Rki�֐���`�FRound API Function for Kernel													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
LONG	RkiMsgifGetCmdMsgLength(						/* �w�߃��b�Z�[�W���擾						*/
		UCHAR	*CmdBuf,								/* Command Message Buffer					*/
		LONG	CmdBufLen	);							/* Command Message Buffer Length			*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Lpx�֐���`�F�֐����ύX���R																	*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusLoopbackTest( HMSGIF Hmsg );				/* Memobus�F���[�v�o�b�N�e�X�g				*/
#if 0
void	LpxMbusSystemDownLoad( HMSGIF Hmsg );			/* Memobus�F�V�X�e���_�E�����[�h			*/
#endif
#if 0	/* <S150> */
void	LpxMbusSysDLRes( HMSGIF Hmsg, SHORT dlSts );	/* Memobus�F�V�X�e���_�E�����[�h���X�|���X�쐬*/
#endif
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadOneRegister( HMSGIF Hmsg );			/* Memobus�F�P�ꃌ�W�X�^�Ǐo��				*/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg );			/* Memobus�F�P�ꃌ�W�X�^������				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg );			/* Memobus�F�A�����W�X�^�Ǐo��				*/
void	LpxMbusWriteSeqRegisters( HMSGIF Hmsg );		/* Memobus�F�A�����W�X�^������				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadRandomRegisters( HMSGIF Hmsg );		/* Memobus�F��A�����W�X�^�Ǐo��			*/
void	LpxMbusWriteRandomRegisters( HMSGIF Hmsg );		/* Memobus�F��A�����W�X�^������			*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadMaxNumberOfRegisters( HMSGIF Hmsg );	/* Memobus�F�ő僌�W�X�^���Ǐo��			*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadSeqMemories( HMSGIF Hmsg );			/* Memobus�F�A���������Ǐo��				*/
void	LpxMbusWriteSeqMemories( HMSGIF Hmsg );			/* Memobus�F�A��������������				*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxGetMaxNumberOfRdRegisters( HMSGIF Hmsg, LONG FixedLen );
LONG	LpxGetMaxNumberOfWrRegisters( HMSGIF Hmsg, LONG FixedLen );
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusSendErrorResponse( HMSGIF Hmsg, ULONG ErrCode );
LONG	LpxRegifRegNoError( LONG RegNo, LONG Number, USHORT *RdBuf, HMSGIF Hmsg );
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S150> */
void	LpxMbusTstUsbProcedure( HMSGIF Hmsg );			/* Ingram��Memobus							*/
void	LpxMbusTstUartProcedure( HMSGIF Hmsg );			/* Ingram��Memobus							*/
#endif	/* <S150> */
/****************************************************************************************************/
/* <S150> >>>>> */
void LpxMbusMechatrolink( HMSGIF Hmsg, LONG *MsgLen );	/* ���J�g�������N�R�}���h					*/
void LpxMbusLink( HMSGIF Hmsg );						/* ���p���b�Z�[�W�t�@���N�V����				*/
static void	mbusFuncReadMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void	mbusFuncWriteMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void	mbusFuncReadMaxSize42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void mbusFuncErrorManager42H( HMSGIF Hmsg, LONG ErrCode, ULONG ErrAddress, LONG *TrnbLength );

static LONG mbusFuncLongSwap( void *Buf );
static LONG mbusFuncShortSwap( void *Buf );

static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize );
static void	mbusFuncSetArrayShortData( UCHAR *Buf, USHORT data, UINT AccessSize );

static LONG mbusFuncSetValueByteData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueShortData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueLongData( UCHAR *Buf, LONG BlockNum, LONG dIndex );

static REG_MANAGER_HANDLE *mbusFuncConvAddress(MEMOBUS_ADDR_MAP *AxisMap, ULONG *MemAddress );
static UCHAR mbusFuncCheckBlockAccess(UCHAR *CmdBuf, ULONG CurrentAddress, 
													UINT AccessSize, LONG BlockNum, LONG dIndex);
/* <<<<< <S150> */

/* <S205> Start */
/*--------------------------------------------------------------------------------------------------*/
/*	���i���ǂݏo��																				*/
/*--------------------------------------------------------------------------------------------------*/
static void mbusFuncReadProductInfo42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void mbusFuncSetSpecialSpecIdData( HMSGIF Hmsg, LONG RspBuffOffset);
static LONG mbusFuncSetIdData( HMSGIF Hmsg, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset, LONG *TrnbLength );
/*--------------------------------------------------------------------------------------------------*/
/* <S205> End */

/*<11J42> add start*/
#if 0 &&  CCSW_CMDID == CMDID_EXIST
void	MemobusRespTrnsStart( HMSGIF, USHORT, USHORT );	/*             ���M�J�n�ݒ�					*/

ULONG	RpxSmsgTransOptRspMessage( HMSGIF );			/* �I�v�V�����J�[�h�ʐM�����f�[�^�]������	*/
void	MemobusTransCmdOptCmdMsg( HMSGIF );				/* CmdOpt�F�R�}���h�f�[�^�]�� (TOOL��OPT)	*/
ULONG	MemobusTransCmdOptRspMsg( HMSGIF );				/* CmdOpt�F���X�|���X�f�[�^�]�� (OPT��TOOL)	*/
#define	SMBUS_MPU_MASK		0xf0
#define	SMBUS_MPU0			0x00
#define	SMBUS_MPU1			0x10
#define	SMBUS_MPU2			0x20
#define	SMBUS_MPU3			0x30
#endif
/*<11J42> add end*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		<S166>																						*/
/*--------------------------------------------------------------------------------------------------*/
static LONG mbusCheckOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg );
static void mbusFreeOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg );
static LONG	mbusGetOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg );
static void	mbusFreeOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg );
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Parameter Register Read/Write																*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadPrmRegister(					/* �p�����[�^�ǂݏo��(���W�X�^�w��)						*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �Ǐo���f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �Ǐo���f�[�^�o�b�t�@									*/
		HMSGIF	Hmsg			);			/* �Ǐo��IF�n���h��										*/
LONG	LpxWritePrmRegister(				/* �p�����[�^�������ݏ���(���W�X�^�w��)					*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �������݃f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �������݃f�[�^�o�b�t�@								*/
		HMSGIF	Hmsg			);			/* �Ǐo��IF�n���h��										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Common Object Register Read/Write															*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadComObject(					/* �p�����[�^�ǂݏo��(���W�X�^�w��)						*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �Ǐo���f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �Ǐo���f�[�^�o�b�t�@									*/
		HMSGIF	Hmsg			);			/* �Ǐo��IF�n���h��										*/
LONG	LpxWriteComObject(					/* �p�����[�^�������ݏ���(���W�X�^�w��)					*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �������݃f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �������݃f�[�^�o�b�t�@								*/
		HMSGIF	Hmsg			);			/* �Ǐo��IF�n���h��										*/
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
typedef	struct	{
	REGFUN	ReadRegister;			/* Read  Register Function										*/
	REGFUN	WriteRegister;			/* Write Register Function										*/
//	UCHAR	MblkWchk;				/* @@ Multi-Block Write Check									*/
//	UCHAR	Spare;					/* @@ Spare														*/
	COBJECT	*pObject;				/* Pointer to the Register Object								*/
} REGFUNTBL;
/*--------------------------------------------------------------------------------------------------*/
const	REGFUNTBL	RegFuncTbl[16] = {
/*	ReadRegister			WriteRegister		 pObject 											*/
/*--------------------------------------------------------------------------------------------------*/
 {	LpxReadPrmRegister,		LpxWritePrmRegister, (COBJECT *)CPARAMX	},	/* 0 : TODO: Parameter Non Volatile		*/
 {	LpxReadPrmRegister,		LpxWritePrmRegister, (COBJECT *)CPARAMX	},	/* 1 : Parameter Volatile		*/
 {	LpxReadComObject,		LpxWriteComObject,	 (COBJECT *)COPEREG },	/* 2 : Operation Register	*/
 {	LpxRegifRegNoError,		LpxRegifRegNoError,	NULL },					/* 3 : TODO: Enginee.  Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 4 : TODO: DataTrace Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 5 : TODO: DataTrace Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 6 : TODO: DataTrace Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 7 : TODO: DataTrace Register	*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* 8 : Reserved				*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* 9 : Reserved				*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* A : Reserved				*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* B : Reserved				*/
 {	LpxReadComObject,		LpxRegifRegNoError,	(COBJECT *)CINFREG },	/* C : TypeInfo  Register	*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* D : ParamDef  Register	*/
 {	LpxReadComObject,		LpxRegifRegNoError,	(COBJECT *)CUNREGX },	/* E : UnMonitor Register	*/
 {	LpxRegifRegNoError,		LpxRegifRegNoError,	NULL },					/* F : Parameter Download(JPDRV)	*/
#if 0
 {	RpxReadPrmRegister,		RpxWritePrmRegister,	0x00, 0x00, },	/* 0 : Parameter Normal			*/
 {	RpxReadPrmRegister,		RpxWritePrmRegister,	0x00, 0x00, },	/* 1 : Parameter Temp.			*/
 {	RpxReadOpeRegister,		RpxWriteOpeRegister,	0x01, 0x00, },	/* 2 : Operation Register		*/
 {	RpxReadEngRegister,		RpxWriteEngRegister,	0x00, 0x00, },	/* 3 : Enginee.  Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 4 : DataTrace Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 5 : DataTrace Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 6 : DataTrace Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 7 : DataTrace Register		*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* 8 : Reserved					*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* 9 : Reserved					*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* A : Reserved					*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* B : Reserved					*/
 {	RpxReadTyInfRegister,	LpxRegifRegNoError,		0x00, 0x00, },	/* C : TypeInfo  Register		*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* D : ParamDef  Register		*/
 {	RpxReadMonRegister,		LpxRegifRegNoError,		0x00, 0x00, },	/* E : UnMonitor Register		*/
 {	RpxReadPdlRegister,		RpxWritePdlRegister,	0x00, 0x00, },	/* F : Parameter Download		*/
#endif
};
/*--------------------------------------------------------------------------------------------------*/
#define Lpx_GetObject( regno )   (RegFuncTbl[ (regno >> 12) & 0x0F ].pObject)
#define OBJ_WRITE( obj, idx, data, hmsg ) ((obj)->Write( (obj), (idx), (data), hmsg ))
#define OBJ_WRITE_SUB( obj, idx, subidx, data, hmsg ) ((obj)->WriteS( (obj), (idx), (subidx), (data), hmsg ))
#define OBJ_READ( obj, idx, pdata, hmsg ) ((obj)->Read( (obj), (idx), (pdata), MCHK_REG_READ, hmsg ))  /* <S073> */
#define OBJ_READ_SUB( obj, idx, subidx, pdata, hmsg ) ((obj)->ReadS( (obj), (idx), (subidx), (pdata), MCHK_REG_READ, hmsg ))  /* <S073> */
#define OBJ_SEARCH( obj, num, pidx, hmsg ) ((obj)->Search( (obj), (num), 0, pidx, MCHK_REG_READ, hmsg ))
#define OBJ_SEARCH_SUB( obj, num, pidx, psubidx, hmsg )  \
		((obj)->SearchS( (obj), (num), 0, pidx, psubidx, MCHK_REG_READ, hmsg ))  /* <S073> */
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
#define Lpx_GetMaxSubIdx( tbl, idx )  /* <S073> */\
			( (GET_OBJDEF( tbl, idx )->DataLength * GET_OBJDEF( tbl, idx )->NumberOfArray)>>1)


/****************************************************************************************************/
/*																									*/
/*		MessageI/F : Main Procedure																	*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifMainProcedure( HMSGIF Hmsg )
{
		LONG	MsgLen;									/* <S150> */
/*--------------------------------------------------------------------------------------------------*/
/*		Reset Hmsg Variables																		*/
/*--------------------------------------------------------------------------------------------------*/
		Hmsg->FunSts    = 0;							/* �⏕�@�\���s����							*/
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
#if 0 && CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			 if( (Hmsg->CmdBuf[2] == 0x01) && ((Hmsg->CmdBuf[4] & SMBUS_MPU_MASK) == SMBUS_MPU1))
			 {
				switch( Hmsg->CmdBuf[3] )
				{
				case SFC_RD_ONEREG:
				case SFC_WR_ONEREG:
				case SFC_RD_SEQREG:
				case SFC_WR_SEQREG:
				case SFC_RD_RANREG:
				case SFC_WR_RANREG:
				case SFC_RD_MAXNUM:
					MemobusTransCmdOptCmdMsg( Hmsg );
					break;
				default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
				}
				break;
			 }
#endif
			 switch( Hmsg->CmdBuf[3] )
			 {
			 case SFC_RD_ONEREG: LpxMbusReadOneRegister( Hmsg ); break;
			 case SFC_WR_ONEREG: LpxMbusWriteOneRegister( Hmsg ); break;
			 case SFC_RD_SEQREG: LpxMbusReadSeqRegisters( Hmsg ); break;
			 case SFC_WR_SEQREG: LpxMbusWriteSeqRegisters( Hmsg ); break;
			 case SFC_RD_RANREG: LpxMbusReadRandomRegisters( Hmsg ); break;
			 case SFC_WR_RANREG: LpxMbusWriteRandomRegisters( Hmsg ); break;
			 case SFC_RD_MAXNUM: LpxMbusReadMaxNumberOfRegisters( Hmsg ); break;
			 default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
			 }
			 break;
		case MFC_SVXMSG32:
			 switch( Hmsg->CmdBuf[3] )
		 	 {
			 case SFC_RD_SEQMEM: LpxMbusReadSeqMemories( Hmsg ); break;
			 case SFC_WR_SEQMEM: LpxMbusWriteSeqMemories( Hmsg ); break;
			 default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
			 }
			 break;
		case MFC_MECHATROLINK:
#if 0	/*<S150>*/
			 if( Hmsg->f.UsbMsgIF ) { LpxMbusTstUsbProcedure( Hmsg ); }
			 else { LpxMbusTstUartProcedure( Hmsg ); /* TODO: */ }
#else
			 LpxMbusMechatrolink( Hmsg, &MsgLen );
			 /* �f�[�^���M */
			 MSGIF_SEND( Hmsg->ResBuf, MsgLen );
#endif
			 break;
//#if (SVFSEL_SWDOWNLOAD != 0)
		case MFC_SYSDWNLD:
#if 0	/*<S150>*/
//			 LpxMbusSystemDownLoad( Hmsg );
			{
				MEMOBUS_IF *MbusIf;
				SHORT dlSts;
				MbusIf = (MEMOBUS_IF*)MbusFuncSysDLHmsg2MbusIf(Hmsg);
				dlSts = MbusFuncSystemDownLoad(MbusIf);
//				MbusFuncSysDLMbusIf2Hmsg(Hmsg);
				if( dlSts != MBUS_BUSY )		/* BUSY�ȊO�ł���Ή�����Ԃ� */
				{
//<S073>			LpxMbusSysDLRes( Hmsg, dlSts );
					MlibSetCRC16MB( Hmsg->ResBuf, MbusIf->TrnbLength/* <S073> <S077> */ );
					MSGIF_SEND( Hmsg->ResBuf, MbusIf->TrnbLength/*@@ CRC*/ );			/* <S073> <S077> */
				}
			}
#else
			 MbusFuncSystemDownLoad( Hmsg );
#endif
			 break;
//#endif
		case MFC_RELAY:
#if 0	/*<S150>*/
			 if( Hmsg->f.UsbMsgIF ) { LpxMbusTstUsbProcedure( Hmsg ); }
			 else{ LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); /*TODO:*/ }
#else
			 LpxMbusLink( Hmsg );
#endif
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
/*		MessageI/F : �w�߃��b�Z�[�W���擾(for Kernel\Driver\SifMbus.c)								*/
/*																									*/
/****************************************************************************************************/
#define LPX_GET_CMDMSGLEN( i, Mul, Add )\
		if( CmdBufLen < (i+2) ){ return( 0 );}\
		else{ return( (((CmdBuf[i]<<8) + CmdBuf[i+1]) * Mul) + Add );}
/*--------------------------------------------------------------------------------------------------*/
#if 0
LONG	RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen )
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
			 /* unreachable return( -1 ); */
		case MFC_SVXMSG32:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_SEQMEM: return( 12 + s );
			 case SFC_WR_SEQMEM: LPX_GET_CMDMSGLEN( 8 + s, 2, 12 + s );
			 default: return( -1 );
			 }
			 /* unreachable return( -1 ); */
		case MFC_SYSDWNLD:								/* @@@ Add Later @@@						*/
			 return( -1 );
		default:
			 if( KPI_CHK_OPEMSG( CmdBuf[1] ) )
			 {
				return( CmdBuf[2] + (CmdBuf[3]<<8) );
			 }
			 return( -1 );
		}
/*--------------------------------------------------------------------------------------------------*/
		/* unreachable return( -1 ); */									/* Unknown Function Code					*/
}
#endif



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : ���b�Z�[�W�h�e�I������															*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifFinishProcedure( HMSGIF Hmsg )
{
		RpxFreeOpeRegWriteHandleAll( Hmsg );	/* <S166> ���샌�W�X�^�����݌������������			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : ��M���b�Z�[�W�`�F�b�N															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ��M���b�Z�[�W�̃`�F�b�N���s���B														*/
/*																									*/
/*			1) ��M���b�Z�[�W���̃`�F�b�N															*/
/*			2) ��M���b�Z�[�W�̃T���`�F�b�N															*/
/*																									*/
/*			��) ���A�h���X�̃`�F�b�N�́A�ʐM�h���C�o�ōs���B										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	RpxMsgifCheckRecvMessage( HMSGIF Hmsg )
{
LONG	MsgLen = Hmsg->CmdMsgLen;
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMessage (LCD-Operater)																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( KPI_CHK_OPEMSG( Hmsg->CmdBuf[1] ) )
		{
			if( MsgLen & 0x0001 ){ return( NG );}
			return( MlibChkSum16SM( Hmsg->CmdBuf, MsgLen ) );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMemobus (PC, etc)																		*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			return( MlibChkCRC16MB( Hmsg->CmdBuf, MsgLen ) );
		}
}
#endif


/****************************************************************************************************/
/*																									*/
/*		MessageI/F : CmdMsg�T�u�A�h���X�擾															*/
/*																									*/
/****************************************************************************************************/
LONG	RpxMsgifGetCmdMsgSubAddr( HMSGIF Hmsg )
{
UCHAR	*CmdBuf = Hmsg->CmdBuf;
#ifdef MCCSW_JPDRVMX
#define CSUBADDR_IDX (4)			/* Jupiter Multi Axis Drive : CmdBuf[4] */
#define CSUBADDR_OFS (0)			/* 01H: 1st axis / 02H: 2nd axis */
#else
#define CSUBADDR_IDX (5)			/* Mercury : CmdBuf[5] */
#define CSUBADDR_OFS (1)			/* 00H: 1st axis / 01H: 2nd axis */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 return( -1 );
		case MFC_SVXMSG16:
			 if( CmdBuf[2] == 0x01 ){ return( CmdBuf[CSUBADDR_IDX] + CSUBADDR_OFS );}
			 break;
		case MFC_SVXMSG32:
			 if( CmdBuf[2] == 0x01 ){ return( CmdBuf[CSUBADDR_IDX] + CSUBADDR_OFS );}
			 break;
		case MFC_SYSDWNLD:
			 if( CmdBuf[2] == 0x01 ){ return( CmdBuf[CSUBADDR_IDX] + CSUBADDR_OFS );}
			 break;
		default:
			 break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( -2 );
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : SubAddr�ُ퉞�����M����														*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifSendErrResSubAddr( HMSGIF Hmsg )
{
		LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );
		return;
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : �⏕�@�\���s���ʉ�������														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �⏕�@�\���s�̎w�߃��b�Z�[�W�̏ꍇ�A�⏕�@�\�̎��s���I������܂ł́A����������			*/
/*			�����ُ킪���f�ł��Ȃ����߁A�����ɂ́A�������b�Z�[�W���쐬�ł��Ȃ��B					*/
/*			�{�֐��́A�⏕�@�\�̎��s���I���������_�ŁA�⏕�@�\�Ǘ�����Ăяo����A�⏕�@�\			*/
/*			���s���ʂ���ɉ������b�Z�[�W���쐬����B												*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifFuncExecResponse( HMSGIF Hmsg )
{
LONG	i,x;
LONG	ResLen;
USHORT	*wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT	*wResBuf = (USHORT *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		�⏕�@�\���s�����ُ펞																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Hmsg->FunSts != REGIF_COMPLETE) && (Hmsg->FunSts != REGIF_CONTINUE) )
		{
			LpxMbusSendErrorResponse( Hmsg, Hmsg->FunSts );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�������b�Z�[�W�o�C�g���v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
		ResLen = (Hmsg->CmdBuf[1] == MFC_SVXMSG16)? 10 : 12;
		ResLen = ResLen + LPX_GET_SLOT_BYTELEN( Hmsg->CmdBuf[2] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݊��������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		for( x=(ResLen>>1)-1, i=0 ; i < x ; i++ )
		{
			wResBuf[i] = wCmdBuf[i];					/* Set Response Data						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, ResLen );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, ResLen );		/* Start Transmit Response Message			*/
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


#if 0	/* <S150> */
#if (SVFSEL_SWDOWNLOAD != 0)
/****************************************************************************************************/
/*																									*/
/*		Memobus�F�V�X�e���_�E�����[�h																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusSystemDownLoad( HMSGIF Hmsg )
{

LONG	errorFlg = FALSE;
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);
USHORT	usDmy=1;
USHORT	TrnsLen;
//Jpdrv	MBUSFUNCTBL		MbusFunc = MbusFuncTbl[ FuncCodeIdx ];
USHORT	SubFuncCode = (((UCHAR)Hmsg->CmdBuf[2] << 8) | (UCHAR)Hmsg->CmdBuf[3]);
/*--------------------------------------------------------------------------------------------------*/
/*		�_�E�����[�h�R�}���h��́�����																*/
/*--------------------------------------------------------------------------------------------------*/
		switch ( SubFuncCode & 0x00FF )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�_�E�����[�h�����擾																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DLID_RD :
				usDmy	= KsysP.SysDlFlushNum;					/* Flash��							*/
				ResBufW[0]	= CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/* MPU�ԍ�							*/
				ResBufW[3]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlFlushNum );	/* Flush��		*/
				ResBufW[4]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts1 );	/* DownLoad Sts1	*/
				ResBufW[5]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts2 );	/* DownLoad Sts2	*/
				ResBufW[6]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts3 );	/* DownLoad Sts3	*/
				ResBufW[7]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts4 );	/* DownLoad Sts4	*/
				ResBufW[8]	= (USHORT)MlibByteSwap16( (LONG )KsysP.DriveID   );	/* ServoID			*/
				ResBufW[9]	= (USHORT)MlibByteSwap16( (LONG )BootSw.ExtRomSizeKB );	/* Flash #0 Manu ID	*/
				ResBufW[10]	= 0;								/* Flash #0 Device ID				*/
				TrnsLen = 24;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		�\�t�g�o�[�W�����Ǐo��																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_SWVER_RD :
				ResBufW[0] = CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1] = CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2] = CmdBufW[2];						/* MPU�ԍ�							*/
				ResBufW[3] = 0x0200;							/* Ver��`��=2					*/
				ResBufW[4] = (USHORT)MlibByteSwap16((LONG )KsysP.Bt2MswVersion );	/* Boot2 Ver	*/
				ResBufW[5] = (USHORT)MlibByteSwap16((LONG )DrvSw.MswVersion ); 		/* Runtime Ver	*/
				TrnsLen = 14;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		�_�E�����[�h�����擾																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DLWAY_RD :	/* �_�E�����[�h�����ݏ����擾 */
				ResBufW[0] = CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1] = CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2] = CmdBufW[2];						/* MPU�ԍ�							*/
				ResBufW[3] = (USHORT)MlibByteSwap16((LONG )KsysP.SysDlWay );	/* �_�E�����[�h���@	*/
				ResBufW[4] = (USHORT)MlibByteSwap16((LONG )KsysP.SysDlPrgbLen );/* 1MsgPRG��������	*/
				TrnsLen = 12;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		�_�E�����[�h�J�n�v��																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_START :	/* �_�E�����[�h�J�n�v�� */
				RpxSetDlMode( Hmsg );							/* �_�E�����[�h�J�n					*/
				/*----------------------------------------------------------------------------------*/
				/* �J�n�����炱���ւ͖߂��Ă��Ȃ�.�J�n�o���Ȃ��ꍇ�Ɉُ퉞��(�������ُ�) 			*/
				/*----------------------------------------------------------------------------------*/
				LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
				errorFlg = TRUE;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		�_�E�����[�h����Ǐo��																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_HIS_RD :
				ResBufW[0] = CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1] = CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2] = CmdBufW[2];						/* MPU�ԍ�							*/
				ResBufW[3] = (USHORT)MlibByteSwap16((LONG )KsysP.SwUpDateHis );	/* �_�E�����[�h����	*/
				TrnsLen = 10;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		���T�|�[�g�T�u�t�@���N�V����														*/
		/*		�_�E�����[�h�J�n�v���ȍ~�̂ݗL���ȃR�}���h											*/
		/*			�������G���[																	*/
		/*------------------------------------------------------------------------------------------*/
		  default:
				LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
				errorFlg = TRUE;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���퉞�����M																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( errorFlg == FALSE )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	CRC-16�쐬�������f�[�^���M�w��															*/
		/*------------------------------------------------------------------------------------------*/
			MlibSetCRC16MB( Hmsg->ResBuf, TrnsLen );
			MSGIF_SEND( Hmsg->ResBuf, TrnsLen );			/* Start Transmit Response Message	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#else
/****************************************************************************************************/
/*																									*/
/*		Memobus�F�V�X�e���_�E�����[�h																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusSysDLRes( HMSGIF Hmsg, SHORT dlSts )
{

LONG	errorFlg = FALSE;
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);
USHORT	TrnsLen;
USHORT	SubFuncCode = (((UCHAR)Hmsg->CmdBuf[2] << 8) | (UCHAR)Hmsg->CmdBuf[3]);

/*--------------------------------------------------------------------------------------------------*/
/*		�_�E�����[�h�R�}���h��́�����																*/
/*--------------------------------------------------------------------------------------------------*/
		switch ( SubFuncCode & 0x00FF )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		��ۯ�����															*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_PRGDEL :
				ResBufW[0]	= CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/* MPU�ԍ�							*/
				ResBufW[3]	= CmdBufW[3];						/* Flush�ԍ�		*/
				ResBufW[4]	= CmdBufW[4];						/* �����݃A�h���X��ʃ��[�h	*/
				ResBufW[5]	= CmdBufW[5];						/* �����݃A�h���X���ʃ��[�h	*/
				if(dlSts == 0)
				{
					TrnsLen = 14;
				}
				else
				{
					LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
					errorFlg = TRUE;
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		��۸��я�����																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_PRGWR :
				ResBufW[0]	= CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/* MPU�ԍ�							*/
				ResBufW[3]	= CmdBufW[3];						/* Flush�ԍ�		*/
				ResBufW[4]	= CmdBufW[4];						/* �f�[�^��	*/
				ResBufW[5]	= CmdBufW[5];						/* �����݃A�h���X��ʃ��[�h	*/
				ResBufW[6]	= CmdBufW[6];						/* �����݃A�h���X���ʃ��[�h	*/
				if(dlSts == 0)
				{
					TrnsLen = 16;
				}
				else
				{
					LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
					errorFlg = TRUE;
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		�رقh�e���x�ύX				<S041>											*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_SIFCHG:
				ResBufW[0]	= CmdBufW[0];						/*+ AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/*+ SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/*+ MPU�ԍ�							*/
				ResBufW[3]	= CmdBufW[3];						/*+ �]�����x��ʃ��[�h	*/
				ResBufW[4]	= CmdBufW[4];						/*+ �]�����x���ʃ��[�h	*/
				if(dlSts == 0)
				{
					TrnsLen = 12;
				}
				else
				{
					LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
					errorFlg = TRUE;
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		���T�|�[�g�T�u�t�@���N�V����														*/
		/*		�_�E�����[�h�J�n�v���ȍ~�̂ݗL���ȃR�}���h											*/
		/*			�������G���[																	*/
		/*------------------------------------------------------------------------------------------*/
		  default:
				LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
				errorFlg = TRUE;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���퉞�����M																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( errorFlg == FALSE )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	CRC-16�쐬�������f�[�^���M�w��															*/
		/*------------------------------------------------------------------------------------------*/
			MlibSetCRC16MB( Hmsg->ResBuf, TrnsLen );
			MSGIF_SEND( Hmsg->ResBuf, TrnsLen );			/* Start Transmit Response Message	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#endif
#endif	/* <S150> */


/****************************************************************************************************/
/*																									*/
/*		Memobus�F�P�ꃌ�W�X�^�Ǐo��																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadOneRegister( HMSGIF Hmsg )
{
LONG	rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo������																			*/
/*--------------------------------------------------------------------------------------------------*/
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].ReadRegister( RegNo, 1, &wResBuf[2+s], Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo���ُ�`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo�����������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = MlibCnvBig16( wResBuf[2+s] );	/* Read Register Data						*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 8+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 8+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F�P�ꃌ�W�X�^������																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg )
{
LONG	rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����ݏ���																			*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[3+s] = MlibCnvBig16( wCmdBuf[3+s] );
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, 1, &wResBuf[3+s], Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		�����`�F�b�N (�⏕�@�\���s��)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc == REGIF_CONTINUE )
		{												/* �⏕�@�\���s��A������ԐM����̂�		*/
			return;										/* �����ł́A�������Ȃ�						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����ُ݈�`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݊��������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* RegNo									*/
		wResBuf[3+s] = wCmdBuf[3+s];					/* WriteData								*/
		wResBuf[4+s] = wCmdBuf[4+s];					/* CRC-16									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����f�[�^���M�w��																			*/
/*--------------------------------------------------------------------------------------------------*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F�A�����W�X�^�Ǐo��																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg )
{
LONG	i, rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
LONG	RegNum  = MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo�������`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > LpxGetMaxNumberOfRdRegisters( Hmsg, 8+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo������																			*/
/*--------------------------------------------------------------------------------------------------*/
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].ReadRegister( RegNo, RegNum, &wResBuf[3+s], Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo���ُ�`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
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
//#if		defined( CCSW_LITTLE_ENDIAN ) TODO
		for( i=0; i < RegNum; i++ ){ wResBuf[3+s+i] = MlibCnvBig16( wResBuf[3+s+i] );}
//#endif
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		i = (RegNum * 2) + 8 + (2*s);					/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, i );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, i );				/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F�A�����W�X�^������																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteSeqRegisters( HMSGIF Hmsg )
{
LONG	i, rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
LONG	RegNum  = MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݌����`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > LpxGetMaxNumberOfWrRegisters( Hmsg, 12+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����ݏ���																			*/
/*--------------------------------------------------------------------------------------------------*/
#if		defined( CCSW_BIG_ENDIAN )
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, RegNum, &wCmdBuf[5+s], Hmsg );
#else
		for( i=0; i < RegNum; i++ ){ wResBuf[5+s+i] = MlibCnvBig16( wCmdBuf[5+s+i] );}
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, RegNum, &wResBuf[5+s], Hmsg );
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�����`�F�b�N (�⏕�@�\���s��)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc == REGIF_CONTINUE )
		{												/* �⏕�@�\���s��A������ԐM����̂�		*/
			return;										/* �����ł́A�������Ȃ�						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����ُ݈�`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݊��������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* RegNo									*/
		wResBuf[3+s] = wCmdBuf[3+s];					/* Write Number								*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 10+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F��A�����W�X�^�Ǐo��																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadRandomRegisters( HMSGIF Hmsg )
{
LONG	rc;
LONG	i,j,x;
LONG	RegNo;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNum  = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo�������`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > LpxGetMaxNumberOfRdRegisters( Hmsg, 8+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo������																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < RegNum; i=i+j )
		{
			RegNo = MlibCnvBig16( wCmdBuf[3+s+i] );
			for( j=1; j < (RegNum - i); j++ )
			{
				x = MlibCnvBig16( wCmdBuf[3+s+i+j] );
				if( x != (RegNo + j) ) break;
			}
		/*------------------------------------------------------------------------------------------*/
			rc = RegFuncTbl[ RFTBLIDX(RegNo) ].ReadRegister( RegNo, j, &wResBuf[3+s+i], Hmsg );
			if( rc != REGIF_COMPLETE )
			{
				LpxMbusSendErrorResponse( Hmsg, rc );
				return;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�Ǐo�����������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = MlibCnvBig16( RegNum * 2 );		/* Data Byte Number							*/
/*--------------------------------------------------------------------------------------------------*/
//#if		defined( CCSW_LITTLE_ENDIAN ) TODO
		for( i=0; i < RegNum; i++ ){ wResBuf[3+s+i] = MlibCnvBig16( wResBuf[3+s+i] );}
//#endif
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		i = (RegNum * 2) + 8 + (2*s);					/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, i );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, i );				/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F��A�����W�X�^������																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteRandomRegisters( HMSGIF Hmsg )
{
LONG	rc;
LONG	i,j,x;
LONG	RegNo;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNum  = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݌����`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > (LpxGetMaxNumberOfWrRegisters( Hmsg, 10+(2*s) ) >> 1) ) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����ݏ���																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < RegNum; i=i+j )
		{
			RegNo = MlibCnvBig16( wCmdBuf[4+s+(2*i)] );
			for( j=1; j < (RegNum - i); j++ )
			{
				x = MlibCnvBig16( wCmdBuf[4+s+(2*(i+j))] );
				if( x != (RegNo + j) ) break;
			}
		/*------------------------------------------------------------------------------------------*/
			for( x=0; x < j; x++ )
			{
				wResBuf[x] = MlibCnvBig16( wCmdBuf[4+s+(2*(i+x))+1] );
			}
		/*------------------------------------------------------------------------------------------*/
			rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, j, &wResBuf[0], Hmsg );
			if( rc != REGIF_COMPLETE )
			{
				LpxMbusSendErrorResponse( Hmsg, rc );
				return;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�����݊��������쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* RegNum									*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 8+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 8+(2*s) );		/* Start Transmit Response Message			*/
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
/*		Memobus�F�A���������Ǐo��																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadSeqMemories( HMSGIF Hmsg )
{
//LONG	i, rc;
LONG	i;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
ULONG	MemNum  = MlibCnvBig16( wCmdBuf[4+s] );
ULONG	MemAddr = (MlibCnvBig16( wCmdBuf[2+s] ) << 16) +  MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		�������Ǐo�������`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MemNum < 1) || (MemNum > LpxGetMaxNumberOfRdRegisters( Hmsg, 8+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�������Ǐo���A�h���X�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if ( 0 /* KpiCheckReadMemAddr( MemAddr, MemNum) == MEMDEF_READ_NG  */)
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�������Ǐo�������f�[�^�쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[4+s];					/* Data Word Number							*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MemNum; i++ )
		{
			wResBuf[3+s+i] = MlibCnvBig16( ((USHORT*)MemAddr)[i] );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		i = (MemNum * 2) + 8 + (2*s);					/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, i );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, i );				/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus�F�A��������������																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteSeqMemories( HMSGIF Hmsg )
{
LONG	i;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
ULONG	MemNum  = MlibCnvBig16( wCmdBuf[4+s] );
ULONG	MemAddr = (MlibCnvBig16( wCmdBuf[2+s] ) << 16) +  MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		�����������݌����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MemNum < 1) || (MemNum > LpxGetMaxNumberOfWrRegisters( Hmsg, 12+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����������݃A�h���X�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if ( 0 /* KpiCheckWriteMemAddr( MemAddr, MemNum ) == MEMDEF_WRITE_NG */)
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����������ݏ���																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MemNum; i++ )
		{
			((USHORT*)MemAddr)[i] = MlibCnvBig16( wCmdBuf[5+s+i] );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����������݉����f�[�^�쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* Memory Address							*/
		wResBuf[3+s] = wCmdBuf[3+s];					/* Memory Address							*/
		wResBuf[4+s] = wCmdBuf[4+s];					/* Data Word Number							*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16�쐬�������f�[�^���M�w��																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 12 + (2*s) );	/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 12 + (2*s) );	/* Start Transmit Response Message			*/
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

#if 0 && CCSW_CMDID == CMDID_EXIST
/****************************************************************************************************/
/*																									*/
/*		�I�v�V�����J�[�h�ʐM�����f�[�^�]������														*/
/*																									*/
/****************************************************************************************************/
ULONG	RpxSmsgTransOptRspMessage( HMSGIF Hmsg )
{
ULONG	rc;

/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ʐM�f�[�^�����]��																	*/
/*--------------------------------------------------------------------------------------------------*/
		rc = MemobusTransCmdOptRspMsg( Hmsg );		/* �w�߃I�v�V�����J�[�h (SC���b�Z�[�W�ʐM)		*/

		return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		�w�߃I�v�V�����J�[�h�ւ̃R�}���h�f�[�^�]���ݒ� (SigmaWin+���̏�ʃc�[������)				*/
/*																									*/
/****************************************************************************************************/
void	MemobusTransCmdOptCmdMsg( HMSGIF Hmsg )
{
LONG	size = (LONG)(Hmsg->CmdMsgLen);
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߃I�v�V�����ʐM��Ԏ擾																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( ( Hmsg->f.ComMsgIF ||  Hmsg->f.UsbMsgIF ) && (OpiGetSCMsgState() == SCMSG_ENABLE) )
		{
			OpiReqSCMsgTrans( CmdBufW, size);						/* SC���b�Z�[�W���M�J�n			*/
			KpiRstLongTimer( &Hmsg->ComTimeLong );					/* �^�C���A�E�g�p�^�C�}���Z�b�g */
			Hmsg->RcvWait = TRUE;									/* SC���b�Z�[�W�ʐM��M�҂��Z�b�g*/
		}
		else
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );	/* �A�N�Z�X�����G���[ (0x31)	*/
		}

		return;
}



/****************************************************************************************************/
/*																									*/
/*		�w�߃I�v�V�����J�[�h����̃��X�|���X�f�[�^�]���ݒ� (SigmaWin+���̏�ʃc�[����)				*/
/*																									*/
/****************************************************************************************************/
ULONG	MemobusTransCmdOptRspMsg( HMSGIF Hmsg )
{
LONG	rc,size;
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);

/*--------------------------------------------------------------------------------------------------*/
/*		SC���b�Z�[�W��M�҂����͉����擾�������s													*/
/*--------------------------------------------------------------------------------------------------*/
		if( Hmsg->RcvWait == TRUE )
		{
/*--------------------------------------------------------------------------------------------------*/
/*			MEMOBUS���b�Z�[�W�R�}���h�ɑ΂��鉞���擾												*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OpiGetSCMsgRspData( ResBufW, &size );

/*--------------------------------------------------------------------------------------------------*/
/*			�����f�[�^���																			*/
/*--------------------------------------------------------------------------------------------------*/
			if( rc == SCMSG_SUCCESS )
			{/* ����I�� */
				Hmsg->StxResMsg( Hmsg->ResBuf, (USHORT)size );		/* Start Transmit Response Message	*/
				Hmsg->RcvWait = FALSE;								 /* ��M�҂��N���A			*/
			}
			else if( rc == SCMSG_BUSY )
			{/* �����p���� */
				if ( KpiGetLongTimer( &Hmsg->ComTimeLong ) > SCM_COM_TIMEOUT ) /* 1000msec�o��	*/
				{
					OpiCancelSCMsgReq( );							/* SC���b�Z�[�W��M�҂��L�����Z��	*/
					Hmsg->StxResMsg( Hmsg->ResBuf, 0 );					/* �������f�[�^���M�J�n	*/
					Hmsg->RcvWait = FALSE;								 /* ��M�҂��N���A			*/
				}
			}
			else if( (rc == SCMSG_REQ_ERR) || (rc == SCMSG_RCV_ERR) )
			{/* �ُ�I�� */
				Hmsg->StxResMsg( Hmsg->ResBuf, 0 );							/* �������f�[�^���M�J�n	*/
				Hmsg->RcvWait = FALSE;									 /* ��M�҂��N���A			*/
			}
			else
			{
				LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );	 /* �A�N�Z�X�����G���[(0x31)*/
				Hmsg->RcvWait = FALSE;									 /* ��M�҂��N���A			*/
			}
			return( TRUE );		/* Hmsg->RcvWait == TRUE�œ����Ă����ꍇ�͂Ƃɂ���TRUE�ŕԂ��B�@*/
								/* �������Ȃ���FALSE�Ŗ߂�������ɃR�}���h��͂��ēx���Ă��܂��B*/
		}
		else
		{
			return( Hmsg->RcvWait );
		}
}
#endif


/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�ǂݏo��(���W�X�^�w��)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v:	�w�肳���p�����[�^���w���(WORD)�ǂݏo���B											*/
/*			Reads the specified Parameter value														*/
/*																									*/
/*	�ߒl:	�X�e�[�^�X																				*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadPrmRegister(					/* �p�����[�^�ǂݏo��(���W�X�^�w��)						*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �Ǐo���f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �Ǐo���f�[�^�o�b�t�@									*/
		HMSGIF	Hmsg			)			/* �Ǐo��IF�n���h��										*/
{
//LONG	rc;
LONG	i,idx;
LONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
//CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�A�N�Z�X�G���A�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( ((RegNoX + Number) >= 0x1000)
		 ||	((RegNoX < 0x0800) && ((RegNoX + Number) >= 0x0800))
		 || ((RegNoX < 0x0D00) && ((RegNoX + Number) >= 0x0D00))	)
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(�O���[�v���E)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�ԍ�����																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�(REG�ԍ�����)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�J�n���W�X�^�����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(LONG���E)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^�Ǐo��																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�V�X�e���p�����[�^�Ǐo���`�F�b�N(�s���������ǂ������I)									*/
		/*------------------------------------------------------------------------------------------*/
#if 0
			if( 0 && (PnPrmTbl[idx].PrmDefPtr->AccessLevel >= ACCLVL_SYSTEM) )
			{
				if( Hmsg->AccessLvl < ACCLVL_SYSTEM )
				{
					Hmsg->ErrRno = RegNo + i;			/* �ُ탌�W�X�^�ԍ�							*/
					return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	���W�X�^�ԍ��̘A�����`�F�b�N															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				return( REGIF_REGNOERR );				/* ���W�X�^�ԍ��ُ�(REG�ԍ���A��)			*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^�Ǎ���																		*/
		/*------------------------------------------------------------------------------------------*/
			OBJ_READ( pObj, idx, (ULONG*)&pn_data, Hmsg );
#if 0
//			rc = RpxPrmReadIdx( idx, (ULONG *)(&pn_data), PCHK_REG_READ );	/* <11J45>�폜 */

			/* <11J45>�ǉ� Start */
			if(Hmsg->f.NotConvScale == TRUE)
			{
				rc = RpxPrmReadIdxNotConvScale( idx, (ULONG *)(&pn_data), PCHK_REG_READ ); /* �X�P�[���ϊ����Ȃ� */
			}
			else
			{
				rc = RpxPrmReadIdx( idx, (ULONG *)(&pn_data), PCHK_REG_READ );				/* �X�P�[���ϊ����� */
			}
			/* <11J45>�ǉ� End */

			if(rc != OK)
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				switch(rc)
				{
				case PERR_DFIT:
					return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
					break;
				case PERR_PRMCOPY:
					return( REGIF_FOPEERR3 );			/* �����s��(���b�g�Ƃ̏�������)				*/
					break;
				default:
					return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
					break;
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	�Ǎ��݃f�[�^�̓V���[�g/�����O?															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				RdBuf[i+0]= GET_LWD_REGDATA( pn_data );	/* LONG  Parameter Low  Word				*/
				RdBuf[i+1]= GET_HWD_REGDATA( pn_data );	/* LONG  Parameter High Word				*/
				i++;
			}
			else
			{
				RdBuf[i] = (USHORT)(pn_data);			/* SHORT Parameter							*/
			}
#if 0
//			if( PnPrmTbl[idx].PrmDefPtr->Attr.IfIsLong == FALSE )/* <11J45>�폜 */
			if (((PnPrmTbl[idx].PrmDefPtr->Attr.IfIsLong == FALSE)&&( Hmsg->f.NotConvScale == FALSE))
				||((PnPrmTbl[idx].PrmDefPtr->Attr.VarIsLong == FALSE)&&( Hmsg->f.NotConvScale == TRUE)))/* <11J45> */
			{
				RdBuf[i] = (USHORT)(pn_data);			/* SHORT Parameter							*/
			}
			else if( (i+1) < Number )
			{
				RdBuf[i+0]= GET_LWD_REGDATA( pn_data );	/* LONG  Parameter Low  Word				*/
				RdBuf[i+1]= GET_HWD_REGDATA( pn_data );	/* LONG  Parameter High Word				*/
				i++;
			}
			else
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				return( REGIF_DAMTHERR );				/* �f�[�^�����ُ�(LONG���E)					*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}

/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�������ݏ���(���W�X�^�w��)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v:	���W�X�^�Ŏw�肳���p�����[�^�l��RAM�֏������ށB										*/
/*																									*/
/*	�ߒl:	�X�e�[�^�X																				*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWritePrmRegister(				/* �p�����[�^�������ݏ���(���W�X�^�w��)					*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �������݃f�[�^��(WORD��)							*/
		USHORT	*WrBuf,						/* �������݃f�[�^�o�b�t�@								*/
		HMSGIF	Hmsg			)			/* ��������IF�n���h��									*/
{
LONG	rc;
LONG	i,idx;
ULONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�A�N�Z�X�G���A�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( ((RegNoX + Number) >= 0x1000)
		 ||	((RegNoX < 0x0800) && ((RegNoX + Number) >= 0x0800))
		 || ((RegNoX < 0x0D00) && ((RegNoX + Number) >= 0x0D00))	)
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(�O���[�v���E)				*/
		}
#if 0 
/*--------------------------------------------------------------------------------------------------*/
/*		�V�X�e���萔�p�����[�^�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (OpeReg.PrmAccess == FALSE) &&
			((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�萔�p�����[�^�`�F�b�N															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
		if( ENC_PRM_WRT_DISABLE && ((RegNoX + Number) >= ENC_PRM_NUM) )
		{
			i = (RegNoX >= 0x0F00)? 0 : (ENC_PRM_NUM - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�ԍ�����																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�(REG�ԍ�����)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�J�n���W�X�^�����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(LONG���E)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�V�X�e���萔�p�����[�^�`�F�b�N								<12J49>�ǉ�					 	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
/* ���W�X�^�ԍ������Ŏ擾�����A�uidx�v���g�p���邽�߂ɁA�{���������̏ꏊ�֎����Ă��Ă���B		 	*/
		if( (pPrm->prmwr_spn == 0x0001) 									   /* Pn030 = 0x0001(���[�^Prm�������ݗL��) */
		 && (PnPrmTbl[idx].PrmDefPtr->Attr.MotorPrmAmp == PRMDEF_MOTPRM_AMP)   /* �A���v���̃��[�^Prm(�厲���[�^Prm) 	*/
		 && (PnPrmTbl[idx].PrmDefPtr != (&pndef_MotPrmSumChkEnb)) 			   /* PnFFF�͏��� 							*/
		 && (Hmsg->f.LnkMsgIF == TRUE) )									   /* SVPRM_WR���g�p	 					*/
		{
			;
		}
		else if( (OpeReg.PrmAccess == FALSE) &&
				((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^�����݋֎~�`�F�b�N															*/
/*--------------------------------------------------------------------------------------------------*/
//		if( RndWrtDisable.FunA.f.PrmReg || RndWrtDisable.FunB.f.PrmReg )
//		{
//			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
//			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^������																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^�A�N�Z�X���x���`�F�b�N														*/
		/*------------------------------------------------------------------------------------------*/
#if 0 /* TODO :*/
			if( (OpeReg.PrmAccess == FALSE) && (PrmAttributeChkIdx(idx, PCHK_ACCS_LVL, Hmsg) != OK) )
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				return( REGIF_REGNOERR );				/* ���W�X�^�ԍ��ُ�							*/
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	���W�X�^�ԍ��̘A�����`�F�b�N															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				return( REGIF_REGNOERR );				/* ���W�X�^�ԍ��ُ�(REG�ԍ���A��)			*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�����݃f�[�^�̓����O?																	*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				if( (i + 1) < Number )
				{
					pn_data = ULONGOF_REGDATA( WrBuf[i], WrBuf[i+1] );
				}
				else
				{
					Hmsg->ErrRno = RegNo + i;			/* �ُ탌�W�X�^�ԍ�							*/
					return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
				}
			}
			else
			{
				pn_data = WrBuf[i];
				if( CHK_PNPRM_SIGNED( GET_PNPRMDEF( pTbl, idx ) ) )
				{
					pn_data = (LONG)((SHORT)pn_data); 	/* �����t���[�h�̏ꍇ�A�����g�����Ă���	*/
				}
			}
#if 0
			if( PnPrmTbl[idx].PrmDefPtr->Attr.IfIsLong )
			{	/* LONG �f�[�^	*/
				if( (i + 1) < Number )
				{
					pn_data = ULONGOF_REGDATA( RdBuf[i], RdBuf[i+1] );
				}
				else
				{
					Hmsg->ErrRno = RegNo + i;			/* �ُ탌�W�X�^�ԍ�							*/
					return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
				}
			}
			else
			{	/* WORD �f�[�^		*/
				pn_data = RdBuf[i];
				/* WORD�T�C�Y�ŕ����t	*/
				if( PnPrmTbl[idx].PrmDefPtr->Attr.Sign == DSPDEF_W_SIGN )
				{
					/* �����t���[�h�̏ꍇ�A�����g�����Ă���	*/
					pn_data = (LONG)((SHORT)pn_data);
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^RAM������																		*/
		/*------------------------------------------------------------------------------------------*/
			rc = OBJ_WRITE( pObj, idx, pn_data, Hmsg );
			if(  rc != OK )
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				switch(rc)
				{
				case OERR_DFIT:
					 return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
//					 break;//<S0C7>statement is unreachable
				case OERR_RONLY:
					 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
//					 break;//<S0C7>statement is unreachable
				case OERR_CALC:
					 return( REGIF_DACALERR );			/* �f�[�^�ُ�(���Z�ُ�) 					*/
//					 break;//<S0C7>statement is unreachable
				case OERR_RANGE:
					 return( REGIF_DALMTERR );			/* �f�[�^�ُ�(�㉺���ُ�)					*/
//					 break;//<S0C7>statement is unreachable
				case OERR_PRMCOPY:
				case OERR_WRITE_HANDLE:
					 return( REGIF_FOPEERR3 );			/* �����s��(���b�g�Ƃ̏�������)				*/
//					 break;//<S0C7>statement is unreachable
				default:
					 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
//					 break;//<S0C7>statement is unreachable
				}
			}
#if 0
			rc = RpxPrmWriteRamIdx( idx, pn_data, PCHK_REG_WRITE, Hmsg );
			if( rc != OK )
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				switch(rc)
				{
				case PERR_DFIT:
					 return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
					 break;
				case PERR_RONLY:
					 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
					 break;
				case PERR_CALC:
					 return( REGIF_DACALERR );			/* �f�[�^�ُ�(���Z�ُ�) 					*/
					 break;
				case PERR_RANGE:
					 return( REGIF_DALMTERR );			/* �f�[�^�ُ�(�㉺���ُ�)					*/
					 break;
				case PERR_PRMCOPY:
				case PERR_WRITE_HANDLE:
					 return( REGIF_FOPEERR3 );			/* �����s��(���b�g�Ƃ̏�������)				*/
					 break;
				default:
					 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
					 break;
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^EEPROM������																	*/
		/*------------------------------------------------------------------------------------------*/
//#if 0		/* 40H EEPROM�����ݑΉ�<S111>	*/
			if( !(RegNo & 0x1000) )
			{
#if 0 /* <12J49>�폜 */
				rc = RpiPrmUpdateNvmem( PnPrmTbl[idx].PrmDefPtr );
				if( rc != OK && rc != PRM_EEPROM_RAM_NO_DIFF )
				{
					return( REGIF_DAMTHERR );
				}
			}

#endif /* <12J49>�폜End */

#if 0	/* 40H EEPROM�����ݑΉ�<S111>	*/ /* <12J49>�ǉ� */
                if( (rc = LpxChkSumFlagSave(PnPrmTbl[idx].PrmDefPtr, Hmsg)) == TRUE)
                {
                    rc = RpiPrmUpdateNvmem( PnPrmTbl[idx].PrmDefPtr );
                }

                if( rc != OK && rc != PRM_EEPROM_RAM_NO_DIFF )
                {
                    return( REGIF_DAMTHERR );
				}
#endif	/* 40H EEPROM�����ݑΉ�<S111>	*/ /* <12J49>�ǉ�End */
/* 40H EEPROM�����ݑΉ�<S111>	*/
				rc = PrmWriteValueToEeprom( Hmsg->pAxRsc->RegManager->hPnReg, PnPrmTbl[idx].Attribute, 0 );
				if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_NO_DIFF) )
				{
					return( REGIF_DAMTHERR );
				}
/* 40H EEPROM�����ݑΉ�<S111>	*/
			}
//#endif		/* 40H EEPROM�����ݑΉ�<S111>	*/

		/*------------------------------------------------------------------------------------------*/
		/*	LONG�f�[�^�̏ꍇ																		*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				i++;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}

/****************************************************************************************************/
/*																									*/
/*		���W�X�^�ǂݏo��(����G���A�A�@����Aetc)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v:	�w�肳���p�����[�^���w���(WORD)�ǂݏo���B											*/
/*			Reads the specified Parameter value														*/
/*																									*/
/*	�ߒl:	�X�e�[�^�X																				*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadComObject(					/* �p�����[�^�ǂݏo��(���W�X�^�w��)						*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �Ǐo���f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �Ǐo���f�[�^�o�b�t�@									*/
		HMSGIF	Hmsg			)			/* �Ǐo��IF�n���h��										*/
{
//LONG	rc;
LONG	i,idx;
LONG	subidx;								/* <S073> */
LONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�A�N�Z�X�G���A�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(�O���[�v���E)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�ԍ�����																			*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		if( OK != OBJ_SEARCH_SUB( pObj, RegNoX, &idx, &subidx, Hmsg ) )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�(REG�ԍ�����)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�J�n���W�X�^�����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx )!= RegNoX )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(LONG���E)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^�Ǐo��																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�V�X�e���p�����[�^�Ǐo���`�F�b�N(�s���������ǂ������I)									*/
		/*------------------------------------------------------------------------------------------*/
#if 0
			if( 0 && (PnPrmTbl[idx].PrmDefPtr->AccessLevel >= ACCLVL_SYSTEM) )
			{
				if( Hmsg->AccessLvl < ACCLVL_SYSTEM )
				{
					Hmsg->ErrRno = RegNo + i;			/* �ُ탌�W�X�^�ԍ�							*/
					return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	���W�X�^�ԍ��̘A�����`�F�b�N															*/
		/*------------------------------------------------------------------------------------------*/
//<S073>			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx) != (RegNoX + i) ) /* <S073> */
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				return( REGIF_REGNOERR );				/* ���W�X�^�ԍ��ُ�(REG�ԍ���A��)			*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^�Ǎ���																		*/
		/*------------------------------------------------------------------------------------------*/
#if 1	/* <S073> Array�Ή� */
			do {
				/*----------------------------------------------------------------------------------*/
				/*	�Ǎ��݃f�[�^�̓V���[�g/�����O?													*/
				/*----------------------------------------------------------------------------------*/
				if( pObj->isLong( pObj, idx, Hmsg ) )
				{
					OBJ_READ_SUB( pObj, idx, subidx, (ULONG*)&pn_data, Hmsg );
					RdBuf[i+0]= GET_LWD_REGDATA( pn_data );		/* LONG  Parameter Low  Word		*/
					RdBuf[i+1]= GET_HWD_REGDATA( pn_data );		/* LONG  Parameter High Word		*/
					i++; subidx++;								/* increment index */
				}
				else
				{
					OBJ_READ_SUB( pObj, idx, subidx, (ULONG*)&pn_data, Hmsg );
					RdBuf[i] = (USHORT)(pn_data);				/* SHORT Parameter					*/
				}
				/*----------------------------------------------------------------------------------*/
				i++; subidx++;									/* increment index */
			} 
			while ( (i < Number) && (subidx < Lpx_GetMaxSubIdx( pTbl, idx ) ) );
			subidx = 0;
#else
			OBJ_READ( pObj, idx, &pn_data, Hmsg );
		/*------------------------------------------------------------------------------------------*/
		/*	�Ǎ��݃f�[�^�̓V���[�g/�����O?															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				RdBuf[i+0]= GET_LWD_REGDATA( pn_data );	/* LONG  Parameter Low  Word				*/
				RdBuf[i+1]= GET_HWD_REGDATA( pn_data );	/* LONG  Parameter High Word				*/
				i++;
			}
			else
			{
				RdBuf[i] = (USHORT)(pn_data);			/* SHORT Parameter							*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�������ݏ���(���W�X�^�w��)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v:	���W�X�^�Ŏw�肳���p�����[�^�l��RAM�֏������ށB										*/
/*																									*/
/*	�ߒl:	�X�e�[�^�X																				*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWriteComObject(					/* �p�����[�^�������ݏ���(���W�X�^�w��)					*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �������݃f�[�^��(WORD��)							*/
		USHORT	*WrBuf,						/* �������݃f�[�^�o�b�t�@								*/
		HMSGIF	Hmsg			)			/* ��������IF�n���h��									*/
{
LONG	rc;
LONG	i,idx;
LONG	subidx;								/* <S073> */
ULONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�A�N�Z�X�G���A�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(�O���[�v���E)				*/
		}
#if 0 
/*--------------------------------------------------------------------------------------------------*/
/*		�V�X�e���萔�p�����[�^�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (OpeReg.PrmAccess == FALSE) &&
			((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�萔�p�����[�^�`�F�b�N															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
		if( ENC_PRM_WRT_DISABLE && ((RegNoX + Number) >= ENC_PRM_NUM) )
		{
			i = (RegNoX >= 0x0F00)? 0 : (ENC_PRM_NUM - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�ԍ�����																			*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		if( OK != OBJ_SEARCH_SUB( pObj, RegNoX, &idx, &subidx, Hmsg ) )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�(REG�ԍ�����)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�J�n���W�X�^�����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx )!= RegNoX )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_DAMTHERR );					/* �f�[�^�����ُ�(LONG���E)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�V�X�e���萔�p�����[�^�`�F�b�N								<12J49>�ǉ�					 	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
/* ���W�X�^�ԍ������Ŏ擾�����A�uidx�v���g�p���邽�߂ɁA�{���������̏ꏊ�֎����Ă��Ă���B		 	*/
		if( (pPrm->prmwr_spn == 0x0001) 									   /* Pn030 = 0x0001(���[�^Prm�������ݗL��) */
		 && (PnPrmTbl[idx].PrmDefPtr->Attr.MotorPrmAmp == PRMDEF_MOTPRM_AMP)   /* �A���v���̃��[�^Prm(�厲���[�^Prm) 	*/
		 && (PnPrmTbl[idx].PrmDefPtr != (&pndef_MotPrmSumChkEnb)) 			   /* PnFFF�͏��� 							*/
		 && (Hmsg->f.LnkMsgIF == TRUE) )									   /* SVPRM_WR���g�p	 					*/
		{
			;
		}
		else if( (OpeReg.PrmAccess == FALSE) &&
				((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* �ُ탌�W�X�^�ԍ�							*/
			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^�����݋֎~�`�F�b�N															*/
/*--------------------------------------------------------------------------------------------------*/
//		if( RndWrtDisable.FunA.f.PrmReg || RndWrtDisable.FunB.f.PrmReg )
//		{
//			Hmsg->ErrRno = (USHORT)RegNo;				/* �ُ탌�W�X�^�ԍ�							*/
//			return( REGIF_REGNOERR );					/* ���W�X�^�ԍ��ُ�							*/
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^������																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	TODO: �p�����[�^�A�N�Z�X���x���`�F�b�N													*/
		/*------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	���W�X�^�ԍ��̘A�����`�F�b�N															*/
		/*------------------------------------------------------------------------------------------*/
//<S073>			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx) != (RegNoX + i) )	/* <S073> */
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				return( REGIF_REGNOERR );				/* ���W�X�^�ԍ��ُ�(REG�ԍ���A��)			*/
			}
#if 1	/* <S073> Array */
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^������																		*/
		/*------------------------------------------------------------------------------------------*/
			do {
				/*----------------------------------------------------------------------------------*/
				/*	�����݃f�[�^�̓����O?															*/
				/*----------------------------------------------------------------------------------*/
				if( pObj->isLong( pObj, idx, Hmsg ) )
				{
					if( (i + subidx + 1) < Number )
					{
						pn_data = ULONGOF_REGDATA( WrBuf[i+subidx], WrBuf[i+subidx+1] );
					}
					else
					{
						Hmsg->ErrRno = RegNo + i;			/* �ُ탌�W�X�^�ԍ�						*/
						return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)				*/
					}
				}
				else
				{
					pn_data = WrBuf[i];
					if( CHK_PNPRM_SIGNED( GET_PNPRMDEF( pTbl, idx ) ) )
					{
						pn_data = (LONG)((SHORT)pn_data); 	/* �����t���[�h�̏ꍇ�A�����g�����Ă���	*/
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	�p�����[�^RAM������																*/
				/*----------------------------------------------------------------------------------*/
				if( pObj->isLong( pObj, idx, Hmsg ) )
				{
					rc = OBJ_WRITE_SUB( pObj, idx, subidx, pn_data, Hmsg );		/* @@ */
					i++;
					subidx++;
				}
				else
				{
					rc = OBJ_WRITE_SUB( pObj, idx, subidx, pn_data, Hmsg );		/* @@ */
				}
				/*----------------------------------------------------------------------------------*/
				/*	Error Check																		*/
				/*----------------------------------------------------------------------------------*/
				if(  rc != OK )
				{
					Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�						*/
					switch(rc)
					{
					case OERR_DFIT:
						 return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)				*/
//						 break;//<S0C7>statement is unreachable
					case OERR_RONLY:
						 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�						*/
//						 break;//<S0C7>statement is unreachable
					case OERR_CALC:
						 return( REGIF_DACALERR );			/* �f�[�^�ُ�(���Z�ُ�) 				*/
//						 break;//<S0C7>statement is unreachable
					case OERR_RANGE:
						 return( REGIF_DALMTERR );			/* �f�[�^�ُ�(�㉺���ُ�)				*/
//						 break;//<S0C7>statement is unreachable
					case OERR_PRMCOPY:
					case OERR_WRITE_HANDLE:
						 return( REGIF_FOPEERR3 );			/* �����s��(���b�g�Ƃ̏�������)			*/
//						 break;//<S0C7>statement is unreachable
					default:
						 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�						*/
//						 break;//<S0C7>statement is unreachable
					}
				}
			} while ((i < Number) && (subidx < Lpx_GetMaxSubIdx( pTbl, idx ) ) );
			subidx = 0;
#else	
		/*------------------------------------------------------------------------------------------*/
		/*	�����݃f�[�^�̓����O?																	*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				if( (i + 1) < Number )
				{
					pn_data = ULONGOF_REGDATA( WrBuf[i], WrBuf[i+1] );
				}
				else
				{
					Hmsg->ErrRno = RegNo + i;			/* �ُ탌�W�X�^�ԍ�							*/
					return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
				}
			}
			else
			{
				pn_data = WrBuf[i];
				if( CHK_PNPRM_SIGNED( GET_PNPRMDEF( pTbl, idx ) ) )
				{
					pn_data = (LONG)((SHORT)pn_data); 	/* �����t���[�h�̏ꍇ�A�����g�����Ă���	*/
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^RAM������																		*/
		/*------------------------------------------------------------------------------------------*/
			rc = OBJ_WRITE( pObj, idx, pn_data, Hmsg );
			if(  rc != OK )
			{
				Hmsg->ErrRno = RegNo + i;				/* �ُ탌�W�X�^�ԍ�							*/
				switch(rc)
				{
				case OERR_DFIT:
					 return( REGIF_DAMTHERR );			/* �f�[�^�����ُ�(LONG���E)					*/
					 break;
				case OERR_RONLY:
					 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
					 break;
				case OERR_CALC:
					 return( REGIF_DACALERR );			/* �f�[�^�ُ�(���Z�ُ�) 					*/
					 break;
				case OERR_RANGE:
					 return( REGIF_DALMTERR );			/* �f�[�^�ُ�(�㉺���ُ�)					*/
					 break;
				case OERR_PRMCOPY:
				case OERR_WRITE_HANDLE:
					 return( REGIF_FOPEERR3 );			/* �����s��(���b�g�Ƃ̏�������)				*/
					 break;
				default:
					 return( REGIF_REGNOERR );			/* ���W�X�^�ԍ��ُ�							*/
					 break;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	TODO: �p�����[�^EEPROM������															*/
		/*------------------------------------------------------------------------------------------*/

		/*------------------------------------------------------------------------------------------*/
		/*	LONG�f�[�^�̏ꍇ																		*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				i++;
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}


/****************************************************************************************************/
/*																									*/
/*		Read Memory Object																			*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadMemObject(					/* Read Memory Object									*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �Ǐo���f�[�^��(WORD��)							*/
		USHORT	*RdBuf,						/* �Ǐo���f�[�^�o�b�t�@									*/
		HMSGIF	Hmsg			)			/* �Ǐo��IF�n���h��										*/
{
LONG	i;
LONG	pn_data;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
LONG	idx = RegNo - 0x4000;				/* Trace Data Index */
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^�Ǐo��																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^�Ǎ���																		*/
		/*------------------------------------------------------------------------------------------*/
			OBJ_READ( pObj, idx, (ULONG*)&pn_data, Hmsg );
			RdBuf[i] = (USHORT)(pn_data);			
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}

/****************************************************************************************************/
/*																									*/
/*		Write Memory Object																			*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWriteMemObject(					/* Write Memory Object									*/
		LONG	RegNo,						/* ���W�X�^�ԍ�											*/
		LONG	Number,						/* �������݃f�[�^��(WORD��)							*/
		USHORT	*WrBuf,						/* �������݃f�[�^�o�b�t�@								*/
		HMSGIF	Hmsg			)			/* ��������IF�n���h��									*/
{
LONG	i;
ULONG	pn_data;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
LONG	idx = RegNo - 0x4000;				/* Trace Data Index */
/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�f�[�^������																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
			pn_data = WrBuf[i];
		/*------------------------------------------------------------------------------------------*/
		/*	�p�����[�^RAM������																		*/
		/*------------------------------------------------------------------------------------------*/
			OBJ_WRITE( pObj, idx, pn_data, Hmsg );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}




 /****************************************************************************************************/
/*																									*/
/*		Memobus�FIngram��Memobus																	*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
void	LpxMbusTstUsbProcedure( HMSGIF Hmsg )
{
LONG	MsgLen;
CSIF	*Sif = Hmsg->Sif;
USB_MBUS_SERVER *pUsbMbusServer = (USB_MBUS_SERVER *)Sif->Com;
	MsgLen = MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) ); 				/* ���b�Z�[�W��́����s���� */
	MlibCopyByteMemory( Hmsg->ResBuf, pUsbMbusServer->MbusIf.TxBuf, MsgLen  );	/* TODO: ���Ƃŏ��� */
	MSGIF_SEND( Hmsg->ResBuf, MsgLen );											/* Start Tx */
	return;
}
#endif	/* <S150> */

 /****************************************************************************************************/
/*																									*/
/*		Memobus�FIngram��Memobus																	*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
void	LpxMbusTstUartProcedure( HMSGIF Hmsg )
{
LONG	MsgLen;
CSIF	*Sif = Hmsg->Sif;
UART_MBUS_SERVER *pUartMbusServer = (UART_MBUS_SERVER *)Sif->ComMbus42;
	MsgLen = MbusFuncMsgProcedure( &(pUartMbusServer->MbusIf) ); 				/* ���b�Z�[�W��́����s���� */
	MsgLen += 2;
	MlibSetCRC16(pUartMbusServer->MbusIf.TxBuf, MsgLen);
	Hmsg->ErrRno = pUartMbusServer->MbusIf.ErrRno;								/*  ���W�X�^�ǂݏo��:MEMOBUS�G���[��� */	/* <S056> */
	MlibCopyByteMemory( Hmsg->ResBuf, pUartMbusServer->MbusIf.TxBuf, MsgLen  );	/* TODO: ���Ƃŏ��� */
	MSGIF_SEND( Hmsg->ResBuf, MsgLen );											/* Start Tx */
	return;
}
#endif	/* <S150> */

/* <S150> >>>>> */
/****************************************************************************************************/
/*																									*/
/*		MEMOBUS MESSAGE MANAGER																		*/
/*																									*/
/****************************************************************************************************/
void LpxMbusMechatrolink( HMSGIF Hmsg, LONG *MsgLen )
{
	LONG	SubFuncCode;
	UCHAR	*CmdBuf;

	CmdBuf = Hmsg->CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
	SubFuncCode = (LONG)CmdBuf[4];	/* �T�u�t�@���N�V�����R�[�h   */
	
	//KpiRstLongTimer( &(MbusIfPtr->ComTimeLong) );		/* �R�}���h�����^�C���A�E�g�p	*/
	#if 0 /* �������݋֎~�������󖢑Ή� */
	RpiSetAccsLvl( &Access, MsgPtr->AccsLvl );
	#endif

	switch( SubFuncCode )/* �T�u�t�@���N�V�����R�[�h */
	{
		case MBUS_RD_CMEM_42H:		/* 01H �������̓ǂݏo���i�A���j */
			mbusFuncReadMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_WR_CMEM_42H:		/* 02H �������̏������݁i�A��)	*/
			mbusFuncWriteMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_RD_NCMEM_42H:		/* 03H �������̓ǂݏo���i��A��)*/
			mbusFuncReadMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_WR_NCMEM_42H:		/* 04H �������̏������݁i��A��)*/
			mbusFuncWriteMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_RD_MAXSIZE_42H:	/* 11H �ő�T�C�Y�ǂݏo��		*/
			mbusFuncReadMaxSize42H( Hmsg, SubFuncCode, MsgLen );
			break;

/* <S061>	Start */
//#ifdef _INFO_ID_READ_CHECK_	/* <S205> Delete */
		case MBUS_RD_INFO_42H:	/* ffH(����) ���i���ǂݏo��	*/
			mbusFuncReadProductInfo42H( Hmsg, SubFuncCode, MsgLen );
			break;
//#endif						/* <S205> Delete */
/* <S061>	End */

		default:					/* �R�}���h�s��v				*/
			mbusFuncErrorManager42H( Hmsg, FUNCCODE_ERROR, 0, MsgLen );
			break;
	}

	/* ���M�f�[�^���`�F�b�N */
	if(( *MsgLen > MBUS_MAXSIZE ) || ( *MsgLen < MBUS_MINSIZE ) )
	{
		/* ���M�f�[�^���ُ�	*/
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, MsgLen );
	}

	 /* ���J�g�������N����̃R�}���h�łȂ���΁ACRC��t������ */
	 if( !Hmsg->f.MlnkMsgIF )
	 {
		/* CRC�t�� */
		*MsgLen += 2;
		MlibSetCRC16MB( Hmsg->ResBuf, *MsgLen );
	 }
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUS MESSAGE MANAGER																		*/
/*																									*/
/****************************************************************************************************/
void LpxMbusLink( HMSGIF Hmsg )
{
	LONG	Offset;
	LONG	RouteNum;
	LONG	Length;
	LONG	MsgLen;
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	USHORT	RelayHdSize;

	/* �ϐ������� */
	MsgLen = 0;

	CmdBuf = Hmsg->CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
	ResBuf = Hmsg->ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

	/* ���p�i��	*/
	RouteNum = (CmdBuf[5] & 0x0F);				

	/* ���p�R�}���h�� */
	RelayHdSize = (USHORT)(RouteNum * MBUS_RLY_PASS_DATA_SIZE + MBUS_RLY_FIX_SIZE);

	/* ��M�E���M�o�b�t�@�|�C���^�擾	*/
	Hmsg->CmdBuf = CmdBuf + RelayHdSize;
	Hmsg->ResBuf = ResBuf + RelayHdSize;

	/* �w�b�_�̃I�t�Z�b�g�����Z�o */
	Offset = RelayHdSize;

	/* �f�[�^���R�s�[ */
	MlibCopyByteMemory( ResBuf, CmdBuf, RelayHdSize);

	ResBuf[4] |= 0x80;					/* �����t���O						*/
	ResBuf[5]  = (UCHAR)RouteNum;		/* �ʉߒi���N���A&���p�i���Z�b�g	*/

	/* ���p�i���I�[�o�[ or ���p�i���ƒʉߒi�����قȂ�ꍇ�̓G���[ */
	if( (RouteNum > MBUS_RLY_MAX_ROUTENUM) || (RouteNum != (CmdBuf[5] >> 4)) )
	{/* ���p�i���G���[ */

		/* ��M���b�Z�[�W�G���[ */

		/* ���b�Z�[�W�擪�ɖ߂� */
		Hmsg->CmdBuf = CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
		Hmsg->ResBuf = ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

		/* �f�[�^���M�i�������j */
		MSGIF_SEND( Hmsg->ResBuf, MsgLen );

		return;
	}
	else
	{/* ���p�i������ */

		/* ��M�f�[�^�T�C�Y(�o�C�g�P��)���i�[ */
		Hmsg->CmdMsgLen =(USHORT)( ((ULONG)CmdBuf[Offset-4] << 24) 
								 | ((ULONG)CmdBuf[Offset-3] << 16) 
								 | ((ULONG)CmdBuf[Offset-2] <<  8) 
								 | ((ULONG)CmdBuf[Offset-1] <<  0) );

		/* ��M�f�[�^��(�o�C�g�P��)�`�F�b�N */
		if(( Hmsg->CmdMsgLen < MBUS_MINSIZE ) || ( Hmsg->CmdMsgLen > MBUS_MAXSIZE ))
		{
			/* ��M���b�Z�[�W�G���[ */

			/* ���b�Z�[�W�擪�ɖ߂� */
			Hmsg->CmdBuf = CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
			Hmsg->ResBuf = ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

			/* �f�[�^���M�i�������j */
			MSGIF_SEND( Hmsg->ResBuf, MsgLen );

			return;	
		}
#ifdef __MYAD_CHECK__
		/* ���ǃA�h���X�`�F�b�N	*/
		else if( (FALSE == mbusFuncChkMyMsg( Hmsg, CmdBuf )) 
			  || (FALSE == mbusFuncChkMyMsg( Hmsg, Hmsg->CmdBuf )))
		{
			/* ���b�Z�[�W�擪�ɖ߂� */
			Hmsg->CmdBuf = CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
			Hmsg->ResBuf = ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

			/* �f�[�^���M�i�������j */
			MSGIF_SEND( Hmsg->ResBuf, MsgLen );

			return;
		}
#endif
		/* �t�@���N�V�����R�[�h�`�F�b�N */
		else if( Hmsg->CmdBuf[1] != MBUS_FC_42H )
		{
			/* �t�@���N�V�����R�[�h�G���[ */
			mbusFuncErrorManager42H( Hmsg, FUNCCODE_ERROR, 0, &MsgLen );
		}
#ifdef __EXAD_CHECK__
		/* �g���A�h���X�`�F�b�N */
		else if(Hmsg->CmdBuf[2] != 0xFF)
		{
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, 0, &MsgLen );
		}
#endif
		else
		{
			/* ���b�Z�[�W�������s */
			LpxMbusMechatrolink( Hmsg, &MsgLen );
		}

		/* ���b�Z�[�W�擪�ɖ߂� */
		Hmsg->CmdBuf = CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
		Hmsg->ResBuf = ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

		/* ���M�f�[�^�T�C�Y�ɒ��p�R�}���h���̃T�C�Y�����Z */
		MsgLen += RelayHdSize;

		/* �f�[�^���̃T�C�Y���Z�o */
		Length = MsgLen - Offset;

		/* ���M�f�[�^�T�C�Y(�o�C�g�P��)��ݒ� */
		mbusFuncSetArrayLongData( &Hmsg->ResBuf[ Offset - 4 ], (ULONG)Length, sizeof(LONG) );

		/* �f�[�^���M */
		MSGIF_SEND( Hmsg->ResBuf, MsgLen );
	}
}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�������ǂݏo���i�A���^��A�����ʁj													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
#ifdef __RCVLEN_CHECK__
	LONG	CmdLength;
#endif
//	UCHAR	Mode; 
	UCHAR	DataType;
	UCHAR	dIndex;
	USHORT	DataNum;
	UINT	AccessSize,RegSize;
	REG_ACC_T AccessPrm;
	LONG	i;
	LONG	ret;
	LONG	Value;
	LONG	Offset;
	LONG	RspLength;
	LONG	BlockNum;
	ULONG	BeginAddress;
	ULONG	CurrentAddress;
	REG_MANAGER_HANDLE	*hRegMngr;

	/* ��M�A���M�f�[�^�擪�A�h���X */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	Value  = 0;
	//Mode      = (CmdBuf[5] >> 4);					/* Mode		*/
	DataType  = (CmdBuf[5] & 0x0F);					/* DataType */
	DataNum   = (CmdBuf[6] << 8) + CmdBuf[7];		/* DataNum	*/
	

	/* �f�[�^�^�`�F�b�N */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{
		/* ���[�h/�f�[�^�^�G���[*/
		mbusFuncErrorManager42H( Hmsg, MODETYPE_ERROR, 0, TrnbLength );
		return;	
	}
	AccessSize   = (UCHAR)(0x0001 << (DataType-1));					/* �A�N�Z�X�T�C�Y		  */
	BeginAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE]);	/* �ǂݏo���f�[�^�A�h���X */


	/* ��M�f�[�^���A���M�f�[�^���A�f�[�^���`�F�b�N */
	if (CmdBuf[4] == MBUS_RD_CMEM_42H)
	{/* �A���������ǂݏo�� */ 
#ifdef __RCVLEN_CHECK__
		CmdLength = MBUS_HEAD_BASE_SIZE;	/* ��M�f�[�^��(���b�Z�[�W�S��) */
#endif
		RspLength = AccessSize * DataNum;	/* ���M�f�[�^��(�f�[�^��)		*/	
	}
	else
	{/* ��A���������ǂݏo�� */ 
		//CmdLength = MBUS_HEAD_FIX_SIZE + 4 * DataNum;
		RspLength = 4 * DataNum;		
	}
#ifdef __RCVLEN_CHECK__
	if(  (CmdLength != Hmsg->CmdMsgLen )
		|| (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#else
	if( (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#endif
	{	/* ���s�ǃG���[ */
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, BeginAddress, TrnbLength );
		return;
	}

	for ( i = 0; i < DataNum; i++ )
	{
		if (CmdBuf[4] == MBUS_RD_CMEM_42H)
		{/* �A���������ǂݏo�� */ 
			CurrentAddress = BeginAddress + (AccessSize * i);/* �ǂݏo���f�[�^�A�h���X	*/
		}
		else
		{/* ��A���������ǂݏo�� */ 
			CurrentAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE + 4*i] );			
		}

		/* ���z�������@���@���A�h���X�ϊ� */
		AccessPrm.MemAddr = CurrentAddress;
		//hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr,&ret);		
		hRegMngr = mbusFuncConvAddress(Hmsg->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* �������A�h���X�G���[ */
			//mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );//kira_1202
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* �i�[����f�[�^�ʒu�܂ł̃I�t�Z�b�g���v�Z	*/
		Offset = MBUS_HEAD_FIX_SIZE + i*AccessSize;

		/* �p�����[�^�����擾 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* �������A�h���X�G���[	*/
			mbusFuncErrorManager42H( Hmsg, MEMADRES_ERROR, CurrentAddress, TrnbLength );
			return;
		}
		/* �ǂݏo���p�����[�^�T�C�Y */
		RegSize = AccessPrm.cPrmDef->DataLength;

		/*------------------------------------------------------------------------------------------*/
		/*	�u���b�N�ǂݏo��																		*/
		/*------------------------------------------------------------------------------------------*/
		BlockNum = RegSize/AccessSize;		/* �u���b�N�ǂݏo���� */

		if( (BlockNum > 0) && ( DataNum > (i + BlockNum - 1)) )
		{
			if(SubFuncCode == MBUS_RD_NCMEM_42H)
			{/* ��A���������ǂݏo�� */

				dIndex = 4;

				/* �A�h���X�A���`�F�b�N */
				ret = mbusFuncCheckBlockAccess( &CmdBuf[MBUS_HEAD_FIX_SIZE + dIndex*i], 
																CurrentAddress, AccessSize,BlockNum,dIndex);
				if( ret != 0 )
				{	
					/* �A�h���X�w�肪�A�����Ă��Ȃ��ꍇ�G���[ */
					mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
					return;
				}
			}
			i = i + (BlockNum-1);				/* �u���b�N�ǂݏo���񐔕�i�����Z����	*/
		}
		else
		{	
			/* ���s�� */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* �f�[�^�ǂݏo�� */
		AccessPrm.IfHandle = Hmsg;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( Hmsg, ret, CurrentAddress, TrnbLength );
			return;
		}

		/* ���M�f�[�^�Z�b�g */
		switch (RegSize)
		{
			case 1:		/* byte�^ */
				ResBuf[Offset] = (UCHAR)(Value);
				break;
			case 2:		/* short�^ */
				mbusFuncSetArrayShortData( &ResBuf[Offset], (USHORT)Value, AccessSize );
				break;
			case 4:		/* long�^ */
				mbusFuncSetArrayLongData( &ResBuf[Offset], (ULONG)Value, AccessSize );
				break;
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_HEAD_FIX_SIZE);			/* �����f�[�^�쐬 */
	*TrnbLength = MBUS_HEAD_FIX_SIZE + AccessSize*DataNum;	/* ���M�o�b�t�@�T�C�Y���� */
}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�������������݁i�A���^��A�����ʁj													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncWriteMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
#ifdef __RCVLEN_CHECK__
	LONG	CmdLength;
#endif
//	UCHAR	Mode;
	USHORT	DataNum;
	UCHAR	AccessSize;
	UCHAR	DataType;
	UCHAR	dIndex;
	REG_ACC_T AccessPrm;
	LONG	i;
	LONG	ret;
	LONG	Value;
	LONG	Offset;
	LONG	MaxSize;
	LONG	RspLength;
	LONG	BlockNum;
	ULONG	BeginAddress; 
	ULONG	CurrentAddress;
	UINT	RegSize;
	REG_MANAGER_HANDLE	*hRegMngr;

	UCHAR  *CmdBuf;	/* ��M�o�b�t�@ */
	UCHAR  *ResBuf;	/* ���M�o�b�t�@ */

	/* ��M�A���M�f�[�^�擪�A�h���X�i�[ */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

//	Mode     = (CmdBuf[5] >> 4);					/* ���[�h	*/
	DataType = (CmdBuf[5] & 0x0F);					/* �f�[�^�^ */
	DataNum  = (CmdBuf[6] << 8) + CmdBuf[7];		/* �f�[�^��	*/

	/* �f�[�^�^�`�F�b�N */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{	/* ���[�h/�f�[�^�^�G���[*/
		mbusFuncErrorManager42H( Hmsg, MODETYPE_ERROR, 0, TrnbLength );
		return;	
	}
	AccessSize   = (UCHAR)(0x0001 << (DataType-1));					/* �A�N�Z�X�T�C�Y			*/
	BeginAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE] );	/* �������݃f�[�^�A�h���X	*/

	/* ��M�f�[�^���A���M�f�[�^���A�f�[�^���`�F�b�N */
	if (CmdBuf[4] == MBUS_WR_CMEM_42H)
	{/* �A���������������� */
#ifdef __RCVLEN_CHECK__
		CmdLength = MBUS_HEAD_BASE_SIZE + AccessSize * DataNum;	/* ��M�f�[�^��(���b�Z�[�W�S��) */
#endif
		RspLength = AccessSize * DataNum;						/* ���M�f�[�^��(�f�[�^��)		*/
		MaxSize   = MBUS_MAXSIZE - MBUS_HEAD_BASE_SIZE;			/* �ő�o�b�t�@�T�C�Y(�f�[�^��)	*/
	}
	else
	{/* ��A���������������� */ 
#ifdef __RCVLEN_CHECK__
		CmdLength = MBUS_HEAD_FIX_SIZE + 8 * DataNum;
#endif
		RspLength = 8 * DataNum;
		MaxSize   = MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE;
	}

#ifdef __RCVLEN_CHECK__
	if( (Hmsg->CmdMsgLen != CmdLength ) || (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#else
	if( (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#endif
	{	/* ���s�� */
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, BeginAddress,TrnbLength );		
		return;
	}

	for ( i = 0; i < DataNum; i++ )
	{ 
		if (CmdBuf[4] == MBUS_WR_CMEM_42H)
		{/* �A���������������� */
			CurrentAddress   = BeginAddress + (AccessSize * i);	/* �������݃f�[�^�A�h���X	*/
			Offset = MBUS_HEAD_BASE_SIZE + i*AccessSize;		/* �������݃f�[�^�I�t�Z�b�g */
			dIndex = AccessSize;
		}
		else
		{/* ��A���������������� */ 
			CurrentAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE + 8*i] );
			Offset = MBUS_HEAD_BASE_SIZE + i*sizeof(LONG)*2;
			dIndex = sizeof(LONG)*2;
		}

		/* ���z������ �� ���������A�h���X�ϊ� */
		AccessPrm.MemAddr = CurrentAddress;
		hRegMngr = mbusFuncConvAddress(Hmsg->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* �������A�h���X�G���[ */
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* �p�����[�^�����擾 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* �������A�h���X�G���[	*/
			mbusFuncErrorManager42H( Hmsg, MEMADRES_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* �p�����[�^�T�C�Y�擾 */
		RegSize = AccessPrm.cPrmDef->DataLength;

		/*------------------------------------------------------------------------------------------*/
		/*	�u���b�N��������																		*/
		/*------------------------------------------------------------------------------------------*/
		BlockNum = RegSize/AccessSize;		/* �u���b�N�������݉� */

		if( (BlockNum > 0) && ( DataNum > (i + BlockNum-1)) )
		{
			if(SubFuncCode == MBUS_WR_NCMEM_42H)
			{/* ��A���������������� */

				/* �A�h���X�A���`�F�b�N */
				ret = mbusFuncCheckBlockAccess( &CmdBuf[MBUS_HEAD_FIX_SIZE + dIndex*i], 
																CurrentAddress, AccessSize, BlockNum, dIndex);
				if( ret != 0 )
				{	/* �A�h���X�w�肪�A�����Ă��Ȃ��ꍇ�G���[ */
					mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
					return;
				}
			}
			i = i + (BlockNum-1);	/* �u���b�N�������݉񐔕�i�����Z����	*/
			Value = 0;				/* �������ݒl������						*/

			switch(AccessSize)
			{
				case 1:
					Value = mbusFuncSetValueByteData( &CmdBuf[Offset], BlockNum, dIndex );
					break;
				case 2:
					Value = mbusFuncSetValueShortData( &CmdBuf[Offset], BlockNum, dIndex );
					break;
				case 4:
					Value = mbusFuncSetValueLongData( &CmdBuf[Offset], BlockNum, dIndex );
					break;
				default:/* �����ɗ��邱�Ƃ͂Ȃ� */
					break;
			}
		}
		else
		{	/* ���s�� */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	<S166> ���샌�W�X�^�������݌��擾														*/
		/*------------------------------------------------------------------------------------------*/
		ret = mbusCheckOperationWriteHandle( &AccessPrm, Hmsg );
		if ( ret != OK ) { 
			mbusFuncErrorManager42H( Hmsg, COMPETON_ERROR, CurrentAddress, TrnbLength );
		}
		/*------------------------------------------------------------------------------------------*/

		/* �f�[�^�������� */
		AccessPrm.IfHandle = Hmsg;
		ret = (LONG)PrmWriteValue( &AccessPrm, &Value, RegSize);
		if(0 > ret)
		{
			mbusFuncErrorManager42H( Hmsg, ret, CurrentAddress, TrnbLength );
			return;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	<S166> ���샌�W�X�^�������݌����														*/
		/*------------------------------------------------------------------------------------------*/
		mbusFreeOperationWriteHandle( &AccessPrm, Hmsg );
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_HEAD_FIX_SIZE);	/* �����f�[�^�쐬 */
	*TrnbLength = MBUS_HEAD_FIX_SIZE;							/* ���M�o�b�t�@�T�C�Y���� */

}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ő�T�C�Y�ǂݏo��																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMaxSize42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
	LONG	MsgSize;

	UCHAR  *CmdBuf;//��M�o�b�t�@�擪�|�C���^
	UCHAR  *ResBuf;//���M�o�b�t�@�擪�|�C���^

	/* ��M�A���M�f�[�^�擪�A�h���X�i�[ */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

#ifdef __RCVLEN_CHECK__
	/*----------------------------------------------------------------------------------------------*/
	/*		��M�f�[�^���`�F�b�N																	*/
	/*----------------------------------------------------------------------------------------------*/
	/* ��M�f�[�^�����s�� */
	if( Hmsg->CmdMsgLen != MBUS_HEAD_FIX_SIZE )
	{	/* ���s�ǃG���[	*/
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, BeginAddress, TrnbLength );
		return;
	}
#endif

	/* �ő�T�C�Y�ǂݏo���T�C�Y */
	MsgSize = MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE;

	/* �����f�[�^�쐬 */
	ResBuf[0]  = CmdBuf[0];								/* 0:Slave Addrress		*/
	ResBuf[1]  = CmdBuf[1];								/* 1:Function Code		*/
	ResBuf[2]  = CmdBuf[2];								/* 2:Extended Address	*/
	ResBuf[3]  = CmdBuf[3];								/* 3:Reserve1			*/
	ResBuf[4]  = CmdBuf[4];								/* 4:Sub Function Code	*/
	ResBuf[5]  = CmdBuf[5];								/* 5:Mode,DataType 		*/
	ResBuf[6]  = 0;										/* 6:DataNumH			*/
	ResBuf[7]  = 0;										/* 7:DataNumL			*/
	mbusFuncSetArrayLongData( &ResBuf[8], (ULONG)MsgSize, 4);	/* 8-11 MaxMessageSize	*/

	/* ���M�o�b�t�@�T�C�Y���� */
	*TrnbLength = MBUS_HEAD_BASE_SIZE;

}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ُ퉞���ԐM����																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncErrorManager42H( HMSGIF Hmsg, LONG ErrorCode, ULONG ErrorAddress, LONG *TrnbLength )
{
	UCHAR  *CmdBuf;
	UCHAR  *ResBuf;

	/* ��M�A���M�f�[�^�擪�A�h���X */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	/* �ُ퉞���f�[�^�쐬 */
	ResBuf[0]  =  CmdBuf[0];						/* 0:Slave Addrress		*/
	ResBuf[1]  = (CmdBuf[1] | 0x80);				/* 1:Function Code		*/
	ResBuf[2]  =  CmdBuf[2];						/* 2:Extended Address	*/
	ResBuf[3]  =  CmdBuf[3];						/* 3:Reserve1			*/
	ResBuf[4]  =  CmdBuf[4];						/* 4:Sub Function Code	*/
	ResBuf[5]  =  (UCHAR)ErrorCode;					/* 5:Error Code			*/
	ResBuf[6]  =  0x00;								/* 6:Reserve2			*/
	ResBuf[7]  =  0x00;								/* 7:Reserve3			*/
	mbusFuncSetArrayLongData( &ResBuf[8], ErrorAddress, sizeof(LONG) );	/* 8-11:ErrorAddress	*/

	/* �G���[�ԐM�T�C�Y��12byte	*/
	*TrnbLength = MBUS_HEAD_BASE_SIZE;

	/* ���W�X�^�ǂݏo��:MEMOBUS�G���[���(�G���[�����A�h���X) */
	Hmsg->ErrRno = ErrorAddress;
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�o�b�t�@����ւ��֐�(ULONG)														*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncLongSwap( void *Buf )
{
	ULONG data;
#ifdef __BIG_ENDIAN__	
	/* BigEndian�̂Ƃ� */
	data =  (ULONG)((*( UCHAR *)Buf	     << 24) 
		 	      + (*((UCHAR *)Buf + 1) << 16)
			      + (*((UCHAR *)Buf + 2) <<  8)
				  + (*((UCHAR *)Buf + 3)      ) );
#else					
	/* LittleEndian�̂Ƃ� */
	/* ���m�F */
	//���͋t
	//data = (ULONG)((*( UCHAR *)Buf	         ) 
	//	         + (*((UCHAR *)Buf + 1) <<  8)
	//		     + (*((UCHAR *)Buf + 2) << 16)
	//		  	 + (*((UCHAR *)Buf + 3) << 24) );
	data = MlibCnvBig32( *((ULONG *)Buf) );
#endif
	return data;
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�o�b�t�@����ւ��֐�(USHORT)														*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncShortSwap( void *Buf )
{
	ULONG data;
#ifdef __BIG_ENDIAN__	
	/* BigEndian�̂Ƃ� */
	data = (ULONG)((*( UCHAR *)Buf      << 8) 
		         + (*((UCHAR *)Buf + 1)     ) );
#else					
	/* LittleEndian�̂Ƃ� */
	/* ���m�F */
/* <S001> */
//	data = (ULONG)((*( UCHAR *)Buf           ) 
//		 	     + (*((UCHAR *)Buf + 1) << 8 ));
	data = (ULONG)((*( UCHAR *)Buf      << 8) 
		         + (*((UCHAR *)Buf + 1)     ) );
/* <S001> */
#endif
	return data;
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�o�b�t�@�i�[�֐�(ULONG)															*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize )
{
#ifdef __BIG_ENDIAN__
/* BigEndian�̂Ƃ� */
	switch(AccessSize)
	{
		case 1:
			Buf[3] = (UCHAR)((data) >> 24);
			Buf[2] = (UCHAR)((data) >> 16);
			Buf[1] = (UCHAR)((data) >>  8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[2] = (UCHAR)((data) >> 24);
			Buf[3] = (UCHAR)((data) >> 16);
			Buf[0] = (UCHAR)((data) >>  8);
			Buf[1] = (UCHAR)( data );
			break;
		case 4:
			Buf[0] = (UCHAR)((data) >> 24);
			Buf[1] = (UCHAR)((data) >> 16);
			Buf[2] = (UCHAR)((data) >>  8);
			Buf[3] = (UCHAR)( data );
			break;
		default:
			break;
	}
#else
/* LittleEndian�̂Ƃ� */
/* <S001> */
	/* ���m�F */
	//���t
	//Buf[3] = (UCHAR)((data) >> 24);
	//Buf[2] = (UCHAR)((data) >> 16);
	//Buf[1] = (UCHAR)((data) >>  8);
	//Buf[0] = (UCHAR)( data );
//	*(ULONG *)Buf = MlibCnvBig32( data );

	switch(AccessSize)
	{
		case 1:
			Buf[3] = (UCHAR)((data) >> 24);
			Buf[2] = (UCHAR)((data) >> 16);
			Buf[1] = (UCHAR)((data) >>  8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[2] = (UCHAR)((data) >> 24);
			Buf[3] = (UCHAR)((data) >> 16);
			Buf[0] = (UCHAR)((data) >>  8);
			Buf[1] = (UCHAR)( data );
			break;
		case 4:
			Buf[0] = (UCHAR)((data) >> 24);
			Buf[1] = (UCHAR)((data) >> 16);
			Buf[2] = (UCHAR)((data) >>  8);
			Buf[3] = (UCHAR)( data );
			break;
		default:
			break;
	}
/* <S001> */
#endif
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�o�b�t�@�i�[�֐�(USHORT)															*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncSetArrayShortData( UCHAR *Buf, USHORT data, UINT AccessSize)
{
#ifdef __BIG_ENDIAN__	
/* BigEndian�̂Ƃ� */
	switch(AccessSize)
	{
		case 1:
			Buf[1] = (UCHAR)((data) >> 8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[0] = (UCHAR)((data) >> 8);
			Buf[1] = (UCHAR)( data );
			break;
		default:
			break;
	}
#else					
/* LittleEndian�̂Ƃ� */
/* <S001> */
	/* ���t */
	//Buf[1] = (UCHAR)((data) >> 8);
	//Buf[0] = (UCHAR)( data );
//	Buf[0] = (UCHAR)((data) >> 8);
//	Buf[1] = (UCHAR)( data );

	switch(AccessSize)
	{
		case 1:
			Buf[1] = (UCHAR)((data) >> 8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[0] = (UCHAR)((data) >> 8);
			Buf[1] = (UCHAR)( data );
			break;
		default:
			break;
	}
/* <S001> */
#endif
}

/****************************************************************************************************/
/*																									*/
/*		���z�����������������ϊ��֐�																*/
/*		�i�A�h���X�͈̓`�F�b�N���A���ԍ��A�p�����[�^�ԍ����i�[����j								*/
/*																									*/
/*			�߂�l�FREG_MANAGER_HANDLE															*/
/*																									*/
/****************************************************************************************************/
static REG_MANAGER_HANDLE *mbusFuncConvAddress(MEMOBUS_ADDR_MAP *AxisMap, ULONG *MemAddress )
{
	ULONG	mem_address, i;
	ULONG	AxisNumber;

	mem_address = *MemAddress;
	AxisNumber = (mem_address & 0x00F00000) >> 20;

	if(AxisNumber >= MAX_AXIS_NUMBER)
	{
		return NULL;
	}
	
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		if(AxisMap[i].AxisAddress == AxisNumber)
		{
			return AxisMap[i].hRegMngr;
		}
	}

	return NULL;
}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�A�h���X�A���`�F�b�N�i��A���@�ǂݏo���^�������݋��ʁj								*/
/*																									*/
/****************************************************************************************************/
static UCHAR mbusFuncCheckBlockAccess(UCHAR *CmdBuf, ULONG CurrentAddress, 
													UINT AccessSize, LONG BlockNum, LONG dIndex)
{
	LONG  j;
	LONG  cnt;
	ULONG NextAddress;

	cnt=0;
	for( j=0; j<(BlockNum-1); j++ )
	{
		/* �A�h���X�X�V	*/
		NextAddress = mbusFuncLongSwap( &CmdBuf[ dIndex*(j+1) ]);
		
		if( (NextAddress - CurrentAddress) == (AccessSize*(j+1)) )
		{/* �A�h���X�w�肪�A�����Ă���ꍇ */
			cnt++;	
		}
	}
	if( cnt != (BlockNum-1) )
	{	/* �A�h���X�w�肪�A�����Ă��Ȃ��ꍇ�G���[ */
		return MEMADRES_ERROR;
	}
	return 0;
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�f�[�^�i�[�֐�(Byte)																*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncSetValueByteData( UCHAR *Buf, LONG BlockNum, LONG dIndex )
{
	LONG Value,j;
	LONG BitShift;
	Value = 0;

	for( j=0; j<BlockNum; j++ )
	{
		BitShift = 8 * sizeof(CHAR) * j;
		Value += (LONG)(Buf[ dIndex*j ] << BitShift );
	}
	return Value;
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�f�[�^�i�[�֐�(SHORT)																*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncSetValueShortData( UCHAR *Buf, LONG BlockNum, LONG dIndex )
{
	LONG Value,j;
	LONG BitShift;
	Value = 0;

	for( j=0; j<BlockNum; j++ )
	{
		BitShift = 8 * sizeof(SHORT) * j;
		Value += (mbusFuncShortSwap( (void*)Buf ) << BitShift);
		Buf += dIndex;
	}
	return Value;
}

/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W�f�[�^�i�[�֐�(LONG)																*/
/*																									*/
/****************************************************************************************************/
static LONG mbusFuncSetValueLongData( UCHAR *Buf, LONG BlockNum, LONG dIndex )
{
	LONG Value,j;
	LONG BitShift;
	Value = 0;

	for( j=0; j<BlockNum; j++ )
	{
		BitShift = 8 * sizeof(LONG) * j;
		Value += (mbusFuncLongSwap( (void*)Buf ) << BitShift);
		Buf += dIndex;
	}
	return Value;
}
/* <<<<< <S150> */

/****************************************************************************************************/
/*																									*/
/*		<S166> Check and Get the Operation Write Handle												*/
/*																									*/
/****************************************************************************************************/
LONG mbusCheckOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg )
{
_enum_OPMODE modex;
ULONG  memaddr = accessprm->MemAddr & 0xFF0FFFFF;
PREGIF_CONFIG_T pRegCfg = accessprm->hRegCfg;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)pRegCfg->PrmCalParam;				/* �b�� */
/*--------------------------------------------------------------------------------------------------*/
	switch ( memaddr )
	{
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004000:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004002:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004008:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400A:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400C:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400E:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_NULL;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( modex != OPMODE_NULL )
	{
		if ( mbusGetOpeRegWriteHandle( pAxRsc->pAccessOp, modex, Hmsg ) != TRUE )
		{
			return (NG);					/* ��ch���� */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (OK);
}

/****************************************************************************************************/
/*																									*/
/*		<S166> Free Operation Write Handle															*/
/*																									*/
/****************************************************************************************************/
void mbusFreeOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg )
{
_enum_OPMODE modex;
FUN_EXECH ch;
ULONG  memaddr = accessprm->MemAddr & 0xFF0FFFFF;
PREGIF_CONFIG_T pRegCfg = accessprm->hRegCfg;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)pRegCfg->PrmCalParam;				/* �b�� */
FUN_MANAGER 	*FunMngr = pAxRsc->FnManager;							/* �b�� */
/*--------------------------------------------------------------------------------------------------*/
	switch ( memaddr )
	{
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004000:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004002:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004008:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH2;
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400A:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH2;
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400C:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;		/* @@ */
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400E:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;		/* @@ */
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;		/* @@ */
		modex = OPMODE_NULL;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Free Handle																						*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ( modex == OPMODE_OPE1 ) || ( modex == OPMODE_OPE2 ) )
	{
		if ( 0x000 == FunGetOpeModeRegister( FunMngr, ch ) )
		{
			mbusFreeOpeRegWriteHandle( pAxRsc->pAccessOp, modex, Hmsg );
		}
	}
	else if ( modex == OPMODE_TRACE )
	{
		if( 0x000 == FunMngr->AxCommon->TraceMode )
		{
			mbusFreeOpeRegWriteHandle( pAxRsc->pAccessOp, modex, Hmsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		<S166> ���샌�W�X�^�����݌��擾																*/
/*																									*/
/****************************************************************************************************/
LONG	mbusGetOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�n���h���`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Accs->Hmsg[mode] == NULL )
		{
			Accs->Hmsg[mode] = Hmsg;					/* �n���h�������g�p�ł���Ίl��				*/
			return( TRUE );								/* �n���h���l������							*/
		}
		else if( Accs->Hmsg[mode] == Hmsg )
		{
			return( TRUE );								/* �n���h���������ł���΂n�j				*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );								/* �n���h���l�����s							*/

}

/****************************************************************************************************/
/*																									*/
/*		���샌�W�X�^�����݌��������																*/
/*																									*/
/****************************************************************************************************/
void	mbusFreeOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Free Operator Write Handle (Called from FunMngr)											*/
/*--------------------------------------------------------------------------------------------------*/
		if( Accs->Hmsg[mode] == Hmsg )
		{
			Accs->Hmsg[mode]   = NULL;					/* Free Register I/F Write Handle			*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/* <S205> Start */
/****************************************************************************************************/
/*																									*/
/*		Memobus�F���i���ǂݏo��																	*/
/*																									*/
/****************************************************************************************************/
static void mbusFuncReadProductInfo42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
	UCHAR	BlockSize;
	UCHAR	ReadBlockSize;
	UCHAR	ReadSlotNo;
	UCHAR	ReadAxisNo;
	UCHAR	ReadIdNo;

	LONG	CmdBuffOffset;
	LONG	NextCmdBuffOffset;
	LONG	RspBuffOffset;
	LONG	RspBuffSize;
	LONG	ReadBlockStartOffset;


	LONG	ReadIdRegAdd;
	UCHAR	ReadDataSize;
	UCHAR	ReadDataNum;

	LONG	i;
	LONG	j;
	
	LONG	ret;
	LONG	CurrentDataLength;
	LONG	BlockDataSize;


	/* ��M�A���M�f�[�^�擪�A�h���X */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	/* �ϐ������� */
	ret = 0;
	NextCmdBuffOffset = 0;							/* ����u���b�N���i�[�o�b�t�@�I�t�Z�b�g */
	BlockDataSize = 0;							/* �e�u���N�Ɋi�[�����f�[�^�T�C�Y(�X���b�g�ԍ� �` �ŏI�f�[�^) */
	RspBuffOffset = MBUS_IDRD_RSP_HEAD_BASE_SIZE;	/* RspBuff�f�[�^���I�t�Z�b�g */
	RspBuffSize = MBUS_IDRD_RSP_HEAD_BASE_SIZE;		/* RspBuff�f�[�^�T�C�Y */
	BlockSize = CmdBuf[MBUS_IDRD_HEAD_BASE_SIZE];	/* �ǂݏo���u���b�N���擾 */

	/* �u���b�N���`�F�b�N  */
	if( BlockSize == 0) 
	{	/* �ݒ�l�͈͊O�G���[ */
		mbusFuncErrorManager42H( Hmsg, RANGOUTS_ERROR, 0, TrnbLength );
		return;
	}

	for ( i = 0; i < BlockSize; i++ )
	{
		CmdBuffOffset = MBUS_IDRD_HEAD_BASE_SIZE + 1 + NextCmdBuffOffset;	/* �ǂݏo���u���b�N�T�C�Y���i�[����Ă���CmdBuff�̃I�t�Z�b�g�Z�o */
		ReadBlockSize =  CmdBuf[CmdBuffOffset];								/* �ǂݏo���u���b�N�T�C�Y(�Ǐo��ID�̐�)�̎擾 */

		/* �u���b�N�T�C�Y�`�F�b�N */
		if( (ReadBlockSize < 4) || (ReadBlockSize > MBUS_IDRD_ID_NUM + 3) )
		{
			/* ���s�ǃG���[ */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, TrnbLength );
			return;
		}
		else
		{
			ReadSlotNo = CmdBuf[CmdBuffOffset + 1];			/* �u�X���b�g�ԍ��v�擾 */
			/* �X���b�g�ԍ��`�F�b�N  */
			if( ReadSlotNo > (MBUS_IDRD_SLOT_NUM - 1) )
			{
				/* �ݒ�l�͈͊O�G���[ */
				mbusFuncErrorManager42H( Hmsg, RANGOUTS_ERROR, 0, TrnbLength );
				return;
			}

			ReadAxisNo = CmdBuf[CmdBuffOffset + 2];			/* �u���ԍ��v�擾 */
			ReadBlockStartOffset = RspBuffOffset;			/* �u���b�N�Ǐo���J�n�I�t�Z�b�g */
			ResBuf[RspBuffOffset + 1] = ReadSlotNo;			/* �u�X���b�g�ԍ��v�����X�|���X�o�b�t�@�Ɋi�[ */
			ResBuf[RspBuffOffset + 2] = ReadAxisNo;			/* �u���ԍ��v�����X�|���X�o�b�t�@�Ɋi�[ */

			RspBuffSize = RspBuffSize + 3;					/* ��M�f�[�^�T�C�Y�X�V */
			RspBuffOffset = RspBuffOffset + 3;				/* �����o�b�t�@�I�t�Z�b�g�X�V */

			/* �����o�b�t�@�T�C�Y�`�F�b�N */
			if( RspBuffSize > MBUS_MAXSIZE )
			{	/* ���s�ǃG���[ */
				mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, TrnbLength );
				return;
			}

			/* �ǂݏo�����i���擾 */
			for ( j = 0; j < (ReadBlockSize - 3 ) ; j++ )		/* ReadBlockSize : �u���b�N�T�C�Y �` �ŏI��ID */
			{
				ReadIdNo =  CmdBuf[CmdBuffOffset + (3 + j)];	/* �ǂݏo��ID */;

				/* ID-No.�`�F�b�N */
				if( ReadIdNo > (MBUS_IDRD_ID_NUM - 1) )
				{
					/* �ݒ�l�͈͊O�G���[ */
					mbusFuncErrorManager42H( Hmsg, RANGOUTS_ERROR, 0, TrnbLength );
					return;
				}

				ReadIdRegAdd = IdRegAddTbl[ReadSlotNo][ReadIdNo];		/* �ǂݏo��ID�̃��W�X�^�A�h���X�擾 */
				ReadDataSize = IdDataSizeTbl[ReadSlotNo][ReadIdNo];		/* �ǂݏo��ID�̃f�[�^�T�C�Y�擾 */
				ReadDataNum = IdDataNumTbl[ReadSlotNo][ReadIdNo];		/* �ǂݏo��ID�̃f�[�^���擾 */

				if( (ReadIdRegAdd == 0) && (ReadDataSize == 0) && (ReadDataNum == 0) )	/* �ǂݏo��ID�f�[�^�Ȃ� */
				{
					CurrentDataLength = 1;								/* �f�[�^�� = 2[byte]�Ƃ��� */
					ResBuf[RspBuffOffset] = ReadIdNo;					/* �uID�v�����X�|���X�o�b�t�@�Ɋi�[ */
					ResBuf[RspBuffOffset + 1] = CurrentDataLength;		/* �u�ǂݏo��ID�̃f�[�^�T�C�Y�v�����X�|���X�o�b�t�@�Ɋi�[ */
					ResBuf[RspBuffOffset + 2] = 0x00;					/* �u�ǂݏo��ID�̃f�[�^�v = 0x00��ݒ� */
					RspBuffOffset = RspBuffOffset + 3;					/* �����o�b�t(�f�[�^��)�@�I�t�Z�b�g�X�V */
					RspBuffSize = RspBuffSize + 3;						/* ��M�f�[�^�T�C�Y�X�V */
				}
				else	/* �ǂݏo��ID�f�[�^���� */
				{
					CurrentDataLength = ReadDataSize * ReadDataNum;			/* ��M�f�[�^��[byte]�Z�o */
					ResBuf[RspBuffOffset] = ReadIdNo;						/* �uID�v�����X�|���X�o�b�t�@�Ɋi�[ */
					ResBuf[RspBuffOffset + 1] = CurrentDataLength;			/* �u�ǂݏo��ID�̃f�[�^�T�C�Y�v�����X�|���X�o�b�t�@�Ɋi�[ */
					RspBuffOffset = RspBuffOffset + 2;						/* �����o�b�t�@�I�t�Z�b�g�X�V */
					RspBuffSize = RspBuffSize + CurrentDataLength + 2;		/* ��M�f�[�^�T�C�Y�X�V */

					/* �����o�b�t�@�T�C�Y�`�F�b�N */
					if( (RspBuffSize > MBUS_MAXSIZE) || (ReadDataNum < MBUS_MIN_REGNUM))
					{	/* ���s�ǃG���[ */
						mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, TrnbLength );
						return;
					}

					if( ReadIdNo == 0x02 )	/* ID2(�d�l)�ǂݏo���̏ꍇ */
					{
						mbusFuncSetSpecialSpecIdData( Hmsg, RspBuffOffset);		/* �d�l�擾 */
						RspBuffOffset = RspBuffOffset + CurrentDataLength;		/* �����o�b�t�@(�f�[�^��)�I�t�Z�b�g�X�V */
					}
					else					/* ID2(�d�l)�ǂݏo���ȊO�̏ꍇ */
					{
						ret = mbusFuncSetIdData( Hmsg, ReadIdRegAdd, ReadDataSize, ReadDataNum, RspBuffOffset, TrnbLength);	/* ���i���擾(ID2�ȊO) */
						if(ret !=0 )
						{	/* ���i���擾���s */
							/* �G���[(�G���[��ʂ�ret�̓��e�ɂ��) */
							mbusFuncErrorManager42H( Hmsg, ret, ReadIdRegAdd, TrnbLength );
							return;
						}
						else
						{	/* ���i���擾���� */
							RspBuffOffset = RspBuffOffset + CurrentDataLength;	/* �����o�b�t�@(�f�[�^��)�I�t�Z�b�g�X�V */

						}
					}
				}

				BlockDataSize += CurrentDataLength + 2;	/* �ǂݏo�����u���b�N�T�C�Y[byte] : ID-No. + �f�[�^�T�C�Y + �f�[�^�� */
			}
			/* ���X�|���X�̃u���b�N�T�C�Y = �u���b�N�T�C�Y + �X���b�g�ԍ� + ���ԍ� + �e�ǂݏo�����f�[�^(ID-No. + �Ǐo���f�[�^�T�C�Y + �ǂݏo�����f�[�^) */
			ResBuf[ReadBlockStartOffset] = 3 + BlockDataSize;			/* �u���b�N�T�C�Y + �u�u���b�N�T�C�Y�v�����X�|���X�o�b�t�@�Ɋi�[ */
			NextCmdBuffOffset = NextCmdBuffOffset + ReadBlockSize;		/* ����u���b�N���i�[�o�b�t�@�I�t�Z�b�g�X�V(�u���b�N�T�C�Y���ǂ����邩�ŏ�̏����ƋA��) */

			BlockDataSize = 0;											/* �ǂݏo�����u���b�N�T�C�Y[byte] 0�N���A */
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_IDRD_HEAD_BASE_SIZE+1);	/* �����f�[�^�쐬 */
	*TrnbLength = RspBuffSize;											/* ���M�o�b�t�@�T�C�Y���� */

}

/****************************************************************************************************/
/*																									*/
/*		���iID�i�[�֐�																				*/
/*			ID0:���i�^��																			*/
/*			ID1:�@��ID																				*/
/*			ID3:�V���A��No.																			*/
/*			ID4:Soft Ver.																			*/
/*			ID5:���W�X�^ Ver.																		*/
/*			ID6:BTO�ԍ�																				*/
/*																									*/
/****************************************************************************************************/
static LONG mbusFuncSetIdData( HMSGIF Hmsg, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset, LONG *TrnbLength )
{
	UCHAR	DataType;
	UCHAR	dIndex;
	USHORT	DataNum;
	UINT	AccessSize,RegSize;
	REG_ACC_T AccessPrm;
	LONG	i;
	LONG	ret;
	LONG	Value;
	LONG	RspLength;
	LONG	BlockNum;
	ULONG	BeginAddress;
	ULONG	CurrentAddress;
	REG_MANAGER_HANDLE	*hRegMngr;

	/* ��M�A���M�f�[�^�擪�A�h���X */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

		ret = 0;

	for ( i = 0; i < ReadDataNum; i++ )
	{
			CurrentAddress = ReadIdRegAdd + (2*i);

		/* ���z�������@���@���A�h���X�ϊ� */
		AccessPrm.MemAddr = CurrentAddress;
		hRegMngr = mbusFuncConvAddress(Hmsg->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* �������A�h���X�G���[ */
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, CurrentAddress, TrnbLength );
			ret = SELAXIS_ERROR;
			return ret;
		}

		/* �p�����[�^�����擾 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* �������A�h���X�G���[	*/
			mbusFuncErrorManager42H( Hmsg, MEMADRES_ERROR, CurrentAddress, TrnbLength );
			ret = MEMADRES_ERROR;
			return ret;
		}
		/* �ǂݏo���p�����[�^�T�C�Y */
		RegSize = AccessPrm.cPrmDef->DataLength;

		/*------------------------------------------------------------------------------------------*/
		/*	�u���b�N�ǂݏo��																		*/
		/*------------------------------------------------------------------------------------------*/
		BlockNum = RegSize/ReadDataSize;		/* �u���b�N�ǂݏo���� */

		if( (BlockNum > 0) && ( ReadDataNum > (i + BlockNum - 1)) )
		{
			i = i + (BlockNum-1);				/* �u���b�N�ǂݏo���񐔕�i�����Z����	*/
		}
		else
		{	
			/* ���s�� */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
			ret = DATANUMB_ERROR;
			return ret;
		}

		/* �f�[�^�ǂݏo�� */
		AccessPrm.IfHandle = Hmsg;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( Hmsg, ret, CurrentAddress, TrnbLength );
			return ret;
		}

		/* ���M�f�[�^�Z�b�g */
		switch (RegSize)
		{
			case 1:		/* byte�^ */
				ResBuf[RspBuffOffset + (i*RegSize)] = (UCHAR)(Value);
				break;
			case 2:		/* short�^ */
				mbusFuncSetArrayShortData( &ResBuf[RspBuffOffset + (i*RegSize)], (USHORT)Value, ReadDataSize );
				break;
			case 4:		/* long�^ */
				mbusFuncSetArrayLongData( &ResBuf[RspBuffOffset + (i*RegSize)], (ULONG)Value, ReadDataSize );
				break;
		}
		
	}

	
	return ret;
}

/****************************************************************************************************/
/*																									*/
/*		���iID2:�d�l�i�[�֐�																		*/
/*																									*/
/****************************************************************************************************/
static void mbusFuncSetSpecialSpecIdData( HMSGIF Hmsg, LONG RspBuffOffset)
{

	CHAR ServoSpecialSpec[4];

	/* ��M�A���M�f�[�^�擪�A�h���X */
	UCHAR  *ResBuf;
	ResBuf = Hmsg->ResBuf;	

	/* ����d�l�ԍ�(SERVO_SPECIAL_SPEC_VER)�擾 */
	ServoSpecialSpec[0] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER >> 12) );
	ServoSpecialSpec[1] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER >> 8) );
	ServoSpecialSpec[2] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER >> 4) );
	ServoSpecialSpec[3] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER) );

	/* ����d�l��ʂ������o�b�t�@�Ɋi�[ */
	if( ServoSpecialSpec[3] == 0x00 )	/* �W���d�l�̏ꍇ */
	{
		/* 0x00 = �W���d�l�F'0' */
		ResBuf[RspBuffOffset] = NumAsciiTbl[ServoSpecialSpec[3]];

	}
	else	/* ����d�l�̏ꍇ */
	{
		/* 0x01 = FT�d�l�F'F' */
		/* 0x02 = EX�d�l�F'E' */
		/* 0x03 = Y�d�l �F'Y' */
		ResBuf[RspBuffOffset] = StrAsciiTbl[(ServoSpecialSpec[3] -  1)];

	}

	/* ����d�l�ԍ�(ASCII�R�[�h)�������o�b�t�@�Ɋi�[ */
	ResBuf[RspBuffOffset + 1] = NumAsciiTbl[ServoSpecialSpec[2]];
	ResBuf[RspBuffOffset + 2] = NumAsciiTbl[ServoSpecialSpec[1]];
	ResBuf[RspBuffOffset + 3] = NumAsciiTbl[ServoSpecialSpec[0]];

	return;
}
/* <S205> End */
/***************************************** end of file **********************************************/
