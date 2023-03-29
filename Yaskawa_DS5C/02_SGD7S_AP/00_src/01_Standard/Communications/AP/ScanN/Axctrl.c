/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Axctrl.c : Indexer Motion Control															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		1. Motion Control		 																	*/
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
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"

/****************************************************************************************************/
/*																									*/
/*		Axctrl Function Prototypes																	*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlMain( MTHNDL *mthdl, struct _token *nmsg );  /* Indexer Motion Control Main		*/
void	IxxInitAxctrl( AXCTRLV *pax );			              /* Initialize Indexer Motion			*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqFollowUp( MT_CMD *mtcmd );                /* follow up request                  */
void	IxxAxctrlReqSkip( MT_CMD *mtcmd, MT_HOLD_MODE mode ); /* skip request                       */
void	IxxAxctrlReqHold( MT_CMD *mtcmd );                    /* hold request                       */
void	IxxAxctrlReqFeed( MT_CMD *mtcmd,                      /* Feed request                       */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec );                                   /* deceleration [ref/cycle^2]         */
void	IxxAxctrlReqHoming( MT_CMD *mtcmd, long tspeed, MT_LATCH_SGNL sgnl );  /* Homing request    */
void	IxxAxctrlReqZstop( MT_CMD *mtcmd );                   /* Zero stop request                  */
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqPosing( MT_CMD *mtcmd,                    /* Posing request                     */
				long tdst,                                    /* target distance [ref]              */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec );                                   /* deceleration [ref/cycle^2]         */
void	IxxAxctrlReqExPosing( MT_CMD *mtcmd,                  /* Ex Posing request                  */
				long tdst,                                    /* target distance [ref]              */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long rdst,                                    /* registration desitination [ref]    */
				long rspd,                                    /* registration speed [ref/s]         */
				MT_LATCH_SGNL sgnl );
void	IxxAxctrlReqExFeed( MT_CMD *mtcmd,                    /* Ex Feed request                    */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long rdst,                                    /* registration desitination [ref]    */
				long rspd,                                    /* registration speed [ref/s]         */
				MT_LATCH_SGNL sgnl );
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqAstop( MT_CMD *mtcmd,                     /* Astop request                      */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long tdst);                                   /* target desitination [ref]          */
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqResume( MT_CMD *mtcmd, long tspd );       /* Resume request                     */
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqInterpolate( MT_CMD *mtcmd,               /* @@ Interpolate request             */
				long tdst,                                    /* target distance [ref]              */
				long vff,                                     /* velocity feed forwared[2^24/OS]    */
				long tff );                                   /* torque feed forwared[2^24/MaxTrq]  */
void	IxxAxctrlReqVelctrl( MT_CMD *cmdData,                 /* @@ Velctrl request                 */
				long tvelo,                                   /* target velocity [2^24/OS]          */
				long tff );                                   /* torque feed forwared[2^24/MaxTrq]	*/

/****************************************************************************************************/
/*																									*/
/*		Indexer Motion Control Main 																*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlMain( MTHNDL *mthdl, struct _token *nmsg )
{
int den;
/*--------------------------------------------------------------------------------------------------*/
/*  Get request message                                                                             */
/*--------------------------------------------------------------------------------------------------*/
	if ( nmsg->Channel == CHANNEL_AXCTRL ) 
	{
		mthdl->cmdData = nmsg->fmt.CmdData;						/* get motion command data */
		nmsg->Channel = CHANNEL_SQI;							/* channel = SQI (念のため) */
	}
/*--------------------------------------------------------------------------------------------------*/
/*  Motion control main sequence                                                                    */
/*--------------------------------------------------------------------------------------------------*/
	mthdl->ctrlMode = mthdl->cmdData.CtrlMode;					/* set control mode */
	MtcCreateInCmd(mthdl);										/* create motion control command */
	den = MtgGenerate(&(mthdl->mtgHdl), &(mthdl->outData));		/* モーション生成メイン処理 */
/*--------------------------------------------------------------------------------------------------*/
	PsnSetIPOS(&(mthdl->psnHdl), mthdl->outData.DposOut);		/* set IPOS */
	PsnSetPOS(&(mthdl->psnHdl), mthdl->outData.DposOut);		/* set POS */
	PsnSetDenSts(&(mthdl->psnHdl), den);						/* set DEN */
/*--------------------------------------------------------------------------------------------------*/
//	NoutV.bInPos = (MciGetInPosStatus( mthdl )) ? TRUE : FALSE;  /* set INPOS (SQO用) */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Initialize Indexer Motion   																*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
void	IxxInitAxctrl( AXCTRLV *pax )
{
//KSGAIN  kx;
	MlibResetLongMemory( pax, sizeof(AXCTRLV)/4 );      /* Reset Memory 							*/
//	pndef_AccRate.FuncPtr( 0, 0, 0 );					/* TODO: Calculate default acceleration     */
//	pndef_DecRate.FuncPtr( 0, 0, 0 );					/* TODO: Calculate default deceleration     */
/*--------------------------------------------------------------------------------------------------*/
//	pndef_HomeSpd.FuncPtr( 0, 0, 0 );					/* TODO: Calculate default homespeed        */
/*--------------------------------------------------------------------------------------------------*/
//	pndef_ZretPressTrq.FuncPtr( 0, 0, 0 );				/* Calculate default hometlim               */
/*--------------------------------------------------------------------------------------------------*/
/*	TODO: パラメータ計算関数を作成 																	*/
/*--------------------------------------------------------------------------------------------------*/
//	kx = MtHdl.mtPrm.BasePrm.PosingRateKxIx;			/* get Acceleration Kx 						*/
//	NoutV.Acc = MlibMulgain29( OpPrm.AccRate, kx );		/* PnB29 Acceleration [ref/min/ms] 			*/
//	NoutV.Dec = MlibMulgain29( OpPrm.DecRate, kx );		/* PnB31 Deceleration [ref/min/ms]			*/
//	NoutV.HomeSpd = OpPrm.HomeSpd;						/* PnB33 Homing Speed [ref/min]				*/
/*--------------------------------------------------------------------------------------------------*/
//	kx = MtHdl.mtPrm.BasePrm.invTrqKx;
//	NoutV.HomeTlim = MlibMulgain(OpPrm.ZretPressTrq, kx );	/* PnBDA Homing Press Torque [%] 		*/
/*--------------------------------------------------------------------------------------------------*/
//	MtcSetHomeMethod( OpPrm.HomeMethod );               /* set homing method(PnB31)                 */
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#pragma arm section code 

/****************************************************************************************************/
/*																									*/
/*  Follow up request 																				*/
/*																									*/
/****************************************************************************************************/
void IxxAxctrlReqFollowUp( MT_CMD *cmdData ) 
{
	cmdData->Mtype    = MTYPE_FOLLOW;
	cmdData->CtrlMode = MT_POS_CTRLMD;
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Skip request 																					*/
/*																									*/
/****************************************************************************************************/
void IxxAxctrlReqSkip( MT_CMD *cmdData, MT_HOLD_MODE mode ) 
{
MTHNDL *hdl = (&MtHdl);                                     /* 暫定 */
	cmdData->Mtype = MTYPE_HOLD;                            /* MTYPEセット */
	cmdData->HoldMod = mode;                                /* Holdモードセット */
	cmdData->CtrlMode = MT_POS_CTRLMD;                      /* 位置制御モードセット */
	cmdData->CmdRstReq = TRUE;                              /* コマンド更新セット */
	MciCancelLatch( hdl, MOTION_MDL_LTMD);                  /* MOTION LTモードクリア */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Hold request 																					*/
/*																									*/
/****************************************************************************************************/
void IxxAxctrlReqHold( MT_CMD *cmdData ) 
{
MTHNDL *hdl = (&MtHdl);                                     /* 暫定 */
    *cmdData = hdl->cmdData;									/* copy current command data */
	cmdData->bHold = TRUE;                                  /* set hold mode */
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Feed request 																					*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqFeed( MT_CMD *cmdData, long tspeed, long acc, long dec )
{
MTHNDL *hdl = (&MtHdl);							/* 暫定 */

	cmdData->Mtype     = MTYPE_FEED;			/* MTYPEセット */
	cmdData->TSpeed    = tspeed;				/* TSPDセット */
	cmdData->CtrlMode  = MT_POS_CTRLMD;			/* 位置制御モードセット */
	cmdData->CmdRstReq = TRUE;					/* コマンド更新セット */
	cmdData->Trq = 0;							/* TODO:TFF/TLIMセット */
	hdl->mtPrm.MgPrm.accrate2 = acc;	        /* TODO:Acceleration: ref/cycle/cycle  */
	hdl->mtPrm.MgPrm.decrate2 = dec;	        /* TODO:Acceleration: ref/cycle/cycle  */
	MciCancelLatch( hdl, MOTION_MDL_LTMD);		/* TODO:MOTION LTモードクリア */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->TspdKx    = hdl->mtgHdl.basePrm->PosingSpdKxIx;  /* 暫定 */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Homing request 																					*/
/*  TODO: Homing -> Hold -> HomingでZretを再開できるかどうか、確認									*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqHoming( MT_CMD *cmdData, long tspeed, MT_LATCH_SGNL sgnl )
{
MTHNDL *hdl = (&MtHdl);							/* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_HOMING;				/* MTYPEセット */
	cmdData->TSpeed = tspeed;					/* TSPDセット */
	cmdData->LatchSgnl = sgnl;					/* ラッチ信号セット */
	cmdData->CtrlMode = MT_POS_CTRLMD;			/* 位置制御モードセット */
	cmdData->CmdRstReq = TRUE;					/* コマンド更新セット */
	cmdData->Trq = 0;							/* TODO:TFF/TLIMセット */
	hdl->mtPrm.MgPrm.accrate2 = NoutV.Acc;		/* TODO:Acceleration: ref/cycle/cycle  */
	hdl->mtPrm.MgPrm.decrate2 = NoutV.Dec;		/* TODO:Acceleration: ref/cycle/cycle  */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->TspdKx    = hdl->mtgHdl.basePrm->PosingSpdKxIx;  /* 暫定 */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Zero stop request 																				*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqZstop( MT_CMD *cmdData )
{
MTHNDL *hdl = (&MtHdl);							/* 暫定 */
	cmdData->Mtype = MTYPE_VELCTRL;				/* MTYPEセット */
	cmdData->Velocity = 0;						/* Vrefセット */
	cmdData->CtrlMode = MT_VEL_CTRLMD;			/* 速度制御モードセット */
	cmdData->CmdRstReq = TRUE;					/* コマンド更新セット */
	cmdData->Trq = 0;							/* TODO:TFF/TLIMセット */
	MciCancelLatch( hdl, MOTION_MDL_LTMD);		/* TODO:  MOTION LTモードクリア */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Posing request 																					*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqPosing( MT_CMD *cmdData,                  /* Posing request                     */
				long tdst,                                    /* target distance [ref]              */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec )                                    /* deceleration [ref/cycle^2]         */
{
MTHNDL *hdl = (&MtHdl);							    /* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_POSING;				    /* motion type = positioning */
	cmdData->TPos  = PsnGetPOS(&(hdl->psnHdl)) + tdst; /* target position */
	cmdData->TSpeed = tspeed;					    /* target speed */
	cmdData->CtrlMode = MT_POS_CTRLMD;			    /* 位置制御モードセット */
	cmdData->CmdRstReq = TRUE;					    /* コマンド更新セット */
	cmdData->Trq = 0;							    /* TODO:TFF/TLIMセット */
	/*----------------------------------------------------------------------------------------------*/
	hdl->mtPrm.MgPrm.accrate2 = acc;			    /* Acceleration: ref/cycle/cycle  */
	hdl->mtPrm.MgPrm.decrate2 = dec;			    /* Acceleration: ref/cycle/cycle  */
	/*----------------------------------------------------------------------------------------------*/
	MciCancelLatch( hdl, MOTION_MDL_LTMD);		    /* MOTION LTモードクリア */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->TspdKx    = hdl->mtgHdl.basePrm->PosingSpdKxIx;  /* 暫定 */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Ex Posing request 																				*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqExPosing( MT_CMD *cmdData,                /* Posing request                     */
				long tdst,                                    /* target position [ref]              */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long rdst,
				long rspd,
				MT_LATCH_SGNL sgnl )              
{
MTHNDL *hdl = (&MtHdl);							/* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	IxxAxctrlReqPosing( cmdData, tdst, tspeed, acc, dec );   /* set posing cmd */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->LatchSgnl = sgnl;                               /* ラッチ信号セット */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_EXPOSING;                         /* set Motion type */ 
	cmdData->Rdst = rdst;
	cmdData->Rspd = rspd;
	hdl->latch.Mode = EXPOSING_LTMD; 			             /* MOTION LT MODEセット */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Ex Feed request 																				*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqExFeed( MT_CMD *cmdData,                  /* Feed request                       */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long rdst,
				long rspd,
				MT_LATCH_SGNL sgnl )              
{
MTHNDL *hdl = (&MtHdl);							/* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	IxxAxctrlReqFeed( cmdData, tspeed, acc, dec );           /* set feed cmd */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->LatchSgnl = sgnl;                               /* ラッチ信号セット */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_EXFEED;                           /* set Motion type */ 
	cmdData->Rdst = rdst;
	cmdData->Rspd = rspd;
	hdl->latch.Mode = EXPOSING_LTMD; 			             /* MOTION LT MODEセット */
/*--------------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*  Resume request 																					*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqResume( MT_CMD *mtcmd, long tspd )
{
MTHNDL *hdl = (&MtHdl);							/* 暫定 */
	*mtcmd = hdl->cmdData;                                 /* copy current command */
	mtcmd->bHold = FALSE;                                  /* reset hold flag */
	mtcmd->TSpeed = tspd;                                  /* TODO:reset target speed */
	return;
}


/****************************************************************************************************/
/*																									*/
/*  Interpolate request 																			*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqInterpolate( MT_CMD *cmdData,             /* @@ Interpolate request             */
				long tdst,                                    /* target distance [ref]              */
				long vff,                                     /* velocity feed forwared[2^24/OS]    */
				long tff )                                    /* torque feed forwared[2^24/MaxTrq]  */
{
MTHNDL *hdl = (&MtHdl);							    		/* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_INTERPOL;					    /* motion type = positioning */
	cmdData->TPos  = PsnGetPOS(&(hdl->psnHdl)) + tdst; 		/* target position */
	cmdData->TSpeed = 0;								    /* set target speed */
	cmdData->CtrlMode = MT_POS_CTRLMD;			    		/* position control mode*/
	cmdData->CmdRstReq = TRUE;					    		/* command update */
	cmdData->Velocity = vff;								/* set VFF */
	cmdData->Trq = tff;							    		/* set TFF */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット */
	return;
}


/****************************************************************************************************/
/*																									*/
/*  Velctrl request 																				*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqVelctrl( MT_CMD *cmdData,               /* @@ Velctrl request                 */
				long tvelo,                                 /* target velocity [2^24/OS]          */
				long tff )                                  /* torque feed forwared[2^24/MaxTrq]	*/
{
//MTHNDL *hdl = (&MtHdl);							    		/* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_VELCTRL;						    /* motion type = velctrl */
	cmdData->Velocity = tvelo;								/* target velocity [2^24/OS] */
	cmdData->Trq = tff;                                     /* torque feed forwared [2^24/MaxTrq] */
	cmdData->CtrlMode = MT_VEL_CTRLMD;			    		/* velocity control mode */
	/*----------------------------------------------------------------------------------------------*/
	/* Vref ->(MlibMulgain)<- hdl->mtgHdl.basePrm->maxtoOsKx => inOut->VrefOut ;                      */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット      */
	return;
}


/****************************************************************************************************/
/*																									*/
/*  Trqctrl request 																				*/
/*																									*/
/****************************************************************************************************/
void	IxxAxctrlReqTrqctrl( MT_CMD *cmdData,               /* @@ Velctrl request                   */
				long ttrq,                                  /* target torque [2^24/MAXTRQ]          */
				long vref )								/* velocity limit [2^24/OS]				*/
{
//MTHNDL *hdl = (&MtHdl);							    		/* 暫定 */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->Mtype = MTYPE_TRQCTRL;						    /* motion type = velctrl 				*/
	cmdData->Velocity = vref;								/* velocity limit [2^24/OS] 			*/
	cmdData->Trq = ttrq;                                    /* target torque [2^24/MAXTRQ]          */
	cmdData->CtrlMode = MT_TRQ_CTRLMD;			 	  		/* torque control mode					*/
	/*----------------------------------------------------------------------------------------------*/
	/* Vref ->(MlibMulgain)<- hdl->mtgHdl.basePrm->maxtoOsKx => inOut->VrefOut ;                    */
	/*----------------------------------------------------------------------------------------------*/
	cmdData->bHold = FALSE;                                 /* Holdモードセット      				*/
	return;
}

