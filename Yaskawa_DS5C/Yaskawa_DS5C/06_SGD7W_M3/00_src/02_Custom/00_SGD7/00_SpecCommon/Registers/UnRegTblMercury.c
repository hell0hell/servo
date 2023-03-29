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
/*  Attribute of 0x000 : モータ回転/移動速度 */
/***************************************************************************/
const PRM_ATTR_T undef_SpdFbk = {
    0x000,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x001 : 速度指令 */
/***************************************************************************/
const PRM_ATTR_T undef_SpdRef = {
    0x001,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x002 : 内部トルク/推力指令 */
/***************************************************************************/
const PRM_ATTR_T undef_TrqRef = {
    0x002,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x003 : 電気角1(32ビット10進表示) */
/***************************************************************************/
const PRM_ATTR_T undef_MotAngPls = {
    0x003,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotAngle,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x005 : 電気角2(U相0degからの角度) */
/***************************************************************************/
const PRM_ATTR_T undef_MotAngDeg = {
    0x005,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotPhase,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x006 : 入力信号モニタ(dummy) */
/***************************************************************************/
const PRM_ATTR_T undef_InSignal = {
    0x006,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x007 : 出力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_OutSignal = {
    0x007,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x008 : 入力指令パルス速度 */
/***************************************************************************/
const PRM_ATTR_T undef_PosRefSpd = {
    0x008,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x009 : 偏差カウンタ(位置偏差量) */
/***************************************************************************/
static void *undefGetRamPtr_PosErrCntr(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.var.PosErrA.per32s);}

const PRM_ATTR_T undef_PosErrCntr = {
    0x009,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x00B : 累積負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_MotLoad = {
    0x00B,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadIsqrSum,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00C : 回生負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_RegLoad = {
    0x00C,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRegenePower,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00D : ＤＢ抵抗消費電力 */
/***************************************************************************/
const PRM_ATTR_T undef_DbrxPower = {
    0x00D,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDBPower,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00E : 入力指令パルスカウンタ */
/***************************************************************************/
static void *undefGetRamPtr_PcmdCounter(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->UnMonitor->UnPcmdCntr);}

const PRM_ATTR_T undef_PcmdCounter = {
    0x00E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x010 : モータエンコーダ位置 */
/***************************************************************************/
const PRM_ATTR_T undef_PfbkCounter = {
    0x010,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x012 : フルクローズドフィードバックパルスカウンタ */
/***************************************************************************/
const PRM_ATTR_T undef_FposCounter = {
    0x012,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x014 : フルクローズドフィードバック速度 */
/***************************************************************************/
const PRM_ATTR_T undef_FcPfbkSpd = {
    0x014,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x016 : 延べ稼働時間 */
/***************************************************************************/
const PRM_ATTR_T undef_PowerOnTime = {
    0x016,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTimeStamp,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x018 : モータ最大速度上限 */
/***************************************************************************/
static void *undefGetRamPtr_DivMaxSpdX(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->MencV->DivInfo.MaxDivSpd);}

const PRM_ATTR_T undef_DivMaxSpdX = {
    0x018,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_DivMaxSpdX,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x019 : 最高速度時の分周比設定上限値 */
/***************************************************************************/
static void *undefGetRamPtr_DivMaxRatX(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->MencV->DivInfo.MaxDivRat);}

const PRM_ATTR_T undef_DivMaxRatX = {
    0x019,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_DivMaxRatX,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01A : ホールセンサ情報 */
/***************************************************************************/
const PRM_ATTR_T undef_LmPoleSensorX = {
    0x01A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x01B : フィードバックパルスカウンタ */
/***************************************************************************/
const PRM_ATTR_T undef_Pfbk2Counter = {
    0x01B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x01D : 有効ゲインモニタ */
/***************************************************************************/
const PRM_ATTR_T undef_ActGain = {
    0x01D,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadActGain,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01E : セーフティ入出力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyIOSignal = {
    0x01E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x01F : セーフティモジュール外部入出力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyExternalIOSignal = {
    0x01F,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x020 : セーフティモジュール安全機能設定/動作モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySettingMon = {
    0x020,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x021 : セーフティモジュールシステム状態モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySysStatMon = {
    0x021,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x022 : セーフティモジュール安全速度到達時間A */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySpdReachTimeA = {
    0x022,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x024 : セーフティモジュール安全速度到達時間B */
/***************************************************************************/
const PRM_ATTR_T undef_SafetySpdReachTimeB = {
    0x024,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x026 : アクティブモード内部指令速度 */
/***************************************************************************/
const PRM_ATTR_T undef_ActiveModeInterCmdSpd = {
    0x026,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x027 : セーフティモジュールモータ速度 */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMotorSpd = {
    0x027,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x028 : セーフティモジュールモータ位置 */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMotorPos = {
    0x028,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x02A : セーフティモジュール監視速度A */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMonitoringSpdA = {
    0x02A,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02B : セーフティモジュール監視速度B */
/***************************************************************************/
const PRM_ATTR_T undef_SafetyMonitoringSpdB = {
    0x02B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x02C : サーボパック設置環境モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_AmpInstEnvMon = {
    0x02C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x02D : モータ設置環境モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_MotInstEnvMon = {
    0x02D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x02E : 主回路電圧値 */
/***************************************************************************/
const PRM_ATTR_T undef_DcVolt = {
    0x02E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x030 : バックラッシ補正量 */
/***************************************************************************/
static void *undefGetRamPtr_BlshComp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.BlshCmp.var.Filo);}

const PRM_ATTR_T undef_BlshComp = {
    0x030,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x032 : バックラッシ補正量設定制限値 */
/***************************************************************************/
static void *undefGetRamPtr_BlshCompLimit(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.BlshCmp.conf.MaxCompRef);}

const PRM_ATTR_T undef_BlshCompLimit = {
    0x032,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_BlshCompLimit,          /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x034 : 消費電力 */
/***************************************************************************/
static void *undefGetRamPtr_electricityConsumption(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnConsumedPower);}

const PRM_ATTR_T undef_electricityConsumption = {
    0x034,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_electricityConsumption, /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x035 : 消費電力量 */
/***************************************************************************/
static void *undefGetRamPtr_PowerConsumption(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnEnergyConsumption);}

const PRM_ATTR_T undef_PowerConsumption = {
    0x035,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_PowerConsumption,       /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x037 : 累積消費電力量（整数部） */
/***************************************************************************/
static void *undefGetRamPtr_TotalPowerConsumptionConst(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnCumulativeEnergyI);}

const PRM_ATTR_T undef_TotalPowerConsumptionConst = {
    0x037,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_TotalPowerConsumptionConst,/* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x039 : 累積消費電力量（小数部） */
/***************************************************************************/
static void *undefGetRamPtr_TotalPowerConsumptionDecimal(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->PowerMonitor->var.UnCumulativeEnergyD);}

const PRM_ATTR_T undef_TotalPowerConsumptionDecimal = {
    0x039,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x03E : 位置アンプ偏差 */
/***************************************************************************/
static void *undefGetRamPtr_PosErr32s(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->PosManager.var.PosErr);}

const PRM_ATTR_T undef_PosErr32s = {
    0x03E,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x040 : 絶対値エンコーダマルチターンデータ */
/***************************************************************************/
const PRM_ATTR_T undef_MultiTurn = {
    0x040,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x041 : 絶対値エンコーダの1回転内位置 */
/***************************************************************************/
const PRM_ATTR_T undef_IncPulse = {
    0x041,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x043 : 絶対値エンコーダの位置データ（下位） */
/***************************************************************************/
const PRM_ATTR_T undef_ScaleAbsPos_l = {
    0x043,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x045 : 絶対値エンコーダの位置データ（上位） */
/***************************************************************************/
const PRM_ATTR_T undef_ScaleAbsPos_h = {
    0x045,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x046 : モータ定格速度 [回転型：min-1,リニア型：mm/s] */
/***************************************************************************/
const PRM_ATTR_T undef_MotRatSpd = {
    0x046,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorRatedSpeed,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x047 : モータ最大速度 [回転型：min-1,リニア型：mm/s] */
/***************************************************************************/
const PRM_ATTR_T undef_MotMaxSpd = {
    0x047,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorMaxSpeed,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x050 : 内蔵FAN寿命モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_BuiltInFanLifeMon = {
    0x050,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x051 : コンデンサ寿命モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_CondenserLifeMon = {
    0x051,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x052 : 突防回路寿命モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_CollisionProofCircuitLifeMon = {
    0x052,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x053 : DB回路寿命モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_DBCircuitLifeMon = {
    0x053,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x054 : 突防リレー動作回数 */
/***************************************************************************/
const PRM_ATTR_T undef_CollisionProofRelayMotionNum = {
    0x054,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRushSwitchingCount,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x056 : DBリレー動作回数 */
/***************************************************************************/
const PRM_ATTR_T undef_DBRelayMotionNum = {
    0x056,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDBSwitchingCount,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x080 : 磁極検出時正方向最大移動量 */
/***************************************************************************/
const PRM_ATTR_T undef_MpfMovePosP = {
    0x080,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x081 : 磁極検出時逆方向最大移動量 */
/***************************************************************************/
const PRM_ATTR_T undef_MpfMovePosN = {
    0x081,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x082 : 磁極検出所要時間 */
/***************************************************************************/
const PRM_ATTR_T undef_MpfFindingTime = {
    0x082,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x083 : 磁極検出位相オフセット量 */
/***************************************************************************/
const PRM_ATTR_T undef_MpfPhaseOffset = {
    0x083,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x084 : リニアスケールピッチ */
/***************************************************************************/
const PRM_ATTR_T undef_LinearScalePitch = {
    0x084,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadLinearScalePitch,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x086 : リニアスケールピッチ指数 */
/***************************************************************************/
const PRM_ATTR_T undef_LinearScalePitchIndex = {
    0x086,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x087 : 磁極検出判定モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_MpfJudgeMonitor = {
    0x087,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMpfStatus,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x104 : シリアルエンコーダ通信異常回数 */
/***************************************************************************/
const PRM_ATTR_T undef_SencComErrCnt = {
    0x104,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadEncoderErrorCount,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x105 : 位置決め整定時間 */
/***************************************************************************/
const PRM_ATTR_T undef_SettlingTime = {
    0x105,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x106 : 位置決めオーバシュート量 */
/***************************************************************************/
const PRM_ATTR_T undef_OverShoot = {
    0x106,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x107 : 残留振動周波数 */
/***************************************************************************/
const PRM_ATTR_T undef_RvibFrequency = {
    0x107,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x10A : ヒートシンク温度モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_HeatsinkTemp = {
    0x10A,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x10B : 位置決め完了失敗時間積算 */
/***************************************************************************/
const PRM_ATTR_T undef_CoinOffTime = {
    0x10B,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10C : 発振検出用差速度 */
/***************************************************************************/
const PRM_ATTR_T undef_VibDetSpd = {
    0x10C,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10D : 制御基板温度モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_CtrlBoardTemp = {
    0x10D,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x10E : 力センサモニタ */
/***************************************************************************/
const PRM_ATTR_T undef_SifAdCnvData = {
    0x10E,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10F : エンコーダ内気温 */
/***************************************************************************/
static void *undefGetRamPtr_Temperature(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->MencV->Temperature);}

const PRM_ATTR_T undef_Temperature = {
    0x10F,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x110 : 内部信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX = {
    0x110,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x120 : 内部入力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX = {
    0x120,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x130 : 内部出力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX = {
    0x130,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x140 : 主回路母線電圧 */
/***************************************************************************/
const PRM_ATTR_T undef_DcVoltx = {
    0x140,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDCVoltage,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x141 : 電流検出値 */
/***************************************************************************/
const PRM_ATTR_T undef_IqRate = {
    0x141,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadIqRate,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x142 : 累積負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_MotLoad2ms = {
    0x142,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorLoad2ms,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x143 : 回生負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_RegerRate2ms = {
    0x143,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadRegeneLoad2ms,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x144 : ＤＢ抵抗消費電力 */
/***************************************************************************/
const PRM_ATTR_T undef_DbrxPower2ms = {
    0x144,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDbPower2ms,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x145 : 最大累積負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_MotLoad10s = {
    0x145,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxMotorLoad10s,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x146 : 慣性モーメント比/重量比 */
/***************************************************************************/
static void *undefGetRamPtr_JLRatio(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->JatHdl.EstimatJrat);}

const PRM_ATTR_T undef_JLRatio = {
    0x146,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_JLRatio,                /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x148 : T-REFモニタ */
/***************************************************************************/
const PRM_ATTR_T undef_TrefAdMon = {
    0x148,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x149 : V-REFモニタ */
/***************************************************************************/
const PRM_ATTR_T undef_VrefAdMon = {
    0x149,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x14A : 入力指令パルス周波数 */
/***************************************************************************/
const PRM_ATTR_T undef_DPulseHwCntMon = {
    0x14A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x14C : d軸電流指令モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_IdrefRate = {
    0x14C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x14D : MEMOBUSエラー情報（エラー発生アドレス） */
/***************************************************************************/
const PRM_ATTR_T undef_MbusErrReg = {
    0x14D,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0xffff,                                 /* Upper Limit */
    0xffff,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMemobusErrorInfo,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x150 : 基板ID */
/***************************************************************************/
const PRM_ATTR_T undef_SubstrateID = {
    0x150,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x170 : 平均内部温度 */
/***************************************************************************/
const PRM_ATTR_T undef_AvgIntTmpr = {
    0x170,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x171 : 制御基板温度 */
/***************************************************************************/
static void *undefGetRamPtr_ControlSubstrateTemp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->OverHeat->UnMon.UnCtrlBoadTemp);}

const PRM_ATTR_T undef_ControlSubstrateTemp = {
    0x171,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x172 : パワー基板温度 */
/***************************************************************************/
static void *undefGetRamPtr_PowerSubstrateTemp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->OverHeat->UnMon.UnPowerBoadTemp);}

const PRM_ATTR_T undef_PowerSubstrateTemp = {
    0x172,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x200 : エンコーダ内気温温度モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_MencTemp = {
    0x200,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x201 : ROMチェックサム値（システムレベル） */
/***************************************************************************/
const PRM_ATTR_T undef_RomChkSumVal = {
    0x201,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x203 : 設定異常パラメータ番号(A.040) */
/***************************************************************************/
static void *undefGetRamPtr_ErrPrmNo(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AlmManager->PrmErrInfo.ErrPrmNo);}

const PRM_ATTR_T undef_ErrPrmNo = {
    0x203,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x204 : リミット処理パラメータ番号(NoAlarm) */
/***************************************************************************/
const PRM_ATTR_T undef_LmtPrmNo = {
    0x204,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x205 : サーボパックブートROMチェックサム値（システムレベル） */
/***************************************************************************/
const PRM_ATTR_T undef_SvPacBootRomChkSumVal = {
    0x205,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x206 : インターフェースモジュールブートROMチェックサム値（システムレベル） */
/***************************************************************************/
const PRM_ATTR_T undef_IfBootRomChkSumVal = {
    0x206,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x210 : ユーザパラメータCRC計算値 */
/***************************************************************************/
const PRM_ATTR_T undef_PrmCrcValue = {
    0x210,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadUsrPrmCrcValue,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x300 : 2msあたりのCPU負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeS = {
    0x300,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x302 : 2msあたりのCPU最大負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeS = {
    0x302,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x304 : ScanB現在処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeB = {
    0x304,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x306 : ScanB最大処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeB = {
    0x306,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x308 : ScanC現在処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeC = {
    0x308,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x30A : ScanC最大処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeC = {
    0x30A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x310 : μプログラムIntAD現在処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeMI = {
    0x310,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x312 : μプログラムIntAD最大処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeMI = {
    0x312,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x314 : μプログラムScanA現在処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeMA = {
    0x314,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x316 : μプログラムScanA最大処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeMA = {
    0x316,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x318 : μプログラムRound現在処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_CurExeTimeMR = {
    0x318,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x31A : μプログラムRound最大処理時間 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxExeTimeMR = {
    0x31A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x500 : アラーム履歴データ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTrace = {
    0x500,                                  /* Register Number */
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
    10,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlarmHistory,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50A : 現在のアラーム状態 */
/***************************************************************************/
static void *undefGetRamPtr_AlmCode(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->AlmManager)->AlmCode);}

const PRM_ATTR_T undef_AlmCode = {
    0x50A,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_AlmCode,                /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x50B : 速度指令調整用データ */
/***************************************************************************/
const PRM_ATTR_T undef_SpdRefAdj = {
    0x50B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x50C : トルク指令調整用データ（正側） */
/***************************************************************************/
const PRM_ATTR_T undef_TrqRefAdjp = {
    0x50C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x50D : アナログモニタ(CH1)　零調用データ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh1Zadj = {
    0x50D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x50E : アナログモニタ(CH2)　零調用データ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh2Zadj = {
    0x50E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x50F : アナログモニタ（CH1）ゲイン調データ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh1Gadj = {
    0x50F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x510 : アナログモニタ（CH2）ゲイン調データ */
/***************************************************************************/
const PRM_ATTR_T undef_AmonCh2Gadj = {
    0x510,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x511 : 電流検出（U相）零調用データ */
/***************************************************************************/
const PRM_ATTR_T undef_CurUphZadj = {
    0x511,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x512 : 電流検出（V相）零調用データ */
/***************************************************************************/
const PRM_ATTR_T undef_CurVphZadj = {
    0x512,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x513 : モータタイプ */
/***************************************************************************/
const PRM_ATTR_T undef_MotorType = {
    0x513,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorID,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x514 : モータ容量 */
/***************************************************************************/
const PRM_ATTR_T undef_MotorCapacity = {
    0x514,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMotorWattage,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x515 : エンコーダタイプ */
/***************************************************************************/
const PRM_ATTR_T undef_EncoderType = {
    0x515,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadEncoderType,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x516 : Y仕様 */
/***************************************************************************/
const PRM_ATTR_T undef_Yspecification = {
    0x516,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadYspecVer,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x517 : サーボパックソフトバージョン */
/***************************************************************************/
const PRM_ATTR_T undef_SvPacSoftVer = {
    0x517,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFirmVerBase,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x518 : エンコーダソフトバージョン */
/***************************************************************************/
const PRM_ATTR_T undef_EncoderSoftVer = {
    0x518,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadEncoderVer,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x519 : フラッシュ書き込み回数（システムレベル） */
/***************************************************************************/
const PRM_ATTR_T undef_FlashWriteNum = {
    0x519,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x51A : オプションカード検出情報 */
/***************************************************************************/
static void *undefGetRamPtr_OpDetInfo(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->OptCardHdl->OpDetInfo);}

const PRM_ATTR_T undef_OpDetInfo = {
    0x51A,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &undefGetRamPtr_OpDetInfo,              /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x51B : サーボステータスモニタ */
/***************************************************************************/
const PRM_ATTR_T undef_SvStatus = {
    0x51B,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSvStatus,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x51C : 制御モードモニタ */
/***************************************************************************/
const PRM_ATTR_T undef_CtrlMode = {
    0x51C,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadServoControlMode,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x52A : アラーム履歴タイムスタンプ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTmStamp = {
    0x52A,                                  /* Register Number */
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
/*  Attribute of 0x53E : 異常トレースバックカウンタ0 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter0 = {
    0x53E,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x53F : 異常トレースバックカウンタ1 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter1 = {
    0x53F,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x540 : 異常トレースバックカウンタ2 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter2 = {
    0x540,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x541 : 異常トレースバックカウンタ3 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter3 = {
    0x541,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x542 : 異常トレースバックカウンタ4 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter4 = {
    0x542,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x543 : 異常トレースバックカウンタ5 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter5 = {
    0x543,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x544 : 異常トレースバックカウンタ6 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter6 = {
    0x544,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x545 : 異常トレースバックカウンタ7 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter7 = {
    0x545,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x546 : 異常トレースバックカウンタ8 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter8 = {
    0x546,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x547 : 異常トレースバックカウンタ9 */
/***************************************************************************/
const PRM_ATTR_T undef_AbnormalityTraceBackCounter9 = {
    0x547,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x548 : アラームトレースデータアドレス0 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd0 = {
    0x548,                                  /* Register Number */
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
/*  Attribute of 0x549 : アラームトレースデータアドレス1 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd1 = {
    0x549,                                  /* Register Number */
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
/*  Attribute of 0x54A : アラームトレースデータアドレス2 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd2 = {
    0x54A,                                  /* Register Number */
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
/*  Attribute of 0x54B : アラームトレースデータアドレス3 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd3 = {
    0x54B,                                  /* Register Number */
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
/*  Attribute of 0x54C : アラームトレースデータアドレス4 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd4 = {
    0x54C,                                  /* Register Number */
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
/*  Attribute of 0x54D : アラームトレースデータアドレス5 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd5 = {
    0x54D,                                  /* Register Number */
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
/*  Attribute of 0x54E : アラームトレースデータアドレス6 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd6 = {
    0x54E,                                  /* Register Number */
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
/*  Attribute of 0x54F : アラームトレースデータアドレス7 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceDataAdd7 = {
    0x54F,                                  /* Register Number */
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
/*  Attribute of 0x558 : アラームトレース状態 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTraceStat = {
    0x558,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadDummy,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x700 : サンプリング時間の単位 */
/***************************************************************************/
const PRM_ATTR_T undef_SamplingTime = {
    0x700,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSamplingTimeUnit,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x701 : データが16384の時のトルク */
/***************************************************************************/
const PRM_ATTR_T undef_MaxTorque = {
    0x701,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxTorqueConvertValue,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x702 : データが16384の時の速度 */
/***************************************************************************/
const PRM_ATTR_T undef_MaxSpeed = {
    0x702,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxSpeedConvertValue,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x704 : データが16384の時の速度の指数 */
/***************************************************************************/
const PRM_ATTR_T undef_Dummy = {
    0x704,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMaxSpeedExpConvertValue,         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x705 : 内部動作状態 */
/***************************************************************************/
const PRM_ATTR_T undef_NewServoOpmode = {
    0x705,                                  /* Register Number */
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
/*  Attribute of 0x710 : DSW/RSW入力状態 */
/***************************************************************************/
const PRM_ATTR_T undef_SWInputsStatus = {
    0x710,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x712 : FB基板情報 */
/***************************************************************************/
const PRM_ATTR_T undef_FBA_BoardID = {
    0x712,                                  /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadFBboardID,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x713 : MB基板情報 */
/***************************************************************************/
const PRM_ATTR_T undef_MB_BoardID = {
    0x713,                                  /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadMBboardID,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x714 : コンバータ負荷電流(AD値) */
/***************************************************************************/
const PRM_ATTR_T undef_CnvCurrentAD = {
    0x714,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x716 : コンバータシリアル入力データ */
/***************************************************************************/
const PRM_ATTR_T undef_CnvSerialInputs = {
    0x716,                                  /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadConverterSerialInputs,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x718 : ユニット内部温度モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_MbTemperature = {
    0x718,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x719 : コンバータ温度モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_CnvTemperature = {
    0x719,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x71A : 制御基板温度モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_IfTemperature = {
    0x71A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x720 : (R)ブートソフトウェアバージョン */
/***************************************************************************/
const PRM_ATTR_T undef_R_BootsoftVersion = {
    0x720,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x724 : (R)ブートストラップバージョン */
/***************************************************************************/
const PRM_ATTR_T undef_R_BootstrapVersion = {
    0x724,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
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
/*  Attribute of 0x800 : モータ回転/移動速度 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSpdFbkX = {
    0x800,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x801 : 速度指令 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSpdRefX = {
    0x801,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x802 : 内部トルク/推力指令 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtTrqRefX = {
    0x802,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x803 : 入力指令パルス速度 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtPosRefSpdX = {
    0x803,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x804 : 偏差カウンタ(位置偏差量) */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtPosErrCntrX = {
    0x804,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x810 : 内部信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtInterSignalX = {
    0x810,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
/*  Attribute of 0x814 : 内部入力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtIntInSignalX = {
    0x814,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
/*  Attribute of 0x818 : 内部出力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtIntOutSignalX = {
    0x818,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
/*  Attribute of 0x820 : 主回路母線電圧 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtDcVoltxX = {
    0x820,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x821 : 電流検出値 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtIqRateX = {
    0x821,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x822 : 累積負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtMotLoad2msX = {
    0x822,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMotLoad2ms,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x823 : 回生負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtRegerRate2msX = {
    0x823,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtRegerRate2ms,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x824 : ＤＢ抵抗消費電力 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtDbrxPower2msX = {
    0x824,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtDbrxPower2ms,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x825 : 最大累積負荷率 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtMotLoad10sX = {
    0x825,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtMotLoad10s,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x826 : 慣性モーメント比/重量比 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtJLRatioX = {
    0x826,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtJLRatio,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x830 : シリアルエンコーダ通信異常回数 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSencComErrCntX = {
    0x830,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtSencComErrCnt,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x831 : MECHATROLINK通信異常回数 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtM2ComErrCntX = {
    0x831,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtM2ComErrCnt,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x832 : セーフティ入出力信号モニタ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtSafetyIOSignalX = {
    0x832,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0x833 : アラーム番号 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtAlmCodeX = {
    0x833,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadAlmLtAlmCode,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x834 : タイムスタンプ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmLtTmStampX = {
    0x834,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level */
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
/*  Attribute of 0x900 : トレースアラーム番号 */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTrcAlmCode = {
    0x900,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x901 : トレースアラームタイムスタンプ */
/***************************************************************************/
const PRM_ATTR_T undef_AlmTrcTmStamp = {
    0x901,                                  /* Register Number */
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
/*  Attribute of 0xC00 : フィードバック位置(APOS) */
/***************************************************************************/
const PRM_ATTR_T undef_Apos = {
    0xC00,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC02 : 現在指令位置(CPOS) */
/***************************************************************************/
const PRM_ATTR_T undef_Cpos = {
    0xC02,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC04 : 位置偏差(PERR) */
/***************************************************************************/
const PRM_ATTR_T undef_Perr = {
    0xC04,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC06 : 目標位置(TPOS) */
/***************************************************************************/
const PRM_ATTR_T undef_Tpos = {
    0xC06,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC08 : ラッチ位置１(LPOS1) */
/***************************************************************************/
const PRM_ATTR_T undef_Lpos1 = {
    0xC08,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC0A : ラッチ位置２(LPOS2) */
/***************************************************************************/
const PRM_ATTR_T undef_Lpos2 = {
    0xC0A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC0C : ラッチ位置３(LPOS3) */
/***************************************************************************/
const PRM_ATTR_T undef_Lpos3 = {
    0xC0C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC0E : 目標速度(TSPD) */
/***************************************************************************/
const PRM_ATTR_T undef_Tspd = {
    0xC0E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC10 : フィードバック速度(FSPD) */
/***************************************************************************/
const PRM_ATTR_T undef_Fspd = {
    0xC10,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC12 : 現在位置指令速度(CSPD) */
/***************************************************************************/
const PRM_ATTR_T undef_Cspd = {
    0xC12,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xC14 : トルク制限値(TRQ_LIM) */
/***************************************************************************/
const PRM_ATTR_T undef_TrqLim = {
    0xC14,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadTrqLim,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC16 : 速度制限値(SPD_LIM) */
/***************************************************************************/
const PRM_ATTR_T undef_SpdLim = {
    0xC16,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadSpdLim,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC18 : M-Ⅲ通信フェーズ */
/***************************************************************************/
const PRM_ATTR_T undef_M3ComPhase = {
    0xC18,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadM3CommPhase,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC19 : M-Ⅲ現在実行コマンドコード(RCMD) */
/***************************************************************************/
const PRM_ATTR_T undef_M3Rcmd = {
    0xC19,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadM3MainCommandCode,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC1A : M-Ⅲ現在実行サブコマンドコード(SUB_RCMD) */
/***************************************************************************/
const PRM_ATTR_T undef_M3SubRcmd = {
    0xC1A,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &UnReadM3SubCommandCode,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xF000 : 全入力信号モニタ1(for LCD Operator) */
/***************************************************************************/
const PRM_ATTR_T undef_AllInSignal1 = {
    0xF000,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF001 : 全入力信号モニタ2(for LCD Operator) */
/***************************************************************************/
const PRM_ATTR_T undef_AllInSignal2 = {
    0xF001,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF002 : 全出力信号モニタ(for LCD Operator) */
/***************************************************************************/
const PRM_ATTR_T undef_AllOutSignal = {
    0xF002,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF003 : 内部信号モニタ1 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX1 = {
    0xF003,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF004 : 内部信号モニタ2 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX2 = {
    0xF004,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF005 : 内部信号モニタ3 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX3 = {
    0xF005,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF006 : 内部信号モニタ4 */
/***************************************************************************/
const PRM_ATTR_T undef_InterSignalX4 = {
    0xF006,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF007 : 内部入力信号モニタ 1 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX1 = {
    0xF007,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF008 : 内部入力信号モニタ 2 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX2 = {
    0xF008,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF009 : 内部入力信号モニタ 3 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX3 = {
    0xF009,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF00A : 内部入力信号モニタ 4 */
/***************************************************************************/
const PRM_ATTR_T undef_IntInSignalX4 = {
    0xF00A,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF00B : 内部出力信号モニタ1 */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX1 = {
    0xF00B,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF00C : 内部出力信号モニタ2 */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX2 = {
    0xF00C,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF00D : 内部出力信号モニタ3 */
/***************************************************************************/
const PRM_ATTR_T undef_IntOutSignalX3 = {
    0xF00D,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
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
/*  Attribute of 0xF00E : モータ最大速度上限/分周比設定上限値 */
/***************************************************************************/
const PRM_ATTR_T undef_SelSpdOrDiv = {
    0xF00E,                                 /* Register Number */
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
    {0x000   , &undef_SpdFbk                        },/* 0x000: モータ回転/移動速度 */
    {0x001   , &undef_SpdRef                        },/* 0x001: 速度指令 */
    {0x002   , &undef_TrqRef                        },/* 0x002: 内部トルク/推力指令 */
    {0x003   , &undef_MotAngPls                     },/* 0x003: 電気角1(32ビット10進表示) */
    {0x005   , &undef_MotAngDeg                     },/* 0x005: 電気角2(U相0degからの角度) */
    {0x006   , &undef_InSignal                      },/* 0x006: 入力信号モニタ(dummy) */
    {0x007   , &undef_OutSignal                     },/* 0x007: 出力信号モニタ */
    {0x008   , &undef_PosRefSpd                     },/* 0x008: 入力指令パルス速度 */
    {0x009   , &undef_PosErrCntr                    },/* 0x009: 偏差カウンタ(位置偏差量) */
    {0x00B   , &undef_MotLoad                       },/* 0x00B: 累積負荷率 */
    {0x00C   , &undef_RegLoad                       },/* 0x00C: 回生負荷率 */
    {0x00D   , &undef_DbrxPower                     },/* 0x00D: ＤＢ抵抗消費電力 */
    {0x00E   , &undef_PcmdCounter                   },/* 0x00E: 入力指令パルスカウンタ */
    {0x010   , &undef_PfbkCounter                   },/* 0x010: モータエンコーダ位置 */
    {0x012   , &undef_FposCounter                   },/* 0x012: フルクローズドフィードバックパルスカウンタ */
    {0x014   , &undef_FcPfbkSpd                     },/* 0x014: フルクローズドフィードバック速度 */
    {0x016   , &undef_PowerOnTime                   },/* 0x016: 延べ稼働時間 */
    {0x018   , &undef_DivMaxSpdX                    },/* 0x018: モータ最大速度上限 */
    {0x019   , &undef_DivMaxRatX                    },/* 0x019: 最高速度時の分周比設定上限値 */
    {0x01A   , &undef_LmPoleSensorX                 },/* 0x01A: ホールセンサ情報 */
    {0x01B   , &undef_Pfbk2Counter                  },/* 0x01B: フィードバックパルスカウンタ */
    {0x01D   , &undef_ActGain                       },/* 0x01D: 有効ゲインモニタ */
    {0x01E   , &undef_SafetyIOSignal                },/* 0x01E: セーフティ入出力信号モニタ */
    {0x01F   , &undef_SafetyExternalIOSignal        },/* 0x01F: セーフティモジュール外部入出力信号モニタ */
    {0x020   , &undef_SafetySettingMon              },/* 0x020: セーフティモジュール安全機能設定/動作モニタ */
    {0x021   , &undef_SafetySysStatMon              },/* 0x021: セーフティモジュールシステム状態モニタ */
    {0x022   , &undef_SafetySpdReachTimeA           },/* 0x022: セーフティモジュール安全速度到達時間A */
    {0x024   , &undef_SafetySpdReachTimeB           },/* 0x024: セーフティモジュール安全速度到達時間B */
    {0x026   , &undef_ActiveModeInterCmdSpd         },/* 0x026: アクティブモード内部指令速度 */
    {0x027   , &undef_SafetyMotorSpd                },/* 0x027: セーフティモジュールモータ速度 */
    {0x028   , &undef_SafetyMotorPos                },/* 0x028: セーフティモジュールモータ位置 */
    {0x02A   , &undef_SafetyMonitoringSpdA          },/* 0x02A: セーフティモジュール監視速度A */
    {0x02B   , &undef_SafetyMonitoringSpdB          },/* 0x02B: セーフティモジュール監視速度B */
    {0x02C   , &undef_AmpInstEnvMon                 },/* 0x02C: サーボパック設置環境モニタ */
    {0x02D   , &undef_MotInstEnvMon                 },/* 0x02D: モータ設置環境モニタ */
    {0x02E   , &undef_DcVolt                        },/* 0x02E: 主回路電圧値 */
    {0x030   , &undef_BlshComp                      },/* 0x030: バックラッシ補正量 */
    {0x032   , &undef_BlshCompLimit                 },/* 0x032: バックラッシ補正量設定制限値 */
    {0x034   , &undef_electricityConsumption        },/* 0x034: 消費電力 */
    {0x035   , &undef_PowerConsumption              },/* 0x035: 消費電力量 */
    {0x037   , &undef_TotalPowerConsumptionConst    },/* 0x037: 累積消費電力量（整数部） */
    {0x039   , &undef_TotalPowerConsumptionDecimal  },/* 0x039: 累積消費電力量（小数部） */
    {0x03E   , &undef_PosErr32s                     },/* 0x03E: 位置アンプ偏差 */
    {0x040   , &undef_MultiTurn                     },/* 0x040: 絶対値エンコーダマルチターンデータ */
    {0x041   , &undef_IncPulse                      },/* 0x041: 絶対値エンコーダの1回転内位置 */
    {0x043   , &undef_ScaleAbsPos_l                 },/* 0x043: 絶対値エンコーダの位置データ（下位） */
    {0x045   , &undef_ScaleAbsPos_h                 },/* 0x045: 絶対値エンコーダの位置データ（上位） */
    {0x046   , &undef_MotRatSpd                     },/* 0x046: モータ定格速度 [回転型：min-1,リニア型：mm/s] */
    {0x047   , &undef_MotMaxSpd                     },/* 0x047: モータ最大速度 [回転型：min-1,リニア型：mm/s] */
    {0x050   , &undef_BuiltInFanLifeMon             },/* 0x050: 内蔵FAN寿命モニタ */
    {0x051   , &undef_CondenserLifeMon              },/* 0x051: コンデンサ寿命モニタ */
    {0x052   , &undef_CollisionProofCircuitLifeMon  },/* 0x052: 突防回路寿命モニタ */
    {0x053   , &undef_DBCircuitLifeMon              },/* 0x053: DB回路寿命モニタ */
    {0x054   , &undef_CollisionProofRelayMotionNum  },/* 0x054: 突防リレー動作回数 */
    {0x056   , &undef_DBRelayMotionNum              },/* 0x056: DBリレー動作回数 */
    {0x080   , &undef_MpfMovePosP                   },/* 0x080: 磁極検出時正方向最大移動量 */
    {0x081   , &undef_MpfMovePosN                   },/* 0x081: 磁極検出時逆方向最大移動量 */
    {0x082   , &undef_MpfFindingTime                },/* 0x082: 磁極検出所要時間 */
    {0x083   , &undef_MpfPhaseOffset                },/* 0x083: 磁極検出位相オフセット量 */
    {0x084   , &undef_LinearScalePitch              },/* 0x084: リニアスケールピッチ */
    {0x086   , &undef_LinearScalePitchIndex         },/* 0x086: リニアスケールピッチ指数 */
    {0x087   , &undef_MpfJudgeMonitor               },/* 0x087: 磁極検出判定モニタ */
    {0x104   , &undef_SencComErrCnt                 },/* 0x104: シリアルエンコーダ通信異常回数 */
    {0x105   , &undef_SettlingTime                  },/* 0x105: 位置決め整定時間 */
    {0x106   , &undef_OverShoot                     },/* 0x106: 位置決めオーバシュート量 */
    {0x107   , &undef_RvibFrequency                 },/* 0x107: 残留振動周波数 */
    {0x10A   , &undef_HeatsinkTemp                  },/* 0x10A: ヒートシンク温度モニタ */
    {0x10B   , &undef_CoinOffTime                   },/* 0x10B: 位置決め完了失敗時間積算 */
    {0x10C   , &undef_VibDetSpd                     },/* 0x10C: 発振検出用差速度 */
    {0x10D   , &undef_CtrlBoardTemp                 },/* 0x10D: 制御基板温度モニタ */
    {0x10E   , &undef_SifAdCnvData                  },/* 0x10E: 力センサモニタ */
    {0x10F   , &undef_Temperature                   },/* 0x10F: エンコーダ内気温 */
    {0x110   , &undef_InterSignalX                  },/* 0x110: 内部信号モニタ */
    {0x120   , &undef_IntInSignalX                  },/* 0x120: 内部入力信号モニタ */
    {0x130   , &undef_IntOutSignalX                 },/* 0x130: 内部出力信号モニタ */
    {0x140   , &undef_DcVoltx                       },/* 0x140: 主回路母線電圧 */
    {0x141   , &undef_IqRate                        },/* 0x141: 電流検出値 */
    {0x142   , &undef_MotLoad2ms                    },/* 0x142: 累積負荷率 */
    {0x143   , &undef_RegerRate2ms                  },/* 0x143: 回生負荷率 */
    {0x144   , &undef_DbrxPower2ms                  },/* 0x144: ＤＢ抵抗消費電力 */
    {0x145   , &undef_MotLoad10s                    },/* 0x145: 最大累積負荷率 */
    {0x146   , &undef_JLRatio                       },/* 0x146: 慣性モーメント比/重量比 */
    {0x148   , &undef_TrefAdMon                     },/* 0x148: T-REFモニタ */
    {0x149   , &undef_VrefAdMon                     },/* 0x149: V-REFモニタ */
    {0x14A   , &undef_DPulseHwCntMon                },/* 0x14A: 入力指令パルス周波数 */
    {0x14C   , &undef_IdrefRate                     },/* 0x14C: d軸電流指令モニタ */
    {0x14D   , &undef_MbusErrReg                    },/* 0x14D: MEMOBUSエラー情報（エラー発生アドレス） */
    {0x150   , &undef_SubstrateID                   },/* 0x150: 基板ID */
    {0x170   , &undef_AvgIntTmpr                    },/* 0x170: 平均内部温度 */
    {0x171   , &undef_ControlSubstrateTemp          },/* 0x171: 制御基板温度 */
    {0x172   , &undef_PowerSubstrateTemp            },/* 0x172: パワー基板温度 */
    {0x200   , &undef_MencTemp                      },/* 0x200: エンコーダ内気温温度モニタ */
    {0x201   , &undef_RomChkSumVal                  },/* 0x201: ROMチェックサム値（システムレベル） */
    {0x202   , &undef_MbusErrReg                    },/* 0x202: MEMOBUSエラー情報（エラー発生アドレス） */
    {0x203   , &undef_ErrPrmNo                      },/* 0x203: 設定異常パラメータ番号(A.040) */
    {0x204   , &undef_LmtPrmNo                      },/* 0x204: リミット処理パラメータ番号(NoAlarm) */
    {0x205   , &undef_SvPacBootRomChkSumVal         },/* 0x205: サーボパックブートROMチェックサム値（システムレベル） */
    {0x206   , &undef_IfBootRomChkSumVal            },/* 0x206: インターフェースモジュールブートROMチェックサム値（システムレベル） */
    {0x210   , &undef_PrmCrcValue                   },/* 0x210: ユーザパラメータCRC計算値 */
    {0x300   , &undef_CurExeTimeS                   },/* 0x300: 2msあたりのCPU負荷率 */
    {0x302   , &undef_MaxExeTimeS                   },/* 0x302: 2msあたりのCPU最大負荷率 */
    {0x304   , &undef_CurExeTimeB                   },/* 0x304: ScanB現在処理時間 */
    {0x306   , &undef_MaxExeTimeB                   },/* 0x306: ScanB最大処理時間 */
    {0x308   , &undef_CurExeTimeC                   },/* 0x308: ScanC現在処理時間 */
    {0x30A   , &undef_MaxExeTimeC                   },/* 0x30A: ScanC最大処理時間 */
    {0x310   , &undef_CurExeTimeMI                  },/* 0x310: μプログラムIntAD現在処理時間 */
    {0x312   , &undef_MaxExeTimeMI                  },/* 0x312: μプログラムIntAD最大処理時間 */
    {0x314   , &undef_CurExeTimeMA                  },/* 0x314: μプログラムScanA現在処理時間 */
    {0x316   , &undef_MaxExeTimeMA                  },/* 0x316: μプログラムScanA最大処理時間 */
    {0x318   , &undef_CurExeTimeMR                  },/* 0x318: μプログラムRound現在処理時間 */
    {0x31A   , &undef_MaxExeTimeMR                  },/* 0x31A: μプログラムRound最大処理時間 */
    {0x500   , &undef_AlmTrace                      },/* 0x500: アラーム履歴データ */
    {0x50A   , &undef_AlmCode                       },/* 0x50A: 現在のアラーム状態 */
    {0x50B   , &undef_SpdRefAdj                     },/* 0x50B: 速度指令調整用データ */
    {0x50C   , &undef_TrqRefAdjp                    },/* 0x50C: トルク指令調整用データ（正側） */
    {0x50D   , &undef_AmonCh1Zadj                   },/* 0x50D: アナログモニタ(CH1)　零調用データ */
    {0x50E   , &undef_AmonCh2Zadj                   },/* 0x50E: アナログモニタ(CH2)　零調用データ */
    {0x50F   , &undef_AmonCh1Gadj                   },/* 0x50F: アナログモニタ（CH1）ゲイン調データ */
    {0x510   , &undef_AmonCh2Gadj                   },/* 0x510: アナログモニタ（CH2）ゲイン調データ */
    {0x511   , &undef_CurUphZadj                    },/* 0x511: 電流検出（U相）零調用データ */
    {0x512   , &undef_CurVphZadj                    },/* 0x512: 電流検出（V相）零調用データ */
    {0x513   , &undef_MotorType                     },/* 0x513: モータタイプ */
    {0x514   , &undef_MotorCapacity                 },/* 0x514: モータ容量 */
    {0x515   , &undef_EncoderType                   },/* 0x515: エンコーダタイプ */
    {0x516   , &undef_Yspecification                },/* 0x516: Y仕様 */
    {0x517   , &undef_SvPacSoftVer                  },/* 0x517: サーボパックソフトバージョン */
    {0x518   , &undef_EncoderSoftVer                },/* 0x518: エンコーダソフトバージョン */
    {0x519   , &undef_FlashWriteNum                 },/* 0x519: フラッシュ書き込み回数（システムレベル） */
    {0x51A   , &undef_OpDetInfo                     },/* 0x51A: オプションカード検出情報 */
    {0x51B   , &undef_SvStatus                      },/* 0x51B: サーボステータスモニタ */
    {0x51C   , &undef_CtrlMode                      },/* 0x51C: 制御モードモニタ */
    {0x52A   , &undef_AlmTmStamp                    },/* 0x52A: アラーム履歴タイムスタンプ */
    {0x53E   , &undef_AbnormalityTraceBackCounter0  },/* 0x53E: 異常トレースバックカウンタ0 */
    {0x53F   , &undef_AbnormalityTraceBackCounter1  },/* 0x53F: 異常トレースバックカウンタ1 */
    {0x540   , &undef_AbnormalityTraceBackCounter2  },/* 0x540: 異常トレースバックカウンタ2 */
    {0x541   , &undef_AbnormalityTraceBackCounter3  },/* 0x541: 異常トレースバックカウンタ3 */
    {0x542   , &undef_AbnormalityTraceBackCounter4  },/* 0x542: 異常トレースバックカウンタ4 */
    {0x543   , &undef_AbnormalityTraceBackCounter5  },/* 0x543: 異常トレースバックカウンタ5 */
    {0x544   , &undef_AbnormalityTraceBackCounter6  },/* 0x544: 異常トレースバックカウンタ6 */
    {0x545   , &undef_AbnormalityTraceBackCounter7  },/* 0x545: 異常トレースバックカウンタ7 */
    {0x546   , &undef_AbnormalityTraceBackCounter8  },/* 0x546: 異常トレースバックカウンタ8 */
    {0x547   , &undef_AbnormalityTraceBackCounter9  },/* 0x547: 異常トレースバックカウンタ9 */
    {0x548   , &undef_AlmTraceDataAdd0              },/* 0x548: アラームトレースデータアドレス0 */
    {0x549   , &undef_AlmTraceDataAdd1              },/* 0x549: アラームトレースデータアドレス1 */
    {0x54A   , &undef_AlmTraceDataAdd2              },/* 0x54A: アラームトレースデータアドレス2 */
    {0x54B   , &undef_AlmTraceDataAdd3              },/* 0x54B: アラームトレースデータアドレス3 */
    {0x54C   , &undef_AlmTraceDataAdd4              },/* 0x54C: アラームトレースデータアドレス4 */
    {0x54D   , &undef_AlmTraceDataAdd5              },/* 0x54D: アラームトレースデータアドレス5 */
    {0x54E   , &undef_AlmTraceDataAdd6              },/* 0x54E: アラームトレースデータアドレス6 */
    {0x54F   , &undef_AlmTraceDataAdd7              },/* 0x54F: アラームトレースデータアドレス7 */
    {0x558   , &undef_AlmTraceStat                  },/* 0x558: アラームトレース状態 */
    {0x600   , &undef_MultiTurn                     },/* 0x600: 絶対値エンコーダ多回転量 */
    {0x601   , &undef_IncPulse                      },/* 0x601: 絶対値エンコーダ1回転内パルス */
    {0x603   , &undef_ScaleAbsPos_l                 },/* 0x603: 絶対値スケール位置（下位） */
    {0x605   , &undef_ScaleAbsPos_h                 },/* 0x605: 絶対値スケール位置（上位） */
    {0x700   , &undef_SamplingTime                  },/* 0x700: サンプリング時間の単位 */
    {0x701   , &undef_MaxTorque                     },/* 0x701: データが16384の時のトルク */
    {0x702   , &undef_MaxSpeed                      },/* 0x702: データが16384の時の速度 */
    {0x704   , &undef_Dummy                         },/* 0x704: データが16384の時の速度の指数 */
    {0x705   , &undef_NewServoOpmode                },/* 0x705: 内部動作状態 */
    {0x710   , &undef_SWInputsStatus                },/* 0x710: DSW/RSW入力状態 */
    {0x712   , &undef_FBA_BoardID                   },/* 0x712: FB基板情報 */
    {0x713   , &undef_MB_BoardID                    },/* 0x713: MB基板情報 */
    {0x714   , &undef_CnvCurrentAD                  },/* 0x714: コンバータ負荷電流(AD値) */
    {0x716   , &undef_CnvSerialInputs               },/* 0x716: コンバータシリアル入力データ */
    {0x718   , &undef_MbTemperature                 },/* 0x718: ユニット内部温度モニタ */
    {0x719   , &undef_CnvTemperature                },/* 0x719: コンバータ温度モニタ */
    {0x71A   , &undef_IfTemperature                 },/* 0x71A: 制御基板温度モニタ */
    {0x720   , &undef_R_BootsoftVersion             },/* 0x720: (R)ブートソフトウェアバージョン */
    {0x724   , &undef_R_BootstrapVersion            },/* 0x724: (R)ブートストラップバージョン */
    {0x800   , &undef_AlmLtSpdFbkX                  },/* 0x800: モータ回転/移動速度 */
    {0x801   , &undef_AlmLtSpdRefX                  },/* 0x801: 速度指令 */
    {0x802   , &undef_AlmLtTrqRefX                  },/* 0x802: 内部トルク/推力指令 */
    {0x803   , &undef_AlmLtPosRefSpdX               },/* 0x803: 入力指令パルス速度 */
    {0x804   , &undef_AlmLtPosErrCntrX              },/* 0x804: 偏差カウンタ(位置偏差量) */
    {0x810   , &undef_AlmLtInterSignalX             },/* 0x810: 内部信号モニタ */
    {0x814   , &undef_AlmLtIntInSignalX             },/* 0x814: 内部入力信号モニタ */
    {0x818   , &undef_AlmLtIntOutSignalX            },/* 0x818: 内部出力信号モニタ */
    {0x820   , &undef_AlmLtDcVoltxX                 },/* 0x820: 主回路母線電圧 */
    {0x821   , &undef_AlmLtIqRateX                  },/* 0x821: 電流検出値 */
    {0x822   , &undef_AlmLtMotLoad2msX              },/* 0x822: 累積負荷率 */
    {0x823   , &undef_AlmLtRegerRate2msX            },/* 0x823: 回生負荷率 */
    {0x824   , &undef_AlmLtDbrxPower2msX            },/* 0x824: ＤＢ抵抗消費電力 */
    {0x825   , &undef_AlmLtMotLoad10sX              },/* 0x825: 最大累積負荷率 */
    {0x826   , &undef_AlmLtJLRatioX                 },/* 0x826: 慣性モーメント比/重量比 */
    {0x830   , &undef_AlmLtSencComErrCntX           },/* 0x830: シリアルエンコーダ通信異常回数 */
    {0x831   , &undef_AlmLtM2ComErrCntX             },/* 0x831: MECHATROLINK通信異常回数 */
    {0x832   , &undef_AlmLtSafetyIOSignalX          },/* 0x832: セーフティ入出力信号モニタ */
    {0x833   , &undef_AlmLtAlmCodeX                 },/* 0x833: アラーム番号 */
    {0x834   , &undef_AlmLtTmStampX                 },/* 0x834: タイムスタンプ */
    {0x900   , &undef_AlmTrcAlmCode                 },/* 0x900: トレースアラーム番号 */
    {0x901   , &undef_AlmTrcTmStamp                 },/* 0x901: トレースアラームタイムスタンプ */
    {0xC00   , &undef_Apos                          },/* 0xC00: フィードバック位置(APOS) */
    {0xC02   , &undef_Cpos                          },/* 0xC02: 現在指令位置(CPOS) */
    {0xC04   , &undef_Perr                          },/* 0xC04: 位置偏差(PERR) */
    {0xC06   , &undef_Tpos                          },/* 0xC06: 目標位置(TPOS) */
    {0xC08   , &undef_Lpos1                         },/* 0xC08: ラッチ位置１(LPOS1) */
    {0xC0A   , &undef_Lpos2                         },/* 0xC0A: ラッチ位置２(LPOS2) */
    {0xC0C   , &undef_Lpos3                         },/* 0xC0C: ラッチ位置３(LPOS3) */
    {0xC0E   , &undef_Tspd                          },/* 0xC0E: 目標速度(TSPD) */
    {0xC10   , &undef_Fspd                          },/* 0xC10: フィードバック速度(FSPD) */
    {0xC12   , &undef_Cspd                          },/* 0xC12: 現在位置指令速度(CSPD) */
    {0xC14   , &undef_TrqLim                        },/* 0xC14: トルク制限値(TRQ_LIM) */
    {0xC16   , &undef_SpdLim                        },/* 0xC16: 速度制限値(SPD_LIM) */
    {0xC18   , &undef_M3ComPhase                    },/* 0xC18: M-Ⅲ通信フェーズ */
    {0xC19   , &undef_M3Rcmd                        },/* 0xC19: M-Ⅲ現在実行コマンドコード(RCMD) */
    {0xC1A   , &undef_M3SubRcmd                     },/* 0xC1A: M-Ⅲ現在実行サブコマンドコード(SUB_RCMD) */
    {0xF000  , &undef_AllInSignal1                  },/* 0xF000: 全入力信号モニタ1(for LCD Operator) */
    {0xF001  , &undef_AllInSignal2                  },/* 0xF001: 全入力信号モニタ2(for LCD Operator) */
    {0xF002  , &undef_AllOutSignal                  },/* 0xF002: 全出力信号モニタ(for LCD Operator) */
    {0xF003  , &undef_InterSignalX1                 },/* 0xF003: 内部信号モニタ1 */
    {0xF004  , &undef_InterSignalX2                 },/* 0xF004: 内部信号モニタ2 */
    {0xF005  , &undef_InterSignalX3                 },/* 0xF005: 内部信号モニタ3 */
    {0xF006  , &undef_InterSignalX4                 },/* 0xF006: 内部信号モニタ4 */
    {0xF007  , &undef_IntInSignalX1                 },/* 0xF007: 内部入力信号モニタ 1 */
    {0xF008  , &undef_IntInSignalX2                 },/* 0xF008: 内部入力信号モニタ 2 */
    {0xF009  , &undef_IntInSignalX3                 },/* 0xF009: 内部入力信号モニタ 3 */
    {0xF00A  , &undef_IntInSignalX4                 },/* 0xF00A: 内部入力信号モニタ 4 */
    {0xF00B  , &undef_IntOutSignalX1                },/* 0xF00B: 内部出力信号モニタ1 */
    {0xF00C  , &undef_IntOutSignalX2                },/* 0xF00C: 内部出力信号モニタ2 */
    {0xF00D  , &undef_IntOutSignalX3                },/* 0xF00D: 内部出力信号モニタ3 */
    {0xF00E  , &undef_SelSpdOrDiv                   },/* 0xF00E: モータ最大速度上限/分周比設定上限値 */
    {0xFFFF  , NULL                                 },/* 0xFFFF: End of Table List */
};
const ULONG UnRegTableEntNum = sizeof(UnRegTable) / sizeof(UnRegTable[0]);


/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL UnMonTable[] = {
    {0x000   , &undef_SpdFbk                        },/* 0x000: モータ回転/移動速度 */
    {0x001   , &undef_SpdRef                        },/* 0x001: 速度指令 */
    {0x002   , &undef_TrqRef                        },/* 0x002: 内部トルク/推力指令 */
    {0x003   , &undef_MotAngPls                     },/* 0x003: 電気角1(32ビット10進表示) */
    {0x004   , &undef_MotAngDeg                     },/* 0x005: 電気角2(U相0degからの角度) */
    {0x005   , &undef_InSignal                      },/* 0x006: 入力信号モニタ(dummy) */
    {0x006   , &undef_OutSignal                     },/* 0x007: 出力信号モニタ */
    {0x007   , &undef_PosRefSpd                     },/* 0x008: 入力指令パルス速度 */
    {0x008   , &undef_PosErrCntr                    },/* 0x009: 偏差カウンタ(位置偏差量) */
    {0x009   , &undef_MotLoad                       },/* 0x00B: 累積負荷率 */
    {0x00A   , &undef_RegLoad                       },/* 0x00C: 回生負荷率 */
    {0x00B   , &undef_DbrxPower                     },/* 0x00D: ＤＢ抵抗消費電力 */
    {0x00C   , &undef_PcmdCounter                   },/* 0x00E: 入力指令パルスカウンタ */
    {0x00D   , &undef_PfbkCounter                   },/* 0x010: モータエンコーダ位置 */
    {0x00E   , &undef_FposCounter                   },/* 0x012: フルクローズドフィードバックパルスカウンタ */
    {0x00F   , &undef_FcPfbkSpd                     },/* 0x014: フルクローズドフィードバック速度 */
    {0x010   , &undef_SelSpdOrDiv                   },/* 0xF00E: モータ最大速度上限/分周比設定上限値 */
    {0x011   , &undef_LmPoleSensorX                 },/* 0x01A: ホールセンサ情報 */
    {0x012   , &undef_PowerOnTime                   },/* 0x016: 延べ稼働時間 */
    {0x013   , &undef_Pfbk2Counter                  },/* 0x01B: フィードバックパルスカウンタ */
    {0x014   , &undef_ActGain                       },/* 0x01D: 有効ゲインモニタ */
    {0x015   , &undef_SafetyIOSignal                },/* 0x01E: セーフティ入出力信号モニタ */
    {0x016   , &undef_SafetyExternalIOSignal        },/* 0x01F: セーフティモジュール外部入出力信号モニタ */
    {0x017   , &undef_SafetySettingMon              },/* 0x020: セーフティモジュール安全機能設定/動作モニタ */
    {0x018   , &undef_SafetySysStatMon              },/* 0x021: セーフティモジュールシステム状態モニタ */
    {0x019   , &undef_SafetySpdReachTimeA           },/* 0x022: セーフティモジュール安全速度到達時間A */
    {0x01A   , &undef_SafetySpdReachTimeB           },/* 0x024: セーフティモジュール安全速度到達時間B */
    {0x01B   , &undef_ActiveModeInterCmdSpd         },/* 0x026: アクティブモード内部指令速度 */
    {0x01C   , &undef_SafetyMotorSpd                },/* 0x027: セーフティモジュールモータ速度 */
    {0x01D   , &undef_SafetyMotorPos                },/* 0x028: セーフティモジュールモータ位置 */
    {0x01E   , &undef_SafetyMonitoringSpdA          },/* 0x02A: セーフティモジュール監視速度A */
    {0x01F   , &undef_SafetyMonitoringSpdB          },/* 0x02B: セーフティモジュール監視速度B */
    {0x020   , &undef_MotRatSpd                     },/* 0x046: モータ定格速度 [回転型：min-1,リニア型：mm/s] */
    {0x021   , &undef_MotMaxSpd                     },/* 0x047: モータ最大速度 [回転型：min-1,リニア型：mm/s] */
    {0x025   , &undef_AmpInstEnvMon                 },/* 0x02C: サーボパック設置環境モニタ */
    {0x026   , &undef_MotInstEnvMon                 },/* 0x02D: モータ設置環境モニタ */
    {0x027   , &undef_BuiltInFanLifeMon             },/* 0x050: 内蔵FAN寿命モニタ */
    {0x028   , &undef_CondenserLifeMon              },/* 0x051: コンデンサ寿命モニタ */
    {0x029   , &undef_CollisionProofCircuitLifeMon  },/* 0x052: 突防回路寿命モニタ */
    {0x02A   , &undef_DBCircuitLifeMon              },/* 0x053: DB回路寿命モニタ */
    {0x02B   , &undef_CollisionProofRelayMotionNum  },/* 0x054: 突防リレー動作回数 */
    {0x02C   , &undef_DBRelayMotionNum              },/* 0x056: DBリレー動作回数 */
    {0x030   , &undef_BlshComp                      },/* 0x030: バックラッシ補正量 */
    {0x031   , &undef_BlshCompLimit                 },/* 0x032: バックラッシ補正量設定制限値 */
    {0x032   , &undef_electricityConsumption        },/* 0x034: 消費電力 */
    {0x033   , &undef_PowerConsumption              },/* 0x035: 消費電力量 */
    {0x034   , &undef_TotalPowerConsumptionConst    },/* 0x037: 累積消費電力量（整数部） */
    {0x035   , &undef_PosErr32s                     },/* 0x03E: 位置アンプ偏差 */
    {0x040   , &undef_MultiTurn                     },/* 0x040: 絶対値エンコーダマルチターンデータ */
    {0x041   , &undef_IncPulse                      },/* 0x041: 絶対値エンコーダの1回転内位置 */
    {0x042   , &undef_ScaleAbsPos_l                 },/* 0x043: 絶対値エンコーダの位置データ（下位） */
    {0x043   , &undef_ScaleAbsPos_h                 },/* 0x045: 絶対値エンコーダの位置データ（上位） */
    {0x050   , &undef_AllInSignal1                  },/* 0xF000: 全入力信号モニタ1(for LCD Operator) */
    {0x051   , &undef_AllInSignal2                  },/* 0xF001: 全入力信号モニタ2(for LCD Operator) */
    {0x052   , &undef_AllOutSignal                  },/* 0xF002: 全出力信号モニタ(for LCD Operator) */
    {0x080   , &undef_MpfMovePosP                   },/* 0x080: 磁極検出時正方向最大移動量 */
    {0x081   , &undef_MpfMovePosN                   },/* 0x081: 磁極検出時逆方向最大移動量 */
    {0x082   , &undef_MpfFindingTime                },/* 0x082: 磁極検出所要時間 */
    {0x083   , &undef_MpfPhaseOffset                },/* 0x083: 磁極検出位相オフセット量 */
    {0x084   , &undef_LinearScalePitch              },/* 0x084: リニアスケールピッチ */
    {0x085   , &undef_LinearScalePitchIndex         },/* 0x086: リニアスケールピッチ指数 */
    {0x086   , &undef_MpfJudgeMonitor               },/* 0x087: 磁極検出判定モニタ */
    {0x104   , &undef_SencComErrCnt                 },/* 0x104: シリアルエンコーダ通信異常回数 */
    {0x105   , &undef_SettlingTime                  },/* 0x105: 位置決め整定時間 */
    {0x106   , &undef_OverShoot                     },/* 0x106: 位置決めオーバシュート量 */
    {0x107   , &undef_RvibFrequency                 },/* 0x107: 残留振動周波数 */
    {0x10A   , &undef_HeatsinkTemp                  },/* 0x10A: ヒートシンク温度モニタ */
    {0x10B   , &undef_CoinOffTime                   },/* 0x10B: 位置決め完了失敗時間積算 */
    {0x10C   , &undef_VibDetSpd                     },/* 0x10C: 発振検出用差速度 */
    {0x10D   , &undef_CtrlBoardTemp                 },/* 0x10D: 制御基板温度モニタ */
    {0x10E   , &undef_SifAdCnvData                  },/* 0x10E: 力センサモニタ */
    {0x10F   , &undef_Temperature                   },/* 0x10F: エンコーダ内気温 */
    {0x110   , &undef_InterSignalX                  },/* 0x110: 内部信号モニタ */
    {0x110   , &undef_InterSignalX1                 },/* 0xF003: 内部信号モニタ1 */
    {0x111   , &undef_InterSignalX2                 },/* 0xF004: 内部信号モニタ2 */
    {0x112   , &undef_InterSignalX3                 },/* 0xF005: 内部信号モニタ3 */
    {0x113   , &undef_InterSignalX4                 },/* 0xF006: 内部信号モニタ4 */
    {0x120   , &undef_IntInSignalX                  },/* 0x120: 内部入力信号モニタ */
    {0x120   , &undef_IntInSignalX1                 },/* 0xF007: 内部入力信号モニタ 1 */
    {0x121   , &undef_IntInSignalX2                 },/* 0xF008: 内部入力信号モニタ 2 */
    {0x122   , &undef_IntInSignalX3                 },/* 0xF009: 内部入力信号モニタ 3 */
    {0x123   , &undef_IntInSignalX4                 },/* 0xF00A: 内部入力信号モニタ 4 */
    {0x130   , &undef_IntOutSignalX                 },/* 0x130: 内部出力信号モニタ */
    {0x130   , &undef_IntOutSignalX1                },/* 0xF00B: 内部出力信号モニタ1 */
    {0x131   , &undef_IntOutSignalX2                },/* 0xF00C: 内部出力信号モニタ2 */
    {0x132   , &undef_IntOutSignalX3                },/* 0xF00D: 内部出力信号モニタ3 */
    {0x140   , &undef_DcVoltx                       },/* 0x140: 主回路母線電圧 */
    {0x141   , &undef_IqRate                        },/* 0x141: 電流検出値 */
    {0x142   , &undef_MotLoad2ms                    },/* 0x142: 累積負荷率 */
    {0x143   , &undef_RegerRate2ms                  },/* 0x143: 回生負荷率 */
    {0x144   , &undef_DbrxPower2ms                  },/* 0x144: ＤＢ抵抗消費電力 */
    {0x145   , &undef_MotLoad10s                    },/* 0x145: 最大累積負荷率 */
    {0x146   , &undef_JLRatio                       },/* 0x146: 慣性モーメント比/重量比 */
    {0x148   , &undef_TrefAdMon                     },/* 0x148: T-REFモニタ */
    {0x149   , &undef_VrefAdMon                     },/* 0x149: V-REFモニタ */
    {0x14A   , &undef_DPulseHwCntMon                },/* 0x14A: 入力指令パルス周波数 */
    {0x14C   , &undef_IdrefRate                     },/* 0x14C: d軸電流指令モニタ */
    {0x14D   , &undef_MbusErrReg                    },/* 0x14D: MEMOBUSエラー情報（エラー発生アドレス） */
    {0x170   , &undef_AvgIntTmpr                    },/* 0x170: 平均内部温度 */
    {0x171   , &undef_ControlSubstrateTemp          },/* 0x171: 制御基板温度 */
    {0x172   , &undef_PowerSubstrateTemp            },/* 0x172: パワー基板温度 */
    {0x1D0   , &undef_AlmLtSpdFbkX                  },/* 0x800: モータ回転/移動速度 */
    {0x1D1   , &undef_AlmLtSpdRefX                  },/* 0x801: 速度指令 */
    {0x1D2   , &undef_AlmLtTrqRefX                  },/* 0x802: 内部トルク/推力指令 */
    {0x1D3   , &undef_AlmLtPosRefSpdX               },/* 0x803: 入力指令パルス速度 */
    {0x1D4   , &undef_AlmLtPosErrCntrX              },/* 0x804: 偏差カウンタ(位置偏差量) */
    {0x1DF   , &undef_AlmLtDcVoltxX                 },/* 0x820: 主回路母線電圧 */
    {0x1E0   , &undef_AlmLtIqRateX                  },/* 0x821: 電流検出値 */
    {0x1E1   , &undef_AlmLtMotLoad2msX              },/* 0x822: 累積負荷率 */
    {0x1E2   , &undef_AlmLtRegerRate2msX            },/* 0x823: 回生負荷率 */
    {0x1E3   , &undef_AlmLtDbrxPower2msX            },/* 0x824: ＤＢ抵抗消費電力 */
    {0x1E4   , &undef_AlmLtMotLoad10sX              },/* 0x825: 最大累積負荷率 */
    {0x1E5   , &undef_AlmLtJLRatioX                 },/* 0x826: 慣性モーメント比/重量比 */
    {0x1E6   , &undef_AlmLtSencComErrCntX           },/* 0x830: シリアルエンコーダ通信異常回数 */
    {0x1E7   , &undef_AlmLtM2ComErrCntX             },/* 0x831: MECHATROLINK通信異常回数 */
    {0x1E8   , &undef_AlmLtSafetyIOSignalX          },/* 0x832: セーフティ入出力信号モニタ */
    {0x1E9   , &undef_AlmLtAlmCodeX                 },/* 0x833: アラーム番号 */
    {0x1EA   , &undef_AlmLtTmStampX                 },/* 0x834: タイムスタンプ */
    {0x200   , &undef_MencTemp                      },/* 0x200: エンコーダ内気温温度モニタ */
    {0x201   , &undef_RomChkSumVal                  },/* 0x201: ROMチェックサム値（システムレベル） */
    {0x202   , &undef_MbusErrReg                    },/* 0x202: MEMOBUSエラー情報（エラー発生アドレス） */
    {0x203   , &undef_ErrPrmNo                      },/* 0x203: 設定異常パラメータ番号(A.040) */
    {0x204   , &undef_LmtPrmNo                      },/* 0x204: リミット処理パラメータ番号(NoAlarm) */
    {0x212   , &undef_CurExeTimeMA                  },/* 0x314: μプログラムScanA現在処理時間 */
    {0x213   , &undef_MaxExeTimeMA                  },/* 0x316: μプログラムScanA最大処理時間 */
    {0x214   , &undef_CurExeTimeB                   },/* 0x304: ScanB現在処理時間 */
    {0x215   , &undef_MaxExeTimeB                   },/* 0x306: ScanB最大処理時間 */
    {0x216   , &undef_CurExeTimeC                   },/* 0x308: ScanC現在処理時間 */
    {0x217   , &undef_MaxExeTimeC                   },/* 0x30A: ScanC最大処理時間 */
    {0x220   , &undef_CurExeTimeS                   },/* 0x300: 2msあたりのCPU負荷率 */
    {0x221   , &undef_MaxExeTimeS                   },/* 0x302: 2msあたりのCPU最大負荷率 */
    {0xFFFF  , NULL                                 },/* 0xFFFF: End of Table List */
};
const ULONG UnMonTableEntNum = sizeof(UnMonTable) / sizeof(UnMonTable[0]);



/******************************* End of File *******************************/

