/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Information.h : Servo Information Header File												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Object Data Type Definition																*/
/*																									*/
/*		2) Object Resource Macro Definition															*/
/*																									*/
/*		3) 																							*/
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
/*		Rev.1.00 : 2013.5.15  K.Ozaki																*/
/*		Rev.1.10 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___INFORMATION___
#define  ___INFORMATION___
#include "KnlApi.h"
#include "Object.h" 			

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
extern const PRMTBL InfoRegTable[];			/* Information Register Table */
extern const ULONG InfoRegTableEntNum;		/* Information Register Entry Number */
/*--------------------------------------------------------------------------------------------------*/
#define INFREGTBL_ENTNUM (InfoRegTableEntNum)
#define INFREGTBL_PTR    (InfoRegTable)
/*--------------------------------------------------------------------------------------------------*/
#define INFREG_NUMBERX (1)
extern COBJECT LInformation[];				/* Servo Information Access Class Instance 				*/
#define CINFREG (&LInformation[0])			/* Servo Information Access Class 						*/


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
void	RpxInitInfManager( void );			/* Operation Manager Initialize							*/
void	RpxPrstInfManager( void );			/* Operation Manager Parameter Reset					*/


#endif
/***************************************** end of file **********************************************/
