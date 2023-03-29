/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Sqi.c : Sequence Input Signal 																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Translate Sequence Input Signal Logics													*/
/*		2. Sequence Input Signal State transition													*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.0x : 2013.09.04  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"

/****************************************************************************************************/
/*																									*/
/*		Sqi Definitions																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Macro Definitions																			*/
/*																									*/
/****************************************************************************************************/
#define LPX_CHECK_EDGEON( psqi, sig )  ( (!((psqi)->LastSigs & (sig))) && ( (psqi)->Sigs & (sig) ) )
#define LPX_CHECK_OFF( psqi, sig ) (!((psqi)->Sigs & (sig)))
#define LPX_CHECK_ON( psqi, sig )  (((psqi)->Sigs & (sig)))
#define LPX_CHECK_JOGCHG( psqi )  (((psqi)->LastSigs ^ (psqi)->Sigs) & \
			( SQI_SIGS_SEL4JOG3 | SQI_SIGS_SEL3JOG2 | SQI_SIGS_SEL2JOG1 | SQI_SIGS_SEL1JOG0))
#define LPX_GET_PGMSEL( psqi )    (((psqi)->Sigs) >> (SQI_BNO_SEL0JOGN)) & ((PGM_N_PGM)-1)
//#define LPX_GET_JOGSEL( psqi )    (((psqi)->Sigs) >> (SQI_BNO_SEL1JOG0)) & ((JSPD_N_JSPD)-1)
#define LPX_GET_JOGSEL( psqi )    (0)                             /* サンプル版機能制限 */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*	Position Error Clear Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
/*	1)Level Clear: Ipos,Tpos = Feedback Position, Motion = None(DposIn=0)							*/
/*	2)Latch Clear: Ipos,Tpos = Latched Position, Motion = Interpolate(DposIn=Lpos-Apos)				*/
/*	3)Caution: do not execute perr clear on Velocity or Torque control mode							*/
/*--------------------------------------------------------------------------------------------------*/
#define Lpx_ClrSequence( nmsg ) {                                                                    \
			PsnResetCmdPosAll( &(MtHdl.psnHdl), 0 );                       /* Follow Up */           \
			(nmsg)->Channel = CHANNEL_AXCTRL;                              /* update motion */       \
			IxxAxctrlReqSkip( &(nmsg->fmt.CmdData), MT_HOLD_FASTSTOP );    /* hold request */        \
			NoutV.Ctrl.ClrPosErr = TRUE;                                   /* set clr command */     \
}

/*--------------------------------------------------------------------------------------------------*/
/*  Check Perr Clear Command																		*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_CHECK_CLRON( psqiv ) ( IXX_BITCHK(NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO ) )
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Sqi Function Prototypes																		*/
/*																									*/
/****************************************************************************************************/
void	IxxSqiMain( 										/* Sqi Main Function 					*/
			SQIV *psqiv, 									/* Sqi Valiable							*/
			JSPDT *pJspdCache, 								/* Jog Speed Table Cache				*/
			struct _token *nmsg );							/* ScanN Message						*/
void	IxxInitSqi( 										/* Initialize Sqi 						*/
			SQIV *psqiv,  									/* Sqi Variable 						*/
			OPPRMDAT *pPrm );								/* the Parameters						*/
void	IxxSqiSetLgcs( SQIV *psqiv, OPPRMDAT *pprm );		/* Set sequence input signal logics 	*/ 
void	IxxSqiSetMaps( SQIV *psqiv, OPPRMDAT *pPrm );		/* Set sequence input signal maps	 	*/ 
UINT	IxxSqiGetSigs( void );								/* Get Signals                          */
/*--------------------------------------------------------------------------------------------------*/
UINT	IxxSqiSetSigClrTime( USHORT *pBuf );				/* Set Clear Time for Register I/F		*/
/*--------------------------------------------------------------------------------------------------*/
UINT	IxxSqiGetSigClrTime( USHORT *pBuf );				/* Get Clear Time for Register I/F		*/
UINT	IxxSqiGetInSig( USHORT *pBuf );						/* Get Input Signal for Register I/F	*/
UCHAR GetControlMode( void );								/* Get Control Mode						*/	/* <S03D> */
/****************************************************************************************************/
/*																									*/
/*		Local Functions 																			*/
/*																									*/
/****************************************************************************************************/
void	LpxTranslateSig( SQIV *psqiv );				        /* Translate Input Signal Logic			*/
void	LpxSqiSequence( SQIV *psqiv,                        /* Sequence Input Signal State Machine	*/
				JSPDT *pJspdCache, struct _token *nmsg );	
/*--------------------------------------------------------------------------------------------------*/
void	LpxJogSequence( SQIV *psqiv, 
				JSPDT *pJspdCache, struct _token *nmsg );	/* JOG/homing mode sequence				*/
void	LpxCancelJogSequence( SQIV *psqiv, 
				struct _token *nmsg );                      /* cancel JOG/homing mode sequence		*/
void	LpxPgmSequence( SQIV *psqiv, struct _token *nmsg );	/* Program table mode sequence			*/
void	LpxCancelPgmSequence( SQIV *psqiv, 
				struct _token *nmsg );		                /* cancel Program table mode sequence	*/
/*--------------------------------------------------------------------------------------------------*/
SQI_SIGS LpxGetSigs( SQIV *psqiv, UINT lgc );			    /* Get input signals 					*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_LONG( psrc )    ((LONG)(psrc[0]) | ((LONG)(psrc[1]) << 16))
/*--------------------------------------------------------------------------------------------------*/
#define LpxInputSignalFromRegister( x ) { ;}
//void	LpxInputSignalFromRegister( SQIV *psqiv );			/* Get Input Signal for Register I/F	*/
/*--------------------------------------------------------------------------------------------------*/
#define LpxCheckSvonNotYetError( x ) { ;}
//void	LpxCheckSvonNotYetError( SQIV *psqiv ); 			/* Check E54E:Servo on not yet 			*/
/*--------------------------------------------------------------------------------------------------*/
static void	 LpxSqiMakeSensorOn( SQIV *psqiv );				/* <S168> 								*/

/****************************************************************************************************/
/*																									*/
/*		Sqi Variables Definition																	*/
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*  Input Signal Default Assign Table																*/
/*																									*/
/****************************************************************************************************/
const UCHAR CSqiAssignTbl[] = 
{
	SQI_LGC_NONACTIVE,          /* /MODE (0x000001)													*/
	SQI_LGC_NONACTIVE,          /* /STARTHOME (0x000002)											*/
	SQI_LGC_NONACTIVE,          /* /PGMRESJOGP (0x000004)											*/
	SQI_LGC_NONACTIVE,          /* /SEL0JOGN (0x000008)												*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_NONACTIVE,          /* /SEL1JOG0 (0x000010)												*/
	SQI_LGC_NONACTIVE,          /* /SEL2JOG1 (0x000020)												*/
	SQI_LGC_NONACTIVE,          /* /SEL3JOG2 (0x000040)												*/
	SQI_LGC_NONACTIVE,          /* /SEL4JOG3 (0x000080)												*/
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_NONACTIVE,          /* /SEL5JOG4 (0x000100)												*/
	SQI_LGC_NONACTIVE,          /* /SEL6JOG5 (0x000200)												*/
	SQI_LGC_NONACTIVE,          /* /SEL7JOG4 (0x000400)												*/
//	SQI_LGC_NONACTIVE,          /* Reserved (0x000800)												*/
	SQI_LGC_NONACTIVE,          /* /P-DET   (0x000800)									 <S21C> 	*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_DEFAULT | (0<<4),   /* /S-ON (0x001000) Default, SI0									*/
	SQI_LGC_DEFAULT | (4<<4),   /* /ALM-RST (0x002000) Default, SI4									*/
	SQI_LGC_NONACTIVE,          /* /DEC (0x004000)													*/
	SQI_LGC_NONACTIVE,          /* /RGRT (0x008000)													*/
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_DEFAULT | (2<<4),   /* P-OT (0x010000) Default, SI2										*/
	SQI_LGC_DEFAULT | (3<<4),   /* N-OT (0x020000) Default, Si3										*/
	SQI_LGC_DEFAULT | (5<<4),   /* /P-CL (0x040000) Default, SI5									*/
	SQI_LGC_DEFAULT | (6<<4),   /* /N-CL (0x080000)	Default, SI6									*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_NONACTIVE,          /* /CLR(0x100000)	@@ Mercury未使用								*/
	SQI_LGC_DEFAULT | (1<<4),   /* /P-CON (0x200000) Default, SI1									*/
	SQI_LGC_NONACTIVE,          /* /ZCLAMP (0x400000) NonActive										*/
	SQI_LGC_NONACTIVE,          /* /INHIBIT (0x800000) NonActive									*/
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_NONACTIVE,          /* /P-SEL (0x1000000) NonActive										*/
/* <S03D> Start */
	/*----------------------------------------------------------------------------------------------*/
	SQI_LGC_NONACTIVE,          /* /SPD-D (0x2000000) NonActive										*/
	SQI_LGC_NONACTIVE,          /* /SPD-A (0x4000000) NonActive										*/
	SQI_LGC_NONACTIVE,          /* /SPD-B (0x8000000) NonActive										*/
	SQI_LGC_NONACTIVE,          /* /C-SEL (0x10000000) NonActive									*/
	/*----------------------------------------------------------------------------------------------*/
/* <S03D> End */
	SQI_LGC_NONACTIVE,          /* /G-SEL (0x20000000) NonActive <S09B>								*/
	SQI_LGC_NONACTIVE,          /* /SEN (0x40000000) NonActive <S168>								*/
	SQI_LGC_NONACTIVE,          /* /FSTP (0x80000000) NonActive <S1F5>							*/
};

/****************************************************************************************************/
/*  																								*/
/*  Sqi Main Function 																				*/
/*  																								*/
/****************************************************************************************************/
void	IxxSqiMain( SQIV *psqiv, JSPDT *pJspdCache, struct _token *nmsg )
{
/*--------------------------------------------------------------------------------------------------*/
	if ( nmsg->Channel != CHANNEL_SQI ) { return; }
/*--------------------------------------------------------------------------------------------------*/
	LpxInputSignalFromRegister( psqiv );                          /* input signal from Reg. I/F     */
	LpxTranslateSig( psqiv );   					              /* Translate input signal logic	*/
	LpxSqiSequence( psqiv, pJspdCache, nmsg );  		          /* SQI state machine              */
/*--------------------------------------------------------------------------------------------------*/
	IXX_BITSET( NoutV.Seq, ALM_RESET_BITNO,                       /* Alerm reset reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_ALMRST ) );
	IXX_BITSET( NoutV.Seq, ENB_EXTPTRQLMT_BITNO,                  /* P-CL reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_PCL ) );
	IXX_BITSET( NoutV.Seq, ENB_EXTNTRQLMT_BITNO,                  /* N-CL reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_NCL ) );
/*--------------------------------------------------------------------------------------------------*/
	IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, ENBPCTRL_BITNO,            /* @@ P-CON reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_PCON ) );
	IXX_BITSET( NoutV.Seq, POSITIVE_OT_BITNO,                     /* @@ P-OT reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_POT ) );
	IXX_BITSET( NoutV.Seq, NEGATIVE_OT_BITNO,                     /* @@ N-OT reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_NOT ) );
	IXX_BITSET( NoutV.Seq, ZCLAMP_BITNO,                          /* @@ ZCLAMP reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_ZCLAMP ) );
/* <S03D> Start */
	IXX_BITSET( NoutV.Seq, SPDD_BITNO,                          /* @@ SPD-D reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_SPDD ) );
	IXX_BITSET( NoutV.Seq, SPDA_BITNO,                          /* @@ SPD-A reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_SPDA ) );
	IXX_BITSET( NoutV.Seq, SPDB_BITNO,                          /* @@ SPD-B reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_SPDB ) );
	IXX_BITSET( NoutV.Seq, CSEL_BITNO,                          /* @@ C-SEL reference */
					LPX_CHECK_ON( psqiv, SQI_SIGS_CSEL ) );
/* <S03D> End */

/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア指令																			*/
/*--------------------------------------------------------------------------------------------------*/
/*	補足：ＡＬＭとＣＯＩＮをほぼ同時に出力すると、上位シーケンスがＣＯＩＮを先に見ていた場合に、	*/
/*		  シーケンスを動かしてしまうケースがあった。												*/
/*		  ＳＧＤＳではＡＬＭ発生時に、ＡＬＭ信号出力後１０ｍｓの遅延タイマ後に偏差クリアしている。	*/
/*--------------------------------------------------------------------------------------------------*/
#define	DELAY_ALM_TO_COIN	KPI_SCANN_MS( 10 )				/* ALMからCOIN出力までの遅延ﾀｲﾏ=10[ms]	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( NinV.P.PerClrMode )
	{
	/*----------------------------------------------------------------------------------------------*/
	case 0x00:		/* ベースブロック時＋CLR信号で、偏差クリア										*/
	/*----------------------------------------------------------------------------------------------*/
		if(	IXX_BITCHK( NinV.Seq, ALM_DETECTED_BITNO ) ) 	/* Alarm Detected						*/
		{
			if( psqiv->DelayAlmToCoin > DELAY_ALM_TO_COIN )	/* 遅延タイマチェック					*/
			{
			/*--------------------------------------------------------------------------------------*/
			/*	遅延タイマ経過で、TRUE																*/
			/*--------------------------------------------------------------------------------------*/
				IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, TRUE );
			}
			else
			{
				psqiv->DelayAlmToCoin++;					/* 遅延タイマカウントアップ				*/
			/*--------------------------------------------------------------------------------------*/
			/* 	遅延タイマ経過まではCLR信号に従う 													*/
			/*--------------------------------------------------------------------------------------*/
				IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, (NinV.NrmBits.CLR) );
			}
		}
		else if ( ! IXX_BITCHK( NinV.Seq, SERVO_ON_COMP_BITNO ) ) 	/* Base Blocked (not OT state)	*/
		{																				  
			/*--------------------------------------------------------------------------------------*/
			/* ベースブロック時はすぐにTRUE															*/
			/*--------------------------------------------------------------------------------------*/
			IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, TRUE );
			psqiv->DelayAlmToCoin = 0;						/* 遅延タイマクリア						*/
		}																				  
		else												/* CLR信号								*/
		{
			IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, (NinV.NrmBits.CLR) );
			psqiv->DelayAlmToCoin = 0;						/* 遅延タイマクリア						*/
		}
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x01:												/* CLR信号でのみ偏差クリア				*/
	/*----------------------------------------------------------------------------------------------*/
		IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, (NinV.NrmBits.CLR) );
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x02:												/* アラーム発生時＋CLR信号で、偏差クリア*/
	/*----------------------------------------------------------------------------------------------*/
		if(	IXX_BITCHK( NinV.Seq, ALM_DETECTED_BITNO ) ) 	/* Alarm Detected						*/
		{
			if( psqiv->DelayAlmToCoin > DELAY_ALM_TO_COIN )	/* 遅延タイマチェック					*/
			{
			/*--------------------------------------------------------------------------------------*/
			/*	遅延タイマ経過で、TRUE																*/
			/*--------------------------------------------------------------------------------------*/
				IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, TRUE );
			}
			else
			{
			/*--------------------------------------------------------------------------------------*/
			/* 遅延タイマ経過まではCLR信号に従う 													*/
			/*--------------------------------------------------------------------------------------*/
				psqiv->DelayAlmToCoin++;					/* 遅延タイマカウントアップ				*/
				IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, (NinV.NrmBits.CLR) );
			}
		}
		else												/* CLR信号								*/
		{
			IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, (NinV.NrmBits.CLR) );
			psqiv->DelayAlmToCoin = 0;						/* 遅延タイマクリア						*/
		}
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x03:												/* 偏差クリアしない(CLR信号無視)		*/
	/*----------------------------------------------------------------------------------------------*/
		IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, FALSE );
		break;
	/*----------------------------------------------------------------------------------------------*/
	default  :												/* 本ケースは無し						*/
	/*----------------------------------------------------------------------------------------------*/
		IXX_BITSET( NoutV.Ctrl.CmdCtrlBit, CLRPOSERR_BITNO, FALSE );
		break;
	}
	/*----------------------------------------------------------------------------------------------*/
	if( LPX_CHECK_CLRON( psqiv ) ) { PsnResetCmdPosAll( &(MtHdl.psnHdl), 0 ); }	/*  follow up 		*/

/*--------------------------------------------------------------------------------------------------*/
/*	sen signal <S00C> <S168>																		*/
/*--------------------------------------------------------------------------------------------------*/
//<S168>	IXX_BITSET( NoutV.Seq, SENSOR_ON_BITNO, (NinV.NrmBits.SEN) ); 		
	LpxSqiMakeSensorOn( psqiv );												/* <S168> */
/*--------------------------------------------------------------------------------------------------*/
/*  Servo On <S090> 																				*/
/*--------------------------------------------------------------------------------------------------*/
	IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, LPX_CHECK_ON( psqiv, SQI_SIGS_SON ) );
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
/*--------------------------------------------------------------------------------------------------*/
/*  Pole Detect		 																				*/
/*--------------------------------------------------------------------------------------------------*/
	IXX_BITSET( NoutV.Seq, POL_DETECT_BITNO, LPX_CHECK_ON( psqiv, SQI_SIGS_PDET ) );
#endif								/* <S21C> */
/*--------------------------------------------------------------------------------------------------*/
/*	Gain Change <S09B>																				*/
/*--------------------------------------------------------------------------------------------------*/
	NoutV.Ctrl.SelBank = (LPX_CHECK_ON( psqiv, SQI_SIGS_GSEL ) ? 1 : 0);

	return;
}

/****************************************************************************************************/
/*  																								*/
/*  Initialize Sqi 																					*/
/*  																								*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxInitSqi( 										/* Initialize Sqi 						*/
			SQIV *psqiv,  									/* Sqi Variable 						*/
			OPPRMDAT *pPrm )								/* the Parameters						*/
{
	MlibResetLongMemory( psqiv, sizeof( SQIV )/4 );
/*--------------------------------------------------------------------------------------------------*/
	IxxSqiSetLgcs( psqiv, pPrm );				    /* set input signal logics */
	IxxSqiSetMaps( psqiv, pPrm );				    /* set input signal maps */
/*--------------------------------------------------------------------------------------------------*/
	psqiv->Tlim = SCANN_DEFTRQ;						/* set initial torque limit */
/*--------------------------------------------------------------------------------------------------*/
//	psqiv->SIMode = (OpPrm.SIMode) & 0x0F;			/* PnB90.0:Signal Input Mode */
//	psqiv->AnMode = (OpPrm.SIMode>>4) & 0x0F;		/* PnB90.1:Analog Signal Input Mode */
//	psqiv->SIStopTime = KPI_SCANN_MS( 3000 );		/* Signal Input Timeout = 3000[ms] */
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*  																								*/
/*  Set sequence input signal logics																*/
/*  																								*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxSqiSetLgcs( SQIV *psqiv, OPPRMDAT *pPrm )
{
UINT wk;
#define LPX_GETLGCPRM(x) ((x) & 0x0F)
#define LPX_GETMAPPRM(x)    (((x)>>4) & 0x0F)
/*--------------------------------------------------------------------------------------------------*/
/*	Load Signal Logic Parameters																	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	psqiv->Lgc.Mode01 = LPX_GETLGCPRM( pPrm->MODE01Lgc );
	psqiv->Lgc.StartHome = LPX_GETLGCPRM( pPrm->STARTHOMELgc );
	psqiv->Lgc.PgmresJogp = LPX_GETLGCPRM( pPrm->PGMRESJOGPLgc );
	psqiv->Lgc.Sel0JogN = LPX_GETLGCPRM( pPrm->SEL0JOGNLgc );
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Lgc.Sel1Jog0 = LPX_GETLGCPRM( pPrm->SEL1JOG0Lgc );
	psqiv->Lgc.Sel2Jog1 = LPX_GETLGCPRM( pPrm->SEL2JOG1Lgc );
	psqiv->Lgc.Sel3Jog2 = LPX_GETLGCPRM( pPrm->SEL3JOG2Lgc );
	psqiv->Lgc.Sel4Jog3 = LPX_GETLGCPRM( pPrm->SEL4JOG3Lgc );
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Lgc.Sel5Jog4 = LPX_GETLGCPRM( pPrm->SEL5Lgc );
	psqiv->Lgc.Sel6Jog5 = LPX_GETLGCPRM( pPrm->SEL6Lgc );
	psqiv->Lgc.Sel7Jog6 = LPX_GETLGCPRM( pPrm->SEL7Lgc );
	/*----------------------------------------------------------------------------------------------*/
#endif
	psqiv->Lgc.Son = LPX_GETLGCPRM( pPrm->SONLgc );
	psqiv->Lgc.Ares = LPX_GETLGCPRM( pPrm->ALMRSTLgc );
//	psqiv->Lgc.Dec = LPX_GETLGCPRM( pPrm->DECLgc );
//	psqiv->Lgc.Rgrt = LPX_GETLGCPRM( pPrm->RTRGLgc );
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Lgc.Pot = LPX_GETLGCPRM( pPrm->POTLgc );
	psqiv->Lgc.Not = LPX_GETLGCPRM( pPrm->NOTLgc );
	psqiv->Lgc.Pcl = LPX_GETLGCPRM( pPrm->PCLLgc );
	psqiv->Lgc.Ncl = LPX_GETLGCPRM( pPrm->NCLLgc );
	/*----------------------------------------------------------------------------------------------*/
//	psqiv->Lgc.Clr = LPX_GETLGCPRM( pPrm->CLRLgc );	        //@@ Anlg
	psqiv->Lgc.Pcon = LPX_GETLGCPRM( pPrm->PCONLgc );	    //@@ Anlg
	psqiv->Lgc.Zclamp = LPX_GETLGCPRM( pPrm->ZCLAMPLgc );	//@@ Anlg
	psqiv->Lgc.Inhibit = LPX_GETLGCPRM( pPrm->INHIBITLgc );	//@@ Anlg
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Lgc.Psel = LPX_GETLGCPRM( pPrm->PSELLgc );	    //@@ Anlg
/* <S03D> Start */
/*--------------------------------------------------------------------------------------------------*/
	psqiv->Lgc.SpdD = LPX_GETLGCPRM( pPrm->SPDDLgc );	    //@@ Anlg
	psqiv->Lgc.SpdA = LPX_GETLGCPRM( pPrm->SPDALgc );	    //@@ Anlg
	psqiv->Lgc.SpdB = LPX_GETLGCPRM( pPrm->SPDBLgc );	    //@@ Anlg
	psqiv->Lgc.Csel = LPX_GETLGCPRM( pPrm->CSELLgc );	    //@@ Anlg
/* <S03D> End */
/*--------------------------------------------------------------------------------------------------*/
	psqiv->Lgc.Gsel = LPX_GETLGCPRM( pPrm->GSELLgc );	    /* <S09B> */
	psqiv->Lgc.Sen = LPX_GETLGCPRM( pPrm->SENLgc );	    	/* <S168> */
	psqiv->Lgc.Fstp = LPX_GETLGCPRM( pPrm->FstpLgc );	    	/* <S1F5> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	psqiv->Lgc.Pdet = LPX_GETLGCPRM( pPrm->PdetLgc );
#endif	    						/* <S21C> */

/*--------------------------------------------------------------------------------------------------*/
/*	Set active logics 																				*/
/*--------------------------------------------------------------------------------------------------*/
	wk = 0;
#if 0
	wk = ( psqiv->Lgc.Mode01 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_MODE01 ) : (wk & ~SQI_SIGS_MODE01);
	wk = ( psqiv->Lgc.StartHome == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_STARTHOME ) : (wk & ~SQI_SIGS_STARTHOME);
	wk = ( psqiv->Lgc.PgmresJogp == SQI_LGC_OPPOSITE  ) ? ( wk | SQI_SIGS_PGMRESJOGP ) : (wk & ~SQI_SIGS_PGMRESJOGP);
	wk = ( psqiv->Lgc.Sel0JogN == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL0JOGN ) : (wk & ~SQI_SIGS_SEL0JOGN);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Sel1Jog0 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL1JOG0 ) : (wk & ~SQI_SIGS_SEL1JOG0);
	wk = ( psqiv->Lgc.Sel2Jog1 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL2JOG1 ) : (wk & ~SQI_SIGS_SEL2JOG1);
	wk = ( psqiv->Lgc.Sel3Jog2 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL3JOG2 ) : (wk & ~SQI_SIGS_SEL3JOG2);
	wk = ( psqiv->Lgc.Sel4Jog3 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL4JOG3 ) : (wk & ~SQI_SIGS_SEL4JOG3);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Sel5Jog4 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL5JOG4 ) : (wk & ~SQI_SIGS_SEL5JOG4);
	wk = ( psqiv->Lgc.Sel6Jog5 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL6JOG5 ) : (wk & ~SQI_SIGS_SEL6JOG5);
	wk = ( psqiv->Lgc.Sel7Jog6 == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEL7JOG6 ) : (wk & ~SQI_SIGS_SEL7JOG6);
	/*----------------------------------------------------------------------------------------------*/
#endif
	wk = ( psqiv->Lgc.Son == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SON ) : (wk & ~SQI_SIGS_SON);
	wk = ( psqiv->Lgc.Ares == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_ALMRST) : (wk & ~SQI_SIGS_ALMRST);
//	wk = ( psqiv->Lgc.Dec == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_DEC ) : (wk & ~SQI_SIGS_DEC);
//	wk = ( psqiv->Lgc.Rgrt == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_RGRT ) : (wk & ~SQI_SIGS_RGRT);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Pot == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_POT) : (wk & ~SQI_SIGS_POT);
	wk = ( psqiv->Lgc.Not == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_NOT) : (wk & ~SQI_SIGS_NOT);
	wk = ( psqiv->Lgc.Pcl == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_PCL) : (wk & ~SQI_SIGS_PCL);
	wk = ( psqiv->Lgc.Ncl == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_NCL) : (wk & ~SQI_SIGS_NCL);
	/*----------------------------------------------------------------------------------------------*/
//	wk = ( psqiv->Lgc.Clr == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_CLR) : (wk & ~SQI_SIGS_CLR); /* @@ Anlg */
	wk = ( psqiv->Lgc.Pcon == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_PCON) : (wk & ~SQI_SIGS_PCON); /* @@ Anlg */
	wk = ( psqiv->Lgc.Zclamp == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_ZCLAMP) : (wk & ~SQI_SIGS_ZCLAMP); /* @@ Anlg */
	wk = ( psqiv->Lgc.Inhibit == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_INHIBIT) : (wk & ~SQI_SIGS_INHIBIT); /* @@ Anlg */
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Psel == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_PSEL ) : (wk & ~SQI_SIGS_PSEL ); /* @@ Anlg */
	/*----------------------------------------------------------------------------------------------*/
/* <S03D> Start */
	wk = ( psqiv->Lgc.SpdD == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SPDD ) : (wk & ~SQI_SIGS_SPDD ); /* @@ Anlg */
	wk = ( psqiv->Lgc.SpdA == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SPDA ) : (wk & ~SQI_SIGS_SPDA ); /* @@ Anlg */
	wk = ( psqiv->Lgc.SpdB == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SPDB ) : (wk & ~SQI_SIGS_SPDB ); /* @@ Anlg */
	wk = ( psqiv->Lgc.Csel == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_CSEL ) : (wk & ~SQI_SIGS_CSEL ); /* @@ Anlg */
/* <S03D> End */
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Gsel == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_GSEL ) : (wk & ~SQI_SIGS_GSEL ); /* <S09B> */
	wk = ( psqiv->Lgc.Sen == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_SEN ) : (wk & ~SQI_SIGS_SEN ); /* <S168> */
	wk = ( psqiv->Lgc.Fstp == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_FSTP ) : (wk & ~SQI_SIGS_FSTP ); /* <S1F5> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	wk = ( psqiv->Lgc.Pdet == SQI_LGC_OPPOSITE ) ? ( wk | SQI_SIGS_PDET ) : (wk & ~SQI_SIGS_PDET );
#endif								/* <S21C> */
	/*----------------------------------------------------------------------------------------------*/
	psqiv->ActvLgcs = wk;

/*--------------------------------------------------------------------------------------------------*/
/*	Set active masks 																				*/
/*--------------------------------------------------------------------------------------------------*/
	wk = 0;
#if 0
	wk = ( psqiv->Lgc.Mode01 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_MODE01 ) : (wk & ~SQI_SIGS_MODE01);
	wk = ( psqiv->Lgc.StartHome == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_STARTHOME ) : (wk & ~SQI_SIGS_STARTHOME);
	wk = ( psqiv->Lgc.PgmresJogp == SQI_LGC_ACTIVE  ) ? ( wk | SQI_SIGS_PGMRESJOGP ) : (wk & ~SQI_SIGS_PGMRESJOGP);
	wk = ( psqiv->Lgc.Sel0JogN == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL0JOGN ) : (wk & ~SQI_SIGS_SEL0JOGN);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Sel1Jog0 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL1JOG0 ) : (wk & ~SQI_SIGS_SEL1JOG0);
	wk = ( psqiv->Lgc.Sel2Jog1 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL2JOG1 ) : (wk & ~SQI_SIGS_SEL2JOG1);
	wk = ( psqiv->Lgc.Sel3Jog2 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL3JOG2 ) : (wk & ~SQI_SIGS_SEL3JOG2);
	wk = ( psqiv->Lgc.Sel4Jog3 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL4JOG3 ) : (wk & ~SQI_SIGS_SEL4JOG3);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Sel5Jog4 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL5JOG4 ) : (wk & ~SQI_SIGS_SEL5JOG4);
	wk = ( psqiv->Lgc.Sel6Jog5 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL6JOG5 ) : (wk & ~SQI_SIGS_SEL6JOG5);
	wk = ( psqiv->Lgc.Sel7Jog6 == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEL7JOG6 ) : (wk & ~SQI_SIGS_SEL7JOG6);
	/*----------------------------------------------------------------------------------------------*/
#endif
	wk = ( psqiv->Lgc.Son == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SON ) : (wk & ~SQI_SIGS_SON);
	wk = ( psqiv->Lgc.Ares == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_ALMRST) : (wk & ~SQI_SIGS_ALMRST);
//	wk = ( psqiv->Lgc.Dec == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_DEC ) : (wk & ~SQI_SIGS_DEC);
//	wk = ( psqiv->Lgc.Rgrt == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_RGRT ) : (wk & ~SQI_SIGS_RGRT);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Pot == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_POT	) : (wk & ~SQI_SIGS_POT	);
	wk = ( psqiv->Lgc.Not == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_NOT	) : (wk & ~SQI_SIGS_NOT	);
	wk = ( psqiv->Lgc.Pcl == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_PCL	) : (wk & ~SQI_SIGS_PCL	);
	wk = ( psqiv->Lgc.Ncl == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_NCL	) : (wk & ~SQI_SIGS_NCL	);
	/*----------------------------------------------------------------------------------------------*/
//	wk = ( psqiv->Lgc.Clr == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_CLR	) : (wk & ~SQI_SIGS_CLR	);
	wk = ( psqiv->Lgc.Pcon == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_PCON ) : (wk & ~SQI_SIGS_PCON );
	wk = ( psqiv->Lgc.Zclamp == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_ZCLAMP ) : (wk & ~SQI_SIGS_ZCLAMP );
	wk = ( psqiv->Lgc.Inhibit == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_INHIBIT ) : (wk & ~SQI_SIGS_INHIBIT );
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Psel == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_PSEL ) : (wk & ~SQI_SIGS_PSEL );
	/*----------------------------------------------------------------------------------------------*/
/* <S03D> Start */
	wk = ( psqiv->Lgc.SpdD == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SPDD ) : (wk & ~SQI_SIGS_SPDD );
	wk = ( psqiv->Lgc.SpdA == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SPDA ) : (wk & ~SQI_SIGS_SPDA );
	wk = ( psqiv->Lgc.SpdB == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SPDB ) : (wk & ~SQI_SIGS_SPDB );
	wk = ( psqiv->Lgc.Csel == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_CSEL ) : (wk & ~SQI_SIGS_CSEL );
/* <S03D> End */
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Gsel == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_GSEL ) : (wk & ~SQI_SIGS_GSEL ); /* <S09B> */
	wk = ( psqiv->Lgc.Sen == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_SEN ) : (wk & ~SQI_SIGS_SEN ); /* <S168> */
	wk = ( psqiv->Lgc.Fstp == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_FSTP ) : (wk & ~SQI_SIGS_FSTP ); /* <S1F5> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	wk = ( psqiv->Lgc.Pdet == SQI_LGC_ACTIVE ) ? ( wk | SQI_SIGS_PDET ) : (wk & ~SQI_SIGS_PDET );
#endif								/* <S21C> */
	/*----------------------------------------------------------------------------------------------*/
	psqiv->ActvMask = wk;

/*--------------------------------------------------------------------------------------------------*/
/*	Set none active masks 																			*/
/*--------------------------------------------------------------------------------------------------*/
	wk = 0;
#if 0
	wk = ( psqiv->Lgc.Mode01 == SQI_LGC_NONACTIVE) ? ( wk & ~SQI_SIGS_MODE01 ) : (wk | SQI_SIGS_MODE01);
	wk = ( psqiv->Lgc.StartHome == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_STARTHOME ) : (wk | SQI_SIGS_STARTHOME);
	wk = ( psqiv->Lgc.PgmresJogp == SQI_LGC_NONACTIVE  ) ? ( wk & ~SQI_SIGS_PGMRESJOGP ) : (wk | SQI_SIGS_PGMRESJOGP);
	wk = ( psqiv->Lgc.Sel0JogN == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL0JOGN ) : (wk | SQI_SIGS_SEL0JOGN);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Sel1Jog0 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL1JOG0 ) : (wk | SQI_SIGS_SEL1JOG0);
	wk = ( psqiv->Lgc.Sel2Jog1 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL2JOG1 ) : (wk | SQI_SIGS_SEL2JOG1);
	wk = ( psqiv->Lgc.Sel3Jog2 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL3JOG2 ) : (wk | SQI_SIGS_SEL3JOG2);
	wk = ( psqiv->Lgc.Sel4Jog3 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL4JOG3 ) : (wk | SQI_SIGS_SEL4JOG3);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Sel5Jog4 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL5JOG4 ) : (wk | SQI_SIGS_SEL5JOG4);
	wk = ( psqiv->Lgc.Sel6Jog5 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL6JOG5 ) : (wk | SQI_SIGS_SEL6JOG5);
	wk = ( psqiv->Lgc.Sel7Jog6 == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEL7JOG6 ) : (wk | SQI_SIGS_SEL7JOG6);
	/*----------------------------------------------------------------------------------------------*/
#endif
	wk = ( psqiv->Lgc.Son == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SON ) : (wk | SQI_SIGS_SON);
	wk = ( psqiv->Lgc.Ares == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_ALMRST) : (wk | SQI_SIGS_ALMRST);
//	wk = ( psqiv->Lgc.Dec == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_DEC ) : (wk | SQI_SIGS_DEC);
//	wk = ( psqiv->Lgc.Rgrt == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_RGRT ) : (wk | SQI_SIGS_RGRT);
	/*----------------------------------------------------------------------------------------------*/
	/* P-OT,N-OT,P-CL,N-CL                                                                          */
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Pot == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_POT ) : (wk | SQI_SIGS_POT);
	wk = ( psqiv->Lgc.Not == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_NOT ) : (wk | SQI_SIGS_NOT);
	wk = ( psqiv->Lgc.Pcl == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_PCL ) : (wk | SQI_SIGS_PCL);
	wk = ( psqiv->Lgc.Ncl == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_NCL ) : (wk | SQI_SIGS_NCL);
	/*----------------------------------------------------------------------------------------------*/
//	wk = ( psqiv->Lgc.Clr == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_CLR ) : (wk | SQI_SIGS_CLR);
	wk = ( psqiv->Lgc.Pcon == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_PCON ) : (wk | SQI_SIGS_PCON);
	wk = ( psqiv->Lgc.Zclamp == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_ZCLAMP ) : (wk | SQI_SIGS_ZCLAMP);
	wk = ( psqiv->Lgc.Inhibit == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_INHIBIT ) : (wk | SQI_SIGS_INHIBIT);
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Psel == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_PSEL ) : (wk | SQI_SIGS_PSEL);
	/*----------------------------------------------------------------------------------------------*/
/* <S03D> Start */
	wk = ( psqiv->Lgc.SpdD == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SPDD ) : (wk | SQI_SIGS_SPDD);
	wk = ( psqiv->Lgc.SpdA == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SPDA ) : (wk | SQI_SIGS_SPDA);
	wk = ( psqiv->Lgc.SpdB == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SPDB ) : (wk | SQI_SIGS_SPDB);
	wk = ( psqiv->Lgc.Csel == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_CSEL ) : (wk | SQI_SIGS_CSEL);
/* <S03D> End */
	/*----------------------------------------------------------------------------------------------*/
	wk = ( psqiv->Lgc.Gsel == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_GSEL ) : (wk | SQI_SIGS_GSEL); /* <S09B> */
	wk = ( psqiv->Lgc.Sen == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_SEN ) : (wk | SQI_SIGS_SEN); /* <S168> */
	wk = ( psqiv->Lgc.Fstp == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_FSTP ) : (wk | SQI_SIGS_FSTP); /* <S1F5> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	wk = ( psqiv->Lgc.Pdet == SQI_LGC_NONACTIVE ) ? ( wk & ~SQI_SIGS_PDET ) : (wk | SQI_SIGS_PDET);
#endif								/* <S21C> */
	/*----------------------------------------------------------------------------------------------*/
	psqiv->NonActvMask = wk;
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*  																								*/
/*  Set sequence input signal maps																	*/
/*  																								*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxSqiSetMaps( SQIV *psqiv, OPPRMDAT *pPrm )
{
/*--------------------------------------------------------------------------------------------------*/
//	psqiv->Map.Mode01 = LPX_GETMAPPRM( pPrm->MODE01Lgc );
//	psqiv->Map.StartHome = LPX_GETMAPPRM( pPrm->STARTHOMELgc );
//	psqiv->Map.PgmresJogp = LPX_GETMAPPRM( pPrm->PGMRESJOGPLgc );
//	psqiv->Map.Sel0JogN = LPX_GETMAPPRM( pPrm->SEL0JOGNLgc );
	/*----------------------------------------------------------------------------------------------*/
//	psqiv->Map.Sel1Jog0 = LPX_GETMAPPRM( pPrm->SEL1JOG0Lgc );
//	psqiv->Map.Sel2Jog1 = LPX_GETMAPPRM( pPrm->SEL2JOG1Lgc );
//	psqiv->Map.Sel3Jog2 = LPX_GETMAPPRM( pPrm->SEL3JOG2Lgc );
//	psqiv->Map.Sel4Jog3 = LPX_GETMAPPRM( pPrm->SEL4JOG3Lgc );
	/*----------------------------------------------------------------------------------------------*/
//	psqiv->Map.Sel5Jog4 = LPX_GETMAPPRM( pPrm->SEL5Lgc );
//	psqiv->Map.Sel6Jog5 = LPX_GETMAPPRM( pPrm->SEL6Lgc );
//	psqiv->Map.Sel7Jog6 = LPX_GETMAPPRM( pPrm->SEL7Lgc );
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Map.Son = LPX_GETMAPPRM( pPrm->SONLgc );
	psqiv->Map.Ares = LPX_GETMAPPRM( pPrm->ALMRSTLgc );
//	psqiv->Map.Dec = LPX_GETMAPPRM( pPrm->DECLgc );
//	psqiv->Map.Rgrt = LPX_GETMAPPRM( pPrm->RTRGLgc );
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Map.Pot = LPX_GETMAPPRM( pPrm->POTLgc );
	psqiv->Map.Not = LPX_GETMAPPRM( pPrm->NOTLgc );
	psqiv->Map.Pcl = LPX_GETMAPPRM( pPrm->PCLLgc );
	psqiv->Map.Ncl = LPX_GETMAPPRM( pPrm->NCLLgc );
	/*----------------------------------------------------------------------------------------------*/
//	psqiv->Map.Clr = LPX_GETMAPPRM( pPrm->CLRLgc );	        		/* @@ Anlg */
	psqiv->Map.Pcon = LPX_GETMAPPRM( pPrm->PCONLgc );	    		/* @@ Anlg */
	psqiv->Map.Zclamp = LPX_GETMAPPRM( pPrm->ZCLAMPLgc );			/* @@ Anlg */
	psqiv->Map.Inhibit = LPX_GETMAPPRM( pPrm->INHIBITLgc );			/* @@ Anlg */
	/*----------------------------------------------------------------------------------------------*/
	psqiv->Map.Psel = LPX_GETMAPPRM( pPrm->PSELLgc );	    		/* @@ Anlg */

/* <S03D> Start */
	/*--------------------------------------------------------------------------------------------------*/
	psqiv->Map.SpdD = LPX_GETMAPPRM( pPrm->SPDDLgc );			/* @@ Anlg */
	psqiv->Map.SpdA = LPX_GETMAPPRM( pPrm->SPDALgc );			/* @@ Anlg */
	psqiv->Map.SpdB = LPX_GETMAPPRM( pPrm->SPDBLgc );			/* @@ Anlg */
	psqiv->Map.Csel = LPX_GETMAPPRM( pPrm->CSELLgc );			/* @@ Anlg */
/* <S03D> End */
	/*--------------------------------------------------------------------------------------------------*/
	psqiv->Map.Gsel = LPX_GETMAPPRM( pPrm->GSELLgc );			/* <S09B> */
	psqiv->Map.Sen = LPX_GETMAPPRM( pPrm->SENLgc );				/* <S168> */
	psqiv->Map.Fstp = LPX_GETMAPPRM( pPrm->FstpLgc );				/* <S1F5> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	psqiv->Map.Pdet = LPX_GETMAPPRM( pPrm->PdetLgc );
#endif								/* <S21C> */
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*  																								*/
/*  Sequence Input Signal State Machine																*/
/*  																								*/
/****************************************************************************************************/
void	LpxSqiSequence( SQIV *psqiv, JSPDT *pJspdCache, struct _token *nmsg )					
{
LONG seni;
UCHAR CtrMode;	/* <S03D> */
	switch( psqiv->SqiSeq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case SQSQI_INITIAL:												/* 0: Initial step */
/*--------------------------------------------------------------------------------------------------*/
/* 絶対値エンコーダ対応 <S00C> */
		if( IXX_BITCHK( NinV.Seq, SENS_ON_COMP_BITNO ) )			/*(NinV.Seq.SensOnComp)*/
		{ 
/*--------------------------------------------------------------------------------------------------*/
			PsnResetApos( &(MtHdl.psnHdl), TRUE);                   /* reset APOS */
//Mercury	IxxSetOriginPos( &Npos, Npos.HomePos );                 /* set origin position */
			PsnSetRefPoint( &(MtHdl.psnHdl), !(NoutV.HomeUse) );    /* reset ref point */
/*--------------------------------------------------------------------------------------------------*/
			nmsg->Channel = CHANNEL_AXCTRL;                         /* update motion */
//Mercury	IxxAxctrlReqFollowUp( &(nmsg->fmt.CmdData) );           /* follow up request */
			IxxAxctrlReqSkip( &(nmsg->fmt.CmdData), MT_HOLD_FASTSTOP );  /* hold request */
			psqiv->SqiSeq = SQSQI_BB;                               /* down below */
		} 
		else
		{
//<S00C>			IXX_BITSET( NoutV.Seq, SENSOR_ON_BITNO, TRUE );         /* sensor on */
			nmsg->Channel = CHANNEL_AXCTRL;                         /* update motion */
			nmsg->fmt.CmdData.CtrlMode = NO_CTRL_MODE;              /* disable control mode */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case SQSQI_BB:													/* 1: Base block state */
/*--------------------------------------------------------------------------------------------------*/
		if( ! IXX_BITCHK( NinV.Seq, SENS_ON_COMP_BITNO ) )			/* sensor off <S00C> */
		{ 
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );         /* servo off */
			nmsg->Channel = CHANNEL_AXCTRL;                         /* update motion */
			nmsg->fmt.CmdData.CtrlMode = NO_CTRL_MODE;              /* disable control mode */
			psqiv->SqiSeq = SQSQI_INITIAL;                          /* initial step */
		}
		else
	/*----------------------------------------------------------------------------------------------*/
		if ( IXX_BITCHK( NinV.Seq, SERVO_ON_COMP_BITNO ))           /* <S090> */
		{
			psqiv->SqiSeq = SQSQI_RUN;
		}
#if 0	/* <S090> */
		if(	IXX_BITCHK( NinV.Seq, ALM_DETECTED_BITNO ) ) 
		{															/* check alarm */
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );         /* servo off */
			psqiv->SqiSeq = SQSQI_WAITALMRES ;                      /* wait alarm reset */
			break;
		}
		else if ( LPX_CHECK_ON( psqiv, SQI_SIGS_SON ) )             /* check servo on ref */
		{
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, TRUE );          /* make servo on reference */
			psqiv->SqiSeq = SQSQI_WAITSVON ;                        /* wait servo on state */
			psqiv->SonRefTimeCnt = 0;                               /* reset counter */
			break;
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
		LpxCheckSvonNotYetError( psqiv );                           /* Check E54E:Servo on not yet  */
/*--------------------------------------------------------------------------------------------------*/
/*		Mercury: Set Analog Reference 																*/
/*--------------------------------------------------------------------------------------------------*/
/* <S03D> Start */
		//IXX_ANLG_REQX( NoutV.CtrlModeA,  NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg );
		if(NoutV.CtrlModeA != NoutV.CtrlModeB )
		{
			CtrMode = GetControlMode( );
		}
		else
		{
			CtrMode = NoutV.CtrlModeA;
		}
		IXX_ANLG_REQX( CtrMode,  NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg );
/* <S03D> End */
/*--------------------------------------------------------------------------------------------------*/
		break;
#if 0	/* <S090> */
/*--------------------------------------------------------------------------------------------------*/
	case SQSQI_WAITSVON:											/* 2: Wait servo on state */
/*--------------------------------------------------------------------------------------------------*/
		if( ! IXX_BITCHK( NinV.Seq, SENS_ON_COMP_BITNO ) )			/* sensor off <S00C> */
		{ 
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );         /* servo off */
			nmsg->Channel = CHANNEL_AXCTRL;                         /* update motion */
			nmsg->fmt.CmdData.CtrlMode = NO_CTRL_MODE;              /* disable control mode */
			psqiv->SqiSeq = SQSQI_INITIAL;                          /* initial step */
		}
		else
/*--------------------------------------------------------------------------------------------------*/
		if ( (++(psqiv->SonRefTimeCnt)) >= KPI_SCANN_MS( 2000 ) )
		{
			IXX_ERRORSET( ERRCODE_SVONFAIL );                       /* E55E Servo on fail */
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );         /* make servo  off reference */
			psqiv->SqiSeq = SQSQI_WAITALMRES;                       /* wait alm reset state */
		}
		else if ( ! IXX_BITCHK( NinV.Seq, SERVO_ON_COMP_BITNO ) ) 
		{ 
			LpxCheckSvonNotYetError( psqiv );                       /* Check E54E:Servo on not yet */
			break;                                                  /* break until servo is on */
		}
		else
		{
			nmsg->Channel = CHANNEL_AXCTRL;                              /* update motion */
			IxxAxctrlReqSkip( &(nmsg->fmt.CmdData), MT_HOLD_FASTSTOP );  /* hold request */
			psqiv->SqiSeq = SQSQI_RUN;                                   /* run state */
		}
		break;
#endif
/*--------------------------------------------------------------------------------------------------*/
	case SQSQI_RUN:			/* 3: Run state */
/*--------------------------------------------------------------------------------------------------*/
		if (! IXX_BITCHK( NinV.Seq, SERVO_ON_COMP_BITNO ))               /* <S090> */
		{
			psqiv->SqiSeq = SQSQI_BB;
		}
#if 0	/* <S090> */
		if ( IXX_BITCHK( NinV.Seq, ALM_DETECTED_BITNO )                  /*( NinV.Seq.AlmDetected ) */
			 || ( LPX_CHECK_OFF( psqiv, SQI_SIGS_SON ) )
		     || (! IXX_BITCHK( NinV.Seq, SERVO_ON_COMP_BITNO )) )
		{                                                                /* Alerm detected */
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );              /* Servo off */
/* <S03D> Start */
			//IXX_ANLG_REQX( NoutV.CtrlModeA,  NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg );
			if(NoutV.CtrlModeA != NoutV.CtrlModeB )
			{
				CtrMode = GetControlMode( );
			}
			else
			{
				CtrMode = NoutV.CtrlModeA;
			}
			IXX_ANLG_REQX( CtrMode,  NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg );
/* <S03D> End */

/*--------------------------------------------------------------------------------------------------*/
			/* 
			 * TODO: ｾﾞﾛ速停止中のﾄﾙｸ制限仕様をどうするか検討する 
			 */
/*--------------------------------------------------------------------------------------------------*/
			NoutV.ExeMode = EMODE_NOCTRL;                       /* no control */
/*--------------------------------------------------------------------------------------------------*/
			psqiv->SqiSeq = SQSQI_WAITALMRES;
		}
		else 
#endif
		{
			NoutV.ExeMode = EMODE_ANLG;                         /* Analog Mode */
		/* -------------------------------------------------------------------------------------
		 * @@ Anlg
		 * -------------------------------------------------------------------------------------*/
/* <S03D> Start */
			//IXX_ANLG_REQX( NoutV.CtrlModeA, NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg );
			if(NoutV.CtrlModeA != NoutV.CtrlModeB )
			{
				CtrMode = GetControlMode( );
			}
			else
			{
				CtrMode = NoutV.CtrlModeA;
			}
			IXX_ANLG_REQX( CtrMode, NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg );
/* <S03D> End */
		}
		break;
#if 0	/*	<S090> */
/*--------------------------------------------------------------------------------------------------*/
	case SQSQI_ZSTOP:		/* 4: Emergency stop state */ 
/*--------------------------------------------------------------------------------------------------*/
		if( IXX_BITCHK( NinV.Ctrl.RspCtrlBit, MOTOR_MOVING_BITNO ) )
		{
			break;
		}
		psqiv->SqiSeq = SQSQI_WAITALMRES;                       /* wait alerm reset state */
		                                                        /* down below */
/*--------------------------------------------------------------------------------------------------*/
	case SQSQI_WAITALMRES:  /* 5: Wait alerm reset state */
/*--------------------------------------------------------------------------------------------------*/
		if( ! IXX_BITCHK( NinV.Seq, SENS_ON_COMP_BITNO ) )			/* sensor off <S00C> */
		{ 
			IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );         /* servo off */
			nmsg->Channel = CHANNEL_AXCTRL;                         /* update motion */
			nmsg->fmt.CmdData.CtrlMode = NO_CTRL_MODE;              /* disable control mode */
			psqiv->SqiSeq = SQSQI_INITIAL;                          /* initial step */
		}
		else
/*--------------------------------------------------------------------------------------------------*/
		/*--------------------------------------------------------------------------------------*/
		/* ** BaseBlockへの遷移条件は検討中 **                                                  */
		/* とりあえず、モータ停止 && アラームなし && SVON信号オフ                               */
		/*--------------------------------------------------------------------------------------*/
		if(	(! IXX_BITCHK( NinV.Seq, ALM_DETECTED_BITNO ) ) 
               && ( LPX_CHECK_OFF( psqiv, SQI_SIGS_SON ) ) ) 
		{
			psqiv->SqiSeq = SQSQI_BB;					        /* base block state */
		}
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;
		}
/*--------------------------------------------------------------------------------------------------*/
/* <S03D> Start */
		//IXX_ANLG_REQX( NoutV.CtrlModeA,  NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg ); /* Mercury */ 
		if(NoutV.CtrlModeA != NoutV.CtrlModeB )
		{
			CtrMode = GetControlMode( );
		}
		else
		{
			CtrMode = NoutV.CtrlModeA;
		}
		IXX_ANLG_REQX( CtrMode,  NinV.Ad1, NinV.Ad2, NinV.PcmdRingCntr, nmsg ); /* Mercury */ 
/* <S03D> End */

		break;
#endif
/*--------------------------------------------------------------------------------------------------*/
	default:				/* ?: Sequence error */
/*--------------------------------------------------------------------------------------------------*/
		IXX_BITSET( NoutV.Seq, SERVO_ON_BITNO, FALSE );     /* servo off */
		break;
	}
	return;
}


/****************************************************************************************************/
/*  																								*/
/* Translate Input Signal Logic	*/
/*  																								*/
/****************************************************************************************************/
void	LpxTranslateSig( SQIV *psqiv )
{
SQI_SIGS wk;
/* <S03D> Start */
//	NINV *pninv = &NinV;	/* set signal assign mode(=Pn50A.0の値) */
//	UINT p_con;		/* P-CON */
/* <S03D> End */
/*--------------------------------------------------------------------------------------------------*/
	wk = LpxGetSigs( psqiv, 0 );        /* get signals (negative logic) */
	psqiv->PhsSig = wk;                 /* save physical input signal */
	wk = wk ^ psqiv->ActvLgcs;          /* convert active logics */
	wk = wk |= psqiv->ActvMask;         /* set active logic masks */
	wk = wk &= psqiv->NonActvMask;      /* clear none active signals */
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	psqiv->LastSigs = psqiv->Sigs;		/* save last signals */
	psqiv->Sigs = wk;					/* set current signals */
/*--------------------------------------------------------------------------------------------------*/

/* <S03D> Start */
	/* 入力信号割付モード取得 */
/* <S162>  */
//	if(pninv->P.AssignMode == SQI_ASSIGN_DEFAULT)						/* Pn50A.0 = 0 */
//	{
//		p_con = ((psqiv->Sigs  & SQI_SIGS_PCON ) >> SQI_BNO_PCON);
//
//		if( NoutV.CtrlModeSel == 0x0A)	/* 制御モード = 速度制御 ⇔ ゼロクランプ付き速度制御(Pn000.1 = A) */
//		{
//			/* P-CON信号の状態をZCLAMP信号の状態とする。 */
//			//psqiv->Sigs = (( p_con ) ? (psqiv->Sigs | SQI_SIGS_ZCLAMP) : psqiv->Sigs);
//			psqiv->Sigs = (( p_con ) ? (psqiv->Sigs | SQI_SIGS_ZCLAMP) : (psqiv->Sigs & ~SQI_SIGS_ZCLAMP));
//		}
//		else if( NoutV.CtrlModeSel == 0x0B)	/* 制御モード = 位置制御 ⇔ 指令パルス阻止機能付き位置制御(Pn000.1 = B) */
//		{
//			/* P-CON信号の状態をINHIBIT信号の状態とする。 */
//			psqiv->Sigs = (( p_con ) ? (psqiv->Sigs | SQI_SIGS_INHIBIT) : (psqiv->Sigs & ~SQI_SIGS_INHIBIT));
//		}
//	}

/* <S03D> End */

	return;
}


/****************************************************************************************************/
/*  																								*/
/* Get Input Signals 																				*/
/*  																								*/
/****************************************************************************************************/
SQI_SIGS	LpxGetSigs( SQIV *psqiv, UINT logic )
{
#define LPX_GET_SIGBITX( hwsin, mapbit, sqibit )  (((ULONG)1 & ((hwsin) >> (mapbit))) << (sqibit))
#define LPX_GET_SIGBITB( hwsin, mapbit, sqibit )  (((ULONG)1 & (1 ^ ((hwsin) >> (mapbit)))) << (sqibit))  /* B接点用(OT,etc) */
SQI_SIGS wk;
int mapx;
ULONG  hwsin;
UINT mask;
/*--------------------------------------------------------------------------------------------------*/
	wk = 0;
	mask = (logic) ? 0x00 : 0xFFFFFFFF;
	hwsin = NinV.Hwsin ^ mask;										/* SEL1-SEL4, SI0-SI4 */
/*--------------------------------------------------------------------------------------------------*/
/*  Remap signals 																					*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	mapx = psqiv->Map.Mode01;    wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_MODE01 );
	mapx = psqiv->Map.StartHome; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_STARTHOME );
	mapx = psqiv->Map.PgmresJogp; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_PGMRESJOGP );
	mapx = psqiv->Map.Sel0JogN; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL0JOGN );
	/*----------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.Sel1Jog0; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL1JOG0 );
	mapx = psqiv->Map.Sel2Jog1; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL2JOG1 );
	mapx = psqiv->Map.Sel3Jog2; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL3JOG2 );
	mapx = psqiv->Map.Sel4Jog3; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL4JOG3 );
	/*----------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.Sel5Jog4; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL5JOG4 );
	mapx = psqiv->Map.Sel6Jog5; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL6JOG5 );
	mapx = psqiv->Map.Sel7Jog6; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEL7JOG6 );
	/*----------------------------------------------------------------------------------------------*/
#endif
	mapx = psqiv->Map.Son; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SON );
	mapx = psqiv->Map.Ares; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_ALMRST );
//<S00C>	mapx = psqiv->Map.Dec; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_DEC );
//<S00C>	mapx = psqiv->Map.Rgrt; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_RGRT );
	/*----------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.Pot; wk |= LPX_GET_SIGBITB( hwsin, mapx, SQI_BNO_POT );			/* @@ Anlg */
	mapx = psqiv->Map.Not; wk |= LPX_GET_SIGBITB( hwsin, mapx, SQI_BNO_NOT );			/* @@ Anlg */
	mapx = psqiv->Map.Pcl; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_PCL );
	mapx = psqiv->Map.Ncl; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_NCL );
	/*----------------------------------------------------------------------------------------------*/
//	mapx = psqiv->Map.Clr; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_CLR );	        /* @@ Anlg */
	/*----------------------------------------------------------------------------------------------*/
	/*	Mercury 																					*/
	/*----------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.Pcon; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_PCON );	        /* @@ Anlg */
	mapx = psqiv->Map.Zclamp; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_ZCLAMP );	    /* @@ Anlg */
	mapx = psqiv->Map.Inhibit; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_INHIBIT );	/* @@ Anlg */
	/*----------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.Psel; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_PSEL );	        /* @@ Anlg */
/* <S03D> Start */
	/*--------------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.SpdD; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SPDD );	        /* @@ Anlg */
	mapx = psqiv->Map.SpdA; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SPDA );	        /* @@ Anlg */
	mapx = psqiv->Map.SpdB; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SPDB );	        /* @@ Anlg */
	mapx = psqiv->Map.Csel; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_CSEL );	        /* @@ Anlg */
/* <S03D> End */
	/*--------------------------------------------------------------------------------------------------*/
	mapx = psqiv->Map.Gsel; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_GSEL );	        /* <S09B> */
	mapx = psqiv->Map.Sen; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_SEN );	        /* <S168> */
	mapx = psqiv->Map.Fstp; wk |= LPX_GET_SIGBITB( hwsin, mapx, SQI_BNO_FSTP );	        /* <S1F5> */
//#if CSW_BOARD_TYPE == SGD7S_TYPE	/* <S21C> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE	/* <S226> */
	mapx = psqiv->Map.Pdet; wk |= LPX_GET_SIGBITX( hwsin, mapx, SQI_BNO_PDET );
#endif								/* <S21C> */
	return wk;
}


/****************************************************************************************************/
/*  																								*/
/* Get Current Input Signals 																		*/
/*  																								*/
/****************************************************************************************************/
UINT	IxxSqiGetSigs( void )
{
	return ( SqiV.Sigs );
}



/* <S03D> Start */
/****************************************************************************************************/
/*  																								*/
/*  Get Control Mode 																				*/
/*  																								*/
/****************************************************************************************************/
UCHAR GetControlMode( void )
{

	UINT p_con;		/* P-CON */
	UINT p_cl;		/* P-CL */
	UINT p_nl;		/* N-CL */
	UINT c_sel;		/* C-SEL */

	UCHAR CtrMode;			/* 実行制御モード */

	NINV *pninv = &NinV;	/* set signal assign mode(=Pn50A.0) */

	/* 入力信号割付モード取得 */
	if(pninv->P.AssignMode == SQI_ASSIGN_DEFAULT)						/* Pn50A.0 = 0 */
	{
		//p_con = ((SqiV.Sigs & SQI_SIGS_PCON ) >> SQI_BNO_PCON);		/* P-CON */
		p_con = ((SqiV.Sigs & SQI_SIGS_CSEL ) >> SQI_BNO_CSEL);						/* <S162>  */

		if( NoutV.CtrlModeA == ANLG_REQ_INTEVELCTRL )					/* Pm000.1 = 4～6:内部設定速度との制御モード切り替え */
		{
			//p_cl = ((SqiV.Sigs & SQI_SIGS_PCL ) >> SQI_BNO_PCL);		/* P-CL */
			//p_nl = ((SqiV.Sigs & SQI_SIGS_NCL ) >> SQI_BNO_NCL);		/* N-CL */
			p_cl = ((SqiV.Sigs & SQI_SIGS_SPDA ) >> SQI_BNO_SPDA );					/* <S162>  */
			p_nl = ((SqiV.Sigs & SQI_SIGS_SPDB ) >> SQI_BNO_SPDB );					/* <S162>  */

			if( ((p_cl<<1) | p_nl) != 0 )	/* P-CL = ON or N-CL = ON  */
			{
				CtrMode = NoutV.CtrlModeA;
			}
			else							/* P-CL = OFF and N-CL = OFF */
			{
				CtrMode = NoutV.CtrlModeB;
			}
			
		}
		else	/* Pm000.1 = 7～B:内部設定速度以外との制御モード切り替え */
		{
			if( p_con )	/* P-CON = ON(閉) */
			{
				CtrMode = NoutV.CtrlModeB;
			}
			else	/* P-CON = OFF(開) */
			{
				CtrMode = NoutV.CtrlModeA;
			}
		}
	}
	else						/* Pn50A.0 = 1 */
	{
		c_sel = ((SqiV.Sigs & SQI_SIGS_CSEL ) >> SQI_BNO_CSEL);	/* C-SEL */

		if( c_sel )	/* C-CEL = ON(閉) */
		{
			CtrMode = NoutV.CtrlModeB;
		}
		else	/* C-CEL = OFF(開) */
		{
			CtrMode = NoutV.CtrlModeA;
		}
	}

	return(CtrMode);

}
/* <S03D> End */

/****************************************************************************************************/
/*  																								*/
/* <S168> Make Sensor On Reference 																	*/
/*  																								*/
/****************************************************************************************************/
void	 LpxSqiMakeSensorOn( SQIV *psqiv )
{
UINT sen1, sen2;
NINV *pninv = &NinV;
/*--------------------------------------------------------------------------------------------------*/
/*	sen1 : CN1-4																					*/
/*	sen2 : CN1-40 ~ CN1-46																			*/
/*--------------------------------------------------------------------------------------------------*/
	sen1 = (((pninv->NrmBits.SENI) && (psqiv->Lgc.Sen == SQI_LGC_NONACTIVE)) ? TRUE : FALSE);
	sen2 = (LPX_CHECK_ON( psqiv, SQI_SIGS_SEN ) ? TRUE : FALSE);
/*--------------------------------------------------------------------------------------------------*/
/*	Output Reference																				*/
/*--------------------------------------------------------------------------------------------------*/
	IXX_BITSET( NoutV.Seq, SENSOR_ON_BITNO, ((sen1 | sen2) | (! (pninv->P.AbsUse))) );
/*--------------------------------------------------------------------------------------------------*/
	return;
}
