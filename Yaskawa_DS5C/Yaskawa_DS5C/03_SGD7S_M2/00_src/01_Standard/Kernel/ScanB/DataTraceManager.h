#ifndef	_TRACE_MANAGER_H_
#define	_TRACE_MANAGER_H_

/****************************************************************************************************/
/*																									*/
/*																									*/
/*		DataTraceManager.h : �f�[�^�g���[�X��`�w�b�_�t�@�C��										*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.10.26	T.Kira	For INGRAM												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "PrmAccessIf.h"
#include "MemobusIF.h"


/*--------------------------------------------------------------------------------------------------*/
/*		Variable Type Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X���s�w�ߒ�`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	TRCCMD_NOCMD			0x00				/* �w�߂Ȃ�										*/
#define	TRCCMD_DATATRACE		0x01				/* �g���[�X���s�w��								*/
#define	TRCCMD_RINGTRACE		0x02				/* �A���g���[�X���s�w��							*/
#define	TRCCMD_CONTTRACE		0x03				/* �g���[�X���s�w��(���[�h�ێ�)					*/
/*--------------------------------------------------------------------------------------------------*/
#define TRC_CH_MAXNUM			16					/* �g���[�X�`�����l����							*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPEMODE_NOCMD			0x0000				/* �ʏ탂�[�h									*/
#define	OPEMODE_DATATRACE		0x2000				/* �f�[�^�g���[�X���[�h							*/
#define	OPEMODE_RINGTRACE		0x200A				/* �A���g���[�X���[�h							*/
/*--------------------------------------------------------------------------------------------------*/
#if defined(_TEST_BOARD_)
#define TRC_BUF_SIZE			0x220				/* �f�[�^�g���[�X�o�b�t�@�T�C�Y(USHORT)			*/
#else
#define TRC_BUF_SIZE			0x4000				/* �f�[�^�g���[�X�o�b�t�@�T�C�Y(USHORT)			*//* <S04C> */
#endif
/*--------------------------------------------------------------------------------------------------*/
#define DATATRACE_SHORT			0x00				/* �V���[�g�f�[�^�g���[�X						*/
#define DATATRACE_LONG			0x01				/* �����O�f�[�^�g���[�X							*/
/*--------------------------------------------------------------------------------------------------*/
#define IODATA_MERGE			0x00				/* IO�f�[�^��ϐ��f�[�^�Ɍ���					*/
#define IODATA_DIV				0x01				/* IO�f�[�^���p�̈�Ɋi�[						*/
/*--------------------------------------------------------------------------------------------------*/
#define SINGLE_TRG				0x00				/* �V���O���g���K�f�[�^�g���[�X					*/
#define MULTI_TRG				0x01				/* �}���`�g���K�f�[�^�g���[�X					*/
/*--------------------------------------------------------------------------------------------------*/
#define TRG_SEL2_USE			0x0F				/* �g���K�f�[�^�I��2���g�p				*//* <S012> */
#define NET_TRG_SEL				0x0F				/* �l�b�g���[�N�g���K���g�p				*//* <S012> */

//#define _DATA_TRACE_DBUG_

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X��`�\����																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X����G���A�F2100H - 220AH															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCREG{
	USHORT	TrcMode;						/* 0x2100 : �g���[�X���[�h								*/
	USHORT	DataSize;						/* 0x2101 : �g���[�X�f�[�^��							*/
	//USHORT	AreaSize;					/* 0x2102 : �g���[�X�o�b�t�@��							*/	/* <S0C9> */
	USHORT	AreaSize;						/* 0x2102 : �g�p�G���A�T�C�Y							*/	/* <S0C9> */
	USHORT	SampIntv;						/* 0x2103 : �g���[�X�T���v�����O�Ԋu					*/
	USHORT	SampTime;						/* 0x2104 : �g���[�X�T���v�����O�P�� [Us]				*/
	USHORT	SampLlim;						/* 0x2105 : �g���[�X�T���v�����O�Ԋu����				*/
	USHORT	SampUlim;						/* 0x2106 : �g���[�X�T���v�����O�Ԋu���				*/
	USHORT	MaxAreaSize;					/* 0x2107 : �ő�g���[�X�o�b�t�@��						*/
	USHORT	OpeModeTrc;						/* 0x2108 : �g���[�X��p���샂�[�h�ݒ�/�Q��				*/
	USHORT	OpeCmdTrc;						/* 0x2109 : �g���[�X��p����ݒ�						*/
	USHORT	TrcBuffSize;					/* 0x210A : �g���[�X�o�b�t�@��							*/	/* <S0C9> */
	USHORT	MaxTrcBuffSize;					/* 0x210B : �ő�g���[�X�o�b�t�@��						*/	/* <S0C9> */
	/*----------------------------------------------------------------------------------------------*/
/* <S0C9> Start */
//	USHORT	TrcSel[TRC_CH_MAXNUM];			/* 0x2110 : �f�[�^�g���[�X�ΏۑI��(1�`16)				*/
//	USHORT	IOTrcSel[TRC_CH_MAXNUM];		/* 0x2120 : I/O�g���[�X�ΏۑI��(1�`16)					*/
//	ULONG	*TrcAdr[TRC_CH_MAXNUM];			/* 0x2130 : �g���[�X�ΏۃA�h���X�w��(����J)			*/
	USHORT	DataIOTrcSel[TRC_CH_MAXNUM];	/* 0x2110 : �f�[�^�AI/O�g���[�X�ΏۑI��(1�`16)			*/
	USHORT	TrcAxisSel[TRC_CH_MAXNUM];		/* 0x2120 : �f�[�^�g���[�X�Ώێ��I��(1�`16)				*/
	USHORT	TrcDataSel[TRC_CH_MAXNUM];		/* 0x2130 : �f�[�^�g���[�X�Ώۃf�[�^�I��(1�`16)			*/
	USHORT	IOTrcAxisSel[TRC_CH_MAXNUM];	/* 0x2140 : I/O�g���[�X1�Ώێ��I��(1�`16)				*/
	USHORT	IOTrcDataSel[TRC_CH_MAXNUM];	/* 0x2150 : I/O�g���[�X1�Ώۃf�[�^�I��(1�`16)			*/
	ULONG	*TrcAdr[TRC_CH_MAXNUM];			/* 0x2160 : �g���[�X�ΏۃA�h���X�w��(����J)			*/
/* <S0C9> End */
	/*----------------------------------------------------------------------------------------------*/
	USHORT	PtrgDnum;						/* 0x2200 : �v���g���K�f�[�^��							*/
	USHORT	TrgSel;							/* 0x2201 : �g���K����									*/
	LONG	TrgLevel;						/* 0x2202 : �g���K���x��								*/
	USHORT	TrgPoint;						/* 0x2204 : �g���K�ʒu									*/
	USHORT	TrcState;						/* 0x2205 : �g���[�X����								*/
	ULONG 	RngTrcPtr;						/* 0x2206 : �A���g���[�X�����݈ʒu						*/
	/*----------------------------------------------------------------------------------------------*/
	USHORT	MTrgMode;						/* 0x2208 : �}���`�g���K�ݒ�							*/
	USHORT	MTrgSel;						/* 0x2209 : �g���K�����Q								*/
	LONG	MTrgLevel;						/* 0x220A : �g���K���x���Q								*/
} TRCREG;



/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X���s�p�ϐ���`																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X�^�e�[�u���^�]���s�p�ϐ��́ARound:DtrcMngr.c ����� ScanB:DtrcExec.c			*/
/*		�ɂ��Ǘ�(������)����A���̃��W���[���́A�Q�Ƃ̂݉\�ł���B								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���A�h���X��`																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union TRCADRDATA{
	USHORT	*ShortAdrData;					/* ���A�h���X�w��|�C���^								*/
	ULONG	*LongAdrData;					/* ���A�h���X�w��|�C���^								*/
} TRCADRDATA;
/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X�Ώے�`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCOBJ{						/* �g���[�X�Ώے�`										*/
	USHORT	TrcExeflg;						/* �g���[�X���s����t���O(0x01:���s,���̑�:�s���s)		*/
	void	*TrcBuf;						/* �g���[�X�f�[�^�i�[�o�b�t�@							*/
	TRCADRDATA	Adr;						/* ���A�h���X��`										*/

	struct {								/* ���l�g���[�X�ϐ�										*/
		void	*AxisPtr;					/* ���l�g���[�XAXISHANDLE								*/
		USHORT	VxSftL;						/* ���l�g���[�X�ϐ����V�t�g�X�P�[��						*/
		USHORT	VxSftR;						/* ���l�g���[�X�ϐ��E�V�t�g�X�P�[��						*/
		LONG	VxData;						/* ���l�g���[�X�f�[�^									*/
		LONG	VxGain;						/* ���l�g���[�X���Z�Q�C��								*/
		BOOL	AvrgSel;					/* ���l�g���[�X���ϑI��:TRUE:���ω�����,FALSE�˂��Ȃ�	*/
		LONG	(*GetData)( void *Axis );	/* �g���[�X�ϐ��A�h���X�擾�֐�							*/
	} TrcV;

	struct {
		//USHORT	BtvBit;					/* �r�b�g�g���[�X�ϐ��r�b�g�I��(�}�X�N)					*/
		void	*AxisPtr;					/* �r�b�g�g���[�XAXISHANDLE								*/
		BOOL	BtvData;					/* �r�b�g�g���[�X�ϐ��A�h���X							*/
		BOOL	(*GetData)( void *Axis );	/* �g���[�X�ϐ��A�h���X�擾�֐�							*/
	} TrcB;

} TRCOBJ;

/* <S0C9> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		�f�[�^�g���[�X�ʃp�����[�^(�Ώۃf�[�^�A�Ώێ�)											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCINDIVPRM{
	USHORT	TrcAxis[TRC_CH_MAXNUM];				/* �f�[�^�g���[�X�Ώێ��I��(1�`16)				*/
	USHORT	TrcData[TRC_CH_MAXNUM];				/* �f�[�^�g���[�X�Ώۃf�[�^�I��(1�`16)			*/
	USHORT	IOTrcAxis[TRC_CH_MAXNUM];			/* I/O�g���[�X1�Ώێ��I��(1�`16)				*/
	USHORT	IOTrcData[TRC_CH_MAXNUM];			/* I/O�g���[�X1�Ώۃf�[�^�I��(1�`16)			*/
} TRCINDIVPRM;
/* <S0C9> End */

/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X�g���K�ϐ�����`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCTRG{						/* �g���[�X���s�p�����[�^��`							*/
	USHORT	TrgSel;							/* �g���[�X�g���K�I��									*/
	USHORT	TrgEdge;						/* �g���[�X�g���K�G�b�W�w��								*/
	USHORT	TrgVsft;						/* �g���[�X�g���K�f�[�^�V�t�g							*/
	LONG	TrgLevel;						/* �g���[�X�g���K���x��									*/
	ULONG	TrgBitSetMsk;					/* �g���[�X�g���K�r�b�g�}�X�N							*/
	void	*TrgBufx;						/* �g���[�X�g���K�f�[�^�o�b�t�@							*/
}TRCTRG;

/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X���s�p�����[�^��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCEXEP{					/* �g���[�X���s�p�����[�^��`							*/
	USHORT	TrcPrmOk;						/* �g���[�X�p�����[�^�n�j								*/
	USHORT	TrcDataType;					/* �g���[�X�f�[�^�^�C�v									*/
	USHORT	TrcIOType;						/* IO�g���[�X�f�[�^�^�C�v								*/
	ULONG	TrcDnum;						/* �g���[�X�f�[�^��									*/
	ULONG	DataSampIntv;					/* �g���[�X�f�[�^�T���v�����O�Ԋu						*/
	LONG	UpperLmt;						/* �g���[�X�f�[�^���									*/
	LONG	LowerLmt;						/* �g���[�X�f�[�^����									*/
	ULONG	TrcBitSet;						/* �g���[�X�r�b�g�Z�b�g�p								*/
	ULONG	TrcBitClr;						/* �g���[�X�r�b�g�N���A�p								*/
	/*----------------------------------------------------------------------------------------------*/
	TRCTRG	TrcTrg[2];						/* �g���[�X�g���K�ϐ����i�}���`�g���K�Ή��j			*/
	TRCOBJ	TrcObj[TRC_CH_MAXNUM];			/* �g���[�X�ϐ����										*/
} TRCEXEP;

/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X���s����p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCEXEV{					/* �g���[�X���s����p�ϐ���`							*/
	USHORT	TrcCmd;							/* �g���[�X���s�w��										*/
	USHORT	Opened;	 						/* �g���[�X�@�\���I�[�v��								*/
	USHORT	OpmRun;							/* �g���[�X���샂�[�h���s��(OpeMode����p)				*/
	USHORT	TrcStep;						/* �g���[�X���s�X�e�b�v									*/
	USHORT	TrcSamp;						/* �g���[�X�T���v�����O									*/
	USHORT	TrcIndx;						/* �g���[�X�C���f�N�X									*/
	USHORT  RngTrcCnt;						/* �A���g���[�X�������݉�								*/
	LONG	TrgVar[2];						/* �g���K�`�F�b�N�ϐ�(����l,�O��l)					*/
	LONG	MTrgVar[2];						/* �g���K�`�F�b�N�ϐ�(����l,�O��l)(�}���`�g���K�Ή�)	*/
	BOOL	NetTrigger;						/* �l�b�g���[�N�g���K���								*//* <S012> */
} TRCEXEV;

/*--------------------------------------------------------------------------------------------------*/
/*		�f�[�^�g���[�X�Ǘ��p�ϐ���`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCMNGP{					/* �g���[�X�Ǘ��p�����[�^��`							*/
	ULONG	TrcVarTblEntNum;				/* ���l�g���[�X��`�e�[�u���o�^��						*/
	ULONG	TrcVarTblSrhIniW;				/* ���l�g���[�X��`�e�[�u�����������������l				*/
	ULONG	TrcBitTblEntNum;				/* �r�b�g�g���[�X��`�e�[�u���o�^��					*/
	ULONG	TrcBitTblSrhIniW;				/* �r�b�g�g���[�X��`�e�[�u�����������������l			*/
} TRCMNGP;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	TRCMNGV{					/* �g���[�X�Ǘ��f�[�^�ϐ�								*/
	ULONG	TrcOpeRegWrtCnt;				/* �p�����[�^�v�Z���s�`�F�b�N�p							*/
	ULONG	TdrvOpeRegWrtCnt;				/* �p�����[�^�v�Z���s�`�F�b�N�p							*/
} TRCMNGV;

/****************************************************************************************************/
/*																									*/
/*		�g���[�X���s�ϐ���`																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct	TRCHNDL{
	TRCREG	TrcReg;								/* �g���[�X����G���A���W�X�^						*/
	TRCEXEP	TrcExeP;							/* �g���[�X���s�p�����[�^							*/
	TRCEXEV	TrcExeV;							/* �g���[�X���s����p�ϐ�							*/
	TRCMNGP	TrcMngP;							/* �g���[�X�Ǘ��p�����[�^							*/
	TRCMNGV	TrcMngV;							/* �g���[�X�Ǘ��f�[�^�ϐ�							*/
	MEMOBUS_ADDR_MAP AxisMap[MAX_AXIS_NUMBER];	/* ���A�h���X��`									*/
	TRCINDIVPRM	TrcIncivPrm;					/* �g���[�X�ʃp�����[�^							 */	/* <S0C9> */
}TRCHNDL;

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void		DtrcInitDtrcManager( TRCHNDL *TrcHndl, USHORT *TraceBuffer,	MEMOBUS_ADDR_MAP *AxisMap );
PRM_RSLT	DtrcCalcParameter( TRCHNDL *TrcHndl, USHORT *TraceBuffer );
PRM_RSLT	DtrcWriteOpeModeTrcRegister( TRCHNDL *TrcHndl, USHORT WrData );
PRM_RSLT	DtrcWriteOpeCmdTrcRegister( TRCHNDL *TrcHndl, USHORT *TraceBuffer, USHORT WrData );
void		DtrcExecute( TRCHNDL *TrcHndl );
void		DtrcProcedure( TRCHNDL *TrcHndl, LONG Idx );
void		DtrcSetTrcVar( TRCHNDL *TrcHndl );
void		DtrcResetAverageVar( TRCHNDL *TrcHndl );
void		DtrcInitExecute( TRCHNDL *TrcHndl );
void		DtrcInitExecuteRingTrc( TRCHNDL *TrcHndl );
void		DtrcSetNetworkTrigger( TRCHNDL *TrcHndl, BOOL Trigger ); /* <S012> */

/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
ULONG	DtrcGetSampIntv( TRCHNDL *TrcHndl );
ULONG	DtrcGetTrcDataNum( TRCHNDL *TrcHndl );
void	DtrcSetTrgPointToZero( TRCHNDL *TrcHndl );
LONG	DtrcSearchTrcVarTbl( TRCHNDL *TrcHndl, LONG SelNo );

/***************************************** end of file **********************************************/
#endif	// _TRACE_MANAGER_H_
