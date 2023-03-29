/****************************************************************************
Description	: Kernel System Library
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
//#include <INTEGRITY.h>
#include "Basedef.h"
#include "KLib.h"
#ifndef _MERCURY_PLATFORM_	/* �Í����t�H�[�}�b�g�Ή� <S146> */
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#endif

/* 2012.08.19 Y.Oka Make Mercury Base Software */
#ifdef _VC_DEBUG_
#include <stdio.h>
#include <time.h>
#endif
/* 2012.08.19 Y.Oka Make Mercury Base Software */

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
#include "nbpfapbtm32.h"
#endif

#define KLIB_EEPDISABLE_TIME		100		/* 100ms */

/* globals */
#pragma arm section rwdata = "Globals", zidata = "Globals"
static LONG		StartUpTimer;
static ULONG	Timer100msCounter;
static ULONG	PowerOnTimer;
static LONG		EepDisableTimer;

static ULONG	KlibScanCounter;
static ULONG	KlibTimerValue;

/* ===>> delete <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0
static ASICS	*pSvAsicRegs;
#endif
/* <<=== delete <TimerAPI> 2013.08.23 K.Sakamoto */
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
//static UCHAR	IntScanBWaitCounter;/* ���g�p */
//static UCHAR	IntScanCWaitCounter;/* ���g�p */
static UCHAR 	IntScanBTimingCounter;
static UCHAR 	IntScanCTimingCounter;
#endif

#pragma arm section rwdata, zidata


/****************************************************************************
	Initialize the Klib variables
****************************************************************************/
void KlibInitialize( void )
{
	ITmr_InitTimer();
}

/****************************************************************************
	Initialize the Time Stamp values
****************************************************************************/
void KlibInitializeTimeStamp( ULONG time )
{
	Timer100msCounter = time;
	PowerOnTimer = 0;
	StartUpTimer = 0;
	EepDisableTimer = KLIB_EEPDISABLE_TIME;
}

/****************************************************************************
	Count up the Time Stamp value(Called from cyclic scan task)
****************************************************************************/
void KlibCountTimeStamp( ULONG tick_in_msec, BOOL PowerOn )
{
	/* �^�C���X�^���v�J�E���^�X�V */
	if(PowerOn != FALSE)		/* Power On���̂ݍX�V */
	{
		PowerOnTimer += tick_in_msec;
		if(PowerOnTimer >= 100)
		{
			Timer100msCounter++;
			PowerOnTimer = 0;
		}
		EepDisableTimer = 0;
	}
	else
	{
		if(EepDisableTimer < KLIB_EEPDISABLE_TIME)
		{	EepDisableTimer += tick_in_msec;}
	}

	/* ���C���^�C�}(����d����������̃^�C�}[ms], Max.1193H/49D)*/
	if( StartUpTimer < 0xFFFF0000 )	
	{
		StartUpTimer += tick_in_msec;
	}
}

/****************************************************************************
	Get a time stamp value
****************************************************************************/
ULONG KlibGetTimeStamp( void )
{
	return Timer100msCounter;
}

/****************************************************************************
	Get a power on time after start up 
****************************************************************************/
LONG KlibGetTimeAfterPowerOn( void )
{
	return StartUpTimer;
}

/****************************************************************************
	Get the power off time state 
****************************************************************************/
LONG KlibGetMainPowerOffTimer( void )
{
	return EepDisableTimer;
}


/****************************************************************************
	[ns]�P�ʃE�F�C�g 
****************************************************************************/
void KlibWaitns( LONG ns )
{
/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
	ULONG	StTime;		/* �J�n����	*/
	ULONG	CuTime;		/* ���ݎ���	*/
	ULONG	Count;		/* �J�E���g�l	*/

	/* �J�n�J�E���g�擾 */
	StTime = ITmr_GetTimerCount();

	/* ���̒l�̎��ɃE�F�C�g�����s���� */
	if( ns > 0 )
	{
		/* �w�莞�Ԃ��J�E���g�l�ɕϊ� */
		Count = ITmr_ConvNsec2count( ns );
		/* ���ԑ҂����� */
		do
		{
			/* ���݃J�E���g�擾 */
			CuTime = ITmr_GetTimerCount();
		} while((CuTime - StTime) < Count);
	}
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

	return;
}

/****************************************************************************
	[��s]�P�ʃE�F�C�g 
****************************************************************************/
void KlibWaitus( LONG us )
{

	ULONG	StTime;		/* �J�n����	*/
	ULONG	CuTime;		/* ���ݎ���	*/
	ULONG	Count;		/* �J�E���g�l	*/
	LONG	TimeUsec;	/* �҂�����	*/

	/* �J�n�J�E���g�擾 */
	StTime = ITmr_GetTimerCount();

	/* ���̒l�̎��ɃE�F�C�g�����s���� */
	if( us > 0 )
	{
		/* �҂����ԕۑ� */
		TimeUsec = us;

		/* �ő厞��(usec)�������҂�����*/
		ITmr_WaitExceedTimeUsec( &TimeUsec, &StTime );

		/* �w�莞�Ԃ��J�E���g�l�ɕϊ� */
		Count = ITmr_ConvUsec2count( TimeUsec );

		/* ���ԑ҂����� */
		do
		{
			/* ���݃J�E���g�擾 */
			CuTime = ITmr_GetTimerCount();

		} while((CuTime - StTime) < Count);
	}

	return;
}

/****************************************************************************
	[ms]�P�ʃE�F�C�g 
****************************************************************************/
void KlibWaitms( LONG ms )
{

	ULONG	StTime;		/* �J�n����	*/
	ULONG	CuTime;		/* ���ݎ���	*/
	ULONG	Count;		/* �J�E���g�l	*/
	LONG	TimeMsec;	/* �҂�����	*/

	/* �J�n�J�E���g�擾 */
	StTime = ITmr_GetTimerCount();

	/* ���̒l�̎��ɃE�F�C�g�����s���� */
	if( ms > 0 )
	{
		/* �҂����ԕۑ� */
		TimeMsec = ms;

		/* �ő厞��(msec)�������҂�����*/
		ITmr_WaitExceedTimeMsec( &TimeMsec, &StTime );

		/* �w�莞�Ԃ��J�E���g�l�ɕϊ� */
		Count = ITmr_ConvMsec2count( TimeMsec );

		/* ���ԑ҂����� */
		do
		{
			/* ���݃J�E���g�擾 */
			CuTime = ITmr_GetTimerCount();

		} while((CuTime - StTime) < Count);
	}
}

/****************************************************************************
	�����O�^�C�}�J�E���^���Z�b�g						<S00E>
****************************************************************************/
void KlibRstLongCounter( ULONG *timer )
{
	*timer = KlibTimerValue;
}

/****************************************************************************
	�����O�^�C�}�l�̎擾 [counter]				<S00E>
****************************************************************************/
ULONG KlibGetLongCounter( ULONG timer )
{
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0	/*<S00B>*//* ��������<S043> */
	return (ULONG)(KlibTimerValue - timer);
#else
	ULONG	CuCount;	/* ���݃J�E���g		*/
	ULONG	Dcount;		/* �J�E���g����		*/

	/* ���݃J�E���g�擾 */
	CuCount = ITmr_GetTimerCount();

	/* �����l�v�Z�@*/
	Dcount = CuCount - timer;

	/* �J�E���^�l��Ԃ� */
	return Dcount;
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	�����O�^�C�}���Z�b�g
****************************************************************************/
void KlibRstLongTimer( ULONG *timer )
{
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0	/*<S00B>*//*�R�����g����<S00E>*/
	*timer = KlibTimerValue;
#else
	*timer = ITmr_GetTimerCount();
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	�����O�^�C�}�l�̎擾 [ms] 					<S00E>
****************************************************************************/
ULONG KlibGetLongTimerMs( ULONG timer )
{
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0	/*<S00B>*//*�R�����g����<S00E>*/
	return (ULONG)(KlibTimerValue - timer);
#else
	ULONG	CuCount;	/* ���݃J�E���g		*/
	ULONG	Dcount;		/* �J�E���g����		*/
	ULONG	TimeMsec;	/* �o�ߎ���(msec)	*/

	/* ���݃J�E���g�擾 */
	CuCount = ITmr_GetTimerCount();

	/* �����l�v�Z�@*/
	Dcount = CuCount - timer;

	/* �J�E���g->����(msec)�ϊ� */
	TimeMsec = ITmr_ConvCount2msec( Dcount );

	/* ����(msec)��Ԃ� */
	return TimeMsec;
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	�����O�^�C�}�l�̎擾 [us]						<S00E>
****************************************************************************/
ULONG KlibGetLongTimerUs( ULONG timer )
{
	ULONG	CuCount;	/* ���݃J�E���g		*/
	ULONG	Dcount;		/* �J�E���g����		*/
	ULONG	TimeUsec;	/* �o�ߎ���(usec)	*/

	/* ���݃J�E���g�擾 */
	CuCount = ITmr_GetTimerCount();

	/* �����l�v�Z�@*/
	Dcount = CuCount - timer;

	/* �J�E���g->����(usec)�ϊ� */
	TimeUsec = ITmr_ConvCount2usec( Dcount );

	/* ����(usec)��Ԃ� */
	return TimeUsec;
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	�����O�^�C�}�l�J�E���g [ms] 
	ScanB 125us�Ȃ�AdivShift=3, 250us�Ȃ�AdivShift=2
****************************************************************************/
void KlibCountLongTimer( ULONG CountInMS )
{
	KlibScanCounter++;
	if(KlibScanCounter >= CountInMS)
	{
		KlibScanCounter = 0;
		KlibTimerValue ++;
	}

}

/****************************************************************************
	�V�X�e���N���b�N�^�C�}�l�擾(32bit Clock Count��1�b)
****************************************************************************/
 ULONG KlibGetClockTime(void)
{
//	Time t;
//	GetClockTime( HighResClock, &t);
//	return t.Fraction;
	ULONG t;
#ifdef _VC_DEBUG_ /* 2012.08.19 Y.Oka Make Mercury Base Software */
	t = clock();
#elif defined(_TEST_BOARD_) /* 2012.08.19 Y.Oka Make Mercury Base Software */
	timer_ioctl( 0, TIMER_IOCTL_GET_COUNT, (VOID *)&t);
#else
	t = 0;
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software */
	return t;
}

/****************************************************************************
	�V�X�e���N���b�N��[ns]�ϊ�
****************************************************************************/
ULONG KlibClockToTime(ULONG clock_diff)
{
//	float time_in_ns;
//	time_in_ns = (float)clock_diff * (float)(1000000000.0/4294967296.0);
//	return (ULONG)time_in_ns; /* [ns] */
	ULONG time_in_ns;
#ifdef _VC_DEBUG_ /* 2012.08.19 Y.Oka Make Mercury Base Software */
	time_in_ns = clock_diff * 1000000;	/* clock_diff[ms] */
#else /* _VC_DEBUG_ */ /* 2012.08.19 Y.Oka Make Mercury Base Software */
//	time_in_ns = clock_diff * 20;	/* 1000/50MHz */
	time_in_ns = clock_diff * 32;	/* 1000/31.25MHz�@SoC�̃^�C�}�ɍ��킹��@<S043> */
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software */
	return (ULONG)time_in_ns; /* [ns] */
}


/****************************************************************************
	ScanB, ScanC ���荞�݃^�C�~���O������
****************************************************************************/
void InitializeInterruptTiming( void )
{
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
//	IntScanBWaitCounter = 0;/* ���g�p */
//	IntScanCWaitCounter = 0;/* ���g�p */
	IntScanBTimingCounter = INT_TIMING_ScanB;
	IntScanCTimingCounter = INT_TIMING_ScanC;
#endif
	return;
}


/****************************************************************************
	���荞�݃^�C�~���O�擾		��ScanA, ScanB����Call
							���߂�l �F TRUE�F���荞�݃^�C�~���O
****************************************************************************/
UCHAR GetInterruptTiming( UCHAR IntName )
{
	UCHAR	ret;
	ret = FALSE;
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)

	if( IntName == INTERRUPT_ScanB )
	{
		if( (IntScanBTimingCounter & INT_TIMING_ScanB) == INT_TIMING_ScanB )
		{
			ret = TRUE;
		}
		IntScanBTimingCounter++;
	}
	else if( IntName == INTERRUPT_ScanC )
	{
		if( (IntScanCTimingCounter & INT_TIMING_ScanC) == INT_TIMING_ScanC )
		{
			ret = TRUE;
		}
		IntScanCTimingCounter++;
	}
	else
	{
		/* DO NOTHING */
	}
#endif
	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		�v���I�Ȉُ펞�̃G���[����																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�@�\�T�v : �����݋֎~�ɂ��ăx�[�X�u���b�N��ԂŃv���O�������~����B						*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	KpiSystemError( void )
{

/*--------------------------------------------------------------------------------------------------*/
/*		�v���O������~																				*/
/*--------------------------------------------------------------------------------------------------*/
	for( ; ; )											/* �v���O������~�������[�v					*/
	{													/*											*/
		;												/*											*/
	}													/*											*/
/*--------------------------------------------------------------------------------------------------*/

}

/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
/****************************************************************************
	�C���^�[�o���E�^�C�}������
****************************************************************************/
void	ITmr_InitTimer( void )
{
	/* �R���y�A�E���W�X�^�ݒ� */
	ITMR_CMD = ITMR_CMD_SET;

	/* �R���g���[���E���W�X�^�ݒ� */
	ITMR_TMCD = ITMR_TMCD_SET;
}

/****************************************************************************
	�^�C�}�E�J�E���^�E���W�X�^�l�擾
****************************************************************************/
ULONG	ITmr_GetTimerCount( void )
{
	ULONG	Count;		/* �J�E���g�l	*/

	/* �^�C�}�E�J�E���^�E���W�X�^�l�擾 */
	Count = ITMR_TMD;

	return Count;
}

/****************************************************************************
	����(nsec)->�J�E���g�ϊ�
****************************************************************************/
ULONG	ITmr_ConvNsec2count( LONG TimeNsec )
{
	ULONG	Count;		/* �J�E���g�l	*/

	/* �J�E���g�l�v�Z */
	Count = (ULONG)TimeNsec / ITMR_NSEC_PER_COUNT;

	return Count;
}

/****************************************************************************
	�ő厞��(usec)������wait
****************************************************************************/
void	ITmr_WaitExceedTimeUsec( LONG *TimeUsec, ULONG *StTimer )
{
	ULONG	CuTimer;	/* ���݃J�E���g�l	*/
	ULONG	Dcount;		/* �J�E���g����	*/
	ULONG	LoopMax;	/* ���[�v��		*/

	/* ���[�v�񐔎擾 */
	LoopMax = (ULONG)(*TimeUsec) >> ITMR_USEC_SHIFT;

	/* �c���Ԃ�ݒ� */
	*TimeUsec = *TimeUsec & ITMR_USEC_MASK;

	/* ���[�v���s */
	for( ; LoopMax > 0; LoopMax-- )
	{
		/* �E�F�C�g���� */
		do
		{
			/* ���݃J�E���g�l�擾 */
			CuTimer = ITmr_GetTimerCount();

			/* �J�E���g�����v�Z */
			Dcount = CuTimer - *StTimer;
		} while( Dcount < ITMR_USEC_FIX_COUNT );

		/* �J�n�J�E���g�X�V */
		*StTimer = *StTimer + ITMR_USEC_FIX_COUNT;
	}
}

/****************************************************************************
	����(usec)->�J�E���g�ϊ�
****************************************************************************/
ULONG	ITmr_ConvUsec2count( LONG TimeUsec )
{
	LONG	WaitTime;	/* �w�莞��	*/
	ULONG	Count;		/* �J�E���g�l	*/

	/* �ϐ������� */
	WaitTime = TimeUsec;

	/* �ő�l�𒴂��Ă��邩�`�F�b�N */
	if( WaitTime > ITMR_USEC_MAX )
	{
		WaitTime = ITMR_USEC_MAX;	/* �����I�ɍő�l		*/
	}

	/* �J�E���g�l�v�Z */
	Count = ((ULONG)WaitTime * ITMR_USEC_MUL_VAL) / ITMR_USEC_DIV_VAL;

	return Count;
}

/****************************************************************************
	�ő厞��(msec)������wait
****************************************************************************/
void	ITmr_WaitExceedTimeMsec( LONG *TimeMsec, ULONG *StTimer )
{
	ULONG	CuTimer;	/* ���݃J�E���g�l	*/
	ULONG	Dcount;		/* �J�E���g����	*/
	ULONG	LoopMax;	/* ���[�v��		*/

	/* ���[�v�񐔎擾 */
	LoopMax = (ULONG)(*TimeMsec) >> ITMR_MSEC_SHIFT;

	/* �c���Ԃ�ݒ� */
	*TimeMsec = *TimeMsec & ITMR_MSEC_MASK;

	/* ���[�v���s */
	for( ; LoopMax > 0; LoopMax-- )
	{
		/* �E�F�C�g���� */
		do
		{
			/* ���݃J�E���g�l�擾 */
			CuTimer = ITmr_GetTimerCount();

			/* �J�E���g�����v�Z */
			Dcount = CuTimer - *StTimer;
		} while( Dcount < ITMR_MSEC_FIX_COUNT );

		/* �J�n�J�E���g�X�V */
		*StTimer = *StTimer + ITMR_MSEC_FIX_COUNT;
	}
}

/****************************************************************************
	����(msec)->�J�E���g�ϊ�
****************************************************************************/
ULONG	ITmr_ConvMsec2count( LONG TimeMsec )
{
	LONG	WaitTime;	/* �w�莞��	*/
	ULONG	Count;		/* �J�E���g�l	*/

	/* �ϐ������� */
	WaitTime = TimeMsec;

	/* �ő�l�𒴂��Ă��邩�`�F�b�N */
	if( WaitTime > ITMR_MSEC_MAX )
	{
		WaitTime = ITMR_MSEC_MAX;	/* �����I�ɍő�l		*/
	}

	/* �J�E���g�l�v�Z */
	Count = (ULONG)WaitTime * ITMR_MSEC_MUL_VAL;

	return Count;
}

/****************************************************************************
	�J�E���g->����(msec)�ϊ�
****************************************************************************/
ULONG	ITmr_ConvCount2msec( ULONG Count )
{
	ULONG	TimeMsec;	/* �o�ߎ���(msec)	*/

	/* �o�ߎ��Ԍv�Z */
	TimeMsec = Count / ITMR_MSEC_MUL_VAL;

	return TimeMsec;
}
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

/****************************************************************************
	�J�E���g->����(usec)�ϊ�
****************************************************************************/
ULONG	ITmr_ConvCount2usec( ULONG Count )
{
	ULONG	TimeUsec;	/* �o�ߎ���(usec)	*/

	/* �o�ߎ��Ԍv�Z */
	TimeUsec = Count / ITMR_USEC_MUL_VAL;

	return TimeUsec;
}


