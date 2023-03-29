/****************************************************************************
  Description   : LED Display 
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-11-18

****************************************************************************/
#ifndef _LED_FLUSH_H_
#define _LED_FLUSH_H_


/* Display pattern */
typedef enum LED_DISP_TYPE {
	LED_ALWAYS_OFF = 0,
	LED_ALWAYS_ON,
	LED_FLICKERING,
	LED_BLINKING,
	LED_SINGL_FLUSH,
	LED_DOUBLE_FLUSH,
}LED_DISP_TYPE;

/* LED control data */
typedef struct LED_STATE_T {
	ULONG			TimerCnt;
	BOOL			LedOut;
	UINT			Step;
	LED_DISP_TYPE	Type;
	LED_DISP_TYPE	CrtType;
} LED_STATE_T;


/****************************************************************************
 LED Flush Service 
****************************************************************************/
void LedFlushMain(AXIS_HANDLE *AxisRsc, LED_STATE_T *LedState, INT ExLedOut);




#endif /* _LED_FLUSH_H_ */
/******************************* End of File *******************************/
