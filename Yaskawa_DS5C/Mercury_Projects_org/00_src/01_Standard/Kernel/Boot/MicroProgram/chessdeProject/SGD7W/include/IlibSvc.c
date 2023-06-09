/****************************************************************************************************/
/*																									*/
/*																									*/
/*		IlibSvc.c : マイクロプログラムC言語化時ライブラリ											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2011.05.12  Y.tanaka		・ファイル作成											*/
/*																									*/
/****************************************************************************************************/
//#pragma  AsicName = JL-076
//#pragma  OutMpCodeName = YecAsicPrg
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
#include "Basedef.h"
#endif	//#ifdef WIN32			/* VCのみ有効(ASIPコンパイラ対応)			*/
/*--------------------------------------------------------------------------------------------------*/
#include "IxInst.h"
#ifndef WIN32
#include "MprgSgdv_chessde.h"				/* Include MicroProgram Source File					*/
#else
#include "MprgSgdv_VC.h"				/* Include MicroProgram Source File					*/
#endif




/****************************************************************************************************/
/*																									*/
/*		Function Prototype																			*/
/*																									*/
/****************************************************************************************************/
SHORT	IxLmtCBS16( SHORT src );			/* 符号付16ビットリミット演算								*/
LONG	IxLmtCBS32( LONG src );				/* 符号付32ビットリミット演算								*/
SHORT	IlibASR16( SHORT src , SHORT sft );	/* 算術シフト演算16bit									*/
LONG	IlibASR32( LONG src , SHORT sft);	/* 算術シフト演算32bit									*/
INT64	IlibASR64( INT64 src , SHORT sft);	/* 算術シフト演算64bit									*/


/****************************************************************************************************/
/*																									*/
/*		IxLmtCBS16																					*/
/*																									*/
/****************************************************************************************************/
SHORT	IxLmtCBS16( SHORT src )
{
	if( Flag.Carry )
	{
		src = 0x7FFF;
		Flag.Carry = 0;
	}
	else if( Flag.Borrow || src == 0x8000 )
	{
		src = 0x8001;
		Flag.Borrow = 0;
	}
	
	return (src);

}

/****************************************************************************************************/
/*																									*/
/*		IxLmtCBS32																					*/
/*																									*/
/****************************************************************************************************/
LONG	IxLmtCBS32( LONG src )
{
	if( Flag.Carry )
	{
		src = 0x7FFFFFFF;
		Flag.Carry = 0;
	}
	else if( Flag.Borrow || src == 0x80000000 )
	{
		src = 0x80000001;
		Flag.Borrow = 0;
	}

	return (src);

}

/****************************************************************************************************/
/*																									*/
/*		IlibASR16																					*/
/*																									*/
/****************************************************************************************************/
SHORT	IlibASR16( SHORT src , SHORT sft )	/* 算術シフト演算16bit									*/
{
SHORT dst;

/*--------------------------------------------------------------------------------------------------*/
/*		Shift																						*/
/*--------------------------------------------------------------------------------------------------*/
	if( sft == 0 )
	{
		dst = src;
	}
	else
	{
		if( src >= 0 )
		{
			dst = src >> (sft - 1);
			dst = ((dst + 1) >> 1);
		}
		else
		{
			src = -src;
			dst = src >> (sft - 1);
			dst = ((dst + 1) >> 1);
			dst = -dst;
		}
	}
	return(dst);
}

/****************************************************************************************************/
/*																									*/
/*		IlibASR32																					*/
/*																									*/
/****************************************************************************************************/
LONG	IlibASR32( LONG src , SHORT sft )	/* 算術シフト演算32bit									*/
{
LONG dst;

/*--------------------------------------------------------------------------------------------------*/
/*		Shift																						*/
/*--------------------------------------------------------------------------------------------------*/
	if( sft == 0 )
	{
		dst = src;
	}
	else
	{
		if( src >= 0 )
		{
			dst = src >> (sft - 1);
			dst = ((dst + 1) >> 1);
		}
		else
		{
			src = -src;
			dst = src >> (sft - 1);
			dst = ((dst + 1) >> 1);
			dst = -dst;
		}
	}
	if( dst > LPX_REG16_MAX )
	{
		Flag.Carry = 1;
	}
	else if ( dst < LPX_REG16_MIN )
	{
		Flag.Borrow = 1;
	}
	return(dst);
}

/****************************************************************************************************/
/*																									*/
/*		IlibASR64																					*/
/*																									*/
/****************************************************************************************************/
INT64	IlibASR64( INT64 src , SHORT sft )	/* 算術シフト演算64bit									*/
{
INT64 dst;

/*--------------------------------------------------------------------------------------------------*/
/*		Shift																						*/
/*--------------------------------------------------------------------------------------------------*/
	if( sft == 0 )
	{
		dst = src;
	}
	else
	{
		if( src >= 0 )
		{
			dst = src >> (sft - 1);
			dst = ((dst + 1) >> 1);
		}
		else
		{
			src = -src;
			dst = src >> (sft - 1);
			dst = ((dst + 1) >> 1);
			dst = -dst;
		}
	}
	if( dst > LPX_REG32_MAX )
	{
		Flag.Carry = 1;
	}
	else if ( dst < LPX_REG32_MIN )
	{
		Flag.Borrow = 1;
	}
	return(dst);
}



/******************************************* end of file ********************************************/
