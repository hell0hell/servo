#ifndef __TABLE_DRIVE_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		TableDrive.h : �e�[�u���^�]�p	���W���[����`												*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2011.02.18	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "CtrlLoopHandler.h"
#include "MfcHandler.h"
#include "PrmAccessIf.h"
#include "DataTraceManager.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�[������w�ߒ�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef union {							/* �e�[�u���^�]����w�ߒ�`									*/
	struct	{							/* �[�����䃂�[�h�����o��`									*/
		UCHAR	zm;						/* �[�����䃂�[�h											*/
		UCHAR	zmchg;					/* �[�����䃂�[�h�ω����									*/
		UCHAR	zmlast;					/* �\��														*/
		UCHAR	Dummy;					/* �\��														*/
	} b;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;							/* ���䃂�[�h�ҏW�p(Copy,etc)								*/
} TBLZMD;


/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s����p��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TBLDRIVE {
	struct {
		USHORT	TblDriveDataSize;		/* 0x2101 : �e�[�u���^�]�f�[�^��							*/
		USHORT	TblDriveSampIntv;		/* 0x2103 : �e�[�u���^�]�T���v�����O�Ԋu					*/
		USHORT	TblMode;				/* 0x2300 : �e�[�u���^�]���䃂�[�h							*/
		USHORT	TblLimit;				/* 0x2301 : �e�[�u���^�]��]��(�ړ���)���~�b�g				*/
		USHORT	TblState;				/* 0x2302 : �e�[�u���^�]����								*/
		USHORT	TblFuncSel;				/* 0x2303 : �e�[�u���^�]�@�\�I��							*/
	} OpeReg;
	/*----------------------------------------------------------------------------------------------*/
	struct {
		UCHAR	TblPrmOk;				/* �e�[�u���^�]�p�����[�^�n�j								*/
		UCHAR	TblMode;				/* �e�[�u���^�]���[�h										*/
		USHORT	CtrlMode;				/* �e�[�u���^�]���䃂�[�h									*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	TrqInputPos;			/* �e�[�u���^�]�g���N���͈ʒu								*/
		UCHAR	TuneLessDisable;		/* �������X���䖳��											*/
		UCHAR	EGearDisable;			/* �d�q�M�A�䖳��											*/
		UCHAR	PcmdFilDisable;			/* �ʒu�w�߃t�B���^����										*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	MotPulse;				/* ���[�^�p���X�� [pulse/rev, pulse/mm]						*/
		LONG	MtnLimit;				/* �}���`�^�[�����~�b�g										*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	RefType;				/* �w�߃^�C�v												*/
		UCHAR	RefIpSft;				/* �w�ߕ�ԃV�t�g��											*/
		USHORT	RefIpNum;				/* �w�ߕ�ԉ�												*/
		ULONG	TblDataNum;				/* �e�[�u���^�]�w�߃f�[�^��								*/
		ULONG	DataSampIntv;			/* �e�[�u���f�[�^�T���v�����O�Ԋu							*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	KoutDir;				/* �e�[�u���^�]�w�ߏo�̓Q�C��(���K���Q�C���{����)			*/
		LONG	*RefOutPx;				/* �e�[�u���^�]�w�ߏo�̓|�C���^								*/
	/*----------------------------------------------------------------------------------------------*/
		void	*TblBuffer;				/* �e�[�u���^�]�w�߃o�b�t�@									*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {
		TBLZMD	TblZmd;					/* �e�[�u���^�]�[�����䃂�[�h�w��							*/
		UCHAR	TblCmd;					/* �e�[�u���^�]���s�w��										*/
		UCHAR	TblStep;				/* �e�[�u���^�]���s�X�e�b�v									*/
		USHORT	TblSamp;				/* �e�[�u���^�]�T���v�����O									*/
		USHORT	TblIndx;				/* �e�[�u���^�]�C���f�N�X									*/
		USHORT	MregCsw;				/* ASIC�}�C�N���g���N�t�B���^����(�����^����)�p				*/
		USHORT	ErrState;				/* �ُ���													*/
		USHORT	Dummy;					/* For Alignment											*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	MotMtn;					/* �e�[�u���^�]�}���`�^�[��			[rev,mm]				*/
		LONG	MotPos;					/* �e�[�u���^�]���[�^�ʒu			[pulse]					*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	TrqRef;					/* �e�[�u���^�]�g���N�w�ߏo��		[NorTrq]				*/
		LONG	SpdRef;					/* �e�[�u���^�]���x�w�ߏo��			[NorSpd]				*/
		LONG	dPosRef;				/* �e�[�u���^�]�ʒu�w�ߏo��			[pulse/scan]			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	InPosRef;				/* �e�[�u���^�]�ʒu�w�ߕ�ԉ��Z�p	[pulse/samp]			*/
		LONG	IxPosRef;				/* �e�[�u���^�]�ʒu�w�ߕ�ԉ��Z�p	[pulse/scan]			*/
		LONG	IxPosRem;				/* �e�[�u���^�]�ʒu�w�ߕ�ԉ��Z�p	[pulse]					*/
		LONG	WkPosRem;				/* �e�[�u���^�]�ʒu�w�ߕ�ԉ��Z�p	[pulse]					*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL	KeepTblMode;			/* �e�[�u���^�]�p���t���O									*/
		BOOL	TrqFilDisable;			/* �g���N�w�߃t�B���^�����v��								*/
		BOOL	NotchLpassDisable;		/* �m�b�`�����[�p�X�t�B���^�����v��							*/
		LONG	DisTrqInTiming;			/* �O���g���N�w�ߓ��͈ʒu									*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL	WithTrcReq;				/* �e�[�u���^�]�����g���[�X�v��								*/
	} var;
	TRCHNDL		*TrcHndl;				/* �f�[�^�g���[�X�n���h�� */
} TBLDRIVE;


typedef	struct TBLCHK_SV_STS {
	BOOL	AlmFlag;				/* �A���[���������t���O											*/
	BOOL	HwbbSts;				/* HWBB��Ԍ��o�p�f�[�^											*/
	BOOL	MainPowerOn;			/* ���HON���													*/
	BOOL	OverTrvlSts;			/* OT���(OT�V�[�P���X�����o��)									*/
	BOOL	BeComplete;				/* �x�[�X�C�l�[�u���������										*/
	BOOL	TrqClamp;				/* �g���N�������t���O											*/
	LONG	dMotPos;				/* �ʒuFB�����l[pulse/ScanB]									*/
} TBLCHK_SV_STS;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s�w�ߒ�`																	*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_EXE_STS {
	TBLCMD_NOCMD		= 0x00,				/* �w�߂Ȃ�												*/
	TBLCMD_TABLESTART,						/* �e�[�u���^�]�����J�n�w��								*/
	TBLCMD_TABLEDRIVE,						/* �e�[�u���^�]���s�J�n�w��								*/
	TBLCMD_TBLDRV_END,						/* �e�[�u���^�]���s����I��								*/
	TBLCMD_TBLDRV_ERR	= 0x0E,				/* �e�[�u���^�]���s�ُ�I��								*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]��Ԓ�`																		*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_SERVO_STS {
	TBLCMD_STS_INIT			= 0,			/* �C�j�V�������										*/
	TBLCMD_STS_RUN,							/* �e�[�u���^�]��										*/
	TBLCMD_STS_FINISH,						/* �e�[�u���^�]����										*/
	TBLCMD_STS_POSLMT		= 100,			/* ��]��/�ړ��ʃ��~�b�g���o							*/
	TBLCMD_STS_TRQLMT,						/* �g���N�������o										*/
	TBLCMD_STS_BB,							/* �T�[�{�I�t											*/
	TBLCMD_STS_OT,							/* �I�[�o�g���x��										*/
	TBLCMD_STS_POWEROFF,					/* ���H�d���I�t										*/
	TBLCMD_STS_ALARM,						/* �A���[������											*/
	TBLCMD_STS_HWBB,						/* HWBB���												*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�w�߃^�C�v��`																	*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_REF_TYPE {
	TBLREFTYPE_NOTPOS		= 0x00,			/* �ʒu�w�߈ȊO(���x,�g���N)							*/
	TBLREFTYPE_POSIP1,						/* �ʒu�w�߁F��Ԗ���									*/
	TBLREFTYPE_POSIP2,						/* �ʒu�w�߁F�V�t�g���									*/
	TBLREFTYPE_POSIP3,						/* �ʒu�w�߁F���Z���									*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�w�߃f�[�^����`																*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_REF_LENGTH {
	TBLREFLENGTH_NOTDEF		= 0x00,			/* ����`(SHORT)										*/
	TBLREFLENGTH_15BIT_IO,					/* Data:15bit + IO:1bit									*/
	TBLREFLENGTH_31BIT_IO,					/* Data:31bit + IO:1bit									*/
	TBLREFLENGTH_16BIT,						/* Data:16bit											*/
	TBLREFLENGTH_32BIT,						/* Data:32bit											*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�I���r�b�g��`																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	TBLFUNCBIT_TUNELESS		0x0100		/* �������X����											*/
#define	TBLFUNCBIT_EGEAR		0x0200		/* �d�q�M�A��											*/
#define	TBLFUNCBIT_PCMDFIL		0x0400		/* �ʒu�w�߃t�B���^										*/


/* �e�[�u���^�]�o�b�t�@��1���ɂ�0x1000[2byte]�Ƃ���B��-�X��0xC00[2byte]�Ȃ̂ŗ]�T���� */
//#define	TBL_BUF_SIZE		0x30000		/* �e�[�u���^�]�p�w�߃o�b�t�@�T�C�Y[2byte]				*/
//#define	AX_TBL_BUF_SIZE		0x6000		/* �����̃e�[�u���^�]�p�w�߃o�b�t�@�T�C�Y[2byte]		*/
#define		AX_TBL_BUF_SIZE		0x1000		/* �����̃e�[�u���^�]�p�w�߃o�b�t�@�T�C�Y[2byte]		*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void		TblDriveInitialize( TBLDRIVE *TblDrive, USHORT *BufferAddress, TRCHNDL *TrcHndl );
void		TblDriveGetTraceRegData( TBLDRIVE *TblDrive, USHORT DataSize, USHORT SampIntv,
																ULONG DataSampIntv, ULONG DataNUm );
void		TblDriveExecute( TBLDRIVE *TblDrive, TBLCHK_SV_STS *TblChkSts,
														POS_MNG_HNDL *PosManager, MFCTRL *MFControl );
PRM_RSLT	TblDriveCalculateParameter( TBLDRIVE *TblDrive, LONG XfbPulse1 );
PRM_RSLT	TblDrivePrepareProc( TBLDRIVE *TblDrive );
PRM_RSLT	TblDriveStartRequest( TBLDRIVE *TblDrive, LONG TblDir );
PRM_RSLT	TblDriveSetStopRequest( TBLDRIVE *TblDrive );
USHORT		TblDriveGetControlMode( TBLDRIVE *TblDrive );
void		TblDriveSetWithTrcReq( TBLDRIVE *TblDrive, BOOL Switch );




/****************************************************************************************************/
#define __TABLE_DRIVE_H
#endif /* __TABLE_DRIVE_H */
