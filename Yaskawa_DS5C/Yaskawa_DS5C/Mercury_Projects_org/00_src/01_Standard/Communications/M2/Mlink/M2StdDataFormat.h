#ifndef M2STDCMD_FORMAT_H_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		M2StdDataFormat.h : M-II�W���v���t�@�C���t�H�[�}�b�g��`�p�w�b�_�t�@�C��					*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����																				*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************/
/*							�w�߃f�[�^�t�H�[�}�b�g							*/
/****************************************************************************/
/* typedefs: struct */
/* -------- Main Command -------- */
typedef struct MOTION_CMD_ {	/* Motion... */
	UCHAR	cmd;
	UCHAR	lt_sgn;
	USHORT	option;
	LONG	pos1;
	LONG	pos2;
	UCHAR	sel_mon12;
	UCHAR	iocmd[2];
	UCHAR	wdt;
} MOTION_CMD;

typedef struct NOP_CMD_ {	/* NOP.. */
	UCHAR	cmd;
	UCHAR	lt_sgn;
	UCHAR	dummy[2];
	UCHAR	data[11];
	UCHAR	wdt;
} NOP_CMD;

typedef struct ID_RD_CMD_ {	/* ID_RD */
	UCHAR	cmd;
	UCHAR	dummy[3];
	UCHAR	dev_cod;
	UCHAR	offset;
	UCHAR	size;
	UCHAR	id_info[8];
	UCHAR	wdt;
} ID_RD_CMD;

typedef struct CONNECT_CMD_ {	/* CONNECT */
	UCHAR	cmd;
	UCHAR	dummy1[3];
	UCHAR	ver;
	UCHAR	com_mod;
	UCHAR	com_time;
	UCHAR	dummy2[8];
	UCHAR	wdt;
} CONNECT_CMD;

typedef struct ALM_CMD_ {	/* ALM_RD */
	UCHAR	cmd;
	UCHAR	dummy1[3];
	UCHAR	mode;
	UCHAR	data[10];
	UCHAR	wdt;
} ALM_CMD;

typedef struct POS_SET_CMD_ {	/* POS_SET */
	UCHAR	cmd;
	UCHAR	dummy1[3];
	UCHAR	ps_cmd;
	UCHAR	pos[4];
	UCHAR	dummy2[6];
	UCHAR	wdt;
} POS_SET_CMD;

typedef struct PRM_CMD_ {	/* PRM_RD/WR */
	UCHAR	cmd;
	UCHAR	dummy[3];
	USHORT	no;
	UCHAR	size;
	UCHAR	data[8];
	UCHAR	wdt;
} PRM_CMD;

typedef struct ADJ_CMD3_ {	/* ADJ3 */
	UCHAR	cmd;
	UCHAR	subcode;
	UCHAR	dummy[2];
	USHORT	ccmd;
	USHORT	caddress;
	USHORT	csize;
	UCHAR	cdata[4];
	UCHAR	dummy2;
	UCHAR	wdt;
} ADJ_CMD3;

typedef struct ADJ_CMD2_ {	/* ADJ2 */
	UCHAR	cmd;
	UCHAR	subcode;
	UCHAR	dummy[2];
	UCHAR	ccmd;
	UCHAR	caddress[2];
	UCHAR	cdata[2];
	UCHAR	dummy2[6];
	UCHAR	wdt;
} ADJ_CMD2;


/* MECHATROLINK�t�B�[���h���C���R�}���h�\���� */
typedef union CMD_LINK_DATA_ {
	UCHAR		byte[16];
	USHORT		word[8];
	ULONG		dword[4];
	NOP_CMD		nop;		/* NOP�n */
	MOTION_CMD	mt;			/* ���[�V�����n */
	ID_RD_CMD	id_rd;		/* ID_RD */
	CONNECT_CMD	connect;	/* CONNECT */
	ALM_CMD		alm;		/* ALM_RD */
	POS_SET_CMD	pos_set;	/* POS_SET */
	PRM_CMD		prm;		/* PRM_RD / WR */
	ADJ_CMD3	adj;		/* ADJ(Sigma3) */
	ADJ_CMD2	adj2;		/* ADJ(Sigma2) */
} CMD_LINK_DATA;

/* -------- Sub Command -------- */
typedef struct MON_EX_CMD_ {	/* Monitor */
	UCHAR	cmd;
	UCHAR	lt_sgn;
	UCHAR	sel_mon34;
	UCHAR	mon3[4];
	UCHAR	mon4[4];
} MON_EX_CMD;

typedef struct PRM_EX_CMD_ {	/* PRM_RD/WR */
	UCHAR	cmd;
	UCHAR	dummy;
	USHORT	no;
	UCHAR	size;
	UCHAR	data[8];
} PRM_EX_CMD;

typedef struct ALM_EX_CMD_ {	/* CONNECT, ID_RD */
	UCHAR	cmd;
	UCHAR	dummy;
	UCHAR	mode;
	UCHAR	data[10];
} ALM_EX_CMD;

/* MECHATROLINK�t�B�[���h�T�u�R�}���h�\���� */
typedef union CMD_LINK_EXDATA_ {
	UCHAR		byte[16];
	USHORT		word[8];
	ULONG		dword[4];
	MON_EX_CMD	mon;	/* ���j�^ */
	PRM_EX_CMD	prm;	/* PRM_RD/WR */
	ALM_EX_CMD	alm;	/* ALM_RD */
} CMD_LINK_EXDATA;


/****************************************************************************/
/*							�����f�[�^�t�H�[�}�b�g							*/
/****************************************************************************/
/****************************************************************************/
/*	RSP_CMD_STAT_BITS	#define��`											*/
/****************************************************************************/
enum RSP_CMD_STAT_BITNO {
	RSP_DEV_ALM_BIT_NO		= 0,	/* �A���[��								*/
	RSP_DEV_WRN_BIT_NO		= 1,	/* ���[�j���O							*/
	RSP_CMD_READY_BIT_NO	= 2,	/* �R�}���h���f�B						*/
	RSP_SVON_BIT_NO			= 3,	/* �T�[�{�I��							*/
	
	RSP_POWER_ON_BIT_NO		= 4,	/* ��d���I��							*/
	RSP_MLOCK_BIT_NO		= 5,	/* ��������								*/
	RSP_ZPOINT_BIT_NO		= 6,	/* ���_�ʒu�͈͓�						*/
	RSP_PSET_BIT_NO			= 7,	/* �ʒu���ߊ����͈͓� / ���x��v		*/
	
	RSP_DEN_ON_BIT_NO		= 8,	/* �����o���� / �[�����x���o			*/
	RSP_TRQ_LIM_BIT_NO		= 9,	/* �g���N������							*/
	RSP_LCMP_BIT_NO			= 10,	/* ���b�`����							*/
	RSP_NEAR_BIT_NO			= 11,	/* �ʒu���ߋߖT�͈͓� / ���x�������o	*/
	
	RSP_P_SOFT_LIM_BIT_NO	= 12,	/* �����\�t�g���~�b�g					*/
	RSP_N_SOFT_LIM_BIT_NO	= 13,	/* �����\�t�g���~�b�g					*/
};

/* ���g�p	*/
#define	RSP_DEV_ALM_BIT		0x0001
#define	RSP_DEV_WRN_BIT		0x0002
#define	RSP_CMD_READY_BIT	0x0004
#define	RSP_SVON_BIT		0x0008

#define	RSP_POWER_ON_BIT	0x0010
#define	RSP_MLOCK_BIT		0x0020
#define	RSP_ZPOINT_BIT		0x0040
#define	RSP_PSET_BIT		0x0080

#define	RSP_DEN_ON_BIT		0x0100
#define	RSP_TRQ_LIM_BIT		0x0200
#define	RSP_LCMP_BIT		0x0400
#define	RSP_NEAR_BIT		0x0800

#define	RSP_PSOFT_LIM_BIT	0x1000
#define	RSP_NSOFT_LIM_BIT	0x2000


#define	RSP_CLR_CMDSTAT_BITS	(0xC000)

/****************************************************************************/
/*	RSP_SUB_CMD_STAT_BITS	#define��`										*/
/****************************************************************************/
#define	RSP_SUBCMD_WRN_BIT_NO		1
#define	RSP_SUBCMD_READY_BIT_NO		2

#define	RSP_SUBCMD_CLR_BITS_MSK		0xFFF004FF


/****************************************************************************/
typedef union RSP_CMD_STAT_ {
//	UCHAR				RspCmdStatByte[2];
	USHORT				RspCmdStat;
/*	RSP_CMD_STAT_BITS	bitVal; */
} RSP_CMD_STAT;


typedef union RSP_SUB_CMD_STAT_ {
//	UCHAR				RspSubCmdStatByte[4];
	ULONG				RspSubCmdStat;
/*	RSP_SUB_CMD_STAT_BITS	bitVal; */
} RSP_SUB_CMD_STAT;


/****************************************************************************/
/*	Main Command															*/
/****************************************************************************/
typedef struct MOTION_RSP_ {		/* Motion... */
	UCHAR			cmd;
	UCHAR			alarm;
	USHORT			status;
	LONG			mon1;
	LONG			mon2;
	UCHAR			sel_mon12;
	UCHAR			io_mon[2];
	UCHAR			wdt;
} MOTION_RSP;

typedef struct NOP_RSP_ {
	UCHAR			cmd;
	UCHAR			alarm;
	USHORT			status;
	UCHAR			dummy[11];
	UCHAR			wdt;
} NOP_RSP;


/* MECHATROLINK�t�B�[���h���C���R�}���h�\���� */
typedef union RSP_LINK_DATA_ {
	UCHAR			byte[16];
	USHORT			word[8];
	ULONG			dword[4];
	NOP_RSP			nop;			/* NOP�n */
	MOTION_RSP		mt;				/* ���[�V�����n */
} RSP_LINK_DATA;

/****************************************************************************/
/*	Sub Command																*/
/****************************************************************************/
typedef struct MON_EX_RSP_ {		/* Monitor */
	UCHAR			cmd;
	UCHAR			status;
	UCHAR			sel_mon;
	UCHAR			mon3[4];
	UCHAR			mon4[4];
	UCHAR			exSts[2];
} MON_EX_RSP;


/* MECHATROLINK�t�B�[���h�T�u�R�}���h�\���� */
typedef union RSP_LINK_EXDATA_ {
	UCHAR			byte[16];
	USHORT			word[8];
	ULONG			dword[4];
	MON_EX_RSP		mon;			/* ���j�^ */
} RSP_LINK_EXDATA;



#define M2STDCMD_FORMAT_H_
#endif /* M2STDCMD_FORMAT_H_ */
/***************************************** end of file **********************************************/
