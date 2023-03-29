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
/*		補助機能の実行構造体																		*/
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
				LONG	Index,				/* 補助機能の管理テーブルインデックス					*/
				UINT 	*FLcdKeyMk,			/* LCDキーMAKE(OFF->ON)フラグ							*/
				UINT 	*FLcdKeyBrk,		/* LCDキーBREAK(ON->OFF)フラグ							*/
				UINT 	*FLcdKeyLvl,		/* LCDキーレベルフラグ									*/
				UINT 	*FOpBlnkKickFlg,	/* LCD点滅用フラグ										*/
				UCHAR	*FTxBuff,			/* LCDオペレータ表示情報								*/
				HMSGIF	Hmsg );				/* 使用元識別ハンドル									*/
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
/*		補助機能管理ＩＦ定義(RndRpx.h)																*/
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
void	RpxInitFunManager( void );			/* 補助機能管理初期化処理								*/
void	RpxPrstFunManager( void );			/* 補助機能管理パラメータリセット処理					*/

/****************************************************************************************************/
/*																									*/
/*		Ｆｎ補助機能管理ＡＰＩ定義(RndRpx.h)														*/
/*																									*/
/****************************************************************************************************/
#define FEXE_LCDBUF_SIZE	114					/* LcdDspBuff[] Size Definition						*/

/*--------------------------------------------------------------------------------------------------*/
/*	Operator Application Execution State 															*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEAPP_INIT		  0						/* 実行前											*/
#define OPEAPP_EXEC		  1						/* 実行中											*/
#define OPEAPP_END		  2						/* 実行終了											*/

/*--------------------------------------------------------------------------------------------------*/
/*	Servo Function Reference Data																	*/
/*--------------------------------------------------------------------------------------------------*/
#define FREFER_INIT		  0						/* 実行前											*/
#define FREFER_EXEC		  1						/* 実行中											*/
#define FREFER_END		  2						/* 実行終了											*/

/*--------------------------------------------------------------------------------------------------*/
/*	Servo Function Command Code																		*/
/*--------------------------------------------------------------------------------------------------*/
//#define FCMD_NONE		0x00					/* 命令なし											*/
#define FCMD_NONE		0xFF					/* 命令なし											*//* <S14E> */
#define FCMD_EXEC		0x01					/* 実行												*/
#define FCMD_UP			0x02					/* UP,正転,正方向									*/
#define FCMD_DOWN		0x03					/* DOWN,逆転,逆方向									*/
#define FCMD_SV			0x04					/* サーボオン/オフ									*/
#define FCMD_CH1		0x05					/* CH1,U相選択										*/
#define FCMD_CH2		0x06					/* CH2,V相選択										*/
#define FCMD_EWRT		0x07					/* EEPROM書込み処理									*/
//#define FCMD_STOP		0x08					/* 停止												*/
#define FCMD_STOP		0x00					/* 停止												*//* <S14E> */
#define FCMD_PMODE		0x09					/* LCD MODE/SET押下									*/
#define FCMD_LMODE		0x0A					/* パネル MODE/SET押下								*/
#define FCMD_PDATA		0x0B					/* LCD DATA押下										*/
#define FCMD_LDATA		0x0C					/* パネル DATA押下									*/
#define FCMD_RSET		0x0D					/* リセット											*/
#define FCMD_PSHUP		0x0E					/* パネル シフトアップ								*/
#define FCMD_LSHUP		0x0F					/* LCD シフトアップ									*/
#define FCMD_SHDOWN		0x10					/* シフトダウン										*/
#define FCMD_ALMRST		0x11					/* アラームリセット									*/
#define FCMD_MTUP		0x12					/* 操作モード維持(正転)								*/
#define FCMD_MTDOWN		0x13					/* 操作モード維持(逆転)								*/
#define FCMD_SCROLL		0x14					/* スクロール										*/
#define FCMD_READ		0x15					/* READ												*/
#define FCMD_WRITE		0x16					/* WRITE											*/
#define FCMD_SGNONE		0x17					/* SigmaMsgコマンドなし								*/
//#define FCMD_EXEC2		0x18					/* 補助実行											*/
#define FCMD_EXEC2		0x08					/* 補助実行											*//* <S14E> */
//#define FCMD_MTEXEC		0x19					/* 操作モード維持(実行)								*/
#define FCMD_MTEXEC		0x11					/* 操作モード維持(実行)								*//* <S14E> */
#define FCMD_HIWND		0x1A					/* 高速巻線選択										*/
#define FCMD_LOWND		0x1B					/* 低速巻線選択										*/
#if 0 /* Message通信との定義共通化(動作未確認のため、コメントアウト) */
//#define FCMD_STOP		0x08					/* 停止												*/
#define FCMD_STOP		0x00					/* 停止												*/
#define FCMD_EXEC		0x01					/* 実行												*/
#define FCMD_UP			0x02					/* UP,正転,正方向									*/
#define FCMD_DOWN		0x03					/* DOWN,逆転,逆方向									*/
#define FCMD_SV			0x04					/* サーボオン/オフ									*/
#define FCMD_CH1		0x05					/* CH1,U相選択										*/
#define FCMD_CH2		0x06					/* CH2,V相選択										*/
#define FCMD_EWRT		0x07					/* EEPROM書込み処理									*/
//#define FCMD_EXEC2	0x18					/* 補助実行											*/
#define FCMD_EXEC2		0x08					/* 補助実行											*/
#define FCMD_PMODE		0x09					/* LCD MODE/SET押下									*/
#define FCMD_LMODE		0x0A					/* パネル MODE/SET押下								*/
#define FCMD_PDATA		0x0B					/* LCD DATA押下										*/
#define FCMD_LDATA		0x0C					/* パネル DATA押下									*/
#define FCMD_RSET		0x0D					/* リセット											*/
#define FCMD_PSHUP		0x0E					/* パネル シフトアップ								*/
#define FCMD_LSHUP		0x0F					/* LCD シフトアップ									*/
#define FCMD_SHDOWN		0x10					/* シフトダウン										*/
//#define FCMD_MTEXEC	0x19					/* 操作モード維持(実行)								*/
#define FCMD_MTEXEC		0x11					/* 操作モード維持(実行)								*/
#define FCMD_MTUP		0x12					/* 操作モード維持(正転)								*/
#define FCMD_MTDOWN		0x13					/* 操作モード維持(逆転)								*/
#define FCMD_SCROLL		0x14					/* スクロール										*/
#define FCMD_READ		0x15					/* READ												*/
#define FCMD_WRITE		0x16					/* WRITE											*/
#define FCMD_SGNONE		0x17					/* SigmaMsgコマンドなし								*/
//#define FCMD_ALMRST	0x11					/* アラームリセット									*/
#define FCMD_ALMRST		0x18					/* アラームリセット									*/
#define FCMD_HIWND		0x1A					/* 高速巻線選択										*/
#define FCMD_LOWND		0x1B					/* 低速巻線選択										*/
//#define FCMD_NONE		0x00					/* 命令なし											*/
#define FCMD_NONE		0xFF					/* 命令なし											*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*	Servo Function End Method																		*/
/*--------------------------------------------------------------------------------------------------*/
#define FNORMAL_END		0x00
#define FFORCE_END		0x01

/*--------------------------------------------------------------------------------------------------*/
/*		データ書込み禁止IF定義																		*/
/*		注意：データ書込み管理の構造体(RNDWRTD)メンバに対応した並びする必要有り						*/
/*--------------------------------------------------------------------------------------------------*/
#define FPRMOPE_IF		0x01					/* オペレータ書込みIF					[0bit目]	*/
#define FPRMREG_IF		0x02					/* パラメータレジスタ書込みIF			[1bit目]	*/
#define FOPEREG_IF		0x04					/* 一般操作レジスタ書込みIF				[2bit目]	*/
												/* (2000H, 2001H, 2002H は除く)						*/
#define FALLOPEREG_IF	0x08					/* 全操作レジスタ書込みIF				[3bit目]	*/
#define FENGREG_IF		0x10					/* エンジニアリングレジスタ書込みIF		[4bit目]	*/
#define FTRCREG_IF		0x20					/* トレースデータレジスタ書込みIF		[5bit目]	*/
#define FSPRMREG_IF		0x40					/* 特殊パラメータレジスタ書込みIF		[6bit目]	*/

/*--------------------------------------------------------------------------------------------------*/
/*		FPRM_IF			ユーザパラメータ書込みIF													*/
/*		FEXCEPT_FN_IF	レジスタ	2000H, 2001H, 2002H 以外の書込みIF								*/
/*		FALL_IF			全てのデータ書込みIF														*/
/*--------------------------------------------------------------------------------------------------*/
#define FPRM_IF			FPRMOPE_IF | FPRMREG_IF | FSPRMREG_IF
#define FEXCEPT_FN_IF	FPRM_IF	| FOPEREG_IF | FENGREG_IF | FTRCREG_IF
#define FALL_IF			FEXCEPT_FN_IF | FALLOPEREG_IF

#endif
/***************************************** end of file **********************************************/
