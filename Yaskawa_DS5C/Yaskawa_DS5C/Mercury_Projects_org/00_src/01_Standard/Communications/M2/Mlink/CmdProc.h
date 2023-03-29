/****************************************************************************/
/*																			*/
/*																			*/
/*		CMD Proc ヘッダファイル												*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		CMD_PROC_H_
#define		CMD_PROC_H_

/* コマンド解析関数定義 */
INT32 cmd_NO_DEFINE(void *hNetData);
INT32 cmd_NOP(void *hNetData);
INT32 cmd_PRM_RD(void *hNetData);
INT32 cmd_PRM_WR(void *hNetData);
INT32 cmd_ID_RD(void *hNetData);
INT32 cmd_CONFIG(void *hNetData);
INT32 cmd_ALM_RD(void *hNetData);
INT32 cmd_ALM_CLR(void *hNetData);
INT32 cmd_SYNC_SET(void *hNetData);
INT32 cmd_CONNECT(void *hNetData);
INT32 cmd_DISCONNECT(void *hNetData);
INT32 cmd_PPRM_WR(void *hNetData);
INT32 cmd_MEM_RD(void *hNetData);
INT32 cmd_POS_SET(void *hNetData);
INT32 cmd_BRK_ON(void *hNetData);
INT32 cmd_BRK_OFF(void *hNetData);
INT32 cmd_SENS_ON(void *hNetData);
INT32 cmd_SENS_OFF(void *hNetData);
INT32 cmd_HOLD(void *hNetData);
INT32 cmd_LTMOD_ON(void *hNetData);
INT32 cmd_LTMOD_OFF(void *hNetData);
INT32 cmd_SMON(void *hNetData);
INT32 cmd_SV_ON(void *hNetData);
INT32 cmd_SV_OFF(void *hNetData);
INT32 cmd_INTERPOLATE(void *hNetData);
INT32 cmd_POSING(void *hNetData);
INT32 cmd_FEED(void *hNetData);
INT32 cmd_LATCH(void *hNetData);
INT32 cmd_EX_POSING(void *hNetData);
INT32 cmd_ZRET(void *hNetData);
INT32 cmd_VELCTRL(void *hNetData);
INT32 cmd_TRQCTRL(void *hNetData);
INT32 cmd_ADJ(void *hNetData);
INT32 cmd_SVCTRL(void *hNetData);
INT32 cmd_NO_SUPP(void *hNetData);

/* サブコマンド解析関数定義 */
INT32 exCmd_NO_DEFINE(void *hNetData);
INT32 exCmd_NOP(void *hNetData);
INT32 exCmd_PRM_RD(void *hNetData);
INT32 exCmd_PRM_WR(void *hNetData);
INT32 exCmd_ALM_RD(void *hNetData);
INT32 exCmd_PPRM_WR(void *hNetData);
INT32 exCmd_LTMOD_ON(void *hNetData);
INT32 exCmd_LTMOD_OFF(void *hNetData);
INT32 exCmd_SMON(void *hNetData);
INT32 exCmd_NO_SUPP(void *hNetData);


#endif /* CMD_PROC_H_ */
