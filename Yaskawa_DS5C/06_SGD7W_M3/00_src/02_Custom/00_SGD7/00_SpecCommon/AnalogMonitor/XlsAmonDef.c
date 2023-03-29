/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsAmonDef.c : アナログモニタ定義ソースファイル												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. アナログモニタ定義テーブル																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2013.10.03 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "XlsAmonDef.h"
#include "AnalogMonitorCalc.h"
#include "DataTraceCalc.h"
#include "MercuryGlobals.h"





/****************************************************************************************************/
/*																									*/
/*		アナログモニタ定義テーブル																	*/
/*																									*/
/****************************************************************************************************/
const	AMONDEF	AmonDefTbl[] = {
/*   Sel  Type  VarFuncPtr                      GainFuncPtr											*/
/*--------------------------------------------------------------------------------------------------*/
  { 0x00, 0x00, GetVarData_AmonMotSpd,			AmonGcalMotorSpeed	},	/* モータ回転速度			*/
  { 0x01, 0x00, GetVarData_AmonSpdRef,			AmonGcalMotorSpeed	},	/* 速度指令					*/
  { 0x02, 0x00, GetVarData_AmonTrqRef,			AmonGcalTorqueRef	},	/* トルク指令				*/
  { 0x03, 0x00, GetVarData_AmonPosErra,			AmonGcalPulseError	},	/* 位置偏差					*/
  { 0x04, 0x00, GetVarData_AmonPosErrx,			AmonGcalPulseError	},	/* 位置アンプ偏差			*/
  { 0x05, 0x00, GetVarData_AmondPosCmdSmp,		AmonGcalPosCmdaSpd	},	/* 位置指令速度				*/
  { 0x06, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約(使用不可)			*/
  { 0x07, 0x00, GetVarData_sfperra_per32s,		AmonGcalPulseErr01	},	/* モータ－負荷間位置偏差	*/
  { 0x08, 0x02, (LONG(*)(void*))GetBitData_Coin,AmonGcalBitSignal	},	/* 位置決め完了指令			*/
  { 0x09, 0x00, GetVarData_SpdFFCx,				AmonGcalMotorSpeed	},	/* 速度ＦＦ					*/
  { 0x0A, 0x00, GetVarData_TrqFFCx,				AmonGcalTorqueRef	},	/* トルクＦＦ				*/
  { 0x0B, 0x00, GetVarData_AmonActGain,			AmonGcalActGain		},	/* 有効ゲイン				*/
  { 0x0C, 0x02, (LONG(*)(void*))GetBitData_Den,	AmonGcalBitSignal	},	/* 位置指令払い出し完了指令	*/
  { 0x0D, 0x00, GetVarData_AmonFencSpdsmp,		AmonGcalMotorSpeed	},	/* 外部エンコーダ速度		*/
  { 0x0E, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x0F, 0xF0, GetVarData_Dummy,				AmonGcalDummy		},	/* 指定変数(PnE40,PnE42)	*/
  { 0x40, 0x00, GetVarData_dPosRefi,			AmonGcalMotPfbkSpd	},	/* 位置ループ前指令速度		*/
  { 0x41, 0x00, GetVarData_AmonDisTrq,			AmonGcalTorqueRef	},	/* 摩擦補償トルク			*/
  { 0x42, 0x00, GetVarData_MonResVib,			AmonGcalMotorSpeed	},	/* 制振モニタ				*/
  { 0x43, 0x00, GetVarData_TrqRefBeforeComp,	AmonGcalTorqueRef	},	/* リップル補正前トルク指令	*/
  { 0x44, 0x00, GetVarData_AmonIdRef,			AmonGcalTrqRef15K	},	/* d軸電流指令				*/
  { 0x45, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x46, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x47, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x48, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x49, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x4A, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x4B, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x4C, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x4D, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x4E, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
  { 0x4F, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* 予約						*/
};
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	AmonDefTblEntNum = sizeof(AmonDefTbl)/sizeof(AmonDefTbl[0]);
/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/
