/****************************************************************************
  Description   :MECHATROLINK ID information Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

    Project       : Mercury

  Create Date   : 2014/04/17

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "NetworkIdRegFunc.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of 0x0000 : 00:サーボパック型式 */
/***************************************************************************/
static void *m2idefGetRamPtr_ID000(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->ModuleID->IfInfo)->ProductName[ArrayIdx]);}

const PRM_ATTR_T m2idef_ID000 = {
    0x0000,                                 /* Register Number */
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
    &m2idefGetRamPtr_ID000,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0010 : 02:サーボパックVer */
/***************************************************************************/
const PRM_ATTR_T m2idef_ID010 = {
    0x0010,                                 /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    ((SERVO_FIRM_VER >> 16) & 0xFF),        /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0020 : 10:モータエンコーダ型式 */
/***************************************************************************/
static void *m2idefGetRamPtr_ID020(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MencV)->EncInfo.Model[ArrayIdx]);}

const PRM_ATTR_T m2idef_ID020 = {
    0x0020,                                 /* Register Number */
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
    &m2idefGetRamPtr_ID020,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0030 : 12:モータエンコーダVer */
/***************************************************************************/
static void *m2idefGetRamPtr_ID030(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms)->Prm->MencP.encver);}

const PRM_ATTR_T m2idef_ID030 = {
    0x0030,                                 /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m2idefGetRamPtr_ID030,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0040 : 20:モータ型式 */
/***************************************************************************/
static void *m2idefGetRamPtr_ID040(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->MencV)->MotInfo.Model[ArrayIdx]);}

const PRM_ATTR_T m2idef_ID040 = {
    0x0040,                                 /* Register Number */
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
    &m2idefGetRamPtr_ID040,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0060 : 30:外部PG型式 */
/***************************************************************************/
static void *m2idefGetRamPtr_ID060(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->FencV)->EncInfo.Model[ArrayIdx]);}

const PRM_ATTR_T m2idef_ID060 = {
    0x0060,                                 /* Register Number */
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
    &m2idefGetRamPtr_ID060,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x0070 : 32:外部エンコーダVer */
/***************************************************************************/
static void *m2idefGetRamPtr_ID070(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OptCardHdl)->FbkOpt.conf.EncVer);}

const PRM_ATTR_T m2idef_ID070 = {
    0x0070,                                 /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m2idefGetRamPtr_ID070,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00A0 : 50:通信IF型式 */
/***************************************************************************/
static void *m2idefGetRamPtr_ID0A0(UINT ArrayIdx, void *AxParam)
{   return &COM_MODEL_STR[ArrayIdx];}

const PRM_ATTR_T m2idef_ID0A0 = {
    0x00A0,                                 /* Register Number */
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
    24,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m2idefGetRamPtr_ID0A0,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00B0 : 52:通信IFVer */
/***************************************************************************/
const PRM_ATTR_T m2idef_ID0B0 = {
    0x00B0,                                 /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    (COM_MODULE_VER & 0x000F),              /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00C0 : 60:Safety Option型式 */
/***************************************************************************/
const PRM_ATTR_T m2idef_ID0C0 = {
    0x00C0,                                 /* Register Number */
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
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00D0 : 62:Safety OptionVer */
/***************************************************************************/
const PRM_ATTR_T m2idef_ID0D0 = {
    0x00D0,                                 /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    NULL,                                   /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00E0 : 70:Feedback Option型式 */
/***************************************************************************/
static void *m2idefGetRamPtr_ID0E0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OptCardHdl)->FbkOpt.conf.OpInfo.ModelStr[ArrayIdx]);}

const PRM_ATTR_T m2idef_ID0E0 = {
    0x00E0,                                 /* Register Number */
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
    24,                                     /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m2idefGetRamPtr_ID0E0,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of 0x00F0 : 72:Feedback OptionVer */
/***************************************************************************/
static void *m2idefGetRamPtr_ID0F0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->OptCardHdl)->FbkOpt.conf.OpInfo.SoftVer);}

const PRM_ATTR_T m2idef_ID0F0 = {
    0x00F0,                                 /* Register Number */
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
    0,                                      /* Lower Limit */
    0,                                      /* Upper Limit */
    0,                                      /* Default value */
    &m2idefGetRamPtr_ID0F0,                 /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL M2IdRegTable[] = {
    {0x0000  , &m2idef_ID000                        },/* 0x0000: 00:サーボパック型式 */
    {0x0010  , &m2idef_ID010                        },/* 0x0010: 02:サーボパックVer */
    {0x0020  , &m2idef_ID020                        },/* 0x0020: 10:モータエンコーダ型式 */
    {0x0030  , &m2idef_ID030                        },/* 0x0030: 12:モータエンコーダVer */
    {0x0040  , &m2idef_ID040                        },/* 0x0040: 20:モータ型式 */
    {0x0060  , &m2idef_ID060                        },/* 0x0060: 30:外部PG型式 */
    {0x0070  , &m2idef_ID070                        },/* 0x0070: 32:外部エンコーダVer */
    {0x00A0  , &m2idef_ID0A0                        },/* 0x00A0: 50:通信IF型式 */
    {0x00B0  , &m2idef_ID0B0                        },/* 0x00B0: 52:通信IFVer */
    {0x00C0  , &m2idef_ID0C0                        },/* 0x00C0: 60:Safety Option型式 */
    {0x00D0  , &m2idef_ID0D0                        },/* 0x00D0: 62:Safety OptionVer */
    {0x00E0  , &m2idef_ID0E0                        },/* 0x00E0: 70:Feedback Option型式 */
    {0x00F0  , &m2idef_ID0F0                        },/* 0x00F0: 72:Feedback OptionVer */
    {0xFFFF  , NULL                                 },/* 0xFFFF: End of Table List */
};
const ULONG M2IdRegTableEntNum = sizeof(M2IdRegTable) / sizeof(M2IdRegTable[0]);




/******************************* End of File *******************************/

