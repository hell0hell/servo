/************************************************************************************************/
/*																								*/
/*		JL100S_MSG.C : JL-100 interface access driver											*/
/*																								*/
/************************************************************************************************/
/*																								*/
/*		Ver 1.10			:	2010.03.31	Initial coding	T.soeda								*/
/*																								*/
/************************ Copyright(C) 2007 Yaskawa Electric Corporation ************************/
/*																								*/
/*		FUNCTIONS :																				*/
/*			jl100s_req_rcv_msgdata()	:	Request receive message data.						*/
/*			jl100s_req_snd_msgdata()	:	Request send message data.							*/
/*			jl100s_abort_msgdata()		:	Request abort message communication.				*/
/*			jl100s_read_msgdata()		:	Read message data.									*/
/*			jl100s_write_msgdata()		:	write message data.									*/
/*																								*/
/************************************************************************************************/
/* includes */
#include "jl100s_gbl.h"

/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif
short jl100s_req_rcv_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT *st_adr_da, USHORT st_adr, USHORT *msg_size);
short jl100s_req_snd_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT st_adr, USHORT st_adr_sa, USHORT msg_size);
short jl100s_abort_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg);
short jl100s_read_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT offset, USHORT size, ULONG *rdata);
short jl100s_write_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT offset, USHORT size, ULONG *sdata);
#ifdef __cplusplus
}
#endif

/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_req_rcv_msgdata()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request receive message data.															*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	cnmsg			:	Select C1 or C2												*/
/*		USHORT	st_adr			:	Station address												*/
/*		USHORT	*msg_size		:	Message data size											*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Complete receive message data								*/
/*		RCVING_MSG				:	In receiving message data									*/
/*		ERROR_MSG_ABORT			:	Message communication aborted								*/
/*		ERROR_MSG_OVERFLOW		:	Message data size over flow									*/
/*		ERROR_BUSY_MSG			:	Message communication rejected								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_req_rcv_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT *st_adr_da, USHORT st_adr, USHORT *msg_size)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	JL100_FRAME_HEADER_MSG frm_header;	/* Message frame header struct */
	ULONG	cmd, cmd_msk;				/* CMD register */
	ULONG	ints, ints_msk;				/* INTS register */
	ULONG	msg_adr;					/* Message receive buffer offset address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	USHORT	size_buff;
	short	ret;						/* Return code */

	/* Initialize value */
	ret = LIB_OK;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Check C1 or C2 */
	if(cnmsg == DEF_C1_MST)
	{
		cmd_msk = JL100_CMD_C1MRX_R;
		ints_msk = JL100_INTS_C1MRX_I;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + chbuffp->c1msg_sz;
	}
	else
	{
		cmd_msk = JL100_CMD_C2MRX_R;
		ints_msk = JL100_INTS_C2MRX_I;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + (chbuffp->c1msg_sz << 1) + chbuffp->c2msg_sz;
	}

	/* Get CMD register */
	cmd = jl100_get_cmd(hirp);
	
	if((cmd & cmd_msk) != 0)	/* In receiving message */
	{
		return(RCVING_MSG);
	}

	/* Get INTS register */
	ints = jl100_get_ints(hirp);

	if((ints & ints_msk) != 0)	/* Receive message complete */
	{
		/* Clear interrupt factor */
		jl100_clear_ints(hirp, ints_msk);

		/* Get header data */
		rdp = (ULONG *)msg_adr;
		wtp = &frm_header.ulong_ptr;
		size_buff = sizeof(JL100_FRAME_HEADER_MSG);
		while(size_buff > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			size_buff -= DWORD_SIZE;
		}

		/* Set message size */
		*msg_size = (frm_header.str.ftyp_len & 0x0fff);
		/* Set message destination address */
		*st_adr_da = frm_header.str.da;

		/* Check receive status */
		if(frm_header.str.stat == MSG_OVERFLOW)
		{
			ret = ERROR_MSG_OVERFLOW;
		}
		else if(frm_header.str.stat == MSG_ABORT)
		{
			ret = ERROR_MSG_ABORT;
		}
		else if(frm_header.str.stat == MSG_BUSY)
		{
			ret = ERROR_BUSY_MSG;
		}

		return(ret);
	}

	/* Request receive message data */
	jl100_set_cmd(hirp, cmd_msk);

	return(RCVING_MSG);
}




/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_req_snd_msgdata()																*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request send message data.																*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	cnmsg			:	Select C1 or C2												*/
/*		USHORT	st_adr			:	Station address												*/
/*		USHORT	*msg_size		:	Message data size											*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Complete send message data									*/
/*		SNDING_MSG				:	In sending message data										*/
/*		ERROR_MSG_ABORT			:	Message communication aborted								*/
/*		ERROR_MSG_UNDERFLOW		:	Message data size under flow								*/
/*		ERROR_BUSY_MSG			:	Message communication rejected								*/
/*		ERROR_MSG_DIV_OVER		:	Message division over										*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_req_snd_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT st_adr, USHORT st_adr_sa, USHORT msg_size)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	JL100_FRAME_HEADER_MSG frm_header;	/* Message frame header struct */
	ULONG	cmd, cmd_msk;				/* CMD register */
	ULONG	ints, ints_msk;				/* INTS register */
	ULONG	msg_adr;					/* Message receive buffer offset address */
	ULONG	*rdp, *wtp;					/* for RAM access */
	USHORT	st_no;						/* Station number */
	USHORT	size_buff;
	USHORT	t_rsp_work;					/* transmit monitering time */
	USHORT	pkt_size;
	short	ret;						/* Return code */

	/* Initialize value */
	ret = SNDING_MSG;

	if(msg_size > chbuffp->pkt_sz)
	{
		pkt_size = chbuffp->pkt_sz;
	}
	else
	{
		pkt_size = msg_size;
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Get messege buffer pointer */
	if(cnmsg == DEF_C1_MST)
	{
		cmd_msk = JL100_CMD_C1MTX_R;
		ints_msk = JL100_INTS_C1MTX_I;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr;
	}
	else
	{
		cmd_msk = JL100_CMD_C2MTX_R;
		ints_msk = JL100_INTS_C2MTX_I;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + (chbuffp->c1msg_sz << 1);
	}

	/* Get CMD register */
	cmd = jl100_get_cmd(hirp);
	
	if((cmd & cmd_msk) != 0)	/* In receiving message */
	{
		return(SNDING_MSG);
	}

	/* Get INTS register */
	ints = jl100_get_ints(hirp);

	/* Sending messege complete */
	if((ints & ints_msk) != 0)
	{
		ret = LIB_OK;
		/* Clear interrupt factor */
		jl100_clear_ints(hirp, ints_msk);
		
		/* Get header data */
		rdp = (ULONG *)msg_adr;
		wtp = &frm_header.ulong_ptr;
		size_buff = sizeof(JL100_FRAME_HEADER_MSG);
		while(size_buff > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			size_buff -= DWORD_SIZE;
		}
		
		/* Check send status */
		if(frm_header.str.stat == MSG_SND_ABORT)
		{
			ret = ERROR_MSG_ABORT;
		}
		else if(frm_header.str.stat == MSG_UNDERFLOW)
		{
			ret = ERROR_MSG_UNDERFLOW;
		}
		else if(frm_header.str.stat == MSG_SND_BUSY)
		{
			ret = ERROR_BUSY_MSG;
		}
		else if(frm_header.str.stat == MSG_DIV_OVER)
		{
			ret = ERROR_MSG_DIV_OVER;
		}
		
		return(ret);
	}

	/* Get station number */
	if(chbuffp->ma_max == 0)
	{
		/* Slave */
		st_no = 1;
	}
	else
	{
		/* Monitor slave */
		for(st_no = 0; st_no <= chbuffp->ma_max; st_no++)
		{
			if(jl100s_read_iomap_member(chbuffp, st_no, AXIS_ADR) == chbuffp->ma0)
			{
				break;
			}
		}
	}

	/* Calculate transmit monitering time */
	t_rsp_work = (jl100s_read_iomap_member(chbuffp, st_no, TR_DLY) << 1);
	if(chbuffp->t_unit == TUNIT_10NSEC)
	{
		t_rsp_work += 2400;
		if(pkt_size > 64)
		{
			t_rsp_work += ((pkt_size - 64) << 3);
		}
	}
	else if(chbuffp->t_unit == TUNIT_100NSEC)
	{
		t_rsp_work += 240;
		if(pkt_size > 64)
		{
			t_rsp_work += (((pkt_size - 64) << 3) / 10);
		}
	}
	else
	{
		t_rsp_work += 24;
		if(pkt_size > 64)
		{
			t_rsp_work += (((pkt_size - 64) << 3) / 100);
		}
	}

	/* Set header data */
	frm_header.str.reserve = t_rsp_work;				/* Set transmit monitering time */
	frm_header.str.stat    = 0x0000;					/* Set transmit status(set by JL100) */
	frm_header.str.da      = st_adr;					/* Set destination address */
	frm_header.str.sa      = st_adr_sa;					/* Set source address */
	frm_header.str.msg_ctrl= 0x0000;					/* Set message control(set by JL100) */
	frm_header.str.ftyp_len= (FTYPE_MSG | msg_size);	/* Set frame type and data length */

	/* Write header data */
	rdp = &frm_header.ulong_ptr;
	wtp = (ULONG *)msg_adr;
	size_buff = sizeof(JL100_FRAME_HEADER_MSG);
	while(size_buff > 0)
	{
		jl100_write_dwdata(wtp, rdp);
		size_buff -= DWORD_SIZE;
	}

	/* Request send message data */
	jl100_set_cmd(hirp, cmd_msk);

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_abort_msgdata()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Request abort send/receive message data.												*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	cnmsg			:	Select C1 or C2												*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_ABT_MSG			:	Abort message did not completed.							*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_abort_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg)
{
	volatile JL100_HOST_IF_REGS *hirp;			/* Host I/F Top address */
	ULONG	cmd_abt;							/* CMD register */
	ULONG	ints, ints_msk;						/* INTS register */
	ULONG	i;									/* Polling counter */
	short	ret;								/* Return code */

	/* Initialize value */
	ret = ERROR_ABT_MSG;

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	if(cnmsg == DEF_C1_MST)
	{
		cmd_abt = JL100_CMD_C1ABT_R;
		ints_msk = JL100_INTS_C1ABT_I;
	}
	else
	{
		cmd_abt = JL100_CMD_C2ABT_R;
		ints_msk = JL100_INTS_C2ABT_I;
	}

	/* Request abort Cn message communication */
	jl100_set_cmd(hirp, cmd_abt);

	/* Wait abort Cn message communication complete */
	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		ints = jl100_get_ints(hirp);
		if((ints & ints_msk) != 0)
		{
			ret = LIB_OK;
			/* Clear interrupt factor */
			jl100_clear_ints(hirp, ints_msk);
			break;
		}
	}
	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_read_msgdata()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Read message data.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	cnmsg			:	Select C1 or C2												*/
/*		USHORT	offset			:	Offset address to be read									*/
/*		USHORT	size			:	Size to be read												*/
/*		ULONG	*rdata			:	Receive message save buffer									*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_SIZE		:	Invalid size												*/
/*		ERROR_REQ_MSG_SIZE		:	Request size is over message buffer area.					*/
/*		ERROR_RCVING_MSG		:	In requesting receive message data							*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_read_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT offset, USHORT size, ULONG *rdata)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	msg_adr;					/* Message receive buffer offset address */
	ULONG	offset_adr;					/* Offset address */
	ULONG	*wtp, *rdp;					/* for RAM access */
	ULONG	cmd, cmd_msk;				/* CMD register */
	USHORT	msg_size;					/* Message size */
	short	ret;						/* Return code */

	/* Check request offset and size */
	if((offset % DWORD_SIZE != 0) || (size % DWORD_SIZE != 0))
	{
		return(ERROR_INVALID_SIZE);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Check C1 or C2 */
	if(cnmsg == DEF_C1_MST)
	{
		cmd_msk = JL100_CMD_C1MRX_R;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + chbuffp->c1msg_sz + sizeof(JL100_FRAME_HEADER_MSG);
		msg_size = chbuffp->c1msg_sz;
	}
	else
	{
		cmd_msk = JL100_CMD_C2MRX_R;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + (chbuffp->c1msg_sz << 1) +chbuffp->c2msg_sz
                                                                     + sizeof(JL100_FRAME_HEADER_MSG);
		msg_size = chbuffp->c2msg_sz;
	}

	/* Check message size and offset */
	if((offset + size) > msg_size)
	{
		return(ERROR_REQ_MSG_SIZE);
	}

	/* Get CMD register */
	cmd = jl100_get_cmd(hirp);
	if((cmd & cmd_msk) == 0)
	{
		/* Calculate offset of message receive buffer */
		offset_adr = msg_adr + offset;

		/* Get message data */
		rdp = (ULONG *)offset_adr;
		wtp = rdata;
		while(size > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			size -= DWORD_SIZE;
		}
		ret = LIB_OK;
	}
	else
	{
		ret = ERROR_RCVING_MSG;
	}

	return(ret);
}



/************************************************************************************************/
/*																								*/
/*	Function Name :																				*/
/*		jl100s_write_msgdata()																	*/
/*																								*/
/*	Function Abstruct:																			*/
/*		Write message data.																		*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Parameter :																					*/
/*		CHANNEL_INFO *chbuffp	:	Channel buffer address										*/
/*		USHORT	cnmsg			:	Select C1 or C2												*/
/*		USHORT	offset			:	Offset address to be write									*/
/*		USHORT	size			:	Size to be write											*/
/*		ULONG	*sdata			:	Send message data buffer									*/
/*																								*/
/*	Return Code :																				*/
/*		LIB_OK					:	Normal end													*/
/*		ERROR_INVALID_SIZE		:	Invalid size												*/
/*		ERROR_REQ_MSG_SIZE		:	Request size is over message buffer area.					*/
/*		ERROR_SNDING_MSG		:	In requesting send message data								*/
/*																								*/
/*==============================================================================================*/
/*																								*/
/*	Note	:																					*/
/*																								*/
/*	Log		:																					*/
/*																								*/
/************************************************************************************************/
short jl100s_write_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT offset, USHORT size, ULONG *sdata)
{
	volatile JL100_HOST_IF_REGS *hirp;	/* Host I/F Top address */
	ULONG	msg_adr;					/* Message receive buffer offset address */
	ULONG	offset_adr;					/* Offset address */
	ULONG	*wtp, *rdp;					/* for RAM access */
	ULONG	cmd, cmd_msk;				/* CMD register */
	USHORT	msg_size;					/* Message size */
	short	ret;						/* Return code */

	/* Check request offset and size */
	if((offset % DWORD_SIZE != 0) || (size % DWORD_SIZE != 0))
	{
		return(ERROR_INVALID_SIZE);
	}

	/* Get Host I/F register top address */
	hirp = chbuffp->hif_reg_top;

	/* Check C1 or C2 */
	if(cnmsg == DEF_C1_MST)
	{
		cmd_msk = JL100_CMD_C1MTX_R;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + sizeof(JL100_FRAME_HEADER_MSG);
		msg_size = chbuffp->c1msg_sz;
	}
	else
	{
		cmd_msk = JL100_CMD_C2MTX_R;
		msg_adr = (ULONG)hirp + chbuffp->msg_ptr + (chbuffp->c1msg_sz << 1) + sizeof(JL100_FRAME_HEADER_MSG);
		msg_size = chbuffp->c2msg_sz;
	}

	/* Check message size and offset */
	if((offset + size) > msg_size)
	{
		return(ERROR_REQ_MSG_SIZE);
	}

	/* Get CMD register */
	cmd = jl100_get_cmd(hirp);
	if((cmd & cmd_msk) == 0)
	{
		/* Calculate offset of message send buffer */
		offset_adr = msg_adr + offset;

		/* Get message data */
		rdp = sdata;
		wtp = (ULONG *)offset_adr;
		while(size > 0)
		{
			jl100_write_dwdata(wtp, rdp);
			size -= DWORD_SIZE;
		}
		ret = LIB_OK;
	}
	else
	{
		ret = ERROR_SNDING_MSG;
	}

	return(ret);
}
