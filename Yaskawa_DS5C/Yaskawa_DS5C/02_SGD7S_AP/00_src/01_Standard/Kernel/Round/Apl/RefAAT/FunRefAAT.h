/*****************************************************************************
Description		: Fn202  Reference Input Type Advanced Auto Tuning Mode Headfile
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.17		created

*****************************************************************************/

#ifndef FUN_REF_AAT_H_
#define FUN_REF_AAT_H_


#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File*/

/*--------------------------------------------------------------------------------------------------*/
/*		 Advanced Auto Tuning  State																*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnRefAATState
{
	REF_AAT_SEQ_INIT = 0L,				/* �������							*/
	REF_AAT_SEQ_MODESET = 1L,			/* �`���[�j���O���[�h�ݒ���		*/
	REF_AAT_SEQ_JRATSET = 2L,			/* �C�i�[�V���ݒ���				*/
	REF_AAT_TUNNING_WAIT = 3L,			/* �`���[�j���O�҂��������			*/
	REF_AAT_SEQ_ENDDISP = 4L,			/* �I���\�����						*/	
	REF_AAT_SEQ_END = 5L,				/* �ŏI�ݒ���						*/
	REF_AAT_SEQ_WAIT = 6L,				/* �I���҂����						*/
	REF_AAT_SEQ_RETURN 			= 7L		/* �㏈�����									*/
};


/****************************************************************************************************/
/*		STRUCT DEFINITION													  */
/****************************************************************************************************/

typedef struct _FnREFAATV {
	struct {
		enum _enumFnRefAATState State;		/* Fn202  Reference Input Type Execution State */

		LONG OpeModeIdx;				/* Operation Mode Register Index */
		LONG OpeSetIdx;					/* Operation Set Register Index */
		LONG AdatModeIdx;					/* 0x2090: �A�h�o���X�g�`�s���[�h Index			*/
		LONG AdatLevelIdx;				/* 0x2091 : �����ݽ�AT���x��	Index					*/
		LONG AdatStateIdx;				/* 0x2094 : �����ݽ�AT�����m�F	Index					*/
		LONG AdatFilTypeIdx;			/* 0x2095 : �����ݽ�AT̨������	Index					*/
		ULONG AdatMode;						/* 0x2090: �A�h�o���X�g�`�s���[�h				*/
		ULONG AdatLevel;			/* 0x2091 : �����ݽ�AT���x��								*/
		ULONG AdatState;					/* 0x2094 : �����ݽ�AT�����m�F					*/
		ULONG AdatFilType;			/* 0x2095 : �����ݽ�AT̨������								*/

		LONG LoophzIdex;			/* Pn100 : ���x���[�v�Q�C��									*/
		LONG PitimeIdex;			/* Pn101 : ���x���[�v�ϕ����萔								*/
		LONG PosgnIdex;				/* Pn102 : �ʒu���[�v�Q�C��									*/
		LONG MdlgnIdex;				/* Pn141 : ���f���Ǐ]����Q�C��								*/

		LONG ModeSetPtr;			/* ���[�h�ݒ�F�V�[�P���X�|�C���^							*/
		LONG DPos;					/* �_�Ō���													*/
		LONG JratSetPtr;			/* �������[�����g����F�V�[�P���X�|�C���^					*/
		ULONG DispGain;				/* Disp Gain				*/

		LONG Reserved[(256/4)-1-10-4-4];	/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNREFAATV;

/***************************************** end of file **********************************************/
#endif /* FUN_REF_AAT_H_ */
