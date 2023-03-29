/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FirmDownload.c : MEMOBUS�t�@�[���_�E�����[�h�Ǘ�											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : MEMOBUS�t�@�[���_�E�����[�h�ʐM�h�e�������s���B											*/
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
/*	Note	:	����	2013.07.08	Y.Tanaka	For Mercury											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "MemobusIF.h"
#include "MemobusFunctions.h"
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
#include "PrmAccessIf.h"
#endif	/*_MERCURY_PLATFORM_*/
#include "MLib.h"
#include "FirmDownload.h"

//@@���Ή�
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
#include "MercuryGlobals.h"
#endif	/*_MERCURY_PLATFORM_*/
#include "Sif.h"
#include "MsgIfDef.h"
#include "KnlApi.h"

#include "elf.h"	/* �Í����t�H�[�}�b�g�Ή� <S146> */
#ifdef _MERCURY_PLATFORM_	/* <S190> */
#include "SpecCommon.h"
//<S1A2>#define SERVO_ID			0x071	/* �T�[�{�@��ID : SFD7S_M2 = 0x0071 */
extern USHORT	ServoId4Boot;				/* �T�[�{�@��ID (�u�[�g���s���ɐݒ肷��)*/
#define SERVO_ID	ServoId4Boot
#endif	/* <S190> */


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
SHORT	MbusFuncSystemDownLoad( HMSGIF Hmsg );
LONG	MbusFuncSysDLGetMsgLength( UCHAR *CmdBuf );
void	MbusFuncSystemDownLoadCheckSum( void );	/* <S146> */
void	MbusFuncSysDlInitWork( void );	/* <S1A2> */

static SHORT	mbusFuncSysDLReadID50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );				/* 00H ID���ǂݏo��	*/
static SHORT	mbusFuncSysDLReadSwVersion50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 01H �o�[�W�����Ǐo��	*/
static SHORT	mbusFuncSysDLReadCondition50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 02H �v���O���������ݏ����擾	*/
static SHORT	mbusFuncSysDLStart50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );				/* 03H �_�E�����[�h�J�n�w��		*/
static SHORT	mbusFuncSysDLEraseBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 04H �u���b�N�����w��	*/
static SHORT	mbusFuncSysDLWriteBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 05H �v���O���������ݎw��		*/
static SHORT	mbusFuncSysDLRecvEraseInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 06H �u���b�N�������w��(�������]���̂�.���������Ȃ�)		*/
static SHORT	mbusFuncSysDLRecvWriteInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 07H �v���O�������M�w��(�v���O�������M�̂�.���������Ȃ�)		*/
static SHORT	mbusFuncSysDLEraseAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 08H �u���b�N�ꊇ�����w��		*/
static SHORT	mbusFuncSysDLWriteAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 09H �v���O�����ꊇ�����ݎw��		*/
static SHORT	mbusFuncSysDLFinish50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);				/* 0AH �_�E�����[�h�I���w��		*/
static SHORT	mbusFuncSysDLReadProg50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);			/* 0BH �v���O�����Ǐo���w��		*/
static SHORT	mbusFuncSysDLReboot50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);				/* 0CH �v���O�����ċN���w��		*/
static SHORT	mbusFuncSysDLChangeSifComSpd50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);		/* 0DH �V���A���h�e���x�ύX		*/
static SHORT	mbusFuncSysDLReadHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);			/* 0EH �_�E�����[�h����Ǐo��		*/
static SHORT	mbusFuncSysDLWriteHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);		/* 0FH �_�E�����[�h����������		*/
static SHORT	mbusFuncSysDLGetCheckSumResult50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);	/* 10H �`�F�b�N�T�����ʎ擾		*//*<S146>*/
static SHORT	mbusFuncErrorManager50H( HMSGIF Hmsg, LONG ErrorCode, USHORT *TrnbLength);			/* �ُ퉞���ԐM���� */
/*--------------------------------------------------------------------------------------------------*/
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize );
#endif	/*_MERCURY_PLATFORM_*/
/*--------------------------------------------------------------------------------------------------*/
static SHORT	SysDLEraseBlock( ULONG EraseAddr, UCHAR MpuNo, UCHAR FlashNum );					/* �u���b�N��������				*/
static SHORT	SysDLWritePage( ULONG WriteAddr, ULONG* WriteBuf, UCHAR MpuNo, UCHAR FlashNum );	/* �y�[�W�������ݏ���			*/
/*--------------------------------------------------------------------------------------------------*/
/* <S146> >>>>> */
static SHORT	checkElfZipSum( UCHAR *pTopAddress );												/* ELFZIP�t�@�C���̃T���l�`�F�b�N	*/
static SHORT	checkAreaSum( UCHAR *pTopAddress, LONG Length );									/* �w��̈�̃T���l�`�F�b�N			*/
static SHORT	checkSum( void );																	/* ��M�o�C�i���̃T���l�`�F�b�N		*/
/* <<<<< <S146> */

/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/
SYSDL	SysDl;

/****************************************************************************************************/
/*																									*/
/*		MEMOBUS FirmDownload MANAGER																*/
/*																									*/
/****************************************************************************************************/
SHORT	MbusFuncSystemDownLoad( HMSGIF Hmsg )
{
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	UCHAR	SubFuncCode;
	UCHAR	MpuNo;
	USHORT	TrnbLength;
	SHORT	rc = MBUS_BUSY;

//	SysDl.DlStatus = 0x0001;	/* for debug */
	if(SYSDL_CHK_DLMODE_ON())			/* <S077> */
	{
		KpiRstLongTimer( &(SysDl.BpsChkTime) );	/* Reset BpsChkTimer	<S041>		*/
	}

	CmdBuf = Hmsg->CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
	ResBuf = Hmsg->ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

	SubFuncCode = CmdBuf[3];	/* �T�u�t�@���N�V�����R�[�h   */
	MpuNo = CmdBuf[4];
	
	//KpiRstLongTimer( &(Hmsg->ComTimeLong) );		/* �R�}���h�����^�C���A�E�g�p	*/
	#if 0 /* �������݋֎~�������󖢑Ή� */
	RpiSetAccsLvl( &Access, MsgPtr->AccsLvl );
	#endif

	/* ���ʉ����f�[�^�쐬 */
	ResBuf[0] = CmdBuf[0];	/* slave address	*/
	ResBuf[1] = CmdBuf[1];	/* function code	*/
	ResBuf[2] = CmdBuf[2];	/* sub function code high	*/
	ResBuf[3] = CmdBuf[3];	/* sub function code low	*/
	ResBuf[4] = CmdBuf[4];	/* MPU No	*/
	ResBuf[5] = 0x00;	/* reserved	*/

	switch( SubFuncCode )/* �T�u�t�@���N�V�����R�[�h */
	{
		case MBUS_DL_ID_RD:			/* 00H ID���ǂݏo�� */
			rc = mbusFuncSysDLReadID50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_SWVER_RD:			/* 01H �o�[�W�����Ǐo�� */
			rc = mbusFuncSysDLReadSwVersion50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_WAY_RD:		/* 02H �v���O���������ݏ����擾	*/
			rc = mbusFuncSysDLReadCondition50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_START:		/* 03H �_�E�����[�h�J�n�w��		*/
			rc = mbusFuncSysDLStart50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGDEL:		/* 04H �u���b�N�����w��	*/
			rc = mbusFuncSysDLEraseBlock50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGWR:	/* 05H �v���O���������ݎw��		*/
			rc = mbusFuncSysDLWriteBlock50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_DELSND:	/* 06H �u���b�N�������w��(�������]���̂�.���������Ȃ�)		*/
			rc = mbusFuncSysDLRecvEraseInfo50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGSND:	/* 07H �v���O�������M�w��(�v���O�������M�̂�.���������Ȃ�)		*/
			rc = mbusFuncSysDLRecvWriteInfo50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGDELA:	/* 08H �u���b�N�ꊇ�����w��		*/
			rc = mbusFuncSysDLEraseAll50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGWRA:	/* 09H �v���O�����ꊇ�����ݎw��		*/
			rc = mbusFuncSysDLWriteAll50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_END:	/* 0AH �_�E�����[�h�I���w��		*/
			rc = mbusFuncSysDLFinish50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGRD:	/* 0BH �v���O�����Ǐo���w��		*/
			rc = mbusFuncSysDLReadProg50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_REBOOT:	/* 0CH �v���O�����ċN���w��		*/
			rc = mbusFuncSysDLReboot50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_SIFCHG:	/* 0DH �V���A���h�e���x�ύX		*/
			rc = mbusFuncSysDLChangeSifComSpd50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_HIS_RD:	/* 0EH �_�E�����[�h����Ǐo��		*/
			rc = mbusFuncSysDLReadHistory50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_HIS_WR:	/* 0FH �_�E�����[�h����������		*/
			rc = mbusFuncSysDLWriteHistory50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_CHKSUM:	/* 10H �`�F�b�N�T�����ʎ擾		*/ /* <S146> */
			rc = mbusFuncSysDLGetCheckSumResult50H( Hmsg, MpuNo, &TrnbLength );
			break;
		default:					/* �R�}���h�s��v				*/
			rc = mbusFuncErrorManager50H( Hmsg, FUNCCODE_ERROR, &TrnbLength );
			break;
	}

	if( rc != MBUS_BUSY )
	{
		/* ���M�f�[�^���`�F�b�N */
		if(( TrnbLength > MBUS_MAXSIZE ) || ( TrnbLength < MBUS_DL_MIN_RSPSIZE ) )
		{
			/* ���M�f�[�^���ُ�	*/
			rc = mbusFuncErrorManager50H( Hmsg, DATANUMB_ERROR, &TrnbLength );
		}

		/* CRC�t�� */
		MlibSetCRC16MB( Hmsg->ResBuf, TrnbLength );
	}
	else
	{
		/* ������ */
		TrnbLength = 0;
	}

	/* �f�[�^���M */
	Hmsg->Sif->StxRes( Hmsg->Sif, Hmsg->ResBuf, TrnbLength );

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F00H ID���ǂݏo��																	*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadID50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;

	/* ���M�f�[�^�擪�A�h���X		*/
	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_ID_RD_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		ResBuf[6] = 0x00;	/* reserved	*/
		ResBuf[7] = FLASH_NUM;	/* Flash Num	*/
#if 0	//�ŏI�I�ɂ͍\���̂���������
		ResBuf[8] = (SysDl.DlSts1 >> 8);	/* Download Status1	*/
		ResBuf[9] = (SysDl.DlSts1 & 0x00FF);	/* Download Status1	*/
		ResBuf[10] = (SysDl.DlSts2 >> 8);	/* Download Status2	*/
		ResBuf[11] = (SysDl.DlSts2 & 0x00FF);	/* Download Status2	*/
		ResBuf[12] = (SysDl.DlSts3 >> 8);	/* Download Status3	*/
		ResBuf[13] = (SysDl.DlSts3 & 0x00FF);	/* Download Status3	*/
		ResBuf[14] = (SysDl.DlSts4 >> 8);	/* Download Status4	*/
		ResBuf[15] = (SysDl.DlSts4 & 0x00FF);	/* Download Status4	*/
//		ResBuf[16] = 0x00;	/* Servo ID	*///�W���^(��۸�/��ٽ��`)�ŉ��Ή�
//		ResBuf[17] = 0x28;	/* Servo ID	*///�W���^(��۸�/��ٽ��`)�ŉ��Ή�
		ResBuf[16] = (UCHAR)(SERVO_ID>>0);	/* Servo ID	*/
		ResBuf[17] = (UCHAR)(SERVO_ID>>2);	/* Servo ID	*/
#else	//�f�o�b�O�p�i�Œ�l�j
		ResBuf[8] = (SYSDL_STS1 >> 8);	/* Download Status1	*/
		ResBuf[9] = (SYSDL_STS1 & 0x00FF);	/* Download Status1	*/
		ResBuf[10] = (SYSDL_STS2 >> 8);	/* Download Status2	*/
		ResBuf[11] = (SYSDL_STS2 & 0x00FF);	/* Download Status2	*/
		ResBuf[12] = (SYSDL_STS3 >> 8);	/* Download Status3	*/
		ResBuf[13] = (SYSDL_STS3 & 0x00FF);	/* Download Status3	*/
		ResBuf[14] = (SYSDL_STS4 >> 8);	/* Download Status4	*/
		ResBuf[15] = (SYSDL_STS4 & 0x00FF);	/* Download Status4	*/
//		ResBuf[16] = 0x00;	/* Servo ID	*///�W���^(��۸�/��ٽ��`)�ŉ��Ή�
//		ResBuf[17] = 0x28;	/* Servo ID	*///�W���^(��۸�/��ٽ��`)�ŉ��Ή�
		ResBuf[16] = (UCHAR)(SERVO_ID>>4);	/* Servo ID <S150>	*/
		ResBuf[17] = (UCHAR)(SERVO_ID>>0);	/* Servo ID <S150>	*/
#endif
		ResBuf[18] = 0x00;	/* Flash0 Manufacture ID	*///Flash����2�ȏ�ɂȂ������̊g���������K�v�i2�ȏ�ɂȂ�́H�j
		ResBuf[19] = 0x00;	/* Flash0 Manufacture ID	*/
		ResBuf[20] = 0x00;	/* Flash0 Device ID	*/
		ResBuf[21] = 0x00;	/* Flash0 Device ID	*/
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* �Ƃ肠������ɂ��Ă���	*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		rc = MBUS_NG;

		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F01H �o�[�W�����Ǐo��																*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadSwVersion50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;

	/* ���M�f�[�^�擪�A�h���X		*/
	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_SWVER_RD_RSPSIZE;			/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		ResBuf[6] = 0x00;						/* Ver��`��=2					*/
		ResBuf[7] = 0x02;						/* Ver��`��=2					*/
#if 0	//�ŏI�I�ɂ͍\���̂���������
		ResBuf[8] = ( Kprm.Bt2MswVersion >> 8 );		/* Boot2 Ver		*/
		ResBuf[9] = ( Kprm.Bt2MswVersion & 0x00FF );	/* Boot2 Ver		*/
		ResBuf[10] = ( SvSw.MswVersion >> 8 );	 		/* Runtime Ver		*/
		ResBuf[11] = ( SvSw.MswVersion & 0x00FF ); 		/* Runtime Ver		*/
#else	//�f�o�b�O�p�i�Œ�l�j
		ResBuf[8] = (SERVO_BOOT_VER >> 4);		/* Boot2 Ver		*/
		ResBuf[9] = (SERVO_BOOT_VER >> 0);		/* Boot2 Ver		*/
//<S072>		ResBuf[10] = 0x00;		/* Runtime Ver		*/
//<S072>		ResBuf[11] = 0x01; 		/* Runtime Ver		*/
		ResBuf[10] = (SERVO_FIRM_VER >> 24 ) & 0x00FF;		/* <S072> TODO: ��ŏ��� */
		ResBuf[11] = (SERVO_FIRM_VER >> 16 ) & 0x00FF;		/* <S072> TODO: ��ŏ��� */
#endif
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* �Ƃ肠������ɂ��Ă���	*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return (MBUS_OK);								/* <S072> �߂�l���Ȃ����� */
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F02H �v���O���������ݏ����擾														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadCondition50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;

	/* ���M�f�[�^�擪�A�h���X		*/
	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_WAY_RD_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
	case 0x01:	/* OptionCPU (NetX etc...) *//* ���ʂł�����H */
		ResBuf[6] = ( SYSDL_WAY >> 8 );		/* �_�E�����[�h�������ݏ���		*/
		ResBuf[7] = ( SYSDL_WAY & 0x00FF );	/* �_�E�����[�h�������ݏ���		*/
		ResBuf[8] = ( SYSDL_1DLEN >> 8 );	 		/* 1���b�Z�[�W������̃o�C�g��		*/
		ResBuf[9] = ( SYSDL_1DLEN & 0x00FF ); 		/* 1���b�Z�[�W������̃o�C�g��		*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F03H �_�E�����[�h�J�n�w��															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLStart50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;
	
	*TrnbLength = MBUS_DL_START_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		/* �x�[�X�u���b�N�ł���΃_�E�����[�h�J�n	*/
		if( SYSDL_CHK_BASE_BLOCK() )
		{
			SYSDL_DLMODE_ON();	/* download mode on	*/
/* <S146> >>>>> */
			/* Sum check flag => off */
			SysDl.MicroSumCheckF = FALSE;
			SysDl.MirSumCheckF = FALSE;
			SysDl.RtmSumCheckF = FALSE;
			SysDl.BootSumCheckF = FALSE;
			/* Round�ւ̈˗��֘A�t���O�ݒ� */
			SysDl.CheckSumRequestF = FALSE;
			if( SysDl.CheckSumCalcF == TRUE )
			{
				SysDl.CheckSumCancelF = TRUE;
			}
#ifdef	_MERCURY_PLATFORM_ 		/*<S190>*/
			else
			{
				SysDl.CheckSumCancelF = FALSE;
			}
#endif	/* _MERCURY_PLATFORM_ *//*<S190>*/
			SysDl.CheckSumEndF = FALSE;
/* <<<<< <S146> */
			KpiRstLongTimer( &(SysDl.BpsChkTime) );			/* Reset BpsChkTimer	<S041>	*/
		}
		else
		{
			SYSDL_DLMODE_OFF();	/* download mode off	*/
			mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
		}
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* �Ƃ肠������ɂ��Ă���	*/
		/* �x�[�X�u���b�N�Ə�ʂƂ̒ʐM�m�F */
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F04H �u���b�N�����w��									<S077>					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�w�肳�ꂽMPU��SerialFlash�Z�N�^�[�����R�}���h���s									*/
/*		�����F��������Z�N�^�[�̐擪�A�h���X														*/
/*		�߂�l�FOK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:�Z�N�^�[��������I��																	*/
/*			BUSY:Erase���s��(�p��)																	*/
/*			NG_ADDR:�ݒ�A�h���X�ُ�																*/
/*			NG_CMD:���R�}���h(write/read)���s��														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLEraseBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	UCHAR	FlashNum;		/* ������Flash�ԍ� */
	ULONG	EraseAddr;		/* ������Flash�擪�A�h���X */
	LONG	EraseSts;		/* �u���b�N�������ߎ��s�X�e�[�^�X */
	SHORT	rc;

	EraseSts = 0/*ERASE_OK*/;
	rc = MBUS_OK;
	
	CmdBuf = Hmsg->CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
	ResBuf = Hmsg->ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

	ResBuf[6] = CmdBuf[6];		/* Flash No		*/
	ResBuf[7] = CmdBuf[7];		/* reserved		*/
	ResBuf[8] = CmdBuf[8];	 	/* erase address		*/
	ResBuf[9] = CmdBuf[9];	 	/* erase address		*/
	ResBuf[10] = CmdBuf[10]; 	/* erase address		*/
	ResBuf[11] = CmdBuf[11]; 	/* erase address		*/

	if(SYSDL_CHK_DLMODE_ON())	/* �_�E�����[�h���[�h���Ȃ���s	*/
	{
		switch( MpuNo )
		{
		case 0x00:	 /* MainCPU(SoC JL-086) */
			SYSDL_ERASE_ON();		/* Start Erase Flash		*/
			FlashNum = ResBuf[6];	/* FlashNum *//* ����Ȃ��C������BFlash�ƃA�h���X�̐����������H */
			EraseAddr = ((ResBuf[8]<<24)|(ResBuf[9]<<16)|(ResBuf[10]<<8)|(ResBuf[11]));	/* 0xFFE00000����64KB(0x10000)����	*/
#ifndef _MERCURY_PLATFORM_			/*<S190>*/
			EraseSts = SysDLEraseBlock( EraseAddr, MpuNo, FlashNum );
#else			/*<S190>*/
			/* �u�[�g�Ń��b�Z�[�W�������s���Ƃ��́A�u�[�g�̈�͐G��Ȃ� */
			if( EraseAddr < FLS_BOOT_TOP_ADDRESS )
			{
				EraseSts = SysDLEraseBlock( EraseAddr, MpuNo, FlashNum );
			}
#endif	/* _MERCURY_PLATFORM_ *//*<S190>*/
			if( EraseSts == 0/*ERASE_OK*/ )
			{
				SYSDL_ERASE_OFF();	/* Finish Erase Flash		*/
/* <S146> >>>>> */
				/* Set sum check flag */
				if( FLS_MICRO_CODE_ADDRESS <= EraseAddr )
				{
					if( EraseAddr < FLS_MIRROR_EFW_ADDRESS )
					{
						SysDl.MicroSumCheckF = TRUE;
					}
					else if( EraseAddr < FLS_CODE_EFW_ADDRESS )
					{
						SysDl.MirSumCheckF = TRUE;
					}
					else if( EraseAddr < FLS_BOOT_TOP_ADDRESS )
					{
						SysDl.RtmSumCheckF = TRUE;
					}
					else if( EraseAddr <= FLS_BOOT_BTM_ADDRESS )
					{
						SysDl.BootSumCheckF = TRUE;
					}
				}
/* <<<<< <S146> */
			}
			else if( EraseSts == 1/*ERASE_BUSY*/ )
			{
				rc = MBUS_BUSY;
			}
			else	/* ERASE_NG */
			{
				rc = MBUS_NG;
			}
			break;
		case 0x01:		/* OptionCPU (NetX etc...) */
			/* �Ƃ肠������ɂ��Ă���	*/
			/* �x�[�X�u���b�N�Ə�ʂƂ̒ʐM�m�F */
			break;
		default:
			mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
			break;
		}
	}
	else
	{
		mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
	}

	if( (EraseSts != 1)/*ERASE_BUSY*/ && ((ResBuf[1] & 0x80) == 0))
	{
		*TrnbLength = MBUS_DL_PRGDEL_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	}
	
	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F05H �v���O���������ݎw��									<S077>				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�w�肳�ꂽMPU��SerialFlash�������݃R�}���h���s										*/
/*		�����F																						*/
/*			HMSGIF *Hmsg�F�������ݐ�擪�A�h���X													*/
/*			UCHAR MpuNo�FMPU�ԍ�(0-16)																*/
/*		�߂�l�FOK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:�������ݓ���I��																		*/
/*			BUSY:���s��(�p��)																		*/
/*			NG_ADDR:�ݒ�A�h���X�ُ�																*/
/*			NG_CMD:���R�}���h(write/read)���s��														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLWriteBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	UCHAR	FlashNum;		/* �����ݐ�Flash�ԍ� */
	ULONG	WriteAddr;		/* ������Flash�擪�A�h���X */
	LONG	WriteSts;		/* �����ݖ��ߎ��s�X�e�[�^�X */
	ULONG	WriteBuf[(SYSDL_1DLEN>>2)];		/* �����݃f�[�^�o�b�t�@ */
	USHORT	WriteDataNum;
	LONG	i,j;
	SHORT	rc;

	WriteSts = 0/*WRITE_OK*/;
	rc = MBUS_OK;
	
	/* ��M�f�[�^�擪�A�h���X		*/
	CmdBuf = Hmsg->CmdBuf;		/* ��M���b�Z�[�W�擪�A�h���X */
	ResBuf = Hmsg->ResBuf;		/* ���M���b�Z�[�W�擪�A�h���X */

	ResBuf[6] = CmdBuf[6];		/* Flash No		*/
	ResBuf[7] = CmdBuf[7];		/* reserved		*/
	ResBuf[8] = CmdBuf[8];	 	/* data num(Hi)		*/
	ResBuf[9] = CmdBuf[9];	 	/* data num(Lo)		*/
	ResBuf[10] = CmdBuf[10]; 	/* write address		*/
	ResBuf[11] = CmdBuf[11]; 	/* write address		*/
	ResBuf[12] = CmdBuf[12]; 	/* write address		*/
	ResBuf[13] = CmdBuf[13]; 	/* write address		*/

	WriteDataNum = ( (ResBuf[8]<<2) | ResBuf[9] );
	if( SYSDL_1DLEN < WriteDataNum )
	{
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
	}
	else
	{
		for(i=14,j=0;j<(SYSDL_1DLEN>>2);i+=4,j++)
		{
			WriteBuf[j] = (((CmdBuf[i+3]<<24))|((CmdBuf[i+2]<<16))|((CmdBuf[i+1]<<8))|(CmdBuf[i]));
		}

		if(SYSDL_CHK_DLMODE_ON())	/* �_�E�����[�h���[�h���Ȃ���s	*/
		{
			switch( MpuNo )
			{
			case 0x00:	 /* MainCPU(SoC JL-086) */
				SYSDL_ERASE_ON();		/* Start Erase Flash		*/
				FlashNum = ResBuf[6];	/* FlashNum *//* ����Ȃ��C������BFlash�ƃA�h���X�̐����������H */

				SYSDL_WRITE_ON();		/* Start Write Flash		*/
				FlashNum = ResBuf[6];	/* FlashNum *//* ����Ȃ��C������BFlash�ƃA�h���X�̐����������H */
				WriteAddr = ((ResBuf[10]<<24)|(ResBuf[11]<<16)|(ResBuf[12]<<8)|(ResBuf[13]));	/* 0xFFE00000����64KB(0x10000)����	*/
#ifndef _MERCURY_PLATFORM_			/*<S190>*/
				WriteSts = SysDLWritePage( WriteAddr, WriteBuf, MpuNo, FlashNum );
#else			/*<S190>*/
				/* �u�[�g�Ń��b�Z�[�W�������s���Ƃ��́A�u�[�g�̈�͐G��Ȃ� */
				if( WriteAddr < FLS_BOOT_TOP_ADDRESS )
				{
					WriteSts = SysDLWritePage( WriteAddr, WriteBuf, MpuNo, FlashNum );
				}
#endif	/* _MERCURY_PLATFORM_ *//*<S190>*/
				if( WriteSts == 0/*WRITE_OK*/ )
				{
					SYSDL_WRITE_OFF();	/* Finish Write Flash		*/
				}
				else if( WriteSts == 1/*WRITE_BUSY*/ )
				{
					rc = MBUS_BUSY;
				}
				else	/* WRITE_NG */
				{
					rc = MBUS_NG;
				}
				break;
			case 0x01:		/* OptionCPU (NetX etc...) */
				/* �Ƃ肠������ɂ��Ă���	*/
				/* �x�[�X�u���b�N�Ə�ʂƂ̒ʐM�m�F */
				break;
			default:
				mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
				break;
			}
		}
		else
		{
			mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
		}
	}

	if( (WriteSts != 1)/*WRITE_BUSY*/ && ((ResBuf[1] & 0x80) == 0) )
	{
		*TrnbLength = MBUS_DL_PRGWR_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	}
	
	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F06H �u���b�N�������w��(�������]���̂�.���������Ȃ�)							*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLRecvEraseInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_DELSND_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F07H �v���O�������M�w��(�v���O�������M�̂�.���������Ȃ�)																	*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLRecvWriteInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_PRGSND_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F08H �u���b�N�ꊇ�����w��															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLEraseAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_PRGDELA_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F09H �v���O�����ꊇ�����ݎw��														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLWriteAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_PRGWRA_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F0AH �_�E�����[�h�I���w��									<S077>				*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLFinish50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*CmdBuf;//<S0C7>
//	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;
//<S146>	SHORT	rslt = OK;

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_END_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		/* �x�[�X�u���b�N�ł���΃_�E�����[�h�J�n	*/
		if(SYSDL_CHK_DLMODE_ON())	/* �_�E�����[�h���[�h���Ȃ���s	*/
		{
/* <S146> */
//			//rslt = checkSum();		/* firmware check sum		*/
//			if( NG == rslt )
//			{
//				rc = MBUS_NG;
//				mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
//			}
//			else
//			{
//				SYSDL_DLMODE_RESET();
//			}
			SYSDL_DLMODE_RESET();
			SysDl.CheckSumRequestF = TRUE;	/* �`�F�b�N�T�������J�n�˗� */
		}
		else
		{
			mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
		}
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* �Ƃ肠������ɂ��Ă���	*/
		/* �x�[�X�u���b�N�Ə�ʂƂ̒ʐM�m�F */
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F0BH �v���O�����Ǐo���w��															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadProg50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_START_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F0CH �v���O�����ċN���w��															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReboot50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_REBOOT_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F0DH �V���A���h�e���x�ύX						<S041>								*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLChangeSifComSpd50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//UCHAR	*ResBuf;  /* <S072> */
SHORT	rc = MBUS_OK;  /* <S072> */
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);  /* <S072> */
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);  /* <S072> */

	/* ���M�f�[�^�擪�A�h���X */
//	ResBuf = Hmsg->CmdBuf;

	*TrnbLength = MBUS_DL_SIFCHG_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
//		ResBufW[0]	= CmdBufW[0];						/*+ <S072> AxisAdr, FunctionCode			*/
//		ResBufW[1]	= CmdBufW[1];						/*+ <S072> SubFunctionCode					*/
//		ResBufW[2]	= CmdBufW[2];						/*+ <S072> MPU�ԍ�							*/
		ResBufW[3]	= CmdBufW[3];						/*+ <S072> �]�����x��ʃ��[�h	*/
		ResBufW[4]	= CmdBufW[4];						/*+ <S072> �]�����x���ʃ��[�h	*/
		/* �Ƃ肠������ɂ��Ă���	*/
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* �Ƃ肠������ɂ��Ă���	*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F0EH �_�E�����[�h����Ǐo��															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_HIS_RD_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	return (MBUS_OK);									/* <S072> �߂�l���Ȃ����� */
}


/****************************************************************************************************/
/*																									*/
/*		Memobus�F0FH �_�E�����[�h����������															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLWriteHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* ���M�f�[�^�擪�A�h���X		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_HIS_WR_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	return (MBUS_OK);									/* <S072> �߂�l���Ȃ����� */
}


/* <S146> >>>>> */
/****************************************************************************************************/
/*																									*/
/*		Memobus�F10H �`�F�b�N�T�����ʎ擾															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLGetCheckSumResult50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;
	UCHAR	Status;
	SHORT	rc = MBUS_OK;

	Status = 0x40;	/* ���̑� */

	/* ���M�f�[�^�擪�A�h���X		*/
	ResBuf = Hmsg->ResBuf;
	
	*TrnbLength = MBUS_DL_CHKSUM_RSPSIZE;		/* ���M�o�b�t�@�T�C�Y���� */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		/* ���݂̏�Ԃ��Z�b�g���ĕԂ�	*/
		if( SysDl.CheckSumCalcF == TRUE )
		{
			Status = 0x80;
		}
		else if( SysDl.CheckSumEndF == TRUE )
		{
			if( SysDl.CheckSumResult == TRUE )
			{
				Status = 0x00;	/* �`�F�b�N�T��OK */
			}
			else
			{
				Status = 0x01;	/* �`�F�b�N�T��NG */
			}
		}
		ResBuf[6] = Status;
		ResBuf[7] = 0x00;
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* �Ƃ肠������ɂ��Ă���	*/
		/* �x�[�X�u���b�N�Ə�ʂƂ̒ʐM�m�F */
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}
	
	return( rc );
}
/* <<<<< <S146> */


/****************************************************************************************************/
/*																									*/
/*		Memobus�F�ُ퉞���ԐM����																	*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncErrorManager50H( HMSGIF Hmsg, LONG ErrorCode, USHORT *TrnbLength )
{
	UCHAR  *CmdBuf;
	UCHAR  *ResBuf;

	/* ��M�A���M�f�[�^�擪�A�h���X */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	/* �ُ퉞���f�[�^�쐬 */
	ResBuf[0]  =  CmdBuf[0];						/* 0:Slave Addrress		*/
	ResBuf[1]  = (CmdBuf[1] | 0x80);				/* 1:Function Code		*/
	ResBuf[2]  =  (UCHAR)ErrorCode;					/* 4:Error Code			*/

	/* �G���[�ԐM�T�C�Y��5byte	*/
	*TrnbLength = MBUS_DL_ERROR_RSPSIZE;

	return (MBUS_OK);								/* <S072> �߂�l���Ȃ����� */
}

#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
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
	/* ���m�F */
	Buf[3] = (UCHAR)((data) >> 24);
	Buf[2] = (UCHAR)((data) >> 16);
	Buf[1] = (UCHAR)((data) >>  8);
	Buf[0] = (UCHAR)( data );
#endif
}
#endif	/*_MERCURY_PLATFORM_*//*<S190>*/


/****************************************************************************************************/
/*																									*/
/*		Flash�u���b�N��������																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�w�肳�ꂽMPU��SerialFlash�Z�N�^�[�����R�}���h���s									*/
/*		�����F																						*/
/*			ULONG EraseAddr	:��������Z�N�^�[�̐擪�A�h���X											*/
/*			UCHAR MpuNo		:��������MPU�ԍ�														*/
/*			UCHAR FlashNum	:��������MPU��Flash�ԍ��i�g���p�j										*/
/*		�߂�l�FOK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:�Z�N�^�[��������I��																	*/
/*			BUSY:Erase���s��(�p��)																	*/
/*			NG_ADDR:�ݒ�A�h���X�ُ�																*/
/*			NG_CMD:���R�}���h(write/read)���s��														*/
/*																									*/
/****************************************************************************************************/
static SHORT	SysDLEraseBlock( ULONG EraseAddr, UCHAR MpuNo, UCHAR FlashNum )
{
	LONG	EraseSts;
	
	EraseSts = 0/*ERASE_OK*/;
	
	switch( MpuNo )
	{
		case 0x00:	 /* MainCPU(SoC JL-086) */
			EraseSts = SFlashEraseSectorSoC( EraseAddr );
			break;
		case 0x01:		/* OptionCPU (NetX etc...) */
//			EraseSts = FlashEraseSectorOption( EraseAddr );
			break;
		default:
			EraseSts = -1/*ERASE_MPUNO_ERROR*/;
			break;
	}

	return( EraseSts );
}


/****************************************************************************************************/
/*																									*/
/*		Flash�y�[�W�������ݏ���																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�w�肳�ꂽMPU��SerialFlash�y�[�W���C�g�R�}���h���s									*/
/*		�����F																						*/
/*			ULONG WriteAddr	:�������ރy�[�W�̐擪�A�h���X											*/
/*			ULONG* WriteBuf	:�������݃f�[�^�o�b�t�@													*/
/*			UCHAR MpuNo		:��������MPU�ԍ�														*/
/*			UCHAR FlashNum	:��������MPU��Flash�ԍ��i�g���p�j										*/
/*		�߂�l�FOK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK		:����I��																		*/
/*			BUSY	:���s��(�p��)																	*/
/*			NG_ADDR	:�ݒ�A�h���X�ُ�																*/
/*			NG_CMD	:���R�}���h(write/read)���s��													*/
/*																									*/
/****************************************************************************************************/
static SHORT	SysDLWritePage( ULONG WriteAddr, ULONG* WriteBuf, UCHAR MpuNo, UCHAR FlashNum )
{
	LONG	WriteSts;
	
	WriteSts = 0/*WRITE_OK*/;
	
	switch( MpuNo )
	{
		case 0x00:	 /* MainCPU(SoC JL-086) */
			WriteSts = SFlashWritePageSoC( WriteAddr, WriteBuf );
			break;
		case 0x01:		/* OptionCPU (NetX etc...) */
//			WriteSts = FlashWriteSectorOption( WriteAddr );
			break;
		default:
			WriteSts = -1/*WRITE_MPUNO_ERROR*/;
			break;
	}

	return( WriteSts );
}


/****************************************************************************************************/
/*																									*/
/*		FirmDownload : �w�߃��b�Z�[�W���擾 																*/
/*																									*/
/****************************************************************************************************/
LONG	MbusFuncSysDLGetMsgLength( UCHAR *CmdBuf )
{
	UCHAR	SubFuncCode;
	SHORT	dlsize;
	LONG	lret = -1;

	SubFuncCode = CmdBuf[3];	/* �T�u�t�@���N�V�����R�[�h   */


	switch( SubFuncCode )/* �T�u�t�@���N�V�����R�[�h */
	{
		case MBUS_DL_ID_RD:			/* 00H ID���ǂݏo�� */
			lret = 8;
			break;
		case MBUS_DL_SWVER_RD:			/* 01H �o�[�W�����Ǐo�� */
			lret = 8;
			break;
		case MBUS_DL_WAY_RD:		/* 02H �v���O���������ݏ����擾	*/
			lret = 8;
			break;
		case MBUS_DL_START:		/* 03H �_�E�����[�h�J�n�w��		*/
			lret = 8;
			break;
		case MBUS_DL_PRGDEL:		/* 04H �u���b�N�����w��	*/
			lret = 14;
			break;
		case MBUS_DL_PRGWR:	/* 05H �v���O���������ݎw��		*/
			dlsize = ((CmdBuf[8]<<8)|(CmdBuf[9]));
			if( (dlsize<0) || (dlsize>32768) )	/* �f�[�^���m�F	*/
			{
				break;
			}
			lret = dlsize + 16;
			break;
		case MBUS_DL_DELSND:	/* 06H �u���b�N�������w��(�������]���̂�.���������Ȃ�)		*/
			lret = 14;
			break;
		case MBUS_DL_PRGSND:	/* 07H �v���O�������M�w��(�v���O�������M�̂�.���������Ȃ�)		*/
			dlsize = ((CmdBuf[8]<<8)|(CmdBuf[9]));
			if( (dlsize<0) || (dlsize>32768) )	/* �f�[�^���m�F	*/
			{
				break;
			}
			lret = dlsize + 16;
			break;
		case MBUS_DL_PRGDELA:	/* 08H �v���O�����ꊇ�����w��		*/
			lret = 8;
			break;
		case MBUS_DL_PRGWRA:	/* 09H �v���O�����ꊇ�����ݎw��		*/
			lret = 8;
			break;
		case MBUS_DL_END:	/* 0AH �_�E�����[�h�I���w��		*/
			lret = 8;
			break;
		case MBUS_DL_PRGRD:	/* 0BH �v���O�����Ǐo���w��		*/
			lret = 14;
			break;
		case MBUS_DL_REBOOT:	/* 0CH �v���O�����ċN���w��		*/
			lret = 8;
			break;
		case MBUS_DL_SIFCHG:	/* 0DH �V���A���h�e���x�ύX		*/
//<S041>		lret = 14;
			lret = 12;
			break;
		case MBUS_DL_HIS_RD:	/* 0EH �_�E�����[�h����Ǐo��		*/
			lret = 8;
			break;
		case MBUS_DL_HIS_WR:	/* 0FH �_�E�����[�h����������		*/
			lret = 10;
			break;
		case MBUS_DL_CHKSUM:	/* 10H �`�F�b�N�T�����ʎ擾		*/ /* <S146> */
			lret = 8;
			break;
		default:					/* �R�}���h�s��v				*/
			break;
	}

	return( lret );

}


/* �Í����t�H�[�}�b�g�Ή� <S146>  >>>>> */
/****************************************************************************************************/
/*																									*/
/*		MEMOBUS FirmDownload �`�F�b�N�T������														*/
/*																									*/
/****************************************************************************************************/
/* <S182> �t�F�[�Y�Ǘ��p�ɏ��������Ă��邽�ߏ����������͂Ȃ� */
/* <S1A3> �T�u�t�F�[�Y�ǉ��Ή�(�����������͂Ȃ�) */
void	MbusFuncSystemDownLoadCheckSum( void )
{
	SHORT	rc;			/* �߂�l		*/

	/* �ϐ������� */
	rc = CHKSUM_OK;

	/* �`�F�b�N�T�������v���m�F */
	if( SysDl.CheckSumRequestF == TRUE )
	{
		/* �`�F�b�N�T�������v��OFF */
		SysDl.CheckSumRequestF = FALSE;

		/* �������ݒ� */
		SysDl.CheckSumCalcF = TRUE;

		/* �����t�F�[�Y������ */
		SysDl.CheckSumPhase = 0;
	}

	/* �������Ȃ�`�F�b�N�T���v�Z���{ */
	if( SysDl.CheckSumCalcF == TRUE )
	{
		/* �L�����Z���v���m�F */
		if( SysDl.CheckSumCancelF == FALSE )
		{
			/* �`�F�b�N�T���v�Z */
			rc = checkSum();

			/* ���ʐݒ� */
			/* �ŏI�t�F�[�Y�܂Ŋ��� */
			if( (rc == CHKSUM_OK) && (SysDl.CheckSumPhase == CHKSUM_PHASE_END) )
			{
				/* ���� : OK */
				SysDl.CheckSumResult = TRUE;
				/* �I����ݒ� */
				SysDl.CheckSumEndF = TRUE;
			}
			/* NG�̏ꍇ */
			else if( rc == CHKSUM_NG )
			{
				/* ���� : NG */
				SysDl.CheckSumResult = FALSE;
				/* �I����ݒ� */
				SysDl.CheckSumEndF = TRUE;
			}
		}

		/* �L�����Z���v�����肩�A�`�F�b�N�I���Ȃ珈������OFF */
		if( (SysDl.CheckSumCancelF == TRUE) || (SysDl.CheckSumEndF == TRUE)  )
		{
			/* ������OFF */
			SysDl.CheckSumCalcF = FALSE;
		}
	}

	return;
}


/****************************************************************************************************/
/*																									*/
/*		ELFZIP�t�@�C���̃T���l�`�F�b�N		 														*/
/*																									*/
/****************************************************************************************************/
/* <S1A3> �����X�L�����Ń`�F�b�N����悤�ɕύX(�����������͂Ȃ�) */
static SHORT	checkElfZipSum( UCHAR *pTopAddress )
{
	SHORT			rc;					/* �߂�l							*/
	FIRM_HEADER_T	*pHeader;			/* �t�@�[���w�b�_�ւ̃|�C���^		*/
	UINT32			FirmFileSize;		/* �t�@�C���T�C�Y					*/
	UINT32			*pZipLocalHeader;	/* ZIP���[�J���w�b�_�ւ̃|�C���^	*/
	LONG			i;					/* ���[�v�J�E���^					*/

	/* �ϐ������� */
	rc = CHKSUM_BUSY;

	pHeader = (FIRM_HEADER_T *)pTopAddress;
	FirmFileSize = pHeader->zip_file_size + sizeof(FIRM_HEADER_T);

	switch( SysDl.CheckSumPhase & CHKSUM_SUB_PHASE_MASK )
	{
	case 0:
		/*------------------*/
		/* �t�@�C���`�F�b�N */
		/*------------------*/
		/* ���炩�Ɉُ�ȃt�@�C���T�C�Y���`�F�b�N */
		if( (pHeader->zip_file_size < sizeof(UINT32))
			|| (pHeader->zip_file_size > FLS_CODE_AREA_SIZE)
			|| (FirmFileSize > FLS_CODE_AREA_SIZE) )
		{
			rc = CHKSUM_NG;
		}

		/* ZIP�V�O�l�`���`�F�b�N */
		if( rc != CHKSUM_NG )
		{
			pZipLocalHeader = (UINT32 *)(pTopAddress + sizeof(FIRM_HEADER_T));

			if( *pZipLocalHeader != ZIP_LOCAL_HEADER_SIGNATURE )
			{
				rc = CHKSUM_NG;
			}
		}

		/* ������ */
		if( rc != CHKSUM_NG )
		{
			SysDl.CheckSumVal = 0;
			SysDl.CheckSumMem = pTopAddress;
			SysDl.CheckSumIdx = 0;
			SysDl.CheckSumPhase++;
		}
		break;

	case 1:
		/* �S������Z */
		while( SysDl.CheckSumIdx < FirmFileSize )
		{
			SysDl.CheckSumVal += (UINT16)*SysDl.CheckSumMem;	/* 1�o�C�g�����Z		*/
			SysDl.CheckSumMem++;
			SysDl.CheckSumIdx++;
			/* 1�u���b�N�������������U�I�� */
			if( !(SysDl.CheckSumIdx & CHKSUM_BLOCK_MASK ) )
			{
				break;
			}
		}
		/* �Ō�܂œ��B������T���l�`�F�b�N */
		if( SysDl.CheckSumIdx >= FirmFileSize )
		{
			/*--------------*/
			/* �T���`�F�b�N */
			/*--------------*/
			/* �t�@�[���w�b�_���̑S�̃`�F�b�N�T���l���������Z */
			SysDl.CheckSumMem = (UCHAR *)&(pHeader->all_check_sum);
			for( i = 0; i < sizeof(pHeader->all_check_sum); i++ )
			{
				SysDl.CheckSumVal -= (UINT16)*SysDl.CheckSumMem;	/* 1�o�C�g�����Z		*/
				SysDl.CheckSumMem++;
			}
			/* �`�F�b�N�T���l���Z	*/
			SysDl.CheckSumVal += pHeader->all_check_sum;

			/* ���ʔ��� */
			if( SysDl.CheckSumVal == 0 )
			{
				rc = CHKSUM_OK;
			}
			else
			{
				rc = CHKSUM_NG;
			}
		}
		break;

	default:
		rc = CHKSUM_NG;
		break;
	}

	return rc;
}


/****************************************************************************************************/
/*																									*/
/*		�w��̈�̃T���l�`�F�b�N		 															*/
/*																									*/
/****************************************************************************************************/
/* <S1A3> �����X�L�����Ń`�F�b�N����悤�ɕύX(�����������͂Ȃ�) */
static SHORT	checkAreaSum( UCHAR *pTopAddress, LONG Length )
{
	SHORT	rc;					/* �߂�l			*/

	/* �ϐ������� */
	rc = CHKSUM_BUSY;

	switch( SysDl.CheckSumPhase & CHKSUM_SUB_PHASE_MASK )
	{
	case 0:	/* ������ */
		SysDl.CheckSumVal = 0;
		SysDl.CheckSumMem = pTopAddress;
		SysDl.CheckSumIdx = 0;
		SysDl.CheckSumPhase++;
		break;

	case 1:	/* �S����Z���`�F�b�N�T���m�F */
		while( SysDl.CheckSumIdx < Length )
		{
			SysDl.CheckSumVal += (USHORT)*SysDl.CheckSumMem;	/* 1�o�C�g�����Z		*/
			SysDl.CheckSumMem++;
			SysDl.CheckSumIdx++;
			/* 1�u���b�N�������������U�I�� */
			if( !(SysDl.CheckSumIdx & CHKSUM_BLOCK_MASK) )
			{
				break;
			}
		}
		/* �Ō�܂œ��B������T���l�`�F�b�N */
		if( SysDl.CheckSumIdx >= Length )
		{
			/* �`�F�b�N�T���l���Z	*/
			SysDl.CheckSumVal += *((USHORT *)SysDl.CheckSumMem);

			/* ���ʔ��� */
			if( SysDl.CheckSumVal == 0 )
			{
				rc = CHKSUM_OK;
			}
			else
			{
				rc = CHKSUM_NG;
			}
		}
		break;
	default:
		rc = CHKSUM_NG;
		break;
	}

	return rc;
}


/****************************************************************************************************/
/*																									*/
/*		��M�o�C�i���̃T���l�`�F�b�N		 														*/
/*																									*/
/****************************************************************************************************/
/* <S182> �t�F�[�Y�Ǘ��p�ɏ��������Ă��邽�ߏ����������͂Ȃ� */
/* <S1A3> �����X�L�����Ń`�F�b�N����悤�ɕύX(�����������͂Ȃ�) */
static SHORT	checkSum( void )
{
	SHORT	rc;					/* �߂�l			*/
	UINT32	*pZipLocalHeader;	/* ZIP���[�J���w�b�_�ւ̃|�C���^	*/

	/* �ϐ������� */
	rc = CHKSUM_OK;

	/* ����̊e���C���t�F�[�Y�̏��������s */
	switch( SysDl.CheckSumPhase >> CHKSUM_MAIN_PHASE_SHFT )
	{
	case 0:
	/*------------------*/
	/* �~���[���`�F�b�N */
	/*------------------*/
		if( SysDl.MirSumCheckF == TRUE )
		{
			/* �H��o�חp�ȈՔ��� */
			pZipLocalHeader = (UINT32 *)(FLS_MIRROR_EFW_ADDRESS + sizeof(FIRM_HEADER_T));
			if( (*pZipLocalHeader != ZIP_LOCAL_HEADER_SIGNATURE)
				&& ( *((UCHAR *)FLS_FACTORY_PASSWD_ADDRESS) != 0xFF) )
			{
				/* ZIP�V�O�l�`�������݂����A�p�X���[�h���ݒ肳��Ă���΁A�H��o�׎����`�F�b�N�s�v */
			}
			else
			{
				/* �~���[���̃T���`�F�b�N */
				rc = checkElfZipSum( (UCHAR *)FLS_MIRROR_EFW_ADDRESS );
			}
		}
		break;

	case 1:
	/*----------------------*/
	/* �����^�C�����`�F�b�N */
	/*----------------------*/
		if( SysDl.RtmSumCheckF == TRUE )
		{
			/* �����^�C�����̃T���`�F�b�N */
			rc = checkElfZipSum( (UCHAR *)FLS_CODE_EFW_ADDRESS );
		}
		break;

	case 2:
	/*------------------*/
	/* �u�[�g���`�F�b�N */
	/*------------------*/
		if( SysDl.BootSumCheckF == TRUE )
		{
			rc = checkAreaSum( (UCHAR *)FLS_BOOT_TOP_ADDRESS, FLS_BOOT_SUM_OFFSET );
		}
		break;

	case 3:
	/*--------------------------*/
	/* �}�C�N���R�[�h���`�F�b�N */
	/*--------------------------*/
		if( SysDl.MicroSumCheckF == TRUE )
		{
			rc = checkAreaSum( (UCHAR *)FLS_MICRO_CODE_ADDRESS, FLS_MICRO_SUM_OFFSET );
		}
		break;

	default:
	/*--------------*/
	/* �ŏI�t�F�[�Y */
	/*--------------*/
		SysDl.CheckSumPhase = CHKSUM_PHASE_END;
		break;
	}

	if( (rc == CHKSUM_OK) && (SysDl.CheckSumPhase != CHKSUM_PHASE_END) )
	{
		/* �t�F�[�Y�X�V */
		SysDl.CheckSumPhase = (SysDl.CheckSumPhase + CHKSUM_MAIN_PHASE_ADD) & CHKSUM_MAIN_PHASE_MASK;
	}

	return rc;
}
/* <<<<< �Í����t�H�[�}�b�g�Ή� <S146> */

/* <S1A2> >>>>> */
/****************************************************************************************************/
/*																									*/
/*		���[�N�̃N���A						 														*/
/*																									*/
/****************************************************************************************************/
void MbusFuncSysDlInitWork( void )
{
	MlibResetByteMemory( &SysDl, sizeof(SysDl) );
	return;
}
/* <<<<< <S1A2> */
/***************************************** end of file **********************************************/
