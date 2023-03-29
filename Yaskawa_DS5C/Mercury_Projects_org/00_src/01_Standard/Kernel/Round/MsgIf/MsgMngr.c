/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MsgMngr.c : Message Manager																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		1) Message Interace																			*/
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
/*		Rev.1.00 : 2013.04.18  K.Ozaki																*/
/*		Rev.1.10 : 2013.08.08  K.Ozaki		@@Panel Operator										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"								/* Kernel API */
#include "LcdOp.h"								/* LcdOpIf Common Header */


/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
void KpxMsgManager(void);							/* Message Manager Main Function				*/
//<S150> void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc ); 	/* Initialize Message Interface					*/
void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap ); 	/* Initialize Message Interface					*/

/*--------------------------------------------------------------------------------------------------*/
static void	LpxSerialMsgManager( HMSGIF hmsg );				/* シリアルメッセージ管理				*/
//<S150> static void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc );		/* メッセージＩＦ初期化処理				*/
static void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap );		/* メッセージＩＦ初期化処理				*/

/*--------------------------------------------------------------------------------------------------*/
static void	LpxUsbMsgManager( HMSGIF hmsg );				/* <S166> USBメッセージ管理				*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
static void LpxTstCmdManager( HMSGIF Hmsg );		/*												*/
#endif
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxCheckRecvMessage( HMSGIF Hmsg );		/* Check Received Message						*/
LONG	RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen ); /*							*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxGetCmdMsgAxisIdx( HMSGIF Hmsg );		/* 指令メッセージ軸番号取得						*/
/*--------------------------------------------------------------------------------------------------*/
static USHORT LpxPrmGetOpeDspMode( AXIS_HANDLE *pAxRsc );	/* オペレータのユーザー定数表示モード取得 */	/* <S113> */
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PANELOP_USE == 1
static void	LpxPopMsgManager( HMSGIF hmsg );		/* パネルOPメッセージ管理						*/
#else
#define LpxPopMsgManager( x )     { ;}				/*												*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Macro Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_LOPMSGHDL(hLopMsg, hMsg)  ((hLopMsg == NULL) ? (hLopMsg = (hMsg)):   /* Get LcdOpe Handle */ \
								((hLopMsg == (hMsg)) ? (hLopMsg) : FALSE))
#define LPX_FREE_LOPMSGHDL(hMsg)  ((hMsg) = NULL) 							/* Free LcdOpe Handle */



/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
//RINV	RinV;									/* Round Input  Variables							*/
//ROUTV	RoutV;									/* Round Output Variables							*/
//RMNGV	RmngV;									/* Round Manage Variables							*/
//RNDWRTD	RndWrtDisable;						/* データ書込み禁止フラグ							*/

#ifdef CSW_NETIF_M3_TYPE	/*<S150>*/
extern CSIF		M3Sif[2];							/* MECHATROLINK Interface Instance	<SAKA_TEST>		*/
#endif	/* <S150> */

/*--------------------------------------------------------------------------------------------------*/
/*	Message I/F																						*/
/*--------------------------------------------------------------------------------------------------*/
MSGIF	SrlMsg[SIF_N_CH];						/* Serial Message I/F for SigmaMsg/LcdOp/Memobus	*/
MSGIF	UsbMsg[USBIF_N_CH];						/* USB Message I/F 									*/
MSGIF	NetMsg;									/* Network Message I/F(*)							*/
//MSGIF	NetMsg[NETIF_N_CH];						/* Network Message I/F(*)							*/
MSGIF	M3Msg[2];								/* Mechatrolink Message I/F							*//*<S150>*/
/*--------------------------------------------------------------------------------------------------*/
MSGIF	LopMsg;									/* LCD Operator Messsage I/F 						*/
MSGIF	PopMsg;									/* Panel Operator Message I/F(*)					*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*	Lcd Operator Mutex																				*/
/*--------------------------------------------------------------------------------------------------*/
void	*mLopMsg;								/* LCD Operator Message I/F Mutex					*/
/*--------------------------------------------------------------------------------------------------*/
UCHAR	PopTxBuf[8];	/* <S025> */
UCHAR	PopRxBuf[8];	/* <S025> */



/****************************************************************************************************/
/*																									*/
/*		メッセージ管理																				*/
/*																									*/
/****************************************************************************************************/
void KpxMsgManager(void)
{
	if( UART0_PORT_USE == 1 ) { LpxSerialMsgManager( &SrlMsg[COM0_UART] ); } /* Serial CH:0         */
	if( UART1_PORT_USE == 1 ) { LpxSerialMsgManager( &SrlMsg[COM1_UART] ); } /* Serial CH:1         */
	if( USB0_PORT_USE == 1 ) { LpxUsbMsgManager( &UsbMsg[COM0_USB] ); }   	 /* <S166> Usb CH:0     */
//<S166>	if( USB0_PORT_USE == 1 ) { LpxSerialMsgManager( &UsbMsg[COM0_USB] ); }   /* Usb CH:0            */
	if( 1 ) { LpxPopMsgManager( &PopMsg ); }                           		 /* Panel Operator (AP) */
}

#ifdef CSW_NETIF_M3_TYPE	/*<S150>*/
/* M3MsgIf.cのメッセージ処理からコールされる */
void KpxMsgManager2( UCHAR Channel )
{
	LpxSerialMsgManager( &M3Msg[Channel] );
}
#endif	/* <S150> */

/****************************************************************************************************/
/*																									*/
/*		シリアルメッセージ管理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : シリアルメッセージの受信を監視し、各シリアルメッセージ処理(LcdOp,SigmaMsg,etc)			*/
/*			の実行制御を行う。																		*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	LpxSerialMsgManager( HMSGIF Hmsg )
{
CSIF *Sif = Hmsg->Sif;
/*--------------------------------------------------------------------------------------------------*/
		if( Sif == NULL ) { return; }								/* No Serial Interface 			*/
/*--------------------------------------------------------------------------------------------------*/
/*		メッセージ受信完時																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Sif->ChkCmd( Sif, &(Hmsg->CmdMsgLen) ) )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	受信メッセージ正常																		*/
		/*------------------------------------------------------------------------------------------*/
			if( LpxCheckRecvMessage( Hmsg ) != OK )					/* check message */
			{
				Sif->StxRes( Sif, Hmsg->ResBuf, 0 );				/* no response */
			}
		/*------------------------------------------------------------------------------------------*/
		/*	LcdOp Message Procedure																	*/
		/*------------------------------------------------------------------------------------------*/
			else if ( KPI_CHK_OPEMSG( Hmsg->CmdBuf[1] ) )
			{
				switch( Hmsg->ComMode )								/* Check ComMode and			*/
				{													/* Call Finish Procedure		*/
				case SCM_NOCOM:	 RpxPnlOpFinish( &PopMsg ); break;
				case SCM_LCDCOM: break;
				case SCM_MSGCOM: RpxMsgifFinishProcedure( Hmsg ); break;
				default:		 break;
				}
			/*--------------------------------------------------------------------------------------*/
			/*	デジオペ排他処理()																	*/
			/*--------------------------------------------------------------------------------------*/
				if ( LPX_GET_LOPMSGHDL( mLopMsg, Hmsg ) == FALSE )	/* Chk & Get LcdOpe Handle 	 	*/
				{
					Sif->StxRes( Sif, Hmsg->ResBuf, 0 );			/* No Response					*/
					return;
				}
			/*--------------------------------------------------------------------------------------*/
				KRI_BIND_MSGIF( &LopMsg, Hmsg );					/* bind CmdBuf,ResBuf etc 		*/
			/*--------------------------------------------------------------------------------------*/
				Hmsg->ComMode = SCM_LCDCOM;							/* Update ComMode				*/
				LopMsg.CmdMsgLen = Hmsg->CmdMsgLen;					/* Set cmd message length		*/
				KpiRstLongTimer( &(LopMsg.ComChkTime) );			/* Reset ComChkTimer			*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
				RpxLcdOpMain( &LopMsg, &DrvSel );					/* Call LcdOp Main Procedure	*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
				RpxLcdOpMain( &LopMsg, NULL );						/* Call LcdOp Main Procedure	*/
#endif
			}
		/*------------------------------------------------------------------------------------------*/
		/*	Serial Message Procedure (SigmaWin, etc)												*/
		/*------------------------------------------------------------------------------------------*/
			else
			{
				switch( Hmsg->ComMode )								/* Check ComMode and			*/
				{													/* Call Finish Procedure		*/
				case SCM_NOCOM:	 RpxPnlOpFinish( &PopMsg ); break;
				case SCM_LCDCOM: RpxLcdOpFinish( &LopMsg ); break;
				case SCM_MSGCOM: break;
				default:		 break;
				}
			/*--------------------------------------------------------------------------------------*/
				Hmsg->ComMode = SCM_MSGCOM;							/* Update ComMode				*/
				KpiRstLongTimer( &(Hmsg->ComChkTime) );				/* Reset ComChkTimer			*/
				RpxMsgifMainProcedure( Hmsg );						/* Call MsgIf Main Procedure	*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メッセージ受信未完時 : Check Timeout														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			switch( Hmsg->ComMode )										/* Check ComMode			*/
			{
			case SCM_LCDCOM:											/* LcdOp Message Mode		*/
				 if( KpiGetLongTimer( &(LopMsg.ComChkTime) ) > 500 )	/* Check Timeout 0.5sec		*/
				 {
					LPX_FREE_LOPMSGHDL(mLopMsg);						/* Free LcdOpe Handle 		*/
					RpxLcdOpFinish( &LopMsg );							/* Finish LcdOp				*/
					Hmsg->ComMode = SCM_NOCOM;							/* Update ComMode			*/
					RpxPnlOpInit();										/* Initialize PnlOp			*/
				 }
				 break;
			case SCM_MSGCOM:											/* Serial Message Mode		*/
				 if( KpiGetLongTimer( &(Hmsg->ComChkTime) ) > 20000 )	/* Check Timeout 20sec		*/
				 {
					RpxMsgifFinishProcedure( Hmsg );					/* Finish SifMsg			*/
					Hmsg->ComMode = SCM_NOCOM;							/* Update ComMode			*/
					RpxPnlOpInit();										/* Initialize PnlOp			*/
				 }
				 break;
			default:
				 break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		<S166> USBメッセージ管理																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : シリアルメッセージの受信を監視し、各シリアルメッセージ処理(LcdOp,SigmaMsg,etc)			*/
/*			の実行制御を行う。																		*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	LpxUsbMsgManager( HMSGIF Hmsg )
{
CSIF *Sif = Hmsg->Sif;
/*--------------------------------------------------------------------------------------------------*/
		if( Sif == NULL ) { return; }								/* No Serial Interface 			*/
/*--------------------------------------------------------------------------------------------------*/
/*		メッセージ受信完時																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Sif->ChkCmd( Sif, &(Hmsg->CmdMsgLen) ) )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	受信メッセージ正常																		*/
		/*------------------------------------------------------------------------------------------*/
			if( LpxCheckRecvMessage( Hmsg ) != OK )					/* check message */
			{
				Sif->StxRes( Sif, Hmsg->ResBuf, 0 );				/* no response */
			}
			/*--------------------------------------------------------------------------------------*/
			Hmsg->ComMode = SCM_MSGCOM;								/* Update ComMode				*/
			KpiRstLongTimer( &(Hmsg->ComChkTime) );					/* Reset ComChkTimer			*/
			RpxMsgifMainProcedure( Hmsg );							/* Call MsgIf Main Procedure	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メッセージ受信未完時 : Check Timeout														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			switch( Hmsg->ComMode )										/* Check ComMode			*/
			{
			case SCM_MSGCOM:											/* Serial Message Mode		*/
				 if( KpiGetLongTimer( &(Hmsg->ComChkTime) ) > 20000 )	/* Check Timeout 20sec		*/
				 {
					RpxMsgifFinishProcedure( Hmsg );					/* Finish SifMsg			*/
					Hmsg->ComMode = SCM_NOCOM;							/* Update ComMode			*/
				 }
				 break;
			default:
				 break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		パネルオペレータメッセージ管理																*/
/*																									*/
/****************************************************************************************************/
#if SVFSEL_PANELOP_USE == 1
void	LpxPopMsgManager( HMSGIF Hmsg )
{
CSIF *Sif = Hmsg->Sif;
UINT ComModeX = SCM_NOCOM;
HMSGIF hSrlMsg1 = NULL;
HMSGIF hSrlMsg2 = NULL;
/*--------------------------------------------------------------------------------------------------*/
		if( Sif == NULL ) { return; }								/* No Serial Interface 			*/
/*--------------------------------------------------------------------------------------------------*/
		if( UART0_PORT_USE == 1 ) { hSrlMsg1 = ( &SrlMsg[COM0_UART] ); } /* Serial CH:0         */
		if( UART1_PORT_USE == 1 ) { hSrlMsg2 = ( &SrlMsg[COM1_UART] ); } /* Serial CH:1         */
/*--------------------------------------------------------------------------------------------------*/
/*		Check Command Message																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( Sif->ChkCmd( Sif, &(Hmsg->CmdMsgLen) ) )
		{
			/*--------------------------------------------------------------------------------------*/
			/*
			 * Check Serial Communication Mode
			 * 
			 * PopMsg.ComMode = SrlMsg[].ComMode
			 *
			 */
			/*--------------------------------------------------------------------------------------*/
			if( hSrlMsg1 != NULL ) 
			{
				ComModeX = ((hSrlMsg1->ComMode != SCM_NOCOM )  			/* Check Serial CH:0    */
						? (hSrlMsg1->ComMode)
						: ComModeX );
			}
			if( hSrlMsg2 != NULL ) 
			{ 
				ComModeX = ((hSrlMsg2->ComMode != SCM_NOCOM )  			/* Check Serial CH:1    */
						? (hSrlMsg2->ComMode)
						: ComModeX );
			}
			Hmsg->ComMode = ComModeX;									/* Set ComMode */

			/*--------------------------------------------------------------------------------------*/
			/*
			 * Start Panel Operator Main Function
			 *
			 *
			 */
			/*--------------------------------------------------------------------------------------*/
			KpiRstLongTimer( &(Hmsg->ComChkTime) );						/* Reset ComChkTimer		*/
			RpxPnlOpMain( Hmsg );										/* Panel Operator Main		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Not received message : Check Timeout														*/
/*--------------------------------------------------------------------------------------------------*/
		else 
		{
			 if( Hmsg->ComMode == SCM_NOCOM )	return;					/* <S14F> 接続機器なし		*/
			 if( KpiGetLongTimer( &(Hmsg->ComChkTime) ) > 500 )			/* Check Timeout 0.5sec		*/
			 {
				RpxPnlOpFinish( Hmsg );									/* Finish Function 			*/
				RpxPnlOpInit();											/* Initialize PnlOp			*/
			 }
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#endif




/****************************************************************************************************/
/*																									*/
/*		メッセージＩＦ初期化処理																	*/
/*																									*/
/****************************************************************************************************/
/* <S150> */
//void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc )
//{
//		LpxInitMsgInterface( pAxRsc );
//}
void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap )
{
		LpxInitMsgInterface( pAxRsc, AxisMap );
}


/****************************************************************************************************/
/*																									*/
/*		メッセージＩＦ初期化処理																	*/
/*																									*/
/****************************************************************************************************/
//<S150>void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc )
void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap )
{
	FUN_AXCOMMON	*AxCommon;	/* <S056> */
	
	AxCommon = ((AXIS_HANDLE*)pAxRsc)->FnManager->AxCommon;	/* <S056> */

/*--------------------------------------------------------------------------------------------------*/
/*		シリアルメッセージＩＦ初期化																*/
/*--------------------------------------------------------------------------------------------------*/
#if UART0_PORT_USE	== 1
		KRI_INIT_MSGIF( &SrlMsg[COM0_UART], &LSif[COM0_UART] );	/* init com I/F (UART ch:0)			*/
		KRI_SET_FLG_SRLMSG( &SrlMsg[COM0_UART] );				/* set sif message flag				*/
//		SrlMsg[COM0_UART].AccessLvl = ACCLVL_USER1;		    	/* TODO: Set Access Lvl				*/	/* <S056>	Delete : Fn010で設定するアクセルレベルを設定 */
//		SrlMsg[COM0_UART].AccessLvl = (USHORT)(AxCommon->AccessLevel);	/* Set Access Lvl			*/	/* <S056> *//* <S0FE>	*/
		SrlMsg[COM0_UART].AccessLvl = ACCLVL_USER2			;	/* Set Access Lvl					*/	/* <S0FE> */
		SrlMsg[COM0_UART].pAxRsc = pAxRsc;						/* TODO: あとで消す */
		SrlMsg[COM0_UART].AxisMap = AxisMap;					/* <S150> */
#endif
	/*----------------------------------------------------------------------------------------------*/
#if UART1_PORT_USE	== 1
		KRI_INIT_MSGIF( &SrlMsg[COM1_UART], &LSif[COM1_UART] );	/* init com I/F (UART ch:1)			*/
		KRI_SET_FLG_SRLMSG( &SrlMsg[COM1_UART] );				/* set sif message flag				*/
//		SrlMsg[COM1_UART].AccessLvl = ACCLVL_USER1;		    	/* TODO: Set Access Lvl				*/	/* <S056>	Delete : Fn010で設定するアクセルレベルを設定 */
//		SrlMsg[COM1_UART].AccessLvl = (USHORT)(AxCommon->AccessLevel);	/* Set Access Lvl			*/	/* <S056> *//* <S0FE> */
		SrlMsg[COM1_UART].AccessLvl = ACCLVL_USER2;				/* Set Access Lvl					*/	/* <S0FE> */
		SrlMsg[COM1_UART].pAxRsc = pAxRsc;						/* TODO: あとで消す */
		SrlMsg[COM1_UART].AxisMap = AxisMap;					/* <S150> */
#endif
	/*----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		パネルオペレータレジスタＩＦ初期化 (レジスタＩＦ共通化のため)								*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PANELOP_USE == 1
		MlibResetLongMemory( &PopMsg, sizeof(PopMsg)/4 );		/* 後で消す							*/
		VSif[VCOM0_PANEL].RxBuf = PopRxBuf;						/* 後で消す 						*//* <S025> */
		VSif[VCOM0_PANEL].TxBuf = PopTxBuf;						/* 後で消す 						*//* <S025> */
		KRI_INIT_MSGIF( &PopMsg, &VSif[VCOM0_PANEL] );			/* init com I/F (Virtual UART ch:0)	*/
		KRI_SET_FLG_PNLOPE( &PopMsg );							/* set panel message flag			*/
//		PopMsg.AccessLvl = ACCLVL_SYSTEM /* ACCLVL_USER1*/;		/* TODO: Set Access Lvl 			*//* <S025> */	/* <S056>	Delete : Fn010で設定するアクセルレベルを設定 */
//		PopMsg.AccessLvl = (USHORT)(AxCommon->AccessLevel);		/* Set Access Lvl					*/	/* <S056> *//* <S0FE> */
		PopMsg.AccessLvl = ACCLVL_USER1;						/* Set Access Lvl					*/	/* <S0FE> */
		PopMsg.pAxRsc = pAxRsc;									/* TODO: あとで消す 				*/
		RpxPnlOpInit( );										/* Initialize Panel Operator		*/
		RpxPnlOpMonInit( &PopMsg );								/* Initialize Panel Operator		*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		LCDオペレータレジスタＩＦ初期化 (レジスタＩＦ共通化のため)									*/
/*--------------------------------------------------------------------------------------------------*/
		KRI_INIT_MSGIF( &LopMsg, &LSif[COM1_UART] );			/* set initial com I/F (UART ch:1)	*/
		KRI_SET_FLG_LCDOPE( &LopMsg );							/* set lcd message flag				*/
		LcdOpInit( &LopMsg );									/* 									*/
		LopMsg.OpeDspMode = LpxPrmGetOpeDspMode( pAxRsc );				/* Set User Paramete Display Mode	*/	/* <S113> */
//		LopMsg.AccessLvl = ACCLVL_SYSTEM /* ACCLVL_USER1*/;		/* TODO: Set Access Lvl(Debug!!)	*/	/* <S056>	Delete : Fn010で設定するアクセルレベルを設定 */
//		LopMsg.AccessLvl = (USHORT)(AxCommon->AccessLevel);		/* Set Access Lvl					*/	/* <S056> *//* <S0FE> */
		LopMsg.AccessLvl = ACCLVL_USER1;						/* Set Access Lvl					*/	/* <S0FE> */
		LopMsg.pAxRsc = pAxRsc;									/* TODO: あとで消す 				*/

/*--------------------------------------------------------------------------------------------------*/
/*		USBメッセージＩＦ初期化																		*/
/*--------------------------------------------------------------------------------------------------*/
#if USB0_PORT_USE	== 1
		KRI_INIT_MSGIF( &UsbMsg[COM0_USB], &USif[COM0_USB] );	/* init com I/F (USB ch:0)			*/
		KRI_SET_FLG_USBMSG( &UsbMsg[COM0_USB] );				/* set sif message flag				*/
//		UsbMsg[COM0_USB].AccessLvl = ACCLVL_USER1;		    	/* TODO: Set Access Lvl 			*/	/* <S056>	Delete : Fn010で設定するアクセルレベルを設定 */
//		UsbMsg[COM0_USB].AccessLvl = (USHORT)(AxCommon->AccessLevel);		/* TODO: Set Access Lvl */	/* <S056>	*//* <S0FE> */
		UsbMsg[COM0_USB].AccessLvl = ACCLVL_USER2;				/* TODO: Set Access Lvl				*/	/* <S0FE>	*/
		UsbMsg[COM0_USB].pAxRsc = pAxRsc;						/* TODO: あとで消す 				*/
		UsbMsg[COM0_USB].AxisMap = AxisMap;						/* <S150> */
#endif
		
/*--------------------------------------------------------------------------------------------------*/
/*		ネットワークメッセージＩＦ初期化 (レジスタＩＦ共通化のため)									*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
		IxiInitNetMessage( &NetMsg, pAxRsc );					/* TODO: 後でけす 					*/
#endif

/* <S150> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/*		MECHATROLINKメッセージＩＦ初期化															*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef CSW_NETIF_M3_TYPE
	{
		int	i;
		HMSGIF	Hmsg;
		for( i = 0; i < 2; i++ )
		{
			Hmsg = &M3Msg[i];
			KRI_INIT_MSGIF( Hmsg, &M3Sif[i] );			/* init com I/F (Mechatrolink)			*/
			KRI_SET_FLG_MLNKMSG( Hmsg );				/* set sif message flag				*/
			Hmsg->AccessLvl = (USHORT)(ACCLVL_USER2);	/* TODO: Set Access Lvl */	/* <S056>	*/
			Hmsg->pAxRsc = pAxRsc;						/* TODO: あとで消す 				*/
			Hmsg->AxisMap = AxisMap;
		}
	}
#endif
/* <<<<< <S150> */
/*--------------------------------------------------------------------------------------------------*/
		LPX_FREE_LOPMSGHDL( mLopMsg );							/* Free Mutex */
		return;
}


/****************************************************************************************************/
/*																									*/
/*		MessageI/F : 受信メッセージチェック															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 受信メッセージのチェックを行う。														*/
/*																									*/
/*			1) 受信メッセージ長のチェック															*/
/*			2) 受信メッセージのサムチェック															*/
/*			3) 軸アドレスのチェック(MEMOBUS)														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxCheckRecvMessage( HMSGIF Hmsg )
{
//LONG	Sum;//<S0C7>
LONG	MsgLen = Hmsg->CmdMsgLen = RkiMsgifGetCmdMsgLength( Hmsg->CmdBuf, Hmsg->RcvBufSize );		
		if( MsgLen <= 0 ){  return (NG); } /* wrong message */
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMessage (LCD-Operator)																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( KPI_CHK_OPEMSG( Hmsg->CmdBuf[1] ) )
		{
			if( MsgLen & 0x0001 ){ return( NG );}
			return MlibChkSum16SM( Hmsg->CmdBuf, MsgLen );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMemobus (PC, etc)																		*/
/*--------------------------------------------------------------------------------------------------*/
		else 
		{
			/* TODO: , then check axis number */
			if ( Hmsg->f.UsbMsgIF )				
			{
				return (OK);					/* USBからの一部メッセージはCRCがない!! */
			}
/* <S150> >>>>> */
			else if ( Hmsg->f.MlnkMsgIF )
			{
				if( (Hmsg->CmdBuf[1] != MFC_MECHATROLINK) && (Hmsg->CmdBuf[1] != MFC_RELAY) )
				{
					return MlibChkCRC16MB( Hmsg->CmdBuf, MsgLen );
				}
				return (OK);
			}
/* <<<<< <S150> */
			else
			{
//<S150>				((UART_MBUS_SERVER *)Hmsg->Sif->ComMbus42)->MbusIf.RcvbLength = MsgLen - MBUS42_CRC_SIZE;
//<S150>				((UART_MBUS_SERVER *)Hmsg->Sif->ComMbus42)->MbusIf.RcvwLength = (MsgLen - MBUS42_CRC_SIZE)>>1;
				return MlibChkCRC16MB( Hmsg->CmdBuf, MsgLen );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
//		return (NG);//<S0C7>
}

/****************************************************************************************************/
/*																									*/
/*		MessageI/F : 指令メッセージ長取得(for Kernel\Driver\SifMbus.c)								*/
/*																									*/
/****************************************************************************************************/
#define LPX_GET_CMDMSGLEN( i, Mul, Add )\
		if( CmdBufLen < (i+2) ){ return( 0 );}\
		else{ return( (((CmdBuf[i]<<8) + CmdBuf[i+1]) * Mul) + Add );}
#define LPX_CHK_SLOT( SfcH )			(SfcH == 0x01)
#define LPX_GET_SLOT_BYTELEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 2 : 0)
#define LPX_GET_SLOT_WORDLEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 1 : 0)
/*--------------------------------------------------------------------------------------------------*/
LONG	RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen )
{
LONG	s = LPX_GET_SLOT_BYTELEN( CmdBuf[2] );
LONG	Offset;											/* <S150> */
/*--------------------------------------------------------------------------------------------------*/
/*		Check CmdBufLen																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( CmdBufLen < 4 )
		{
			return( 0 );								/* CmdBufLen is Short						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Calculate CmdMsgLen													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 return( 8 );
		case MFC_SVXMSG16:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_ONEREG: return(  8 + s );
			 case SFC_WR_ONEREG: return( 10 + s );
			 case SFC_RD_SEQREG: return( 10 + s );
			 case SFC_WR_SEQREG: LPX_GET_CMDMSGLEN( 6 + s, 2, 12 + s );
			 case SFC_RD_RANREG: LPX_GET_CMDMSGLEN( 4 + s, 2,  8 + s );
			 case SFC_WR_RANREG: LPX_GET_CMDMSGLEN( 4 + s, 4, 10 + s );
			 case SFC_RD_MAXNUM: return(  8 + s );
			 default: return( -1 );
			 }
//			 return( -1 );//<S0C7>statement is unreachable
		case MFC_SVXMSG32:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_SEQMEM: return( 12 + s );
			 case SFC_WR_SEQMEM: LPX_GET_CMDMSGLEN( 8 + s, 2, 12 + s );
			 default: return( -1 );
			 }
//			 return( -1 );//<S0C7>statement is unreachable
		case MFC_MECHATROLINK:
			 switch( CmdBuf[4] )
			 {
			 case SFC_RD_REQREG_42:
				 return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + MBUS42_CRC_SIZE );
			 case SFC_RD_RANREG_42:
			 	return( MBUS42_HEADERSIZE + (MBUS42_ADRSIZE * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 case SFC_WR_SEQREG_42:
			 	switch( CmdBuf[5] & 0x0F )
			 	{
		 		case MBUS42_DTYPE_B: /* データ型：Byte(8bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (1 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_S: /* データ型：Byte(16bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (2 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_L: /* データ型：Byte(32bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (4 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 	default: return( -1 );
			 	}
			 case SFC_WR_RANREG_42:
			 	return( MBUS42_HEADERSIZE + ((MBUS42_ADRSIZE + 4) * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 case SFC_RD_MAXNUM_42:
				 	return( MBUS42_HEADERSIZE + MBUS42_CRC_SIZE );
			 case SFC_RD_PRDCTID_42:
			 default: return( -1 );
			 }
		case MFC_SYSDWNLD:								/* @@@ Add Later @@@						*/
			 return( MbusFuncSysDLGetMsgLength(CmdBuf) );
/* <S150> >>>>> */
		case MFC_RELAY:		/* 中継コマンド自体にCRCは付かない */
							/* UARTでは 6 or 8バイト以上受信時にも本関数を使用するため、中継コマンドは破棄される可能性あり */
			 Offset = (CmdBuf[5] & 0x0F) * MBUS51_PASS_DATA_SIZE + MBUS51_FIX_SIZE;
			 if( CmdBufLen < Offset )	/* パケットサイズ取得不能の場合 */
			 {
				 return( 0 );
			 }
			 return( Offset +
			 	 	 ((ULONG)CmdBuf[Offset-4] << 24)
					|((ULONG)CmdBuf[Offset-3] << 16)
					|((ULONG)CmdBuf[Offset-2] <<  8)
					|((ULONG)CmdBuf[Offset-1]      ) );
/* <<<<< <S150> */
		default:
			 if( KPI_CHK_OPEMSG( CmdBuf[1] ) )
			 {
				return( CmdBuf[2] + (CmdBuf[3]<<8) );
			 }
			 return( -1 );
		}
/*--------------------------------------------------------------------------------------------------*/
		/* unreachable return( -1 );  */				/* Unknown Function Code					*/
}


/****************************************************************************************************/
/*																									*/
/*		指令メッセージ軸インデクス取得(Jupiter用)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		仮処置：当面、CmdBuf[0]による軸アドレス指定と、CmdBuf[4](Slot指定)による軸アドレス指定の	*/
/*				両方を可能としておく。																*/
/*				最終的には、CmdBuf[4](Slot指定)による軸アドレス指定にする							*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetCmdMsgAxisIdx( HMSGIF Hmsg )
{
LONG	x;
LONG	AxisNo = Hmsg->CmdBuf[0];
/*--------------------------------------------------------------------------------------------------*/
/*		軸番号補正(メッセージ種別依存)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( Hmsg->f.SifMsgIF || Hmsg->f.UsbMsgIF )
		{
			x = RpxMsgifGetCmdMsgSubAddr( Hmsg );
			if( x >= 1 )
			{
				AxisNo = x;
			}
			else if( x == -1 )
			{
				AxisNo = Hmsg->LastAxisIdx + 1;
			}
			if( Hmsg->f.SifMsgIF && AxisNo == 0 ){ AxisNo = 1;}
		}
		else
		{
			;	/* 追加要	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( AxisNo - 1 );
}

/* <S113> Start */
/****************************************************************************************************/
/*																									*/
/*		オペレータのユーザー定数表示選択															*/
/*																									*/
/****************************************************************************************************/
USHORT LpxPrmGetOpeDspMode( AXIS_HANDLE *pAxRsc)
{
	PRMDATA		*Prm;
	USHORT		rc;

	Prm = pAxRsc->UniPrms->Prm;		/* パラメータ設定データ */

	if( (Prm->b_prmB & 0x000F) == OPEDSP_SETUPPRM )
	{
		/* セットアップ用ユーザー定数のみ表示 */
		rc = OPEDSP_SETUPPRM;
	}
	else if( (Prm->b_prmB & 0x000F) == OPEDSP_ALLPRM )
	{
		/* すべてのユーザー定数を表示 */
		rc = OPEDSP_ALLPRM;
	}
	else
	{
		/* Do Nothing */
	}

	return(rc);

}
/* <S113> End */

/****************************************************************************************************///<S197>
/*																									*/
/*		オペレータのユーザー定数表示 強制変更（すべてのユーザー定数を表示）										*/
/*																									*/
/****************************************************************************************************/
void LpxPrmChangeOpeDspMode()
{
	LopMsg.LstOpeDspMode = LopMsg.OpeDspMode;
	LopMsg.OpeDspMode = OPEDSP_ALLPRM;
}

/****************************************************************************************************///<S197>
/*																									*/
/*		オペレータのユーザー定数表示 強制変更（元に戻す）														*/
/*																									*/
/****************************************************************************************************/
void LpxPrmReChangeOpeDspMode()
{
	LopMsg.OpeDspMode = LopMsg.LstOpeDspMode;
}

/***************************************** end of file **********************************************/
