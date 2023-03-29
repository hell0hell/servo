/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdOpFun.c : LCD Function Display Mode														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	�@ �\ : SigmaMessage�ɂ��k�b�c�I�y���[�^�ƒʐM���A�I�y���[�^����̊e�푀������s����B		*/
/*																									*/
/*			1)���j�^�@�\ : �w�胂�j�^�ϐ�(UnXXX)�̕\��												*/
/*			2)�p�����[�^ : �w��p�����[�^(PnXXX)�̕\��,�ҏW,�ύX									*/
/*			3)�⏕�@�\	 : �w��⏕�@�\(FnXXX)�̎��s�Ǝ��s���ʂ̕\��								*/
/*			4)ParamCopy  : �I�y���[�^����EEPROM���g�p�������[�U�p�����[�^�̕ۑ��ƕ���				*/
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
/*		Rev.1.30 : 2012.04.xx  K.Ozaki      Revised for Mercury 									*/
/*		Rev.1.40 : 2013.09.26  K.Ozaki		<S00C>													*/
/*		Rev.1.50 : 2013.10.24  K.Ozaki		<S031> Remove Compile Warning							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"			/* Kernel API */
#include "LcdOp.h"			/* LcdOpIf Common Header */
#include "Function.h"		/* Fn Function */

/****************************************************************************************************/
/*																									*/
/*		Funciton List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function 																			*/
/*--------------------------------------------------------------------------------------------------*/
void	LcdFun( MSGIF *hMsg, DRVSEL *pDrvSel );			/* 2:�⏕�@�\���s���						*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function 																			*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxSaveDispInfoTableFn( void );					/* Save Display Information Table 			*/


/****************************************************************************************************/
/*																									*/
/*		Lcd Function mode (�߂�l: TRUE=���[�h�L�[���������m���ꂽ���Ƃ�����)						*/
/*																									*/
/****************************************************************************************************/
void	LcdFun( MSGIF *hMsg, DRVSEL *pDrvSel )
{
LONG	i;
LONG	FnIdx[4];
UCHAR	*ResBuf = hMsg->ResBuf;
UCHAR	*pFname;
/*--------------------------------------------------------------------------------------------------*/
/*		Lcd Function mode																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( LopV.FunMode )
		{
	  /*--------------------------------------------------------------------------------------------*/
		case LCDFM_Exe:		/* �⏕�@�\���s��														*/
	  /*--------------------------------------------------------------------------------------------*/
			if ( CLCDFUN->Exec( CLCDFUN, hMsg ) )
			{
				LopV.FunMode = LCDFM_Num;
			}
			break;

	  /*--------------------------------------------------------------------------------------------*/
		case LCDFM_Num: default: /* �⏕�@�\�\������												*/
	  /*--------------------------------------------------------------------------------------------*/
			if( OpKeyBrk.data )
			{
				if( CLCDFUN->Begin( 
						CLCDFUN, 
						LopDsp.CurFunIndx, 
						(UINT *)&OpKeyMk,
						(UINT *)&OpKeyBrk,
						(UINT *)&OpKeyLvl,
						(UINT *)&OpBlnkKickFlg,
						ResBuf,
						hMsg ) != OK  )	/* Fn���s�ł��Ȃ���	*/
				{
					OpBlnkKickFlg.no_op = 1;
				}
				else
				{
					LopV.FunMode = LCDFM_Exe;
					LpxSaveDispInfoTableFn( ) ;
				}
			}
			else if( OpKeyMk.up )
			{
				LopDsp.CurFunIndx = CLCDFUN->Prev( CLCDFUN, LopDsp.CurFunIndx, hMsg );
			}
			else if( OpKeyMk.down )
			{
				LopDsp.CurFunIndx = CLCDFUN->Next( CLCDFUN, LopDsp.CurFunIndx, hMsg );
			}
			else if( OpKeyMk.scroll )
			{
				i = LopDsp.CurFunIndx;
				i = CLCDFUN->Next( CLCDFUN, i, hMsg );
				i = CLCDFUN->Next( CLCDFUN, i, hMsg );
				i = CLCDFUN->Next( CLCDFUN, i, hMsg );
				i = CLCDFUN->Next( CLCDFUN, i, hMsg );
				LopDsp.CurFunIndx = i;
			}
		/*------------------------------------------------------------------------------------------*/
		/*		�\������																			*/
		/*------------------------------------------------------------------------------------------*/
			RpxLcdClrTxBuf( ResBuf );
//			RpxLcdSetStr( 0, 7, (UCHAR *)"-FUNCTION-", ResBuf );	/* TITLE	*//* <S154> */
			RpxLcdSetStr( 0, 9, (UCHAR *)"FUNCTION", ResBuf );		/* TITLE	*//* <S154> */
		/*------------------------------------------------------------------------------------------*/
			FnIdx[0] = CLCDFUN->Prev( CLCDFUN, LopDsp.CurFunIndx, hMsg );
			FnIdx[1] = LopDsp.CurFunIndx;
			FnIdx[2] = CLCDFUN->Next( CLCDFUN, LopDsp.CurFunIndx, hMsg );
			FnIdx[3] = CLCDFUN->Next( CLCDFUN, FnIdx[2], hMsg );
		/*------------------------------------------------------------------------------------------*/
			for( i=0; i<4; i++ )									/* Display four lines */
			{
				pFname = (UCHAR *) GET_FNFUNNAME( GET_FUNDEF( CLCDFUN->pFunTbl, FnIdx[i] ) );
				RpxLcdSetStr(    i+1, 0, (UCHAR *)"Fn", ResBuf );
				RpxLcdSetValHex( i+1, 2, CLCDFUN->GetNumber( CLCDFUN, FnIdx[i], hMsg ), 3, ResBuf );
				RpxLcdSetStr(    i+1, 6, pFname, ResBuf );
			}
		/*------------------------------------------------------------------------------------------*/
			RpxLcdSetBlnk(   2, 0, 17, (UCHAR *)ResBuf );			/* Blink current line */
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N���X�e�[�^�X�\������																*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdSetResStatus (ResBuf);								/* <S031> */
		RpxLcdSetBlink (ResBuf);									/* <S031> */
		return;
}


#if	0	/* <S208> */
/****************************************************************************************************/
/*																									*/
/*		Save Display Inf. Table for FnXxx															*/
/*																									*/
/****************************************************************************************************/
void	LpxSaveDispInfoTableFn( void )
{
//#if 0 /* TODO: minaosi */
#if 1	/* <S18F> */
LONG	i;
LONG	ChkSum;
LCDEEP	LcdEep;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Access Level																			*/
/*--------------------------------------------------------------------------------------------------*/
		//if( FnFunTbl[LopDsp.CurFunIndx].FunDefPtr->AccessLevel == ACCLVL_SYSTEM )
		if( FnFunTbl[LopDsp.CurFunIndx].FunDefAttr->AccessLevel == ACCLVL_SYSTEM )		/* <S18F> */
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM�Ǐo��																				*/
/*--------------------------------------------------------------------------------------------------*/
		//KriReadEepromData( EEPADR_LCDINF, &LcdEep, LCDEEP_WDSIZE );
		KriReadEepromData( EEPADR_LCDINF, (USHORT*)(&LcdEep), LCDEEP_WDSIZE );		/* <S18F> */
		for( ChkSum=0, i=0; i < LCDEEP_WDSIZE; i++ )
		{
			ChkSum += *((USHORT *)(&LcdEep) + i);
		}
		if( (USHORT)ChkSum != 0 )
		{
			for( i=0; i < LCDPMDSP_MAX; i++ )				/* �T��������Ȃ��ꍇ�́A 				*/
			{												/* ���𖳌������ɂ���					*/
				LcdEep.accesslvl[i] = 0;					/* (dsipID�����͌�Ő�����)				*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM������(Fn�ԍ����s��v�̏ꍇ�̂ݕۑ�)													*/
/*--------------------------------------------------------------------------------------------------*/
		if( LcdEep.Old_FnNo != FnFunTbl[LopDsp.CurFunIndx].FnNo )
		{
			LcdEep.Old_FnNo = FnFunTbl[LopDsp.CurFunIndx].FnNo;
			for( ChkSum=0, i=0; i < (LCDEEP_WDSIZE - 1); i++ )
			{
				ChkSum += *((USHORT *)(&LcdEep) + i);
			}
			LcdEep.chksum = (USHORT)(-ChkSum);
			//KriWriteEepromData( EEPADR_LCDINF, &LcdEep, LCDEEP_WDSIZE );
			KriWriteEepromData( EEPADR_LCDINF, (USHORT*)(&LcdEep), LCDEEP_WDSIZE );		/* <S18F> */
		}
/*--------------------------------------------------------------------------------------------------*/
#endif
		return ;
}
#else	/* <S208> */
/****************************************************************************************************/
/*																									*/
/*		Save Display Inf. Table for FnXxx															*/
/*																									*/
/****************************************************************************************************/
void	LpxSaveDispInfoTableFn( void )
{
LONG	i;
LONG	ChkSum;
LCDEEP	LcdEep;

/*--------------------------------------------------------------------------------------------------*/
		if( LcdWtReq == TRUE )	return;						/* �������ݒ� 							*/
/*--------------------------------------------------------------------------------------------------*/
/*		Check Access Level																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( FnFunTbl[LopDsp.CurFunIndx].FunDefAttr->AccessLevel == ACCLVL_SYSTEM )
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM�Ǐo��																				*/
/*--------------------------------------------------------------------------------------------------*/
		for( ChkSum = 0, i = 0; i < LCDEEP_WDSIZE ; i++ )
		{
			ChkSum += *((USHORT *)( pLcdEep ) + i );
		}
		if( (USHORT)ChkSum != 0 )
		{
			for( i = 0 ; i < LCDPMDSP_MAX ; i++ )			/* �T��������Ȃ��ꍇ�́A 				*/
			{												/* ���𖳌������ɂ���					*/
				LcdEep.accesslvl[i] = 0;					/* (dsipID�����͌�Ő�����)				*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM������(Fn�ԍ����s��v�̏ꍇ�̂ݕۑ�)													*/
/*--------------------------------------------------------------------------------------------------*/
		if( pLcdEep->Old_FnNo != FnFunTbl[LopDsp.CurFunIndx].FnNo )
		{
			pLcdEep->Old_FnNo = FnFunTbl[LopDsp.CurFunIndx].FnNo;
			for( ChkSum = 0, i = 0; i < (LCDEEP_WDSIZE - 1 ) ; i++ )
			{
				ChkSum += *((USHORT *)( pLcdEep ) + i );
			}
			pLcdEep->chksum = (USHORT)(-ChkSum);
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM�����ݗv��																			*/
/*--------------------------------------------------------------------------------------------------*/
			LcdRomAdr = LcdRomBaseAdr;
			LcdMemAdr = (USHORT*)( pLcdEep );
			LcdWtCnt  = LCDEEP_WDSIZE;
			LcdWtReq  = TRUE;
/*--------------------------------------------------------------------------------------------------*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return ;
}
#endif	/* <S208> */

