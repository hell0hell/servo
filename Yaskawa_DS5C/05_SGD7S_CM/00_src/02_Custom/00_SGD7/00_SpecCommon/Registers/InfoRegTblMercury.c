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
/*  Attribute of 0x0000 : サーボ機種ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id000 = {
    0x0000,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0001 : サーボパラメータセットID(bit0～bit7) */
/***************************************************************************/
const PRM_ATTR_T infdef_id001 = {
    0x0001,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0002 : サーボパラメータセットID(bit8～bit31) */
/***************************************************************************/
const PRM_ATTR_T infdef_id002 = {
    0x0002,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0005 : 制御モジュールファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id005 = {
    0x0005,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0007 : 制御モジュールY仕様番号 */
/***************************************************************************/
static void *infdefGetRamPtr_id007(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->UniPrms->Prm->yopt);}

const PRM_ATTR_T infdef_id007 = {
    0x0007,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0009 : PWMアンプモジュール容量[W] */
/***************************************************************************/
static void *infdefGetRamPtr_id009(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ampw);}

const PRM_ATTR_T infdef_id009 = {
    0x0009,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x000B : コンバータ入力電圧/出力電圧[V] */
/***************************************************************************/
const PRM_ATTR_T infdef_id00B = {
    0x000B,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x000C : モータID */
/***************************************************************************/
const PRM_ATTR_T infdef_id00C = {
    0x000C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x000D : エンコーダタイプ／分解能 */
/***************************************************************************/
const PRM_ATTR_T infdef_id00D = {
    0x000D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x000E : エンコーダファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id00E = {
    0x000E,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x000F : モータ容量[W] */
/***************************************************************************/
const PRM_ATTR_T infdef_id00F = {
    0x000F,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0011 : モータ電圧[V] */
/***************************************************************************/
const PRM_ATTR_T infdef_id011 = {
    0x0011,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0012 : 絶対値エンコーダタイプ */
/***************************************************************************/
const PRM_ATTR_T infdef_id012 = {
    0x0012,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0013 : アプリモジュールID */
/***************************************************************************/
const PRM_ATTR_T infdef_id013 = {
    0x0013,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0014 : アプリモジュールパラメータセットID */
/***************************************************************************/
const PRM_ATTR_T infdef_id014 = {
    0x0014,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0018 : アプリモジュールファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id018 = {
    0x0018,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0019 : アプリモジュールファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id019 = {
    0x0019,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001A : アプリモジュールＹ仕様番号 */
/***************************************************************************/
const PRM_ATTR_T infdef_id01A = {
    0x001A,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001B : アプリモジュールＨＷバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id01B = {
    0x001B,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001C : モータ定格速度 [回転型：min-1,リニア型：mm/s] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01C = {
    0x001C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001D : モータ最大速度 [回転型：min-1,リニア型：mm/s] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01D = {
    0x001D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001E : モータ速度指数 [10のべき乗] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01E = {
    0x001E,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001F : モータ定格トルク [回転型：N･m, リニア型：N] */
/***************************************************************************/
const PRM_ATTR_T infdef_id01F = {
    0x001F,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0020 : モータ最大トルク [回転型：N･m, リニア型：N] */
/***************************************************************************/
const PRM_ATTR_T infdef_id020 = {
    0x0020,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0021 : モータトルク指数[10のべき乗] */
/***************************************************************************/
const PRM_ATTR_T infdef_id021 = {
    0x0021,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0022 : エンコーダ分解能[回転型：pulse/rev,リニア・フルク型：pulse/pitch] */
/***************************************************************************/
const PRM_ATTR_T infdef_id022 = {
    0x0022,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0024 : マルチターンリミット値 */
/***************************************************************************/
const PRM_ATTR_T infdef_id024 = {
    0x0024,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0026 : 出力可能最大トルク　[回転型：N･m, リニア型：N]  */
/***************************************************************************/
const PRM_ATTR_T infdef_id026 = {
    0x0026,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0027 : 動作可能モータ最高速度 [回転型：min-1,リニア型：mm/s] */
/***************************************************************************/
const PRM_ATTR_T infdef_id027 = {
    0x0027,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0028 : フルクスケールタイプ */
/***************************************************************************/
const PRM_ATTR_T infdef_id028 = {
    0x0028,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0029 : 絶対値リニアスケール原点位置設定 */
/***************************************************************************/
const PRM_ATTR_T infdef_id029 = {
    0x0029,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x002A : リニアスケールピッチ[pm] */
/***************************************************************************/
const PRM_ATTR_T infdef_id02A = {
    0x002A,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x002C : リニアスケールピッチ指数[10のべき乗] */
/***************************************************************************/
const PRM_ATTR_T infdef_id02C = {
    0x002C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0030 : サーボパックファームウェアブートバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id030 = {
    0x0030,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0040 : 指令オプションカード機種ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id040 = {
    0x0040,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0041 : 指令オプションカードファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id041 = {
    0x0041,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0043 : 指令オプションカードY仕様番号 */
/***************************************************************************/
const PRM_ATTR_T infdef_id043 = {
    0x0043,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0044 : セーフティモジュール機種ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id044 = {
    0x0044,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0045 : セーフティモジュールファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id045 = {
    0x0045,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0047 : セーフティモジュールY仕様番号 */
/***************************************************************************/
const PRM_ATTR_T infdef_id047 = {
    0x0047,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0048 : フィードバックオプションカード機種ID */
/***************************************************************************/
const PRM_ATTR_T infdef_id048 = {
    0x0048,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0049 : フィードバックオプションカードファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id049 = {
    0x0049,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x004B : フィードバックオプションカードY仕様番号 */
/***************************************************************************/
const PRM_ATTR_T infdef_id04B = {
    0x004B,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x004C : フィードバックオプション内モータID */
/***************************************************************************/
const PRM_ATTR_T infdef_id04C = {
    0x004C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x004D : フィードバックオプション内モータ容量[W] */
/***************************************************************************/
const PRM_ATTR_T infdef_id04D = {
    0x004D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x004F : フィードバックオプション内モータ電圧[V] */
/***************************************************************************/
const PRM_ATTR_T infdef_id04F = {
    0x004F,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0050 : フィードバックオプション内エンコーダタイプ／分解能 */
/***************************************************************************/
const PRM_ATTR_T infdef_id050 = {
    0x0050,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0051 : フィードバックオプション内エンコーダ分解能 */
/***************************************************************************/
const PRM_ATTR_T infdef_id051 = {
    0x0051,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0053 : フィードバックオプション内絶対値エンコーダタイプ */
/***************************************************************************/
const PRM_ATTR_T infdef_id053 = {
    0x0053,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0054 : フィードバックオプション内エンコーダファームウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id054 = {
    0x0054,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0100 : レジスタスペックバージョン */
/***************************************************************************/
const PRM_ATTR_T infdef_id100 = {
    0x0100,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0800 : 制御モジュール形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id800 = {
    0x0800,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0808 : 制御モジュールシリアルナンバー文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id808 = {
    0x0808,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0810 : 制御モジュール製造年月 */
/***************************************************************************/
const PRM_ATTR_T infdef_id810 = {
    0x0810,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0811 : モータ形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id811 = {
    0x0811,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x081C : モータシリアルナンバー文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id81C = {
    0x081C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0824 : モータ製造年月 */
/***************************************************************************/
const PRM_ATTR_T infdef_id824 = {
    0x0824,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0825 : エンコーダ形式領域 */
/***************************************************************************/
const PRM_ATTR_T infdef_id825 = {
    0x0825,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x082D : エンコーダシリアルNo領域 */
/***************************************************************************/
const PRM_ATTR_T infdef_id82D = {
    0x082D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0835 : エンコーダ製造年月領域 */
/***************************************************************************/
const PRM_ATTR_T infdef_id835 = {
    0x0835,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x0845 : 制御軸数 */
/***************************************************************************/
const PRM_ATTR_T infdef_id845 = {
    0x0845,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0860 : サーボパック形式拡張領域 */
/***************************************************************************/
static void *infdefGetRamPtr_id860(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID->IfInfo)->ProductName[16+ArrayIdx*2]);}

const PRM_ATTR_T infdef_id860 = {
    0x0860,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0864 : 指令オプションカード形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id864 = {
    0x0864,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0870 : 指令オプションカードシリアルNo領域 */
/***************************************************************************/
const PRM_ATTR_T infdef_id870 = {
    0x0870,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x087C : 指令オプションカード製造年月領域 */
/***************************************************************************/
const PRM_ATTR_T infdef_id87C = {
    0x087C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x087D : セーフティモジュール形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id87D = {
    0x087D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0889 : セーフティモジュールシリアルナンバー文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id889 = {
    0x0889,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0895 : セーフティモジュール製造年月 */
/***************************************************************************/
const PRM_ATTR_T infdef_id895 = {
    0x0895,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0896 : フィードバックオプションカード形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id896 = {
    0x0896,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08A2 : フィードバックオプションカードシリアルナンバー文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8A2 = {
    0x08A2,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08AE : フィードバックオプションカード製造年月 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8AE = {
    0x08AE,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08AF : フィードバックオプション内モータ形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8AF = {
    0x08AF,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08BA : フィードバックオプション内モータシリアルナンバー文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8BA = {
    0x08BA,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08C2 : フィードバックオプション内モータ製造年月 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8C2 = {
    0x08C2,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08C3 : フィードバックオプション内エンコーダ形式文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8C3 = {
    0x08C3,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08CB : フィードバックオプション内エンコーダシリアルナンバー文字列 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8CB = {
    0x08CB,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08D3 : フィードバックオプション内エンコーダ製造年月 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8D3 = {
    0x08D3,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08E0 : 入力信号 有効/無効情報 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8E0 = {
    0x08E0,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08E2 : 出力信号 有効/無効情報 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8E2 = {
    0x08E2,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08E4 : Safety入出力信号 有効/無効情報 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8E4 = {
    0x08E4,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x08F0 : サーボパックBTO番号 */
/***************************************************************************/
const PRM_ATTR_T infdef_id8F0 = {
    0x08F0,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x0910 : サーボパック特殊仕様種別 */
/***************************************************************************/
const PRM_ATTR_T infdef_id910 = {
    0x0910,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
    {0x0000  , &infdef_id000                   },/* 0x0000: サーボ機種ID */
    {0x0001  , &infdef_id001                   },/* 0x0001: サーボパラメータセットID(bit0～bit7) */
    {0x0002  , &infdef_id002                   },/* 0x0002: サーボパラメータセットID(bit8～bit31) */
    {0x0005  , &infdef_id005                   },/* 0x0005: 制御モジュールファームウェアバージョン */
    {0x0007  , &infdef_id007                   },/* 0x0007: 制御モジュールY仕様番号 */
    {0x0009  , &infdef_id009                   },/* 0x0009: PWMアンプモジュール容量[W] */
    {0x000B  , &infdef_id00B                   },/* 0x000B: コンバータ入力電圧/出力電圧[V] */
    {0x000C  , &infdef_id00C                   },/* 0x000C: モータID */
    {0x000D  , &infdef_id00D                   },/* 0x000D: エンコーダタイプ／分解能 */
    {0x000E  , &infdef_id00E                   },/* 0x000E: エンコーダファームウェアバージョン */
    {0x000F  , &infdef_id00F                   },/* 0x000F: モータ容量[W] */
    {0x0011  , &infdef_id011                   },/* 0x0011: モータ電圧[V] */
    {0x0012  , &infdef_id012                   },/* 0x0012: 絶対値エンコーダタイプ */
    {0x0013  , &infdef_id013                   },/* 0x0013: アプリモジュールID */
    {0x0014  , &infdef_id014                   },/* 0x0014: アプリモジュールパラメータセットID */
    {0x0018  , &infdef_id018                   },/* 0x0018: アプリモジュールファームウェアバージョン */
    {0x0019  , &infdef_id019                   },/* 0x0019: アプリモジュールファームウェアバージョン */
    {0x001A  , &infdef_id01A                   },/* 0x001A: アプリモジュールＹ仕様番号 */
    {0x001B  , &infdef_id01B                   },/* 0x001B: アプリモジュールＨＷバージョン */
    {0x001C  , &infdef_id01C                   },/* 0x001C: モータ定格速度 [回転型：min-1,リニア型：mm/s] */
    {0x001D  , &infdef_id01D                   },/* 0x001D: モータ最大速度 [回転型：min-1,リニア型：mm/s] */
    {0x001E  , &infdef_id01E                   },/* 0x001E: モータ速度指数 [10のべき乗] */
    {0x001F  , &infdef_id01F                   },/* 0x001F: モータ定格トルク [回転型：N･m, リニア型：N] */
    {0x0020  , &infdef_id020                   },/* 0x0020: モータ最大トルク [回転型：N･m, リニア型：N] */
    {0x0021  , &infdef_id021                   },/* 0x0021: モータトルク指数[10のべき乗] */
    {0x0022  , &infdef_id022                   },/* 0x0022: エンコーダ分解能[回転型：pulse/rev,リニア・フルク型：pulse/pitch] */
    {0x0024  , &infdef_id024                   },/* 0x0024: マルチターンリミット値 */
    {0x0026  , &infdef_id026                   },/* 0x0026: 出力可能最大トルク　[回転型：N･m, リニア型：N]  */
    {0x0027  , &infdef_id027                   },/* 0x0027: 動作可能モータ最高速度 [回転型：min-1,リニア型：mm/s] */
    {0x0028  , &infdef_id028                   },/* 0x0028: フルクスケールタイプ */
    {0x0029  , &infdef_id029                   },/* 0x0029: 絶対値リニアスケール原点位置設定 */
    {0x002A  , &infdef_id02A                   },/* 0x002A: リニアスケールピッチ[pm] */
    {0x002C  , &infdef_id02C                   },/* 0x002C: リニアスケールピッチ指数[10のべき乗] */
    {0x0030  , &infdef_id030                   },/* 0x0030: サーボパックファームウェアブートバージョン */
    {0x0040  , &infdef_id040                   },/* 0x0040: 指令オプションカード機種ID */
    {0x0041  , &infdef_id041                   },/* 0x0041: 指令オプションカードファームウェアバージョン */
    {0x0043  , &infdef_id043                   },/* 0x0043: 指令オプションカードY仕様番号 */
    {0x0044  , &infdef_id044                   },/* 0x0044: セーフティモジュール機種ID */
    {0x0045  , &infdef_id045                   },/* 0x0045: セーフティモジュールファームウェアバージョン */
    {0x0047  , &infdef_id047                   },/* 0x0047: セーフティモジュールY仕様番号 */
    {0x0048  , &infdef_id048                   },/* 0x0048: フィードバックオプションカード機種ID */
    {0x0049  , &infdef_id049                   },/* 0x0049: フィードバックオプションカードファームウェアバージョン */
    {0x004B  , &infdef_id04B                   },/* 0x004B: フィードバックオプションカードY仕様番号 */
    {0x004C  , &infdef_id04C                   },/* 0x004C: フィードバックオプション内モータID */
    {0x004D  , &infdef_id04D                   },/* 0x004D: フィードバックオプション内モータ容量[W] */
    {0x004F  , &infdef_id04F                   },/* 0x004F: フィードバックオプション内モータ電圧[V] */
    {0x0050  , &infdef_id050                   },/* 0x0050: フィードバックオプション内エンコーダタイプ／分解能 */
    {0x0051  , &infdef_id051                   },/* 0x0051: フィードバックオプション内エンコーダ分解能 */
    {0x0053  , &infdef_id053                   },/* 0x0053: フィードバックオプション内絶対値エンコーダタイプ */
    {0x0054  , &infdef_id054                   },/* 0x0054: フィードバックオプション内エンコーダファームウェアバージョン */
    {0x0100  , &infdef_id100                   },/* 0x0100: レジスタスペックバージョン */
    {0x0800  , &infdef_id800                   },/* 0x0800: 制御モジュール形式文字列 */
    {0x0808  , &infdef_id808                   },/* 0x0808: 制御モジュールシリアルナンバー文字列 */
    {0x0810  , &infdef_id810                   },/* 0x0810: 制御モジュール製造年月 */
    {0x0811  , &infdef_id811                   },/* 0x0811: モータ形式文字列 */
    {0x081C  , &infdef_id81C                   },/* 0x081C: モータシリアルナンバー文字列 */
    {0x0824  , &infdef_id824                   },/* 0x0824: モータ製造年月 */
    {0x0825  , &infdef_id825                   },/* 0x0825: エンコーダ形式領域 */
    {0x082D  , &infdef_id82D                   },/* 0x082D: エンコーダシリアルNo領域 */
    {0x0835  , &infdef_id835                   },/* 0x0835: エンコーダ製造年月領域 */
    {0x0845  , &infdef_id845                   },/* 0x0845: 制御軸数 */
    {0x0860  , &infdef_id860                   },/* 0x0860: サーボパック形式拡張領域 */
    {0x0864  , &infdef_id864                   },/* 0x0864: 指令オプションカード形式文字列 */
    {0x0870  , &infdef_id870                   },/* 0x0870: 指令オプションカードシリアルNo領域 */
    {0x087C  , &infdef_id87C                   },/* 0x087C: 指令オプションカード製造年月領域 */
    {0x087D  , &infdef_id87D                   },/* 0x087D: セーフティモジュール形式文字列 */
    {0x0889  , &infdef_id889                   },/* 0x0889: セーフティモジュールシリアルナンバー文字列 */
    {0x0895  , &infdef_id895                   },/* 0x0895: セーフティモジュール製造年月 */
    {0x0896  , &infdef_id896                   },/* 0x0896: フィードバックオプションカード形式文字列 */
    {0x08A2  , &infdef_id8A2                   },/* 0x08A2: フィードバックオプションカードシリアルナンバー文字列 */
    {0x08AE  , &infdef_id8AE                   },/* 0x08AE: フィードバックオプションカード製造年月 */
    {0x08AF  , &infdef_id8AF                   },/* 0x08AF: フィードバックオプション内モータ形式文字列 */
    {0x08BA  , &infdef_id8BA                   },/* 0x08BA: フィードバックオプション内モータシリアルナンバー文字列 */
    {0x08C2  , &infdef_id8C2                   },/* 0x08C2: フィードバックオプション内モータ製造年月 */
    {0x08C3  , &infdef_id8C3                   },/* 0x08C3: フィードバックオプション内エンコーダ形式文字列 */
    {0x08CB  , &infdef_id8CB                   },/* 0x08CB: フィードバックオプション内エンコーダシリアルナンバー文字列 */
    {0x08D3  , &infdef_id8D3                   },/* 0x08D3: フィードバックオプション内エンコーダ製造年月 */
    {0x08E0  , &infdef_id8E0                   },/* 0x08E0: 入力信号 有効/無効情報 */
    {0x08E2  , &infdef_id8E2                   },/* 0x08E2: 出力信号 有効/無効情報 */
    {0x08E4  , &infdef_id8E4                   },/* 0x08E4: Safety入出力信号 有効/無効情報 */
    {0x08F0  , &infdef_id8F0                   },/* 0x08F0: サーボパックBTO番号 */
    {0x0910  , &infdef_id910                   },/* 0x0910: サーボパック特殊仕様種別 */
    {0xFFFF  , NULL                            },/* 0xFFFF: End of Table List */
};
const ULONG InfoRegTableEntNum = sizeof(InfoRegTable) / sizeof(InfoRegTable[0]);




/******************************* End of File *******************************/

