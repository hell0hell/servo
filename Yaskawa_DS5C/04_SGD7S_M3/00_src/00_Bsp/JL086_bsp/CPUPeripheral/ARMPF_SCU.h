/************************************************************************************************************/
/*																											*/
/*																											*/
/*		ARMPF_SCU.h : MercurySoC �y���t�F���� SCU�w�b�_														*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		�V�X�e���R���g���[�����j�b�g(SCU)�֘A�w�b�_�t�@�C��													*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.30  Y.Tanaka		����															*/
/*																											*/
/************************************************************************************************************/
#include	"ARMPF.h"


#ifndef	_ARMPF_SCU_H_
#define	_ARMPF_SCU_H_

#define	ARMPF_SCU_BASE	0xEFF86000

/*----------------------------------------------------------------------------------------------------------*/
/*																											*/
/*		SCU���W�X�^�ݒ�l																					*/
/*																											*/
/*----------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------*/
/*		SCU register																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_SCU_DEVICE_ID		(ARMPF_SCU_BASE + 0x000)		/* �f�o�C�XID�R�[�h���W�X�^					*/
#define	ARMPF_SCU_SOFT_RST		(ARMPF_SCU_BASE + 0x004)		/* �\�t�g���Z�b�g���W�X�^					*/
#define	ARMPF_SCU_CPU_HALT		(ARMPF_SCU_BASE + 0x008)		/* PCIex�u�[�g���䃌�W�X�^					*/
#define	ARMPF_SCU_CLK_EN		(ARMPF_SCU_BASE + 0x00C)		/* �N���b�N����p���W�X�^					*/
#define	ARMPF_SCU_CLK_SEL		(ARMPF_SCU_BASE + 0x010)		/* �N���b�N�؂�ւ����W�X�^					*/
#define	ARMPF_SCU_MODE			(ARMPF_SCU_BASE + 0x014)		/* ���[�h�m�F�p���W�X�^						*/
#define	ARMPF_SCU_REMAP			(ARMPF_SCU_BASE + 0x018)		/* ���}�b�v�@�\���䃌�W�X�^					*/
#define	ARMPF_SCU_PCIE_INT		(ARMPF_SCU_BASE + 0x01C)		/* PCIex�G���h�|�C���g�����݃��W�X�^		*/
#define	ARMPF_SCU_PCIE_INTMSK	(ARMPF_SCU_BASE + 0x020)		/* PCIex�G���h�|�C���g�����݃}�X�N���W�X�^	*/
#define	ARMPF_SCU_PCIE_INTSEL	(ARMPF_SCU_BASE + 0x024)		/* PCIex������UDL-IF�I�����W�X�^			*/
#define	ARMPF_SCU_WDT_MODE		(ARMPF_SCU_BASE + 0x028)		/* UDL-WDT���Z�b�g���䃌�W�X�^				*/
#define	ARMPF_SCU_5TBUF_EN		(ARMPF_SCU_BASE + 0x02C)		/* 5V�g�������g�o�b�t�@�X�^���o�C���䃌�W�X�^	*/
#define	ARMPF_SCU_RESERVE0		(ARMPF_SCU_BASE + 0x030)		/* reserved									*/
#define	ARMPF_SCU_NMI_SET		(ARMPF_SCU_BASE + 0x034)		/* NMI�ݒ背�W�X�^							*/
#define	ARMPF_SCU_INTSYS1_EN	(ARMPF_SCU_BASE + 0x038)		/* ���Ӊ�H�����ݓ��͓���/�񓯊��ݒ�1���W�X�^	*/
#define	ARMPF_SCU_INTSYS2_EN	(ARMPF_SCU_BASE + 0x03C)		/* ���Ӊ�H�����ݓ��͓���/�񓯊��ݒ�2���W�X�^	*/
#define	ARMPF_SCU_VICIFSYN_EN	(ARMPF_SCU_BASE + 0x040)		/* CPU-VIC�Ԃ̓���/�񓯊��ݒ背�W�X�^		*/
#define	ARMPF_SCU_PCIE_CLKSEL	(ARMPF_SCU_BASE + 0x044)		/* PCIex�N���b�N�I�����W�X�^				*/
#define	ARMPF_SCU_RESET_STATUS	(ARMPF_SCU_BASE + 0x048)		/* ���Z�b�g�X�e�[�^�X���W�X�^				*/
#define	ARMPF_SCU_UDLCNT		(ARMPF_SCU_BASE + 0x04C)		/* M3-UDLPRGSEL�ݒ背�W�X�^					*/
#define	ARMPF_SCU_RESERVE1		(ARMPF_SCU_BASE + 0x050)		/* reserved									*/
#define	ARMPF_SCU_CPUSS_TRIGINT	(ARMPF_SCU_BASE + 0x054)		/* CTI���W���[������̃N���X�g���K���荞�ݗv�����W�X�^	*/
#define	ARMPF_SCU_GBETHER_CONT	(ARMPF_SCU_BASE + 0x058)		/* GbEther���[�h�ݒ背�W�X�^				*/
#define	ARMPF_SCU_UDLINT_CONT	(ARMPF_SCU_BASE + 0x05C)		/* PCIex�ɑ΂���UDL�����ݑI�����W�X�^		*/
#define	ARMPF_SCU_UDLINT_STATUS	(ARMPF_SCU_BASE + 0x060)		/* UDL�����ݗv�����W�X�^					*/

#define	ARMPF_SCU_APBREG15		(0xEFFF027C)					/* I2C���쐧�䃌�W�X�^(ES3�ȍ~�L��)		<S152>			*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_DVICE_ID																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	JL086ID				0x086A								/* Device ID								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_SOFT_RST																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SOFT_RESET			0									/* �\�t�g���Z�b�g���s						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CPU_HALT 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	HALT_OFF			1									/* HALT����									*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CLK_EN 	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	UDL_NET_CLK			0									/* M3UDL�N���b�N							*/
#define	UDL_SERVO_CLK		1									/* ServoUDL�N���b�N							*/
#define	ADC_CLK				2									/* ADC�N���b�N								*/
#define	GBETHER_CLK			3									/* GbEther�N���b�N							*/
#define	DDR3_CLK			4									/* DDR3�N���b�N								*/
#define	USB_CLK				5									/* USB�N���b�N								*/
#define	BUSOUT_CLK			6									/* BUSCLK_OUT�N���b�N						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CLK_SEL 	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	CLK_MODE1		0										/* ���[�h1									*/
#define	CLK_MODE2		1										/* ���[�h2									*/
#define	CLK_MODE3		2										/* ���[�h3									*/
#define	CLK_MODE4		3										/* ���[�h4									*/
#define	I2C_CKSEL		0										/* I2C�N���b�N���g���؂�ւ�				*/
																/* ���[�h1�F357Kbps							*/
																/* ���[�h2�F384Kbps							*/
																/* ���[�h3�F400Kbps							*/
																/* ���[�h4�F�ݒ�֎~						*/
#define	DDR3_CKSEL		2										/* DDR3�N���b�N���g���؂�ւ�				*/
																/* ���[�h1�F600Mbps							*/
																/* ���[�h2�F800Mbps							*/
																/* ���[�h3�F1200Mbps						*/
																/* ���[�h4�F�ݒ�֎~						*/
#define	OUT_CKSEL		4										/* �O���o�X�N���b�N���g���؂�ւ�			*/
																/* ���[�h1�FBUSCLK(125MHz)					*/
																/* ���[�h2�FBUSCLK/2						*/
																/* ���[�h3�FBUSCLK/4						*/
																/* ���[�h4�FBUSCLK/8						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_MODE		 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	MOVE_MODE		0										/* ���샂�[�h								*/
#define	VECTOR_MODE		4										/* VECTOR���[�h								*/
#define	CPU_CLK_MODE	6										/* CPU�N���b�N���g���I��					*/
#define	OSC_MODE		7										/* OSC�o�b�t�@���[�h						*/
#define	MODE_VNDMD		0										/* ���l�T�X�e�X�g���[�h						*/
#define	MODE_RSV0		1										/* ���U�[�u���[�h							*/
#define	MODE_RSV1		2										/* ���U�[�u���[�h							*/
#define	MODE_TMD3		3										/* �V�~�����[�V�������[�h(TMD3)				*/
#define	MODE_TMD4		4										/* �V�~�����[�V�������[�h(TMD4)				*/
#define	MODE_DBGMD		5										/* UDL_Servo�f�o�b�O���[�h					*/
#define	MODE_MDBGMD		6										/* UDL_M3�f�o�b�O���[�h						*/
#define	MODE_NORMAL		7										/* �m�[�}�����[�h							*/
#define	BOOT_PROM		0										/* �p������ROM�u�[�g(LOWVEC)				*/
#define	BOOT_PCI		1										/* PCIex�u�[�g(HIVEC)						*/
#define	BOOT_SROM		2										/* �V���A��ROM�u�[�g(HIVEC)					*/
#define	BOOT_M3			3										/* M3�u�[�g(HIVEC)							*/
#define	CPUCLK_250		0										/* CPU Clock 250MHz							*/
#define	CPUCLK_500		1										/* CPU Clock 500MHz							*/
#define	OSC_XTOFF		0										/* OSC�o�b�t�@���g�p						*/
#define	OSC_X25MD		1										/* X25OSC�g�p								*/
#define	OSC_X30MD		2										/* X30OSC�g�p								*/
#define	OSC_XTON		3										/* ���U�q���p								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_REMAP	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	REMAP_OFF		0										/* ���}�b�v����								*/
#define	REMAP_ON		1										/* ���}�b�v�L��								*/
#define	DDR_REMAP		0										/* DDR REMAP								*/
#define	AXI_REMAP		1										/* AXI-RAM REMAP(read only)					*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_PCIE_INT, SSPRO7_PCIE_INTMSK																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	PCIE_INTA		0										/* INTA_EP�����ݔ��s/�}�X�N					*/
#define	PCIE_INTB		1										/* INTB_EP�����ݔ��s/�}�X�N					*/
#define	PCIE_INTC		2										/* INTC_EP�����ݔ��s/�}�X�N					*/
#define	PCIE_INTD		3										/* INTD_EP�����ݔ��s/�}�X�N					*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_PCIE_INTSEL 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	PCIE_INTSEL		0										/* PCIex�����ݑI�����W�X�^					*/
#define	PCIE_INTMODE	4										/* �����ݐM���^�C�~���O���䃌�W�X�^			*/
#define	PCIE_TLPEND_INT	0										/* PCIE_TLPEND_INT��UDL�ɐڑ�				*/
#define	PCIEX_GPO_0		1										/* PCIEX_GPO_0��UDL�ɐڑ�					*/
#define	PCIE_INTA_RC	2										/* PCIE_INTA_RC��UDL�ɐڑ�					*/
#define	PCIE_INTB_RC	3										/* PCIE_INTB_RC��UDL�ɐڑ�					*/
#define	PCIE_INTC_RC	4										/* PCIE_INTC_RC��UDL�ɐڑ�					*/
#define	PCIE_INTD_RC	5										/* PCIE_INTD_RC��UDL�ɐڑ�					*/
#define	PCIE_INTMSI_RC	6										/* PCIE_INTMSI_RC��UDL�ɐڑ�				*/
#define	PCIE_MSG_INT_RC	7										/* PCIE_MSG_INT_RC��UDL�ɐڑ�				*/
#define	INT_8CLK_PLS	0										/* ACLK��8�N���b�N���ŏo��					*/
#define	INT_LVL01		1										/* ���x���o��								*/
#define	INT_3CLK_PLS	2										/* ACLK��3�N���b�N���ŏo��					*/
#define	INT_LVL11		3										/* ���x���o��								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_WDT_MODE	 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_5TBUF_EN	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	OV_CNT		0											/* OV�[�q�X�^���o�C���䃌�W�X�^				*/
#define	OC1_CNT		1											/* OC1�[�q�X�^���o�C���䃌�W�X�^			*/
#define	OC2_CNT		2											/* OC2�[�q�X�^���o�C���䃌�W�X�^			*/
#define	SYNCL_CNT	3											/* SUNCL�[�q�X�^���o�C���䃌�W�X�^			*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_NMI_SET	 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_INTSYS1_EN 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_INTSYS2_EN 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_VICIFSYN_EN 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_PCIE_CLKSEL 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_RESET_STATUS																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SW_RST_STS		1										/* �\�t�g�E�F�A���Z�b�g						*/
#define	WDT_RST_STS		2										/* UDL WDT���Z�b�g							*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_UDLCNT	 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CPUSS_TRIGINT																				*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_GBETHER_CONT																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	GB_PHYMODE		0										/* GbEther�ʐM���[�h�ݒ�					*/
#define	GB_SPEED		4										/* �ʐM���x�ݒ�								*/
#define	GB_RMIIMODE		8										/* RMII�]�����̓]�����[�h�ݒ�				*/
#define	GB_CLKSEL		12										/* RGMII�]�����̃N���b�N����ݒ�			*/
#define	GB_PHY_MII		0x0										/* MII���[�h								*/
#define	GB_PHY_RMII		0x1										/* RMII�ʏ탂�[�h							*/
#define	GB_PHY_RMIIRX	0x9										/* RMII���M�N���b�NRX���̓��[�h				*/
#define	GB_PHY_GMII		0xA										/* GMII���[�h								*/
#define	GB_PHY_RGMII	0xB										/* RGMII���[�h								*/
#define	GB_SPD_10M		0										/* �ʐM���x10Mbps							*/
#define	GB_SPD_100M		1										/* �ʐM���x100Mbps							*/
#define	GB_SPD_1G		2										/* �ʐM���x1Gbps							*/



/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_UDLINT_CONT 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	PCI_EP_A_SEL	0										/* PCI EP������A�ɏo�͂��銄���ݑI��		*/
#define	PCI_EP_B_SEL	4										/* PCI EP������B�ɏo�͂��銄���ݑI��		*/
#define	PCI_EP_C_SEL	8										/* PCI EP������C�ɏo�͂��銄���ݑI��		*/
#define	PCI_EP_D_SEL	12										/* PCI EP������D�ɏo�͂��銄���ݑI��		*/
#define	UDL_INTA		0x0										/* UDL_INTA(Servo)���o��					*/
#define	UDL_INTB		0x1										/* UDL_INTB(Servo)���o��					*/
#define	UDL_INTC		0x2										/* UDL_INTC(Servo)���o��					*/
#define	UDL_INTN		0x3										/* UDL_INTN(Servo)���o��					*/
#define	UDL_INT1		0x4										/* UDL_INT1(Servo)���o��					*/
#define	UDL_INT2		0x5										/* UDL_INT2(Servo)���o��					*/
#define	UDL_M3INT0		0x6										/* UDL-M3_INT0(Network)���o�� 				*/
#define	UDL_M3INT1		0x7										/* UDL-M3_INT1(Network)���o�� 				*/
#define	UDL_ALLSERVO	0x8										/* Servo���荞�݂̂��ׂĂ� OR �o�� 			*/
#define	UDL_ALLM3		0x9										/* M3UDL���荞�݂̂��ׂĂ� OR �o�� 			*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_UDLINT_STATUS																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	UDL_INTA_OM		0x01									/* UDL_INTA(Servo)���o��					*/
#define	UDL_INTB_OM		0x02									/* UDL_INTB(Servo)���o��					*/
#define	UDL_INTC_OM		0x04									/* UDL_INTC(Servo)���o��					*/
#define	UDL_INTN_OM		0x08									/* UDL_INTN(Servo)���o��					*/
#define	UDL_INT1_OM		0x10									/* UDL_INT1(Servo)���o��					*/
#define	UDL_INT2_OM		0x20									/* UDL_INT2(Servo)���o��					*/
#define	UDL_M3INT0_OM	0x40									/* UDL-M3_INT0(Network)���o�� 				*/
#define	UDL_M3INT1_OM	0x80									/* UDL-M3_INT1(Network)���o�� 				*/


/*----------------------------------------------------------------------------------------------------------*/
/*		APBREG15	<S152>																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	I2C_LOW			12										/* */
#define	LOW_DISABLE		0										/* Low���o����(ES2�Ɠ���) */
#define	LOW_ENAABLE		1										/* Low���o�L�� */
/*----------------------------------------------------------------------------------------------------------*/
#define	I2C_RATE		8										/* */
#define	RATE_D1			0									/* 00:D1(ES2�Ɠ���) */
#define	RATE_D2			1									/* 01:D2 */
#define	RATE_D3			2									/* 10:D3 */
#define	RATE_D4			3									/* 11:D4(for High speed mode */
/*----------------------------------------------------------------------------------------------------------*/
#define	I2C_CK			0										/* */
#define	CLK_738			0x00									/* 7.38MHz */
#define	CLK_750			0x01									/* 7.50MHz */
#define	CLK_762			0x02									/* 7.62MHz */
#define	CLK_774			0x03									/* 7.74MHz */
#define	CLK_787			0x04									/* 7.87MHz */
#define	CLK_800			0x05									/* 8.00MHz */
#define	CLK_814			0x06									/* 8.14MHz */
#define	CLK_828			0x07									/* 8.28MHz */
#define	CLK_842			0x08									/* 8.42MHz */
#define	CLK_857			0x09									/* 8.57MHz */
#define	CLK_873			0x0A									/* 8.73MHz */
#define	CLK_889			0x0B									/* 8.89MHz */
#define	CLK_906			0x0C									/* 9.06MHz */
#define	CLK_923			0x0D									/* 9.23MHz */
#define	CLK_941			0x0E									/* 9.41MHz */
#define	CLK_960			0x0F									/* 9.60MHz */
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/*		API MACRO																							*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_GET_ID()		(ARMPF_REG_READ( ARMPF_SCU_DEVICE_ID ) & 0xFFFF)			/* JL-086ID�擾				*//* <S119>ES4�ȍ~�Ή� *//* �R�[�h����<S191> */
#define	SCU_GET_VER()		(ARMPF_REG_READ( ARMPF_SCU_DEVICE_ID ) & 0xFFFF) >> 16 )	/* JL-086 ES version�擾	*//* <S191>ES4�ȍ~�Ή� */
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SOFTRESET()		ARMPF_REG_WRITE( ARMPF_SCU_SOFT_RST, SOFT_RESET )	/* JL-086�\�t�g���Z�b�g		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_CPU_HALT()		ARMPF_REG_WRITE( ARMPF_SCU_CPU_HALT, HALT_OFF )		/* PCI�u�[�g��CPUHALT����	*/
/*----------------------------------------------------------------------------------------------------------*/
#if 1
#define	SCU_CLK_START( x )	ARMPF_REG_BITCLR( ARMPF_SCU_CLK_EN, x )				/* ���W���[���N���b�N����	*/
#define	SCU_CLK_STOP( x )	ARMPF_REG_BITSET( ARMPF_SCU_CLK_EN, x )				/* ���W���[���N���b�N��~	*/
#else
#define	SCU_CLK_START( x )					/* ���W���[���N���b�N����	*/
#define	SCU_CLK_STOP( x )					/* ���W���[���N���b�N��~	*/
#endif
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_CLK_SEL( x, mode )	ARMPF_REG_VALSET( ARMPF_SCU_CLK_SEL, (mode << x) )	/* ���W���[���N���b�N�I��	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_GET_MODE( mode )	( ARMPF_REG_READ(ARMPF_SCU_MODE) >> mode )			/* ���샂�[�h�擾			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_REMAP( ram )	ARMPF_REG_WRITE( ARMPF_SCU_REMAP, (REMAP_ON << ram) )	/* ���}�b�v���s			*/
#define	SCU_GET_REMAP( ram )	( ARMPF_REG_READ(ARMPF_SCU_REMAP) >> ram )		/* ���}�b�v��Ԏ擾			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_PCIEINT( x )	ARMPF_REG_BITSET( ARMPF_SCU_PCIE_INT, x )		/* PCIINT���s				*/
#define	SCU_CLR_PCIEINT( x )	ARMPF_REG_BITCLR( ARMPF_SCU_PCIE_INT, x )		/* PCIINT�N���A				*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_MASK_PCIEINT( x )	ARMPF_REG_BITSET( ARMPF_SCU_PCIE_INTMSK, x )	/* PCIINT�}�X�N				*/
#define	SCU_MASKCLR_PCIEINT( x )	ARMPF_REG_BITCLR( ARMPF_SCU_PCIE_INTMSK, x )	/* PCIINT�}�X�N����		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_PCIE_INTSEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_PCIE_INTSEL, (x << PCIE_INTSEL) )	/* PCIex�����ݑI�����W�X�^�Z�b�g	*/
#define	SCU_SET_PCIE_INTMODE( x )	ARMPF_REG_VALSET( ARMPF_SCU_PCIE_INTSEL, (x << PCIE_INTMODE) )	/* �����ݐM���^�C�~���O�Z�b�g	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_ENABLE_WDT_RST()	ARMPF_REG_BITSET( ARMPF_SCU_WDT_MODE, 0 )		/* UDL_WDT���Z�b�g�L��		*/
#define	SCU_DISABLE_WDT_RST()	ARMPF_REG_BITCLR( ARMPF_SCU_WDT_MODE, 0 )		/* UDL_WDT���Z�b�g����		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_5TBUF_EN( x )	ARMPF_REG_BITSET( ARMPF_SCU_5TBUF_EN, x )		/* 5V�g�������g�o�b�t�@�X�^���o�C	*/
#define	SCU_CLR_5TBUF_EN( x )	ARMPF_REG_BITCLR( ARMPF_SCU_5TBUF_EN, x )		/* 5V�g�������g�o�b�t�@����			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_ENABLE_FIQ_MASK()	ARMPF_REG_BITSET( ARMPF_SCU_NMI_SET, 0 )		/* FIQ�}�X�N�L��			*/
#define	SCU_DISABLE_FIQ_MASK()	ARMPF_REG_BITCLR( ARMPF_SCU_NMI_SET, 0 )		/* FIQ�}�X�N����			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_INTSYS1_EN( x )		ARMPF_REG_WRITE( ARMPF_SCU_INTSYS1_EN, x )		/* ���Ӊ�H�����ݓ��͓���/�񓯊��ݒ�1	*/
#define	SCU_INTSYS2_EN( x )		ARMPF_REG_WRITE( ARMPF_SCU_INTSYS2_EN, x )		/* ���Ӊ�H�����ݓ��͓���/�񓯊��ݒ�2	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_VICIFSYN()		ARMPF_REG_BITSET( ARMPF_SCU_VICIFSYN_EN, 0 )	/* CPU-VIC�Ԃ̓����ݒ�		*/
#define	SCU_CLR_VICIFSYN()		ARMPF_REG_BITCLR( ARMPF_SCU_VICIFSYN_EN, 0 )	/* CPU-VIC�Ԃ̔񓯊��ݒ�	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_PCIECLK_IN()		ARMPF_REG_BITSET( ARMPF_SCU_PCIE_CLKSEL, 0 )	/* PCIe�����N���b�N����		*/
#define	SCU_PCIECLK_OUT()		ARMPF_REG_BITCLR( ARMPF_SCU_PCIE_CLKSEL, 0 )	/* PCIe�O�������N���b�N����	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_READ_RST_STATUS()	ARMPF_REG_READ(ARMPF_SCU_RESET_STATUS)			/* ���Z�b�g�X�e�[�^�X���[�h	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_UDLM3_PRGSEL()	ARMPF_REG_WRITE( ARMPF_SCU_UDLCNT, 1 )			/* M3UDL PRGSEL�L��			*/
#define	SCU_CLR_UDLM3_PRGSEL()	ARMPF_REG_WRITE( ARMPF_SCU_UDLCNT, 0 )			/* M3UDL PRGSEL����			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_READ_CPUSS_TRIGINT()	ARMPF_REG_READ(ARMPF_SCU_CPUSS_TRIGINT)		/* �N���X�g���K�����ݗv�����[�h	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_GB_PHYMODE( x )	ARMPF_REG_VALSET( ARMPF_SCU_GBETHER_CONT, (x << GB_PHYMODE) )	/* GbEther�ʐM���[�h�ݒ�		*/
#define	SCU_SET_GB_SPEED( x )	ARMPF_REG_VALSET( ARMPF_SCU_GBETHER_CONT, (x << GB_SPEED) )		/* �ʐM���x�ݒ�					*/
#define	SCU_SET_GB_HALF_DUPLX()	ARMPF_REG_BITSET( ARMPF_SCU_GBETHER_CONT, GB_RMIIMODE )			/* RMII�]��������d�]��			*/
#define	SCU_SET_GB_FULL_DUPLX()	ARMPF_REG_BITCLR( ARMPF_SCU_GBETHER_CONT, GB_RMIIMODE )			/* RMII�]�����S��d�]��			*/
#define	SCU_SET_GB_CLK_REVPHASE()	ARMPF_REG_BITSET( ARMPF_SCU_GBETHER_CONT, GB_CLKSEL )		/* RGMII�]�����N���b�N�t�ʑ�	*/
#define	SCU_SET_GB_CLK_COPHASE()	ARMPF_REG_BITCLR( ARMPF_SCU_GBETHER_CONT, GB_CLKSEL )		/* RGMII�]�����N���b�N���ʑ�	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_PCI_EP_A_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_A_SEL) )	/* PCI EP������A�ɏo�͂��銄���ݐݒ�		*/
#define	SCU_PCI_EP_B_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_B_SEL) )	/* PCI EP������B�ɏo�͂��銄���ݐݒ�		*/
#define	SCU_PCI_EP_C_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_C_SEL) )	/* PCI EP������C�ɏo�͂��銄���ݐݒ�		*/
#define	SCU_PCI_EP_D_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_D_SEL) )	/* PCI EP������D�ɏo�͂��銄���ݐݒ�		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_READ_UDLINT_STATUS()	ARMPF_REG_READ(ARMPF_SCU_UDLINT_STATUS)		/* UDL INT�����ݗv�����[�h	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_APBREG15( a, b, c )	ARMPF_REG_VALSET( ARMPF_SCU_APBREG15, ( (a)<<(I2C_LOW) | (b)<<(I2C_RATE) | (c)<<(I2C_CK) ) )		/* <S152> */

#endif
/*-------end of file----------------------------------------------------------------------------------------*/
