/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FunMngr.c : �⏕�@�\�Ǘ����W���[��															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		�@�\ : �T�[�{�⏕�@�\(FnXXX)�̎��s���䁕���s�Ǘ����s���B									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2002.11.08  K.Hamasuna															*/
/*		Rev.1.01 : 2006.02.15  K.Hamasuna	�A�v���ˑ��̕⏕�@�\���s�Ǘ��\���̃����o������			*/
/*		Rev.1.10 : 2008.04.23  T.Taniguchi	�\�[�X�t�@�C������										*/
/*		Rev.1.20 : 2013.05.17  K.Ozaki 																*/
/*		Rev.1.30 : 2013.09.26  K.Ozaki 		<S00C>													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Function.h" /* #include "Round.h" */
#include "LcdOp.h"		/* ��ŏ���??? */


/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
#define OPEAPP_BSS_SIZE (256)
struct _App{									/* 													*/
		UCHAR	State;							/* ���s�X�e�[�g										*/
		UCHAR	Refer;							/* ���s�󋵃t���O									*/
		USHORT	RCode;							/* Return Code										*/
	/*----------------------------------------------------------------------------------------------*/
		FUNMSG	OpFunMsg;						/* Operator Function Message						*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	(*Begin)(FUNMSG *);				/* Function Begin Method 							*/
		LONG	(*Main)(FUNMSG *);				/* Function Main Method 							*/
		void	(*End)(FUNMSG *);				/* Function End Method 								*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	Bss[OPEAPP_BSS_SIZE];			/* Function common variable area 					*/
	/*----------------------------------------------------------------------------------------------*/
};
/*--------------------------------------------------------------------------------------------------*/
CFUNCTION CFunction[CFUN_ENT_NUM];			/* Function Access Class Instance 						*/
struct _App App;							/* Operator Application Instance						*/
/*--------------------------------------------------------------------------------------------------*/
#define FNSEARCH_FORWARD (1)				/* forward search */
#define FNSEARCH_BACKWARD (-1)				/* backward search */

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Number																			*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_FUNNUMBER( tbl, idx )   (((CFUNTBL *)(tbl))[(idx)].FnNo)


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxInitFunManager( void );			/* Initialize Function									*/
void	RpxPrstFunManager( void );			/* Parameter Reset										*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFnSearch(						/* Index Search Method (binary search)					*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFnSearchB(						/* Next Index Search Method (linear search) 			*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFunSearchNext(					/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFunSearchPrev(					/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFunSearchIdxNxt(					/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				LONG Dir,					/* Searching Direction 		 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFunAttributeChk(					/* Check Function Attributes							*/
		CFUNDEF	*pFunDef,					/* Pointer to the Function Definition					*/
		UCHAR	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		);				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFunGetNumber(					/* Get Function Number from Object Index				*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxFunChkLong(						/* 														*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg );						/* Message Interface 									*/


/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Function Management API														*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxFunExeBeginLcd(					/* LCD�I�y���[�^����̕⏕�@�\�J�n����					*/
		struct _CFunction *this,			/* this Pointer 										*/
		LONG	FunIdx,						/* Index of the Function								*/
		UINT 	*FLcdKeyMk,					/* LCD�L�[MAKE(OFF->ON)�t���O							*/
		UINT 	*FLcdKeyBrk,				/* LCD�L�[BREAK(ON->OFF)�t���O							*/
		UINT 	*FLcdKeyLvl,				/* LCD�L�[���x���t���O									*/
		UINT 	*FOpBlnkKickFlg,			/* LCD�_�ŗp�t���O										*/
		UCHAR	*FTxBuff,					/* LCD�I�y���[�^�\�����								*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxFunExeLcd(						/* LCD�I�y���[�^����̕⏕�@�\���s						*/
		struct _CFunction *this,			/* this Pointer 										*/
		HMSGIF	 Hmsg			  );		/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxFunFinishLcd(					/* LCD�I�y���[�^����̕⏕�@�\�I������					*/
		struct _CFunction *this,			/* this Pointer 										*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxFunSetOpeApl( 					/* Set Application Information */
		CFUNDEF	*pFunDef,					/* Pointer to the Function Definition					*/
		struct _App *pApp, 					/* Pointer to the Application 							*/
		UINT 	*FLcdKeyMk,					/* LCD�L�[MAKE(OFF->ON)�t���O							*/
		UINT 	*FLcdKeyBrk,				/* LCD�L�[BREAK(ON->OFF)�t���O							*/
		UINT 	*FLcdKeyLvl,				/* LCD�L�[���x���t���O									*/
		UINT 	*FOpBlnkKickFlg,			/* LCD�_�ŗp�t���O										*/
		UCHAR	*FTxBuff,					/* LCD�I�y���[�^�\�����								*/
		HMSGIF	Hmsg);						/* Message Interface									*/


/****************************************************************************************************/
/*																									*/
/*		Dummy Function																				*/
/*																									*/
/****************************************************************************************************/
//LONG	LpxFnReadDummy( void *this, COBJDEF *pObjDef, ULONG *pResData, ULONG CheckType, MSGIF *hMsg ) { return OK; }
//LONG	LpxFnReadXDummy( void *this, LONG Index, ULONG *pResData, ULONG CheckType, MSGIF *hMsg ) { return OK; }
LONG	LpxFnReadDummy( void *this, LONG Index, ULONG *pResData, ULONG CheckType, MSGIF *hMsg ) { return OK; }  /* <S073> */


/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\�Ǘ�����������																		*/
/*																									*/
/****************************************************************************************************/
void	RpxInitFunManager( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Set LCD Function Access Methods																*/
/*--------------------------------------------------------------------------------------------------*/
		CLCDFUN->FnTblEntNum = FNFUNTBL_ENTNUM;			/* Set Function Table Entry Number */
		CLCDFUN->pFunTbl = FNFUNTBL_PTR;				/* Set Function Table Pointer */
		CLCDFUN->Read = LpxFnReadDummy; 				/* Read Method (Dummy) */
//<S073>		CLCDFUN->ReadX = LpxFnReadXDummy; 				/* Read Method (Dummy) */
		CLCDFUN->Search = LpxFnSearch; 					/* Index Search Method (binary search) */
		CLCDFUN->SearchB = LpxFnSearchB; 				/* Next Index Search Method (linear search) */
		CLCDFUN->Next = LpxFunSearchNext;				/* Next Index Search */
		CLCDFUN->Prev = LpxFunSearchPrev;				/* Previous Index Search */
		CLCDFUN->Exec = RpxFunExeLcd;					/* Execute Function from LCD Operator */
		CLCDFUN->Begin = RpxFunExeBeginLcd;				/* Begin Function from LCD Operator */
		CLCDFUN->Finish = RpxFunFinishLcd;				/* Finish Function from LCD Operator */
/*--------------------------------------------------------------------------------------------------*/
		CLCDFUN->GetNumber = LpxFunGetNumber;			/* Get Object Number */
		CLCDFUN->isLong = LpxFunChkLong;				/* �O�̂��� */
		CLCDFUN->SearchS = LpxFnReadDummy;				/* @@ ��ŏ���!! <S073> */
		CLCDFUN->SearchBS = LpxFnReadDummy;				/* @@ ��ŏ���!! <S073> */
		CLCDFUN->WriteArray = LpxFnReadDummy;			/* @@ ��ŏ���!! <S073> */
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( &App, sizeof(struct _App)/4 ); 	/* Reset Application */
		return;
}



/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\�Ǘ��p�����[�^���Z�b�g����															*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstFunManager( void )
{
		return;
}



/****************************************************************************************************/
/*																									*/
/* 		Index Search Method for FnFun Table (binary search)											*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFnSearch(						/* Index Search Method (binary search)					*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			)	/* Message Interface									*/
{
int FindIdx = -1;
CFUNTBL	*pTbl = this->pFunTbl;
/*--------------------------------------------------------------------------------------------------*/
/*		Binary Search																				*/
/*--------------------------------------------------------------------------------------------------*/
	{
	int is = 0;								/* start index */
	int ie = this->FnTblEntNum;				/* end index */
	int in = (this->FnTblEntNum)>>1;		/* intermediate index */
		do 
		{
			if ( SrchNo == GET_FUNNUMBER( this->pFunTbl, in ) ) { FindIdx = in; break; }
			else if ( SrchNo < GET_FUNNUMBER( this->pFunTbl, in ) ) { ie = in-1; }
			else { is = in+1; }
			in = (is+ie)>>1;
		} while ( is < ie ) ;
	}
/*--------------------------------------------------------------------------------------------------*/
	if( FindIdx >= 0 )
	{													
		*pFindIdx = FindIdx;										/* Found */
		/*------------------------------------------------------------------------------------------*/
		/* Check Attribute( Access level, etc ) 													*/
		/*------------------------------------------------------------------------------------------*/
		return LpxFunAttributeChk( GET_FUNDEF( pTbl, FindIdx ), CheckType, hMsg);
	}
	else
	{
		*pFindIdx = 0;												/* Not Found */
		return (FERR_FOUND) /*(NG)*/;									 
	}
}

/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\�����̃`�F�b�N����																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v:	CheckType��茈�܂�����ɏ]���āA�w�肵���⏕�@�\���Ó����`�F�b�N����B					*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFunAttributeChk(					/* �⏕�@�\�����̃`�F�b�N����							*/
		CFUNDEF	*pFunDef,					/* Pointer to the Function Definition					*/
		UCHAR	CheckType,					/* �G���[�`�F�b�N�^�C�v									*/
		HMSGIF	Hmsg		)				/* �g�p�����ʃn���h��									*/
{
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;			/* ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		�C���f�N�X�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: Index >= (LONG)FunMngP.FnFunTblEntNum */ )
		{
			return( FERR_FOUND );	/* �p�����[�^�ԍ��G���[ */	
		}
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�^�C�v�`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( CheckType & FCHK_MTR_TYPE )
		{
			if( 1 /* TODO: (pFunDef->ChkSpUse & Bprm.ChkSpUse) */ == 0x00 )
			{
				return(FERR_MTR_TYPE);
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�A�N�Z�X���x���`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (CheckType & FCHK_ACCS_LVL) && (Hmsg != NULL) )
		{
			if(	GET_FNACCLVL( pFunDef ) /* FnFunTbl[Index].FunDefPtr->AccessLevel */ > Hmsg->AccessLvl )
			{
				return(FERR_LEVEL);
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����݋֎~�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( (CheckType & FCHK_PWINH) && CHK_WRITEINHIBIT() /* TODO: (Iprm.f.PrmWrtInhibit) */ )
		{
			if(	0 /* TODO: FnFunTbl[Index].FunDefPtr->ExeDisable */ == TRUE )
			{
				if( Hmsg == NULL )
				{
					return(FERR_INHIBIT);
				}
				else if( Hmsg->AccessLvl != ACCLVL_SYSTEM )
				{
					return(FERR_INHIBIT);
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}


/****************************************************************************************************/
/*																									*/
/* 		Next Get Function Number of FnFun Table 													*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFunGetNumber(					/* Get Function Number from Object Index				*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
CFUNTBL *pTbl = (CFUNTBL *)this->pTbl;

	if( Index >= this->TblEntNum )
	{
		Index = this->TblEntNum;
	}

	return (pTbl[Index].FnNo);
}


/****************************************************************************************************/
/*																									*/
/* 		(�O�̂���)																					*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFunChkLong(						/* 														*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/* 		Next Index Search Method for FnFun Table (linear search)									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFnSearchB(						/* Next Index Search Method (linear search) 			*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
LONG idx;
LONG rtIdx;    								/* return index */
LONG dir;      								/* Search direction */
CFUNTBL *pFunTbl = this->pFunTbl;			/* Pointer to the Function Table */
/*--------------------------------------------------------------------------------------------------*/
	rtIdx = BaseIdx;
/*--------------------------------------------------------------------------------------------------*/
	if ( SrchNo >= GET_FUNNUMBER( this->pFunTbl, BaseIdx ) )
	{ /* forward search */
		dir = FNSEARCH_FORWARD;
	}
	else
	{ /* backword search */
		dir = FNSEARCH_BACKWARD;
	}
/*--------------------------------------------------------------------------------------------------*/
	for ( idx = BaseIdx; (idx < this->FnTblEntNum) && (idx >= 0); idx = idx + dir )
	{
		if ( GET_FUNNUMBER( pFunTbl, idx ) == 0xFFFF ) { break; } /* reached end of the table */
		/*------------------------------------------------------------------------------------------*/
		if ( LpxFunAttributeChk( GET_FUNDEF( pFunTbl, idx), FCHK_DEF, hMsg ) != OK ) { continue; }
		/*------------------------------------------------------------------------------------------*/
		rtIdx = idx; /* visible index */
		if ( (dir == FNSEARCH_FORWARD ) && ( GET_FUNNUMBER( pFunTbl, idx ) >= SrchNo ) ){ break; }
		else if ( (dir == FNSEARCH_BACKWARD ) && ( GET_FUNNUMBER( pFunTbl, idx ) <= SrchNo ) ){ break; }
		/*------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rtIdx);
#if 0
LONG	idx, idx_sv;
LONG	rc;
/*--------------------------------------------------------------------------------------------------*/
/*		Start Procedure																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(BaseIndex >= FunMngP.FnFunTblEntNum)
		{
			BaseIndex = FunMngP.FnFunTblEntNum - 1;
		}
		idx = idx_sv = BaseIndex;
/*--------------------------------------------------------------------------------------------------*/
/*		�������																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( SrchNo >= FnFunTbl[BaseIndex].FnNo )
		{
			for( ; idx < (LONG)FunMngP.FnFunTblEntNum; idx++)
			{
				/* �A�N�Z�X���x�����`�F�b�N */
				rc = FunAttributeChkIdx(idx, FCHK_DEF, Hmsg);
				if( rc != OK ){ continue;}
				if(FnFunTbl[idx].FnNo == 0xFFFF)	/* Fn�ԍ��̍Ō�܂œ��B	*/
				{
					return( (USHORT)idx_sv );
				}
				if(FnFunTbl[idx].FnNo == SrchNo)	/* �ԍ���v */
				{
					return( (USHORT)idx );
				}
				if(FnFunTbl[idx].FnNo > SrchNo)		/* �Y���ԍ������傫�� */
				{
					return( (USHORT)idx );
				}
				idx_sv = idx;		/* �\���\�p�����[�^�Ȃ̂ŕۑ����Ă��� */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�O������																					*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			for( ; idx >= 0; idx--)
			{
				/* �A�N�Z�X���x�����`�F�b�N */
				rc = FunAttributeChkIdx(idx, FCHK_DEF, Hmsg);
				if( rc != OK ){ continue;}
				if(FnFunTbl[idx].FnNo == SrchNo)	/* �ԍ���v */
				{
					return( (USHORT)idx );
				}
				if( FnFunTbl[idx].FnNo < SrchNo )	/* �Y���ԍ����������� */
				{
					return( (USHORT)idx );
				}
				idx_sv = idx;		/* �\���\�p�����[�^�Ȃ̂ŕۑ����Ă��� */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( (USHORT)idx_sv );
#endif
}

/****************************************************************************************************/
/*																									*/
/* 		Previous Index Search Method 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFunSearchNext(					/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
		return LpxFunSearchIdxNxt( this, BaseIdx, FNSEARCH_FORWARD, hMsg );
}

/****************************************************************************************************/
/*																									*/
/* 		Previous Index Search Method 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFunSearchPrev(					/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
		return LpxFunSearchIdxNxt( this, BaseIdx, FNSEARCH_BACKWARD, hMsg );
}

/****************************************************************************************************/
/*																									*/
/* 		Next Index Search Method 																	*/
/*																									*/
/****************************************************************************************************/
LONG	LpxFunSearchIdxNxt(					/* Next Index Search Method								*/
				struct _CFunction *this,	/* this Pointer 										*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				LONG Dir,					/* Searching Direction(1: forward, -1:backward)			*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
LONG	idx;
LONG	rc;
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BaseIdx >= this->FnTblEntNum )
		{
			BaseIdx = this->FnTblEntNum - 1;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Direction																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( ( Dir != 1 ) && ( Dir != -1 ) ){ return BaseIdx; }
/*--------------------------------------------------------------------------------------------------*/
/*		1st Search																					*/
/*--------------------------------------------------------------------------------------------------*/
		for( idx = BaseIdx + Dir; (idx < this->FnTblEntNum) && (idx >= 0); idx += Dir)
		{
		/*------------------------------------------------------------------------------------------*/
		/* Check Access Level, etc 																	*/
		/*------------------------------------------------------------------------------------------*/
			rc = LpxFunAttributeChk( GET_FUNDEF( this->pFunTbl, idx) , FCHK_DEF, hMsg); 				
			if( rc != OK ){ continue;}
			if( GET_FUNNUMBER( this->pFunTbl, idx ) == 0xFFFF ){ continue;}
			if( this->pFunTbl[idx].FunDefAttr->OpeBeginFunc == NULL ){ continue;}		/* <S18F> */
		/*------------------------------------------------------------------------------------------*/
			return( idx );												/* Found */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		If not found, start 2nd search																*/
/*--------------------------------------------------------------------------------------------------*/
		if( idx >= this->FnTblEntNum ) 
		{
			idx = 0; 													/* overflow */
		}
		else 
		{
			idx = this->FnTblEntNum - 1; 								/* underflow */
		}
/*--------------------------------------------------------------------------------------------------*/
		for( ; idx != BaseIdx; idx += Dir )
		{
		/*------------------------------------------------------------------------------------------*/
		/* Check Access Level, etc 																	*/
		/*------------------------------------------------------------------------------------------*/
			rc = LpxFunAttributeChk( GET_FUNDEF( this->pFunTbl, idx), FCHK_DEF, hMsg); 				
			if( rc != OK ){ continue;}
			if( GET_FUNNUMBER( this->pFunTbl, idx ) == 0xFFFF ){ continue;}
			if( this->pFunTbl[idx].FunDefAttr->OpeBeginFunc == NULL ){ continue;}		/* <S18F> */
		/*------------------------------------------------------------------------------------------*/
			return( idx );												/* Found */
		}
/*--------------------------------------------------------------------------------------------------*/
		return( BaseIdx );
}


/****************************************************************************************************/
/*																									*/
/*		Operator Application Begin Method															*/
/*																									*/
/****************************************************************************************************/
LONG	RpxFunExeBeginLcd(					/* LCD�I�y���[�^����̕⏕�@�\�J�n����					*/
		struct _CFunction *this,			/* this Pointer 										*/
		LONG	Index,						/* Index of the Function								*/
		UINT 	*FLcdKeyMk,					/* LCD�L�[MAKE(OFF->ON)�t���O							*/
		UINT 	*FLcdKeyBrk,				/* LCD�L�[BREAK(ON->OFF)�t���O							*/
		UINT 	*FLcdKeyLvl,				/* LCD�L�[���x���t���O									*/
		UINT 	*FOpBlnkKickFlg,			/* LCD�_�ŗp�t���O										*/
		UCHAR	*FTxBuff,					/* LCD�I�y���[�^�\�����								*/
		HMSGIF	Hmsg			)			/* �g�p�����ʃn���h��									*/
{
LONG	rc;
CFUNTBL *pTbl = GET_FUNTBL( this );			/* Get Function Table */
CFUNDEF *pDef = GET_FUNDEF( pTbl, Index );	/* Get Function Definition */
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: RpxGetOpeRegWriteHandle( Hmsg ) != TRUE */)
		{
			return( FALSE );
		}
		else if(0 /* TODO: LpxFunSetExe( pDef, 0, Hmsg ) != OK */)
		{
			/* RpxFreeOpeRegWriteHandle( Hmsg ) */;
			return( FALSE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if ( Index >= this->FnTblEntNum ) { return (FERR_FOUND); }
/*--------------------------------------------------------------------------------------------------*/
/*		Check FunDef																				*/
/*--------------------------------------------------------------------------------------------------*/
		if ( pDef == NULL ) { return (FERR_FOUND); }
/*--------------------------------------------------------------------------------------------------*/
/*		�A�N�Z�X���x�����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxFunAttributeChk( pDef, FCHK_DEF | FCHK_PWINH, Hmsg);
		if( rc != OK ){ return( rc );}
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�������݌��擾																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: RpxGetPrmWriteHandle( Hmsg ) != TRUE */)
		{
			return( FERR_INHIBIT );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�⏕�@�\���s�e�[�u���o�^																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( App.State != OPEAPP_EXEC )
		{
/*--------------------------------------------------------------------------------------------------*/
			LpxFunSetOpeApl( pDef, &App, FLcdKeyMk,				/* Set the Application */
							FLcdKeyBrk, FLcdKeyLvl, 
							FOpBlnkKickFlg, FTxBuff, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
			rc = App.Begin( &(App.OpFunMsg) ); 					/* ���������֐����s			*/
			if( rc == OK )
			{
				App.State = OPEAPP_EXEC;
			}
			else
			{
				;
			}
		}
		else
		{
			rc = FERR_FULL;
		}
/*--------------------------------------------------------------------------------------------------*/
	if( rc != OK ){ /* TODO: RpxFreePrmWriteHandle( Hmsg )*/;}
/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		Operator Application Execute Method															*/
/*																									*/
/****************************************************************************************************/
LONG	RpxFunExeLcd(						/* LCD�I�y���[�^����̕⏕�@�\���s						*/
		struct _CFunction *this,			/* this Pointer 										*/
		HMSGIF	Hmsg			  )			/* �g�p�����ʃn���h��									*/
{
	if ( App.State == OPEAPP_EXEC ) 
	{
		if( App.Main( &(App.OpFunMsg) ) )
		{
			App.State = OPEAPP_END;
			return (1);
		}
		else
		{
			return (0);
		}
	}
	else
	{
		return (1);
	}
}

/****************************************************************************************************/
/*																									*/
/*		Operator Application End Method																*/
/*																									*/
/****************************************************************************************************/
void	RpxFunFinishLcd(					/* LCD�I�y���[�^����̕⏕�@�\�I������					*/
		struct _CFunction *this,			/* this Pointer 										*/
		HMSGIF	Hmsg			)			/* �g�p�����ʃn���h��									*/
{
		if( App.State == OPEAPP_EXEC ) 
		{
			RpxFreeOpeRegWriteHandleAll( Hmsg );		/* <S166> Free Write Handle */
			App.End( &(App.OpFunMsg) );
			App.State = OPEAPP_END;
		}
		return;
}


/****************************************************************************************************/
/*																									*/
/* 		Set Application Information 																*/
/*																									*/
/****************************************************************************************************/
LONG FunBeginDummy(FUNMSG *Fmsg){ return (OK); }		/* Dummy Begin								*/
LONG FunMainDummy(FUNMSG *Fmsg){return (1);}			/* Dummy Main								*/
void FunEndDummy(FUNMSG *Fmsg){;}						/* Dummy End								*/
/****************************************************************************************************/
void	LpxFunSetOpeApl( 					/* Set Application Information */
		CFUNDEF	*pFunDef,					/* Pointer to the Function Definition					*/
		struct _App *pApp, 					/* Pointer to the Application 							*/
		UINT 	*FLcdKeyMk,					/* LCD�L�[MAKE(OFF->ON)�t���O							*/
		UINT 	*FLcdKeyBrk,				/* LCD�L�[BREAK(ON->OFF)�t���O							*/
		UINT 	*FLcdKeyLvl,				/* LCD�L�[���x���t���O									*/
		UINT 	*FOpBlnkKickFlg,			/* LCD�_�ŗp�t���O										*/
		UCHAR	*FTxBuff,					/* LCD�I�y���[�^�\�����								*/
		HMSGIF	Hmsg)						/* Message Interface									*/
{
/*--------------------------------------------------------------------------------------------------*/
/* 		Set Function Message 																		*/
/*--------------------------------------------------------------------------------------------------*/
		pApp->OpFunMsg.Super = Hmsg;
		pApp->OpFunMsg.pKeyMk = FLcdKeyMk;
		pApp->OpFunMsg.pKeyBrk = FLcdKeyBrk;
		pApp->OpFunMsg.pKeyLvl = FLcdKeyLvl;
		pApp->OpFunMsg.pBlnkKick = FOpBlnkKickFlg;
		pApp->OpFunMsg.pTxBuf = FTxBuff;
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( pApp->Bss, OPEAPP_BSS_SIZE/4 );
		pApp->OpFunMsg.pVar = pApp->Bss;							/* Set RAM Pointer */
/*--------------------------------------------------------------------------------------------------*/
/* 		Set Function Method 																		*/
/*--------------------------------------------------------------------------------------------------*/
		pApp->Begin = ( (pFunDef->OpeBeginFunc == NULL) ? 
				FunBeginDummy :
				( pFunDef->OpeBeginFunc ) );
		pApp->Main = ( (pFunDef->OpeMainFunc == NULL) ?
				FunMainDummy :
				(pFunDef->OpeMainFunc) );
		pApp->End = ( (pFunDef->OpeEndFunc == NULL) ?
				FunEndDummy :
				(pFunDef->OpeEndFunc) );
/*--------------------------------------------------------------------------------------------------*/
		return;
}




/***************************************** end of file **********************************************/
