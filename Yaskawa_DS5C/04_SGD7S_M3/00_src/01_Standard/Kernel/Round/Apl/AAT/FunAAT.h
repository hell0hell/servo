/*****************************************************************************
Description		: Fn201  Advanced Auto Tuning Mode Headfile
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

#ifndef FUN_AAT_H_
#define FUN_AAT_H_


#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File*/
#include	"Monitor.h"		/* Un Monitor *///<S197>

/*--------------------------------------------------------------------------------------------------*/
/*		 Advanced Auto Tuning  State																*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnRefAATState
{
	AAT_SEQ_INIT = 0L,							/* �������							*/
	AAT_SEQ_MODESET = 1L,						/* �`���[�j���O���[�h�ݒ���		*/
	AAT_SEQ_JRATSET = 2L,						/* �C�i�[�V���ݒ���				*/
	AAT_TUNNING_WAIT = 3L,						/* �`���[�j���O�҂��������			*/
	AAT_SEQ_ENDDISP = 4L,						/* �I���\�����						*/
	AAT_SEQ_END = 5L,							/* �ŏI�ݒ���						*/
	AAT_SEQ_WAIT = 6L,							/* �I���҂����						*/
	AAT_SEQ_RETURN = 7L, 						/* �㏈�����						*/
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/

typedef struct _FnAATV {
	struct {
		enum _enumFnRefAATState State;	/* Advanced Auto Tuning (Fn201) Execution State */

		LONG OpeModeIdx;			/* Operation Mode Register Index 				*/
		LONG OpeSetIdx;				/* Operation Set Register Index 				*/
		LONG TimeCntrIdex;			/*���S��~����[s] Register Index				*/
		LONG AdatModeIdx;			/* 0x2090 : �A�h�o���X�g�I�[�g�`���[�j���O�֘A���[�h�ݒ� Register Index	*/
		LONG AdatLevelIdx;			/* 0x2091 : �A�h�o���X�g�I�[�g�`���[�j���O���[�h�ݒ� Register Index	*/
		LONG AdatMovepIdx;			/* 0x2092 : �����ݽ�AT�ړ�����	Register Index	*/
		LONG AdatStateIdx;			/* 0x2094 : �����ݽ�AT�����m�F	Register Index	*/
		LONG AdatFilTypeIdx;		/* 0x2095 : �����ݽ�AT̨������	Register Index	*/

		BOOL AdatMode;				/* 0x2090 : �����ݽ�ATӰ��						*/
		ULONG AdatLevel;			/* 0x2091 : �����ݽ�AT���x��					*/
		LONG AdatMovep;				/* 0x2092 : �����ݽ�AT�ړ�����					*/
		LONG AdatState;				/* 0x2094 : �����ݽ�AT�����m�F					*/
		ULONG AdatFilType;			/* 0x2095 : �����ݽ�AT̨������					*/

		LONG LoophzIdex;			/* Pn100 : ���x���[�v�Q�C��		Register Index			*/
		LONG PitimeIdex;			/* Pn101 : ���x���[�v�ϕ����萔	Register Index			*/
		LONG PosgnIdex;				/* Pn102 : �ʒu���[�v�Q�C��		Register Index			*/
		LONG JrateIdex;				/* Pn103 : �������[�����g��		Register Index			*/
		LONG MdlgnIdex;				/* Pn141 : ���f���Ǐ]����Q�C��	Register Index			*/
//		LONG Syssw1Idex;			/* PnE0B : �V�X�e���X�C�b�`�P	Register Index			*///<S197>

		LONG IntModeStateIdex;		/* 0xE705 : ����������						*///<S197

		LONG ModeSetPtr;			/* ���[�h�ݒ�F�V�[�P���X�|�C���^				*/
		LONG DPos;					/* �_�Ō���										*/
		LONG JratSetPtr;			/* �������[�����g����F�V�[�P���X�|�C���^		*/
//		LONG JratErr;				/* �������[�����g����F�G���[���				*//*TODO: Delete */
		ULONG JratTimeCntr;			/* ���S��~���ԃJ�E���^							*/
//		BOOL ServonFlag;			/* Servo on flag								*//*TODO: Delete */
		ULONG SafeTime;				/* Safe time									*/
		ULONG DispGain;				/* Disp Gain									*/
		LONG AmpType;				/* Amp   type									*/

		BOOL	DrvErrDetect;
		LONG	SafeStopCmd;														  /* <S195> */

		LONG Reserved[(256/4)-1-8-5-6-8]; 	/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNAATV;

/***************************************** end of file **********************************************/
#endif /* FUN_AAT_H_ */
