/****************************************************************************
  Description   : Main Initializing Module for MECURY
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2013 All Rights Reserved

  Project       : Mecury

-----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
-----------------------------------------------------------------------------
	H.I.   2013.10.07    created <S04D>

****************************************************************************/
#include	"SvOptCardIf.h"


/*****************************************************************************************************
  Description: Initialize Option Module Variables
------------------------------------------------------------------------------------------------------
  Parameters	:
		OPTCARD_HNDL *OptCardHdl	: Pointer to OPTION CARD Handle
  Return		:
		none
*****************************************************************************************************/
void	OpCardInitOptionVariables( OPTCARD_HNDL *OptCardHdl, PRMDATA *Prm )
{

/*--------------------------------------------------------------------------------------------------*/
/*		Reset Servo-Option I/F Variables															*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_OPT_CARD_CMDOPTION != CSW_CMDOPTION_NO_SUPPORT/* TODO:対応の有無に関わらず本関数は実行すべき？*/
		KpxInitSvCommandOptVar( &OptCardHdl->CmdOpt, Prm );					/* Command Option		*/
#endif/* CSW_CMDOPTION_NO_SUPPORT */

		SftOptInitSvSafetyOptVar( &OptCardHdl->SftOpt );					/* Safety Option <S0C1>	*/

		FbOptInitSvFeedbackOptVar( &OptCardHdl->FbkOpt, Prm );				/* Feedback Option		*/

		return;
}

/*****************************************************************************************************
  Description: Detect Option Module
------------------------------------------------------------------------------------------------------
  Parameters	:
		PRMDATA *Prm				: Pointer to Parameter Data
		OPTCARD_HNDL *OptCardHdl	: Pointer to OPTION CARD Handle
		ALMMAN_HANDLE *AlmManager 	: Pointer to Alarm Handle
  Return		:
		none
*****************************************************************************************************/
void	OpCardDetectOptions( PRMDATA *Prm, OPTCARD_HNDL *OptCardHdl,
												ALMMAN_HANDLE *AlmManager, ASIC_HW_IF_STR *AsicHwReg )
{

/*--------------------------------------------------------------------------------------------------*/
/*		Reset Option Detect Info Register (E51AH)													*/
/*--------------------------------------------------------------------------------------------------*/
		OptCardHdl->OpDetInfo = 0x0000;				/* All Option Cards Not Connected				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Detect Option Cards																			*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_OPT_CARD_CMDOPTION != CSW_CMDOPTION_NO_SUPPORT
		/*------------------------------------------------------------------------------------------*/
		/*		Command Option Card																	*/
		/*------------------------------------------------------------------------------------------*/
		if( ((Prm->opdetsw & 0x0001) != 0) && ((Prm->b_prmD & 0x000F) == 0) )
		{
			KpxDetectCommandOption( &OptCardHdl->OpDetInfo, &OptCardHdl->CmdOpt, AlmManager );
		}
#endif/* CSW_CMDOPTION_NO_SUPPORT */

//#if CSW_OPT_CARD_SAFETY != CSW_SAFETY_NO_SUPPORT	/* コメント：暫定処置 <S0C1>*/
		/*------------------------------------------------------------------------------------------*/
		/*		Safety Option Card																	*/
		/*------------------------------------------------------------------------------------------*/
		if((Prm->opdetsw & 0x0002) != 0)
		{
			SftOptDetectSafetyOption( &OptCardHdl->SftOpt, AlmManager );
		}
//#endif/* CSW_SAFETY_NO_SUPPORT */

#if CSW_OPT_CARD_FBOPTION != CSW_FBOPTION_NO_SUPPORT
		/*------------------------------------------------------------------------------------------*/
		/*		Feedback Option Card																*/
		/*------------------------------------------------------------------------------------------*/
		if((Prm->opdetsw & 0x0004) != 0)
		{
			FbOptDetectFeedbackOption( &OptCardHdl->OpDetInfo, &OptCardHdl->FbkOpt, AlmManager, AsicHwReg );
		}
#endif/* CSW_FBOPTION_SUPPORT */

		return;
}

/*****************************************************************************************************
  Description: Option Card ID writing Process
------------------------------------------------------------------------------------------------------
  Parameters	:
		OPTCARD_HNDL *OptCardHdl	: Pointer to OPTION CARD Handle
  Return		:
		none
*****************************************************************************************************/
void	OpCardWriteEepromOptionId( OPTCARD_HNDL *OptCardHdl, LONG ax_no )
{

	USHORT blockVal[EEP_OPTID_SIZE] = {0};
//	BOOL wrEepReq = FALSE;//<S0C7>

/*--------------------------------------------------------------------------------------------------*/
/*	Read EEPROM Option ID																			*/
/*--------------------------------------------------------------------------------------------------*/
	EepdevReadValues(PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_OPTID_OFS), blockVal, EEP_OPTID_SIZE);
/*--------------------------------------------------------------------------------------------------*/
/*	Create the data to be written																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( OptCardHdl->CmdOpt.conf.OpInfo.f.WrEepReq )			/* Command Option ID					*/
	{
		blockVal[0] = OptCardHdl->CmdOpt.conf.OpInfo.IdType;
	}
	if( OptCardHdl->SftOpt.conf.OpInfo.f.WrEepReq )			/* Safety Option ID						*/
	{
		blockVal[1] = OptCardHdl->SftOpt.conf.OpInfo.IdType;
	}
	if( OptCardHdl->FbkOpt.conf.OpInfo.f.WrEepReq )			/* Feedback Option ID					*/
	{
		blockVal[2] = OptCardHdl->FbkOpt.conf.OpInfo.IdType;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Write EEPROM Option ID																			*/
/*--------------------------------------------------------------------------------------------------*/
	EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_OPTID_OFS), blockVal, EEP_OPTID_SIZE);

	return;
}


/*****************************************************************************************************
  Description: Option Card ID getting Process
------------------------------------------------------------------------------------------------------
  Parameters	:
		OPTCARD_HNDL *OptCardHdl	: Pointer to OPTION CARD Handle
  Return		:
		none
*****************************************************************************************************/
void	OpCardSetLastOptionId( OPTCARD_HNDL *OptCardHdl )
{
	USHORT blockVal[EEP_OPTID_SIZE] = {0};
/*--------------------------------------------------------------------------------------------------*/
/*	Read EEPROM Option ID																			*/
/*--------------------------------------------------------------------------------------------------*/
	EepdevReadValues( PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_OPTID_OFS), blockVal, EEP_OPTID_SIZE);
	
/*--------------------------------------------------------------------------------------------------*/
/*	Set the value obtained																			*/
/*--------------------------------------------------------------------------------------------------*/
	OptCardHdl->CmdOpt.conf.OpInfo.LastIdType = blockVal[0];				/* Command Option ID	*/
	OptCardHdl->SftOpt.conf.OpInfo.LastIdType = blockVal[1];				/* Safety Option ID		*/
	OptCardHdl->FbkOpt.conf.OpInfo.LastIdType = blockVal[2];				/* Feedback Option ID	*/

	return;
}


/*****************************************************************************************************
  Description: Option Card ID writing Process for EEPROM INIT
------------------------------------------------------------------------------------------------------
  Parameters	:
		OPTCARD_HNDL *OptCardHdl	: Pointer to OPTION CARD Handle
  Return		:
		none
*****************************************************************************************************/
void	OpCardEepDeviceInit( void )
{
	USHORT blockVal[EEP_OPTID_SIZE] = {0};

/*--------------------------------------------------------------------------------------------------*/
/*	Set Option ID																					*/
/*--------------------------------------------------------------------------------------------------*/
	blockVal[0] = CMDOP_ID_NONE;
	blockVal[1] = SFTOP_ID_NONE;
	blockVal[2] = FBKOP_ID_NONE;

/*--------------------------------------------------------------------------------------------------*/
/*	Write EEPROM Option ID																			*/
/*--------------------------------------------------------------------------------------------------*/
	EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_OPTID_OFS), blockVal, EEP_OPTID_SIZE);

	return;
}


/**************************** end of file **********************************/
