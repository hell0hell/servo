/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g          */
/*                                                                          */
/*            �t�������������@���C�u����	 								*/
/*																			*/
/****************************************************************************/
/* ��`�֐� :                                                               */
/*																			*/
/* void *UtilMemmset()			�������Z�b�g								*/
/* void *UtilMemcpy()			�������R�s�[								*/
/*                                                                          */
/****************************************************************************/
/*	�y���ŗ����z															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/

/* includes */
#include "Basedef.h"
/* defines */


typedef union UNION_DW_ {
	LONG	dw;
	SHORT	wd[2];
	UCHAR	bd[4];
} UNION_DW;


/* globals */
#if defined(__ghs__)
#pragma ghs startzda
#elif !defined(WIN32)
#pragma section sidata begin
#endif
ULONG TimerCnt;
#if defined(__ghs__)
#pragma ghs endzda
#elif !defined(WIN32)
#pragma section sidata end
#endif



/* forward declarations */
void *UtilMemSet(void *s,UCHAR c,LONG n);
void *UtilMemCpy(void *s1,void *s2,LONG n);
char *UtilStrCpy(char *s1, char *s2);

LONG UtilGetDWData(UCHAR *data_ptr);
SHORT UtilGetWData(UCHAR *data_ptr);
VOID UtilSetDWData(UCHAR *buf_ptr, LONG data);
VOID UtilSetWData(UCHAR *buf_ptr, SHORT data);
VOID UtilSetLLData(VOID *buf_ptr, VOID *data_ptr);

/****************************************************************************/
/* �T�v		�F�������Z�b�g�֐�												*/
/****************************************************************************/
/* ����		�Fvoid *s �F�L���Ώ�											*/
/*			  UCHAR c �F�ݒ肷��l											*/
/*			  LONG n  : �ݒ�o�C�g��										*/
/* �߂�l	�Fs       �F�L���Ώ�											*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
void *UtilMemSet(void *s,UCHAR c,LONG n)
{
	CHAR *p = s;
	while(n>0)
	{
		*p = (CHAR)c;
		p++;
		n--;
	}
	return s;
}

/****************************************************************************/
/* �T�v		�F�������R�s�[�֐�												*/
/****************************************************************************/
/* ����		�Fvoid *s1 �F���ʐ�												*/
/*			  void *s2 �F���ʌ�												*/
/*			  LONG n  : ���ʃo�C�g��										*/
/* �߂�l	�Fs       �F�L���Ώ�											*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
void *UtilMemCpy(void *s1,void *s2,LONG n)
{
	char *p=s1,*s=s2;
	
	while(n>0)
	{
		*p = *s;
		*p++;
		*s++;
		n--;
	}
	return s1;
}

#if 0
/****************************************************************************/
/* �T�v		�F������R�s�[�֐�												*/
/****************************************************************************/
/* ����		�Fchar *s1 �F���ʐ�												*/
/*			  char *s2 �F���ʌ�												*/
/* �߂�l	�Fs       �F�L���Ώ�											*/
/****************************************************************************/
char *UtilStrCpy(char *s1, char *s2)
{
	char *p;
	for(p = s1; *s1 = *s2; s1++, s2++);
	return p;
}
#endif

/****************************************************************************/
/* �T�v		�F���J�g�������N�o�b�t�@����w��̃t�B�[���h����n�܂�S�o�C�g	*/
/*		      �f�[�^��؂�o���ALONG�^�ŕԂ��܂��B							*/
/*		      ������ڽ��ܰ�ޱ������s���Ȃ��ׂ̏���						*/
/****************************************************************************/
/* ����		�FUCHAR* data_ptr 	�F�f�[�^�̐擪�̃|�C���^					*/
/* �߂�l	�FLONG data			�F�S�o�C�g�f�[�^							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
LONG UtilGetDWData(UCHAR *data_ptr)
{
	UNION_DW data;

	data.bd[0] = data_ptr[0];
	data.bd[1] = data_ptr[1];
	data.bd[2] = data_ptr[2];
	data.bd[3] = data_ptr[3];

	return data.dw;
}

/****************************************************************************/
/* �T�v		�F���J�g�������N�o�b�t�@����w��̃t�B�[���h����n�܂�Q�o�C�g	*/
/*		      �f�[�^��؂�o���ASHORT�^�ŕԂ��܂��B							*/
/*		      ������ڽ��ܰ�ޱ������s���Ȃ��ׂ̏���						*/
/****************************************************************************/
/* ����		�FUCHAR* data_ptr 	�F�f�[�^�̐擪�̃|�C���^					*/
/* �߂�l	�FSHORT data		�F�Q�o�C�g�f�[�^							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
SHORT UtilGetWData(UCHAR *data_ptr)
{
	UNION_DW data;

	data.bd[0] = data_ptr[0];
	data.bd[1] = data_ptr[1];

	return data.wd[0];
}

/****************************************************************************/
/* �T�v		�F���J�g�������N�o�b�t�@�֎w��̃t�B�[���h����n�܂�S�o�C�g	*/
/*		      �f�[�^��ݒ肵�܂��B											*/
/*		      ������ڽ��ܰ�ޱ������s���Ȃ��ׂ̏���						*/
/****************************************************************************/
/* ����		�FUCHAR* buf_ptr 	�F�ݒ��̐擪�̃|�C���^					*/
/* 			�FLONG data			�F�S�o�C�g�f�[�^�i�ݒ�f�[�^�j				*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID UtilSetDWData(UCHAR *buf_ptr, LONG data)
{
	UCHAR *p = (UCHAR*)&data;

	buf_ptr[0] = *(p+0);
	buf_ptr[1] = *(p+1);
	buf_ptr[2] = *(p+2);
	buf_ptr[3] = *(p+3);
}

/****************************************************************************/
/* �T�v		�F���J�g�������N�o�b�t�@�֎w��̃t�B�[���h����n�܂�S�o�C�g	*/
/*		      �f�[�^��ݒ肵�܂��B											*/
/*		      ������ڽ��ܰ�ޱ������s���Ȃ��ׂ̏���						*/
/****************************************************************************/
/* ����		�FUCHAR* buf_ptr 	�F�ݒ��̐擪�̃|�C���^					*/
/* 			�FLONG data			�F�Q�o�C�g�f�[�^�i�ݒ�f�[�^�j				*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID UtilSetWData(UCHAR *buf_ptr, SHORT data)
{
	UCHAR *p = (UCHAR*)&data;

	buf_ptr[0] = *(p+0);
	buf_ptr[1] = *(p+1);
}

/****************************************************************************/
/* �T�v		�F���J�g�������N�o�b�t�@�֎w��̃t�B�[���h����n�܂�W�o�C�g	*/
/*		      �f�[�^��ݒ肵�܂��B											*/
/*		      ������ڽ��ܰ�ޱ������s���Ȃ��ׂ̏���						*/
/****************************************************************************/
/* ����		�FUCHAR* buf_ptr 	�F�ݒ��̐擪�̃|�C���^					*/
/* 			�FUCHAR* data_ptr	�F�W�o�C�g�f�[�^�i�ݒ�f�[�^�j				*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID UtilSetLLData(VOID *buf_ptr, VOID *data_ptr)
{
	UCHAR *buf_wk = buf_ptr,*data_wk = data_ptr;

	*buf_wk = *data_wk;
	*(buf_wk+1) = *(data_wk+1);
	*(buf_wk+2) = *(data_wk+2);
	*(buf_wk+3) = *(data_wk+3);
	*(buf_wk+4) = *(data_wk+4);
	*(buf_wk+5) = *(data_wk+5);
	*(buf_wk+6) = *(data_wk+6);
	*(buf_wk+7) = *(data_wk+7);
}
