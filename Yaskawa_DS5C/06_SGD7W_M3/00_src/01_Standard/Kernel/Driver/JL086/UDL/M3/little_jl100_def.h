/****************************************************************************/
/*                                                                          */
/*        LITTLE_JL100_DEF.H : Common header of JL-100 access driver        */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*       Defines:                                                           */
/*          Definetion of Host Interface registers                          */
/*                                                                          */
/******** Copyright(C) 2007 Yaskawa Electric Corporation ********************/
/*                                                                          */
/*		Ver 1.10			:	2010.03.31	Initial coding	T.soeda			*/
/*                                                                          */
/****************************************************************************/
#ifndef		JL100_DEF_H_
#define		JL100_DEF_H_

#include	"Basedef.h"

#if 0
/***********************************************************************/
/*   Type definition                                                   */
/***********************************************************************/
#ifndef		SHORT
typedef		short          SHORT;
#endif		/* SHORT */
#ifndef		USHORT
typedef		unsigned short USHORT;
#endif		/* USHORT */
#ifndef		LONG
typedef		long           LONG;
#endif		/* LONG */
#ifndef		ULONG
typedef		unsigned long  ULONG;
#endif		/* ULONG */
#ifndef		VULONG
typedef		volatile unsigned long	VULONG;
#endif		/* VULONG */
#ifndef		VOID
typedef		void           VOID;
#endif		/* VOID */
#ifndef		INT
typedef		int            INT;
#endif		/* int */
#ifndef		UINT
typedef		unsigned int   UINT;
#endif		/* UINT */
#ifndef		UCHAR
typedef		unsigned char   UCHAR;
#endif		/* UCHAR */
#endif		/* #if 0 */


/***********************************************************************/
/*      HOST I/F register bit pattern definition                       */
/***********************************************************************/

/* CMD register */
#define JL100_CMD_GTX_R			0x00000001
#define JL100_CMD_RBX_R			0x00000002
#define JL100_CMD_CBX_R			0x00000004
#define JL100_CMD_TX_R			0x00000008		/* for only async mode CMD regs */
#define JL100_CMD_C1MTX_R		0x00000010
#define JL100_CMD_C1MRX_R		0x00000020
#define JL100_CMD_C1ABT_R		0x00000040
#define JL100_CMD_RX_R			0x00000080		/* for only async mode CMD regs */
#define JL100_CMD_C2MTX_R		0x00000100
#define JL100_CMD_C2MRX_R		0x00000200
#define JL100_CMD_C2ABT_R		0x00000400
#define JL100_CMD_WD_R			0x00001000
#define JL100_CMD_GTIM_RST		0x00004000
#define JL100_CMD_RST_ASC		0x00008000
#define JL100_CMD_P_MOD_R		0x00010000
#define JL100_CMD_DTCT_R		0x00020000
#define JL100_CMD_PAR_R			0x00040000
#define JL100_CMD_DLY_R			0x00080000
#define JL100_CMD_SCNF_R		0x00100000
#define JL100_CMD_C_START_R		0x00200000
#define JL100_CMD_A_START_R		0x00400000

/* STS register */
#define JL100_STS_COM_ENA		0x00000001
#define JL100_STS_M3_MOD		0x00000002
#define JL100_STS_FM_MOD		0x00000004
#define JL100_STS_PRGSEL		0x00000008
#define JL100_STS_TEST0			0x00000010
#define JL100_STS_RDY			0x00002000
#define JL100_STS_ERRL			0x00004000
#define JL100_STS_ERR			0x00008000

/* INTS register & IMK register */
#define JL100_INTS_GTX_I		0x00000001
#define JL100_INTS_TX_I			0x00000008		/* for only async mode INTS regs */
#define JL100_INTS_C1MTX_I		0x00000010
#define JL100_INTS_C1MRX_I		0x00000020
#define JL100_INTS_C1ABT_I		0x00000040
#define JL100_INTS_RX_I			0x00000080		/* for only async mode INTS regs */
#define JL100_INTS_C2MTX_I		0x00000100
#define JL100_INTS_C2MRX_I		0x00000200
#define JL100_INTS_C2ABT_I		0x00000400
#define JL100_INTS_TIM1			0x00000800
#define JL100_INTS_TXEN_I		0x00002000
#define JL100_INTS_TXST_I		0x00004000
#define JL100_INTS_SFR			0x00008000
#define JL100_INTS_P_MOD_I		0x00010000
#define JL100_INTS_DTCT_I		0x00020000
#define JL100_INTS_PAR_I		0x00040000
#define JL100_INTS_DLY_I		0x00080000
#define JL100_INTS_SCNF_I		0x00100000
#define JL100_INTS_C_START_I	0x00200000
#define JL100_INTS_A_START_I	0x00400000
#define JL100_INTS_RX_TMC		0x01000000

/* ESTS register */
#define JL100_ESTS_FCS_ERR		0x00000001
#define JL100_ESTS_SCNF_ERR		0x00000002
#define JL100_ESTS_PAR_ERR		0x00000004
#define JL100_ESTS_TMCYCOVR		0x00000008
#define JL100_ESTS_AXIS_NO_ERR	0x00000010
#define JL100_ESTS_ADDSTN_ERR	0x00000020
#define JL100_ESTS_RXLEN_ERR	0x00000100
#define JL100_ESTS_RFO_ERR		0x00000200
#define JL100_ESTS_RUR_ERR		0x00000400
#define JL100_ESTS_RXSM_ERR		0x00000800
#define JL100_ESTS_WDTOVR		0x00008000
#define JL100_ESTS_TXTIM_OVR	0x00010000
#define JL100_ESTS_RXTX_ERR		0x00020000
#define JL100_ESTS_TXRX_ERR		0x00040000
#define JL100_ESTS_RXRX_ERR		0x00080000
#define JL100_ESTS_TXSM_UNDER	0x00100000
#define JL100_ESTS_TXSM_OVER	0x00200000
#define JL100_ESTS_TFO_ERR		0x02000000
#define JL100_ESTS_TUR_ERR		0x04000000
#define JL100_ESTS_TXSM_ERR		0x08000000
#define JL100_ESTS_TX_ERROR		0x0E3F0000
#define JL100_ESTS_RX_ERROR		0x00000F01

/* MOD register */
#define JL100_MOD_TYPE_SLV		0x00000000
#define JL100_MOD_TYPE_MONSLV	0x00000001
#define JL100_MOD_TYPE_MULTISLV	0x00000001
#define JL100_MOD_TYPE_C1MST	0x00000002
#define JL100_MOD_TYPE_C2MST	0x00000003
#define JL100_MOD_INTLV_LVL		0x00000000
#define JL100_MOD_INTLV_PLS		0x00000800
#define JL100_MOD_ESYNC			0x00001000
#define JL100_MOD_MDIO_EN		0x00004000
#define JL100_MOD_INT_FR		0x00008000

/* CID register */
/* JL100A,JL101A */
#define JL100_CID				0x0000100A
#define JL100_CID_LO_ADR		0x100A
#define JL100_CID_HI_ADR		0x0000
#define JL100_ARCV_TOUT_TIME	0xFFFF
/* JL102A,JL103A */
#define JL102_CID				0x0000102A
#define JL102_CID_LO_ADR		0x102A
#define JL102_CID_HI_ADR		0x0000
#define JL102_ARCV_TOUT_TIME	0x0000

/* CRCV1 register */
#define JL100_CRCV1_00			0x00000001

#define DEF_C1_MST				0
#define DEF_C2_MST				1

#define C1MST_ADR				0x0001
#define C2MST_ADR				0x0002

#define COM_MODE_SYNC			0				/* Communication mode : SYNC */
#define COM_MODE_ASYNC			1				/* Communication mode : ASYNC */

#define WORD_SIZE				2
#define DWORD_SIZE				4

#define SYNC_FRAME_SIZE			32
#define HEADER_SIZE				8
#define ASYNC_HEADER_SIZE		12
#define FCS_SIZE				4

#define IOMAP_SIZE				16
#define USER_IOMAP_SIZE			8



/***********************************************************************/
/*      Default parameter definition                                   */
/***********************************************************************/
/* RAM top address definition */
#define DEF_DOWNLOAD_SRAM_TOP_ADR	0x4000	/* Download SRAM top address */
#define DEF_DOWNLOAD_SRAM_SIZE		0x4000	/* Download SRAM size */
#define DEF_SRAM_TOP_ADR		0x8000		/* SRAM top address */
#define DEF_SRAM_SIZE			0x8000		/* SRAM SIZE : 32Kbyte */

/* Register parameter */
#define DEF_IMK_REG				0
#define DEF_PARS_ADR			0x7b00		/* Communication parameter area top address */

/* Communication parameter */
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0E4> */
#define DEF_MA_MSK0				0xFFFF
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
#define DEF_MA_MSK0				0x00FF
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE */
#define DEF_MA_MSK1				DEF_MA_MSK0	/* default : not use multicast address */
#define DEF_SRCH_TIME			0xFFFF
#define DEF_MSG_PTR				0x5000
#define DEF_C1MSG_SZ			1536
#define DEF_C2MSG_SZ			1536
#define DEF_IFGSET				0
#define DEF_MAX_FRM				2
#define DEF_IOMAP_PTR			0x7c00

/* C1 */
#define DEF_C1_MA0				0x0001
#define DEF_C1_MA1				DEF_C1_MA0	/* default : not use multicast address */
#define DEF_C1_MA_MAX			62
#define DEF_C1_MAX_RTRY			1
#define DEF_C1_DLY_CNT			1			/* default : min */
#define DEF_TRSP_BASE_TIME		1500
#define DEF_COMPAR_RESERVE2_10NS	222
#define DEF_COMPAR_RESERVE2_100NS	20
#define DEF_COMPAR_RESERVE2_1US		0

/* C2 */
#define DEF_C2_MA0				0x0002
#define DEF_C2_MA1				DEF_C2_MA0	/* default : not use multicast address */
#define DEF_C2_MA_MAX			62
#define DEF_C2_MAX_RTRY			0
#define DEF_C2_DLY_CNT			0

/* Common */
#define DEF_PROTSEL_SYNC		0
#define DEF_PROTSEL_ASYNC		1
#define DEF_DLYFRM_WAIT_CNT		0x0023
#define DEF_DLYAPP_WAIT_CNT		0x056D

#define COM_START_TOUT			3000000		/* Async com. start reqest timeout(time unit:usec) */
#define ASYNC_REQ_POLL_TIME		500			/* Async com. start reqest Pollin time(time unit:usec) */



/***********************************************************************/
/*      Error Code definition                                          */
/***********************************************************************/
#ifndef LIB_OK
#define	LIB_OK				0
#endif
#ifndef LIB_NG
#define LIB_NG				-1
#endif

#define RCVING_MSG			0x0001
#define SNDING_MSG			0x0002
#define RCVING_FRAME		0x0001
#define SNDING_FRAME		0x0002
#define RECEIVED_FRAME		0x0003
#define ASYNC_RCV_OK		0x0001
#define RCVED_TMC_FRAME		0x0001
#define NOT_RCV_TMC_FRAME	0x0000

/* Error type definition */
#define ERROR_TYPE_USER		0x1000		/* user setting error */
#define ERROR_TYPE_COM		0x2000		/* communication error */
#define ERROR_TYPE_SYS		0x3000		/* system error */

/* Error code of initial function */
#define ERROR_INVALID_USRPAR		(ERROR_TYPE_USER | 0x00)
#define ERROR_TOUT_MODE_SET			(ERROR_TYPE_SYS  | 0x01)
#define ERROR_TOUT_USRPRM_SET		(ERROR_TYPE_SYS  | 0x02)
#define ERROR_REQ_SIZE_OVER			(ERROR_TYPE_USER | 0x04)
#define ERROR_INVALID_HIFPTR		(ERROR_TYPE_USER | 0x05)
#define ERROR_RAM_SIZE_OVER			(ERROR_TYPE_USER | 0x06)
#define ERROR_INVALID_TMCYC			(ERROR_TYPE_USER | 0x07)
#define ERROR_TOUT_CSTART			(ERROR_TYPE_SYS  | 0x08)
#define ERROR_TOUT_ASTART			(ERROR_TYPE_SYS  | 0x09)
#define ERROR_INFM_CMODE			(ERROR_TYPE_SYS  | 0x0A)
#define ERROR_WRONG_SEQ				(ERROR_TYPE_USER | 0x0B)
#define ERROR_SLV_AXIS_NO			(ERROR_TYPE_USER | 0x0C)
#define ERROR_IOMAP_SIZE			(ERROR_TYPE_USER | 0x0D)
#define ERROR_VERIFY_MICRO			(ERROR_TYPE_SYS  | 0x0E)
#define ERROR_PHY_RESET				(ERROR_TYPE_USER | 0x0F)
#define ERROR_RESET_ASIC			(ERROR_TYPE_COM  | 0x10)
#define ERROR_TX_FRM				(ERROR_TYPE_COM  | 0x11)

/* Error code of link communication function */
#define ERROR_TOUT_CHANG_RBUFF		(ERROR_TYPE_SYS  | 0x1a)
#define ERROR_TOUT_CHANG_CBUFF		(ERROR_TYPE_SYS  | 0x1c)
#define ERROR_INVALID_STNO			(ERROR_TYPE_USER | 0x1e)
#define ERROR_REQ_SIZE				(ERROR_TYPE_USER | 0x1f)
#define ERROR_INVALID_SIZE			(ERROR_TYPE_USER | 0x20)
#define ERROR_INVALID_FTYPE			(ERROR_TYPE_USER | 0x21)
#define ERROR_TOUT_RCV_FRAME		(ERROR_TYPE_SYS  | 0x22)
#define ERROR_TOUT_SEND_FRAME		(ERROR_TYPE_SYS  | 0x23)
#define ERROR_SEND_SYNCFRAM			(ERROR_TYPE_USER | 0x26)
#define ERROR_MDIO_BUSY				(ERROR_TYPE_SYS  | 0x27)
#define ERROR_NOT_SYNC				(ERROR_TYPE_SYS  | 0x28)
#define ERROR_GBL_NOT_RCV			(ERROR_TYPE_COM  | 0x29)
#define ERROR_TOUT_CMD_GTX			(ERROR_TYPE_SYS  | 0x2a)
#define ERROR_ASYNC_FTYPE			(ERROR_TYPE_COM  | 0x2b)

/* Error code of message communication function */
#define ERROR_MSG_ABORT				(ERROR_TYPE_USER | 0x30)
#define ERROR_MSG_OVERFLOW			(ERROR_TYPE_USER | 0x31)
#define ERROR_SNDING_MSG			(ERROR_TYPE_USER | 0x32)
#define ERROR_REQ_MSG_SIZE			(ERROR_TYPE_USER | 0x33)
#define ERROR_RCVING_MSG			(ERROR_TYPE_USER | 0x34)
#define ERROR_ABT_MSG				(ERROR_TYPE_SYS  | 0x35)
#define ERROR_BUSY_MSG				(ERROR_TYPE_SYS  | 0x36)
#define	ERROR_MSG_UNDERFLOW			(ERROR_TYPE_USER | 0x37)
#define ERROR_MSG_DIV_OVER			(ERROR_TYPE_USER | 0x38)



/***********************************************************************/
/*	Process code definition for processing sequence check              */
/***********************************************************************/
#define PROC_INITIAL				0x0001
#define PROC_REQ_DETECT_SLV			0x0002
#define PROC_CHK_DETECT_SLV_CMP		0x0004
#define PROC_REQ_MEASUR_TRANSDLY	0x0008
#define PROC_CHK_TRANSDLY_CMP		0x0010
#define PROC_INFM_CMODE				0x0020
#define PROC_INFM_CMODE_CMP			0x0040
#define PROC_GET_CMODE				0x0040
#define PROC_START_SYNC				0x0080



/***********************************************************************/
/*	Status number(IOMAP) definition(slave/C2master)                    */
/***********************************************************************/
#define STSNUM_PON					0x0000	/* Status number : power on */
#define STSNUM_WAIT_PARS_MOD		0x0001	/* Status number : waiting set PARS and MOD */
#define STSNUM_WAIT_PAR				0x0002	/* Status number : waiting set COM PARAM and IOMAP */
#define STSNUM_WAIT_DTCT			0x0021	/* Status number : waitnig request detect slave */
#define STSNUM_DETECTED				0x0022	/* Status number : detected slave */
#define STSNUM_AXISNUM_ERR			0x0023	/* Status number : some slave has same axis number  */
#define STSNUM_WAIT_MEASURE_DLY		0x0024	/* Status number : waiting request transdlay measure */
#define STSNUM_WAIT_DLY_APPFRM		0x0025	/* Status number : waiting measure dly appoint frame */
#define STSNUM_WAIT_DLY_MFRM		0x0026	/* Status number : waiting measure dly frame */
#define STSNUM_WAIT_TMCFRM			0x0027	/* Status number : waiting transmisson cycle frame */
#define STSNUM_WAIT_SYNC_START		0x0040	/* Status number : waiting sync start */
#define STSNUM_SYNC_COM				0x0050	/* Status number : in sync communication */
#define STSNUM_ASYNC_COM			0x0060	/* Status number : in async communication */



/***********************************************************************/
/*	Status definition				                                   */
/***********************************************************************/
#define WAIT_SETUP					1		/*  */
#define WAIT_ASIC_READY				1		/*  */
#define WAIT_CMP_DETECT				2		/*  */
#define MEASURING_TRANSDLY			3		/*  */
#define WAIT_TRANSCYC_FRM			4		/*  */
#define WAIT_SLV_RSP				5		/*  */



/***********************************************************************/
/*	Message communication status definition                            */
/***********************************************************************/
#define MSG_SND_ABORT		0x8001
#define MSG_UNDERFLOW		0x8003
#define MSG_SND_BUSY		0x8005
#define MSG_DIV_OVER		0x8007
#define MSG_ABORT			0x8002
#define MSG_OVERFLOW		0x8004
#define MSG_BUSY			0x8006



/***********************************************************************/
/*	Async. communication status definition                             */
/***********************************************************************/
#define ASYNC_RCV_CMP		0x0001
#define ASYNC_RCV_ERROR		0x0002
#define ASYNC_RCV_TIMOUT	0x0003
#define ASYNC_SND_CMP		0x0001
#define ASYNC_SND_ERROR		0x0002
#define ASYNC_SND_TIMOUT	0x0003



/***********************************************************************/
/*	Frame type definition			                                   */
/***********************************************************************/
#define FTYPE_SYNC					0x1000	/* Frame type of SYNC frame */
#define FTYPE_CR					0x2000	/* Frame type of CMD/RSP frame */
#define FTYPE_STATUS				0x6000	/* Frame type of STATUS frame */
#define FTYPE_TMC					0x7000	/* Frame type of TRANSMIT CYCLIC TIME frame */
#define FTYPE_MSG					0xC000	/* Frame type of MESSAGE frame */



/***********************************************************************/
/*	IOMAP member code definition						               */
/***********************************************************************/
#define AXIS_ADR	0x0001
#define T_RSP		0x0002
#define CD_LEN		0x0003
#define RD_LEN		0x0004
#define STS_NUM		0x0005
#define TR_DLY		0x0006



/***********************************************************************/
/*	Communication Parameter member code definition		               */
/***********************************************************************/
#define T_MCYC					1		/* COM PAR code definition : transission cycle*/
#define T_UNIT					2		/* COM PAR code definition : time unit */
#define PROT_SEL				3		/* COM PAR code definition : protcol select */
#define C2_DLY					4		/* COM PAR code definition : c2 delay time */



/***********************************************************************/
/*	T_UNIT code definition								               */
/***********************************************************************/
#define	TUNIT_10NSEC			0		/* time unit for range (t_mcyc < 500us) */
#define TUNIT_100NSEC			1		/* time unit for range (500us < t_mcyc < 4ms) */
#define TUNIT_1USEC				2		/* time unit for range (4ms < t_mcyc) */



/***********************************************************************/
/*	Structure of HOST I/F registers                                    */
/***********************************************************************/
typedef struct _JL100_HOST_IF_REGS {
	volatile ULONG cmd;			/* Command */
	volatile ULONG sts;			/* Status */
	volatile ULONG ints;		/* Interrupt Status */
	volatile ULONG imk;			/* Interrupt Mask (same as Interruput Status) */
	volatile ULONG ests;		/* Error Status */
	volatile ULONG mod;			/* Mode Setting */
	volatile ULONG cid;			/* Chip ID */
	volatile ULONG pars;		/* Communication Parameter Top Address */
	volatile ULONG sti1;		/* Connection Status 1 */
	volatile ULONG sti2;		/* Connection Status 2 */
	volatile ULONG crcv1;		/* Command Data Receive Status 1 */
	volatile ULONG crcv2;		/* Command Data Receive Status 2 */
	volatile ULONG rrcv1;		/* Response Data Receive Status 1 */
	volatile ULONG rrcv2;		/* Response Data Receive Status 2 */
	volatile ULONG cbadr;		/* Command Data Buffer Top Address */
	volatile ULONG rbadr;		/* Response Data Buffer Top Address */
	volatile ULONG gtim;		/* Global Time */
	volatile ULONG idly;		/* Interrupt Delay Time */
	volatile ULONG m3_gar;		/* MII address register for M3 */
	volatile ULONG m3_gdr;		/* MII data register for M3 */
	volatile ULONG reserved1;	/* Reserve */
	volatile ULONG reserved2;	/* Reserve */
	volatile ULONG reserved3;	/* Reserve */
	volatile ULONG reserved4;	/* Reserve */
	volatile ULONG test;		/* Test Mode */
} JL100_HOST_IF_REGS;



/***********************************************************************/
/*    Communication Parameter                                          */
/***********************************************************************/
typedef union _JL100_COM_PAR {
	struct{
		USHORT ma0;				/* My Address */
		USHORT ma_msk0;			/* Receive address filter setting for ma0*/
		USHORT ma1;				/* Multicast Address */
		USHORT ma_msk1;			/* Receive address filter setting for ma1 */
		USHORT srch_time;		/* Serch time for detecting slave */
		USHORT ma_max;			/* Max Number of Slave */
		USHORT t_mcyc;			/* Tmcyc */
		USHORT t_unit;			/* Global Time Unit */
		USHORT prot_sel;		/* Protcol Select */
		USHORT max_rtry;		/* Number of Max Retry */
		USHORT msg_ptr;			/* C1 Message buffer area top address */
		USHORT c1msg_sz;		/* C1 Message size */
		USHORT c2msg_sz;		/* C2 Message size */
		USHORT pkt_sz;			/* Sending size of one message frame */
		USHORT wdt;				/* Watch Dog Setting */
		USHORT c2_dly;			/* C2 Master Send Start Time */
		USHORT dly_cnt;			/* Number of Transmition delay time measure */
		USHORT iomap_ptr;		/* IOMAP buffer area top address  */
		USHORT ifgset;			/* Frame gap setting */
		USHORT max_frm;			/* Max Data size of PHY transfer */
		USHORT reserve1;		/*  */
		USHORT reserve2;		/*  */
		USHORT reserve3;		/*  */
		USHORT reserve4;		/*  */
		USHORT async_req_rtry;	/* Async start request retry count */
		USHORT dummy;			/*  */
	} str;
	unsigned long ulong_ptr;
} JL100_COM_PAR;



/***********************************************************************/
/*    IOMAP Struct                                                     */
/***********************************************************************/
typedef union _JL100_IO_MAP {
	struct{
		USHORT axis_adr;		/* My axis address */
		USHORT t_rsp;			/* Response monitering time */
		USHORT cd_len;			/* Command data length */
		USHORT rd_len;			/* Response data length */
		USHORT sts_num;			/* Status number */
		USHORT tr_dly;			/* Transmission delay time */
		USHORT dummy1;			/* dummy */
		USHORT dummy2;			/* dummy */
	} str;
	unsigned long ulong_ptr;
} JL100_IO_MAP;



/***********************************************************************/
/*    IOMAP Struct(for Read/Write access)                              */
/***********************************************************************/
typedef struct _JL100_IO_MAP_DWORD {
	ULONG dword1;			/* Lo:axis_adr, Hi:t_rsp  */
	ULONG dword2;			/* Lo:cd_len,   Hi:rd_len */
	ULONG dword3;			/* Lo:sts_num,  Hi:tr_dly */
	ULONG dword4;			/* Lo:dummy1,   Hi:dummy2 */
} JL100_IO_MAP_DWORD;



/***********************************************************************/
/*    USER SETTING IOMAP PARAM Struct                                  */
/***********************************************************************/
typedef struct _JL100_USER_IOMAP {
	ULONG  t_rsp;			/* Response monitering time */
	USHORT axis_adr;		/* My axis address */
	USHORT cd_len;			/* Command data length */
	USHORT rd_len;			/* Response data length */
	USHORT cmdbuff_offset;	/* Command buffer offset */
	USHORT rspbuff_offset;	/* Response buffer offset */
	USHORT dummy;			/* dummy */
} JL100_USER_IOMAP;



/***********************************************************************/
/*    User Setting Parameter                                           */
/***********************************************************************/
typedef struct _JL100_USER_PAR {
	ULONG  mod;				/* MOD register */
	USHORT ma0;				/* My Address */
	USHORT ma_max;			/* Max Number of Slaves */
	ULONG  t_mcyc;			/* Tmcyc */
	ULONG  intoffset;		/* Interrupt delay offset time */
	ULONG  c2_dly;			/* C2 Master Send Start Time */
	USHORT prot_sel;		/* Protocol Select */
	USHORT max_rtry;		/* Max Number of Retry */
	USHORT pkt_sz;			/* Sending Data size of One Message Frame */
	USHORT wdt;				/* Watch Dog Setting */
	USHORT dly_cnt;			/* Number of Transmition delay time measure */
	USHORT reserve1;		/* Reserve */
} JL100_USER_PAR;



/***********************************************************************/
/*    Register Parameter                                               */
/***********************************************************************/
typedef union _JL100_REG_PAR {
	struct{
		ULONG mod;				/* MOD register  */
		ULONG imk;				/* IMK register  */
		ULONG pars;				/* PARS register */
		ULONG idly;				/* IDLY register */
	} str;
	unsigned long ulong_ptr;
} JL100_REG_PAR;



/***********************************************************************/
/*    Global Frame Struct                                              */
/***********************************************************************/
typedef union _JL100_GBL_FRAME {
	struct{
		USHORT da;				/* Destination address */
		USHORT sa;				/* Source address */
		USHORT msg_ctrl;		/* Message control */
		USHORT ftyp_len;		/* Frame type & length */
		ULONG  time;			/* Global time */
		USHORT idly;			/* Interrupt offset time */
		USHORT reserve;			/* Reserve */
		ULONG  fcs;				/* Frame check sequence */
	} str;
	unsigned long ulong_ptr;
} JL100_GBL_FRAME;



/***********************************************************************/
/*    Header Info Struct                                               */
/***********************************************************************/
typedef union _JL100_FRAME_HEADER {
	struct{
		USHORT da;				/* Destination address */
		USHORT sa;				/* Source address */
		USHORT msg_ctrl;		/* Message control */
		USHORT ftyp_len;		/* Frame type & length */
	} str;
	unsigned long ulong_ptr;
} JL100_FRAME_HEADER;



/***********************************************************************/
/*    Header Info Struct(for async communication)                      */
/***********************************************************************/
typedef union _JL100_FRAME_HEADER_ASYNC {
	struct{
		USHORT t_rsp;			/* Transmission response monitering time */
		USHORT stat;			/* Status */
		USHORT da;				/* Destination address */
		USHORT sa;				/* Source address */
		USHORT msg_ctrl;		/* Message control */
		USHORT ftyp_len;		/* Frame type & length */
	} str;
	unsigned long ulong_ptr;
} JL100_FRAME_HEADER_ASYNC;



/***********************************************************************/
/*    Header Info Struct(for message communication)                    */
/***********************************************************************/
typedef union _JL100_FRAME_HEADER_MSG {
	struct{
		VUSHORT reserve;		/* Reserve */
		VUSHORT stat;			/* Status */
		VUSHORT da;				/* Destination address */
		VUSHORT sa;				/* Source address */
		VUSHORT msg_ctrl;		/* Message control */
		VUSHORT ftyp_len;		/* Frame type & length */
	} str;
	unsigned long ulong_ptr;
} JL100_FRAME_HEADER_MSG;



/***********************************************************************/
/*    Channel Buffer save  Parameter                                   */
/***********************************************************************/
typedef struct _CHANNEL_INFO {
	JL100_HOST_IF_REGS *hif_reg_top;	/* Host I/F register top */
	JL100_USER_IOMAP *usr_iomap;		/* User IOMAP buffer top */
	USHORT ma0;							/* My address */
	USHORT ma1;							/* Multicast address */
	USHORT ma_max;						/* Max nods */
	USHORT t_unit;						/* Time unit */
	USHORT prot_sel;					/* Communication mode */
	USHORT max_rtry;					/* Max retry */
	USHORT msg_ptr;						/* Message buffer pointer */
	USHORT pars_data;					/* Communication Param. address */
	USHORT iomap_ptr;					/* IOMAP buffer area top address */
	USHORT c1msg_sz;					/* C1 message buffer size */
	USHORT c2msg_sz;					/* C2 message buffer size */
	USHORT pkt_sz;						/* Packet size of message */
	USHORT chk_code;					/* Check code for processing sequence */
	USHORT my_st_no;					/* My station number (for monitor slave) */
	ULONG  cid;							/* ASIC chip ID */
} CHANNEL_INFO;



/***********************************************************************/
/*    Common Structure definition                                      */
/***********************************************************************/
typedef union _DWORD {
	ULONG dword;
	struct{
		USHORT lo;
		USHORT hi;
	} word;
	struct{
		UCHAR  l_lo;
		UCHAR  l_hi;
		UCHAR  h_lo;
		UCHAR  h_hi;
	} byte;
} DWORD;



/***********************************************************************/
/*    Host I/F Register Access Macro Definition                        */
/***********************************************************************/
#define jl100_get_cmd(pJL)          	(((JL100_HOST_IF_REGS *)pJL)->cmd)
#define jl100_set_cmd(pJL, data)    	(((JL100_HOST_IF_REGS *)pJL)->cmd = (ULONG)data)
#define jl100_get_sts(pJL)          	(((JL100_HOST_IF_REGS *)pJL)->sts)
#define jl100_get_ints(pJL)         	(((JL100_HOST_IF_REGS *)pJL)->ints)
#define jl100_clear_ints(pJL, data) 	(((JL100_HOST_IF_REGS *)pJL)->ints = (ULONG)data)
#define jl100_get_imk(pJL)    			(((JL100_HOST_IF_REGS *)pJL)->imk)
#define jl100_set_imk(pJL, data)    	(((JL100_HOST_IF_REGS *)pJL)->imk = (ULONG)data)
#define jl100_get_ests(pJL)         	(((JL100_HOST_IF_REGS *)pJL)->ests)
#define jl100_clear_ests(pJL, data) 	(((JL100_HOST_IF_REGS *)pJL)->ests =(ULONG)data)
#define jl100_get_mod(pJL)          	(((JL100_HOST_IF_REGS *)pJL)->mod)
#define jl100_set_mod(pJL, data)    	(((JL100_HOST_IF_REGS *)pJL)->mod = (ULONG)data)
#define jl100_get_cid(pJL)          	(((JL100_HOST_IF_REGS *)pJL)->cid)
#define jl100_get_pars(pJL)				(((JL100_HOST_IF_REGS *)pJL)->pars)
#define jl100_set_pars(pJL, data)		(((JL100_HOST_IF_REGS *)pJL)->pars = (ULONG)data)
#define jl100_get_sti1(pJL)         	(((JL100_HOST_IF_REGS *)pJL)->sti1)
#define jl100_get_sti2(pJL)         	(((JL100_HOST_IF_REGS *)pJL)->sti2)
#define jl100_get_crcv1(pJL)			(((JL100_HOST_IF_REGS *)pJL)->crcv1)
#define jl100_get_crcv2(pJL)			(((JL100_HOST_IF_REGS *)pJL)->crcv2)
#define jl100_get_rrcv1(pJL)			(((JL100_HOST_IF_REGS *)pJL)->rrcv1)
#define jl100_get_rrcv2(pJL)			(((JL100_HOST_IF_REGS *)pJL)->rrcv2)
#define jl100_get_cbadr(pJL)			((((JL100_HOST_IF_REGS *)pJL)->cbadr) << 1)	/* << 1 : Word -> Byte */
#define jl100_get_rbadr(pJL)			((((JL100_HOST_IF_REGS *)pJL)->rbadr) << 1)	/* << 1 : Word -> Byte */
#define jl100_get_gtim(pJL)				(((JL100_HOST_IF_REGS *)pJL)->gtim)
#define jl100_get_idly(pJL)				(((JL100_HOST_IF_REGS *)pJL)->idly)
#define jl100_set_idly(pJL, data)		(((JL100_HOST_IF_REGS *)pJL)->idly = (ULONG)data)
#define jl100_get_m3gar(pJL)			(((JL100_HOST_IF_REGS *)pJL)->m3_gar)
#define jl100_set_m3gar(pJL, data)		(((JL100_HOST_IF_REGS *)pJL)->m3_gar = (ULONG)data)
#define jl100_get_m3gdr(pJL)			(((JL100_HOST_IF_REGS *)pJL)->m3_gdr)
#define jl100_set_m3gdr(pJL, data)		(((JL100_HOST_IF_REGS *)pJL)->m3_gdr = (ULONG)data)
#define jl100_get_reserved1(pJL)		(((JL100_HOST_IF_REGS *)pJL)->reserved1)
#define jl100_set_reserved1(pJL, data)	(((JL100_HOST_IF_REGS *)pJL)->reserved1 = (ULONG)data)
#define jl100_get_reserved2(pJL)		(((JL100_HOST_IF_REGS *)pJL)->reserved2)
#define jl100_set_reserved2(pJL, data)	(((JL100_HOST_IF_REGS *)pJL)->reserved2 = (ULONG)data)
#define jl100_get_reserved3(pJL)		(((JL100_HOST_IF_REGS *)pJL)->reserved3)
#define jl100_set_reserved3(pJL, data)	(((JL100_HOST_IF_REGS *)pJL)->reserved3 = (ULONG)data)
#define jl100_get_reserved4(pJL)		(((JL100_HOST_IF_REGS *)pJL)->reserved4)
#define jl100_set_reserved4(pJL, data)	(((JL100_HOST_IF_REGS *)pJL)->reserved4 = (ULONG)data)
#define jl100_set_test(pJL, data)		(((JL100_HOST_IF_REGS *)pJL)->test = (ULONG)data)

#define jl100_read_dwdata(rdp)			*((ULONG *)rdp); rdp++
#define jl100_read_wdata(rdp)			*((USHORT *)rdp); rdp++
#define jl100_write_dwdata(wtp, rdp)	*((ULONG *)wtp) = *((ULONG *)rdp); ++wtp; ++rdp
#define jl100_write_wdata(wtp, rdp)		*((USHORT *)wtp) = *((USHORT *)rdp); ++wtp; ++rdp

#endif /* JL100_DEF_H_ */
