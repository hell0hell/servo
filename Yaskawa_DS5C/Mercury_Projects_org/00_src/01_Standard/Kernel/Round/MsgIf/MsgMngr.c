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
/*		�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
void KpxMsgManager(void);							/* Message Manager Main Function				*/
//<S150> void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc ); 	/* Initialize Message Interface					*/
void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap ); 	/* Initialize Message Interface					*/

/*--------------------------------------------------------------------------------------------------*/
static void	LpxSerialMsgManager( HMSGIF hmsg );				/* �V���A�����b�Z�[�W�Ǘ�				*/
//<S150> static void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc );		/* ���b�Z�[�W�h�e����������				*/
static void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap );		/* ���b�Z�[�W�h�e����������				*/

/*--------------------------------------------------------------------------------------------------*/
static void	LpxUsbMsgManager( HMSGIF hmsg );				/* <S166> USB���b�Z�[�W�Ǘ�				*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
static void LpxTstCmdManager( HMSGIF Hmsg );		/*												*/
#endif
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxCheckRecvMessage( HMSGIF Hmsg );		/* Check Received Message						*/
LONG	RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen ); /*							*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxGetCmdMsgAxisIdx( HMSGIF Hmsg );		/* �w�߃��b�Z�[�W���ԍ��擾						*/
/*--------------------------------------------------------------------------------------------------*/
static USHORT LpxPrmGetOpeDspMode( AXIS_HANDLE *pAxRsc );	/* �I�y���[�^�̃��[�U�[�萔�\�����[�h�擾 */	/* <S113> */
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PANELOP_USE == 1
static void	LpxPopMsgManager( HMSGIF hmsg );		/* �p�l��OP���b�Z�[�W�Ǘ�						*/
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
//RNDWRTD	RndWrtDisable;						/* �f�[�^�����݋֎~�t���O							*/

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
/*		���b�Z�[�W�Ǘ�																				*/
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
/* M3MsgIf.c�̃��b�Z�[�W��������R�[������� */
void KpxMsgManager2( UCHAR Channel )
{
	LpxSerialMsgManager( &M3Msg[Channel] );
}
#endif	/* <S150> */

/****************************************************************************************************/
/*																									*/
/*		�V���A�����b�Z�[�W�Ǘ�																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A�����b�Z�[�W�̎�M���Ď����A�e�V���A�����b�Z�[�W����(LcdOp,SigmaMsg,etc)			*/
/*			�̎��s������s���B																		*/
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
/*		���b�Z�[�W��M����																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Sif->ChkCmd( Sif, &(Hmsg->CmdMsgLen) ) )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	��M���b�Z�[�W����																		*/
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
			/*	�f�W�I�y�r������()																	*/
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
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*//* <S0E4> */
				RpxLcdOpMain( &LopMsg, &DrvSel );					/* Call LcdOp Main Procedure	*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
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
/*		���b�Z�[�W��M������ : Check Timeout														*/
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
/*		<S166> USB���b�Z�[�W�Ǘ�																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A�����b�Z�[�W�̎�M���Ď����A�e�V���A�����b�Z�[�W����(LcdOp,SigmaMsg,etc)			*/
/*			�̎��s������s���B																		*/
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
/*		���b�Z�[�W��M����																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Sif->ChkCmd( Sif, &(Hmsg->CmdMsgLen) ) )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	��M���b�Z�[�W����																		*/
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
/*		���b�Z�[�W��M������ : Check Timeout														*/
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
/*		�p�l���I�y���[�^���b�Z�[�W�Ǘ�																*/
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
			 if( Hmsg->ComMode == SCM_NOCOM )	return;					/* <S14F> �ڑ��@��Ȃ�		*/
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
/*		���b�Z�[�W�h�e����������																	*/
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
/*		���b�Z�[�W�h�e����������																	*/
/*																									*/
/****************************************************************************************************/
//<S150>void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc )
void	LpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap )
{
	FUN_AXCOMMON	*AxCommon;	/* <S056> */
	
	AxCommon = ((AXIS_HANDLE*)pAxRsc)->FnManager->AxCommon;	/* <S056> */

/*--------------------------------------------------------------------------------------------------*/
/*		�V���A�����b�Z�[�W�h�e������																*/
/*--------------------------------------------------------------------------------------------------*/
#if UART0_PORT_USE	== 1
		KRI_INIT_MSGIF( &SrlMsg[COM0_UART], &LSif[COM0_UART] );	/* init com I/F (UART ch:0)			*/
		KRI_SET_FLG_SRLMSG( &SrlMsg[COM0_UART] );				/* set sif message flag				*/
//		SrlMsg[COM0_UART].AccessLvl = ACCLVL_USER1;		    	/* TODO: Set Access Lvl				*/	/* <S056>	Delete : Fn010�Őݒ肷��A�N�Z�����x����ݒ� */
//		SrlMsg[COM0_UART].AccessLvl = (USHORT)(AxCommon->AccessLevel);	/* Set Access Lvl			*/	/* <S056> *//* <S0FE>	*/
		SrlMsg[COM0_UART].AccessLvl = ACCLVL_USER2			;	/* Set Access Lvl					*/	/* <S0FE> */
		SrlMsg[COM0_UART].pAxRsc = pAxRsc;						/* TODO: ���Ƃŏ��� */
		SrlMsg[COM0_UART].AxisMap = AxisMap;					/* <S150> */
#endif
	/*----------------------------------------------------------------------------------------------*/
#if UART1_PORT_USE	== 1
		KRI_INIT_MSGIF( &SrlMsg[COM1_UART], &LSif[COM1_UART] );	/* init com I/F (UART ch:1)			*/
		KRI_SET_FLG_SRLMSG( &SrlMsg[COM1_UART] );				/* set sif message flag				*/
//		SrlMsg[COM1_UART].AccessLvl = ACCLVL_USER1;		    	/* TODO: Set Access Lvl				*/	/* <S056>	Delete : Fn010�Őݒ肷��A�N�Z�����x����ݒ� */
//		SrlMsg[COM1_UART].AccessLvl = (USHORT)(AxCommon->AccessLevel);	/* Set Access Lvl			*/	/* <S056> *//* <S0FE> */
		SrlMsg[COM1_UART].AccessLvl = ACCLVL_USER2;				/* Set Access Lvl					*/	/* <S0FE> */
		SrlMsg[COM1_UART].pAxRsc = pAxRsc;						/* TODO: ���Ƃŏ��� */
		SrlMsg[COM1_UART].AxisMap = AxisMap;					/* <S150> */
#endif
	/*----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		�p�l���I�y���[�^���W�X�^�h�e������ (���W�X�^�h�e���ʉ��̂���)								*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PANELOP_USE == 1
		MlibResetLongMemory( &PopMsg, sizeof(PopMsg)/4 );		/* ��ŏ���							*/
		VSif[VCOM0_PANEL].RxBuf = PopRxBuf;						/* ��ŏ��� 						*//* <S025> */
		VSif[VCOM0_PANEL].TxBuf = PopTxBuf;						/* ��ŏ��� 						*//* <S025> */
		KRI_INIT_MSGIF( &PopMsg, &VSif[VCOM0_PANEL] );			/* init com I/F (Virtual UART ch:0)	*/
		KRI_SET_FLG_PNLOPE( &PopMsg );							/* set panel message flag			*/
//		PopMsg.AccessLvl = ACCLVL_SYSTEM /* ACCLVL_USER1*/;		/* TODO: Set Access Lvl 			*//* <S025> */	/* <S056>	Delete : Fn010�Őݒ肷��A�N�Z�����x����ݒ� */
//		PopMsg.AccessLvl = (USHORT)(AxCommon->AccessLevel);		/* Set Access Lvl					*/	/* <S056> *//* <S0FE> */
		PopMsg.AccessLvl = ACCLVL_USER1;						/* Set Access Lvl					*/	/* <S0FE> */
		PopMsg.pAxRsc = pAxRsc;									/* TODO: ���Ƃŏ��� 				*/
		RpxPnlOpInit( );										/* Initialize Panel Operator		*/
		RpxPnlOpMonInit( &PopMsg );								/* Initialize Panel Operator		*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		LCD�I�y���[�^���W�X�^�h�e������ (���W�X�^�h�e���ʉ��̂���)									*/
/*--------------------------------------------------------------------------------------------------*/
		KRI_INIT_MSGIF( &LopMsg, &LSif[COM1_UART] );			/* set initial com I/F (UART ch:1)	*/
		KRI_SET_FLG_LCDOPE( &LopMsg );							/* set lcd message flag				*/
		LcdOpInit( &LopMsg );									/* 									*/
		LopMsg.OpeDspMode = LpxPrmGetOpeDspMode( pAxRsc );				/* Set User Paramete Display Mode	*/	/* <S113> */
//		LopMsg.AccessLvl = ACCLVL_SYSTEM /* ACCLVL_USER1*/;		/* TODO: Set Access Lvl(Debug!!)	*/	/* <S056>	Delete : Fn010�Őݒ肷��A�N�Z�����x����ݒ� */
//		LopMsg.AccessLvl = (USHORT)(AxCommon->AccessLevel);		/* Set Access Lvl					*/	/* <S056> *//* <S0FE> */
		LopMsg.AccessLvl = ACCLVL_USER1;						/* Set Access Lvl					*/	/* <S0FE> */
		LopMsg.pAxRsc = pAxRsc;									/* TODO: ���Ƃŏ��� 				*/

/*--------------------------------------------------------------------------------------------------*/
/*		USB���b�Z�[�W�h�e������																		*/
/*--------------------------------------------------------------------------------------------------*/
#if USB0_PORT_USE	== 1
		KRI_INIT_MSGIF( &UsbMsg[COM0_USB], &USif[COM0_USB] );	/* init com I/F (USB ch:0)			*/
		KRI_SET_FLG_USBMSG( &UsbMsg[COM0_USB] );				/* set sif message flag				*/
//		UsbMsg[COM0_USB].AccessLvl = ACCLVL_USER1;		    	/* TODO: Set Access Lvl 			*/	/* <S056>	Delete : Fn010�Őݒ肷��A�N�Z�����x����ݒ� */
//		UsbMsg[COM0_USB].AccessLvl = (USHORT)(AxCommon->AccessLevel);		/* TODO: Set Access Lvl */	/* <S056>	*//* <S0FE> */
		UsbMsg[COM0_USB].AccessLvl = ACCLVL_USER2;				/* TODO: Set Access Lvl				*/	/* <S0FE>	*/
		UsbMsg[COM0_USB].pAxRsc = pAxRsc;						/* TODO: ���Ƃŏ��� 				*/
		UsbMsg[COM0_USB].AxisMap = AxisMap;						/* <S150> */
#endif
		
/*--------------------------------------------------------------------------------------------------*/
/*		�l�b�g���[�N���b�Z�[�W�h�e������ (���W�X�^�h�e���ʉ��̂���)									*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
		IxiInitNetMessage( &NetMsg, pAxRsc );					/* TODO: ��ł��� 					*/
#endif

/* <S150> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/*		MECHATROLINK���b�Z�[�W�h�e������															*/
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
			Hmsg->pAxRsc = pAxRsc;						/* TODO: ���Ƃŏ��� 				*/
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
/*		MessageI/F : ��M���b�Z�[�W�`�F�b�N															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ��M���b�Z�[�W�̃`�F�b�N���s���B														*/
/*																									*/
/*			1) ��M���b�Z�[�W���̃`�F�b�N															*/
/*			2) ��M���b�Z�[�W�̃T���`�F�b�N															*/
/*			3) ���A�h���X�̃`�F�b�N(MEMOBUS)														*/
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
				return (OK);					/* USB����̈ꕔ���b�Z�[�W��CRC���Ȃ�!! */
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
/*		MessageI/F : �w�߃��b�Z�[�W���擾(for Kernel\Driver\SifMbus.c)								*/
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
		 		case MBUS42_DTYPE_B: /* �f�[�^�^�FByte(8bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (1 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_S: /* �f�[�^�^�FByte(16bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (2 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_L: /* �f�[�^�^�FByte(32bit) */
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
		case MFC_RELAY:		/* ���p�R�}���h���̂�CRC�͕t���Ȃ� */
							/* UART�ł� 6 or 8�o�C�g�ȏ��M���ɂ��{�֐����g�p���邽�߁A���p�R�}���h�͔j�������\������ */
			 Offset = (CmdBuf[5] & 0x0F) * MBUS51_PASS_DATA_SIZE + MBUS51_FIX_SIZE;
			 if( CmdBufLen < Offset )	/* �p�P�b�g�T�C�Y�擾�s�\�̏ꍇ */
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
/*		�w�߃��b�Z�[�W���C���f�N�X�擾(Jupiter�p)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�����u�F���ʁACmdBuf[0]�ɂ�鎲�A�h���X�w��ƁACmdBuf[4](Slot�w��)�ɂ�鎲�A�h���X�w���	*/
/*				�������\�Ƃ��Ă����B																*/
/*				�ŏI�I�ɂ́ACmdBuf[4](Slot�w��)�ɂ�鎲�A�h���X�w��ɂ���							*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetCmdMsgAxisIdx( HMSGIF Hmsg )
{
LONG	x;
LONG	AxisNo = Hmsg->CmdBuf[0];
/*--------------------------------------------------------------------------------------------------*/
/*		���ԍ��␳(���b�Z�[�W��ʈˑ�)																*/
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
			;	/* �ǉ��v	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( AxisNo - 1 );
}

/* <S113> Start */
/****************************************************************************************************/
/*																									*/
/*		�I�y���[�^�̃��[�U�[�萔�\���I��															*/
/*																									*/
/****************************************************************************************************/
USHORT LpxPrmGetOpeDspMode( AXIS_HANDLE *pAxRsc)
{
	PRMDATA		*Prm;
	USHORT		rc;

	Prm = pAxRsc->UniPrms->Prm;		/* �p�����[�^�ݒ�f�[�^ */

	if( (Prm->b_prmB & 0x000F) == OPEDSP_SETUPPRM )
	{
		/* �Z�b�g�A�b�v�p���[�U�[�萔�̂ݕ\�� */
		rc = OPEDSP_SETUPPRM;
	}
	else if( (Prm->b_prmB & 0x000F) == OPEDSP_ALLPRM )
	{
		/* ���ׂẴ��[�U�[�萔��\�� */
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
/*		�I�y���[�^�̃��[�U�[�萔�\�� �����ύX�i���ׂẴ��[�U�[�萔��\���j										*/
/*																									*/
/****************************************************************************************************/
void LpxPrmChangeOpeDspMode()
{
	LopMsg.LstOpeDspMode = LopMsg.OpeDspMode;
	LopMsg.OpeDspMode = OPEDSP_ALLPRM;
}

/****************************************************************************************************///<S197>
/*																									*/
/*		�I�y���[�^�̃��[�U�[�萔�\�� �����ύX�i���ɖ߂��j														*/
/*																									*/
/****************************************************************************************************/
void LpxPrmReChangeOpeDspMode()
{
	LopMsg.OpeDspMode = LopMsg.LstOpeDspMode;
}

/***************************************** end of file **********************************************/
