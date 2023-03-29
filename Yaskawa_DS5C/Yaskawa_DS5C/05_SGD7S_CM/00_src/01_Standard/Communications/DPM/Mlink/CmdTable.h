/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g          */
/*                                                                          */
/*            �R�}���h�Ǘ��e�[�u��	�w�b�_�[�t�@�C��						*/
/*																			*/
/****************************************************************************/
/* �h�e��`�֐� :                                                           */
/*																			*/
/*                                                                          */
/****************************************************************************/
/*	�y���ŗ����z															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/
#ifndef		CMD_TABLE_H_
#define		CMD_TABLE_H_


/* �^�C���A�E�g���Ԓ�` */
#define TIMEOUT_OF_NOP			0
#define TIMEOUT_OF_PRM_RD		1000
#define TIMEOUT_OF_PRM_WR		1000
#define TIMEOUT_OF_ID_RD		1000
#define TIMEOUT_OF_CONFIG		5000
#define TIMEOUT_OF_ALM_RD		1000
#define TIMEOUT_OF_ALM_CLR		1000
#define TIMEOUT_OF_SYNC_SET		10000
#define TIMEOUT_OF_CONNECT		10000
#define TIMEOUT_OF_DISCONNECT	10000
#define TIMEOUT_OF_PPRM_WR		1000
#define TIMEOUT_OF_POS_SET		0
#define TIMEOUT_OF_BRK_ON		0
#define TIMEOUT_OF_BRK_OFF		0
#define TIMEOUT_OF_SENS_ON		2000
#define TIMEOUT_OF_SENS_OFF		2000
#define TIMEOUT_OF_HOLD			0
#define TIMEOUT_OF_LTMOD_ON		0
#define TIMEOUT_OF_LTMOD_OFF	0
#define TIMEOUT_OF_SMON			0
#define TIMEOUT_OF_SV_ON		15000
#define TIMEOUT_OF_SV_OFF		0
#define TIMEOUT_OF_INTERPOLATE	0
#define TIMEOUT_OF_POSING		0
#define TIMEOUT_OF_FEED			0
#define TIMEOUT_OF_LATCH		0
#define TIMEOUT_OF_EX_POSING	0
#define TIMEOUT_OF_ZRET			0
#define TIMEOUT_OF_VELCTRL		0
#define TIMEOUT_OF_TRQCTRL		0
#define TIMEOUT_OF_ADJ			10000
#define TIMEOUT_OF_SVCTRL		1000
#define TIMEOUT_OF_NO_DEFINE	0

/* �萔��` */
#define	CMD_NUMBER		0x100	/* �R�}���h��`Index�e�[�u���ɓo�^����Ă���R�}���h�� *//* <V715> */

/* ���C���R�}���h  �R�}���h�R�[�h�C���f�b�N�X */
enum {	Idx_NO_DEFINE,	Idx_NOP,		Idx_PRM_RD,		Idx_PRM_WR,		Idx_ID_RD,
		Idx_CONFIG,		Idx_ALM_RD,		Idx_ALM_CLR,	Idx_SYNC_SET,	Idx_CONNECT,
		Idx_DISCONNECT,	Idx_PPRM_RD,	Idx_PPRM_WR,	Idx_MEM_RD,		Idx_MEM_WR,	
		Idx_POS_SET,	Idx_BRK_ON,		Idx_BRK_OFF,	Idx_SENS_ON,	Idx_SENS_OFF,
		Idx_HOLD,		Idx_MLOCK_ON,	Idx_MLOCK_OFF,	Idx_LTMOD_ON,	Idx_LTMOD_OFF,
		Idx_SMON,		Idx_SV_ON,		Idx_SV_OFF,		Idx_INTERPOLATE,Idx_POSING,	
		Idx_FEED,		Idx_LATCH,		Idx_EX_POSING,	Idx_ZRET,		Idx_VELCTRL,
		Idx_TRQCTRL,	Idx_ADJ,		Idx_SVCTRL,		
	 };

/* �T�u�R�}���h  �R�}���h�R�[�h�C���f�b�N�X */
enum {	Idx_ExNO_DEFINE,Idx_ExNOP,		Idx_ExPRM_RD,	Idx_ExPRM_WR,	Idx_ExALM_RD,
		Idx_ExPPRM_RD,	Idx_ExPPRM_WR,	Idx_ExLTMOD_ON,	Idx_ExLTMOD_OFF,Idx_ExSMON,		
	 };


/* typedefs: struct */
typedef struct CMD_DEF_TBL_ {
	ULONG timeOut;				/* �R�}���h�^�C���A�E�g����[ms] */
	UCHAR availablePhase;		/* �g�p�\�t�F�[�Y */
	UCHAR activeModeFlg;		/* �A�N�e�B�u���[�h�����҂��t���O *//*<V730>Add*/
	UCHAR exCmdTblIdx;			/* �T�u�R�}���h�Ǘ��e�[�u���ւ�Index */
	BOOL  chkCmdLock;			/* �T�u���`�F�b�N(�T�u�s��)  1 : �L��(�s��)  0 : �Ȃ�(��) */
	INT32 (*cmdFuncPtr)(void *hNetData);		/* �R�}���h��͊֐������|�C���^ */
	VOID (*rspFuncPtr)(void* hNetData, void *rsp_buf);		/* ���X�|���X�쐬�֐������|�C���^ */
} CMD_DEF_TBL;

typedef struct EX_CMD_DEF_TBL_ {
	ULONG timeOut;				/* �R�}���h�^�C���A�E�g����[ms] */
	UCHAR availablePhase;		/* �g�p�\�t�F�[�Y */
	BOOL  chkCmdLock;			/* ���C���R�}���h���`�F�b�N 1 : �L��  0 : �Ȃ� */
	INT32 (*exCmdFuncPtr)(void *hNetData);	/* �R�}���h��͊֐������|�C���^ */
	VOID (*exRspFuncPtr)(void *hNetData, void *rsp_buf); 	/* ���X�|���X�쐬�֐������|�C���^ */
} EX_CMD_DEF_TBL;

/* externs */
void CmdSetCMDTablePointer(void **IdxTblPtr, void **CmdTblPtr, void **ExCmdTblPtr);
								

#if 0/* �b�菈���Ftable�̊��t����	*/
/* variables *//* <V715> */
#ifdef	__ghs__
#pragma ghs startzda
#endif
extern CMD_DEF_TBL CmdTbl[];		/* ���C���R�}���h  �R�}���h�Ǘ��e�[�u�� */
extern EX_CMD_DEF_TBL ExCmdTbl[];	/* �T�u�R�}���h  �R�}���h�Ǘ��e�[�u�� */
extern UCHAR CmdIdx[];				/* �R�}���h��`index�e�[�u�� */
#ifdef	__ghs__
#pragma ghs endzda
#endif

/* function */
extern VOID CmdTableCopyforRam(VOID);	    /* <V715> */
#endif /* �b�菈���Ftable�̊��t����	*/

#endif /* CMD_TABLE_H_ */
