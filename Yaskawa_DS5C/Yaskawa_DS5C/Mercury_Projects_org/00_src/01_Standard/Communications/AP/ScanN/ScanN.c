/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanN.c : ScanN Main Module																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. ScanN main program and initializer.														*/
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
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"



/****************************************************************************************************/
/*																									*/
/*		ScanN Function Prototypes																	*/
/*																									*/
/****************************************************************************************************/
void	NpiScanNMain( SV_OPTION_IF *psvif );		/* ScanN Main Function							*/
void	NpiScanNInitMain( SV_OPTION_IF *psvif );	/* ScanN Initialize Main Function				*/
void	NpiScanNPrstMain( SV_OPTION_IF *psvif );	/* ScanN ParamReset Main Function				*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Local Functions 																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Position management                                                                             */
/*--------------------------------------------------------------------------------------------------*/
void	LpxInitPos( struct _pos *pNpos, OPPRMDAT *pPrm );       /* Initialize Position              */
/*--------------------------------------------------------------------------------------------------*/
void	LpxSetPosLinear ( struct _pos *pNpos, long pgcnt, long pos, long *dstpos );
#define LpxSetPosCircle( x1, x2, x3, x4 ) LpxSetPosLinear ( x1, x2, x3, x4 )
/*--------------------------------------------------------------------------------------------------*/
void	LpxSetDstLinear ( struct _pos *pNpos, long tpos, long *tdst );
#define	LpxSetDstCircleShort ( x1, x2, x3 ) LpxSetDstLinear( x1, x2, x3 )
#define	LpxSetDstCircleP( x1, x2, x3 ) LpxSetDstLinear( x1, x2, x3 )
#define	LpxSetDstCircleN( x1, x2, x3 ) LpxSetDstLinear( x1, x2, x3 )

/*--------------------------------------------------------------------------------------------------*/
void	LpxInitNetMessage( MSGIF *Hmsg );						/* 暫定 */

/* 外部関数 */
extern LONG IxiLoadInitPrm( MTHNDL *mthndl, NMSG *Nmsg );

/****************************************************************************************************/
/*																									*/
/*		ScanN Variables Definition																	*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		ScanN Common Variables																		*/
/*--------------------------------------------------------------------------------------------------*/
//		TODO: Multi Axis
NINV	NinV;									/* ScanN Input  Variables							*/
NOUTV	NoutV;									/* ScanN Output Variables							*/
struct _token Nmsg;                             /* ScanN message token (Motion)                     */
struct _pos Npos;                               /* ScanN position                                   */
OPPRMDAT OpPrm[SCANN_N_AXIS];					/* Option Parameter									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Indexer Option Module Variables																*/
/*--------------------------------------------------------------------------------------------------*/
//		TODO: Multi Axis
SQIV	SqiV;									/* SQI variable 									*/
SQOV	SqoV;									/* SQO variable										*/
AXCTRLV AxctrlV;								/* Axctrl variable                                  */
ANLGV	AnlgV;									/* Anlg variable									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Indexer Table Cache Variables																*/
/*--------------------------------------------------------------------------------------------------*/
#define PgmCache  (*(PGMT *)0)					/* TODO: Program Table Cache                        */
#define ZoneCache (*(ZONET *)0)					/* TODO: Zone Table Cache                           */
#define JspdCache (*(JSPDT *)0)					/* TODO: Jog Speed Table Cache                      */

/*--------------------------------------------------------------------------------------------------*/
/*		Motion Compornent Valiables																	*/
/*--------------------------------------------------------------------------------------------------*/
MTHNDL	MtHdl;									/* Motion handle */

/****************************************************************************************************/
/*																									*/
/*		ScanN Main Program																			*/
/*																									*/
/****************************************************************************************************/
void	NpiScanNMain( SV_OPTION_IF *psvif )
{
/*--------------------------------------------------------------------------------------------------*/
		IXX_ERRORRESET();                           /* Reset Error */
/*--------------------------------------------------------------------------------------------------*/
		NpxInputMain( psvif, &NinV, &Npos );		/* ScanN Input Main Function 					*/
/*--------------------------------------------------------------------------------------------------*/
		if ( 1 )                                    /* TODO: サーボ同期確立前の処理検討 */ 
		{ 
			Nmsg.Channel = CHANNEL_SQI;             /* Set default channel */
		}
		else
		{
			Nmsg.Channel = CHANNEL_NULL;            /* NULL Channel */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Indexer Motion Control Application Procedure												*/
/*--------------------------------------------------------------------------------------------------*/
		IxxSqiMain( &SqiV, &JspdCache, &Nmsg );				/* Sequence signal input main function 	*/
		IxxAnlgMain( &AnlgV, &Nmsg );						/* Analog Reference Interface			*/
		IxxIntprtMain( &IntprtV, &PgmCache, &Nmsg, &Npos ); /* Program Table Interpreter    		*/
		IxxAxctrlMain( &MtHdl, &Nmsg );						/* Motion control main function 		*/
/*--------------------------------------------------------------------------------------------------*/
/*      Other Procedure                                                                             */
/*--------------------------------------------------------------------------------------------------*/
		IxxPgmSequence( &PgmV, &PgmCache );			/* Program table Store/Init sequence            */
		IxxZoneSequence( &ZoneV, &ZoneCache );      /* Zone table Store/Init sequence               */
		IxxJspdSequence( &JspdV, &JspdCache );      /* Jspd table Store/Init sequence               */
/*--------------------------------------------------------------------------------------------------*/
		IxxSqoMain( &SqoV, &ZoneCache, &Npos );		/* Sequence signal output main function 		*/
/*--------------------------------------------------------------------------------------------------*/
		IxxAlmManager( &OpAlmV );                   /* Indexer Alarm Manager                        */
/*--------------------------------------------------------------------------------------------------*/
		NpxOutputMain( psvif, &NoutV );				/* ScanN Output Main Function					*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		ScanN Initialize Main Program																*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	NpiScanNInitMain( SV_OPTION_IF *psvif )
{
UINT rv;
LONG AxisNo = 0;				/* @@ */
NMSG	*pmsg = &Nmsg;
OPPRMDAT *pPrm = &(OpPrm[AxisNo]);
/*--------------------------------------------------------------------------------------------------*/
/*		Initialize ScanN Message Token																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( pmsg, sizeof(struct _token)/4 );    /* Initialize ScanN Message		*/
		MlibResetLongMemory( pPrm, sizeof(OPPRMDAT)/4 );         /* MercuryはPnBxxが無い!!			*/
		/*------------------------------------------------------------------------------------------*/
		pmsg->Super = &NetMsg;									 /* TODO: Multi axis				*/
		pmsg->pPrm = pPrm; 									     /* TODO: Multi axis				*/
/*--------------------------------------------------------------------------------------------------*/
		NpxInitInOutProc( psvif, &NinV, &NoutV );                /* ScanN I/O Init Main   			*/
/*--------------------------------------------------------------------------------------------------*/
		MtcInit( &MtHdl, psvif );                                /* Initialize M2 Motion            */

/*--------------------------------------------------------------------------------------------------*/
/* 		Load initial parameters																		*/
/*--------------------------------------------------------------------------------------------------*/
		IxiLoadInitPrm( &MtHdl, pmsg );                         /* Load Initial Parameters			*/

/*--------------------------------------------------------------------------------------------------*/
/* 		Calculate Motion Parameters																	*/
/*--------------------------------------------------------------------------------------------------*/
		rv = MPrmCalcInit( &(MtHdl.mtPrm), &(MtHdl.iniPrm.mt) );
		NoutV.AlmBits |= ( rv != 0 ) ? ALMBITS_PRMVALEXCEED	: 0; /* 暫定：本当はA.04A */
/*--------------------------------------------------------------------------------------------------*/
		LpxInitPos( &Npos, pPrm );                               /* Initialize Position             */
		NpiInitReqQueues( );                                     /* Initialize ReqQueue				*/

/*--------------------------------------------------------------------------------------------------*/
/*      Load Program Table                                                                          */
/*--------------------------------------------------------------------------------------------------*/
		rv = IxxPgmInitLoad( &PgmCache );                        
		NoutV.AlmBits |= ( rv & PGM_INITLOAD_SUMWRONG ) ? ALMBITS_PGMSUMWRONG : 0;
		NoutV.AlmBits |= ( rv & PGM_INITLOAD_INVALID ) ? ALMBITS_PGMVALEXCEED : 0;
		NoutV.AlmBits |= ( rv & PGM_INITLOAD_VERUNMATCH ) ? ALMBITS_PGMVERUNMATCH : 0;
/*--------------------------------------------------------------------------------------------------*/
/*      Load Zone Table                                                                             */
/*--------------------------------------------------------------------------------------------------*/
		rv = IxxZoneInitLoad( &ZoneCache );                      
		NoutV.AlmBits |= ( rv & ZONE_INITLOAD_SUMWRONG ) ? ALMBITS_ZONESUMWRONG : 0;
		NoutV.AlmBits |= ( rv & ZONE_INITLOAD_INVALID ) ? ALMBITS_ZONEVALEXCEED	: 0;
		NoutV.AlmBits |= ( rv & ZONE_INITLOAD_VERUNMATCH ) ? ALMBITS_ZONEVERUNMATCH	: 0;
/*--------------------------------------------------------------------------------------------------*/
/*      Load Jspd Table                                                                             */
/*--------------------------------------------------------------------------------------------------*/
		rv = IxxJspdInitLoad( &JspdCache );                      
		NoutV.AlmBits |= ( rv & JSPD_INITLOAD_SUMWRONG ) ? ALMBITS_JSPDSUMWRONG : 0;
		NoutV.AlmBits |= ( rv & JSPD_INITLOAD_INVALID ) ? ALMBITS_JSPDVALEXCEED	: 0;
		NoutV.AlmBits |= ( rv & JSPD_INITLOAD_VERUNMATCH ) ? ALMBITS_JSPDVERUNMATCH	: 0;
/*--------------------------------------------------------------------------------------------------*/
		IxxInitSqi( &SqiV, pPrm );	      /* Initialize SQI */
		IxxInitSqo( &SqoV, pPrm );		  /* Initialize SQO */
		IxxInitAxctrl( &AxctrlV );		  /* Initialize motion control */
		IxxInitIntprt( &IntprtV );		  /* Initialize Interpreter */
		IxxInitPgm( &PgmV );              /* Initialize Pgm */
		IxxInitZone( &ZoneV );            /* Initialize Zone */
		IxxInitJspd( &JspdV );			  /* Initialize Jspd */
/*--------------------------------------------------------------------------------------------------*/
		IxxInitAlmMngr( &OpAlmV );        /* Initialize Alm Mngr */
/*--------------------------------------------------------------------------------------------------*/
		IxxInitAnlg( &AnlgV );            /* Initialize Anlg */
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#pragma arm section code

/****************************************************************************************************/
/*																									*/
/*		TODO: ScanN Parameter Reset Program															*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	NpiScanNPrstMain( SV_OPTION_IF *psvif )
{
UINT 	rv;
NMSG	*pmsg = &Nmsg;
OPPRMDAT *pPrm = pmsg->pPrm;
/*--------------------------------------------------------------------------------------------------*/
		NpxInitInOutProc( psvif, &NinV, &NoutV );                /* ScanN I/O Init Main   			*/
/*--------------------------------------------------------------------------------------------------*/
		MtcInit( &MtHdl, psvif );                                /* Initialize M2 Motion            */
/*--------------------------------------------------------------------------------------------------*/
/* 		Load initial parameters																		*/
/*--------------------------------------------------------------------------------------------------*/
		IxiLoadInitPrm( &MtHdl, pmsg );                          /* Load Initial Parameters			*/

/*--------------------------------------------------------------------------------------------------*/
/* 		Calculate Motion Parameters																	*/
/*--------------------------------------------------------------------------------------------------*/
		rv = MPrmCalcInit( &(MtHdl.mtPrm), &(MtHdl.iniPrm.mt) );
		NoutV.AlmBits |= ( rv != 0 ) ? ALMBITS_PRMVALEXCEED	: 0; /* 暫定：本当はA.04A */

/*--------------------------------------------------------------------------------------------------*/
		LpxInitPos( &Npos, pPrm );                               /* Initialize Position             */
		NpiInitReqQueues( );                                     /* Initialize ReqQueue				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Reset Program table																			*/
/*--------------------------------------------------------------------------------------------------*/
		rv = IxxPgmInitLoad( &PgmCache );
		NoutV.AlmBits |= ( rv & PGM_INITLOAD_SUMWRONG ) ? ALMBITS_PGMSUMWRONG : 0;
		NoutV.AlmBits |= ( rv & PGM_INITLOAD_INVALID ) ? ALMBITS_PGMVALEXCEED : 0;
		NoutV.AlmBits |= ( rv & PGM_INITLOAD_VERUNMATCH ) ? ALMBITS_PGMVERUNMATCH : 0;
/*--------------------------------------------------------------------------------------------------*/
/*		Reset ZONE table																			*/
/*--------------------------------------------------------------------------------------------------*/
		rv = IxxZoneInitLoad( &ZoneCache );
		NoutV.AlmBits |= ( rv & ZONE_INITLOAD_SUMWRONG ) ? ALMBITS_ZONESUMWRONG : 0;
		NoutV.AlmBits |= ( rv & ZONE_INITLOAD_INVALID ) ? ALMBITS_ZONEVALEXCEED	: 0;
		NoutV.AlmBits |= ( rv & ZONE_INITLOAD_VERUNMATCH ) ? ALMBITS_ZONEVERUNMATCH	: 0;
/*--------------------------------------------------------------------------------------------------*/
/*		Reset JSPD table																			*/
/*--------------------------------------------------------------------------------------------------*/
		rv = IxxJspdInitLoad( &JspdCache );
		NoutV.AlmBits |= ( rv & JSPD_INITLOAD_SUMWRONG ) ? ALMBITS_JSPDSUMWRONG : 0;
		NoutV.AlmBits |= ( rv & JSPD_INITLOAD_INVALID ) ? ALMBITS_JSPDVALEXCEED	: 0;
		NoutV.AlmBits |= ( rv & JSPD_INITLOAD_VERUNMATCH ) ? ALMBITS_JSPDVERUNMATCH	: 0;
/*--------------------------------------------------------------------------------------------------*/
/*		Set sequence input signal parameters														*/
/*--------------------------------------------------------------------------------------------------*/
		IxxInitSqi( &SqiV, pPrm  );                /* Initialize SQI */
/*--------------------------------------------------------------------------------------------------*/
/*		Set sequence output signal logics															*/
/*--------------------------------------------------------------------------------------------------*/
		IxxInitSqo( &SqoV, pPrm );                 /* Initialize SQO */
/*--------------------------------------------------------------------------------------------------*/
		IxxInitAnlg( &AnlgV );                     /* Initialize Anlg */
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#pragma arm section code



/****************************************************************************************************/
/*																									*/
/*		Initialize Position Management                     											*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	LpxInitPos( struct _pos *pNpos, OPPRMDAT *pPrm )
{
ULONG wkprm;
LONG poscnt;
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( pNpos, sizeof(struct _pos)/4 );
/*--------------------------------------------------------------------------------------------------*/
//	rv = OK;
	wkprm = 0;
/*--------------------------------------------------------------------------------------------------*/
/*  Read Parameters                                                                                 */
/*--------------------------------------------------------------------------------------------------*/
	pNpos->f.LinearCircle = LINERCIRCLE_LINER /* pPrm->LinerCircle */;
	pNpos->PosLimP = 0 /* pPrm->PosLimP */;
	pNpos->PosLimN = 0 /* pPrm->PosLimN */;
	pNpos->HomePos = 0 /* pPrm->HomePos */;
/*--------------------------------------------------------------------------------------------------*/
/*  Reset Position Parameters                                                                       */
/*--------------------------------------------------------------------------------------------------*/
	pNpos->PosMax = ( ( pNpos->f.LinearCircle == LINERCIRCLE_LINER ) ? INT_MAX : 
				                                   MlibMAX( pNpos->PosLimP, pNpos->PosLimN ) );
	/*----------------------------------------------------------------------------------------------*/
	pNpos->PosMin = ( ( pNpos->f.LinearCircle == LINERCIRCLE_LINER ) ? INT_MIN : 
						                           MlibMIN( pNpos->PosLimP, pNpos->PosLimN ) );
	/*----------------------------------------------------------------------------------------------*/
	pNpos->PosWidth = pNpos->PosMax - pNpos->PosMin + 1;
/*--------------------------------------------------------------------------------------------------*/
	NoutV.AlmBits |=
	(( pNpos->HomePos < pNpos->PosMin ) || ( pNpos->HomePos > pNpos->PosMax ) ) ? ALMBITS_PRMVALEXCEED : 0;
/*--------------------------------------------------------------------------------------------------*/
	pNpos->f.bSotUse = ( ( pNpos->f.LinearCircle == LINERCIRCLE_LINER )
					&& ( ( pNpos->PosLimP != 0 ) || ( pNpos->PosLimN != 0 ) ) );
/*--------------------------------------------------------------------------------------------------*/
/*  Reset Positions                                                                                 */
/*--------------------------------------------------------------------------------------------------*/
	if ( pNpos->f.LinearCircle == LINERCIRCLE_LINER )                 /* liner coordinate */
	{
		poscnt = 0;
		pNpos->Apos = poscnt;
		pNpos->Pos = poscnt;
	}
	else                                                              /* circle coodinate */
	{
		poscnt = (0 - pNpos->PosMin);
		poscnt = (poscnt % (pNpos->PosWidth));
		poscnt += (poscnt < 0) ? pNpos->PosWidth : 0;
		pNpos->Apos = poscnt + pNpos->PosMin;
		pNpos->Pos = poscnt + pNpos->PosMin;
	}
/*--------------------------------------------------------------------------------------------------*/
	pNpos->Tpos = 0;
	pNpos->Tdst = 0;
	pNpos->Rpos = 0;
	pNpos->Rdst = 0;
/*--------------------------------------------------------------------------------------------------*/
/*  Set Position Management Methods                                                                 */
/*--------------------------------------------------------------------------------------------------*/
	pNpos->SetPos = LpxSetPosLinear;			/* 0: Liner coordinate */
	pNpos->SetDst = LpxSetDstLinear;			/* 0: Liner coordinate */

/*--------------------------------------------------------------------------------------------------*/
/*	Set M2 Soft limit parameters																	*/
/*--------------------------------------------------------------------------------------------------*/
	wkprm = ((pNpos->f.bSotUse ) && ( pNpos->f.LinearCircle == LINERCIRCLE_LINER )) ? 
			( 0x0100 ) : ( 0x0003);
	MPrmSetSoftLimitFlag( &(MtHdl.mtPrm), wkprm );           /* sot enable or disable */
	/*----------------------------------------------------------------------------------------------*/
	/*  内部電子ギア変換 : ギア比16倍以上はオーバーフローするので注意!!                             */
	/*----------------------------------------------------------------------------------------------*/
	poscnt = 0;                                                      /* internal position */
//	rv = 0;                                                          /* dummy rem */
//	poscnt = MlibPcmdEgear( pNpos->PosLimP, &Iprm.EgearX, &rv );     /* convert position */
	MPrmSetSoftLimitP( &(MtHdl.mtPrm), poscnt );                     /* set positive sot */
//	poscnt = MlibPcmdEgear( pNpos->PosLimN, &Iprm.EgearX, &rv );     /* convert position */
	MPrmSetSoftLimitN( &(MtHdl.mtPrm), poscnt );                     /* set negative sot */
	/*----------------------------------------------------------------------------------------------*/
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*																									*/
/*		Position Set Function (Linear)                     											*/
/*																									*/
/****************************************************************************************************/
void  LpxSetPosLinear ( struct _pos *pNpos, long pgcnt, long pos, long *dstpos )
{
long poscnt;
	poscnt = pos;
	poscnt += pgcnt;
	//poscnt += ( poscnt < 0 ) ? (pNpos->PosWidth) : (0) ;  //直線座標の場合、PosWidth=INT_MAX-INT_MIN+1=0
	*dstpos = poscnt;
//Mercury	*dstpos = ((poscnt << 3) >> 3);    // 29bit longに変換
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Desitination Set Function (Linear)                 											*/
/*																									*/
/****************************************************************************************************/
void	LpxSetDstLinear ( struct _pos *pNpos, long tpos, long *tdst )
{
	*tdst = tpos - (pNpos->Pos);
	return;
}

/****************************************************************************************************/
/*  																								*/
/*  Set Origin Position 																			*/
/*  																								*/
/****************************************************************************************************/
void	IxxSetOriginPos( struct _pos *pNpos, long homepos )
{
LONG tdst;
PSNHNDL *hPos = &(MtHdl.psnHdl);

	pNpos->SetDst( pNpos, homepos, &tdst );                 /* get destination */
//Mercury	tdst = MlibPcmdEgear( tdst, &Iprm.EgearX, &dummy );     /* convert position (10->13) */
	PsnSetCordinate(hPos, 0, PsnGetPOS(hPos) + tdst );      /* set home position  */
	PsnSetRefPoint(hPos, TRUE);                             /* set reference point */
	return;
}


/****************************************************************************************************/
/*																									*/
/*		磁極検出サーボオン要求作成作成													 <S21C> 	*/
/*																									*/
/****************************************************************************************************/
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE
//void	PdetServoOnMake( SERVO_CONTROL_IF *SvControlIf, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV, BOOL Alm )
ALMID_T	PdetServoOnMake( SERVO_CONTROL_IF *SvControlIf, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV, AXIS_HANDLE *Axis )	/* <S226> */
{
	BOOL	bwk;
	BOOL	AC22;						/* <S226> */
	BOOL	Alm;						/* <S226> */
	ALMID_T	AlmCode	= NO_ALARM;			/* <S226> */
	
	Alm  = ( Axis->AlmManager->AlmReady == FALSE );					/* <S226> */
	AC22 = ALMGetOtherAlarmChk( Axis->AlmManager, ALM_PHASEANGLE );	/* 磁極検出許可アラームA.C22 <S226> */
	
	if( MencV->AxisMotType == MOTTYPE_LINEAR )						/* リニアモータ					*/
	{
		if( FnCmnCtrl->FnSvControl == FALSE )						/* 通常中						*/
		{
			if(( Alm == FALSE )&&( AC22 == TRUE ))					/* <S226> */
			{
				if(( NinV.P.AssignMode == SQI_ASSIGN_DEFAULT ) &&		/* Pn50A.0=0:ディフォルト設定	*/
				  (( SvControlIf->CmdSeqBit & SERVO_ON_BIT ) != 0 ))	/* サーボオン要求				*/
				{
					SvControlIf->CmdSeqBit |= POL_DETECT_BIT;
				}
				else													/* <S226> */
				{
					if(( SvControlIf->CmdSeqBit & SERVO_ON_BIT ) != 0 )	/* サーボオン要求			*/
					{
						AlmCode = ALM_PDET;								/* 磁極検出未完				*/
					}
				}														/* <S226> */
			}
			else
			{
				SvControlIf->CmdSeqBit &= ~( POL_DETECT_BIT );
			}
		}
		else														/* ROUND制御中					*/
		{
			if( Alm == FALSE )
			{
				bwk = FnCmnCtrl->PdetReq;
				if( bwk == FALSE )
				{
					if( NinV.P.AssignMode == SQI_ASSIGN_DEFAULT )	/* Pn50A.0=0:ディフォルト設定	*/
					{
						bwk = FnCmnCtrl->FnSvonReq;					/* サーボオン要求				*/
					}
					else											/* <S226> */
					{
						if( FnCmnCtrl->FnSvonReq == TRUE )			/* サーボオン要求				*/
						{
							AlmCode = ALM_PDET;						/* 磁極検出未完					*/
						}
					}												/* <S226> */
				}
			}
			else
			{
				bwk = FALSE;
				if( AC22 == FALSE )									/* <S226> */
				{
					bwk = FnCmnCtrl->PdetReq;
				}													/* <S226> */
			}
			FnCmnCtrl->SvOnPdetReq = bwk;
		}
	}
	
	return( AlmCode );		/* <S226> */
}
#endif
/****************************************************************************************************/
/*  																								*/
/*  Initialize Message Interface 																	*/
/*  																								*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxiInitNetMessage( MSGIF *Hmsg, AXIS_HANDLE *pAxRsc )
{
//	FUN_AXCOMMON	*AxCommon;								/* <S056> */
//	AxCommon = ((AXIS_HANDLE*)pAxRsc)->FnManager->AxCommon;	/* <S056> */

	MlibResetLongMemory( Hmsg, sizeof( MSGIF )/4 );
	/*----------------------------------------------------------------------------------------------*/
	Hmsg->f.NetMsgIF = TRUE;									/* ??? */
//	Hmsg->AccessLvl = ACCLVL_SYSTEM;							/* Set Access Lvl */	/* <S056>	Delete : Fn010で設定するアクセルレベルを設定*/
//	Hmsg->AccessLvl = AxCommon->AccessLevel;					/* Set Access Lvl */	/* <S056> *//* <S0FE> */
	Hmsg->AccessLvl = ACCLVL_USER2;								/* Set Access Lvl */	/* <S0FE> */
	Hmsg->pAxRsc = pAxRsc;										/* TODO: あとで消す */
	/*----------------------------------------------------------------------------------------------*/
	;															/* 				*/
	/*----------------------------------------------------------------------------------------------*/
	return;
}
#pragma arm section code


