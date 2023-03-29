#ifndef		 MLIB_H_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		mlib_c_float.h : float��MLIB�֐���`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		����   : 2000.01.14  T.Taniguchi	Mlib85E.c												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"

/****************************************************************************************************/
/*																									*/
/*		�G���f�B�A���ϊ��}�N��																		*/
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
#define	FLOAT2KF1GAIN			0x800000			/* Float����KF1GAIN�ւ̕ϊ��萔					*/

#define	PI						3.1415926535		/* ��											*/
#define	DEG2RAD					0.0174532925194		/* �� / 180										*/
#define	FOURTEENBIT2RAD			0.0003834951969604	/* (�� / 180) * (360 / 16384)					*/

#define	C10POW9					1000000000			/* 10^9											*/
#define	C10POW7					10000000			/* 10^7											*/
#define	C2PAIE7					62831853			/* (2*PAI) * 10^7								*/

/****************************************************************************************************/
/*																									*/
/*		��{���l���Z�}�N��																			*/
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
/*		��{���l���Z�}�N��																			*/
/*																									*/
/****************************************************************************************************/
extern	const	SHORT	MlibFastSinTbl[1024];				/* �����r�������Z : ����:[360deg/1024]	*/
#define	MlibFASTSINS16( a )	(MlibFastSinTbl[(a)&0x3FF])		/* �����r�������Z : �o��:[1.000/16384]	*/
#define MlibFASTCOSS16( a ) (MlibFastSinTbl[(a+256)&0x3FF])	/* �����b�������Z : �o��:[1.000/16384]	*/
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
/*		64bit���l���Z�}�N��																			*/
/*																									*/
/****************************************************************************************************/
#if defined( __PowerPC )
/****************************************************************************************************/
/*																									*/
/*		�b�o�t�@��ˑ��FINGRAM(PPC)�p																*/
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
/*		���荞�݋֎~�^����																			*/
/*--------------------------------------------------------------------------------------------------*/
#undef	KPI_DI
#define	KPI_DI( )				{ ;}				/* �����݋֎~									*/
#undef	KPI_EI
#define	KPI_EI( )				{ ;}				/* �����݋���									*/
/*--------------------------------------------------------------------------------------------------*/

#elif	defined( WIN32 )

/***** ���������_�f�[�^�������\���� *****//*<S00A>*/
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
/*		�b�o�t�@��ˑ��F�p�\�R���p																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define	MUL3232( a, b, xx )		*(INT64*)xx = ((INT64)(a))*((INT64)(b))
#define	MULU32U32( a, b, xx )	*(INT64*)xx = ((INT64)(ULONG)(a))*((INT64)(ULONG)(b))
/*--------------------------------------------------------------------------------------------------*/
/*		���荞�݋֎~�^����																			*/
/*--------------------------------------------------------------------------------------------------*/
#undef	KPI_DI
#define	KPI_DI( )				{ ;}				/* �����݋֎~									*/
#undef	KPI_EI
#define	KPI_EI( )				{ ;}				/* �����݋���									*/
/*--------------------------------------------------------------------------------------------------*/

#else
/****************************************************************************************************/
/*																									*/
/*		�b�o�t�@��ˑ��FSGDZ-SVN(SH-4A)�p															*/
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
/*		���荞�݋֎~�^����																			*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef _TEST_BOARD_
#undef	KPI_DI
#define	KPI_DI( )		{  /*  TODO:  ���荞�݋֎~ */                                \
							long st;                                                 \
							__asm {                                                  \
								MRS st, CPSR;      /* st <- CPSR */                  \
								ORR st, st, 0x80; /* set interrupt disable bit */   \
								MSR CPSR_c, st;    /* CPSR <- st */                  \
							} \
						} \

#undef	KPI_EI
#define	KPI_EI( )		{ /* TODO: �����݋���	*/\
							long st;                                                 \
							__asm {                                                  \
								MRS st, CPSR;      /* st <- CPSR */                  \
								BIC st, 0x80;     /* clear interrupt disable bit */ \
								MSR CPSR_c, st;    /* CPSR <- st */                  \
							} \
						}
/*--------------------------------------------------------------------------------------------------*/
#elif defined( WIN32 )
/* ���ւȂ� */
#elif defined( _MERCURY_PLATFORM_ )			/*<S190>*/
#define	KPI_DI( )		;
#define	KPI_EI( )		;
#else /* �b��I�Ɋ��ւȂ� */
#define	KPI_DI( )		__DI();
#define	KPI_EI( )		__EI();
#endif
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		��{���䉉�Z�֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibLimitul()       : �㉺�����~�b�g				rv=���~�b�g�l;							*/
/*   1-2) FlibLimitul()     : �㉺�����~�b�g				rv=���~�b�g�l;							*//*<S00A>*/
/*   2) MlibMulgain()       : �Q�C����Z(Limit�W��:2^24)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   3) MlibMulgain27()     : �Q�C����Z(Limit�g��:2^26)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   4) MlibMulgain29()     : �Q�C����Z(Limit�g��:2^28)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   5) MlibMulgain30()     : �Q�C����Z(Limit�g��:2^29)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   6) MlibMulgain32()     : �Q�C����Z(Limit�g��:2^31)	rv=(kx*u)>>sx; 0<=sx<=24  				*/
/*   7) MlibMulgainSx48()   : �Q�C����Z(smax�g��) 			rv=(kx*u)>>sx; 0<=sx<=48				*/
/*   8) MlibMulgainNolim()  : �Q�C����Z(Limit����)			rv=(kx*u)>>sx; 0<=sx<=24				*/
/*   9) MlibIntegral()      : �ϕ����Z(Limit�t��)			rv=(iu[1]>>1);iu[]=iu[]+kx*u;			*/
/*   9-2) FlibIntegral()      : �ϕ����Z(Limit�t��)			rv=iu; iu+=kx*u							*//*<S00A>*/
/*--------------------------------------------------------------------------------------------------*/
/*  10) MlibPfbkxremLim()   : �]��t���ʒu�e�a�v�Z			rv=(kx*u+pfbrem)>>sx;					*/
/*  11) MlibPfbkxremNolim() : �]��t���ʒu�e�a�v�Z			rv=(kx*u+pfbrem)>>sx;					*/
/*  12) MlibMulkprem()      : �]��t���ʒu�Q�C����Z		rv=(kx*u-((kx*pfbrem)>>24))>>sx;		*/
/*  13) MlibMulkxBiasrem()  : �o�C�A�X�]��t���Q�C����Z	rv=(kx*u+rem+(sign(u)*bias>>s))>>sx;	*/
/*--------------------------------------------------------------------------------------------------*/
/*	14) MlibKrMulgain()     : �Q�C����Z(�Q�C���䗦�t��)    rv=(kr*u)>>sx; kr=(kx*r)/(10000<<16)	*/
/*	15) MlibKrMulgain27()   : �Q�C����Z(�Q�C���䗦�t��)    rv=(kr*u)>>sx; kr=(kx*r)/(10000<<16)	*/
/*	16) MlibKrIntegral()    : �ϕ����Z(�Q�C���䗦�t��)      iu=iu+(kr*u); rv=((iu[1]+x)>>1);		*/
/*--------------------------------------------------------------------------------------------------*/
/*  17) MlibSymMulgain()    : �Q�C����Z(�����Ώ̎l�̌ܓ�)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*  18) MlibSymMulgainNolim : �Q�C����Z(�����Ώ̎l�̌ܓ�)	rv=(kx*u)>>sx; 0<=sx<=24				*/
/*  19) MlibSymIntegral()   : �ϕ����Z(�����Ώ̎l�̌ܓ�)	rv=(iu[1]>>1);iu[]=iu[]+kx*u;			*/
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
/*		�t�B���^���Z�֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibLpfilter1()     : �P�����[�p�X�t�B���^			rv=x+((kx*(u-x))>>24);					*/
/*   1-2) FlibLpfilter1()     : �P�����[�p�X�t�B���^			rv=x+((kx*(u-x))>>24);					*//*<S00A>*/
/*   2) MlibHpfilter1()     : �P���n�C�p�X�t�B���^			rv=u-z; z=z+((kx*(u-z))>>24);			*/
/*   3) MlibLpfilter2()     : �Q�����[�p�X�t�B���^			rv=(z[2]>>1);							*/
/*   4) MlibNxfilter2()     : �Q���m�b�`�t�B���^			rv=k[2]*(u-(z[2]>>1)-z[0])+(z[2]>>1);	*/
/*   5) MlibLaufilter()     : ����������(LAU)�t�B���^		rv= z + dz or rv= z - dz;				*/
/*   6) MlibBITFILTER1()    : �P��x��r�b�g�t�B���^		out = f( out, oldin, newin )			*/
/*   7) MlibKrLpfilter1()   : �P�����[�p�X�t�B���^(�䗦�t)	rv=x+((kr*(u-x))>>24);					*/
/*   8) MlibLowPassfilter2(): �Q�����[�p�X�t�B���^���Z(�o�ꎟ�ϊ�)									*/
/*   8-2) FlibLowPassfilter2(): �Q�����[�p�X�t�B���^���Z(�o�ꎟ�ϊ�)									*//*<S00A>*/
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
float	FlibLowPassfilter2( float u, float k[4], float z[4] );	/* �Q�����[�p�X�t�B���^���Z(�o�ꎟ�ϊ�)	*/
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		�P��x��r�b�g�t�B���^�}�N��																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MlibBITFILTER1( out, oldin, newin )\
		{ out = (((oldin | newin) & out ) | (oldin & newin)); oldin = newin;}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�ʒu�֘A���Z�֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibPcmdIpfil()     : �ʒu�w�ߕ�ԃt�B���^			rv=(pcmdin+rem)/n						*/
/*   2) MlibPcmdMafil()     : �ʒu�w�߈ړ����σt�B���^		rv=sum/n, sum=sum+pcmdin-pmafbuf[idx]	*/
/*   3) MlibPcmdMafilSec()  : �ʒu�w�߈ړ����σt�B���^2		rv=sum/n, sum=sum+pcmdin-pmafbuf[idx]	*//*<S0C3>*/
/*   4) MlibPcmdImafil()    : �ʒu��Ԉړ����σt�B���^		rv=sum/n, sum=sum+newpcmd/n-oldpcmd/n	*/
/*   5) MlibPcmdImafilSec() : �ʒu��Ԉړ����σt�B���^2		rv=sum/n, sum=sum+newpcmd/n-oldpcmd/n	*//*<S0C3>*/
/*   6) MlibPcmdExpfil()    : �ʒu�w�ߎw���������t�B���^	rv=sum*kexp, sum=sum+pcmdin-rv			*/
/*   7) MlibPcmdLpfil1()    : �ʒu�w�߂P�����[�p�X�t�B���^	rv=z*kf, z=z+pcmdin-rv					*/
/*   8) MlibPcmdLpfil2()    : �ʒu�w�߂Q�����[�p�X�t�B���^	rv=z[1]*kf1, z[0]=z[0]+pcmdin-rv		*/
/*   8-2) FlibPcmdLpfil2()  : �ʒu�w�߂Q�����[�p�X�t�B���^	rv=z[1]*kf1, z[0]=z[0]+pcmdin-rv		*//*<S00A>*/
/*   9) MlibPcmdNxfil2()    : �ʒu�w�߂Q���m�b�`�t�B���^	rv=pcmdin - Delta(kf2*z[1])				*/
/*  10) MlibPcmdEgear()     : �ʒu�w�ߓd�q�M���v�Z			rv=(B/A)*pcmda							*/
/*  10-2) FlibPcmdEgear()   : �ʒu�w�ߓd�q�M���v�Z			rv=(B/A)*pcmda							*//*<S00A>*/
/*  11) MlibEgearRvscnv()   : �d�q�M���t�ϊ��v�Z			rv=(A/B)*inpls							*/
/*  12) MlibPcalKegear()    : �d�q�M���p�����[�^�v�Z 		OUT: egear.k1, egear.k2, etc			*/
/*  13) MlibPerrcala()      : �ʒu�΍��v�Z�`(�w�ߒP��)		OUT: perra.per32s, perra.per32a, etc	*/
/*  13-2) FlibPerrcala()    : �ʒu�΍��v�Z�`(�w�ߒP��)		OUT: perra.per32s, perra.per32a, etc	*//*<S00A>*/
/*  14) MlibPerrcalx()      : �ʒu�΍��v�Z�w(�e�a�P��)		rv=per64[0]; per64[]=per64[]+pcmd-pfbk;	*/
/*  14-2) FlibPerrcalx()    : �ʒu�΍��v�Z�w(�e�a�P��)		rv=per64[0]; per64[]=per64[]+pcmd-pfbk;	*//*<S00A>*/
/*  15) MlibAposRg64iv()    : ������Έʒu�v�Z(�t�d�q�M��)	OUT: aposrg.apos[],aposrem,etc			*/
/*	16) MlibAposRg64dp()    : ��Έʒu�X�V�v�Z(�t�d�q�M��)	OUT: aposrg.apos[],aposrem,etc			*/
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
/*		�ʒu�w�ߕ�ԃt�B���^																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* �ʒu�w�ߕ�ԃt�B���^�\���̒�`									*/
		LONG	cmdx;			/* �ʒu�w�ߓ���														*/
		LONG	remx;			/* �ʒu�w�ߏo�͗]��													*/
} PIPFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdIpfil( LONG pcmdin, LONG shift, LONG index, PIPFV *pipfvar );
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߈ړ����σt�B���^																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* �ʒu�w�߈ړ����σt�B���^�\���̒�`								*/
		SHORT	idx;			/* �o�b�t�@�C���f�N�X												*/
		SHORT	ksub;			/* ���Z�ʉ��Z�Q�C��(0/1)											*/
		LONG	sumx;			/* �ʒu�w�ߍ��v														*/
		LONG	remx;			/* �ʒu�w�ߗ]��														*/
		USHORT	zcntx;			/* ���o�������`�F�b�N(0:����)										*/
		USHORT	spare;			/* �\��																*/
} PMAFV;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߈ړ����σt�B���^2																	        */
/*--------------------------------------------------------------------------------------------------*//*<S0C3>*/
typedef	struct	{				/* �ʒu�w�߈ړ����σt�B���^�\���̒�`*/
		SHORT	idxSec;			/* �o�b�t�@�C���f�N�X												*/
		SHORT	ksubSec;		/* ���Z�ʉ��Z�Q�C��(0/1)											*/
		LONG	sumxSec;		/* �ʒu�w�ߍ��v														*/
		LONG	remxSec;		/* �ʒu�w�ߗ]��														*/
		USHORT	zcntxSec;		/* ���o�������`�F�b�N(0:����)										*/
		USHORT	spareSec;		/* �\��		                                                        */
} HIGHFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdMafil( LONG pcmdin, LONG pmafnum, PMAFV *pmafvar, LONG *pmafbuf );
LONG	MlibPcmdMafilSec( LONG pcmdin, LONG pmafnum, HIGHFV *pmafvarSec, LONG *pmafbufSec );   /*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߕ�Ԉړ����σt�B���^																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* �ʒu�w�ߕ�Ԉړ����σt�B���^�\���̒�`							*/
		LONG	sumx;			/* �ʒu�w�ߍ��v														*/
		LONG	remx;			/* �ʒu�w�ߗ]��														*/
		LONG	newpcmd;		/* �ʒu�w�ߓ���														*/
		LONG	oldpcmd;		/* �ʒu�w�ߓ��͑O��l												*/
		USHORT	zcntx;			/* ���o�������`�F�b�N(0:����)										*/
		USHORT	spare;			/* �\��																*/
} PIMFV;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߕ�Ԉړ����σt�B���^2																*/
/*--------------------------------------------------------------------------------------------------*/ /*<S0C3>*/
typedef	struct	{				/* High filter��Ԉړ����σt�B���^�\���̒�`	                    */
		LONG	sumxSec;		/* �ʒu�w�ߍ��v														*/
		LONG	remxSec;		/* �ʒu�w�ߗ]��														*/
		LONG	newpcmdSec;		/* �ʒu�w�ߓ���														*/
		LONG	oldpcmdSec;		/* �ʒu�w�ߓ��͑O��l												*/
		USHORT	zcntxSec;		/* ���o�������`�F�b�N(0:����)										*/
		USHORT	spareSec;		/* �\��		                                                        */
} HIMFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdImafil( LONG pcmdin, LONG shift, LONG index, PIMFV *pimfvar );
LONG	MlibPcmdImafilSec( LONG pcmdin, LONG shift, LONG indexSec, HIMFV *pimfvarSec );   /*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߎw���������t�B���^																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* �ʒu�w�ߎw���������t�B���^�\���̒�`								*/
		LONG	sumx;			/* �ʒu�w�ߍ��v														*/
		LONG	remx;			/* �ʒu�w�ߗ]��(���g�p)												*/
} PEXFV;
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdExpfil( LONG pcmdin, LONG kexp, LONG pbias, PEXFV *pexfvar );
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃��[�p�X�t�B���^																	*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdLpfil1( LONG pcmdin, LONG kf, LONG *z );
LONG	MlibPcmdLpfil2( LONG pcmdin, LONG kf[2], LONG z[2] );
#if (FLOAT_USE==TRUE)
float	FlibPcmdLpfil2( float pcmdin, float kf[2], float z[2] );
#endif /* FLOAT_USE */
LONG	MlibPcmdNxfil2( LONG pcmdin, LONG kf[3], LONG z[3] );
/*--------------------------------------------------------------------------------------------------*/
/*		�d�q�M�����ʒu�΍��\����																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* �d�q�M���\���̒�`												*/
		LONG	a;				/* �d�q�M���`														*/
		LONG	b;				/* �d�q�M���a														*/
		LONG	k1;				/* �d�q�M���Q�C���Fb/a�̏��bit										*/
		LONG	k2;				/* �d�q�M���Q�C���Fb/a�̉���bit										*/
		LONG	g1;				/* �d�q�M���Q�C���Fa/b�̏��bit										*/
		LONG	g2;				/* �d�q�M���Q�C���Fa/b�̉���bit										*/
#if (FLOAT_USE==TRUE)
		float		f_a;		/* �d�q�M���`														*/
		float		f_b;		/* �d�q�M���a														*/
		float		f_k1;		/* �d�q�M���Q�C��(���ϊ�)�Fb/a										*/
		float		f_k2;		/* �d�q�M���Q�C��(���ϊ�)�Fb/a�]��									*/
		float		f_g1;		/* �d�q�M���Q�C��(�t�ϊ�)�Fa/b										*/
		float		f_g2;		/* �d�q�M���Q�C��(�t�ϊ�)�Fa/b�]��									*/
#endif /* FLOAT_USE */
} EGEAR;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{				/* �ʒu�΍��`�\���̒�`												*/
		LONG	pfbkb;			/* �ʒu�e�a(�w�ߒP��)���Z����										*/
		LONG	pfbkrem;		/* �ʒu�e�a(�w�ߒP��)���Z���ʗ]��									*/
		LONG	per64[2];		/* �ʒu�΍�64bit���Z�p												*/
		LONG	per32s;			/* �ʒu�΍��o��(�l�̌ܓ�)											*/
		LONG	per32a;			/* �����Βl(�l�̌ܓ�)												*/
		LONG	per32sx;		/* �ʒu�΍��o��(�؂�グ)											*/
		LONG	per32ax;		/* �����Βl(�؂�グ)												*/
		LONG	per32mx;		/* �ʒu�΍����j�^�p(�؂�グ�^0.5�ȉ��Ń[��)						*/
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
void	FlibPerrcala( LONG pcmda, LONG pfbka, EGEAR *egear, PERRA *perra );	/* �����u	*/
LONG	FlibPerrcalx( LONG pcmdx, LONG pfbkx, LONG per64[2] );	/* �����u	*/
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		��Έʒu���Z(�t�d�q�M���F�e�a�P��-->�w�ߒP��)												*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{
		LONG	dposix;				/* �ʒu����				[�w�ߒP��]								*/
		LONG	aposrem;			/* �ʒu�]��				[�w�ߒP��]								*/
		LONG	apos[2];			/* ��Έʒu(64bit)		[�w�ߒP��]								*/
} APOSRG;
/*--------------------------------------------------------------------------------------------------*/
void	MlibAposRg64iv( LONG inipos0, LONG inipos1, EGEAR *egear, APOSRG *aposrg );
void	MlibAposRg64dp( LONG dposin, EGEAR *egear, APOSRG *aposrg, LONG *latchpos );
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�ߍ쐬��֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibPcmdMaker()     : �ʒu�w�ߍ쐬��(64bit)			rv=vp, vp=vp+vpacc/vp=vp-vpdec			*/
/*   2) MlibIpTposLimit()   : �����Ԉʒu�w�߃��~�b�g		rv=TRUE(Limit)/FALSE(NotLimit)			*/
/*   3) MlibRstPcmdMaker()  : ���ナ�Z�b�g����				pcmdout[]=pcmdset0,pcmdset1,etc			*/
/*   4) MlibIpcalPcmdMaker  : ���㏉���p�����[�^�v�Z		OUT: pcmkprm.osvpm, maxvpm, pshlx, etc	*/
/*   5) MlibPcalaPcmdMaker  : ����ʒu���߃p�����[�^�v�Z�`	OUT: pcmkprm.vpacc, vpdec,  vpamx, etc	*/
/*   6) MlibPcalaPcmdMkrIP  : �����ԉ��Z�p�����[�^�v�Z�`	OUT: pcmkprm.ipmaxspd, ipmaxacc,   etc	*/
/*   7) MlibPcalbPcmdMaker  : ����ʒu���߃p�����[�^�v�Z�a	OUT: pcmkprm.vpacc, vpdec,  vpamx, etc	*/
/*   8) MlibPcalbPcmdMkrIP  : �����ԉ��Z�p�����[�^�v�Z�a	OUT: pcmkprm.ipmaxspd, ipmaxacc,   etc	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߍ쐬��																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{					/* �ʒu�쐬��p�����[�^�\���̒�`								*/
		LONG	osvpm;				/* �n�r���[�^�ʒu�w�ߑ��x				[xpulse/scan]			*/
		LONG	maxvpm;				/* �ő僂�[�^�ʒu�w�ߑ��x				[xpulse/scan]			*/
		LONG	maxipv;				/* ��ԃ��[�h�ő�ʒu�w�ߑ��x			[xpulse/scan]			*/
		UCHAR	pshlx;				/* �ʒu�w�߉��Z�{��(�V�t�g)				[-]						*/
		UCHAR	pcmd64f;			/* �ʒu�w�߂U�S�r�b�g�t���O				[TRUE/FALSE]			*/
		USHORT	iptimes;			/* Interpolate��ԉ�					[-]						*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	vpacc;				/* �ʒu�w�߉����x						[xpulse/scan/scan]		*/
		LONG	vpdec;				/* �ʒu�w�ߌ����x						[xpulse/scan/scan]		*/
		LONG	vpamx;				/* ��(vpacc/vpdec)						[-]						*/
		LONG	vparx;				/* �]��(vpacc%vpdec)					[-]						*/
		LONG	vpapx;				/* ������������~�p���X��				[xpulse]				*/
		LONG	maxspd;				/* �ő�ʒu�w�ߑ��x						[2^24/OvrSpd]			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	ipmaxspd;			/* ��ԃ��[�h�ő呬�x					[ipulse/IPcycle]		*/
		LONG	ipmaxacc;			/* ��ԃ��[�h�ő�����x(���g�p)			[ipulse/IPcycle^2]		*/
		LONG	ipstpdec;			/* ��ԃ��[�h����~�����x				[ipulse/IPcycle^2]		*/
} PCMKP;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{					/* �ʒu�쐬�퉉�Z�p�ϐ��\���̒�`								*/
		UCHAR	calendf;			/* ���Z�I���t���O (���Z��:0, ���Z�I��:1)						*/
		UCHAR	cmderrf;			/* �w�߈ُ�t���O (��ԃ��[�h�ʒu�w�߈ُ�,etc)					*/
		UCHAR	pcmkmode;			/* �ʒu�w�ߍ쐬���[�h											*/
		UCHAR	modechgf;			/* �ʒu�w�ߍ쐬���[�h�ω��t���O									*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	vpx;				/* �ʒu�w�ߑ��x							[xpulse/scan]			*/
		LONG	vpxrem;				/* �ʒu�w�ߑ��x�o�͗]��					[xpulse/scan]			*/
		LONG	pcmdout[2];			/* �ʒu�w�ߏo�͐�Βl					[ipulse]				*/
		LONG	avp;				/* Work:�ʒu�w�ߑ��x��Βl				[xpulse/scan]			*/
		LONG	n;					/* Work:��(avp/vpdec)					[-]						*/
		LONG	rem;				/* Work:�]��(avp%vpdec)					[-]						*/
		LONG	maxvp;				/* Work:�ő�ʒu�w�ߑ��x				[xpulse/scan]			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	ipvpi;				/* Interpolate�ʒu�w�ߑ��x				[ipulse/IPcycle]		*/
		LONG	ipvpx;				/* Interpolate�ʒu�w�ߑ��x				[ipulse/scan]			*/
		LONG	ipremi;				/* Interpolate�ʒu�w�ߑ��x�]��			[ipulse/scan]			*/
		LONG	ipremx;				/* Interpolate�ʒu�w�ߑ��x�]��			[ipulse/scan]			*/
		LONG	ipstopx[2];			/* IpWork:������~�ʒu�v�Z�l			[ipulse]				*/
} PCMKV;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{					/* �ʒu�쐬�퉉�Z�p�ϐ�(P&V)�\���̒�`							*/
		PCMKP	P;					/* �p�����[�^													*/
		PCMKV	V;					/* ���Z�p�ϐ�													*/
} PCMKPV;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߍ쐬�퉉�Z���s�֐�																	*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcmdMaker(				/* �ʒu�w�ߍ쐬�퉉�Z���s�֐�									*/
		LONG	pcmdin0,			/* �ʒu�w�ߓ���(����32bit)				[pulse]					*/
		LONG	pcmdin1,			/* �ʒu�w�ߓ���(���32bit)				[pulse]					*/
		LONG	pcmdspd,			/* �ʒu�w�ߑ��x(�ő�^����)				[2^24/OvrSpd]			*/
		PCMKPV	*pcmdmkr,			/* �ʒu�w�ߍ쐬�퉉�Z�p�ϐ�(P&V)		[-]						*/
		ULONG	pcmkmode	);		/* �ʒu�w�ߍ쐬���[�h					[-]						*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߍ쐬���[�h��`�F��{���[�h�́A���ʃo�C�g�Œ�`���邱��								*/
/*--------------------------------------------------------------------------------------------------*/
#define	PCMKMODE_NONE   0x0000		/* �ʒu�w�ߍ쐬���[�h : ����									*/
#define	PCMKMODE_IPA	0x0001		/* �ʒu�w�ߍ쐬���[�h : ���(���͍X�V�L��)						*/
#define	PCMKMODE_IPB	0x0101		/* �ʒu�w�ߍ쐬���[�h : ���(���͍X�V����)						*/
#define	PCMKMODE_POS	0x0002		/* �ʒu�w�ߍ쐬���[�h : �ʒu����								*/
#define	PCMKMODE_FEED	0x0003		/* �ʒu�w�ߍ쐬���[�h : �葬����								*/
#define	PCMKMODE_STOP	0x0004		/* �ʒu�w�ߍ쐬���[�h : ������~								*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibIpTposLimit(			/* ��Ԉʒu�w�߃��~�b�g���Z										*/
		LONG	*tpos,				/* �ڕW�ʒu(64bit)						[pulse]					*/
		LONG	*lmtpos,			/* ���~�b�g�ʒu(64bit)					[pulse]					*/
		LONG	lmtdir,				/* ���~�b�g����(�����^����)				[-]						*/
		PCMKPV	*pcmdmkr	);		/* �ʒu�w�ߍ쐬�퉉�Z�p�ϐ�(P&V)		[-]						*/
/*--------------------------------------------------------------------------------------------------*/
#define	IPTPOSLMT_PSIDE		1		/* ���~�b�g�����F����											*/
#define	IPTPOSLMT_NSIDE		0		/* ���~�b�g�����F����											*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibRstPcmdMaker(			/* �ʒu�w�ߍ쐬�탊�Z�b�g����									*/
		LONG	pcmdset0,			/* �ʒu�w�ߏ����l(����32bit)			[pulse]					*/
		LONG	pcmdset1,			/* �ʒu�w�ߏ����l(���32bit)			[pulse]					*/
		PCMKPV	*pcmdmkr	);		/* �ʒu�w�ߍ쐬�퉉�Z�p�ϐ�(P&V)		[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibIpcalPcmdMaker(			/* �ʒu�w�ߍ쐬�평���p�����[�^�v�Z								*/
		KSGAIN	ksosvp,				/* �n�r�p���X���x						[xpulse/scan]			*/
		LONG	maxspdm,			/* �ő僂�[�^���x						[2^24/OvrSpd]			*/
		LONG	maxspdi,			/* ��ԃ��[�h�ő呬�x					[2^24/OvrSpd]			*/
		LONG	pcmd64f,			/* �ʒu�w�߂U�S�r�b�g�t���O				[TRUE/FALSE]			*/
		PCMKP	*pcmkprm	);		/* �v�Z���ʏo�͍\���̃|�C���^			[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalaPcmdMaker(			/* �ʒu�w�ߍ쐬��ʒu���߃p�����[�^�v�Z�`						*/
		LONG	maxspd,				/* �ő呬�x								[2^24/OvrSpd]			*/
		LONG	acctime,			/* ��������								[xs]					*/
		LONG	dectime,			/* ��������								[xs]					*/
		LONG	scantime,			/* �X�L�����^�C��						[xs]					*/
		PCMKP	*pcmkprm	);		/* �v�Z���ʏo�͍\���̃|�C���^			[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalaPcmdMkrIP(			/* �ʒu�w�ߍ쐬���ԉ��Z�p�����[�^�v�Z�`						*/
		LONG	iptimes,			/* ��ԉ�								[-]						*/
		LONG	maxspd,				/* �ő呬�x								[2^24/OvrSpd]			*/
		LONG	ipacctm,			/* �ŏ�����������(���g�p)				[xs]					*/
		LONG	ipdectm,			/* ����~��������						[xs]					*/
		LONG	scantime,			/* �X�L�����^�C��						[xs]					*/
		PCMKP	*pcmkprm	);		/* �v�Z���ʏo�͍\���̃|�C���^			[-]						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalbPcmdMaker(			/* �ʒu�w�ߍ쐬��ʒu���߃p�����[�^�v�Z�a						*/
		LONG	accrate,			/* �������[�g							[10000pulse/s/s]		*/
		LONG	decrate,			/* �������[�g							[10000pulse/s/s]		*/
		LONG	scantime,			/* �X�L�����^�C��(stm)					[us(Y=0)/ns(Y=1)]		*/
		PCMKP	*pcmkprm,			/* �v�Z���ʏo�͍\���̃|�C���^			[-]						*/
		LONG	insel		);		/* ���͑I��(0xYX)						[X:�s�g�p, Y:us/ns�I��]	*/
/*--------------------------------------------------------------------------------------------------*/
LONG	MlibPcalbPcmdMkrIP(			/* �ʒu�w�ߍ쐬���ԉ��Z�p�����[�^�v�Z�a						*/
		LONG	iptimes,			/* ��ԉ�								[-]						*/
		LONG	ipaccrt,			/* �ő���������[�g(���g�p)				[10000pulse/s/s]		*/
		LONG	ipdecrt,			/* ����~�������[�g					[10000pulse/s/s]		*/
		LONG	scantime,			/* �X�L�����^�C��(stm)					[us(Y=0)/ns(Y=1)]		*/
		PCMKP	*pcmkprm,			/* �v�Z���ʏo�͍\���̃|�C���^			[-]						*/
		LONG	insel		);		/* ���͑I��(0xYX)						[X:�s�g�p, Y:us/ns�I��]	*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�v�Z�p�֐�																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibPcalKxgain()    : ���Q�C���v�Z				{kx,sx} = (a*b/c)<<sx;					*/
/*   2) MlibPcalKskxkx()    : �g���Q�C���v�Z				{kx,sx} = {ka,sa}*b/c<<sx;				*/
/*   3) MlibPcalKxkskx()    : �g���Q�C���v�Z				{kx,sx} = a*{kb,sb}/c<<sx;				*/
/*   4) MlibPcalKxkxks()    : �g���Q�C���v�Z				{kx,sx} = a*b/{kc,sc}<<sx;				*/
/*   5) MlibPcalKskskx()    : �g���Q�C���v�Z				{kx,sx} = {ka,sa}*{kb,sb}/c<<sx;		*/
/*   6) MlibPcalKxksks()    : �g���Q�C���v�Z				{kx,sx} = a*{kb,sb}/{kc,sc}<<sx;		*/
/*   7) MlibPcalKsksks()    : �g���Q�C���v�Z				{kx,sx} = {ka,sa}*{kb,sb}/{kc,sc}<<sx;	*/
/*--------------------------------------------------------------------------------------------------*/
/*   8) MlibScalKxgain()    : ���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = (a*b/c)<<sx; (�w���������s�v)	*/
/*   9) MlibScalKskxkx()    : �g���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = {ka,sa}*b/c<<sx;				*/
/*  10) MlibScalKxkskx()    : �g���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = a*{kb,sb}/c<<sx;				*/
/*  11) MlibScalKxkxks()    : �g���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = a*b/{kc,sc}<<sx;				*/
/*  12) MlibScalKskskx()    : �g���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = {ka,sa}*{kb,sb}/c<<sx;		*/
/*  13) MlibScalKxksks()    : �g���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = a*{kb,sb}/{kc,sc}<<sx;		*/
/*  14) MlibScalKsksks()    : �g���Q�C���v�Z(�v�Z�J�n�p)	{kx,sx} = {ka,sa}*{kb,sb}/{kc,sc}<<sx;	*/
/*--------------------------------------------------------------------------------------------------*/
/*  15) MlibPcalKxaddx()    : ���Q�C�����Z				{kx,sx} = {ka,sa}+{kb,sb};				*/
/*  16) MlibPcalKxsubx()    : ���Q�C�����Z				{kx,sx} = {ka,sa}-{kb,sb};				*/
/*  17) MlibPcalKxmulx()    : ���Q�C����Z				{kx,sx} = (a*b*c)<<sx;					*/
/*  18) MlibPcalKxdivx()    : ���Q�C�����Z				{kx,sx} = {ka,sa}/{kb,sb};				*/
/*--------------------------------------------------------------------------------------------------*/
/*  19) MlibPcalKf1gain()   : �P���t�B���^�Q�C���v�Z		{kf,24} = (ts/(tx+ts))<<24;				*/
/*  19-2) FlibPcalKf1gain()   : �P���t�B���^�Q�C���v�Z		{kf} = (ts/(tx+ts));					*//*<S00A>*/
/*  20) MlibPcalKf2gain()   : �Q���t�B���^�Q�C���v�Z		OUT: kf[0], kf[1]						*/
/*  20-2) FlibPcalKf2gain()   : �Q���t�B���^�Q�C���v�Z		OUT: kf[0], kf[1]						*//*<S00A>*/
/*  21) MlibPcalKnf2gain()  : �Q���m�b�`�t�B���^�Q�C���v�Z	OUT: kf[0], kf[1], kf[2]				*/
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
LONG	MlibPcalKf1gain(			/* �P���t�B���^�Q�C���v�Z										*/
		LONG	tx,					/* ���萔(Tx)/���g��(Fx)	[xs],[0.1Hz] (X=0:Tx, X=1:Fx)		*/
		LONG	ts,					/* �X�L�����^�C��			[xs],[us/ns] (Y=0:us, Y=1:ns)		*/
		LONG	insel	);			/* ���͑I��(0xYX)			[X:Tx/Fx�I��, Y:us/ns�I��]			*/

#if (FLOAT_USE==TRUE)
float	FlibPcalKf1gain(		/* �P���t�B���^�Q�C���v�Z										*/
		LONG	tx,				/* ���萔(Tx)/���g��(Fx)	[sec],[Hz] (X=0:Tx, X=1:Fx)			*/
		LONG	ts,				/* ���Z����					[sec] 								*/
		LONG	insel	);			/* ���͎w��(0x0X)			[X:Tx/Fx�I��]						*/
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
void	MlibPcalKf2gain(			/* �Q���t�B���^�Q�C���v�Z										*/
		LONG	hz,					/* �t�B���^���g��			[0.1Hz]								*/
		LONG	dx,					/* �t�B���^�����W��			[0.001]								*/
		LONG	ts,					/* �X�L�����^�C��			[us/ns]	(Y=0:us, Y=1:ns)			*/
		LONG	kf[2],				/* �v�Z���ʊi�[�|�C���^		[--]								*/
		LONG	insel	);			/* ���͑I��(0xYX) 			[X:�s�g�p, Y:us/ns�I��]				*/
#if (FLOAT_USE==TRUE)
void	FlibPcalKf2gain(			/* �Q���t�B���^�Q�C���v�Z										*/
		float	hz,				/* �t�B���^���g��			[Hz]								*/
		float	dx,				/* �t�B���^�����W��(��)		[-]									*/
		float	ts,				/* �X�L�����^�C��			[sec]								*/
		float	kf[2]);			/* �v�Z���ʊi�[�|�C���^		[--]								*/
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
void	MlibPcalKnf2gain(			/* �Q���m�b�`�t�B���^�Q�C���v�Z									*/
		LONG	hz,					/* �t�B���^���g��			[0.1Hz]								*/
		LONG	qx,					/* �p�萔(Qx)/�����W��(Dx)	[0.001]	(X=0:Qx, X=1:Dx)			*/
		LONG	kn,					/* �m�b�`�[���Q�C��			[0.001]								*/
		LONG	ts,					/* �X�L�����^�C��			[us/ns] (Y=0:us, Y=1:ns)			*/
		LONG	kf[3],				/* �v�Z���ʊi�[�|�C���^		[--]								*/
		LONG	insel	);			/* ���͑I��(0xYX)			[X:Qx/Dx�I��, Y:us/ns�I��]			*/
// <V069> Change Start
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�C������}�N��																				*/
/*--------------------------------------------------------------------------------------------------*/
/*			Little Endian �p																			*/
#define	MlibGAINKX( k )		((LONG)((k)<<8)>>8)
#define	MlibGAINSX( k )		(((CHAR*)&(k))[3])
/*--------------------------------------------------------------------------------------------------*/
/*			�؂�̂� �p																				*/
#define	MlibGAINRD( k )		(MlibGAINKX( k ) >> ((LONG) ((ULONG) (k)>>24)))
/*--------------------------------------------------------------------------------------------------*/
// <V069> Change End



/****************************************************************************************************/
/*																									*/
/*		��{���l���Z�֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibAbs32()         : ��Βl(32bit)															*/
/*   2) MlibAdd6432()       : ���Z(�����t,64bit+32bit)												*/
/*   3) MlibAdd6464()       : ���Z(�����t,64bit+64bit)												*/
/*   4)	MlibSub6432()       : ���Z(�����t,64bit-32bit)												*/
/*   5)	MlibSub6464()       : ���Z(�����t,64bit-64bit)												*/
/*   6) MlibErr6464()       : �΍�(�����t,64bit-64bit,�߂�l�O�a�@�\�t��)							*/
/*   7) MlibSatAdd24()      : �O�a���Z(�����t,32bit+32bit,Limit:0x01000000,0xFF000000)				*/
/*   8) MlibSatAdd28()      : �O�a���Z(�����t,32bit+32bit,Limit:0x10000000,0xF0000000)				*/
/*   9) MlibSatAdd32()      : �O�a���Z(�����t,32bit+32bit,Limit:0x7FFFFFFF,0x80000000)				*/
/*  10) MlibSatAddu32()     : �O�a���Z(������,32bit+32bit,Limit:0xFFFFFFFF)							*/
/*  11) MlibMul3232()       : ��Z(�����t,32bit*32bit)												*/
/*  12) MlibMulu32u32()     : ��Z(������,32bit*32bit)												*/
/*  13) MlibMulhigh32()     : ��Z(�����t,32bit*32bit,�߂�l:���32bit,�l�̌ܓ�)					*/
/*  14) MlibMulhighu32()    : ��Z(������,32bit*32bit,�߂�l:���32bit,�l�̌ܓ�)					*/
/*  15) MlibDivhrem()       : ���Z(�����t,32bit/16bit,�o��:�����]��)								*/
/*  16) MlibDiv6432()       : ���Z(�����t,32bit*32bit/32bit,Limit:0x7FFFFFFF,�l�̌ܓ�)				*/
/*  17) MlibDivx3232()      : �g�����Z((32bit<<sx)/32bit,Limit:(qmax<<sx))							*/
/*  18) MlibSqrtu32()       : ���[�g���Z(����:32bit, �o��:16bit(Max.0xFFFF),�l�̌ܓ�)				*/
/*  19) MlibSqrtu64()       : ���[�g���Z(����:64bit, �o��:32bit(Max.0xFFFFFFFF),�l�̌ܓ�)			*/
/*  20) MlibSqrtu32u32()    : ���[�g���Z(����:32bit*32bit, �o��:32bit(Max.0xFFFFFFFF),�l�̌ܓ�)		*/
/*  21) MlibSins16()        : �r�������Z(����:[0.1deg],[360/16384deg],�o��:[1.0/10000],[1.0/16384])	*/
/*	22) MlibAbsErrchk32()   : ��Βl�΍��`�F�b�N(rv=(|a32-b32|<=chk32))								*/
/*	23) MlibAbsErrchk64()   : ��Βl�΍��`�F�b�N(rv=(|a64-b64|<=chk32))								*/
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
/*		���ꉉ�Z�����֐�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*   1) MlibSrhbiton()      : �T�[�`�r�b�g�n�m(LSB/MSB����T�[�`, �߂�l:BitNo/NG(-1))				*/
/*	 2) MlibSetCRC16()		: �b�q�b�P�U�̐ݒ�(MEMOBUS�p),		�߂�l:����							*/
/*	 3) MlibChkCRC16()		: �b�q�b�P�U�̃`�F�b�N(MEMOBUS�p),	�߂�l:TRUE(����)/FALSE(�ُ�)		*/
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
LONG	MlibCheckPowerOf2(							/* �Q�̗ݏ�`�F�b�N : rv = BitNo/NG(-1)			*/
			ULONG	x 			);					/* �`�F�b�N�f�[�^								*/
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		���������Z�b�g�֐�																			*/
/*																									*/
/****************************************************************************************************/
void	MlibResetByteMemory(						/* �o�C�g���������Z�b�g							*/
			void	*pRstMem,						/* ���Z�b�g�������擪�|�C���^					*/
			LONG	ByteNum		);					/* ���Z�b�g�������o�C�g��						*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibResetLongMemory(						/* �����O���������Z�b�g							*/
			void	*pRstMem,						/* ���Z�b�g�������擪�|�C���^					*/
			LONG	LwdNum		);					/* ���Z�b�g�����������O���[�h��					*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibCopyByteMemory(							/* �o�C�g�������R�s�[							*/
			void		*pDesMem,					/* �R�s�[��										*/
			const void	*pSrcMem,					/* �R�s�[��										*/
			INT			ByteNum	);					/* �R�s�[�o�C�g��								*/
/*--------------------------------------------------------------------------------------------------*/
void	MlibCopyLongMemory(							/* �����O�������R�s�[							*/
			void	*dstMem,						/* �R�s�[��										*/
			void	*srcMem,						/* �R�s�[��										*/
			INT		nbyte );						/* �R�s�[�o�C�g��								*/
/*--------------------------------------------------------------------------------------------------*/
void	memcpy( void*, void*, UINT );				/* �o�C�g�������R�s�[							*/

/****************************************************************************************************/
/*																									*/
/*		���̑�																			*/
/*																									*/
/****************************************************************************************************/
/*		�w���l�̌v�Z : 10^exp																		*/
LONG	MlibExpCal( CHAR exp );
#if (FLOAT_USE==TRUE)
void	FlibExpCal( CHAR exp, float *output );
#endif /* FLOAT_USE */
/*		KSGAIN �� LONG Data �ϊ�																	*/
LONG	MlibKsgain2Long( KSGAIN Input );

extern const	UCHAR	MlibAsciiTbl[ ];
extern const	UCHAR	MlibEvenAsciiTbl[ ]; 					/*<S02D>*/

/*--------------------------------------------------------------------------------------------------*/
/*		�P�O�i�ݏ�e�[�u��(Decimal Power Table)														*/
/*--------------------------------------------------------------------------------------------------*/
extern const ULONG MlibDecPowerTbl[];




#define MLIB_H_
#endif /* MLIB_H_ */
/***************************************** end of file **********************************************/


