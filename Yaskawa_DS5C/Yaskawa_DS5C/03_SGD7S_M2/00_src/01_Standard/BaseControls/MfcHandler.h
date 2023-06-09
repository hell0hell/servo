#ifndef	_MFC_HANDLER_H /* ΔΗέέh~p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MfcHandler.h : fΗ]§δpnhθ`													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	Ε	2009.12.22	Y.Oka	SGDV-M3 Λ SVN											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		lebZpθθ`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MFCPRM {							/* lebpp[^θ`							*/
#if (FLOAT_USE==TRUE)
	float	Fsft;									/* csqtB^ͺπ\VtgΚ				*/
	float	InvFsft;								/* csqtB^ͺπ\VtgΚΜt		*/
	float	Kf1[2];									/* QtB^QCP							*/
	float	Kf2;									/* QtB^QCQ(PKχͺtB^p)		*/
	float	Kf3;									/* QtB^QCR(PKχͺtB^p)		*/
	float	Kj1;									/* JWQCP								*/
	float	Kj2;									/* JWQCQ								*/
	float	Kj2_sft;								/* JWQCQ(ͺπ\UPpΌΜΔΘ΅)			*/
	float	Kj3;									/* JWQCR								*/
	float	Kvff;									/* ¬xtB[htH[hQC					*/
	float	Ktff1;									/* gNtB[htH[hQCP(³]p)		*/
	float	Ktff2;									/* gNtB[htH[hQCQ(t]p)		*/
	float	Cfric;									/* N[CβgN						*/
	float	Kfcf;									/* N[CβtB^QC				*/
	float	Fccflag;								/* 2-mass && Full-Closed Control Flag			*/
	float	PSFccflag;								/* 2-mass && PS Type Full-Closed Control Flag	*/
	LONG	MFCModel;								/* @Bf									*/
	float	MFCType;								/* MFC^Cv								*/
#else
	LONG	Fsft;									/* csqtB^ͺπ\VtgΚ				*/
	LONG	InvFsft;								/* csqtB^ͺπ\VtgΚΜt		*/
	LONG	Kf1[2];									/* QtB^QCP							*/
	LONG	Kf2;									/* QtB^QCQ(PKχͺtB^p)		*/
	LONG	Kf3;									/* QtB^QCR(PKχͺtB^p)		*/
	LONG	Kj1;									/* JWQCP								*/
	LONG	Kj2;									/* JWQCQ								*/
	LONG	Kj2_sft;								/* JWQCQ(ͺπ\UPpΌΜΔΘ΅)			*/
	LONG	Kj3;									/* JWQCR								*/
	LONG	Kvff;									/* ¬xtB[htH[hQC					*/
	LONG	Ktff1;									/* gNtB[htH[hQCP(³]p)		*/
	LONG	Ktff2;									/* gNtB[htH[hQCQ(t]p)		*/
	LONG	Cfric;									/* N[CβgN						*/
	LONG	Kfcf;									/* N[CβtB^QC				*/
	LONG	Fccflag;								/* 2-mass && Full-Closed Control Flag			*/
	LONG	PSFccflag;								/* 2-mass && PS Type Full-Closed Control Flag	*/
	LONG	MFCModel;								/* @Bf									*/
	LONG	MFCType;								/* MFC^Cv								*/
#endif /* FLOAT_USE */
} MFCPRM;



/*--------------------------------------------------------------------------------------------------*/
/*		lebZpΟθ`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MFCTRL {							/* lebpp[^Οθ`						*/
	struct	{									/* lebpp[^θ`							*/
		MFCPRM		MfcPrm[3];						/* lebpp[^							*/
		MFCPRM		MfcPexe;						/* ΐsplebpp[^						*/
		LONG		MFCTypeSetting;					/* MFC^Cv(p[^έθl)				*/
		LONG		MFCModelSetting;				/* @Bf(p[^έθl)					*/
		BOOL		MFCFF;							/* lebΜOee(¬xCΔΩΈ)Lψ^³ψέθ	*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* lebpΟf[^θ`							*/
		LONG		CurMFCModel;					/* @Bf(»έΜέθl)						*/
		LONG		execnt;							/* Execute Count for MFC Execute Control		*/
		LONG		MfcPosErr;						/* fΚuΞ·(Zp)	[pulse]				*/
		LONG		PosErr;							/* fΚuΞ·(j^p)	[pulse]				*/
		LONG		Per64[2];						/* fΚuΞ·(Zp)	[pulse]				*/
		LONG		Vref;							/* f¬xwί			[2^24/OvrSpd]		*/
		LONG		Pfbk;							/* fΚuea			[pulse/Scan]		*/
	/*----------------------------------------------------------------------------------------------*/
		KSGAIN		Fwk1[2];						/* QtB^Zp							*//*<S00A>*/
		KSGAIN		Fwk2[2];						/* QtB^Zp							*//*<S00A>*/
		KSGAIN		Fwk3[2];						/* QtB^Zp							*//*<S00A>*/
		KSGAIN		Fwk4[2];						/* QtB^Zp							*//*<S00A>*/
		LONG		Filo;							/* PKχͺQtB^γΚuwί				*/
		LONG		FCmdF;							/* ͺπ\UPQtB^γΚuwί				*/
		LONG		dFilwk;							/* PKχͺQtB^Zp					*/
		LONG		Fwkrem1;						/* PKχͺQtB^Zp					*/
		LONG		Fwkrem2;						/* PKχͺQtB^Zp					*/
	/*----------------------------------------------------------------------------------------------*/
		KSGAIN		PCmdsub;						/* ΚuwίόΝ									*//*<S00A>*/
		KSGAIN		FCmd[3];						/* tB^γΚuwί						*//*<S00A>*/
		LONG		dFCmd[2];						/* tB^γΚuwίPKχͺl				*/
		LONG		d2FCmd[2];						/* tB^γΚuwίQKχͺl				*/
		LONG		d3FCmd;							/* tB^γΚuwίRKχͺl				*/
		LONG		wkrem1;							/* ΚuwίZp								*/
		LONG		wkrem2;							/* ΚuwίZp								*/
		LONG		PosSum[2];						/* ΚuwίΑZΟZl							*/
		LONG		TrqRef;							/* fgNwί(j^Β)					*/
		LONG		Ktffcmdx;						/* gNeeQC								*/
		LONG		FricCmpTrq;						/* N[CβgN						*/
		LONG		FCmdFilo[3];					/* tB^γΚuwί(ͺπ\UPvZ)			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG		PffCmd;							/* Κueewί(j^Β)						*/
		LONG		VffCmd;							/* ¬xeewί 								*/
		LONG		TffCmd;							/* gNeewί(j^Β)						*/
	/*----------------------------------------------------------------------------------------------*/
		BOOL		MfcStopReq;						/* MFCβ~v(Fn©ηΜv)					*/
	} var;
} MFCTRL;



#define _MFC_HANDLER_H /* ΔΗέέh~p */
#endif /* _MFC_HANDLER_H */
/***************************************** end of file **********************************************/
