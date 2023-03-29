#ifndef WAIT_FUNC_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		WainFunc.h : �^�C�}/�E�F�C�g�n������`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"

#ifdef _TEST_BOARD_
#define FREERUN_TIMER				0
#define TIMER_CLOCK_PER_1US			25
#endif

#define KLIB_EEPDISABLE_TIME		100		/* 100ms */

/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
/* �C���^�[�o���E�^�C�}���W�X�^ */
#define ITMR_TMD				(*((volatile unsigned long*)(0xEFFF1000)))	/* �^�C�}�E�J�E���^�E���W�X�^		*/
#define ITMR_CMD				(*((volatile unsigned long*)(0xEFFF1004)))	/* �R���y�A�E���W�X�^			*/
#define ITMR_TMCD				(*((volatile unsigned long*)(0xEFFF1008)))	/* �R���g���[���E���W�X�^		*/
/* �ݒ�l */
#define ITMR_CMD_SET			0xFFFFFFFF		/* �R���y�A�E���W�X�^�ݒ�l(0,1,..,0xFFFFFFFE,0xFFFFFFFF,0,1,..�ƂȂ�ݒ�)	*/
#define ITMR_TMCD_SET			0x00000003		/* �R���g���[���E���W�X�^�ݒ�l(INCLOCK��2����,���싖��)						*/
/* nsec�v�Z�p */
#define ITMR_NSEC_PER_COUNT		32UL			/* 1�J�E���g������̎���(nsec)	*/
/* usec�v�Z�p(�ݒ���ꕔ�̂ݏC������ƁA���������삵�Ȃ��̂Œ��ӂ��邱��) */
#define ITMR_USEC_MAX			0x020C49BA		/* wait�ŉ��Z�\�ȍő厞��(usec) 34,359,738 usec	*/
												/* 0x02000000 ���Œ�J�E���g�l�ł̃��[�v�����l�Ƃ���		*/
#define ITMR_USEC_SHIFT			25				/* �Œ�J�E���g�l�ł̃��[�v���K�v�����肷�邽�߂̃V�t�g��	*/
#define ITMR_USEC_MASK			0x01FFFFFF		/* �w�莞�Ԃ��烋�[�v�񐔂���菜�����߂̃}�X�N�p�^�[��		*/
#define ITMR_USEC_FIX_COUNT		0x3E800000		/* �Œ�J�E���g�l((0x02000000 * 125) / 4)			*/
#define ITMR_USEC_MUL_VAL		125UL			/* �搔(�{����1,000�����A1,000/8=125)				*/
#define ITMR_USEC_DIV_VAL		4UL				/* ����(�{����32�����A32/8=4)						*/
/* msec�v�Z�p(�ݒ���ꕔ�̂ݏC������ƁA���������삵�Ȃ��̂Œ��ӂ��邱��) */
#define ITMR_MSEC_MAX			0x000218DE		/* wait�ŉ��Z�\�ȍő厞��(msec) 137,438 msec	*/
												/* 0x00020000 ���Œ�J�E���g�l�ł̃��[�v�����l�Ƃ���		*/
#define ITMR_MSEC_SHIFT			17				/* �Œ�J�E���g�l�ł̃��[�v���K�v�����肷�邽�߂̃V�t�g��	*/
#define ITMR_MSEC_MASK			0x0001FFFF		/* �w�莞�Ԃ��烋�[�v�񐔂���菜�����߂̃}�X�N�p�^�[��		*/
#define ITMR_MSEC_FIX_COUNT		0xF4240000		/* �Œ�J�E���g�l(0x00020000 * 31,250)				*/
#define ITMR_MSEC_MUL_VAL		31250UL			/* �搔(�{����1,000,000 �����A1,000,000/32=31,250)	*/
#define ITMR_MSEC_DIV_VAL		1				/* ����(�{����32�����A32/32=1)						*/
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

enum	INTERRUPT_NAME {
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE	/* AP�ȊO */
	INTERRUPT_IntSync = 1,
//	CoE_SRST = 3,		/*<S008>*/
	OP_SWRST = 3,							/* <S106> */
	SIL3_TEST_PLS	= 4,					/* <S0F1> */
	INTERRUPT_ScanA	= 5 ,
	INTERRUPT_ScanB = 6,
	INTERRUPT_ScanC = 7,
#else
//	DebugPort1		= 1,					/* ��ŏ��� */
//	DebugPort2		= 7,					/* ��ŏ��� */
	INTERRUPT_ScanA	= 1,
	OP_SWRST 		= 3,					/* <S106> */
	SIL3_TEST_PLS	= 4,					/* <S0F1> */
	INTERRUPT_ScanB = 7,
	INTERRUPT_IntSync,						/* DebugPort�͎g�p���Ȃ�����!!!!!!!!! */
	INTERRUPT_ScanC,						/* DebugPort�͎g�p���Ȃ�����!!!!!!!!! */
#endif
};

enum	INTERRUPT_TIMING {
	INT_TIMING_ScanB	= 0x01,
	INT_TIMING_ScanC	= 0x0F,
};


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
void	KlibInitialize( void );								/* Klib������	*/
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

void	KlibWaitns( LONG ns );
void	KlibWaitus( LONG us );
void	KlibWaitms( LONG ms );

void	KlibRstLongCounter( ULONG *timer );			/* �����J�E���^���Z�b�g	<S00E>	*/
ULONG	KlibGetLongCounter( ULONG timer );			/* �J�E���^�v��	<S00E>	*/
void	KlibRstLongTimer( ULONG *timer );			/* �^�C�}�[���Z�b�g	<S00E>	*/
ULONG	KlibGetLongTimerMs( ULONG timer );			/* �^�C�}[ms]�v��	<S00E>	*/
ULONG	KlibGetLongTimerUs( ULONG timer );			/* �^�C�}[ms]�v��	<S00E>	*/
void	KlibCountLongTimer( ULONG divShift );		/* �J�E���^����	<S00E>	*/

void KlibInitializeTimeStamp( ULONG time );
void KlibCountTimeStamp( ULONG tick_in_msec, BOOL PowerOn );
LONG KlibGetTimeAfterPowerOn( void );
ULONG KlibGetTimeStamp( void );
LONG KlibGetMainPowerOffTimer( void );

ULONG KlibClockToTime(ULONG clock_diff);
ULONG KlibGetClockTime(void);

void InitializeInterruptTiming( void );
UCHAR GetInterruptTiming( UCHAR IntName );

/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
void	ITmr_InitTimer( void );										/* �C���^�[�o���E�^�C�}������		*/
ULONG	ITmr_GetTimerCount( void );									/* �^�C�}�E�J�E���^�E���W�X�^�l�擾	*/
ULONG	ITmr_ConvNsec2count( LONG TimeNsec );						/* ����(nsec)->�J�E���g�ϊ�	*/
void	ITmr_WaitExceedTimeUsec( LONG *TimeUsec, ULONG *StTimer );	/* �ő厞��(usec)������wait	*/
ULONG	ITmr_ConvUsec2count( LONG TimeUsec );						/* ����(usec)->�J�E���g�ϊ�	*/
void	ITmr_WaitExceedTimeMsec( LONG *TimeMsec, ULONG *StTimer );	/* �ő厞��(msec)������wait	*/
ULONG	ITmr_ConvMsec2count( LONG TimeMsec );						/* ����(msec)->�J�E���g�ϊ�	*/
ULONG	ITmr_ConvCount2msec( ULONG Count );							/* �J�E���g->����(msec)�ϊ�	*/
ULONG	ITmr_ConvCount2usec( ULONG Count );							/* �J�E���g->����(usec)�ϊ�	*/
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */


/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SystemError.h : �V�X�e���G���[������`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	KpiSystemError( void );

#define WAIT_FUNC_H
#endif /* WAIT_FUNC_H */
/***************************************** end of file **********************************************/
