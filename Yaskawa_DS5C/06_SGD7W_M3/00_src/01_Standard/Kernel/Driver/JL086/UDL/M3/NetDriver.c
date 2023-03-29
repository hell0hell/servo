/****************************************************************************
Description	: MECHATROLINK Communication Driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions     :
	NdInit()						:JL-100初期化
	NdNormalCommunicationIsReady()	:通常通信レディチェック
	NdReceiveTmcFrame()				:TMCフレームの受信確認
	NdReceiveCMD()					:CMDデータ取得
	NdSendRSP()						:RSPデータ送信
	NdGetDevStatus()				:ASIC状態取得
	NdGetComMode()					:通信モード取得
	NdGetAsyncSw()					:非同期通信シーケンスフラグ取得
	NdExchangeAsyncData()			:イベントドリブン通信時の通信データ送受信処理
	NdAsyncRcvRequest()				:イベントドリブン通信時のフレーム受信要求セット
	NdReceiveAsyncCMD()				:イベントドリブン通信時の通信データ受信
	NdSendAsyncRSP()				:イベントドリブン通信時の通信データ送信

----------------------------------------------------------------------------
Changes		:
 Name   Date            Description
----------------------------------------------------------------------------
 Y.Oka	2010.02.23		created.
 I.M.   2011.01.18		reviewed. 

****************************************************************************/
#include "Basedef.h"
#include "NetDriver.h"
#include "jl100s_gbl.h"
#include "KLib.h"
#include "MLib.h"



/* JL-100 status definitions */
typedef enum ML_INITIAL_STATUS_ {
	ND_WAIT_INIT_CMP = 0,		/* 初期化処理完了待ち状態 */
	ND_INIT_PROC,				/* JL-100初期化処理状態 */
	ND_MEASURE_TRANSDLY,		/* 伝送遅延計測状態 */
	ND_CHK_CONNECT_SLVCMP,		/* 接続局検出完了状態 */
	ND_GET_CMODE_CMP,			/* 通信周期通知フレーム受信状態 */
	ND_RESET_ASIC_PROC,			/* 通信ASICリセット待ち状態 */
	ND_COM_READY_CMP,			/* 通常通信準備完了 */
}ML_INITIAL_STATUS;


/* definition of function pointer type */
typedef	INT32 (*ND_BEGIN_COM_FPTR)(ULONG *);
typedef	INT32 (*ND_READ_FPTR)(UINT, ULONG *);
typedef	void (*ND_WRITE_FPTRE)(UINT, ULONG *);

typedef struct ML_NDFUNC_DEF_ {
	ND_BEGIN_COM_FPTR 	NdBeginComm;
	ND_READ_FPTR		NdReadCycData;
	ND_WRITE_FPTRE		NdWriteCycData;
} ML_NDFUNC_DEF;



typedef struct MLDRV_CHINFO {
	CHANNEL_INFO		chbuff;						/* Channel buffer								*/
	JL100_USER_PAR		usr_par;					/* Communication parameters						*/
	JL100_USER_IOMAP	usr_io_map[DEF_MA_MAX+1];	/* IO MAP Parameters(Slave 2node)				*/
	ND_PARAM			nd_prm;						/* 通信パラメータ								*/
	ASYNC_ND_PARAM 		async_nd_parm;				/* イベントドリブン通信パラメータ				*/

	ML_INITIAL_STATUS	InitialStep;				/* Step number of the M3 Initial sequence */
	ULONG				startTime;					/* ASICリセット開始時間					*/
	SHORT				comRdyErrInfo;				/* JL100通常通信開始時の異常情報ポインタ*/

	/* for Cyclic Communications */
	ULONG				crcvstat[2];				/* 受信ステータス			*/
	ULONG				RcvAltBufAddr;
	ULONG				SndAltBufAddr;
	ULONG				RcvBufAddrOfs[DEF_MA_MAX+1];
	ULONG				SndBufAddrOfs[DEF_MA_MAX+1];
	ULONG				DmaRcvBuffer[DEF_MA_MAX+1][DEF_ASYNC_DATA_SIZE/4];
	ULONG				DmaSndBuffer[DEF_MA_MAX+1][DEF_ASYNC_DATA_SIZE/4];

	ULONG				RcvAltBufOfsAddr[2];
	ULONG				SndAltBufOfsAddr[2];

	/* for Message Communications */
	USHORT				crcv_msgsz1;				/* C1 Receive message data size buffer	*/
	USHORT				crcv_msgsz2;				/* C2 Receive message data size buffer	*/
	USHORT				csnd_msgsz;					/* Send message data size buffer		*/
	USHORT				c1msg_peer_multi;			/* for multi slave message				*/
	USHORT				c2msg_peer_multi;			/* for multi slave message				*/

	const ML_NDFUNC_DEF	*NdFunc;
} MLDRV_CHINFO;

/* Local functions */
static INT32 nd_start_normal_comunication( CHANNEL_INFO *chbuffp, USHORT *t_mcycp );
static VOID nd_set_usr_prm(JL100_USER_PAR	*usr_parp, 
				JL100_USER_IOMAP *usr_iomapp, ND_PARAM *nd_prmp );
static SHORT nd_ram_check( ULONG *pJL, USHORT size, ULONG chkdata );
static void nd_set_cycbuf_addr(MLDRV_CHINFO	*MLDevice, CHANNEL_INFO *chbuffp);

static INT32 NdBeginCyclicCommunication( ULONG *ests );
static INT32 NdReceiveCyclicCMD( UINT st_no, ULONG *CmdBuffer );
static void NdSendCyclicRSP( UINT st_no, ULONG *RspBuffer );

static INT32 NdBeginAsyncCommunication( ULONG *ests );
static INT32 NdReceiveAsyncCMD( UINT st_no, ULONG *CmdBuffer );
static void NdSendAsyncRSP(UINT st_no, ULONG *async_rbuff );


/* Local variables */
static MLDRV_CHINFO		MLDevice;		/* ML Device driver information */

const ML_NDFUNC_DEF ML_CyclicFunc = {
                                     &NdBeginCyclicCommunication,
                                     &NdReceiveCyclicCMD,
                                     &NdSendCyclicRSP
};
const ML_NDFUNC_DEF ML_AsyncFunc = {
                                     &NdBeginAsyncCommunication,
                                     &NdReceiveAsyncCMD,
                                     &NdSendAsyncRSP
};


/********************************************************************************/
/*																				*/
/*	関数名 :	NdInit()														*/
/*																				*/
/*	機能概要：																	*/
/*		MECHATROLINK-3通信の初期ASICチェック処理								*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		ND_PARAM* nd_prmp		通信パラメータ（アプリ設定）のポインタ			*/
/*		ULONG devOpt			ASICの種別を指定（未使用）						*/
/*	戻り値 :																	*/
/*		= ND_SUCCESS			正常終了										*/
/*		= ND_SRAM_CHECK_ERROR	JL-100内臓RAMチェックサム異常					*/
/*		= ND_JL100_READY_TIMEOUTJL-100内臓RAMチェックサム異常					*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	Note	:	通信初期化処理はこの関数が正常終了し、							*/
/*				NdIsNormalCommunicationReady()=ND_SUCCESSとなることで完了となる	*/
/*	Log		:																	*/
/*																				*/
/********************************************************************************/
INT32 NdInit( ND_PARAM *nd_prmp, void *DeviceAddress )
{
	ML_INITIAL_STATUS	*init_stsp;		/* JL100初期化状態						*/
	USHORT				*async_swp;		/* 非同期通信シーケンス番号				*/
	ULONG				*pJL;			/* JL-100 register top address.			*/
	ND_PARAM			*ndprmp;		/* 通信パラメータ（保存用）のポインタ	*/
	CHANNEL_INFO		*chbuffp;		/* JL-100アクセス定義ポインタ			*/
	JL100_USER_PAR		*usr_parp;		/* JL100_USER_PARポインタ				*/
	JL100_USER_IOMAP	*usr_io_mapp;	/* JL100_USER_IOMAPポインタ				*/
	VULONG				Timer;			

	MlibResetByteMemory(&MLDevice, sizeof(MLDevice));
	/* Get the pointer of Buffer */
	init_stsp	= &MLDevice.InitialStep;
	async_swp   = &MLDevice.async_nd_parm.sw;
	ndprmp		= &MLDevice.nd_prm;
	chbuffp		= &MLDevice.chbuff;
	usr_parp	= &MLDevice.usr_par;
	usr_io_mapp = MLDevice.usr_io_map;
	MLDevice.NdFunc = &ML_AsyncFunc;

	/* Set JL-100 register top address */
	pJL = (ULONG*)DeviceAddress;
	chbuffp->hif_reg_top = (JL100_HOST_IF_REGS *)DeviceAddress;

#ifdef _VC_DEBUG_
	return ND_SUCCESS;
#endif

	/* jl100s_init_sts Initialization */
	*init_stsp = ND_WAIT_INIT_CMP;
	/* 非同期シーケンス番号初期化 */
	*async_swp = ND_ASYNC_REQ_RCV_SEQ;
	/* 通信パラメータの値を保存 */
	*ndprmp = *nd_prmp;

	/* M-III Core Software Reset */
	jl100s_reset_asic( chbuffp );		/* <S01A> */

	/* JL-100 Set up */
	Timer = 0;
	while( WAIT_SETUP == jl100s_setup_asic( pJL ))
	{
		KlibWaitms(1);
		Timer++;
		if( Timer > JL100_READY_TIMEOUT )
		{/* タイムアウト 10[s] */
			return ND_JL100_READY_TIMEOUT;
		}
	}

	/* JL-100起動完了待ち */
	Timer = 0;
	while( LIB_OK != jl100s_chk_asic_ready(pJL) )
	{
		KlibWaitms(1);
		Timer++;
		if( Timer > JL100_READY_TIMEOUT )
		{/* タイムアウト 10[s] */
			return ND_JL100_READY_TIMEOUT;
		}
	}

	/* Check & Clear JL-100 RAM */
	if( nd_ram_check( pJL, DEF_SRAM_SIZE, 0x5a5a5a5a) != ND_SUCCESS )
	{
		return ND_SRAM_CHECK_ERROR;
	}
	if( nd_ram_check( pJL, DEF_SRAM_SIZE, 0xa5a5a5a5) != ND_SUCCESS )
	{
		return ND_SRAM_CHECK_ERROR;
	}
	if( nd_ram_check( pJL, DEF_SRAM_SIZE, 0x00000000) != ND_SUCCESS )
	{
		return ND_SRAM_CHECK_ERROR;
	}

	/* ユーザで設定が必要な通信パラメータの設定 */
	nd_set_usr_prm( usr_parp, usr_io_mapp, nd_prmp );
	/* JL-100 Initialization */
	if( jl100s_initialize(chbuffp, pJL, usr_parp, usr_io_mapp) == LIB_OK )
	{
		nd_set_cycbuf_addr(&MLDevice, chbuffp);
		*init_stsp = ND_MEASURE_TRANSDLY;
	}
	else
	{
		*init_stsp = ND_INIT_PROC;
	}
	
	return ND_SUCCESS;
}

/********************************************************************************/
/*																				*/
/*	関数名 :	NdNormalCommunicationReady()									*/
/*																				*/
/*	機能概要：																	*/
/*		MECHATROLINK-3通信の通常通信準備処理									*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		USHORT* t_mcycp			伝送周期[0.25us]のポインタ						*/
/*		ULONG*  t_int_ofs		Interrupt Delay Time							*/
/*	戻り値 :																	*/
/*		= ND_SUCCESS			正常終了										*/
/*		= ND_CONTINUE			準備処理実行中									*/
/*		= ND_COMM_STAT_RESET	通信状態再設定要求								*/
/*		= 上記以外				準備処理失敗									*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	Note	:	本関数はNdInit()関数の正常終了後実行すること					*/
/*				本関数は低レベルのタスクで常時実行すること						*/
/*	Log		:	関数の第二引数をSGDV-MDにあわせる <S0EC>	 					*/
/*																				*/
/********************************************************************************/
INT32 NdNormalCommunicationIsReady( USHORT *t_mcycp, ULONG *t_int_ofs )
{
	INT32				ret;			/* return code							*/
	ULONG				*pJL;			/* JL-100 register top address			*/
	CHANNEL_INFO		*chbuffp;		/* JL-100アクセス定義ポインタ			*/
	JL100_USER_PAR		*usr_parp;		/* JL100_USER_PARポインタ				*/
	JL100_USER_IOMAP	*usr_io_mapp;	/* JL100_USER_IOMAPポインタ				*/
	ND_PARAM			*nd_prmp;		/* 通信パラメータ						*/
	ULONG				t_mcyc_10ns;	/* 伝送周期 10ns						*/

	/* Get the pointer of Buffer */
	chbuffp			= &MLDevice.chbuff;
	usr_parp		= &MLDevice.usr_par;
	usr_io_mapp 	= MLDevice.usr_io_map;
	nd_prmp			= &MLDevice.nd_prm;

	/* Set JL-100 register top address */
	pJL = (ULONG*)chbuffp->hif_reg_top;

	switch( MLDevice.InitialStep )
	{
	case ND_INIT_PROC:
		/* ユーザで設定が必要な通信パラメータの設定 */
//		nd_set_usr_prm( usr_parp, usr_io_mapp, nd_prmp );
		/* JL-100 Initialization */
		if( jl100s_initialize(chbuffp, pJL, usr_parp, usr_io_mapp) == LIB_OK )
		{
//			nd_set_cycbuf_addr(&MLDevice, chbuffp);
			MLDevice.InitialStep = ND_MEASURE_TRANSDLY;
			ret = ND_CONTINUE;
		}
		else
		{
			ret = ND_JL100_INIT_ERROR;
		}
		break;

	case ND_MEASURE_TRANSDLY:
		/* Check complete detect connecting slave */
		ret = jl100s_chk_detect_slv_cmp( chbuffp );
		if( ret == LIB_OK )
		{
			/* Excute measure transmission delay time */
			ret = jl100s_req_measure_transdly( chbuffp );
			if(ret == LIB_OK)
			{
				MLDevice.InitialStep = ND_CHK_CONNECT_SLVCMP;
				ret = ND_CONTINUE;
			}
			else
			{
				ret =  ND_MEASURE_TRANSDLY_ERROR;
			}
		}
		else if( ret == WAIT_CMP_DETECT )
		{
			// 上位コントローラがいつオンラインになるかわからないので
			// 処理時間オーバのチェックは行わない！！
			ret = ND_CONTINUE;
		}
		else
		{
//			MLDevice.InitialStep = ND_INIT_PROC;
			ret = ND_DETECT_SLV_CMP_ERROR;
		}

		break;

	case ND_CHK_CONNECT_SLVCMP:
		/* Get communication mode */
		ret = jl100s_get_cmode(chbuffp);
		if( ret == LIB_OK )
		{
			MLDevice.InitialStep = ND_GET_CMODE_CMP;
			ret = ND_CONTINUE;
		}
		else if( (ret == WAIT_TRANSCYC_FRM) || (ret == MEASURING_TRANSDLY) )
		{
			// 上位コントローラがいつオンラインになるかわからないので
			// 処理時間オーバのチェックは行わない！！
			ret = ND_CONTINUE;
		}
		else if(ret == ERROR_RESET_ASIC)
		{
			jl100s_reset_asic( chbuffp );
			KlibRstLongTimer(&MLDevice.startTime);
			MLDevice.InitialStep = ND_RESET_ASIC_PROC;
			ret = ND_CONTINUE;
		}
		else
		{
			ret = ND_GET_CMODE_ERROR;
		}
		break;

	case ND_GET_CMODE_CMP:
		ret = nd_start_normal_comunication( chbuffp, &nd_prmp->TCycle);
		if(ret == ND_SUCCESS)
		{
			MLDevice.InitialStep = ND_COM_READY_CMP;
			*t_int_ofs = jl100_get_idly(chbuffp->hif_reg_top); /* <S0EC> */
		}
		*t_mcycp = nd_prmp->TCycle;
		break;

	case ND_RESET_ASIC_PROC:
		/* Check ASIC ready (ASIC ready状態になるまで待つ) */
		if(( jl100s_chk_asic_ready(pJL) ) == LIB_OK)
		{
			MLDevice.InitialStep = ND_INIT_PROC;
			ret = ND_CONTINUE;
		}
		else
		{
			ret = ND_CONTINUE;
			if( KlibGetLongTimerMs(MLDevice.startTime) > JL100_READY_TIMEOUT )
			{/* タイムアウト */
				ret = ND_JL100_READY_TIMEOUT;
			}
		}
		break;

	case ND_COM_READY_CMP:
		/* TMCフレーム受信状態確認 */
		ret = ND_CONTINUE;
		if( RCVED_TMC_FRAME == jl100s_chk_rcv_tmcframe(chbuffp, &t_mcyc_10ns) )
		{ /* TMCフレームを受信したら */
			MLDevice.nd_prm.TCycle = (USHORT)( t_mcyc_10ns / 25 );	/* 単位変換 10ns → 250ns */
			MLDevice.InitialStep = ND_GET_CMODE_CMP;
			ret = ND_RCVED_TMC_FRAME;
		}
		else
		{
			/* Asic ready check */
			if(( jl100s_chk_asic_ready(pJL) ) != LIB_OK)
			{
				ret = ND_JL100_READY_TIMEOUT;
			}
		}
		break;

	default:
		MLDevice.InitialStep = ND_INIT_PROC;
		ret = ND_INIT_SEQ_ERROR;
		break;
	}
	
	/* 異常情報の保存 */
	if( ret < 0 )
	{
		MLDevice.comRdyErrInfo = (SHORT)ret;
	}

	return ret;
}


/****************************************************************************/
/* 概要：通信モードのチェックを行い、同期通信開始処理若しくは				*/
/*		 非同期通信開始処理を開始する										*/
/****************************************************************************/
/* 引数		：	CHANNEL_INFO* chbuffp	JL-100アクセス定義ポインタ			*/
/*				USHORT* t_mcycp			伝送周期[0.25us]					*/
/*				USHORT* init_stsp		JL100初期化状態						*/
/*				ULONG* start_time		JL100リセット開始時間				*/
/* 戻り値   ：	= ND_SUCCESS			通信初期化処理完了					*/
/*				= ND_CONTINUE			処理継続中							*/
/*				= 上記以外				異常終了							*/
/****************************************************************************/
static INT32 nd_start_normal_comunication( CHANNEL_INFO *chbuffp, USHORT *t_mcycp )
{
	INT32	ret,i;
	ULONG	t_mcyc_10ns;						/* 伝送周期 10ns			*/

	/* Get transmission cycle */
	jl100s_get_trans_cycle( chbuffp, &t_mcyc_10ns );
	*t_mcycp = (USHORT)(t_mcyc_10ns/25);		/* 単位変換 10ns → 250ns	*/
	/* 2009.09.01 Y.Oka 現状未対応 */

	MLDevice.crcvstat[0] = 0;
	/* Start cyclic communication */
	if(chbuffp->prot_sel == COM_MODE_SYNC)
	{
		if(LIB_OK != jl100s_start_sync(chbuffp))
		{
			ret = ND_START_SYNC_ERROR;
		}
		else
		{
			MLDevice.NdFunc = &ML_CyclicFunc;
			ret = ND_SUCCESS;
		}
	}
	else if(chbuffp->prot_sel == COM_MODE_ASYNC)
	{
		MLDevice.async_nd_parm.sw = ND_ASYNC_REQ_RCV_SEQ;
		for(i = 0; i < (DEF_ASYNC_DATA_SIZE/4); i++)
		{	MLDevice.DmaRcvBuffer[0][i] = 0;}

//		ret = ND_SUCCESS;
		if(LIB_OK != jl100s_start_async(chbuffp))
		{
			ret = ND_START_ASYNC_ERROR;
		}
		else
		{
			MLDevice.NdFunc = &ML_AsyncFunc;
			ret = ND_SUCCESS;
		}
	}
	else
	{	/* 未サポート通信モード */
		ret = ND_PORT_SEL_ERROR;
	}
	return ret;
}

#if 0
/********************************************************************************/
/*																				*/
/*	関数名 :	NdReceiveTmcFrame()												*/
/*																				*/
/*	機能概要：																	*/
/*		通常運転状態でTMCフレームの受信確認を行う。								*/
/*	TMCフレームの受信確認により上位コントローラの通信リセットを検出する			*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*	戻り値 :																	*/
/*		= ND_RCVED_TMC_FRAME	TMCフレーム受信									*/
/*		= ND_NOT_RCV_TMC_FRAME	TMCフレーム未受信								*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	Note	:	本関数はM3通信状態で実行すること								*/
/*				本関数は低レベルのタスクで常時実行すること						*/
/*	Log		:																	*/
/*																				*/
/********************************************************************************/
SHORT NdReceiveTmcFrame( VOID )
{
	SHORT 			ret;					/* return code						*/
	ULONG			t_mcyc_10ns;			/* return code						*/
	CHANNEL_INFO	*chbuffp;				/* JL-100アクセス定義ポインタ		*/

	/* ローカル変数の初期化 */
	chbuffp		= &MLDevice.chbuff;

	/* TMCフレーム受信状態確認 */
	ret = jl100s_chk_rcv_tmcframe( chbuffp, &t_mcyc_10ns );

	if( ret == RCVED_TMC_FRAME )
	{ /* TMCフレームを受信したら */
		MLDevice.nd_prm.TCycle = (USHORT)( t_mcyc_10ns / 25 );	/* 単位変換 10ns → 250ns */
		MLDevice.InitialStep = ND_GET_CMODE_CMP;
		MLDevice.async_nd_parm.sw = ND_ASYNC_REQ_RCV_SEQ;
		MLDevice.crcvstat[0] = 0;
		ret = ND_RCVED_TMC_FRAME;
	}
	return ret;
}
#endif


/********************************************************************************/
/*																				*/
/*	関数名 :	サイクリック/イベントドリブン通信処理							*/
/*																				*/
/********************************************************************************/
INT32 NdBeginCommunication( ULONG *ests )
{
	return (MLDevice.NdFunc)->NdBeginComm(ests);
}

INT32 NdReceiveCMD( UINT st_no, ULONG *Buffer )
{
	return (MLDevice.NdFunc)->NdReadCycData(st_no, Buffer);
}

void NdSendRSP( UINT st_no, ULONG *Buffer )
{
	(MLDevice.NdFunc)->NdWriteCycData(st_no, Buffer);
}



/********************************************************************************/
/*																				*/
/*	関数名 :	NdReceiveCyclicCMD()											*/
/*																				*/
/*	機能概要:																	*/
/*		指令データの受信処理（同期通信）										*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		ULONG* lnk_cbuff		指令データの受信バッファ						*/
/*	戻り値 :																	*/
/*		INT32 					受信異常軸数									*/
/*																				*/
/********************************************************************************/
static INT32 NdReceiveCyclicCMD( UINT st_no, ULONG *CmdBuffer )
{
	INT32 			ret;					/* return code						*/
	CHANNEL_INFO	*chbuffp;				/* JL-100アクセス定義ポインタ		*/
	ULONG			*rdp;					/* for RAM access */
	USHORT			size_buf;				/* Responce data length from IOMAP */

	/* Receive [JL-100 -> buffer] */
	if(0 != (MLDevice.crcvstat[0] & ( 1 << st_no )))
	{/* CMDデータ正常受信 */
		/* Get response data length */
		chbuffp = &MLDevice.chbuff;
		size_buf = chbuffp->usr_iomap[st_no].cd_len;

		/* Read receive data */
#if (CSW_COMASIC_DMA_TRANSFER == FALSE)
		rdp = (ULONG *)(MLDevice.RcvAltBufAddr + MLDevice.RcvBufAddrOfs[st_no]);
#else
		rdp = MLDevice.DmaRcvBuffer[st_no];
#endif
		while(size_buf > 0)
		{
			*CmdBuffer = jl100_read_dwdata(rdp);
			CmdBuffer++;
			size_buf -= DWORD_SIZE;
		}
		ret = ND_SUCCESS;
	}
	else
	{
		ret = ND_DATA_RECV_ERROR;
	}

	return ret;
}


/********************************************************************************/
/*																				*/
/*	関数名 :	NdSendCyclicRSP()												*/
/*																				*/
/*	機能概要:																	*/
/*		応答データ送信処理（同期通信）											*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		INT ch					チャンネル番号									*/
/*		ULONG* lnk_rbuff		送信データの送信バッファ						*/
/*	戻り値 :																	*/
/*		= ND_SUCCESS			正常終了										*/
/*		= 上記以外				応答データ書き込み失敗							*/
/*																				*/
/********************************************************************************/
void NdSendCyclicRSP( UINT st_no, ULONG *RspBuffer )
{
	CHANNEL_INFO	*chbuffp;					/* JL-100アクセス定義ポインタ	*/
	ULONG	*wtp;						/* for RAM access */
	USHORT	size_buf;					/* Command data length from IOMAP */

	chbuffp = &MLDevice.chbuff;
//	jl100s_write_ldata( chbuffp, (USHORT)st_no, (ULONG*)RspBuffer );

	size_buf = chbuffp->usr_iomap[st_no].rd_len;
	/* Write command data */
#if (CSW_COMASIC_DMA_TRANSFER == FALSE)
	wtp = (ULONG *)(MLDevice.SndAltBufAddr + MLDevice.SndBufAddrOfs[st_no]);
#else
	wtp = MLDevice.DmaSndBuffer[st_no];
#endif
	while(size_buf > 0)
	{
		jl100_write_dwdata(wtp, RspBuffer);
		size_buf -= DWORD_SIZE;
	}
}

/********************************************************************************/
/*																				*/
/*	関数名 :	NdGetDevStatus()												*/
/*																				*/
/*	機能概要:																	*/
/*		サイクリック通信開始 & 通信状態取得処理（ASIC状態）						*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		ULONG *ests				エラーステータス								*/
/*	戻り値 :																	*/
/*		= ND_SUCCESS				正常終了									*/
/*																				*/
/********************************************************************************/
static INT32 NdBeginCyclicCommunication( ULONG *ests )
{
	ULONG			i;
	VULONG			asic_stat;
	CHANNEL_INFO	*chbuffp;		/* JL-100アクセス定義ポインタ				*/
	JL100_HOST_IF_REGS *hif_reg_top;

	chbuffp = &MLDevice.chbuff;
	hif_reg_top = chbuffp->hif_reg_top;

	/* Request change receive buffer */
	jl100_set_cmd(hif_reg_top, (JL100_CMD_CBX_R|JL100_CMD_RBX_R|JL100_CMD_WD_R));
	/* Clear interrupt factor */
	jl100_clear_ints(hif_reg_top, JL100_INTS_TIM1);

	/* Toggle response buffer */
//	jl100s_change_cbuff( chbuffp );
//	jl100s_change_rbuff( chbuffp );
//	jl100s_begin_cyclic_com( chbuffp );

	/* Get ESTS register */
	asic_stat = jl100s_read_comstat(chbuffp);
	*ests = asic_stat & (JL100_ESTS_WDTOVR | JL100_ESTS_RXLEN_ERR | JL100_ESTS_FCS_ERR);

	/* Get command receive status */
	MLDevice.crcvstat[0] = jl100_get_crcv1(hif_reg_top);
//	jl100s_get_crcvstat(chbuffp, MLDevice.crcvstat);

	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		asic_stat = jl100_get_cmd(hif_reg_top);
		if((asic_stat & (JL100_CMD_CBX_R|JL100_CMD_RBX_R)) == 0)
		{
			break;
		}
	}
	MLDevice.RcvAltBufAddr = (USHORT)jl100_get_cbadr(hif_reg_top);
	MLDevice.SndAltBufAddr = (USHORT)jl100_get_rbadr(hif_reg_top);

	return (MLDevice.crcvstat[0] & 1);
}

/****************************************************************************/
/* 概要：伝送周期取得処理													*/
/****************************************************************************/
/* 引数		：	なし														*/
/* 戻り値   ：	伝送周期[0.25us]											*/
/****************************************************************************/
USHORT NdGetTransmissionCycle( VOID )
{
	return MLDevice.nd_prm.TCycle;
}

/****************************************************************************/
/* 概要：通信モード取得処理													*/
/****************************************************************************/
/* 引数		：	なし														*/
/* 戻り値   ：	COM_MODE_SYNC	サイクリック通信モード						*/
/*			：	COM_MODE_ASYNC	イベントドリブン通信モード					*/
/*			：	上記以外		通信モード異常設定							*/
/****************************************************************************/
USHORT NdGetComMode( VOID )
{
//	CHANNEL_INFO	*chbuffp;		/* JL-100アクセス定義ポインタ */
//	chbuffp = &MLDevice.chbuff;				/* ポインタにアドレスをセット */

//	return chbuffp->prot_sel;
	return ((MLDevice.NdFunc == &ML_AsyncFunc) ? COM_MODE_ASYNC : COM_MODE_SYNC);
}

#if 0
/****************************************************************************/
/* 概要：非同期通信シーケンスフラグ取得処理									*/
/****************************************************************************/
/* 引数		：	なし														*/
/* 戻り値   ：	ASYNC_RCV_SEQ	指令データ受信シーケンス					*/
/*			：	ASYNC_SND_SEQ	応答データ送信シーケンス					*/
/****************************************************************************/
USHORT NdGetAsyncSw( VOID )
{
	ASYNC_ND_PARAM* asy_prmp;	/* イベントドリブン通信パラメータ */
	asy_prmp = &MLDevice.async_nd_parm;	/* ポインタにアドレスをセット */

	return asy_prmp->sw;
}

/****************************************************************************/
/* 概要：非同期通信シーケンスフラグ設定処理									*/
/****************************************************************************/
/* 引数		：	非同期通信シーケンスフラグ									*/
/* 戻り値   ：	なし														*/
/****************************************************************************/
VOID NdSetAsyncSw( USHORT sw )
{
	ASYNC_ND_PARAM* asy_prmp;	/* イベントドリブン通信パラメータ */
	asy_prmp = &MLDevice.async_nd_parm;	/* ポインタにアドレスをセット */

	asy_prmp->sw = sw;
}
#endif

/****************************************************************************/
/* 概要：スレーブ用の通信パラメータを設定する								*/
/****************************************************************************/
/* 引数		：	JL100_USER_PAR	*usr_parp	JL-100ユーザパラメータのポインタ*/
/*				JL100_USER_IOMAP* usr_iomapp JL-100ユーザI/Oマップのポインタ*/
/*				ND_PARAM* nd_prmp	通信パラメータ（アプリ設定）のポインタ	*/
/* 戻り値   ：	なし														*/
/****************************************************************************/
static VOID nd_set_usr_prm(JL100_USER_PAR *usr_parp, 
							JL100_USER_IOMAP *usr_iomapp, ND_PARAM *nd_prmp )
{
	LONG ax_no;

	/* set default user parameter setting */
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
	usr_parp->mod = JL100_MOD_TYPE_SLV | JL100_MOD_INT_FR | JL100_MOD_INTLV_PLS;
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
	usr_parp->mod = JL100_MOD_TYPE_MULTISLV | JL100_MOD_INT_FR | JL100_MOD_INTLV_PLS;
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE */
	usr_parp->ma0 = nd_prmp->NodeAddress;			/* My Address from switch */
	usr_parp->ma_max = MAX_AXIS_NUMBER;				/* MAX slave node  */
//	usr_parp->wdt = DEF_WDT;						/* Watch dog timer [8usec]; if wdt=0, WDT function disabled */
	usr_parp->wdt = WDT_NOTCHK;						/* <2008.10.22 Y.Oka (>_<) ASICでホストCPUのWDTをチェックしない設定> */
	usr_parp->pkt_sz = MSG_PKT_SIZE;				/* Message communication packet size */
	usr_parp->max_rtry = 0;							/* Reserve (default:0) */
	usr_parp->c2_dly   = 0;							/* Reserve (default:0) */
	usr_parp->dly_cnt  = 0;							/* Reserve (default:0) */

	/* Set IOMAP parameters(C1 master) */
	usr_iomapp->axis_adr = 0x0001;					/* Station address */
	usr_iomapp->t_rsp  = 0;							/* Reserve(default:0) */
	usr_iomapp->cd_len = 8;							/* Command data length */
	usr_iomapp->rd_len = 8;							/* Response data length */

	/* Set IOMAP prameters(slave) */
	for(ax_no = 1; ax_no <= MAX_AXIS_NUMBER; ax_no++)
	{
		(usr_iomapp+ax_no)->axis_adr = 
			(USHORT)(((ax_no-1) << 8) | nd_prmp->NodeAddress);	/* Station address */
		(usr_iomapp+ax_no)->t_rsp = 0;							/* reserve : default 0 */
		(usr_iomapp+ax_no)->cd_len = nd_prmp->DataSize;			/* Commando data length */
		(usr_iomapp+ax_no)->rd_len = nd_prmp->DataSize;			/* Response data length */
	}
}

static void nd_set_cycbuf_addr(MLDRV_CHINFO	*MLDevice, CHANNEL_INFO *chbuffp)
{
	LONG			ax_no;

	chbuffp->usr_iomap[0].cmdbuff_offset = 0;	/* <S0EC> */
	chbuffp->usr_iomap[0].rspbuff_offset = 0;

	/* Set IOMAP prameters(slave) */
	for(ax_no = 0; ax_no <= MAX_AXIS_NUMBER; ax_no++)
	{
		MLDevice->RcvBufAddrOfs[ax_no] = ((ULONG)chbuffp->hif_reg_top + DEF_SRAM_TOP_ADR);
		MLDevice->RcvBufAddrOfs[ax_no] += chbuffp->usr_iomap[ax_no].cmdbuff_offset;
		MLDevice->RcvBufAddrOfs[ax_no] += HEADER_SIZE;

		MLDevice->SndBufAddrOfs[ax_no] = ((ULONG)chbuffp->hif_reg_top + DEF_SRAM_TOP_ADR);
		MLDevice->SndBufAddrOfs[ax_no] += chbuffp->usr_iomap[ax_no].rspbuff_offset;
		MLDevice->SndBufAddrOfs[ax_no] += HEADER_SIZE;
	}
}

/****************************************************************************/
/* 概要：JL-100 初期化処理用トグルアドレス更新処理							*/
/****************************************************************************/
/* 引数		：	なし														*/
/* 戻り値   ：	0以外：エラー												*/
/****************************************************************************/
INT32 NdChangeTglBufAddress(ULONG *RcvAltBufOfst, ULONG *SndAltBufOfst, UINT alt_no)
{
	INT32			ret1, ret2;
	CHANNEL_INFO	*chbuffp;

	chbuffp	= &MLDevice.chbuff;

	/* Toggle buffer */
	ret1 = jl100s_change_rbuff( chbuffp );
	ret2 = jl100s_change_cbuff( chbuffp );
	MLDevice.RcvAltBufAddr = (USHORT)jl100_get_cbadr(chbuffp->hif_reg_top);
	MLDevice.SndAltBufAddr = (USHORT)jl100_get_rbadr(chbuffp->hif_reg_top);
	MLDevice.RcvAltBufOfsAddr[alt_no] = MLDevice.RcvAltBufAddr;
	MLDevice.SndAltBufOfsAddr[alt_no] = MLDevice.SndAltBufAddr;

	if(SndAltBufOfst != NULL)
	{	
		*SndAltBufOfst = MLDevice.SndAltBufAddr;
	}
	if(RcvAltBufOfst != NULL)
	{	
		*RcvAltBufOfst = MLDevice.RcvAltBufAddr;
	}

	return (ret1|ret2);
}

/****************************************************************************/
/* 概要：JL-100 サイクリックバッファアドレス取得処理						*/
/****************************************************************************/
/* 引数		：	UINT ch					チャンネル番号						*/
/*				ULONG *AltBufOfst		トグルバッファオフセットアドレス	*/
/* 戻り値   ：	バッファアドレス											*/
/****************************************************************************/
void *NdGetRecvCycDevBufAddress(UINT ch)
{
	return (void*)(MLDevice.RcvAltBufAddr + MLDevice.RcvBufAddrOfs[ch]);
}

void *NdGetSndCycDevBufAddress(UINT ch)
{
	return (void*)(MLDevice.SndAltBufAddr + MLDevice.SndBufAddrOfs[ch]);
}

void *NdGetRecvCycBufferPointer(UINT ch)
{
	return (void*)MLDevice.DmaRcvBuffer[ch];
}

void *NdGetSndCycBufferPointer(UINT ch)
{
	return (void*)MLDevice.DmaSndBuffer[ch];
}

ULONG NdGetRecvAltBufferAddress(void)
{
	return ((MLDevice.RcvAltBufOfsAddr[0] == MLDevice.RcvAltBufAddr) ? 0 : 1);
//	return (MLDevice.RcvAltBufOfsAddr[0] != MLDevice.RcvAltBufAddr);
}
ULONG NdGetSndAltBufferAddress(void)
{
//	return (MLDevice.SndAltBufOfsAddr[0] != MLDevice.SndAltBufAddr);
	return ((MLDevice.SndAltBufOfsAddr[0] == MLDevice.SndAltBufAddr) ? 0 : 1);
}
/****************************************************************************/
/* 概要：JL-100 内臓RAMチェック処理											*/
/****************************************************************************/
/* 引数		：	ULONG *pJL		JL100 register top address					*/
/*				USHORT size		SRAM データサイズ							*/
/*				ULONG  chkdata	チェックデータ								*/
/* 戻り値   ：	なし														*/
/****************************************************************************/
static SHORT nd_ram_check( ULONG *pJL, USHORT size, ULONG chkdata )
{
	VULONG	*pVal;			/* Host I/F Top address							*/
	ULONG	rdata;			/* Read data									*/
	USHORT	ret;			/* return code									*/
	int		i;				/* loop counter									*/

	/* Set JL-100 register top address */
	pVal = (VULONG*)pJL;

	/* Write data */
	ret = jl100s_write_ram( pJL, 0, size, chkdata );
	if(ret != LIB_OK)	return ret;

	/* Read and verify writed data */
	for( i = 0; i < size; i+=4 )
	{
		ret = jl100s_read_ram( (ULONG*)pVal, i, 4, &rdata );
		if(ret != LIB_OK)	return ret;

		if(rdata != chkdata)
			return ND_SRAM_CHECK_ERROR;
	}
	return ND_SUCCESS;
}

//#ifdef M3FSEL_EVENT_DRIVEN_ENABLE
/********************************************************************************/
/*																				*/
/*	関数名 :	NdGetDevStatus()												*/
/*																				*/
/*	機能概要:																	*/
/*		サイクリック通信開始 & 通信状態取得処理（ASIC状態）						*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		ULONG *ests				エラーステータス								*/
/*	戻り値 :																	*/
/*		= ND_SUCCESS				正常終了									*/
/*																				*/
/********************************************************************************/
static INT32 NdBeginAsyncCommunication( ULONG *ests )
{
//	ULONG			asic_stat;
//	CHANNEL_INFO	*chbuffp;		/* JL-100アクセス定義ポインタ				*/

//	chbuffp = &MLDevice.chbuff;

//	jl100s_begin_cyclic_com( chbuffp );

	/* Get ESTS register */
//	asic_stat = jl100s_read_comstat(chbuffp);

	MLDevice.crcvstat[0] = 0;
	*ests = 0;

//	JL-102のWDT機能は未使用なので処理不要 /* <S0EC> */
	return 0;//(asic_stat & JL100_ESTS_WDTOVR);
}


/********************************************************************************/
/*																				*/
/*	関数名 :	NdReceiveAsyncCMD()												*/
/*																				*/
/*	機能概要:																	*/
/*		イベントドリブン通信時の通信データ受信（通信ドライバ）					*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		ULONG* async_cbuff		指令データのバッファ							*/
/*	戻り値 :																	*/
/*		= ND_ASYNC_RCV_SUCCESS	正常終了（受信）								*/
/*		= ND_ASYNC_RCV_ERR		受信時エラー検出								*/
/*		= ND_ASYNC_TOUT			受信処理タイムアウト							*/
/*																				*/
/********************************************************************************/
static INT32 NdReceiveAsyncCMD( UINT st_no, ULONG *CmdBuffer )
{
	INT32			ret,i;
	ULONG			no;
	CHANNEL_INFO*	chbuffp;
	ASYNC_ND_PARAM*	asy_prmp;
	JL100_USER_IOMAP *usr_iomapp;

	/* Set pointer of Channel Buffer and Async. communication parameter */
	chbuffp = &MLDevice.chbuff;
	asy_prmp = &MLDevice.async_nd_parm;
	usr_iomapp = MLDevice.usr_io_map;

	no = (asy_prmp->rcvDstAdr>>8)+1;
	if(asy_prmp->sw == ND_ASYNC_REQ_RCV_SEQ)
	{
		/* Request receive async frame */
		ret = jl100s_req_rcv_frame( chbuffp, 
				&asy_prmp->rcvDstAdr, &asy_prmp->rcvSrcAdr, 
				&asy_prmp->rcvStat, &asy_prmp->rcvFtype, 
				MLDevice.DmaRcvBuffer[0], &asy_prmp->rcvSize );

		if( (ret == RECEIVED_FRAME) && (asy_prmp->rcvStat == ASYNC_RCV_CMP) )
		{/* 受信が正常完了したら */
			no = (asy_prmp->rcvDstAdr>>8)+1;
			if(no <= DEF_MA_MAX)
			{
				if(asy_prmp->rcvDstAdr == usr_iomapp[no].axis_adr)
				{
					MLDevice.crcvstat[0] = 1<< no;
					asy_prmp->sw = ND_ASYNC_RCVBUF_SEQ;
				}
			}
		}
	}

	if((no == st_no) && (asy_prmp->sw == ND_ASYNC_RCVBUF_SEQ)) /* <S0EC> */
	{
		for(i = 0; i < (asy_prmp->rcvSize>>2); i++)
		{	CmdBuffer[i] = MLDevice.DmaRcvBuffer[0][i];}
		asy_prmp->sw = ND_ASYNC_SND_SEQ;
	}

	return ND_SUCCESS;
}


/********************************************************************************/
/*																				*/
/*	関数名 :	NdSendAsyncRSP()												*/
/*																				*/
/*	機能概要:																	*/
/*		イベントドリブン通信時の通信データ送信（通信ドライバ）					*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		ULONG* async_rbuff		応答データのバッファ							*/
/*	戻り値 :																	*/
/*		= ND_ASYNC_SEND_SUCCESS	正常終了（送信）								*/
/*		= ND_SENDING_FRAME		データ送信中									*/
/*		= ND_SEND_FRAME_ERR		送信処理失敗									*/
/*																				*/
/********************************************************************************/
static void NdSendAsyncRSP(UINT st_no, ULONG *async_rbuff )
{
	CHANNEL_INFO*	chbuffp;	/* Channel Buffer */
	ASYNC_ND_PARAM* asy_prmp;	/* イベントドリブン通信パラメータ */
	INT32			ret;		/* return code */

	chbuffp = &MLDevice.chbuff;
	asy_prmp = &MLDevice.async_nd_parm;

	if( (asy_prmp->sw == ND_ASYNC_SND_SEQ) 
		&& ((st_no-1) == (asy_prmp->rcvDstAdr>>8)) )
	{	/* Request send async frame */
		ret = jl100s_send_frame(chbuffp, 
								asy_prmp->rcvSrcAdr, asy_prmp->rcvDstAdr, 
								DEF_ASYNC_FTYPE, 
								async_rbuff, DEF_ASYNC_DATA_SIZE );
		if( ret != SNDING_FRAME )
		{
			asy_prmp->sw = ND_ASYNC_REQ_RCV_SEQ;
//			printf("Send Async RSP(%d): 0x%04x\n",st_no, async_rbuff[0]);
		}
	}

}


/****************************************************************************/
/* 						メッセージ用通信ドライバ							*/
/* 						（SCF用の空処理)									*/
/*	概要：				なにもせずに無難に処理を終了する					*/
/****************************************************************************/

/*****************************************************************************
	メッセージ受信開始要求セット
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
		ND_RECV_BUSY	・・・受信中(要求発行失敗)
		ND_SUCCESS		・・・正常終了
*****************************************************************************/
LONG NdReqRecvMsg( UCHAR ch )
{
	CHANNEL_INFO*	chbuffp;			/* チャンネルバッファ領域			*/
	USHORT			cmsg_rcv_stat;		/* C1 or C2 message receive status	*/
	USHORT			crcv_msgsz;
	USHORT			local_adr;
	LONG			lRet;

	chbuffp = &MLDevice.chbuff;
	lRet    = ND_RECV_BUSY;

	if (chbuffp->hif_reg_top == NULL)
	{
		return lRet;
	}

	if (ch == 1)
	{
		MLDevice.crcv_msgsz1 = 0;
		cmsg_rcv_stat = jl100s_req_rcv_msgdata(chbuffp, DEF_C1_MST, &local_adr, C1MST_ADR, &crcv_msgsz);
	}
	else
	{
		MLDevice.crcv_msgsz2 = 0;
		cmsg_rcv_stat = jl100s_req_rcv_msgdata(chbuffp, DEF_C2_MST, &local_adr, C2MST_ADR, &crcv_msgsz);
	}

//	cmsg_rcv_stat = jl100s_req_rcv_msgdata(chbuffp, def_mst, &local_adr, st_adrs, &crcv_msgsz);

	if ( cmsg_rcv_stat == LIB_OK )
	{
		if (crcv_msgsz > 0)
		{
			if (ch == 1)
			{
				MLDevice.crcv_msgsz1 = crcv_msgsz;
				MLDevice.c1msg_peer_multi = local_adr;
			}
			else
			{
				MLDevice.crcv_msgsz2 = crcv_msgsz;
				MLDevice.c2msg_peer_multi = local_adr;
			}
			lRet = ND_SUCCESS;
		}
	} 
	else
	{
		lRet = ND_RECVMSG_ERR;
	}

	return lRet;

}

/*****************************************************************************
	メッセージ受信完了チェック
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
		rcvWSz	・・・受信メッセージワードサイズ
		rcvBSz	・・・受信メッセージバイトサイズ（常に0）
	戻り値：
		ND_RECV_BUSY	・・・受信中, or 受信待ち状態
		ND_SUCCESS		・・・受信完了
		それ以外		・・・受信エラー(ハンドシェイクエラー含)
*****************************************************************************/
LONG NdGetRecvMsgSts( UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz )
{
	LONG		lRet;

	if (ch == 1)
	{
		if (MLDevice.crcv_msgsz1 > 0)
		{
			*rcvWSz = MLDevice.crcv_msgsz1 >> 1;	/* ワードサイズ	*/
			*rcvBSz = MLDevice.crcv_msgsz1;		/* バイトサイズ	*/
			lRet = ND_SUCCESS;
		}
		else
		{
			*rcvWSz = 0;
			*rcvBSz = 0;
			lRet = ND_RECVMSG_ERR;
		}
	}
	else
	{
		if (MLDevice.crcv_msgsz2 > 0)
		{
			*rcvWSz = MLDevice.crcv_msgsz2 >> 1;	/* ワードサイズ	*/
			*rcvBSz = MLDevice.crcv_msgsz2;		/* バイトサイズ	*/
			lRet = ND_SUCCESS;
		}
		else
		{
			*rcvWSz = 0;
			*rcvBSz = 0;
			lRet = ND_RECVMSG_ERR;
		}
	}
	return lRet;
}

/*****************************************************************************
	メッセージデータリード
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
		rcvData	・・・受信データ格納先
		rdSz	・・・リードワード数
*****************************************************************************/
void NdReadMsgData( UCHAR ch, USHORT *rcvData, USHORT rdSz )
{
	CHANNEL_INFO*	chbuffp;
	USHORT			cmsg_rcv_stat;		/* C1 or C2 message receive status	*/
	USHORT			offset;				/*									*/
	USHORT			read_size, size;
	USHORT			def_mst;
	USHORT			nokori;
	UCHAR			*crcvData;

	chbuffp = &MLDevice.chbuff;
	offset = 0;
	crcvData = (UCHAR*)rcvData;
	def_mst = (ch == 1) ? DEF_C1_MST : DEF_C2_MST;

	read_size = 32;
	size = rdSz * 2;

	while (offset < size)
	{
		nokori = size - offset;
		if (read_size > nokori) read_size = nokori;
		cmsg_rcv_stat = jl100s_read_msgdata(chbuffp, def_mst, offset, read_size,
					 (ULONG*)(&crcvData[offset]));
		if(cmsg_rcv_stat != LIB_OK)
		{
			return;
		}
		offset += read_size;
	}
}

/*****************************************************************************
	メッセージデータライト
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
		sndData	・・・送信データ格納先
		wrSz	・・・ライトワード数
		wrSz	・・・ライトバイト数
*****************************************************************************/
void NdWriteMsgData( UCHAR ch, USHORT *sndData, USHORT wrSz )
{
	CHANNEL_INFO*	chbuffp;			/* チャンネルバッファ領域			*/
	USHORT			cmsg_snd_stat;		/* C1 or C2 message receive status	*/
	USHORT			offset;				/*									*/
	USHORT			write_size;
	USHORT			def_mst;
	USHORT			nokori;
	UCHAR			*csndData;
	USHORT			rem;
	USHORT			size;
	USHORT			msg_size;
	
	chbuffp = &MLDevice.chbuff;
	csndData = (UCHAR*)sndData;
	offset = 0;
	def_mst = (ch == 1) ? DEF_C1_MST : DEF_C2_MST;

//	size = wrSz * 2;
	size = wrSz;
	rem = size % 4;
	if (rem != 0)	msg_size = size + (4-rem);
	else			msg_size = size;

	write_size = 32;

	while (offset < msg_size)
	{
		nokori = msg_size - offset;
		if (write_size > nokori) write_size = nokori;
		cmsg_snd_stat = jl100s_write_msgdata(chbuffp, def_mst, offset, write_size,
				 (ULONG*)(csndData+offset));
		if(cmsg_snd_stat != LIB_OK)
		{
			return;
		}
		offset += write_size;
	}
	MLDevice.csnd_msgsz = msg_size;
	//グローバル変数にそのまま値を格納しようとすると
	//0となってしまうため、statiac変数を経由して格納する。
	//コンパイラの最適化の影響？？
}

/*****************************************************************************
	メッセージ送信開始要求セット
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
		ND_SEND_BUSY	・・・送信中(要求発行失敗)
		ND_SUCCESS		・・・正常終了
		それ以外		・・・送信エラー
*****************************************************************************/
LONG NdReqSendMsg( UCHAR ch )
{
	CHANNEL_INFO*	chbuffp;			/* チャンネルバッファ領域			*/
	USHORT			cmsg_snd_stat;		/* C1 or C2 message send status		*/
	USHORT			st_adrs;			/* Satation address					*/
	USHORT			*st_adr_da;			/* Local node station address		*/
	LONG			lRet;
	USHORT			def_mst;

	chbuffp = &MLDevice.chbuff;

	if (ch == 1)
	{
		def_mst   = DEF_C1_MST;
		st_adrs   = C1MST_ADR;
		st_adr_da = &MLDevice.c1msg_peer_multi;

	}
	else
	{
		def_mst   = DEF_C2_MST;
		st_adrs   = C2MST_ADR;
		st_adr_da = &MLDevice.c2msg_peer_multi;

	}

	cmsg_snd_stat = jl100s_req_snd_msgdata(chbuffp, def_mst, 
								st_adrs, *st_adr_da, MLDevice.csnd_msgsz);
	if ( cmsg_snd_stat == LIB_OK )
	{
		lRet = ND_SUCCESS;
	}
	else if (cmsg_snd_stat == SNDING_MSG)
	{
		lRet = ND_SUCCESS;
	}
	else
	{
		lRet = ND_SENDMSG_ERR;
	}
	return lRet;
}

/*****************************************************************************
	メッセージ送信完了チェック
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
		ND_SEND_BUSY	・・・送信中
		ND_SUCCESS		・・・送信完了
		それ以外		・・・送信エラー(ハンドシェイクエラー含)
*****************************************************************************/
LONG NdGetSendMsgSts( UCHAR ch )
{
	CHANNEL_INFO*	chbuffp;			/* チャンネルバッファ領域			*/
	USHORT			cmsg_snd_stat;		/* C1 or C2 message send status		*/
	USHORT			st_adrs;			/* Satation address					*/
	USHORT			*st_adr_da;			/* Local node station address		*/
	LONG			lRet;
	USHORT			def_mst;

	chbuffp = &MLDevice.chbuff;

	if (ch == 1)
	{
		def_mst   = DEF_C1_MST;
		st_adrs   = C1MST_ADR;
		st_adr_da = &MLDevice.c1msg_peer_multi;
	}
	else
	{
		def_mst   = DEF_C2_MST;
		st_adrs   = C2MST_ADR;
		st_adr_da = &MLDevice.c2msg_peer_multi;
	}

	cmsg_snd_stat = jl100s_req_snd_msgdata(chbuffp, def_mst, 
							st_adrs, *st_adr_da, MLDevice.csnd_msgsz);
	if ( cmsg_snd_stat == LIB_OK )
	{
		lRet = ND_SUCCESS;
	}
	else if (cmsg_snd_stat == SNDING_MSG)
	{
		lRet = ND_SEND_BUSY;
	}
	else
	{
		lRet = ND_SENDMSG_ERR;
	}
	return lRet;
}

/*****************************************************************************
	アボートメッセージ送信
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
*****************************************************************************/
void NdSendMsgAbort(UCHAR ch)
{
	CHANNEL_INFO*	chbuffp;
	USHORT			def_mst;

	chbuffp = &MLDevice.chbuff;

	if (ch == 1)
	{
		def_mst = DEF_C1_MST;
	}
	else
	{
		def_mst = DEF_C2_MST;
	}

	(void)jl100s_abort_msgdata(chbuffp, def_mst);

	return;
}

/*****************************************************************************
	通信パラメータ取得
	引数：
	戻り値：*ND_PRAM	//kira_for_MEMOBUS_Message
*****************************************************************************/
ND_PARAM* NdGetNdParamPtr( VOID )
{
	return (&MLDevice.nd_prm);

}

/*****************************************************************************
	通信ASICリセット <S14E>
	引数：DeviceAddress 通信ASIC先頭アドレス
	戻り値：
*****************************************************************************/
void NdResetAsic( void *DeviceAddress )
{
	CHANNEL_INFO *chbuffp;		/* JL-100アクセス定義ポインタ */
	chbuffp		= &MLDevice.chbuff;
	chbuffp->hif_reg_top = (JL100_HOST_IF_REGS *)DeviceAddress;
	jl100s_reset_asic( chbuffp );

	return;
}


/************** end of file **********************/

