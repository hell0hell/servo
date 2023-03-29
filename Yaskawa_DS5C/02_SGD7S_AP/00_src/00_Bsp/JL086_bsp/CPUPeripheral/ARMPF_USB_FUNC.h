/****************************************************************************
Description	: USB function driver(ARMPF_USB_FUNC.h)
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  : none

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.05.12		created

****************************************************************************/
#ifndef U2F_REG_H__
#define U2F_REG_H__

/*===========================================================================*/
/* I N C L U D E                                                             */
/*===========================================================================*/
#include "Basedef.h"
#include "ARMPF.h"

/*===========================================================================*/
/* S T R U C T                                                               */
/*===========================================================================*/

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
#define	ARMPF_U2F_BASE					((UINT32)0xEF040000)

#define EPn_CONTROL_ADR				((UINT32)(ARMPF_U2F_BASE + 0x00000040))
#define EPn_STATUS_ADR				((UINT32)(ARMPF_U2F_BASE + 0x00000044))
#define EPn_INT_ENA_ADR				((UINT32)(ARMPF_U2F_BASE + 0x00000048))
#define EPn_DMA_CTRL_ADR			((UINT32)(ARMPF_U2F_BASE + 0x0000004C))
#define EPn_PCKT_ADRS_ADR			((UINT32)(ARMPF_U2F_BASE + 0x00000050))
#define EPn_LEN_DCNT_ADR			((UINT32)(ARMPF_U2F_BASE + 0x00000054))
#define EPn_READ_ADR				((UINT32)(ARMPF_U2F_BASE + 0x00000058))
#define EPn_WRITE_ADR				((UINT32)(ARMPF_U2F_BASE + 0x0000005C))

/* --- EPn DMA --- */
#define EPnDCR1_ADR					((UINT32)(ARMPF_U2F_BASE + 0x00001110))
#define EPnDCR2_ADR					((UINT32)(ARMPF_U2F_BASE + 0x00001114))
#define EPnTADR_ADR					((UINT32)(ARMPF_U2F_BASE + 0x00001118))


/*===========================================================================*/
/* ARMPF_USB_CONTROL [0x000]														 */
/*===========================================================================*/
#define D_USB_F_RST					BIT0
#define D_USB_PHY_RST				BIT1
#define D_USB_PUE2					BIT2
#define D_USB_CONNECTB				BIT3
#define D_USB_DEFAULT				BIT4
#define D_USB_CONF					BIT5
#define D_USB_SUSPEND				BIT6
#define D_USB_RSUM_IN				BIT7
#define D_USB_SOF_RCV				BIT8
#define D_USB_CONSTFS				BIT9
#define D_USB_INT_SEL				BIT10
#define D_USB_USBTESTMODE			(BIT16 | BIT17 | BIT18)

/*===========================================================================*/
/* ARMPF_USB_STATUS [0x004]														 */
/*===========================================================================*/
#define D_USB_VBUS_LEVEL			BIT0
#define D_USB_RSUM_OUT				BIT1
#define D_USB_SPND_OUT				BIT2
#define D_USB_USB_RST				BIT3
#define D_USB_DEFAULT				BIT4
#define D_USB_CONF_ST				BIT5
#define D_USB_SPEED_MODE			BIT6

/*===========================================================================*/
/* ARMPF_USB_ADDRESS [0x008]														 */
/*===========================================================================*/
#define D_USB_SOF_STATUS			BIT15
#define D_USB_USB_ADDR				((UINT32)0x007F0000)

/*===========================================================================*/
/* ARMPF_USB_INT_STA [0x020]														 */
/*===========================================================================*/
#define D_USB_VBUS_LEVEL			BIT0
#define D_USB_RSUM_INT				BIT1
#define D_USB_SPND_INT				BIT2
#define D_USB_USB_RST_INT			BIT3
#define D_USB_SOF_INT				BIT4
#define D_USB_SOF_ERROR_INT			BIT5
#define D_USB_SPEED_MODE_INT		BIT6
#define D_USB_VBUS_INT				BIT7
#define D_USB_EP0_INT				BIT8
#define D_USB_EP1_INT				BIT9
#define D_USB_EP2_INT				BIT10
#define D_USB_EP3_INT				BIT11
#define D_USB_EP4_INT				BIT12
#define D_USB_EP5_INT				BIT13
#define D_USB_EP6_INT				BIT14
#define D_USB_EP7_INT				BIT15
#define D_USB_EP8_INT				BIT16
#define D_USB_EP9_INT				BIT17
#define D_USB_EPn_INT				((UINT32)0x00FFFF00)		/* EP0, EP1-EP15 */

/*===========================================================================*/
/* USB_INT_ENA [0x024]														 */
/*===========================================================================*/
#define D_USB_RSUM_EN				BIT1
#define D_USB_SPND_EN				BIT2
#define D_USB_USB_RST_EN			BIT3
#define D_USB_SOF_EN				BIT4
#define D_USB_SOF_ERROR_EN			BIT5
#define D_USB_SPEED_MODE_EN			BIT6
#define D_USB_VBUS_EN				BIT7
#define D_USB_EP0_EN				BIT8
#define D_USB_EP1_EN				BIT9
#define D_USB_EP2_EN				BIT10
#define D_USB_EP3_EN				BIT11
#define D_USB_EP4_EN				BIT12
#define D_USB_EP5_EN				BIT13
#define D_USB_EP6_EN				BIT14
#define D_USB_EP7_EN				BIT15
#define D_USB_EP8_EN				BIT16
#define D_USB_EP9_EN				BIT17
#define D_USB_EPn_EN				((UINT32)0x00FFFF00)		/* EP0, EP1-EP15 */

/*===========================================================================*/
/* ARMPF_EP0_CONTROL [0x028]														 */
/*===========================================================================*/
#define D_EP0_ONAK					BIT0
#define D_EP0_INAK					BIT1
#define D_EP0_STL					BIT2
#define D_EP0_PERR_NAK_CLR			BIT3
#define D_EP0_INAK_EN				BIT4
#define D_EP0_DW					(BIT5 | BIT6)
#define D_EP0_DEND					BIT7
#define D_EP0_BCLR					BIT8
#define D_EP0_PIDCLR				BIT9
#define D_EP0_AUTO					BIT16
#define D_EP0_OVERSEL				BIT17
#define D_EP0_STGSEL				BIT18

/*===========================================================================*/
/* ARMPF_EP0_STATUS [0x02C]														 */
/*===========================================================================*/
#define D_EP0_SETUP_INT				BIT0
#define D_EP0_STG_START_INT			BIT1
#define D_EP0_STG_END_INT			BIT2
#define D_EP0_STALL_INT				BIT3
#define D_EP0_IN_INT				BIT4
#define D_EP0_OUT_INT				BIT5
#define D_EP0_OUT_OR_INT			BIT6
#define D_EP0_OUT_NULL_INT			BIT7
#define D_EP0_IN_EMPTY				BIT8
#define D_EP0_IN_FULL				BIT9
#define D_EP0_IN_DATA				BIT10
#define D_EP0_IN_NAK_INT			BIT11
#define D_EP0_OUT_EMPTY				BIT12
#define D_EP0_OUT_FULL				BIT13
#define D_EP0_OUT_NULL				BIT14
#define D_EP0_OUT_NAK_INT			BIT15
#define D_EP0_PERR_NAK_INT			BIT16
#define D_EP0_PERR_NAK				BIT17
#define D_EP0_PID					BIT18

/*===========================================================================*/
/* ARMPF_EP0_INT_ENA [0x030]														 */
/*===========================================================================*/
#define D_EP0_SETUP_EN				BIT0
#define D_EP0_STG_START_EN			BIT1
#define D_EP0_STG_END_EN			BIT2
#define D_EP0_STALL_EN				BIT3
#define D_EP0_IN_EN					BIT4
#define D_EP0_OUT_EN				BIT5
#define D_EP0_OUT_OR_EN				BIT6
#define D_EP0_OUT_NULL_EN			BIT7
#define D_EP0_IN_NAK_EN				BIT11
#define D_EP0_OUT_NAK_EN			BIT15
#define D_EP0_PERR_NAK_EN			BIT16

/*===========================================================================*/
/* EP0_LENGTH [0x034]														 */
/*===========================================================================*/
#define D_EP0_LDATA					((UINT32)0x0000007F)

/*===========================================================================*/
/* EPn_CONTROL_BIT															 */
/*===========================================================================*/
#define D_EPn_ONAK					BIT0
#define D_EPn_OSTL					BIT2
#define D_EPn_ISTL					BIT3
#define D_EPn_OSTL_EN				BIT4
#define D_EPn_DW					(BIT5 | BIT6)
#define D_EPn_DEND					BIT7
#define D_EPn_CBCLR					BIT8
#define D_EPn_BCLR					BIT9
#define D_EPn_OPIDCLR				BIT10
#define D_EPn_IPIDCLR				BIT11
#define D_EPn_AUTO					BIT16
#define D_EPn_OVERSEL				BIT17
#define D_EPn_MODE					(BIT24 | BIT25)
#define D_EPn_DIR0					BIT26
#define D_EPn_BUF_TYPE				BIT30
#define D_EPn_EN					BIT31

/*===========================================================================*/
/* EPn_STATUS_BIT															 */
/*===========================================================================*/
#define D_EPn_IN_EMPTY				BIT0
#define D_EPn_IN_FULL				BIT1
#define D_EPn_IN_DATA				BIT2
#define D_EPn_IN_INT				BIT3
#define D_EPn_IN_STALL_INT			BIT4
#define D_EPn_IN_NAK_ERR_INT		BIT5
#define D_EPn_IN_END_INT			BIT7
#define D_EPn_IPID					BIT10
#define D_EPn_OUT_EMPTY				BIT16
#define D_EPn_OUT_FULL				BIT17
#define D_EPn_OUT_NULL_INT			BIT18
#define D_EPn_OUT_INT				BIT19
#define D_EPn_OUT_STALL_INT			BIT20
#define D_EPn_OUT_NAK_ERR_INT		BIT21
#define D_EPn_OUT_OR_INT			BIT22
#define D_EPn_OUT_END_INT			BIT23
#define D_EPn_OPID					BIT28

/*===========================================================================*/
/* EPn_INT_ENA																 */
/*===========================================================================*/
#define D_EPn_IN_EN					BIT3
#define D_EPn_IN_STALL_EN			BIT4
#define D_EPn_IN_NAK_ERR_EN			BIT5
#define D_EPn_IN_END_EN				BIT7
#define D_EPn_OUT_NULL_EN			BIT18
#define D_EPn_OUT_EN				BIT19
#define D_EPn_OUT_STALL_EN			BIT20
#define D_EPn_OUT_NAK_ERR_EN		BIT21
#define D_EPn_OUT_OR_EN				BIT22
#define D_EPn_OUT_END_EN			BIT23

/*===========================================================================*/
/* EPn_DMA_CTRL																 */
/*===========================================================================*/
#define D_EPn_DMAMODE0				BIT0
#define D_EPn_DMAMODE2				BIT2
#define D_EPn_DMA_EN				BIT4
#define D_EPn_STOP_SET				BIT8
#define D_EPn_BURST_SET				BIT9
#define D_EPn_DEND_SET				BIT10
#define D_EPn_STOP_MODE				BIT11
#define D_EPn_BUS_SEL				(BIT12 | BIT13)

/*===========================================================================*/
/* EPn_PCKT_ADRS															 */
/*===========================================================================*/
#define D_EPn_MPKT					((UINT32)0x000007FF)
#define D_EPn_BASEAD				((UINT32)0x1FFF0000)

/*===========================================================================*/
/* EPn_LEN_DCNT																 */
/*===========================================================================*/
#define D_EPn_LDATA					((UINT32)0x000007FF)
#define D_EPn_DMACNT				((UINT32)0x01FF0000)

/*===========================================================================*/
/* SYSSCTR [0x1000]															 */
/*===========================================================================*/
#define D_SYS_WAIT_MODE				BIT0
#define D_SYS_NOT_RETRY_MASTER		((UINT32)0xFFFF0000)

/*===========================================================================*/
/* SYSMCTR [0x1004]															 */
/*===========================================================================*/
#define D_SYS_ARBITER_CTR			BIT31

/*===========================================================================*/
/* ARMPF_SYSBINT [0x1008]															 */
/*===========================================================================*/
#define D_SYS_ERR_MASTER			((UINT32)0x0000000F)
#define D_SYS_SBUS_ERRINT0			BIT4
#define D_SYS_SBUS_ERRINT1			BIT5
#define D_SYS_MBUS_ERRINT			BIT6
#define D_SYS_VBUS_INT				BIT13
#define D_SYS_DMA_ENDINT_EP1		BIT17
#define D_SYS_DMA_ENDINT_EP2		BIT18
#define D_SYS_DMA_ENDINT_EP3		BIT19
#define D_SYS_DMA_ENDINT_EP4		BIT20
#define D_SYS_DMA_ENDINT_EP5		BIT21
#define D_SYS_DMA_ENDINT_EP6		BIT22
#define D_SYS_DMA_ENDINT_EP7		BIT23
#define D_SYS_DMA_ENDINT_EP8		BIT24
#define D_SYS_DMA_ENDINT_EP9		BIT25
#define D_SYS_DMA_ENDINT_EPn		((UINT32)0xFFFE0000)

/*===========================================================================*/
/* SYSBINTEN [ 0x100C ]														 */
/*===========================================================================*/
#define D_SYS_SBUS_ERRINT0EN		BIT4
#define D_SYS_SBUS_ERRINT1EN		BIT5
#define D_SYS_MBUS_ERRINTEN			BIT6
#define D_SYS_VBUS_INTEN			BIT13
#define D_SYS_DMA_ENDINTEN_EP1		BIT17
#define D_SYS_DMA_ENDINTEN_EP2		BIT18
#define D_SYS_DMA_ENDINTEN_EP3		BIT19
#define D_SYS_DMA_ENDINTEN_EP4		BIT20
#define D_SYS_DMA_ENDINTEN_EP5		BIT21
#define D_SYS_DMA_ENDINTEN_EP6		BIT22
#define D_SYS_DMA_ENDINTEN_EP7		BIT23
#define D_SYS_DMA_ENDINTEN_EP8		BIT24
#define D_SYS_DMA_ENDINTEN_EP9		BIT25
#define D_SYS_DMA_ENDINTEN_EPn		((UINT32)0xFFFE0000)

/*===========================================================================*/
/* ARMPF_EPCTR [ 0x1010 ]															 */
/*===========================================================================*/
#define D_SYS_EPC_RST				BIT0
#define D_SYS_USBH_RST				BIT1
#define D_SYS_PLL_RST				BIT2
#define D_SYS_PCICLK_MASK			BIT3
#define D_SYS_PLL_LOCK				BIT4
#define D_SYS_PLL_RESUME			BIT5
#define D_SYS_VBUS_LEVEL			BIT8
#define D_SYS_DIRPD					BIT12

/*===========================================================================*/
/* USBSSVER [ 0x1020 ]														 */
/*===========================================================================*/
#define D_SYS_SS_VER				((UINT32)0x000000FF)
#define D_SYS_EPC_VER				((UINT32)0x0000FF00)
#define D_SYS_SYSB_VER				((UINT32)0x00FF0000)

/*===========================================================================*/
/* USBSSCONF [ 0x1024 ]														 */
/*===========================================================================*/
#define D_SYS_DMA_AVAILABLE			((UINT32)0x0000FFFF)
#define D_SYS_EP_AVAILABLE			((UINT32)0xFFFF0000)

/*===========================================================================*/
/* DCR1																		 */
/*===========================================================================*/
#define D_SYS_EPn_REQEN				BIT0
#define D_SYS_EPn_DIR0				BIT1
#define D_SYS_EPn_DMACNT			((UINT32)0x00FF0000)

/*===========================================================================*/
/* DCR2																		 */
/*===========================================================================*/
#define D_SYS_EPn_MPKT				((UINT32)0x000007FF)
#define D_SYS_EPn_LMPKT				((UINT32)0x07FF0000)


#define	ARMPF_USB_CONTROL			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000000)))	/* USB制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_USB_STATUS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000004)))	/* USBステータス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_USB_ADDRESS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000008)))	/* USBアドレス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))						*/
#define	ARMPF_USB_TEST_CTRL			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000010)))	/* USBテスト制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))						*/
#define	ARMPF_SETUP_DATA0			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000018)))	/* セットアップ・データ・レジスタ0(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_SETUP_DATA1			(*((VULONG*)(ARMPF_U2F_BASE + 0x0000001C)))	/* セットアップ・データ・レジスタ1(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_USB_INT_STA			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000020)))	/* USB割り込みステータス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_USB_INT_ENA			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000024)))	/* USB割り込み許可レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP0_CONTROL			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000028)))	/* EP0制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP0_STATUS			(*((VULONG*)(ARMPF_U2F_BASE + 0x0000002C)))	/* EP0ステータス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP0_INT_ENA			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000030)))	/* EP0割り込み許可レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP0_LENGTH			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000034)))	/* EP0受信データ長レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP0_READ				(*((VULONG*)(ARMPF_U2F_BASE + 0x00000038)))	/* EP0受信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP0_WRITE				(*((VULONG*)(ARMPF_U2F_BASE + 0x0000003C)))	/* EP0送信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP1_CONTROL			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000040)))	/* EP1制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP1_STATUS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000044)))	/* EP1ステータス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP1_INT_ENA			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000048)))	/* EP1割り込み許可レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP1_DMA_CTRL			(*((VULONG*)(ARMPF_U2F_BASE + 0x0000004C)))	/* EP1 DMA転送制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP1_PCKT_ADRS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000050)))	/* EP1バッファ設定レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP1_LEN_DCNT			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000054)))	/* EP1受信データ長／DMA サイズ設定レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))	*/
#define	ARMPF_EP1_READ				(*((VULONG*)(ARMPF_U2F_BASE + 0x00000058)))	/* EP1受信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP1_WRITE				(*((VULONG*)(ARMPF_U2F_BASE + 0x0000005C)))	/* EP1送信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP2_CONTROL			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000060)))	/* EP2制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP2_STATUS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000064)))	/* EP2ステータス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP2_INT_ENA			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000068)))	/* EP2割り込み許可レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP2_DMA_CTRL			(*((VULONG*)(ARMPF_U2F_BASE + 0x0000006C)))	/* EP1 DMA転送制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP2_PCKT_ADRS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000070)))	/* EP2バッファ設定レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP2_LEN_DCNT			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000074)))	/* EP2受信データ長／DMA サイズ設定レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))	*/
#define	ARMPF_EP2_READ				(*((VULONG*)(ARMPF_U2F_BASE + 0x00000078)))	/* EP2受信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP2_WRITE				(*((VULONG*)(ARMPF_U2F_BASE + 0x0000007C)))	/* EP2送信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP3_CONTROL			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000080)))	/* EP3制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP3_STATUS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000084)))	/* EP3ステータス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP3_INT_ENA			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000088)))	/* EP3割り込み許可レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP3_DMA_CTRL			(*((VULONG*)(ARMPF_U2F_BASE + 0x0000008C)))	/* EP3 DMA転送制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP3_PCKT_ADRS			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000090)))	/* EP3バッファ設定レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP3_LEN_DCNT			(*((VULONG*)(ARMPF_U2F_BASE + 0x00000094)))	/* EP3受信データ長レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))					*/
#define	ARMPF_EP3_READ				(*((VULONG*)(ARMPF_U2F_BASE + 0x00000098)))	/* EP3受信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_EP3_WRITE				(*((VULONG*)(ARMPF_U2F_BASE + 0x0000009C)))	/* EP3送信バッファ(USB2.0 Function(NBU2SSHCFUNCV01))							*/
#define	ARMPF_SYSSCTR				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001000)))	/* システム・バス・スレーブ制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))		*/
#define	ARMPF_SYSMCTR				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001004)))	/* システム・バス・マスタ制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_SYSBINT				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001008)))	/* システム・バス・ブリッジ割り込み要因レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))	*/
#define	ARMPF_SYSBINTEN				(*((VULONG*)(ARMPF_U2F_BASE + 0x0000100C)))	/* システム・バス・ブリッジ割り込み許可レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))	*/
#define	ARMPF_EPCTR					(*((VULONG*)(ARMPF_U2F_BASE + 0x00001010)))	/* EPC周辺制御レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))						*/
#define	ARMPF_USBSSVER				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001020)))	/* USBサブシステムバージョンレジスタ(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_USBSSCONF				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001024)))	/* USBサブシステムコンフィグレジスタ(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_EP1DCR1				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001110)))	/* EP1 DMAコントロール・レジスタ1(USB2.0 Function(NBU2SSHCFUNCV01))				*/
#define	ARMPF_EP1DCR2				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001114)))	/* EP1 DMAコントロール・レジスタ2(USB2.0 Function(NBU2SSHCFUNCV01))				*/
#define	ARMPF_EP1TADR				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001118)))	/* EP1 DMA転送先アドレス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))			*/
#define	ARMPF_EP2DCR1				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001120)))	/* EP2 DMAコントロール・レジスタ1(USB2.0 Function(NBU2SSHCFUNCV01))				*/
#define	ARMPF_EP2DCR2				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001124)))	/* EP2 DMAコントロール・レジスタ2(USB2.0 Function(NBU2SSHCFUNCV01))				*/
#define	ARMPF_EP2TADR				(*((VULONG*)(ARMPF_U2F_BASE + 0x00001128)))	/* EP2 DMA転送先アドレス・レジスタ(USB2.0 Function(NBU2SSHCFUNCV01))			*/

#endif	/* U2F_REG_H__ */
/*-------end of file----------------------------------------------------------------------------------------*/
