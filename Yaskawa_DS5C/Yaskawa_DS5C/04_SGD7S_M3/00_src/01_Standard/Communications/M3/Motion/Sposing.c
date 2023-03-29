/****************************************************************************
Description	: SPOSING Motion Generator
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "MtGenerator.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"



/*------------------------------------------------------------------------------------------*/
/*		SPOSING functions																	*/
/*------------------------------------------------------------------------------------------*/
//static void mtgSpSetPrm( MG_SPOSING* sposing, MG_PRM* prm, LONG TargetSpd );
static void mtgSpCalAccPrm( MG_SPOSING* sposing, LONG CurrentSpd );
//static BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
//								LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 );
static LONG mtgSpAccelerator( MG_SPOSING* sposing, LONG CurrentSpd );
static LONG mtgSpDecelerator( MG_SPOSING* sposing, LONG CurrentSpd, LONG *Segment );
static LONG mtgSpCheckDecelaration(MG_S_POSWK* wk, MC_BASE_PRM* bPrm);
static LONG mtgSpCalSpdRemDist( MG_SPOSING* sposing, LONG Td, LONG* work );


/*********************************/	
//#ifdef	__ghs__
//#pragma ghs section bss=".fixed_buffer"
//#endif

//static ULONG debug_xx[1000][2];
//static ULONG i;

//#ifdef	__ghs__
//#pragma ghs section bss=default
//#endif
/*********************************/	

/****************************************************************************
 SHOLD���[�V��������
****************************************************************************/
BOOL MtgShold( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_PRM *prm;
	BOOL	rc;

	prm = hdl->prm;

	if( inOut->Ctrl == MG_HOLD_CTRL2 )
	{
		hdl->prm = &hdl->basePrm->StopPrm;
	}
	
	rc = MtgSposing( hdl, inOut );

	hdl->prm = prm;

	return rc;
}


/*********************************************************************************************
 SPOSING���[�V��������
*********************************************************************************************/
BOOL MtgSposing( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_S_POSWK wk;
	MG_SPOSING *sposing;
	KSGAIN	tmp;
	LONG 	buf;
	LONG 	ChkDecDistance;
	BOOL 	ChkEnd;				/* SPOSING�I��OK�FTRUE�ASPOSING�I�����Ȃ�:FALSE				*/
	BOOL 	ChkOverflow;		/* ���������I�[�o�t���[:TRUE�A�I�[�o�t���[�Ȃ�:FALSE		*/
	BOOL	ChkReverse;			/* �ڕW�ʒu����]�����Ɠ�������:TRUE�A��]�����Ƌt:FALSE	*/

	sposing = &hdl->sposing;
	ChkEnd = TRUE;
//	ChkOverflow = FALSE;

	hdl->dposRem    = 0;
	inOut->VrefOut  = 0;
	inOut->TrefOut  = 0;
	hdl->VrefSum[0] = 0;
	hdl->VrefSum[1] = 0;

#if (FLOAT_USE==TRUE)
	tmp = (float)inOut->Tspd * hdl->basePrm->PosingSpdKx;
#else
	tmp = MlibMulgain32( inOut->Tspd, hdl->basePrm->PosingSpdKx );
#endif /* FLOAT_USE */
	if( (ULONG)tmp >= 0x40000000UL )
	{
		wk.TargetSpd = 0x3FFFFFFF;
	}
	else
	{
		wk.TargetSpd = (LONG)tmp;
	}

	wk.CurrentSpd  = hdl->Cspd;				/* �w�ߑ��x���R�s�|***.*[ref/cyc]				*/
	wk.LastSpd     = wk.CurrentSpd;			/* �w�ߑ��x��ۊ�								*/
	wk.Segment     = wk.CurrentSpd;			/* �w�ߑ��x(dpos)��ۊ�							*/
	wk.ErrorDistance = inOut->DposIn;		/* �c�����v�Z�i�����t���j						*/
	/*--------------------------------------------------------------------------------------*/
	/* ��]�����`�F�b�N																		*/
	/*--------------------------------------------------------------------------------------*/
	/* ���݂̎w�߈ʒu����ڕW�ʒu�̕��������� */
	hdl->tMinusDir = FALSE;					/* +���� 										*/
	if( wk.ErrorDistance < 0 )
	{
		hdl->tMinusDir   = TRUE;			/* -���� 										*/
		wk.ErrorDistance = -inOut->DposIn;	/* �c����										*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* SPOSING�J�n�E�I���`�F�b�N															*/
	/*--------------------------------------------------------------------------------------*/
	if( wk.CurrentSpd == 0 )
	{	/* �w�ߑ��x=0�Ȃ�, SPOSING������Ԃփ��Z�b�g */
		hdl->cMinusDir = hdl->tMinusDir;
		hdl->dpos0     = 0;					/* DPOS�̏�����(�w�ߑ��x(������)�̐ώZ)			*/
		inOut->DposOut = 0;					/* �o�͈ړ���[ref](Vel->Pos)					*/

		/* �c����=0�ŕ����o������ */
		if( wk.ErrorDistance == 0 )
		{
			/* �����o������ */
			sposing->State = SPOSING_INI_MODE;
			hdl->Cspd = 0;
			return TRUE;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* �t��]�`�F�b�N																		*/
	/*--------------------------------------------------------------------------------------*/
	/* �ڕW�ʒu����]�����Ɠ��������FTRUE�A��]�����Ƌt:FALSE 								*/
	ChkReverse = !(hdl->tMinusDir ^ hdl->cMinusDir);
	/*--------------------------------------------------------------------------------------*/
	/* �O��]��␳ 																		*/
	/*--------------------------------------------------------------------------------------*/
	if( hdl->dpos0 != 0 )
	{
		/* dpos�̏�������0�łȂ������ꍇ�́A�]�肪�����Ŏc���Ă���̂ŗ\��-1���Ă��� */
		wk.ErrorDistance = wk.ErrorDistance - 1;
	}
	wk.ErrorDistance0 = (0 - hdl->dpos0) & VELREM_BIT_MSK;

	/*--------------------------------------------------------------------------------------*/
	/*			SPOSING ���[�h�I��															*/
	/*--------------------------------------------------------------------------------------*/
	if( (sposing->State != SPOSING_DEC_MODE1) && (sposing->State != SPOSING_DEC_MODE2) )
	{
		/* �ڕW���x�A�����x�A�����x�AS���䗦������ */
		mtgSpSetPrm( sposing, hdl->prm, wk.TargetSpd );

		/* ���������v�Z */
		ChkOverflow = mtgSpCalDecPrm(sposing, hdl->basePrm, FALSE,
								wk.CurrentSpd, &wk.StopDistance, &wk.StopDistance0);

		/* �������苗�� = �c���� - �������� - �w�ߑ��x */
		ChkDecDistance = mtgSpCheckDecelaration(&wk, hdl->basePrm);

		/* �ڕW�ʒu���t�����Hor ���������I�[�o�H �� ���� */
		if( (ChkReverse == FALSE) || (ChkOverflow != FALSE) )
		{
			ChkEnd = FALSE; 									/* SPOSING�I�����Ȃ�		*/
			sposing->StopDistRem = 0;							/* �c�����[���͕��������Ȃ�	*/
			sposing->State = SPOSING_DEC_MODE2;					/* ������ �������[�h2��		*/
		}
		/* ���������[�h�̂Ƃ� */
		else if( sposing->State == SPOSING_INI_MODE )
		{
			/* �ڕW���x �� ���ݑ��x�H */
			if( (sposing->TargetSpd >= wk.CurrentSpd) || ChkDecDistance < 0)
			{
				/* �����p�����[�^�v�Z����	*/
				mtgSpCalAccPrm( sposing, wk.CurrentSpd );	
				sposing->State = SPOSING_ACC_MODE;				/* ���� �葬���[�h��		*/
			}
			else
			{	/* ���������v�Z�A�����p�����[�^�v�Z����		*/
				mtgSpCalDecPrm( sposing, hdl->basePrm, ChkReverse,
										wk.CurrentSpd, &wk.StopDistance, &wk.ErrorDistance0);
				sposing->State = SPOSING_DEC_MODE1;				/*�������[�h1��(�ڕW���x�ύX)*/
			}
		}
		else
		{
			/* �ʏ폈��(���[�h�ύX�Ȃ�) */
		}
	}

	/*--------------------------------------------------------------------------------------*/
	/*		SPOSING �w�ߐ�������															*/
	/*--------------------------------------------------------------------------------------*/
	switch( sposing->State )
	{	/*----------------------------------------------------------------------------------*/
		/*	�������葬���[�h																*/
		/*----------------------------------------------------------------------------------*/
		case SPOSING_ACC_MODE :

			/* �c���������������H �� ���� */
			if( ChkDecDistance < 0 )
			{
				/* �c�����[�����v�Z */
				sposing->StopDistRem = ChkDecDistance + wk.CurrentSpd;

				/* �c�����[��(����)���v�Z */
				if( sposing->TmpDecVal != 0 )
				{
					sposing->jStopDistRem = ((ULONG)sposing->StopDistRem / (ULONG)sposing->TmpDecVal);
					sposing->rStopDistRem = sposing->StopDistRem 
									- ( sposing->jStopDistRem * sposing->TmpDecVal);
					sposing->rdStopDistRem = (sposing->rStopDistRem / sposing->DecCnt) + 1;
				}
				else
				{
					sposing->jStopDistRem = sposing->StopDistRem;
				}

				if( sposing->StopDistRem < 0 )
				{/* �s���߂� */
					sposing->StopDistRem = 0;					/* �c�����[���͕��������Ȃ�	*/
					ChkEnd = FALSE;								/* SPOSING�I�����Ȃ�		*/
				}
				sposing->State = SPOSING_DEC_MODE2;				/* �������������[�h2��		*/
				wk.Segment = wk.CurrentSpd;						/* �Z�O�����g�����o��		*/
			}
			/* �c���������������H */
			else
			{
				/* ���ݑ��x < �ڕW���x�ɓ��B���Ă��Ȃ� */
				if( wk.CurrentSpd < sposing->TargetSpd )
				{
					/* ��������		*/
					wk.CurrentSpd = mtgSpAccelerator( sposing, wk.CurrentSpd );
					/* �c�����v�Z	*/
					ChkOverflow   = mtgSpCalDecPrm( sposing, hdl->basePrm, ChkReverse, 
													wk.CurrentSpd, &wk.StopDistance, &wk.StopDistance0 );

					/* �������苗�� = �c���� - �������� - �w�ߑ��x */
					ChkDecDistance = mtgSpCheckDecelaration(&wk, hdl->basePrm);

					/* �����萔�ŉ����ł���H */
					if((ChkDecDistance < 0) || (ChkOverflow != 0))
					{
						/* �����萔�ŉ����ł��Ȃ��ꍇ	*/
						/* �O�񑬓x��0���H				*/
						if(wk.LastSpd != 0)
						{	/* �O��̑��x�ɂ��� */
							wk.Segment = wk.CurrentSpd = wk.LastSpd;
						}
						else
						{/* �O�񑬓x=0�Ȃ琔�p���X���� */
							/* �~�j�}���萔�ŉ��� */
							if( sposing->InitDec >= sposing->InitAcc )
							{
								wk.CurrentSpd = wk.LastSpd + sposing->InitAcc;
							}
							else
							{
								wk.CurrentSpd = wk.LastSpd + sposing->InitDec;
							}
							buf = wk.ErrorDistance << hdl->basePrm->vel_shift;
							if( wk.CurrentSpd >= buf )
							{
								/* �c�����𑬓x�ɂ��� */
								wk.Segment = wk.CurrentSpd = buf;
							}
							else
							{
								/* �~�j�}�������x�ŉ����������x�ɂ��� */
								wk.Segment = wk.CurrentSpd;
							}
						}
					}
					else
					{	/* �����萔�ŉ����ł���ꍇ�͉��� */
						wk.Segment = wk.CurrentSpd;
						if( wk.CurrentSpd == wk.LastSpd )
						{
							sposing->StopDistRem = ( ChkDecDistance << hdl->basePrm->vel_shift );
							if( hdl->sposing.StopDistRem  > wk.LastSpd )
							{
								wk.Segment = sposing->StopDistRem;
							}
						}

					}
				}
				else
				{	/* ���ݑ��x = �ڕW���x�ɓ��B���Ă���ꍇ�͑��x�ێ� */
					wk.Segment = wk.CurrentSpd;
				}
			}
			break;
		/*----------------------------------------------------------------------------------*/
		/*	�������[�h1(�ڕW���x�ύX)														*/
		/*----------------------------------------------------------------------------------*/
		case SPOSING_DEC_MODE1 : 							/* �ڕW���x < ���ݑ��x 			*/
			sposing->StopDistRem = 0;						/* �c�����[���͕����o���Ȃ�		*/
			/* ��������	*/
			wk.CurrentSpd = mtgSpDecelerator( sposing, wk.CurrentSpd, &wk.Segment );

			if( wk.Segment == sposing->TargetSpd )
			{	/*--------------------------------------------------------------------------*/
				/* �ڕW���x���B�ŁA�������������[�h��										*/
				/*--------------------------------------------------------------------------*/
				mtgSpSetPrm( sposing, hdl->prm, wk.TargetSpd );	/* ������ 					*/
				/* �����p�����[�^�v�Z����	*/
				mtgSpCalAccPrm( sposing, wk.CurrentSpd );
				sposing->State = SPOSING_ACC_MODE;				/* �������������[�h�� 		*/
			}
			ChkEnd = FALSE;										/* SPOSING�I�����Ȃ�		*/
			break;
		/*----------------------------------------------------------------------------------*/
		/*	�������[�h2(�ʒu����)															*/
		/*----------------------------------------------------------------------------------*/
		case SPOSING_DEC_MODE2 : 								/* ���ݑ��x��0(0 < Vt <= V)	*/
			if( (wk.CurrentSpd == 0) && (wk.ErrorDistance != 0) )
			{
				//if(wk.ErrorDistance != 1)
				//{
					//int a=0;
				//}
				/*--------------------------------------------------------------------------*/
				/* ���ݑ��x=0�ł��c��������������ĉ���(�������������[�h��) 				*/
				/*--------------------------------------------------------------------------*/
				mtgSpSetPrm( sposing, hdl->prm, wk.TargetSpd );	/* ������					*/
				/* �����p�����[�^�v�Z����	*/
				mtgSpCalAccPrm( sposing, wk.CurrentSpd );	
				sposing->State = SPOSING_ACC_MODE;				/* �������������[�h��		*/
//				ChkEnd = FALSE;									/* SPOSING�I�����Ȃ�		*/
			}

			/* ��������	*/
			wk.CurrentSpd = mtgSpDecelerator( sposing, wk.CurrentSpd, &wk.Segment );
			break;
		default:
			 break;
	}
	/*--------------------------------------------------------------------------------------*/
	/* �w�ߑ��x���i�[																		*/
	/*--------------------------------------------------------------------------------------*/
	hdl->Cspd = wk.CurrentSpd;
	/*--------------------------------------------------------------------------------------*/
	/* ��������ώZ																			*/
	/*--------------------------------------------------------------------------------------*/
	buf = hdl->dpos0 + ( (wk.Segment << hdl->basePrm->vel_shift_s) & VELREM_BIT_MSK );
	hdl->dpos0 = buf & VELREM_BIT_MSK;
	if( buf > VELREM_BIT_MSK )
	{
		buf = ( wk.Segment >> hdl->basePrm->vel_shift ) + 1;
	}
	else
	{
		buf = ( wk.Segment >> hdl->basePrm->vel_shift );
	}
	/*--------------------------------------------------------------------------------------*/
	/* �����o�������ɂ�镄���g�����ĕ����o��												*/
	/*--------------------------------------------------------------------------------------*/
	if( (inOut->DposIn == 1) || (inOut->DposIn == -1) )
	{/* �����I������(�c����1�̂Ƃ��͋��������1�𕥂��o��) */
		inOut->DposOut = inOut->DposIn;
	}
	else if( hdl->cMinusDir )
	{/* �����g�� */
		inOut->DposOut = (~buf) + 1;
	}
	else
	{
		inOut->DposOut = buf;
	}
	/* ���j�^�p(for debug) */
	sposing->AccMonitor = wk.Segment - sposing->LastSegment;
	sposing->LastSegment = wk.Segment;
	/*--------------------------------------------------------------------------------------*/
	/* �I������																				*/
	/*--------------------------------------------------------------------------------------*/
	if((inOut->DposOut == inOut->DposIn) && (ChkEnd == TRUE) )
//	if( (inOut->DposOut == inOut->DposIn) && (ChkEnd == TRUE) && (hdl->Cspd == 0) )
	{
		sposing->State = SPOSING_INI_MODE;
		hdl->Cspd = 0;
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************************************
 SPOSING �������AS���p�����[�^�Z�b�g

	���x�A�ʒu���ύX���ꂽ�Ƃ��ɑ��x�������A�������������AS�����Ԕ䗦���������s���B
*********************************************************************************************/
void mtgSpSetPrm(MG_SPOSING* sposing, MG_PRM* prm, LONG TargetSpd)
{
	sposing->TargetSpd = TargetSpd;							/* �ڕW���x������				*/
	sposing->InitAcc = prm->accrate2;						/* �����x������					*/
	sposing->InitDec = prm->decrate2;						/* �����x������					*/
	sposing->CurrentSratio = prm->Sratio;					/* S�����Ԕ䗦������			*/
	
	/* S���������䗦LIMIT  */
	if( prm->Sratio <= S_RATIO_MIN )
	{
		sposing->CurrentSratio = S_RATIO_MIN;
	}
	if( prm->Sratio >= S_RATIO_MAX )
	{
		sposing->CurrentSratio = S_RATIO_MAX;
	}

}

/*********************************************************************************************
	SPOSING �����p�����[�^�v�Z����

	SPOSING�����ɕK�v�ȓ����p�����[�^�̐ݒ�B
	S���������� = 0�AJerk < 1�ƂȂ�ꍇ�ɂ́APOSING�Ƃ���(S��������Ȃ�����)
*********************************************************************************************/
static void mtgSpCalAccPrm( MG_SPOSING* sposing, LONG CurrentSpd )
{
	LONG work;
	LONG Ta,Taj;
	LONG spd;
	LONG xx[2] = {0};

	/*--------------------------------------------------------------------------------------*/
	/* �ڕW���x�̐ݒ�																		*/
	/*--------------------------------------------------------------------------------------*/
	sposing->InitSpdAcc = CurrentSpd;							/* SPOSING�J�n�����x		*/
	spd = sposing->TargetSpd - CurrentSpd;						/* SPOSING�ڕW���x(��)		*/
	if( sposing->InitAcc == 0 )
	{	
		sposing->InitAcc = 1;
	}
	/*--------------------------------------------------------------------------------------*/
	/* �����񐔎Z�o																			*/
	/*--------------------------------------------------------------------------------------*/
	Ta  = (LONG)( spd / sposing->InitAcc );						/* �����񐔌v�Z �������� 	*/
	Taj = (LONG)( (Ta * sposing->CurrentSratio) / 100 );		/* S������������		 	*/
	MlibMul3232( Taj, (Ta - Taj), xx );
	/*--------------------------------------------------------------------------------------*/
	/* Jerk(����)�A�����؂�ւ����x�̎Z�o													*/
	/*--------------------------------------------------------------------------------------*/
	if( (Taj != 0) && (xx[1] == 0) )
	{
		sposing->dJerkAcc = (LONG)((ULONG)spd / (ULONG)xx[0]);
	}
	else
	{
		sposing->dJerkDec = 0;
	}
//	if( (Taj != 0) && (sposing->dJerkAcc != 0) )
	if( sposing->dJerkAcc != 0 )
	{	/*----------------------------------------------------------------------------------*/
		/*	SPOSING�̂Ƃ�																	*/
		/*----------------------------------------------------------------------------------*/
		sposing->JerkAccRate = sposing->dJerkAcc * Taj;			/* SPOSING�ł̉����x�̍ő�l*/
		work = (sposing->JerkAccRate * Taj) >> 1;
		sposing->SpdRemAcc = spd - sposing->JerkAccRate * (Ta-Taj);	/* ���x�[��(����) 		*/
		sposing->AltSpd_a1 = sposing->InitSpdAcc + work;			/* �����؂�ւ����x1	*/
		sposing->AltSpd_a2 = sposing->TargetSpd						/* �����؂�ւ����x2	*/
									- work - sposing->SpdRemAcc;
	}
	else
	{	/*----------------------------------------------------------------------------------*/
		/*	POSING�̂Ƃ�																	*/
		/*----------------------------------------------------------------------------------*/
		sposing->dJerkAcc    = sposing->InitAcc;				/* �����x�͈��				*/
		sposing->JerkAccRate = sposing->InitAcc;				/* �����x�͈��				*/
		sposing->SpdRemAcc = 0;									/* ���x�[��(����)			*/
		sposing->AltSpd_a1 = sposing->InitSpdAcc;				/* �����؂�ւ����x1		*/
		sposing->AltSpd_a2 = sposing->TargetSpd;				/* �����؂�ւ����x2		*/
	}
	/*--------------------------------------------------------------------------------------*/
	/*	���ʕϐ������� 																		*/
	/*--------------------------------------------------------------------------------------*/
	sposing->CurrentAccRate = 0;								/* �����x�w��				*/
	sposing->dSpdRemAcc = sposing->SpdRemAcc;
	if( Ta != 0 )
	{
		sposing->dSpdRemAcc = (sposing->SpdRemAcc / Ta) + 1;
	}
}


/********************************************************************************************
	SPOSING �������p�����[�^�v�Z����

	SPOSING�����ɕK�v�ȓ����p�����[�^�̐ݒ�
	���������Z�o�̂��߂ɕK�v�ȃf�[�^�������ŎZ�o�����̂Ō��������̎Z�o�������ōs��
	S���������� = 0�AJerk < 1�ƂȂ�ꍇ�ɂ́APOSING�Ƃ���(S��������Ȃ�����)
*********************************************************************************************/
//static BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
//								LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 )
BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
								LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 )
{
	LONG tmp;
	LONG Td,Tdj;
	LONG spd,tspd,tdec;
	LONG work[2]  = {0}; 									/* ���Z���ʊi�[�p���[�N�f�[�^ */
	LONG work1[2] = {0};
	LONG work2[2] = {0};
	LONG xx[2] = {0};

	tdec = 0;
	sposing->TmpDecVal = 0;
	sposing->DecCnt  = 0;
	sposing->dStopDistRem = 0;
	sposing->jStopDistRem = 0;
	sposing->rStopDistRem = 0;
	sposing->rdStopDistRem = 0;
	
	if( CurrentSpd != 0 )
	{
		/*----------------------------------------------------------------------------------*/
		/* �ڕW���x�̐ݒ�																	*/
		/*----------------------------------------------------------------------------------*/
		if( (sposing->TargetSpd >= CurrentSpd) || (ChkReverse == FALSE) )
		{	/* ChkReverse = FALSE�̂Ƃ��A�����												*/
			/* �ڕW�ʒu���ړ������Ɣ��΂̏ꍇ�͑��x0�܂ŋ����I�Ɍ���������					*/
			tspd = 0;
		}
		else
		{
			tspd = sposing->TargetSpd;
		}
		/*----------------------------------------------------------------------------------*/
		/* �����񐔎Z�o																		*/
		/*----------------------------------------------------------------------------------*/
		spd = CurrentSpd - tspd;
		if( sposing->InitDec==0 )
		{
			sposing->InitDec = 1;
		}
		Td  = (LONG)(spd / sposing->InitDec);				/* �����񐔌v�Z					*/
		Tdj = (LONG)(Td * sposing->CurrentSratio / 100);	/* S������������				*/
		MlibMul3232( Tdj, (Td - Tdj), xx );
		/*----------------------------------------------------------------------------------*/
		/* Jerk(����)�A�����؂�ւ����x�̎Z�o												*/
		/*----------------------------------------------------------------------------------*/
		if( (Tdj != 0) && (xx[1] == 0) )
		{
			sposing->dJerkDec  = (LONG)((ULONG)spd / (ULONG)xx[0]);
			sposing->TmpDecVal = xx[0];
		}
		else
		{
			sposing->dJerkDec  = 0;
			sposing->TmpDecVal = 0xFFFFFFFF;
		}

//		if( (Tdj != 0) && (sposing->dJerkDec != 0) )
		if( sposing->dJerkDec != 0 )
		{	/*------------------------------------------------------------------------------*/
			/*	SPOSING�̂Ƃ�																*/
			/*------------------------------------------------------------------------------*/
			sposing->JerkDecRate = sposing->dJerkDec * Tdj;	/* �����x�ő�l					*/
			tmp = (sposing->JerkDecRate * Tdj) >> 1;
			sposing->SpdRemDec = spd - (sposing->JerkDecRate * (Td-Tdj));/* ���x�[��(����)	*/
			sposing->AltSpd_d1 = tspd + tmp + sposing->SpdRemDec;/* �����؂�ւ����x1		*/
			sposing->AltSpd_d2 = CurrentSpd - tmp;				 /* �����؂�ւ����x2		*/
////			work1[0] = ((sposing->SpeedRemDec - spdrem) * (tcnt + 1) * 2) 
////															+ (spdrem * (tcnt + 1));
			tdec = mtgSpCalSpdRemDist( sposing, Td, work1 );	/* ���x�[���̕����o����	*/
			sposing->DecCnt = Td;						/* �����[�����������̂��߂̌�����	*/
			Td = Td - 1;
		}
		else
		{	/*------------------------------------------------------------------------------*/
			/*	POSING�̂Ƃ�																*/
			/*------------------------------------------------------------------------------*/
			if( Td == 0 )
			{
				sposing->dJerkDec	 = spd;					/* �����x						*/
				sposing->JerkDecRate = spd;					/* �����x						*/
				sposing->SpdRemDec = 0;						/* �����x�[��					*/
				Td = 1;
			}
			else
			{
				sposing->dJerkDec	 = spd / Td;			/* �����x						*/
				sposing->JerkDecRate = sposing->dJerkDec;	/* �����x						*/
				sposing->SpdRemDec = spd - (sposing->JerkDecRate * Td);/* ���x�[��(����)	*/
				tdec = mtgSpCalSpdRemDist( sposing, Td, work1 );/* ���x�[���̕����o����	*/
			}
			sposing->AltSpd_d1 = tspd;						/* �����؂�ւ����x1			*/
			sposing->AltSpd_d2 = CurrentSpd;				/* �����؂�ւ����x2			*/
			sposing->DecCnt = Td;						/* �����[�����������̂��߂̌�����	*/
		}
		/*----------------------------------------------------------------------------------*/
		/*	���ʕϐ������� 																	*/
		/*----------------------------------------------------------------------------------*/
		sposing->InitSpdDec = CurrentSpd;					/* SPOSING�J�n�����x			*/
		sposing->EndSpdDec  = tspd;							/* SPOSING�I�����x				*/
		sposing->CurrentDecRate = 0;						/* �����x������					*/
		sposing->dSpdRemDec = tdec;							/* ���x�[���������x(����)		*/
		/*----------------------------------------------------------------------------------*/
		/* ���������v�Z 																	*/
		/*----------------------------------------------------------------------------------*/
		MlibMul3232( (spd - sposing->SpdRemDec), (Td + 1), work2 );
		MlibAdd6464( work1, work2, work );
	}
/*********************************/
//	debug_xx[i][0]=work2[0];
//	debug_xx[i][1]=work2[1];
//	i++;
//	if(i>1000){i=0;}
/*********************************/	
	/*--------------------------------------------------------------------------------------*/
	/* ��������(������) = (���������v�Z����(������)) / 2 									*/
	/*--------------------------------------------------------------------------------------*/
	*StopDistance  = ((work[1] << (31-bPrm->vel_shift))
								 | ((work[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)));
	/*--------------------------------------------------------------------------------------*/
	/* ��������(������) = (���������v�Z����(������)) / 2 									*/
	/*--------------------------------------------------------------------------------------*/
	*StopDistance0 = (work[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;
	/*--------------------------------------------------------------------------------------*/
	/* ���������I�[�o�t���[�`�F�b�N 														*/
	/*--------------------------------------------------------------------------------------*/
	if( (work[1] >> (bPrm->vel_shift-1)) != 0 )
	{
		return TRUE;										/* �I�[�o�[�t���[				*/
	}
	return FALSE;
}

/*********************************************************************************************
  SPOSING ���x�[���i�����j����

  ���x�[���i�����j�̒�~�������Z�o����B
  ���x�[���i�����j�̕����o���񐔂�Ԃ��B
*********************************************************************************************/
static LONG mtgSpCalSpdRemDist( MG_SPOSING* sposing, LONG Td, LONG* work )
{
	LONG tdec,tcnt;
	LONG spdrem;

	tdec = (sposing->SpdRemDec / Td) + 1;
	tcnt = sposing->SpdRemDec / tdec;
	spdrem = tcnt * tdec;
	MlibMul3232( ((sposing->SpdRemDec * 2) - spdrem), (tcnt + 1), work );
	return tdec;
}

/*********************************************************************************************
 SPOSING  ��������
 *********************************************************************************************/
static LONG mtgSpAccelerator( MG_SPOSING* sposing, LONG CurrentSpd )
{
	LONG CurrentAccRate;										/* ��������x 				*/
	LONG tspd,dSpdRem;

	tspd = sposing->TargetSpd; 									/* �ڕW���x 				*/
	dSpdRem = 0;

	/*--------------------------------------------------------------------------------------*/
	/* Jerk++																				*/
	/*--------------------------------------------------------------------------------------*/
	if( (sposing->InitSpdAcc <= CurrentSpd) && (CurrentSpd <= sposing->AltSpd_a1) )
	{
		sposing->CurrentAccRate += sposing->dJerkAcc;			/* �����x��dJerkAcc�������Z	*/
		if( sposing->CurrentAccRate > sposing->JerkAccRate )
		{
			sposing->CurrentAccRate = sposing->JerkAccRate;		/* LIMIT����				*/
		}
		CurrentAccRate = sposing->CurrentAccRate;				/* ��������x				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk constant																		*/
	/*--------------------------------------------------------------------------------------*/
	else if( CurrentSpd < sposing->AltSpd_a2 )	
	{
		CurrentAccRate = sposing->JerkAccRate;					/* ��������x				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk--																				*/
	/*--------------------------------------------------------------------------------------*/
//	else if( CurrentSpd <= tspd )	
	else
	{
		sposing->CurrentAccRate -= sposing->dJerkAcc;			/* �����x��dJerkAcc�������Z	*/
		if( sposing->CurrentAccRate < 0 )
		{
			sposing->CurrentAccRate = sposing->dJerkAcc;		/* LIMIT���� 				*/
		}
		CurrentAccRate = sposing->CurrentAccRate;				/* ��������x 				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* ���x�[�������o�������i���������j														*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->SpdRemAcc != 0 )
	{
		if( sposing->SpdRemAcc > sposing->dSpdRemAcc )
		{
			dSpdRem = sposing->dSpdRemAcc;
			sposing->SpdRemAcc -= sposing->dSpdRemAcc;
		}
		else
		{
			dSpdRem = sposing->SpdRemAcc;
			sposing->SpdRemAcc = 0;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* �w�ߑ��x���Z�o																		*/
	/*--------------------------------------------------------------------------------------*/
	CurrentSpd = CurrentSpd + CurrentAccRate + dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* �؂�ւ����x�𒲐�																	*/
	/*--------------------------------------------------------------------------------------*/
	sposing->AltSpd_a1 += dSpdRem;
	sposing->AltSpd_a2 += dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* LIMIT���� 																			*/
	/*--------------------------------------------------------------------------------------*/
	if( CurrentSpd >= tspd )
	{
		CurrentSpd = tspd;
	}
	return CurrentSpd;
}

/*********************************************************************************************
  SPOSING ��������

  ���݂̎w�ߑ��x���猸���������w�ߑ��x���v�Z����B
  ���x�[���A�c�����[���͂����ŕ����o���B
*********************************************************************************************/
static LONG mtgSpDecelerator( MG_SPOSING* sposing, LONG CurrentSpd, LONG *Segment )
{
	LONG lRem;
	LONG CurrentDecRate;
	LONG dSpdRem, dStopDistRem;

	lRem  = sposing->StopDistRem;
	dSpdRem = 0;
	dStopDistRem = 0;

	/*--------------------------------------------------------------------------------------*/
	/* Jerk++																				*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->AltSpd_d2 <= CurrentSpd )
	{
		sposing->CurrentDecRate += sposing->dJerkDec;			/* �����x��dJerkDec�������Z	*/
		if( sposing->CurrentDecRate > sposing->JerkDecRate )	/* LIMIT���� 				*/
		{
			sposing->CurrentDecRate = sposing->JerkDecRate;
		}
		CurrentDecRate = sposing->CurrentDecRate;				/* ���񌸑��x 				*/
		sposing->dStopDistRem += sposing->jStopDistRem;	/* �c�����[����jStopDistRem�������Z	*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk constant 																		*/
	/*--------------------------------------------------------------------------------------*/
	else if( sposing->AltSpd_d1 < CurrentSpd )
	{
		CurrentDecRate = sposing->JerkDecRate;					/* ���񌸑��x				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk--																				*/
	/*--------------------------------------------------------------------------------------*/
// 	else if( sposing->EndSpdDec <= CurrentSpd )
 	else
	{
 		sposing->CurrentDecRate -= sposing->dJerkDec;			/* �����x��dJerkDec�������Z	*/
		if( sposing->CurrentDecRate < 0 )						/* LIMIT���� 				*/
		{
			sposing->CurrentDecRate = 0;
		}
		CurrentDecRate = sposing->CurrentDecRate;				/* ���񌸑��x				*/
		sposing->dStopDistRem -= sposing->jStopDistRem;	/* �c�����[����jStopDistRem�������Z	*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* ���x�[�������o�������i���������j														*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->SpdRemDec != 0 )
	{
		if( sposing->SpdRemDec > sposing->dSpdRemDec )
		{
			dSpdRem = sposing->dSpdRemDec;						/* ���x�[�����񕥂��o����	*/
			sposing->SpdRemDec -= sposing->dSpdRemDec;
		}
		else
		{
			dSpdRem = sposing->SpdRemDec;						/* ���x�[�����񕥂��o����	*/
			sposing->SpdRemDec = 0;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* �w�ߑ��x���Z�o																		*/
	/*--------------------------------------------------------------------------------------*/
	CurrentSpd  = CurrentSpd - CurrentDecRate - dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* �c�����[�������o�������i���������j													*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->rStopDistRem > sposing->rdStopDistRem )
	{	/* �c�����[���̂���ɗ]�蕪�̕����ʂ����Z		*/
		dStopDistRem += sposing->rdStopDistRem;	
		sposing->rStopDistRem -= sposing->rdStopDistRem;
	}
	else
	{	/* �c�����[���̂���ɗ]�蕪�𕥂��o���Ō��1�� */
		dStopDistRem += sposing->rStopDistRem;
		sposing->rStopDistRem  = 0;
	}

	if( sposing->StopDistRem != 0 )
	{	/* �c�����[���̍��񕥂��o���ʂ��Z�o		*/
		dStopDistRem += sposing->dStopDistRem;
		sposing->StopDistRem -= dStopDistRem;
		if( sposing->StopDistRem < 0 )
		{	/* �c�����[���𕥂��o���Ō��1��	*/
			sposing->dStopDistRem = lRem;
			sposing->StopDistRem  = 0;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* �؂�ւ����x�𒲐�																	*/
	/*--------------------------------------------------------------------------------------*/
	sposing->AltSpd_d1 -= dSpdRem;
	sposing->AltSpd_d2 -= dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* LIMIT���� 																			*/
	/*--------------------------------------------------------------------------------------*/
	if( CurrentSpd < sposing->EndSpdDec )
	{
		CurrentSpd = sposing->EndSpdDec;
	}
	if( CurrentSpd < 0 )
	{
		CurrentSpd = 0;
	}
	/*--------------------------------------------------------------------------------------*/
	/* ����̕����o���ʂ��X�V�A���ݑ��x��Ԃ�												*/
	/*--------------------------------------------------------------------------------------*/
	*Segment = CurrentSpd + dStopDistRem;
	return CurrentSpd;
}

/*********************************************************************************************
 �������ʗp�f�[�^���Z����
  �T�v:  �c�����|���������|�w�ߑ��x(�c�����[���̍ő�l)�����߂�B
         �}�C�i�X�ƂȂ鎞�������܂ł��v�Z����B
         �v���X�ƂȂ�Ƃ��͂��̂܂܁i�����`�F�b�N�ł͏����_�ȉ��s�v�j
         ��������������̎��̓~�j�}���l(-2147483648)�Ƃ���B
         -2147483648�̈ړ����������������,�c������-2147483648�̎���
         �����I��214748367�ɂ���B
*********************************************************************************************/
static LONG mtgSpCheckDecelaration(MG_S_POSWK* wk, MC_BASE_PRM* bPrm)
{
	LONG ChkDecDistance;
	LONG CheckDecDistance0;

	/* �������苗�� = �c���� - ��~���� - �w�ߑ��x(�c�����[���̍ő�l) */
	ChkDecDistance = wk->ErrorDistance - wk->StopDistance - (wk->CurrentSpd >> bPrm->vel_shift);

	if( ChkDecDistance > -bPrm->vel_max )/* �ő呬�x(=2^(31-vel_shift))[ref/cyc] */
	{
		/* �������������� */
		if( ChkDecDistance <= 1 )
		{
			/* �c����(ErrorDistance)�����傾�ƌ�������K�v�Ȃ��A1���傫�����́A�������������Ă��}�C�i�X�ɂȂ�Ȃ��B*/
			CheckDecDistance0 = (LONG)(wk->ErrorDistance0 - wk->StopDistance0)
									- ((wk->CurrentSpd << bPrm->vel_shift_s) & VELREM_BIT_MSK);  /* ������ */

			/* �������苗�� = �c���� - ��~���� - �c�����[���̍ő�l */
			ChkDecDistance = MlibSatAdd32( (ChkDecDistance << bPrm->vel_shift),
														(CheckDecDistance0 >> bPrm->vel_shift_s) );

		}
		if( ChkDecDistance > 0 )
		{
			/* ���ʃv���X�Ȃ珬���_�ȉ��s�v */
			ChkDecDistance = ChkDecDistance >> bPrm->vel_shift;
		}
	}
	else
	{
		if( wk->ErrorDistance == 0x80000000/*-2147483648*/ )
		{
			/* �����-2147483648�̈ړ��ʂ��������邽�� */
			ChkDecDistance = 0x7fffffff/*2147483647*/;
		}
		else
		{
			ChkDecDistance = 0x80000000/*-2147483648*/;
		}
	}
	return ChkDecDistance;
}

/****************************************************************************
  �����x�ŉ��������f
  �T�v:  ���݂̑��x���猸���萔�ŉ����ł��邩���f���A���x�w�߂��o�͂���B
         �������ł��������
         �����萔�������萔�Ȃ�����ł������B
         �ڕW���x�����Ȃ�n�j�i�ڕW���x�͉����x�Ń`�F�b�N�ς݁j
         �����x�ύX���x�����Ȃ�n�j�i�����x�ύX���x�͉����x�Ń`�F�b�N�ς݁j
****************************************************************************/
#if 0
static LONG mtgAccelerateDecRateSposing(MG_S_POSWK*	wk, MG_SPOSING* SposingPrm, MC_BASE_PRM* bPrm)
{

	LONG spd,S,work;

	// ���������v�Z
	S = (wk->ErrorDistance << bPrm->vel_shift) / SposingPrm->InitDec;//wk->ErrorDistance�����傾������I�[�o�[�t���[�I�I
	work = 1 + 8 * S;
	spd  = (-1 + MlibSqrtu32u32( work, 1))>>1;
	
	if(spd <= 1)
	{
		wk->Segment = wk->ErrorDistance << bPrm->vel_shift;
		wk->CurrentSpeed = 0;
	}
	else
	{
		wk->CurrentSpeed = ((spd-1) * SposingPrm->InitDec);
		wk->Segment = wk->CurrentSpeed;
	}

	return spd;
}
#endif


/***************************************** end of file **********************************************/
