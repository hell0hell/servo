/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AnlgRefManAdj.c : Analog Reference Offset Manual Adjust (Fn00A,Fn00B) Sample Source			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Execute Analog Reference Auto Adjust (Fn00B)											*/
/*			2)Auto Adjust Mode Display																*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.09.11  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"					/* Kernel API */
#include "LcdOp.h"					/* LCD Operator */
#include "Monitor.h"				/* Un Monitor */
#include "Function.h"				/* Fn Function */
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "PnPrmListTbl.h"	/* <S056> */
#include "FunListDef.h"		/* <S056> */


/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _FnRefManAdjV{
	int		Reserved[(256/4)];	/* Reserved: All functions can use ram variables at least 256 bytes. */
} FNREFMANADJV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#if	( SVFSEL_VREFAD_24BIT != 0 )
#define	ZADJV_MAX			52500000
#define	ZADJV_MIN			-52500000
#define	ZADJV_RATE			3500			/* 52500000÷15000 = 3500	*/
#else
#define	ZADJV_MAX			240000
#define	ZADJV_MIN			-240000
#define	ZADJV_RATE			16				/* 240000÷15000 = 16		*/
#endif
#define	ZADJT_MAX			(127)
#define	ZADJT_MIN			(-127)

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Register Access Function																	*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/* Fn00A: Entry of Analog Velocity Reference Offset Manual Adjustment for Register IF				*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegAnlgVRefManAdjBegin(FUNEXE *FunExe, void *Axis)
{
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/* Fn00A: Execution of Analog Reference Velocity Offset Manual Adjustment for Register IF			*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegAnlgVRefManAdjExe(FUNEXE *FunExe, void *Axis)
{
#if SVFSEL_ADC_USE == 1 /* TODO: ｿｰｽ整理 */
PRM_RSLT rc = PRM_RSLT_CONTINUE;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;
CADC *pAdc = pAxRsc->pAdc;
LONG zadjv_tmp = pAdc->P.ZadjVelo;
/*--------------------------------------------------------------------------------------------------*/
	switch ( FunExe->CmdCode )
	{
	case FCMD_UP:											/* UP,正転,正方向		*/
		zadjv_tmp += ZADJV_RATE;
		if( zadjv_tmp > ZADJV_MAX )
		{
			zadjv_tmp = ZADJV_MAX;
		}
		pAdc->P.ZadjVelo = zadjv_tmp;						/* TODO: パラメータ書き込み処理 */
		break;
	case FCMD_DOWN:											/* DOWN,逆転,逆方向		*/
		zadjv_tmp -= ZADJV_RATE;
		if( zadjv_tmp < ZADJV_MIN )
		{
			zadjv_tmp = ZADJV_MIN;
		}
		pAdc->P.ZadjVelo = zadjv_tmp;						/* TODO: パラメータ書き込み処理 */
		break;
/*--------------------------------------------------------------------------------------------------*/
	case FCMD_EXEC:											/* execution */
	case FCMD_SV:											/* サーボオン/オフ		*/
	case FCMD_CH1:											/* CH1,U相選択			*/
	case FCMD_CH2:											/* CH2,V相選択			*/
	case FCMD_EWRT:											/* EEPROM書込み処理		*/
	case FCMD_MTUP:											/* 操作モード維持(正転)	*/
	case FCMD_MTDOWN:										/* 操作モード維持(逆転)	*/
/*--------------------------------------------------------------------------------------------------*/
		rc = PRM_RSLT_CONDITION_ERR;						/* error */
		break;

/*--------------------------------------------------------------------------------------------------*/
	default:												/* other command */
/*--------------------------------------------------------------------------------------------------*/
		rc = PRM_RSLT_CONTINUE;						/* error */
		break;
	}
	return (rc);
#else
	return PRM_RSLT_SUCCESS;
#endif
}


/****************************************************************************************************/
/*																									*/
/* Fn00A: End of Analog Velocity Reference Offset Manual Adjustment for Register IF					*/
/*																									*/
/****************************************************************************************************/
void RegAnlgVRefManAdjEnd(FUNEXE *FunExe, void *Axis)
{
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
REGIF_CONFIG_T	*RegCfg;							/* <S056> */
//FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;	/* <S056>	Delete:未使用である為 */

/* <S056>	Start */
	RegCfg = pAxRsc->RegManager->hPnReg;

	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );		/* PnE53 : 速度指令ゼロ調 */
/* <S056>	End */

}

/****************************************************************************************************/
/*																									*/
/* Fn00B: Entry of Analog Torque Reference Offset Manual Adjustment for Register IF					*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegAnlgTRefManAdjBegin(FUNEXE *FunExe, void *Axis)
{
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/* Fn00B: Execution of Analog Reference Torque Offset Manual Adjustment for Register IF				*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegAnlgTRefManAdjExe(FUNEXE *FunExe, void *Axis)
{
#if SVFSEL_ADC_USE == 1 /* TODO: ｿｰｽ整理 */
PRM_RSLT rc = PRM_RSLT_CONTINUE;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;
CADC *pAdc = pAxRsc->pAdc;
DBYTEX zadjt_tmp;
/*--------------------------------------------------------------------------------------------------*/
	zadjt_tmp.b.l = (UCHAR)pAdc->P.ZadjTrqP;
	zadjt_tmp.b.h = (UCHAR)pAdc->P.ZadjTrqN;
/*--------------------------------------------------------------------------------------------------*/
	switch ( FunExe->CmdCode )
	{
	case FCMD_UP:											/* UP,正転,正方向		*/
		if( ((CHAR)zadjt_tmp.b.l < ZADJT_MAX) && ((CHAR)zadjt_tmp.b.h < ZADJT_MAX) )
		{
			(CHAR)zadjt_tmp.b.l++;
			(CHAR)zadjt_tmp.b.h++;
		}
		pAdc->P.ZadjTrqP = (CHAR)zadjt_tmp.b.l;				/* TODO: パラメータ書き込み処理 */
		pAdc->P.ZadjTrqN = (CHAR)zadjt_tmp.b.h;				/* TODO: パラメータ書き込み処理 */
		break;
	case FCMD_DOWN:											/* DOWN,逆転,逆方向		*/
		if( ((CHAR)zadjt_tmp.b.l > ZADJT_MIN) && ((CHAR)zadjt_tmp.b.h > ZADJT_MIN) )
		{
			(CHAR)zadjt_tmp.b.l--;
			(CHAR)zadjt_tmp.b.h--;
		}
		pAdc->P.ZadjTrqP = (CHAR)zadjt_tmp.b.l;				/* TODO: パラメータ書き込み処理 */
		pAdc->P.ZadjTrqN = (CHAR)zadjt_tmp.b.h;				/* TODO: パラメータ書き込み処理 */
		break;
/*--------------------------------------------------------------------------------------------------*/
	case FCMD_EXEC:											/* execution */
	case FCMD_SV:											/* サーボオン/オフ		*/
	case FCMD_CH1:											/* CH1,U相選択			*/
	case FCMD_CH2:											/* CH2,V相選択			*/
	case FCMD_EWRT:											/* EEPROM書込み処理		*/
	case FCMD_MTUP:											/* 操作モード維持(正転)	*/
	case FCMD_MTDOWN:										/* 操作モード維持(逆転)	*/
/*--------------------------------------------------------------------------------------------------*/
		rc = PRM_RSLT_CONDITION_ERR;						/* error */
		break;

/*--------------------------------------------------------------------------------------------------*/
	default:												/* other command */
/*--------------------------------------------------------------------------------------------------*/
		rc = PRM_RSLT_CONTINUE;						/* error */
		break;
	}
	return (rc);
#else
	return PRM_RSLT_SUCCESS;
#endif
}

/****************************************************************************************************/
/*																									*/
/* Fn00B: End of Analog Torque Reference Offset Manual Adjustment for Register IF					*/
/*																									*/
/****************************************************************************************************/
void RegAnlgTRefManAdjEnd(FUNEXE *FunExe, void *Axis)
{
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
REGIF_CONFIG_T	*RegCfg;							/* <S056> */
//FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;	/* <S056>	Delete:未使用である為 */

/* <S056>	Start */
	RegCfg = pAxRsc->RegManager->hPnReg;

	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );		/* PnE56 : トルク指令ゼロ調(正入力/負入力) */
/* <S056>	End */
}
