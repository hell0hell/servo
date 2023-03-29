/*****************************************************************************
Description		: �A���[�����b�`�@�\����
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
		AlmLatchMonitorInitial		�A���[�����b�`����������
		AlmLatchMonitorSequence		�A���[�����b�`����

	-- Locals --
		AlmLatchMonitorData			�A���[�����b�`���j�^���f�[�^�쐬
		AlmLatchMonDataWrite		�A���[�����b�`���j�^�A EEPROM�֊i�[���܂�

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.10.11		created

*****************************************************************************/

#include "AlmLatch.h"
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
#include "Sqi.h"
#endif
#define	MONSETBIT( sig, bit )	((sig)? (1<<bit) : 0x0000)
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
#define LPX_CHECK_ON( psqi, sig )  (((psqi)->Sigs & (sig)))
#endif
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
extern SQIV	SqiV;
#endif
/****************************************************************************************************/
/*																									*/
/*		API�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
void AlmLatchMonitorInitial( ALMLATCH_HNDL *hAlmLatchHndl );
void AlmLatchMonitorSequence( ALMLATCH_HNDL *hAlmLatchHndl );

/****************************************************************************************************/
/*																									*/
/*		Local�֐��ꗗ																				*/
/*																									*/
/****************************************************************************************************/
static void AlmLatchMonitorData( ALMLATCH_HNDL *hAlmLatchHndl );
//static void AlmLatchMonDataWrite( ALMLTMON *hAlmMonitor, ALMMAN_HANDLE *hAlmHndl );



/*****************************************************************************
  Description: �A���[�����b�`����������
------------------------------------------------------------------------------
  Parameters:
	hAlmLatchHndl			Alarm Latch handle

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
void AlmLatchMonitorInitial( ALMLATCH_HNDL *hAlmLatchHndl )
{
	MlibResetLongMemory( &(hAlmLatchHndl->var), sizeof( hAlmLatchHndl->var )/4 );
	
	return ;
}



/*****************************************************************************
  Description: �A���[�����b�`����
------------------------------------------------------------------------------
  Parameters:
	hAlmLatchHndl			Alarm Latch handle

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
void AlmLatchMonitorSequence( ALMLATCH_HNDL *hAlmLatchHndl )
{

/*--------------------------------------------------------------------------------------------------*/
/*		���H����d�����t�u���x����艺��������A�f�[�^�X�V�������ݗv���͂��Ȃ�					*/
/*--------------------------------------------------------------------------------------------------*/
	if ( hAlmLatchHndl->PowerManager->PowerSts.DcVolt >=
			hAlmLatchHndl->PowerManager->MainPowerChecker.conf.UvLevel )
	{
#if 0	/* <S212> */
		/*----------------------------------------------------------------------------------*/
		/*		�A���[�������o�̏ꍇ�A�f�[�^�X�V											*/
		/*----------------------------------------------------------------------------------*/
		if ( hAlmLatchHndl->AlmManager->Status.AlmFlag == FALSE )
		{
			AlmLatchMonitorData( hAlmLatchHndl );

		}
		/*----------------------------------------------------------------------------------*/
		/*		�O��A���[���Ȃ�������A���[�����o�ŁAEEPROM�����ݑ҂��s��ւ̓o�^			*/
		/*----------------------------------------------------------------------------------*/
		else if ( hAlmLatchHndl->var.LastAlmStatus == FALSE )
		{
#else	/* <S212> */
		if( hAlmLatchHndl->AlmManager->LtAlmReq == 4 )
		{
			hAlmLatchHndl->var.LastAlmStatus    = FALSE;
			hAlmLatchHndl->AlmManager->LtAlmReq = 0;
		}
		if( hAlmLatchHndl->var.LastAlmStatus == FALSE )
		{
			hAlmLatchHndl->var.LastAlmStatus = TRUE;

			AlmLatchMonitorData( hAlmLatchHndl );
#endif	/* <S212> */
			/*--------------------------------------------------------------------------*/
			/*		0x833 : �A���[���ԍ� [�|]											*/
			/*		0x834 : �^�C���X�^���v [�|]											*/
			/*--------------------------------------------------------------------------*/
#if 0	/* <S1CF>  ���b�`�f�[�^���������ށB�������A�������ݒ��͖�������B	*/
			hAlmLatchHndl->AlmLtMon.AlmCode = ALMGetAlarmHistory( hAlmLatchHndl->AlmManager, 0,
										&hAlmLatchHndl->AlmLtMon.TmStamp );
//			AlmLatchMonDataWrite( &hAlmLatchHndl->AlmLtMon, hAlmLatchHndl->AlmManager );	  /* <S194> */

			/* ���j�^�R�s�[ */
			MlibCopyLongMemory( &(hAlmLatchHndl->LatchMon), &(hAlmLatchHndl->AlmLtMon),
								(sizeof(ALMLTMON) >> 2) );
#else
			if( hAlmLatchHndl->AlmManager->LtAlmReq == 0 )
			{
				/* ���j�^�R�s�[ */
				MlibCopyLongMemory( &(hAlmLatchHndl->LatchMon), &(hAlmLatchHndl->AlmLtMon),
																	(sizeof( ALMLTMON ) >> 2) );

				/* �������ݗv�� */
				hAlmLatchHndl->AlmManager->LtRomAdr = EEP_NOSUM_ADDRESS( hAlmLatchHndl->AlmManager->myAsicNo, EEP_ALMMON_OFS );
				hAlmLatchHndl->AlmManager->LtMemAdr = (USHORT*)&( hAlmLatchHndl->LatchMon );
				hAlmLatchHndl->AlmManager->AlmCdAdr = (USHORT*)&( hAlmLatchHndl->LatchMon.AlmCode );
				hAlmLatchHndl->AlmManager->TmStpAdr = (ULONG *)&( hAlmLatchHndl->LatchMon.TmStamp );
				hAlmLatchHndl->AlmManager->LtDtWtCt = sizeof( ALMLTMON ) >> 1;
				hAlmLatchHndl->AlmManager->LtAlmReq = 1;
			}
#endif
		}
		else
		{
			/* Do Nothing */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[����ԕۑ�																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S212> */
	if ( hAlmLatchHndl->AlmManager->Status.AlmFlag == TRUE )
	{
		hAlmLatchHndl->var.LastAlmStatus = TRUE;
	}
	else
	{
		hAlmLatchHndl->var.LastAlmStatus = FALSE;
	}
#endif
	return;
}

/*****************************************************************************
  Description: �A���[�����b�`���j�^���f�[�^�쐬
------------------------------------------------------------------------------
  Parameters:
	hAlmLtUnitHndl		Alarm Latch Unit Handle

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
static void AlmLatchMonitorData( ALMLATCH_HNDL *hAlmLatchHndl )
{
	LONG	lwk;
	LONG	val;
	ALMLTMON	*pAlmLtMon;
	PLSIFV		*pPls;
	
	pAlmLtMon	= &hAlmLatchHndl->AlmLtMon;
	pPls		= hAlmLatchHndl->pPlsIfV;
/*--------------------------------------------------------------------------------------------------*/
/*		0x800 : ���[�^��]/�ړ����x [min-1, mm/s]													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1CF> */
	pAlmLtMon->MotSpd =
			(SHORT)MlibLimitul(hAlmLatchHndl->UnMonitor->UnMotSpdx, 0x00007FFF, 0xFFFF8000);
#else
	pAlmLtMon->MotSpd = hAlmLatchHndl->UnMonitor->UnMotSpd;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		0x801 : ���x�w�� [min-1, mm/s]																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1CF> */
	val = MlibMulgainSx48( hAlmLatchHndl->UnMonitor->UnSpdRef, hAlmLatchHndl->Bprm->Kmonspd );
	pAlmLtMon->SpdRef = (SHORT)MlibLimitul(val, 0x00007FFF, 0xFFFF8000) ;
#else
	val = MlibMulgainSx48( hAlmLatchHndl->BaseControls->CtrlCmdMngr.SpdRefo, hAlmLatchHndl->Bprm->Kmonspd);
	pAlmLtMon->SpdRef = MlibLimitul(val, 0x00007FFF, 0xFFFF8000);
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		0x802 : �����g���N/���͎w�� [%]																*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->TrqRef = (SHORT)( hAlmLatchHndl->UnMonitor->UnTrqRef );

/*--------------------------------------------------------------------------------------------------*/
/*		0x803 : ���͎w�߃p���X���x [min-1, mm/s]													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1CF> */
	val = MlibMulgain( hAlmLatchHndl->SeqMotSts->var.dPcmdCnt,
							hAlmLatchHndl->Bprm->Kmonpcmdspd2ms );
	pAlmLtMon->PcmdSpd = (SHORT)MlibLimitul(val, 0x00007FFF, 0xFFFF8000) ;
#else
	pAlmLtMon->PcmdSpd = hAlmLatchHndl->UnMonitor->UnPcmd64ms;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		0x804 : �΍��J�E���^(�ʒu�΍���) [pulse]													*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->PosErr = hAlmLatchHndl->BaseLoops->BaseCtrls->PosManager.var.PosErrA.per32s;

/*--------------------------------------------------------------------------------------------------*/
/*		0x810 : �����M�����j�^ [�|]																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->InterSignal = (
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->MainPowerOn,	0	) |			/* ���HON��� */
								MONSETBIT( hAlmLatchHndl->MencV->EncReady,					1	) |	/* Encoder Ready */
								MONSETBIT( hAlmLatchHndl->BeSequence->DbSeqData.DbBeReq,	2	) |	/* ���[�^�ʓd(�v��) */
								MONSETBIT( hAlmLatchHndl->BeSequence->DbOn,					3	) |	/* DB ON */
								MONSETBIT( hAlmLatchHndl->Bprm->RvsDir,						4	) |	/* �t��]���[�h */
								MONSETBIT( hAlmLatchHndl->BaseControls->ModeSwData.var.ModeSWFlag,	5	) |	/* ���[�h�X�C�b�`ON Flag	 */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.Vrefon,				6	) |	/* Speed  Reference Input On */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.Trefon,				7	) |	/* Torque Reference Input On */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.Prefon,				8	) |	/* Torque Reference Input On */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.PrefSign,			9	) |	/* Pulse  Reference Sign  (plus:1, minus:0) */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.PerrClr,			10	) |	/* PosErr Clear Input On */
								MONSETBIT( 0,							11	) |	/*MONSETBIT( BoutV.f.BiasONFlag,	11	)�A0�Œ� */
								MONSETBIT( hAlmLatchHndl->PowerManager->PowerSts.Acon,		12	) |	/* AC�d�����o��� */
								MONSETBIT( hAlmLatchHndl->PowerManager->PowerSts.ChargeOk,	13	) |	/* DC�[�d�������*/
								MONSETBIT( HALdrv_GetRegeneControlState(),					14	) |	/* ��Tr�w�߃I�� */
								MONSETBIT( hAlmLatchHndl->PowerManager->RegeneCtrl.RegChk.RegeAlmSignal,	15	) |	/* �񐶈ُ팟�o�I�� */
								MONSETBIT( hAlmLatchHndl->BeSequence->BeReqSeqOut,			16	) |	/* BaseEnable Request(�V�[�P���X�����o��) */
								MONSETBIT( 0,												17	) |
								MONSETBIT( ALMCheckEachState(hAlmLatchHndl->AlmManager, ALM_OC),			18	) |	/* �ߓd�� */
								MONSETBIT((hAlmLatchHndl->MencV->RxAlmCode & 0x0040),		19	) |	/* ���_���ʉߊ��� */
								MONSETBIT( hAlmLatchHndl->BaseControls->JatHdl.jatv.enable,	20	) |	/* �C�i�[�V�����蒆 */
								MONSETBIT( (hAlmLatchHndl->BeSequence->BeReqSeqOut != FALSE)
										&& (hAlmLatchHndl->MencV->PhaseReady == FALSE),		21	) |	/* ���Ɍ��o�� */
								MONSETBIT( hAlmLatchHndl->MencV->PhaseReady,				22	) 	/* ���ɔF������ */
								);

/*--------------------------------------------------------------------------------------------------*/
/*		0x814 : �������͐M�����j�^ [�|]																*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_READ_PCMD_STATUS( lwk );
	pAlmLtMon->InSignal = (
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & SERVO_ON_BIT)),	0	) |	/* �T�[�{�I�� */
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & ENBPCTRL_BIT)),	1	) |	/* P/PI�؂�ւ� */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->PotSigSts,							2	) |	/* ���]�쓮�֎~��� */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->NotSigSts,							3	) |	/* �t�]�쓮�֎~��� */
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmRst,								4	) |	/* ALM���Z�b�g�v�� */
							MONSETBIT( hAlmLatchHndl->BaseControls->TrqLimitData.var.PclSignal ,		5	) |	/* PCL Signal */
							MONSETBIT( hAlmLatchHndl->BaseControls->TrqLimitData.var.NclSignal,			6	) |	/* PCL Signal */
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & SENSOR_ON_BIT)),	7	) |	/* �Z���T�I��	*/
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							/* �����ݒ葬�x���� */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_SPDD ),		8	) |	/* SPD-D Signal */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_SPDA ),		9	) |	/* SPD-A Signal */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_SPDB ),		10	) |	/* SPD-B Signal */
							/* ��������؂�ւ� */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_CSEL ),		11	) |	/* C-SEL Signal */
#else
							MONSETBIT( 0,		8	) |
							MONSETBIT( 0,		9	) |
							MONSETBIT( 0,		10	) |
							MONSETBIT( 0,		11	) |
#endif
							MONSETBIT( hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & ZEROCLAMP_BIT,	12	) |	/* Zero Clamp */
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_INHIBIT ),					13	) |	/* INHIBIT Signal */
#else
							MONSETBIT( 0,		13	) |
#endif
							MONSETBIT( hAlmLatchHndl->BaseControls->GselNum & 0x03,				14	) |	/* �}�j���A���Q�C���ؑ֗p�Q�C���ԍ�(bit14,15) */
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & POL_DETECT_BIT)),		16	) |	/* P-DET Signal */
							MONSETBIT( hAlmLatchHndl->IoSignals->EstpSignal.SignalStatus.v,		17	) |	/* E-Stop Signal *//* <S045> */
							MONSETBIT( hAlmLatchHndl->IoSignals->DecSignal.SignalStatus.v,		18	) |	/* E-Stop Signal *//* <S045> */
							/* �O�����b�`�������͏��(Ext1,Ext2,Ext3) */
							MONSETBIT( hAlmLatchHndl->IoSignals->ExtSignal[0].SignalStatus.v,	19	) | /* <S045> */
							MONSETBIT( hAlmLatchHndl->IoSignals->ExtSignal[1].SignalStatus.v,	20	) | /* <S045> */
							MONSETBIT( hAlmLatchHndl->IoSignals->ExtSignal[2].SignalStatus.v,	21	) | /* <S045> */

							MONSETBIT( KPX_CHECK_PCMD_CADAT( lwk ),	24	) |
							MONSETBIT( KPX_CHECK_PCMD_CBDAT( lwk ),	25	) |
							MONSETBIT( hAlmLatchHndl->BaseControls->CtrlCmdMngr.PerrClrReq,		26	) |	/* �ʒu�΍��N���A�v��(�e�[�u���^�]/PJOG��p) */
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_PSEL ),					27	)	/* �w�߃p���X���͐ؑ�(/PSEL) */
#else
							MONSETBIT( 0,		27	)
#endif
							);

/*--------------------------------------------------------------------------------------------------*/
/*		0x818 : �����o�͐M�����j�^ [�|]																*/
/*--------------------------------------------------------------------------------------------------*/
	val = 0;
	if(hAlmLatchHndl->Bprm->FencUse != FALSE)
	{
		val = hAlmLatchHndl->FencV->CposOk & (hAlmLatchHndl->FencV->MposFromCpos == 0);
	}
	else
	{
		val = hAlmLatchHndl->MencV->CposOk & (hAlmLatchHndl->MencV->MposFromCpos == 0);
	}
	
	pAlmLtMon->OutSignal = (
							MONSETBIT( hAlmLatchHndl->AlmManager->Status.AlmFlag,			0	) |	/* �A���[�������� */
							MONSETBIT( hAlmLatchHndl->BaseControls->PosCtrlSts.CoinSignal,	1	) |	/* �ʒu���ߊ��� */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->VcmpSts,					2	) |	/* ���x��v */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->TgOnSts,					3	) |	/* ���[�^��]�� */
							MONSETBIT( hAlmLatchHndl->BeSequence->ServoReady,				4	) |/* �T�[�{���f�B */
							/* �g���N������ */
							MONSETBIT( ((hAlmLatchHndl->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO) & 1) ,	5	) |
							/* ���x������ */
							MONSETBIT( ((hAlmLatchHndl->SvControlIf->RspCtrl.RspCtrlBit >> SPD_CLAMP_BITNO) & 1),	6	) |
							MONSETBIT( (!(hAlmLatchHndl->SeqCtrlOut->BrkSigSts)),			7	) |	/* �u���[�L������ */
							MONSETBIT( hAlmLatchHndl->AlmManager->Status.WrnFlag,			8	) |	/* ���[�j���O������ */
							MONSETBIT( hAlmLatchHndl->BaseControls->PosCtrlSts.NearSignal,	9	) |	/* Near Signal */
							MONSETBIT( val,													10	) |	/* PgCphase Signal */
							/* �A���[���R�[�h�M���o��	*/
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmCodeOut.Alo1,			16	) |
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmCodeOut.Alo2,			17	) |
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmCodeOut.Alo3,			18	) |
						/*	((BinK.DivPulseSig & 0x07)			 << 19  ) |		*//*�����p���X�o�͖��Ή�*/
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_PSEL ),				22	)	/* �w�߃p���X���͐ؑ։���(/PSELA ) */
#else
							MONSETBIT( 0 /*BoutV.f.Sv.PselA */,			22	)
#endif
							);

/*--------------------------------------------------------------------------------------------------*/
/*		0x820 : ���H����d�� [V]																	*/
/*--------------------------------------------------------------------------------------------------*/
#if ( FLOAT_USE == TRUE )
	pAlmLtMon->DcVoltage = hAlmLatchHndl->PowerManager->PowerSts.DcVolt *
							hAlmLatchHndl->PowerManager->DetectDcVolt.conf.Kdcvolt;
#else
	pAlmLtMon->DcVoltage = MlibMulgain(hAlmLatchHndl->PowerManager->PowerSts.DcVolt,
								hAlmLatchHndl->PowerManager->DetectDcVolt.conf.Kdcvolt );
#endif /* FLOAT_USE */
	
/*--------------------------------------------------------------------------------------------------*/
/*		0x821 : �d�����o�l [%]																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	pAlmLtMon->IFBMon = hAlmLatchHndl->UnMonitor->UnCrntDet *
						hAlmLatchHndl->Bprm->Kmoncrnt;
#else
	pAlmLtMon->IFBMon =
			MlibMulgain( hAlmLatchHndl->UnMonitor->UnCrntDet, hAlmLatchHndl->Bprm->Kmoncrnt );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		0x822 : �ݐϕ��ח� [%(����2ms)]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	pAlmLtMon->MotLoad = hAlmLatchHndl->UnMonitor->UnCrntRef *
						hAlmLatchHndl->Bprm->Kmoncrnt;
#else
	pAlmLtMon->MotLoad =
			MlibMulgain( hAlmLatchHndl->UnMonitor->UnCrntRef, hAlmLatchHndl->Bprm->Kmoncrnt );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		0x823 : �񐶕��ח� [10%(����2ms)]															*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->RgPower = hAlmLatchHndl->UnMonitor->UnRgPower;

/*--------------------------------------------------------------------------------------------------*/
/*		0x824 : �c�a��R����d�� [%(����2ms)]														*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->DbPower = hAlmLatchHndl->UnMonitor->UnDbPower;

/*--------------------------------------------------------------------------------------------------*/
/*		0x825 : �ő�ݐϕ��ח� [%]																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->MaxMotLoad10s = (USHORT)hAlmLatchHndl->UnMonitor->UnIsqrSumMax ;

/*--------------------------------------------------------------------------------------------------*/
/*		0x826 : �������[�����g��/�d�ʔ� [%]															*/
/*--------------------------------------------------------------------------------------------------*/
	/*	����Ŏg�p���Ă��銵�����ۑ�	*/
//	pAlmLtMon->JLRatio = hAlmLatchHndl->BaseControls->JatHdl.EstimatJrat;
	if( hAlmLatchHndl->BaseControls->JatHdl.jatv.enable == TRUE )		/* �C�i�[�V������L��		*/
	{
		pAlmLtMon->JLRatio = hAlmLatchHndl->BaseControls->JatHdl.EstimatJrat;
	}
	else
	{
		pAlmLtMon->JLRatio = hAlmLatchHndl->UniPrm->Prm->jrate;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		0x830 : �V���A���G���R�[�_�ʐM�ُ�� [�|]													*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->EncComErrCnt = hAlmLatchHndl->MencV->ComErrCnt;

/*--------------------------------------------------------------------------------------------------*/
/*		0x832 : �Z�[�t�e�B���o�͐M�����j�^ [�|]														*/
/*--------------------------------------------------------------------------------------------------*/
	/* �Z�[�t�e�B�I�v�V�����J�[�h�����̎� */
	if( hAlmLatchHndl->SftOpt->conf.OpInfo.f.Connect == TRUE )
	{
		/* �Z�[�t�e�B�I�v�V�����J�[�h�L��̎� */
		pAlmLtMon->SafetyIOSignal = (
									MONSETBIT( FALSE,					0	) |
									MONSETBIT( FALSE,					1	) |
									MONSETBIT( FALSE,					2	) |
									MONSETBIT( FALSE,					3	) |
									MONSETBIT( FALSE,					4	) |
									MONSETBIT( FALSE,					5	) );
	}
	else
	{
		/* �Z�[�t�e�B�I�v�V�����J�[�h�����̎� */
		pAlmLtMon->SafetyIOSignal = (
									MONSETBIT( hAlmLatchHndl->IoSignals->DetHwbbSts.Bbon1,	0	) |
									MONSETBIT( hAlmLatchHndl->IoSignals->DetHwbbSts.Bbon2,	1	) |
									MONSETBIT( FALSE,					2	) |
									MONSETBIT( FALSE,					3	) |
									MONSETBIT( FALSE,					4	) |
									MONSETBIT( FALSE,					5	) );
	}

	return;
}

#if 0
/*****************************************************************************
  Description: �A���[�����b�`���j�^�A EEPROM�֊i�[���܂�
------------------------------------------------------------------------------
  Parameters:
	hAlmMonitor			�A���[�����b�`�p���j�^�̃|�C���^
	hAlmHndl			�A���[���p�f�[�^�̃|�C���^

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
static void AlmLatchMonDataWrite( ALMLTMON *hAlmMonitor, ALMMAN_HANDLE *hAlmHndl )
{
	ULONG *pMonitorVal;
	ULONG MonitorVal;
	ULONG Offset;
	USHORT MonitorSize;		/* �A���[�����b�`���j�^�̃T�C�Y */
	USHORT i;

	pMonitorVal = (ULONG *)(hAlmMonitor);
	/* �A���[�����b�`���j�^�̃T�C�Y�l���v�Z���� */
	MonitorSize = EEPSIZE_ALMMON >> 2 ;

	for(i = 0U; i < MonitorSize; i++)
	{
		/* Write Alarm Latch Monitor to EEPROM */
		Offset = EEP_NOSUM_ADDRESS( hAlmHndl->myAsicNo, EEP_ALMMON_OFS ) + (i << 2);
		MonitorVal = *(pMonitorVal + i);	/* �A���[�����b�`���j�^�l���擾���� */
		EepdevPutQueue(&hAlmHndl->EepQue, 0, Offset, MonitorVal, 4U );
	}

	return ;
}
#endif



/***************************************** end of file **********************************************/
