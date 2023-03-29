/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MemobusFunctions.c : MEMOBUS���b�Z�[�W�ʐM�Ǘ�												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : MEMOBUS���b�Z�[�W�ʐM�h�e�������s���B													*/
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
/*	Note	:	����	2010.09.06	T.Kira	For INGRAM												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"
#include "MemobusIF.h"
#include "MemobusFunctions.h"
//#include "WaitFunc.h"
#include "PrmAccessIf.h"
//#include "MLib.h"

/* <S205> Delete Start */
/* <S150>:�w���b�Z�[�W�ʐM�������ʉ��x�ɔ����A�uSigmaMemobus.c�v�ֈړ� */
#if 0
/* <S061>	Start */
#ifdef _INFO_ID_READ_CHECK_
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
/* 00:�T�[�{�p�b�N */	{	2,			2,			4,			2,				2,				4,					2			},
/* 01:CMD */			{	2,			0,			0,			2,				2,				0,					0			},
/* 02:Safty */			{	2,			0,			0,			2,				2,				0,					0			},
/* 03:FB */				{	2,			0,			0,			2,				0,				0,					0			},
/* 04:���[�^ */			{	2,			0,			0,			2,				0,				0,					0			},
/* 05:�G���R�[�_ */		{	2,			0,			0,			2,				2,				0,					0			}
};

/* �ǂݏo��ID�̃f�[�^��Table */
const CHAR IdDataNumTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(�^��)	ID1(�d�l)	ID2(�d�l��)	ID3(�V���A����)	ID4(Soft Ver)	ID5(���W�X�^ Ver)	ID6(BTO ��)	 */
/* 00:�T�[�{�p�b�N */	{	8,			1,			1,			8,				2,				1,					32			},
/* 01:CMD */			{	8,			0,			0,			8,				2,				0,					0			},
/* 02:Safty */			{	12,			0,			0,			12,				2,				0,					0			},
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
#endif
/* <S061>	End */
#endif
/* <S205> Delete End */


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
static void	  mbusFuncMsgManager( MEMOBUS_IF *MbusIfPtr );
static UCHAR *mbusFuncGetRcvAdr(MEMOBUS_IF *MbusIfPtr);
static UCHAR *mbusFuncGetTrnsAdr(MEMOBUS_IF *MbusIfPtr);
#ifdef __MYAD_CHECK__
static ULONG  mbusFuncChkMyMsg( MEMOBUS_IF *MbusIfPtr, UCHAR *CmdBuf );
#endif

static void	mbusLoopbackTest( MEMOBUS_IF *MbusIfPtr );

static void	mbusFuncReadMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );
static void	mbusFuncWriteMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );
static void	mbusFuncReadMaxSize42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );
static void mbusFuncErrorManager42H( MEMOBUS_IF *MbusIfPtr, LONG ErrCode, ULONG ErrAddress );

//
static LONG mbusFuncShortSwap( void *Buf );
static LONG mbusFuncLongSwap( void *Buf );

//
static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize );
static void	mbusFuncSetArrayShortData( UCHAR *Buf, USHORT data, UINT AccessSize );

//
static LONG mbusFuncSetValueByteData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueShortData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueLongData( UCHAR *Buf, LONG BlockNum, LONG dIndex );

static REG_MANAGER_HANDLE *mbusFuncConvAddress(MEMOBUS_ADDR_MAP *AxisMap, ULONG *MemAddress );
static UCHAR mbusFuncCheckBlockAccess(UCHAR *CmdBuf, ULONG CurrentAddress, 
													UINT AccessSize, LONG BlockNum, LONG dIndex);

#ifdef _INFO_ID_READ_CHECK_
static void mbusFuncReadProductInfo42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );		/* <S061> */
static void mbusFuncSetSpecialSpecIdData( MEMOBUS_IF *MbusIfPtr, LONG RspBuffOffset);	/* <S061> */
static LONG mbusFuncSetIdData( MEMOBUS_IF *MbusIfPtr, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset);		/* <S061> */
#endif
#endif	/*<S150>*/

/****************************************************************************************************/
/*																									*/
/*		Memobus IF Initialize																		*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
void	MbusFuncInitialize(MEMOBUS_IF *MbusIfPtr, MEMOBUS_ADDR_MAP *AxisCfg, LONG node_address )
{	
	INT i;

	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		MbusIfPtr->AxisMap[i] = AxisCfg[i];
	}

	/* �m�[�h�A�h���X	*/
	MbusIfPtr->NodeAddress = node_address;
}
#endif	/* <S150> */

#if 0	/* <S150> */
/****************************************************************************************************/
/*																									*/
/*		���b�Z�[�W��́����s����																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�߂�l�F���M�o�b�t�@��																		*/
/*				(0�̂Ƃ��͑��M�A�{�[�g���Ė������Ƃ���)												*/
/*																									*/
/****************************************************************************************************/
USHORT	MbusFuncMsgProcedure( MEMOBUS_IF *MbusIfPtr )
{
	LONG	Offset;
	LONG	RouteNum;
	LONG	Length;
	UCHAR  *RxBuf;
	UCHAR  *TxBuf;

	MbusIfPtr->RelayHdSize = 0;	/* ���p�R�}���h�w�b�_�T�C�Y������	*/
	MbusIfPtr->TrnbLength  = 0;	/* ���M�o�b�t�@�T�C�Y������			*/

	/* ��M�A���M�f�[�^�擪�A�h���X */
	RxBuf = &MbusIfPtr->RxBuf[MBUS_HDROFS];
	TxBuf = &MbusIfPtr->TxBuf[MBUS_HDROFS];

	/* ��M�A���M�o�b�t�@�|�C���^�擾	*/
	MbusIfPtr->CmdBuf = mbusFuncGetRcvAdr(  MbusIfPtr );
	MbusIfPtr->ResBuf = mbusFuncGetTrnsAdr( MbusIfPtr );

	/*----------------------------------------------------------------------------------------------*/
	/* MEMOBUS�R�}���h�̉��																		*/
	/*----------------------------------------------------------------------------------------------*/
	switch( RxBuf[1] )
	{	
		case MBUS_LBAK_08H:
			/*--------------------------------------------------------------------------------------*/
			/*		���[�v�o�b�N�R�}���h																	*/
			/*--------------------------------------------------------------------------------------*/
			mbusLoopbackTest( MbusIfPtr );
			break;

		case MBUS_FC_42H:
			/*--------------------------------------------------------------------------------------*/
			/*		MEMOBUS�t�@���N�V�����R�[�h														*/
			/*--------------------------------------------------------------------------------------*/
			/* ��M�f�[�^��(�o�C�g�P��)�`�F�b�N */
			if(( MbusIfPtr->RcvbLength < MBUS_MINSIZE ) || ( MbusIfPtr->RcvbLength > MBUS_MAXSIZE ))
			{
				return(0);
			}
#ifdef __MYAD_CHECK__
			/* ���ǃA�h���X�`�F�b�N	*/
			else if(FALSE == mbusFuncChkMyMsg( MbusIfPtr, RxBuf ))
			{
				return(0);	
			}
#endif	
#ifdef __EXAD_CHECK__
			/* �g���A�h���X�`�F�b�N */
			else if(MbusIfPtr->CmdBuf[2] != 0xFF)
			{
				mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, 0);
			}
#endif	
			else
			{
				/* ���b�Z�[�W�������s */
				mbusFuncMsgManager( MbusIfPtr );
			}
			break;	
		
#if 1	//2013.09.04
		case MBUS_SYSDL_50H:
			/*--------------------------------------------------------------------------------------*/
			/*		MEMOBUS�t�@�[���_�E�����[�h�R�[�h														*/
			/*--------------------------------------------------------------------------------------*/
			/* ��M�f�[�^��(�o�C�g�P��)�`�F�b�N */
			if(( MbusIfPtr->RcvbLength < MBUS_MINSIZE ) || ( MbusIfPtr->RcvbLength > MBUS_MAXSIZE ))
			{
				return(0);
			}
			else
			{
				/* �t�@�[���_�E�����[�h�������s */
				MbusFuncSystemDownLoad( MbusIfPtr );
			}
			break;
#endif

		case MBUS_RELAY_51H:		
			/*--------------------------------------------------------------------------------------*/
			/*		���p���b�Z�[�W�t�@���N�V����													*/
			/*--------------------------------------------------------------------------------------*/
			/* ���p�i��	*/
			RouteNum = (RxBuf[5] & 0x0F);				

			/* ���p�R�}���h�� */
			MbusIfPtr->RelayHdSize = (USHORT)(RouteNum * MBUS_RLY_PASS_DATA_SIZE + MBUS_RLY_FIX_SIZE);

			/* ��M�E���M�o�b�t�@�|�C���^�擾	*/
			MbusIfPtr->CmdBuf = mbusFuncGetRcvAdr( MbusIfPtr );
			MbusIfPtr->ResBuf = mbusFuncGetTrnsAdr( MbusIfPtr );
			
			/* �w�b�_�̃I�t�Z�b�g�����Z�o */
			Offset = MbusIfPtr->RelayHdSize + MBUS_HDROFS;					
			
			/* �f�[�^���R�s�[ */
			MlibCopyByteMemory( TxBuf, RxBuf, MbusIfPtr->RelayHdSize);

			TxBuf[4] |= 0x80;					/* �����t���O						*/
			TxBuf[5]  = (UCHAR)RouteNum;		/* �ʉߒi���N���A&���p�i���Z�b�g	*/

			/* ���p�i���I�[�o�[ or ���p�i���ƒʉߒi�����قȂ�ꍇ�̓G���[ */
			if( (RouteNum > MBUS_RLY_MAX_ROUTENUM) || (RouteNum != (RxBuf[5] >> 4)) )
			{/* ���p�i���G���[ */
#if 0
				MbusIfPtr->TxBuf[Offset - 6] |= 0x80;
				MbusIfPtr->TxBuf[Offset + 0] = 0x00;
				MbusIfPtr->TxBuf[Offset + 1] = MsgPtr->RxBuf[MBUS_HDROFS + 5];
				MbusIfPtr->TxBuf[Offset + 2] = 0x00;
				MbusIfPtr->TxBuf[Offset + 3] = 0x03;
				//MbusIfPtr->TrnwLength = Offset + 2;		/* �G���[�ԐM�T�C�Y��2Word				*/
				MbusIfPtr->TrnbLength = Offset + 4;			/* �G���[�ԐM�T�C�Y��4byte				*/
				//MbusIfPtr->MsgAnaSts = M3MSG_APLLAYER_ERR;
#endif
				/* ��M���b�Z�[�W�G���[ */
				return(0);		
			}
			else
			{/* ���p�i������ */

				/* ��M�f�[�^�T�C�Y(�o�C�g�P��)���i�[ */
				MbusIfPtr->RcvbLength =(USHORT)(((ULONG)MbusIfPtr->RxBuf[Offset-4] << 24) 
											 | ( (ULONG)MbusIfPtr->RxBuf[Offset-3] << 16) 
											 | ( (ULONG)MbusIfPtr->RxBuf[Offset-2] <<  8) 
											 |   (ULONG)MbusIfPtr->RxBuf[Offset-1] <<  0);

				/* ��M�f�[�^��(�o�C�g�P��)�`�F�b�N */
				if(( MbusIfPtr->RcvbLength < MBUS_MINSIZE ) || ( MbusIfPtr->RcvbLength > MBUS_MAXSIZE ))
				{
					/* ��M���b�Z�[�W�G���[ */
					return(0);	
				}
#ifdef __MYAD_CHECK__
				/* ���ǃA�h���X�`�F�b�N	*/
				else if( (FALSE == mbusFuncChkMyMsg( MbusIfPtr, RxBuf )) 
					  || (FALSE == mbusFuncChkMyMsg( MbusIfPtr, MbusIfPtr->CmdBuf )))
				{
					return(0);	
				}
#endif
				/* �t�@���N�V�����R�[�h�`�F�b�N */
				else if( MbusIfPtr->CmdBuf[1] != MBUS_FC_42H )
				{
					/* �t�@���N�V�����R�[�h�G���[ */
					mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0);	
				}
#ifdef __EXAD_CHECK__
				/* �g���A�h���X�`�F�b�N */
				else if(MbusIfPtr->CmdBuf[2] != 0xFF)
				{
					mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, 0);
				}
#endif
				else
				{
					/* ���b�Z�[�W�������s */
					mbusFuncMsgManager( MbusIfPtr );			
				}

				/* ���M�f�[�^�T�C�Y�ɒ��p�R�}���h���̃T�C�Y�����Z */
				MbusIfPtr->TrnbLength = MbusIfPtr->TrnbLength + MBUS_HDROFS + MbusIfPtr->RelayHdSize;

				/* ���p�R�}���h�T�C�Y���Z�o */
				Length = MbusIfPtr->TrnbLength - Offset;

				/* ���M�f�[�^�T�C�Y(�o�C�g�P��)��ݒ� */
				mbusFuncSetArrayLongData( &MbusIfPtr->TxBuf[ Offset - 4 ], (ULONG)Length, sizeof(LONG) );
			}
			break;

		default:
			/*--------------------------------------------------------------------------------------*/
			/*		�t�@���N�V�����R�[�h�@�T�|�[�g�O												*/
			/*--------------------------------------------------------------------------------------*/
			/* ���ǃA�h���X�`�F�b�N	*/
#ifdef __MYAD_CHECK__
			if(FALSE == mbusFuncChkMyMsg( MbusIfPtr, RxBuf ))
			{
				/* ���ǈ��ĂłȂ����A�{�[�g		*/
				return(MbusIfPtr->TrnbLength);	
			}
			else
			{
				/* ���ǈ��ā��t�@���N�V�����G���[	*/
				mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0);
			}
#else
			/* ���ǈ��ā��t�@���N�V�����G���[	*/
			mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0);
#endif
			break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	��o�C�g�̑��M�̏ꍇ�Ō�̂P�͂��݂Ȃ̂�0�N���A���Ă���									*/
	/*----------------------------------------------------------------------------------------------*/
	if( MbusIfPtr->TrnbLength & 1 )
	{
		/* 0�N���A	*/
		MbusIfPtr->TxBuf[ MbusIfPtr->TrnbLength ] = 0;
	}

	return( MbusIfPtr->TrnbLength );
}

#ifdef __MYAD_CHECK__
/****************************************************************************************************/
/*																									*/
/*		���ǈ����b�Z�[�W�̃`�F�b�N																	*/
/*																									*/
/****************************************************************************************************/
ULONG	mbusFuncChkMyMsg( MEMOBUS_IF *MbusIfPtr,UCHAR *CmdBuf )		
{

	/*----------------------------------------------------------------------------------------------*/
	/*		�����M�������b�Z�[�W�̃A�h���X=="0" or ���ǃA�h���X�̏ꍇTRUE��Ԃ�					*/
	/*----------------------------------------------------------------------------------------------*/
	if(( CmdBuf[0] == 0x00 )
	 ||( CmdBuf[0] == MbusIfPtr->NodeAddress ))
	{
		/* ���ǈ� */
		return( TRUE );	
	}
	else
	{
		/* ���ǈ��ł͂Ȃ� */
		return( FALSE );			
	}

}
#endif

/****************************************************************************************************/
/*																									*/
/*		MEMOBUS MESSAGE MANAGER																		*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncMsgManager( MEMOBUS_IF *MbusIfPtr )
{
	LONG	SubFuncCode;
	UCHAR  *CmdBuf;	
	
	CmdBuf = MbusIfPtr->CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
	SubFuncCode = (LONG)CmdBuf[4];	/* �T�u�t�@���N�V�����R�[�h   */
	
	//KpiRstLongTimer( &(MbusIfPtr->ComTimeLong) );		/* �R�}���h�����^�C���A�E�g�p	*/
	#if 0 /* �������݋֎~�������󖢑Ή� */
	RpiSetAccsLvl( &Access, MsgPtr->AccsLvl );
	#endif

	switch( SubFuncCode )/* �T�u�t�@���N�V�����R�[�h */
	{
		case MBUS_RD_CMEM_42H:		/* 01H �������̓ǂݏo���i�A���j */
			mbusFuncReadMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_WR_CMEM_42H:		/* 02H �������̏������݁i�A��)	*/
			mbusFuncWriteMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_RD_NCMEM_42H:		/* 03H �������̓ǂݏo���i��A��)*/
			mbusFuncReadMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_WR_NCMEM_42H:		/* 04H �������̏������݁i��A��)*/
			mbusFuncWriteMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_RD_MAXSIZE_42H:	/* 11H �ő�T�C�Y�ǂݏo��		*/
			mbusFuncReadMaxSize42H( MbusIfPtr, SubFuncCode );
			break;

/* <S061>	Start */
#ifdef _INFO_ID_READ_CHECK_
		case MBUS_RD_INFO_42H:	/* ffH(����) ���i���ǂݏo��	*/
			mbusFuncReadProductInfo42H( MbusIfPtr, SubFuncCode );
			break;
#endif
/* <S061>	End */

		default:					/* �R�}���h�s��v				*/
			mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0 );
			break;
	}

	/* ���M�f�[�^���`�F�b�N */
	if(( MbusIfPtr->TrnbLength > (USHORT)MBUS_MAXSIZE ) || ( MbusIfPtr->TrnbLength < MBUS_MINSIZE ) )
	{
		/* ���M�f�[�^���ُ�	*/
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, 0 );
	}
}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ُ퉞���ԐM����																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncErrorManager42H( MEMOBUS_IF *MbusIfPtr, LONG ErrorCode, ULONG ErrorAddress )
{
	UCHAR  *CmdBuf;
	UCHAR  *ResBuf;

	/* ��M�A���M�f�[�^�擪�A�h���X */
	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

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
	MbusIfPtr->TrnbLength = MBUS_HEAD_BASE_SIZE;

/* <S056>	Start */
	/* ���W�X�^�ǂݏo��:MEMOBUS�G���[���(�G���[�����A�h���X) */
	MbusIfPtr->ErrRno = ErrorAddress;
/* <S056>	End */
}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F���[�v�o�b�N�e�X�g																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusLoopbackTest( MEMOBUS_IF *MbusIfPtr )
{
ULONG	i;
USHORT	*CmdBuf = (USHORT *)(MbusIfPtr->CmdBuf);
USHORT	*ResBuf = (USHORT *)(MbusIfPtr->ResBuf);

/*--------------------------------------------------------------------------------------------------*/
/*		�����f�[�^�쐬 (CRC-16�v�Z�͕s�v)															*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i = 0; i < /*MbusStdCmd[ FuncCodeIdx ].PacketSize*/ 8 / 2; i++ )
		{
			ResBuf[i] = CmdBuf[i];
		}
//		MbusIfPtr->TrnbLength = sizeof(MbusIfPtr->RxBuf[ MSG_MAXBUFSZB ]);		/* Set size of Response data	*/
		MbusIfPtr->TrnbLength = 8;		/* Set size of Response data	*/

		return;
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
/*		Memobus�F�������ǂݏo���i�A���^��A�����ʁj													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
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

	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

	Value  = 0;
	//Mode      = (CmdBuf[5] >> 4);					/* Mode		*/
	DataType  = (CmdBuf[5] & 0x0F);					/* DataType */
	DataNum   = (CmdBuf[6] << 8) + CmdBuf[7];		/* DataNum	*/
	

	/* �f�[�^�^�`�F�b�N */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{
		/* ���[�h/�f�[�^�^�G���[*/
		mbusFuncErrorManager42H( MbusIfPtr, MODETYPE_ERROR, 0 );
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
	if(  (CmdLength != MbusIfPtr->RcvbLength )
		|| (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#else
	if( (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#endif
	{	/* ���s�ǃG���[ */
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, BeginAddress );	
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
		hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* �������A�h���X�G���[ */
			//mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );//kira_1202
			mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, CurrentAddress );
			return;
		}

		/* �i�[����f�[�^�ʒu�܂ł̃I�t�Z�b�g���v�Z	*/
		Offset = MBUS_HEAD_FIX_SIZE + i*AccessSize;

		/* �p�����[�^�����擾 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* �������A�h���X�G���[	*/
			mbusFuncErrorManager42H( MbusIfPtr, MEMADRES_ERROR, CurrentAddress );
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
					mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
					return;
				}
			}
			i = i + (BlockNum-1);				/* �u���b�N�ǂݏo���񐔕�i�����Z����	*/
		}
		else
		{	
			/* ���s�� */
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
			return;
		}

		/* �f�[�^�ǂݏo�� */
		AccessPrm.IfHandle = MbusIfPtr;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
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
	MbusIfPtr->TrnbLength = MBUS_HEAD_FIX_SIZE + AccessSize*DataNum;	/* ���M�o�b�t�@�T�C�Y���� */
}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�������������݁i�A���^��A�����ʁj													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncWriteMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
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
	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

//	Mode     = (CmdBuf[5] >> 4);					/* ���[�h	*/
	DataType = (CmdBuf[5] & 0x0F);					/* �f�[�^�^ */
	DataNum  = (CmdBuf[6] << 8) + CmdBuf[7];		/* �f�[�^��	*/

	/* �f�[�^�^�`�F�b�N */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{	/* ���[�h/�f�[�^�^�G���[*/
		mbusFuncErrorManager42H( MbusIfPtr, MODETYPE_ERROR, 0 );
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
	if( (MbusIfPtr->RcvbLength != CmdLength ) || (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#else
	if( (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#endif
	{	/* ���s�� */
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, BeginAddress );		
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
		hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* �������A�h���X�G���[ */
			//mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
			mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, CurrentAddress );//kira_1202
			return;
		}

		/* �p�����[�^�����擾 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* �������A�h���X�G���[	*/
			mbusFuncErrorManager42H( MbusIfPtr, MEMADRES_ERROR, CurrentAddress );
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
					mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
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
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
			return;
		}

		/* �f�[�^�������� */
		AccessPrm.IfHandle = MbusIfPtr;
		ret = (LONG)PrmWriteValue( &AccessPrm, &Value, RegSize);
		if(0 > ret)
		{
			mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
			return;
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_HEAD_FIX_SIZE);	/* �����f�[�^�쐬 */
	MbusIfPtr->TrnbLength = MBUS_HEAD_FIX_SIZE;					/* ���M�o�b�t�@�T�C�Y���� */

}

/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ő�T�C�Y�ǂݏo��																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMaxSize42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
{
	LONG	MsgSize;

	UCHAR  *CmdBuf;//��M�o�b�t�@�擪�|�C���^
	UCHAR  *ResBuf;//���M�o�b�t�@�擪�|�C���^

	/* ��M�A���M�f�[�^�擪�A�h���X�i�[ */
	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

#ifdef __RCVLEN_CHECK__
	/*----------------------------------------------------------------------------------------------*/
	/*		��M�f�[�^���`�F�b�N																	*/
	/*----------------------------------------------------------------------------------------------*/
	/* ��M�f�[�^�����s�� */
	if( MbusIfPtr->RcvbLength != MBUS_HEAD_FIX_SIZE )
	{	/* ���s�ǃG���[	*/
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, BeginAddress );		
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
	MbusIfPtr->TrnbLength = MBUS_HEAD_BASE_SIZE;

}
#endif	/* <S150> */

/****************************************************************************************************/
/*																									*/
/*	���b�Z�[�W�o�b�t�@����ւ��֐�																	*/
/*																									*/
/****************************************************************************************************/
void	mbusFuncBufSwap( UCHAR *Buf )
{
#ifdef __BIG_ENDIAN__
/* BigEndian�̂Ƃ� */

	LONG	i;
	LONG	Num;
	UCHAR	tmp[4];

	Num = MBUS_MAXSIZE / 4;

	for( i=0; i<Num; i++ )
	{
		tmp[0] = Buf[i*4    ]; 
		tmp[1] = Buf[i*4 + 1]; 
		tmp[2] = Buf[i*4 + 2]; 
		tmp[3] = Buf[i*4 + 3]; 

		Buf[i*4    ] = tmp[3];
		Buf[i*4 + 1] = tmp[2];
		Buf[i*4 + 2] = tmp[1];
		Buf[i*4 + 3] = tmp[0];
	}
#else
/* LittleEndian�̂Ƃ� */
	/* ���m�F */
	/*�������Ȃ�*/

#endif
}

#if 0	/* <S150> */
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

/****************************************************************************************************/
/*																									*/
/*		MEMOBUS���b�Z�[�W��M�o�b�t�@�A�h���X�񋟊֐�												*/
/*																									*/
/*			�߂�l�F���M�o�b�t�@�̐擪�A�h���X														*/
/*																									*/
/****************************************************************************************************/
static UCHAR	*mbusFuncGetRcvAdr( MEMOBUS_IF *MbusIfPtr )
{
	return( &MbusIfPtr->RxBuf[ MbusIfPtr->RelayHdSize + MBUS_HDROFS ] );		
}

/****************************************************************************************************/
/*																									*/
/*		MEMOBUS���b�Z�[�W���M�o�b�t�@�A�h���X�񋟊֐�												*/
/*																									*/
/*			�߂�l�F��M�o�b�t�@�̐擪�A�h���X														*/
/*																									*/
/****************************************************************************************************/
 static UCHAR	*mbusFuncGetTrnsAdr( MEMOBUS_IF *MbusIfPtr )
{
	return( &MbusIfPtr->TxBuf[ MbusIfPtr->RelayHdSize + MBUS_HDROFS ] );		
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

/* <S061>	Start */
#ifdef _INFO_ID_READ_CHECK_
/****************************************************************************************************/
/*																									*/
/*		Memobus�F���i���ǂݏo��																	*/
/*																									*/
/****************************************************************************************************/
static void mbusFuncReadProductInfo42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
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

	LONG	ReadIdRegAdd;
	UCHAR	ReadDataSize;
	UCHAR	ReadDataNum;

	LONG	i;
	LONG	j;
	
	LONG	ret;
	LONG	CurrentDataLength;

	/* ��M�A���M�f�[�^�擪�A�h���X */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

	/* �ϐ������� */
	ret = 0;
	NextCmdBuffOffset = 0;							/* ����u���b�N���i�[�o�b�t�@�I�t�Z�b�g */
	RspBuffOffset = MBUS_IDRD_RSP_HEAD_BASE_SIZE;	/* RspBuff�f�[�^���I�t�Z�b�g */
	RspBuffSize = MBUS_IDRD_RSP_HEAD_BASE_SIZE;		/* RspBuff�f�[�^�T�C�Y */
	BlockSize = CmdBuf[MBUS_IDRD_HEAD_BASE_SIZE];	/* �ǂݏo���u���b�N���擾 */

	/* �u���b�N���`�F�b�N  */
	if( BlockSize == 0) 
	{	/* �ݒ�l�͈͊O�G���[  */
		mbusFuncErrorManager42H( MbusIfPtr, RANGOUTS_ERROR, 0 );	
		return;
	}

	for ( i = 0; i < BlockSize; i++ )
	{
		CmdBuffOffset = MBUS_IDRD_HEAD_BASE_SIZE + 1 + NextCmdBuffOffset;	/* �ǂݏo���u���b�N�T�C�Y���i�[����Ă���CmdBuff�̃I�t�Z�b�g�Z�o */
		ReadBlockSize =  CmdBuf[CmdBuffOffset];								/* �ǂݏo���u���b�N�T�C�Y�̎擾 */

		/* �u���b�N�T�C�Y�`�F�b�N */
		if( (ReadBlockSize < 3) || (ReadBlockSize > MBUS_IDRD_ID_NUM + 2) )
		{
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, 0 );	
			return;
		}
		else
		{
			ReadSlotNo = CmdBuf[CmdBuffOffset + 1];			/* �u�X���b�g�ԍ��v�擾 */
			/* �X���b�g�ԍ��`�F�b�N  */
			if( ReadSlotNo > (MBUS_IDRD_SLOT_NUM - 1) )
			{
				/* �ݒ�l�͈͊O�G���[ */
				mbusFuncErrorManager42H( MbusIfPtr, RANGOUTS_ERROR, 0 );	
				return;
			}

			ReadAxisNo = CmdBuf[CmdBuffOffset + 2];			/* �u���ԍ��v�擾 */
			ResBuf[RspBuffOffset] = ReadBlockSize;			/* �u�u���b�N�T�C�Y�v�����X�|���X�o�b�t�@�Ɋi�[ */
			ResBuf[RspBuffOffset + 1] = ReadSlotNo;			/* �u�X���b�g�ԍ��v�����X�|���X�o�b�t�@�Ɋi�[ */
			ResBuf[RspBuffOffset + 2] = ReadAxisNo;			/* �u���ԍ��v�����X�|���X�o�b�t�@�Ɋi�[ */

			RspBuffSize = RspBuffSize + 3;					/* ��M�f�[�^�T�C�Y�X�V */
			RspBuffOffset = RspBuffOffset + 3;				/* �����o�b�t�@�I�t�Z�b�g�X�V */

			/* �����o�b�t�@�T�C�Y�`�F�b�N */
			if( RspBuffSize > MBUS_MAXSIZE )
			{	/* ���s�ǃG���[ */
				mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, 0 );	
				return;
			}

			/* �ǂݏo�����i���擾 */
			for ( j = 0; j < (ReadBlockSize - 2 ) ; j++ )
			{
				ReadIdNo =  CmdBuf[CmdBuffOffset + (3 + j)];	/* �ǂݏo��ID */;

				/* ID-No.�`�F�b�N */
				if( ReadIdNo > (MBUS_IDRD_ID_NUM - 1) )
				{
					/* �ݒ�l�͈͊O�G���[ */
					mbusFuncErrorManager42H( MbusIfPtr, RANGOUTS_ERROR, 0 );	
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
						mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, ReadIdRegAdd );	
						return;
					}


					if( ReadIdNo == 0x02 )	/* ID2(�d�l)�ǂݏo���̏ꍇ */
					{
						mbusFuncSetSpecialSpecIdData( MbusIfPtr, RspBuffOffset);	/* �d�l�擾 */
						RspBuffOffset = RspBuffOffset + CurrentDataLength;			/* �����o�b�t�@(�f�[�^��)�I�t�Z�b�g�X�V */
					}
					else					/* ID2(�d�l)�ǂݏo���ȊO�̏ꍇ */
					{
						ret = mbusFuncSetIdData( MbusIfPtr, ReadIdRegAdd, ReadDataSize, ReadDataNum, RspBuffOffset);
						if(ret !=0 )
						{
							/* �G���[ */
							mbusFuncErrorManager42H( MbusIfPtr, ret, ReadIdRegAdd );
							return;
						}
						else
						{
							RspBuffOffset = RspBuffOffset + CurrentDataLength;	/* �����o�b�t�@(�f�[�^��)�I�t�Z�b�g�X�V */

						}
					}
				}
			}
			
			NextCmdBuffOffset = NextCmdBuffOffset + ReadBlockSize + 1;	/* ����u���b�N���i�[�o�b�t�@�I�t�Z�b�g�X�V */
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_IDRD_HEAD_BASE_SIZE+1);	/* �����f�[�^�쐬 */
	MbusIfPtr->TrnbLength = RspBuffSize;								/* ���M�o�b�t�@�T�C�Y���� */
	
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
static LONG mbusFuncSetIdData( MEMOBUS_IF *MbusIfPtr, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset)
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

	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

		ret = 0;

	for ( i = 0; i < ReadDataNum; i++ )
	{
			CurrentAddress = ReadIdRegAdd + (2*i);

		/* ���z�������@���@���A�h���X�ϊ� */
		AccessPrm.MemAddr = CurrentAddress;
		hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* �������A�h���X�G���[ */
			mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, CurrentAddress );
			ret = SELAXIS_ERROR;
			return ret;
		}

		/* �p�����[�^�����擾 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* �������A�h���X�G���[	*/
			mbusFuncErrorManager42H( MbusIfPtr, MEMADRES_ERROR, CurrentAddress );
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
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
			ret = DATANUMB_ERROR;
			return ret;
		}

		/* �f�[�^�ǂݏo�� */
		AccessPrm.IfHandle = MbusIfPtr;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
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
static void mbusFuncSetSpecialSpecIdData( MEMOBUS_IF *MbusIfPtr, LONG RspBuffOffset)
{

	CHAR ServoSpecialSpec[4];

	/* ��M�A���M�f�[�^�擪�A�h���X */
	UCHAR  *ResBuf;
	ResBuf = MbusIfPtr->ResBuf;	

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

#endif
/* <S061>	End */
#endif	/* <S150> */

/***************************************** end of file **********************************************/
