/**
 **************************************************************************************************
 * 
 * @file  nppfahbmemsramsp1.c
 * @brief sample driver program for Memory (Flash/SRAM/SDR-SDRAM) controller 
 * 
 * @attention 
 * @note Memory controller (NPPFAHBMEMSRAMSP1V10) is developed by Renesas Electronics Corporation.
 * 
 * $Rev:: 260                        $
 * $Date:: 2011-09-29 19:03:01 +0900#$
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

/*
 *=================================================================================================
 *
 *  I N C L U D E 
 *
 *=================================================================================================
 */
#include "nppfahbmemsramsp1.h"


/*
 *=================================================================================================
 *
 *  D E F I N E 
 *
 *=================================================================================================
 */
/** Memory Controller Register Mapping */
typedef struct {
	volatile unsigned int Rsrvd_000H[(0x04-0x00)/4];	/**< (reserved) */
	volatile unsigned int BSC ;						/**< [reg] Bus Size Control */
	volatile unsigned int SMC[4];						/**< [reg] Sram Mode Control #0,#1,#2,#3 */
	volatile unsigned int PRC ;						/**< [reg] Page Rom Control */
	volatile unsigned int DMC[4];						/**< [reg] Dram Mode Control #0,#1,#2,#3 */
	volatile unsigned int RFC[4];						/**< [reg] dram ReFresh Control #0,#1,#2,#3 */
	volatile unsigned int Rsrvd_03CH[(0x80-0x3C)/4];	/**< (reserved) */
} tMEMC_REG;

#define MEMC_REG_ST(a)   do { memc_reg = (tMEMC_REG*)(a); } while (0)
#define MEMC_REG_WR(a,d) do { (memc_reg->a) = (d); } while (0)
#define MEMC_REG_RD(a,d) do { (d) = (memc_reg->a); } while (0)

#define MEMC_SDRAM_REFRESH_SOURCE_CLOCK_FACTOR (32*1000000)	/* clock source clock factor for refresh interval (RFC.RCC = 0x0) */
#define ER_PARAM -3
/*
 *=================================================================================================
 *
 *  V A R I A B L E 
 *
 *=================================================================================================
 */


/*
 *=================================================================================================
 *
 *  P R O T O T Y P E 
 *
 *=================================================================================================
 */
static unsigned int memc_div(unsigned int time, unsigned int clock) __attribute__ ((section ("_memc_init_")));


/*
 *=================================================================================================
 *
 *  P R O G R A M 
 *
 *=================================================================================================
 */

/**
 **************************************************************************************************

  @brief   Initialize memory controller 
  @param   *init : parameter table for initializing memory controller registers 
  @return  error condition 
  @retval  ER_OK    : no error 
  @retval  ER_PARAM : parameter error 

 **************************************************************************************************
 */
#pragma arm section code = "_memc_init_"
int memc_init(tMEMC_INIT* init)
{
	tMEMC_REG* memc_reg;	/* register base address */
	unsigned short i;					/* loop counter */
	unsigned int ref_factor;		/* refresh interval factor */
	unsigned int value;			/* temporary value */
	
	/* check parameter (bus clock frequency) */
	if (init->busclock == 0)
	{
		return ER_PARAM;
	}
	
	/* check parameter (page rom spec.) */
	if (( ((init->page.type != 0) && (init->page.type != 1)))
	 || ( ((init->page.size != 8) && (init->page.size != 16) && (init->page.size != 32) && (init->page.size != 64) && (init->page.size != 128)))
	 || (!((init->page.wait >= 1) && (init->page.wait <= 15))))
	{
		return ER_PARAM;
	}
	
	/* check parameter (sram spec.) */
	for (i = 0; i < 4; i++)
	{
		if (init->sram[i].bus_size == (unsigned int)0xffffffff)
		{
			;	/* do not care */
		}
		else if ((init->sram[i].bus_size == 8) || (init->sram[i].bus_size == 16) || (init->sram[i].bus_size == 32))
		{
			if ((!(                                  (init->sram[i].addr_wait <= 15)))
			 || (!((init->sram[i].data_wait >= 1) && (init->sram[i].data_wait <= 15)))
			 || (!((init->sram[i].wrcv_wait >= 1) && (init->sram[i].wrcv_wait <= 15)))
			 || (!((init->sram[i].idle_wait >= 1) && (init->sram[i].idle_wait <= 16))))
			{
				return ER_PARAM;
			}
		}
		else
		{
			return ER_PARAM;
		}
	}
	
	/* check parameter (sdram spec.) */
	for (i = 0; i < 4; i++)
	{
		if (init->sdram[i].bus_size == (unsigned int)0xffffffff)
		{
			;	/* do not care */
		}
		else if ((init->sdram[i].bus_size == 16) || (init->sdram[i].bus_size == 32))
		{
			ref_factor = (init->sdram[i].tref_spec * init->busclock);
			ref_factor = memc_div(ref_factor, MEMC_SDRAM_REFRESH_SOURCE_CLOCK_FACTOR);
			if ((!((init->sdram[i].cas_latency   >= 1 ) && (init->sdram[i].cas_latency   <= 3  )))
			 || (!((init->sdram[i].bank_interval >= 1 ) && (init->sdram[i].bank_interval <= 3  )))
			 || (!((init->sdram[i].row_width     >= 11) && (init->sdram[i].row_width     <= 13 )))
			 || (!((init->sdram[i].col_width     >= 8 ) && (init->sdram[i].col_width     <= 11 )))
			 || (!((init->sdram[i].mode_ref      >= 2 ) && (init->sdram[i].mode_ref      <= 32 )))
			 || (!((ref_factor                   >= 1 ) && (ref_factor                   <= 256))))
			{
				return ER_PARAM;
			}
		}
		else
		{
			return ER_PARAM;
		}
	}
	
	/* Set base address */
	memc_reg = (tMEMC_REG*)(MEMC_REGISTER_BASE);
	
	/* SRAM setting */
	for (i = 0; i < 4; i++)
	{
		if (init->sram[i].bus_size != (unsigned int)0xffffffff)
		{
			/* bus size */
			MEMC_REG_RD(BSC, value);
			if (init->sram[i].bus_size == 16)
			{
				value &= ~((0x1 << (i * 2)) << 1);
			}
			else
			{
				value |=  ((0x1 << (i * 2)) << 1);
			}
			MEMC_REG_WR(BSC, value);
			
			/* timming */
			value = 0;
			value |= (((init->sram[i].addr_wait + 0) & 0xf) << 0 );
			value |= (((init->sram[i].data_wait + 0) & 0xf) << 4 );
			value |= (((init->sram[i].wrcv_wait + 0) & 0xf) << 8 );
			value |= (((init->sram[i].idle_wait - 1) & 0xf) << 12);
			MEMC_REG_WR(SMC[i], value);
		}
	}
	
	/* PAGE ROM setting */
	value = 0;
	value |= ((((init->page.type >> 0) - 0) & 0x1) << 0 );
	value |= ((((init->page.size >> 3) - 1) & 0xf) << 16);
	value |= ((((init->page.wait >> 0) - 0) & 0xf) << 28);
	MEMC_REG_WR(PRC, value);
	
	/* SDRAM setting */
	for (i = 0; i < 4; i++)
	{
		if (init->sdram[i].bus_size != (unsigned int)0xffffffff)
		{
			/* bus size */
			MEMC_REG_RD(BSC, value);
			if (init->sdram[i].bus_size == 16)
			{
				value &= ~((0x1 << (i * 2 + 8)) << 1);
			}
			else
			{
				value |=  ((0x1 << (i * 2 + 8)) << 1);
			}
			MEMC_REG_WR(BSC, value);
			/* refresh */
			ref_factor = (init->sdram[i].tref_spec * init->busclock);
			ref_factor = memc_div(ref_factor, MEMC_SDRAM_REFRESH_SOURCE_CLOCK_FACTOR);
			value = 0;
			value |= (((init->sdram[i].mode_ref - 1) & 0x1f) << 16);			/* RFC.RFP */
			value |= ((((unsigned int)0x1             + 0) & 0x01) << 15);			/* RFC.REN */
			value |= ((((unsigned int)0x0             + 0) & 0x03) << 8 );			/* RFC.RCC */
			value |= (((ref_factor              - 1) & 0xff) << 0 );			/* RFC.RIN */
			MEMC_REG_WR(RFC[i], value);
			/* bus control */
			value= 0;
			value |= (((init->sdram[i].cas_latency   + 0 ) & 0xf) << 28);		/* DMC.LTM */
			value |= (((init->sdram[i].bank_interval + 0 ) & 0x3) << 22);		/* DMC.BCW */
			value |= (((init->sdram[i].row_width     - 11) & 0x3) << 18);		/* DMC.RAW */
			value |= (((init->sdram[i].col_width     - 8 ) & 0x3) << 16);		/* DMC.SAW */
			MEMC_REG_WR(DMC[i], value);
			do
			{
				MEMC_REG_RD(DMC[i], value);
			} while ((value & 0x00008000) != 0x00008000);						/* DMC.IST */
		}
	}
	
	return 0;
}
#pragma arm section code


/**
 **************************************************************************************************
  @brief   Simplified division calculation 
  @param   dividend : dividend 
  @param   divisor  : divisor 
  @return  result of division 
 **************************************************************************************************
 */
static unsigned int memc_div(unsigned int dividend, unsigned int divisor)
{
	unsigned int result;
	
	/* check parameter */
	if (divisor == 0)
	{
		return 0;
	}
	
	/* division */
	result = 0;
	while (dividend >= divisor)
	{
		dividend -= divisor;
		result++;
	}
	
	return result;
}

