/************************************************************************************************/
/*																								*/
/*		JL100S_LNK.C : JL-100 interface access driver											*/
/*																								*/
/************************************************************************************************/
/*																								*/
/*		Ver 1.10			:	2010.03.31	Initial coding	T.soeda								*/
/*																								*/
/************************ Copyright(C) 2007 Yaskawa Electric Corporation ************************/
/*																								*/
/*		FUNCTIONS :																				*/
/*			jl100s_change_rbuff()		:	Change toggle buffer(RSP)							*/
/*			jl100s_change_cbuff()		:	Change toggle buffer(CMD).							*/
/*			jl100s_read_gbldata()		:	Read global frame data.								*/
/*			jl100s_read_ldata()			:	Read cyclic data. (for sync mode)					*/
/*			jl100s_write_ldata()		:	Write cyclic data. (for sync mode)					*/
/*			jl100s_read_other_ldata()	:	Read cyclic data. (for moniter slave)				*/
/*			jl100s_req_rcv_frame()		:	Request receive frame and 							*/
/*												read received frame data.(for async mode)		*/
/*			jl100s_send_frame()			:	Request send frame.(for async mode)					*/
/*			jl100s_get_crcvstat()		:	Get receive status from CRCV* register.				*/
/*			jl100s_get_rrcvstat()		:	Get receive status from RRCV* register.				*/
/*			jl100s_get_stistat()		:	Get  status from STI* register.						*/
/*			jl100s_read_comstat()		:	Get ESTS register.									*/
/*			jl100s_get_gblstat()		:	Get global frame status								*/
/*			jl100s_chk_transmitstat()	:	Check transmission end.								*/
/*			jl100s_get_cid()			:	Get CID register.									*/
/*			jl100s_begin_cyclic_com()	:	Set WD_R(CMD), and clear interrupt factors.			*/
/*			jl100s_chk_rcv_tmcframe()	:	Reception check of TMC frame.						*/
/*			jl100s_chk_intrp_factor()	:	Check interrupt factor.								*/
/*			jl100s_request_wdt()		:	Request to kick wdt.								*/
/*			jl100s_async_rcv_enable()	:	Enable receive frame.(for async mode)				*/
/*																								*/
/************************************************************************************************/

/* includes */
#include "jl100s_gbl.h"

/* local defines */
#define MASK_SYSRSV_ERR_BIT		0x0E3FAF3F

/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif
short jl100s_change_rbuff(CHANNEL_INFO *chbuffp);
short jl100s_change_cbuff(CHANNEL_INFO *chbuffp);
short jl100s_read_gbldata(CHANNEL_INFO *chbuffp, ULONG *rbuffp);
short jl100s_read_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *rbuffp);
short jl100s_write_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *sbuffp);
short jl100s_read_other_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *rbuffp);
short jl100s_req_rcv_frame(CHANNEL_INFO *chbuffp, USHORT *st_adr_da, USHORT *st_adr_sa, USHORT *rcv_stat, 
																     USHORT *ftype, ULONG *rbuffp, USHORT *size);
short jl100s_send_frame(CHANNEL_INFO *chbuffp, USHORT st_adr_da, USHORT st_adr_sa, 
                                                                 USHORT ftype, ULONG *sbuffp, USHORT size);
void jl100s_get_crcvstat(CHANNEL_INFO *chbuffp, ULONG *crcvstat);
void jl100s_get_rrcvstat(CHANNEL_INFO *chbuffp, ULONG *rrcvstat);
void jl100s_get_stistat(CHANNEL_INFO *chbuffp, ULONG *stistat);
ULONG jl100s_read_comstat(CHANNEL_INFO *chbuffp);
short jl100s_get_gblstat(CHANNEL_INFO *chbuffp);
short jl100s_get_cid(CHANNEL_INFO *chbuffp);
short jl100s_begin_cyclic_com(CHANNEL_INFO *chbuffp);
short jl100s_chk_rcv_tmcframe(CHANNEL_INFO *chbuffp, ULONG *t_mcyc);
unsigned long jl100s_chk_intrp_factor(CHANNEL_INFO *chbuffp);
void jl100s_request_wdt(CHANNEL_INFO *chbuffp);
void jl100s_async_rcv_enable(CHANNEL_INFO *chbuffp);


#ifdef __cplusplus
}
#endif

/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_change_rbuff()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Change toggle buffer(RSP).																*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_TOUT_CHANG_RBUFF	:	Change response buffer didn't complete						*/
/*		ERROR_CHANG_RBUFF		:	Change response buffer is failed							*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_change_rbuff(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	data;						/* Register read data */
	ULONG	i;							/* Polling counter */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Request change receive buffer */
	jl100_set_cmd(hirp, JL100_CMD_RBX_R);

	/* Wait change receive buffer complete */
	ret = ERROR_TOUT_CHANG_RBUFF;
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		data = jl100_get_cmd(hirp);
		if((data & JL100_CMD_RBX_R) == 0)
		{
			ret = LIB_OK;
			break;
		}
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_change_cbuff()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Change toggle buffer(CMD).																*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_TOUT_CHANG_CBUFF	:	Change command buffer didn't complete						*/
/*		ERROR_CHANG_CBUFF		:	Change command buffer is failed								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_change_cbuff(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	data;						/* Register read data */
	ULONG	i;							/* Polling counter */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Request change receive buffer */
	jl100_set_cmd(hirp, JL100_CMD_CBX_R);

	/* Wait change command buffer complete */
	ret = ERROR_TOUT_CHANG_CBUFF;
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		data = jl100_get_cmd(hirp);
		if((data & JL100_CMD_CBX_R) == 0)
		{
			ret = LIB_OK;
			break;
		}
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_gbldata()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read global frame data. (for sync mode)													*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		ULONG	*rbuffp			:	Global frame data save buffer								*/
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
short jl100s_read_gbldata(CHANNEL_INFO *chbuffp, ULONG *rbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	cbadr;						/* Command buffer address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	ULONG	size_buf;
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;
	size_buf = 8;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get receive buffer offset address */
	cbadr = ((ULONG)hirp + DEF_SRAM_TOP_ADR);
	cbadr += (USHORT)jl100_get_cbadr(hirp);
	cbadr += HEADER_SIZE;

	/* Read receive data */
	rdp = (ULONG *)cbadr;
	wtp = rbuffp;
	while(size_buf > 0)
	{
		*wtp = jl100_read_dwdata(rdp);
		wtp++;
		size_buf -= DWORD_SIZE;
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_ldata()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read cyclic data. (for sync mode)														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number to be read									*/
/*		ULONG	*rbuffp			:	Response data save buffer									*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_STNO		:	Setting st_no is not exist									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_read_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *rbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	cbadr;						/* Command buffer address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	USHORT	size_buf;					/* Responce data length from IOMAP */
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Check st_no */
	/* Slave */
	if((chbuffp->ma_max == 0) && (st_no != 1))
	{
		return(ERROR_INVALID_STNO);
	}
	/* Monitor slave */
	if((chbuffp->ma_max != 0) && ((st_no == 0) || (st_no > chbuffp->ma_max)))
	{
		return(ERROR_INVALID_STNO);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get response data length */
	size_buf = jl100s_read_iomap_member(chbuffp, st_no, CD_LEN);

	/* Data length check */
	if((size_buf != 8) && (size_buf != 16) && (size_buf != 32) && (size_buf != 48) && (size_buf != 64))
	{
		return(ERROR_IOMAP_SIZE);
	}

	/* Get receive buffer offset address */
	cbadr = ((ULONG)hirp + DEF_SRAM_TOP_ADR);
	cbadr += (USHORT)jl100_get_cbadr(hirp);
	cbadr += (chbuffp->usr_iomap + st_no)->cmdbuff_offset;
	cbadr += HEADER_SIZE;

	/* Read receive data */
	rdp = (ULONG *)cbadr;
	wtp = rbuffp;
	while(size_buf > 0)
	{
		*wtp = jl100_read_dwdata(rdp);
		wtp++;
		size_buf -= DWORD_SIZE;
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_write_ldata()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Write cyclic data. (for sync mode)														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number to be read									*/
/*		ULONG	*sbuffp			:	Send data save buffer										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_STNO		:	Setting st_no is not exist									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_write_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *sbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	rbadr;						/* Response buffer address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	USHORT	size_buf;					/* Command data length from IOMAP */
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Check st_no */
	/* Slave */
	if((chbuffp->ma_max == 0) && (st_no != 1))
	{
		return(ERROR_INVALID_STNO);
	}
	/* Monitor slave */
	if((chbuffp->ma_max != 0) && ((st_no == 0) || (st_no > chbuffp->ma_max)))
	{
		return(ERROR_INVALID_STNO);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get command data length from IOMAP */
	size_buf = jl100s_read_iomap_member(chbuffp, st_no, RD_LEN);

	/* Data length check */
	if((size_buf != 8) && (size_buf != 16) && (size_buf != 32) && (size_buf != 48) && (size_buf != 64))
	{
		return(ERROR_IOMAP_SIZE);
	}

	/* Get command buffer offset address */
	rbadr = ((ULONG)hirp + DEF_SRAM_TOP_ADR);
	rbadr += (USHORT)jl100_get_rbadr(hirp);
	rbadr += (chbuffp->usr_iomap + st_no)->rspbuff_offset;
	rbadr += HEADER_SIZE;

	/* Write command data */
	rdp = sbuffp;
	wtp = (ULONG *)rbadr;
	while(size_buf > 0)
	{
		jl100_write_dwdata(wtp, rdp);
		size_buf -= DWORD_SIZE;
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_other_ldata()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read cyclic data. (for moniter slave sync mode)											*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_no			:	Station number to be read									*/
/*		ULONG	*rbuffp			:	Response data save buffer									*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_STNO		:	Setting st_no is not exist									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_read_other_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *rbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	rbadr;						/* Response buffer address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	USHORT	size_buf;					/* Responce data length from IOMAP */
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Check st_no */
	if((st_no > (chbuffp->ma_max)) || (st_no == 0))
	{
		return(ERROR_INVALID_STNO);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get response data length */
	size_buf = jl100s_read_iomap_member(chbuffp, st_no, RD_LEN);

	/* Data length check */
	if((size_buf != 8) && (size_buf != 16) && (size_buf != 32) && (size_buf != 48) && (size_buf != 64))
	{
		return(ERROR_IOMAP_SIZE);
	}

	/* Get receive buffer offset address */
	rbadr = ((ULONG)hirp + DEF_SRAM_TOP_ADR);
	rbadr += (USHORT)jl100_get_rbadr(hirp);
	rbadr += (chbuffp->usr_iomap + st_no)->rspbuff_offset;
	rbadr += HEADER_SIZE;

	/* Read receive data */
	rdp = (ULONG *)rbadr;
	wtp = rbuffp;
	while(size_buf > 0)
	{
		*wtp = jl100_read_dwdata(rdp);
		wtp++;
		size_buf -= DWORD_SIZE;
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_req_rcv_frame()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request receive frame.																	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address.										*/
/*		USHORT *st_adr			:	Station address save buffer pointer.						*/
/*		USHORT *rcv_stat		:	Receive status save buffer pointer.							*/
/*		USHORT *ftype			:	Frame type save buffer pointer.								*/
/*		ULONG  *rbuffp			:	Receive data save buffer pointer.							*/
/*		USHORT *size			:	Receive data size save buffer pointer.						*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end.													*/
/*		RCVING_FRAME			:	Waiting receive frame.										*/
/*		RECEIVED_FRAME			:	Received frame.												*/
/*																								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_req_rcv_frame(CHANNEL_INFO *chbuffp, USHORT *st_adr_da, USHORT *st_adr_sa, USHORT *rcvstat, 
																  USHORT *ftype, ULONG *rbuffp, USHORT *size)
{
	volatile JL100_HOST_IF_REGS *hirp;		/* Host I/F Top address */
	JL100_FRAME_HEADER_ASYNC frm_header;	/* Frame header struct */
	ULONG	rbadr;							/* Responce buffer address */
	ULONG	*rdp, *wtp;						/* for RAM access */
	ULONG	data;							/* Register read data */
	ULONG	work;
	USHORT	size_buff;
	short	ret;							/* Return code */
	
	/* Initialize value */
//	ret =RCVING_FRAME;
	
	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;
	
	/* Get CMD register */
	data = jl100_get_cmd(hirp);
	
	if((data & JL100_CMD_RX_R) != 0)
	{
		/* Waiting receive frame */
		return(RCVING_FRAME);
	}
	else
	{
		/* Not request receive frame */
		data = jl100_get_ints(hirp);
		if((data & JL100_INTS_RX_I) != 0)
		{
			/* Received frame */
			ret = RECEIVED_FRAME;
			jl100_clear_ints(hirp, JL100_INTS_RX_I);

			/* Get receive buffer offset address */
			rbadr = (ULONG)((ULONG)hirp + DEF_SRAM_TOP_ADR);
			rbadr += (chbuffp->pars_data - DEF_SRAM_TOP_ADR) >> 1;

			/* Read header data */
			rdp = (ULONG *)rbadr;
			wtp = &frm_header.ulong_ptr;
			size_buff = ASYNC_HEADER_SIZE;
			while(size_buff > 0)
			{
				*wtp = jl100_read_dwdata(rdp);
				wtp++;
				size_buff -= DWORD_SIZE;
			}
			/* Save destination address */
			*st_adr_da = frm_header.str.da;
			/* Save source address */
			*st_adr_sa = frm_header.str.sa;
			/* Save receive status */
			*rcvstat = frm_header.str.stat;
			if(*rcvstat == ASYNC_RCV_TIMOUT)
			{
				/* Request receive frame */
				jl100_set_cmd(hirp, JL100_CMD_RX_R);
				return(RCVING_FRAME);
			}
			/* Save frame type */
			*ftype = (frm_header.str.ftyp_len >> 12) & 0x000f;
			/* Save data size */
			*size = frm_header.str.ftyp_len & 0x0fff;
			
			/* Read receive data */
			rdp = (ULONG *)(rbadr + ASYNC_HEADER_SIZE);
			wtp = rbuffp;
			size_buff = *size;
			while(size_buff > 0)
			{
				*wtp = jl100_read_dwdata(rdp);
				wtp++;
				size_buff -= DWORD_SIZE;
			}
		}
		else
		{
			/* Set response monitering time */
			/* Get receive buffer offset address */
			rbadr = (ULONG)((ULONG)hirp + DEF_SRAM_TOP_ADR);
			rbadr += (chbuffp->pars_data - DEF_SRAM_TOP_ADR) >> 1;
			work = 0xffff;
			wtp = (ULONG *)rbadr;
			*wtp |= work;
			
			/* Request receive frame */
			jl100_set_cmd(hirp, JL100_CMD_RX_R);
			ret = RCVING_FRAME;
		}
	}
	
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_send_frame()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Send frame data. (for async mode)														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	st_adr			:	Station address to be read									*/
/*		USHORT	ftype			:	Frame type													*/
/*		USHORT	*sbuffp			:	Send data buffer											*/
/*		USHORT	size			:	Send data size												*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		SNDING_FRAME			:	Sending frame												*/
/*		ERROR_INVALID_SIZE		:	Invalid size												*/
/*		ERROR_INVALID_FTYPE		:	Invalid ftype												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_send_frame(CHANNEL_INFO *chbuffp, USHORT st_adr_da, USHORT st_adr_sa, USHORT ftype, ULONG *sbuffp, USHORT size)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	JL100_FRAME_HEADER_ASYNC frm_header;/* Frame header struct */
	ULONG	cbadr;						/* Command buffer address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	ULONG	data;						/* Register read data */
	USHORT	size_buff;
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Check frame type and size */
	if((ftype != 0x02) && (ftype != 0x0c))
	{
		ret = ERROR_INVALID_FTYPE;
	}
	if((size % DWORD_SIZE) != 0)
	{
		ret = ERROR_INVALID_SIZE;
	}

	if(ret != LIB_OK)
	{
		return(ret);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get CMD register */
	data = jl100_get_cmd(hirp);

	/* Check transmit request */
	if((data & JL100_CMD_TX_R) != 0)
	{
		ret = SNDING_FRAME;
	}
	else
	{
		/* Not request send frame */
		data = jl100_get_ints(hirp);
		if((data & JL100_INTS_TX_I) != 0)
		{
			/* Sent frame */
			ret = LIB_OK;
			jl100_clear_ints(hirp, JL100_INTS_TX_I);
		}
		
		/* Set frame header data */
		frm_header.str.t_rsp = 0xffff;
		frm_header.str.stat = 0x0000;
		frm_header.str.da = st_adr_da;
		frm_header.str.sa = st_adr_sa;
		frm_header.str.msg_ctrl = 0x0000;
		frm_header.str.ftyp_len =  (ftype << 12) | (size & 0x0fff);

		/* Get command buffer address offset */
		cbadr = (ULONG)((ULONG)hirp + DEF_SRAM_TOP_ADR);

		/* Write frame header data to SRAM area */
		rdp = &frm_header.ulong_ptr;
		wtp = (ULONG *)cbadr;
		size_buff = ASYNC_HEADER_SIZE;
		while(size_buff > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			size_buff -= DWORD_SIZE;
		}

		/* Write send data */
		rdp = sbuffp;	/* Set send data buffer pointer */
		while(size > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			size -= DWORD_SIZE;
		}

		/* Request transmit data */
		jl100_set_cmd(hirp, JL100_CMD_TX_R);
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_crcvstat()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get receive status from CRCV* register.													*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		ULONG	*rcvstat		:	Receive status save buffer									*/
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
void jl100s_get_crcvstat(CHANNEL_INFO *chbuffp, ULONG *crcvstat)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get receive status */
	crcvstat[0] = jl100_get_crcv1(hirp);
	crcvstat[1] = jl100_get_crcv2(hirp);

}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_rrcvstat()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get receive status from RRCV* register.													*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		ULONG	*rcvstat		:	Receive status save buffer									*/
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
void jl100s_get_rrcvstat(CHANNEL_INFO *chbuffp, ULONG *rrcvstat)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get receive status */
	rrcvstat[0] = jl100_get_rrcv1(hirp);
	rrcvstat[1] = jl100_get_rrcv2(hirp);

}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_stistat()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get  status from STI* register.															*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		ULONG	*stistat		:	Connection status save buffer								*/
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
void jl100s_get_stistat(CHANNEL_INFO *chbuffp, ULONG *stistat)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get connection status */
	stistat[0] = jl100_get_sti1(hirp);
	stistat[1] = jl100_get_sti2(hirp);

}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_comstat()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get ESTS register.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		LIB_NG					:	ESTS														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
unsigned long jl100s_read_comstat(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get ESTS register */
	ret = jl100_get_ests(hirp);

	/* Clear ESTS register */
	jl100_clear_ests(hirp, ret);

	ret &= MASK_SYSRSV_ERR_BIT;

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_gblstat()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get global frame status.																*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_GBL_NOT_RCV		:	Not receive global frame 									*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_get_gblstat(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	ints;						/* Register read */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get INTS register */
	ints = jl100_get_ints(hirp);

	/* Check global status */
	if((ints & JL100_INTS_SFR) != 0)
	{
		/* Clear interrupt factor */
		jl100_clear_ints(hirp, JL100_INTS_SFR);

		ret = LIB_OK;
	}
	else
	{
		ret = ERROR_GBL_NOT_RCV;
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_get_cid()																		*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Get CID register .																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*																								*/
/*	Return Code :																				*/
/*		cid						:	Chip ID														*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_get_cid(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get CID register */
	ret = (USHORT)jl100_get_cid(hirp);

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_begin_cyclic_com()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Set WD_R(CMD), and clear interrupt factors.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp		:	Channel buffer address									*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK						:	Normal end												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_begin_cyclic_com(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	short   ret;						/* Return code */
	
	/* Initialize value */
	ret = LIB_OK;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;
	
	/* Set WD */
	jl100_set_cmd(hirp, JL100_CMD_WD_R);
	
	/* Clear interrupt factor */
	jl100_clear_ints(hirp, JL100_INTS_TIM1);
	
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_chk_rcv_tmcframe()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Reception check of transmission cycle notification frame.								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp		:	Channel buffer address									*/
/*																								*/
/*	Return Code :																				*/
/*		RCVED_TMC_FRAME				:	Received TMC frame.										*/
/*		NOT_RCV_TMC_FRAME			:	Not receiving TMC frame.								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_chk_rcv_tmcframe(CHANNEL_INFO *chbuffp, ULONG *t_mcyc)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	ints;						/* for register read */
	short	ret;						/* Return code */

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get INTS register */
	ints = jl100_get_ints(hirp);
	
	/* Check INTS(RX_TMC) */
	if((ints & JL100_INTS_RX_TMC) != 0)
	{
		/* Clear interrupt factor */
		jl100_clear_ints(hirp, JL100_INTS_RX_TMC);
		
		/* Save protocol type */
		chbuffp->prot_sel = jl100s_read_compar_member(chbuffp, PROT_SEL);
		
		/* Get t_mcyc from Communication parameter area */
		jl100s_get_trans_cycle(chbuffp, t_mcyc);
		
		ret = RCVED_TMC_FRAME;
	}
	else
	{
		ret = NOT_RCV_TMC_FRAME;
	}
	
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_chk_intrp_factor()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Check interrupt factor.																	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp		:	Channel buffer address									*/
/*																								*/
/*	Return Code :																				*/
/*		INTS regs.																				*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
unsigned long jl100s_chk_intrp_factor(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	ints;						/* for register read */
	
	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get INTS register */
	ints = jl100_get_ints(hirp);
	
	return(ints);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_request_wdt()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request to kick WDT.																	*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp		:	Channel buffer address									*/
/*																								*/
/*	Return Code :																				*/
/*		Non																						*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_request_wdt(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	
	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Kick WDT */
	jl100_set_cmd(hirp, JL100_CMD_WD_R);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_async_rcv_enable()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Enable receive frame (for async mode).													*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp		:	Channel buffer address									*/
/*																								*/
/*	Return Code :																				*/
/*		Non																						*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
void jl100s_async_rcv_enable(CHANNEL_INFO *chbuffp)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	
	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Receive enable */
	jl100_set_cmd(hirp, JL100_CMD_RX_R);
}
