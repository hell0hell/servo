/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanBio.c : ScanN Common Input and Output procedure											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Input Data from Kernel																	*/
/*		2. Output Data to Kernel																	*/
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
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*		Rev.1.0l : 2013.05.30  K.Ozaki																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define ACCESSX_MONITOR
#include "ScanN.h"



/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
void	NpxInputMain( 											/* ScanN inuput main function 		*/
			SV_OPTION_IF *psvif,								/* SV-OPTION Interface				*/
			NINV *pninv, 										/* ScanN input valiables			*/
			struct _pos *pNpos );								/* ScanN position 					*/
void	NpxOutputMain( 											/* ScanN output main function		*/
			SV_OPTION_IF *psvif, 								/* SV-OPTION Interface				*/
			NOUTV *pnoutv );									/* ScanN output valiable			*/
void	NpxInitInOutProc( 										/* ScanN I/O init main function   	*/
			SV_OPTION_IF *psvif,								/* SV-OPTION Interface				*/
			NINV *pninv, 										/* ScanN input valiables			*/
			NOUTV *pnoutv );									/* ScanN output valiables			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Register Interface 																			*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxMakeMonitor( long idx, long rem, USHORT *RegBuf );  /* set indexer monitor */

/*--------------------------------------------------------------------------------------------------*/
/*		Local Functions 																			*/
/*--------------------------------------------------------------------------------------------------*/
long	LpxMakeStFlg( void *pVal );                            /* Make Status Flag                  */
long	LpxSNtoMS( void *pVal );                               /* ScanN Cycle to msec               */
long	LpxMakeInSig( void *pVal );                            /* Set Input Signal                  */
long	LpxMakeOutSig( void *pVal );                           /* Set Input Signal                  */
/*--------------------------------------------------------------------------------------------------*/
long	LpxMod( long inx, unsigned long modx );                /* Calculate Mod(inx, modx)          */

/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef struct _ixmon 
{
	void *pVal;	                       /* pointer to the monitor variable */
	long (*MonCalFunc)(void *pval);    /* pointer to the monitor calculation function */
} IXMON;

/****************************************************************************************************/
/*																									*/
/*		Enumurator Definition																		*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Const Data																					*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/* 		ScanN Input Main Function 																	*/
/*																									*/
/****************************************************************************************************/
void	NpxInputMain( 											/* ScanN inuput main function 		*/
			SV_OPTION_IF *psvif,								/* SV-OPTION Interface				*/
			NINV *pninv, 										/* ScanN input valiables			*/
			struct _pos *pNpos )								/* ScanN position 					*/
{
/*--------------------------------------------------------------------------------------------------*/
		MtcInputData( &MtHdl, psvif );								    /* Update Motion data 		*/
/*--------------------------------------------------------------------------------------------------*/
		pninv->Ctrl = psvif->CycRsp.RspCtrl;							/* Control response 		*/
		pninv->Seq = psvif->CycRsp.RspSeqBit;							/* Sequence response 		*/
		pninv->Lt = psvif->CycRsp.RspLtBit;								/* Latch response 			*/
		pninv->Hwsin = IXX_GET_HWSIN( psvif ) & 0x07F/*0x1FF*/;			/* SI0-SI6			 		*/
/*--------------------------------------------------------------------------------------------------*/
		pninv->NrmBits.BB = ( ! IXX_GET_SERVOONCOMP( pninv ) ) ? TRUE : FALSE;       /* base block  */
//		pninv->NrmBits.HOMEDONE = ( MtHdl.psnHdl.RefEnable ) ? TRUE : FALSE;         /* homing finished */
//		pninv->NrmBits.NEAR = ( PsnGetNearSts( &(MtHdl.psnHdl) ) ) ? TRUE : FALSE;   /* set near */
//		pninv->NrmBits.INPOS = ( PsnGetPsetSts( &(MtHdl.psnHdl) ) ) ? TRUE : FALSE;  /* set pset */
//		pninv->NrmBits.DEN = ( PsnGetDenSts( &(MtHdl.psnHdl) ) ) ? TRUE : FALSE;     /* set den */
//		pninv->NrmBits.PLS = (( MtHdl.psnHdl.SoftLimit ) & SOFT_LIMIT_P ) ?          /* set pls */
//									TRUE : FALSE;
//		pninv->NrmBits.NLS = (( MtHdl.psnHdl.SoftLimit ) & SOFT_LIMIT_N ) ?          /* set nls */
//									TRUE : FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		Get Monitors																				*/
/*--------------------------------------------------------------------------------------------------*/
		pninv->PcmdRingCntr = 	IXX_GET_HWPCMD( psvif ); 				/* @@ Anlg Pulse 			*/
		pninv->Ad1 =  			IXX_GET_HWAD1( psvif ); 				/* @@ Anlg Pulse 			*/
		pninv->Ad2 =  			IXX_GET_HWAD2( psvif ); 				/* @@ Anlg Pulse 			*/

/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア信号																			*/
/* 		※Sigma-Sとは処理が変わる!!																	*/
/*		※HwsInモニタのBit15をCLR信号とする。論理変換はASICの設定で実施(1:ON, 0:OFF)				*/
/*--------------------------------------------------------------------------------------------------*/
		if( pninv->P.PerClrLvlSel == 0x01 )			/* 信号のＨｉレベルで偏差クリア					*/
		{
			pninv->NrmBits.CLR = (( IXX_GET_CLR( psvif ) )? TRUE : FALSE);
		}
		else if( pninv->P.PerClrLvlSel == 0x02 )	/* 信号のＬｏレベルで偏差クリア					*/
		{
//			pninv->NrmBits.CLR = (( IXX_GET_CLR( psvif ) )? FALSE : TRUE);		/* <S00C>			*/
			pninv->NrmBits.CLR = (( IXX_GET_CLR( psvif ) )? TRUE: FALSE);		/* <S00C> 			*/
		}
		else										/* 偏差レベルクリア無し							*/
		{
			pninv->NrmBits.CLR = FALSE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		SEN Signal <S00C> <S168>																	*/
/*--------------------------------------------------------------------------------------------------*/
//<S168>		pninv->NrmBits.SEN = (( IXX_GET_SEN( psvif ) | (! (pninv->P.AbsUse)) ) ? TRUE : FALSE);
		pninv->NrmBits.SENI = (( IXX_GET_SEN( psvif )) ? TRUE : FALSE);

/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/* 		ScanN Output Main Function 																	*/
/*																									*/
/****************************************************************************************************/
void	NpxOutputMain( 											/* ScanN output main function		*/
			SV_OPTION_IF *psvif, 								/* SV-OPTION Interface				*/
			NOUTV *pnoutv )										/* ScanN output valiable			*/
{
/*--------------------------------------------------------------------------------------------------*/
/* 		Set cyclic command data 																	*/
/*--------------------------------------------------------------------------------------------------*/
		psvif->CycCmd.CmdCtrl = pnoutv->Ctrl;	     /* output control command */
		psvif->CycCmd.CmdSeqBit = pnoutv->Seq;		 /* output sequence command */
		psvif->CycCmd.CmdLtBit = pnoutv->Lt;		 /* output latch command */
		psvif->CycCmd.DOSignal = pnoutv->Sigs;       /* output signals */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
		MtcOutputData( &MtHdl, psvif );				 /* Update motion data							*/
/*--------------------------------------------------------------------------------------------------*/
/*      output tlim                                                                           		*/
/*--------------------------------------------------------------------------------------------------*/
		psvif->CycCmd.ExtCmd1.dwVal = AnlgV.Tlim;    /* analog torque limit */
		psvif->CycCmd.ExtCmd2.dwVal = AnlgV.Tlim;    /* analog torque limit */
/*--------------------------------------------------------------------------------------------------*/

		return;
}



/****************************************************************************************************/
/*																									*/
/* 		ScanN Input Main Function 																	*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	NpxInitInOutProc( 										/* ScanN I/O init main function   	*/
			SV_OPTION_IF *psvif,								/* SV-OPTION Interface				*/
			NINV *pninv, 										/* ScanN input valiables			*/
			NOUTV *pnoutv )										/* ScanN output valiables			*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		Initialize I/O valiable																		*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory ( pninv, sizeof(NinV)/4 );
		MlibResetLongMemory ( pnoutv, sizeof(NoutV)/4 );
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Initialize Servo I/F																		*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( psvif, sizeof(SV_OPTION_IF) );
		psvif->CycCtrl.CmdUpdateCycle = 1;
		psvif->CycCtrl.CmdUpdate = TRUE;
		psvif->CycCtrl.RspUpdate = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		Initialize Cyclic Data																		*/
/*--------------------------------------------------------------------------------------------------*/
		psvif->CycCmd.SelMon1 = MONSEL_LTPOS_C_L;		/* 0x30: Cphase Latch Position (not used) */
		psvif->CycCmd.SelMon2 = MONSEL_TRQREF;			/* 0x02: Torque Reference (not used) */
		psvif->CycCmd.SelMon3 = MONSEL_FBSPD;			/* 0x01: Feedback Speed */
		psvif->CycCmd.SelMon4 = MONSEL_ANALOG_PCNTR;	/* 0x3F: Command Pulse Counter */
		psvif->CycCmd.SelMon5 = MONSEL_INSGNL;			/* 0x3A: Input Signal Status */
		psvif->CycCmd.SelMon6 = MONSEL_ANALOG_AD1;		/* 0x3D: Anlg AD1 (VREF) */
		psvif->CycCmd.SelMon7 = MONSEL_ANALOG_AD2;		/* 0x3C: Anlg AD2 (TREF) */
		psvif->CycCmd.SelMon8 = MONSEL_PERCLRLATCHPOS;	/* 0x3E: Position Error Clear Latch Position */
/*--------------------------------------------------------------------------------------------------*/
/*		Set Extended Cyclic Command																	*/
/*--------------------------------------------------------------------------------------------------*/
		psvif->CycCtrl.ExtCmdSel1 = EXTCMD_TRQLMT_P;	/* Positive Torque Limit */
		psvif->CycCtrl.ExtCmdSel2 = EXTCMD_TRQLMT_N;	/* Negative Torque Limit */
		psvif->CycCtrl.ExtCmdSel3 = 0;					/* Not used */
		psvif->CycCtrl.ExtCmdSel4 = 0;					/* Not used */

/*--------------------------------------------------------------------------------------------------*/
		return;
}
#pragma arm section code

