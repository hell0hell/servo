/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント          */
/*                                                                          */
/*            レスポンス作成関数定義										*/
/*																			*/
/****************************************************************************/
/* ＩＦ定義関数 :                                                           */
/*																			*/
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*******************   Yaskawa Electric Corporation *************************/
#ifndef		RSP_FUNCDEF_H_
#define		RSP_FUNCDEF_H_


/* レスポンス作成関数定義 */
extern VOID rsp_SYS_CMD(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_NOP(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_PRM_RD(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_ID_RD(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_ALM_RD(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_ALM_CLR(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_CONNECT(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_POS_SET(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_ADJ(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);
extern VOID rsp_MON_CMD(RSP_HNDL *hdl, RSP_LINK_DATA* rsp_buf);

/* サブコマンド  レスポンス作成関数定義 */
extern VOID exRsp_SYS_CMD(RSP_HNDL *hdl, RSP_LINK_EXDATA* rsp_buf);
extern VOID exRsp_NOP(RSP_HNDL *hdl, RSP_LINK_EXDATA* rsp_buf);
extern VOID exRsp_PRM_RD(RSP_HNDL *hdl, RSP_LINK_EXDATA* rsp_buf);
extern VOID exRsp_ALM_RD(RSP_HNDL *hdl, RSP_LINK_EXDATA* rsp_buf);
extern VOID exRsp_MON_CMD(RSP_HNDL *hdl, RSP_LINK_EXDATA* rsp_buf);

#endif
