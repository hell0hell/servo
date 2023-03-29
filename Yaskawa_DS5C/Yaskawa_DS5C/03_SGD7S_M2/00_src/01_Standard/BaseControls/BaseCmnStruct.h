#ifndef	_COMMON_STR_H /* 再読み込み防止用 */
//****************************************************************************************************
//
//
//		BaseCmnStruct.h : Local Header File for Common Structure
//
//
//****************************************************************************************************
//
//
//************** Copyright (C) Yaskawa Electric Corporation ******************************************
#include "ServoIf.h"

/*--------------------------------------------------------------------------------------------------*/
/*		基本制御モード定義 : 上位バイトはゼロを指定すること											*/
/*--------------------------------------------------------------------------------------------------*/
#define	CTRLMODE_SPD		0x0000			/* 速度制御モード(アナログ指令)							*/
#define	CTRLMODE_POS		0x0001			/* 位置制御モード(パルス列指令)							*/
#define	CTRLMODE_TRQ		0x0002			/* トルク制御モード(アナログ指令)						*/
#define	CTRLMODE_JOG		0x0003			/* ＪＯＧ運転モード										*/
#define CTRLMODE_ZSRCH		0x0004			/* 原点サーチモード										*/
/*--------------------------------------------------------------------------------------------------*/
/*		拡張制御モード定義 : 下位バイトは基本制御モードを指定すること								*/
/*--------------------------------------------------------------------------------------------------*/
#define CTRLMODE_CSPD		0x0100			/* 内部設定速度制御モード(接点指令)						*/
#define CTRLMODE_PJOG		0x0101			/* プログラムＪＯＧ運転モード							*/
#define CTRLMODE_NOCMD		0xFFFF			/* 制御モード指定なし(ScanC専用)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ制御モード定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CTRLMODE_ZNONE		0x00			/* ゼロ制御なし											*/
#define CTRLMODE_ZSTOP		0x01			/* ゼロ速停止モード										*/
#define CTRLMODE_LDSTOP		0x02			/* 直線減速度による減速停止モード						*/ /* <S1F5> */
#define CTRLMODE_ZCLMP		0x03			/* ゼロクランプモード									*/ /* <S1F5> */
// #define CTRLMODE_ZCLMP		0x02			/* ゼロクランプモード									*/ /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------*/
/*		制御モード定義																				*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef __BIG_ENDIAN__
typedef union CTRLMD {						/* 制御モード構造体定義 (4byte以内)						*/
	struct	{								/* 制御モードメンバ定義									*/
		UCHAR	ot;							/* ＯＴ発生中フラグ										*/
		UCHAR	zm;							/* ゼロ制御モード										*/
		UCHAR	cx;							/* 拡張制御モード										*/
		UCHAR	cm;							/* 基本制御モード										*/
	} b;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* 制御モード編集用(Copy,etc)							*/
	struct  {
		USHORT	uwd_h;						/* 制御モード編集用(Copy,etc)							*/
		USHORT	uwd_l;						/* 制御モード編集用(Copy,etc)							*/
	} us;
} CTRLMD;
#else  /* Little Endian */
typedef union CTRLMD {						/* 制御モード構造体定義 (4byte以内)						*/
	struct	{								/* 制御モードメンバ定義									*/
		UCHAR	cm;							/* 基本制御モード										*/
		UCHAR	cx;							/* 拡張制御モード										*/
		UCHAR	zm;							/* ゼロ制御モード										*/
		UCHAR	ot;							/* ＯＴ発生中フラグ										*/
	} b;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* 制御モード編集用(Copy,etc)							*/
	struct  {
		USHORT	uwd_l;						/* 制御モード編集用(Copy,etc)							*/
		USHORT	uwd_h;						/* 制御モード編集用(Copy,etc)							*/
	} us;
} CTRLMD;
#endif /* __BIG_ENDIAN__ */


/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ制御モード定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ZCTRMD {						/* ゼロ制御モード構造体定義 (4byte以内)					*/
	UCHAR	zm;								/* ゼロ制御モード										*/
	UCHAR	zmchg;							/* ゼロ制御モード変化情報								*/
	UCHAR	spare0;							/* 予備													*/
	UCHAR	spare1;							/* 予備													*/
} ZCTRMD;


typedef struct CTRLMD_SET {					/* 制御モード関連定義									*/
	CTRLMD			CtrlMode;				/* Control Mode											*/
	CTRLMD			CtrlModeLst;			/* Last Control Mode									*/
	CTRLMD			CtrlModeChg;			/* Control Mode Change Info (XORで作成)					*/
	ZCTRMD			ZctrlMode;				/* Zero Control Mode									*/
} CTRLMD_SET;



/*--------------------------------------------------------------------------------------------------*/
/*	速度検出移動平均フィルタ		<V110>															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MOTSPDMAFIL {
	KSGAIN	mabuf[16];			/* 移動平均用バッファ	[2^24/OvrSpd]						<S00A>	*/
	LONG	mabufSumx;			/* 移動平均用バッファ積算値											*/
	LONG	idx;				/* バッファインデックス												*/
	LONG	manumBit;			/* 移動平均回数(2の累乗)											*/
	LONG	defaultNumBit;		/* 移動平均回数(2の累乗)の初期値									*/
	LONG	ksub;				/* 減算量演算ゲイン(0/1)											*/
} MOTSPDMAFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		Kernel Mode Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum FUN_KNLMD_T {
	KPI_KMD_NORMAL		  = 0x00,		/* 通常モード												*/
	KPI_KMD_ADJREFAD		,			/* 指令ＡＤ調整中											*/
	KPI_KMD_ADJCURAD		,			/* 電流検出ＡＤ調整中										*/
	KPI_KMD_SOFTRESET		,			/* ソフトリセット中											*/
	KPI_KMD_PARAMRESET		,			/* パラメータリセット中										*/
	KPI_KMD_PARAMCPY		,			/* <S031> パラメータコピー									*/
	KPI_KMD_ADJANLGMON		,			/* アナログモニタ調整中										*/	/* <S0A9> */
	KPI_KMD_PBLEDTEST		,			/* プッシュボタン & 7SegLed確認試験中						*/	/* <S0CB>  */
	KPI_KMD_ADJGAIN			,			/* ゲイン調整中												*/	/* <S0CB>  */
	KPI_KMD_ANLGMONVOLTTEST	,			/* アナログモニタ出力電圧試験中								*/	/* <S0FD>  */
	KPI_KMD_RIPPLECOMPWRITE	,			/* 速度リップル補償情報書き込み中							*/	/* <S14B>  */
} FUN_KNLMD_T;


/*--------------------------------------------------------------------------------------------------*/
/*		Fn機能共通 サーボ制御データ																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FUN_CMN_CONTROL {
	volatile BOOL	FnSvControl;			/* ROUND運転権											*/
	LONG			FnCtrlMcmd;				/* ROUND運転でのコントロールモード						*/
	BOOL			FnSvonReq;				/* ROUND運転でのサーボオン要求							*/
	BOOL			FnSensOnReq;			/* ROUND運転でのサーボオン要求							*/
	BOOL			PdetReq;				/* 磁極検出要求(通電要求)								*/
	BOOL			forcedAlmReady;			/* 強制AlmReadyフラグ									*//* <S215> */
	LONG			JogSpeed;				/* JOG速度指令 +:正転、-:逆転							*/
	USHORT			SafeStopTime;			/* JOG安全停止時間										*/
	SHORT			OperatorSts;			/* オペレータ表示ステータス		*//* <S085> */
	BOOL			SvOnPdetReq;			/* サーボオン磁極検出要求					 	<S21C> 	*/

	USHORT			ForceOutSignals;		/* 強制信号出力指令										*/
	USHORT			ForceOutSignalsHW;		/* 強制信号出力指令(ＨＷ出力信号に反映)					*/	/* <S0CB> */
	UCHAR			OutModeFlg;				/* 強制出力モードフラグ									*/	/* <S0CB> */

	ULONG			LightIntervalTime;		/* 7SegLed点灯時間[100ms]								*/	/* <S0CB> */
	UCHAR			PbAndLedChkTestFlag;	/* プッシュボタン&7SegLed確認フラグ						*/	/* <S0CB> */
	USHORT			AnlgLedChkInitFlag;		/* AP形 プッシュボタン&7SegLed確認開始フラグ			*/	/* <S0CB> */
	UINT			AnlgLedChkCnt;			/* AP形 プッシュボタン&7SegLed確認 カウンタ				*/	/* <S0CB> */
	UINT			AnlgLedChkCnt2;			/* AP形 プッシュボタン&7SegLed確認 カウンタ(左橋のLED)	*/	/* <S0CB> */
	UINT			AnlgLedChkLstPnlBtn;	/* AP形 プッシュボタン&7SegLed確認 前回押下ボタン		*/	/* <S0CB> */
	CHAR			AnlgMonVoltTstFlag;		/* アナログモニタ出力電圧確認試験中フラグ				*/	/* <S0FD> */
	USHORT			AnlgMonVoltTstChSel;	/* アナログモニタ出力電圧確認試験 Ch選択				*/	/* <S0FD> */
	SHORT			OutputVoltVal_Ch0;		/* アナログモニタ出力電圧確認試験 出力電圧設定(Ch0)		*/	/* <S0FD> */
	SHORT			OutputVoltVal_Ch1;		/* アナログモニタ出力電圧確認試験 出力電圧設定(Ch1)		*/	/* <S0FD> */
	FUN_KNLMD_T		KnlMode;				/* Tester mode											*/	/* <S0CB> */

	//原点サーチ操作レジスタ定義場所はここでOK？？
	struct {
		USHORT	ZsrchState;					/* 0x2101 : 0x030: 原点サーチ完了						*/
	} OpeReg;

	struct {
		BOOL		FnSvCtrlDisable;		/* Fn運転不可状態										*/
		BOOL		LockNormalOpeDrive;		/* Fn運転後、通常運転禁止フラグ							*/

		BOOL		TuningRun;				/* サーボ調整中											*/
		BOOL		TuningPctrlRun;			/* サーボ調整P制御中									*/
	} State;

	struct {								/* <S031> Flags */
		UCHAR PrmCopy;						/* <S031> Lcd Operator Parameter copy flag */
		UCHAR Reserved0;					/* <S031> reserved */
		UCHAR Reserved1;					/* <S031> reserved */
		UCHAR Reserved2;					/* <S031> reserved */
	} f;
} FUN_CMN_CONTROL;


/*--------------------------------------------------------------------------------------------------*/
/*		制御ループ出力データ構造体																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CTRL_LOOP_OUT {
//	LONG			MotPos;					/* モータ位置						[Pulse]				*/
	LONG			MotPosErr;				/* 位置アンプ偏差					[Pulse]				*/
	LONG			SpdRefo;				/* 速度指令出力						[2^24/OvrSpd]		*/
	LONG			SpdFFC;					/* 速度FF補償						[2^24/OvrSpd]		*/
	LONG			MotSpd_a;				/* モータ速度平均値					[2^24/OvrSpd]		*//* 2013/04/01版MDでは、ｺﾒﾝﾄｱｳﾄされている	*/
	LONG			TrqRefo;				/* トルク指令出力					[2^24/MaxTrq]		*/
	LONG			TrqRefo_a;				/* トルク指令出力(平均値)			[2^24/MaxTrq]		*/
	LONG			TrqRefMon;				/* トルク指令モニタ					[2^24/MaxTrq]		*/
	LONG			TrqRefMon_a;			/* トルク指令モニタ					[2^24/MaxTrq]		*/
	SHORT			IdRefMon;				/* d軸電流指令モニタ				[15000/Imax]		*/
	SHORT			IqRefMon;				/* q軸電流指令モニタ				[15000/Imax]		*/
	SHORT			IqRefMon_a;				/* q軸電流指令モニタ(平均値)		[15000/Imax]		*/
	SHORT			IqRefMon_l;				/* q軸電流指令モニタ(前回値)		[15000/Imax]		*/
	LONG			IqFbMon;				/* q軸電流FBモニタ										*/
	LONG			IdFbMon;				/* d軸電流FBモニタ										*/
	LONG			IqRefFilMon;			/* q軸電流指令モニタ(フィルタ後)						*/
//	BOOL			PConActFlg;				/* P制御状態確認フラグ									*//* <S064> */
	BOOL			SvonSpdLmtFlag;			/* サーボON時速度制限中フラグ							*/
//	BOOL			SpdClamp;				/* トルク制御時速度制限中フラグ							*//* <S04B> */
	BOOL			TrqClamp;				/* トルク制限中フラグ									*/
} CTRL_LOOP_OUT;


/*--------------------------------------------------------------------------------------------------*/
/*		ベース制御出力データ構造体																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct BASE_CTRL_OUT {
	CTRLMD			CtrlModeOut;			/* 制御モード(ScanB出力)								*/
	LONG			TrqCtrlRef;				/* トルク制御時のトルク指令				[2^24/MaxTrq]	*/
	LONG			SpdCtrlRef;				/* 速度制御時の速度指令					[2^24/OvrSpd]	*/
	LONG			OutputSpdRef;			/* 速度指令出力値(ScanB→ScanC)			[2^24/OvrSpd]	*/
//	LONG			SpdRefSum;				/* 速度指令出力値(Ref+FF)				[2^24/OvrSpd]	*/
	LONG			MotPos;					/* モータ位置(ScanB積算値)				[32bitRingPulse]*/
	LONG			FencPos;				/* モータ位置(ScanB積算値)				[32bitRingPulse]*/
	LONG			PcmdCntr;				/* 指令パルスカウント値									*/
	LONG			MotSpdSum;				/* モータ速度加算値(ScanC平均速度演算用)				*/
	LONG			MotSpdCnt;				/* ScanC平均速度演算用ScanBカウンタ						*/
	LONG			PositionError;			/* 位置偏差												*/
	LONG			SvonSpdLmtReq;			/* サーボON時速度制限要求								*/
	LONG			CmdSeqBit;				/* シーケンス制御指令(ScanB→ScanC)						*/
	BOOL			TblZclamp;				/* テーブル運転ゼロクランプ中フラグ						*/
	LONG			IdRefMon;				/* d軸電流指令モニタ				[15000/Imax]		*/
	LONG			IqRefMon;				/* q軸電流指令モニタ				[15000/Imax]		*/
	LONG			TrqRefMon;				/* トルク指令モニタ					[2^24/MaxTrq]		*/
} BASE_CTRL_OUT;


typedef	struct SEQ_CTRL_OUT {
/* Request commands to the Control module */
	CTRLMD			CtrlModeReq;			/* ScanC Control Mode要求								*/
	VBOOL			BaseEnableReq;			/* ScanC作成ベースイネーブル要求						*//* <S193> */
	BOOL			BeComplete;				/* ベースイネーブル完了状態								*/
	BOOL			UdlBeStatus;			/* ベースイネーブル完了状態(UDL側状態モニタ)			*/
	BOOL			BBSvcRefClrReq;			/* Servo Control Ref. Clear Request Flag				*/
	BOOL			TrqLimitReq;			/* OTトルク制限要求(OTシーケンス処理出力)				*/
	LONG			ModeSwAcc;				/* Motor Acceleration fow ModeSw		[NorSpd/ScanC]	*/

/* Sequence Status */
	BOOL			HwbbSts;				/* HWBB状態												*/
	BOOL			FstpSts;				/* FSTP状態												*/ /* <S1F5> */
	BOOL			MainPowerOn;			/* 主回路ON状態											*/
	BOOL			MotStop;				/* モータ停止状態										*/
	BOOL			OverTrvlSts;			/* OT状態(OTシーケンス処理出力)							*/
	BOOL			SvRdyPhaseIgnore;		/* Servo Ready (Ignore Phase)							*/

	BOOL			PotSigSts;				/* 正転駆動禁止状態										*/
	BOOL			NotSigSts;				/* 逆転駆動禁止状態										*/
	BOOL			BrkSigSts;				/* ブレーキ解放状態										*/
	BOOL			EstpSigSts;				/* E-STOP状態											*/

	BOOL			TgOnSts;				/* モータ回転状態										*/
	BOOL			VcmpSts;				/* 速度一致状態											*/
	BOOL            MotSuspLvl;                /*Motor Suspend Level Flag <S201> */
	BOOL			RndReady;				/* 2013/04/01MD版では、RndReadyはない。暫定的に、ここで定義する	*/

	LONG			RspSeqBits;				/* シーケンス制御状態(ALM/WRN/SVON/SENON/PDET/BK/SRDY/MRDY)*/
	LONG			RspSigBits;				/* 入力信号状態(HWBB/P-OT/N-OT)							*/
	LONG			RspCtrlBits;			/* 制御状態(HBB/TGON/VCMP)								*/
/*--------------------------------------------------------------------------------------------------*/
	struct 	{								/* AP	<S00C>											*/
		UINT	Vrefon              :1;		/* 0x10 : Speed  Reference Input On						*/
		UINT	Trefon              :1;		/* 0x11 : Torque Reference Input On						*/
		UINT	Prefon              :1;		/* 0x12 : Pulse  Reference Input On						*/
		UINT	PerrClr             :1;		/* 0x13 : PosErr Clear     Input On						*/
		UINT	PrefSign            :1;		/* 0x14 : Pulse  Reference Sign  (plus:1, minus:0)		*/
	}f;
/*--------------------------------------------------------------------------------------------------*/
	UINT	AlmRstLast;						/* アラームリセット信号前回値 @@ どっかに移動			*/
/*--------------------------------------------------------------------------------------------------*/
} SEQ_CTRL_OUT;


typedef	struct ANLG_UN_MON_DATA {
/* 暫定処理 2013/04/01MDは、ｺﾒﾝﾄｱｳﾄしている。	*/
	LONG			AmonSpdRef;				/* AnaMon : 速度指令(速度ＦＦ含)	[2^24/OvrSpd]		*/
	LONG			AmonTrqRef;				/* AnaMon : トルク指令(重力補償後)	[2^24/MaxTrq]		*/
/* 暫定処理 2013/04/01MDは、ｺﾒﾝﾄｱｳﾄしている。	*/

	LONG			UnMotSpd;				/* Un000  : モータ速度				[pulse/64ms]		*/
	LONG			UnMotSpdx;				/* Un000  : モータ速度(2ms平均速度)	[2^24/OvrSpd]		*/
	LONG			UnPfbk64ms;				/* Un000  : モータ速度				[pulse/64ms]		*/
	LONG			UnSpdRef;				/* Un001  : 速度指令				[2^24/OvrSpd]		*/
	LONG			UnTrqRef;				/* Un002  : トルク指令				[2^24/MaxTrq]		*/
	LONG			UnMotAngle;				/* Un003  : 回転角１(パルス)		[pulse]				*/
	LONG			UnMotPhase;				/* Un004  : 回転角２(電気角)		[deg]				*/
	LONG			UnPcmd64ms;				/* Un007  : 指令パルス速度			[pulse/64ms]		*/
	LONG			UnIsqrSum;				/* Un009  : 累積負荷率				[(15000/MaxTrq)/10sec]*/
	LONG			UnMeanRgPower;			/* Un00A  : 回生消費電力平均値		[%]					*/
	LONG			UnMeanDbPower;			/* Un00B  : ＤＢ消費電力平均値 		[%]					*/
	LONG			UnPcmdCntr;				/* Un00C  : 指令パルスカウンタ		[pulse]				*/
	LONG			UnPfbkCntr;				/* Un00D  : ＦＢパルスカウンタ		[pulse]				*/
	LONG			UnPfbkCntr2;			/* Un013  : ＦＢパルスカウンタ		[指令単位]			*//* <S056> */
	LONG			RemCntr2;				/* Un013  : ＦＢパルスカウンタの余り[指令単位]			*//* <S056> */
	LONG			UnInstEnvMonAmp;		/* Un025 : 設置環境モニタ(2s更新)	[0.001%]			*//* <S1D7> */
	LONG			UnInstEnvMonMot;		/* Un026 : 設置環境モニタ(2s更新)	[0.001%]			*//* <S1D7> */
	ULONG			UnCrntDet;				/* Un141 : ２乗電流ＦＢ値			[(15000/MAXTRQ)^2]	*/
	ULONG			UnCrntRef;				/* Un142 : ２乗電流指令値			[(15000/MAXTRQ)^2]	*/
	LONG			UnRgPower;				/* Un143  : 回生消費電力値			[%]					*/
	LONG			UnDbPower;				/* Un144  : ＤＢ消費電力値 			[%]					*/
	ULONG			UnIsqrSumMax;			/* Un145 : 累積負荷率最大値			[(15000/MaxTrq)/10sec]*/

	LONG			UnOLRate;				/* Un1F0  : 過負荷レベル率			[%]					*/
	LONG			UnOLType;				/* Un1F1  : 過負荷種別 (0～4)		[]					*/
											/*			0: 過負荷なし 1:モータ連続  2:モータ瞬時  	*/
											/*						  3:アンプ連続  4:アンプ瞬時	*/
	LONG			TrcRgPower;				/* Trace  : 回生消費電力値			[%]					*/
	LONG			TrcDbPower;				/* Trace  : ＤＢ消費電力値 			[%]					*/

	LONG			UnPcmd64Sum;			/* 指令パルス速度 <S1B6>			[64ms]				*/

	USHORT			SvStatus;				/* サーボステータスモニタ								*//* <S049> */
	USHORT			LedDispStatus;			/* ネットワーク7Seg表示ステータス						*//* <S0A1> */

} ANLG_UN_MON_DATA;


/***************************************** end of file **********************************************/
#define	_COMMON_STR_H
#endif
