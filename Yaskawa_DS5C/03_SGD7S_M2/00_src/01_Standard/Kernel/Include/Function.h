/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Function.h : Function Management Module														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Function Data Type Definition															*/
/*																									*/
/*		2) Function Resource Macro Definition														*/
/*																									*/
/*		3) 																							*/
/*																									*/
/*		4) 																							*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.5.08  K.Ozaki																*/
/*		Rev.1.10 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___FUNCTION___
#define  ___FUNCTION___
#include "KnlApi.h"
#include "Object.h"
#include "FunListDef.h"

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* 			ONLY IN MERCURY 																		*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Get Function Table Pointer																	*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_FUNTBL( p )             (CFUNTBL *)((p)->pFunTbl)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_FUNDEF( p, idx )		(((CFUNTBL *)(p))[(idx)].FunDefAttr)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Access Level																		*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_FNACCLVL( fndef )		((fndef)->AccessLevel)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Name 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_FNFUNNAME( fndef )		((fndef)->SName)



/****************************************************************************************************/
/*																									*/
/*		Function Access Class Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�⏕�@�\�̎��s�\����																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _CFunction{					/* Function Access Class Definition						*/
	USHORT	FnTblEntNum;					/* Function Table Entry Number							*/
	USHORT  Reserved;						/* Reserved 											*/
/*--------------------------------------------------------------------------------------------------*/
	CFUNTBL	*pFunTbl;						/* Pointer to the Function Table						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Read)(  						/* Read Method ( by using DefPtr ) 						*/
				struct _CObject *this,		/* this Pointer 										*/
				COBJDEF *pObjDef,  			/* Pointer to the Parameter Defitnion 					*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadX)(						/* Read Method ( by using Index ) 						*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Search)(						/* Index Search Method									*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchB)(						/* Index Search Method ( by using Base Index ) 			*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadArray)(					/* Read Array Object ( by using Index ) 				*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Write)(  						/* Write Method 										*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				ULONG Data,					/* Write Value 											*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*GetNumber)(					/* Get Object Number from Object Index					*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*isLong)(						/* Check Object Size is Long or Not						*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchS)(						/* <S073> Index Search witch Sub Index 					*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				LONG	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchBS)(					/* <S073> Next Index Search Method 						*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG 	SrchNo,				/* Number of Un Monitor 								*/
				LONG 	BaseIdx,			/* Start Index of Searching 							*/
				LONG 	BaseSubIdx,			/* Start Sub Index of Searching 						*/
				LONG 	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				MSGIF 	*hMsg );			/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteArray)(					/* <S073> Write Array Data Method 						*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG WriteSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
/*	extended																						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Next)(						/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Prev)(						/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Begin)(						/* Begin Function										*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG	Index,				/* �⏕�@�\�̊Ǘ��e�[�u���C���f�b�N�X					*/
				UINT 	*FLcdKeyMk,			/* LCD�L�[MAKE(OFF->ON)�t���O							*/
				UINT 	*FLcdKeyBrk,		/* LCD�L�[BREAK(ON->OFF)�t���O							*/
				UINT 	*FLcdKeyLvl,		/* LCD�L�[���x���t���O									*/
				UINT 	*FOpBlnkKickFlg,	/* LCD�_�ŗp�t���O										*/
				UCHAR	*FTxBuff,			/* LCD�I�y���[�^�\�����								*/
				HMSGIF	Hmsg );				/* �g�p�����ʃn���h��									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Exec)(						/* Execute Function										*/
				struct _CFunction *this,	/* this Pointer 										*/
				HMSGIF Hmsg );				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	void	(*Finish)(						/* Finish Function										*/
				struct _CFunction *this,	/* this Pointer 										*/
				HMSGIF Hmsg );				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
} CFUNCTION;

/*--------------------------------------------------------------------------------------------------*/
#define CFUN_ENT_NUM (1)					/* Function Instance Entry Number 						*/
extern	CFUNCTION CFunction[];				/* Function Access Class Instance						*/
/*--------------------------------------------------------------------------------------------------*/
#define FNFUNTBL_ENTNUM   (FnFunTblEntNum)
#define FNFUNTBL_PTR      (FnFunTbl)
/*--------------------------------------------------------------------------------------------------*/
#define CLCDFUN (&CFunction[0])				/* LCD Operator Function Access Class 					*/
#define CPNLFUN (CLCDFUN)					/* Panel Operator Function Access Class					*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\�Ǘ��h�e��`(RndRpx.h)																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Servo Function Check Flag																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	FCHK_NONE			0x00
#define	FCHK_ACCS_LVL		0x01
#define	FCHK_AMP_TYPE		0x02
#define	FCHK_PWINH			0x04
#define	FCHK_MTR_TYPE		0x08
#define	FCHK_ALL			0xFF
#define	FCHK_DEF			FCHK_MTR_TYPE | FCHK_ACCS_LVL
/*--------------------------------------------------------------------------------------------------*/
/*		Servo Function Object Return Code															*/
/*--------------------------------------------------------------------------------------------------*/
#define	FERR_FOUND			0x0001
#define	FERR_FULL			0x0002
#define	FERR_DATA			0x0003
#define	FERR_LEVEL			0x0004
#define	FERR_AMP_TYPE		0x0005
#define	FERR_INHIBIT		0x0006
#define	FERR_MTR_TYPE		0x0007

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitFunManager( void );			/* �⏕�@�\�Ǘ�����������								*/
void	RpxPrstFunManager( void );			/* �⏕�@�\�Ǘ��p�����[�^���Z�b�g����					*/

/****************************************************************************************************/
/*																									*/
/*		�e���⏕�@�\�Ǘ��`�o�h��`(RndRpx.h)														*/
/*																									*/
/****************************************************************************************************/
#define FEXE_LCDBUF_SIZE	114					/* LcdDspBuff[] Size Definition						*/

/*--------------------------------------------------------------------------------------------------*/
/*	Operator Application Execution State 															*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEAPP_INIT		  0						/* ���s�O											*/
#define OPEAPP_EXEC		  1						/* ���s��											*/
#define OPEAPP_END		  2						/* ���s�I��											*/

/*--------------------------------------------------------------------------------------------------*/
/*	Servo Function Reference Data																	*/
/*--------------------------------------------------------------------------------------------------*/
#define FREFER_INIT		  0						/* ���s�O											*/
#define FREFER_EXEC		  1						/* ���s��											*/
#define FREFER_END		  2						/* ���s�I��											*/

/*--------------------------------------------------------------------------------------------------*/
/*	Servo Function Command Code																		*/
/*--------------------------------------------------------------------------------------------------*/
//#define FCMD_NONE		0x00					/* ���߂Ȃ�											*/
#define FCMD_NONE		0xFF					/* ���߂Ȃ�											*//* <S14E> */
#define FCMD_EXEC		0x01					/* ���s												*/
#define FCMD_UP			0x02					/* UP,���],������									*/
#define FCMD_DOWN		0x03					/* DOWN,�t�],�t����									*/
#define FCMD_SV			0x04					/* �T�[�{�I��/�I�t									*/
#define FCMD_CH1		0x05					/* CH1,U���I��										*/
#define FCMD_CH2		0x06					/* CH2,V���I��										*/
#define FCMD_EWRT		0x07					/* EEPROM�����ݏ���									*/
//#define FCMD_STOP		0x08					/* ��~												*/
#define FCMD_STOP		0x00					/* ��~												*//* <S14E> */
#define FCMD_PMODE		0x09					/* LCD MODE/SET����									*/
#define FCMD_LMODE		0x0A					/* �p�l�� MODE/SET����								*/
#define FCMD_PDATA		0x0B					/* LCD DATA����										*/
#define FCMD_LDATA		0x0C					/* �p�l�� DATA����									*/
#define FCMD_RSET		0x0D					/* ���Z�b�g											*/
#define FCMD_PSHUP		0x0E					/* �p�l�� �V�t�g�A�b�v								*/
#define FCMD_LSHUP		0x0F					/* LCD �V�t�g�A�b�v									*/
#define FCMD_SHDOWN		0x10					/* �V�t�g�_�E��										*/
#define FCMD_ALMRST		0x11					/* �A���[�����Z�b�g									*/
#define FCMD_MTUP		0x12					/* ���샂�[�h�ێ�(���])								*/
#define FCMD_MTDOWN		0x13					/* ���샂�[�h�ێ�(�t�])								*/
#define FCMD_SCROLL		0x14					/* �X�N���[��										*/
#define FCMD_READ		0x15					/* READ												*/
#define FCMD_WRITE		0x16					/* WRITE											*/
#define FCMD_SGNONE		0x17					/* SigmaMsg�R�}���h�Ȃ�								*/
//#define FCMD_EXEC2		0x18					/* �⏕���s											*/
#define FCMD_EXEC2		0x08					/* �⏕���s											*//* <S14E> */
//#define FCMD_MTEXEC		0x19					/* ���샂�[�h�ێ�(���s)								*/
#define FCMD_MTEXEC		0x11					/* ���샂�[�h�ێ�(���s)								*//* <S14E> */
#define FCMD_HIWND		0x1A					/* ���������I��										*/
#define FCMD_LOWND		0x1B					/* �ᑬ�����I��										*/
#if 0 /* Message�ʐM�Ƃ̒�`���ʉ�(���얢�m�F�̂��߁A�R�����g�A�E�g) */
//#define FCMD_STOP		0x08					/* ��~												*/
#define FCMD_STOP		0x00					/* ��~												*/
#define FCMD_EXEC		0x01					/* ���s												*/
#define FCMD_UP			0x02					/* UP,���],������									*/
#define FCMD_DOWN		0x03					/* DOWN,�t�],�t����									*/
#define FCMD_SV			0x04					/* �T�[�{�I��/�I�t									*/
#define FCMD_CH1		0x05					/* CH1,U���I��										*/
#define FCMD_CH2		0x06					/* CH2,V���I��										*/
#define FCMD_EWRT		0x07					/* EEPROM�����ݏ���									*/
//#define FCMD_EXEC2	0x18					/* �⏕���s											*/
#define FCMD_EXEC2		0x08					/* �⏕���s											*/
#define FCMD_PMODE		0x09					/* LCD MODE/SET����									*/
#define FCMD_LMODE		0x0A					/* �p�l�� MODE/SET����								*/
#define FCMD_PDATA		0x0B					/* LCD DATA����										*/
#define FCMD_LDATA		0x0C					/* �p�l�� DATA����									*/
#define FCMD_RSET		0x0D					/* ���Z�b�g											*/
#define FCMD_PSHUP		0x0E					/* �p�l�� �V�t�g�A�b�v								*/
#define FCMD_LSHUP		0x0F					/* LCD �V�t�g�A�b�v									*/
#define FCMD_SHDOWN		0x10					/* �V�t�g�_�E��										*/
//#define FCMD_MTEXEC	0x19					/* ���샂�[�h�ێ�(���s)								*/
#define FCMD_MTEXEC		0x11					/* ���샂�[�h�ێ�(���s)								*/
#define FCMD_MTUP		0x12					/* ���샂�[�h�ێ�(���])								*/
#define FCMD_MTDOWN		0x13					/* ���샂�[�h�ێ�(�t�])								*/
#define FCMD_SCROLL		0x14					/* �X�N���[��										*/
#define FCMD_READ		0x15					/* READ												*/
#define FCMD_WRITE		0x16					/* WRITE											*/
#define FCMD_SGNONE		0x17					/* SigmaMsg�R�}���h�Ȃ�								*/
//#define FCMD_ALMRST	0x11					/* �A���[�����Z�b�g									*/
#define FCMD_ALMRST		0x18					/* �A���[�����Z�b�g									*/
#define FCMD_HIWND		0x1A					/* ���������I��										*/
#define FCMD_LOWND		0x1B					/* �ᑬ�����I��										*/
//#define FCMD_NONE		0x00					/* ���߂Ȃ�											*/
#define FCMD_NONE		0xFF					/* ���߂Ȃ�											*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*	Servo Function End Method																		*/
/*--------------------------------------------------------------------------------------------------*/
#define FNORMAL_END		0x00
#define FFORCE_END		0x01

/*--------------------------------------------------------------------------------------------------*/
/*		�f�[�^�����݋֎~IF��`																		*/
/*		���ӁF�f�[�^�����݊Ǘ��̍\����(RNDWRTD)�����o�ɑΉ��������т���K�v�L��						*/
/*--------------------------------------------------------------------------------------------------*/
#define FPRMOPE_IF		0x01					/* �I�y���[�^������IF					[0bit��]	*/
#define FPRMREG_IF		0x02					/* �p�����[�^���W�X�^������IF			[1bit��]	*/
#define FOPEREG_IF		0x04					/* ��ʑ��샌�W�X�^������IF				[2bit��]	*/
												/* (2000H, 2001H, 2002H �͏���)						*/
#define FALLOPEREG_IF	0x08					/* �S���샌�W�X�^������IF				[3bit��]	*/
#define FENGREG_IF		0x10					/* �G���W�j�A�����O���W�X�^������IF		[4bit��]	*/
#define FTRCREG_IF		0x20					/* �g���[�X�f�[�^���W�X�^������IF		[5bit��]	*/
#define FSPRMREG_IF		0x40					/* ����p�����[�^���W�X�^������IF		[6bit��]	*/

/*--------------------------------------------------------------------------------------------------*/
/*		FPRM_IF			���[�U�p�����[�^������IF													*/
/*		FEXCEPT_FN_IF	���W�X�^	2000H, 2001H, 2002H �ȊO�̏�����IF								*/
/*		FALL_IF			�S�Ẵf�[�^������IF														*/
/*--------------------------------------------------------------------------------------------------*/
#define FPRM_IF			FPRMOPE_IF | FPRMREG_IF | FSPRMREG_IF
#define FEXCEPT_FN_IF	FPRM_IF	| FOPEREG_IF | FENGREG_IF | FTRCREG_IF
#define FALL_IF			FEXCEPT_FN_IF | FALLOPEREG_IF

#endif
/***************************************** end of file **********************************************/
