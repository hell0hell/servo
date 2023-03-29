#ifndef _PRGRAM_JOG_H
#define _PRGRAM_JOG_H
/****************************************************************************
Description	: Program Jog
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira  2011.03.08    created

*****************************************************************************/
#include "Basedef.h"
#include "CtrlLoopHandler.h"
#include "BaseCmnStruct.h"
#include "AlarmManager.h"
#include "KLib.h"
#include "MLib.h"

/************************************************************************************************/
/*																								*/
/*		�v���O����JOG�^�]�p�f�[�^��`															*/
/*																								*/
/************************************************************************************************/
/*----------------------------------------------------------------------------------------------*/
/*		�v���O����JOG�^�]�f�[�^�\����															*/
/*----------------------------------------------------------------------------------------------*/
typedef	struct PJOGV{
	struct{							/* �v���O����JOG�g�p�萔�f�[�^								*/
		LONG	Distance;			/* �v���O����JOG�^�]�ڕW�ʒu		[�w�ߒP��]				*/
		LONG	MaxSpd;				/* �v���O����JOG�^�]�ō����x		[2^24/OvrSpd]			*/
		LONG	AccTime;			/* �v���O����JOG�^�]����������		[us]					*/
		USHORT	WaitTime;			/* �҂�����							[ms]					*/
		USHORT	MTimes;				/* �ړ���													*/
		CHAR	Pattern;			/* �v���O����JOG�^�]�w�߃p�^�[��							*/
		CHAR	RevDir;				/* �v���O����JOG�^�]���[�h�J�n����							*/
		CHAR	PrmUnMatch;			/* �p�����[�^�g���킹�ُ���								*/
		CHAR	PrmUnMatch2;		/* �p�����[�^�g���킹�ُ���2								*/
	} Cnst;
/*----------------------------------------------------------------------------------------------*/
	struct{							/* �v���O����JOG�g�p�ϐ��f�[�^								*/
		LONG	AbsPosCmd;			/* �v���O����JOG�ʒu�w��			[�w�ߒP��	]			*/
		ULONG	TimeWork;			/* �҂����Ԍv���p�f�[�^										*/
		USHORT	DispTime;			/* �p�l�I�y�\���p�f�[�^	<V068>								*/
		USHORT	MCntr;				/* �J��Ԃ���												*/
		USHORT	SeqPtr;				/* �v���O����JOG�^�]�w�ߍ쐬�V�[�P���X�|�C���^				*/
		UCHAR	RoutCmd;			/* �v���O����JOG�^�]�w��	(Round  -> Scan C)				*/
		UCHAR	CoutCmd;			/* �v���O����JOG�^�]�w��	(Scan C -> Scan B)				*/
		UCHAR	State;				/* �v���O����JOG�^�]���									*/
		BOOL	ScanCRunFlg;		/* ScanC��ON,Round��OFF����t���O(���荞�ݑ�p)				*/
	} Var;
} PJOGV;

/*----------------------------------------------------------------------------------------------*/
/*		�v���O����JOG�\����																		*/
/*----------------------------------------------------------------------------------------------*/
typedef	struct PJOGHNDL{
	PJOGV   PJogV;
	PCMKPV	PJogPcmk;
	SHORT	PJogState;
}PJOGHNDL;

/*----------------------------------------------------------------------------------------------*/
/*		�v���O����JOG�^�]�X�e�[�^�X��`															*/
/*----------------------------------------------------------------------------------------------*/
#define	PJOG_INIT	0x00				/* �C�j�V�������										*/
#define	PJOG_ABORT	0x10				/* ���f���												*/
#define	PJOG_START	0x01				/* �^�]���												*/
#define	PJOG_AFTER	0x11				/* �^�]�㏈�����										*/
#define	PJOG_END	0x02				/* �I�����												*/
#define	PJOG_ERROR	0x03				/* �ُ�I��												*/
/*----------------------------------------------------------------------------------------------*/
/*		�v���O����JOG�^�]�R�}���h��`															*/
/*----------------------------------------------------------------------------------------------*/
#define	PJOGCMD_NONE	0x00			/* �R�}���h�Ȃ�											*/
#define	PJOGCMD_INIT	0x01			/* �C�j�V�����C�Y										*/
#define	PJOGCMD_ABORT	0x02			/* ���f													*/
#define	PJOGCMD_START	0x03			/* �^�]													*/

#define		LMTLOWSPEED	10
/*----------------------------------------------------------------------------------------------*/
/*		�v���O����JOG�^�]�֘A�֐���`															*/
/*----------------------------------------------------------------------------------------------*/
void PrgJogReset( PJOGHNDL *PJogHdl );
void IprmcalPrgJog( PJOGHNDL *PJogHdl, BPRMDAT *Bprm, POS_MNG_HNDL *PosManager );
void CpxPrgJogPosSequence( PJOGHNDL *PJogHdl, SEQ_CTRL_OUT *SeqCtrlOut, 
												ALMMAN_HANDLE *AlmManager, LONG FnCtrlMcmd ); 
BOOL PrgJogMakePositionReference( PJOGHNDL *PJogHdl, LONG *dPosRefo );

#endif //_PRGRAM_JOG_H 
/***************************************** end of file ******************************************/
