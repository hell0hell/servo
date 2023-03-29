/****************************************************************************
Description	: Motor Less Test Mode
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	-- APIs --
	 - ���[�^���X�e�X�g -
	ApxMotorLessTestModeMechaModel()	: ���[�^���X�e�X�g���[�h���J���f�����Z
	PcalMotorLessTestMode()				: ���[�^���X�e�X�g���[�h�p�����[�^�v�Z
	ApxInitMotorLessTestMode()			: ���[�^���X�e�X�g���[�h�ϐ�������
	ApiSetCurDatasMotorLessTestMode()	: ���[�^���X�e�X�g���[�h�d���f�[�^�ݒ�
	ApiSetMechaDatasMotorLessTestMode()	: ���[�^���X�e�X�g���[�h���x�E�ʒu�f�[�^�ݒ�
	ApiReadMotPrmMotorLessTestMode()	: ���[�^���X�e�X�g���[�h�p���[�^�p�����[�^�v�Z�F�G���R�[�_���ڑ���
	ApiReadFencPrmMotorLessTestMode()	: ���[�^���X�e�X�g���[�h�p���[�^�p�����[�^�v�Z�F�t���N�G���R�[�_���ڑ���

	 - ���[�^���X�e�X�g���[�h -
	MltSetMotorLessTestMode()			: ���[�^���X�e�X�g���[�h�t���O�ݒ�

	- ���[�^���X�e�X�g���[�hIF��`�A�h���X�擾(���󖢎g�p) -
	MltGetMotorLessTestMode()			: ���[�^���X�e�X�g���[�h�X�e�[�^�X�擾
	MltGetMotorLessTestHandle()			: ���[�^���X�e�X�g���[�hIF��`�A�h���X�擾

----------------------------------------------------------------------------
	 - �Z�~�N���[�Y�h�G���R�[�_�֘A - (SencScanService.c�ɑ���)
	LpxSencMotLessInitVariables()		: �G���R�[�_�ϐ�����������
	RmxReadMotorLessPosition()			: ��]�^�ʒu�f�[�^�Ǎ���
	LmxReadMotorLessPosition()			: ���j�A�ʒu�f�[�^�Ǎ���
	LpxSetSencInfoMotorLessTestMode()	: �G���R�[�_�p�����[�^�ݒ菈��
	LpxGetSencMotLessMturnData()		: �}���`�^�[���f�[�^�擾����
	LpxSencMotLessMultiTurnSet()		: �}���`�^�[���f�[�^�ݒ菈��

	 - �t���N���[�Y�h�G���R�[�_�֘A - (SencScanService.c�ɑ���)
	LpxFencMotLessInitVariables()		: �G���R�[�_�ϐ�����������
	LpxSetFencInfoMotorLessTestMode()	: �G���R�[�_�p�����[�^�ݒ菈��
	LpxReadMotorLessFencPosition()		: �ʒu�f�[�^�Ǎ���

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.11.11		created	<S00X>

****************************************************************************/
#include "Basedef.h"
#include "config.h"
#include "MercuryGlobals.h"
#include "BaseControlPrmCalc.h"
#include "MotorLessTest.h"
#include "KnlApi.h"
#include "Mlib.h"

/*==========================================================================*/
/* defines																	*/
/*==========================================================================*/
#define	MDLCYCLE		KPI_SACYCLENS			/* ���J���f�����Z�������(���xٰ�߂Ɠ�������)	[ns]*/
#define	C10POW9			1000000000				/* 10^9												*/
#define	C10POW7			10000000				/* 10^7												*/
#define	C2PAIE7			62831853				/* (2*PAI) * 10^7									*/


/*==========================================================================*/
/* PROTOTYPE																*/
/*==========================================================================*/


/*==========================================================================*/
/* PROGRAM																	*/
/*==========================================================================*/

/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h���J���f�����Z
	���͂��ꂽ�g���N�w�߂ɑ΂��鍄�̃��J���f���̑��x�A�ʒu���������Z����

	MotorLessMdl.V.MotMdlSpdFbk		�F���x�e�a				[2^24/OvrSpd]
	MotorLessMdl.V.MotMdldPosFbk	�F���[�^�ʒu�e�a����	[pluse/scan]
	MotorLessMdl.V.MotMdlPosFbk		�F���[�^�ʒu�e�a		[pluse]
	MotorLessMdl.V.MechaMdldPosFbk	�F�ʒu�e�a����			[pluse/scan]
	MotorLessMdl.V.MechaMdlPosFbk	�F�ʒu�e�a				[pluse]
----------------------------------------------------------------------------
  Parameters:
	TrqRef;					�g���N�w��	[2^24/MaxTrq]
	BeComplete;				�x�[�X�C�l�[�u���������
	MotorLessTest;			���[�^���X�e�X�g���[�hIF��`�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void ApxMotorLessTestModeMechaModel( LONG TrqRef, BOOL BeComplete, MOTLESSTST *MotorLessTest )
{
	MOTLESSMDL	*MotorLessMdl;		/* �������J���f���p�f�[�^�ւ̃|�C���^	*/
	MOTLESSMDLP	*MotorLessMdl_P;	/* �L���ȃp�����[�^�ւ̃|�C���^			*/

	LONG		MotSpd;				/* ���[�^���x							*/
	LONG		dMotPos;			/* ���[�^�ʒu�e�a����					*/
	LONG		dFbPos;				/* �ʒu�e�a����							*/
	LONG		MotAcc;				/* �����x�e�a							*/

	/* �ϐ�������	*/
	MotorLessMdl	= &(MotorLessTest->MotorLessMdl);
	MotorLessMdl_P	= MotorLessMdl->PrmPtr;

	/*----------------------------------------------------------------------*/
	/*	�x�[�X�u���b�N���̏���												*/
	/*----------------------------------------------------------------------*/
	if( BeComplete == FALSE )
	{
		MotorLessMdl->V.MotMdlpfbrem = 0;
		MotorLessMdl->V.MotMdlpfbremM = 0;
		MotorLessMdl->V.Mvar = 0;
		MotorLessMdl->V.MotMdlSfbrem = 0;
	}

	/*----------------------------------------------------------------------*/
	/*	���̃��J���f��														*/
	/*----------------------------------------------------------------------*/
	/* �����x�e�a	*/
	MotAcc = MlibPfbkxremNolim( TrqRef, MotorLessMdl_P->MdlKj, &MotorLessMdl->V.MotMdlSfbrem );
	/* ���x�e�a		*/
	MotorLessMdl->V.Mvar = MlibLimitul( (MotorLessMdl->V.Mvar + MotAcc), 0x2000000, -0x2000000 );
	MotSpd = MotorLessMdl->V.Mvar;

	/* ���[�^�ʒu�e�a���� �� �t���N���̓t���N�ʒu������Ԃ�		*/
	dFbPos = MlibPfbkxremNolim( MotorLessMdl->V.Mvar, MotorLessMdl_P->MdlKvp, &MotorLessMdl->V.MotMdlpfbrem );

	/* ���[�^�ʒu�e�a���� �� �t���N���ł����[�^�ʒu������Ԃ�	*/
	dMotPos = MlibPfbkxremNolim( MotorLessMdl->V.Mvar, MotorLessMdl_P->MdlKvpM, &MotorLessMdl->V.MotMdlpfbremM );

	/*----------------------------------------------------------------------*/
	/*	���x�A�ʒu�e�a�쐬													*/
	/*----------------------------------------------------------------------*/
	MotorLessMdl->V.MotMdlSpdFbk = MotSpd;		/* ���x�e�a				[2^24/OvrSpd]	*/
	MotorLessMdl->V.MotMdldPosFbk = dMotPos;	/* ���[�^�ʒu�e�a����	[pluse/scan]	*/
	MotorLessMdl->V.MotMdlPosFbk += dMotPos;	/* ���[�^�ʒu�e�a		[pluse]			*/
	MotorLessMdl->V.MechaMdldPosFbk = dFbPos;	/* �ʒu�e�a����			[pluse/scan]	*/
	MotorLessMdl->V.MechaMdlPosFbk += dFbPos;	/* �ʒu�e�a				[pluse]			*/

	/*----------------------------------------------------------------------*/
	/*	���j�^�p�ϐ��o��													*/
	/*----------------------------------------------------------------------*/
//[org]	BoutV.AmonMotorLessTestMdlSpd = MotorLessMdl->V.MotMdlSpdFbk;

	return;

}


/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�p�����[�^�v�Z
----------------------------------------------------------------------------
  Parameters:
	Fenc;					�t���N�G���R�[�_�f�[�^�ւ̃|�C���^
	Bprm;					��{�p�����[�^��`�ւ̃|�C���^
	Prm;					�p�����[�^��`�ւ̃|�C���^
	MOTLESSMDL;				�������J���f���p�f�[�^�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void PcalMotorLessTestMode( MENCV *FencV, BPRMDAT *Bprm, PRMDATA *Prm, MOTLESSMDL *MotorLessMdl )
{

	LONG		sx;						/* �Q�C���v�Z�p���[�N				*/
	LONG		kx;						/* �Q�C���v�Z�p���[�N				*/
	LONG		index;					/* �ҏW���p�����[�^�C���f�b�N�X		*/
	MOTLESSMDLP	*wrkp;					/* �ҏW���p�����[�^�ւ̃|�C���^		*/

	/* �X�V���鑤�̃C���f�b�N�X���擾 */
	index = 0;
	if( MotorLessMdl->PrmPtr == &(MotorLessMdl->P[index]) )
	{
		index = 1;
	}

	/* �ҏW���p�����[�^�A�h���X�̎擾	*/
	wrkp = &(MotorLessMdl->P[index]);

	/*----------------------------------------------------------------------*/
	/*	���f�����Z	: NormalizedSpeed -> Pulse/Scan							*/
	/*----------------------------------------------------------------------*/
	/*																		*/
	/*			  scantime[ns]												*/
	/*	MdlKvp = -----------------											*/
	/*			 Hprm.Kpx * 10^9  											*/
	/*																		*/
	/*----------------------------------------------------------------------*/
	wrkp->MdlKvp = MlibScalKxkxks( 1, 1, Bprm->Kpx, &sx,  0 );
	wrkp->MdlKvp = MlibPcalKxgain( wrkp->MdlKvp, MDLCYCLE, C10POW9, &sx, 24 );

	/*--------------------------------------------------------------------------------------*/
	/*	���[�^���f�����Z	: NormalizedSpeed -> Pulse/Scan									*/
	/*--------------------------------------------------------------------------------------*/
	/*																						*/
	/*		  	    scantime[ns]		Bprm.MotPulse										*/
	/*	MdlKvpM = -----------------	* ------------------	Bprm.MotPulse	:[pulse/rev]	*/
	/*			   Hprm.Kpx * 10^9  	     kx 			FencP.MotPulseNo:[pulse/rev]	*/
	/*																						*/
	/*										kx : Full-Closed -> FencP.MotPulseNo			*/
	/*											 Semi-Closed -> Bprm.MotPulse				*/
	/*--------------------------------------------------------------------------------------*/
	if( Bprm->FencUse )
	{
		kx = FencV->PulseNo;							/* Full-Closed Control				*/
	}
	else
	{
		kx = Bprm->MotPulse;							/* Semi-Closed Control				*/
	}
	wrkp->MdlKvpM = MlibScalKxkxks( 1, 1, Bprm->Kpx, &sx,  0 );
	wrkp->MdlKvpM = MlibPcalKxgain( wrkp->MdlKvpM, Bprm->MotPulse, kx, &sx,  0 );
	wrkp->MdlKvpM = MlibPcalKxgain( wrkp->MdlKvpM, MDLCYCLE, C10POW9, &sx, 24 );

	/*----------------------------------------------------------------------*/
	/*	���f�����Z	:  NormalizedObsJGain									*/
	/*----------------------------------------------------------------------*/
	wrkp->MdlKj = BpxNorObsJGainCal( Bprm->Kvx, Prm->jrate, 100 , MDLCYCLE );

	/*----------------------------------------------------------------------*/
	/*	Set Parameters														*/
	/*----------------------------------------------------------------------*/
	/* �Q�Ɨp�|�C���^���X�V���� */
	MotorLessMdl->PrmPtr = &(MotorLessMdl->P[index]);

	return;
}


/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�ϐ�������
----------------------------------------------------------------------------
  Parameters:
	AxisRsc;				���n���h��
  Return:
	none
----------------------------------------------------------------------------
  Note:
	�b���
*****************************************************************************/
void ApxInitMotorLessTestMode( AXIS_HANDLE *AxisRsc )
{
	MOTLESSTST	*MotorLessTest;		/* ���[�^���X�e�X�g���[�hIF��`�ւ̃|�C���^	*/
	MOTLESSMDL	*MotorLessMdl;		/* �������J���f���p�f�[�^�ւ̃|�C���^		*/

	/* �ϐ������� */
	MotorLessTest	= AxisRsc->MotorLessTest;
	MotorLessMdl 	= &(MotorLessTest->MotorLessMdl);

	/* API�̈����Ŏw�肪����ȃf�[�^�ւ̃|�C���^�̏�����	*/
	MotorLessTest->MencV	= AxisRsc->MencV;
	MotorLessTest->FencV	= AxisRsc->FencV;
	MotorLessTest->Prm		= AxisRsc->UniPrms->Prm;

	/* �̈揉���� */
	MlibResetLongMemory( &(MotorLessMdl->V), sizeof(MotorLessMdl->V)/4 );

	return;
}


/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�d���f�[�^�ݒ�
	�d���f�[�^���e�a�f�[�^�ɑ������
----------------------------------------------------------------------------
  Parameters:
	BaseLoops;				���䃋�[�v���Z�p�f�[�^�ւ̃|�C���^
	CtrlLoopOut;			ScanA�o�̓f�[�^�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void ApiSetCurDatasMotorLessTestMode( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut )
{
	/*----------------------------------------------------------------------*/
	/*	�d���w�߁A�e�a														*/
	/*----------------------------------------------------------------------*/
	/* �����d���w��			[15000/Imax]	*/
	CtrlLoopOut->IqRefMon = MlibMulhigh32( BaseLoops->TrqRefoLimit, 3840000 );
	/* �����d���w��			[15000/Imax]	*/
	CtrlLoopOut->IdRefMon = 0;

	CtrlLoopOut->IqRefMon_a = ( CtrlLoopOut->IqRefMon + CtrlLoopOut->IqRefMon_l ) >> 1;
	CtrlLoopOut->IqRefMon_l = CtrlLoopOut->IqRefMon;

	/* �����d���e�a = �����d���w��			*/
	CtrlLoopOut->IqFbMon = CtrlLoopOut->IqRefMon;
	/* �����d���e�a = �����d���w��			*/
	CtrlLoopOut->IdFbMon = CtrlLoopOut->IdRefMon;

	return;
}


/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h���x�E�ʒu�f�[�^�ݒ�
	���J���f���f�[�^���e�a�f�[�^�ɑ������
----------------------------------------------------------------------------
  Parameters:
	Bprm;					��{�p�����[�^��`�ւ̃|�C���^
	MotorLessTest;			���[�^���X�e�X�g���[�hIF��`�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void ApiSetMechaDatasMotorLessTestMode( BPRMDAT *Bprm, MOTLESSTST *MotorLessTest )
{
	MENCV		*MencV;					/* ���[�^�G���R�[�_�f�[�^�ւ̃|�C���^	*/
	MENCV		*FencV;					/* �t���N�G���R�[�_�f�[�^�ւ̃|�C���^	*/
	MOTLESSMDL	*MotorLessMdl;			/* �������J���f���p�f�[�^�ւ̃|�C���^	*/
	MOTOR_LESS	*MotorLessTestDatas;	/* �G���R�[�_IF�p�f�[�^�ւ̃|�C���^		*/

	/* �ϐ�������	*/
	MencV				= MotorLessTest->MencV;
	FencV				= MotorLessTest->FencV;
	MotorLessMdl		= &(MotorLessTest->MotorLessMdl);
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);

	/*----------------------------------------------------------------------*/
	/*	���[�^�ʒu�e�a����													*/
	/*----------------------------------------------------------------------*/
	if( MencV->RevAsmMotor ^ Bprm->RvsDir )	/* ���]�ڑ� XOR �t��]���[�h	*/
	{
		MotorLessTestDatas->dSemiModePos =-MotorLessMdl->V.MotMdldPosFbk;
	}
	else
	{
		MotorLessTestDatas->dSemiModePos = MotorLessMdl->V.MotMdldPosFbk;
	}

	/*----------------------------------------------------------------------*/
	/*	�t���N�ʒu�e�a����	-->		���[�^�ʒu�e�a�����Ɠ���				*/
	/*----------------------------------------------------------------------*/
	if( FencV->RevAsmMotor ^ Bprm->RvsDir )	/* Fenc�t��]���[�h XOR �t��]���[�h	*/
	{
		MotorLessTestDatas->dFullModePos =-MotorLessMdl->V.MechaMdldPosFbk;
	}
	else
	{
		MotorLessTestDatas->dFullModePos = MotorLessMdl->V.MechaMdldPosFbk;
	}

	return;

}


/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�p���[�^�p�����[�^�v�Z
	�G���R�[�_���ڑ������z���[�^�p�����[�^��PnFxx�ɓǂݍ���
----------------------------------------------------------------------------
  Parameters:
	MencV;					���[�^�G���R�[�_�f�[�^�ւ̃|�C���^
	Prm;					�p�����[�^��`�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void ApiReadMotPrmMotorLessTestMode( MENCV *MencV, PRMDATA *Prm )
{
	LONG	kx;						/* �Q�C���v�Z�p���[�N					*/
	LONG	sx;						/* �Q�C���v�Z�p���[�N					*/
	USHORT	ivcode;					/* ���͓d���R�[�h						*/
	USHORT	EncBitNo;				/* �G���R�[�_�r�b�g��					*/
	USHORT	EncType;				/* �G���R�[�_�^�C�v						*/
	MENCPRM	*MencP;					/* �G���R�[�_�p�����[�^��`�ւ̃|�C���^	*/

	/* �ϐ�������	*/
	MencP = &(Prm->MencP);

	/****************************************************************************/
	/*	PnF00 : ���[�^�`��(8bit), ���͓d��(4bit), �G���R�[�_�^�C�v(4bit) [�|]	*/
	/****************************************************************************/

	/* ���[�^�`��	*/
	MencP->typm_e.b.l = 0xFF;						/* ����`					*/

	/* ���͓d��	*/
	ivcode = ( Prm->styp.b.h & 0xFF );				/* PnE11.H					*/
	if( ivcode == 0x05 )	ivcode = 0x01;			/* 100V�{�d����200V�ɂ��� 	*/

	/* �G���R�[�_�^�C�v	*/
	switch((Prm->b_prmC >> 8) & 0x0F)				/* Pn00C.2					*/
	{
		case 0x00:
		default :
				EncType = 0;						/* �C���N�������^��			*/
			break;
		case 0x01:
				EncType = 1;						/* �A�u�\�����[�g			*/
			break;
	}

	MencP->typm_e.b.h = ivcode + (EncType << 4);

	/****************************************************************************/
	/*	PnF01 : �G���R�[�_�\�t�g�o�[�W���� [�|]									*/
	/****************************************************************************/
	MencP->encver = 0x0000;

	/****************************************************************************/
	/*	PnF02 : ���[�^�e�� [W]													*/
	/****************************************************************************/
	MencP->motw = Prm->ampw;						/* = PnE12					*/

	/****************************************************************************/
	/*	PnF03 : �G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu [�|]					*/
	/****************************************************************************/
/* <S1BB> >>>>> */
	/* Pn000.3�̒l�Ń��[�^���X�e�X�g���[�h���̉�]�`�^���j�A�����肵�G���R�[�_�����X�V���� */
	switch( (Prm->b_prm0 >> 12) & 0xF )				/* Pn000.3					*/
	{
	case 0: /* ��]�` */
		/* fall through */
	default:
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	case 1:	/* ���j�A */
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	}
/* <<<<< <S1BB> */
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		MencP->bit_dp.b.l = 0x08;
		MencP->bit_dp.b.h = 0x44;					/* 68���ڌŒ�				*/
	}
	else
	{
		/* �G���R�[�_����\	*/
		switch((Prm->b_prmC >> 4) & 0x0F)			/* Pn00C.1					*/
		{
			case 0x00:	
					EncBitNo = 13;					/* 13bit					*/
				break;
			case 0x01:
					EncBitNo = 20;					/* 20bit					*/
				break;
			case 0x02:
					EncBitNo = 22;					/* 22bit					*/
				break;
			case 0x03:
			default :
					EncBitNo = 24;					/* 24bit					*/
				break;
		}

		MencP->bit_dp.b.l = EncBitNo;
		MencP->bit_dp.b.h = 0x50;					/* 80���ڌŒ�				*/
	}

	/****************************************************************************/
	/*	PnF04 : �}���`�^�[�����~�b�g [Rev]										*/
	/****************************************************************************/
	MencP->limmlt = Prm->mtlmt;						/* = Pn205					*/

	/****************************************************************************/
	/*	PnF05 : ��i���x, �ő呬�x [100min-1]									*/
	/****************************************************************************/
	MencP->vrat_max.w = Prm->MLess_vrat_max.w;		/* = PnEE0					*/

	/****************************************************************************/
	/*	PnF06 : �n�r���o���x��, �|�[���� [%, -]									*/
	/****************************************************************************/
	MencP->oslv_pol.w = Prm->MLess_oslv_pol.w;		/* = PnEE1					*/

	/****************************************************************************/
	/*	PnF07 : ��i�g���N [0.01Nm]												*/
	/*																			*/
	/*						Ӱ��e��[W] * 60			���[�^�e��:PnF02(=PnE12)	*/
	/*		��i�ٸ[Nm] = -----------------------	��i���x  :PnF05.L(=PnEE0.L)*/
	/*						2�� * ��i���x[min-1]								*/
	/*																			*/
	/****************************************************************************/
	kx = MlibScalKxgain( (LONG)MencP->motw, (60*C10POW7),  C2PAIE7, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencP->vrat_max.b.l, &sx, -24 );

	MencP->rattrq = MlibLimitul( kx, 0xFFFF, 0x0000 );

	/****************************************************************************/
	/*	PnF08 : �ő�g���N [%]													*/
	/*																			*/
	/*		�ő��ٸ[%] = �ő�d�� * 100 / ��i�d��		�ő�d��:PnE15			*/
	/*													��i�d��:PnE14			*/
	/****************************************************************************/
	MencP->maxtrq = (((Prm->imaxs * 200)/Prm->irats) + 1) >> 1;

	/****************************************************************************/
	/*	PnF09 : ��i�d�� [0.1A]													*/
	/****************************************************************************/
	MencP->irat = Prm->irats;						/* PnF09 = PnE14			*/

	/****************************************************************************/
	/*	PnF0A : �u���ő�d�� [0.1A]												*/
	/****************************************************************************/
	MencP->imax = Prm->imaxs;						/* PnF0A = PnE15			*/

	/****************************************************************************/
	/*	PnF0B : �d�l�e�萔 [0.1mV/min-1]										*/
	/****************************************************************************/
	MencP->emfcmp = 100;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF0C : ���[�^�C�i�[�V�� [10-6kgm2]										*/
	/****************************************************************************/
	MencP->jmot = Prm->Mless_jmot;					/* PnF0C = PnEE2			*/

	/****************************************************************************/
	/*	PnF0D : �d�@�q������R [0.001��]										*/
	/****************************************************************************/
	MencP->motr = 1000;								/* ���g�p					*/

	/****************************************************************************/
	/*	PnF0E : �d�@�q�C���_�N�^���X [0.01mH]									*/
	/****************************************************************************/
	MencP->motl = 1000;								/* ���g�p					*/

	/****************************************************************************/
	/*	PnF0F : �ߕ��׌��o�x�[�X�d�� [%]										*/
	/****************************************************************************/
	MencP->bastrq = Prm->ibs_md.b.l;				/* PnF0F = PnE17.l			*/

	/****************************************************************************/
	/*	PnF10 : �ߕ��׌��o���ԓd�� [%]											*/
	/****************************************************************************/
	MencP->midtrq = Prm->ibs_md.b.h * 10;			/* PnF10 = PnE17.h			*/

	/****************************************************************************/
	/*	PnF11 : �ߕ��׌��o���Ԏ��� [10s]										*/
	/****************************************************************************/
	MencP->midolt = Prm->imdt_mxt.b.l;				/* PnF11 = PnE18.l			*/

	/****************************************************************************/
	/*	PnF12 : �ߕ��׌��o���ԓd���Q [%]										*/
	/****************************************************************************/
													/* PnF12 = PnE61.h	* imaxs/irats	*/
	kx = (LONG)Prm->ratmtt_2.b.h * (LONG)MencP->maxtrq / 100;
	MencP->mtrq2 = (USHORT)kx;

	/****************************************************************************/
	/*	PnF13 : �ߕ��׌��o���Ԏ��ԂQ [s]										*/
	/****************************************************************************/
	MencP->mtolt2 = Prm->imdt_mxt.b.h;				/* PnF13 = PnE18.h			*/

	/****************************************************************************/
	/*	PnF14 : �ʑ��⏞�P(�⏞�l,�J�n���x) [deg,100min-1]						*/
	/****************************************************************************/
	MencP->phscmpM1.w = 0;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF15 : �|�[���s�b�` [0.1mm/180deg]										*/
	/****************************************************************************/
	MencP->ppitch = 225;							/* 22.5mm�Œ�				*/

	/****************************************************************************/
	/*	PnF16 : q���C���_�N�^���XLq0 [0.01mH]									*/
	/****************************************************************************/
	MencP->motlq0 = 1000;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF17 : q���C���_�N�^���XLq1 [0.01mH]									*/
	/****************************************************************************/
	MencP->motlq1 = 1000;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF18 : ���ʁF��i�g���N�w���A��ʁF���[�^�C�i�[�V���w�� [10n]			*/
	/****************************************************************************/
	MencP->exp_trat_jmot.b.l = 0;					/* ���g�p					*/
													/* PnF18.h = PnEE3.h		*/
	MencP->exp_trat_jmot.b.h = Prm->Mless_exp_spd_jmot.b.h;

	/****************************************************************************/
	/*	PnF19 : ���ʁF��i�o�͎w���A��ʁF��]���w�� [10n]						*/
	/****************************************************************************/
	MencP->exp_motw_spd.b.l = 0;					/* ���g�p					*/
													/* PnF19.h = PnEE3.l		*/
	MencP->exp_motw_spd.b.h = Prm->Mless_exp_spd_jmot.b.l;

	/****************************************************************************/
	/*	PnF1A : �t���N�G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu [�|]				*/
	/****************************************************************************/
	/* �����ł͐ݒ肵�Ȃ� */

	/****************************************************************************/
	/*	PnF1B : �|�[���Z���T�I�t�Z�b�g�� [deg]									*/
	/****************************************************************************/
	MencP->poleoffset = 0;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF1C : ���[�^���U�}���p�m�b�`�t�B���^���g�� [Hz]						*/
	/****************************************************************************/
	MencP->motresfrq = 0;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF1D : �ʑ��p�␳�l [0.01deg]											*/
	/****************************************************************************/
	MencP->phscmpM2 = 0;							/* ���g�p					*/

	/****************************************************************************/
	/*	PnF1E : �t���O [-]														*/
	/****************************************************************************/
	MencP->flg_wf = 0;								/* ���g�p					*/

}


/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�p�t���N�p�����[�^�v�Z
	�t���N�G���R�[�_���ڑ������z�G���R�[�_�p�����[�^��PnFxx�ɓǂݍ���
----------------------------------------------------------------------------
  Parameters:
	MencP;					�G���R�[�_�p�����[�^��`�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void ApiReadFencPrmMotorLessTestMode( MENCPRM *MencP )
{

	/*	PnF1A : �t���N�G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu [�|]			*/
	MencP->BitnoDpoint.b.l = 8;				/* �Wbit�Œ�					*/
	MencP->BitnoDpoint.b.h = 0x44;			/* �U�W���ڌŒ�					*/

	return;
}

#if 0	// ���󖢎g�p
/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�X�e�[�^�X�擾
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	TRUE;					���[�^���X�e�X�g���[�h
	FALSE;					�ʏ탂�[�h
----------------------------------------------------------------------------
  Note:
	�b���
*****************************************************************************/
BOOL MltGetMotorLessTestMode( void )
{
	AXIS_HANDLE		*AxisRsc;			/* �����	*/
	MOTLESSTST		*MotorLessTest;

	/* ���n���h�����擾���� */
	/* TODO:�������Ή����K�v */
	AxisRsc = GetAxisHandle(0);
	MotorLessTest = AxisRsc->MotorLessTest;

	/* ���[�h�X�e�[�^�X��Ԃ� */
	return( MotorLessTest->MotorLessTestMode );
}
#endif

/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�h�t���O�ݒ�
----------------------------------------------------------------------------
  Parameters:
	AxisRscI;					���n���h��(�S��)
	AxisRscNum;					����
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void MltSetMotorLessTestMode( AXIS_HANDLE *AxisRscI, INT AxisRscNum )
{
	AXIS_HANDLE 	*AxisRsc;
	PRMDATA 		*Prm;
	MOTLESSTST		*MotorLessTest;

	INT	 			ax_no;
	BOOL			flag;

	/*----------------------------------------------------------------------*/
	/*	���[�^���X�e�X�g���[�h�ݒ�											*/
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		Prm = AxisRsc->UniPrms->Prm;
		MotorLessTest = AxisRsc->MotorLessTest;

		/* ���[�^���X�e�X�g���[�h */
		flag = FALSE;
		if( Prm->b_prmC & 0x0001)
		{
			flag = TRUE;
		}
		MotorLessTest->MotorLessTestMode = flag;

		/* �t���N���[�Y�h�G���R�[�_�^�C�v */
		flag = FALSE;
		if( Prm->b_prmC & 0x0100)
		{
			flag = TRUE;
		}
		MotorLessTest->MotorLessTestDatas.FencType = flag;
	}

	return;
}


#if 0	// ���󖢎g�p
/*****************************************************************************
  Description : ���[�^���X�e�X�g���[�hIF��`�A�h���X�擾
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	���[�^���X�e�X�g���[�hIF��`�̃A�h���X
----------------------------------------------------------------------------
  Note:
	�b���
*****************************************************************************/
MOTLESSTST *MltGetMotorLessTestHandle( void )
{
	AXIS_HANDLE		*AxisRsc;			/* �����	*/

	/* ���n���h�����擾���� */
	/* TODO:�������Ή����K�v */
	AxisRsc = GetAxisHandle(0);

	/* ���[�^���X�e�X�g�\���̂̃A�h���X��Ԃ� */
	return( AxisRsc->MotorLessTest );
}
#endif

/******************************* end of file  *******************************/
