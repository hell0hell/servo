/****************************************************************************/
/*                                                                          */
/* Module : Common header of JL-076/JL-077 access interface                 */
/*                                                                          */
/****************************************************************************/
/* Defines :                                                                */
/*   Definetion of JL-076/JL-077 Interface registers                        */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* History:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*******************  2006 Yaskawa Electric Corporation *********************/
#ifndef		JL076DEF_H_
#define		JL076DEF_H_


/* WR register offset address */
#define JL076_MLSET1_ADR		0x420
#define JL076_MLSY_ADR			0x424
#define JL076_MLSET2_ADR		0x42C
#define JL076_MB_C1_ADR			0x520
#define JL076_MB_C2_ADR			0x540
#define JL076_TC_ADR			0x560
#define JL076_TM_C1_ADR			0x580
#define JL076_TM_C2_ADR			0x5A0
#define JL076_CTRL_C1_ADR		0x5C8
#define JL076_CTRL_C2_ADR		0x5CC

/* RD register offset address */
#define JL076_MLSTAT_ADR		0x428
#define JL076_RC_ADR			0x500
#define JL076_RM_C1_ADR			0x5C0
#define JL076_RM_C2_ADR			0x5C4
#define JL076_GR_ADR			0x5E0

/* mode1(JL076_MLSET1_ADR) resigster */
#define JL076_MD1_SQOFSEL		0x00000800
#define JL076_MD1_INTFREE		0x00000200
#define JL076_MD1_INTOUT		0x00000480
#define JL076_MD1_SYNCLOE		0x00000440
#define JL076_MD1_LBMOD			0x00000020
#define JL076_MD1_TXEDLY		0x00000010
#define JL076_MD1_MLTENA		0x00000008
#define JL076_MD1_MLRENA		0x00000004
#define JL076_MD1_MLTST			0x00000002
#define JL076_MD1_STU16			0x00000001

/* prm(JL076_MLSET2_ADR) resigster */
#define JL076_PRM_4MHZ			0x00000000
#define JL076_PRM_10MHZ			0x00001000
#define JL076_PRM_17BYTE		0x00000000
#define JL076_PRM_32BYTE		0x00000100

/* MLSTAT resigster */
#define JL076_MYERR_BIT			0
#define JL076_GBLERR_BIT		1
#define JL076_C1ERR_BIT			2
#define JL076_C2ERR_BIT			3

#define JL076_RECV_CODE			0xFF
#define JL076_MYERR_STS			0x1
#define JL076_GBLERR_STS		0x2
#define JL076_C1ERR_STS			0x4
#define JL076_C2ERR_STS			0x8
#define JL076_RCVERR_CLR		0xfd	/* GBLèúÇ≠ëSÇƒ */

/* JL-076 access interface macro */
#define JL076_RD_REG(pJL)			*((VUSHORT*)(pJL))
#define JL076_WR_REG(pJL, data)		*((VUSHORT*)(pJL)) = (USHORT)(data)
#define JL076_RD_DWREG(pJL)			*((VULONG*)(pJL))
#define JL076_WR_DWREG(pJL, data)	*((VULONG*)(pJL)) = (ULONG)(data)
#define JL076_RD_LNK(pJL)			*((VUSHORT*)(pJL))
#define JL076_WR_LNK(pJL, data)		*((VUSHORT*)(pJL)) = (USHORT)(data)
#define JL076_RD_DWLNK(pJL)			*((VULONG*)(pJL))
#define JL076_WR_DWLNK(pJL, data)	*((VULONG*)(pJL)) = (ULONG)(data)



#endif /* JL076DEF_H_ */
