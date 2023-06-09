/************************************************************************************************************/
/*																											*/
/*																											*/
/*		ARMPF_SRAM.h : MercurySoC ペリフェラル SRAMヘッダ									<S008>				*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		ROM/SRAMコントローラ関連ヘッダファイル																*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.09.19  Y.Tanaka		初版															*/
/*																											*/
/************************************************************************************************************/
#include	"ARMPF.h"


#ifndef	_ARMPF_SRAM_H_
#define	_ARMPF_SRAM_H_

#define	ARMPF_SRAM_BASE	0xEFF30000

/*----------------------------------------------------------------------------------------------------------*/
/*																											*/
/*		ROM/SRAMレジスタ設定値																				*/
/*																											*/
/*----------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------*/
/*		ROM/SRAM register																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_SRAM_RESERVE0		(ARMPF_SRAM_BASE + 0x000)		/* reserved									*/
#define	ARMPF_SRAM_BSC			(ARMPF_SRAM_BASE + 0x004)		/* バスサイズ制御レジスタ					*/
#define	ARMPF_SRAM_SMC0			(ARMPF_SRAM_BASE + 0x008)		/* スタティックメモリ制御レジスタ0			*/
#define	ARMPF_SRAM_SMC1			(ARMPF_SRAM_BASE + 0x00C)		/* スタティックメモリ制御レジスタ1			*/


/*----------------------------------------------------------------------------------------------------------*/
/*		BSC(Bus Size Controller)																			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	BUS_SIZE16			1									/* 16bit bus size							*/
#define	BUS_SIZE32			3									/* 32bit bus size							*/
#define	BUS_SBS0			0									/* Area0 Cofigue							*/
#define	BUS_SBS1			2									/* Area1 Cofigue							*/
#define	SBSn_INI			0x5550								/* Reserve Area Cofigue(Default value)		*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SMC(Static Memory Controller)																		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ADDRESS_WAIT		0									/* Address set Wait bit						*/
#define	DATA_WAIT			4									/* Data Wait bit							*/
#define	WRITE_WAIT			8									/* Write recovery Wait bit					*/
#define	IDLE_WAIT			12									/* Idle Wait bit							*/

/*----------------------------------------------------------------------------------------------------------*/
/*		API MACRO																							*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SRAM_BSC_SET( area0, area1 )		ARMPF_REG_WRITE( ARMPF_SRAM_BSC,		/* Set BSCreg.				*/\
										( ((area0)<<BUS_SBS0) + ((area1)<<BUS_SBS1) + SBSn_INI) )
/*----------------------------------------------------------------------------------------------------------*/
#define	SRAM_SMC0_SET( AC, DW, WW, IW )	ARMPF_REG_WRITE( ARMPF_SRAM_SMC0,		/* Set SMC0reg.				*/\
										( ((AC)<<ADDRESS_WAIT) + ((DW)<<DATA_WAIT)\
										  + ((WW)<<WRITE_WAIT) + ((IW)<<IDLE_WAIT)) )
#define	SRAM_SMC1_SET( AC, DW, WW, IW )	ARMPF_REG_WRITE( ARMPF_SRAM_SMC1,		/* Set SMC1reg.				*/\
										( ((AC)<<ADDRESS_WAIT) + ((DW)<<DATA_WAIT)\
										  + ((WW)<<WRITE_WAIT) + ((IW)<<IDLE_WAIT)) )
/*----------------------------------------------------------------------------------------------------------*/


#endif
/*-------end of file----------------------------------------------------------------------------------------*/
