/*********************************************************************************************/
/*  ROMSIM BENCH                                                                             */
/*                                                                                           */
/*  ROMSIM BENCH test program                                                                */
/*                                                                                           */
/*  asicpf_sim_cfg.h                                                                         */
/*                                                Copyrigth(C) NEC Micro Systems, Ltd. 2007  */
/*********************************************************************************************/
/*  07.08.28 Ver.1.00 N.kanno                                                                */
/*-------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------*/
/* BASE ADDRESS                                             */
/*----------------------------------------------------------*/
#define CPU_GIC_D_BASE  0x21200000
#define CPU_GIC_C_BASE  0x21201000

//#define SIM_MPX_BASE  0xc0600000
#define SIM_MPX_BASE  0xEF010000



/*----------------------------------------------------------*/
/* �и���ǥ�����ݡ�����³����                             */
/*----------------------------------------------------------*/
#ifndef SIM_MPX_GROUP0
      #define SIM_MPX_GROUP0 0
#endif

#ifndef SIM_MPX_GROUP1
      #define SIM_MPX_GROUP1 1
#endif

/*----------------------------------------------------------*/
/* INTSRCü����³����                                       */
/*----------------------------------------------------------*/

//#define SIM_INTSRC_TEST0 0 /* INTSRC[0] �˳������ */
//#define SIM_INTSRC_TEST1 1 /* INTSRC[1] �˳������ */
//#define SIM_INTSRC_TEST2 2 /* INTSRC[2] �˳������ */
//#define SIM_INTSRC_TEST3 3 /* INTSRC[3] �˳������ */
//#define SIM_INTSRC_TEST4 4 /* INTSRC[4] �˳������ */
//#define SIM_INTSRC_TEST5 5 /* INTSRC[5] �˳������ */
//#define SIM_INTSRC_TEST6 6 /* INTSRC[6] �˳������ */
//#define SIM_INTSRC_TEST7 7 /* INTSRC[7] �˳������ */

/*=====================*/
/*   End Of File       */
/*=====================*/
