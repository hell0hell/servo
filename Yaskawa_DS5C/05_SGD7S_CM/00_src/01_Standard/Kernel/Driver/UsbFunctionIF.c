/****************************************************************************
Description	: USB function Server for MEMBUS protocol
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	-- APIs --
	MbusUsb_Initialize()	: Initialize the USB function Server
	MbusUsb_Server()		: Runtime process of the MEMOBUS USB Server
	MbusUsb_PollOutIn()		: �|�[�����O�ɂ��USB����M�Ď����䏈��

	-- Locals --
	MbusUsb_ChkMyMsg()		: ���ǃA�h���X�̃`�F�b�N

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.05.10		created

****************************************************************************/
#include "Basedef.h"
#include "arm.h"
#include "UsbFunctionIF.h"
#include "KLib.h"
#include "MLib.h"
#include "usb_f.h"

//debug -->
#if 1
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
#endif
//debug <--


/****************************************************************************/
/*		PROTOTYPE															*/
/****************************************************************************/
/* Locals */
static ULONG	MbusUsb_ChkMyMsg(MEMOBUS_IF *MbusIf);		/* ���ǃA�h���X�`�F�b�N		*/

/***************************************************************************
Description: Initialize the USB Memobus Server
----------------------------------------------------------------------------
Parameters:
	USB_MBUS_SERVER		*pUsbMbusServer	:Pointer to this module handle
	MEMOBUS_ADDR_MAP	*pAxisCfg		:Axis Address configuration
	ULONG				MyAddress		:my address
Return:
	0					: Success.
****************************************************************************/
INT32 MbusUsb_Initialize(USB_MBUS_SERVER *pUsbMbusServer,
						 MEMOBUS_ADDR_MAP *pAxisCfg, ULONG MyAddress)
{
/* FIXME:MyAddress�̈��������s���m */
#if 1	/* debug */
	MyAddress = 1;
#endif
	/* ���[�N�̈揉���� */
	MlibResetByteMemory(pUsbMbusServer, sizeof(USB_MBUS_SERVER));

	/* MEMOBUS I/F ������ */
	MbusFuncInitialize(&(pUsbMbusServer->MbusIf), pAxisCfg, MyAddress);

	/* USB�t�@���N�V�����f�o�C�X������ */
	UsbFuncInitializeDriver(MyAddress);

	/* ����M�Ď��V�[�P���X��Ԑݒ� */
	pUsbMbusServer->State = USB_STATE_INITIAL1;

	return 0;
}


/***************************************************************************
Description: Runtime process of the USB Memobus Server
----------------------------------------------------------------------------
Parameters:
	USB_MBUS_SERVER		*pUsbMbusServer	:Pointer to this module handle
Return: none
****************************************************************************/
void MbusUsb_Server(USB_MBUS_SERVER *pUsbMbusServer)
{
	/* ���M�J�n�҂� */
	if(pUsbMbusServer->State == USB_STATE_WAIT_START_IN1)
	{
/* FIXME:MbusFuncMsgProcedure()���p���̎d�l�s���m������ */
#if 0	/* �b��Ή�(�����̏����𐳂����쐬����K�v������) */
		/* ���b�Z�[�W��́����s���� */
		MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
#else
		switch( pUsbMbusServer->MbusIf.RcvbLength )
		{
		case LEN_MSGREADSVSERIAL:		// ���W�X�^���e�ǂݏo��
			if( (pUsbMbusServer->MbusIf.RxBuf[1] == 0x40) && (pUsbMbusServer->MbusIf.RxBuf[3] == 0x03) )
			{
				// �T�[�{�p�b�N�V���A��No.��Ԃ����A�ԐM�f�[�^�ɖ�肪����悤��MBCommander�ŃG���[�ƂȂ��Ă���l�q
				memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadSvSerial, LEN_RESREADSVSERIAL);
				pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADSVSERIAL;
			}
			else
			{
				/* ���b�Z�[�W��́����s���� */
				MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
			}
			break;
		case LEN_MSGREADMAXREGSIZEA:	// 8�o�C�g
			// �ő�T�C�Y�ǂݏo��(16�r�b�g�A�h���X�p)
			if( (pUsbMbusServer->MbusIf.RxBuf[1] == 0x40) && (pUsbMbusServer->MbusIf.RxBuf[3] == 0x41) )
			{
				if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x03 )		// ���W�X�^���e�ǂݏo��
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeA, LEN_RESREADMAXREGSIZEA);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZEA;
				}
				else if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x0d )	// ���W�X�^�̔�A���ǂݏo��
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeB, LEN_RESREADMAXREGSIZEB);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZEB;
				}
				else if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x10 )	// �������W�X�^�ւ̏�������
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeC, LEN_RESREADMAXREGSIZEC);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZEC;
				}
				else if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x0e )	// ���W�X�^�̔�A���ǂݏo��
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeD, LEN_RESREADMAXREGSIZED);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZED;
				}
			}
//tanaka			else if( pUsbMbusServer->MbusIf.RxBuf[1] == 0x08 )		// ���[�v�o�b�N
//tanaka			{
//tanaka				memcpy(pUsbMbusServer->MbusIf.TxBuf, pUsbMbusServer->MbusIf.RxBuf, LEN_MSGREADMAXREGSIZEA);
//tanaka				pUsbMbusServer->MbusIf.TrnbLength = LEN_MSGREADMAXREGSIZEA;
//tanaka			}
			else
			{
				/* ���b�Z�[�W��́����s���� */
				MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
			}
			break;
		default:
			/* ���b�Z�[�W��́����s���� */
			MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
			break;
		}
#endif
//		MlibSetCRC16(pUsbMbusServer->MbusIf.TxBuf, pUsbMbusServer->MbusIf.TrnbLength);
//		pUsbMbusServer->MbusIf.TrnbLength += 2;

		/* �o���N�]���t���O�̐��䂪�K�v�I�I(BlkInSkip, BlkInStart) */
		/* �����_�ł�IN�J�n��ݒ� */
		pUsbMbusServer->f.BlkInStart = TRUE;

		/* Round����̑��M�J�n��ԂɈڍs */
		pUsbMbusServer->State = USB_STATE_WAIT_START_IN2;
	}
}



/***************************************************************************
Description: Runtime process of the USB Memobus Server
----------------------------------------------------------------------------
Parameters:
	USB_MBUS_SERVER		*pUsbMbusServer	:Pointer to this module handle
Return: none
****************************************************************************/
void MbusUsb_PollOutIn(USB_MBUS_SERVER *pUsbMbusServer)
{
	ULONG	rc = 0;

	UsbFuncPollBulkOutIn();	// ���u��

	/*----------------------------------------------------------------------------------------------*/
	/*		�t�r�a�ɂ��MEMOBUS�ʐM�@����M�Ď��V�[�P���X											*/
	/*----------------------------------------------------------------------------------------------*/
	switch( pUsbMbusServer->State ){
		/*------------------------------------------------------------------------------------------*/
		/*	�����X�e�b�v																			*/
		/*		�EUSB�h���C�o�̑���MFIFO��S�N���A����												*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_INITIAL1:
			UsbFuncResetBuffer( );
//*			if( ReadFuncLmtflg() == FALSE )	/* �@�\�����t���O�������Ă��Ȃ���Ώ�Ԃ�i�߂�			*/
			{								/* �t���O�������Ă���Ƃ���FIFO�̃��Z�b�g�̂�			*/
				pUsbMbusServer->State = USB_STATE_INITIAL2;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	�����X�e�b�v�Q																			*/
		/*		�EUSB�h���C�o�̏�Ԃ��`�F�b�N���ʐM�\��Ԃɓ��������Ƃ��m�F���āAEndpoint2��OUT	*/
		/*		  �f�[�^���󂯂�悤�ɗv������B�v����AOUT�p�P�b�g��M���������Ŋ�����҂B		*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_INITIAL2:
			/*--------------------------------------------------------------------------------------*/
			/*	USB�h���C�o�ɑ΂���USB�|�[�g���ʐM�\���⍇�����s���A�ʐM�\�Ȃ�MEMOBUS�ʐM��		*/
			/*	�������s���B																		*/
			/*--------------------------------------------------------------------------------------*/
			rc = UsbFuncPortConfiguration();
			if( rc == TRUE )
			{
				/*----------------------------------------------------------------------------------*/
				/*	USB�h���C�o�ɑ΂��ăf�[�^����M����悤�ɗv��									*/
				/*		�h���C�o������Ɏ�M�J�n���������s(rc==TRUE)������OUT��M�����҂��֑J��		*/
				/*----------------------------------------------------------------------------------*/
				rc = UsbFuncStartBkout( pUsbMbusServer->MbusIf.RxBuf, ((USHORT)MSG_MAXBUFSZB -1 ));
				if( rc == TRUE )
				{
					pUsbMbusServer->State = USB_STATE_WAIT_OUT_END;		/* OUT��M�����҂������ɑJ��	*/
				}
			}
			pUsbMbusServer->f.BlkInSkip = FALSE;						/* �������w�߃t���O�N���A		*/
			pUsbMbusServer->f.BlkInStart = FALSE;						/* �����w�߃t���O���N���A		*/
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	MEMOBUS�f�[�^��M�����҂�����															*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_WAIT_OUT_END:
			rc = UsbFuncPortConfiguration();							/* USB�L��?				*/
			if( rc == FALSE )
			{
				pUsbMbusServer->State = USB_STATE_INITIAL1;				/* �����X�e�b�v�֑J��	*/
			}
			else
			{
				rc = UsbFuncChkRcvComp();					/* �S�f�[�^��M�����⍇��			*/
				if( rc == BLKTRNS_END )						/* �S�f�[�^��M�����H				*/
				{
					/*------------------------------------------------------------------------------*/
					/*	��M�������̏���															*/
					/*		���b�Z�[�W�̃`�F�b�N													*/
					/*			�E���ǈ��`�F�b�N( ���ڽ"0"�̏ꍇ��۰�޷��Ăł��邽�ߎ��ǂƔ���)		*/
					/*		��CRC�`�F�b�N�̓A�v���P�[�V�����ōs���B									*/
					/*------------------------------------------------------------------------------*/
					pUsbMbusServer->MbusIf.RcvbLength = UsbFuncGetRcvSize();/* ��M�����o�C�g���𓾂�			*/
					rc = MbusUsb_ChkMyMsg(&(pUsbMbusServer->MbusIf));
					if( rc == TRUE )
					{
						pUsbMbusServer->State = USB_STATE_WAIT_START_IN1;	/* Adr=0 or ���ǈ�:���M�J�n�҂���	*/
					}
					else
					{
						pUsbMbusServer->State = USB_STATE_INITIAL1;			/* ���ǈ��ȊO�͎�M�J�n�֖߂�		*/
					}
				}
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	���M�J�n�҂�����																		*/
		/*		�A�v���P�[�V��������̑��M�J�n�v����҂�											*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_WAIT_START_IN1:
			break;
		case USB_STATE_WAIT_START_IN2:
			rc = UsbFuncPortConfiguration();							/* USB�L��?				*/
			if( rc == FALSE )
			{
				pUsbMbusServer->State = USB_STATE_INITIAL1;				/* �����X�e�b�v�֑J��	*/
			}
			/*----------------------------------------------------------------------------------*/
			/*	�������̏ꍇ�͑��M�X�e�b�v���s�킸��M�҂��֖߂�								*/
			/*----------------------------------------------------------------------------------*/
			else
			{
				if( pUsbMbusServer->f.BlkInSkip == TRUE )
				{
					pUsbMbusServer->State = USB_STATE_INITIAL1;			/* �����X�e�b�v�֑J��	*/
				}
				else if( pUsbMbusServer->f.BlkInStart == TRUE )
				{
					/*--------------------------------------------------------------------------*/
					/*	USB�h���C�o�֑��M�o�b�t�@�𑗐M�f�[�^���������M����悤�ɗv��			*/
					/*--------------------------------------------------------------------------*/
					rc = UsbFuncStartBkin( pUsbMbusServer->MbusIf.TxBuf, pUsbMbusServer->MbusIf.TrnbLength );
					if( rc == TRUE )
					{
						pUsbMbusServer->State = USB_STATE_WAIT_IN_END;	/* �S�f�[�^���M�����҂��֑J��	*/
					}
					else
					{
						pUsbMbusServer->State = USB_STATE_INITIAL1;		/* �����X�e�b�v�֑J��			*/
					}
				}
				else
				{
					/* �����Ȃ� */
				}
			}
			break;

		/*------------------------------------------------------------------------------------------*/
		/*	�S�f�[�^���M�����҂�����																*/
		/*		USB�h���C�o���M������҂�															*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_WAIT_IN_END:
			rc = UsbFuncPortConfiguration();							/* USB�L��?					*/
			if( rc == FALSE )
			{
				pUsbMbusServer->State = USB_STATE_INITIAL1;				/* �����X�e�b�v�֑J��		*/
			}
			else
			{
				rc = UsbFuncChkTrnsComp();								/* �S�f�[�^���M�����⍇��	*/
				/*------------------------------------------------------------------------------*/
				/*	���M�����`�F�b�N															*/
				/*------------------------------------------------------------------------------*/
				if( rc == BLKTRNS_END )
				{
					pUsbMbusServer->State = USB_STATE_INITIAL1;			/* �����X�e�b�v�֑J��		*/
				}
			}
			break;

		/*------------------------------------------------------------------------------------------*/
		/*	���̑��i�����ɂ͗��Ȃ��j���ی�															*/
		/*------------------------------------------------------------------------------------------*/
		default :
			break;

	}

/*--------------------------------------------------------------------------------------------------*/
/*	����M�V�[�P���X�����																			*/
/*--------------------------------------------------------------------------------------------------*/

	return;

}

/**************************** Start of Local Functions **********************************************/
/****************************************************************************************************/
/*																									*/
/*		���ǈ����b�Z�[�W�̃`�F�b�N																	*/
/*																									*/
/****************************************************************************************************/
/***************************************************************************
Description: ���ǃA�h���X�`�F�b�N
----------------------------------------------------------------------------
Parameters:
	MEMOBUS_IF			*MbusIfPtr	:Pointer to this module handle
Return: none
****************************************************************************/
static ULONG	MbusUsb_ChkMyMsg(MEMOBUS_IF *MbusIfPtr)
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�����M�������b�Z�[�W�̃A�h���X��=="0" or ==���ǃA�h���X�̏ꍇTRUE��Ԃ�				*/
	/*----------------------------------------------------------------------------------------------*/
	if(( MbusIfPtr->RxBuf[0] == 0x00 )||( MbusIfPtr->RxBuf[0] == MbusIfPtr->NodeAddress ))
	{
		return( TRUE );					/* ���ǈ��ł��邱�Ƃ�Ԃ�								*/
	}
	else
	{
		return( FALSE );				/* ���ǈ��ł͂Ȃ����Ƃ�Ԃ�								*/
	}

}
/**************************** End of Local Functions ************************************************/


/* End of File */
