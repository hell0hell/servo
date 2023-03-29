/****************************************************************************
Description	: Mercury simulation servo interface
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

Functions		:

----------------------------------------------------------------------------
Changes			:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.08.24	created

****************************************************************************/
#include "MercuryGlobals.h"

/* Global */
extern	AXIS_HANDLE		*AxisRscI;

extern	void ALMSetMask( ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id, BOOL MaskSet );

/* Defines */
#define	PosRefChngRate		2.184533F		/* 125us周期 (1048576/60/1000/8) */
#define	SpeedRefChngRate	4.266668F			/* ADと最高速度との兼ね合い */

/* Static */
static	ULONG	PositionRefCntrL[MAX_AXIS_NUMBER] = {0};
static	USHORT	PositionRefCntrS[MAX_AXIS_NUMBER] = {0};


USHORT	SvifGetAlmCode( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	AxHdl->AlmManager->AlmCode;
}

void	SvifSetControlMode( USHORT ControlMode, unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	AxHdl->UniPrms->Prm->b_prm0 = (AxHdl->UniPrms->Prm->b_prm0 & 0xFF0F) | ((ControlMode << 4) & 0x00F0);
}

void	SvifSetEncoderData( unsigned char *Data, unsigned char axno )
{
	int				i;
	unsigned long	EncData;
	AXIS_HANDLE		*AxHdl;

	AxHdl = &(AxisRscI[axno]);

	/* Set Encoder Data */
#ifndef _JL076_USE_
	for( i = 0; i < 3; i++ )
	{
		EncData = (ULONG)((Data[4*i+3] << 24) | (Data[4*i+2] << 16) | (Data[4*i+1] << 8) | (Data[4*i]));
		AxHdl->BaseLoops->MencV->EncIf.RxBuf[i+1] = EncData;
	}
	/* Receive comp flg ON */
	*(AxHdl->BaseLoops->MencV->EncIf.RxFlg) = (ULONG)0x8000;
#else
	for( i = 0; i < 6; i++ )
	{
		EncData = (Data[2*i+1] << 8) | (Data[2*i]);
		AxHdl->BaseLoops->MencV->EncIf.RxBuf[i+2] = (USHORT)EncData;
	}
	/* Receive comp flg ON */
	*(AxHdl->BaseLoops->MencV->EncIf.RxFlg) = 0x8000;
#endif
}

void	SvifSetPulseRef( ULONG Pulse, unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);

#ifndef _JL076_USE_
	PositionRefCntrL[axno] += Pulse;
	AxHdl->SvAsicRegs->AsicHwReg->AREG_CMDLDL = PositionRefCntrL[axno];
#else
	PositionRefCntrS[axno] += (USHORT)Pulse;
	AxHdl->SvAsicRegs->AsicHwReg->AREG_CMDLDL = PositionRefCntrS[axno];
#endif
}

void	SvifSetSpeedRefAd( USHORT SpeedRef, unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);

	SpeedRef = (USHORT)((float)SpeedRef * SpeedRefChngRate);
	AxHdl->SvAsicRegs->AsicHwReg->AREG_ACUL = (ULONG)SpeedRef;
}

void	SvifSetAlmMaskPGCOM( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	ALMSetMask( AxHdl->AlmManager, ALM_PGCOM , TRUE );
}

SHORT	SvifGetTrqRef( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);

#ifndef	_JL076_USE_
	return	AxHdl->SvAsicRegs->AsicMicroReg->AdinV.IqIn;
#else
	return	AxHdl->SvAsicRegs->AsicMicroReg->MREG_IQIN;
#endif
}

LONG	SvifGetMotPos( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	AxHdl->MencV->MotPos >> 6;
}


SHORT	SvifGetMotSpd( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	(SHORT)MlibMulhigh32( AxHdl->MencV->MotSpd, 3840000 );
}

SHORT	SvifGetMotSpdRef( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	(SHORT)MlibMulhigh32( AxHdl->BaseLoops->SpdRefo, 3840000 );
}

SHORT	SvifGetMotdMotPos( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	(SHORT)MlibMulhigh32( AxHdl->BaseLoops->SpdCtrl.V.SpdFbFilo, 3840000 );
}

SHORT	SvifGetSpdPacOut( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	(SHORT)MlibMulhigh32( AxHdl->BaseLoops->SpdCtrl.V.PacOut, 3840000 );
}

SHORT	SvifGetSpdIacOut( unsigned char axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);
	return	(SHORT)MlibMulhigh32( AxHdl->BaseLoops->SpdCtrl.V.IacOut, 3840000 );
}

void	SvifSendMemobusMsg( UCHAR *TxBuf, UCHAR TxSize )
{
	int i;

	for( i=0; i<TxSize; i++ )
	{
		/* シリアルI/F対応のため暫定コメントアウト */
//		AxisRscI->NetMsgIf->UartMsg.MbusIf.RxBuf[i] = TxBuf[i];
	}
	AxisRscI->NetMsgIf->UartMsg.MbusIf.RcvbLength = TxSize;
}

void	SvifSetTraceControl( UCHAR TraceNum, UCHAR DataNum, USHORT *Target, USHORT Sampling,
														USHORT PreTrigger, USHORT TrigCondition )
{
	UCHAR	Buf[88];

	Buf[0] = 0x00;						/* Reserve */
	Buf[1] = 0x42;						/* Function Code : 42H(メモリ書き込み) */
	Buf[2] = 0xFF;						/* Reserve */
	Buf[3] = 0x00;						/* Reserve */
	Buf[4] = 0x04;						/* Sub Function Code : 04H(非連続) */
	Buf[5] = 0x02;						/* Data Type : SHORT */
	*(USHORT*)&Buf[6] = 0x0A00;			/* Data Number : 10 */
	*(ULONG*)&Buf[8] = 0x80004200;		/* Write Register Address */
	*(USHORT*)&Buf[12] = TraceNum;		/* Divide Number */
	*(USHORT*)&Buf[14] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[16] = 0x80004202;		/* Write Register Address */
	*(USHORT*)&Buf[20] = 1;				/* Trace Data Length : SHORT */
	*(USHORT*)&Buf[22] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[24] = 0x80004204;		/* Write Register Address */
	*(USHORT*)&Buf[28] = DataNum;		/* Trace Buffer Length */
	*(USHORT*)&Buf[30] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[32] = 0x80004206;		/* Write Register Address */
	*(USHORT*)&Buf[36] = Sampling;		/* Sampling : 125us * Sampling */
	*(USHORT*)&Buf[38] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[40] = 0x80004208;		/* Write Register Address */
	*(USHORT*)&Buf[44] = 125;			/* Sampling : 125us * Sampling */
	*(USHORT*)&Buf[46] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[48] = 0x80004220;		/* Write Register Address */
	*(USHORT*)&Buf[52] = Target[0];		/* Trace Data1 */
	*(USHORT*)&Buf[54] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[56] = 0x80004222;		/* Write Register Address */
	*(USHORT*)&Buf[60] = Target[1];		/* Trace Data2 */
	*(USHORT*)&Buf[62] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[64] = 0x80004224;		/* Write Register Address */
	*(USHORT*)&Buf[68] = Target[2];		/* Trace Data3 */
	*(USHORT*)&Buf[70] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[72] = 0x80004400;		/* Write Register Address */
	*(USHORT*)&Buf[76] = PreTrigger;	/* PreTrigger : Data Number(1000) * PreTrigger[%] / 100 */
	*(USHORT*)&Buf[78] = 0x0000;		/* Reserve */
	*(ULONG*)&Buf[80] = 0x80004402;		/* Write Register Address */
	*(USHORT*)&Buf[84] = TrigCondition;	/* Trigger Condition */
	*(USHORT*)&Buf[86] = 0x0000;		/* Reserve */
	SvifSendMemobusMsg( Buf, 88 );
}

void	GetMotorAmpParameter( USHORT *MotPrmBuf, USHORT *AmpPrmBuf, UCHAR axno )
{
	AXIS_HANDLE		*AxHdl;
	AxHdl = &(AxisRscI[axno]);

	MotPrmBuf[0] = AxHdl->UniPrms->Prm->MencP.irat;
	MotPrmBuf[1] = AxHdl->UniPrms->Prm->MencP.imax;
	MotPrmBuf[2] = AxHdl->UniPrms->Prm->MencP.motl;
	MotPrmBuf[3] = AxHdl->UniPrms->Prm->MencP.motr;
	MotPrmBuf[4] = AxHdl->UniPrms->Prm->MencP.emfcmp;
	MotPrmBuf[5] = AxHdl->UniPrms->Prm->MencP.oslv_pol.b.h;
	MotPrmBuf[6] = AxHdl->UniPrms->Prm->MencP.jmot;
	MotPrmBuf[7] = AxHdl->UniPrms->Prm->MencP.exp_trat_jmot.b.h;

	AmpPrmBuf[0] = AxHdl->UniPrms->Prm->imaxs;
	AmpPrmBuf[1] = AxHdl->UniPrms->Prm->shant;

}
/***************************************** end of file **********************************************/
