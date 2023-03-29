#ifndef __TABLE_DRIVE_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		TableDrive.h : テーブル運転用	モジュール定義												*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2011.02.18	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "CtrlLoopHandler.h"
#include "MfcHandler.h"
#include "PrmAccessIf.h"
#include "DataTraceManager.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転ゼロ制御指令定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef union {							/* テーブル運転制御指令定義									*/
	struct	{							/* ゼロ制御モードメンバ定義									*/
		UCHAR	zm;						/* ゼロ制御モード											*/
		UCHAR	zmchg;					/* ゼロ制御モード変化情報									*/
		UCHAR	zmlast;					/* 予備														*/
		UCHAR	Dummy;					/* 予備														*/
	} b;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;							/* 制御モード編集用(Copy,etc)								*/
} TBLZMD;


/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行制御用定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TBLDRIVE {
	struct {
		USHORT	TblDriveDataSize;		/* 0x2101 : テーブル運転データ長							*/
		USHORT	TblDriveSampIntv;		/* 0x2103 : テーブル運転サンプリング間隔					*/
		USHORT	TblMode;				/* 0x2300 : テーブル運転制御モード							*/
		USHORT	TblLimit;				/* 0x2301 : テーブル運転回転量(移動量)リミット				*/
		USHORT	TblState;				/* 0x2302 : テーブル運転完了								*/
		USHORT	TblFuncSel;				/* 0x2303 : テーブル運転機能選択							*/
	} OpeReg;
	/*----------------------------------------------------------------------------------------------*/
	struct {
		UCHAR	TblPrmOk;				/* テーブル運転パラメータＯＫ								*/
		UCHAR	TblMode;				/* テーブル運転モード										*/
		USHORT	CtrlMode;				/* テーブル運転制御モード									*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	TrqInputPos;			/* テーブル運転トルク入力位置								*/
		UCHAR	TuneLessDisable;		/* 調整レス制御無効											*/
		UCHAR	EGearDisable;			/* 電子ギア比無効											*/
		UCHAR	PcmdFilDisable;			/* 位置指令フィルタ無効										*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	MotPulse;				/* モータパルス数 [pulse/rev, pulse/mm]						*/
		LONG	MtnLimit;				/* マルチターンリミット										*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	RefType;				/* 指令タイプ												*/
		UCHAR	RefIpSft;				/* 指令補間シフト数											*/
		USHORT	RefIpNum;				/* 指令補間回数												*/
		ULONG	TblDataNum;				/* テーブル運転指令データ個数								*/
		ULONG	DataSampIntv;			/* テーブルデータサンプリング間隔							*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	KoutDir;				/* テーブル運転指令出力ゲイン(正規化ゲイン＋方向)			*/
		LONG	*RefOutPx;				/* テーブル運転指令出力ポインタ								*/
	/*----------------------------------------------------------------------------------------------*/
		void	*TblBuffer;				/* テーブル運転指令バッファ									*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {
		TBLZMD	TblZmd;					/* テーブル運転ゼロ制御モード指令							*/
		UCHAR	TblCmd;					/* テーブル運転実行指令										*/
		UCHAR	TblStep;				/* テーブル運転実行ステップ									*/
		USHORT	TblSamp;				/* テーブル運転サンプリング									*/
		USHORT	TblIndx;				/* テーブル運転インデクス									*/
		USHORT	MregCsw;				/* ASICマイクロトルクフィルタ制御(無効／復元)用				*/
		USHORT	ErrState;				/* 異常状態													*/
		USHORT	Dummy;					/* For Alignment											*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	MotMtn;					/* テーブル運転マルチターン			[rev,mm]				*/
		LONG	MotPos;					/* テーブル運転モータ位置			[pulse]					*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	TrqRef;					/* テーブル運転トルク指令出力		[NorTrq]				*/
		LONG	SpdRef;					/* テーブル運転速度指令出力			[NorSpd]				*/
		LONG	dPosRef;				/* テーブル運転位置指令出力			[pulse/scan]			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	InPosRef;				/* テーブル運転位置指令補間演算用	[pulse/samp]			*/
		LONG	IxPosRef;				/* テーブル運転位置指令補間演算用	[pulse/scan]			*/
		LONG	IxPosRem;				/* テーブル運転位置指令補間演算用	[pulse]					*/
		LONG	WkPosRem;				/* テーブル運転位置指令補間演算用	[pulse]					*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL	KeepTblMode;			/* テーブル運転継続フラグ									*/
		BOOL	TrqFilDisable;			/* トルク指令フィルタ無効要求								*/
		BOOL	NotchLpassDisable;		/* ノッチ＆ローパスフィルタ無効要求							*/
		LONG	DisTrqInTiming;			/* 外乱トルク指令入力位置									*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL	WithTrcReq;				/* テーブル運転同時トレース要求								*/
	} var;
	TRCHNDL		*TrcHndl;				/* データトレースハンドル */
} TBLDRIVE;


typedef	struct TBLCHK_SV_STS {
	BOOL	AlmFlag;				/* アラーム発生中フラグ											*/
	BOOL	HwbbSts;				/* HWBB状態検出用データ											*/
	BOOL	MainPowerOn;			/* 主回路ON状態													*/
	BOOL	OverTrvlSts;			/* OT状態(OTシーケンス処理出力)									*/
	BOOL	BeComplete;				/* ベースイネーブル完了状態										*/
	BOOL	TrqClamp;				/* トルク制限中フラグ											*/
	LONG	dMotPos;				/* 位置FB増分値[pulse/ScanB]									*/
} TBLCHK_SV_STS;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行指令定義																	*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_EXE_STS {
	TBLCMD_NOCMD		= 0x00,				/* 指令なし												*/
	TBLCMD_TABLESTART,						/* テーブル運転処理開始指令								*/
	TBLCMD_TABLEDRIVE,						/* テーブル運転実行開始指令								*/
	TBLCMD_TBLDRV_END,						/* テーブル運転実行正常終了								*/
	TBLCMD_TBLDRV_ERR	= 0x0E,				/* テーブル運転実行異常終了								*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転状態定義																		*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_SERVO_STS {
	TBLCMD_STS_INIT			= 0,			/* イニシャル状態										*/
	TBLCMD_STS_RUN,							/* テーブル運転中										*/
	TBLCMD_STS_FINISH,						/* テーブル運転完了										*/
	TBLCMD_STS_POSLMT		= 100,			/* 回転量/移動量リミット検出							*/
	TBLCMD_STS_TRQLMT,						/* トルク制限検出										*/
	TBLCMD_STS_BB,							/* サーボオフ											*/
	TBLCMD_STS_OT,							/* オーバトラベル										*/
	TBLCMD_STS_POWEROFF,					/* 主回路電源オフ										*/
	TBLCMD_STS_ALARM,						/* アラーム発生											*/
	TBLCMD_STS_HWBB,						/* HWBB状態												*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転指令タイプ定義																	*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_REF_TYPE {
	TBLREFTYPE_NOTPOS		= 0x00,			/* 位置指令以外(速度,トルク)							*/
	TBLREFTYPE_POSIP1,						/* 位置指令：補間無し									*/
	TBLREFTYPE_POSIP2,						/* 位置指令：シフト補間									*/
	TBLREFTYPE_POSIP3,						/* 位置指令：除算補間									*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転指令データ長定義																*/
/*--------------------------------------------------------------------------------------------------*/
enum TBLCMD_REF_LENGTH {
	TBLREFLENGTH_NOTDEF		= 0x00,			/* 未定義(SHORT)										*/
	TBLREFLENGTH_15BIT_IO,					/* Data:15bit + IO:1bit									*/
	TBLREFLENGTH_31BIT_IO,					/* Data:31bit + IO:1bit									*/
	TBLREFLENGTH_16BIT,						/* Data:16bit											*/
	TBLREFLENGTH_32BIT,						/* Data:32bit											*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転選択ビット定義																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	TBLFUNCBIT_TUNELESS		0x0100		/* 調整レス制御											*/
#define	TBLFUNCBIT_EGEAR		0x0200		/* 電子ギア比											*/
#define	TBLFUNCBIT_PCMDFIL		0x0400		/* 位置指令フィルタ										*/


/* テーブル運転バッファは1軸につき0x1000[2byte]とする。Σ-Ⅴは0xC00[2byte]なので余裕あり */
//#define	TBL_BUF_SIZE		0x30000		/* テーブル運転用指令バッファサイズ[2byte]				*/
//#define	AX_TBL_BUF_SIZE		0x6000		/* 軸毎のテーブル運転用指令バッファサイズ[2byte]		*/
#define		AX_TBL_BUF_SIZE		0x1000		/* 軸毎のテーブル運転用指令バッファサイズ[2byte]		*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void		TblDriveInitialize( TBLDRIVE *TblDrive, USHORT *BufferAddress, TRCHNDL *TrcHndl );
void		TblDriveGetTraceRegData( TBLDRIVE *TblDrive, USHORT DataSize, USHORT SampIntv,
																ULONG DataSampIntv, ULONG DataNUm );
void		TblDriveExecute( TBLDRIVE *TblDrive, TBLCHK_SV_STS *TblChkSts,
														POS_MNG_HNDL *PosManager, MFCTRL *MFControl );
PRM_RSLT	TblDriveCalculateParameter( TBLDRIVE *TblDrive, LONG XfbPulse1 );
PRM_RSLT	TblDrivePrepareProc( TBLDRIVE *TblDrive );
PRM_RSLT	TblDriveStartRequest( TBLDRIVE *TblDrive, LONG TblDir );
PRM_RSLT	TblDriveSetStopRequest( TBLDRIVE *TblDrive );
USHORT		TblDriveGetControlMode( TBLDRIVE *TblDrive );
void		TblDriveSetWithTrcReq( TBLDRIVE *TblDrive, BOOL Switch );




/****************************************************************************************************/
#define __TABLE_DRIVE_H
#endif /* __TABLE_DRIVE_H */
