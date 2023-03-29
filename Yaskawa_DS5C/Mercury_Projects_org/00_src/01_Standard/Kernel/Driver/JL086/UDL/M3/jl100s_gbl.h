/****************************************************************************/
/*                                                                          */
/*        JL100S_GBL.H : Common header of JL-100 access driver              */
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

#ifdef __BIG_ENDIAN__
#include "big_jl100_def.h" 
#else
#include "little_jl100_def.h"
#endif


/***********************************************************************/
/*   ASIC polling counter (max setting)                                */
/***********************************************************************/
#define JL100_CMD_POLLING_CNT	0x0000ffff	/* max polling count for CMD register */

/***********************************************************************/
/*   access driver version                                             */
/***********************************************************************/
#define ACCESS_DRIVER_VERSION	0x01200003		/* Access driver version : 1.20  */
												/* Support type          : Multi Slave */

/***********************************************************************/
/*   function prototype                                                */
/***********************************************************************/
#ifdef __cplusplus
extern	"C"	{
#endif

extern void jl100s_init_channel( CHANNEL_INFO *chbuffp, ULONG *hif_reg_top, JL100_USER_IOMAP *usr_iomapp );
extern short jl100s_setup_asic(ULONG *hif_reg_top);
extern short jl100s_chk_asic_ready(ULONG *hif_reg_top);
extern short jl100s_set_usr_param(JL100_USER_PAR *usr_par, 
                           JL100_REG_PAR *reg_par, JL100_COM_PAR *com_par);
extern void  jl100s_reset_asic(CHANNEL_INFO *chbuffp);
extern short jl100s_set_regpar(CHANNEL_INFO *chbuffp, JL100_REG_PAR *reg_par);
extern short jl100s_write_compar(CHANNEL_INFO *chbuffp, JL100_COM_PAR *com_parp);
extern short jl100s_write_iomap(CHANNEL_INFO *chbuffp, JL100_USER_IOMAP *usr_iomapp);
extern short jl100s_req_set_par(CHANNEL_INFO *chbuffp);
extern short jl100s_initialize( CHANNEL_INFO *chbuffp, ULONG *hif_reg_top, 
                          JL100_USER_PAR *usr_parp, JL100_USER_IOMAP *usr_iomapp );
extern short jl100s_chk_detect_slv_cmp(CHANNEL_INFO *chbuffp);
extern short jl100s_req_measure_transdly(CHANNEL_INFO *chbuffp);
extern short jl100s_get_cmode(CHANNEL_INFO *chbuffp);
extern void  jl100s_get_trans_cycle(CHANNEL_INFO *chbuffp, ULONG *t_mcyc);
extern short jl100s_start_sync(CHANNEL_INFO *chbuffp);
extern short jl100s_start_async(CHANNEL_INFO *chbuffp);
extern short  jl100s_set_first_data(CHANNEL_INFO *chbuffp);
extern short jl100s_read_ram(ULONG *hif_reg_top, USHORT ofst_adr, USHORT size, ULONG *rdata);
extern short jl100s_write_ram(ULONG *hif_reg_top, USHORT ofst_adr, USHORT size, ULONG wdata);
extern short jl100s_activate_comprm(CHANNEL_INFO *chbuffp, JL100_USER_PAR *usr_parp,
                                                           JL100_USER_IOMAP *iomapp);
extern USHORT jl100s_read_iomap_member(CHANNEL_INFO *chbuffp, USHORT st_no, USHORT member);
extern void  jl100s_write_iomap_member(CHANNEL_INFO *chbuffp, USHORT st_no, USHORT member, USHORT data);
extern USHORT jl100s_read_compar_member(CHANNEL_INFO *chbuffp, USHORT member);
extern short jl100s_chk_iomap_prm(CHANNEL_INFO *chbuffp, JL100_USER_IOMAP *usr_iomapp);
extern short jl100s_begin_cyclic_com(CHANNEL_INFO *chbuffp);
extern short jl100s_chk_rcv_tmcframe(CHANNEL_INFO *chbuffp, ULONG *t_mcyc);
extern ULONG jl100s_chk_intrp_factor(CHANNEL_INFO *chbuffp);
extern void  jl100s_request_wdt(CHANNEL_INFO *chbuffp);
extern void  jl100s_async_rcv_enable(CHANNEL_INFO *chbuffp);
extern short jl100s_change_rbuff(CHANNEL_INFO *chbuffp);
extern short jl100s_change_cbuff(CHANNEL_INFO *chbuffp);
extern short jl100s_read_gbldata(CHANNEL_INFO *chbuffp, ULONG *rbuffp);
extern short jl100s_read_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *rbuffp);
extern short jl100s_read_other_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *rbuffp);
extern short jl100s_write_ldata(CHANNEL_INFO *chbuffp, USHORT st_no, ULONG *sbuffp);
extern short jl100s_req_rcv_frame(CHANNEL_INFO *chbuffp, USHORT *st_adr_da, USHORT *st_adr_sa, USHORT *rcv_stat, 
																     USHORT *ftype, ULONG *rbuffp, USHORT *size);
extern short jl100s_send_frame(CHANNEL_INFO *chbuffp, USHORT st_adr_da, USHORT st_adr_sa, 
                                                                 USHORT ftype, ULONG *sbuffp, USHORT size);
extern void  jl100s_get_crcvstat(CHANNEL_INFO *chbuffp, ULONG *crcvstat);
extern void  jl100s_get_rrcvstat(CHANNEL_INFO *chbuffp, ULONG *rcvstat);
extern void  jl100s_get_stistat(CHANNEL_INFO *chbuffp, ULONG *stistat);
extern ULONG jl100s_read_comstat(CHANNEL_INFO *chbuffp);
extern short jl100s_get_gblstat(CHANNEL_INFO *chbuffp);
extern short jl100s_get_cid(CHANNEL_INFO *chbuffp);
extern ULONG jl100s_get_accdrv_ver(void);
extern USHORT jl100s_get_rbuff_offset(CHANNEL_INFO *chbuffp, USHORT st_no);
extern USHORT jl100s_get_cbuff_offset(CHANNEL_INFO *chbuffp, USHORT st_no);
extern short jl100s_req_rcv_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT *st_adr_da, USHORT st_adr, USHORT *msg_size);
extern short jl100s_req_snd_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT st_adr, USHORT st_adr_sa, USHORT msg_size);
extern short jl100s_abort_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg);
extern short jl100s_read_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT offset, 
                                                                      USHORT size, ULONG *rdata);
extern short jl100s_write_msgdata(CHANNEL_INFO *chbuffp, USHORT cnmsg, USHORT offset, 
                                                                       USHORT size, ULONG *sdata);
extern ULONG jl100s_swaphl(ULONG i);
#ifdef __cplusplus
}
#endif
