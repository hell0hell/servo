/****************************************************************************
  Description   : Monitor Register Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

    Project       : Mercury

  Create Date   : 2014/04/15

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "UnMonFunc.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of 0x000 : ���[�^��]/�ړ����x */
/***************************************************************************/
const PRM_ATTR_T undef_SpdFbk = {
    0x000,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorSpeed,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001 : ���x�w�� */
/***************************************************************************/
const PRM_ATTR_T undef_SpdRef = {
    0x001,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadVelocityRef,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002 : �����g���N/���͎w�� */
/***************************************************************************/
const PRM_ATTR_T undef_TrqRef = {
    0x002,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTorqueRef,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x003 : �d�C�p1(32�r�b�g10�i�\��) */
/***************************************************************************/
const PRM_ATTR_T undef_MotAngPls = {
    0x003,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotAngle,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x005 : �d�C�p2(U��0deg����̊p�x) */
/***************************************************************************/
const PRM_ATTR_T undef_MotAngDeg = {
    0x005,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotPhase,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x006 : ���͐M�����j�^(dummy) */
/***************************************************************************/
const PRM_ATTR_T undef_InSignal = {
    0x006,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDigitalInputSignals,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x007 : �o�͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_OutSignal = {
    0x007,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDigitalOutputSignals,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x008 : ���͎w�߃p���X���x */
/***************************************************************************/
const PRM_ATTR_T undef_PosRefSpd = {
    0x008,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadCmdSpeedCounter,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x009 : �΍��J�E���^(�ʒu�΍���) */
/***************************************************************************/
static void *undefGetRamPtr_PosErrCntr(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.var.PosErrA.per32s);}

const PRM_ATTR_T undef_PosErrCntr = {
    0x009,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_PosErrCntr,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00B : �ݐϕ��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_MotLoad = {
    0x00B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadIsqrSum,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00C : �񐶕��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_RegLoad = {
    0x00C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRegenePower,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00D : �c�a��R����d�� */
/***************************************************************************/
const PRM_ATTR_T undef_DbrxPower = {
    0x00D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDBPower,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00E : ���͎w�߃p���X�J�E���^ */
/***************************************************************************/
static void *undefGetRamPtr_PcmdCounter(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->UnMonitor->UnPcmdCntr);}

const PRM_ATTR_T undef_PcmdCounter = {
    0x00E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_PcmdCounter,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x010 : ���[�^�G���R�[�_�ʒu */
/***************************************************************************/
const PRM_ATTR_T undef_PfbkCounter = {
    0x010,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorEncoderPosition,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x012 : �t���N���[�Y�h�t�B�[�h�o�b�N�p���X�J�E���^ */
/***************************************************************************/
const PRM_ATTR_T undef_FposCounter = {
    0x012,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFullcEncoderPosition,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x014 : �t���N���[�Y�h�t�B�[�h�o�b�N���x */
/***************************************************************************/
const PRM_ATTR_T undef_FcPfbkSpd = {
    0x014,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x016 : ���׉ғ����� */
/***************************************************************************/
const PRM_ATTR_T undef_PowerOnTime = {
    0x016,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTimeStamp,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x018 : ���[�^�ő呬�x��� */
/***************************************************************************/
static void *undefGetRamPtr_DivMaxSpdX(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->MencV->DivInfo.MaxDivSpd);}

const PRM_ATTR_T undef_DivMaxSpdX = {
    0x018,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_DivMaxSpdX,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x019 : �ō����x���̕�����ݒ����l */
/***************************************************************************/
static void *undefGetRamPtr_DivMaxRatX(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->MencV->DivInfo.MaxDivRat);}

const PRM_ATTR_T undef_DivMaxRatX = {
    0x019,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_DivMaxRatX,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01A : �z�[���Z���T��� */
/***************************************************************************/
const PRM_ATTR_T undef_LmPoleSensorX = {
    0x01A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadPoleSensorInfo,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01B : �t�B�[�h�o�b�N�p���X�J�E���^ */
/***************************************************************************/
const PRM_ATTR_T undef_Pfbk2Counter = {
    0x01B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadPfbkCntr,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01D : �L���Q�C�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_ActGain = {
    0x01D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadActGain,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01E : �Z�[�t�e�B���o�͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyIOSignal = {
    0x01E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSafetySignals,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01F : �Z�[�t�e�B���W���[���O�����o�͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyExternalIOSignal = {
    0x01F,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x020 : �Z�[�t�e�B���W���[�����S�@�\�ݒ�/���샂�j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySettingMon = {
    0x020,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x021 : �Z�[�t�e�B���W���[���V�X�e����ԃ��j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySysStatMon = {
    0x021,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x022 : �Z�[�t�e�B���W���[�����S���x���B����A */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySpdReachTimeA = {
    0x022,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x024 : �Z�[�t�e�B���W���[�����S���x���B����B */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySpdReachTimeB = {
    0x024,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x026 : �A�N�e�B�u���[�h�����w�ߑ��x */
/***************************************************************************/
const PRM_ATTR_T undef_ActiveModeInterCmdSpd = {
    0x026,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x027 : �Z�[�t�e�B���W���[�����[�^���x */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMotorSpd = {
    0x027,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x028 : �Z�[�t�e�B���W���[�����[�^�ʒu */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMotorPos = {
    0x028,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02A : �Z�[�t�e�B���W���[���Ď����xA */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMonitoringSpdA = {
    0x02A,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02B : �Z�[�t�e�B���W���[���Ď����xB */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMonitoringSpdB = {
    0x02B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02C : �T�[�{�p�b�N�ݒu�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AmpInstEnvMon = {
    0x02C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnAmpInstEnvMon,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02D : ���[�^�ݒu�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_MotInstEnvMon = {
    0x02D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnMotInstEnvMon,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02E : ���H�d���l */
/***************************************************************************/
const PRM_ATTR_T undef_DcVolt = {
    0x02E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDcVolt,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x030 : �o�b�N���b�V�␳�� */
/***************************************************************************/
static void *undefGetRamPtr_BlshComp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.BlshCmp.var.Filo);}

const PRM_ATTR_T undef_BlshComp = {
    0x030,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_BlshComp,               /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x032 : �o�b�N���b�V�␳�ʐݒ萧���l */
/***************************************************************************/
static void *undefGetRamPtr_BlshCompLimit(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.BlshCmp.conf.MaxCompRef);}

const PRM_ATTR_T undef_BlshCompLimit = {
    0x032,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_BlshCompLimit,          /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x034 : ����d�� */
/***************************************************************************/
static void *undefGetRamPtr_electricityConsumption(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnConsumedPower);}

const PRM_ATTR_T undef_electricityConsumption = {
    0x034,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_electricityConsumption, /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x035 : ����d�͗� */
/***************************************************************************/
static void *undefGetRamPtr_PowerConsumption(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnEnergyConsumption);}

const PRM_ATTR_T undef_PowerConsumption = {
    0x035,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_PowerConsumption,       /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x037 : �ݐϏ���d�͗ʁi�������j */
/***************************************************************************/
static void *undefGetRamPtr_TotalPowerConsumptionConst(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnCumulativeEnergyI);}

const PRM_ATTR_T undef_TotalPowerConsumptionConst = {
    0x037,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_TotalPowerConsumptionConst,/* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x039 : �ݐϏ���d�͗ʁi�������j */
/***************************************************************************/
static void *undefGetRamPtr_TotalPowerConsumptionDecimal(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnCumulativeEnergyD);}

const PRM_ATTR_T undef_TotalPowerConsumptionDecimal = {
    0x039,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_TotalPowerConsumptionDecimal,/* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x03E : �ʒu�A���v�΍� */
/***************************************************************************/
static void *undefGetRamPtr_PosErr32s(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.var.PosErr);}

const PRM_ATTR_T undef_PosErr32s = {
    0x03E,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_PosErr32s,              /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x040 : ��Βl�G���R�[�_�}���`�^�[���f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_MultiTurn = {
    0x040,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x041 : ��Βl�G���R�[�_��1��]���ʒu */
/***************************************************************************/
const PRM_ATTR_T undef_IncPulse = {
    0x041,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x043 : ��Βl�G���R�[�_�̈ʒu�f�[�^�i���ʁj */
/***************************************************************************/
const PRM_ATTR_T undef_ScaleAbsPos_l = {
    0x043,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x045 : ��Βl�G���R�[�_�̈ʒu�f�[�^�i��ʁj */
/***************************************************************************/
const PRM_ATTR_T undef_ScaleAbsPos_h = {
    0x045,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x046 : ���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
/***************************************************************************/
const PRM_ATTR_T undef_MotRatSpd = {
    0x046,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorRatedSpeed,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x047 : ���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
/***************************************************************************/
const PRM_ATTR_T undef_MotMaxSpd = {
    0x047,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorMaxSpeed,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x050 : ����FAN�������j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_BuiltInFanLifeMon = {
    0x050,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFanLifeTime,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x051 : �R���f���T�������j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CondenserLifeMon = {
    0x051,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadCapaLifeTime,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x052 : �˖h��H�������j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CollisionProofCircuitLifeMon = {
    0x052,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRushLifeTime,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x053 : DB��H�������j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_DBCircuitLifeMon = {
    0x053,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDBLifeTime,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x054 : �˖h�����[����� */
/***************************************************************************/
const PRM_ATTR_T undef_CollisionProofRelayMotionNum = {
    0x054,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRushSwitchingCount,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x056 : DB�����[����� */
/***************************************************************************/
const PRM_ATTR_T undef_DBRelayMotionNum = {
    0x056,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDBSwitchingCount,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x080 : ���Ɍ��o���������ő�ړ��� */
/***************************************************************************/
const PRM_ATTR_T undef_MpfMovePosP = {
    0x080,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMpfMaxPositiveMoveDist,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x081 : ���Ɍ��o���t�����ő�ړ��� */
/***************************************************************************/
const PRM_ATTR_T undef_MpfMovePosN = {
    0x081,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMpfMaxNegativeMoveDist,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x082 : ���Ɍ��o���v���� */
/***************************************************************************/
const PRM_ATTR_T undef_MpfFindingTime = {
    0x082,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMpfDetectingTime,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x083 : ���Ɍ��o�ʑ��I�t�Z�b�g�� */
/***************************************************************************/
const PRM_ATTR_T undef_MpfPhaseOffset = {
    0x083,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMpfPhaseOffset,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x084 : ���j�A�X�P�[���s�b�` */
/***************************************************************************/
const PRM_ATTR_T undef_LinearScalePitch = {
    0x084,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadLinearScalePitch,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x086 : ���j�A�X�P�[���s�b�`�w�� */
/***************************************************************************/
const PRM_ATTR_T undef_LinearScalePitchIndex = {
    0x086,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadLinearScalePitchExp,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x087 : ���Ɍ��o���胂�j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_MpfJudgeMonitor = {
    0x087,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMpfStatus,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x104 : �V���A���G���R�[�_�ʐM�ُ�� */
/***************************************************************************/
const PRM_ATTR_T undef_SencComErrCnt = {
    0x104,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadEncoderErrorCount,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x105 : �ʒu���ߐ��莞�� */
/***************************************************************************/
const PRM_ATTR_T undef_SettlingTime = {
    0x105,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x106 : �ʒu���߃I�[�o�V���[�g�� */
/***************************************************************************/
const PRM_ATTR_T undef_OverShoot = {
    0x106,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x107 : �c���U�����g�� */
/***************************************************************************/
const PRM_ATTR_T undef_RvibFrequency = {
    0x107,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRvibFrequency,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10A : �q�[�g�V���N���x���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_HeatsinkTemp = {
    0x10A,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10B : �ʒu���ߊ������s���ԐώZ */
/***************************************************************************/
const PRM_ATTR_T undef_CoinOffTime = {
    0x10B,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10C : ���U���o�p�����x */
/***************************************************************************/
const PRM_ATTR_T undef_VibDetSpd = {
    0x10C,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10D : �������x���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CtrlBoardTemp = {
    0x10D,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10E : �̓Z���T���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SifAdCnvData = {
    0x10E,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10F : �G���R�[�_���C�� */
/***************************************************************************/
static void *undefGetRamPtr_Temperature(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->MencV->Temperature);}

const PRM_ATTR_T undef_Temperature = {
    0x10F,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_Temperature,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x110 : �����M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX = {
    0x110,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInterSignalX,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x120 : �������͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX = {
    0x120,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalInputSignals,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x130 : �����o�͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX = {
    0x130,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalOutputSignals,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x140 : ���H����d�� */
/***************************************************************************/
const PRM_ATTR_T undef_DcVoltx = {
    0x140,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDCVoltage,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x141 : �d�����o�l */
/***************************************************************************/
const PRM_ATTR_T undef_IqRate = {
    0x141,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadIqRate,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x142 : �ݐϕ��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_MotLoad2ms = {
    0x142,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorLoad2ms,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x143 : �񐶕��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_RegerRate2ms = {
    0x143,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRegeneLoad2ms,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x144 : �c�a��R����d�� */
/***************************************************************************/
const PRM_ATTR_T undef_DbrxPower2ms = {
    0x144,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDbPower2ms,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x145 : �ő�ݐϕ��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_MotLoad10s = {
    0x145,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxMotorLoad10s,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x146 : �������[�����g��/�d�ʔ� */
/***************************************************************************/
static void *undefGetRamPtr_JLRatio(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->JatHdl.EstimatJrat);}

const PRM_ATTR_T undef_JLRatio = {
    0x146,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_JLRatio,                /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x148 : T-REF���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_TrefAdMon = {
    0x148,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTrefAdMon,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x149 : V-REF���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_VrefAdMon = {
    0x149,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadVrefAdMon,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x14A : ���͎w�߃p���X���g�� */
/***************************************************************************/
const PRM_ATTR_T undef_DPulseHwCntMon = {
    0x14A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDPulseHwCntMon,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x14C : d���d���w�߃��j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_IdrefRate = {
    0x14C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadIdRefRate,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x14D : MEMOBUS�G���[���i�G���[�����A�h���X�j */
/***************************************************************************/
const PRM_ATTR_T undef_MbusErrReg = {
    0x14D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0xffff,                                 /* Upper Limit */
    0xffff,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMemobusErrorInfo,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x150 : ���ID */
/***************************************************************************/
const PRM_ATTR_T undef_SubstrateID = {
    0x150,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x170 : ���ϓ������x */
/***************************************************************************/
const PRM_ATTR_T undef_AvgIntTmpr = {
    0x170,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAvgIntTmrp,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x171 : �������x */
/***************************************************************************/
static void *undefGetRamPtr_ControlSubstrateTemp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->OverHeat->UnMon.UnCtrlBoadTemp);}

const PRM_ATTR_T undef_ControlSubstrateTemp = {
    0x171,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_ControlSubstrateTemp,   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x172 : �p���[����x */
/***************************************************************************/
static void *undefGetRamPtr_PowerSubstrateTemp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->OverHeat->UnMon.UnPowerBoadTemp);}

const PRM_ATTR_T undef_PowerSubstrateTemp = {
    0x172,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_PowerSubstrateTemp,     /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x200 : �G���R�[�_���C�����x���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_MencTemp = {
    0x200,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotroEncoderTemperature,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x201 : ROM�`�F�b�N�T���l�i�V�X�e�����x���j */
/***************************************************************************/
const PRM_ATTR_T undef_RomChkSumVal = {
    0x201,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x203 : �ݒ�ُ�p�����[�^�ԍ�(A.040) */
/***************************************************************************/
static void *undefGetRamPtr_ErrPrmNo(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AlmManager->PrmErrInfo.ErrPrmNo);}

const PRM_ATTR_T undef_ErrPrmNo = {
    0x203,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_ErrPrmNo,               /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x204 : ���~�b�g�����p�����[�^�ԍ�(NoAlarm) */
/***************************************************************************/
const PRM_ATTR_T undef_LmtPrmNo = {
    0x204,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x205 : �T�[�{�p�b�N�u�[�gROM�`�F�b�N�T���l�i�V�X�e�����x���j */
/***************************************************************************/
const PRM_ATTR_T undef_SvPacBootRomChkSumVal = {
    0x205,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x206 : �C���^�[�t�F�[�X���W���[���u�[�gROM�`�F�b�N�T���l�i�V�X�e�����x���j */
/***************************************************************************/
const PRM_ATTR_T undef_IfBootRomChkSumVal = {
    0x206,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x210 : ���[�U�p�����[�^CRC�v�Z�l */
/***************************************************************************/
const PRM_ATTR_T undef_PrmCrcValue = {
    0x210,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadUsrPrmCrcValue,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x300 : 2ms�������CPU���ח� */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeS = {
    0x300,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadCPULoad,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x302 : 2ms�������CPU�ő啉�ח� */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeS = {
    0x302,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxCPULoad,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x304 : ScanB���ݏ������� */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeB = {
    0x304,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadScanBProcessTime,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x306 : ScanB�ő又������ */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeB = {
    0x306,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadScanBMaxProcessTime,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x308 : ScanC���ݏ������� */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeC = {
    0x308,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadScanCProcessTime,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x30A : ScanC�ő又������ */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeC = {
    0x30A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadScanCMaxProcessTime,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x310 : �ʃv���O����IntAD���ݏ������� */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeMI = {
    0x310,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMIntADProcessTime,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x312 : �ʃv���O����IntAD�ő又������ */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeMI = {
    0x312,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMIntADMaxProcessTime,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x314 : �ʃv���O����ScanA���ݏ������� */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeMA = {
    0x314,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMScanAProcessTime,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x316 : �ʃv���O����ScanA�ő又������ */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeMA = {
    0x316,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    TRUE,                                   /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMScanAMaxProcessTime,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x318 : �ʃv���O����Round���ݏ������� */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeMR = {
    0x318,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMRoundProcessTime,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x31A : �ʃv���O����Round�ő又������ */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeMR = {
    0x31A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMRoundMaxProcessTime,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x500 : �A���[�������f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTrace = {
    0x500,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    10,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlarmHistory,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50A : ���݂̃A���[����� */
/***************************************************************************/
static void *undefGetRamPtr_AlmCode(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->AlmManager)->AlmCode);}

const PRM_ATTR_T undef_AlmCode = {
    0x50A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_AlmCode,                /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50B : ���x�w�ߒ����p�f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SpdRefAdj = {
    0x50B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_VrefAdjData,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50C : �g���N�w�ߒ����p�f�[�^�i�����j */
/***************************************************************************/
const PRM_ATTR_T undef_TrqRefAdjp = {
    0x50C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_TrefAdjData,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50D : �A�i���O���j�^(CH1)�@�뒲�p�f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh1Zadj = {
    0x50D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_AmonZadjCh1,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50E : �A�i���O���j�^(CH2)�@�뒲�p�f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh2Zadj = {
    0x50E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_AmonZadjCh2,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50F : �A�i���O���j�^�iCH1�j�Q�C�����f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh1Gadj = {
    0x50F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_AmonGadjCh1,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x510 : �A�i���O���j�^�iCH2�j�Q�C�����f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh2Gadj = {
    0x510,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_AmonGadjCh2,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x511 : �d�����o�iU���j�뒲�p�f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CurUphZadj = {
    0x511,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_CurZadjUphase,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x512 : �d�����o�iV���j�뒲�p�f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CurVphZadj = {
    0x512,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &uncal_CurZadjVphase,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x513 : ���[�^�^�C�v */
/***************************************************************************/
const PRM_ATTR_T undef_MotorType = {
    0x513,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorID,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x514 : ���[�^�e�� */
/***************************************************************************/
const PRM_ATTR_T undef_MotorCapacity = {
    0x514,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorWattage,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x515 : �G���R�[�_�^�C�v */
/***************************************************************************/
const PRM_ATTR_T undef_EncoderType = {
    0x515,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadEncoderType,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x516 : Y�d�l */
/***************************************************************************/
const PRM_ATTR_T undef_Yspecification = {
    0x516,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadYspecVer,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x517 : �T�[�{�p�b�N�\�t�g�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T undef_SvPacSoftVer = {
    0x517,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFirmVerBase,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x518 : �G���R�[�_�\�t�g�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T undef_EncoderSoftVer = {
    0x518,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadEncoderVer,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x519 : �t���b�V���������݉񐔁i�V�X�e�����x���j */
/***************************************************************************/
const PRM_ATTR_T undef_FlashWriteNum = {
    0x519,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x51A : �I�v�V�����J�[�h���o��� */
/***************************************************************************/
static void *undefGetRamPtr_OpDetInfo(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->OptCardHdl->OpDetInfo);}

const PRM_ATTR_T undef_OpDetInfo = {
    0x51A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_OpDetInfo,              /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x51B : �T�[�{�X�e�[�^�X���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_SvStatus = {
    0x51B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSvStatus,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x51C : ���䃂�[�h���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CtrlMode = {
    0x51C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadServoControlMode,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x52A : �A���[�������^�C���X�^���v */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTmStamp = {
    0x52A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    10,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlarmTimeStamp,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x53E : �ُ�g���[�X�o�b�N�J�E���^0 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter0 = {
    0x53E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x53F : �ُ�g���[�X�o�b�N�J�E���^1 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter1 = {
    0x53F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x540 : �ُ�g���[�X�o�b�N�J�E���^2 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter2 = {
    0x540,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x541 : �ُ�g���[�X�o�b�N�J�E���^3 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter3 = {
    0x541,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x542 : �ُ�g���[�X�o�b�N�J�E���^4 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter4 = {
    0x542,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x543 : �ُ�g���[�X�o�b�N�J�E���^5 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter5 = {
    0x543,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x544 : �ُ�g���[�X�o�b�N�J�E���^6 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter6 = {
    0x544,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x545 : �ُ�g���[�X�o�b�N�J�E���^7 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter7 = {
    0x545,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x546 : �ُ�g���[�X�o�b�N�J�E���^8 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter8 = {
    0x546,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x547 : �ُ�g���[�X�o�b�N�J�E���^9 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter9 = {
    0x547,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x548 : �A���[���g���[�X�f�[�^�A�h���X0 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd0 = {
    0x548,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x549 : �A���[���g���[�X�f�[�^�A�h���X1 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd1 = {
    0x549,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x54A : �A���[���g���[�X�f�[�^�A�h���X2 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd2 = {
    0x54A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x54B : �A���[���g���[�X�f�[�^�A�h���X3 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd3 = {
    0x54B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x54C : �A���[���g���[�X�f�[�^�A�h���X4 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd4 = {
    0x54C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x54D : �A���[���g���[�X�f�[�^�A�h���X5 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd5 = {
    0x54D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x54E : �A���[���g���[�X�f�[�^�A�h���X6 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd6 = {
    0x54E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x54F : �A���[���g���[�X�f�[�^�A�h���X7 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd7 = {
    0x54F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x558 : �A���[���g���[�X��� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceStat = {
    0x558,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x700 : �T���v�����O���Ԃ̒P�� */
/***************************************************************************/
const PRM_ATTR_T undef_SamplingTime = {
    0x700,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSamplingTimeUnit,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x701 : �f�[�^��16384�̎��̃g���N */
/***************************************************************************/
const PRM_ATTR_T undef_MaxTorque = {
    0x701,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxTorqueConvertValue,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x702 : �f�[�^��16384�̎��̑��x */
/***************************************************************************/
const PRM_ATTR_T undef_MaxSpeed = {
    0x702,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxSpeedConvertValue,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x704 : �f�[�^��16384�̎��̑��x�̎w�� */
/***************************************************************************/
const PRM_ATTR_T undef_Dummy = {
    0x704,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxSpeedExpConvertValue,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x705 : ���������� */
/***************************************************************************/
const PRM_ATTR_T undef_NewServoOpmode = {
    0x705,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalServoMode,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x710 : DSW/RSW���͏�� */
/***************************************************************************/
const PRM_ATTR_T undef_SWInputsStatus = {
    0x710,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadHSWInputsStatus,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x712 : FB���� */
/***************************************************************************/
const PRM_ATTR_T undef_FBA_BoardID = {
    0x712,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFBboardID,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x713 : MB���� */
/***************************************************************************/
const PRM_ATTR_T undef_MB_BoardID = {
    0x713,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMBboardID,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x714 : �R���o�[�^���דd��(AD�l) */
/***************************************************************************/
const PRM_ATTR_T undef_CnvCurrentAD = {
    0x714,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadConverterCurrentAD,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x716 : �R���o�[�^�V���A�����̓f�[�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CnvSerialInputs = {
    0x716,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadConverterSerialInputs,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x718 : ���j�b�g�������x���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_MbTemperature = {
    0x718,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMbTemperature,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x719 : �R���o�[�^���x���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_CnvTemperature = {
    0x719,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadCnvTemperature,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x71A : �������x���j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_IfTemperature = {
    0x71A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadIfTemperature,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x720 : (R)�u�[�g�\�t�g�E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T undef_R_BootsoftVersion = {
    0x720,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    2,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadBootsoftVersion,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x724 : (R)�u�[�g�X�g���b�v�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T undef_R_BootstrapVersion = {
    0x724,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    2,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadBootstrapVersion,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x800 : ���[�^��]/�ړ����x */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSpdFbkX = {
    0x800,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtSpdFbk,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x801 : ���x�w�� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSpdRefX = {
    0x801,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtSpdRef,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x802 : �����g���N/���͎w�� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtTrqRefX = {
    0x802,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtTrqRef,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x803 : ���͎w�߃p���X���x */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtPosRefSpdX = {
    0x803,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtPosRefSpd,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x804 : �΍��J�E���^(�ʒu�΍���) */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtPosErrCntrX = {
    0x804,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtPosErrCntr,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x810 : �����M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtInterSignalX = {
    0x810,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMonInterSig,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x814 : �������͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtIntInSignalX = {
    0x814,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMonInSig,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x818 : �����o�͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtIntOutSignalX = {
    0x818,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMonOutSig,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x820 : ���H����d�� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtDcVoltxX = {
    0x820,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtDcVoltx,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x821 : �d�����o�l */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtIqRateX = {
    0x821,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtIqRate,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x822 : �ݐϕ��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtMotLoad2msX = {
    0x822,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMotLoad2ms,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x823 : �񐶕��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtRegerRate2msX = {
    0x823,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtRegerRate2ms,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x824 : �c�a��R����d�� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtDbrxPower2msX = {
    0x824,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtDbrxPower2ms,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x825 : �ő�ݐϕ��ח� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtMotLoad10sX = {
    0x825,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMotLoad10s,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x826 : �������[�����g��/�d�ʔ� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtJLRatioX = {
    0x826,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtJLRatio,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x830 : �V���A���G���R�[�_�ʐM�ُ�� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSencComErrCntX = {
    0x830,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtSencComErrCnt,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x831 : MECHATROLINK�ʐM�ُ�� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtM2ComErrCntX = {
    0x831,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtM2ComErrCnt,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x832 : �Z�[�t�e�B���o�͐M�����j�^ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSafetyIOSignalX = {
    0x832,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMonSafetyIOSig,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x833 : �A���[���ԍ� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtAlmCodeX = {
    0x833,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtAlmCode,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x834 : �^�C���X�^���v */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtTmStampX = {
    0x834,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtTmStamp,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x900 : �g���[�X�A���[���ԍ� */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTrcAlmCode = {
    0x900,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x901 : �g���[�X�A���[���^�C���X�^���v */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTrcTmStamp = {
    0x901,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC00 : �t�B�[�h�o�b�N�ʒu(APOS) */
/***************************************************************************/
const PRM_ATTR_T undef_Apos = {
    0xC00,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadApos,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC02 : ���ݎw�߈ʒu(CPOS) */
/***************************************************************************/
const PRM_ATTR_T undef_Cpos = {
    0xC02,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadCpos,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC04 : �ʒu�΍�(PERR) */
/***************************************************************************/
const PRM_ATTR_T undef_Perr = {
    0xC04,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadPerr,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC06 : �ڕW�ʒu(TPOS) */
/***************************************************************************/
const PRM_ATTR_T undef_Tpos = {
    0xC06,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTpos,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC08 : ���b�`�ʒu�P(LPOS1) */
/***************************************************************************/
const PRM_ATTR_T undef_Lpos1 = {
    0xC08,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadLpos1,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC0A : ���b�`�ʒu�Q(LPOS2) */
/***************************************************************************/
const PRM_ATTR_T undef_Lpos2 = {
    0xC0A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadLpos2,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC0C : ���b�`�ʒu�R(LPOS3) */
/***************************************************************************/
const PRM_ATTR_T undef_Lpos3 = {
    0xC0C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadLpos3,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC0E : �ڕW���x(TSPD) */
/***************************************************************************/
const PRM_ATTR_T undef_Tspd = {
    0xC0E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTspd,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC10 : �t�B�[�h�o�b�N���x(FSPD) */
/***************************************************************************/
const PRM_ATTR_T undef_Fspd = {
    0xC10,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFbspd,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC12 : ���݈ʒu�w�ߑ��x(CSPD) */
/***************************************************************************/
const PRM_ATTR_T undef_Cspd = {
    0xC12,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadCspd,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC14 : �g���N�����l(TRQ_LIM) */
/***************************************************************************/
const PRM_ATTR_T undef_TrqLim = {
    0xC14,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTrqLim,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC16 : ���x�����l(SPD_LIM) */
/***************************************************************************/
const PRM_ATTR_T undef_SpdLim = {
    0xC16,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSpdLim,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC18 : M-�V�ʐM�t�F�[�Y */
/***************************************************************************/
const PRM_ATTR_T undef_M3ComPhase = {
    0xC18,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadM3CommPhase,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC19 : M-�V���ݎ��s�R�}���h�R�[�h(RCMD) */
/***************************************************************************/
const PRM_ATTR_T undef_M3Rcmd = {
    0xC19,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadM3MainCommandCode,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC1A : M-�V���ݎ��s�T�u�R�}���h�R�[�h(SUB_RCMD) */
/***************************************************************************/
const PRM_ATTR_T undef_M3SubRcmd = {
    0xC1A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadM3SubCommandCode,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF000 : �S���͐M�����j�^1(for LCD Operator) */
/***************************************************************************/
const PRM_ATTR_T undef_AllInSignal1 = {
    0xF000,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDigitalAllInputSignals1,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF001 : �S���͐M�����j�^2(for LCD Operator) */
/***************************************************************************/
const PRM_ATTR_T undef_AllInSignal2 = {
    0xF001,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDigitalAllInputSignals2,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF002 : �S�o�͐M�����j�^(for LCD Operator) */
/***************************************************************************/
const PRM_ATTR_T undef_AllOutSignal = {
    0xF002,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDigitalAllOutputSignals,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF003 : �����M�����j�^1 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX1 = {
    0xF003,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInterSignalX1,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF004 : �����M�����j�^2 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX2 = {
    0xF004,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInterSignalX2,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF005 : �����M�����j�^3 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX3 = {
    0xF005,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInterSignalX3,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF006 : �����M�����j�^4 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX4 = {
    0xF006,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInterSignalX4,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF007 : �������͐M�����j�^ 1 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX1 = {
    0xF007,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalInputSignals1,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF008 : �������͐M�����j�^ 2 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX2 = {
    0xF008,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalInputSignals2,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF009 : �������͐M�����j�^ 3 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX3 = {
    0xF009,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalInputSignals3,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF00A : �������͐M�����j�^ 4 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX4 = {
    0xF00A,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalInputSignals4,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF00B : �����o�͐M�����j�^1 */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX1 = {
    0xF00B,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalOutputSignals1,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF00C : �����o�͐M�����j�^2 */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX2 = {
    0xF00C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalOutputSignals2,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF00D : �����o�͐M�����j�^3 */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX3 = {
    0xF00D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEBIT,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadInternalOutputSignals3,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF00E : ���[�^�ő呬�x���/������ݒ����l */
/***************************************************************************/
const PRM_ATTR_T undef_SelSpdOrDiv = {
    0xF00E,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* ������ */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnMaxSpdOrMaxDiv,                      /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL UnRegTable[] = {
    {0x000   , &undef_SpdFbk                        },/* 0x000: ���[�^��]/�ړ����x */
    {0x001   , &undef_SpdRef                        },/* 0x001: ���x�w�� */
    {0x002   , &undef_TrqRef                        },/* 0x002: �����g���N/���͎w�� */
    {0x003   , &undef_MotAngPls                     },/* 0x003: �d�C�p1(32�r�b�g10�i�\��) */
    {0x005   , &undef_MotAngDeg                     },/* 0x005: �d�C�p2(U��0deg����̊p�x) */
    {0x006   , &undef_InSignal                      },/* 0x006: ���͐M�����j�^(dummy) */
    {0x007   , &undef_OutSignal                     },/* 0x007: �o�͐M�����j�^ */
    {0x008   , &undef_PosRefSpd                     },/* 0x008: ���͎w�߃p���X���x */
    {0x009   , &undef_PosErrCntr                    },/* 0x009: �΍��J�E���^(�ʒu�΍���) */
    {0x00B   , &undef_MotLoad                       },/* 0x00B: �ݐϕ��ח� */
    {0x00C   , &undef_RegLoad                       },/* 0x00C: �񐶕��ח� */
    {0x00D   , &undef_DbrxPower                     },/* 0x00D: �c�a��R����d�� */
    {0x00E   , &undef_PcmdCounter                   },/* 0x00E: ���͎w�߃p���X�J�E���^ */
    {0x010   , &undef_PfbkCounter                   },/* 0x010: ���[�^�G���R�[�_�ʒu */
    {0x012   , &undef_FposCounter                   },/* 0x012: �t���N���[�Y�h�t�B�[�h�o�b�N�p���X�J�E���^ */
    {0x014   , &undef_FcPfbkSpd                     },/* 0x014: �t���N���[�Y�h�t�B�[�h�o�b�N���x */
    {0x016   , &undef_PowerOnTime                   },/* 0x016: ���׉ғ����� */
    {0x018   , &undef_DivMaxSpdX                    },/* 0x018: ���[�^�ő呬�x��� */
    {0x019   , &undef_DivMaxRatX                    },/* 0x019: �ō����x���̕�����ݒ����l */
    {0x01A   , &undef_LmPoleSensorX                 },/* 0x01A: �z�[���Z���T��� */
    {0x01B   , &undef_Pfbk2Counter                  },/* 0x01B: �t�B�[�h�o�b�N�p���X�J�E���^ */
    {0x01D   , &undef_ActGain                       },/* 0x01D: �L���Q�C�����j�^ */
    {0x01E   , &undef_SafetyIOSignal                },/* 0x01E: �Z�[�t�e�B���o�͐M�����j�^ */
    {0x01F   , &undef_SafetyExternalIOSignal        },/* 0x01F: �Z�[�t�e�B���W���[���O�����o�͐M�����j�^ */
    {0x020   , &undef_SafetySettingMon              },/* 0x020: �Z�[�t�e�B���W���[�����S�@�\�ݒ�/���샂�j�^ */
    {0x021   , &undef_SafetySysStatMon              },/* 0x021: �Z�[�t�e�B���W���[���V�X�e����ԃ��j�^ */
    {0x022   , &undef_SafetySpdReachTimeA           },/* 0x022: �Z�[�t�e�B���W���[�����S���x���B����A */
    {0x024   , &undef_SafetySpdReachTimeB           },/* 0x024: �Z�[�t�e�B���W���[�����S���x���B����B */
    {0x026   , &undef_ActiveModeInterCmdSpd         },/* 0x026: �A�N�e�B�u���[�h�����w�ߑ��x */
    {0x027   , &undef_SafetyMotorSpd                },/* 0x027: �Z�[�t�e�B���W���[�����[�^���x */
    {0x028   , &undef_SafetyMotorPos                },/* 0x028: �Z�[�t�e�B���W���[�����[�^�ʒu */
    {0x02A   , &undef_SafetyMonitoringSpdA          },/* 0x02A: �Z�[�t�e�B���W���[���Ď����xA */
    {0x02B   , &undef_SafetyMonitoringSpdB          },/* 0x02B: �Z�[�t�e�B���W���[���Ď����xB */
    {0x02C   , &undef_AmpInstEnvMon                 },/* 0x02C: �T�[�{�p�b�N�ݒu�����j�^ */
    {0x02D   , &undef_MotInstEnvMon                 },/* 0x02D: ���[�^�ݒu�����j�^ */
    {0x02E   , &undef_DcVolt                        },/* 0x02E: ���H�d���l */
    {0x030   , &undef_BlshComp                      },/* 0x030: �o�b�N���b�V�␳�� */
    {0x032   , &undef_BlshCompLimit                 },/* 0x032: �o�b�N���b�V�␳�ʐݒ萧���l */
    {0x034   , &undef_electricityConsumption        },/* 0x034: ����d�� */
    {0x035   , &undef_PowerConsumption              },/* 0x035: ����d�͗� */
    {0x037   , &undef_TotalPowerConsumptionConst    },/* 0x037: �ݐϏ���d�͗ʁi�������j */
    {0x039   , &undef_TotalPowerConsumptionDecimal  },/* 0x039: �ݐϏ���d�͗ʁi�������j */
    {0x03E   , &undef_PosErr32s                     },/* 0x03E: �ʒu�A���v�΍� */
    {0x040   , &undef_MultiTurn                     },/* 0x040: ��Βl�G���R�[�_�}���`�^�[���f�[�^ */
    {0x041   , &undef_IncPulse                      },/* 0x041: ��Βl�G���R�[�_��1��]���ʒu */
    {0x043   , &undef_ScaleAbsPos_l                 },/* 0x043: ��Βl�G���R�[�_�̈ʒu�f�[�^�i���ʁj */
    {0x045   , &undef_ScaleAbsPos_h                 },/* 0x045: ��Βl�G���R�[�_�̈ʒu�f�[�^�i��ʁj */
    {0x046   , &undef_MotRatSpd                     },/* 0x046: ���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x047   , &undef_MotMaxSpd                     },/* 0x047: ���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x050   , &undef_BuiltInFanLifeMon             },/* 0x050: ����FAN�������j�^ */
    {0x051   , &undef_CondenserLifeMon              },/* 0x051: �R���f���T�������j�^ */
    {0x052   , &undef_CollisionProofCircuitLifeMon  },/* 0x052: �˖h��H�������j�^ */
    {0x053   , &undef_DBCircuitLifeMon              },/* 0x053: DB��H�������j�^ */
    {0x054   , &undef_CollisionProofRelayMotionNum  },/* 0x054: �˖h�����[����� */
    {0x056   , &undef_DBRelayMotionNum              },/* 0x056: DB�����[����� */
    {0x080   , &undef_MpfMovePosP                   },/* 0x080: ���Ɍ��o���������ő�ړ��� */
    {0x081   , &undef_MpfMovePosN                   },/* 0x081: ���Ɍ��o���t�����ő�ړ��� */
    {0x082   , &undef_MpfFindingTime                },/* 0x082: ���Ɍ��o���v���� */
    {0x083   , &undef_MpfPhaseOffset                },/* 0x083: ���Ɍ��o�ʑ��I�t�Z�b�g�� */
    {0x084   , &undef_LinearScalePitch              },/* 0x084: ���j�A�X�P�[���s�b�` */
    {0x086   , &undef_LinearScalePitchIndex         },/* 0x086: ���j�A�X�P�[���s�b�`�w�� */
    {0x087   , &undef_MpfJudgeMonitor               },/* 0x087: ���Ɍ��o���胂�j�^ */
    {0x104   , &undef_SencComErrCnt                 },/* 0x104: �V���A���G���R�[�_�ʐM�ُ�� */
    {0x105   , &undef_SettlingTime                  },/* 0x105: �ʒu���ߐ��莞�� */
    {0x106   , &undef_OverShoot                     },/* 0x106: �ʒu���߃I�[�o�V���[�g�� */
    {0x107   , &undef_RvibFrequency                 },/* 0x107: �c���U�����g�� */
    {0x10A   , &undef_HeatsinkTemp                  },/* 0x10A: �q�[�g�V���N���x���j�^ */
    {0x10B   , &undef_CoinOffTime                   },/* 0x10B: �ʒu���ߊ������s���ԐώZ */
    {0x10C   , &undef_VibDetSpd                     },/* 0x10C: ���U���o�p�����x */
    {0x10D   , &undef_CtrlBoardTemp                 },/* 0x10D: �������x���j�^ */
    {0x10E   , &undef_SifAdCnvData                  },/* 0x10E: �̓Z���T���j�^ */
    {0x10F   , &undef_Temperature                   },/* 0x10F: �G���R�[�_���C�� */
    {0x110   , &undef_InterSignalX                  },/* 0x110: �����M�����j�^ */
    {0x120   , &undef_IntInSignalX                  },/* 0x120: �������͐M�����j�^ */
    {0x130   , &undef_IntOutSignalX                 },/* 0x130: �����o�͐M�����j�^ */
    {0x140   , &undef_DcVoltx                       },/* 0x140: ���H����d�� */
    {0x141   , &undef_IqRate                        },/* 0x141: �d�����o�l */
    {0x142   , &undef_MotLoad2ms                    },/* 0x142: �ݐϕ��ח� */
    {0x143   , &undef_RegerRate2ms                  },/* 0x143: �񐶕��ח� */
    {0x144   , &undef_DbrxPower2ms                  },/* 0x144: �c�a��R����d�� */
    {0x145   , &undef_MotLoad10s                    },/* 0x145: �ő�ݐϕ��ח� */
    {0x146   , &undef_JLRatio                       },/* 0x146: �������[�����g��/�d�ʔ� */
    {0x148   , &undef_TrefAdMon                     },/* 0x148: T-REF���j�^ */
    {0x149   , &undef_VrefAdMon                     },/* 0x149: V-REF���j�^ */
    {0x14A   , &undef_DPulseHwCntMon                },/* 0x14A: ���͎w�߃p���X���g�� */
    {0x14C   , &undef_IdrefRate                     },/* 0x14C: d���d���w�߃��j�^ */
    {0x14D   , &undef_MbusErrReg                    },/* 0x14D: MEMOBUS�G���[���i�G���[�����A�h���X�j */
    {0x150   , &undef_SubstrateID                   },/* 0x150: ���ID */
    {0x170   , &undef_AvgIntTmpr                    },/* 0x170: ���ϓ������x */
    {0x171   , &undef_ControlSubstrateTemp          },/* 0x171: �������x */
    {0x172   , &undef_PowerSubstrateTemp            },/* 0x172: �p���[����x */
    {0x200   , &undef_MencTemp                      },/* 0x200: �G���R�[�_���C�����x���j�^ */
    {0x201   , &undef_RomChkSumVal                  },/* 0x201: ROM�`�F�b�N�T���l�i�V�X�e�����x���j */
    {0x202   , &undef_MbusErrReg                    },/* 0x202: MEMOBUS�G���[���i�G���[�����A�h���X�j */
    {0x203   , &undef_ErrPrmNo                      },/* 0x203: �ݒ�ُ�p�����[�^�ԍ�(A.040) */
    {0x204   , &undef_LmtPrmNo                      },/* 0x204: ���~�b�g�����p�����[�^�ԍ�(NoAlarm) */
    {0x205   , &undef_SvPacBootRomChkSumVal         },/* 0x205: �T�[�{�p�b�N�u�[�gROM�`�F�b�N�T���l�i�V�X�e�����x���j */
    {0x206   , &undef_IfBootRomChkSumVal            },/* 0x206: �C���^�[�t�F�[�X���W���[���u�[�gROM�`�F�b�N�T���l�i�V�X�e�����x���j */
    {0x210   , &undef_PrmCrcValue                   },/* 0x210: ���[�U�p�����[�^CRC�v�Z�l */
    {0x300   , &undef_CurExeTimeS                   },/* 0x300: 2ms�������CPU���ח� */
    {0x302   , &undef_MaxExeTimeS                   },/* 0x302: 2ms�������CPU�ő啉�ח� */
    {0x304   , &undef_CurExeTimeB                   },/* 0x304: ScanB���ݏ������� */
    {0x306   , &undef_MaxExeTimeB                   },/* 0x306: ScanB�ő又������ */
    {0x308   , &undef_CurExeTimeC                   },/* 0x308: ScanC���ݏ������� */
    {0x30A   , &undef_MaxExeTimeC                   },/* 0x30A: ScanC�ő又������ */
    {0x310   , &undef_CurExeTimeMI                  },/* 0x310: �ʃv���O����IntAD���ݏ������� */
    {0x312   , &undef_MaxExeTimeMI                  },/* 0x312: �ʃv���O����IntAD�ő又������ */
    {0x314   , &undef_CurExeTimeMA                  },/* 0x314: �ʃv���O����ScanA���ݏ������� */
    {0x316   , &undef_MaxExeTimeMA                  },/* 0x316: �ʃv���O����ScanA�ő又������ */
    {0x318   , &undef_CurExeTimeMR                  },/* 0x318: �ʃv���O����Round���ݏ������� */
    {0x31A   , &undef_MaxExeTimeMR                  },/* 0x31A: �ʃv���O����Round�ő又������ */
    {0x500   , &undef_AlmTrace                      },/* 0x500: �A���[�������f�[�^ */
    {0x50A   , &undef_AlmCode                       },/* 0x50A: ���݂̃A���[����� */
    {0x50B   , &undef_SpdRefAdj                     },/* 0x50B: ���x�w�ߒ����p�f�[�^ */
    {0x50C   , &undef_TrqRefAdjp                    },/* 0x50C: �g���N�w�ߒ����p�f�[�^�i�����j */
    {0x50D   , &undef_AmonCh1Zadj                   },/* 0x50D: �A�i���O���j�^(CH1)�@�뒲�p�f�[�^ */
    {0x50E   , &undef_AmonCh2Zadj                   },/* 0x50E: �A�i���O���j�^(CH2)�@�뒲�p�f�[�^ */
    {0x50F   , &undef_AmonCh1Gadj                   },/* 0x50F: �A�i���O���j�^�iCH1�j�Q�C�����f�[�^ */
    {0x510   , &undef_AmonCh2Gadj                   },/* 0x510: �A�i���O���j�^�iCH2�j�Q�C�����f�[�^ */
    {0x511   , &undef_CurUphZadj                    },/* 0x511: �d�����o�iU���j�뒲�p�f�[�^ */
    {0x512   , &undef_CurVphZadj                    },/* 0x512: �d�����o�iV���j�뒲�p�f�[�^ */
    {0x513   , &undef_MotorType                     },/* 0x513: ���[�^�^�C�v */
    {0x514   , &undef_MotorCapacity                 },/* 0x514: ���[�^�e�� */
    {0x515   , &undef_EncoderType                   },/* 0x515: �G���R�[�_�^�C�v */
    {0x516   , &undef_Yspecification                },/* 0x516: Y�d�l */
    {0x517   , &undef_SvPacSoftVer                  },/* 0x517: �T�[�{�p�b�N�\�t�g�o�[�W���� */
    {0x518   , &undef_EncoderSoftVer                },/* 0x518: �G���R�[�_�\�t�g�o�[�W���� */
    {0x519   , &undef_FlashWriteNum                 },/* 0x519: �t���b�V���������݉񐔁i�V�X�e�����x���j */
    {0x51A   , &undef_OpDetInfo                     },/* 0x51A: �I�v�V�����J�[�h���o��� */
    {0x51B   , &undef_SvStatus                      },/* 0x51B: �T�[�{�X�e�[�^�X���j�^ */
    {0x51C   , &undef_CtrlMode                      },/* 0x51C: ���䃂�[�h���j�^ */
    {0x52A   , &undef_AlmTmStamp                    },/* 0x52A: �A���[�������^�C���X�^���v */
    {0x53E   , &undef_AbnormalityTraceBackCounter0  },/* 0x53E: �ُ�g���[�X�o�b�N�J�E���^0 */
    {0x53F   , &undef_AbnormalityTraceBackCounter1  },/* 0x53F: �ُ�g���[�X�o�b�N�J�E���^1 */
    {0x540   , &undef_AbnormalityTraceBackCounter2  },/* 0x540: �ُ�g���[�X�o�b�N�J�E���^2 */
    {0x541   , &undef_AbnormalityTraceBackCounter3  },/* 0x541: �ُ�g���[�X�o�b�N�J�E���^3 */
    {0x542   , &undef_AbnormalityTraceBackCounter4  },/* 0x542: �ُ�g���[�X�o�b�N�J�E���^4 */
    {0x543   , &undef_AbnormalityTraceBackCounter5  },/* 0x543: �ُ�g���[�X�o�b�N�J�E���^5 */
    {0x544   , &undef_AbnormalityTraceBackCounter6  },/* 0x544: �ُ�g���[�X�o�b�N�J�E���^6 */
    {0x545   , &undef_AbnormalityTraceBackCounter7  },/* 0x545: �ُ�g���[�X�o�b�N�J�E���^7 */
    {0x546   , &undef_AbnormalityTraceBackCounter8  },/* 0x546: �ُ�g���[�X�o�b�N�J�E���^8 */
    {0x547   , &undef_AbnormalityTraceBackCounter9  },/* 0x547: �ُ�g���[�X�o�b�N�J�E���^9 */
    {0x548   , &undef_AlmTraceDataAdd0              },/* 0x548: �A���[���g���[�X�f�[�^�A�h���X0 */
    {0x549   , &undef_AlmTraceDataAdd1              },/* 0x549: �A���[���g���[�X�f�[�^�A�h���X1 */
    {0x54A   , &undef_AlmTraceDataAdd2              },/* 0x54A: �A���[���g���[�X�f�[�^�A�h���X2 */
    {0x54B   , &undef_AlmTraceDataAdd3              },/* 0x54B: �A���[���g���[�X�f�[�^�A�h���X3 */
    {0x54C   , &undef_AlmTraceDataAdd4              },/* 0x54C: �A���[���g���[�X�f�[�^�A�h���X4 */
    {0x54D   , &undef_AlmTraceDataAdd5              },/* 0x54D: �A���[���g���[�X�f�[�^�A�h���X5 */
    {0x54E   , &undef_AlmTraceDataAdd6              },/* 0x54E: �A���[���g���[�X�f�[�^�A�h���X6 */
    {0x54F   , &undef_AlmTraceDataAdd7              },/* 0x54F: �A���[���g���[�X�f�[�^�A�h���X7 */
    {0x558   , &undef_AlmTraceStat                  },/* 0x558: �A���[���g���[�X��� */
    {0x600   , &undef_MultiTurn                     },/* 0x600: ��Βl�G���R�[�_����]�� */
    {0x601   , &undef_IncPulse                      },/* 0x601: ��Βl�G���R�[�_1��]���p���X */
    {0x603   , &undef_ScaleAbsPos_l                 },/* 0x603: ��Βl�X�P�[���ʒu�i���ʁj */
    {0x605   , &undef_ScaleAbsPos_h                 },/* 0x605: ��Βl�X�P�[���ʒu�i��ʁj */
    {0x700   , &undef_SamplingTime                  },/* 0x700: �T���v�����O���Ԃ̒P�� */
    {0x701   , &undef_MaxTorque                     },/* 0x701: �f�[�^��16384�̎��̃g���N */
    {0x702   , &undef_MaxSpeed                      },/* 0x702: �f�[�^��16384�̎��̑��x */
    {0x704   , &undef_Dummy                         },/* 0x704: �f�[�^��16384�̎��̑��x�̎w�� */
    {0x705   , &undef_NewServoOpmode                },/* 0x705: ���������� */
    {0x710   , &undef_SWInputsStatus                },/* 0x710: DSW/RSW���͏�� */
    {0x712   , &undef_FBA_BoardID                   },/* 0x712: FB���� */
    {0x713   , &undef_MB_BoardID                    },/* 0x713: MB���� */
    {0x714   , &undef_CnvCurrentAD                  },/* 0x714: �R���o�[�^���דd��(AD�l) */
    {0x716   , &undef_CnvSerialInputs               },/* 0x716: �R���o�[�^�V���A�����̓f�[�^ */
    {0x718   , &undef_MbTemperature                 },/* 0x718: ���j�b�g�������x���j�^ */
    {0x719   , &undef_CnvTemperature                },/* 0x719: �R���o�[�^���x���j�^ */
    {0x71A   , &undef_IfTemperature                 },/* 0x71A: �������x���j�^ */
    {0x720   , &undef_R_BootsoftVersion             },/* 0x720: (R)�u�[�g�\�t�g�E�F�A�o�[�W���� */
    {0x724   , &undef_R_BootstrapVersion            },/* 0x724: (R)�u�[�g�X�g���b�v�o�[�W���� */
    {0x800   , &undef_AlmLtSpdFbkX                  },/* 0x800: ���[�^��]/�ړ����x */
    {0x801   , &undef_AlmLtSpdRefX                  },/* 0x801: ���x�w�� */
    {0x802   , &undef_AlmLtTrqRefX                  },/* 0x802: �����g���N/���͎w�� */
    {0x803   , &undef_AlmLtPosRefSpdX               },/* 0x803: ���͎w�߃p���X���x */
    {0x804   , &undef_AlmLtPosErrCntrX              },/* 0x804: �΍��J�E���^(�ʒu�΍���) */
    {0x810   , &undef_AlmLtInterSignalX             },/* 0x810: �����M�����j�^ */
    {0x814   , &undef_AlmLtIntInSignalX             },/* 0x814: �������͐M�����j�^ */
    {0x818   , &undef_AlmLtIntOutSignalX            },/* 0x818: �����o�͐M�����j�^ */
    {0x820   , &undef_AlmLtDcVoltxX                 },/* 0x820: ���H����d�� */
    {0x821   , &undef_AlmLtIqRateX                  },/* 0x821: �d�����o�l */
    {0x822   , &undef_AlmLtMotLoad2msX              },/* 0x822: �ݐϕ��ח� */
    {0x823   , &undef_AlmLtRegerRate2msX            },/* 0x823: �񐶕��ח� */
    {0x824   , &undef_AlmLtDbrxPower2msX            },/* 0x824: �c�a��R����d�� */
    {0x825   , &undef_AlmLtMotLoad10sX              },/* 0x825: �ő�ݐϕ��ח� */
    {0x826   , &undef_AlmLtJLRatioX                 },/* 0x826: �������[�����g��/�d�ʔ� */
    {0x830   , &undef_AlmLtSencComErrCntX           },/* 0x830: �V���A���G���R�[�_�ʐM�ُ�� */
    {0x831   , &undef_AlmLtM2ComErrCntX             },/* 0x831: MECHATROLINK�ʐM�ُ�� */
    {0x832   , &undef_AlmLtSafetyIOSignalX          },/* 0x832: �Z�[�t�e�B���o�͐M�����j�^ */
    {0x833   , &undef_AlmLtAlmCodeX                 },/* 0x833: �A���[���ԍ� */
    {0x834   , &undef_AlmLtTmStampX                 },/* 0x834: �^�C���X�^���v */
    {0x900   , &undef_AlmTrcAlmCode                 },/* 0x900: �g���[�X�A���[���ԍ� */
    {0x901   , &undef_AlmTrcTmStamp                 },/* 0x901: �g���[�X�A���[���^�C���X�^���v */
    {0xC00   , &undef_Apos                          },/* 0xC00: �t�B�[�h�o�b�N�ʒu(APOS) */
    {0xC02   , &undef_Cpos                          },/* 0xC02: ���ݎw�߈ʒu(CPOS) */
    {0xC04   , &undef_Perr                          },/* 0xC04: �ʒu�΍�(PERR) */
    {0xC06   , &undef_Tpos                          },/* 0xC06: �ڕW�ʒu(TPOS) */
    {0xC08   , &undef_Lpos1                         },/* 0xC08: ���b�`�ʒu�P(LPOS1) */
    {0xC0A   , &undef_Lpos2                         },/* 0xC0A: ���b�`�ʒu�Q(LPOS2) */
    {0xC0C   , &undef_Lpos3                         },/* 0xC0C: ���b�`�ʒu�R(LPOS3) */
    {0xC0E   , &undef_Tspd                          },/* 0xC0E: �ڕW���x(TSPD) */
    {0xC10   , &undef_Fspd                          },/* 0xC10: �t�B�[�h�o�b�N���x(FSPD) */
    {0xC12   , &undef_Cspd                          },/* 0xC12: ���݈ʒu�w�ߑ��x(CSPD) */
    {0xC14   , &undef_TrqLim                        },/* 0xC14: �g���N�����l(TRQ_LIM) */
    {0xC16   , &undef_SpdLim                        },/* 0xC16: ���x�����l(SPD_LIM) */
    {0xC18   , &undef_M3ComPhase                    },/* 0xC18: M-�V�ʐM�t�F�[�Y */
    {0xC19   , &undef_M3Rcmd                        },/* 0xC19: M-�V���ݎ��s�R�}���h�R�[�h(RCMD) */
    {0xC1A   , &undef_M3SubRcmd                     },/* 0xC1A: M-�V���ݎ��s�T�u�R�}���h�R�[�h(SUB_RCMD) */
    {0xF000  , &undef_AllInSignal1                  },/* 0xF000: �S���͐M�����j�^1(for LCD Operator) */
    {0xF001  , &undef_AllInSignal2                  },/* 0xF001: �S���͐M�����j�^2(for LCD Operator) */
    {0xF002  , &undef_AllOutSignal                  },/* 0xF002: �S�o�͐M�����j�^(for LCD Operator) */
    {0xF003  , &undef_InterSignalX1                 },/* 0xF003: �����M�����j�^1 */
    {0xF004  , &undef_InterSignalX2                 },/* 0xF004: �����M�����j�^2 */
    {0xF005  , &undef_InterSignalX3                 },/* 0xF005: �����M�����j�^3 */
    {0xF006  , &undef_InterSignalX4                 },/* 0xF006: �����M�����j�^4 */
    {0xF007  , &undef_IntInSignalX1                 },/* 0xF007: �������͐M�����j�^ 1 */
    {0xF008  , &undef_IntInSignalX2                 },/* 0xF008: �������͐M�����j�^ 2 */
    {0xF009  , &undef_IntInSignalX3                 },/* 0xF009: �������͐M�����j�^ 3 */
    {0xF00A  , &undef_IntInSignalX4                 },/* 0xF00A: �������͐M�����j�^ 4 */
    {0xF00B  , &undef_IntOutSignalX1                },/* 0xF00B: �����o�͐M�����j�^1 */
    {0xF00C  , &undef_IntOutSignalX2                },/* 0xF00C: �����o�͐M�����j�^2 */
    {0xF00D  , &undef_IntOutSignalX3                },/* 0xF00D: �����o�͐M�����j�^3 */
    {0xF00E  , &undef_SelSpdOrDiv                   },/* 0xF00E: ���[�^�ő呬�x���/������ݒ����l */
    {0xFFFF  , NULL                                 },/* 0xFFFF: End of Table List */
};
const ULONG UnRegTableEntNum = sizeof(UnRegTable) / sizeof(UnRegTable[0]);


/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL UnMonTable[] = {
    {0x000   , &undef_SpdFbk                        },/* 0x000: ���[�^��]/�ړ����x */
    {0x001   , &undef_SpdRef                        },/* 0x001: ���x�w�� */
    {0x002   , &undef_TrqRef                        },/* 0x002: �����g���N/���͎w�� */
    {0x003   , &undef_MotAngPls                     },/* 0x003: �d�C�p1(32�r�b�g10�i�\��) */
    {0x004   , &undef_MotAngDeg                     },/* 0x005: �d�C�p2(U��0deg����̊p�x) */
    {0x005   , &undef_InSignal                      },/* 0x006: ���͐M�����j�^(dummy) */
    {0x006   , &undef_OutSignal                     },/* 0x007: �o�͐M�����j�^ */
    {0x007   , &undef_PosRefSpd                     },/* 0x008: ���͎w�߃p���X���x */
    {0x008   , &undef_PosErrCntr                    },/* 0x009: �΍��J�E���^(�ʒu�΍���) */
    {0x009   , &undef_MotLoad                       },/* 0x00B: �ݐϕ��ח� */
    {0x00A   , &undef_RegLoad                       },/* 0x00C: �񐶕��ח� */
    {0x00B   , &undef_DbrxPower                     },/* 0x00D: �c�a��R����d�� */
    {0x00C   , &undef_PcmdCounter                   },/* 0x00E: ���͎w�߃p���X�J�E���^ */
    {0x00D   , &undef_PfbkCounter                   },/* 0x010: ���[�^�G���R�[�_�ʒu */
    {0x00E   , &undef_FposCounter                   },/* 0x012: �t���N���[�Y�h�t�B�[�h�o�b�N�p���X�J�E���^ */
    {0x00F   , &undef_FcPfbkSpd                     },/* 0x014: �t���N���[�Y�h�t�B�[�h�o�b�N���x */
    {0x010   , &undef_SelSpdOrDiv                   },/* 0xF00E: ���[�^�ő呬�x���/������ݒ����l */
    {0x011   , &undef_LmPoleSensorX                 },/* 0x01A: �z�[���Z���T��� */
    {0x012   , &undef_PowerOnTime                   },/* 0x016: ���׉ғ����� */
    {0x013   , &undef_Pfbk2Counter                  },/* 0x01B: �t�B�[�h�o�b�N�p���X�J�E���^ */
    {0x014   , &undef_ActGain                       },/* 0x01D: �L���Q�C�����j�^ */
    {0x015   , &undef_SafetyIOSignal                },/* 0x01E: �Z�[�t�e�B���o�͐M�����j�^ */
    {0x016   , &undef_SafetyExternalIOSignal        },/* 0x01F: �Z�[�t�e�B���W���[���O�����o�͐M�����j�^ */
    {0x017   , &undef_SafetySettingMon              },/* 0x020: �Z�[�t�e�B���W���[�����S�@�\�ݒ�/���샂�j�^ */
    {0x018   , &undef_SafetySysStatMon              },/* 0x021: �Z�[�t�e�B���W���[���V�X�e����ԃ��j�^ */
    {0x019   , &undef_SafetySpdReachTimeA           },/* 0x022: �Z�[�t�e�B���W���[�����S���x���B����A */
    {0x01A   , &undef_SafetySpdReachTimeB           },/* 0x024: �Z�[�t�e�B���W���[�����S���x���B����B */
    {0x01B   , &undef_ActiveModeInterCmdSpd         },/* 0x026: �A�N�e�B�u���[�h�����w�ߑ��x */
    {0x01C   , &undef_SafetyMotorSpd                },/* 0x027: �Z�[�t�e�B���W���[�����[�^���x */
    {0x01D   , &undef_SafetyMotorPos                },/* 0x028: �Z�[�t�e�B���W���[�����[�^�ʒu */
    {0x01E   , &undef_SafetyMonitoringSpdA          },/* 0x02A: �Z�[�t�e�B���W���[���Ď����xA */
    {0x01F   , &undef_SafetyMonitoringSpdB          },/* 0x02B: �Z�[�t�e�B���W���[���Ď����xB */
    {0x020   , &undef_MotRatSpd                     },/* 0x046: ���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x021   , &undef_MotMaxSpd                     },/* 0x047: ���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x025   , &undef_AmpInstEnvMon                 },/* 0x02C: �T�[�{�p�b�N�ݒu�����j�^ */
    {0x026   , &undef_MotInstEnvMon                 },/* 0x02D: ���[�^�ݒu�����j�^ */
    {0x027   , &undef_BuiltInFanLifeMon             },/* 0x050: ����FAN�������j�^ */
    {0x028   , &undef_CondenserLifeMon              },/* 0x051: �R���f���T�������j�^ */
    {0x029   , &undef_CollisionProofCircuitLifeMon  },/* 0x052: �˖h��H�������j�^ */
    {0x02A   , &undef_DBCircuitLifeMon              },/* 0x053: DB��H�������j�^ */
    {0x02B   , &undef_CollisionProofRelayMotionNum  },/* 0x054: �˖h�����[����� */
    {0x02C   , &undef_DBRelayMotionNum              },/* 0x056: DB�����[����� */
    {0x030   , &undef_BlshComp                      },/* 0x030: �o�b�N���b�V�␳�� */
    {0x031   , &undef_BlshCompLimit                 },/* 0x032: �o�b�N���b�V�␳�ʐݒ萧���l */
    {0x032   , &undef_electricityConsumption        },/* 0x034: ����d�� */
    {0x033   , &undef_PowerConsumption              },/* 0x035: ����d�͗� */
    {0x034   , &undef_TotalPowerConsumptionConst    },/* 0x037: �ݐϏ���d�͗ʁi�������j */
    {0x035   , &undef_PosErr32s                     },/* 0x03E: �ʒu�A���v�΍� */
    {0x040   , &undef_MultiTurn                     },/* 0x040: ��Βl�G���R�[�_�}���`�^�[���f�[�^ */
    {0x041   , &undef_IncPulse                      },/* 0x041: ��Βl�G���R�[�_��1��]���ʒu */
    {0x042   , &undef_ScaleAbsPos_l                 },/* 0x043: ��Βl�G���R�[�_�̈ʒu�f�[�^�i���ʁj */
    {0x043   , &undef_ScaleAbsPos_h                 },/* 0x045: ��Βl�G���R�[�_�̈ʒu�f�[�^�i��ʁj */
    {0x050   , &undef_AllInSignal1                  },/* 0xF000: �S���͐M�����j�^1(for LCD Operator) */
    {0x051   , &undef_AllInSignal2                  },/* 0xF001: �S���͐M�����j�^2(for LCD Operator) */
    {0x052   , &undef_AllOutSignal                  },/* 0xF002: �S�o�͐M�����j�^(for LCD Operator) */
    {0x080   , &undef_MpfMovePosP                   },/* 0x080: ���Ɍ��o���������ő�ړ��� */
    {0x081   , &undef_MpfMovePosN                   },/* 0x081: ���Ɍ��o���t�����ő�ړ��� */
    {0x082   , &undef_MpfFindingTime                },/* 0x082: ���Ɍ��o���v���� */
    {0x083   , &undef_MpfPhaseOffset                },/* 0x083: ���Ɍ��o�ʑ��I�t�Z�b�g�� */
    {0x084   , &undef_LinearScalePitch              },/* 0x084: ���j�A�X�P�[���s�b�` */
    {0x085   , &undef_LinearScalePitchIndex         },/* 0x086: ���j�A�X�P�[���s�b�`�w�� */
    {0x086   , &undef_MpfJudgeMonitor               },/* 0x087: ���Ɍ��o���胂�j�^ */
    {0x104   , &undef_SencComErrCnt                 },/* 0x104: �V���A���G���R�[�_�ʐM�ُ�� */
    {0x105   , &undef_SettlingTime                  },/* 0x105: �ʒu���ߐ��莞�� */
    {0x106   , &undef_OverShoot                     },/* 0x106: �ʒu���߃I�[�o�V���[�g�� */
    {0x107   , &undef_RvibFrequency                 },/* 0x107: �c���U�����g�� */
    {0x10A   , &undef_HeatsinkTemp                  },/* 0x10A: �q�[�g�V���N���x���j�^ */
    {0x10B   , &undef_CoinOffTime                   },/* 0x10B: �ʒu���ߊ������s���ԐώZ */
    {0x10C   , &undef_VibDetSpd                     },/* 0x10C: ���U���o�p�����x */
    {0x10D   , &undef_CtrlBoardTemp                 },/* 0x10D: �������x���j�^ */
    {0x10E   , &undef_SifAdCnvData                  },/* 0x10E: �̓Z���T���j�^ */
    {0x10F   , &undef_Temperature                   },/* 0x10F: �G���R�[�_���C�� */
    {0x110   , &undef_InterSignalX                  },/* 0x110: �����M�����j�^ */
    {0x110   , &undef_InterSignalX1                 },/* 0xF003: �����M�����j�^1 */
    {0x111   , &undef_InterSignalX2                 },/* 0xF004: �����M�����j�^2 */
    {0x112   , &undef_InterSignalX3                 },/* 0xF005: �����M�����j�^3 */
    {0x113   , &undef_InterSignalX4                 },/* 0xF006: �����M�����j�^4 */
    {0x120   , &undef_IntInSignalX                  },/* 0x120: �������͐M�����j�^ */
    {0x120   , &undef_IntInSignalX1                 },/* 0xF007: �������͐M�����j�^ 1 */
    {0x121   , &undef_IntInSignalX2                 },/* 0xF008: �������͐M�����j�^ 2 */
    {0x122   , &undef_IntInSignalX3                 },/* 0xF009: �������͐M�����j�^ 3 */
    {0x123   , &undef_IntInSignalX4                 },/* 0xF00A: �������͐M�����j�^ 4 */
    {0x130   , &undef_IntOutSignalX                 },/* 0x130: �����o�͐M�����j�^ */
    {0x130   , &undef_IntOutSignalX1                },/* 0xF00B: �����o�͐M�����j�^1 */
    {0x131   , &undef_IntOutSignalX2                },/* 0xF00C: �����o�͐M�����j�^2 */
    {0x132   , &undef_IntOutSignalX3                },/* 0xF00D: �����o�͐M�����j�^3 */
    {0x140   , &undef_DcVoltx                       },/* 0x140: ���H����d�� */
    {0x141   , &undef_IqRate                        },/* 0x141: �d�����o�l */
    {0x142   , &undef_MotLoad2ms                    },/* 0x142: �ݐϕ��ח� */
    {0x143   , &undef_RegerRate2ms                  },/* 0x143: �񐶕��ח� */
    {0x144   , &undef_DbrxPower2ms                  },/* 0x144: �c�a��R����d�� */
    {0x145   , &undef_MotLoad10s                    },/* 0x145: �ő�ݐϕ��ח� */
    {0x146   , &undef_JLRatio                       },/* 0x146: �������[�����g��/�d�ʔ� */
    {0x148   , &undef_TrefAdMon                     },/* 0x148: T-REF���j�^ */
    {0x149   , &undef_VrefAdMon                     },/* 0x149: V-REF���j�^ */
    {0x14A   , &undef_DPulseHwCntMon                },/* 0x14A: ���͎w�߃p���X���g�� */
    {0x14C   , &undef_IdrefRate                     },/* 0x14C: d���d���w�߃��j�^ */
    {0x14D   , &undef_MbusErrReg                    },/* 0x14D: MEMOBUS�G���[���i�G���[�����A�h���X�j */
    {0x170   , &undef_AvgIntTmpr                    },/* 0x170: ���ϓ������x */
    {0x171   , &undef_ControlSubstrateTemp          },/* 0x171: �������x */
    {0x172   , &undef_PowerSubstrateTemp            },/* 0x172: �p���[����x */
    {0x1D0   , &undef_AlmLtSpdFbkX                  },/* 0x800: ���[�^��]/�ړ����x */
    {0x1D1   , &undef_AlmLtSpdRefX                  },/* 0x801: ���x�w�� */
    {0x1D2   , &undef_AlmLtTrqRefX                  },/* 0x802: �����g���N/���͎w�� */
    {0x1D3   , &undef_AlmLtPosRefSpdX               },/* 0x803: ���͎w�߃p���X���x */
    {0x1D4   , &undef_AlmLtPosErrCntrX              },/* 0x804: �΍��J�E���^(�ʒu�΍���) */
    {0x1DF   , &undef_AlmLtDcVoltxX                 },/* 0x820: ���H����d�� */
    {0x1E0   , &undef_AlmLtIqRateX                  },/* 0x821: �d�����o�l */
    {0x1E1   , &undef_AlmLtMotLoad2msX              },/* 0x822: �ݐϕ��ח� */
    {0x1E2   , &undef_AlmLtRegerRate2msX            },/* 0x823: �񐶕��ח� */
    {0x1E3   , &undef_AlmLtDbrxPower2msX            },/* 0x824: �c�a��R����d�� */
    {0x1E4   , &undef_AlmLtMotLoad10sX              },/* 0x825: �ő�ݐϕ��ח� */
    {0x1E5   , &undef_AlmLtJLRatioX                 },/* 0x826: �������[�����g��/�d�ʔ� */
    {0x1E6   , &undef_AlmLtSencComErrCntX           },/* 0x830: �V���A���G���R�[�_�ʐM�ُ�� */
    {0x1E7   , &undef_AlmLtM2ComErrCntX             },/* 0x831: MECHATROLINK�ʐM�ُ�� */
    {0x1E8   , &undef_AlmLtSafetyIOSignalX          },/* 0x832: �Z�[�t�e�B���o�͐M�����j�^ */
    {0x1E9   , &undef_AlmLtAlmCodeX                 },/* 0x833: �A���[���ԍ� */
    {0x1EA   , &undef_AlmLtTmStampX                 },/* 0x834: �^�C���X�^���v */
    {0x200   , &undef_MencTemp                      },/* 0x200: �G���R�[�_���C�����x���j�^ */
    {0x201   , &undef_RomChkSumVal                  },/* 0x201: ROM�`�F�b�N�T���l�i�V�X�e�����x���j */
    {0x202   , &undef_MbusErrReg                    },/* 0x202: MEMOBUS�G���[���i�G���[�����A�h���X�j */
    {0x203   , &undef_ErrPrmNo                      },/* 0x203: �ݒ�ُ�p�����[�^�ԍ�(A.040) */
    {0x204   , &undef_LmtPrmNo                      },/* 0x204: ���~�b�g�����p�����[�^�ԍ�(NoAlarm) */
    {0x212   , &undef_CurExeTimeMA                  },/* 0x314: �ʃv���O����ScanA���ݏ������� */
    {0x213   , &undef_MaxExeTimeMA                  },/* 0x316: �ʃv���O����ScanA�ő又������ */
    {0x214   , &undef_CurExeTimeB                   },/* 0x304: ScanB���ݏ������� */
    {0x215   , &undef_MaxExeTimeB                   },/* 0x306: ScanB�ő又������ */
    {0x216   , &undef_CurExeTimeC                   },/* 0x308: ScanC���ݏ������� */
    {0x217   , &undef_MaxExeTimeC                   },/* 0x30A: ScanC�ő又������ */
    {0x220   , &undef_CurExeTimeS                   },/* 0x300: 2ms�������CPU���ח� */
    {0x221   , &undef_MaxExeTimeS                   },/* 0x302: 2ms�������CPU�ő啉�ח� */
    {0xFFFF  , NULL                                 },/* 0xFFFF: End of Table List */
};
const ULONG UnMonTableEntNum = sizeof(UnMonTable) / sizeof(UnMonTable[0]);



/******************************* End of File *******************************/

