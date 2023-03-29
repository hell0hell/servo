/****************************************************************************
Description	: Register and Parameter Access Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	PrmInitilize()				:Initialization of this module
	PrmLoadEepromValues()		:Load stored parameters in EEPROM
	PrmRecalcAllParameters()	:Calculate all parameters
	PrmRestoreDefaultPrm()		:Restore all parameters with default value
	PrmFindTableIndex()			:Find the table index by register number
	PrmFindNextTableIndex()		:Find the next table index by register number(for Digi-Op)
	PrmWirteAutoClampedValue()	:Write auto-limited register value(for Digi-Op) 
	PrmReadValue()				:Read a parameter value
	PrmWriteValue()				:Write a parameter value
	PrmStoreValueToEeprom()		:Store a parameter to eeprom 
	PrmReLoadValueFromEeprom()	:Reload a Parameter value from Eeprom

	RegMngrReadValues()			:Read a register value
	RegMngrWriteValues()		:Write a register value

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"
#include "MLib.h"
#include "PnPrmListTbl.h"	/* <S0AA> */
/*--------------------------------------------------------------------------------------------------*/
ULONG	PrmBootWarnChkPdef(					/* 電源再投入要パラメータチェック処理					*/
		CPRM_ATTR_T *pPrmDef,				/* パラメータ定義ポインタ								*/
		ULONG	NewData,					/* EEPROM更新値											*/
		ULONG	OldData,					/* EEPROM更新前値										*/
		ALMMAN_HANDLE	*AlmManager );		/* <S0AA> 後で消す 										*/

/****************************************************************************
 private functions 
****************************************************************************/
/* Get a long value from any type */
static LONG prmGetLongValue(void *pSrc, UINT bSize, BOOL sign)
{
	if(sign != FALSE)
	{
		if(bSize == 4){ return *((LONG*)pSrc);}
		if(bSize == 2){ return (LONG)*((SHORT*)pSrc);}
		if(bSize == 1){ return (LONG)*((CHAR*)pSrc);}
	}
	else
	{
		if(bSize == 4){ return *((LONG*)pSrc);}
		if(bSize == 2){ return (LONG)*((USHORT*)pSrc);}
		if(bSize == 1){ return (LONG)*((UCHAR*)pSrc);}
	}
	return 0;
}

/* Set a long value to any type */
static void prmSetLongValue(void *pDst, LONG src, UINT bSize)
{
	if(bSize == 4){ *((LONG*)pDst) = src;}
	else if(bSize == 2){ *((USHORT*)pDst) = (USHORT)src;}
	else if(bSize == 1){ *((UCHAR*)pDst) = (UCHAR)src;}
	else {	;}
}

/* Check the data range */
static PRM_RSLT prmCheckDataLimits(CPPRM_ATTR_T RegAttr, LONG *value)
{
	PRM_RSLT	rslt;
	UINT		i, sz;
	VARIANT_T	min, max, val, cval;
	ULONG		mask;

	rslt = PRM_RSLT_SUCCESS;

	if(PRMDEF_BASENBL == RegAttr->FormatType)
	{
		val.lVal = 0;
		sz = RegAttr->DataLength*8;
		for(i = 0; i < sz; i += 4)
		{
			mask = 0xF << i;
			min.lVal = RegAttr->LowerLimit & mask;
			max.lVal = RegAttr->UpperLimit & mask;
			cval.lVal = *value & mask;
			if(cval.ulVal < min.ulVal)
			{
				cval.ulVal = min.ulVal;
				rslt = PRM_RSLT_LIMIT_ERR;
			}
			else if(cval.ulVal > max.ulVal)
			{
				cval.ulVal = max.ulVal;
				rslt = PRM_RSLT_LIMIT_ERR;
			}
			val.lVal |= cval.lVal;
		}
	}
	else
	{
		val.lVal = *value;
		min.lVal = RegAttr->LowerLimit;
		max.lVal = RegAttr->UpperLimit;
		mask = (4 - RegAttr->DataLength) * 8;

		if(RegAttr->Sign)	/* signed value */
		{
			val.lVal = (val.lVal << mask) >> mask;
			if(val.lVal < min.lVal)
			{
				val.lVal = min.lVal;
				rslt = PRM_RSLT_LIMIT_ERR;
			}
			else if(val.lVal > max.lVal)
			{
				val.lVal = max.lVal;
				rslt = PRM_RSLT_LIMIT_ERR;
			}
		}
		else				/* unsigned value */
		{
			val.ulVal = (val.ulVal << mask) >> mask;
			if(val.ulVal < min.ulVal)
			{
				val.ulVal = min.ulVal;
				rslt = PRM_RSLT_LIMIT_ERR;
			}
			else if(val.ulVal > max.ulVal)
			{
				val.ulVal = max.ulVal;
				rslt = PRM_RSLT_LIMIT_ERR;
			}
		}
	}

	*value = val.lVal;
	return rslt;
}


/****************************************************************************
  Description: Initialization of this module
-----------------------------------------------------------------------------
  Parameters:
	hEepDev;			Pointer to Eeprom device handle
	PrmListTable;		Pointer to the Parameter list table
	PrmCalParam;		Input parameter for the callback function "GetPtrCallback"
	GetRamPtrParam;		Input parameter for the callback function "PrmCalCallback" 
  Return:
		none
****************************************************************************/
void PrmInitialize(REGIF_CONFIG_T *RegCfg, UINT AxisNo, CPRMTBL *PrmListTable,
				   void *PrmCalParam, void *GetRamPtrParam)
{
//	RegCfg->hEepDev = hEepDev;/* <S1D2> */
	RegCfg->PrmListTable = PrmListTable;
	RegCfg->PrmCalParam = PrmCalParam;
	RegCfg->GetRamPtrParam = GetRamPtrParam;
	*RegCfg->SysLevel = 1;
//	RegCfg->EepromBlockAddress = EEP_MAP_BLOCK_LENGTH * AxisNo;   /* <S0E4> */
	RegCfg->EepromBlockAddress = EEP_MAP_AXIS_OFFSET * AxisNo;	  /* <S0E4> */
}

/****************************************************************************
  Description: Load system default value 
-----------------------------------------------------------------------------
  Parameters:
		RegCfg		:[IN]		:Pointer to Register Access Interface Handle
  Return:
		none
****************************************************************************/
void PrmLoadSystemDefaultValues(REGIF_CONFIG_T* RegCfg)
{
	void		*pDstData;
	CPRMTBL		*pTbl;
	UINT		usSize, i, ArraySize;

	for(pTbl = RegCfg->PrmListTable; pTbl->Number != 0xFFFF; pTbl++)
	{
		usSize = pTbl->Attribute->DataLength;
		if( (usSize != 0) && (NULL != pTbl->Attribute->GetRamPtrCallback) )
		{
			ArraySize = (pTbl->Attribute->NumberOfArray != 0) ? 
						pTbl->Attribute->NumberOfArray : 1;
			for(i = 0; i < ArraySize; i++)
			{
				/* Get Parameter ram address */
				pDstData = pTbl->Attribute->GetRamPtrCallback(i, RegCfg->GetRamPtrParam);
				/* Copy to Parameter ram */
				prmSetLongValue(pDstData, pTbl->Attribute->DefaultValue, usSize);
			}
		}
	}
}

/****************************************************************************
  Description: Find a same attribute from the table top to the table end 
****************************************************************************/
static BOOL prmCheckSameAttribute(CPPRMTBL TableTop, CPPRMTBL TableEnd)
{
	CPPRMTBL	pTbl;

	for(pTbl = TableTop; pTbl != TableEnd; pTbl++)
	{
		if(pTbl->Attribute == TableEnd->Attribute)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/****************************************************************************
  Description: Store system default value 
-----------------------------------------------------------------------------
  Parameters:
		RegCfg		:[IN]		:Pointer to Register Access Interface Handle
		bAxCommon	:[IN]		:0=axis parameters, 1= common parameter
		Level		:[IN]		:parameter access level to be restored
  Return:
		none
****************************************************************************/
void PrmStoreSystemDefaultValues(REGIF_CONFIG_T* RegCfg, BOOL bAxCommon, UINT Level)
{
	CPRMTBL		*pTbl;
	void		*pRamV;/* <S0AD> */
	USHORT		EepAddress, offset;
	USHORT		checksum;
	UINT		wdSize, i, ArraySize;
	VARIANT_T	eepVal, rdVal;
	BOOL		skip;
	EEP_HEADER_INFO_T eep_header;

	checksum = 0;
	offset = (bAxCommon != FALSE) ?
			EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;

	for(pTbl = RegCfg->PrmListTable; pTbl->Number != 0xFFFF; pTbl++)
	{
		wdSize = (pTbl->Attribute->DataLength+1)/2;
		skip = prmCheckSameAttribute(RegCfg->PrmListTable, pTbl);
		if( (skip == FALSE)
			&& (bAxCommon == pTbl->Attribute->AxisCommon)
			/*&& (NULL != pTbl->Attribute->GetRamPtrCallback)*/
			&& (pTbl->Attribute->EepromDevice == PRMDEF_EEPROM_IF) )
		{
			EepAddress = offset + pTbl->Attribute->EepromAddress;
			ArraySize = (pTbl->Attribute->NumberOfArray != 0) ?
						pTbl->Attribute->NumberOfArray : 1;
			for(i = 0; i < ArraySize; i++)
			{
				/* Read the current value */
				rdVal.lVal = 0;
				EepdevReadValues(PRMDEF_EEPROM_IF, EepAddress, rdVal.usArray, wdSize);

				if(ACCLVL_SYSTEM > pTbl->Attribute->AccessLevel)
				{
					/* Read the default value */
					eepVal.lVal = 0;
					EepdevReadValues(PRMDEF_EEPROM_IF, EepAddress+(ArraySize*wdSize*2), eepVal.usArray, wdSize);

					/* Write the default value */
					if(rdVal.lVal != eepVal.lVal)
					{
						/* write to EEPROM	*/
						EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, EepAddress, eepVal.usArray, wdSize);
						/* write to RAM	*//* <S0AD> */
						if(NULL != pTbl->Attribute->GetRamPtrCallback)
						{
							pRamV = pTbl->Attribute->GetRamPtrCallback(
												0, RegCfg->GetRamPtrParam);
							prmSetLongValue(pRamV, eepVal.lVal, wdSize*2);
						}
					}
				}
				else
				{	/* Skip a parameter writing. */
					eepVal.lVal = rdVal.lVal;
				}

				checksum = (USHORT)(checksum
						 + eepVal.usArray[0] + eepVal.usArray[1]);
				EepAddress += (USHORT)(wdSize*2);
			}
		}
	}

	/* Write values */
	EepdevReadValues(PRMDEF_EEPROM_IF, (offset + sizeof(eep_header.Checksum)), eepVal.usArray, 1);
	checksum = (USHORT)(-(checksum + eepVal.usArray[0]));
	EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, offset, (USHORT*)&checksum, 1);

	ALMSetServoAlarm( RegCfg->AlmManager, WRN_BOOT );		/* <S0AA> A.941 */
}

/****************************************************************************
  Description: Load stored parameters in EEPROM
-----------------------------------------------------------------------------
  Parameters:
		RegCfg		:[IN]		:Pointer to Register Access Interface Handle
		bAxCommon	:[IN]		:0=axis parameters, 1= common parameter
		devID		:[IN]		:EEPROM device ID
  Return:
		 = 0 : Success
		!= 0 : Error
****************************************************************************/
LONG PrmLoadEepromValues(REGIF_CONFIG_T* RegCfg, UINT devID, BOOL bAxCommon)
{
	void		*pDstData;
	VARIANT_T	eepVal;
	UINT		i, ArraySize;
	CPRMTBL		*pTbl;
	LONG		eRslt;
	USHORT		usEepAdr, usSize;
	USHORT		checksum, sizeB, eep_adr;
	USHORT		offset;
	BOOL		skip;
	EEP_HEADER_INFO_T eep_header;

	offset = (bAxCommon != FALSE) ? 
			EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;

	eep_adr = (devID != PRMDEF_EEPROM_IF) ? offset + EEP_MAP_HEADER_INFO_LENGTH : offset;
	eRslt = EepdevReadValues(devID, eep_adr, &checksum, 1);
	if(0 != eRslt)
	{
		return eRslt;
	}

	eep_adr += sizeof(eep_header.Checksum);
	eRslt = EepdevReadValues(devID, eep_adr, &sizeB, 1);
	if(0 != eRslt)
	{
		return eRslt;
	}
	checksum += sizeB;

//	for(pTbl = RegCfg->PrmListTable;  pTbl->Number != 0xFFFF; pTbl++) /* <S001> */
	pTbl = RegCfg->PrmListTable;
	while(pTbl->Number != 0xFFFF)
	{
		usEepAdr = pTbl->Attribute->EepromAddress + offset;
		usSize = (pTbl->Attribute->DataLength+1)/2;
		skip = prmCheckSameAttribute(RegCfg->PrmListTable, pTbl);

		if( (skip == FALSE)
			&& (bAxCommon == pTbl->Attribute->AxisCommon)
			&& (pTbl->Attribute->EepromDevice == devID) )
			//&& (NULL != pTbl->Attribute->GetRamPtrCallback) )
		{

			ArraySize = (pTbl->Attribute->NumberOfArray != 0) ? 
						pTbl->Attribute->NumberOfArray : 1;
			for(i = 0; i < ArraySize; i++)
			{
				eepVal.lVal = 0;
				/* Read stored value from EEPROM */
				EepdevReadValues(devID,	usEepAdr, eepVal.usArray, usSize);
				
				if(NULL != pTbl->Attribute->GetRamPtrCallback)
				{
					/* Get Parameter ram address */
					pDstData = pTbl->Attribute->GetRamPtrCallback(i, 
															RegCfg->GetRamPtrParam);

					/* EEPROM -> RAM */
					MlibCopyByteMemory(pDstData, &eepVal, pTbl->Attribute->DataLength);
				}

				/* Add check sum value */
				checksum = (USHORT)(checksum + eepVal.usArray[0] + eepVal.usArray[1]);
				
//				usEepAdr += (usSize*2); /* <S001> */
				usEepAdr += (usSize*4);
			}
		}
		pTbl++; /* <S001> */
	}

	return (LONG)checksum;
}

/****************************************************************************
  Description: Calculate all parameters
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		:Pointer to Register Access Interface Handle
	bAxCommon	:[IN]		0=axis parameters, 1= common parameter
  Return:
	none
****************************************************************************/
PRM_RSLT PrmRecalcAllParameters(REGIF_CONFIG_T *RegCfg, BOOL bAxCommon)
{
	UINT		sub, ArraySize;
	CPRMTBL		*pTbl;
	PRM_RSLT	eRslt, err;
	LONG		value;
	void		*pRamV;

	eRslt = PRM_RSLT_SUCCESS;
	for(pTbl = RegCfg->PrmListTable;  pTbl->Number != 0xFFFF; pTbl++)
	{
		if( (pTbl->Attribute->AxisCommon == bAxCommon) 
			&& (pTbl->Attribute->ReadOnly == FALSE) 
			&& (pTbl->Attribute->PrmCalCallback != NULL) )
		{
			ArraySize = (pTbl->Attribute->NumberOfArray != 0) ? 
						pTbl->Attribute->NumberOfArray : 1;
			for(sub = 0;  sub < ArraySize; sub++)
			{
				/* Read the current parameter value */
				value = pTbl->Attribute->DefaultValue;
				if(NULL != pTbl->Attribute->GetRamPtrCallback)
				{
					pRamV = pTbl->Attribute->GetRamPtrCallback(
												sub, RegCfg->GetRamPtrParam);
					value = prmGetLongValue(pRamV, 
											pTbl->Attribute->DataLength,
											pTbl->Attribute->Sign);
				}

				/* Calculate the internal parameters  */
				err = pTbl->Attribute->PrmCalCallback(PRM_ACCCMD_RECALC, 
											sub, RegCfg->PrmCalParam, &value);
				if(PRM_RSLT_SUCCESS != err)
				{
					eRslt = err;
				}
			}
		}
	}

	return eRslt;
}

/****************************************************************************
  Description: Find a table index by register number
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegNo		:[IN]		Parameter Number
	ArrayIdx	:[OUT]		Array Number 
	Length		:[OUT]		Parameter size in bytes
  Return:
	CPPRM_ATTR_T	: Pointer to the attribute
					  If no parameter is found, NULL will return.
****************************************************************************/
CPPRM_ATTR_T PrmFindTableIndex(REGIF_CONFIG_T *RegCfg, UINT RegNo, UINT *ArrayIdx)
{
	CPPRM_ATTR_T	pPrmAttr, FindAttr;
	CPPRMTBL		PrmListTable;
	INT				low, high, mid;
	INT				reg_sz,idx;
	UINT			end_no;

	low = 0;
	high = RegCfg->NumOfTableEntry - 2;
	FindAttr = NULL;

	while(low <= high)
	{
		mid = (INT)((UINT)(low + high) >> 1);
		PrmListTable = &RegCfg->PrmListTable[mid];
		pPrmAttr = PrmListTable->Attribute;

		reg_sz = (pPrmAttr->NumberOfArray != 0) ? (pPrmAttr->NumberOfArray-1) : 0;
		end_no = ((reg_sz*pPrmAttr->DataLength) / 2) + PrmListTable->Number;

		if(RegNo < PrmListTable->Number)
		{
			high = mid - 1;
		}
		else if(RegNo > end_no)
		{
			low = mid + 1;
		}
		else 
		{
			FindAttr = pPrmAttr;
			idx = (RegNo - PrmListTable->Number) * 2;
			*ArrayIdx = idx / pPrmAttr->DataLength;
			if((idx % pPrmAttr->DataLength) != 0)
			{
				FindAttr = NULL;
			}
			break;
		}
	}

	return FindAttr;
}

#if 0
/****************************************************************************
  Description: Find the next table index by register number(for Digi-Op)
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		:Pointer to Register Access Interface Handle
	RegNo		:[IN]		Parameter Number
	ArrayIdx	:[OUT]		Index of Array variables 
	Length		:[OUT]		Parameter size in bytes
  Return:
	CPPRM_ATTR_T	: Pointer to the attribute
					  If no parameter is found, NULL will return.
****************************************************************************/
CPPRM_ATTR_T PrmFindNextTableIndex(REGIF_CONFIG_T *RegCfg, 
							   UINT RegNo, UINT *ArrayIdx, BOOL FwdDir)
{
	CPPRM_ATTR_T	pPrmAttr, FindAttr;
	CPPRMTBL		PrmListTable;
	INT				low, high, mid;
	UINT			end_no, reg_sz;

	low = 0;
	high = RegCfg->NumOfTableEntry - 2;
	FindAttr = NULL;
	*ArrayIdx = 0;

	while(low <= high)
	{
		mid = (low + high) >> 1;
		PrmListTable = &RegCfg->PrmListTable[mid];
		pPrmAttr = PrmListTable->Attribute;

		reg_sz = ((pPrmAttr->DataLength+1) / 2);
		end_no = (pPrmAttr->NumberOfArray != 0) ? (pPrmAttr->NumberOfArray-1) : 0;
		end_no = (reg_sz * end_no) + PrmListTable->Number;

		if(RegNo < PrmListTable->Number)
		{
			high = mid - 1;
			if( FwdDir )
			{	FindAttr = pPrmAttr;}
		}
		else if(RegNo > end_no)
		{
			low = mid + 1;
			if( !FwdDir )
			{	FindAttr = pPrmAttr;}
		}
		else 
		{
			if( FwdDir )
			{	*ArrayIdx = (RegNo - PrmListTable->Number) / reg_sz;}
			else
			{	*ArrayIdx = (RegNo - PrmListTable->Number + (reg_sz>>1)) / reg_sz;}

			FindAttr = pPrmAttr;
			break;
		}
	}

	if(FindAttr == NULL)
	{
		FindAttr = pPrmAttr;
	}

	return FindAttr;
}

/****************************************************************************
  Description: Write auto-limited register value(for Digi-Op)
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegAttr		:[IN]		Attribute of Parameter
	ArrayIdx	:[IN]		Array Number 
	value		:[IN]		new write value 
	bSize		:[IN]		Parameter size in bytes
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
LONG PrmWriteAutoClampedValue(REGIF_CONFIG_T *RegCfg, CPPRM_ATTR_T RegAttr, 
							UINT ArrayIdx, LONG *value, UINT bSize)
{
	LONG		cval;
	LONG		*pRAMValue;

	/* parameter number ? */
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	if( (RegAttr->NumberOfArray != 0) && 
		(ArrayIdx >= RegAttr->NumberOfArray) )
	{	return PRM_RSLT_REGNO_ERR;}

	/* check the read-only flag */
	if(FALSE != RegAttr->ReadOnly)
	{	return PRM_RSLT_RWACC_ERR;}

	/* data size ? */
	if((bSize != RegAttr->DataLength) || (bSize > 4))
	{	return PRM_RSLT_SIZE_ERR;}

	/* check the aceess level */
	if(*RegCfg->SysLevel < RegAttr->AccessLevel)
	{	return PRM_RSLT_RWACC_ERR;}

	/* check prameter range */
	prmCheckDataLimits(RegAttr, value);
	
	/* Write a new value to RAM */
	if(NULL != RegAttr->GetRamPtrCallback)
	{
		pRAMValue = (LONG*)RegAttr->GetRamPtrCallback(ArrayIdx, 
												RegCfg->GetRamPtrParam);
		cval = *pRAMValue;
		*pRAMValue = *value;
	}

	/* Calculate the internal parameters */
	if(NULL != RegAttr->PrmCalCallback)
	{
		if(FALSE == RegAttr->PrmCalCallback(PRM_ACCCMD_WRITE, 
								ArrayIdx, RegCfg->PrmCalParam, value))
		{
			/* if error, restore to previous parameters */
			if(NULL != RegAttr->GetRamPtrCallback)
			{	*pRAMValue = cval;}

			RegAttr->PrmCalCallback(PRM_ACCCMD_WRITE, 
								ArrayIdx, RegCfg->PrmCalParam, &cval);

			return PRM_RSLT_CALC_ERR;
		}
	}

	return PRM_RSLT_SUCCESS;
}
#endif

/****************************************************************************
  Description: Read register values
-----------------------------------------------------------------------------
  Parameters:
	AccessPrm.RegCfg	:[IN]		Pointer to Register Access Interface Handle
	AccessPrm.RegAttr	:[IN]		Attribute of Parameter
	AccessPrm.ArrayIdx	:[IN]		Array Number 
	value				:[OUT]		pointer to read value
	bSize				:[IN]		Parameter size in bytes
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
PRM_RSLT PrmReadValue(REG_ACC_T *AccessPrm, LONG *value, UINT bSize)
{
	void			*pRamV;
	REGIF_CONFIG_T	*RegCfg;
	CPPRM_ATTR_T	RegAttr;
	UINT			data_sz; /* <S001> */

	RegCfg = AccessPrm->hRegCfg; 
	RegAttr = AccessPrm->cPrmDef; 			

	/* parameter number ? */
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	if( (RegAttr->NumberOfArray != 0) && 
		(AccessPrm->ArrayIdx >= RegAttr->NumberOfArray) )
	{	return PRM_RSLT_REGNO_ERR;}

	/* data size ? */
//	if((bSize != RegAttr->DataLength) || (bSize > 4)) /* <S001> */
	data_sz = ((RegAttr->DataLength == 1) && (bSize > 4))
			? RegAttr->NumberOfArray : RegAttr->DataLength;
	if(bSize != data_sz)
	{	return PRM_RSLT_SIZE_ERR;}

	/* aceess level ? */
//	if(*RegCfg->SysLevel < RegAttr->AccessLevel)
//	{	return PRM_RSLT_RWACC_ERR;}
	
	/* Read a Parameter value */
	if(NULL != RegAttr->GetRamPtrCallback)
	{
		pRamV = RegAttr->GetRamPtrCallback(
						AccessPrm->ArrayIdx, RegCfg->GetRamPtrParam);
//		*value = prmGetLongValue(pRamV, RegAttr->DataLength, RegAttr->Sign); /* <S001> */
		if(data_sz <= 4)
		{
			*value = prmGetLongValue(pRamV, RegAttr->DataLength, RegAttr->Sign);
		}
		else
		{
			MlibCopyByteMemory(value, pRamV, data_sz);
		}
	}
	else
	{
		*value = RegAttr->DefaultValue;
		if(NULL != RegAttr->PrmCalCallback)
		{	
			RegAttr->PrmCalCallback(PRM_ACCCMD_READ, 
						AccessPrm->ArrayIdx, RegCfg->PrmCalParam, value);
		}
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
  Description: Write register values
-----------------------------------------------------------------------------
  Parameters:
	AccessPrm.RegCfg	:[IN]		Pointer to Register Access Interface Handle
	AccessPrm.RegAttr	:[IN]		Attribute of Parameter
	AccessPrm.ArrayIdx	:[IN]		Array Number 
	value				:[IN]		new write value 
	bSize				:[IN]		Parameter size in bytes
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
PRM_RSLT PrmWriteValue(REG_ACC_T *AccessPrm, LONG *value, UINT bSize)
{
	PRM_RSLT		rslt;
	LONG			cval, data_sz; /* <S001> */
	void			*pRamV;
	PRM_ACCCMD		PrmCmd;
	REGIF_CONFIG_T	*RegCfg;
	CPPRM_ATTR_T	RegAttr;

	RegCfg = AccessPrm->hRegCfg; 
	RegAttr = AccessPrm->cPrmDef;

	/* parameter number ? */
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	if( (RegAttr->NumberOfArray != 0) && 
		(AccessPrm->ArrayIdx >= RegAttr->NumberOfArray) )
	{	return PRM_RSLT_REGNO_ERR;}

	/* data size ? */
//	if((bSize != RegAttr->DataLength) || (bSize > 4)) /* <S001> */
	data_sz = ((RegAttr->DataLength == 1) && (bSize > 4))
			? RegAttr->NumberOfArray : RegAttr->DataLength;
	if(bSize != data_sz)
	{	return PRM_RSLT_SIZE_ERR;}

	if(NULL != AccessPrm->IfHandle)
	{
		/* check the read-only flag */
		if(FALSE != RegAttr->ReadOnly)
		{	return PRM_RSLT_RWACC_ERR;}

		/* check the access level */
		if(*RegCfg->SysLevel < RegAttr->AccessLevel)
		{	return PRM_RSLT_RWACC_ERR;}

		/* check the access right */
		if(RegCfg->WriteAccessLock != FALSE)
		{
			if(RegCfg->WriteAccessOwner != AccessPrm->IfHandle)
			{	return PRM_RSLT_NOACCRIGHT_ERR;}
		}
		else
		{
			RegCfg->WriteAccessOwner = AccessPrm->IfHandle;
		}
	}

	/* check parameter range */
	cval = *value;
	rslt = prmCheckDataLimits(RegAttr, &cval);
	if(rslt != PRM_RSLT_SUCCESS)
	{	return rslt;}
	
	/* Write a new value to RAM */
	if(NULL != RegAttr->GetRamPtrCallback)
	{
		pRamV = RegAttr->GetRamPtrCallback(
							AccessPrm->ArrayIdx, RegCfg->GetRamPtrParam);
//		cval = prmGetLongValue(pRamV, bSize, RegAttr->Sign); /* <S001> */
//		prmSetLongValue(pRamV, *value, bSize);
		if(bSize > 4)
		{
			MlibCopyByteMemory(pRamV, value, bSize);
		}
		else
		{
			cval = prmGetLongValue(pRamV, bSize, RegAttr->Sign);
			prmSetLongValue(pRamV, *value, bSize);
		}
	}

	/* Calls back to the registered function of parameter calc.*/
	if(NULL != RegAttr->PrmCalCallback)
	{
		PrmCmd = (AccessPrm->EepWrite != FALSE) ? 
					PRM_ACCCMD_EEPWRITE : PRM_ACCCMD_WRITE;

		rslt = RegAttr->PrmCalCallback(PrmCmd, AccessPrm->ArrayIdx, 
											RegCfg->PrmCalParam, value);
//		if(rslt < 0) /* <S001> */
		if((rslt < 0) && (bSize <= 4))
		{
			/* if error, restore to previous parameter */
			if(NULL != RegAttr->GetRamPtrCallback)
			{	prmSetLongValue(pRamV, cval, bSize);}

			RegAttr->PrmCalCallback(PrmCmd, AccessPrm->ArrayIdx, 
									RegCfg->PrmCalParam, &cval);
		}
	}

	/* Store to eeprom. */
	if((rslt >= 0) && (AccessPrm->EepWrite != FALSE))
	{
		PrmStoreValueToEeprom(RegCfg, RegAttr, AccessPrm->ArrayIdx);
	}

	RegCfg->RegWrResult = rslt;
	return rslt;
}

/****************************************************************************
  Description: Write register value to Eeprom
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegAttr		:[IN]		Attribute of Parameter
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
PRM_RSLT PrmWriteValueToEeprom(REGIF_CONFIG_T *RegCfg, 
								CPPRM_ATTR_T RegAttr, UINT ArrayIdx)
{
	PRM_RSLT	rslt;
	USHORT		wrSize;
	void		*pRamV;
	VARIANT_T	eepVal;
	USHORT		offset;
	VARIANT_T	oldVal;		/* <S0AA> */
	
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	rslt = PRM_RSLT_SUCCESS;

#if 0 /* デジオペ/パネオペからのEEPROM正式対応 *//* <S098> */
	if((NULL != RegAttr->GetRamPtrCallback) && 
		(PRMDEF_NO_EEPROM != RegAttr->EepromDevice))
#endif
	if( NULL == RegAttr->GetRamPtrCallback )
	{ /* コールバック関数が未定義の場合 */
		/* ありえないが番号エラーを返す */
		rslt = PRM_RSLT_REGNO_ERR;
	}
	else if( PRMDEF_NO_EEPROM == RegAttr->EepromDevice )
	{ /* RAM Only Parameter */
		rslt = PRM_RSLT_RAMONLY;
	}
	else
	{
		pRamV = RegAttr->GetRamPtrCallback(ArrayIdx, 
											RegCfg->GetRamPtrParam);
		
		wrSize = (USHORT)((RegAttr->DataLength + 1) / 2);
		offset = (RegAttr->AxisCommon != FALSE) ? 
					EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;
		offset += (ArrayIdx * wrSize * 4);
		
		eepVal.lVal = 0;
		MlibCopyByteMemory(&eepVal, pRamV, RegAttr->DataLength);

#if 1 /* <S0AA> */
		/* Read the current value */
		oldVal.lVal = 0;
		if ( 0 == EepdevReadValues( RegAttr->EepromDevice,
									RegAttr->EepromAddress + offset,
									oldVal.usArray,
									wrSize ) )
		{
			/* Check Need Reboot Warning (A.941) */
			PrmBootWarnChkPdef( RegAttr, 
								eepVal.ulVal,
								oldVal.ulVal,
								RegCfg->AlmManager );
		}
#endif

#if 0 /* <S098> */
		if( 0 > EepdevWriteParameters(RegCfg->hEepDev,
									  RegAttr->EepromDevice,
									  RegAttr->EepromAddress + offset,
									  eepVal.usArray, wrSize) )
		{
			rslt = PRM_RSLT_RWACC_ERR;
		}
#endif
		switch ( EepdevWriteParameters( RegAttr->EepromDevice,
										RegAttr->EepromAddress + offset,
										eepVal.usArray,
										wrSize ) )
		{
		case 0: /* Parameter change successful */
			rslt = PRM_RSLT_SUCCESS;
			break;

		case 1: /* Parameter is not different */
			rslt = PRM_RSLT_NO_DIFF;
			break;

		default: /* Parameter change error */
			rslt = PRM_RSLT_RWACC_ERR;
			break;
		}

		/* Calls back to the registered function of parameter calc.*/
		if(NULL != RegAttr->PrmCalCallback)
		{
			eepVal.lVal = prmGetLongValue(pRamV, RegAttr->DataLength, RegAttr->Sign);
			RegAttr->PrmCalCallback(PRM_ACCCMD_EEPWRITE, ArrayIdx, 
											RegCfg->PrmCalParam, &eepVal.lVal);
		}
	}

	return rslt;
}

/****************************************************************************
  Description: Write register value to Eeprom
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegAttr		:[IN]		Attribute of Parameter
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
PRM_RSLT PrmStoreValueToEeprom(REGIF_CONFIG_T *RegCfg, 
								CPPRM_ATTR_T RegAttr, UINT ArrayIdx)
{
	PRM_RSLT	rslt;
	USHORT		wrSize;
	void		*pRamV;
	VARIANT_T	eepVal;
	USHORT		offset;
	VARIANT_T	oldVal;		/* <S0AA> */
	
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	rslt = PRM_RSLT_SUCCESS;

	if((NULL != RegAttr->GetRamPtrCallback) && 
		(PRMDEF_NO_EEPROM != RegAttr->EepromDevice))
	{
		
		pRamV = RegAttr->GetRamPtrCallback(ArrayIdx, 
											RegCfg->GetRamPtrParam);
		
		wrSize = (USHORT)((RegAttr->DataLength + 1) / 2);
		offset = (RegAttr->AxisCommon != FALSE) ? 
					EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;
		offset += (ArrayIdx * wrSize * 4);
		
		eepVal.lVal = 0;
		MlibCopyByteMemory(&eepVal, pRamV, RegAttr->DataLength);

#if 1 /* <S0AA> */
		/* Read the current value */
		oldVal.lVal = 0;
		if ( 0 == EepdevReadValues( RegAttr->EepromDevice,
									RegAttr->EepromAddress + offset,
									oldVal.usArray,
									wrSize ) )
		{
			/* Check Need Reboot Warning (A.941) */
			PrmBootWarnChkPdef( RegAttr, 
								eepVal.ulVal,
								oldVal.ulVal,
								RegCfg->AlmManager );
		}
#endif

//		if(0 != EepdevWriteParameters(RegCfg->hEepDev, 						  /* <S098> */ 
		if( 0 > EepdevWriteParameters( RegAttr->EepromDevice,
									   RegAttr->EepromAddress + offset,
									   eepVal.usArray,
									   wrSize) )
		{
			rslt = PRM_RSLT_RWACC_ERR;
		}
	}

	return rslt;
}

/****************************************************************************
  Description: Write all of registers value to Eeprom
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
PRM_RSLT PrmStoreAllValuesToEeprom(REGIF_CONFIG_T* RegCfg)
{
	PRM_RSLT	rslt;
	CPRMTBL		*pTbl;
	UINT		i, ArraySize;

	rslt = PRM_RSLT_SUCCESS;
	for(pTbl = RegCfg->PrmListTable; pTbl->Number != 0xFFFF; pTbl++)
	{
		ArraySize = (pTbl->Attribute->NumberOfArray != 0) ?
					pTbl->Attribute->NumberOfArray : 1;
		for(i = 0; i < ArraySize; i++)
		{
			rslt |= PrmStoreValueToEeprom(RegCfg, pTbl->Attribute, i);
		}
	}

	return rslt;
}


/****************************************************************************
  Description: Reload a Parameter value from Eeprom
  				To copy to EEPROM the value of RAM
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegAttr		:[IN]		Attribute of Parameter
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
PRM_RSLT PrmReLoadValueFromEeprom(REGIF_CONFIG_T *RegCfg, 
									CPPRM_ATTR_T RegAttr, UINT ArrayIdx)
{
	PRM_RSLT	rslt;
	USHORT		wrSize;
	LONG		cval;
	void		*pRamV;
	VARIANT_T	eepVal;
	USHORT		offset;
	
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	rslt = PRM_RSLT_SUCCESS;
	cval = 0;

	if(PRMDEF_NO_EEPROM != RegAttr->EepromDevice)
	{		
		wrSize = (USHORT)((RegAttr->DataLength + 1) / 2);
		offset = (RegAttr->AxisCommon != FALSE) ? 
					EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;
		offset += (ArrayIdx * wrSize * 4);
		
		eepVal.lVal = 0;

		if(0 != EepdevReadValues( RegAttr->EepromDevice,
								  RegAttr->EepromAddress + offset,
								  eepVal.usArray, wrSize))
		{
			return PRM_RSLT_RWACC_ERR;
		}

		/* Write a new value to RAM */
		if(NULL != RegAttr->GetRamPtrCallback)
		{
			pRamV = RegAttr->GetRamPtrCallback(ArrayIdx, 
										RegCfg->GetRamPtrParam);

			MlibCopyByteMemory(pRamV, &eepVal, RegAttr->DataLength);
			cval = prmGetLongValue(pRamV, wrSize*2, RegAttr->Sign);
		}

		/* Calls back to the registered function of parameter calc.*/
		if(NULL != RegAttr->PrmCalCallback)
		{
			rslt = RegAttr->PrmCalCallback(PRM_ACCCMD_WRITE,
										   ArrayIdx,
										   RegCfg->PrmCalParam,
										   &cval);
		}
	}

	return rslt;
}

/****************************************************************************
  Description: Load a Parameter value from Eeprom (To simple)
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegAttr		:[IN]		Attribute of Parameter
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************//* <S027> */
PRM_RSLT PrmLoadValueFromEeprom(REGIF_CONFIG_T *RegCfg,
									 CPPRM_ATTR_T RegAttr, UINT ArrayIdx, LONG* pVal)
{
	PRM_RSLT	rslt;
	USHORT		wrSize;
	VARIANT_T	eepVal;
	USHORT		offset;
	
	if(NULL == RegAttr)
	{	return PRM_RSLT_REGNO_ERR;}

	rslt = PRM_RSLT_SUCCESS;

	if(PRMDEF_NO_EEPROM != RegAttr->EepromDevice)
	{		
		wrSize = (USHORT)((RegAttr->DataLength + 1) / 2);
		offset = (RegAttr->AxisCommon != FALSE) ? 
					EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;
		offset += (ArrayIdx * wrSize * 4);
		
		eepVal.lVal = 0;

		if(0 != EepdevReadValues( RegAttr->EepromDevice,
								  RegAttr->EepromAddress + offset,
								  eepVal.usArray, wrSize))
		{
			return PRM_RSLT_RWACC_ERR;
		}
		
		MlibCopyByteMemory(pVal, &eepVal, RegAttr->DataLength);
	}

	return rslt;
}

#if 0
/****************************************************************************
  Description: Get a Parameter Attribute
-----------------------------------------------------------------------------
  Parameters:
	RegCfg		:[IN]		Pointer to Register Access Interface Handle
	RegAttr		:[IN]		Attribute of Parameter
	ArrayIdx	:[IN]		Index of Array variable 
	bSize		:[IN]		Parameter size in bytes
  Return:
	 = 0 : Success
	!= 0 : Error
****************************************************************************/
CPPRM_ATTR_T RegMngrGetPrmAttr(REG_MANAGER_HANDLE *hRegMngr, 
					    ULONG RegNumber, UINT bSize)
{
	ULONG			category;
	UINT			regNo;
	UINT			Len, SubIdx;
	CPPRM_ATTR_T	cPrmDef;
	REGIF_CONFIG_T	*hReg;

	category = RegNumber & 0xF000;
	regNo = (UINT)(RegNumber & 0x0FFF);
	cPrmDef = NULL;
	
	switch(category)
	{
	case 0x0000:	// parameter area(ram and eeprom)
		hReg = hRegMngr->hPnReg;
		break;
	case 0x1000:	// parameter ram area
		hReg = hRegMngr->hPnReg;
		break;
	case 0x2000:	// operation area
		hReg = hRegMngr->hFnReg;
		break;
	case 0x3000:	// engineering erea
		hReg = hRegMngr->hEngReg;
		break;
	case 0xC000:	// product information area
		hReg = hRegMngr->hInfReg;
		break;
	case 0xE000:	// monitor area
		hReg = hRegMngr->hUnReg;
		break;
	case 0xF000:	// special area
		hReg = NULL;
		break;
	default:		// trace area
		regNo = (UINT)(RegNumber - 0x4000);
		hReg = hRegMngr->hTrcReg;
		break;
	}
	
	if(NULL != hReg)
	{
		cPrmDef = PrmFindTableIndex(hReg, regNo, &SubIdx, &Len); 
	}

	return cPrmDef;
}
#endif


/****************************************************************************
  Description: Get a register attribute by Virtual Memory Address
-----------------------------------------------------------------------------
  Parameters:
	hRegMngr	:[IN]		Pointer to Register Access Interface Handle
	MemAddr		:[IN]		Virtual Memory Address
	hRegCfg		:[OUT]		Pointer to the specified register configuration
	ArrayIdx	:[OUT]		Index of Array variable
	EepWr		:[OUT]		0->no Eeprom write, 1->Eeprom write
  Return:
	CPPRM_ATTR_T*	: Pointer to the attribute
					  If no parameter is found, NULL will return.
****************************************************************************/
CPPRM_ATTR_T RegMngrGetAttrByMemAddr(REG_MANAGER_HANDLE *hRegMngr, 
									 REG_ACC_T *AccessPrm)
{
	ULONG			category;
	UINT			regNo;

	AccessPrm->hRegCfg = NULL;
	AccessPrm->cPrmDef = NULL;

	category = AccessPrm->MemAddr & 0xFF0FF000;
	regNo = ((AccessPrm->MemAddr&0x1FFF)>>1);

	switch(category)
	{
	case 0x00000000:	// Network ID area
		AccessPrm->hRegCfg = hRegMngr->hNetworkIdReg;
		AccessPrm->EepWrite = FALSE;
		break;
	case 0x00010000:	// M3 Common Parameter area
		regNo = (UINT)(((AccessPrm->MemAddr - 0x00010000)>>1) | 0xA00);
		AccessPrm->hRegCfg = hRegMngr->hPnReg;
//		AccessPrm->EepWrite = TRUE;							/* <S06F>:Dell */
		AccessPrm->EepWrite = FALSE;							/* <S06F>:Add */
		hRegMngr->hPnReg->WriteAccessOwner = NULL;
		break;

	case 0x80000000:	// parameter area(ram and eeprom)
	case 0x80001000:	// parameter area(ram and eeprom)
		AccessPrm->hRegCfg = hRegMngr->hPnReg;
		AccessPrm->EepWrite = TRUE;
		hRegMngr->hPnReg->WriteAccessOwner = NULL;
		break;
	case 0x80002000:	// parameter ram area
	case 0x80003000:	// parameter ram area
		AccessPrm->hRegCfg = hRegMngr->hPnReg;
		AccessPrm->EepWrite = FALSE;
		hRegMngr->hPnReg->WriteAccessOwner = NULL;
		break;

	case 0x80004000:	// operation area
	case 0x80005000:	// operation area
	case 0x80006000:	// engineering erea
		regNo = ((AccessPrm->MemAddr&0x3FFF)>>1);
		AccessPrm->hRegCfg = hRegMngr->hFnReg;
		AccessPrm->EepWrite = FALSE;
		break;

	case 0x80018000:	// product information area
	case 0x80019000:	// product information area
		AccessPrm->hRegCfg = hRegMngr->hInfReg;
		AccessPrm->EepWrite = FALSE;
		break;

	case 0x8001C000:	// monitor area
	case 0x8001D000:	// monitor area
		AccessPrm->hRegCfg = hRegMngr->hUnReg;
		AccessPrm->EepWrite = FALSE;
		break;
	case 0x8001E000:	// special area
	case 0x8001F000:	// special area
		AccessPrm->EepWrite = FALSE;
		break;

	default:
//		if((category >= 0x80020000) && (category <= 0x8009F000))
//		if((category >= 0x80030000) && (category <= 0x800FF000)) /* <S0B7> */
		if((category >= 0x80030000) && (category <= 0x80038000))
		{ /* Trace Area */
//			regNo = (UINT)(AccessPrm->MemAddr - 0x80020000)>>1;
			regNo = (UINT)(AccessPrm->MemAddr - 0x80030000)>>1;
			AccessPrm->hRegCfg = hRegMngr->hTrcReg;
		}
//		else if((category >= 0x800A0000) && (category <= 0x800FF000))
		else if((category >= 0x80020000) && (category <= 0x8002B000))
		{ /* Table Command Area */
			regNo = ((AccessPrm->MemAddr & 0x000FFFFF)>>1);
			AccessPrm->hRegCfg = hRegMngr->hFnReg;
		}
		else
		{
			;
		}
		AccessPrm->EepWrite = FALSE;
		break;
	}

	if(NULL != AccessPrm->hRegCfg)
	{
		AccessPrm->cPrmDef = PrmFindTableIndex(AccessPrm->hRegCfg, 
										regNo, &AccessPrm->ArrayIdx);
		/* Address check */
		if((NULL != AccessPrm->cPrmDef) && (0 != (AccessPrm->MemAddr & 1)))
		{
			if(AccessPrm->cPrmDef->DataLength == 1)
			{/* Adjusts the Array index of the byte parameter */
				AccessPrm->ArrayIdx++;
			}
			else
			{
				return NULL;
			}
		}
	}

	return AccessPrm->cPrmDef;
}


/****************************************************************************
  Description: Lock/Free to Write Access to Operation registers
-----------------------------------------------------------------------------
  Parameters:
	hRegMngr	:[IN]		Pointer to Register Access Interface Handle
  Return: 
	none
****************************************************************************/
void RegMngrLockWriteOpeRegister(REG_MANAGER_HANDLE *hRegMngr)
{
	if(hRegMngr->hFnReg != NULL)
	{
		hRegMngr->hFnReg->WriteAccessLock = TRUE;
	}
}

void RegMngrFreeWriteOpeRegister(REG_MANAGER_HANDLE *hRegMngr)
{
	if(hRegMngr->hFnReg != NULL)
	{
		hRegMngr->hFnReg->WriteAccessLock = FALSE;
		hRegMngr->hFnReg->WriteAccessOwner = NULL;
	}
}

PRM_RSLT RegMngrGetWriteOpeRegStatus(REG_MANAGER_HANDLE *hRegMngr)
{
	if(hRegMngr->hFnReg == NULL)
	{
		return PRM_RSLT_SUCCESS;
	}
	return hRegMngr->hFnReg->RegWrResult;
}

void RegMngrSetWriteOpeRegStatus(REG_MANAGER_HANDLE *hRegMngr, PRM_RSLT sts)
{
	if(hRegMngr->hFnReg != NULL)
	{
		hRegMngr->hFnReg->RegWrResult = sts;
	}
}

/****************************************************************************
  Description: Lock/Free to Write Access to Parameters
-----------------------------------------------------------------------------
  Parameters:
	hRegMngr	:[IN]		Pointer to Register Access Interface Handle
  Return:
	none
****************************************************************************/
void RegMngrLockWriteParameters(REG_MANAGER_HANDLE *hRegMngr)
{
	if(hRegMngr->hPnReg != NULL)
	{
		hRegMngr->hPnReg->WriteAccessLock = TRUE;
	}
}

void RegMngrFreeWriteParameters(REG_MANAGER_HANDLE *hRegMngr)
{
	if(hRegMngr->hPnReg != NULL)
	{
		hRegMngr->hPnReg->WriteAccessLock = FALSE;
		hRegMngr->hPnReg->WriteAccessOwner = NULL;
	}
}

BOOL RegMngrGetWritePrmAccessLock(REG_MANAGER_HANDLE *hRegMngr)
{
	if(hRegMngr->hPnReg != NULL)
	{
		return hRegMngr->hPnReg->WriteAccessLock;
	}
	return FALSE;
}


#if defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
/****************************************************************************
  Description: Load stored parameters in Dummy EEPROM for VC_DEBUG
-----------------------------------------------------------------------------
  Parameters:
		RegCfg		:[IN]		:Pointer to Register Access Interface Handle
		bAxCommon	:[IN]		:0=axis parameters, 1= common parameter
		devID		:[IN]		:EEPROM device ID
  Return:
		 = 0 : Success
		!= 0 : Error
****************************************************************************/
LONG DbgPrmLoadEepValues(REGIF_CONFIG_T* RegCfg, UCHAR *EepBuf, BOOL bAxCommon)
{
	void		*pDstData;
	VARIANT_T	eepVal;
	UINT		i, ArraySize;
	CPRMTBL		*pTbl;
	LONG		eRslt;
	USHORT		usEepAdr, usSize;
	USHORT		checksum, sizeB, eep_adr;
	USHORT		offset;
	BOOL		skip;
	EEP_HEADER_INFO_T eep_header;

	offset = (bAxCommon != FALSE) ? 
			EEP_CMNPRM_ADDRESS : RegCfg->EepromBlockAddress;

	eep_adr = offset;
	eRslt = DbgEepReadValues(EepBuf, eep_adr, (UCHAR*)&checksum, 1);
	if(0 != eRslt)
	{
		return eRslt;
	}

	eep_adr += sizeof(eep_header.Checksum);
	eRslt = DbgEepReadValues(EepBuf, eep_adr, (UCHAR*)&sizeB, 1);
	if(0 != eRslt)
	{
		return eRslt;
	}
	checksum += sizeB;

	for(pTbl = RegCfg->PrmListTable;  pTbl->Number != 0xFFFF; pTbl++)
	{
		usEepAdr = pTbl->Attribute->EepromAddress + offset;
		usSize = (pTbl->Attribute->DataLength+1)/2;

		skip = prmCheckSameAttribute(RegCfg->PrmListTable, pTbl);

		if( (skip == FALSE)
			&& (bAxCommon == pTbl->Attribute->AxisCommon)
			&& (pTbl->Attribute->EepromDevice != PRMDEF_NO_EEPROM))
		{
			ArraySize = (pTbl->Attribute->NumberOfArray != 0) ? 
						pTbl->Attribute->NumberOfArray : 1;
			for(i = 0; i < ArraySize; i++)
			{
				eepVal.lVal = 0;
				/* Read stored value from EEPROM */
				DbgEepReadValues(EepBuf, usEepAdr, (UCHAR*)&eepVal.usArray, usSize);
				
				if(NULL != pTbl->Attribute->GetRamPtrCallback)
				{
					/* Get Parameter ram address */
					pDstData = pTbl->Attribute->GetRamPtrCallback(i, 
															RegCfg->GetRamPtrParam);

					/* EEPROM -> RAM */
					MlibCopyByteMemory(pDstData, &eepVal, pTbl->Attribute->DataLength);
				}

				/* Add check sum value */
				checksum = (USHORT)(checksum + eepVal.usArray[0] + eepVal.usArray[1]);
				
				usEepAdr += (usSize*2);
			}
		}
	}

	return (LONG)checksum;
}
#endif /* #if defined(_VC_DEBUG_) */ /* 2012.08.23 Y.Oka Make Mercury Base Software */

/****************************************************************************************************/
/*																									*/
/*		<S0AA> 電源再投入要パラメータチェック処理													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	電源再投入が必要なパラメータか否かのチェック処理。										*/
/*			電源再投入が必要なパラメータであれば、電源再投入要ワーニング(A.941)を設定する			*/
/*																									*/
/****************************************************************************************************/
ULONG	PrmBootWarnChkPdef(					/* 電源再投入要パラメータチェック処理					*/
		CPRM_ATTR_T *pPrmDef,				/* パラメータ定義ポインタ								*/
		ULONG	NewData,					/* EEPROM更新値											*/
		ULONG	OldData,					/* EEPROM更新前値										*/
		ALMMAN_HANDLE	*AlmManager )		/* <S0AA> 後で消す 										*/
{
ULONG	err = FALSE;
ULONG	ChkReboot = 0xFFFFFFFF;
/*--------------------------------------------------------------------------------------------------*/
/*		電源再投入要チェック																		*/
/*--------------------------------------------------------------------------------------------------*/
		if ( pPrmDef->NeedReboot != TRUE )
		{
			ChkReboot = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set check reboot mask 																		*/
/*--------------------------------------------------------------------------------------------------*/
		else if ( (&pndef_gnmode) == pPrmDef 	/* Pn10B */ )
		{
			ChkReboot = 0xFFF0; 
		}
		else if ( (&pndef_DatLevel) == pPrmDef 	/* Pn170 */ )
		{
			ChkReboot = 0x00FF;
		}
		else if ( (&pndef_tfuncsw) == pPrmDef 	/* Pn408 */ )
		{
			ChkReboot = 0x00F0;
		}
		else if ( (&pndef_b_prmD) == pPrmDef 	/* Pn00D */ )
		{
			ChkReboot = 0x0FFF;
		}
		else if ( (&pndef_syssw3) == pPrmDef 	/* PnE0D */ )
		{
			ChkReboot = ~(0x0022);
		}
/*--------------------------------------------------------------------------------------------------*/
		err = (NewData ^ OldData) & ChkReboot;
/*--------------------------------------------------------------------------------------------------*/
/*		ワーニング A.941 設定																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( err != 0 )
		{
			ALMSetServoAlarm( AlmManager, WRN_BOOT );
		}
/*--------------------------------------------------------------------------------------------------*/
		return (err);
}

