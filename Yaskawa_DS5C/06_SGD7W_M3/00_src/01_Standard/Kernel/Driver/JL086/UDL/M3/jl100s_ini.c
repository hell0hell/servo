/************************************************************************************************/
/*																								*/
/*		JL100S_INI.C : JL-100 interface access driver											*/
/*																								*/
/************************************************************************************************/
/*																								*/
/*		Ver 1.10			:	2010.03.31	Initial coding	T.soeda								*/
/*																								*/
/************************ Copyright(C) 2007 Yaskawa Electric Corporation ************************/
/*																								*/
/*		FUNCTIONS :																				*/
/*			jl100s_init_channel()			:	Clear Chnannel buffer and save 					*/
/*													host interface top address.					*/
/*			jl100s_setup_asic()				:	Setup ASIC.										*/
/*			jl100s_chk_asic_ready()			:	Check ASIC ready.								*/
/*			jl100s_set_usr_param()			:	Set user parameter.								*/
/*			jl100s_reset_asic()				:	Reset asic.										*/
/*			jl100s_set_regpar()				:	Set register parameter 							*/
/*													to register parameter area.					*/
/*			jl100s_write_compar()			:	Write communication parameter 					*/
/*													to JL100 SRAM area.							*/
/*			jl100s_write_iomap()			:	Write iomap parameter to JL100 SRAM area.		*/
/*			jl100s_req_set_par()			:	Request set CMD(PAR_R).							*/
/*			jl100s_initialize()				:	Excute JL100 initializing.						*/
/*			jl100s_chk_detect_slv_cmp()		:	Check complete detect 							*/
/*															   connecting slave(or C2 master).	*/
/*			jl100s_req_measure_transdly()	:	Request transmit delay measurement.				*/
/*			jl100s_get_cmode()				:	Get C1 master setting communication mode .		*/
/*			jl100s_start_sync()				:	Start cyclic communication with sync mode.		*/
/*			jl100s_start_async()			:	Start cyclic communication with async mode.		*/
/*			jl100s_set_first_data()			:	Set first transmit data 						*/
/*													to toggle buffer A and B.					*/
/*			jl100s_set_link_header()		:	Set frame header data to JL100 SRAM area.		*/
/*			jl100s_read_ram()				:	Read data from RAM.								*/
/*			jl100s_write_ram()				:	Write data from RAM.							*/
/*			jl100s_activate_comprm()		:	Activate communication parameter.				*/
/*			jl100s_get_trans_cycle()		:	Get trans cycle from register.					*/
/*			jl100s_read_iomap_member()		:	Read IOMAP member.								*/
/*			jl100s_write_iomap_member()		:	Write IOMAP member.								*/
/*			jl100s_download_asicprg()		:	Download JL-100 micro code.						*/
/*			jl100s_get_rbuff_offset()		:	Get receive buffer offset of st_no station.		*/
/*			jl100s_get_cbuff_offset()		:	Get command buffer offset of st_no station.		*/
/*			jl100s_get_accdrv_ver()			:	Get access driver version.						*/
/*																								*/
/************************************************************************************************/

/* includes */
#include "jl100s_gbl.h"

/* local defines */
#define TIME_500USEC			50000	/* 500usec at time unit 10nsec */
#define TIME_4MSEC				400000	/* 4msec   at time unit 10nsec */
#define TIME_64MSEC				6400000	/* 64msec  at time unit 10nsec */

/* external */
//<S146>
//extern const	unsigned short	AsicUprg[];
//extern const	unsigned short	AsicUprgSz;

/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif
void  jl100s_init_channel( CHANNEL_INFO *chbuffp, ULONG *hif_reg_top, JL100_USER_IOMAP *usr_iomapp );
short jl100s_setup_asic(ULONG *hif_reg_top);
short jl100s_chk_asic_ready(ULONG *hif_reg_top);
short jl100s_set_usr_param(JL100_USER_PAR *usr_par, JL100_REG_PAR *reg_par, JL100_COM_PAR *com_par);
void  jl100s_reset_asic(CHANNEL_INFO *chbuffp);
short jl100s_set_regpar(CHANNEL_INFO *chbuffp, JL100_REG_PAR *reg_par);
short jl100s_write_compar(CHANNEL_INFO *chbuffp, JL100_COM_PAR *com_parp);
short jl100s_write_iomap(CHANNEL_INFO *chbuffp, JL100_USER_IOMAP *user_iomapp);
short jl100s_req_set_par(CHANNEL_INFO *chbuffp);
short jl100s_initialize(CHANNEL_INFO *chbuffp, ULONG *hif_reg_top, 
                          JL100_USER_PAR *usr_parp, JL100_USER_IOMAP *usr_iomapp);
short jl100s_chk_detect_slv_cmp(CHANNEL_INFO *chbuffp);
short jl100s_req_measure_transdly(CHANNEL_INFO *chbuffp);
short jl100s_get_cmode(CHANNEL_INFO *chbuffp);
short jl100s_start_sync(CHANNEL_INFO *chbuffp);
short jl100s_start_async(CHANNEL_INFO *chbuffp);
short  jl100s_set_first_data(CHANNEL_INFO *chbuffp);
void  jl100s_set_link_header(CHANNEL_INFO *chbuffp, ULONG adr);
short jl100s_read_ram(ULONG *hif_reg_top, USHORT ofst_adr, USHORT size, ULONG *rdata);
short jl100s_write_ram(ULONG *hif_reg_top, USHORT ofst_adr, USHORT size, ULONG wdata);
short jl100s_activate_comprm(CHANNEL_INFO *chbuffp, JL100_USER_PAR *usr_parp,
                                                    JL100_USER_IOMAP *iomapp);
void  jl100s_get_trans_cycle(CHANNEL_INFO *chbuffp, ULONG *t_mcyc);
USHORT jl100s_read_iomap_member(CHANNEL_INFO *chbuffp, USHORT st_no, USHORT member);
void jl100s_write_iomap_member(CHANNEL_INFO *chbuffp, USHORT st_no, USHORT member, USHORT data);
USHORT jl100s_read_compar_member(CHANNEL_INFO *chbuffp, USHORT member);
short jl100s_chk_iomap_prm(CHANNEL_INFO *chbuffp, JL100_USER_IOMAP *usr_iomapp);
short jl100s_download_asicprg(ULONG *hif_reg_top);
USHORT jl100s_get_rbuff_offset(CHANNEL_INFO *chbuffp, USHORT st_no);
USHORT jl100s_get_cbuff_offset(CHANNEL_INFO *chbuffp, USHORT st_no);
ULONG jl100s_get_accdrv_ver(void);
ULONG jl100s_swaphl(ULONG i);
#ifdef __cplusplus
}
#endif

/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_init_channel()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Clear Chnannel buffer and save host interface top address.								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	*hif_reg_top	:	Host I/F register top address								*/
/*																								*/
/*	Return Code :																				*/
/*		Non																						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_init_channel( CHANNEL_INFO *chbuffp, ULONG *hif_reg_top, JL100_USER_IOMAP *usr_iomapp)
{
	USHORT	*wtp;		/* for channel buffer access */
	USHORT	size, i;	/* Channel buffer size */
	
	/* Clear channel buffer */
	wtp = (USHORT *)chbuffp;
	size = sizeof(CHANNEL_INFO) / sizeof(USHORT);
	for (i = 0; i < size; i++)
	{
		*wtp = 0;
		wtp++;
	}

	/* Set top address of Host I/F register */
	chbuffp->hif_reg_top = (JL100_HOST_IF_REGS *)hif_reg_top;
	chbuffp->usr_iomap = usr_iomapp;
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_setup_asic()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Setup ASIC.																				*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Nomal end.													*/
/*		WAIT_SETUP				:	Waiting ASIC setup cmplete.									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_setup_asic(ULONG *hif_reg_top)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	USHORT	 cid[2], ret;				/* Chip ID */
	
	/* Get Host I/F register top address */
	hirp = (JL100_HOST_IF_REGS *)hif_reg_top;


#if 0 /* 2013.07.14 Y.Oka JL-086ÇÕM-IIIÇÃChip IDÇ™Ç»Ç¢ */
	  /* Chip IDÇ…ë„ÇÌÇÈèàóùÇåüì¢Ç∑ÇÈÇ±Ç∆ (>_<) */
	/* Get Chip ID */
	*(ULONG*)(&cid) = (ULONG)jl100_get_cid(hirp);

	/* Check chip ID */
	if(*(ULONG*)(&cid) == JL100_CID)
	{
		if(cid[0] != JL100_CID_LO_ADR)
		{
			return(WAIT_SETUP);	/* ASIC not ready */
		}
		if(cid[1] != JL100_CID_HI_ADR)
		{
			return(WAIT_SETUP);	/* ASIC not ready */
		}
	}
	else if(*(ULONG*)(&cid) == JL102_CID)
	{
		if(cid[0] != JL102_CID_LO_ADR)
		{
			return(WAIT_SETUP);	/* ASIC not ready */
		}
		if(cid[1] != JL102_CID_HI_ADR)
		{
			return(WAIT_SETUP);	/* ASIC not ready */
		}
	}
	else
	{
		return(WAIT_SETUP);	/* ASIC not ready */
	}
#endif

#if CSW_BOARD_TYPE == SGD7W_TYPE /* <S0A2> */
	/* Download micro code */
	ret = jl100s_download_asicprg(hif_reg_top);
#endif /* CSW_BOARD_TYPE == SGD7W_TYPE */

	ret = LIB_OK;

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_chk_asic_ready()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Check ASIC ready.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Nomal end.													*/
/*		WAIT_ASIC_READY			:	Waiting ASIC ready.											*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_chk_asic_ready(ULONG *hif_reg_top)
{
	volatile JL100_HOST_IF_REGS *hirp;		/* Host I/F Top address */
	USHORT	 cid[2], sts;					/* Chip ID */

	/* Get Host I/F register top address */
	hirp = (JL100_HOST_IF_REGS *)hif_reg_top;

#if 0 /* 2013.07.14 Y.Oka JL-086ÇÕM-IIIÇÃChip IDÇ™Ç»Ç¢ */
	  /* Chip IDÇ…ë„ÇÌÇÈèàóùÇåüì¢Ç∑ÇÈÇ±Ç∆ (>_<) */
	/* Get Chip ID */
	*(ULONG*)(&cid) = (ULONG)jl100_get_cid(hirp);

	/* Check chip ID */
	if(*(ULONG*)(&cid) == JL100_CID)
	{
		if(cid[0] != JL100_CID_LO_ADR)
		{
			return(WAIT_ASIC_READY);	/* ASIC not ready */
		}
		if(cid[1] != JL100_CID_HI_ADR)
		{
			return(WAIT_ASIC_READY);	/* ASIC not ready */
		}
	}
	else if(*(ULONG*)(&cid) == JL102_CID)
	{
		if(cid[0] != JL102_CID_LO_ADR)
		{
			return(WAIT_ASIC_READY);	/* ASIC not ready */
		}
		if(cid[1] != JL102_CID_HI_ADR)
		{
			return(WAIT_ASIC_READY);	/* ASIC not ready */
		}
	}
	else
	{
		return(WAIT_ASIC_READY);	/* ASIC not ready */
	}
#endif

	/* Check STATUS.RDY */
	sts = (USHORT)jl100_get_sts(hirp);
	if((sts & JL100_STS_RDY) != 0)
	{
		return(LIB_OK);
	}
	else
	{
		return(WAIT_ASIC_READY);	/* ASIC not ready */
	}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_set_usr_param()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Set user parameter.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		JL100_USER_PAR *usr_par	:	JL100 user parameter										*/
/*		JL100_REG_PAR *reg_par	:	JL100 register parameter									*/
/*		JL100_COM_PAR *com_par	:	JL100 communication parameter								*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_RAM_SIZE_OVER		:	Using memory size larger than RAM.							*/
/*		ERROR_INVALID_TMCYC		:	Invalid transmite cycle.									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_set_usr_param(JL100_USER_PAR *usr_par, 
                           JL100_REG_PAR *reg_par, JL100_COM_PAR *com_par)
{
	USHORT buf;
	ULONG  *wtp, size, i;

	/* Initialize value */
	wtp = &com_par->ulong_ptr;
	size = sizeof(JL100_COM_PAR) / sizeof(ULONG);
	for (i = 0; i < size; i++)
	{
		*wtp = 0;
		wtp++;
	}
	
	wtp = &reg_par->ulong_ptr;
	size = sizeof(JL100_REG_PAR) / sizeof(ULONG);
	for (i = 0; i < size; i++)
	{
		*wtp = 0;
		wtp++;
	}

	/* Set register parameter of user setting "usr_par -> reg_par" */
	reg_par->str.mod = usr_par->mod;
	reg_par->str.imk = DEF_IMK_REG;
	reg_par->str.pars = DEF_PARS_ADR;

	/* Set user setting communication parameter "usr_par -> com_par" */
	com_par->str.ma0 = usr_par->ma0;
	com_par->str.pkt_sz = usr_par->pkt_sz;
	com_par->str.wdt = (16384 - usr_par->wdt);	/* WDT works : (16384 - (user setting)) * 8us */

	/* Set default setting communication parameter */
	com_par->str.ma_msk0 = DEF_MA_MSK0;
	com_par->str.ma1 = usr_par->ma0;
	com_par->str.ma_msk1 = DEF_MA_MSK1;
	com_par->str.prot_sel = 0;
	com_par->str.ifgset = DEF_IFGSET;
	com_par->str.max_frm = DEF_MAX_FRM;
	com_par->str.ma_max = 0;
	com_par->str.max_rtry = 0;
	com_par->str.c2_dly = 0;
	com_par->str.dly_cnt = 0;
	com_par->str.srch_time = DEF_SRCH_TIME;
	com_par->str.reserve1 = 2;	/* Wait count */
	
	if((usr_par->mod & JL100_MOD_TYPE_MONSLV) != 0)
	{
		/* Monitor slave */
		com_par->str.ma_max = usr_par->ma_max;
	}

	/* Check message size */
	buf = (USHORT)(DEF_MSG_PTR + DEF_C1MSG_SZ * 2 + DEF_C2MSG_SZ * 2);
	if((reg_par->str.pars) < buf)
	{
		return(ERROR_RAM_SIZE_OVER);
	}

	buf = DEF_IOMAP_PTR + IOMAP_SIZE * 64;
	if(DEF_SRAM_SIZE < buf)
	{
		return(ERROR_RAM_SIZE_OVER);
	}

	/* Set pointer address and message size */
	com_par->str.msg_ptr 	= DEF_MSG_PTR;
	com_par->str.iomap_ptr 	= DEF_IOMAP_PTR;
	com_par->str.c1msg_sz 	= DEF_C1MSG_SZ;
	com_par->str.c2msg_sz 	= DEF_C2MSG_SZ;

	return(LIB_OK);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_reset_asic()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Reset asic.																				*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		Non																						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_reset_asic(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Request reset asic */
	jl100_set_cmd(hirp, JL100_CMD_RST_ASC);
	
	while((jl100_get_cmd(hirp) & JL100_CMD_RST_ASC) != 0){ ;}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_set_regpar()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Set register parameter to register parameter area.										*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		JL100_REG_PAR *reg_par	:	JL100 register parameter									*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_TOUT_MODE_SET		:	MODE regs. set request did not completed					*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_set_regpar(CHANNEL_INFO *chbuffp, JL100_REG_PAR *reg_par)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	i;							/* Polling Counter */
	ULONG	data;						/* Register read data */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Save PARS pointer data */
	chbuffp->pars_data = (USHORT)reg_par->str.pars + DEF_SRAM_TOP_ADR;

	/* Write data to register area */
	jl100_set_imk(hirp, reg_par->str.imk);
	jl100_set_mod(hirp, reg_par->str.mod);
	jl100_set_pars(hirp, reg_par->str.pars);

	/* Request set CMD(P_MOD) */
	jl100_set_cmd(hirp, JL100_CMD_P_MOD_R);

	/* Wait p_mod complete */
	ret = ERROR_TOUT_MODE_SET;
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		data = jl100_get_ints(hirp);
		if((data & JL100_INTS_P_MOD_I) != 0)
		{
			ret = LIB_OK;
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, JL100_INTS_P_MOD_I);
			break;
		}
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_write_compar()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Write communication parameter to JL100 SRAM area.										*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		JL100_COM_PAR *com_par	:	JL100 communication parameter								*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_write_compar(CHANNEL_INFO *chbuffp, JL100_COM_PAR *com_parp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	*wtp, *rdp;					/* for RAM access */
	USHORT	compar_sz;					/* JL100_COM_PAR size */
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Save communication parameter to channel buffer */
	chbuffp->ma0 = com_parp->str.ma0;
	chbuffp->ma1 = com_parp->str.ma1;
	chbuffp->ma_max = com_parp->str.ma_max;
	chbuffp->max_rtry = com_parp->str.max_rtry;
	chbuffp->msg_ptr = com_parp->str.msg_ptr + DEF_SRAM_TOP_ADR;
	chbuffp->iomap_ptr = com_parp->str.iomap_ptr + DEF_SRAM_TOP_ADR;
	chbuffp->c1msg_sz = com_parp->str.c1msg_sz;
	chbuffp->c2msg_sz = com_parp->str.c2msg_sz;
	chbuffp->pkt_sz = com_parp->str.pkt_sz;

	/* Write communication parameter */
	rdp = &com_parp->ulong_ptr;
	wtp = (ULONG *)((ULONG)hirp + chbuffp->pars_data);
	compar_sz = sizeof(JL100_COM_PAR);
	while(compar_sz > 0)
	{
		jl100_write_dwdata(wtp, rdp);
		compar_sz -= DWORD_SIZE;
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_write_iomap()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Write iomap parameter to JL100 SRAM area.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp		:	Channel buffer address									*/
/*		JL100_USER_IOMAP *iomapp	:	JL100 iomap parameter(user setting)						*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK						:	Normal end												*/
/*		ERROR_IOMAP_SIZE			:	Invalid command data length or response data length.	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_write_iomap(CHANNEL_INFO *chbuffp, JL100_USER_IOMAP *usr_iomapp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	volatile JL100_IO_MAP *iomapp;		/* IOMAP top address */
	volatile JL100_IO_MAP iomap_work;	/* Work */
	volatile ULONG	*wtp, *rdp;			/* for RAM access */
	USHORT	usrio_sz, i;				/* JL100_USER_IOMAP size */
	short	ret;						/* Return code */

	/* Initialize value */
//	ret = LIB_OK;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get IOMAP area top address */
	iomapp = (volatile JL100_IO_MAP *)((ULONG)hirp + chbuffp->iomap_ptr);

	/* User setting IOMAP data check */
	ret = jl100s_chk_iomap_prm(chbuffp, usr_iomapp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Write iomap data to #0 - #ma_max area */
	wtp = &iomapp->ulong_ptr;

/*	SGD7SÇ≈ÇÕÅAma_maxÇ™0Ç∆Ç»ÇÈ	*/
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
	for(i = 0; i <= (chbuffp->ma_max + 1); i++)
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
	for(i = 0; i <= (chbuffp->ma_max); i++)
#endif /* f CSW_BOARD_TYPE == SGD7S_TYPE */
	{
		
		rdp = &iomap_work.ulong_ptr;
		iomap_work.str.axis_adr = (usr_iomapp + i)->axis_adr;
		iomap_work.str.cd_len = (usr_iomapp + i)->cd_len;
		iomap_work.str.rd_len = (usr_iomapp + i)->rd_len;
		iomap_work.str.t_rsp = 0;	/* slave */

		usrio_sz = USER_IOMAP_SIZE;
		while(usrio_sz > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			usrio_sz -= DWORD_SIZE;
		}
		wtp += (sizeof(JL100_IO_MAP) - USER_IOMAP_SIZE)/sizeof(ULONG);

		/* Save CMD/RSP buffer offset */
		(usr_iomapp + i)->cmdbuff_offset = jl100s_get_cbuff_offset(chbuffp, i);
		(usr_iomapp + i)->rspbuff_offset = jl100s_get_rbuff_offset(chbuffp, i);
	}

	if(chbuffp->ma_max != 0)	/* Monitor slave */
	{
		/* Write C2 master iomap data to #63 area */
		wtp = &iomapp->ulong_ptr;
		wtp += 0x00FC;
		*wtp = C2MST_ADR;
		wtp++;
		*wtp = 0;	/* command length = 0, response length = 0 */
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_req_set_par()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request set CMD(PAR_R).																	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_TOUT_USRPRM_SET	:	User param set request did not completed					*/
/*		ERROR_INVALID_USRPAR	:	Invalid user setting parameter.								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_req_set_par(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	data;						/* Register read data */
	ULONG	i;							/* Polling counter */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Request set par_r of CMD register */
	jl100_set_cmd(hirp, JL100_CMD_PAR_R);

	/* Wait par_r complete */
	ret = ERROR_TOUT_USRPRM_SET;
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		data = jl100_get_ints(hirp);
		if((data & JL100_INTS_PAR_I) != 0)
		{
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, JL100_INTS_PAR_I);

			/* Check error status */
			data = jl100_get_ests(hirp);
			if((data & JL100_ESTS_PAR_ERR) != 0)
			{
				ret = ERROR_INVALID_USRPAR;
				/* Clear error status */
				jl100_clear_ests(hirp, JL100_ESTS_PAR_ERR);
				break;
			}
			else
			{
				ret = LIB_OK;
			}
			break;
		}
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_initialize()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Excute JL100 initializing.																*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp			:	Channel buffer address								*/
/*		ULONG *hif_reg_top				:	Host I/F register top address						*/
/*		JL100_USER_PAR *usr_parp		:	JL100 user parameter								*/
/*		JL100_USER_IOMAP *usr_iomapp	:	JL100 IOMAP parameter(user setting)					*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK							:	Normal end											*/
/*		ERROR_INVALID_HIFPTR			:	Invalid host I/F register address setting			*/
/*		ERROR_INVALID_USRPAR			:	Invalid user setting parameter.						*/
/*		ERROR_RAM_SIZE_OVER				:	Using memory size larger than RAM.					*/
/*		ERROR_TOUT_MODE_SET				:	MODE regs. set request did not completed			*/
/*		ERROR_TOUT_USRPRM_SET			:	User param set request did not completed			*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_initialize( CHANNEL_INFO *chbuffp, ULONG *hif_reg_top, 
                          JL100_USER_PAR *usr_parp, JL100_USER_IOMAP *usr_iomapp )
{
	JL100_COM_PAR	com_par;		/* Communication parameter */
	JL100_REG_PAR	reg_par;		/* Register parameter */
	short	ret;					/* Return code */

	/* Check HOST I/F regs. top address */
	if((ULONG)hif_reg_top > 0xffff0000)
	{
		return(ERROR_INVALID_HIFPTR);
	}

	/* Clear channel buffer, and set top address of Host I/F register */
	jl100s_init_channel(chbuffp, hif_reg_top, usr_iomapp);

	/* Get Chip ID */
	chbuffp->cid = (ULONG)jl100_get_cid(chbuffp->hif_reg_top);

	/* Set user parameter & default parameter */
	ret = jl100s_set_usr_param(usr_parp, &reg_par, &com_par);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Set register parameter */
	ret = jl100s_set_regpar(chbuffp, &reg_par);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Write communication parameter */
	ret = jl100s_write_compar(chbuffp, &com_par);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Write IOMAP parameter */
	ret = jl100s_write_iomap(chbuffp, usr_iomapp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Request set Communication parameter and IOMAP */
	ret = jl100s_req_set_par(chbuffp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Request detect connecting slave */
	jl100_set_cmd(chbuffp->hif_reg_top, JL100_CMD_DTCT_R);

	/* Set check code of this process */
	chbuffp->chk_code = PROC_INITIAL;

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_chk_detect_slv_cmp()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Check detecting connect slave(or C2 master) complete.									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Complete detect slave.										*/
/*		WAIT_CMP_DETECT			:	Waiting complete detect slave.								*/
/*		ERROR_WRONG_SEQ			:	Wrong process sequence.										*/
/*		ERROR_SLV_AXIS_NO		:	Some slave has same station address.						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_chk_detect_slv_cmp(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;		/* Host I/F top address */
	USHORT	 need_proc;						/* for process sequence check */
	ULONG    data;							/* for register read */
	short    ret;							/* Return code */

	/* Initialize value */
//	ret = LIB_OK;

	/* Check processing sequence */
	need_proc = PROC_INITIAL;
	if(chbuffp->chk_code != need_proc)
	{
		return(ERROR_WRONG_SEQ);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Check detect connecting slave complete */
	data = jl100_get_ints(hirp);
	if((data & JL100_INTS_DTCT_I) != 0)
	{
		/* Clear interrupt factor */
		jl100_clear_ints(hirp, JL100_INTS_DTCT_I);
		
		/* Check error status */
		data = jl100_get_ests(hirp);
		if((data & JL100_ESTS_AXIS_NO_ERR) != 0)
		{
			ret = ERROR_SLV_AXIS_NO;
			/* Clear error status */
			jl100_clear_ests(hirp, JL100_ESTS_AXIS_NO_ERR);
		}
		else
		{
			ret = LIB_OK;
		}
		/* Set check code of this process */
		chbuffp->chk_code |= PROC_CHK_DETECT_SLV_CMP;
	}
	else
	{
		ret = WAIT_CMP_DETECT;
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_req_measure_transdly()															*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request transmit delay measurement.														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_WRONG_SEQ			:	Wrong process sequence										*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_req_measure_transdly(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	USHORT	need_proc;					/* for process sequence check */
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Check processing sequence */
	need_proc = PROC_INITIAL | PROC_CHK_DETECT_SLV_CMP;
	if(chbuffp->chk_code != need_proc)
	{
		return(ERROR_WRONG_SEQ);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Request measure trans delay */
	jl100_set_cmd(hirp, JL100_CMD_DLY_R);

	/* Set check code of this process */
	chbuffp->chk_code |= PROC_REQ_MEASUR_TRANSDLY;

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_cmode()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get C1 master setting communication mode.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end.													*/
/*		MEASURING_TRANSDLY		:	Measuring transmission delay time.							*/
/*		WAIT_TRANSCYC_FRM		:	Waiting receive TMC frame.									*/
/*		ERROR_WRONG_SEQ			:	Wrong process sequence.										*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_get_cmode(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG 	ints;						/* for register read */
	USHORT	need_proc, needless_proc;	/* for proccess sequence check */
	short	ret;						/* Return code */

	/* Check processing sequence */
	need_proc = PROC_INITIAL | PROC_CHK_DETECT_SLV_CMP | PROC_REQ_MEASUR_TRANSDLY;
	if((chbuffp->chk_code & need_proc) != need_proc)
	{
		return(ERROR_WRONG_SEQ);
	}
	needless_proc = (USHORT)(~(need_proc | PROC_GET_CMODE | PROC_CHK_TRANSDLY_CMP));
	if((chbuffp->chk_code & needless_proc) != 0)
	{
		return(ERROR_WRONG_SEQ);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get INTS register */
	ints = jl100_get_ints(hirp);

	/* Check complete measure transmission delay time */
	if((chbuffp->chk_code & PROC_CHK_TRANSDLY_CMP) !=0)
	{
		/* Check INTS(RX_TMC) */
		if((ints & JL100_INTS_RX_TMC) != 0)
		{
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, JL100_INTS_RX_TMC);
			
			/* Clear error factor */
			jl100_clear_ests(hirp, (JL100_ESTS_RXTX_ERR | JL100_ESTS_TXRX_ERR | JL100_ESTS_RXRX_ERR));

			/* Get communication mode setting */ 
			chbuffp->prot_sel = jl100s_read_compar_member(chbuffp, PROT_SEL);

			/* Set check code of this process */
			chbuffp->chk_code |= PROC_GET_CMODE;
			chbuffp->chk_code &= ~(PROC_REQ_MEASUR_TRANSDLY | PROC_CHK_TRANSDLY_CMP);

			ret = LIB_OK;
		}
		else
		{
			return(WAIT_TRANSCYC_FRM);
		}
	}
	else
	{
		/* Check INTS(DLY_I) */
		if((ints & JL100_INTS_DLY_I) != 0)
		{
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, JL100_INTS_DLY_I);

			/* Set check code of transmission delay measurement complete */
			chbuffp->chk_code |= PROC_CHK_TRANSDLY_CMP;

			ret = WAIT_TRANSCYC_FRM;
		}
		else
		{
			return(MEASURING_TRANSDLY);
		}
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_start_sync()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Start cyclic communication with sync mode.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_WRONG_SEQ			:	Wrong process sequence.										*/
/*		ERROR_CHANG_CBUFF		:	Couldn't change command buffer.								*/
/*		ERROR_TOUT_CHANG_CBUFF	:	Request change command buffer did not complete.				*/
/*		ERROR_TOUT_CSTART		:	Request start sync com. is not complete.					*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_start_sync(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	i;							/* Polling counter */
	ULONG	data;						/* Register read data */
	USHORT	need_proc;					/* for process sequence check */
	short	ret;						/* Return code */
	
	/* Initialize value */
//	ret = LIB_OK;

	/* Check processing sequence */
	need_proc = PROC_INITIAL | PROC_CHK_DETECT_SLV_CMP | PROC_GET_CMODE;
	if((chbuffp->chk_code & need_proc) != need_proc)
	{
		return(ERROR_WRONG_SEQ);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Mask interrupt factor (INT0) */
	jl100_set_imk(hirp, 0xFFFFFFFF);

	/* Set sync frame and first transmit data */
	ret = jl100s_set_first_data(chbuffp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Save time unit */
	chbuffp->t_unit = jl100s_read_compar_member(chbuffp, T_UNIT);

	/* Request start sync mode communication */
	jl100_set_cmd(hirp, JL100_CMD_C_START_R);

	/* Wait sync mode communication start interrupt */
	ret = ERROR_TOUT_CSTART;
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		data = jl100_get_ints(hirp);
		if((data & JL100_INTS_C_START_I) != 0)
		{
			ret = LIB_OK;
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, JL100_INTS_C_START_I);
			break;
		}
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_start_async()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Start cyclic communication with async mode.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_WRONG_SEQ			:	Wrong process sequence.										*/
/*		ERROR_TOUT_ASTART		:	Start async com request did not completed.					*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_start_async(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	i;							/* Polling counter */
	ULONG	data;						/* Register read data */
	ULONG	rbadr;						/* Responce buffer address */
	ULONG	*wtp;						/* for RAM access */
	ULONG	work;
	USHORT	need_proc;					/* for process sequence check */
	short	ret;						/* Return code */

	/* Check processing sequence */
	need_proc = PROC_INITIAL | PROC_CHK_DETECT_SLV_CMP | PROC_GET_CMODE;
	if((chbuffp->chk_code & need_proc) != need_proc)
	{
		return(ERROR_WRONG_SEQ);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Mask interrupt factor except RX_I (INT0) */
	jl100_set_imk(hirp, 0xFFFFFF7F);

	/* Set Timeout time */
	rbadr = (ULONG)((ULONG)hirp + DEF_SRAM_TOP_ADR);
	rbadr += (chbuffp->pars_data - DEF_SRAM_TOP_ADR) >> 1;
	work = 0xffff;
	wtp = (ULONG *)rbadr;
	*wtp |= work;

	/* Request start async mode communication */
	jl100_set_cmd(hirp, (JL100_CMD_A_START_R | JL100_CMD_RX_R));

	/* Wait sync mode communication start interrupt */
	ret = ERROR_TOUT_ASTART;
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		data = jl100_get_ints(hirp);
		if((data & JL100_INTS_A_START_I) != 0)
		{
			ret = LIB_OK;
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, JL100_INTS_A_START_I);
			break;
		}
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_set_first_data()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Set first transmit data to toggle buffer A and B.										*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_CHANG_CBUFF		:	Couldn't change command buffer.								*/
/*		ERROR_TOUT_CHANG_CBUFF	:	Request change command buffer did not complete.				*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_set_first_data(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	rbadr;						/* Command buffer address */
	short	ret;

	/* Initialize value */
//	ret = LIB_OK;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get command buffer offset address */
	rbadr = (ULONG)(chbuffp->hif_reg_top) + DEF_SRAM_TOP_ADR;
	rbadr += (USHORT)jl100_get_rbadr(hirp);

	/* Set link header */
	jl100s_set_link_header(chbuffp, rbadr);

	/* Change command buffer A to B */
	ret = jl100s_change_rbuff(chbuffp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Get command buffer offset address */
	rbadr = (ULONG)(chbuffp->hif_reg_top) + DEF_SRAM_TOP_ADR;
	rbadr += (USHORT)jl100_get_rbadr(hirp);

	/* Set link header */
	jl100s_set_link_header(chbuffp, rbadr);
	
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_set_link_header()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Set frame header data to JL100 SRAM area.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		ULONG adr				:	Cyclic frame start address									*/
/*																								*/
/*	Return Code :																				*/
/*		Non																						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_set_link_header(CHANNEL_INFO *chbuffp, ULONG adr)
{
	JL100_FRAME_HEADER frm_header;		/* frame header struct  */
	ULONG	*wtp, *rdp;					/* for RAM access */
	USHORT	size, my_st_no;
	USHORT	i;

	/* Get my station number */
/* SGD7SÇ≈ÇÕÅAma_maxÇ™0Ç∆Ç»ÇÈ */
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
	for(i = 1; i <= (chbuffp->ma_max + 1); i++)
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
	for(i = 1; i <= (chbuffp->ma_max); i++)
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE */
	{
		if(((jl100s_read_iomap_member(chbuffp, i, AXIS_ADR)) & 0x00FF) == chbuffp->ma0)
		{
			my_st_no = i;
			
			/* Set first command data */
			/* Set frame header data */
			frm_header.str.da = 0x0001;
			frm_header.str.sa = jl100s_read_iomap_member(chbuffp, my_st_no, AXIS_ADR);
			frm_header.str.msg_ctrl = 0x0000;
			frm_header.str.ftyp_len = FTYPE_CR | jl100s_read_iomap_member(chbuffp, my_st_no, RD_LEN);

			/* Calculate top address of C/R frame buffer */
			wtp = (ULONG *)(adr +  jl100s_get_rbuff_offset(chbuffp, my_st_no));

			/* Write frame header data to SRAM area */
			rdp = &frm_header.ulong_ptr;
			size = HEADER_SIZE;
			while(size > 0)
			{
				jl100_write_dwdata(wtp, rdp);
				size -= DWORD_SIZE;
			}

			/* Set FCS data */
			wtp += jl100s_read_iomap_member(chbuffp, my_st_no, RD_LEN)>>2;
			*wtp = 0x00000000;
		}
	}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_ram()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read data from RAM.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		ULONG	*hif_reg_top	:	Host I/F register top address								*/
/*		USHORT	ofst_adr		:	Offset address												*/
/*		USHORT	size			:	Read data size												*/
/*		ULONG	*rdata			:	Read data save buffer										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_SIZE		:	Invalid request size.										*/
/*		ERROR_REQ_SIZE_OVER		:	Request size is over RAM area.								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_read_ram(ULONG *hif_reg_top, USHORT ofst_adr, USHORT size, ULONG *rdata)
{
	ULONG	*rdp;				/* for RAM access */
	ULONG	*sram_top_adr;		/* SRAM area top address */

	/* Check request offset and size */
	if((ofst_adr % DWORD_SIZE != 0) || (size % DWORD_SIZE != 0))
	{
		return(ERROR_INVALID_SIZE);
	}
	if((ofst_adr + size) > DEF_SRAM_SIZE)
	{
		return(ERROR_REQ_SIZE_OVER);
	}

	/* Calcurate SRAM area top address */
	sram_top_adr = (ULONG *)((ULONG)hif_reg_top + DEF_SRAM_TOP_ADR);

	/* Read data from SRAM */
	rdp = (ULONG *)((ULONG)sram_top_adr + ofst_adr);
	while(size > 0)
	{
		*rdata = jl100_read_dwdata(rdp);
		rdata++;
		size -= DWORD_SIZE;
	}
	return(LIB_OK);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_write_ram()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Write data from RAM.																	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		ULONG	*hif_reg_top	:	Host I/F register top address								*/
/*		USHORT	ofst_adr		:	Offset address												*/
/*		USHORT	size			:	Read data size												*/
/*		ULONG	wdata			:	Write data													*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_SIZE		:	Invalid request size.										*/
/*		ERROR_REQ_SIZE_OVER		:	Request size is over RAM area.								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_write_ram(ULONG *hif_reg_top, USHORT ofst_adr, USHORT size, ULONG wdata)
{
	ULONG	*wtp;			/* for RAM access */
	ULONG	*sram_top_adr;	/* SRAM area top address */

	/* Check request offset and size */
	if((ofst_adr % DWORD_SIZE != 0) || (size % DWORD_SIZE != 0))
	{
		return(ERROR_INVALID_SIZE);
	}
	if((ofst_adr + size) > DEF_SRAM_SIZE)
	{
		return(ERROR_REQ_SIZE_OVER);
	}

	/* Calcurate RAM area top address */
	sram_top_adr = (ULONG *)((ULONG)hif_reg_top + DEF_SRAM_TOP_ADR);

	/* Write data to RAM */
	wtp = (ULONG *)((ULONG)sram_top_adr + ofst_adr);
	while(size > 0)
	{
		*wtp = wdata;
		wtp++;
		size -= DWORD_SIZE;
	}
	return(LIB_OK);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_activate_comprm()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Activate communication parameter.														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp			:	Channel buffer address								*/
/*		JL100_USER_PAR *usr_parp		:	JL100 user parameter								*/
/*		JL100_USER_IOMAP *usr_iomapp	:	JL100 IOMAP parameter(user setting)					*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK							:	Normal end											*/
/*		ERROR_RAM_SIZE_OVER				:	Using memory size larger than RAM.					*/
/*		ERROR_TOUT_USRPRM_SET			:	User param set request did not completed.			*/
/*		ERROR_INVALID_USRPAR			:	Invalid user setting parameter.						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_activate_comprm(CHANNEL_INFO *chbuffp, JL100_USER_PAR *usr_parp,
                                                    JL100_USER_IOMAP *usr_iomapp)
{
	JL100_COM_PAR com_par;				/* Communication parameter */
	JL100_REG_PAR reg_par;				/* Register parameter */
	short	ret;						/* Return code */

	/* Set user parameter & default parameter */
	ret = jl100s_set_usr_param(usr_parp, &reg_par, &com_par);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Write communication parameter */
	ret = jl100s_write_compar(chbuffp, &com_par);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Write IOMAP parameter */
	ret = jl100s_write_iomap(chbuffp, usr_iomapp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Request set Communication parameter and IOMAP */
	ret = jl100s_req_set_par(chbuffp);
	if(ret != LIB_OK)
	{
		return(ret);
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_trans_cycle()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get trans cycle from register.															*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp			:	Channel buffer address								*/
/*		ULONG *t_mcyc					:	Transmission cycle save buffer pointer				*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK							:	Normal end											*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_get_trans_cycle(CHANNEL_INFO *chbuffp, ULONG *t_mcyc)
{
	USHORT	t_unit_work, t_mcyc_work;	/* Work */

	/* Get t_unit and t_mcyc from Communication parameter area */
	t_unit_work = jl100s_read_compar_member(chbuffp, T_UNIT);
	t_mcyc_work = jl100s_read_compar_member(chbuffp, T_MCYC);
	t_mcyc_work += 1;

	if(t_unit_work == TUNIT_10NSEC)
	{
		*t_mcyc = t_mcyc_work;
	}
	else if(t_unit_work == TUNIT_100NSEC)
	{
		*t_mcyc = t_mcyc_work * 10;
	}
	else
	{
		*t_mcyc = t_mcyc_work * 100;
	}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_iomap_member()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read IOMAP member.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number												*/
/*		USHORT	member			:	IOMAP member to be read										*/
/*																								*/
/*	Return Code :																				*/
/*		IOMAP member																			*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
USHORT jl100s_read_iomap_member(CHANNEL_INFO *chbuffp, USHORT st_no, USHORT member)
{
	volatile JL100_HOST_IF_REGS *hirp;			/* Host I/F Top address */
	volatile JL100_IO_MAP_DWORD *iomapp;		/* IOMAP top address */
	DWORD	work;								/* Work */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get IOMAP area top address */
	iomapp = (volatile JL100_IO_MAP_DWORD *)((ULONG)hirp + chbuffp->iomap_ptr);

	switch(member)
	{
		case AXIS_ADR	:	/* Read axis address */
			work.dword = (iomapp + st_no)->dword1;
			return(work.word.lo);

		case T_RSP		:	/* Read transmit response */
			work.dword = (iomapp + st_no)->dword1;
			return(work.word.hi);

		case CD_LEN		:	/* Read command data length */
			work.dword = (iomapp + st_no)->dword2;
			return(work.word.lo);

		case RD_LEN		:	/* Read response data length */
			work.dword = (iomapp + st_no)->dword2;
			return(work.word.hi);

		case STS_NUM	:	/* Read status number */
			work.dword = (iomapp + st_no)->dword3;
			return(work.word.lo);

		case TR_DLY		:	/* Read transmit delay time */
			work.dword = (iomapp + st_no)->dword3;
			return(work.word.hi);

		default			:
			return(LIB_OK);
	}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_write_iomap_member()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Write IOMAP member.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number												*/
/*		USHORT	member			:	IOMAP member to be read										*/
/*		USHORT	data			:	Write data													*/
/*																								*/
/*	Return Code :																				*/
/*		Non																						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_write_iomap_member(CHANNEL_INFO *chbuffp, USHORT st_no, USHORT member, USHORT data)
{
	volatile JL100_HOST_IF_REGS *hirp;			/* Host I/F Top address */
	volatile JL100_IO_MAP_DWORD *iomapp;		/* IOMAP top address */
	DWORD	work;								/* Work */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get IOMAP area top address */
	iomapp = (volatile JL100_IO_MAP_DWORD *)((ULONG)hirp + chbuffp->iomap_ptr);

	switch(member)
	{
		case AXIS_ADR	:	/* Write axis address */
			work.dword = (iomapp + st_no)->dword1;
			work.word.lo = data;
			(iomapp + st_no)->dword1 = work.dword;
			break;

		case T_RSP		:	/* Write transmit response time */
			work.dword = (iomapp + st_no)->dword1;
			work.word.hi = data;
			(iomapp + st_no)->dword1 = work.dword;
			break;

		case CD_LEN		:	/* Write command data length */
			work.dword = (iomapp + st_no)->dword2;
			work.word.lo = data;
			(iomapp + st_no)->dword2 = work.dword;
			break;

		case RD_LEN		:	/* Write response data length */
			work.dword = (iomapp + st_no)->dword2;
			work.word.hi = data;
			(iomapp + st_no)->dword2 = work.dword;
			break;

		default			:
			break;
	}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_compar_member()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read communication parameter from SRAM area.											*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT member			:	Member definition code to be read							*/
/*																								*/
/*	Return Code :																				*/
/*		USHORT					:	Setting parameter 											*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
ULONG dummy_work;

USHORT jl100s_read_compar_member(CHANNEL_INFO *chbuffp, USHORT member)
{
	volatile JL100_HOST_IF_REGS *hirp;		/* Host I/F Top address */
	VULONG *com_parp;						/* Communication parameter accsess pointer */
	DWORD	work;							/* Work */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get IOMAP area top address */
	com_parp = (ULONG *)((ULONG)hirp + chbuffp->pars_data);

	switch(member)
	{
		case T_MCYC		:	/* Read axis address */
			work.dword = *(ULONG *)(com_parp + 3);
			dummy_work = work.dword;
			work.dword = *(ULONG *)(com_parp + 3);
			return(work.word.lo);

		case T_UNIT		:	/* Read transmit response */
			work.dword = *(ULONG *)(com_parp + 3);
			dummy_work = work.dword;
			work.dword = *(ULONG *)(com_parp + 3);
			return(work.word.hi);

		case PROT_SEL	:	/* Read command data length */
			work.dword = *(ULONG *)(com_parp + 4);
			dummy_work = work.dword;
			work.dword = *(ULONG *)(com_parp + 4);
			return(work.word.lo);

		case C2_DLY		:	/* Read response data length */
			work.dword = *(ULONG *)(com_parp + 7);
			dummy_work = work.dword;
			work.dword = *(ULONG *)(com_parp + 7);
			return(work.word.hi);

		default			:
			return(LIB_OK);
	}
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_chk_iomap_prm()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Check user setting IOMAP parameters.													*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp			:	Channel buffer address								*/
/*		JL100_USER_IOMAP *usr_iomapp	:	JL100 iomap parameter(user setting)					*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK				:	Normal end														*/
/*		ERROR_IOMAP_SIZE	:	User setting command(or response) length is not arrowed			*/
/*		ERROR_IOMAP_TRSP	:																	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_chk_iomap_prm(CHANNEL_INFO *chbuffp, JL100_USER_IOMAP *usr_iomapp)
{
	USHORT cd_len, rd_len, ch, ma_max;	/* Work */
	short  ret;							/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Set ma_max */
	if(chbuffp->ma_max == 0)
	{
		/* Slave */
		ma_max = 1;
	}
	else
	{
		/* Monitor slave */
		ma_max = chbuffp->ma_max;
	}

	/* Check command data length and response data length */
	for(ch = 0; ch <= ma_max; ch++)
	{
		cd_len = (usr_iomapp + ch)->cd_len;
		rd_len = (usr_iomapp + ch)->rd_len;
		if(((cd_len!=8) && (cd_len!=16) && (cd_len!=32) && (cd_len!=48) && (cd_len!=64)) ||
           ((rd_len!=8) && (rd_len!=16) && (rd_len!=32) && (rd_len!=48) && (rd_len!=64)))
		{
			return(ERROR_IOMAP_SIZE);
		}
	}
	return(ret);
}


//<S146>
#define AsicUprg_Base		(0xFFE7C000)
#define	AsicUprgSz			*(volatile CUSHORT*)(AsicUprg_Base)
#define	AsicUprg_Addr		(AsicUprg_Base + 2)
#define	AsicUprg			(volatile CUSHORT*)(AsicUprg_Addr)
/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_download_asicprg()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Download micro program to download SRAM area.(32bit bus connect)						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		ULONG	*hif_reg_top			:	Host I/F register top address						*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK							:	Normal end											*/
/*		ERROR_VERIFY_MICRO				:	Download micro is not correctly						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_download_asicprg(ULONG *hif_reg_top)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	VULONG   *dl_micro_adr;
	ULONG    *dl_micro_code_tbl;
	LONG     dl_micro_sz;
	ULONG    i, work, micro_code;
	short    ret;
	
	/* Initialize value */
	ret = LIB_OK;
	
	/* Get Host I/F register top address */
	hirp = (JL100_HOST_IF_REGS *)hif_reg_top;
	
	/* Get download SRAM top address */
	dl_micro_adr = (VULONG *)((ULONG)hirp + DEF_DOWNLOAD_SRAM_TOP_ADR);
	
	/* Get micro code table top address & size */
	dl_micro_code_tbl = (ULONG *)AsicUprg;
	dl_micro_sz  = (AsicUprgSz/2 < 0x4000) ? AsicUprgSz/2 : (AsicUprgSz/2 - 1);
	
	/* Download micro code */
	for(i = 0; i < dl_micro_sz; i++)
	{
#ifdef BIG_ENDIAN
		jl100_swap_long_data(dl_micro_code_tbl[i], micro_code);
		dl_micro_adr[i] = micro_code;
#else
		dl_micro_adr[i] = dl_micro_code_tbl[i];
#endif /* BIG_ENDIAN */
	}
	
	/* Verify download micro code */
	for(i = 0; i < dl_micro_sz; i++)
	{
		work = dl_micro_adr[i];
#ifdef BIG_ENDIAN
		jl100_swap_long_data(dl_micro_code_tbl[i], micro_code);
#else
		micro_code = dl_micro_code_tbl[i];
#endif /* BIG_ENDIAN */
		if(work != micro_code)
		{
			return(ERROR_VERIFY_MICRO);
		}
	}
	
	/*  */
	dl_micro_adr[0x1000-1] = 0xFFFFFFFF;
	
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_rbuff_offset()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get receive buffer offset of st_no station.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number												*/
/*																								*/
/*	Return Code :																				*/
/*		USHORT					:	Receive buffer offset of st_no station						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
unsigned short jl100s_get_rbuff_offset(CHANNEL_INFO *chbuffp, USHORT st_no)
{
	USHORT	offset;						/* Responce buffer offset */
	USHORT	i;

	/* Calculate offset of st_no station buffer */
	offset = 0;
	offset += SYNC_FRAME_SIZE;				/* #0 reserve area size */
	for(i = 1; i < st_no; i++)
	{
		offset += jl100s_read_iomap_member(chbuffp, i, RD_LEN);		/* responce data length */
		offset += (HEADER_SIZE + FCS_SIZE);	/* header info length */
	}
	return(offset);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_cbuff_offset()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get command buffer offset of st_no station.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number												*/
/*																								*/
/*	Return Code :																				*/
/*		USHORT					:	Command buffer offset of st_no station						*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
unsigned short jl100s_get_cbuff_offset(CHANNEL_INFO *chbuffp, USHORT st_no)
{
	USHORT	offset;						/* Command buffer offset */
	USHORT	i;

	/* Calculate offset of st_no station buffer */
	offset = 0;
	offset += SYNC_FRAME_SIZE;				/* #0 sync frame size */
	for(i = 1; i < st_no; i++)
	{
		offset += jl100s_read_iomap_member(chbuffp, i, CD_LEN);		/* command data length */
		offset += (HEADER_SIZE + FCS_SIZE);	/* header info length */
	}
	return(offset);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_accdrv_ver()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get access driver version.																*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		Nothing																					*/
/*																								*/
/*	Return Code :																				*/
/*		Access driver version : High byte	Access driver version.								*/
/*								Low  byte	Supported type.										*/
/*											(00h:Slave, 01h:C1master, 02h:C2master)				*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
ULONG jl100s_get_accdrv_ver(void)
{
	ULONG version;
	
	/* Get access driver version */
	version = ACCESS_DRIVER_VERSION;
	
	return(version);
}


ULONG jl100s_swaphl(ULONG i)
{
	return ( ((i & 0x0000FFFF) << 16) | ((i & 0xFFFF0000) >> 16) );
}
