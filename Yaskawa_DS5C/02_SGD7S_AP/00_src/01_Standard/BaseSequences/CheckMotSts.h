/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckMotSts.h : ���[�^��ԊĎ�������`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef CHECK_MOT_STS_H
#define CHECK_MOT_STS_H

#include "Bprm.h"
#include "BaseCmnStruct.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^��]���o�p��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define MOTSPD_TGMGN	12710		/* 12710 / 16777216 x 6600 �� 5rpm		LpxCheckMotorRun( )�Q��	*/
#define MOTSPD_DBLVL	60000		/* 16777216 x 0.7/175 = 67108 -> 60000	LpxCheckMotorStop( )�Q��*/

/*--------------------------------------------------------------------------------------------------*/
/*		ScanC���[�^�X�e�[�^�X�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct SEQ_MOT_STS {
	struct {								/* ���[�^�X�e�[�^�X�萔��`								*/
		LONG			VcmpSpdLevel;			/* ���x��v���o��				[2^24/OvrSpd]		*/
		LONG			TgonSpdLevel;			/* ���[�^��]���o���x��			[2^24/OvrSpd]		*/
	} conf;
	struct {								/* ���[�^�X�e�[�^�X�ϐ���`								*/
//		BOOL			TgOn;					/* TGON Status										*/
//		BOOL			Vcmp;					/* VCMP Status										*/
//		BOOL			MotStop;				/* Motor Stop Status								*/
		LONG			MotSpd;					/* Motor Speed										*/
		LONG			AbsMotSpd;				/* Absolute Motor Speed								*/
		LONG			LstMotSpd;				/* Last Motor Speed for ModeSwAcc Cal.				*/
		LONG			MotPos;					/* Motor Position									*/
		LONG			dMotPos;				/* Delta Motor Position								*/
		LONG			PcmdCntr;				/* Pulse Command Counter	[pulse]					*/
		LONG			dPcmdCnt;				/* Delta Pulse Command		[pulse]					*/

		LONG			UnMotSpd;				/* Un000  : ���[�^���x				[pulse/64ms]	*/
		LONG			UnPfbk64ms;				/* Un000  : ���[�^���x				[pulse/64ms]	*/
		LONG			UnPcmd64ms;				/* Un007  : �w�߃p���X���x			[pulse/64ms]	*/
		LONG			Pfbk64SumCnt;			/* ���j�^�p���[�^���x���Z�p							*/
		LONG			Pcmd64Sum;				/* ���j�^�p�w�߃p���X���x���Z�p						*/
		LONG			Pfbk64Sum;				/* ���j�^�p���[�^���x���Z�p							*/
		LONG			MotSpd64Sum;			/* ���j�^�p���[�^���x���Z�p							*/

		LONG			IsqrSum;				/* ���j�^�p�d�����׉��Z�p							*/
		LONG			IsqrSumCnt;				/* ���j�^�p�d�����׉��Z�p							*/
		LONG			UnIsqrSum;				/* Un009  : �ݐϕ��ח�		[(15000/MaxTrq)/10sec]	*/
		ULONG			UnCrntRef;				/* Un142 : �Q��d���w�ߒl	[(15000/MAXTRQ)^2]		*/
		ULONG			UnIsqrSumMax;			/* Un145 : �ݐϕ��ח��ő�l	[(15000/MaxTrq)/10sec]	*/

		LONG			TrqRefMon;				/* �g���N�w�߃��j�^					[2^24/MaxTrq]	*/

		LONG			UnPcmd64Sum;			/* �w�߃p���X���x <S1B6>			[64ms]			*/
	} var;
} SEQ_MOT_STS;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void SMotSts_InputMotorStatus( SEQ_MOT_STS *MotSts, BASE_CTRL_OUT *BaseCtrlOut,
                               SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm );
BOOL	LpxCheckMotorStop( LONG AbsMotSpd );
BOOL	LpxCheckMotorVcmp( SEQ_MOT_STS *MotSts, LONG SpdRef, BOOL SpdCtrlMode );
BOOL	LpxCheckMotorRun( SEQ_MOT_STS *MotSts, LONG LstTgonSts );
BOOL    LpxCheckMotorSuspendLevel( SEQ_MOT_STS *MotSts,LONG NorSuspSpd);/*<S201>*/


#endif /* CHECK_MOT_STS_H */
/***************************************** end of file **********************************************/
