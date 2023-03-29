/****************************************************************************************************/
/*																									*/
/*																									*/
/*		DataTraceManager.c : �f�[�^�g���[�X����														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �f�[�^�g���[�X����																		*/
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
/*	Note	:	����	2010.10.26	T.Kira	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MLib.h"
#include "XlsTrcDef.h"
#include "DataTraceCalc.h"
#include "DataTraceManager.h"
#include "Memory.h"					/* @@ */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static BOOL	dtrcCalcTraceObject( TRCHNDL *TrcHndl, LONG Tno );
static LONG	dtrcSearchTrcBitTbl( TRCHNDL *TrcHndl, LONG SelNo );
//static LONG	dtrcCalcSetSampLlim( void* Axis, LONG Axno );
static void dtrcSetTrgParameter( LONG TrgNo, SHORT Adj, TRCEXEP *TrcExeP, TRCREG *TrcReg );
static void dtrcClearTrgParameter( TRCTRG *TrcTrg );
//
static LONG	dtrcGetMergeData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx );
static void	dtrcGetTrcData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx, LONG *VerData, LONG *BitData, LONG Idx );
static BOOL	dtrcCheckTriggerProcedure( TRCHNDL *TrcHndl );
static BOOL	dtrcCheckTrigger( LONG TrgNo, TRCHNDL *TrcHndl, LONG *TrgVar );
static BOOL	dtrcFinishOpeModeOperation( TRCHNDL *TrcHndl );
static void* dtrcConvAxisAddress( MEMOBUS_ADDR_MAP *AxisMap, LONG Address, LONG *AxisNum );
static void* dtrcGetTopAxisAddress( MEMOBUS_ADDR_MAP *AxisMap );
//
static BOOL	dtrcSetDataTraceCmd( TRCHNDL *TrcHndl, ULONG OpeModeTrc, ULONG OpeCmdTrc );
static BOOL	dtrcStartDataTrace( TRCHNDL *TrcHndl, UCHAR mode );
static BOOL	dtrcStopDataTrace( TRCHNDL *TrcHndl );
static BOOL	dtrcStartRingTrace( TRCHNDL *TrcHndl );
static BOOL	dtrcStopRingTrace( TRCHNDL *TrcHndl, LONG finmode );
//
static void	DtrcSelSetInitValue( TRCHNDL *TrcHndl );	/* <S0C9> */

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X�Ǘ�����������																*/
/*																									*/
/****************************************************************************************************/
void	DtrcInitDtrcManager( TRCHNDL *TrcHndl, USHORT *TraceBuffer, MEMOBUS_ADDR_MAP *AxisMap )
{
	LONG	EntNum,i;	
	TRCMNGP	*TrcMngP;
	TrcMngP = &TrcHndl->TrcMngP;

	/*----------------------------------------------------------------------------------------------*/
	/*		���l�g���[�X��`�Q���������̐ݒ�														*/
	/*----------------------------------------------------------------------------------------------*/
	EntNum = TRCVARTBL_ENTNUM;
	TrcMngP->TrcVarTblEntNum = (ULONG)EntNum;
	TrcMngP->TrcVarTblSrhIniW = (1 << MlibSrhbiton( EntNum - 1, SRH_FROM_MSB ));
	/*----------------------------------------------------------------------------------------------*/
	/*		�r�b�g�g���[�X��`�Q���������̐ݒ�														*/
	/*----------------------------------------------------------------------------------------------*/
	EntNum = TRCBITTBL_ENTNUM;
	TrcMngP->TrcBitTblEntNum = (ULONG)EntNum;
	TrcMngP->TrcBitTblSrhIniW = (1 << MlibSrhbiton( EntNum - 1, SRH_FROM_MSB ));
	/*----------------------------------------------------------------------------------------------*/
	/*		�f�[�^�g���[�X�Ǘ��f�[�^�ϐ����Z�b�g													*/
	/*----------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &TrcHndl->TrcMngV, sizeof(TrcHndl->TrcMngV)/4 );
	MlibResetLongMemory( &TrcHndl->TrcExeV, sizeof(TrcHndl->TrcExeV)/4 );
	/*----------------------------------------------------------------------------------------------*/
	/*		���A�h���X��`���擾																	*/
	/*----------------------------------------------------------------------------------------------*/
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		TrcHndl->AxisMap[i] = AxisMap[i];
	}

/* <S0C9>	Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�Ώۃf�[�^�I���AI/O�g���[�X�Ώۃf�[�^�I���̏����l�ݒ�							*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcSelSetInitValue( TrcHndl );
/* <S0C9>	End */

#ifdef _DATA_TRACE_DBUG_
//�f�o�b�O�p
	TrcReg->TrcMode     = 0x0010;
	TrcReg->DataSize    = 0x0003;
	TrcReg->AreaSize    = 8192;
	TrcReg->SampIntv    = 0x0001;
	TrcReg->SampTime    = 250;
	TrcReg->SampLlim    = 1;
	TrcReg->SampUlim    = 255;
	TrcReg->MaxAreaSize = 8192;
	TrcReg->OpeModeTrc  = 0;
	TrcReg->OpeCmdTrc   = 0;

	TrcReg->PtrgDnum  = 0x0064;
	TrcReg->TrgSel    = 0x0100;
	TrcReg->TrgLevel  = 0x100;
	TrcReg->TrgPoint  = 0x0000;
	TrcReg->TrcState  = 0x0000;
	TrcReg->MTrgMode  = 0;
	TrcReg->MTrgSel   = 0;
	TrcReg->MTrgLevel = 0;
#endif
	/*----------------------------------------------------------------------------------------------*/
	/*		�f�[�^�g���[�X�p�����[�^�v�Z															*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcCalcParameter( TrcHndl, TraceBuffer );			/* �f�[�^�g���[�X���s�p�����[�^�v�Z			*/

}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X���s�p�����[�^�v�Z����														*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT DtrcCalcParameter( TRCHNDL *TrcHndl, USHORT *TraceBuffer )
{
	ULONG	i;
	ULONG	x,y,z;
	SHORT	Adj;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCINDIVPRM	*TrcIncivPrm;	/* <S0C9> */

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;	/* <S0C9> */

	TrcExeP->TrcPrmOk = FALSE;								/* �f�[�^�g���[�X�p�����[�^�Z�b�gOK		*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�T���v�����O�����ŏ��l�ݒ�														*/
	/*----------------------------------------------------------------------------------------------*/
	/*for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		x = dtrcCalcSetSampLlim( TrcHndl, i );
	
		if(x > TrcReg->SampLlim)
		{
			TrcReg->SampLlim = (USHORT)x;
		}
	}*/
/* <S0C9>	Delete Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�G���A�T�C�Y																	*/
	/*----------------------------------------------------------------------------------------------*/
	//if ( TrcReg->AreaSize > TrcReg->MaxAreaSize )
	//{
	//	return PRM_RSLT_CALC_ERR;
	//}
/* <S0C9>	Delete End */
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�f�[�^�^�C�v																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->DataSize == 1 || TrcReg->DataSize == 3 )				/* �V���[�g�f�[�^�g���[�X	*/
	{
		TrcExeP->TrcDataType = DATATRACE_SHORT;
	}
	else if( TrcReg->DataSize == 2 || TrcReg->DataSize == 4 )			/* �����O�f�[�^�g���[�X		*/
	{
		TrcExeP->TrcDataType = DATATRACE_LONG;
	}
	else
	{
		return PRM_RSLT_CONDITION_ERR;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		IO�g���[�X�^�C�v																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->DataSize == 1 || TrcReg->DataSize == 2 )
	{
		TrcExeP->TrcIOType = IODATA_MERGE;						/* IO�f�[�^��ϐ��f�[�^�Ɍ���		*/
		Adj = 0;												/* �g���[�X�{���A�r�b�g�V�t�g�����l */
	}
	else if( TrcReg->DataSize == 3 || TrcReg->DataSize == 4 )
	{
		TrcExeP->TrcIOType = IODATA_DIV;						/* IO�f�[�^���p�̈�Ɋi�[			*/
		Adj = 1;												/* �g���[�X�{���A�r�b�g�V�t�g�����l */
	}
	else
	{ /* ����` */
		TrcExeP->TrcIOType = IODATA_MERGE;						/* IO�f�[�^��ϐ��f�[�^�Ɍ���		*/
		Adj = 0;												/* �g���[�X�{���A�r�b�g�V�t�g�����l */
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�g���[�X�f�[�^�㉺���f�[�^�A�r�b�g�Z�b�g�^�N���A�f�[�^										*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcExeP->TrcIOType == IODATA_MERGE )							/* IO�f�[�^��ϐ��f�[�^�Ɍ���	*/
	{	 
		if( TrcExeP->TrcDataType == DATATRACE_SHORT )
		{	/* �V���[�g�f�[�^�g���[�X */
			TrcExeP->UpperLmt =  16383;
			TrcExeP->LowerLmt = -16384;
			TrcExeP->TrcBitSet = 0x8000;
			TrcExeP->TrcBitClr = ~TrcExeP->TrcBitSet;
		}
		else
		{	/* �����O�f�[�^�g���[�X */
			TrcExeP->UpperLmt =  1073741823;
			TrcExeP->LowerLmt = -1073741824;
			TrcExeP->TrcBitSet = 0x80000000;
			TrcExeP->TrcBitClr = ~TrcExeP->TrcBitSet;
		}
	}
	else															/* IO�f�[�^���p�̈�Ɋi�[		*/ 
	{
		if( TrcExeP->TrcDataType == DATATRACE_SHORT )
		{	/* �V���[�g�f�[�^�g���[�X */
			TrcExeP->UpperLmt =  32767;
			TrcExeP->LowerLmt = -32768;
			TrcExeP->TrcBitSet = 1;
			TrcExeP->TrcBitClr = 0;
		}
		else
		{	/* �����O�f�[�^�g���[�X	*/
			TrcExeP->UpperLmt =  0x7FFFFFFF; /*  2147483647	*/
			TrcExeP->LowerLmt =  0x80000000; /* -2147483648 */
			TrcExeP->TrcBitSet = 1;
			TrcExeP->TrcBitClr = 0;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�f�[�^�{��																		*/
	/*----------------------------------------------------------------------------------------------*/
	x = TrcReg->TrcMode;
	if( (x < 1) || (x > TRC_CH_MAXNUM ) )
	{
		return PRM_RSLT_CALC_ERR;
	}

/* <S0C9>	Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�o�b�t�@���A�g���[�X�G���A�T�C�Y												*/
	/*																								*/
	/*		�g���[�X�o�b�t�@���́A��-7�V���[�Y�Œǉ����ꂽ���W�X�^�ł��B							*/
	/*		�g���[�X�o�b�t�@�� = 0�Ɛݒ肳�ꂽ�ꍇ�A��-�X�݊����[�h�Ƃ��āu�g�p�G���A�T�C�Y�v��		*/
	/*		�L���ƂȂ�܂��B																		*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	if ( TrcReg->TrcBuffSize != 0 )		/* �g���[�X�o�b�t�@�� �L�� (�g���[�X�o�b�t�@�� �� 0) */
	{
		if ( TrcReg->TrcBuffSize > TrcReg->MaxTrcBuffSize )
		{
			return PRM_RSLT_CALC_ERR;
		}
		else
		{
			TrcReg->AreaSize = TrcReg->TrcBuffSize * TrcReg->TrcMode;
		}
	}
	else		/* �g���[�X�G���A�T�C�Y �L�� (�g���[�X�o�b�t�@�� = 0) */
	{
		if ( TrcReg->AreaSize > TrcReg->MaxAreaSize )
		{
			return PRM_RSLT_CALC_ERR;
		}
	}
/* <S0C9>	End */

	/*----------------------------------------------------------------------------------------------*/
	/*		�v���g���K�f�[�^���`�F�b�N																*/
	/*----------------------------------------------------------------------------------------------*/
//#if CSW_NET_TRG_USE == TRUE		/* <S012> *//* <S021> */
//	y = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? x : (2*x);
//#else /* CSW_NET_TRG_USE */
//	y = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? 1 : 2;		/* <S0C9>	Delete */
//#endif /* CSW_NET_TRG_USE */
	y = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? x : (2*x);	/* <S0C9> */

	TrcExeP->TrcDnum = TrcReg->AreaSize / y;

	if( TrcReg->PtrgDnum >= TrcExeP->TrcDnum )
	{
		return PRM_RSLT_CALC_ERR;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�f�[�^�o�b�t�@																	*/
	/*----------------------------------------------------------------------------------------------*/
	x = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? TrcExeP->TrcDnum : ( 2 * TrcExeP->TrcDnum );
//	y = TRC_CH_MAXNUM + Adj;
	y = TRC_CH_MAXNUM;			/* <S1B6>  +Adj����Ɨ̈浰�ް�ƂȂ� */
	z = TrcReg->TrcMode + Adj;
	for( i=0; i < y; i++ )
	{
		if( z > i )
		{
			TrcExeP->TrcObj[i].TrcBuf = TraceBuffer + (i*x);
		}
		else
		{
			TrcExeP->TrcObj[i].TrcBuf = NULL;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�f�[�^�T���v�����O�Ԋu															*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcReg->SampIntv > 64)			/* �I�[�o�[�t���[���Ȃ��T���v�����O�Ԋu�� 2^(30-24)=64		*/
	{
		TrcExeP->DataSampIntv = 64;
	}
	else
	{
		TrcExeP->DataSampIntv = TrcReg->SampIntv;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�Ώیʃp�����[�^�v�Z															*/
	/*----------------------------------------------------------------------------------------------*/
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		if( dtrcCalcTraceObject( TrcHndl, i ) == FALSE )
		{
			return PRM_RSLT_CALC_ERR;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���K�����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
	dtrcSetTrgParameter( SINGLE_TRG, Adj,TrcExeP, TrcReg );

	/*----------------------------------------------------------------------------------------------*/
	/*		�}���`�g���K�����ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( (TrcReg->MTrgMode == 1) || (TrcReg->MTrgMode == 2) )
	{
		dtrcSetTrgParameter( MULTI_TRG, Adj, TrcExeP, TrcReg );
	}
	else
	{
		dtrcClearTrgParameter( &TrcExeP->TrcTrg[MULTI_TRG] );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�f�[�^�g���[�X�p�����[�^�v�Z����														*/
	/*----------------------------------------------------------------------------------------------*/
	TrcExeP->TrcPrmOk = TRUE;							/* �f�[�^�g���[�X�p�����[�^�n�j�Z�b�g		*/
	return( PRM_RSLT_SUCCESS );							/* �p�����[�^�v�Z�������^�[��				*/

}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X �g���K�����ݒ�																*/
/*																									*/
/****************************************************************************************************/
static void dtrcSetTrgParameter( LONG TrgNo, SHORT Adj, TRCEXEP *TrcExeP, TRCREG *TrcReg )
{
#if 0	/* <S0C9> Delete Start */
	LONG x,y,z;
	TRCTRG *TrcTrg;
#if CSW_NET_TRG_USE == FALSE		/* <S012> */
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];
	
	x = (TrcReg->TrgSel       & 0x000F);									/* �g���K�f�[�^�I��		*/
	y = (TrcReg->TrgSel >> 4) & 0x000F;										/* �g���K�ΏۑI��		*/
	z = (TrcReg->TrgSel >> 8) & 0x000F;										/* �g���K�ΏۑI��		*/

	if(z > 0)
	{
		if(y == 0)															/* ���l���x���g���K		*/
		{
			TrcTrg->TrgEdge  = (USHORT)z;
			TrcTrg->TrgBitSetMsk = 0;
			TrcTrg->TrgBufx  = TrcExeP->TrcObj[x].TrcBuf;
			TrcTrg->TrgVsft  = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? ( 17-Adj ) : ( 1-Adj );
			TrcTrg->TrgLevel = TrcReg->TrgLevel;
			TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 
																			0 : TrcReg->TrgSel;
		}
		else if( (0 < y)&&(y < TrcReg->TrcMode) )							/* �r�b�g�g���K			*/
		{
			TrcTrg->TrgEdge  = (USHORT)z;
			TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? 
																TrcExeP->TrcBitSet : ( 0x01 << x );
			x  = ( TrcExeP->TrcIOType == IODATA_MERGE )? x : TrcReg->TrcMode;
			TrcTrg->TrgBufx  = TrcExeP->TrcObj[x].TrcBuf;
			TrcTrg->TrgVsft  = 0;
			TrcTrg->TrgLevel = 0;
			TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 
																			0 : TrcReg->TrgSel;
		}
	}
	else																	/* �g���K�w��Ȃ�		*/
	{	/* �g���K�ϐ��N���A */
		dtrcClearTrgParameter( TrcTrg );
	}
#else /* CSW_NET_TRG_USE */		/* <S012> */
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];

	x = (TrcReg->TrgSel       & 0x000F);									/* �g���K�f�[�^�I��		*/
	y = (TrcReg->TrgSel >> 4) & 0x000F;										/* �g���K�^�C�v�I��		*/
//	y = TrcReg.TrgLevel.Long;												/* �g���K�f�[�^�I��		*/

	if( (1 <= x)&&(x <= 7) )
	{ /* ���l���x���g���K */
		TrcTrg->TrgEdge  = (USHORT)y;
		TrcTrg->TrgBufx  = TrcExeP->TrcObj[x-1].TrcBuf;
		TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
		TrcTrg->TrgLevel = TrcReg->TrgLevel;
		TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
	}
	else if( (x == 9)&&(1 <= TrcReg->TrgLevel)&&(TrcReg->TrgLevel <= 7) )
	{ /* �r�b�g�g���K */
		TrcTrg->TrgEdge  = (USHORT)y;
		TrcTrg->TrgBufx  = TrcExeP->TrcObj[y-1].TrcBuf;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
	}
	else if( y == NET_TRG_SEL ) /* <S021> */
	{ /* �l�b�g���[�N�g���K */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = TrcReg->TrgSel;
	}
	else
	{ /* �g���K�w��Ȃ� */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = 0;
	}
#endif
#endif	/* <S0C9> Delete End */


/* <S0C9> Start */
//	LONG x,y,z; /* <Oka02> */
	LONG	TrgDataSel;
	LONG	TrgTypeSel;
	LONG	TrgDataSel2;
	LONG	TrgTypeSel2;
	TRCTRG	*TrcTrg;

#if 0	/* <S142> */
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];

/* <Oka02> */
	if( TrgNo == MULTI_TRG )
	{
		TrcReg->TrgSel   = TrcReg->MTrgSel;
		TrcReg->TrgLevel = TrcReg->MTrgLevel;
	}
/* <Oka02> */

	/* �g���K���擾 */
	TrgDataSel = (TrcReg->TrgSel & 0x000F);				/* �g���K�f�[�^�I��		*/
	TrgTypeSel = (TrcReg->TrgSel >> 4) & 0x000F;		/* �g���K�����I��		*/
	TrgDataSel2 = (TrcReg->TrgSel >> 8) & 0x000F;		/* �g���K�f�[�^�I��2	*/
	TrgTypeSel2 = (TrcReg->TrgSel >> 12) & 0x000F;		/* �g���K�ΏۑI��2		*/
#else
	USHORT	TrgSel;
	LONG	TrgLevel;
	
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];

	if( TrgNo == SINGLE_TRG )
	{
		TrgSel   = TrcReg->TrgSel;
		TrgLevel = TrcReg->TrgLevel;
	}
	else
	{
		TrgSel   = TrcReg->MTrgSel;
		TrgLevel = TrcReg->MTrgLevel;
	}

	/* �g���K���擾 */
	TrgDataSel = (TrgSel & 0x000F);				/* �g���K�f�[�^�I��		*/
	TrgTypeSel = (TrgSel >> 4) & 0x000F;		/* �g���K�����I��		*/
	TrgDataSel2 =(TrgSel >> 8) & 0x000F;		/* �g���K�f�[�^�I��2	*/
	TrgTypeSel2 = (TrgSel >> 12) & 0x000F;		/* �g���K�ΏۑI��2		*/
#endif

/* <S0D2> Start */
#if 0
/* ��������ς��̂��߃R�����g�A�E�g Start */
	if( TrgDataSel != 0 )
	{	/* �g���K�w�肠�� */

		if( TrgDataSel < 15 )
		{	/* �g���K2���g�p */
			if( TrgTypeSel == NET_TRG_SEL )
			{ /* �l�b�g���[�N�g���K */
				TrcTrg->TrgEdge  = 0;
				TrcTrg->TrgBitSetMsk = 0;
				TrcTrg->TrgBufx  = NULL;
				TrcTrg->TrgVsft  = 0;
				TrcTrg->TrgLevel = 0;
				TrcTrg->TrgSel   = TrcReg->TrgSel;
			}
			else
			{
				if( (1 <= TrgDataSel)&&(TrgDataSel <= 8) )
				{ /* ���l���x���g���K */
					TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
					TrcTrg->TrgBitSetMsk = 0;
					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel-1].TrcBuf;
					TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
					TrcTrg->TrgLevel = TrcReg->TrgLevel;
					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
				}
				else if( (TrgDataSel == 9)&&(1 <= TrcReg->TrgLevel)&&(TrcReg->TrgLevel <= 7) )
				{ /* �r�b�g�g���K */
					TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
					TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << x );
					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgTypeSel-1].TrcBuf;
					TrcTrg->TrgVsft  = 0;
					TrcTrg->TrgLevel = 0;
					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
				}
			}
		}
		else
		{/* �g���K2�g�p */
			TrcTrg->TrgEdge  = (USHORT)TrgTypeSel2;
			TrcTrg->TrgBitSetMsk = 0;
			TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel2-1].TrcBuf;
			TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
			TrcTrg->TrgLevel = TrcReg->TrgLevel;
			TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
		}
	}
	else
	{ /* �g���K�w��Ȃ� */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBitSetMsk = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = 0;
	}
/* ��������ς��̂��߃R�����g�A�E�g End */
#endif
/* <S0C9> End */


/*****************************************/
/*	���������蒼�������� <S0D2>		*/
/****************************************/
	if( TrgTypeSel == NET_TRG_SEL )
	{ /* �l�b�g���[�N�g���K */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBitSetMsk = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
//		TrcTrg->TrgSel   = TrcReg->TrgSel;
		TrcTrg->TrgSel   = TrgSel;	/* <S142> */
	}
	else
	{ /* �l�b�g���[�N�g���K�ȊO */
		if( TrgDataSel != 0 )
		{ /* �g���K�w�肠�� */
			if( TrgDataSel < 15 )
			{	/* �g���K2���g�p */
					if( (1 <= TrgDataSel)&&(TrgDataSel <= 8) )
					{ /* ���l���x���g���K */
						TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
						TrcTrg->TrgBitSetMsk = 0;
						TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel-1].TrcBuf;
						TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
//						TrcTrg->TrgLevel = TrcReg->TrgLevel;
						TrcTrg->TrgLevel = TrgLevel;			/* <S142> */
//						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;		/* <S142> */
					}
//					else if( (TrgDataSel == 9)&&(1 <= TrcReg->TrgLevel)&&(TrcReg->TrgLevel <= 7) )
					else if( (TrgDataSel == 9)&&(1 <= TrgLevel)&&(TrgLevel <= 7) )	/* <S142> */
					{ /* �r�b�g�g���K */
						TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
//						TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << x ); /* <Oka02> */
						TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << TrgDataSel ); /* <Oka02> */
						TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel-1].TrcBuf;
						TrcTrg->TrgVsft  = 0;
						TrcTrg->TrgLevel = 0;
//						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;	/* <S142> */
					}
			}
			else
			{ /* �g���K2�g�p */
				if( TrgTypeSel2 == 0 )
				{/* ���l���x���g���K */
						TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
						TrcTrg->TrgBitSetMsk = 0;
						TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel2].TrcBuf;
						TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
//						TrcTrg->TrgLevel = TrcReg->TrgLevel;
						TrcTrg->TrgLevel = TrgLevel;	/* <S142> */
//						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;	/* <S142> */
				}
				else
				{ /* �r�b�g�g���K */
					TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
//					TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << x ); /* <Oka02> */
					TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << TrgDataSel2 ); /* <Oka02> */
//					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel2].TrcBuf;		/* <Oka02> */
					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrcReg->TrcMode].TrcBuf;	/* <Oka02> */
					TrcTrg->TrgVsft  = 0;
					TrcTrg->TrgLevel = 0;
//					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;	/* <S142> */
				}
			}
		}
		else
		{ /* �g���K�w��Ȃ� */
			TrcTrg->TrgEdge  = 0;
			TrcTrg->TrgBitSetMsk = 0;
			TrcTrg->TrgBufx  = NULL;
			TrcTrg->TrgVsft  = 0;
			TrcTrg->TrgLevel = 0;
			TrcTrg->TrgSel   = 0;
		}
	}
/* <S0D2> End */

}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X �g���K�ϐ��N���A																*/
/*																									*/
/****************************************************************************************************/
static void dtrcClearTrgParameter( TRCTRG *TrcTrg )
{
	TrcTrg->TrgSel   = 0;
	TrcTrg->TrgEdge  = 0;
	TrcTrg->TrgVsft  = 0;
	TrcTrg->TrgLevel = 0;
	TrcTrg->TrgBitSetMsk = 0;
	TrcTrg->TrgBufx  = NULL;
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X ���ԍ��ϊ��֐�																*/
/*																									*/
/****************************************************************************************************/
static void* dtrcConvAxisAddress( MEMOBUS_ADDR_MAP *AxisMap, LONG Address, LONG *AxisNum )
{
	LONG i;
//	for(i = 0; i < NumberOfAxes; i++)
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		if(AxisMap[i].AxisAddress == Address)
		{
			*AxisNum = i;
			return AxisMap[i].Axis;
		}
	}
	*AxisNum = -1;
	return NULL;
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X AXIS�擪�A�h���X�擾�֐�														*/
/*																									*/
/****************************************************************************************************/
static void* dtrcGetTopAxisAddress( MEMOBUS_ADDR_MAP *AxisMap )
{
	return AxisMap[0].Axis;
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X�Ώیʃp�����[�^�v�Z����													*/
/*																									*/
/****************************************************************************************************/
BOOL	dtrcCalcTraceObject( TRCHNDL *TrcHndl, LONG Tno )
{
	LONG	i,AxisAddress;
	LONG	TrcSelNum,AxisNum;
	void	*AxisPtr;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCOBJ	*TrcObj;
	TRCINDIVPRM	*TrcIncivPrm;				/* <S0C9> */

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcObj  = &TrcHndl->TrcExeP.TrcObj[Tno];
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;	/* <S0C9> */
	AxisPtr = NULL;

	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�I���`�F�b�N																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcObj->TrcBuf == NULL )
	{
		return( TRUE );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�I�u�W�F�N�g�̏�����															*/
	/*----------------------------------------------------------------------------------------------*/
	TrcObj->TrcExeflg = 0x00;
	TrcObj->Adr.LongAdrData = NULL;
	MlibResetByteMemory( &TrcObj->TrcV, sizeof(TrcObj->TrcV) );
	MlibResetByteMemory( &TrcObj->TrcB, sizeof(TrcObj->TrcB) );
	/*----------------------------------------------------------------------------------------------*/
	/*		���l�g���[�X�Ώۃf�[�^�ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
/* <S0C9>	Start */
//	TrcSelNum = TrcReg->TrcSel[Tno] & 0x00FF;
//	AxisAddress = (TrcReg->TrcSel[Tno] >> 8) & 0x00FF;					

	TrcSelNum = TrcIncivPrm->TrcData[Tno];
	AxisAddress = TrcIncivPrm->TrcAxis[Tno];
/* <S0C9>	End */
	AxisPtr = dtrcConvAxisAddress( TrcHndl->AxisMap, AxisAddress, &AxisNum );
	if( (TrcSelNum == 0x7F) || (TrcSelNum == 0xFF) )							/* ���A�h���X�w��	*/
	{		
		if( (LONG*)(TrcReg->TrcAdr[Tno]) == 0x00000000 )
		{
			/* �A�h���X�w�肪�����l�̏ꍇ�̓_�~�[�ϐ����g���[�X */
			TrcObj->TrcV.GetData = GetVarData_Dummy;
		}
		else
		{
			if( TrcExeP->TrcDataType == DATATRACE_SHORT )
			{
				/* �V���[�g�f�[�^�g���[�X	*/
				TrcObj->Adr.ShortAdrData = (USHORT*)TrcReg->TrcAdr[Tno];	
				TrcObj->TrcV.GetData = GetVarData_AdrShortData;
			}
			else
			{
				/* �����O�f�[�^�g���[�X		*/
				TrcObj->Adr.LongAdrData = TrcReg->TrcAdr[Tno];	
				TrcObj->TrcV.GetData = GetVarData_AdrLongData;
			}
		}
		TrcObj->TrcV.AvrgSel = FALSE;
		TrcObj->TrcV.VxSftL = 0;
		TrcObj->TrcV.VxSftR = ( TrcExeP->TrcDataType == DATATRACE_SHORT )? 16 : 0;
		TrcObj->TrcV.VxGain = 1;
		TrcObj->TrcV.AxisPtr = dtrcGetTopAxisAddress( TrcHndl->AxisMap );
/* <S0C9>	Start */
//		TrcReg->IOTrcSel[Tno] = 0x00;
		TrcIncivPrm->IOTrcData[Tno] = 0x00;
/* <S0C9>	End */	}
	else if( (AxisNum < 0) || (AxisNum > (MAX_AXIS_NUMBER - 1)) )				/* ���ݒ�͈͊O		*/
	{	
		return( FALSE );
	}
	else if( (i = DtrcSearchTrcVarTbl( TrcHndl, TrcSelNum )) >= 0 )				/* ���l�g���[�X�ݒ�	*/
	{
		/*------------------------------------------------------------------------------------------*/
		/*	�g���[�X���ω��I���iTRUE�˕��ω�����AFALSE�˕��ω����Ȃ��j								*/
		/*------------------------------------------------------------------------------------------*/
		TrcObj->TrcV.AvrgSel = TrcVarTbl[i].AvrgSel;
		/*------------------------------------------------------------------------------------------*/
		/*	�g���[�X�ϐ����擾����֐��|�C���^��ݒ�												*/
		/*------------------------------------------------------------------------------------------*/
		if( NULL != TrcVarTbl[i].GetVarData )
		{
			TrcObj->TrcV.GetData = TrcVarTbl[i].GetVarData;
			TrcObj->TrcV.AxisPtr = AxisPtr;
		}
		else
		{
			TrcObj->TrcV.GetData = GetVarData_Dummy;
			TrcObj->TrcV.AxisPtr = NULL;
		}
		/*------------------------------------------------------------------------------------------*/
		/* �g���[�X�ϐ��̃Q�C�����v�Z����֐��|�C���^��ݒ�i�f�t�H���g:�Q�C��=1�j					*/
		/*------------------------------------------------------------------------------------------*/
		if( NULL != TrcVarTbl[i].GetVarGain && TrcObj->TrcV.AxisPtr != NULL )
		{
			TrcObj->TrcV.VxGain = TrcVarTbl[i].GetVarGain( TrcObj->TrcV.AxisPtr );
		}
		else
		{
			TrcObj->TrcV.VxGain = 1;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�r�b�g�V�t�g�ʂ�ݒ�																	*/
		/*------------------------------------------------------------------------------------------*/
		TrcObj->TrcV.VxSftL = TrcVarTbl[i].VxSftL;
		TrcObj->TrcV.VxSftR = TrcVarTbl[i].VxSftR;
	}
	else
	{	/* �g���[�X�ΏۂȂ� */
		return( FALSE );
	}

#if 0 /* �A�h���X����ƂȂ�ꍇ���� @@CHECK */  /* <S001> */
	/*----------------------------------------------------------------------------------------------*/
	/*		���l�g���[�X�ϐ��A�h���X�`�F�b�N														*/
	/*----------------------------------------------------------------------------------------------*/
	if( (LONG)TrcObj->TrcV.GetData & 0x01 )
	{	/* �ݒ�A�h���X�G���[ */
		TrcObj->TrcExeflg  = 0x00;
		TrcObj->Adr.LongAdrData = NULL;
		MlibResetByteMemory( &TrcObj->TrcV, sizeof(TrcObj->TrcV) );
		return( FALSE );
	}
#endif /* <S001> */

	/*----------------------------------------------------------------------------------------------*/
	/*		�r�b�g�g���[�X�Ώۃf�[�^�ݒ�															*/
	/*----------------------------------------------------------------------------------------------*/
/* <S0C9>	Start */
//	TrcSelNum = (TrcReg->IOTrcSel[Tno] & 0xFF);
//	AxisAddress = (TrcReg->IOTrcSel[Tno] >> 8) & 0xFF;

	TrcSelNum = TrcIncivPrm->IOTrcData[Tno];
	AxisAddress = TrcIncivPrm->IOTrcAxis[Tno];
/* <S0C9>	End */

	AxisPtr = dtrcConvAxisAddress( TrcHndl->AxisMap, AxisAddress, &AxisNum );

	if( (AxisNum < 0) || (AxisNum > MAX_AXIS_NUMBER - 1) )					/* ���ݒ�͈͊O			*/
	{	
		return( FALSE );
	}
	if( (i = dtrcSearchTrcBitTbl( TrcHndl, TrcSelNum )) >= 0 )				/* �r�b�g�g���[�X�ݒ�	*/
	{
		if( NULL != TrcBitTbl[i].GetBitData )
		{
			TrcObj->TrcB.GetData = TrcBitTbl[i].GetBitData;
			TrcObj->TrcB.AxisPtr = AxisPtr;
		}
		else
		{
			TrcObj->TrcB.GetData = NULL;
			TrcObj->TrcB.AxisPtr = NULL;
		}
	}
	else
	{
		return( FALSE );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���s�t���O�ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	TrcObj->TrcExeflg = 0x01;
	return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X�Ώەϐ��e�[�u����������														*/
/*																									*/
/****************************************************************************************************/
LONG	DtrcSearchTrcVarTbl( TRCHNDL *TrcHndl, LONG SelNo )
{
	LONG	i,w;
	LONG	EntNum;

	const	TVXDEF *TvxTbl;

	TRCMNGP	*TrcMngP;
	TrcMngP = &TrcHndl->TrcMngP;

	/*----------------------------------------------------------------------------------------------*/
	/*		�����pData�̐ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
	TvxTbl = TrcVarTbl;										/* ��`�e�[�u���|�C���^					*/
	EntNum = TrcMngP->TrcVarTblEntNum;						/* ��`�e�[�u���o�^��					*/
	w = TrcMngP->TrcVarTblSrhIniW;							/* �����������l�ݒ�						*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�����J�nIndex�̐ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
		i = ( SelNo >= TvxTbl[w].SelNo )? (EntNum - w) : 0;	/* �����J�nIndex�ݒ�					*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�e�[�u������																			*/
	/*----------------------------------------------------------------------------------------------*/
	for( w = w>>1; w > 0; w = w>>1 )						/* ����Loop								*/
	{
		if( SelNo >= TvxTbl[i+w].SelNo ){ i = i + w; }		/* ��r��Index�X�V						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�������ʂ̃`�F�b�N																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( SelNo == TvxTbl[i].SelNo )							/* �������ʃ`�F�b�N						*/
	{
		return( i );										/* ��������								*/
	}
	else
	{
		return( -1 );										/* �������s(SelNo����)					*/
	}

}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X�Ώۃr�b�g�e�[�u����������													*/
/*																									*/
/****************************************************************************************************/
LONG	dtrcSearchTrcBitTbl( TRCHNDL *TrcHndl, LONG SelNo )
{
	LONG	i,w;
	LONG	EntNum;

	const	TBXDEF* TbxTbl;
	
	TRCMNGP	*TrcMngP;
	TrcMngP = &TrcHndl->TrcMngP;

	/*----------------------------------------------------------------------------------------------*/
	/*		�����pData�̐ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
	TbxTbl = TrcBitTbl;										/* ��`�e�[�u���|�C���^					*/
	EntNum = TrcMngP->TrcBitTblEntNum;						/* ��`�e�[�u���o�^��					*/
	w = TrcMngP->TrcBitTblSrhIniW;							/* �����������l�ݒ�						*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�����J�nIndex�̐ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
	i = ( SelNo >= TbxTbl[w].SelNo )? (EntNum - w) : 0;		/* �����J�nIndex�ݒ�					*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�e�[�u������																			*/
	/*----------------------------------------------------------------------------------------------*/
	for( w = w>>1; w > 0; w = w>>1 )						/* ����Loop								*/
	{
		if( SelNo >= TbxTbl[i+w].SelNo ){ i = i + w;}		/* ��r��Index�X�V						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�������ʂ̃`�F�b�N																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( SelNo == TbxTbl[i].SelNo )							/* �������ʃ`�F�b�N						*/
	{
		return( i );										/* ��������								*/
	}
	else
	{
		return( -1 );										/* �������s(SelNo����)					*/
	}
}

/****************************************************************************************************/
/*																									*/
/*		�g���[�X���샂�[�h���W�X�^�����ݏ���														*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	DtrcWriteOpeModeTrcRegister( TRCHNDL *TrcHndl, USHORT WriteData )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�ʏ탂�[�h�ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
	if( WriteData == 0 )
	{
		dtrcFinishOpeModeOperation( TrcHndl );				/* ���샂�[�h����I������				*/
		return( PRM_RSLT_SUCCESS );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���[�h�ݒ�F�P�� or �A��														*/
	/*----------------------------------------------------------------------------------------------*/
	else if( (WriteData == OPEMODE_DATATRACE) || (WriteData == OPEMODE_RINGTRACE) )
	{
		if ( TrcHndl->TrcExeV.Opened == TRUE )
		{
			return ( PRM_RSLT_CALC_ERR );
		}
		/*------------------------------------------------------------------------------------------*/
		/* @@ PrmCopy�@�\��TraceBuffer���g�p���邽�߁A�r���ɂ��� 									*/
		/*------------------------------------------------------------------------------------------*/
		//else if ( CTRCMEM->GetBuffer( CTRCMEM, (void *)TrcHndl ) == NULL )
		//{
		//	return ( PRM_RSLT_NOACCRIGHT_ERR );								/* �v�m�F */
		//}
		/*------------------------------------------------------------------------------------------*/
		else
		{
			TrcHndl->TrcExeV.Opened = TRUE;
			TrcHndl->TrcExeV.OpmRun = TRUE;
			return( PRM_RSLT_SUCCESS );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		���̑�																					*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		return( PRM_RSLT_CALC_ERR );
	}
}

/****************************************************************************************************/
/*																									*/
/*		���샂�[�h����I������																		*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcFinishOpeModeOperation( TRCHNDL *TrcHndl )
{
	TRCREG	*TrcReg;
	TrcReg = &TrcHndl->TrcReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		���[�h�`�F�b�N����~����																*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->OpeModeTrc == OPEMODE_NOCMD )
	{														
		dtrcStopDataTrace( TrcHndl );								/* �R�}���h�Ȃ��̏ꍇ�̏���		*/
	}
	else if( TrcReg->OpeModeTrc == OPEMODE_DATATRACE )
	{														
		dtrcStopDataTrace( TrcHndl );								/* �P���f�[�^�g���[�X��~����	*/
	}
	else if( TrcReg->OpeModeTrc == OPEMODE_RINGTRACE )				
	{
		dtrcStopRingTrace( TrcHndl, TRUE );							/* �A���f�[�^�g���[�X��~����	*/
	}
	//TrcHndl->TrcReg.OpeModeTrc = 0x0000;							/* ���Z�b�g���샂�[�h			*/
	TrcHndl->TrcReg.OpeCmdTrc  = 0x0000;							/* ���Z�b�g����R�}���h			*/

	//CTRCMEM->FreeBuffer( CTRCMEM, (void *)TrcHndl );				/* @@ Free TraceBuffer			*/
	return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		�g���[�X����ݒ背�W�X�^�����ݏ���															*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	DtrcWriteOpeCmdTrcRegister( TRCHNDL *TrcHndl, USHORT *TraceBuffer, USHORT WriteData )
{
	PRM_RSLT ErrRes;
	LONG size;

	TRCREG	*TrcReg;
	TrcReg = &TrcHndl->TrcReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���[�h�F�P�� or �A��															*/
	/*----------------------------------------------------------------------------------------------*/
	if( (TrcReg->OpeModeTrc == OPEMODE_DATATRACE ) || (TrcReg->OpeModeTrc == OPEMODE_RINGTRACE) )
	{
		/* �o�b�t�@�T�C�Y�`�F�b�N */
//#if CSW_NET_TRG_USE == TRUE		/* <S012> *//* <S021> */
//		size = TrcReg->AreaSize;
//#else /* CSW_NET_TRG_USE */
//		size = TrcReg->TrcMode * TrcReg->AreaSize;	/* <S0C9>	Delete */
		size = TrcReg->AreaSize;					/* <S0C9> */
//#endif /* CSW_NET_TRG_USE */
		if(size > TRC_BUF_SIZE)
		{
			return PRM_RSLT_CALC_ERR;
		}

		/* �g���[�X����R�}���h�ݒ菈�� */
		if( TRUE == dtrcSetDataTraceCmd( TrcHndl, TrcReg->OpeModeTrc, WriteData ) )
		{ 
			ErrRes = PRM_RSLT_SUCCESS;
		}
		else
		{
			ErrRes = PRM_RSLT_CALC_ERR;		
		}
		return( ErrRes );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		���̑��̃��[�h																			*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		return( PRM_RSLT_CALC_ERR );
	}
}

/****************************************************************************************************/
/*																									*/
/*		�g���[�X����R�}���h�ݒ菈��																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcSetDataTraceCmd( TRCHNDL *TrcHndl, ULONG OpeModeTrc, ULONG WriteData )
{
	LONG	rc;
	TRCREG	*TrcReg;
	TrcReg = &TrcHndl->TrcReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���[�h�F�P��																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->OpeModeTrc == OPEMODE_DATATRACE )
	{
		if( WriteData == 0x0000 )
		{
			rc = dtrcStopDataTrace( TrcHndl );						/* �P���f�[�^�g���[�X��~����	*/
		}
		else if( WriteData == 0x0001 )
		{
			rc = dtrcStartDataTrace( TrcHndl, TRCCMD_DATATRACE );	/* �P���f�[�^�g���[�X�J�n����	*/
		}
		else if ( WriteData == 0x0011 )					
		{
			rc = dtrcStartDataTrace( TrcHndl, TRCCMD_CONTTRACE );
		}
		else
		{
			return( FALSE );										/* �R�}���h�ُ�					*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���[�h�F�A��																	*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		if( WriteData == 0x0000 )
		{
			rc = dtrcStopRingTrace( TrcHndl, FALSE );				/* �A���f�[�^�g���[�X��~����	*/
		}
		else if( WriteData == 0x0001 )
		{
			rc = dtrcStartRingTrace( TrcHndl );						/* �A���f�[�^�g���[�X�J�n����	*/
		}
		else
		{
			return( FALSE );										/* �R�}���h�ُ�					*/
		}
	}
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		�P���f�[�^�g���[�X�X�^�[�g����																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStartDataTrace( TRCHNDL *TrcHndl, UCHAR mode )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�P���g���[�X�X�^�[�g�`�F�b�N															*/
	/*----------------------------------------------------------------------------------------------*/
	if ( (TrcHndl->TrcExeV.TrcCmd != TRCCMD_CONTTRACE) || (TrcHndl->TrcExeV.OpmRun != TRUE) )
	{
		if( TrcHndl->TrcExeV.TrcCmd != TRCCMD_NOCMD )
		{
			return( FALSE );
		}
		else if( TrcHndl->TrcExeP.TrcPrmOk != TRUE )
		{
			return( FALSE );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�P���g���[�X�X�^�[�g����																*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcResetAverageVar( TrcHndl );							/* ���ω��ϐ�������						*/
	TrcHndl->TrcReg.TrcState = 0;							/* �g���[�X���s��ԃ��Z�b�g				*/
	//TrcHndl->TrcExeP.TrcObj[0].TrcExeflg &= 0x0F;			/* �g���[�X�b�g�O���s����				*/
	TrcHndl->TrcExeV.TrcCmd = mode;							/* �P���g���[�X���s�w��					*/
	DtrcInitExecute( TrcHndl );								/* �f�[�^�g���[�X���s���� ����������	*/
															/* �g���[�X���s�w��(or���[�h�ێ�)		*/

	
	return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		�P���f�[�^�g���[�X�X�g�b�v����																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStopDataTrace( TRCHNDL *TrcHndl )
{
		TrcHndl->TrcExeV.TrcCmd = TRCCMD_NOCMD;
		TrcHndl->TrcExeV.OpmRun = FALSE;
		TrcHndl->TrcExeV.Opened = FALSE;
	
		return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		�A���f�[�^�g���[�X�X�^�[�g����																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStartRingTrace( TRCHNDL *TrcHndl )
{
		/*------------------------------------------------------------------------------------------*/
		/*		�A���g���[�X�X�^�[�g�`�F�b�N														*/
		/*------------------------------------------------------------------------------------------*/
		if( TrcHndl->TrcExeV.TrcCmd != TRCCMD_NOCMD )
		{
			return( FALSE );
		}
		else if( TrcHndl->TrcExeP.TrcPrmOk != TRUE )
		{
			return( FALSE );
		}

		/*------------------------------------------------------------------------------------------*/
		/*		�A���g���[�X�X�^�[�g����															*/
		/*------------------------------------------------------------------------------------------*/
		DtrcResetAverageVar( TrcHndl );							/* ���ω��ϐ�������					*/
		TrcHndl->TrcReg.TrcState   = 0;							/* �g���[�X���s��ԃ��Z�b�g			*/
		//TrcHndl->TrcReg.RngTrcPtr  = 0;						/* �A���g���[�X�����݈ʒu���Z�b�g	*/
		//TrcHndl->TrcExeV.RngTrcCnt = 0;						/* �A���g���[�X�����݉񐔃��Z�b�g	*/
		//TrcHndl->TrcExeP.TrcObj[0].TrcExeflg &= 0x0F;			/* �g���[�X�b�g�O���s����			*/
		TrcHndl->TrcExeV.TrcCmd = TRCCMD_RINGTRACE;				/* �A���g���[�X���s�w��				*/
		DtrcInitExecuteRingTrc( TrcHndl );

		return( TRUE );

}

/****************************************************************************************************/
/*																									*/
/*		�A���f�[�^�g���[�X�X�g�b�v����																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStopRingTrace( TRCHNDL *TrcHndl, LONG finmode )
{
		TrcHndl->TrcExeV.TrcCmd  = TRCCMD_NOCMD;
		TrcHndl->TrcExeV.OpmRun  = FALSE;
		TrcHndl->TrcReg.TrcState = 3;

		if(finmode == TRUE)	
		{
			TrcHndl->TrcExeV.Opened = FALSE;
		}
		return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X���s����@�����������i�P���f�[�^�g���[�X�j									*/
/*																									*/
/****************************************************************************************************/
void DtrcInitExecute( TRCHNDL *TrcHndl )
{
	TrcHndl->TrcReg.TrgPoint = 0;
	TrcHndl->TrcExeV.TrcIndx = 0;
	TrcHndl->TrcExeV.TrcStep = ((TrcHndl->TrcExeP.TrcTrg[0].TrgSel != 0x0000) 
								|| (TrcHndl->TrcExeP.TrcTrg[1].TrgSel != 0x0000)) ? 1 : 5;
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X���s����@�����������i�A���f�[�^�g���[�X�j									*/
/*																									*/
/****************************************************************************************************/
void DtrcInitExecuteRingTrc( TRCHNDL *TrcHndl )
{
	TrcHndl->TrcReg.TrgPoint   = 0;							/* �g���K�ʒu���Z�b�g				*/
	TrcHndl->TrcExeV.TrcIndx   = 0;							/* �g���[�X�C���f�N�X���Z�b�g		*/
	TrcHndl->TrcExeV.TrcStep   = 7;							/* �g���[�X���s�X�e�b�v���Z�b�g		*/
	TrcHndl->TrcExeV.RngTrcCnt = 0;							/* �A���g���[�X�������݉񐔃��Z�b�g	*/
	TrcHndl->TrcReg.RngTrcPtr  = 0;							/* �A���g���[�X�����݈ʒu���Z�b�g	*/
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X���s����																		*/
/*																									*/
/****************************************************************************************************/
void	DtrcExecute( TRCHNDL *TrcHndl )
{
	ULONG	ulwk;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCEXEV	*TrcExeV;

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcExeV = &TrcHndl->TrcExeV;

	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���s�w�߃`�F�b�N																*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcExeV->TrcCmd == TRCCMD_NOCMD )
	{
		TrcExeV->TrcStep = 0;
		TrcExeV->TrcSamp = 0;
		return;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X�f�[�^�X�V����																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcExeV->TrcSamp >= (TrcReg->SampIntv - TrcExeP->DataSampIntv) )
	{
		DtrcSetTrcVar( TrcHndl );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���s�T���v�����O�`�F�b�N														*/
	/*----------------------------------------------------------------------------------------------*/
	if( (++TrcExeV->TrcSamp >= TrcReg->SampIntv) || (TrcExeV->TrcStep == 0) )
	{
		TrcExeV->TrcSamp = 0;
	}
	else
	{
		return;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X���s�V�[�P���X����																*/
	/*----------------------------------------------------------------------------------------------*/
	switch( TrcExeV->TrcStep )
	{
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace for PreTrigger															*/
		/*------------------------------------------------------------------------------------------*/
		case  1:
			TrcReg->TrcState = 1;
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcReg->PtrgDnum )
			{
				TrcExeV->TrcStep = 2;
			}
			if( TrcExeV->TrcIndx >= TrcExeP->TrcDnum ){ TrcExeV->TrcIndx = 0;}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace and Wait Trigger															*/
		/*------------------------------------------------------------------------------------------*/
		case  2:
			TrcExeV->TrgVar[1]  = TrcExeV->TrgVar[0];
			TrcExeV->MTrgVar[1] = TrcExeV->MTrgVar[0];
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( dtrcCheckTriggerProcedure( TrcHndl ) == TRUE )
			{
				TrcReg->TrcState = 2;
				TrcReg->TrgPoint = (USHORT)TrcExeV->TrcIndx;
				TrcExeV->TrcStep = ((TrcExeP->TrcDnum - TrcReg->PtrgDnum) > 1)? 3 : 4;
			}
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum ){ TrcExeV->TrcIndx = 0;}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace and Check End WithTrigger												*/
		/*------------------------------------------------------------------------------------------*/
		case  3:
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum ){ TrcExeV->TrcIndx = 0;}
			if( TrcReg->TrgPoint >= TrcReg->PtrgDnum )
			{
				ulwk = TrcReg->TrgPoint - TrcReg->PtrgDnum;
			}
			else
			{
				ulwk = TrcReg->TrgPoint + TrcExeP->TrcDnum - TrcReg->PtrgDnum;
			}
			if( TrcExeV->TrcIndx == ulwk )
			{
				TrcExeV->TrcStep = 4;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace End Procedure															*/
		/*------------------------------------------------------------------------------------------*/
		case  4:
			TrcReg->TrcState = 3;
			TrcExeV->TrcStep = 0;
			TrcExeV->TrcCmd  = TRCCMD_NOCMD;
			if ( TrcExeV->TrcCmd == TRCCMD_DATATRACE ) { TrcExeV->OpmRun = FALSE; }
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace Procedure(TriggerLess)													*/
		/*------------------------------------------------------------------------------------------*/
		case  5:
			TrcReg->TrcState = 1;
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum )
			{
				TrcExeV->TrcIndx = 0;
				TrcExeV->TrcStep = 6;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Trace End Procedure(TriggerLess)													*/
		/*------------------------------------------------------------------------------------------*/
		case  6:
			TrcReg->TrcState = 3;
			TrcExeV->TrcStep = 0;
			TrcExeV->TrcCmd  = TRCCMD_NOCMD;
			if ( TrcExeV->TrcCmd == TRCCMD_DATATRACE ) { TrcExeV->OpmRun = FALSE; }
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Ring Data Trace																		*/
		/*------------------------------------------------------------------------------------------*/
		case  7:
			TrcReg->TrcState = 1;
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum )
			{
				TrcExeV->TrcIndx = 0;
				TrcExeV->RngTrcCnt++;
			}
			TrcReg->RngTrcPtr = (TrcExeV->RngTrcCnt << 16) +TrcExeV->TrcIndx;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Default(No Operation)																*/
		/*------------------------------------------------------------------------------------------*/
		case  0:
		default:
			break;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���[�X��p�ϐ��F���Z�b�g����															*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcResetAverageVar( TrcHndl );
}

/****************************************************************************************************/
/*																									*/
/*		Set Trace Variable Function																	*/
/*																									*/
/****************************************************************************************************/
void	DtrcSetTrcVar( TRCHNDL *TrcHndl )
{
	LONG i;

	TRCREG	*TrcReg;
	TRCOBJ	*Tobjpx;

	TrcReg  = &TrcHndl->TrcReg;

	for( i=0; i<TrcReg->TrcMode; i++ )
	{
		Tobjpx = &TrcHndl->TrcExeP.TrcObj[i];
		
		/* ���l�g���[�X */
		if(Tobjpx->TrcV.AvrgSel == TRUE )
		{
			/* Add Average Trace Variable */
			Tobjpx->TrcV.VxData += Tobjpx->TrcV.GetData( Tobjpx->TrcV.AxisPtr );
		}
		else
		{
			/* Set Normal Trace Variable */
			Tobjpx->TrcV.VxData = Tobjpx->TrcV.GetData( Tobjpx->TrcV.AxisPtr );
		}

		/* �r�b�g�g���[�X */
		if( Tobjpx->TrcB.GetData != NULL )
		{
			/* Set Bit Trace Variable */
			Tobjpx->TrcB.BtvData = Tobjpx->TrcB.GetData( Tobjpx->TrcB.AxisPtr );
		}
		else
		{
			Tobjpx->TrcB.BtvData = FALSE;		
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		Reset Average Variable Function																*/
/*																									*/
/****************************************************************************************************/
void	DtrcResetAverageVar( TRCHNDL *TrcHndl )
{
	LONG i;

	TRCREG	*TrcReg;
	TRCOBJ	*Tobjpx;

	TrcReg  = &TrcHndl->TrcReg;

	for(i=0;i<TrcReg->TrcMode;i++)
	{
		Tobjpx = &TrcHndl->TrcExeP.TrcObj[i];
		Tobjpx->TrcV.VxData = 0;
	}
}
/****************************************************************************************************/
/*																									*/
/*		Get Trace Data (Bit Data Merged)															*/
/*																									*/
/****************************************************************************************************/
static LONG	dtrcGetMergeData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx )
{
	LONG	Data;

	if( Tobjpx->TrcExeflg == 0x01 )
	{
#if (FLOAT_USE==TRUE)
		Data = Tobjpx->TrcV.VxData * Tobjpx->TrcV.VxGain;
#else
		Data = MlibMulgainNolim( Tobjpx->TrcV.VxData , Tobjpx->TrcV.VxGain );
#endif /*FLOAT_USE==TRUE*/
		Data = ( Data << Tobjpx->TrcV.VxSftL) >> Tobjpx->TrcV.VxSftR;
		Data = MlibLIMITUL( Data, TrcExeP->UpperLmt, TrcExeP->LowerLmt );

		if( Tobjpx->TrcB.GetData != NULL )
		{
			if( Tobjpx->TrcB.BtvData )
			{
				Data = (Data | TrcExeP->TrcBitSet);
			}
			else
			{
				Data = (Data & TrcExeP->TrcBitClr);
			}
		}
		return Data;
	}
	return 0;
}

/****************************************************************************************************/
/*																									*/
/*		Get Each Trace Data ( Veriable Data and Bit Data )											*/
/*																									*/
/****************************************************************************************************/
static void	dtrcGetTrcData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx, LONG *VerData, LONG *BitData, LONG Idx )
{
	LONG	Data;
	ULONG	TrcBitSet, TrcBitClr;

	if( Tobjpx->TrcExeflg == 0x01 )
	{
#if (FLOAT_USE==TRUE)
		Data = Tobjpx->TrcV.VxData * Tobjpx->TrcV.VxGain;
#else
		Data = MlibMulgainNolim( Tobjpx->TrcV.VxData , Tobjpx->TrcV.VxGain );
#endif /* FLOAT_USE */
		Data = ( Data << Tobjpx->TrcV.VxSftL) >> Tobjpx->TrcV.VxSftR;
		*VerData = MlibLIMITUL( Data, TrcExeP->UpperLmt, TrcExeP->LowerLmt );

		TrcBitSet = (0x00000001 << Idx );
		TrcBitClr = ~TrcBitSet;

		if( Tobjpx->TrcB.BtvData )
		{
			*BitData = (*BitData | TrcBitSet);
		}
		else
		{
			*BitData = (*BitData & TrcBitClr);
		}
	}
	else
	{
		*VerData = 0;
		*BitData = 0;
	}
}

/****************************************************************************************************/
/*																									*/
/*		Data Trace Procedure																		*/
/*																									*/
/****************************************************************************************************/
void	DtrcProcedure( TRCHNDL *TrcHndl, LONG Idx )
{

	ULONG i;
	LONG VerData, BitData;
	SHORT x,y;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCEXEV	*TrcExeV;
	TRCOBJ	*Tobjpx;
	TRCOBJ	*TobjpxB;

	BitData = 0;				/* ���������ϐ��C��<S191> */

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcExeV = &TrcHndl->TrcExeV;

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���[�g�f�[�^�g���[�X																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcExeP->TrcDataType == DATATRACE_SHORT )	
	{
		if( TrcExeP->TrcIOType == IODATA_MERGE )						/* ��ʃr�b�g��IO�f�[�^�i�[	*/
		{
			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		
				VerData = dtrcGetMergeData( TrcExeP, Tobjpx );	
				((SHORT*)Tobjpx->TrcBuf)[Idx] = (SHORT)VerData;
			}
		}
		else															/* ��pIO�f�[�^�̈�Ɋi�[	*/
		{
			TobjpxB = &TrcExeP->TrcObj[TrcReg->TrcMode];		

			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		

				dtrcGetTrcData( TrcExeP, Tobjpx, &VerData, &BitData, i );
				((SHORT*)Tobjpx->TrcBuf)[Idx]  = (SHORT)(VerData); 
				((SHORT*)TobjpxB->TrcBuf)[Idx] = (SHORT)(BitData);
			}
		}
	
		if(TrcExeP->TrcTrg[0].TrgBufx != NULL)
		{
			TrcExeV->TrgVar[0] = ((SHORT*)TrcExeP->TrcTrg[0].TrgBufx)[Idx];	/* Update Trigger1 Data	*/	
		}
		if(TrcExeP->TrcTrg[1].TrgBufx != NULL)
		{
			TrcExeV->MTrgVar[0] = ((SHORT*)TrcExeP->TrcTrg[1].TrgBufx)[Idx];/* Update Trigger2 Data	*/
		}

	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�����O�f�[�^�g���[�X																	*/
	/*----------------------------------------------------------------------------------------------*/
	else											
	{
		if( TrcExeP->TrcIOType == IODATA_MERGE )						/* ��ʃr�b�g��IO�f�[�^�i�[	*/
		{
			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		
				VerData = dtrcGetMergeData( TrcExeP, Tobjpx );	
				((SHORT *)Tobjpx->TrcBuf)[2*Idx    ] = (SHORT)(VerData);	
				((SHORT *)Tobjpx->TrcBuf)[2*Idx + 1] = (SHORT)(VerData >> 16);	
			}
		}
		else															/* ��pIO�f�[�^�̈�Ɋi�[	*/
		{
			TobjpxB = &TrcExeP->TrcObj[TrcReg->TrcMode];		

			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		

				dtrcGetTrcData( TrcExeP, Tobjpx, &VerData, &BitData, i );
				((SHORT*)Tobjpx->TrcBuf)[2*Idx    ] = (SHORT)(VerData);	
				((SHORT*)Tobjpx->TrcBuf)[2*Idx + 1] = (SHORT)(VerData >> 16);	

				((SHORT*)TobjpxB->TrcBuf)[2*Idx    ] = (SHORT)(BitData);	
				((SHORT*)TobjpxB->TrcBuf)[2*Idx + 1] = (SHORT)(BitData >> 16);	
			}
		}
		
		if(TrcExeP->TrcTrg[0].TrgBufx != NULL)
		{
			x = ((SHORT*)TrcExeP->TrcTrg[0].TrgBufx)[2*Idx];					
			y = ((SHORT*)TrcExeP->TrcTrg[0].TrgBufx)[2*Idx +1];
			TrcExeV->TrgVar[0] = (y << 16) + x;							/* Update Trigger1 Data		*/
		}
		if(TrcExeP->TrcTrg[1].TrgBufx != NULL)
		{
			x = ((SHORT*)TrcExeP->TrcTrg[1].TrgBufx)[2*Idx];					
			y = ((SHORT*)TrcExeP->TrcTrg[1].TrgBufx)[2*Idx+1];			
			TrcExeV->MTrgVar[0] = (y << 16) + x;						/* Update Trigger2 Data		*/
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		�f�[�^�g���[�X�g���K�`�F�b�N����															*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcCheckTriggerProcedure( TRCHNDL *TrcHndl )
{
	LONG	x,y;
	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCEXEV *TrcExeV;

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcExeV = &TrcHndl->TrcExeV;

	/*----------------------------------------------------------------------------------------------*/
	/*		�g���K�`�F�b�N�P																		*/
	/*----------------------------------------------------------------------------------------------*/
#if CSW_NET_TRG_USE == TRUE		/* <S021> */
	if(((TrcExeP->TrcTrg[0].TrgSel >> 4) & 0x000F) == NET_TRG_SEL)
	{ /* �l�b�g���[�N�g���K */
		x = TrcExeV->NetTrigger;
	}
	else if((TrcExeP->TrcTrg[0].TrgSel == 0x00))
#else
	if((TrcExeP->TrcTrg[0].TrgSel == 0x00))
#endif
	{
		x = TRUE;
		if( TrcReg->MTrgMode == 1 )	/* <S148> */
		{
			x = TRUE;										/* AND�g���K							*/
		}
		else if( TrcReg->MTrgMode == 2 )
		{
			x = FALSE;										/* OR�g���K								*/
		}
	}
	else
	{
		x = dtrcCheckTrigger( SINGLE_TRG, TrcHndl, TrcExeV->TrgVar);
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�g���K�`�F�b�N�Q																		*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcExeP->TrcTrg[1].TrgSel == 0x00)
	{
		y = TRUE;
		if( TrcReg->MTrgMode == 1 )	/* <S148> */
		{
			y = TRUE;										/* AND�g���K							*/
		}
		else if( TrcReg->MTrgMode == 2 )
		{
			y = FALSE;										/* OR�g���K								*/
		}
	}
	else
	{
		y = dtrcCheckTrigger( MULTI_TRG, TrcHndl, TrcExeV->MTrgVar);
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�}���`�g���K�`�F�b�N																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcReg->MTrgMode == 1)								/* AND�g���K							*/
	{
		return( x & y );
	}
	/*----------------------------------------------------------------------------------------------*/
	else if(TrcReg->MTrgMode == 2)							/* OR�g���K								*/
	{
		return( x | y );
	}
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* <S021> */
#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	else if(TrcReg->MTrgMode == NET_TRG_SEL)				/* �l�b�g���[�N�g���K					*/
	{
		return( TrcExeV->NetTrigger );
	}
#endif
#endif
	else													/* �P��g���K							*/
	{
		return( x );
	}
}


/****************************************************************************************************/
/*																									*/
/*		�g���K�`�F�b�N����																			*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcCheckTrigger( LONG TrgNo, TRCHNDL *TrcHndl, LONG *TrgVar )
{
	LONG	Data0;
	LONG	Data1;
//	ULONG	x;				/* <S0D2> */
	ULONG	TrgData;		/* <S0D2> */
	ULONG	TrgEdge;		/* <S0D2> */
	ULONG	TrgTarget;		/* <S0D2> */


	TRCREG	*TrcReg;
	TRCTRG	*TrcTrg;

	TrcReg  = &TrcHndl->TrcReg;
	TrcTrg  = TrcHndl->TrcExeP.TrcTrg;
	
#if 0	/* <S142> */
//	x = (TrcReg->TrgSel >> 4) & 0x0000000F;	/* �g���K�ΏۑI�� */
	TrgData = (TrcReg->TrgSel) & 0x0000000F;			/* �g���K�f�[�^�I�� */	/* <S0D2> */
	TrgEdge = (TrcReg->TrgSel >> 4 ) & 0x0000000F;		/* �g���K�����I�� */	/* <S0D2> */
	TrgTarget = (TrcReg->TrgSel >> 12) & 0x0000000F;	/* �g���K�ΏۑI�� */	/* <S0D2> */
#else
	TrgData   = (TrcTrg[TrgNo].TrgSel      ) & 0x0000000F;	/* �g���K�f�[�^�I�� */
	TrgEdge   = (TrcTrg[TrgNo].TrgSel >> 4 ) & 0x0000000F;	/* �g���K�����I�� */
	TrgTarget = (TrcTrg[TrgNo].TrgSel >> 12) & 0x0000000F;	/* �g���K�ΏۑI�� */
#endif

	/* <S0D2>  Start*/
#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	/*----------------------------------------------------------------------------------------------*/
	/*		�l�b�g���[�N�g���K�`�F�b�N(�`�F�b�N���ڂȂ�)											*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrgEdge == NET_TRG_SEL )
	{
		return( TRUE );
	}
#endif /* CSW_NET_TRG_USE */
	/* <S0D2> End */

	/*----------------------------------------------------------------------------------------------*/
	/*		�r�b�g�g���K�`�F�b�N																	*/
	/*----------------------------------------------------------------------------------------------*/
//	if( (0 < x)&&(x < TrcReg->TrcMode) )	/* <S0D2> */
	if( ( TrgData == 0x0009) || ((TrgData == 0x000f)&&(TrgTarget == 0x0001) ) ) 	/* <S0D2> */
	{
		Data0 = (TrgVar[0] ^ TrgVar[1]) & TrcTrg[TrgNo].TrgBitSetMsk;
	/*----------------------------------------------------------------------------------------------*/
		if( Data0 == 0 )									/* �r�b�g�ω��Ȃ�						*/
		{
			return( FALSE );
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x01 )					/* ���オ��G�b�W�g���K					*/
		{
			if( TrgVar[0] & TrcTrg[TrgNo].TrgBitSetMsk )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x02 )					/* ��������G�b�W�g���K					*/
		{
			if( TrgVar[1] & TrcTrg[TrgNo].TrgBitSetMsk )
			{
				return( TRUE );
			}
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		���l���x���g���K�`�F�b�N																*/
	/*----------------------------------------------------------------------------------------------*/
	//else if(x == 0)	/* <S0D2> */
	else if( (( TrgData > 0 ) && (TrgData <= 8)) || ( (( TrgData == 0x000f ) && (TrgTarget == 0x0000)) ) ) 	/* <S0D2> */
	{
		Data0 = (LONG)(TrgVar[0] << TrcTrg[TrgNo].TrgVsft) >> TrcTrg[TrgNo].TrgVsft;
		Data1 = (LONG)(TrgVar[1] << TrcTrg[TrgNo].TrgVsft) >> TrcTrg[TrgNo].TrgVsft;
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge == 0x04 )					/* ���x���g���K(�ȏ�)					*/
		{
			if( Data0 >= TrcTrg[TrgNo].TrgLevel )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge == 0x05 )					/* ���x���g���K(�ȉ�)					*/
		{
			if( Data0 <= TrcTrg[TrgNo].TrgLevel )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/* �ω��G�b�W�������ď������邽�߁Abit0,1�Ƃ�AND�����Ƃ���(���x���g���K���f��O�Ɏ����Ă���)	*/
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x01 )					/* ���オ��G�b�W�g���K					*/
		{
			if( (Data1 < TrcTrg[TrgNo].TrgLevel) && (Data0 >= TrcTrg[TrgNo].TrgLevel) )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x02 )					/* ��������G�b�W�g���K					*/
		{
			if( (Data1 > TrcTrg[TrgNo].TrgLevel) && (Data0 <= TrcTrg[TrgNo].TrgLevel) )
			{
				return( TRUE );
			}
		}
	}

#if 0	/* <S0D2> Delete Start */
#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	/*----------------------------------------------------------------------------------------------*/
	/*		�l�b�g���[�N�g���K�`�F�b�N(�`�F�b�N���ڂȂ�)											*/
	/*----------------------------------------------------------------------------------------------*/
	else if( x == NET_TRG_SEL )
	{
		return( TRUE );
	}
#endif /* CSW_NET_TRG_USE */
#endif	/* <S0D2> Delete End */

	/*----------------------------------------------------------------------------------------------*/
		return( FALSE );
}

/****************************************************************************************************/
/*																									*/
/*		�g���[�X�T���v�����O�Ԋu�擾�pAPI															*/
/*																									*/
/****************************************************************************************************/
ULONG	DtrcGetSampIntv( TRCHNDL *TrcHndl )
{
	return	(TrcHndl->TrcExeP.DataSampIntv);
}


/****************************************************************************************************/
/*																									*/
/*		�g���[�X�T���v�����O�Ԋu�擾�pAPI															*/
/*																									*/
/****************************************************************************************************/
ULONG	DtrcGetTrcDataNum( TRCHNDL *TrcHndl )
{
	return	(TrcHndl->TrcExeP.TrcDnum);
}


/****************************************************************************************************/
/*																									*/
/*		�g���[�X�g���K�ʒu��ݒ�pAPI																*/
/*																									*/
/****************************************************************************************************/
void	DtrcSetTrgPointToZero( TRCHNDL *TrcHndl )
{
	TrcHndl->TrcReg.TrgPoint = 0;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*		�g���[�X�T���v�����O�����ŏ��l���Z								<V508>						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*					Egear.b * Hprm.Kmotspd  * 1000000												*/
/*		SampLlim = -----------------------------------												*/
/*					Egear.a * KPI_SBCYCLEUS *  2^23													*/
/*																									*/
/****************************************************************************************************/
LONG	dtrcCalcSetSampLlim( void* Axis, LONG Axno )
{
	LONG	kx,sx,plsno;
	LONG	Egear_a,Egear_b;
	BOOL	EgearDisable;
	AXIS_HANDLE *AxHdl;

	AxHdl =  ((AXIS_HANDLE*)Axis + Axno);
	EgearDisable = (((AXIS_HANDLE*)Axis + Axno)->BaseCtrlData->TblDrive.OpeReg.TblFuncSel & TBLFUNCBIT_EGEAR);

	/* ����t���N���Ή�	*/	
	//if( Kprm.f.FencUse )
	//{
	//	plsno = Hprm.KmotspdFC;
	//}
	//else
	//{
		plsno = AxHdl->prm_hndl_ptr->hprm_ptr->Kmotspd;
	//}

	if( EgearDisable == FALSE )
	{
		Egear_a = AxHdl->BaseCtrlData->PosManager.conf.Egear.a;
		Egear_b = AxHdl->BaseCtrlData->PosManager.conf.Egear.b;
		kx = MlibScalKskxkx( plsno, Egear_b, Egear_a, &sx,  0 );
	}
	else
	{ /* �d�q�M�A���� */
		kx = MlibScalKskxkx( plsno, 1, 1, &sx,  0 );
	}
	kx = MlibPcalKxgain( kx, 1000000, 0x7FFFFF, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, KPI_SBCYCLEUS, &sx, 0 );
	kx = MlibPcalKxaddx( kx, sx, 4194304, 23, &sx );		// kx = kx + 0.5
	kx = MlibPcalKxgain( kx, 1, 1, &sx, -24 );
	return( kx );
}
#endif



void	DtrcSetNetworkTrigger( TRCHNDL *TrcHndl, BOOL Trigger )		/* <S012> */
{
	TrcHndl->TrcExeV.NetTrigger = Trigger;
	return;
}

/* <S0C9>	Start */
/****************************************************************************************************/
/*																									*/
/*		�g���[�X�Ώۃf�[�^�I���AI/O�g���[�X�Ώۃf�[�^�I���̏����l�ݒ�								*/
/*																									*/
/****************************************************************************************************/
void	DtrcSelSetInitValue( TRCHNDL *TrcHndl )
{
	LONG	i;
	TRCREG	*TrcReg;
	TRCINDIVPRM	*TrcIncivPrm;

	TrcReg = &TrcHndl->TrcReg;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;


	/* �f�[�^�I/O�g���[�X1�`3�Ώۃf�[�^�I���@�����l�ݒ� */
	TrcReg->DataIOTrcSel[0] = 0x0101;
	TrcReg->DataIOTrcSel[1] = 0x0204;
	TrcReg->DataIOTrcSel[2] = 0x8105;

	/* �g���[�X1�`3�Ώۃf�[�^�I���@�����l�ݒ� */
	TrcReg->TrcDataSel[0] = 0x0001;
	TrcReg->TrcDataSel[1] = 0x0002;
	TrcReg->TrcDataSel[2] = 0x0003;

	/* I/O�g���[�X1�`3�Ώۃf�[�^�I���@�����l�ݒ� */
	TrcReg->IOTrcDataSel[0] = 0x0001;
	TrcReg->IOTrcDataSel[1] = 0x0002;
	TrcReg->IOTrcDataSel[2] = 0x0081;


}
/* <S0C9>	End */

/***************************************** end of file **********************************************/
