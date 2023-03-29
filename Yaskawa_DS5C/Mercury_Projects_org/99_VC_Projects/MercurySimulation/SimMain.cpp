/****************************************************************************
Description	: Mercury visual servo main module
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

Functions		:
	_tWinMain()		:Visual Servo Simulation Main

----------------------------------------------------------------------------
Changes			:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.08.24	created

****************************************************************************/
#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#include "SimMain.h"
#include "resource.h"
#include "MercuryGlobals.h"


/* global functions */
extern "C" {
	void	MercuryMain( void );
	void	SysScanAInt( void );
	void	SysScanBInt( void );
	void	SysScanCInt( void );
	void	SysRoundTask( void );
	USHORT	SvifGetAlmCode( unsigned char axno );
	void	SvifSetEncoderData( unsigned char *Data, unsigned char axno );
	void	SvifSetControlMode( USHORT ControlMode, unsigned char axno );
	void	SvifSetPulseRef( ULONG Pulse, unsigned char axno );
	void	SvifSetSpeedRefAd( USHORT SpeedRef, unsigned char axno );
	void	SvifSetAlmMaskPGCOM( unsigned char axno );
	SHORT	SvifGetTrqRef( unsigned char axno );
	SHORT	SvifGetMotSpd( unsigned char axno );
	SHORT	SvifGetMotSpdRef( unsigned char axno );

	SHORT	SvifGetSpdPacOut( unsigned char axno );
	SHORT	SvifGetSpdIacOut( unsigned char axno );

	void	SvifSendMemobusMsg( UCHAR *TxBuf, UCHAR TxSize );
	void	SvifSetTraceControl( UCHAR TraceNum, UCHAR DataNum, USHORT *Target, USHORT Sampling,
														USHORT PreTrigger, USHORT TrigCondition );


	void	GetMotorAmpParameter( USHORT *MotPrmBuf, USHORT *AmpPrmBuf, UCHAR axno );

	void	InitializeMotorModel( void );
	void	SetMotorAmpParameter( unsigned short *MotPrm,unsigned short *AmpPrm, unsigned char axno );
	void	CalcMotorFeedback( double Vu, double Vv, double Vw, double *IuFb, double *IvFb, unsigned char axno );
	void	GetEncoderInfo( void *EncoderData, unsigned char axno );


	void	MicroCallMpStart( unsigned char axno );
	void	MicroCallMpIntAD( double *Vu, double *Vv, double *Vw, double *IuFb, double *IvFb, unsigned char axno );
	void	MicroCallMpIntHost( void );

	void	InitPositionReference( void );
	UCHAR	SetPositionReference( LONG Tpos, LONG Tspd, LONG AccTime, LONG DecTime );
	LONG	MakeCurrentReference( UCHAR *Den );


}

/* Window関連 */
#define MAX_LOADSTRING 100
#define WINDOW_WIDTH  (400)		// ウィンドウの幅
#define WINDOW_HEIGHT (300)		// ウィンドウの高さ
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)

/* Timer関連 */
#define TIMER_ID	 (100)		// 作成するタイマの識別ID
#define TIMER_ELAPSE (50)		// WM_TIMERの発生間隔

/* Drawing関連 */
#define		OriginX		60
#define		OriginY		340
#define		EndX		560
#define		EndY		40
#define		OneStepX	50
#define		OneStepY	30
#define		SpaceSizeX	600
#define		SpaceSizeY	370
#define		ZeroPosX	60
#define		ZeroPosY	190
#define		DrawGainUS	300/65535
#define		DrawGainS	300/32767*10


// グローバル変数:
HINSTANCE hInst;								/* 現在のインターフェイス */
TCHAR szTitle[MAX_LOADSTRING];					/* タイトル バーのテキスト */
TCHAR szWindowClass[MAX_LOADSTRING];			/* メイン ウィンドウ クラス名 */

// Drawing関連
POINT       points[5];
HBRUSH      hdefb, hbrackb, hredb, hblueb, hyellowb;
HPEN        hdefp, GreenPen, RedPen, YellowPen;
HPEN        WhitePen, WhiteDotPen;

// サーボ制御用変数
USHORT	MotPrm[SIM_AXISNUM][8];
USHORT	AmpPrm[SIM_AXISNUM][2];
UCHAR	EncoderData[SIM_AXISNUM][12];
double	Vu[SIM_AXISNUM], Vv[SIM_AXISNUM], Vw[SIM_AXISNUM];
double	IuFb[SIM_AXISNUM], IvFb[SIM_AXISNUM];
UCHAR	TraceCmpFlg;
UCHAR	SimStartFlg;
SHORT	TraceBuf0[TRACE_BUF_SIZE];
SHORT	TraceBuf1[TRACE_BUF_SIZE];
SHORT	TraceBuf2[TRACE_BUF_SIZE];
ULONG	OperatingTimer;			/* 電流ループ周期カウンタ */
ULONG	PosRefCounter;
int		TraceCounter;
UCHAR	CtrlMode;
LONG	dPositionRef;
LONG	PositionRef;
LONG	PositionFb;
USHORT	SpeedRef;				/* Speed Reference [min-1] */
LONG	SpeedFb;
LONG	AccTime;
LONG	DecTime;

/* Memobus Message用変数 */
UCHAR	MemobusBuf[50];
UCHAR	MessageSize;
UCHAR	MemobusTxReq;

/* Trace用変数 */
TRACE_INFO	TraceInfo;
double		TraceGain[3];
double		TraceOffset[3];

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

int ServoSimMain( void );
void SetAlmCode2Buf( USHORT AlmCode, CHAR *Buffer );
void InitializeDialogItems( HWND *hDlg, HDC *hMemDC, HBITMAP *hBitmap );
void GetTraceSetting( HWND *hDlg );
void GetSimulationSetting( HWND *hDlg );
int InitDraw( void );
int releaseObj( void );
void DrawTraceBackGround( HDC *hMemDC );
void DrawTraceWave( HDC *hMemDC );


// タイマ関数
typedef void (CALLBACK TIMECALLBACK)(
	UINT			uTimerID,
	UINT			nMsg,
	DWORD_PTR		dwUser,
	DWORD_PTR		dwParam1,
	DWORD_PTR		dwParam2);
typedef TIMECALLBACK FAR *LPTIMECALLBACK;

WINMMAPI MMRESULT WINAPI timeSetEvent(
	UINT			uDelay,
	UINT			uResolution,
	LPTIMECALLBACK	lpTimeProc,
	DWORD_PTR		dwUser,
	UINT			fuEvent);

// 関数呼び出し
void CALLBACK TimerCallback(
	UINT			uTimerID,
	UINT			nMsg,
	DWORD_PTR		dwUser,
	DWORD_PTR		dwParam1,
	DWORD_PTR		dwParam2)
{
	ServoSimMain();
}



/*******************************************************************************
*  Description : VC Simulation Main function
*  Parameter   : -
*  Return      : -
*******************************************************************************/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	int i;
	HWND hWnd;
	MSG msg;
	HACCEL hAccelTable;
	UCHAR ret;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MENU1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	hWnd = InitInstance(hInstance, nCmdShow);
	if( hWnd == NULL )
	{
		MessageBox( NULL, _T("ウィンドウの作成に失敗しました"), _T("エラー"), MB_OK );
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MENU1));

	// サーボ初期化処理実行
	/* Initialize Servo Control */
	MercuryMain( );
	/* Initialize Motor Model Control */
	for( i=0; i<SIM_AXISNUM; i++ )
	{
		GetMotorAmpParameter( MotPrm[i], AmpPrm[i], i );
		SetMotorAmpParameter( MotPrm[i], AmpPrm[i], i );
		Vu[i] = Vv[i] = Vw[i] = 0.0;
		IuFb[i] = IvFb[i] = 0.0;
	}
	/* Prepare the Simulation */
	MicroCallMpStart( SIM_AXISNUM );
	InitializeMotorModel( );
	InitPositionReference( );
	OperatingTimer = 0;
	PosRefCounter = 0;
	TraceCounter = 0;
	TraceCmpFlg = FALSE;
	SimStartFlg = FALSE;
	for( i=0; i<SIM_AXISNUM; i++ )
	{
		SvifSetAlmMaskPGCOM( i );
	}

//	SpeedRef = 0;
//	SetPositionReference( 1048576, 500, 10, 10 );


	// メイン メッセージ ループ:
	while (1)
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ret = GetMessage(&msg, NULL, 0, 0);
		if( ret == 0 || ret == -1 )
		{
			break;
		}
	}

	return (int) msg.wParam;
}



/*******************************************************************************
*  Description : Servo Simulation Main function
*  Parameter   : -
*  Return      : -
*******************************************************************************/
int	ServoSimMain( void )
{
	int		i;
	UCHAR	DenStatus;

	/* Check Simulation Start Request */
	if( SimStartFlg == FALSE )
	{
		return 0;
	}

	/* サーボ制御処理シミュレーション */
	/* for debug */
	if( (OperatingTimer % (SCANA_EXE_TIMING*10)) == 0 )
//	if( (OperatingTimer % 4) == 0 )
	{ /* サンプリングタイム設定 */
		if( TraceCounter < TRACE_BUF_SIZE )
		{
			/* q軸指令 */
//			TraceBuf0[TraceCounter] = GetqAxisReference( );
			/* q軸フィードバック */
//			TraceBuf1[TraceCounter] = GetqAxisFeedback( );
			/* q軸電圧指令 */
//			TraceBuf2[TraceCounter] = GetqAxisVReference( );

			/* 比例制御出力 */
//			TraceBuf0[TraceCounter] = SvifGetSpdPacOut( 0 );
			/* 積分制御出力 */
//			TraceBuf1[TraceCounter] = SvifGetSpdIacOut( 0 );

			/* 位置指令速度 */
			TraceBuf0[TraceCounter] = (short)((double)15000 * dPositionRef / 14418);
			/* 速度指令 */
//			TraceBuf0[TraceCounter] = SvifGetMotSpdRef( 0 );
			/* モータ速度 */
			TraceBuf1[TraceCounter] = SvifGetMotSpd( TraceInfo.TraceTarget[1].TraceAxis );
			/* トルク指令 */
			TraceBuf2[TraceCounter] = SvifGetTrqRef( TraceInfo.TraceTarget[2].TraceAxis );
			TraceCounter++;
		}
		else
		{
			TraceCmpFlg = TRUE;
		}
	}


	/* ScanA */
	if( (OperatingTimer % SCANA_EXE_TIMING) == 0 )
	{
		for( i=0; i<SIM_AXISNUM; i++ )
		{
			/* Get Encode Information from Motor Model */
			GetEncoderInfo( EncoderData[i], i );
			/* Set Encode Information for JL-086 Register */
			SvifSetEncoderData( EncoderData[i], i );
		}

		/* ScanA Interrupt */
		SysScanAInt( );

		/* Micro Program IntHost */
		MicroCallMpIntHost( );
	}

	/* ScanB */
	if( (OperatingTimer % SCANB_EXE_TIMING) == 0 )
	{
		if( CtrlMode == POS_CTRL_MODE )
		{ /* Position Control Mode */
			/* Set Speed Ref [min-1] */
			dPositionRef = MakeCurrentReference( &DenStatus );
			for( i=0; i<SIM_AXISNUM; i++ )
			{
				SvifSetPulseRef( dPositionRef, i );
			}
		}
		else
		{ /* Speed Control Mode */
			/* Set Speed Ref [min-1] */
			for( i=0; i<SIM_AXISNUM; i++ )
			{
				SvifSetSpeedRefAd( SpeedRef, i );
			}
		}

		/* ScanB Interrupt */
		SysScanBInt();
	}

	/* ScanC */
	if( (OperatingTimer % SCANC_EXE_TIMING) == 0 )
	{
		SysScanCInt();
	}

	/* Round */
	if( (OperatingTimer % ROUND_EXE_TIMING) == 0 )
	{
		SysRoundTask();
	}


	/* 電流ループ */
	if( (OperatingTimer % INTAD_EXE_TIMING) == 0 )
	{
		MicroCallMpIntAD( Vu, Vv, Vw, IuFb, IvFb, SIM_AXISNUM );
	}

	for( i=0; i<SIM_AXISNUM; i++ )
	{
		/* モータモデル */
		CalcMotorFeedback( Vu[i], Vv[i], Vw[i], &(IuFb[i]), &(IvFb[i]), i );
	}

	/* Operating Timer Count Up */
	OperatingTimer++;

	return 1;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVOSIM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MENU1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	hWnd = CreateWindow( szWindowClass,
						 szTitle,
						 WS_OVERLAPPEDWINDOW,
						 WINDOW_WIDTH,
						 WINDOW_HEIGHT,
						 WINDOW_X,
						 WINDOW_Y,
						 NULL,
						 NULL,
						 hInstance,
						 NULL );

	if (!hWnd)
	{
		; //処理なし
	}
	else
	{
		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
	}
	return hWnd;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case ID_OPEN_DIALOG:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		releaseObj();
		PostQuitMessage(0);
		break;

//	case WM_VSCROLL:
//		return  Wm_VScrollProc(hwnd,LOWORD(wparam),HIWORD(wparam),(HWND)lparam);

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
// ダイアログプロシージャ
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int				i;
	/* Double Buffer */
	static HDC		hMemDC;
	static HBITMAP	hBitmap;
	/* Dialog Standard */
	static UINT		uTimerID;
	HDC				hdc;
	PAINTSTRUCT		ps;
	HWND			pWnd;
	/* Alarm Viewer */
	CHAR			AlmBuffer[6] = "A.000";
	USHORT			AlmCode;
	/* TraceSettings */
	USHORT			TrigCondition;
	USHORT			PreTrigger;

	switch( message ){
	case WM_INITDIALOG:  // ダイアログボックスが作成されたとき
		/* Initialize Dialog Item */
		InitializeDialogItems( &hDlg, &hMemDC, &hBitmap );
		/* Control Mode Set */
		for( i=0; i<SIM_AXISNUM; i++ )
		{
			SvifSetControlMode( CtrlMode, i );
		}
		/* Set Initial Position Control Command */
		if( CtrlMode == POS_CTRL_MODE )
		{
			SetPositionReference( PositionRef, SpeedRef, AccTime, DecTime );
		}
		/* Set Initial Trace Settings */
		TrigCondition = (TraceInfo.TriggerMode << 8) | TraceInfo.TriggerItem;
		PreTrigger = (USHORT)(TRACE_DATANUM * TraceInfo.PreTrigger / 100);
		SvifSetTraceControl( (UCHAR)TRACE_NUMBER,
							 (UCHAR)TRACE_DATANUM,
							 (USHORT*)TraceInfo.TraceTarget,
							 (USHORT)TraceInfo.TraceSampling,
							 PreTrigger,
							 TrigCondition );
		/* Initialize Drawing */
		InitDraw();

		/* Set Alarm Code */
		AlmCode = SvifGetAlmCode( 0 );
		if( AlmCode != NO_ALARM )
		{
			SetAlmCode2Buf( AlmCode, AlmBuffer );
			SetDlgItemText( hDlg, IDC_ALMVIEW, AlmBuffer );
		}

		/* Make the Multimedia Timer */
		SetTimer( hDlg, TIMER_ID, TIMER_ELAPSE, NULL );
		uTimerID = timeSetEvent( 1, 0, TimerCallback, 0, TIME_PERIODIC );
		if( !uTimerID )
		{
			MessageBox( NULL, _T("タイマーの初期化に失敗しました"), _T("エラー"), MB_OK );
			return FALSE;
		}

		return TRUE;

	case WM_TIMER:          // タイマメッセージ
		if( wParam != TIMER_ID )
		{
			break;  // 識別IDが一致しない場合
		}

		/* Send and Receive Memobus Message */
//		SvifSendMemobusMsg( MemobusBuf, MessageSize );

		/* Draw back ground image to double buffer */
		DrawTraceBackGround( &hMemDC );

		/* Draw trace wave to double buffer */
		DrawTraceWave( &hMemDC );

		//アラーム表示
		AlmCode = SvifGetAlmCode( 0 );
		if( AlmCode != NO_ALARM )
		{
			SetAlmCode2Buf( AlmCode, AlmBuffer );
			SetDlgItemText( hDlg, IDC_ALMVIEW, AlmBuffer );
		}

		// 無効領域を発生させる
		InvalidateRect( hDlg, NULL, FALSE );
		return 0;

	case WM_COMMAND:     // ダイアログボックス内の何かが選択されたとき
		switch( LOWORD( wParam ) ){
		case IDOK:       // 「OK」ボタンが選択された
		case IDCANCEL:   // 「キャンセル」ボタンが選択された
			// ダイアログボックスを消す
			EndDialog( hDlg, 0 );
			break;

		case IDC_TRACE_SET:
			/* Get Trace Setting Infomation from Dialog Items */
			GetTraceSetting( &hDlg );
			/* Set Trace Settings */
			TrigCondition = (TraceInfo.TriggerMode << 8) | TraceInfo.TriggerItem;
			PreTrigger = (USHORT)(TRACE_DATANUM * TraceInfo.PreTrigger / 100);
			SvifSetTraceControl( (UCHAR)TRACE_NUMBER,
								 (UCHAR)TRACE_DATANUM,
								 (USHORT*)TraceInfo.TraceTarget,
								 (USHORT)TraceInfo.TraceSampling,
								 PreTrigger,
								 TrigCondition );
			break;

		case IDC_SIM_SET:
			/* Get Simulation Setting Infomation from Dialog Items */
			GetSimulationSetting( &hDlg );
			/* Control Mode Set */
			for( i=0; i<SIM_AXISNUM; i++ )
			{
				SvifSetControlMode( CtrlMode, i );
			}
			/* Set Position Reference */
			if( CtrlMode == POS_CTRL_MODE )
			{
				SetPositionReference( PositionRef, SpeedRef, AccTime, DecTime );
			}
			break;

		case IDC_START_BUTTON:
			SimStartFlg = TRUE;
			break;

		case IDC_RELOAD_ALM:
			AlmCode = SvifGetAlmCode( 0 );
			if( AlmCode != NO_ALARM )
			{
				SetAlmCode2Buf( AlmCode, AlmBuffer );
				SetDlgItemText( hDlg, IDC_ALMVIEW, AlmBuffer );
			}

			// 無効領域を発生させる
			InvalidateRect( hDlg, NULL, FALSE );
			break;
		}
		return TRUE;

	case WM_PAINT:
		/* DCコンパチブルの描画 */
		pWnd = GetDlgItem( hDlg, IDC_PICTURE );
		hdc = BeginPaint( pWnd, &ps );
		BitBlt( hdc, 0, 0, SpaceSizeX, SpaceSizeY, hMemDC, 0, 0, SRCCOPY );
		EndPaint( hDlg, &ps );
		break;

	case WM_CLOSE:
		/* Delete Double Buffer */
		DeleteDC( hMemDC );
		DeleteObject( hBitmap );
		/* Kill the Timer */
		KillTimer( hDlg, TIMER_ID );
		timeKillEvent( uTimerID );
		/* Close Dialog */
		EndDialog( hDlg, IDOK );
		return TRUE;

	default:
		break;
	}

	return FALSE;  // DefWindowProc()ではなく、FALSEを返すこと！
}


// Initialize the Dialog Items
void	InitializeDialogItems( HWND *hDlg, HDC *hMemDC, HBITMAP *hBitmap )
{
	int		i;
	HDC		hdc;
	HWND	pWnd, hRadio, hCombo[3];

	/* Radio Button */
	hRadio = GetDlgItem( *hDlg, IDC_RADIO_POWER_ON );		// Main Power Status
	SendMessage( hRadio, BM_SETCHECK, BST_CHECKED, 0 );		// ON
	hRadio = GetDlgItem( *hDlg, IDC_RADIO_SVON );			// Servo On Status
	SendMessage( hRadio, BM_SETCHECK, BST_CHECKED, 0 );		// ON
	hRadio = GetDlgItem( *hDlg, IDC_RADIO_TRACE_SINGLE );	// Single Trace Select
	SendMessage( hRadio, BM_SETCHECK, BST_CHECKED, 0 );		// ON
	TraceInfo.SeriesTraceMode = FALSE;
	hRadio = GetDlgItem( *hDlg, IDC_RADIO_POSCTRL );		// Control Mode Select
	SendMessage( hRadio, BM_SETCHECK, BST_CHECKED, 0 );		// ON
	CtrlMode = CTRLMODE_POS;

	/* Trace Items */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRACE_SEL1 );			// Trace Target 1
	hCombo[1] = GetDlgItem( *hDlg, IDC_TRACE_SEL2 );			// Trace Target 2
	hCombo[2] = GetDlgItem( *hDlg, IDC_TRACE_SEL3 );			// Trace Target 3
	for( i=0; i<TRACE_ITEM_NUMBER; i++ )
	{
		SendMessage( hCombo[0], CB_INSERTSTRING, i, (LPARAM)TraceItemTable[i].TargetName );
		SendMessage( hCombo[1], CB_INSERTSTRING, i, (LPARAM)TraceItemTable[i].TargetName );
		SendMessage( hCombo[2], CB_INSERTSTRING, i, (LPARAM)TraceItemTable[i].TargetName );
	}
	/* ウインドウ生成時にはじめに表示するデータを指定 */
	TraceInfo.TraceTarget[0].TraceData = TraceItemTable[3].Index; /* 位置指令速度 */
	SendMessage( hCombo[0], CB_SETCURSEL, 3, 0);
	TraceInfo.TraceTarget[1].TraceData = TraceItemTable[1].Index; /* フィードバック速度 */
	SendMessage( hCombo[1], CB_SETCURSEL, 1, 0);
	TraceInfo.TraceTarget[2].TraceData = TraceItemTable[0].Index; /* トルク/推力指令 */
	SendMessage( hCombo[2], CB_SETCURSEL, 0, 0);

	/* Trace Axis */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRACE_AXIS1 );			// Trace Axis No 1
	hCombo[1] = GetDlgItem( *hDlg, IDC_TRACE_AXIS2 );			// Trace Axis No 2
	hCombo[2] = GetDlgItem( *hDlg, IDC_TRACE_AXIS3 );			// Trace Axis No 3
	for( i=0; i<SIM_AXISNUM; i++ )
	{
		SendMessage( hCombo[0], CB_INSERTSTRING, i, (LPARAM)TraceAxisTable[i].AxisNumber );
		SendMessage( hCombo[1], CB_INSERTSTRING, i, (LPARAM)TraceAxisTable[i].AxisNumber );
		SendMessage( hCombo[2], CB_INSERTSTRING, i, (LPARAM)TraceAxisTable[i].AxisNumber );
	}
	/* ウインドウ生成時にはじめに表示するデータを指定 */
	TraceInfo.TraceTarget[0].TraceAxis = 0;
	SendMessage( hCombo[0], CB_SETCURSEL, 0, 0);
	TraceInfo.TraceTarget[1].TraceAxis = 0;
	SendMessage( hCombo[1], CB_SETCURSEL, 0, 0);
	TraceInfo.TraceTarget[2].TraceAxis = 0;
	SendMessage( hCombo[2], CB_SETCURSEL, 0, 0);

	/* Sampling Time */
	TraceInfo.TraceSampling = 1;
	SetDlgItemText( *hDlg, IDC_SAMPLING, TEXT("1") );

	/* Trigger Setting */
	/* Trigger Item */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRIG_SEL );			// Trigger Select
	SendMessage( hCombo[0], CB_INSERTSTRING, 0, (LPARAM)TEXT("1") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 1, (LPARAM)TEXT("2") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 2, (LPARAM)TEXT("3") );
	SendMessage( hCombo[0], CB_SETCURSEL, 0, 0);
	TraceInfo.TriggerItem = 0;
	/* Trigger Mode */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRIG_MODE );			// Trigger Select
	SendMessage( hCombo[0], CB_INSERTSTRING, 0, (LPARAM)TEXT("トリガなし") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 1, (LPARAM)TEXT("立上りエッジ") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 2, (LPARAM)TEXT("立下りエッジ") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 3, (LPARAM)TEXT("変化エッジ") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 4, (LPARAM)TEXT("Level Up") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 5, (LPARAM)TEXT("Level Down") );
	SendMessage( hCombo[0], CB_SETCURSEL, 0, 0);
	TraceInfo.TriggerMode = 0;
	/* Trigger Level */
	TraceInfo.TriggerLevel = 0;
	SetDlgItemText( *hDlg, IDC_TRIG_LEVEL, TEXT("0") );
	TraceInfo.PreTrigger = 0;
	SetDlgItemText( *hDlg, IDC_PRE_TRIG, TEXT("0") );
	/* Trace Gains */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRACE_GAIN1 );		// Trace Gain 1
	hCombo[1] = GetDlgItem( *hDlg, IDC_TRACE_GAIN2 );		// Trace Gain 2
	hCombo[2] = GetDlgItem( *hDlg, IDC_TRACE_GAIN3 );		// Trace Gain 3
	for( i=0; i<TRACE_GAIN_NUMBER; i++ )
	{
		SendMessage( hCombo[0], CB_INSERTSTRING, i, (LPARAM)TraceGainTable[i].GainView );
		SendMessage( hCombo[1], CB_INSERTSTRING, i, (LPARAM)TraceGainTable[i].GainView );
		SendMessage( hCombo[2], CB_INSERTSTRING, i, (LPARAM)TraceGainTable[i].GainView );
	}
	/* ウインドウ生成時にはじめに表示するデータを指定 */
	TraceGain[0] = TraceGainTable[4].GainValue; /* 1.0 */
	SendMessage( hCombo[0], CB_SETCURSEL, 4, 0);
	TraceGain[1] = TraceGainTable[4].GainValue; /* 1.0 */
	SendMessage( hCombo[1], CB_SETCURSEL, 4, 0);
	TraceGain[2] = TraceGainTable[4].GainValue; /* 1.0 */
	SendMessage( hCombo[2], CB_SETCURSEL, 4, 0);

	/* Trace Offset */
	TraceOffset[0] = 0.0;
	SetDlgItemText( *hDlg, IDC_TRACE_OFFSET1, TEXT("0.0") );
	TraceOffset[0] = 0.0;
	SetDlgItemText( *hDlg, IDC_TRACE_OFFSET2, TEXT("0.0") );
	TraceOffset[0] = 0.0;
	SetDlgItemText( *hDlg, IDC_TRACE_OFFSET3, TEXT("0.0") );

	/* Reference Setting */
	/* Position Control Mode */
	PositionRef = 1048576;
	SetDlgItemText( *hDlg, IDC_TPOS, TEXT("1048576") );
	SpeedRef = 500;
	SetDlgItemText( *hDlg, IDC_TSPD_POS, TEXT("500") );
	PositionFb = 10;
	SetDlgItemText( *hDlg, IDC_APOS_VIEW, TEXT("10") );
	SpeedFb = 10;
	SetDlgItemText( *hDlg, IDC_CSPD_VIEW_POS, TEXT("10") );
	/* Velocity Control Mode */
	SpeedRef = 500;
	SetDlgItemText( *hDlg, IDC_TSPD_VEL, TEXT("500") );
	SpeedFb = 0;
	SetDlgItemText( *hDlg, IDC_CSPD_VIEW_VEL, TEXT("0") );
	/* Common Parameters */
	AccTime = 10;
	SetDlgItemText( *hDlg, IDC_ACC_TIME, TEXT("10") );
	DecTime = 10;
	SetDlgItemText( *hDlg, IDC_DEC_TIME, TEXT("10") );

	/* Initialize the Trace Viewer */
	pWnd = GetDlgItem( *hDlg, IDC_PICTURE );
	hdc = GetDC( pWnd );
	*hMemDC = CreateCompatibleDC( hdc );
	*hBitmap = CreateCompatibleBitmap( hdc, SpaceSizeX, SpaceSizeY );
	SelectObject( *hMemDC, *hBitmap );
	SelectObject( *hMemDC, GetStockObject(DC_PEN) );
	SelectObject( *hMemDC, GetStockObject(DC_BRUSH) );
	ReleaseDC( *hDlg, hdc );
}



// Get Trace Setting Infomation from Dialog Items
void	GetTraceSetting( HWND *hDlg )
{
	HWND	hCombo[3];
	TCHAR	GetText[31];
	int		GetStatus;
	int		i;

	/* Trace Items */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRACE_SEL1 );
	hCombo[1] = GetDlgItem( *hDlg, IDC_TRACE_SEL2 );
	hCombo[2] = GetDlgItem( *hDlg, IDC_TRACE_SEL3 );
	for( i=0; i<3; i++ )
	{
		GetStatus = SendMessage( hCombo[i], CB_GETCURSEL, 0, 0);
		TraceInfo.TraceTarget[i].TraceData = TraceItemTable[GetStatus].Index;
	}

	/* Trace Axis */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRACE_AXIS1 );
	hCombo[1] = GetDlgItem( *hDlg, IDC_TRACE_AXIS2 );
	hCombo[2] = GetDlgItem( *hDlg, IDC_TRACE_AXIS3 );
	for( i=0; i<3; i++ )
	{
		GetStatus = SendMessage( hCombo[i], CB_GETCURSEL, 0, 0);
		TraceInfo.TraceTarget[i].TraceAxis = TraceAxisTable[GetStatus].Index;
	}

	/* Sampling Time */
	GetDlgItemText( *hDlg, IDC_SAMPLING, (LPTSTR)GetText, (int)sizeof(GetText));
	TraceInfo.TraceSampling = _ttoi( GetText );

	/* Trigger Setting */
	/* Trigger Item */
	hCombo[0] = GetDlgItem( *hDlg, IDC_TRIG_SEL );
	GetStatus = SendMessage( hCombo[0], CB_GETCURSEL, 0, 0);
	TraceInfo.TriggerItem = GetStatus;
	/* Trigger Mode */
	hCombo[1] = GetDlgItem( *hDlg, IDC_TRIG_MODE );
	GetStatus = SendMessage( hCombo[1], CB_GETCURSEL, 0, 0);
	TraceInfo.TriggerMode = GetStatus;
	/* Trigger Level */
	GetDlgItemText( *hDlg, IDC_TRIG_LEVEL, (LPTSTR)GetText, (int)sizeof(GetText));
	TraceInfo.TriggerLevel = _ttoi( GetText );
	/* Pre Trigger */
	GetDlgItemText( *hDlg, IDC_PRE_TRIG, (LPTSTR)GetText, (int)sizeof(GetText));
	TraceInfo.PreTrigger = _ttoi( GetText );

	/* Series Trace Select */
	TraceInfo.SeriesTraceMode =
		(UCHAR)SendMessage( GetDlgItem( *hDlg, IDC_RADIO_TRACE_SERIES), BM_GETCHECK, 0, 0 );

}



// Get Simulation Setting Infomation from Dialog Items
void	GetSimulationSetting( HWND *hDlg )
{
	TCHAR	GetText[31];
	int		GetStatus;

	/* Simulation Control Mode */
	GetStatus = (UCHAR)SendMessage( GetDlgItem( *hDlg, IDC_RADIO_POSCTRL), BM_GETCHECK, 0, 0 );
	if( GetStatus == TRUE )
	{
		/* Position Control Mode */
		CtrlMode = CTRLMODE_POS;
		GetDlgItemText( *hDlg, IDC_TPOS, (LPTSTR)GetText, (int)sizeof(GetText));
		PositionRef = _ttoi( GetText );
		GetDlgItemText( *hDlg, IDC_TSPD_POS, (LPTSTR)GetText, (int)sizeof(GetText));
		SpeedRef = _ttoi( GetText );
		GetDlgItemText( *hDlg, IDC_APOS_VIEW, (LPTSTR)GetText, (int)sizeof(GetText));
		PositionFb = _ttoi( GetText );
		GetDlgItemText( *hDlg, IDC_CSPD_VIEW_POS, (LPTSTR)GetText, (int)sizeof(GetText));
		SpeedFb = _ttoi( GetText );
	}
	else
	{
		/* Velocity Control Mode */
		CtrlMode = CTRLMODE_SPD;
		GetDlgItemText( *hDlg, IDC_TSPD_VEL, (LPTSTR)GetText, (int)sizeof(GetText));
		SpeedRef = _ttoi( GetText );
		GetDlgItemText( *hDlg, IDC_CSPD_VIEW_VEL, (LPTSTR)GetText, (int)sizeof(GetText));
		SpeedFb = _ttoi( GetText );
	}

	/* Common Parameters */
	GetDlgItemText( *hDlg, IDC_ACC_TIME, (LPTSTR)GetText, (int)sizeof(GetText));
	AccTime = _ttoi( GetText );
	GetDlgItemText( *hDlg, IDC_DEC_TIME, (LPTSTR)GetText, (int)sizeof(GetText));
	DecTime = _ttoi( GetText );
}



//Drawing Trace Wave Image
void	DrawTraceWave( HDC *hMemDC )
{
	int		i;

	/* Set wave color */
	hdefp = (HPEN)SelectObject( *hMemDC, GreenPen );
	/* Set begin position */
	MoveToEx( *hMemDC, ZeroPosX, (int)(((float)-TraceBuf0[0]*DrawGainS)+ZeroPosY), NULL );
	for( i=1; i<TraceCounter; i++ )
	{
		LineTo( *hMemDC, i+ZeroPosX, (int)(((float)-TraceBuf0[i]*DrawGainS)+ZeroPosY) );
	}

	/* Set wave color */
	hdefp = (HPEN)SelectObject( *hMemDC, RedPen );
	/* Set begin position */
	MoveToEx( *hMemDC, ZeroPosX, (int)(((float)-TraceBuf1[0]*DrawGainS)+ZeroPosY), NULL );
	for( i=1; i<TraceCounter; i++ )
	{
		LineTo( *hMemDC, i+ZeroPosX, (int)(((float)-TraceBuf1[i]*DrawGainS)+ZeroPosY) );
	}

	/* Set wave color */
	hdefp = (HPEN)SelectObject( *hMemDC, YellowPen );
	/* Set begin position */
	MoveToEx( *hMemDC, ZeroPosX, (int)(((float)-TraceBuf2[0]*DrawGainS)+ZeroPosY), NULL );
	for( i=1; i<TraceCounter; i++ )
	{
		LineTo( *hMemDC, i+ZeroPosX, (int)(((float)-TraceBuf2[i]*DrawGainS)+ZeroPosY) );
	}

}


void	DrawTraceBackGround( HDC *hMemDC )
{
	int		i;
	int		CurrentX, CurrentY;

	//一応デフォルトのブラシを保存(キャストを忘れずに)
	hdefp = (HPEN)SelectObject( *hMemDC, WhitePen );
	hdefb = (HBRUSH)SelectObject( *hMemDC, hbrackb );

	/* 背景描画 */
	Rectangle( *hMemDC, 0, 0, SpaceSizeX, SpaceSizeY );

	MoveToEx( *hMemDC, OriginX, OriginY, NULL );	// 始点を設定する
	CurrentX = OriginX;
	CurrentY = OriginY;

	// X軸描画
	LineTo( *hMemDC, EndX, CurrentY );				// 直線を描く

	// Y座標 補助目盛り線描画
	for( i=0; i<10; i++ )
	{
		// 描画準備
		MoveToEx( *hMemDC, CurrentX, CurrentY, NULL );			// 始点を設定する

		// 1目盛り分のY軸描画
		hdefp = (HPEN)SelectObject( *hMemDC, WhitePen );
		LineTo( *hMemDC, CurrentX, CurrentY - OneStepY );		// 直線を描く
		CurrentY -= OneStepY;

		// 目盛り線描画
		LineTo( *hMemDC, CurrentX - 2, CurrentY );				// 直線を描く
		LineTo( *hMemDC, CurrentX + 2, CurrentY );				// 直線を描く

		// Y座標目盛り点線描画
		hdefp = (HPEN)SelectObject( *hMemDC, WhiteDotPen );
		LineTo( *hMemDC, EndX - 2, CurrentY );					// 点線を描く
	}

	// X座標 補助目盛り線描画
	hdefp = (HPEN)SelectObject( *hMemDC, WhiteDotPen );
	CurrentX = OriginX;
	CurrentY = OriginY;
	for( i=0; i<10; i++ )
	{
		// 描画準備
		CurrentX += OneStepX;
		MoveToEx( *hMemDC, CurrentX, CurrentY, NULL );			// 始点を設定する

		// X座標目盛り点線描画
		LineTo( *hMemDC, CurrentX, EndY );						// 点線を描く
	}
}


void	SetAlmCode2Buf( USHORT AlmCode, CHAR *Buffer )
{
	int		i;
	CHAR	RepeatNum;
	CHAR	Offset;
	CHAR	TmpBuffer[4];

	if( AlmCode != NO_ALARM )
	{
		if( AlmCode < 0x100 )
		{ /* アラームの桁が少ない場合 (ex. A.030) */
			RepeatNum = 2;
			Offset = 3;
		}
		else
		{
			RepeatNum = 3;
			Offset = 2;
		}

		_itoa_s( AlmCode, TmpBuffer, 16 );
		for( i=0; i<RepeatNum; i++ )
		{
			Buffer[i+Offset] = TmpBuffer[i];
		}
	}
	return;
}



//初期化関数
int InitDraw( void )
{
	points[0].x = 110; points[0].y = 20;
	points[1].x = 130; points[1].y = 70;
	points[2].x =  80; points[2].y = 40;
	points[3].x = 140; points[3].y = 40;
	points[4].x =  90; points[4].y = 70;

	hbrackb		= CreateSolidBrush( RGB(   0,   0,   0 ) );
	hredb		= CreateSolidBrush( RGB( 255,   0,   0 ) );
	hblueb		= CreateSolidBrush( RGB(   0,   0, 255 ) );
	hyellowb	= CreateSolidBrush( RGB( 255, 255,   0 ) );
	RedPen		= CreatePen( PS_SOLID, 1, RGB( 255, 0, 0 ) );
	GreenPen	= CreatePen( PS_SOLID, 1, RGB( 0, 255, 255 ) );
	YellowPen	= CreatePen( PS_SOLID, 1, RGB( 255, 255, 0 ) );
	WhitePen	= CreatePen( PS_SOLID, 1, RGB( 255, 255, 255 ) );
	WhiteDotPen	= CreatePen( PS_DOT, 1, RGB( 0, 0, 0 ) );

	return TRUE;
}


//後始末関数
int releaseObj( void )
{
	DeleteObject( hbrackb );
	DeleteObject( hredb );
	DeleteObject( hblueb );
	DeleteObject( hyellowb );
	DeleteObject( RedPen );
	DeleteObject( GreenPen );
	DeleteObject( YellowPen );
	DeleteObject( WhitePen );
	DeleteObject( WhiteDotPen );

	return TRUE;
}



/**************************** end of file **********************************/
