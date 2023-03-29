/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MotorPoleFind.h : ���Ɍ��o����p�w�b�_�[�t�@�C��											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2011.01.25	Y.Oka	for INGRAM												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef	_MOT_POLE_FIND_H
#define _MOT_POLE_FIND_H


#include "Basedef.h"
#include "MpfHandler.h"
#include "alarm.h"
#include "EncData.h"
#include "Bprm.h"



/****************************************************************************************************/
/*		DEFINES																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��萔																					*/
/*--------------------------------------------------------------------------------------------------*/
#define	PHASE_DEG000		0
#define PHASE_DEG022_5		4096
#define PHASE_DEG030		5461
#define PHASE_DEG045		8192
#define PHASE_DEG090		16384
#define PHASE_DEG135		24576
#define PHASE_DEG180		32768
#define PHASE_DEG225		40960
#define PHASE_DEG315		57344
#define PHASE_DEG360		65536
/*--------------------------------------------------------------------------------------------------*/
#define SINPHASE_360		16384
#define SINPHASE_090		4096
#define SINPHASE_045		2048


/*--------------------------------------------------------------------------------------------------*/
/*		���Ɍ��o�V�[�P���X�X�e�b�v																	*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_SEQ_STEP {
	MPF_INIT		= 0,
	MPF_INIT2,
	MPF_DRIVE,
	MPF_CAL,
	MPF_STOP,
	MPF_CONFIRM,
	MPF_END,
};

/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�ߍ쐬�V�[�P���X�X�e�b�v																*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_VREF_STEP {
	MPFLAU_INIT		= 0,
	MPFLAU_ACC,
	MPFLAU_CONST,
	MPFLAU_DEC,
	MPFLAU_WAIT,
};

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߍ쐬�V�[�P���X�X�e�b�v																*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_PREF_STEP {
	MPFPOS_INIT		= 0,
	MPFPOS_PLUS,
	MPFPOS_WAIT1,
	MPFPOS_MINUS,
	MPFPOS_WAIT2,
};

/*--------------------------------------------------------------------------------------------------*/
/*		���͎w�ߍ쐬�V�[�P���X�X�e�b�v																*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_TREF_STEP {
	MPFTRQ_INIT		= 0,
	MPFTRQ_ACC,
	MPFTRQ_CONST,
	MPFTRQ_DEC,
	MPFTRQ_END,
};

/*--------------------------------------------------------------------------------------------------*/
/*		����m�F�V�[�P���X�X�e�b�v																	*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_CONF_STEP {
	MPFCSTEP_WAIT		= 0,
	MPFCSTEP_PHSCHANGE,
	MPFCSTEP_ADJUST,
	MPFCSTEP_PHSSTORE,
	MPFCSTEP_PHSREVERT,
};

/*--------------------------------------------------------------------------------------------------*/
/*		8�����̈�`�F�b�N�V�[�P���X�X�e�b�v															*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_ZONE_CHK_STEP {
	PAREACHECK_000	= 0,
	PAREACHECK_001,
	PAREACHECK_002,
	PAREACHECK_003,
};

/*--------------------------------------------------------------------------------------------------*/
/*		d-q���������																				*/
/*--------------------------------------------------------------------------------------------------*/
#define AXISERR_EQUAL		0				/* dAxisMax = qAxisMax									*/
#define AXISERR_MINUS		1				/* dAxisMax > qAxisMax									*/
#define AXISERR_PLUS		2				/* dAxisMax < qAxisMax									*/

/*--------------------------------------------------------------------------------------------------*/
/*		���̑��̒萔																				*/
/*--------------------------------------------------------------------------------------------------*/
#define TRYCOUNT			3



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
LONG	MpoleFindSpeedControl( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL BeComplete );
ALMID_T	MpoleFindSequence( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq, BOOL BaseEnable );
//ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL NetPdetReq );
//ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, LONG dMotPos, KSGAIN Kmovpos );


/****************************************************************************************************/
/*		for Parameter Calculations																	*/
/****************************************************************************************************/
typedef struct MPFIND_UPRM {
	USHORT	pdetloophz;				/* Pn481 : ���Ɍ��o���x���[�v�Q�C��									*/
	USHORT	pdetpitime;				/* Pn482 : ���Ɍ��o���x���[�v�ϕ�����								*/
	USHORT	pdetmaxspd;				/* Pn485 : ���Ɍ��o�w�ߑ��x											*/
	USHORT	pdetmaxt;				/* Pn486 : ���Ɍ��o�w�߉���������									*/
	USHORT	pdetclmpt;				/* Pn487 : ���Ɍ��o�w�߈�葬����									*/
	USHORT	pdetwait;				/* Pn488 : ���Ɍ��o�w�ߑ҂�����										*/
	USHORT	pdetrepeat;				/* Pn48D : ���Ɍ��o�J��Ԃ���										*/
	USHORT	pdetdistok;				/* Pn48E : ���Ɍ��o���͈�											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	pdetjrate;				/* Pn490 : ���Ɍ��o���׃��x��										*/
	USHORT	pdetintegless;			/* Pn491 : ���Ɍ��o�ϕ����X�Q�C��									*/
	USHORT	pdetinteglesst;			/* Pn492 : ���Ɍ��o�ϕ����X�J�n����									*/
	USHORT	pdetmaxspd_r;			/* Pn493 : ���Ɍ��o�w�ߑ��x											*/
	USHORT	pdetdistok_r;			/* Pn494 : ���Ɍ��o���͈�											*/
	USHORT	pdetmaxtrq;				/* Pn495 : ���Ɍ��o�m�F�g���N�w��									*/
	USHORT	pdetinctime;			/* Pn496 : ���Ɍ��o�m�F�g���N�w�߉�������							*/
	USHORT	pdetconsttime;			/* Pn497 : ���Ɍ��o�m�F�g���N�w�߈�莞��							*/
	USHORT	pdetdegreeok;			/* Pn498 : ���Ɍ��o�덷���e�͈�										*/
	USHORT	pdetrevlvspdref;		/* Pn499 : ���Ɍ��o���x�w�ߎ��t�����x��								*/
	USHORT	pdetrevlvwait;			/* Pn49A : ���Ɍ��o�҂����Ԏ��t�����x��								*/
	USHORT	pdetoslevel;			/* Pn49B : ���Ɍ��o���ߑ��x���x��									*/
} MPFIND_UPRM;

void	MpoleFind_CalcSpeedCommandPrm( MPFIND *MpFind, MENCV *MencV,
    	                           BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm );
void	MpoleFind_CalcSpeedControlPrm( MPFIND *MpFind,
    	                           USHORT trqfil11, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  );
void	MpoleFind_CalcErrorLevel( MPFIND *MpFind, MENCV *MencV,
    	                         MENCPRM *MencP, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  );



#endif /* _MOT_POLE_FIND_H */
/***************************************** end of file **********************************************/
