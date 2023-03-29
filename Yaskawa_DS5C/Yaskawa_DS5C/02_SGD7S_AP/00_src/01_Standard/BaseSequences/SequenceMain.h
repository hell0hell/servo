/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SequenceMain.h : ScanCÉÅÉCÉìèàóùíËã`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	èâî≈	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef SEQUNECE_MAIN_H
#define SEQUNECE_MAIN_H



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void SyscInitServoSequence( AXIS_HANDLE *AxisRscC );
void SyscInputSeqStatus( AXIS_HANDLE *AxisRscC );
void SyscAlarmDetections( AXIS_HANDLE *AxisRscC );
void SyscSequenceMain( AXIS_HANDLE *AxisRscC );
void SyscBaseDriveSequence( AXIS_HANDLE *AxisRscC );
void SyscSequenceCommon( AXIS_HANDLE *AxisRscC );
void SyscSencAbsoOperation( AXIS_HANDLE *AxisRscC );  /*<S02D>*/


#endif /* SEQUNECE_MAIN_H */
/***************************************** end of file **********************************************/
