/****************************************************************************/
/*																			*/
/*																			*/
/*		Servo - Option Card Interface 定義									*/
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
/*																			*/
/*		Rev.1.00 : 2007.02.01  (Den-S-Kai)									*/
/*					Comment rearranging for Sigma5 BaseSoft					*/
/*		Rev.2.00 : 2013.09.30 <S04D>										*/
/*					SigamV -> Sigma7										*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		SERVO_OPTCARD_IF_H_
#define		SERVO_OPTCARD_IF_H_

#include "EncData.h"
#include "AlarmManager.h"
#include "PnPrmStruct.h"

/****************************************************************************/
/*		Servo - Option Card Interface Version								*/
/****************************************************************************/
#define SERVO_COMOPT_IF_VER	0x0001
#define SERVO_SFTOPT_IF_VER	0x0001
#define SERVO_FBKOPT_IF_VER	0x0001

/****************************************************************************/
/*																			*/
/*		Card Type Definition 												*/
/*																			*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/* 		Defines Option Card Detect Status									*/
/*--------------------------------------------------------------------------*/
#define STS_NO_CONNECT	0x0			/* Not Connected						*/
#define STS_DET_NORMAL	0x1			/* Normally Detected					*/
#define STS_DET_FAIL	0x2			/* Detection Failed						*/
#define STS_DET_UNMATCH	0x3			/* Detected but Unmatched				*/
#define STS_DET_ERR		0x7			/* Detected but Unsupported/ComErr		*/

/*--------------------------------------------------------------------------*/
/* 		Defines Option Card Type 											*/
/*--------------------------------------------------------------------------*/
enum OPT_CARDTYPE {
	CARDTYPE_CMPOP = 0,				/* 0: Command Option Card				*/
	CARDTYPE_SFTOP,					/* 1: Safety Option Card				*/
	CARDTYPE_FBKOP,					/* 2: Feedback Option Card				*/
};

/*--------------------------------------------------------------------------*/
/* 		Defines Command Option Card I/F Type 								*/
/*--------------------------------------------------------------------------*/
enum COMMANDOPT_IFTYPE {
	CMDOP_NOT_CONNECTED = 0,		/* 0: Not Connected						*/
	CMDOP_NOT_SUPPORTED,			/* 1: Not Supported Card (Unknown)		*/
	CMDOP_DPM_NETXIF,				/* 2: DPM netX Card						*/
	CMDOP_DPM_CMNIF,				/* 3: DPM Card							*/
	CMDOP_M2_IF,					/* 4: M2 Bus Card						*/
};

/*--------------------------------------------------------------------------*/
/* 		Defines Safety Option Card I/F Type 								*/
/*--------------------------------------------------------------------------*/
enum SAFETYOPT_IFTYPE {
	SFTOP_NOT_CONNECTED = 0,		/* 0: Not Connected						*/
	SFTOP_NOT_SUPPORTED,			/* 1: Not Supported Card (Unknown)		*/
	SFTOP_ADVANCED_SAFETY_IF,		/* 2: Advanced Safety Card				*/
};

/*--------------------------------------------------------------------------*/
/* 		Defines Feedback Option Card I/F Type 								*/
/*--------------------------------------------------------------------------*/
enum FEEDBACKOPT_IFTYPE {
	FBKOP_NOT_CONNECTED = 0,		/* 0: Not Connected						*/
	FBKOP_NOT_SUPPORTED,			/* 1: Not Supported Module (Unknown)	*/
	FBKOP_FULLCLS_ONLY,				/* 2: Used for only Full-Closed control	*/
	FBKOP_SCNV_MODULE,				/* 3: Convert sensor signal into a serial*/
};



/*--------------------------------------------------------------------------*/
/* 		Defines Command Option Card ID Type 								*/
/*--------------------------------------------------------------------------*/
#define CMDOP_ID_NONE		0xFFFF	/* NotConnected/NotSupported/ComError	*/
/*--------------------------------------------------------------------------*/
/* 		Defines Safety Option Card ID Type 									*/
/*--------------------------------------------------------------------------*/
#define SFTOP_ID_NONE		0xFFFF	/* NotConnected/NotSupported/ComError	*/
/*--------------------------------------------------------------------------*/
/* 		Defines Feedback Option Card ID Type 								*/
/*--------------------------------------------------------------------------*/
#define FBKOP_ID_NONE		0xFFFF	/* NotConnected/NotSupported/ComError	*/
#define FBKOP_ID_FULLCLOSE	0x0001	/* Full Closed I/F Card					*/





/****************************************************************************/
/*																			*/
/*		構造体定義			 												*/
/*																			*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/* 		 OPTION - SERVO I/F オプション共通部定義	 						*/
/*--------------------------------------------------------------------------*/
typedef struct {
/*--------------------------------------------------------------------------*/
		struct {
			UINT	Connect			:1;	/* Option Card Connection Status	*/
			UINT	WrEepReq		:1;	/* Option ID EEPROM Write Request	*/
			UINT	SpareBit02		:1;	/* Spare02							*/
			UINT	SpareBit03		:1;	/* Spare03							*/
			UINT	SpareBit04		:1;	/* Spare04							*/
			UINT	SpareBit05		:1;	/* Spare05							*/
			UINT	SpareBit06		:1;	/* Spare06							*/
			UINT	SpareBit07		:1;	/* Spare07							*/
			UCHAR	SpareBit08_0F;		/* Spare08-0F						*/
		} f;
/*--------------------------------------------------------------------------*/
		UCHAR		Date[2];		/* Option Module Manufactured Date		*/
		UCHAR		ModelStr[24];	/* Option Module Name					*/
		UCHAR		SerNumStr[24];	/* Option Module Serial Number			*/
/*--------------------------------------------------------------------------*/
		USHORT		IfSuppVer;		/* Option Module Supported I/f Version	*/
		USHORT		IfType;			/* Option Module I/f Type				*/
		USHORT		IdType;			/* Option Module ID Type				*/
		USHORT		YNumber;		/* Option Module Y-Spec Number			*/
/*--------------------------------------------------------------------------*/
		USHORT		SoftVer;		/* Option Module Software Version		*/
		USHORT		SoftYspVer;		/* Option Module Software Y-Spec Version*/
		USHORT		SoftTstVer;		/* Option Module Software Test Version	*/
		USHORT		PrmVer;			/* Option Module Parameter Version		*/
/*--------------------------------------------------------------------------*/
		USHORT		LastIfType;		/* Last Option I/f Type					*/
		USHORT		LastIdType;		/* Last Option ID Type					*/
/*--------------------------------------------------------------------------*/
} OPTCARD_INFO;



/*==============================================================================*/
/* 		 Command Option - Servo I/F オプション依存部データ定義					*/
/*==============================================================================*/
typedef struct {
/*-- 固定データ定義 ------------------------------------------------------------*/
	struct {
			OPTCARD_INFO	OpInfo;
	} conf;
/*-- 可変データ定義 ------------------------------------------------------------*/
	struct {
		struct {							/* ビット信号						*/
			UCHAR	Spare00; 				/* Spare00							*/
			UCHAR	Spare01; 				/* Spare01							*/
			UCHAR	Spare02; 				/* Spare02							*/
			UCHAR	Spare03; 				/* Spare03							*/
		} f;
	} var;
/*------------------------------------------------------------------------------*/
} OPCMD_HNDL;




/*==============================================================================*/
/* 		 Safety Option - Servo I/F オプション依存部データ定義					*/
/*==============================================================================*/
typedef struct {
/*-- 固定データ定義 ------------------------------------------------------------*/
	struct {
			OPTCARD_INFO	OpInfo;
	} conf;
/*-- 可変データ定義 ------------------------------------------------------------*/
	struct {
		struct {							/* ビット信号						*/
			UCHAR	Spare00; 				/* Spare00							*/
			UCHAR	Spare01; 				/* Spare01							*/
			UCHAR	Spare02; 				/* Spare02							*/
			UCHAR	Spare03; 				/* Spare03							*/
		} f;
	} var;
/*------------------------------------------------------------------------------*/
} OPSFT_HNDL;


/*==============================================================================*/
/* 		 Feedback Option - Servo I/F オプション依存部データ定義					*/
/*==============================================================================*/
typedef struct {
/*-- 固定データ定義 ------------------------------------------------------------*/
		struct {
			OPTCARD_INFO	OpInfo;
			UCHAR			PrmMakerID[16];	/* Motor/Enc Prm Manufacturer ID	*/
			/*------------------------------------------------------------------*/
			IDINFO			MotInfo;		/* Motor Information				*/
			IDINFO			EncInfo;		/* Encoder Information				*/
			/*------------------------------------------------------------------*/
			/*	フィードバックオプション内情報（機種定義レジスタなどで使用）	*/
			/*------------------------------------------------------------------*/
			USHORT			MotID;			/* モータID							*/
			USHORT			MotVolt;		/* モータ電圧						*/
			USHORT			EncType;		/* エンコーダタイプ/分解能			*/
			USHORT			AbsEncType;		/* 絶対値エンコーダタイプ			*/
			USHORT			EncVer;			/* エンコーダファームウェアバージョン*/
			/*------------------------------------------------------------------*/
			DWORDX			MotWat;			/* モータ容量						*/
			LONG			XfbPulse2;		/* エンコーダ分解能					*/
			/*------------------------------------------------------------------*/
			USHORT			PgBaudRateType;	/* エンコーダ通信速度設定タイプ		*/
			USHORT			PgBaudRate;		/* エンコーダ通信速度設定値			*/
			/*------------------------------------------------------------------*/
		} conf;
/*-- 可変データ定義 ------------------------------------------------------------*/
		struct {
			struct {						/* ビット信号						*/
				UINT	ChekedConnect	:1;	/* 接続チェック済みフラグ			*/
				UINT	Connect			:1;	/* 接続チェック結果					*/
				UINT	PrmSumErrAsSemi	:1;	/* FBOPTのSUM異常(セミク使用時)		*/
				UINT	PrmSumErrAsFulc	:1;	/* FBOPTのSUM異常(フルク使用時)		*/
				UINT	FullclosedUse	:1;	/* FBOPTのFull-closed使用履歴		*/
				UINT	SpareBit05		:1;	/* Spare05							*/
				UINT	SpareBit06		:1;	/* Spare06							*/
				UINT	SpareBit07		:1;	/* Spare07							*/
			} f;
		} var;
/*------------------------------------------------------------------------------*/
} OPFBK_HNDL;



/*==============================================================================*/
/* 		Option Card - Servo I/F オプション定義									*/
/*==============================================================================*/
typedef struct {
	OPCMD_HNDL	CmdOpt;		/* 指令オプションデータへのポインタ				*//* 未実装	*/
	OPSFT_HNDL	SftOpt;		/* セーフティーモジュールデータへのポインタ		*//* 未実装	*/
	OPFBK_HNDL	FbkOpt;		/* フィードバックオプションデータへのポインタ	*/
	USHORT		OpDetInfo;	/* オプションモジュール接続状態					*/
} OPTCARD_HNDL;



/****************************************************************************/
/*																			*/
/*		Option API Functions Prototype Definition							*/
/*																			*/
/****************************************************************************/
void OpCardInitOptionVariables( OPTCARD_HNDL *OptCardHdl, PRMDATA *Prm );
void OpCardDetectOptions( PRMDATA *Prm, OPTCARD_HNDL *OptCardHdl, ALMMAN_HANDLE *AlmManager, ASIC_HW_IF_STR *AsicHwReg );
void OpCardWriteEepromOptionId( OPTCARD_HNDL *OptCardHdl, LONG ax_no );
void OpCardSetLastOptionId( OPTCARD_HNDL *OptCardHdl );
void OpCardEepDeviceInit( void );



/*--------------------------------------------------------------------------*/
/*		Command Option Service Function										*/
/*--------------------------------------------------------------------------*/
/* Initialize Servo-Command Option Variables	*/
void	KpxInitSvCommandOptVar( OPCMD_HNDL *CmdOpt, PRMDATA *Prm );
#if CSW_OPT_CARD_SAFETY == CSW_SAFETY_NO_SUPPORT
	#define	KpxDetectCommandOption( ) { ;}
#else	/* CSW_SAFETY_NO_SUPPORT	*/
	void	KpxDetectCommandOption(void);
	void	KpxSetCommandOptID( const OPINFO_DATA *pOpInfo );
	void	KpiSetOptDetectInfo( USHORT, USHORT );
#endif	/* CSW_OPT_CARD_SAFETY	*/
#define KPI_GET_CMDOP_TYPE()	OptCardHdl->CmdOpt.conf.OpInfo.IfType


/*--------------------------------------------------------------------------*/
/*		Safety Option Service Function										*/
/*--------------------------------------------------------------------------*/
/* Initialize Servo-Safety Option Variables	*/
void	SftOptInitSvSafetyOptVar( OPSFT_HNDL *SftOpt );
/* Detect Safety Option Card				*/
void	SftOptDetectSafetyOption( OPSFT_HNDL *SftOpt, ALMMAN_HANDLE *AlmManager );


/*--------------------------------------------------------------------------*/
/*		Feedback Option Service Function										*/
/*--------------------------------------------------------------------------*/
/* Initialize Servo-Feedback Option Variables	*/
#if(PRODUCT_SERIES_SEVEN==TRUE)                                       /* <S09F> */
void	FbOptInitSvFeedbackOptVar( OPFBK_HNDL *FbkOpt, PRMDATA *Prm );
#if CSW_OPT_CARD_FBOPTION == CSW_FBOPTION_NO_SUPPORT
	#define	FbOptDetectFeedbackOption( x1, x2, x3, x4 ) { ;}
	#define	FbOptSetIDInfo( x1, x2, x3 ) { ;}
	#define	FbOptCheckConnectNonSupport( ) { ;}
	#define	FbOptInitRegister( x1, x2, x3, x4 ) { ;}
	#define	FbOptPrstSelectEncoder( x1, x2, x3 ) FALSE
	#define	FbOptFullSemiPrmChk( x1, x2, x3, x4, x5 ) TRUE
	
#else	/* CSW_FBOPTION_SUPPORT */

	/* Detect Feedback Option Card						*/
	void	FbOptDetectFeedbackOption( USHORT *OpDetInfo, OPFBK_HNDL *FbkOpt, ALMMAN_HANDLE *AlmManager, ASIC_HW_IF_STR *AsicHwReg );
	/* Handshake with serial conversion module			*/
	LONG	FbOptSCMHandShake( OPFBK_HNDL *FbkOpt, ASIC_HW_IF_STR *AsicHwReg );
	/* Feedback Option Motor/Encoder Info				*/
	void	FbOptSetIDInfo( OPFBK_HNDL *FbkOpt, MENCV *MencV, BOOL SencFbOptUse );
	/* Check Non-support								*/
	void	FbOptCheckConnectNonSupport( void );	
	/* Set Feedback Option Info to Register				*/
	void	FbOptInitRegister(OPFBK_HNDL *FbkOpt, MENCV *FencV, PRMDATA* Prm, BPRMDAT* Bprm);
	/* Select encoder for the reset						*/
	BOOL	FbOptPrstSelectEncoder(OPFBK_HNDL *FbkOpt, PRMDATA* Prm, BPRMDAT* Bprm );
	/* Parameter check 									*/
	LONG	FbOptFullSemiPrmChk( MENCV *MencV, OPFBK_HNDL *FbkOpt, PRMDATA* Prm, BPRMDAT* Bprm, LONG ChkSw );
#endif	/* CSW_OPT_CARD_FBOPTION	*/
#else
void	FbOptInitSvFeedbackOptVar( OPFBK_HNDL *FbkOpt, PRMDATA *Prm );
//#if CSW_OPT_CARD_FBOPTION == CSW_FBOPTION_NO_SUPPORT              /* <S09F> */
//	#define	FbOptDetectFeedbackOption( ) { ;}                       /* <S09F> */
//	#define	FbOptSetIDInfo( ) { ;}                                  /* <S09F> */
//	#define	FbOptCheckConnectNonSupport( ) { ;}                     /* <S09F> */
//	#define	FbOptInitRegister( ) { ;}                               /* <S09F> */
//	#define	FbOptPrstSelectEncoder( ) { ;}                          /* <S09F> */
//	#define	FbOptFullSemiPrmChk( ) { ;}                             /* <S09F> */
	
//#else	/* CSW_FBOPTION_SUPPORT */                                  /* <S09F> */

	/* Detect Feedback Option Card						*/
	void	FbOptDetectFeedbackOption( USHORT *OpDetInfo, OPFBK_HNDL *FbkOpt, ALMMAN_HANDLE *AlmManager, ASIC_HW_IF_STR *AsicHwReg );
	/* Handshake with serial conversion module			*/
	LONG	FbOptSCMHandShake( OPFBK_HNDL *FbkOpt, ASIC_HW_IF_STR *AsicHwReg );
	/* Feedback Option Motor/Encoder Info				*/
	void	FbOptSetIDInfo( OPFBK_HNDL *FbkOpt, MENCV *MencV, BOOL SencFbOptUse );
	/* Check Non-support								*/
	void	FbOptCheckConnectNonSupport( void );	
	/* Set Feedback Option Info to Register				*/
	void	FbOptInitRegister(OPFBK_HNDL *FbkOpt, MENCV *FencV, PRMDATA* Prm, BPRMDAT* Bprm);
	/* Select encoder for the reset						*/
	BOOL	FbOptPrstSelectEncoder(OPFBK_HNDL *FbkOpt, PRMDATA* Prm, BPRMDAT* Bprm );
	/* Parameter check 									*/
	LONG	FbOptFullSemiPrmChk( MENCV *MencV, OPFBK_HNDL *FbkOpt, PRMDATA* Prm, BPRMDAT* Bprm, LONG ChkSw );
//#endif	/* CSW_OPT_CARD_FBOPTION	*/                  /* <S09F> */
#endif

#endif		/* SERVO_OPTCARD_IF_H_ */
