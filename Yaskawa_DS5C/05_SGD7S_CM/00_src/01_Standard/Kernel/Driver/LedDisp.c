/****************************************************************************
  Description   : LED Display 
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-11-18

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "HwDeviceIf.h"
#include "KLib.h"
#include "AlarmManager.h"
#include "LedDisp.h"


typedef struct LED_ONOFF_T {
	UINT	time;
	BOOL	LedOut;
} LED_ONOFF_T;
typedef const LED_ONOFF_T	CLED_ONOFF_T;


CLED_ONOFF_T LED_On_Flush[] = 
{
	{200, TRUE},
};
CLED_ONOFF_T LED_Off_Flush[] = 
{
	{200, FALSE},
};
CLED_ONOFF_T LED_Flickering_Flush[] = 
{
	{50, TRUE}, 
	{50, FALSE},
};
CLED_ONOFF_T LED_Blinking_Flush[] = 
{
	{200, TRUE}, 
	{200, FALSE},
};
CLED_ONOFF_T LED_Single_Flush[] = 
{
	{200, TRUE}, 
	{1000, FALSE},
};
CLED_ONOFF_T LED_Double_Flush[] = 
{
	{200, TRUE}, 
	{200, FALSE}, 
	{200, TRUE}, 
	{1000, FALSE},
};


/****************************************************************************
  Make LED Signal pattern 
****************************************************************************/
static BOOL LedMakeSignal(LED_STATE_T *LedState, CLED_ONOFF_T *LedType, UINT StepNum)
{
	LED_ONOFF_T led_t;
	BOOL		step_change;
	UINT		step;

	step = LedState->Step;
	if(step == 0)
	{
		KlibRstLongTimer(&LedState->TimerCnt);
		step = 1;
	}

	led_t = LedType[step - 1];
	step_change = FALSE;

	if(led_t.time <= KlibGetLongTimerMs(LedState->TimerCnt))
	{
		KlibRstLongTimer(&LedState->TimerCnt);
		step = (step >= StepNum) ? 1 : step + 1;
		step_change = TRUE;
	}

	LedState->Step = step;
	LedState->LedOut = led_t.LedOut;
	return step_change;
}

/****************************************************************************
 Make LED Signal pattern
****************************************************************************/
static BOOL LedMakeLEDSignal(LED_STATE_T *LedState)
{
	BOOL chg;

	switch(LedState->CrtType)
	{
	case LED_ALWAYS_OFF:
		chg = LedMakeSignal(LedState, LED_Off_Flush, 
							sizeof(LED_Off_Flush)/sizeof(LED_ONOFF_T));
		break;
	case LED_ALWAYS_ON:
		chg = LedMakeSignal(LedState, LED_On_Flush, 
							sizeof(LED_On_Flush)/sizeof(LED_ONOFF_T));
		break;
	case LED_BLINKING:
		chg = LedMakeSignal(LedState, LED_Blinking_Flush, 
							sizeof(LED_Blinking_Flush)/sizeof(LED_ONOFF_T));
		break;
	case LED_SINGL_FLUSH:
		chg = LedMakeSignal(LedState, LED_Single_Flush, 
							sizeof(LED_Single_Flush)/sizeof(LED_ONOFF_T));
		break;
	case LED_DOUBLE_FLUSH:
		chg = LedMakeSignal(LedState, LED_Double_Flush, 
							sizeof(LED_Double_Flush)/sizeof(LED_ONOFF_T));
		break;
	default:
		chg = FALSE;
		break;
	}

	if((chg != FALSE) && (LedState->CrtType != LedState->Type))
	{
		LedState->CrtType = LedState->Type;
		LedState->Step = 0;
	}

	return LedState->LedOut;
}

/****************************************************************************
 LED Flush Service 
****************************************************************************/
void LedFlushMain(AXIS_HANDLE *AxisRsc, LED_STATE_T *LedState, INT ExLedOut)
{
	INT			LedOut;
	LONG		ax_no;
	BOOL		ot_sts, hwbb_sts, svon_sts;
	AXIS_HANDLE *Axis;
	ALM_EX_INFO tmpalm; 
	ALM_EX_INFO alminfo={0};
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	BOOL		phase = 0;
#endif

	hwbb_sts = 0;
	ot_sts = 0;
	svon_sts = 0;

	/* Create Led information */
	Axis = AxisRsc;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* Alarm */
		ALMGetExNetStatus(Axis->AlmManager, &tmpalm);
		alminfo.CmdAlm |= tmpalm.CmdAlm;
		alminfo.CommAlm |= tmpalm.CommAlm;
		ALMGetStatus(Axis->AlmManager, &tmpalm.DevAlm, &tmpalm.DevWrn);
		alminfo.DevAlm |= tmpalm.DevAlm;
		alminfo.DevWrn |= tmpalm.DevWrn;

		/* HWBB */
		hwbb_sts |= Axis->SeqCtrlOut->HwbbSts;
		/* OT */
		ot_sts |= Axis->SeqCtrlOut->OverTrvlSts;
		/* SVON */
		svon_sts |= Axis->SeqCtrlOut->BaseEnableReq;

#if 0
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
/* MECHATROLINK Communication phase */
		phase |= Axis->NetAxisIf->com_hdl.Phase;
#endif
#endif
		Axis++;
	}

	/* Make RDY LED flushing type */
	if(hwbb_sts != FALSE)
	{	LedState[0].Type = LED_SINGL_FLUSH;}
	else if(ot_sts != FALSE)
	{	LedState[0].Type = LED_DOUBLE_FLUSH;}
	else if(svon_sts != FALSE)
	{	LedState[0].Type = LED_BLINKING;}
	else
	{	LedState[0].Type = LED_ALWAYS_ON;}

	/* Make Alarm LED flushing type */
	if(alminfo.DevAlm != FALSE)
	{	LedState[1].Type = LED_ALWAYS_ON;}
	else if(alminfo.DevWrn != FALSE)
	{	LedState[1].Type = LED_SINGL_FLUSH;}
	else
	{	LedState[1].Type = LED_ALWAYS_OFF;}

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	/* Make COM State LED flushing type */
	if(phase == 0)
	{	LedState[2].Type = LED_ALWAYS_OFF;}
	else if(phase == 1)
	{	LedState[2].Type = LED_BLINKING;}
	else
	{	LedState[2].Type = LED_ALWAYS_ON;}

/* Make COM Err LED flushing type */
	if(alminfo.CommAlm != FALSE)
	{	LedState[3].Type = LED_ALWAYS_ON;}
	else if(alminfo.CmdAlm != FALSE)
	{	LedState[3].Type = LED_SINGL_FLUSH;}
	else
	{	LedState[3].Type = LED_ALWAYS_OFF;}
#endif
	/* Outputs LED status */
	LedOut = (LedMakeLEDSignal(&LedState[0]) << LED_RDY_BITNO);
	LedOut |= (LedMakeLEDSignal(&LedState[1]) << LED_ERR_BITNO);
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	LedOut |= (LedMakeLEDSignal(&LedState[2]) << LED_CNCT_BITNO);
	LedOut |= (LedMakeLEDSignal(&LedState[3]) << LED_CMERR_BITNO);
#endif

	HALdrv_OutputLED( LedOut | ExLedOut );
}




/******************************* End of File *******************************/
