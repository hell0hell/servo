/****************************************************************************
Description	: Serial Encoder Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes	      :
	Name   Date          Description
	------------------------------------------------------------------------
	Y.Oka  2010.02.23    created
	I.M.   2011.02.08    reviewed

****************************************************************************/
#ifndef SERIAL_ENC_IF_H
#define SERIAL_ENC_IF_H


#include "EncData.h"
#include "JL086ServoIpHw.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"
#include "AlarmManager.h"		/* <S018> */
#include "SvOptCardIf.h"		/* <S04D> */

#define ECHO_DATA_NUM	7
#define	ENCODER_WAIT_TIMEOUT	2000	/* 2000[ms](2[s]) *//* <S223> */

/****************************************************************************************************/
/*		Encoder Error Definition																	*/
/****************************************************************************************************/
typedef enum SENC_ERROR_ {
	SENCERR_SUCCESS		= 0x0000,		/* �߂�l�F�ʐM����											*/
	SENCERR_BUSY		= 0x0001,		/* �߂�l�F������											*/
	/*----------------------------------------------------------------------------------------------*/
	SENCERR_PGCOM		= 0x0002,		/* �߂�l�FA.C90 : �G���R�[�_�ʐM�ُ�						*/
	SENCERR_PGECHO		= 0x0004,		/* �߂�l�FA.CB0 : �G���R�[�_�G�R�[�o�b�N�ُ�				*/
	SENCERR_PRMDATA		= 0x0008,		/* �߂�l�FA.CA0 : �G���R�[�_�p�����[�^�ُ�					*/
	SENCERR_PRMSUM		= 0x0010,		/* �߂�l�FA.820 : �G���R�[�_�T���`�F�b�N�A���[��			*/
	SENCERR_MTLMT		= 0x0020,		/* �߂�l�FA.CC0 : �}���`�^�[�����~�b�g�l�s��v				*/
	SENCERR_SCALE		= 0x0040,		/* �߂�l�FA.080 : �X�P�[���s�b�`�ݒ�ُ�					*/
	SENCERR_PHASEANGLE	= 0x0080,		/* �߂�l�FA.C22 : �ʑ����s��v							*/
	SENCERR_POLESENSOR	= 0x0100,		/* �߂�l�FA.C21 : �|�[���Z���T�ُ�							*/
	SENCERR_BRCHNGFAIL	= 0x0200,		/* �߂�l�FA.C93 : �G���R�[�_�ʐM�V�X�e���ُ�				*/
	SENCERR_PGCLR		= 0x0400,		/* �߂�l�FA.C80 : �G���R�[�_�N���A�ُ�						*//* <S1BE> */
	SENCERR_NOTSUPPORT	= 0x0800,		/* �߂�l�FA.051 : ���i���T�|�[�g�A���[��					*//* <S1BE> */
	/*----------------------------------------------------------------------------------------------*/
} SENC_ERROR;



 /* <S003> */
/****************************************************************************************************/
/*		Baud Rate Exchange Selection																*/
/****************************************************************************************************/
typedef enum SENC_BRCHNG_SUP_ {
	SENCBR_AUTO_SEL		= 0x0000,		/* �G���R�[�_�ʐM���x�؂�ւ��I��							*/
	SENCBR_8MBPS_SEL	= 0x0001,		/* �G���R�[�_�ʐM���x8MBps�I��								*/
	SENCBR_4MBPS_SEL	= 0x0002,		/* �G���R�[�_�ʐM���x4MBps�I��								*/
} SENC_BRCHNG_SUP;
 /* <S003> */
 


/****************************************************************************************************/
/*		Baud Rate Exchange Definition																*/
/****************************************************************************************************/
typedef enum SENC_BRCHNG_SET_ {
	SENCBR_SUPPORT		= 0x0001,		/* �G���R�[�_�ʐM���x�؂�ւ��Ή�							*/
	SENCBR_4MBPS_SUP	= 0x0002,		/* �G���R�[�_�ʐM���x4MBps�Ή�								*/
	SENCBR_8MBPS_SUP	= 0x0004,		/* �G���R�[�_�ʐM���x8MBps�Ή�								*/
} SENC_BRCHNG_SET;


/****************************************************************************************************/
/*		Expand area Use Definition										<S1A6>						*/
/****************************************************************************************************/
typedef enum SENC_EXAREA_SET_ {
	SENCBR_EXAREA1		= 0x0001,		/* �G���R�[�_�g���̈�1�g�p									*/
} SENC_EXAREA_SET;



/****************************************************************************************************/
/*		DivSet Error Definition																		*/
/****************************************************************************************************/
typedef enum DIVSET_ERROR_ {
	DIVSET_SUCCESS = 0,	/* �߂�l�F�����ݒ萳��						*/
	DIVSET_PRMERR_R,	/* �߂�l�F�p�����[�^�ݒ�ُ�(��])�@:A.040	*/
	DIVSET_PRMERR_L,	/* �߂�l�F�p�����[�^�ݒ�ُ�(���j�A):A.040	*/
	DIVSET_DIVERR,		/* �߂�l�F�����p���X�ݒ�ُ�:A.041			*/
} DIVSET_ERROR;

/* API's for Serial Encoder Operations */
/* for scan service */
BOOL LmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq );
BOOL RmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq );
LONG FencReadPosition(MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	/* <S04D> *//* <S0E1>:AlmManager�ǉ� */
						void *BaseLoops );/*<S1B7>*/
/* for initializing Motor encoder *//* <S04D>:FbkOpt,SencFbOptUse�ǉ� */
SENC_ERROR	SencInitMotorEncoder( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,	ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse );
										
SENC_ERROR	SencCalcBaseParameters( MENCV *MencV, MENC_INIT_PRM *MencInitPrm );
SENC_ERROR SencInitMencVariables(MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencInPrm);
void	SencMakeAbsPosition(MENCV *MencV, USHORT limmlt, DBYTEX bit_dp, BOOL RvsDir);
DIVSET_ERROR SencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );
BOOL	SencSetEncoderType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2, BOOL SencFbOptUse ); /* <S16A> */
/*--------------------------------------------------------------------------------------------------*/
/* �V���A���G���R�[�_�o�`�n���V���A���o��												<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void	SencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							 BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag );
/*--------------------------------------------------------------------------------------------------*/
/* �����C���N���p���X�o�͏���															<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void	SencOutIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg );				/* ��]		*/
void	LmxOutSencIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg, UCHAR Sftx );/* ���j�A	*//* <S209> */
/*--------------------------------------------------------------------------------------------------*/


/* for initializing Fully-closed encoder */
SENC_ERROR FencInitEncoder( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,	ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse, BOOL rvsDir);/*<S02D>*//* <S04D> */
void FencBasePrmCalc(MENCV *FencV, MENCPRM *MencP, USHORT b_prm2, ULONG fcplsml, BOOL rvsDir);/*<S02D>*/


SENC_ERROR FencInitVariables(MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg,
                             MENCPRM *MencP, USHORT scnvorgoffset, BOOL OrgOffsetEnable,
                             LONG KConvLoadSpd, LONG sfgfail_b_h);
UCHAR FencGetFencAlarm(MENCV *FencV);
/*--------------------------------------------------------------------------------------------------*/
/* �t���N�G���R�[�_�F�����p���X�h�e������												<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
DIVSET_ERROR FencSencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *FencV, UNI_PRM_STR *UniPrms );
/*--------------------------------------------------------------------------------------------------*/
/* �t���N�G���R�[�_�F�����C���N���p���X�o�͏���											<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void  FencSencOutIniIncPulse( MENCV  *FencV, ASIC_HW_IF_STR  *AsicHwReg );
/*--------------------------------------------------------------------------------------------------*/
/* �t���N�G���R�[�_�F�V���A���G���R�[�_�o�`�n���V���A���o��								<S02D>		*/
/*--------------------------------------------------------------------------------------------------*/
void  FencSencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							   BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag );
/*--------------------------------------------------------------------------------------------------*/

/* common functions(SerialEnc.c) */
void SencInitChannel( MENCV *MencV, UINT ch, ASIC_HW_IF_STR *encAsicHwReg, ASIC_HW_IF_STR *posCmpAsicHwReg );/* <S04D> */
SENC_ERROR SencConnect( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg, USHORT EncBaudrate,  
										OPFBK_HNDL *FbkOpt, BOOL SencFbOptUse );/* <S04D>:FbkOpt,SencFbOptUse�ǉ� */
SENC_ERROR SencExchngBaudrate( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,
										ASIC_HW_IF_STR *AsicHwReg, UCHAR BaudRate, BOOL *EncExchgSupported);/*<S09C>*/
SENC_ERROR SencReadParamAll( MENCV *MencV, UCHAR *PrmBuf );
SENC_ERROR SencSendRstOrgCmd(MENCV *MencV);
SENC_ERROR SencReadInitialP1Data(MENCV *MencV);
void SencChangeP0Mode( MENCV *MencV );
void SencInitChangeP0Mode( MENCV *MencV );			/* <S0F8> */
void SencChangeP1Mode( MENCV *MencV );
void SencSetIntCycleP0Mode( MENCV *MencV );			/* <S1D3> */
void SencSetIntCycleP1Mode( MENCV *MencV );
BOOL SencReadParam8byte( SENCX *Enc, USHORT Addr, UCHAR *PrmBuf );
BOOL SencBankChange( SENCX *Enc, USHORT BankNum );/* <S0D7> */
void SencSetIDINFO( USHORT addr, UCHAR *buffer, IDINFO *idinfo );
void SencFillNullIDINFO( IDINFO *idinfo );

SENC_ERROR SencChkConnect( SENCX *Enc );/* <S04D> */

SENC_ERROR SencResetMultiturn(MENCV *MencV, ULONG limmlt, ULONG bit_dp_l_h);
SENC_ERROR SencChangeMultiturnLimit(MENCV *MencV, USHORT mtlmt);
SENC_ERROR SencResetScaleZeroPosition( MENCV *MencV );
SENC_ERROR SencWriteMotorPhaseOffset( MENCV *MencV, USHORT PhaseOffset );
SENC_ERROR SencReadMotorParameter(MENCV *MencV, USHORT address, UINT num, USHORT *buffer);
SENC_ERROR SencWriteMotorParameter(MENCV *MencV, USHORT address, UINT num, USHORT *buffer);
#if 0 /*<S02D>*/
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV );
#else/*<S02D>*/
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV, PRM_ACCCMD Cmd, LONG Data );
#endif/*<S02D>*/

/* 2012.08.24 Y.Oka Make Mercury Base Software */
void	SencReadPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
											ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops );/* <S018> *//*<S0C5>*//*<S1B7>*/
/* 2012.08.24 Y.Oka Make Mercury Base Software */
LONG	TimeBasesMotSpdCalc( VFBTIMBASE *Vfbtimbase, MENCV *MencV, LONG dPos, LONG dPosSpd,
																	BOOL VcompSts, BOOL EnableFlag ); /* <S067> */
/* <S1B7> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/* ���[�^���X�e�X�g																		<S00X>		*/
/*--------------------------------------------------------------------------------------------------*/
/* --- �Z�~�N���[�Y�h�G���R�[�_ --- */
/* SencScanService.c�ɒ�` */
/* ���[�^���X�F�G���R�[�_�ϐ�����������	*/
void LpxSencMotLessInitVariables( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg );
/* ���[�^���X�F��]�^�ʒu�f�[�^�Ǎ���	*/
void RmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	void *BaseLoops );
/* ���[�^���X�F���j�A�ʒu�f�[�^�Ǎ���	*/
void LmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	void *BaseLoops );
/* ���[�^���X�F�G���R�[�_�p�����[�^�ݒ菈��	*/
void LpxSetSencInfoMotorLessTestMode( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 );

/* --- �t���N���[�Y�h�G���R�[�_ --- */
/* SencScanService.c�ɒ�` */
/* ���[�^���X�F�G���R�[�_�ϐ�����������	*/
void LpxFencMotLessInitVariables( MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg );
/* ���[�^���X�F�G���R�[�_�p�����[�^�ݒ菈��	*/
void LpxSetFencInfoMotorLessTestMode( MENCV *FencV, MENCPRM *MencP );
/* ���[�^���X�F�ʒu�f�[�^�Ǎ���	*/
LONG LpxReadMotorLessFencPosition( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops );
/* <<<<< <S1B7> */


#endif /* SERIAL_ENC_IF */
/***************************************** end of file **********************************************/
