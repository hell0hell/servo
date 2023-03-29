/************************************************************************************************************/
/*																											*/
/*																											*/
/*		JL086Core.c : Mercury CortexR4F関連処理																*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		CortexR4F設定ソースファイル																			*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.31  Y.Tanaka		初版															*/
/*																											*/
/************************************************************************************************************/
#include	"BaseDef.h"
#include	"ARMPF_SCU.h"
#include	"ARMPF_VIC.h"
#include	"Interrupt.h"
#ifndef _MERCURY_PLATFORM_		/*<S190>*/
#include	"MercuryGlobals.h"			/* <S044> */
#include	"SerialEncIf.h"		/* <S076> */
#endif	/*_MERCURY_PLATFORM_*/
#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/
#include	"ARMPF_SRAM.h"
#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/


/****************************************************************************************************/
/*																									*/
/*		外部関数extern宣言																			*/
/*																									*/
/****************************************************************************************************/
extern void InitSFlashSoC( void );
#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/
extern void InitSRAMMemc( void );							/* ROM/SRAMコントローラ関連初期化処理	*/
#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
void InitCorePeripheral( void );							/* JL086初期化メイン処理				*/
static void InitSCU( void );								/* SCUレジスタ初期化処理				*/
static void InitScuClock( void );						/* SCUクロック関連初期化処理			*/
static void InitScuUDLReg( void );						/* SCU UDL関連初期化処理				*/
static void InitScuINTCReg( void );						/* SCU割込み関連初期化処理				*/
static void InitScustack( void );						/* SCUスタック領域初期化処理			*//*<S127>*/
#ifdef	_PCIE_USE_
static void InitScuPCIeReg( void );						/* SCU PCIe関連初期化処理				*/
#endif
#ifdef	_GBETHER_USE_
static void InitScuGbEtherReg( void );					/* SCU GbEther関連初期化処理			*/
#endif
#ifndef _MERCURY_PLATFORM_			/*<S190>*/
void ScuSoftReset( AXIS_HANDLE	*AxisHandle );			/* JL086ソフトリセット処理		<S044>		*//* <S076> */
#endif	/*_MERCURY_PLATFORM_*/

/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		JL086CPUペリフェラル初期化処理																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：CortexR4ペリフェラルの初期化															*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void InitCorePeripheral( void )
{

	/* Initialize SCU */
	InitSCU();

	/* Initialize SerialFlashIF */
	InitSFlashSoC();

#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/
	/* Initialize ROM/SRAM */
	InitSRAMMemc();
#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/

#ifndef _MERCURY_PLATFORM_			/*<S190>*/
	/* Initialize stack *//* <S127> */
	InitScustack();

	/* Initialize VIC */
	InitInterrupt();
#endif /*_MERCURY_PLATFORM_*/
}


/****************************************************************************************************/
/*																									*/
/*		SCU初期化処理																				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SCUの初期化(モジュール別)																*/
/*				REMAPを行なう場合はは本関数内で行なう												*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitSCU( void )
{
	/* Core ID check */
//	while(SCU_GET_ID() != JL086ID)
	if( JL086ID != SCU_GET_ID() )		/* ES4以降対応<S191> */
	{/* アラーム処理を入れる	*/
		;
	}

//	SCU_GET_VER();		/* ES4以降対応 versionによる処理が必要になった時に有効にする <S191> */
	
	/* set module clock */
	InitScuClock();
	
	/* UDL関連設定		*/
	InitScuUDLReg();

#ifndef _MERCURY_PLATFORM_			/*<S190>*/
	/* 割込み関連設定		*/
	InitScuINTCReg();
#endif /*_MERCURY_PLATFORM_*/

#ifdef	_PCIE_USE_
	/* PCIe関連設定		*/
	InitScuPCIeReg();
#endif

#ifdef	_GBETHER_USE_
	/* PCIe関連設定		*/
	InitScuGbEtherReg();
#endif


	return;

}


/****************************************************************************************************/
/*																									*/
/*		SCUクロック関連初期化処理																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SSPRO7_CLK_EN, SSPRO7_CLK_SELの初期化													*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuClock( void )
{

	/* SSPRO7_CLK_EN			*/
	SCU_CLK_START( UDL_NET_CLK );									/* M3UDLクロック供給			*/
	SCU_CLK_START( UDL_SERVO_CLK );									/* ServoUDLクロック供給			*/
#if (SVFSEL_ADC_USE == 1)||(SVFSEL_ONLY_OH_ADC_USE == 1)			/* <S0E0>:SVFSEL_ONLY_OH_ADC_USE追加 */
	SCU_CLK_START( ADC_CLK );										/* ADCクロック供給				*/
#else
	SCU_CLK_STOP( ADC_CLK );										/* ADCクロック停止				*/
#endif
	SCU_CLK_STOP( GBETHER_CLK );									/* GbEtherクロック停止			*/
	SCU_CLK_STOP( DDR3_CLK );										/* DDR3クロック停止				*/
	SCU_CLK_START( USB_CLK );										/* USBクロック供給				*/
	SCU_CLK_STOP( BUSOUT_CLK );										/* BUSCLK_OUTクロック停止		*/

	/* SSPRO7_CLK_SEL			*/
//for ES1,2	<S152>
	SCU_CLK_SEL( I2C_CKSEL, CLK_MODE2 );							/* I2Cクロック 384Kbps			*/

//for ES3,4	<S152>
	SCU_CLK_SEL( I2C_CKSEL, CLK_MODE4 );							/* I2Cクロック APBREG15で設定	*/
	SCU_SET_APBREG15( LOW_DISABLE, RATE_D3, CLK_828 );

//	SCU_CLK_SEL( DDR3_CKSEL, CLK_MODE2 );							/* DDR3クロック 600Mbps			*/
//	SCU_CLK_SEL( OUT_CKSEL, CLK_MODE2 );							/* 外部バスクロック 125MHz		*/

	return;
}


/****************************************************************************************************/
/*																									*/
/*		SCU UDL関連初期化処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SSPRO7_WDT_MODE, SSPRO7_UDLCNT, SSPRO7_UDLINT_STATUS初期設定		*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuUDLReg( void )
{

	/* SSPRO7_WDT_MODE			*/
	SCU_DISABLE_WDT_RST();											/* UDL_WDTリセット無効			*/

#ifdef	_M3UDL_MICRO_DOWNLOAD_
	/* SSPRO7_UDLCNT			*/
	SCU_SET_UDLM3_PRGSEL();											/* M3UDL PRGSEL有効				*/
#endif

	/* SSPRO7_UDLINT_STATUS			*/
	SCU_READ_UDLINT_STATUS();										/* UDL INT割込み要因クリア		*/



}


/****************************************************************************************************/
/*																									*/
/*		SCU割込み関連初期化処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SSPRO7_CPUSS_TRIGINT, SSPRO7_NMI_SET, SSPRO7_INTSYS1_EN,								*/
/*				SSPRO7_INTSYS2_EN, SSPRO7_VICIFSYN_EN初期設定										*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuINTCReg( void )
{

	/* SSPRO7_CPUSS_TRIGINT			*/
	SCU_READ_CPUSS_TRIGINT();										/* クロストリガ割込み要因クリア	*/

	/* SSPRO7_NMI_SET			*/
	SCU_ENABLE_FIQ_MASK();											/* FIQをマスク可				*/

	/* SSPRO7_INTSYS1_EN			*/
//	SCU_INTSYS1_EN(ARMPF_LONGALL1);									/* 周辺回路割込みすべて同期		*//* <S0E2> */

	/* SSPRO7_INTSYS2_EN			*/
//	SCU_INTSYS2_EN(ARMPF_LONGALL1);									/* 周辺回路割込みすべて同期		*//* <S0E2> */

	/* SSPRO7_VICIFSYN_EN			*/
//	SCU_SET_VICIFSYN();												/* VIC-CPU間同期				*//* <S0E2> */

}


/****************************************************************************************************/
/*																									*/
/*		SCUスタック領域初期化処理									<S127>								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：スタック領域の終端20hをF詰めする（アラームチェック用）												*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitScustack( void )							/* SCUスタック領域初期化処理			*/
{
	LONG i;

	for(i=0;i<(0x20/4);i++)
	{
		*(VULONG*)(0x67FAF600 + i*4) = 0xFFFFFFFF;
	}
}


#ifdef	_PCIE_USE_
/****************************************************************************************************/
/*																									*/
/*		SCU PCIe関連初期化処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SSPRO7_PCIE_INT, SSPRO7_PCIE_INTMSK, SSPRO7_PCIE_INTSEL,								*/
/*				SSPRO7_PCIE_CLKSEL, SSPRO7_UDLINT_CONT初期設定										*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuPCIeReg( void )
{

	/* SSPRO7_UDLINT_CONT			*/
	SCU_PCI_EP_A_SEL( UDL_INTA );								/* PCI EP割込みAにUDL_INTA出力(仮)	*/
	SCU_PCI_EP_B_SEL( UDL_INTB );								/* PCI EP割込みBにUDL_INTB出力(仮)	*/
	SCU_PCI_EP_C_SEL( UDL_INTC );								/* PCI EP割込みCにUDL_INTC出力(仮)	*/
	SCU_PCI_EP_S_SEL( UDL_INTN );								/* PCI EP割込みDにUDL_INTN出力(仮)	*/


}
#endif


#ifdef	_GBETHER_USE_
/****************************************************************************************************/
/*																									*/
/*		SCU GbEther関連初期化処理																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SSPRO7_GBETHER_CONT初期設定															*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuGbEtherReg( void )
{


}
#endif

#ifndef _MERCURY_PLATFORM_			/*<S190>*/
/****************************************************************************************************/
/*																									*/
/*		JL086SoCソフトリセット処理								<S044><S076>						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：CortexR4SoCのソフトリセット															*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void ScuSoftReset( AXIS_HANDLE	*AxisHandle )
{
	BOOL			softResetSts;
	BOOL			EncExchgSupported;	/* <S09C> */
	AXIS_HANDLE 	*AxisRsc;
	ASIC_HW_IF_STR 	*AsicHwReg;
	MENC_INIT_PRM	MencInitPrm;
	PRMDATA 		*Prm;
	SENC_ERROR      err[MAX_AXIS_NUMBER];
	SHORT			ax_no;
	ULONG 			wtimer;				/* <S1C5> */


	AsicHwReg = AxisHandle->SvAsicRegs->AsicHwReg;
	Prm = AxisHandle->UniPrms->Prm;
	softResetSts = 0;

	/*------------------------------------------------------------------------------*/
	/*		Get Parameters related to Motor Encoder									*/
	/*------------------------------------------------------------------------------*/
	MencInitPrm.MencP = &(Prm->MencP);
	MencInitPrm.b_prm2 = Prm->b_prm2;
	MencInitPrm.b_prm80 = Prm->b_prm80;
	MencInitPrm.mtlmt = Prm->mtlmt;
	MencInitPrm.scalepitch2l = Prm->scalepitch2l;
	MencInitPrm.spgfail = Prm->spgfail;

	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisHandle[ax_no];
//		softResetSts |= ((AxisRsc->SeqCtrlOut->BeComplete)|(AxisRsc->SeqCtrlOut->TgOnSts));
		softResetSts |= ((AxisRsc->SeqCtrlOut->BeComplete)||(!(AxisRsc->SeqCtrlOut->MotStop)));		/* リセット可能条件の見直し<S119> */
	}

	if( softResetSts == 0 )
	{/* 「サーボオンでない」＆「モータ回転中でない」ときのみリセット可*/
		
		__DI();							/* リセット前に割り込みを止める */

		/* 通信ASIC,コマンドオプションリセット *//* <S14E> */
#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )
        NdResetAsic( (VOID*)HW_NETLSI_BASE_ADDRESS );
#endif
   		HALdrv_ClearDebugSP( OP_SWRST );//オプションリセット (GPIO13) オン	/* <S106> */
		KlibWaitms(100); //100ms 待つ								/* <S106> */
		HALdrv_OutputDebugSP( OP_SWRST ); //オプションリセット オフ			/* <S106> */

		for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )		/* SGD7Wのソフトリセットに対応<S119> */
		{
			MENCV			*MencV;
			MencV = (&AxisHandle[ax_no])->MencV;
			/* エンコーダ初期化用パラメータ定義へのポインタセット */	/* <S1C5> */
			MencInitPrm.MencP = &((&AxisHandle[ax_no])->UniPrms->Prm->MencP);
			if( (TRUE == MencV->EncConnect) && (SENC_COMRATE_8MBPS == MencV->PgBaudRate) )
			{/* エンコーダ接続&8Mbps通信確認 */
	//			SencChangeP0Mode( MencV );		/* エンコーダP0モード切替 *//* <S0F8> */
				SencInitChangeP0Mode( MencV );	/* エンコーダP0モード切替 *//* <S0F8> */
				KlibRstLongTimer( &wtimer );	/* 切替失敗でもタイムアウトで抜けるように修正 */	/* <S1C5> */
				do{
					/* 通信速度4Mbps切り替え */
					err[0] = SencExchngBaudrate(MencV,
												&MencInitPrm,
												AsicHwReg,
												SENC_COMRATE_4MBPS,
												&EncExchgSupported );	/*<S09C>*/
					KlibWaitus(10);
//-				}while( err[0] != SENCERR_SUCCESS );
				/* 切替失敗でもタイムアウト(5秒)で抜けるように修正 */	/* <S1C5> */
				}while(( err[0] != SENCERR_SUCCESS ) && (KlibGetLongTimerMs(wtimer) < 5000));
				MencV->PgBaudRate = SENC_COMRATE_4MBPS;
			}
		}
		SCU_SOFTRESET();
	}

	return;
}
#endif	/*_MERCURY_PLATFORM_*/


/*-------end of file----------------------------------------------------------------------------------------*/
