/****************************************************************************/
/*																			*/
/*																			*/
/*		POSITION MANAGER ヘッダファイル										*/
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
#ifndef		POSITION_H_
#define		POSITION_H_


/* defines */
//#define SOFT_LIMIT_P	0x1
//#define SOFT_LIMIT_N	0x2

#define SOFT_LIMIT_PBIT	0x0
#define SOFT_LIMIT_NBIT	0x1

/* typedefs: struct */


/* forward declarations */
/* Public functions */
extern VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm);
extern VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt);
extern BOOL PsnGetPsetSts(PSNHNDL* hdl);
extern BOOL PsnGetNearSts(PSNHNDL* hdl);
extern BOOL PsnGetZPointSts(PSNHNDL* hdl);
extern LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos);
extern LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos);
extern VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err);
extern UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos);
extern UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot);
extern VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos);
extern VOID PsnResetApos(PSNHNDL* hdl, BOOL abs);
extern BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt);

/* 操作関数マクロ定義 */
#if 0
/* VOID PsnSetRefPoint(PSHANDL* hdl, BOOL enable) */
#define	PsnSetRefPoint(hdl, enable)	((hdl)->RefEnable = enable)
/* VOID PsnResetIPOS(PSHANDL* hdl) */
#define	PsnResetIPOS(hdl)			((hdl)->Ipos = (hdl)->Pos)
/* VOID	PsnSetTPOSNoLimit(hdl, tpos) */
#define	PsnSetTPOSNoLimit(hdl,dpos)	((hdl)->Tpos = dpos + (hdl)->Ipos)
/* VOID PsnSetPOS(PSNHNDL* hdl, LONG dpos);*/
#define	PsnSetPOS(hdl, dpos)		{(hdl)->Pos += dpos;\
									 (hdl)->lstDpos = (hdl)->Dpos;\
									 (hdl)->Dpos = dpos;}
/* VOID PsnSetIPOS(PSHANDL* hdl,LONG dpos) */
#define	PsnSetIPOS(hdl, dpos)		((hdl)->Ipos += dpos)
/* VOID PsnSetDenSts(PSHANDL* hdl, BOOL den) */
#define	PsnSetDenSts(hdl, den)		((hdl)->Den = den)
/* BOOL PsnGetDenSts(PSHANDL* hdl) */
#define	PsnGetDenSts(hdl)			((hdl)->Den)
/* LONG PsnGetTPOS(PSHANDL* hdl) */
#define	PsnGetTPOS(hdl)				((hdl)->Tpos)
/* LONG PsnGetIPOS(PSHANDL* hdl) */
#define	PsnGetIPOS(hdl)				((hdl)->Ipos)
/* LONG PsnGetPOS(PSHANDL* hdl) */
#define	PsnGetPOS(hdl)				((hdl)->Pos)
/* LONG PsnGetMPOS(PSHANDL* hdl) */
#define	PsnGetMPOS(hdl)				((hdl)->Mpos)
/* LONG PsnGetLPOS(PSHANDL* hdl, UCHAR sgnl) */
#define	PsnGetLPOS(hdl)				((hdl)->Lpos)
#define	PsnGetLstLPOS(hdl)			((hdl)->LstLpos)
/* LONG PsnGetAPOS(PSHANDL* hdl) */
#define	PsnGetAPOS(hdl)				((hdl)->Apos)
/* LONG PsnGetPERR(PSHANDL* hdl) */
#define	PsnGetPERR(hdl)				((hdl)->Perr)
/* UCHAR PsnGetSOTSts(PSHANDL* hdl) */
#define	PsnGetSOTSts(hdl)			((hdl)->SoftLimit)
/* LONG PsnGetFBSPD(PSHANDL* hdl) */
#define	PsnGetFBSPD(hdl)			((hdl)->Fbspd)
/* VOID PsnAttachPRM(PSHANDL* hdl,PSN_PRM* prm) */
#define	PsnAttachPRM(hdl, prm)		((hdl)->Prm = prm)
#endif

#endif /* POSITION_H_ */
