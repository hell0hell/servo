/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Parameter.h : Parameter Definition Header File												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) 																							*/
/*																									*/
/*		2) 																							*/
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
/*		Rev.1.00 : 2013.4.26  K.Ozaki																*/
/*		Rev.1.10 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___PARAMETER___
#define  ___PARAMETER___
#include "KnlApi.h"
#include "Object.h"

/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* 			ONLY IN MERCURY 																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef COBJDEF CPRMDEF;						/* Parameter Definition */
typedef COBJTBL CPRMTBL;						/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Get Parameter Table Pointer																	*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_PNPRMTBL( obj )		(CPRMTBL *)(((COBJECT *)(obj))->pTbl)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Parameter Definition 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_PNPRMDEF( tbl, idx ) (((CPRMTBL *)(tbl))[(idx)].Attribute)

/*--------------------------------------------------------------------------------------------------*/
/*		Get pRealDef																				*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_pRealDef( pPrmDef )  (pPrmDef)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Parameter Base		 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	GET_PNPRMBASE( pndef )		((pndef)->FormatType) 

/*--------------------------------------------------------------------------------------------------*/
/*		Check the parameter is signed or not	 													*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_PNPRM_SIGNED( pndef )   ( (pndef)->Sign )

/*--------------------------------------------------------------------------------------------------*/
/*		Get the parameter Decimals   																*/
/*--------------------------------------------------------------------------------------------------*/
//#define GET_PNPRM_DECIMALS( pndef ) ( (pndef)->Decimals /* TODO */)
#define GET_PNPRM_DECIMALS( this, pndef, hmsg ) ((this)->PrmGetDecimals( this, pndef, hmsg ))	/* <S14F> */

/*--------------------------------------------------------------------------------------------------*/
/*		Get the parameter Upper Limit  																*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_PNPRM_UPPERLMT( pndef ) ((pndef)->UpperLimit)

/*--------------------------------------------------------------------------------------------------*/
/*		Get the parameter Lower Limit  																*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_PNPRM_LOWERLMT( pndef ) ((pndef)->LowerLimit)

/*--------------------------------------------------------------------------------------------------*/
/*		Get the parameter operator input adjust <S149>  											*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_PNPRM_ADJOPE( pndef ) ((pndef)->RegNumber == (0x212))	/* only pn212 */




/****************************************************************************************************/
/*																									*/
/*		Parameter Access Class Definition															*/
/*																									*/
/****************************************************************************************************/
typedef struct _CParameter{					/* Parameter Access Class Definiton						*/
	USHORT	PrmTblEntNum;					/* Un Monitor Table Entry Number(Protected)				*/
	USHORT  Reserved;						/* Reserved(Protected)									*/
/*--------------------------------------------------------------------------------------------------*/
	CPRMTBL *pPrmTbl;						/* Pointer to the Parameter Table(Protected)			*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>	LONG	(*Read)(  				/* Read Method ( by using DefPtr ) 						*/
//				COBJECT *this,				/* this Pointer 										*/
//				COBJDEF *pPrmDef,  			/* Pointer to the Parameter Defitnion 					*/
//				ULONG *pResData,			/* Pointer to the Read Value 							*/
//				ULONG CheckType,			/* Error Check Type 									*/
//				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Read)(						/* <S073> Read Method ( by using Index ) 				*/
				COBJECT *this,				/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadS)(						/* <S073> Read Method ( by using Index ) 				*/
				COBJECT *this,				/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				LONG SubIndex,				/* Monitor Table Sub Index 								*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Search)(						/* Index Search Method									*/
				COBJECT *this,				/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg );			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchB)(						/* Index Search Method ( by using Base Index ) 			*/
				COBJECT *this,				/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadArray)(					/* Read Array Object ( by using Index ) 				*/
				COBJECT *this,				/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Write)(						/* Write a Parameter in RAM 							*/
				struct _CParameter  *this,	/* this Pointer											*/
				LONG 	Index,				/* Pointer to the PrmDef								*/
				ULONG	Data,				/* Parameter Value 										*/
				HMSGIF	Hmsg );				/* Message Interface									*/
				//ULONG	FnPnChk			);	/* 補助機能実行中規制チェック有無フラグ	@@@見直し要@@@	*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteS)(						/* <S073> Write a Parameter in RAM 						*/
				struct _CParameter  *this,	/* this Pointer											*/
				LONG 	Index,				/* Pointer to the PrmDef								*/
				LONG 	SubIndex,			/* Pointer to the PrmDef								*/
				ULONG	Data,				/* Parameter Value 										*/
				HMSGIF	Hmsg );				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*GetNumber)(					/* Get Object Number from Object Index					*/
				struct _CParameter *this,	/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*isLong)(						/* Check Object is Long or Not 							*/
				struct _CParameter *this,	/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchS)(						/* <S073> Index Search witch Sub Index					*/
				struct _CParameter *this,	/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				LONG	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchBS)(					/* <S073> Next Index Search Method 					 	*/
				struct _CParameter *this,	/* this Pointer 										*/
				LONG 	SrchNo,				/* Number of Un Monitor 								*/
				LONG 	BaseIdx,			/* Start Index of Searching 							*/
				LONG 	BaseSubIdx,			/* Start Sub Index of Searching 						*/
				LONG 	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				MSGIF 	*hMsg );			/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteArray)(					/* <S073> Write Array Data Method 						*/
				struct _CParameter *this,	/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG WriteSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*DigitInc)(					/* Increment Parameter Digit in RAM						*/
				struct _CParameter  *this,	/* this Pointer											*/
				LONG	Index,				/* Parameter Index										*/
				ULONG	Digit,				/* Digit(0~)											*/
				MSGIF  *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*DigitDec)(					/* Decrement Parameter Digit in RAM						*/
				struct _CParameter  *this,	/* this Pointer											*/
				LONG	Index,				/* Parameter Index										*/
				ULONG	Digit,				/* Digit(0~)											*/
				MSGIF  *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	(*PrmGetDecimals)(				/* <S14F>												*/
				struct _CParameter  *this,	/* this Pointer											*/
				CPRMDEF	*prmdef,			/* パラメータ定義へのポインタ							*/
				MSGIF	*hMsg );			/* メッセージハンドル									*/
/*--------------------------------------------------------------------------------------------------*/
} CPARAM;

/*--------------------------------------------------------------------------------------------------*/
#define CPARAM_ENT_NUM (1)					/* Parameter Instance Entry Number 						*/
extern CPARAM CParam[];						/* Parameter Accesser Instance 							*/
extern const PRMTBL PnPrmTbl[];				/* Parameter Table 										*/
extern const ULONG PnPrmTblEntNum;			/* Parameter Table Entry Number 						*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMTBL_ENTNUM   (PnPrmTblEntNum)
#define PRMTBL_PTR      (PnPrmTbl)
/*--------------------------------------------------------------------------------------------------*/
#define CPARAMX (&CParam[0])				/* Servo Parameter Accesser Class 						*/
#define CPARAMY (&CParam[1])				/* Option Parameter Accesser Class 						*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Parameter Manager Definition (<-RndRpx.h)													*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Parameter Object Return Code																*/
/*--------------------------------------------------------------------------------------------------*/
#define	PERR_NO					0x0001
#define	PERR_DFIT				0x0002
#define	PERR_SIZE				0x0003
#define	PERR_LEVEL				0x0004
#define	PERR_RONLY				0x0005
#define	PERR_MTR_TYPE			0x0006
#define	PERR_AMP_TYPE			0x0007
#define	PERR_CALC				0x0008
#define	PERR_RANGE				0x0009
#define	PERR_FOUND				0x000A
#define PERR_INH				0x000B
#define PERR_PRMCOPY			0x000C
#define PERR_WRITE_HANDLE		0x000D
#define	PERR_OPEDSP				0x000E
#define	PRM_EEPROM_RAM_NO_DIFF	0x0020
#define	PRM_EEPROM_WRITE_WAIT	0x0021
#define	PRM_EEPROM_WRITE_FAIL	0x0022

/*--------------------------------------------------------------------------------------------------*/
/*		Parameter Check Flag																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	PCHK_NONE				0x00
#define	PCHK_ACCS_LVL			0x01
#define PCHK_INHIBIT			0x04
#define	PCHK_READONLY			0x08
#define	PCHK_SP_USE				0x10
#define	PCHK_OPEDSP				0x20
/*--------------------------------------------------------------------------------------------------*/
#define PCHK_REG_READ			PCHK_NONE
#define PCHK_REG_WRITE			PCHK_ACCS_LVL | PCHK_INHIBIT | PCHK_READONLY
/*--------------------------------------------------------------------------------------------------*/
#define PCHK_OPE_READ			PCHK_ACCS_LVL | PCHK_SP_USE
#define PCHK_OPE_WRITE			PCHK_ACCS_LVL | PCHK_INHIBIT | PCHK_READONLY | PCHK_SP_USE

/*--------------------------------------------------------------------------------------------------*/
/*		Operator Parameter Write Modes																*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPWM_None				0					/*												*/
#define	OPWM_UpdtWait			1					/*												*/
#define	OPWM_UpdtBlnk			2					/* LCD ONLY										*/
#define	OPWM_InfWrWait			3					/* LCD ONLY										*/

#endif
/***************************************** end of file **********************************************/
