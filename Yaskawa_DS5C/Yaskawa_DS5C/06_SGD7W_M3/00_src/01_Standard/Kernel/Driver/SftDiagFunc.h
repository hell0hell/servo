/*********************************** Start of file **************************************************/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SftDiagFunc.h		: Safety Diagnostic function for Sigma-7(SGD7S/W)						*/
/*																									*/
/*		Identification																				*/
/*			Module ID		: 0002(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Module version	: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.03.12															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S1F6>																*/
/*																									*/
/****************************************************************************************************/
#ifndef SFTDIAGFUNC_H
#define SFTDIAGFUNC_H

#include "Basedef.h"

/*--------------------------------------------------------------------------------------------------*/
/*		HWBB���o�p�ϐ���`																			*/
/*--------------------------------------------------------------------------------------------------*/
//typedef	struct DET_HWBB_STS {
	//BOOL			HwbbState;				/* Hard-wired Baseblock Status							*//* <HWBB> */
	//BOOL			Bbon1;					/* BBON1 Signal											*//* <HWBB> */
	//BOOL			Bbon2;					/* BBON2 Signal											*//* <HWBB> */
	//BOOL			Bbon1Edge;				/* BBON1 Signal Rising Edge								*//* <HWBB> */
	//BOOL			Bbon2Edge;				/* BBON2 Signal Rising Edge								*//* <HWBB> */
	//BOOL			GateDrvDefect;			/* Gate Drive Defect Status Flag						*//* <HWBB> */
	//BOOL			LackOneBbon;			/* One BBON Signal Input Only							*//* <HWBB> */
	//BOOL			hwbbsupport;			/* HWBB Support flag(PnE0D.bit9 = 1�ŗL��)				*//* <HWBB> */
//} DET_HWBB_STS;

/*--------------------------------------------------------------------------------------------------*/
/*		�����M���ُ팟�o(Test Pulse Diagnostic)�p��`									<S0F1>		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TEST_PULSE_DIAG {
	BOOL			DpInEnable;				/* DPIN Signal Enable									*/
	BOOL			DpInLastEnable;			/* DPIN Signal Enable (�O��l)							*/
	BOOL			DpInState;				/* DPIN Signal Status									*/
	BOOL			DpInLastState;			/* DPIN Signal Status (�O��l)							*/
	BOOL			DpInDiagState;			/* DPIN Signal Diag. Status								*/
	BOOL			DpInLastDiagState;		/* DPIN Signal Diag. Status (�O��l)					*/
	USHORT			DpInErrCnt;				/* DPIN Signal Error Count								*/
	USHORT			DpInErrLvl;				/* DPIN Signal Error Level								*/
} TEST_PULSE_DIAG;

/*--------------------------------------------------------------------------------------------------*/
/*		�Z�[�t�e�B���͐M���^�C�~���O�ُ�`�F�b�N�p�\���̒�`										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_HWBB_TIMING {
	struct	{							/* �Z�[�t�e�B���͐M���^�C�~���O�ُ�`�F�b�N�p�萔��`		*/
		LONG	MaxBbonTimeDiff;			/* ���͐M�����ԍ��Ď��F�ő勖�e����						*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* �Z�[�t�e�B���͐M���^�C�~���O�ُ�`�F�b�N�p�ϐ���`		*/
		LONG	BbonTimeDiffCnt;			/* �Z�[�t�e�B���͐M���^�C�~���O�ُ펞�ԃJ�E���g			*/
	} var;
} CHECK_HWBB_TIMING;


#define	GET_DPIN01( )	(HALDRV_GET_GPIO06)
#define	GET_DPIN02( )	(HALDRV_GET_GPIO07)
#define	GET_DPIN11( )	(HALDRV_GET_GPIO00)
#define	GET_DPIN12( )	(HALDRV_GET_GPIO01)

/* GPIO0�o�̓s�� */
enum	TESTPLSOUT {
	SIL3_TEST_PLS1	= 3,
	SIL3_TEST_PLS0	= 4,
};


/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
BOOL AdetTestPulseDiagnostic( TEST_PULSE_DIAG *TestPulseDiag );	/*<S1D1>*/
void SftReverceTestPls( void );

#endif /* SFTDIAGFUNC_H */

/*********************************** End of file ****************************************************/
