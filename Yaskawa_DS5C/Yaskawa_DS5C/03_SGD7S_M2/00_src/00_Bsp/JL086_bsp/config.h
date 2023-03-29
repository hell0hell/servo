/**
 **************************************************************************************************
 *
 * @file  config.h
 * @brief configuration file for the MICROSSP Evaluation Board soft
 *
 * @attention
 * @note
 *
 * $Rev:: 350                        $
 * $Date:: 2012-03-09 19:48:21 +0900#$
 *
 **************************************************************************************************
 */
/*
 *-------------------------------------------------------------------------------------------------
 *
 * Notice
 *  This is a sample program.
 *  Renesas Electronics assumes no responsibility for any losses incurred.
 *
 * Copyright(C) 2011 Renesas Electronics Corporation
 * Copyright(C) 2011 Renesas Micro Systems Co., Ltd.
 *
 *-------------------------------------------------------------------------------------------------
 */

#ifndef CONFIG_H__
#define CONFIG_H__

/*
 *=================================================================================================
 *
 *  I N C L U D E
 *
 *=================================================================================================
 */
/* none */


/*
 *=================================================================================================
 *
 *  D E F I N E
 *
 *=================================================================================================
 */
#define	MICROSSP_MODEL_NO	"4"
#define	MICROSSP_MODEL4		1

/* Pre-stack base */
#define PRE_STACK_BASE		(0xE8020000)	/* TOP of AHB-RAM */


/*
 *-------------------------------------------------------------------------------------------------
 *
 *  CLOCK Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_CLKFREQ_OSC   (33.3333)							/**< Oscillator Frequency (MHz) */
//#define CONFIG_CLKFREQ_BUS   ((unsigned int)(100000))					/**< BUS CLOCK Frequency (kHz) */
#define CONFIG_CLKFREQ_BUS   ((unsigned int)(62500))					/**< @@ BUS CLOCK Frequency (kHz) */
#define CONFIG_CLKFREQ_TIMER ((unsigned int)(CONFIG_CLKFREQ_BUS/4))	/**< TIMER Base Clock Frequency (kHz) */
#define CONFIG_CLKFREQ_UART  ((unsigned int)(CONFIG_CLKFREQ_BUS/4))	/**< UART Base Clock Frequency (kHz) */
#define CONFIG_CLKFREQ_CSI   ((unsigned int)(CONFIG_CLKFREQ_BUS/2))	/**< CSI Base Clock Frequency (kHz) */
#define CONFIG_CLKFREQ_IIC   ((unsigned int)(CONFIG_CLKFREQ_BUS/16))	/**< IIC Base Clock Frequency (kHz) */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  CPU MMU Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_CPU_MMU_FINE_TABLE_NUMBER   ((unsigned int)(1))				/**< level2 fine table number */
#define CONFIG_CPU_MMU_COARSE_TABLE_NUMBER ((unsigned int)(2))				/**< level2 coarse table number */
#define CONFIG_CPU_MMU_DOMAIN_ACCESS       ((unsigned int)(MMU_D0_MANAGER))	/**< domain access !!! Must match the table settings !!! */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  CPUID/SYSREG/SYSCTL Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_CPUID_REGBASE    ((unsigned int)(0xEFF86000))			/**< CPUID register base address */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  TIMER Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_TIMER_CHANNEL_NUMBER  ((unsigned int)(9))				/**< TIMER Channel number */
#define CONFIG_TIMER_REGBASE         ((unsigned int)(0xEFFF1000))		/**< TIMER register base address */
#define CONFIG_TIMER_REGOFFSET       ((unsigned int)(0x00000010))		/**< TIMER register offset address */
#define CONFIG_SYSTEM_TIMER_CHANNEL  ((unsigned int)(0))				/**< SYSTEM TIMER Channel */
#define CONFIG_SYSTEM_TIMER_INTERVAL ((unsigned int)(1))				/**< SYSTEM TIMER Interval (ms) */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  UART Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_UART_CHANNEL_NUMBER  ((unsigned int)(2))				/**< UART Channel number */
#define CONFIG_UART_REGBASE         ((unsigned int)(0xEFFF4800))		/**< UART register base address */
#define CONFIG_UART_REGOFFSET       ((unsigned int)(0x00000200))		/**< UART register offset address */
#define CONFIG_SYSTEM_UART_CHANNEL  ((unsigned int)(0))				/**< SYSTEM UART Channel */
#define CONFIG_SYSTEM_UART_BAUDRATE ((unsigned int)(115200))			/**< SYSTEM UART Baudrate (bps) */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  CSI Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_CSI_CHANNEL_NUMBER ((unsigned int)(2))					/**< CSI Channel number */
#define CONFIG_CSI_REGBASE        ((unsigned int)(0xEFFF4000))		/**< CSI register base address */
#define CONFIG_CSI_REGOFFSET      ((unsigned int)(0x00000040))		/**< CSI register offset address */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  IIC Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_IIC_CHANNEL_NUMBER ((unsigned int)(1))					/**< IIC Channel number */
#define CONFIG_IIC_REGBASE        ((unsigned int)(0xEFFF5000))		/**< IIC register base address */
#define CONFIG_IIC_REGOFFSET      ((unsigned int)(0x00000040))		/**< IIC register offset address */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  SROM Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_SROM_CHANNEL_NUMBER ((unsigned int)(1))				/**< SROM Channel number */
#define CONFIG_SROM_REGBASE        ((unsigned int)(0xE8020000))		/**< SROM register base address */
#define CONFIG_SROM_REGOFFSET      ((unsigned int)(0x00000400))		/**< SROM register offset address */
#define CONFIG_SROM_MEMBASE        ((unsigned int)(0xF0000000))		/**< SROM memory base address */
#define CONFIG_SROM_MEMOFFSET      ((unsigned int)(0x01000000))		/**< SROM memory offset address */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  INTC Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_INTC_CHANNEL_NUMBER ((unsigned int)(1))				/**< INTC Channel number */
#define CONFIG_INTC_SOURCE_NUMBER  ((unsigned int)(128))				/**< INTC Interrupt Source Number */

/* INTC INDEX NUMBER */
#define	INTC_IDX_AWDT_INT			((unsigned int)0)			///< WDT alarm
#define	INTC_IDX_SYS_IDLE_INT		((unsigned int)1)			///< SYS-SS IDLE released
#define	INTC_IDX_EXT_INT0			((unsigned int)2)			///< External interrput input 0
#define	INTC_IDX_EXT_INT1			((unsigned int)3)			///< External interrput input 1
#define	INTC_IDX_EXT_INT2			((unsigned int)4)			///< External interrput input 2
#define	INTC_IDX_EXT_INT3			((unsigned int)5)			///< External interrput input 3
#define	INTC_IDX_EXT_INT4			((unsigned int)6)			///< External interrput input 4
#define	INTC_IDX_EXT_INT5			((unsigned int)7)			///< External interrput input 5
#define	INTC_IDX_EXT_INT6			((unsigned int)8)			///< External interrput input 6
#define	INTC_IDX_EXT_INT7			((unsigned int)9)			///< External interrput input 7
#define	INTC_IDX_COM_INT0			((unsigned int)10)		///< Other CPU system interrupt 0
#define	INTC_IDX_COM_INT1			((unsigned int)11)		///< Other CPU system interrupt 1
#define	INTC_IDX_COM_INT2			((unsigned int)12)		///< Other CPU system interrupt 2
#define	INTC_IDX_COM_INT3			((unsigned int)13)		///< Other CPU system interrupt 3
#define	INTC_IDX_ATIM_INT0			((unsigned int)14)		///< APB TIMER CMD0 mached
#define	INTC_IDX_ATIM_INT1			((unsigned int)15)		///< APB TIMER CMD1 mached
#define	INTC_IDX_ATIM_INT2			((unsigned int)16)		///< APB TIMER CMD2 mached
#define	INTC_IDX_ATIM_INT3			((unsigned int)17)		///< APB TIMER CMD3 mached
#define	INTC_IDX_ATIM_INT4			((unsigned int)18)		///< APB TIMER CMD4 mached
#define	INTC_IDX_ATIM_INT5			((unsigned int)19)		///< APB TIMER CMD5 mached
#define	INTC_IDX_ATIM_INT6			((unsigned int)20)		///< APB TIMER CMD6 mached
#define	INTC_IDX_ATIM_INT7			((unsigned int)21)		///< APB TIMER CMD7 mached
#define	INTC_IDX_ATIM_INT8			((unsigned int)22)		///< APB TIMER CMD8 mached
#define	INTC_IDX_APWM_INT0			((unsigned int)23)		///< APB PWM0 Interrupt request
#define	INTC_IDX_APWM_INT1			((unsigned int)24)		///< APB PWM1 Interrupt request
#define	INTC_IDX_ARTCP_INT			((unsigned int)25)		///< APB RTC alarm
#define	INTC_IDX_ACSI_INT0			((unsigned int)26)		///< APB CSI0 Interrupt
#define	INTC_IDX_ACSI_INT1			((unsigned int)27)		///< APB CSI1 Interrupt
#define	INTC_IDX_AUART_INT0			((unsigned int)28)		///< APB UART0 Interrupt
#define	INTC_IDX_AUART_INT1			((unsigned int)29)		///< APB UART1 Interrupt
#define	INTC_IDX_AIIC_INT			((unsigned int)30)		///< APB IIC Interrupt
#define	INTC_IDX_BUS_ERR_INT		((unsigned int)31)		///< BUS-SS bus bridge error interrupt(OR)
#define	INTC_IDX_EAHB_BRSL_ERR		((unsigned int)32)		///< EXT_AHB_IF(external AHB slave interface) bridge error interrupt
//#define	INTC_IDX_				((unsigned int)33)		///< unused
#define	INTC_IDX_AHB_DMA_ERR		((unsigned int)34)		///< DMAC-SS(Type-AHB) DMAC error interrupt
#define	INTC_IDX_AHB_DMA0_INT0		((unsigned int)35)		///< DMAC-SS(Type-AHB) DMAC0 channel 0 transfer terminated
#define	INTC_IDX_AHB_DMA0_INT1		((unsigned int)36)		///< DMAC-SS(Type-AHB) DMAC0 channel 1 transfer terminated
#define	INTC_IDX_AHB_DMA0_INT2		((unsigned int)37)		///< DMAC-SS(Type-AHB) DMAC0 channel 2 transfer terminated
#define	INTC_IDX_AHB_DMA0_INT3		((unsigned int)38)		///< DMAC-SS(Type-AHB) DMAC0 channel 3 transfer terminated
#define	INTC_IDX_AHB_DMA1_INT0		((unsigned int)39)		///< DMAC-SS(Type-AHB) DMAC1 channel 0 transfer terminated
#define	INTC_IDX_AHB_DMA1_INT1		((unsigned int)40)		///< DMAC-SS(Type-AHB) DMAC1 channel 1 transfer terminated
#define	INTC_IDX_AHB_DMA1_INT2		((unsigned int)41)		///< DMAC-SS(Type-AHB) DMAC1 channel 2 transfer terminated
#define	INTC_IDX_AHB_DMA1_INT3		((unsigned int)42)		///< DMAC-SS(Type-AHB) DMAC1 channel 3 transfer terminated
#define	INTC_IDX_AHB_DMA2_INT0		((unsigned int)43)		///< DMAC-SS(Type-AHB) DMAC2 channel 0 transfer terminated
#define	INTC_IDX_AHB_DMA2_INT1		((unsigned int)44)		///< DMAC-SS(Type-AHB) DMAC2 channel 1 transfer terminated
#define	INTC_IDX_AHB_DMA2_INT2		((unsigned int)45)		///< DMAC-SS(Type-AHB) DMAC2 channel 2 transfer terminated
#define	INTC_IDX_AHB_DMA2_INT3		((unsigned int)46)		///< DMAC-SS(Type-AHB) DMAC2 channel 3 transfer terminated
#define	INTC_IDX_AHB_DMA3_INT0		((unsigned int)47)		///< DMAC-SS(Type-AHB) DMAC3 channel 0 transfer terminated
#define	INTC_IDX_AHB_DMA3_INT1		((unsigned int)48)		///< DMAC-SS(Type-AHB) DMAC3 channel 1 transfer terminated
#define	INTC_IDX_AHB_DMA3_INT2		((unsigned int)49)		///< DMAC-SS(Type-AHB) DMAC3 channel 2 transfer terminated
#define	INTC_IDX_AHB_DMA3_INT3		((unsigned int)50)		///< DMAC-SS(Type-AHB) DMAC3 channel 3 transfer terminated
#define	INTC_IDX_ETH_CINTMAC		((unsigned int)51)		///< ETHER-SS Ether MAC core interrupt
#define	INTC_IDX_ETH_CINTFS			((unsigned int)52)		///< ETHER-SS FIFO status interrupt
#define	INTC_IDX_ETH_CINTTS			((unsigned int)53)		///< ETHER-SS TX status interrupt
#define	INTC_IDX_ETH_CINTRS			((unsigned int)54)		///< ETHER-SS RX status interrupt
#define	INTC_IDX_ETH_IRQSCTX		((unsigned int)55)		///< ETHER-SS packet send interrupt
#define	INTC_IDX_ETH_IRQSCRX		((unsigned int)56)		///< ETHER-SS packet recive interrupt
#define	INTC_IDX_ETH_SRXREQ			((unsigned int)57)		///< ETHER-SS recive data ready
#define	INTC_IDX_ETH_IRQSCTX_TCH	((unsigned int)58)		///< ETHER-SS send data check-sum cluclate complite
#define	INTC_IDX_ETH_IRQSCRX_TCH	((unsigned int)59)		///< ETHER-SS send data check-sum write complite
#define	INTC_IDX_A1176_PMUIRQ		((unsigned int)60)		///< ARM1176 system performance minitor interrupt
#define	INTC_IDX_COMMRX_INT			((unsigned int)61)		///< DEBUG recive buffer empty
#define	INTC_IDX_COMMTX_INT			((unsigned int)62)		///< DEBUG send buffer empty
#define	INTC_IDX_U2H_INT			((unsigned int)63)		///< USB-SS(Type-H2) Host Corntroller interrupt(Bridge)
#define	INTC_IDX_U2H_OHCI_INT		((unsigned int)64)		///< USB-SS(Type-H2) Host Corntroller interrupt(OHCI)
#define	INTC_IDX_U2H_EHCI_INT		((unsigned int)65)		///< USB-SS(Type-H2) Host Corntroller interrupt(EHCI)
#define	INTC_IDX_U2H_PWE_INT		((unsigned int)66)		///< USB-SS(Type-H2) Host Corntroller interrupt(PWE)
#define	INTC_IDX_U2H_BIND_INT		((unsigned int)67)		///< USB-SS(Type-H2) Host Corntroller interruptOR(Bridge+OHCI+EHCI+PWE)
#define	INTC_IDX_U2F_INT			((unsigned int)68)		///< USB-SS(Type-F2) AHB-EPC Bridge interrupt signal
#define	INTC_IDX_U2F_EPC_INT		((unsigned int)69)		///< USB-SS(Type-F2) EPC interrupt signal
#define	INTC_IDX_SATA_HINTIPF		((unsigned int)70)		///< SATA-SS Device Interrupt
#define	INTC_IDX_SATA_HINTSYS		((unsigned int)71)		///< SATA-SS System Interrupt
#define	INTC_IDX_SATA_HINTPLUG		((unsigned int)72)		///< SATA-SS Host plug Interrupt
#define	INTC_IDX_SATA_HINTDMA		((unsigned int)73)		///< SATA-SS DMAC interrupt request
#define	INTC_IDX_SATA_HINTALL		((unsigned int)74)		///< SATA-SS interrupt(OR)
#define	INTC_IDX_PCI1_INTA_RC		((unsigned int)75)		///< PCI Express-SS(*1) INTA Interrupt
#define	INTC_IDX_PCI1_INTB_RC		((unsigned int)76)		///< PCI Express-SS(*1) INTB Interrupt
#define	INTC_IDX_PCI1_INTC_RC		((unsigned int)77)		///< PCI Express-SS(*1) INTC Interrupt
#define	INTC_IDX_PCI1_INTD_RC		((unsigned int)78)		///< PCI Express-SS(*1) INTD Interrupt
#define	INTC_IDX_PCI1_INTMSI_RC		((unsigned int)79)		///< PCI Express-SS(*1) MSI interrupt
#define	INTC_IDX_PCI1_MSG_INT		((unsigned int)80)		///< PCI Express-SS(*1) Message interrupt
#define	INTC_IDX_PCI1_PCIE_ERR		((unsigned int)81)		///< PCI Express-SS(*1) PCI Error interrupt
#define	INTC_IDX_PCI1_AXI_ERR		((unsigned int)82)		///< PCI Express-SS(*1) AXI Error interrupt
#define	INTC_IDX_PCI1_INTALL		((unsigned int)83)		///< PCI Express-SS(*1) interrupt(OR)
#define	INTC_IDX_PCI4_INTA_RC		((unsigned int)84)		///< PCI Express-SS(*4) INTA interrupt
#define	INTC_IDX_PCI4_INTB_RC		((unsigned int)85)		///< PCI Express-SS(*4) INTB interrupt
#define	INTC_IDX_PCI4_INTC_RC		((unsigned int)86)		///< PCI Express-SS(*4) INTC interrupt
#define	INTC_IDX_PCI4_INTD_RC		((unsigned int)87)		///< PCI Express-SS(*4) INTD interrupt
#define	INTC_IDX_PCI4_INTMSI_RC		((unsigned int)88)		///< PCI Express-SS(*4) MSI interrupt
#define	INTC_IDX_PCI4_MSG_INT		((unsigned int)89)		///< PCI Express-SS(*4) Message interrupt
#define	INTC_IDX_PCI4_PCIE_ERR		((unsigned int)90)		///< PCI Express-SS(*4) PCI Error interrupt
#define	INTC_IDX_PCI4_AXI_ERR		((unsigned int)91)		///< PCI Express-SS(*4) AXI Error interrupt
#define	INTC_IDX_PCI4_INTALL		((unsigned int)92)		///< PCI Express-SS(*4) interrupt(OR)
#define	INTC_IDX_SFMEMC_INT			((unsigned int)93)		///< SFMEMC interrupt
#define	INTC_IDX_EXT_INT8			((unsigned int)94)		///< External interrput input 8
#define	INTC_IDX_EXT_INT9			((unsigned int)95)		///< External interrput input 9
#define	INTC_IDX_EXT_INT10			((unsigned int)96)		///< External interrput input 10
#define	INTC_IDX_EXT_INT11			((unsigned int)97)		///< External interrput input 11
#define	INTC_IDX_EXT_INT12			((unsigned int)98)		///< External interrput input 12
#define	INTC_IDX_EXT_INT13			((unsigned int)99)		///< External interrput input 13
#define	INTC_IDX_EXT_INT14			((unsigned int)100)		///< External interrput input 14
#define	INTC_IDX_EXT_INT15			((unsigned int)101)		///< External interrput input 15
#define	INTC_IDX_EXT_INT16			((unsigned int)102)		///< External interrput input 16
#define	INTC_IDX_EXT_INT17			((unsigned int)103)		///< External interrput input 17
#define	INTC_IDX_EXT_INT18			((unsigned int)104)		///< External interrput input 18
#define	INTC_IDX_EXT_INT19			((unsigned int)105)		///< External interrput input 19
#define	INTC_IDX_EXT_INT20			((unsigned int)106)		///< External interrput input 20
#define	INTC_IDX_EXT_INT21			((unsigned int)107)		///< External interrput input 21
#define	INTC_IDX_EXT_INT22			((unsigned int)108)		///< External interrput input 22
#define	INTC_IDX_EXT_INT23			((unsigned int)109)		///< External interrput input 23
#define	INTC_IDX_EXT_INT24			((unsigned int)110)		///< External interrput input 24
#define	INTC_IDX_EXT_INT25			((unsigned int)111)		///< External interrput input 25
#define	INTC_IDX_EXT_INT26			((unsigned int)112)		///< External interrput input 26
#define	INTC_IDX_EXT_INT27			((unsigned int)113)		///< External interrput input 27
#define	INTC_IDX_EXT_INT28			((unsigned int)114)		///< External interrput input 28
#define	INTC_IDX_EXT_INT29			((unsigned int)115)		///< External interrput input 29
#define	INTC_IDX_EXT_INT30			((unsigned int)116)		///< External interrput input 30
#define	INTC_IDX_EXT_INT31			((unsigned int)117)		///< External interrput input 31
#define	INTC_IDX_AXI_DMA_ERR		((unsigned int)118)		///< DMAC-SS(Type-AXI) DMAC ERROR interrput
#define	INTC_IDX_AXI_DMA0_INT0		((unsigned int)119)		///< DMAC-SS(Type-AXI) DMAC0 channel 0 transfer terminated
#define	INTC_IDX_AXI_DMA0_INT1		((unsigned int)120)		///< DMAC-SS(Type-AXI) DMAC0 channel 1 transfer terminated
#define	INTC_IDX_AXI_DMA0_INT2		((unsigned int)121)		///< DMAC-SS(Type-AXI) DMAC0 channel 2 transfer terminated
#define	INTC_IDX_AXI_DMA0_INT3		((unsigned int)122)		///< DMAC-SS(Type-AXI) DMAC0 channel 3 transfer terminated
//#define	INTC_IDX_				((unsigned int)123)		///< unused
//#define	INTC_IDX_				((unsigned int)124)		///< unused
//#define	INTC_IDX_				((unsigned int)125)		///< unused
//#define	INTC_IDX_				((unsigned int)126)		///< unused
//#define	INTC_IDX_				((unsigned int)127)		///< unused

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  MEMC Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_MEMC_STCS0_TYPE                  ((unsigned int)(1))			/**< SYSTEM MEMC STCS0 Type (1=Page ROM, 0=SRAM) */
#define CONFIG_MEMC_STCS0_PAGE_SIZE             ((unsigned int)(16))			/**< SYSTEM MEMC STCS0 Page Size */
#define CONFIG_MEMC_STCS0_ONPAGE_WAIT           ((unsigned int)(4))			/**< SYSTEM MEMC STCS0 On Page Data Wait */
#define CONFIG_MEMC_STCS0_WIDTH                 ((unsigned int)(16))			/**< SYSTEM MEMC STCS0 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_STCS0_ADDRESS_SETUP         ((unsigned int)(1))			/**< SYSTEM MEMC STCS0 Address Setup Wait Cycle */
#define CONFIG_MEMC_STCS0_DATA_WAIT             ((unsigned int)(12))			/**< SYSTEM MEMC STCS0 Data Wait Cycle */
#define CONFIG_MEMC_STCS0_WRITE_RECOVERY        ((unsigned int)(4))			/**< SYSTEM MEMC STCS0 Write Recovery Wait Cycle */
#define CONFIG_MEMC_STCS0_IDLE_WAIT             ((unsigned int)(2))			/**< SYSTEM MEMC STCS0 Idle Wait Cycle */
#define CONFIG_MEMC_STCS1_WIDTH                 ((unsigned int)(16))			/**< SYSTEM MEMC STCS1 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_STCS1_ADDRESS_SETUP         ((unsigned int)(3))			/**< SYSTEM MEMC STCS1 Address Setup Wait Cycle */
#define CONFIG_MEMC_STCS1_DATA_WAIT             ((unsigned int)(6))			/**< SYSTEM MEMC STCS1 Data Wait Cycle */
#define CONFIG_MEMC_STCS1_WRITE_RECOVERY        ((unsigned int)(2))			/**< SYSTEM MEMC STCS1 Write Recovery Wait Cycle */
#define CONFIG_MEMC_STCS1_IDLE_WAIT             ((unsigned int)(2))			/**< SYSTEM MEMC STCS1 Idle Wait Cycle */
//tanaka21#define CONFIG_MEMC_STCS2_WIDTH                 ((unsigned int)(16))			/**< SYSTEM MEMC STCS2 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_STCS2_WIDTH                 ((unsigned int)(8))			/**< SYSTEM MEMC STCS2 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_STCS2_ADDRESS_SETUP         ((unsigned int)(1))			/**< SYSTEM MEMC STCS2 Address Setup Wait Cycle */
#define CONFIG_MEMC_STCS2_DATA_WAIT             ((unsigned int)(1))			/**< SYSTEM MEMC STCS2 Data Wait Cycle */
#define CONFIG_MEMC_STCS2_WRITE_RECOVERY        ((unsigned int)(1))			/**< SYSTEM MEMC STCS2 Write Recovery Wait Cycle */
#define CONFIG_MEMC_STCS2_IDLE_WAIT             ((unsigned int)(1))			/**< SYSTEM MEMC STCS2 Idle Wait Cycle */
#define CONFIG_MEMC_STCS3_WIDTH                 ((unsigned int)(32))			/**< SYSTEM MEMC STCS3 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_STCS3_ADDRESS_SETUP         ((unsigned int)(1))			/**< SYSTEM MEMC STCS3 Address Setup Wait Cycle */
#define CONFIG_MEMC_STCS3_DATA_WAIT             ((unsigned int)(2))			/**< SYSTEM MEMC STCS3 Data Wait Cycle */
#define CONFIG_MEMC_STCS3_WRITE_RECOVERY        ((unsigned int)(1))			/**< SYSTEM MEMC STCS3 Write Recovery Wait Cycle */
#define CONFIG_MEMC_STCS3_IDLE_WAIT             ((unsigned int)(1))			/**< SYSTEM MEMC STCS3 Idle Wait Cycle */
#define CONFIG_MEMC_DYCS0_WIDTH                 ((unsigned int)(32))			/**< SYSTEM MEMC DYCS0 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_DYCS0_CAS_LATENCY           ((unsigned int)(2))			/**< SYSTEM MEMC DYCS0 CAS Latency */
#define CONFIG_MEMC_DYCS0_BANK_ACTIVE_INTERVAL  ((unsigned int)(2))			/**< SYSTEM MEMC DYCS0 Bank Active Interval Cycle */
#define CONFIG_MEMC_DYCS0_ROW_WIDTH             ((unsigned int)(13))			/**< SYSTEM MEMC DYCS0 Row Address Width */
#define CONFIG_MEMC_DYCS0_COL_WIDTH             ((unsigned int)(9))			/**< SYSTEM MEMC DYCS0 Column Address Width */
#define CONFIG_MEMC_DYCS0_MODE_REFRESH_INTERVAL ((unsigned int)(8))			/**< SYSTEM MEMC DYCS0 Refresh Interval on mode setting */
#define CONFIG_MEMC_DYCS0_REFRESH_INTERVAL      ((unsigned int)(7812))		/**< SYSTEM MEMC DYCS0 Refresh Interval (7812[7812.5ns=64ms/8192]) */
#define CONFIG_MEMC_DYCS1_WIDTH                 ((unsigned int)(16))			/**< SYSTEM MEMC DYCS1 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_DYCS1_CAS_LATENCY           ((unsigned int)(2))			/**< SYSTEM MEMC DYCS1 CAS Latency */
#define CONFIG_MEMC_DYCS1_BANK_ACTIVE_INTERVAL  ((unsigned int)(2))			/**< SYSTEM MEMC DYCS1 Bank Active Interval Cycle */
#define CONFIG_MEMC_DYCS1_ROW_WIDTH             ((unsigned int)(13))			/**< SYSTEM MEMC DYCS1 Row Address Width */
#define CONFIG_MEMC_DYCS1_COL_WIDTH             ((unsigned int)(9))			/**< SYSTEM MEMC DYCS1 Column Address Width */
#define CONFIG_MEMC_DYCS1_MODE_REFRESH_INTERVAL ((unsigned int)(8))			/**< SYSTEM MEMC DYCS1 Refresh Interval on mode setting */
#define CONFIG_MEMC_DYCS1_REFRESH_INTERVAL      ((unsigned int)(7812))		/**< SYSTEM MEMC DYCS1 Refresh Interval (7812[7812.5ns=64ms/8192]) */
#define CONFIG_MEMC_DYCS2_WIDTH                 ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_DYCS2_CAS_LATENCY           ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 CAS Latency */
#define CONFIG_MEMC_DYCS2_BANK_ACTIVE_INTERVAL  ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 Bank Active Interval Cycle */
#define CONFIG_MEMC_DYCS2_ROW_WIDTH             ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 Row Address Width */
#define CONFIG_MEMC_DYCS2_COL_WIDTH             ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 Column Address Width */
#define CONFIG_MEMC_DYCS2_MODE_REFRESH_INTERVAL ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 Refresh Interval on mode setting */
#define CONFIG_MEMC_DYCS2_REFRESH_INTERVAL      ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS2 Refresh Interval */
#define CONFIG_MEMC_DYCS3_WIDTH                 ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 Data Bus Width (16, 32, -1=not use) */
#define CONFIG_MEMC_DYCS3_CAS_LATENCY           ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 CAS Latency */
#define CONFIG_MEMC_DYCS3_BANK_ACTIVE_INTERVAL  ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 Bank Active Interval Cycle */
#define CONFIG_MEMC_DYCS3_ROW_WIDTH             ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 Row Address Width */
#define CONFIG_MEMC_DYCS3_COL_WIDTH             ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 Column Address Width */
#define CONFIG_MEMC_DYCS3_MODE_REFRESH_INTERVAL ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 Refresh Interval on mode setting */
#define CONFIG_MEMC_DYCS3_REFRESH_INTERVAL      ((unsigned int)(-1))			/**< SYSTEM MEMC DYCS3 Refresh Interval */
#define CONFIG_MEMC_REGBASE                     ((unsigned int)(0xEFF30000))	/**< SYSTEM MEMC register base address */
#define CONFIG_MEMC_REGOFFSET                   ((unsigned int)(0x00000400))	/**< SYSTEM MEMC register offset address */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  DDR3 Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_DDR3_SIZE        ((unsigned int)(256))					/**< SYSTEM DDR3 Size (MB) */
#define CONFIG_DDR3_PORT_NUMBER ((unsigned int)(1))					/**< SYSTEM DDR3 Port Number */
#define CONFIG_DDR3_BANK_NUMBER ((unsigned int)(8))					/**< SYSTEM DDR3 Bank Number */
#define CONFIG_DDR3_ROW_WIDTH   ((unsigned int)(13))					/**< SYSTEM DDR3 Row Address Width */
#define CONFIG_DDR3_COL_WIDTH   ((unsigned int)(10))					/**< SYSTEM DDR3 Column Address Width */
#define CONFIG_DDR3_tCK         ((unsigned int)(   3750+0))			/**< SYSTEM DDR3 tCK   (ps) (min    3.75 ns) */
#define CONFIG_DDR3_tRTP        ((unsigned int)(   7500+0))			/**< SYSTEM DDR3 tRTP  (ps) (min    7.50 ns) */
#define CONFIG_DDR3_tCL         ((unsigned int)(  12000+0))			/**< SYSTEM DDR3 tCL   (ps) (min   12.00 ns) */
#define CONFIG_DDR3_tRAS        ((unsigned int)(  40000+0))			/**< SYSTEM DDR3 tRAS  (ps) (min   40.00 ns) */
#define CONFIG_DDR3_tWTR        ((unsigned int)(   7500+0))			/**< SYSTEM DDR3 tWTR  (ps) (min    7.50 ns) */
#define CONFIG_DDR3_tRFC        ((unsigned int)( 127500+0))			/**< SYSTEM DDR3 tRFC  (ps) (min  127.50 ns) */
#define CONFIG_DDR3_tWR         ((unsigned int)(  15000+0))			/**< SYSTEM DDR3 tWR   (ps) (min   15.00 ns) */
#define CONFIG_DDR3_tRP         ((unsigned int)(  12000+0))			/**< SYSTEM DDR3 tRP   (ps) (min   12.00 ns) */
#define CONFIG_DDR3_tRCD        ((unsigned int)(  12000+0))			/**< SYSTEM DDR3 tRCD  (ps) (min   12.00 ns) */
#define CONFIG_DDR3_tREFI       ((unsigned int)(7800000-1))			/**< SYSTEM DDR3 tREFI (ps) (max 7800.00 ns) */
#define CONFIG_DDR3_REGBASE     ((unsigned int)(0xEFF80000))			/**< SYSTEM DDR3 register base address */
#define CONFIG_DDR3_REGOFFSET   ((unsigned int)(0x00010000))			/**< SYSTEM DDR3 register offset address */

/*
 *-------------------------------------------------------------------------------------------------
 *
 *  Other Configuration
 *
 *-------------------------------------------------------------------------------------------------
 */
#define CONFIG_AXIRAM_MEMBASE     ((unsigned int)(0x68000000))		/**< AXIRAM memory base address */
#define CONFIG_AXIRAM_MEMOFFSET   ((unsigned int)(0x00020000))		/**< AXIRAM memory offset address */
#define CONFIG_SATA_REGBASE       ((unsigned int)(0x6EFC0000))		/**< SATA register base address */
#define CONFIG_SATA_REGOFFSET     ((unsigned int)(0x00010000))		/**< SATA register offset address */
#define CONFIG_PCIE1_REGBASE      ((unsigned int)(0x6FED0000))		/**< PCIe x1 register base address */
#define CONFIG_PCIE1_REGOFFSET    ((unsigned int)(0x00001000))		/**< PCIe x1 register offset address */
#define CONFIG_PCIE4_REGBASE      ((unsigned int)(0x6FED2000))		/**< PCIe x4 register base address */
#define CONFIG_PCIE4_REGOFFSET    ((unsigned int)(0x00001000))		/**< PCIe x4 register offset address */
#define CONFIG_AXIDMA_REGBASE     ((unsigned int)(0x6FEE0000))		/**< AXIDMA register base address */
#define CONFIG_AXIDMA_REGOFFSET   ((unsigned int)(0x00010000))		/**< AXIDMA register offset address */
#define CONFIG_PCIE1_WINBASE      ((unsigned int)(0x70000000))		/**< PCIe x1 window base address */
#define CONFIG_PCIE1_WINOFFSET    ((unsigned int)(0x10000000))		/**< PCIe x1 window offset address */
#define CONFIG_PCIE4_WINBASE      ((unsigned int)(0x80000000))		/**< PCIe x4 window base address */
#define CONFIG_PCIE4_WINOFFSET    ((unsigned int)(0x40000000))		/**< PCIe x4 window offset address */
#define CONFIG_AHBRAM_MEMBASE     ((unsigned int)(0xE8000000))		/**< AHBRAM memory base address */
#define CONFIG_AHBRAM_MEMOFFSET   ((unsigned int)(0x00020000))		/**< AHBRAM memory offset address */
#define CONFIG_USBF_REGBASE       ((unsigned int)(0xEF040000))		/**< USB function register base address */
#define CONFIG_USBF_REGOFFSET     ((unsigned int)(0x00010000))		/**< USB function register offset address */
#define CONFIG_USBH_REGBASE       ((unsigned int)(0xEF020000))		/**< USB host register base address */
#define CONFIG_USBH_REGOFFSET     ((unsigned int)(0x00020000))		/**< USB host register offset address */
#define CONFIG_ETHER_REGBASE      ((unsigned int)(0xEFBE0000))		/**< ETHER mac register base address */
#define CONFIG_ETHER_REGOFFSET    ((unsigned int)(0x00010000))		/**< ETHER mac register offset address */
#define CONFIG_ETHERSUM_REGBASE   ((unsigned int)(0xEFBF0000))		/**< ETHER checksum register base address */
#define CONFIG_ETHERSUM_REGOFFSET ((unsigned int)(0x00010000))		/**< ETHER checksum register offset address */
#define CONFIG_AHBDMA_REGBASE     ((unsigned int)(0xEFF50000))		/**< AHBDMA register base address */
#define CONFIG_AHBDMA_REGOFFSET   ((unsigned int)(0x00010000))		/**< AHBDMA register offset address */
#define CONFIG_PWM_REGBASE        ((unsigned int)(0xEFFF1200))		/**< PWM register base address */
#define CONFIG_PWM_REGOFFSET      ((unsigned int)(0x00000040))		/**< PWM register offset address */
#define CONFIG_WDT_REGBASE        ((unsigned int)(0xEFFF1600))		/**< WDT register base address */
#define CONFIG_WDT_REGOFFSET      ((unsigned int)(0x00000010))		/**< WDT register offset address */
#define CONFIG_RTC_REGBASE        ((unsigned int)(0xEFFF1700))		/**< RTC register base address */
#define CONFIG_RTC_REGOFFSET      ((unsigned int)(0x00000010))		/**< RTC register offset address */


/*
 *=================================================================================================
 *
 *  V A R I A B L E
 *
 *=================================================================================================
 */
/* none */


/*
 *=================================================================================================
 *
 *  P R O T O T Y P E
 *
 *=================================================================================================
 */
/* none */


#endif /* CONFIG_H__ */
