/****************************************************************************
  Description   :MECHATROLINK ID information Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

    Project       : Mercury

  Create Date   : 2014/02/10

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "NetworkIdRegFunc.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of 0x0000 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID000 = {
    0x0000,                                 /* Register Number */
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
    1,                                      /* Data Length */
    4,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0002 : ベンダーIDコード */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID002 = {
    0x0002,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0004 : デバイスコード */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID004 = {
    0x0004,                                 /* Register Number */
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
    SERVO_FIRM_CODE,                        /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0006 : デバイスバージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID006 = {
    0x0006,                                 /* Register Number */
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
    SERVO_FIRM_VER,                         /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0008 : 機器定義(MDI)ファイルバージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID008 = {
    0x0008,                                 /* Register Number */
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
    0x00001000,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000A : 拡張アドレス設定 */
/***************************************************************************/
static void *m3idefGetRamPtr_ID00A(UINT ArrayIdx, void *AxParam)
{   return &NumberOfAxes;}

const PRM_ATTR_T m3idef_ID00A = {
    0x000A,                                 /* Register Number */
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
    1,                                      /* Default value */
    &m3idefGetRamPtr_ID00A,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x000C : シリアル番号 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID00C = {
    0x000C,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &NetworkIdReadSerialNumber,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x001C : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID01C = {
    0x001C,                                 /* Register Number */
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
    1,                                      /* Data Length */
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0020 : プロファイルタイプ１（プライマリ） */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID020 = {
    0x0020,                                 /* Register Number */
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
    0x00000010,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0022 : プロファイルバージョン１（プライマリ） */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID022 = {
    0x0022,                                 /* Register Number */
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
    0x00000030,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0024 : プロファイルタイプ２ */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID024 = {
    0x0024,                                 /* Register Number */
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
    0x000000FF,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0026 : プロファイルバージョン２ */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID026 = {
    0x0026,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0028 : プロファイルタイプ３ */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID028 = {
    0x0028,                                 /* Register Number */
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
    0x000000FF,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002A : プロファイルバージョン３ */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID02A = {
    0x002A,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002C : 伝送周期最小値 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID02C = {
    0x002C,                                 /* Register Number */
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
    12500,                                  /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x002E : 伝送周期最大値 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID02E = {
    0x002E,                                 /* Register Number */
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
    400000,                                 /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0030 : 伝送周期刻み（GRANULARITY） */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID030 = {
    0x0030,                                 /* Register Number */
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
    3,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0032 : 通信周期最小値 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID032 = {
    0x0032,                                 /* Register Number */
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
    12500,                                  /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0034 : 通信周期最大値 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID034 = {
    0x0034,                                 /* Register Number */
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
    3200000,                                /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0036 : 伝送バイト数 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID036 = {
    0x0036,                                 /* Register Number */
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
    0x0000000E,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0038 : 伝送バイト数(現在設定値) */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID038 = {
    0x0038,                                 /* Register Number */
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
    0x0000000E,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    &NetworkIdReadTByteSetting,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x003A : プロファイルタイプ（現在選択値） */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID03A = {
    0x003A,                                 /* Register Number */
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
    0x00000010,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x003C : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID03C = {
    0x003C,                                 /* Register Number */
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
    1,                                      /* Data Length */
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0040 : 通信モード対応 */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID040 = {
    0x0040,                                 /* Register Number */
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
    0x0000000F,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0042 : ＭＡＣアドレス(未サポート) */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID042 = {
    0x0042,                                 /* Register Number */
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
    1,                                      /* Data Length */
    8,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0046 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID046 = {
    0x0046,                                 /* Register Number */
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
    1,                                      /* Data Length */
    52,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0060 : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID060 = {
    0x0060,                                 /* Register Number */
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
    0x6000E079,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0062 : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID062 = {
    0x0062,                                 /* Register Number */
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
    0x36F7001F,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0064 : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID064 = {
    0x0064,                                 /* Register Number */
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
    0x00000003,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0066 : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID066 = {
    0x0066,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0068 : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID068 = {
    0x0068,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x006A : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID06A = {
    0x006A,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x006C : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID06C = {
    0x006C,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x006E : メインコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID06E = {
    0x006E,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0070 : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID070 = {
    0x0070,                                 /* Register Number */
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
    0x60000061,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0072 : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID072 = {
    0x0072,                                 /* Register Number */
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
    0x00010000,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0074 : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID074 = {
    0x0074,                                 /* Register Number */
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
    0x00000003,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0076 : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID076 = {
    0x0076,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0078 : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID078 = {
    0x0078,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x007A : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID07A = {
    0x007A,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x007C : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID07C = {
    0x007C,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x007E : サブコマンド対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID07E = {
    0x007E,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0080 : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID080 = {
    0x0080,                                 /* Register Number */
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
    0x00001FFE,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0082 : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID082 = {
    0x0082,                                 /* Register Number */
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
    0x000003FE,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0084 : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID084 = {
    0x0084,                                 /* Register Number */
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
    0x000003FE,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0086 : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID086 = {
    0x0086,                                 /* Register Number */
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
    0x000000FE,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0088 : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID088 = {
    0x0088,                                 /* Register Number */
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
    0x000FFFFE,                             /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x008A : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID08A = {
    0x008A,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x008C : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID08C = {
    0x008C,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x008E : 共通パラメータ対応リスト */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID08E = {
    0x008E,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0090 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID090 = {
    0x0090,                                 /* Register Number */
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
    1,                                      /* Data Length */
    224,                                    /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0100 : 主デバイス名称 (IF) */
/***************************************************************************/
static void *m3idefGetRamPtr_ID100(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID->IfInfo)->ProductName[ArrayIdx]);}

const PRM_ATTR_T m3idef_ID100 = {
    0x0100,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m3idefGetRamPtr_ID100,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0110 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID110 = {
    0x0110,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0120 : サブデバイス1名称(Motor Encoder) */
/***************************************************************************/
static void *m3idefGetRamPtr_ID120(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MencV)->MotInfo.Model[ArrayIdx]);}

const PRM_ATTR_T m3idef_ID120 = {
    0x0120,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m3idefGetRamPtr_ID120,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0130 : サブデバイス1バージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID130 = {
    0x0130,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0132 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID132 = {
    0x0132,                                 /* Register Number */
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
    1,                                      /* Data Length */
    28,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0140 : サブデバイス2名称(External Encoder) */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID140 = {
    0x0140,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0150 : サブデバイス2バージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID150 = {
    0x0150,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0152 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID152 = {
    0x0152,                                 /* Register Number */
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
    1,                                      /* Data Length */
    28,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0160 : サブデバイス3名称(reserve) */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID160 = {
    0x0160,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0170 : サブデバイス3バージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID170 = {
    0x0170,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0172 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID172 = {
    0x0172,                                 /* Register Number */
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
    1,                                      /* Data Length */
    28,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0180 : サブデバイス4名称(PWM amp) */
/***************************************************************************/
static void *m3idefGetRamPtr_ID180(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID)->SdaInfo.ProductName[ArrayIdx]);}

const PRM_ATTR_T m3idef_ID180 = {
    0x0180,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m3idefGetRamPtr_ID180,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0190 : サブデバイス4バージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID190 = {
    0x0190,                                 /* Register Number */
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
/*  Attribute of 0x0192 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID192 = {
    0x0192,                                 /* Register Number */
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
    1,                                      /* Data Length */
    28,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01A0 : サブデバイス5名称(Converter) */
/***************************************************************************/
static void *m3idefGetRamPtr_ID1A0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID->CoaInfo)->ProductName[ArrayIdx]);}

const PRM_ATTR_T m3idef_ID1A0 = {
    0x01A0,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m3idefGetRamPtr_ID1A0,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01B0 : サブデバイス5バージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID1B0 = {
    0x01B0,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01B2 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID1B2 = {
    0x01B2,                                 /* Register Number */
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
    1,                                      /* Data Length */
    28,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01C0 : サブデバイス6名称(Mount base unit) */
/***************************************************************************/
static void *m3idefGetRamPtr_ID1C0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID->MbuInfo)->ProductName[ArrayIdx]);}

const PRM_ATTR_T m3idef_ID1C0 = {
    0x01C0,                                 /* Register Number */
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
    1,                                      /* Data Length */
    32,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m3idefGetRamPtr_ID1C0,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01D0 : サブデバイス6バージョン */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID1D0 = {
    0x01D0,                                 /* Register Number */
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
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x01D2 : Reserve */
/***************************************************************************/
const PRM_ATTR_T m3idef_ID1D2 = {
    0x01D2,                                 /* Register Number */
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
    1,                                      /* Data Length */
    15450,                                  /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL M3IdRegTable[] = {
    {0x0000  , &m3idef_ID000                   },/* 0x0000: Reserve */
    {0x0002  , &m3idef_ID002                   },/* 0x0002: ベンダーIDコード */
    {0x0004  , &m3idef_ID004                   },/* 0x0004: デバイスコード */
    {0x0006  , &m3idef_ID006                   },/* 0x0006: デバイスバージョン */
    {0x0008  , &m3idef_ID008                   },/* 0x0008: 機器定義(MDI)ファイルバージョン */
    {0x000A  , &m3idef_ID00A                   },/* 0x000A: 拡張アドレス設定 */
    {0x000C  , &m3idef_ID00C                   },/* 0x000C: シリアル番号 */
    {0x001C  , &m3idef_ID01C                   },/* 0x001C: Reserve */
    {0x0020  , &m3idef_ID020                   },/* 0x0020: プロファイルタイプ１（プライマリ） */
    {0x0022  , &m3idef_ID022                   },/* 0x0022: プロファイルバージョン１（プライマリ） */
    {0x0024  , &m3idef_ID024                   },/* 0x0024: プロファイルタイプ２ */
    {0x0026  , &m3idef_ID026                   },/* 0x0026: プロファイルバージョン２ */
    {0x0028  , &m3idef_ID028                   },/* 0x0028: プロファイルタイプ３ */
    {0x002A  , &m3idef_ID02A                   },/* 0x002A: プロファイルバージョン３ */
    {0x002C  , &m3idef_ID02C                   },/* 0x002C: 伝送周期最小値 */
    {0x002E  , &m3idef_ID02E                   },/* 0x002E: 伝送周期最大値 */
    {0x0030  , &m3idef_ID030                   },/* 0x0030: 伝送周期刻み（GRANULARITY） */
    {0x0032  , &m3idef_ID032                   },/* 0x0032: 通信周期最小値 */
    {0x0034  , &m3idef_ID034                   },/* 0x0034: 通信周期最大値 */
    {0x0036  , &m3idef_ID036                   },/* 0x0036: 伝送バイト数 */
    {0x0038  , &m3idef_ID038                   },/* 0x0038: 伝送バイト数(現在設定値) */
    {0x003A  , &m3idef_ID03A                   },/* 0x003A: プロファイルタイプ（現在選択値） */
    {0x003C  , &m3idef_ID03C                   },/* 0x003C: Reserve */
    {0x0040  , &m3idef_ID040                   },/* 0x0040: 通信モード対応 */
    {0x0042  , &m3idef_ID042                   },/* 0x0042: ＭＡＣアドレス(未サポート) */
    {0x0046  , &m3idef_ID046                   },/* 0x0046: Reserve */
    {0x0060  , &m3idef_ID060                   },/* 0x0060: メインコマンド対応リスト */
    {0x0062  , &m3idef_ID062                   },/* 0x0062: メインコマンド対応リスト */
    {0x0064  , &m3idef_ID064                   },/* 0x0064: メインコマンド対応リスト */
    {0x0066  , &m3idef_ID066                   },/* 0x0066: メインコマンド対応リスト */
    {0x0068  , &m3idef_ID068                   },/* 0x0068: メインコマンド対応リスト */
    {0x006A  , &m3idef_ID06A                   },/* 0x006A: メインコマンド対応リスト */
    {0x006C  , &m3idef_ID06C                   },/* 0x006C: メインコマンド対応リスト */
    {0x006E  , &m3idef_ID06E                   },/* 0x006E: メインコマンド対応リスト */
    {0x0070  , &m3idef_ID070                   },/* 0x0070: サブコマンド対応リスト */
    {0x0072  , &m3idef_ID072                   },/* 0x0072: サブコマンド対応リスト */
    {0x0074  , &m3idef_ID074                   },/* 0x0074: サブコマンド対応リスト */
    {0x0076  , &m3idef_ID076                   },/* 0x0076: サブコマンド対応リスト */
    {0x0078  , &m3idef_ID078                   },/* 0x0078: サブコマンド対応リスト */
    {0x007A  , &m3idef_ID07A                   },/* 0x007A: サブコマンド対応リスト */
    {0x007C  , &m3idef_ID07C                   },/* 0x007C: サブコマンド対応リスト */
    {0x007E  , &m3idef_ID07E                   },/* 0x007E: サブコマンド対応リスト */
    {0x0080  , &m3idef_ID080                   },/* 0x0080: 共通パラメータ対応リスト */
    {0x0082  , &m3idef_ID082                   },/* 0x0082: 共通パラメータ対応リスト */
    {0x0084  , &m3idef_ID084                   },/* 0x0084: 共通パラメータ対応リスト */
    {0x0086  , &m3idef_ID086                   },/* 0x0086: 共通パラメータ対応リスト */
    {0x0088  , &m3idef_ID088                   },/* 0x0088: 共通パラメータ対応リスト */
    {0x008A  , &m3idef_ID08A                   },/* 0x008A: 共通パラメータ対応リスト */
    {0x008C  , &m3idef_ID08C                   },/* 0x008C: 共通パラメータ対応リスト */
    {0x008E  , &m3idef_ID08E                   },/* 0x008E: 共通パラメータ対応リスト */
    {0x0090  , &m3idef_ID090                   },/* 0x0090: Reserve */
    {0x0100  , &m3idef_ID100                   },/* 0x0100: 主デバイス名称 (IF) */
    {0x0110  , &m3idef_ID110                   },/* 0x0110: Reserve */
    {0x0120  , &m3idef_ID120                   },/* 0x0120: サブデバイス1名称(Motor Encoder) */
    {0x0130  , &m3idef_ID130                   },/* 0x0130: サブデバイス1バージョン */
    {0x0132  , &m3idef_ID132                   },/* 0x0132: Reserve */
    {0x0140  , &m3idef_ID140                   },/* 0x0140: サブデバイス2名称(External Encoder) */
    {0x0150  , &m3idef_ID150                   },/* 0x0150: サブデバイス2バージョン */
    {0x0152  , &m3idef_ID152                   },/* 0x0152: Reserve */
    {0x0160  , &m3idef_ID160                   },/* 0x0160: サブデバイス3名称(reserve) */
    {0x0170  , &m3idef_ID170                   },/* 0x0170: サブデバイス3バージョン */
    {0x0172  , &m3idef_ID172                   },/* 0x0172: Reserve */
    {0x0180  , &m3idef_ID180                   },/* 0x0180: サブデバイス4名称(PWM amp) */
    {0x0190  , &m3idef_ID190                   },/* 0x0190: サブデバイス4バージョン */
    {0x0192  , &m3idef_ID192                   },/* 0x0192: Reserve */
    {0x01A0  , &m3idef_ID1A0                   },/* 0x01A0: サブデバイス5名称(Converter) */
    {0x01B0  , &m3idef_ID1B0                   },/* 0x01B0: サブデバイス5バージョン */
    {0x01B2  , &m3idef_ID1B2                   },/* 0x01B2: Reserve */
    {0x01C0  , &m3idef_ID1C0                   },/* 0x01C0: サブデバイス6名称(Mount base unit) */
    {0x01D0  , &m3idef_ID1D0                   },/* 0x01D0: サブデバイス6バージョン */
    {0x01D2  , &m3idef_ID1D2                   },/* 0x01D2: Reserve */
    {0xFFFF  , NULL                            },/* 0xFFFF: End of Table List */
};
const ULONG M3IdRegTableEntNum = sizeof(M3IdRegTable) / sizeof(M3IdRegTable[0]);




/******************************* End of File *******************************/

