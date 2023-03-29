/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MotorPoleFind.h : 磁極検出制御用ヘッダーファイル											*/
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
/*	Note	:	初版	2011.01.25	Y.Oka	for INGRAM												*/
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
/*		位相定数																					*/
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
/*		磁極検出シーケンスステップ																	*/
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
/*		速度指令作成シーケンスステップ																*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_VREF_STEP {
	MPFLAU_INIT		= 0,
	MPFLAU_ACC,
	MPFLAU_CONST,
	MPFLAU_DEC,
	MPFLAU_WAIT,
};

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令作成シーケンスステップ																*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_PREF_STEP {
	MPFPOS_INIT		= 0,
	MPFPOS_PLUS,
	MPFPOS_WAIT1,
	MPFPOS_MINUS,
	MPFPOS_WAIT2,
};

/*--------------------------------------------------------------------------------------------------*/
/*		推力指令作成シーケンスステップ																*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_TREF_STEP {
	MPFTRQ_INIT		= 0,
	MPFTRQ_ACC,
	MPFTRQ_CONST,
	MPFTRQ_DEC,
	MPFTRQ_END,
};

/*--------------------------------------------------------------------------------------------------*/
/*		動作確認シーケンスステップ																	*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_CONF_STEP {
	MPFCSTEP_WAIT		= 0,
	MPFCSTEP_PHSCHANGE,
	MPFCSTEP_ADJUST,
	MPFCSTEP_PHSSTORE,
	MPFCSTEP_PHSREVERT,
};

/*--------------------------------------------------------------------------------------------------*/
/*		8分割領域チェックシーケンスステップ															*/
/*--------------------------------------------------------------------------------------------------*/
enum POLE_FIND_ZONE_CHK_STEP {
	PAREACHECK_000	= 0,
	PAREACHECK_001,
	PAREACHECK_002,
	PAREACHECK_003,
};

/*--------------------------------------------------------------------------------------------------*/
/*		d-q軸ずれ方向																				*/
/*--------------------------------------------------------------------------------------------------*/
#define AXISERR_EQUAL		0				/* dAxisMax = qAxisMax									*/
#define AXISERR_MINUS		1				/* dAxisMax > qAxisMax									*/
#define AXISERR_PLUS		2				/* dAxisMax < qAxisMax									*/

/*--------------------------------------------------------------------------------------------------*/
/*		その他の定数																				*/
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
	USHORT	pdetloophz;				/* Pn481 : 磁極検出速度ループゲイン									*/
	USHORT	pdetpitime;				/* Pn482 : 磁極検出速度ループ積分時間								*/
	USHORT	pdetmaxspd;				/* Pn485 : 磁極検出指令速度											*/
	USHORT	pdetmaxt;				/* Pn486 : 磁極検出指令加減速時間									*/
	USHORT	pdetclmpt;				/* Pn487 : 磁極検出指令一定速時間									*/
	USHORT	pdetwait;				/* Pn488 : 磁極検出指令待ち時間										*/
	USHORT	pdetrepeat;				/* Pn48D : 磁極検出繰り返し回数										*/
	USHORT	pdetdistok;				/* Pn48E : 磁極検出可動範囲											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	pdetjrate;				/* Pn490 : 磁極検出負荷レベル										*/
	USHORT	pdetintegless;			/* Pn491 : 磁極検出積分レスゲイン									*/
	USHORT	pdetinteglesst;			/* Pn492 : 磁極検出積分レス開始時間									*/
	USHORT	pdetmaxspd_r;			/* Pn493 : 磁極検出指令速度											*/
	USHORT	pdetdistok_r;			/* Pn494 : 磁極検出可動範囲											*/
	USHORT	pdetmaxtrq;				/* Pn495 : 磁極検出確認トルク指令									*/
	USHORT	pdetinctime;			/* Pn496 : 磁極検出確認トルク指令加減時間							*/
	USHORT	pdetconsttime;			/* Pn497 : 磁極検出確認トルク指令一定時間							*/
	USHORT	pdetdegreeok;			/* Pn498 : 磁極検出誤差許容範囲										*/
	USHORT	pdetrevlvspdref;		/* Pn499 : 磁極検出速度指令時逆走レベル								*/
	USHORT	pdetrevlvwait;			/* Pn49A : 磁極検出待ち時間時逆走レベル								*/
	USHORT	pdetoslevel;			/* Pn49B : 磁極検出時過速度レベル									*/
} MPFIND_UPRM;

void	MpoleFind_CalcSpeedCommandPrm( MPFIND *MpFind, MENCV *MencV,
    	                           BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm );
void	MpoleFind_CalcSpeedControlPrm( MPFIND *MpFind,
    	                           USHORT trqfil11, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  );
void	MpoleFind_CalcErrorLevel( MPFIND *MpFind, MENCV *MencV,
    	                         MENCPRM *MencP, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  );



#endif /* _MOT_POLE_FIND_H */
/***************************************** end of file **********************************************/
