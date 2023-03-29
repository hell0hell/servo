/****************************************************************************/
/*																			*/
/*																			*/
/*		Excel(PRM, Alarm,Warning)Table’è‹`ƒwƒbƒ_							*/
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
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		XLS_DEF_TABLE_
#define		XLS_DEF_TABLE_


#ifdef MCCSW_SGDVCMOP
#ifndef _WIN32
#include "XlsAlmDefCmOpt.h"
#endif
#include "XlsPrmSgdvCmOpt.h"
#define GetPRMPtr(axis)		(&OpPrm)
#endif

#ifndef MCCSW_SGDVCMOP
#ifndef _WIN32
#include "XlsAlmDefM2Opt.h"
#endif
#include "XlsPrmSgdvM2Opt.h"
#define GetPRMPtr(axis)		(&OpPrm)
#endif




#endif		/* XLS_DEF_TABLE_ */
