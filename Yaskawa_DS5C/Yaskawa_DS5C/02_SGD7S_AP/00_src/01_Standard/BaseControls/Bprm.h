/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Bprm.h : �T�[�{����x�[�X�p�����[�^�\���̒�`												*/
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
#ifndef	_BPRM_H_
#define	_BPRM_H_


#include "alarm.h"
#include "EncData.h"

/*---------------------------------------------------------------------------*/
/*		Un Monitor Average time												 */
/*---------------------------------------------------------------------------*/
#define SVCDEF_UNSPD_SCMEANNUM	((64 + KPI_SCCYCLEMS - 1) / KPI_SCCYCLEMS)
#define SVCDEF_UNSPD_SCMEANTMMS	( KPI_SCCYCLEMS * SVCDEF_UNSPD_SCMEANNUM )

/*---------------------------------------------------------------------------*/
/*		Normalize Multiplier Factor											 */
/*---------------------------------------------------------------------------*/
#define NORM_TRQ_TO_IQ			3840000		/* TrqRef(2^24) --> IqRef(15000) */
#define NORM_IQ_TO_TRQ			73300775	/* IqRef(15000) --> TrqRef(2^24) */

/****************************************************************************************************/
/*																									*/
/*		��{�p�����[�^��`																			*/
/*																									*/
/****************************************************************************************************/
typedef	struct BPRMDAT
{									/* Comment							Rotary		Linear		*/
	BOOL	RvsDir;					/* �t��]���[�h												*/
	LONG	DirSign;				/* 1:����]���[�h, -1:�t��]���[�h							*/
	LONG	MotSel;					/* ���[�^�I��												*//* <S04D> */
	LONG	EncSel;					/* �G���R�[�_�I��											*//* <S04D> */
	BOOL	FencUse;				/* �t���N�G���R�[�_�g�p�t���O								*/
	BOOL	FencType;				/* �t���N�G���R�[�_�g�p�\AMP								*//* <S04D> */
	BOOL	PsFulcSelect;			/* ���S�t���N�^�C�v											*//* <S04D> */
	BOOL	SencFbOptUse;			/* Use Feedback Option Module Encoder for Semi-close control*//* <S04D> */
	BOOL	LinearMotorType;		/* Motor Type Rotary or Linear								*/
/*----------------------------------------------------------------------------------------------*/
	LONG	FbPulse;				/* Feedback PulseNo (Menc/Fenc)		[p/rev]		[p/360deg]	*/
	LONG	MotPulse;				/* Motor Encoder PulseNo			[p/rev]		[p/360deg]	*/
	LONG	XfbPulse1;				/* Feedback PulseNo for I/F			[p/rev]		[p/mm]		*/
	LONG	XfbPulse2;				/* Feedback PulseNo for I/F			[p/rev]		[p/pitch]	*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Kfbpls;					/* FB Pulse Factor (Menc/Fenc)		[rad/p]		[m/p]		*/
	KSGAIN	Kmotpls;				/* Motor Encoder Pulse Factor		[rad/p]		[m/p]		*/
	KSGAIN	KmotplsFC;				/* Motor Encoder Pulse Factor		[rad/p]		[m/p]		*/
	KSGAIN	Keangle;				/* Electric Angle Trans Factor		[Erad/Mrad]	[Erad/m]	*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	RatSpd;					/* RatedSpeed						[rad/sec]	[m/sec]		*/
	KSGAIN	OvrSpd;					/* OverSpeed						[rad/sec]	[m/sec]		*/
//	LONG	MaxAcc;	(���󖢎g�p)	/* Max.Acceleration					[rad/s^2]	[m/s^2]		*/
	KSGAIN	MaxTrq;					/* Max.Torque						[Nm]		[N]			*/
	KSGAIN	MaxCur;					/* Max.Current						[A]			[A]			*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Vdc;					/* DC Voltage						[V]			[V]			*/
	KSGAIN	MotW;					/* Motor Power						[W]			[W]			*/
	KSGAIN	MotA;					/* Motor Rated Current				[A]			[A]			*/
	KSGAIN	MotR;					/* Motor Resistance					[ohm]		[ohm]		*/
	KSGAIN	MotLd;					/* Motor d-axis Inductance			[H]			[H]			*/
	KSGAIN	MotLq;					/* Motor q-axis Inductance			[H]			[H]			*/
	KSGAIN	MotLdb;					/* Motor Inductance for DbAlmChk	[H]			[H]			*/
	KSGAIN	MotEmf;					/* Motor EMF Constant				[Vrms/r/s]	[Vrms/m/s]	*/
	KSGAIN	Jmot;					/* Motor Rotor Inertia				[kg*m^2]	[kg]		*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	OvrPspd;				/* Pulse Speed at OvrSpd			[p/sec]     [p/sec]		*/
	KSGAIN	MaxPspd;				/* Pulse Speed at MaxSpd			[p/sec]     [p/sec]		*/
	KSGAIN	Knorspd;				/* Normalized Speed Trans Factor	[r/min,mm/s --> NorSpd]	*/
	KSGAIN	NorspdConv;				/* Normalized Speed Trans Factor	[r/min,mm/s --> NorSpd]	*/
	KSGAIN	KMotSpdConv;			/* ���x���Z�W��		<V053>			[0.1r/min,0.1mm/s->p/ms]*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	ExpSpd;					/* Motor Speed   Exponent			[-]			[-]			*/
	KSGAIN	ExpTrq;					/* Motor Torque  Exponent			[-]			[-]			*/
	KSGAIN	ExpMotW;				/* Motor Power   Exponent			[-]			[-]			*/
	KSGAIN	ExpJmot;				/* Motor Inertia Exponent			[-]			[-]			*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Kmottrq;				/* Kmottrq = MotMaxTrq / MaxTrq		[-]			[-]			*/
	USHORT	PerMaxTrq;				/* Max.  Torque Percent				[%]			[%]			*/
	USHORT	PerOvrSpd;				/* Over  Speed Percent				[0.01%]		[0.01%]		*/
	LONG	NorOvrSpd;				/* Over  Speed Normalized			[2^24]		[2^24]		*/
#if (FLOAT_USE==TRUE)
	float	pNorOvrSpd;				/* Over  Speed Normalized(����)		[100.0]		[100.0]		*/
	float	nNorOvrSpd;				/* Over  Speed Normalized(����)		[-100.0]	[-100.0]	*/
#endif /* FLOAT_USE */
	LONG	NorMaxSpd;				/* Max.  Speed Normalized			[2^24/OS]	[2^24/OS]	*/
	LONG	NorRatSpd;				/* Rated Speed Normalized			[2^24/OS]	[2^24/OS]	*/
	LONG    NorSuspSpd;             /* Suspend Speed Normalized			[2^24/OS]	[2^24/OS]   <S201>*/
	ULONG	OSCountMax;				/* OS��ԘA���J�E���^����l			[-]			[-]			*/
/*----------------------------------------------------------------------------------------------*/
	LONG	AmpMotWatRatio;			/* (AmpWat  / MotWat ) * 2^16		[-]			[-]			*/
	LONG	AmpMotImaxRatio;		/* (AmpImax / MotImax) * 2^16		[-]			[-]			*/
/*----------------------------------------------------------------------------------------------*/
	LONG	CompVolt;				/* ���H�d���␳�����l				<V116>					*/
/*----------------------------------------------------------------------------------------------*/
	UCHAR	AmpInVcode;				/* Amp Input  Volt Code				[-]			[-]			*/
	UCHAR	AmpOutVcode;			/* Amp Output Volt Code				[-]			[-]			*/
	UCHAR	AmpDcinType;			/* Amp DC Input Type (AC:0, DC:1)	[-]			[-]			*/
	UCHAR	AmpOut2VType;			/* Amp �{�d���d�l(TRUE/FALSE)		[-]			[-]			*/
	USHORT	AmpUnVolt;				/* Amp Volt for UnXXX Monitor		[V]			[V]			*/
/*----------------------------------------------------------------------------------------------*/
	UCHAR	MotVcode;				/* Motor Volt Code					[-]			[-]			*/
	UCHAR	MaxVel;					/* Max Velocity						[100r/min]	[100mm/s]	*/
	USHORT	MotUnVolt;				/* Motor Volt For UnXXX Monitor		[V]			[V]			*/
	USHORT	VdetMax;				/* Amp Volt detect at max input		[data/MaxV] [data/MaxV] */
	USHORT	VdetAdMax;				/* Amp Volt AD detect at max input	[data/MaxV] [data/MaxV] */
/*----------------------------------------------------------------------------------------------*/
	BOOL	CurrentLoopTiSw;		/* �d�����[�v�ϕ����萔�ؑփX�C�b�`							*/
	UCHAR	MotTypeCode;			/* Motor Type Code					[-]			[-]			*/

/*----------------------------------------------------------------------------------------------*/
	LONG	SvCycleUs;				/* �T�[�{������� [us]										*/
	LONG	SvCycleNs;				/* �T�[�{������� [ns]										*/
	LONG	MPFCycleUs_SC;			/* ���Ɍ��o�������(���x�E�g���N����) [us]					*/
	LONG	MPFCycleUs_MR;			/* ���Ɍ��o�������(���C������) [us]						*/
/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float	Kposlvl;				/* Position Level Cal. Factor	[CmdPulse --> FbkPulse]		*/
	float	Kspdlvl;				/* Speed    Level Cal. Factor	[0.1r/min,mm/s --> NorSpd]	*/
	float	Ktrqlvl;				/* Torque   Level Cal. Factor	[0.01% --> 2^24/MaxTrq]		*/
	float	Ktrqlvls;				/* Torque   Level Cal. Factor	[0.01% --> 15000/MaxTrq]	*/
/*----------------------------------------------------------------------------------------------*/
	float	Kmonorspd;				/* Normal. Speed Trans Factor	[r/min,mm/s --> NorSpd]		*/
	float	Knortrq;				/* Normal. Torque Trans Factor	[%RatedTorque --> NorTrq]	*/
	float	Kmonspd;				/* Monitor Speed Trans Factor	[NorSpd  --> r/min,mm/s]	*/
	float	Kmontrq;				/* Monitor Torque Trans Factor	[NorTrq  --> %RatedTorque]	*/
	float	Kmoncrnt;				/* Monitor Current Trans Factor	[NorCurrent->%RatedCurrent]	*/
	float	Kmonpfbkspd;			/* Monitor PfbkSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	float	Kmonpcmdspd;			/* Monitor PcmdSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	float	Kmonpcmdspd2ms;			/* Monitor PcmdSpd Trans Factor [p/2ms   --> r/min,mm/s]	*/
	float	Knorpulsspd;			/* Monitor PulsSpd Trans Factor [p/ScanB --> 2^24/OvrSpd]	*/
/*----------------------------------------------------------------------------------------------*/
	float	KmotspdA;				/* ScanA Motor Speed Cal. Gain	(KIOP����ړ�)				*/
	float	KmotspdB;				/* ScanB Motor Speed Cal. Gain	(KIOP����ړ�)				*/
	float	KmotspdC;				/* ScanC Motor Speed Cal. Gain	(KIOP����ړ�)				*/
/*----------------------------------------------------------------------------------------------*/
	float	Kcurload;				/* �����g���N(�d��)���Z�Q�C��(ScanC�ϐ�����ړ�)			*/
/*----------------------------------------------------------------------------------------------*/
	BOOL	SpdDetMaFil;			/* ���[�^���x���o�p�ړ����σt�B���^�L��/�����I��			*/
	float	SpdDetectUnit;			/* ���x���o����\	[0.1r/min/pulse],[0.1mm/s/pulse]		*/
	float	SpdFbLpassFil;			/* ���[�^�É��p���x�e�aLPF�ݒ�	[0.01ms]		<V423>		*/
/*----------------------------------------------------------------------------------------------*/
	float	MonScalePitch;			/* Scale Pitch for Monitor				[pm]	MENCV����	*/
	float	MonScaleExp;			/* Scale Pitch Exponent for Monitor		[-]		MENCV����	*/
/*----------------------------------------------------------------------------------------------*/
	float	KConvLoadSpd;			/* ���ב��x�ϊ��W��	[pls/ScanA] -> [2^24/OvrSpd]			*/
/*----------------------------------------------------------------------------------------------*/
	float	Kvrefad;				/* ���x�w��AD���Z�Q�C��										*/
#else
	LONG	Kposlvl;				/* Position Level Cal. Factor	[CmdPulse --> FbkPulse]		*/
	LONG	Kspdlvl;				/* Speed    Level Cal. Factor	[0.1r/min,mm/s --> NorSpd]	*/
	LONG	Ktrqlvl;				/* Torque   Level Cal. Factor	[0.01% --> 2^24/MaxTrq]		*/
	LONG	Ktrqlvls;				/* Torque   Level Cal. Factor	[0.01% --> 15000/MaxTrq]	*/
/*----------------------------------------------------------------------------------------------*/
	LONG	Kmonorspd;				/* Normal. Speed Trans Factor	[r/min,mm/s --> NorSpd]		*/
	LONG	Knortrq;				/* Normal. Torque Trans Factor	[%RatedTorque --> NorTrq]	*/
	LONG	Kmonspd;				/* Monitor Speed Trans Factor	[NorSpd  --> r/min,mm/s]	*/
	LONG	Kmontrq;				/* Monitor Torque Trans Factor	[NorTrq  --> %RatedTorque]	*/
	LONG	Kmoncrnt;				/* Monitor Current Trans Factor	[NorCurrent->%RatedCurrent]	*/
	LONG	Kmonpfbkspd;			/* Monitor PfbkSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	LONG	Kmonpcmdspd;			/* Monitor PcmdSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	LONG	Kmonpcmdspd2ms;			/* Monitor PcmdSpd Trans Factor [p/2ms   --> r/min,mm/s]	*/
	LONG	Knorpulsspd;			/* Monitor PulsSpd Trans Factor [p/ScanB --> 2^24/OvrSpd]	*/
/*----------------------------------------------------------------------------------------------*/
	LONG	KmotspdA;				/* ScanA Motor Speed Cal. Gain	(KIOP����ړ�)				*/
	LONG	KmotspdB;				/* ScanB Motor Speed Cal. Gain	(KIOP����ړ�)				*/
	LONG	KmotspdC;				/* ScanC Motor Speed Cal. Gain	(KIOP����ړ�)				*/
/*----------------------------------------------------------------------------------------------*/
	LONG	Kcurload;				/* �����g���N(�d��)���Z�Q�C��(ScanC�ϐ�����ړ�)			*/
/*----------------------------------------------------------------------------------------------*/
	BOOL	SpdDetMaFil;			/* ���[�^���x���o�p�ړ����σt�B���^�L��/�����I��			*/
	LONG	SpdDetectUnit;			/* ���x���o����\	[0.1r/min/pulse],[0.1mm/s/pulse]		*/
	LONG	SpdFbLpassFil;			/* ���[�^�É��p���x�e�aLPF�ݒ�	[0.01ms]		<V423>		*/
/*----------------------------------------------------------------------------------------------*/
	ULONG	MonScalePitch;			/* Scale Pitch for Monitor				[pm]	MENCV����	*/
	LONG	MonScaleExp;			/* Scale Pitch Exponent for Monitor		[-]		MENCV����	*/
/*----------------------------------------------------------------------------------------------*/
	LONG	KConvLoadSpd;			/* ���ב��x�ϊ��W��	[pls/ScanA] -> [2^24/OvrSpd]			*/
/*----------------------------------------------------------------------------------------------*/
	LONG	Kvrefad;				/* ���x�w��AD���Z�Q�C��										*/
#endif /* FLOAT_USE */
/*----------------------------------------------------------------------------------------------*/
/*		���ԃp�����[�^��`																		*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Kpx;					/* Position Control Gain									*/
	KSGAIN	Kvx;					/* Speed    Control Gain									*/
	KSGAIN	Kpvx;					/* Position-Speed Control Gain for MFC & Predictive Control	*/
	KSGAIN	Kmotspd;				/* Motor Speed Cal. Gain  [pulse/sec] --> [2^24/OvrSpd]		*/
	KSGAIN	KmotspdFC;				/* Motor Speed Cal. Gain  [pulse/sec] --> [2^24/OvrSpd]		*/
/*----------------------------------------------------------------------------------------------*/
/*	AP <S00C>																					*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN  TrefonLevel;			/* Torque Reference Input ON Level [2^24/MaxTrq]			*/
	ULONG	OsAlmLvl;				/* Os�A���[�����x�� */	/* <S1D0> */
} BPRMDAT;

typedef struct BPRM_INICFG {
	ULONG	ratb2l;		/* Pn20E : �d�q�M�A��(���q)									*/
	ULONG	rata2l;		/* Pn210 : �d�q�M�A��(����)									*/
	USHORT	maxvel_l;	/* Pn385 : ���[�^�ō����x(���j�A)							*/
	DBYTEX	styp;		/* PnE11 : ����:�T�[�{�p�b�N�`���R�[�h, ���:���͓d���R�[�h	*/
	USHORT	ampw;		/* PnE12 : �T�[�{�p�b�N�e��									*/
	USHORT	imaxs;		/* PnE15 : �T�[�{�p�b�N�ő�d��								*/
	USHORT	dq_sw2;		/* PnE39 : �d������X�C�b�`2								*/
	DBYTEX	ratmt2_os;	/* PnE62 : ����:�ߕ��׌��o���Ԏ���2, ���:OS���o���x��		*/
	USHORT	vrefgn;		/* Pn300 : ���x�w�ߓ��̓Q�C��								*/
	USHORT	gadjv;		/* PnE55 : ���x�w�߃Q�C����									*/
} BPRM_INICFG;

typedef enum BPRM_ERR_ {
	BPRM_NOALM		= 0,				/* �A���[���Ȃ�								*/
	BPRM_NOTSUPPORT	= 0x00000001,		/* A.051 : ���i���T�|�[�g�A���[��			*/
	BPRM_VELPRM		= 0x00000002,		/* A.550 : �ō����x�ݒ�ُ�					*/
} BPRM_ERR;

/****************************************************************************************************/
/*	��{�p�����[�^�v�Z���C�� */
/****************************************************************************************************/
void BprmSetServoCycle( BPRMDAT *Bprm, LONG SACycleNs, LONG SBCycleNs );
LONG BprmCalcBaseParameters( BPRMDAT *Bprm, MENCPRM *MencP, BPRM_INICFG *CfgPrm, MENCV *MencV, MENCV *FencV );

LONG	BprmTorqueLevelCal( BPRMDAT *Brpm, LONG Torque, LONG LvlSel );
KSGAIN	BprmSpeedLevelCal( BPRMDAT *Brpm, KSGAIN Speed, KSGAIN LmtSel );		/*<S00A>*/
LONG	BprmPositionLevelCal( BPRMDAT *Brpm, LONG Position );


#endif	/* _BPRM_H_ */
/***************************************** end of file **********************************************/
