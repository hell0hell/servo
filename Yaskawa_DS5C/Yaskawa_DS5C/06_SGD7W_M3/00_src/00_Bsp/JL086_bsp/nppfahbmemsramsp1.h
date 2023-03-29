/**
 **************************************************************************************************
 * 
 * @file  nppfahbmemsramsp1.h
 * @brief sample driver header for Memory (Flash/SRAM/SDR-SDRAM) controller 
 * 
 * @attention 
 * @note Memory controller (NPPFAHBMEMSRAMSP1V10) is developed by Renesas Electronics Corporation.
 * 
 * $Rev:: 322                        $
 * $Date:: 2012-01-05 17:15:55 +0900#$
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

#ifndef NPPFAHBMEMSRAMSP1_H__
#define NPPFAHBMEMSRAMSP1_H__

/*
 *=================================================================================================
 *
 *  I N C L U D E 
 *
 *=================================================================================================
 */
//#include "errcodes.h"
//#include "typedefs.h"
#include "config.h"


/*
 *=================================================================================================
 *
 *  D E F I N E 
 *
 *=================================================================================================
 */
#define MEMC_REGISTER_BASE   (unsigned int)CONFIG_MEMC_REGBASE			/**< MEMC register base address */
#define MEMC_REGISTER_OFFSET (unsigned int)CONFIG_MEMC_REGOFFSET			/**< MEMC register offset address */

/** Memory Controller SUBSYSTEM Initialize Table */
typedef struct
{
	/* global spec. */
	unsigned int busclock;					/**< Bus clock frequency */
	/* port0 page rom spec. */
	struct
	{
		unsigned int type;					/**< Memory type (0=SRAM,1=PAGE ROM) */
		unsigned int size;					/**< Page size (byte) */
		unsigned int wait;					/**< Page access wait cycle */
	} page;
	/* port0/1/2/3 sram spec. */
	struct
	{
		unsigned int bus_size;				/**< Bus size */
		unsigned int addr_wait;				/**< Address wait */
		unsigned int data_wait;				/**< Data wait */
		unsigned int wrcv_wait;				/**< Write recovary wait */
		unsigned int idle_wait;				/**< Idle wait */
	} sram[4];
	/* port0/1/2/3 sdram spec. */
	struct
	{
		unsigned int bus_size;				/**< Bus size */
		unsigned int cas_latency;				/**< CAS latency */
		unsigned int bank_interval;			/**< Bank active interval */
		unsigned int row_width;				/**< Row address width */
		unsigned int col_width;				/**< Column address width */
		unsigned int mode_ref;				/**< refrech interval on mode setting */
		unsigned int tref_spec;				/**< refrech interval spec. */
	} sdram[4];
} tMEMC_INIT;


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
int memc_init(tMEMC_INIT* init);


#endif	/* NPPFAHBMEMSRAMSP1_H__ */
