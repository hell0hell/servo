/****************************************************************************
Description	: MECHATROLINK Register and Parameter Access Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.11.22     created

****************************************************************************/
#include "Basedef.h"
#include "RegTable.h"
#include "SysApi.h"
#include "ComCfg.h"
#include "MLib.h"
#include "PrmAccessIf.h"


/* �������W�X�^��񃍁[�h�e�[�u�� */
typedef struct REG_LOAD_TBL_ {
	USHORT	regNo;	/* ���W�X�^�ԍ� */
	UCHAR	size;	/* ���W�X�^�T�C�Y[word] */
	UCHAR	type;	/* �A�N�Z�X�^�C�v */
	INT32	(*LoadFunc)(REG_HNDL *hReg, UNI_PRM_STR *pUniPrm);/* �ݒ菈���֐� */
} REG_LOAD_TBL;

/* Fn���s���W�X�^�A�N�Z�X��` */
typedef struct REG_FNACC_TBL_ {
	ULONG adrs;
	ULONG data;
} REG_FNACC_TBL;
typedef const REG_FNACC_TBL CREG_FNACC_TBL;


/* local functions */
static	PRM_RSLT regRunRead( REG_HNDL *hReg );
static	PRM_RSLT regRunWrite( REG_HNDL *hReg );
static	PRM_RSLT regRunIDRead( REG_HNDL *hReg );
static	PRM_RSLT regRunFnMode( REG_HNDL *hReg, CREG_FNACC_TBL *FnRegs, UINT regNum );
static	PRM_RSLT regRunCalcCycPrm( REG_HNDL *hReg );
static	INT32 regSetAmpInfo( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetABSEncMode( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetPGPls( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetFullcScaleType( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetTrefAndABSMode( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetFullcPls( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetEGearB( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetEGearA( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetMaxTrq( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetOverSpd( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetOverSpdExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetMaxSpd( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetMaxSpdExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetScalePitch( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetScalePitchExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetRatSpd( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetRatTrq( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetRatTrqExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regComPrmSetzretSpd1( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regComPrmSetzretSpd2( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );

#if 0 /* M2�v���t�@�C���Ή� */
static	INT32 regChkInSig50A( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regChkInSig50C( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regChkInSig50D( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
#endif
static	INT32 regInitOutSigEnable( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );
static	INT32 regSetOutSignEnable1( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm ); /* <S027> */
static	INT32 regSetOutSignEnable2( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm ); /* <S027> */
static	INT32 regSetOutSignEnable3( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm ); /* <S1C0> */


/****************************************************************************
 TABLE DEFINITION
****************************************************************************/
/* �������W�X�^��񃍁[�h�e�[�u�� */
const REG_LOAD_TBL regLoadTbl[] = {
	{0xE703, 1, COMMON_TYPE				, regSetAmpInfo},		/* �T�[�{������� */
	{0xC00D, 1, COMMON_TYPE | REG_1WDACC, regSetABSEncMode},	/* �G���R�[�_ID/����\ */
	{0xC028, 1, FULLC_ONLY				, regSetFullcScaleType},/* �t���NPG Type */
	{0x0002, 1, COMMON_TYPE				, regSetTrefAndABSMode},/* �g���N�w�߃��[�h,ABS���[�h */
	{0xC022, 2, COMMON_TYPE | REG_1WDACC, regSetPGPls},			/* �G���R�[�_�p���X�� */
	{0x020A, 2, FULLC_ONLY				, regSetFullcPls},		/* �t���NPG[pitch/rev] */
	{0x020E, 2, COMMON_TYPE				, regSetEGearB},		/* �d�q�M�AB */
	{0x0210, 2, COMMON_TYPE				, regSetEGearA},		/* �d�q�M�AA */
	{0xE701, 1, COMMON_TYPE				, regSetMaxTrq},		/* �ő�g���N[%] */
	{0xC021, 1, COMMON_TYPE				, regSetRatTrqExp},		/* ���[�^�g���N�w�� */
	{0xC01F, 1, COMMON_TYPE				, regSetRatTrq},		/* ��i�g���N/����[Nm/N] */
	{0xE704, 1, COMMON_TYPE				, regSetOverSpdExp},	/* OS���x�w�� */
	{0xE702, 1, COMMON_TYPE				, regSetOverSpd},		/* OS���x[rpm, mm/s] */
	{0xC01E, 1, COMMON_TYPE | REG_1WDACC, regSetMaxSpdExp},		/* MAX���x�w�� */
	{0xC027, 1, COMMON_TYPE | REG_1WDACC, regSetMaxSpd},		/* MAX���x[rpm, mm/s] */
	{0xC01C, 1, COMMON_TYPE				, regSetRatSpd},		/* ��i���x[rpm, mm/s] */
	{0xE086, 1, LINEAR_ONLY				, regSetScalePitchExp},	/* ���j�A�X�P�[���s�b�`�w�� */
	{0xE084, 2, LINEAR_ONLY				, regSetScalePitch},	/* ���j�A�X�P�[���s�b�`[pm] */

#if 0 /* M2�v���t�@�C���Ή� */
	{0x050A, 1, COMMON_TYPE				, regChkInSig50A},		/* ���͐M�����t1 */
	/* ���͐M�����t2�́ASigam7�����Ұ��̒l����BSigmaV�ł́A�ǂ̒l�ł�OK�Ȃ̂��������Ȃ�	*/
	{0x050C, 1, COMMON_TYPE				, regChkInSig50C},		/* ���͐M�����t3 */
	{0x050D, 1, COMMON_TYPE				, regChkInSig50D},		/* ���͐M�����t4 */
#endif
	{0x050E, 1, COMMON_TYPE				, regInitOutSigEnable},	/* �o�͐M�����t1 */
	{0x050F, 1, COMMON_TYPE				, regSetOutSignEnable1},/* �o�͐M�����t2 */
	{0x0510, 1, COMMON_TYPE				, regSetOutSignEnable2},/* �o�͐M�����t3 */
	{0x0514, 1, COMMON_TYPE				, regSetOutSignEnable3},/* �o�͐M�����t4 *//* <S1C0> */
	{0xFFFF, 0, COMMON_TYPE				, NULL}					/* end of table */
};


/* ���W�X�^�e�[�u����` */
CREG_FNACC_TBL fnSysReset[] = {
	{0x80004000, 0x2006},
	{0x80004002, 0x1}
};
CREG_FNACC_TBL fnAlmHistClr[] = {
	{0x80004000, 0x1006},
	{0x80004002, 0x1}
};
CREG_FNACC_TBL fnReCalcPrm[] = {
	{0x80004000, 0x2008},
	{0x80004002, 0x1}
};
CREG_FNACC_TBL fnInitPrm[] = {
	{0x80004000, 0x1005},
	{0x80004002, 0x1}
};
CREG_FNACC_TBL fnCancelAccess[] = {
	{0x80004004, 0x0}
};



/****************************************************************************
 ����������
****************************************************************************/
VOID RegInit( REG_HNDL *hReg, MT_PRM *mtPrm,
			  COM_NET_PRM *netPrm, PRMDATA *Prm, void *hRegMngr )
{
	MlibResetByteMemory( hReg, sizeof(REG_HNDL) );

	hReg->mtPrm = mtPrm;
	hReg->netPrm = netPrm;
	hReg->Prm = Prm;
	hReg->hSvRegMngr = hRegMngr;

	hReg->accSeq = 0;

	hReg->iniPrm.mt.Cycle = CFG_TIO_BASE_TIME * SCAN_N_DEFCYCLE;
	hReg->iniPrm.mt.SegNum = 1;
	hReg->iniPrm.mt.SvCycle = SCAN_N_DEFCYCLE;
	hReg->iniPrm.mt.dlyTime = 0;

	hReg->Prm->comErrCnt = 0;	/* for debug monitor */
}


/****************************************************************************
 �T�[�{�����p�����[�^�̈ꊇ���[�h�p �f�[�^�ݒ菈��
****************************************************************************/
/** �T�[�{���샂�[�h�ݒ� **/
static	INT32 regSetAmpInfo( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	/* OpiCalcInitParam�Ŏ擾�ς�	*/
//	ULONG data = pUniPrm->kprm_ptr->EncSel;
//	ULONG data = 0;
//	hReg->iniPrm.mt.LinearMotor = (BOOL)(data >> AMP_LINEAR_BIT)&0x1;
//	hReg->iniPrm.mt.FullcCtrl = (BOOL)(data >> AMP_FULC_BIT)&0x1;

	hReg->Prm->m3_motorTypeL = hReg->iniPrm.mt.LinearMotor;					/* PnA04	*/
	hReg->Prm->m3_fulcTypeL = hReg->iniPrm.mt.FullcCtrl;					/* PnA06	*/
	return REG_SUCCESS;
}

/** �G���R�[�_ABS���[�h�ݒ� **/
static	INT32 regSetABSEncMode( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	USHORT x;
	x = (pUniPrm->Prm->MencP.typm_e.b.h & 0xF0) ? 0x0100 : 0x0000;
	x = x | pUniPrm->Prm->MencP.bit_dp.b.l ;
	hReg->iniPrm.mt.absPG  = (BOOL)(x >> 8) & 0x1;

	hReg->Prm->m3_encTypeL = ((hReg->iniPrm.mt.absPG == 0) ? 1 : 0);		/* PnA02	*/
	return REG_SUCCESS;
}

/** �G���R�[�_PG�p���X���ݒ� **/
static	INT32 regSetPGPls( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.PgPuls = pUniPrm->Bprm->XfbPulse2;
	hReg->Prm->m3_pulsePerPitchL = hReg->iniPrm.mt.PgPuls;					/* PnA18	*/
	return REG_SUCCESS;
}

/** �t���NPG ABS���[�h�ݒ� **/
static	INT32 regSetFullcScaleType( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
//	LONG data = (USHORT)(Kprm.f.FencUse & FencP.f.AbsoEncoder);
	LONG data = 0;
	if(hReg->iniPrm.mt.FullcCtrl)
	{
		hReg->iniPrm.mt.absPG = (BOOL)(data & 0x1);	/* ABS���[�h�Z�b�g */
		hReg->Prm->m3_encTypeL = ((hReg->iniPrm.mt.absPG == 0) ? 1 : 0);	/* PnA02	*/
	}
	return REG_SUCCESS;
}

/** Tref���[�h�AABS���[�h�ݒ� **/
static	INT32 regSetTrefAndABSMode( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	LONG data = pUniPrm->Prm->b_prm2;
	hReg->iniPrm.mt.trefMode = data & 0xF;	/* TRef���[�h�Z�b�g */
	if((0x100 == (data&0xF00)) && (hReg->iniPrm.mt.FullcCtrl == FALSE))	/* <S16A> */
	{
		hReg->iniPrm.mt.absPG = FALSE;		/* ABS���[�h�Z�b�g */
		hReg->Prm->m3_encTypeL = ((hReg->iniPrm.mt.absPG == 0) ? 1 : 0);	/* PnA02	*/
	}
	
	return REG_SUCCESS;
}

/** �t���NPG�p���X���ݒ� **/
static	INT32 regSetFullcPls( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	LONG data = pUniPrm->Prm->fcplsml;
	if(hReg->iniPrm.mt.FullcCtrl)
	{
		hReg->iniPrm.mt.PgPuls = hReg->iniPrm.mt.PgPuls*data;/* [pitch/rev]*[pls/pitch] */
	}

	hReg->Prm->m3_reslutionL = (hReg->iniPrm.mt.LinearMotor == FALSE) ?		/* PnA14	*/
								hReg->iniPrm.mt.PgPuls : 0;
	return REG_SUCCESS;
}

/** �d�q�M�AB�ݒ� **/
static	INT32 regSetEGearB( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.EGearB = pUniPrm->Prm->ratb2l;
	return REG_SUCCESS;
}

/** �d�q�M�AA�ݒ� **/
static	INT32 regSetEGearA( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.EGearA = pUniPrm->Prm->rata2l;
	return REG_SUCCESS;
}

/** ���[�^�g���N�w�� **/
static	INT32 regSetRatTrqExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.RatTrqExp = (LONG)((CHAR)pUniPrm->Prm->MencP.exp_trat_jmot.b.l - 2);
	/* �g���N�w�� */
	if(hReg->iniPrm.mt.LinearMotor != FALSE)
	{	/* ���j�A�̏ꍇ�APnF07�̒l��N �i�����_�ȉ��̕\����̖�肾���j*/
		hReg->Prm->m3_trqMultiplierL = hReg->iniPrm.mt.RatTrqExp + 2;		/* PnA12	*/
	}
	else
	{	/* ���[�^���̏ꍇ�APnF07�̒l��0.01Nm */
		hReg->Prm->m3_trqMultiplierL = hReg->iniPrm.mt.RatTrqExp;			/* PnA12	*/
	}
	return REG_SUCCESS;
}

/** �ő�g���N�ݒ� **/
static	INT32 regSetMaxTrq( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.MaxTrq = (LONG)pUniPrm->Bprm->PerMaxTrq;
	/* �� * ��i�g���N */
	hReg->Prm->m3_permitTrqL = ((hReg->iniPrm.mt.RatTrq * pUniPrm->Prm->MencP.maxtrq) + 50) / 100;	/* PnA10	*/
	return REG_SUCCESS;
}

/** ��i�g���N�ݒ� **/
static	INT32 regSetRatTrq( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.RatTrq = (LONG)pUniPrm->Prm->MencP.rattrq;				/* PnA0E	*/
	hReg->Prm->m3_ratTrqL = hReg->iniPrm.mt.RatTrq;

	/* �� * ��i�g���N */
	hReg->Prm->m3_permitTrqL = ((hReg->iniPrm.mt.RatTrq * pUniPrm->Prm->MencP.maxtrq) + 50) / 100;	/* PnA10	*/
	return REG_SUCCESS;
}

/** OS���x���w���ݒ� **/
static	INT32 regSetOverSpdExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.OverSpdExp = 0;
	return REG_SUCCESS;
}

/** OS���x���ݒ� **/
static	INT32 regSetOverSpd( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	LONG val;
	val = MlibMulgainSx48( 0x01000000, pUniPrm->Bprm->Kmonspd );
	hReg->iniPrm.mt.OverSpd = val;
	return REG_SUCCESS;
}

/** �ō����x�ݒ� **/
static	INT32 regSetMaxSpd( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.MaxSpd = (LONG)pUniPrm->Bprm->MaxVel;
	hReg->Prm->m3_permitSpdL = hReg->iniPrm.mt.MaxSpd;						/* PnA0A	*/
	return REG_SUCCESS;
}

/** �ō����x�w���ݒ� **/
static	INT32 regSetMaxSpdExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.MaxSpdExp = (LONG)((CHAR)pUniPrm->Prm->MencP.exp_motw_spd.b.h + 2);	/* PnA0C	*/
	hReg->Prm->m3_spdMultiplierL = hReg->iniPrm.mt.MaxSpdExp;
	return REG_SUCCESS;
}

/** ��i���x�ݒ� **/
static	INT32 regSetRatSpd( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.RatSpd = (LONG)pUniPrm->Prm->MencP.vrat_max.b.l;
	hReg->Prm->m3_ratSpdL  = hReg->iniPrm.mt.RatSpd;				/* PnA08 [rpm],[mm/s] */
	return REG_SUCCESS;
}

/** �X�P�[���s�b�`���w���ݒ� **/
static	INT32 regSetScalePitchExp( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	hReg->iniPrm.mt.ScalePitchExp = pUniPrm->Bprm->MonScaleExp;
	return REG_SUCCESS;
}

/** �X�P�[���s�b�`���ݒ� **/
static	INT32 regSetScalePitch( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	LONG	spitch, index, i, ScaleExp;

	hReg->iniPrm.mt.ScalePitch = pUniPrm->Bprm->MonScalePitch;

	spitch = 0;
	if(hReg->iniPrm.mt.LinearMotor != FALSE)
	{
		ScaleExp = 1;
		index = hReg->iniPrm.mt.ScalePitchExp - 3;
		if (index > 0)
		{
			for(i=0; i < index; i++ )
			{
				ScaleExp = 10 * ScaleExp;
			}
			/* �P�ʕϊ�  0.01um �� nm */
			spitch = hReg->iniPrm.mt.ScalePitch * ScaleExp;
		}
		else if (index < 0)
		{
			index = index * -1;
			for(i=0; i < index; i++ )
			{
				ScaleExp = 10 * ScaleExp;
			}
			/* �P�ʕϊ�  0.01um �� nm */
			spitch = hReg->iniPrm.mt.ScalePitch / ScaleExp;
		}
		else
		{
			spitch = hReg->iniPrm.mt.ScalePitch;
		}
	}
	hReg->Prm->m3_scalepitchL = spitch;										/* PnA16	*/

	return REG_SUCCESS;
}

#if 0 /* M2�v���t�@�C���Ή� */
/** ���͐M�����t�ݒ�`�F�b�N **/
static	INT32 regChkInSig50A( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	if((pUniPrm->Prm->insel1 & 0x0FFF) != 0x0881) return REG_DATA_ERR;
	return REG_SUCCESS;
}
static	INT32 regChkInSig50C( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	if((pUniPrm->Prm->insel3 & 0xFFFF) != 0x8888) return REG_DATA_ERR;
	return REG_SUCCESS;
}
static	INT32 regChkInSig50D( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	if((pUniPrm->Prm->insel4 & 0xFFFF) != 0x8888) return REG_DATA_ERR
	return REG_SUCCESS;
}
#endif
/* <S14D>:�ȉ��A�R�����g�A�E�g������� */
/** �o�͐M�����t�����ݒ� **/
static	INT32 regInitOutSigEnable( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
/* �ŏ��̐M�����t�`�F�b�N�Ńt���O�����Z�b�g */
	hReg->iniPrm.mt.OutSigEnable = (pUniPrm->Prm->outsel1 == 0) ? TRUE: FALSE;
	return REG_SUCCESS;
}
static	INT32 regSetOutSignEnable1( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	if(pUniPrm->Prm->outsel2 != 0)	/* ���t�L�̏ꍇ��DISABLE */
	{	hReg->iniPrm.mt.OutSigEnable = FALSE;}
	return REG_SUCCESS;
}
static	INT32 regSetOutSignEnable2( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	if(pUniPrm->Prm->outsel3 != 0)	/* ���t�L�̏ꍇ��DISABLE */
	{	hReg->iniPrm.mt.OutSigEnable = FALSE;}
	return REG_SUCCESS;
}
/* <S14D>:�����܂� */
static	INT32 regSetOutSignEnable3( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )/* <S1C0> */
{
	if(pUniPrm->Prm->outsel4 != 0)	/* ���t�L�̏ꍇ��DISABLE */
	{	hReg->iniPrm.mt.OutSigEnable = FALSE;}
	return REG_SUCCESS;
}

/****************************************************************************
 �����p�����[�^�̈ꊇ���[�h()
****************************************************************************/
INT32 RegLoadDefaultVal( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	UCHAR	idx = 0;
	INT32	ret = REG_SUCCESS;

	hReg->iniPrm.mt.Cycle = pUniPrm->Bprm->SvCycleUs*SCAN_N_DEFCYCLE*4;
	hReg->iniPrm.mt.SvCycle = SCAN_N_DEFCYCLE;

	while( regLoadTbl[idx].regNo != 0xFFFF )
	{/* Table End�܂Ń��[�v */
		if( regLoadTbl[idx].LoadFunc(hReg, pUniPrm) != REG_SUCCESS )
		{
			ret = REG_DATA_ERR;
		}
		++idx;
	}
	return ret;
}


/****************************************************************************
 ����(�I�t���C��)�p�����[�^�̈ꊇ�v�Z
****************************************************************************/
INT32 RegCalcDefaultVal( REG_HNDL *hReg )
{
	INT32 rc;
/* �ʐM�����p�����[�^�ݒ� */

#if 0 /* M2�v���t�@�C���Ή� */
	hReg->iniPrm.mt.OptEnable = hReg->Prm->optBitSw;
	hReg->iniPrm.mt.OptMap[0] = hReg->Prm->optbit1;
	hReg->iniPrm.mt.OptMap[1] = hReg->Prm->optbit2;
	hReg->iniPrm.mt.OptMap[2] = hReg->Prm->optbit3;
	hReg->iniPrm.mt.OptMap[3] = hReg->Prm->optbit4;
	hReg->iniPrm.mt.OptMap[4] = hReg->Prm->optbit5;
#endif
	hReg->iniPrm.mt.SpdDiv = hReg->Prm->spdDivision;
	/* Pn833:POSING�萔�I��	*/
	hReg->iniPrm.rateChgSw = (UCHAR)hReg->Prm->rateChgSw;
	
	/* ���[�V���������p�����[�^�v�Z */
	rc = MPrmCalcInit( hReg->mtPrm, &hReg->iniPrm.mt, hReg->Prm );
	rc = MBnkInit(hReg->mtPrm, hReg->Prm->bnkNum, 
					hReg->Prm->bnkMemNum, hReg->Prm->bnkMember);/* <S080> */
	MPrmCalcSvCmdIo( hReg->mtPrm, hReg->Prm );				/* <S1C0> */

	return rc;
}

/** SET_COM_PRM ***********************************************************************************************/

/** ���_���A�A�v���[�`���x **//* <S027> */
static INT32 regComPrmSetzretSpd1( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	ULONG wk;
	LONG data;

	wk = pUniPrm->Prm->m3_zretSpd1L;
	data = MlibMulgain32( wk, hReg->mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef ) / 100;

	if( pUniPrm->Prm->zretSpd1 )
	{
		if( data != pUniPrm->Prm->zretSpd1 )
		{
			data = MlibMulgain32(pUniPrm->Prm->zretSpd1 * 100, hReg->mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm);
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pUniPrm->Prm->m3_zretSpd1L   = data;
		}
	}
	else
	{
		wk = pUniPrm->Prm->zretSpd12L;
		if( data != wk )
		{
			data = MlibMulgain32(wk * 100, hReg->mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm);
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pUniPrm->Prm->m3_zretSpd1L   = data;
		}
	}

	return REG_SUCCESS;
}

/** ���_���A�N���[�v���x **//* <S027> */
static INT32 regComPrmSetzretSpd2( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	ULONG wk;
	LONG data;

	wk = pUniPrm->Prm->m3_zretSpd2L;
	data = MlibMulgain32( wk, hReg->mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef ) / 100;

	if( pUniPrm->Prm->zretSpd2 )
	{
		if( data != pUniPrm->Prm->zretSpd2 )
		{
			data = MlibMulgain32(pUniPrm->Prm->zretSpd2 * 100, hReg->mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm);
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pUniPrm->Prm->m3_zretSpd2L   = data;
		}
	}
	else
	{
		wk = pUniPrm->Prm->zretSpd22L;
		if( data != wk )
		{
			data = MlibMulgain32(wk * 100, hReg->mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm);
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pUniPrm->Prm->m3_zretSpd2L   = data;
		}
	}

	return REG_SUCCESS;
}
/****************************************************************************
 �����p�����[�^���狤�ʃp�����[�^�̌v�Z�����{����B
****************************************************************************//* <S027> */
INT32 RegSetComDefaultVal( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm )
{
	UCHAR	idx = 0;
	INT32	ret = REG_SUCCESS;

	ret = regComPrmSetzretSpd1(hReg, pUniPrm);
	ret |= regComPrmSetzretSpd2(hReg, pUniPrm);
	
	return ret;
}

/****************************************************************************
 �ʐM�f�[�^���j�^�X�V
****************************************************************************//* <S0BD> */
VOID RegSetComMonData(REG_HNDL* hdl, USHORT* pCmd, USHORT* pRsp, UCHAR prfType)
{
//	USHORT	*comCmd, *comRsp;
	ULONG	*srcBuff, *dstBuff;
	USHORT	alm;

	if(prfType == 0) // �b�菈�u
	{/* M2�݊��v���t�@�C���Ȃ� */
		alm = (pRsp[0] >> 8);
	}
	else
	{/* M3�T�[�{�W���v���t�@�C���Ȃ� */
		alm = ((pRsp[1]  & 0xFF03) || (pRsp[17] & 0x000F));
	}

/* ���j�^�f�[�^�X�V */
	if((hdl->Prm->comDataCtrl == 1) || !(hdl->Prm->comDataCtrl | alm))
	{/* Ctrl=1:�������j�^, ctrl=0:�A���[�����O��f�[�^�ۑ� */
		srcBuff = (ULONG*)pCmd;
//		dstBuff = &hdl->Prm->comCmd[0];			/* <S0CA> */
		dstBuff = (ULONG*)&hdl->Prm->comCmd[0];	/* <S0CA> */
		*(dstBuff+0)  = *(srcBuff + 0);  *(dstBuff+1)  = *(srcBuff + 1);
		*(dstBuff+2)  = *(srcBuff + 2);  *(dstBuff+3)  = *(srcBuff + 3);
		*(dstBuff+4)  = *(srcBuff + 4);  *(dstBuff+5)  = *(srcBuff + 5);
		*(dstBuff+6)  = *(srcBuff + 6);  *(dstBuff+7)  = *(srcBuff + 7);
		*(dstBuff+8)  = *(srcBuff + 8);  *(dstBuff+9)  = *(srcBuff + 9);
		*(dstBuff+10) = *(srcBuff + 10); *(dstBuff+11) = *(srcBuff + 11);
		
		srcBuff = (ULONG*)pRsp;
//		dstBuff = &hdl->Prm->comRsp[0];			/* <S0CA> */
		dstBuff = (ULONG*)&hdl->Prm->comRsp[0];	/* <S0CA> */
		*(dstBuff+0)  = *(srcBuff + 0);  *(dstBuff+1)  = *(srcBuff + 1);
		*(dstBuff+2)  = *(srcBuff + 2);  *(dstBuff+3)  = *(srcBuff + 3);
		*(dstBuff+4)  = *(srcBuff + 4);  *(dstBuff+5)  = *(srcBuff + 5);
		*(dstBuff+6)  = *(srcBuff + 6);  *(dstBuff+7)  = *(srcBuff + 7);
		*(dstBuff+8)  = *(srcBuff + 8);  *(dstBuff+9)  = *(srcBuff + 9);
		*(dstBuff+10) = *(srcBuff + 10); *(dstBuff+11) = *(srcBuff + 11);
	}

/* �A���[������ && ���j�^���擾 */
	if(!hdl->iniPrm.monStop && alm)
	{
		srcBuff = (ULONG*)pCmd;
//		dstBuff = &hdl->Prm->almCmd[0];			/* <S0CA> */
		dstBuff = (ULONG*)&hdl->Prm->almCmd[0];	/* <S0CA> */
		*(dstBuff+0)  = *(srcBuff + 0);  *(dstBuff+1)  = *(srcBuff + 1);
		*(dstBuff+2)  = *(srcBuff + 2);  *(dstBuff+3)  = *(srcBuff + 3);
		*(dstBuff+4)  = *(srcBuff + 4);  *(dstBuff+5)  = *(srcBuff + 5);
		*(dstBuff+6)  = *(srcBuff + 6);  *(dstBuff+7)  = *(srcBuff + 7);
		*(dstBuff+8)  = *(srcBuff + 8);  *(dstBuff+9)  = *(srcBuff + 9);
		*(dstBuff+10) = *(srcBuff + 10); *(dstBuff+11) = *(srcBuff + 11);
		
		srcBuff = (ULONG*)pRsp;
//		dstBuff = &hdl->Prm->almRsp[0];			/* <S0CA> */
		dstBuff = (ULONG*)&hdl->Prm->almRsp[0];	/* <S0CA> */
		*(dstBuff+0)  = *(srcBuff + 0);  *(dstBuff+1)  = *(srcBuff + 1);
		*(dstBuff+2)  = *(srcBuff + 2);  *(dstBuff+3)  = *(srcBuff + 3);
		*(dstBuff+4)  = *(srcBuff + 4);  *(dstBuff+5)  = *(srcBuff + 5);
		*(dstBuff+6)  = *(srcBuff + 6);  *(dstBuff+7)  = *(srcBuff + 7);
		*(dstBuff+8)  = *(srcBuff + 8);  *(dstBuff+9)  = *(srcBuff + 9);
		*(dstBuff+10) = *(srcBuff + 10); *(dstBuff+11) = *(srcBuff + 11);
	}
	hdl->iniPrm.monStop = alm;
}

/****************************************************************************
 �ʐMASIC�G���[���擾����
****************************************************************************//* <S0BD> */
VOID RegSetComErrData(REG_HNDL* hdl, USHORT cnt, ULONG ests)
{
	if(ests != 0)
	{
		hdl->Prm->comErrSts2L |= ests;
		hdl->Prm->comErrSts1L = ests;
	}
	hdl->Prm->comErrCnt = cnt;
}


/** SET_COM_PRM ***********************************************************************************************/

/****************************************************************************
 ���W�X�^�A�N�Z�X�o�b�N�O�����h����
****************************************************************************/

// �X���b�v�����́A�r�b�O�E���g������
typedef union REG_SWAP_VAR {
	LONG	val;
	struct {
		USHORT	us_l;	// word 0
		USHORT	us_h;	// word 1
	}word;
	struct {
		UCHAR	uc_ll;
		UCHAR	uc_lh;
		UCHAR	uc_hl;
		UCHAR	uc_hh;
	}byte;
}REG_SWAP_VAR;


/* Copy a LONG data to specified type buffer */
static void regCopyData(void *dst, LONG src, UINT bsz, UINT d_type)
{
	UCHAR	*ucwk;
	USHORT	*uswk;
	ULONG	*ulwk;
	UINT i, num;

	num = (bsz/d_type);
	if(d_type == 1)
	{
		ucwk = (UCHAR*)dst;
		for(i = 0; i < num; i++)
		{	ucwk[i] = (UCHAR)((ULONG)src >> (i*8));}
	}
	else if(d_type == 2)
	{
		uswk = (USHORT*)dst;
		for(i = 0; i < num; i++)
		{	uswk[i] = (USHORT)((ULONG)src >> (i*16));}
	}
	else
	{
		ulwk = (ULONG*)dst;
		ulwk[0] = src;
	}
}

/* Get a LONG data from specified type buffer */
static LONG regGetData(void *dst, UINT bsz, UINT d_type)
{
	UCHAR	*ucwk;
	USHORT	*uswk;
	LONG	value, num, i;

	num = (bsz/d_type);
	value = 0;
	if(d_type == 1)
	{
		ucwk = (UCHAR*)dst;
		for(i = 0; i < num; i++)
		{	value |= (ucwk[i] << (8*i));}
	}
	else if(d_type == 2)
	{
		uswk = (USHORT*)dst;
		for(i = 0; i < num; i++)
		{	value |= (uswk[i] << (16*i));}
	}
	else
	{
		value = *((LONG*)dst);
	}
	return value;
}

/* Swap data in specified type */
static void regSwapAllData(LONG *dst, UINT bsz, UINT d_type)
{
	REG_SWAP_VAR	wk;
	UINT i, num;

	num = (bsz+3)/4;
	if(d_type == 1)
	{
		for(i = 0; i < num; i++)
		{
			wk.val = dst[i];
			dst[i]	= (wk.byte.uc_hh<<24) | (wk.byte.uc_hl<<16)
					| (wk.byte.uc_lh<<8 ) |  wk.byte.uc_ll;
		}
	}
	else if(d_type == 2)
	{
		for(i = 0; i < num; i++)
		{
			wk.val = dst[i];
			dst[i]	= (wk.word.us_h << 16) | wk.word.us_l;
		}
	}
	else
	{
		/* do nothing */
	}
}

/****************************************************************************
 ���W�X�^���[�h����
****************************************************************************/
static	PRM_RSLT regRunRead( REG_HNDL *hReg )
{
	PRM_RSLT	rc;
	REG_ACC_T	AccessPrm;
	LONG		t_bsz, prmSz, accSz;
	LONG		rdValue;
	UCHAR		*buffer;

	t_bsz = hReg->accData->Length * hReg->accData->AccessSize;
	if(t_bsz > hReg->accData->BufferSize)
	{/* Buffer under run */
		return PRM_RSLT_SIZE_ERR;
	}

	accSz = 0;
	buffer = (UCHAR*)hReg->accData->pBuf;
//	MlibResetByteMemory(buffer, sizeof(hReg->accData->BufferSize));/* <S052> */
	MlibResetByteMemory(buffer, hReg->accData->BufferSize);
	AccessPrm.MemAddr = hReg->accData->Number;
	AccessPrm.IfHandle = hReg;

	rc = PRM_RSLT_SUCCESS;

	while(accSz < t_bsz)
	{
		/* Get Paremter Attribute */
		if(NULL != RegMngrGetAttrByMemAddr(hReg->hSvRegMngr, &AccessPrm))
		{
			prmSz = AccessPrm.cPrmDef->DataLength;
			if( (t_bsz < (accSz + prmSz)) ||
				(prmSz < hReg->accData->AccessSize) )
			{/* size over */
				rc = PRM_RSLT_SIZE_ERR;
				break;
			}
			/* Read a Paremter */
			rc = PrmReadValue(&AccessPrm, &rdValue, prmSz);
			if(rc < 0)
			{
				break;
			}

			regCopyData(&buffer[accSz], rdValue,
							prmSz, hReg->accData->AccessSize);
			accSz = accSz + prmSz;
		}
		else
		{
			rc = PRM_RSLT_REGNO_ERR;
			break;
		}
		AccessPrm.MemAddr += prmSz;
	}
	regSwapAllData(hReg->accData->pBuf, t_bsz, hReg->accData->AccessSize);

	return rc;
}


/****************************************************************************
 ���W�X�^���C�g����
****************************************************************************/
static	PRM_RSLT regRunWrite( REG_HNDL *hReg )
{
	PRM_RSLT	rc;
	REG_ACC_T	AccessPrm;
	LONG		t_bsz, prmSz, accSz;
	LONG		wrValue;
	UCHAR		*buffer;

	if( hReg->accWaitBusy != FALSE )
	{ /* �p�����̏ꍇ�́A��������܂�Check */
		return RegMngrGetWriteOpeRegStatus(hReg->hSvRegMngr);
	}

	t_bsz = hReg->accData->Length * hReg->accData->AccessSize;
	if((t_bsz > hReg->accData->BufferSize) || (t_bsz > sizeof(hReg->accBuffer)))
	{/* Buffer under run */
		return PRM_RSLT_SIZE_ERR;
	}

	accSz = 0;
	buffer = (UCHAR*)hReg->accBuffer;
	AccessPrm.MemAddr = hReg->accData->Number;
	AccessPrm.IfHandle = hReg;
	MlibCopyByteMemory(hReg->accBuffer, hReg->accData->pBuf, t_bsz+(4-(t_bsz%0x3)));
	regSwapAllData(hReg->accBuffer, t_bsz, hReg->accData->AccessSize);

	while(accSz < t_bsz)
	{
		/* Get Paremter Attribute */
		if(NULL != RegMngrGetAttrByMemAddr(hReg->hSvRegMngr, &AccessPrm))
		{
			prmSz = AccessPrm.cPrmDef->DataLength;
			if((t_bsz < (accSz + prmSz)) || (prmSz < hReg->accData->AccessSize))
			{/* size over */
				return PRM_RSLT_SIZE_ERR;
			}
			/* Write a Paremter */
			AccessPrm.EepWrite |= hReg->accData->EepWrite;
			wrValue = regGetData(&buffer[accSz], prmSz, hReg->accData->AccessSize);
			rc = PrmWriteValue(&AccessPrm, &wrValue, prmSz);
			if(rc < 0)
			{
				return rc;
			}
			if(rc == PRM_RSLT_CONTINUE)
			{
				hReg->accWaitBusy = TRUE;
				return rc;
			}

			accSz = accSz + prmSz;
		}
		else
		{
			return PRM_RSLT_REGNO_ERR;
		}
		AccessPrm.MemAddr += prmSz;
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 ID���[�h����
****************************************************************************//* <S222> *//* <S**1> */
static	PRM_RSLT regRunIDRead( REG_HNDL *hReg )
{
	PRM_RSLT	rc;
	REG_ACC_T	AccessPrm;
	LONG		accessSize, num;
	LONG		t_bsz, accSz, maxSz;
	LONG		rdValue;
	UCHAR		*buffer, idx;
	
	rc = PRM_RSLT_SUCCESS;
	accSz = 0;
	buffer = (UCHAR*)hReg->accData->pBuf;
	MlibResetByteMemory(buffer, hReg->accData->BufferSize);

	t_bsz = hReg->accData->Length * hReg->accData->AccessSize;
	if(t_bsz > hReg->accData->BufferSize)
	{/* Buffer under run */
		return PRM_RSLT_SIZE_ERR;
	}

	/* Get Paremter Attribute */
	AccessPrm.MemAddr = hReg->accData->Number;
	AccessPrm.IfHandle = hReg;
	if(NULL != RegMngrGetAttrByMemAddr(hReg->hSvRegMngr, &AccessPrm))
	{
		/* check reserved */
		if(AccessPrm.cPrmDef->SpUse == TRUE)
		{/* resrved area */
			return PRM_RSLT_REGNO_ERR;
		}

		accessSize = AccessPrm.cPrmDef->DataLength;
		if(AccessPrm.cPrmDef->NumberOfArray != 0)/* <S0D4> */
		{
			num = AccessPrm.cPrmDef->NumberOfArray;
		}
		else
		{/* NumberOfArray�̐ݒ肪�ȗ�����Ă���ꍇ	*/
			num = 1;
		}
		maxSz = num * accessSize;

		/* �擾�̈�`�F�b�N */
		if( (t_bsz + hReg->accData->offset) > maxSz )
		{
			return PRM_RSLT_SIZE_ERR;
		}

	}
	else
	{/* No ID_CODE */
		return PRM_RSLT_REGNO_ERR;
	}

	
	AccessPrm.MemAddr += hReg->accData->offset;
 	while(accSz < t_bsz)
	{

		/* Get Paremter Attribute */
		RegMngrGetAttrByMemAddr(hReg->hSvRegMngr, &AccessPrm);
		
		/* Read a Paremter */
		rc = PrmReadValue(&AccessPrm, &rdValue, accessSize);
		if(rc < 0)
		{
			break;
		}

		regCopyData(&buffer[accSz], rdValue,
						accessSize, accessSize);
		accSz = accSz + accessSize;
		AccessPrm.MemAddr += accessSize;
	}
	regSwapAllData(hReg->accData->pBuf, t_bsz, accessSize);

	return rc;

//	return regRunRead(hReg);
	return rc;
}

/****************************************************************************
 FnMode����v�����s����
****************************************************************************/
static	PRM_RSLT regRunFnMode( REG_HNDL *hReg, CREG_FNACC_TBL *FnRegs, UINT regNum )
{
	PRM_RSLT		rc;
	REG_ACC_T		AccessPrm;

	if( hReg->accWaitBusy == FALSE )
	{ /* �p�����łȂ��ꍇ�́A���C�g���� */
		AccessPrm.MemAddr =  FnRegs[hReg->accSeq].adrs;
		AccessPrm.IfHandle = hReg;

		/* Get Paremter Attribute */
		if(NULL != RegMngrGetAttrByMemAddr(hReg->hSvRegMngr, &AccessPrm))
		{
			/* Write a Paremter */
			rc = PrmWriteValue(&AccessPrm,
								(LONG *)&(FnRegs[hReg->accSeq].data),
								AccessPrm.cPrmDef->DataLength);
			if(rc == PRM_RSLT_CONTINUE)
			{
				hReg->accWaitBusy = TRUE;
			}
		}
		else
		{
			rc = PRM_RSLT_REGNO_ERR;
		}
	}
	else
	{ /* �p�����̏ꍇ�́A��������܂�Check */
		rc = RegMngrGetWriteOpeRegStatus(hReg->hSvRegMngr);
	}

	if(rc == PRM_RSLT_SUCCESS)
	{ /* ����I�� */
		++hReg->accSeq;
		rc = (hReg->accSeq >= regNum) ? PRM_RSLT_SUCCESS : PRM_RSLT_CONTINUE;
		hReg->accWaitBusy = FALSE;
	}

	return rc;
}


/****************************************************************************
 ���[�V�����T�C�N���ύX�p�����[�^�v�Z����
****************************************************************************/
static	PRM_RSLT regRunCalcCycPrm( REG_HNDL *hReg )
{
	CYC_CHG_PRM		*cPrm;

	cPrm = (CYC_CHG_PRM*)hReg->accData->pBuf;

//	hReg->iniPrm.mt.Cycle = cPrm->scanCyc;
//	hReg->iniPrm.mt.SvCycle = SCAN_N_DEFCYCLE;
	hReg->iniPrm.mt.SegNum = ( cPrm->intCyc * cPrm->aplCyc ) / hReg->iniPrm.mt.Cycle;
	hReg->iniPrm.mt.dlyTime = cPrm->dlyTime;

	hReg->Prm->comTCycle = (USHORT)cPrm->intCyc;
	hReg->Prm->comAplCycle = (USHORT)cPrm->aplCyc;

	MPrmCalcInit( hReg->mtPrm, &hReg->iniPrm.mt, hReg->Prm );

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 ���W�X�^����o�b�N�O�����h����
****************************************************************************/
VOID RegRunBackgrndProc( REG_HNDL *hReg )
{
	PRM_RSLT rc;
	INT32	 sts;
	UINT	 regNum;

	/* ���W�X�^�����֐����s */
	if(hReg->accCode != REG_CMD_NONE)
	{
		switch(hReg->accCode)
		{
		case REG_CMD_PRMRD:		/* 1:���W�X�^���[�h */
			rc = regRunRead( hReg );
			break;
		case REG_CMD_PRMWR:		/* 2:���W�X�^���C�g */
			rc = regRunWrite( hReg );
			break;
		case REG_CMD_RECALC:	/* 3:�p�����[�^�Čv�Z */
			regNum = sizeof(fnReCalcPrm)/sizeof(REG_FNACC_TBL);
			rc = regRunFnMode( hReg, fnReCalcPrm, regNum );
			break;
		case REG_CMD_ALMHISTCLR:/* 4:�T�[�{�A���[�������N���A */
			regNum = sizeof(fnAlmHistClr)/sizeof(REG_FNACC_TBL);
			rc = regRunFnMode( hReg, fnAlmHistClr, regNum );
			break;
		case REG_CMD_SYSRST:	/* 5:�V�X�e�����Z�b�g */
//			regNum = sizeof(fnSysReset)/sizeof(REG_FNACC_TBL);
//			rc = regRunFnMode( hReg, fnSysReset, regNum );
			regNum = sizeof(fnCancelAccess)/sizeof(REG_FNACC_TBL);
			rc = regRunFnMode( hReg, fnCancelAccess, regNum );
			break;
		case REG_CMD_CHGCYC:	/* 6:Task�����ύX�ɂ��p�����[�^�Čv�Z */
			rc = regRunCalcCycPrm( hReg );
			break;
		case REG_CMD_INITPRM:	/* 7:�p�����[�^������ */
			regNum = sizeof(fnInitPrm)/sizeof(REG_FNACC_TBL);
			rc = regRunFnMode( hReg, fnInitPrm, regNum );
			break;
		case REG_CMD_IDRD:		/* 8:ID_RD */
			rc = regRunIDRead( hReg );
			break;
		default:
			rc = PRM_RSLT_SUCCESS;
			break;
		}

		if(rc != PRM_RSLT_CONTINUE)
		{ /* �����I��	*/
			switch(rc)
			{
			case PRM_RSLT_REGNO_ERR: sts = REG_NO_ERR;	break;	/* ���W�X�^�ԍ��ُ� */
			case PRM_RSLT_LIMIT_ERR: sts = REG_DATA_ERR;break;	/* �f�[�^�ُ�(�㉺���ُ�) */
			case PRM_RSLT_CALC_ERR:	 sts = REG_DATA_ERR;break;	/* �f�[�^�ُ�(���Z�ُ�)*/
			case PRM_RSLT_SIZE_ERR:	 sts = REG_SIZE_ERR;break;	/* �f�[�^���ُ� */
			case PRM_RSLT_RWACC_ERR: sts = REG_NO_ERR;	break;	/* �A�N�Z�X�G���[ *//* <S14D> */
//			case PRM_RSLT_NEEDREBOOT:sts = REG_SUCCESS;	break;	/* �������� */
			case PRM_RSLT_SUCCESS:	 sts = REG_SUCCESS;	break;	/* �������� */
			default:				 sts = REG_CNDTN_ERR;break;	/* ���s�����ُ� */
			}
			hReg->accWaitBusy = FALSE;
			hReg->accSts = sts;
			hReg->accSeq = 0;
			hReg->accCode = REG_CMD_NONE;
		}
	}
}


/****************************************************************************
 ���W�X�^�R�}���h�����v�����sAPI
 (���ӁFREG_DATA�́A���������܂Ŏ��̂�ێ����邱��)
****************************************************************************/
INT32 RegRequestCommand( REG_HNDL *hReg, REG_CMD_TYPE type, REG_DATA *pData )
{
	if(hReg->accCode == REG_CMD_NONE)
	{
		hReg->accData = pData;
		hReg->accSeq = 0;
		hReg->accSts = REG_CONTINUE;
		hReg->accWaitBusy = FALSE;
		hReg->accCode = type;
	}
	return ((hReg->accCode != type) ? REG_CNDTN_ERR : REG_CONTINUE);
}

/****************************************************************************
 ���W�X�^�R�}���h���������m�FAPI
****************************************************************************/
INT32 RegCheckCommandState( REG_HNDL *hReg )
{
	return hReg->accSts;
}


/****************************************************************************
 �ʐM���p�����[�^�X�VAPI
****************************************************************************/
VOID RegSetComPrmMon( REG_HNDL* hReg, USHORT adr, USHORT byte )
{
	hReg->Prm->nodeAdr = adr;
	hReg->Prm->trByte = byte;
}



#if 0
static void regReadDevInfoTable( DEV_INFO_TBL *info, DEV_DATA *data )
{
	UCHAR	*pStrAscii;
	ULONG	*pStr;
	ULONG	i;
	ULONG	maxSize;
	ULONG	ofset;
	ULONG	rdSize;
	ULONG	dwcnt;
	ULONG	val1, val2;
	ULONG	ofs_sht;

	maxSize = info->size;

	if( info->type == ASCII )
	{
		dwcnt = 0;
		rdSize = data->size;
		pStrAscii = (UCHAR*)info->ptr;

		for( i=0; i<rdSize; i++ )
		{
			dwcnt = (i>>2);
			ofset = data->ofst + i;
			if(ofset < maxSize)
			{
				if( 0 == (i & 0x3) )
				{
					data->pBuf[dwcnt].dw = pStrAscii[ofset];
				}
				else
				{
					data->pBuf[dwcnt].dw |= ( pStrAscii[ofset] << ((i & 0x3) * 8) );
				}
			}
			else
			{
				if( 0 == (i & 0x3) )
				{
					data->pBuf[dwcnt].dw = 0;
				}
			}
		}
	}
	else
	{
		ofs_sht = (data->ofst & 0x3) * 8;
		ofset = data->ofst >> 2;
		rdSize = (data->size + 3) >> 2;
		pStr = (ULONG*)info->ptr;

		maxSize = maxSize >> 2;
		for( i=0; i<rdSize; i++ )
		{
			if( ofset < maxSize )
			{
				val1 = pStr[ofset];
			}
			else
			{
				val1 = 0;
			}

			if( (ofset+1) < maxSize )
			{
				val2 = pStr[ofset + 1];
			}
			else
			{
				val2 = 0;
			}

			data->pBuf[i].dw = ( val2 << (32-ofs_sht)) | (val1 >> ofs_sht );
			ofset++;
		}
	}
}

/****************************************************************************
 �f�o�C�X���擾API(StdProfile) for ID_RD
****************************************************************************/
INT32 RegReadIDInfo( DEV_INFO_TBL *info, DEV_DATA *data )
{
	ULONG i;

	for( i=0; info[i].id_code != 0xFFFF; i++ )
	{
		/* Resrve�͓ǂ߂Ȃ� */
		if( info[i].type == RESRVE )
		{
			continue;
		}

		if( data->type == info[i].id_code )
		{
			regReadDevInfoTable( &info[i], data );
//			if ( info[i].size < data->ofst + data->size )
//			{
//				return ERR_DATA_SZ;		/* A.94D ���W�X�^�T�C�Y�ُ� */
//			}
			return REG_SUCCESS;
		}
	}

	/* ������Ȃ��ꍇ */
	return REG_NO_ERR;				/* A.94A ���W�X�^�ԍ��ُ� */
}

/****************************************************************************
 �f�o�C�X���擾API(StdProfile) for MEM_RD
****************************************************************************/
INT32 RegReadIDInfoForMemRD( DEV_INFO_TBL *info, DEV_DATA *data )
{
	LONG	i;
	LONG	offset, id_code;
	LONG	top_adrs, adrs, end_adrs;

	/* ���[�J���ϐ��̏����� */
	offset = data->ofst;
	id_code = data->type;

	/* id_code, offset�̍Đݒ� */
	for( i=0; info[i].id_code != 0xFFFF; i++ )
	{
		adrs = ( id_code*4+offset );
		top_adrs = info[i].id_code*4;
		end_adrs = info[i+1].id_code*4;

		if( (adrs >= top_adrs) && (adrs < end_adrs) )
		{
			data->type = (UCHAR)info[i].id_code;
			data->ofst = adrs - top_adrs;
			regReadDevInfoTable( &info[i], data );
			break;
		}
	}

	return REG_SUCCESS;
}

/****************************************************************************
 �f�o�C�X���擾API for MESSAGE
****************************************************************************/
LONG RegReadIDInfoForMessage( DEV_INFO_TBL *info, LONG RegNo, LONG Number, ULONG *RdBuf )
{
	LONG		ret = PRM_RSLT_SUCCESS;
	DEV_DATA	dev_data;
	SHORT		rem;

	if ((RegNo + Number - 1) > 0x0000FFFF)
	{
		return REG_SIZE_ERR;							/* A.94D �f�[�^�T�C�Y�x��		*/
	}
	dev_data.type  = (UCHAR)MlibDivhrem(RegNo, 4, &rem);
	dev_data.ofst  = (UCHAR)rem;
	dev_data.size  = (UCHAR)(Number);					/* �A�N�Z�X�T�C�Y���� */;
	dev_data.pBuf  = (DWORDX*)RdBuf;
	ret = RegReadIDInfoForMemRD( info, &dev_data );
	/* �߂�l�ݒ� */
	return ret;
}
#endif







