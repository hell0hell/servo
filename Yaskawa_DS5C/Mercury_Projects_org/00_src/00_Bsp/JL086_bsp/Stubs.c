/****************************************************************************************************/
/*																									*/
/*																									*/
/*		VC_Compile.h : VCによりコンパイルするためのスタブ定義										*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.04.20	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include <INTEGRITY.h>


void __EI( void )
{
	return;
}


USHORT __rev( USHORT x )
{
	USHORT rc;
	rc = (USHORT)( (((x) & 0xFF) << 8) + (((x) >> 8) & 0xFF) );
	return rc;
}


BOOL GetIntUDLStatus(UCHAR num)
{
	return TRUE;
}


VOID ReqIntUDLStatusClear(UCHAR num)
{
	return;
}


VOID	InitCorePeripheral( void )
{
	return;
}


VOID	SerialRomInit( void )
{
	return;
}


UCHAR	SFlashChkStatus( void )
{
	return	0;
}


VOID	SerialRomSectorErase( ULONG lAddr )
{
	return;
}


VOID	SerialRomPageProgram( ULONG lAddr, ULONG *pBuff, LONG nSize )
{
	return;
}


VOID	SerialRomReadDataBytes( ULONG lAddr, UCHAR *pBuff, LONG nSize )
{
	return;
}


VOID	UsbFuncInitializeDriver( ULONG MyAddress )
{
	return;
}


VOID	CommonCreateTask( LONG a, VOID *b, LONG c, CHAR *d, Task *e )
{
	return;
}


VOID	RunTask( Task a )
{
	return;
}


Error	RequestResource( Object *a, const CHAR *b, CHAR *c )
{
	return	(Error)Success;
}


VOID	Exit( LONG a )
{
	return;
}


VOID	sleep( LONG a )
{
	return;
}


VOID	usleep( LONG a )
{
	return;
}


Error	MapMemory( BOOL a, MemoryRegion b, MemoryRegion *c, VOID *d, VOID *e )
{
	return	(Error)Success;
}


Error	GetMemoryRegionAddresses( MemoryRegion a, VOID *b, VOID *c )
{
	return	(Error)Success;
}


Error	CreateSemaphore( LONG a, Semaphore *b )
{
	return	(Error)Success;
}


Error	WaitForSemaphore( Semaphore a )
{
	return	(Error)Success;
}


Error	TryToClearSemaphore( Semaphore a, VOID *b )
{
	return	(Error)Success;
}


Error	LinkSemaphore( Semaphore a, Semaphore *b )
{
	return	(Error)Success;
}


Error	ReadIODeviceRegister( IODevice a, LONG b, Value *c )
{
	return	(Error)Success;
}


Error	WriteIODeviceRegister( IODevice a, LONG b, Value c )
{
	return	(Error)Success;
}


Error	ClearIODeviceOverruns( IODevice a, Value *b )
{
	return	(Error)Success;
}


VOID	ReleaseSemaphore( Semaphore a )
{
	return;
}


BOOL	SynchronousReceive( Connection a, VOID *b )
{
	return	TRUE;
}


VOID	consolestring( CHAR *a )
{
	return;
}


VOID	RegisterResource( Object a, CHAR *b, CHAR *c )
{
	return;
}


SHORT	MP_ResetHINT( IODevice a )
{
	return (SHORT)TRUE;
}


SHORT	MP_EnableM3SSync( IODevice a )
{
	return (SHORT)TRUE;
}


SHORT	MP_DisableM3SSync( IODevice a )
{
	return (SHORT)TRUE;
}


SHORT	MP_SetMHINTTimer( IODevice a, USHORT b )
{
	return (SHORT)TRUE;
}



/***************************************** end of file **********************************************/
