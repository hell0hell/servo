/****************************************************************************
Description	: Definition of MECHATROLINK Response packet
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef RSP_HANDLER_H_


/* RSP �n���h�� */
typedef struct RSP_HNDL_ {
	LONG		enableLtMon;		/* ���b�`���j�^�����ϊ��v��		*/
	BOOL		rspSetDone;			/* �R�}���h���������t���O		*/
	BOOL		exRspSetDone;		/* �T�u�R�}���h���������t���O	*/
	BOOL		last_L_CMP;			/* �O��̃��b�`�������			*/
} RSP_HNDL;



#define RSP_HANDLER_H_
#endif /* RSP_HANDLER_H_ */



