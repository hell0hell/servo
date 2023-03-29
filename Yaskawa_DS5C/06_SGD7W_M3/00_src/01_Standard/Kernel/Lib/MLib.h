#ifndef		 MLIB_H_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		mlib_c_float.h : float版MLIB関数定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		初版   : 2000.01.14  T.Taniguchi	Mlib85E.c												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"

/****************************************************************************************************/
/*																									*/
/*		エンディアン変換マクロ																		*/
/*																									*/
/****************************************************************************************************/
#if		defined( __CA850__ )
/*--------------------------------------------------------------------------------------------------*/
#define MlibByteSwap16( x, y )		{(y) = (USHORT)__bsh( (x) );}
/*--------------------------------------------------------------------------------------------------*/
	#if		defined( CCSW_LITTLE_ENDIAN ) 
	#define MlibCnvLittle16( x, y )	{ ; }
	#define MlibCnvBig16( x, y )	{ MlibByteSwap16( (x), (y) );}
	#endif
	/*----------------------------------------------------------------------------------------------*/
	#if		defined( CCSW_BIG_ENDIAN ) 
	#define MlibCnvBig16( x, y )		( x )
	#define MlibCnvLittle16( x, y )		{ MlibByteSwap16( (x), (y) );}
	#endif
/*--------------------------------------------------------------------------------------------------*/
#elif	defined( __RENESAS__ )
#define MlibByteSwap16( x, y )		{(y) = (USHORT)swapb( (x) );}
/*--------------------------------------------------------------------------------------------------*/
	#if		defined( CCSW_LITTLE_ENDIAN ) 
	#define MlibCnvLittle16( x, y )	{ ; }
	#define MlibCnvBig16( x, y )	{ MlibByteSwap16( (x), (y) );}
	#endif
	/*----------------------------------------------------------------------------------------------*/
	#if		defined( CCSW_BIG_ENDIAN ) 
	#define MlibCnvBig16( x, y )		{ ; }
	#define MlibCnvLittle16( x, y )		{ MlibByteSwap16( (x), (y) );}
	#endif
/*--------------------------------------------------------------------------------------------------*/
#elif	defined( __CC_ARM )
//#define MlibByteSwap16( x, y )   	__asm {  REV16  y, x  }
#define MlibByteSwap16( x, y )   	{ y = __rev( x ); }
/*--------------------------------------------------------------------------------------------------*/
	#if		(! defined( __BIG_ENDIAN ))
	#define MlibCnvLittle16( x, y )	{ ; }
	#define MlibCnvBig16( x, y )	{ MlibByteSwap16( (x), (y) );}
	#define MlibCnvBig32( x )	__rev( x )
	/*----------------------------------------------------------------------------------------------*/
	#else
	#define MlibCnvBig16( x, y )		( x )
	#define MlibCnvLittle16( x, y )		{ MlibByteSwap16( (x), (y) );}
	#endif
/*--------------------------------------------------------------------------------------------------*/
#elif	defined( _VC_DEBUG_ )
#define MlibByteSwap16( x, y )		(y) = (USHORT)( (((x) & 0xFF) << 8) + (((x) >> 8) & 0xFF) )
#define MlibByteSwap32( x, y )		(y) = (ULONG)( (((x) & 0xFF) << 24) + ((((x) >> 8) & 0xFF)<<16) + ((((x) >> 16) & 0xFF)<<8) + (((x) >> 24) & 0xFF) )
	#if		defined( CCSW_LITTLE_ENDIAN ) 
	#define MlibCnvLittle16( x, y )	{ ; }
	#define MlibCnvBig16( x, y )	( MlibByteSwap16( (x), (y) ) )
	#define MlibCnvBig32( x )		( MlibByteSwap32( (x), (x) ) )
	#endif
	/*----------------------------------------------------------------------------------------------*/
	#if		defined( CCSW_BIG_ENDIAN ) 
	#define MlibCnvBig16( x, y )		{ ; }
	#define MlibCnvLittle16( x, y )		( MlibByteSwap16( (x), (y) ) )
	#endif
/*--------------------------------------------------------------------------------------------------*/
#else
#define MlibByteSwap16( x, y )		{(y) = (USHORT)( (((x) & 0xFF) << 8) + (((x) >> 8) & 0xFF) );}
#endif
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Basic Constant Definition																	*/
/*																									*/
/****************************************************************************************************/
#define	SRH_FROM_LSB			0
#define	SRH_FROM_MSB			1
#define	KSPAI					0x156487ED				/* {k,s} Type   PAI							*/
#define	KS2PAI					0x146487ED				/* {k,s} Type 2*PAI							*/
#define	KS3PAI					0x134B65F2				/* {k,s} Type 3*PAI							*/
#define	KS4PAI					0x136487ED				/* {k,s} Type 4*PAI							*/
/*--------------------------------------------------------------------------------------------------*/
#define	NORMAXVALUE				0x01000000				/* Normalized Maximum Value					*/
#define	NORMINVALUE				-0x01000000				/* Normalized Minimum Value					*/
/*--------------------------------------------------------------------------------------------------*/
#define	CCSW_HsMulgain			1					/* for MlibHsMulgain() Parameter Cal.			*/
#define	FLOAT2KF1GAIN			0x800000			/* FloatからKF1GAINへの変換定数					*/

#define	PI						3.1415926535		/* π											*/
#define	DEG2RAD					0.0174532925194		/* π / 180										*/
#define	FOURTEENBIT2RAD			0.0003834951969604	/* (π / 180) * (360 / 16384)					*/

#define	C10POW9					1000000000			/* 10^9											*/
#define	C10POW7					10000000			/* 10^7											*/
#define	C2PAIE7					62831853			/* (2*PAI) * 10^7								*/

/****************************************************************************************************/
/*																									*/
/*		基本数値演算マクロ																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Basic Macro																					*/
/*--------------------------------------------------------------------------------------------------*/
#define MlibABS( u )	(((u)>=0) ? (u) : -(u))
#define MlibMAX( a, b )	(((a)>(b)) ? (a) : (b))
#define MlibMIN( a, b )	(((a)<(b)) ? (a) : (b))
#define	MlibLIMITUL( u, ulim, llim )	(((u)>=(ulim))? (ulim) : (((u)<=(llim))? (llim) : (u)))
/*--------------------------------------------------------------------------------------------------*/
/*		64bit Compare Macro : GT(a>b),LT(a<b),GTE(a>=b),LTE(a<=b)									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MlibEQ64( a, b )	( (a[1] == b[1]) && (a[0] == b[0]) )
#define	MlibGT64( a, b )	( (a[1] >  b[1]) || ((a[1] == b[1])&&((ULONG)a[0] >  (ULONG)b[0])) )
#define	MlibLT64( a, b )	( (a[1] <  b[1]) || ((a[1] == b[1])&&((ULONG)a[0] <  (ULONG)b[0])) )
#define	MlibGTE64( a, b )	( (a[1] >  b[1]) || ((a[1] == b[1])&&((ULONG)a[0] >= (ULONG)b[0])) )
#define	MlibLTE64( a, b )	( (a[1] <  b[1]) || ((a[1] == b[1])&&((ULONG)a[0] <= (ULONG)b[0])) )
/*--------------------------------------------------------------------------------------------------*/
/*		64bit Upper/Lower Limit Macro																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MlibUPPERLMT64( x, ulim )	if( MlibGT64( x, ulim ) ){ x[0] = ulim[0]; x[1] = ulim[1];}
#define	MlibLOWERLMT64( x, llim )	if( MlibLT64( x, llim ) ){ x[0] = llim[0]; x[1] = llim[1];}
/*--------------------------------------------------------------------------------------------------*/
/*		Integer Data <--> Floating Data Macro							<S00A>				*/
/*--------------------------------------------------------------------------------------------------*/
#define	NORMALIZEINT( x )		((LONG)(x*(double)167772.16))		/* 167772.16 = 16777216/100		*/
#define	NORMALIZEINTX( x )		((LONG)(x*(float)150.00))		/* 150.00 = 15000/100			*/
#define	NORMALIZEFLOAT( x )		((float)(x/(double)167772.16))	/* 167772.16 = 100/16777216		*/
#define	NORMALIZEFLOATX( x )	((float)(x/(float)150.00))	/* 150.00 = 100/15000			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		基本数値演算マクロ																			*/
/*																									*/
/****************************************************************************************************/
extern	const	SHORT	MlibFastSinTbl[1024];				/* 高速Ｓｉｎ演算 : 入力:[360deg/1024]	*/
#define	MlibFASTSINS16( a )	(MlibFastSinTbl[(a)&0x3FF])		/* 高速Ｓｉｎ演算 : 出力:[1.000/16384]	*/
#define MlibFASTCOSS16( a ) (MlibFastSinTbl[(a+256)&0x3FF])	/* 高速Ｃｏｓ演算 : 出力:[1.000/16384]	*/
/*--------------------------------------------------------------------------------------------------*/
__inline LONG MlibLABS(LONG u)
{
	return 	(((u)>=0) ? (u) : -(u));
}
__inline LONG MlibAbs32(LONG u)
{
	return 	(((u)>=0) ? (u) : -(u));
}

/****************************************************************************************************/
/*																									*/
/*		64bit数値演算マクロ																			*/
/*																									*/
/****************************************************************************************************/
#if defined( __PowerPC )
/****************************************************************************************************/
/*																									*/
/*		ＣＰＵ機種依存：INGRAM(PPC)用																*/
/*																									*/
/****************************************************************************************************/
__inline void	MUL3232(LONG a, LONG b, LONG *xx)
{
	xx[0] = (a)*(b);	
	xx[1] = __MULSH(a, b);		
}

__inline void	MULU32U32(ULONG a, ULONG b, ULONG *xx)
{
	xx[0] = (ULONG)((a)*(b));
	xx[1] = __MULUH(a, b);		
}

/*--------------------------------------------------------------------------------------------------*/
/*		割り込み禁止／許可																			*/
/*--------------------------------------------------------------------------------------------------*/
#undef	KPI_DI
#define	KPI_DI( )				{ ;}				/* 割込み禁止									*/
#undef	KPI_EI
#define	KPI_EI( )				{ ;}				/* 割込み許可									*/
/*--------------------------------------------------------------------------------------------------*/

#elif	defined( WIN32 )

/***** 浮動小数点データを扱う構造体 *****//*<S00A>*/
typedef union F2L
{
	long				l;
	unsigned long		u;
	float				f;
	//struct
	//{
	//	long			l;
	//	long			h;
	//} sl;
	//struct
	//{
	//	unsigned long	l;
	//	unsigned long	h;
	//} ul;
	//double			df;
} F2L;
/****************************************************************************************************/
/*																									*/
/*		ＣＰＵ機種依存：パソコン用																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define	MUL3232( a, b, xx )		*(INT64*)xx = ((INT64)(a))*((INT64)(b))
#define	MULU32U32( a, b, xx )	*(INT64*)xx = ((INT64)(ULONG)(a))*((INT64)(ULONG)(b))
/*--------------------------------------------------------------------------------------------------*/
/*		割り込み禁止／許可																			*/
/*--------------------------------------------------------------------------------------------------*/
#undef	KPI_DI
#define	KPI_DI( )				{ ;}				/* 割込み禁止									*/
#undef	KPI_EI
#define	KPI_EI( )				{ ;}				/* 割込み許可									*/
/*--------------------------------------------------------------------------------------------------*/

#else
/****************************************************************************************************/
/*																									*/
/*		ＣＰＵ機種依存：SGDZ-SVN(SH-4A)用															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define	MUL3232( a, b, xx )		*(INT64*)xx = ((INT64)(a))*((INT64)(b))
#define	MULU32U32( a, b, xx )	*(INT64*)xx = ((INT64)(ULONG)(a))*((INT64)(ULONG)(b))
/*--------------------------------------------------------------------------------------------------*/

#endif

typedef struct ull 
  { unsigned long lower, upper; }
ull;

typedef struct longlong
  { unsigned long lower; long upper; }
longlong;

#define Sub64_2(dest, a1, a2)	/* Subtract a2 from a1 result in dest  */  \
     (((dest).upper = (a1).upper - (a2).upper - ((a1).lower < (a2).lower)), \
     ((dest).lower = (a1).lower - (a2).lower))
#define Add64_2(dest, a1, a2)	/* Add a2 to a1.   */	   \
	((dest).lower = (a1).lower + (a2).lower, \
	(dest).upper = (a1).upper + (a2).upper + \
	((dest).lower < (a2).lower))
#define Add64_64_32_2(dest, a1, a2)	/* Like Add64_2, but a2 is UINT4 */ \
	((dest).lower = (a1).lower + (a2), \
	(dest).upper = (a1).upper + ((dest).lower < (a2)))

__inline void	ADD6464(LONG *a, LONG *b, LONG *xx)
{
#if 0
	xx[0] = a[0] + b[0];
//	xx[1] = a[1] + b[1] + (xx[0] < b[0]);
	xx[1] = a[1] + b[1] + ((ULONG)(xx[0]) < (ULONG)(b[0]));    /* <S053> */
#else/* <S07E> *///<S0C7>
LONG lworka0,lworka1;
LONG lworkb0,lworkb1;

lworka0 = a[0];
lworkb0 = b[0];
lworka1 = a[1];
lworkb1 = b[1];

	xx[0] = lworka0 + lworkb0;
	xx[1] = lworka1 + lworkb1 + ((ULONG)xx[0] < (ULONG)lworkb0);

#endif
}
__inline void	SUB6464(LONG *a, LONG *b, LONG *xx)
{
//	xx[1] = a[1] - b[1] - (a[0] < b[0]);
	xx[1] = a[1] - b[1] - ((ULONG)(a[0]) < (ULONG)(b[0]));   /* <S053> */
	xx[0] = a[0] - b[0];
}
__inline LONG	MlibMulhigh32( LONG a, LONG b )
{
	LONG xx[2];
	MUL3232(a,b,xx);
	return( xx[1] + ((ULONG)xx[0]>>31) );
}
__inline LONG	MlibMulhighu32( LONG a, LONG b )
{
	ULONG xx[2];
	MULU32U32(a,b,xx);
	return( xx[1] + ((ULONG)xx[0]>>31) );
}


/*--------------------------------------------------------------------------------------------------*/
/*		割り込み禁止／許可																			*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef _TEST_BOARD_
#undef	KPI_DI
#define	KPI_DI( )		{  /*  TODO:  割り込み禁止 */                                \
							long st;                                                 \
							__asm {                                                  \
								MRS st, CPSR;      /* st <- CPSR */                  \
								ORR st, st, 0x80; /* set interrupt disable bit */   \
								MSR CPSR_c, st;    /* CPSR <- st */                  \
							} \
						} \

#undef	KPI_EI
#define	KPI_EI( )		{ /* TODO: 割込み許可	*/\
							long st;                                                 \
							__asm {                                                  \
								MRS st, CPSR;      /* st <- CPSR */                  \
								BIC st, 0x80;     /* clear interrupt disable bit */ \
								MSR CPSR_c, st;    /* CPSR <- st */                  \
							} \
						}
/*--------------------------------------------------------------------------------------------------*/
#elif defined( WIN32 )
/* 割禁なし */
#elif defined( _MERCURY_PLATFORM_ )			/*<S190>*/
#define	KPI_DI( )		;
#define	KPI_EI( )		;
#else /* 暫定的に割禁なし */
#define	KPI_DI( )		__DI();
#define	KPI_EI( )		__EI();
#endif
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		基本制御演算関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibLimitul()       : 上下限リミット				rv=リミット値;							*/
/*   1-2) FlibLimitul()     : 上下限リミット				rv=リミット値;							*//*<S00A>*/
/*   2) MlibMulgain()       : ゲイン乗算(Limit標準:2^24)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   3) MlibMulgain27()     : ゲイン乗算(Limit拡張:2^26)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   4) MlibMulgain29()     : ゲイン乗算(Limit拡張:2^28)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   5) MlibMulgain30()     : ゲイン乗算(Limit拡張:2^29)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   6) MlibMulgain32()     : ゲイン乗算(Limit拡張:2^31)	rv=(kx*u)>>sx; 0<=sx<=24  				*/
/*   7) MlibMulgainSx48()   : ゲイン乗算(smax拡大) 			rv=(kx*u)>>sx; 0<=sx<=48				*/
/*   8) MlibMulgainNolim()  : ゲイン乗算(Limit無し)			rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   9) MlibIntegral()      : 積分演算(Limit付き)			rv=(iu[1]>>1);iu[]=iu[]+kx*u;			*/
/*   9-2) FlibIntegral()      : 積分演算(Limit付き)			rv=iu; iu+=kx*u							*//*<S00A>*/
/*--------------------------------------------------------------------------------------------------*/
/*  10) MlibPfbkxremLim()   : 余り付き位置ＦＢ計算			rv=(kx*u+pfbrem)>>sx;					*/
/*  11) MlibPfbkxremNolim() : 余り付き位置ＦＢ計算			rv=(kx*u+pfbrem)>>sx;					*/
/*  12) MlibMulkprem()      : 余り付き位置ゲイン乗算		rv=(kx*u-((kx*pfbrem)>>24))>>sx;		*/
/*  13) MlibMulkxBiasrem()  : バイアス余り付きゲイン乗算	rv=(kx*u+rem+(sign(u)*bias>>s))>>sx;	*/
/*--------------------------------------------------------------------------------------------------*/
/*	14) MlibKrMulgain()     : ゲイン乗算(ゲイン比率付き)    rv=(kr*u)>>sx; kr=(kx*r)/(10000<<16)	*/
/*	15) MlibKrMulgain27()   : ゲイン乗算(ゲイン比率付き)    rv=(kr*u)>>sx; kr=(kx*r)/(10000<<16)	*/
/*	16) MlibKrIntegral()    : 積分演算(ゲイン比率付き)      iu=iu+(kr*u); rv=((iu[1]+x)>>1);		*/
/*--------------------------------------------------------------------------------------------------*/
/*  17) MlibSymMulgain()    : ゲイン乗算(正負対称四捨五入)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*  18) MlibSymMulgainNolim : ゲイン乗算(正負対称四捨五入)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*  19) MlibSymIntegral()   : 積分演算(正負対称四捨五入)	rv=(iu[1]>>1);iu[]=iu[]+kx*u;			*/
/*																									*/
/****************************************************************************************************/
LONG	MlibLimitul( LONG u, LONG ulim, LONG llim );
#if (FLOAT_USE==TRUE)
float	FlibLimitul( float u, float ulim, float llim );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibMulgain( LONG u, LONG k );
LONG	MlibMulgain27( LONG u, LONG k );
LONG	MlibMulgain29( LONG u, LONG k );
LONG	MlibMulgain30( LONG u, LONG k );
LONG	MlibMulgain32( LONG u, LONG k );
ULONG	MlibMulgain30U( ULONG u, LONG k );
ULONG	MlibMulgain31U( ULONG u, LONG k );
ULONG	MlibMulgain32U( ULONG u, LONG k );
LONG	MlibMulgainSx48( LONG u, LONG k );
LONG	MlibMulgain32Sx48( LONG u, LONG k );
LONG	MlibMulgainNolim( LONG u, LONG k );
LONG	MlibIntegral( LONG u, LONG k, LONG iu[2] );

#if (FLOAT_USE==TRUE)
float	FlibIntegral( float u, float k, float *iu );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPfbkxremLim( LONG u, LONG k, LONG *pfbrem );
LONG	MlibPfbkxremNolim( LONG u, LONG k, LONG *pfbrem );
LONG	MlibMulkprem( LONG u, LONG k, LONG  pfbrem );
LONG	MlibMulkxBiasrem( LONG u, LONG k, LONG *rem, LONG bias );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibKrMulgain( LONG u, LONG k, LONG r );
LONG	MlibKrMulgain27( LONG u, LONG k, LONG r );
LONG	MlibKrIntegral( LONG u, LONG k, LONG r, LONG iu[2] );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibSymMulgain( LONG u, LONG k );
LONG	MlibSymMulgainNolim( LONG u, LONG k );
LONG	MlibSymIntegral( LONG u, LONG k, LONG iu[2] );
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		フィルタ演算関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibLpfilter1()     : １次ローパスフィルタ			rv=x+((kx*(u-x))>>24);					*/
/*   1-2) FlibLpfilter1()     : １次ローパスフィルタ			rv=x+((kx*(u-x))>>24);					*//*<S00A>*/
/*   2) MlibHpfilter1()     : １次ハイパスフィルタ			rv=u-z; z=z+((kx*(u-z))>>24);			*/
/*   3) MlibLpfilter2()     : ２次ローパスフィルタ			rv=(z[2]>>1);							*/
/*   4) MlibNxfilter2()     : ２次ノッチフィルタ			rv=k[2]*(u-(z[2]>>1)-z[0])+(z[2]>>1);	*/
/*   5) MlibLaufilter()     : 直線加速器(LAU)フィルタ		rv= z + dz or rv= z - dz;				*/
/*   6) MlibBITFILTER1()    : １回遅れビットフィルタ		out = f( out, oldin, newin )			*/
/*   7) MlibKrLpfilter1()   : １次ローパスフィルタ(比率付)	rv=x+((kr*(u-x))>>24);					*/
/*   8) MlibLowPassfilter2(): ２次ローパスフィルタ演算(双一次変換)									*/
/*   8-2) FlibLowPassfilter2(): ２次ローパスフィルタ演算(双一次変換)									*//*<S00A>*/
/*																									*/
/****************************************************************************************************/
LONG	MlibLpfilter1( LONG u, LONG k, LONG x  );
#if (FLOAT_USE==TRUE)
float	FlibLpfilter1( float u, float k, float x  );
float	FlibHpfilter1( float u, float k, float *z );
#endif /* FLOAT_USE */
LONG	MlibHpfilter1( LONG u, LONG k, LONG *z );
LONG	MlibLpfilter2( LONG u, LONG k[2], LONG z[3] );
LONG	MlibNxfilter2( LONG u, LONG k[3], LONG z[3] );
LONG	MlibLaufilter( LONG u, LONG z, LONG dz );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibKrLpfilter1( LONG u, LONG k, LONG r, LONG x );
LONG	MlibLowPassfilter2( LONG u, LONG k[4], LONG z[4] );
#if (FLOAT_USE==TRUE)
float	FlibLowPassfilter2( float u, float k[4], float z[4] );	/* ２次ローパスフィルタ演算(双一次変換)	*/
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		１回遅れビットフィルタマクロ																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MlibBITFILTER1( out, oldin, newin )\
		{ out = (((oldin | newin) & out ) | (oldin & newin)); oldin = newin;}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		位置関連演算関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibPcmdIpfil()     : 位置指令補間フィルタ			rv=(pcmdin+rem)/n						*/
/*   2) MlibPcmdMafil()     : 位置指令移動平均フィルタ		rv=sum/n, sum=sum+pcmdin-pmafbuf[idx]	*/
/*   3) MlibPcmdMafilSec()  : 位置指令移動平均フィルタ2		rv=sum/n, sum=sum+pcmdin-pmafbuf[idx]	*//*<S0C3>*/
/*   4) MlibPcmdImafil()    : 位置補間移動平均フィルタ		rv=sum/n, sum=sum+newpcmd/n-oldpcmd/n	*/
/*   5) MlibPcmdImafilSec() : 位置補間移動平均フィルタ2		rv=sum/n, sum=sum+newpcmd/n-oldpcmd/n	*//*<S0C3>*/
/*   6) MlibPcmdExpfil()    : 位置指令指数加減速フィルタ	rv=sum*kexp, sum=sum+pcmdin-rv			*/
/*   7) MlibPcmdLpfil1()    : 位置指令１次ローパスフィルタ	rv=z*kf, z=z+pcmdin-rv					*/
/*   8) MlibPcmdLpfil2()    : 位置指令２次ローパスフィルタ	rv=z[1]*kf1, z[0]=z[0]+pcmdin-rv		*/
/*   8-2) FlibPcmdLpfil2()  : 位置指令２次ローパスフィルタ	rv=z[1]*kf1, z[0]=z[0]+pcmdin-rv		*//*<S00A>*/
/*   9) MlibPcmdNxfil2()    : 位置指令２次ノッチフィルタ	rv=pcmdin - Delta(kf2*z[1])				*/
/*  10) MlibPcmdEgear()     : 位置指令電子ギヤ計算			rv=(B/A)*pcmda							*/
/*  10-2) FlibPcmdEgear()   : 位置指令電子ギヤ計算			rv=(B/A)*pcmda							*//*<S00A>*/
/*  11) MlibEgearRvscnv()   : 電子ギヤ逆変換計算			rv=(A/B)*inpls							*/
/*  12) MlibPcalKegear()    : 電子ギヤパラメータ計算 		OUT: egear.k1, egear.k2, etc			*/
/*  13) MlibPerrcala()      : 位置偏差計算Ａ(指令単位)		OUT: perra.per32s, perra.per32a, etc	*/
/*  13-2) FlibPerrcala()    : 位置偏差計算Ａ(指令単位)		OUT: perra.per32s, perra.per32a, etc	*//*<S00A>*/
/*  14) MlibPerrcalx()      : 位置偏差計算Ｘ(ＦＢ単位)		rv=per64[0]; per64[]=per64[]+pcmd-pfbk;	*/
/*  14-2) FlibPerrcalx()    : 位置偏差計算Ｘ(ＦＢ単位)		rv=per64[0]; per64[]=per64[]+pcmd-pfbk;	*//*<S00A>*/
/*  15) MlibAposRg64iv()    : 初期絶対位置計算(逆電子ギヤ)	OUT: aposrg.apos[],aposrem,etc			*/
/*	16) MlibAposRg64dp()    : 絶対位置更新計算(逆電子ギヤ)	OUT: aposrg.apos[],aposrem,etc			*/
/*																									*/
/****************************************************************************************************/
/*START <S0CD>*/
typedef	struct	{	
		LONG	GrvTrq;							/* Gravity Torque          */
		LONG	PosCTrq;						/* Positive Coulomb Torque */
		LONG	NegCTrq;						/* Negative Coulomb Torque */
		LONG	VisTrq;					        /* Viscous Torque          */
		LONG	MotFricEnable;	                /* Motor Friction Enable*/
		LONG    TrqCLast;						/* Torque Coulomb Last      */
		LONG    TrqGrvLast;						/* Torque Gravity Last      */
		LONG    SpdHys;				          	/* offset speed            */
		LONG    RatSpeed;				    	/* rate speed            */
		BOOL    InitEnable; 					/* Motor Friction Parameters Initialization Enable */
}FRICTION;
/*END <S0CD>*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令補間フィルタ																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* 位置指令補間フィルタ構造体定義									*/
		LONG	cmdx;			/* 位置指令入力														*/
		LONG	remx;			/* 位置指令出力余り													*/
} PIPFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdIpfil( LONG pcmdin, LONG shift, LONG index, PIPFV *pipfvar );
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令移動平均フィルタ																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* 位置指令移動平均フィルタ構造体定義								*/
		SHORT	idx;			/* バッファインデクス												*/
		SHORT	ksub;			/* 減算量演算ゲイン(0/1)											*/
		LONG	sumx;			/* 位置指令合計														*/
		LONG	remx;			/* 位置指令余り														*/
		USHORT	zcntx;			/* 払出し完了チェック(0:完了)										*/
		USHORT	spare;			/* 予備																*/
} PMAFV;
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令移動平均フィルタ2																	        */
/*--------------------------------------------------------------------------------------------------*//*<S0C3>*/
typedef	struct	{				/* 位置指令移動平均フィルタ構造体定義*/
		SHORT	idxSec;			/* バッファインデクス												*/
		SHORT	ksubSec;		/* 減算量演算ゲイン(0/1)											*/
		LONG	sumxSec;		/* 位置指令合計														*/
		LONG	remxSec;		/* 位置指令余り														*/
		USHORT	zcntxSec;		/* 払出し完了チェック(0:完了)										*/
		USHORT	spareSec;		/* 予備		                                                        */
} HIGHFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdMafil( LONG pcmdin, LONG pmafnum, PMAFV *pmafvar, LONG *pmafbuf );
LONG	MlibPcmdMafilSec( LONG pcmdin, LONG pmafnum, HIGHFV *pmafvarSec, LONG *pmafbufSec );   /*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令補間移動平均フィルタ																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* 位置指令補間移動平均フィルタ構造体定義							*/
		LONG	sumx;			/* 位置指令合計														*/
		LONG	remx;			/* 位置指令余り														*/
		LONG	newpcmd;		/* 位置指令入力														*/
		LONG	oldpcmd;		/* 位置指令入力前回値												*/
		USHORT	zcntx;			/* 払出し完了チェック(0:完了)										*/
		USHORT	spare;			/* 予備																*/
} PIMFV;
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令補間移動平均フィルタ2																*/
/*--------------------------------------------------------------------------------------------------*/ /*<S0C3>*/
typedef	struct	{				/* High filter補間移動平均フィルタ構造体定義	                    */
		LONG	sumxSec;		/* 位置指令合計														*/
		LONG	remxSec;		/* 位置指令余り														*/
		LONG	newpcmdSec;		/* 位置指令入力														*/
		LONG	oldpcmdSec;		/* 位置指令入力前回値												*/
		USHORT	zcntxSec;		/* 払出し完了チェック(0:完了)										*/
		USHORT	spareSec;		/* 予備		                                                        */
} HIMFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdImafil( LONG pcmdin, LONG shift, LONG index, PIMFV *pimfvar );
LONG	MlibPcmdImafilSec( LONG pcmdin, LONG shift, LONG indexSec, HIMFV *pimfvarSec );   /*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令指数加減速フィルタ																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* 位置指令指数加減速フィルタ構造体定義								*/
		LONG	sumx;			/* 位置指令合計														*/
		LONG	remx;			/* 位置指令余り(未使用)												*/
} PEXFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdExpfil( LONG pcmdin, LONG kexp, LONG pbias, PEXFV *pexfvar );
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令ローパスフィルタ																	*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdLpfil1( LONG pcmdin, LONG kf, LONG *z );
LONG	MlibPcmdLpfil2( LONG pcmdin, LONG kf[2], LONG z[2] );
#if (FLOAT_USE==TRUE)
float	FlibPcmdLpfil2( float pcmdin, float kf[2], float z[2] );
#endif /* FLOAT_USE */
LONG	MlibPcmdNxfil2( LONG pcmdin, LONG kf[3], LONG z[3] );
/*--------------------------------------------------------------------------------------------------*/
/*		電子ギヤ＆位置偏差構造体																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* 電子ギヤ構造体定義												*/
		LONG	a;				/* 電子ギヤＡ														*/
		LONG	b;				/* 電子ギヤＢ														*/
		LONG	k1;				/* 電子ギヤゲイン：b/aの上位bit										*/
		LONG	k2;				/* 電子ギヤゲイン：b/aの下位bit										*/
		LONG	g1;				/* 電子ギヤゲイン：a/bの上位bit										*/
		LONG	g2;				/* 電子ギヤゲイン：a/bの下位bit										*/
#if (FLOAT_USE==TRUE)
		float		f_a;		/* 電子ギヤＡ														*/
		float		f_b;		/* 電子ギヤＢ														*/
		float		f_k1;		/* 電子ギヤゲイン(正変換)：b/a										*/
		float		f_k2;		/* 電子ギヤゲイン(正変換)：b/a余り									*/
		float		f_g1;		/* 電子ギヤゲイン(逆変換)：a/b										*/
		float		f_g2;		/* 電子ギヤゲイン(逆変換)：a/b余り									*/
#endif /* FLOAT_USE */
} EGEAR;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* 位置偏差Ａ構造体定義												*/
		LONG	pfbkb;			/* 位置ＦＢ(指令単位)演算結果										*/
		LONG	pfbkrem;		/* 位置ＦＢ(指令単位)演算結果余り									*/
		LONG	per64[2];		/* 位置偏差64bit演算用												*/
		LONG	per32s;			/* 位置偏差出力(四捨五入)											*/
		LONG	per32a;			/* 同上絶対値(四捨五入)												*/
		LONG	per32sx;		/* 位置偏差出力(切り上げ)											*/
		LONG	per32ax;		/* 同上絶対値(切り上げ)												*/
		LONG	per32mx;		/* 位置偏差モニタ用(切り上げ／0.5以下でゼロ)						*/
} PERRA;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdEgear( LONG pcmda, EGEAR *egear, LONG *pcmdrem );
#if (FLOAT_USE==TRUE)
float	FlibPcmdEgear( float pcmda, EGEAR *egear, float *pcmdrem );
#endif /* FLOAT_USE */
void	MlibPcalKegear( LONG a, LONG b, EGEAR *egear );
LONG	MlibEgearRvscnv( LONG inpls, EGEAR *egear, LONG *plsrem );
void	MlibPerrcala( LONG pcmda, LONG pfbka, EGEAR *egear, PERRA *perra );
LONG	MlibPerrcalx( LONG pcmdx, LONG pfbkx, LONG per64[2] );
#if (FLOAT_USE==TRUE)
void	FlibPerrcala( LONG pcmda, LONG pfbka, EGEAR *egear, PERRA *perra );	/* 仮処置	*/
LONG	FlibPerrcalx( LONG pcmdx, LONG pfbkx, LONG per64[2] );	/* 仮処置	*/
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		絶対位置演算(逆電子ギヤ：ＦＢ単位-->指令単位)												*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{
		LONG	dposix;				/* 位置差分				[指令単位]								*/
		LONG	aposrem;			/* 位置余り				[指令単位]								*/
		LONG	apos[2];			/* 絶対位置(64bit)		[指令単位]								*/
} APOSRG;
/*--------------------------------------------------------------------------------------------------*/
void	MlibAposRg64iv( LONG inipos0, LONG inipos1, EGEAR *egear, APOSRG *aposrg );
void	MlibAposRg64dp( LONG dposin, EGEAR *egear, APOSRG *aposrg, LONG *latchpos );
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		位置指令作成器関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibPcmdMaker()     : 位置指令作成器(64bit)			rv=vp, vp=vp+vpacc/vp=vp-vpdec			*/
/*   2) MlibIpTposLimit()   : 同上補間位置指令リミット		rv=TRUE(Limit)/FALSE(NotLimit)			*/
/*   3) MlibRstPcmdMaker()  : 同上リセット処理				pcmdout[]=pcmdset0,pcmdset1,etc			*/
/*   4) MlibIpcalPcmdMaker  : 同上初期パラメータ計算		OUT: pcmkprm.osvpm, maxvpm, pshlx, etc	*/
/*   5) MlibPcalaPcmdMaker  : 同上位置決めパラメータ計算Ａ	OUT: pcmkprm.vpacc, vpdec,  vpamx, etc	*/
/*   6) MlibPcalaPcmdMkrIP  : 同上補間演算パラメータ計算Ａ	OUT: pcmkprm.ipmaxspd, ipmaxacc,   etc	*/
/*   7) MlibPcalbPcmdMaker  : 同上位置決めパラメータ計算Ｂ	OUT: pcmkprm.vpacc, vpdec,  vpamx, etc	*/
/*   8) MlibPcalbPcmdMkrIP  : 同上補間演算パラメータ計算Ｂ	OUT: pcmkprm.ipmaxspd, ipmaxacc,   etc	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令作成器																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{					/* 位置作成器パラメータ構造体定義								*/
		LONG	osvpm;				/* ＯＳモータ位置指令速度				[xpulse/scan]			*/
		LONG	maxvpm;				/* 最大モータ位置指令速度				[xpulse/scan]			*/
		LONG	maxipv;				/* 補間モード最大位置指令速度			[xpulse/scan]			*/
		UCHAR	pshlx;				/* 位置指令演算倍率(シフト)				[-]						*/
		UCHAR	pcmd64f;			/* 位置指令６４ビットフラグ				[TRUE/FALSE]			*/
		USHORT	iptimes;			/* Interpolate補間回数					[-]						*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	vpacc;				/* 位置指令加速度						[xpulse/scan/scan]		*/
		LONG	vpdec;				/* 位置指令減速度						[xpulse/scan/scan]		*/
		LONG	vpamx;				/* 商(vpacc/vpdec)						[-]						*/
		LONG	vparx;				/* 余り(vpacc%vpdec)					[-]						*/
		LONG	vpapx;				/* 加速分減速停止パルス量				[xpulse]				*/
		LONG	maxspd;				/* 最大位置指令速度						[2^24/OvrSpd]			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	ipmaxspd;			/* 補間モード最大速度					[ipulse/IPcycle]		*/
		LONG	ipmaxacc;			/* 補間モード最大加速度(未使用)			[ipulse/IPcycle^2]		*/
		LONG	ipstpdec;			/* 補間モード非常停止減速度				[ipulse/IPcycle^2]		*/
} PCMKP;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{					/* 位置作成器演算用変数構造体定義								*/
		UCHAR	calendf;			/* 演算終了フラグ (演算中:0, 演算終了:1)						*/
		UCHAR	cmderrf;			/* 指令異常フラグ (補間モード位置指令異常,etc)					*/
		UCHAR	pcmkmode;			/* 位置指令作成モード											*/
		UCHAR	modechgf;			/* 位置指令作成モード変化フラグ									*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	vpx;				/* 位置指令速度							[xpulse/scan]			*/
		LONG	vpxrem;				/* 位置指令速度出力余り					[xpulse/scan]			*/
		LONG	pcmdout[2];			/* 位置指令出力絶対値					[ipulse]				*/
		LONG	avp;				/* Work:位置指令速度絶対値				[xpulse/scan]			*/
		LONG	n;					/* Work:商(avp/vpdec)					[-]						*/
		LONG	rem;				/* Work:余り(avp%vpdec)					[-]						*/
		LONG	maxvp;				/* Work:最大位置指令速度				[xpulse/scan]			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	ipvpi;				/* Interpolate位置指令速度				[ipulse/IPcycle]		*/
		LONG	ipvpx;				/* Interpolate位置指令速度				[ipulse/scan]			*/
		LONG	ipremi;				/* Interpolate位置指令速度余り			[ipulse/scan]			*/
		LONG	ipremx;				/* Interpolate位置指令速度余り			[ipulse/scan]			*/
		LONG	ipstopx[2];			/* IpWork:減速停止位置計算値			[ipulse]				*/
} PCMKV;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{					/* 位置作成器演算用変数(P&V)構造体定義							*/
		PCMKP	P;					/* パラメータ													*/
		PCMKV	V;					/* 演算用変数													*/
} PCMKPV;
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令作成器演算実行関数																	*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdMaker(				/* 位置指令作成器演算実行関数									*/
		LONG	pcmdin0,			/* 位置指令入力(下位32bit)				[pulse]					*/
		LONG	pcmdin1,			/* 位置指令入力(上位32bit)				[pulse]					*/
		LONG	pcmdspd,			/* 位置指令速度(最大／送り)				[2^24/OvrSpd]			*/
		PCMKPV	*pcmdmkr,			/* 位置指令作成器演算用変数(P&V)		[-]						*/
		ULONG	pcmkmode	);		/* 位置指令作成モード					[-]						*/
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令作成モード定義：基本モードは、下位バイトで定義すること								*/
/*--------------------------------------------------------------------------------------------------*/
#define	PCMKMODE_NONE   0x0000		/* 位置指令作成モード : 無し									*/
#define	PCMKMODE_IPA	0x0001		/* 位置指令作成モード : 補間(入力更新有り)						*/
#define	PCMKMODE_IPB	0x0101		/* 位置指令作成モード : 補間(入力更新無し)						*/
#define	PCMKMODE_POS	0x0002		/* 位置指令作成モード : 位置決め								*/
#define	PCMKMODE_FEED	0x0003		/* 位置指令作成モード : 定速送り								*/
#define	PCMKMODE_STOP	0x0004		/* 位置指令作成モード : 減速停止								*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibIpTposLimit(			/* 補間位置指令リミット演算										*/
		LONG	*tpos,				/* 目標位置(64bit)						[pulse]					*/
		LONG	*lmtpos,			/* リミット位置(64bit)					[pulse]					*/
		LONG	lmtdir,				/* リミット方向(正側／負側)				[-]						*/
		PCMKPV	*pcmdmkr	);		/* 位置指令作成器演算用変数(P&V)		[-]						*/
/*--------------------------------------------------------------------------------------------------*/
#define	IPTPOSLMT_PSIDE		1		/* リミット方向：正側											*/
#define	IPTPOSLMT_NSIDE		0		/* リミット方向：負側											*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibRstPcmdMaker(			/* 位置指令作成器リセット処理									*/
		LONG	pcmdset0,			/* 位置指令初期値(下位32bit)			[pulse]					*/
		LONG	pcmdset1,			/* 位置指令初期値(上位32bit)			[pulse]					*/
		PCMKPV	*pcmdmkr	);		/* 位置指令作成器演算用変数(P&V)		[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibIpcalPcmdMaker(			/* 位置指令作成器初期パラメータ計算								*/
		KSGAIN	ksosvp,				/* ＯＳパルス速度						[xpulse/scan]			*/
		LONG	maxspdm,			/* 最大モータ速度						[2^24/OvrSpd]			*/
		LONG	maxspdi,			/* 補間モード最大速度					[2^24/OvrSpd]			*/
		LONG	pcmd64f,			/* 位置指令６４ビットフラグ				[TRUE/FALSE]			*/
		PCMKP	*pcmkprm	);		/* 計算結果出力構造体ポインタ			[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalaPcmdMaker(			/* 位置指令作成器位置決めパラメータ計算Ａ						*/
		LONG	maxspd,				/* 最大速度								[2^24/OvrSpd]			*/
		LONG	acctime,			/* 加速時間								[xs]					*/
		LONG	dectime,			/* 減速時間								[xs]					*/
		LONG	scantime,			/* スキャンタイム						[xs]					*/
		PCMKP	*pcmkprm	);		/* 計算結果出力構造体ポインタ			[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalaPcmdMkrIP(			/* 位置指令作成器補間演算パラメータ計算Ａ						*/
		LONG	iptimes,			/* 補間回数								[-]						*/
		LONG	maxspd,				/* 最大速度								[2^24/OvrSpd]			*/
		LONG	ipacctm,			/* 最小加減速時間(未使用)				[xs]					*/
		LONG	ipdectm,			/* 非常停止減速時間						[xs]					*/
		LONG	scantime,			/* スキャンタイム						[xs]					*/
		PCMKP	*pcmkprm	);		/* 計算結果出力構造体ポインタ			[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalbPcmdMaker(			/* 位置指令作成器位置決めパラメータ計算Ｂ						*/
		LONG	accrate,			/* 加速レート							[10000pulse/s/s]		*/
		LONG	decrate,			/* 減速レート							[10000pulse/s/s]		*/
		LONG	scantime,			/* スキャンタイム(stm)					[us(Y=0)/ns(Y=1)]		*/
		PCMKP	*pcmkprm,			/* 計算結果出力構造体ポインタ			[-]						*/
		LONG	insel		);		/* 入力選択(0xYX)						[X:不使用, Y:us/ns選択]	*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalbPcmdMkrIP(			/* 位置指令作成器補間演算パラメータ計算Ｂ						*/
		LONG	iptimes,			/* 補間回数								[-]						*/
		LONG	ipaccrt,			/* 最大加減速レート(未使用)				[10000pulse/s/s]		*/
		LONG	ipdecrt,			/* 非常停止減速レート					[10000pulse/s/s]		*/
		LONG	scantime,			/* スキャンタイム(stm)					[us(Y=0)/ns(Y=1)]		*/
		PCMKP	*pcmkprm,			/* 計算結果出力構造体ポインタ			[-]						*/
		LONG	insel		);		/* 入力選択(0xYX)						[X:不使用, Y:us/ns選択]	*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		パラメータ計算用関数																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibPcalKxgain()    : 比例ゲイン計算				{kx,sx} = (a*b/c)<<sx;					*/
/*   2) MlibPcalKskxkx()    : 拡張ゲイン計算				{kx,sx} = {ka,sa}*b/c<<sx;				*/
/*   3) MlibPcalKxkskx()    : 拡張ゲイン計算				{kx,sx} = a*{kb,sb}/c<<sx;				*/
/*   4) MlibPcalKxkxks()    : 拡張ゲイン計算				{kx,sx} = a*b/{kc,sc}<<sx;				*/
/*   5) MlibPcalKskskx()    : 拡張ゲイン計算				{kx,sx} = {ka,sa}*{kb,sb}/c<<sx;		*/
/*   6) MlibPcalKxksks()    : 拡張ゲイン計算				{kx,sx} = a*{kb,sb}/{kc,sc}<<sx;		*/
/*   7) MlibPcalKsksks()    : 拡張ゲイン計算				{kx,sx} = {ka,sa}*{kb,sb}/{kc,sc}<<sx;	*/
/*--------------------------------------------------------------------------------------------------*/
/*   8) MlibScalKxgain()    : 比例ゲイン計算(計算開始用)	{kx,sx} = (a*b/c)<<sx; (指数初期化不要)	*/
/*   9) MlibScalKskxkx()    : 拡張ゲイン計算(計算開始用)	{kx,sx} = {ka,sa}*b/c<<sx;				*/
/*  10) MlibScalKxkskx()    : 拡張ゲイン計算(計算開始用)	{kx,sx} = a*{kb,sb}/c<<sx;				*/
/*  11) MlibScalKxkxks()    : 拡張ゲイン計算(計算開始用)	{kx,sx} = a*b/{kc,sc}<<sx;				*/
/*  12) MlibScalKskskx()    : 拡張ゲイン計算(計算開始用)	{kx,sx} = {ka,sa}*{kb,sb}/c<<sx;		*/
/*  13) MlibScalKxksks()    : 拡張ゲイン計算(計算開始用)	{kx,sx} = a*{kb,sb}/{kc,sc}<<sx;		*/
/*  14) MlibScalKsksks()    : 拡張ゲイン計算(計算開始用)	{kx,sx} = {ka,sa}*{kb,sb}/{kc,sc}<<sx;	*/
/*--------------------------------------------------------------------------------------------------*/
/*  15) MlibPcalKxaddx()    : 比例ゲイン加算				{kx,sx} = {ka,sa}+{kb,sb};				*/
/*  16) MlibPcalKxsubx()    : 比例ゲイン減算				{kx,sx} = {ka,sa}-{kb,sb};				*/
/*  17) MlibPcalKxmulx()    : 比例ゲイン乗算				{kx,sx} = (a*b*c)<<sx;					*/
/*  18) MlibPcalKxdivx()    : 比例ゲイン除算				{kx,sx} = {ka,sa}/{kb,sb};				*/
/*--------------------------------------------------------------------------------------------------*/
/*  19) MlibPcalKf1gain()   : １次フィルタゲイン計算		{kf,24} = (ts/(tx+ts))<<24;				*/
/*  19-2) FlibPcalKf1gain()   : １次フィルタゲイン計算		{kf} = (ts/(tx+ts));					*//*<S00A>*/
/*  20) MlibPcalKf2gain()   : ２次フィルタゲイン計算		OUT: kf[0], kf[1]						*/
/*  20-2) FlibPcalKf2gain()   : ２次フィルタゲイン計算		OUT: kf[0], kf[1]						*//*<S00A>*/
/*  21) MlibPcalKnf2gain()  : ２次ノッチフィルタゲイン計算	OUT: kf[0], kf[1], kf[2]				*/
/*																									*/
/****************************************************************************************************/
LONG	MlibPcalKxgain( LONG a,   LONG b,   LONG c,   LONG *psx, LONG smax );
LONG	MlibPcalKskxkx( LONG ksa, LONG b,   LONG c,   LONG *psx, LONG smax );
LONG	MlibPcalKxkskx( LONG a,   LONG ksb, LONG c,   LONG *psx, LONG smax );
LONG	MlibPcalKxkxks( LONG a,   LONG b,   LONG ksc, LONG *psx, LONG smax );
LONG	MlibPcalKskskx( LONG ksa, LONG ksb, LONG c,   LONG *psx, LONG smax );
LONG	MlibPcalKxksks( LONG a,   LONG ksb, LONG ksc, LONG *psx, LONG smax );
LONG	MlibPcalKsksks( LONG ksa, LONG ksb, LONG ksc, LONG *psx, LONG smax );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibScalKxgain( LONG a,   LONG b,   LONG c,   LONG *psx, LONG smax );
LONG	MlibScalKskxkx( LONG ksa, LONG b,   LONG c,   LONG *psx, LONG smax );
LONG	MlibScalKxkskx( LONG a,   LONG ksb, LONG c,   LONG *psx, LONG smax );
LONG	MlibScalKxkxks( LONG a,   LONG b,   LONG ksc, LONG *psx, LONG smax );
LONG	MlibScalKskskx( LONG ksa, LONG ksb, LONG c,   LONG *psx, LONG smax );
LONG	MlibScalKxksks( LONG a,   LONG ksb, LONG ksc, LONG *psx, LONG smax );
LONG	MlibScalKsksks( LONG ksa, LONG ksb, LONG ksc, LONG *psx, LONG smax );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalKxaddx( LONG ka,  LONG sa,  LONG kb,  LONG sb,   LONG *psx );
LONG	MlibPcalKxsubx( LONG ka,  LONG sa,  LONG kb,  LONG sb,   LONG *psx );
LONG	MlibPcalKxmulx( LONG a,   LONG b,   LONG c,   LONG *psx            );
LONG	MlibPcalKxdivx( LONG ka,  LONG sa,  LONG kb,  LONG sb,   LONG smax );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalKf1gain(			/* １次フィルタゲイン計算										*/
		LONG	tx,					/* 時定数(Tx)/周波数(Fx)	[xs],[0.1Hz] (X=0:Tx, X=1:Fx)		*/
		LONG	ts,					/* スキャンタイム			[xs],[us/ns] (Y=0:us, Y=1:ns)		*/
		LONG	insel	);			/* 入力選択(0xYX)			[X:Tx/Fx選択, Y:us/ns選択]			*/

#if (FLOAT_USE==TRUE)
float	FlibPcalKf1gain(		/* １次フィルタゲイン計算										*/
		LONG	tx,				/* 時定数(Tx)/周波数(Fx)	[sec],[Hz] (X=0:Tx, X=1:Fx)			*/
		LONG	ts,				/* 演算周期					[sec] 								*/
		LONG	insel	);			/* 入力指定(0x0X)			[X:Tx/Fx選択]						*/
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
void	MlibPcalKf2gain(			/* ２次フィルタゲイン計算										*/
		LONG	hz,					/* フィルタ周波数			[0.1Hz]								*/
		LONG	dx,					/* フィルタ減衰係数			[0.001]								*/
		LONG	ts,					/* スキャンタイム			[us/ns]	(Y=0:us, Y=1:ns)			*/
		LONG	kf[2],				/* 計算結果格納ポインタ		[--]								*/
		LONG	insel	);			/* 入力選択(0xYX) 			[X:不使用, Y:us/ns選択]				*/
#if (FLOAT_USE==TRUE)
void	FlibPcalKf2gain(			/* ２次フィルタゲイン計算										*/
		float	hz,				/* フィルタ周波数			[Hz]								*/
		float	dx,				/* フィルタ減衰係数(ζ)		[-]									*/
		float	ts,				/* スキャンタイム			[sec]								*/
		float	kf[2]);			/* 計算結果格納ポインタ		[--]								*/
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
void	MlibPcalKnf2gain(			/* ２次ノッチフィルタゲイン計算									*/
		LONG	hz,					/* フィルタ周波数			[0.1Hz]								*/
		LONG	qx,					/* Ｑ定数(Qx)/減衰係数(Dx)	[0.001]	(X=0:Qx, X=1:Dx)			*/
		LONG	kn,					/* ノッチ深さゲイン			[0.001]								*/
		LONG	ts,					/* スキャンタイム			[us/ns] (Y=0:us, Y=1:ns)			*/
		LONG	kf[3],				/* 計算結果格納ポインタ		[--]								*/
		LONG	insel	);			/* 入力選択(0xYX)			[X:Qx/Dx選択, Y:us/ns選択]			*/
// <V069> Change Start
/*--------------------------------------------------------------------------------------------------*/
/*		ゲイン操作マクロ																				*/
/*--------------------------------------------------------------------------------------------------*/
/*			Little Endian 用																			*/
#define	MlibGAINKX( k )		((LONG)((k)<<8)>>8)
#define	MlibGAINSX( k )		(((CHAR*)&(k))[3])
/*--------------------------------------------------------------------------------------------------*/
/*			切り捨て 用																				*/
#define	MlibGAINRD( k )		(MlibGAINKX( k ) >> ((LONG) ((ULONG) (k)>>24)))
/*--------------------------------------------------------------------------------------------------*/
// <V069> Change End



/****************************************************************************************************/
/*																									*/
/*		基本数値演算関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibAbs32()         : 絶対値(32bit)															*/
/*   2) MlibAdd6432()       : 加算(符号付,64bit+32bit)												*/
/*   3) MlibAdd6464()       : 加算(符号付,64bit+64bit)												*/
/*   4)	MlibSub6432()       : 減算(符号付,64bit-32bit)												*/
/*   5)	MlibSub6464()       : 減算(符号付,64bit-64bit)												*/
/*   6) MlibErr6464()       : 偏差(符号付,64bit-64bit,戻り値飽和機能付き)							*/
/*   7) MlibSatAdd24()      : 飽和加算(符号付,32bit+32bit,Limit:0x01000000,0xFF000000)				*/
/*   8) MlibSatAdd28()      : 飽和加算(符号付,32bit+32bit,Limit:0x10000000,0xF0000000)				*/
/*   9) MlibSatAdd32()      : 飽和加算(符号付,32bit+32bit,Limit:0x7FFFFFFF,0x80000000)				*/
/*  10) MlibSatAddu32()     : 飽和加算(符号無,32bit+32bit,Limit:0xFFFFFFFF)							*/
/*  11) MlibMul3232()       : 乗算(符号付,32bit*32bit)												*/
/*  12) MlibMulu32u32()     : 乗算(符号無,32bit*32bit)												*/
/*  13) MlibMulhigh32()     : 乗算(符号付,32bit*32bit,戻り値:上位32bit,四捨五入)					*/
/*  14) MlibMulhighu32()    : 乗算(符号無,32bit*32bit,戻り値:上位32bit,四捨五入)					*/
/*  15) MlibDivhrem()       : 除算(符号付,32bit/16bit,出力:商＆余り)								*/
/*  16) MlibDiv6432()       : 除算(符号付,32bit*32bit/32bit,Limit:0x7FFFFFFF,四捨五入)				*/
/*  17) MlibDivx3232()      : 拡張除算((32bit<<sx)/32bit,Limit:(qmax<<sx))							*/
/*  18) MlibSqrtu32()       : ルート演算(入力:32bit, 出力:16bit(Max.0xFFFF),四捨五入)				*/
/*  19) MlibSqrtu64()       : ルート演算(入力:64bit, 出力:32bit(Max.0xFFFFFFFF),四捨五入)			*/
/*  20) MlibSqrtu32u32()    : ルート演算(入力:32bit*32bit, 出力:32bit(Max.0xFFFFFFFF),四捨五入)		*/
/*  21) MlibSins16()        : Ｓｉｎ演算(入力:[0.1deg],[360/16384deg],出力:[1.0/10000],[1.0/16384])	*/
/*	22) MlibAbsErrchk32()   : 絶対値偏差チェック(rv=(|a32-b32|<=chk32))								*/
/*	23) MlibAbsErrchk64()   : 絶対値偏差チェック(rv=(|a64-b64|<=chk32))								*/
/*																									*/
/****************************************************************************************************/
//LONG	MlibAbs32( LONG u );
//void	MlibAdd6432( LONG *a, LONG  b, LONG *x );
//void	MlibAdd6464( LONG *a, LONG *b, LONG *x );
/*--------------------------------------------------------------------------------------------------*/
//void	MlibSub6432( LONG *a, LONG  b, LONG *x );
//void	MlibSub6464( LONG *a, LONG *b, LONG *x );
LONG	MlibErr6464( LONG *a, LONG *b );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibSatAdd24( LONG a, LONG b );
LONG	MlibSatAdd28( LONG a, LONG b );
KSGAIN	MlibSatAdd32( KSGAIN a, KSGAIN b );		/*<S00A>*/
ULONG	MlibSatAddu32( ULONG a, ULONG b );
/*--------------------------------------------------------------------------------------------------*/
//void	MlibMul3232( LONG a, LONG b, LONG *x );
//void	MlibMulu32u32( ULONG a, ULONG b, ULONG *x );
//LONG	MlibMulhigh32( LONG a, LONG b );
//ULONG	MlibMulhighu32( ULONG a, ULONG b );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibDivhrem( LONG u, SHORT k, SHORT *rem );
LONG	MlibDiv6432( LONG a, LONG b, LONG c );
LONG	MlibDivx3232( LONG a, LONG b, LONG qmax, LONG sx );
/*--------------------------------------------------------------------------------------------------*/
USHORT	MlibSqrtu32( ULONG a );
ULONG	MlibSqrtu64( ULONG a0, ULONG a1 );
ULONG	MlibSqrtu32u32( ULONG a, ULONG b );
LONG	MlibSins16( LONG a, LONG unit );
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibAbsErrchk32( LONG  a32, LONG  b32, LONG chk32 );
LONG	MlibAbsErrchk64( LONG *a64, LONG *b64, LONG chk32 );
/*--------------------------------------------------------------------------------------------------*/
#define	MlibAdd6464( a, b, xx )		ADD6464( a, b, xx )
#define	MlibSub6464( a, b, xx )		SUB6464( a, b, xx )
#define	MlibMul3232( a, b, xx )		MUL3232( a, b, xx )
#define	MlibMulu32u32( a, b, xx )	MULU32U32( a, b, xx )



/****************************************************************************************************/
/*																									*/
/*		特殊演算処理関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibSrhbiton()      : サーチビットＯＮ(LSB/MSBからサーチ, 戻り値:BitNo/NG(-1))				*/
/*	 2) MlibSetCRC16()		: ＣＲＣ１６の設定(MEMOBUS用),		戻り値:無し							*/
/*	 3) MlibChkCRC16()		: ＣＲＣ１６のチェック(MEMOBUS用),	戻り値:TRUE(正常)/FALSE(異常)		*/
/*																									*/
/****************************************************************************************************/
LONG	MlibSrhbiton( ULONG data, ULONG dir );
#define	SRH_FROM_LSB	0
#define	SRH_FROM_MSB	1
/*--------------------------------------------------------------------------------------------------*/
#define MlibSetCRC16( Dbuf, Length) MlibSetCRC16MB( Dbuf, Length )
#define MlibChkCRC16( Dbuf, Length) MlibChkCRC16MB( Dbuf, Length )
/*--------------------------------------------------------------------------------------------------*/
void	MlibSetCRC16MB( UCHAR *MsgBuf, LONG MsgLen );
LONG	MlibChkCRC16MB( UCHAR *MsgBuf, LONG MsgLen );
/*--------------------------------------------------------------------------------------------------*/
void	MlibSetSum16SM( UCHAR *MsgBuf, LONG MsgLen );		/* for Sigma Message */
LONG	MlibChkSum16SM( UCHAR *MsgBuf, LONG MsgLen );		/* for Sigma Message */
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibCheckPowerOf2(							/* ２の累乗チェック : rv = BitNo/NG(-1)			*/
			ULONG	x 			);					/* チェックデータ								*/
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		メモリリセット関数																			*/
/*																									*/
/****************************************************************************************************/
void	MlibResetByteMemory(						/* バイトメモリリセット							*/
			void	*pRstMem,						/* リセットメモリ先頭ポインタ					*/
			LONG	ByteNum		);					/* リセットメモリバイト数						*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibResetLongMemory(						/* ロングメモリリセット							*/
			void	*pRstMem,						/* リセットメモリ先頭ポインタ					*/
			LONG	LwdNum		);					/* リセットメモリロングワード数					*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibCopyByteMemory(							/* バイトメモリコピー							*/
			void		*pDesMem,					/* コピー先										*/
			const void	*pSrcMem,					/* コピー元										*/
			INT			ByteNum	);					/* コピーバイト数								*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibCopyLongMemory(							/* ロングメモリコピー							*/
			void	*dstMem,						/* コピー先										*/
			void	*srcMem,						/* コピー元										*/
			INT		nbyte );						/* コピーバイト数								*/
/*--------------------------------------------------------------------------------------------------*/
void	memcpy( void*, void*, UINT );				/* バイトメモリコピー							*/

/****************************************************************************************************/
/*																									*/
/*		その他																			*/
/*																									*/
/****************************************************************************************************/
/*		指数値の計算 : 10^exp																		*/
LONG	MlibExpCal( CHAR exp );
#if (FLOAT_USE==TRUE)
void	FlibExpCal( CHAR exp, float *output );
#endif /* FLOAT_USE */
/*		KSGAIN → LONG Data 変換																	*/
LONG	MlibKsgain2Long( KSGAIN Input );

extern const	UCHAR	MlibAsciiTbl[ ];
extern const	UCHAR	MlibEvenAsciiTbl[ ]; 					/*<S02D>*/

/*--------------------------------------------------------------------------------------------------*/
/*		１０進累乗テーブル(Decimal Power Table)														*/
/*--------------------------------------------------------------------------------------------------*/
extern const ULONG MlibDecPowerTbl[];




#define MLIB_H_
#endif /* MLIB_H_ */
/***************************************** end of file **********************************************/


