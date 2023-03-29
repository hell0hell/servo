/****************************************************************************
  Description   : Information Register Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

    Project       : Mercury

  Create Date   : 2014/02/26

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "InfRegFunc.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of 0x0000 : �T�[�{�@��ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id000 = {
    0x0000,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadServoID,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0001 : �T�[�{�p�����[�^�Z�b�gID(bit0�`bit7) */
/***************************************************************************/
const PRM_ATTR_T infdef_id001 = {
    0x0001,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadServoPrmVer,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0002 : �T�[�{�p�����[�^�Z�b�gID(bit8�`bit31) */
/***************************************************************************/
const PRM_ATTR_T infdef_id002 = {
    0x0002,                                 /* Register Number */
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
    3,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0005 : ���䃂�W���[���t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id005 = {
    0x0005,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFirmVersion,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0007 : ���䃂�W���[��Y�d�l�ԍ� */
/***************************************************************************/
static void *infdefGetRamPtr_id007(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->UniPrms->Prm->yopt);}

const PRM_ATTR_T infdef_id007 = {
    0x0007,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &infdefGetRamPtr_id007,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0009 : PWM�A���v���W���[���e��[W] */
/***************************************************************************/
static void *infdefGetRamPtr_id009(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ampw);}

const PRM_ATTR_T infdef_id009 = {
    0x0009,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &infdefGetRamPtr_id009,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000B : �R���o�[�^���͓d��/�o�͓d��[V] */
/***************************************************************************/
const PRM_ATTR_T infdef_id00B = {
    0x000B,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0101,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadServoVoltage,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000C : ���[�^ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id00C = {
    0x000C,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorID,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000D : �G���R�[�_�^�C�v�^����\ */
/***************************************************************************/
const PRM_ATTR_T infdef_id00D = {
    0x000D,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadEncoderTypeAndBits,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000E : �G���R�[�_�t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id00E = {
    0x000E,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadEncoderVer,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000F : ���[�^�e��[W] */
/***************************************************************************/
const PRM_ATTR_T infdef_id00F = {
    0x000F,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorWattage,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0011 : ���[�^�d��[V] */
/***************************************************************************/
const PRM_ATTR_T infdef_id011 = {
    0x0011,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorVoltage,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0012 : ��Βl�G���R�[�_�^�C�v */
/***************************************************************************/
const PRM_ATTR_T infdef_id012 = {
    0x0012,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadEncoderType,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0013 : �A�v�����W���[��ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id013 = {
    0x0013,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0014 : �A�v�����W���[���p�����[�^�Z�b�gID */
/***************************************************************************/
const PRM_ATTR_T infdef_id014 = {
    0x0014,                                 /* Register Number */
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
    4,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0018 : �A�v�����W���[���t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id018 = {
    0x0018,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0019 : �A�v�����W���[���t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id019 = {
    0x0019,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001A : �A�v�����W���[���x�d�l�ԍ� */
/***************************************************************************/
const PRM_ATTR_T infdef_id01A = {
    0x001A,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001B : �A�v�����W���[���g�v�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id01B = {
    0x001B,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001C : ���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01C = {
    0x001C,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorRatedSpeed,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001D : ���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01D = {
    0x001D,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorMaxSpeed,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001E : ���[�^���x�w�� [10�ׂ̂���] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01E = {
    0x001E,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorSpeedExp,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001F : ���[�^��i�g���N [��]�^�FN�m, ���j�A�^�FN] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01F = {
    0x001F,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorRatedTorque,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0020 : ���[�^�ő�g���N [��]�^�FN�m, ���j�A�^�FN] */
/***************************************************************************/
const PRM_ATTR_T infdef_id020 = {
    0x0020,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorMaxTorque,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0021 : ���[�^�g���N�w��[10�ׂ̂���] */
/***************************************************************************/
const PRM_ATTR_T infdef_id021 = {
    0x0021,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMotorTorqueExp,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0022 : �G���R�[�_����\[��]�^�Fpulse/rev,���j�A�E�t���N�^�Fpulse/pitch] */
/***************************************************************************/
const PRM_ATTR_T infdef_id022 = {
    0x0022,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadEncoderResolution,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0024 : �}���`�^�[�����~�b�g�l */
/***************************************************************************/
const PRM_ATTR_T infdef_id024 = {
    0x0024,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMultiTurnLimit,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0026 : �o�͉\�ő�g���N�@[��]�^�FN�m, ���j�A�^�FN]  */
/***************************************************************************/
const PRM_ATTR_T infdef_id026 = {
    0x0026,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMaxTorqueRef,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0027 : ����\���[�^�ō����x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
/***************************************************************************/
const PRM_ATTR_T infdef_id027 = {
    0x0027,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadMaxSpeedRef,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0028 : �t���N�X�P�[���^�C�v */
/***************************************************************************/
const PRM_ATTR_T infdef_id028 = {
    0x0028,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadFullcScaleType,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0029 : ��Βl���j�A�X�P�[�����_�ʒu�ݒ� */
/***************************************************************************/
const PRM_ATTR_T infdef_id029 = {
    0x0029,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadAbsScaleZeroSetting,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002A : ���j�A�X�P�[���s�b�`[pm] */
/***************************************************************************/
const PRM_ATTR_T infdef_id02A = {
    0x002A,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadLinearScalePitch,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002C : ���j�A�X�P�[���s�b�`�w��[10�ׂ̂���] */
/***************************************************************************/
const PRM_ATTR_T infdef_id02C = {
    0x002C,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadLinearScalePitchExp,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0030 : �T�[�{�p�b�N�t�@�[���E�F�A�u�[�g�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id030 = {
    0x0030,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadServoFirmwareBootVer,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0040 : �w�߃I�v�V�����J�[�h�@��ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id040 = {
    0x0040,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadCmdCardID,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0041 : �w�߃I�v�V�����J�[�h�t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id041 = {
    0x0041,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadCmdCardFirmwareVer,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0043 : �w�߃I�v�V�����J�[�hY�d�l�ԍ� */
/***************************************************************************/
const PRM_ATTR_T infdef_id043 = {
    0x0043,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadCmdCardYspecVer,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0044 : �Z�[�t�e�B���W���[���@��ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id044 = {
    0x0044,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetyID,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0045 : �Z�[�t�e�B���W���[���t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id045 = {
    0x0045,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetyFirmwareVer,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0047 : �Z�[�t�e�B���W���[��Y�d�l�ԍ� */
/***************************************************************************/
const PRM_ATTR_T infdef_id047 = {
    0x0047,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetyYspecVer,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0048 : �t�B�[�h�o�b�N�I�v�V�����J�[�h�@��ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id048 = {
    0x0048,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbID,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0049 : �t�B�[�h�o�b�N�I�v�V�����J�[�h�t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id049 = {
    0x0049,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbFirmwareVer,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x004B : �t�B�[�h�o�b�N�I�v�V�����J�[�hY�d�l�ԍ� */
/***************************************************************************/
const PRM_ATTR_T infdef_id04B = {
    0x004B,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbYspecVer,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x004C : �t�B�[�h�o�b�N�I�v�V���������[�^ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id04C = {
    0x004C,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbMotorID,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x004D : �t�B�[�h�o�b�N�I�v�V���������[�^�e��[W] */
/***************************************************************************/
const PRM_ATTR_T infdef_id04D = {
    0x004D,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbMotorWattage,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x004F : �t�B�[�h�o�b�N�I�v�V���������[�^�d��[V] */
/***************************************************************************/
const PRM_ATTR_T infdef_id04F = {
    0x004F,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbMotorVoltage,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0050 : �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�^�C�v�^����\ */
/***************************************************************************/
const PRM_ATTR_T infdef_id050 = {
    0x0050,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbEncoderTypeAndBits,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0051 : �t�B�[�h�o�b�N�I�v�V�������G���R�[�_����\ */
/***************************************************************************/
const PRM_ATTR_T infdef_id051 = {
    0x0051,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbEncoderBits,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0053 : �t�B�[�h�o�b�N�I�v�V��������Βl�G���R�[�_�^�C�v */
/***************************************************************************/
const PRM_ATTR_T infdef_id053 = {
    0x0053,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbAbsEncoderType,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0054 : �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�t�@�[���E�F�A�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id054 = {
    0x0054,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbEncoderVer,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0100 : ���W�X�^�X�y�b�N�o�[�W���� */
/***************************************************************************/
const PRM_ATTR_T infdef_id100 = {
    0x0100,                                 /* Register Number */
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
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    REG_SPEC_VER,                           /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0800 : ���䃂�W���[���`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id800 = {
    0x0800,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadModuleIDProductName,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0808 : ���䃂�W���[���V���A���i���o�[������ */
/***************************************************************************/
const PRM_ATTR_T infdef_id808 = {
    0x0808,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadModuleIDSerialNumber,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0810 : ���䃂�W���[�������N�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id810 = {
    0x0810,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadModuleIDDate,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0811 : ���[�^�`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id811 = {
    0x0811,                                 /* Register Number */
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
    11,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadMotorProductName,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x081C : ���[�^�V���A���i���o�[������ */
/***************************************************************************/
const PRM_ATTR_T infdef_id81C = {
    0x081C,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadMotorSerialNumber,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0824 : ���[�^�����N�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id824 = {
    0x0824,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadMotorManufactureDate,          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0825 : �G���R�[�_�`���̈� */
/***************************************************************************/
const PRM_ATTR_T infdef_id825 = {
    0x0825,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadEncoderProductName,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x082D : �G���R�[�_�V���A��No�̈� */
/***************************************************************************/
const PRM_ATTR_T infdef_id82D = {
    0x082D,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadEncoderSerialNumber,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0835 : �G���R�[�_�����N���̈� */
/***************************************************************************/
const PRM_ATTR_T infdef_id835 = {
    0x0835,                                 /* Register Number */
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
    1,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadEncoderManufactureDate,        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0845 : ���䎲�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id845 = {
    0x0845,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EinfReadNumberOfAxes,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0860 : �T�[�{�p�b�N�`���g���̈� */
/***************************************************************************/
static void *infdefGetRamPtr_id860(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID->IfInfo)->ProductName[16+ArrayIdx*2]);}

const PRM_ATTR_T infdef_id860 = {
    0x0860,                                 /* Register Number */
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
    4,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &infdefGetRamPtr_id860,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0864 : �w�߃I�v�V�����J�[�h�`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id864 = {
    0x0864,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadCmdType,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0870 : �w�߃I�v�V�����J�[�h�V���A��No�̈� */
/***************************************************************************/
const PRM_ATTR_T infdef_id870 = {
    0x0870,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadCmdSerialNo,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x087C : �w�߃I�v�V�����J�[�h�����N���̈� */
/***************************************************************************/
const PRM_ATTR_T infdef_id87C = {
    0x087C,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadCmdManufactureDate,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x087D : �Z�[�t�e�B���W���[���`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id87D = {
    0x087D,                                 /* Register Number */
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
    12,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetyType,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0889 : �Z�[�t�e�B���W���[���V���A���i���o�[������ */
/***************************************************************************/
const PRM_ATTR_T infdef_id889 = {
    0x0889,                                 /* Register Number */
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
    12,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetySerialNo,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0895 : �Z�[�t�e�B���W���[�������N�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id895 = {
    0x0895,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetyManufactureDate,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0896 : �t�B�[�h�o�b�N�I�v�V�����J�[�h�`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id896 = {
    0x0896,                                 /* Register Number */
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
    12,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbType,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08A2 : �t�B�[�h�o�b�N�I�v�V�����J�[�h�V���A���i���o�[������ */
/***************************************************************************/
const PRM_ATTR_T infdef_id8A2 = {
    0x08A2,                                 /* Register Number */
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
    12,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbSerialNo,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08AE : �t�B�[�h�o�b�N�I�v�V�����J�[�h�����N�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8AE = {
    0x08AE,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbManufactureDate,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08AF : �t�B�[�h�o�b�N�I�v�V���������[�^�`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8AF = {
    0x08AF,                                 /* Register Number */
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
    11,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbMotorType,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08BA : �t�B�[�h�o�b�N�I�v�V���������[�^�V���A���i���o�[������ */
/***************************************************************************/
const PRM_ATTR_T infdef_id8BA = {
    0x08BA,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbMotorSerialNo,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08C2 : �t�B�[�h�o�b�N�I�v�V���������[�^�����N�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8C2 = {
    0x08C2,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbMotorManufactureDate,        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08C3 : �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�`�������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8C3 = {
    0x08C3,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbEncoderType,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08CB : �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�V���A���i���o�[������ */
/***************************************************************************/
const PRM_ATTR_T infdef_id8CB = {
    0x08CB,                                 /* Register Number */
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
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbEncoderSerialNo,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08D3 : �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�����N�� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8D3 = {
    0x08D3,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadFbEncoderManufactureDate,      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08E0 : ���͐M�� �L��/������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8E0 = {
    0x08E0,                                 /* Register Number */
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
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadInputSignalInfo,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08E2 : �o�͐M�� �L��/������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8E2 = {
    0x08E2,                                 /* Register Number */
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
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadOutputSignalInfo,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08E4 : Safety���o�͐M�� �L��/������� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8E4 = {
    0x08E4,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadSafetyIOSignalInfo,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08F0 : �T�[�{�p�b�NBTO�ԍ� */
/***************************************************************************/
const PRM_ATTR_T infdef_id8F0 = {
    0x08F0,                                 /* Register Number */
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
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadServoBtoNo,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0910 : �T�[�{�p�b�N����d�l��� */
/***************************************************************************/
const PRM_ATTR_T infdef_id910 = {
    0x0910,                                 /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &EInfReadServoSpecialSpec,              /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL InfoRegTable[] = {
    {0x0000  , &infdef_id000                   },/* 0x0000: �T�[�{�@��ID */
    {0x0001  , &infdef_id001                   },/* 0x0001: �T�[�{�p�����[�^�Z�b�gID(bit0�`bit7) */
    {0x0002  , &infdef_id002                   },/* 0x0002: �T�[�{�p�����[�^�Z�b�gID(bit8�`bit31) */
    {0x0005  , &infdef_id005                   },/* 0x0005: ���䃂�W���[���t�@�[���E�F�A�o�[�W���� */
    {0x0007  , &infdef_id007                   },/* 0x0007: ���䃂�W���[��Y�d�l�ԍ� */
    {0x0009  , &infdef_id009                   },/* 0x0009: PWM�A���v���W���[���e��[W] */
    {0x000B  , &infdef_id00B                   },/* 0x000B: �R���o�[�^���͓d��/�o�͓d��[V] */
    {0x000C  , &infdef_id00C                   },/* 0x000C: ���[�^ID */
    {0x000D  , &infdef_id00D                   },/* 0x000D: �G���R�[�_�^�C�v�^����\ */
    {0x000E  , &infdef_id00E                   },/* 0x000E: �G���R�[�_�t�@�[���E�F�A�o�[�W���� */
    {0x000F  , &infdef_id00F                   },/* 0x000F: ���[�^�e��[W] */
    {0x0011  , &infdef_id011                   },/* 0x0011: ���[�^�d��[V] */
    {0x0012  , &infdef_id012                   },/* 0x0012: ��Βl�G���R�[�_�^�C�v */
    {0x0013  , &infdef_id013                   },/* 0x0013: �A�v�����W���[��ID */
    {0x0014  , &infdef_id014                   },/* 0x0014: �A�v�����W���[���p�����[�^�Z�b�gID */
    {0x0018  , &infdef_id018                   },/* 0x0018: �A�v�����W���[���t�@�[���E�F�A�o�[�W���� */
    {0x0019  , &infdef_id019                   },/* 0x0019: �A�v�����W���[���t�@�[���E�F�A�o�[�W���� */
    {0x001A  , &infdef_id01A                   },/* 0x001A: �A�v�����W���[���x�d�l�ԍ� */
    {0x001B  , &infdef_id01B                   },/* 0x001B: �A�v�����W���[���g�v�o�[�W���� */
    {0x001C  , &infdef_id01C                   },/* 0x001C: ���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x001D  , &infdef_id01D                   },/* 0x001D: ���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x001E  , &infdef_id01E                   },/* 0x001E: ���[�^���x�w�� [10�ׂ̂���] */
    {0x001F  , &infdef_id01F                   },/* 0x001F: ���[�^��i�g���N [��]�^�FN�m, ���j�A�^�FN] */
    {0x0020  , &infdef_id020                   },/* 0x0020: ���[�^�ő�g���N [��]�^�FN�m, ���j�A�^�FN] */
    {0x0021  , &infdef_id021                   },/* 0x0021: ���[�^�g���N�w��[10�ׂ̂���] */
    {0x0022  , &infdef_id022                   },/* 0x0022: �G���R�[�_����\[��]�^�Fpulse/rev,���j�A�E�t���N�^�Fpulse/pitch] */
    {0x0024  , &infdef_id024                   },/* 0x0024: �}���`�^�[�����~�b�g�l */
    {0x0026  , &infdef_id026                   },/* 0x0026: �o�͉\�ő�g���N�@[��]�^�FN�m, ���j�A�^�FN]  */
    {0x0027  , &infdef_id027                   },/* 0x0027: ����\���[�^�ō����x [��]�^�Fmin-1,���j�A�^�Fmm/s] */
    {0x0028  , &infdef_id028                   },/* 0x0028: �t���N�X�P�[���^�C�v */
    {0x0029  , &infdef_id029                   },/* 0x0029: ��Βl���j�A�X�P�[�����_�ʒu�ݒ� */
    {0x002A  , &infdef_id02A                   },/* 0x002A: ���j�A�X�P�[���s�b�`[pm] */
    {0x002C  , &infdef_id02C                   },/* 0x002C: ���j�A�X�P�[���s�b�`�w��[10�ׂ̂���] */
    {0x0030  , &infdef_id030                   },/* 0x0030: �T�[�{�p�b�N�t�@�[���E�F�A�u�[�g�o�[�W���� */
    {0x0040  , &infdef_id040                   },/* 0x0040: �w�߃I�v�V�����J�[�h�@��ID */
    {0x0041  , &infdef_id041                   },/* 0x0041: �w�߃I�v�V�����J�[�h�t�@�[���E�F�A�o�[�W���� */
    {0x0043  , &infdef_id043                   },/* 0x0043: �w�߃I�v�V�����J�[�hY�d�l�ԍ� */
    {0x0044  , &infdef_id044                   },/* 0x0044: �Z�[�t�e�B���W���[���@��ID */
    {0x0045  , &infdef_id045                   },/* 0x0045: �Z�[�t�e�B���W���[���t�@�[���E�F�A�o�[�W���� */
    {0x0047  , &infdef_id047                   },/* 0x0047: �Z�[�t�e�B���W���[��Y�d�l�ԍ� */
    {0x0048  , &infdef_id048                   },/* 0x0048: �t�B�[�h�o�b�N�I�v�V�����J�[�h�@��ID */
    {0x0049  , &infdef_id049                   },/* 0x0049: �t�B�[�h�o�b�N�I�v�V�����J�[�h�t�@�[���E�F�A�o�[�W���� */
    {0x004B  , &infdef_id04B                   },/* 0x004B: �t�B�[�h�o�b�N�I�v�V�����J�[�hY�d�l�ԍ� */
    {0x004C  , &infdef_id04C                   },/* 0x004C: �t�B�[�h�o�b�N�I�v�V���������[�^ID */
    {0x004D  , &infdef_id04D                   },/* 0x004D: �t�B�[�h�o�b�N�I�v�V���������[�^�e��[W] */
    {0x004F  , &infdef_id04F                   },/* 0x004F: �t�B�[�h�o�b�N�I�v�V���������[�^�d��[V] */
    {0x0050  , &infdef_id050                   },/* 0x0050: �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�^�C�v�^����\ */
    {0x0051  , &infdef_id051                   },/* 0x0051: �t�B�[�h�o�b�N�I�v�V�������G���R�[�_����\ */
    {0x0053  , &infdef_id053                   },/* 0x0053: �t�B�[�h�o�b�N�I�v�V��������Βl�G���R�[�_�^�C�v */
    {0x0054  , &infdef_id054                   },/* 0x0054: �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�t�@�[���E�F�A�o�[�W���� */
    {0x0100  , &infdef_id100                   },/* 0x0100: ���W�X�^�X�y�b�N�o�[�W���� */
    {0x0800  , &infdef_id800                   },/* 0x0800: ���䃂�W���[���`�������� */
    {0x0808  , &infdef_id808                   },/* 0x0808: ���䃂�W���[���V���A���i���o�[������ */
    {0x0810  , &infdef_id810                   },/* 0x0810: ���䃂�W���[�������N�� */
    {0x0811  , &infdef_id811                   },/* 0x0811: ���[�^�`�������� */
    {0x081C  , &infdef_id81C                   },/* 0x081C: ���[�^�V���A���i���o�[������ */
    {0x0824  , &infdef_id824                   },/* 0x0824: ���[�^�����N�� */
    {0x0825  , &infdef_id825                   },/* 0x0825: �G���R�[�_�`���̈� */
    {0x082D  , &infdef_id82D                   },/* 0x082D: �G���R�[�_�V���A��No�̈� */
    {0x0835  , &infdef_id835                   },/* 0x0835: �G���R�[�_�����N���̈� */
    {0x0845  , &infdef_id845                   },/* 0x0845: ���䎲�� */
    {0x0860  , &infdef_id860                   },/* 0x0860: �T�[�{�p�b�N�`���g���̈� */
    {0x0864  , &infdef_id864                   },/* 0x0864: �w�߃I�v�V�����J�[�h�`�������� */
    {0x0870  , &infdef_id870                   },/* 0x0870: �w�߃I�v�V�����J�[�h�V���A��No�̈� */
    {0x087C  , &infdef_id87C                   },/* 0x087C: �w�߃I�v�V�����J�[�h�����N���̈� */
    {0x087D  , &infdef_id87D                   },/* 0x087D: �Z�[�t�e�B���W���[���`�������� */
    {0x0889  , &infdef_id889                   },/* 0x0889: �Z�[�t�e�B���W���[���V���A���i���o�[������ */
    {0x0895  , &infdef_id895                   },/* 0x0895: �Z�[�t�e�B���W���[�������N�� */
    {0x0896  , &infdef_id896                   },/* 0x0896: �t�B�[�h�o�b�N�I�v�V�����J�[�h�`�������� */
    {0x08A2  , &infdef_id8A2                   },/* 0x08A2: �t�B�[�h�o�b�N�I�v�V�����J�[�h�V���A���i���o�[������ */
    {0x08AE  , &infdef_id8AE                   },/* 0x08AE: �t�B�[�h�o�b�N�I�v�V�����J�[�h�����N�� */
    {0x08AF  , &infdef_id8AF                   },/* 0x08AF: �t�B�[�h�o�b�N�I�v�V���������[�^�`�������� */
    {0x08BA  , &infdef_id8BA                   },/* 0x08BA: �t�B�[�h�o�b�N�I�v�V���������[�^�V���A���i���o�[������ */
    {0x08C2  , &infdef_id8C2                   },/* 0x08C2: �t�B�[�h�o�b�N�I�v�V���������[�^�����N�� */
    {0x08C3  , &infdef_id8C3                   },/* 0x08C3: �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�`�������� */
    {0x08CB  , &infdef_id8CB                   },/* 0x08CB: �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�V���A���i���o�[������ */
    {0x08D3  , &infdef_id8D3                   },/* 0x08D3: �t�B�[�h�o�b�N�I�v�V�������G���R�[�_�����N�� */
    {0x08E0  , &infdef_id8E0                   },/* 0x08E0: ���͐M�� �L��/������� */
    {0x08E2  , &infdef_id8E2                   },/* 0x08E2: �o�͐M�� �L��/������� */
    {0x08E4  , &infdef_id8E4                   },/* 0x08E4: Safety���o�͐M�� �L��/������� */
    {0x08F0  , &infdef_id8F0                   },/* 0x08F0: �T�[�{�p�b�NBTO�ԍ� */
    {0x0910  , &infdef_id910                   },/* 0x0910: �T�[�{�p�b�N����d�l��� */
    {0xFFFF  , NULL                            },/* 0xFFFF: End of Table List */
};
const ULONG InfoRegTableEntNum = sizeof(InfoRegTable) / sizeof(InfoRegTable[0]);




/******************************* End of File *******************************/

