/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MpgTstMain.c : Micro-Program Test Main														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		機 能 : マイクロプログラムをＶＣ＋＋環境で実行し、マイクロプログラムのデバッグを行う。		*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright Yaskawa Electric Corporation **********************************************/
/*																									*/
/*		Rev.1.00 : 2007.03.03	T.Taniguchi															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifdef WIN32			/* VCのみ有効(ASIPコンパイラ対応)			*/
#include <windows.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <direct.h>
#include <sys\stat.h>
#else	//#ifdef WIN32			/* VCのみ有効(ASIPコンパイラ対応)			*/
//#include "Basedef.h"
#endif	//#ifdef WIN32			/* VCのみ有効(ASIPコンパイラ対応)			*/
/*--------------------------------------------------------------------------------------------------*/

#define	JL086_MICRO_OFS		0x7000

/****************************************************************************************************/
#ifdef WIN32			/* VCのみ有効(ASIPコンパイラ対応)			*/
#pragma	warning( disable : 4068 )				/* Disable VC++ Warning : Unknown #pragma			*/
#pragma	warning( disable : 4102 )				/* Disable VC++ Warning : Not Used Label			*/
#pragma	warning( disable : 4305 )				/* Disable VC++ Warning : const int --> short		*/
#pragma	warning( disable : 4996 )				/* Disable VC++ Warning : check 'fopen' declaration	*///110811tanaka21追加
#endif	//#ifdef WIN32			/* VCのみ有効(ASIPコンパイラ対応)			*/
/****************************************************************************************************/
/*		Return Code Definition																		*/
/****************************************************************************************************/
#define TSTOK	 0								/* Test OK											*/
#define TSTNG	-1								/* Test NG											*/
/****************************************************************************************************/
/*		Include MicroProgram Source File															*/
/****************************************************************************************************/
#define RomVlmtTableXX	RomVlmtTable02			/* Select RomVlmtTable01/02							*/
/*--------------------------------------------------------------------------------------------------*/
#include "IxInst.h"						/* Include IxInst Function							*/
/****************************************************************************************************/
/*		Include Test MicroProgram File																*/
/****************************************************************************************************/
#include "RefTbl.h"
#include "MprgStruct.h"
/*--------------------------------------------------------------------------------------------------*/
extern	void MpStart( void );
extern	void MpIntAD( void );
extern	void MpIntHost( void );


/****************************************************************************************************/
/*																									*/
/*		Test Functions																				*/
/*																									*/
/****************************************************************************************************/
LONG	GetSvipTopAddress( void );
void	MicroCallMpStart( unsigned char axno );
void	MicroCallMpIntAD( double *Vu, double *Vv, double *Vw, double *IuFb, double *IvFb, unsigned char axno );
void	MicroCallMpIntHost( void );



/****************************************************************************************************/
/*																									*/
/*		Get Svip Top Address Function																*/
/*																									*/
/****************************************************************************************************/
LONG	GetSvipTopAddress( void )
{
	return	(LONG)((UCHAR*)&(AxisHdl[0].AcrV.IdIntgl) - JL086_MICRO_OFS);
}



/****************************************************************************************************/
/*																									*/
/*		Call MpStart() Function																		*/
/*																									*/
/****************************************************************************************************/
void	MicroCallMpStart( unsigned char axno )
{
	/* Set the Axis Number */
	AxisInfo.AxisNum = axno;

	/* Initialize Micro Programs */
	MpStart( );
}


/****************************************************************************************************/
/*																									*/
/*		Call MpIntAD() Function																		*/
/*																									*/
/****************************************************************************************************/
void	MicroCallMpIntAD( double *Vu, double *Vv, double *Vw, double *IuFb, double *IvFb, unsigned char axno )
{
	int	i;

	/* Set the Current Feedback from Motor Model */
	for( i=0; i<axno; i++ )
	{
		/* Set Current AD Data (4倍はμプログラムでの2bit右シフトに合わせたもの) */
		AxisHdl[i].SvIpRegR->IuAD = (USHORT)(IuFb[i] * 4.0);
		AxisHdl[i].SvIpRegR->IvAD = (USHORT)(IvFb[i] * 4.0);
	}

	/* Run the INTAD */
	MpIntAD( );

	/* Output the Voltage Reference */
	for( i=0; i<axno; i++ )
	{
		Vu[i] = AxisHdl[i].VcmpV.VuOut;
		Vv[i] = AxisHdl[i].VcmpV.VvOut;
		Vw[i] = AxisHdl[i].VcmpV.VwOut;
	}
}



/****************************************************************************************************/
/*																									*/
/*		Call MpIntHost() Function																	*/
/*																									*/
/****************************************************************************************************/
void	MicroCallMpIntHost( void )
{
	MpIntHost( );
}



#if 0
/****************************************************************************************************/
/*																									*/
/*		Set Initial Pramater for IntAD																*/
/*																									*/
/****************************************************************************************************/
void	SetInitPrm( void )
{
	IntAdV.IuOffset = 0xFEFF;
	IntAdV.IvOffset = 0xFAFF;
	IntAdP.Kcu = 0X0001;
	IntAdP.Kcv = 0X0001;
	VcmpV.VdRef = 0x0000;
	VcmpV.VqRef = 0x0000;
	IntAdV.TLimP = 0x983A;
	IntAdV.TLimM = 0x983A;
	IntAdV.KEangle = 0x0300;
	IntAdP.Kf31 = 0X0000;
	IntAdP.Kc = 0X7903;
	IntAdP.Ld = 0xBA43;
	IntAdP.Lq = 0xBA43;
	IntAdP.Mag = 0x7048;
	IntAdP.KdP = 0XED07;
	IntAdP.KdI = 0X4D03;
	IntAdP.KqI = 0X4D03;
	IntAdP.VdLim = 0xBE28;
	IntAdP.VqLim = 0xBE28;
	IntAdP.OnDelayLvl = 0x983A;
	IntAdP.Tfil = 0x0040;
	IntAdP.OnDelayComp = 0x7800;
	IntAdP.CtrlSw = 0XF28D;
	IntAdP.CrFreq = 0XA60E;
	IntAdP.Kf11 = 0X400F;
	IntAdP.TLpf2 = 0x0040;
	IntAdP.MotResist = 0x2A10;
	IntAdP.OnDelaySlope = 0x9D36;
	IntAdP.FccRst = 0X0000;
	IntAdP.L_dIdt = 0X0000;
	DobsP.TsPerL = 0XB505;
	DobsP.Gobs = 0x0728;
	DobsP.RLTs = 0xE90F;
	DobsP.FilObsGain = 0xBD20;
	PhaseV.PhaseIpF = 0x0000;
	WeakFV.WfKiV.s[0] = 0x983A;
	WeakFV.WfKpV.l= 0x0F0008F0;
//	WFKPVH = 0x0F00;
	WeakFV.WfKiV.l = 0x000006F9;
//	WFKIVH = 0x0000;
	WeakFV.WfV1Max = 0xCC24;
	WeakFV.WfIdRefLim = 0x0000;

}


void	SetTrqRef( int i )
{
	PhaseV.PhaseH = TrqRefIn[i].PHA;
	PhaseV.PhaseIp = TrqRefIn[i].PHAIP;
	WeakFV.IdOut = TrqRefIn[i].IDO;
	IntAdV.IqOut1L.s[0]= TrqRefIn[i].IQO;
	WeakFV.Vel = TrqRefIn[i].VEL;
	IntAdP.Kvv = TrqRefIn[i].KVV;
}

typedef struct{
	SHORT	IDO;			/* q軸電流指令(IntAdV.IqOut1L.s[0])				*/
	LONG	IDIL;			/* 弱め界磁用速度(WeakFV.Vel)			*/
	LONG	VDFL;			/* ACRd Filter Output(IAcrV.VdFil.l)			*/
	LONG	VQFL;			/* ACRq Filter Output(AcrV.VqFil.l)			*/
	SHORT	WFVDREF;			/* d軸電圧指令(WeakFV.WfVdRef)			*/
	SHORT	WFVQREF;			/* q軸電圧指令(WeakFV.WfVqRef)			*/
}ADOUT;

ADOUT ADOutTbl[1000];
void	GetADOut( int i )
{
	ADOutTbl[i].IDO = WeakFV.IdOut;
	ADOutTbl[i].IDIL = AcrV.IdIntgl.l;
	ADOutTbl[i].VDFL = AcrV.VdFil.l;
	ADOutTbl[i].VQFL = AcrV.VqFil.l;
	ADOutTbl[i].WFVDREF = WeakFV.WfVdRef;
	ADOutTbl[i].WFVQREF = WeakFV.WfVqRef;
}


void	MakeADOutFile( int i )
{
	FILE *fp;
	char *filename = "ADOut.csv";
	int count = 0;
	/* ファイルのオープン */
	/*  ここで、ファイルポインタを取得する */
	if ((fp = fopen(filename, "a")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* エラーの場合は通常、異常終了する */
	}
	
	/* ファイルの読み（書き）*/
	if(i==0)
	{
		count = fprintf(fp, "time,WeakFV.IdOut,AcrV.IdIntgl.l,AcrV.VdFil.l,AcrV.VqFil.l,WeakFV.WfVdRef,WeakFV.WfVqRef\n");
	}
	count |= fprintf(fp, "%f,%d,%d,%d,%d,%d,%d\n",
			i*0.125,ADOutTbl[i].IDO, ADOutTbl[i].IDIL, ADOutTbl[i].VDFL, ADOutTbl[i].VQFL, ADOutTbl[i].WFVDREF, ADOutTbl[i].WFVQREF);

	if(count <= 0)
	{
		fprintf(stderr,"write error\n");
		fclose(fp);
		return;
	}
	fclose(fp);	/* ファイルのクローズ */
	
	return ;

}
#endif

/**************************************** end of file ***********************************************/
