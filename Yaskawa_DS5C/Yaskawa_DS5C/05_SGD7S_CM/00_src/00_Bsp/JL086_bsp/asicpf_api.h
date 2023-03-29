/************************************************************************************************/
/* ROMSIM BENCH */
/* */
/* asicpf_api.h */
/* */
/* Copyrigth(C) NEC Micro Systems, Ltd. 2007 */
/*----------------------------------------------------------------------------------------------*/
/* 2007.07.23 Ver1.0 N.kanno */
/* 2007.07.30 Ver1.1 N.kanno add NORMAL_END,ERROR_END */
/* NILESTONE0_RD redefined by CPU_RDW */
/* NILESTONE1_RD redefined by CPU_RDW */
/* NILESTONE0_WR redefined by CPU_WRW */
/* NILESTONE1_WR redefined by CPU_WRW */
/* NORMALEND_WR redefined by CPU_WRW */
/* ERROREND_WR redefined by CPU_WRW */
/* 2007.08.28 Ver1.2 N.kanno add CLKCTL */
/* 2007.08.29 Ver1.3 N.kanno parameter wrapped by() */
/************************************************************************************************/
/*-------------------------------------------------------------------------------------------*/
/* サブシステムの検証でDUT,ROMSIM ベンチのリソースにアクセスする場合は以下のマクロを使用する */
/*-------------------------------------------------------------------------------------------*/
/*----------------------------*/
/* シミュレーション制御 */
/*----------------------------*/
#define NORMAL_END 0x900d00dd
#define ERROR_END 0x0bad00dd
#define UNDEFINST_END 0x00aa00dd
#define PREFETCH_ABORT_END 0x00bb00dd
#define DATA_ABORT_END 0x00cc00dd
#define FIQ_END 0x00ff00dd
/*----------------------------*/
/* リード，ライト */
/*----------------------------*/
#define CPU_WRW( addr, data ) (*((volatile unsigned long *)(unsigned long)(addr)) = ((unsigned long )(data)))
#define CPU_WRH( addr, data ) (*((volatile unsigned short *)(unsigned long)(addr)) = ((unsigned short)(data)))
#define CPU_WRB( addr, data ) (*((volatile unsigned char *)(unsigned long)(addr)) = ((unsigned char )(data)))
#define CPU_RDW( addr ) (*((volatile unsigned long *)(unsigned long)(addr)))
#define CPU_RDH( addr ) (*((volatile unsigned short *)(unsigned long)(addr)))
#define CPU_RDB( addr ) (*((volatile unsigned char *)(unsigned long)(addr)))
/*----------------------------*/
/* マイルストン設定用 */
/*----------------------------*/
#define MILESTONE0_WR( data ) (CPU_WRW( SIM_MPX_PORTW0_0, (data)))
#define MILESTONE0_RD( void ) (CPU_RDW( SIM_MPX_PORTW0_0 ))
#define MILESTONE1_WR( data ) (CPU_WRW( SIM_MPX_PORTW1_0, (data)))
#define MILESTONE1_RD( void ) (CPU_RDW( SIM_MPX_PORTW1_0 ))
/*----------------------------*/
/* シミュレーション終了制御 */
/*----------------------------*/
#define NORMALEND_WR( void ) (CPU_WRW( SIM_MEMCP0, (NORMAL_END)))
#define ERROREND_WR( void ) (CPU_WRW( SIM_MEMCP0, (ERROR_END)))
/*-------------------------------*/
/* クロック制御(MEMCPX:CLKCTRL) */
/*-------------------------------*/
#define ACLK_ON 0x04
#define HCLK_ON 0x02
#define PCLK_ON 0x01
#ifndef CLK_CTRL
#define CLK_CTRL(condition) (CPU_WRW( (SIM_CLKCTL), (condition)))
#endif
/*=====================*/
/* End Of File */
/*=====================*/

