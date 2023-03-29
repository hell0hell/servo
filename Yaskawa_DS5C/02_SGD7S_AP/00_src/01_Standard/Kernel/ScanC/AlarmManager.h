/****************************************************************************
Description	: Alarm Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.11.02     created

****************************************************************************/
#ifndef ALARM_MANAGER_H

#include "alarm.h"
#include "EepromIF.h"
#include "EncData.h"

/*****************************************************************************
*		�A���[���Ǘ��e�[�u������`
*****************************************************************************/
#define	ALM_BITTBL_NUM		8	/* �A���[���o�^�r�b�g�e�[�u�������O���[�h��	*/
#define	KPX_ALMTBLMAX	32					/* �A���[���o�^�e�[�u���ő��	*/
#define	KPX_ALMTRCMAX	10					/* �A���[�������e�[�u���ő��	*/
#define KPX_ALMDEFMAX	(32 * ALM_BITTBL_NUM)	/* �A���[����`���ő�l	*/
#define KPX_WRNDEFMAX	(32 * WRN_BITTBL_NUM)	/* ���[�j���O��`���ő�l	*/


/*****************************************************************************
*	�A���[��History�Ǘ��\���̒�`
*****************************************************************************/
/* Alarm Table	*/
typedef struct ALM_TBL {
	ALMID_T			AlmId;					/* �A���[���h�c					*/
	CPALM_TBL_DEF	AlmDef;					/* �A���[����`					*/
	ULONG			TmStamp;				/* �A���[�����o�^�C���X�^���v	*/
} ALM_TBL;

/* Alarm Trace Header */
typedef union ALM_TRACE_HDR {
	USHORT			w;
	struct {
		UCHAR		Idx;
		UCHAR		Cnt;
	} b;
} ALM_TRACE_HDR;

/* Alarm Trace Data Table */
typedef struct ALM_TRACE_DATA {
	USHORT			AlmCode;				/* �A���[���R�[�h				*/
	ULONG			TmStamp;				/* �A���[�����o�^�C���X�^���v	*/
} ALM_TRACE_DATA;

/* Alarm Management Data */
typedef	struct ALM_HISTORY {
	UINT			AlmCnt;					/* �A���[����(������)			*/
	UINT			LstAlmCnt;				/* ����o�^��	<S0FF>			*/
//	UINT			LstAlmDetCnt;			/* �A���[��������	<S0FF>		*/

	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];	/* �A���[���o�^�e�[�u��(A&W���p)*/

	ALM_TRACE_HDR	AlmTrcHdr;				/* �A���[������Index&Count		*/
	ALM_TRACE_DATA	AlmTrc[KPX_ALMTRCMAX];	/* �A���[�������e�[�u��			*/
	ALM_TRACE_DATA	*AlmTrcSp;				/* �A���[������O��i�[�|�C���^	*/

	ULONG			EepAlmOLInfo;			/* �A���[�������ߕ��׏�� */
} ALM_HISTORY;


/*****************************************************************************
*	�A���[���Ǘ��h�e�\���̒�`
*****************************************************************************/
/* �ʐM�p�g���A���[������` */
typedef struct ALM_EX_INFO {
	BOOL	DevAlm;						/*	�@��A���[�� */
	BOOL	DevWrn;						/*	�@�탏�[�j���O	*/
	BOOL	CmdAlm;						/*	�R�}���h�A���[�� */
	BOOL	CommAlm;					/*	�ʐM�A���[�� */
} ALM_EX_INFO;

/*	<S0FF>	*/
typedef struct ALM_STATUS {
	BOOL	AlmFlag			:1;			/* �A���[���������t���O				*/
	BOOL	WrnFlag			:1;			/* ���[�j���O�������t���O			*/
	BOOL	MencDataNG		:1;			/* ���[�^�G���R�[�_�f�[�^����		*/
	BOOL	PosDataNG		:1;			/* �ʒu�f�[�^����					*/

	BOOL	DevAlm			:1;			/*	�@��A���[�� */
	BOOL	DevWrn			:1;			/*	�@�탏�[�j���O	*/
	BOOL	CmdAlm			:1;			/*	�R�}���h�A���[�� */
	BOOL	CommAlm			:1;			/*	�ʐM�A���[�� */

	UINT	StopMode		:4;
	UINT	AlmOLInfo		:4;

	USHORT	AlmCode;					/* �A���[���R�[�h���				*/
} ALM_STATUS;


/* �p�����[�^�G���[����`	*/
typedef	struct PRMERR {
	UCHAR	ErrFlag;					/* �G���[�t���O(�I�����C���p)		*/
	UCHAR	ErrCount;					/* �G���[�J�E���g(�I�t���C���p)		*/
	USHORT	ErrPrmNo;					/* �p�����[�^�ԍ�(�p�����[�^�G���[)	*/
} PRMERR;

/* �A���[���R�[�h����`	*//* <S04B> */
typedef	struct ALM_CODE_OUT {
	BOOL	Alo1;						/* ALO1 */
	BOOL	Alo2;						/* ALO2 */
	BOOL	Alo3;						/* ALO3 */
} ALM_CODE_OUT;


/* �A���[���Ǘ��h�e�f�[�^��` */
typedef	struct ALMMAN_HANDLE {
/* Public member */
	BOOL			AlmRst;				/* ALM���Z�b�g�v��					*/
	BOOL			WrnRst;				/* Warning���Z�b�g�v��				*/
	BOOL			NetCmdAlmRst;		/* �ʐM�pCMD_ALM���Z�b�g�v��		*/
	BOOL			AlmEepRst;			/* EEPROM�A���[������v��Flag		*/

	ALM_STATUS		Status;				/* alarm status <S0FF>				*/
	BOOL			AlmReady;			/* �A���[�����f�B					*/
	BOOL			WrnFlagX;			/* <S0FF>							*/
//<S0FF>	BOOL			AlmFlag;			/* �A���[���������t���O				*/
//<S0FF>	BOOL			WrnFlag;			/* ���[�j���O�������t���O			*/
//<S0FF>	BOOL			MencDataNG;			/* ���[�^�G���R�[�_�f�[�^����		*/
//<S0FF>	BOOL			PosDataNG;			/* �ʒu�f�[�^����					*/
//<S0FF>	ALM_EX_INFO		AlmExInfo;			/* �g���p�A���[�����				*/
//<S0FF>	ULONG			AlmOLInfo;			/* �A���[�������ߕ��׏��			*/
//<S0FF>	ULONG			StopMode;			/* �A���[���������̒�~���@			*/

	USHORT			AlmCode;			/* �A���[���R�[�h���				*/
//	USHORT			AlmCodeOut;			/* �A���[���R�[�h�M���o��			*//* <S04B> */
	ALM_CODE_OUT	AlmCodeOut;			/* �A���[���R�[�h�M���o��			*//* <S04B> */
	PRMERR			PrmErrInfo;			/* �p�����[�^�G���[���				*/
	ALM_HISTORY		Trace;				/* Alarm trace information			*/

/* Private member(do not write these values from other modules) */
	ULONG			AlmSet[ALM_BITTBL_NUM];		/* �A���[���o�^�r�b�g		*/
	ULONG			AlmSetMsk[ALM_BITTBL_NUM];	/* �A���[���}�X�N�r�b�g		*/
	ULONG			AlmRstMsk[ALM_BITTBL_NUM];	/* �A���[�����Z�b�g�s�r�b�g*/
	ULONG			WrnRstMsk[ALM_BITTBL_NUM];	/* �A���[�����Z�b�g�s�r�b�g*/

	ULONG			StopModeParam;		/* �A���[���������̒�~���@			*/
	CPALM_TBL_DEF	DefTable;			/* Pointer to Alarm table */
	UINT			TableEntryNum;		/* Number of Table entry */
	void			*mutex;				/* for Interlock */

	UINT				myAsicNo;		/* axis number */
//	EEP_DEVICE_HANDLE	*EepDevice;		/* Eeprom Device handle *//* <Oka02S1D2> */
	EEP_QUE_BUFFER		EepQueBuf[32];	/* Eeprom Queue buffer */
	EEP_QUE_HANDLE		EepQue;			/* Eeprom Queue */
	volatile BOOL		AlmEepRstBusy;	/* EEPROM�A���[������BusyFlag		*/

	USHORT				IntAlmId;		/* �N�������o�A���[��ID */
	BOOL				IntAlmEnable;	/* �N�������o�A���[���L�� */
	BOOL				OCAlmOccurFlag1;/* �ߓd��(OC1)�A���[��(A.100)�����t���O(�����o�͐M�����j�^�p) */	/* <S1A0> *//* <S1C9> */
	BOOL				OCAlmOccurFlag2;/* �ߓd��(OC2)�A���[��(A.100)�����t���O(�����o�͐M�����j�^�p) */	/* <S1C9> */
	BOOL				WrnOutSel;		/* ���[�j���O�R�[�h�o�͑I�� */	/* <S1A8> */

	USHORT				LtAlmReq;		/* �A���[�����b�`EEPROM�������ݗv��				 <S1CF> */
	USHORT				LtDtWtCt;		/* �A���[�����b�`�f�[�^�������ݒ��J�E���^�t���O	 <S1CF> */
	ULONG				LtRomAdr;		/* ��������EEPROM�A�h���X						 <S1CF> */
	USHORT				*LtMemAdr;		/* �������݃������[�A�h���X						 <S1CF> */
	USHORT				*AlmCdAdr;		/* �A���[���R�[�h�i�[�A�h���X					 <S1CF> */
	ULONG				*TmStpAdr;		/* �A���[���^�C���X�^���v�i�[�A�h���X			 <S1CF> */

	void				*hAlmLatchHndl;	/* �A���[�����b�`�p�f�[�^						 <S212> */

	BOOL				AlmClear;		/* ����A���[���̃N���A�v��						 <S226> */
	ALMID_T				ClrAlmId;		/* �N���A�A���[���h�c							 <S226> */

} ALMMAN_HANDLE;



/* API's */

/* for all axises */
void ALMInitGlobalHandle(ALMMAN_HANDLE **AxisTopHandle, LONG AxisNum);
void ALMSetGlobalAlarm(ALMID_T alm_id);
void ALMClearGlobalAlarm(ALMID_T alm_id);
BOOL ALMCheckGlobalAlarm(ALMID_T alm_id);

/* for an axis */
void ALMInitAlarmManager(ALMMAN_HANDLE *AlmMngr, CPALM_TBL_DEF AlmDefTbl,
						 UINT AlmDefCntx, UINT AxisNumber);
ULONG ALMStartupProcedure(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam,
										LONG AxisMotType, LONG AbsoEncType);
void ALMRuntimeProcess(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam);
void ALMRuntimeProcess2(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam);		/* <S226> */
void ALMHistoryWriteService(ALMMAN_HANDLE *AlmMngr);

CPALM_TBL_DEF ALMSetServoAlarm(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
void ALMSetMask(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id, BOOL MaskSet);
BOOL ALMCheckEachState(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
BOOL ALMCheckSetMask(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);	/* @@ add */
void ALMSetPramError(ALMMAN_HANDLE *AlmMngr, USHORT ErrPrmNo);
void ALMClearWarning(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
BOOL ALMGetOtherAlarmChk(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
ULONG ALMGetStatus(ALMMAN_HANDLE *AlmMngr, BOOL *AlmFlag, BOOL *WrnFlag);
BOOL ALMGetExNetStatus(ALMMAN_HANDLE *AlmMngr, ALM_EX_INFO *AlmExInfo);
void ALMGetNetAlarmTableInfo(ALMMAN_HANDLE *AlmMngr, USHORT *AlmInfo, UINT Index);
void ALMGetNetAlarmTraceInfo(ALMMAN_HANDLE *AlmMngr, USHORT *AlmInfo, UINT Index);
USHORT ALMGetAlarmHistory(ALMMAN_HANDLE *AlmMngr, UINT Index, ULONG *TimeStamp);
LONG ALMGetNetAlarmTableCode(ALMMAN_HANDLE *AlmMngr, UCHAR *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmTableDetailCode(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmNum(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetNetWarningNum(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmTraceCode(ALMMAN_HANDLE *AlmMngr, UCHAR *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmTraceDetailCode(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetAlarmInfo(ALMMAN_HANDLE *AlmMngr);
void ALMSetAlarmResetRequest(ALMMAN_HANDLE *AlmMngr);
void ALMSetWarningResetRequest(ALMMAN_HANDLE *AlmMngr);
BOOL ALMGetAlarmResetStatus(ALMMAN_HANDLE *AlmMngr);
BOOL ALMGetWarningResetStatus(ALMMAN_HANDLE *AlmMngr);
void ALMSetAlarmHistResetRequest(ALMMAN_HANDLE *AlmMngr);
BOOL ALMGetAlarmHistResetStatus(ALMMAN_HANDLE *AlmMngr);
void SetOutputAloSignals( ALMMAN_HANDLE *AlmMngr );	/* <S1A8> */
void LcdEep2EepRomWrite( UINT devId );	/* <S208> */

#define ALARM_MANAGER_H
#endif /* ALARM_MANAGER_H */
