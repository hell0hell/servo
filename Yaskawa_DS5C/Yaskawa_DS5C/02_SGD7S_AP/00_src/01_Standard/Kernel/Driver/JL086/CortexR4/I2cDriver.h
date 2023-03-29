/****************************************************************************************************/
/*																									*/
/*																									*/
/*		I2cDriver.h : I2C Interface Driver Definition File											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : I2C Common Definitions.																	*/
/*																									*/
/*			1. 																						*/
/*			2. 																						*/
/*			3. 																						*/
/*			4. 																						*/
/*			5. 																						*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.07.25  K.Ozaki		##Cortex-R4##											*/
/*		Rev.2.00 : 2013.12.17  Y.Oka		<S097>													*/
/*																									*/
/****************************************************************************************************/
#ifndef			___I2C_DRIVER___
#define			___I2C_DRIVER___

#include	"Basedef.h"
#include	"config.h"								/* Config Header File							*/
#include	"ARMPF_VIC.h"							/* @@  */

#ifdef _MERCURY_PLATFORM_	/*<S146>*/
#ifndef SVFSEL_EEPROM_USE
#define SVFSEL_EEPROM_USE 	(1)			/* 1: EEPROM使用 0: 未使用			*/
#endif
#endif

/****************************************************************************************************/
/*																									*/
/*		EEPROM Interface Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define CPU_I2C_N_CH 		(CONFIG_IIC_CHANNEL_NUMBER)	/* Channel Number */
/*--------------------------------------------------------------------------------------------------*/
#define CPU_I2C_REGBASE   	(CONFIG_IIC_REGBASE)		/* I2C Register BaseAddr */
#define CPU_I2C_REGOFFSET 	(CONFIG_IIC_REGOFFSET)		/* I2C Register BaseOffset */
/*--------------------------------------------------------------------------------------------------*/
//#define	EEPSIZE_TOTAL		0x0800						/* EEPROM Word Size : Total 2K word/4KB		*//* <S0E4> */
#define	EEPSIZE_TOTAL		0x1000						/* EEPROM Word Size : Total 2K word/4KB		*//* <S0E4> */
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_WRITE_BIT	(0x00)						/* I2C address lower bit for Write			*/
#define KPX_I2C_READ_BIT	(0x01)						/* I2C address lower bit for Read			*/
/*--------------------------------------------------------------------------------------------------*/
#define I2C_DRV_TIMEOUT		(20)						/* I2C Driver Operation Timeout 20[ms]		*/
/*--------------------------------------------------------------------------------------------------*/
#define I2C_DRV_SUCCESS		(0)						/* I2C Driver Timeout Error					*/
#define I2C_TIMEOUT_ERR		(-1)						/* I2C Driver Timeout Error					*/



/****************************************************************************************************/
/*																									*/
/*		I2C Register Mapping 																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _IIC_REG_
{
	VUINT8 IICACT;								/**< IIC ACTive enable register */
	VUINT8 Rsrvd_01H[3];						/**< (reserved) */
	VUINT8 IIC;									/**< IIC shift register */
	VUINT8 Rsrvd_05H[3];						/**< (reserved) */
	VUINT8 IICC;								/**< IIC Control register */
	VUINT8 Rsrvd_09H[3];						/**< (reserved) */
	VUINT8 SVA;									/**< SlaVe Address register */
	VUINT8 Rsrvd_0DH[3];						/**< (reserved) */
	VUINT8 IICCL;								/**< IIC CLock select register */
	VUINT8 Rsrvd_11H[3];						/**< (reserved) */
	VUINT8 IICX;								/**< IIC eXtention register */
	VUINT8 Rsrvd_15H[3];						/**< (reserved) */
	VUINT8 IICS;								/**< IIC Status register */
	VUINT8 Rsrvd_19H[3];						/**< (reserved) */
	VUINT8 IICSE;								/**< IIC Status register for Emulation */
	VUINT8 Rsrvd_1DH[3];						/**< (reserved) */
	VUINT8 IICF;								/**< IIC Flag register */
	VUINT8 Rsrvd_21H[3];						/**< (reserved) */
	VUINT8 Rsrvd_24H[4*6];						/**< (reserved) */
} IIC_REG;

/****************************************************************************************************/
/*																									*/
/*		Macro Definition 	<S00D>																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/** 	IICACT Bit Mapping Definition 																*/
/*--------------------------------------------------------------------------------------------------*/
#define IICE ((UINT8)1<<0)					/**< IIC Enable bit */

/*--------------------------------------------------------------------------------------------------*/
/** 	IICC Bit Mapping Definition 																*/
/*--------------------------------------------------------------------------------------------------*/
#define SPT    ((UINT8)1<<0)				/**< StoP condition Trigger control bit */
#define STT    ((UINT8)1<<1)				/**< StarT condition Trigger control bit */
#define ACKE   ((UINT8)1<<2)				/**< ACK Enable control bit */
#define WTIM   ((UINT8)1<<3)				/**< Wait TIMing in interrupt detect bit */
#define SPIE   ((UINT8)1<<4)				/**< StoP condition Interrupt Enable bit */
#define WREL   ((UINT8)1<<5)				/**< Wait RELease control bit */
#define LREL   ((UINT8)1<<6)				/**< Line RELease control bit */

/*--------------------------------------------------------------------------------------------------*/
/** 	IICCL Bit Mapping Definition 																*/
/*--------------------------------------------------------------------------------------------------*/
#define CL0   ((UINT8)1<<0)					/**< CLock select control bit */
#define DFC   ((UINT8)1<<2)					/**< Digital Noise Filter bit */
#define SMC   ((UINT8)1<<3)					/**< Speed Mode Control bit */
#define DAD   ((UINT8)1<<4)					/**< Data/Address level Detect flag bit */
#define CLD   ((UINT8)1<<5)					/**< CLock level Detect flag bit */

/*--------------------------------------------------------------------------------------------------*/
/** 	IICX Bit Mapping Definition 																*/
/*--------------------------------------------------------------------------------------------------*/
#define CLX    ((UINT8)1<<0)				/**< CLock speed eXtention control bit */

/*--------------------------------------------------------------------------------------------------*/
/** 	IICS Bit Mapping Definition 																*/
/*--------------------------------------------------------------------------------------------------*/
#define SPD    ((UINT8)1<<0)				/**< StoP condition Detect flag bit */
#define STD    ((UINT8)1<<1)				/**< StarT condition Detect flag bit */
#define ACKD   ((UINT8)1<<2)				/**< ACK Detect flag bit */
#define TRC    ((UINT8)1<<3)				/**< Transmit/Recieve Condition flag bit */
#define COI    ((UINT8)1<<4)				/**< address match flag bit */
#define EXC    ((UINT8)1<<5)				/**< EXtention Code detect flag bit */
#define ALD    ((UINT8)1<<6)				/**< Arbitration Status bit */
#define MSTS   ((UINT8)1<<7)				/**< MaSTer Status bit */

/*--------------------------------------------------------------------------------------------------*/
/** 	IICF Bit Mapping Definition 																*/
/*--------------------------------------------------------------------------------------------------*/
#define IICRSV ((UINT8)1<<0)				/**< IIC Reserve transfer disable bit */
#define STCEN  ((UINT8)1<<1)				/**< initial Start Condition enable bit */
#define IICBSY ((UINT8)1<<6)				/**< IIC Busy flag bit */
#define STCF   ((UINT8)1<<7)				/**< Start Condition Clear Flag bit */


/*--------------------------------------------------------------------------------------------------*/
/*		Register Base Address Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define I2C_CH0_REGBASE ((struct _IIC_REG_ *)( CPU_I2C_REGBASE + (CPU_I2C_REGOFFSET * 0)))

/*--------------------------------------------------------------------------------------------------*/
/*		Register Access Macro Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define I2C_REG_WR(reg, data) (reg) = (data)
#define I2C_REG_RD(reg, data) (data) = (reg)

/*--------------------------------------------------------------------------------------------------*/
/** 	IIC Interrupt Register Definition 															*/
/*--------------------------------------------------------------------------------------------------*/
//#define INTIIC_IRQS0		(VUINT *)(ARMPF_VIC_IRQS0)				/* IRQ Status Register0 		*/
#define INTIIC_RAIS0		(VUINT *)(ARMPF_VIC_RAIS0)				/* IRQ Status Register0 		*/
#define INTIIC_PIC0			(VUINT *)(ARMPF_VIC_PIC0)				/* IRQ Edge Clear Register0		*/
#define IIC_IRQ				(29)									/* IRQ Number of I2C Interrupt	*/
//#define Kpx_ChkI2cInterrupt( ) ( (*INTIIC_IRQS0) & (1<<IIC_IRQ) )	/* Check Interrupt Status		*/
#define Kpx_ChkI2cInterrupt( ) ( (*INTIIC_RAIS0) & (1<<IIC_IRQ) )	/* Check Interrupt Status		*/
#define Kpx_ClrI2cInterrupt( ) ( (*INTIIC_PIC0) = (1<<IIC_IRQ) )	/* clear interrupt status 		*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM Access Driver definitions															*/
/*--------------------------------------------------------------------------------------------------*/
#define CFG_EEPROM_EEPADR_OUTH    1
/*--------------------------------------------------------------------------------------------------*/
#define CFG_EEPROM_PORTADR   (VUINT *)(0xEFFF8010)						/* 実際は、GPIO4 			*/
//-#define CFG_EEPROM_WRITE_ENABLE( ) { *CFG_EEPROM_PORTADR &= ~(1<<0); }	/* EEPROM Write Enable		*/
//-#define CFG_EEPROM_WRITE_DISABLE( ) { *CFG_EEPROM_PORTADR |= (1<<0); }	/* EEPROM Write Disable */
/* EEPROMの２軸対応 */ /* <S1AC> */
#define CFG_EEPROM1_WRITE_ENABLE() ( *CFG_EEPROM_PORTADR &= ~(1<<0) )		/* EEPROM1 Write Enable	*/
#define CFG_EEPROM1_WRITE_DISABLE() ( *CFG_EEPROM_PORTADR |= (1<<0) )		/* EEPROM1 Write Disable*/
#define CFG_EEPROM2_WRITE_ENABLE() ( *CFG_EEPROM_PORTADR &= ~(1<<1) )		/* EEPROM2 Write Enable	*/
#define CFG_EEPROM2_WRITE_DISABLE() ( *CFG_EEPROM_PORTADR |= (1<<1) )		/* EEPROM2 Write Disable*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2CEEP1_DEV_ADR_WR	(0xA0)						/* I2C Eeprom device address			*//* <S0E4> */
#define KPX_I2CEEP2_DEV_ADR_WR	(0xA2)						/* I2C Eeprom device address			*//* <S0E4> */
#define KPX_I2CEEP1_DEV_ADR_RD	(KPX_I2CEEP1_DEV_ADR_WR+1)	/* I2C Eeprom device address for read	*//* <S0E4> */
#define KPX_I2CEEP2_DEV_ADR_RD	(KPX_I2CEEP2_DEV_ADR_WR+1)	/* I2C Eeprom device address for read	*//* <S0E4> */
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_START( )  { 											/* Set Start Condition 		*/\
		I2C_CH0_REGBASE->IICC = ( WTIM | SPIE | STT ); 					/*							*/\
}
#define KPX_I2C_STOP( )   { 											/* Set STOP Condition 		*/\
		I2C_CH0_REGBASE->IICC = ( WTIM | SPIE | SPT );					/*							*/\
}
#define KPX_I2C_STOP_END_CHK( ) ( !(I2C_CH0_REGBASE->IICF & IICBSY ) )	/* check stop condition 	*/
#define KPX_I2C_START_END_CHK( )  ( !(I2C_CH0_REGBASE->IICF & STCF ) )	/* check start condition	*/

/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_CHK_TX_END( ) ( Kpx_ChkI2cInterrupt( ) )                /* chk sending complete 	*/
#define KPX_I2C_CHK_RX_END( ) ( Kpx_ChkI2cInterrupt( ) )                /* chk sending complete 	*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_CLR_INT( )    {Kpx_ClrI2cInterrupt( );}                 /* clear interrupt flag 	*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*  	Eeprom.c : Cpu I2C Eeprom I/F Driver														*/
/*--------------------------------------------------------------------------------------------------*/
LONG	I2CdrvInitCpuI2CBusIF( void );					/* Initialize I2C bus interface				*/
#if (SVFSEL_EEPROM_USE == 1) && (!defined(_NON_EEPROM_))	/* <S00D> */
LONG	EepI2Cdrv_ReadEepromWord( 						/* Read EEPROM One Word						*/
			ULONG EepAdr, 								/* EEPROM Address							*/
			USHORT *RdData,								/* WAIT: blocking read NOWAIT: queued read  */
			ULONG EepromNo );							/* EEPROM select							*//* <S0E4> */
LONG	EepI2Cdrv_WriteEepromWord(						/* Write EEPROM One Word					*/
			ULONG EepAdr, 								/* EEPROM Address							*/
			USHORT WrData,								/* WAIT: blocking write NOWAIT: queued write*/
			ULONG EepromNo );							/* EEPROM select							*//* <S0E4> */
#define ___CPU_I2C_HANDLER___( x )				( x )
#else
//#define	I2CdrvInitCpuI2CBusIF( )			{;}		/* <S00D> */
#define EepI2Cdrv_ReadEepromWord( x1, x2 )		(0)
#define	EepI2Cdrv_WriteEepromWord( x1, x2 )	{;}
#define ___CPU_I2C_HANDLER___( x )				(IntIgnore)
#endif
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		EEPROM Parameter Download Definition(EepromPrmDownload.c) <S00C>							*/
/*																									*/
/****************************************************************************************************/
typedef	struct
{
	USHORT	RwReq;								/* Read/Write Request								*/
	USHORT	RwStep;								/* Read/Write Processing Step						*/
  /*------------------------------------------------------------------------------------------------*/
	ULONG	RwCnt;								/* Read/Write Data Count							*/
	USHORT *RwBuf;								/* Read/Write Data Buffer							*/
	ULONG	ChkTimer;							/* Read/Write Processing Check Timer				*/
  /*------------------------------------------------------------------------------------------------*/
	UCHAR	EepWrDisable;						/* EEPROM Write Disable								*/
	UCHAR	SpareUchar[3];						/* Spare											*/
} RWALLMD;
/*--------------------------------------------------------------------------------------------------*/
#define RWALL_REQ_NONE		0x0000				/* No Request										*/
#define RWALL_REQ_READ		0x1000				/* Read  All Request								*/
#define RWALL_REQ_WRITE		0x2000				/* Write All Request								*/
/*--------------------------------------------------------------------------------------------------*/
#define RWALL_REQ_READOK	0x1001				/* Read  All Result OK								*/
#define RWALL_REQ_READNG	0x100F				/* Read  All Result NG								*/
/*--------------------------------------------------------------------------------------------------*/
#define RWALL_REQ_WRITEOK	0x2001				/* Write All Result OK								*/
#define RWALL_REQ_WRITENG	0x200F				/* Write All Result OK								*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_CHK_RWALL_NOREQ( RwReq )	( (RwReq == 0) || ((RwReq & 0x00FF) != 0) )

/*--------------------------------------------------------------------------------------------------*/
/*		APIs																						*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitDownloadEeprom( RWALLMD *pRwAllMng );	/* Initiazlie Download EEPROM				*/
void	KpxDownloadEeprom( RWALLMD *pRwAllMng );		/* Download EEPROM							*/
LONG	KriRequestReadAllEeprom( RWALLMD *pRwAllMng, USHORT *ReadBuf );
LONG	KriRequestWriteAllEeprom( RWALLMD *pRwAllMng, USHORT *WriteBuf );
LONG	KriChkDoneReadAllEeprom( RWALLMD *pRwAllMng );
LONG	KriChkDoneWriteAllEeprom( RWALLMD *pRwAllMng );

/****************************************************************************************************/
/*																									*/
/*		EEPROM Lcd Operator Parameter Download Definition(Eeprom_LcdDownload.c) <S031>				*/
/*																									*/
/****************************************************************************************************/
typedef	struct
{
	USHORT	RwReq;								/* Read/Write Request								*/
	USHORT	RwStep;								/* Read/Write Processing Step						*/
  /*------------------------------------------------------------------------------------------------*/
	ULONG	RwCnt;								/* Read/Write Data Count							*/
	USHORT *RwBuf;								/* Read/Write Data Buffer							*/
	ULONG	ChkTimer;							/* Read/Write Processing Check Timer				*/
  /*------------------------------------------------------------------------------------------------*/
	void	*PdlTbl;							/* Read/Write Parameter Download Table				*/
	ULONG	TblEntNum;							/* Read/Write Parameter Download Table Entry Number */
  /*------------------------------------------------------------------------------------------------*/
	USHORT	ChkSumX;							/* Check Sum										*/
	USHORT	ChkSumY;							/* Check Sum										*/
  /*------------------------------------------------------------------------------------------------*/
	UCHAR	EepWrDisable;						/* EEPROM Write Disable								*/
	UCHAR	SpareUchar[3];						/* Spare											*/
} RWPDLMD;
/*--------------------------------------------------------------------------------------------------*/
#define RWPDL_REQ_NONE		0x0000				/* No Request										*/
#define RWPDL_REQ_READ		0x1000				/* Read  All Request								*/
#define RWPDL_REQ_WRITE		0x2000				/* Write All Request								*/
/*--------------------------------------------------------------------------------------------------*/
#define RWPDL_REQ_READOK	0x1001				/* Read  All Result OK								*/
#define RWPDL_REQ_READNG	0x100F				/* Read  All Result NG								*/
/*--------------------------------------------------------------------------------------------------*/
#define RWPDL_REQ_WRITEOK	0x2001				/* Write All Result OK								*/
#define RWPDL_REQ_WRITENG	0x200F				/* Write All Result OK								*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		APIs																						*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitLcdPdlEeprom( RWPDLMD *pRwPdlMng );		/* Initiazlie Download EEPROM				*/
LONG	KpxLcdPdlEeprom( RWPDLMD *pRwPdlMng );			/* Download EEPROM							*/
LONG	KriRequestReadLcdPdlEeprom( RWPDLMD *pRwPdlMng, USHORT *ReadBuf, void *PdlTbl, LONG TblEntNum );
LONG	KriRequestWriteLcdPdlEeprom( RWPDLMD *pRwPdlMng, USHORT *WriteBuf, void *PdlTbl, LONG TblEntNum );
LONG	KriChkDoneReadLcdPdlEeprom( RWPDLMD *pRwPdlMng );
LONG	KriChkDoneWriteLcdPdlEeprom( RWPDLMD *pRwPdlMng );

/*--------------------------------------------------------------------------------------------------*/
/*		Temperature Detect IC Access Driver definitions		<S00D>									*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2CTMPC_DEV_ADR		(0x90)						/* Control board temp IC device address	*/
#define KPX_I2CTMPP_DEV_ADR		(0x94)						/* Power board temp IC device address	*/
#define	KPX_I2CTMP_TEMP_REG		0x0000						/* Temperature-Register (R/W)			*/
#define	KPX_I2CTMP_CONFIG_REG	0x0001						/* Configration-Register (R/W)			*/

/*--------------------------------------------------------------------------------------------------*/
/*		I2C Send/Recv Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_TX( data ) \
		{ \
			KPX_I2C_CLR_INT( );								/* Clear IIC Interrupt Request		*/\
			I2C_CH0_REGBASE->IIC = (data);					/* set send data 					*/\
		}
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_RX( ) \
		{ \
			KPX_I2C_CLR_INT( );								/* Clear IIC Interrupt Request		*/\
			I2C_CH0_REGBASE->IICC = (WREL | SPIE | ACKE);	/* start receiving ?? 				*/\
		}
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_RX_NOACK( ) \
		{ \
			I2C_CH0_REGBASE->IICC &= ~(ACKE);				/* Clear ACKE bit	 				*/\
		}
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_NackCnt( NakCnt )	{ if( !(I2C_CH0_REGBASE->IICS & ACKD) ){ NakCnt++; } }
/*--------------------------------------------------------------------------------------------------*/
#define KPX_I2C_GetRdData	(I2C_CH0_REGBASE->IIC)
/*--------------------------------------------------------------------------------------------------*/



#endif	/* ___I2C_DRIVER___ */
/***************************************** end of file **********************************************/
