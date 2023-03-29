/****************************************************************************
  Description   : Operation Register Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

    Project       : Mercury

  Create Date   : 2014/03/18

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "OpeRegFunc.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of 0x000 : 操作モード */
/***************************************************************************/
const PRM_ATTR_T opedef_OpeMode = {
    0x000,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcOprationMode1,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001 : 操作設定 */
/***************************************************************************/
const PRM_ATTR_T opedef_OpeCmd = {
    0x001,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcOprationCmd1,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002 : 操作終了 */
/***************************************************************************/
const PRM_ATTR_T opedef_OpeEnd = {
    0x002,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcOperationFinish,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x003 : アクセスレベル */
/***************************************************************************/
const PRM_ATTR_T opedef_AccLevel = {
    0x003,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcAccessLevel,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x004 : 第２操作モード */
/***************************************************************************/
const PRM_ATTR_T opedef_OpeMode2 = {
    0x004,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcOprationMode2,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x005 : 第２操作設定  */
/***************************************************************************/
const PRM_ATTR_T opedef_OpeCmd2 = {
    0x005,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcOprationCmd2,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x006 : トレース操作モード */
/***************************************************************************/
static void *opedefGetRamPtr_OpeModeTrc(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.OpeModeTrc);}

const PRM_ATTR_T opedef_OpeModeTrc = {
    0x006,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_OpeModeTrc,            /* Parameter varialbles  */
    &fnCalcTraceMode,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x007 : トレース操作設定 */
/***************************************************************************/
static void *opedefGetRamPtr_OpeCmdTrc(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.OpeCmdTrc);}

const PRM_ATTR_T opedef_OpeCmdTrc = {
    0x007,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_OpeCmdTrc,             /* Parameter varialbles  */
    &fnCalcTraceCmd,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x008 : 安全停止時間[s] */
/***************************************************************************/
static void *opedefGetRamPtr_SafetyTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->SafeStopTime);}

const PRM_ATTR_T opedef_SafetyTime = {
    0x008,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &opedefGetRamPtr_SafetyTime,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x009 : 通信伝送速度設定（シリアル通信用） */
/***************************************************************************/
const PRM_ATTR_T opedef_ComTransmissionSpdCmd = {
    0x009,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00F : オペレータ表示ステータス */
/***************************************************************************/
static void *opedefGetRamPtr_OperatorSts(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->OperatorSts);}

const PRM_ATTR_T opedef_OperatorSts = {
    0x00F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x8000,                                 /* Lower Limit */
    0x7FFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_OperatorSts,           /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x017 : オンライン振動モニタ完了確認 */
/***************************************************************************/
static void *opedefGetRamPtr_VibmState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->VibMon)->OpeReg.VibmState);}

const PRM_ATTR_T opedef_VibmState = {
    0x017,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmState,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x020 : ユーザ定数書込み禁止 */
/***************************************************************************/
const PRM_ATTR_T opedef_PrmWrtDis = {
    0x020,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcPrmAccessLock,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x030 : 原点サーチ完了 */
/***************************************************************************/
static void *opedefGetRamPtr_ZsrchState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->OpeReg.ZsrchState);}

const PRM_ATTR_T opedef_ZsrchState = {
    0x030,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_ZsrchState,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x031 : プログラムＪＯＧ運転完了確認 */
/***************************************************************************/
static void *opedefGetRamPtr_PjogState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->BaseControls)->PJogHdl.PJogState);}

const PRM_ATTR_T opedef_PjogState = {
    0x031,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_PjogState,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x032 : 磁極検出完了 */
/***************************************************************************/
const PRM_ATTR_T opedef_PdetState = {
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
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnGetPdetStatus,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x033 : 原点位置設定完了 */
/***************************************************************************/
const PRM_ATTR_T opedef_OrgSetState = {
    0x033,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnGetScaleZsetStatus,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x034 : ＪＯＧ運転完了 */
/***************************************************************************/
const PRM_ATTR_T opedef_JogState = {
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x043 : 出力信号設定データ */
/***************************************************************************/
static void *opedefGetRamPtr_OutData(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->ForceOutSignals);}

const PRM_ATTR_T opedef_OutData = {
    0x043,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_OutData,               /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x050 : １パラチューニングモード */
/***************************************************************************/
static void *opedefGetRamPtr_SpatMode(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OnePrmTune)->OpeReg.SpatMode);}

const PRM_ATTR_T opedef_SpatMode = {
    0x050,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    3,                                      /* Upper Limit */
    2,                                      /* Default value */
    &opedefGetRamPtr_SpatMode,              /* Parameter varialbles  */
    &FnOnePrmTuneSpatMode,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x051 : １パラチューニングレベル */
/***************************************************************************/
static void *opedefGetRamPtr_SpatLevel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OnePrmTune)->OpeReg.SpatLevel);}

const PRM_ATTR_T opedef_SpatLevel = {
    0x051,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_SpatLevel,             /* Parameter varialbles  */
    &FnOnePrmTuneSpatLevel,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x052 : １パラチューニングＦＦレベル */
/***************************************************************************/
static void *opedefGetRamPtr_SpatFFLevel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OnePrmTune)->OpeReg.SpatFFLevel);}

const PRM_ATTR_T opedef_SpatFFLevel = {
    0x052,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_SpatFFLevel,           /* Parameter varialbles  */
    &FnOnePrmTuneSpatFFLevel,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x053 : １パラメータチューニングレベル変更完了 */
/***************************************************************************/
static void *opedefGetRamPtr_SpatLvlChgState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OnePrmTune)->OpeReg.SpatLvlChgState);}

const PRM_ATTR_T opedef_SpatLvlChgState = {
    0x053,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_SpatLvlChgState,       /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x054 : １パラメータチューニング自動ノッチ完了 */
/***************************************************************************/
static void *opedefGetRamPtr_SpatANotchState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OnePrmTune)->OpeReg.SpatANotchState);}

const PRM_ATTR_T opedef_SpatANotchState = {
    0x054,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_SpatANotchState,       /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x055 : １パラメータチューニングＡ型制振制御完了確認 */
/***************************************************************************/
static void *opedefGetRamPtr_SpatAresState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OnePrmTune)->OpeReg.SpatAresState);}

const PRM_ATTR_T opedef_SpatAresState = {
    0x055,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_SpatAresState,         /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x060 : EasyFFTモード */
/***************************************************************************/
static void *opedefGetRamPtr_EzFftMode(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->EasyFFT)->OpeReg.EzFftMode);}

const PRM_ATTR_T opedef_EzFftMode = {
    0x060,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_EzFftMode,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x061 : EasyFFT完了確認 */
/***************************************************************************/
static void *opedefGetRamPtr_EzFftState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->EasyFFT)->OpeReg.EzFftState);}

const PRM_ATTR_T opedef_EzFftState = {
    0x061,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_EzFftState,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x062 : EasyFFTトルク振幅 */
/***************************************************************************/
static void *opedefGetRamPtr_EzFftTrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->EasyFFT)->OpeReg.EzFftTrq);}

const PRM_ATTR_T opedef_EzFftTrq = {
    0x062,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_EzFftTrq,              /* Parameter varialbles  */
    &FnEasyFftTrqAmplitude,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x080 : モータ定数読み書き先頭アドレス */
/***************************************************************************/
const PRM_ATTR_T opedef_MotPrmOpeAddress = {
    0x080,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0x009F,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcMotorParameterAddress,           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x081 : モータ定数読み書きサイズ */
/***************************************************************************/
const PRM_ATTR_T opedef_MotPrmOpeSize = {
    0x081,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0001,                                 /* Lower Limit */
    0x0400,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcMotorParameterSize,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x082 : モータ定数読み書き完了 */
/***************************************************************************/
const PRM_ATTR_T opedef_MotPrmOpeState = {
    0x082,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnGetMotorDownloadState,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x083 : モータ定数スケール読書き対象指定 */
/***************************************************************************/
const PRM_ATTR_T opedef_MotPrmScaleRW = {
    0x083,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0x0200,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08A : サーボパラメータ格納先先頭アドレス */
/***************************************************************************/
const PRM_ATTR_T opedef_SvPrmRwOpeAddress = {
    0x08A,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x4000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08B : サーボパラメータデータサイズ */
/***************************************************************************/
const PRM_ATTR_T opedef_SvPrmRwOpeSize = {
    0x08B,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    EEPROM_SIZE,                            /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x08C : サーボパラメータ読み書き完了 */
/***************************************************************************/
const PRM_ATTR_T opedef_SvPrmRwState = {
    0x08C,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnGetPrmDownloadState,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x090 : アドバンストＡＴモード */
/***************************************************************************/
static void *opedefGetRamPtr_AdatMode(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AdatHndl->AdatReg.AdatMode);}

const PRM_ATTR_T opedef_AdatMode = {
    0x090,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_AdatMode,              /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x091 : アドバンストＡＴレベル */
/***************************************************************************/
static void *opedefGetRamPtr_AdatLevel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AdatHndl->AdatReg.AdatLevel);}

const PRM_ATTR_T opedef_AdatLevel = {
    0x091,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Lower Limit */
    3,                                      /* Upper Limit */
    2,                                      /* Default value */
    &opedefGetRamPtr_AdatLevel,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x092 : アドバンストＡＴ移動距離 */
/***************************************************************************/
static void *opedefGetRamPtr_AdatMovep(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AdatHndl->AdatReg.AdatMovep.Long);}

const PRM_ATTR_T opedef_AdatMovep = {
    0x092,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x80000000,                             /* Lower Limit */
    0x7FFFFFFF,                             /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_AdatMovep,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x094 : アドバンストＡＴ完了確認 */
/***************************************************************************/
static void *opedefGetRamPtr_AdatState(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AdatHndl->AdatReg.AdatState);}

const PRM_ATTR_T opedef_AdatState = {
    0x094,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_AdatState,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x095 : アドバンストＡＴフィルタタイプ */
/***************************************************************************/
static void *opedefGetRamPtr_AdatFilType(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->AdatHndl->AdatReg.AdatFilType);}

const PRM_ATTR_T opedef_AdatFilType = {
    0x095,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Lower Limit */
    3,                                      /* Upper Limit */
    2,                                      /* Default value */
    &opedefGetRamPtr_AdatFilType,           /* Parameter varialbles  */
    &FnAdatFilType,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0A0 : A型制振機能調整モード設定 */
/***************************************************************************/
static void *opedefGetRamPtr_AvibMode(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->AvibTune)->OpeReg.AvibMode);}

const PRM_ATTR_T opedef_AvibMode = {
    0x0A0,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_AvibMode,              /* Parameter varialbles  */
    &FnAvibTuneAvibMode,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0A1 : A型制振機能調整完了 */
/***************************************************************************/
static void *opedefGetRamPtr_AvibState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->AvibTune)->OpeReg.AvibState);}

const PRM_ATTR_T opedef_AvibState = {
    0x0A1,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_AvibState,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0A2 : A型制振機能調整周波数 */
/***************************************************************************/
static void *opedefGetRamPtr_AvibFreq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->AvibTune)->OpeReg.AvibFreq);}

const PRM_ATTR_T opedef_AvibFreq = {
    0x0A2,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_AvibFreq,              /* Parameter varialbles  */
    &FnAvibTuneAvibFreq,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0A3 : A型制振機能調整ゲイン */
/***************************************************************************/
static void *opedefGetRamPtr_AvibGain(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->AvibTune)->OpeReg.AvibGain);}

const PRM_ATTR_T opedef_AvibGain = {
    0x0A3,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_AvibGain,              /* Parameter varialbles  */
    &FnAvibTuneAvibGain,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B0 : 振動抑制付きMFC調整周波数 */
/***************************************************************************/
static void *opedefGetRamPtr_MdlVibFreq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MfcTune)->OpeReg.MdlVibFreq);}

const PRM_ATTR_T opedef_MdlVibFreq = {
    0x0B0,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MdlVibFreq,            /* Parameter varialbles  */
    &FnMfcTuneMdlVibFreq,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B1 : 振動抑制付きMFC調整完了 */
/***************************************************************************/
static void *opedefGetRamPtr_MdlVibState(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MfcTune)->OpeReg.MdlVibState);}

const PRM_ATTR_T opedef_MdlVibState = {
    0x0B1,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MdlVibState,           /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B2 : 振動抑制機能チューニング周波数2 */
/***************************************************************************/
static void *opedefGetRamPtr_MdlVibFreq2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MfcTune)->OpeReg.MdlVibFreq2);}

const PRM_ATTR_T opedef_MdlVibFreq2 = {
    0x0B2,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MdlVibFreq2,           /* Parameter varialbles  */
    &FnMfcTuneMdlVibFreq2,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B3 : 振動抑制機能チューニング周波数2補正 */
/***************************************************************************/
static void *opedefGetRamPtr_MdlVibFil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MfcTune)->OpeReg.MdlVibFil2);}

const PRM_ATTR_T opedef_MdlVibFil2 = {
    0x0B3,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MdlVibFil2,            /* Parameter varialbles  */
    &FnMfcTuneMdlVibFil2,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B4 : 振動抑制機能設定2完了確認 */
/***************************************************************************/
static void *opedefGetRamPtr_MdlVibState2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MfcTune)->OpeReg.MdlVibState2);}

const PRM_ATTR_T opedef_MdlVibState2 = {
    0x0B4,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MdlVibState2,          /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B5 : 速度リップル補償情報書込み確認 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompInfoWriteChk = {
    0x0B5,                                  /* Register Number */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCmpInfo,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B6 : 速度リップル補償ゲイン */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompGain = {
    0x0B6,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompGain,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B7 : 速度リップル補償第1成分周期 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompCycle1 = {
    0x0B7,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompCycle1,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B8 : 速度リップル補償第1成分振幅値 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompAmplitude1 = {
    0x0B8,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompAmplitude1,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0B9 : 速度リップル補償第1成分位相 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompPhase1 = {
    0x0B9,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompPhase1,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0BA : 速度リップル補償第2成分周期 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompCycle2 = {
    0x0BA,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompCycle2,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0BB : 速度リップル補償第2成分振幅値 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompAmplitude2 = {
    0x0BB,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompAmplitude2,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0BC : 速度リップル補償第2成分位相 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompPhase2 = {
    0x0BC,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompPhase2,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0BD : 速度リップル補償第3成分周期 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompCycle3 = {
    0x0BD,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompCycle3,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0BE : 速度リップル補償第3成分振幅値 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompAmplitude3 = {
    0x0BE,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompAmplitude3,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0BF : 速度リップル補償第3成分位相 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompPhase3 = {
    0x0BF,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompPhase3,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C0 : 速度リップル補償第4成分周期 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompCycle4 = {
    0x0C0,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompCycle4,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C1 : 速度リップル補償第4成分振幅値 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompAmplitude4 = {
    0x0C1,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompAmplitude4,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C2 : 速度リップル補償第4成分位相 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompPhase4 = {
    0x0C2,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompPhase4,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C3 : 速度リップル補償第5成分周期 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompCycle5 = {
    0x0C3,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompCycle5,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C4 : 速度リップル補償第5成分振幅値 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompAmplitude5 = {
    0x0C4,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompAmplitude5,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C5 : 速度リップル補償第5成分位相 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompPhase5 = {
    0x0C5,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompPhase5,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C6 : 速度リップル補償第6成分周期 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompCycle6 = {
    0x0C6,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompCycle6,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C7 : 速度リップル補償第6成分振幅値 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompAmplitude6 = {
    0x0C7,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompAmplitude6,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0C8 : 速度リップル補償第6成分位相 */
/***************************************************************************/
const PRM_ATTR_T opedef_RippleCompPhase6 = {
    0x0C8,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcRippleCompPhase6,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0D0 : オプションカード選択 */
/***************************************************************************/
static void *opedefGetRamPtr_OptCardSel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OptClr)->OpeReg.OptCardNo);}

const PRM_ATTR_T opedef_OptCardSel = {
    0x0D0,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    3,                                      /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_OptCardSel,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0E0 : ダイナミックオートチューニングゲインレベル */
/***************************************************************************/
static void *opedefGetRamPtr_DatGnLevel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->BaseControls)->TuneLessCtrl.OpeReg.DatGnLevel);}

const PRM_ATTR_T opedef_DatGnLevel = {
    0x0E0,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Lower Limit */
    7,                                      /* Upper Limit */
    4,                                      /* Default value */
    &opedefGetRamPtr_DatGnLevel,            /* Parameter varialbles  */
    &FnSetTLLvlGainLevel,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0E1 : ダイナミックオートチューニング負荷レベル */
/***************************************************************************/
static void *opedefGetRamPtr_DatJratLevel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->BaseControls)->TuneLessCtrl.OpeReg.DatJratLevel);}

const PRM_ATTR_T opedef_DatJratLevel = {
    0x0E1,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    2,                                      /* Upper Limit */
    1,                                      /* Default value */
    &opedefGetRamPtr_DatJratLevel,          /* Parameter varialbles  */
    &FnSetTLLvlJratLevel,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x100 : トレースモード */
/***************************************************************************/
static void *opedefGetRamPtr_TrcMode(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrcMode);}

const PRM_ATTR_T opedef_TrcMode = {
    0x100,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0002,                                 /* Lower Limit */
    0x0010,                                 /* Upper Limit */
    0x0003,                                 /* Default value */
    &opedefGetRamPtr_TrcMode,               /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x101 : トレースデータ長 */
/***************************************************************************/
static void *opedefGetRamPtr_DataSize(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.DataSize);}

const PRM_ATTR_T opedef_DataSize = {
    0x101,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0001,                                 /* Lower Limit */
    0x0004,                                 /* Upper Limit */
    0x0001,                                 /* Default value */
    &opedefGetRamPtr_DataSize,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x102 : 使用エリアサイズ */
/***************************************************************************/
static void *opedefGetRamPtr_AreaSize(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.AreaSize);}

const PRM_ATTR_T opedef_AreaSize = {
    0x102,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    512,                                    /* Lower Limit */
    8192,                                   /* Upper Limit */
    3072,                                   /* Default value */
    &opedefGetRamPtr_AreaSize,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x103 : トレースサンプリング間隔 */
/***************************************************************************/
static void *opedefGetRamPtr_SampIntv(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.SampIntv);}

const PRM_ATTR_T opedef_SampIntv = {
    0x103,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0001,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0001,                                 /* Default value */
    &opedefGetRamPtr_SampIntv,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x104 : トレースサンプリング時間[us] */
/***************************************************************************/
static void *opedefGetRamPtr_SampTime(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.SampTime);}

const PRM_ATTR_T opedef_SampTime = {
    0x104,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0001,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    125,                                    /* Default value */
    &opedefGetRamPtr_SampTime,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x105 : トレースサンプリング間隔下限 */
/***************************************************************************/
static void *opedefGetRamPtr_SampLlim(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.SampLlim);}

const PRM_ATTR_T opedef_SampLlim = {
    0x105,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0001,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    1,                                      /* Default value */
    &opedefGetRamPtr_SampLlim,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x106 : トレースサンプリング間隔上限 */
/***************************************************************************/
static void *opedefGetRamPtr_SampUlim(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.SampUlim);}

const PRM_ATTR_T opedef_SampUlim = {
    0x106,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0001,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    65535,                                  /* Default value */
    &opedefGetRamPtr_SampUlim,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x107 : 最大エリアサイズ */
/***************************************************************************/
static void *opedefGetRamPtr_MaxAreaSize(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.MaxAreaSize);}

const PRM_ATTR_T opedef_MaxAreaSize = {
    0x107,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    8192,                                   /* Default value */
    &opedefGetRamPtr_MaxAreaSize,           /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10A : トレースバッファ長 */
/***************************************************************************/
static void *opedefGetRamPtr_TrcBuffSize(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrcBuffSize);}

const PRM_ATTR_T opedef_TrcBuffSize = {
    0x10A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_TrcBuffSize,           /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10B : 最大トレースバッファ長 */
/***************************************************************************/
static void *opedefGetRamPtr_MaxTrcBuffSize(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.MaxTrcBuffSize);}

const PRM_ATTR_T opedef_MaxTrcBuffSize = {
    0x10B,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    8192,                                   /* Default value */
    &opedefGetRamPtr_MaxTrcBuffSize,        /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x110 : データ、I/Oトレース対象選択 */
/***************************************************************************/
static void *opedefGetRamPtr_DataIOTrcSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.DataIOTrcSel[ArrayIdx]);}

const PRM_ATTR_T opedef_DataIOTrcSel = {
    0x110,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    16,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_DataIOTrcSel,          /* Parameter varialbles  */
    &fnCalcDataIoTraceSel,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x120 : データトレース対象軸選択 */
/***************************************************************************/
static void *opedefGetRamPtr_TrcAxisSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrcAxisSel[ArrayIdx]);}

const PRM_ATTR_T opedef_TrcAxisSel = {
    0x120,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    16,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TrcAxisSel,            /* Parameter varialbles  */
    &fnCalcTraceDataSel,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x130 : データトレース対象データ選択 */
/***************************************************************************/
static void *opedefGetRamPtr_TrcDataSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrcDataSel[ArrayIdx]);}

const PRM_ATTR_T opedef_TrcDataSel = {
    0x130,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    16,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TrcDataSel,            /* Parameter varialbles  */
    &fnCalcTraceDataSel,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x140 : I/Oトレース1対象軸選択 */
/***************************************************************************/
static void *opedefGetRamPtr_IOTrcAxisSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.IOTrcAxisSel[ArrayIdx]);}

const PRM_ATTR_T opedef_IOTrcAxisSel = {
    0x140,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    16,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_IOTrcAxisSel,          /* Parameter varialbles  */
    &fnCalcTraceDataSel,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x150 : I/Oトレース1対象データ選択 */
/***************************************************************************/
static void *opedefGetRamPtr_IOTrcDataSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.IOTrcDataSel[ArrayIdx]);}

const PRM_ATTR_T opedef_IOTrcDataSel = {
    0x150,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    16,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_IOTrcDataSel,          /* Parameter varialbles  */
    &fnCalcTraceDataSel,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x160 : トレース実アドレス指定 */
/***************************************************************************/
static void *opedefGetRamPtr_TrcAdr(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrcAdr[ArrayIdx]);}

const PRM_ATTR_T opedef_TrcAdr = {
    0x160,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    4,                                      /* Data Length */
    16,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x00000000,                             /* Default value */
    &opedefGetRamPtr_TrcAdr,                /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x200 : プリトリガデータ数 */
/***************************************************************************/
static void *opedefGetRamPtr_PtrgDnum(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.PtrgDnum);}

const PRM_ATTR_T opedef_PtrgDnum = {
    0x200,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_PtrgDnum,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x201 : トリガ条件 */
/***************************************************************************/
static void *opedefGetRamPtr_TrgSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrgSel);}

const PRM_ATTR_T opedef_TrgSel = {
    0x201,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0019,                                 /* Default value */
    &opedefGetRamPtr_TrgSel,                /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x202 : トリガレベル */
/***************************************************************************/
static void *opedefGetRamPtr_TrgLevel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrgLevel);}

const PRM_ATTR_T opedef_TrgLevel = {
    0x202,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0003,                                 /* Default value */
    &opedefGetRamPtr_TrgLevel,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x204 : トリガ位置 */
/***************************************************************************/
static void *opedefGetRamPtr_TrgPoint(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrgPoint);}

const PRM_ATTR_T opedef_TrgPoint = {
    0x204,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TrgPoint,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x205 : トレース完了 */
/***************************************************************************/
static void *opedefGetRamPtr_TrcState(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.TrcState);}

const PRM_ATTR_T opedef_TrcState = {
    0x205,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TrcState,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x206 : 連続トレース書込みポインタ */
/***************************************************************************/
static void *opedefGetRamPtr_RngTrcPtr(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.RngTrcPtr);}

const PRM_ATTR_T opedef_RngTrcPtr = {
    0x206,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_RngTrcPtr,             /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x208 : マルチトリガモード */
/***************************************************************************/
static void *opedefGetRamPtr_MTrgMode(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.MTrgMode);}

const PRM_ATTR_T opedef_MTrgMode = {
    0x208,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MTrgMode,              /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x209 : トリガ条件2 */
/***************************************************************************/
static void *opedefGetRamPtr_MTrgSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.MTrgSel);}

const PRM_ATTR_T opedef_MTrgSel = {
    0x209,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MTrgSel,               /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x20A : トリガレベル2 */
/***************************************************************************/
static void *opedefGetRamPtr_MTrgLevel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->TrcHndl->TrcReg.MTrgLevel);}

const PRM_ATTR_T opedef_MTrgLevel = {
    0x20A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_MTrgLevel,             /* Parameter varialbles  */
    &fnCalcTraceRegisters,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x240 : アラームトレース実行残回数 */
/***************************************************************************/
const PRM_ATTR_T opedef_AlmTrcNum = {
    0x240,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    3000,                                   /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnAlmTrcRemainingNum,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x241 : アラームトレースメモリ書き込み状態 */
/***************************************************************************/
const PRM_ATTR_T opedef_AlmTrcWrite = {
    0x241,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnAlmTrcWritingStat,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x242 : アラームトレースデータサイズ */
/***************************************************************************/
const PRM_ATTR_T opedef_AlmTrcSize = {
    0x242,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    4096,                                   /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnAlmTrcDataSize,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x243 : アラームトレース停止要求 */
/***************************************************************************/
const PRM_ATTR_T opedef_AlmTrcStopReq = {
    0x243,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnAlmTrcStopReq,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x300 : テーブル運転制御モード */
/***************************************************************************/
static void *opedefGetRamPtr_TblMode(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->TblDrive.OpeReg.TblMode);}

const PRM_ATTR_T opedef_TblMode = {
    0x300,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0001,                                 /* Lower Limit */
    0x0005,                                 /* Upper Limit */
    0x0001,                                 /* Default value */
    &opedefGetRamPtr_TblMode,               /* Parameter varialbles  */
    &fnCalcTblDriveRegisters,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x301 : テーブル運転回転量リミット */
/***************************************************************************/
static void *opedefGetRamPtr_TblLimit(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->TblDrive.OpeReg.TblLimit);}

const PRM_ATTR_T opedef_TblLimit = {
    0x301,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TblLimit,              /* Parameter varialbles  */
    &fnCalcTblDriveRegisters,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x302 : テーブル運転完了 */
/***************************************************************************/
static void *opedefGetRamPtr_TblState(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->TblDrive.OpeReg.TblState);}

const PRM_ATTR_T opedef_TblState = {
    0x302,                                  /* Register Number */
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
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TblState,              /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x303 : テーブル運転機能選択 */
/***************************************************************************/
static void *opedefGetRamPtr_TblFuncSel(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->BaseControls->TblDrive.OpeReg.TblFuncSel);}

const PRM_ATTR_T opedef_TblFuncSel = {
    0x303,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0x0000,                                 /* Lower Limit */
    0xFF03,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &opedefGetRamPtr_TblFuncSel,            /* Parameter varialbles  */
    &fnCalcTblDriveRegisters,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x400 : 振動モニタ共振周波数１ */
/***************************************************************************/
static void *opedefGetRamPtr_VibmRf0Freq(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmRf[0].Freq);}

const PRM_ATTR_T opedef_VibmRf0Freq = {
    0x400,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmRf0Freq,           /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x401 : 振動モニタ共振周波数１Peak値 */
/***************************************************************************/
static void *opedefGetRamPtr_VibmRf0Amp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmRf[0].Amp);}

const PRM_ATTR_T opedef_VibmRf0Amp = {
    0x401,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmRf0Amp,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x402 : 振動モニタ共振周波数２ */
/***************************************************************************/
static void *opedefGetRamPtr_VibmRf1Freq(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmRf[1].Freq);}

const PRM_ATTR_T opedef_VibmRf1Freq = {
    0x402,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmRf1Freq,           /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x403 : 振動モニタ共振周波数２Peak値 */
/***************************************************************************/
static void *opedefGetRamPtr_VibmRf1Amp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmRf[1].Amp);}

const PRM_ATTR_T opedef_VibmRf1Amp = {
    0x403,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmRf1Amp,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x404 : 振動モニタ共振周波数３ */
/***************************************************************************/
static void *opedefGetRamPtr_VibmRf2Freq(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmRf[2].Freq);}

const PRM_ATTR_T opedef_VibmRf2Freq = {
    0x404,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmRf2Freq,           /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x405 : 振動モニタ共振周波数３Peak値 */
/***************************************************************************/
static void *opedefGetRamPtr_VibmRf2Amp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmRf[2].Amp);}

const PRM_ATTR_T opedef_VibmRf2Amp = {
    0x405,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmRf2Amp,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x406 : 振動モニタ反共振周波数１ */
/***************************************************************************/
static void *opedefGetRamPtr_VibmAfFreq(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmAf.Freq);}

const PRM_ATTR_T opedef_VibmAfFreq = {
    0x406,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmAfFreq,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x407 : 振動モニタ反共振周波数１P値 */
/***************************************************************************/
static void *opedefGetRamPtr_VibmAfAmp(UINT ArrayIdx, void *AxParam)
{   return &(((AXIS_HANDLE*)AxParam)->FftAna->FftAnaRslt.VibmAf.Amp);}

const PRM_ATTR_T opedef_VibmAfAmp = {
    0x407,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_VibmAfAmp,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x410 : EasyFFT検出周波数 */
/***************************************************************************/
static void *opedefGetRamPtr_EzFftDetf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->EasyFFT)->OpeReg.EzFftDetf);}

const PRM_ATTR_T opedef_EzFftDetf = {
    0x410,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_EzFftDetf,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x411 : EasyFFTフィルタ周波数１ */
/***************************************************************************/
static void *opedefGetRamPtr_EzFftFil1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->EasyFFT)->OpeReg.EzFftFil1);}

const PRM_ATTR_T opedef_EzFftFil1 = {
    0x411,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_EzFftFil1,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x412 : EasyFFTフィルタ周波数２ */
/***************************************************************************/
static void *opedefGetRamPtr_EzFftFil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->EasyFFT)->OpeReg.EzFftFil2);}

const PRM_ATTR_T opedef_EzFftFil2 = {
    0x412,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_EzFftFil2,             /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x420 : 同定慣性モーメント比 */
/***************************************************************************/
const PRM_ATTR_T opedef_Jrateid = {
    0x420,                                  /* Register Number */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x800 : テストモード設定 */
/***************************************************************************/
const PRM_ATTR_T opedef_TestMode = {
    0x800,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnTestMode,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x801 : ゲイン調　モータ電流検出調整:U相 */
/***************************************************************************/
const PRM_ATTR_T opedef_CurrentDetU = {
    0x801,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    PRMDEF_BASEHEX,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &FnSetCurDetectGainAdjU,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x802 : ゲイン調　モータ電流検出調整:V相 */
/***************************************************************************/
const PRM_ATTR_T opedef_CurrentDetV = {
    0x802,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    PRMDEF_BASEHEX,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
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
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &FnSetCurDetectGainAdjV,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x803 : PN母線電圧 */
/***************************************************************************/
const PRM_ATTR_T opedef_DcVoltx = {
    0x803,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &FnSetDcVoltx,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x804 : 7SEG-LED確認試験切替時間モニタ */
/***************************************************************************/
static void *opedefGetRamPtr_7SegLedTestTimeMon(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->LightIntervalTime);}

const PRM_ATTR_T opedef_7SegLedTestTimeMon = {
    0x804,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    1,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &opedefGetRamPtr_7SegLedTestTimeMon,    /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x805 : オーバヒートＡＤ入力値モニタ */
/***************************************************************************/
const PRM_ATTR_T opedef_InputOHAdMon = {
    0x805,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &FnInputOHAdMon1,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x806 : オーバヒートＡＤ入力値モニタ2 */
/***************************************************************************/
const PRM_ATTR_T opedef_InputOHAdMon2 = {
    0x806,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &FnInputOHAdMon2,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x807 : アナログモニタ出力電圧確認試験 Ch選択 */
/***************************************************************************/
static void *opedefGetRamPtr_AnlgMonVoltTstChSel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->AnlgMonVoltTstChSel);}

const PRM_ATTR_T opedef_AnlgMonVoltTstChSel = {
    0x807,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_AnlgMonVoltTstChSel,   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x808 : アナログモニタ出力電圧確認試験 出力電圧設定(Ch0) */
/***************************************************************************/
static void *opedefGetRamPtr_OutputVoltVal_Ch0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->OutputVoltVal_Ch0);}

const PRM_ATTR_T opedef_OutputVoltVal_Ch0 = {
    0x808,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_OutputVoltVal_Ch0,     /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x809 : アナログモニタ出力電圧確認試験 出力電圧設定(Ch1) */
/***************************************************************************/
static void *opedefGetRamPtr_OutputVoltVal_Ch1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FnCmnCtrl)->OutputVoltVal_Ch1);}

const PRM_ATTR_T opedef_OutputVoltVal_Ch1 = {
    0x809,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    PRMDEF_BASEDEC,                         /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_OutputVoltVal_Ch1,     /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0xC00 : モータ定数データエリア */
/***************************************************************************/
const PRM_ATTR_T opedef_MotorParam = {
    0xC00,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    512,                                    /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnCalcMotorParameter,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x1000 : エンジニアリングエリア */
/***************************************************************************/
const PRM_ATTR_T opedef_EngArea = {
    0x1000,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
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
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnAceessEngneeringInfo,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x10000 : テーブル運転指令入力エリア */
/***************************************************************************/
static void *opedefGetRamPtr_TblCmdArea(UINT ArrayIdx, void *AxParam)
{   return ((USHORT*)((AXIS_HANDLE*)AxParam)->TableRefBuffer+ArrayIdx);}

const PRM_ATTR_T opedef_TblCmdArea = {
    0x1000,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0x1000,                                 /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &opedefGetRamPtr_TblCmdArea,            /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x70000 : EEPROMダイレクトアクセスエリア(IF) */
/***************************************************************************/
const PRM_ATTR_T opedef_EepromIFArea = {
    0x7000,                                 /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0x4000,                                 /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnDirectEepromAccessIF,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x74000 : EEPROMダイレクトアクセスエリア(COA) */
/***************************************************************************/
const PRM_ATTR_T opedef_EepromCnvArea = {
    0x7400,                                 /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0x80,                                   /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnDirectEepromAccessCNV,               /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x74080 : EEPROMダイレクトアクセスエリア(MBB) */
/***************************************************************************/
const PRM_ATTR_T opedef_EepromMBArea = {
    0x7408,                                 /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0x80,                                   /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnDirectEepromAccessMB,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x74800 : EEPROMダイレクトアクセスエリア(SDA) */
/***************************************************************************/
const PRM_ATTR_T opedef_EepromAMPArea = {
    0x7480,                                 /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    0,                                      /* SpUse */
    0,                                      /* reserved */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0x800,                                  /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &fnDirectEepromAccessAMP,               /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL OpeRegTable[] = {
    {0x000   , &opedef_OpeMode                 },/* 0x000: 操作モード */
    {0x001   , &opedef_OpeCmd                  },/* 0x001: 操作設定 */
    {0x002   , &opedef_OpeEnd                  },/* 0x002: 操作終了 */
    {0x003   , &opedef_AccLevel                },/* 0x003: アクセスレベル */
    {0x004   , &opedef_OpeMode2                },/* 0x004: 第２操作モード */
    {0x005   , &opedef_OpeCmd2                 },/* 0x005: 第２操作設定  */
    {0x006   , &opedef_OpeModeTrc              },/* 0x006: トレース操作モード */
    {0x007   , &opedef_OpeCmdTrc               },/* 0x007: トレース操作設定 */
    {0x008   , &opedef_SafetyTime              },/* 0x008: 安全停止時間[s] */
    {0x009   , &opedef_ComTransmissionSpdCmd   },/* 0x009: 通信伝送速度設定（シリアル通信用） */
    {0x00F   , &opedef_OperatorSts             },/* 0x00F: オペレータ表示ステータス */
    {0x017   , &opedef_VibmState               },/* 0x017: オンライン振動モニタ完了確認 */
    {0x020   , &opedef_PrmWrtDis               },/* 0x020: ユーザ定数書込み禁止 */
    {0x030   , &opedef_ZsrchState              },/* 0x030: 原点サーチ完了 */
    {0x031   , &opedef_PjogState               },/* 0x031: プログラムＪＯＧ運転完了確認 */
    {0x032   , &opedef_PdetState               },/* 0x032: 磁極検出完了 */
    {0x033   , &opedef_OrgSetState             },/* 0x033: 原点位置設定完了 */
    {0x034   , &opedef_JogState                },/* 0x034: ＪＯＧ運転完了 */
    {0x043   , &opedef_OutData                 },/* 0x043: 出力信号設定データ */
    {0x050   , &opedef_SpatMode                },/* 0x050: １パラチューニングモード */
    {0x051   , &opedef_SpatLevel               },/* 0x051: １パラチューニングレベル */
    {0x052   , &opedef_SpatFFLevel             },/* 0x052: １パラチューニングＦＦレベル */
    {0x053   , &opedef_SpatLvlChgState         },/* 0x053: １パラメータチューニングレベル変更完了 */
    {0x054   , &opedef_SpatANotchState         },/* 0x054: １パラメータチューニング自動ノッチ完了 */
    {0x055   , &opedef_SpatAresState           },/* 0x055: １パラメータチューニングＡ型制振制御完了確認 */
    {0x060   , &opedef_EzFftMode               },/* 0x060: EasyFFTモード */
    {0x061   , &opedef_EzFftState              },/* 0x061: EasyFFT完了確認 */
    {0x062   , &opedef_EzFftTrq                },/* 0x062: EasyFFTトルク振幅 */
    {0x080   , &opedef_MotPrmOpeAddress        },/* 0x080: モータ定数読み書き先頭アドレス */
    {0x081   , &opedef_MotPrmOpeSize           },/* 0x081: モータ定数読み書きサイズ */
    {0x082   , &opedef_MotPrmOpeState          },/* 0x082: モータ定数読み書き完了 */
    {0x083   , &opedef_MotPrmScaleRW           },/* 0x083: モータ定数スケール読書き対象指定 */
    {0x08A   , &opedef_SvPrmRwOpeAddress       },/* 0x08A: サーボパラメータ格納先先頭アドレス */
    {0x08B   , &opedef_SvPrmRwOpeSize          },/* 0x08B: サーボパラメータデータサイズ */
    {0x08C   , &opedef_SvPrmRwState            },/* 0x08C: サーボパラメータ読み書き完了 */
    {0x090   , &opedef_AdatMode                },/* 0x090: アドバンストＡＴモード */
    {0x091   , &opedef_AdatLevel               },/* 0x091: アドバンストＡＴレベル */
    {0x092   , &opedef_AdatMovep               },/* 0x092: アドバンストＡＴ移動距離 */
    {0x094   , &opedef_AdatState               },/* 0x094: アドバンストＡＴ完了確認 */
    {0x095   , &opedef_AdatFilType             },/* 0x095: アドバンストＡＴフィルタタイプ */
    {0x0A0   , &opedef_AvibMode                },/* 0x0A0: A型制振機能調整モード設定 */
    {0x0A1   , &opedef_AvibState               },/* 0x0A1: A型制振機能調整完了 */
    {0x0A2   , &opedef_AvibFreq                },/* 0x0A2: A型制振機能調整周波数 */
    {0x0A3   , &opedef_AvibGain                },/* 0x0A3: A型制振機能調整ゲイン */
    {0x0B0   , &opedef_MdlVibFreq              },/* 0x0B0: 振動抑制付きMFC調整周波数 */
    {0x0B1   , &opedef_MdlVibState             },/* 0x0B1: 振動抑制付きMFC調整完了 */
    {0x0B2   , &opedef_MdlVibFreq2             },/* 0x0B2: 振動抑制機能チューニング周波数2 */
    {0x0B3   , &opedef_MdlVibFil2              },/* 0x0B3: 振動抑制機能チューニング周波数2補正 */
    {0x0B4   , &opedef_MdlVibState2            },/* 0x0B4: 振動抑制機能設定2完了確認 */
    {0x0B5   , &opedef_RippleCompInfoWriteChk  },/* 0x0B5: 速度リップル補償情報書込み確認 */
    {0x0B6   , &opedef_RippleCompGain          },/* 0x0B6: 速度リップル補償ゲイン */
    {0x0B7   , &opedef_RippleCompCycle1        },/* 0x0B7: 速度リップル補償第1成分周期 */
    {0x0B8   , &opedef_RippleCompAmplitude1    },/* 0x0B8: 速度リップル補償第1成分振幅値 */
    {0x0B9   , &opedef_RippleCompPhase1        },/* 0x0B9: 速度リップル補償第1成分位相 */
    {0x0BA   , &opedef_RippleCompCycle2        },/* 0x0BA: 速度リップル補償第2成分周期 */
    {0x0BB   , &opedef_RippleCompAmplitude2    },/* 0x0BB: 速度リップル補償第2成分振幅値 */
    {0x0BC   , &opedef_RippleCompPhase2        },/* 0x0BC: 速度リップル補償第2成分位相 */
    {0x0BD   , &opedef_RippleCompCycle3        },/* 0x0BD: 速度リップル補償第3成分周期 */
    {0x0BE   , &opedef_RippleCompAmplitude3    },/* 0x0BE: 速度リップル補償第3成分振幅値 */
    {0x0BF   , &opedef_RippleCompPhase3        },/* 0x0BF: 速度リップル補償第3成分位相 */
    {0x0C0   , &opedef_RippleCompCycle4        },/* 0x0C0: 速度リップル補償第4成分周期 */
    {0x0C1   , &opedef_RippleCompAmplitude4    },/* 0x0C1: 速度リップル補償第4成分振幅値 */
    {0x0C2   , &opedef_RippleCompPhase4        },/* 0x0C2: 速度リップル補償第4成分位相 */
    {0x0C3   , &opedef_RippleCompCycle5        },/* 0x0C3: 速度リップル補償第5成分周期 */
    {0x0C4   , &opedef_RippleCompAmplitude5    },/* 0x0C4: 速度リップル補償第5成分振幅値 */
    {0x0C5   , &opedef_RippleCompPhase5        },/* 0x0C5: 速度リップル補償第5成分位相 */
    {0x0C6   , &opedef_RippleCompCycle6        },/* 0x0C6: 速度リップル補償第6成分周期 */
    {0x0C7   , &opedef_RippleCompAmplitude6    },/* 0x0C7: 速度リップル補償第6成分振幅値 */
    {0x0C8   , &opedef_RippleCompPhase6        },/* 0x0C8: 速度リップル補償第6成分位相 */
    {0x0D0   , &opedef_OptCardSel              },/* 0x0D0: オプションカード選択 */
    {0x0E0   , &opedef_DatGnLevel              },/* 0x0E0: ダイナミックオートチューニングゲインレベル */
    {0x0E1   , &opedef_DatJratLevel            },/* 0x0E1: ダイナミックオートチューニング負荷レベル */
    {0x100   , &opedef_TrcMode                 },/* 0x100: トレースモード */
    {0x101   , &opedef_DataSize                },/* 0x101: トレースデータ長 */
    {0x102   , &opedef_AreaSize                },/* 0x102: 使用エリアサイズ */
    {0x103   , &opedef_SampIntv                },/* 0x103: トレースサンプリング間隔 */
    {0x104   , &opedef_SampTime                },/* 0x104: トレースサンプリング時間[us] */
    {0x105   , &opedef_SampLlim                },/* 0x105: トレースサンプリング間隔下限 */
    {0x106   , &opedef_SampUlim                },/* 0x106: トレースサンプリング間隔上限 */
    {0x107   , &opedef_MaxAreaSize             },/* 0x107: 最大エリアサイズ */
    {0x10A   , &opedef_TrcBuffSize             },/* 0x10A: トレースバッファ長 */
    {0x10B   , &opedef_MaxTrcBuffSize          },/* 0x10B: 最大トレースバッファ長 */
    {0x110   , &opedef_DataIOTrcSel            },/* 0x110: データ、I/Oトレース対象選択 */
    {0x120   , &opedef_TrcAxisSel              },/* 0x120: データトレース対象軸選択 */
    {0x130   , &opedef_TrcDataSel              },/* 0x130: データトレース対象データ選択 */
    {0x140   , &opedef_IOTrcAxisSel            },/* 0x140: I/Oトレース1対象軸選択 */
    {0x150   , &opedef_IOTrcDataSel            },/* 0x150: I/Oトレース1対象データ選択 */
    {0x160   , &opedef_TrcAdr                  },/* 0x160: トレース実アドレス指定 */
    {0x200   , &opedef_PtrgDnum                },/* 0x200: プリトリガデータ数 */
    {0x201   , &opedef_TrgSel                  },/* 0x201: トリガ条件 */
    {0x202   , &opedef_TrgLevel                },/* 0x202: トリガレベル */
    {0x204   , &opedef_TrgPoint                },/* 0x204: トリガ位置 */
    {0x205   , &opedef_TrcState                },/* 0x205: トレース完了 */
    {0x206   , &opedef_RngTrcPtr               },/* 0x206: 連続トレース書込みポインタ */
    {0x208   , &opedef_MTrgMode                },/* 0x208: マルチトリガモード */
    {0x209   , &opedef_MTrgSel                 },/* 0x209: トリガ条件2 */
    {0x20A   , &opedef_MTrgLevel               },/* 0x20A: トリガレベル2 */
    {0x240   , &opedef_AlmTrcNum               },/* 0x240: アラームトレース実行残回数 */
    {0x241   , &opedef_AlmTrcWrite             },/* 0x241: アラームトレースメモリ書き込み状態 */
    {0x242   , &opedef_AlmTrcSize              },/* 0x242: アラームトレースデータサイズ */
    {0x243   , &opedef_AlmTrcStopReq           },/* 0x243: アラームトレース停止要求 */
    {0x300   , &opedef_TblMode                 },/* 0x300: テーブル運転制御モード */
    {0x301   , &opedef_TblLimit                },/* 0x301: テーブル運転回転量リミット */
    {0x302   , &opedef_TblState                },/* 0x302: テーブル運転完了 */
    {0x303   , &opedef_TblFuncSel              },/* 0x303: テーブル運転機能選択 */
    {0x400   , &opedef_VibmRf0Freq             },/* 0x400: 振動モニタ共振周波数１ */
    {0x401   , &opedef_VibmRf0Amp              },/* 0x401: 振動モニタ共振周波数１Peak値 */
    {0x402   , &opedef_VibmRf1Freq             },/* 0x402: 振動モニタ共振周波数２ */
    {0x403   , &opedef_VibmRf1Amp              },/* 0x403: 振動モニタ共振周波数２Peak値 */
    {0x404   , &opedef_VibmRf2Freq             },/* 0x404: 振動モニタ共振周波数３ */
    {0x405   , &opedef_VibmRf2Amp              },/* 0x405: 振動モニタ共振周波数３Peak値 */
    {0x406   , &opedef_VibmAfFreq              },/* 0x406: 振動モニタ反共振周波数１ */
    {0x407   , &opedef_VibmAfAmp               },/* 0x407: 振動モニタ反共振周波数１P値 */
    {0x410   , &opedef_EzFftDetf               },/* 0x410: EasyFFT検出周波数 */
    {0x411   , &opedef_EzFftFil1               },/* 0x411: EasyFFTフィルタ周波数１ */
    {0x412   , &opedef_EzFftFil2               },/* 0x412: EasyFFTフィルタ周波数２ */
    {0x420   , &opedef_Jrateid                 },/* 0x420: 同定慣性モーメント比 */
    {0x800   , &opedef_TestMode                },/* 0x800: テストモード設定 */
    {0x801   , &opedef_CurrentDetU             },/* 0x801: ゲイン調　モータ電流検出調整:U相 */
    {0x802   , &opedef_CurrentDetV             },/* 0x802: ゲイン調　モータ電流検出調整:V相 */
    {0x803   , &opedef_DcVoltx                 },/* 0x803: PN母線電圧 */
    {0x804   , &opedef_7SegLedTestTimeMon      },/* 0x804: 7SEG-LED確認試験切替時間モニタ */
    {0x805   , &opedef_InputOHAdMon            },/* 0x805: オーバヒートＡＤ入力値モニタ */
    {0x806   , &opedef_InputOHAdMon2           },/* 0x806: オーバヒートＡＤ入力値モニタ2 */
    {0x807   , &opedef_AnlgMonVoltTstChSel     },/* 0x807: アナログモニタ出力電圧確認試験 Ch選択 */
    {0x808   , &opedef_OutputVoltVal_Ch0       },/* 0x808: アナログモニタ出力電圧確認試験 出力電圧設定(Ch0) */
    {0x809   , &opedef_OutputVoltVal_Ch1       },/* 0x809: アナログモニタ出力電圧確認試験 出力電圧設定(Ch1) */
    {0xC00   , &opedef_MotorParam              },/* 0xC00: モータ定数データエリア */
    {0x1000  , &opedef_EngArea                 },/* 0x1000: エンジニアリングエリア */
    {0x10000 , &opedef_TblCmdArea              },/* 0x10000: テーブル運転指令入力エリア */
    {0x70000 , &opedef_EepromIFArea            },/* 0x70000: EEPROMダイレクトアクセスエリア(IF) */
    {0x74000 , &opedef_EepromCnvArea           },/* 0x74000: EEPROMダイレクトアクセスエリア(COA) */
    {0x74080 , &opedef_EepromMBArea            },/* 0x74080: EEPROMダイレクトアクセスエリア(MBB) */
    {0x74800 , &opedef_EepromAMPArea           },/* 0x74800: EEPROMダイレクトアクセスエリア(SDA) */
    {0xFFFF  , NULL                            },/* 0xFFFF: End of Table List */
};
const ULONG OpeRegTableEntNum = sizeof(OpeRegTable) / sizeof(OpeRegTable[0]);




/******************************* End of File *******************************/

