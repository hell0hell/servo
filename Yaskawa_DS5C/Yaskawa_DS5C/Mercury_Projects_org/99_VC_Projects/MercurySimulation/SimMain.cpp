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

#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>

// C �����^�C�� �w�b�_�[ �t�@�C��
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

/* Window�֘A */
#define MAX_LOADSTRING 100
#define WINDOW_WIDTH  (400)		// �E�B���h�E�̕�
#define WINDOW_HEIGHT (300)		// �E�B���h�E�̍���
#define WINDOW_X ((GetSystemMetrics( SM_CXSCREEN ) - WINDOW_WIDTH ) / 2)
#define WINDOW_Y ((GetSystemMetrics( SM_CYSCREEN ) - WINDOW_HEIGHT ) / 2)

/* Timer�֘A */
#define TIMER_ID	 (100)		// �쐬����^�C�}�̎���ID
#define TIMER_ELAPSE (50)		// WM_TIMER�̔����Ԋu

/* Drawing�֘A */
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


// �O���[�o���ϐ�:
HINSTANCE hInst;								/* ���݂̃C���^�[�t�F�C�X */
TCHAR szTitle[MAX_LOADSTRING];					/* �^�C�g�� �o�[�̃e�L�X�g */
TCHAR szWindowClass[MAX_LOADSTRING];			/* ���C�� �E�B���h�E �N���X�� */

// Drawing�֘A
POINT       points[5];
HBRUSH      hdefb, hbrackb, hredb, hblueb, hyellowb;
HPEN        hdefp, GreenPen, RedPen, YellowPen;
HPEN        WhitePen, WhiteDotPen;

// �T�[�{����p�ϐ�
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
ULONG	OperatingTimer;			/* �d�����[�v�����J�E���^ */
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

/* Memobus Message�p�ϐ� */
UCHAR	MemobusBuf[50];
UCHAR	MessageSize;
UCHAR	MemobusTxReq;

/* Trace�p�ϐ� */
TRACE_INFO	TraceInfo;
double		TraceGain[3];
double		TraceOffset[3];

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
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


// �^�C�}�֐�
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

// �֐��Ăяo��
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

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	int i;
	HWND hWnd;
	MSG msg;
	HACCEL hAccelTable;
	UCHAR ret;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MENU1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	hWnd = InitInstance(hInstance, nCmdShow);
	if( hWnd == NULL )
	{
		MessageBox( NULL, _T("�E�B���h�E�̍쐬�Ɏ��s���܂���"), _T("�G���["), MB_OK );
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MENU1));

	// �T�[�{�������������s
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


	// ���C�� ���b�Z�[�W ���[�v:
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

	/* �T�[�{���䏈���V�~�����[�V���� */
	/* for debug */
	if( (OperatingTimer % (SCANA_EXE_TIMING*10)) == 0 )
//	if( (OperatingTimer % 4) == 0 )
	{ /* �T���v�����O�^�C���ݒ� */
		if( TraceCounter < TRACE_BUF_SIZE )
		{
			/* q���w�� */
//			TraceBuf0[TraceCounter] = GetqAxisReference( );
			/* q���t�B�[�h�o�b�N */
//			TraceBuf1[TraceCounter] = GetqAxisFeedback( );
			/* q���d���w�� */
//			TraceBuf2[TraceCounter] = GetqAxisVReference( );

			/* ��ᐧ��o�� */
//			TraceBuf0[TraceCounter] = SvifGetSpdPacOut( 0 );
			/* �ϕ�����o�� */
//			TraceBuf1[TraceCounter] = SvifGetSpdIacOut( 0 );

			/* �ʒu�w�ߑ��x */
			TraceBuf0[TraceCounter] = (short)((double)15000 * dPositionRef / 14418);
			/* ���x�w�� */
//			TraceBuf0[TraceCounter] = SvifGetMotSpdRef( 0 );
			/* ���[�^���x */
			TraceBuf1[TraceCounter] = SvifGetMotSpd( TraceInfo.TraceTarget[1].TraceAxis );
			/* �g���N�w�� */
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


	/* �d�����[�v */
	if( (OperatingTimer % INTAD_EXE_TIMING) == 0 )
	{
		MicroCallMpIntAD( Vu, Vv, Vw, IuFb, IvFb, SIM_AXISNUM );
	}

	for( i=0; i<SIM_AXISNUM; i++ )
	{
		/* ���[�^���f�� */
		CalcMotorFeedback( Vu[i], Vv[i], Vw[i], &(IuFb[i]), &(IvFb[i]), i );
	}

	/* Operating Timer Count Up */
	OperatingTimer++;

	return 1;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
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
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

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
		; //�����Ȃ�
	}
	else
	{
		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
	}
	return hWnd;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
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
		// �I�����ꂽ���j���[�̉��:
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
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
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

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
// �_�C�A���O�v���V�[�W��
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
	case WM_INITDIALOG:  // �_�C�A���O�{�b�N�X���쐬���ꂽ�Ƃ�
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
			MessageBox( NULL, _T("�^�C�}�[�̏������Ɏ��s���܂���"), _T("�G���["), MB_OK );
			return FALSE;
		}

		return TRUE;

	case WM_TIMER:          // �^�C�}���b�Z�[�W
		if( wParam != TIMER_ID )
		{
			break;  // ����ID����v���Ȃ��ꍇ
		}

		/* Send and Receive Memobus Message */
//		SvifSendMemobusMsg( MemobusBuf, MessageSize );

		/* Draw back ground image to double buffer */
		DrawTraceBackGround( &hMemDC );

		/* Draw trace wave to double buffer */
		DrawTraceWave( &hMemDC );

		//�A���[���\��
		AlmCode = SvifGetAlmCode( 0 );
		if( AlmCode != NO_ALARM )
		{
			SetAlmCode2Buf( AlmCode, AlmBuffer );
			SetDlgItemText( hDlg, IDC_ALMVIEW, AlmBuffer );
		}

		// �����̈�𔭐�������
		InvalidateRect( hDlg, NULL, FALSE );
		return 0;

	case WM_COMMAND:     // �_�C�A���O�{�b�N�X���̉������I�����ꂽ�Ƃ�
		switch( LOWORD( wParam ) ){
		case IDOK:       // �uOK�v�{�^�����I�����ꂽ
		case IDCANCEL:   // �u�L�����Z���v�{�^�����I�����ꂽ
			// �_�C�A���O�{�b�N�X������
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

			// �����̈�𔭐�������
			InvalidateRect( hDlg, NULL, FALSE );
			break;
		}
		return TRUE;

	case WM_PAINT:
		/* DC�R���p�`�u���̕`�� */
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

	return FALSE;  // DefWindowProc()�ł͂Ȃ��AFALSE��Ԃ����ƁI
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
	/* �E�C���h�E�������ɂ͂��߂ɕ\������f�[�^���w�� */
	TraceInfo.TraceTarget[0].TraceData = TraceItemTable[3].Index; /* �ʒu�w�ߑ��x */
	SendMessage( hCombo[0], CB_SETCURSEL, 3, 0);
	TraceInfo.TraceTarget[1].TraceData = TraceItemTable[1].Index; /* �t�B�[�h�o�b�N���x */
	SendMessage( hCombo[1], CB_SETCURSEL, 1, 0);
	TraceInfo.TraceTarget[2].TraceData = TraceItemTable[0].Index; /* �g���N/���͎w�� */
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
	/* �E�C���h�E�������ɂ͂��߂ɕ\������f�[�^���w�� */
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
	SendMessage( hCombo[0], CB_INSERTSTRING, 0, (LPARAM)TEXT("�g���K�Ȃ�") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 1, (LPARAM)TEXT("�����G�b�W") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 2, (LPARAM)TEXT("������G�b�W") );
	SendMessage( hCombo[0], CB_INSERTSTRING, 3, (LPARAM)TEXT("�ω��G�b�W") );
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
	/* �E�C���h�E�������ɂ͂��߂ɕ\������f�[�^���w�� */
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

	//�ꉞ�f�t�H���g�̃u���V��ۑ�(�L���X�g��Y�ꂸ��)
	hdefp = (HPEN)SelectObject( *hMemDC, WhitePen );
	hdefb = (HBRUSH)SelectObject( *hMemDC, hbrackb );

	/* �w�i�`�� */
	Rectangle( *hMemDC, 0, 0, SpaceSizeX, SpaceSizeY );

	MoveToEx( *hMemDC, OriginX, OriginY, NULL );	// �n�_��ݒ肷��
	CurrentX = OriginX;
	CurrentY = OriginY;

	// X���`��
	LineTo( *hMemDC, EndX, CurrentY );				// ������`��

	// Y���W �⏕�ڐ�����`��
	for( i=0; i<10; i++ )
	{
		// �`�揀��
		MoveToEx( *hMemDC, CurrentX, CurrentY, NULL );			// �n�_��ݒ肷��

		// 1�ڐ��蕪��Y���`��
		hdefp = (HPEN)SelectObject( *hMemDC, WhitePen );
		LineTo( *hMemDC, CurrentX, CurrentY - OneStepY );		// ������`��
		CurrentY -= OneStepY;

		// �ڐ�����`��
		LineTo( *hMemDC, CurrentX - 2, CurrentY );				// ������`��
		LineTo( *hMemDC, CurrentX + 2, CurrentY );				// ������`��

		// Y���W�ڐ���_���`��
		hdefp = (HPEN)SelectObject( *hMemDC, WhiteDotPen );
		LineTo( *hMemDC, EndX - 2, CurrentY );					// �_����`��
	}

	// X���W �⏕�ڐ�����`��
	hdefp = (HPEN)SelectObject( *hMemDC, WhiteDotPen );
	CurrentX = OriginX;
	CurrentY = OriginY;
	for( i=0; i<10; i++ )
	{
		// �`�揀��
		CurrentX += OneStepX;
		MoveToEx( *hMemDC, CurrentX, CurrentY, NULL );			// �n�_��ݒ肷��

		// X���W�ڐ���_���`��
		LineTo( *hMemDC, CurrentX, EndY );						// �_����`��
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
		{ /* �A���[���̌������Ȃ��ꍇ (ex. A.030) */
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



//�������֐�
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


//��n���֐�
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
