/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdOpPrmMon.c : LCD�I�y���[�^�������W���[��:PnUn���										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	�@ �\ : SigmaMessage�ɂ��k�b�c�I�y���[�^�ƒʐM���A�I�y���[�^����̊e�푀������s����B		*/
/*																									*/
/*			1)���j�^�@�\ : �w�胂�j�^�ϐ�(UnXXX)�̕\��												*/
/*			2)�p�����[�^ : �w��p�����[�^(PnXXX)�̕\��,�ҏW,�ύX									*/
/*			3)																						*/
/*			4)																						*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.30 : 2012.04.xx  K.Ozaki      Revised for Mercury 									*/
/*		Rev.1.40 : 2013.09.26  K.Ozaki		<S00C>													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"			/* Kernel API */
#include "LcdOp.h"			/* LcdOpIf Common Header */
#include "Monitor.h"		/* Un (Monitor) Common Header */
#include "Parameter.h"		/* Pn (Parameter) Common Header */
#include "PrmAccessIf.h"	/* Parameter Access Management Header */

extern MSGIF LopMsg;					/* TODO: ����LcdOpe�FMessage I/F										*/
#define RpxPrmUpdateEepromIdx( Idx ) (OK) /* TODO: */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�k�b�c�I�y���[�^�h�e�֐�																	*/
/*--------------------------------------------------------------------------------------------------*/
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel );		/* PRM/MON Display Main Function		*/
void	LcdRstPnUnDspData( void );							/* Reset Pn/UN Display Data 			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Local Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnUnEditSub(										/* Pn/Un Edit Mode Procedure			*/
		USHORT	dispID,										/* �f�B�X�v���[ID(LCDPMDSP_PM/JOG/etc)	*/
		OPKEYFLG *pKeyMk,									/* �L�[�������̃A�h���X				*/
		BLINKFLG *pBlnkKickFlg	);							/* �u�����N�N���t���O�|�C���^�[			*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnUnNumEdit( 
		USHORT dispID, 
		OPKEYFLG *pKeyMk, 
		BLINKFLG *pBlnkKickFlg );
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnUnSel( 
		USHORT dispID, 
		OPKEYFLG *pKeyMk,
		BLINKFLG *pBlnkKickFlg );
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnDataEdit( 
		USHORT dispID,
		OPKEYFLG *pKeyMk,
		BLINKFLG *pBlnkKickFlg );
/*--------------------------------------------------------------------------------------------------*/
void	LcdMkDspDataPnUnLines(			/* Make Display Data for Pn/Un Lines					*/
		USHORT	dispID,						/* �f�B�X�v���[ID(LCDPMDSP_PM/JOG/etc)					*/
		USHORT	BlnkSupFlg,					/* �u�����N�}�~�t���O									*/
		UCHAR	*ResBuf			);			/* �����o�b�t�@�A�h���X									*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPrmDataBlinkDpos( 
		CPRMDEF *pPrmDef );
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Lcd PRM/MON Display Mode Main Function														*/
/*																									*/
/****************************************************************************************************/
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel )
{
UCHAR	*ResBuf = hMsg->ResBuf;
/*--------------------------------------------------------------------------------------------------*/
/*		�ҏW����																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( LCD_CHKCMD_RCVWAIT() ) 			/** �I�v�V��������̉����f�[�^�҂��ȊO�̎��͕ҏW���� **/
		{
			LcdPnUnEditSub(LCDPMDSP_PM, &OpKeyMk, &OpBlnkKickFlg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\���f�[�^�쐬���� : Clear ResBuf															*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		�\���f�[�^�쐬���� : TITLE																	*/
/*--------------------------------------------------------------------------------------------------*/
//		if( (hMsg->AccessLvl == ACCLVL_USER1) /* TODO: &&  (Iprm.OpeDspMode == OPEDSP_SETUPPRM) */)	/* <S113> */
		if( (hMsg->AccessLvl == ACCLVL_USER1) &&  (hMsg->OpeDspMode == OPEDSP_SETUPPRM) )			/* <S113> */
		{
//			RpxLcdSetStr(0, 8, (UCHAR *)"-PRM/MON-", (UCHAR *)ResBuf);		/* <S154> */
			RpxLcdSetStr(0, 10, (UCHAR *)"PRM/MON", (UCHAR *)ResBuf);		/* <S154> */
		}
		else
		{
//			RpxLcdSetStr(0, 8, (UCHAR *)"-ALL PRM-", (UCHAR *)ResBuf);				/* <S154> */
			RpxLcdSetStr(0, 10, (UCHAR *)"ALL PRM", (UCHAR *)ResBuf);				/* <S154> */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�\���f�[�^�쐬���� : Pn/Un Lines															*/
/*--------------------------------------------------------------------------------------------------*/
		LcdMkDspDataPnUnLines( LCDPMDSP_PM, FALSE, (UCHAR *)ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		�u�����N���X�e�[�^�X�\������																*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdSetResStatus (ResBuf);			/* <S031> */
		RpxLcdSetBlink (ResBuf);				/* <S031> */
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Pn/Un Edit mode (�߂�l:TRUE=���[�h�L�[���������m���ꂽ���Ƃ�����)							*/
/*																									*/
/****************************************************************************************************/
USHORT	LcdPnUnEditSub(						/* Pn/Un Edit mode										*/
		USHORT	dispID,						/* �f�B�X�v���[ID(LCDPMDSP_PM/JOG/etc)					*/
		OPKEYFLG *pKeyMk,					/* �L�[�������̃A�h���X								*/
		BLINKFLG *pBlnkKickFlg	)			/* �u�����N�N���t���O�|�C���^�[							*/
{
USHORT	ModeChg = FALSE;
#if CCSW_CMDID == CMDID_EXIST				/*<11J42>*/
LONG	rc_tmp;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Switch by EEPROM Write Mode																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( LopV.EewriteMode )
		{
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_None:			/* �p�����[�^�l�\���E�ҏW����										*/
	/*----------------------------------------------------------------------------------------------*/
			switch( LopV.EditMode )
			{
			case LCDEM_PnUnNum:
				ModeChg = LcdPnUnNumEdit( dispID, pKeyMk, pBlnkKickFlg );
				break;
			case LCDEM_PnUnSel:
				ModeChg = LcdPnUnSel( dispID, pKeyMk, pBlnkKickFlg );
				break;
			case LCDEM_PnData:
				ModeChg = LcdPnDataEdit( dispID, pKeyMk, pBlnkKickFlg );
#if CCSW_CMDID == CMDID_EXIST				/*<11J42>*/
				if( ModeChg == TRUE )
				{
					/* �I�v�V�����p�����[�^�ҏW���̎� */
					if( LcdOpt[dispID].Prm[LopV.ActiveLine].State.Editing == TRUE )
					{
						/* �I�v�V�����p�����[�^�������ݗv���Z�b�g(�s�������������̈�) */
						rc_tmp = OpiReqDegOpPrmWrite( LcdOpt[dispID].Prm[LopV.ActiveLine].CurrPnNo,
													  LcdOpt[dispID].Prm[LopV.ActiveLine].Size,
													  LcdOpt[dispID].Prm[LopV.ActiveLine].Data );
						if( rc_tmp == DEGOP_SUCCESS )
						{
							/* �����f�[�^�҂��Z�b�g */
							LcdOpt[dispID].Prm[LopV.ActiveLine].State.RcvWait = TRUE;
							/* �^�C���A�E�g�p�^�C�}���Z�b�g */
							KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
							/* �����f�[�^��M�����R�[��(���F����I�����̂ݍX�V) */
							rc_tmp = LcdOptUpdateResponseData(dispID, &LcdOpt[dispID]);
							while(rc_tmp == TRUE)
							{
								rc_tmp = LcdOptUpdateResponseData(dispID, &LcdOpt[dispID]);
							}
						}
					}
					else
					{
						rc_tmp = RpxPrmUpdateEepromIdx(LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx);
						while(rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL)
						{
							rc_tmp = RpxPrmUpdateEepromIdx(LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx);
						}
					}
				}
#endif
	 			/* 
				 * * Jupiter �ł�Data���������̂�EEPROM���X�V���� *
				 */
				break;
			default:
				break;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_UpdtWait:		/* EEPROM��������(�v���o�^)�҂�����									*/
	/*----------------------------------------------------------------------------------------------*/
			switch( RpxPrmUpdateEepromIdx( LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx ) )
			{
			case OK:	/* ����	*/
				pBlnkKickFlg->PrmEewrite = 1;	/* �u�����N�N��	*/
				LopV.EewriteMode = OPWM_UpdtBlnk;
				break;
			default:
				break;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_UpdtBlnk:		/* �p�����[�^EEPROM�������ݎ��u�����N����							*/
	/*----------------------------------------------------------------------------------------------*/
			if( OpBlnkFlg.PrmEewrite == 0 )
			{
				LopV.EewriteMode = OPWM_None;
				LopV.EditMode = LCDEM_PnUnNum;
				LopV.CurDigit = 0;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_InfWrWait:	/* ��ʍ\�����EEPROM�������ݕs������								*/
	/*----------------------------------------------------------------------------------------------*/
			if( LcdSaveDispInfoTable( dispID ) == OK )
			{
				pBlnkKickFlg->write = TRUE;
				LopV.EewriteMode = OPWM_None;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		default:
	/*----------------------------------------------------------------------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}


/****************************************************************************************************/
/*																									*/
/*		Pn/Un Number Edit mode																		*/
/*																									*/
/****************************************************************************************************/
/*		dispID: �f�B�X�v���[ID(LCDPMDSP_PM=PRM/MON���, LCDPMDSP_JOG=JOG��� etc.), 				*/
/*		pKeyMk: �L�[�������̃A�h���X, pBlnkKickFlg: �u�����N�N���t���O�|�C���^�[					*/
/*		�߂�l: TRUE=���[�h�L�[���������m���ꂽ���Ƃ�����											*/
/****************************************************************************************************/
USHORT	LcdPnUnNumEdit( USHORT dispID, OPKEYFLG *pKeyMk, BLINKFLG *pBlnkKickFlg )
{
LONG	i;
CPRMDEF	*pPrmDef;
USHORT	Numx;
USHORT	PnUn  = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn;
USHORT	UnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].UnIdx;
USHORT	PnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
USHORT	idx_tmp, cur_num_tmp, cur_idx_tmp;
DEGOP_PRM	*OpPrm = &LcdOpt[dispID].Prm[LopV.ActiveLine];
LONG rc;
#endif
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;										/* TODO: ��ŏ��� */

/*--------------------------------------------------------------------------------------------------*/
/*		Pn/Un Number Edit																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( pKeyMk->mode )
		{
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->up )
		{
			if( PnUn == UnSelected )
			{
				Numx = CUNMONX->GetNumber( CUNMONX, UnIdx, &LopMsg ) + (0x01 << (LopV.CurDigit*4));
				if( Numx >= 0x1000 ){ Numx = 0xFFF;}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].UnIdx = 
					CUNMONX->SearchB( (COBJECT *)CUNMONX, Numx, UnIdx, &LopMsg );
			}
			else if( PnUn == PnSelected )
			{
/*<11J42>*/
#if CCSW_CMDID == CMDID_EXIST
/*--------------------------------------------------------------------------------------------------*/
/*			�������擾																			*/
/*--------------------------------------------------------------------------------------------------*/
			/* ���݃T�[�{�p�����[�^�C���f�b�N�X */
			cur_idx_tmp = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
			/* ���݃T�[�{�p�����[�^�ԍ� */
			cur_num_tmp = PnPrmTbl[cur_idx_tmp].PnNo;
			/* ����T�[�{�p�����[�^�ԍ� */
			Numx = cur_num_tmp + (0x01 << (LopV.CurDigit*4));
			if(Numx >= 0x1000)	Numx = 0x0FFF;
			/* ����T�[�{�p�����[�^�C���f�b�N�X */
			idx_tmp = RpxPrmSearchIdxOp(Numx, cur_idx_tmp, &LopMsg);
			OpPrm->NextSvIdx = idx_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM��Ԏ擾																	*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OpiGetDegOpState(&LcdOpt[dispID].PrmRange);

/*--------------------------------------------------------------------------------------------------*/
/*			���ݕҏW���̃p�����[�^�̈�ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
			/* ���[�U�p�����[�^�̈� */
			if( cur_num_tmp < OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedLwrLmt = TRUE;					/* �����l�I�[�o�t���O�Z�b�g		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.LwrLmt;	/* ���݃I�v�V����Prm�ԍ��X�V	*/
			}
			/* �V�X�e���p�����[�^�̈� */
			else if( cur_num_tmp > OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedUprLmt = TRUE;					/* ����l�I�[�o�t���O�Z�b�g		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.UprLmt;	/* ���݃I�v�V����Prm�ԍ��X�V	*/
			}

/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM�L����																		*/
/*--------------------------------------------------------------------------------------------------*/
			if( rc == DEGOP_ENABLE )
			{
			/*--------------------------------------------------------------------------------------*/
			/*	�V�X�e��/�I�v�V�����p�����[�^�̈�̕ҏW												*/
			/*--------------------------------------------------------------------------------------*/
				if( PnPrmTbl[idx_tmp].PnNo >= OPT_PNNO_VIRTUAL )
				{
					/* ����I�v�V�����p�����[�^�ԍ����擾 */
					OpPrm->NextPnNo = LcdOptGetNextOpPrmNo( DEGOP_READ_UP, cur_idx_tmp, LopV.CurDigit, OpPrm );

				/*----------------------------------------------------------------------------------*/
				/*	�I�v�V�����p�����[�^�̈�𒴂����ꍇ											*/
				/*----------------------------------------------------------------------------------*/
					if( OpPrm->NextPnNo >= LcdOpt[dispID].PrmRange.UprLmt )
					{
						/* �I�v�V�����p�����[�^�̈�ȏ�Ƀp�����[�^�����݂��鎞�i�V�X�e�����x���j 	*/
						if( PnPrmTbl[idx_tmp].PnNo > OPT_PNNO_VIRTUAL )
						{
							/* �I�v�V�����p�����[�^�ҏW���t���O���Z�b�g */
							OpPrm->State.Editing = FALSE;
							/* �T�[�{�p�����[�^�C���f�b�N�X�X�V */
							LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
								LcdOptGetNextSvPrmIdx( DEGOP_READ_UP, cur_idx_tmp, &LcdOpt[dispID] );
						}
						/* �I�v�V�����p�����[�^�̈�ȏ�Ƀp�����[�^�����݂��Ȃ����i���[�U���x���j 	*/
						else
						{
							/* �p�����[�^�ǂݏo���v���Z�b�g */
							LcdOptSetOpPrmReadReq( DEGOP_READ_UP, OpPrm->NextPnNo, &LcdOpt[dispID] );
						}
					}
				/*----------------------------------------------------------------------------------*/
				/*	�I�v�V�����p�����[�^�̈���ɂ���ꍇ											*/
				/*----------------------------------------------------------------------------------*/
					else
					{
						/* �p�����[�^�ǂݏo���v���Z�b�g */
						LcdOptSetOpPrmReadReq( DEGOP_READ_UP, OpPrm->NextPnNo, &LcdOpt[dispID] );
					}
				}
			/*--------------------------------------------------------------------------------------*/
			/*	���[�U�p�����[�^�̈�̕ҏW															*/
			/*--------------------------------------------------------------------------------------*/
				else
				{
					/* �I�v�V�����p�����[�^�ҏW���t���O���Z�b�g */
					OpPrm->State.Editing = FALSE;
					/* �T�[�{�p�����[�^�C���f�b�N�X�X�V */
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM������																		*/
/*--------------------------------------------------------------------------------------------------*/
			else if( rc == DEGOP_BUSY )
			{
				;
			}
/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM������																		*/
/*--------------------------------------------------------------------------------------------------*/
			else
			{
				/* �I�v�V�����p�����[�^��ԃt���O���Z�b�g */
				MlibResetByteMemory((void *)&OpPrm->State, sizeof(OpPrm->State));

				/* ���z�I�v�V�����p�����[�^�iPnB00�j��\�������Ȃ����߂̏��u */
				if( PnPrmTbl[idx_tmp].PnNo == OPT_PNNO_VIRTUAL )
				{
					idx_tmp = RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL+1), cur_idx_tmp, &LopMsg);
					if( PnPrmTbl[idx_tmp].PnNo > OPT_PNNO_VIRTUAL )
					{
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
					}
					else
					{
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
							RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), idx_tmp, &LopMsg);
					}
				}
				else
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
#else		//CCSW_CMDID != CMDID_EXIST 
				Numx = CPARAMX->GetNumber( CPARAMX, PnIdx, &LopMsg ) + (0x01 << (LopV.CurDigit*4));
				if( Numx >= 0x1000 ){ Numx = 0xFFF;}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = CPARAMX->SearchB( (COBJECT *)CPARAMX, Numx, PnIdx, &LopMsg );
#endif
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->down )
		{
			if( PnUn == UnSelected )
			{
				if( CUNMONX->GetNumber( CUNMONX, UnIdx, &LopMsg ) < (0x01 << (LopV.CurDigit*4)))
				{
					Numx = 0x000;
				}
				else
				{
					Numx = CUNMONX->GetNumber( CUNMONX, UnIdx, &LopMsg ) - (0x01 << (LopV.CurDigit*4));
				}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].UnIdx = 
						CUNMONX->SearchB( (COBJECT *)CUNMONX, Numx, UnIdx, &LopMsg );
			}
			else if( PnUn == PnSelected )
			{
/*<11J42>*/
#if CCSW_CMDID == CMDID_EXIST
/*--------------------------------------------------------------------------------------------------*/
/*			�������擾																			*/
/*--------------------------------------------------------------------------------------------------*/
			/* ���݃T�[�{�p�����[�^�C���f�b�N�X */
			cur_idx_tmp = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
			/* ���݃T�[�{�p�����[�^�ԍ� */
			cur_num_tmp = PnPrmTbl[cur_idx_tmp].PnNo;
			/* ����T�[�{�p�����[�^�ԍ� */
			if(cur_num_tmp < (0x01 << (LopV.CurDigit*4)))
				Numx = 0x0000;
			else
				Numx = cur_num_tmp - (0x01 << (LopV.CurDigit*4));
			/* ����T�[�{�p�����[�^�C���f�b�N�X */
			idx_tmp = RpxPrmSearchIdxOp(Numx, cur_idx_tmp, &LopMsg);
			OpPrm->NextSvIdx = idx_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM��Ԏ擾																	*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OpiGetDegOpState(&LcdOpt[dispID].PrmRange);

/*--------------------------------------------------------------------------------------------------*/
/*			���ݕҏW���̃p�����[�^�̈�ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
			/* ���[�U�p�����[�^�̈� */
			if( cur_num_tmp < OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedLwrLmt = TRUE;					/* �����l�I�[�o�t���O�Z�b�g		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.LwrLmt;	/* ���݃I�v�V����Prm�ԍ��X�V	*/
			}
			/* �V�X�e���p�����[�^�̈� */
			else if( cur_num_tmp > OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedUprLmt = TRUE;					/* ����l�I�[�o�t���O�Z�b�g		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.UprLmt;	/* ���݃I�v�V����Prm�ԍ��X�V	*/
			}

/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM�L����																		*/
/*--------------------------------------------------------------------------------------------------*/
			if( rc == DEGOP_ENABLE )
			{
			/*--------------------------------------------------------------------------------------*/
			/*	���[�U/�I�v�V�����p�����[�^�̈�̕ҏW												*/
			/*--------------------------------------------------------------------------------------*/
				if( PnPrmTbl[idx_tmp].PnNo <= OPT_PNNO_VIRTUAL )
				{
					/* ����I�v�V�����p�����[�^�ԍ����擾 */
					OpPrm->NextPnNo = LcdOptGetNextOpPrmNo( DEGOP_READ_DOWN, cur_idx_tmp, LopV.CurDigit, OpPrm );

				/*----------------------------------------------------------------------------------*/
				/*	�I�v�V�����p�����[�^�̈�𔲂����ꍇ											*/
				/*----------------------------------------------------------------------------------*/
					if( OpPrm->NextPnNo < LcdOpt[dispID].PrmRange.LwrLmt )
					{
						/* �I�v�V�����p�����[�^�ҏW���t���O���Z�b�g */
						OpPrm->State.Editing = FALSE;
						/* �T�[�{�p�����[�^�C���f�b�N�X�X�V */
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
							LcdOptGetNextSvPrmIdx( DEGOP_READ_DOWN, cur_idx_tmp, &LcdOpt[dispID] );
					}
				/*----------------------------------------------------------------------------------*/
				/*	�I�v�V�����p�����[�^�̈���ɂ���ꍇ											*/
				/*----------------------------------------------------------------------------------*/
					else
					{
						/* �p�����[�^�ǂݏo���v���Z�b�g */
						LcdOptSetOpPrmReadReq( DEGOP_READ_DOWN, OpPrm->NextPnNo, &LcdOpt[dispID] );
					}
				}
			/*--------------------------------------------------------------------------------------*/
			/*	�V�X�e���p�����[�^�̈�̕ҏW														*/
			/*--------------------------------------------------------------------------------------*/
				else
				{
					/* �I�v�V�����p�����[�^�ҏW���t���O���Z�b�g */
					OpPrm->State.Editing = FALSE;
					/* �T�[�{�p�����[�^�C���f�b�N�X�X�V */
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM������																		*/
/*--------------------------------------------------------------------------------------------------*/
			else if( rc == DEGOP_BUSY )
			{
				;
			}
/*--------------------------------------------------------------------------------------------------*/
/*			�f�W�I�y�ʐM������																		*/
/*--------------------------------------------------------------------------------------------------*/
			else
			{
				/* �I�v�V�����p�����[�^��ԃt���O���Z�b�g */
				MlibResetByteMemory((void *)&OpPrm->State, sizeof(OpPrm->State));

				/* ���z�I�v�V�����p�����[�^�iPnB00�j��\�������Ȃ����߂̏��u */
				if( PnPrmTbl[idx_tmp].PnNo == OPT_PNNO_VIRTUAL )
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
						RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), cur_idx_tmp, &LopMsg);
				}
				else
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
#else		//CCSW_CMDID != CMDID_EXIST
				if( CPARAMX->GetNumber( CPARAMX, PnIdx, &LopMsg ) < (0x01 << (LopV.CurDigit*4)) )
				{
					Numx = 0x000;
				}
				else
				{
					Numx = CPARAMX->GetNumber( CPARAMX, PnIdx, &LopMsg ) - (0x01 << (LopV.CurDigit*4));
				}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
						CPARAMX->SearchB( (COBJECT *)CPARAMX, Numx, PnIdx, &LopMsg );
#endif
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shup )
		{
			if(      LopV.CurDigit == (3-1) ){ LopV.EditMode = LCDEM_PnUnSel;}
			else if( LopV.CurDigit <  (3-1) ){ LopV.CurDigit++;}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shdown )
		{
			if(      LopV.CurDigit == 0 ){ LopV.EditMode = LCDEM_PnUnSel;}
			else if( 0 < LopV.CurDigit  ){ LopV.CurDigit--;}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->data )
		{
			if( PnUn == UnSelected )
			{
				LopV.CurDigit = 0;
			}
			else if( PnUn == PnSelected )
			{
				if( CHK_WRITEINHIBIT() && (LopMsg.AccessLvl != ACCLVL_SYSTEM) )
				{
					OpBlnkKickFlg.no_op = 1;
				}
				else
				{
					LopV.EditMode = LCDEM_PnData;
					LopV.CurDigit = 0;
/*<11J42>*/
#if CCSW_CMDID == CMDID_EXIST		
					if( OpPrm->State.Editing == TRUE )
					{							/* �I�v�V�����p�����[�^�ҏW�� */
						if(OpPrm->IsLong)/* �����O�p�����[�^	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 10: 8;
						}
						else						/* ���[�h�p�����[�^	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 5: 4;
						}
					}
					else
					{
#endif
						pPrmDef = GET_PNPRMDEF( CPARAMX->pPrmTbl, PnIdx );		/* �p�����[�^��`	*/
						if( CPARAMX->isLong( CPARAMX, PnIdx, &LopMsg ) )		/* �����O�p�����[�^	*/
						{
							UINT dot = ((CHK_PNPRM_SIGNED( pPrmDef ) 			/* <S18B> */
										&& (GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg )>0)) ? 1 : 0);
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? (10-dot): 8;	/* <S18B> */
						}
						else													/* ���[�h�p�����[�^	*/
						{
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? 5: 4;
						}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
					}
#endif
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->scroll )
		{
			LopV.ActiveLine = (LopV.ActiveLine == 0)? 3 : (--LopV.ActiveLine);
			LopV.CurDigit = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->write )
		{
			if( LcdSaveDispInfoTable( dispID ) == OK )
			{
				pBlnkKickFlg->write = TRUE;
			}
			else
			{
				LopV.EewriteMode = OPWM_InfWrWait;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->rst )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		��ʓ��ɂ��郊�Z�b�g�\���j�^�́A���Z�b�g���{										*/
		/*------------------------------------------------------------------------------------------*/
			for( i = 0; i < 4; i++ )
			{
				if ( LopDsp.InfTbl[dispID][i].PnUn == UnSelected )
				{
					CUNMONX->Clear( (COBJECT *)CUNMONX, LopDsp.InfTbl[dispID][i].UnIdx, &LopMsg );
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );
}

/****************************************************************************************************/
/*																									*/
/*		Pn/Un Select mode																			*/
/*																									*/
/****************************************************************************************************/
/*		dispID: �f�B�X�v���[ID(LCDPMDSP_PM=PRM/MON���, LCDPMDSP_JOG=JOG��� etc.), 				*/
/*		pKeyMk: �L�[�������̃A�h���X, pBlnkKickFlg: �u�����N�N���t���O�|�C���^�[					*/
/*		�߂�l: TRUE=���[�h�L�[���������m���ꂽ���Ƃ�����											*/
/****************************************************************************************************/
USHORT	LcdPnUnSel( USHORT dispID, OPKEYFLG *pKeyMk, BLINKFLG *pBlnkKickFlg )
{
CPRMDEF		*pPrmDef; 														/* PRMDEF	*pPrmDef; */
USHORT	PnUn  = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn;
USHORT	PnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
	DEGOP_PRM	*OpPrm = &LcdOpt[dispID].Prm[LopV.ActiveLine];
#endif
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;										/* TODO: ��ŏ��� */
/*--------------------------------------------------------------------------------------------------*/
/*		Pn/Un Select																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( pKeyMk->mode )
		{
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( (pKeyMk->up) || (pKeyMk->down) )
		{
			if( PnUn == UnSelected )
			{
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn = PnSelected;
			}
			else if( PnUn == PnSelected )
			{
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn = UnSelected;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shup )
		{
			LopV.EditMode = LCDEM_PnUnNum;
			LopV.CurDigit = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shdown )
		{
			LopV.EditMode = LCDEM_PnUnNum;
			LopV.CurDigit = 3-1;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->data )
		{
			if( PnUn == UnSelected )
			{
				LopV.EditMode = LCDEM_PnUnNum;
				LopV.CurDigit = 0;
			}
			else if( PnUn == PnSelected )
			{
				if( CHK_WRITEINHIBIT() /* Iprm.f.PrmWrtInhibit */ && (LopMsg.AccessLvl != ACCLVL_SYSTEM) )
				{
					OpBlnkKickFlg.no_op = 1;
				}
				else
				{
					LopV.EditMode = LCDEM_PnData;
					LopV.CurDigit = 0;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
					if( OpPrm->State.Editing == TRUE )
					{							/* �I�v�V�����p�����[�^�ҏW�� */
						if(OpPrm->IsLong)/* �����O�p�����[�^	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 10: 8;
						}
						else						/* ���[�h�p�����[�^	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 5: 4;
						}
					}
					else
					{
#endif
						pPrmDef = GET_PNPRMDEF( CPARAMX->pPrmTbl, PnIdx ) /* (PRMDEF *)PnPrmTbl[PnIdx].PrmDefPtr */; /* �p�����[�^��`	*/
						if( CPARAMX->isLong( CPARAMX, PnIdx, &LopMsg ) )	/* �����O�p�����[�^	*/
						{
							UINT dot = ((CHK_PNPRM_SIGNED( pPrmDef ) 			/* <S18B> */
										&& (GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg )>0)) ? 1 : 0);
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? (10-dot): 8;	/* <S18B> */
						}
						else								/* ���[�h�p�����[�^	*/
						{
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? 5: 4;
						}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
					}
#endif
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->scroll )
		{
			LopV.EditMode = LCDEM_PnUnNum;
			LopV.ActiveLine = (LopV.ActiveLine == 0)? 3 : (--LopV.ActiveLine);
			LopV.CurDigit = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->write )
		{
			if( LcdSaveDispInfoTable(dispID) == OK )
			{
				pBlnkKickFlg->write = TRUE;
			}
			else
			{
				LopV.EewriteMode = OPWM_InfWrWait;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return(FALSE);
}


/****************************************************************************************************/
/*																									*/
/*		Pn Data Edit mode																			*/
/*																									*/
/****************************************************************************************************/
/*		dispID: �f�B�X�v���[ID(LCDPMDSP_PM=PRM/MON���, LCDPMDSP_JOG=JOG��� etc.)					*/
/*		pKeyMk: �L�[�������̃A�h���X, pBlnkKickFlg: �u�����N�N���t���O�|�C���^�[					*/
/*		�߂�l: TRUE=���[�h�L�[���������m���ꂽ���Ƃ�����											*/
/****************************************************************************************************/
USHORT	LcdPnDataEdit( USHORT dispID, OPKEYFLG *pKeyMk, BLINKFLG *pBlnkKickFlg )
{
LONG	rc;
ULONG	PnData;
USHORT	PnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
	DEGOP_PRM	*OpPrm = &LcdOpt[dispID].Prm[LopV.ActiveLine];
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Pn Data Edit																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(pKeyMk->mode)
		{
			return(TRUE);
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->up)
		{
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
		/* �I�v�V�����p�����[�^�ҏW�� */
			if( OpPrm->State.Editing == TRUE )
			{
				/* �f�[�^�ҏW�i������j */
				PnData = RpiPrmDataDigitInc( OpPrm->Data, LopV.CurDigit,
												LcdOptPrmLmtTbl[OpPrm->Attr.Base][(OpPrm->IsLong<<1)|(OpPrm->Attr.Sign)].UprLmt,
												OpPrm->Attr.Base,
												OpPrm->IsLong,
												OpPrm->Attr.Sign );
				/* �ꎞ�I�v�V�����p�����[�^�������ݗv���Z�b�g */
				rc = OpiReqDegOpPrmWrite( (OpPrm->CurrPnNo|0x1000), OpPrm->Size, PnData );

				if( rc == DEGOP_SUCCESS )
				{
					/* RAM�������ݗv���t���O�Z�b�g */
					LcdOpt[dispID].f.PrmRamWrite = TRUE;
					/* �u�����N�L�b�N�t���O�ւ̃|�C���^�ۑ� */
					LcdOptBlnkKick = pBlnkKickFlg;
					/* �����f�[�^�҂��Z�b�g */
					OpPrm->State.RcvWait = TRUE;
					/* �^�C���A�E�g�p�^�C�}���Z�b�g */
					KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
				}
			}
			else
			{
#endif
				rc = CPARAMX->DigitInc( CPARAMX, PnIdx, LopV.CurDigit, &LopMsg );
				if( rc == PERR_CALC ){ OpBlnkKickFlg.error = 1;}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->down)
		{	/* �f�[�^�ҏW�i�������j */
#if CCSW_CMDID == CMDID_EXIST
			if( OpPrm->State.Editing == TRUE )
			{	/* �I�v�V�����p�����[�^�ҏW�� */
				PnData = RpiPrmDataDigitDec( OpPrm->Data, LopV.CurDigit,
												LcdOptPrmLmtTbl[OpPrm->Attr.Base][(OpPrm->IsLong<<1)|(OpPrm->Attr.Sign)].LwrLmt,
												OpPrm->Attr.Base,
												OpPrm->IsLong,
												OpPrm->Attr.Sign );
				/* �ꎞ�I�v�V�����p�����[�^�������ݗv���Z�b�g */
				rc = OpiReqDegOpPrmWrite( (OpPrm->CurrPnNo|0x1000), OpPrm->Size, PnData );

				if( rc == DEGOP_SUCCESS )
				{
					/* RAM�������ݗv���t���O�Z�b�g */
					LcdOpt[dispID].f.PrmRamWrite = TRUE;
					/* �u�����N�L�b�N�t���O�ւ̃|�C���^�ۑ� */
					LcdOptBlnkKick = pBlnkKickFlg;
					/* �����f�[�^�҂��Z�b�g */
					OpPrm->State.RcvWait = TRUE;
					/* �^�C���A�E�g�p�^�C�}���Z�b�g */
					KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
				}
			}
			else
			{
#endif
				rc = CPARAMX->DigitDec( CPARAMX, PnIdx, LopV.CurDigit, &LopMsg );
				if( rc == PERR_CALC ){ OpBlnkKickFlg.error = 1;}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->shup)
		{
			LopV.CurDigit = (LopV.CurDigit == (LopV.DataDigit-1))? 0 : (++LopV.CurDigit);
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->shdown)
		{
			LopV.CurDigit = (LopV.CurDigit == 0)? (LopV.DataDigit-1) : (--LopV.CurDigit);
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->data)
		{
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			if( OpPrm->State.Editing == TRUE )
			{			/* �I�v�V�����p�����[�^�ҏW�� */
				/* �I�v�V�����p�����[�^�������ݗv���Z�b�g */
				rc = OpiReqDegOpPrmWrite( OpPrm->CurrPnNo, OpPrm->Size, OpPrm->Data );

				if( rc == DEGOP_SUCCESS )
				{
					/* EEPROM�������ݗv���t���O�Z�b�g */
					LcdOpt[dispID].f.PrmEepWrite = TRUE;
					/* �u�����N�L�b�N�t���O�ւ̃|�C���^�ۑ� */
					LcdOptBlnkKick = pBlnkKickFlg;
					/* �����f�[�^�҂��Z�b�g */
					OpPrm->State.RcvWait = TRUE;
					/* �^�C���A�E�g�p�^�C�}���Z�b�g */
					KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
					LopV.EditMode = LCDEM_PnUnNum;
					LopV.CurDigit = 0;
				}
			}
			else
			{
#endif

				rc = PrmWriteValueToEeprom( LopMsg.pAxRsc->RegManager->hPnReg,						  /* <S099> */
						PnPrmTbl[PnIdx].Attribute,
						0 );
				switch( rc )																		  /* <S098> */
				{
				case PRM_RSLT_SUCCESS:			/* ����												*/
					pBlnkKickFlg->PrmEewrite = 1;	/* �u�����N�N��	*/
					LopV.EewriteMode = OPWM_UpdtBlnk;
					break;
				/* ����Queue���Ȃ����߁A�ȉ��R�����g�A�E�g Y.Oka */
//				case PRM_EEPROM_WRITE_FAIL:		/* �܂�Queue���󂢂Ă��Ȃ�							*/
//					LopV.EewriteMode = OPWM_UpdtWait;
//					break;
				case PRM_RSLT_RAMONLY:			/* RAM�݂̂̃p�����[�^								*/
				case PRM_RSLT_NO_DIFF:			/* �������ޕK�v���Ȃ�(EEPROM�l���������ݒl�Ɠ���)	*/
					LopV.EditMode = LCDEM_PnUnNum;
					LopV.CurDigit = 0;
					break;
				default:
					break;
				}

#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->scroll)
		{
			/* �������Ȃ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->write)
		{
			if(LcdSaveDispInfoTable(dispID) == OK)
			{
				pBlnkKickFlg->write = TRUE;
			}
			else
			{
				LopV.EewriteMode = OPWM_InfWrWait;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return(FALSE);
}


/****************************************************************************************************/
/*																									*/
/*		Make Display Data for Pn/Un Lines															*/
/*																									*/
/****************************************************************************************************/
void	LcdMkDspDataPnUnLines(				/* Make Display Data for Pn/Un Lines					*/
		USHORT	dispID,						/* �f�B�X�v���[ID(LCDPMDSP_PM/JOG/etc)					*/
		USHORT	BlnkSupFlg,					/* �u�����N�}�~�t���O									*/
		UCHAR	*ResBuf			)			/* �����o�b�t�@�A�h���X									*/
{
USHORT	i, Idx;
ULONG	Ldata;
CPRMDEF	*pPrmDef;
CMONDEF	*pMonDef; 							/* MONDEF	*pMonDef; */
LOPDIT	*pInf = (LOPDIT *)(&(LopDsp.InfTbl[dispID][0]));
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
USHORT j;
DEGOP_PRM *optprm;
DEGOP_PRM *act_optprm = &LcdOpt[dispID].Prm[LopV.ActiveLine];

		if( act_optprm->State.RcvWait == TRUE )
		{			/* �I�v�V��������̉����f�[�^�҂��̎��͎�M���� */
			LcdOptUpdateResponseData(dispID, &LcdOpt[dispID]);
		}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data for Pn/Un Lines															*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < 4; i++)
		{
			if(pInf[i].PnUn == UnSelected)
			{
				Idx = pInf[i].UnIdx;
				pMonDef = GET_UNMONDEF( CUNMONX->pUnMonTbl, Idx );
				RpxLcdSetStr((USHORT)(i+1), 0, (UCHAR *)"Un", ResBuf);
				RpxLcdSetValHex((USHORT)(i+1), 2, (ULONG) CUNMONX->GetNumber( CUNMONX, Idx, &LopMsg ), 3, ResBuf);
				RpxLcdSetStr((USHORT)(i+1), 5, (UCHAR *)"=", ResBuf);
				CUNMONX->Read( (COBJECT *)CUNMONX, Idx, &Ldata, MCHK_ACCS_LVL, &LopMsg );   /* Ldata <-- ���j�^�l	*/ /* <S073> */
				RpxLcdSetPnUnData((USHORT)(i+1), 
					6, 
					Ldata, 
					CUNMONX->isLong( CUNMONX, Idx, &LopMsg),	
					CHK_UNBASE( pMonDef ),
					pMonDef->Sign, 
					CHK_UNDECIMALS( pMonDef ),
					ResBuf);
			}
			else if(pInf[i].PnUn == PnSelected)
			{
#if CCSW_CMDID == CMDID_EXIST
				optprm = &LcdOpt[dispID].Prm[i];
				/* �I�v�V�����p�����[�^�ҏW���̎� */
				if( optprm->State.Editing == TRUE )
				{
					/* �����`�F�b�N���Z�b�g */
					optprm->IsLong = (optprm->Size == 4)? TRUE : FALSE;
					if( optprm->Attr.Base == DSPDEF_BASEBIT )	/* ����A�r�b�g�\���̃T�|�[�g�Ȃ� */
					{ optprm->Attr.Base = DSPDEF_BASEHEX; }		/* �����I��16�i�\�� */
					optprm->Attr.Decimals = 0;					/* ����A�����_�\���̃T�|�[�g�Ȃ� */

					/* �e�s�ɂĕҏW����Pn�ԍ����A�N�e�B�u�s�̂���Ɠ����ꍇ�̓A�N�e�B�u�s��Pn�ԍ��̒l�ɓ��� */
					if( i != LopV.ActiveLine )
					{
						if( optprm->CurrPnNo == act_optprm->CurrPnNo )
						{
							optprm->Data = act_optprm->Data;
						}
					}

					/* �e��ʂɂĕҏW����Pn�ԍ����A�N�e�B�u�s�̂���Ɠ����ꍇ�̓A�N�e�B�u�s��Pn�ԍ��̒l�ɓ��� */
					for(j = 0; j < LCDPMDSP_MAX; j++)
					{
						if( j != dispID )
						{
							if( LcdOpt[j].Prm[i].CurrPnNo == act_optprm->CurrPnNo )
							{
								LcdOpt[j].Prm[i].Data = act_optprm->Data;
							}
						}
					}

					/* �����f�[�^�\�� */
					RpxLcdSetStr((USHORT)(i+1), 0, (UCHAR *)"Pn", ResBuf);
					RpxLcdSetValHex((USHORT)(i+1), 2, (ULONG)optprm->CurrPnNo, 3, ResBuf);
					RpxLcdSetStr((USHORT)(i+1), 5, (UCHAR *)"=", ResBuf);
					RpxLcdSetPnUnData( (USHORT)(i+1), 6, optprm->Data,
										(USHORT)optprm->IsLong, (USHORT)optprm->Attr.Base,
										(USHORT)optprm->Attr.Sign, optprm->Attr.Decimals, ResBuf);
				}
				else
				{
#endif
					Idx = pInf[i].PnIdx;
					pPrmDef = GET_PNPRMDEF( CPARAMX->pPrmTbl, Idx );
					RpxLcdSetStr((USHORT)(i+1), 0, (UCHAR *)"Pn", ResBuf);
					RpxLcdSetValHex((USHORT)(i+1), 2, CPARAMX->GetNumber( CPARAMX, Idx, &LopMsg ), 3, ResBuf);
					RpxLcdSetStr((USHORT)(i+1), 5, (UCHAR *)"=", ResBuf);
					CPARAMX->Read( (COBJECT *)CPARAMX, Idx, &Ldata, PCHK_OPE_READ, &LopMsg );	/* Ldata <-- �p�����[�^�l	*/  /* <S073> */
					RpxLcdSetPnUnData( 
						(USHORT)(i+1), 
						6, 
						Ldata,
						CPARAMX->isLong( CPARAMX, Idx, &LopMsg ),
						GET_PNPRMBASE( pPrmDef ),
						CHK_PNPRM_SIGNED( pPrmDef ), 
						/*GET_PNPRM_DECIMALS( pPrmDef ), */
						GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg ),	/* <S14F> */ 
						ResBuf);
				}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set Blink in the active line																*/
/*--------------------------------------------------------------------------------------------------*/
		if(!BlnkSupFlg)
		{
			switch(LopV.EditMode)
			{
			case LCDEM_PnUnNum:
				RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1), (USHORT)(2+3-1 - LopV.CurDigit), 1, ResBuf);
				break;
			case LCDEM_PnUnSel:
				RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1), 0, 2, ResBuf);
				break;
			case LCDEM_PnData:
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
				if( act_optprm->State.Editing == TRUE )
				{	/* �I�v�V�����p�����[�^�ҏW���̎� */
					RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1),
								  (USHORT)(2+3+1 + LcdOptSetDataBlinkDPos(act_optprm)), 1, ResBuf);
				}
				else
				{
#endif
					Idx = pInf[LopV.ActiveLine].PnIdx;
					pPrmDef =GET_PNPRMDEF( CPARAMX->pPrmTbl, Idx ); /* (PRMDEF *)PnPrmTbl[Idx].PrmDefPtr; */
					RpxLcdSetBlnk( (USHORT)(LopV.ActiveLine+1),
							   (USHORT)(2+3+1 + LcdPrmDataBlinkDpos(pPrmDef)), 1, ResBuf );
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
				}
#endif
				break;
			default:
				break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Reset Pn/UN Display Data 																	*/
/*																									*/
/****************************************************************************************************/
void	LcdRstPnUnDspData( void )
{
		LopV.ActiveLine = 0;
		LopV.EditMode = LCDEM_PnUnNum;
		LopV.EewriteMode = 0 /* OPWM_None */;
		LopV.CurDigit = 0;
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Parameter Blink Data position 													*/
/*																									*/
/****************************************************************************************************/
USHORT	LcdPrmDataBlinkDpos( CPRMDEF *pPrmDef )
{
USHORT	Dpos;
USHORT	Decimals;

/*--------------------------------------------------------------------------------------------------*/
/*		Set Dpos, Decimals																			*/
/*--------------------------------------------------------------------------------------------------*/
		Dpos = LopV.DataDigit - 1 - LopV.CurDigit;			/* "H."����я����_�𖢍l���̈ʒu		*/
		/*Decimals = GET_PNPRM_DECIMALS( pPrmDef );*/		/* �����_�ʒu�̎擾						*/
		Decimals = GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg );		 /* <S14F> �����_�ʒu�̎擾	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Modify Dpos by Sign,Decimals,etc															*/
/*--------------------------------------------------------------------------------------------------*/
		if( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC )	/* �ꂪ10�i	*/
		{
			if( CHK_PNPRM_SIGNED( pPrmDef )  == DSPDEF_W_SIGN )
			{
				Dpos++;	/* ������	*/
			}
			if( (Decimals > 0) && (LopV.CurDigit < (USHORT)Decimals) )
			{
				Dpos++;	/* �����_��	*/
			}
		}
		else												/* �ꂪ10�i�ȊO	*/
		{
			Dpos += 2;	/* "n." or "H." �̕�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( Dpos );
}

