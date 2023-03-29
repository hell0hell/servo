#ifndef STDCMD_FORMAT_H_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		M3StdDataFormat.h : M-III標準プロファイルフォーマット定義用ヘッダファイル					*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************/
/*							指令データフォーマット							*/
/****************************************************************************/
/* -------- Main Command -------- */
/* 共通コマンド */
#ifdef __BIG_ENDIAN__
typedef struct NOP_CMD_ {	/* NOP.. */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;
	UCHAR	dummy[28];
} NOP_CMD;
#else
typedef struct NOP_CMD_ {	/* NOP.. */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	UCHAR	dummy[28];
} NOP_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct ID_RD_CMD_ {	/* ID_RD */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;

	USHORT	size;
	UCHAR	offset;
	UCHAR	id_code;

	ULONG	id_info[6];
} ID_RD_CMD;
#else
typedef struct ID_RD_CMD_ {	/* ID_RD */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	UCHAR	id_code;
	UCHAR	offset;
	USHORT	size;
	ULONG	id_info[6];
} ID_RD_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct CONFIG_CMD_ {	/* CONFIG */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;

	UCHAR	dummy[3];
	UCHAR	config_mod;

	UCHAR	dummy2[24];
} CONFIG_CMD;
#else
typedef struct CONFIG_CMD_ {	/* CONFIG */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	UCHAR	config_mod;
	UCHAR	dummy[27];
} CONFIG_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct ALM_CMD_ {	/* ALM_RD/CLR */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;

	USHORT	index;
	USHORT	mode;

	USHORT	data[12];
} ALM_CMD;
#else
typedef struct ALM_CMD_ {	/* ALM_RD/CLR */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	USHORT	mode;
	USHORT	index;
	USHORT	data[12];
} ALM_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct CONNECT_CMD_ {	/* CONNECT */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;

	UCHAR	profile_type;
	UCHAR	com_time;
	UCHAR	com_mod;
	UCHAR	ver;

	UCHAR	dummy[24];
} CONNECT_CMD;
#else
typedef struct CONNECT_CMD_ {	/* CONNECT */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	UCHAR	ver;
	UCHAR	com_mod;
	UCHAR	com_time;
	UCHAR	profile_type;
	UCHAR	dummy[24];
} CONNECT_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct MEM_CMD_ {	/* MEM_RD/WR */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;

	USHORT	size;
	UCHAR	mode;
	UCHAR	dummy;

	ULONG	address;
	LONG	ldata[5];
} MEM_CMD;
#else
typedef struct MEM_CMD_ {	/* MEM_RD/WR */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	UCHAR	dummy;
	UCHAR	mode;
	USHORT	size;
	ULONG	address;
	LONG	ldata[5];
} MEM_CMD;
#endif /*__BIG_ENDIAN__*/


/* 標準サーボプロファイル用コマンド */
#ifdef __BIG_ENDIAN__
typedef struct MOTION_CMD_ {	/* Motion... */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;
	ULONG	svcmd_ctrl;
	ULONG	svcmd_io;
	LONG	data1;
	LONG	data2;
	LONG	data3;
	LONG	data4;
	LONG	data5;
} MOTION_CMD;
#else
typedef struct MOTION_CMD_ {	/* Motion... */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	ULONG	svcmd_ctrl;
	ULONG	svcmd_io;
	LONG	data1;
	LONG	data2;
	LONG	data3;
	LONG	data4;
	LONG	data5;
} MOTION_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct POS_SET_CMD_ {	/* POS_SET */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;
	ULONG	svcmd_ctrl;
	ULONG	svcmd_io;
	LONG	pos_set_mod;
	LONG	pos_data;
	LONG	data[3];
} POS_SET_CMD;
#else
typedef struct POS_SET_CMD_ {	/* POS_SET */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	ULONG	svcmd_ctrl;
	ULONG	svcmd_io;
	LONG	pos_set_mod;
	LONG	pos_data;
	LONG	data[3];
} POS_SET_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct SVPRM_CMD_ {	/* SVPRM_RD/WR */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;
	ULONG	svcmd_ctrl;
	ULONG	svcmd_io;
	UCHAR	mode;
	UCHAR	size;
	USHORT	no;
	LONG	ldata[4];
} SVPRM_CMD;
#else
typedef struct SVPRM_CMD_ {	/* SVPRM_RD/WR */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;
	ULONG	svcmd_ctrl;
	ULONG	svcmd_io;
	USHORT	no;
	UCHAR	size;
	UCHAR	mode;
	LONG	ldata[4];
} SVPRM_CMD;
#endif /*__BIG_ENDIAN__*/


/* MECHATROLINKフィールドメインコマンド構造体 */
typedef union CMD_STD_DATA_ {
//	UCHAR		byte[32];
//	USHORT		word[16];
//	ULONG		dword[8];
	NOP_CMD		nop;			/* NOP系 */
	ID_RD_CMD	id_rd;			/* ID_RD */
	CONFIG_CMD	config;			/* CONFIG */
	ALM_CMD		alm;			/* ALM_RD */
	CONNECT_CMD	connect;		/* CONNECT */
	MEM_CMD		mem;			/* MEM_RD/WR */
	MOTION_CMD	mt;				/* モーション系 */
	POS_SET_CMD	pos_set;		/* POS_SET */
	SVPRM_CMD	svprm;			/* SVPRM_RD / WR */
} CMD_STD_DATA;

/* MECHATROLINKフィールドメインコマンド構造体 */
typedef union CMD_LINK_DATA_ {
	ULONG			dword[8];
	CMD_STD_DATA	stdData;
//	CMD_M2_DATA		m2Data;
} CMD_LINK_DATA;



/* -------- Sub Command -------- */
#ifdef __BIG_ENDIAN__
typedef struct MON_EX_CMD_ {	/* Monitor */
	UCHAR	sub_ctrl2;
	UCHAR	sub_ctrl1;
	UCHAR	sub_ctrl0;
	UCHAR	cmd;
	LONG	mon4;
	LONG	mon5;
	LONG	mon6;
} MON_EX_CMD;
#else
typedef struct MON_EX_CMD_ {	/* Monitor */
	UCHAR	cmd;
	UCHAR	sub_ctrl0;
	UCHAR	sub_ctrl1;
	UCHAR	sub_ctrl2;
	LONG	mon4;
	LONG	mon5;
	LONG	mon6;
} MON_EX_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct PRM_EX_CMD_ {	/* SVPRM_RD/WR */
	UCHAR	sub_ctrl2;
	UCHAR	sub_ctrl1;
	UCHAR	sub_ctrl0;
	UCHAR	cmd;
	UCHAR	mode;
	UCHAR	size;
	USHORT	no;
	LONG	ldata[2];
} SVPRM_EX_CMD;
#else
typedef struct PRM_EX_CMD_ {	/* SVPRM_RD/WR */
	UCHAR	cmd;
	UCHAR	sub_ctrl0;
	UCHAR	sub_ctrl1;
	UCHAR	sub_ctrl2;
	USHORT	no;
	UCHAR	size;
	UCHAR	mode;
	LONG	ldata[2];
} SVPRM_EX_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct ALM_EX_CMD_ {	/* ALM_RD/CLR */
	UCHAR	sub_ctrl2;
	UCHAR	sub_ctrl1;
	UCHAR	sub_ctrl0;
	UCHAR	cmd;
	USHORT	index;
	USHORT	mode;
	USHORT	data[4];
} ALM_EX_CMD;
#else
typedef struct ALM_EX_CMD_ {	/* ALM_RD/CLR */
	UCHAR	cmd;
	UCHAR	sub_ctrl0;
	UCHAR	sub_ctrl1;
	UCHAR	sub_ctrl2;
	USHORT	mode;
	USHORT	index;
	USHORT	data[4];
} ALM_EX_CMD;
#endif /*__BIG_ENDIAN__*/


#ifdef __BIG_ENDIAN__
typedef struct MEM_EX_CMD_ {	/* MEM_RD/WR */
	USHORT	cmd_ctrl;
	UCHAR	wdt;
	UCHAR	cmd;

	USHORT	size;
	UCHAR	mode;
	UCHAR	dummy;

	ULONG	address;
	LONG	ldata;
} MEM_EX_CMD;
#else
typedef struct MEM_EX_CMD_ {	/* MEM_RD/WR */
	UCHAR	cmd;
	UCHAR	wdt;
	USHORT	cmd_ctrl;

	UCHAR	dummy;
	UCHAR	mode;
	USHORT	size;

	ULONG	address;
	LONG	ldata;
} MEM_EX_CMD;
#endif /*__BIG_ENDIAN__*/


/* MECHATROLINKフィールドサブコマンド構造体 */
typedef union CMD_STD_EXDATA_ {
//	UCHAR		byte[16];
//	USHORT		word[8];
//	ULONG		dword[4];
	MON_EX_CMD	mon;				/* モニタ */
	SVPRM_EX_CMD svprm;				/* SVPRM_RD/WR */
	ALM_EX_CMD	alm;				/* ALM_RD */
	MEM_EX_CMD	mem;				/* MEM_RD/WR */
} CMD_STD_EXDATA;


/* MECHATROLINKフィールドサブコマンド構造体 */
typedef union CMD_LINK_EXDATA_ {
	ULONG			dword[4];
	CMD_STD_EXDATA	stdData;
//	CMD_M2_EXDATA	m2Data;
} CMD_LINK_EXDATA;


/****************************************************************************/
/*							応答データフォーマット							*/
/****************************************************************************/
/****************************************************************************/
/*	RSP_CMD_STAT_BITS	#define定義											*/
/****************************************************************************/
enum RSP_CMD_STAT_BITNO {
	RSP_DEV_ALM_BIT_NO			= 0,
	RSP_DEV_WRN_BIT_NO			= 1,
	RSP_CMD_READY_BIT_NO		= 2,
	RSP_ALMCLR_CMP_BIT_NO		= 3,
	RSP_CMD_ALM_BIT_NO			= 8,
	RSP_COMM_ALM_BIT_NO			= 12,
};

#define	RSP_DEV_ALM_BIT			0x01
#define	RSP_DEV_WRN_BIT			0x02
#define	RSP_CMD_READY_BIT		0x04
#define	RSP_ALMCLR_CMP_BIT		0x08
#define	RSP_RCMD_ID				0x40
#define	RSP_CMD_ALM				0x100
#define	RSP_COMM_ALM			0x1000

#define	RSP_CLR_CMDSTAT_BITS	(0x00C0)
/****************************************************************************/



/****************************************************************************/
/*	RSP_SVCMD_STAT_BITS	#define定義											*/
/****************************************************************************/
enum RSP_SVCMD_STAT_BITNO {
	RSP_CMD_PAUSE_CMP_BITNO		= 0,
	RSP_CMD_CANCEL_CMP_BITNO	= 1,
	RSP_CMD_ACC_FILNO			= 4,
	RSP_CMD_LCOMP1_BITNO		= 8,
	RSP_CMD_LCOMP2_BITNO		= 9,
	RSP_CMD_POS_READY_BITNO		= 10,
	RSP_CMD_POWER_ON_BITNO		= 11,
	RSP_CMD_MOTOR_READY_BITNO	= 12,
	RSP_CMD_SERVO_ON_BITNO		= 13,
	RSP_CMD_SELMON1NO			= 16,
	RSP_CMD_SELMON2NO			= 20,
	RSP_CMD_SELMON3NO			= 24,
};

#define	RSP_CMD_PAUSE_CMP_BIT		0x01		
#define	RSP_CMD_CANCEL_CMP_BIT		0x02		

#define	RSP_CMD_ACC_FIL				0x10		

#define	RSP_CMD_LCOMP1_BIT			0x100		
#define	RSP_CMD_LCOMP2_BIT			0x200		
#define	RSP_CMD_POS_READY_BIT		0x400		
#define	RSP_CMD_POWER_ON_BIT		0x800		
#define	RSP_CMD_MOTOR_READY_BIT		0x1000		
#define	RSP_CMD_SERVO_ON_BIT		0x2000		

#define	RSP_CMD_SELMON1				0x10000		
#define	RSP_CMD_SELMON2				0x100000	
#define	RSP_CMD_SELMON3				0x1000000	
/****************************************************************************/




/****************************************************************************/
/*	RSP_SVCMD_IO_BITS	#define定義											*/
/****************************************************************************/
enum RSP_SVCMD_IO_BITNO {
	RSP_CMD_DEC_BITNO			= 1,
	RSP_CMD_POT_BITNO			= 2,
	RSP_CMD_NOT_BITNO			= 3,
	RSP_CMD_EXT1_BITNO			= 4,
	RSP_CMD_EXT2_BITNO			= 5,
	RSP_CMD_EXT3_BITNO			= 6,
	RSP_CMD_ESTOP_BITNO			= 7,

	RSP_CMD_BRK_ON_BITNO		= 9,
	RSP_CMD_PSOFT_LMT_BITNO		= 10,
	RSP_CMD_NSOFT_LMT_BITNO		= 11,
	RSP_CMD_DEN_BITNO			= 12,
	RSP_CMD_NEAR_BITNO			= 13,
	RSP_CMD_PSET_BITNO			= 14,
	RSP_CMD_ZPOINT_BITNO		= 15,
	RSP_CMD_TRQ_LMT_BITNO		= 16,
	RSP_CMD_VEL_LMT_BITNO		= 17,
	RSP_CMD_VEL_CMP_BITNO		= 18,
	RSP_CMD_ZSPD_BITNO			= 19,
/* <S0AC>start */
	RSP_CMD_IOSTS1_BITNO		= 24,
	RSP_CMD_IOSTS2_BITNO		= 25,
	RSP_CMD_IOSTS3_BITNO		= 26,
	RSP_CMD_IOSTS4_BITNO		= 27,
	RSP_CMD_IOSTS5_BITNO		= 28,
	RSP_CMD_IOSTS6_BITNO		= 29,
	RSP_CMD_IOSTS7_BITNO		= 30,
	RSP_CMD_IOSTS8_BITNO		= 31,
/* <S0AC>end */
};

#define	RSP_CMD_DEC_BIT			0x02		
#define	RSP_CMD_POT_BIT			0x04		
#define	RSP_CMD_NOT_BIT			0x08		
#define	RSP_CMD_EXT1_BIT		0x10		
#define	RSP_CMD_EXT2_BIT		0x20		
#define	RSP_CMD_EXT3_BIT		0x40		
#define	RSP_CMD_ESTOP_BIT		0x80		

#define	RSP_CMD_BRK_ON_BIT		0x200		
#define	RSP_CMD_PSOFT_LMT_BIT	0x400		
#define	RSP_CMD_NSOFT_LMT_BIT	0x800		
#define	RSP_CMD_DEN_BIT			0x1000		
#define	RSP_CMD_NEAR_BIT		0x2000		
#define	RSP_CMD_PSET_BIT		0x4000		
#define	RSP_CMD_ZPOINT_BIT		0x8000		
#define	RSP_CMD_TRQ_LMT_BIT		0x10000		
#define	RSP_CMD_VEL_LMT_BIT		0x20000		
#define	RSP_CMD_VEL_CMP_BIT		0x40000		
#define	RSP_CMD_ZSPD_BIT		0x80000		

#define	RSP_CMD_IO_STS1_BIT		0x1000000		
#define	RSP_CMD_IO_STS2_BIT		0x2000000		
#define	RSP_CMD_IO_STS3_BIT		0x4000000		
#define	RSP_CMD_IO_STS4_BIT		0x8000000		
#define	RSP_CMD_IO_STS5_BIT		0x10000000		
#define	RSP_CMD_IO_STS6_BIT		0x20000000		
#define	RSP_CMD_IO_STS7_BIT		0x40000000		
#define	RSP_CMD_IO_STS8_BIT		0x80000000		
/****************************************************************************/



/****************************************************************************/
/*	RSP_SUB_CMD_STAT_BITS	#define定義										*/
/****************************************************************************/
#define	RSP_SUBCMD_READY_BIT_NO		10			
#define	RSP_SUBCMD_ALM_BIT_NO		16			
#define	RSP_SUBCMD_CLR_BITS_MSK		0xFFF004FF
/****************************************************************************/



typedef union RSP_CMD_STAT_ {
//	UCHAR				RspCmdStatByte[2];
	USHORT				RspCmdStat;
/*	RSP_CMD_STAT_BITS	bitVal; */
} RSP_CMD_STAT;


typedef union RSP_SVCMD_STAT_ {
//	UCHAR				RspSvCmdStatByte[4];
	ULONG				RspSvCmdStat;
/*	RSP_SVCMD_STAT_BITS	bitVal; */
} RSP_SVCMD_STAT;


typedef union RSP_SVCMD_IO_ {
	ULONG				RspCmdIoBit;
/*	RSP_SVCMD_IO_BITS	bitVal; */
} RSP_SVCMD_IO;



typedef union RSP_SUB_CMD_STAT_ {
//	UCHAR				RspSubCmdStatByte[4];
	ULONG				RspSubCmdStat;
/*	RSP_SUB_CMD_STAT_BITS	bitVal; */
} RSP_SUB_CMD_STAT;



/****************************************************************************/
/*	Main Command															*/
/****************************************************************************/
#ifdef __BIG_ENDIAN__
typedef struct MOTION_RSP_ {
	USHORT			cmdStat;
	UCHAR			rwdt;
	UCHAR			cmd;

	ULONG			svCmdStat;
	ULONG			svCmdIo;
	LONG			cprm_mon1;
	LONG			cprm_mon2;
	LONG			mon1;
	LONG			mon2;
	LONG			mon3;
} MOTION_RSP;
#else
typedef struct MOTION_RSP_ {
	UCHAR			cmd;
	UCHAR			rwdt;
	USHORT			cmdStat;
	ULONG			svCmdStat;
	ULONG			svCmdIo;
	LONG			cprm_mon1;
	LONG			cprm_mon2;
	LONG			mon1;
	LONG			mon2;
	LONG			mon3;
} MOTION_RSP;
#endif	// __BIG_ENDIAN__


#ifdef __BIG_ENDIAN__
typedef struct NOP_RSP_ {
	USHORT			cmd_stat;
	UCHAR			rwdt;
	UCHAR			cmd;
	UCHAR			dummy[28];
} NOP_RSP;
#else
typedef struct NOP_RSP_ {
	UCHAR			cmd;
	UCHAR			rwdt;
	USHORT			cmd_stat;
	UCHAR			dummy[28];
} NOP_RSP;
#endif	// __BIG_ENDIAN__

/* MECHATROLINKフィールドメインコマンド構造体 */
typedef union RSP_STD_DATA_ {
	NOP_RSP			nop;			/* NOP系 */
	MOTION_RSP		mt;				/* モーション系 */
} RSP_STD_DATA;

/* MECHATROLINKフィールドメインコマンド構造体 */
typedef union RSP_LINK_DATA_ {
//	UCHAR			byte[32];
//	USHORT			word[16];
	ULONG			dword[8];
	RSP_STD_DATA	stdData;
//	RSP_M2_DATA		m2Data;
} RSP_LINK_DATA;

/****************************************************************************/
/*	Sub Command																*/
/****************************************************************************/
#ifdef __BIG_ENDIAN__
typedef struct MON_EX_RSP_ {
	UCHAR			subStat2;
	UCHAR			subStat1;
	UCHAR			subStat0;
	UCHAR			cmd;
	LONG			mon4;
	LONG			mon5;
	LONG			mon6;
} MON_EX_RSP;
#else
typedef struct MON_EX_RSP_ {
	UCHAR			cmd;
	UCHAR			subStat0;
	UCHAR			subStat1;
	UCHAR			subStat2;
	LONG			mon4;
	LONG			mon5;
	LONG			mon6;
} MON_EX_RSP;
#endif	// __BIG_ENDIAN__

/* MECHATROLINKフィールドサブコマンド構造体 */
typedef union RSP_STD_EXDATA_ {
	MON_EX_RSP		mon;		/* モニタ */
} RSP_STD_EXDATA;


/* MECHATROLINKフィールドサブコマンド構造体 */
typedef union RSP_LINK_EXDATA_ {
//	UCHAR			byte[16];
//	USHORT			word[8];
	ULONG			dword[4];
	RSP_STD_EXDATA	stdData;
//	RSP_M2_EXDATA	m2Data;
} RSP_LINK_EXDATA;



#define STDCMD_FORMAT_H_
#endif /* STDCMD_FORMAT_H_ */
/***************************************** end of file **********************************************/
