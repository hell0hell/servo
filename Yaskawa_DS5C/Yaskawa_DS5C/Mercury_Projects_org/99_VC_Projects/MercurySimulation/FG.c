/****************************************************************************
Description	: Mercury simulation FG function
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

Functions		:

----------------------------------------------------------------------------
Changes			:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.10.15	created

****************************************************************************/
#include "basedef.h"

#define		SAMPLING_TIME		0.000125	/* ScanB cycle[s] */
#define		PG_PULSE			1048576		/* Encoder resolution[pulse] */

typedef struct _POSING_DATA {
	UCHAR	DenFlg;				/* Den Status */
	LONG	CurrentTime;		/* [cnt/ScanB] */
	LONG	AccTimes;			/* [cnt/ScanB] */
	LONG	DecTimes;			/* [cnt/ScanB] */
	LONG	RestDist;			/* [ref] */
	LONG	CurrentPos;			/* [ref] */
	LONG	CurrentSpeed;		/* [ref/ScanB] */
	LONG	TargetSpeed;		/* [ref/ScanB] */
	LONG	AccRate;			/* [ref/ScanB] */
	LONG	DecRate;			/* [ref/ScanB] */
} POSING_DATA;

static	POSING_DATA		PosingData;


/* Initialize Position Reference */
void	InitPositionReference( void )
{
	PosingData.DenFlg = TRUE;
	PosingData.CurrentTime = 0;
	PosingData.AccTimes = 0;
	PosingData.DecTimes = 0;
	PosingData.RestDist = 0;
	PosingData.CurrentPos = 0;
	PosingData.CurrentSpeed = 0;
	PosingData.TargetSpeed = 0;
	PosingData.AccRate = 0;
	PosingData.DecRate = 0;
}


/* Set Position Reference for Change Reference */
UCHAR	SetPositionReference( LONG Tpos, LONG Tspd, LONG AccTime, LONG DecTime )
{
	UCHAR err;
	err = FALSE;

	if( PosingData.DenFlg != TRUE )
	{
		err = TRUE;
	}
	else
	{
		PosingData.CurrentTime = 0;
		PosingData.TargetSpeed = (LONG)((double)PG_PULSE * Tspd * SAMPLING_TIME / 60.0);
		PosingData.AccTimes = (LONG)((double)AccTime / 1000 / SAMPLING_TIME);
		PosingData.DecTimes = (LONG)((double)DecTime / 1000 / SAMPLING_TIME);
		PosingData.RestDist = Tpos;
		PosingData.AccRate = PosingData.TargetSpeed / PosingData.AccTimes;
		PosingData.DecRate = PosingData.TargetSpeed / PosingData.DecTimes;
	}

	return err;
}


/* Make Current Delta Position Reference */
LONG	MakeCurrentReference( UCHAR *Den )
{
	UCHAR	DecFlg;
	LONG	SpeedRef;
	LONG	RequiredDecDist;
	LONG	lwk;

	/* Check the Termination Condition */
	if( PosingData.RestDist == 0 )
	{
		PosingData.DenFlg = TRUE;
	}
	else
	{
		PosingData.DenFlg = FALSE;
	}

	/* Calculate Required Dec Distance */
	RequiredDecDist = PosingData.CurrentSpeed * PosingData.DecTimes >> 1;
	if( RequiredDecDist > PosingData.RestDist )
	{ /* Require > Reference */
		/* Dec Request */
		DecFlg = TRUE;
	}
	else
	{
		DecFlg = FALSE;
	}

	if( PosingData.DenFlg == TRUE )
	{
		SpeedRef = 0;
	}
	else if( DecFlg == TRUE )
	{ /* Decelerating Request */
		/* Decelerate Operation */
		lwk = PosingData.CurrentSpeed - PosingData.DecRate;
		if( lwk < 0 )
		{ /* The last time */
			PosingData.CurrentSpeed = 0;
			SpeedRef = PosingData.RestDist;
			PosingData.DenFlg = TRUE;
		}
		else
		{
			PosingData.CurrentSpeed = lwk;
			SpeedRef = PosingData.CurrentSpeed;
		}
	}
	else if( PosingData.CurrentSpeed < PosingData.TargetSpeed )
	{
		/* Accelerate Operation */
		lwk = PosingData.CurrentSpeed + PosingData.AccRate;
		if( lwk > PosingData.TargetSpeed )
		{ /* Acc last time */
			PosingData.CurrentSpeed = PosingData.TargetSpeed;
		}
		else
		{
			PosingData.CurrentSpeed = lwk;
		}
		SpeedRef = PosingData.CurrentSpeed;
	}
	else
	{ /* Keep Current Speed */
		SpeedRef = PosingData.CurrentSpeed;
	}

	PosingData.RestDist -= SpeedRef;
	PosingData.CurrentPos += SpeedRef;
	*Den = PosingData.DenFlg;

	return SpeedRef;
}



/***************************************** end of file **********************************************/
