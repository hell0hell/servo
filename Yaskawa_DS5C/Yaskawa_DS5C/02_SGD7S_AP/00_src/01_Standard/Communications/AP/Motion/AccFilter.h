/****************************************************************************/
/*																			*/
/*																			*/
/*		ACC FILTER �w�b�_�t�@�C��											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		ACC_FIL_H_
#define		ACC_FIL_H_

#include "ComCfg.h"
//#include "MtPRM.h"

/* defines */
#ifdef ACC_FIL_DISABLE
 #define ACF_BUF_SZ		1
#else
 #define REAL_BUF_SZ	(512000*4)/(SCAN_N_DEFCYCLE*CFG_TIO_BASE_TIME)
 #if (REAL_BUF_SZ > ACF_MAX_SZ)
  #define ACF_BUF_SZ	ACF_MAX_SZ
 #else
  #define ACF_BUF_SZ	REAL_BUF_SZ
 #endif
#endif

#define ACF_SW_NOFIL		0
#define ACF_SW_EXPFIL		1
#define ACF_SW_MAFIL		2


/* typedefs: struct */
typedef struct ACF_MAFIL_ {
	LONG		maSum;		/* �ړ����σt�B���^�T�� */
	LONG		maRem;		/* �ړ����σt�B���^�]�� */
	LONG*		maBuf;		/* �ړ����σt�B���^�o�b�t�@ */
	USHORT		maTb;		/* �ړ����σt�B���^���萔 */
	USHORT		maBufPtr;	/* �ړ����σt�B���^�o�b�t�@�|�C���^ */
	USHORT		maBufCnt;	/* �ړ����σt�B���^�o�͎��� */
} ACF_MAFIL;

typedef struct ACF_IMAFIL_ {
	LONG		inPosSum;	/* ���͈ړ��ʐώZ */
	LONG		dPos;		/* �ړ��ʑ����l */
	LONG		lstPos;		/* �O��w�ߓ��͈ړ��� */
	LONG		dPosSum;	/* ��ԋ����ώZ */
	LONG		iRem;		/* �ړ����σt�B���^�]�� */
	UCHAR		iTbSft;		/* �ړ����σt�B���^���萔 */
	UCHAR		iTb;		/* �ړ����σt�B���^���萔 */
	UCHAR		iCnt;		/* ��ԉ񐔃J�E���^*/
} ACF_IMAFIL;


typedef struct ACFHNDL_ {
	UCHAR		filSw;		/* ���݃t�B���^�^�C�v */
	BOOL		filComp;	/* �t�B���^�����o������ */
	USHORT		expRem;		/* �w���֐��t�B���^�o�b�t�@�]�� */
	LONG		expBuf;		/* �w���֐��t�B���^�o�b�t�@ */
	LONG		expBias;	/* �w���֐��t�B���^�o�C�A�X���x */
	LONG		expTb;		/* �w���֐��t�B���^���萔 */
	LONG		maBuf[ACF_BUF_SZ];/* �ړ����σt�B���^�o�b�t�@ */
	ULONG		maFilTb;	/* �ړ����σt�B���^���萔 */
	ACF_MAFIL	maFil;		/* �ړ����σt�B���^ */
	ACF_IMAFIL	maIFil;		/* �ړ����ϕ�ԃt�B���^ */
	ACF_PRM*	Prm;		/* �p�����[�^ */
	MC_BASE_PRM* bPrm;		/* ��{�p�����[�^ */
} ACFHNDL;

/* forward declarations */
/* Public functions */
#ifdef ACC_FIL_DISABLE
#define AcfInit(hdl, bprm, prm)			(;)
#define AcfGenerate(hdl, spd, sel, den)	(;)
#define AcfClear(hdl)					(;)
#else
extern VOID AcfInit(ACFHNDL* hdl, MC_BASE_PRM* bPrm, ACF_PRM* prm);
extern VOID AcfGenerate(ACFHNDL* hdl, LONG* spd, UCHAR sel, BOOL* den);
extern VOID AcfClear(ACFHNDL* hdl);
#endif

/* ����֐��}�N����` */
#define AcfAttachPRM(hdl, prm)			{(hdl)->Prm = prm;}

#endif /* ACC_FIL_H_ */
