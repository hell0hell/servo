/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MicroScanAIf.c : �ʃv���O����ScanA I/F������`												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �ʃv���O������ScanA��CPU�Ԃ�I/F����														*/
/*																									*/
/*		�ʃv���O�����|CPU�Ԃ̃f�[�^������s���B														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef MICRO_SCANA_IF_H
#define MICRO_SCANA_IF_H


typedef struct MICRO_SA_IPRMS {
	BOOL	RvsDir;				/* �t��]���[�h 													*/
	BOOL	CarrChange;			/* �L�����A���g���؂�ւ��t���O [0:OFF,1:ON]						*/
	USHORT	flg_wf;				/* PnF1E : �t���O													*/
	DBYTEX	spgfail;			/* PnE1E : ����:S-PG��M���s���e��, ���:FS-PG��M���s���e��	*/
	LONG	SpdMaNumBit;		/* ���[�^���x���o�ړ����σt�B���^��(2�̗ݏ�)�̏����l				*/
	LONG	KmotspdA;			/* ScanA Motor Speed Cal. Gain										*/
	LONG	SvCycleNs;			/* �T�[�{������� [ns]												*/
} MICRO_SA_IPRMS;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void MicroSA_InitVariables( BASE_CTRL_HNDL *pBaseCtrl, MENCV *MencV, ASICS *SvAsicRegs, MICRO_SA_IPRMS *uIniPrm );
//void MicroSA_ResetVariables( AXIS_HANDLE *Axis );
void MicroSA_InitGainParameters( ASIC_U_IF_STR *pAsicMicroReg );

void MicroSA_InputCycDataB( ASICS *SvAsicRegs, MENCV *MencV, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm );
void MicroSA_OutputCycDataB( ASICS *SvAsicRegs, MENCV *MencV, BASE_CTRL_HNDL *pBaseCtrl, LONG CmdCtrlBit );
void MicroSA_InputCycDataExB( ASICS *SvAsicRegs, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm );
void MicroSA_OutputCycDataExB( ASICS *SvAsicRegs, BASE_CTRL_HNDL *pBaseCtrl);

void MicroSA_InputCycDataA(ASICS *SvAsicRegs, MENCV *MencV, CTRL_LOOP_OUT *CtrlLoopOut, BPRMDAT *Bprm);
void MicroSA_OutputCycDataA(ASICS *SvAsicRegs, BASE_CTRL_HNDL *pBaseCtrl);

void MicroIfInputCycData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs );
void MicroIfOutputCycData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs );

#endif /* MICRO_SCANA_IF_H */
/***************************************** end of file **********************************************/
