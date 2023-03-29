#include <math.h>
#include "SimMain.h"

#define		SAMPLING	SIMULATION_SAMPLING			/* Sampling Time [s] */
#define		PG_BIT		20							/* Encoder PG Bit Num */
#define		BASETIME_CK	6000000						/* BaseTimer Clock [Hz] */
#define		JL			100							/* 負荷イナーシャ比 [%] */

#define		PI2div3		2.0943951023931953F			/* Expression: 2*pi/3 */
#define		SQRT2div3	0.816496580927726F			/* Expression: sqrt(2/3) */
#define		SQRT2		1.414213562373095F			/* Expression: sqrt(2) */
#define		PI2			6.283185307179586F			/* Expression: 2*pi */
#define		VDCdiv2		141.4213562373095F			/* Voltage Normalize Gain */

#define		BASE_TIME_UP_COUNT		SAMPLING	1/6000000*SAMPLING

#define		FALSE		0
#define		TRUE		1


typedef struct _MOTOR_PROPATY
{
	double		Irate;		/* Expression: 定格電流[Arms] */
	double		Imax;		/* Expression: 最大電流[Arms] */
	double		La_Inv;		/* Expression: 1/LA */
	double		Ra;			/* Expression: RA */
	double		Kt;			/* Expression: KT */
	double		Ke;			/* Expression: Ke */
	double		Pole;		/* Expression: POLE */
	double		J_Inv;		/* Expression: 1/J */
} MOTOR_PROPATY;


typedef union _ENC_DATA_D7
{
	struct {
		unsigned int InvalidData	: 4;
		unsigned int Position_L		: 4;
	} Data;
	unsigned char Position_L;
} END_DATA_D7;

typedef union _ENC_DATA_D10
{
	struct {
		unsigned int Signal_Z		: 1;
		unsigned int Pole_W			: 1;
		unsigned int Pole_V			: 1;
		unsigned int Pole_U			: 1;
		unsigned int IncreComp_L	: 4;
	} Data;
	unsigned char EncData10;
} END_DATA_D10;

typedef union _ENC_DATA_D11
{
	struct {
		unsigned int IncreComp_H	: 8;
	} Data;
	unsigned char EncData11;
} END_DATA_D11;

typedef struct _ENC_DATA
{ /* for Make Encoder Data */
	unsigned char	AlmCode;
	unsigned char	Data1;
	unsigned char	Data2;
	unsigned char	TimeBaseTimer_L;
	unsigned char	TimeBaseTimer_H;
	unsigned char	LastChangePosTime_L;
	unsigned char	LastChangePosTime_H;
	unsigned char	Position_L;
	unsigned char	Position_M;
	unsigned char	Position_H;
	unsigned char	EncData10;
	unsigned char	EncData11;
} ENCODER_DATA;

typedef struct _OMEGA_KE_UVW
{ /* for Split to 3Phase */
	double OmegaKe_U;
	double OmegaKe_V;
	double OmegaKe_W;
} OMEGA_KE_UVW;

typedef struct _OUT_MOTOR_COIL
{ /* for Motor Coil Pass */
	double MotorTrq;
	double CurrentFB_U;
	double CurrentFB_V;
	double CurrentFB_W;
} OUT_MOTOR_COIL;

typedef struct _CALC_MOTOR_COIL
{ /* for Motor Coil Pass */
	double IntegralBuf1;
	double IntegralBuf2;
	double IntegralBuf3;
	double LstCurrentFB_U;
	double LstCurrentFB_V;
	double LstCurrentFB_W;
} CALC_MOTOR_COIL;

typedef struct _OUT_MOTOR_MODEL
{ /* for Motor Model Calc */
	double MotorTrqInv;
	double MotorSpeedFB;
	double CurrentFB_U;
	double CurrentFB_V;
} OUT_MOTOR_MODEL;

typedef struct _CALC_MOTOR_MODEL
{ /* for Motor Model Calc */
	double IntegralBuf1;
	double IntegralBuf2;
	double LstMotorSpeed;
} CALC_MOTOR_MODEL;

typedef struct _OUT_MOTOR_FB
{ /* for Motor feedback calc. */
	double CurrentFB_U;
	double CurrentFB_V;
	double MotorCurrent;
	double MotSpeed;
	double MotPhase;
	long MotPosition;
} OUT_MOTOR_FB;

typedef struct _CALC_MOTOR_FB
{ /* for Motor Model Calc */
	double IntegralBuf;
} CALC_MOTOR_FB;

typedef struct _MODEL_SIM
{ /* for Machine Model Simulation */
	MOTOR_PROPATY		MotorPropaty;
	CALC_MOTOR_MODEL	CalcMotModel;
	CALC_MOTOR_COIL		CalcMotCoil;
	CALC_MOTOR_FB		CalcMotFb;
	OUT_MOTOR_FB		OutMotorFb;
	OMEGA_KE_UVW		OmegaKeUVW;
} MODEL_SIM;

typedef struct _AMP_INFO
{ /* Servopack Infomation */
	unsigned short ShantR;
	double AmpImax;
	double MaxCurrent;
	double CurrentChgGain;
} AMP_INFO;

/* Prototype */
/* Globals */
void InitializeMotorModel( void );
void SetMotorAmpParameter( unsigned short *MotPrm,unsigned short *AmpPrm, unsigned char axno );
void CalcMotorFeedback( double Vu, double Vv, double Vw, double *IuFb, double *IvFb, unsigned char axno );
void GetEncoderInfo( ENCODER_DATA *EncoderData, unsigned char axno );

/* Statics */
OUT_MOTOR_MODEL CalcMotorModel( double Vu, double Vv, double Vw, MODEL_SIM	*SimData );
OUT_MOTOR_COIL PassMotorCoil( double Vu, double Vv, double Vw, double Phase, MODEL_SIM *SimData );
double CalcMotorTorque( double Iu, double Iv, double Iw, double Phase, double Kt );
OMEGA_KE_UVW CalcSplitTo3Phase( double OmegaKe, double Phase );

/* Statics */
static	MODEL_SIM	ModelSimData[SIM_AXISNUM];
static	AMP_INFO	ServopackData[SIM_AXISNUM];
static	unsigned short BaseTimer;
static	unsigned short BaseTimerUpCount;

/* for debug */
double	debug_bufU[5000];
double	debug_bufV[5000];
double	debug_bufW[5000];
long	debug_ctr;
long	debug_flg;



/****************************************************************************************************/
/*																									*/
/*		Start Function Difinition																	*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Split ωKe three phase (UVW)																*/
/****************************************************************************************************/
void InitializeMotorModel( void )
{
	int	i;

	for( i=0; i<SIM_AXISNUM; i++ )
	{
		ModelSimData[i].CalcMotCoil.IntegralBuf1 = 0.0f;
		ModelSimData[i].CalcMotCoil.IntegralBuf2 = 0.0f;
		ModelSimData[i].CalcMotCoil.IntegralBuf3 = 0.0f;
		ModelSimData[i].CalcMotCoil.LstCurrentFB_U = 0.0f;
		ModelSimData[i].CalcMotCoil.LstCurrentFB_V = 0.0f;
		ModelSimData[i].CalcMotCoil.LstCurrentFB_W = 0.0f;

		ModelSimData[i].CalcMotModel.IntegralBuf1 = 0.0f;
		ModelSimData[i].CalcMotModel.IntegralBuf2 = 0.0f;
		ModelSimData[i].CalcMotModel.LstMotorSpeed = 0.0f;

		ModelSimData[i].CalcMotFb.IntegralBuf = 0.0f;

		ModelSimData[i].OmegaKeUVW.OmegaKe_U = 0.0f;
		ModelSimData[i].OmegaKeUVW.OmegaKe_V = 0.0f;
		ModelSimData[i].OmegaKeUVW.OmegaKe_W = 0.0f;
	}

	BaseTimerUpCount = (unsigned short)((double)SAMPLING * (1 / (double)BASETIME_CK));

	debug_ctr = 0;
	debug_flg = 0;

	return;
}


/****************************************************************************************************/
/*		Split ωKe three phase (UVW)																*/
/****************************************************************************************************/
void SetMotorAmpParameter( unsigned short *MotPrm,unsigned short *AmpPrm, unsigned char axno )
{
	double dwk;
	MOTOR_PROPATY	*MotInfo;
	AMP_INFO		*AmpInfo;

	MotInfo = &(ModelSimData[axno].MotorPropaty);
	AmpInfo = &(ServopackData[axno]);

	MotInfo->Irate = (double)MotPrm[0] * 0.1;
	MotInfo->Imax = (double)MotPrm[1] * 0.1;
	MotInfo->La_Inv = 1 / ((double)MotPrm[2] * 0.01 / 1000);
	MotInfo->Ra = (double)MotPrm[3] / 1000;
	MotInfo->Kt = (double)MotPrm[4] * SQRT2 * 60 / PI2 / 10000 * SQRT2div3;
	MotInfo->Ke = MotInfo->Kt;
	MotInfo->Pole = MotPrm[5];
	dwk = ((double)MotPrm[6] * pow(10.0, (double)((char)MotPrm[7])) / 1000000);
	MotInfo->J_Inv = 1 / (dwk * (100 + JL) / 100);

	AmpInfo->AmpImax = (double)AmpPrm[0] * 0.1;
	AmpInfo->ShantR = AmpPrm[1];

	if( AmpInfo->AmpImax < MotInfo->Imax )
	{
		AmpInfo->MaxCurrent = AmpInfo->AmpImax;
		AmpInfo->CurrentChgGain = (double)(1.0 / AmpInfo->AmpImax);
	}
	else
	{
		AmpInfo->MaxCurrent = MotInfo->Imax;
		AmpInfo->CurrentChgGain = (double)(MotInfo->Imax / AmpInfo->AmpImax / AmpInfo->AmpImax);
	}
}


/****************************************************************************************************/
/*		Get Encoder Information (I/F Function)														*/
/****************************************************************************************************/
void GetEncoderInfo( ENCODER_DATA *EncoderData, unsigned char axno )
{
	unsigned short *TimeBaseTimer;
	unsigned short *LastChangePosTime;

	END_DATA_D7		Position_L;
	END_DATA_D10	EncData10;
	OUT_MOTOR_FB	*OutMotorFb;

	OutMotorFb = &(ModelSimData[axno].OutMotorFb);

	/* AlmCode, Data1, Data2 */
	EncoderData->AlmCode = 0x00;
	EncoderData->Data1 = 0x00;
	EncoderData->Data2 = 0x00;

	/* TimeBaseTimer, LastChangePosTime */
	TimeBaseTimer = (unsigned short*)&EncoderData->TimeBaseTimer_L;
	*TimeBaseTimer = BaseTimer;
	LastChangePosTime = (unsigned short*)&EncoderData->LastChangePosTime_L;
	*LastChangePosTime = 0;

	/* Position Data */
	Position_L.Position_L = (unsigned char)( (OutMotorFb->MotPosition << 4) & 0xf0 );
	EncoderData->Position_L = (unsigned char)Position_L.Position_L;
	EncoderData->Position_M = (unsigned char)(OutMotorFb->MotPosition >> 4);
	EncoderData->Position_H = (unsigned char)(OutMotorFb->MotPosition >> 12);

	/* Hole Cencer, IncreComp */
	EncData10.Data.Signal_Z = 0;
	EncData10.Data.Pole_W = 0;
	EncData10.Data.Pole_V = 0;
	EncData10.Data.Pole_U = 0;
	EncData10.Data.IncreComp_L = 0;
	EncoderData->EncData10 = (unsigned char)EncData10.EncData10;
	
	EncoderData->EncData11 = 0;

	return;
}


/****************************************************************************************************/
/*		Calculate Motor Model																		*/
/****************************************************************************************************/
void CalcMotorFeedback( double Vu, double Vv, double Vw, double *IuFb, double *IvFb, unsigned char axno )
{
	OUT_MOTOR_MODEL	OutMotorModel;
	AMP_INFO		*AmpInfo;
	MODEL_SIM		*SimData;
	MOTOR_PROPATY	*MotorPropaty;
	CALC_MOTOR_FB	*CalcMotFb;
	OUT_MOTOR_FB	*OutMotorFb;
	double			dwk;

	/* Set to Pointers */
	AmpInfo = &(ServopackData[axno]);
	SimData = &(ModelSimData[axno]);
	MotorPropaty = &(SimData->MotorPropaty);
	CalcMotFb = &(SimData->CalcMotFb);
	OutMotorFb = &(SimData->OutMotorFb);

	Vu = Vu * VDCdiv2 / 8192;
	Vv = Vv * VDCdiv2 / 8192;
	Vw = Vw * VDCdiv2 / 8192;

	/* Calculate Motor Model */
	OutMotorModel = CalcMotorModel( Vu, Vv, Vw, SimData );

	/* Set Output Buffer */
	OutMotorFb->CurrentFB_U = OutMotorModel.CurrentFB_U;
	OutMotorFb->CurrentFB_V = OutMotorModel.CurrentFB_V;
	OutMotorFb->MotSpeed = OutMotorModel.MotorSpeedFB;

	/* Calculate MotorCurrent */
	OutMotorFb->MotorCurrent = OutMotorModel.MotorTrqInv * 100 / MotorPropaty->Irate / MotorPropaty->Kt;

	/* Calculate MotorPosition */
	dwk = CalcMotFb->IntegralBuf * (double)(1<<PG_BIT) / PI2;
	OutMotorFb->MotPosition = (long)dwk;
	CalcMotFb->IntegralBuf += OutMotorModel.MotorSpeedFB * SAMPLING;

	/* Calculate MotorPhase */
	OutMotorFb->MotPhase = OutMotorFb->MotPosition * (MotorPropaty->Pole / 2) * PI2 / (double)(1<<PG_BIT);

	/* BaseTimer Count up */
	BaseTimer += BaseTimerUpCount;

	/* Output Current Feedback */
	*IuFb = OutMotorFb->CurrentFB_U * AmpInfo->ShantR * AmpInfo->CurrentChgGain;
	*IvFb = OutMotorFb->CurrentFB_V * AmpInfo->ShantR * AmpInfo->CurrentChgGain;
	
	if( (debug_ctr < 5000) && (debug_flg == 0) )
	{
		if( (debug_ctr & 0x01) == 0x00 )
		{
			debug_bufU[debug_ctr>>1] = OutMotorFb->CurrentFB_U;
			debug_bufV[debug_ctr>>1] = OutMotorFb->CurrentFB_V;
			debug_bufW[debug_ctr>>1] = OutMotorFb->MotSpeed;
		}
		debug_ctr++;
	}
	else
	{
		debug_flg = 1;
	}

	return;
}


/****************************************************************************************************/
/*		Calculate Motor Model																		*/
/****************************************************************************************************/
OUT_MOTOR_MODEL CalcMotorModel( double Vu, double Vv, double Vw, MODEL_SIM *SimData )
{
	OUT_MOTOR_MODEL		OutMotorModel;
	OUT_MOTOR_COIL		OutMotorCoil;
	MOTOR_PROPATY		*MotorPropaty;
	CALC_MOTOR_MODEL	*CalcMotModel;
	OMEGA_KE_UVW		*OmegaKeUVW;
	double				OmegaKe, Phase;

	/* Set to Pointers */
	CalcMotModel = &(SimData->CalcMotModel);
	MotorPropaty = &(SimData->MotorPropaty);
	OmegaKeUVW = &(SimData->OmegaKeUVW);

	/* Prepare Split ωKe three phase */
	CalcMotModel->IntegralBuf1 += CalcMotModel->LstMotorSpeed * SAMPLING;
	Phase = CalcMotModel->IntegralBuf1 * MotorPropaty->Pole / 2;

	/* Pass Motor Coil */
	OutMotorCoil = PassMotorCoil( Vu, Vv, Vw, Phase, SimData );

	/* Set Output Buffer */
	OutMotorModel.MotorTrqInv = OutMotorCoil.MotorTrq * (-1);
	OutMotorModel.CurrentFB_U = OutMotorCoil.CurrentFB_U;
	OutMotorModel.CurrentFB_V = OutMotorCoil.CurrentFB_V;

	/* Calculate Motor Speed */
	OutMotorModel.MotorSpeedFB = CalcMotModel->IntegralBuf2 * (-1);
	CalcMotModel->LstMotorSpeed = OutMotorModel.MotorSpeedFB;
	CalcMotModel->IntegralBuf2 += OutMotorCoil.MotorTrq * MotorPropaty->J_Inv * SAMPLING;

	/* Split ωKe three phase (UVW) */
	OmegaKe = CalcMotModel->LstMotorSpeed * MotorPropaty->Ke;
	*OmegaKeUVW = CalcSplitTo3Phase( OmegaKe, Phase );

	/* Prepare Into Motor Coil */
	Vu = Vu - OmegaKeUVW->OmegaKe_U;
	Vv = Vv - OmegaKeUVW->OmegaKe_V;
	Vw = Vw - OmegaKeUVW->OmegaKe_W;

	if( (debug_ctr < 5000) && (debug_flg == 0) )
	{
//		debug_bufU[debug_ctr] = OutMotorCoil.MotorTrq;
//		debug_bufV[debug_ctr] = OutMotorModel.MotorSpeedFB;
//		debug_bufW[debug_ctr] = Vw;
//		debug_ctr++;
	}
	else
	{
		debug_flg = 1;
	}

	return OutMotorModel;
}


/****************************************************************************************************/
/*		Pass Motor Coil																				*/
/****************************************************************************************************/
OUT_MOTOR_COIL PassMotorCoil( double Vu, double Vv, double Vw, double Phase, MODEL_SIM *SimData )
{
	OUT_MOTOR_COIL	OutMotorCoil;
	MOTOR_PROPATY	*MotorPropaty;
	CALC_MOTOR_COIL	*CalcMotCoil;
	double			dwk;

	/* Set to Pointers */
	CalcMotCoil = &(SimData->CalcMotCoil);
	MotorPropaty = &(SimData->MotorPropaty);

	/* Make U phase current */
	dwk = (Vu - (CalcMotCoil->LstCurrentFB_U * MotorPropaty->Ra)) * MotorPropaty->La_Inv;
	OutMotorCoil.CurrentFB_U = CalcMotCoil->IntegralBuf1;
	CalcMotCoil->IntegralBuf1 += dwk * SAMPLING;

	/* Make V phase current */
	dwk = (Vv - (CalcMotCoil->LstCurrentFB_V * MotorPropaty->Ra)) * MotorPropaty->La_Inv;
	OutMotorCoil.CurrentFB_V = CalcMotCoil->IntegralBuf2;
	CalcMotCoil->IntegralBuf2 += dwk * SAMPLING;

	/* Make W phase current */
	dwk = (Vw - (CalcMotCoil->LstCurrentFB_W * MotorPropaty->Ra)) * MotorPropaty->La_Inv;
	OutMotorCoil.CurrentFB_W = CalcMotCoil->IntegralBuf3;
	CalcMotCoil->IntegralBuf3 += dwk * SAMPLING;

	/* Current Mix */
	OutMotorCoil.MotorTrq = CalcMotorTorque( OutMotorCoil.CurrentFB_U,
											 OutMotorCoil.CurrentFB_V,
											 OutMotorCoil.CurrentFB_W,
											 Phase,
											 MotorPropaty->Kt );

	/* Keep Last Output */
	CalcMotCoil->LstCurrentFB_U = OutMotorCoil.CurrentFB_U;
	CalcMotCoil->LstCurrentFB_V = OutMotorCoil.CurrentFB_V;
	CalcMotCoil->LstCurrentFB_W = OutMotorCoil.CurrentFB_W;

	return OutMotorCoil;
}


/****************************************************************************************************/
/*		Make Motor Torque																			*/
/****************************************************************************************************/
double CalcMotorTorque( double Iu, double Iv, double Iw, double Phase, double Kt )
{
	double	Torque;
	double	dwk;

	dwk = sin( Phase ) * Kt * Iu;
	dwk += sin( Phase - PI2div3 ) * Kt * Iv;
	dwk += sin( Phase + PI2div3 ) * Kt * Iw;
	Torque = dwk;

	return Torque;
}


/****************************************************************************************************/
/*		Split ωKe three phase (UVW)																*/
/****************************************************************************************************/
OMEGA_KE_UVW CalcSplitTo3Phase( double OmegaKe, double Phase )
{
	OMEGA_KE_UVW	OmegaKeUVW;

	/* Make OmegaKeU */
	OmegaKeUVW.OmegaKe_U = OmegaKe * sin( Phase );

	/* Make OmegaKeV */
	OmegaKeUVW.OmegaKe_V = sin( Phase - PI2div3 ) * OmegaKe;

	/* Make OmegaKeW */
	OmegaKeUVW.OmegaKe_W = sin( Phase + PI2div3) * OmegaKe;

	return OmegaKeUVW;
}
