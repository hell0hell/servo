#ifndef __BACKLASH_COMP_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BacklashComp.h : �o�b�N���b�V�␳�@�\	���W���[����`										*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2011.04.05	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "Bprm.h"
#include "MLib.h"

/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�o�b�N���b�V�␳�p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* �o�b�N���b�V�␳�p�萔���ϐ���`					*/
	struct {									/* �o�b�N���b�V�␳�p�萔�f�[�^��`					*/
		BOOL	BacklashDir;					/* �o�b�N���b�V�␳�����I��ݒ�						*/
		LONG	CompPulse;							/* �o�b�N���b�V�␳��			[Pulse]			*/
		LONG	Kfil;								/* �o�b�N���b�V�t�B���^�Q�C��					*/
		LONG	KfilTmp;							/* �o�b�N���b�V�t�B���^�Q�C�����Z�p				*/
		LONG	MaxCompPulse;						/* �o�b�N���b�V�␳�ʐ����l		[Pulse]			*/
		ULONG	MaxCompRef;							/* �o�b�N���b�V�␳�ʐ����l		[�w�ߒP��]		*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* �o�b�N���b�V�␳�p�ϐ��f�[�^��`					*/
		LONG	BlshCmpAmountPls;					/* �o�b�N���b�V�␳�ʍ����l						*/
		LONG	Filo;								/* �o�b�N���b�V�␳�t�B���^�o��					*/
		LONG	LastFilo;							/* �O��o�b�N���b�V�␳�t�B���^�o��				*/
		LONG	Cmppls;								/* �o�b�N���b�V�␳��			[Pulse]			*/
		LONG	Cmpgrrem;							/* �d�q�M�A�ϊ���o�b�N���b�V�␳�ʗ]��			*/
		LONG	Cmppos;								/* �o�b�N���b�V�␳�ʒu			[Pulse]			*/
		LONG	remx;								/* �o�b�N���b�V�␳�t�B���^�v�Z�]��				*/
	} var;
} BLSHCMP;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define		BLSHCOMP_UPLIM		500000				/* �o�b�N���b�V�␳�ʍő�l						*/
#define		FOR_BACKLASH		0x00				/* ���]���Ƀo�b�N���b�V�␳						*/
#define		REV_BACKLASH		0x01				/* �t�]���Ƀo�b�N���b�V�␳						*/


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void	BlshCompensateBacklash( BLSHCMP *BlshCmp, EGEAR *Egear, LONG *dPosRef, LONG *dPcmd, BOOL *RefZ, BOOL BlshActive );
void	BlshCalcMaxBlshCompPulse( BLSHCMP *BlshCmp, EGEAR *Egear, BPRMDAT *Bprm );
void	BlshCalcCompensateAmount( BLSHCMP *BlshCmp, EGEAR *Egear, LONG blcm2l );
void	BlshCalcCompensateTime( BLSHCMP *BlshCmp, LONG SvCycleUs, USHORT blfiltime );
void	BlshInvaildCompensate( BLSHCMP *BlshCmp );
void	BlshInitializeCompensate( BLSHCMP *BlshCmp );



/****************************************************************************************************/
#define __BACKLASH_COMP_H
#endif /* __BACKLASH_COMP_H */
