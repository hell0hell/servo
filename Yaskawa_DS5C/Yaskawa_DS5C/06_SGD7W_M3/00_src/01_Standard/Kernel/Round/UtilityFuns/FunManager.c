/****************************************************************************
Description	: Utility Function Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	FunInitilize()				:Initialization of this module
	FunSetOpeModeRegister()		:Sets Operation mode register
	FunGetOpeModeRegister()		:Gets Operation mode register value
	FunSetTraceModeRegister()	:Sets Trace mode register
	FunSetOpeCmdRegister()		:Sets Operation command register
	FunRunTimeService()			:Execute Utility Function

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.11.12     created

****************************************************************************/
#include "Basedef.h"
#include "FunManager.h"
#include "MLib.h"



/****************************************************************************
  Description: Initialization of this module
-----------------------------------------------------------------------------
  Parameters:
	FunMngr;			Pointer to Function Manager handle
	FunTable;			Pointer to the Function list table
	TableNum;			Number of the Function Table list
	FuncParam;			Pointer to the parameter of Callback function
  Return:
	none
*****************************************************************************/
void FunInitialize(FUN_MANAGER *FunMngr, void *RegMngr,
				   FUN_AXCOMMON *FunAxCommon, CPFUNTBL FunTable, UINT TableNum)
{
//	UINT i;

	MlibResetByteMemory(FunMngr, sizeof(FUN_MANAGER));
	FunMngr->FunTable = FunTable;
	FunMngr->NumOfFunTable = TableNum;
	FunMngr->AxCommon = FunAxCommon;
	FunMngr->RegManager = RegMngr;

	RegMngrFreeWriteOpeRegister(RegMngr);

#if 0
	/* Initialize all Utility functions */
	for(i = 0; i < FunMngr->NumOfFunTable; i++)
	{
		if(NULL != FunTable[i].FunDefAttr->InitFunc)
		{
			FunTable[i].FunDefAttr(AxisParam);
		}
	}
#endif
}

/****************************************************************************
  Description: Set the Operation mode.
-----------------------------------------------------------------------------
  Parameters:
	FunMngr		: Pointer to Function Manager handle
	ch			: FUN_CH1 or FUN_CH2
	mode		: Operation mode
	ClbkParam	: Pointer to the parameter of the Fn Callback functions
  Return:
	enum FUN_ERROR
*****************************************************************************/
PRM_RSLT FunSetOpeModeRegister(FUN_MANAGER *FunMngr, FUN_EXECH ch, ULONG mode, void* ClbkParam)
{
	CPFUNTBL	cFunTable, FindFnTbl;
	PRM_RSLT	err;
	UINT		i;

	/* Check the mode value */
	if(mode == 0)
	{
		cFunTable = FunMngr->cFunMode[ch];
		if( (cFunTable != NULL)
			&& (NULL != cFunTable->FunDefAttr->LeaveFunc) )
		{	/* Clear Opration mode. */
			cFunTable->FunDefAttr->LeaveFunc(&FunMngr->FunExeState[ch], ClbkParam);
		}

		FunMngr->cFunMode[ch] = NULL;
		FunMngr->AxCommon->modeLock = FALSE;

		/* Unlock the register write access */
		if( (FunMngr->AxCommon->TraceMode == 0)
			&& (FunMngr->cFunMode[0] == NULL) && (FunMngr->cFunMode[1] == NULL) )
		{
			RegMngrFreeWriteOpeRegister((REG_MANAGER_HANDLE*)FunMngr->RegManager);
		}
		return PRM_RSLT_SUCCESS;
	}
	else
	{
		if(FunMngr->cFunMode[ch] != NULL)
		{	/* mode setting error */
			err = (FunMngr->cFunMode[ch]->FnMode != (USHORT)mode) ?
					PRM_RSLT_CONDITION_ERR : PRM_RSLT_SUCCESS;
			return err;
		}
	}

	/* Search function mode from the Function list table */
	FindFnTbl = NULL;
	err = PRM_RSLT_LIMIT_ERR;
	for(i = 0; i < FunMngr->NumOfFunTable; i++)
	{
		cFunTable = &FunMngr->FunTable[i];
		if( (cFunTable->FnMode == (USHORT)mode)
			&& (FunMngr->AxCommon->AccessLevel >= cFunTable->FunDefAttr->AccessLevel) )
		{
			FindFnTbl = cFunTable;
			break;
		}
	}

	/* Sets an operation mode */
	if(FindFnTbl != NULL)
	{
		/* 軸依存チェック */
		if(FunMngr->AxCommon->modeLock != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
		/* 操作モード同時実行チェック */
		cFunTable = FunMngr->cFunMode[((INT)ch+1)&1];
		if(cFunTable != NULL)
		{
			if(3 != (cFunTable->FunDefAttr->RunLevel + FindFnTbl->FunDefAttr->RunLevel))
			{	/* level 1と2の組み合わせのみ可*/
				return PRM_RSLT_CONDITION_ERR;
			}
		}
		/* トレース操作モード同時実行チェック */
		if( (FunMngr->AxCommon->TraceMode != 0)
			&& (FALSE == FindFnTbl->FunDefAttr->ParaTrace) )
		{
			return PRM_RSLT_CONDITION_ERR;
		}

		/* パラメータ書き込み禁止状態チェック */
		if((FindFnTbl->FunDefAttr->ExePrmProhibit == FALSE)
			&& (FALSE != RegMngrGetWritePrmAccessLock((REG_MANAGER_HANDLE*)FunMngr->RegManager)) )
		{
			return PRM_RSLT_RWACC_ERR;
		}

		/* 操作レジスタライトアクセスロック */
		RegMngrLockWriteOpeRegister((REG_MANAGER_HANDLE*)FunMngr->RegManager);

		/* Initialize the Executing state */
		MlibResetByteMemory(&FunMngr->FunExeState[ch], sizeof(FUNEXE));
		FunMngr->FunExeState[ch].CmdCode = FCMD_NONE;

		/* Call the registerd callback function */
		err = PRM_RSLT_SUCCESS;
		if(NULL != FindFnTbl->FunDefAttr->EnterFunc)
		{
			err = FindFnTbl->FunDefAttr->EnterFunc(
								&FunMngr->FunExeState[ch], ClbkParam);
		}

		if(err == PRM_RSLT_SUCCESS)
		{	/* Set a executing mode */
			FunMngr->AxCommon->modeLock = FindFnTbl->FunDefAttr->AxCommon;
			FunMngr->CallbackParam = ClbkParam;
			FunMngr->cFunMode[ch] = FindFnTbl;
		}
	}

	return err;
}

/****************************************************************************
  Description: Set the Trace mode
-----------------------------------------------------------------------------
  Parameters:
	FunMngr			: Pointer to Function Manager handle
	mode			: trace mode to be written
  Return:
	enum FUN_ERROR
*****************************************************************************/
PRM_RSLT FunSetTraceModeRegister(FUN_MANAGER *FunMngr, ULONG mode)
{
	CPFUNTBL	cFunTbl1, cFunTbl2;
	PRM_RSLT	err;

	err = PRM_RSLT_SUCCESS;
	if(mode == 0)
	{	/* Clear opration mode. */
		FunMngr->AxCommon->TraceMode = 0;
		/* Unlock the register write access */
		if((FunMngr->cFunMode[0] == NULL) && (FunMngr->cFunMode[1] == NULL))
		{
			RegMngrFreeWriteOpeRegister((REG_MANAGER_HANDLE*)FunMngr->RegManager);
		}
	}
	else
	{
		if(FunMngr->AxCommon->TraceMode != 0)
		{	/* already in executing */
			err = PRM_RSLT_CONDITION_ERR;
		}
		else
		{
			/* 操作モード同時実行チェック */
			cFunTbl1 = FunMngr->cFunMode[0];
			cFunTbl2 = FunMngr->cFunMode[1];
			if( ((cFunTbl1 != NULL) && (cFunTbl1->FunDefAttr->ParaTrace == FALSE))
				|| ((cFunTbl2 != NULL) && (cFunTbl2->FunDefAttr->ParaTrace == FALSE)) )
			{
				err = PRM_RSLT_CONDITION_ERR;
			}
			else
			{
				FunMngr->AxCommon->TraceMode = mode;
				/* 操作レジスタライトアクセスロック */
				RegMngrLockWriteOpeRegister((REG_MANAGER_HANDLE*)FunMngr->RegManager);
			}
		}
	}

	return err;
}


/****************************************************************************
  Description: Set the operation command value
-----------------------------------------------------------------------------
  Parameters:
	FunMngr			: Pointer to Function Manager handle
	ch				: FUN_CH1 or FUN_CH2
	cmd				: Command code
  Return:
	enum FUN_ERROR
*****************************************************************************/
PRM_RSLT FunSetOpeCommandRegister(FUN_MANAGER *FunMngr, FUN_EXECH ch, ULONG cmd)
{
	PRM_RSLT err;

	/* Check that Function mode was already set. */
	if(FunMngr->cFunMode[ch] == NULL)
	{
		err = PRM_RSLT_CONDITION_ERR;
	}
	else
	{
		FunMngr->FunExeState[ch].CmdCode = (FCMD_CODE)cmd;
		FunRunTimeService(FunMngr, ch);
		err = FunMngr->FunExeState[ch].ErrorResult;
	}
	return err;
}


/****************************************************************************
  Description: Execute Utility Function
-----------------------------------------------------------------------------
  Parameters:
	FunMngr			: Pointer to Function Manager handle
	ch				: FUN_CH1 or FUN_CH2
  Return:
	none
*****************************************************************************/
void FunRunTimeService(FUN_MANAGER *FunMngr, FUN_EXECH ch)
{
	CPFUNTBL	cFunTable;
	PRM_RSLT	err;

	cFunTable = FunMngr->cFunMode[ch];
	if(cFunTable != NULL)
	{
		if(NULL != cFunTable->FunDefAttr->ExecuteFunc)
		{
			err = cFunTable->FunDefAttr->ExecuteFunc(&FunMngr->FunExeState[ch],
												 FunMngr->CallbackParam);

			FunMngr->FunExeState[ch].ErrorResult = err;
			FunMngr->FunExeState[ch].CmdCode = FCMD_NONE;
			RegMngrSetWriteOpeRegStatus((REG_MANAGER_HANDLE*)FunMngr->RegManager, err);

			if((err != PRM_RSLT_CONTINUE)
				&& (FunMngr->FunExeState[ch].HoldFnMode == FALSE))
			{
				/* Finish the function */
				FunMngr->cFunMode[ch] = NULL;
				FunMngr->FunExeState[ch].CmdCode = FCMD_NONE;
				FunMngr->FunExeState[ch].State = 0;
				FunMngr->AxCommon->modeLock = FALSE;

				/* Unlock the register write access */
				if( (FunMngr->AxCommon->TraceMode == 0)
					&& (FunMngr->cFunMode[0] == NULL)
					&& (FunMngr->cFunMode[1] == NULL) )
				{
					RegMngrFreeWriteOpeRegister((REG_MANAGER_HANDLE*)FunMngr->RegManager);
				}
			}
		}
	}
}


/****************************************************************************
  Description: Get the operation mode value
-----------------------------------------------------------------------------
  Parameters:
	FunMngr			: Pointer to Function Manager handle
	ch				: FUN_CH1 or FUN_CH2
  Return:
	mode value
*****************************************************************************/
USHORT FunGetOpeModeRegister(FUN_MANAGER *FunMngr, FUN_EXECH ch)
{
	USHORT mode;
	mode = 0;
	if(FunMngr->cFunMode[ch] != NULL)
	{
		mode = FunMngr->cFunMode[ch]->FnMode;
	}
	return mode;
}





