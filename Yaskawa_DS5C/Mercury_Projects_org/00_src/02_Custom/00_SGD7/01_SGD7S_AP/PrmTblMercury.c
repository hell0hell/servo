/****************************************************************************
  Description   : Parameter List Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2013
                  All Rights Reserved

  Project       : Mercury
  Product       : SGD7S-AP

  Create Date   : 2014/04/09

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PnPrmCalFunc.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of Pn000 : 機能選択基本スイッチ０ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm0);}

const PRM_ATTR_T pndef_b_prm0 = {
    0x000,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    68,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x10B1,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm0,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn001 : 機能選択応用スイッチ１ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm1);}

const PRM_ATTR_T pndef_b_prm1 = {
    0x001,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    72,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1142,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm1,                 /* Pointer to Parameter varialble  */
    &pncal_b_prm1,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn002 : 機能選択応用スイッチ２ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm2);}

const PRM_ATTR_T pndef_b_prm2 = {
    0x002,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    76,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x4213,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm2,                 /* Pointer to Parameter varialble  */
    &pncal_b_prm2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn006 : 機能選択応用スイッチ６ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->b_prm6);}

const PRM_ATTR_T pndef_b_prm6 = {
    0x006,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    38,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x105F,                                 /* Upper Limit */
    0x0002,                                 /* Default value */
    &pndefGetRamPtr_b_prm6,                 /* Pointer to Parameter varialble  */
    &pncal_b_prm6,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn007 : 機能選択応用スイッチ７ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm7(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->b_prm7);}

const PRM_ATTR_T pndef_b_prm7 = {
    0x007,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    42,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x105F,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm7,                 /* Pointer to Parameter varialble  */
    &pncal_b_prm7,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn008 : 機能選択応用スイッチ８ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm8(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm8);}

const PRM_ATTR_T pndef_b_prm8 = {
    0x008,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    80,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x7121,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm8,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn009 : 機能選択応用スイッチ９ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm9(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm9);}

const PRM_ATTR_T pndef_b_prm9 = {
    0x009,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    84,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0111,                                 /* Upper Limit */
    0x0010,                                 /* Default value */
    &pndefGetRamPtr_b_prm9,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn00A : 機能選択応用スイッチA */
/***************************************************************************/
static void *pndefGetRamPtr_b_prmA(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prmA);}

const PRM_ATTR_T pndef_b_prmA = {
    0x00A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    88,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0044,                                 /* Upper Limit */
    0x0001,                                 /* Default value */
    &pndefGetRamPtr_b_prmA,                 /* Pointer to Parameter varialble  */
    &pncal_b_prmA,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn00B : 機能選択応用スイッチB */
/***************************************************************************/
static void *pndefGetRamPtr_b_prmB(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prmB);}

const PRM_ATTR_T pndef_b_prmB = {
    0x00B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    92,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1121,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prmB,                 /* Pointer to Parameter varialble  */
    &pncal_b_prmB,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn00C : 機能選択応用スイッチC */
/***************************************************************************/
static void *pndefGetRamPtr_b_prmC(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prmC);}

const PRM_ATTR_T pndef_b_prmC = {
    0x00C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    96,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0131,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prmC,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn00D : 機能選択応用スイッチD */
/***************************************************************************/
static void *pndefGetRamPtr_b_prmD(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prmD);}

const PRM_ATTR_T pndef_b_prmD = {
    0x00D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    100,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1001,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prmD,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn00E : 機能選択応用スイッチE */
/***************************************************************************/
static void *pndefGetRamPtr_b_prmE(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->b_prmE);}

const PRM_ATTR_T pndef_b_prmE = {
    0x00E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    46,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2010,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prmE,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn00F : 機能選択応用スイッチF */
/***************************************************************************/
static void *pndefGetRamPtr_b_prmF(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->b_prmF);}

const PRM_ATTR_T pndef_b_prmF = {
    0x00F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    50,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2011,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prmF,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn010 : スロット0軸アドレス */
/***************************************************************************/
static void *pndefGetRamPtr_AxisAddress0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->AxisAddress[0]);}

const PRM_ATTR_T pndef_AxisAddress0 = {
    0x010,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    54,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    0x007F,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_AxisAddress0,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn011 : スロット1軸アドレス */
/***************************************************************************/
static void *pndefGetRamPtr_AxisAddress1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->AxisAddress[1]);}

const PRM_ATTR_T pndef_AxisAddress1 = {
    0x011,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    58,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    0x007F,                                 /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_AxisAddress1,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn020 : 桁毎ユニット共通パラメータ設定 */
/***************************************************************************/
static void *pndefGetRamPtr_UniPrmSel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->UniPrmSel);}

const PRM_ATTR_T pndef_UniPrmSel = {
    0x020,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    62,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    0x0011,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_UniPrmSel,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn041 : スキャン周期切り替えスイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_CycleSwitch(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->CycleSwitch);}

const PRM_ATTR_T pndef_CycleSwitch = {
    0x041,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    66,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0001,                                 /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_CycleSwitch,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn080 : 機能選択応用スイッチ８０ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm80(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm80);}

const PRM_ATTR_T pndef_b_prm80 = {
    0x080,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    104,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1111,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm80,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn081 : 機能選択応用スイッチ８１ */
/***************************************************************************/
static void *pndefGetRamPtr_b_prm81(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->b_prm81);}

const PRM_ATTR_T pndef_b_prm81 = {
    0x081,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    108,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1111,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_b_prm81,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn100 : 速度ループゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_loophz(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->loophz);}

const PRM_ATTR_T pndef_loophz = {
    0x100,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    112,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_loophz,                 /* Pointer to Parameter varialble  */
    &pncal_loophz,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn101 : 速度ループ積分時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_pitime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pitime);}

const PRM_ATTR_T pndef_pitime = {
    0x101,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    116,                                    /* EEPROM address */
    15,                                     /* Lower Limit */
    51200,                                  /* Upper Limit */
    2000,                                   /* Default value */
    &pndefGetRamPtr_pitime,                 /* Pointer to Parameter varialble  */
    &pncal_pitime,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn102 : 位置ループゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_posgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->posgn);}

const PRM_ATTR_T pndef_posgn = {
    0x102,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    120,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_posgn,                  /* Pointer to Parameter varialble  */
    &pncal_posgn,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn103 : 慣性モーメント比 */
/***************************************************************************/
static void *pndefGetRamPtr_jrate(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jrate);}

const PRM_ATTR_T pndef_jrate = {
    0x103,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    124,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    20000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_jrate,                  /* Pointer to Parameter varialble  */
    &pncal_jrate,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn104 : 第２速度ループゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_loophz2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->loophz2);}

const PRM_ATTR_T pndef_loophz2 = {
    0x104,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    128,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_loophz2,                /* Pointer to Parameter varialble  */
    &pncal_loophz2,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn105 : 第２速度ループ積分時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_pitime2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pitime2);}

const PRM_ATTR_T pndef_pitime2 = {
    0x105,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    132,                                    /* EEPROM address */
    15,                                     /* Lower Limit */
    51200,                                  /* Upper Limit */
    2000,                                   /* Default value */
    &pndefGetRamPtr_pitime2,                /* Pointer to Parameter varialble  */
    &pncal_pitime2,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn106 : 第２位置ループゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_posgn2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->posgn2);}

const PRM_ATTR_T pndef_posgn2 = {
    0x106,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    136,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_posgn2,                 /* Pointer to Parameter varialble  */
    &pncal_posgn2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn109 : フィードフォワード */
/***************************************************************************/
static void *pndefGetRamPtr_ffgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ffgn);}

const PRM_ATTR_T pndef_ffgn = {
    0x109,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    140,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ffgn,                   /* Pointer to Parameter varialble  */
    &pncal_ffgn,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn10A : フィードフォワードフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_fffil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fffil);}

const PRM_ATTR_T pndef_fffil = {
    0x10A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    144,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    6400,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_fffil,                  /* Pointer to Parameter varialble  */
    &pncal_fffil,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn10B : ゲイン関係応用スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_gnmode(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gnmode);}

const PRM_ATTR_T pndef_gnmode = {
    0x10B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    148,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x5334,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_gnmode,                 /* Pointer to Parameter varialble  */
    &pncal_gnmode,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn10C : モードスイッチ(トルク指令) */
/***************************************************************************/
static void *pndefGetRamPtr_mdswlv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdswlv);}

const PRM_ATTR_T pndef_mdswlv = {
    0x10C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    152,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    200,                                    /* Default value */
    &pndefGetRamPtr_mdswlv,                 /* Pointer to Parameter varialble  */
    &pncal_mdswlv,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn10D : モードスイッチ(速度指令) */
/***************************************************************************/
static void *pndefGetRamPtr_mswrf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mswrf);}

const PRM_ATTR_T pndef_mswrf = {
    0x10D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    156,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mswrf,                  /* Pointer to Parameter varialble  */
    &pncal_mswrf,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn10E : モードスイッチ(加速度) */
/***************************************************************************/
static void *pndefGetRamPtr_mswacc(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mswacc);}

const PRM_ATTR_T pndef_mswacc = {
    0x10E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    160,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    30000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mswacc,                 /* Pointer to Parameter varialble  */
    &pncal_mswacc,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn10F : モードスイッチ(位置偏差) */
/***************************************************************************/
static void *pndefGetRamPtr_msplvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->msplvl);}

const PRM_ATTR_T pndef_msplvl = {
    0x10F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    164,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_msplvl,                 /* Pointer to Parameter varialble  */
    &pncal_msplvl,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn11F : 位置積分時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_pitfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pitfil);}

const PRM_ATTR_T pndef_pitfil = {
    0x11F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    168,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    50000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pitfil,                 /* Pointer to Parameter varialble  */
    &pncal_pitfil,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn121 : 摩擦補償ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_dobgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dobgn);}

const PRM_ATTR_T pndef_dobgn = {
    0x121,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    172,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_dobgn,                  /* Pointer to Parameter varialble  */
    &pncal_dobgn,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn122 : 第２摩擦補償ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_dobgn2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dobgn2);}

const PRM_ATTR_T pndef_dobgn2 = {
    0x122,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    176,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_dobgn2,                 /* Pointer to Parameter varialble  */
    &pncal_dobgn2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn123 : 摩擦補償係数 */
/***************************************************************************/
static void *pndefGetRamPtr_dtrqgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dtrqgn);}

const PRM_ATTR_T pndef_dtrqgn = {
    0x123,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    180,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dtrqgn,                 /* Pointer to Parameter varialble  */
    &pncal_dtrqgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn124 : 摩擦補償周波数補正 */
/***************************************************************************/
static void *pndefGetRamPtr_dlpfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dlpfil);}

const PRM_ATTR_T pndef_dlpfil = {
    0x124,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    184,                                    /* EEPROM address */
    -10000,                                 /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dlpfil,                 /* Pointer to Parameter varialble  */
    &pncal_dlpfil,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn125 : 摩擦補償ゲイン補正 */
/***************************************************************************/
static void *pndefGetRamPtr_dobjgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dobjgn);}

const PRM_ATTR_T pndef_dobjgn = {
    0x125,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    188,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_dobjgn,                 /* Pointer to Parameter varialble  */
    &pncal_dobjgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn127 : 速度オブザーバゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_evobgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->evobgn);}

const PRM_ATTR_T pndef_evobgn = {
    0x127,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    192,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    500,                                    /* Upper Limit */
    40,                                     /* Default value */
    &pndefGetRamPtr_evobgn,                 /* Pointer to Parameter varialble  */
    &pncal_evobgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn128 : 速度オブザーバ位置補償ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_evobjgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->evobjgn);}

const PRM_ATTR_T pndef_evobjgn = {
    0x128,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    196,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    150,                                    /* Default value */
    &pndefGetRamPtr_evobjgn,                /* Pointer to Parameter varialble  */
    &pncal_evobjgn,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn131 : ゲイン切り替え時間１ */
/***************************************************************************/
static void *pndefGetRamPtr_gnswfil1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gnswfil1);}

const PRM_ATTR_T pndef_gnswfil1 = {
    0x131,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    200,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gnswfil1,               /* Pointer to Parameter varialble  */
    &pncal_gnswfil1,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn132 : ゲイン切り替え時間２ */
/***************************************************************************/
static void *pndefGetRamPtr_gnswfil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gnswfil2);}

const PRM_ATTR_T pndef_gnswfil2 = {
    0x132,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    204,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gnswfil2,               /* Pointer to Parameter varialble  */
    &pncal_gnswfil2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn135 : ゲイン切り替え待ち時間１ */
/***************************************************************************/
static void *pndefGetRamPtr_gnswwait1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gnswwait1);}

const PRM_ATTR_T pndef_gnswwait1 = {
    0x135,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    208,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gnswwait1,              /* Pointer to Parameter varialble  */
    &pncal_gnswwait1,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn136 : ゲイン切り替え待ち時間２ */
/***************************************************************************/
static void *pndefGetRamPtr_gnswwait2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gnswwait2);}

const PRM_ATTR_T pndef_gnswwait2 = {
    0x136,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    212,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gnswwait2,              /* Pointer to Parameter varialble  */
    &pncal_gnswwait2,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn139 : 自動ゲイン切り替え関係スイッチ１ */
/***************************************************************************/
static void *pndefGetRamPtr_gnsw1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gnsw1);}

const PRM_ATTR_T pndef_gnsw1 = {
    0x139,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    216,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0052,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_gnsw1,                  /* Pointer to Parameter varialble  */
    &pncal_gnsw1,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn13B : 速度オブザーバローパスフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_Pn13B(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->Pn13B);}

const PRM_ATTR_T pndef_Pn13B = {
    0x13B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    220,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    601,                                    /* Default value */
    &pndefGetRamPtr_Pn13B,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn13D : 電流ゲインレベル */
/***************************************************************************/
static void *pndefGetRamPtr_KcGainRatio(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->KcGainRatio);}

const PRM_ATTR_T pndef_KcGainRatio = {
    0x13D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    224,                                    /* EEPROM address */
    100,                                    /* Lower Limit */
    2000,                                   /* Upper Limit */
    2000,                                   /* Default value */
    &pndefGetRamPtr_KcGainRatio,            /* Pointer to Parameter varialble  */
    &pncal_KcGainRatio,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn140 : モデル追従制御関連スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_mdlsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlsw);}

const PRM_ATTR_T pndef_mdlsw = {
    0x140,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    228,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1121,                                 /* Upper Limit */
    0x0100,                                 /* Default value */
    &pndefGetRamPtr_mdlsw,                  /* Pointer to Parameter varialble  */
    &pncal_mdlsw,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn141 : モデル追従制御ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_mdlgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlgn);}

const PRM_ATTR_T pndef_mdlgn = {
    0x141,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    232,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_mdlgn,                  /* Pointer to Parameter varialble  */
    &pncal_mdlgn,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn142 : モデル追従制御ゲイン補正 */
/***************************************************************************/
static void *pndefGetRamPtr_mdlzeta(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlzeta);}

const PRM_ATTR_T pndef_mdlzeta = {
    0x142,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    236,                                    /* EEPROM address */
    500,                                    /* Lower Limit */
    2000,                                   /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_mdlzeta,                /* Pointer to Parameter varialble  */
    &pncal_mdlzeta,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn143 : モデル追従制御バイアス(正転方向) */
/***************************************************************************/
static void *pndefGetRamPtr_mdlptff(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlptff);}

const PRM_ATTR_T pndef_mdlptff = {
    0x143,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    240,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_mdlptff,                /* Pointer to Parameter varialble  */
    &pncal_mdlptff,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn144 : モデル追従制御バイアス(逆転方向) */
/***************************************************************************/
static void *pndefGetRamPtr_mdlntff(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlntff);}

const PRM_ATTR_T pndef_mdlntff = {
    0x144,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    244,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_mdlntff,                /* Pointer to Parameter varialble  */
    &pncal_mdlntff,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn145 : 振動抑制１周波数Ａ */
/***************************************************************************/
static void *pndefGetRamPtr_mdlantfrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlantfrq);}

const PRM_ATTR_T pndef_mdlantfrq = {
    0x145,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    248,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    2500,                                   /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_mdlantfrq,              /* Pointer to Parameter varialble  */
    &pncal_mdlantfrq,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn146 : 振動抑制１周波数Ｂ */
/***************************************************************************/
static void *pndefGetRamPtr_mdlresfrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlresfrq);}

const PRM_ATTR_T pndef_mdlresfrq = {
    0x146,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    252,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    2500,                                   /* Upper Limit */
    700,                                    /* Default value */
    &pndefGetRamPtr_mdlresfrq,              /* Pointer to Parameter varialble  */
    &pncal_mdlresfrq,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn147 : モデル追従制御速度フィードフォワード補償 */
/***************************************************************************/
static void *pndefGetRamPtr_mdlvff(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlvff);}

const PRM_ATTR_T pndef_mdlvff = {
    0x147,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    256,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_mdlvff,                 /* Pointer to Parameter varialble  */
    &pncal_mdlvff,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn148 : 第２モデル追従制御ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_mdlgn2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlgn2);}

const PRM_ATTR_T pndef_mdlgn2 = {
    0x148,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    260,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_mdlgn2,                 /* Pointer to Parameter varialble  */
    &pncal_mdlgn2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn149 : 第２モデル追従制御ゲイン補正 */
/***************************************************************************/
static void *pndefGetRamPtr_mdlzeta2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mdlzeta2);}

const PRM_ATTR_T pndef_mdlzeta2 = {
    0x149,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    264,                                    /* EEPROM address */
    500,                                    /* Lower Limit */
    2000,                                   /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_mdlzeta2,               /* Pointer to Parameter varialble  */
    &pncal_mdlzeta2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn14A : 振動抑制２周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_ff_frq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ff_frq);}

const PRM_ATTR_T pndef_ff_frq = {
    0x14A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    268,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    2000,                                   /* Upper Limit */
    800,                                    /* Default value */
    &pndefGetRamPtr_ff_frq,                 /* Pointer to Parameter varialble  */
    &pncal_ff_frq,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn14B : 振動抑制２補正 */
/***************************************************************************/
static void *pndefGetRamPtr_ff_fil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ff_fil);}

const PRM_ATTR_T pndef_ff_fil = {
    0x14B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    272,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_ff_fil,                 /* Pointer to Parameter varialble  */
    &pncal_ff_fil,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn14F : 制御関連スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_mfctype(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mfctype);}

const PRM_ATTR_T pndef_mfctype = {
    0x14F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    276,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0021,                                 /* Upper Limit */
    0x0021,                                 /* Default value */
    &pndefGetRamPtr_mfctype,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn160 : 制振制御関連スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_avibsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibsw);}

const PRM_ATTR_T pndef_avibsw = {
    0x160,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    280,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0011,                                 /* Upper Limit */
    0x0010,                                 /* Default value */
    &pndefGetRamPtr_avibsw,                 /* Pointer to Parameter varialble  */
    &pncal_avibsw,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn161 : Ａ型制振周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_avibfrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibfrq);}

const PRM_ATTR_T pndef_avibfrq = {
    0x161,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    284,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_avibfrq,                /* Pointer to Parameter varialble  */
    &pncal_avibfrq,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn162 : Ａ型制振ゲイン補正 */
/***************************************************************************/
static void *pndefGetRamPtr_avibgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibgn);}

const PRM_ATTR_T pndef_avibgn = {
    0x162,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    288,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_avibgn,                 /* Pointer to Parameter varialble  */
    &pncal_avibgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn163 : Ａ型制振ダンピングゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_avibdamp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibdamp);}

const PRM_ATTR_T pndef_avibdamp = {
    0x163,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    292,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    300,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_avibdamp,               /* Pointer to Parameter varialble  */
    &pncal_avibdamp,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn164 : Ａ型制振フィルタ時定数１補正 */
/***************************************************************************/
static void *pndefGetRamPtr_avibfil1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibfil1);}

const PRM_ATTR_T pndef_avibfil1 = {
    0x164,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    296,                                    /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_avibfil1,               /* Pointer to Parameter varialble  */
    &pncal_avibfil1,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn165 : Ａ型制振フィルタ時定数２補正 */
/***************************************************************************/
static void *pndefGetRamPtr_avibfil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibfil2);}

const PRM_ATTR_T pndef_avibfil2 = {
    0x165,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    300,                                    /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_avibfil2,               /* Pointer to Parameter varialble  */
    &pncal_avibfil2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn166 : Ａ型制振ダンピングゲイン2 */
/***************************************************************************/
static void *pndefGetRamPtr_avibdamp2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibdamp2);}

const PRM_ATTR_T pndef_avibdamp2 = {
    0x166,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    304,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_avibdamp2,              /* Pointer to Parameter varialble  */
    &pncal_avibdamp2,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn167 : A型制振ハイパスフィルタ遮断周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_avibfrq2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->avibfrq2);}

const PRM_ATTR_T pndef_avibfrq2 = {
    0x167,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    308,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    50000,                                  /* Upper Limit */
    20000,                                  /* Default value */
    &pndefGetRamPtr_avibfrq2,               /* Pointer to Parameter varialble  */
    &pncal_avibfrq2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn170 : ダイナミックオートチューニング関連スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_DatLevel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->DatLevel);}

const PRM_ATTR_T pndef_DatLevel = {
    0x170,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    312,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2711,                                 /* Upper Limit */
    0x1401,                                 /* Default value */
    &pndefGetRamPtr_DatLevel,               /* Pointer to Parameter varialble  */
    &pncal_DatLevel,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn17A : 予約パラメータ（イナーシャ比切り替え関係スイッチ） */
/***************************************************************************/
static void *pndefGetRamPtr_jrateSw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jrateSw);}

const PRM_ATTR_T pndef_jrateSw = {
    0x17A,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    316,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0001,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_jrateSw,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn17B : 第2イナーシャ比 */
/***************************************************************************/
static void *pndefGetRamPtr_jrate2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jrate2);}

const PRM_ATTR_T pndef_jrate2 = {
    0x17B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    320,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    20000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_jrate2,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn17C : イナーシャ比切り替え時間１ */
/***************************************************************************/
static void *pndefGetRamPtr_jrateChgTime1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jrateChgTime1);}

const PRM_ATTR_T pndef_jrateChgTime1 = {
    0x17C,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    324,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_jrateChgTime1,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn17D : イナーシャ比切り替え時間２ */
/***************************************************************************/
static void *pndefGetRamPtr_jrateChgTime2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jrateChgTime2);}

const PRM_ATTR_T pndef_jrateChgTime2 = {
    0x17D,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    328,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_jrateChgTime2,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn181 : モードスイッチ(速度指令) */
/***************************************************************************/
static void *pndefGetRamPtr_mswrf_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mswrf_l);}

const PRM_ATTR_T pndef_mswrf_l = {
    0x181,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    332,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mswrf_l,                /* Pointer to Parameter varialble  */
    &pncal_mswrf_l,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn182 : モードスイッチ(加速度) */
/***************************************************************************/
static void *pndefGetRamPtr_mswacc_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mswacc_l);}

const PRM_ATTR_T pndef_mswacc_l = {
    0x182,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    336,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    30000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mswacc_l,               /* Pointer to Parameter varialble  */
    &pncal_mswacc_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn190 : 予約パラメータ（偏差レス�U関連スイッチ） */
/***************************************************************************/
static void *pndefGetRamPtr_divlessSw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->divlessSw);}

const PRM_ATTR_T pndef_divlessSw = {
    0x190,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    340,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0011,                                 /* Upper Limit */
    0x0010,                                 /* Default value */
    &pndefGetRamPtr_divlessSw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn191 : 速度フィードフォワードゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_velffgain(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->velffgain);}

const PRM_ATTR_T pndef_velffgain = {
    0x191,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    344,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_velffgain,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn192 : トルクフィードフォワードゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_trqffgain(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->trqffgain);}

const PRM_ATTR_T pndef_trqffgain = {
    0x192,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    348,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_trqffgain,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn193 : トルクフィードフォワードフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_trqfftime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->trqfftime);}

const PRM_ATTR_T pndef_trqfftime = {
    0x193,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    352,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    80,                                     /* Default value */
    &pndefGetRamPtr_trqfftime,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn200 : 位置制御指令形態選択スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_prstsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->prstsw);}

const PRM_ATTR_T pndef_prstsw = {
    0x200,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    356,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2236,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_prstsw,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn205 : マルチターンリミット設定 */
/***************************************************************************/
static void *pndefGetRamPtr_mtlmt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mtlmt);}

const PRM_ATTR_T pndef_mtlmt = {
    0x205,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    360,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    65535,                                  /* Default value */
    &pndefGetRamPtr_mtlmt,                  /* Pointer to Parameter varialble  */
    &pncal_mtlmt,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn207 : 位置指令機能スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_cmfilsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->cmfilsw);}

const PRM_ATTR_T pndef_cmfilsw = {
    0x207,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    364,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2210,                                 /* Upper Limit */
    0x0010,                                 /* Default value */
    &pndefGetRamPtr_cmfilsw,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn209 : 位置指令加減速バイアス */
/***************************************************************************/
static void *pndefGetRamPtr_expbias1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->expbias1);}

const PRM_ATTR_T pndef_expbias1 = {
    0x209,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    368,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_expbias1,               /* Pointer to Parameter varialble  */
    &pncal_expbias1,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn20A : 外付けスケールピッチ数 */
/***************************************************************************/
static void *pndefGetRamPtr_fcplsml(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fcplsml);}

const PRM_ATTR_T pndef_fcplsml = {
    0x20A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_FENC | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    372,                                    /* EEPROM address */
    4,                                      /* Lower Limit */
    1048576,                                /* Upper Limit */
    32768,                                  /* Default value */
    &pndefGetRamPtr_fcplsml,                /* Pointer to Parameter varialble  */
    &pncal_fcplsml,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn20E : 電子ギア比(分子) */
/***************************************************************************/
static void *pndefGetRamPtr_ratb2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ratb2l);}

const PRM_ATTR_T pndef_ratb2l = {
    0x20E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    380,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_ratb2l,                 /* Pointer to Parameter varialble  */
    &pncal_ratb2l,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn210 : 電子ギア比(分母) */
/***************************************************************************/
static void *pndefGetRamPtr_rata2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->rata2l);}

const PRM_ATTR_T pndef_rata2l = {
    0x210,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    388,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_rata2l,                 /* Pointer to Parameter varialble  */
    &pncal_rata2l,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn212 : ＰＧ分周比(パルス単位) */
/***************************************************************************/
static void *pndefGetRamPtr_pgrat2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pgrat2l);}

const PRM_ATTR_T pndef_pgrat2l = {
    0x212,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    396,                                    /* EEPROM address */
    16,                                     /* Lower Limit */
    1073741824,                             /* Upper Limit */
    2048,                                   /* Default value */
    &pndefGetRamPtr_pgrat2l,                /* Pointer to Parameter varialble  */
    &pncal_pgrat2l,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn216 : 位置指令加減速時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_expfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->expfil);}

const PRM_ATTR_T pndef_expfil = {
    0x216,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    404,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_expfil,                 /* Pointer to Parameter varialble  */
    &pncal_expfil,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn217 : 位置指令移動平均時間 */
/***************************************************************************/
static void *pndefGetRamPtr_mafil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mafil);}

const PRM_ATTR_T pndef_mafil = {
    0x217,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    408,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mafil,                  /* Pointer to Parameter varialble  */
    &pncal_mafil,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn218 : 指令パルス入力倍率 */
/***************************************************************************/
static void *pndefGetRamPtr_MulFrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MulFrq);}

const PRM_ATTR_T pndef_MulFrq = {
    0x218,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    412,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_MulFrq,                 /* Pointer to Parameter varialble  */
    &pncal_mulfrq,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn21B : 位置指令高次フィルタスムージング割合 */
/***************************************************************************/
static void *pndefGetRamPtr_prefHfilsmooth(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->prefHfilsmooth);}

const PRM_ATTR_T pndef_prefHfilsmooth = {
    0x21B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    416,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_prefHfilsmooth,         /* Pointer to Parameter varialble  */
    &pncal_highrad,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn21C : 位置指令高次フィルタ加減速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_prefHfilacctime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->prefHfilacctime);}

const PRM_ATTR_T pndef_prefHfilacctime = {
    0x21C,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    420,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_prefHfilacctime,        /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn22A : フルクローズ制御選択スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_fcsel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fcsel);}

const PRM_ATTR_T pndef_fcsel = {
    0x22A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_FENC | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    424,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1003,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_fcsel,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn230 : 位置制御拡張機能スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_cmctrlexsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->cmctrlexsw);}

const PRM_ATTR_T pndef_cmctrlexsw = {
    0x230,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    428,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0001,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_cmctrlexsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn231 : バックラッシ補正量 */
/***************************************************************************/
static void *pndefGetRamPtr_blcm2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->blcm2l);}

const PRM_ATTR_T pndef_blcm2l = {
    0x231,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    432,                                    /* EEPROM address */
    -500000,                                /* Lower Limit */
    500000,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_blcm2l,                 /* Pointer to Parameter varialble  */
    &pncal_blcm2l,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn233 : バックラッシ補正時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_blfiltime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->blfiltime);}

const PRM_ATTR_T pndef_blfiltime = {
    0x233,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    440,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_blfiltime,              /* Pointer to Parameter varialble  */
    &pncal_blfiltime,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn240 : 位置偏差クリア信号入力最小時間間隔 */
/***************************************************************************/
static void *pndefGetRamPtr_perrclrsigtime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->perrclrsigtime);}

const PRM_ATTR_T pndef_perrclrsigtime = {
    0x240,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    444,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    2000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_perrclrsigtime,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn250 : エンコーダ通信速度選択 */
/***************************************************************************/
static void *pndefGetRamPtr_encbaudrate(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->encbaudrate);}

const PRM_ATTR_T pndef_encbaudrate = {
    0x250,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    448,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    0x0002,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_encbaudrate,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn281 : エンコーダ出力分解能 */
/***************************************************************************/
static void *pndefGetRamPtr_scalepgrat(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->scalepgrat);}

const PRM_ATTR_T pndef_scalepgrat = {
    0x281,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    (KPI_CHKSPUSE_LINEAR | KPI_CHKSPUSE_FENC) | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    452,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    4096,                                   /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_scalepgrat,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn282 : リニアスケールピッチ */
/***************************************************************************/
static void *pndefGetRamPtr_scalepitch2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->scalepitch2l);}

const PRM_ATTR_T pndef_scalepitch2l = {
    0x282,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    456,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    6553600,                                /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_scalepitch2l,           /* Pointer to Parameter varialble  */
    &pncal_scalepitch2l,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn2D0 : 予約パラメータ（位置センサ出力数） */
/***************************************************************************/
static void *pndefGetRamPtr_psencernum(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->psencernum);}

const PRM_ATTR_T pndef_psencernum = {
    0x2D0,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    464,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    16777216,                               /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_psencernum,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn300 : 速度指令入力ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_vrefgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vrefgn);}

const PRM_ATTR_T pndef_vrefgn = {
    0x300,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    472,                                    /* EEPROM address */
    150,                                    /* Lower Limit */
    3000,                                   /* Upper Limit */
    600,                                    /* Default value */
    &pndefGetRamPtr_vrefgn,                 /* Pointer to Parameter varialble  */
    &pncal_vrefgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn301 : 内部設定速度１(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_speed1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->speed1);}

const PRM_ATTR_T pndef_speed1 = {
    0x301,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    476,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_speed1,                 /* Pointer to Parameter varialble  */
    &pncal_speed1,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn302 : 内部設定速度２(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_speed2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->speed2);}

const PRM_ATTR_T pndef_speed2 = {
    0x302,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    480,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    200,                                    /* Default value */
    &pndefGetRamPtr_speed2,                 /* Pointer to Parameter varialble  */
    &pncal_speed2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn303 : 内部設定速度３(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_speed3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->speed3);}

const PRM_ATTR_T pndef_speed3 = {
    0x303,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    484,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    300,                                    /* Default value */
    &pndefGetRamPtr_speed3,                 /* Pointer to Parameter varialble  */
    &pncal_speed3,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn304 : ＪＯＧ速度(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_jogspd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jogspd);}

const PRM_ATTR_T pndef_jogspd = {
    0x304,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    488,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_jogspd,                 /* Pointer to Parameter varialble  */
    &pncal_jogspd,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn305 : ソフトスタート加速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_sfsacc(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->sfsacc);}

const PRM_ATTR_T pndef_sfsacc = {
    0x305,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    492,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_sfsacc,                 /* Pointer to Parameter varialble  */
    &pncal_sfsacc,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn306 : ソフトスタート減速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_sfsdec(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->sfsdec);}

const PRM_ATTR_T pndef_sfsdec = {
    0x306,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    496,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_sfsdec,                 /* Pointer to Parameter varialble  */
    &pncal_sfsdec,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn307 : 速度指令フィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_srfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->srfil);}

const PRM_ATTR_T pndef_srfil = {
    0x307,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    500,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    40,                                     /* Default value */
    &pndefGetRamPtr_srfil,                  /* Pointer to Parameter varialble  */
    &pncal_srfil,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn308 : 速度Ｆ/Ｂフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_fbfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fbfil);}

const PRM_ATTR_T pndef_fbfil = {
    0x308,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    504,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_fbfil,                  /* Pointer to Parameter varialble  */
    &pncal_fbfil,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn30A : サーボオフ及び強制停止時の減速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_svoffdectime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->svoffdectime);}

const PRM_ATTR_T pndef_svoffdectime = {
    0x30A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    508,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_svoffdectime,           /* Pointer to Parameter varialble  */
    &pncal_svoffdectime,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn30B : 速度指令移動平均時間 */
/***************************************************************************/
static void *pndefGetRamPtr_vrefmafil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vrefmafil);}

const PRM_ATTR_T pndef_vrefmafil = {
    0x30B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    512,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_vrefmafil,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn30C : 速度フィードフォワード移動平均時間 */
/***************************************************************************/
static void *pndefGetRamPtr_vffmafil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vffmafil);}

const PRM_ATTR_T pndef_vffmafil = {
    0x30C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    516,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    5100,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_vffmafil,               /* Pointer to Parameter varialble  */
    &pncal_vffmafil,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn310 : 振動検出スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_vibsel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vibsel);}

const PRM_ATTR_T pndef_vibsel = {
    0x310,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    520,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0002,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_vibsel,                 /* Pointer to Parameter varialble  */
    &pncal_vibsel,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn311 : 振動検出感度 */
/***************************************************************************/
static void *pndefGetRamPtr_vibsens(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vibsens);}

const PRM_ATTR_T pndef_vibsens = {
    0x311,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    524,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    500,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_vibsens,                /* Pointer to Parameter varialble  */
    &pncal_vibsens,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn312 : 振動検出レベル(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_vibnorm(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vibnorm);}

const PRM_ATTR_T pndef_vibnorm = {
    0x312,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    528,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    5000,                                   /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_vibnorm,                /* Pointer to Parameter varialble  */
    &pncal_vibnorm,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn316 : モータ最高速度(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_maxvel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->maxvel);}

const PRM_ATTR_T pndef_maxvel = {
    0x316,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    532,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_maxvel,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn324 : 慣性モーメント同定開始レベル */
/***************************************************************************/
static void *pndefGetRamPtr_Pn324(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->Pn324);}

const PRM_ATTR_T pndef_Pn324 = {
    0x324,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    536,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    20000,                                  /* Upper Limit */
    300,                                    /* Default value */
    &pndefGetRamPtr_Pn324,                  /* Pointer to Parameter varialble  */
    &pncal_Pn324,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn325 : 慣性モーメント同定中の振動検出レベル(回転) */
/***************************************************************************/
static void *pndefGetRamPtr_Pn325(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->Pn325);}

const PRM_ATTR_T pndef_Pn325 = {
    0x325,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    540,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    5000,                                   /* Upper Limit */
    250,                                    /* Default value */
    &pndefGetRamPtr_Pn325,                  /* Pointer to Parameter varialble  */
    &pncal_Pn325,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn380 : 内部設定速度１(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_speed1_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->speed1_l);}

const PRM_ATTR_T pndef_speed1_l = {
    0x380,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    544,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_speed1_l,               /* Pointer to Parameter varialble  */
    &pncal_speed1_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn381 : 内部設定速度２(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_speed2_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->speed2_l);}

const PRM_ATTR_T pndef_speed2_l = {
    0x381,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    548,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_speed2_l,               /* Pointer to Parameter varialble  */
    &pncal_speed2_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn382 : 内部設定速度３(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_speed3_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->speed3_l);}

const PRM_ATTR_T pndef_speed3_l = {
    0x382,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    552,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_speed3_l,               /* Pointer to Parameter varialble  */
    &pncal_speed3_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn383 : ＪＯＧ速度(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_jogspd_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->jogspd_l);}

const PRM_ATTR_T pndef_jogspd_l = {
    0x383,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    556,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_jogspd_l,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn384 : 振動検出レベル(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_vibnorm_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vibnorm_l);}

const PRM_ATTR_T pndef_vibnorm_l = {
    0x384,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    560,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    5000,                                   /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_vibnorm_l,              /* Pointer to Parameter varialble  */
    &pncal_vibnorm_l,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn385 : モータ最高速度(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_maxvel_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->maxvel_l);}

const PRM_ATTR_T pndef_maxvel_l = {
    0x385,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    564,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_maxvel_l,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn386 : 慣性モーメント同定中の振動検出レベル(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_Pn387(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->Pn387);}

const PRM_ATTR_T pndef_Pn387 = {
    0x386,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    568,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    5000,                                   /* Upper Limit */
    125,                                    /* Default value */
    &pndefGetRamPtr_Pn387,                  /* Pointer to Parameter varialble  */
    &pncal_Pn387,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn400 : トルク指令入力ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_trefgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->trefgn);}

const PRM_ATTR_T pndef_trefgn = {
    0x400,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    572,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_trefgn,                 /* Pointer to Parameter varialble  */
    &pncal_trefgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn401 : １段目第１トルク指令フィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_trqfil11(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->trqfil11);}

const PRM_ATTR_T pndef_trqfil11 = {
    0x401,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    576,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_trqfil11,               /* Pointer to Parameter varialble  */
    &pncal_trqfil11,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn402 : 正転トルク制限 */
/***************************************************************************/
static void *pndefGetRamPtr_tlmtf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tlmtf);}

const PRM_ATTR_T pndef_tlmtf = {
    0x402,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    580,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    800,                                    /* Default value */
    &pndefGetRamPtr_tlmtf,                  /* Pointer to Parameter varialble  */
    &pncal_tlmtf,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn403 : 逆転トルク制限 */
/***************************************************************************/
static void *pndefGetRamPtr_tlmtr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tlmtr);}

const PRM_ATTR_T pndef_tlmtr = {
    0x403,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    584,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    800,                                    /* Default value */
    &pndefGetRamPtr_tlmtr,                  /* Pointer to Parameter varialble  */
    &pncal_tlmtr,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn404 : 正転側外部トルク制限 */
/***************************************************************************/
static void *pndefGetRamPtr_outlmf1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outlmf1);}

const PRM_ATTR_T pndef_outlmf1 = {
    0x404,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    588,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_outlmf1,                /* Pointer to Parameter varialble  */
    &pncal_outlmf1,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn405 : 逆転側外部トルク制限 */
/***************************************************************************/
static void *pndefGetRamPtr_outlmr1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outlmr1);}

const PRM_ATTR_T pndef_outlmr1 = {
    0x405,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    592,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_outlmr1,                /* Pointer to Parameter varialble  */
    &pncal_outlmr1,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn406 : 非常停止トルク */
/***************************************************************************/
static void *pndefGetRamPtr_emgtrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->emgtrq);}

const PRM_ATTR_T pndef_emgtrq = {
    0x406,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    596,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    800,                                    /* Default value */
    &pndefGetRamPtr_emgtrq,                 /* Pointer to Parameter varialble  */
    &pncal_emgtrq,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn407 : トルク制御時の速度制限 */
/***************************************************************************/
static void *pndefGetRamPtr_tcrlmt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tcrlmt);}

const PRM_ATTR_T pndef_tcrlmt = {
    0x407,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    600,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_tcrlmt,                 /* Pointer to Parameter varialble  */
    &pncal_tcrlmt,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn408 : トルク関係機能スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_tfuncsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tfuncsw);}

const PRM_ATTR_T pndef_tfuncsw = {
    0x408,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    604,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1111,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_tfuncsw,                /* Pointer to Parameter varialble  */
    &pncal_tfuncsw,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn409 : １段目ノッチフィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchfil1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchfil1);}

const PRM_ATTR_T pndef_ntchfil1 = {
    0x409,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    608,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    5000,                                   /* Default value */
    &pndefGetRamPtr_ntchfil1,               /* Pointer to Parameter varialble  */
    &pncal_ntchfil1,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn40A : １段目ノッチフィルタＱ値 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchq1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchq1);}

const PRM_ATTR_T pndef_ntchq1 = {
    0x40A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    612,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    70,                                     /* Default value */
    &pndefGetRamPtr_ntchq1,                 /* Pointer to Parameter varialble  */
    &pncal_ntchq1,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn40B : １段目ノッチフィルタ深さ */
/***************************************************************************/
static void *pndefGetRamPtr_ntchdamp1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchdamp1);}

const PRM_ATTR_T pndef_ntchdamp1 = {
    0x40B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    616,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ntchdamp1,              /* Pointer to Parameter varialble  */
    &pncal_ntchdamp1,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn40C : ２段目ノッチフィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchfil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchfil2);}

const PRM_ATTR_T pndef_ntchfil2 = {
    0x40C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    620,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    5000,                                   /* Default value */
    &pndefGetRamPtr_ntchfil2,               /* Pointer to Parameter varialble  */
    &pncal_ntchfil2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn40D : ２段目ノッチフィルタＱ値 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchq2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchq2);}

const PRM_ATTR_T pndef_ntchq2 = {
    0x40D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    624,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    70,                                     /* Default value */
    &pndefGetRamPtr_ntchq2,                 /* Pointer to Parameter varialble  */
    &pncal_ntchq2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn40E : ２段目ノッチフィルタ深さ */
/***************************************************************************/
static void *pndefGetRamPtr_ntchdamp2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchdamp2);}

const PRM_ATTR_T pndef_ntchdamp2 = {
    0x40E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    628,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ntchdamp2,              /* Pointer to Parameter varialble  */
    &pncal_ntchdamp2,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn40F : ２段目２次トルク指令フィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_secfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->secfil);}

const PRM_ATTR_T pndef_secfil = {
    0x40F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    632,                                    /* EEPROM address */
    100,                                    /* Lower Limit */
    5000,                                   /* Upper Limit */
    5000,                                   /* Default value */
    &pndefGetRamPtr_secfil,                 /* Pointer to Parameter varialble  */
    &pncal_secfil,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn410 : ２段目２次トルク指令フィルタＱ値 */
/***************************************************************************/
static void *pndefGetRamPtr_secq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->secq);}

const PRM_ATTR_T pndef_secq = {
    0x410,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    636,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_secq,                   /* Pointer to Parameter varialble  */
    &pncal_secq,                            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn412 : １段目第２トルク指令フィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_trqfil12(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->trqfil12);}

const PRM_ATTR_T pndef_trqfil12 = {
    0x412,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    640,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_trqfil12,               /* Pointer to Parameter varialble  */
    &pncal_trqfil12,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn415 : T-REFフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_treffil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->treffil);}

const PRM_ATTR_T pndef_treffil = {
    0x415,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    644,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_treffil,                /* Pointer to Parameter varialble  */
    &pncal_treffil,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn416 : トルク(推力)関係機能スイッチ２ */
/***************************************************************************/
static void *pndefGetRamPtr_tfuncsw2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tfuncsw2);}

const PRM_ATTR_T pndef_tfuncsw2 = {
    0x416,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    648,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1111,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_tfuncsw2,               /* Pointer to Parameter varialble  */
    &pncal_tfuncsw2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn417 : 3段目ノッチフィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchfil3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchfil3);}

const PRM_ATTR_T pndef_ntchfil3 = {
    0x417,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    652,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    5000,                                   /* Default value */
    &pndefGetRamPtr_ntchfil3,               /* Pointer to Parameter varialble  */
    &pncal_ntchfil3,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn418 : 3段目ノッチフィルタQ値 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchq3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchq3);}

const PRM_ATTR_T pndef_ntchq3 = {
    0x418,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    656,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    70,                                     /* Default value */
    &pndefGetRamPtr_ntchq3,                 /* Pointer to Parameter varialble  */
    &pncal_ntchq3,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn419 : 3段目ノッチフィルタ深さ */
/***************************************************************************/
static void *pndefGetRamPtr_ntchdamp3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchdamp3);}

const PRM_ATTR_T pndef_ntchdamp3 = {
    0x419,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    660,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ntchdamp3,              /* Pointer to Parameter varialble  */
    &pncal_ntchdamp3,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn41A : 4段目ノッチフィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchfil4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchfil4);}

const PRM_ATTR_T pndef_ntchfil4 = {
    0x41A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    664,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    5000,                                   /* Default value */
    &pndefGetRamPtr_ntchfil4,               /* Pointer to Parameter varialble  */
    &pncal_ntchfil4,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn41B : 4段目ノッチフィルタQ値 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchq4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchq4);}

const PRM_ATTR_T pndef_ntchq4 = {
    0x41B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    668,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    70,                                     /* Default value */
    &pndefGetRamPtr_ntchq4,                 /* Pointer to Parameter varialble  */
    &pncal_ntchq4,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn41C : 4段目ノッチフィルタ深さ */
/***************************************************************************/
static void *pndefGetRamPtr_ntchdamp4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchdamp4);}

const PRM_ATTR_T pndef_ntchdamp4 = {
    0x41C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    672,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ntchdamp4,              /* Pointer to Parameter varialble  */
    &pncal_ntchdamp4,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn41D : 5段目ノッチフィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchfil5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchfil5);}

const PRM_ATTR_T pndef_ntchfil5 = {
    0x41D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    676,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    5000,                                   /* Default value */
    &pndefGetRamPtr_ntchfil5,               /* Pointer to Parameter varialble  */
    &pncal_ntchfil5,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn41E : 5段目ノッチフィルタQ値 */
/***************************************************************************/
static void *pndefGetRamPtr_ntchq5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchq5);}

const PRM_ATTR_T pndef_ntchq5 = {
    0x41E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    680,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    70,                                     /* Default value */
    &pndefGetRamPtr_ntchq5,                 /* Pointer to Parameter varialble  */
    &pncal_ntchq5,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn41F : 5段目ノッチフィルタ深さ */
/***************************************************************************/
static void *pndefGetRamPtr_ntchdamp5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ntchdamp5);}

const PRM_ATTR_T pndef_ntchdamp5 = {
    0x41F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    684,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ntchdamp5,              /* Pointer to Parameter varialble  */
    &pncal_ntchdamp5,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn423 : 速度リップル補償機能選択スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompSw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompSw);}

const PRM_ATTR_T pndef_RippleCompSw = {
    0x423,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    688,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1111,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_RippleCompSw,           /* Pointer to Parameter varialble  */
    &pncal_RippleCompSW,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn424 : 主回路電圧降下時トルク制限 */
/***************************************************************************/
static void *pndefGetRamPtr_UvTrqLmt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->UvTrqLmt);}

const PRM_ATTR_T pndef_UvTrqLmt = {
    0x424,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    692,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_UvTrqLmt,               /* Pointer to Parameter varialble  */
    &pncal_UvTrqLmt,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn425 : 主回路電圧降下時トルク制限解除時間 */
/***************************************************************************/
static void *pndefGetRamPtr_UvTrqLmtOffFil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->UvTrqLmtOffFil);}

const PRM_ATTR_T pndef_UvTrqLmtOffFil = {
    0x425,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    696,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_UvTrqLmtOffFil,         /* Pointer to Parameter varialble  */
    &pncal_UvTrqLmt,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn426 : トルクフィードフォワード移動平均時間 */
/***************************************************************************/
static void *pndefGetRamPtr_tffmafil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tffmafil);}

const PRM_ATTR_T pndef_tffmafil = {
    0x426,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    700,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    5100,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_tffmafil,               /* Pointer to Parameter varialble  */
    &pncal_tffmafil,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn427 : 速度リップル補償有効速度 */
/***************************************************************************/
static void *pndefGetRamPtr_VelRippleCompSpd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->VelRippleCompSpd);}

const PRM_ATTR_T pndef_VelRippleCompSpd = {
    0x427,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    704,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_VelRippleCompSpd,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompSpd,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn453 : 掃引開始周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_swpminfrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->swpminfrq);}

const PRM_ATTR_T pndef_swpminfrq = {
    0x453,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    708,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    5000,                                   /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_swpminfrq,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn454 : 掃引終了周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_swpmaxfrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->swpmaxfrq);}

const PRM_ATTR_T pndef_swpmaxfrq = {
    0x454,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    4000,                                   /* Default value */
    &pndefGetRamPtr_swpmaxfrq,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn455 : 検出共振周波数下限値 */
/***************************************************************************/
static void *pndefGetRamPtr_detfrqlmt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->detfrqlmt);}

const PRM_ATTR_T pndef_detfrqlmt = {
    0x455,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    712,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    5000,                                   /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_detfrqlmt,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn456 : 掃引トルク指令振幅 */
/***************************************************************************/
static void *pndefGetRamPtr_ezffttrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ezffttrq);}

const PRM_ATTR_T pndef_ezffttrq = {
    0x456,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    716,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    15,                                     /* Default value */
    &pndefGetRamPtr_ezffttrq,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn457 : ＦＦＴ関係機能スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_ezfftsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ezfftsw);}

const PRM_ATTR_T pndef_ezfftsw = {
    0x457,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    720,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2742,                                 /* Upper Limit */
    0x210,                                  /* Default value */
    &pndefGetRamPtr_ezfftsw,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn460 : ノッチフィルタ調整スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_anotchsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->anotchsw);}

const PRM_ATTR_T pndef_anotchsw = {
    0x460,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    724,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0101,                                 /* Upper Limit */
    0x0101,                                 /* Default value */
    &pndefGetRamPtr_anotchsw,               /* Pointer to Parameter varialble  */
    &pncal_anotchsw,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn466 : ノッチフィルタ調整スイッチ2 */
/***************************************************************************/
static void *pndefGetRamPtr_anotchsw2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->anotchsw2);}

const PRM_ATTR_T pndef_anotchsw2 = {
    0x466,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    728,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0111,                                 /* Upper Limit */
    0x0111,                                 /* Default value */
    &pndefGetRamPtr_anotchsw2,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn470 : 重力補償トルク */
/***************************************************************************/
static void *pndefGetRamPtr_gcomptrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gcomptrq);}

const PRM_ATTR_T pndef_gcomptrq = {
    0x470,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    732,                                    /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gcomptrq,               /* Pointer to Parameter varialble  */
    &pncal_gcomptrq,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn471 : 正方向クーロン摩擦補償トルク */
/***************************************************************************/
static void *pndefGetRamPtr_pqfriqcomptrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pqfriqcomptrq);}

const PRM_ATTR_T pndef_pqfriqcomptrq = {
    0x471,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    736,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pqfriqcomptrq,          /* Pointer to Parameter varialble  */
    &pncal_pqfriqcomptrq,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn472 : 負方向クーロン摩擦補償トルク */
/***************************************************************************/
static void *pndefGetRamPtr_nqfriqcomptrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->nqfriqcomptrq);}

const PRM_ATTR_T pndef_nqfriqcomptrq = {
    0x472,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    740,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_nqfriqcomptrq,          /* Pointer to Parameter varialble  */
    &pncal_nqfriqcomptrq,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn473 : 粘性摩擦補償トルク */
/***************************************************************************/
static void *pndefGetRamPtr_vfrigcomptrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vfrigcomptrq);}

const PRM_ATTR_T pndef_vfrigcomptrq = {
    0x473,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    744,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    3000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_vfrigcomptrq,           /* Pointer to Parameter varialble  */
    &pncal_vfrigcomptrq,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn474 : 内部設定速度hysteresis */
/***************************************************************************/
static void *pndefGetRamPtr_spdhys(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->spdhys);}

const PRM_ATTR_T pndef_spdhys = {
    0x474,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    748,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_spdhys,                 /* Pointer to Parameter varialble  */
    &pncal_spdhys,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn480 : 推力制御時の速度制限 */
/***************************************************************************/
static void *pndefGetRamPtr_tcrlmt_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tcrlmt_l);}

const PRM_ATTR_T pndef_tcrlmt_l = {
    0x480,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    752,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_tcrlmt_l,               /* Pointer to Parameter varialble  */
    &pncal_tcrlmt_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn481 : 磁極検出速度ループゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_pdetloophz(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetloophz);}

const PRM_ATTR_T pndef_pdetloophz = {
    0x481,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    756,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    20000,                                  /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_pdetloophz,             /* Pointer to Parameter varialble  */
    &pncal_pdetloophz,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn482 : 磁極検出速度ループ積分時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetpitime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetpitime);}

const PRM_ATTR_T pndef_pdetpitime = {
    0x482,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    760,                                    /* EEPROM address */
    15,                                     /* Lower Limit */
    51200,                                  /* Upper Limit */
    3000,                                   /* Default value */
    &pndefGetRamPtr_pdetpitime,             /* Pointer to Parameter varialble  */
    &pncal_pdetpitime,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn483 : 正転推力制限 */
/***************************************************************************/
static void *pndefGetRamPtr_tlmtf_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tlmtf_l);}

const PRM_ATTR_T pndef_tlmtf_l = {
    0x483,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    764,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_tlmtf_l,                /* Pointer to Parameter varialble  */
    &pncal_tlmtf_l,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn484 : 逆転推力制限 */
/***************************************************************************/
static void *pndefGetRamPtr_tlmtr_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tlmtr_l);}

const PRM_ATTR_T pndef_tlmtr_l = {
    0x484,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    768,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_tlmtr_l,                /* Pointer to Parameter varialble  */
    &pncal_tlmtr_l,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn485 : 磁極検出指令速度 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetmaxspd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetmaxspd);}

const PRM_ATTR_T pndef_pdetmaxspd = {
    0x485,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    772,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_pdetmaxspd,             /* Pointer to Parameter varialble  */
    &pncal_pdetmaxspd,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn486 : 磁極検出指令加減速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetmaxt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetmaxt);}

const PRM_ATTR_T pndef_pdetmaxt = {
    0x486,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    776,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    25,                                     /* Default value */
    &pndefGetRamPtr_pdetmaxt,               /* Pointer to Parameter varialble  */
    &pncal_pdetmaxt,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn487 : 磁極検出指令一定速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetclmpt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetclmpt);}

const PRM_ATTR_T pndef_pdetclmpt = {
    0x487,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    780,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    300,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pdetclmpt,              /* Pointer to Parameter varialble  */
    &pncal_pdetclmpt,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn488 : 磁極検出指令待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetwait(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetwait);}

const PRM_ATTR_T pndef_pdetwait = {
    0x488,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    784,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    500,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_pdetwait,               /* Pointer to Parameter varialble  */
    &pncal_pdetwait,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn48D : 磁極検出繰り返し回数 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetrepeat(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetrepeat);}

const PRM_ATTR_T pndef_pdetrepeat = {
    0x48D,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    788,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    10,                                     /* Upper Limit */
    4,                                      /* Default value */
    &pndefGetRamPtr_pdetrepeat,             /* Pointer to Parameter varialble  */
    &pncal_pdetrepeat,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn48E : 磁極検出可動範囲 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetdistok(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetdistok);}

const PRM_ATTR_T pndef_pdetdistok = {
    0x48E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    792,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_pdetdistok,             /* Pointer to Parameter varialble  */
    &pncal_pdetdistok,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn490 : 磁極検出負荷レベル */
/***************************************************************************/
static void *pndefGetRamPtr_pdetjrate(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetjrate);}

const PRM_ATTR_T pndef_pdetjrate = {
    0x490,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    796,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    20000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_pdetjrate,              /* Pointer to Parameter varialble  */
    &pncal_pdetjrate,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn491 : 磁極検出積分レスゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_pdetintegless(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetintegless);}

const PRM_ATTR_T pndef_pdetintegless = {
    0x491,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    800,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_pdetintegless,          /* Pointer to Parameter varialble  */
    &pncal_pdetintegless,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn492 : 磁極検出積分レス開始時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetinteglesst(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetinteglesst);}

const PRM_ATTR_T pndef_pdetinteglesst = {
    0x492,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    804,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_pdetinteglesst,         /* Pointer to Parameter varialble  */
    &pncal_pdetinteglesst,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn493 : 磁極検出指令速度 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetmaxspd_r(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetmaxspd_r);}

const PRM_ATTR_T pndef_pdetmaxspd_r = {
    0x493,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    808,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_pdetmaxspd_r,           /* Pointer to Parameter varialble  */
    &pncal_pdetmaxspd_r,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn494 : 磁極検出可動範囲 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetdistok_r(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetdistok_r);}

const PRM_ATTR_T pndef_pdetdistok_r = {
    0x494,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    812,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    250,                                    /* Default value */
    &pndefGetRamPtr_pdetdistok_r,           /* Pointer to Parameter varialble  */
    &pncal_pdetdistok_r,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn495 : 磁極検出確認トルク指令 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetmaxtrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetmaxtrq);}

const PRM_ATTR_T pndef_pdetmaxtrq = {
    0x495,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    816,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    200,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_pdetmaxtrq,             /* Pointer to Parameter varialble  */
    &pncal_pdetmaxtrq,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn496 : 磁極検出確認トルク指令加減時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetinctime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetinctime);}

const PRM_ATTR_T pndef_pdetinctime = {
    0x496,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    820,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_pdetinctime,            /* Pointer to Parameter varialble  */
    &pncal_pdetinctime,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn497 : 磁極検出確認トルク指令一定時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetconsttime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetconsttime);}

const PRM_ATTR_T pndef_pdetconsttime = {
    0x497,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    824,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    3000,                                   /* Upper Limit */
    1500,                                   /* Default value */
    &pndefGetRamPtr_pdetconsttime,          /* Pointer to Parameter varialble  */
    &pncal_pdetconsttime,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn498 : 磁極検出誤差許容範囲 */
/***************************************************************************/
static void *pndefGetRamPtr_pdetdegreeok(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetdegreeok);}

const PRM_ATTR_T pndef_pdetdegreeok = {
    0x498,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    828,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    30,                                     /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_pdetdegreeok,           /* Pointer to Parameter varialble  */
    &pncal_pdetdegreeok,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn499 : 磁極検出速度指令時逆走レベル */
/***************************************************************************/
static void *pndefGetRamPtr_pdetrevlvspdref(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetrevlvspdref);}

const PRM_ATTR_T pndef_pdetrevlvspdref = {
    0x499,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    832,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    15,                                     /* Default value */
    &pndefGetRamPtr_pdetrevlvspdref,        /* Pointer to Parameter varialble  */
    &pncal_pdetrevlvspdref,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn49A : 磁極検出待ち時間時逆走レベル */
/***************************************************************************/
static void *pndefGetRamPtr_pdetrevlvwait(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetrevlvwait);}

const PRM_ATTR_T pndef_pdetrevlvwait = {
    0x49A,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    836,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_pdetrevlvwait,          /* Pointer to Parameter varialble  */
    &pncal_pdetrevlvwait,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn49B : 磁極検出時過速度レベル */
/***************************************************************************/
static void *pndefGetRamPtr_pdetoslevel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PdetP.pdetoslevel);}

const PRM_ATTR_T pndef_pdetoslevel = {
    0x49B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    840,                                    /* EEPROM address */
    100,                                    /* Lower Limit */
    500,                                    /* Upper Limit */
    180,                                    /* Default value */
    &pndefGetRamPtr_pdetoslevel,            /* Pointer to Parameter varialble  */
    &pncal_pdetoslevel,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn501 : ゼロクランプレベル */
/***************************************************************************/
static void *pndefGetRamPtr_clamp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->clamp);}

const PRM_ATTR_T pndef_clamp = {
    0x501,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    844,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_clamp,                  /* Pointer to Parameter varialble  */
    &pncal_clamp,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn502 : 回転検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_tgonlv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tgonlv);}

const PRM_ATTR_T pndef_tgonlv = {
    0x502,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    848,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_tgonlv,                 /* Pointer to Parameter varialble  */
    &pncal_tgonlv,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn503 : 速度一致信号出力幅 */
/***************************************************************************/
static void *pndefGetRamPtr_vcmplv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vcmplv);}

const PRM_ATTR_T pndef_vcmplv = {
    0x503,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    852,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_vcmplv,                 /* Pointer to Parameter varialble  */
    &pncal_vcmplv,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn506 : ブレーキ指令−サーボオフ遅れ時間 */
/***************************************************************************/
static void *pndefGetRamPtr_brktim(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->brktim);}

const PRM_ATTR_T pndef_brktim = {
    0x506,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    856,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    50,                                     /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_brktim,                 /* Pointer to Parameter varialble  */
    &pncal_brktim,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn507 : ブレーキ指令出力速度レベル */
/***************************************************************************/
static void *pndefGetRamPtr_brkspd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->brkspd);}

const PRM_ATTR_T pndef_brkspd = {
    0x507,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    860,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_brkspd,                 /* Pointer to Parameter varialble  */
    &pncal_brkspd,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn508 : サーボオフ−ブレーキ指令待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_brkwai(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->brkwai);}

const PRM_ATTR_T pndef_brkwai = {
    0x508,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    864,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_brkwai,                 /* Pointer to Parameter varialble  */
    &pncal_brkwai,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn509 : 瞬停保持時間 */
/***************************************************************************/
static void *pndefGetRamPtr_acoftm(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->acoftm);}

const PRM_ATTR_T pndef_acoftm = {
    0x509,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    70,                                     /* EEPROM address */
    20,                                     /* Lower Limit */
    50000,                                  /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_acoftm,                 /* Pointer to Parameter varialble  */
    &pncal_acoftm,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn50A : 入力信号選択1 */
/***************************************************************************/
static void *pndefGetRamPtr_insel1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel1);}

const PRM_ATTR_T pndef_insel1 = {
    0x50A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    868,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFF2,                                 /* Upper Limit */
    0x2100,                                 /* Default value */
    &pndefGetRamPtr_insel1,                 /* Pointer to Parameter varialble  */
    &pncal_insel1,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn50B : 入力信号選択2 */
/***************************************************************************/
static void *pndefGetRamPtr_insel2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel2);}

const PRM_ATTR_T pndef_insel2 = {
    0x50B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    872,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x6543,                                 /* Default value */
    &pndefGetRamPtr_insel2,                 /* Pointer to Parameter varialble  */
    &pncal_insel2,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn50C : 入力信号選択3 */
/***************************************************************************/
static void *pndefGetRamPtr_insel3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel3);}

const PRM_ATTR_T pndef_insel3 = {
    0x50C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    876,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x8888,                                 /* Default value */
    &pndefGetRamPtr_insel3,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn50D : 入力信号選択4 */
/***************************************************************************/
static void *pndefGetRamPtr_insel4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel4);}

const PRM_ATTR_T pndef_insel4 = {
    0x50D,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    880,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x8888,                                 /* Default value */
    &pndefGetRamPtr_insel4,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn50E : 出力信号選択1 */
/***************************************************************************/
static void *pndefGetRamPtr_outsel1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outsel1);}

const PRM_ATTR_T pndef_outsel1 = {
    0x50E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    884,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x6666,                                 /* Upper Limit */
    0x3211,                                 /* Default value */
    &pndefGetRamPtr_outsel1,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn50F : 出力信号選択2 */
/***************************************************************************/
static void *pndefGetRamPtr_outsel2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outsel2);}

const PRM_ATTR_T pndef_outsel2 = {
    0x50F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    888,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x6666,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outsel2,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn510 : 出力信号選択3 */
/***************************************************************************/
static void *pndefGetRamPtr_outsel3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outsel3);}

const PRM_ATTR_T pndef_outsel3 = {
    0x510,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    892,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0666,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outsel3,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn511 : 入力信号選択5 */
/***************************************************************************/
static void *pndefGetRamPtr_inselA(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->inselA);}

const PRM_ATTR_T pndef_inselA = {
    0x511,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    896,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x8888,                                 /* Default value */
    &pndefGetRamPtr_inselA,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn512 : 出力信号反転設定 */
/***************************************************************************/
static void *pndefGetRamPtr_sorev1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->sorev1);}

const PRM_ATTR_T pndef_sorev1 = {
    0x512,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    900,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x1111,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_sorev1,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn513 : 出力信号選択4 */
/***************************************************************************/
static void *pndefGetRamPtr_sorev2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->sorev2);}

const PRM_ATTR_T pndef_sorev2 = {
    0x513,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    904,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0011,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_sorev2,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn514 : 出力信号選択5 */
/***************************************************************************/
static void *pndefGetRamPtr_outsel4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outsel4);}

const PRM_ATTR_T pndef_outsel4 = {
    0x514,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    908,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0666,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outsel4,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn515 : 入力信号選択6 */
/***************************************************************************/
static void *pndefGetRamPtr_insel5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel5);}

const PRM_ATTR_T pndef_insel5 = {
    0x515,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    912,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x8888,                                 /* Default value */
    &pndefGetRamPtr_insel5,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn516 : 入力信号選択7 */
/***************************************************************************/
static void *pndefGetRamPtr_insel6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel6);}

const PRM_ATTR_T pndef_insel6 = {
    0x516,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    916,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_insel6,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn517 : 出力信号選択6 */
/***************************************************************************/
static void *pndefGetRamPtr_outsel6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outsel6);}

const PRM_ATTR_T pndef_outsel6 = {
    0x517,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    920,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0666,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outsel6,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn518 : 出力信号選択7 */
/***************************************************************************/
static void *pndefGetRamPtr_outsel7(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outsel7);}

const PRM_ATTR_T pndef_outsel7 = {
    0x518,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    924,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x6603,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outsel7,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn519 : 入力信号選択8 */
/***************************************************************************/
static void *pndefGetRamPtr_insel7(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->insel7);}

const PRM_ATTR_T pndef_insel7 = {
    0x519,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    928,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_insel7,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn51B : モータ-負荷位置間偏差過大検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_fpgoverlv2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fpgoverlv2l);}

const PRM_ATTR_T pndef_fpgoverlv2l = {
    0x51B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_FENC | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    932,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_fpgoverlv2l,            /* Pointer to Parameter varialble  */
    &pncal_fpgoverlv2l,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn51E : 位置偏差過大ワーニングレベル */
/***************************************************************************/
static void *pndefGetRamPtr_overwrnlv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->overwrnlv);}

const PRM_ATTR_T pndef_overwrnlv = {
    0x51E,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    940,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_overwrnlv,              /* Pointer to Parameter varialble  */
    &pncal_overwrnlv,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn520 : 位置偏差過大アラームレベル */
/***************************************************************************/
static void *pndefGetRamPtr_overlv2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->overlv2l);}

const PRM_ATTR_T pndef_overlv2l = {
    0x520,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    944,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1073741823,                             /* Upper Limit */
    5242880,                                /* Default value */
    &pndefGetRamPtr_overlv2l,               /* Pointer to Parameter varialble  */
    &pncal_overlv2l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn522 : 位置決め完了幅 */
/***************************************************************************/
static void *pndefGetRamPtr_coinlv2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->coinlv2l);}

const PRM_ATTR_T pndef_coinlv2l = {
    0x522,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    952,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    7,                                      /* Default value */
    &pndefGetRamPtr_coinlv2l,               /* Pointer to Parameter varialble  */
    &pncal_coinlv2l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn524 : ＮＥＡＲ信号幅 */
/***************************************************************************/
static void *pndefGetRamPtr_nearlv2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->nearlv2l);}

const PRM_ATTR_T pndef_nearlv2l = {
    0x524,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    960,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    1073741824,                             /* Default value */
    &pndefGetRamPtr_nearlv2l,               /* Pointer to Parameter varialble  */
    &pncal_nearlv2l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn526 : サーボオン時位置偏差過大アラームレベル */
/***************************************************************************/
static void *pndefGetRamPtr_svonoverlv2l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->svonoverlv2l);}

const PRM_ATTR_T pndef_svonoverlv2l = {
    0x526,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    968,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    1073741823,                             /* Upper Limit */
    5242880,                                /* Default value */
    &pndefGetRamPtr_svonoverlv2l,           /* Pointer to Parameter varialble  */
    &pncal_svonoverlv2l,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn528 : サーボオン時位置偏差過大ワーニングレベル */
/***************************************************************************/
static void *pndefGetRamPtr_svonoverwrnlv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->svonoverwrnlv);}

const PRM_ATTR_T pndef_svonoverwrnlv = {
    0x528,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    976,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_svonoverwrnlv,          /* Pointer to Parameter varialble  */
    &pncal_svonoverwrnlv,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn529 : サーボオン時速度制限レベル */
/***************************************************************************/
static void *pndefGetRamPtr_svonspdlmtlv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->svonspdlmtlv);}

const PRM_ATTR_T pndef_svonspdlmtlv = {
    0x529,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    980,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_svonspdlmtlv,           /* Pointer to Parameter varialble  */
    &pncal_svonspdlmtlv,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn52A : フルクローズ1回転当たりの乗算値 */
/***************************************************************************/
static void *pndefGetRamPtr_fpgcutrate(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fpgcutrate);}

const PRM_ATTR_T pndef_fpgcutrate = {
    0x52A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_FENC | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    984,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_fpgcutrate,             /* Pointer to Parameter varialble  */
    &pncal_fpgcutrate,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn52B : 過負荷ワーニングレベル */
/***************************************************************************/
static void *pndefGetRamPtr_olwarlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->olwarlvl);}

const PRM_ATTR_T pndef_olwarlvl = {
    0x52B,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    988,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_olwarlvl,               /* Pointer to Parameter varialble  */
    &pncal_olwarlvl,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn52C : モータ過負荷検出ベース電流ディレーティング */
/***************************************************************************/
static void *pndefGetRamPtr_mtbastrq_drt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->mtbastrq_drt);}

const PRM_ATTR_T pndef_mtbastrq_drt = {
    0x52C,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    992,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_mtbastrq_drt,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn52D : 単相電源入力選択時サーボパック過負荷検出ベース電流ディレーティング */
/***************************************************************************/
static void *pndefGetRamPtr_svbastrq_drt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->svbastrq_drt);}

const PRM_ATTR_T pndef_svbastrq_drt = {
    0x52D,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    996,                                    /* EEPROM address */
    10,                                     /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_svbastrq_drt,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn52F : 電源投入時のモニタ表示 */
/***************************************************************************/
static void *pndefGetRamPtr_powon_mon(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->powon_mon);}

const PRM_ATTR_T pndef_powon_mon = {
    0x52F,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1000,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0FFF,                                 /* Upper Limit */
    0x0FFF,                                 /* Default value */
    &pndefGetRamPtr_powon_mon,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn530 : プログラムＪＯＧ運転関係スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_pjogsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjogsw);}

const PRM_ATTR_T pndef_pjogsw = {
    0x530,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1004,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0005,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_pjogsw,                 /* Pointer to Parameter varialble  */
    &pncal_pjogsw,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn531 : プログラムＪＯＧ移動距離 */
/***************************************************************************/
static void *pndefGetRamPtr_pjogdistl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjogdistl);}

const PRM_ATTR_T pndef_pjogdistl = {
    0x531,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    1008,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    32768,                                  /* Default value */
    &pndefGetRamPtr_pjogdistl,              /* Pointer to Parameter varialble  */
    &pncal_pjogdistl,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn533 : プログラムＪＯＧ移動速度 */
/***************************************************************************/
static void *pndefGetRamPtr_pjogrotspd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjogrotspd);}

const PRM_ATTR_T pndef_pjogrotspd = {
    0x533,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_ROTARY | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1016,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_pjogrotspd,             /* Pointer to Parameter varialble  */
    &pncal_pjogrotspd,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn534 : プログラムＪＯＧ加減速時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pjogacctm(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjogacctm);}

const PRM_ATTR_T pndef_pjogacctm = {
    0x534,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1020,                                   /* EEPROM address */
    2,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_pjogacctm,              /* Pointer to Parameter varialble  */
    &pncal_pjogacctm,                       /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn535 : プログラムＪＯＧ待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pjogwaittm(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjogwaittm);}

const PRM_ATTR_T pndef_pjogwaittm = {
    0x535,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1024,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_pjogwaittm,             /* Pointer to Parameter varialble  */
    &pncal_pjogwaittm,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn536 : プログラムＪＯＧ移動回数 */
/***************************************************************************/
static void *pndefGetRamPtr_pjognum(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjognum);}

const PRM_ATTR_T pndef_pjognum = {
    0x536,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1028,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_pjognum,                /* Pointer to Parameter varialble  */
    &pncal_pjognum,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn53A : 信号出力アラーム・ワーニング番号1 */
/***************************************************************************/
static void *pndefGetRamPtr_outalm_no1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outalm_no1);}

const PRM_ATTR_T pndef_outalm_no1 = {
    0x53A,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1032,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0FFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outalm_no1,             /* Pointer to Parameter varialble  */
    &pncal_pjogsw,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn53B : 信号出力アラーム・ワーニング番号2 */
/***************************************************************************/
static void *pndefGetRamPtr_outalm_no2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->outalm_no2);}

const PRM_ATTR_T pndef_outalm_no2 = {
    0x53B,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1036,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0FFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_outalm_no2,             /* Pointer to Parameter varialble  */
    &pncal_pjogsw,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn540 : ゲインリミット */
/***************************************************************************/
static void *pndefGetRamPtr_adatgmax(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->adatgmax);}

const PRM_ATTR_T pndef_adatgmax = {
    0x540,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1040,                                   /* EEPROM address */
    10,                                     /* Lower Limit */
    4000,                                   /* Upper Limit */
    3000,                                   /* Default value */
    &pndefGetRamPtr_adatgmax,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn548 : トレース指定アラーム番号 */
/***************************************************************************/
static void *pndefGetRamPtr_trcalm_no(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->trcalm_no);}

const PRM_ATTR_T pndef_trcalm_no = {
    0x548,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1044,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_trcalm_no,              /* Pointer to Parameter varialble  */
    &pncal_pjogsw,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn550 : アナログモニタ１オフセット電圧 */
/***************************************************************************/
static void *pndefGetRamPtr_monoffset1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->monoffset1);}

const PRM_ATTR_T pndef_monoffset1 = {
    0x550,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    74,                                     /* EEPROM address */
    -10000,                                 /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_monoffset1,             /* Pointer to Parameter varialble  */
    &pncal_monoffset1,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn551 : アナログモニタ２オフセット電圧 */
/***************************************************************************/
static void *pndefGetRamPtr_monoffset2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->monoffset2);}

const PRM_ATTR_T pndef_monoffset2 = {
    0x551,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    78,                                     /* EEPROM address */
    -10000,                                 /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_monoffset2,             /* Pointer to Parameter varialble  */
    &pncal_monoffset2,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn552 : アナログモニタ1倍率 */
/***************************************************************************/
static void *pndefGetRamPtr_mongain1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mongain1);}

const PRM_ATTR_T pndef_mongain1 = {
    0x552,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    82,                                     /* EEPROM address */
    -10000,                                 /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_mongain1,               /* Pointer to Parameter varialble  */
    &pncal_mongain1,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn553 : アナログモニタ2倍率 */
/***************************************************************************/
static void *pndefGetRamPtr_mongain2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mongain2);}

const PRM_ATTR_T pndef_mongain2 = {
    0x553,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    86,                                     /* EEPROM address */
    -10000,                                 /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_mongain2,               /* Pointer to Parameter varialble  */
    &pncal_mongain2,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn55A : 消費電力モニタ単位時間 */
/***************************************************************************/
static void *pndefGetRamPtr_powmon_unitime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->powmon_unitime);}

const PRM_ATTR_T pndef_powmon_unitime = {
    0x55A,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    90,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    1440,                                   /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_powmon_unitime,         /* Pointer to Parameter varialble  */
    &pncal_pjogsw,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn560 : 残留振動検出幅 */
/***************************************************************************/
static void *pndefGetRamPtr_remdetw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->remdetw);}

const PRM_ATTR_T pndef_remdetw = {
    0x560,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1048,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    3000,                                   /* Upper Limit */
    400,                                    /* Default value */
    &pndefGetRamPtr_remdetw,                /* Pointer to Parameter varialble  */
    &pncal_remdetw,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn561 : オーバーシュート検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_ovserrdetlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ovserrdetlvl);}

const PRM_ATTR_T pndef_ovserrdetlvl = {
    0x561,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1052,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_ovserrdetlvl,           /* Pointer to Parameter varialble  */
    &pncal_ovserrdetlvl,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn580 : ゼロクランプレベル */
/***************************************************************************/
static void *pndefGetRamPtr_clamp_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->clamp_l);}

const PRM_ATTR_T pndef_clamp_l = {
    0x580,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1056,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_clamp_l,                /* Pointer to Parameter varialble  */
    &pncal_clamp_l,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn581 : 回転検出レベル（リニア） */
/***************************************************************************/
static void *pndefGetRamPtr_tgonlv_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tgonlv_l);}

const PRM_ATTR_T pndef_tgonlv_l = {
    0x581,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1060,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_tgonlv_l,               /* Pointer to Parameter varialble  */
    &pncal_tgonlv_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn582 : 速度一致信号出力幅（リニア） */
/***************************************************************************/
static void *pndefGetRamPtr_vcmplv_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vcmplv_l);}

const PRM_ATTR_T pndef_vcmplv_l = {
    0x582,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1064,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_vcmplv_l,               /* Pointer to Parameter varialble  */
    &pncal_vcmplv_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn583 : ブレーキ指令出力速度レベル */
/***************************************************************************/
static void *pndefGetRamPtr_brkspd_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->brkspd_l);}

const PRM_ATTR_T pndef_brkspd_l = {
    0x583,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1068,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_brkspd_l,               /* Pointer to Parameter varialble  */
    &pncal_brkspd_l,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn584 : サーボオン時速度制限レベル */
/***************************************************************************/
static void *pndefGetRamPtr_svonspdlmtlv_l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->svonspdlmtlv_l);}

const PRM_ATTR_T pndef_svonspdlmtlv_l = {
    0x584,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1072,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_svonspdlmtlv_l,         /* Pointer to Parameter varialble  */
    &pncal_svonspdlmtlv_l,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn585 : プログラムＪＯＧ最高速度(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_pjoglinspd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pjoglinspd);}

const PRM_ATTR_T pndef_pjoglinspd = {
    0x585,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1076,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_pjoglinspd,             /* Pointer to Parameter varialble  */
    &pncal_pjoglinspd,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn586 : モータ自走冷却率 */
/***************************************************************************/
static void *pndefGetRamPtr_msrcra(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->msrcra);}

const PRM_ATTR_T pndef_msrcra = {
    0x586,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_LINEAR | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1080,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_msrcra,                 /* Pointer to Parameter varialble  */
    &pncal_msrcra,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn587 : 絶対値スケール用磁極検出実行選択 */
/***************************************************************************/
static void *pndefGetRamPtr_pldetsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pldetsw);}

const PRM_ATTR_T pndef_pldetsw = {
    0x587,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0001,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pldetsw,                /* Pointer to Parameter varialble  */
    &pncal_pldetsw,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn590 : POT入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_pot_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pot_selsw);}

const PRM_ATTR_T pndef_pot_selsw = {
    0x590,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1084,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pot_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn591 : NOT入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_not_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->not_selsw);}

const PRM_ATTR_T pndef_not_selsw = {
    0x591,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1088,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_not_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn592 : DEC入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_dec_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dec_selsw);}

const PRM_ATTR_T pndef_dec_selsw = {
    0x592,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1092,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dec_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn593 : EXT1入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_ext1_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ext_selsw[0]);}

const PRM_ATTR_T pndef_ext1_selsw = {
    0x593,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1096,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ext1_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn594 : EXT2入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_ext2_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ext_selsw[1]);}

const PRM_ATTR_T pndef_ext2_selsw = {
    0x594,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1100,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ext2_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn595 : EXT3入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_ext3_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ext_selsw[2]);}

const PRM_ATTR_T pndef_ext3_selsw = {
    0x595,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1104,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ext3_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn597 : FSTP入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_fstp_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fstp_selsw);}

const PRM_ATTR_T pndef_fstp_selsw = {
    0x597,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1108,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_fstp_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn598 : P-CL入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_pcl_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pcl_selsw);}

const PRM_ATTR_T pndef_pcl_selsw = {
    0x598,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1112,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pcl_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn599 : N-CL入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_ncl_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ncl_selsw);}

const PRM_ATTR_T pndef_ncl_selsw = {
    0x599,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1116,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ncl_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn59A : CLINKANS入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_clinkans_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->clinkans_selsw);}

const PRM_ATTR_T pndef_clinkans_selsw = {
    0x59A,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1120,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_clinkans_selsw,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn59B : CMCANS入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_cmcans_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->cmcans_selsw);}

const PRM_ATTR_T pndef_cmcans_selsw = {
    0x59B,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1124,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_cmcans_selsw,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn59C : CONV_READY入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_convready_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->convready_selsw);}

const PRM_ATTR_T pndef_convready_selsw = {
    0x59C,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1128,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_convready_selsw,        /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn59D : CONV_MCON入力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_convmcon_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->convmcon_selsw);}

const PRM_ATTR_T pndef_convmcon_selsw = {
    0x59D,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1132,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x3019,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_convmcon_selsw,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B0 : COIN出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_coin_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->coin_selsw);}

const PRM_ATTR_T pndef_coin_selsw = {
    0x5B0,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1136,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_coin_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B1 : V-CMP出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_vcmp_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vcmp_selsw);}

const PRM_ATTR_T pndef_vcmp_selsw = {
    0x5B1,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1140,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_vcmp_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B2 : TGON出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_tgon_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tgon_selsw);}

const PRM_ATTR_T pndef_tgon_selsw = {
    0x5B2,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1144,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_tgon_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B3 : S-RDY出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_srdy_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->srdy_selsw);}

const PRM_ATTR_T pndef_srdy_selsw = {
    0x5B3,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1148,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_srdy_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B4 : CLT出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_clt_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->clt_selsw);}

const PRM_ATTR_T pndef_clt_selsw = {
    0x5B4,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1152,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_clt_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B5 : VLT出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_vlt_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vlt_selsw);}

const PRM_ATTR_T pndef_vlt_selsw = {
    0x5B5,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1156,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_vlt_selsw,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B6 : BK出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_bk_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->bk_selsw);}

const PRM_ATTR_T pndef_bk_selsw = {
    0x5B6,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1160,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_bk_selsw,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B7 : WARN出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_warn_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->warn_selsw);}

const PRM_ATTR_T pndef_warn_selsw = {
    0x5B7,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1164,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_warn_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B8 : NEAR出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_near_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->near_selsw);}

const PRM_ATTR_T pndef_near_selsw = {
    0x5B8,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1168,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_near_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5B9 : C-PULSE出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_cpulse_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->cpulse_selsw);}

const PRM_ATTR_T pndef_cpulse_selsw = {
    0x5B9,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1172,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_cpulse_selsw,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5BA : CLINK出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_clink_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->clink_selsw);}

const PRM_ATTR_T pndef_clink_selsw = {
    0x5BA,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1176,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_clink_selsw,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5BB : CMCON出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_cmcon_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->cmcon_selsw);}

const PRM_ATTR_T pndef_cmcon_selsw = {
    0x5BB,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1180,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_cmcon_selsw,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5BC : PM出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_pm_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pm_selsw);}

const PRM_ATTR_T pndef_pm_selsw = {
    0x5BC,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1184,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_pm_selsw,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5BD : DAO1出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_dao1_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dao_selsw[0]);}

const PRM_ATTR_T pndef_dao1_selsw = {
    0x5BD,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1188,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dao1_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn5BE : DAO2出力信号設定 */
/***************************************************************************/
static void *pndefGetRamPtr_dao2_selsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dao_selsw[1]);}

const PRM_ATTR_T pndef_dao2_selsw = {
    0x5BE,                                  /* Register Number */
    ACCLVL_USER2,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1192,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x2039,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dao2_selsw,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn600 : 回生抵抗容量 */
/***************************************************************************/
static void *pndefGetRamPtr_regres(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->regres);}

const PRM_ATTR_T pndef_regres = {
    0x600,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    94,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_regres,                 /* Pointer to Parameter varialble  */
    &pncal_regres,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn601 : ＤＢ抵抗容量 */
/***************************************************************************/
static void *pndefGetRamPtr_dbres(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dbres);}

const PRM_ATTR_T pndef_dbres = {
    0x601,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1196,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dbres,                  /* Pointer to Parameter varialble  */
    &pncal_dbres,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn603 : 回生抵抗値 */
/***************************************************************************/
static void *pndefGetRamPtr_exreger(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->exreger);}

const PRM_ATTR_T pndef_exreger = {
    0x603,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    98,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_exreger,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn604 : DB抵抗値 */
/***************************************************************************/
static void *pndefGetRamPtr_exdbr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->exdbr);}

const PRM_ATTR_T pndef_exdbr = {
    0x604,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1200,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_exdbr,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn620 : HWBB信号タイミング時間 */
/***************************************************************************/
static void *pndefGetRamPtr_bbondiff0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->bbondiff0);}

const PRM_ATTR_T pndef_bbondiff0 = {
    0x620,                                  /* Register Number */
    ACCLVL_USER3,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1204,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_bbondiff0,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn621 : 安全機能応用選択スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_saftyfuncSw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->saftyfuncSw);}

const PRM_ATTR_T pndef_saftyfuncSw = {
    0x621,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1208,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x0011,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_saftyfuncSw,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn622 : アクティブモード内部減速定数A */
/***************************************************************************/
static void *pndefGetRamPtr_safeindecA(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->safeindecA);}

const PRM_ATTR_T pndef_safeindecA = {
    0x622,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1212,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    30000,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_safeindecA,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn623 : アクティブモード内部減速定数B */
/***************************************************************************/
static void *pndefGetRamPtr_safeindecB(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->safeindecB);}

const PRM_ATTR_T pndef_safeindecB = {
    0x623,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1216,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    30000,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_safeindecB,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn624 : アクティブモード内部減速停止速度 */
/***************************************************************************/
static void *pndefGetRamPtr_safeindecVel(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->safeindecVel);}

const PRM_ATTR_T pndef_safeindecVel = {
    0x624,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1220,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_safeindecVel,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn625 : アクティブモード停止継続時間 */
/***************************************************************************/
static void *pndefGetRamPtr_safeStopTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->safeStopTime);}

const PRM_ATTR_T pndef_safeStopTime = {
    0x625,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1224,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_safeStopTime,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn626 : アクティブモード解除位置偏差レベル */
/***************************************************************************/
static void *pndefGetRamPtr_safePerrlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->safePerrlvl);}

const PRM_ATTR_T pndef_safePerrlvl = {
    0x626,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    1228,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    1073741823,                             /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_safePerrlvl,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of Pn628 : アクティブモード解除速度指令レベル */
/***************************************************************************/
static void *pndefGetRamPtr_safeVreflvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->safeVreflvl);}

const PRM_ATTR_T pndef_safeVreflvl = {
    0x628,                                  /* Register Number */
    ACCLVL_USER1,                           /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1236,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_safeVreflvl,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};





/***************************************************************************/
/*  Attribute of PnE05 : アラームマスク１ */
/***************************************************************************/
static void *pndefGetRamPtr_almmsk1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->almmsk1);}

const PRM_ATTR_T pndef_almmsk1 = {
    0xE05,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1240,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xffff,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_almmsk1,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE06 : アラームマスク２ */
/***************************************************************************/
static void *pndefGetRamPtr_almmsk2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->almmsk2);}

const PRM_ATTR_T pndef_almmsk2 = {
    0xE06,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1242,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xffff,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_almmsk2,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE09 : オプションカード／セーフティ機能対応制限スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_opdetsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->opdetsw);}

const PRM_ATTR_T pndef_opdetsw = {
    0xE09,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1248,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0106,                                 /* Default value */
    &pndefGetRamPtr_opdetsw,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE0B : システムスイッチ１ */
/***************************************************************************/
static void *pndefGetRamPtr_syssw1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->syssw1);}

const PRM_ATTR_T pndef_syssw1 = {
    0xE0B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1250,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0301,                                 /* Default value */
    &pndefGetRamPtr_syssw1,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE0C : システムスイッチ２ */
/***************************************************************************/
static void *pndefGetRamPtr_syssw2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->syssw2);}

const PRM_ATTR_T pndef_syssw2 = {
    0xE0C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    102,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0306,                                 /* Default value */
    &pndefGetRamPtr_syssw2,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE0D : システムスイッチ３ */
/***************************************************************************/
static void *pndefGetRamPtr_syssw3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->syssw3);}

const PRM_ATTR_T pndef_syssw3 = {
    0xE0D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1252,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_syssw3,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE0E : 指令入力フィルタ選択時の指令フィルタ設定 */
/***************************************************************************/
static void *pndefGetRamPtr_comfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->comfil);}

const PRM_ATTR_T pndef_comfil = {
    0xE0E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1254,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0112,                                 /* Default value */
    &pndefGetRamPtr_comfil,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE0F : 下位:設置環境モニタ基準温度, 上位:機能制限スイッチ１ */
/***************************************************************************/
static void *pndefGetRamPtr_InstEnvTmp_sw1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->InstEnvTmp_sw1);}

const PRM_ATTR_T pndef_InstEnvTmp_sw1 = {
    0xE0F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    104,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0055,                                 /* Default value */
    &pndefGetRamPtr_InstEnvTmp_sw1,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE10 : パラメータパスワード */
/***************************************************************************/
static void *pndefGetRamPtr_paswrd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->paswrd);}

const PRM_ATTR_T pndef_paswrd = {
    0xE10,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1256,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0020,                                 /* Default value */
    &pndefGetRamPtr_paswrd,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE11 : 下位:サーボパック形式コード, 上位:入力電圧コード */
/***************************************************************************/
static void *pndefGetRamPtr_styp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->styp);}

const PRM_ATTR_T pndef_styp = {
    0xE11,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1258,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0102,                                 /* Default value */
    &pndefGetRamPtr_styp,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE12 : サーボパック容量 */
/***************************************************************************/
static void *pndefGetRamPtr_ampw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ampw);}

const PRM_ATTR_T pndef_ampw = {
    0xE12,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1260,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    200,                                    /* Default value */
    &pndefGetRamPtr_ampw,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE13 : Ｙ仕様番号 */
/***************************************************************************/
static void *pndefGetRamPtr_yopt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->yopt);}

const PRM_ATTR_T pndef_yopt = {
    0xE13,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1262,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_yopt,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE14 : サーボパック定格電流 */
/***************************************************************************/
static void *pndefGetRamPtr_irats(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->irats);}

const PRM_ATTR_T pndef_irats = {
    0xE14,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1264,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    23,                                     /* Default value */
    &pndefGetRamPtr_irats,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE15 : サーボパック最大電流 */
/***************************************************************************/
static void *pndefGetRamPtr_imaxs(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->imaxs);}

const PRM_ATTR_T pndef_imaxs = {
    0xE15,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1266,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    82,                                     /* Default value */
    &pndefGetRamPtr_imaxs,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE17 : 下位:過負荷検出ベース電流, 上位:過負荷検出中間電流 */
/***************************************************************************/
static void *pndefGetRamPtr_ibs_md(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ibs_md);}

const PRM_ATTR_T pndef_ibs_md = {
    0xE17,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1268,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x1773,                                 /* Default value */
    &pndefGetRamPtr_ibs_md,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE18 : 下位:過負荷検出中間電流時間, 上位:過負荷検出最大電流時間 */
/***************************************************************************/
static void *pndefGetRamPtr_imdt_mxt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->imdt_mxt);}

const PRM_ATTR_T pndef_imdt_mxt = {
    0xE18,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1270,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0301,                                 /* Default value */
    &pndefGetRamPtr_imdt_mxt,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE19 : ＯＬ警告比 */
/***************************************************************************/
static void *pndefGetRamPtr_olwrat(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->olwrat);}

const PRM_ATTR_T pndef_olwrat = {
    0xE19,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1272,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_olwrat,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE1A : 回生抵抗値 */
/***************************************************************************/
static void *pndefGetRamPtr_reger(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->reger);}

const PRM_ATTR_T pndef_reger = {
    0xE1A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    106,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    40000,                                  /* Default value */
    &pndefGetRamPtr_reger,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE1B : ＤＢ抵抗値 */
/***************************************************************************/
static void *pndefGetRamPtr_dbr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dbr);}

const PRM_ATTR_T pndef_dbr = {
    0xE1B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1274,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dbr,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE1C : 回生抵抗容量(デフォルト) */
/***************************************************************************/
static void *pndefGetRamPtr_wre(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->wre);}

const PRM_ATTR_T pndef_wre = {
    0xE1C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    108,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_wre,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE1D : ＤＢ抵抗容量(デフォルト) */
/***************************************************************************/
static void *pndefGetRamPtr_wdb(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->wdb);}

const PRM_ATTR_T pndef_wdb = {
    0xE1D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1276,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_wdb,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE1E : 下位:S-PG受信失敗許容回数, 上位:FS-PG受信失敗許容回数 */
/***************************************************************************/
static void *pndefGetRamPtr_spgfail(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->spgfail);}

const PRM_ATTR_T pndef_spgfail = {
    0xE1E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1278,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0303,                                 /* Default value */
    &pndefGetRamPtr_spgfail,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE1F : 下位:突入抵抗過負荷用電源投入許容回数, 上位:(予備) */
/***************************************************************************/
static void *pndefGetRamPtr_PnE1F(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnE1F);}

const PRM_ATTR_T pndef_PnE1F = {
    0xE1F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1280,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x000A,                                 /* Default value */
    &pndefGetRamPtr_PnE1F,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE20 : 電流ループゲイン(D) */
/***************************************************************************/
static void *pndefGetRamPtr_ignd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ignd);}

const PRM_ATTR_T pndef_ignd = {
    0xE20,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1282,                                   /* EEPROM address */
    100,                                    /* Lower Limit */
    10000,                                  /* Upper Limit */
    1800,                                   /* Default value */
    &pndefGetRamPtr_ignd,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE21 : 電流ループゲイン(Q) */
/***************************************************************************/
static void *pndefGetRamPtr_ignq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ignq);}

const PRM_ATTR_T pndef_ignq = {
    0xE21,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1284,                                   /* EEPROM address */
    100,                                    /* Lower Limit */
    10000,                                  /* Upper Limit */
    1800,                                   /* Default value */
    &pndefGetRamPtr_ignq,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE22 : 電流ループ積分時定数(D) */
/***************************************************************************/
static void *pndefGetRamPtr_kid(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->kid);}

const PRM_ATTR_T pndef_kid = {
    0xE22,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1286,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    600,                                    /* Default value */
    &pndefGetRamPtr_kid,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE23 : 電流ループ積分時定数(Q) */
/***************************************************************************/
static void *pndefGetRamPtr_kiq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->kiq);}

const PRM_ATTR_T pndef_kiq = {
    0xE23,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1288,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    600,                                    /* Default value */
    &pndefGetRamPtr_kiq,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE24 : 電流ループ積分制限値(D) */
/***************************************************************************/
static void *pndefGetRamPtr_limintd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->limintd);}

const PRM_ATTR_T pndef_limintd = {
    0xE24,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1290,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10430,                                  /* Default value */
    &pndefGetRamPtr_limintd,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE25 : 電流ループ積分制限値(Q) */
/***************************************************************************/
static void *pndefGetRamPtr_limintq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->limintq);}

const PRM_ATTR_T pndef_limintq = {
    0xE25,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1292,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10430,                                  /* Default value */
    &pndefGetRamPtr_limintq,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE26 : 電流ループフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_ifil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ifil);}

const PRM_ATTR_T pndef_ifil = {
    0xE26,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1294,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ifil,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE28 : 電流検出ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_shant(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->shant);}

const PRM_ATTR_T pndef_shant = {
    0xE28,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1296,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    8192,                                   /* Upper Limit */
    6298,                                   /* Default value */
    &pndefGetRamPtr_shant,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE2A : 電圧補正ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_vrevgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vrevgn);}

const PRM_ATTR_T pndef_vrevgn = {
    0xE2A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1298,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x7FFF,                                 /* Upper Limit */
    0x2000,                                 /* Default value */
    &pndefGetRamPtr_vrevgn,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE2B : 電圧制限値 */
/***************************************************************************/
static void *pndefGetRamPtr_lmdata(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->lmdata);}

const PRM_ATTR_T pndef_lmdata = {
    0xE2B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1300,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0x7FFF,                                 /* Upper Limit */
    0x7FFF,                                 /* Default value */
    &pndefGetRamPtr_lmdata,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE2C : ＰＷＭ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_pwmf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pwmf);}

const PRM_ATTR_T pndef_pwmf = {
    0xE2C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1302,                                   /* EEPROM address */
    2000,                                   /* Lower Limit */
    65535,                                  /* Upper Limit */
    10667,                                  /* Default value */
    &pndefGetRamPtr_pwmf,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE2D : 下位:オンディレイ時間, 上位:オンディレイ補償定数 */
/***************************************************************************/
static void *pndefGetRamPtr_odt_c(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->odt_c);}

const PRM_ATTR_T pndef_odt_c = {
    0xE2D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1304,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x1E1E,                                 /* Default value */
    &pndefGetRamPtr_odt_c,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE2E : 下位:オンディレイ補償変更レベル, 上位:補償傾き */
/***************************************************************************/
static void *pndefGetRamPtr_odlv_slp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->odlv_slp);}

const PRM_ATTR_T pndef_odlv_slp = {
    0xE2E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1306,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0A64,                                 /* Default value */
    &pndefGetRamPtr_odlv_slp,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE2F : ＤＱ用スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_dq_sw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dq_sw);}

const PRM_ATTR_T pndef_dq_sw = {
    0xE2F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1308,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0D0A,                                 /* Default value */
    &pndefGetRamPtr_dq_sw,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE34 : 位相補償 */
/***************************************************************************/
static void *pndefGetRamPtr_phscmp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->phscmp);}

const PRM_ATTR_T pndef_phscmp = {
    0xE34,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1318,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_phscmp,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE35 : 電流ループ微分時間(D) */
/***************************************************************************/
static void *pndefGetRamPtr_kdd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->kdd);}

const PRM_ATTR_T pndef_kdd = {
    0xE35,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1320,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_kdd,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE36 : 電流ループ微分時間(Q) */
/***************************************************************************/
static void *pndefGetRamPtr_kdq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->kdq);}

const PRM_ATTR_T pndef_kdq = {
    0xE36,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1322,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_kdq,                    /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE37 : 電流ループ微分フィルタ時定数(D) */
/***************************************************************************/
static void *pndefGetRamPtr_dfild(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dfild);}

const PRM_ATTR_T pndef_dfild = {
    0xE37,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1324,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dfild,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE38 : 電流ループ微分フィルタ時定数(Q) */
/***************************************************************************/
static void *pndefGetRamPtr_dfilq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dfilq);}

const PRM_ATTR_T pndef_dfilq = {
    0xE38,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1326,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dfilq,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE39 : 電流制御スイッチ２ */
/***************************************************************************/
static void *pndefGetRamPtr_dq_sw2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dq_sw2);}

const PRM_ATTR_T pndef_dq_sw2 = {
    0xE39,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1328,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0004,                                 /* Default value */
    &pndefGetRamPtr_dq_sw2,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE3B : モータ共振抑制用ノッチフィルタQ値 */
/***************************************************************************/
static void *pndefGetRamPtr_motresq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->motresq);}

const PRM_ATTR_T pndef_motresq = {
    0xE3B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1330,                                   /* EEPROM address */
    5,                                      /* Lower Limit */
    500,                                    /* Upper Limit */
    70,                                     /* Default value */
    &pndefGetRamPtr_motresq,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE3C : N側スイッチON最小時間 */
/***************************************************************************/
static void *pndefGetRamPtr_PnE3C(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnE3C);}

const PRM_ATTR_T pndef_PnE3C = {
    0xE3C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1332,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    85,                                     /* Default value */
    &pndefGetRamPtr_PnE3C,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE3D : サンプル＆ホールド時間 */
/***************************************************************************/
static void *pndefGetRamPtr_PnE3D(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnE3D);}

const PRM_ATTR_T pndef_PnE3D = {
    0xE3D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1334,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    50,                                     /* Upper Limit */
    15,                                     /* Default value */
    &pndefGetRamPtr_PnE3D,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE3E : 主回路電圧補正用ローパスフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_kvvfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->kvvfil);}

const PRM_ATTR_T pndef_kvvfil = {
    0xE3E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    110,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_kvvfil,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE3F : モータ過電流検出フィルタ回数 */
/***************************************************************************/
static void *pndefGetRamPtr_fltrange(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fltrange);}

const PRM_ATTR_T pndef_fltrange = {
    0xE3F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1470,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    4,                                      /* Upper Limit */
    2,                                      /* Default value */
    &pndefGetRamPtr_fltrange,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE40 : DRV内部信号異常検出フィルタ回数 */
/***************************************************************************/
static void *pndefGetRamPtr_tplsfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tplsfil);}

const PRM_ATTR_T pndef_tplsfil = {
    0xE40,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1468,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    10,                                     /* Upper Limit */
    2,                                      /* Default value */
    &pndefGetRamPtr_tplsfil,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE42 : モニタ１ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_mnrng0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mnrng0);}

const PRM_ATTR_T pndef_mnrng0 = {
    0xE42,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    112,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0100,                                 /* Default value */
    &pndefGetRamPtr_mnrng0,                 /* Pointer to Parameter varialble  */
    &pncal_mnrng0,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE43 : モニタ２ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_mnrng1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mnrng1);}

const PRM_ATTR_T pndef_mnrng1 = {
    0xE43,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    114,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0100,                                 /* Default value */
    &pndefGetRamPtr_mnrng1,                 /* Pointer to Parameter varialble  */
    &pncal_mnrng1,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE44 : 下位:i-P制御割合, 上位:暴走検出トルク */
/***************************************************************************/
static void *pndefGetRamPtr_ipg_rwt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ipg_rwt);}

const PRM_ATTR_T pndef_ipg_rwt = {
    0xE44,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1336,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x7864,                                 /* Default value */
    &pndefGetRamPtr_ipg_rwt,                /* Pointer to Parameter varialble  */
    &pncal_ipg_rwt,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE45 : 低速ＯＬ：ベース電流[%]，ＯＬ特性変曲点[電気角10r/min] */
/***************************************************************************/
static void *pndefGetRamPtr_PnE45(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnE45);}

const PRM_ATTR_T pndef_PnE45 = {
    0xE45,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1338,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnE45,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE46 : 低速ＯＬ：中間電流[10%]，最大電流[10%] */
/***************************************************************************/
static void *pndefGetRamPtr_PnE46(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnE46);}

const PRM_ATTR_T pndef_PnE46 = {
    0xE46,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1340,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnE46,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE47 : 低速ＯＬ：中間電流時間[0.1s]，最大電流時間[0.1s] */
/***************************************************************************/
static void *pndefGetRamPtr_PnE47(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnE47);}

const PRM_ATTR_T pndef_PnE47 = {
    0xE47,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1342,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnE47,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE48 : モニタ１アドレス */
/***************************************************************************/
static void *pndefGetRamPtr_mnptr0l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mnptr0l);}

const PRM_ATTR_T pndef_mnptr0l = {
    0xE48,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    116,                                    /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x740A,                                 /* Default value */
    &pndefGetRamPtr_mnptr0l,                /* Pointer to Parameter varialble  */
    &pncal_mnptr0,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE4A : モニタ２アドレス */
/***************************************************************************/
static void *pndefGetRamPtr_mnptr1l(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mnptr1l);}

const PRM_ATTR_T pndef_mnptr1l = {
    0xE4A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    120,                                    /* EEPROM address */
    0x00000000,                             /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0x740E,                                 /* Default value */
    &pndefGetRamPtr_mnptr1l,                /* Pointer to Parameter varialble  */
    &pncal_mnptr1,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE50 : 電流検出ゼロ調(U相) */
/***************************************************************************/
static void *pndefGetRamPtr_zadjiu(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->zadjiu);}

const PRM_ATTR_T pndef_zadjiu = {
    0xE50,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    4,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0xFFFF,                                 /* Default value */
    &pndefGetRamPtr_zadjiu,                 /* Pointer to Parameter varialble  */
    &pncal_zadjiu,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE51 : 電流検出ゼロ調(V相) */
/***************************************************************************/
static void *pndefGetRamPtr_zadjiv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->zadjiv);}

const PRM_ATTR_T pndef_zadjiv = {
    0xE51,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    6,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0xFFFF,                                 /* Default value */
    &pndefGetRamPtr_zadjiv,                 /* Pointer to Parameter varialble  */
    &pncal_zadjiv,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE52 : 電流検出ゲイン調(U,V相) */
/***************************************************************************/
static void *pndefGetRamPtr_gadjiu_v(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gadjiu_v);}

const PRM_ATTR_T pndef_gadjiu_v = {
    0xE52,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    8,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0xFFFF,                                 /* Default value */
    &pndefGetRamPtr_gadjiu_v,               /* Pointer to Parameter varialble  */
    &pncal_gadjiu_v,                        /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE53 : 速度指令ゼロ調 */
/***************************************************************************/
static void *pndefGetRamPtr_zadjvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->zadjvl);}

const PRM_ATTR_T pndef_zadjvl = {
    0xE53,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    12,                                     /* EEPROM address */
    -2147483648,                            /* Lower Limit */
    2147483647,                             /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_zadjvl,                 /* Pointer to Parameter varialble  */
    &pncal_zadjvl,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE55 : 速度指令ゲイン調 */
/***************************************************************************/
static void *pndefGetRamPtr_gadjv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gadjv);}

const PRM_ATTR_T pndef_gadjv = {
    0xE55,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    16,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gadjv,                  /* Pointer to Parameter varialble  */
    &pncal_vrefgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE56 : トルク指令ゼロ調(正入力/負入力) */
/***************************************************************************/
static void *pndefGetRamPtr_zadjtp_n(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->zadjtp_n);}

const PRM_ATTR_T pndef_zadjtp_n = {
    0xE56,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    18,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_zadjtp_n,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE57 : トルク指令ゲイン調(正入力/負入力) */
/***************************************************************************/
static void *pndefGetRamPtr_gadjtp_n(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->gadjtp_n);}

const PRM_ATTR_T pndef_gadjtp_n = {
    0xE57,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    20,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_gadjtp_n,               /* Pointer to Parameter varialble  */
    &pncal_trefgn,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE58 : モニタ１,２ゼロ調 */
/***************************************************************************/
static void *pndefGetRamPtr_mnaz0_1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mnaz0_1);}

const PRM_ATTR_T pndef_mnaz0_1 = {
    0xE58,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    4,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mnaz0_1,                /* Pointer to Parameter varialble  */
    &pncal_mnaz0_1,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE59 : モニタ１,２ゲイン調 */
/***************************************************************************/
static void *pndefGetRamPtr_mnag0_1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->mnag0_1);}

const PRM_ATTR_T pndef_mnag0_1 = {
    0xE59,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    6,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mnag0_1,                /* Pointer to Parameter varialble  */
    &pncal_mnag0_1,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE5A : 下位:PG電源電圧調, 上位:空き */
/***************************************************************************/
static void *pndefGetRamPtr_pgadj(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->pgadj);}

const PRM_ATTR_T pndef_pgadj = {
    0xE5A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    8,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0xFFFF,                                 /* Default value */
    &pndefGetRamPtr_pgadj,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE5B : 下位:P-N電圧検出レベル微調, 上位:P-N電圧検出レベルゼロ調 */
/***************************************************************************/
static void *pndefGetRamPtr_pnadj(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->pnadj);}

const PRM_ATTR_T pndef_pnadj = {
    0xE5B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    10,                                     /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0xFFFF,                                 /* Default value */
    &pndefGetRamPtr_pnadj,                  /* Pointer to Parameter varialble  */
    &pncal_vdetadj,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE60 : 下位:過負荷検出ベース電流, 上位:過負荷検出中間電流 */
/***************************************************************************/
static void *pndefGetRamPtr_ratbt_mt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ratbt_mt);}

const PRM_ATTR_T pndef_ratbt_mt = {
    0xE60,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1346,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x6464,                                 /* Default value */
    &pndefGetRamPtr_ratbt_mt,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE61 : 下位:過負荷検出中間時間, 上位:過負荷検出中間電流２ */
/***************************************************************************/
static void *pndefGetRamPtr_ratmtt_2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ratmtt_2);}

const PRM_ATTR_T pndef_ratmtt_2 = {
    0xE61,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1348,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x6464,                                 /* Default value */
    &pndefGetRamPtr_ratmtt_2,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE62 : 下位:過負荷検出中間時間２, 上位:ＯＳ検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_ratmt2_os(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ratmt2_os);}

const PRM_ATTR_T pndef_ratmt2_os = {
    0xE62,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1350,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x6464,                                 /* Default value */
    &pndefGetRamPtr_ratmt2_os,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE6F : 回生抵抗非内蔵機種で回生不使用時のOV検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_NonRegOvlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->NonRegOvlvl);}

const PRM_ATTR_T pndef_NonRegOvlvl = {
    0xE6F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    124,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    410,                                    /* Default value */
    &pndefGetRamPtr_NonRegOvlvl,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE70 : Ｐ−Ｎ電圧検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_pnvoltgn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->pnvoltgn);}

const PRM_ATTR_T pndef_pnvoltgn = {
    0xE70,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    126,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_pnvoltgn,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE71 : ＯＶ検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_ovlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->ovlvl);}

const PRM_ATTR_T pndef_ovlvl = {
    0xE71,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    128,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    420,                                    /* Default value */
    &pndefGetRamPtr_ovlvl,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE72 : 回生オンレベル */
/***************************************************************************/
static void *pndefGetRamPtr_regonlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->regonlvl);}

const PRM_ATTR_T pndef_regonlvl = {
    0xE72,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    130,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    395,                                    /* Default value */
    &pndefGetRamPtr_regonlvl,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE73 : ＵＶ検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_uvlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->uvlvl);}

const PRM_ATTR_T pndef_uvlvl = {
    0xE73,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    132,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    170,                                    /* Default value */
    &pndefGetRamPtr_uvlvl,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE74 : ＵＶ検出フィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_uvfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->uvfil);}

const PRM_ATTR_T pndef_uvfil = {
    0xE74,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    134,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_uvfil,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE78 : 回生オフヒステリシス幅 */
/***************************************************************************/
static void *pndefGetRamPtr_regoffhys(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->regoffhys);}

const PRM_ATTR_T pndef_regoffhys = {
    0xE78,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    136,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    50,                                     /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_regoffhys,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE79 : 主回路検出ＡＤ用ローパスフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_vdetadfil(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->vdetadfil);}

const PRM_ATTR_T pndef_vdetadfil = {
    0xE79,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    138,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_vdetadfil,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE7A : 主回路検出フィルタ選択スイッチ */
/***************************************************************************/
static void *pndefGetRamPtr_vdefilsw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->vdefilsw);}

const PRM_ATTR_T pndef_vdefilsw = {
    0xE7A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    140,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x55,                                   /* Default value */
    &pndefGetRamPtr_vdefilsw,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE7B : ＵＶワーニング検出レベル */
/***************************************************************************/
static void *pndefGetRamPtr_UvWrnLvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->UvWrnLvl);}

const PRM_ATTR_T pndef_UvWrnLvl = {
    0xE7B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    142,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    200,                                    /* Default value */
    &pndefGetRamPtr_UvWrnLvl,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE7C : PnE70[V]入力時のA/D値 */
/***************************************************************************/
static void *pndefGetRamPtr_VdetADMax(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->VdetADMax);}

const PRM_ATTR_T pndef_VdetADMax = {
    0xE7C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    144,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    21925,                                  /* Default value */
    &pndefGetRamPtr_VdetADMax,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE7F : コンバータ容量 */
/***************************************************************************/
static void *pndefGetRamPtr_ConvW(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->ConvW);}

const PRM_ATTR_T pndef_ConvW = {
    0xE7F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    146,                                    /* EEPROM address */
    50,                                     /* Lower Limit */
    65535,                                  /* Upper Limit */
    8000,                                   /* Default value */
    &pndefGetRamPtr_ConvW,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE80 : 内蔵FAN寿命寿命計算用周囲温度オフセット */
/***************************************************************************/
static void *pndefGetRamPtr_FanTemperatureOffset(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->FanTemperatureOffset);}

const PRM_ATTR_T pndef_FanTemperatureOffset = {
    0xE80,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    164,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    25,                                     /* Default value */
    &pndefGetRamPtr_FanTemperatureOffset,   /* Pointer to Parameter varialble  */
    &pncal_fantemperatureoffset,            /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE81 : FAN交換オフセット時間 */
/***************************************************************************/
static void *pndefGetRamPtr_FanTimeOffset(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->FanTimeOffset);}

const PRM_ATTR_T pndef_FanTimeOffset = {
    0xE81,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    12,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_FanTimeOffset,          /* Pointer to Parameter varialble  */
    &pncal_fantimeoffset,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE82 : 突防回路寿命 */
/***************************************************************************/
static void *pndefGetRamPtr_RushLifeTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->RushLifeTime);}

const PRM_ATTR_T pndef_RushLifeTime = {
    0xE82,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    166,                                    /* EEPROM address */
    1,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1000,                                   /* Default value */
    &pndefGetRamPtr_RushLifeTime,           /* Pointer to Parameter varialble  */
    &pncal_rushlifetime,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE83 : DB回路寿命 */
/***************************************************************************/
static void *pndefGetRamPtr_DBLifeTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->DBLifeTime);}

const PRM_ATTR_T pndef_DBLifeTime = {
    0xE83,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1458,                                   /* EEPROM address */
    1,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_DBLifeTime,             /* Pointer to Parameter varialble  */
    &pncal_dblifetime,                      /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE84 : 現在の突防リレー動作回数 */
/***************************************************************************/
static void *pndefGetRamPtr_RushSwichCount(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->RushSwichCount);}

const PRM_ATTR_T pndef_RushSwichCount = {
    0xE84,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    16,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RushSwichCount,         /* Pointer to Parameter varialble  */
    &pncal_rushswitchcount,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE86 : 現在のDB動作回数 */
/***************************************************************************/
static void *pndefGetRamPtr_DBSwichCount(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->DBSwichCount);}

const PRM_ATTR_T pndef_DBSwichCount = {
    0xE86,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    60,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_DBSwichCount,           /* Pointer to Parameter varialble  */
    &pncal_dbswitchcount,                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE88 : ＤＣ充電完了待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_ChgWaitTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->ChgWaitTime);}

const PRM_ATTR_T pndef_ChgWaitTime = {
    0xE88,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    148,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_ChgWaitTime,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE89 : パワーレディ待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_PrdyWaitTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->PrdyWaitTime);}

const PRM_ATTR_T pndef_PrdyWaitTime = {
    0xE89,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    150,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    500,                                    /* Default value */
    &pndefGetRamPtr_PrdyWaitTime,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE8A : 急速放電継続閾値電圧 */
/***************************************************************************/
static void *pndefGetRamPtr_DischCompVlt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->DischCompVlt);}

const PRM_ATTR_T pndef_DischCompVlt = {
    0xE8A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    168,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_DischCompVlt,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE8C : シリアル変換ユニット原点オフセット */
/***************************************************************************/
static void *pndefGetRamPtr_scnvorgoffset(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->scnvorgoffset);}

const PRM_ATTR_T pndef_scnvorgoffset = {
    0xE8C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1382,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    9000,                                   /* Upper Limit */
    4500,                                   /* Default value */
    &pndefGetRamPtr_scnvorgoffset,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE8D : 電流零位相位置 */
/***************************************************************************/
static void *pndefGetRamPtr_phsofst(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->phsofst);}

const PRM_ATTR_T pndef_phsofst = {
    0xE8D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1384,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    32768,                                  /* Default value */
    &pndefGetRamPtr_phsofst,                /* Pointer to Parameter varialble  */
    &pncal_phsofst,                         /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE90 : 低速過負荷検出速度レベル */
/***************************************************************************/
static void *pndefGetRamPtr_olspdlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->olspdlvl);}

const PRM_ATTR_T pndef_olspdlvl = {
    0xE90,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1386,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_olspdlvl,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE91 : 低速過負荷検出パラメータ */
/***************************************************************************/
static void *pndefGetRamPtr_ollspdprm(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ollspdprm);}

const PRM_ATTR_T pndef_ollspdprm = {
    0xE91,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1388,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x0000,                                 /* Default value */
    &pndefGetRamPtr_ollspdprm,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE93 : ＤＢ過負荷検出電力１ */
/***************************************************************************/
static void *pndefGetRamPtr_dbol_warn(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dbol_warn);}

const PRM_ATTR_T pndef_dbol_warn = {
    0xE93,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1390,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dbol_warn,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE94 : ＤＢ過負荷検出電力２ */
/***************************************************************************/
static void *pndefGetRamPtr_dbol_alm(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dbol_alm);}

const PRM_ATTR_T pndef_dbol_alm = {
    0xE94,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1392,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_dbol_alm,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE9A : 回生同期時の回生ONレベル調整値 */
/***************************************************************************/
static void *pndefGetRamPtr_regsynonlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->regsynonlvl);}

const PRM_ATTR_T pndef_regsynonlvl = {
    0xE9A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    172,                                    /* EEPROM address */
    -10,                                    /* Lower Limit */
    0,                                      /* Upper Limit */
    -5,                                     /* Default value */
    &pndefGetRamPtr_regsynonlvl,            /* Pointer to Parameter varialble  */
    &pncal_regsynonlvl,                     /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE9B : 回生同期時の回生ON待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_regsynontime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->regsynontime);}

const PRM_ATTR_T pndef_regsynontime = {
    0xE9B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    174,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    10,                                     /* Upper Limit */
    2,                                      /* Default value */
    &pndefGetRamPtr_regsynontime,           /* Pointer to Parameter varialble  */
    &pncal_regsynontime,                    /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE9C : 制御基板異常温度/警告温度 */
/***************************************************************************/
static void *pndefGetRamPtr_ctrlboardalmtmpr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->ctrlboardalmtmpr);}

const PRM_ATTR_T pndef_ctrlboardalmtmpr = {
    0xE9C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    152,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0x6464,                                 /* Default value */
    &pndefGetRamPtr_ctrlboardalmtmpr,       /* Pointer to Parameter varialble  */
    &pncal_ctrlboardalmtmpr,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE9D : パワー基板異常温度/警告温度 */
/***************************************************************************/
static void *pndefGetRamPtr_pwrboardalmtmpr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->pwrboardalmtmpr);}

const PRM_ATTR_T pndef_pwrboardalmtmpr = {
    0xE9D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    154,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0x6464,                                 /* Default value */
    &pndefGetRamPtr_pwrboardalmtmpr,        /* Pointer to Parameter varialble  */
    &pncal_pwrboardalmtmpr,                 /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE9E : 制御基板異常クリア可能温度/異常温度検出時間 */
/***************************************************************************/
static void *pndefGetRamPtr_ctrlboardalmclrtmpr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->ctrlboardalmclrtmpr);}

const PRM_ATTR_T pndef_ctrlboardalmclrtmpr = {
    0xE9E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    156,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0x5A0A,                                 /* Default value */
    &pndefGetRamPtr_ctrlboardalmclrtmpr,    /* Pointer to Parameter varialble  */
    &pncal_ctrlboardalmclrtmpr,             /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnE9F : パワー基板異常クリア可能温度/異常温度検出時間 */
/***************************************************************************/
static void *pndefGetRamPtr_pwrboardalmclrtmpr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->pwrboardalmclrtmpr);}

const PRM_ATTR_T pndef_pwrboardalmclrtmpr = {
    0xE9F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    158,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0x5A0A,                                 /* Default value */
    &pndefGetRamPtr_pwrboardalmclrtmpr,     /* Pointer to Parameter varialble  */
    &pncal_pwrboardalmclrtmpr,              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEA4 : 静音モード電流ループフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_ifil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ifil2);}

const PRM_ATTR_T pndef_ifil2 = {
    0xEA4,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1394,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_ifil2,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEA5 : 静音モードPWM周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_pwmf2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pwmf2);}

const PRM_ATTR_T pndef_pwmf2 = {
    0xEA5,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1396,                                   /* EEPROM address */
    2000,                                   /* Lower Limit */
    65535,                                  /* Upper Limit */
    8000,                                   /* Default value */
    &pndefGetRamPtr_pwmf2,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEA6 : キャリア周波数切り替え電流閾値 */
/***************************************************************************/
static void *pndefGetRamPtr_ilimlvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->ilimlvl);}

const PRM_ATTR_T pndef_ilimlvl = {
    0xEA6,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1398,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    800,                                    /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_ilimlvl,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEA7 : 第2PWM周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_pwmf3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->pwmf3);}

const PRM_ATTR_T pndef_pwmf3 = {
    0xEA7,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1400,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10000,                                  /* Default value */
    &pndefGetRamPtr_pwmf3,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEA8 : 第２速度Ｆ/Ｂフィルタ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_fbfil2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->fbfil2);}

const PRM_ATTR_T pndef_fbfil2 = {
    0xEA8,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1402,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_fbfil2,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEB0 : 電圧フィードバックループ比例ゲイン */
/***************************************************************************/
static void *pndefGetRamPtr_kv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->kv);}

const PRM_ATTR_T pndef_kv = {
    0xEB0,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1404,                                   /* EEPROM address */
    10,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    30,                                     /* Default value */
    &pndefGetRamPtr_kv,                     /* Pointer to Parameter varialble  */
    &pncal_kv,                              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEB1 : 電圧フィードバック積分時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_tv(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->tv);}

const PRM_ATTR_T pndef_tv = {
    0xEB1,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1406,                                   /* EEPROM address */
    10,                                     /* Lower Limit */
    1000,                                   /* Upper Limit */
    16,                                     /* Default value */
    &pndefGetRamPtr_tv,                     /* Pointer to Parameter varialble  */
    &pncal_tv,                              /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEB2 : 電圧フィードバック積分リミット */
/***************************************************************************/
static void *pndefGetRamPtr_idlim(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->idlim);}

const PRM_ATTR_T pndef_idlim = {
    0xEB2,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1408,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    200,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_idlim,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEB3 : 電圧指令制限レベル */
/***************************************************************************/
static void *pndefGetRamPtr_v1max(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->v1max);}

const PRM_ATTR_T pndef_v1max = {
    0xEB3,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1410,                                   /* EEPROM address */
    50,                                     /* Lower Limit */
    150,                                    /* Upper Limit */
    115,                                    /* Default value */
    &pndefGetRamPtr_v1max,                  /* Pointer to Parameter varialble  */
    &pncal_v1max,                           /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEB4 : d軸電流指令リミット用電圧制限レベル */
/***************************************************************************/
static void *pndefGetRamPtr_vmaxid(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vmaxid);}

const PRM_ATTR_T pndef_vmaxid = {
    0xEB4,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1412,                                   /* EEPROM address */
    50,                                     /* Lower Limit */
    150,                                    /* Upper Limit */
    95,                                     /* Default value */
    &pndefGetRamPtr_vmaxid,                 /* Pointer to Parameter varialble  */
    &pncal_vmaxid,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEB5 : 主回路電圧平均時間 */
/***************************************************************************/
static void *pndefGetRamPtr_vpnavg(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->vpnavg);}

const PRM_ATTR_T pndef_vpnavg = {
    0xEB5,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1414,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    20,                                     /* Default value */
    &pndefGetRamPtr_vpnavg,                 /* Pointer to Parameter varialble  */
    &pncal_vpnavg,                          /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED0 : 自動ノッチ完了状態 */
/***************************************************************************/
static void *pndefGetRamPtr_PnED0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED0);}

const PRM_ATTR_T pndef_PnED0 = {
    0xED0,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1420,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnED0,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED1 : 自動ノッチ最大振動値 */
/***************************************************************************/
static void *pndefGetRamPtr_PnED1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED1);}

const PRM_ATTR_T pndef_PnED1 = {
    0xED1,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    4,                                      /* Data Length */
    0,                                      /* Number of Array */
    1424,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    1073741824,                             /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnED1,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED3 : ノッチ周波数１回目補正係数１ */
/***************************************************************************/
static void *pndefGetRamPtr_PnED3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED3);}

const PRM_ATTR_T pndef_PnED3 = {
    0xED3,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1428,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    2000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnED3,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED4 : ノッチ周波数１回目補正係数２ */
/***************************************************************************/
static void *pndefGetRamPtr_PnED4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED4);}

const PRM_ATTR_T pndef_PnED4 = {
    0xED4,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1430,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    135,                                    /* Default value */
    &pndefGetRamPtr_PnED4,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED5 : ノッチ周波数１回目補正範囲閾値 */
/***************************************************************************/
static void *pndefGetRamPtr_PnED5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED5);}

const PRM_ATTR_T pndef_PnED5 = {
    0xED5,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1432,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    2000,                                   /* Upper Limit */
    800,                                    /* Default value */
    &pndefGetRamPtr_PnED5,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED6 : ノッチ周波数２回目補正係数１ */
/***************************************************************************/
static void *pndefGetRamPtr_PnED6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED6);}

const PRM_ATTR_T pndef_PnED6 = {
    0xED6,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1434,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    2000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_PnED6,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED7 : ノッチ周波数２回目補正係数２ */
/***************************************************************************/
static void *pndefGetRamPtr_PnED7(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED7);}

const PRM_ATTR_T pndef_PnED7 = {
    0xED7,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1436,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    130,                                    /* Default value */
    &pndefGetRamPtr_PnED7,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED8 : ノッチ周波数２回目補正範囲閾値 */
/***************************************************************************/
static void *pndefGetRamPtr_PnED8(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED8);}

const PRM_ATTR_T pndef_PnED8 = {
    0xED8,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1438,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    2000,                                   /* Upper Limit */
    800,                                    /* Default value */
    &pndefGetRamPtr_PnED8,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnED9 : 自動ノッチ振動チェック時の停止判別待ち時間 */
/***************************************************************************/
static void *pndefGetRamPtr_PnED9(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->PnED9);}

const PRM_ATTR_T pndef_PnED9 = {
    0xED9,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1440,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    10000,                                  /* Upper Limit */
    200,                                    /* Default value */
    &pndefGetRamPtr_PnED9,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEDA : 調整ﾚｽ外乱ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ */
/***************************************************************************/
static void *pndefGetRamPtr_TunLessGain(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->TunLessGain);}

const PRM_ATTR_T pndef_TunLessGain = {
    0xEDA,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1442,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    6000,                                   /* Default value */
    &pndefGetRamPtr_TunLessGain,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEDB : 調整ﾚｽｲﾅｰｼｬ補正ｹﾞｲﾝ */
/***************************************************************************/
static void *pndefGetRamPtr_TunLessStiff(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->TunLessStiff);}

const PRM_ATTR_T pndef_TunLessStiff = {
    0xEDB,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1444,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    100,                                    /* Default value */
    &pndefGetRamPtr_TunLessStiff,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEDC : 調整ﾚｽ外乱ﾄﾙｸﾛｰﾊﾟｽﾌｨﾙﾀ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_TunLessTrqLpf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->TunLessTrqLpf);}

const PRM_ATTR_T pndef_TunLessTrqLpf = {
    0xEDC,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1446,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_TunLessTrqLpf,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEDD : 調整ﾚｽ速度FBﾛｰﾊﾟｽﾌｨﾙﾀ時定数 */
/***************************************************************************/
static void *pndefGetRamPtr_TunLessVfbLpf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->TunLessVfbLpf);}

const PRM_ATTR_T pndef_TunLessVfbLpf = {
    0xEDD,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1448,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_TunLessVfbLpf,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE0 : モータレステスト機能仮想モータ速度 */
/***************************************************************************/
static void *pndefGetRamPtr_MLess_vrat_max(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MLess_vrat_max);}

const PRM_ATTR_T pndef_MLess_vrat_max = {
    0xEE0,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1460,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_MLess_vrat_max,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE1 : モータレステスト機能仮想モータＯＳ検出レベル, ポール数 */
/***************************************************************************/
static void *pndefGetRamPtr_MLess_oslv_pol(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MLess_oslv_pol);}

const PRM_ATTR_T pndef_MLess_oslv_pol = {
    0xEE1,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1462,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_MLess_oslv_pol,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE2 : モータレステスト機能仮想モータロータイナーシャ */
/***************************************************************************/
static void *pndefGetRamPtr_Mless_jmot(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->Mless_jmot);}

const PRM_ATTR_T pndef_Mless_jmot = {
    0xEE2,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1464,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_Mless_jmot,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE3 : モータレステスト機能仮想モータ指数パラメータ */
/***************************************************************************/
static void *pndefGetRamPtr_Mless_exp_spd_jmot(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->Mless_exp_spd_jmot);}

const PRM_ATTR_T pndef_Mless_exp_spd_jmot = {
    0xEE3,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1466,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_Mless_exp_spd_jmot,     /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE5 : ファン停止アラーム遷移時間 */
/***************************************************************************/
static void *pndefGetRamPtr_FanLockTime(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->FanLockTime);}

const PRM_ATTR_T pndef_FanLockTime = {
    0xEE5,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    160,                                    /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_FanLockTime,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE6 : サーボオン時内蔵ファン停止アラーム検出時間 */
/***************************************************************************/
static void *pndefGetRamPtr_SvonFanLock_Time(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->SvonFanLock_Time);}

const PRM_ATTR_T pndef_SvonFanLock_Time = {
    0xEE6,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    170,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    60,                                     /* Upper Limit */
    10,                                     /* Default value */
    &pndefGetRamPtr_SvonFanLock_Time,       /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE8 : ＤＱ用スイッチ(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_dq_sw_L(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->dq_sw_L);}

const PRM_ATTR_T pndef_dq_sw_L = {
    0xEE8,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASENBL,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1450,                                   /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x2D0A,                                 /* Default value */
    &pndefGetRamPtr_dq_sw_L,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEE9 : 電圧指令制限レベル */
/***************************************************************************/
static void *pndefGetRamPtr_v1max_L(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->v1max_L);}

const PRM_ATTR_T pndef_v1max_L = {
    0xEE9,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1452,                                   /* EEPROM address */
    50,                                     /* Lower Limit */
    150,                                    /* Upper Limit */
    127,                                    /* Default value */
    &pndefGetRamPtr_v1max_L,                /* Pointer to Parameter varialble  */
    &pncal_v1max_for_linear,                /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEEA : 調整ﾚｽ外乱ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_TunLessGain_L(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->TunLessGain_L);}

const PRM_ATTR_T pndef_TunLessGain_L = {
    0xEEA,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1454,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    6000,                                   /* Default value */
    &pndefGetRamPtr_TunLessGain_L,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEEB : 調整ﾚｽｲﾅｰｼｬ補正ｹﾞｲﾝ(リニア) */
/***************************************************************************/
static void *pndefGetRamPtr_TunLessStiff_L(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->TunLessStiff_L);}

const PRM_ATTR_T pndef_TunLessStiff_L = {
    0xEEB,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    1456,                                   /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    50,                                     /* Default value */
    &pndefGetRamPtr_TunLessStiff_L,         /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnEF5 : 主回路電圧検出補正用基準電圧 */
/***************************************************************************/
static void *pndefGetRamPtr_DcBaseLvl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Cprm)->DcBaseLvl);}

const PRM_ATTR_T pndef_DcBaseLvl = {
    0xEF5,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    TRUE,                                   /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    162,                                    /* EEPROM address */
    0,                                      /* Lower Limit */
    1000,                                   /* Upper Limit */
    395,                                    /* Default value */
    &pndefGetRamPtr_DcBaseLvl,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF00 : モータ形式(8bit), 入力電圧(4bit), エンコーダタイプ(4bit) */
/***************************************************************************/
static void *pndefGetRamPtr_typm_e(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.typm_e);}

const PRM_ATTR_T pndef_typm_e = {
    0xF00,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_typm_e,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF01 : エンコーダソフトバージョン */
/***************************************************************************/
static void *pndefGetRamPtr_encver(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.encver);}

const PRM_ATTR_T pndef_encver = {
    0xF01,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_encver,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF02 : モータ容量 */
/***************************************************************************/
static void *pndefGetRamPtr_motw(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motw);}

const PRM_ATTR_T pndef_motw = {
    0xF02,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motw,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF03 : エンコーダビット数, 位置データ小数点位置 */
/***************************************************************************/
static void *pndefGetRamPtr_bit_dp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.bit_dp);}

const PRM_ATTR_T pndef_bit_dp = {
    0xF03,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_bit_dp,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF04 : マルチターンリミット */
/***************************************************************************/
static void *pndefGetRamPtr_limmlt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.limmlt);}

const PRM_ATTR_T pndef_limmlt = {
    0xF04,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_limmlt,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF05 : 定格速度, 最大速度 */
/***************************************************************************/
static void *pndefGetRamPtr_vrat_max(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.vrat_max);}

const PRM_ATTR_T pndef_vrat_max = {
    0xF05,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x321E,                                 /* Default value */
    &pndefGetRamPtr_vrat_max,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF06 : ＯＳ検出レベル, ポール数 */
/***************************************************************************/
static void *pndefGetRamPtr_oslv_pol(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.oslv_pol);}

const PRM_ATTR_T pndef_oslv_pol = {
    0xF06,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0x0000,                                 /* Lower Limit */
    0xFFFF,                                 /* Upper Limit */
    0x060A,                                 /* Default value */
    &pndefGetRamPtr_oslv_pol,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF07 : 定格トルク */
/***************************************************************************/
static void *pndefGetRamPtr_rattrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.rattrq);}

const PRM_ATTR_T pndef_rattrq = {
    0xF07,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_rattrq,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF08 : 最大トルク */
/***************************************************************************/
static void *pndefGetRamPtr_maxtrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.maxtrq);}

const PRM_ATTR_T pndef_maxtrq = {
    0xF08,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_maxtrq,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF09 : 定格電流 */
/***************************************************************************/
static void *pndefGetRamPtr_irat(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.irat);}

const PRM_ATTR_T pndef_irat = {
    0xF09,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_irat,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF0A : 瞬時最大電流 */
/***************************************************************************/
static void *pndefGetRamPtr_imax(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.imax);}

const PRM_ATTR_T pndef_imax = {
    0xF0A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_imax,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF0B : ＥＭＦ定数 */
/***************************************************************************/
static void *pndefGetRamPtr_emfcmp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.emfcmp);}

const PRM_ATTR_T pndef_emfcmp = {
    0xF0B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    1,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_emfcmp,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF0C : ロータイナーシャ */
/***************************************************************************/
static void *pndefGetRamPtr_jmot(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.jmot);}

const PRM_ATTR_T pndef_jmot = {
    0xF0C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    1,                                      /* Default value */
    &pndefGetRamPtr_jmot,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF0D : 電機子巻線抵抗 */
/***************************************************************************/
static void *pndefGetRamPtr_motr(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motr);}

const PRM_ATTR_T pndef_motr = {
    0xF0D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    3,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motr,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF0E : 電機子インダクタンス */
/***************************************************************************/
static void *pndefGetRamPtr_motl(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motl);}

const PRM_ATTR_T pndef_motl = {
    0xF0E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motl,                   /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF0F : 過負荷検出ベース電流 */
/***************************************************************************/
static void *pndefGetRamPtr_bastrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.bastrq);}

const PRM_ATTR_T pndef_bastrq = {
    0xF0F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_bastrq,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF10 : 過負荷検出中間電流 */
/***************************************************************************/
static void *pndefGetRamPtr_midtrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.midtrq);}

const PRM_ATTR_T pndef_midtrq = {
    0xF10,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_midtrq,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF11 : 過負荷検出中間時間 */
/***************************************************************************/
static void *pndefGetRamPtr_midolt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.midolt);}

const PRM_ATTR_T pndef_midolt = {
    0xF11,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_midolt,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF12 : 過負荷検出中間電流２ */
/***************************************************************************/
static void *pndefGetRamPtr_mtrq2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mtrq2);}

const PRM_ATTR_T pndef_mtrq2 = {
    0xF12,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mtrq2,                  /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF13 : 過負荷検出中間時間２ */
/***************************************************************************/
static void *pndefGetRamPtr_mtolt2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mtolt2);}

const PRM_ATTR_T pndef_mtolt2 = {
    0xF13,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mtolt2,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF14 : 位相補償１(補償値,開始速度) */
/***************************************************************************/
static void *pndefGetRamPtr_phscmpM1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.phscmpM1);}

const PRM_ATTR_T pndef_phscmpM1 = {
    0xF14,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_phscmpM1,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF15 : ポールピッチ */
/***************************************************************************/
static void *pndefGetRamPtr_ppitch(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.ppitch);}

const PRM_ATTR_T pndef_ppitch = {
    0xF15,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_ppitch,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF16 : q軸インダクタンスLq0 */
/***************************************************************************/
static void *pndefGetRamPtr_motlq0(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motlq0);}

const PRM_ATTR_T pndef_motlq0 = {
    0xF16,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motlq0,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF17 : q軸インダクタンスLq1 */
/***************************************************************************/
static void *pndefGetRamPtr_motlq1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motlq1);}

const PRM_ATTR_T pndef_motlq1 = {
    0xF17,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motlq1,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF18 : 下位：定格トルク指数、上位：ロータイナーシャ指数 */
/***************************************************************************/
static void *pndefGetRamPtr_exp_trat_jmot(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.exp_trat_jmot);}

const PRM_ATTR_T pndef_exp_trat_jmot = {
    0xF18,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_exp_trat_jmot,          /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF19 : 下位：定格出力指数、上位：回転数指数 */
/***************************************************************************/
static void *pndefGetRamPtr_exp_motw_spd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.exp_motw_spd);}

const PRM_ATTR_T pndef_exp_motw_spd = {
    0xF19,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_exp_motw_spd,           /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF1A : フルクエンコーダビット数, 位置データ小数点位置 */
/***************************************************************************/
static void *pndefGetRamPtr_BitnoDpoint(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.BitnoDpoint);}

const PRM_ATTR_T pndef_BitnoDpoint = {
    0xF1A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_BitnoDpoint,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF1B : ポールセンサオフセット量 */
/***************************************************************************/
static void *pndefGetRamPtr_poleoffset(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.poleoffset);}

const PRM_ATTR_T pndef_poleoffset = {
    0xF1B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_poleoffset,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF1C : モータ共振抑制用ノッチフィルタ周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_motresfrq(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motresfrq);}

const PRM_ATTR_T pndef_motresfrq = {
    0xF1C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    2560,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motresfrq,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF1D : 位相角補正値 */
/***************************************************************************/
static void *pndefGetRamPtr_phscmpM2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.phscmpM2);}

const PRM_ATTR_T pndef_phscmpM2 = {
    0xF1D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_phscmpM2,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF1E : フラグ */
/***************************************************************************/
static void *pndefGetRamPtr_flg_wf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.flg_wf);}

const PRM_ATTR_T pndef_flg_wf = {
    0xF1E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_flg_wf,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF24 : 機能フラグ２ */
/***************************************************************************/
static void *pndefGetRamPtr_motfuncf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.motfuncf.ul);}

const PRM_ATTR_T pndef_motfuncf = {
    0xF24,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_motfuncf,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF25 : モータPWM周波数 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_pwmf(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_pwmf);}

const PRM_ATTR_T pndef_mot_pwmf = {
    0xF25,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_pwmf,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF26 : モータ低速過負荷検出ベース電流 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_ibs(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_ibs);}

const PRM_ATTR_T pndef_mot_ibs = {
    0xF26,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_ibs,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF27 : モータ低速過負荷検出中間電流 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_imd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_imd);}

const PRM_ATTR_T pndef_mot_imd = {
    0xF27,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_imd,                /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF28 : モータ低速過負荷検出中間時間 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_imdt(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_imdt);}

const PRM_ATTR_T pndef_mot_imdt = {
    0xF28,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_imdt,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF29 : モータ低速過負荷検出中間電流２ */
/***************************************************************************/
static void *pndefGetRamPtr_mot_imd2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_imd2);}

const PRM_ATTR_T pndef_mot_imd2 = {
    0xF29,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_imd2,               /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF2A : モータ低速過負荷検出中間時間２ */
/***************************************************************************/
static void *pndefGetRamPtr_mot_imdt2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_imdt2);}

const PRM_ATTR_T pndef_mot_imdt2 = {
    0xF2A,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_imdt2,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF2B : モータ低速過負荷検出回転速度閾値 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_ichgspd(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_ichgspd);}

const PRM_ATTR_T pndef_mot_ichgspd = {
    0xF2B,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_ichgspd,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF2C : モータOC検出電流 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_oc(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_oc);}

const PRM_ATTR_T pndef_mot_oc = {
    0xF2C,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_oc,                 /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF2D : 調整レスモード選択 */
/***************************************************************************/
static void *pndefGetRamPtr_mot_TunLess(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.mot_TunLess);}

const PRM_ATTR_T pndef_mot_TunLess = {
    0xF2D,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_mot_TunLess,            /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF2E : 弱め界磁モータ調整値(電圧指令,d軸電流指令) */
/***************************************************************************/
static void *pndefGetRamPtr_RefAdjust_ForWF(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.RefAdjust_ForWF);}

const PRM_ATTR_T pndef_RefAdjust_ForWF = {
    0xF2E,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RefAdjust_ForWF,        /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF2F : 弱め界磁L補正係数 */
/***************************************************************************/
static void *pndefGetRamPtr_LCompForWF(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.LCompForWF);}

const PRM_ATTR_T pndef_LCompForWF = {
    0xF2F,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_LCompForWF,             /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnF30 : 回生時位相進み補償(開始速度,最大回転速度) */
/***************************************************************************/
static void *pndefGetRamPtr_RegPhscmp(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->MencP.RegPhscmp);}

const PRM_ATTR_T pndef_RegPhscmp = {
    0xF30,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    TRUE,                                   /* Need Reboot */
    FALSE,                                  /* With Sign */
    TRUE,                                   /* Read Only */
    PRMDEF_BASEHEX,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,/* SpUse */
    0,                                      /* - */
    PRMDEF_NO_EEPROM,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    65535,                                  /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RegPhscmp,              /* Pointer to Parameter varialble  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA0 : 速度リップル補償ゲイン(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompGainE(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompGainE);}

const PRM_ATTR_T pndef_RippleCompGainE = {
    0xFA0,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    22,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    300,                                    /* Upper Limit */
    80,                                     /* Default value */
    &pndefGetRamPtr_RippleCompGainE,        /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA1 : 速度リップル補償第1成分周期(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompCycle1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompCycleE[0]);}

const PRM_ATTR_T pndef_RippleCompCycle1 = {
    0xFA1,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    24,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompCycle1,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA2 : 速度リップル補償第1成分振幅(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompAmplitude1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompAmplitudeE[0]);}

const PRM_ATTR_T pndef_RippleCompAmplitude1 = {
    0xFA2,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    26,                                     /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompAmplitude1,   /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA3 : 速度リップル補償第1成分位相(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompPhase1(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompPhaseE[0]);}

const PRM_ATTR_T pndef_RippleCompPhase1 = {
    0xFA3,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    28,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompPhase1,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA4 : 速度リップル補償第2成分周期(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompCycle2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompCycleE[1]);}

const PRM_ATTR_T pndef_RippleCompCycle2 = {
    0xFA4,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    30,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompCycle2,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA5 : 速度リップル補償第2成分振幅(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompAmplitude2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompAmplitudeE[1]);}

const PRM_ATTR_T pndef_RippleCompAmplitude2 = {
    0xFA5,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    32,                                     /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompAmplitude2,   /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA6 : 速度リップル補償第2成分位相(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompPhase2(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompPhaseE[1]);}

const PRM_ATTR_T pndef_RippleCompPhase2 = {
    0xFA6,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    34,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompPhase2,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA7 : 速度リップル補償第3成分周期(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompCycle3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompCycleE[2]);}

const PRM_ATTR_T pndef_RippleCompCycle3 = {
    0xFA7,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    36,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompCycle3,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA8 : 速度リップル補償第3成分振幅(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompAmplitude3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompAmplitudeE[2]);}

const PRM_ATTR_T pndef_RippleCompAmplitude3 = {
    0xFA8,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    38,                                     /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompAmplitude3,   /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFA9 : 速度リップル補償第3成分位相(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompPhase3(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompPhaseE[2]);}

const PRM_ATTR_T pndef_RippleCompPhase3 = {
    0xFA9,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    40,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompPhase3,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFAA : 速度リップル補償第4成分周期(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompCycle4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompCycleE[3]);}

const PRM_ATTR_T pndef_RippleCompCycle4 = {
    0xFAA,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    42,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompCycle4,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFAB : 速度リップル補償第4成分振幅(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompAmplitude4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompAmplitudeE[3]);}

const PRM_ATTR_T pndef_RippleCompAmplitude4 = {
    0xFAB,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    44,                                     /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompAmplitude4,   /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFAC : 速度リップル補償第4成分位相(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompPhase4(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompPhaseE[3]);}

const PRM_ATTR_T pndef_RippleCompPhase4 = {
    0xFAC,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    46,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompPhase4,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFAD : 速度リップル補償第5成分周期(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompCycle5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompCycleE[4]);}

const PRM_ATTR_T pndef_RippleCompCycle5 = {
    0xFAD,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    48,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompCycle5,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFAE : 速度リップル補償第5成分振幅(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompAmplitude5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompAmplitudeE[4]);}

const PRM_ATTR_T pndef_RippleCompAmplitude5 = {
    0xFAE,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    50,                                     /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompAmplitude5,   /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFAF : 速度リップル補償第5成分位相(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompPhase5(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompPhaseE[4]);}

const PRM_ATTR_T pndef_RippleCompPhase5 = {
    0xFAF,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    52,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompPhase5,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFB0 : 速度リップル補償第6成分周期(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompCycle6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompCycleE[5]);}

const PRM_ATTR_T pndef_RippleCompCycle6 = {
    0xFB0,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    54,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    100,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompCycle6,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFB1 : 速度リップル補償第6成分振幅(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompAmplitude6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompAmplitudeE[5]);}

const PRM_ATTR_T pndef_RippleCompAmplitude6 = {
    0xFB1,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    TRUE,                                   /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    2,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    56,                                     /* EEPROM address */
    -1000,                                  /* Lower Limit */
    1000,                                   /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompAmplitude6,   /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};


/***************************************************************************/
/*  Attribute of PnFB2 : 速度リップル補償第6成分位相(電気角) */
/***************************************************************************/
static void *pndefGetRamPtr_RippleCompPhase6(UINT ArrayIdx, void *AxParam)
{   return &((((AXIS_HANDLE*)AxParam)->UniPrms->Prm)->RippleCompPhaseE[5]);}

const PRM_ATTR_T pndef_RippleCompPhase6 = {
    0xFB2,                                  /* Register Number */
    ACCLVL_SYSTEM,                          /* Access Level A/P */
    FALSE,                                  /* 軸共通 */
    FALSE,                                  /* Need Reboot */
    FALSE,                                  /* With Sign */
    FALSE,                                  /* Read Only */
    PRMDEF_BASEDEC,                         /* Display format */
    0,                                      /* Decimals */
    0,                                      /* ClrVar */
    0,                                      /* RserveX */
    KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_TUNING,/* SpUse */
    0,                                      /* - */
    PRMDEF_EEPROM_IF,                       /* EEPROM device */
    2,                                      /* Data Length */
    0,                                      /* Number of Array */
    58,                                     /* EEPROM address */
    0,                                      /* Lower Limit */
    360,                                    /* Upper Limit */
    0,                                      /* Default value */
    &pndefGetRamPtr_RippleCompPhase6,       /* Pointer to Parameter varialble  */
    &pncal_VelRippleCompE,                  /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL PnPrmTbl[] = {
    {0x000   , &pndef_b_prm0                        },/* 0x000: 機能選択基本スイッチ０ */
    {0x001   , &pndef_b_prm1                        },/* 0x001: 機能選択応用スイッチ１ */
    {0x002   , &pndef_b_prm2                        },/* 0x002: 機能選択応用スイッチ２ */
    {0x006   , &pndef_b_prm6                        },/* 0x006: 機能選択応用スイッチ６ */
    {0x007   , &pndef_b_prm7                        },/* 0x007: 機能選択応用スイッチ７ */
    {0x008   , &pndef_b_prm8                        },/* 0x008: 機能選択応用スイッチ８ */
    {0x009   , &pndef_b_prm9                        },/* 0x009: 機能選択応用スイッチ９ */
    {0x00A   , &pndef_b_prmA                        },/* 0x00A: 機能選択応用スイッチA */
    {0x00B   , &pndef_b_prmB                        },/* 0x00B: 機能選択応用スイッチB */
    {0x00C   , &pndef_b_prmC                        },/* 0x00C: 機能選択応用スイッチC */
    {0x00D   , &pndef_b_prmD                        },/* 0x00D: 機能選択応用スイッチD */
    {0x00E   , &pndef_b_prmE                        },/* 0x00E: 機能選択応用スイッチE */
    {0x00F   , &pndef_b_prmF                        },/* 0x00F: 機能選択応用スイッチF */
    {0x010   , &pndef_AxisAddress0                  },/* 0x010: スロット0軸アドレス */
    {0x011   , &pndef_AxisAddress1                  },/* 0x011: スロット1軸アドレス */
    {0x020   , &pndef_UniPrmSel                     },/* 0x020: 桁毎ユニット共通パラメータ設定 */
    {0x041   , &pndef_CycleSwitch                   },/* 0x041: スキャン周期切り替えスイッチ */
    {0x080   , &pndef_b_prm80                       },/* 0x080: 機能選択応用スイッチ８０ */
    {0x081   , &pndef_b_prm81                       },/* 0x081: 機能選択応用スイッチ８１ */
    {0x100   , &pndef_loophz                        },/* 0x100: 速度ループゲイン */
    {0x101   , &pndef_pitime                        },/* 0x101: 速度ループ積分時定数 */
    {0x102   , &pndef_posgn                         },/* 0x102: 位置ループゲイン */
    {0x103   , &pndef_jrate                         },/* 0x103: 慣性モーメント比 */
    {0x104   , &pndef_loophz2                       },/* 0x104: 第２速度ループゲイン */
    {0x105   , &pndef_pitime2                       },/* 0x105: 第２速度ループ積分時定数 */
    {0x106   , &pndef_posgn2                        },/* 0x106: 第２位置ループゲイン */
    {0x109   , &pndef_ffgn                          },/* 0x109: フィードフォワード */
    {0x10A   , &pndef_fffil                         },/* 0x10A: フィードフォワードフィルタ時定数 */
    {0x10B   , &pndef_gnmode                        },/* 0x10B: ゲイン関係応用スイッチ */
    {0x10C   , &pndef_mdswlv                        },/* 0x10C: モードスイッチ(トルク指令) */
    {0x10D   , &pndef_mswrf                         },/* 0x10D: モードスイッチ(速度指令) */
    {0x10E   , &pndef_mswacc                        },/* 0x10E: モードスイッチ(加速度) */
    {0x10F   , &pndef_msplvl                        },/* 0x10F: モードスイッチ(位置偏差) */
    {0x11F   , &pndef_pitfil                        },/* 0x11F: 位置積分時定数 */
    {0x121   , &pndef_dobgn                         },/* 0x121: 摩擦補償ゲイン */
    {0x122   , &pndef_dobgn2                        },/* 0x122: 第２摩擦補償ゲイン */
    {0x123   , &pndef_dtrqgn                        },/* 0x123: 摩擦補償係数 */
    {0x124   , &pndef_dlpfil                        },/* 0x124: 摩擦補償周波数補正 */
    {0x125   , &pndef_dobjgn                        },/* 0x125: 摩擦補償ゲイン補正 */
    {0x127   , &pndef_evobgn                        },/* 0x127: 速度オブザーバゲイン */
    {0x128   , &pndef_evobjgn                       },/* 0x128: 速度オブザーバ位置補償ゲイン */
    {0x131   , &pndef_gnswfil1                      },/* 0x131: ゲイン切り替え時間１ */
    {0x132   , &pndef_gnswfil2                      },/* 0x132: ゲイン切り替え時間２ */
    {0x135   , &pndef_gnswwait1                     },/* 0x135: ゲイン切り替え待ち時間１ */
    {0x136   , &pndef_gnswwait2                     },/* 0x136: ゲイン切り替え待ち時間２ */
    {0x139   , &pndef_gnsw1                         },/* 0x139: 自動ゲイン切り替え関係スイッチ１ */
    {0x13B   , &pndef_Pn13B                         },/* 0x13B: 速度オブザーバローパスフィルタ時定数 */
    {0x13D   , &pndef_KcGainRatio                   },/* 0x13D: 電流ゲインレベル */
    {0x140   , &pndef_mdlsw                         },/* 0x140: モデル追従制御関連スイッチ */
    {0x141   , &pndef_mdlgn                         },/* 0x141: モデル追従制御ゲイン */
    {0x142   , &pndef_mdlzeta                       },/* 0x142: モデル追従制御ゲイン補正 */
    {0x143   , &pndef_mdlptff                       },/* 0x143: モデル追従制御バイアス(正転方向) */
    {0x144   , &pndef_mdlntff                       },/* 0x144: モデル追従制御バイアス(逆転方向) */
    {0x145   , &pndef_mdlantfrq                     },/* 0x145: 振動抑制１周波数Ａ */
    {0x146   , &pndef_mdlresfrq                     },/* 0x146: 振動抑制１周波数Ｂ */
    {0x147   , &pndef_mdlvff                        },/* 0x147: モデル追従制御速度フィードフォワード補償 */
    {0x148   , &pndef_mdlgn2                        },/* 0x148: 第２モデル追従制御ゲイン */
    {0x149   , &pndef_mdlzeta2                      },/* 0x149: 第２モデル追従制御ゲイン補正 */
    {0x14A   , &pndef_ff_frq                        },/* 0x14A: 振動抑制２周波数 */
    {0x14B   , &pndef_ff_fil                        },/* 0x14B: 振動抑制２補正 */
    {0x14F   , &pndef_mfctype                       },/* 0x14F: 制御関連スイッチ */
    {0x160   , &pndef_avibsw                        },/* 0x160: 制振制御関連スイッチ */
    {0x161   , &pndef_avibfrq                       },/* 0x161: Ａ型制振周波数 */
    {0x162   , &pndef_avibgn                        },/* 0x162: Ａ型制振ゲイン補正 */
    {0x163   , &pndef_avibdamp                      },/* 0x163: Ａ型制振ダンピングゲイン */
    {0x164   , &pndef_avibfil1                      },/* 0x164: Ａ型制振フィルタ時定数１補正 */
    {0x165   , &pndef_avibfil2                      },/* 0x165: Ａ型制振フィルタ時定数２補正 */
    {0x166   , &pndef_avibdamp2                     },/* 0x166: Ａ型制振ダンピングゲイン2 */
    {0x167   , &pndef_avibfrq2                      },/* 0x167: A型制振ハイパスフィルタ遮断周波数 */
    {0x170   , &pndef_DatLevel                      },/* 0x170: ダイナミックオートチューニング関連スイッチ */
    {0x17A   , &pndef_jrateSw                       },/* 0x17A: 予約パラメータ（イナーシャ比切り替え関係スイッチ） */
    {0x17B   , &pndef_jrate2                        },/* 0x17B: 第2イナーシャ比 */
    {0x17C   , &pndef_jrateChgTime1                 },/* 0x17C: イナーシャ比切り替え時間１ */
    {0x17D   , &pndef_jrateChgTime2                 },/* 0x17D: イナーシャ比切り替え時間２ */
    {0x181   , &pndef_mswrf_l                       },/* 0x181: モードスイッチ(速度指令) */
    {0x182   , &pndef_mswacc_l                      },/* 0x182: モードスイッチ(加速度) */
    {0x190   , &pndef_divlessSw                     },/* 0x190: 予約パラメータ（偏差レス�U関連スイッチ） */
    {0x191   , &pndef_velffgain                     },/* 0x191: 速度フィードフォワードゲイン */
    {0x192   , &pndef_trqffgain                     },/* 0x192: トルクフィードフォワードゲイン */
    {0x193   , &pndef_trqfftime                     },/* 0x193: トルクフィードフォワードフィルタ時定数 */
    {0x200   , &pndef_prstsw                        },/* 0x200: 位置制御指令形態選択スイッチ */
    {0x205   , &pndef_mtlmt                         },/* 0x205: マルチターンリミット設定 */
    {0x207   , &pndef_cmfilsw                       },/* 0x207: 位置指令機能スイッチ */
    {0x209   , &pndef_expbias1                      },/* 0x209: 位置指令加減速バイアス */
    {0x20A   , &pndef_fcplsml                       },/* 0x20A: 外付けスケールピッチ数 */
    {0x20E   , &pndef_ratb2l                        },/* 0x20E: 電子ギア比(分子) */
    {0x210   , &pndef_rata2l                        },/* 0x210: 電子ギア比(分母) */
    {0x212   , &pndef_pgrat2l                       },/* 0x212: ＰＧ分周比(パルス単位) */
    {0x216   , &pndef_expfil                        },/* 0x216: 位置指令加減速時定数 */
    {0x217   , &pndef_mafil                         },/* 0x217: 位置指令移動平均時間 */
    {0x218   , &pndef_MulFrq                        },/* 0x218: 指令パルス入力倍率 */
    {0x21B   , &pndef_prefHfilsmooth                },/* 0x21B: 位置指令高次フィルタスムージング割合 */
    {0x21C   , &pndef_prefHfilacctime               },/* 0x21C: 位置指令高次フィルタ加減速時間 */
    {0x22A   , &pndef_fcsel                         },/* 0x22A: フルクローズ制御選択スイッチ */
    {0x230   , &pndef_cmctrlexsw                    },/* 0x230: 位置制御拡張機能スイッチ */
    {0x231   , &pndef_blcm2l                        },/* 0x231: バックラッシ補正量 */
    {0x233   , &pndef_blfiltime                     },/* 0x233: バックラッシ補正時定数 */
    {0x240   , &pndef_perrclrsigtime                },/* 0x240: 位置偏差クリア信号入力最小時間間隔 */
    {0x250   , &pndef_encbaudrate                   },/* 0x250: エンコーダ通信速度選択 */
    {0x281   , &pndef_scalepgrat                    },/* 0x281: エンコーダ出力分解能 */
    {0x282   , &pndef_scalepitch2l                  },/* 0x282: リニアスケールピッチ */
    {0x2D0   , &pndef_psencernum                    },/* 0x2D0: 予約パラメータ（位置センサ出力数） */
    {0x300   , &pndef_vrefgn                        },/* 0x300: 速度指令入力ゲイン */
    {0x301   , &pndef_speed1                        },/* 0x301: 内部設定速度１(回転) */
    {0x302   , &pndef_speed2                        },/* 0x302: 内部設定速度２(回転) */
    {0x303   , &pndef_speed3                        },/* 0x303: 内部設定速度３(回転) */
    {0x304   , &pndef_jogspd                        },/* 0x304: ＪＯＧ速度(回転) */
    {0x305   , &pndef_sfsacc                        },/* 0x305: ソフトスタート加速時間 */
    {0x306   , &pndef_sfsdec                        },/* 0x306: ソフトスタート減速時間 */
    {0x307   , &pndef_srfil                         },/* 0x307: 速度指令フィルタ時定数 */
    {0x308   , &pndef_fbfil                         },/* 0x308: 速度Ｆ/Ｂフィルタ時定数 */
    {0x30A   , &pndef_svoffdectime                  },/* 0x30A: サーボオフ及び強制停止時の減速時間 */
    {0x30B   , &pndef_vrefmafil                     },/* 0x30B: 速度指令移動平均時間 */
    {0x30C   , &pndef_vffmafil                      },/* 0x30C: 速度フィードフォワード移動平均時間 */
    {0x310   , &pndef_vibsel                        },/* 0x310: 振動検出スイッチ */
    {0x311   , &pndef_vibsens                       },/* 0x311: 振動検出感度 */
    {0x312   , &pndef_vibnorm                       },/* 0x312: 振動検出レベル(回転) */
    {0x316   , &pndef_maxvel                        },/* 0x316: モータ最高速度(回転) */
    {0x324   , &pndef_Pn324                         },/* 0x324: 慣性モーメント同定開始レベル */
    {0x325   , &pndef_Pn325                         },/* 0x325: 慣性モーメント同定中の振動検出レベル(回転) */
    {0x380   , &pndef_speed1_l                      },/* 0x380: 内部設定速度１(リニア) */
    {0x381   , &pndef_speed2_l                      },/* 0x381: 内部設定速度２(リニア) */
    {0x382   , &pndef_speed3_l                      },/* 0x382: 内部設定速度３(リニア) */
    {0x383   , &pndef_jogspd_l                      },/* 0x383: ＪＯＧ速度(リニア) */
    {0x384   , &pndef_vibnorm_l                     },/* 0x384: 振動検出レベル(リニア) */
    {0x385   , &pndef_maxvel_l                      },/* 0x385: モータ最高速度(リニア) */
    {0x386   , &pndef_Pn387                         },/* 0x386: 慣性モーメント同定中の振動検出レベル(リニア) */
    {0x400   , &pndef_trefgn                        },/* 0x400: トルク指令入力ゲイン */
    {0x401   , &pndef_trqfil11                      },/* 0x401: １段目第１トルク指令フィルタ時定数 */
    {0x402   , &pndef_tlmtf                         },/* 0x402: 正転トルク制限 */
    {0x403   , &pndef_tlmtr                         },/* 0x403: 逆転トルク制限 */
    {0x404   , &pndef_outlmf1                       },/* 0x404: 正転側外部トルク制限 */
    {0x405   , &pndef_outlmr1                       },/* 0x405: 逆転側外部トルク制限 */
    {0x406   , &pndef_emgtrq                        },/* 0x406: 非常停止トルク */
    {0x407   , &pndef_tcrlmt                        },/* 0x407: トルク制御時の速度制限 */
    {0x408   , &pndef_tfuncsw                       },/* 0x408: トルク関係機能スイッチ */
    {0x409   , &pndef_ntchfil1                      },/* 0x409: １段目ノッチフィルタ周波数 */
    {0x40A   , &pndef_ntchq1                        },/* 0x40A: １段目ノッチフィルタＱ値 */
    {0x40B   , &pndef_ntchdamp1                     },/* 0x40B: １段目ノッチフィルタ深さ */
    {0x40C   , &pndef_ntchfil2                      },/* 0x40C: ２段目ノッチフィルタ周波数 */
    {0x40D   , &pndef_ntchq2                        },/* 0x40D: ２段目ノッチフィルタＱ値 */
    {0x40E   , &pndef_ntchdamp2                     },/* 0x40E: ２段目ノッチフィルタ深さ */
    {0x40F   , &pndef_secfil                        },/* 0x40F: ２段目２次トルク指令フィルタ周波数 */
    {0x410   , &pndef_secq                          },/* 0x410: ２段目２次トルク指令フィルタＱ値 */
    {0x412   , &pndef_trqfil12                      },/* 0x412: １段目第２トルク指令フィルタ時定数 */
    {0x415   , &pndef_treffil                       },/* 0x415: T-REFフィルタ時定数 */
    {0x416   , &pndef_tfuncsw2                      },/* 0x416: トルク(推力)関係機能スイッチ２ */
    {0x417   , &pndef_ntchfil3                      },/* 0x417: 3段目ノッチフィルタ周波数 */
    {0x418   , &pndef_ntchq3                        },/* 0x418: 3段目ノッチフィルタQ値 */
    {0x419   , &pndef_ntchdamp3                     },/* 0x419: 3段目ノッチフィルタ深さ */
    {0x41A   , &pndef_ntchfil4                      },/* 0x41A: 4段目ノッチフィルタ周波数 */
    {0x41B   , &pndef_ntchq4                        },/* 0x41B: 4段目ノッチフィルタQ値 */
    {0x41C   , &pndef_ntchdamp4                     },/* 0x41C: 4段目ノッチフィルタ深さ */
    {0x41D   , &pndef_ntchfil5                      },/* 0x41D: 5段目ノッチフィルタ周波数 */
    {0x41E   , &pndef_ntchq5                        },/* 0x41E: 5段目ノッチフィルタQ値 */
    {0x41F   , &pndef_ntchdamp5                     },/* 0x41F: 5段目ノッチフィルタ深さ */
    {0x423   , &pndef_RippleCompSw                  },/* 0x423: 速度リップル補償機能選択スイッチ */
    {0x424   , &pndef_UvTrqLmt                      },/* 0x424: 主回路電圧降下時トルク制限 */
    {0x425   , &pndef_UvTrqLmtOffFil                },/* 0x425: 主回路電圧降下時トルク制限解除時間 */
    {0x426   , &pndef_tffmafil                      },/* 0x426: トルクフィードフォワード移動平均時間 */
    {0x427   , &pndef_VelRippleCompSpd              },/* 0x427: 速度リップル補償有効速度 */
    {0x453   , &pndef_swpminfrq                     },/* 0x453: 掃引開始周波数 */
    {0x454   , &pndef_swpmaxfrq                     },/* 0x454: 掃引終了周波数 */
    {0x455   , &pndef_detfrqlmt                     },/* 0x455: 検出共振周波数下限値 */
    {0x456   , &pndef_ezffttrq                      },/* 0x456: 掃引トルク指令振幅 */
    {0x457   , &pndef_ezfftsw                       },/* 0x457: ＦＦＴ関係機能スイッチ */
    {0x460   , &pndef_anotchsw                      },/* 0x460: ノッチフィルタ調整スイッチ */
    {0x466   , &pndef_anotchsw2                     },/* 0x466: ノッチフィルタ調整スイッチ2 */
    {0x470   , &pndef_gcomptrq                      },/* 0x470: 重力補償トルク */
    {0x471   , &pndef_pqfriqcomptrq                 },/* 0x471: 正方向クーロン摩擦補償トルク */
    {0x472   , &pndef_nqfriqcomptrq                 },/* 0x472: 負方向クーロン摩擦補償トルク */
    {0x473   , &pndef_vfrigcomptrq                  },/* 0x473: 粘性摩擦補償トルク */
    {0x474   , &pndef_spdhys                        },/* 0x474: 内部設定速度hysteresis */
    {0x480   , &pndef_tcrlmt_l                      },/* 0x480: 推力制御時の速度制限 */
    {0x481   , &pndef_pdetloophz                    },/* 0x481: 磁極検出速度ループゲイン */
    {0x482   , &pndef_pdetpitime                    },/* 0x482: 磁極検出速度ループ積分時間 */
    {0x483   , &pndef_tlmtf_l                       },/* 0x483: 正転推力制限 */
    {0x484   , &pndef_tlmtr_l                       },/* 0x484: 逆転推力制限 */
    {0x485   , &pndef_pdetmaxspd                    },/* 0x485: 磁極検出指令速度 */
    {0x486   , &pndef_pdetmaxt                      },/* 0x486: 磁極検出指令加減速時間 */
    {0x487   , &pndef_pdetclmpt                     },/* 0x487: 磁極検出指令一定速時間 */
    {0x488   , &pndef_pdetwait                      },/* 0x488: 磁極検出指令待ち時間 */
    {0x48D   , &pndef_pdetrepeat                    },/* 0x48D: 磁極検出繰り返し回数 */
    {0x48E   , &pndef_pdetdistok                    },/* 0x48E: 磁極検出可動範囲 */
    {0x490   , &pndef_pdetjrate                     },/* 0x490: 磁極検出負荷レベル */
    {0x491   , &pndef_pdetintegless                 },/* 0x491: 磁極検出積分レスゲイン */
    {0x492   , &pndef_pdetinteglesst                },/* 0x492: 磁極検出積分レス開始時間 */
    {0x493   , &pndef_pdetmaxspd_r                  },/* 0x493: 磁極検出指令速度 */
    {0x494   , &pndef_pdetdistok_r                  },/* 0x494: 磁極検出可動範囲 */
    {0x495   , &pndef_pdetmaxtrq                    },/* 0x495: 磁極検出確認トルク指令 */
    {0x496   , &pndef_pdetinctime                   },/* 0x496: 磁極検出確認トルク指令加減時間 */
    {0x497   , &pndef_pdetconsttime                 },/* 0x497: 磁極検出確認トルク指令一定時間 */
    {0x498   , &pndef_pdetdegreeok                  },/* 0x498: 磁極検出誤差許容範囲 */
    {0x499   , &pndef_pdetrevlvspdref               },/* 0x499: 磁極検出速度指令時逆走レベル */
    {0x49A   , &pndef_pdetrevlvwait                 },/* 0x49A: 磁極検出待ち時間時逆走レベル */
    {0x49B   , &pndef_pdetoslevel                   },/* 0x49B: 磁極検出時過速度レベル */
    {0x501   , &pndef_clamp                         },/* 0x501: ゼロクランプレベル */
    {0x502   , &pndef_tgonlv                        },/* 0x502: 回転検出レベル */
    {0x503   , &pndef_vcmplv                        },/* 0x503: 速度一致信号出力幅 */
    {0x506   , &pndef_brktim                        },/* 0x506: ブレーキ指令−サーボオフ遅れ時間 */
    {0x507   , &pndef_brkspd                        },/* 0x507: ブレーキ指令出力速度レベル */
    {0x508   , &pndef_brkwai                        },/* 0x508: サーボオフ−ブレーキ指令待ち時間 */
    {0x509   , &pndef_acoftm                        },/* 0x509: 瞬停保持時間 */
    {0x50A   , &pndef_insel1                        },/* 0x50A: 入力信号選択1 */
    {0x50B   , &pndef_insel2                        },/* 0x50B: 入力信号選択2 */
    {0x50C   , &pndef_insel3                        },/* 0x50C: 入力信号選択3 */
    {0x50D   , &pndef_insel4                        },/* 0x50D: 入力信号選択4 */
    {0x50E   , &pndef_outsel1                       },/* 0x50E: 出力信号選択1 */
    {0x50F   , &pndef_outsel2                       },/* 0x50F: 出力信号選択2 */
    {0x510   , &pndef_outsel3                       },/* 0x510: 出力信号選択3 */
    {0x511   , &pndef_inselA                        },/* 0x511: 入力信号選択5 */
    {0x512   , &pndef_sorev1                        },/* 0x512: 出力信号反転設定 */
    {0x513   , &pndef_sorev2                        },/* 0x513: 出力信号選択4 */
    {0x514   , &pndef_outsel4                       },/* 0x514: 出力信号選択5 */
    {0x515   , &pndef_insel5                        },/* 0x515: 入力信号選択6 */
    {0x516   , &pndef_insel6                        },/* 0x516: 入力信号選択7 */
    {0x517   , &pndef_outsel6                       },/* 0x517: 出力信号選択6 */
    {0x518   , &pndef_outsel7                       },/* 0x518: 出力信号選択7 */
    {0x519   , &pndef_insel7                        },/* 0x519: 入力信号選択8 */
    {0x51B   , &pndef_fpgoverlv2l                   },/* 0x51B: モータ-負荷位置間偏差過大検出レベル */
    {0x51E   , &pndef_overwrnlv                     },/* 0x51E: 位置偏差過大ワーニングレベル */
    {0x520   , &pndef_overlv2l                      },/* 0x520: 位置偏差過大アラームレベル */
    {0x522   , &pndef_coinlv2l                      },/* 0x522: 位置決め完了幅 */
    {0x524   , &pndef_nearlv2l                      },/* 0x524: ＮＥＡＲ信号幅 */
    {0x526   , &pndef_svonoverlv2l                  },/* 0x526: サーボオン時位置偏差過大アラームレベル */
    {0x528   , &pndef_svonoverwrnlv                 },/* 0x528: サーボオン時位置偏差過大ワーニングレベル */
    {0x529   , &pndef_svonspdlmtlv                  },/* 0x529: サーボオン時速度制限レベル */
    {0x52A   , &pndef_fpgcutrate                    },/* 0x52A: フルクローズ1回転当たりの乗算値 */
    {0x52B   , &pndef_olwarlvl                      },/* 0x52B: 過負荷ワーニングレベル */
    {0x52C   , &pndef_mtbastrq_drt                  },/* 0x52C: モータ過負荷検出ベース電流ディレーティング */
    {0x52D   , &pndef_svbastrq_drt                  },/* 0x52D: 単相電源入力選択時サーボパック過負荷検出ベース電流ディレーティング */
    {0x52F   , &pndef_powon_mon                     },/* 0x52F: 電源投入時のモニタ表示 */
    {0x530   , &pndef_pjogsw                        },/* 0x530: プログラムＪＯＧ運転関係スイッチ */
    {0x531   , &pndef_pjogdistl                     },/* 0x531: プログラムＪＯＧ移動距離 */
    {0x533   , &pndef_pjogrotspd                    },/* 0x533: プログラムＪＯＧ移動速度 */
    {0x534   , &pndef_pjogacctm                     },/* 0x534: プログラムＪＯＧ加減速時間 */
    {0x535   , &pndef_pjogwaittm                    },/* 0x535: プログラムＪＯＧ待ち時間 */
    {0x536   , &pndef_pjognum                       },/* 0x536: プログラムＪＯＧ移動回数 */
    {0x53A   , &pndef_outalm_no1                    },/* 0x53A: 信号出力アラーム・ワーニング番号1 */
    {0x53B   , &pndef_outalm_no2                    },/* 0x53B: 信号出力アラーム・ワーニング番号2 */
    {0x540   , &pndef_adatgmax                      },/* 0x540: ゲインリミット */
    {0x548   , &pndef_trcalm_no                     },/* 0x548: トレース指定アラーム番号 */
    {0x550   , &pndef_monoffset1                    },/* 0x550: アナログモニタ１オフセット電圧 */
    {0x551   , &pndef_monoffset2                    },/* 0x551: アナログモニタ２オフセット電圧 */
    {0x552   , &pndef_mongain1                      },/* 0x552: アナログモニタ1倍率 */
    {0x553   , &pndef_mongain2                      },/* 0x553: アナログモニタ2倍率 */
    {0x55A   , &pndef_powmon_unitime                },/* 0x55A: 消費電力モニタ単位時間 */
    {0x560   , &pndef_remdetw                       },/* 0x560: 残留振動検出幅 */
    {0x561   , &pndef_ovserrdetlvl                  },/* 0x561: オーバーシュート検出レベル */
    {0x580   , &pndef_clamp_l                       },/* 0x580: ゼロクランプレベル */
    {0x581   , &pndef_tgonlv_l                      },/* 0x581: 回転検出レベル（リニア） */
    {0x582   , &pndef_vcmplv_l                      },/* 0x582: 速度一致信号出力幅（リニア） */
    {0x583   , &pndef_brkspd_l                      },/* 0x583: ブレーキ指令出力速度レベル */
    {0x584   , &pndef_svonspdlmtlv_l                },/* 0x584: サーボオン時速度制限レベル */
    {0x585   , &pndef_pjoglinspd                    },/* 0x585: プログラムＪＯＧ最高速度(リニア) */
    {0x586   , &pndef_msrcra                        },/* 0x586: モータ自走冷却率 */
    {0x587   , &pndef_pldetsw                       },/* 0x587: 絶対値スケール用磁極検出実行選択 */
    {0x590   , &pndef_pot_selsw                     },/* 0x590: POT入力信号設定 */
    {0x591   , &pndef_not_selsw                     },/* 0x591: NOT入力信号設定 */
    {0x592   , &pndef_dec_selsw                     },/* 0x592: DEC入力信号設定 */
    {0x593   , &pndef_ext1_selsw                    },/* 0x593: EXT1入力信号設定 */
    {0x594   , &pndef_ext2_selsw                    },/* 0x594: EXT2入力信号設定 */
    {0x595   , &pndef_ext3_selsw                    },/* 0x595: EXT3入力信号設定 */
    {0x597   , &pndef_fstp_selsw                    },/* 0x597: FSTP入力信号設定 */
    {0x598   , &pndef_pcl_selsw                     },/* 0x598: P-CL入力信号設定 */
    {0x599   , &pndef_ncl_selsw                     },/* 0x599: N-CL入力信号設定 */
    {0x59A   , &pndef_clinkans_selsw                },/* 0x59A: CLINKANS入力信号設定 */
    {0x59B   , &pndef_cmcans_selsw                  },/* 0x59B: CMCANS入力信号設定 */
    {0x59C   , &pndef_convready_selsw               },/* 0x59C: CONV_READY入力信号設定 */
    {0x59D   , &pndef_convmcon_selsw                },/* 0x59D: CONV_MCON入力信号設定 */
    {0x5B0   , &pndef_coin_selsw                    },/* 0x5B0: COIN出力信号設定 */
    {0x5B1   , &pndef_vcmp_selsw                    },/* 0x5B1: V-CMP出力信号設定 */
    {0x5B2   , &pndef_tgon_selsw                    },/* 0x5B2: TGON出力信号設定 */
    {0x5B3   , &pndef_srdy_selsw                    },/* 0x5B3: S-RDY出力信号設定 */
    {0x5B4   , &pndef_clt_selsw                     },/* 0x5B4: CLT出力信号設定 */
    {0x5B5   , &pndef_vlt_selsw                     },/* 0x5B5: VLT出力信号設定 */
    {0x5B6   , &pndef_bk_selsw                      },/* 0x5B6: BK出力信号設定 */
    {0x5B7   , &pndef_warn_selsw                    },/* 0x5B7: WARN出力信号設定 */
    {0x5B8   , &pndef_near_selsw                    },/* 0x5B8: NEAR出力信号設定 */
    {0x5B9   , &pndef_cpulse_selsw                  },/* 0x5B9: C-PULSE出力信号設定 */
    {0x5BA   , &pndef_clink_selsw                   },/* 0x5BA: CLINK出力信号設定 */
    {0x5BB   , &pndef_cmcon_selsw                   },/* 0x5BB: CMCON出力信号設定 */
    {0x5BC   , &pndef_pm_selsw                      },/* 0x5BC: PM出力信号設定 */
    {0x5BD   , &pndef_dao1_selsw                    },/* 0x5BD: DAO1出力信号設定 */
    {0x5BE   , &pndef_dao2_selsw                    },/* 0x5BE: DAO2出力信号設定 */
    {0x600   , &pndef_regres                        },/* 0x600: 回生抵抗容量 */
    {0x601   , &pndef_dbres                         },/* 0x601: ＤＢ抵抗容量 */
    {0x603   , &pndef_exreger                       },/* 0x603: 回生抵抗値 */
    {0x604   , &pndef_exdbr                         },/* 0x604: DB抵抗値 */
    {0x620   , &pndef_bbondiff0                     },/* 0x620: HWBB信号タイミング時間 */
    {0x621   , &pndef_saftyfuncSw                   },/* 0x621: 安全機能応用選択スイッチ */
    {0x622   , &pndef_safeindecA                    },/* 0x622: アクティブモード内部減速定数A */
    {0x623   , &pndef_safeindecB                    },/* 0x623: アクティブモード内部減速定数B */
    {0x624   , &pndef_safeindecVel                  },/* 0x624: アクティブモード内部減速停止速度 */
    {0x625   , &pndef_safeStopTime                  },/* 0x625: アクティブモード停止継続時間 */
    {0x626   , &pndef_safePerrlvl                   },/* 0x626: アクティブモード解除位置偏差レベル */
    {0x628   , &pndef_safeVreflvl                   },/* 0x628: アクティブモード解除速度指令レベル */
    {0xE05   , &pndef_almmsk1                       },/* 0xE05: アラームマスク１ */
    {0xE06   , &pndef_almmsk2                       },/* 0xE06: アラームマスク２ */
    {0xE09   , &pndef_opdetsw                       },/* 0xE09: オプションカード／セーフティ機能対応制限スイッチ */
    {0xE0B   , &pndef_syssw1                        },/* 0xE0B: システムスイッチ１ */
    {0xE0C   , &pndef_syssw2                        },/* 0xE0C: システムスイッチ２ */
    {0xE0D   , &pndef_syssw3                        },/* 0xE0D: システムスイッチ３ */
    {0xE0E   , &pndef_comfil                        },/* 0xE0E: 指令入力フィルタ選択時の指令フィルタ設定 */
    {0xE0F   , &pndef_InstEnvTmp_sw1                },/* 0xE0F: 下位:設置環境モニタ基準温度, 上位:機能制限スイッチ１ */
    {0xE10   , &pndef_paswrd                        },/* 0xE10: パラメータパスワード */
    {0xE11   , &pndef_styp                          },/* 0xE11: 下位:サーボパック形式コード, 上位:入力電圧コード */
    {0xE12   , &pndef_ampw                          },/* 0xE12: サーボパック容量 */
    {0xE13   , &pndef_yopt                          },/* 0xE13: Ｙ仕様番号 */
    {0xE14   , &pndef_irats                         },/* 0xE14: サーボパック定格電流 */
    {0xE15   , &pndef_imaxs                         },/* 0xE15: サーボパック最大電流 */
    {0xE17   , &pndef_ibs_md                        },/* 0xE17: 下位:過負荷検出ベース電流, 上位:過負荷検出中間電流 */
    {0xE18   , &pndef_imdt_mxt                      },/* 0xE18: 下位:過負荷検出中間電流時間, 上位:過負荷検出最大電流時間 */
    {0xE19   , &pndef_olwrat                        },/* 0xE19: ＯＬ警告比 */
    {0xE1A   , &pndef_reger                         },/* 0xE1A: 回生抵抗値 */
    {0xE1B   , &pndef_dbr                           },/* 0xE1B: ＤＢ抵抗値 */
    {0xE1C   , &pndef_wre                           },/* 0xE1C: 回生抵抗容量(デフォルト) */
    {0xE1D   , &pndef_wdb                           },/* 0xE1D: ＤＢ抵抗容量(デフォルト) */
    {0xE1E   , &pndef_spgfail                       },/* 0xE1E: 下位:S-PG受信失敗許容回数, 上位:FS-PG受信失敗許容回数 */
    {0xE1F   , &pndef_PnE1F                         },/* 0xE1F: 下位:突入抵抗過負荷用電源投入許容回数, 上位:(予備) */
    {0xE20   , &pndef_ignd                          },/* 0xE20: 電流ループゲイン(D) */
    {0xE21   , &pndef_ignq                          },/* 0xE21: 電流ループゲイン(Q) */
    {0xE22   , &pndef_kid                           },/* 0xE22: 電流ループ積分時定数(D) */
    {0xE23   , &pndef_kiq                           },/* 0xE23: 電流ループ積分時定数(Q) */
    {0xE24   , &pndef_limintd                       },/* 0xE24: 電流ループ積分制限値(D) */
    {0xE25   , &pndef_limintq                       },/* 0xE25: 電流ループ積分制限値(Q) */
    {0xE26   , &pndef_ifil                          },/* 0xE26: 電流ループフィルタ時定数 */
    {0xE28   , &pndef_shant                         },/* 0xE28: 電流検出ゲイン */
    {0xE2A   , &pndef_vrevgn                        },/* 0xE2A: 電圧補正ゲイン */
    {0xE2B   , &pndef_lmdata                        },/* 0xE2B: 電圧制限値 */
    {0xE2C   , &pndef_pwmf                          },/* 0xE2C: ＰＷＭ周波数 */
    {0xE2D   , &pndef_odt_c                         },/* 0xE2D: 下位:オンディレイ時間, 上位:オンディレイ補償定数 */
    {0xE2E   , &pndef_odlv_slp                      },/* 0xE2E: 下位:オンディレイ補償変更レベル, 上位:補償傾き */
    {0xE2F   , &pndef_dq_sw                         },/* 0xE2F: ＤＱ用スイッチ */
    {0xE34   , &pndef_phscmp                        },/* 0xE34: 位相補償 */
    {0xE35   , &pndef_kdd                           },/* 0xE35: 電流ループ微分時間(D) */
    {0xE36   , &pndef_kdq                           },/* 0xE36: 電流ループ微分時間(Q) */
    {0xE37   , &pndef_dfild                         },/* 0xE37: 電流ループ微分フィルタ時定数(D) */
    {0xE38   , &pndef_dfilq                         },/* 0xE38: 電流ループ微分フィルタ時定数(Q) */
    {0xE39   , &pndef_dq_sw2                        },/* 0xE39: 電流制御スイッチ２ */
    {0xE3B   , &pndef_motresq                       },/* 0xE3B: モータ共振抑制用ノッチフィルタQ値 */
    {0xE3C   , &pndef_PnE3C                         },/* 0xE3C: N側スイッチON最小時間 */
    {0xE3D   , &pndef_PnE3D                         },/* 0xE3D: サンプル＆ホールド時間 */
    {0xE3E   , &pndef_kvvfil                        },/* 0xE3E: 主回路電圧補正用ローパスフィルタ時定数 */
    {0xE3F   , &pndef_fltrange                      },/* 0xE3F: モータ過電流検出フィルタ回数 */
    {0xE40   , &pndef_tplsfil                       },/* 0xE40: DRV内部信号異常検出フィルタ回数 */
    {0xE42   , &pndef_mnrng0                        },/* 0xE42: モニタ１ゲイン */
    {0xE43   , &pndef_mnrng1                        },/* 0xE43: モニタ２ゲイン */
    {0xE44   , &pndef_ipg_rwt                       },/* 0xE44: 下位:i-P制御割合, 上位:暴走検出トルク */
    {0xE45   , &pndef_PnE45                         },/* 0xE45: 低速ＯＬ：ベース電流[%]，ＯＬ特性変曲点[電気角10r/min] */
    {0xE46   , &pndef_PnE46                         },/* 0xE46: 低速ＯＬ：中間電流[10%]，最大電流[10%] */
    {0xE47   , &pndef_PnE47                         },/* 0xE47: 低速ＯＬ：中間電流時間[0.1s]，最大電流時間[0.1s] */
    {0xE48   , &pndef_mnptr0l                       },/* 0xE48: モニタ１アドレス */
    {0xE4A   , &pndef_mnptr1l                       },/* 0xE4A: モニタ２アドレス */
    {0xE50   , &pndef_zadjiu                        },/* 0xE50: 電流検出ゼロ調(U相) */
    {0xE51   , &pndef_zadjiv                        },/* 0xE51: 電流検出ゼロ調(V相) */
    {0xE52   , &pndef_gadjiu_v                      },/* 0xE52: 電流検出ゲイン調(U,V相) */
    {0xE53   , &pndef_zadjvl                        },/* 0xE53: 速度指令ゼロ調 */
    {0xE55   , &pndef_gadjv                         },/* 0xE55: 速度指令ゲイン調 */
    {0xE56   , &pndef_zadjtp_n                      },/* 0xE56: トルク指令ゼロ調(正入力/負入力) */
    {0xE57   , &pndef_gadjtp_n                      },/* 0xE57: トルク指令ゲイン調(正入力/負入力) */
    {0xE58   , &pndef_mnaz0_1                       },/* 0xE58: モニタ１,２ゼロ調 */
    {0xE59   , &pndef_mnag0_1                       },/* 0xE59: モニタ１,２ゲイン調 */
    {0xE5A   , &pndef_pgadj                         },/* 0xE5A: 下位:PG電源電圧調, 上位:空き */
    {0xE5B   , &pndef_pnadj                         },/* 0xE5B: 下位:P-N電圧検出レベル微調, 上位:P-N電圧検出レベルゼロ調 */
    {0xE60   , &pndef_ratbt_mt                      },/* 0xE60: 下位:過負荷検出ベース電流, 上位:過負荷検出中間電流 */
    {0xE61   , &pndef_ratmtt_2                      },/* 0xE61: 下位:過負荷検出中間時間, 上位:過負荷検出中間電流２ */
    {0xE62   , &pndef_ratmt2_os                     },/* 0xE62: 下位:過負荷検出中間時間２, 上位:ＯＳ検出レベル */
    {0xE6F   , &pndef_NonRegOvlvl                   },/* 0xE6F: 回生抵抗非内蔵機種で回生不使用時のOV検出レベル */
    {0xE70   , &pndef_pnvoltgn                      },/* 0xE70: Ｐ−Ｎ電圧検出レベル */
    {0xE71   , &pndef_ovlvl                         },/* 0xE71: ＯＶ検出レベル */
    {0xE72   , &pndef_regonlvl                      },/* 0xE72: 回生オンレベル */
    {0xE73   , &pndef_uvlvl                         },/* 0xE73: ＵＶ検出レベル */
    {0xE74   , &pndef_uvfil                         },/* 0xE74: ＵＶ検出フィルタ時定数 */
    {0xE78   , &pndef_regoffhys                     },/* 0xE78: 回生オフヒステリシス幅 */
    {0xE79   , &pndef_vdetadfil                     },/* 0xE79: 主回路検出ＡＤ用ローパスフィルタ時定数 */
    {0xE7A   , &pndef_vdefilsw                      },/* 0xE7A: 主回路検出フィルタ選択スイッチ */
    {0xE7B   , &pndef_UvWrnLvl                      },/* 0xE7B: ＵＶワーニング検出レベル */
    {0xE7C   , &pndef_VdetADMax                     },/* 0xE7C: PnE70[V]入力時のA/D値 */
    {0xE7F   , &pndef_ConvW                         },/* 0xE7F: コンバータ容量 */
    {0xE80   , &pndef_FanTemperatureOffset          },/* 0xE80: 内蔵FAN寿命寿命計算用周囲温度オフセット */
    {0xE81   , &pndef_FanTimeOffset                 },/* 0xE81: FAN交換オフセット時間 */
    {0xE82   , &pndef_RushLifeTime                  },/* 0xE82: 突防回路寿命 */
    {0xE83   , &pndef_DBLifeTime                    },/* 0xE83: DB回路寿命 */
    {0xE84   , &pndef_RushSwichCount                },/* 0xE84: 現在の突防リレー動作回数 */
    {0xE86   , &pndef_DBSwichCount                  },/* 0xE86: 現在のDB動作回数 */
    {0xE88   , &pndef_ChgWaitTime                   },/* 0xE88: ＤＣ充電完了待ち時間 */
    {0xE89   , &pndef_PrdyWaitTime                  },/* 0xE89: パワーレディ待ち時間 */
    {0xE8A   , &pndef_DischCompVlt                  },/* 0xE8A: 急速放電継続閾値電圧 */
    {0xE8C   , &pndef_scnvorgoffset                 },/* 0xE8C: シリアル変換ユニット原点オフセット */
    {0xE8D   , &pndef_phsofst                       },/* 0xE8D: 電流零位相位置 */
    {0xE90   , &pndef_olspdlvl                      },/* 0xE90: 低速過負荷検出速度レベル */
    {0xE91   , &pndef_ollspdprm                     },/* 0xE91: 低速過負荷検出パラメータ */
    {0xE93   , &pndef_dbol_warn                     },/* 0xE93: ＤＢ過負荷検出電力１ */
    {0xE94   , &pndef_dbol_alm                      },/* 0xE94: ＤＢ過負荷検出電力２ */
    {0xE9A   , &pndef_regsynonlvl                   },/* 0xE9A: 回生同期時の回生ONレベル調整値 */
    {0xE9B   , &pndef_regsynontime                  },/* 0xE9B: 回生同期時の回生ON待ち時間 */
    {0xE9C   , &pndef_ctrlboardalmtmpr              },/* 0xE9C: 制御基板異常温度/警告温度 */
    {0xE9D   , &pndef_pwrboardalmtmpr               },/* 0xE9D: パワー基板異常温度/警告温度 */
    {0xE9E   , &pndef_ctrlboardalmclrtmpr           },/* 0xE9E: 制御基板異常クリア可能温度/異常温度検出時間 */
    {0xE9F   , &pndef_pwrboardalmclrtmpr            },/* 0xE9F: パワー基板異常クリア可能温度/異常温度検出時間 */
    {0xEA4   , &pndef_ifil2                         },/* 0xEA4: 静音モード電流ループフィルタ時定数 */
    {0xEA5   , &pndef_pwmf2                         },/* 0xEA5: 静音モードPWM周波数 */
    {0xEA6   , &pndef_ilimlvl                       },/* 0xEA6: キャリア周波数切り替え電流閾値 */
    {0xEA7   , &pndef_pwmf3                         },/* 0xEA7: 第2PWM周波数 */
    {0xEA8   , &pndef_fbfil2                        },/* 0xEA8: 第２速度Ｆ/Ｂフィルタ時定数 */
    {0xEB0   , &pndef_kv                            },/* 0xEB0: 電圧フィードバックループ比例ゲイン */
    {0xEB1   , &pndef_tv                            },/* 0xEB1: 電圧フィードバック積分時定数 */
    {0xEB2   , &pndef_idlim                         },/* 0xEB2: 電圧フィードバック積分リミット */
    {0xEB3   , &pndef_v1max                         },/* 0xEB3: 電圧指令制限レベル */
    {0xEB4   , &pndef_vmaxid                        },/* 0xEB4: d軸電流指令リミット用電圧制限レベル */
    {0xEB5   , &pndef_vpnavg                        },/* 0xEB5: 主回路電圧平均時間 */
    {0xED0   , &pndef_PnED0                         },/* 0xED0: 自動ノッチ完了状態 */
    {0xED1   , &pndef_PnED1                         },/* 0xED1: 自動ノッチ最大振動値 */
    {0xED3   , &pndef_PnED3                         },/* 0xED3: ノッチ周波数１回目補正係数１ */
    {0xED4   , &pndef_PnED4                         },/* 0xED4: ノッチ周波数１回目補正係数２ */
    {0xED5   , &pndef_PnED5                         },/* 0xED5: ノッチ周波数１回目補正範囲閾値 */
    {0xED6   , &pndef_PnED6                         },/* 0xED6: ノッチ周波数２回目補正係数１ */
    {0xED7   , &pndef_PnED7                         },/* 0xED7: ノッチ周波数２回目補正係数２ */
    {0xED8   , &pndef_PnED8                         },/* 0xED8: ノッチ周波数２回目補正範囲閾値 */
    {0xED9   , &pndef_PnED9                         },/* 0xED9: 自動ノッチ振動チェック時の停止判別待ち時間 */
    {0xEDA   , &pndef_TunLessGain                   },/* 0xEDA: 調整ﾚｽ外乱ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ */
    {0xEDB   , &pndef_TunLessStiff                  },/* 0xEDB: 調整ﾚｽｲﾅｰｼｬ補正ｹﾞｲﾝ */
    {0xEDC   , &pndef_TunLessTrqLpf                 },/* 0xEDC: 調整ﾚｽ外乱ﾄﾙｸﾛｰﾊﾟｽﾌｨﾙﾀ時定数 */
    {0xEDD   , &pndef_TunLessVfbLpf                 },/* 0xEDD: 調整ﾚｽ速度FBﾛｰﾊﾟｽﾌｨﾙﾀ時定数 */
    {0xEE0   , &pndef_MLess_vrat_max                },/* 0xEE0: モータレステスト機能仮想モータ速度 */
    {0xEE1   , &pndef_MLess_oslv_pol                },/* 0xEE1: モータレステスト機能仮想モータＯＳ検出レベル, ポール数 */
    {0xEE2   , &pndef_Mless_jmot                    },/* 0xEE2: モータレステスト機能仮想モータロータイナーシャ */
    {0xEE3   , &pndef_Mless_exp_spd_jmot            },/* 0xEE3: モータレステスト機能仮想モータ指数パラメータ */
    {0xEE5   , &pndef_FanLockTime                   },/* 0xEE5: ファン停止アラーム遷移時間 */
    {0xEE6   , &pndef_SvonFanLock_Time              },/* 0xEE6: サーボオン時内蔵ファン停止アラーム検出時間 */
    {0xEE8   , &pndef_dq_sw_L                       },/* 0xEE8: ＤＱ用スイッチ(リニア) */
    {0xEE9   , &pndef_v1max_L                       },/* 0xEE9: 電圧指令制限レベル */
    {0xEEA   , &pndef_TunLessGain_L                 },/* 0xEEA: 調整ﾚｽ外乱ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ(リニア) */
    {0xEEB   , &pndef_TunLessStiff_L                },/* 0xEEB: 調整ﾚｽｲﾅｰｼｬ補正ｹﾞｲﾝ(リニア) */
    {0xEF5   , &pndef_DcBaseLvl                     },/* 0xEF5: 主回路電圧検出補正用基準電圧 */
    {0xF00   , &pndef_typm_e                        },/* 0xF00: モータ形式(8bit), 入力電圧(4bit), エンコーダタイプ(4bit) */
    {0xF01   , &pndef_encver                        },/* 0xF01: エンコーダソフトバージョン */
    {0xF02   , &pndef_motw                          },/* 0xF02: モータ容量 */
    {0xF03   , &pndef_bit_dp                        },/* 0xF03: エンコーダビット数, 位置データ小数点位置 */
    {0xF04   , &pndef_limmlt                        },/* 0xF04: マルチターンリミット */
    {0xF05   , &pndef_vrat_max                      },/* 0xF05: 定格速度, 最大速度 */
    {0xF06   , &pndef_oslv_pol                      },/* 0xF06: ＯＳ検出レベル, ポール数 */
    {0xF07   , &pndef_rattrq                        },/* 0xF07: 定格トルク */
    {0xF08   , &pndef_maxtrq                        },/* 0xF08: 最大トルク */
    {0xF09   , &pndef_irat                          },/* 0xF09: 定格電流 */
    {0xF0A   , &pndef_imax                          },/* 0xF0A: 瞬時最大電流 */
    {0xF0B   , &pndef_emfcmp                        },/* 0xF0B: ＥＭＦ定数 */
    {0xF0C   , &pndef_jmot                          },/* 0xF0C: ロータイナーシャ */
    {0xF0D   , &pndef_motr                          },/* 0xF0D: 電機子巻線抵抗 */
    {0xF0E   , &pndef_motl                          },/* 0xF0E: 電機子インダクタンス */
    {0xF0F   , &pndef_bastrq                        },/* 0xF0F: 過負荷検出ベース電流 */
    {0xF10   , &pndef_midtrq                        },/* 0xF10: 過負荷検出中間電流 */
    {0xF11   , &pndef_midolt                        },/* 0xF11: 過負荷検出中間時間 */
    {0xF12   , &pndef_mtrq2                         },/* 0xF12: 過負荷検出中間電流２ */
    {0xF13   , &pndef_mtolt2                        },/* 0xF13: 過負荷検出中間時間２ */
    {0xF14   , &pndef_phscmpM1                      },/* 0xF14: 位相補償１(補償値,開始速度) */
    {0xF15   , &pndef_ppitch                        },/* 0xF15: ポールピッチ */
    {0xF16   , &pndef_motlq0                        },/* 0xF16: q軸インダクタンスLq0 */
    {0xF17   , &pndef_motlq1                        },/* 0xF17: q軸インダクタンスLq1 */
    {0xF18   , &pndef_exp_trat_jmot                 },/* 0xF18: 下位：定格トルク指数、上位：ロータイナーシャ指数 */
    {0xF19   , &pndef_exp_motw_spd                  },/* 0xF19: 下位：定格出力指数、上位：回転数指数 */
    {0xF1A   , &pndef_BitnoDpoint                   },/* 0xF1A: フルクエンコーダビット数, 位置データ小数点位置 */
    {0xF1B   , &pndef_poleoffset                    },/* 0xF1B: ポールセンサオフセット量 */
    {0xF1C   , &pndef_motresfrq                     },/* 0xF1C: モータ共振抑制用ノッチフィルタ周波数 */
    {0xF1D   , &pndef_phscmpM2                      },/* 0xF1D: 位相角補正値 */
    {0xF1E   , &pndef_flg_wf                        },/* 0xF1E: フラグ */
    {0xF24   , &pndef_motfuncf                      },/* 0xF24: 機能フラグ２ */
    {0xF25   , &pndef_mot_pwmf                      },/* 0xF25: モータPWM周波数 */
    {0xF26   , &pndef_mot_ibs                       },/* 0xF26: モータ低速過負荷検出ベース電流 */
    {0xF27   , &pndef_mot_imd                       },/* 0xF27: モータ低速過負荷検出中間電流 */
    {0xF28   , &pndef_mot_imdt                      },/* 0xF28: モータ低速過負荷検出中間時間 */
    {0xF29   , &pndef_mot_imd2                      },/* 0xF29: モータ低速過負荷検出中間電流２ */
    {0xF2A   , &pndef_mot_imdt2                     },/* 0xF2A: モータ低速過負荷検出中間時間２ */
    {0xF2B   , &pndef_mot_ichgspd                   },/* 0xF2B: モータ低速過負荷検出回転速度閾値 */
    {0xF2C   , &pndef_mot_oc                        },/* 0xF2C: モータOC検出電流 */
    {0xF2D   , &pndef_mot_TunLess                   },/* 0xF2D: 調整レスモード選択 */
    {0xF2E   , &pndef_RefAdjust_ForWF               },/* 0xF2E: 弱め界磁モータ調整値(電圧指令,d軸電流指令) */
    {0xF2F   , &pndef_LCompForWF                    },/* 0xF2F: 弱め界磁L補正係数 */
    {0xF30   , &pndef_RegPhscmp                     },/* 0xF30: 回生時位相進み補償(開始速度,最大回転速度) */
    {0xFA0   , &pndef_RippleCompGainE               },/* 0xFA0: 速度リップル補償ゲイン(電気角) */
    {0xFA1   , &pndef_RippleCompCycle1              },/* 0xFA1: 速度リップル補償第1成分周期(電気角) */
    {0xFA2   , &pndef_RippleCompAmplitude1          },/* 0xFA2: 速度リップル補償第1成分振幅(電気角) */
    {0xFA3   , &pndef_RippleCompPhase1              },/* 0xFA3: 速度リップル補償第1成分位相(電気角) */
    {0xFA4   , &pndef_RippleCompCycle2              },/* 0xFA4: 速度リップル補償第2成分周期(電気角) */
    {0xFA5   , &pndef_RippleCompAmplitude2          },/* 0xFA5: 速度リップル補償第2成分振幅(電気角) */
    {0xFA6   , &pndef_RippleCompPhase2              },/* 0xFA6: 速度リップル補償第2成分位相(電気角) */
    {0xFA7   , &pndef_RippleCompCycle3              },/* 0xFA7: 速度リップル補償第3成分周期(電気角) */
    {0xFA8   , &pndef_RippleCompAmplitude3          },/* 0xFA8: 速度リップル補償第3成分振幅(電気角) */
    {0xFA9   , &pndef_RippleCompPhase3              },/* 0xFA9: 速度リップル補償第3成分位相(電気角) */
    {0xFAA   , &pndef_RippleCompCycle4              },/* 0xFAA: 速度リップル補償第4成分周期(電気角) */
    {0xFAB   , &pndef_RippleCompAmplitude4          },/* 0xFAB: 速度リップル補償第4成分振幅(電気角) */
    {0xFAC   , &pndef_RippleCompPhase4              },/* 0xFAC: 速度リップル補償第4成分位相(電気角) */
    {0xFAD   , &pndef_RippleCompCycle5              },/* 0xFAD: 速度リップル補償第5成分周期(電気角) */
    {0xFAE   , &pndef_RippleCompAmplitude5          },/* 0xFAE: 速度リップル補償第5成分振幅(電気角) */
    {0xFAF   , &pndef_RippleCompPhase5              },/* 0xFAF: 速度リップル補償第5成分位相(電気角) */
    {0xFB0   , &pndef_RippleCompCycle6              },/* 0xFB0: 速度リップル補償第6成分周期(電気角) */
    {0xFB1   , &pndef_RippleCompAmplitude6          },/* 0xFB1: 速度リップル補償第6成分振幅(電気角) */
    {0xFB2   , &pndef_RippleCompPhase6              },/* 0xFB2: 速度リップル補償第6成分位相(電気角) */
    {0xFFFF  , NULL                                 },/* 0xFFFF: End of Table List */
};
const ULONG PnPrmTblEntNum = sizeof(PnPrmTbl) / sizeof(PnPrmTbl[0]);




/******************************* End of File *******************************/

/****************************************************************************
    Lcd Operator Parameter Download List Table
****************************************************************************/
const PDLTBL LcdPdlTbl[] = {
/*                        EepAddr, PnNo, pPrmDef  */
/* 000: Check Sum      */   0xF000,     0, NULL,    
/* 001: Number of Data */   0xF001,   249, NULL,    
/* 002: Header Size    */   0xF002,    32, NULL,    
/* 003:                */   0x0113, 0x006, &pndef_b_prm6,
/* 004:                */   0x0115, 0x007, &pndef_b_prm7,
/* 005:                */   0x0117, 0x00E, &pndef_b_prmE,
/* 006:                */   0x0119, 0x00F, &pndef_b_prmF,
/* 007:                */   0x011B, 0x010, &pndef_AxisAddress0,
/* 008:                */   0x0123, 0x509, &pndef_acoftm,
/* 009:                */   0x0125, 0x550, &pndef_monoffset1,
/* 010:                */   0x0127, 0x551, &pndef_monoffset2,
/* 011:                */   0x0129, 0x552, &pndef_mongain1,
/* 012:                */   0x012B, 0x553, &pndef_mongain2,
/* 013:                */   0x012D, 0x55A, &pndef_powmon_unitime,
/* 014:                */   0x012F, 0x600, &pndef_regres,
/* 015:                */   0x0131, 0x603, &pndef_exreger,
/* 016:                */   0xFFFF, 0xFFFF, NULL,
/* 017:                */   0xFFFF, 0xFFFF, NULL,
/* 018:                */   0xFFFF, 0xFFFF, NULL,
/* 019:                */   0xFFFF, 0xFFFF, NULL,
/* 020:                */   0xFFFF, 0xFFFF, NULL,
/* 021:                */   0xFFFF, 0xFFFF, NULL,
/* 022:                */   0xFFFF, 0xFFFF, NULL,
/* 023:                */   0xFFFF, 0xFFFF, NULL,
/* 024:                */   0xFFFF, 0xFFFF, NULL,
/* 025:                */   0xFFFF, 0xFFFF, NULL,
/* 026:                */   0xFFFF, 0xFFFF, NULL,
/* 027:                */   0xFFFF, 0xFFFF, NULL,
/* 028:                */   0xFFFF, 0xFFFF, NULL,
/* 029:                */   0xFFFF, 0xFFFF, NULL,
/* 030:                */   0xFFFF, 0xFFFF, NULL,
/* 031:                */   0xFFFF, 0xFFFF, NULL,
/* 032:                */   0x01A2, 0x000, &pndef_b_prm0,
/* 033:                */   0x01A4, 0x001, &pndef_b_prm1,
/* 034:                */   0x01A6, 0x002, &pndef_b_prm2,
/* 035:                */   0x01A8, 0x008, &pndef_b_prm8,
/* 036:                */   0x01AA, 0x009, &pndef_b_prm9,
/* 037:                */   0x01AC, 0x00A, &pndef_b_prmA,
/* 038:                */   0x01AE, 0x00B, &pndef_b_prmB,
/* 039:                */   0x01B0, 0x00C, &pndef_b_prmC,
/* 040:                */   0x01B2, 0x00D, &pndef_b_prmD,
/* 041:                */   0x01B4, 0x080, &pndef_b_prm80,
/* 042:                */   0x01B6, 0x081, &pndef_b_prm81,
/* 043:                */   0x01B8, 0x100, &pndef_loophz,
/* 044:                */   0x01BA, 0x101, &pndef_pitime,
/* 045:                */   0x01BC, 0x102, &pndef_posgn,
/* 046:                */   0x01BE, 0x103, &pndef_jrate,
/* 047:                */   0x01C0, 0x104, &pndef_loophz2,
/* 048:                */   0x01C2, 0x105, &pndef_pitime2,
/* 049:                */   0x01C4, 0x106, &pndef_posgn2,
/* 050:                */   0x01C6, 0x109, &pndef_ffgn,
/* 051:                */   0x01C8, 0x10A, &pndef_fffil,
/* 052:                */   0x01CA, 0x10B, &pndef_gnmode,
/* 053:                */   0x01CC, 0x10C, &pndef_mdswlv,
/* 054:                */   0x01CE, 0x10D, &pndef_mswrf,
/* 055:                */   0x01D0, 0x10E, &pndef_mswacc,
/* 056:                */   0x01D2, 0x10F, &pndef_msplvl,
/* 057:                */   0x01D4, 0x11F, &pndef_pitfil,
/* 058:                */   0x01D6, 0x121, &pndef_dobgn,
/* 059:                */   0x01D8, 0x122, &pndef_dobgn2,
/* 060:                */   0x01DA, 0x123, &pndef_dtrqgn,
/* 061:                */   0x01DC, 0x124, &pndef_dlpfil,
/* 062:                */   0x01DE, 0x125, &pndef_dobjgn,
/* 063:                */   0x01E4, 0x131, &pndef_gnswfil1,
/* 064:                */   0x01E6, 0x132, &pndef_gnswfil2,
/* 065:                */   0x01E8, 0x135, &pndef_gnswwait1,
/* 066:                */   0x01EA, 0x136, &pndef_gnswwait2,
/* 067:                */   0x01EC, 0x139, &pndef_gnsw1,
/* 068:                */   0x01F0, 0x13D, &pndef_KcGainRatio,
/* 069:                */   0x01F2, 0x140, &pndef_mdlsw,
/* 070:                */   0x01F4, 0x141, &pndef_mdlgn,
/* 071:                */   0x01F6, 0x142, &pndef_mdlzeta,
/* 072:                */   0x01F8, 0x143, &pndef_mdlptff,
/* 073:                */   0x01FA, 0x144, &pndef_mdlntff,
/* 074:                */   0x01FC, 0x145, &pndef_mdlantfrq,
/* 075:                */   0x01FE, 0x146, &pndef_mdlresfrq,
/* 076:                */   0x0200, 0x147, &pndef_mdlvff,
/* 077:                */   0x0202, 0x148, &pndef_mdlgn2,
/* 078:                */   0x0204, 0x149, &pndef_mdlzeta2,
/* 079:                */   0x0206, 0x14A, &pndef_ff_frq,
/* 080:                */   0x0208, 0x14B, &pndef_ff_fil,
/* 081:                */   0x020A, 0x14F, &pndef_mfctype,
/* 082:                */   0x020C, 0x160, &pndef_avibsw,
/* 083:                */   0x020E, 0x161, &pndef_avibfrq,
/* 084:                */   0x0210, 0x162, &pndef_avibgn,
/* 085:                */   0x0212, 0x163, &pndef_avibdamp,
/* 086:                */   0x0214, 0x164, &pndef_avibfil1,
/* 087:                */   0x0216, 0x165, &pndef_avibfil2,
/* 088:                */   0x0218, 0x166, &pndef_avibdamp2,
/* 089:                */   0x021C, 0x170, &pndef_DatLevel,
/* 090:                */   0x0226, 0x181, &pndef_mswrf_l,
/* 091:                */   0x0228, 0x182, &pndef_mswacc_l,
/* 092:                */   0x0232, 0x200, &pndef_prstsw,
/* 093:                */   0x0234, 0x205, &pndef_mtlmt,
/* 094:                */   0x0236, 0x207, &pndef_cmfilsw,
/* 095:                */   0x023A, 0x20A, &pndef_fcplsml,
/* 096:                */   0x023B, 0x20A, &pndef_fcplsml,
/* 097:                */   0x023E, 0x20E, &pndef_ratb2l,
/* 098:                */   0x023F, 0x20E, &pndef_ratb2l,
/* 099:                */   0x0242, 0x210, &pndef_rata2l,
/* 100:                */   0x0243, 0x210, &pndef_rata2l,
/* 101:                */   0x0246, 0x212, &pndef_pgrat2l,
/* 102:                */   0x0247, 0x212, &pndef_pgrat2l,
/* 103:                */   0x024A, 0x216, &pndef_expfil,
/* 104:                */   0x024C, 0x217, &pndef_mafil,
/* 105:                */   0x024E, 0x218, &pndef_MulFrq,
/* 106:                */   0x0254, 0x22A, &pndef_fcsel,
/* 107:                */   0x025E, 0x240, &pndef_perrclrsigtime,
/* 108:                */   0x0262, 0x281, &pndef_scalepgrat,
/* 109:                */   0x0264, 0x282, &pndef_scalepitch2l,
/* 110:                */   0x0265, 0x282, &pndef_scalepitch2l,
/* 111:                */   0x026C, 0x300, &pndef_vrefgn,
/* 112:                */   0x026E, 0x301, &pndef_speed1,
/* 113:                */   0x0270, 0x302, &pndef_speed2,
/* 114:                */   0x0272, 0x303, &pndef_speed3,
/* 115:                */   0x0274, 0x304, &pndef_jogspd,
/* 116:                */   0x0276, 0x305, &pndef_sfsacc,
/* 117:                */   0x0278, 0x306, &pndef_sfsdec,
/* 118:                */   0x027A, 0x307, &pndef_srfil,
/* 119:                */   0x027C, 0x308, &pndef_fbfil,
/* 120:                */   0x027E, 0x30A, &pndef_svoffdectime,
/* 121:                */   0x0280, 0x30B, &pndef_vrefmafil,
/* 122:                */   0x0282, 0x30C, &pndef_vffmafil,
/* 123:                */   0x0284, 0x310, &pndef_vibsel,
/* 124:                */   0x0286, 0x311, &pndef_vibsens,
/* 125:                */   0x0288, 0x312, &pndef_vibnorm,
/* 126:                */   0x028A, 0x316, &pndef_maxvel,
/* 127:                */   0x028C, 0x324, &pndef_Pn324,
/* 128:                */   0x0290, 0x380, &pndef_speed1_l,
/* 129:                */   0x0292, 0x381, &pndef_speed2_l,
/* 130:                */   0x0294, 0x382, &pndef_speed3_l,
/* 131:                */   0x0296, 0x383, &pndef_jogspd_l,
/* 132:                */   0x0298, 0x384, &pndef_vibnorm_l,
/* 133:                */   0x029A, 0x385, &pndef_maxvel_l,
/* 134:                */   0x029E, 0x400, &pndef_trefgn,
/* 135:                */   0x02A0, 0x401, &pndef_trqfil11,
/* 136:                */   0x02A2, 0x402, &pndef_tlmtf,
/* 137:                */   0x02A4, 0x403, &pndef_tlmtr,
/* 138:                */   0x02A6, 0x404, &pndef_outlmf1,
/* 139:                */   0x02A8, 0x405, &pndef_outlmr1,
/* 140:                */   0x02AA, 0x406, &pndef_emgtrq,
/* 141:                */   0x02AC, 0x407, &pndef_tcrlmt,
/* 142:                */   0x02AE, 0x408, &pndef_tfuncsw,
/* 143:                */   0x02B0, 0x409, &pndef_ntchfil1,
/* 144:                */   0x02B2, 0x40A, &pndef_ntchq1,
/* 145:                */   0x02B4, 0x40B, &pndef_ntchdamp1,
/* 146:                */   0x02B6, 0x40C, &pndef_ntchfil2,
/* 147:                */   0x02B8, 0x40D, &pndef_ntchq2,
/* 148:                */   0x02BA, 0x40E, &pndef_ntchdamp2,
/* 149:                */   0x02BC, 0x40F, &pndef_secfil,
/* 150:                */   0x02BE, 0x410, &pndef_secq,
/* 151:                */   0x02C0, 0x412, &pndef_trqfil12,
/* 152:                */   0x02C2, 0x415, &pndef_treffil,
/* 153:                */   0x02C4, 0x416, &pndef_tfuncsw2,
/* 154:                */   0x02C6, 0x417, &pndef_ntchfil3,
/* 155:                */   0x02C8, 0x418, &pndef_ntchq3,
/* 156:                */   0x02CA, 0x419, &pndef_ntchdamp3,
/* 157:                */   0x02CC, 0x41A, &pndef_ntchfil4,
/* 158:                */   0x02CE, 0x41B, &pndef_ntchq4,
/* 159:                */   0x02D0, 0x41C, &pndef_ntchdamp4,
/* 160:                */   0x02D2, 0x41D, &pndef_ntchfil5,
/* 161:                */   0x02D4, 0x41E, &pndef_ntchq5,
/* 162:                */   0x02D6, 0x41F, &pndef_ntchdamp5,
/* 163:                */   0x02D8, 0x423, &pndef_RippleCompSw,
/* 164:                */   0x02DA, 0x424, &pndef_UvTrqLmt,
/* 165:                */   0x02DC, 0x425, &pndef_UvTrqLmtOffFil,
/* 166:                */   0x02DE, 0x426, &pndef_tffmafil,
/* 167:                */   0x02E0, 0x427, &pndef_VelRippleCompSpd,
/* 168:                */   0x02E6, 0x456, &pndef_ezffttrq,
/* 169:                */   0x02EA, 0x460, &pndef_anotchsw,
/* 170:                */   0x02F8, 0x480, &pndef_tcrlmt_l,
/* 171:                */   0x02FA, 0x481, &pndef_pdetloophz,
/* 172:                */   0x02FC, 0x482, &pndef_pdetpitime,
/* 173:                */   0x02FE, 0x483, &pndef_tlmtf_l,
/* 174:                */   0x0300, 0x484, &pndef_tlmtr_l,
/* 175:                */   0x0302, 0x485, &pndef_pdetmaxspd,
/* 176:                */   0x0304, 0x486, &pndef_pdetmaxt,
/* 177:                */   0x0306, 0x487, &pndef_pdetclmpt,
/* 178:                */   0x0308, 0x488, &pndef_pdetwait,
/* 179:                */   0x030C, 0x48E, &pndef_pdetdistok,
/* 180:                */   0x030E, 0x490, &pndef_pdetjrate,
/* 181:                */   0x0318, 0x495, &pndef_pdetmaxtrq,
/* 182:                */   0x031E, 0x498, &pndef_pdetdegreeok,
/* 183:                */   0x0326, 0x501, &pndef_clamp,
/* 184:                */   0x0328, 0x502, &pndef_tgonlv,
/* 185:                */   0x032A, 0x503, &pndef_vcmplv,
/* 186:                */   0x032C, 0x506, &pndef_brktim,
/* 187:                */   0x032E, 0x507, &pndef_brkspd,
/* 188:                */   0x0330, 0x508, &pndef_brkwai,
/* 189:                */   0x0332, 0x50A, &pndef_insel1,
/* 190:                */   0x0334, 0x50B, &pndef_insel2,
/* 191:                */   0x0336, 0x50C, &pndef_insel3,
/* 192:                */   0x0338, 0x50D, &pndef_insel4,
/* 193:                */   0x033A, 0x50E, &pndef_outsel1,
/* 194:                */   0x033C, 0x50F, &pndef_outsel2,
/* 195:                */   0x033E, 0x510, &pndef_outsel3,
/* 196:                */   0x0340, 0x511, &pndef_inselA,
/* 197:                */   0x0342, 0x512, &pndef_sorev1,
/* 198:                */   0x0344, 0x513, &pndef_sorev2,
/* 199:                */   0x0346, 0x514, &pndef_outsel4,
/* 200:                */   0x0348, 0x515, &pndef_insel5,
/* 201:                */   0x034A, 0x516, &pndef_insel6,
/* 202:                */   0x034C, 0x517, &pndef_outsel6,
/* 203:                */   0x034E, 0x518, &pndef_outsel7,
/* 204:                */   0x0350, 0x519, &pndef_insel7,
/* 205:                */   0x0352, 0x51B, &pndef_fpgoverlv2l,
/* 206:                */   0x0353, 0x51B, &pndef_fpgoverlv2l,
/* 207:                */   0x0356, 0x51E, &pndef_overwrnlv,
/* 208:                */   0x0358, 0x520, &pndef_overlv2l,
/* 209:                */   0x0359, 0x520, &pndef_overlv2l,
/* 210:                */   0x035C, 0x522, &pndef_coinlv2l,
/* 211:                */   0x035D, 0x522, &pndef_coinlv2l,
/* 212:                */   0x0360, 0x524, &pndef_nearlv2l,
/* 213:                */   0x0361, 0x524, &pndef_nearlv2l,
/* 214:                */   0x0364, 0x526, &pndef_svonoverlv2l,
/* 215:                */   0x0365, 0x526, &pndef_svonoverlv2l,
/* 216:                */   0x0368, 0x528, &pndef_svonoverwrnlv,
/* 217:                */   0x036A, 0x529, &pndef_svonspdlmtlv,
/* 218:                */   0x036C, 0x52A, &pndef_fpgcutrate,
/* 219:                */   0x036E, 0x52B, &pndef_olwarlvl,
/* 220:                */   0x0370, 0x52C, &pndef_mtbastrq_drt,
/* 221:                */   0x0374, 0x52F, &pndef_powon_mon,
/* 222:                */   0x0376, 0x530, &pndef_pjogsw,
/* 223:                */   0x0378, 0x531, &pndef_pjogdistl,
/* 224:                */   0x0379, 0x531, &pndef_pjogdistl,
/* 225:                */   0x037C, 0x533, &pndef_pjogrotspd,
/* 226:                */   0x037E, 0x534, &pndef_pjogacctm,
/* 227:                */   0x0380, 0x535, &pndef_pjogwaittm,
/* 228:                */   0x0382, 0x536, &pndef_pjognum,
/* 229:                */   0x038A, 0x548, &pndef_trcalm_no,
/* 230:                */   0x038C, 0x560, &pndef_remdetw,
/* 231:                */   0x038E, 0x561, &pndef_ovserrdetlvl,
/* 232:                */   0x0390, 0x580, &pndef_clamp_l,
/* 233:                */   0x0392, 0x581, &pndef_tgonlv_l,
/* 234:                */   0x0394, 0x582, &pndef_vcmplv_l,
/* 235:                */   0x0396, 0x583, &pndef_brkspd_l,
/* 236:                */   0x0398, 0x584, &pndef_svonspdlmtlv_l,
/* 237:                */   0x039A, 0x585, &pndef_pjoglinspd,
/* 238:                */   0x039C, 0x586, &pndef_msrcra,
/* 239:                */   0x03D6, 0x601, &pndef_dbres,
/* 240:                */   0x03D8, 0x604, &pndef_exdbr,
/* 241:                */   0x03DC, 0x621, &pndef_saftyfuncSw,
/* 242:                */   0x03DE, 0x622, &pndef_safeindecA,
/* 243:                */   0x03E0, 0x623, &pndef_safeindecB,
/* 244:                */   0x03E2, 0x624, &pndef_safeindecVel,
/* 245:                */   0x03E4, 0x625, &pndef_safeStopTime,
/* 246:                */   0x03E6, 0x626, &pndef_safePerrlvl,
/* 247:                */   0x03E7, 0x626, &pndef_safePerrlvl,
/* 248:                */   0x03EA, 0x628, &pndef_safeVreflvl,
};
const ULONG LcdPdlTblEntNum = sizeof(LcdPdlTbl) / sizeof(LcdPdlTbl[0]);





/******************************* End of File *******************************/

