/****************************************************************************
Description	: Trigger I/O functions
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
H.N.   2012.03.05     created

****************************************************************************/
#include "IngramGlobals.h"
#include "HwDeviceIf.h"

/* define */
#define HW_PLD_TRIGIO_DELAYTIME_ADDR_L	(HW_PLD_BASE_ADDRESS + 0x90001C)	/* OUTE �o�͒x�����Ԑݒ背�W�X�^L */
#define HW_PLD_TRIGIO_DELAYTIME_ADDR_H	(HW_PLD_BASE_ADDRESS + 0x90001E)	/* OUTF �o�͒x�����Ԑݒ背�W�X�^H */
#define HW_PLD_TRIGIO_OUTPIN_ADDR		(HW_PLD_BASE_ADDRESS + 0x900002)	/* OUT1 �o�͒[�q�ݒ背�W�X�^ */
#define HW_PLD_TRIGIO_ONTIME_ADDR		(HW_PLD_BASE_ADDRESS + 0x90000C)	/* OUT6 ON���Ԑݒ背�W�X�^ */
#define HW_PLD_TRIGIO_STS_ADDR			(HW_PLD_BASE_ADDRESS + 0x100020)	/* IN10 ��ԃ��j�^�o�̓��W�X�^ */

#define TRIG_IO_NORMAL 					0
#define TRIG_IO_MANUAL 					1
#define ENC_COM_DELAY 					131		/* �G���R�[�_�ʐM�x��[0.5us] 		 65.625us */
#define ENC_SAMPLING_TIME				125		/* �ʒu���̃T���v�����O�^�C��[0.5us] 62.5us */
#define DELAYTIME_CALC_RANGE			3		/* �g���KI/O�o��k���ԎZ�o�G���A[62.5us] 250us */

/* function prototype */

/* Trigger I/O HAL Driver */
static void hALdrv_WriteTrigIoDelayTimeL(USHORT delayTime, USHORT tbl_no);
static void hALdrv_WriteTrigIoDelayTimeH(USHORT delayTime, USHORT tbl_no);
static void hALdrv_WriteTrigIoPinSetting(USHORT pinNo, USHORT tbl_no);
static void hALdrv_WriteTrigIoOnTime(USHORT onTime, USHORT tbl_no);
static USHORT hALdrv_ReadTrigIoSts(void);

/* Trigger I/O �p�����[�^�E���W�X�^�v�Z�֐� */
PRM_RSLT pncal_trigIoTbl1_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl1_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl2_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl2_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl3_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl3_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl4_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl4_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl5_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl5_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl6_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl6_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl7_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl7_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl8_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl8_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);

PRM_RSLT fnCalcTriggerIOStsMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT fnCalcTriggerIOControl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);

/* Trigger I/O �A�v���P�[�V�������� */
static LONG calcDelayTime(TRIGIO_INFO *trigWk, LONG crtPos, SHORT bufPos);
void TrigIOSetDelayTime(AXIS_HANDLE	*Axis);
void TrigIOInitialize(AXIS_HANDLE *Axis);

/****************************************************************************/
/*																			*/
/*																			*/
/*		Trigger I/O HAL Driver ��`											*/
/*																			*/
/*																			*/
/****************************************************************************/


/****************************************************************************/
/* �T�v�F�g���KIO �o�͒x�����Ԑݒ菈��										*/
/****************************************************************************/
/* ����		:	delayTime		�x������[us]								*/
/*			:	tbl_no			�e�[�u���ԍ�								*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static void hALdrv_WriteTrigIoDelayTimeL(USHORT delayTime, USHORT tbl_no)
{
	*((USHORT*)(HW_PLD_TRIGIO_DELAYTIME_ADDR_L+tbl_no*0x100)) = delayTime;
}

static void hALdrv_WriteTrigIoDelayTimeH(USHORT delayTime, USHORT tbl_no)
{
	*((USHORT*)(HW_PLD_TRIGIO_DELAYTIME_ADDR_H+tbl_no*0x100)) = delayTime;
}

/****************************************************************************/
/* �T�v�F�g���KIO �o�͒[�q�ݒ菈��											*/
/****************************************************************************/
/* ����		:	pinNo			�s���ԍ�									*/
/*			:	tbl_no			�e�[�u���ԍ�								*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static void hALdrv_WriteTrigIoPinSetting(USHORT pinNo, USHORT tbl_no)
{
	*((VUSHORT*)(HW_PLD_TRIGIO_OUTPIN_ADDR + tbl_no*0x100)) = pinNo;
}

/****************************************************************************/
/* �T�v�F�g���KIO ON���Ԑݒ菈��											*/
/****************************************************************************/
/* ����		:	onTime			ON����[us]									*/
/*			:	tbl_no			�e�[�u���ԍ�								*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static void hALdrv_WriteTrigIoOnTime(USHORT onTime, USHORT tbl_no)
{
	*((VUSHORT*)(HW_PLD_TRIGIO_ONTIME_ADDR + tbl_no*0x100)) = onTime;
}

/****************************************************************************/
/* �T�v�F�g���KIO��ԃ��j�^�擾����											*/
/****************************************************************************/
/*			:	tbl_no			�e�[�u���ԍ�								*/
/* �߂�l	:	�g���KIO��ԃ��j�^�o��										*/
/****************************************************************************/
static USHORT hALdrv_ReadTrigIoSts(void)
{
	return *((VUSHORT*)(HW_PLD_TRIGIO_STS_ADDR));
}

/****************************************************************************/
/*																			*/
/*																			*/
/*		Trigger I/O �p�����[�^�E���W�X�^�v�Z�֐� ��`						*/
/*																			*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*		Pn642 : �g���KIO�e�[�u��1 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl1_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[0];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 0);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
		
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn643 : �g���KIO�e�[�u��1 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl1_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[0].onTime, 0);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��2 */
/****************************************************************************/
/*		Pn64A : �g���KIO�e�[�u��2 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl2_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[1];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 1);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn64B : �g���KIO�e�[�u��2 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl2_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[1].onTime, 1);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��3 */
/****************************************************************************/
/*		Pn652 : �g���KIO�e�[�u��3 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl3_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[2];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 2);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn653 : �g���KIO�e�[�u��3 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl3_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[2].onTime, 2);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��4 */
/****************************************************************************/
/*		Pn65A : �g���KIO�e�[�u��4 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl4_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[3];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 3);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn65B : �g���KIO�e�[�u��4 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl4_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[3].onTime, 3);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��5 */
/****************************************************************************/
/*		Pn662 : �g���KIO�e�[�u��5 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl5_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[4];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 4);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn663 : �g���KIO�e�[�u��5 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl5_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[4].onTime, 4);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��6 */
/****************************************************************************/
/*		Pn66A : �g���KIO�e�[�u��6 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl6_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[5];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 5);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn66B : �g���KIO�e�[�u��6 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl6_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[5].onTime, 5);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��7 */
/****************************************************************************/
/*		Pn672 : �g���KIO�e�[�u��7 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl7_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[6];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 6);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn673 : �g���KIO�e�[�u��7 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl7_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[6].onTime, 6);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* �e�[�u��8 */
/****************************************************************************/
/*		Pn67A : �g���KIO�e�[�u��8 ���ԍ��A�o�̓s���A�ʉߕ���				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl8_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[7];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 7);// pinNo �� ServoFPGA �փZ�b�g
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* �I�����ꂽ���� MotAbsPos���o�b�t�@�ɃZ�b�g */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn67B : �g���KIO�e�[�u��8 IO�o�͎���[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl8_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[7].onTime, 7);	// onTime �� ServoFPGA �փZ�b�g
	return PRM_RSLT_SUCCESS;
}

/* ���W�X�^ */
/****************************************************************************/
/*		 0x2300�F�g���KIO��ԃ��j�^���W�X�^									*/
/****************************************************************************/
PRM_RSLT fnCalcTriggerIOStsMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	switch(Cmd)
	{
	case PRM_ACCCMD_READ:
		// wk = Servo FPGA�̃f�[�^��ǂݍ���;
		*pValue = (LONG)hALdrv_ReadTrigIoSts();
		break;	
	default:
		/* do nothing */
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		 0x2301�F�g���KIO���䃌�W�X�^										*/
/****************************************************************************/
PRM_RSLT fnCalcTriggerIOControl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	USHORT	wk,wk1,wk2,tbl_no;
	BOOL	trigIOEnable_wk[8];	/* �g���KIO�L�� [0:�����A1:�L��] */
	BOOL	trigIOMode_wk[8];	/* ���[�h [0:�ʏ탂�[�h�A1:�}�j���A���o��] */
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	
	trigWk = &Axis->TrigInfo[0];
	
	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		wk = (USHORT)*pValue;	
		wk1 = wk & 0x00FF;
		wk2 = (wk & 0xFF00)>>8;

#if 0
		������ �O���[�o���ϐ����Z�b�g���Ă��܂��ƁAScanA�̃g���KIO���������s����Ă��܂��̂�NG
		/* �g���KIO�o�̓��[�h */
		trigWk->trigIOMode =  wk2 & 0x01;
		(trigWk+1)->trigIOMode = (wk2 & 0x02)>>1;
		(trigWk+2)->trigIOMode = (wk2 & 0x04)>>2;
		(trigWk+3)->trigIOMode = (wk2 & 0x08)>>3;
		(trigWk+4)->trigIOMode = (wk2 & 0x10)>>4;
		(trigWk+5)->trigIOMode = (wk2 & 0x20)>>5;
		(trigWk+6)->trigIOMode = (wk2 & 0x40)>>6;
		(trigWk+7)->trigIOMode = (wk2 & 0x80)>>7;
	
		/* �g���KIO�o�͋@�\�L�� */
		trigWk->trigIOEnable =  wk1 & 0x01;
		(trigWk+1)->trigIOEnable = (wk1 & 0x02)>>1;
		(trigWk+2)->trigIOEnable = (wk1 & 0x04)>>2;
		(trigWk+3)->trigIOEnable = (wk1 & 0x08)>>3;
		(trigWk+4)->trigIOEnable = (wk1 & 0x10)>>4;
		(trigWk+5)->trigIOEnable = (wk1 & 0x20)>>5;
		(trigWk+6)->trigIOEnable = (wk1 & 0x40)>>6;
		(trigWk+7)->trigIOEnable = (wk1 & 0x80)>>7;
#endif
		/* �g���KIO�o�̓��[�h */
		trigIOMode_wk[0] =  wk2 & 0x01;
		trigIOMode_wk[1] = (wk2 & 0x02)>>1;
		trigIOMode_wk[2] = (wk2 & 0x04)>>2;
		trigIOMode_wk[3] = (wk2 & 0x08)>>3;
		trigIOMode_wk[4] = (wk2 & 0x10)>>4;
		trigIOMode_wk[5] = (wk2 & 0x20)>>5;
		trigIOMode_wk[6] = (wk2 & 0x40)>>6;
		trigIOMode_wk[7] = (wk2 & 0x80)>>7;

		/* �g���KIO�o�͋@�\�L�� */
		trigIOEnable_wk[0] =  wk1 & 0x01;
		trigIOEnable_wk[1] = (wk1 & 0x02)>>1;
		trigIOEnable_wk[2] = (wk1 & 0x04)>>2;
		trigIOEnable_wk[3] = (wk1 & 0x08)>>3;
		trigIOEnable_wk[4] = (wk1 & 0x10)>>4;
		trigIOEnable_wk[5] = (wk1 & 0x20)>>5;
		trigIOEnable_wk[6] = (wk1 & 0x40)>>6;
		trigIOEnable_wk[7] = (wk1 & 0x80)>>7;		
		
		for( tbl_no = 0; tbl_no < 8; tbl_no++ )
		{			
//			if((trigWk+tbl_no)->trigIOEnable == TRUE) 
			if(trigIOEnable_wk[tbl_no] == TRUE) 
			{
//				if((trigWk+tbl_no)->trigIOMode == TRIG_IO_NORMAL)	
				if(trigIOMode_wk[tbl_no]  == TRIG_IO_NORMAL)	
				{/* �����l�Ƃ��� 0xFFFF ���Z�b�g */
					hALdrv_WriteTrigIoDelayTimeH(0x0000, tbl_no);
					hALdrv_WriteTrigIoDelayTimeL(0xFFFF, tbl_no);				
				}
				else
				{/* ManualDelay �� �o�͒x�����Ԑݒ背�W�X�^�ɃZ�b�g */
					wk1 = (USHORT)((trigWk+tbl_no)->manualDelay & 0x0000FFFF);
					wk2 = (USHORT)(((trigWk+tbl_no)->manualDelay & 0xFFFF0000) >> 16);				
					hALdrv_WriteTrigIoDelayTimeH(wk2, tbl_no);
					hALdrv_WriteTrigIoDelayTimeL(wk1, tbl_no);					
				}
			}
			else
			{/* �g���KIO�����̏ꍇ�� �o�͒x�����ԂƂ���0x7FFFFFFF���Z�b�g */
				hALdrv_WriteTrigIoDelayTimeH(0x7FFF, tbl_no);
				hALdrv_WriteTrigIoDelayTimeL(0xFFFF, tbl_no);
				(trigWk+tbl_no)->posReady = FALSE;	/* �|�W�V������������ */	
			}
		}
		
		/* �g���KIO�o�̓��[�h */
		trigWk->trigIOMode =  trigIOMode_wk[0];
		(trigWk+1)->trigIOMode = trigIOMode_wk[1];
		(trigWk+2)->trigIOMode = trigIOMode_wk[2];
		(trigWk+3)->trigIOMode = trigIOMode_wk[3];
		(trigWk+4)->trigIOMode = trigIOMode_wk[4];
		(trigWk+5)->trigIOMode = trigIOMode_wk[5];
		(trigWk+6)->trigIOMode = trigIOMode_wk[6];
		(trigWk+7)->trigIOMode = trigIOMode_wk[7];
	
		/* �g���KIO�o�͋@�\�L�� */
		trigWk->trigIOEnable =  trigIOEnable_wk[0];
		(trigWk+1)->trigIOEnable = trigIOEnable_wk[1];
		(trigWk+2)->trigIOEnable = trigIOEnable_wk[2];
		(trigWk+3)->trigIOEnable = trigIOEnable_wk[3];
		(trigWk+4)->trigIOEnable = trigIOEnable_wk[4];
		(trigWk+5)->trigIOEnable = trigIOEnable_wk[5];
		(trigWk+6)->trigIOEnable = trigIOEnable_wk[6];
		(trigWk+7)->trigIOEnable = trigIOEnable_wk[7];
		
		break;

	case PRM_ACCCMD_READ:
	default:
		wk1 = hALdrv_ReadTrigIoSts();
		wk  = wk1 & 0xFF;
	
		wk |= trigWk->trigIOMode << 8;
		wk |= (trigWk+1)->trigIOMode << 9;
		wk |= (trigWk+2)->trigIOMode << 10;
		wk |= (trigWk+3)->trigIOMode << 11;
		wk |= (trigWk+4)->trigIOMode << 12;
		wk |= (trigWk+5)->trigIOMode << 13;	
		wk |= (trigWk+6)->trigIOMode << 14;			
		wk |= (trigWk+7)->trigIOMode << 15;	
		
		*pValue = (LONG)wk;
		break;
	}
	return PRM_RSLT_SUCCESS;
}
// ScanA�ł̏����Ƃ̋����͑��v���H �ˁ@���Ȃ�

/****************************************************************************/
/*																			*/
/*																			*/
/*		Trigger I/O �A�v���P�[�V�������� ��`								*/
/*																			*/
/*																			*/
/****************************************************************************/

/****************************************************************************
  Trigger IO ���W���[������������ 
****************************************************************************/
void TrigIOInitialize(AXIS_HANDLE *Axis)
{
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	USHORT tbl_no;			/* �e�[�u���ԍ� */
	
	for( tbl_no = 0; tbl_no < 8; tbl_no++ )
	{
		trigWk = &Axis->TrigInfo[tbl_no];
		MlibResetByteMemory( trigWk, sizeof(TRIGIO_INFO) );
	}
}

/****************************************************************************
  Trigger IO �g���KI/O�o�͎��ԎZ�o���� 
****************************************************************************/
static LONG calcDelayTime(TRIGIO_INFO *trigWk, LONG crtPos, SHORT bufPos)
{
	LONG delayTime;			/* I/O�o�͒x�ꎞ�� */
	LONG dtrigPos,dcrtPos;	/* �g���K�ʒu�܂ł̋����A�O��l�ƍ���l�̍��� */
	
	dtrigPos = trigWk->trigPosition - crtPos;
	dcrtPos  = crtPos - trigWk->lstPos[bufPos];
	
	if(trigWk->passDir*dtrigPos >= 0)
	{/* �g���KI/O�|�W�V�����L�� */
		trigWk->posReady = TRUE;
	}
	
	if(trigWk->posReady == TRUE)
	{
	    if(crtPos == trigWk->lstPos[bufPos])
		{/* ���x�[���̏ꍇ */
			delayTime = 0x7FFFFFFF;
		}
		else if(MlibABS(dtrigPos) > 0x7FFFFFFF/(ENC_SAMPLING_TIME*(trigWk->mSpdCycle+1)))
		{/* �g���KI/O�o�͎��ԎZ�o�������I�[�o�t���[����ꍇ */
			delayTime = 0x7FFFFFFF;
//			delayTime = (0x7FFFFFFF/MlibABS(dcrtPos) - ENC_COM_DELAY)/2 -trigWk->onAdjTime;
		}
		else
		{/* �ʏ펞 */
			if(trigWk->passDir*dcrtPos>0)
			{/* �ʉߕ���=�g���K���� */
				delayTime = (trigWk->passDir*dtrigPos*ENC_SAMPLING_TIME*(trigWk->mSpdCycle+1)
					/MlibABS(dcrtPos) - ENC_COM_DELAY)/2 - trigWk->onAdjTime;
			}
			else
			{/* �ʉߕ������t�̏ꍇ */
					delayTime = 0x7FFFFFFF;	
			}
		}	
	}
	else
	{
		delayTime = 0x7FFFFFFF;
	}
	
	return delayTime;
}

/****************************************************************************
  Setting Trigger IO Delay Time for SeroFPGA 
****************************************************************************/
void TrigIOSetDelayTime(AXIS_HANDLE	*Axis)
{
	LONG delayTime = 0xFFFF;/* �g���KI/O�o�͒x������[us] */
	USHORT tbl_no;			/* �e�[�u���ԍ� */
	LONG crtPos;			/* ���݂̈ʒu */
	TRIGIO_INFO *trigWk;	/* �g���KIO�ݒ��� */
	SHORT bufPos;			/* �����O�o�b�t�@�|�W�V���� */
	
	for( tbl_no = 0; tbl_no < 8; tbl_no++ )
	{
		trigWk = &Axis->TrigInfo[tbl_no];
		crtPos = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	
		if((trigWk->trigIOMode == TRIG_IO_NORMAL) && (trigWk->trigIOEnable == TRUE))
		{
			/* �o�b�t�@���̈ʒu�v�Z */
			bufPos = trigWk->fbPosBufPtr - trigWk->mSpdCycle;
			if(bufPos < 0){bufPos = TRIG_IO_LSTPOS_MAX + bufPos;}
		
			/* delayTime�v�Z */
			delayTime = calcDelayTime(trigWk, crtPos, bufPos);
			
			/* ServoFPGA�ݒ�͈͂ɉ��H */
			if(delayTime > 0xFFFF)
			{
				delayTime = 0xFFFF;
			}
			else if(delayTime <= 0)
			{
				trigWk->trigIOEnable = FALSE;	// ��xI/O�o�͂�������I��
				delayTime = 0;
			}
			/* delayTime�̒l��ServoFPGA�ɃZ�b�g */
			hALdrv_WriteTrigIoDelayTimeL((USHORT)delayTime, tbl_no);			
		}
		
		/* �o�b�t�@�̍X�V */
		trigWk->fbPosBufPtr++;
		if(trigWk->fbPosBufPtr >= TRIG_IO_LSTPOS_MAX)
		{trigWk->fbPosBufPtr = 0;}			
		trigWk->lstPos[trigWk->fbPosBufPtr] = crtPos;
	}	
}
// �}�j���A�����[�h�̂Ƃ��̓p�����[�^�v�Z�֐��ɂă}�j���A���x�����Ԃ����W�X�^�ɃZ�b�g
// MotAbsPos �̑O��l�̓O���[�o���ɕێ����Ă��Ȃ��̂ŁATrigIo�\���̂ɕێ�����
// �G���R�[�_�x�[�X�^�C�}�l��JL-077����CPU���͓ǂݍ���ł��Ȃ��B
