/****************************************************************************************************/
/*																									*/
/*																									*/
/*		InfMngr.c : Servo Information Management													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Read Object Data																			*/
/*																									*/
/*		2) Search Obeject 																			*/
/*																									*/
/*		3) Write Object Data																		*/
/*																									*/
/*		4) Check Object Access Level																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.20  K.Ozaki																*/
/*		Rev.1.10 : 2013.09.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Information.h"

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxInitInfManager( void );			/* Initialize Function 									*/
void	RpxPrstInfManager( void );			/* Parameter Reset Function 							*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Valiable Definition																			*/
/*																									*/
/****************************************************************************************************/
COBJECT LInformation[INFREG_NUMBERX];		/* Servo Information Access Instance 					*/
/*--------------------------------------------------------------------------------------------------*/






/****************************************************************************************************/
/*																									*/
/*		Initialize Function																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitInfManager( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Table Entry Number																		*/
/*--------------------------------------------------------------------------------------------------*/
		CINFREG->TblEntNum = INFREGTBL_ENTNUM;
		CINFREG->pTbl = (void *)INFREGTBL_PTR;
/*--------------------------------------------------------------------------------------------------*/
		CINFREG->Read = RpxObjRead;
//		CINFREG->ReadX = RpxObjReadIdx;
		CINFREG->ReadS = RpxObjReadS;					/* <S073> */
		CINFREG->Search = RpxObjSearch;
		CINFREG->SearchB = RpxObjSearchIdx;
		CINFREG->ReadArray = RpxObjReadArray;
		CINFREG->Write = RpxObjWrite;
		CINFREG->WriteS = RpxObjWriteS;					/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
		CINFREG->GetNumber = RpxObjGetNumber;			/* Get Object Number */
		CINFREG->isLong = RpxObjChkLong;				/* Check Object Data Size */
/*--------------------------------------------------------------------------------------------------*/
		CINFREG->SearchS = RpxObjSearchSub;				/* <S073> */
		CINFREG->SearchBS = RpxObjSearchIdxSub;			/* <S073> */
		CINFREG->WriteArray = RpxObjWriteArray;			/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Parameter Reset Function																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstInfManager( void )
{
		return;
}


/***************************************** end of file **********************************************/