/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Operation.h : Operation Object Resource Management											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Operation Object Macro Definition														*/
/*																									*/
/*		2) Operation Object Resource Macro Definition												*/
/*																									*/
/*		3) Operation Object Function Definition														*/
/*																									*/
/*		4) 																							*/
/*																									*/
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
/*		Rev.1.00 : 2013.5.08  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___OPERATION___
#define  ___OPERATION___
#include "KnlApi.h"
#include "Object.h" 			

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
extern const PRMTBL OpeRegTable[];			/* Operation Register Table */
extern const ULONG OpeRegTableEntNum;		/* Operation Register Entry Number */
/*--------------------------------------------------------------------------------------------------*/
#define OPEREGTBL_ENTNUM (OpeRegTableEntNum)
#define OPEREGTBL_PTR    (OpeRegTable)
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_NUMBERX (1)
extern COBJECT LOperation[];				/* Servo Operation Access Instance 						*/
#define COPEREG (&LOperation[0])			/* Operation Register Access Class 						*/

/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitOpeManager( void );			/* Operation Manager Initialize							*/
void	RpxPrstOpeManager( void );			/* Operation Manager Parameter Reset					*/


#endif
/***************************************** end of file **********************************************/
