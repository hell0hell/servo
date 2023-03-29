/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Sqo.c : Sequence Output Signal 																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Translate Sequence Output Signal Logics													*/
/*		2. Make Output Signal 																		*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"

/****************************************************************************************************/
/*																									*/
/*		Sqo Function Prototypes																		*/
/*																									*/
/****************************************************************************************************/
void	IxxSqoMain( SQOV *psqov, ZONET *pZoneCache,     /* Make sequence output signal 				*/
		struct _pos *pNpos );
void	IxxInitSqo( SQOV *psqov, OPPRMDAT *pprm );		/* Initialize Sqo 							*/
void	IxxSqoSetLgcs( SQOV *psqov, OPPRMDAT *pprm );	/* Set sequence output signal logics 		*/ 
void	IxxSqoSetMaps( SQOV *psqov, OPPRMDAT *pprm );	/* Set sequence output signal maps 			*/ 
/*--------------------------------------------------------------------------------------------------*/
UINT	IxxSqoGetOutSig( USHORT *pBuf );				/* Get Output Signal for Register I/F		*/


/****************************************************************************************************/
/*																									*/
/*		Local Functions 																			*/
/*																									*/
/****************************************************************************************************/
void	LpxMakeOutputSignals( SQOV *psqov );                      /* Make output signals            */
//UINT	LpxSqoGetSigs( SQOV *psqov, UINT bitmask, UINT logic );   /* Get sequence output signals    */ 
UINT	LpxSqoGetSigs( SQOV *psqov, UINT bitmask, ULONG logic );  /* Get sequence output signals    *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
void	LpxSqoSetPouts( SQOV *psqov, POUTS pouts );        /* Set programable output signals        */
void	LpxSqoInitPouts( SQOV *psqov, OPPRMDAT *pprm );    /* Set initial programable output signals*/
/*--------------------------------------------------------------------------------------------------*/
//<S081> UINT	LpxSqoMakeInpos( LONG err, LONG width );			/* Make Inpos Signal <S00C>				*/
//<S081> UINT	LpxSqoMakeInposFil( LONG err, LONG width );
//<S081> UINT	LpxSqoMakeInposDen( LONG err, LONG width );


/****************************************************************************************************/
/*																									*/
/*  Output Signal Default Assign Table																*/
/*																									*/
/****************************************************************************************************/
const UCHAR CSqoAssignTbl[] = 
{
	0x00,
	0x00,
	0x00,
};


/****************************************************************************************************/
/*																									*/
/*		Make Sequence Output Signal Main Function													*/
/*																									*/
/****************************************************************************************************/
void	IxxSqoMain( SQOV *psqov, ZONET *pZoneCache, struct _pos *pNpos )
{
/*--------------------------------------------------------------------------------------------------*/
	psqov->ZoneIdx = IxxZoneGetIdx( pNpos->Apos, pZoneCache );  /* set zone index */
//	LpxSqoSetPouts( psqov, NoutV.Pouts );                       /* set POUTs */
/*--------------------------------------------------------------------------------------------------*/
	LpxMakeOutputSignals( psqov );                              /* Make output signals */
/*--------------------------------------------------------------------------------------------------*/
//	NoutV.Sigs = LpxSqoGetSigs( psqov, 0x07F, 0 );              /* Output signals (SO1-SO7) *//* <S04B> */
	NoutV.Sigs = LpxSqoGetSigs( psqov, 0x07F, NoutV.XorMask );  /* Output signals (SO1-SO7) *//* <S04B> */
	return;
}

/****************************************************************************************************/
/*																									*/
/* 		Initialize Sqo 																				*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxInitSqo( SQOV *psqov, OPPRMDAT *pPrm)
{
	MlibResetLongMemory( psqov, sizeof( SQOV )/4 );
/*--------------------------------------------------------------------------------------------------*/
	IxxSqoSetLgcs( psqov, pPrm );				       /* set output signal logics */
	IxxSqoSetMaps( psqov, pPrm );				       /* set output signal maps */
/*--------------------------------------------------------------------------------------------------*/
//	LpxSqoInitPouts( psqov, pPrm );                    /* TODO: set default Pout (PnB4F) */
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* <S081> */
	switch ( NoutV.P.CoinOut )						   /* <S00C> */
	{
	case INPOUTSEL_PWIDTH:
		psqov->MakeInpos = LpxSqoMakeInpos; break;
	case INPOUTSEL_PWIDTH_FILTER:
		psqov->MakeInpos = LpxSqoMakeInposFil; break;
	case INPOUTSEL_PWIDTH_DEN:
		psqov->MakeInpos = LpxSqoMakeInposDen; break;
	default:
		psqov->MakeInpos = LpxSqoMakeInposDen; break;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*																									*/
/*		Set sequence output signal logics 															*/ 
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxSqoSetLgcs( SQOV *psqov, OPPRMDAT *pprm )
{
SQO_SIGS wkx;
#define LPX_SETACTVLGCS( lgcs, prm, poutsig ) \
	(lgcs) = (((prm) & 0x0F) == SQO_LGC_OPPOSITE) ? ( (lgcs) | (poutsig) ) : ( (lgcs) & ~(poutsig) )
#define LPX_SETNONACTVMASK( mask, prm, sqosig) \
	(mask) = ((((prm) & 0x0F) == SQO_LGC_NONACTIVE) ? ( (mask) & ~(sqosig) ) : ( (mask) | (sqosig) ))
/*--------------------------------------------------------------------------------------------------*/
/*  Set active logics																				*/
/*--------------------------------------------------------------------------------------------------*/
	wkx = 0;
/*--------------------------------------------------------------------------------------------------*/
//<S081>	LPX_SETACTVLGCS( wkx, (pprm->bINPOSLgc), SQO_SIGS_INPOS );
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT0Lgc), SQO_SIGS_POUT0 );
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT1Lgc), SQO_SIGS_POUT1 );
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT2Lgc), SQO_SIGS_POUT2 );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT3Lgc), SQO_SIGS_POUT3 );
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT4Lgc), SQO_SIGS_POUT4 );
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT5Lgc), SQO_SIGS_POUT5 );
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT6Lgc), SQO_SIGS_POUT6 );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETACTVLGCS( wkx, (pprm->bPOUT7Lgc), SQO_SIGS_POUT7 );
	LPX_SETACTVLGCS( wkx, (pprm->bWRNLgc), SQO_SIGS_WRN );
	LPX_SETACTVLGCS( wkx, (pprm->bBKLgc), SQO_SIGS_BK );
	LPX_SETACTVLGCS( wkx, (pprm->bSRDYLgc), SQO_SIGS_SRDY );
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETACTVLGCS( wkx, (pprm->CLTLgc), SQO_SIGS_CLT	);
	LPX_SETACTVLGCS( wkx, (pprm->TGONLgc), SQO_SIGS_TGON );
//	LPX_SETACTVLGCS( wkx, (pprm->NEARLgc), SQO_SIGS_NEAR );	/* <S1D4> */
	LPX_SETACTVLGCS( wkx, (pprm->VCMPLgc), SQO_SIGS_VCMP );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETACTVLGCS( wkx, (pprm->BUSYLgc), SQO_SIGS_BUSY );	    /* @@ Anlg */
//	LPX_SETACTVLGCS( wkx, (pprm->POSRDYLgc), SQO_SIGS_POSRDY );	/* @@ Anlg */
//	LPX_SETACTVLGCS( wkx, (pprm->PCOLgc), SQO_SIGS_PCO );	    /* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETACTVLGCS( wkx, (pprm->VLTLgc), SQO_SIGS_VLT );	    /* @@ Anlg */
	LPX_SETACTVLGCS( wkx, (pprm->PSELALgc), SQO_SIGS_PSELA );	/* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
	psqov->ActvLgcs = wkx;
/*--------------------------------------------------------------------------------------------------*/
/*  Set non active mask																				*/
/*--------------------------------------------------------------------------------------------------*/
	wkx = 0;
/*--------------------------------------------------------------------------------------------------*/
//<S081>	LPX_SETNONACTVMASK( wkx, (pprm->bINPOSLgc), SQO_SIGS_INPOS );
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT0Lgc), SQO_SIGS_POUT0 );
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT1Lgc), SQO_SIGS_POUT1 );
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT2Lgc), SQO_SIGS_POUT2 );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT3Lgc), SQO_SIGS_POUT3 );
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT4Lgc), SQO_SIGS_POUT4 );
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT5Lgc), SQO_SIGS_POUT5 );
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT6Lgc), SQO_SIGS_POUT6 );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETNONACTVMASK( wkx, (pprm->bPOUT7Lgc), SQO_SIGS_POUT7 );
	LPX_SETNONACTVMASK( wkx, (pprm->bWRNLgc), SQO_SIGS_WRN );
	LPX_SETNONACTVMASK( wkx, (pprm->bBKLgc), SQO_SIGS_BK );
	LPX_SETNONACTVMASK( wkx, (pprm->bSRDYLgc), SQO_SIGS_SRDY );
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETNONACTVMASK( wkx, (pprm->CLTLgc), SQO_SIGS_CLT	);
	LPX_SETNONACTVMASK( wkx, (pprm->TGONLgc), SQO_SIGS_TGON );
//	LPX_SETNONACTVMASK( wkx, (pprm->NEARLgc), SQO_SIGS_NEAR );	/* <S1D4> */
	LPX_SETNONACTVMASK( wkx, (pprm->VCMPLgc), SQO_SIGS_VCMP );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETNONACTVMASK( wkx, (pprm->BUSYLgc), SQO_SIGS_BUSY );		/* @@ Anlg */
//	LPX_SETNONACTVMASK( wkx, (pprm->POSRDYLgc), SQO_SIGS_POSRDY );	/* @@ Anlg */
//	LPX_SETNONACTVMASK( wkx, (pprm->PCOLgc), SQO_SIGS_PCO );		/* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETNONACTVMASK( wkx, (pprm->VLTLgc), SQO_SIGS_VLT );	        /* @@ Anlg */
	LPX_SETNONACTVMASK( wkx, (pprm->PSELALgc), SQO_SIGS_PSELA );	    /* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
	psqov->NonActvMask = wkx;
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*																									*/
/*		Set sequence output signal maps 															*/ 
/*																									*/
/*		ex)																							*/
/*      bINPOSLGC.1 = 0 -> Map = 0 ( output to SO1)                                                 */
/*      bINPOSLGC.1 = 1 -> Map = 1 ( output to SO2)                                                 */
/*      bINPOSLGC.1 = 2 -> Map = 2 ( output to SO3)                                                 */
/*            :                                                                                     */
/*      bINPOSLGC.1 = F -> Map = F ( no output signal)                                              */
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxSqoSetMaps( SQOV *psqov, OPPRMDAT *pprm )
{
#define LPX_SETMAP( map, prm ) (map) = (((prm)>>4) & 0x0F)
/*--------------------------------------------------------------------------------------------------*/
//<S081>	LPX_SETMAP( psqov->Map.InPos, pprm->bINPOSLgc );
//	LPX_SETMAP( psqov->Map.Pout0, pprm->bPOUT0Lgc );
//	LPX_SETMAP( psqov->Map.Pout1, pprm->bPOUT1Lgc );
//	LPX_SETMAP( psqov->Map.Pout2, pprm->bPOUT2Lgc );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETMAP( psqov->Map.Pout3, pprm->bPOUT3Lgc );
//	LPX_SETMAP( psqov->Map.Pout4, pprm->bPOUT4Lgc );
//	LPX_SETMAP( psqov->Map.Pout5, pprm->bPOUT5Lgc );
//	LPX_SETMAP( psqov->Map.Pout6, pprm->bPOUT6Lgc );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETMAP( psqov->Map.Pout7, pprm->bPOUT7Lgc );
	LPX_SETMAP( psqov->Map.Wrn, pprm->bWRNLgc );
	LPX_SETMAP( psqov->Map.Bk, pprm->bBKLgc );
	LPX_SETMAP( psqov->Map.Srdy, pprm->bSRDYLgc );
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETMAP( psqov->Map.Clt, pprm->CLTLgc );
	LPX_SETMAP( psqov->Map.Tgon, pprm->TGONLgc );
//	LPX_SETMAP( psqov->Map.Near, pprm->NEARLgc );	/* <S1D4> */
	LPX_SETMAP( psqov->Map.Vcmp, pprm->VCMPLgc );
/*--------------------------------------------------------------------------------------------------*/
//	LPX_SETMAP( psqov->Map.Busy, pprm->BUSYLgc );     /* @@ Anlg */
//	LPX_SETMAP( psqov->Map.Posrdy, pprm->POSRDYLgc ); /* @@ Anlg */
//	LPX_SETMAP( psqov->Map.Pco, pprm->PCOLgc );       /* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETMAP( psqov->Map.Vlt, pprm->VLTLgc );       /* @@ Anlg */
	LPX_SETMAP( psqov->Map.Psela, pprm->PSELALgc );       /* @@ Anlg */
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*																									*/
/*  Make output signals (SQO_MidScan)																*/
/*  TODO: 初期化中や、パラメータリセット中の出力信号仕様は未定。                                    */
/*																									*/
/****************************************************************************************************/
void	LpxMakeOutputSignals( SQOV *psqov )
{
SQO_SIGS sigs;
UINT zoneidx;
UINT lgc;
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SET_POUTSIGS( out, no, pout_sig, lgc )     \
	switch ( (lgc) ) \
    { \
    case SQO_POUT_NONACTIVE: \
        (out) &= ~(pout_sig); \
        break; \
	case SQO_POUT_ACTIVE: \
		(out) |= (pout_sig); \
		break; \
	case SQO_POUT_ZONE: \
		(out) = (( zoneidx ) & (1<<(no))) ? ( (out) | (pout_sig) ) : ((out) & ~(pout_sig)); \
		break; \
	case SQO_POUT_PREVIOUS: \
		break; \
	} 
/*--------------------------------------------------------------------------------------------------*/
	zoneidx = psqov->ZoneIdx;                                                        /* get zone    */
	sigs = psqov->Sigs;                                                              /* get sigs    */
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S081> */
	sigs = 
#if 1	/* <S00C> */
//Sigma-SのCOIN  (NinV.NrmBits.INPOS) ? ( sigs | SQO_SIGS_INPOS ) : ( sigs & ~SQO_SIGS_INPOS ); /* set inpos   */
		( psqov->MakeInpos( MlibABS( MtHdl.psnHdl.Tpos - MtHdl.psnHdl.Apos ), MtHdl.psnHdl.Prm->psetWidth )) 
		? ( sigs | SQO_SIGS_INPOS ) 
		: ( sigs & ~SQO_SIGS_INPOS ); 							/* set inpos   */
#else
		/*  
		 * 制御部からのレスポンスのCOINはScanB一回だけ遅れる。
		 * SGDVと同じタイミングで出力するなら、何か方法を検討する
		 *
		 */
	  ( IXX_BITCHK( NinV.Ctrl.RspCtrlBit, COIN_BITNO )) ? ( sigs | SQO_SIGS_INPOS ) : ( sigs & ~SQO_SIGS_INPOS ); /* set inpos   */
#endif
#endif
/*--------------------------------------------------------------------------------------------------*/
//	lgc = psqov->Pouts.POut0; LPX_SET_POUTSIGS( sigs, 0, SQO_SIGS_POUT0, lgc );      /* set POUT0   */
//	lgc = psqov->Pouts.POut1; LPX_SET_POUTSIGS( sigs, 1, SQO_SIGS_POUT1, lgc );      /* set POUT1   */
//	lgc = psqov->Pouts.POut2; LPX_SET_POUTSIGS( sigs, 2, SQO_SIGS_POUT2, lgc );      /* set POUT2   */
//	lgc = psqov->Pouts.POut3; LPX_SET_POUTSIGS( sigs, 3, SQO_SIGS_POUT3, lgc );      /* set POUT3   */
//	lgc = psqov->Pouts.POut4; LPX_SET_POUTSIGS( sigs, 4, SQO_SIGS_POUT4, lgc );      /* set POUT4   */
//	lgc = psqov->Pouts.POut5; LPX_SET_POUTSIGS( sigs, 5, SQO_SIGS_POUT5, lgc );      /* set POUT5   */
//	lgc = psqov->Pouts.POut6; LPX_SET_POUTSIGS( sigs, 6, SQO_SIGS_POUT6, lgc );      /* set POUT6   */
//	lgc = psqov->Pouts.POut7; LPX_SET_POUTSIGS( sigs, 7, SQO_SIGS_POUT7, lgc );      /* set POUT7   */
/*--------------------------------------------------------------------------------------------------*/
	sigs = (IXX_BITCHK( NinV.Seq, WRN_DETECTED_BITNO ) ) ? ( sigs | SQO_SIGS_WRN ) : ( sigs & ~SQO_SIGS_WRN ); /* set WRN  */
	sigs = (IXX_BITCHK( NinV.Seq, BRK_RELEASING_BITNO ) ) ? ( sigs | SQO_SIGS_BK ) : ( sigs & ~SQO_SIGS_BK );  /* set BK   */
	sigs = (IXX_BITCHK( NinV.Seq, SERVO_READY_BITNO ) ) ? ( sigs | SQO_SIGS_SRDY) : ( sigs & ~SQO_SIGS_SRDY ); /* set SRDY */
/*--------------------------------------------------------------------------------------------------*/
	sigs = (IXX_BITCHK( NinV.Ctrl.RspCtrlBit, TRQ_CLAMP_BITNO )  ) ? ( sigs | SQO_SIGS_CLT ) : (sigs & ~SQO_SIGS_CLT);     /* set CLT  */
	sigs = (IXX_BITCHK( NinV.Ctrl.RspCtrlBit, MOTOR_MOVING_BITNO ) ) ? ( sigs | SQO_SIGS_TGON ) : (sigs & ~SQO_SIGS_TGON);/* set TGON */
//	sigs = ( PsnGetNearSts( &(MtHdl.psnHdl) ) /* <S00C> NinV.NrmBits.NEAR */) ? ( sigs | SQO_SIGS_NEAR ) : (sigs & ~SQO_SIGS_NEAR);    /* set NEAR */	/* <S1D4> */
	sigs = (IXX_BITCHK( NinV.Ctrl.RspCtrlBit, REACH_SPD_BITNO ) ) ? ( sigs | SQO_SIGS_VCMP ) : (sigs & ~SQO_SIGS_VCMP);   /* set VCMP */
/*--------------------------------------------------------------------------------------------------*/
//	sigs = ( !(NinV.NrmBits.DEN)) ? ( sigs | SQO_SIGS_BUSY ) : (sigs & ~SQO_SIGS_BUSY); /* @@ Anlg BUSY */
//	sigs = ( NinV.NrmBits.HOMEDONE ) ? ( sigs | SQO_SIGS_POSRDY ) : (sigs & ~SQO_SIGS_POSRDY); /* @@ Anlg POSRDY */
//	sigs = ( NinV.NrmBits.PGCPHASE ) ? ( sigs | SQO_SIGS_PCO ) : (sigs & ~SQO_SIGS_PCO); /* @@ Anlg PCO */
/*--------------------------------------------------------------------------------------------------*/
	sigs = (IXX_BITCHK( NinV.Ctrl.RspCtrlBit, SPD_CLAMP_BITNO ) ? ( sigs | SQO_SIGS_VLT ) : (sigs & ~SQO_SIGS_VLT));
	sigs = (IXX_BITCHK( SqiV.Sigs, SQI_BNO_PSEL ) ? ( sigs | SQO_SIGS_PSELA ) : (sigs & ~SQO_SIGS_PSELA));
/*--------------------------------------------------------------------------------------------------*/
	psqov->RegSigs = sigs;                                                        /* save signals   */
	sigs = sigs ^ (psqov->ActvLgcs);                                              /* convert logics */
	sigs &= (psqov->NonActvMask);                                                 /* clear bits     */
	psqov->Sigs = sigs;                                                           /* set signals    */
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Get sequence output signals                                                                     */ 
/*  return: SO1-SO7 (Dout) output signals                                                           */
/*  bitmask(0-0xFF): output bit mask. if SO1-SO3 are used, mask = 0x07.                             */
/*                                                                                                  */
/*																									*/
/****************************************************************************************************/
//UINT	LpxSqoGetSigs( SQOV *psqov, UINT bitmask, UINT logic ) /* <S04B> */
UINT	LpxSqoGetSigs( SQOV *psqov, UINT bitmask, ULONG logic ) /* <S04B> */
{
#define LPX_SETOUTSIGS( outsigs, insigs, map, poutsig ) ((outsigs) = ((insigs) & (poutsig)) ? \
                                                        ((outsigs) | (1<<(map))) : (outsigs))
UINT outsigs;
UINT insigs;
UINT map;
//UINT mask; /* <S04B> */
	outsigs = 0;
	insigs = psqov->Sigs;
/*--------------------------------------------------------------------------------------------------*/
//<S081>	map = psqov->Map.InPos; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_INPOS );
//	map = psqov->Map.Pout0; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT0 );
//	map = psqov->Map.Pout1; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT1 );
//	map = psqov->Map.Pout2; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT2 );
/*--------------------------------------------------------------------------------------------------*/
//	map = psqov->Map.Pout3; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT3 );
//	map = psqov->Map.Pout4; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT4 );
//	map = psqov->Map.Pout5; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT5 );
//	map = psqov->Map.Pout6; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT6 );
/*--------------------------------------------------------------------------------------------------*/
//	map = psqov->Map.Pout7; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POUT7 );
	map = psqov->Map.Wrn; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_WRN );
	map = psqov->Map.Bk; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_BK );
	map = psqov->Map.Srdy; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_SRDY );
/*--------------------------------------------------------------------------------------------------*/
	map = psqov->Map.Clt; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_CLT );
	map = psqov->Map.Tgon; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_TGON );
//	map = psqov->Map.Near; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_NEAR );		/* <S1D4> */
	map = psqov->Map.Vcmp; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_VCMP );
/*--------------------------------------------------------------------------------------------------*/
//	map = psqov->Map.Busy; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_BUSY );     /* @@ Anlg */
//	map = psqov->Map.Posrdy; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_POSRDY ); /* @@ Anlg */
//	map = psqov->Map.Pco; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_PCO );       /* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
	map = psqov->Map.Vlt; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_VLT );       /* @@ Anlg */
	map = psqov->Map.Psela; LPX_SETOUTSIGS( outsigs, insigs, map, SQO_SIGS_PSELA );   /* @@ Anlg */
/*--------------------------------------------------------------------------------------------------*/
//	mask = (logic) ? 0x00 : 0xFFFFFFFF;												  /* <S04B> */
//	outsigs = outsigs ^ mask;														  /* <S04B> */
//<S084>	outsigs = outsigs ^ logic;												  /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
	return (outsigs & bitmask);
}

/****************************************************************************************************/
/*																									*/
/*  Set initial programable output signals                                                          */
/*                                                                                                  */
/*  psqov: SQO variable                                                                             */
/*  pprm: Option parameters                                                                         */
/*                                                                                                  */
/*																									*/
/****************************************************************************************************/
#if 0
void	LpxSqoInitPouts( SQOV *psqov, OPPRMDAT *pprm )
{
#define LPX_INITPOUTSIGS( psqo, no, pout ) ((psqo)->Pouts.POut##no = (pout))
/*--------------------------------------------------------------------------------------------------*/
	switch ( pprm->bPOutInitZone ) 
	{
		case 0:
			LPX_INITPOUTSIGS( psqov, 0, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 1, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 2, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 3, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 4, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 5, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 6, PGM_POUT_NONACTIVE );
			LPX_INITPOUTSIGS( psqov, 7, PGM_POUT_NONACTIVE );
			break;

		default:
			LPX_INITPOUTSIGS( psqov, 0, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 1, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 2, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 3, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 4, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 5, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 6, PGM_POUT_ZONE );
			LPX_INITPOUTSIGS( psqov, 7, PGM_POUT_ZONE );
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	NoutV.Pouts = psqov->Pouts;                                          /* Set default pouts */
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Get Output Signal for Register I/F															*/
/*                                                                                                  */
/****************************************************************************************************/
#if 0
UINT	IxxSqoGetOutSig( USHORT *pBuf )
{
ULONG sigs = SqoV.RegSigs;
	pBuf[0] = (USHORT)(sigs);
	pBuf[1] = (USHORT)(sigs >> 16);
	return ( REGIF_COMPLETE );
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Make Inpos Signal <S00C> 																	*/
/*                                                                                                  */
/****************************************************************************************************/
#if 0	/* <S081> */
UINT	LpxSqoMakeInpos( LONG err, LONG width )
{
	return ( err <= width );
}

UINT	LpxSqoMakeInposFil( LONG err, LONG width )
{
	return /* TODO: */( err <= width );
}

UINT	LpxSqoMakeInposDen( LONG err, LONG width )
{
	return  ( ( PsnGetDenSts( &(MtHdl.psnHdl) ) ) && (err <= width));
}
#endif


