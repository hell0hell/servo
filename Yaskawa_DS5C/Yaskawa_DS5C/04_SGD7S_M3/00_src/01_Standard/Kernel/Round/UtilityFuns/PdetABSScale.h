/****************************************************************************
  Description   : Absolute Scale Utility functions
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
  ------------------------------------------------------------------------
	I.M.   2011.02.01     created

****************************************************************************/
#ifndef __ABS_SCALE_SET_H 
#define __ABS_SCALE_SET_H


/* Sequence step */
typedef enum PDET_PHWR_SEQ {
	PDETSEQ_IDOL = 0,
	PDETSEQ_PHASE_WR_READY,
	PDETSEQ_WAIT_BASEBLOCK,
	PDETSEQ_WRITE_PHASE_OFS,
	PDETSEQ_FINISHED,
	PDETSEQ_ERROR,
} PDET_PHWR_SEQ;

/* work handle */
typedef	struct PDET_ABSCONTROL {
	PDET_PHWR_SEQ	SeqStep;
} PDET_ABSCONTROL;



/****************************************************************************
 API functions
*****************************************************************************/
BOOL PdetEnablePhaseOffsetWrite(PDET_ABSCONTROL *PdtCtrl, MENCV *MencV, BOOL enable);
BOOL PdetStartPhaseOffsetWrite(PDET_ABSCONTROL *PdtCtrl, MENCV *MencV, LONG PhaseOffset);
BOOL PdetGetPhaseOffsetWriteReq(PDET_ABSCONTROL *PdtCtrl);
BOOL PdetCheckPdetCondition(PDET_ABSCONTROL *PdtCtrl, MENCV *MencV);
BOOL PdetWriteMotorPhaseSequence(PDET_ABSCONTROL	*PdtCtrl, 
								 MENCV				*MencV, 
								 ASIC_HW_IF_STR		*AsicHwReg, 
								 PREGIF_CONFIG_T	hPnReg);

#endif //__ABS_SCALE_SET_H 

