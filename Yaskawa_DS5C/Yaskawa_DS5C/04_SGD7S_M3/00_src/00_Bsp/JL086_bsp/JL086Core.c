/************************************************************************************************************/
/*																											*/
/*																											*/
/*		JL086Core.c : Mercury CortexR4F�֘A����																*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		CortexR4F�ݒ�\�[�X�t�@�C��																			*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.31  Y.Tanaka		����															*/
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
#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/
#include	"ARMPF_SRAM.h"
#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/


/****************************************************************************************************/
/*																									*/
/*		�O���֐�extern�錾																			*/
/*																									*/
/****************************************************************************************************/
extern void InitSFlashSoC( void );
#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/
extern void InitSRAMMemc( void );							/* ROM/SRAM�R���g���[���֘A����������	*/
#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
void InitCorePeripheral( void );							/* JL086���������C������				*/
static void InitSCU( void );								/* SCU���W�X�^����������				*/
static void InitScuClock( void );						/* SCU�N���b�N�֘A����������			*/
static void InitScuUDLReg( void );						/* SCU UDL�֘A����������				*/
static void InitScuINTCReg( void );						/* SCU�����݊֘A����������				*/
static void InitScustack( void );						/* SCU�X�^�b�N�̈揉��������			*//*<S127>*/
#ifdef	_PCIE_USE_
static void InitScuPCIeReg( void );						/* SCU PCIe�֘A����������				*/
#endif
#ifdef	_GBETHER_USE_
static void InitScuGbEtherReg( void );					/* SCU GbEther�֘A����������			*/
#endif
#ifndef _MERCURY_PLATFORM_			/*<S190>*/
void ScuSoftReset( AXIS_HANDLE	*AxisHandle );			/* JL086�\�t�g���Z�b�g����		<S044>		*//* <S076> */
#endif	/*_MERCURY_PLATFORM_*/

/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		JL086CPU�y���t�F��������������																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FCortexR4�y���t�F�����̏�����															*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
void InitCorePeripheral( void )
{

	/* Initialize SCU */
	InitSCU();

	/* Initialize SerialFlashIF */
	InitSFlashSoC();

#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/
	/* Initialize ROM/SRAM */
	InitSRAMMemc();
#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/

#ifndef _MERCURY_PLATFORM_			/*<S190>*/
	/* Initialize stack *//* <S127> */
	InitScustack();

	/* Initialize VIC */
	InitInterrupt();
#endif /*_MERCURY_PLATFORM_*/
}


/****************************************************************************************************/
/*																									*/
/*		SCU����������																				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSCU�̏�����(���W���[����)																*/
/*				REMAP���s�Ȃ��ꍇ�͖͂{�֐����ōs�Ȃ�												*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitSCU( void )
{
	/* Core ID check */
//	while(SCU_GET_ID() != JL086ID)
	if( JL086ID != SCU_GET_ID() )		/* ES4�ȍ~�Ή�<S191> */
	{/* �A���[������������	*/
		;
	}

//	SCU_GET_VER();		/* ES4�ȍ~�Ή� version�ɂ�鏈�����K�v�ɂȂ������ɗL���ɂ��� <S191> */
	
	/* set module clock */
	InitScuClock();
	
	/* UDL�֘A�ݒ�		*/
	InitScuUDLReg();

#ifndef _MERCURY_PLATFORM_			/*<S190>*/
	/* �����݊֘A�ݒ�		*/
	InitScuINTCReg();
#endif /*_MERCURY_PLATFORM_*/

#ifdef	_PCIE_USE_
	/* PCIe�֘A�ݒ�		*/
	InitScuPCIeReg();
#endif

#ifdef	_GBETHER_USE_
	/* PCIe�֘A�ݒ�		*/
	InitScuGbEtherReg();
#endif


	return;

}


/****************************************************************************************************/
/*																									*/
/*		SCU�N���b�N�֘A����������																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSSPRO7_CLK_EN, SSPRO7_CLK_SEL�̏�����													*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuClock( void )
{

	/* SSPRO7_CLK_EN			*/
	SCU_CLK_START( UDL_NET_CLK );									/* M3UDL�N���b�N����			*/
	SCU_CLK_START( UDL_SERVO_CLK );									/* ServoUDL�N���b�N����			*/
#if (SVFSEL_ADC_USE == 1)||(SVFSEL_ONLY_OH_ADC_USE == 1)			/* <S0E0>:SVFSEL_ONLY_OH_ADC_USE�ǉ� */
	SCU_CLK_START( ADC_CLK );										/* ADC�N���b�N����				*/
#else
	SCU_CLK_STOP( ADC_CLK );										/* ADC�N���b�N��~				*/
#endif
	SCU_CLK_STOP( GBETHER_CLK );									/* GbEther�N���b�N��~			*/
	SCU_CLK_STOP( DDR3_CLK );										/* DDR3�N���b�N��~				*/
	SCU_CLK_START( USB_CLK );										/* USB�N���b�N����				*/
	SCU_CLK_STOP( BUSOUT_CLK );										/* BUSCLK_OUT�N���b�N��~		*/

	/* SSPRO7_CLK_SEL			*/
//for ES1,2	<S152>
	SCU_CLK_SEL( I2C_CKSEL, CLK_MODE2 );							/* I2C�N���b�N 384Kbps			*/

//for ES3,4	<S152>
	SCU_CLK_SEL( I2C_CKSEL, CLK_MODE4 );							/* I2C�N���b�N APBREG15�Őݒ�	*/
	SCU_SET_APBREG15( LOW_DISABLE, RATE_D3, CLK_828 );

//	SCU_CLK_SEL( DDR3_CKSEL, CLK_MODE2 );							/* DDR3�N���b�N 600Mbps			*/
//	SCU_CLK_SEL( OUT_CKSEL, CLK_MODE2 );							/* �O���o�X�N���b�N 125MHz		*/

	return;
}


/****************************************************************************************************/
/*																									*/
/*		SCU UDL�֘A����������																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSSPRO7_WDT_MODE, SSPRO7_UDLCNT, SSPRO7_UDLINT_STATUS�����ݒ�		*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuUDLReg( void )
{

	/* SSPRO7_WDT_MODE			*/
	SCU_DISABLE_WDT_RST();											/* UDL_WDT���Z�b�g����			*/

#ifdef	_M3UDL_MICRO_DOWNLOAD_
	/* SSPRO7_UDLCNT			*/
	SCU_SET_UDLM3_PRGSEL();											/* M3UDL PRGSEL�L��				*/
#endif

	/* SSPRO7_UDLINT_STATUS			*/
	SCU_READ_UDLINT_STATUS();										/* UDL INT�����ݗv���N���A		*/



}


/****************************************************************************************************/
/*																									*/
/*		SCU�����݊֘A����������																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSSPRO7_CPUSS_TRIGINT, SSPRO7_NMI_SET, SSPRO7_INTSYS1_EN,								*/
/*				SSPRO7_INTSYS2_EN, SSPRO7_VICIFSYN_EN�����ݒ�										*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuINTCReg( void )
{

	/* SSPRO7_CPUSS_TRIGINT			*/
	SCU_READ_CPUSS_TRIGINT();										/* �N���X�g���K�����ݗv���N���A	*/

	/* SSPRO7_NMI_SET			*/
	SCU_ENABLE_FIQ_MASK();											/* FIQ���}�X�N��				*/

	/* SSPRO7_INTSYS1_EN			*/
//	SCU_INTSYS1_EN(ARMPF_LONGALL1);									/* ���Ӊ�H�����݂��ׂē���		*//* <S0E2> */

	/* SSPRO7_INTSYS2_EN			*/
//	SCU_INTSYS2_EN(ARMPF_LONGALL1);									/* ���Ӊ�H�����݂��ׂē���		*//* <S0E2> */

	/* SSPRO7_VICIFSYN_EN			*/
//	SCU_SET_VICIFSYN();												/* VIC-CPU�ԓ���				*//* <S0E2> */

}


/****************************************************************************************************/
/*																									*/
/*		SCU�X�^�b�N�̈揉��������									<S127>								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����F�X�^�b�N�̈�̏I�[20h��F�l�߂���i�A���[���`�F�b�N�p�j												*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitScustack( void )							/* SCU�X�^�b�N�̈揉��������			*/
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
/*		SCU PCIe�֘A����������																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSSPRO7_PCIE_INT, SSPRO7_PCIE_INTMSK, SSPRO7_PCIE_INTSEL,								*/
/*				SSPRO7_PCIE_CLKSEL, SSPRO7_UDLINT_CONT�����ݒ�										*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuPCIeReg( void )
{

	/* SSPRO7_UDLINT_CONT			*/
	SCU_PCI_EP_A_SEL( UDL_INTA );								/* PCI EP������A��UDL_INTA�o��(��)	*/
	SCU_PCI_EP_B_SEL( UDL_INTB );								/* PCI EP������B��UDL_INTB�o��(��)	*/
	SCU_PCI_EP_C_SEL( UDL_INTC );								/* PCI EP������C��UDL_INTC�o��(��)	*/
	SCU_PCI_EP_S_SEL( UDL_INTN );								/* PCI EP������D��UDL_INTN�o��(��)	*/


}
#endif


#ifdef	_GBETHER_USE_
/****************************************************************************************************/
/*																									*/
/*		SCU GbEther�֘A����������																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FSSPRO7_GBETHER_CONT�����ݒ�															*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
/*																									*/
/****************************************************************************************************/
static void InitScuGbEtherReg( void )
{


}
#endif

#ifndef _MERCURY_PLATFORM_			/*<S190>*/
/****************************************************************************************************/
/*																									*/
/*		JL086SoC�\�t�g���Z�b�g����								<S044><S076>						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		����FCortexR4SoC�̃\�t�g���Z�b�g															*/
/*		�����F�Ȃ�																					*/
/*		�߂�l�F�Ȃ�																				*/
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
		softResetSts |= ((AxisRsc->SeqCtrlOut->BeComplete)||(!(AxisRsc->SeqCtrlOut->MotStop)));		/* ���Z�b�g�\�����̌�����<S119> */
	}

	if( softResetSts == 0 )
	{/* �u�T�[�{�I���łȂ��v���u���[�^��]���łȂ��v�Ƃ��̂݃��Z�b�g��*/
		
		__DI();							/* ���Z�b�g�O�Ɋ��荞�݂��~�߂� */

		/* �ʐMASIC,�R�}���h�I�v�V�������Z�b�g *//* <S14E> */
#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )
        NdResetAsic( (VOID*)HW_NETLSI_BASE_ADDRESS );
#endif
   		HALdrv_ClearDebugSP( OP_SWRST );//�I�v�V�������Z�b�g (GPIO13) �I��	/* <S106> */
		KlibWaitms(100); //100ms �҂�								/* <S106> */
		HALdrv_OutputDebugSP( OP_SWRST ); //�I�v�V�������Z�b�g �I�t			/* <S106> */

		for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )		/* SGD7W�̃\�t�g���Z�b�g�ɑΉ�<S119> */
		{
			MENCV			*MencV;
			MencV = (&AxisHandle[ax_no])->MencV;
			/* �G���R�[�_�������p�p�����[�^��`�ւ̃|�C���^�Z�b�g */	/* <S1C5> */
			MencInitPrm.MencP = &((&AxisHandle[ax_no])->UniPrms->Prm->MencP);
			if( (TRUE == MencV->EncConnect) && (SENC_COMRATE_8MBPS == MencV->PgBaudRate) )
			{/* �G���R�[�_�ڑ�&8Mbps�ʐM�m�F */
	//			SencChangeP0Mode( MencV );		/* �G���R�[�_P0���[�h�ؑ� *//* <S0F8> */
				SencInitChangeP0Mode( MencV );	/* �G���R�[�_P0���[�h�ؑ� *//* <S0F8> */
				KlibRstLongTimer( &wtimer );	/* �ؑ֎��s�ł��^�C���A�E�g�Ŕ�����悤�ɏC�� */	/* <S1C5> */
				do{
					/* �ʐM���x4Mbps�؂�ւ� */
					err[0] = SencExchngBaudrate(MencV,
												&MencInitPrm,
												AsicHwReg,
												SENC_COMRATE_4MBPS,
												&EncExchgSupported );	/*<S09C>*/
					KlibWaitus(10);
//-				}while( err[0] != SENCERR_SUCCESS );
				/* �ؑ֎��s�ł��^�C���A�E�g(5�b)�Ŕ�����悤�ɏC�� */	/* <S1C5> */
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
