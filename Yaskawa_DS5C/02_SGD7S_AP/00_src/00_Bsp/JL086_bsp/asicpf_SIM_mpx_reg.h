/**************************************************************************************************/
/*  ROMSIM BENCH                                                                                  */
/*                                                                                                */
/*   asicpf_mpx_reg.h                                                                             */
/*                                                                                                */
/*                                                Copyright(C) NEC Micro Systems, Ltd. 2007       */
/**************************************************************************************************/
/*  07.07.23 Ver.1.0  for ROMSIM bench.                                                           */
/*  07.07.26 Ver.1.1  (PORTW, PORTW) x2)                                                          */
/*  07.07.31 Ver.1.2  SIM_MPX => MPX                                                              */
/*                    add #ifndef-#define-#endif to SIM_MPX_GROUP0, SIM_MPX_GROUP1, SIM_MPX_BASE  */
/*  07.08.07 Ver.1.3  MPX => SIM_MPX                                                              */
/*                    SIM_MPX_GROUP0, SIM_MPX_GROUP1, SIM_MPX_BASE move to asicpf_SIM_cfg.h       */
/*  07.09.20 Ver.1.4  add printf function                                                         */
/*------------------------------------------------------------------------------------------------*/
/*-------------------------------------*/
/* --- MEMCPX                      --- */
/*-------------------------------------*/

#define	SIM_MEMCP0   (SIM_MPX_BASE + 0x0000)    /* simulation stop */
#define	SIM_CLKCTL   (SIM_MPX_BASE + 0x0004)	   /* clock control   */

/*-------------------------------------------------------------------------------------*/
/*																						*/
/*-------------------------------------------------------------------------------------*/
/* word */
#define SIM_MPX_PORTW0_0   (SIM_MPX_BASE + 0x1000 + (0x20 * SIM_MPX_GROUP0)) /* milestone */
#define SIM_MPX_PORTW0_1   (SIM_MPX_BASE + 0x1004 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_2   (SIM_MPX_BASE + 0x1008 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_3   (SIM_MPX_BASE + 0x100C + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_4   (SIM_MPX_BASE + 0x1010 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_5   (SIM_MPX_BASE + 0x1014 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_6   (SIM_MPX_BASE + 0x1018 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_7   (SIM_MPX_BASE + 0x101C + (0x20 * SIM_MPX_GROUP0))

#define SIM_MPX_PORTW1_0   (SIM_MPX_BASE + 0x1000 + (0x20 * SIM_MPX_GROUP1)) /* milestone */
#define SIM_MPX_PORTW1_1   (SIM_MPX_BASE + 0x1004 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTW1_2   (SIM_MPX_BASE + 0x1008 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTW1_3   (SIM_MPX_BASE + 0x100C + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTW1_4   (SIM_MPX_BASE + 0x1010 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTW1_5   (SIM_MPX_BASE + 0x1014 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTW1_6   (SIM_MPX_BASE + 0x1018 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTW1_7   (SIM_MPX_BASE + 0x101C + (0x20 * SIM_MPX_GROUP1))

#define SIM_MPX_PORTR0_0   (SIM_MPX_BASE + 0x2000 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_1   (SIM_MPX_BASE + 0x2004 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_2   (SIM_MPX_BASE + 0x2008 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_3   (SIM_MPX_BASE + 0x200C + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_4   (SIM_MPX_BASE + 0x2010 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_5   (SIM_MPX_BASE + 0x2014 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_6   (SIM_MPX_BASE + 0x2018 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_7   (SIM_MPX_BASE + 0x201C + (0x20 * SIM_MPX_GROUP0))

#define SIM_MPX_PORTR1_0   (SIM_MPX_BASE + 0x2000 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_1   (SIM_MPX_BASE + 0x2004 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_2   (SIM_MPX_BASE + 0x2008 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_3   (SIM_MPX_BASE + 0x200C + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_4   (SIM_MPX_BASE + 0x2010 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_5   (SIM_MPX_BASE + 0x2014 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_6   (SIM_MPX_BASE + 0x2018 + (0x20 * SIM_MPX_GROUP1))
#define SIM_MPX_PORTR1_7   (SIM_MPX_BASE + 0x201C + (0x20 * SIM_MPX_GROUP1))

/* Halfword,Byte */
#define SIM_MPX_PORTW0_00 (SIM_MPX_BASE + 0x1000 + (0x20 * SIM_MPX_GROUP0)) /* milestone */
#define SIM_MPX_PORTW0_01 (SIM_MPX_BASE + 0x1001 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_02 (SIM_MPX_BASE + 0x1002 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_03 (SIM_MPX_BASE + 0x1003 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_10 (SIM_MPX_BASE + 0x1004 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_11 (SIM_MPX_BASE + 0x1005 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_12 (SIM_MPX_BASE + 0x1006 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_13 (SIM_MPX_BASE + 0x1007 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_20 (SIM_MPX_BASE + 0x1008 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_21 (SIM_MPX_BASE + 0x1009 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_22 (SIM_MPX_BASE + 0x100a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_23 (SIM_MPX_BASE + 0x100b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_30 (SIM_MPX_BASE + 0x100c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_31 (SIM_MPX_BASE + 0x100d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_32 (SIM_MPX_BASE + 0x100e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_33 (SIM_MPX_BASE + 0x100f + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_40 (SIM_MPX_BASE + 0x1010 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_41 (SIM_MPX_BASE + 0x1011 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_42 (SIM_MPX_BASE + 0x1012 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_43 (SIM_MPX_BASE + 0x1013 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_50 (SIM_MPX_BASE + 0x1014 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_51 (SIM_MPX_BASE + 0x1015 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_52 (SIM_MPX_BASE + 0x1016 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_53 (SIM_MPX_BASE + 0x1017 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_60 (SIM_MPX_BASE + 0x1018 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_61 (SIM_MPX_BASE + 0x1019 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_62 (SIM_MPX_BASE + 0x101a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_63 (SIM_MPX_BASE + 0x101b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_70 (SIM_MPX_BASE + 0x101c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_71 (SIM_MPX_BASE + 0x101d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_72 (SIM_MPX_BASE + 0x101e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW0_73 (SIM_MPX_BASE + 0x101f + (0x20 * SIM_MPX_GROUP0))

#define SIM_MPX_PORTW1_00 (SIM_MPX_BASE + 0x1000 + (0x20 * SIM_MPX_GROUP0)) /* milestone */
#define SIM_MPX_PORTW1_01 (SIM_MPX_BASE + 0x1001 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_02 (SIM_MPX_BASE + 0x1002 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_03 (SIM_MPX_BASE + 0x1003 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_10 (SIM_MPX_BASE + 0x1004 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_11 (SIM_MPX_BASE + 0x1005 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_12 (SIM_MPX_BASE + 0x1006 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_13 (SIM_MPX_BASE + 0x1007 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_20 (SIM_MPX_BASE + 0x1008 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_21 (SIM_MPX_BASE + 0x1009 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_22 (SIM_MPX_BASE + 0x100a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_23 (SIM_MPX_BASE + 0x100b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_30 (SIM_MPX_BASE + 0x100c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_31 (SIM_MPX_BASE + 0x100d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_32 (SIM_MPX_BASE + 0x100e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_33 (SIM_MPX_BASE + 0x100f + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_40 (SIM_MPX_BASE + 0x1010 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_41 (SIM_MPX_BASE + 0x1011 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_42 (SIM_MPX_BASE + 0x1012 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_43 (SIM_MPX_BASE + 0x1013 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_50 (SIM_MPX_BASE + 0x1014 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_51 (SIM_MPX_BASE + 0x1015 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_52 (SIM_MPX_BASE + 0x1016 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_53 (SIM_MPX_BASE + 0x1017 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_60 (SIM_MPX_BASE + 0x1018 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_61 (SIM_MPX_BASE + 0x1019 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_62 (SIM_MPX_BASE + 0x101a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_63 (SIM_MPX_BASE + 0x101b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_70 (SIM_MPX_BASE + 0x101c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_71 (SIM_MPX_BASE + 0x101d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_72 (SIM_MPX_BASE + 0x101e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTW1_73 (SIM_MPX_BASE + 0x101f + (0x20 * SIM_MPX_GROUP0))

#define SIM_MPX_PORTR0_00 (SIM_MPX_BASE + 0x2000 + (0x20 * SIM_MPX_GROUP0)) /* milestome */
#define SIM_MPX_PORTR0_01 (SIM_MPX_BASE + 0x2001 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_02 (SIM_MPX_BASE + 0x2002 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_03 (SIM_MPX_BASE + 0x2003 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_10 (SIM_MPX_BASE + 0x2004 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_11 (SIM_MPX_BASE + 0x2005 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_12 (SIM_MPX_BASE + 0x2006 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_13 (SIM_MPX_BASE + 0x2007 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_20 (SIM_MPX_BASE + 0x2008 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_21 (SIM_MPX_BASE + 0x2009 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_22 (SIM_MPX_BASE + 0x200a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_23 (SIM_MPX_BASE + 0x200b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_30 (SIM_MPX_BASE + 0x200c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_31 (SIM_MPX_BASE + 0x200d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_32 (SIM_MPX_BASE + 0x200e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_33 (SIM_MPX_BASE + 0x200f + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_40 (SIM_MPX_BASE + 0x2010 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_41 (SIM_MPX_BASE + 0x2011 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_42 (SIM_MPX_BASE + 0x2012 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_43 (SIM_MPX_BASE + 0x2013 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_50 (SIM_MPX_BASE + 0x2014 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_51 (SIM_MPX_BASE + 0x2015 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_52 (SIM_MPX_BASE + 0x2016 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_53 (SIM_MPX_BASE + 0x2017 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_60 (SIM_MPX_BASE + 0x2018 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_61 (SIM_MPX_BASE + 0x2019 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_62 (SIM_MPX_BASE + 0x201a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_63 (SIM_MPX_BASE + 0x201b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_70 (SIM_MPX_BASE + 0x201c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_71 (SIM_MPX_BASE + 0x201d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_72 (SIM_MPX_BASE + 0x201e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR0_73 (SIM_MPX_BASE + 0x201f + (0x20 * SIM_MPX_GROUP0))

#define SIM_MPX_PORTR1_00 (SIM_MPX_BASE + 0x2000 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_01 (SIM_MPX_BASE + 0x2001 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_02 (SIM_MPX_BASE + 0x2002 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_03 (SIM_MPX_BASE + 0x2003 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_10 (SIM_MPX_BASE + 0x2004 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_11 (SIM_MPX_BASE + 0x2005 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_12 (SIM_MPX_BASE + 0x2006 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_13 (SIM_MPX_BASE + 0x2007 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_20 (SIM_MPX_BASE + 0x2008 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_21 (SIM_MPX_BASE + 0x2009 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_22 (SIM_MPX_BASE + 0x200a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_23 (SIM_MPX_BASE + 0x200b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_30 (SIM_MPX_BASE + 0x200c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_31 (SIM_MPX_BASE + 0x200d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_32 (SIM_MPX_BASE + 0x200e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_33 (SIM_MPX_BASE + 0x200f + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_40 (SIM_MPX_BASE + 0x2010 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_41 (SIM_MPX_BASE + 0x2011 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_42 (SIM_MPX_BASE + 0x2012 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_43 (SIM_MPX_BASE + 0x2013 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_50 (SIM_MPX_BASE + 0x2014 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_51 (SIM_MPX_BASE + 0x2015 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_52 (SIM_MPX_BASE + 0x2016 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_53 (SIM_MPX_BASE + 0x2017 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_60 (SIM_MPX_BASE + 0x2018 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_61 (SIM_MPX_BASE + 0x2019 + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_62 (SIM_MPX_BASE + 0x201a + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_63 (SIM_MPX_BASE + 0x201b + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_70 (SIM_MPX_BASE + 0x201c + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_71 (SIM_MPX_BASE + 0x201d + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_72 (SIM_MPX_BASE + 0x201e + (0x20 * SIM_MPX_GROUP0))
#define SIM_MPX_PORTR1_73 (SIM_MPX_BASE + 0x201f + (0x20 * SIM_MPX_GROUP0))

/* printf function */

#define SIM_MPX_DISPC  (SIM_MPX_BASE + 0x0100)
#define SIM_MPX_DISPH  (SIM_MPX_BASE + 0x0104)
#define SIM_MPX_DISPD  (SIM_MPX_BASE + 0x0108)

/*-------------------------------------*/
/* --- End Of File                 --- */
/*-------------------------------------*/

