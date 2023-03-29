/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsPrmSgdvCmOpt.h : パラメータ定義ヘッダファイル											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. パラメータ定義構造体																		*/
/*																									*/
/*		2. パラメータ設定データ構造体																*/
/*																									*/
/*		3. 標準パラメータ計算関数																	*/
/*																									*/
/*		4. パラメータ定義Extern宣言																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2013.01.11 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/

#define FORMAT_ID			0x0909





/****************************************************************************************************/
/*																									*/
/*		エクセル表共通定義																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		表示時の底																					*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_BASENBL			0				/* 属性：ニブル表示									*/
#define DSPDEF_BASEDEC			1				/* 属性：10進表示									*/
#define DSPDEF_BASEHEX			2				/* 属性：16進表示									*/
#define DSPDEF_BASEBIT			3				/* 属性：ビット表示									*/
/*--------------------------------------------------------------------------------------------------*/
/*		符号付きか否か																				*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_W_SIGN			TRUE			/* 符号付											*/
#define DSPDEF_NOSIGN			FALSE			/* 符号なし											*/
/*--------------------------------------------------------------------------------------------------*/
/*		アクセスレベル(注意：0 は使用不可)															*/
/*--------------------------------------------------------------------------------------------------*/
#define ACCLVL_USER1			1				/* ユーザー１										*/
#define ACCLVL_USER2			2				/* ユーザー２										*/
#define ACCLVL_USER3			3				/* ユーザー３										*//* <S0FE> */
//#define ACCLVL_SYSTEM			3				/* システム											*//* <S0FE> */
#define ACCLVL_SYSTEM			4				/* システム											*//* <S0FE> */
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		パラメータ定義構造体定義																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ定義構造体																		*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	PRMDEF_VERSION
typedef struct	{
	struct	{
		UINT	IsLong     : 1;					/* ロングパラメータ(TRUE/FALSE)						*/
		UINT	WordOrder  : 1;					/* パラメータ中の、このワードの順番					*/
		UINT	AdjustPrm  : 1;					/* 調整パラメータか?(ADJUST/NORMAL)					*/
		UINT	NeedReboot : 1;					/* 電源再立上げ(NEEDBOOT/NON_BOOT)					*/
		UINT	Place      : 1;					/* 格納場所(BOTH_RAM_EEPROM/ONLY_RAM)				*/
		UINT	Base       : 2;					/* 表示時の底(DEC/HEX/NBL)							*/
		UINT	Sign       : 1;					/* 表示時の符号有無(W_SIGN/NOSIGN)					*/
		UINT	Decimals   : 3;					/* 小数点以下桁数(0-4)								*/
		UINT	ChgDpLsm   : 1;					/* 小数点位置変更(低速モータ)						*/
		UINT	ReadOnly   : 1;					/* リードオンリー(WRITABLE/READONLY)				*/
		UINT	AdjOpeIn   : 1;					/* オペレータ入力調整(TRUE/FALSE)					*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	SpareBits  : 2;					/* 予備												*/
		UINT	FormatRev  : 8;					/* FORMAT_ID改版履歴 1byteデータで使用しますので配置変更しないでください(ﾋﾞｯﾄﾌｨｰﾙﾄﾞ内16～23bitに定義されていること)	*/
		UINT	Unit       : 8;					/* 単位 (Reserved)									*/
	} Attr;										/* パラメータ属性									*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ChkSpUse;							/* ﾃﾞｼﾞOPE表示選択(上位:4Bit)/専用使用ﾁｪｯｸ(下位:4Bit) 	*/
	UCHAR	AccessLevel;						/* 1:User1, 2:User2, 3:System						*/
	USHORT	EepromAddr;							/* EEPROMアドレス + 属性(DefaultPrm無し)			*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*FuncPtr)( ULONG Cmd, ULONG Data, ULONG Digit );	/* 処理関数へのポインタ				*/
	USHORT	*RamPtr;							/* RAM格納場所へのポインタ							*/
	USHORT	LowerLimit;							/* 下限値											*/
	USHORT	UpperLimit;							/* 上限値											*/
} PRMDEF;
typedef const PRMDEF CPRMDEF;					/* const パラメータ定義								*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ管理テーブル構造体																*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	PRMTBL_VERSION
typedef struct	{
	USHORT	PnNo;								/* Ｐｎ番号											*/
	CPRMDEF	*PrmDefPtr;							/* パラメータ定義へのポインタ						*/
} PRMTBL;
typedef const PRMTBL CPRMTBL;					/* const パラメータ管理テーブル						*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		PRMDEF Version																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_VERSION			0x0001			/* PRMDEF Version									*/
#define PRMTBL_VERSION			0x0001			/* PRMTBL Version									*/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ種別																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_ADJUST			TRUE			/* 調整パラメータ									*/
#define PRMDEF_NORMAL			FALSE			/* 通常パラメータ									*/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ変更後再立上げの要・不要															*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_NEEDBOOT			TRUE			/* 再立上げ必要										*/
#define PRMDEF_NON_BOOT			FALSE			/* 再立上げ不要										*/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ格納場所																			*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_RAM_EEPROM		TRUE			/* RAMとEEPROM										*/
#define PRMDEF_ONLY_RAM			FALSE			/* RAMのみ											*/
/*--------------------------------------------------------------------------------------------------*/
/*		リードオンリー																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_READONLY			TRUE			/* リードオンリー									*/
#define PRMDEF_WRITABLE			FALSE			/* 書込み可											*/
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROMアドレス属性定義(PRMDEF.EepromAddr)													*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMDEF_EEPADRMSK		0x0FFF			/* アドレスマスク									*/
#define PRMDEF_NODEFTPRM		0x8000			/* デフォルトパラメータ無し定義						*/
/*--------------------------------------------------------------------------------------------------*/
/*		単 位 (Reserved)																			*/
/*--------------------------------------------------------------------------------------------------*/
#define UNIT_NONE				0x00			/* No Unit											*/
#define UNIT_TIME_SEC			0x01			/* time [sec]										*/
#define UNIT_TIME_MS			0x02			/* time [ms]										*/
#define UNIT_TIME_US			0x03			/* time [us]										*/
#define UNIT_SECINV				0x04			/* [1/s]											*/
#define UNIT_FREQ				0x05			/* Frequency [Hz]									*/
#define UNIT_PERCENT			0x06			/* percent (Numerical value 0-100)					*/
#define UNIT_ROT_SPEED			0x07			/* Speed [min-1]									*/
#define UNIT_LIN_SPEED			0x08			/* Speed [mm/s]										*/
#define UNIT_ROT_ACCEL			0x09			/* Acceleration [r/min/s]							*/
#define UNIT_LIN_ACCEL			0x0A			/* Acceleration [mm/s^2]							*/
#define UNIT_REFPULS			0x0B			/* length [Pulse]									*/
#define UNIT_ENCPULS			0x0C			/* length [Pulse]									*/
#define UNIT_PPR				0x0D			/* others [Pulse/rev]								*/
#define UNIT_REVOLUTION			0x0E			/* length revolution [rev]							*/
#define UNIT_REFPULS_SPD		0x0F			/* Pulse Speed [P/s]								*/
#define UNIT_ENCPULS_PITCH		0x10			/* length [Pulse/Pitch]								*/
#define UNIT_VOLT_PER_SPD		0x11			/* Voltage [V] per rate speed						*/
#define UNIT_VOLT_PER_TRQ		0x12			/* Voltage [V] per rate torque						*/
#define UNIT_VOLTAGE			0x13			/* Voltage [V]										*/
#define UNIT_CURRENT			0x14			/* Current [A]										*/
#define UNIT_RESIST				0x15			/* Resistance [Ohm]									*/
#define UNIT_WATTAGE			0x16			/* Wattage [W]										*/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算コマンド定義																	*/
/*--------------------------------------------------------------------------------------------------*/
#define PNCAL_PRMCAL			0x00			/* Parameter Calculation							*/
#define PNCAL_ADJINCDATA		0x01			/* Adjust Increment Data							*/
#define PNCAL_ADJDECDATA		0x02			/* Adjust Decrement Data							*/
#define PNCAL_PRMCALFROMREG		0x03			/* Parameter Calculation(From Register I/F)			*/
#define PNCAL_COMPRMEEPROM_WR	0x04			/* パラメータEEPROM書き込み							*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		パラメータ設定データ構造体定義																*/
/*																									*/
/****************************************************************************************************/
typedef struct
{
	USHORT	comCtrl;			/* Pn800 : 通信制御機能選択											*/
	USHORT	sLimitSw;			/* Pn801 : ソフトリミット機能選択									*/
	USHORT	cmdCtrlSw;			/* Pn802 : コマンド動作設定											*/
	USHORT	zpWidth;			/* Pn803 : 原点位置範囲												*/
	USHORT	p_sLimitL;			/* Pn804 : 正側ソフトリミット値 L									*/
	USHORT	p_sLimitH;			/* Pn805 : 正側ソフトリミット値 H									*/
	USHORT	n_sLimitL;			/* Pn806 : 負側ソフトリミット値 L									*/
	USHORT	n_sLimitH;			/* Pn807 : 負側ソフトリミット値 H									*/
	USHORT	absOffsetL;			/* Pn808 : アブソＰＧ原点位置オフセット L							*/
	USHORT	absOffsetH;			/* Pn809 : アブソＰＧ原点位置オフセット H							*/
	USHORT	accRate1;			/* Pn80A : １段目直線加速定数										*/
	USHORT	accRate2;			/* Pn80B : ２段目直線加速定数										*/
	USHORT	accChgSpd;			/* Pn80C : 加速定数切替え速度										*/
	USHORT	decRate1;			/* Pn80D : １段目直線減速定数										*/
	USHORT	decRate2;			/* Pn80E : ２段目直線減速定数										*/
	USHORT	decChgSpd;			/* Pn80F : 減速定数切替え速度										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	filExpBias;			/* Pn810 : 指数関数加減速バイアス									*/
	USHORT	filExpTb;			/* Pn811 : 指数関数加減速時定数										*/
	USHORT	filMaTb;			/* Pn812 : 移動平均時間												*/
	USHORT	resrv2;				/* Pn813 : 予約														*/
	USHORT	exDistL;			/* Pn814 : 外部位置決め最終走行距離 L								*/
	USHORT	exDistH;			/* Pn815 : 外部位置決め最終走行距離 H								*/
	USHORT	zretDir;			/* Pn816 : 原点復帰モード設定										*/
	USHORT	zretSpd1;			/* Pn817 : 原点復帰アプローチ速度１									*/
	USHORT	zretSpd2;			/* Pn818 : 原点復帰アプローチ速度２									*/
	USHORT	zretDistL;			/* Pn819 : 原点復帰最終走行距離 L									*/
	USHORT	zretDistH;			/* Pn81A : 原点復帰最終走行距離 H									*/
	USHORT	resrv3;				/* Pn81B : 予約														*/
	USHORT	resrv4;				/* Pn81C : 予約														*/
	USHORT	resrv5;				/* Pn81D : 予約														*/
	USHORT	ioMonSel;			/* Pn81E : 入力信号モニタ選択										*/
	USHORT	optBitSw;			/* Pn81F : コマンドデータ割付選択									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	p_ltLimitL;			/* Pn820 : 正側ラッチ可能領域 L										*/
	USHORT	p_ltLimitH;			/* Pn821 : 正側ラッチ可能領域 H										*/
	USHORT	n_ltLimitL;			/* Pn822 : 逆側ラッチ可能領域 L										*/
	USHORT	n_ltLimitH;			/* Pn823 : 逆側ラッチ可能領域 H										*/
	USHORT	opMon1;				/* Pn824 : オプションモニタ１選択									*/
	USHORT	opMon2;				/* Pn825 : オプションモニタ２選択									*/
	USHORT	decRate1Emg;		/* Pn826 : 非常停止用１段目直線減速定数								*/
	USHORT	decRate2Emg;		/* Pn827 : 非常停止用２段目直線減速定数								*/
	USHORT	decChgSpdEmg;		/* Pn828 : 非常停止用減速定数切替え速度								*/
	USHORT	svOffTime;			/* Pn829 : SVOFF待ち時間(減速停止SVOFF時)							*/
	USHORT	optbit1;			/* Pn82A : OPTION機能ビット割付１									*/
	USHORT	optbit2;			/* Pn82B : OPTION機能ビット割付２									*/
	USHORT	optbit3;			/* Pn82C : OPTION機能ビット割付３									*/
	USHORT	optbit4;			/* Pn82D : OPTION機能ビット割付４									*/
	USHORT	optbit5;			/* Pn82E : OPTION機能ビット割付５									*/
	USHORT	resrv6;				/* Pn82F : 予約														*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	rotrefL;			/* Pn830 : 機械一回転当たりの指令単位量L							*/
	USHORT	rotrefH;			/* Pn831 : 機械一回転当たりの指令単位量H							*/
	USHORT	st_num;				/* Pn832 : 機械一回転当たりのステーション数							*/
	USHORT	rateChgSw;			/* Pn833 : POSING定数選択											*/
	USHORT	accRate12L;			/* Pn834 : １段目直線加速定数 2nd L									*/
	USHORT	accRate12H;			/* Pn835 : １段目直線加速定数 2nd H									*/
	USHORT	accRate22L;			/* Pn836 : ２段目直線加速定数 2nd L									*/
	USHORT	accRate22H;			/* Pn837 : ２段目直線加速定数 2nd H									*/
	USHORT	accChgSpd2L;		/* Pn838 : 加速定数切替え速度 2nd L									*/
	USHORT	accChgSpd2H;		/* Pn839 : 加速定数切替え速度 2nd H									*/
	USHORT	decRate12L;			/* Pn83A : １段目直線減速定数 2nd L									*/
	USHORT	decRate12H;			/* Pn83B : １段目直線減速定数 2nd H									*/
	USHORT	decRate22L;			/* Pn83C : ２段目直線減速定数 2nd L									*/
	USHORT	decRate22H;			/* Pn83D : ２段目直線減速定数 2nd H									*/
	USHORT	decChgSpd2L;		/* Pn83E : 減速定数切替え速度 2nd L									*/
	USHORT	decChgSpd2H;		/* Pn83F : 減速定数切替え速度 2nd H									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	decRateEmg2L;		/* Pn840 : 非常停止用２段目直線減速定数　2nd L						*/
	USHORT	decRateEmg2H;		/* Pn841 : 非常停止用２段目直線減速定数　2nd H						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ltSeqNum;			/* Pn850 : 連続ラッチシーケンス数									*/
	USHORT	ltLoopNum;			/* Pn851 : 連続ラッチ回数											*/
	USHORT	ltSeqSet1;			/* Pn852 : 連続ラッチシーケンス設定1-4								*/
	USHORT	ltSeqSet2;			/* Pn853 : 連続ラッチシーケンス設定4-8								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	comDataCtrl;		/* Pn88F : 通信CMD/RSPデータ制御									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkNum;				/* Pn900 : バンク数													*/
	USHORT	bnkMemNum;			/* Pn901 : バンクメンバ数											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkMember[15];		/* Pn902 : バンクメンバ												*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	prmVer;				/* Pn9E0 : パラメータVer.											*/
	USHORT	spdDivision;		/* Pn9E1 : 速度分解能設定											*/
	USHORT	CmdOpSw;			/* Pn9E2 : Command-Option機能SW										*/
	USHORT	sys_rsrv2;			/* Pn9E3 : 															*/
	USHORT	sys_rsrv3;			/* Pn9E4 : 															*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	zretSpd12L;			/* Pn842 : 原点復帰アプローチ速度１ 2nd L							*/
	USHORT	zretSpd12H;			/* Pn843 : 原点復帰アプローチ速度１ 2nd H							*/
	USHORT	zretSpd22L;			/* Pn844 : 原点復帰アプローチ速度２ 2nd L							*/
	USHORT	zretSpd22H;			/* Pn845 : 原点復帰アプローチ速度２ 2nd H							*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	nodeAdr;			/* Pn880 : 局アドレス設定											*/
	USHORT	trByte;				/* Pn881 : 伝送バイト数												*/
	USHORT	comTCycle;			/* Pn882 : 伝送周期													*/
	USHORT	comAplCycle;		/* Pn883 : 通信周期													*/
	USHORT	comErrCnt;			/* Pn88A : 受信エラーカウンタ										*/
	USHORT	comErrSts1;			/* Pn88B : (予約)													*/
	USHORT	comErrSts2;			/* Pn88C : 通信エラー発生時の、エラーステータス(累積)				*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	almCmd[16];			/* Pn890 : アラーム時、通信CMDデータ								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	almRsp[16];			/* Pn8A0 : アラーム時、通信RSPデータ								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	comCmd[16];			/* Pn8B0 : Online通信CMDデータ										*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	comRsp[16];			/* Pn8C0 : Online通信RSPデータ										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	OpCardID;			/* Pn8E0 : 指令Option用 Option Card ID								*/
	USHORT	OpCardCtrlSw;		/* Pn8E1 : 指令Option用 Option 動作設定								*/
	USHORT	OpCardSwVerL;		/* Pn8E2 : 指令Option用 Option S/W Ver.								*/
	USHORT	OpCardSwVerH;		/* Pn8E3 : 指令Option用 Option S/W Ver.								*/
	USHORT	OpCardYNo;			/* Pn8E4 : 指令Option用 Option Y Spec. No.							*/
	USHORT	OpCardDate;			/* Pn8E5 : 指令Option用 ManufactureDate.							*/
	USHORT	OpCardSvSw;			/* Pn8E6 : 指令Option用 サーボSW設定								*/
	USHORT	OpCardAlmCode;		/* Pn8E8 : 指令Option用 アラーム設定								*/
	USHORT	OpCardAlmAttr;		/* Pn8E9 : 指令Option用 アラーム設定								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	OpCardModel[8];		/* Pn8F0 : 指令Option用 Option Model								*/
	USHORT	OpCardSNo[8];		/* Pn8F8 : 指令Option用 Serial No									*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkData[64];		/* Pn920 : バンクデータ												*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	cmdopv;				/* PnB00 : 仮想指令オプションパラメータ								*/
} OPPRMDAT;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		パラメータ設定データ変数Extern																*/
/*																									*/
/****************************************************************************************************/
#pragma ghs startzda
extern	OPPRMDAT	OpPrm;
#pragma ghs endzda



/****************************************************************************************************/
/*																									*/
/*		標準パラメータ計算関数																		*/
/*																									*/
/****************************************************************************************************/
LONG	pncal_StdDummy( ULONG Cmd, ULONG Data, ULONG Digit );	/* 標準パラメータ計算Dummy関数		*/



/****************************************************************************************************/
/*																									*/
/*		パラメータ定義Extern宣言																	*/
/*																									*/
/****************************************************************************************************/
extern	const	PRMDEF	pndef_comCtrl;
extern	const	PRMDEF	pndef_sLimitSw;
extern	const	PRMDEF	pndef_cmdCtrlSw;
extern	const	PRMDEF	pndef_zpWidth;
extern	const	PRMDEF	pndef_p_sLimitL;
extern	const	PRMDEF	pndef_p_sLimitH;
extern	const	PRMDEF	pndef_n_sLimitL;
extern	const	PRMDEF	pndef_n_sLimitH;
extern	const	PRMDEF	pndef_absOffsetL;
extern	const	PRMDEF	pndef_absOffsetH;
extern	const	PRMDEF	pndef_accRate1;
extern	const	PRMDEF	pndef_accRate2;
extern	const	PRMDEF	pndef_accChgSpd;
extern	const	PRMDEF	pndef_decRate1;
extern	const	PRMDEF	pndef_decRate2;
extern	const	PRMDEF	pndef_decChgSpd;
extern	const	PRMDEF	pndef_filExpBias;
extern	const	PRMDEF	pndef_filExpTb;
extern	const	PRMDEF	pndef_filMaTb;
extern	const	PRMDEF	pndef_resrv2;
extern	const	PRMDEF	pndef_exDistL;
extern	const	PRMDEF	pndef_exDistH;
extern	const	PRMDEF	pndef_zretDir;
extern	const	PRMDEF	pndef_zretSpd1;
extern	const	PRMDEF	pndef_zretSpd2;
extern	const	PRMDEF	pndef_zretDistL;
extern	const	PRMDEF	pndef_zretDistH;
extern	const	PRMDEF	pndef_resrv3;
extern	const	PRMDEF	pndef_resrv4;
extern	const	PRMDEF	pndef_resrv5;
extern	const	PRMDEF	pndef_ioMonSel;
extern	const	PRMDEF	pndef_optBitSw;
extern	const	PRMDEF	pndef_p_ltLimitL;
extern	const	PRMDEF	pndef_p_ltLimitH;
extern	const	PRMDEF	pndef_n_ltLimitL;
extern	const	PRMDEF	pndef_n_ltLimitH;
extern	const	PRMDEF	pndef_opMon1;
extern	const	PRMDEF	pndef_opMon2;
extern	const	PRMDEF	pndef_decRate1Emg;
extern	const	PRMDEF	pndef_decRate2Emg;
extern	const	PRMDEF	pndef_decChgSpdEmg;
extern	const	PRMDEF	pndef_svOffTime;
extern	const	PRMDEF	pndef_optbit1;
extern	const	PRMDEF	pndef_optbit2;
extern	const	PRMDEF	pndef_optbit3;
extern	const	PRMDEF	pndef_optbit4;
extern	const	PRMDEF	pndef_optbit5;
extern	const	PRMDEF	pndef_resrv6;
extern	const	PRMDEF	pndef_rotrefL;
extern	const	PRMDEF	pndef_rotrefH;
extern	const	PRMDEF	pndef_st_num;
extern	const	PRMDEF	pndef_rateChgSw;
extern	const	PRMDEF	pndef_accRate12L;
extern	const	PRMDEF	pndef_accRate12H;
extern	const	PRMDEF	pndef_accRate22L;
extern	const	PRMDEF	pndef_accRate22H;
extern	const	PRMDEF	pndef_accChgSpd2L;
extern	const	PRMDEF	pndef_accChgSpd2H;
extern	const	PRMDEF	pndef_decRate12L;
extern	const	PRMDEF	pndef_decRate12H;
extern	const	PRMDEF	pndef_decRate22L;
extern	const	PRMDEF	pndef_decRate22H;
extern	const	PRMDEF	pndef_decChgSpd2L;
extern	const	PRMDEF	pndef_decChgSpd2H;
extern	const	PRMDEF	pndef_decRateEmg2L;
extern	const	PRMDEF	pndef_decRateEmg2H;
extern	const	PRMDEF	pndef_zretSpd12L;
extern	const	PRMDEF	pndef_zretSpd12H;
extern	const	PRMDEF	pndef_zretSpd22L;
extern	const	PRMDEF	pndef_zretSpd22H;
extern	const	PRMDEF	pndef_ltSeqNum;
extern	const	PRMDEF	pndef_ltLoopNum;
extern	const	PRMDEF	pndef_ltSeqSet1;
extern	const	PRMDEF	pndef_ltSeqSet2;
extern	const	PRMDEF	pndef_nodeAdr;
extern	const	PRMDEF	pndef_trByte;
extern	const	PRMDEF	pndef_comTCycle;
extern	const	PRMDEF	pndef_comAplCycle;
extern	const	PRMDEF	pndef_comErrCnt;
extern	const	PRMDEF	pndef_comErrSts1;
extern	const	PRMDEF	pndef_comErrSts2;
extern	const	PRMDEF	pndef_comDataCtrl;
extern	const	PRMDEF	pndef_almCmd0;
extern	const	PRMDEF	pndef_almCmd1;
extern	const	PRMDEF	pndef_almCmd2;
extern	const	PRMDEF	pndef_almCmd3;
extern	const	PRMDEF	pndef_almCmd4;
extern	const	PRMDEF	pndef_almCmd5;
extern	const	PRMDEF	pndef_almCmd6;
extern	const	PRMDEF	pndef_almCmd7;
extern	const	PRMDEF	pndef_almCmd8;
extern	const	PRMDEF	pndef_almCmd9;
extern	const	PRMDEF	pndef_almCmd10;
extern	const	PRMDEF	pndef_almCmd11;
extern	const	PRMDEF	pndef_almCmd12;
extern	const	PRMDEF	pndef_almCmd13;
extern	const	PRMDEF	pndef_almCmd14;
extern	const	PRMDEF	pndef_almCmd15;
extern	const	PRMDEF	pndef_almRsp0;
extern	const	PRMDEF	pndef_almRsp1;
extern	const	PRMDEF	pndef_almRsp2;
extern	const	PRMDEF	pndef_almRsp3;
extern	const	PRMDEF	pndef_almRsp4;
extern	const	PRMDEF	pndef_almRsp5;
extern	const	PRMDEF	pndef_almRsp6;
extern	const	PRMDEF	pndef_almRsp7;
extern	const	PRMDEF	pndef_almRsp8;
extern	const	PRMDEF	pndef_almRsp9;
extern	const	PRMDEF	pndef_almRsp10;
extern	const	PRMDEF	pndef_almRsp11;
extern	const	PRMDEF	pndef_almRsp12;
extern	const	PRMDEF	pndef_almRsp13;
extern	const	PRMDEF	pndef_almRsp14;
extern	const	PRMDEF	pndef_almRsp15;
extern	const	PRMDEF	pndef_comCmd0;
extern	const	PRMDEF	pndef_comCmd1;
extern	const	PRMDEF	pndef_comCmd2;
extern	const	PRMDEF	pndef_comCmd3;
extern	const	PRMDEF	pndef_comCmd4;
extern	const	PRMDEF	pndef_comCmd5;
extern	const	PRMDEF	pndef_comCmd6;
extern	const	PRMDEF	pndef_comCmd7;
extern	const	PRMDEF	pndef_comCmd8;
extern	const	PRMDEF	pndef_comCmd9;
extern	const	PRMDEF	pndef_comCmd10;
extern	const	PRMDEF	pndef_comCmd11;
extern	const	PRMDEF	pndef_comCmd12;
extern	const	PRMDEF	pndef_comCmd13;
extern	const	PRMDEF	pndef_comCmd14;
extern	const	PRMDEF	pndef_comCmd15;
extern	const	PRMDEF	pndef_comRsp0;
extern	const	PRMDEF	pndef_comRsp1;
extern	const	PRMDEF	pndef_comRsp2;
extern	const	PRMDEF	pndef_comRsp3;
extern	const	PRMDEF	pndef_comRsp4;
extern	const	PRMDEF	pndef_comRsp5;
extern	const	PRMDEF	pndef_comRsp6;
extern	const	PRMDEF	pndef_comRsp7;
extern	const	PRMDEF	pndef_comRsp8;
extern	const	PRMDEF	pndef_comRsp9;
extern	const	PRMDEF	pndef_comRsp10;
extern	const	PRMDEF	pndef_comRsp11;
extern	const	PRMDEF	pndef_comRsp12;
extern	const	PRMDEF	pndef_comRsp13;
extern	const	PRMDEF	pndef_comRsp14;
extern	const	PRMDEF	pndef_comRsp15;
extern	const	PRMDEF	pndef_OpCardID;
extern	const	PRMDEF	pndef_OpCardCtrlSw;
extern	const	PRMDEF	pndef_OpCardSwVerL;
extern	const	PRMDEF	pndef_OpCardSwVerH;
extern	const	PRMDEF	pndef_OpCardYNo;
extern	const	PRMDEF	pndef_OpCardDate;
extern	const	PRMDEF	pndef_OpCardSvSw;
extern	const	PRMDEF	pndef_OpCardAlmCode;
extern	const	PRMDEF	pndef_OpCardAlmAttr;
extern	const	PRMDEF	pndef_OpCardModel0;
extern	const	PRMDEF	pndef_OpCardModel1;
extern	const	PRMDEF	pndef_OpCardModel2;
extern	const	PRMDEF	pndef_OpCardModel3;
extern	const	PRMDEF	pndef_OpCardModel4;
extern	const	PRMDEF	pndef_OpCardModel5;
extern	const	PRMDEF	pndef_OpCardModel6;
extern	const	PRMDEF	pndef_OpCardModel7;
extern	const	PRMDEF	pndef_OpCardSNo0;
extern	const	PRMDEF	pndef_OpCardSNo1;
extern	const	PRMDEF	pndef_OpCardSNo2;
extern	const	PRMDEF	pndef_OpCardSNo3;
extern	const	PRMDEF	pndef_OpCardSNo4;
extern	const	PRMDEF	pndef_OpCardSNo5;
extern	const	PRMDEF	pndef_OpCardSNo6;
extern	const	PRMDEF	pndef_OpCardSNo7;
extern	const	PRMDEF	pndef_bnkNum;
extern	const	PRMDEF	pndef_bnkMemNum;
extern	const	PRMDEF	pndef_bnkMember0;
extern	const	PRMDEF	pndef_bnkMember1;
extern	const	PRMDEF	pndef_bnkMember2;
extern	const	PRMDEF	pndef_bnkMember3;
extern	const	PRMDEF	pndef_bnkMember4;
extern	const	PRMDEF	pndef_bnkMember5;
extern	const	PRMDEF	pndef_bnkMember6;
extern	const	PRMDEF	pndef_bnkMember7;
extern	const	PRMDEF	pndef_bnkMember8;
extern	const	PRMDEF	pndef_bnkMember9;
extern	const	PRMDEF	pndef_bnkMemberA;
extern	const	PRMDEF	pndef_bnkMemberB;
extern	const	PRMDEF	pndef_bnkMemberC;
extern	const	PRMDEF	pndef_bnkMemberD;
extern	const	PRMDEF	pndef_bnkMemberE;
extern	const	PRMDEF	pndef_bnkData00;
extern	const	PRMDEF	pndef_bnkData01;
extern	const	PRMDEF	pndef_bnkData02;
extern	const	PRMDEF	pndef_bnkData03;
extern	const	PRMDEF	pndef_bnkData04;
extern	const	PRMDEF	pndef_bnkData05;
extern	const	PRMDEF	pndef_bnkData06;
extern	const	PRMDEF	pndef_bnkData07;
extern	const	PRMDEF	pndef_bnkData08;
extern	const	PRMDEF	pndef_bnkData09;
extern	const	PRMDEF	pndef_bnkData0A;
extern	const	PRMDEF	pndef_bnkData0B;
extern	const	PRMDEF	pndef_bnkData0C;
extern	const	PRMDEF	pndef_bnkData0D;
extern	const	PRMDEF	pndef_bnkData0E;
extern	const	PRMDEF	pndef_bnkData0F;
extern	const	PRMDEF	pndef_bnkData10;
extern	const	PRMDEF	pndef_bnkData11;
extern	const	PRMDEF	pndef_bnkData12;
extern	const	PRMDEF	pndef_bnkData13;
extern	const	PRMDEF	pndef_bnkData14;
extern	const	PRMDEF	pndef_bnkData15;
extern	const	PRMDEF	pndef_bnkData16;
extern	const	PRMDEF	pndef_bnkData17;
extern	const	PRMDEF	pndef_bnkData18;
extern	const	PRMDEF	pndef_bnkData19;
extern	const	PRMDEF	pndef_bnkData1A;
extern	const	PRMDEF	pndef_bnkData1B;
extern	const	PRMDEF	pndef_bnkData1C;
extern	const	PRMDEF	pndef_bnkData1D;
extern	const	PRMDEF	pndef_bnkData1E;
extern	const	PRMDEF	pndef_bnkData1F;
extern	const	PRMDEF	pndef_bnkData20;
extern	const	PRMDEF	pndef_bnkData21;
extern	const	PRMDEF	pndef_bnkData22;
extern	const	PRMDEF	pndef_bnkData23;
extern	const	PRMDEF	pndef_bnkData24;
extern	const	PRMDEF	pndef_bnkData25;
extern	const	PRMDEF	pndef_bnkData26;
extern	const	PRMDEF	pndef_bnkData27;
extern	const	PRMDEF	pndef_bnkData28;
extern	const	PRMDEF	pndef_bnkData29;
extern	const	PRMDEF	pndef_bnkData2A;
extern	const	PRMDEF	pndef_bnkData2B;
extern	const	PRMDEF	pndef_bnkData2C;
extern	const	PRMDEF	pndef_bnkData2D;
extern	const	PRMDEF	pndef_bnkData2E;
extern	const	PRMDEF	pndef_bnkData2F;
extern	const	PRMDEF	pndef_bnkData30;
extern	const	PRMDEF	pndef_bnkData31;
extern	const	PRMDEF	pndef_bnkData32;
extern	const	PRMDEF	pndef_bnkData33;
extern	const	PRMDEF	pndef_bnkData34;
extern	const	PRMDEF	pndef_bnkData35;
extern	const	PRMDEF	pndef_bnkData36;
extern	const	PRMDEF	pndef_bnkData37;
extern	const	PRMDEF	pndef_bnkData38;
extern	const	PRMDEF	pndef_bnkData39;
extern	const	PRMDEF	pndef_bnkData3A;
extern	const	PRMDEF	pndef_bnkData3B;
extern	const	PRMDEF	pndef_bnkData3C;
extern	const	PRMDEF	pndef_bnkData3D;
extern	const	PRMDEF	pndef_bnkData3E;
extern	const	PRMDEF	pndef_bnkData3F;
extern	const	PRMDEF	pndef_prmVer;
extern	const	PRMDEF	pndef_spdDivision;
extern	const	PRMDEF	pndef_CmdOpSw;
extern	const	PRMDEF	pndef_sys_rsrv2;
extern	const	PRMDEF	pndef_sys_rsrv3;
extern	const	PRMDEF	pndef_cmdopv;
/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/
