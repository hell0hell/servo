/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Control �w�b�_�t�@�C��										*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	POSING�p�����x�w�ߒǉ�							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTION_CTRL_H_
#define		MOTION_CTRL_H_

//#include "DataType.h"
//#include "ComCfg.h"
//#include "System.h"
#include "MtPRM.h"
#include "MtGenerator.h"
#include "Position.h"
#include "ServoIf.h"
//#include "AccFilter.h"
#include "PRMBank.h"


/* defines */

enum MT_OTSTATE {
	OTSTAT_NORMAL=0,	/* Disable��� */
	OTSTAT_STOP,		/* ��~����� */
	OTSTAT_WAITNORMAL,	/* OT�����҂���� */
	OTSTAT_RETRACTION,	/* �����߂������� <V679> */
};

#if defined( CCSW_SFTID ) && ( CCSW_SFTID == SFTID_EXIST )					/*<V717>Add:Start*/
enum MT_AMSTATE {
	AMSTAT_NORMAL=0,	/* Disable��� */
	AMSTAT_STOP,		/* ��~����� */
	AMSTAT_WAITNORMAL,	/* OT�����҂���� */
};
#endif												/*<V717>Add:End  */


#define MTLT_NO_SEQ_REQ	0	/* ���b�`�V�[�P���X�Ȃ���� */
#define MTLT_SEQ_REQ	1	/* ���b�`�V�[�P���X�N����� */
#define MTLT_SEQ_DONE	2	/* ���b�`�V�[�P���X������� */

#define MTLT_CMP_STS	0x11	/* ���b�`������� */


/* forward declarations */
/* Public functions */
extern INT32 MtcReqLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl);
extern VOID MtcCancelLatch(MTHNDL* hdl, UCHAR mode);

/* ����֐��}�N����` */

/*--------------------------------------------------------------------------------------------------*/
/* ���b�`���j�^�ԍ�Table <V715> 																	*/
/*--------------------------------------------------------------------------------------------------*/
extern UCHAR LtMonTbl[];


#endif /* MOTION_CTRL_H_ */
