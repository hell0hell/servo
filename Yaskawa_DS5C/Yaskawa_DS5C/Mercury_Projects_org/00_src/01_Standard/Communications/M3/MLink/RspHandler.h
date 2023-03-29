/****************************************************************************
Description	: Definition of MECHATROLINK Response packet
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef RSP_HANDLER_H_


/* RSP �n���h�� */
typedef struct RSP_HNDL_ {
	ULONG		cPrmMon[2];			/* �Œ胂�j�^�I��1/2 */
	LONG		enableLtMon[2];		/* ���b�`���j�^�����ϊ��v�� */
	BOOL		rspSetDone;			/* �R�}���h���������t���O */
	BOOL		exRspSetDone;		/* �T�u�R�}���h���������t���O */
	BOOL		last_L_CMP[2];		/* �O��̃��b�`�������1/2 */
} RSP_HNDL;



#define RSP_HANDLER_H_
#endif /* RSP_HANDLER_H_ */



