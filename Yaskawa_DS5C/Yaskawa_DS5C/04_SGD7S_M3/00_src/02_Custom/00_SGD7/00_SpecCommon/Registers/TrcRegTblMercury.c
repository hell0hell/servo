/****************************************************************************
  Description   : Trace Register List Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

    Project       : Mercury

  Create Date   : 2013/12/28

****************************************************************************/


#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"


/****************************************************************************
    Definitions of Register Attribute
****************************************************************************/

/***************************************************************************/
/*  Attribute of 0x00000 : Trace Buffer */
/***************************************************************************/
static void *trcdefGetRamPtr_Trc00000(UINT ArrayIdx, void *AxParam)
{   return ((USHORT*)AxParam+ArrayIdx);}

const PRM_ATTR_T trcdef_Trc00000 = {
    0x0000,                                 /* Register Number */
    ACCLVL_USER1,                           /* Access Level */
    TRUE,                                   /* Ž²‹¤’Ê */
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
    0x8000,                                 /* Number of Array */
    0,                                      /* EEPROM address */
    0,                                      /* Lower Limit */
    0xFFFFFFFF,                             /* Upper Limit */
    0,                                      /* Default value */
    &trcdefGetRamPtr_Trc00000,              /* Parameter varialbles  */
    NULL,                                   /* Parameter Read/Write Callback Function */
};




/****************************************************************************
    Parameter(Register) List Table
****************************************************************************/
const PRMTBL TrcRegTable[] = {
    {0x00000 , &trcdef_Trc00000                },/* 0x00000: Trace Buffer */
    {0xFFFF  , NULL                            },/* 0xFFFF: End of Table List */
};
const ULONG TrcRegTableEntNum = sizeof(TrcRegTable) / sizeof(TrcRegTable[0]);





/******************************* End of File *******************************/

