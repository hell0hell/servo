/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsAmonDef.c : �A�i���O���j�^��`�\�[�X�t�@�C��												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. �A�i���O���j�^��`�e�[�u��																*/
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
/*		�A�i���O���j�^��`�e�[�u��																	*/
/*																									*/
/****************************************************************************************************/
const	AMONDEF	AmonDefTbl[] = {
/*   Sel  Type  VarFuncPtr                      GainFuncPtr											*/
/*--------------------------------------------------------------------------------------------------*/
  { 0x00, 0x00, GetVarData_AmonMotSpd,			AmonGcalMotorSpeed	},	/* ���[�^��]���x			*/
  { 0x01, 0x00, GetVarData_AmonSpdRef,			AmonGcalMotorSpeed	},	/* ���x�w��					*/
  { 0x02, 0x00, GetVarData_AmonTrqRef,			AmonGcalTorqueRef	},	/* �g���N�w��				*/
  { 0x03, 0x00, GetVarData_AmonPosErra,			AmonGcalPulseError	},	/* �ʒu�΍�					*/
  { 0x04, 0x00, GetVarData_AmonPosErrx,			AmonGcalPulseError	},	/* �ʒu�A���v�΍�			*/
  { 0x05, 0x00, GetVarData_AmondPosCmdSmp,		AmonGcalPosCmdaSpd	},	/* �ʒu�w�ߑ��x				*/
  { 0x06, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��(�g�p�s��)			*/
  { 0x07, 0x00, GetVarData_sfperra_per32s,		AmonGcalPulseErr01	},	/* ���[�^�|���׊Ԉʒu�΍�	*/
  { 0x08, 0x02, (LONG(*)(void*))GetBitData_Coin,AmonGcalBitSignal	},	/* �ʒu���ߊ����w��			*/
  { 0x09, 0x00, GetVarData_SpdFFCx,				AmonGcalMotorSpeed	},	/* ���x�e�e					*/
  { 0x0A, 0x00, GetVarData_TrqFFCx,				AmonGcalTorqueRef	},	/* �g���N�e�e				*/
  { 0x0B, 0x00, GetVarData_AmonActGain,			AmonGcalActGain		},	/* �L���Q�C��				*/
  { 0x0C, 0x02, (LONG(*)(void*))GetBitData_Den,	AmonGcalBitSignal	},	/* �ʒu�w�ߕ����o�������w��	*/
  { 0x0D, 0x00, GetVarData_AmonFencSpdsmp,		AmonGcalMotorSpeed	},	/* �O���G���R�[�_���x		*/
  { 0x0E, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x0F, 0xF0, GetVarData_Dummy,				AmonGcalDummy		},	/* �w��ϐ�(PnE40,PnE42)	*/
  { 0x40, 0x00, GetVarData_dPosRefi,			AmonGcalMotPfbkSpd	},	/* �ʒu���[�v�O�w�ߑ��x		*/
  { 0x41, 0x00, GetVarData_AmonDisTrq,			AmonGcalTorqueRef	},	/* ���C�⏞�g���N			*/
  { 0x42, 0x00, GetVarData_MonResVib,			AmonGcalMotorSpeed	},	/* ���U���j�^				*/
  { 0x43, 0x00, GetVarData_TrqRefBeforeComp,	AmonGcalTorqueRef	},	/* ���b�v���␳�O�g���N�w��	*/
  { 0x44, 0x00, GetVarData_AmonIdRef,			AmonGcalTrqRef15K	},	/* d���d���w��				*/
  { 0x45, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x46, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x47, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x48, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x49, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x4A, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x4B, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x4C, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x4D, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x4E, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
  { 0x4F, 0x00, GetVarData_Dummy,				AmonGcalDummy		},	/* �\��						*/
};
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	AmonDefTblEntNum = sizeof(AmonDefTbl)/sizeof(AmonDefTbl[0]);
/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/
