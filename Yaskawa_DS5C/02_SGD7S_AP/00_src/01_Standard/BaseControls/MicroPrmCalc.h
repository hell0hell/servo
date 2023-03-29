/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MicroScanAIf.c : �ʃv���O����ScanA I/F������`												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �ʃv���O������ScanA��CPU�Ԃ�I/F����														*/
/*																									*/
/*		�ʃv���O�����|CPU�Ԃ̃f�[�^������s���B														*/
/*																									*/
/*																									*/
/*																									*/
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
#ifndef MICRO_PRM_CALC_H
#define MICRO_PRM_CALC_H


#include "JL086ServoIpReg.h"
#include "Bprm.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define		MICRO_TR_READY			0		/* �}�C�N���v���O��������(READY���)					*/
#define		MICRO_TR_SIZE_SHORT		1		/* �}�C�N���p�����[�^�ꊇ�]���p�������݃f�[�^�T�C�Y		*/
#define		MICRO_TR_SIZE_LONG		2		/* �}�C�N���p�����[�^�ꊇ�]���p�������݃f�[�^�T�C�Y		*/
#define		MICRO_TR_DATA_MAX		32		/* �}�C�N���p�����[�^�ꊇ�]���p�������݃f�[�^��[WORD]	*/
#define		MICRO_TR_DATA_ERROR		-1		/* �}�C�N���p�����[�^�ꊇ�]���G���[�X�e�[�^�X			*/
#define		MICRO_TR_SUCCESS		0		/* �}�C�N���p�����[�^�ꊇ�]���f�[�^�ݒ芮��				*/
#define		MICRO_ADDRESS_CHANGE	0x0fff	/* �}�C�N���A�h���X�ύX�p								*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		��ߊE������p�ϐ���`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct WEAKENFIELD {
	struct {						/* ��ߊE���p�萔�f�[�^��`										*/
		KSGAIN	KVdcAsic;				/* ���H���o�l���Z�Q�C�� [Admax] �� [8192/Vdc]				*/
		KSGAIN	KpvBeforeDiv;			/* �d���t�B�[�h�o�b�N���[�v�Q�C���v�Z�p�萔(���x�Ŋ���O)	*/
		KSGAIN	KivBeforeDiv;			/* �d���t�B�[�h�o�b�N���[�v�ϕ��Q�C���v�Z�p�萔(���x�Ŋ���O)*/
		LONG	IdrefLimTerm1;			/* d���d���w�߃��~�b�g�l�v�Z�p�萔(���x^2�Ŋ���O)			*/
		LONG	IdrefLimTerm23;			/* d���d���w�߃��~�b�g�l�v�Z�p�萔(���x�Ɋ֌W�̂Ȃ���)		*/
		LONG	IdrefLimTerm4;			/* d���d���w�߃��~�b�g�l�v�Z�p�萔(���x�Ŋ���O)			*/
		SHORT	V1Max;					/* �d���w�ߐ������x��										*/
		SHORT	dummy1;					/* for alignment											*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {						/* ��ߊE���p�ϐ��f�[�^��`										*/
		LONG	Kpv;					/* �d���t�B�[�h�o�b�N���[�v�Q�C��(���x�Ŋ�������)			*/
		LONG	Kiv;					/* �d���t�B�[�h�o�b�N���[�v�ϕ��Q�C��(���x�Ŋ�������)		*/
		LONG	VdcSum;					/* ���H�d���ώZ�l											*/
		SHORT	IdrefLim;				/* d���d���w�߃��~�b�g�l									*/
		SHORT	KpvL;					/* Kpv�̉���16bit											*/
		SHORT	KpvH;					/* Kpv�̏��16bit											*/
		SHORT	KivL;					/* Kiv�̉���16bit											*/
		SHORT	KivH;					/* Kiv�̏��16bit											*/
		SHORT	VdcAsic;				/* ���H�d�����ϒl											*/
		SHORT	VdcAvgCnt;				/* ���H�d�����ω�										*/
		SHORT	VdcSumCnt;				/* ���H�d���ώZ��										*/
	} var;
} WEAKENFIELD;


/*--------------------------------------------------------------------------------------------------*/
/*		�}�C�N���d���⏞�Q�C���@�\�p�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MPKVVCOMP {
	struct {						/* �}�C�N���d���⏞�Q�C���@�\�p�萔�f�[�^��`					*/
		LONG	CompLevel;				/* ���H�d���␳���x��										*/
		LONG	KdMpKvv;				/* Calculate Gain for MpKvv									*/
		LONG	KfMpKvv;				/* Filter Gain for MpKvv									*/
		BOOL	MpKvvFlg;				/* ���H�d���␳�@�\�I��	[0:OFF,1:ON]					*/
		USHORT	vrevgn;					/* �d���␳�Q�C��											*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {						/* �}�C�N���d���⏞�Q�C���@�\�p�ϐ��f�[�^��`					*/
		SHORT	MpKvv;					/* �d���⏞�Q�C���w��			[8192/VdcBase]				*/
		SHORT	Dummy;					/* Dummy for Alignment										*/
	} var;
} MPKVVCOMP;


/* Input Parameter of "KpxPcalMicroProgram()" */
typedef struct MAICRO_CFGPRM {
	USHORT	imaxs;					/* PnE15 : �T�[�{�p�b�N�ő�d��									*/
	USHORT	ignd;					/* PnE20 : �d�����[�v�Q�C��(D)									*/
	USHORT	ignq;					/* PnE21 : �d�����[�v�Q�C��(Q)									*/
	USHORT	kid;					/* PnE22 : �d�����[�v�ϕ����萔(D)								*/
	USHORT	kiq;					/* PnE23 : �d�����[�v�ϕ����萔(Q)								*/
	USHORT	limintd;				/* PnE24 : �d�����[�v�ϕ������l(D)								*/
	USHORT	limintq;				/* PnE25 : �d�����[�v�ϕ������l(Q)								*/
	USHORT	ifil;					/* PnE26 : �d�����[�v�t�B���^���萔								*/
	USHORT	shant;					/* PnE28 : �d�����o�Q�C��										*/
	USHORT	pwmf;					/* PnE2C : �o�v�l���g��											*/
	USHORT	dq_sw;					/* PnE2F : �c�p�p�X�C�b�`										*/
	USHORT	vrevgn;					/* PnE2A : �d���␳�Q�C��										*/
	USHORT	lmdata;					/* PnE2B : �d�������l											*/
	DBYTEX	odt_c;					/* PnE2D : ����:�I���f�B���C����, ���:�I���f�B���C�⏞�萔		*/
	DBYTEX	odlv_slp;				/* PnE2E : ����:�I���f�B���C�⏞�ύX���x��, ���:�⏞�X��		*/
	USHORT	motresq;				/* PnE3B : ���[�^���U�}���p�m�b�`�t�B���^Q�l					*/
	USHORT	fltrange;				/* PnE3F : ���[�^�ߓd�����o�t�B���^��							*/	/* <S1F5> */
	USHORT	zadjiu;					/* PnE50 : �d�����o�[����(U��)									*/
	USHORT	zadjiv;					/* PnE51 : �d�����o�[����(V��)									*/
	DBYTEX	gadjiu_v;				/* PnE52 : �d�����o�Q�C����(U,V��)								*/
	USHORT	pnvoltgn;				/* PnE70 : P-N�d�����o���x��									*/
	USHORT	pwmf2;					/* PnEA5 : �É����[�hPWM���g��									*/
	USHORT	ilimlvl;				/* PnEA6 : �L�����A���g���؂�ւ��d��臒l						*/
	USHORT	pwmf3;					/* PnEA7 : ��2PWM���g��											*/
	USHORT	idlim;					/* PnEB2 : �d���t�B�[�h�o�b�N�ϕ����~�b�g						*/
	DBYTEX	oslv_pol;				/* PnF06 : �n�r���o���x��, �|�[����								*/
	USHORT	irat;					/* PnF09 : ��i�d��												*/
	USHORT	imax;					/* PnF0A : �u���ő�d��											*/
	USHORT	motresfrq;				/* PnF1C : ���[�^���U�}���p�m�b�`�t�B���^���g��					*/
	USHORT	flg_wf;					/* PnF1E : �t���O												*/
	MFUNCF	motfuncf;				/* PnF24 : ���[�^�@�\�t���O2									*//*���[�^�L�����A���g���؂�ւ�<S117>*/
	USHORT	mot_pwmf;				/* PnF25 : ���[�^PWM���g��										*//*���[�^�L�����A���g���؂�ւ�<S117>*/
#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
	BOOL	motCarrUse;				/* ���[�^�L�����A(PnF25)�L�� 	[0:OFF,1:ON]					*//*���[�^�L�����A���g���؂�ւ�<S117>*/
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
	SHORT	NormCarrCnt;			/* �ʏ�L�����A�J�E���g�l		[-]								*//*���[�^�L�����A���g���؂�ւ�<S117>*/
	SHORT	HighCarrCnt;			/* ���L�����A�J�E���g�l			[-]								*//*���[�^�L�����A���g���؂�ւ�<S117>*/
} MAICRO_CFGPRM;


/* Input Parameter of "PcalMicroDqAcrGain()"  */
typedef struct DQACR_GAIN_PRM {
	USHORT	ignd;					/* PnE20 : �d�����[�v�Q�C��(D)									*/
	USHORT	ignq;					/* PnE21 : �d�����[�v�Q�C��(Q)									*/
	USHORT	kid;					/* PnE22 : �d�����[�v�ϕ����萔(D)								*/
	USHORT	kiq;					/* PnE23 : �d�����[�v�ϕ����萔(Q)								*/
	USHORT	ifil;					/* PnE26 : �d�����[�v�t�B���^���萔								*/
	USHORT	ifil2;					/* PnEA4 : �É����[�h�d�����[�v�t�B���^���萔					*/
} DQACR_GAIN_PRM;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/

LONG	KpxPcalMicroProgram( MAICRO_CFGPRM *uCfgPrm, BPRMDAT *Bprm,
							 ASICS *SvAsicRegs, WEAKENFIELD* WeakenField );
LONG	KpxMotorOCLevel( MAICRO_CFGPRM *uCfgPrm, ASICS *SvAsicRegs, MENCPRM *MencP );	/*<S1F5>*/
void	KpiPcalMicroNotchFilter2( ASICS *SvAsicRegs, LONG hz, LONG qx, LONG dx, SHORT *MpReg );
LONG	KpiChangePwmFrequency( ULONG PwmFx, USHORT pwmf, USHORT pwmf3, USHORT flg_wf );
void	KpiManageMicroTorqueFilter( VUSHORT *mreg_csw_ptr, USHORT *CswData, USHORT FilSel, LONG FilOn );
LONG	LpxSetPrmToASIC( SHORT *adr, LONG prm, BOOL limchk );
void	PcalMicroDqAcrGain(	ASIC_U_IF_STR	*pAsicMicroReg,	/* ����ϐ��|�C���^						*/
							DQACR_GAIN_PRM	*Prm,			/* �p�����[�^�f�[�^�|�C���^				*/
							BPRMDAT			*Bprm,			/* �x�[�X�p�����[�^�|�C���^ 			*/
							ULONG			Loophz,			/* ���x���[�v�Q�C��			[0.1Hz]		*/
							ULONG			ratio);			/* �Q�C���o�����X			[%]			*/

LONG	PcalDqAcrKp( LONG Gain, LONG Derate, BPRMDAT *Bprm, LONG axis_type );
LONG	PcalDqAcrKi( LONG Ti, LONG Kp, KSGAIN MotL, BPRMDAT *Bprm );
LONG	PcalDqAcrFilter( LONG Tf );
#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
LONG	MicroTranslatePrmReq( ASICS *SvAsicRegs, void *Data, VUSHORT *Address, LONG Size );
#endif


void PcmKvvIniPrmCalc( MPKVVCOMP *MpKvvComp, BPRMDAT *Bprm,
					   USHORT kvvfil, USHORT pnvoltgn, USHORT vrevgn, BOOL enable );
void	PcmWFPrmCalc( WEAKENFIELD *wf_ptr, LONG MotSpd, LONG DcVolt );		/* <S024> */
void	PcmMicroKvvCalc( MPKVVCOMP *MpKvvComp, LONG DcVolt, USHORT UvLevel );
void	PcalVFBKp( WEAKENFIELD *WeakenField, ASICS *SvAsicRegs, BPRMDAT *Bprm, USHORT kv, USHORT tv );
void	PcalVFBKi( WEAKENFIELD *WeakenField, ASICS *SvAsicRegs, BPRMDAT *Bprm, USHORT kv);
void	PcalIdrefLimTerm1( WEAKENFIELD *WeakenField, ASICS *SvAsicRegs, BPRMDAT *Bprm, LONG v1max, LONG vmaxid);
void	PcalIdrefLimTerm234(WEAKENFIELD* weaken_ptr, ASICS *SvAsicRegs, BPRMDAT* Bprm, DBYTEX oslv_pol, USHORT pnvoltgn);


#endif /* MICRO_PRM_CALC_H */
/***************************************** end of file **********************************************/
